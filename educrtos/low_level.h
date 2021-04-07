/* Low-level (architecture-dependent) external header for
   x86 32bit architecture. */
#ifndef __LOW_LEVEL_H__
#define __LOW_LEVEL_H__

#include "config.h"
#include <stdint.h>

/* These types should not be manipulated directly, but the high-level
   context has to know their size. */

/* This is pushed on the stack when there is an interrupt 
   and the current privilege level is 0. */
struct intra_privilege_interrupt_frame
{
  uint32_t eip;
  uint32_t cs;
  uint32_t flags;
} __attribute__((packed));

/* This is pushed on the stack when there is an interrupt 
   and the current privilege level is 3. */
struct inter_privilege_interrupt_frame
{
  uint32_t eip;
  uint32_t cs;
  uint32_t flags;  
  uint32_t esp;
  uint32_t ss;
} __attribute__((packed));

/* This is pushed on the stack on a pusha instruction. */
struct pusha
{
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t esp;                 /* Saved by pusha, not restored by popa. */
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
} __attribute__((packed));


/* A segment descriptor is an entry in a GDT or LDT. */
typedef uint64_t segment_descriptor_t __attribute__((aligned(8)));

struct hw_context {
  /* The hardware context is restored with popa;iret. */
  struct pusha           regs;
  struct inter_privilege_interrupt_frame iframe;
#ifdef DYNAMIC_DESCRIPTORS
  uint32_t start_address;
  uint32_t memsize;
#endif
#ifdef FIXED_SIZE_GDT  
  /* Segment selectors are initialized once. They point to the same
     address range, but code and segment are different. */
  segment_descriptor_t code_segment;
  segment_descriptor_t data_segment;
#endif  
} __attribute__((packed,aligned(4)));


/* Setup the context so that it calls function pc with (register) argument arg. */
void
hw_context_init(struct hw_context* ctx, uint32_t pc, uint32_t arg,
                uint32_t start_address, uint32_t end_address);

void
hw_context_idle_init(struct hw_context* ctx);

/* Go in idle mode, waiting for the next interrupt. */
void __attribute__((noreturn))
hw_context_idle(void);

/* Switch to either idle mode or to a task. */
void __attribute__((noreturn))
hw_context_switch(struct hw_context *ctx);

/* Switch to a context which cannot be idle. */
void __attribute__((noreturn))
hw_context_switch_no_idle(struct hw_context *ctx);


/* Assuming the thread is in syscall, change the thread so that it
   returns 1 value ret1. */
void
hw_context_set_return1(struct hw_context* ctx, uint32_t ret1);

/* Divide-by-zero Error 	0 (0x0) 	Fault 	#DE 	No */
/* Debug 	1 (0x1) 	Fault/Trap 	#DB 	No */
/* Non-maskable Interrupt 	2 (0x2) 	Interrupt 	- 	No */
/* Breakpoint 	3 (0x3) 	Trap 	#BP 	No */
/* Overflow 	4 (0x4) 	Trap 	#OF 	No */
/* Bound Range Exceeded 	5 (0x5) 	Fault 	#BR 	No */
/* Invalid Opcode 	6 (0x6) 	Fault 	#UD 	No */
/* Device Not Available 	7 (0x7) 	Fault 	#NM 	No */
/* Double Fault 	8 (0x8) 	Abort 	#DF 	Yes (Zero) */
/* Coprocessor Segment Overrun 	9 (0x9) 	Fault 	- 	No */
/* Invalid TSS 	10 (0xA) 	Fault 	#TS 	Yes */
/* Segment Not Present 	11 (0xB) 	Fault 	#NP 	Yes */
/* Stack-Segment Fault 	12 (0xC) 	Fault 	#SS 	Yes */
/* General Protection Fault 	13 (0xD) 	Fault 	#GP 	Yes */
/* Page Fault 	14 (0xE) 	Fault 	#PF 	Yes */
/* Reserved 	15 (0xF) 	- 	- 	No */
/* x87 Floating-Point Exception 	16 (0x10) 	Fault 	#MF 	No */
/* Alignment Check 	17 (0x11) 	Fault 	#AC 	Yes */
/* Machine Check 	18 (0x12) 	Abort 	#MC 	No */
/* SIMD Floating-Point Exception 	19 (0x13) 	Fault 	#XM/#XF 	No */
/* Virtualization Exception 	20 (0x14) 	Fault 	#VE 	No */
/* Reserved 	21-29 (0x15-0x1D) 	- 	- 	No */
/* Security Exception 	30 (0x1E) 	- 	#SX 	Yes */
/* Reserved 	31 (0x1F) 	- 	- 	No */
/* Triple Fault 	- 	- 	- 	No */
/* FPU Error Interrupt 	IRQ 13 	Interrupt 	#FERR 	No  */

enum interrupt_number {
  /* Exceptions.  */
  DIVIDE_BY_ZERO_INTNUM                = 0x00,
  DEBUG_INTNUM                         = 0x01,
  NON_MASKABLE_INTERRUPT_INTNUM        = 0x02,
  BREAKPOINT_INTNUM                    = 0x03,
  OVERFLOW_INTNUM                      = 0x04,
  BOUND_RANGE_EXCEEDED_INTNUM          = 0x05,
  INVALID_OPCODE_INTNUM                = 0x06,
  DEVICE_NOT_AVAILABLE_INTNUM          = 0x07,
  DOUBLE_FAULT_INTNUM                  = 0x08,
  COPROCESSOR_SEGMENT_OVERRUN_INTNUM   = 0x09,
  INVALID_TSS_INTNUM                   = 0x0a,
  SEGMENT_NOT_PRESENT_INTNUM           = 0x0b,
  STACK_SEGMENT_FAULT_INTNUM           = 0x0c,
  GENERAL_PROTECTION_FAULT_INTNUM      = 0x0d,
  PAGE_FAULT_INTNUM                    = 0x0e,
  RESERVED_INTNUM                      = 0x0f,
  X87_FLOATING_POINT_EXCEPTION_INTNUM  = 0x10,
  ALIGNMENT_CHECK_INTNUM               = 0x11,
  MACHINE_CHECK_INTNUM                 = 0x12,
  SIMD_FLOATING_POINT_EXCEPTION_INTNUM = 0x13,
  VIRTUALIZATION_EXCEPTION_INTNUM      = 0x14,
  SECURITY_EXCEPTION_INTNUM            = 0x1E,

