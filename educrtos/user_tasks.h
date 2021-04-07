#ifndef __USER_TASKS_H__
#define __USER_TASKS_H__

#include "low_level.h"
#include "timer.h"

enum syscalls {
   SYSCALL_YIELD,
   SYSCALL_PUTCHAR,
   SYSCALL_DUP_THREAD,
   SYSCALL_TRIGGER_BUG,   
   SYSCALL_NUMBER
   /* SYSCALL_SLEEP = 0x33 */
};

static inline void
yield(duration_t next_wakeup, duration_t next_deadline){
  uint32_t next_wakeup_high = (uint32_t) (next_wakeup >> 32ULL);
  uint32_t next_wakeup_low = (uint32_t) (next_wakeup & 0xFFFFFFFFULL);  

  uint32_t next_deadline_high = (uint32_t) (next_deadline >> 32ULL);
  uint32_t next_deadline_low = (uint32_t) (next_deadline & 0xFFFFFFFFULL);  

  syscall5(SYSCALL_YIELD, next_wakeup_high, next_wakeup_low, next_deadline_high, next_deadline_low);
}

static inline void putchar(unsigned char x){
  syscall2(SYSCALL_PUTCHAR, x);
}

static inline uint32_t dup_thread(void){
  return syscall1_1(SYSCALL_DUP_THREAD);
}


#include "lib/fprint.h"
#define printf(...) fprint(putchar, __VA_ARGS__)

struct task_description {
  /* The contexts of the task. */
  struct context * const context;
  uint32_t const start_pc;
  char* const task_begin;
  char* const task_end;
#ifdef FP_SCHEDULING     
  unsigned int const priority;
#endif   
};

/* High-level description of the application. */
extern const struct user_tasks_image {
  unsigned int const nb_tasks;
  unsigned int const nb_contexts;
  struct task_description const *const tasks;
  struct low_level_description const low_level;
#ifdef SCHED_PRIORITY_QUEUE_USE_HEAP
  struct context ** const ready_heap_array;
  struct context ** const waiting_heap_array;  
#endif
  char *idle_start;        /* Bound for the idle code. */
  char *idle_end;        /* Bound for the idle code. */  
  struct context * const idle_ctx_array;
  struct context * reserve;
} user_tasks_image;

/* Provided by the application */


#endif  /* __USER_TASKS_H__ */
