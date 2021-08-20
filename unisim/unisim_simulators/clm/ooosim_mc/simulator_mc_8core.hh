/*
 *  Copyright (c) 2007,
 *  University of Perpignan (UPVD),
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

/***************************************************************************
                            simulator.hh  -  description
 ***************************************************************************/

// -------- COMMANDS ------------------------------
#define USE_REF
#define DD_UNSAFE_OPTIMIZATION
//#define STOP
// Must be define in kernel files //#define DEBUG_FSC
// --------- CYCLE FOR DEBUGING -----------------------------
//#define DD_DEBUG_TIMESTAMP 438300
//#define DD_DEBUG_TIMESTAMP 9450
//#define DD_DEBUG_TIMESTAMP 3400
//#define DD_DEBUG_TIMESTAMP 82300
//#define DD_DEBUG_TIMESTAMP 352320
//#define DD_DEBUG_TIMESTAMP 381929
//#define DD_DEBUG_TIMESTAMP 70000
//#define DD_DEBUG_TIMESTAMP 3552229
//#define DD_DEBUG_TIMESTAMP 0
//#define DD_DEBUG_TIMESTAMP 1000
//#define DD_DEBUG_TIMESTAMP 300

// ------------------------------------------------
// -------- DEBUG TAGS ----------------------------
// ------------------------------------------------
// -------- MEMORY SUBSYSTEM ----------------------


#ifdef DD_DEBUG_TIMESTAMP
/*
#define DD_DEBUG_USEREF
#define DD_DEBUG_OPERATIONREFCOUNT
*/

//#define DD_DISPLAY_SIGNALS

//#define DEBUG_BUS_MQ

#define DD_DEBUG_PIPELINE_VERB2
#define DD_DEBUG_DCACHE_VERB101
#define DEBUG_BUS_MQ_VERB100
#define DD_DEBUG_BUS_MQ2_VERB100
#define DD_DEBUG_DCACHE_UPDATES_VERB100
#define DD_DEBUG_DCACHE_SNOOPING_VERB100
//#define DD_DEBUG_REORDER_VERB100
//#define DD_DEBUG_FETCH_VERB100
//#define DD_DEBUG_DCACHE_VERB101
//#define DD_DEBUG_DCACHE_VERB102

//#define DD_DEBUG_BUS_MQ2_VERB100
//#define DD_DEBUG_DCACHE_VERB2

//#define DEBUG_CACHEWB

//#define DD_DEBUG_DCACHE
//#define DD_DEBUG_SPLITTING
//#define DD_DEBUG_DCACHE_VERB2
//#define DD_DEBUG_FPLOAD
//#define DD_DEBUG_LSQ
//#define DD_DEBUG_BUS

//#define DD_DEBUG_DRAM
//#define DD_DEBUG_DRAM_DATATRANS
//#define DD_DEBUG_DRAM_DATATRANS_IN

// -------- PIPELINE STAGES ----------------------
//#define DD_DEBUG_PIPELINE_VERB

//#define DD_DEBUG_CPUPPCSS_VERB1
//#define DD_DEBUG_FETCH_VERB1
//#define DD_DEBUG_FETCH_VERB2
//#define DD_DEBUG_FETCH_VERB3
//#define DD_DEBUG_FETCH_BRANCH
//#define DD_DEBUG_FETCH_PREDECODE
//#define DD_DEBUG_PREDECODE
//#define DD_DEBUG_SPLITTING
//#define DD_DEBUG_FETCH_BHT_VERB2
//#define DD_DEBUG_SCHEDULER_VERB1
//#define DD_DEBUG_ALLOCATOR_VERB1
//#define DD_DEBUG_ALLOCATOR_RETIRE_VERB3
//#define DD_DEBUG_ALLOCATOR_RETIRE_VERB1
//#define DD_DEBUG_SCHEDULER_VERB1
//#define DD_DEBUG_FUNCTIONALUNIT_VERB1
//#define DD_DEBUG_LSQ
//#define DD_DEBUG_LSCONFLICT
//#define DD_DEBUG_MISALIGNED_LOAD
//#define DD_DEBUG_LSQ_VERB2
//#define DD_DEBUG_REORDERBUFFER_VERB1
//#define DD_DEBUG_REORDERBUFFER_VERB2
//#define DD_DEBUG_REORDERBUFFER_REPLAY1
//#define DD_DEBUG_FLUSH
// --------- SYSCALL -----------------------------
//#define DD_DEBUG_SYSCALL
//#define DD_DEBUG_SYSCALL_DCACHE_WM
//#define DD_DEBUG_SYSCALL_DCACHE_RM
//#define DD_DEBUG_SYSCALL_VERB1

//#define DEBUG_WRITEMEMORY_MEMORY_EMU
//#define DD_DEBUG_EMULATOR
//#define SAFE_MODE
//#define DD_DEBUG_SIGNALS

//#define DD_DEBUG_ALIGNMENT


//#define DD_DEBUG_LSCONFLICT_VERB2
/*
#define DD_DEBUG_LSCONFLICT
#define DD_DEBUG_FPSTORE
*/
//#define DD_DEBUG_LSQ_VERB3

//#define DD_DISPLAY_SIGNALS
#endif

// --------- TO USED !!!
//#define DD_CHECK_WITH_EMULATOR

//#define CHECK_REGISTER_STEP 10000000
//#define CHECK_REGISTER_STEP 1

