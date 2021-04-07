#include "copy64.h"
#include <stdint.h>

void copy64(uint64_t* dst, uint64_t* src) {
  asm volatile
    ("movq %1,%%mm0 \n\
      movq %%mm0, %0 \n" : "=m"(*dst) : "m"(*src)/* : "mm0"*/);
}

