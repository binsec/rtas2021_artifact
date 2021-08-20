#include <inttypes.h>

#define IO_PIPE_ADDR 0xe0000

void putchr( char ch ) { (*(uint32_t volatile*)IO_PIPE_ADDR) = ch; }

char getchr() { return (char)(*(uint32_t volatile*)IO_PIPE_ADDR); }

void sendnum( unsigned num )
{
  unsigned digit = num % 10, decade = num / 10;
  if (decade) sendnum(decade);
  putchr('0'+digit);
}

void _start()
{
  /*guessing a number in the range [1..999]*/
  unsigned lo = 0, hi=1000;
  while ((hi-lo) > 1)
    {
      unsigned mid = (lo+hi)/2;
      sendnum(mid); putchr('?'); putchr('\n');
      char ch;
      while ('\n' != (ch = getchr()))
        {
          if      (ch == '>') { lo = mid; }
          else if (ch == '<') { hi = mid; }
          else if (ch == '=') { putchr('b'); putchr('y'); putchr('e'); putchr('\n'); return; }
        }
    }
  return;
}
