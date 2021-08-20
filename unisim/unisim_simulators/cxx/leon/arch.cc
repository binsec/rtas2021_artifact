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
 
#include <arch.hh>
#include <hw/peripheral.hh>
#include <utils/cfmt.hh>
#include <cstring>
#include <iostream>

using SSv8::CFmt;

namespace Star {
  Arch::Arch()
    : m_execute_mode( true ), m_annul( false ),
      m_tbr( 0 ), m_psr( 0 ), m_pc( 0 ), m_npc( 0 ), m_nnpc( 0 ), m_instcount( 0 ),
      m_wim( 0xffffffff ), m_y( 0 ),
      m_fp32( m_fprawbank ), m_fp64( m_fprawbank ), m_fp128( m_fprawbank ), m_fpint( m_fprawbank ),
      m_peripheralcount( 0 ), m_peripherals( 0 )
  {
    for( uint32_t idx = 0; idx < (1<<s_bits); ++idx )
      m_pages[idx] = 0;
    for( int idx = 0; idx < 256; ++idx )
      m_asi_accesses[idx] = 0;
    m_trap.clear();
  }
  
  Arch::~Arch() {
    for( uint32_t idx = 0; idx < (1<<s_bits); ++idx )
      delete m_pages[idx];
    delete [] m_peripherals;
  }
  
  void
  Arch::hwtrap( SSv8::Trap_t::TrapType_t  _traptype ) {
    SSv8::Trap_t const& trap = SSv8::Trap_t::s_hardware[_traptype];
    if( m_trap.m_name and trap.m_priority > m_trap.m_priority ) return;
    m_trap = trap;
  }
  
  void
  Arch::swtrap( uint32_t _idx ) {
    SSv8::Trap_t const& trap = SSv8::Trap_t::s_hardware[SSv8::Trap_t::trap_instruction];
    if( m_trap.m_name and trap.m_priority > m_trap.m_priority ) return;
    m_trap = trap;
    m_trap.m_traptype = _idx | 0x80;
    if( _idx == 0 ) {
      /* Halting execution (Gaisler specific) */;
      m_execute_mode = false;
    }
  }
  
  void
  Arch::rotate( int32_t _off ) {
    // Export GPRs
    std::memcpy( &(m_wgpr[(cwp() % s_nwindows)*16]),     &(m_gpr[8]),  16*sizeof( uint32_t ) );
    std::memcpy( &(m_wgpr[((cwp()+1) % s_nwindows)*16]), &(m_gpr[24]),  8*sizeof( uint32_t ) );
    
    cwp() = (cwp() + s_nwindows + _off) % s_nwindows;
    
    // Import GPRs
    std::memcpy( &(m_gpr[8]),  &(m_wgpr[(cwp() % s_nwindows)*16]),     16*sizeof( uint32_t ) );
    std::memcpy( &(m_gpr[24]), &(m_wgpr[((cwp()+1) % s_nwindows)*16]),  8*sizeof( uint32_t ) );
  }

  void
  Arch::jmp( uint32_t _nnpc, uint32_t _pcreg ) {
    m_gpr[_pcreg] = m_pc;
    m_nnpc = _nnpc;
  }
  
  uint32_t
  Arch::rdasr( uint32_t _idx ) {
    switch( _idx & 0x1f ) {
    case 17:
      return
        (((0 /* IDX */)  & 0x0f) << 28) |
        (((0 /* DWT */)  & 0x01) << 14) |
        (((0 /* SVT */)  & 0x01) << 13) |
        (((0 /* LD */)   & 0x01) << 12) |
        (((1 /* FPU */)  & 0x03) << 10) |
        (((0 /* MAC */)  & 0x01) <<  9) |
        (((1 /* V8 */)   & 0x01) <<  8) |
        (((0 /* NWP */)  & 0x07) <<  4) |
        (((s_nwindows-1) & 0x1f) <<  0);
      break;
    default:
      hwtrap( SSv8::Trap_t::illegal_instruction );
      break;
    }
    return 0;
  }
  
