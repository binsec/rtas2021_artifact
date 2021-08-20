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
 
#include <hw/console.hh>
#include <iostream>
#include <cstring>

namespace SSv8 {
  Console::Console()
    : Peripheral( 0x80000100, 0x8000010f )
  {}
  
  bool
  Console::read( uint32_t _addr, uint32_t _size, uint8_t* _value ) {
    if( (_addr & 0xfffffff0) != 0x80000100 or _size != 4 ) return false;
    uint32_t reg = (_addr / 4) % 4;
    if( reg == 1 ) {
      static const uint8_t status_ready[4] = {0,0,0,5};
      bytes_copy( _value, status_ready, 4 );
      return true;
    }
    // Not implemented
    return false;
  }

  bool
  Console::write( uint32_t _addr, uint32_t _size, uint8_t const* _value ) {
    if( (_addr & 0xfffffff0) != 0x80000100 or _size != 4 ) return false;
    uint32_t reg = (_addr / 4) % 4;
    if( reg == 0 ) {
      std::cout << char( _value[3] );
      return true;
    }
    // Not implemented
    return true;
  };
};
