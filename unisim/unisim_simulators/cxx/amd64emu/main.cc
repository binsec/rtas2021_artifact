/*
 *  Copyright (c) 2018,
 *  Commissariat a l'Energie Atomique (CEA)
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

// #include <unisim/component/cxx/processor/intel/arch.hh>
#include <unisim/component/cxx/processor/intel/isa/intel.hh>
#include <unisim/component/cxx/processor/intel/modrm.hh>
#include <unisim/component/cxx/processor/intel/disasm.hh>
#include <unisim/component/cxx/processor/intel/vectorbank.hh>
#include <unisim/component/cxx/processor/intel/types.hh>
#include <unisim/component/cxx/memory/sparse/memory.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/debug/simple_register.hh>
#include <linuxsystem.hh>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <iomanip>
#include <cmath>
#include <cctype>
#include <cassert>

//template <typename T> using VectorTypeInfo = unisim::component::cxx::processor::intel::VectorTypeInfo<T>;
template <typename A, unsigned S> using TypeFor = typename unisim::component::cxx::processor::intel::TypeFor<A,S>;

struct Arch
  : public unisim::service::interfaces::MemoryInjection<uint64_t>
  , public unisim::service::interfaces::Memory<uint64_t>
  , public unisim::service::interfaces::Registers
{
  typedef uint8_t      u8_t;
  typedef uint16_t     u16_t;
  typedef uint32_t     u32_t;
  typedef uint64_t     u64_t;
  typedef int8_t       s8_t;
  typedef int16_t      s16_t;
  typedef int32_t      s32_t;
  typedef int64_t      s64_t;
  typedef bool         bit_t;
  typedef uint64_t     addr_t;
  typedef float        f32_t;
  typedef double       f64_t;
  typedef long double  f80_t;

  typedef unisim::component::cxx::processor::intel::GObLH GObLH;
  typedef unisim::component::cxx::processor::intel::GOb GOb;
  typedef unisim::component::cxx::processor::intel::GOw GOw;
  typedef unisim::component::cxx::processor::intel::GOd GOd;
  typedef unisim::component::cxx::processor::intel::GOq GOq;

  typedef unisim::component::cxx::processor::intel::SSE SSE;
  typedef unisim::component::cxx::processor::intel::XMM XMM;
  typedef unisim::component::cxx::processor::intel::YMM YMM;

  typedef GOq   GR;
  typedef u64_t gr_type;
  
  struct OpHeader
  {
    OpHeader( addr_t _address ) : address( _address ) {} addr_t address;
  };

  struct EFlagsRegister  : public unisim::service::interfaces::Register
  {
    uint32_t eflagsread() const;
    EFlagsRegister(Arch& _cpu) : cpu(_cpu) {}
    Arch& cpu;
    char const* GetName() const override { return "%eflags"; }
    void GetValue(void* buffer) const override { *((uint32_t*)buffer) = eflagsread(); }
    void SetValue(void const* buffer) override
    {
      uint32_t bits = *((uint32_t*)buffer);
      struct { unsigned bit; FLAG::Code flag; }
      ffs[] = {{0, FLAG::CF}, {2, FLAG::PF}, {4, FLAG::AF}, {6, FLAG::ZF}, {7, FLAG::SF}, {10, FLAG::DF}, {11, FLAG::OF}};
      for (unsigned idx = 0, end = sizeof(ffs)/sizeof(ffs[0]); idx < end; ++idx)
        cpu.flagwrite( ffs[idx].flag, bit_t((bits >> ffs[idx].bit) & 1u) );
      uint32_t chk = eflagsread();
      if (bits != chk)
        std::cerr << "Warning: imported eflags " << std::hex << bits << " ended with " << chk << std::endl;
    }
    int GetSize() const override { return 32; }
  };
  
  Arch()
    : unisim::service::interfaces::MemoryInjection<uint64_t>()
    , unisim::service::interfaces::Memory<uint64_t>()
    , unisim::service::interfaces::Registers()
    , linux_os(0)
    , m_mem()
    , segregs(), fs_base(), gs_base()
    , rip(), u64regs(), m_flags()
    , m_fregs(), m_ftop(0), m_fcw(0x37f)
    , umms(), vmm_storage(), mxcsr(0x1fa0)
    , latest_instruction(0), do_disasm(false)
    , instruction_count(0)
      //    , gdbchecker()
  {
    regmap["%rip"] = new unisim::util::debug::SimpleRegister<uint64_t>("%rip", &this->rip);
    regmap["%fs_base"] = new unisim::util::debug::SimpleRegister<uint64_t>("%fs_base", &this->fs_base);
    regmap["%gs_base"] = new unisim::util::debug::SimpleRegister<uint64_t>("%gs_base", &this->fs_base);
    regmap["%fcw"] = new unisim::util::debug::SimpleRegister<uint16_t>("%fcw", &this->m_fcw);

    {
      EFlagsRegister* reg = new EFlagsRegister(*this);
      regmap[reg->GetName()] = reg;
    }
    
    struct XmmRegister : public unisim::service::interfaces::Register
    {
      XmmRegister(std::string const& _name, Arch& _cpu, unsigned _reg) : name(_name), cpu(_cpu), reg(_reg) {}
      std::string name;
      Arch& cpu;
      unsigned reg;
      char const* GetName() const override { return name.c_str(); }
      void GetValue(void *buffer) const override
      {
        uint8_t* buf = reinterpret_cast<uint8_t*>(buffer);
        std::fill(&buf[0],&buf[16],0);
        auto const& ureg = cpu.umms[reg];
        ureg.transfer( &buf[0], &cpu.vmm_storage[reg][0], std::min(ureg.size,16u), false );
      }
      void SetValue(const void *buffer) override
      {
        uint8_t* elems = cpu.umms[reg].GetStorage( &cpu.vmm_storage[reg][0], uint8_t(), VUConfig::BYTECOUNT );
        memcpy(elems,buffer,16);
      }
      int GetSize() const override { return 16; }
    };
    
    for (int idx = 0; idx < 16; ++idx)
      {
        std::ostringstream regname;
        using unisim::component::cxx::processor::intel::DisasmV;
        typedef unisim::component::cxx::processor::intel::XMM XMM;
        regname << DisasmV( XMM(), idx );
        regmap[regname.str()] = new XmmRegister(regname.str(), *this, idx);
      }
    
    for (int idx = 0; idx < 16; ++idx)
      {
        std::ostringstream regname;
        regname << unisim::component::cxx::processor::intel::DisasmG(GOq(), idx);
        regmap[regname.str()] = new unisim::util::debug::SimpleRegister<uint64_t>(regname.str(), &u64regs[idx]);
      }
    
    //   struct SegmentRegister : public unisim::service::interfaces::Register
    //   {
    //     SegmentRegister( Arch& _arch, std::string _name, unsigned _idx ) : arch(_arch), name(_name), idx(_idx) {}
    //     virtual const char *GetName() const { return "pc"; }
    //     virtual void GetValue( void* buffer ) const { *((uint16_t*)buffer) = arch.segregread( idx ); }
    //     virtual void SetValue( void const* buffer ) { arch.segregwrite( idx, *((uint16_t*)buffer) ); }
    //     virtual int  GetSize() const { return 2; }
    //     virtual void Clear() {} // Clear is meaningless for segment registers
    //     Arch&        arch;
    //     std::string  name;
    //     unsigned     idx;
    //   };

    //   for (int idx = 0; idx < 6; ++idx) {
    //     std::ostringstream regname;
    //     regname << unisim::component::cxx::processor::intel::DisasmS(idx);
    //     regmap[regname.str()] = new SegmentRegister(*this, regname.str(), idx);
    //   }
    
    //   for (int idx = 0; idx < 4; ++idx) {
    //     std::ostringstream regname;
    //     regname << "@gdt[" << idx<< "].base";
    //     regmap[regname.str()] = new unisim::util::debug::SimpleRegister<uint64_t>(regname.str(), &m_gdt_bases[idx]);
    //   }
    
    //   struct X87Register : public unisim::service::interfaces::Register
    //   {
    //     X87Register( std::string _name, Arch& _arch, unsigned _idx ) : name(_name), arch(_arch), idx(_idx) {}
    //     char const* GetName() const { return name.c_str(); }
    //     void GetValue(void *buffer) const { *((f64_t*)buffer) = arch.fread( idx ); }
    //     void SetValue(const void *buffer) { arch.fwrite( idx, *((f64_t*)buffer) ); }
    //     int GetSize() const { return 8; }
    //     std::string name;
    //     Arch& arch;
    //     unsigned idx;
    //   };
    //   for (int idx = 0; idx < 8; ++idx) {
    //     std::ostringstream regname;
    //     regname << unisim::component::cxx::processor::intel::DisasmFPR(idx);
    //     regmap[regname.str()] = new X87Register( regname.str(), *this, idx );
    //   }
  }
  ~Arch()
  {
    for (auto reg : regmap)
      delete reg.second;
  }
  
  void SetLinuxOS( unisim::service::interfaces::LinuxOS* _linux_os ) { linux_os = _linux_os; }
  // unisim::service::interfaces::LinuxOS* GetLinuxOS() { return linux_os; }
  
  unisim::service::interfaces::LinuxOS* linux_os;
  std::map<std::string,unisim::service::interfaces::Register*> regmap;
  
  struct ClearMemSet { void operator() ( uint8_t* base, uintptr_t size ) const { __builtin_bzero(base, size); } };
  typedef typename unisim::component::cxx::memory::sparse::Memory<addr_t,15,15,ClearMemSet> Memory;
  Memory                      m_mem;

  // unisim::service::interfaces::Memory<addr_t>
  void ResetMemory() {}
  bool ReadMemory(addr_t addr, void* buffer, uint32_t size ) { m_mem.read( (uint8_t*)buffer, addr, size ); return true; }
  bool WriteMemory(addr_t addr, void const* buffer, uint32_t size) { m_mem.write( addr, (uint8_t*)buffer, size ); return true; }

  typedef unisim::component::cxx::processor::intel::RMOp<Arch> RMOp;
  
  // MEMORY STATE
  // Segment Registers
  uint16_t segregs[6];
  addr_t fs_base, gs_base;
  addr_t segbase(unsigned seg) const { switch(seg){ case 4: return fs_base; case 5: return gs_base; } return 0; }
  void                        segregwrite( unsigned idx, uint16_t value )
  {
    if (idx > 6) throw 0;
    segregs[idx] = value;
  }
  uint16_t                    segregread( unsigned idx )
  {
    throw 0;
    if (idx > 6) throw 0;
    return segregs[idx];
  }

  // Low level memory access routines
  enum LLAException_t { LLAError };
  void                        lla_memcpy( addr_t addr, uint8_t const* buf, addr_t size )
  { m_mem.write( addr, buf, size ); }
  void                        lla_memcpy( uint8_t* buf, addr_t addr, addr_t size )
  { m_mem.read( buf, addr, size ); }
  void                        lla_bzero( addr_t addr, addr_t size )
  { m_mem.clear( addr, size ); }
  template<class INT_t>
  void                        lla_memwrite( addr_t _addr, INT_t _val )
  {
    uintptr_t const int_size = sizeof( INT_t );
      
    addr_t last_addr = _addr, addr = _addr;
    typename Memory::Page* page = m_mem.getpage( addr );
      
    for (uintptr_t idx = 0; idx < int_size; ++idx, ++addr)
      {
        uint8_t byte = (_val >> (idx*8)) & 0xff;
        if ((last_addr ^ addr) >> Memory::Page::s_bits)
          page = m_mem.getpage( addr );
        addr_t offset = addr % (1 << Memory::Page::s_bits);
        page->m_storage[offset] = byte;
      }
  }
    
  template<class INT_t>
  INT_t                       lla_memread( addr_t _addr )
  {
    uintptr_t const int_size = sizeof( INT_t );
      
    addr_t last_addr = _addr, addr = _addr;
    typename Memory::Page* page = m_mem.getpage( addr );
      
    INT_t result = 0;
      
    for (uintptr_t idx = 0; idx < int_size; ++idx, ++ addr)
      {
        if ((last_addr ^ addr) >> Memory::Page::s_bits)
          page = m_mem.getpage( addr );
        addr_t offset = addr % (1 << Memory::Page::s_bits);
        result |= (INT_t( page->m_storage[offset] ) << (idx*8));
      }
      
    return result;

  }
  
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
    _addr += addr_t( segbase(_seg) );
    uintptr_t const buf_size = 10;
    uint8_t buf[buf_size];
    {
      addr_t last_addr = _addr, addr = _addr;
      typename Memory::Page* page = m_mem.getpage( addr );
      
      for (uintptr_t idx = 0; idx < buf_size; ++idx, ++ addr)
        {
          if ((last_addr ^ addr) >> Memory::Page::s_bits)
            page = m_mem.getpage( addr );
          addr_t offset = addr % (1 << Memory::Page::s_bits);
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
  fpmemread( unsigned seg, addr_t addr )
  {
    if (OPSIZE==32) return this->fmemread32( seg, addr );
    if (OPSIZE==64) return this->fmemread64( seg, addr );
    if (OPSIZE==80) return this->fmemread80( seg, addr );
    throw 0;        return typename TypeFor<Arch,OPSIZE>::f();
  }

  template <unsigned OPSIZE>
  typename TypeFor<Arch,OPSIZE>::f
  frmread( RMOp const& rmop )
  {
    typedef typename TypeFor<Arch,OPSIZE>::f f_type;
    if (not rmop.is_memory_operand()) return f_type( this->fread( rmop.ereg() ) );
    return this->fpmemread<OPSIZE>( rmop->segment, rmop->effective_address( *this ) );
  }

  void                        fmemwrite32( unsigned int _seg, u64_t _addr, f32_t _val )
  {
    union IEEE754_t { float as_f; uint32_t as_u; } word;
    word.as_f = _val;
    memwrite<32>( _seg, _addr, word.as_u );
  }
  void                        fmemwrite64( unsigned int _seg, u64_t _addr, f64_t _val )
  {
    union IEEE754_t { double as_f; uint64_t as_u; } word;
    word.as_f = _val;
    memwrite<64>( _seg, _addr, word.as_u );
  }
  void                        fmemwrite80( unsigned int _seg, u64_t _addr, f80_t _val )
  {
    _addr += u64_t( segbase(_seg) );
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

    addr_t last_addr = _addr, addr = _addr;
    typename Memory::Page* page = m_mem.getpage( addr );

    for (uintptr_t idx = 0; idx < buf_size; ++idx, ++addr)
      {
        uint8_t byte = buf[idx];
        if ((last_addr ^ addr) >> Memory::Page::s_bits)
          page = m_mem.getpage( addr );
        addr_t offset = addr % (1 << Memory::Page::s_bits);
        page->m_storage[offset] = byte;
      }
  }

  template <unsigned OPSIZE>
  void
  fpmemwrite( unsigned seg, addr_t addr, typename TypeFor<Arch,OPSIZE>::f value )
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
  memwrite( unsigned _seg, u64_t _addr, typename TypeFor<Arch,OPSIZE>::u _val )
  {
    uintptr_t const int_size = (OPSIZE/8);
    addr_t addr = _addr + segbase(_seg), last_addr = addr;

    typename Memory::Page* page = m_mem.getpage( addr );

    for (uintptr_t idx = 0; idx < int_size; ++idx, ++addr)
      {
        uint8_t byte = (_val >> (idx*8)) & 0xff;
        if ((last_addr ^ addr) >> Memory::Page::s_bits)
          page = m_mem.getpage( addr );
        addr_t offset = addr % (1 << Memory::Page::s_bits);
        page->m_storage[offset] = byte;
      }
  }

  template <unsigned OPSIZE>
  typename TypeFor<Arch,OPSIZE>::u
  pop()
  {
    // TODO: handle stack address size
    u64_t sptr = regread( GOq(), 4 );
    regwrite( GOq(), 4, sptr + u64_t( OPSIZE/8 ) );
    return memread<OPSIZE>( unisim::component::cxx::processor::intel::SS, sptr );
  }

  void shrink_stack( addr_t offset ) { regwrite( GR(), 4, regread( GR(), 4 ) + offset ); }
  void grow_stack( addr_t offset ) { regwrite( GR(), 4, regread( GR(), 4 ) - offset ); }

  template <unsigned OPSIZE>
  void
  push( typename TypeFor<Arch,OPSIZE>::u value )
  {
    // TODO: handle stack address size
    u64_t sptr = regread( GR(), 4 ) - u64_t( OPSIZE/8 );
    memwrite<OPSIZE>( unisim::component::cxx::processor::intel::SS, sptr, value );
    regwrite( GR(), 4, sptr );
  }


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
  rmwrite( GOP const&, RMOp const& rmop, typename TypeFor<Arch,GOP::SIZE>::u value )
  {
    if (not rmop.is_memory_operand())
      return regwrite( GOP(), rmop.ereg(), value );

    return memwrite<GOP::SIZE>( rmop->segment, rmop->effective_address( *this ), value );
  }

  template <unsigned OPSIZE>
  typename TypeFor<Arch,OPSIZE>::u
  memread( unsigned _seg, u64_t _addr )
  {
    uintptr_t const int_size = (OPSIZE/8);
    addr_t addr = _addr + segbase(_seg), last_addr = addr;
    typename Memory::Page* page = m_mem.getpage( addr );

    typedef typename TypeFor<Arch,OPSIZE>::u u_type;
    u_type result = 0;

    for (uintptr_t idx = 0; idx < int_size; ++idx, ++ addr)
      {
        if ((last_addr ^ addr) >> Memory::Page::s_bits)
          page = m_mem.getpage( addr );
        addr_t offset = addr % (1 << Memory::Page::s_bits);
        result |= (u_type( page->m_storage[offset] ) << (idx*8));
      }

    return result;
  }
  
  
  // unisim::service::interfaces::Registers
  unisim::service::interfaces::Register* GetRegister(char const* name)
  {
    auto reg = regmap.find( name );
    return (reg == regmap.end()) ? 0 : reg->second;
  }
  void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
  {
    // Program counter
    scanner.Append( GetRegister( "%rip" ) );
    // General purpose registers
    // scanner.Append( GetRegister( "%eax" ) );
    // scanner.Append( GetRegister( "%ecx" ) );
    // scanner.Append( GetRegister( "%edx" ) );
    // scanner.Append( GetRegister( "%ebx" ) );
    // scanner.Append( GetRegister( "%esp" ) );
    // scanner.Append( GetRegister( "%ebp" ) );
    // scanner.Append( GetRegister( "%esi" ) );
    // scanner.Append( GetRegister( "%edi" ) );
    // Segments
    // scanner.Append( GetRegister( "%es" ) );
    // scanner.Append( GetRegister( "%cs" ) );
    // scanner.Append( GetRegister( "%ss" ) );
    // scanner.Append( GetRegister( "%ds" ) );
    // scanner.Append( GetRegister( "%fs" ) );
    // scanner.Append( GetRegister( "%gs" ) );
    // FP registers
    // scanner.Append( GetRegister( "%st" ) );
    // scanner.Append( GetRegister( "%st(1)" ) );
    // scanner.Append( GetRegister( "%st(2)" ) );
    // scanner.Append( GetRegister( "%st(3)" ) );
    // scanner.Append( GetRegister( "%st(4)" ) );
    // scanner.Append( GetRegister( "%st(5)" ) );
    // scanner.Append( GetRegister( "%st(6)" ) );
    // scanner.Append( GetRegister( "%st(7)" ) );
  }
  // unisim::service::interfaces::MemoryInjection<ADDRESS>

  bool InjectReadMemory(addr_t addr, void *buffer, uint32_t size) { m_mem.read( (uint8_t*)buffer, addr, size ); return true; }
  bool InjectWriteMemory(addr_t addr, void const* buffer, uint32_t size) { m_mem.write( addr, (uint8_t*)buffer, size ); return true; }
  // Implementation of ExecuteSystemCall
  
  void ExecuteSystemCall( unsigned id )
  {
    if (not linux_os)
      { throw std::logic_error( "No linux OS emulation connected" ); }
    linux_os->ExecuteSystemCall( id );
    // auto los = dynamic_cast<LinuxOS*>( linux_os );
    // los->LogSystemCall( id );
  }
  
  addr_t rip;

  enum ipproc_t { ipjmp = 0, ipcall, ipret };
  addr_t                      getnip() { return rip; }
  void                        setnip( addr_t _rip, ipproc_t ipproc = ipjmp ) { rip = _rip; }
  //void                        addeip( u64_t offset ) { rip += offset; }

  void                        syscall()
  {
    this->ExecuteSystemCall( this->regread( GR(), 0 ) );
  }

  void                        interrupt( uint8_t op, uint8_t code )
  {
    std::cerr << "Unhandled interruption (0x" << std::hex << unsigned( op ) << ", 0x" << unsigned( code ) << ").\n";
    exit( 0 );
  }
  
  uint64_t u64regs[16]; ///< extended reg

  template <class GOP>
  typename TypeFor<Arch,GOP::SIZE>::u regread( GOP const&, unsigned idx )
  {
    return typename TypeFor<Arch,GOP::SIZE>::u( u64regs[idx] );
  }

  u8_t regread( GObLH const&, unsigned idx )
  {
    unsigned reg = idx%4, sh = idx*2 & 8;
    return u8_t( u64regs[reg] >> sh );
  }

  template <class GOP>
  void regwrite( GOP const&, unsigned idx, typename TypeFor<Arch,GOP::SIZE>::u value )
  {
    u64regs[idx] = u64_t( value );
    //gdbchecker.gmark( idx );
  }

  void regwrite( GObLH const&, unsigned idx, u8_t value )
  {
    unsigned reg = idx%4, sh = idx*2 & 8;
    u64regs[reg] = (u64regs[reg] & ~u64_t(0xff << sh)) | ((value & u64_t(0xff)) << sh);
    //gdbchecker.gmark( reg );
  }
  void regwrite( GOb const&, unsigned idx, u8_t value )
  {
    u64regs[idx] = (u64regs[idx] & ~u64_t(0xff)) | ((value & u64_t(0xff)));
    //gdbchecker.gmark( idx );
  }
  void regwrite( GOw const&, unsigned idx, u16_t value )
  {
    u64regs[idx] = (u64regs[idx] & ~u64_t(0xffff)) | ((value & u64_t(0xffff)));
    //gdbchecker.gmark( idx );
  }

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
  
  // FLOATING POINT STATE
protected:
  double                      m_fregs[8];
  unsigned                    m_ftop;
  uint16_t                    m_fcw;
    
public:
  void                        fnanchk( f64_t value ) {};
  unsigned                    ftopread() { return m_ftop; }
  int                         fcwreadRC() const { return int( (m_fcw >> 10) & 3 ); }
  void                        fpush( f64_t value )
  { fnanchk( value ); m_ftop = ((m_ftop + 0x7) & 0x7); m_fregs[m_ftop] = value; /*m_dirtfregs |= (1 << m_ftop);*/ }
  void                        fwrite( unsigned idx, f64_t value )
  { fnanchk( value ); unsigned fidx = (m_ftop + idx) & 0x7; m_fregs[fidx] = value; /*m_dirtfregs |= (1 << fidx);*/ }
  f64_t                       fpop()
  { f64_t value = m_fregs[m_ftop++]; m_ftop &= 0x7; fnanchk( value ); return value; }
  f64_t                       fread( unsigned idx )
  { unsigned fidx = (m_ftop + idx) & 0x7; f64_t value = m_fregs[fidx]; fnanchk( value ); return value; }
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
    
  u64_t tscread() { return instruction_count; }
    
