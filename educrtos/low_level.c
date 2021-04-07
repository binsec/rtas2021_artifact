#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "terminal.h"
#include "segment.h"
#include "low_level.h"
#include "high_level.h"
#include "config.h"
#include "error.h"
#include "copy64.h"
#include "macros.h"

/* As Qemu can dump the state before each basic block, the following
   fake jump is useful to debug assembly code.  */
#define CHANGE_BB_FOR_ASM_DEBUG "jmp 1f\n1:\n"

/* Check if the compiler thinks you are targeting the wrong operating system. */
/* #if defined(__linux__) */
/* #warning "Not using a cross-compiler" */
/* #endif */
 
/* This OS works only for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "Compile for 32 bit x86"
#endif

#include "user_tasks.h"

/**************** Boot ****************/

#define MULTIBOOT_MAGIC 0x1BADB002
#define MULTIBOOT_FLAGS ((1 << 0) | (1 << 1) | (1 << 2))
#define MULTIBOOT_CHECKSUM -(MULTIBOOT_FLAGS + MULTIBOOT_MAGIC)

struct multiboot {
  uint32_t magic;
  uint32_t flags;
  uint32_t checksum;
} __attribute__((packed,aligned(4)));

struct extended_multiboot {
  struct multiboot base;
  uint32_t header_addr;
  uint32_t load_addr;
  uint32_t load_end_addr;
  uint32_t bss_end_addr;
  uint32_t entry_addr;
  uint32_t mode_type;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
};


const struct extended_multiboot multiboot __attribute__((section(".multiboot"))) = {
  .base = {
   .magic = MULTIBOOT_MAGIC,
   .flags = MULTIBOOT_FLAGS,
   .checksum = MULTIBOOT_CHECKSUM, },
   /* Standard 80x25 text mode. */
   .mode_type = 1,
   .width = 80,
   .height =  25,
   .depth = 0
};



#define KERNEL_STACK_SIZE 1024
/* System V ABI mandates that stacks are 16-byte aligned. */
static char kernel_stack[KERNEL_STACK_SIZE] __attribute__((used,aligned(16)));


/* Startup: just setup the stack and call the C function. */
asm("\
.global _start\n\
.type _start, @function\n\
_start:\n\
        /* Setup the stack */ \n\
	mov $(kernel_stack +" XSTRING(KERNEL_STACK_SIZE) "), %esp\n\
        mov %eax, %ecx\n\
        mov %ebx, %edx\n\
        call low_level_init\n\
        call boot\n\
        jmp error_infinite_loop\n\
/* setup size of _start symbol. */\n\
.size _start, . - _start\n\
");

/**************** TSS ****************/

/* The TSS; almost everything is unused when using software task switching. */
struct tss
{
   uint32_t unused_prev_tss;
   uint32_t esp0;       // The stack pointer to load when we change to kernel mode.
   uint32_t ss0;        // The stack segment to load when we change to kernel mode.
   uint32_t unused_esp1;      
   uint32_t unused_ss1;
   uint32_t unused_esp2;
   uint32_t unused_ss2;
   uint32_t unused_cr3;
   uint32_t unused_eip;
   uint32_t unused_eflags;
   uint32_t unused_eax;
   uint32_t unused_ecx;
   uint32_t unused_edx;
   uint32_t unused_ebx;
   uint32_t unused_esp;
   uint32_t unused_ebp;
   uint32_t unused_esi;
   uint32_t unused_edi;
   uint32_t unused_es;         
   uint32_t unused_cs;        
   uint32_t unused_ss;        
   uint32_t unused_ds;        
   uint32_t unused_fs;       
   uint32_t unused_gs;         
   uint32_t unused_ldt;      
   uint16_t unused_trap;
   uint16_t unused_iomap_base;
} __attribute__((packed));

/* TSS for the processors. */

#define current_cpu() 0
static struct tss tss_array[NUM_CPUS];

/**************** GDT and segment descriptors. ****************/

/* A note on x86 privilege.
   
   DPL = privilege in the data segment descriptor.
   RPL = privilege in the data segment selector. Allows to lower the privilege for some operations.
   CPL = privilege of the code segment selector, this is the current privilege level.
   There is a general protection fault if max(CPL,RPL) >= DPL. */

