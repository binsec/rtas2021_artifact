/*
 *  Copyright (c) 2007-2020,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY 
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <unisim/component/cxx/processor/sparc/trap.hh>

namespace SSv8 {
  Trap_t Trap_t::s_hardware[] = {
    { "data_store_error",             2, 0x2B },
    { "instruction_access_MMU_miss",  2, 0x3C },
    { "instruction_access_error",     3, 0x21 },
    { "r_register_access_error",      4, 0x20 },
    { "instruction_access_exception", 5, 0x01 },
    { "privileged_instruction",       6, 0x03 },
    { "illegal_instruction",          7, 0x02 },
    { "fp_disabled",                  8, 0x04 },
    { "cp_disabled",                  8, 0x24 },
    { "unimplemented_FLUSH",          8, 0x25 },
    { "watchpoint_detected",          8, 0x0B },
    { "window_overflow",              9, 0x05 },
    { "window_underflow",             9, 0x06 },
    { "mem_address_not_aligned",     10, 0x07 },
    { "fp_exception",                11, 0x08 },
    { "cp_exception",                11, 0x28 },
    { "data_access_error",           12, 0x29 },
    { "data_access_MMU_miss",        12, 0x2C },
    { "data_access_exception",       13, 0x09 },
    { "tag_overflow",                14, 0x0A },
    { "division_by_zero",            15, 0x2A },
    { "trap_instruction",            16, 0x80 },
    { "interrupt_level_15",          17, 0x1F },
    { "interrupt_level_14",          18, 0x1E },
    { "interrupt_level_13",          19, 0x1D },
    { "interrupt_level_12",          20, 0x1C },
    { "interrupt_level_11",          21, 0x1B },
    { "interrupt_level_10",          22, 0x1A },
    { "interrupt_level_9",           23, 0x19 },
    { "interrupt_level_8",           24, 0x18 },
    { "interrupt_level_7",           25, 0x17 },
    { "interrupt_level_6",           26, 0x16 },
    { "interrupt_level_5",           27, 0x15 },
    { "interrupt_level_4",           28, 0x14 },
    { "interrupt_level_3",           29, 0x13 },
    { "interrupt_level_2",           30, 0x12 },
    { "interrupt_level_1",           31, 0x11 }
  };
  
}
