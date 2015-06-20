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
/* 
 * arch/i386/idt.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 2010       - Created
 * 02-04-2014 - Cleaned up
 */

#ifndef __ARCH_I386_IDT_H__
#define __ARCH_I386_IDT_H__

#ifdef __cplusplus
extern "C"{
#endif
#include <stdint.h>

#define I386_IDT_PRESENT_BIT			(1<<7)
#define I386_IDT_SUPERVISOR_MASK		(0<<5)
#define I386_IDT_DRIVER_0_MASK			(1<<5)
#define I386_IDT_DRIVER_1_MASK			(2<<5)
#define I386_IDT_USERMODE_MASK			(3<<5)
#define I386_IDT_SYSTEM_SEGMENT_BIT		(1<<4)
#define I386_IDT_32_BIT_GATE_BIT		(1<<3)
#define I386_IDT_RESERVED_BIT			(1<<2)
#define I386_IDT_NOT_TASK_GATE			(1<<1)
#define I386_IDT_NOT_INTERRUPT_GATE  		(1<<0)
#define I386_IDT_TRAP_GATE_MASK			(IDT_NOT_TASK_GATE | IDT_NOT_INTERRUPT_GATE)
#define I386_IDT_TASK_GATE_MASK			(IDT_NOT_INTERRUPT_GATE)
#define I386_IDT_INTERRUPT_GATE_MASK		(IDT_NOT_TASK_GATE)
#define I386_IDT_32BIT_INTERRUPT		(0x8E)

#define I386_IDT_BS_32				(IDT_PRESENT_BIT | IDT_SUPERVISOR_MASK | IDT_32_BIT_GATE_BIT | IDT_RESERVED_BIT |IDT_INTERRUPT_GATE_MASK)

#define I386_IDT_32BIT_TRAP			(IDT_PRESENT_BIT | IDT_SUPERVISOR_MASK | IDT_32_BIT_GATE_BIT | IDT_RESERVED_BIT | IDT_TRAP_GATE_MASK)

#define I386_IDT_MAX_DESCRIPTORS		(256)

struct i386_idt_descriptor {
	uint16_t		offsetLo;
	uint16_t		segmentOrTSS;
	uint8_t			reserved;
	uint8_t			flags;
	uint16_t		offsetHi;
}  __attribute__((packed));

typedef struct i386_idt_descriptor i386_idt_descriptor_t;

struct i386_idtr {
	uint16_t		m_limit;
	uint32_t		m_base;
}  __attribute__((packed));

typedef struct i386_idtr i386_idtr_t;

void i386_load_idt( void );
void i386_idt_initialize( void );
void i386_idt_set_descriptor(uint8_t id, void *isr, uint8_t flags, uint32_t segment);

#ifdef __cplusplus
}
#endif

#endif
