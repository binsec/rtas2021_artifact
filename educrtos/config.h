#ifndef __CONFIG_H__
#define __CONFIG_H__

/* If set, the GDT has a fixed size, the user and code descriptors are
   set in the context and overwritten on context switches. */
/* #define FIXED_SIZE_GDT */

/* If set, the GDT has a fixed size, and the descriptors are created
   dynamically on context switch. This is necessary for dynamic task
   creation. */
#define DYNAMIC_DESCRIPTORS

/* Set by the makefile.  */
//#define DYNAMIC_TASK_CREATION
#define DYNAMIC_TASK_INITIAL_HEIGHT 3 /* Means a maximum of 2**HEIGHT contexts per task.  */


/* If recursive, each created task can create another task. */
/* If parent childs, the parent can create several childs who cannot
   have childs themselves. */
/* #define DYNAMIC_TASK_CREATION_RECURSIVE */
#define DYNAMIC_TASK_CREATION_PARENT_CHILDS

#if (defined(DYNAMIC_TASK_CREATION_PARENT_CHILDS) + defined(DYNAMIC_TASK_CREATION_RECURSIVE)) != 1
#error "Dynamic task creation should be one of these types"
#endif




/* If nothing is set, the GDT has a parametric size and the user and
   code descriptors are written once at boot time. */
#define NUM_CPUS 1


/* Is the priority queue implemented using a heap-array, or using a linked list.   */
/* #define SCHED_PRIORITY_QUEUE_USE_HEAP */
#define SCHED_PRIORITY_QUEUE_USE_LIST
#if (defined(SCHED_PRIORITY_QUEUE_USE_LIST) + defined(SCHED_PRIORITY_QUEUE_USE_HEAP)) != 1
#error "Must define exactly one implementation"
#endif


//#define DEADLINE_MONITORING
#if defined(DEADLINE_MONITORING)
#error "Not yet implemented"
#endif

#if (defined(FP_SCHEDULING) + defined(EDF_SCHEDULING) + defined(ROUND_ROBIN_SCHEDULING)) != 1
#error "Must define exactly one scheduler"
#endif

/* If UNPRIVILEGED_IDLE, then user tasks provide an idle program which is executed unprivileged.
   If PRIVILEGED_IDLE, the idle executes with privileged. */
#define UNPRIVILEGED_IDLE
#if (defined(PRIVILEGED_IDLE) + defined(UNPRIVILEGED_IDLE)) != 1
#error "define either UNPRIVILEGED_IDLE or PRIVILEGED_IDLE"
#endif


#endif