#define SYSCALL_DISPATCH_WITHOUT_MIB

// Simulator paramters
//#include <unisim/component/clm/processor/ooosim_mc/parameters_mc_8core.hh>
#include <unisim/component/clm/processor/ooosim_mc/parameters.hh>
#include <unisim/component/clm/interfaces/instruction_interface.hh>


#include <unisim/component/clm/cache/cache_wb_mc.hh>
#include <unisim/component/clm/fsb/bus_multiqueue3_mc.hh>
#include <unisim/component/clm/memory/dram/dram2.hh>
#include <unisim/component/clm/processor/ooosim_mc/cpu_simulator_mc.hh>
#include <unisim/component/clm/processor/ooosim_mc/cpu_simulator_mc.tcc>
#include <unisim/kernel/kernel.hh>

// Includes for services


#include <unisim/service/loader/elf_loader/elf32_loader.hh>

#include <unisim/service/loader/linux_loader/linux_loader.hh>
#include "unisim/service/os/linux_os/linux_os.hh"

#include <unisim/service/power/cache_power_estimator.hh>
#include <unisim/util/garbage_collector/garbage_collector.hh>


/* Following includes have been moved into cpu_emulator.hh */
/*
#include <unisim/component/cxx/processor/powerpc/powerpc.hh>
#include <unisim/component/cxx/processor/powerpc/config.hh>
#include <unisim/component/cxx/processor/powerpc/powerpc.tcc>
#include <unisim/component/cxx/processor/powerpc/cpu.tcc>
#include <unisim/component/cxx/processor/powerpc/fpu.tcc>
*/

// Using components
using unisim::component::clm::fsb::BusMultiQueue;
using unisim::component::clm::memory::dram::DRAM;
using unisim::component::clm::cache::CacheWB;
using unisim::component::clm::processor::ooosim_mc::OooSimCpu;

//#ifdef interface
//#undef interface
//#endif
using unisim::component::clm::interfaces::InstructionPtr;

// Using Parameters
using unisim::component::clm::processor::ooosim_mc::nCPU;
using unisim::component::clm::processor::ooosim_mc::BUS_BufferSize;
using unisim::component::clm::processor::ooosim_mc::BUS_RequestWidth;
using unisim::component::clm::processor::ooosim_mc::Snooping;

using unisim::component::clm::processor::ooosim_mc::nProg;

using unisim::component::clm::processor::ooosim_mc::DRAM_nBanks;
using unisim::component::clm::processor::ooosim_mc::DRAM_nRows;
using unisim::component::clm::processor::ooosim_mc::DRAM_nCols;
using unisim::component::clm::processor::ooosim_mc::DRAM_TRRD;
using unisim::component::clm::processor::ooosim_mc::DRAM_TRAS;
using unisim::component::clm::processor::ooosim_mc::DRAM_TRCD;
using unisim::component::clm::processor::ooosim_mc::DRAM_CL;
using unisim::component::clm::processor::ooosim_mc::DRAM_TRP;
using unisim::component::clm::processor::ooosim_mc::DRAM_TRC;
using unisim::component::clm::processor::ooosim_mc::DRAM_TREF;
using unisim::component::clm::processor::ooosim_mc::DRAM_nDataPathSize;
using unisim::component::clm::processor::ooosim_mc::DRAM_nCacheLineSize;
using unisim::component::clm::processor::ooosim_mc::DRAM_nCtrlQueueSize;

using unisim::component::clm::processor::ooosim_mc::DL1_nCPUtoCacheDataPathSize;
using unisim::component::clm::processor::ooosim_mc::DL1_nCachetoCPUDataPathSize;
using unisim::component::clm::processor::ooosim_mc::DL1_nMemtoCacheDataPathSize;
using unisim::component::clm::processor::ooosim_mc::DL1_nCachetoMemDataPathSize;
using unisim::component::clm::processor::ooosim_mc::DL1_nLineSize;
using unisim::component::clm::processor::ooosim_mc::DL1_nCacheLines;
using unisim::component::clm::processor::ooosim_mc::DL1_nAssociativity;
using unisim::component::clm::processor::ooosim_mc::DL1_nStages;
using unisim::component::clm::processor::ooosim_mc::DL1_nDelay;

using unisim::component::clm::processor::ooosim_mc::IL1_nCPUtoCacheDataPathSize;
using unisim::component::clm::processor::ooosim_mc::IL1_nCachetoCPUDataPathSize;
using unisim::component::clm::processor::ooosim_mc::IL1_nMemtoCacheDataPathSize;
using unisim::component::clm::processor::ooosim_mc::IL1_nCachetoMemDataPathSize;
using unisim::component::clm::processor::ooosim_mc::IL1_nLineSize;
using unisim::component::clm::processor::ooosim_mc::IL1_nCacheLines;
using unisim::component::clm::processor::ooosim_mc::IL1_nAssociativity;
using unisim::component::clm::processor::ooosim_mc::IL1_nStages;
using unisim::component::clm::processor::ooosim_mc::IL1_nDelay;

using unisim::component::clm::processor::ooosim_mc::nIntegerRegisters;
//using unisim::component::clm::processor::ooosim_mc::IL1_nCachetoCPUDataPathSize;
//using unisim::component::clm::processor::ooosim_mc::IL1_nCPUtoCacheDataPathSize;
//using unisim::component::clm::processor::ooosim_mc::DL1_nCachetoCPUDataPathSize;
//using unisim::component::clm::processor::ooosim_mc::DL1_nCPUtoCacheDataPathSize;