public: 
  struct VUConfig
  {
    static unsigned const BYTECOUNT = 32;
    template <typename T> using TypeInfo = unisim::component::cxx::processor::intel::VectorTypeInfo<T>;
    typedef u8_t Byte;
  };

  unisim::component::cxx::processor::intel::VUnion<VUConfig> umms[16];
  
  uint8_t vmm_storage[16][VUConfig::BYTECOUNT];
  uint32_t mxcsr;

  void mxcswrite(uint32_t v) { mxcsr = v; }
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
  template <class TYPE> TYPE vmm_memread( unsigned seg, addr_t addr, TYPE const& e )
  {
    typedef unisim::component::cxx::processor::intel::atpinfo<Arch,TYPE> atpinfo;
    return TYPE(memread<atpinfo::bitsize>(seg,addr));
  }
  
  f32_t vmm_memread( unsigned seg, addr_t addr, f32_t const& e ) { return fmemread32( seg, addr ); }
  f64_t vmm_memread( unsigned seg, addr_t addr, f64_t const& e ) { return fmemread64( seg, addr ); }
  f80_t vmm_memread( unsigned seg, addr_t addr, f80_t const& e ) { return fmemread80( seg, addr ); }
  
  // Integer case
  template <class TYPE> void vmm_memwrite( unsigned seg, addr_t addr, TYPE const& e )
  {
    typedef unisim::component::cxx::processor::intel::atpinfo<Arch,TYPE> atpinfo;
    memwrite<atpinfo::bitsize>(seg,addr,typename atpinfo::utype(e));
  }
  
  void vmm_memwrite( unsigned seg, addr_t addr, f32_t const& e ) { return fmemwrite32( seg, addr, e ); }
  void vmm_memwrite( unsigned seg, addr_t addr, f64_t const& e ) { return fmemwrite64( seg, addr, e ); }
  void vmm_memwrite( unsigned seg, addr_t addr, f80_t const& e ) { return fmemwrite80( seg, addr, e ); }

  template <class VR, class ELEM>
  ELEM
  vmm_read( VR const& vr, RMOp const& rmop, unsigned sub, ELEM const& e )
  {
    if (not rmop.is_memory_operand()) return vmm_read( vr, rmop.ereg(), sub, e );
    return vmm_memread( rmop->segment, rmop->effective_address( *this ) + sub*VUConfig::TypeInfo<ELEM>::bytecount, e );
  }
    
  template <class VR, class ELEM>
  void
  vmm_write( VR const& vr, RMOp const& rmop, unsigned sub, ELEM const& e )
  {
    if (not rmop.is_memory_operand()) return vmm_write( vr, rmop.ereg(), sub, e );
    return vmm_memwrite( rmop->segment, rmop->effective_address( *this ) + sub*VUConfig::TypeInfo<ELEM>::bytecount, e );
  }

  void xgetbv();
  void cpuid();
  void _DE() { std::cerr << "#DE: Division Error.\n"; throw std::runtime_error("diverr"); }
  void unimplemented() { std::cerr << "Unimplemented.\n"; throw 0; }
  
  typedef unisim::component::cxx::processor::intel::Operation<Arch> Operation;
  Operation* latest_instruction;
  Operation* fetch();

  void stop() { std::cerr << "Processor halt.\n"; throw 0; }

  struct Unimplemented {};
  void noexec( Operation const& op )
  {
    std::cerr
      << "error: no execute method in `" << typeid(op).name() << "'\n"
      << std::hex << op.address << ":\t";
    op.disasm( std::cerr );
    std::cerr << '\n';
    throw Unimplemented();
  }
  
  bool do_disasm;
  uint64_t instruction_count;

  bool Test( bool b ) const { return b; }

  // struct GDBChecker
  // {
  //   GDBChecker() : sink("check.gdb"), visited(), nassertid(0), gdirtmask( 0 ), xdirtmask( 0 ) {}
  //   std::ofstream sink;
  //   std::map<uint64_t,uint64_t> visited;
  //   uint64_t nassertid;
  //   uint32_t gdirtmask;
  //   uint32_t xdirtmask;
  //   void gmark( unsigned reg ) { gdirtmask |= (1 << reg); }
  //   void xmark( unsigned reg ) { xdirtmask |= (1 << reg); }
  //   uint64_t getnew_aid() { if (++nassertid > 0x80000) throw 0; return nassertid; }
  //   void start( Arch const& cpu)
  //   {
  //     sink << "set pagination off\n\n"
  //          << "define insn_assert\n  if $arg1 != $arg2\n    printf \"insn_assert %u failed.\\n\", $arg0\n    bad_assertion\n  end\nend\n\n"
  //          << "break *0x" << std::hex << cpu.rip << "\nrun\n\n";
  //   }
  //   void step( Arch const& cpu )
  //   {
  //     cpu.latest_instruction->disasm( sink << "# " ); sink << "\n";
  //     uint64_t cia = cpu.rip;
  //     uint64_t revisit = visited[cia]++;
  //     sink << "stepi\n";
  //     if (revisit == 0)
  //       sink << "# advance *0x" << std::hex << cia << "\n";
  //     else
  //       sink << "# break *0x" << std::hex << cia << "; cont; cont " << std::dec << revisit << "\n";
  //     sink << "insn_assert " << std::dec << getnew_aid() << " $rip 0x" << std::hex  << cia << '\n';
  //     for (unsigned reg = 0; reg < 16; ++reg)
  //       {
  //         if (not ((gdirtmask>>reg) & 1)) continue;
  //         uint64_t value = cpu.u64regs[reg];
  //         std::ostringstream rn; rn << unisim::component::cxx::processor::intel::DisasmG(GR(),reg);
  //         sink << "insn_assert " << std::dec << getnew_aid() << " $" << &(rn.str().c_str()[1]) << " 0x" << std::hex << value << '\n';
  //       }
  //     gdirtmask = 0;
  //     for (unsigned reg = 0; reg < 16; ++reg)
  //       {
  //         if (not ((xdirtmask>>reg) & 1)) continue;
  //         uint64_t quads[2] = {0};
  //         cpu.umms[reg].transfer( (uint8_t*)&quads[0], &cpu.vmm_storage[reg][0], 16, false );
  //         sink << "insn_assert " << std::dec << getnew_aid() << " $xmm" << reg << ".v2_int64[0] 0x" << std::hex << quads[0] << '\n';
  //         sink << "insn_assert " << std::dec << getnew_aid() << " $xmm" << reg << ".v2_int64[1] 0x" << std::hex << quads[1] << '\n';
  //       }
  //     xdirtmask = 0;
  //   }
  // } gdbchecker;
};

