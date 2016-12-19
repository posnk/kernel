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
 * @file path/file.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"
#include <assert.h>
#include "platinit/elflink.h"
#include "platinit/platldr.h"
void elfreloc_apply ( elfinfo_t *elf )
{
	Elf32_Addr relbas = (Elf32_Addr) elf->rel_p;
	Elf32_Addr reloff, rel;
	Elf32_Rel *rel_p;
	Elf32_Addr val;
	Elf32_Word *relval;
	Elf32_Word sym;
	Elf32_Addr symbas = (Elf32_Addr) elf->symtab_p;
	Elf32_Sym *sym_p;
	unsigned char type;
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
			case R_386_32:
				relval = platldr_getphys( rel_p->r_offset );
				val = *relval;
				val += sym_p->st_value;
				*relval = val;
				printf("R_386_32 0x%08x = 0x%08x\n", rel_p->r_offset, val);
				break;
			case R_386_PC32:
				relval = platldr_getphys( rel_p->r_offset );
				val = *relval;
				val += sym_p->st_value;
				val -= rel_p->r_offset;
				*relval = val;
				printf("R_386_PC32 0x%08x = 0x%08x\n", rel_p->r_offset, val);
				break;
			default:
				assert("!unknwn reloc i386");
		}
		
	}
}
