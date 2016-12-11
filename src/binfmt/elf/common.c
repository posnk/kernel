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
 * @file binfmt/elf/common.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"
#include "binfmt/elf.h"
#include <string.h>
#include <assert.h>

/**
 * Verifies the ELF ident entry from the Header
 * This makes sure the ELF file has:
 *   * The correct magic
 *   * The 32-bit ELF class
 *   * The ELFDATA2LSB encoding
 *   * The correct version.
 * @return Whether the header was correct ( 0 = false )
 */
int elf_verify_ident( Elf32_Ehdr * Header )
{
	if ( 0 != memcmp( Header->e_ident, ELFMAG, SELFMAG) )
		return 0; /* Invalid magic */

	else if ( Header->e_ident[EI_CLASS] != ELFCLASS32 )
		return 0;

	else if ( Header->e_ident[EI_DATA] != ELFDATA2LSB )
		return 0;

	else if ( Header->e_ident[EI_VERSION] != EV_CURRENT )
		return 0;
	else
		return 1;

}

/**
 * The hash function for ELF hash tables
 * @param name The string to hash
 * @return The hash for the string
 */
Elf32_Word	elf_hash_func( const char *name )
{

	Elf32_Word	h = 0, g;

	assert ( name != NULL );

	while ( *name ) {

		h = ( h << 4 ) + (unsigned char ) *name++;

		if ( g = h & 0xF0000000 )
			h ^= g >> 24;

		h &= ~g;

	}

	return h;

}

/**
 * Gets the size from an ELF hash table
 * @param The pointer to an ELF hash table
 * @return The size in words
 */
Elf32_Word	elf_hash_size( Elf32_Word *hashtab )
{
	return hashtab[1];
}

/**
 * Gets the first bucket in the ELF hashtable for given hash
 * @param hashtab The hash table
 * @param hash The hash to lookup
 * @return The first bucket index
 */
Elf32_Word	elf_hash_find( Elf32_Word *hashtab, Elf32_Word hash )
{
	Elf32_Word nbucket;
	nbucket= hashtab[0];
	return hashtab[ 2 + ( hash % nbucket ) ];
}


/**
 * Gets the next bucket in the ELF hashtable bucket chain
 * @param The hash table
 * @param The current bucket
 * @return The next bucket
 * @note The caller must check whether the operation is valid.
 */
Elf32_Word	elf_hash_next( Elf32_Word *hashtab, Elf32_Word symb )
{


	Elf32_Word nchain, nbucket;
	nbucket= hashtab[0];
	nchain = hashtab[1];
	assert( symb != STN_UNDEF );
	assert( symb < nchain );
	return hashtab[ 2 + nbucket + symb ];
}

Elf32_Word	elf_findsym(	Elf32_Word *hashtab, 
				Elf32_Sym *symtab,
				const char *strtab,
				const char *name )
{

	Elf32_Word cur;
	Elf32_Word hash;

	hash = elf_hash_func( name );
	
	cur = elf_hash_find( hashtab, hash );

	while ( cur != STN_UNDEF ) {
	
		if ( symtab[ cur ].st_name != 0 )
			printf("SYMBOL %i %s cmp %s\n", cur,
				&strtab[symtab[ cur ].st_name],name);
	
		if (	symtab[ cur ].st_name != 0 && 
			!strcmp( name, &strtab[ symtab[ cur ].st_name ] ) )
			return cur;
	
		cur = elf_hash_next( hashtab, cur );

	}

	return STN_UNDEF;

}

