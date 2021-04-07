#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "error.h"

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};
 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t * const terminal_buffer = (uint16_t *) 0xB8000;

 
__attribute__((noinline)) void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}

/*#define le(a,b) ({int res; asm ("cmp %1,%2 \n jae 1f \n mov $0,%0 \n jmp 2f \n 1f: mov $1,%0 "); })*/
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
        assert(index <= VGA_WIDTH*VGA_HEIGHT-1);
        asm ("cmp %1,%0 \n jae error_infinite_loop"::"r"(index), "i"(0x7d0));
	terminal_buffer[index] = vga_entry(c, color);
}

__attribute__((noinline)) void terminal_newline(void){
  terminal_column = 0;
  if (++terminal_row >= VGA_HEIGHT){
    /* Copy everything one row up. */
    for(unsigned int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++){
      int j = i + VGA_WIDTH;
      terminal_buffer[i] = terminal_buffer[j];
    }
    /* An clear the last line. */
    for(unsigned int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++)
      terminal_buffer[i] = vga_entry(' ', terminal_color);
    /* On which we will write again. */
    terminal_row = VGA_HEIGHT-1;
  }
}

void terminal_putchar(unsigned char c) 
{
  switch(c){
   case '\n':
     terminal_newline();
     break;
     
  default:
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column >= VGA_WIDTH) {
      terminal_newline();
    }
    break;
  }
}
 
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) 
{
  while(*data != 0) terminal_putchar(*data++);
}

void terminal_write_hexa_digit(unsigned int n){
  if(n < 10){
    terminal_putchar('0' + n);
  }
  else{
    terminal_putchar('a' - 10 + n);
  }
}

void terminal_write_uint8(uint8_t num){
  terminal_write_hexa_digit(num & 15);
  terminal_write_hexa_digit(num >> 4);

}

void terminal_write_uint32(uint32_t num){
  for(int i = 28; i >=0; i-=4){
    terminal_write_hexa_digit((num >> i)&15);
  }
}

#include <stdarg.h>
#include "lib/fprint.h"
void terminal_print(char *format, ...){
  va_list args;
  va_start(args,format);
  vfprint(terminal_putchar, format, args);
  va_end(args);
}