struct UInt128
{
  UInt128() : bits{0,0} {}
  UInt128( uint64_t value ) : bits{value,0} {}
  UInt128( uint64_t hi, uint64_t lo ) : bits{lo,hi} {}

  template <typename T> UInt128 operator << (T lshift) const { UInt128 res(*this); res <<= lshift; return res; }
  template <typename T> UInt128 operator >> (T rshift) const { UInt128 res(*this); res >>= rshift; return res; }
  
  template <typename T>
  UInt128& operator <<= (T lshift)
  {
    if (lshift >= 128) { bits[1] = 0; bits[0] = 0; return *this; }
    if (lshift >= 64) { bits[1] = bits[0]; bits[0] = 0; lshift -= 64; }
    T rshift = 63 - lshift;
    bits[1] = (bits[1] << lshift) | (bits[0] >> rshift);
    bits[0] = (bits[0] << lshift);
    return *this;
  }

  template <typename T>
  UInt128& operator >>= (T rshift)
  {
    if (rshift >= 128) { bits[1] = 0; bits[0] = 0; return *this; }
    if (rshift >= 64) { bits[0] = bits[1]; bits[1] = 0; rshift -= 64; }
    T lshift = 63 - rshift;
    bits[0] = (bits[0] >> rshift) | (bits[1] << lshift);
    bits[1] >>= rshift;
    return *this;
  }

