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

#ifndef __UNISIM_COMPONENT_CLM_PROCESSOR_OOOSIM_CPU_SIMULATOR_MC_TCC__
#define __UNISIM_COMPONENT_CLM_PROCESSOR_OOOSIM_CPU_SIMULATOR_MC_TCC__

#define TAG_SEND_ON_MEMORY_REQUEST 0
//#define __BYTE_ORDER __BIG_ENDIAN

namespace unisim {
namespace component {
namespace clm {
namespace processor {
namespace ooosim_mc {

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

using unisim::component::clm::utility::hexa;

using unisim::kernel::Object;


/**
 * Module embedding a PowerPCSS processor pipeline
 */
  /**
   * \brief Creates a new OooSimCpu
   */
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  OooSimCpu(const char *name, Object *parent) : module(name)
					,Object(name, parent)
  { 
    class_name = " OooSimCpu";
    category = category_PROCESSOR;

    //    initialization();
    int a = 0x1234;
    host_endianess = (*(char *) &a == 0x34) ? little_endian : big_endian;
    //    this->endianess = options.endianess;
    this->endianess = big_endian;

    sensitive_pos_method(start_of_cycle) << inClock;
    
    speculative_cpu_state = new CPUSim("speculative_cpu_state",this);
    for(int cfg=0; cfg<nConfig; cfg++)
    {
      stringstream str;
      str << "check_emulator[" << cfg << "]";
      check_emulator[cfg] = new CPUEmu((str.str()).c_str(),this);
      // fetch_emulator[cfg] = new CPUSim("fetch_emulator",this);
    }
    fetch_emulator = new CPUSim("fetch_emulator",this);
    // For spec emu...
    //    mem_emu = new MyMemEmulator("mem_emu",this);

    fetch = new FetcherClass("fetch",this->endianess,fetch_emulator);
    
    allocate = new AllocatorRenamerClass("allocate");

    dispatch = new DispatcherClass("dispatch");
    // Does theses three lines have to be here?
    /*
    dispatch->SetIntegerIssueQueueFunction(FnInteger | FnBranch | FnSystem | FnSysCall);
    dispatch->SetFloatingPointIssueQueueFunction(FnFloatingPoint);
    dispatch->SetLoadStoreIssueQueueFunction(FnLoad | FnStore | FnPrefetchLoad);
    */
    dispatch->SetIntegerIssueQueueFunction( FnNop | FnIntBasic | FnIntBasicSerial | FnIntExtended | FnBranch | FnConditionalBranch | FnBranchCountReg | FnBranchLinkReg | FnCondRegister | FnSysRegister | FnSysRegisterSerial | FnSysCall | FnMac | FnCache );
    //      FnInteger | FnBranch | FnSystem | FnSysCall);
    dispatch->SetFloatingPointIssueQueueFunction( FnFpCommon | FnFpFPSCR | FnFpFPSCRSerial );// FnFloatingPoint);
    dispatch->SetLoadStoreIssueQueueFunction(FnLoad | FnStore | FnPrefetchLoad);


    schedule = new SchedulerClass("schedule");

    registerfile = new RegisterFileClass("registerfile");

    
    //    for (int i=0; i<nIntegerUnits; i++)
    //  { iu[i] = new IntegerUnitClass("iu",speculative_cpu_state);
    //  }
    iu = new IntegerUnitClass("iu",speculative_cpu_state);
    //	iu1_stage = new IntegerUnitClass("iu1_stage");
    //    iu2_stage = new IntegerUnitClass("iu2_stage");
    //    iu3_stage = new IntegerUnitClass("iu3_stage");
   
    //for (int i=0; i<nFloatingPointUnits; i++)
    //  { fpu[i] = new FloatingPointUnitClass("fpu",speculative_cpu_state);
    //  }
    fpu = new FloatingPointUnitClass("fpu",speculative_cpu_state);
    //    fpu1_stage = new FloatingPointUnitClass("fpu1_stage");
    
    //for (int i=0; i<nAddressGenerationUnits; i++)
    //  { agu[i] = new AddressGenerationUnitClass("agu",speculative_cpu_state);
    //  }
    agu = new AddressGenerationUnitClass("agu",speculative_cpu_state);

    lsq = new LoadStoreQueueClass("lsq",this->endianess);
    
    cdba = new CommonDataBusArbiterClass("cdba");
    
    rob = new ReorderBufferClass("rob",check_emulator);
 
    retbroadcast = new RetireBroadcasterClass("retbroadcast");

    //    execute_stage = new DummyExecutionCoreClass("execute_stage", speculative_cpu_state, check_emulator);

    // Internal signals
    /*
    inClock >> fetch_stage->inClock;
    inClock >> execute_stage->inClock;
    */
    
    fetch->inClock(inClock);
    allocate->inClock(inClock);
    schedule->inClock(inClock);
    registerfile->inClock(inClock);
    iu->inClock(inClock);
    fpu->inClock(inClock);
    agu->inClock(inClock);
    lsq->inClock(inClock);
    rob->inClock(inClock);
    
    // Flush connections
    //    execute_stage->outFlush >> fetch_stage->inFlush;

    /*
      for (int i=0; i<fetchWidth; i++)
      {
	// IL1 / Fetch connections
	//	inIL1Data >> fetch_stage->inIL1;
	//	fetch_stage->inIL1(inIL1Data);
	inIL1Data >> fetch->inIL1;
	//fetch_stage->outIL1 >> outIL1Data;
	outIL1Data >>  fetch->outIL1;


	
	//inDL1Data >> execute_stage->dummy_inDL1;
	//outDL1Data >> execute_stage->dummy_outDL1;
	

	// Fetch / Execute connections
	
	//fetch_stage->outInstruction[i] >> execute_stage->inInstruction[i];

	//execute_stage->outInstruction[i] >> fetch_stage->inWriteBackInstruction[i];
	//execute_stage->outRetireInstruction[i] >> fetch_stage->inRetireInstruction[i];
	
	}
    */
    /*
    inIL1Data >> fetch->inIL1;
    outIL1Data >>  fetch->outIL1;
    */
    
    // Fetch -> Allocator
    fetch->outInstruction >> allocate->inInstruction;
    // Allocator -> Dispatch
    allocate->outInstructionIssue >> dispatch->inInstruction;
    // Allocator -> ROB
    allocate->outInstructionReorder >> rob->inAllocateInstruction;
    // Allocator -> LSQ
    allocate->outLoadInstruction >> lsq->inAllocateLoadInstruction;
    allocate->outStoreInstruction >> lsq->inAllocateStoreInstruction;
    // Dispatch -> Scheduler
    dispatch->outIntegerInstruction >> schedule->inIntegerInstruction;
    dispatch->outFloatingPointInstruction >> schedule->inFloatingPointInstruction;
    dispatch->outLoadStoreInstruction >> schedule->inLoadStoreInstruction;
    // Scheduler -> RegisterFile
    schedule->outIntegerInstruction >> registerfile->inIntegerInstruction;
    schedule->outFloatingPointInstruction >> registerfile->inFloatingPointInstruction;
    schedule->outLoadStoreInstruction >> registerfile->inLoadStoreInstruction;
    // RegisterFile -> "Execution stage"
    registerfile->outIntegerInstruction >> iu->inInstruction;
    registerfile->outFloatingPointInstruction >> fpu->inInstruction;
    registerfile->outLoadStoreInstruction >> agu->inInstruction;
    // AGU -> LSQ
    agu->outLSQInstruction >> lsq->inInstruction;
    // "Execution stage" -> CDBA

    iu->outInstruction >> cdba->inINTInstruction;
    //iu->outInstruction(cdba->inInstruction[0]);

    fpu->outInstruction >> cdba->inFPUInstruction;

    agu->outCDBInstruction >> cdba->inAGUInstruction;
 
    lsq->outInstruction >> cdba->inLSQInstruction;

    // CDBA -> fetch
    cdba->outInstruction[0] >> fetch->inWriteBackInstruction;
    // CDBA -> allocate
    cdba->outInstruction[1] >> allocate->inWriteBackInstruction;
    // CDBA -> schedule
    cdba->outInstruction[2] >> schedule->inWriteBackInstruction;
    // CDBA -> registerfile
    cdba->outInstruction[3] >> registerfile->inWriteBackInstruction;
    // CDBA -> rob
    cdba->outInstruction[4] >> rob->inFinishInstruction;

    //    inport<InstructionPtr, nConfig > &tmpin = retbroadcast->inInstruction[0];
    //rob->outRetireInstruction >> tmpin;
    rob->outRetireInstruction >> retbroadcast->inInstruction;
    //    rob->outRetireInstruction(retbroadcast->inInstruction[0]);
    //    rob->outRetireInstruction(tmpin);

    // RetBroadcaster -> ...
    // retbroadcast -> fetch
    retbroadcast->outInstruction[0] >> fetch->inRetireInstruction;
    // rob -> allocate
    retbroadcast->outInstruction[1] >> allocate->inRetireInstruction;
    // rob -> lsq
    retbroadcast->outInstruction[2] >> lsq->inRetireInstruction;	
    
    // Loop back writeback ...
    registerfile->outWriteBackReceived >> rob->inWriteBackInstruction;

    // CPU -> Cache (lsq->dcache) 
    //lsq->outDL1[0] >> outDL1Data;
    // Forwards removed.
    /*
    outDL1Data >> lsq->outDL1[0];
    inDL1Data >> lsq->inDL1[0];
    */

    //// Flush signals
    rob->outFlush >> lsq->inFlush;
    lsq->outFlush >> agu->inFlush;
    agu->outFlush >> fpu->inFlush;
    fpu->outFlush >> iu->inFlush;
    iu->outFlush >> registerfile->inFlush;
    
    /*
    for (int i=1; i<Degree; i++)
      {
	agu[i]->outFlush >> agu[i-1]->inFlush;
	fpu[i]->outFlush >> fpu[i-1]->inFlush;
	iu[i]->outFlush >> iu[i-1]->inFlush;
      }
    */
    registerfile->outFlush >> schedule->inFlush;
    schedule->outFlush >> allocate->inFlush;
    allocate->outFlush >> fetch->inFlush;

    ///////////////////////   END OF Internal Connexions /////////////////////////////////////

    //Log files
    sprintf(ppc_pipeline_log,"ppc%d_pipeline.log",nProg);
    sprintf(ppc_regs_log,"ppc%d_regs.log",nProg);

  }

