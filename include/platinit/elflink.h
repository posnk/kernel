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
 * @file platinit/elflink.h
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#ifndef __elflink__
#define __elflink__

#include "config.h"

#include "binfmt/elf.h"

typedef struct {

	/** The offset of this struct in the array */
	int		id;

	/** The physical start of the ELF file */
	void *		pstart;

	/** The physical size of the ELF file */
	size_t		psize;

	/** The base address of the ELF file */
	Elf32_Addr	base;

	/** The end address of the ELF file */
	Elf32_Addr	end;

	/** The mapped base address */
	Elf32_Addr	mbase;

	/** The address of the string table */
	Elf32_Addr	strtab;
	
	/** Pointer to the string table */
	const char *	strtab_p;

	/** The address of the symbol table */
	Elf32_Addr	symtab;

	/** Pointer to the symbol table */
	Elf32_Sym *	symtab_p;

	/** The address of the hash table */
	Elf32_Addr	hash;

	/** Pointer to the hash table */
	Elf32_Word *	hash_p;

	/** The address of the absolute relocations */
	Elf32_Addr	rela;

	/** Pointer to the absolute relocations */
	Elf32_Rela *	rela_p;

	/** The address of the relocations */
	Elf32_Addr	rel;

	/** Pointer to the relocations */
	Elf32_Rel *	rel_p;

	Elf32_Word	symsz;
	
	/** The size of the string table in bytes */
	Elf32_Word	strsz;

	/** The size of a symbol table entry in bytes */
	Elf32_Word	syment;

	/** The size of the relocation table in bytes */
	Elf32_Word	relsz;

	/** The size of the absolute relocation table in bytes */
	Elf32_Word	relasz;

} elfinfo_t;

void elflink_firstpass( elfinfo_t *elf );

#endif
