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

#ifndef __con_early__
#define __con_early__
#include "config.h"

#ifndef OPT_con_early
#define con_early_disable()
#define con_early_putch(Ch)
#define con_early_putstr(Ch)
#else

/**
 * Disable early console driver and release hardware resources
 */
void con_early_disable();

/**
 * Print a character on the early console
 * @param c	The character to print
 */
void con_early_putch( char c );

/**
 * Print a string on the early console
 * @param str	The string to print
 */
void con_early_putstr( const char *str );

#endif

#endif
