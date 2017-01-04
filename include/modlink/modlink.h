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
 * @file modlink/modlink.h
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#ifndef __modlink__
#define __modlink__

#include "config.h"
#include <status.h>

#include "binfmt/elf.h"

#define MODULE_NOTHEAP	(1)
#define MODULE_LINKED	(2)

typedef struct modinfo modinfo_t;

struct modinfo {
	
	modinfo_t *	next;

	/** The offset of this struct in the array */
	int		flags;

	/** The physical start of the ELF file */
	void *		image_virt;

	/** The physical size of the ELF file */
	size_t		image_size;

	/** The base address of the ELF file */
	Elf32_Addr	base;

	/** The end address of the ELF file */
	Elf32_Addr	end;

	/** The mapped base address */
	Elf32_Addr	mbase;

	/** The address of the string table */
	Elf32_Addr	strtab;

	/** The address of the symbol table */
	Elf32_Addr	symtab;	

	/** The address of the hash table */
	Elf32_Addr	hash;
	
	/** The address of the absolute relocations */
	Elf32_Addr	rela;
	
	/** The address of the relocations */
	Elf32_Addr	rel;

	Elf32_Word	relent;

	Elf32_Word	symsz;
	
	/** The size of the string table in bytes */
	Elf32_Word	strsz;

	/** The size of a symbol table entry in bytes */
	Elf32_Word	syment;

	/** The size of the relocation table in bytes */
	Elf32_Word	relsz;

	/** The size of the absolute relocation table in bytes */
	Elf32_Word	relasz;

};

extern modinfo_t *module_list;

void modlink_map(	modinfo_t *mod, 
					Elf32_Addr virt,
					physaddr_t phys,
					int flags, STPD );

void modlink_remap(	modinfo_t *mod, 
					Elf32_Addr virt,
					Elf32_Addr orig,
					int flags, STPD );

void modlink_unmap( modinfo_t *mod, Elf32_Addr virt, STPD );

void modlink_unmap_unsafe( modinfo_t *mod, Elf32_Addr virt );
void modlink_parse_dynamic( modinfo_t *mod, Elf32_Phdr *hdr );
void modlink_parse_load( modinfo_t *mod, Elf32_Phdr *hdr, STPD );
void modlink_symtab_fixup( modinfo_t *mod );
void modlink_init( void );
uintptr_t modlink_image_start( modinfo_t *mod );
void modlink_image_end( modinfo_t *mod, uintptr_t end );
void modlink_firstpass( modinfo_t *mod, STPD );
void modlink_secondpass( STPD );
Elf32_Sym modlink_getsym( const char *name );
void modlink_call( const char *name, void *param1, STPD );
void modreloc_apply ( modinfo_t *elf );
void modlink_load_image( void *image, size_t size, STPD );
#endif
