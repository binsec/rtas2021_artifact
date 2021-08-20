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

#ifndef SSV8_HW_TRAP_HPP
#define SSV8_HW_TRAP_HPP

#include <inttypes.h>

namespace SSv8 {
  struct Trap_t {
    char const* m_name;
    uint8_t     m_priority;
    uint8_t     m_traptype;
    
    Trap_t&     operator=( Trap_t const& _trap ) { m_name = _trap.m_name; m_priority = _trap.m_priority; m_traptype = _trap.m_traptype; return *this; }
    void        clear() { m_name = 0; m_priority = 0xff; m_traptype = 0; }
    operator    bool() const { return bool( m_name ); }
    
    enum TrapType_t {
      data_store_error = 0,
      instruction_access_MMU_miss,
      instruction_access_error,
      r_register_access_error,
      instruction_access_exception,
      privileged_instruction,
      illegal_instruction,
      fp_disabled,
      cp_disabled,
      unimplemented_FLUSH,
      watchpoint_detected,
      window_overflow,
      window_underflow,
      mem_address_not_aligned,
      fp_exception,
      cp_exception,
      data_access_error,
      data_access_MMU_miss,
      data_access_exception,
      tag_overflow,
      division_by_zero,
      trap_instruction,
      interrupt_level_15,
      interrupt_level_14,
      interrupt_level_13,
      interrupt_level_12,
      interrupt_level_11,
      interrupt_level_10,
      interrupt_level_9,
      interrupt_level_8,
      interrupt_level_7,
      interrupt_level_6,
      interrupt_level_5,
      interrupt_level_4,
      interrupt_level_3,
      interrupt_level_2,
      interrupt_level_1,
      trapcount
    };
    
    static Trap_t s_hardware[];
  };
}

#endif // SSV8_HW_TRAP_HPP
