/*
 *  Copyright (c) 2007,
 *  Institut National de Recherche en Informatique et en Automatique (INRIA)
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
 *     Marek Doniec (???)
 *     Sylvain Girbal (sylvain.girbal@inria.fr)
 *     David Parello (david.parello@univ-perp.fr)
 *
 */

/***************************************************************************
                            OooSimCpu.sim  -  description
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_PROCESSOR_OOOSIM_CPU_SIMULATOR_MC_HH__
#define __UNISIM_COMPONENT_CLM_PROCESSOR_OOOSIM_CPU_SIMULATOR_MC_HH__

#define TAG_SEND_ON_MEMORY_REQUEST 0
//#define __BYTE_ORDER __BIG_ENDIAN

#include <unisim/kernel/clm/engine/unisim.h>

//#include <unisim/component/clm/interface/iss_interface.hh>
#include <unisim/component/clm/processor/ooosim/iss_interface.hh>
#include <unisim/component/clm/utility/error.h>
//#include "memreq.h"
#include <unisim/component/clm/interfaces/memreq_mc.hh>
#include <unisim/component/clm/memory/mem_common.hh>

#include <unisim/component/clm/utility/utility.hh>

#include <unisim/component/clm/processor/ooosim_mc/cpu_emulator.hh>

// To enable Emulator and Simulator communication ...
//#include <generic/memory/memory_interface.hh>
//#include <unisim/component/cxx/memory/ram/memory_interface.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/kernel/kernel.hh>

#include <sstream>

// Simulator parameters :
//#include <unisim/component/clm/processor/ooosim_mc/parameters.hh>
////////////////////////////////////////////////////////////////////
// Pipeline Stages
////////////////////////////////////////////////////////////////////
#include <unisim/component/clm/pipeline/fetch/fetcher_mc.hh>

#include <unisim/component/clm/pipeline/decode/dispatcher_mc.hh>
#include <unisim/component/clm/pipeline/decode/allocator_renamer_mc.hh>

#include <unisim/component/clm/pipeline/issue/scheduler_mc.hh>
#include <unisim/component/clm/pipeline/issue/register_file_mc.hh>

#include <unisim/component/clm/pipeline/execute/functional_unit_mc.hh>
#include <unisim/component/clm/pipeline/execute/address_generation_unit_mc.hh>
#include <unisim/component/clm/pipeline/execute/load_store_queue_mc.hh>

//#include <unisim/component/clm/pipeline/common/simple_arbiter_mc.hh>
#include <unisim/component/clm/pipeline/common/data_bus_arbiter_mc.hh>
#include <unisim/component/clm/pipeline/common/simple_arbiter_onetoall_mc.hh>

#include <unisim/component/clm/pipeline/commit/reorder_buffer_mc.hh>
////////////////////////////////////////////////////////////////////
//#include <unisim/component/clm/pipeline/execute/DummyExecutionCore.hh>

//#include <generic/memory/MyMemEmulator.hh>
//#include <generic/memory/MyMemEmulator.hh>

namespace unisim {
namespace component {
namespace clm {
namespace processor {
namespace ooosim_mc {

using unisim::component::clm::memory::address_t;
using unisim::component::clm::memory::endianess_t;
using unisim::component::clm::memory::big_endian;
using unisim::component::clm::memory::little_endian;
using unisim::component::clm::memory::ByteArray;

using unisim::component::clm::interfaces::InstructionPtr;

using unisim::component::clm::interfaces::memreq;
using unisim::component::clm::interfaces::memreq_types;

using unisim::component::clm::pipeline::fetch::Fetcher;
using unisim::component::clm::pipeline::decode::AllocatorRenamer;
using unisim::component::clm::pipeline::decode::Dispatcher;
using unisim::component::clm::pipeline::issue::Scheduler;
using unisim::component::clm::pipeline::issue::RegisterFile;
using unisim::component::clm::pipeline::execute::FunctionalUnit;
using unisim::component::clm::pipeline::execute::AddressGenerationUnit;
using unisim::component::clm::pipeline::execute::LoadStoreQueue;
using unisim::component::clm::pipeline::common::DataBusArbiter;
using unisim::component::clm::pipeline::common::ArbiterOnetoAll;
using unisim::component::clm::pipeline::commit::ReorderBuffer;
  //using unisim::component::clm::pipeline::fetch::;
  //using unisim::component::clm::pipeline::fetch::;
  //using unisim::component::clm::pipeline::fetch::;
  //using unisim::component::clm::pipeline::fetch::;
  //using unisim::component::clm::pipeline::fetch::;

using unisim::component::clm::utility::hexa;

using unisim::kernel::Object;
/*
using full_system::utils::services::ServiceExport;
using full_system::utils::services::ServiceImport;
using full_system::utils::services::Object;
using full_system::utils::services::Service;
using full_system::utils::services::Client;
using full_system::generic::memory::MemoryInterface;
*/

