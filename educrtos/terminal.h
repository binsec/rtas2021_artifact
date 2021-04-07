#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include <stdint.h>

/* Low-level functions to display to the VGA terminal. */

/* Initialize the disiplay. */
void terminal_initialize(void);

/* Printf in the terminal.  */
void terminal_print(char * format,...) __attribute__ ((format (printf, 1, 2)));

/* Simpler printing functions. */
void terminal_writestring(const char* data);
void terminal_write_uint32(uint32_t num);
void terminal_putchar(unsigned char c);
#endif