/* Size of descriptors. */
#define S16BIT 0
#define S32BIT 1

/* Granularity of descriptors: byte or 4k. */


#define create_descriptor(/* uint32_t */ base,                          \
/* uint32_t */ limit,                                                   \
/* bool */ present, /* 1 if used */           \
/* unsigned int */ privilege, /*  0 = full privileged, 3 = unprivileged  */ \
/* bool */ normal_segment ,   /* 0 for TSS and LDT, 1 for normal segment */ \
/* enum permissions */ permissions,                                   \
/* bool */ accessed,                                                  \
/* bool */ granularity, /* 0 = par byte, 1 = par block de 4k */ \
/* bool */ size)  /* 0 = 16 bit, 1 = 32 bit */ \
  (( (uint64_t) (limit) & 0x000F0000) |                                 \
   (((base) >> 16) & 0x000000FF) |                                      \
   ((base) & 0xFF000000) |                                              \
   ((accessed | (permissions << 1) | (normal_segment << 4) | (privilege << 5) | (present << 7)) << 8) | \
   (((size << 2) | (granularity << 3)) << 20)) << 32 |                  \
  ((base) << 16) |                                                      \
  ((limit) & 0x0000FFFF)

#define create_code_descriptor(base,limit,privilege,conforming,readable,accessed,granularity,size) \
  create_descriptor(base,limit,1,privilege,1,(4 | (conforming << 1) | readable),accessed,granularity,size)

#define create_data_descriptor(base,limit,privilege,growdown,writable,accessed,granularity,size) \
  create_descriptor(base,limit,1,privilege,1,(0 | (growdown << 1) | writable),accessed,granularity,size)

/* TSS descriptors must have a minimum size of 0x67. */
#define create_tss_descriptor(base,limit,privilege,busy,granularity)    \
  create_descriptor(base,((limit) <= 0x67?0x67:(limit)),1,privilege,0,(4 | 0 << 1 | busy),1,granularity,0)


static const segment_descriptor_t null_descriptor = create_descriptor(0,0,0,0,0,0,0,0,0);

/* Full access to all addresses for the kernel code and data. */
static const segment_descriptor_t kernel_code_descriptor = create_code_descriptor(0,0xFFFFFFFF,0,0,1,0,1,S32BIT);
static const segment_descriptor_t kernel_data_descriptor = create_data_descriptor(0,0xFFFFFFFF,0,0,1,0,1,S32BIT);

/* Because we use that in file-scope assembly, this must be a macro
   instead of an enum. */
#define _KERNEL_DATA_SEGMENT_INDEX   2
_Static_assert(_KERNEL_DATA_SEGMENT_INDEX == KERNEL_DATA_SEGMENT_INDEX,
               "_KERNEL_DATA_SEGMENT_INDEX must be a separate macro "
               "because it is used in inline assembly: "
               "set it to KERNEL_DATA_SEGMENT_INDEX");

#define _SYSCALL_NUMBER 4
_Static_assert(_SYSCALL_NUMBER == SYSCALL_NUMBER,
               "_SYSCALL_NUMBER must be a separate macro "
               "because it is used in inline assembly: "
               "set it to SYSCALL_NUMBER");

/* Address of the gdt, and size in bytes. */
static inline void lgdt(segment_descriptor_t *gdt, int size)
{
  struct gdt_register {
    uint16_t limit; /* Maximum offset to access an entry in the GDT. */
    uint32_t base; } __attribute__((packed,aligned(8)));
  struct gdt_register gdtr;
  gdtr.limit = size;
  gdtr.base = (uint32_t) gdt;
  asm volatile ("lgdt %0": : "m" (gdtr) : "memory");
}

/**************** Interrupts ****************/

/* Do we need CLD in all interrupt handlers? static analysis will tell! */

extern void ignore_interrupt_handler(void);
asm("\
.global ignore_interrupt_handler\n\t\
.type ignore_interrupt_handler, @function\n\
ignore_interrupt_handler:\n\
        iret\n\
.size ignore_interrupt_handler, . - ignore_interrupt_handler\n\
");

