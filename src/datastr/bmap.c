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
 * @file datastr/bmap.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "datastr/bmap.h"

#include <stddef.h>
#include <assert.h>

int bmap_ffc( bmap_t *map, int count ) {
	int word, wc, bit, bp = -1;
	bmap_t w;
	wc = count / BMAP_BITS;
	for ( word = 0; word < wc; word++ ) {
		w = map[word];
		if ( w == ~((bmap_t) 0) )
			continue;
		for ( bit = 0; bit < BMAP_BITS; bit++ )
			if ( !( w & ( 1 << bit ) ) ) {
				bp = word + BMAP_BITS * bit; 
				if ( bp >= count )
					bp = -1;
				return bp;
			}
	}
	return -1;
}

int bmap_ffs( bmap_t *map, int count ) {
	int word, wc, bit, bp = -1;
	bmap_t w;
	wc = count / BMAP_BITS;
	for ( word = 0; word < wc; word++ ) {
		w = map[word];
		if ( w == 0 )
			continue;
		for ( bit = 0; bit < BMAP_BITS; bit++ )
			if ( ( w & ( 1 << bit ) ) ) {
				bp = word + BMAP_BITS * bit; 
				if ( bp >= count )
					bp = -1;
				return bp;
			}
	}
	return -1;
}
