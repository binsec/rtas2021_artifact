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
 
#ifndef STAR_ARCH_HPP
#define STAR_ARCH_HPP

#include <fpu.hh>
#include <fwd.hh>
#include <hw/peripheral.hh>
#include <unisim/component/cxx/processor/sparc/trap.hh>
#include <unisim/component/cxx/processor/sparc/asi.hh>
#include <inttypes.h>

namespace Star {
  struct BitField_t {
    uint32_t&           m_value;
    uint32_t            m_shift;
    uint32_t            m_bitcount;
    
    BitField_t( uint32_t& _value, uint32_t _shift, uint32_t _bitcount )
      : m_value( _value ), m_shift( _shift ), m_bitcount( _bitcount ) {}
    
    operator            uint32_t() const { return (m_value >> m_shift) & ((1 << m_bitcount)-1); }
    void                operator = ( uint32_t _newbits ) {
      uint32_t oldbits = (m_value >> m_shift) & ((1 << m_bitcount)-1);
      m_value ^= ((oldbits ^ _newbits) << m_shift);
    }
    
    BitField_t&         operator = ( BitField_t const& _bf ) {
      uint32_t oldbits = (m_value >> m_shift) & ((1 << m_bitcount)-1);
      uint32_t newbits = (_bf.m_value >> _bf.m_shift) & ((1 << _bf.m_bitcount)-1);
      m_value ^= ((oldbits ^ newbits) << m_shift);
      return *this;
    }
  };
  
  struct Arch {
    // Processor State
    bool                          m_execute_mode;
    bool                          m_annul;
    SSv8::Trap_t                  m_trap;
    uint32_t                      m_tbr;
    uint32_t                      m_psr;
    uint32_t                      m_pc;
    uint32_t                      m_npc;
    uint32_t                      m_nnpc;
    int64_t                       m_instcount;
    
    // Integer Registers
    static
    uint32_t const                s_nwindows = 8;
    uint32_t                      m_gpr[32]; // Current window
    uint32_t                      m_wgpr[s_nwindows*16]; // Windowed regs
    uint32_t                      m_wim;
    uint32_t                      m_y;
    
    // Floating Point Registers
    uint32_t                      m_fsr;
    FPRawBank                     m_fprawbank;
    FP32Bank                      m_fp32;
    FP64Bank                      m_fp64;
    FP128Bank                     m_fp128;
    FPIntBank                     m_fpint;

    // Memory system
    struct Page {
      static uint32_t const       s_bits = 12;
      
      Page*                       m_next;
      uint32_t                    m_address;
      uint8_t                     m_storage[1<<s_bits];
      
      Page( Page* _next, uint32_t _address )
        : m_next( _next ), m_address( _address & uint32_t(-1 << s_bits) ) {}
      ~Page() { delete m_next; }
    };
    
    static uint32_t const         s_bits = 12;
    Page*                         m_pages[1<<s_bits];
    
    // Peripherals
    int                           m_peripheralcount;
    SSv8::Peripheral**            m_peripherals;
    
    // Memory stats
    int64_t                       m_asi_accesses[256];
    
    Arch();
    ~Arch();

    void                          pc( uint32_t _pc ) { m_pc = _pc; m_npc = _pc + 4; m_nnpc = _pc + 8; };
    void                          hwtrap( SSv8::Trap_t::TrapType_t  _trap );
    void                          swtrap( uint32_t _idx );
    uint32_t                      nwindows() { return s_nwindows; }
    void                          jmp( uint32_t _nnpc, uint32_t _pcreg );
    
