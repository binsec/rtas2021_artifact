#include "user_tasks.h"

#ifndef TASK_NUMBER
#warning "You should define a TASK_NUMBER from the command line (default 0)"
#define TASK_NUMBER 0
#endif

#define USER_STACK_SIZE 1024
#define XSTRING(x) STRING(x)
#define STRING(x) #x

/* The tasks setup their stack themselves, in assembly. */
static char user_stack[USER_STACK_SIZE] __attribute__((used, aligned(16)));
static char user_stack2[USER_STACK_SIZE] __attribute__((used, aligned(16)));

asm("\
.global _start\n\
.type _start, @function\n\
_start:\n\
        /* Setup the stack */ \n\
	mov $(user_stack + " XSTRING(USER_STACK_SIZE) "), %esp\n\
        call test_userspace\n\
        jmp user_error_infinite_loop\n\
/* setup size of _start symbol. */\n\
.size _start, . - _start\n\
");

asm("\
.global user_error_infinite_loop\n\
.type user_error_infinite_loop, @function\n\
user_error_infinite_loop:\n\
        /* Infinite loop. */\n\
1:	hlt\n\
	jmp 1b\n\
");

void
thread_create(void (*func)(void), char *stack, unsigned int stack_size){
  int res = dup_thread();
  if(res == 0){
    asm("\
mov %0, %%esp\n\
jmp *%1 \n\
" : : "g"(stack+stack_size), "r"(func) );
  }
  else return;
}

void test_func(void){
  printf("New thread!\n");
  yield(10*1000000000ULL,1000000000ULL);
  printf("New thread: ending execution");
  while(1);
}

/* The PID is passed in register (eax on x86-32bit). */
void __attribute__((used,regparm(3)))
test_userspace(int pid)  {
  putchar('0' + TASK_NUMBER);
  printf(" says hello %d %d\n", TASK_NUMBER, pid);
#ifdef DYNAMIC_TASK_CREATION  
  if(TASK_NUMBER == 0){
    thread_create(test_func,&user_stack2[0],USER_STACK_SIZE);
    printf("Dupped thread\n");
    /* int res = dup_thread(); */
    /* if(res == 0) */
    /* printf("dup thread result: %d\n", res); */
  }
#endif  
  int i = 0;
  while(1){
    i++;
    printf("task" XSTRING(TASK_NUMBER) " pid %d" ": i=%d\n", pid, i);
    yield((TASK_NUMBER + 2)*1000000000ULL,1000000000ULL);
    //asm("int $3");    
    //asm("div %%ecx" : :"c"(0));
    /* yield(3000000ULL,3000000ULL);     */
  }
  while(1);
}
