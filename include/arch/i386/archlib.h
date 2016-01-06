/******************************************************************************\
Copyright (C) 2015,2016 Peter Bosch

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
 * @file include/arch/i386/archlib.h
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#ifndef __archlib__
#define __archlib__
#include "config.h"
#include <stdint.h>

void i386_out8( uint16_t port, uint8_t data );
void i386_out16( uint16_t port, uint16_t data );
void i386_out32( uint16_t port, uint32_t data );
void i386_out_array8( uint16_t port, uint8_t *data, uint32_t count );
void i386_out_array16( uint16_t port, uint16_t *data, uint32_t count );
void i386_out_array32( uint16_t port, uint32_t *data, uint32_t count );
uint8_t i386_in8( uint16_t port );
uint16_t i386_in16( uint16_t port );
uint32_t i386_in32( uint16_t port );
void i386_in_array8( uint16_t port, uint8_t *data, uint32_t count );
void i386_in_array16( uint16_t port, uint16_t *data, uint32_t count );
void i386_in_array32( uint16_t port, uint32_t *data, uint32_t count );

void i386_set_cr0( uint32_t value );
void i386_set_cr3( uint32_t value );
void i386_set_cr4( uint32_t value );
uint32_t i386_get_cr0( void );
uint32_t i386_get_cr2( void );
uint32_t i386_get_cr3( void );
uint32_t i386_get_cr4( void );

void i386_invtlb_addr( void *addr );
void i386_invtlb_all( void );

void i386_cli( void );
void i386_sti( void );

#endif
