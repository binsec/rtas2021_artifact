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
 
#ifndef STAR_FPU_HPP
#define STAR_FPU_HPP

#include <inttypes.h>

namespace Star {
  /*
    Sun fp registers
   */
  struct FPBank {
    uint32_t                m_dirtymask;
    
    
    FPBank() : m_dirtymask( 0xffffffff ) {}
    virtual ~FPBank() {}
    virtual void            writeback( uint32_t _idx, uint8_t* _storage ) = 0;
    virtual uint32_t        words() = 0;
  };
  
  struct FPRawBank {
    uint8_t                 m_storage[128];
    int                     m_bankscount;
    FPBank**                m_banks;
    
    FPRawBank() : m_bankscount( 0 ), m_banks( 0 ) {}
    ~FPRawBank() { delete [] m_banks; }
    
    uint8_t*                get( uint32_t _words, uint32_t _idx ) {
      // Doing necessary writebacks
      uint32_t wbmask = ((1 << _words) - 1) << (_idx*_words);
      for( int idx = m_bankscount; (--idx) >= 0; ) {
        if( ((m_banks[idx]->m_dirtymask) & wbmask) == wbmask ) continue;
        uint32_t subwords = m_banks[idx]->words();
        uint32_t subwbmask = ((1 << subwords) - 1);
        for( uint32_t subidx = 0; subidx < 32; (subwbmask = subwbmask << subwords), subidx += subwords ) {
          if( not (subwbmask & wbmask) or ((m_banks[idx]->m_dirtymask) & subwbmask) == subwbmask ) continue;
          m_banks[idx]->writeback( subidx / subwords, &m_storage[subidx*4] );
          m_banks[idx]->m_dirtymask |= subwbmask;
        }
      }
      return &m_storage[_idx*_words*4];
    }
    
    uint8_t*                update( uint32_t _words, uint32_t _idx ) {
      // Doing necessary writebacks
      uint32_t wbmask = ((1 << _words) - 1) << (_idx*_words);
      for( int idx = m_bankscount; (--idx) >= 0; ) {
        if( ((m_banks[idx]->m_dirtymask) & wbmask) == wbmask ) continue;
        uint32_t subwords = m_banks[idx]->words();
        uint32_t subwbmask = ((1 << subwords) - 1);
        for( uint32_t subidx = 0; subidx < 32; (subwbmask = subwbmask << subwords), subidx += subwords ) {
          if( not (subwbmask & wbmask) or ((m_banks[idx]->m_dirtymask) & subwbmask) == subwbmask ) continue;
          m_banks[idx]->writeback( subidx / subwords, &m_storage[subidx*4] );
        }
      }
      return &m_storage[_idx*_words*4];
    }
    
    void                    add( FPBank* _bank ) {
      if( (m_bankscount % 8) == 0 ) {
        FPBank** nbanks = new FPBank*[m_bankscount+8];
        for( int idx = m_bankscount; (--idx) >= 0; ) nbanks[idx] = m_banks[idx];
        delete [] m_banks;
        m_banks = nbanks;
      }
      m_banks[m_bankscount++] = _bank;
    }
    
    void                    load32( uint8_t const* _storage, int _idx ) {
      uint32_t const* src = (uint32_t const*)( _storage );
      uint32_t*       dst = (uint32_t*)( &m_storage[_idx*4] );
      *dst = *src;
      uint32_t dirtymask = (uint32_t( 0x1 ) << _idx);
      for( int idx = m_bankscount; (--idx) >= 0; )
        m_banks[idx]->m_dirtymask |= dirtymask;
    }
    
    void                    load64( uint8_t const* _storage, int _idx ) {
      uint64_t const* src = (uint64_t const*)( _storage );
      uint64_t*       dst = (uint64_t*)( &m_storage[_idx*4] );
      *dst = *src;
      uint32_t dirtymask = (uint32_t( 0x3 ) << _idx);
      for( int idx = m_bankscount; (--idx) >= 0; )
        m_banks[idx]->m_dirtymask |= dirtymask;
    }
    
    void                    store32( int _idx, uint8_t* _storage ) {
      uint32_t const* src = (uint32_t const*)( this->update( 1, _idx ) );
      uint32_t*       dst = (uint32_t*)( _storage );
      *dst = *src;
    }
    
    void                    store64( int _idx, uint8_t* _storage ) {
      uint64_t const* src = (uint64_t const*)( this->update( 1, _idx ) );
      uint64_t*       dst = (uint64_t*)( _storage );
      *dst = *src;
    }
  };
  
  // Hopefully native float type is IEEE.754 and its memory
  // endianess is consistent with that of native uint32_t. This
  // way, we can reinterpret IEEE.754 as uint32_t and use portable
  // shift operations.

  struct FP32Bank : public FPBank {
    static uint32_t const   s_words = 1;
    float                   m_regs[32/s_words];
    FPRawBank&              m_rawbank;
    
    FP32Bank( FPRawBank& _rawbank ) : m_rawbank( _rawbank ) { _rawbank.add( this ); }
    
    uint32_t                words() { return s_words; }
    
