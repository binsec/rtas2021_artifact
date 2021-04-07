/* References: */
/* https://en.wikibooks.org/wiki/X86_Assembly/Programmable_Interval_Timer */
/* https://wiki.osdev.org/Programmable_Interval_Timer#PIT_Channel_0_Example_Code */
/* https://en.wikipedia.org/wiki/Intel_8253#Operation_modes */

#include "timer.h"
#include "config.h"

#define _1_NANOSECOND 1ULL
#define _1_MICROSECOND (1000ULL * _1_NANOSECOND)
#define _1_MILLISECOND (1000ULL * _1_MICROSECOND)
#define _1_SECOND (1000ULL * _1_MILLISECOND)

#define PIT_HZ 1193182               /* The PIT fixed frequency */
#define WANTED_TICK _1_MILLISECOND   /* The time, in nano seconds, between ticks. */
/* How to divide to tick to the required frequency. */
#define DIVISOR ((PIT_HZ * WANTED_TICK)/_1_SECOND)

/* Using this allows to be precise; else the difference can be
   significant (10s of second every day). */
#define ACTUAL_TICK ((_1_SECOND * DIVISOR)/PIT_HZ) 

/* DIVISOR must be held on 16 bits. This implies that wanted_tick must
   be smaller than this (approximatively 50 milliseconds). */
#define MAX_DIVISOR 65535
_Static_assert(WANTED_TICK < (_1_SECOND * MAX_DIVISOR)/PIT_HZ,
               "WANTED_TICK must be small enough so that divisor fits in a tick");
_Static_assert(DIVISOR < MAX_DIVISOR,
               "DIVISOR must fit in a 16bit register");


#include <stdint.h>
#include "low_level.h"
#include "high_level.h"
#include <stdatomic.h> 
#include "x86/port.h"



void init_apic(void){
  /* We do not use the APIC for now. But we can use the PIT with the
     APIC. */
}

static const uint16_t master_pic_command = 0x20;
static const uint16_t master_pic_data = 0x21;
static const uint16_t slave_pic_command = 0xA0;
static const uint16_t slave_pic_data = 0xA1;


/* https://wiki.osdev.org/8259_PIC */
void init_pic(void){

  /* Initialize. Wait for three words on the data port.*/
  outb(master_pic_command, 0x11);
  outb(slave_pic_command, 0x11);  

  /* Change the interrupt number from defaults 0x08 and 0x70. */
  outb(master_pic_data, TIMER_INTNUM);
  outb(slave_pic_data, SPURIOUS_TIMER_INTNUM);

  /* Connect master and slave PIC. */
  outb(master_pic_data, 4);
  outb(slave_pic_data, 2);

  /* Require explicit end of interrupt. */
  outb(master_pic_data, 1);
  outb(slave_pic_data, 1);

  uint8_t activate_none = 0xFF;
  uint8_t activate_irq0 = 0xFE;

  /* If we want to disable the PIC, we put activate_none here. */
  outb(master_pic_data, activate_irq0);
  outb(slave_pic_data, activate_none);  

  
}

static const uint16_t pit_command = 0x43;
static const uint16_t pit_data = 0x40;

#if NUM_CPUS == 1
/* Time from the boot, in nano-seconds. */
static /* _Atomic */uint64_t current_time;
/* No need to read/write time atomically on single core, and if we
   disable interrupts in the kernel.  */

uint64_t timer_current_time(void){
  return *(&current_time);
}
#else
static _Atomic(uint64_t) current_time;

uint64_t timer_current_time(void){
  return atomic_load(&current_time);
}

#endif

void timer_init(void){
  current_time = 0;

  /* Just send the divisor to the PIT. */
  outb(pit_command, 0x34);
  outb(pit_data, DIVISOR & 0xFF);
  outb(pit_data, DIVISOR >> 8);
}




static uint64_t next_wake_date = DATE_FAR_AWAY;

static int count;

void timer_wake_at(date_t next_wakeup){
  next_wake_date = next_wakeup;
}

void timer_dont_wake(void){
  next_wake_date = DATE_FAR_AWAY;
}

#include "terminal.h"

void __attribute__((regparm(3),noreturn,used))
timer_interrupt_handler(struct hw_context *cur_hw_ctx){
  /* Acknowledge interrupt. */
  outb(master_pic_command, 0x20);

  /* This instance is the only one changing the time. So we do not
     need to be atomic. */
  uint64_t cur = *(&current_time);
  cur += ACTUAL_TICK;
  *(&current_time) = cur;

#ifdef DEBUG_PRINTING
  /* Temporary: write a & every 10th of second, to show time passing. */
  if(++count % 100 == 0) {
    terminal_putchar('&');
    /* terminal_print("cur: %llu, next_wake_date: %llu\n",
       (date_t) cur, (date_t) next_wake_date);     */
  }
#endif

  
  if(cur >= next_wake_date){
    timer_dont_wake();
    high_level_timer_interrupt_handler(cur_hw_ctx, cur);
  }

  hw_context_switch(cur_hw_ctx);
}