/*
typedef ServiceExport< Memory<address_t> > MI_ServiceExport;
typedef ServiceImport< Memory<address_t> > MI_ServiceImport;

typedef Service< Memory<address_t> > MI_Service;
typedef Client< Memory<address_t> > MI_Client;
*/

/**
 * Module embedding a PowerPCSS processor pipeline
 */
template
<
  int nIntegerRegisters,
  int nIL1CachetoCPUDataPathSize,
  int nIL1CPUtoCacheDataPathSize,
  //int nIL1CachetoMemDataPathSize,
  //int nIL1MemtoCacheDataPathSize,
  //int nIL1LineSize,
  //int nIL1CacheLines,
  //int nIL1Associativity,
  //  int nDL1LineSize,
  //int nDL1CacheLines,
  //int nDL1Associativity,
  int nDL1CachetoCPUDataPathSize,
  int nDL1CPUtoCacheDataPathSize,
  //int nDL1CachetoMemDataPathSize,
  //int nDL1MemtoCacheDataPathSize
  int nProg,
  bool VERBOSE = false,
  uint32_t nConfig=2
>
class OooSimCpu : public module, public Object//, public MI_Client, public MI_Service
{public:

  /*
  MI_ServiceExport syscall_MemExp;
  MI_ServiceImport syscall_MemImp;
  */

  /* Clock */
  inclock inClock;                                                             ///< clock port

  /* Ports */
  /*
  outport < memreq < Instruction, nIL1CachetoMemDataPathSize > > outIL1Data;   ///< To L1 Instruction Memory
  inport  < memreq < Instruction, nIL1MemtoCacheDataPathSize > > inIL1Data;    ///< From L1 Instruction Cache
  */

  // Forwarded ports are not very well supported, especially for latex generation...
  // So we remove them.
  /*
  outport < memreq < InstructionPtr, nIL1CPUtoCacheDataPathSize > > outIL1Data;   ///< To L1 Instruction Memory
  inport  < memreq < InstructionPtr, nIL1CachetoCPUDataPathSize > > inIL1Data;    ///< From L1 Instruction Cache

  outport < memreq < InstructionPtr, nDL1CPUtoCacheDataPathSize > > outDL1Data;   ///< To data cache
  inport  < memreq < InstructionPtr, nDL1CachetoCPUDataPathSize > > inDL1Data;    ///< From data Cache
  */
  bool verbose;               ///< Increase verbosity level when set to true
  bool use_emulator;          ///< Validate against the emulator if set to true

  /**
   * \brief Creates a new OooSimCpu
   */
  OooSimCpu(const char *name, Object *parent=0);
  /**
   * \brief Class destructor
   */
  virtual ~OooSimCpu();

  /**
   * Dump the simulator registers for debugging purpose
   */
  /*
  void DumpRegisters(ostream& os)
  { os << "------ Simulator registers ---------------------------------------------" << endl;
    speculative_cpu_state->dump_registers_compare(os,check_emulator);
    //    if (use_emulator)
    { os << "------ Emulator registers ---------------------------------------------" << endl;
      check_emulator->dump_registers_compare(os,speculative_cpu_state);
    }
  }
  */
  void ctrlz_hook();

////////////////////////////////
  void start_of_cycle();

