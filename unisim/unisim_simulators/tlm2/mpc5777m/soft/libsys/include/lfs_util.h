/*
 * lfs utility functions
 *
 * Copyright (c) 2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef LFS_UTIL_H
#define LFS_UTIL_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>


// Builtin functions, these may be replaced by more
// efficient implementations in the system
static inline uint32_t lfs_max(uint32_t a, uint32_t b) {
    return (a > b) ? a : b;
}

static inline uint32_t lfs_min(uint32_t a, uint32_t b) {
    return (a < b) ? a : b;
}

static inline uint32_t lfs_ctz(uint32_t a) {
    return __builtin_ctz(a);
}

static inline uint32_t lfs_npw2(uint32_t a) {
    return 32 - __builtin_clz(a-1);
}

static inline uint32_t lfs_popc(uint32_t a) {
    return __builtin_popcount(a);
}

static inline int lfs_scmp(uint32_t a, uint32_t b) {
    return (int)(unsigned)(a - b);
}

// CRC-32 with polynomial = 0x04c11db7
void lfs_crc(uint32_t *crc, const void *buffer, size_t size);


// Logging functions, these may be replaced by system-specific
// logging functions
#define LFS_DEBUG(fmt, ...) printf("lfs debug: " fmt "\n", __VA_ARGS__)
#define LFS_WARN(fmt, ...)  printf("lfs warn: " fmt "\n", __VA_ARGS__)
#define LFS_ERROR(fmt, ...) printf("lfs error: " fmt "\n", __VA_ARGS__)


#endif
