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
 * @file modlink/link.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"
#include <assert.h>
#include <string.h>
#include <status.h>
#include <stdio.h>
#include "core/physmm.h"
#include "modlink/modlink.h"


Elf32_Sym modlink_getsym( const char *name ) {
	int has_weak = 0, bind, weak_s;
	Elf32_Sym *weak;
	Elf32_Word sym;
	Elf32_Sym *sym_p;
	Elf32_Sym notfound;
	modinfo_t *mod, *weak_mod;
	for (	mod = module_list;
			mod != NULL;
			mod = mod->next ) {
		sym = elf_findsym(	(Elf32_Word *) mod->hash, 
					( Elf32_Sym *) mod->symtab,
					( const char *) mod->strtab,
					name );
		
		if ( sym == STN_UNDEF )
			continue;		

		sym_p = ( Elf32_Sym *) (mod->symtab + mod->syment * sym );

		if ( sym_p->st_shndx == SHN_UNDEF )
			continue;

		bind = ELF32_ST_BIND( sym_p->st_info );
		
		if ( bind == STB_GLOBAL ) {
			printf("Bound symbol %s to image 0x%08X symbol %i at 0x%08X\n", 
				name, mod, sym, sym_p->st_value );
			return *sym_p;
		} else if ( bind == STB_WEAK ) {
			weak = sym_p;
			weak_mod = mod;
			weak_s = sym;
			has_weak = 1;
		}

	}
	
	if ( has_weak ) {
		printf("Weakly bound symbol %s to image 0x%08X symbol %i at 0x%08X\n",
			name, weak_mod, weak_s,weak->st_value);
		return *weak;
	}

	notfound.st_shndx = SHN_UNDEF;

	printf("Unresolved symbol %s\n", name);

	return notfound;
}

void modlink_secondpass( STPD ) {
	int m;
	Elf32_Addr sym;
	Elf32_Sym *sym_p, sym_f;
	const char *name;
	modinfo_t *mod;
	for (	mod = module_list;
			mod != NULL;
			mod = mod->next ) {

		if ( mod->flags & MODULE_LINKED )
			continue;

		sym = mod->symtab;
		
		for ( m = 0; m < mod->symsz; m++ ) {
			sym_p = ( Elf32_Sym * ) sym;
			sym += mod->syment;

			if ( m == STN_UNDEF )
				continue;

			if ( sym_p->st_shndx != SHN_UNDEF )
				continue;

			name = (const char *) (mod->strtab + sym_p->st_name);
			//printf("Resolving symbol %s to %i=0x%08x\n", name, sym_f.st_shndx, sym_f.st_value);
			sym_f = modlink_getsym( name );

			if ( sym_f.st_shndx == SHN_UNDEF )
				continue;

			sym_p->st_value = sym_f.st_value;
			sym_p->st_info = sym_f.st_info;
			sym_p->st_shndx = sym_f.st_shndx;


		}
		modreloc_apply ( mod );
		mod->flags |= MODULE_LINKED;
	}

	STRETV;
}
