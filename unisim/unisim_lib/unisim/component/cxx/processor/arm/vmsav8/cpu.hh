/*
 *  Copyright (c) 2007-2016,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or withou
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specifi
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, TH
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOS
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR AN
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGE
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AN
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TOR
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE O
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_HH__

#include <unisim/component/cxx/processor/arm/isa_arm64.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/inlining/inlining.hh>
#include <unisim/util/debug/simple_register_registry.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/register.hh>
#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <map>
#include <set>
#include <stdexcept>
#include <iosfwd>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace vmsav8 {
  
  template <typename T>
  struct VectorTypeInfo
  {
    enum { bytecount = sizeof (T) };
    static void ToBytes( uint8_t* dst, T src )
    {
      for (unsigned idx = 0; idx < sizeof (T); ++idx)
        { dst[idx] = src & 0xff; src >>= 8; }
    }
    static void FromBytes( T& dst, uint8_t const* src )
    {
      T tmp = 0;
      for (unsigned idx = sizeof (T); idx-- > 0;)
        { tmp <<= 8; tmp |= unsigned( src[idx] ); }
      dst = tmp;
    }
  };

  template <> struct VectorTypeInfo<float>
  {
    enum bytecount_t { bytecount = 4 };
    static void ToBytes( uint8_t* dst, float const& src ) { VectorTypeInfo<uint32_t>::ToBytes( dst, reinterpret_cast<uint32_t const&>( src ) ); }
    static void FromBytes( float& dst, uint8_t const* src ) { VectorTypeInfo<uint32_t>::FromBytes( reinterpret_cast<uint32_t&>( dst ), src ); }
  };

  template <> struct VectorTypeInfo<double>
  {
    enum bytecount_t { bytecount = 8 };
    static void ToBytes( uint8_t* dst, double const& src ) { VectorTypeInfo<uint64_t>::ToBytes( dst, reinterpret_cast<uint64_t const&>( src ) ); }
    static void FromBytes( double& dst, uint8_t const* src ) { VectorTypeInfo<uint64_t>::FromBytes( reinterpret_cast<uint64_t&>( dst ), src ); }
  };

/** Armv8 cpu
 *
 * Defines and implements architectural state of armv8 processors.
 */

