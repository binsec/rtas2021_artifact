/*
 *  Copyright (c) 2019-2020,
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

#include <unisim/component/cxx/memory/sparse/memory.hh>
#include <unisim/component/cxx/processor/mips/isa/mipsel.tcc>
#include <unisim/component/cxx/processor/mips/isa/disasm.hh>
#include <unisim/util/debug/simple_register.hh>
#include <linuxsystem.hh>
#include <iostream>

struct Arch
  : public unisim::service::interfaces::MemoryInjection<uint32_t>
  , public unisim::service::interfaces::Memory<uint32_t>
  , public unisim::service::interfaces::Registers

{
  typedef unisim::component::cxx::processor::mips::isa::CodeType CodeType;
  typedef unisim::component::cxx::processor::mips::isa::Operation<Arch> Operation;
  typedef unisim::component::cxx::processor::mips::isa::Decoder<Arch> Decoder;

  typedef bool  BOOL;
  typedef uint8_t  U8;
  typedef uint16_t U16;
  typedef uint32_t U32;
  typedef uint64_t U64;
  typedef int8_t  S8;
  typedef int16_t S16;
  typedef int32_t S32;
  typedef int64_t S64;
  
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_DBG, B_RFE };
  
  Arch()
    : unisim::service::interfaces::MemoryInjection<uint32_t>()
    , unisim::service::interfaces::Memory<uint32_t>()
    , unisim::service::interfaces::Registers()
    , decoder(), linux_os(0), regmap()
    , mem(), gprs(), hi(), lo(), ulr(), insn_addrs()
    , m_disasm(false)
  {
    for (int idx = 1; idx < 32; ++idx)
      {
        unisim::service::interfaces::Register* reg = 0;
        {
          std::ostringstream buf;
          buf << unisim::component::cxx::processor::mips::isa::PrintGPR(idx);
          regmap[buf.str()] = reg = new unisim::util::debug::SimpleRegister<uint32_t>(buf.str(), &gprs[idx]);
        }
        {
          std::ostringstream buf;
          buf << unisim::component::cxx::processor::mips::isa::PrintR("$",idx);
          regmap[buf.str()] = reg;
        }
    }
    regmap["hwr_ulr"] = new unisim::util::debug::SimpleRegister<uint32_t>("hwr_ulr", &ulr);
    
    struct ProgramCounter : public unisim::service::interfaces::Register
    {
      ProgramCounter( Arch& _core ) : core(_core) {}
      virtual const char *GetName() const override { return "pc"; }
      virtual void GetValue( void* buffer ) const override { *((uint32_t*)buffer) = core.insn_addrs[0]; }
      virtual void SetValue( void const* buffer ) override
      {
        uint32_t address = *((uint32_t*)buffer);
        core.insn_addrs[1] = address;
        core.Branch(address+4, core.B_DBG);
      }
      virtual int  GetSize() const override { return 4; }
      virtual void Clear() { /* Clear is meaningless for PC */ }
      Arch&        core;
    };
    regmap["pc"] = new ProgramCounter( *this );

  }
  
  ~Arch()
  {
    for (auto& reg : regmap)
      if (reg.first.compare(reg.second->GetName()))
        reg.second = 0;
    for (auto& reg : regmap)
      delete reg.second;
  }
  
  void SetLinuxOS( unisim::service::interfaces::LinuxOS* _linux_os )
  {
    linux_os = _linux_os;
  }

  Decoder decoder;
  unisim::service::interfaces::LinuxOS* linux_os;
  std::map<std::string, unisim::service::interfaces::Register*> regmap;
  
  struct ClearMemSet {
    void operator() ( uint8_t* base, uintptr_t size ) const {
      for (uintptr_t idx = 0; idx < size; ++idx) base[idx] = 0;
    }
  };
  typedef typename unisim::component::cxx::memory::sparse::Memory<uint32_t,12,12,ClearMemSet> Memory;
  Memory                      mem;

  void     SetGPR(unsigned idx, U32 value) { if (idx) gprs[idx] = value; }
  U32      GetGPR(unsigned idx) { return gprs[idx]; }
  void     SetDivU(U32 rs, U32 rt) { lo = rs / rt; hi = rs % rt; }
  void     SetHI(unsigned ra, U32 value) { if (ra) throw "nope"; hi = value; }
  void     SetLO(unsigned ra, U32 value) { if (ra) throw "nope"; lo = value; }
  U32      GetHI(unsigned ra) { if (ra) throw "nope"; return hi; }
  U32      GetLO(unsigned ra) { if (ra) throw "nope"; return lo; }
  
  uint32_t                    gprs[32];
  uint32_t                    hi,lo;
  uint32_t                    ulr; /* User Local Register */

  struct HWR
  {
    virtual ~HWR() {}
    virtual uint32_t Get(Arch&) const = 0;
    virtual void Set(Arch&, uint32_t) const = 0;
  };
  HWR* FindHWR(unsigned idx)
  {
    switch (idx) {
    case 29: { /* User Local Register */
      static struct : public HWR {
        virtual uint32_t Get(Arch& core) const override { return core.ulr; }
        virtual void Set(Arch& core, uint32_t val) const override { core.ulr = val; }
      } _;
      return &_;
    }
    }
    std::cerr << "unknown HWR: " << idx << std::endl;
    return 0;
  }


  uint32_t GetHWR(unsigned idx) { if (HWR* hwr = FindHWR(idx)) return hwr->Get(*this); return 0; throw "nope"; };
  void SetHWR(unsigned idx, uint32_t val) { if (HWR* hwr = FindHWR(idx)) return hwr->Set(*this, val); throw "nope"; };
  
  bool    Test( bool cond ) { return cond; }
  U32     GetPC() { return insn_addrs[0]; }
  void    Branch(U32 target,   branch_type_t) { insn_addrs[2] = target; }
  void    CancelDelaySlot() { insn_addrs[1] = insn_addrs[2]; insn_addrs[2] += 4; }
  uint32_t                    insn_addrs[3];
  
  // unisim::service::interfaces::Memory<uint32_t>
  void ResetMemory() {}
  bool ReadMemory(uint32_t addr, void* buffer, uint32_t size ) { mem.read( (uint8_t*)buffer, addr, size ); return true; }
  bool WriteMemory(uint32_t addr, void const* buffer, uint32_t size) { mem.write( addr, (uint8_t*)buffer, size ); return true; }
  // unisim::service::interfaces::Registers
  unisim::service::interfaces::Register* GetRegister(char const* name)
  {
    auto reg = regmap.find( name );
    return (reg == regmap.end()) ? 0 : reg->second;
  }
  void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
  {
    // General purpose registers
  }
  // unisim::service::interfaces::MemoryInjection<ADDRESS>
  bool InjectReadMemory(uint32_t addr, void *buffer, uint32_t size) { mem.read( (uint8_t*)buffer, addr, size ); return true; }
  bool InjectWriteMemory(uint32_t addr, void const* buffer, uint32_t size) { mem.write( addr, (uint8_t*)buffer, size ); return true; }
  
  bool IsBigEndian() { return false; }

  template <typename U>
  U
  MemRead( U32 addr )
  {
    uint8_t buffer[sizeof (U)];
    mem.read( &buffer[0], addr, sizeof buffer );
    U res = 0;
    for (int idx = sizeof buffer; --idx >= 0;)
      res = (res << 8) | buffer[idx];
    return res;
  }

  template <typename U>
  void
  MemWrite( U32 addr, U value )
  {
    uint8_t buffer[sizeof (U)];
    for (unsigned idx = 0; idx < sizeof buffer; ++idx)
      { buffer[idx] = value; value >>= 8; }
    mem.write( addr, &buffer[0], sizeof buffer );
  }

  void AtomicBegin(uint32_t addr) { }
  bool AtomicUpdate(uint32_t addr) { return true; }

  CodeType ReadInsn(uint32_t addr)
  {
    uint8_t buffer[4];
    mem.read( (uint8_t*)&buffer[0], addr, 4 );
    return (buffer[0] << 0) | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
  }

  void SysCall(unsigned id)
  {
    if (not linux_os)
      { throw std::logic_error( "No linux OS emulation connected" ); }
    linux_os->ExecuteSystemCall( id );
  }

  Operation*
  StepInstruction()
  {
    /* Start new instruction */
    uint32_t insn_addr = insn_addrs[0] = insn_addrs[1];
    insn_addrs[1] = insn_addrs[2];
    insn_addrs[2] += 4;
    
    /* Fetch instruction word from memory */
    CodeType insn = ReadInsn(insn_addr);

    /* Decode instruction */
    Operation* op = decoder.Decode(insn_addr, insn);

    /* Disassemble instruction */
    if (m_disasm)
      {
        op->disasm(std::cerr << "0x" << std::hex << insn_addr << ": ");
        std::cerr << '\n';
      }

    /* Execute instruction*/
    asm volatile ("operation_execute:");
    op->execute( *this );
    
    return op;
  }

  bool m_disasm;
};

int main(int argc, char *argv[])
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

  std::vector<std::string> envs;
  envs.push_back( "LANG=C" );
  
  Arch core;
  LinuxOS linux32( std::cerr, &core, &core, &core );
  core.SetLinuxOS( &linux32 );
  
  linux32.Setup( simargs, envs );
  
  core.m_disasm = false;
  
  // Loading image
  std::cerr << "*** Loading elf image: " << simargs[0] << " ***" << std::endl;
  
  std::cerr << "\n*** Run ***" << std::endl;
  
  while (not linux32.exited)
    {
      core.StepInstruction();
      //      Arch::Operation* op = core.fetch();
      // op->disasm( std::cerr );
      // std::cerr << std::endl;
      //      asm volatile ("operation_execute:");
      //      op->execute( core );
      //{ uint64_t chksum = 0; for (unsigned idx = 0; idx < 8; ++idx) chksum ^= core.regread32( idx ); std::cerr << '[' << std::hex << chksum << std::dec << ']'; }
      
      // if ((core.m_instcount % 0x1000000) == 0)
      //   { std::cerr << "Executed instructions: " << std::dec << core.m_instcount << " (" << std::hex << op->address << std::dec << ")"<< std::endl; }
    }
  
  std::cerr << "Program exited with status:" << linux32.app_ret_status << std::endl;
  
  
  return 0;
}
