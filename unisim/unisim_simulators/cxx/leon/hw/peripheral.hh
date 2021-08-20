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
 
#ifndef SSV8_HW_PERIPHERAL_HH
#define SSV8_HW_PERIPHERAL_HH

#include <inttypes.h>

namespace SSv8 {
  struct Peripheral {
    enum Exception_t { RangeError };
    uint32_t                    m_faddr;
    uint32_t                    m_laddr;
    
    Peripheral( uint32_t _faddr, uint32_t _laddr )
      : m_faddr( _faddr ), m_laddr( _laddr ) { if( _laddr < _faddr ) throw RangeError; }
    
    virtual ~Peripheral() {};
    virtual bool                read( uint32_t _addr, uint32_t _size, uint8_t* _value ) = 0;
    virtual bool                write( uint32_t _addr, uint32_t _size, uint8_t const* _value ) = 0;
    
    static void                 bytes_copy( uint8_t* _dst, uint8_t const* _src, uintptr_t _size ) {
      if( _size == 1 )      *_dst = *_src;
      else if( _size == 2 ) *((uint16_t*)( _dst )) = *((uint16_t const*)( _src ));
      else if( _size == 4 ) *((uint32_t*)( _dst )) = *((uint32_t const*)( _src ));
      else if( _size == 8 ) *((uint64_t*)( _dst )) = *((uint64_t const*)( _src ));
      else { for( uint32_t idx = 0; idx < _size; ++ idx ) _dst[idx] = _src[idx]; }
    }
  };
};

#endif // SSV8_HW_PERIPHERAL_HH
