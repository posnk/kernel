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
 * @file include/crt/string.h
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#ifndef __string__
#define __string__

#include <stddef.h>
#include "config.h"

/**
 * @brief Copies a block of memory
 * @param dest	The place the data will be copied to
 * @param src	The place the data will be copied from
 * @param len	The length of the memory block.
 * @return	Returns the destination address
 */
void *	memcpy(	void *dst, const void *src, size_t len );

/**
 * @brief Sets a block of memory to a certain value
 * @param dest	The memory block to set
 * @param val	The value to write ( an unsigned char )
 * @param len	The length of the memory block.
 * @return	Returns the destination address
 */
void *	memset(	void *dst, int val, size_t len );

/**
 * @brief Copies a block of memory terminated by an NUL char
 * @param dest	The place the data will be copied to
 * @param src	The place the data will be copied from
 * @param len	The length of the memory block.
 * @return	Returns the destination address
 */
void *	strncpy( void *dst, const void *src, size_t len );

/**
 * @brief Finds the length (position of NUL char) of a string
 * @param str	The string to search
 * @return 	The length of the string.
 */
size_t	strlen( const char *str );

#endif
