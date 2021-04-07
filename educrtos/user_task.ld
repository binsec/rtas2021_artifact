/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)
 
/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	. = 0x0000;
	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.all 0x0 : /* : BLOCK(4K) : ALIGN(4K) */
	{
                /* LONG(0xFFFFFFFF)               /\* Try to catch null-pointer dereferences. *\/ */
		*(.text)
                *(.text.*)
		*(.rodata)
                *(.rodata.*)
                *(.eh_frame)         /* Used for stack unwinding, so possibly useful. */                
		*(.data)
		*(COMMON)
		*(.bss)
	}

        /* Allows the other sections to pass through.  */
        .interp : { *(.interp) }

        /DISCARD/ : { *(.comment) *(.dynamic) *(.dynstr) }

        /* We should discard these.  */
        /* /DISCARD/ : { *(.interp) *(.note.gnu.build-id) *(.dynsym)  } */



	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