#define INTERRUPT_HANDLER_PROLOGUE                                      \
"pusha\n\t"                                                             \
"cld\n\t"                                                               \
"movw $(" XSTRING(_KERNEL_DATA_SEGMENT_INDEX) " << 3), %ax \n\t"        \
"movw %ax, %ds\n\t"                                                     \
"mov %esp, %eax\n\t"                                                    \
"mov $(kernel_stack +" XSTRING(KERNEL_STACK_SIZE) "), %esp\n\t"         \


/* This:
   - Saves the registers in the context structure;
   - Restores the cld flag (maybe not useful);
   - Restores the ds register (cs and ss are restored by the interrupt mechanism)
   - Loads the kernel stack, call high_level_syscall  */
extern void asm_syscall_handler(void);
asm("\
.global asm_syscall_handler\n\t\
.type asm_syscall_handler, @function\n\
asm_syscall_handler:\n\t"
INTERRUPT_HANDLER_PROLOGUE    
"       cmp $" XSTRING(_SYSCALL_NUMBER) ", %ebx\n           \
        jae error_infinite_loop\n\
        jmp *syscall_array(,%ebx,4)\n\
        jmp error_infinite_loop\n\
.size asm_syscall_handler, . - asm_syscall_handler\n\
");

/* MAYBE: share the code. */
extern void asm_timer_interrupt_handler(void);
asm("\
.global asm_timer_interrupt_handler\n\t\
.type asm_timer_interrupt_handler, @function\n\
asm_timer_interrupt_handler:\n\t"
INTERRUPT_HANDLER_PROLOGUE    
"       /* Note: must use the regparm3 calling ABI. */\n\
	call timer_interrupt_handler\n\
        jmp error_infinite_loop\n\
.size asm_timer_interrupt_handler, . - asm_timer_interrupt_handler\n\
");

void __attribute__((noreturn))
hw_context_switch(struct hw_context* ctx);

/* https://wiki.osdev.org/IDT */
/* I shoud set up interrupt gates, so that interrupts are disabled on entry. */

/* Gate descriptors are entries in the idt. */
typedef uint64_t gate_descriptor_t;

/* We don't want to use task gate, these are for hardware context switching. */
/* The difference between interrupt and task gates is that interrupt
   gates clear the IF flag (interrupts are masked). */

/* No need to define a gate descriptor for every interrupt; the later
   ones can be caught by the general protection fault handler. */
#define NB_GATE_DESCRIPTORS 256

static gate_descriptor_t idt[NB_GATE_DESCRIPTORS];

/* Privilege is the needed privilege level for calling this interrupt.
   So, hardware interrupts (or exceptions?) should have 0, but for
   softwrare interrupt it should be 3. */
#define create_interrupt_gate_descriptor(func_ptr,code_segment_selector,privilege,size) \
((((func_ptr) & 0xFFFFULL) |                                               \
  ((code_segment_selector) << 16)) |                                    \
 ((((func_ptr) & 0xFFFF0000) |                                          \
  ((1 << 15) |                                                          \
   (privilege << 13) |                                                  \
   (size << 11) |                                                       \
   (0b00110ULL << 8ULL))) << 32ULL))

/* Trap gate are like interrupt gates, but does not disable the interrupts. */
#define create_trap_gate_descriptor(func_ptr,code_segment_selector,privilege,size) \
((((func_ptr) & 0xFFFFULL) |                                               \
  ((code_segment_selector) << 16)) |                                    \
 ((((func_ptr) & 0xFFFF0000) |                                          \
  ((1 << 15) |                                                          \
   (privilege << 13) |                                                  \
   (size << 11) |                                                       \
   (0b00111ULL << 8ULL))) << 32ULL))

extern void unimplemented_interrupt_handler(void);
asm("\
.global unimplemented_interrupt_handler\n\t\
.type unimplemented_interrupt_handler, @function\n\
unimplemented_interrupt_handler:\n\
        jmp unimplemented_interrupt_handler2    \n\
unimplemented_interrupt_handler2:               \n\
        cli                                     \n\
        hlt                                     \n\
        jmp unimplemented_interrupt_handler2    \n\
.size unimplemented_interrupt_handler, . - unimplemented_interrupt_handler\n\
");