  UInt128 operator | (UInt128 const& rhs) { UInt128 res(*this); res |= rhs; return res; }
  UInt128& operator |= (UInt128 const& rhs) { bits[1] |= rhs.bits[1]; bits[0] |= rhs.bits[0]; return *this; }
  UInt128 operator & (UInt128 const& rhs) { UInt128 res(*this); res &= rhs; return res; }
  UInt128& operator &= (UInt128 const& rhs) { bits[1] &= rhs.bits[1]; bits[0] &= rhs.bits[0]; return *this; }

  unsigned clz() const
  {
    return bits[1] ?
      63 - unisim::util::arithmetic::BitScanReverse(bits[1]) :
      127 - unisim::util::arithmetic::BitScanReverse(bits[0]);
  }
  UInt128& neg()
  {
    bits[0] = ~bits[0] + 1;
    bits[1] = ~bits[1] + uint64_t(not bits[0]);
    return *this;
  }
  
  explicit operator uint64_t () const { return bits[0]; }
  explicit operator bool () const { return bits[0] or bits[1]; }

  uint64_t bits[2];
};

struct SInt128 : public UInt128
{
  SInt128( int64_t value ) : UInt128( value, value >> 63 ) {}
  SInt128( uint64_t value ) : UInt128( value, 0 ) {}
  SInt128( UInt128 const& value ) : UInt128( value ) {}
  template <typename T> UInt128 operator >> (T rshift) const { UInt128 res(*this); res >>= rshift; return res; }
  
