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
 * @file arch/i386/pic.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 *  	  2010 - Created
 *  03-04-2014 - Cleaned up
 *  18-06-2015 - Commented
 *  18-06-2015 - Ported to pos-kernel, added GPL license header
 */


#include <arch/i386/pic.h>
#include <arch/i386/x86.h>

/**
 * Internal function, sends a command to one of the PIC's
 * @param cmd		The command code to send to the PIC
 * @param picNum	The PIC to send the command to (0 or 1)
 */
void i386_pic_send_command ( uint8_t cmd, uint8_t picNum )
{
	uint8_t	reg;
	
	// Select the right port to send the command to
	reg = (picNum==1) ? I386_PIC2_REG_COMMAND : I386_PIC1_REG_COMMAND;
	
	// Bail out if the PIC index was out of bounds
	if (picNum > 1)
		return;
		
	// Write the command to the port
	i386_outb (reg, cmd);
	
}

/**
 * Internal function, sends data to one of the PIC's
 * @param cmd		The data byte to send to the PIC
 * @param picNum	The PIC to send the command to (0 or 1)
 */
void i386_pic_send_data (uint8_t data, uint8_t picNum)
{
	uint8_t	reg;
	
	// Select the right port to send the data to
	reg = (picNum==1) ? I386_PIC2_REG_DATA : I386_PIC1_REG_DATA;
	
	// Bail out if the PIC index was out of bounds
	if (picNum > 1)
		return;
		
	// Write the data to the port
	i386_outb (reg, data);
}

/**
 * Internal function, reads data from one of the PIC's
 * @param picNum	The PIC to send the command to (0 or 1)
 * @return 			The data read from the PIC
 */
uint8_t i386_pic_read_data (uint8_t picNum)
{
	uint8_t	reg;
	
	// Select the right port to read the data from
	reg = (picNum==1) ? I386_PIC2_REG_DATA : I386_PIC1_REG_DATA;
	
	// Bail out if the PIC index was out of bounds
	if (picNum > 1)
		return 0;
		
	// Return the data read from the port
	return i386_inb (reg);
}

/**
 * Internal function, sends end of interrupt command to one of the PIC's
 * @param The PIC to send the command to (0 or 1)
 */
void i386_send_end_of_interrupt(int pic_id)
{
	i386_pic_send_command(I386_PIC_OCW2_MASK_EOI, pic_id);
}

/**
 * Initialize the Programmable Interrupt Controllers
 */
void i386_pic_initialize ( void )
{

	uint8_t		icw	= 0;

	//! Begin initialization of PIC

	icw = (icw & ~I386_PIC_ICW1_MASK_INIT) | I386_PIC_ICW1_INIT_YES;
	icw = (icw & ~I386_PIC_ICW1_MASK_IC4) | I386_PIC_ICW1_IC4_EXPECT;

	i386_pic_send_command (icw, 0);
	i386_pic_send_command (icw, 1);

	//! Send initialization control word 2. This is the base addresses of the irq's

	i386_pic_send_data (I386_PIC0_IRQ_BASE, 0);
	i386_pic_send_data (I386_PIC1_IRQ_BASE, 1);

	//! Send initialization control word 3. This is the connection between master and slave.
	//! ICW3 for master PIC is the IR that connects to secondary pic in binary format
	//! ICW3 for secondary PIC is the IR that connects to master pic in decimal format

	i386_pic_send_data (0x04, 0);
	i386_pic_send_data (0x02, 1);

	//! Send Initialization control word 4. Enables i86 mode

	icw = (icw & ~I386_PIC_ICW4_MASK_UPM) | I386_PIC_ICW4_UPM_86MODE;

	i386_pic_send_data (icw, 0);
	i386_pic_send_data (icw, 1);
}

/**
 * Clear the interrupt from the PIC
 * @param The interrupt request line that generated the interrupt
 */ 
void i386_interrupt_done (int id)
{
	i386_send_end_of_interrupt(0);
	if (id > 7)
		i386_send_end_of_interrupt(1);
}