    // PSR access functions
    BitField_t                    impl()  { return BitField_t( m_psr, 28, 4 ); }
    BitField_t                    ver()   { return BitField_t( m_psr, 24, 4 ); }
    BitField_t                    icc()   { return BitField_t( m_psr, 20, 4 ); }
    BitField_t                    n()     { return BitField_t( m_psr, 23, 1 ); }
    BitField_t                    z()     { return BitField_t( m_psr, 22, 1 ); }
    BitField_t                    v()     { return BitField_t( m_psr, 21, 1 ); }
    BitField_t                    c()     { return BitField_t( m_psr, 20, 1 ); }
    BitField_t                    ec()    { return BitField_t( m_psr, 13, 1 ); }
    BitField_t                    ef()    { return BitField_t( m_psr, 12, 1 ); }
    BitField_t                    pil()   { return BitField_t( m_psr,  8, 4 ); }
    BitField_t                    s()     { return BitField_t( m_psr,  7, 1 ); }
    BitField_t                    ps()    { return BitField_t( m_psr,  6, 1 ); }
    BitField_t                    et()    { return BitField_t( m_psr,  5, 1 ); }
    BitField_t                    cwp()   { return BitField_t( m_psr,  0, 5 ); }
    // + read only functions
    bool                          super() { return (m_psr & (1ul << 7)); }
    SSv8::ASI_t                   rqasi() { return (m_psr & (1ul << 7)) ? SSv8::supervisor_data : SSv8::user_data; }
    SSv8::ASI_t                   insn_asi() { return (m_psr & (1ul << 7)) ? SSv8::supervisor_instruction : SSv8::user_instruction; }
    // common conditions
    bool                          condcs()  { return (0xaaaa/*0b1010101010101010*/ >> ((m_psr >> 20) & 15)) & 1; } //  C
    bool                          condcc()  { return (0x5555/*0b0101010101010101*/ >> ((m_psr >> 20) & 15)) & 1; } // ~C
    bool                          condvs()  { return (0xcccc/*0b1100110011001100*/ >> ((m_psr >> 20) & 15)) & 1; } //  V
    bool                          condvc()  { return (0x3333/*0b0011001100110011*/ >> ((m_psr >> 20) & 15)) & 1; } // ~V
    bool                          conde()   { return (0xf0f0/*0b1111000011110000*/ >> ((m_psr >> 20) & 15)) & 1; } //  Z
    bool                          condne()  { return (0x0f0f/*0b0000111100001111*/ >> ((m_psr >> 20) & 15)) & 1; } // ~Z
    bool                          condneg() { return (0xff00/*0b1111111100000000*/ >> ((m_psr >> 20) & 15)) & 1; } //  N
    bool                          condpos() { return (0x00ff/*0b0000000011111111*/ >> ((m_psr >> 20) & 15)) & 1; } // ~N
    bool                          condle()  { return (0xf3fc/*0b1111001111111100*/ >> ((m_psr >> 20) & 15)) & 1; } //  (Z | (N ^ V))
    bool                          condg()   { return (0x0c03/*0b0000110000000011*/ >> ((m_psr >> 20) & 15)) & 1; } // ~(Z | (N ^ V))
    bool                          condl()   { return (0x33cc/*0b0011001111001100*/ >> ((m_psr >> 20) & 15)) & 1; } //  (N ^ V)
    bool                          condge()  { return (0xcc33/*0b1100110000110011*/ >> ((m_psr >> 20) & 15)) & 1; } // ~(N ^ V)
    bool                          condleu() { return (0xfafa/*0b1111101011111010*/ >> ((m_psr >> 20) & 15)) & 1; } //  (C | Z)
    bool                          condgu()  { return (0x0505/*0b0000010100000101*/ >> ((m_psr >> 20) & 15)) & 1; } // ~(C | Z)
    
    // + write only functions
    void                          modicc( uint32_t _mask, uint32_t _n, uint32_t _z, uint32_t _v, uint32_t _c ) {
      m_psr &= ~(_mask << 20); // clearing modified bits
      m_psr |= (((_n << 3) | (_z << 2) | (_v << 1) | (_c << 0)) & _mask) << 20; // applying modification
    }
    
