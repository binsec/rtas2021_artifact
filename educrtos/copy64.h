#ifndef COPY64_H
#define COPY64_H

#include <stdint.h>

/* Atomic copy of 64 bits. */
void copy64(uint64_t* dst, uint64_t* src);

#endif /* COPY64_H */