/*
using unisim::component::clm::processor::ooosim_mc::;
using unisim::component::clm::processor::ooosim_mc::;
using unisim::component::clm::processor::ooosim_mc::;
using unisim::component::clm::processor::ooosim_mc::;
using unisim::component::clm::processor::ooosim_mc::;
*/

/*
using unisim::component::clm::;
using unisim::component::clm::;
using unisim::component::clm::;
*/

// Using for services
using unisim::util::endian::E_BIG_ENDIAN;

using unisim::service::loader::elf_loader::Elf32Loader;
using unisim::service::loader::linux_loader::LinuxLoader;

using unisim::service::os::linux_os::LinuxOS;


using unisim::service::power::CachePowerEstimator;
using unisim::util::garbage_collector::GarbageCollector;
using unisim::kernel::ServiceManager;


const int nConfig = 8;


class GeneratedSimulator : public Simulator{
public:

  /**************************************************************************
   *                      DEFINITION OF CLM COMPONENTS                      *
   **************************************************************************/

  //  typedef StopAtCycle<100000000> cSAC;

  // cBUS
  /*
    #define INSTRUCTION Instruction
    #define nCPU 2
    #define BufferSize 3
    #define RequestWidth 32
    #define Snooping 0
  */
  typedef BusMultiQueue<InstructionPtr,nConfig,BUS_BufferSize,BUS_RequestWidth,Snooping> cBUS;
  /*
  // cDRAM
#define INSTRUCTION Instruction
#define nBanks 4
#define nRows 8192
#define nCols 1024
#define TRRD 2
#define TRAS 5
#define TRCD 2
#define CL 2
#define TRP 2
#define TRC 7
#define TREF 10000000
#define nDataPathSize 32
#define nCacheLineSize 32
#define nCtrlQueueSize 16
#define nProg 1
#define Snooping 0
  */
  typedef DRAM<InstructionPtr,
	       DRAM_nBanks,
	       DRAM_nRows,
	       DRAM_nCols,
	       DRAM_TRRD,
	       DRAM_TRAS,
	       DRAM_TRCD,
	       DRAM_CL,
	       DRAM_TRP,
	       DRAM_TRC,
	       DRAM_TREF,
	       DRAM_nDataPathSize,
	       DRAM_nCacheLineSize,
	       DRAM_nCtrlQueueSize,
	       nProg,
	       Snooping> cDRAM;

  /*
  // cDCACHE
#define INSTRUCTION Instruction
#define nCPUtoCacheDataPathSize 8
#define nCachetoCPUDataPathSize 8
#define nCPULineSize 0
#define nMemtoCacheDataPathSize 32
#define nCachetoMemDataPathSize 32
#define nLineSize 32
#define nCacheLines 128
#define nAssociativity 2
#define nStages 1
#define nDelay 1
#define nProg 1
#define Snooping 0
  */
  typedef CacheWB<InstructionPtr,
		  DL1_nCPUtoCacheDataPathSize,
		  DL1_nCachetoCPUDataPathSize,
		  //		  DL1_nCPULineSize,
		  DL1_nMemtoCacheDataPathSize,
		  DL1_nCachetoMemDataPathSize,
		  DL1_nLineSize,
		  DL1_nCacheLines,
		  DL1_nAssociativity,
		  DL1_nStages,
		  DL1_nDelay,
		  nProg,
		  Snooping,
		  false,
		  nConfig > cDCACHE;

  /*
  // cICACHE
#define INSTRUCTION Instruction
#define nCPUtoCacheDataPathSize 8
#define nCachetoCPUDataPathSize 8
#define nCPULineSize 0
#define nMemtoCacheDataPathSize 32
#define nCachetoMemDataPathSize 32
#define nLineSize 32
#define nCacheLines 128
#define nAssociativity 2
#define nStages 1
#define nDelay 1
#define nProg 1
#define Snooping 0
  */
  //  typedef CacheWB<Instruction,8,8,0,32,32,32,128,2,1,1,1,0> cICACHE;
  typedef CacheWB<InstructionPtr,
		  IL1_nCPUtoCacheDataPathSize,
		  IL1_nCachetoCPUDataPathSize,
		  //		  IL1_nCPULineSize,
		  IL1_nMemtoCacheDataPathSize,
		  IL1_nCachetoMemDataPathSize,
		  IL1_nLineSize,
		  IL1_nCacheLines,
		  IL1_nAssociativity,
		  IL1_nStages,
		  IL1_nDelay,
		  nProg,
		  Snooping,
		  false,
		  nConfig > cICACHE;

  /*
  // cCPU
#define nIntegerRegisters 32
#define nIL1CachetoCPUDataPathSize 8
// ?
//#define nIL1CachetoMemDataPathSize 32
//#define nIL1MemtoCacheDataPathSize 32
//#define nIL1LineSize 32
//#define nIL1CacheLines 128
//#define nIL1Associativity 2
//#define nDL1LineSize 32
#define nDL1CachetoCPUDataPathSize 8
#define nDL1CPUtoCacheDataPathSize 8
#define nProg 1
  */
  typedef OooSimCpu<nIntegerRegisters,IL1_nCachetoCPUDataPathSize,IL1_nCPUtoCacheDataPathSize,DL1_nCachetoCPUDataPathSize,DL1_nCPUtoCacheDataPathSize,nProg, false, nConfig> cCPU;

