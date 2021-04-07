/*************************************************************************
*                                                                        *
*  This file is part of Codex.                                           *
*                                                                        *
*  Copyright (C) 2007-2020                                               *
*    CEA (Commissariat a l'energie atomique et aux energies              *
*         alternatives)                                                  *
*                                                                        *
*  you can redistribute it and/or modify it under the terms of the GNU   *
*  Lesser General Public License as published by the Free Software       *
*  Foundation, version 2.1.                                              *
*                                                                        *
*  It is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*  GNU Lesser General Public License for more details.                   *
*                                                                        *
*  See the GNU Lesser General Public License version 2.1                 *
*  for more details (enclosed in the file licenses/LGPLv2.1).            *
*                                                                        *
*************************************************************************/

#define CAML_NAME_SPACE
#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>

#ifdef _MSC_VER
#include <intrin.h>
#endif

__attribute__((__always_inline__))
static unsigned int clz(unsigned int v){
  /* Note: on a 64 bit platform, GCC's _builtin_clz will perform a 32
     bit operation (even if the argument has type int). We have to use
     _builtin_clzll instead. */
#if __GNUC__
  #ifdef ARCH_SIXTYFOUR  
    return __builtin_clzll(v);
  #else
    return __builtin_clz(v)
  #endif
#endif
#ifdef _MSC_VER
    int res = 0;     
  #ifdef ARCH_SIXTYFOUR  
    _BitScanForward64(&res,v);
  #else
    _BitScanForward(&res,v);
  #endif
    return res;
#endif    
}

/**************** Log2 (with rounding to the floor). ****************/
__attribute__((__always_inline__))
static uintnat inline_caml_int_builtin_log2 (value i){
  /* log2(v) is normally 32-1-clz(v), but because of the tag we
     must substract one more. */
  return (8*sizeof(value) - 2 - clz(i));
}

CAMLprim uintnat caml_int_builtin_log2 (value i){
  return inline_caml_int_builtin_log2(i);
}

CAMLprim uintnat caml_int_builtin_log2_untagged (uintnat i){
  return (8*sizeof(value) - 1 - clz(i));    
}

CAMLprim value caml_int_builtin_log2_byte (value i){
  return Val_int(inline_caml_int_builtin_log2(i));
}


/**************** Highest bit ****************/
CAMLprim uintnat caml_int_builtin_highest_bit (value i){
  return (1 << inline_caml_int_builtin_log2(i));
}

CAMLprim uintnat caml_int_builtin_highest_bit_untagged (uintnat i){
  return (1 << caml_int_builtin_log2_untagged(i));
}

CAMLprim value caml_int_builtin_highest_bit_byte (value i){
  return Val_int(1 << inline_caml_int_builtin_log2(i));
}







#if 0

/* Voire jaestreet.c aussi. */

#define CAML_NAME_SPACE
#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>

#include <stdint.h>


#ifdef _MSC_VER
/* Microsoft equivalents to GCC builtins. */


#endif

/* I am not sure what clz would mean on 31-bit integers ocaml, so we
   do not define it. Still, we can perform these computations on
   the tagged representation of the bits. */

#if 0
XXX: Je les ai mis dans c_bindings.c pour le moment, ici ca marche mal
__attribute__((__always_inline__))
static uintnat inline_caml_int_builtin_log2 (value i){
  /* log2(v) is normally 32-clz(v)-1, but because of the tag we
     already have that clz(i) is clz(int_val(i)) */
  printf("sizevalue:%d\n",sizeof(value));
  return (8*sizeof(value) - __builtin_clz(i));  
}

CAMLprim uintnat caml_int_builtin_log2 (value i){
  return inline_caml_int_builtin_log2(i);
}

CAMLprim uintnat caml_int_builtin_log2_byte (value i){
  return Val_int(inline_caml_int_builtin_log2(i));
}
#endif


/* CAMLprim CAMLweakdef value caml_int_builtin_clz_byte (value i) */
/* { */
/*   return Val_int(caml_int_builtin_clz(i)); */
/* } */






/* __attribute__((target ("bmi2"))) */
/* CAMLprim CAMLweakdef uintnat libase_popcount (uintnat i) */
/* { */
/*   return __builtin_popcountll(i); */
/* } */

/* #define LOG2(X) ((unsigned) (8*sizeof (unsigned long long) - __builtin_clzll((X)) - 1)) */

/* /\* https://stackoverflow.com/questions/11376288/fast-computing-of-log2-for-64-bit-integers *\/ */

/* /\* That's just an academical question, right? Otherwise just use _BitScanReverse64 (msvc) or __builtin_clzll (gcc) – harold Jul 7 '12 at 15:42 *\/ */


/* /\* #define Popcnt __builtin_popcountll *\/ */
/* /\* #define Bzhi __builtin_ia32_bzhi_di *\/ */
/* /\* #define Tzcnt __builtin_ctzll *\/ */
/* /\* #define Lzcnt __builtin_clzll *\/ */
/* /\* #define Blsr(x) ((x) & ((x) - 1)) *\/ */
/* /\* #define Bextr(x, y, z) __builtin_ia32_bextr_u64((x), (y) | ((z) << 8)) *\/ */


/* __attribute__((target ("bmi2"))) */
/* CAMLprim CAMLweakdef uintnat libase_log2 (uintnat i) */
/* { */
/*   return __builtin_popcountll(i); */
/* } */

#endif
