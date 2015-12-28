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
 * @file crt/assert.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"
#include "util/numfmt.h"
#include "core/con_early.h"
#include <assert.h>

void assert_fail(	const char *cond, 
			const char *file, 
			int line, 
			const char *func )
{
	#ifdef OPT_util
		char buffer[8];
	#endif
	#ifdef OPT_con_early
		con_early_putstr("Assertion failed: ");
		con_early_putstr(cond);
		con_early_putstr(" in ");
		con_early_putstr(file);
		#ifdef OPT_util
			con_early_putch(':');
			numfmt_signed(line, 0, 7, 10, buffer, 8);
			con_early_putstr(buffer);
		#endif
		con_early_putch(' ');
		con_early_putstr(func);
		con_early_putstr("()\n");
	#endif
	#ifdef OPT_panic
		panic();
	#else
		for (;;);
	#endif
}