  template <typename T>
  UInt128& operator >>= (T rshift)
  {
    if (rshift >= 128) { bits[0] = int64_t(bits[1]) >> 63; bits[1] = bits[0]; return *this; }
    if (rshift >= 64) { bits[0] = bits[1]; bits[1] = int64_t(bits[1]) >> 63; rshift -= 64; }
    T lshift = 63 - rshift;
    bits[0] = (bits[0] >> rshift) | (bits[1] << lshift);
    bits[1] = int64_t(bits[1]) >> rshift;
    return *this;
  }
  
};

void eval_div( Arch& arch, uint64_t& hi, uint64_t& lo, uint64_t divisor )
{
  if (not divisor) arch._DE();
  UInt128 quotient( 0 ), rem( hi, lo );

  for (int pos = 64; pos and rem;)
    {
      int shift = rem.clz();
      if (shift > pos) shift = pos;
      quotient <<= shift;
      rem <<= shift;
      pos = pos - shift;

      if (rem.bits[1] < divisor and pos > 0)
        {
          quotient <<= 1;
          rem <<= 1;
          pos -= 1;
          quotient.bits[0] |= 1;
          rem.bits[1] -= divisor;
        }
      else
        {
          quotient.bits[0] |= rem.bits[1] / divisor;
          rem.bits[1] = rem.bits[1] % divisor;
        }
    }
   
  if (quotient.bits[1]) arch._DE();
  
  lo  = quotient.bits[0];
  hi = rem.bits[1];
}

void eval_div( Arch& arch, int64_t& hi, int64_t& lo, int64_t divisor )
{
  uint64_t uhi = std::abs(hi), ulo = std::abs(lo);
  int64_t sign = (hi >> 63) ^ (lo >> 63);
  eval_div( arch, uhi, ulo, uint64_t(divisor) );
  int64_t shi, slo;
  if (sign) { shi = -uhi; slo = -ulo; }
  else      { shi = +uhi; slo = +ulo; }
  if ((slo >> 63) ^ sign) arch._DE();
  hi = shi; lo = slo;
}