    // TBR access functions
    BitField_t                    tba()   { return BitField_t( m_tbr, 12, 20 ); }
    BitField_t                    tt()    { return BitField_t( m_tbr, 4, 8 ); }
    BitField_t                    zero()  { return BitField_t( m_tbr, 0, 4 ); }
    
    // Register window access functions
    void                          rotate( int32_t _off );
    bool                          invalidwindow( int32_t _off ) { return ((m_wim >> (uint32_t(cwp() + s_nwindows + _off) % s_nwindows)) & 1) != 0; }
    
    // Ancillary state registers (ASR) access functions
    bool                          asr_perm( uint32_t _idx ) { return super(); }
    uint32_t                      rdasr( uint32_t _idx );
    void                          wrasr( uint32_t _idx, uint32_t _value );
    
    // Floating-point Status Register (FSR) access function 
    BitField_t                    frd()   { return BitField_t( m_fsr, 30, 2 ); }
    BitField_t                    ftem()  { return BitField_t( m_fsr, 23, 5 ); }
    BitField_t                    fns()   { return BitField_t( m_fsr, 22, 1 ); }
    BitField_t                    fver()  { return BitField_t( m_fsr, 17, 3 ); }
    BitField_t                    ftt()   { return BitField_t( m_fsr, 14, 3 ); }
    BitField_t                    fqne()  { return BitField_t( m_fsr, 13, 1 ); }
    BitField_t                    fcc()   { return BitField_t( m_fsr, 10, 2 ); }
    BitField_t                    aexc()  { return BitField_t( m_fsr,  5, 5 ); }
    BitField_t                    cexc()  { return BitField_t( m_fsr,  0, 5 ); }
    // + read only functions
    bool                          fconde()   { return (0x1/*0b0001*/ >> ((m_fsr >> 10) & 3)) & 1; }
    bool                          fcondl()   { return (0x2/*0b0010*/ >> ((m_fsr >> 10) & 3)) & 1; }
    bool                          fcondle()  { return (0x3/*0b0011*/ >> ((m_fsr >> 10) & 3)) & 1; }
    bool                          fcondg()   { return (0x4/*0b0100*/ >> ((m_fsr >> 10) & 3)) & 1; }
    bool                          fcondge()  { return (0x5/*0b0101*/ >> ((m_fsr >> 10) & 3)) & 1; }
    bool                          fcondlg()  { return (0x6/*0b0110*/ >> ((m_fsr >> 10) & 3)) & 1; }
    bool                          fcondo()   { return (0x7/*0b0111*/ >> ((m_fsr >> 10) & 3)) & 1; }
    bool                          fcondu()   { return (0x8/*0b1000*/ >> ((m_fsr >> 10) & 3)) & 1; }
    bool                          fcondue()  { return (0x9/*0b1001*/ >> ((m_fsr >> 10) & 3)) & 1; }
    bool                          fcondul()  { return (0xa/*0b1010*/ >> ((m_fsr >> 10) & 3)) & 1; }
    bool                          fcondule() { return (0xb/*0b1011*/ >> ((m_fsr >> 10) & 3)) & 1; }
    bool                          fcondug()  { return (0xc/*0b1100*/ >> ((m_fsr >> 10) & 3)) & 1; }
    bool                          fconduge() { return (0xd/*0b1101*/ >> ((m_fsr >> 10) & 3)) & 1; }
    bool                          fcondne()  { return (0xe/*0b1110*/ >> ((m_fsr >> 10) & 3)) & 1; }
    
    Page*                         getpage( uint32_t _addr );
    
    SSv8::Peripheral*             peripheral( uint32_t addr );
    void                          add( SSv8::Peripheral& _peripheral );
    
    void                          uninitialized_data( SSv8::ASI_t _asi, uint32_t _addr, uint32_t _size );
    
