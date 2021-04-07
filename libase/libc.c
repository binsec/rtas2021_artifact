#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

int rand_int(void);
unsigned char rand_char(void);

int tcsetattr(int fd, int optional_actions,
              struct termios *termios_p){
  unsigned char *ptr;
  for(ptr=termios_p;ptr < termios_p + 1; ptr++)
    *ptr = rand_char();
  return rand_int();
}

int tcgetattr(int ft, struct termios *termios_p){
  unsigned char *ptr;
  for(ptr=termios_p;ptr < termios_p + 1; ptr++)
    *ptr = rand_char();
  return rand_int();
}

int snprintf(char *str, size_t size, const char *format, ...){
  unsigned char *ptr;
  for(ptr = str; ptr < str + size; ptr++) {
    *ptr = rand_char();
  };

  int v = rand_int();
  if(0 <= v && v < size) return v;
  else while(1);
}

void *memcpy(void *dst, void *source, size_t n){
  int i;
  char *dest = dst;
  char *src = source;
  for(i = 0; i < n; i++) dest[i] = src[i];
  return dest;
}

void *memset(void *dst, int c, size_t n){
  int i;
  char *dest = dst;
  for(i = 0; i < n; i ++) dest[i] = c;
  return dest;
}
