#ifndef  __FPRINT_H__
#define  __FPRINT_H__

#include <stdarg.h>

/* A helper function to print strings, with no buffering, one
   character at a time. */

/* This is a function that can be used directly. */
void __attribute__ ((format (printf, 2, 3)))
fprint(void (*putchar)(unsigned char), char * format,...);

/* This is a function that helps writing printf-like functions. */
void
vfprint(void (*putchar)(unsigned char), char * format,va_list ap);


#endif
