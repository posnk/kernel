;Copyright (C) 2015,2016 Peter Bosch
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
; @file src/archlib/i386/portio.s
;
; Part of posnk kernel
;
; Written by Peter Bosch <peterbosc@gmail.com>
;

[BITS 32]
[section .text]

; Writes a byte to a port 
; void i386_out8( uint16_t port, uint8_t data );
[global i386_out8]

i386_out8:
	mov	edx,	[esp + 4]	; Move 1st parameter into edx
	mov	eax,	[esp + 8]	; Move 2nd parameter into eax
	out	dx,	al		; Write to port
	ret				; Return

; Writes a short to a port 
; void i386_out16( uint16_t port, uint16_t data );
[global i386_out16]

i386_out16:
	mov	edx,	[esp + 4]	; Move 1st parameter into edx
	mov	eax,	[esp + 8]	; Move 2nd parameter into eax
	out	dx,	ax		; Write to port
	ret				; Return

; Writes an int to a port 
; void i386_out32( uint16_t port, uint32_t data );
[global i386_out32]

i386_out32:
	mov	edx,	[esp + 4]	; Move 1st parameter into edx
	mov	eax,	[esp + 8]	; Move 2nd parameter into eax
	out	dx,	eax		; Write to port
	ret				; Return

; Writes a byte array to a port
; void i386_out_array8( uint16_t port, uint8_t *data, uint32_t count );
[global i386_out_array8]

i386_out_array8:
	push	ebp			; Preserve EBP
	mov	ebp,	esp		; Update EBP
	push	esi			; Preserve ESI
	push	ecx			; Preserve ECX
	mov	edx,	[ebp + 8]	; Move 1st parameter into edx
	mov	esi,	[ebp + 12]	; Move 2nd parameter into esi
	mov	ecx,	[ebp + 16]	; Move 3rd parameter into ecx
	rep	outsb			; Write string
	pop	ecx			; Restore ECX
	pop	esi			; Restore ESI
	mov	esp,	ebp		; Restore ESP
	pop	ebp			; Restore EBP
	ret				; Return

; Writes a short array to a port
; void i386_out_array16( uint16_t port, uint16_t *data, uint32_t count );
[global i386_out_array16]

i386_out_array16:
	push	ebp			; Preserve EBP
	mov	ebp,	esp		; Update EBP
	push	esi			; Preserve ESI
	push	ecx			; Preserve ECX
	mov	edx,	[ebp + 8]	; Move 1st parameter into edx
	mov	esi,	[ebp + 12]	; Move 2nd parameter into esi
	mov	ecx,	[ebp + 16]	; Move 3rd parameter into ecx
	rep	outsw			; Write string
	pop	ecx			; Restore ECX
	pop	esi			; Restore ESI
	mov	esp,	ebp		; Restore ESP
	pop	ebp			; Restore EBP
	ret				; Return

; Writes an int array to a port
; void i386_out_array32( uint16_t port, uint32_t *data, uint32_t count );
[global i386_out_array32]

i386_out_array32:
	push	ebp			; Preserve EBP
	mov	ebp,	esp		; Update EBP
	push	esi			; Preserve ESI
	push	ecx			; Preserve ECX
	mov	edx,	[ebp + 8]	; Move 1st parameter into edx
	mov	esi,	[ebp + 12]	; Move 2nd parameter into esi
	mov	ecx,	[ebp + 16]	; Move 3rd parameter into ecx
	rep	outsd			; Write string
	pop	ecx			; Restore ECX
	pop	esi			; Restore ESI
	mov	esp,	ebp		; Restore ESP
	pop	ebp			; Restore EBP
	ret				; Return

; Reads a byte from a port
; uint8_t i386_in8( uint16_t port )
[global i386_in8]
i386_in8:
	mov	edx,	[esp + 4]	; Read first parameter into edx
	in	 al,	dx		; Read port
	and	eax,	0xFF		; Make sure the other bits of eax are 0
	ret

; Reads a short from a port
; uint16_t i386_in16( uint16_t port )
[global i386_in16]
i386_in16:
	mov	edx,	[esp + 4]	; Read first parameter into edx
	in	 ax,	dx		; Read port
	and	eax,	0xFFFF		; Make sure the other bits of eax are 0
	ret

; Reads an int from a port
; uint32_t i386_in32( uint16_t port )
[global i386_in32]
i386_in32:
	mov	edx,	[esp + 4]	; Read first parameter into edx
	in	eax,	dx		; Read port
	ret

; Reads a byte array from a port
; void i386_in_array8( uint16_t count, uint8_t *data, uint32_t count)
[global i386_in_array8]
i386_in_array8:
	push	ebp			; Preserve EBP
	mov	ebp,	esp		; Update EBP
	push	esi			; Preserve ESI
	push	ecx			; Preserve ECX
	mov	edx,	[ebp + 8]	; Move 1st parameter into edx
	mov	edi,	[ebp + 12]	; Move 2nd parameter into esi
	mov	ecx,	[ebp + 16]	; Move 3rd parameter into ecx
	rep	insb			; Read string
	pop	ecx			; Restore ECX
	pop	esi			; Restore ESI
	mov	esp,	ebp		; Restore ESP
	pop	ebp			; Restore EBP
	ret				; Return

; Reads a short array from a port
; void i386_in_array16( uint16_t count, uint16_t *data, uint32_t count)
[global i386_in_array16]
i386_in_array16:
	push	ebp			; Preserve EBP
	mov	ebp,	esp		; Update EBP
	push	esi			; Preserve ESI
	push	ecx			; Preserve ECX
	mov	edx,	[ebp + 8]	; Move 1st parameter into edx
	mov	edi,	[ebp + 12]	; Move 2nd parameter into esi
	mov	ecx,	[ebp + 16]	; Move 3rd parameter into ecx
	rep	insw			; Read string
	pop	ecx			; Restore ECX
	pop	esi			; Restore ESI
	mov	esp,	ebp		; Restore ESP
	pop	ebp			; Restore EBP
	ret

; Reads an int array from a port
; void i386_in_array32( uint16_t count, uint32_t *data, uint32_t count)
[global i386_in_array32]
i386_in_array32:
	push	ebp			; Preserve EBP
	mov	ebp,	esp		; Update EBP
	push	esi			; Preserve ESI
	push	ecx			; Preserve ECX
	mov	edx,	[ebp + 8]	; Move 1st parameter into edx
	mov	edi,	[ebp + 12]	; Move 2nd parameter into esi
	mov	ecx,	[ebp + 16]	; Move 3rd parameter into ecx
	rep	insd			; Read string
	pop	ecx			; Restore ECX
	pop	esi			; Restore ESI
	mov	esp,	ebp		; Restore ESP
	pop	ebp			; Restore EBP
	ret

; vim: syntax=nasm