  /**
   * \brief Automatic Setup of OooSimCpu
   */
  /*
  bool ClientIndependentSetup()
  {
    if (syscall_MemImp) {return true;} else { cerr << "Client IS : "<< this->name() 
						   << "  Syscal_MemImp not connected !!!" 
						   << endl; return false;}
  }
  */
  /*
  bool ClientDependentSetup()
  {
    if (syscall_MemImp) {return true;} else {return false;}
  }
  */
  /**
   * \brief Implement a memory interface service to handle system calls
   */
  /*
  bool ReadMemory(address_t addr, void *buffer, uint32_t size)
  {
#ifdef DD_DEBUG_SYSCALL
    cerr << "[DD_DEBUG_SYSCALL]: in CPU_PPC ... ReadMemory() ..." << endl;
#endif
    //    Read(buffer,addr,size);
    //return syscall_MemImp->ReadMemory(addr,buffer,size);
    //    return true;
    return syscall_MemImp->ReadMemory(addr,buffer,size);
  }
  */
  /**
   * \brief Implement a memory interface service to handle system calls
   */
  /*
  bool WriteMemory(address_t addr, const void *buffer, uint32_t size)
  {
#ifdef DD_DEBUG_SYSCALL
    cerr << "[DD_DEBUG_SYSCALL]: in CPU_PPC ... WriteMemory() ..." << endl;
#endif
    //    Write(buffer,addr,size);
    //return syscall_MemImp->WriteMemory(addr,buffer,size);
    //    return true;
    return syscall_MemImp->WriteMemory(addr,buffer,size);
  }
  */
  /**
   * \brief Write to a global register
   */
  /*
  void WriteGPR(int regnum, uint32_t data)
  { //speculative_cpu_state->gpr[regnum] = data;
  speculative_cpu_state->SetGPR(regnum, data);
  }
  */
  /**
   * \brief Read from a global register
   */
  /*
     uint32_t ReadGPR(int regnum)
     { //return speculative_cpu_state->gpr[regnum];
     return speculative_cpu_state->GetGPR(regnum);
     }
  */
     /**
     * \write Write to the Current Instruction Address register
     */
  /*
    void WriteCIA(address_t pc)
    { speculative_cpu_state->SetCIA(pc);
    //    pipeline.cia = pc;
    }
  */
  /**
   * \write Read from the Current Instruction Address register
   */
  /*
    address_t ReadCIA()
    { //return speculative_cpu_state->cia;
    return speculative_cpu_state->GetCIA();
    }
  */
  /*
    void Set(address_t address, uint8_t data, int size)
    { //	DL1->Set(address, data, size);
    //	L2->Set(address, data, size);
    //	memory->Set(address, data, size);
    
    // STF !!!!!
    }
  */
  void WriteCIA(address_t pc, uint32_t cfg);
  
  address_t ReadCIA(int cfg);

  ////////////////////// 
  void WriteGPR(int regnum, uint32_t data, int cfg);
  
  uint32_t ReadGPR(int regnum, int cfg);
  
  void WriteFPR(int regnum, uint32_t data, int cfg);
  
  //  uint32_t ReadFPR(int regnum)
  uint64_t ReadFPR(int regnum, int cfg);

  // For other registers...
  uint32_t ReadCR(int regnum, int cfg);

  void WriteCR(int regnum, uint32_t data, int cfg);
  
  uint32_t ReadFPSCR(int regnum, int cfg);

  void WriteFPSCR(int regnum, uint32_t data, int cfg);
  
  uint32_t ReadLR(int regnum, int cfg);

  void WriteLR(int regnum, uint32_t data, int cfg);
  
  uint32_t ReadCTR(int regnum, int cfg);

  void WriteCTR(int regnum, uint32_t data, int cfg);
  
  uint32_t ReadXER(int regnum, int cfg);

  void WriteXER(int regnum, uint32_t data, int cfg);
  

/*
  template <class T>
  void Read(T *data, address_t address, endianess_t target_endianess)
  { if(host_endianess != target_endianess)
    { T buffer;
      Read(&buffer, address, sizeof(T));
      Swap(data, &buffer);
    }
    else
    { Read(data, address, sizeof(T));
    }
    // STF !!!!!
  }
  
  template <class T>
  void Write(T *data, address_t address, endianess_t target_endianess)
  { if(host_endianess != target_endianess)
    { T buffer;
      Swap(&buffer, data);
      Write(address, &buffer, sizeof(T));
    }
    else
    { Write(address, data, sizeof(T));
    }
    // STF !!!!!
  }
*/

  void Check(int cfg);

  void RepaireAfterSyscall(int cfg);

  uint64_t GetRetiredInstructions();

  /**
   * Dump the simulator registers for debugging purpose
   */
  void DumpRegistersCompare(ostream& os, CPUEmu *another_cpu, int cfg);

  double GetTimeStamp();

  void FastForward(uint64_t fastfwd);

  /**
   * \brief Dump the module statistics
   */
  void DumpStats(ostream& os, uint64_t timeStamp);

