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
 * @file platinit/i386/init.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"
#include <stdint.h>
#include "core/con_early.h"
#include "util/numfmt.h"

/**
 * C entry point, called from start.s
 */
void i386_init( void *mb_info, uint32_t mb_magic )
{
	char buffer[100];

	con_early_putstr("posldr ");
	con_early_putstr(ARCH_STRING);
	con_early_putstr(" by Peter Bosch (c) 2016\n");
	
	numfmt_signed( 42, 0, 0, 10, buffer, 100 );
	con_early_putstr(buffer);
	con_early_putch('\n');
	numfmt_signed( 31337, 0, 0, 10, buffer, 100 );
	con_early_putstr(buffer);
	con_early_putch('\n');
	numfmt_signed( -123, 0, 5, 10, buffer, 100 );
	con_early_putstr(buffer);
	con_early_putch('\n');
	numfmt_signed( 123, 0, 5, 10, buffer, 100 );
	con_early_putstr(buffer);
	con_early_putch('\n');
	numfmt_signed( 123, NF_ZEROPAD | NF_SGNPLUS, 5, 10, buffer, 100 );
	con_early_putstr(buffer);
	con_early_putch('\n');
	numfmt_signed( 0x55AA, NF_ZEROPAD, 8, 16, buffer, 100 );
	con_early_putstr(buffer);
	con_early_putch('\n');
	numfmt_signed( 0x55AA, NF_ZEROPAD, 32, 2, buffer, 100 );
	con_early_putstr(buffer);
	con_early_putch('\n');
	


	for ( ;; ) ;

}
