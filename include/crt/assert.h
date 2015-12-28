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
 * @file path/file.h
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#ifndef __assert__
#define __assert__
#include "config.h"

/**
 * Implements the handler and information output routine for assert failures
 * @param cond	The condition that was asserted
 * @param file	The source unit containing the failing assert
 * @param line	The line in that file containing the failing assert
 * @param func	The function containing the failing assert
 */
void	assert_fail( 	const char *cond, 
			const char *file, 
			int line, 
			const char *func);

/**
 * Asserts a condition, if the condition is false an exception handler will be
 * invoked.
 * @param CoNd	The asserted condition
 */
#define assert(CoNd)	do {\
	if ( ! (CoNd) )  \
		assert_fail( #CoNd, __FILE__, __LINE__, __FUNCTION__ ); \
} while ( 0 );

#endif
