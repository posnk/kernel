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
 * @file modlink/i386/reloc.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include "modlink/modlink.h"

void modreloc_apply ( modinfo_t *elf )
{
	Elf32_Addr relbas = elf->rel;
	Elf32_Addr reloff, rel;
	Elf32_Rel *rel_p;
	Elf32_Addr val;
	Elf32_Word *relval;
	Elf32_Word sym;
	Elf32_Addr symbas = elf->symtab;
	Elf32_Sym *sym_p;
	for ( reloff = 0; reloff < elf->relsz; reloff += elf->relent ) {
		rel = relbas + reloff;
		rel_p = ( Elf32_Rel * ) rel;

		sym = ELF32_R_SYM( rel_p->r_info );
		sym_p = (Elf32_Sym *)(symbas + sym*elf->syment);
		rel_p->r_offset -= elf->base;
		rel_p->r_offset += elf->mbase;
		switch ( ELF32_R_TYPE( rel_p->r_info ) ) {
			case R_386_NONE:
				break;
			case R_386_RELATIVE:
				relval = (void *) rel_p->r_offset;
				val = *relval;
				val -= elf->base;
				val += elf->mbase;
				*relval = val;
				break;
			case R_386_32:
				relval = (void *) rel_p->r_offset;
				val = *relval;
				val += sym_p->st_value;
				*relval = val;
			//	printf("R_386_32 at 0x%08x = 0x%08x written as 0x%08x\n", rel_p->r_offset, sym_p->st_value, val);
				break;
			case R_386_PC32:
				relval = (void *) rel_p->r_offset;
				val = *relval;
				val += sym_p->st_value;
				val -= rel_p->r_offset;
				*relval = val;
			//	printf("R_386_PC32 at 0x%08x = 0x%08x written as 0x%08x\n", rel_p->r_offset, sym_p->st_value, val);
				break;
			default:
				assert(!"!unknwn reloc i386");
		}
		
	}
}
