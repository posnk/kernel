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
 * @file arch/i386/idt.c
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 *  	  2010 - Created
 *  18-06-2015 - Commented
 *  18-06-2015 - Ported to pos-kernel, added GPL license header
 */

#include <string.h>
#include <arch/i386/idt.h>
#include <arch/i386/x86.h>
#include <arch/i386/isr_entry.h>
#include <core/core.h>

#define I386_KERNEL_CODE_SEGMENT					0x18

#define ADD_ISR_ENTRY(NO)	i386_idt_set_descriptor(NO, &i386_isr_entry_ ## NO,\
													I386_IDT_32BIT_INTERRUPT, \
													I386_KERNEL_CODE_SEGMENT);

i386_idt_descriptor_t		i386_idt[I386_IDT_MAX_DESCRIPTORS];
i386_idtr_t					i386_idt_pointer;

/**
 * Internal function, loads the IDTR with the i386_idt_pointer
 */
void i386_load_idt(){
	asm volatile("lidt %0"::"m" (i386_idt_pointer));
}

/**
 * Trap for any uninitialized interrupt gates
 */
void i386_isr_entry_undefined( void )
{
	
	// Panic
	core_panic("uinitialized interrupt gate fired");
	
	//TODO: Handle invalid software interrupts by oopsing and reporting a signal
	
	// Fallback block
	for (;;);
	
}

/**
 * Fills an IDT entry
 * @warning	The ISR passed to this function must save and restore all registers
 * 			it uses.
 * @warning The descriptor set by this function might not become active until 
 * 			the IDTR has been reloaded using i386_load_idt
 * 
 * @param id		The IDT slot to fill, corresponds to the interrupt number
 * @param isr		A pointer to the interrupt service routine entry point
 * @param flags 	The IDT gate flags describing how to handle this interrupt
 * @param segment	The code segment (or task state segment) the ISR is in
 */
void i386_idt_set_descriptor(	uint8_t id, 
								void *isr, 
								uint8_t flags, 
								uint32_t segment)
{
	
	i386_idt_descriptor_t *idt_d;	
	uint32_t isr_i;
	
	// Acquire a pointer to the descriptor to fill	
	idt_d = &i386_idt[id];
	
	// For convenience, pre-cast the id to uint32_t
	isr_i = (uint32_t) isr;
	
	// Clear the descriptor
	memset(idt_d, 0, sizeof(i386_idt_descriptor_t));
	
	// Set the ISR offset low word
	idt_d->offsetLo		= (uint16_t) ( isr_i		& 0xFFFF);	
	
	// Set the ISR offset high word
	idt_d->offsetHi		= (uint16_t) ((isr_i >> 16) & 0xFFFF);
	
	// Set the segment field of the descriptor
	idt_d->segmentOrTSS	= segment;
	
	// Set the flags field of the descriptor
	idt_d->flags		= flags;
	
}

/**
 * Loads a valid IDT that has all needed gates correctly set up
 */
void i386_idt_initialize( void )
{
	int c;
	
	// Setup the IDT pointer structure to point to the IDT we are initializing
	i386_idt_pointer.m_base  = (uint32_t) i386_idt;
	i386_idt_pointer.m_limit = (I386_IDT_MAX_DESCRIPTORS * 
								sizeof(i386_idt_descriptor_t)) - 1;
								
	// Fill the IDT with uninitialized gate traps
	for (c = 0;c < I386_IDT_MAX_DESCRIPTORS;c++)
		i386_idt_set_descriptor(	(uint8_t) c, 
									&i386_isr_entry_undefined, 
									I386_IDT_32BIT_INTERRUPT, 
									I386_KERNEL_CODE_SEGMENT);

	// Fill the processor exception interrupt handlers
	ADD_ISR_ENTRY(0)
	ADD_ISR_ENTRY(1)
	ADD_ISR_ENTRY(3)
	ADD_ISR_ENTRY(4)
	ADD_ISR_ENTRY(5)
	ADD_ISR_ENTRY(6)
	ADD_ISR_ENTRY(7)
	ADD_ISR_ENTRY(8)
	ADD_ISR_ENTRY(9)
	ADD_ISR_ENTRY(10)
	ADD_ISR_ENTRY(11)
	ADD_ISR_ENTRY(12)
	ADD_ISR_ENTRY(13)
	ADD_ISR_ENTRY(14)
	ADD_ISR_ENTRY(16)
	
	// Fill the hardware interrupt handlers
	ADD_ISR_ENTRY(32)
	ADD_ISR_ENTRY(33)
	ADD_ISR_ENTRY(34)
	ADD_ISR_ENTRY(35)
	ADD_ISR_ENTRY(36)
	ADD_ISR_ENTRY(37)
	ADD_ISR_ENTRY(38)
	ADD_ISR_ENTRY(39)
	ADD_ISR_ENTRY(40)
	ADD_ISR_ENTRY(41)
	ADD_ISR_ENTRY(42)
	ADD_ISR_ENTRY(43)
	ADD_ISR_ENTRY(44)
	ADD_ISR_ENTRY(45)
	ADD_ISR_ENTRY(46)
	ADD_ISR_ENTRY(47)

	// Fill the syscall gate
	i386_idt_set_descriptor(0x80,
							&i386_isr_entry_80h, 
							0xEE, 
							I386_KERNEL_CODE_SEGMENT);

	// Load the newly generated IDT into the processor
	i386_load_idt();
	
}

