;Copyright (C) 2015 Peter Bosch
;
;This program is free software; you can redistribute it and/or
;modify it under the terms of the GNU General Public License
;as published by the Free Software Foundation; either version 2
;of the License, or (at your option) any later version.
;
;This program is distributed in the hope that it will be useful,
;but WITHOUT ANY WARRANTY; without even the implied warranty of
;MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;GNU General Public License for more details.
;
;You should have received a copy of the GNU General Public License
;along with this program; if not, write to the Free Software
;Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

;
; @file src/archlib/i386/ctrlregs.s
;
; Part of posnk kernel
;
; Written by Peter Bosch <peterbosc@gmail.com>
;

[BITS 32]
[section .text]

; Set CR0 to value in argument
; void i386_set_cr0( uint32_t value );
[global i386_set_cr0]

i386_set_cr0:
	mov	eax,	[esp + 4]	; Move first parameter into eax
	mov	cr0,	eax		; Write it into CR0
	ret				; Return

; Set CR3 to value in argument
; void i386_set_cr3( uint32_t value );
[global i386_set_cr3]

i386_set_cr3:
	mov	eax,	[esp + 4]	; Move first parameter into eax
	mov	cr3,	eax		; Write it into CR3
	ret				; Return

; Set CR4 to value in argument
; void i386_set_cr4( uint32_t value );
[global i386_set_cr4]

i386_set_cr4:
	mov	eax,	[esp + 4]	; Move first parameter into eax
	mov	cr4,	eax		; Write it into CR4
	ret				; Return

; Get CR0 value and return it
; uint32_t i386_get_cr0( void );
[global i386_get_cr0]
i386_get_cr0:
	mov	eax,	cr0		; Load CR0 into eax (return value)
	ret				; Return

; Get CR2 value and return it
; uint32_t i386_get_cr2( void );
[global i386_get_cr2]
i386_get_cr2:
	mov	eax,	cr2		; Load CR2 into eax (return value)
	ret				; Return

; Get CR3 value and return it
; uint32_t i386_get_cr3( void );
[global i386_get_cr3]
i386_get_cr3:
	mov	eax,	cr3		; Load CR3 into eax (return value)
	ret				; Return

; Get CR4 value and return it
; uint32_t i386_get_cr4( void );
[global i386_get_cr4]
i386_get_cr4:
	mov	eax,	cr4		; Load CR3 into eax (return value)
	ret				; Return


; vim: syntax=nasm
