#ifndef __HIGH_LEVEL_H__
#define __HIGH_LEVEL_H__

#include "config.h"
#include "scheduler.h"
#include "timer.h"
#include "low_level.h"


/* High-level: parts of the kernel which are independent from the
   hardware architecture. */

struct context {
  /* Hardware context must come first. */
  struct hw_context hw_context;
  struct scheduling_context sched_context;
#ifdef DYNAMIC_TASK_CREATION
  /* A PID identifies a task, but is also a path in a tree.  The
     remaining [height] least significant bit are still available.

     When we fork, task keeps the current pid, the other gets pid | (1
     << height). Then, the height of both children is decreased. */
  int pid;

  /* A power of two; the task can create process whose pid is in [pid,pid+range[ */
  /* invariant: \forall k: 0 <= k < ctx->range => ctx + k is valid. */
  int range;

#endif  
};

_Static_assert(__builtin_offsetof(struct context,hw_context) == 0,
               "Hardware context must be the first field to allow type casts");


/* Returns the idle context. */
struct context *context_idle(void);

/* Check if ctx is the idle ctxt. */
_Bool context_is_idle(struct context *ctx);

/* Should be called once the low-level initialization is complete. */
void high_level_init(void);

/* Called by the pit timer when the time specified by wake_at is reached. */
void
high_level_timer_interrupt_handler(struct hw_context *cur_hw_ctx, date_t curtime);


#ifdef DYNAMIC_TASK_CREATION
#define CONTEXTS_PER_TASK (1 << DYNAMIC_TASK_INITIAL_HEIGHT)
#else
#define CONTEXTS_PER_TASK 1
#endif

/**************** For system description ****************/

#define HIGH_LEVEL_SYSTEM_DESC(NB_TASKS)                \
  /* struct context system_contexts[NB_TASKS * CONTEXTS_PER_TASK]; */

#endif /* __HIGH_LEVEL_H__ */