    bool                          read( SSv8::ASI_t _asi, uint32_t _addr, uint32_t _size, uint8_t* _value ) {
      m_asi_accesses[_asi&0xff] ++;
      if( (_addr % _size) or _size > (1 << Page::s_bits) ) return false;
      uint32_t pageidx = _addr >> Page::s_bits;
      uint32_t pageaddr = pageidx << Page::s_bits;
      pageidx = pageidx % (1 << s_bits);
      uint32_t offset = _addr - pageaddr;

      Page* match = m_pages[pageidx];
      if( match and match->m_address != pageaddr ) {
        match = 0;
        for( Page* node = m_pages[pageidx]; node->m_next; node = node->m_next ) {
          if( node->m_next->m_address != pageaddr ) continue;
          match = node->m_next;
          node->m_next = match->m_next;
          match->m_next = m_pages[pageidx];
          m_pages[pageidx] = match;
          break;
        }
      }
      if( not match ) {
        SSv8::Peripheral* target = peripheral( _addr );
        if( target and target->read( _addr, _size, _value ) )
          return true;
        return uninitialized_data( _asi, _addr, _size ), false;
      }
      uint8_t const* storage = &(match->m_storage[offset]);
      if( _size == 1 )      *_value = *storage;
      else if( _size == 2 ) *((uint16_t*)( _value )) = *((uint16_t const*)( storage ));
      else if( _size == 4 ) *((uint32_t*)( _value )) = *((uint32_t const*)( storage ));
      else if( _size == 8 ) *((uint64_t*)( _value )) = *((uint64_t const*)( storage ));
      else { for( uint32_t idx = 0; idx < _size; ++ idx ) _value[idx] = storage[idx]; }
      return true;
    }
    
    bool                          write( SSv8::ASI_t _asi, uint32_t _addr, uint32_t _size, uint8_t const* _value ) {
      m_asi_accesses[_asi&0xff] ++;
      if( (_addr % _size) or _size > (1 << Page::s_bits) ) return false;
      uint32_t pageidx = _addr >> Page::s_bits;
      uint32_t pageaddr = pageidx << Page::s_bits;
      pageidx = pageidx % (1 << s_bits);
      uint32_t offset = _addr - pageaddr;

      Page* match = m_pages[pageidx];
      if( match and match->m_address != pageaddr ) {
        match = 0;
        for( Page* node = m_pages[pageidx]; node->m_next; node = node->m_next ) {
          if( node->m_next->m_address != pageaddr ) continue;
          match = node->m_next;
          node->m_next = match->m_next;
          match->m_next = m_pages[pageidx];
          m_pages[pageidx] = match;
          break;
        }
      }
      
      if( not match ) {
        SSv8::Peripheral* target = peripheral( _addr );
        if( target ) return target->write( _addr, _size, _value );
        match = new Page( m_pages[pageidx], pageaddr );
        m_pages[pageidx] = match;
      }
      
      uint8_t* storage = &(match->m_storage[offset]);
      
      if( _size == 1 )      *storage = *_value;
      else if( _size == 2 ) *((uint16_t*)( storage )) = *((uint16_t*)( _value ));
      else if( _size == 4 ) *((uint32_t*)( storage )) = *((uint32_t*)( _value ));
      else if( _size == 8 ) *((uint64_t*)( storage )) = *((uint64_t*)( _value ));
      else { for( uint32_t idx = 0; idx < _size; ++ idx ) storage[idx] = _value[idx]; }
      return true;
    }

    void                          memcpy( uint32_t _addr, uint8_t const* _buffer, uint32_t _size );
  };
} // end of namespace Star

#endif // STAR_ARCH_HPP

/*
  Emacs lisp utility functions
  
  (defun bintohex (beg end)
  "Maintains a dual hex/bin constant."
    (interactive "*r")
      (save-excursion
        (goto-char beg)
        (while (re-search-forward "0x\\([A-Fa-f0-9]+\\)/\\*0b\\([01]+\\)\\*\/" end t)
          (replace-match (format "0x%x/\*0b\\2*\/" (string-to-int (match-string 2) 2)))
          )
      )
    )
*/