void eval_mul( Arch& arch, uint64_t& hi, uint64_t& lo, uint64_t multiplier )
{
  uint64_t opl = lo, opr = multiplier;
  uint64_t lhi = uint64_t(opl >> 32), llo = uint64_t(uint32_t(opl)), rhi = uint64_t(opr >> 32), rlo = uint64_t(uint32_t(opr));
  uint64_t hihi( lhi*rhi ), hilo( lhi*rlo), lohi( llo*rhi ), lolo( llo*rlo );
  hi = (((lolo >> 32) + uint64_t(uint32_t(hilo)) + uint64_t(uint32_t(lohi))) >> 32) + (hilo >> 32) + (lohi >> 32) + hihi;
  lo = opl * opr;
  arch.flagwrite( Arch::FLAG::OF, bool(hi) );
  arch.flagwrite( Arch::FLAG::CF, bool(hi) );
}

void eval_mul( Arch& arch, int64_t& hi, int64_t& lo, int64_t multiplier )
{
  UInt128 u128( 0, std::abs(lo) );
  int64_t sign = (hi >> 63) ^ (lo >> 63);
  eval_mul( arch, u128.bits[1], u128.bits[0], uint64_t(multiplier) );
  if (sign) u128.neg();
  hi = u128.bits[1]; lo = u128.bits[0];
  bool ovf = (int64_t(u128.bits[0]) >> 63) != int64_t(u128.bits[1]);
  arch.flagwrite( Arch::FLAG::OF, bool(ovf) );
  arch.flagwrite( Arch::FLAG::CF, bool(ovf) );
}

Arch::f64_t sine( Arch::f64_t );
Arch::f64_t cosine( Arch::f64_t );
Arch::f64_t tangent( Arch::f64_t );
Arch::f64_t arctan( Arch::f64_t angle );
Arch::f64_t fmodulo( Arch::f64_t, Arch::f64_t );
Arch::f64_t firound( Arch::f64_t, int x87frnd_mode );
Arch::f64_t power( Arch::f64_t, Arch::f64_t );
Arch::f64_t logarithm( Arch::f64_t );
Arch::f64_t square_root( Arch::f64_t );

namespace unisim { namespace component { namespace cxx { namespace processor { namespace intel {
using unisim::util::arithmetic::BitScanForward;
using unisim::util::arithmetic::BitScanReverse;
using unisim::util::endian::ByteSwap;
}}}}}

#include <unisim/component/cxx/processor/intel/isa/intel.tcc>
#include <unisim/component/cxx/processor/intel/execute.hh>
#include <unisim/component/cxx/processor/intel/math.hh>

template <typename DECODER>
struct ICache : public DECODER
{
  typedef unisim::component::cxx::processor::intel::Operation<Arch> Operation;

  struct Page
  {
    static unsigned const NUM_OPERATIONS_PER_PAGE = 0x1000;
    Page* next;
    uint64_t key;
    Operation* operations[NUM_OPERATIONS_PER_PAGE];
    uint8_t bytes[NUM_OPERATIONS_PER_PAGE+15];
    
    Page( Page* _next, uint64_t _key ) : next( _next ), key( _key ) { memset( operations, 0, sizeof (operations) ); }
    ~Page() { for( unsigned idx = 0; idx < NUM_OPERATIONS_PER_PAGE; ++idx ) delete operations[idx]; delete next; }
  };
    
  static unsigned const NUM_HASH_TABLE_ENTRIES = 0x1000;
  Page* hash_table[NUM_HASH_TABLE_ENTRIES];
  Page* mru_page;
  ICache()
    : mru_page( 0 )
  {
    memset( hash_table, 0, sizeof (hash_table) );
  }
  ~ICache()
  {
    for(unsigned idx = 0; idx < NUM_HASH_TABLE_ENTRIES; ++idx)
      delete hash_table[idx];
  }
  
  Operation* Get( unisim::component::cxx::processor::intel::Mode mode, uint64_t address, uint8_t* bytes )
  {
    uint64_t offset;
    Page* page = GetPage( address, offset );
    Operation* operation = page->operations[offset];
      
    if (operation)
      {
        if ((operation->mode == mode) and (memcmp( &bytes[0], &page->bytes[offset], operation->length ) == 0))
          return operation;
        delete operation;
      }
      
    page->operations[offset] = operation = this->Decode( mode, address, bytes );
    if (not operation) return 0;
    memcpy( &page->bytes[offset], bytes, operation->length );
    
    // {
    //   std::ostringstream buf;
    //   buf << std::hex << address << ":\t" << unisim::component::cxx::processor::intel::DisasmBytes(bytes,operation->length) << '\t';
    //   operation->disasm( buf );
    //   if (certs.insert( buf.str() ).second)
    //     {
    //       std::cerr << "unknown instruction: " << buf.str() << '\n';
    //       return 0;
    //     }
    // }
    
    return operation;
  }

  std::set<std::string> certs;
    
private:
  Page*
  GetPage( uint64_t address, uint64_t& offset )
  {
    uint64_t page_key = address / Page::NUM_OPERATIONS_PER_PAGE;
    offset = address & (Page::NUM_OPERATIONS_PER_PAGE-1);
      
    if (mru_page and (mru_page->key == page_key)) return mru_page;
      
    unsigned index = page_key % NUM_HASH_TABLE_ENTRIES; // hash the key
      
    Page* cur = hash_table[index];
    Page** ref = &hash_table[index];
      
    while (cur) {
      if (cur->key == page_key) {

        *ref = cur->next;
        cur->next = hash_table[index];
        return (mru_page = hash_table[index] = cur);
      }
      ref = &cur->next;
      cur = cur->next;
    }
      
    return (mru_page = hash_table[index] = new Page( hash_table[index], page_key ));
  }
};

struct Decoder
{
  typedef unisim::component::cxx::processor::intel::Operation<Arch> Operation;
  typedef unisim::component::cxx::processor::intel::Mode Mode;
  
  Operation* Decode( Mode mode, uint64_t address, uint8_t* bytes )
  {
    if (Operation* op = getoperation( unisim::component::cxx::processor::intel::InputCode<Arch>( mode, bytes, Arch::OpHeader( address ) ) ))
      return op;
      
    std::cerr << "No decoding for " << std::hex << address << ": " << unisim::component::cxx::processor::intel::DisasmBytes( bytes, 15 ) << std::dec << std::endl;
    return 0;
  }
};

Arch::Operation*
Arch::fetch()
{
  static ICache<Decoder> icache;
  addr_t insn_addr = this->rip;
  asm volatile ("operation_fetch:");
  uint8_t decbuf[15];
  lla_memcpy( decbuf, insn_addr, sizeof (decbuf) );
  Decoder::Mode mode( 1, 0, 1 );

  asm volatile ("operation_decode:");
  latest_instruction = icache.Get( mode, insn_addr, &decbuf[0] );
  if (not latest_instruction) return 0;
  
  this->rip = insn_addr + latest_instruction->length;
    
  if (do_disasm)
    {
      std::ios fmt(NULL);
      fmt.copyfmt(std::cout);
      std::cout << std::hex << insn_addr << ":\t";
      latest_instruction->disasm( std::cout );
      std::cout << " (" << unisim::component::cxx::processor::intel::DisasmBytes(&decbuf[0],latest_instruction->length) << ")\n";
      std::cout.copyfmt(fmt);
    }
    
  ++instruction_count;
  return latest_instruction;
}

void
Arch::xgetbv()
{
  uint32_t a, d, c = this->regread( GOd(), 1 );
  __asm__ ("xgetbv\n\t" : "=a" (a), "=d" (d) : "c" (c));
  this->regwrite( GOd(), 0, a );
  this->regwrite( GOd(), 2, d );
}

// void
// Arch::cpuid()
// {
//   uint32_t level = this->regread( GOd(), 0 ), count = this->regread( GOd(), 1 );
  
//   uint32_t r[4];
  
//   __asm__ ("cpuid\n\t" : "=a" (r[0]), "=c" (r[1]), "=d" (r[2]), "=b" (r[3]) : "0" (level), "1" (count));

//   for (int idx = 0; idx < 4; ++idx)
//     this->regwrite( GOd(), idx, r[idx] );
// }

void
Arch::cpuid()
{
  switch (this->regread( GOd(), 0 )) {
  case 0: {
    this->regwrite( GOd(), 0, u32_t( 4 ) );
  
    char const* name = "GenuineIntel";
    { uint32_t word = 0;
      int idx = 12;
      while (--idx >= 0) {
        word = (word << 8) | name[idx];
        if (idx % 4) continue;
        this->regwrite( GOd(), 3 - (idx/4), u32_t( word ) );
        word = 0;
      }
    }
  } break;
  case 1: {
    uint32_t const eax =
      (6  << 0 /* stepping id */) |
      (0  << 4 /* model */) |
      (15 << 8 /* family */) |
      (0  << 12 /* processor type */) |
      (0  << 16 /* extended model */) |
      (0  << 20 /* extended family */);
    this->regwrite( GOd(), 0, u32_t( eax ) );
    
    uint32_t const ebx =
      (0 <<  0 /* Brand index */) |
      (4 <<  8 /* Cache line size (/ 64bits) */) |
      (1 << 16 /* Maximum number of addressable IDs for logical processors in this physical package* */) |
      (0 << 24 /* Initial APIC ID */);
    this->regwrite( GOd(), 3, u32_t( ebx ) );
    
    uint32_t const ecx =
      (0 << 0x00 /* Streaming SIMD Extensions 3 (SSE3) */) |
      (0 << 0x01 /* PCLMULQDQ Available */) |
      (0 << 0x02 /* 64-bit DS Area */) |
      (0 << 0x03 /* MONITOR/MWAIT */) |
      (0 << 0x04 /* CPL Qualified Debug Store */) |
      (0 << 0x05 /* Virtual Machine Extensions */) |
      (0 << 0x06 /* Safer Mode Extensions */) |
      (0 << 0x07 /* Enhanced Intel SpeedStep® technology */) |
      (0 << 0x08 /* Thermal Monitor 2 */) |
      (0 << 0x09 /* Supplemental Streaming SIMD Extensions 3 (SSSE3) */) |
      (0 << 0x0a /* L1 Context ID */) |
      (0 << 0x0b /* Reserved */) |
      (0 << 0x0c /* FMA */) |
      (0 << 0x0d /* CMPXCHG16B Available */) |
      (0 << 0x0e /* xTPR Update Control */) |
      (0 << 0x0f /* Perfmon and Debug Capability */) |
      (0 << 0x10 /* Reserved */) |
      (0 << 0x11 /* Process-context identifiers */) |
      (0 << 0x12 /* DCA */) |
      (0 << 0x13 /* SSE4.1 */) |
      (0 << 0x14 /* SSE4.2 */) |
      (0 << 0x15 /* x2APIC */) |
      (1 << 0x16 /* MOVBE Available */) |
      (1 << 0x17 /* POPCNT Available */) |
      (0 << 0x18 /* TSC-Deadline */) |
      (0 << 0x19 /* AESNI */) |
      (0 << 0x1a /* XSAVE */) |
      (0 << 0x1b /* OSXSAVE */) |
      (0 << 0x1c /* AVX */) |
      (1 << 0x1d /* F16C */) |
      (1 << 0x1e /* RDRAND Available */) |
      (1 << 0x1f /* Is virtual machine */);
    this->regwrite( GOd(), 1, u32_t( ecx ) );
    
    uint32_t const edx =
      (1 << 0x00 /* Floating Point Unit On-Chip */) |
      (0 << 0x01 /* Virtual 8086 Mode Enhancements */) |
      (0 << 0x02 /* Debugging Extensions */) |
      (0 << 0x03 /* Page Size Extension */) |
      (0 << 0x04 /* Time Stamp Counter */) |
      (0 << 0x05 /* Model Specific Registers RDMSR and WRMSR Instructions */) |
      (0 << 0x06 /* Physical Address Extension */) |
      (0 << 0x07 /* Machine Check Exception */) |
      (0 << 0x08 /* CMPXCHG8B Available */) |
      (0 << 0x09 /* APIC On-Chip */) |
      (0 << 0x0a /* Reserved */) |
      (0 << 0x0b /* SYSENTER and SYSEXIT Instructions */) |
      (0 << 0x0c /* Memory Type Range Registers */) |
      (0 << 0x0d /* Page Global Bit */) |
      (0 << 0x0e /* Machine Check ARCHitecture */) |
      (1 << 0x0f /* Conditional Move Instructions */) |
      (0 << 0x10 /* Page Attribute Table */) |
      (0 << 0x11 /* 36-Bit Page Size Extension */) |
      (0 << 0x12 /* Processor Serial Number */) |
      (0 << 0x13 /* CLFLUSH Instruction */) |
      (0 << 0x14 /* Reserved */) |
      (0 << 0x15 /* Debug Store */) |
      (0 << 0x16 /* Thermal Monitor and Software Controlled Clock Facilities */) |
      (0 << 0x17 /* Intel MMX Technology */) |
      (0 << 0x18 /* FXSAVE and FXRSTOR Instructions */) |
      (0 << 0x19 /* SSE */) |
      (0 << 0x1a /* SSE2 */) |
      (0 << 0x1b /* Self Snoop */) |
      (0 << 0x1c /* Max APIC IDs reserved field is Valid */) |
      (0 << 0x1d /* Thermal Monitor */) |
      (0 << 0x1e /* Resrved */) |
      (0 << 0x1f /* Pending Break Enable */);
    this->regwrite( GOd(), 2, u32_t( edx ) );
    
  } break;
  case 2: {
    this->regwrite( GOd(), 0, u32_t( 0 ) );
    this->regwrite( GOd(), 3, u32_t( 0 ) );
    this->regwrite( GOd(), 1, u32_t( 0 ) );
    this->regwrite( GOd(), 2, u32_t( 0 ) );
  } break;
  case 4: {
    // Small cache config
    switch (this->regread( GOd(), 1 )) { // %ecx holds requested cache id
    case 0: { // L1 D-CACHE
      this->regwrite( GOd(), 0, u32_t( (1 << 26) | (0 << 14) | (1 << 8) | (1 << 5) | (1 << 0) ) ); // 0x4000121
      this->regwrite( GOd(), 3, u32_t( (0 << 26) | (3 << 22) | (0 << 12) | (0x3f << 0) ) ); // 0x1c0003f
      this->regwrite( GOd(), 1, u32_t( (0 << 22) | (0x03f << 0) ) ); // 0x000003f
      this->regwrite( GOd(), 2, u32_t( 0x0000001 ) ); // 0x0000001
    } break;
    case 1: { // L1 I-CACHE
      this->regwrite( GOd(), 0, u32_t( (1 << 26) | (0 << 14) | (1 << 8) | (1 << 5) | (2 << 0) ) ); // 0x4000122
      this->regwrite( GOd(), 3, u32_t( (0 << 26) | (3 << 22) | (0 << 12) | (0x3f << 0) ) ); // 0x1c0003f
      this->regwrite( GOd(), 1, u32_t( (0 << 22) | (0x03f << 0) ) ); // 0x000003f
      this->regwrite( GOd(), 2, u32_t( 0x0000001 ) ); // 0x0000001
    } break;
    case 2: { // L2 U-CACHE
      this->regwrite( GOd(), 0, u32_t( (1 << 26) | (1 << 14) | (1 << 8) | (2 << 5) | (3 << 0) ) ); // 0x4000143
      this->regwrite( GOd(), 3, u32_t( (1 << 26) | (3 << 22) | (0 << 12) | (0x3f << 0) ) ); // 0x5c0003f
      this->regwrite( GOd(), 1, u32_t( (0 << 22) | (0xfff << 0) ) ); // 0x0000fff
      this->regwrite( GOd(), 2, u32_t( 0x0000001 ) ); // 0x0000001
    } break;
    case 3: { // TERMINATING NULL ENTRY
      // 0, 0, 0, 0
      this->regwrite( GOd(), 0, u32_t( 0 ) );
      this->regwrite( GOd(), 3, u32_t( 0 ) );
      this->regwrite( GOd(), 1, u32_t( 0 ) );
      this->regwrite( GOd(), 2, u32_t( 0 ) );
    } break;
    }
  } break;
  
  case 0x80000000: {
    this->regwrite( GOd(), 0, u32_t( 0x80000001 ) );
    this->regwrite( GOd(), 3, u32_t( 0 ) );
    this->regwrite( GOd(), 1, u32_t( 0 ) );
    this->regwrite( GOd(), 2, u32_t( 0 ) );
  } break;
  case 0x80000001: {
    this->regwrite( GOd(), 0, u32_t( 0 ) );
    this->regwrite( GOd(), 3, u32_t( 0 ) );
    this->regwrite( GOd(), 1, u32_t( 0 ) );
    this->regwrite( GOd(), 2, u32_t( 0 ) );
  } break;
  default:
    std::cerr << "Unknown cmd for cpuid, " << std::hex
              << "%eax=0x" << this->regread( GOd(), 0 ) << ", "
              << "%eip=0x" << latest_instruction->address << "\n";
    break;
  }
}