    void                    writeback( uint32_t _idx, uint8_t* _storage ) {
      uint32_t* src = (uint32_t*)(&m_regs[_idx]);
      uint32_t uword = *src;
      _storage[0] = uword >> 24;
      _storage[1] = uword >> 16;
      _storage[2] = uword >>  8;
      _storage[3] = uword >>  0;
    }
    
    float&                  operator[]( uint32_t _idx ) {
      uint32_t regmask = ((1 << s_words)-1) << _idx;
      _idx /= s_words;
      if( m_dirtymask & regmask ) {
        uint8_t* storage = m_rawbank.get( s_words, _idx );
        uint32_t uword = 
          (uint32_t( storage[0] ) << 24) |
          (uint32_t( storage[1] ) << 16) |
          (uint32_t( storage[2] ) <<  8) |
          (uint32_t( storage[3] ) <<  0);
        uint32_t* dst = (uint32_t*)(&m_regs[_idx]);
        *dst = uword;
        m_dirtymask &= ~regmask;
      }
      return m_regs[_idx];
    }
  };

  struct FP64Bank : public FPBank {
    static uint32_t const   s_words = 2;
    double                  m_regs[32/s_words];
    FPRawBank&              m_rawbank;
    
    FP64Bank( FPRawBank& _rawbank ) : m_rawbank( _rawbank ) { _rawbank.add( this ); }
    
    uint32_t                words() { return s_words; }
    
    void                    writeback( uint32_t _idx, uint8_t* _storage ) {
      uint64_t* src = (uint64_t*)(&m_regs[_idx]);
      uint64_t uword = *src;
      _storage[0] = uword >> 56;
      _storage[1] = uword >> 48;
      _storage[2] = uword >> 40;
      _storage[3] = uword >> 32;
      _storage[4] = uword >> 24;
      _storage[5] = uword >> 16;
      _storage[6] = uword >>  8;
      _storage[7] = uword >>  0;
    }
    
    double&                 operator[]( uint32_t _idx ) {
      uint32_t regmask = ((1 << s_words)-1) << _idx;
      _idx /= s_words;
      if( m_dirtymask & regmask ) {
        uint8_t* storage = m_rawbank.get( s_words, _idx );
        uint64_t uword = 
          (uint64_t( storage[0] ) << 56) |
          (uint64_t( storage[1] ) << 48) |
          (uint64_t( storage[2] ) << 40) |
          (uint64_t( storage[3] ) << 32) |
          (uint64_t( storage[4] ) << 24) |
          (uint64_t( storage[4] ) << 16) |
          (uint64_t( storage[6] ) <<  8) |
          (uint64_t( storage[7] ) <<  0);
        uint64_t* dst = (uint64_t*)(&m_regs[_idx]);
        *dst = uword;
        m_dirtymask &= ~regmask;
      }
      return m_regs[_idx];
    }
  };

  struct QuadFPReg {
    double                         m_value;
    
    QuadFPReg() {}
    
    void                           source( uint8_t const* _storage ) {};
    void                           sink( uint8_t* _storage ) {};
  };
  
  struct FP128Bank : public FPBank {
    static uint32_t const   s_words = 4;
    QuadFPReg               m_regs[32/s_words];
    FPRawBank&              m_rawbank;
    
    FP128Bank( FPRawBank& _rawbank ) : m_rawbank( _rawbank ) { _rawbank.add( this ); }
    
    uint32_t                words() { return s_words; }
    
    void                    writeback( uint32_t _idx, uint8_t* _storage ) {
      m_regs[_idx].sink( _storage );
    }
    
    QuadFPReg&              operator[]( uint32_t _idx ) {
      uint32_t regmask = ((1 << s_words)-1) << _idx;
      _idx /= s_words;
      if( m_dirtymask & regmask ) {
        uint8_t* storage = m_rawbank.get( s_words, _idx );
        m_regs[_idx].source( storage );
        m_dirtymask &= ~regmask;
      }
      return m_regs[_idx];
    }
  };
  
  struct FPIntBank : public FPBank {
    static uint32_t const   s_words = 1;
    uint32_t                m_regs[32/s_words];
    FPRawBank&              m_rawbank;
    
    FPIntBank( FPRawBank& _rawbank ) : m_rawbank( _rawbank ) { _rawbank.add( this ); }
    
    uint32_t                words() { return s_words; }
    
    void                    writeback( uint32_t _idx, uint8_t* _storage ) {
      uint32_t uword = m_regs[_idx];
      _storage[0] = uword >> 24;
      _storage[1] = uword >> 16;
      _storage[2] = uword >>  8;
      _storage[3] = uword >>  0;
    }
    
    uint32_t&                operator[]( uint32_t _idx ) {
      uint32_t regmask = ((1 << s_words)-1) << _idx;
      _idx /= s_words;
      if( ((m_dirtymask >> (_idx*s_words)) % (1 << s_words)) ) {
        uint8_t* storage = m_rawbank.get( s_words, _idx );
        m_regs[_idx] =
          (uint32_t( storage[0] ) << 24) |
          (uint32_t( storage[1] ) << 16) |
          (uint32_t( storage[2] ) <<  8) |
          (uint32_t( storage[3] ) <<  0);
        m_dirtymask &= ~regmask;
      }
      return m_regs[_idx];
    }
  };
};

#endif // STAR_FPU_HPP
