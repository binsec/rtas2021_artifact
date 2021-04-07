#include "high_level.h"
#include "terminal.h"
#include "user_tasks.h"
#include "per_cpu.h"
#include "error.h"
#include <stddef.h>
#include "config.h"
#include "low_level.h"

/* Conversion from hw_context to context works because of this. */
_Static_assert(__builtin_offsetof(struct context,hw_context) == 0,
               "Hardware context must be the first field to allow type casts");


struct context *context_idle(void){
  return &user_tasks_image.idle_ctx_array[current_cpu()];
}

_Bool context_is_idle(struct context *ctx){
/* #ifdef PRIVILEGED_IDLE */
/*   return (ctx == 0); */
/* #endif */
/* #ifdef UNPRIVILEGED_IDLE */
  return (ctx == &user_tasks_image.idle_ctx_array[current_cpu()]);
/* #endif   */
}

/* /\* Switch to a context, or idle if NULL. *\/ */
/* void __attribute__((noreturn)) */
/* context_switch(struct context *ctx){ */
/* #ifdef PRIVILEGED_IDLE   */
/*   if(context_is_idle(ctx) == 0) hw_context_idle(); */
/* #endif   */
/*   hw_context_switch(&ctx->hw_context); */
/* } */


/* Prepare the arguments for the call. This prologue will be the same
   for all functions that take 5 arguments. */
extern void syscall_yield(void);
asm("\
.global syscall_yield\n\t\
.type syscall_yield, @function\n\
syscall_yield:\n\
        push %edi\n\
        push %esi\n\
        call c_syscall_yield\n\
.size syscall_yield, . - syscall_yield\n\
");


void __attribute__((regparm(3),noreturn,used)) 
c_syscall_yield(struct context *ctx,
                uint32_t next_wakeup_high, uint32_t next_wakeup_low,
                uint32_t next_deadline_high, uint32_t next_deadline_low){
  /* terminal_print("Syscall yield %x\n", ctx); */
  /* terminal_print("%d|%x %x %x %x\n", next_wakeup_high, next_wakeup_low, */
  /*        next_deadline_high, next_deadline_low); */
  duration_t incr_wakeup = ((uint64_t) next_wakeup_high << 32) + (uint64_t) next_wakeup_low;
  ctx->sched_context.wakeup_date += incr_wakeup;
#if defined(EDF_SCHEDULING) || defined (DEADLINE_MONITORING)
  duration_t incr_deadline = ((uint64_t) next_deadline_high << 32) + (uint64_t) next_deadline_low;
  ctx->sched_context.deadline = ctx->sched_context.wakeup_date + incr_deadline;
#else
  /* Suppress warnings. */
  (void) next_deadline_high; (void) next_deadline_low;
#endif  
  sched_set_waiting(ctx);
  struct context *new_ctx  = sched_choose_next();
  hw_context_switch(&new_ctx->hw_context);
}

void __attribute__((regparm(3),noreturn,used)) 
syscall_putchar(struct context *ctx, int arg1) {
  /* terminal_print("Syscall putchar %x\n", ctx); */
#ifdef DEBUG_PRINTING
  terminal_putchar(arg1);
#else
  fatal("Compiled without printing feature");
#endif
  hw_context_switch_no_idle(&ctx->hw_context);
}

__attribute__((noinline)) void copy_context(struct context *dest, struct context *src) {
#pragma GCC unroll sizeof(struct context)
  for(size_t i = 0; i < sizeof(struct context) / 4; i++) {
    asm("":::"memory");
    *((uint32_t*)dest + i) = *((uint32_t*)src + i);
  }
}


void __attribute__((regparm(3),noreturn,used)) 
syscall_trigger_bug(struct context *ctx, int arg1, int arg2, int arg3){
  arg1 = arg1;
  arg2 = arg2;
  arg3 = arg3;
#ifdef INJECT_BUG0
  /* Arbitrary jumps. */
  asm("jmp *%0" :: "r"(arg1));

#elif defined(INJECT_BUG1)
  /* Corrupt the code descriptor */
#ifdef DYNAMIC_DESCRIPTORS
  ctx->hw_context.iframe.cs = arg1;
#endif
#ifdef FIXED_SIZE_GDT
  system_gdt.user_code_descriptor = ((uint64_t)arg1 << 32) & (uint64_t)arg2;
#endif

#elif defined(INJECT_BUG2)
  /* Write to memory tables. */
#ifdef DYNAMIC_DESCRIPTORS
  ctx->hw_context.start_address = arg2;
#endif
#ifdef FIXED_SIZE_GDT
  ctx->hw_context.data_segment = arg2;
#endif

#elif defined(INJECT_BUG3)
  /* Arbitrary writes. */
  *(int *)arg2 = arg3;

#elif defined(INJECT_BUG4)
  /* Arbitrary reads. */
  { volatile int r = *(int *) arg2;  }  

#elif defined(INJECT_BUG5)
  /* Illegal opcode. */
  asm volatile("ud2" : : : "memory");

#elif defined(INJECT_BUG6)
  /* Division by zero. */
  { volatile int r = arg2/arg3;  
    printf("Result is %d", r);}
#else
  /* Do nothing. */
#endif
  hw_context_switch_no_idle(&ctx->hw_context);
}