struct FPException : public std::exception {};

Arch::f64_t sine( Arch::f64_t angle ) { return sin( angle ); }

Arch::f64_t cosine( Arch::f64_t angle ) { return cos( angle ); }

Arch::f64_t tangent( Arch::f64_t angle ) { return tan( angle ); }

Arch::f64_t arctan( Arch::f64_t angle ) { return atan( angle ); }

Arch::f64_t fmodulo( Arch::f64_t dividend, Arch::f64_t modulus ) { return fmod( dividend, modulus ); }

Arch::f64_t firound( Arch::f64_t value, int x87frnd_mode )
{
  // double floor_res = floor( value );

  double result = unisim::component::cxx::processor::intel::x87frnd( value, unisim::component::cxx::processor::intel::x87frnd_mode_t( x87frnd_mode ) );

  // if (floor_res != result) {
  //   std::cerr << "Rounding result fixed; old: " << floor_res << ", new: " << result << std::endl;
  // }

  return result;
}

Arch::f64_t power( Arch::f64_t exponent, Arch::f64_t value ) { return pow( exponent, value ); }

Arch::f64_t logarithm( Arch::f64_t value ) { return log2( value ); }

Arch::f64_t square_root( Arch::f64_t value ) { return sqrt( value ); }

int
main( int argc, char *argv[] )
{
  uintptr_t simargs_idx = 1;
  std::vector<std::string> simargs(&argv[simargs_idx], &argv[argc]);
  
  {
    std::cerr << "arguments:\n";
    unsigned idx = 0;
    for (std::string const& arg : simargs) {
      std::cerr << "  args[" << idx << "]: " << arg << '\n';
    }
  }
  
  if (simargs.size() == 0) {
    std::cerr << "Simulation command line empty." << std::endl;
    return 1;
  }

  Arch cpu;
  LinuxOS linux64( std::cerr, &cpu, &cpu, &cpu );
  cpu.SetLinuxOS( &linux64 );
  cpu.do_disasm = false;
  linux64.Setup( false );
  
  // Loading image
  std::cerr << "*** Loading elf image: " << simargs[0] << " ***" << std::endl;
  if (char* var = getenv("CORE_SETUP"))
    {
      linux64.Core( simargs[0] );
      linux64.SetBrk( strtoull(var,&var,0) );
      if (*var != ':')  throw 0;
      cpu.fs_base = strtoull(var+1,&var,0);
      if (*var != '\0') throw 0;
      cpu.finit();
    }
  else
    {
      //linux64.ApplyHostEnvironment();
      {
        std::vector<std::string> envs{"LANG=C"};
        linux64.SetEnvironment( envs );
      }
      linux64.Process( simargs );
    }
  
  std::cerr << "\n*** Run ***" << std::endl;
  //cpu.gdbchecker.start(cpu);
  while (not linux64.exited)
    {
      Arch::Operation* op = cpu.fetch();
      if (not op)
        return 1;
      // op->disasm( std::cerr );
      // std::cerr << std::endl;
      asm volatile ("operation_execute:");
      op->execute( cpu );
      //cpu.gdbchecker.step(cpu);
      // { uint64_t chksum = 0; for (unsigned idx = 0; idx < 8; ++idx) chksum ^= cpu.regread( GOd(), idx ); std::cerr << '[' << std::hex << chksum << std::dec << ']'; }
      if (cpu.instruction_count >= 0x100000)
        break;
      // if ((cpu.instruction_count % 0x1000000) == 0)
      //   { std::cerr << "Executed instructions: " << std::dec << cpu.instruction_count << " (" << std::hex << op->address << std::dec << ")"<< std::endl; }
    }

  std::cerr << "Program exited with status:" << linux64.app_ret_status << std::endl;
  
  return 0;
}


Arch::u32_t Arch::EFlagsRegister::eflagsread() const { return unisim::component::cxx::processor::intel::eflagsread( cpu ); }
