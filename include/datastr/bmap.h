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
 * @file include/datastr/bmap.h
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#ifndef __bmap__
#define __bmap__

#include <limits.h>

typedef unsigned int bmap_t;

/**
 * The amount of bits in a bmap_t
 */
#define BMAP_BITS		( CHAR_BITS * sizeof( bmap_t ) )

/**
 * @brief Tests if a bit is on.
 * @note This function is generally defined as a macro.
 * @param Map	The bitmap to work on
 * @param Pos	The bit position to test
 * @return Zero if and only if the bit is clear
 */
#define bmap_test(Map,Pos)	( (Map)[(Pos)/BMAP_BITS] & \
								( 1 << ( (Pos) % BMAP_BITS ) ) )

/**
 * @brief Sets a bit in the bitmap.
 * @note This function is generally defined as a macro.
 * @param Map	The bitmap to work on
 * @param Pos	The bit position to set
 */
#define bmap_set(Map,Pos)	do{ (Map)[(Pos)/BMAP_BITS] |= \
							( 1 << ( (Pos) % BMAP_BITS ) ) }while(0)

/**
 * @brief Clears a bit in the bitmap
 * @note This function is generally defined as a macro.
 * @param Map	The bitmap to work on
 * @param Pos	The bit position to clear
 */
#define bmap_clear(Map,Pos)	do{ (Map)[(Pos)/BMAP_BITS] &= \
							~( 1 << ( (Pos) % BMAP_BITS ) ) }while(0)

/**
 * @brief Gets the memory size of a bitmap.
 * @note This function is generally defined as a macro.
 * @param Size	The number of bits in the bitmap
 * @return	The number of char sized memory positions needed
 */
#define bmap_size(Size)	( ((Size) - 1 + CHAR_BITS) / CHAR_BITS )

/**
 * @brief Find the first clear bit in a bitmap.
 * @param map	The bitmap to work on.
 * @param count The number of bits in the bitmap.
 * @return	The position of the clear bit or -1 when none is found.
 */
int bmap_ffc( bmap_t *map, int count );

/**
 * @brief Find the first set bit in a bitmap.
 * @param map	The bitmap to work on.
 * @param count The number of bits in the bitmap.
 * @return	The position of the set bit or -1 when none is found.
 */
int bmap_ffs( bmap_t *map, int count );

#endif
