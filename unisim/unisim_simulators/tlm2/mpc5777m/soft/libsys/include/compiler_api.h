/**************************************************************************
 * FILE NAME: compiler_api.h                COPYRIGHT (c) Freescale 2016  *
 *                                                All Rights Reserved     *
 * DESCRIPTION:  MPC57xx compiler specific file                           *
 *                                                                        *
***************************************************************************/
#ifndef _COMPILER_API_H_
#define _COMPILER_API_H_

#if !defined(__GNUC__) && !defined(__ghs__) && !defined (__DCC__)
#error "Compiler is not defined"
#endif

#define stringify(s) tostring(s)
#define tostring(s) #s

/* definitions for assembler functions */

#ifdef __GNUC__   /* GCC */
#define PPCASM             __asm__
#define MFSPR( rn )        ({unsigned int rval; PPCASM volatile("mfspr %0," stringify(rn) : "=r" (rval)); rval;})
#define MTSPR(rn, v)       PPCASM volatile("mtspr " stringify(rn) ",%0" : : "r" (v))
#define MFDCR( rn )        ({unsigned int rval; PPCASM volatile("mfdcr %0," stringify(rn) : "=r" (rval)); rval;})
#define MTDCR(rn, v)       PPCASM volatile("mtdcr " stringify(rn) ",%0" : : "r" (v))
#endif

#ifdef __ghs__    /* GreenHills */
#include <ppc_ghs.h>    /* for intrinsic functions only */
#define PPCASM   asm
#define MFSPR( spr )       __MFSPR( spr )
#define MTSPR( spr, val )  __MTSPR( spr, val )
#define MFDCR( dcr )       __MFDCR( dcr )
#define MTDCR( dcr, val )  __MTDCR( dcr, val )
#endif

#ifdef __DCC__ /* Diab */
#include <diab/ppcasm.h>    /* for intrinsic functions only */
#define PPCASM   asm
#define MFSPR( spr )       __mfspr( spr )
#define MTSPR( spr, val )  __mtspr( spr, val )
#define MFDCR( dcr )       __mfdcr( dcr )
#define MTDCR( dcr, val )  __mtdcr( dcr, val )
#endif

#define VEC_ALIGN          PPCASM (" .align 4 ")

#endif

#ifdef NO_PPC_WAIT
#define PPC_WAIT               do {} while(0)
#else
#define PPC_WAIT               PPCASM volatile (".long 0x7c00007c") // WARNING! Binutils 2.28 emits 0x7c00003c instead of 0x7c00007c for WAIT instruction
#endif

#define PPC_ISYNC              PPCASM volatile ("se_isync")

#define PPC_MPUWE              PPCASM volatile ("mpuwe")

#define PPC_MPURE              PPCASM volatile ("mpure")

#define LOAD_LE32(p) ({uint32_t rval; PPCASM volatile("lwbrx %0, %y1" : "=r"(rval) : "Z"(*(p))); rval;})
#define STORE_LE32(p, v) do { PPCASM volatile("stwbrx %0, %y1" : : "r" (v), "Z"(*(p))); } while(0)
