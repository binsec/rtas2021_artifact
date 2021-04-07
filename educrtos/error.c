#include "terminal.h"
#include  "error.h"

#include <stdarg.h>
#include "lib/fprint.h"
void __attribute__ ((format (printf, 1, 2), noreturn))
fatal(char * format,...){
  va_list ap;
  va_start(ap, format);  
  vfprint(terminal_putchar, format, ap);
  va_end(ap);
  error_infinite_loop();
}

void __attribute__ ((format (printf, 1, 2), noreturn))
task_fatal(char * format,...){
  va_list ap;
  va_start(ap, format);  
  vfprint(terminal_putchar, format, ap);
  va_end(ap);
  error_infinite_loop();
}


/* An endless infinite loop, used to catch errors, even in contexts
   where there is no stack.
   MAYBE: Tell why we enter this loop in an ecx argument.  */
void __attribute__((noreturn))
error_infinite_loop(void);

asm("\
.global error_infinite_loop\n\
.type error_infinite_loop, @function\n\
error_infinite_loop:\n\
        /* Infinite loop. */\n\
	cli\n\
1:	hlt\n\
	jmp 1b\n\
");