/* Note: there is no way to get the interrupt number other than 
   having different functions.  */
#define ASM_DEFAULT_INTERRUPT_HANDLER(x)                                \
extern void asm_default_interrupt_handler_##x (void);                   \
asm("\n"                                                                \
".global asm_default_interrupt_handler_" #x "\n"                        \
".type asm_default_interrupt_handler_" #x ", @function\n"               \
"asm_default_interrupt_handler_" #x ":\n"                               \
INTERRUPT_HANDLER_PROLOGUE                                              \
"       pushl $" #x "                           \n"                     \
"       call default_interrupt_handler          \n"                     \
"       jmp error_infinite_loop                 \n"                     \
".size asm_default_interrupt_handler_" #x ", . - asm_default_interrupt_handler_" #x "\n" \
    );

FOREACH_256(ASM_DEFAULT_INTERRUPT_HANDLER);

void __attribute__((stdcall)) default_interrupt_handler(int num){
  switch(num){
    case DIVIDE_BY_ZERO_INTNUM:
      task_fatal("Division by zero exception\n");
    case DEBUG_INTNUM:
      task_fatal("Debug exception\n");
    case  NON_MASKABLE_INTERRUPT_INTNUM:
      task_fatal("Non maskable interrupt exception\n");
    case  BREAKPOINT_INTNUM:
      task_fatal("Breakpoint exception\n");      
    case OVERFLOW_INTNUM:
      task_fatal("Overflow exception\n");
    case BOUND_RANGE_EXCEEDED_INTNUM:
      task_fatal("Bound range exceeded exception\n");
    case INVALID_OPCODE_INTNUM:
      task_fatal("Invalid opcode exception\n");
    case DEVICE_NOT_AVAILABLE_INTNUM:
      task_fatal("Device not available exception\n");
    case DOUBLE_FAULT_INTNUM:
      task_fatal("Double fault exception\n");      
    case COPROCESSOR_SEGMENT_OVERRUN_INTNUM:
      task_fatal("Coprocessor segment overrun exception\n");
    case INVALID_TSS_INTNUM:
      task_fatal("Invalid TSS exception\n");
    case SEGMENT_NOT_PRESENT_INTNUM:
      task_fatal("Segment not present exception\n");
    case STACK_SEGMENT_FAULT_INTNUM:
      task_fatal("Stack segment fault exception\n");    
    case GENERAL_PROTECTION_FAULT_INTNUM:
      task_fatal("General protection fault exception\n");        
    case PAGE_FAULT_INTNUM:
      task_fatal("Page fault exception\n");              
    case X87_FLOATING_POINT_EXCEPTION_INTNUM:
      task_fatal("X86 floating point exception\n");              
    case ALIGNMENT_CHECK_INTNUM:
      task_fatal("Alignment check exception\n");              
    case MACHINE_CHECK_INTNUM:
      task_fatal("Machine check exception\n");              
    case SIMD_FLOATING_POINT_EXCEPTION_INTNUM:
      task_fatal("SIMD floating point exception\n");                    
    case VIRTUALIZATION_EXCEPTION_INTNUM:
      task_fatal("Virtualization exception\n");                      
    case SECURITY_EXCEPTION_INTNUM:
      task_fatal("Security exception\n");                            
    default:
      fatal("Received unexpected interrupt: %d (%x)\n", num, num);    
  }
}


void init_pic(void);
void init_apic(void);

void init_interrupts(void){
  init_pic();
  init_apic();

#define INSTALL_DEFAULT_HANDLER(x) \
  idt[x] =                                                              \
    create_interrupt_gate_descriptor((uintptr_t) &asm_default_interrupt_handler_ ## x, \
                                     gdt_segment_selector(0,KERNEL_CODE_SEGMENT_INDEX),\
                                     3, S32BIT);
  FOREACH_256(INSTALL_DEFAULT_HANDLER);
#undef INSTALL_DEFAULT_HANDLER  

  idt[SYSCALL_INTNUM] =
    create_interrupt_gate_descriptor((uintptr_t) &asm_syscall_handler,
                                     gdt_segment_selector(0,KERNEL_CODE_SEGMENT_INDEX),
                                     3, S32BIT);
  
  idt[TIMER_INTNUM] =
    create_interrupt_gate_descriptor((uintptr_t) &asm_timer_interrupt_handler,
                                     gdt_segment_selector(0,KERNEL_CODE_SEGMENT_INDEX),
                                     0, S32BIT);

  idt[SPURIOUS_TIMER_INTNUM] =
    create_interrupt_gate_descriptor((uintptr_t) &unimplemented_interrupt_handler,
                                     gdt_segment_selector(0,KERNEL_CODE_SEGMENT_INDEX),
                                     0, S32BIT);
  
  struct idt_register {
    uint16_t limit; /* Maximum offset to access an entry in the GDT. */
    uint32_t base; } __attribute__((packed,aligned(8)));
  struct idt_register idtr;
  idtr.limit = sizeof(idt);
  idtr.base = (uint32_t) idt;
  asm volatile ("lidt %0": : "m" (idtr) : "memory");    

}

/* Here is what interrupt does:

   - Compare the CPL with the privilege level of the gate
     descriptor. A general protection fault is issued if they do not
     match.

   - load eip from func_ptr in the interrupt gate
   - load cs from the interrupt gate

   - load esp and ss from tss?
 */


/****************  ****************/

#if defined(FIXED_SIZE_GDT) || defined(DYNAMIC_DESCRIPTORS)
struct system_gdt system_gdt;
#endif

_Bool hw_context_is_idle(struct hw_context *ctx){
  struct context *high = context_idle();
  struct hw_context *idle = &high->hw_context;  
  return (ctx == idle);
}

void __attribute__((noreturn))
hw_context_idle(void){
  struct context *high = context_idle();
  struct hw_context *ctx = &high->hw_context;  

#ifdef UNPRIVILEGED_IDLE
  /* Normal context switch. */
  hw_context_switch(ctx);
#endif
#ifdef PRIVILEGED_IDLE  
  /* int a; */
  /* terminal_print("Idle: stack Address is %x\n", &ctx); */
  /* tss_array[current_cpu()].esp0 = (uint32_t) ctx + sizeof(struct pusha) + sizeof(struct intra_privilege_interrupt_frame); */

  /* We will get the interrupt while still in the kernel: the
     interrupt is pushed on top of the current esp (TSS is not
     read).  */
  asm volatile
    ("mov %0,%%esp" : : "r"((uint32_t) ctx + sizeof(struct pusha) + sizeof(struct intra_privilege_interrupt_frame)) : "memory");
  asm("sti");
  asm("hlt");
  asm("jmp error_infinite_loop");
  __builtin_unreachable ();  
#endif
}


#include "per_cpu.h"

void __attribute__((noreturn))
hw_context_switch(struct hw_context* ctx){
#ifdef PRIVILEGED_IDLE
  if(hw_context_is_idle(ctx))
    hw_context_idle();
#endif
    hw_context_switch_no_idle(ctx);
}

void __attribute__((noreturn))
hw_context_switch_no_idle(struct hw_context* ctx){
  /* terminal_print("Switching to %x\n", ctx); */

  /* terminal_print("Code segment is %llx\n", ctx->code_segment);   */
  /* terminal_print("Data segment is %llx\n", ctx->data_segment); */
  
  /* We will save the context in the context structure. */
  tss_array[current_cpu()].esp0 = (uint32_t) ctx + sizeof(struct pusha) + sizeof(struct inter_privilege_interrupt_frame);

#ifdef PRIVILEGED_IDLE  
  /* This should not happen; call hw_context_idle instead. */
  assert(!hw_context_is_idle(ctx));
#endif  

#ifdef FIXED_SIZE_GDT
  copy64(&system_gdt.user_code_descriptor, &ctx->code_segment);
  copy64(&system_gdt.user_data_descriptor, &ctx->data_segment);
#elif defined(DYNAMIC_DESCRIPTORS)
  uint64_t desc = create_code_descriptor(ctx->start_address, ctx->memsize,3,0,1,0,1,S32BIT);
  copy64(&system_gdt.user_code_descriptor, &desc);

  desc = create_data_descriptor(ctx->start_address, ctx->memsize,3,0,1,0,1,S32BIT);
  copy64(&system_gdt.user_data_descriptor, &desc);
    
#endif  
  
  /* terminal_print("ds reg will be %x\n", ctx->iframe.ss); */
  load_ds_reg(ctx->iframe.ss);
  /* Load the context. */
  asm volatile
    ("mov %0,%%esp \n\
      popa\n\
      iret" : : "r"(ctx) : "memory");
  __builtin_unreachable();
}

void
hw_context_set_return1(struct hw_context* ctx, uint32_t ret1){
  ctx->regs.eax = ret1;
}

asm("\
.global _idle\n\
.type _idle, @function\n\
_idle:\n\
/*        sti\n\
        hlt*/\n\
        jmp _idle\n\
.size _idle, . - _idle\n\
");
extern void _idle(void);


void hw_context_idle_init(struct hw_context* ctx){
  /* terminal_print("Initializing idle %x\n", ctx); */
  ctx->iframe.eip = (uint32_t) &_idle;

  /* We reuse the kernel segment for idle tasks, by simplicity. Maybe
     we could put it to privilege number 3. */
  ctx->iframe.cs = gdt_segment_selector(3,KERNEL_CODE_SEGMENT_INDEX);

    /* gdt_segment_selector(0,KERNEL_CODE_SEGMENT_INDEX); */
  /* ctx->iframe.ss = gdt_segment_selector(0, 0);/\* gdt_segment_selector(0,KERNEL_DATA_SEGMENT_INDEX);   *\/ */
  ctx->iframe.ss = gdt_segment_selector(3, USER_DATA_SEGMENT_INDEX);
  
#ifdef FIXED_SIZE_GDT
  ctx->code_segment = create_code_descriptor(0,0xFFFFFFFF,3,0,1,0,1,S32BIT); /* kernel_code_descriptor; */
  ctx->data_segment = create_data_descriptor(0,0xFFFFFFFF,3,0,1,0,1,S32BIT);

    /* kernel_data_descriptor;   */
  /* ctx->data_segment = null_descriptor; */
    /* kernel_data_descriptor; */
#elif defined(DYNAMIC_DESCRIPTORS)
  ctx->start_address = 0;
  ctx->memsize = 0xFFFFFFFF;
#endif
  /* Set only the reserved status flag, that should be set to 1; and
     the interrupt enable flag. */
  ctx->iframe.flags = (1 << 1) | (1 << 9);
}

void hw_context_init(struct hw_context* ctx, uint32_t pc, uint32_t arg,
                     uint32_t start_address, uint32_t end_address){
  /* terminal_print("Init task %x\n", ctx); */
#ifdef DEBUG
  ctx->regs.eax = 0xaaaaaaaa;
  ctx->regs.ecx = 0xcccccccc;
  ctx->regs.edx = 0xdddddddd;
  ctx->regs.ebx = 0xbbbbbbbb;
  ctx->regs.ebp = 0x99999999;        
  ctx->regs.esi = 0x88888888;
  ctx->regs.edi = 0x77777777;
#endif
  ctx->regs.eax = arg;    

#if defined(FIXED_SIZE_GDT) || defined(DYNAMIC_DESCRIPTORS)
#define CODE_INDEX USER_CODE_SEGMENT_INDEX
#define DATA_INDEX USER_DATA_SEGMENT_INDEX    
#else
#define CODE_INDEX (START_USER_INDEX + 2 * idx)
#define DATA_INDEX (START_USER_INDEX + 2 * idx + 1)
#endif  
  
  ctx->iframe.eip = pc;
  ctx->iframe.cs = (gdt_segment_selector(3, CODE_INDEX));
  /* Set only the reserved status flag, that should be set to 1; and
     the interrupt enable flag. */
  ctx->iframe.flags = (1 << 1) | (1 << 9);
#ifdef DEBUG  
  ctx->iframe.esp = 0xacacacac;
#endif
  ctx->iframe.ss = (gdt_segment_selector(3, DATA_INDEX));

  /* terminal_print("Init ctx is %x; ", ctx); */

#if defined(FIXED_SIZE_GDT)  /* || defined(DYNAMIC_DESCRIPTORS) */
  ctx->code_segment =
    create_code_descriptor(start_address, end_address - start_address,3,0,1,0,1,S32BIT);
  ctx->data_segment =
    create_data_descriptor(start_address, end_address - start_address,3,0,1,0,1,S32BIT);
#elif defined(DYNAMIC_DESCRIPTORS)
  ctx->start_address = start_address;
  ctx->memsize = end_address - start_address;
#else
  struct system_gdt * const gdt = user_tasks_image.low_level.system_gdt;
  /* terminal_print("gdt is  %x\n", gdt);   */
  gdt->user_task_descriptors[idx].code_descriptor =
    create_code_descriptor(start_address, end_address - start_address,3,0,1,0,1,S32BIT);
  gdt->user_task_descriptors[idx].data_descriptor =
    create_data_descriptor(start_address, end_address - start_address,3,0,1,0,1,S32BIT);  
#endif  


}

struct module_information {
  char *mod_start;
  char *mod_end;
  char *string;
  uint32_t reserved;
}  __attribute__((packed));

struct multiboot_information {
  uint32_t flags;
  /* If flags[0]. */
  uint32_t mem_lower;
  uint32_t mem_upper;
  /* If flags[1]. */
  uint32_t boot_device;
  /* If flags[2]. */
  uint32_t cmdline;
  /* If flags[3]. */
  uint32_t mods_count;
  struct module_information *mods_addr;
}  __attribute__((packed));

void __attribute__((fastcall,used))
low_level_init(uint32_t magic_value, struct multiboot_information *mbi) 
{
  /* Suppress the warning because we do not use mbi for now. */
  (void) mbi;
  
#ifdef DEBUG_PRINTING
  /* Initialize terminal interface */
  terminal_initialize();
#endif

  if(magic_value != 0x2BADB002)
    fatal("Not loaded by a multiboot loader");
  
  //terminal_print("multiboot_information flags: %x\n", mbi->flags);

  #if 0  
  if((mbi->flags & 3) == 0)
    fatal("Multiboot information flags 3 is not present");

  if(mbi->mods_count != 1)
    fatal("This kernel must be loaded with exactly one module, here %d\n", mbi->mods_count);
  #endif
  
  //terminal_writestring("Kernel start\n");
  
  /* Up to now we used the segments loaded by grub. Set up a new gdt
     and make sure that the segments use it. */
  {
    struct system_gdt *gdt =
#if defined(FIXED_SIZE_GDT) || defined(DYNAMIC_DESCRIPTORS)
      &system_gdt;
#else      
      user_tasks_image.low_level.system_gdt;
#endif 
    gdt->null_descriptor = null_descriptor;
    gdt->kernel_code_descriptor = kernel_code_descriptor;
    gdt->kernel_data_descriptor = kernel_data_descriptor;
    /* Initialization of TSS. */
    for(int i = 0; i < NUM_CPUS; i++){
      gdt->tss_descriptor[i] =
        create_tss_descriptor((uint32_t) &tss_array[i], sizeof(tss_array[i]), 3,0,0);
      tss_array[i].ss0 = gdt_segment_selector(0,KERNEL_DATA_SEGMENT_INDEX);
    }

    lgdt((segment_descriptor_t *) gdt,sizeof(struct system_gdt) + user_tasks_image.nb_tasks * sizeof(struct user_task_descriptors));
    /* terminal_writestring("After lgdt\n"); */

    load_code_segment(gdt_segment_selector(0,KERNEL_CODE_SEGMENT_INDEX));
    /* terminal_writestring("after load_cs\n"); */
    
    load_data_segments(gdt_segment_selector(0,KERNEL_DATA_SEGMENT_INDEX));
    /* terminal_writestring("after load data segments\n"); */
    
    load_tr(gdt_segment_selector(0,TSS_SEGMENTS_FIRST_INDEX));
  }

  /* Set-up the idt. */
  init_interrupts();

  //terminal_writestring("Switching to userpsace\n");

  timer_init();
  
  /* high_level_init(); */

}