void __attribute__((regparm(3),noreturn,used)) 
syscall_dup_thread(struct context *ctx){
#ifndef DYNAMIC_TASK_CREATION
  fatal("This kernel was not compiled with dynamic task creation; "
        "dup_thread syscall is not available\n");
#else  
  uint32_t range = ctx->range;
  //terminal_print("dup_thread %x\n",ctx);
  if(range == 0){
    fatal("This thread (%x) cannot create another thread", (uint32_t) ctx);
  }
#if 1                           /* Temporary allocation of ctx. */
  static unsigned int idx_reserve = 10;
  if(idx_reserve == 0) fatal("Not enough ctx in reserve\n");
  struct context *ctx2 = &user_tasks_image.reserve[--idx_reserve];
  ctx2->range = 0;
#else  
#ifdef DYNAMIC_TASK_CREATION_RECURSIVE  
  range >>= 1;
  ctx->range = range;
  struct context *ctx2 = ctx + range;
#endif
#ifdef DYNAMIC_TASK_CREATION_PARENT_CHILDS
  range--;
  ctx->range = range;
  struct context *ctx2 = ctx + range;
  ctx2->range = 0;
#endif
#endif  
  /* Duplicate the thread state entirely (hw + sched), except for the return value. */
  copy_context(ctx2, ctx);
  sched_insert_duplicated_context(ctx,ctx2);
  hw_context_set_return1(&ctx->hw_context,1);
  hw_context_set_return1(&ctx2->hw_context,0);    
  hw_context_switch_no_idle(&ctx->hw_context);
#endif  
}



void * const syscall_array[SYSCALL_NUMBER] __attribute__((used)) = {
  [SYSCALL_YIELD] = syscall_yield,
  [SYSCALL_PUTCHAR] = syscall_putchar,
  [SYSCALL_DUP_THREAD] = syscall_dup_thread,
  [SYSCALL_TRIGGER_BUG] = syscall_trigger_bug,  
};

void __attribute__((noreturn,used))
high_level_timer_interrupt_handler(struct hw_context *cur_hw_ctx, date_t curtime){
  _Static_assert(__builtin_offsetof(struct context,hw_context) == 0,
                 "Hardware context must be the first field to allow type casts");
  struct context *cur_ctx = (struct context *) cur_hw_ctx;
  /* terminal_print("interrupt handler %x\n", cur_ctx); */
  sched_wake_tasks(curtime);
  struct context *new_ctx;
  if(context_is_idle(cur_ctx)) {
    new_ctx = sched_choose_next();
  }
  else {
    new_ctx = sched_maybe_preempt(cur_ctx);
  }
  hw_context_switch(&new_ctx->hw_context);
}

void context_init(struct context * const ctx, int idx,
                  uint32_t pc,
                  uint32_t start, uint32_t end) {
  (void) idx;
  uint32_t pid;
#ifdef DYNAMIC_TASK_CREATION
  ctx->range = 1 << DYNAMIC_TASK_INITIAL_HEIGHT;
  ctx->pid = idx << DYNAMIC_TASK_INITIAL_HEIGHT;
  pid = ctx->pid;
#else
  pid = idx;
#endif
  hw_context_init(&ctx->hw_context, pc, pid, start, end);
}

void __attribute__((noinline)) init_reserve(void) {
  /* Initialize all reserve contexts like those of the first task. The contents
   * does not matter as it will be rewritten when a new thread is created, but
   * we prefer not to have uninitialized data. */
  struct task_description const *task0 = &user_tasks_image.tasks[0];
#pragma GCC unroll 10
  for(unsigned int i = 0; i < 10; i++) {
    context_init(&user_tasks_image.reserve[i], 0, task0->start_pc,
        (uint32_t) task0->task_begin, (uint32_t) task0->task_end);
#if defined(ROUND_ROBIN_SCHEDULING) || defined(SCHED_PRIORITY_QUEUE_USE_LIST)
    user_tasks_image.reserve[i].sched_context.next = &user_tasks_image.reserve[0];
#endif
  }

}

void /* __attribute__((noreturn)) */
high_level_init(void){
  unsigned int const nb_tasks = user_tasks_image.nb_tasks;
  
  for(unsigned int i = 0; i < nb_tasks; i++){
    asm ("cmp %1,%0 \n jae error_infinite_loop"::"r"(i), "r"(nb_tasks));
    struct task_description const *task = &user_tasks_image.tasks[i];
    context_init(task->context, i, task->start_pc,
                 (uint32_t) task->task_begin, (uint32_t) task->task_end);
  }

  init_reserve();

  for(int i =0; i < NUM_CPUS; i++ ){
    struct context * ctx = &user_tasks_image.idle_ctx_array[i];
    context_init(ctx, 0, 0, 
                 (uint32_t) user_tasks_image.idle_start, (uint32_t) user_tasks_image.idle_end);
    /* hw_context_idle_init(&ctx->hw_context, user_tasks_image.idle_start_code); */
#if defined(ROUND_ROBIN_SCHEDULING) || defined(SCHED_PRIORITY_QUEUE_USE_LIST)
    ctx->sched_context.next = ctx;
#endif
    
    ctx->sched_context.wakeup_date = 0ULL;
#if defined(EDF_SCHEDULING) || defined(DEADLINE_MONITORING)
    ctx->sched_context.deadline = 0xFFFFFFFFFFFFFFFFULL;
#endif
  }
  scheduler_init();


}

void __attribute__((noreturn))
boot(void){
  high_level_init();
  
  struct context *new_ctx = sched_choose_next();
#ifdef DEBUG_PRINTING
  terminal_writestring("Switching to userspace\n");
#endif
  hw_context_switch(&new_ctx->hw_context);  
}
