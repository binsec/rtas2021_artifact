#ifdef __ghs__    /* GreenHills */
        .section    .startup, axv
		.vle
 
        .align 3  ;# 8 bytes
		
		.extern   main
		.extern __SRAM_SIZE
		.extern __SRAM_BASE_ADDR
		.extern __LOCAL_DMEM_SIZE
		.extern __LOCAL_DMEM_BASE_ADDR
		.extern __LOCAL_IMEM_SIZE
		.extern __LOCAL_IMEM_BASE_ADDR
		.extern __DATA_SIZE
		.extern __DATA_ROM_ADDR
		.extern __DATA_SRAM_ADDR
		.extern __SDATA_SIZE
		.extern __SDATA_ROM_ADDR
		.extern __SDATA_SRAM_ADDR
		.extern __BSS_START                 
        .extern __BSS_END                 
        .extern __BSS_SIZE
		.extern __SP_INIT
		.extern _SDA_BASE_
		.extern _SDA2_BASE_
		.extern sys_init
		.extern sys_halt
#endif
#ifdef __GNUC__   /* GCC */
		.section .startup, "ax"
#endif
#ifdef __DCC__ /* Diab */
       .section ".startup",4,rx
#endif
        .globl	_start
        
_start:

;#********************************* Enable BTB ********************************
;# Flush & Enable BTB - Set BBFI bit in BUCSR
		e_li	r3, 0x201
		mtspr	1013, r3
		se_isync

;#**************************** Init Core Registers ****************************
;# The E200Z4 core needs its registers initialising before they are used
;# otherwise in Lock Step mode the two cores will contain different random data.
;# If this is stored to memory (e.g. stacked) it will cause a Lock Step error.

;# GPRs 0-31
		e_li	r0, 0
		e_li	r1, 0
		e_li	r2, 0
		e_li	r3, 0
		e_li	r4, 0
		e_li	r5, 0
		e_li	r6, 0
		e_li	r7, 0
		e_li	r8, 0
		e_li	r9, 0
		e_li	r10, 0
		e_li	r11, 0
		e_li	r12, 0
		e_li	r13, 0
		e_li	r14, 0
		e_li	r15, 0
		e_li	r16, 0
		e_li	r17, 0
		e_li	r18, 0
		e_li	r19, 0
		e_li	r20, 0
		e_li	r21, 0
		e_li	r22, 0
		e_li	r23, 0
		e_li	r24, 0
		e_li	r25, 0
		e_li	r26, 0
		e_li	r27, 0
		e_li	r28, 0
		e_li	r29, 0
		e_li	r30, 0
		e_li	r31, 0

;# Init any other CPU register which might be stacked (before being used).

		mtspr	1, r1		;#XER
		mtcrf	0xFF, r1
		mtspr	CTR, r1
		mtspr	272, r1		;#SPRG0
		mtspr	273, r1		;#SPRG1
		mtspr	274, r1		;#SPRG2
		mtspr	275, r1		;#SPRG3
		mtspr	58, r1		;#CSRR0
		mtspr	59, r1		;#CSRR1
		mtspr	570, r1		;#MCSRR0
		mtspr	571, r1		;#MCSRR1
		mtspr	61, r1		;#DEAR
		mtspr	63, r1		;#IVPR
		mtspr	256, r1		;#USPRG0
		mtspr	62, r1		;#ESR
		mtspr	8,r31		;#LR

;#*************** Load Initialised Data Values from Flash into RAM ************/
;# Initialised Data - ".data"
DATACOPY:
    e_lis       r9, __DATA_SIZE@ha      # Load upper SRAM load size (# of bytes) into R9
    e_or2i      r9, __DATA_SIZE@l       # Load lower SRAM load size into R9                                     
    e_cmp16i    r9,0                    # Compare to see if equal to 0                                  
    e_beq       SDATACOPY               # Exit cfg_ROMCPY if size is zero (no data to initialise)
                                        
    mtctr       r9                      # Store no. of bytes to be moved in counter
                                        
    e_lis       r10, __DATA_ROM_ADDR@h  # Load address of first SRAM load into R10
    e_or2i      r10, __DATA_ROM_ADDR@l  # Load lower address of SRAM load into R10
    e_subi      r10,r10, 1              # Decrement address to prepare for ROMCPYLOOP

    e_lis       r5, __DATA_SRAM_ADDR@h  # Load upper SRAM address into R5 (from linker file)
    e_or2i      r5, __DATA_SRAM_ADDR@l  # Load lower SRAM address into R5 (from linker file)
    e_subi      r5, r5, 1               # Decrement address to prepare for ROMCPYLOOP