  void
  Arch::wrasr( uint32_t _idx, uint32_t _value ) {
    switch( _idx & 0x1f ) {
    case 17:
      if( ((_value >> 14) & 0x01) == 1 ) { hwtrap( SSv8::Trap_t::illegal_instruction ); return; }
      break;
    default:
      hwtrap( SSv8::Trap_t::illegal_instruction );
      break;
    }
  }

  void
  Arch::uninitialized_data( SSv8::ASI_t _asi, uint32_t _addr, uint32_t _size ) {
    std::cerr << CFmt( "Use of uninitialized data: [%#x]%#x, %d\n", _asi, _addr, _size );
  }

  void
  Arch::memcpy( uint32_t _addr, uint8_t const* _buffer, uint32_t _size ) {
    {
      uint32_t offset = _addr % (1 << Page::s_bits);
      if( offset ) {
        uint32_t size = (1 << Page::s_bits) - offset;
        size = size < _size ? size : _size;
        std::memcpy( (void*)(getpage( _addr )->m_storage + offset), (void const*)_buffer, size );
        _size -= size;
        _buffer += size;
        _addr += size;
      }
    }
  
    for( Page* cur; _size > 0 and (cur = getpage( _addr )); ) {
      uint32_t size = 1 << s_bits;
      size = size < _size ? size : _size;
      std::memcpy( (void*)(cur->m_storage), (void const*)_buffer, size );
      _size -= size;
      _buffer += size;
      _addr += size;
    }
  }

  Arch::Page*
  Arch::getpage( uint32_t _addr ) {
    uint32_t pageidx = _addr >> Page::s_bits;
    uint32_t pageaddr = pageidx << Page::s_bits;
    pageidx = pageidx % (1 << s_bits);
  
    Page* match = m_pages[pageidx];
    while( match and match->m_address != pageaddr ) match = match->m_next;
    return ( match ? match : (m_pages[pageidx] = new Page( m_pages[pageidx], pageaddr )) );
  }

  SSv8::Peripheral*
  Arch::peripheral( uint32_t _addr ) {
    int first = 0, last = m_peripheralcount - 1;
    if( _addr < m_peripherals[first]->m_faddr or _addr > m_peripherals[last]->m_laddr )
      return 0;
    if( _addr <= m_peripherals[first]->m_laddr )
      return m_peripherals[first];
    if( _addr >= m_peripherals[last]->m_faddr )
      return m_peripherals[last];
    
    while( (last - first) > 1  ) { /* at least one item in between */
      int mid = (last+first)/2;
      if( _addr < m_peripherals[mid]->m_faddr ) { last = mid; continue; }
      if( _addr > m_peripherals[mid]->m_laddr ) { first = mid; continue; }
      return m_peripherals[mid];
    }
    return 0;
  }
  
  void
  Arch::add( SSv8::Peripheral& _peripheral ) {
    SSv8::Peripheral* ins = &(_peripheral);
    if( m_peripheralcount % 8 == 0 ) {
      SSv8::Peripheral** nperipherals = new SSv8::Peripheral* [(m_peripheralcount+8)];
      for( int idx = 0; idx < m_peripheralcount; idx += 1 ) nperipherals[idx] = m_peripherals[idx];
      delete [] m_peripherals;
      m_peripherals = nperipherals;
    }
    for( int idx = 0; idx < m_peripheralcount; idx += 1 ) {
      if( m_peripherals[idx]->m_laddr < ins->m_faddr ) continue;
      if( m_peripherals[idx]->m_faddr > ins->m_laddr ) {
        SSv8::Peripheral* tmp = m_peripherals[idx];
        m_peripherals[idx] = ins;
        ins = tmp;
      } else {
        std::cerr << CFmt( "Ranges overlap: [%#x,%#x] !! [%#x,%#x]\n",
                           m_peripherals[idx]->m_faddr, m_peripherals[idx]->m_laddr, ins->m_faddr, ins->m_laddr );
        throw SSv8::Peripheral::RangeError;
      }
    }
    m_peripherals[m_peripheralcount++] = ins;
  }

} // end of namespace ASV8