  /**
   * \brief Class destructor
   */
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  ~OooSimCpu()
  { delete speculative_cpu_state;
  }

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
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  ctrlz_hook()
  {
    cerr << "[CTRL-Z (OooSimCpu)] Hook !!! " << endl;
    cerr << "     Fetch CIA: "<< hexa(fetch_emulator->GetCIA()) << endl;
    cerr << "     CPU   CIA: "<< hexa(speculative_cpu_state->GetCIA()) << endl;
    //    for(int )
    //    cerr << "     Emu   CIA: "<< hexa(check_emulator[cfg]->GetCIA()) << endl;
  }

// Mourad Modifs 
////////////////////////////////
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  start_of_cycle()
  {
    // Check if simulation is finished :
    bool is_finished=true;
    for(int cfg=0; cfg<nConfig; cfg++)
    {
      is_finished &= check_emulator[cfg]->program_ended;
      if(check_emulator[cfg]->program_ended) fetch->set_terminated(cfg);
    }
    //is_finished = check_emulator[0]->program_ended;
    if (is_finished)
    {
      cerr << "Simulation ended at cycle : " << timestamp() << endl;
      for(int cfg=0; cfg<nConfig; cfg++)
      {
	cerr << "       Cpu[" << cfg << "] ended at cycle : " << check_emulator[cfg]->end_at_cycle << endl;
      }
      terminate_now();
    }

#ifdef DD_DEBUG_PIPELINE_VERB2
    if (DD_DEBUG_TIMESTAMP <= timestamp())
      {
	//	cerr << "("<< timestamp() << ")" << *cache;
	cerr << "("<< timestamp() << ")" << *fetch;
	cerr << "("<< timestamp() << ")" << *allocate;
	//    cerr << *dispatch;
	cerr << "("<< timestamp() << ")" << *schedule;
	cerr << "("<< timestamp() << ")" << *registerfile;
	
	//	for (int i=0; i<nIntegerUnits; i++)
	{ cerr << "("<< timestamp() << ")" << *iu;//[i];
	}
	//	for (int i=0; i<nIntegerUnits; i++)
	{ cerr << "("<< timestamp() << ")" << *fpu;//[i];
	}
	//	for (int i=0; i<nIntegerUnits; i++)
	{ cerr << "("<< timestamp() << ")" << *agu;//[i];
	}
	cerr << "("<< timestamp() << ")" << *lsq;
	cerr << "("<< timestamp() << ")" << *rob;
      }
#endif

    // executing in simulator has finished (theoreticaly, ie. effects will become visible
    // once the emulator executes), check correctness of CIA and execute in emulator:
    //          if (instruction_wb->cia != emulator->ReadCIA())
    //          if (instruction_wb->cia != cpu->GetCIA())
    
    //#ifdef DONT_CHECK_EACH_CYCLE 
    for(int cfg=0; cfg<nConfig; cfg++)
    {
#ifdef DD_CHECK_WITH_EMULATOR
      if ( (timestamp()%CHECK_REGISTER_STEP) == 0 )
      {
	if (!rob->syscall_retired[cfg])
	{
	  Check(cfg);
	}
      }
#endif    
      if (rob->syscall_retired[cfg] || timestamp() == 0)
      {
	RepaireAfterSyscall(cfg);
	//cerr << "(" << timestamp() << ") REPAIRE AFTER SYSCALL !!!" << endl;
      }
    // Dumps
#ifdef DD_DEBUG_CPUPPCSS_VERB1
// Mourad modifs 
///////////////////////////////////
      if (DD_DEBUG_TIMESTAMP <= timestamp())
	{
	  //	  cerr <<"["<<this->name()<<"("<<timestamp()<<")]===== DEBUG OooSimCpu (Begin) =====================" << "(" << timestamp() << ")" << endl;
	  cerr <<"["<<this->name()<<"("<<timestamp()<<")] Fetcher     CIA: " << hexa(fetch_emulator->GetCIA()) << endl;
	  cerr <<"["<<this->name()<<"("<<timestamp()<<")] Speculative CIA: " << hexa(speculative_cpu_state->GetCIA()) << endl;
	  cerr <<"["<<this->name()<<"("<<timestamp()<<")] Emulator    CIA: " << hexa(check_emulator->GetCIA()) << endl;

	  //DumpRegisters(cerr);
	  //	  cerr << *fetch_stage << endl;
	  //cerr << "===== DEBUG OooSimCpu (End) =======================" << "(" << timestamp() << ")" << endl;
	}
#endif
    } // End of foreach Config.
  }// End of start_of_cycle()
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
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  WriteCIA(address_t pc, uint32_t cfg)
  {
    fetch->WriteCIA(pc, cfg);
    rob->WriteCIA(pc, cfg);
  }
  
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  address_t
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  ReadCIA(int cfg)
  {
    return rob->ReadCIA(cfg);
  }