  void DumpParameters(ostream& os);

  template
  <
    int nnIntegerRegisters,
    int nnIL1CachetoCPUDataPathSize,
    int nnIL1CPUtoCacheDataPathSize,
    int nnDL1CachetoCPUDataPathSize,
    int nnDL1CPUtoCacheDataPathSize,
    int nnProg,
    bool nnVERBOSE,
    uint32_t nnConfig
    >
  friend ostream& operator << (ostream& os, const OooSimCpu<nnIntegerRegisters,nnIL1CachetoCPUDataPathSize,nnIL1CPUtoCacheDataPathSize,nnDL1CachetoCPUDataPathSize,nnDL1CPUtoCacheDataPathSize,nnProg,nnVERBOSE,nnConfig>& processor);


  void Read(void *buffer, address_t address, int size);

  void Write(address_t address, const void *buffer, int size);

  /**
   * \brief Reset the contents of the module
   */
  void Reset();

  //  cpu_ppc405_sim_mem_obj_tpl memory_object;
  /*
  void SetDataMemoryContainer(MemoryContainer *container)
  { memory_object.memory = container;
    memory_object.icache = &icache;
    //    memory_object.dcache = &dcache;
    // TODO : ???
    //    speculative_cpu_state->simulation_memory_object = &memory_object;
    //    check_emulator->simulation_memory_object = &memory_object;
  }
  */

 private:
  
  //  bool	initialized;            ///< Guarding variable to avoid systemc conflicts
  endianess_t host_endianess;   ///< Endianess to be used

  //  PPC405Pipeline<nIL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize> pipeline;  ///< The PPC pipeline
 //  Emulator emulator;            ///< The external instruction set simulator to use
 //  ppc_cpu_t *speculative_cpu_state;
 public:
  CPUSim *speculative_cpu_state;
  CPUEmu *check_emulator[nConfig];
  CPUSim *fetch_emulator;

  // For spec and fetch emu :
  //  MyMemEmulator *mem_emu;


  //private:
public:
  // Pipeline Stages
  //   Fetch
  //  typedef Fetcher<UInt64, nSources, fetchWidth, IL1_nLineSize, IL1_nCachetoCPUDataPathSize, InstructionQueueSize, InstructionSize, BHT_Size, BHT_nLevels, BHT_nHistorySize, BTB_nlines, BTB_associativity, RAS_Size, retireWidth, WriteBackWidth,fetchMaxPendingRequests, MaxBranches> FetcherClass;
  typedef Fetcher<UInt64, nSources, fetchWidth, IL1_nLineSize, nIL1CachetoCPUDataPathSize, InstructionQueueSize, InstructionSize, BHT_Size, BHT_nLevels, BHT_nHistorySize, BTB_nlines, BTB_associativity, RAS_Size, retireWidth, WriteBackWidth,fetchMaxPendingRequests, MaxBranches, nConfig> FetcherClass;
public: // Allow external connections with Inst. Cache.
  FetcherClass *fetch;
  //private:
public:

  //typedef DummyExecutionCore<UInt64, nSources, Width, WriteBackWidth, retireWidth, nStages> DummyExecutionCore_stage;
  //  typedef DummyExecutionCore<UInt64, nSources, fetchWidth, WriteBackWidth, retireWidth, 1, nDL1CachetoCPUDataPathSize> DummyExecutionCoreClass;
  //  DummyExecutionCoreClass *execute_stage;
  
typedef AllocatorRenamer<UInt64, nSources, nIntegerArchitecturalRegisters, nFloatingPointArchitecturalRegisters, nIntegerRegisters, nFloatingPointRegisters, allocateRenameWidth, WriteBackWidth, retireWidth, reorderBufferSize, nAllocateRenameStages, nConfig> AllocatorRenamerClass;
  AllocatorRenamerClass *allocate;

