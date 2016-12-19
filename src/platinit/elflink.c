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
		elf->base = page_floor( hdr->p_vaddr );
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
	rstart = page_floor( hdr->p_vaddr );
	rend = page_ceil( hdr->p_vaddr + hdr->p_memsz );
	
	fstart = hdr->p_vaddr;
	fend = hdr->p_vaddr + hdr->p_filesz;

	/* pstart contains the physical offset of the segment data */
	pstart = (physaddr_t) elf->pstart;
	pstart += page_floor(hdr->p_offset);

	/* Calculate the page boundary of the last filled page */
	frend = page_ceil(fend);
	
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

//			printf("Copy over: 0x%x to 0x%x\n", cfstart, cdstart );

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
//			printf("Copy over: 0x%x to 0x%x %x bytes\n", fphys, cphys,
//				csz);	
			/* Copy in the data */
			memcpy( (void *) cphys, (void *) fphys, csz );

		} /* If the page fully contains the file, map it */
//			else printf("Nocopy\n");
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
			case DT_RELENT:
				elf->relent = ent[e].d_un.d_val;
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
	elf->hash_p  = platldr_getphys( elf->hash );
	elf->symsz = elf_hash_size( elf->hash_p );

	printf("strtab_p = 0x%08x\n", elf->strtab_p);

	if ( elf->rel )
		elf->rel_p = platldr_getphys( elf->rel );
	if ( elf->rela )
		elf->rela_p = platldr_getphys( elf->rela );

}

void elflink_symtab_fixup( elfinfo_t *elf ) {
	int n; 
	Elf32_Addr symtab = (Elf32_Addr) elf->symtab_p;
	Elf32_Sym *sym;

	for ( n = 0; n < elf->symsz; n++ ) {
		sym = ( Elf32_Sym * ) ( n * elf->syment + symtab );
		
		if ( sym->st_shndx == SHN_UNDEF )
			continue;
		
		sym->st_value -= elf->base;
		sym->st_value += elf->mbase;
	} 

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
	elflink_symtab_fixup( elf );

}

Elf32_Sym elflink_getsym( const char *name, elfinfo_t *elf, int elfcount ) {
	int n, has_weak = 0, weak_n, bind, weak_s;
	Elf32_Sym *weak;
	Elf32_Word sym;
	Elf32_Sym *sym_p;
	Elf32_Sym notfound;
	for ( n = 0; n < elfcount; n++ ) {
		sym = elf_findsym(	elf[n].hash_p, 
					elf[n].symtab_p,
					elf[n].strtab_p,
					name );
		
		if ( sym == STN_UNDEF )
			continue;		

		sym_p = ( Elf32_Sym *) (((uintptr_t)elf[n].symtab_p) + 
					( elf[n].syment * sym ));

		if ( sym_p->st_shndx == SHN_UNDEF )
			continue;

		bind = ELF32_ST_BIND( sym_p->st_info );
		
		if ( bind == STB_GLOBAL ) {
			printf("Bound symbol %s to image %i symbol %i\n", 
				name, n, sym );
			return *sym_p;
		} else if ( bind == STB_WEAK ) {
			weak = sym_p;
			weak_n = n;
			weak_s = sym;
			has_weak = 1;
		}

	}
	
	if ( has_weak ) {
		printf("Weakly bound symbol %s to image %i symbol %i\n",
			name, weak_n, weak_s);
		return *weak;
	}

	notfound.st_shndx = SHN_UNDEF;

	printf("Unresolved symbol %s\n", name);

	return notfound;
}

void elflink_secondpass( elfinfo_t *elf, int elfcount ) {
	int n,m;
	Elf32_Addr sym;
	Elf32_Sym *sym_p, sym_f;
	const char *name;

	for ( n = 0; n < elfcount; n++ ) {
		sym = ( Elf32_Addr ) elf[n].symtab_p;
		
		for ( m = 0; m < elf[n].symsz; m++ ) {
			sym_p = ( Elf32_Sym * ) sym;
			sym += elf[n].syment;

			if ( m == STN_UNDEF )
				continue;

			if ( sym_p->st_shndx != SHN_UNDEF )
				continue;

			name = elf[n].strtab_p + sym_p->st_name;
			sym_f = elflink_getsym( name, elf, elfcount );

			if ( sym_f.st_shndx == SHN_UNDEF )
				continue;

			sym_p->st_value = sym_f.st_value;
			sym_p->st_info = sym_f.st_info;
			sym_p->st_shndx = sym_f.st_shndx;


		}
		elfreloc_apply ( &elf[n] );
	}
}
