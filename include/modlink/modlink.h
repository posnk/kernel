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

#include "binfmt/elf.h"

typedef struct {

	/** The offset of this struct in the array */
	int		id;

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

} modinfo_t;

void elflink_firstpass( elfinfo_t *elf );

#endif
