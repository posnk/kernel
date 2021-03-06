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
 * @file modlink/load.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"
#include <status.h>
#include "core/physmm.h"
#include "core/paging.h"
#include "modlink/modlink.h"
#include <stdio.h>

void modlink_parse_load( modinfo_t *mod, Elf32_Phdr *hdr, STPD )
{
	Elf32_Addr rstart, rend;
	Elf32_Addr fstart, fend, frend;
	Elf32_Addr caddr, cend, cmaddr;
	Elf32_Addr imstart, impstart, cdstart, cfstart;
	Elf32_Word psz, csz;
	physaddr_t cphys;
	int flags;

	/* Determine base address */
	if ( mod->base == 0xFFFFFFFF ) {
		mod->base = page_floor( hdr->p_vaddr );
		mod->mbase = modlink_image_start( mod );
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
	rstart = page_floor( hdr->p_vaddr );
	rend = page_ceil( hdr->p_vaddr + hdr->p_memsz );
	
	fstart = hdr->p_vaddr;
	fend = hdr->p_vaddr + hdr->p_filesz;

	/* pstart contains the physical offset of the segment data */
	imstart = (Elf32_Addr) mod->image_virt;
	imstart += page_floor(hdr->p_offset);

	/* Calculate the page boundary of the last filled page */
	frend = page_ceil(fend);

	/* Map all pages containing file data */
	for ( caddr = rstart; caddr < frend; caddr = cend ) {
		cend = caddr + ARCH_PAGE_SIZE;
		cmaddr = (caddr - mod->base) + mod->mbase;
		impstart = caddr - rstart + imstart;
		if ( caddr < fstart ) {
			/* This page does not fully contain the file */

			/* Allocate a fresh frame for this page */
			cphys = physmm_alloc_frame();

			/* Check whether the allocation succeded */
			if ( cphys == PHYSMM_NO_FRAME )
				STERRV( STCODE_NOMEM, 
						"No memory to allocate segment start frame" );
			
			/* Map the page */
			modlink_map( mod, cmaddr, cphys, flags, STPC );
			
			/* Check for errors */
			if ( STPF ) {
				STERRV( STCODE_NOMEM, 
						"Could not map start frame" );
				physmm_free_frame( cphys );
			}

			/* Calculate the padding size */
			psz = fstart - caddr;

			/* Clear the padded start */
			memset( (void *) cmaddr, 0, psz );

			/* Calculate the offsets */
			cdstart = cmaddr + psz;
			cfstart = impstart + psz;
			csz = cend - fstart;

			/* Copy in the data */
			memcpy( (void *) cdstart, (void *) cfstart, csz );
   
		} else if ( cend > fend ) {
			/* This page does not fully contain the file */

			/* Allocate a fresh frame for this page */
			cphys = physmm_alloc_frame();

			/* Check whether we had enough memory */
			if ( cphys == PHYSMM_NO_FRAME )
				STERRF( STCODE_NOMEM, 
						"No memory to allocate segment end frame",
						undomap );
			
			/* Map the page */
			modlink_map( mod, cmaddr, cphys, flags, STPC );
			
			/* Check for errors */
			if ( STPF ) {
				physmm_free_frame( cphys );
				goto undomap;
			}
			
			/* Calculate the padding size */
			psz = cend - fend;
		
			/* Calculate the data size and padding start */
			csz = fend - caddr;
			cdstart = cmaddr + csz;
	
			/* Clear the padded end */
			memset( (void *) cdstart, 0, psz );

			/* Copy in the data */
			memcpy( (void *) cmaddr, (void *) impstart, csz );

		} else {
		
			modlink_remap( mod, cmaddr, impstart, flags, STPC );
			
			/* Check for errors */
			if ( STPF )
				goto undomap;
		}

	}

	/* Map all pages which should not contain file data */
	for ( caddr = frend; caddr < rend; caddr = cend ) {
		cend = caddr + ARCH_PAGE_SIZE;
		cmaddr = (caddr - mod->base) + mod->mbase;
		
		/* Allocate a physical frame */
		cphys = physmm_alloc_frame();

		/* Check whether we had enough memory */
		if ( cphys == PHYSMM_NO_FRAME )
			STERRF( STCODE_NOMEM, 
					"No memory to allocate segment end frame",
					undomap );
		
		/* Map the frame */
		modlink_map( mod, cmaddr, cphys, flags, STPC );
			
		/* Check for errors */
		if ( STPF )
			goto undomap;
			
		/* Clear the page */
		memset( (void *) cmaddr, 0, ARCH_PAGE_SIZE );
	}
	
	/* Determine image end address */
	if ( rend > mod->end )
		mod->end = rend;

	STRETV;

undomap:
	fend = caddr;

	/* Unmap all mapped pages */
	for ( caddr = rstart; caddr < rend; caddr = cend ) {
		cend = caddr + ARCH_PAGE_SIZE;
		cmaddr = (caddr - mod->base) + mod->mbase;
		
		/* Actually unmap the page */
		modlink_unmap_unsafe( mod, cmaddr );
		
		
	}

	return;
	
}

/**
 * Parse the PT_DYNAMIC segment and initialize the fields in
 * the modinfo structure.
 *
 * This function is executed after the module has been loaded
 * to the correct virtual addresses but before fixups have been
 * applied.
 *
 * @param mod	The module info block
 * @param hdr	The program header for the PT_DYNAMIC segment
 */

void modlink_parse_dynamic( modinfo_t *mod, Elf32_Phdr *hdr )
{

#ifdef OPT_no_paging
	physaddr_t	ent_frame,
				ent_phys;
	Elf32_Dyn	ent_tmp;
	Elf32_Addr	ent_page = 0xFFFFFFFF,
				ent_max;
#endif

	Elf32_Addr	ent_addr, ent_off;
	Elf32_Dyn *	ent;
	
	for (	ent_off = 0; 
			ent_off < hdr->p_filesz; 
			ent_off += sizeof( Elf32_Dyn )
		) {

		ent_addr = hdr->p_vaddr + ent_off;
		ent_addr = ( ent_addr - mod->base ) + mod->mbase;

#ifdef OPT_no_paging
	
		/* Calculate the physical offset to the address */
		ent_phys = ent_frame + page_mod( ent_addr );

		ent = ( Elf32_Dyn * ) ent_phys;

		/* Check whether we crossed a page boundary */
		if ( page_floor( ent_addr ) != ent_page ) {

			ent_page = page_floor( ent_addr );

			ent_frame = modlink_getphys( ent_page );
	
			ent_phys = ent_frame + page_mod( ent_addr );

			ent = ( Elf32_Dyn * ) ent_phys;
			
		} else if ( ent_addr > ent_max ) {
			/* The entry is on a page boundary */

			/* The entry ought to be aligned to a 4 byte boundary */
			assert( ( ent_addr & 0x3 ) == 0 );

			/* Use some hacking to decode the structure */
			//XXX: Dependent on ELF structure layout

			/* Upper portion, in old frame */
			ent_tmp.d_tag = ent->d_tag;

			ent_page = page_ceil( ent_addr );
			ent_frame = modlink_getphys( ent_page );

			ent_phys = ent_frame - 0x4;

			ent = ( Elf32_Dyn * ) ent_phys;

			/* Lower portion */
			ent_tmp.d_un.d_val = ent->d_un.d_val;

		}
#else
		ent = ( Elf32_Dyn * ) ent_addr;
#endif

		switch( ent->d_tag ) {
			case DT_STRTAB:
				mod->strtab = (ent->d_un.d_ptr - mod->base ) + mod->mbase;
				break;
			case DT_SYMTAB:
				mod->symtab = (ent->d_un.d_ptr - mod->base ) + mod->mbase;
				break;
			case DT_HASH:
				mod->hash = (ent->d_un.d_ptr - mod->base ) + mod->mbase;
				break;
			case DT_REL:
				mod->rel = (ent->d_un.d_ptr - mod->base ) + mod->mbase;
				break;
			case DT_RELA:
				mod->rela = (ent->d_un.d_ptr - mod->base ) + mod->mbase;
				break;
			case DT_STRSZ:
				mod->strsz = ent->d_un.d_val;
				break;
			case DT_RELENT:
				mod->relent = ent->d_un.d_val;
				break;
			case DT_RELSZ:
				mod->relsz = ent->d_un.d_val;
				break;
			case DT_RELASZ:
				mod->relasz = ent->d_un.d_val;
				break;
			case DT_SYMENT:
				mod->syment = ent->d_un.d_val;
				break;
		}

	}

}

void modlink_symtab_fixup( modinfo_t *mod ) {
	int n; 
	Elf32_Addr symtab = mod->symtab;
	Elf32_Sym *sym;

	for ( n = 0; n < mod->symsz; n++ ) {
		sym = ( Elf32_Sym * ) ( n * mod->syment + symtab );
		
		if ( sym->st_shndx == SHN_UNDEF )
			continue;
		
		sym->st_value -= mod->base;
		sym->st_value += mod->mbase;
	} 

}

void modlink_firstpass( modinfo_t *mod, STPD ) {
	Elf32_Phdr *phdr;
	Elf32_Ehdr *ehdr;
	Elf32_Off phdr_a, phdr_e, phdr_i;

	ehdr = mod->image_virt;

	phdr_i = ehdr->e_phentsize;
	phdr_a = ehdr->e_phoff;
	phdr_e = phdr_a + phdr_i * ehdr->e_phnum;
	
	mod->base = 0xFFFFFFFF;

	for ( ; phdr_a < phdr_e; phdr_a += phdr_i ) {
		phdr = (void *)(((uintptr_t)mod->image_virt) + phdr_a);
		
		switch ( phdr->p_type ) {
			case PT_NULL:
				break;
			case PT_LOAD:
				modlink_parse_load( mod, phdr, STPC );
				if ( STPF ) {
					modlink_image_end(	mod, 
										( mod->end - mod->base ) + 
										mod->mbase );
					return;
				}
				break;
			case PT_DYNAMIC:
				modlink_parse_dynamic( mod, phdr );
				break;
			default:
				printf( "Image %08x: Unknown program header "
					"type %08x!\n", mod, phdr->p_type );
				break;			
		}
	}
	mod->symsz = elf_hash_size( (Elf32_Word *) mod->hash );
	modlink_image_end( mod, ( mod->end - mod->base ) + mod->mbase );
	modlink_symtab_fixup( mod );

}

