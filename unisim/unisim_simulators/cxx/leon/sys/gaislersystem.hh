/*
 *  Copyright (c) 2007-2020,
 *  Commissariat a l'Energie Atomique (CEA),
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */
 
#ifndef SSV8_SYS_GAISLERSYSTEM_HH
#define SSV8_SYS_GAISLERSYSTEM_HH

#include <inttypes.h>

namespace SSv8 {
  template <class t_Arch_t>
  struct GaislerSystem {
    t_Arch_t&                    m_arch;
    
    GaislerSystem( t_Arch_t& _arch )
      : m_arch( _arch ) {}
    
    void load( char const* _imgpath );
    
    enum Exception_t { OpenError, ImageError };
    
    struct Loc {
      static uint32_t const MCFG1     = 0x10380233;
      static uint32_t const MCFG2     = 0xe6A06e60;
      static uint32_t const MCFG3     = 0x000ff000;
      static uint32_t const ASDCFG    = 0xfff00100;
      static uint32_t const DSDCFG    = 0xe6A06e60;
      static uint32_t const L2MCTRLIO = 0x80000000;
      static uint32_t const IRQCTRL   = 0x80000200;
      static uint32_t const RAMSTART  = 0x40000000;
      static uint32_t const RAMSIZE   = 0x00100000;
      static uint32_t const STACKSIZE = 0x00002000;
    };
  };
};

#endif // SSV8_SYS_GAISLERSYSTEM_HH