  //typedef Dispatcher<UInt64, nSources, allocateRenameWidth, nIntegerIssueQueueWritePorts, nFloatingPointIssueQueueWritePorts, nLoadStoreIssueQueueWritePorts> DispatcherClass;

typedef Dispatcher<allocateRenameWidth, nIntegerIssueQueueWritePorts, nFloatingPointIssueQueueWritePorts, nLoadStoreIssueQueueWritePorts, nConfig> DispatcherClass;
  DispatcherClass *dispatch;

typedef Scheduler<UInt64,
                nSources,
		  //                IssueWidth,
                IntegerIssueQueueSize,
                IntegerIssueQueueIssueWidth,
                nIntegerIssueQueueWritePorts,
                FloatingPointIssueQueueSize,
                FloatingPointIssueQueueIssueWidth,
                nFloatingPointIssueQueueWritePorts,
                LoadStoreIssueQueueSize,
                LoadStoreIssueQueueIssueWidth,
                nLoadStoreIssueQueueWritePorts,
                WriteBackWidth,
                nIntegerRegisters,
                nFloatingPointRegisters,
		  nConfig> SchedulerClass;
  SchedulerClass *schedule;

typedef RegisterFile<UInt64, nSources, nIntegerRegisters, nFloatingPointRegisters, nReadRegisterStages, IntegerReadRegisterWidth, FloatingPointReadRegisterWidth, LoadStoreReadRegisterWidth, nWriteBackStages, WriteBackWidth, nConfig> RegisterFileClass;
  RegisterFileClass *registerfile;

  //typedef InstructionBroadcaster<UInt64, nSources, IntegerReadRegisterWidth, nIntegerUnits> IntegerInstructionBroadcasterClass;

  //typedef InstructionBroadcaster<UInt64, nSources, FloatingPointReadRegisterWidth, nFloatingPointUnits> FloatingPointInstructionBroadcasterClass;

  //typedef InstructionBroadcaster<UInt64, nSources, LoadStoreReadRegisterWidth, nAddressGenerationUnits> LoadStoreInstructionBroadcasterClass;

typedef FunctionalUnit<UInt64, nSources, integerPipelineDepth, nIntegerUnits, nConfig> IntegerUnitClass;
  IntegerUnitClass *iu;
  /*
  IntegerUnitClass *iu1_stage;
  IntegerUnitClass *iu2_stage;
  IntegerUnitClass *iu3_stage;
  */
typedef FunctionalUnit<UInt64, nSources, floatingPointPipelineDepth, nFloatingPointUnits, nConfig > FloatingPointUnitClass;
  FloatingPointUnitClass *fpu;
  //  FloatingPointUnitClass *fpu1_stage;

typedef AddressGenerationUnit<UInt64, nSources, addressGenerationPipelineDepth, nAddressGenerationUnits, nConfig > AddressGenerationUnitClass;
  AddressGenerationUnitClass *agu;
  //  AddressGenerationUnitClass *agu1_stage;

typedef LoadStoreQueue<UInt64, nSources, loadQueueSize, storeQueueSize, nAddressGenerationUnits, allocateRenameWidth, retireWidth, DL1_nCPUtoCacheDataPathSize, DL1_nPorts, LSQ_nCDBPorts, nConfig> LoadStoreQueueClass;
public: // Allow external connections with Inst. Cache.
  LoadStoreQueueClass *lsq;
  //private:
public:
  static const int nChannels = 5;
  //typedef Arbiter<UInt64, nSources, commonDataBusArbiterPorts, WriteBackWidth, nChannels> CommonDataBusArbiterClass;
typedef DataBusArbiter<UInt64, nSources, nIntegerUnits, nFloatingPointUnits, nAddressGenerationUnits, LSQ_nCDBPorts, WriteBackWidth, nChannels, nConfig> CommonDataBusArbiterClass;
  CommonDataBusArbiterClass *cdba;

  static const int nRetireChannels = 3;
  //typedef Arbiter<UInt64, nSources, retireWidth, retireWidth, nRetireChannels> RetireBroadcasterClass;
typedef ArbiterOnetoAll<UInt64, nSources, retireWidth, retireWidth, nRetireChannels, nConfig> RetireBroadcasterClass;
  RetireBroadcasterClass *retbroadcast;

typedef ReorderBuffer<UInt64, nSources, reorderBufferSize, allocateRenameWidth, WriteBackWidth, retireWidth, nConfig> ReorderBufferClass;
  ReorderBufferClass *rob;
 
private:
 