  // Unisim modules
  //  cSAC *__sac;
  cBUS *__bus;
  cDRAM *__dram;
  cDCACHE *__dcache;
  cICACHE *__icache;
  cCPU *__cpu;

  /**************************************************************************
   *                      CLM COMPONENT GENERATION and CONNECTION           *
   **************************************************************************/
  GeneratedSimulator() {
#ifdef STOP
    //    __sac = new cSAC("sac");
    //    sac->clock(global_clock);
#endif

    // Module instantiactions
    __bus = new cBUS("__bus");
    __dram = new cDRAM("__dram");
    __dcache = new cDCACHE("__cache_mc");
    __icache = new cICACHE("__icache_mc");
    __cpu = new cCPU("__ppc_mc");

    // Clock connections
    __bus->clock(global_clock);
    __dram->inClock(global_clock);
    __dcache->inClock(global_clock);
    __icache->inClock(global_clock);
    __cpu->inClock(global_clock);
    /*
    __cpu->fetch->inClock(global_clock);
    __cpu->allocate->inClock(global_clock);
    __cpu->schedule->inClock(global_clock);
    __cpu->registerfile->inClock(global_clock);
    __cpu->iu->inClock(global_clock);
    __cpu->fpu->inClock(global_clock);
    __cpu->agu->inClock(global_clock);
    __cpu->lsq->inClock(global_clock);
    __cpu->rob->inClock(global_clock);
    */

   // SIGNAL (or Module) connections
    __dram->out >> __bus->inMEM;
    __bus->outMEM >> __dram->in;

    __dcache->outMEM >> __bus->inDataCPU;
    __icache->outMEM >> __bus->inInstCPU;
    __bus->outDataCPU >> __dcache->inMEM;
    __bus->outInstCPU >> __icache->inMEM;
    /*
    __dcache->outCPU >> __cpu->inDL1Data;
    __icache->outCPU >> __cpu->inIL1Data;
    __cpu->outDL1Data >> __dcache->inCPU;
    //__cpu->lsq->outDL1[0] >> __dcache->inCPU;
    __cpu->outIL1Data >> __icache->inCPU;
    */
    __dcache->outCPU >> __cpu->lsq->inDL1;
    __icache->outCPU >> __cpu->fetch->inIL1;
    __cpu->lsq->outDL1 >> __dcache->inCPU;
    //__cpu->lsq->outDL1[0] >> __dcache->inCPU;
    __cpu->fetch->outIL1 >> __icache->inCPU;

    // Shared signal connexions
    __icache->outSharedMEM >> __bus->inInstSharedCPU;
    __dcache->outSharedMEM >> __bus->inDataSharedCPU;
    __bus->outSharedMEM >> __dram->inShared;

    // Service instantiations
    //    PPCLinux = new PowerPCLinux("my-ppclinux",0);
    // David : MIB no more usefull
    //    MIB = new MemoryInterfaceBroadcast<address_t>("MIB",0);



  } //GeneratedSimulator()

  // Services
  ///////////////////////////////////////// "From Emulator"  Start ///////////////////////
  // ---------------------
  // Root Emulator Objects
  // ---------------------
  // CPU
  //	full_system::processors::powerpc::powerpc755::CPU mycpu("MyCpu0",0);
  // Memory System
  //	MyMemEmulator mymem("MyMemorySystem", 0);
  // PPC Linux OS
  //  PowerPCLinux *PPCLinux;
  
  // David : Because, syscall are duplicated from CPU emulator and sent to the memory hierarchy,
  //         the memory broadcaster is no more useful.
  //  MemoryInterfaceBroadcast<address_t> *MIB;
  


  
  /**************************************************************************
   *                      DEFINITION OF SERVICE COMPONENTS                  *
   **************************************************************************/
  
  //=========================================================================
  //===                       Constants definitions                       ===
  //=========================================================================
  // Front Side Bus template parameters
  typedef CPU_CONFIG::address_t CPU_ADDRESS_TYPE;
  typedef CPU_ADDRESS_TYPE FSB_ADDRESS_TYPE;
  typedef CPU_ADDRESS_TYPE MEMORY_ADDRESS_TYPE;
  typedef CPU_CONFIG::reg_t CPU_REG_TYPE;
  static const uint32_t FSB_MAX_DATA_SIZE = 32;        // in bytes
  static const uint32_t FSB_NUM_PROCS = 1;
  //=========================================================================
  //===                     Aliases for components classes                ===
  //=========================================================================
  //typedef unisim::component::tlm::fsb::snooping_bus::Bus<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, 1> FRONT_SIDE_BUS;
  //typedef unisim::component::tlm::bridge::snooping_fsb_to_mem::Bridge<unisim::component::tlm::bridge::snooping_fsb_to_mem::Addr32BurstSize32_Config> FSB_TO_MEM_BRIDGE;
  typedef unisim::component::cxx::memory::ram::Memory<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> MEMORY;
  
  


