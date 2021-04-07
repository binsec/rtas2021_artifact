#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

/* The time in nano-seconds since some reference (the boot, the
   epoch...)*/
typedef uint64_t date_t;

/* A difference between 2 dates. */
typedef uint64_t duration_t;

#define DATE_FAR_AWAY 0xFFFFFFFFFFFFFFFFULL;

/* Initialize the timer. */
void timer_init(void);

/* Get the current date. */
date_t timer_current_time(void);

/* Will call high_level_timer_interrupt_handler "soon after" we reach this date. */
void timer_wake_at(date_t);

/* Disarm the timer so that it will never wake. */
void timer_dont_wake(void);


#endif