template <typename CONFIG>
struct CPU
  : public virtual unisim::kernel::Object
  , public unisim::kernel::Client<unisim::service::interfaces::DebugYielding>
  , public unisim::kernel::Client<unisim::service::interfaces::TrapReporting>
  , public unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<uint64_t> >
  , public unisim::kernel::Client<unisim::service::interfaces::Memory<uint64_t> >
  , public unisim::kernel::Client<unisim::service::interfaces::LinuxOS>
  , public unisim::kernel::Client<unisim::service::interfaces::MemoryAccessReporting<uint64_t> >
  , public unisim::kernel::Service<unisim::service::interfaces::Registers>
  , public unisim::kernel::Service<unisim::service::interfaces::Memory<uint64_t> >
  , public unisim::kernel::Service<unisim::service::interfaces::Disassembly<uint64_t> >
  , public unisim::kernel::Service<unisim::service::interfaces::MemoryAccessReportingControl>
  , public unisim::kernel::Service<unisim::service::interfaces::MemoryInjection<uint64_t> >
{
  typedef CONFIG Config;
  // typedef simfloat::FP FP;
  // typedef FP::F64  F64;
  // typedef FP::F32  F32;
  typedef uint8_t  U8;
  typedef uint16_t U16;
  typedef uint32_t U32;
  typedef uint64_t U64;
  typedef int8_t   S8;
  typedef int16_t  S16;
  typedef int32_t  S32;
  typedef int64_t  S64;
  typedef bool     BOOL;
  
  /**********************************************************************
   ***                   Constructors / Destructors                   ***
   **********************************************************************/
    
  CPU(const char* name, Object* parent);
  ~CPU();
  
  /**********************************************************************
   ***                  Service methods and members                   ***
   **********************************************************************/
    
  //=====================================================================
  //=                    Registers interface methods                    =
  //=====================================================================

  unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;
  virtual unisim::service::interfaces::Register* GetRegister( const char* name );
  virtual void ScanRegisters( unisim::service::interfaces::RegisterScanner& scanner );

		
  //=====================================================================
  //=              Memory interface methods (non intrusive)             =
  //=====================================================================

  unisim::kernel::ServiceExport<unisim::service::interfaces::Memory<uint64_t> > memory_export;
  virtual bool ReadMemory( uint64_t addr, void* buffer, uint32_t size );
  virtual bool WriteMemory( uint64_t addr, void const* buffer, uint32_t size );

  //=====================================================================
  //=                   Disassembly interface methods                   =
  //=====================================================================

  unisim::kernel::ServiceExport<unisim::service::interfaces::Disassembly<uint64_t> > disasm_export;
  virtual std::string Disasm( uint64_t addr, uint64_t& next_addr );
  
  //=====================================================================
  //=             Memory access reporting interface methods             =
  //=====================================================================

  unisim::kernel::ServiceExport<unisim::service::interfaces::MemoryAccessReportingControl> memory_access_reporting_control_export;
  virtual void RequiresMemoryAccessReporting( unisim::service::interfaces::MemoryAccessReportingType type, bool report );
  
  //=====================================================================
  //=                Memory injection interface methods                 =
  //=====================================================================

  unisim::kernel::ServiceExport<unisim::service::interfaces::MemoryInjection<uint64_t> > memory_injection_export;
  virtual bool InjectReadMemory( uint64_t addr, void* buffer, uint32_t size );
  virtual bool InjectWriteMemory( uint64_t addr, void const* buffer, uint32_t size );

  //=====================================================================
  //=                          Service imports                          =
  //=====================================================================

  unisim::kernel::ServiceImport<unisim::service::interfaces::DebugYielding>                    debug_yielding_import;
  unisim::kernel::ServiceImport<unisim::service::interfaces::TrapReporting>                    trap_reporting_import;
  unisim::kernel::ServiceImport<unisim::service::interfaces::SymbolTableLookup<uint64_t> >     symbol_table_lookup_import;
  unisim::kernel::ServiceImport<unisim::service::interfaces::Memory<uint64_t> >                memory_import;
  unisim::kernel::ServiceImport<unisim::service::interfaces::MemoryAccessReporting<uint64_t> > memory_access_reporting_import;
  unisim::kernel::ServiceImport<unisim::service::interfaces::LinuxOS>                          linux_os_import;

  /**********************************************************************
   ***                Functional methods and members                  ***
   **********************************************************************/
    
  void StepInstruction();

  void UndefinedInstruction( isa::arm64::Operation<CPU>* insn );
  
  bool Cond( bool cond ) { return cond; }
  
  //=====================================================================
  //=             General Purpose Registers access methods              =
  //=====================================================================

  /** Get the value contained by a General-purpose or Stack Register.
   *
   * @param id the register index
   * @return the value contained by the register
   */
  uint64_t GetGSR(unsigned id) const
  {
    return gpr[id];
  }

  /** Get the value contained by a General-purpose or Zero Register.
   *
   * @param id the register index
   * @return the value contained by the register
   */
  uint64_t GetGZR(unsigned id) const
  {
    return (id != 31) ? gpr[id] : 0;
  }

  /** Set the value of a General-purpose or Stack Register
   *
   * @param id the register index
   * @param val the value to set
   */
  void SetGSR(unsigned id, uint64_t val)
  {
    gpr[id] = val;
  }
	
  /** Set the value of a General-purpose or Zero Register
   *
   * @param id the register index
   * @param val the value to set
   */
  void SetGZR(unsigned id, uint64_t val)
  {
    if (id != 31) gpr[id] = val;
  }
  
  //====================================================================
  //=                 Special  Registers access methods                 
  //====================================================================

  U8  GetVU8 ( unsigned reg, unsigned sub ) { return VectorStorage<U8> (reg)[sub]; }
  U16 GetVU16( unsigned reg, unsigned sub ) { return VectorStorage<U16>(reg)[sub]; }
  U32 GetVU32( unsigned reg, unsigned sub ) { return VectorStorage<U32>(reg)[sub]; }
  U64 GetVU64( unsigned reg, unsigned sub ) { return VectorStorage<U64>(reg)[sub]; }
  S8  GetVS8 ( unsigned reg, unsigned sub ) { return VectorStorage<S8> (reg)[sub]; }
  S16 GetVS16( unsigned reg, unsigned sub ) { return VectorStorage<S16>(reg)[sub]; }
  S32 GetVS32( unsigned reg, unsigned sub ) { return VectorStorage<S32>(reg)[sub]; }
  S64 GetVS64( unsigned reg, unsigned sub ) { return VectorStorage<S64>(reg)[sub]; }
  
  void SetVU8 ( unsigned reg, unsigned sub, U8  value ) { VectorStorage<U8> (reg)[sub] = value; }
  void SetVU16( unsigned reg, unsigned sub, U16 value ) { VectorStorage<U16>(reg)[sub] = value; }
  void SetVU32( unsigned reg, unsigned sub, U32 value ) { VectorStorage<U32>(reg)[sub] = value; }
  void SetVU64( unsigned reg, unsigned sub, U64 value ) { VectorStorage<U64>(reg)[sub] = value; }
  void SetVS8 ( unsigned reg, unsigned sub, S8  value ) { VectorStorage<S8> (reg)[sub] = value; }
  void SetVS16( unsigned reg, unsigned sub, S16 value ) { VectorStorage<S16>(reg)[sub] = value; }
  void SetVS32( unsigned reg, unsigned sub, S32 value ) { VectorStorage<S32>(reg)[sub] = value; }
  void SetVS64( unsigned reg, unsigned sub, S64 value ) { VectorStorage<S64>(reg)[sub] = value; }
  
  void SetVU8 ( unsigned reg, U8 value )  { VectorZeroedStorage<U8> (reg)[0] = value; }
  void SetVU16( unsigned reg, U16 value ) { VectorZeroedStorage<U16>(reg)[0] = value; }
  void SetVU32( unsigned reg, U32 value ) { VectorZeroedStorage<U32>(reg)[0] = value; }
  void SetVU64( unsigned reg, U64 value ) { VectorZeroedStorage<U64>(reg)[0] = value; }
  void SetVS8 ( unsigned reg, S8 value )  { VectorZeroedStorage<S8> (reg)[0] = value; }
  void SetVS16( unsigned reg, S16 value ) { VectorZeroedStorage<S16>(reg)[0] = value; }
  void SetVS32( unsigned reg, S32 value ) { VectorZeroedStorage<S32>(reg)[0] = value; }
  void SetVS64( unsigned reg, S64 value ) { VectorZeroedStorage<S64>(reg)[0] = value; }
  
  void ClearHighV( unsigned reg, unsigned bytes ) { for (unsigned idx = bytes; idx < VUnion::BYTECOUNT; idx+=1 ) vector_data[reg][idx] = 0; }

  //=====================================================================
  //=              Special/System Registers access methods              =
  //=====================================================================

  /** Set the value of PSTATE.<N,Z,C,V>
   *
   *  @param n the value to be assignd to PSTATE.<N>
   *  @param z the value to be assignd to PSTATE.<Z>
   *  @param c the value to be assignd to PSTATE.<C>
   *  @param v the value to be assignd to PSTATE.<V>
   */
  void SetNZCV( uint32_t n, uint32_t z, uint32_t c, uint32_t v )
  {
    nzcv = (n << 3) | (z << 2) | (c << 1) | (v << 0);
  }
  
  uint8_t GetNZCV() const { return nzcv; }
  uint8_t GetCarry() const { return (nzcv >> 1) & 1; }
  
  /** Get the current Program Counter */
  uint64_t GetPC() { return current_insn_addr; }
  
  /** Get the next Program Counter */
  uint64_t GetNPC() { return next_insn_addr; }
  
  void        CheckSystemAccess( uint8_t op1 );
  uint64_t    ReadSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2 );
  void        WriteSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint64_t value );
  void        DescribeSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, std::ostream& sink );
  void        NameSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, std::ostream& sink );
  
  //=====================================================================
  //=                      Control Transfer methods                     =
  //=====================================================================
  
  /** Set the next Program Counter */
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET };
  void BranchTo( uint64_t addr, branch_type_t branch_type ) { next_insn_addr = addr; }
  bool Test( bool cond ) { return cond; }
  void CallSupervisor( uint32_t imm );
  
  //=====================================================================
  //=                       Memory access methods                       =
  //=====================================================================
  
  template <typename T>
  T
  MemReadT(uint64_t addr)
  {
    unsigned const size = sizeof (T);
    uint8_t buffer[size];
    MemRead( &buffer[0], addr, size );
    T res(0);
    for (unsigned idx = size; --idx < size; )
      res = (res << 8) | T(buffer[idx]);
    return res;
  }

  uint64_t MemRead64(uint64_t addr) { return MemReadT<uint64_t>(addr); }
  uint32_t MemRead32(uint64_t addr) { return MemReadT<uint32_t>(addr); }
  uint16_t MemRead16(uint64_t addr) { return MemReadT<uint16_t>(addr); }
  uint8_t  MemRead8 (uint64_t addr) { return MemReadT<uint8_t> (addr); }
  
  void MemRead( uint8_t* buffer, uint64_t addr, unsigned size );
  
  template <typename T>
  void
  MemWriteT(uint64_t addr, T val)
  {
    unsigned const size = sizeof (T);
    uint8_t buffer[size];
    for (unsigned idx = 0; idx < size; ++idx)
      { buffer[idx] = val; val >>= 8; }
    MemWrite( addr, &buffer[0], size );
  }
  
  void MemWrite64(uint64_t addr, uint64_t val) { MemWriteT(addr, val); }
  void MemWrite32(uint64_t addr, uint32_t val) { MemWriteT(addr, val); }
  void MemWrite16(uint64_t addr, uint16_t val) { MemWriteT(addr, val); }
  void MemWrite8 (uint64_t addr, uint8_t  val) { MemWriteT(addr, val); }
  
  void MemWrite( uint64_t addr, uint8_t const* buffer, unsigned size );

  void     SetExclusiveMonitors( uint64_t addr, unsigned size ) { /*TODO: MP support*/ }
  bool     ExclusiveMonitorsPass( uint64_t addr, unsigned size ) { /*TODO: MP support*/ return true; }
  void     ClearExclusiveLocal() {}
  
protected:
  
  /**********************************************************************
   ***                   Non functional variables                     ***
   **********************************************************************/
  
  //=====================================================================
  //=                              Logger                               =
  //=====================================================================
  
  /** Unisim logging services. */
  unisim::kernel::logger::Logger logger;
  
  /** Verbosity of the CPU implementation */
  bool verbose;

  //=====================================================================
  //=                  Registers interface structures                   =
  //=====================================================================
  
  /** The registers interface for debugging purpose */
  typedef unisim::util::debug::SimpleRegisterRegistry RegistersRegistry;
  RegistersRegistry registers_registry;
  
  typedef std::set<unisim::kernel::VariableBase*> VariableRegisterPool;
  VariableRegisterPool variable_register_pool;
  
  /**********************************************************************
   ***                       Architectural state                      ***
   **********************************************************************/
  
  uint64_t gpr[32];
  uint32_t nzcv;
  uint64_t current_insn_addr, next_insn_addr;

  uint64_t TPIDRURW; //< User Read/Write Thread ID Register
  
  struct SysReg
  {
    virtual            ~SysReg() {}
    virtual void        Write( CPU& cpu, uint64_t value ) const {
      cpu.logger << unisim::kernel::logger::DebugWarning << "Writing " << Describe() << unisim::kernel::logger::EndDebugWarning;
      throw 0; // cpu.UnpredictableInsnBehaviour();
    }
    virtual uint64_t    Read( CPU& cpu ) const {
      cpu.logger << unisim::kernel::logger::DebugWarning << "Reading " << Describe() << unisim::kernel::logger::EndDebugWarning;
      throw 0; // cpu.UnpredictableInsnBehaviour();
      return 0;
    }
    virtual char const* Describe() const = 0;
    virtual char const* Name() const = 0;
  };
  
  virtual SysReg const&  GetSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2 );
  virtual void     ResetSystemRegisters();
  
  static unsigned const VECTORCOUNT = 32;
  
  struct VUnion
  {
    static unsigned const BYTECOUNT = 16;
    
    typedef void (*arrangement_t)( uint8_t* storage );
    arrangement_t arrangement;
    
    template <typename T>
    static unsigned ItemCount() { return BYTECOUNT / VectorTypeInfo<T>::bytecount; }
    
    template <typename T>
    static void ToBytes( uint8_t* storage )
    {
      T const* vec = reinterpret_cast<T const*>( storage );
      
      for (unsigned idx = 0, stop = ItemCount<T>(); idx < stop; ++idx) {
        T val = vec[idx];
        VectorTypeInfo<T>::ToBytes( &storage[idx*VectorTypeInfo<T>::bytecount], val );
      }
    }

    template <typename T>
    T*
    GetStorage( uint8_t* storage )
    {
      T* res = reinterpret_cast<T*>( storage );

      arrangement_t current = &ToBytes<T>;
      if (arrangement != current) {
        arrangement( storage );
        
        for (int idx = ItemCount<T>(); --idx >= 0;) {
          T val;
          VectorTypeInfo<T>::FromBytes( val, &storage[idx*VectorTypeInfo<T>::bytecount] );
          res[idx] = val;
        }
        arrangement = current;
      }
      
      return res;
    }
    
    template <typename T>
    T*
    GetZeroedStorage( uint8_t* storage )
    {
      T* res = reinterpret_cast<T*>( storage );
      arrangement = &ToBytes<T>;
      for (int idx = ItemCount<T>(); --idx >= 0;)
        res[idx] = T();
      return res;
    }
      
    VUnion() : arrangement( &ToBytes<uint8_t> ) {}
  } vector_view[VECTORCOUNT];
    
  uint8_t vector_data[VECTORCOUNT][VUnion::BYTECOUNT];
  
  template <typename T> T* VectorStorage( unsigned reg ) { return vector_view[reg].template GetStorage<T>( &vector_data[reg][0] ); }
  template <typename T> T* VectorZeroedStorage( unsigned reg ) { return vector_view[reg].template GetZeroedStorage<T>( &vector_data[reg][0] ); }
  
