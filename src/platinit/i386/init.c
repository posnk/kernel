/******************************************************************************\
Copyright (C) 2015 Peter Bosch

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
\******************************************************************************/

/**
 * @file platinit/i386/init.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "core/physmm.h"
#include "arch/i386/multiboot.h"
#include "core/paging.h"
#include "platinit/paging.h"
#include "modlink/modlink.h"

extern void *i386_end_kernel;
extern void *i386_start_kernel;

physaddr_t initial_pagedir;

char *mb_mmap_types[6] = {
	"TYPE0",
	"RAM",
	"RESERVED",
	"ACPI_RECLAIM",
	"ACPI_NVS",
	"BADRAM"
};



/**
 * C entry point, called from start.s
 */
void i386_init( multiboot_info_t *mb_info, uint32_t mb_magic )
{
	STPO;
	char *str;
	int idx;
	int mmap_s = 0;
	uint32_t sz;
	physaddr_t start, end;
	multiboot_memory_map_t 	*mmap;
	multiboot_module_t	*mods;

	printf("posldr %s by Peter Bosch (c) 2016\n", ARCH_STRING);
	
	printf("Multiboot magic number: 0x%08x\n", mb_magic );
	printf("mb_info[0x%08p]\n", mb_info);
	printf("|-flags:         0x%08x\n", mb_info->flags);
	printf("|-mem_lower:     0x%08x\n", mb_info->mem_lower);
	printf("|-mem_upper:     0x%08x\n", mb_info->mem_upper);
	printf("|-boot_device:   0x%08x\n", mb_info->boot_device);
	printf("|-cmdline:       0x%08x\n", mb_info->cmdline);
	printf("|-mods_count:    0x%08x\n", mb_info->mods_count);
	printf("|-mods_addr:     0x%08x\n", mb_info->mods_addr);
	printf("|-mmap_length:   0x%08x\n", mb_info->mmap_length);
	printf("|-mmap_addr:     0x%08x\n", mb_info->mmap_addr);
	printf("|-drives_length: 0x%08x\n", mb_info->drives_length);
	printf("|-drives_addr:   0x%08x\n", mb_info->drives_addr);
	printf("|-config_table:  0x%08x\n", mb_info->config_table);
	printf("|-bootldr_name:  0x%08x\n", mb_info->boot_loader_name);
	printf("|-apm_table:     0x%08x\n", mb_info->apm_table);
	printf("|-vbe_ctrl_info: 0x%08x\n", mb_info->vbe_control_info);
	printf("|-vbe_mode_info: 0x%08x\n", mb_info->vbe_mode_info);
	printf("|-vbe_mode:      0x%08x\n", mb_info->vbe_mode);
	printf("|-vbe_if_seg:    0x%08x\n", mb_info->vbe_interface_seg);
	printf("|-vbe_if_off:    0x%08x\n", mb_info->vbe_interface_off);
	printf("\\-vbe_if_len:    0x%08x\n", mb_info->vbe_interface_len);
	
	mmap = ( multiboot_memory_map_t *) mb_info->mmap_addr;
	mods = ( multiboot_module_t *) mb_info->mods_addr;

	printf("physmm: initializing...\n");
	physmm_init();

	printf("bootstrap: multiboot memory map:\n");
	
	do {
		
		printf(	"    base:0x%08x%08x len :0x%08x%08x type:  %s\n", 
			mmap->base_addr_high, mmap->base_addr_low,
			mmap->length_high, mmap->length_low, 
			mb_mmap_types[mmap->type]);

		if ( mmap->type == 1 && mmap->base_addr_high == 0 ) {
			if ( mmap->length_high == 0)
				sz = mmap->length_low;
			else
				sz = 0xFFFFFFFF - mmap->base_addr_low;
			start = (mmap->base_addr_low + 0xFFF) & ~0xFFF;
			end = (mmap->base_addr_low + sz) & ~0xFFF;
			if ( start != end )
				physmm_free_range( start, end );
		}

		mmap_s += mmap->size + sizeof( unsigned long );
		mmap = ( multiboot_memory_map_t * )
				( mb_info->mmap_addr + mmap_s);

	} while ( mmap_s < mb_info->mmap_length );

	printf("bootstrap: registered %u MB of RAM\n", 
		physmm_count_free() / (1024*1024));
	
	printf("bootstrap: loaded between 0x%08p and 0x%08p\n", 
		&i386_start_kernel,
		&i386_end_kernel);

	physmm_claim_range( ( physaddr_t ) &i386_start_kernel, 
			    ( physaddr_t ) &i386_end_kernel );

	physmm_claim_range( 0, 0x100000 );
	if ( mb_info->mods_count != 0 )
		physmm_claim_range( (physaddr_t) mods, 
							(physaddr_t) mods + mb_info->mods_count );
	physmm_claim_range( (physaddr_t) mb_info,
						(physaddr_t) mb_info + 1 );

	printf("bootstrap: multiboot modules:\n");
	for ( idx = 0; idx < mb_info->mods_count; idx++ ) {
		if ( mods[idx].string == 0 )
			str = "<NULL>";
		else
			str = (char *) mods[idx].string;
		printf("    start: 0x%08x end: 0x%08x str: %s\n",
			mods[idx].mod_start, mods[idx].mod_end, str );
		physmm_claim_range( (physaddr_t) mods[idx].mod_start, 
							(physaddr_t) mods[idx].mod_end );
	}

	printf("bootstrap: %u MB of RAM available\n", 
		physmm_count_free() / (1024*1024));

	printf("bootstrap: creating page directory\n");
	platinit_mkdir( STPC );
	if ( STPF ) {
		printf("bootstrap: failed to create page directory\n");
		goto failure;
	}

	printf("bootstrap: identity mapping loader code and data\n");
	platinit_idmap( (physaddr_t) &i386_start_kernel, 
					(physaddr_t) &i386_end_kernel,
					PAGE_PERM_XRW,
					STPC );
	if ( STPF )
		goto idmapfail;
	platinit_idmap( (physaddr_t) mb_info,
					(physaddr_t) &mb_info[1],
					PAGE_PERM_XRW,
					STPC );
	if ( STPF )
		goto idmapfail;
	platinit_idmap( (physaddr_t) mods,
					(physaddr_t) &mods[ mb_info->mods_count ],
					PAGE_PERM_XRW,
					STPC );
	if ( STPF )
		goto idmapfail;
	for ( idx = 0; idx < mb_info->mods_count; idx++ ) {
		platinit_idmap( (physaddr_t) mods[idx].mod_start,
						(physaddr_t) mods[idx].mod_end,
						PAGE_PERM_XRW,
						STPC );
		if ( STPF )
			goto idmapfail;
	}
	
	//TODO: ID map other parts of the multiboot info

	printf("bootstrap: identity mapping first megabyte\n");
	
	platinit_idmap( 0x0, 0x100000,	PAGE_PERM_XRW |
									PAGE_FLAG_NOCACHE, STPC );
	if ( STPF )
		goto idmapfail;

	printf("bootstrap: enable paging\n");
	paging_enable( initial_pagedir );

	printf("bootstrap: intitializing module linker\n");
	
	modlink_init();
	for ( idx = 0; idx < mb_info->mods_count; idx++ ) {
		if ( mods[idx].string == 0 )
			str = "<NULL>";
		else
			str = (char *) mods[idx].string;
		printf("    start: 0x%08x end: 0x%08x str: %s\n",
			mods[idx].mod_start, mods[idx].mod_end, str );
		sz = mods[idx].mod_end - mods[idx].mod_start;
		modlink_load_image( (void *) mods[idx].mod_start, sz, STPC );
		if ( STPF )
			goto modulefail;
	}
	modlink_secondpass( STPC );
	if ( STPF )
		goto linkfail;
	printf("bootstrap: dynamically looking up and calling \"main\"\n");
	modlink_call( "main", NULL, STPC );
	if ( STPF )
		goto callfail;
	goto failure;
callfail:
	printf("bootstrap: failed to call main!\n");
	goto failure;
modulefail:
	printf("bootstrap: failed to load modules!\n");
	goto failure;
linkfail:
	printf("bootstrap: failed to link modules!\n");
	goto failure;
idmapfail:
	printf("bootstrap: failed to identitymap!\n");
failure:
	printf("bootstrap: halting processor!\n");
	for ( ;; ) ;

}
