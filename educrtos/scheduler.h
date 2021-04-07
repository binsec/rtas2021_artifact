#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "config.h"
#include "timer.h"

/* Description of a scheduling algorithm. */

struct context;

/* Block a task, i.e. set it as waiting (for a time event.) */
void sched_set_waiting(struct context *ctx);

/* Change the status (to ready) of all tasks that are now ready. */
void sched_wake_tasks(date_t curtime);

/* Pass to the scheduler the current context; either return it, or
   return one with higher priority if a preemption is needed. */
struct context *sched_maybe_preempt(struct context *ctx);

/* Return the next context to execute (may be the idle context). */
struct context *sched_choose_next(void);

/* Insert the newly created context that was duplicated from orig. */
void sched_insert_duplicated_context(struct context *orig,
                                     struct context *new);


/* Initialize the scheduler. */
void scheduler_init(void);


struct scheduling_context {
  date_t wakeup_date;           /* If active, last time it was awaken. If inactive: next time. */
#if defined(EDF_SCHEDULING) || defined(DEADLINE_MONITORING)
  date_t deadline;
#endif  
#ifdef FP_SCHEDULING
  unsigned int priority;
#endif
#if defined(ROUND_ROBIN_SCHEDULING) || defined(SCHED_PRIORITY_QUEUE_USE_LIST)
  struct context *next;
#endif  
};

#endif
