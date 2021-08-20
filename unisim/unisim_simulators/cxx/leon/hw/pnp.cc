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
 
#include <hw/pnp.hh>

#define VENDORID( VALUE )  ((VALUE & 0x00ff) << 24)
#define DEVICEID( VALUE )  ((VALUE & 0x0fff) << 12)
#define VERSION( VALUE )   ((VALUE & 0x001f) <<  5)
#define IRQ( VALUE )       ((VALUE & 0x001f) <<  0)
#define ADDR( VALUE )      ((VALUE & 0x0fff) << 20)
#define PREFETCHABLE       (0x01 << 17)
#define CACHEABLE          (0x01 << 16)
#define MASK( VALUE )      ((VALUE & 0x0fff) <<  4)
#define TYPE( VALUE )      ((VALUE & 0x000f) <<  0)

namespace SSv8 {
  void
  PNP::setreg( uint32_t _value ) {
    m_word[0] = (_value >> 24) & 0xff;
    m_word[1] = (_value >> 16) & 0xff;
    m_word[2] = (_value >>  8) & 0xff;
    m_word[3] = (_value >>  0) & 0xff;
  }
 
  bool
  AHBPNP::getreg( uint32_t _idx ) {
    uint32_t unit = _idx / 8, reg = _idx % 8;
    switch( unit ) {
    case 64: // European Space Agency; Leon2 Memory Controller
      switch( reg ) {
      case 0: setreg( VENDORID( 4 ) | DEVICEID( 15 ) | VERSION( 1 ) | IRQ( 0 ) ); return true;
      }
      break;
    case 65: // Gaisler Research, AHB/APB Bridge
      switch( reg ) {
      case 0: setreg( VENDORID( 1 ) | DEVICEID( 6 ) | VERSION( 0 ) | IRQ( 0 ) ); return true;
      case 4: setreg( ADDR( 0x0800 ) | MASK( 0xfff ) | TYPE( 2 ) ); return true;
      }
      break;
    }
    return false;
  }
  
  bool
  APBPNP::getreg( uint32_t _idx ) {
    uint32_t unit = _idx / 2, reg = _idx % 2;
    switch( unit ) {
    case 0: // European Space Agency; Leon2 Memory Controller
      switch( reg ) {
      case 0: setreg( VENDORID( 4 ) | DEVICEID( 15 ) | VERSION( 1 ) | IRQ( 0 ) ); return true;
      }
      break;
    case 1: // Gaisler Research, Generic UART
      switch( reg ) {
      case 0: setreg( VENDORID( 1 ) | DEVICEID( 12 ) | VERSION( 1 ) | IRQ( 2 ) ); return true;
      case 1: setreg( ADDR( 0x0001 ) | MASK( 0xfff ) | TYPE( 1 ) ); return true;
      }
      break;
    case 2: // Gaisler Research, Multi-processor Interrupt Ctrl.
      switch( reg ) {
      case 0: setreg( VENDORID( 1 ) | DEVICEID( 13 ) | VERSION( 3 ) | IRQ( 0 ) ); return true;
      }
    case 3: // Gaisler Research, Modular Timer Unit
      switch( reg ) {
      case 0: setreg( VENDORID( 1 ) | DEVICEID( 17 ) | VERSION( 0 ) | IRQ( 8 ) ); return true;
      case 1: setreg( ADDR( 0x0003 ) | MASK( 0xfff ) | TYPE( 1 ) ); return true;
      }
      break;
    }
    return false;
  }
};

#undef VENDORID
#undef DEVICEID
#undef VERSION
#undef IRQ
#undef ADDR
#undef PREFETCHABLE
#undef CACHEABLE
#undef MASK
#undef TYPE
