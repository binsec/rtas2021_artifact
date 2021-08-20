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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_ARCH_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_ARCH_HH__

#include <unisim/component/cxx/memory/sparse/memory.hh>
#include <unisim/component/cxx/processor/intel/segments.hh>
#include <unisim/component/cxx/processor/intel/modrm.hh>
#include <unisim/component/cxx/processor/intel/vectorbank.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/inlining/inlining.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/register.hh>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <iosfwd>
#include <cmath>
#include <cassert>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace intel {
  
  template <class ARCH> struct Operation;
  
  // // TODO: should handle twice of 64-bit types
  // template <typename T>
  // struct Twice
  // {
  //   struct Error {};
  //   Twice() {}
  //   Twice( T const& val ) {}
  //   template <typename X>    Twice operator <<  (X) { throw Error(); return Twice(); }
  //   template <typename X>     void operator <<= (X) { throw Error(); }
  //   template <typename X>    Twice operator >>  (X) { throw Error(); return Twice(); }
  //   template <typename X>     void operator >>= (X) { throw Error(); }
  //   Twice operator | (Twice const& val) { throw Error(); return Twice(); }
    
  //   Twice& operator |= (Twice const& val) { return *this; }
  //   operator T () { throw Error(); return T(); }
  //   void unimplented();
  // };

  struct Arch
  {
    typedef uint8_t      u8_t;
    typedef uint16_t     u16_t;
    typedef uint32_t     u32_t;
    typedef uint64_t     u64_t;
    //    typedef Twice<u64_t> u128_t;
    typedef int8_t       s8_t;
    typedef int16_t      s16_t;
    typedef int32_t      s32_t;
    typedef int64_t      s64_t;
    //    typedef Twice<s64_t> s128_t;
    typedef bool         bit_t;
    typedef uint32_t     addr_t;
    
    typedef float        f32_t;
    typedef double       f64_t;
    typedef long double  f80_t;
    
    typedef GOq   GR;
    typedef u64_t gr_type;
  
    struct OpHeader
    {
      OpHeader( uint32_t _address ) : address( _address ) {} uint32_t address;
    };
    
    // CONSTRUCTORS/DESTRUCTORS
    Arch();
  
    // PROCESSOR META STATE
    bool                        m_running;      ///< Processors on/off
    int64_t                     m_instcount;    ///< Instruction count from the start
    bool                        m_disasm;       ///< Instruction disassembly on/off
    Operation<Arch>*            m_latest_insn;  ///< latest (current) instruction
    
    void                        stop() { m_running = false; }

    Operation<Arch>*            fetch();
    
    virtual void ExecuteSystemCall( unsigned id ) = 0;

    void                        syscall()
    {
      this->ExecuteSystemCall( m_regs[0] );
    }
    void                        interrupt( uint8_t op, uint8_t code )
    {
      switch (code)
        {
        case 0x80: {
          this->ExecuteSystemCall( m_regs[0] );
        } break;
      
      default:
        std::cerr << "Unhandled interruption (0x" << std::hex << uint32_t( op ) << ", 0x" << uint32_t( code ) << ").\n";
        exit( 0 );
      }
    }
    
    void  noexec( Operation<Arch> const& op );
    
    // PROCESSOR STATE
  protected:
    uint32_t                    m_EIP;
    
  public:
    enum ipproc_t { ipjmp = 0, ipcall, ipret };
    addr_t                      getnip() { return m_EIP; }
    void                        setnip( addr_t _eip, ipproc_t ipproc = ipjmp ) { m_EIP = _eip; }

    // INTEGER STATE
    // EFLAGS
    // bool m_ID, m_VIP, m_VIF, m_AC, m_VM, m_RF, m_NT, m_IOPL, m_IF, m_TF;

    struct FLAG
    {
      enum Code {
        CF = 0, PF,     AF,     ZF,     SF,     DF,     OF,
        C0,     C1,     C2,     C3,
        end
      };
    };
    
  protected:
    bool                        m_flags[FLAG::end];
    
  public:
    bit_t                       flagread( FLAG::Code flag )
    { return m_flags[flag]; }
    void                        flagwrite( FLAG::Code flag, bit_t fval )
    { m_flags[flag] = fval; }
    
    // Registers
  protected:
    uint32_t                    m_regs[8]; ///< extended reg
    
  public:
    template <class GOP>
    typename TypeFor<Arch,GOP::SIZE>::u regread( GOP const&, unsigned idx )
    {
      return typename TypeFor<Arch,GOP::SIZE>::u( m_regs[idx] );
    }
    
    u8_t regread( GObLH const&, unsigned idx )
    {
      unsigned reg = idx%4, sh = idx*2 & 8;
      return u8_t( m_regs[reg] >> sh );
    }
    
    template <class GOP>
    void regwrite( GOP const&, unsigned idx, typename TypeFor<Arch,GOP::SIZE>::u value )
    {
      m_regs[idx] = u32_t( value );
    }

    void regwrite( GObLH const&, unsigned idx, u8_t value )
    {
      uint32_t reg = idx%4, sh = idx*2 & 8;
      m_regs[reg] = (m_regs[reg] & ~u32_t(0xff << sh)) | ((value & u32_t(0xff)) << sh);
    }
    
    void regwrite( GOw const&, unsigned idx, u16_t value )
    {
      m_regs[idx] = (m_regs[idx] & ~u32_t(0xffff)) | ((value & u32_t(0xffff)));
    }
    
    // low level register access routines
    uint32_t                    lla_regread32( uint32_t idx ) const { return m_regs[idx]; }
    void                        lla_regwrite32( uint32_t idx, uint32_t value ) { m_regs[idx] = value; }
    
    // MEMORY STATE
    // Segment Registers
    intel::SegmentReg            m_srs[8];
    
    uint32_t m_gdt_bases[4]; // fake GDT registers used under Linux OS emulation
    
    void                        segregwrite( uint32_t idx, uint16_t _value )
    {
      uint32_t id = (_value >> 3) & 0x1fff; // entry number
      uint32_t ti = (_value >> 2) & 0x0001; // global or local
      uint32_t pl = (_value >> 0) & 0x0003; // requested privilege level
      
      //if (not GetLinuxOS()) throw 0;
      if ((idx >= 6) or (id == 0) or (id >= 4) or (ti != 0) or (pl != 3)) throw 0;
      
      m_srs[idx].update( id, ti, pl, m_gdt_bases[id] );
    }
    
    uint16_t                    segregread( unsigned num ) { throw 0; }
    
    // Memory content
  protected:
    struct ClearMemSet {
      void operator() ( uint8_t* base, uintptr_t size ) const {
        for (uintptr_t idx = 0; idx < size; ++idx) base[idx] = 0;
      }
    };
    typedef typename component::cxx::memory::sparse::Memory<uint32_t,12,12,ClearMemSet> Memory;
    Memory                      m_mem;
    // static uint32_t const       s_bits = 12;
    // Page*                       m_pages[1<<s_bits];
    
  public:
    // Memory access functions
    uint32_t
    mem_upperbound() const
    {
      uint32_t upperbound = 0;
      for (uint32_t idx = 0; idx < (1<<Memory::s_bits); ++idx) {
        for (typename Memory::Page* page = m_mem.m_pages[idx]; page; page = page->m_next) {
          if (page->m_address <= upperbound) continue;
          upperbound = page->m_address;
        }
      }
      uint32_t const pagesize = (1 << Memory::Page::s_bits);
      return (upperbound + pagesize) & -pagesize;
    }
    
  public:
    f32_t                       fmemread32( unsigned int _seg, addr_t _addr )
    {
      union IEEE754_t { float as_f; uint32_t as_u; } word;
      word.as_u = memread<32>( _seg, _addr );
      return word.as_f;
    }
    f64_t                       fmemread64( unsigned int _seg, addr_t _addr )
    {
      union IEEE754_t { double as_f; uint64_t as_u; } word;
      word.as_u = memread<64>( _seg, _addr );
      return word.as_f;
    }
    f80_t
    fmemread80( unsigned int _seg, addr_t _addr )
    {
      _addr += addr_t( m_srs[_seg].m_base );
      uintptr_t const buf_size = 10;
      uint8_t buf[buf_size];
      {
        uint32_t last_addr = _addr, addr = _addr;
        typename Memory::Page* page = m_mem.getpage( addr );
      
        for (uintptr_t idx = 0; idx < buf_size; ++idx, ++ addr)
          {
            if ((last_addr ^ addr) >> Memory::Page::s_bits)
              page = m_mem.getpage( addr );
            uint32_t offset = addr % (1 << Memory::Page::s_bits);
            buf[idx] = page->m_storage[offset];
          }
      }
      uint8_t sign = (buf[9] >> 7) & 1;
      int32_t exponent = ((uint16_t(buf[9]) << 8) & 0x7f00) | (uint16_t(buf[8]) & 0x00ff);
      uint64_t mantissa = 0;
      for (uintptr_t idx = 0; idx < 8; ++idx) { mantissa |= uint64_t( buf[idx] ) << (idx*8); }
      union IEEE754_t { double as_f; uint64_t as_u; } word;
    
      if (exponent != 0x7fff) {
        if (exponent == 0) {
          if (mantissa == 0) return sign ? -0.0 : 0.0;
          exponent += 1;
        }
        exponent -= 16383;
        /* normalizing */
        int32_t eo = __builtin_clzll( mantissa );
        mantissa <<= eo;
        exponent -= eo;
        if (exponent < 1024) {
          if (exponent < -1022) {
            /* denormalizing */
            eo = (-1022 - exponent);
            mantissa >>= eo;
            exponent = -1023;
          }
          word.as_u = ((mantissa << 1) >> 12);
          word.as_u |= uint64_t(exponent + 1023) << 52;
          word.as_u |= uint64_t(sign) << 63;
        } else /* exponent >= 1024 */ {
          /* huge number, convert to infinity */
          word.as_u = (uint64_t(0x7ff) << 52) | (uint64_t(sign) << 63);
        }
      }
    
      else /* (exponent == 0x7fff) */ {
        if (mantissa >> 63) {
          /* IEEE 754 compatible */
          word.as_u = ((mantissa << 1) >> 12);
          word.as_u |= uint64_t(0x7ff) << 52;
          word.as_u |= uint64_t(sign) << 63;
        } else {
          /* invalid operand ==> convert to quiet NaN (keep sign) */
          word.as_u = (uint64_t(0xfff) << 51) | (uint64_t(sign) << 63);
        }
      }
    
      return word.as_f;
    }

    template <unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::f
    fpmemread( unsigned seg, u64_t addr )
    {
      if (OPSIZE==32) return this->fmemread32( seg, addr );
      if (OPSIZE==64) return this->fmemread64( seg, addr );
      if (OPSIZE==80) return this->fmemread80( seg, addr );
      throw 0;        return typename TypeFor<Arch,OPSIZE>::f();
    }
    
    typedef unisim::component::cxx::processor::intel::RMOp<Arch> RMOp;
    
    template <unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::f
    frmread( RMOp const& rmop )
    {
      typedef typename TypeFor<Arch,OPSIZE>::f f_type;
      if (not rmop.is_memory_operand()) return f_type( fread( rmop.ereg() ) );
      return this->fpmemread<OPSIZE>( rmop->segment, rmop->effective_address( *this ) );
    }

    void                        fmemwrite32( unsigned int _seg, addr_t _addr, f32_t _val )
    {
      union IEEE754_t { float as_f; uint32_t as_u; } word;
      word.as_f = _val;
      memwrite<32>( _seg, _addr, word.as_u );
    }
    void                        fmemwrite64( unsigned int _seg, addr_t _addr, f64_t _val )
    {
      union IEEE754_t { double as_f; uint64_t as_u; } word;
      word.as_f = _val;
      memwrite<64>( _seg, _addr, word.as_u );
    }
    void                        fmemwrite80( unsigned int _seg, addr_t _addr, f80_t _val )
    {
      _addr += addr_t( m_srs[_seg].m_base );
      union IEEE754_t { double as_f; uint64_t as_u; } word;
      word.as_f = _val;
      uint8_t sign = (word.as_u >> 63) & 1;
      int32_t exponent = (word.as_u >> 52) & 0x7ff;
      uint64_t mantissa = (word.as_u << 12) >> 1;
      if (exponent != 0x7ff) {
        if (exponent != 0) {
          mantissa |= uint64_t( 1 ) << 63;
        } else if (mantissa != 0) {
          /* normalizing */
          int32_t eo = __builtin_clzll( mantissa );
          exponent = 1 - eo;
          mantissa <<= eo;
        } else {
          exponent = 1023 - 16383;
        }
        exponent = exponent - 1023 + 16383;
        assert( (exponent >= 0) and (exponent < 0x8000) );
      } else {
        mantissa |= uint64_t( 1 ) << 63;
        exponent = 0x7fff;
      }

      uintptr_t const buf_size = 10;
      uint8_t buf[buf_size];
    
      buf[9] = (sign << 7) | (uint8_t( exponent >> 8 ) & 0x7f);
      buf[8] = uint8_t( exponent ) & 0xff;
      for ( uintptr_t idx = 0; idx < 8; ++idx)
        buf[idx] = uint8_t( mantissa >> (idx*8) );
    
      uint32_t last_addr = _addr, addr = _addr;
      typename Memory::Page* page = m_mem.getpage( addr );
      
      for (uintptr_t idx = 0; idx < buf_size; ++idx, ++addr)
        {
          uint8_t byte = buf[idx];
          if ((last_addr ^ addr) >> Memory::Page::s_bits)
            page = m_mem.getpage( addr );
          uint32_t offset = addr % (1 << Memory::Page::s_bits);
          page->m_storage[offset] = byte;
        }
    }

    template <unsigned OPSIZE>
    void
    fpmemwrite( unsigned seg, u64_t addr, typename TypeFor<Arch,OPSIZE>::f value )
    {
      if (OPSIZE==32) return fmemwrite32( seg, addr, value );
      if (OPSIZE==64) return fmemwrite64( seg, addr, value );
      if (OPSIZE==80) return fmemwrite80( seg, addr, value );
      throw 0;
    }
    
    template <unsigned OPSIZE>
    void
    frmwrite( RMOp const& rmop, typename TypeFor<Arch,OPSIZE>::f value )
    {
      if (not rmop.is_memory_operand()) return fwrite( rmop.ereg(), f64_t( value ) );
      fpmemwrite<OPSIZE>( rmop->segment, rmop->effective_address( *this ), value );
    }

    template <unsigned OPSIZE>
    void
    memwrite( unsigned _seg, addr_t _addr, typename TypeFor<Arch,OPSIZE>::u _val )
    {
      uintptr_t const int_size = (OPSIZE/8);
      uint32_t addr = _addr + m_srs[_seg].m_base, last_addr = addr;
      
      typename Memory::Page* page = m_mem.getpage( addr );
      
      for (uintptr_t idx = 0; idx < int_size; ++idx, ++addr)
        {
          uint8_t byte = (_val >> (idx*8)) & 0xff;
          if ((last_addr ^ addr) >> Memory::Page::s_bits)
            page = m_mem.getpage( addr );
          uint32_t offset = addr % (1 << Memory::Page::s_bits);
          page->m_storage[offset] = byte;
        }
    }
    
    template <unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::u
    pop()
    {
      // TODO: handle stack address size
      gr_type sptr = regread( GR(), 4 );
      regwrite( GR(), 4, sptr + gr_type( OPSIZE/8 ) );
      return memread<OPSIZE>( SS, addr_t( sptr ) );
    }
    
    template <unsigned OPSIZE>
    void
    push( typename TypeFor<Arch,OPSIZE>::u value )
    {
      // TODO: handle stack address size
      gr_type sptr = regread( GR(), 4 ) - gr_type( OPSIZE/8 );
      memwrite<OPSIZE>( SS, addr_t( sptr ), value );
      regwrite( GR(), 4, sptr );
    }

    void shrink_stack( addr_t offset ) { regwrite( GR(), 4, regread( GR(), 4 ) + offset ); }
    void grow_stack( addr_t offset ) { regwrite( GR(), 4, regread( GR(), 4 ) - offset ); }
    
    template <class GOP>
    typename TypeFor<Arch,GOP::SIZE>::u
    rmread( GOP const& g, RMOp const& rmop )
    {
      if (not rmop.is_memory_operand())
        return regread( g, rmop.ereg() );
      
      return memread<GOP::SIZE>( rmop->segment, rmop->effective_address( *this ) );
    }
    
    template <class GOP>
    void
    rmwrite( GOP const& g, RMOp const& rmop, typename TypeFor<Arch,GOP::SIZE>::u value )
    {
      if (not rmop.is_memory_operand())
        return regwrite( g, rmop.ereg(), value );
      
      return memwrite<GOP::SIZE>( rmop->segment, rmop->effective_address( *this ), value );
    }
    
    // void
    // Arch::fnanchk( f64_t value )
    // {
    //   if (not m_fnanchk) return;
    //   switch (__fpclassify( value )) {
    //   case FP_NAN:      std::cerr << "[FPNAN]"; break;
    //   case FP_INFINITE: std::cerr << "[FPINF]"; break;
    //   default:          return;
    //   }
    //   this->fpdump();
    //   throw FPException();
    // }
  
    template<unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::u
    memread( unsigned _seg, addr_t _addr )
    {
      uintptr_t const int_size = (OPSIZE/8);
      uint32_t addr = _addr + m_srs[_seg].m_base, last_addr = addr;
      typename Memory::Page* page = m_mem.getpage( addr );
      
      typedef typename TypeFor<Arch,OPSIZE>::u u_type;
      u_type result = 0;
      
      for (uintptr_t idx = 0; idx < int_size; ++idx, ++ addr)
        {
          if ((last_addr ^ addr) >> Memory::Page::s_bits)
            page = m_mem.getpage( addr );
          uint32_t offset = addr % (1 << Memory::Page::s_bits);
          result |= (u_type( page->m_storage[offset] ) << (idx*8));
        }
      
      return result;
    }
    
    // Low level memory access routines
    enum LLAException_t { LLAError };
    void                        lla_memcpy( uint32_t addr, uint8_t const* buf, uint32_t size )
    { m_mem.write( addr, buf, size ); }
    void                        lla_memcpy( uint8_t* buf, uint32_t addr, uint32_t size )
    { m_mem.read( buf, addr, size ); }
    void                        lla_bzero( uint32_t addr, uint32_t size )
    { m_mem.clear( addr, size ); }
    template<class INT_t>
    void                        lla_memwrite( uint32_t _addr, INT_t _val )
    {
      uintptr_t const int_size = sizeof( INT_t );
      
      uint32_t last_addr = _addr, addr = _addr;
      typename Memory::Page* page = m_mem.getpage( addr );
      
      for (uintptr_t idx = 0; idx < int_size; ++idx, ++addr)
        {
          uint8_t byte = (_val >> (idx*8)) & 0xff;
          if ((last_addr ^ addr) >> Memory::Page::s_bits)
            page = m_mem.getpage( addr );
          uint32_t offset = addr % (1 << Memory::Page::s_bits);
          page->m_storage[offset] = byte;
        }
    }
    
    template<class INT_t>
    INT_t                       lla_memread( uint32_t _addr )
    {
      uintptr_t const int_size = sizeof( INT_t );
      
      uint32_t last_addr = _addr, addr = _addr;
      typename Memory::Page* page = m_mem.getpage( addr );
      
      INT_t result = 0;
      
      for (uintptr_t idx = 0; idx < int_size; ++idx, ++ addr)
        {
          if ((last_addr ^ addr) >> Memory::Page::s_bits)
            page = m_mem.getpage( addr );
          uint32_t offset = addr % (1 << Memory::Page::s_bits);
          result |= (INT_t( page->m_storage[offset] ) << (idx*8));
        }
      
      return result;

    }
    
    uint8_t                     lla_memread8( uint32_t _addr ) { return lla_memread<uint8_t>( _addr ); }
    uint16_t                    lla_memread16( uint32_t _addr ) { return lla_memread<uint16_t>( _addr ); }
    uint32_t                    lla_memread32( uint32_t _addr ) { return lla_memread<uint32_t>( _addr ); }
    void                        lla_memwrite8( uint32_t _addr, uint8_t _val ) { lla_memwrite<uint8_t>( _addr, _val ); }
    void                        lla_memwrite16( uint32_t _addr, uint16_t _val ) { lla_memwrite<uint16_t>( _addr, _val ); }
    void                        lla_memwrite32( uint32_t _addr, uint32_t _val ) { lla_memwrite<uint32_t>( _addr, _val ); }

    // FLOATING POINT STATE
  protected:
    double                      m_fregs[8];
    uint32_t                    m_ftop;
    uint16_t                    m_fcw;
    
  public:
    void                        fnanchk( f64_t value ) {};
    uint32_t                    ftopread() { return m_ftop; }
    int                         fcwreadRC() const { return int( (m_fcw >> 10) & 3 ); }
    void                        fpush( f64_t value )
    { fnanchk( value ); m_ftop = ((m_ftop + 0x7) & 0x7); m_fregs[m_ftop] = value; /*m_dirtfregs |= (1 << m_ftop);*/ }
    void                        fwrite( uint32_t idx, f64_t value )
    { fnanchk( value ); uint32_t fidx = (m_ftop + idx) & 0x7; m_fregs[fidx] = value; /*m_dirtfregs |= (1 << fidx);*/ }
    f64_t                       fpop()
    { f64_t value = m_fregs[m_ftop++]; m_ftop &= 0x7; fnanchk( value ); return value; }
    f64_t                       fread( uint32_t idx )
    { uint32_t fidx = (m_ftop + idx) & 0x7; f64_t value = m_fregs[fidx]; fnanchk( value ); return value; }
    void                        finit()
    {
      m_ftop = 0;
      m_fcw = 0x37f;
      flagwrite( FLAG::C0, 0 );
      flagwrite( FLAG::C1, 0 );
      flagwrite( FLAG::C2, 0 );
      flagwrite( FLAG::C3, 0 );
    }
    
    void
    fxam()
    {
      double val = this->fread( 0 );
      
      flagwrite( FLAG::C1, __signbit( val ) );
      
      switch (__fpclassify( val )) {
      case FP_NAN:
        flagwrite( FLAG::C3, 0 );
        flagwrite( FLAG::C2, 0 );
        flagwrite( FLAG::C0, 1 );
        break;
      case FP_NORMAL:
        flagwrite( FLAG::C3, 0 );
        flagwrite( FLAG::C2, 1 );
        flagwrite( FLAG::C0, 0 );
        break;
      case FP_INFINITE:
        flagwrite( FLAG::C3, 0 );
        flagwrite( FLAG::C2, 1 );
        flagwrite( FLAG::C0, 1 );
        break;
      case FP_ZERO:
        flagwrite( FLAG::C3, 1 );
        flagwrite( FLAG::C2, 0 );
        flagwrite( FLAG::C0, 0 );
        break;
      case FP_SUBNORMAL:
        flagwrite( FLAG::C3, 1 );
        flagwrite( FLAG::C2, 1 );
        flagwrite( FLAG::C0, 0 );
        break;
      }
    }
    
    u16_t
    fcwread()
    {
      return u16_t( m_fcw );
      // return u16_t( (1 << 0/*IM*/) |
      //               (1 << 1/*DM*/) |
      //               (1 << 2/*ZM*/) |
      //               (1 << 3/*OM*/) |
      //               (1 << 4/*UM*/) |
      //               (1 << 5/*PM*/) |
      //               (2 << 8/*PC*/) |
      //               (0 << 10/*RC*/) |
      //               (0 << 12/*X*/) );
    }
    
    void
    fcwwrite( u16_t _value )
    {
      m_fcw = _value;
      struct field { uint16_t offset, mask, expected, err; char const* name; };
      static field fields[] =
        {
          { 0, 1, 1, 0, "IM"},
          { 1, 1, 1, 0, "DM"},
          { 2, 1, 1, 0, "ZM"},
          { 3, 1, 1, 0, "OM"},
          { 4, 1, 1, 0, "UM"},
          { 5, 1, 1, 0, "PM"},
          { 8, 3, 2, 0, "PC"},
          {10, 3, 0, 0, "RC"},
          {12, 1, 0, 0, "X"},
        };
      for (uintptr_t idx = 0; idx < (sizeof (fields) / sizeof (field)); ++idx)
        {
          uint16_t field_val = ((_value >> fields[idx].offset) & fields[idx].mask);
          if (field_val == fields[idx].expected)
            continue; /* value is expected one*/
          if ((fields[idx].err >> field_val) & 1)
            continue; /* error already reported */
          fields[idx].err |= (1 << field_val);
          std::cerr << "Warning: unimplemented FPUControlWord." << fields[idx].name
                    << " value: " << field_val << ".\n";
        }
    }
    
    u64_t tscread() { return m_instcount; }
    void  xgetbv();
    void  cpuid();
    void _DE() { std::cerr << "#DE: Division Error.\n"; throw std::runtime_error("diverr"); }
    
    // void                        fpdump()
    // {
    //   std::cerr << ": #insn: " << std::dec << m_instcount
    //             << ", @insn: " << std::hex << m_latest_insn->GetAddr()
    //             << ", insn: ";
    //   m_latest_insn->disasm( std::cerr );
    //   std::cerr << " (" << m_latest_insn->GetEncoding() << ")" << std::endl;
    // }
    
    void unimplemented() { throw std::runtime_error("unimplemented"); }
    
    bool Test( bool b ) const { return b; }
    
    // Debug methods
  protected:
    uint32_t                    m_dirtfregs;
    
  public:
    uint32_t                    pop_dirtfregs() { uint32_t r = m_dirtfregs; m_dirtfregs = 0; return r; }
    
  public:
    struct VUConfig
    {
      static unsigned const BYTECOUNT = 32;
      template <typename T> using TypeInfo = unisim::component::cxx::processor::intel::VectorTypeInfo<T>;
      typedef u8_t Byte;
    };

    unisim::component::cxx::processor::intel::VUnion<VUConfig> umms[8];
    uint8_t vmm_storage[8][VUConfig::BYTECOUNT];
    uint32_t mxcsr;

    void mxcswrite( uint32_t v ) { mxcsr = v; }
    uint32_t mxcsread() { return mxcsr; }
    
    template <class VR> static unsigned vmm_wsize( VR const& vr ) { return VR::size() / 8; }
    static unsigned vmm_wsize( unisim::component::cxx::processor::intel::SSE const& ) { return VUConfig::BYTECOUNT; }
  
    template <class VR, class ELEM>
    ELEM
    vmm_read( VR const& vr, unsigned reg, unsigned sub, ELEM const& e )
    {
      ELEM const* elems = umms[reg].GetConstStorage( &vmm_storage[reg][0], e, vr.size() / 8 );
      return elems[sub];
    }
  
    template <class VR, class ELEM>
    void
    vmm_write( VR const& vr, unsigned reg, unsigned sub, ELEM const& e )
    {
      ELEM* elems = umms[reg].GetStorage( &vmm_storage[reg][0], e, vmm_wsize( vr ) );
      elems[sub] = e;
      //gdbchecker.xmark(reg);
    }

    // Integer case
    template <class TYPE> TYPE vmm_memread( unsigned seg, u64_t addr, TYPE const& e )
    {
      typedef unisim::component::cxx::processor::intel::atpinfo<Arch,TYPE> atpinfo;
      return TYPE(memread<atpinfo::bitsize>(seg,addr));
    }
  
    f32_t vmm_memread( unsigned seg, u64_t addr, f32_t const& e ) { return fmemread32( seg, addr ); }
    f64_t vmm_memread( unsigned seg, u64_t addr, f64_t const& e ) { return fmemread64( seg, addr ); }
    f80_t vmm_memread( unsigned seg, u64_t addr, f80_t const& e ) { return fmemread80( seg, addr ); }
  
    // Integer case
    template <class TYPE> void vmm_memwrite( unsigned seg, u64_t addr, TYPE const& e )
    {
      typedef unisim::component::cxx::processor::intel::atpinfo<Arch,TYPE> atpinfo;
      memwrite<atpinfo::bitsize>(seg,addr,typename atpinfo::utype(e));
    }
  
    void vmm_memwrite( unsigned seg, u64_t addr, f32_t const& e ) { return fmemwrite32( seg, addr, e ); }
    void vmm_memwrite( unsigned seg, u64_t addr, f64_t const& e ) { return fmemwrite64( seg, addr, e ); }
    void vmm_memwrite( unsigned seg, u64_t addr, f80_t const& e ) { return fmemwrite80( seg, addr, e ); }

    // void vmm_memwrite( unsigned seg, u64_t addr, u8_t  const& e ) { return memwrite< 8>( seg, addr, e ); }
    // void vmm_memwrite( unsigned seg, u64_t addr, u16_t const& e ) { return memwrite<16>( seg, addr, e ); }
    // void vmm_memwrite( unsigned seg, u64_t addr, u32_t const& e ) { return memwrite<32>( seg, addr, e ); }
    // void vmm_memwrite( unsigned seg, u64_t addr, u64_t const& e ) { return memwrite<64>( seg, addr, e ); }
  
    template <class VR, class ELEM>
    ELEM
    vmm_read( VR const& vr, RMOp const& rmop, unsigned sub, ELEM const& e )
    {
      if (not rmop.is_memory_operand()) return vmm_read( vr, rmop.ereg(), sub, e );
      return vmm_memread( rmop->segment, rmop->effective_address( *this ) + sub*VectorTypeInfo<ELEM>::bytecount, e );
    }
    
    template <class VR, class ELEM>
    void
    vmm_write( VR const& vr, RMOp const& rmop, unsigned sub, ELEM const& e )
    {
      if (not rmop.is_memory_operand()) return vmm_write( vr, rmop.ereg(), sub, e );
      return vmm_memwrite( rmop->segment, rmop->effective_address( *this ) + sub*VectorTypeInfo<ELEM>::bytecount, e );
    }

  };
  

  struct FPException : public std::exception {};

  Arch::f64_t sine( Arch::f64_t );
  Arch::f64_t cosine( Arch::f64_t );
  Arch::f64_t tangent( Arch::f64_t );
  Arch::f64_t arctan( Arch::f64_t angle );
  Arch::f64_t fmodulo( Arch::f64_t, Arch::f64_t );
  Arch::f64_t firound( Arch::f64_t, int x87frnd_mode );
  Arch::f64_t power( Arch::f64_t, Arch::f64_t );
  Arch::f64_t logarithm( Arch::f64_t );
  Arch::f64_t square_root( Arch::f64_t );

} // end of namespace intel
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_ARCH_HH__
