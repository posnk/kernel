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
 * @file binfmt/elf/elfmem.c
 *
 * Part of posnk kernel bootstrap
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"

#include "binfmt/elf.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "core/paging.h"

char *phtypes[8] = {	
			"NULL", 
			"LOAD", 
			"DYNAMIC", 
			"INTERP", 
			"NOTE", 
			"SHLIB", 
			"PHDR",
			"TLS"
		};

void elfmem_do_load_segment(	void		*start, 
				size_t		 size, 
				void		*vstart,
				physaddr_t	 dir,
				Elf32_Phdr*	 seg )
{
	
}

#define ELFPTR(Off)	((void *) ( ((uintptr_t) start ) + ( Off ) ) )

Elf32_Off elfmem_find_offset( void *start, size_t size, Elf32_Addr vaddr )
{
	int phidx;
	Elf32_Phdr *seg_hdr;
	Elf32_Ehdr *elf_hdr = start;
	
	/* NOTE: All checks should already have been done by elfmem_load...*/
	
	seg_hdr = ELFPTR( elf_hdr->e_phoff );

	for ( phidx = 0; phidx < elf_hdr->e_phnum; phidx++ ) {
		if ( vaddr < seg_hdr[phidx].p_vaddr )
			continue;
		vaddr -= seg_hdr[phidx].p_vaddr;
		if ( vaddr >= seg_hdr[phidx].p_memsz )
			continue;
		assert( vaddr < seg_hdr[phidx].p_filesz );

		return seg_hdr[phidx].p_offset + ( Elf32_Off ) vaddr;
	}

	assert( !"Reference to unknown ELF VMA!" );

	return 0; /* Keep the compiler happy ! */
}

#define ELFDREF( AdDr )	( elfmem_find_offset ( start, size, AdDr ) )
#define ELFSTR( St, Off ) ( ELFPTR( St + Off ) )
int elfmem_parse_dynamic( void *start, size_t size, Elf32_Phdr *hdr )
{

	int e, cnt;
	Elf32_Dyn *ent;
	Elf32_Addr	strtab_ptr, symtab_ptr, rela_ptr, rel_ptr;
	Elf32_Word	strtab_sz, symtab_sz, rela_sz, rel_sz;
	Elf32_Off	strtab;
	
	ent = ELFPTR( hdr->p_offset );
	cnt = hdr->p_filesz / sizeof( Elf32_Dyn );

	for ( e = 0; e < cnt; e++ ) {
		switch( ent[e].d_tag ) {
			case DT_SYMTAB:
				symtab_ptr = ent[e].d_un.d_ptr;
				break;
			case DT_STRTAB:
				strtab_ptr = ent[e].d_un.d_ptr;
				break;
			case DT_RELA:
				rela_ptr = ent[e].d_un.d_ptr;
				break;
			case DT_REL:
				rel_ptr = ent[e].d_un.d_ptr;
				break;
			case DT_STRSZ:
				strtab_sz = ent[e].d_un.d_val;
				break;
			case DT_RELSZ:
				rel_sz = ent[e].d_un.d_val;
				break;
			case DT_RELASZ:
				rela_sz = ent[e].d_un.d_val;
				break;
			case DT_SYMENT:
				symtab_sz = ent[e].d_un.d_val;
				break;
			default:
				//printf("Unknown DYNAMIC entry tag 0x%08x\n", 
				//	ent[e].d_tag);
				break;
		}
	}
	strtab = ELF_DREF( strtab_ptr );

	for ( e = 0; e < cnt; e++ ) {
		switch( ent[e].d_tag ) {
			case DT_NEEDED:
				printf("DT_NEEDED: %s\n", ELFSTR( strtab, ent[e].d_un.d_val ) );
				break;
		}
	}
	return 0;

}

int elfmem_load_segments( void *start, size_t size )
{
	int phidx;
	Elf32_Phdr *seg_hdr;
	Elf32_Ehdr *elf_hdr = start;
	
	if ( !elf_verify_ident( elf_hdr ) )
		return 0; 
	
	if ( elf_hdr->e_machine != EM_386 )
		return 0;

	seg_hdr = ELFPTR( elf_hdr->e_phoff );

	assert( elf_hdr->e_phentsize == sizeof( Elf32_Phdr ) );

	for ( phidx = 0; phidx < elf_hdr->e_phnum; phidx++ ) {
		printf(	"segment: %s offset: 0x%08x vaddr: 0x%08x paddr:0x%08x"
			"\n     filesz: 0x%08x memsz:0x%08x flags:0x%08x "
			"align: 0x%08x\n", phtypes[seg_hdr[phidx].p_type],
				seg_hdr[phidx].p_offset,
				seg_hdr[phidx].p_vaddr,
				seg_hdr[phidx].p_paddr,
				seg_hdr[phidx].p_filesz,
				seg_hdr[phidx].p_memsz,
				seg_hdr[phidx].p_flags,
				seg_hdr[phidx].p_align);

		switch ( seg_hdr->p_type ) {
	
			case PT_NULL:
				break;

			case PT_LOAD:
				break;

			case PT_NOTE:
				break;

			case PT_TLS:
				break;

			case PT_INTERP:
				break;
	
			case PT_DYNAMIC:
				elfmem_parse_dynamic( start, size, &seg_hdr[ phidx ] );
				break;

			case PT_PHDR:
				break;

		}
	}
	return 0;
}
