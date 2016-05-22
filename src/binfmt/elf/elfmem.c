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
#include "core/paging.h"

char *phtypes[8] = {	"NULL", 
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
		switch ( elf_hdr->p_type ) {
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
				break;

			case PT_PHDR:
				break;



		}
	}
	return 0;
}