  ////////////////////// 
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  WriteGPR(int regnum, uint32_t data, int cfg)
  {
    int tag = allocate->ReadIntegerMappingTable(regnum, cfg);
    registerfile->WriteGPR(tag, data, cfg);
  }
  
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  uint32_t
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  ReadGPR(int regnum, int cfg)
  {
    int tag = allocate->ReadIntegerMappingTable(regnum, cfg);
    return registerfile->ReadGPR(tag, cfg);
  }
  
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  WriteFPR(int regnum, uint32_t data, int cfg)
  {
    int tag = allocate->ReadFloatingPointMappingTable(regnum, cfg);
    registerfile->WriteFPR(tag, data, cfg);
  }
  
  //  uint32_t ReadFPR(int regnum)
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  uint64_t
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  ReadFPR(int regnum, int cfg)
  {
    int tag = allocate->ReadFloatingPointMappingTable(regnum, cfg);
    return registerfile->ReadFPR(tag, cfg);
  }

  // For other registers...
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  uint32_t
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  ReadCR(int regnum, int cfg)
  {
    int tag = allocate->ReadConditionMappingTable(regnum, cfg);
    return registerfile->ReadCR(tag, cfg);
  }
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  WriteCR(int regnum, uint32_t data, int cfg)
  {
    int tag = allocate->ReadConditionMappingTable(regnum, cfg);
    registerfile->WriteCR(tag, data, cfg);
  }
  
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  uint32_t
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  ReadFPSCR(int regnum, int cfg)
  {
    int tag = allocate->ReadFPSCRMappingTable(regnum, cfg);
    return registerfile->ReadFPSCR(tag, cfg);
  }
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  WriteFPSCR(int regnum, uint32_t data, int cfg)
  {
    int tag = allocate->ReadFPSCRMappingTable(regnum, cfg);
    registerfile->WriteFPSCR(tag, data, cfg);
  }
  
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  uint32_t
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  ReadLR(int regnum, int cfg)
  {
    int tag = allocate->ReadLinkMappingTable(regnum, cfg);
    return registerfile->ReadLR(tag, cfg);
  }
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  WriteLR(int regnum, uint32_t data, int cfg)
  {
    int tag = allocate->ReadLinkMappingTable(regnum, cfg);
    registerfile->WriteLR(tag, data, cfg);
  }
  
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  uint32_t
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  ReadCTR(int regnum, int cfg)
  {
    int tag = allocate->ReadCountMappingTable(regnum, cfg);
    return registerfile->ReadCTR(tag, cfg);
  }
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  WriteCTR(int regnum, uint32_t data, int cfg)
  {
    int tag = allocate->ReadCountMappingTable(regnum, cfg);
    registerfile->WriteCTR(tag, data, cfg);
  }
  
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  uint32_t
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  ReadXER(int regnum, int cfg)
  {
    int tag = allocate->ReadXERMappingTable(regnum, cfg);
    return registerfile->ReadXER(tag, cfg);
  }
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  WriteXER(int regnum, uint32_t data, int cfg)
  {
    int tag = allocate->ReadXERMappingTable(regnum, cfg);
    registerfile->WriteXER(tag, data, cfg);
  }
  

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

  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  Check(int cfg)
  {
	address_t sim_pc = ReadCIA(cfg);
	address_t emul_pc = check_emulator[cfg]->GetCIA();
	if(sim_pc != emul_pc)
	{
		cerr << *this;
		cerr << "time stamp " << timestamp() << endl;
		cerr << "Different value for PC:" << endl;
		cerr << "simulator: " << hexa(sim_pc) << endl;
		cerr << "emulator : " << hexa(emul_pc) << endl;
		DumpRegistersCompare(cerr,check_emulator[cfg], cfg);
		exit(-1);
	}

	if (timestamp() == 0)
	  {
	    // At cycle 0, we need to set correct values into the RegisterFile.
	    for(int i = 0; i < nIntegerArchitecturalRegisters; i++)
	      {
		UInt64 sim_value = ReadGPR(i,cfg);
		UInt64 emul_value = check_emulator[cfg]->GetGPR(i);
		if(sim_value != emul_value)
		  {
		    WriteGPR(i,emul_value, cfg);
		  }
	      }	    
	    
	  }
	else
	  {
	    if (!rob->skip_reg_checking[cfg])
	      {
		for(int i = 0; i < nIntegerArchitecturalRegisters; i++)
		  {
		    UInt64 sim_value = ReadGPR(i, cfg);
		    UInt64 emul_value = check_emulator[cfg]->GetGPR(i);
		    if(sim_value != emul_value)
		      {
			cerr << " ==== CONFIGURATION ["<<cfg<<"] ====" << endl;
			cerr << *this;
			cerr << "time stamp " << timestamp() << endl;
			cerr << "Different value for GPR" << i << ":" << endl;
			cerr << "simulator: " << hexa(sim_value) << endl;
			cerr << "emulator : " << hexa(emul_value) << endl;
			DumpRegistersCompare(cerr,check_emulator[cfg], cfg);
			exit(-1);
		      }
		  }
		for(int i = 0; i < nFloatingPointArchitecturalRegisters; i++)
		  {
		    UInt64 sim_value = ReadFPR(i, cfg);
		    //    UInt64 emul_value = (check_emulator[cfg]->GetFp64(i, cfg)).queryValue();
		    UInt64 emul_value = check_emulator[cfg]->GetFPR(i).queryValue();
		    if(sim_value != emul_value)
		      {
			cerr << " ==== CONFIGURATION ["<<cfg<<"] ====" << endl;
			cerr << *this;
			cerr << "time stamp " << timestamp() << endl;
			cerr << "Different value for FPR" << i << ":" << endl;
			cerr << "simulator: " << hexa(sim_value) << endl;
			cerr << "emulator : " << hexa(emul_value) << endl;
			DumpRegistersCompare(cerr,check_emulator[cfg], cfg);
			exit(-1);
		      }
		  }
		// Check of control registers
		for(int i = 0; i < nConditionArchitecturalRegisters; i++)
		  {
		    UInt64 sim_value = ReadCR(i, cfg);
		    UInt64 emul_value = (check_emulator[cfg]->GetCRF(i) & 0x0000000f);
		    if ( sim_value != emul_value )
		      {
			cerr << " ==== CONFIGURATION ["<<cfg<<"] ====" << endl;
			cerr << *this;
			cerr << "time stamp " << timestamp() << endl;
			cerr << "Different value for CR" << i << ":" << endl;
			cerr << "simulator: " << hexa(sim_value) << endl;
			cerr << "emulator : " << hexa(emul_value) << endl;
			DumpRegistersCompare(cerr,check_emulator[cfg], cfg);
			exit(-1);
		      }
		  }
		for(int i = 0; i < nFPSCArchitecturalRegisters; i++)
		  {
		    UInt64 sim_value = ReadFPSCR(i, cfg);
		    UInt64 emul_value = check_emulator[cfg]->GetFPSCR();
		    if ( sim_value != emul_value )
		      {
			cerr << " ==== CONFIGURATION ["<<cfg<<"] ====" << endl;
			cerr << *this;
			cerr << "time stamp " << timestamp() << endl;
			cerr << "Different value for FPSCR" << i << ":" << endl;
			cerr << "simulator: " << hexa(sim_value) << endl;
			cerr << "emulator : " << hexa(emul_value) << endl;
			DumpRegistersCompare(cerr,check_emulator[cfg], cfg);
			exit(-1);
		      }
		  }
		for(int i = 0; i < nLinkArchitecturalRegisters; i++)
		  {
		    UInt64 sim_value = ReadLR(i, cfg);
		    UInt64 emul_value = check_emulator[cfg]->GetLR();
		    if ( sim_value != emul_value )
		      {
			cerr << " ==== CONFIGURATION ["<<cfg<<"] ====" << endl;
			cerr << *this;
			cerr << "time stamp " << timestamp() << endl;
			cerr << "Different value for LR" << i << ":" << endl;
			cerr << "simulator: " << hexa(sim_value) << endl;
			cerr << "emulator : " << hexa(emul_value) << endl;
			DumpRegistersCompare(cerr,check_emulator[cfg], cfg);
			exit(-1);
		      }
		  }
		for(int i = 0; i < nCountArchitecturalRegisters; i++)
		  {
		    UInt64 sim_value = ReadCTR(i, cfg);
		    UInt64 emul_value = check_emulator[cfg]->GetCTR();
		    if ( sim_value != emul_value )
		      {
			cerr << " ==== CONFIGURATION ["<<cfg<<"] ====" << endl;
			cerr << *this;
			cerr << "time stamp " << timestamp() << endl;
			cerr << "Different value for CTR" << i << ":" << endl;
			cerr << "simulator: " << hexa(sim_value) << endl;
			cerr << "emulator : " << hexa(emul_value) << endl;
			DumpRegistersCompare(cerr,check_emulator[cfg], cfg);
			exit(-1);
		      }
		  }
		for(int i = 0; i < nXERArchitecturalRegisters; i++)
		  {
		    UInt64 sim_value = ReadXER(i, cfg);
		    //UInt64 emul_value = check_emulator[cfg]->GetXER();
		    UInt64 emul_value;
		    switch(i)
		      {
		      case 0: emul_value = check_emulator[cfg]->GetXER_SO(); break;
		      case 1: emul_value = check_emulator[cfg]->GetXER_OV(); break;
		      case 2: emul_value = check_emulator[cfg]->GetXER_CA(); break;
		      case 3: emul_value = check_emulator[cfg]->GetXER_BYTE_COUNT(); break;
		      default: break;
		      }
		    if ( sim_value != emul_value )
		      {
			cerr << " ==== CONFIGURATION ["<<cfg<<"] ====" << endl;
			cerr << *this;
			cerr << "time stamp " << timestamp() << endl;
			cerr << "Different value for XER" << i << ":" << endl;
			cerr << "simulator: " << hexa(sim_value) << endl;
			cerr << "emulator : " << hexa(emul_value) << endl;
			DumpRegistersCompare(cerr,check_emulator[cfg], cfg);
			exit(-1);
		      }
		  }
	      }
	  }
  } // End of Check

  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  RepaireAfterSyscall(int cfg)
  {
	address_t sim_pc = ReadCIA(cfg);
	address_t emul_pc = check_emulator[cfg]->GetCIA();
	if(sim_pc != emul_pc)
	{
	  WriteCIA(emul_pc, cfg);
	}
	/*
	if (timestamp() == 0)
	  {
	    // At cycle 0, we need to set correct values into the RegisterFile.
	    for(int i = 0; i < nIntegerArchitecturalRegisters; i++)
	      {
		UInt64 sim_value = ReadGPR(i, cfg);
		UInt64 emul_value = check_emulator[cfg]->GetGPR(i);
		if(sim_value != emul_value)
		  {
		    WriteGPR(i,emul_value,cfg);
		  }
	      }	    
	    
	  }
	else
	*/
	  {
	    if (!rob->skip_reg_checking[cfg])
	      {
		for(int i = 0; i < nIntegerArchitecturalRegisters; i++)
		  {
		    UInt64 sim_value = ReadGPR(i, cfg);
		    UInt64 emul_value = check_emulator[cfg]->GetGPR(i);
		    if(sim_value != emul_value)
		      {
			WriteGPR(i,emul_value, cfg);
		      }
		  }
		// Check of FP registers
		for(int i = 0; i < nFloatingPointArchitecturalRegisters; i++)
		  {
		    UInt64 sim_value = ReadFPR(i, cfg);
		    //   UInt64 emul_value = (check_emulator[cfg]->GetFp64(i, cfg)).queryValue();
		    UInt64 emul_value = check_emulator[cfg]->GetFPR(i).queryValue();
		    if ( sim_value != emul_value )
		      {
			/*
			cerr << "REPAIRING time stamp " << timestamp() << endl;
			cerr << "Different value for CR" << i << ":" << endl;
			cerr << "simulator: " << hexa(sim_value) << endl;
			cerr << "emulator : " << hexa(emul_value) << endl;
			*/
			WriteFPR(i,emul_value, cfg);
		      }
		  }
		// Check of control registers
		for(int i = 0; i < nConditionArchitecturalRegisters; i++)
		  {
		    UInt64 sim_value = ReadCR(i, cfg);
		    UInt64 emul_value = (check_emulator[cfg]->GetCRF(i) & 0x0000000f);
		    if ( sim_value != emul_value )
		      {
			/*
			cerr << "REPAIRING time stamp " << timestamp() << endl;
			cerr << "Different value for CR" << i << ":" << endl;
			cerr << "simulator: " << hexa(sim_value) << endl;
			cerr << "emulator : " << hexa(emul_value) << endl;
			*/
			WriteCR(i,emul_value, cfg);
		      }
		  }
		for(int i = 0; i < nFPSCArchitecturalRegisters; i++)
		  {
		    UInt64 sim_value = ReadFPSCR(i, cfg);
		    UInt64 emul_value = check_emulator[cfg]->GetFPSCR();
		    if ( sim_value != emul_value )
		      {
			WriteFPSCR(i,emul_value, cfg);
		      }
		  }
		for(int i = 0; i < nLinkArchitecturalRegisters; i++)
		  {
		    UInt64 sim_value = ReadLR(i, cfg);
		    UInt64 emul_value = check_emulator[cfg]->GetLR();
		    if ( sim_value != emul_value )
		      {
			WriteLR(i,emul_value, cfg);			
		      }
		  }
		for(int i = 0; i < nCountArchitecturalRegisters; i++)
		  {
		    UInt64 sim_value = ReadCTR(i, cfg);
		    UInt64 emul_value = check_emulator[cfg]->GetCTR();
		    if ( sim_value != emul_value )
		      {
			WriteCTR(i,emul_value, cfg);
		      }
		  }
		for(int i = 0; i < nXERArchitecturalRegisters; i++)
		  {
		    UInt64 sim_value = ReadXER(i, cfg);
		    //UInt64 emul_value = check_emulator[cfg]->GetXER();
		    UInt64 emul_value;
		    switch(i)
		      {
		      case 0: emul_value = check_emulator[cfg]->GetXER_SO(); break;
		      case 1: emul_value = check_emulator[cfg]->GetXER_OV(); break;
		      case 2: emul_value = check_emulator[cfg]->GetXER_CA(); break;
		      case 3: emul_value = check_emulator[cfg]->GetXER_BYTE_COUNT(); break;
		      default: break;
		      }
		    if ( sim_value != emul_value )
		      {
			WriteXER(i,emul_value, cfg);
		      }
		  }
	      }
	  }
  } // End of RepaireAfterSyscall

  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  uint64_t
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  GetRetiredInstructions() {return 0;}

