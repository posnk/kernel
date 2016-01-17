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
 * @file core/i386/vga_early.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#include "config.h"
#include "arch/i386/archlib.h"
#include <stdint.h>
#include <string.h>

int	pos_x = 0, pos_y = 0;
int	vgac_early_init = 0;

#define VGA_IDX_REG	( 0x3D4 )
#define VGA_DATA_REG	( 0x3D5 )
#define VGA_ADDR	( ( uint16_t * ) 0xB8000 )

void	vga_early_set_cursor( int col, int row )
{
	int pos = row * 80 + col;
	i386_out8( VGA_IDX_REG, 0x0F );
	i386_out8( VGA_DATA_REG, ( uint8_t) ( pos & 0xFF ) );
	i386_out8( VGA_IDX_REG, 0x0E );
	i386_out8( VGA_DATA_REG, ( uint8_t ) ( (pos >> 8) & 0xFF ) );
}

void	vga_early_set_char( char glyph, uint8_t attrib )
{
	VGA_ADDR[ pos_x + pos_y * 80 ] = glyph | attrib << 8;
	
}

void	vga_early_scroll( void )
{
	int row;
	for ( row = 0; row < 24; row++ )
		memcpy( &VGA_ADDR[ row * 80 ], &VGA_ADDR[ row * 80 + 80 ], 160);
	memset( &VGA_ADDR[ row * 80 ], 0, 160 );
}

void	con_early_putch( char c )
{
	int pos;
	if ( !vgac_early_init ) {
		i386_out8( VGA_IDX_REG, 0x0F );
		pos  = i386_in8(VGA_DATA_REG ) & 0xFF;
		i386_out8( VGA_IDX_REG, 0x0E );
		pos |= (i386_in8( VGA_DATA_REG ) << 8) & 0xFF00;
		pos &= 0xFFFF;
		pos_x = pos % 80;
		pos_y = pos / 80;
		if ( pos_y > 25 ) {
			pos_x = 0;
			pos_y = 0;
		}
		vgac_early_init = 1;
	}
	if ( c == '\n' ) {
		if ( pos_y == 24 )
			vga_early_scroll();
		else
			pos_y++;
		pos_x = 0;
	} else if ( c == '\b' ) {
		if ( pos_x == 0 ) {
			if ( pos_y != 0 ){
				pos_y--;
				pos_x = 79;
			}
		} else
			pos_x--;
	} else {
		vga_early_set_char( c, 0x0F );
		if ( pos_x == 79 ) {
			pos_x = 0;
			if ( pos_y == 24 )
				vga_early_scroll();
			else
				pos_y++;
		} else
			pos_x++;	
	}
	vga_early_set_cursor(pos_x, pos_y );
}

void	con_early_putstr( const char *str )
{
	char c;
	while ( (c = *str++) != 0 ) 
		con_early_putch( c );
}