private:
  virtual void Sync() = 0;
  
  //=====================================================================
  //=                          Memory Accesses                          =
  //=====================================================================
  static unsigned const IPB_LINE_SIZE = 32; //< IPB: Instruction prefetch buffer
  uint8_t ipb_bytes[IPB_LINE_SIZE];  //< The instruction prefetch buffer
  uint32_t ipb_base_address;         //< base address of IPB content (cache line size aligned if valid)
  
  bool RefillInsnPrefetchBuffer( uint64_t base_address );
  void ReadInsn( uint64_t address, isa::arm64::CodeType& insn );
  
  /** Decoder for the ARM32 instruction set. */
  unisim::component::cxx::processor::arm::isa::arm64::Decoder<CPU> decoder;
  
  // Intrusive memory accesses
  virtual bool  PhysicalReadMemory( uint64_t addr, uint8_t*       buffer, unsigned size ) = 0;
  virtual bool PhysicalWriteMemory( uint64_t addr, uint8_t const* buffer, unsigned size ) = 0;
  // Non-intrusive memory accesses
  virtual bool  ExternalReadMemory( uint64_t addr, uint8_t*       buffer, unsigned size ) = 0;
  virtual bool ExternalWriteMemory( uint64_t addr, uint8_t const* buffer, unsigned size ) = 0;

  bool requires_memory_access_reporting;      //< indicates if the memory accesses require to be reported
  bool requires_fetch_instruction_reporting;  //< indicates if the fetched instructions require to be reported
  bool requires_commit_instruction_reporting; //< indicates if the committed instructions require to be reported
  
  void ReportMemoryAccess( unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mtp,
                           uint64_t addr, uint32_t size )
  {
    if (requires_memory_access_reporting and memory_access_reporting_import)
      memory_access_reporting_import->ReportMemoryAccess(mat, mtp, addr, size);
  }
};

} // end of namespace vmsav8
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_HH__
