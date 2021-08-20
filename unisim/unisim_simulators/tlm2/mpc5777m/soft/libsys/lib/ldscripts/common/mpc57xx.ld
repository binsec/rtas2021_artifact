/* Entry Point */
ENTRY(_start)

MEMORY
{
    m_text      : org = FLASH_BASE_ADDR     , len = FLASH_SIZE  
    m_data      : org = SRAM_BASE_ADDR      , len = SRAM_SIZE
    m_share     : org = SHARE_BASE_ADDR     , len = SHARE_SIZE
    
    local_imem  : org = LOCAL_IMEM_BASE_ADDR, len = LOCAL_IMEM_SIZE
    local_dmem  : org = LOCAL_DMEM_BASE_ADDR, len = LOCAL_DMEM_SIZE
    
    m_ramdisk   : org = RAMDISK_BASE_ADDR   , len = RAMDISK_SIZE
}

SECTIONS
{
    .startup : ALIGN(0x400)
    {
    __start = . ;
    	*(.startup)
    } > m_text
    
    .core_exceptions_table   : ALIGN(4096)
    {
      __IVPR_VALUE = . ;
      KEEP(*(.core_exceptions_table))
    } > m_text

    .intc_vector_table   : ALIGN(4096)
    {
      KEEP(*(.intc_vector_table))
    } > m_data
      
    .text : 
    {
      *(.text.startup)
      *(.text)      
      *(.text.*)
      KEEP (*(.init))
      KEEP (*(.fini))            
      . = ALIGN(16);
    } > m_text
      
    .ctors :
    {
      __CTOR_LIST__ = .;
      /* gcc uses crtbegin.o to find the start of
         the constructors, so we make sure it is
         first.  Because this is a wildcard, it
         doesn't matter if the user does not
         actually link against crtbegin.o; the
         linker won't look for a file to match a
         wildcard.  The wildcard also means that it
         doesn't matter which directory crtbegin.o
         is in.  */
      KEEP (*crtbegin.o(.ctors))
      KEEP (*crtbegin?.o(.ctors))
      /* We don't want to include the .ctor section from
         from the crtend.o file until after the sorted ctors.
         The .ctor section from the crtend file contains the
         end of ctors marker and it must be last */
      KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors))
      KEEP (*(SORT(.ctors.*)))
      KEEP (*(.ctors))
      __CTOR_END__ = .;
    } > m_text

    .dtors :
    {
      __DTOR_LIST__ = .;
      KEEP (*crtbegin.o(.dtors))
      KEEP (*crtbegin?.o(.dtors))
      KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
      KEEP (*(SORT(.dtors.*)))
      KEEP (*(.dtors))
      __DTOR_END__ = .;
    } > m_text
        
    .preinit_array :
    {
      PROVIDE_HIDDEN (__preinit_array_start = .);
      KEEP (*(.preinit_array*))
      PROVIDE_HIDDEN (__preinit_array_end = .);
    } > m_text

    .init_array :
    {
      PROVIDE_HIDDEN (__init_array_start = .);
      KEEP (*(SORT(.init_array.*)))
      KEEP (*(.init_array*))
      PROVIDE_HIDDEN (__init_array_end = .);
    } > m_text

    .fini_array :
    {
      PROVIDE_HIDDEN (__fini_array_start = .);
      KEEP (*(SORT(.fini_array.*)))
      KEEP (*(.fini_array*))
      PROVIDE_HIDDEN (__fini_array_end = .);
    } > m_text
          
    .rodata :
    {
      *(.rodata)
      *(.rodata.*)
    } > m_text
    
    .eh_frame_hdr : { *(.eh_frame_hdr) } > m_text
    .eh_frame     : { KEEP (*(.eh_frame)) } > m_text    
   
	.data   : 
	{
	  *(.data)
	  *(.data.*)
	}  > m_data AT>m_text
    
    .sdata2  :
	{  
	  *(.sdata2)
	  *(.sdata2.*)
	} > m_data AT>m_text

	.sbss2    (NOLOAD)   :
	{
      /* _SDA2_BASE_ = .; */ 
	  *(.sbss2)
	  *(.sbss2.*)
	} > m_data
    
    .sdata  :
	{
	  *(.sdata)
	  *(.sdata.*)
	} > m_data AT>m_text
    
	.bss   (NOLOAD)  :
	{
	  __BSS_START = .;
	  *(.sbss)
	  *(.sbss.*)	
      *(.bss)
      *(.bss.*)
      *(COMMON)
      __BSS_END = .;
    } > m_data
    
    .stack (NOLOAD) : ALIGN(16) 
    {
      __HEAP = . ;
      PROVIDE (_end = . );
      PROVIDE (end = . );            
      . += __HEAP_SIZE ;
      __HEAP_END = . ;
      _stack_end = . ;
      . +=  __STACK_SIZE ;
      _stack_addr = . ;
      __SP_INIT = . ;
      . += 4;
    } > local_dmem
    
    .share (NOLOAD) :
    {
	  *(.share)
    } > m_share
    
/*-------- LABELS USED IN CODE -------------------------------*/
        
/* Labels for Copying Initialised Data from Flash to RAM */
__DATA_SRAM_ADDR  = ADDR(.data);
__SDATA_SRAM_ADDR = ADDR(.sdata);

__DATA_SIZE   = SIZEOF(.data);
__SDATA_SIZE  = SIZEOF(.sdata);

__DATA_ROM_ADDR  = LOADADDR(.data);
__SDATA_ROM_ADDR = LOADADDR(.sdata);
   
/* Labels Used for Initialising SRAM ECC */
__SRAM_SIZE = SRAM_SIZE;
__SRAM_BASE_ADDR = SRAM_BASE_ADDR;
      
__LOCAL_DMEM_SIZE = LOCAL_DMEM_SIZE;
__LOCAL_DMEM_BASE_ADDR = LOCAL_DMEM_BASE_ADDR;

__LOCAL_IMEM_SIZE = LOCAL_IMEM_SIZE;
__LOCAL_IMEM_BASE_ADDR = LOCAL_IMEM_BASE_ADDR;

__BSS_SIZE    = __BSS_END - __BSS_START;      

/* Labels Used for programming MPU */
__FLASH = FLASH_BASE_ADDR;
__FLASH_END = FLASH_BASE_ADDR + FLASH_SIZE - 1;
__SRAM = SRAM_BASE_ADDR;
__SRAM_END = SRAM_BASE_ADDR + SRAM_SIZE - 1;
__LOCAL_IMEM = LOCAL_IMEM_BASE_ADDR;
__LOCAL_IMEM_END = LOCAL_IMEM_BASE_ADDR + LOCAL_IMEM_SIZE - 1;
__LOCAL_DMEM = LOCAL_DMEM_BASE_ADDR;
__LOCAL_DMEM_END = LOCAL_DMEM_BASE_ADDR + LOCAL_DMEM_SIZE - 1;
__RAMDISK = RAMDISK_BASE_ADDR;
__RAMDISK_END = RAMDISK_BASE_ADDR + RAMDISK_SIZE - 1;
__SHARE = SHARE_BASE_ADDR;
__SHARE_END = SHARE_BASE_ADDR + SHARE_SIZE - 1;
}