  /**
   * Dump the simulator registers for debugging purpose
   */
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  DumpRegistersCompare(ostream& os, CPUEmu *another_cpu, int cfg)
  { 
    os << "------ Simulator registers ---------------------------------------------" << endl;
    //    speculative_cpu_state->dump_registers_compare(os,check_emulator);
    /*
    for(int i = 0; i < nIntegerArchitecturalRegisters; i++)
      {
	UInt64 sim_value = ReadGPR(i);
	UInt64 emul_value = check_emulator->GetGPR(i);
	if(sim_value != emul_value)
	  {
	    cerr << *this;
	    cerr << "time stamp " << timestamp() << endl;
	    cerr << "Different value for GPR" << i << ":" << endl;
	    cerr << "simulator: " << hexa(sim_value) << endl;
	    cerr << "emulator : " << hexa(emul_value) << endl;
	    DumpRegisters(cerr);
	    exit(-1);
	  }
      }
    */
    //////
    os << "PC=" << hexa(ReadCIA(cfg)) << endl;
    //    for(int i = 0; i < 32; i++)
    for(int i = 0; i < nIntegerArchitecturalRegisters; i++)
      { 
	os << "r" << i;
	if (i < 10) os << " ";
	os << "=" << hexa(ReadGPR(i, cfg));
	if (ReadGPR(i, cfg) != another_cpu->GetGPR(i)) os << "* ";
	else os << "  ";
	if ((i % 8) == 7) os << endl;
      }
    
    for(int i = 0; i < nConditionArchitecturalRegisters; i++)
      { 
	os << "cr" << i;
	if (i < 10) os << " ";
	os << "=" << hexa(ReadCR(i, cfg));
	if (ReadCR(i, cfg) != another_cpu->GetCRF(i)) os << "* ";
	else os << "  ";
	if ((i % 8) == 7) os << endl;
      }
    
    for(int i = 0; i < nFPSCArchitecturalRegisters; i++)
      { 
	os << "fpscr" << i;
	if (i < 10) os << " ";
	os << "=" << hexa(ReadFPSCR(i, cfg));
	if (ReadFPSCR(i, cfg) != another_cpu->GetFPSCR()) os << "* ";
	else os << "  ";
	if ((i % 8) == 7) os << endl;
      }
    for(int i = 0; i < nLinkArchitecturalRegisters; i++)
      { 
	os << "lr" << i;
	if (i < 10) os << " ";
	os << "=" << hexa(ReadLR(i, cfg));
	if (ReadLR(i, cfg) != another_cpu->GetLR()) os << "* ";
	else os << "  ";
	if ((i % 8) == 7) os << endl;
      }
    for(int i = 0; i < nCountArchitecturalRegisters; i++)
      { 
	os << "ctr" << i;
	if (i < 10) os << " ";
	os << "=" << hexa(ReadCTR(i, cfg));
	if (ReadCTR(i, cfg) != another_cpu->GetCTR()) os << "* ";
	else os << "  ";
	if ((i % 8) == 7) os << endl;
      }    
    for(int i = 0; i < nXERArchitecturalRegisters; i++)
      { 
	os << "xer" << i;
	if (i < 10) os << " ";
	os << "=" << hexa(ReadXER(i, cfg));
	if (ReadXER(i, cfg) != another_cpu->GetXER()) os << "* ";
	else os << "  ";
	if ((i % 8) == 7) os << endl;
      }
    
    /*
    os << "CR= " << hexa(ReadCR());
    if (GetCR() != another_cpu->GetCR()) os << "* ";
    os << "  XER=" << hexa(GetXER());
    if (GetXER() != another_cpu->GetXER()) os << "* ";
    os << "  LR= " << hexa(GetLR());
    if (GetLR() != another_cpu->GetLR()) os << "* ";
    os << "  CTR=" << hexa(GetCTR());
    if (GetCTR() != another_cpu->GetCTR()) os << "* ";
    */
    os << endl;
    
    

    //    if (use_emulator)
    { 
      os << "------ Emulator registers ---------------------------------------------" << endl;
      //      check_emulator->dump_registers_compare(os,speculative_cpu_state);
      check_emulator[cfg]->dump_registers(os);
    }
  }

  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  double
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  GetTimeStamp(){}

  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  FastForward(uint64_t fastfwd)
  {}