  /* Registers interface */
  /*
  static void write_gpr(void *, int, uint32_t);
  static uint32_t read_gpr(void *, int);

  static void write_pc(void *instance, uint32_t pc)
  { OooSimCpu *processor = static_cast<OooSimCpu *>(instance);
    processor->WriteCIA(pc);
  }

  static double read_fpr(void *, int);
  static void write_fpr(void *, int, double);

  // NULL Memory interface
  static uint8_t sim_read8_physical(CPU<CPU_CONFIG> *cpu, address_t addr, int attr)
  { return 0;
  }
  
  static uint16_t sim_read16_physical_big(CPU<CPU_CONFIG> *cpu, address_t addr, int attr)
  { return 0;
  }

  static uint16_t sim_read16_physical_little(CPU<CPU_CONFIG> *cpu, address_t addr, int attr)
  { return 0;
  }

  static uint32_t sim_read32_physical_big(CPU<CPU_CONFIG> *cpu, address_t addr, int attr)
  { return 0;
  }

  static uint32_t sim_read32_physical_little(CPU<CPU_CONFIG> *cpu, address_t addr, int attr)
  { return 0;
  }

  static uint64_t sim_read64_physical_big(CPU<CPU_CONFIG> *cpu, address_t addr, int attr)
  { return 0;
  }

  static uint64_t sim_read64_physical_little(CPU<CPU_CONFIG> *cpu, address_t addr, int attr)
  { return 0;
  }

  static void sim_write8_physical(CPU<CPU_CONFIG> *cpu, address_t addr, uint8_t value, int attr)
  { }

  static void sim_write16_physical_big(CPU<CPU_CONFIG> *cpu, address_t addr, uint16_t value, int attr)
  { }

  static void sim_write16_physical_little(CPU<CPU_CONFIG> *cpu, address_t addr, uint16_t value, int attr)
  { }

  static void sim_write32_physical_big(CPU<CPU_CONFIG> *cpu, address_t addr, uint32_t value, int attr)
  { }

  static void sim_write32_physical_little(CPU<CPU_CONFIG> *cpu, address_t addr, uint32_t value, int attr)
  { }

  static void sim_write64_physical_big(CPU<CPU_CONFIG> *cpu, address_t addr, uint64_t value, int attr)
  { }

  static void sim_write64_physical_little(CPU<CPU_CONFIG> *cpu, address_t addr, uint64_t value, int attr)
  { }

  static void sim_set_buffer_physical(CPU<CPU_CONFIG> *cpu, address_t paddr, uint8_t value, int size)
  { }

  static void sim_read_buffer_physical(CPU<CPU_CONFIG> *cpu, void *buffer, address_t paddr, int size)
  { }

  static void sim_write_buffer_physical(CPU<CPU_CONFIG> *cpu, address_t paddr, void *buffer, int size)
  { }

  */

  /*
  static void sim_syscall_read_buffer_physical(CPU<CPU_CONFIG> *cpu, void *buffer, address_t paddr, int size)
  { //cerr << __FUNCTION__ << " SIMULATION: direct memory read from SimulationMemoryContainer: addr=" << hexa(paddr) << " of size=" << size << endl;
    global_memory->Read(buffer,paddr,size);
  }
  */
  /*
  static void sim_syscall_write_buffer_physical(CPU<CPU_CONFIG> *cpu, address_t paddr, void *buffer, int size)
  { cerr << __FUNCTION__ << "SIMULATION: direct memory write to SimulationMemoryContainer: addr=" << hexa(paddr) << " of size=" << size << endl;
//    exit(1);
  }

  static void emul_syscall_read_buffer_physical(CPU<CPU_CONFIG> *cpu, void *buffer, address_t paddr, int size)
  { cerr << __FUNCTION__ << "EMULATION: direct memory read from SimulationMemoryContainer: addr=" << hexa(paddr) << " of size=" << size << endl;
  }

  static void emul_syscall_write_buffer_physical(CPU<CPU_CONFIG> *cpu, address_t paddr, void *buffer, int size)
  { cerr << __FUNCTION__ << "EMULATION: direct memory write to SimulationMemoryContainer: addr=" << hexa(paddr) << " of size=" << size << endl;
  }
  */
/*
  template <class T>
  void Swap(T *destination, T *source)
  { int size = sizeof(T);
    char *dst = (char *) destination + size - 1;
    char *src = (char *) source;
    do
    { *dst = *src;
    }
    while(dst++, src--, --size);
  }
*/

/*
  void initialization();
*/
  // cpu_ppc405_options options;

  char ppc_pipeline_log[255];
  char ppc_regs_log[255];

  string last_reg_dump;

  /**
   * \brief Dump some debugging information about the module
   */
  void dump();

  endianess_t endianess;
};

} // end of namespace ooosim_mc
} // end of namespace processor
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif


