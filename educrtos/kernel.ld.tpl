/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* TODO: check if this removes the need  */
/* FORCE_COMMON_ALLOCATION */

/* Assigning sections to segment is not sufficient to put user tasks in a separate section. */
/* PHDRS */
/* { */
/*        headers PT_PHDR PHDRS ; */
/*        interp PT_INTERP ; */
/*        text PT_LOAD FILEHDR PHDRS ; */
/*        data PT_LOAD ; */
/*        dynamic PT_DYNAMIC; */
/* } */

/* Note: the alignments are not necessary, but alignment of user
   segments would be useful if we want to use big segments. */

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
	.all :
	{
        _begin_of_all = .;
          /* Multiboot section.  */ 
       		EXCLUDE_FILE(system_desc.o) *(.multiboot)
          /* Code */
		EXCLUDE_FILE(system_desc.o) *(.text .text.*)
  	  /* Read-only data. */
		EXCLUDE_FILE(system_desc.o) *(.rodata .rodata.*)
        _end_of_readonly = .;
          /* Read-write data (initialized) */
		EXCLUDE_FILE(system_desc.o) *(.data)
          /* Read-write data (non-initialized) */                
		EXCLUDE_FILE(system_desc.o) *(COMMON .bss)
        _end_of_fixed_region = .;

        /* . = ALIGN(ABSOLUTE(.),4096); */

        _begin_of_parametrized_region = .;
               system_desc.o(.rodata .rodata.* .data COMMON .bss)
        _end_of_kernel = .;
        . = . + 4;



        _start_of_user_tasks = .;
               system_desc.o(.data.task)
        _end_of_parametrized_region = .;                               
         }

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
