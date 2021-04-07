#ifndef __SEGMENT_H__
#define __SEGMENT_H__

/* X86 segment descriptors and selectors. */


/* Returns a segment selector. */

#define __segment_selector(privilege,in_ldt,index)                     \
  (((privilege) & 3) | (((in_ldt)? 1 : 0) << 2) | ((index) << 3))

#define gdt_segment_selector(privilege,index) __segment_selector(privilege,false,index)
#define ldt_segment_selector(privilege,index) __segment_selector(privilege,true,index)

typedef uint16_t segment_selector_t;

/* TODO: Pass the whole segment reg as an argument, and use
   GDT_SEGMENT_REG in the kernel. */

static inline void load_code_segment(segment_selector_t newcs){  
  asm volatile ("ljmp %0,$1f\n1:\n"
                :
                : "i"(newcs)
                : "memory");
}

/* Loads ds,es,fs,gs,and ss with the same data segment. Usually what we want. */
static inline void load_data_segments(segment_selector_t newds){
  asm volatile ("movw %0, %%ax   \n\
                 movw %%ax,  %%ss\n\
                 movw %%ax,  %%ds\n\
                 movw %%ax,  %%es\n\
                 movw %%ax,  %%fs\n\
                 movw %%ax,  %%gs"
                :
                : "i"(newds) 
                : "memory","eax");
}
#define load_data_segments(newds) asm volatile ("movw %0, %%ax   \n movw %%ax,  %%ss\n movw %%ax,  %%ds\n movw %%ax,  %%es\n movw %%ax,  %%fs\n movw %%ax,  %%gs" : : "i"(newds) : "memory","eax")
#define load_code_segment(newcs)   asm volatile ("ljmp %0,$1f\n1:\n" : : "i"(newcs) : "memory")

static inline void load_ds_reg(segment_selector_t newds){
  asm volatile ("movw %0,  %%ds\n"
                : 
                : "g"(newds) 
                : "memory");
}


static inline void load_ds_imm(segment_selector_t newds){
  asm volatile ("movw %0, %%ax   \n\
                 movw %%ax,  %%ds\n"
                :
                : "i"(newds) 
                : "memory","eax");
}


static inline void load_es(segment_selector_t newes){
  asm volatile ("movw %0, %%ax   \n\
                 movw %%ax,  %%es\n"
                :
                : "i"(newes) 
                : "memory","eax");
}

static inline void load_fs(segment_selector_t newfs){
  asm volatile ("movw %0, %%ax   \n\
                 movw %%ax,  %%fs\n"
                :
                : "i"(newfs) 
                : "memory","eax");
}

static inline void load_gs(segment_selector_t newgs){
  asm volatile ("movw %0, %%ax   \n\
                 movw %%ax,  %%gs\n"
                :
                : "i"(newgs) 
                : "memory","eax");
}

static inline void load_ss(segment_selector_t newss){
  asm volatile ("movw %0, %%ax   \n\
                 movw %%ax,  %%ss\n"
                :
                : "i"(newss) 
                : "memory","eax");
}

/* Note that task register cannot be in a LDT. */
static inline void load_tr(segment_selector_t ntr){
  asm volatile ("ltr %0" : :"r"(ntr) : "memory");
}


#endif
