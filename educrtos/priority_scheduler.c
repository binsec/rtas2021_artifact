#include "scheduler.h"
#include "high_level.h"
#include "user_tasks.h"
#include "per_cpu.h"
#include "error.h"

#include "heap.c"


/* Waiting heap: sort the tasks by their next wakeup date. */
typedef date_t waiting_priority_t;
typedef struct context * waiting_elt_id_t;
static waiting_priority_t waiting_get_priority(waiting_elt_id_t ctx){
  return ctx->sched_context.wakeup_date;
}
/* Earlier wake dates have higher priority. */
static _Bool waiting_is_gt_priority(date_t a, date_t b){
  return a < b;
}
INSTANTIATE_HEAP(waiting);
static struct waiting_heap waiting_heap;

void sched_set_waiting(struct context *ctx){
  assert(waiting_heap.size <= user_tasks_image.nb_contexts);
  waiting_insert_elt(&waiting_heap, ctx);

  /* Set a possible preemption point when we reach the next wakeup. */
  date_t next_wakeup = waiting_get_first(&waiting_heap)->sched_context.wakeup_date;
  timer_wake_at(next_wakeup);
}


typedef struct context * ready_elt_id_t;
#ifdef FP_SCHEDULING
typedef unsigned int ready_priority_t;
static ready_priority_t ready_get_priority(ready_elt_id_t ctx){
  return ctx->sched_context.priority;
}
static _Bool ready_is_gt_priority(ready_priority_t a, ready_priority_t b){
  return a > b;
}
#endif

#ifdef EDF_SCHEDULING
typedef date_t ready_priority_t;
__attribute__((noinline)) static ready_priority_t ready_get_priority(ready_elt_id_t ctx){
  return ctx->sched_context.deadline;
}
/* Earlier deadlines have higher priority. */
static _Bool ready_is_gt_priority(date_t a, date_t b){
  return a < b;
}
#endif
INSTANTIATE_HEAP(ready);

static struct ready_heap ready_heap;

void scheduler_init(void){
#ifdef SCHED_PRIORITY_QUEUE_USE_HEAP
  ready_heap.size = 0;
  ready_heap.array = user_tasks_image.ready_heap_array;
  waiting_heap.size = 0;
  waiting_heap.array = user_tasks_image.waiting_heap_array;
#endif  
#ifdef SCHED_PRIORITY_QUEUE_USE_LIST  
  ready_heap_init(&ready_heap);
  waiting_heap_init(&waiting_heap);
#endif
  

#ifdef FP_SCHEDULING
  /* The idle tasks have a very low priority.  This is probably not
     necessary, as they are not put inside the heap. */
  for(int i =0; i < NUM_CPUS; i++ ){
    struct context * ctx = &user_tasks_image.idle_ctx_array[i];
    ctx->sched_context.priority = 0;
  }
#endif    
  
  /* Initially, all the tasks are ready. */  
  unsigned int const nb_tasks = user_tasks_image.nb_tasks;
  for(unsigned int i = 0; i < nb_tasks; i++){
    asm ("cmp %1,%0 \n jae error_infinite_loop"::"r"(i), "r"(nb_tasks));
    struct task_description const *task = &user_tasks_image.tasks[i];    
    struct context *ctx = task->context;
#ifdef FP_SCHEDULING        
    ctx->sched_context.priority = task->priority;
#endif    
    ready_insert_elt(&ready_heap, ctx);
  }
}

void sched_insert_duplicated_context(__attribute__((unused)) struct context *orig,
                                     struct context *new)
{
  ready_insert_elt(&ready_heap,new);
}



/* Wakeup; set some waiting tasks as ready, and maybe preempt
   others. */
void sched_wake_tasks(date_t curtime){
  while(waiting_heap.size > 0
        && waiting_get_first(&waiting_heap)->sched_context.wakeup_date <= curtime){
    struct context *ctx = waiting_remove_elt(&waiting_heap);
    assert(ready_heap.size <= user_tasks_image.nb_contexts);
    ready_insert_elt(&ready_heap, ctx);
  }
}

struct context * sched_maybe_preempt(struct context *ctx){
  assert(!(context_is_idle(ctx)));
  if(ready_heap.size > 0) {
    ready_priority_t curprio = ready_get_priority(ctx);
    ready_priority_t firstprio = ready_get_priority(ready_get_first(&ready_heap));    
    if(ready_is_gt_priority(firstprio, curprio)) {
      ready_insert_elt(&ready_heap, ctx);
      return ready_remove_elt(&ready_heap);
    }
  }
  return ctx;
}

struct context * sched_choose_next(void){
  if(ready_heap.size == 0) {
    return context_idle();
  }
  return ready_remove_elt(&ready_heap);
}
