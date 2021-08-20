/*
 *  Copyright (c) 2007-2015,
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

#include <unisim/component/cxx/processor/intel/arch.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <stdexcept>

namespace unisim { namespace component { namespace cxx { namespace processor { namespace intel {
using unisim::util::arithmetic::BitScanForward;
using unisim::util::arithmetic::BitScanReverse;
using unisim::util::endian::ByteSwap;

void eval_div( Arch& arch, uint64_t& hi, uint64_t& lo, uint64_t divisor )    { throw std::runtime_error( "operation unvailable in 32 bit mode" ); }
void eval_div( Arch& arch, int64_t& hi, int64_t& lo, int64_t divisor )       { throw std::runtime_error( "operation unvailable in 32 bit mode" ); }
void eval_mul( Arch& arch, uint64_t& hi, uint64_t& lo, uint64_t multiplier ) { throw std::runtime_error( "operation unvailable in 32 bit mode" ); }
void eval_mul( Arch& arch, int64_t& hi, int64_t& lo, int64_t multiplier )    { throw std::runtime_error( "operation unvailable in 32 bit mode" ); }
}}}}}

#include <unisim/component/cxx/processor/intel/isa/intel.tcc>
#include <unisim/component/cxx/processor/intel/math.hh>
#include <iomanip>
#include <cmath>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace intel {
  
  Arch::Arch()
    : m_running( true ), m_instcount( 0 ), m_disasm( false ), m_latest_insn( 0 )
    , m_EIP( 0 ), m_ftop( 0 ), m_fcw( 0x23f ), m_dirtfregs( 0 )
    , umms(), vmm_storage(), mxcsr()
  {
    std::memset( static_cast<void*>( &m_flags[0] ), 0, sizeof (m_flags) );
    std::memset( static_cast<void*>( &m_regs[0] ), 0, sizeof (m_regs) );
    std::memset( static_cast<void*>( &m_fregs[0] ), 0, sizeof (m_fregs) );
    std::memset( static_cast<void*>( &m_gdt_bases[0] ), 0, sizeof (m_gdt_bases) );
  }
  
  template <typename DECODER>
  struct ICache : public DECODER
  {
    struct Page
    {
      static unsigned const NUM_OPERATIONS_PER_PAGE = 0x1000;
      Page* next;
      uint32_t key;
      Operation<Arch>* operations[NUM_OPERATIONS_PER_PAGE];
      uint8_t bytes[NUM_OPERATIONS_PER_PAGE+15];
      
      Page( Page* _next, uint32_t _key ) : next( _next ), key( _key ) { memset( operations, 0, sizeof (operations) ); }
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
    
    Operation<Arch>*  Get( Mode mode, uint32_t address, uint8_t* bytes )
    {
      uint32_t offset;
      Page* page = GetPage( address, offset );
      Operation<Arch>* operation = page->operations[offset];
      
      if (operation) {
        if ((operation->mode == mode) and (memcmp( &bytes[0], &page->bytes[offset], operation->length ) == 0))
          return operation;
        delete operation;
      }
      
      page->operations[offset] = operation = this->Decode( mode, address, bytes );
      memcpy( &page->bytes[offset], &bytes[0], operation->length );
      
      return operation;
    }
    
  private:
    Page*
    GetPage( uint32_t address, uint32_t& offset )
    {
      uint32_t page_key = address / Page::NUM_OPERATIONS_PER_PAGE;
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
    Operation<Arch>* Decode( Mode mode, uint32_t address, uint8_t* bytes )
    {
      if (Operation<Arch>* op = getoperation( InputCode<Arch>( mode, bytes, Arch::OpHeader( address ) ) ))
        return op;
      
      std::cerr << "No decoding for " << DisasmBytes( bytes, 16 ) << " @" << std::hex << address << std::endl;
      throw 0;
      return 0;
    }
  };
  

  Operation<Arch>*
  Arch::fetch()
  {
    static ICache<Decoder> icache;
    uint32_t eip = m_EIP;
    uint8_t decbuf[15];
    lla_memcpy( decbuf, eip, sizeof (decbuf) );
    Mode mode( 0, 1, 1 );
    
    Operation<Arch>* operation = icache.Get( mode, eip, &decbuf[0] );
    
    m_EIP = eip + operation->length;
    
    if (m_disasm) {
      std::ios fmt(NULL);
      fmt.copyfmt(std::cerr);
      std::cerr << "#0x" << std::setfill('0') << std::setw(8) << std::hex << eip;
      std::cerr.copyfmt(fmt);
      operation->disasm( std::cerr );
      std::cerr.copyfmt(fmt);
      std::cerr << " (" << DisasmBytes( &decbuf[0], operation->length ) << ")" << std::endl;
      std::cerr.copyfmt(fmt);
    }
    
    ++m_instcount;
    return operation;
  }

  void
  Arch::noexec( Operation<Arch> const& op )
  {
    std::cerr
      << "error: no execute method in `" << typeid(op).name() << "'\n"
      << std::hex << op.address << ":\t";
    op.disasm( std::cerr );
    std::cerr << '\n';
    throw 0;
  }
    
  void
  Arch::xgetbv()
  {
    // uint32_t a, d, c = this->regread( GOd(), 1 );
    // __asm__ ("xgetbv\n\t" : "=a" (a), "=d" (d) : "c" (c));
    // this->regwrite( GOd(), 0, a );
    // this->regwrite( GOd(), 2, d );
    throw 0;
  }
  
  
  void
  Arch::cpuid()
  {
    switch (this->regread( GOd(), 0 )) {
    case 0: {
      this->regwrite( GOd(), 0, u32_t( 1 ) );
  
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
                << "%eip=0x" << m_latest_insn->address << "\n";
      throw "not implemented";
      break;
    }
  }

  Arch::f64_t sine( Arch::f64_t angle ) { return sin( angle ); }

  Arch::f64_t cosine( Arch::f64_t angle ) { return cos( angle ); }

  Arch::f64_t tangent( Arch::f64_t angle ) { return tan( angle ); }

  Arch::f64_t arctan( Arch::f64_t angle ) { return atan( angle ); }

  Arch::f64_t fmodulo( Arch::f64_t dividend, Arch::f64_t modulus ) { return fmod( dividend, modulus ); }

  Arch::f64_t firound( Arch::f64_t value, int x87frnd_mode )
  {
    // double floor_res = floor( value );                                                                                                                                                     
    double result = intel::x87frnd( value, intel::x87frnd_mode_t( x87frnd_mode ) );
    // if (floor_res != result) {                                                                                                                                                             
    //   std::cerr << "Rounding result fixed; old: " << floor_res << ", new: " << result << std::endl;                                                                                        
    // }                                                                                                                                                                                      

    return result;
  }

  Arch::f64_t power( Arch::f64_t exponent, Arch::f64_t value ) { return pow( exponent, value ); }

  Arch::f64_t logarithm( Arch::f64_t value ) { return log2( value ); }

  Arch::f64_t square_root( Arch::f64_t value ) { return sqrt( value ); }

} // end of namespace intel
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