  /**************************************************************************
   *               GENERATION and CONNECTION OF SERVICE COMPONENTS          *
   **************************************************************************/
  void ServiceConnection(
			 /*


			 const char *filename,
			 int sim_argc,
			 //	char **sim_argv,
			 //list<string> sim_argv,


			 bool kernel_mode,
			 uint32_t video_refresh_period,

			 char *device_tree_filename,

			 uint64_t maxinst
			 */
	)
  {


	int c;


	bool estimate_power = false;


	uint64_t maxinst = 0; // maximum number of instruction to simulate
	double cpu_frequency = 300.0; // in Mhz
	uint32_t cpu_clock_multiplier = 4;
	uint32_t tech_node = 130; // in nm
	double cpu_ipc = 1.0; // in instructions per cycle
	uint64_t cpu_cycle_time = (uint64_t)(1e6 / cpu_frequency); // in picoseconds
	uint64_t fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	uint32_t mem_cycle_time = fsb_cycle_time;

	/*
	const char *filename =  command_line[0];
	int sim_argc = command_line.count();
	*/
	const char *filenames[nConfig];
	int sim_argc[nConfig]; 

	int sim_argc_start[nConfig]; 
	int sim_argc_stop[nConfig]; 
	
	for(int i=0; i<nConfig; i++) { sim_argc_start[i] = 0; }
	for(int i=0; i<nConfig; i++) { sim_argc_stop[i] = 0; }
	for(int i=0; i<nConfig; i++) { sim_argc[i] = 0; }

	{
	  int i=0;
	  int cfg=-1;
	  while (i<command_line.count())
	  {
	    if (strcmp(command_line[i],"bench")==0)
	      {
		if(cfg>=0)
		  {
		    sim_argc_stop[cfg] = i-1;
		  }
		i++;
		cfg++;
		filenames[cfg] = command_line[i];
		sim_argc_start[cfg] = i;
		sim_argc[cfg]++;
	      }
	    else
	      {
		if(cfg>=0)
		  {
		    sim_argc[cfg]++;
		  }
	      }
	    i++;
	  }
	}
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
        // CPU will be instantied into Fetch, FUs and ReoderBuffer.
 	//  - PowerPC processor
    //	CPUEmu *cpu_emu =new CPUEmu("cpu_emu");
	//  - Front side bus
    //	FRONT_SIDE_BUS *bus = new FRONT_SIDE_BUS("bus");
	//  - Front side bus to memory bridge
    //	FSB_TO_MEM_BRIDGE *fsb_to_mem_bridge = new FSB_TO_MEM_BRIDGE("fsb-to-mem-bridge");
	//  - RAM
	//	MEMORY *memory = new MEMORY("memory");

	unisim::component::cxx::memory::ram::Memory<address_t> *memory = __dram->memory_emulator;

	//	for(int cfg=0; cfg<nConfig; cfg++)
	//	  { 
	//	unisim::component::clm::processor::ooosim_mc::CPUEmu *cpu = __cpu->check_emulator[cfg];
	unisim::component::clm::processor::ooosim_mc::CPUEmu *cpu[nConfig];// = __cpu->check_emulator[0];

	for(int cfg=0; cfg<nConfig; cfg++)
	  { 
	    cpu[cfg] = __cpu->check_emulator[cfg];
	  }

	//=========================================================================
	//===            Debugging stuff: Transaction spy instantiations        ===
	//=========================================================================
	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - ELF32 loader
	//	Elf32Loader *elf32_loader = new Elf32Loader("elf32-loader");
	Elf32Loader *elf32_loader[nConfig];
	for(int cfg=0; cfg<nConfig; cfg++)
	  {
	    stringstream str;
	    str << "elf32-loader[" << cfg << "]";
	    elf32_loader[cfg] = new Elf32Loader(str.str().c_str());
	  }
	//  - Linux loader
	//	LinuxLoader<FSB_ADDRESS_TYPE> *linux_loader = new LinuxLoader<FSB_ADDRESS_TYPE>("linux-loader");
	LinuxLoader<FSB_ADDRESS_TYPE> *linux_loader[nConfig];
	for(int cfg=0; cfg<nConfig; cfg++)
	  {
	    stringstream str;
	    str << "linux-loader[" << cfg << "]";
	    linux_loader[cfg] = new LinuxLoader<FSB_ADDRESS_TYPE>(str.str().c_str());
	  }

	//  - Symbol table
	
	//  - Linux OS
	//	LinuxOS<CPU_ADDRESS_TYPE, CPU_REG_TYPE> *linux_os = new LinuxOS<CPU_ADDRESS_TYPE, CPU_REG_TYPE>("linux_os");
	LinuxOS<CPU_ADDRESS_TYPE, CPU_REG_TYPE> *linux_os[nConfig];
	for(int cfg=0; cfg<nConfig; cfg++)
	  {
	    stringstream str;
	    str << "linux_os[" << cfg << "]";
	    linux_os[cfg] = new LinuxOS<CPU_ADDRESS_TYPE, CPU_REG_TYPE>(str.str().c_str());
	  }

	//  - GDB server

	//  - Inline debugger

	//  - SystemC Time
	//	unisim::service::time::sc_time::ScTime *time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	//	unisim::service::time::host_time::HostTime *host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - the optional power estimators
	CachePowerEstimator *il1_power_estimator = estimate_power ? new CachePowerEstimator("il1-power-estimator") : 0;
	CachePowerEstimator *dl1_power_estimator = estimate_power ? new CachePowerEstimator("dl1-power-estimator") : 0;
	CachePowerEstimator *l2_power_estimator = estimate_power ? new CachePowerEstimator("l2-power-estimator") : 0;
	CachePowerEstimator *itlb_power_estimator = estimate_power ? new CachePowerEstimator("itlb-power-estimator") : 0;
	CachePowerEstimator *dtlb_power_estimator = estimate_power ? new CachePowerEstimator("dtlb-power-estimator") : 0;
	
	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

        for(int cfg=0; cfg<nConfig; cfg++)
          {
            (*cpu[cfg])["cpu-cycle-time"] = cpu_cycle_time;
            (*cpu[cfg])["bus-cycle-time"] = fsb_cycle_time;
            (*cpu[cfg])["voltage"] = 1.0 * 1e3; // mV
          }

        (*(__cpu->fetch_emulator))["cpu-cycle-time"] = cpu_cycle_time;
        (*(__cpu->fetch_emulator))["bus-cycle-time"] = fsb_cycle_time;
        (*(__cpu->fetch_emulator))["voltage"] = 1.0 * 1e3; //mV

        (*(__cpu->speculative_cpu_state))["cpu-cycle-time"] = cpu_cycle_time;
        (*(__cpu->speculative_cpu_state))["bus-cycle-time"] = fsb_cycle_time;
        (*(__cpu->speculative_cpu_state))["voltage"] = 1.0 * 1e3; //mV

	//  - Front Side Bus
	//	(*bus)["cycle-time"] = fsb_cycle_time;

	//  - PowerPC processor
	// if the following line ("cpu-cycle-time") is commented, the cpu will use the power estimators to find min cpu cycle time
	/*
	(*cpu)["cpu-cycle-time"] = cpu_cycle_time;
	(*cpu)["bus-cycle-time"] = fsb_cycle_time;
	(*cpu)["voltage"] = 1.3 * 1e3; // mV
	if(maxinst)
	{
		(*cpu)["max-inst"] = maxinst;
	}
	(*cpu)["ipc"] = cpu_ipc;

	//  - Front side bus to memory bridge
	(*fsb_to_mem_bridge)["fsb-cycle-time"] = fsb_cycle_time;
	(*fsb_to_mem_bridge)["mem-cycle-time"] = mem_cycle_time;
	//  - RAM
	(*memory)["cycle-time"] = mem_cycle_time;
	(*memory)["org"] = 0x00000000UL;
	(*memory)["bytesize"] = (uint32_t)-1;

	*/
	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - GDB Server run-time configuration

	{


	}
	//  - ELF32 Loader run-time configuration
	//	(*elf32_loader)["filename"] = filename;
	for(int cfg=0; cfg<nConfig; cfg++)
	  {
	    (*elf32_loader[cfg])["filename"] = filenames[cfg];
	  }

	//  - Linux loader run-time configuration
	for(int cfg=0; cfg<nConfig; cfg++)
	  {
	    (*linux_loader[cfg])["endianess"] = E_BIG_ENDIAN;
	    (*linux_loader[cfg])["stack-base"] = 0xc0000000+cfg*0x01000000;
	    (*linux_loader[cfg])["max-environ"] = 16 * 1024;
	    //	(*linux_loader)["argc"] = sim_argc;
	    (*linux_loader[cfg])["argc"] = sim_argc[cfg];//command_line.count();
	    for(unsigned int i = 0; i < sim_argc[cfg]; i++)
	      {
		//	(*linux_loader)["argv"][i] = sim_argv[i];
		(*linux_loader[cfg])["argv"][i] = command_line[sim_argc_start[cfg]+i];
	      }
	    (*linux_loader[cfg])["envc"] = 0;
	  }

	//  - Linux OS run-time configuration
	for(int cfg=0; cfg<nConfig; cfg++)
	  {
	    (*linux_os[cfg])["system"] = "powerpc";
	    (*linux_os[cfg])["endianess"] = E_BIG_ENDIAN;
	    (*linux_os[cfg])["verbose"] = false;
	  }

	//  - Cache/TLB power estimators run-time configuration
	/*
	if(estimate_power)
	{
		(*il1_power_estimator)["cache-size"] = 32 * 1024;
		(*il1_power_estimator)["line-size"] = 32;
		(*il1_power_estimator)["associativity"] = 8;
		(*il1_power_estimator)["rw-ports"] = 0;
		(*il1_power_estimator)["excl-read-ports"] = 1;
		(*il1_power_estimator)["excl-write-ports"] = 0;
		(*il1_power_estimator)["single-ended-read-ports"] = 0;
		(*il1_power_estimator)["banks"] = 4;
		(*il1_power_estimator)["tech-node"] = tech_node;
		(*il1_power_estimator)["output-width"] = 128;
		(*il1_power_estimator)["tag-width"] = 64;
		(*il1_power_estimator)["access-mode"] = "fast";
	
		(*dl1_power_estimator)["cache-size"] = 32 * 1024;
		(*dl1_power_estimator)["line-size"] = 32;
		(*dl1_power_estimator)["associativity"] = 8;
		(*dl1_power_estimator)["rw-ports"] = 1;
		(*dl1_power_estimator)["excl-read-ports"] = 0;
		(*dl1_power_estimator)["excl-write-ports"] = 0;
		(*dl1_power_estimator)["single-ended-read-ports"] = 0;
		(*dl1_power_estimator)["banks"] = 4;
		(*dl1_power_estimator)["tech-node"] = tech_node;
		(*dl1_power_estimator)["output-width"] = 64;
		(*dl1_power_estimator)["tag-width"] = 64;
		(*dl1_power_estimator)["access-mode"] = "fast";
	
		(*l2_power_estimator)["cache-size"] = 512 * 1024;
		(*l2_power_estimator)["line-size"] = 32;
		(*l2_power_estimator)["associativity"] = 8;
		(*l2_power_estimator)["rw-ports"] = 1;
		(*l2_power_estimator)["excl-read-ports"] = 0;
		(*l2_power_estimator)["excl-write-ports"] = 0;
		(*l2_power_estimator)["single-ended-read-ports"] = 0;
		(*l2_power_estimator)["banks"] = 4;
		(*l2_power_estimator)["tech-node"] = tech_node;
		(*l2_power_estimator)["output-width"] = 256;
		(*l2_power_estimator)["tag-width"] = 64;
		(*l2_power_estimator)["access-mode"] = "fast";
	
		(*itlb_power_estimator)["cache-size"] = 128 * 2 * 4;
		(*itlb_power_estimator)["line-size"] = 4;
		(*itlb_power_estimator)["associativity"] = 2;
		(*itlb_power_estimator)["rw-ports"] = 1;
		(*itlb_power_estimator)["excl-read-ports"] = 0;
		(*itlb_power_estimator)["excl-write-ports"] = 0;
		(*itlb_power_estimator)["single-ended-read-ports"] = 0;
		(*itlb_power_estimator)["banks"] = 4;
		(*itlb_power_estimator)["tech-node"] = tech_node;
		(*itlb_power_estimator)["output-width"] = 32;
		(*itlb_power_estimator)["tag-width"] = 64;
		(*itlb_power_estimator)["access-mode"] = "fast";
	
		(*dtlb_power_estimator)["cache-size"] = 128 * 2 * 4;
		(*dtlb_power_estimator)["line-size"] = 4;
		(*dtlb_power_estimator)["associativity"] = 2;
		(*dtlb_power_estimator)["rw-ports"] = 1;
		(*dtlb_power_estimator)["excl-read-ports"] = 0;
		(*dtlb_power_estimator)["excl-write-ports"] = 0;
		(*dtlb_power_estimator)["single-ended-read-ports"] = 0;
		(*dtlb_power_estimator)["banks"] = 4;
		(*dtlb_power_estimator)["tech-node"] = tech_node;
		(*dtlb_power_estimator)["output-width"] = 32;
		(*dtlb_power_estimator)["tag-width"] = 64;
		(*dtlb_power_estimator)["access-mode"] = "fast";
	}
	*/
	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	/*
	if(logger_on && logger_messages)
	{
		unsigned bus_msg_spy_index = 0;
		unsigned mem_msg_spy_index = 0;

		cpu->bus_port(bus_msg_spy[bus_msg_spy_index]->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["source_module_name"] = cpu->name();
		(*bus_msg_spy[bus_msg_spy_index])["source_port_name"] = cpu->bus_port.name();
		bus_msg_spy[bus_msg_spy_index]->master_port(*bus->inport[0]);
		(*bus_msg_spy[bus_msg_spy_index])["target_module_name"] = bus->name();
		(*bus_msg_spy[bus_msg_spy_index])["target_port_name"] = bus->inport[0]->name();
		bus_msg_spy_index++;

		(*bus->outport[0])(bus_msg_spy[bus_msg_spy_index]->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["source_module_name"] = bus->name();
		(*bus_msg_spy[bus_msg_spy_index])["source_port_name"] = bus->outport[0]->name();
		bus_msg_spy[bus_msg_spy_index]->master_port(cpu->snoop_port);
		(*bus_msg_spy[bus_msg_spy_index])["target_module_name"] = cpu->name();
		(*bus_msg_spy[bus_msg_spy_index])["target_port_name"] = cpu->snoop_port.name();
		bus_msg_spy_index++;

		(*bus->chipset_outport)(bus_msg_spy[bus_msg_spy_index]->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["source_module_name"] = bus->name();
		(*bus_msg_spy[bus_msg_spy_index])["source_port_name"] = bus->chipset_outport->name();
		bus_msg_spy[bus_msg_spy_index]->master_port(fsb_to_mem_bridge->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["target_module_name"] = fsb_to_mem_bridge->name();
		(*bus_msg_spy[bus_msg_spy_index])["target_port_name"] = fsb_to_mem_bridge->slave_port.name();
		bus_msg_spy_index++;

		fsb_to_mem_bridge->master_port(mem_msg_spy[mem_msg_spy_index]->slave_port);
		(*mem_msg_spy[mem_msg_spy_index])["source_module_name"] = fsb_to_mem_bridge->name();
		(*mem_msg_spy[mem_msg_spy_index])["source_port_name"] = fsb_to_mem_bridge->master_port.name();
		mem_msg_spy[mem_msg_spy_index]->master_port(memory->slave_port);
		(*mem_msg_spy[mem_msg_spy_index])["target_module_name"] = memory->name();
		(*mem_msg_spy[mem_msg_spy_index])["target_port_name"] = memory->slave_port.name();
		mem_msg_spy_index++;
	}
	else
	{
		cpu->bus_port(*bus->inport[0]);
		(*bus->outport[0])(cpu->snoop_port);
		(*bus->chipset_outport)(fsb_to_mem_bridge->slave_port);
		fsb_to_mem_bridge->master_port(memory->slave_port);
	}
	*/

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	//	cpu->memory_import >> bus->memory_export;
	//	cpu->memory_import >> memory->memory_export;
	//	cpu->memory_import >> memory->memory_export;
	for (int cfg=0; cfg<nConfig; cfg++)
	  {
	    cpu[cfg]->memory_import >> memory->memory_export;
	    
	    //	cpu->memory_import >> __dcache->syscall_MemExp;
	    //	__dcache->syscall_MemImp >> __dram->syscall_MemExp;
	    cpu[cfg]->memory_injection_import >> __dcache->memory_injection_export;
	  }
	__dcache->memory_injection_import >> __dram->memory_injection_export;

	/*

	{








			cpu->memory_access_reporting_control_export;
	}

	{







			cpu->memory_access_reporting_control_export;
	}

	if(estimate_power)
	{
		// Connect everything related to power estimation
		cpu->il1_power_estimator_import >> il1_power_estimator->power_estimator_export;
		cpu->il1_power_mode_import >> il1_power_estimator->power_mode_export;
		cpu->dl1_power_estimator_import >> dl1_power_estimator->power_estimator_export;
		cpu->dl1_power_mode_import >> dl1_power_estimator->power_mode_export;
		cpu->l2_power_estimator_import >> l2_power_estimator->power_estimator_export;
		cpu->l2_power_mode_import >> l2_power_estimator->power_mode_export;
		cpu->itlb_power_estimator_import >> itlb_power_estimator->power_estimator_export;
		cpu->itlb_power_mode_import >> itlb_power_estimator->power_mode_export;
		cpu->dtlb_power_estimator_import >> dtlb_power_estimator->power_estimator_export;
		cpu->dtlb_power_mode_import >> dtlb_power_estimator->power_mode_export;
	*/
		/*
		il1_power_estimator->time_import >> time->time_export;
		dl1_power_estimator->time_import >> time->time_export;
		l2_power_estimator->time_import >> time->time_export;
		itlb_power_estimator->time_import >> time->time_export;
		dtlb_power_estimator->time_import >> time->time_export;
		*/
	//	}

	//	elf32_loader->memory_import >> memory->memory_export;
	//	elf32_loader->memory_import >> __dram->syscall_MemExp;
        for (int cfg=0; cfg<nConfig; cfg++)
        {
	  elf32_loader[cfg]->memory_import >> __dram->memory_export;
	  
	  //	linux_loader->memory_import >> memory->memory_export;
	  linux_loader[cfg]->memory_import >> __dram->memory_export;
	  linux_loader[cfg]->loader_import >> elf32_loader[cfg]->loader_export;
	  cpu[cfg]->linux_os_import >> linux_os[cfg]->linux_os_export;

	  linux_os[cfg]->cpu_linux_os_import >> cpu[cfg]->cpu_linux_os_export;
	  linux_os[cfg]->memory_import >> cpu[cfg]->memory_export;
	  linux_os[cfg]->memory_injection_import >> cpu[cfg]->memory_injection_export;
	  linux_os[cfg]->registers_import >> cpu[cfg]->registers_export;
	  linux_os[cfg]->loader_import >> linux_loader[cfg]->loader_export;

	  cpu[cfg]->symbol_table_lookup_import >> elf32_loader[cfg]->symbol_table_lookup_export;
	}
	//	linux_os->cpu_linux_os_import >> __cpu->fetch_emulator->cpu_linux_os_export;

	//	bus->memory_import >> fsb_to_mem_bridge->memory_export;
	//	fsb_to_mem_bridge->memory_import >> memory->memory_export;


	/* logger connections */
  /*
	if(logger_on) {
		unsigned int logger_index = 0;
		//		logger->time_import >> time->time_export;
		
		
		
		//		bus->logger_import >> *logger->logger_export[logger_index++];
		//		fsb_to_mem_bridge->logger_import >> *logger->logger_export[logger_index++];
		//		memory->logger_import >> *logger->logger_export[logger_index++];

		linux_os->logger_import >> *logger->logger_export[logger_index++];
  */
		/*
		for(unsigned int i = 0; i < MAX_BUS_TRANSACTION_SPY; i++)
			if(bus_msg_spy[i] != NULL)
				bus_msg_spy[i]->logger_import >> *logger->logger_export[logger_index++];
		for(unsigned int i = 0; i < MAX_MEM_TRANSACTION_SPY; i++)
			if(mem_msg_spy[i] != NULL)
				mem_msg_spy[i]->logger_import >> *logger->logger_export[logger_index++];
		*/
  /*
	}
  */
#ifdef DEBUG_SERVICE
	ServiceManager::Dump(cerr);
#endif

	////////////////////////////////////////////////////////
	// END OF NEW SERVICE CONNECTIONS
	////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////
	// START OF OLD SERVICE CONNECTIONS
	////////////////////////////////////////////////////////

    
    // Starting Service Manager ...
    //ServiceManager::Dump(cout);
    if(ServiceManager::Setup())
      {
	cerr << "Starting simulation at user privilege level (Linux system calls translation enabled)" << endl;
      }
    //    ServiceManager::Dump(cout);
    __cpu->Reset();
    //ServiceManager::Dump(cout);
    ///////////////////////////////////////// "From Emulator"  End ///////////////////////
  } // ServiceConnection(...)

  void ServiceDeconnection()
  {
  }
  
}; //class GeneratedSimulator