  /**
   * \brief Dump the module statistics
   */
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  DumpStats(ostream& os, uint64_t timeStamp)
  { os << fsc_object::name() << ": ";
  //    os << "Instructions Executed: " << pipeline.instructions_executed << " #" << endl;
  }

  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  DumpParameters(ostream& os)
  {}

  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  ostream& operator << (ostream& os, const OooSimCpu
			<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
			nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>& processor)
  { return os;
  }


  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  Read(void *buffer, address_t address, int size)
  { /*	memory->Read(buffer, address, size);
    	L2->Read(buffer, address, size);
    	DL1->Read(buffer, address, size);
    */
    // STF !!!
  }

  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  Write(address_t address, const void *buffer, int size)
  { /*	DL1->Write(address, buffer, size);
    	L2->Write(address, buffer, size);
    	memory->Write(address, buffer, size);
    */
    // STF !!!
  }

  /**
   * \brief Reset the contents of the module
   */
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  Reset()
  {
    //pipeline.Reset();
    //pipeline.instructions_executed = 0;
    
    //Copy the register values from the emulator to the simulator after program loading

    //    speculative_cpu_state->copy_register_values(check_emulator);
    //    WriteCIA(check_emulator->GetCIA());

    //    fetch_emulator->copy_register_values(check_emulator);

    for(int cfg=0; cfg<nConfig; cfg++)
    { 
      // Fetcher Reset will copy into fecth cia the fetch_emulator cia (a wrong cia : 0xfff00100)
      fetch->Reset(cfg);
      
      // WriteCIA will copy the correct starting cia (from check emulator) into fetch cia (0x10000100)
      WriteCIA(check_emulator[cfg]->GetCIA(), cfg);
    }
    speculative_cpu_state->SetMSR_FP();
    speculative_cpu_state->SetMSR_PR();
      //    pipeline.next_fetch_address = check_emulator->GetCIA();
      //Reset instruction cache
      /*
	icache.linefill.size = 0;
	icache.memory_operation = MEMOP_NOTHING;
      */
  }

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
  void initialization()
  {  
    
  }
*/
  /**
   * \brief Dump some debugging information about the module
   */
  template
  <
    int nIntegerRegisters,
    int nIL1CachetoCPUDataPathSize,
    int nIL1CPUtoCacheDataPathSize,
    int nDL1CachetoCPUDataPathSize,
    int nDL1CPUtoCacheDataPathSize,
    int nProg,
    bool VERBOSE,
    uint32_t nConfig
    >
  void
  OooSimCpu<nIntegerRegisters,nIL1CachetoCPUDataPathSize,nIL1CPUtoCacheDataPathSize,
	    nDL1CachetoCPUDataPathSize,nDL1CPUtoCacheDataPathSize,nProg,VERBOSE,nConfig>:: 
  dump()
  { if(timestamp()==0)
    { ofstream os1(ppc_pipeline_log,ios_base::trunc);
      os1.close();
      ofstream os2(ppc_regs_log,ios_base::trunc);
      os2.close();
    }
    { ofstream os(ppc_pipeline_log,ios_base::app);
      os << "\n-----------------------------------------------------------------------" << endl;
      os << "  Cycle " << left << timestamp() << endl;
      os << "-----------------------------------------------------------------------" << endl;
      //      os << pipeline << endl;
      os << "-----------------------------------------------------------------------" << endl;
      os.close();
    }
    { stringstream ss;
    //      DumpRegisters(ss);
      if(last_reg_dump!=ss.str())
      { ofstream os(ppc_regs_log,ios_base::app);
        os << "\n------------------------------------------------------------------------------------------------------------------------------" << endl;
        os << "  Cycle " << left << timestamp() << endl;
        os << "------------------------------------------------------------------------------------------------------------------------------" << endl;
        os << ss.str();
        os << "------------------------------------------------------------------------------------------------------------------------------" << endl;
        os.close();
	last_reg_dump = ss.str();
      }
    }
  }


} // end of namespace ooosim_mc
} // end of namespace processor
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif


