/*
 *  Copyright (c) 2007,
 *  University of Perpignan (UPVD)
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
 * Authors: 
 *     David Parello (david.parello@univ-perp.fr)
 *
 */

#ifndef __UNISIM_COMPONENT_CLM_PROCESSOR_OOOSIM_CPU_EMULATOR__HH__
#define __UNISIM_COMPONENT_CLM_PROCESSOR_OOOSIM_CPU_EMULATOR__HH__

#include <unisim/component/clm/utility/common.hh>
#include <unisim/component/clm/utility/utility.hh>

#include <unisim/component/clm/processor/ooosim/iss_interface.hh>

#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>

#include <unisim/kernel/kernel.hh>

#include <unisim/component/cxx/processor/powerpc/config.hh>


#include <unisim/component/cxx/processor/powerpc/powerpc.hh>
#include <unisim/component/cxx/processor/powerpc/config.hh>
//#include <unisim/component/cxx/processor/powerpc/powerpc.tcc>
//#include <unisim/component/cxx/processor/powerpc/cpu.tcc>
//#include <unisim/component/cxx/processor/powerpc/fpu.tcc>

namespace unisim {
namespace component {
namespace clm {
namespace processor {
namespace ooosim {


using unisim::component::clm::memory::address_t;
  //new usings
  //  using unisim::component::cxx::processor::powerpc::MPC7447AConfig;


using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;


using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;

using unisim::component::clm::utility::hexa;


//using unisim::component::clm::debug::SVGmemreqInterface;

  /*
using full_system::utils::services::ServiceImport;
using full_system::utils::services::ServiceExport;
using full_system::utils::services::Client;
using full_system::utils::services::Service;
//using full_system::utils::services::Object;
using full_system::generic::memory::MemoryInterface;
  */

typedef ServiceImport< Memory<address_t> > MI_ServiceImport;
typedef ServiceExport< Memory<address_t> > MI_ServiceExport;
typedef Client< Memory<address_t> > MI_Client;
typedef Service< Memory<address_t> > MI_Service;

typedef Client< MemoryInjection<address_t> > MJI_Client;

  //  using full_system::processors::powerpc::physical_address_t;

  //  typedef CPU<MPC7447AConfig> CPU;

// CPU Emulator for CPU_SIM_STATE ...
//class CPUSim: public CPU<MPC7447AConfig>//, public MI_Client
class CPUSim: public CPU<CPU_CONFIG>//, public MI_Client
{
public:

  //  MI_ServiceImport syscall_MemImp;
  bool program_ended;
  uint64_t end_at_cycle;

  CPUSim(const char*cpu_name, Object *parent):
    Object(cpu_name,parent), 
    //    MI_Client(cpu_name, parent),
    //    CPU<MPC7447AConfig>(cpu_name, parent)
    CPU<CPU_CONFIG>(cpu_name, parent),
    //    syscall_MemImp("syscall_MemImp",this)
    program_ended(false),
    end_at_cycle(0)
  {}

  virtual ~CPUSim() {}

  void Stop(int ret)
  {
    cerr << Object::GetName() << ": Program exited with code " << ret << endl;
    //cerr << Object::GetName() << ": Simulation TERMINATE NOW ..." << endl;
    //terminate_now();
    program_ended = true;
    end_at_cycle = timestamp();
  }

  void BusRead(physical_address_t physical_addr, void *buffer, uint32_t size, WIMG wimg = CPU_CONFIG::WIMG_DEFAULT, bool rwitm = false) {}
  void BusWrite(physical_address_t physical_addr, const void *buffer, uint32_t size, WIMG wimg = CPU_CONFIG::WIMG_DEFAULT) {}
  void BusZeroBlock(physical_address_t physical_addr) {}
  void BusFlushBlock(physical_address_t physical_addr) {}

  //////////////////////////////
  // Others functions
  //////////////////////////////

  void dump_registers(ostream &os)
  { os << "PC=" << hexa(this->GetCIA()) << endl;
    for(int i = 0; i < 32; i++)
      { os << "r" << i;
      if (i < 10) os << " ";
      os << "=" << hexa(this->GetGPR(i));
      os << "  ";
      if ((i % 8) == 7) os << endl;
      }
    os << "CR= "   << hexa(this->GetCR());
    os << "  XER=" << hexa(this->GetXER());
    os << "  LR= " << hexa(this->GetLR());
    os << "  CTR=" << hexa(this->GetCTR());
    os << std::dec << endl;
  }

  void dump_registers_compare(ostream &os, CPU<CPU_CONFIG> *another_cpu)
  { os << "PC=" << hexa(this->GetCIA()) << endl;
    for(int i = 0; i < 32; i++)
    { os << "r" << i;
      if (i < 10) os << " ";
      os << "=" << hexa(this->GetGPR(i));
      if (this->GetGPR(i) != another_cpu->GetGPR(i)) os << "* ";
      else os << "  ";
      if ((i % 8) == 7) os << endl;
    }
    os << "CR= " << hexa(this->GetCR());
    if (this->GetCR() != another_cpu->GetCR()) os << "* ";
    os << "  XER=" << hexa(GetXER());
    if (this->GetXER() != another_cpu->GetXER()) os << "* ";
    os << "  LR= " << hexa(GetLR());
    if (this->GetLR() != another_cpu->GetLR()) os << "* ";
    os << "  CTR=" << hexa(GetCTR());
    if (this->GetCTR() != another_cpu->GetCTR()) os << "* ";
    os << endl;
  }

  /*
  bool Setup()
  {
    return true;
  }
  */

};

  
  class CPUEmu: public CPU<CPU_CONFIG>
  //		, public Client<MemoryInjection<CPU_CONFIG::address_t> >
		, public MJI_Client
//public CPUSim, public MI_Service
{
public:
  typedef bool (*sim_func_pointer)();
  //  typedef bool (*simulator_func_pointer)();

  //  MI_ServiceImport syscall_MemImp;
  //  MI_ServiceExport syscall_MemExp;
  ServiceImport<MemoryInjection<address_t> > memory_injection_import;
  
  bool program_ended;
  uint64_t end_at_cycle;

  CPUEmu(const char*cpu_name, unisim::kernel::Object *parent):
    Object(cpu_name,parent)
    //    MI_Client(cpu_name, parent),
    //    MI_Service(cpu_name, parent),
    //    CPUSim(cpu_name, parent),
    ,CPU<CPU_CONFIG>(cpu_name, parent)
    //    syscall_MemImp("syscall_MemImp",this),
    //    syscall_MemExp("syscall_MemExp",this)
    //    ,Client<MemoryInjection<CPU_CONFIG::address_t>(cpu_name, parent)
    , MJI_Client(cpu_name,parent)
    ,memory_injection_import("memory-injection-export", this)
    ,program_ended(false)
    ,end_at_cycle(0)
  {}

  virtual ~CPUEmu() {}

  void Stop(int ret)
  {
    cerr << Object::GetName() << ": Program exited with code " << ret << endl;
    //cerr << Object::GetName() << ": Simulation TERMINATE NOW ..." << endl;
    //terminate_now();
    program_ended = true;
    end_at_cycle = timestamp();
  }

  //  using full_system::processors::powerpc::physical_address_t;
  /*
  void Fetch(void *buffer, physical_address_t addr, uint32_t size)
  {
    ReadMemory(addr, buffer, size);
  }
  */

  /*
  void Reset()
  {
    // We don't need to reset DRAM for the moment ...
  }
  */


  /* TODO : We have to revisited CPUEmu and may be CPUSim ... */ 
     /*

  bool ReadMemory(address_t addr, void *buffer, uint32_t size)
  {
#ifdef DD_DEBUG_SYSCALL
    cerr << "[DD_DEBUG_SYSCALL]: in CPU_EMU ... ReadMemory() ...  CYCLE: "<< timestamp() << endl;
#endif
    //    return CPU::ReadMemory(addr,buffer,size);

    // CPU::ReadMemory(addr,buffer,size);
#ifdef DD_DEBUG_SYSCALL_VERB1
    {
      cerr << "["<<Object::GetName()<<"("<<timestamp()<<")]: ReadMemory(): "<< endl;
      cerr << "           addr   : "<<hexa(addr)<<endl;
      cerr << "           size   : "<<size<<endl;
      char tmpbuff[40];
      memcpy(tmpbuff,buffer,40);
      cerr << "           buffer : "<<tmpbuff<<endl;
    }
#endif
    
    return syscall_MemImp->ReadMemory(addr, buffer,size);
  }

  bool WriteMemory(address_t addr, const void *buffer, uint32_t size)
  {
#ifdef DD_DEBUG_SYSCALL
    cerr << "[DD_DEBUG_SYSCALL]: in CPU_EMU ... WriteMemory() ... CYCLE: "<< timestamp() << endl;
#endif
#ifdef DD_DEBUG_SYSCALL_VERB1
    cerr << "["<<Object::GetName()<<"("<<timestamp()<<")]: WriteMemory(): "<< endl;
    cerr << "           addr   : "<<hexa(addr)<<endl;
    cerr << "           size   : "<<size<<endl;
    char tmpbuff[40];
    memcpy(tmpbuff,buffer,40);
    cerr << "           buffer : "<<tmpbuff<<endl;
#endif
    //return CPU::WriteMemory(addr,buffer,size);
    //    CPU::WriteMemory(addr,buffer,size);
    return syscall_MemImp->WriteMemory(addr, buffer,size);
  }
     */

  bool InjectReadMemory(address_t addr, void *buffer, uint32_t size)
  {
    memory_injection_import->InjectReadMemory(addr, buffer, size);
    return true;
  }

  bool InjectWriteMemory(address_t addr, const void *buffer, uint32_t size)
  {
    memory_injection_import->InjectWriteMemory(addr, buffer, size);
    return true;
  }

  void Reset()
  {
  }

  //////////////////////////  
  // Others functions
  //////////////////////////  
  void dump_registers(ostream &os)
  { os << "PC=" << hexa(this->GetCIA()) << endl;
    for(int i = 0; i < 32; i++)
      { os << "r" << i;
      if (i < 10) os << " ";
      os << "=" << hexa(this->GetGPR(i));
      os << "  ";
      if ((i % 8) == 7) os << endl;
      }
    os << "CR= "   << hexa(this->GetCR());
    os << "  XER=" << hexa(this->GetXER());
    os << "  LR= " << hexa(this->GetLR());
    os << "  CTR=" << hexa(this->GetCTR());
    os << std::dec << endl;
  }

  void dump_registers_compare(ostream &os, CPU<CPU_CONFIG> *another_cpu)
  { os << "PC=" << hexa(this->GetCIA()) << endl;
    for(int i = 0; i < 32; i++)
    { os << "r" << i;
      if (i < 10) os << " ";
      os << "=" << hexa(this->GetGPR(i));
      if (this->GetGPR(i) != another_cpu->GetGPR(i)) os << "* ";
      else os << "  ";
      if ((i % 8) == 7) os << endl;
    }
    os << "CR= " << hexa(this->GetCR());
    if (this->GetCR() != another_cpu->GetCR()) os << "* ";
    os << "  XER=" << hexa(GetXER());
    if (this->GetXER() != another_cpu->GetXER()) os << "* ";
    os << "  LR= " << hexa(GetLR());
    if (this->GetLR() != another_cpu->GetLR()) os << "* ";
    os << "  CTR=" << hexa(GetCTR());
    if (this->GetCTR() != another_cpu->GetCTR()) os << "* ";
    os << endl;
  }
  /*
  bool Setup()
  {
    return true;
  }
  */
};


} // end of namespace ooosim
} // end of namespace processor
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif // __CPUEMULATOR__HH__
