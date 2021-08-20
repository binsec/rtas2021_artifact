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
 
#ifndef SSV8_HW_PNP_HH
#define SSV8_HW_PNP_HH

#include <inttypes.h>
#include <hw/peripheral.hh>

#include <fwd.hh>
#include <iosfwd>

namespace SSv8 {
  struct PNP : public SSv8::Peripheral {
    /* Plug 'n Play Controller */
    uint8_t*                    m_word;
    
    PNP( uint32_t _faddr, uint32_t _laddr ) : SSv8::Peripheral( _faddr, _laddr ) {};
    virtual ~PNP() {};
      

    bool                        read( uint32_t _addr, uint32_t _size, uint8_t* _value ) {
      m_word = _value;
      if( _size > 4 or not getreg( (_addr - m_faddr) / 4 ) ) return false;
      return true;
    }
    
    bool                        write( uint32_t _addr, uint32_t _size, uint8_t const* _value ) { /* Read only */ return false; }
    void                        setreg( uint32_t _value );
    virtual bool                getreg( uint32_t _idx ) = 0;
  };

  struct AHBPNP : public PNP {
    AHBPNP() : PNP( 0xfffff000, 0xffffffff ) {}
    
    bool                        getreg( uint32_t _idx );
  };
  
  struct APBPNP : public PNP {
    APBPNP() : PNP( 0x800ff000, 0x800fffff ) {}
    
    bool                        getreg( uint32_t _idx );
  };
};

#endif // SSV8_HW_PNP_HH
