/******************************************************************************\
Copyright (C) 2017 Peter Bosch

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
 * @file modlink/map.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"
#include "core/physmm.h"
#include "modlink/modlink.h"
#include "core/paging.h"

uintptr_t platldr_image_map = KERNEL_SPLIT;
int platldr_image_ctr = 0;
int platldr_cur_image = -1;


void modlink_map(	modinfo_t *mod, 
					Elf32_Addr virt,
					physaddr_t phys,
					int flags, STPD ) {
	paging_map( PAGING_CUR_DIR, (void *) virt, phys, flags, STPC );
}

void modlink_remap(	modinfo_t *mod, 
					Elf32_Addr virt,
					Elf32_Addr orig,
					int flags, STPD ) {
	physaddr_t phys;
	phys = paging_getphys( PAGING_CUR_DIR, (void *) orig, 0, STPC );
	if ( STPF )
		return;
	paging_map( PAGING_CUR_DIR, (void *) virt, phys, flags, STPC );
}

void modlink_unmap( modinfo_t *mod, Elf32_Addr virt, STPD ) {
	return;
}

void modlink_unmap_unsafe( modinfo_t *mod, Elf32_Addr virt ) {
	STPO;
	modlink_unmap( mod, virt, STPC );
}

