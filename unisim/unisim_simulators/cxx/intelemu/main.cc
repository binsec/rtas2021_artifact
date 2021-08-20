/*
 *  Copyright (c) 2009-2020,
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

#include <unisim/component/cxx/processor/intel/arch.hh>
#include <unisim/component/cxx/processor/intel/isa/intel.hh>
#include <unisim/util/debug/simple_register.hh>
#include <linuxsystem.hh>
#include <iostream>

struct Arch
  : public unisim::component::cxx::processor::intel::Arch
  , public unisim::service::interfaces::MemoryInjection<uint32_t>
  , public unisim::service::interfaces::Memory<uint32_t>
  , public unisim::service::interfaces::Registers

{
  typedef unisim::component::cxx::processor::intel::Arch::f64_t f64_t;
  typedef unisim::component::cxx::processor::intel::Arch BaseArch;
  typedef unisim::component::cxx::processor::intel::Operation<BaseArch> Operation;
  
  Arch()
    : unisim::component::cxx::processor::intel::Arch()
    , unisim::service::interfaces::MemoryInjection<uint32_t>()
    , unisim::service::interfaces::Memory<uint32_t>()
    , unisim::service::interfaces::Registers()
    , linux_os(0)
  {
    for (int idx = 0; idx < 8; ++idx) {
      std::ostringstream regname;
      regname << unisim::component::cxx::processor::intel::DisasmGd(idx);
      regmap[regname.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(regname.str(), &m_regs[idx]);
    }
    regmap["%eip"] = new unisim::util::debug::SimpleRegister<uint32_t>("%eip", &m_EIP);
    
    struct SegmentRegister : public unisim::service::interfaces::Register
    {
      SegmentRegister( Arch& _arch, std::string _name, unsigned _idx ) : arch(_arch), name(_name), idx(_idx) {}
      virtual const char *GetName() const { return "pc"; }
      virtual void GetValue( void* buffer ) const { *((uint16_t*)buffer) = arch.segregread( idx ); }
      virtual void SetValue( void const* buffer ) { arch.segregwrite( idx, *((uint16_t*)buffer) ); }
      virtual int  GetSize() const { return 2; }
      virtual void Clear() {} // Clear is meaningless for segment registers
      Arch&        arch;
      std::string  name;
      unsigned     idx;
    };

    for (int idx = 0; idx < 6; ++idx) {
      std::ostringstream regname;
      regname << unisim::component::cxx::processor::intel::DisasmS(idx);
      regmap[regname.str()] = new SegmentRegister(*this, regname.str(), idx);
    }
    
    for (int idx = 0; idx < 4; ++idx) {
      std::ostringstream regname;
      regname << "@gdt[" << idx<< "].base";
      regmap[regname.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(regname.str(), &m_gdt_bases[idx]);
    }
    
    struct X87Register : public unisim::service::interfaces::Register
    {
      X87Register( std::string _name, Arch& _arch, unsigned _idx ) : name(_name), arch(_arch), idx(_idx) {}
      char const* GetName() const { return name.c_str(); }
      void GetValue(void *buffer) const { *((f64_t*)buffer) = arch.fread( idx ); }
      void SetValue(const void *buffer) { arch.fwrite( idx, *((f64_t*)buffer) ); }
      int GetSize() const { return 8; }
      std::string name;
      Arch& arch;
      unsigned idx;
    };
    for (int idx = 0; idx < 8; ++idx) {
      std::ostringstream regname;
      regname << unisim::component::cxx::processor::intel::DisasmFPR(idx);
      regmap[regname.str()] = new X87Register( regname.str(), *this, idx );
    }
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
  
  using unisim::component::cxx::processor::intel::Arch::m_mem;
  // unisim::service::interfaces::Memory<uint32_t>
  void ResetMemory() {}
  bool ReadMemory(uint32_t addr, void* buffer, uint32_t size ) { m_mem.read( (uint8_t*)buffer, addr, size ); return true; }
  bool WriteMemory(uint32_t addr, void const* buffer, uint32_t size) { m_mem.write( addr, (uint8_t*)buffer, size ); return true; }
  // unisim::service::interfaces::Registers
  unisim::service::interfaces::Register* GetRegister(char const* name)
  {
    auto reg = regmap.find( name );
    return (reg == regmap.end()) ? 0 : reg->second;
  }
  void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
  {
    // General purpose registers
    scanner.Append( GetRegister( "%eax" ) );
    scanner.Append( GetRegister( "%ecx" ) );
    scanner.Append( GetRegister( "%edx" ) );
    scanner.Append( GetRegister( "%ebx" ) );
    scanner.Append( GetRegister( "%esp" ) );
    scanner.Append( GetRegister( "%ebp" ) );
    scanner.Append( GetRegister( "%esi" ) );
    scanner.Append( GetRegister( "%edi" ) );
    // Program counter
    scanner.Append( GetRegister( "%eip" ) );
    // Segments
    scanner.Append( GetRegister( "%es" ) );
    scanner.Append( GetRegister( "%cs" ) );
    scanner.Append( GetRegister( "%ss" ) );
    scanner.Append( GetRegister( "%ds" ) );
    scanner.Append( GetRegister( "%fs" ) );
    scanner.Append( GetRegister( "%gs" ) );
    // FP registers
    scanner.Append( GetRegister( "%st" ) );
    scanner.Append( GetRegister( "%st(1)" ) );
    scanner.Append( GetRegister( "%st(2)" ) );
    scanner.Append( GetRegister( "%st(3)" ) );
    scanner.Append( GetRegister( "%st(4)" ) );
    scanner.Append( GetRegister( "%st(5)" ) );
    scanner.Append( GetRegister( "%st(6)" ) );
    scanner.Append( GetRegister( "%st(7)" ) );
  }
  // unisim::service::interfaces::MemoryInjection<ADDRESS>
  bool InjectReadMemory(uint32_t addr, void *buffer, uint32_t size) { m_mem.read( (uint8_t*)buffer, addr, size ); return true; }
  bool InjectWriteMemory(uint32_t addr, void const* buffer, uint32_t size) { m_mem.write( addr, (uint8_t*)buffer, size ); return true; }
  // Implementation of ExecuteSystemCall
  
  virtual void ExecuteSystemCall( unsigned id )
  {
    if (not linux_os)
      { throw std::logic_error( "No linux OS emulation connected" ); }
    linux_os->ExecuteSystemCall( id );
  }
  
};


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

  std::vector<std::string> envs;
  envs.push_back( "LANG=C" );
  
  Arch cpu;
  LinuxOS linux32( std::cerr, &cpu, &cpu, &cpu );
  cpu.SetLinuxOS( &linux32 );
  
  linux32.Setup( simargs, envs );
  
  cpu.m_disasm = false;
  
  // Loading image
  std::cerr << "*** Loading elf image: " << simargs[0] << " ***" << std::endl;
  
  std::cerr << "\n*** Run ***" << std::endl;
  
  while (not linux32.exited)
    {
      Arch::Operation* op = cpu.fetch();
      // op->disasm( std::cerr );
      // std::cerr << std::endl;
      asm volatile ("operation_execute:");
      op->execute( cpu );
      //{ uint64_t chksum = 0; for (unsigned idx = 0; idx < 8; ++idx) chksum ^= cpu.regread32( idx ); std::cerr << '[' << std::hex << chksum << std::dec << ']'; }
      
      // if ((cpu.m_instcount % 0x1000000) == 0)
      //   { std::cerr << "Executed instructions: " << std::dec << cpu.m_instcount << " (" << std::hex << op->address << std::dec << ")"<< std::endl; }
    }
  
  std::cerr << "Program exited with status:" << linux32.app_ret_status << std::endl;
  
  
  return 0;
}
