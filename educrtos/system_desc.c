/* system_desc.c generates the user image. */

/* This file requires a "tasks" parameter which is a xmacro, defined like this:
   #define TASKS \
   TASK(task0, "task0.bin", 10) \
   TASK(task1, "task1.bin", 20)

   The parameters for entry are a symbol which is the name of the
   task, the file containing the file binary, and the task fixed
   priority.

   They can be passed on the command line: 
   gcc '-DTASKS=TASK(task0,"task0.bin",10) TASK(task1,"task1.bin",20)' -E system_desc_manual.c  
*/

#include "user_tasks.h"

#ifndef TASKS
#error "The TASKS xmacro should be defined."
#endif

/* The idle task has the least priority */
#define IDLE_TASK TASK(idle_task, dummy, 0)
#define ALL_TASKS IDLE_TASK TASKS 

#define STRING(x) #x
#define XSTRING(x) STRING(x)


#ifdef UNPRIVILEGED_IDLE
/* TODO: factorize with incbin. */
asm(".section .data.task\n"
    ".global idle_task_begin\n"
    ".balign 16\n"
    ".type idle_task_begin, @object\n"
    "idle_task_begin: jmp idle_task2\n"
    "idle_task2: jmp idle_task2\n"    
    ".global idle_task_end\n"
    ".type idle_task_end, @object\n"
    "idle_task_end: \n"
);
extern __attribute__((aligned(16))) char idle_task_begin[];
extern char idle_task_end[];
#endif


#define INCBIN(name, file) \
    asm(".section .data.task\n" \
            ".global " XSTRING(name) "_begin\n" \
            ".type " XSTRING(name) "_begin, @object\n" \
            ".balign 16\n" \
            XSTRING(name) "_begin:\n" \
            ".incbin \"" file "\"\n" \
            \
            ".global " XSTRING(name) "_end\n" \
            ".type " XSTRING(name) "_end, @object\n" \
            ".balign 1\n" \
            XSTRING(name) "_end:\n" \
            ".byte 0\n" \
    ); \
    extern __attribute__((aligned(16))) char name ## _begin[]; \
    extern char name ## _end[]; \

#include "terminal.h"           /* For now. */
#include "user_tasks.h"
#include "high_level.h"



/* Include each of the file. */
#define TASK(name,file,priority) INCBIN(name,file)
TASKS
#undef TASK

/* Assign a unique id and number for each task, and provide a symbol
   with the number of tasks. */
enum task_ids {
#define TASK(name,file,priority) name##_id,
TASKS
#undef TASK
NB_TASKS              
};

/* Generate the kernel-specific definitions. */
#define NB_TASKS NB_TASKS
#define NB_CONTEXTS (NB_TASKS * CONTEXTS_PER_TASK)

#include "system_desc.h"


/* Generate the array of tasks. */
#ifdef FP_SCHEDULING
#define PRINT_PRIORITY(p) .priority = p,
#else
#define PRINT_PRIORITY(p)
#endif

/* Each task points to an array of contexts of size 2**height.
   We use the height last bits of the pid, and the height, to know where we are.
 */
#define TASK(name,file,priority) \
static struct context contexts_for_##name[CONTEXTS_PER_TASK];
TASKS
#undef TASK

static const struct task_description tasks[NB_TASKS] = {
#define TASK(name,file,priority) \
  [name##_id] = {                               \
     .context = &contexts_for_##name[0],        \
     .start_pc = 0,                             \
     .task_begin = name##_begin,                \
     .task_end = name##_end,                    \
     PRINT_PRIORITY(priority) },
TASKS
#undef TASK
#undef PRINT_PRIORITY
};

#ifdef SCHED_PRIORITY_QUEUE_USE_HEAP
static struct context *ready_heap_array[NB_CONTEXTS];
static struct context *waiting_heap_array[NB_CONTEXTS];
#endif

static struct context reserve[10];

static struct context idle_ctx_array[NUM_CPUS];
  
const struct user_tasks_image user_tasks_image = {
  .nb_tasks = NB_TASKS,
  .nb_contexts = NB_CONTEXTS,
  .tasks = tasks,
  .low_level = {}/* low_level_description */,
#ifdef SCHED_PRIORITY_QUEUE_USE_HEAP
  .ready_heap_array = &ready_heap_array[0],
  .waiting_heap_array = &waiting_heap_array[0],  
#endif
#ifdef UNPRIVILEGED_IDLE  
  .idle_start = &idle_task_begin[0],
  .idle_end = &idle_task_end[0],
#endif  
  .idle_ctx_array = &idle_ctx_array[0],
  .reserve = &reserve[0],
};


