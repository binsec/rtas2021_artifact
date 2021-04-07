#ifndef __ERROR_H__
#define __ERROR_H__

/* Error handling functions. */


/* Enter an infinite loop. */
void __attribute__((noreturn))
error_infinite_loop(void);

/* Fatal error in the kernel. */
void __attribute__ ((format (printf, 1, 2), noreturn))
fatal(char * format,...);

/* Called when a task enters the kernel in a wrong state (e.g. invalid arguments). */
void __attribute__ ((format (printf, 1, 2), noreturn))
task_fatal(char * format,...);


#define assert(x) if(!(x)) fatal("Assertion " #x " failed")


#endif
