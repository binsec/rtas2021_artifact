/*
 *  Copyright (c) 2019,
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

#include <testrun.hh>
#include <unisim/util/endian/endian.hh>
#include <ostream>
#include <cmath>

namespace unisim { namespace component { namespace cxx { namespace processor { namespace intel {
          using unisim::util::arithmetic::BitScanForward;
          using unisim::util::arithmetic::BitScanReverse;
          using unisim::util::endian::ByteSwap;
          using ut::concrete::sine;
          using ut::concrete::cosine;
          using ut::concrete::tangent;
          using ut::concrete::arctan;
          using ut::concrete::fmodulo;
          using ut::concrete::firound;
          using ut::concrete::power;
          using ut::concrete::logarithm;
          using ut::concrete::square_root;
        }}}}}

#include <unisim/component/cxx/processor/intel/isa/intel.tcc>
#include <unisim/component/cxx/processor/intel/execute.hh>
#include <unisim/component/cxx/processor/intel/math.hh>

namespace ut
{
  namespace
  {
    template <typename T>
    void HexStore( std::ostream& sink, T value )
    {
      unsigned const digits = 2*sizeof (T);
      char buf[1 + digits];
      buf[digits] = '\0';
      for (int idx = digits; --idx >= 0;)
        {
          buf[idx] = "0123456789abcdef"[value & 0xf];
          value >>= 4;
        }
      sink << &buf[0];
    }

    template <typename T>
    void HexLoad( T& value, char const*& buf )
    {
      unsigned const digits = 2*sizeof (T);
      T scratch = 0;
      char const* ptr = buf;
      for (char const* end = &buf[digits]; ptr < end; ++ptr)
        {
          char ch = *ptr;
          if      ('0' <= ch and ch <= '9') scratch = (scratch << 4) | T( ch - '0' +  0 );
          else if ('a' <= ch and ch <= 'f') scratch = (scratch << 4) | T( ch - 'a' + 10 );
          else if ('A' <= ch and ch <= 'F') scratch = (scratch << 4) | T( ch - 'A' + 10 );
          else break;
        }
      value = scratch;
      buf = ptr;
    }
  }

  TestbedBase::TestbedBase(char const* seed, uint64_t* elems, uintptr_t count)
  {
    for (unsigned idx = 0, end = 5; idx < end; ++idx)
      HexLoad( rng.State(idx), seed );
    for (unsigned idx = 0, end = count; idx < end; ++idx)
      HexLoad( elems[idx], seed );
    HexLoad( counter, seed );
  }

  void TestbedBase::serial( std::ostream& sink, uint64_t* elems, uintptr_t count ) const
  {
    for (unsigned idx = 0, end = 5; idx < end; ++idx)
      HexStore( sink, rng.State(idx) );
    for (unsigned idx = 0, end = count; idx < end; ++idx)
      HexStore( sink, elems[idx] );
    HexStore( sink, counter );
  }

  namespace concrete
  {
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
    
        Page( Page* _next, uint64_t _key ) : next( _next ), key( _key ), operations() {}
        ~Page() { for( unsigned idx = 0; idx < NUM_OPERATIONS_PER_PAGE; ++idx ) delete operations[idx]; delete next; }
      };
    
      static unsigned const NUM_HASH_TABLE_ENTRIES = 0x1000;
      Page* hash_table[NUM_HASH_TABLE_ENTRIES];
      Page* mru_page;
      ICache() : hash_table(), mru_page( 0 ) {}
      ~ICache() { for(unsigned idx = 0; idx < NUM_HASH_TABLE_ENTRIES; ++idx) delete hash_table[idx]; }
  
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
      memcpy(&decbuf[0], reinterpret_cast<uint8_t const*>(insn_addr), sizeof decbuf);
      Decoder::Mode mode( 1, 0, 1 );

      asm volatile ("operation_decode:");
      latest_instruction = icache.Get( mode, insn_addr, &decbuf[0] );
      if (not latest_instruction) throw 0;
  
      this->rip = insn_addr + latest_instruction->length;
    
      return latest_instruction;
    }

    // void
    // Arch::cpuid()
    // {
    //   switch (this->regread( GOd(), 0 )) {
    //   case 0: {
    //     this->regwrite( GOd(), 0, u32_t( 4 ) );
  
    //     char const* name = "GenuineIntel";
    //     { uint32_t word = 0;
    //       int idx = 12;
    //       while (--idx >= 0) {
    //         word = (word << 8) | name[idx];
    //         if (idx % 4) continue;
    //         this->regwrite( GOd(), 3 - (idx/4), u32_t( word ) );
    //         word = 0;
    //       }
    //     }
    //   } break;
    //   case 1: {
    //     uint32_t const eax =
    //       (6  << 0 /* stepping id */) |
    //       (0  << 4 /* model */) |
    //       (15 << 8 /* family */) |
    //       (0  << 12 /* processor type */) |
    //       (0  << 16 /* extended model */) |
    //       (0  << 20 /* extended family */);
    //     this->regwrite( GOd(), 0, u32_t( eax ) );
    
    //     uint32_t const ebx =
    //       (0 <<  0 /* Brand index */) |
    //       (4 <<  8 /* Cache line size (/ 64bits) */) |
    //       (1 << 16 /* Maximum number of addressable IDs for logical processors in this physical package* */) |
    //       (0 << 24 /* Initial APIC ID */);
    //     this->regwrite( GOd(), 3, u32_t( ebx ) );
    
    //     uint32_t const ecx =
    //       (0 << 0x00 /* Streaming SIMD Extensions 3 (SSE3) */) |
    //       (0 << 0x01 /* PCLMULQDQ Available */) |
    //       (0 << 0x02 /* 64-bit DS Area */) |
    //       (0 << 0x03 /* MONITOR/MWAIT */) |
    //       (0 << 0x04 /* CPL Qualified Debug Store */) |
    //       (0 << 0x05 /* Virtual Machine Extensions */) |
    //       (0 << 0x06 /* Safer Mode Extensions */) |
    //       (0 << 0x07 /* Enhanced Intel SpeedStep® technology */) |
    //       (0 << 0x08 /* Thermal Monitor 2 */) |
    //       (0 << 0x09 /* Supplemental Streaming SIMD Extensions 3 (SSSE3) */) |
    //       (0 << 0x0a /* L1 Context ID */) |
    //       (0 << 0x0b /* Reserved */) |
    //       (0 << 0x0c /* FMA */) |
    //       (0 << 0x0d /* CMPXCHG16B Available */) |
    //       (0 << 0x0e /* xTPR Update Control */) |
    //       (0 << 0x0f /* Perfmon and Debug Capability */) |
    //       (0 << 0x10 /* Reserved */) |
    //       (0 << 0x11 /* Process-context identifiers */) |
    //       (0 << 0x12 /* DCA */) |
    //       (0 << 0x13 /* SSE4.1 */) |
    //       (0 << 0x14 /* SSE4.2 */) |
    //       (0 << 0x15 /* x2APIC */) |
    //       (1 << 0x16 /* MOVBE Available */) |
    //       (1 << 0x17 /* POPCNT Available */) |
    //       (0 << 0x18 /* TSC-Deadline */) |
    //       (0 << 0x19 /* AESNI */) |
    //       (0 << 0x1a /* XSAVE */) |
    //       (0 << 0x1b /* OSXSAVE */) |
    //       (0 << 0x1c /* AVX */) |
    //       (1 << 0x1d /* F16C */) |
    //       (1 << 0x1e /* RDRAND Available */) |
    //       (1 << 0x1f /* Is virtual machine */);
    //     this->regwrite( GOd(), 1, u32_t( ecx ) );
    
    //     uint32_t const edx =
    //       (1 << 0x00 /* Floating Point Unit On-Chip */) |
    //       (0 << 0x01 /* Virtual 8086 Mode Enhancements */) |
    //       (0 << 0x02 /* Debugging Extensions */) |
    //       (0 << 0x03 /* Page Size Extension */) |
    //       (0 << 0x04 /* Time Stamp Counter */) |
    //       (0 << 0x05 /* Model Specific Registers RDMSR and WRMSR Instructions */) |
    //       (0 << 0x06 /* Physical Address Extension */) |
    //       (0 << 0x07 /* Machine Check Exception */) |
    //       (0 << 0x08 /* CMPXCHG8B Available */) |
    //       (0 << 0x09 /* APIC On-Chip */) |
    //       (0 << 0x0a /* Reserved */) |
    //       (0 << 0x0b /* SYSENTER and SYSEXIT Instructions */) |
    //       (0 << 0x0c /* Memory Type Range Registers */) |
    //       (0 << 0x0d /* Page Global Bit */) |
    //       (0 << 0x0e /* Machine Check ARCHitecture */) |
    //       (1 << 0x0f /* Conditional Move Instructions */) |
    //       (0 << 0x10 /* Page Attribute Table */) |
    //       (0 << 0x11 /* 36-Bit Page Size Extension */) |
    //       (0 << 0x12 /* Processor Serial Number */) |
    //       (0 << 0x13 /* CLFLUSH Instruction */) |
    //       (0 << 0x14 /* Reserved */) |
    //       (0 << 0x15 /* Debug Store */) |
    //       (0 << 0x16 /* Thermal Monitor and Software Controlled Clock Facilities */) |
    //       (0 << 0x17 /* Intel MMX Technology */) |
    //       (0 << 0x18 /* FXSAVE and FXRSTOR Instructions */) |
    //       (0 << 0x19 /* SSE */) |
    //       (0 << 0x1a /* SSE2 */) |
    //       (0 << 0x1b /* Self Snoop */) |
    //       (0 << 0x1c /* Max APIC IDs reserved field is Valid */) |
    //       (0 << 0x1d /* Thermal Monitor */) |
    //       (0 << 0x1e /* Resrved */) |
    //       (0 << 0x1f /* Pending Break Enable */);
    //     this->regwrite( GOd(), 2, u32_t( edx ) );
    
    //   } break;
    //   case 2: {
    //     this->regwrite( GOd(), 0, u32_t( 0 ) );
    //     this->regwrite( GOd(), 3, u32_t( 0 ) );
    //     this->regwrite( GOd(), 1, u32_t( 0 ) );
    //     this->regwrite( GOd(), 2, u32_t( 0 ) );
    //   } break;
    //   case 4: {
    //     // Small cache config
    //     switch (this->regread( GOd(), 1 )) { // %ecx holds requested cache id
    //     case 0: { // L1 D-CACHE
    //       this->regwrite( GOd(), 0, u32_t( (1 << 26) | (0 << 14) | (1 << 8) | (1 << 5) | (1 << 0) ) ); // 0x4000121
    //       this->regwrite( GOd(), 3, u32_t( (0 << 26) | (3 << 22) | (0 << 12) | (0x3f << 0) ) ); // 0x1c0003f
    //       this->regwrite( GOd(), 1, u32_t( (0 << 22) | (0x03f << 0) ) ); // 0x000003f
    //       this->regwrite( GOd(), 2, u32_t( 0x0000001 ) ); // 0x0000001
    //     } break;
    //     case 1: { // L1 I-CACHE
    //       this->regwrite( GOd(), 0, u32_t( (1 << 26) | (0 << 14) | (1 << 8) | (1 << 5) | (2 << 0) ) ); // 0x4000122
    //       this->regwrite( GOd(), 3, u32_t( (0 << 26) | (3 << 22) | (0 << 12) | (0x3f << 0) ) ); // 0x1c0003f
    //       this->regwrite( GOd(), 1, u32_t( (0 << 22) | (0x03f << 0) ) ); // 0x000003f
    //       this->regwrite( GOd(), 2, u32_t( 0x0000001 ) ); // 0x0000001
    //     } break;
    //     case 2: { // L2 U-CACHE
    //       this->regwrite( GOd(), 0, u32_t( (1 << 26) | (1 << 14) | (1 << 8) | (2 << 5) | (3 << 0) ) ); // 0x4000143
    //       this->regwrite( GOd(), 3, u32_t( (1 << 26) | (3 << 22) | (0 << 12) | (0x3f << 0) ) ); // 0x5c0003f
    //       this->regwrite( GOd(), 1, u32_t( (0 << 22) | (0xfff << 0) ) ); // 0x0000fff
    //       this->regwrite( GOd(), 2, u32_t( 0x0000001 ) ); // 0x0000001
    //     } break;
    //     case 3: { // TERMINATING NULL ENTRY
    //       // 0, 0, 0, 0
    //       this->regwrite( GOd(), 0, u32_t( 0 ) );
    //       this->regwrite( GOd(), 3, u32_t( 0 ) );
    //       this->regwrite( GOd(), 1, u32_t( 0 ) );
    //       this->regwrite( GOd(), 2, u32_t( 0 ) );
    //     } break;
    //     }
    //   } break;
  
    //   case 0x80000000: {
    //     this->regwrite( GOd(), 0, u32_t( 0x80000001 ) );
    //     this->regwrite( GOd(), 3, u32_t( 0 ) );
    //     this->regwrite( GOd(), 1, u32_t( 0 ) );
    //     this->regwrite( GOd(), 2, u32_t( 0 ) );
    //   } break;
    //   case 0x80000001: {
    //     this->regwrite( GOd(), 0, u32_t( 0 ) );
    //     this->regwrite( GOd(), 3, u32_t( 0 ) );
    //     this->regwrite( GOd(), 1, u32_t( 0 ) );
    //     this->regwrite( GOd(), 2, u32_t( 0 ) );
    //   } break;
    //   default:
    //     std::cerr << "Unknown cmd for cpuid, " << std::hex
    //               << "%eax=0x" << this->regread( GOd(), 0 ) << ", "
    //               << "%eip=0x" << latest_instruction->address << "\n";
    //     break;
    //   }
    // }

    // struct FPException : public std::exception {};

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

    // int
    // main( int argc, char *argv[] )
    // {
    //   uintptr_t simargs_idx = 1;
    //   std::vector<std::string> simargs(&argv[simargs_idx], &argv[argc]);
  
    //   {
    //     std::cerr << "arguments:\n";
    //     unsigned idx = 0;
    //     for (std::string const& arg : simargs) {
    //       std::cerr << "  args[" << idx << "]: " << arg << '\n';
    //     }
    //   }
  
    //   if (simargs.size() == 0) {
    //     std::cerr << "Simulation command line empty." << std::endl;
    //     return 1;
    //   }

    //   Arch cpu;
    //   LinuxOS linux64( std::cerr, &cpu, &cpu, &cpu );
    //   cpu.SetLinuxOS( &linux64 );
    //   cpu.do_disasm = false;
    //   linux64.Setup( false );
  
    //   // Loading image
    //   std::cerr << "*** Loading elf image: " << simargs[0] << " ***" << std::endl;
    //   if (char* var = getenv("CORE_SETUP"))
    //     {
    //       linux64.Core( simargs[0] );
    //       linux64.SetBrk( strtoull(var,&var,0) );
    //       if (*var != ':')  throw 0;
    //       cpu.fs_base = strtoull(var+1,&var,0);
    //       if (*var != '\0') throw 0;
    //       cpu.finit();
    //     }
    //   else
    //     {
    //       //linux64.ApplyHostEnvironment();
    //       {
    //         std::vector<std::string> envs{"LANG=C"};
    //         linux64.SetEnvironment( envs );
    //       }
    //       linux64.Process( simargs );
    //     }
  
    //   std::cerr << "\n*** Run ***" << std::endl;
    //   //cpu.gdbchecker.start(cpu);
    //   while (not linux64.exited)
    //     {
    //       Arch::Operation* op = cpu.fetch();
    //       if (not op)
    //         return 1;
    //       // op->disasm( std::cerr );
    //       // std::cerr << std::endl;
    //       asm volatile ("operation_execute:");
    //       op->execute( cpu );
    //       //cpu.gdbchecker.step(cpu);
    //       // { uint64_t chksum = 0; for (unsigned idx = 0; idx < 8; ++idx) chksum ^= cpu.regread( GOd(), idx ); std::cerr << '[' << std::hex << chksum << std::dec << ']'; }
    //       if (cpu.instruction_count >= 0x100000)
    //         break;
    //       // if ((cpu.instruction_count % 0x1000000) == 0)
    //       //   { std::cerr << "Executed instructions: " << std::dec << cpu.instruction_count << " (" << std::hex << op->address << std::dec << ")"<< std::endl; }
    //     }

    //   std::cerr << "Program exited with status:" << linux64.app_ret_status << std::endl;
  
  
    //   return 0;
    // }


    // Arch::u32_t Arch::EFlagsRegister::eflagsread() const { return unisim::component::cxx::processor::intel::eflagsread( cpu ); }
    
    void Arch::_DE()
    {
      std::cerr << "#DE: Division Error.\n"; throw std::runtime_error("diverr");
    }

    void
    Arch::run(ut::Interface::testcode_t testcode, uint64_t* data)
    {
      u64_t const magic_return_address = 0xdeadc0dedeadc0deULL;

      uint64_t sim_stack[32];
      regwrite( GOq(), 4, reinterpret_cast<u64_t>( &sim_stack[0] ) + sizeof sim_stack );
      
      push<64>( magic_return_address );
      setnip( reinterpret_cast<addr_t>(testcode), ipcall );
      regwrite( GOq(), 7, reinterpret_cast<u64_t>( data ) );
      
      int ttl = 100;
      do {
        Operation* op = fetch();

        asm volatile ("operation_execute:");
        op->execute( *this );
        if (--ttl < 0) throw 0;
      } while (rip != magic_return_address);
    }

    void Arch::segregwrite( unsigned idx, uint16_t value ) { throw std::runtime_error("segment write"); }
    uint16_t Arch::segregread( unsigned idx ) { throw std::runtime_error("segment read"); return 0; }
    
    void
    Arch::unimplemented()
    {
      std::cerr << "Unimplemented.\n";
      throw 0;
    }

    void Arch::syscall() { throw std::runtime_error("syscall"); }
    void Arch::interrupt( int op, int code ) { throw std::runtime_error("interrupt"); }
    void Arch::stop() { throw std::runtime_error("halt"); }
    void Arch::xgetbv() { throw std::runtime_error("xgetbv"); } 
    void Arch::cpuid() { throw std::runtime_error("cpuid"); }
    uint64_t Arch::tscread() { throw std::runtime_error("tscread"); return 0; }


    void
    Arch::noexec( Arch::Operation const& op )
    {
      std::cerr
        << "error: no execute method in `" << typeid(op).name() << "'\n"
        << std::hex << op.address << ":\t";
      op.disasm( std::cerr );
      std::cerr << '\n';
      throw Unimplemented();
    }
    void
    Arch::fxam()
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
    
  } /* end of namespace ut */
  
} /* end of namespace ut */
