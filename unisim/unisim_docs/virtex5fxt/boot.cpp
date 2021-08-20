#include "ppc44x_regs.h"
#define KERNEL_START 0x00000000
#define DEV_TREE_PTR 0x00800000
#define INITRD_START 0x00900000
#define INITRD_END 0x01900000
#define TLB0 0x00000250  /* map 0x00000000 (256 MB) in system address space */
#define TLB1 0x0         /* at physical address 0x0 */
#define TLB2 0x7         /* with access control SX=1 SW=1 SR=1 */

#define MI_CONTROL 0x10
#define MI_CONTROL_ENABLE 0x8000

	.section ".text"
	.org 0

init:
	mfdcr r0, MI_CONTROL
	oris r0, r0, MI_CONTROL_ENABLE
	mtdcr MI_CONTROL, r0    /* enable MCI */
	li r0, 0          /* TLB entry #0 */
	lis r8, TLB0@h
	ori r8, r8, TLB0@l
	li r9, 0
	mtspr MMUCR, r9   /* MMUCR[STID] = 0 */
	lis r10, TLB1@h
	ori r10, r10, TLB1@l
	lis r11, TLB2@h
	ori r11, r11, TLB2@l
	tlbwe r8, r0, 0   /* Set EPN, V, TS, SIZE, TID of UTLB entry #0 word #0 */
	tlbwe r10, r0, 1  /* Set RPN, ERPN of UTLB entry #0 word #1 */
	tlbwe r11, r0, 2  /* Set U0 U1 U2 U3 W I M G E UX UW UR SX SW SR of UTLB entry #0 word #2 */
	lis r3, DEV_TREE_PTR@h
	ori r3, r3, DEV_TREE_PTR@l /* r3 <- device tree address */
	lis r4, INITRD_START@h
	ori r4, r4, INITRD_START@l /* r4 <- initrd start */
	lis r5, INITRD_END@h
	ori r5, r5, INITRD_END@l  /* r5 <- initrd end */
	li r6, 0 /* r6 <- start of kernel command line (unused as kernel command line is in dev tree) */
	li r7, 0 /* r7 <- end of kernel command line (unused as kernel command line is in dev tree) */
	lis r0, KERNEL_START@h
	ori r0, r0, KERNEL_START@l
	mtspr SRR0, r0    /* SRR0 <- kernel start address */
	li r1, 0
	mtspr SRR1, r1    /* MSR <- 0 */
	rfi   /* Branch to Linux kernel and invalidate shadow TLBs */
	
	.global  start
	.org 0xffc
start:
	b init
