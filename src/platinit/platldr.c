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
 * @file platinit/platldr.c
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

uintptr_t platldr_image_map = KERNEL_SPLIT;
int platldr_image_ctr = 0;
int platldr_cur_image = -1;

physaddr_t platldr_pagedir;

uintptr_t platldr_start_image( int image_id )
{
	assert( platldr_cur_image < 0 );
	platldr_cur_image = image_id;
	return platldr_image_map;
}

void platldr_end_image( int image_id, uintptr_t end )
{
	assert( image_id == platldr_cur_image );
	assert( end > platldr_image_map );
	platldr_image_map = end;
}

void *platldr_getphys( uintptr_t virt ) {
	return (void *) paging_getphys( platldr_pagedir,
					( void * )virt,
					0 ,; //TODO: Implement
}

void platldr_map ( int id, uintptr_t virt, physaddr_t phys, int flags )
{
	printf( "Map: 0x%08x to 0x%08x\n", virt, phys );	
}