  /* Other interrupts. */
  SYSCALL_INTNUM = 0x27,
  /* We initialize the PIC here, so 0x40...47 are for the master PIC, 
     and 0x48...4F for the slave PIC. */
  TIMER_INTNUM = 0x40,
  SPURIOUS_TIMER_INTNUM = 0x48
};

/**************** For use by user tasks. ****************/


/* We try to pass the arguments in register using the regparm(3) GCC
   calling convention; except that we do not use the first argument
   eax, as it will be use in the kernel to pass the hardware context.
   Also, we put the syscall number in ebx, and perform syscall number filtering
   in assembly.

   First argument (syscall number) is ebx, second is edx, third is ecx, fourth is esi, fifth is edi, sixth is ebp. */

static inline void
syscall1(uint32_t arg){
  asm volatile ("int %0": :
                "i"(SYSCALL_INTNUM),
                "b"(arg)
                );
}

/* Pass 1 argument, and retrieve also 1 result. */
static inline uint32_t
syscall1_1(uint32_t arg){
  uint32_t retval1;
  asm volatile ("int %1"
                : "=a"(retval1)
                : "i"(SYSCALL_INTNUM),
                  "b"(arg)
                );
  return retval1;
}


static inline void
syscall2(uint32_t arg1, uint32_t arg2){
  asm volatile ("int %0": :
                "i"(SYSCALL_INTNUM),
                "b"(arg1),
                "d"(arg2)
                );
}


static inline void
syscall5(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5){
  asm volatile ("int %0": :
                "i"(SYSCALL_INTNUM),
                "b"(arg1),
                "d"(arg2),
                "c"(arg3),
                "S"(arg4),
                "D"(arg5)                
                );
}



/**************** For use by system description, to allocate the data in the user tasks. ****************/

#define NUM_CPUS 1

struct user_task_descriptors {
  segment_descriptor_t code_descriptor;
  segment_descriptor_t data_descriptor;  
} __attribute__((packed));

struct system_gdt {
  segment_descriptor_t null_descriptor;
  segment_descriptor_t kernel_code_descriptor;
  segment_descriptor_t kernel_data_descriptor;
#if defined(FIXED_SIZE_GDT) || defined(DYNAMIC_DESCRIPTORS)
  segment_descriptor_t user_code_descriptor;
  segment_descriptor_t user_data_descriptor;
#endif  
  segment_descriptor_t tss_descriptor[NUM_CPUS];
#if !(defined(FIXED_SIZE_GDT) || defined(DYNAMIC_DESCRIPTORS))
  struct user_task_descriptors user_task_descriptors[]; /* One per task */
#endif  
} __attribute__((packed,aligned(8)));

struct system_gdt system_gdt;

struct low_level_description {
#if !(defined(FIXED_SIZE_GDT) || defined(DYNAMIC_DESCRIPTORS))
  struct system_gdt * const system_gdt;
#endif
};

#define KERNEL_CODE_SEGMENT_INDEX \
  (offsetof(struct system_gdt,kernel_code_descriptor)/sizeof(segment_descriptor_t))
#define KERNEL_DATA_SEGMENT_INDEX \
  (offsetof(struct system_gdt,kernel_data_descriptor)/sizeof(segment_descriptor_t))
#define TSS_SEGMENTS_FIRST_INDEX \
  (offsetof(struct system_gdt,tss_descriptor)/sizeof(segment_descriptor_t))
#if defined(FIXED_SIZE_GDT) || defined(DYNAMIC_DESCRIPTORS)
#define USER_CODE_SEGMENT_INDEX \
  (offsetof(struct system_gdt,user_code_descriptor)/sizeof(segment_descriptor_t))
#define USER_DATA_SEGMENT_INDEX \
  (offsetof(struct system_gdt,user_data_descriptor)/sizeof(segment_descriptor_t))
#else  
#define START_USER_INDEX \
  (offsetof(struct system_gdt,user_task_descriptors)/sizeof(segment_descriptor_t))
#endif

#if defined(FIXED_SIZE_GDT) || defined(DYNAMIC_DESCRIPTORS)
#define SYSTEM_GDT(NB_TASKS) struct system_gdt system_gdt;
#define SYSTEM_GDT_FIELD 
#else
#define SYSTEM_GDT(NB_TASKS)                                            \
  struct {                                                              \
  struct system_gdt begin;                                              \
  struct user_task_descriptors desc[NB_TASKS]; } __attribute__((packed)) system_gdt;
#define SYSTEM_GDT_FIELD .system_gdt = (struct system_gdt *) &system_gdt,
#endif


#define LOW_LEVEL_SYSTEM_DESC(NB_TASKS)                                 \
  SYSTEM_GDT(NB_TASKS);                                                 \
  static const struct low_level_description low_level_description =     \
    { SYSTEM_GDT_FIELD };

#endif /* __LOW_LEVEL_H__ */
