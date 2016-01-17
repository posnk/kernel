/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(i386_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Begin putting sections at 1 MiB, a conventional place for kernels to be
	   loaded at by the bootloader. */
	. = 1M;

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.setup BLOCK(4K) : ALIGN(4K)
	{
		i386_start_kernel = .;
		*(.multiboot)
		*(.setup)
	}
	.tsetup BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
		*(.rodata)
		*(.data)
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
		i386_end_kernel = .;
		i386_start_kheap = .;
	}
	
	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
