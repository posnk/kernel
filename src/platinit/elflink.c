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
 * @file platinit/elflink.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "core/physmm.h"
#include "core/paging.h"
#include "platinit/elflink.h"
#include "platinit/platldr.h"

void elflink_parse_load( elfinfo_t *elf, Elf32_Phdr *hdr )
{
	Elf32_Addr rstart, rend;
	Elf32_Addr fstart, fend, frend;
	Elf32_Addr caddr, cend, cmaddr;
	physaddr_t cphys, pstart, fphys, cdstart, cfstart, csz, psz;
	int flags;

	/* Determine base address */
	if ( elf->base == 0xFFFFFFFF ) {
		elf->base = arch_page_floor( hdr->p_vaddr );
		elf->mbase = platldr_start_image( elf->id );
	}

	/* Setup page flags */
	flags = PAGE_FLAG_GLOBAL;
	
	if ( hdr->p_flags & PF_R )
		flags |= PAGE_PERM_R;
	if ( hdr->p_flags & PF_W )
		flags |= PAGE_PERM_W;
	if ( hdr->p_flags & PF_X )
		flags |= PAGE_PERM_X;

	//Note: Assume page size power of 2
	/* Calculate the page boundaries of the LOAD */
	rstart = arch_page_floor( hdr->p_vaddr );
	rend = arch_page_ceil( hdr->p_vaddr + hdr->p_memsz );
	
	fstart = hdr->p_vaddr;
	fend = hdr->p_vaddr + hdr->p_filesz;

	/* pstart contains the physical offset of the segment data */
	pstart = (physaddr_t) elf->pstart;
	pstart += arch_page_floor(hdr->p_offset);

	/* Calculate the page boundary of the last filled page */
	frend = arch_page_ceil(fend);
	
	/* Determine image end address */
	if ( rend > elf->end )
		elf->end = rend;

	/* Map all pages containing file data */
	for ( caddr = rstart; caddr < frend; caddr = cend ) {
		cend = caddr + ARCH_PAGE_SIZE;
		cmaddr = (caddr - elf->base) + elf->mbase;
		cphys = fphys = caddr - rstart + pstart;
		if ( caddr < fstart ) {
			/* This page does not fully contain the file */

			/* Allocate a fresh frame for this page */
			cphys = physmm_alloc_frame();

			/* Calculate the padding size */
			psz = fstart - caddr;

			/* Clear the padded start */
			memset( (void *) cphys, 0, psz );

			/* Calculate the offsets */
			cdstart = cphys + psz;
			cfstart = fphys	+ psz;
			csz = cend - fstart;

			/* Copy in the data */
			memcpy( (void *) cdstart, (void *) cfstart, csz );
   
		} else if ( cend > fend ) {
			/* This page does not fully contain the file */

			/* Allocate a fresh frame for this page */
			cphys = physmm_alloc_frame();
			
			/* Calculate the padding size */
			psz = cend - fend;
		
			/* Calculate the data size and padding start */
			csz = fend - caddr;
			cdstart = cphys + csz;
	
			/* Clear the padded end */
			memset( (void *) cdstart, 0, psz );
			
			/* Copy in the data */
			memcpy( (void *) cphys, (void *) fphys, csz );

		} /* If the page fully contains the file, map it */

		platldr_map( elf->id, cmaddr, cphys, flags ); 

	}

	/* Map all pages which should not contain file data */
	for ( caddr = frend; caddr < rend; caddr = cend ) {
		cend = caddr + ARCH_PAGE_SIZE;
		cphys = physmm_alloc_frame();
		cmaddr = (caddr - elf->base) + elf->mbase;
		memset( (void *) cphys, 0, ARCH_PAGE_SIZE );
		platldr_map( elf->id, cmaddr, cphys, flags );
	}
	
	
}

void elflink_parse_dynamic( elfinfo_t *elf, Elf32_Phdr *hdr )
{
	Elf32_Dyn *ent;
	int e, dyncnt;
	
	ent = (Elf32_Dyn *)(hdr->p_offset + (uintptr_t) elf->pstart);
	dyncnt = hdr->p_filesz / sizeof( Elf32_Dyn );
	
	for ( e = 0; e < dyncnt; e++ ) {
		switch( ent[e].d_tag ) {
			case DT_STRTAB:
				elf->strtab = ent[e].d_un.d_ptr;
				break;
			case DT_SYMTAB:
				elf->symtab = ent[e].d_un.d_ptr;
				break;
			case DT_HASH:
				elf->hash = ent[e].d_un.d_ptr;
				break;
			case DT_REL:
				elf->rel = ent[e].d_un.d_ptr;
				break;
			case DT_RELA:
				elf->rela = ent[e].d_un.d_ptr;
				break;
			case DT_STRSZ:
				elf->strsz = ent[e].d_un.d_val;
				break;
			case DT_RELSZ:
				elf->relsz = ent[e].d_un.d_val;
				break;
			case DT_RELASZ:
				elf->relasz = ent[e].d_un.d_val;
				break;
			case DT_SYMENT:
				elf->syment = ent[e].d_un.d_val;
				break;
		}
	} 
}

void elflink_infofixup( elfinfo_t *elf ) {
	assert( elf->strtab );
	assert( elf->symtab );
	assert( elf->hash );
	assert( elf->rel || elf->rela );

	elf->strtab = (elf->strtab - elf->base) + elf->mbase;
	elf->symtab = (elf->symtab - elf->base) + elf->mbase;
	elf->hash = (elf->hash - elf->base) + elf->mbase;
	if ( elf->rel )
		elf->rel = (elf->rel - elf->base) + elf->mbase;
	if ( elf->rela )
		elf->rela = (elf->rela - elf->base) + elf->mbase;

	elf->strtab_p = platldr_getphys( elf->strtab );
	elf->symtab_p = platldr_getphys( elf->symtab );
	elf->hash_p = platldr_getphys( elf->hash );
	if ( elf->rel )
		elf->rel_p = platldr_getphys( elf->rel );
	if ( elf->rela )
		elf->rela_p = platldr_getphys( elf->rela );

}



void elflink_firstpass( elfinfo_t *elf ) {
	Elf32_Phdr *phdr;
	Elf32_Ehdr *ehdr;
	Elf32_Off phdr_a, phdr_e, phdr_i;

	ehdr = elf->pstart;

	phdr_i = ehdr->e_phentsize;
	phdr_a = ehdr->e_phoff;
	phdr_e = phdr_a + phdr_i * ehdr->e_phnum;
	
	elf->base = 0xFFFFFFFF;

	for ( ; phdr_a < phdr_e; phdr_a += phdr_i ) {
		phdr = (void *)(((uintptr_t)elf->pstart) + phdr_a);
		
		switch ( phdr->p_type ) {
			case PT_NULL:
				break;
			case PT_LOAD:
				elflink_parse_load( elf, phdr );
				break;
			case PT_DYNAMIC:
				elflink_parse_dynamic( elf, phdr );
				break;
			default:
				printf( "Image %i: Unknown program header "
					"type %08x!\n", elf->id, phdr->p_type );
				break;			
		}
	}

	platldr_end_image( elf->id, ( elf->end - elf->base ) + elf->mbase );
	elflink_infofixup( elf );

//	elflink_relocate( elf );
}
