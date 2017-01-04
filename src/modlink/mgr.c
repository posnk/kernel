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
 * @file modlink/mgr.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"

#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <status.h>
#include "core/physmm.h"
#include "core/paging.h"
#include "modlink/modlink.h"

modinfo_t *module_list;
modinfo_t *modlink_current_image;

uintptr_t modlink_image_map = KERNEL_SPLIT;

#ifndef OPT_heapmemory

physaddr_t modlink_info_frame;
modinfo_t *modlink_info_map;
int modlink_info_ptr;

modinfo_t *modlink_alloc( STPD )
{
	modinfo_t *result;
	int count = ARCH_PAGE_SIZE / sizeof( modinfo_t );
	assert( count > 0 );
	assert( modlink_current_image == NULL );

	if ( modlink_info_map == NULL || modlink_info_ptr >= count ) {
		modlink_info_frame = physmm_alloc_frame();

		if ( modlink_info_frame == PHYSMM_NO_FRAME )
			STERR( NULL, STCODE_NOMEM,
					"Could not allocate info frame!" );

		modlink_info_map = ( modinfo_t * ) modlink_image_map;

		paging_map( PAGING_CUR_DIR,
					modlink_info_map,
					modlink_info_frame,
					PAGE_FLAG_GLOBAL |
					PAGE_PERM_RW, STPC );

		if ( STPF ) {
			physmm_free_frame( modlink_info_frame );
			modlink_info_map = NULL;
			STERR( NULL, STCODE_NOMEM, "Could not map info frame!" );
		}

		modlink_image_map += ARCH_PAGE_SIZE;

		memset( modlink_info_map, 0, ARCH_PAGE_SIZE );

		modlink_info_ptr = 0;
				
	}

	result = &modlink_info_map[ modlink_info_ptr++ ];
	result->flags |= MODULE_NOTHEAP;

	STRET(result);
}

void modlink_free( modinfo_t *mod )
{
}

#else

modinfo_t *modlink_alloc( STPD )
{
	//TODO: Call malloc
	STERR( STCODE_NOIMPL, "modlink_alloc is not implemented", NULL );
}

void modlink_free( modinfo_t *mod )
{
	if ( mod->flags & MODULE_NOTHEAP )
		return;
	//TODO: Call free
}

#endif 

void modlink_init( void ) {
	module_list = NULL;
	modlink_current_image = NULL;
#ifndef OPT_heapmemory
	modlink_info_map = NULL;
#endif
}

void modlink_load_image( void *image, size_t size, STPD )
{
	modinfo_t *mod = modlink_alloc( STPC );

	if ( STPF )
		STERRV( STCODE_NOMEM, "Could not allocate descriptor" );

	mod->image_virt = image;
	mod->image_size = size;

	mod->next = module_list;
	module_list = mod;

	modlink_firstpass( mod, STPC );
	
	STRETV;

}

void modlink_call( const char *name, void *param1, STPD )
{
	void (*func)(void*);
	Elf32_Sym sym = modlink_getsym( name );

	if ( sym.st_shndx == SHN_UNDEF )
			STERRV( STCODE_NOENT, "No such symbol" );

	func = ( void * ) sym.st_value;

	func(param1);

	STRETV;
	
}

uintptr_t modlink_image_start( modinfo_t *mod )
{
	assert( modlink_current_image == NULL );
	modlink_current_image = mod;
	printf("Base :0x%08x\n", modlink_image_map);
	return modlink_image_map;
}

void modlink_image_end( modinfo_t *mod, uintptr_t end )
{
	assert( modlink_current_image == mod );
	assert( end > modlink_image_map );
	modlink_image_map = end;
	modlink_current_image = NULL;
}

