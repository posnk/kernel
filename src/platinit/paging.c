/******************************************************************************\
Copyright (C) 2016 Peter Bosch

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
 * @file src/platinit/paging.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"
#include <status.h>
#include "core/paging.h"

physaddr_t initial_pagedir;

void platinit_mkdir( STPD )
{
	initial_pagedir = paging_newdir( 0, STPC );	
}

void platinit_idmap(	physaddr_t start, 
						physaddr_t end, 
						int flags, 
						STPD )
{
	physaddr_t work = start;
	for ( work = start; work < end; work += ARCH_PAGE_SIZE ) {
		paging_map( initial_pagedir, 
					(void *) work, 
					work, 
					flags, 
					STPC );
		if ( STPF );
			
	}
}
