[BITS 32]       ; 32 bit cod
[section .start] 

[global i386_start]  ; make 'start' function global
[extern i386_init]  ; our C kernel main

; the kernel entry point
i386_start:

	; back up multiboot parameters

	mov ecx, ebx
	mov edx, eax
	
	mov esp, i386_sys_stack ; set up a new stack for our kernel
	mov ebp, 0xCAFE57AC	; set ebp as a token for the tracer
	; push the multiboot parameters
	
	push edx
	push ecx
 
	call i386_init ; jump to our C kernel ;)
 
	; just a simple protection...
	jmp $

[section .bss]

	resb 0x1000
i386_sys_stack:	; our kernel stack