DATACPYLOOP:
    e_lbzu      r4, 1(r10)              # Load data byte at R10 into R4,incrementing (update) ROM address
    e_stbu      r4, 1(r5)               # Store R4 data byte into SRAM at R5 and update SRAM address 
    e_bdnz      DATACPYLOOP             # Branch if more bytes to load from ROM

;# Small Initialised Data - ".sdata" 
SDATACOPY:    
    e_lis       r9, __SDATA_SIZE@ha     # Load upper SRAM load size (# of bytes) into R9
    e_or2i      r9, __SDATA_SIZE@l      # Load lower SRAM load size into R9                                     
    e_cmp16i    r9,0                    # Compare to see if equal to 0                                  
    e_beq       ROMCPYEND               # Exit cfg_ROMCPY if size is zero (no data to initialise)
                                        
    mtctr       r9                      # Store no. of bytes to be moved in counter
                                        
    e_lis       r10, __SDATA_ROM_ADDR@h # Load address of first SRAM load into R10
    e_or2i      r10, __SDATA_ROM_ADDR@l # Load lower address of SRAM load into R10
    e_subi      r10,r10, 1              # Decrement address to prepare for ROMCPYLOOP

    e_lis       r5, __SDATA_SRAM_ADDR@h # Load upper SRAM address into R5 (from linker file)
    e_or2i      r5, __SDATA_SRAM_ADDR@l # Load lower SRAM address into R5 (from linker file)
    e_subi      r5, r5, 1               # Decrement address to prepare for ROMCPYLOOP

SDATACPYLOOP:
    e_lbzu      r4, 1(r10)              # Load data byte at R10 into R4,incrementing (update) ROM address
    e_stbu      r4, 1(r5)               # Store R4 data byte into SRAM at R5 and update SRAM address 
    e_bdnz      SDATACPYLOOP            # Branch if more bytes to load from ROM
    
    
ROMCPYEND:

;#*************************** Enable ME Bit in MSR *****************************
		mfmsr	r6
		e_or2i	r6,0x1000
		mtmsr	r6

;#****************************** Initialize BSS section ******************************/
bss_Init:
    e_lis        r9, __BSS_SIZE@h       # Load upper BSS load size (# of bytes) into R9
    e_or2i       r9, __BSS_SIZE@l       # Load lower BSS load size into R9 and compare to zero
    e_cmp16i     r9,0
    e_beq        bss_Init_end           # Exit if size is zero (no data to initialise)

    mtctr        r9                     # Store no. of bytes to be moved in counter

    e_lis        r5, __BSS_START@h      # Load upper BSS address into R5 (from linker file)
    e_or2i       r5, __BSS_START@l      # Load lower BSS address into R5 (from linker file)
    e_subi       r5, r5, 1              # Decrement address to prepare for bss_Init_loop

    e_lis        r4, 0x0

bss_Init_loop:
    e_stbu       r4, 1(r5)              # Store zero byte into BSS at R5 and update BSS address
    e_bdnz       bss_Init_loop          # Branch if more bytes to load

bss_Init_end:

;#****************************** Configure Stack ******************************/
		e_lis	r1, __SP_INIT@h	;# Initialize stack pointer r1 to
		e_or2i	r1, __SP_INIT@l	;# value in linker command file.

		e_lis	r13, _SDA_BASE_@h	;# Initialize r13 to sdata base
		e_or2i	r13, _SDA_BASE_@l	;# (provided by linker).

		e_lis	r2, _SDA2_BASE_@h	;# Initialize r2 to sdata2 base
		e_or2i	r2, _SDA2_BASE_@l	;# (provided by linker).

		e_stwu	r0,-64(r1)			;# Terminate stack.
 
;# perform hardware and system init prior to main
		e_bl   sys_init
;# Jump to Main
		e_bl   main
		e_bl   exit
