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

#ifndef __E5500FPV_ARCH_HH__
#define __E5500FPV_ARCH_HH__

#include <core.hh>
#include <top_ppc64.hh>
#include <unisim/component/cxx/processor/powerpc/disasm.hh>
#include <unisim/component/cxx/memory/sparse/memory.hh>
#include <unisim/util/reg/core/register.hh>
#include <unisim/util/reg/core/register.tcc>
#include <unisim/util/random/random.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/kernel/kernel.hh>

struct MisInsn
{
  MisInsn( uint64_t _addr, uint32_t _code )
    : addr(_addr), code(_code)
  {}

  uint64_t addr;
  uint32_t code;
};
  
struct Arch
  : Core
  , public unisim::kernel::Service<unisim::service::interfaces::Registers>
  , public unisim::kernel::Service<unisim::service::interfaces::MemoryInjection<uint64_t> >
  , public unisim::kernel::Service<unisim::service::interfaces::Memory<uint64_t> >
  , public unisim::kernel::Service<unisim::service::interfaces::Disassembly<uint64_t> >
  , public unisim::kernel::Service<unisim::service::interfaces::MemoryAccessReportingControl>
  , public unisim::kernel::Client<unisim::service::interfaces::DebugYielding>
  , public unisim::kernel::Client<unisim::service::interfaces::TrapReporting>
  , public unisim::kernel::Client<unisim::service::interfaces::MemoryAccessReporting<uint64_t> >
{
  typedef ::MSR MSR;
  typedef unisim::component::cxx::processor::powerpc::ppc64::Decoder                             Decoder;
  typedef unisim::component::cxx::processor::powerpc::ppc64::Operation                           Operation;
  typedef unisim::service::interfaces::LinuxOS                                                   LinuxOS;
  
  struct ClearMemSet { void operator() ( uint8_t* base, uintptr_t size ) const { for (uintptr_t idx = 0; idx < size; ++idx) base[idx] = 0; } };
  typedef typename unisim::component::cxx::memory::sparse::Memory<uint64_t,12,12,ClearMemSet>   Memory;

  struct SystemCallInterrupt
  {
    struct SystemCall { typedef SystemCallInterrupt Interrupt; };
    SystemCallInterrupt( Core& core ) { static_cast<Arch&>(core).SystemCall(); }
    void SetELEV(unsigned _elev) { if (_elev != 0) throw *this; /* Hypervisor interrupt ? */ }
  };

  Arch();
  ~Arch();
  
  void SetLinuxOS( LinuxOS* _linux_os ) { linux_os = _linux_os; }
  LinuxOS* GetLinuxOS() { return linux_os; }
  
  // unisim::service::interfaces::Registers
  virtual unisim::service::interfaces::Register* GetRegister( const char* name );
  virtual void ScanRegisters( unisim::service::interfaces::RegisterScanner& scanner );
  unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;

  // unisim::service::interfaces::MemoryInjection<uint64_t>
  virtual bool InjectReadMemory(uint64_t addr, void* buffer, unsigned size);
  virtual bool InjectWriteMemory(uint64_t addr, void const* buffer, unsigned size);
  unisim::kernel::ServiceExport<unisim::service::interfaces::MemoryInjection<uint64_t> > memory_injection_export;

  // unisim::service::interfaces::Memory<uint64_t>
  virtual void ResetMemory() {}
  virtual bool ReadMemory(uint64_t addr, void* buffer, unsigned size );
  virtual bool WriteMemory(uint64_t addr, void const* buffer, unsigned size);
  unisim::kernel::ServiceExport<unisim::service::interfaces::Memory<uint64_t> > memory_export;
  
  // unisim::service::interfaces::Disassembly<uint64_t>
  virtual std::string Disasm(ADDRESS addr, ADDRESS& next_addr);
  unisim::kernel::ServiceExport<unisim::service::interfaces::Disassembly<uint64_t> > disasm_export;
  
  // unisim::service::interfaces::MemoryAccessReportingControl
  virtual void RequiresMemoryAccessReporting(unisim::service::interfaces::MemoryAccessReportingType type, bool report);
  unisim::kernel::ServiceExport<unisim::service::interfaces::MemoryAccessReportingControl> memory_access_reporting_control_export;
  bool requires_memory_access_reporting;      //< indicates if the memory accesses require to be reported
  bool requires_fetch_instruction_reporting;  //< indicates if the fetched instructions require to be reported
  bool requires_commit_instruction_reporting; //< indicates if the committed instructions require to be reported
  
  // unisim::service::interfaces::DebugYielding
  unisim::kernel::ServiceImport<unisim::service::interfaces::DebugYielding> debug_yielding_import;

  // unisim::service::interfaces::TrapReporting
  unisim::kernel::ServiceImport<unisim::service::interfaces::TrapReporting> trap_reporting_import;

  // unisim::service::interfaces::MemoryAccessReporting<uint64_t>
  unisim::kernel::ServiceImport<unisim::service::interfaces::MemoryAccessReporting<uint64_t> > memory_access_reporting_import;
  
  void SystemCall();

  bool MoveFromSPR( unsigned  id, U64& value );
  bool MoveToSPR( unsigned  id, U64 value ) { return false; }
  
  template <typename T>
  void
  IntStore( U64 addr, T value )
  {
    int const size = sizeof (T);
    uint8_t buf[size];
    for (int idx = size; --idx>=0;)
      {
        buf[idx] = value;
        value >>= 8;
      }
    memory.write( addr, &buf[0], size );
  }

  template <typename T>
  T
  IntLoad( U64 addr )
  {
    int const size = sizeof (T);
    uint8_t buf[size];
    memory.read( &buf[0], addr, size );
    T value = 0;
    for (int idx = 0; idx < size; ++idx)
      value = (value << 8) | T(buf[idx]);
    return value;
  }

  bool Int8Store (unsigned id, U64 addr) { IntStore( addr, U8 (gprs[id]) ); return true; }
  bool Int16Store(unsigned id, U64 addr) { IntStore( addr, U16(gprs[id]) ); return true; }
  bool Int32Store(unsigned id, U64 addr) { IntStore( addr, U32(gprs[id]) ); return true; }
  bool Int64Store(unsigned id, U64 addr) { IntStore( addr, U64(gprs[id]) ); return true; }

  bool Int8Load (unsigned id, U64 addr) { gprs[id] = IntLoad<U8> ( addr ); return true; }
  bool Int16Load(unsigned id, U64 addr) { gprs[id] = IntLoad<U16>( addr ); return true; }
  bool Int32Load(unsigned id, U64 addr) { gprs[id] = IntLoad<U32>( addr ); return true; }
  bool Int64Load(unsigned id, U64 addr) { gprs[id] = IntLoad<U64>( addr ); return true; }

  bool SInt8Load (unsigned id, U64 addr) { gprs[id] = IntLoad<S8> ( addr ); return true; }
  bool SInt16Load(unsigned id, U64 addr) { gprs[id] = IntLoad<S16>( addr ); return true; }
  bool SInt32Load(unsigned id, U64 addr) { gprs[id] = IntLoad<S32>( addr ); return true; }
  bool SInt64Load(unsigned id, U64 addr) { gprs[id] = IntLoad<S64>( addr ); return true; }

  bool Lbarx(unsigned id, U64 addr) { return Int8Load ( id, addr ); }
  bool Lharx(unsigned id, U64 addr) { return Int16Load( id, addr ); }
  bool Lwarx(unsigned id, U64 addr) { return Int32Load( id, addr ); }
  bool Ldarx(unsigned id, U64 addr) { return Int64Load( id, addr ); }
  
  bool Stbcx(unsigned id, U64 addr) { cr.Set<CR::CR0>(0b0010 | xer.Get<XER::SO>()); return Int8Store ( id, addr ); }
  bool Sthcx(unsigned id, U64 addr) { cr.Set<CR::CR0>(0b0010 | xer.Get<XER::SO>()); return Int16Store( id, addr ); }
  bool Stwcx(unsigned id, U64 addr) { cr.Set<CR::CR0>(0b0010 | xer.Get<XER::SO>()); return Int32Store( id, addr ); }
  bool Stdcx(unsigned id, U64 addr) { cr.Set<CR::CR0>(0b0010 | xer.Get<XER::SO>()); return Int64Store( id, addr ); }

  bool Fp64Load(unsigned id, U64 addr) { fprs[id].fromRawBitsAssign(IntLoad<U64>( addr )); return true; }
  bool Fp64Store(unsigned id, U64 addr) { IntStore( addr, U64(fprs[id].queryRawBits()) ); return true; }
  bool Fp32Load(unsigned id, U64 addr);
  bool Fp32Store(unsigned id, U64 addr);
  bool FpStoreLSW(unsigned id, U64 addr);
  
  void Isync() {}
  
  bool InstructionFetch(uint64_t addr, uint32_t& insn);
  
  Operation*   fetch();
  void         commit();

  Decoder         decoder;
  Memory          memory;
  LinuxOS*        linux_os;
  std::map<std::string,unisim::service::interfaces::Register*> regmap;
  uintptr_t       insn_count;
  U64             time_base;
};

typedef Arch CPU;
typedef Arch CONFIG;

#endif // __E5500FPV_ARCH_HH__
