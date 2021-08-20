/***************************************************************************
             CacheWB.sim  -  cycle-level Write-back cache module
***************************************************************************/

/* *****************************************************************************  
                                    BSD LICENSE  
********************************************************************************  
Copyright (c) 2006, Commissariat a l'Energie Atomique (CEA) & INRIA
Authors: Daniel Gracia Pérez (daniel.gracia-perez@cea.fr) 
         Sylvain Girbal (sylvain.girbal@inria.fr)
         David Parello (david.parello@univ-perp.fr)
Foundings: Partly founded with HiPEAC foundings
All rights reserved.  
  
Redistribution and use in source and binary forms, with or without modification,   
are permitted provided that the following conditions are met:  
  
 - Redistributions of source code must retain the above copyright notice, this   
   list of conditions and the following disclaimer.   
  
 - Redistributions in binary form must reproduce the above copyright notice,   
   this list of conditions and the following disclaimer in the documentation   
   and/or other materials provided with the distribution.   
   
 - Neither the name of the UNISIM nor the names of its contributors may be   
   used to endorse or promote products derived from this software without   
   specific prior written permission.   
     
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED   
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE   
DISCLAIMED.   
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,   
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,   
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY   
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING   
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,   
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  
***************************************************************************** */

#ifndef __UNISIM_COMPONENT_CLM_CACHE_CACHE_WB_MC_HH__
#define __UNISIM_COMPONENT_CLM_CACHE_CACHE_WB_MC_HH__

#include <unisim/kernel/clm/engine/unisim.h>
#include <unisim/component/clm/memory/mem_common.hh>
#include <unisim/component/clm/interfaces/memreq_mc.hh>
#include <unisim/component/clm/utility/common.hh>
#include <unisim/component/clm/utility/utility.hh>
#include <unisim/component/clm/utility/error.h>
#include <unisim/component/clm/cache/cache_container.hh>
#include <unisim/component/clm/cache/cache_common_mc.hh>

#include <unisim/service/interfaces/memory_injection.hh>

#include <unisim/component/clm/debug/svg_memreq_interface.hh>
#include <unisim/kernel/kernel.hh>

// Interface to Cacti 4
//#include "power_estimation2/CactiCachePowerEstimator.h"
//#include "statistics/StatisticService.h"
//#include <unisim/service/interfaces/statistic_reporting.hh>

#include <sstream>

namespace unisim {
namespace component {
namespace clm {
namespace cache {

  // new usings for merge
using unisim::component::clm::interfaces::memreq;
  //using unisim::service::interfaces::StatisticReporting;
using unisim::component::clm::utility::hexa;
using unisim::component::clm::debug::SVGmemreqInterface;

using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;


  //using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;

  // old using...
  /*
using full_system::utils::services::ServiceExport;
using full_system::utils::services::ServiceImport;
using full_system::utils::services::Service;
using full_system::utils::services::Client;
using full_system::utils::services::Object;
using full_system::generic::memory::MemoryInterface;
using full_system::plugins::SVGmemreqInterface;
  */
typedef Client< MemoryInjection<address_t> > MJI_Client;
typedef Service< MemoryInjection<address_t> > MJI_Service;

#ifdef CACHE_CHECK_COHERENCY
static List < module > all_caches;
#endif

//#define DD_DEBUG_DCACHE

/**
 * \brief Single port, Blocking, WriteBack cache module
 */
template 
< class INSTRUCTION,
//  ReplacementPolicyType replacementPolicy,   ///< replacement policy to apply (random, lru, pseudoLRU)
  int nCPUtoCacheDataPathSize,               ///< max data size in bytes that can receive from the CPU
  int nCachetoCPUDataPathSize,               ///< max data size in bytes that can send towards the CPU
  int nMemtoCacheDataPathSize,               ///< max data size in bytes that can be received from the memory system
  int nCachetoMemDataPathSize,               ///< max data size in bytes that can send towards the memory system, used in the case of write back policy
  int nLineSize,                             ///< size of a cache line in bytes
  int nCacheLines,                           ///< number of cache lines
  int nAssociativity,                        ///< associtivity level, if nAssoc==nCacheLines complete associative, if nAssoc==1 direct
  int nStages,                               ///< number of pipe stages, number of cycles. if nStages == 1 the cache is not pipelined
  int nDelay,                                ///< the delay cycles f each pipeline stage
  int nProg,
  bool Snooping = false,
  bool VERBOSE = false,
  uint32_t nConfig=2
>
class CacheWB : public module
//              , public Client < Memory< address_t > >
//              , public Service < Memory < address_t > >
//              , public Client < MemoryInjection< address_t > >
  , public MJI_Client
  //              , public Service < MemoryInjection< address_t > >
  , public MJI_Service
              , public Client < SVGmemreqInterface<INSTRUCTION> >
//              , public Service < PowerEstimatorInterface >
//		, public StatisticService
//		, public Client<StatisticReporting>
//		, public Service<StatisticReportingControl>
{private:

  typedef CacheWB < INSTRUCTION, nCPUtoCacheDataPathSize, nCachetoCPUDataPathSize, nMemtoCacheDataPathSize, 
                    nCachetoMemDataPathSize, nLineSize, nCacheLines, nAssociativity, nStages, nDelay,
		    nProg, Snooping, VERBOSE, nConfig > this_class;

  /* --------------------------------------------------------------- */
  /*  Module state                                                   */
  /* --------------------------------------------------------------- */

  static const ReplacementPolicyType replacementPolicy = randomReplacementPolicy;

  /// The structure that will keep the tags and data of the cache
  CacheContainer<nCacheLines, nLineSize, nAssociativity, replacementPolicy, 1> cache[nConfig];
  /// The cache pipeline
  Queue<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheQueue[nConfig];
  Queue<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheSnoopQueue[nConfig];
  /// Used as delayed write buffer of the cache
  DelayedWriteBuffer<nLineSize> writeBuffer[nConfig];
  /// Stock temporaly the data sent towards the CPU
  ReturnData<INSTRUCTION, nLineSize, nCachetoCPUDataPathSize> returnBuffer[nConfig];
  /// Stock temporaly the data sent towards the memory system
  RequestData<INSTRUCTION, nMemtoCacheDataPathSize, nLineSize> requestBuffer[nConfig];

 public:

  /* --------------------------------------------------------------- */
  /*  Services ports                                                 */
  /* --------------------------------------------------------------- */

  //  ServiceExport < Memory< address_t > > syscall_MemExp;   ///< Service port from memory
  //  ServiceImport < Memory< address_t > > syscall_MemImp;   ///< Service port to memory
  ServiceImport < SVGmemreqInterface<INSTRUCTION> > svg;                         ///< Service port to SVG builder
  //  ServiceExport < PowerEstimatorInterface > cache_powerport;        ///< Service port from Power Estimator
  //  ServiceImport<StatisticReporting> statistic_reporting_import;
  //ServiceExport<StatisticReportingControl> statistic_reporting_control_export;
  //// StatisticReportingControl service
  //virtual void SetPreferredStatReportingPeriod(double time_hint, time_unit_type time_unit);
  //virtual void RequiresStatReporting(const char *name, bool required);
  
  ServiceImport<MemoryInjection<address_t> > memory_injection_import;
  ServiceExport<MemoryInjection<address_t> > memory_injection_export;
    
  /* --------------------------------------------------------------- */
  /*  Module statistics                                              */
  /* --------------------------------------------------------------- */

  uint64_t accesses[nConfig];            ///< Total number of accesses statistic
  uint64_t accesses_read[nConfig];       ///< Total number of read accesses statistic
  uint64_t accesses_write[nConfig];      ///< Total number of write accesses statistic
  uint64_t accesses_prefetch[nConfig];   ///< Total number of prefetch accesses statistic
  uint64_t accesses_evict[nConfig];      ///< Total number of evict accesses statistic
  uint64_t hits[nConfig];                ///< Total number of hits statistic
  uint64_t hits_read[nConfig];           ///< Total number of read hits statistic
  uint64_t hits_write[nConfig];          ///< Total number of write hits statistic
  uint64_t hits_prefetch[nConfig];       ///< Total number of pretech hits statistic
  uint64_t hits_evict[nConfig];          ///< Total number of evict hits statistic
  uint64_t misses[nConfig];              ///< Total number of misses statistic
  uint64_t misses_read[nConfig];         ///< Total number of read misses statistic
  uint64_t misses_write[nConfig];        ///< Total number of write misses statistic
  uint64_t misses_prefetch[nConfig];     ///< Total number of prefetch misses statistic
  uint64_t misses_evict[nConfig];        ///< Total number of evict misses statistic
  uint64_t writebacks[nConfig];          ///< Total number of writebacks statistic
  uint64_t writebacks_read[nConfig];     ///< Total number of read writebacks statistic
  uint64_t writebacks_write[nConfig];    ///< Total number of write writebacks statistic
  uint64_t writebacks_prefetch[nConfig]; ///< Total number of prefetch writebacks statistic
  uint64_t writebacks_evict[nConfig];    ///< Total number of evict writebacks statistic

  /* --------------------------------------------------------------- */
  /*  Module ports                                                   */
  /* --------------------------------------------------------------- */

  inclock inClock;                                                     ///< Clock port

  inport  < memreq < INSTRUCTION, nCPUtoCacheDataPathSize >, nConfig > inCPU;   ///< Input port for requests
  outport < memreq < INSTRUCTION, nCachetoCPUDataPathSize >, nConfig > outCPU;  ///< Output port for sending data toward CPU
  inport  < memreq < INSTRUCTION, nMemtoCacheDataPathSize >, nConfig > inMEM;   ///< Input port for answers data
  outport < memreq < INSTRUCTION, nCachetoMemDataPathSize >, nConfig > outMEM;  ///< Output port for sending request toward memory hierarchy

  //  outport <bool,Snooping, nConfig> outSharedMEM;   ///< Output port for the shared bit used by some CMP models
  outport <bool, nConfig> outSharedMEM;   ///< Output port for the shared bit used by some CMP models

  /* --------------------------------------------------------------- */
  /*  Class constructor & destructor                                 */
  /* --------------------------------------------------------------- */

  /**
   * \brief Module constructor
   */
  CacheWB(const char *name, Object *parent = 0) : module(name)
						,Object(name, parent)
						,Client < MemoryInjection< address_t > >(name, this)
						,Service < MemoryInjection< address_t > >(name, this)
    //						,syscall_MemExp("syscall_MemExp", this)
    //						,syscall_MemImp("syscall_MemImp", this)
						  , memory_injection_import("mjimport",this)
						  , memory_injection_export("mjexport",this)
						,Client < SVGmemreqInterface<INSTRUCTION> > (name, this)
						,svg("svg", this)
    //                        Service<PowerEstimatorInterface>(name, this),
    //cache_powerport ("cache_powerport", this),
    //                        StatisticService(name, this),
    //						,Client<StatisticReporting>(name, this)
    //						,statistic_reporting_import("statistic", this)
						,fastforwarding(false)
  { // Naming the interface ot enhance debugging
    class_name = " CacheWB";
    category = category_CACHE;
    inCPU.set_unisim_name(this,"inCPU");
    inMEM.set_unisim_name(this,"inMEM");
    outCPU.set_unisim_name(this,"outCPU");
    outMEM.set_unisim_name(this,"outMEM");
    outSharedMEM.set_unisim_name(this,"outSharedMEM");
    // Set the sensitivity list of the module
    sensitive_pos_method(start_of_cycle) << inClock;
    sensitive_neg_method(end_of_cycle) << inClock;
//stf    sensitive_method(on_CPU_data) << inCPU.data << outCPU.accept;

    sensitive_method(on_data) << inCPU.data << outCPU.accept << inMEM.data;

    sensitive_method(on_CPU_accept) << outCPU.accept;
//    sensitive_method(on_MEM_data) << inMEM.data;
    if(Snooping)
    { // With cache snooping, new request can come from the memory side, and may need to be
      // put in the pipeline. So we need to compute if the pipeline will be able to store
      // this new request. This computation may depend on outCPU.accept.
//stf      sensitive_method(on_MEM_data) << outCPU.accept;
      sensitive_method(on_shared_accept) << outSharedMEM.accept;
    }
    sensitive_method(on_MEM_accept) << outMEM.accept << inMEM.enable;
    
    // Module state initialization
    for(int cfg=0; cfg<nConfig; cfg++)
      {
	accesses[cfg] = accesses_read[cfg] = accesses_write[cfg] = accesses_prefetch[cfg] = accesses_evict[cfg] = 0;
	hits[cfg] = hits_read[cfg] = hits_write[cfg] = hits_prefetch[cfg] = hits_evict[cfg] = 0;
	misses[cfg] = misses_read[cfg] = misses_write[cfg] = misses_prefetch[cfg] = misses_evict[cfg] = 0;
	writebacks[cfg] = writebacks_read[cfg] = writebacks_write[cfg] = writebacks_prefetch[cfg] = writebacks_evict[cfg] = 0;
      }
    // Logs
    sprintf(cache_log,"cache_%s.log",name);
    // DD Debug valgrind :
    nCPULineSize = 0;
    // ---  Registring parameters ----------------
    parameters.add("nCPUtoCacheDataPathSize",nCPUtoCacheDataPathSize);
    parameters.add("nCPUtoCacheDataPathSize",nCPUtoCacheDataPathSize);
    parameters.add("nCachetoCPUDataPathSize",nCachetoCPUDataPathSize);
    parameters.add("nCPULineSize",nCPULineSize);
    parameters.add("nMemtoCacheDataPathSize",nMemtoCacheDataPathSize);
    parameters.add("nCachetoMemDataPathSize",nCachetoMemDataPathSize);
    parameters.add("nLineSize",nLineSize,32,128);
    parameters.add("nCacheLines",nCacheLines,128,65536);
    parameters.add("nAssociativity",nAssociativity,1,128);
    parameters.add("nStages",nStages,1,5);
    parameters.add("nDelay",nDelay,1,10);
    parameters.add("nProg",nProg);
    parameters.add("Snooping",Snooping);
    parameters.add("VERBOSE",VERBOSE);
    // ---  Registring statistics ----------------
    /*
    statistics.add("accesses",accesses);
    statistics.add("accesses_read",accesses_read);
    statistics.add("accesses_write",accesses_write);
    statistics.add("accesses_prefetch",accesses_prefetch);
    statistics.add("accesses_evict",accesses_evict);
    statistics.add("hits",hits);
    statistics.add("hits_read",hits_read);
    statistics.add("hits_write",hits_write);
    statistics.add("hits_prefetch",hits_prefetch);
    statistics.add("hits_evict",hits_evict);
    statistics.add("misses",misses);
    statistics.add("misses_read",misses_read);
    statistics.add("misses_write",misses_write);
    statistics.add("misses_prefetch",misses_prefetch);
    statistics.add("misses_evict",misses_evict);
    statistics.add("writebacks",writebacks);
    statistics.add("writebacks_read",writebacks_read);
    statistics.add("writebacks_write",writebacks_write);
    statistics.add("writebacks_prefetch",writebacks_prefetch);
    statistics.add("writebacks_evict",writebacks_evict);
    */
    // -------------------------------------------
    //Latex rendering hints
    /*
    latex_top_ports.push_back(&inCPU);
    latex_top_ports.push_back(&outCPU);
    latex_bottom_ports.push_back(&outMEM);
    latex_bottom_ports.push_back(&inMEM);
    */
#ifdef CACHE_CHECK_COHERENCY
    all_caches << this;
#endif
  }

  /**
   * \brief Memory interface : Flush
   */
  // WARNING: The Flush will flush all caches !!!
  bool FlushMemory(address_t addr, uint32_t size)
  { // Flush the lines matching the addresses. Several lines may be flushed if size is big enough
    for(int cfg=0; cfg<nConfig; cfg++)
    {
      for ( address_t word_addr=addr; word_addr<(addr+size); word_addr=((word_addr+nLineSize) & ~(nLineSize - 1)) )
      { if (cache[cfg].hit(word_addr))
	{ uint32_t t_set = cache[cfg].getSet(word_addr);
	  uint32_t t_line = cache[cfg].getLine(word_addr);
#ifdef DEBUG_CACHE_FLUSHMEMORY
	  if (cache[cfg].getWrite(t_set,t_line)) 
	  { cerr << "\e[1;36m" << timestamp() << ": " << name() << ": "<< "FlushMemory hit on \e[1;31mmodified\e[0m addr=0x" << hex << addr << dec << ", " << size << " word_addr=0x" << hex << word_addr << dec << endl;
	  }
	  else
	  { cerr << "\e[1;36m" << timestamp() << ": " << name() << ": "<< "FlushMemory hit \e[0m addr=0x" << hex << addr << dec << ", " << size << " word_addr=0x" << hex << word_addr << dec << endl;
	  }
#endif
	  if (cache[cfg].getWrite(t_set,t_line)) 
	  { // The line to be flushed is modified, we should write back it to the memory first.
	    char buffer[nLineSize];
	    address_t line_address = cache[cfg].getAddress(t_set, t_line);
	    cache[cfg].getCacheLine(buffer, t_set, t_line);
	    //syscall_MemImp->WriteMemory(line_address,buffer,nLineSize);
	    memory_injection_import->InjectWriteMemory(line_address,buffer,nLineSize);
	  }
	  cache[cfg].mesi_invalidate(t_set, t_line);
	  cache[cfg].setValid(t_set, t_line, false);
	  cache[cfg].setWrite(t_set, t_line, false);
	}
      }
    }
  }

  /**
   * \brief Memory interface : Write all back
   */
  bool WriteAllBack()
  { // Instantly writeback all the modified lines to the memory
    for (int cfg=0; cfg<nConfig; cfg++)
    {

    for(int t_set = 0; t_set < (nCacheLines / nAssociativity); t_set++)
    { for(int t_line = 0; t_line < nAssociativity; t_line++)
      { if(cache[cfg].getValid(t_set,t_line)) 
        { // The line is a local hit
          if (cache[cfg].getWrite(t_set,t_line)) 
          { // The line is modified, we should write back it to the memory
            char buffer[nLineSize];
            address_t line_address = cache[cfg].getAddress(t_set, t_line);
            cache[cfg].getCacheLine(buffer, t_set, t_line);
            //syscall_MemImp->WriteMemory(line_address,buffer,nLineSize);
            memory_injection_import->InjectWriteMemory(line_address,buffer,nLineSize);

            cache[cfg].setWrite(t_set, t_line, false);
            cache[cfg].setMESI(t_set, t_line, MESI::state_EXCLUSIVE);

//cerr << "Found a line to write back : " << t_set << ", " << t_line << endl;
//exit(1);
          }
        }
      }
    }
    }
	//Taj temp
	//cache.InvalidateAll();
  }
  
  /**
   * \brief Memory interface : Reset
   */
  void Reset()
  {
  }

  /**
   * \brief Memory interface : Read from memory
   */
  //  bool ReadMemory(address_t addr, void *buffer, uint32_t size)
  bool InjectReadMemory(address_t addr, void *buffer, uint32_t size)
  { if(fastforwarding) return ReadMemory_ff(addr, buffer, size) ;
    else return ReadMemory_old(addr, buffer, size);
  }

  //-------------------------------------------------------------------------------------

  /**
   * \brief Memory interface : Read from memory (while not forwarding)
   */
  bool ReadMemory_old(address_t addr, void *buffer, uint32_t size)
  {
#ifdef DD_DEBUG_SYSCALL
    cerr << "[DD_DEBUG_SYSCALL]: in DCACHE ... ReadMemory() ..." << endl;
#endif
#ifdef DD_DEBUG_SYSCALL_VERB1
    cerr << "["<<this->name()<<"("<<timestamp()<<")]: ReadMemory(): "<< endl;
    cerr << "           addr   : "<<hexa(addr)<<endl;
    cerr << "           size   : "<<size<<endl;
    char tmpbuff[40];
    memcpy(tmpbuff,buffer,40);
    cerr << "           buffer : "<<tmpbuff<<endl;
#endif
    // Fisrt, perform the read into the main memory.
    // Second, if is some lines are modified into the cache then update the buffer.

    // Read from higher level of the hierarchy
    //    bool tmpres = syscall_MemImp->ReadMemory(addr,buffer,size);
    bool tmpres = memory_injection_import->InjectReadMemory(addr,buffer,size);

    // check for modified lines ... 
#ifdef DD_DEBUG_SYSCALL_DCACHE_RM
    cerr << "**********************************************************************************" << endl;
    cerr << "[DD_DEBUG_SYSCALL_DCACHE_RM]: in ReadMemory() ... Begin" << endl;
    cerr << "**********************************************************************************" << endl;
    cerr << "\t * addr: " << hexa(addr) << endl;
    cerr << "\t * size: " << size << endl;
    cerr << "**********************************************************************************" << endl;
#endif
    
    for(int cfg=0; cfg<nConfig; cfg++)
    {
    
      for ( address_t word_addr=addr; word_addr<(addr+size); word_addr=((word_addr+nLineSize) & ~(nLineSize - 1)) )
      { if (cache[cfg].hit(word_addr))
	{ uint32_t t_set = cache[cfg].getSet(word_addr);
          uint32_t t_line = cache[cfg].getLine(word_addr);
	  if (cache[cfg].getValid(t_set,t_line))
	    if (cache[cfg].getWrite(t_set,t_line))
	    { uint32_t t_size = nLineSize;
              address_t base_addr = (word_addr & ~(nLineSize - 1));
	      address_t write_offset = word_addr - addr;
	      if ( (base_addr+nLineSize) < (word_addr+t_size) ) 
		{ t_size = t_size - (word_addr - base_addr);
		}
	      if ( (addr+size) < (word_addr+t_size) )
		{ t_size = t_size - ( (word_addr+t_size) - (addr+size) );
		}
#ifdef DD_DEBUG_SYSCALL_DCACHE_RM
	      cerr << "\t * word addr    : " << hexa(word_addr) << endl;
	      cerr << "\t * base addr    : " << hexa(base_addr) << endl;
	      cerr << "\t * write_offset : " << write_offset << endl;
	      cerr << "\t * t_size       : " << t_size << endl;
#endif
	      cache[cfg].getCacheLineIndexed(((char *)buffer)+write_offset, t_set, t_line, word_addr-base_addr, t_size);
	    }
	}
      }

    }// Endof foreach Config.
#ifdef DD_DEBUG_SYSCALL_DCACHE_RM
    cerr << "**********************************************************************************" << endl;
    cerr << "[DD_DEBUG_SYSCALL_DCACHE_RM]: in ReadMemory() ... End" << endl;
    cerr << "**********************************************************************************" << endl;
#endif

    return tmpres;
  }
  //-------------------------------------------------------------------------------------

  /**
   * \brief Memory interface : Read from memory (while forwarding)
   */
  bool ReadMemory_ff(address_t addr, void *buffer, uint32_t size)
  {
#ifdef DD_DEBUG_SYSCALL
    cerr << "[DD_DEBUG_SYSCALL]: in DCACHE ... ReadMemory() ..." << endl;
#endif
#ifdef DD_DEBUG_SYSCALL_VERB1
    cerr << "["<<this->name()<<"("<<timestamp()<<")]: ReadMemory(): "<< endl;
    cerr << "           addr   : "<<hexa(addr)<<endl;
    cerr << "           size   : "<<size<<endl;
    char tmpbuff[40];
    memcpy(tmpbuff,buffer,40);
    cerr << "           buffer : "<<tmpbuff<<endl;
#endif
    // Fisrt, perform the read into the main memory.
    // Second, if is some lines are modified into the cache then update the buffer.

    // Read from higher level of the hierarchy
    //    bool tmpres = syscall_MemImp->ReadMemory(addr,buffer,size);
    bool tmpres = memory_injection_import->InjectReadMemory(addr,buffer,size);

    for (int cfg=0; cfg<nConfig; cfg++)
    {
	char line_bufferx[nLineSize]; //Taj -temp
	address_t evicted_addrx;

    // check for modified lines ... 
#ifdef DD_DEBUG_SYSCALL_DCACHE_RM
    cerr << "**********************************************************************************" << endl;
    cerr << "[DD_DEBUG_SYSCALL_DCACHE_RM]: in ReadMemory() ... Begin" << endl;
    cerr << "**********************************************************************************" << endl;
    cerr << "\t * addr: " << hexa(addr) << endl;
    cerr << "\t * size: " << size << endl;
    cerr << "**********************************************************************************" << endl;
#endif
    for ( address_t word_addr=addr; word_addr<(addr+size); word_addr=((word_addr+nLineSize) & ~(nLineSize - 1)) )
    { if (cache[cfg].hit(word_addr))
      { uint32_t t_set = cache[cfg].getSet(word_addr);
        uint32_t t_line = cache[cfg].getLine(word_addr);
//Taj
#ifdef DD_DEBUG_SYSCALL_DCACHE_RM
            cerr << "\t *cacheline read_hit       : " << endl;
#endif
        if (cache[cfg].getValid(t_set,t_line))
	{	// Taj - invalid case handled in else
          if (cache[cfg].getWrite(t_set,t_line))
          { uint32_t t_size = nLineSize;
            address_t base_addr = (word_addr & ~(nLineSize - 1));
            address_t write_offset = word_addr - addr;
            if ( (base_addr+nLineSize) < (word_addr+t_size) ) 
            { t_size = t_size - (word_addr - base_addr);
            }
            if ( (addr+size) < (word_addr+t_size) )
            { t_size = t_size - ( (word_addr+t_size) - (addr+size) );
            }
#ifdef DD_DEBUG_SYSCALL_DCACHE_RM
            cerr << "\t * word addr    : " << hexa(word_addr) << endl;
            cerr << "\t * base addr    : " << hexa(base_addr) << endl;
            cerr << "\t * write_offset : " << write_offset << endl;
            cerr << "\t * t_size       : " << t_size << endl;
#endif
            cache[cfg].getCacheLineIndexed(((char *)buffer)+write_offset, t_set, t_line, word_addr-base_addr, t_size);


          }

//#ifdef CACHEWB_PERFECT_WARMUP
	  else ;//Taj -  cache hit, valid and non-modified.  curline==linefrommemory ??
		//if yes, do nothing as we already have brought data from memory
//#endif

	}

//#ifdef CACHEWB_PERFECT_WARMUP
	else{   //Taj - line was invalid
	 	// replace line with data brought from memory. if(fastforwarding)
		if(fastforwarding){
        		uint32_t t_size = nLineSize;
     		        address_t base_addr = (word_addr & ~(nLineSize - 1));
	              	address_t write_offset = word_addr - addr;
		        if ( (base_addr+nLineSize) < (word_addr+t_size) ) 
	        	{ t_size = t_size - (word_addr - base_addr);
        	    	}
	                if ( (addr+size) < (word_addr+t_size) )
           	 	{ t_size = t_size - ( (word_addr+t_size) - (addr+size) );
		        }
		
//Taj #ifdef DD_DEBUG_SYSCALL_DCACHE_RM
			cerr << "\t READ invlid line, updating with data from memory" <<endl ;
			cerr << "\t * word addr    : " << hexa(word_addr) << endl;
			cerr << "\t * base addr    : " << hexa(base_addr) << endl;
			cerr << "\t * write_offset : " << write_offset << endl;
			cerr << "\t * t_size       : " << t_size << endl;
//#endif

			//cache.setCacheLineIndexed(t_set, t_line, ((char *)buffer)+write_offset, word_addr-base_addr, t_size, true);
			cache[cfg].setCacheLine( t_set,t_line, base_addr, ((char*)buffer)+write_offset, true, false); //Taj - replce whole line 
//	        	cache.setWrite(t_set, t_line, false); //u can also modify the last parameter in above set function, see later
			// new state = E or S ?? when procerror reads  how do the other caches know about it ??
			cache[cfg].setMESI(t_set, t_line,MESI::state_EXCLUSIVE);
	//Taj -temp
/*			evicted_addrx = cache.getAddress( t_set, t_line);
		        cache.getCacheLine(line_bufferx, t_set, t_line);
		        syscall_MemImp->WriteMemory(evicted_addrx,line_bufferx,nLineSize);
*/
		} //end if fastforwarding

	}
//#endif

      }

//#ifdef CACHEWB_PERFECT_WARMUP
      else{ //Taj - cache miss

	if( fastforwarding){      
		//if there's place , update. handled below.
		uint32_t t_set = cache[cfg].getReplaceSet(word_addr);
	        uint32_t t_line = cache[cfg].getReplaceLine(word_addr);

		uint32_t t_size = nLineSize;
	        address_t base_addr = (word_addr & ~(nLineSize - 1));
	        address_t write_offset = word_addr - addr;
         	if ( (base_addr+nLineSize) < (word_addr+t_size) ) 
            	{ t_size = t_size - (word_addr - base_addr);
            	}
            	if ( (addr+size) < (word_addr+t_size) )
	        { t_size = t_size - ( (word_addr+t_size) - (addr+size) );
            	}


		// else if( existing_line_modified)	
		if( cache[cfg].getValid (t_set, t_line )){ //see that only valid modified lines are evicted
		if( cache[cfg].getWrite(t_set, t_line) ){

#ifdef DD_DEBUG_SYSCALL_DCACHE_RM
	    cerr << "\t cacheline miss Evicting, updating with data from memory" <<endl ;
            //cerr << "\t * word addr    : " << hexa(word_addr) << endl;
            //cerr << "\t * base addr    : " << hexa(base_addr) << endl;
            //cerr << "\t * write_offset : " << write_offset << endl;
            //cerr << "\t * t_size       : " << t_size << endl;
#endif

			//write previous line to memory - eviction ??
			address_t evicted_addr = cache[cfg].getAddress( t_set, t_line) ; //addr of prev line in cache
		        char line_buffer[nLineSize];
	        	cache[cfg].getCacheLine(line_buffer, t_set, t_line);
		        //syscall_MemImp->WriteMemory(evicted_addr,line_buffer,nLineSize);
		        memory_injection_import->InjectWriteMemory(evicted_addr,line_buffer,nLineSize);

//cout <<"READ EVICTION address =0x"<<setbase(16)<<evicted_addr<<setbase(10)<<endl;
		
			//update line with data from memory - done below 
	        	//cache.setCacheLineIndexed(t_set, t_line, ((char *)buffer)+write_offset, word_addr-base_addr, t_size, true);
			
			//newstate = modified, will be after update
			//cache.setMESI(t_set, t_line,MESI::state_EXCLUSIVE);
	//Taj -temp
/*			evicted_addrx = cache.getAddress( t_set, t_line);
		        cache.getCacheLine(line_bufferx, t_set, t_line);
		        syscall_MemImp->WriteMemory(evicted_addrx,line_bufferx,nLineSize);
*/

		}
		// else if (existing_line_shared) -  igonring fro the time being
			//send invalidate signal to others
			//newstate = shared, after update
		} //end if validate

#ifdef DD_DEBUG_SYSCALL_DCACHE_RM
	    cerr << "\t cacheline miss overwriting, updating with data from memory" <<endl ;
            cerr << "\t * word addr    : " << hexa(word_addr) << endl;
            cerr << "\t * base addr    : " << hexa(base_addr) << endl;
            cerr << "\t * write_offset : " << write_offset << endl;
            cerr << "\t * t_size       : " << t_size << endl;
#endif

	    //syscall_MemImp->ReadMemory( base_addr, line_bufferx, nLineSize );
		memory_injection_import->InjectReadMemory( base_addr, line_bufferx, nLineSize );
		cache[cfg].setCacheLine( t_set,t_line, base_addr, line_bufferx, true, false); //Taj - get non-buffer data from memory 
		//update line with data from memory 
	        //cache.setCacheLineIndexed(t_set, t_line, ((char *)buffer)+write_offset, word_addr-base_addr, t_size, true);
		//cache.setCacheLine( t_set,t_line, base_addr, ((char*)buffer)+write_offset, true, true ); //Taj - replce whole line 
	
		// setCacheLineIndexed mark the line as modified. 
	        // let's reset the Write field.
        	//cache.setWrite(t_set, t_line, false);  // see last param in above set() function

		//newstate = modified, will be after update
		cache[cfg].setMESI(t_set, t_line,MESI::state_EXCLUSIVE);

	//Taj -temp
/*			evicted_addrx = cache.getAddress( t_set, t_line);
		        cache.getCacheLine(line_bufferx, t_set, t_line);
		        syscall_MemImp->WriteMemory(evicted_addrx,line_bufferx,nLineSize);
*/
      	} //end if(fastforwarding)

      } //end else cache miss
//#endif

    } //end for address

#ifdef DD_DEBUG_SYSCALL_DCACHE_RM
    cerr << "**********************************************************************************" << endl;
    cerr << "[DD_DEBUG_SYSCALL_DCACHE_RM]: in ReadMemory() ... End" << endl;
    cerr << "**********************************************************************************" << endl;
#endif

    //bool tmpres2 = syscall_MemImp->ReadMemory(addr,buffer,size); //Taj testing - temp
    }// End of foreach Config.
    
    return tmpres;
  }
  
  // --------------------------------------------------------------------------


  /**
   * \brief Memory interface : Write to memory
   */
  //  bool WriteMemory(address_t addr, const void *buffer, uint32_t size)
  bool InjectWriteMemory(address_t addr, const void *buffer, uint32_t size)
  { if(fastforwarding) return WriteMemory_ff(addr, buffer,size);
    else return WriteMemory_old(addr, buffer,size);
  }


  // --------------------------------------------------------------------------
  /**
   * \brief Memory interface : Write to memory (while not forwarding)
   */
  bool WriteMemory_old(address_t addr, const void *buffer, uint32_t size)
  {



#ifdef DD_DEBUG_SYSCALL
    cerr << "[DD_DEBUG_SYSCALL]: in DCACHE ... WriteMemory() ..." << endl;
#endif
#ifdef DD_DEBUG_SYSCALL_VERB1
    cerr << "["<<this->name()<<"("<<timestamp()<<")]: WriteMemory(): "<< endl;
    cerr << "           addr   : "<<hexa(addr)<<endl;
    cerr << "           size   : "<<size<<endl;
    char tmpbuff[40];
    memcpy(tmpbuff,buffer,40);
    cerr << "           buffer : "<<tmpbuff<<endl;
#endif
    //    Write(buffer,addr,size);
    //return syscall_MemImp->WriteMemory(addr,buffer,size);

    // How to do the write ?
    // 

    // check for modified lines ...
#ifdef DD_DEBUG_SYSCALL_DCACHE_WM
    cerr << "**********************************************************************************" << endl;
    cerr << "[DD_DEBUG_SYSCALL_DCACHE_WM]: in WriteMemory() ... Begin" << endl;
    cerr << "**********************************************************************************" << endl;
    cerr << "\t * addr: " << hexa(addr) << endl;
    cerr << "\t * size: " << size << endl;
    cerr << "**********************************************************************************" << endl;
#endif

#ifdef DEBUG_CACHE_WRITEMEMORY
//  cerr << "\n---------------------------------------------------------------------------------------\n" << endl;
#endif
    for(int cfg=0; cfg<nConfig; cfg++)
    {

    for ( address_t word_addr=addr; word_addr<(addr+size); word_addr=((word_addr+nLineSize) & ~(nLineSize - 1)) )
    { 
    
      address_t base_addr = (word_addr & ~(nLineSize - 1)); // Address of the cache line
    
#ifdef DEBUG_CACHE_WRITEMEMORY
//cerr << "... line_address=" << hex << base_addr << dec << endl;
#endif

      if (cache[cfg].hit(word_addr))
      { uint32_t t_set = cache[cfg].getSet(word_addr);
        uint32_t t_line = cache[cfg].getLine(word_addr);


#ifdef DEBUG_CACHE_WRITEMEMORY
  cerr << "\n-------- - --------- - --------------- - -------------- - -----------------------------\n" << endl;
  cerr << "\e[1;33m" << timestamp() << ": " << name() << ": "<< "WriteMemory \e[0m addr=0x" << hex << addr << dec << ", " << size << " word_addr=0x" << hex << word_addr << dec << endl;

//  cerr << "before : ";
//  cache.dump(cerr,t_set,t_line);
#endif

        // For a write we don't check if the line is modified : we modify it ...
        // if (cache.getWrite(t_set,t_line))
        uint32_t t_size = nLineSize;
        address_t write_offset = word_addr - addr;
        if ( (base_addr+nLineSize) < (word_addr+t_size) ) 
        { t_size = t_size - (word_addr - base_addr);
        }
        if ( (addr+size) < (word_addr+t_size) )
        { // t_size = t_size - ((addr+size) - word_addr);
          t_size = t_size - ( (word_addr+t_size) - (addr+size) );
        }
#ifdef DD_DEBUG_SYSCALL_DCACHE_WM
        cerr << "\t * word addr    : " << hexa(word_addr) << endl;
        cerr << "\t * base addr    : " << hexa(base_addr) << endl;
        cerr << "\t * write_offset : " << write_offset << endl;
        cerr << "\t * t_size       : " << t_size << endl;
#endif
        cache[cfg].setCacheLineIndexed(t_set, t_line, ((char *)buffer)+write_offset, word_addr-base_addr, t_size, true);
        // setCacheLineIndexed mark the line as modified. As we'll also forward the MemoryWrite to the memory,
        // let's reset the Write field.
        cache[cfg].setWrite(t_set, t_line, false);
        // The mesi state goes to exclusive as other caches are flushed
        cache[cfg].setMESI(t_set, t_line,MESI::state_EXCLUSIVE);


#ifdef DEBUG_CACHE_WRITEMEMORY
//  cerr << "after  : ";
//  cache.dump(cerr,t_set,t_line);
#endif

        //Send the modified cache line to the memory to ensure that line data not bellonging to the
        //buffer is also wrote-back.
        char line_buffer[nLineSize];
        cache[cfg].getCacheLine(line_buffer, t_set, t_line);
	//        syscall_MemImp->WriteMemory(base_addr,line_buffer,nLineSize);
        memory_injection_import->InjectWriteMemory(base_addr,line_buffer,nLineSize);
      }
     
    }
    }//Endof foreach Config.
#ifdef DD_DEBUG_SYSCALL_DCACHE_WM
    cerr << "**********************************************************************************" << endl;
    cerr << "[DD_DEBUG_SYSCALL_DCACHE_WM]: in WriteMemory() ... End" << endl;
    cerr << "**********************************************************************************" << endl;
#endif
    //    return syscall_MemImp->WriteMemory(addr,buffer,size);
    return memory_injection_import->InjectWriteMemory(addr,buffer,size);
  }


  // --------------------------------------------------------------------------

  /**
   * \brief Memory interface : Write to memory (while forwarding)
   */
  bool WriteMemory_ff(address_t addr, const void *buffer, uint32_t size)
  {

#ifdef DD_DEBUG_SYSCALL
    cerr << "[DD_DEBUG_SYSCALL]: in DCACHE ... WriteMemory() ..." << endl;
#endif
#ifdef DD_DEBUG_SYSCALL_VERB1
    cerr << "["<<this->name()<<"("<<timestamp()<<")]: WriteMemory(): "<< endl;
    cerr << "           addr   : "<<hexa(addr)<<endl;
    cerr << "           size   : "<<size<<endl;
    char tmpbuff[40];
    memcpy(tmpbuff,buffer,40);
    cerr << "           buffer : "<<tmpbuff<<endl;
#endif
    //    Write(buffer,addr,size);
    //return syscall_MemImp->WriteMemory(addr,buffer,size);

//syscall_MemImp->WriteMemory(addr,buffer,size); //Taj - testing temp

    // How to do the write ?
    // 

    // check for modified lines ...
#ifdef DD_DEBUG_SYSCALL_DCACHE_WM
    cerr << "**********************************************************************************" << endl;
    cerr << "[DD_DEBUG_SYSCALL_DCACHE_WM]: in WriteMemory() ... Begin" << endl;
    cerr << "**********************************************************************************" << endl;
    cerr << "\t * addr: " << hexa(addr) << endl;
    cerr << "\t * size: " << size << endl;
    cerr << "**********************************************************************************" << endl;
#endif

#ifdef DEBUG_CACHE_WRITEMEMORY
//  cerr << "\n---------------------------------------------------------------------------------------\n" << endl;
#endif
    for(int cfg=0; cfg<nConfig; cfg++)
    {

    for ( address_t word_addr=addr; word_addr<(addr+size); word_addr=((word_addr+nLineSize) & ~(nLineSize - 1)) )
    { 
    
      address_t base_addr = (word_addr & ~(nLineSize - 1)); // Address of the cache line
    
#ifdef DEBUG_CACHE_WRITEMEMORY
//cerr << "... line_address=" << hex << base_addr << dec << endl;
#endif

      if (cache[cfg].hit(word_addr))
      { uint32_t t_set = cache[cfg].getSet(word_addr);
        uint32_t t_line = cache[cfg].getLine(word_addr);


#ifdef DEBUG_CACHE_WRITEMEMORY
  cerr << "\n-------- - --------- - --------------- - -------------- - -----------------------------\n" << endl;
  cerr << "\e[1;33m" << timestamp() << ": " << name() << ": "<< "WriteMemory \e[0m addr=0x" << hex << addr << dec << ", " << size << " word_addr=0x" << hex << word_addr << dec << endl;

//  cerr << "before : ";
//  cache.dump(cerr,t_set,t_line);
#endif

        // For a write we don't check if the line is modified : we modify it ...
        // if (cache.getWrite(t_set,t_line))
        uint32_t t_size = nLineSize;
        address_t write_offset = word_addr - addr;
        if ( (base_addr+nLineSize) < (word_addr+t_size) ) 
        { t_size = t_size - (word_addr - base_addr);
        }
        if ( (addr+size) < (word_addr+t_size) )
        { // t_size = t_size - ((addr+size) - word_addr);
          t_size = t_size - ( (word_addr+t_size) - (addr+size) );
        }
#ifdef DD_DEBUG_SYSCALL_DCACHE_WM
        cerr << "\t * word addr    : " << hexa(word_addr) << endl;
        cerr << "\t * base addr    : " << hexa(base_addr) << endl;
        cerr << "\t * write_offset : " << write_offset << endl;
        cerr << "\t * t_size       : " << t_size << endl;
#endif
        cache[cfg].setCacheLineIndexed(t_set, t_line, ((char *)buffer)+write_offset, word_addr-base_addr, t_size, true);
        // setCacheLineIndexed mark the line as modified. As we'll also forward the MemoryWrite to the memory,
        // let's reset the Write field.
//        cache.setWrite(t_set, t_line, false);
        // The mesi state goes to exclusive as other caches are flushed
        cache[cfg].setMESI(t_set, t_line,MESI::state_MODIFIED);


#ifdef DEBUG_CACHE_WRITEMEMORY
//  cerr << "after  : ";
//  cache.dump(cerr,t_set,t_line);
#endif

        //Send the modified cache line to the memory to ensure that line data not bellonging to the
        //buffer is also wrote-back.
//Taj - commented below: should make a special case for non-fastforwarded system calls to be backward comaptibel
/*        char line_buffer[nLineSize];
        cache.getCacheLine(line_buffer, t_set, t_line);
        syscall_MemImp->WriteMemory(base_addr,line_buffer,nLineSize);
*/        
      }

//#ifdef CACHEWB_PERFECT_WARMUP
      else //Taj - cache miss
      {
	if( fastforwarding){
		uint32_t t_set = cache[cfg].getReplaceSet(word_addr);
	        uint32_t t_line = cache[cfg].getReplaceLine(word_addr);

		uint32_t t_size = nLineSize;
	        address_t write_offset = word_addr - addr;
        	if ( (base_addr+nLineSize) < (word_addr+t_size) ) 
	        { t_size = t_size - (word_addr - base_addr);
	        }
	        if ( (addr+size) < (word_addr+t_size) )
	        { // t_size = t_size - ((addr+size) - word_addr);
	          t_size = t_size - ( (word_addr+t_size) - (addr+size) );
	        }


		// write into memory, shall we ???? as done above 
		// if (place available)
		if( cache[cfg].getValid( t_set, t_line) ){
			//else if ( line_modified )
			if( cache[cfg].getWrite(t_set, t_line) ){

			//write existing line to memory, eviction ??
			char line_buffer[nLineSize];
			address_t evicted_addr = cache[cfg].getAddress( t_set, t_line) ;//address-in-memory of line to be evicted
	        	cache[cfg].getCacheLine(line_buffer, t_set, t_line);
	        	//syscall_MemImp->WriteMemory(evicted_addr,line_buffer,nLineSize);
	        	memory_injection_import->InjectWriteMemory(evicted_addr,line_buffer,nLineSize);

//cout <<"WRITE EVICTION address =0x"<<setbase(16)<<evicted_addr<<setbase(10)<<endl;

			} // end if cache.getWrite
		//else if (line shared) - left for the moment
			//RFO broadcast, invalidate line in other caches first
			//write data in cache line
			//newstate = M, E if memory writethrough
		} // end cache.getValid
		else ;// Taj - miss and existing line's invalid

		// overwrite it
		char line_bufferx[nLineSize];
		//syscall_MemImp->ReadMemory( base_addr, line_bufferx, nLineSize );
		memory_injection_import->InjectReadMemory( base_addr, line_bufferx, nLineSize );
		cache[cfg].setCacheLine( t_set,t_line, base_addr, line_bufferx, true, false); //Taj - get non-buffer data from memory 
		//write data into cache line
        	cache[cfg].setCacheLineIndexed(t_set, t_line, ((char *)buffer)+write_offset, word_addr-base_addr, t_size, true);
		//cache.setCacheLine( t_set,t_line, base_addr, ((char*)buffer)+write_offset, true, true ); //Taj - replce whole line 
		
		//new state = M, E if memory write thorugh
		// setCacheLineIndexed mark the line as modified. As we'll also forward the MemoryWrite to the memory,
	        // let's reset the Write field. //see last param in above set() func
//	        cache.setWrite(t_set, t_line, false); //Taj - why ?? the line content is different unless written to mem below.
	        // The mesi state goes to exclusive as other caches are flushed
	        cache[cfg].setMESI(t_set, t_line,MESI::state_MODIFIED);


	//Taj - writing modified line to memory(esp. to emulator_mem, so that emulator can see these changes next time it validates
	// otherwise, when going into simulation, the cache would have a different version of things than emulator_mem

//Taj - commented below: dont want a write-through behaviour in fastforward
/*        char line_buffer2[nLineSize];
	address_t line_addr = cache.getAddress(t_set, t_line);
        cache.getCacheLine(line_buffer2, t_set, t_line);
        syscall_MemImp->WriteMemory(line_addr,line_buffer2,nLineSize);
*/	
	} //end if fastforwarding


      } //end else cache miss 
//#endif
     
    }
    }//Endof foreach Config.
#ifdef DD_DEBUG_SYSCALL_DCACHE_WM
    cerr << "**********************************************************************************" << endl;
    cerr << "[DD_DEBUG_SYSCALL_DCACHE_WM]: in WriteMemory() ... End" << endl;
    cerr << "**********************************************************************************" << endl;
#endif
    //   return syscall_MemImp->WriteMemory(addr,buffer,size);
   return memory_injection_import->InjectWriteMemory(addr,buffer,size);
  }

  /**
   * \brief Service interface : initialization
   */
  bool ClientIndependentSetup()
  { if(svg) svg->register_cache(name());
    //    if(!syscall_MemImp) 
    if(!memory_injection_import) 
    { ERROR << "syscall_MemImp service port is not connected. This connection is mandatory to perfrom syscalls !" << endl;
      return false;
    }
    // Retreive the linesize of the cache connected toward the CPU. Set it to zero if
    // directly connected to the CPU.
    try
    { nCPULineSize = inCPU.get_connected_module()->get_parameter("nLineSize");
    }
    catch (UnknownParameterException &e)
    { nCPULineSize = 0;
      //nCPULineSize = DL1_nCachetoCPUDataPathSize;
    }
    return true;
  }
 
  /* --------------------------------------------------------------- */
  /*  Module processes                                               */
  /* --------------------------------------------------------------- */
 
  /**
   * \brief Process called on clock rising edge.
   */
  void start_of_cycle()
  { 
#ifdef DD_DEBUG_DCACHE_VERB100
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr << "[Cnonfig::all][DD_DEBUG_DCACHE("<<timestamp()<<")("<< this->name() <<")]  Begin of start_of_cycle..." << endl;
    }
#endif

    // checks if there is something to be sent from the cache pipeline and 
    // the MSHR queue and send them to the cpu and memory system
    SendData();

    for(int cfg=0; cfg<nConfig; cfg++)
    {
      inMEM_was_a_hit[cfg] = false;
      inMEM_was_a_cacheQueue_hit[cfg] = false;
    }
    if(VERBOSE) dump();
#ifdef DD_DEBUG_DCACHE_VERB101
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr << "[DD_DEBUG_DCACHE("<< this->name() <<")("<<timestamp()<<")] Cache Dump : " << endl; 
      cerr << *this << endl;
    }
#endif
#ifdef DD_DEBUG_DCACHE_VERB100
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr << "[Cnonfig::all][DD_DEBUG_DCACHE("<<timestamp()<<")("<< this->name() <<")]  End of start_of_cycle..." << endl;
    }
#endif

  }


  /**
   * \brief Process called on clock falling edge.
   */
  void end_of_cycle()
  {
#ifdef DD_DEBUG_DCACHE_VERB101
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr << "[Cnonfig::all][DD_DEBUG_DCACHE("<<timestamp()<<")("<< this->name() <<")]  Begin of end_of_cycle..." << endl;
    }
#endif
      // checks that the data sent in the previous cycle has been accepted and update the state of 
    // the cache pipeline and the MSHR queue if necessary
    //    CheckAcceptedData();
    CheckCPUAcceptedData();
    CheckMemAcceptedDatafromCache();
    if(Snooping) 
    { CheckSnoopMemAcceptedDatafromCache();
    }
    // checks that the module has received something from the memory system and update the MSHR 
    // queue state and the cache state if necessary
    ReadMemData();
    // run the cache pipeline
    RunCachePipeline();
    // checks that the cpu has sent something to the module an add it to the cache pipeline
    ReadCPUData();
    // Store snooped read request that made a local hit in the cache queue
    if(Snooping) ReadSnoopedData();
    // set the cache head state
    SetCacheHeadState();
    // set the cache snooped head state
    if(Snooping) SetSnoopedCacheHeadState();
#ifdef DD_DEBUG_DCACHE_VERB101
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr << "[Cnonfig::all][DD_DEBUG_DCACHE("<<timestamp()<<")("<< this->name() <<")]  End of end_of_cycle..." << endl;
    }
#endif
  }


  /* --------------------------------------------------------------- */
  /*  Module public methods                                          */
  /* --------------------------------------------------------------- */

  /**
   * Module pretty printer
   */
  friend ostream& operator << (ostream& os, const this_class &cache)
  { 
    for(int cfg=0; cfg<nConfig; cfg++)
    {
      os << "==================== [Config::"<<cfg<<"] " << cache.fsc_object::name() << " =====================" << endl;
    os << "Cache Pipeline:" << endl;
    os << cache.cacheQueue[cfg];
    os << "----------------------------------------------------------" << endl;
    if(Snooping)
    { os << "Cache Snoop Pipeline:" << endl;
      os << cache.cacheSnoopQueue[cfg];
      os << "----------------------------------------------------------" << endl;
    }
    os << "Delayed Write Buffer:" << endl;
    os << cache.writeBuffer[cfg] << endl;
    os << "Return Buffer:" << endl;
    os << cache.returnBuffer[cfg] << endl;
    os << "Request Buffer:" << endl;
    os << cache.requestBuffer[cfg] << endl;
    os << "==========================================================" << endl;
    } 
    return os;
  }

  /* --------------------------------------------------------------- */
  /*  Module private methods                                         */
  /* --------------------------------------------------------------- */

  /**
   * \brief Sends data to the cpu and the memory system if returnBuffer and/or requestBuffer are ready
   */
  void SendData()
  { 
    for (int cfg=0; cfg<nConfig; cfg++)
    {
      // If returnBuffer is empty, cacheQueue is not empty and its head is ready. Then copy from
      // head to return buffer.
#ifdef DD_DEBUG_DCACHE_VERB2
      if (DD_DEBUG_TIMESTAMP <= timestamp())
      {
	cerr << "["<<this->name()<<"("<<timestamp()<<")]: SendData, start..." << endl;
      }
#endif
      // If returnBuffer is empty , cacheQueue is not empty and its head ready, copy from head to returnBuffer
      if(!returnBuffer[cfg].ready) SendCacheDatatoCPU(cfg);
#ifdef DD_DEBUG_DCACHE_VERB2
      else
      {
	if (DD_DEBUG_TIMESTAMP <= timestamp())
	  {
	    cerr << "["<<this->name()<<"("<<timestamp()<<")]: Warning: returnBuffer.ready == True (no call to SendCacheDatatoCPU) ..." << endl;
	  }
      }
#endif
      /* if there is nothing in the requestBuffer the cache can try to sent something
       * to the memory system */
      if(Snooping)
      { if(!requestBuffer[cfg].ready) SendSnoopDatatoMem(cfg);
      }
      if(!requestBuffer[cfg].ready) SendCacheDatatoMem(cfg);
      
      //cerr << "\033[31mCACHE: SendData  returnBuffer.ready="<< returnBuffer.ready << " ,requestBuffer.ready=" << requestBuffer.ready << " \033[0m" << endl;  
      /* if the returnBuffer is ready then set output ports to the cpu */
      if(returnBuffer[cfg].ready)
      { /* set the output signals */
	memreq<INSTRUCTION, nCachetoCPUDataPathSize> req;
	req.message_type = memreq_types::type_ANSWER;
	req.instr = returnBuffer[cfg].instr;
	req.size = returnBuffer[cfg].size > nCachetoCPUDataPathSize ? nCachetoCPUDataPathSize : returnBuffer[cfg].size;
	req.address = returnBuffer[cfg].base + returnBuffer[cfg].index;
	req.Write(&returnBuffer[cfg].data[returnBuffer[cfg].index], (returnBuffer[cfg].size > nCachetoCPUDataPathSize) ? nCachetoCPUDataPathSize : returnBuffer[cfg].size);
	req.uid = returnBuffer[cfg].uid;
	req.memreq_id = returnBuffer[cfg].memreq_id;
	req.sender_type = memreq_types::sender_CACHE;
	//	req.sender = this;
	//	req.sender = this->name();
	stringstream sstr;
	sstr << this->name() << cfg;
	req.sender = sstr.str();
	//
	req.req_sender = returnBuffer[cfg].req_sender;
	req.cachable = returnBuffer[cfg].cachable;
	req.command = memreq_types::cmd_READ; // Only reads are sent back to the CPU
#ifdef DD_DEBUG_DCACHE_VERB2
	if (DD_DEBUG_TIMESTAMP <= timestamp())
	  {
	    cerr <<"["<<this->name()<<"("<<timestamp()<<")]: Sending to CPU: "<< req << endl;
	  }
#endif
#ifdef DEBUG_TEST_UNCACHABLE_WRITE
	//if( !req.cachable && req.command==memreq_types::cmd_WRITE ) 
	INFO << "-CPU-> \e[1;31muncachable write\e[0m: " << req << endl;
#endif
	outCPU.data[cfg] = req;
      }
      else outCPU.data[cfg].nothing();
      
      /* if the requestBuffer is ready then set output signals to the memory system */
      if(requestBuffer[cfg].ready)
      { memreq<INSTRUCTION, nCachetoMemDataPathSize> req;
      req.message_type = requestBuffer[cfg].message_type;
      req.instr = requestBuffer[cfg].instr;
#ifdef NOC_THREADS_DISTRIBUTION
      req.address = requestBuffer[cfg].address+ requestBuffer.index;
#else
      req.address = requestBuffer[cfg].address;

#endif
      req.command = requestBuffer[cfg].command;     
      
      int size = requestBuffer[cfg].size;
      if(requestBuffer[cfg].write && size > nCachetoMemDataPathSize)
      { size = nCachetoMemDataPathSize;
      }
      req.size = size;
      if(requestBuffer[cfg].write)
      { req.Write(requestBuffer[cfg].data + requestBuffer[cfg].index, size);
      }
      req.uid = requestBuffer[cfg].uid;
      req.memreq_id = requestBuffer[cfg].memreq_id;
      req.sender_type = memreq_types::sender_CACHE;
      req.cachable = requestBuffer[cfg].cachable;
      //      req.sender = this;
      {
	stringstream sstr;
	sstr << this->name() << cfg;
	req.sender = sstr.str();
      }
      if(requestBuffer[cfg].message_type==memreq_types::type_REQUEST)
      { // This is a new request, set this module as the original sender
	//        req.req_sender = this;
	{
	  stringstream sstr;
	  sstr << this->name() << cfg;
	  req.req_sender = sstr.str();
	}
      }
      else
      { // This is an answer to an existing request, forward the original sender value
        req.req_sender = requestBuffer[cfg].req_sender;
      }
#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr <<"["<<this->name()<<"("<<timestamp()<<")] Sending to MEM " << req << endl;
    }
#endif
#ifdef DEBUG_TEST_UNCACHABLE_READ 
     if( !req.cachable && req.command!=memreq_types::cmd_WRITE ) INFO << "-MEM-> \e[1;32muncachable read \e[0m: " << req << endl;
#endif
#ifdef DEBUG_TEST_UNCACHABLE_WRITE
      //if( !req.cachable && req.command==memreq_types::cmd_WRITE ) 
      INFO << "-MEM-> \e[1;31muncachable write\e[0m: " << req << endl;
#endif

    #ifdef NOC_THREADS_DISTRIBUTION 
      req.dst_id = requestBuffer[cfg].dst_id;
      if ( (req.command == memreq_types::cmd_WRITE) && (req.cachable == true) )
            req.dst_id = 0;
    #endif


      outMEM.data[cfg] = req;
    }
    else outMEM.data[cfg].nothing();
#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
    cerr << "["<<this->name()<<"("<<timestamp()<<")]: SendData, end..." << endl;
    }
#endif
    } // end of for each Config.
    outCPU.data.send();
    outMEM.data.send();
  } // end of SendData()
  /**
   * \brief Process called upon receiving an accept from the CPU
   *
   * Sensitive to outCPU.accept
   *
   * Enable CPU output upon receiving the accept.
   *
   * Previously called CheckCPUAccept()
   */
  void on_CPU_accept()
  { if(outCPU.accept.known())
    {
//if(outCPU.accept) cerr << "\033[31mCACHE: recieved accept from CPU enabling CPU "<< outCPU.accept.known() <<" \033[0m" << endl;  
      for (int cfg=0; cfg<nConfig; cfg++)
      {  
	outCPU.enable[cfg] = outCPU.accept[cfg];
      }
      outCPU.enable.send();
    }
  }

  /**
   * \brief Process called upon receicing an accept from MEM
   *
   * Sensitive to outMEM.accept
   *
   * Enable MEM output upon receiving the accept.
   *
   * Previously called CheckMemAccept()
   */
  void on_MEM_accept()
  { if(outMEM.accept.known() && inMEM.enable.known())
    {
#ifdef DD_DEBUG_DCACHE_VERB2
      if (DD_DEBUG_TIMESTAMP <= timestamp())
      {
	cerr << "["<<this->name()<<"("<<timestamp()<<"] Cache recieved accept from MEM enabling MEM" << endl; 
	//	if(outMEM.accept[]) cerr << "["<<this->name()<<"("<<timestamp()<<"] Cache recieved accept from MEM enabling MEM" << endl;  
	for (int cfg=0; cfg<nConfig; cfg++)
	{
	  cerr << "[A(" << cfg << ")=" << ((outMEM.accept[cfg])?"t":"f") << "]"; 
	}
	cerr << endl;
	//      else cerr << "\033[31mCACHE: MEM didn't accept the previous request !!!!!!!!!!!! \033[0m" << endl; 
      }
#endif
      // DD: before enabling outMEM data we have to check if a another processor just answer the same request.
      for (int cfg=0; cfg<nConfig; cfg++)
      {
	// DD: before enabling outMEM data we have to check if a another processor just answer the same request.
	if (!inMEM.enable[cfg])
	{
	  outMEM.enable[cfg] = outMEM.accept[cfg];
	}
	else
	{
	  // We check outMEM.accept is true to be sure that outMEMdata is not an older value sent previous cylce.
	  if (outMEM.accept[cfg])
	  {
	    // Make sure the incoming data is a read to be handled by the snooper
	    memreq<INSTRUCTION, nMemtoCacheDataPathSize> mrin = inMEM.data[cfg];
	    memreq<INSTRUCTION, nMemtoCacheDataPathSize> mrout = outMEM.data[cfg];
	    // DD...
	    // If the answer is for another cache but if we are currently trying to answer to
	    // this cache we have to discard this answer.
	    if ( 
		(mrin.message_type==memreq_types::type_ANSWER) && // is in req. an answer ?
		(mrin.req_sender == mrout.req_sender) &&          // is in req_sender equal to out req_sender ?
		(mrin.address == mrout.address)                   // are req. on the same addresses ?
		)
	    {
#ifdef DD_DEBUG_DCACHE_SNOOPING_VERB100
	      if (DD_DEBUG_TIMESTAMP <= timestamp())
		{
		  cerr << "[Cnonfig::"<<cfg<<"][DD_DEBUG_DCACHE("<< this->name() <<")] SNOOPING Not ENABLING outMEM because another cache is answering !!!!" << mrin << endl;
		}
#endif		    
	      outMEM.enable[cfg] = false;	
	    }
	    else
	    {
	      outMEM.enable[cfg] = outMEM.accept[cfg];
	    }
	  }
	  else
	  {
	    outMEM.enable[cfg] = outMEM.accept[cfg];
	  }
	}
      }// end of Foreach Config.
      outMEM.enable.send();
    }
  }

  /**
   * \brief Returns true if a new request can be accepted (If the cache pipeline allows
   * to insert a new request)
   * 
   * outCPU.accept should be known before calling this method 
   */
  bool can_accept_request(Queue<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> &queue, int cfg)
  { // Accepts request if cache pipeline is empty
    if(queue.Empty()) return true;
    // the pipeline is not empty, then check the state of the pipeline to see if the request can be added
    QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;
    int stage = 0; /* stage is used to know if there are holes in the pipelines */
    /* the pipeline is parsed starting by the youngest request in the pipeline */
    for(cacheit = queue.SeekAtTail(); cacheit; cacheit--, stage++)
    { /* the pipeline has a hole which will make the pipeline advance, and makes possible to accept the request */ 
      if(cacheit->stage > stage) return true;

      if(stage != nStages-1)
      { /* the current pipeline request is not in the last stage of the pipeline */
        /* if the current pipeline stage has a delay bigger than zero, then the pipeline will not advance
         * and the request can not be accepted */
        if(cacheit->delay[cacheit->stage] > 0) return false;
      }
      else
      { /* the current request is in the last stage of the pipeline */
        /* if the delay of the current pipeline request is bigger than zero, the pipeline will not advance
           * and the request can not be accepted */
        if(cacheit->delay[stage] > 0) return false;

        /* the head of the pipeline is being serviced, check if it will be removed during the next cycle,
         * if so accept the request */
        if(cacheit->write)
        { switch(cacheit->state)
          { case WRITE:
              /* the head of the pipeline is a write into the cache, accept the new request if this access
               * hits the cache */
              return cache[cfg].hit(cacheit->address);
            default:
              /* if the head of the pipeline is a write that is not in the WRITE state (ex: being requested
               * to the memory system) the new request can not be accepted */
              return false;
          }
        }
        else // not a write
        { switch(cacheit->state)
          { case READ_HIT_READY:
            case READ_MISS_LINE_READY:
            case READ_MISS_EVICTION_WRITE_DONE_LINE_READY:
              /* the head of the pipeline is a read that is being replying to the cpu, accept the new request if the
               * cpu is accepting the reply and all the reply can be sent */

	      // DD: here is a bug because return buffer is also used to return data on the bus (snooping)
	      //     and in these case we must check inMEM.accept[cfg] !!!
	      //     Any way, the returnBuffer is not up to date until end_of_cycle...
              if(outCPU.accept[cfg]) return false; //(returnBuffer[cfg].size <= nCachetoCPUDataPathSize);
              else              return false;
            default:
              /* if the head of the pipeline is a read that is not in one of the previous states (ex: it is a miss being
               * requested to the memory system) the new request can not be accepted */
              return false;
          }
        }    
      }
    } // for
    //Reached when the queue is full and won't be freed (in case of ndelay>1 for instance)
    cerr << __FILE__ << " " << __LINE__ << " should not reach this line" << endl;
    terminate_now();
    return false;
//    exit(1);
  }

  /**
   * \brief Returns true if the address is a hit in the currently queued request. (If it target the same address as a pending
   * request.
   */
  bool request_queue_hit(address_t addr, int cfg)
  { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit = cacheQueue[cfg].SeekAtHead();
    // If the cache queue is empty, it is not a queue hit
    if(!cacheit) return false;
    // Else compare address of the head of the queue and the provided address
    address_t mr_line_addr = addr & ~(nLineSize - 1);
    address_t cq_line_addr = cacheit->address & ~(nLineSize - 1);
    if(mr_line_addr == cq_line_addr)
    { 
    
//      INFO << "\e[1;32mhit? \e[0m" << *cacheit << endl;
    
      switch(cacheit->state)
      { case READ_MISS:
        case READ_MISS_EVICTION:
        case WRITE_MISS:
        case WRITE_MISS_EVICTION:
        case PREFETCH_MISS:
        case PREFETCH_MISS_EVICTION:
          // A matching request is in the queue but hasn't been successfully sent to the hierarchy
          // yet. So we can consider it is a local miss.
          return false;
          break;
        case READ_MISS_REQUESTED:
        case READ_MISS_READY:
        case READ_MISS_LINE_READY:
        case READ_MISS_SERVICED:
        case READ_MISS_EVICTION_WRITE_DONE:
        case READ_MISS_EVICTION_WRITE_DONE_REQUESTED:
        case READ_MISS_EVICTION_WRITE_DONE_READY:
        case READ_MISS_EVICTION_WRITE_DONE_LINE_READY:
        case READ_MISS_EVICTION_WRITE_DONE_SERVICED:
        case WRITE_MISS_REQUESTED:
        case WRITE_MISS_EVICTION_WRITE_DONE:
        case WRITE_MISS_EVICTION_WRITE_DONE_REQUESTED:
        case PREFETCH_MISS_REQUESTED:
        case PREFETCH_MISS_EVICTION_WRITE_DONE:
        case PREFETCH_MISS_EVICTION_WRITE_DONE_REQUESTED:
          // A matching request has been found in the queue, and a corresponding request has already
          // been sent (and accepted) by the memory hierarchy. Consider it is a local hit.
          return true;
          break;
        default:
          ERROR << "unhandled cacheit state: " << *cacheit << endl;
          exit(1);
      }
    }
    return false;
  }

  /** 
   * \brief Process lauched upon receiving a data from CPU.
   *
   * Sensitive to inCPU.data and outCPU.accept
   * 
   * Accepts new requests if the request can be filled into the cache pipeline.
   *
   * Previously called CheckCPURequests()
   */
  void on_CPU_data()
  { if(inCPU.data.known() && outCPU.accept.known())
    { // Upon receiving a request, check if it can be serviced/accepted
      //      INFO << "[DD_DEBUG_DCACHE] <DCACHE> on_CPU_data() REQ: " << inCPU.data << endl;
#ifdef DD_DEBUG_DCACHE_VERB100
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr << "[Cnonfig::all][DD_DEBUG_DCACHE("<<timestamp()<<")("<< this->name() <<")]  Begin of on_CPU_Data..." << endl;
    }
#endif
      for(int cfg=0; cfg<nConfig; cfg++)
      {
	if(inCPU.data[cfg].something()) 
	{
#ifdef DD_DEBUG_DCACHE_VERB101
	  if (DD_DEBUG_TIMESTAMP <= timestamp())
	  {
	    cerr << "[Cnonfig::"<<cfg<<"][DD_DEBUG_DCACHE("<<timestamp()<<")("<< this->name() <<")] on_CPU_data() : REQ                  = " << inCPU.data[cfg] << endl;
	    cerr << "[Cnonfig::"<<cfg<<"][DD_DEBUG_DCACHE("<<timestamp()<<")("<< this->name() <<")] on_CPU_data() : can_accept_request() = " << (can_accept_request(cacheQueue[cfg], cfg)?"True":"False") << endl;
	  }
#endif
	  inCPU.accept[cfg] = can_accept_request(cacheQueue[cfg], cfg);
//cerr <<"C ";
	}
	else 
	{ 
#ifdef DD_DEBUG_DCACHE_VERB101
	  if (DD_DEBUG_TIMESTAMP <= timestamp())
	  {
	    cerr << "[Cnonfig::"<<cfg<<"][DD_DEBUG_DCACHE("<<timestamp()<<")("<< this->name() <<")] on_CPU_data() : inCPU.accept = false " << endl;
	  }
#endif
	  inCPU.accept[cfg] = false;
	}
      }// end of foreach Config.
      inCPU.accept.send();
#ifdef DD_DEBUG_DCACHE_VERB100
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr << "[Cnonfig::all][DD_DEBUG_DCACHE("<<timestamp()<<")("<< this->name() <<")]  End of on_CPU_Data..." << endl;
    }
#endif
    }   
  }

  /**
   * \brief Process lauched upon receiving a data from memory.
   *
   * Sensitive to inMEM.data (and outCPU.accept if snooping)
   *
   * Accepts every (non-nothing) answer from memory 
   *
   * if cache_snooping, also accepts cpu requests that correpond to a hit
   * in this cache, as long as the pipeline can be filled with the new
   * request.
   *
   * Previously called CheckMemValid()
   */
  void on_MEM_data()
  { if(!inMEM.data.known()) return;
    if(Snooping)
    { if(!outCPU.accept.known()) return;
    }
#ifdef DD_DEBUG_DCACHE_VERB102
	  if (DD_DEBUG_TIMESTAMP <= timestamp())
	    {
	      cerr << "[Config::all][DD_DEBUG_DCACHE("<<timestamp()
		   <<")("<< this->name() <<")]  Begin onMEMdata..." << endl;
	    }
#endif
    for(int cfg=0; cfg<nConfig; cfg++)
    {
    if(inMEM.data[cfg].something())
    { if(Snooping)
      { memreq<INSTRUCTION, nMemtoCacheDataPathSize> mr = inMEM.data[cfg];
        if(mr.command == memreq_types::cmd_FLUSH)
        { 
#ifdef DD_DEBUG_DCACHE_VERB102
	  if (DD_DEBUG_TIMESTAMP <= timestamp())
	    {
	      cerr << "[Config::"<<cfg<<"][DD_DEBUG_DCACHE("<<timestamp()
		   <<")("<< this->name() <<")]  onMEMdata cmd_FLUSH..." << endl;
	    }
#endif
	  // Flush requests are always accepted, no need to store the request.
          inMEM.accept[cfg] = true;
          outSharedMEM.data[cfg] = false;
          //return;
	  continue;
        }

        switch(mr.message_type)
        { case memreq_types::type_REQUEST:
          { // Set the shared if the data seeked by the request is in this cache, 
            // or already requested by the cache
            bool local_hit = cache[cfg].hit(mr.address);
            bool can_accept = true;

//zzz
            if(!local_hit)
            { inMEM_was_a_cacheQueue_hit[cfg] = request_queue_hit(mr.address, cfg);
              if(inMEM_was_a_cacheQueue_hit[cfg])
              { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit = cacheQueue[cfg].SeekAtHead();
                address_t line_addr = mr.address & ~(nLineSize - 1);
		/*
                cerr << "[[[---------------------------------------------" << endl;
                INFO << hex << "\e[1;31m" << line_addr << dec << "\e[0m non local hit: " << mr << endl;
                INFO << hex << "\e[1;31m" << line_addr << dec << "\e[0m cacheit: " << *cacheit << endl;
                cerr << " ==>\e[1;32m local hit\e[0m" << endl;
                cerr << "---------------------------------------------]]]" << endl;
		*/
                local_hit = true;
              }
            }
            inMEM_was_a_hit[cfg] = local_hit;

            if(local_hit)
            { // The request made a local cache hit, shared should be set to true.
              // The data can only be accepted if the pipeline has a free slot for
              // this new request.
              outSharedMEM.data[cfg] = true;
              inMEM.accept[cfg] = can_accept && can_accept_request(cacheSnoopQueue[cfg], cfg);
//cerr <<"M ";
#ifdef DD_DEBUG_DCACHE_VERB102
	      if (DD_DEBUG_TIMESTAMP <= timestamp())
		{
		  cerr << "[Config::"<<cfg<<"][DD_DEBUG_DCACHE("<<timestamp()
		       <<")("<< this->name() <<")]  onMEMdata cmd_REQUEST LocalHit..." << endl;
		}
#endif
            }
            else
            { // Not a local cache hit, shared is to be set to false and 
              // the data can be accepted as it does not create any new request.
              outSharedMEM.data[cfg] = false;
              inMEM.accept[cfg] = true; 
#ifdef DD_DEBUG_DCACHE_VERB102
	      if (DD_DEBUG_TIMESTAMP <= timestamp())
	      {
		cerr << "[Config::"<<cfg<<"][DD_DEBUG_DCACHE("<<timestamp()
		     <<")("<< this->name() <<")]  onMEMdata cmd_REQUEST No LocalHit..." << endl;
	      }
#endif
            }
          } break;
          case memreq_types::type_ANSWER:
            // Answers from memory should always be accepted, and shared should
            // not be set for memory answers.
            outSharedMEM.data[cfg] = false;
            inMEM.accept[cfg] = true;
#ifdef DD_DEBUG_DCACHE_VERB102
	      if (DD_DEBUG_TIMESTAMP <= timestamp())
	      {
		cerr << "[Config::"<<cfg<<"][DD_DEBUG_DCACHE("<<timestamp()
		     <<")("<< this->name() <<")]  onMEMdata cmd_ANSWER..." << endl;
	      }
#endif
//INFO << "accepted an anwser, because i accept every answer." << endl;
            break;
          default:
            ERROR << "Unknown message type in request " << mr << endl;
            exit(1);
        }
      }
      else // !Snooping
      { // We are currently waiting for an answer from memory. Let's accept it.
#ifdef DD_DEBUG_DCACHE_VERB2
	if (DD_DEBUG_TIMESTAMP <= timestamp())
	{
	  cerr << "["<<this->name()<<"("<<timestamp()<<")]: Accepting Data from Memory (Always)..." << endl;
	}
#endif
        inMEM.accept[cfg] = true;
#ifdef DD_DEBUG_DCACHE_VERB102
	      if (DD_DEBUG_TIMESTAMP <= timestamp())
	      {
		cerr << "[Config::"<<cfg<<"][DD_DEBUG_DCACHE("<<timestamp()
		     <<")("<< this->name() <<")]  onMEMdata No Snooping..." << endl;
	      }
#endif
      }
    } // !Something
    else 
    {
#ifdef DD_DEBUG_DCACHE_VERB2
      if (DD_DEBUG_TIMESTAMP <= timestamp())
      {
	cerr << "["<<this->name()<<"("<<timestamp()<<")]: Non-Accepting Data from Memory (!something)..." << endl;
      }
#endif
      inMEM.accept[cfg] = false;
      if(Snooping) outSharedMEM.data[cfg].nothing();
#ifdef DD_DEBUG_DCACHE_VERB102
      if (DD_DEBUG_TIMESTAMP <= timestamp())
      {
	cerr << "[Config::"<<cfg<<"][DD_DEBUG_DCACHE("<<timestamp()
	     <<")("<< this->name() <<")]  onMEMdata received Nothing..." << endl;
      }
#endif
    }
    } // end of foreach Config.
    inMEM.accept.send();
    outSharedMEM.data.send();
#ifdef DD_DEBUG_DCACHE_VERB102
	  if (DD_DEBUG_TIMESTAMP <= timestamp())
	    {
	      cerr << "[Config::all][DD_DEBUG_DCACHE("<<timestamp()
		   <<")("<< this->name() <<")]  End onMEMdata..." << endl;
	    }
#endif
  } // end of on_MEM_data()
  
  /**
   * \brief Process setting accept signals
   */
  void on_data()
  { // Check that cpu data has arrived

//cerr << name() << ": CPU.data=" << inCPU.data.known()  << " accept=" << outCPU.accept.known() << " MEM.data=" << inMEM.data.known() << endl; STF!!!


    if(!inCPU.data.known()) return;
    if(!outCPU.accept.known()) return;
    // Check that memory data has arrived
    if(!inMEM.data.known()) return;
    if(Snooping)
    { if(!outCPU.accept.known()) return;
    }
    // Everything has been received, deal with received values
    // if snnoping, higher priority is given to snooping hit reuests than to cpu requests
    on_MEM_data();
    on_CPU_data();    
  }

  /**
   * \brief Process lauched upon receiving an acept on the shared bit output port
   */
  void on_shared_accept()
  {
    for(int cfg=0; cfg<nConfig; cfg++)
    {
      outSharedMEM.enable[cfg] = outSharedMEM.accept[cfg];
    }
    outSharedMEM.enable.send();
  }

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
  
 public:
  /**
   * \brief Checks if data sent to the cpu has been accepted and modify the cache pipeline state if needed
   *
   * Used at End of cycle
   */
  void CheckCPUAcceptedData()
  { 
    for(int cfg=0; cfg<nConfig; cfg++)
    {
    QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;

    /* check if an accept from the cpu is being waited */
    if(returnBuffer[cfg].ready)
    { /* check if an accept is received */
      if(outCPU.accept[cfg])
      { /* decrement the size of the data being sent to the cpu */
        returnBuffer[cfg].size = returnBuffer[cfg].size - nCachetoCPUDataPathSize;
        /* if there is still data to be sent set up the returnBuffer,
         * if all the data has been sent modify the state of the pipeline */
        if(returnBuffer[cfg].size > 0)
        { /* increment the index of the data being sent */
          returnBuffer[cfg].index = returnBuffer[cfg].index + nCachetoCPUDataPathSize;
          if(returnBuffer[cfg].index == nCPULineSize)
            returnBuffer[cfg].index = 0;
          /* set the address of the data being sent */
          returnBuffer[cfg].address = returnBuffer[cfg].base + returnBuffer[cfg].index;
        }
        else
        { /* the returnBuffer is empty */
          /* the cache entry that made the request can be
           * removed, it must be the head the entry to remove */
          cacheit = cacheQueue[cfg].SeekAtHead();
          switch(cacheit->state)
          { case READ_HIT_READY:
              /* a read hit can be removed */
            case READ_MISS_LINE_READY:
              /* if it is a miss which line has been completely received it can be removed */
            case READ_MISS_EVICTION_WRITE_DONE_LINE_READY:
              /* if it is a miss (with eviction) which line has been completely received it can be removed */
            case READ_UNCACHABLE_READY:
              // If an uncachable read result has been sent back to the cpu, remove the request
              cacheQueue[cfg].RemoveHead();
              break;
            case READ_MISS_READY:
              /* a read miss which data is ready but has not received the complete line that caused
               * the miss changes its state as being serviced but it is not removed */
              cacheit->state = READ_MISS_SERVICED;
              break;
            case READ_MISS_EVICTION_WRITE_DONE_READY:
              /* a read miss (with eviction) which data is ready but has not received the complete line that caused
              * the miss changes its state as being serviced but it is not removed */
              cacheit->state = READ_MISS_EVICTION_WRITE_DONE_SERVICED;
              break;
          }
          /* reset the returnBuffer */
          returnBuffer[cfg].ready = false;
        }
      }
    }
    }//endof foreach Config.
  }

  /**
   * \brief Checks if a request sent to the memory system has been accepted and modify the cache pipeline state if needed 
   *
   * Used at End of cycle
   */
  void CheckMemAcceptedDatafromCache()
  { 
    for (int cfg=0; cfg<nConfig; cfg++)
    {
    QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;
    /* are we waiting for an accept from the memory? */
    if(requestBuffer[cfg].ready)
    { /* check if the memory accepted the request */
      if(requestBuffer[cfg].cacheQueueIndex!=cacheQueueIndex) continue;//return;
      if(outMEM.accept[cfg])
      { if(requestBuffer[cfg].write)
        { /* this request is a write */
          /* if last request was finished from the cache pipeline (a write request)
           * remove the request from the cache pipeline */
          requestBuffer[cfg].size -= nCachetoMemDataPathSize;
          if(requestBuffer[cfg].size > 0)
          { /* the request has not bben finished, the request has to be keeped,
             * the address has to be increased,
             * and the cache pipeline is not notified */
            requestBuffer[cfg].index += nCachetoMemDataPathSize;
            requestBuffer[cfg].address = requestBuffer[cfg].base + requestBuffer[cfg].index;
          }
          else
          { /* the request has been finished, the cache pipeline has to be updated */
            cacheit = cacheQueue[cfg].SeekAtHead();
            switch(cacheit->state)
            { /* write cases */
              case WRITE_MISS_EVICTION:
                /* the eviction has been done, change the state of the head pipeline */
                cacheit->state = WRITE_MISS_EVICTION_WRITE_DONE;
                break;
                /* read cases */
              case READ_MISS_EVICTION:
                /* the eviction has been done, change the state of the head pipeline */
                cacheit->state = READ_MISS_EVICTION_WRITE_DONE;
                break;
                /* prefetch cases */
              case PREFETCH_MISS_EVICTION:
                /* the eviction has been done, change the state of the head pipeline */
                cacheit->state = PREFETCH_MISS_EVICTION_WRITE_DONE;
                break;
                /* evict cases */
              case EVICT_READY:
                /* the eviction has been completed, remove the head pipeline */
                cacheQueue[cfg].RemoveHead();
                break;

              case WRITE_UNCACHABLE_READY:
                // The uncachable write has been performed
                cacheQueue[cfg].RemoveHead();
                break;

            #ifdef NOC_THREADS_DISTRIBUTION
              case WRITE_UNCACHABLE:
                // The READX to get the uncachable write has been accepted from memory
                cacheQueue[cfg].RemoveHead();
            #endif     
                break; 
		
              /////////////////////
              // Snooping case   //
              /////////////////////
              case READ_HIT_READY:
                if(!Snooping)
                { ERROR << "Should never happen when snooping is disabled" << endl;
                  exit(1);
                }
                // A snooped read hit has been sent and accepted, i can remove it from the queue
                cacheQueue[cfg].RemoveHead();
                break;
            }
            /* reset the request buffer */
            requestBuffer[cfg].ready=false;
          }
        }
        else // not a write
        { /* the access is a cache miss
           * the cache entry (the head of the cache queue) 
           * is not removed from the cache pipeline (this is a blocking cache) */
          /* the request buffer is marked as not ready, to be used with the next memory request */
          cacheit = cacheQueue[cfg].SeekAtHead();
          switch(cacheit->state)
          { /* write cases */
            case WRITE_MISS:
              /* the read request has been accepted, change the state of the head of the pipeline */
              cacheit->state = WRITE_MISS_REQUESTED;
              break;
            case WRITE_MISS_EVICTION_WRITE_DONE:
              /* the read request has been accepted, change the state of the head of the pipeline */
              cacheit->state = WRITE_MISS_EVICTION_WRITE_DONE_REQUESTED;
              break;
              /* read cases */
            case READ_MISS:
              /* the read request has been accepted, change the state of the head of the pipeline */
              cacheit->state = READ_MISS_REQUESTED;
              break;
            case READ_MISS_EVICTION_WRITE_DONE:
              /* the read request has been accepted, change the state of the head of the pipeline */
              cacheit->state = READ_MISS_EVICTION_WRITE_DONE_REQUESTED;
              break;
              /* prefetch cases */
            case PREFETCH_MISS:
              /* the read request has been accepted, change the state of the head of the pipeline */
              cacheit->state = PREFETCH_MISS_REQUESTED;
              break;
            case PREFETCH_MISS_EVICTION_WRITE_DONE:
              /* the read request has been accepted, change the state of the head of the pipeline */
              cacheit->state = PREFETCH_MISS_EVICTION_WRITE_DONE_REQUESTED;
              break;
            case WRITE_HIT_FLUSH:
              // A cache flush request has been accepted, so other caches will invalidate.
              // We can get rid of the request.
//              cacheQueue.RemoveHead();         replaced by what is below
//STF
              // A cache flush request has been accepted, the write will be done once receiving the
              // corresponding flush to synchronize other cache flush, and local cache write
              cacheit->state = WRITE_HIT_FLUSH_REQUESTED;
//EOSTF

              break;
            case READ_UNCACHABLE:
              cacheit->state = READ_UNCACHABLE_REQUESTED;
#ifdef DEBUG_TEST_UNCACHABLE_READ
              INFO << "read uncachable accepted" << endl;
#endif
              break;
            case WRITE_UNCACHABLE:
              // The READX to get the uncachable write has been accepted from memory
              cacheit->state = WRITE_UNCACHABLE_REQUESTED;
              break;
            default:
              ERROR << "Don't know what to do when a non-writing request of state " << cacheit->state << " has been sent then accepted." << endl;
              exit(1);
          }
          /* reset the request buffer */
          requestBuffer[cfg].ready=false;
        }
      }
    }
#ifdef DEBUG_CACHEWB
    else
    {
      /* if an accept from the memory system that was received that was not expected, error */
      if(outMEM.accept[cfg])
      { cerr << "Error(" << name() << "): an accept was received from the memory system that was not expected" << endl;
        cerr << *this;
        exit(-1);
      }
    }
#endif
    }//Endof foreach Config.
  }

  /**
   * \brief Checks a request or a reply has been accepted by the memory system or by the cpu respectively 
   *
   * Used at End of cycle
   */
  void CheckAcceptedData()
  { CheckCPUAcceptedData();
    CheckMemAcceptedDatafromCache();
    if(Snooping) 
    { CheckSnoopMemAcceptedDatafromCache();
    }
  }


  /**
   * \brief Checks that the memory system has sent data to the cache and update the write buffer if necessary 
   *
   * Called at end of cycle
   */
  void UpdateWriteBuffer(int cfg)
  {/* check that data was received from the memory system */
    if(inMEM.enable[cfg])
    { 
#ifdef DEBUG_BUS_MQ_VERB100
      if (DD_DEBUG_TIMESTAMP <= timestamp())
      {
	INFO << "[CFG::"<<cfg<<"]Receiving from M:   " << inMEM.data[cfg] << endl;
      }
#endif
#ifdef DD_DEBUG_DCACHE_VERB2
      if (DD_DEBUG_TIMESTAMP <= timestamp())
      {
	cerr <<"["<<this->name()<<"("<<timestamp()<<")]: Warning inMEM.enable == True ..." << endl;
      }
#endif
      memreq<INSTRUCTION, nMemtoCacheDataPathSize> mr = inMEM.data[cfg];
      if(Snooping)
      { if(mr.message_type==memreq_types::type_REQUEST)
        { // Snooped requests should not update the write buffer, ignore those
#ifdef DD_DEBUG_DCACHE_UPDATES_VERB100 
	  if (DD_DEBUG_TIMESTAMP <= timestamp())
	    {
	      cerr << "[CFG::"<<cfg<<"]["<<this->name()<<"("<<timestamp()<<")]UPDATE Return: message_type = REQUEST..." << endl;
	    }
#endif
         return;
        }
        else 
        { // The message type is not an request, it is an answer.
          // Discards answers form which request are not issued by this cache
	  stringstream sstr;
	  sstr << this->name() << cfg;
	  //          if(mr.req_sender!=this) 
          if(mr.req_sender!=sstr.str()) 
          { 
            //INFO << "Discarded beacause it is not mine" << endl;
#ifdef _CACHE_MESSAGES_SVG_H_
            // Received an answer for another cache, so I discarded it.
            stringstream ss;
            ss << "$" << nProg;
            SVG.add(timestamp(),"BS",ss.str(),"X",mr);
#endif
            if(svg)
            { svg->add_bus_to_cac(timestamp(),name(),"X",mr);
            }


/* STF: should invalidate on hitting write backs ! */
#ifdef DD_DEBUG_DCACHE_UPDATES_VERB100 
	  if (DD_DEBUG_TIMESTAMP <= timestamp())
	    {
	      cerr << "[CFG::"<<cfg<<"]["<<this->name()<<"("<<timestamp()<<")]UPDATE Return: I'm not the req_sender..." << endl;
	    }
#endif

            return;
          }
        }
      } // if(Snooping)
      
      // David Debug :
      // Do not update the write buffer with data not requested !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      stringstream sstr;
      sstr << this->name() << cfg;
      if (mr.req_sender != sstr.str()) 
      {
#ifdef DD_DEBUG_DCACHE_UPDATES_VERB100 
	if (DD_DEBUG_TIMESTAMP <= timestamp())
	  {
	    cerr << "[CFG::"<<cfg<<"]["<<this->name()<<"("<<timestamp()<<")]UPDATE Return: I'm not the req_sender..." << endl;
	  }
#endif
	return;
      }
      if(sstr.str()==mr.sender)
      { ERROR << "Receiving a request from myself in UpdateWriteBuffer : " << mr << endl;
        terminate_now() ;
      }

#ifdef DD_DEBUG_DCACHE_VERB2
      if (DD_DEBUG_TIMESTAMP <= timestamp())
      {
	cerr << "["<<this->name()<<"("<<timestamp()<<")]: inMEM.enable == True, we are storing into writebuffer..." << endl;
      }
#endif
#ifdef DEBUG_TEST_UNCACHABLE_WRITE
      if( !mr.cachable ) INFO << "<-MEM- \e[1;31muncachable write\e[0m: " << mr << endl;
#endif
 
#ifdef DEBUG_CACHEWB
      /* check that it is not an incorrect line */
      if(writeBuffer[cfg].address != (mr.address & ~((address_t)nLineSize-1)) && writeBuffer[cfg].size != 0)
      { if(writeBuffer[cfg].size == nLineSize)
        { cerr << "Error(" << name() << "): writeBuffer was full and a memory access has been received" << endl;
          cerr << *this;
          exit(-1);
        }
        else
        { cerr << "Error(" << name() << "): memory system has not finished actual writeBuffer" << endl;
          cerr << *this;
          exit(-1);
        }
      }
#endif
      /* update bytes that are received */
      for(int i = 0; i < nMemtoCacheDataPathSize; i++)
      {
#ifdef DEBUG_CACHEWB
        /* check that received bytes were not already received */
        if(writeBuffer[cfg].ready[(int)(i + (mr.address & ((address_t)(nLineSize - 1))))])
        { cerr << "Error(" << name() << "): writeBuffer has received two time the same address" << endl;
          cerr << *this;
          exit(-1);
        }
#endif
        writeBuffer[cfg].ready[i + (mr.address & ((address_t)(nLineSize - 1)))] = true;
      }
      // update writeBuffer address, size, address and uid fields with incomming data
      memcpy(&(writeBuffer[cfg].buffer[mr.address&((address_t)(nLineSize - 1))]), mr.Read(), nMemtoCacheDataPathSize);
      writeBuffer[cfg].size += nMemtoCacheDataPathSize;
      writeBuffer[cfg].address = mr.address&(~((address_t)(nLineSize - 1)));
      writeBuffer[cfg].uid = mr.uid;
      writeBuffer[cfg].memreq_id = mr.memreq_id;
      writeBuffer[cfg].cachable = mr.cachable;

#ifdef DD_DEBUG_DCACHE_UPDATES_VERB100 
	  if (DD_DEBUG_TIMESTAMP <= timestamp())
	    {
	      cerr << "[CFG::"<<cfg<<"]["<<this->name()<<"("<<timestamp()<<")]UPDATE Here we have updated WriteBuffer:" << endl;
	      cerr << writeBuffer[cfg] << endl;
	    }
#endif
     // If the shared bit was not set during the corresponding request, we will get the answer
     // from memory as a READ ANSWER.
     // If the shared bit was set, we will get the answer from a WRITE-BACK to memory of another
     // cache snooped on the bus.
     if(Snooping) writeBuffer[cfg].shared_bit = (mr.command==memreq_types::cmd_WRITE);
//      if(Snooping) writeBuffer.shared_bit = inSharedMEM.data;

//INFO << "Stored   " << mr << " from MEM" << endl;
#ifdef _CACHE_MESSAGES_SVG_H_
      // Received an answer for this cache, and stored it.
      stringstream ss;
      ss << "$" << nProg;
      SVG.add(timestamp(),"BS",ss.str(),"",mr);
#endif
      if(svg)
      { svg->add_bus_to_cac(timestamp(),name(),"",mr);
      }

#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr << "["<<this->name()<<"("<<timestamp()<<")]: inMEM.enable == True WriteBuffer :" << endl;
      cerr << writeBuffer[cfg] << endl;
    }
#endif

    } /* inMEM.enable */ 
    else
    {
#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr <<"["<<this->name()<<"("<<timestamp()<<")]: Warning inMEM.enable == False ..." << endl;
    }
#endif
    }
  }

  /**
   * \brief Updates the cache pipeline with data from the writeBuffer if the data requested by the
   * head of the pipeline is ready 
   */
  void UpdateCachePipelineWithWriteBuffer(int cfg)
  { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;

    /* the cache can return a requested data to the cpu if the cache line in the writeBuffer
     * already contains the data, this way the cpu has not to wait for the full line before
     * receiving a cache answer */
    /* does the writeBuffer have data? */
    if(writeBuffer[cfg].size != 0)
    { cacheit = cacheQueue[cfg].SeekAtHead();
      /* the action is just performed if the cache request was a read */
      if(cacheit)
      { if(!cacheit->write)
        {
	  //#ifdef DEBUG_CACHEWB
          /* check that the head of the pipeline and the writeBuffer have the same address */
          if((cacheit->address & (~(address_t)(nLineSize - 1))) != writeBuffer[cfg].address)
	    { cerr << "[CONFIG::"<<cfg<<"][TS("<<timestamp()<<")]Error(" << name() << "): the cache line request address does not correspond "
            << "with the address in the writeBuffer." << endl;
            cerr << "\t\t\tstate = " << cacheit->state
            << " (" << hexa(cacheit->address & (~(address_t)(nLineSize - 1)))
            << " -- " << hexa(writeBuffer[cfg].address) << ")" << endl;
            cerr <<  *this;
            exit(-1);
          }
	  //#endif
          /* get the index of the data in the writeBuffer */
          unsigned int base_address = 0;
          if(nCPULineSize > 0)
          { base_address = cacheit->address & ((address_t)nLineSize - 1);
            base_address = base_address - (cacheit->address & ((address_t)nCPULineSize - 1));
          }
          else
          { base_address = cacheit->address & ((address_t)nLineSize - 1);
          }
#ifdef DD_DEBUG_DCACHE_VERB101
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr <<"[CFG::"<<cfg<<"]["<<this->name()<<"("<<timestamp()<<")]: " << endl;
          cerr <<"      address       ="<< hexa(cacheit->address) <<endl;
          cerr <<"      size          ="<< cacheit->size << endl;
          cerr <<"      base_address  ="<< hexa(base_address) << endl;
          cerr <<"      nLineSize     ="<< nLineSize << endl;
          cerr <<"      nCPULineSize  ="<< nCPULineSize << endl;
    }
#endif
          /* check that all the bytes requested by the head of the pipeline are ready */
          bool ok = true;
          for(unsigned int i = base_address; ok && i < base_address + cacheit->size; i++)
          { if(!writeBuffer[cfg].ready[i]) ok = false;
          }
          /* if all the data is ready, the prepare the head of the pipeline to reply to the cpu */
          if(ok)
          {
#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
            cerr <<"["<<this->name()<<"("<<timestamp()<<")]: Copy writeBuffer to pipeline entry ..." << endl;
    }
#endif
            /* copy the requested data */
            memcpy(cacheit->data, &(writeBuffer[cfg].buffer[base_address]), cacheit->size);
            /* set the pipeline entry as ready */
            cacheit->ready=true;
            /* set up state */
            switch(cacheit->state)
            { case READ_MISS_REQUESTED:
                cacheit->state = READ_MISS_READY;
                break;
              case READ_UNCACHABLE_REQUESTED:
                cacheit->state = READ_UNCACHABLE_READY;
#ifdef DEBUG_TEST_UNCACHABLE_READ
                INFO << "uncachable read response received" << endl;
#endif
                break;
              case READ_MISS_EVICTION_WRITE_DONE_REQUESTED:
                cacheit->state = READ_MISS_EVICTION_WRITE_DONE_READY;
                break;
            }
          } // if (ok)
#ifdef DD_DEBUG_DCACHE_VERB2
          else
          { 
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr <<"["<<this->name()<<"("<<timestamp()<<")]: Warning: Not Ok !!!!! ..." << endl;
    }
	  }
#endif
        } // if (!cacheit->write)
#ifdef DD_DEBUG_DCACHE_VERB2
      else
      {
	if (DD_DEBUG_TIMESTAMP <= timestamp())
	  { 
	    cerr <<"["<<this->name()<<"("<<timestamp()<<")]: Warning: (!cacheit->write)==False ..." << endl;
	  }
      }
#endif
      } // if (cacheit)
#ifdef DD_DEBUG_DCACHE_VERB2
      else
	{ 
	  if (DD_DEBUG_TIMESTAMP <= timestamp())
	    {
	      cerr <<"["<<this->name()<<"("<<timestamp()<<")]: Warning: no cacheit ..." << endl;
	    }
	}
#endif
    } // if(writeBuffer.size != 0)
#ifdef DD_DEBUG_DCACHE_VERB2
    else
    { 
      if (DD_DEBUG_TIMESTAMP <= timestamp())
      {
	cerr <<"["<<this->name()<<"("<<timestamp()<<")]: Warning writeBuffer.size == 0 ..." << endl;
      }
    }
#endif
  }

  /**
   * \brief Reads data sent by the memory system and updates the writeBuffer, the cache pipeline and the cache as needed 
   *
   * Called at end of cycle
   */
  void ReadMemData()
  {
    for (int cfg=0; cfg<nConfig; cfg++)
    {
      QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;
      // Update the writeBuffer if data is received from the memory system
#ifdef DD_DEBUG_DCACHE_UPDATES_VERB100 
	  if (DD_DEBUG_TIMESTAMP <= timestamp())
	    {
	      cerr << "[CFG::"<<cfg<<"]["<<this->name()<<"("<<timestamp()<<")]UPDATE Before UpdateWriteBuffer()..." << endl;
	    }
#endif
      UpdateWriteBuffer(cfg);
#ifdef DD_DEBUG_DCACHE_UPDATES_VERB100 
	  if (DD_DEBUG_TIMESTAMP <= timestamp())
	    {
	      cerr << "[CFG::"<<cfg<<"]["<<this->name()<<"("<<timestamp()<<")]UPDATE After UpdateWriteBuffer()..." << endl;
	    }
#endif
      // Check if the requested data is ready without waiting that the complete line is received
#ifdef DD_DEBUG_DCACHE_UPDATES_VERB100 
	  if (DD_DEBUG_TIMESTAMP <= timestamp())
	    {
	      cerr << "[CFG::"<<cfg<<"]["<<this->name()<<"("<<timestamp()<<")]UPDATE Before UpdateCachePipe()..." << endl;
	    }
#endif
      UpdateCachePipelineWithWriteBuffer(cfg);
#ifdef DD_DEBUG_DCACHE_UPDATES_VERB100 
	  if (DD_DEBUG_TIMESTAMP <= timestamp())
	    {
	      cerr << "[CFG::"<<cfg<<"]["<<this->name()<<"("<<timestamp()<<")]UPDATE After UpdateCachePipe()..." << endl;
	    }
#endif
      
      /* if writeBuffer has been completed write the cacheline to the cache */
      if(writeBuffer[cfg].size == nLineSize)// || writeBuffer[cfg].size == nLineSize)
      { /* check if the cache entry that made the request is a write
	 * and use it to update the contents of the writeBuffer before
	 * writing it to the cache */
	cacheit = cacheQueue[cfg].SeekAtHead();
	if(cacheit) 
	  /*
	    if (cacheit)
	    {
	    if (
	    ( (cacheit->address&((address_t)nLineSize - 1) + cacheit->size) < nLineSize && writeBuffer.size == nLineSize ) 
	    ||
	    ( (cacheit->address&((address_t)nLineSize - 1) + cacheit->size) > nLineSize && writeBuffer.size == 2*nLineSize )
	    )
	  */
	  { /* if the request was a write the update the data in the writeBuffer before updating the cache */
	    if(cacheit->write)
	      {
		//#ifdef DEBUG_CACHEWB
		/* check that the addresses match */
		if((cacheit->address & (~(address_t)(nLineSize - 1))) != writeBuffer[cfg].address)
		  { cerr << "[CFG::"<<cfg<<"][TS("<<timestamp()<<")]Error(" << name() << "): the cache line request address does not correspond with the address in the "
			 << "writebuffer" << endl;
		  cerr << "\t\t\tstate = " << cacheit->state
		       << " (" << hexa(cacheit->address & (~(address_t)(nLineSize - 1)))
		       << " -- " << hexa(writeBuffer[cfg].address) << ")" << endl;
		  cerr << *this;
		  exit(-1);
		  }
		//#endif
		/* calculate the index in the writeBuffer that is needed by the request */
		unsigned int base_address = 0;
		if(nCPULineSize > 0)
		  { base_address = cacheit->address&((address_t)nLineSize - 1);
		  base_address = base_address - (cacheit->address & ((address_t)nCPULineSize - 1));
		  }
		else
		  { base_address = cacheit->address & ((address_t)nLineSize - 1);
		  }
		/* modify the writeBuffer with the request data */
		memcpy(&(writeBuffer[cfg].buffer[base_address]), cacheit->data, cacheit->size);
	      }
	    /* modify the state of the cache entry */
	    
	    //INFO << "\e[1;33m####\e[0m cacheit->state=" << cacheit->state << " shared_bit=" << writeBuffer.shared_bit<< endl;
	    
	    switch(cacheit->state)
	      { /* write cases */
	      case WRITE_MISS_REQUESTED:
	      case WRITE_MISS_EVICTION_WRITE_DONE_REQUESTED:
		/* write writeBuffer into the cache */
		cache[cfg].setCacheLine(cacheit->set, cacheit->line, writeBuffer[cfg].address,
				   (char *)writeBuffer[cfg].buffer,
				   true,      /* valid bit */
				   true);     /* write bit */
		
		/* [DAVID] set MESI properly */
		if(Snooping)
		  { // It's a miss (that may have been evicted). First set the current state to invalidate, as
		    // We'll replace the line.
		    cache[cfg].mesi_invalidate(cacheit->set, cacheit->line);
		    bool ret = cache[cfg].mesi_transition(cacheit->set, cacheit->line, MESI::transition_PrWr_BusRdX,__FILE__,__LINE__,nProg);
		    if(ret)
		      { ERROR << "Invalid MESI transition from " << *cacheit << endl;
		      exit(1);
		      }
		  }
		
		/* the request can be removed */
		cacheQueue[cfg].RemoveHead();
		break;
	      case READ_UNCACHABLE_READY:
		// Uncachable read is back from the memory and will be sent to the cpu. No cache line is modified.
		break;
	      case WRITE_UNCACHABLE_REQUESTED:
		// Received the cache line from memory. Now ready to put the new data in the line and to send
		// back the line to the memory.
		cacheit->state = WRITE_UNCACHABLE_READY;
		//            memcpy(cacheit->data, &(writeBuffer.buffer[base_address]), cacheit->size);
		
		memcpy(cacheit->linedata,writeBuffer[cfg].buffer,writeBuffer[cfg].size);
#ifdef DEBUG_TEST_UNCACHABLE_WRITE
		//ZZZZZZZZZZZZZ
		cerr << endl;
		INFO << "HERE: " << *cacheit << endl;
		INFO << "writeBuffer " << writeBuffer[cfg] << endl;
		cerr << endl;
#endif
		break;
		/* read cases */
	      case READ_MISS_READY:
		/* write writeBuffer into the cache */
		cache[cfg].setCacheLine(cacheit->set, cacheit->line, writeBuffer[cfg].address,
                               (char *)writeBuffer[cfg].buffer,
				   true,                   /* valid bit */
				   false);                 /* write bit */
		
            // A processor read miss implied a bus read that may be shared
		if(Snooping)
		  { // It's a miss (that may have been evicted). First set the current state to invalidate, as
		    // We'll replace the line.
		    cache[cfg].mesi_invalidate(cacheit->set, cacheit->line);
		    bool ret=true;
		    if(writeBuffer[cfg].shared_bit) ret = cache[cfg].mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrRd_BusRd_S,__FILE__,__LINE__,nProg);
		    else                       ret = cache[cfg].mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrRd_BusRd_NS,__FILE__,__LINE__,nProg);
		    if(ret)
		      { ERROR << "Invalid MESI transition from " << *cacheit << endl;
		      exit(1);
		      }
		  }
		
		/* change the state of the head of the pipeline */
		cacheit->state = READ_MISS_LINE_READY;
		break;
	      case READ_MISS_EVICTION_WRITE_DONE_READY:
		/* write writeBuffer into the cache */
		cache[cfg].setCacheLine(cacheit->set, cacheit->line, writeBuffer[cfg].address,
				   (char *)writeBuffer[cfg].buffer,
				   true,                   /* valid bit */
				   false);                 /* write bit */

		//After the eviction is done, the new data is received. /* [DAVID] set MESI properly */
		if(Snooping)
		  { // It's a miss (that may have been evicted). First set the current state to invalidate, as
		    // We'll replace the line.
		    cache[cfg].mesi_invalidate(cacheit->set, cacheit->line);
		    bool ret=true;
		    if(writeBuffer[cfg].shared_bit) ret = cache[cfg].mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrRd_BusRd_S,__FILE__,__LINE__,nProg);
		    else                       ret = cache[cfg].mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrRd_BusRd_NS,__FILE__,__LINE__,nProg);
		    if(ret)
		      { ERROR << "Invalid MESI transition from " << *cacheit << endl;
		      exit(1);
		      }
		  }
		
		/* change the state of the head of the pipeline */
		cacheit->state = READ_MISS_LINE_READY;
		break;
	      case READ_MISS_SERVICED:
	      case READ_MISS_EVICTION_WRITE_DONE_SERVICED:
		/* write writeBuffer into the cache */
		cache[cfg].setCacheLine(cacheit->set, cacheit->line, writeBuffer[cfg].address,
				   (char *)writeBuffer[cfg].buffer,
				   true,                   /* valid bit */
				   false);                 /* write bit */
		
		/* [DAVID] set MESI properly */
		if(Snooping)
		  { cache[cfg].mesi_invalidate(cacheit->set, cacheit->line);
		  bool ret=true;
              if(writeBuffer[cfg].shared_bit) ret = cache[cfg].mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrRd_BusRd_S,__FILE__,__LINE__,nProg);
              else                       ret = cache[cfg].mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrRd_BusRd_NS,__FILE__,__LINE__,nProg);
              if(ret)
		{ ERROR << "Invalid MESI transition from " << *cacheit << endl;
                exit(1);
		}
		  }
		
		/* the request can be removed */
		cacheQueue[cfg].RemoveHead();
		break;
		/* prefetch cases */
	      case PREFETCH_MISS_REQUESTED:
	      case PREFETCH_MISS_EVICTION_WRITE_DONE_REQUESTED:
		/* write writeBuffer into the cache */
		cache[cfg].setCacheLine(cacheit->set, cacheit->line, writeBuffer[cfg].address,
				   (char *)writeBuffer[cfg].buffer,
				   true,      /* valid bit */
				   false);    /* write bit */
		
		/* [DAVID] set MESI properly 
		 * if(Snooping)
		 * { if(writeBuffer.shared_bit) cache.mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrRd_BusRd_S,__FILE__,__LINE__,nProg);
		 *   else                       cache.mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrRd_BusRd_NS,__FILE__,__LINE__,nProg);
		 * }
		 */
		
		/* the request can be removed */
		cacheQueue[cfg].RemoveHead();
		break;
	      default:
		ERROR << "[Config::"<<cfg<<"]Don't know how to handle cacheit state " << cacheit->state << " when receiving answer from memory." << endl;
		cerr << "cacheit: " << *cacheit << endl;
		cerr << "writeBuffer " << writeBuffer[cfg] << endl;
		cerr << *this << endl;
		
		abort();
	      }
	    /* reset the writeBuffer */
	    writeBuffer[cfg].size = 0;
	    for(int i = 0; i < nLineSize; i++) writeBuffer[cfg].ready[i] = false;
#ifdef DD_DEBUG_DCACHE_UPDATES_VERB100 
	  if (DD_DEBUG_TIMESTAMP <= timestamp())
	    {
	      cerr << "[CFG::"<<cfg<<"]["<<this->name()<<"("<<timestamp()<<")]UPDATE Just after Reseting WriteBuffer()..." << endl;
	    }
#endif
	  } // end of if cacheit
#ifdef DD_DEBUG_DCACHE_UPDATES_VERB100 
	else
	{
	  if (DD_DEBUG_TIMESTAMP <= timestamp())
	  {
	      cerr << "[CFG::"<<cfg<<"]["<<this->name()<<"("<<timestamp()<<")]UPDATE WRANING No cacheit..." << endl;
	  }
	}
#endif
      } // end of if writeBuffer[cfg].size == ...
#ifdef DD_DEBUG_DCACHE_UPDATES_VERB100 
      else
      {
	if (DD_DEBUG_TIMESTAMP <= timestamp())
	{
	  cerr << "[CFG::"<<cfg<<"]["<<this->name()<<"("<<timestamp()<<")]UPDATE WRANING WriteBuffer Not full..." << endl;
	}
      }
#endif
    } // end of for each Config
  } // end of ReadMemData()
  
  /**
   * \brief Advance the requests in a cache pipeline 
   */
  void RunCachePipeline(Queue<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> &queue)
  { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit, cacheitPrev;

    /* try to advance the stage of each pipeline entry
     * an entry will advance if its delay is eaqual to 0 and there is not an entry in the next stage */
    for(cacheit = queue.SeekAtHead(); cacheit; cacheit++)
    { if(cacheit->delay[cacheit->stage] == 0)
      { if(cacheitPrev)
        { if(cacheitPrev->stage > cacheit->stage + 1)
          { cacheit->stage++;
          }
        }
        else
        { if(cacheit->stage < nStages - 1)
          { cacheit->stage++;
          }
        }
      }
      cacheitPrev = cacheit;
    }

    /* decrement the delay of each entry */
    for(cacheit = queue.SeekAtHead(); cacheit; cacheit++)
    { if(cacheit->delay[cacheit->stage] > 0)
      { --cacheit->delay[cacheit->stage];
      }
    }
  }

  /**
   * \brief Advance the requests in the cache pipelines
   */
  void RunCachePipeline()
  { 
    for(int cfg=0; cfg<nConfig; cfg++)
    {
      RunCachePipeline(cacheQueue[cfg]);
    }
    if(Snooping)
    { 
      for(int cfg=0; cfg<nConfig; cfg++)
      {
	RunCachePipeline(cacheSnoopQueue[cfg]);
      }
    }
  }

  /**
   * \brief Enqueue a new request to the cacheQueue from CPU side
   */
  void cacheQueue_enqueue(const memreq<INSTRUCTION, nCPUtoCacheDataPathSize> &mr, int cfg)
  { CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize> *entry;
    // check that the cache queue is not full, it should never happen
    if(cacheQueue[cfg].Full())
    { ERROR << "CPU request should not have been accepted, the cache is full !" << endl;
      cerr << *this;
      //      exit(-1);
      abort();
    }

    /* create new entry in the cache pipeline and set stage and delay for each of the stages */
    entry = cacheQueue[cfg].New();
    entry->stage = 0;
    for(int i = 0; i < nStages; i++) entry->delay[i] = nDelay;
    if(entry->delay[0] > 0) entry->delay[0]--;

    /* copy signals to the cache pipeline entry */
    /* the entry is marked as a write if the request is a write cache command */
    entry->write = (mr.command == memreq_types::cmd_WRITE);
    /* set the state of the entry */
    switch(mr.command)
    { case memreq_types::cmd_WRITE:
        /* increment the number of write accesses */
        accesses_write[cfg]++;
        entry->state = WRITE;
        break;
      case memreq_types::cmd_READ:
        /* increment the number of read accesses */
        accesses_read[cfg]++;
        entry->state = READ;
        break;
      case memreq_types::cmd_PREFETCH:
        /* increment the number of prefetch accesses */
        accesses_prefetch[cfg]++;
        entry->state = PREFETCH;
        break;
      case memreq_types::cmd_EVICT:
        /* increment the number of evict accesses */
        accesses_evict[cfg]++;
        entry->state = EVICT;
        break;
      case memreq_types::cmd_READX:
        accesses_read[cfg]++;
        entry->state = READ;
//        ERROR << "Received a READX from the CPU !" << endl; // THIS WAS ONLY TRUE FOR L1 CACHE. L2 CACHES ARE RECEIVING READX FROM L1 CACHES
//        abort();
        break;
      default:
        ERROR << "Undefined mr.command: " << mr.command << endl;
        exit(1);
        break;
    }

    /* copy the size of the request */
    entry->size = mr.size;
    /* if the access is a write copy the data of the request */
    if(entry->write) memcpy(entry->data, mr.Read(), entry->size);
    /* copy the address of the request */
    entry->address = mr.address;
    /* set the base address and the initial index of the data requested by the cpu
     * this data depends on the size of the cache line of the level making the request */
    if(nCPULineSize > 0)
    { entry->base = entry->address & (~(address_t)(nCPULineSize - 1));
      entry->index = entry->address & ((address_t)(nCPULineSize - 1))
                   & ~((address_t)(nCachetoCPUDataPathSize - 1));
    }
    else
    { entry->base = entry->address;
      entry->index = 0;
    }
    entry->instr = mr.instr;         /* copy the instruction of the request */
    entry->port = 0;                 /* remember which was the port which made the request */
    entry->block = false;            /* the entry is not blocked by default */
    entry->uid = mr.uid;             /* copy the uid of the request */
    entry->memreq_id = mr.memreq_id;
    entry->req_sender = mr.req_sender;
    entry->sender_type = memreq_types::sender_CPU;
    entry->cachable = mr.cachable;
  #ifdef NOC_THREADS_DISTRIBUTION
    entry->dst_id = mr.dst_id;
  #endif

  }
  
  /**
   * \brief Enqueue a new request to the cacheQueue from the memory side
   */
//  void cacheQueue_from_memory_enqueue(const memreq<INSTRUCTION, nCPUtoCacheDataPathSize> &mr)
  void cacheQueue_from_memory_enqueue(const memreq<INSTRUCTION, nMemtoCacheDataPathSize> &mr, int cfg)
  { CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize> *entry;
    // check that the cache queue is not full, it should never happen
    if(cacheSnoopQueue[cfg].Full())
    { ERROR << "[CFG::"<<cfg<<"]"<< "MEM request should not have been accepted, the cache is full !" << endl;
      cerr << "memreq: " << mr << endl;
      cerr << *this;
      exit(-1);
    }

    /* create new entry in the cache pipeline and set stage and delay for each of the stages */
    entry = cacheSnoopQueue[cfg].New();
    entry->stage = 0;
    for(int i = 0; i < nStages; i++) entry->delay[i] = nDelay;
    if(entry->delay[0] > 0) entry->delay[0]--;

    /* copy signals to the cache pipeline entry */
    entry->write = false;
    if(mr.command == memreq_types::cmd_WRITE)
    { ERROR << "enqueued a write from the memory side !"<< endl;
      cerr << mr << endl;
      exit(1);
    }
    /* set the state of the entry */
    switch(mr.command)
    { case memreq_types::cmd_READ:
        accesses_read[cfg]++;
        entry->state = READ;
        break;
      case memreq_types::cmd_READX:
        accesses_read[cfg]++;
        entry->state = READX;
        break;
      case memreq_types::cmd_FLUSH:
        // Flush requests do not go into the cache pipeline !
        ERROR << "FLUSH REQUEST SHOULD NOT HAVE GONE THIS FAR !!!" << endl;
        exit(1);
        break;
      default:
        ERROR << "unhandled mr.command: " << mr.command << " on request : " << mr << endl;
        exit(1);
        break;
    }

    /* copy the size of the request */
    entry->size = mr.size;
    // Can't be a write now.
//  /* if the access is a write copy the data of the request */
//  if(entry->write) memcpy(entry->data, mr.Read(), entry->size);
    /* copy the address of the request */
    entry->address = mr.address;
    /* set the base address and the initial index of the data requested by the cpu
     * this data depends on the size of the cache line of the level making the request */
    if(nCPULineSize > 0)
    { entry->base = entry->address & (~(address_t)(nCPULineSize - 1));
      entry->index = entry->address & ((address_t)(nCPULineSize - 1))
                   & ~((address_t)(nCachetoCPUDataPathSize - 1));
    }
    else
    { entry->base = entry->address;
      entry->index = 0;
    }
    entry->instr = mr.instr;         /* copy the instruction of the request */
    entry->port = 0;                 /* remember which was the port which made the request */
    entry->block = false;            /* the entry is not blocked by default */
    entry->uid = mr.uid;             /* copy the uid of the request */
    entry->memreq_id = mr.memreq_id;
    entry->req_sender = mr.req_sender;
    entry->sender_type = memreq_types::sender_MEM;
    entry->cachable = mr.cachable;
  }
  
  /**
   * \brief Read cpu requests to the cache and puts them in the pipeline 
   */
  void ReadCPUData()
  {

#ifdef DD_DEBUG_DCACHE_VERB101
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr << "[Cnonfig::all][DD_DEBUG_DCACHE("<<timestamp()<<")("<< this->name() <<")]  Begin of ReadCPUData..." << endl;
    }
#endif
    for(int cfg=0; cfg<nConfig; cfg++)
    {
      /* if there is a new request from the CPU it is added to the cache pipeline */
      if(inCPU.enable[cfg])
      { memreq<INSTRUCTION, nCPUtoCacheDataPathSize> mr = inCPU.data[cfg];
#ifdef DEBUG_BUS_MQ
	INFO << "Receiving from C:   " << mr << endl;
#endif
	{
	  stringstream sstr;
	  sstr << this->name() << cfg;
	  if(sstr.str()==mr.sender)
	    { ERROR << "Receiving a request from myself on CPU port !\n" << mr << endl;
	    exit(1);
	    }
	}
#ifdef DD_DEBUG_DCACHE_VERB101
	if (DD_DEBUG_TIMESTAMP <= timestamp())
	{
	  cerr << "[Cnonfig::"<<cfg<<"][DD_DEBUG_DCACHE("<< this->name() <<")] Stored   " << mr << " from CPU" << endl;
	}
#endif
#ifdef DEBUG_TEST_UNCACHABLE_WRITE
	if( !mr.cachable && mr.command==memreq_types::cmd_WRITE ) INFO << "<-CPU- \e[1;31muncachable write\e[0m: " << mr << endl;
#endif
	
#ifdef _CACHE_MESSAGES_SVG_H_
	// Received a local request from the CPU, stored it.
	stringstream ss1, ss2;
	ss1 << "P" << nProg;
	ss2 << "$" << nProg;
	if(cache[cfg].hit(mr.address)) SVG.add(timestamp(),ss1.str(),ss2.str(),"H",mr);
	else                      SVG.add(timestamp(),ss1.str(),ss2.str(),"M",mr);
#endif
	if(svg)
	{ if(cache[cfg].hit(mr.address)) svg->add_cpu_to_cac(timestamp(),name(),"H",mr);
        else                      svg->add_cpu_to_cac(timestamp(),name(),"M",mr);
	}
	
      /* increment number of accesses to the cache */
	accesses[cfg]++;
	if (!inCPU.data[cfg].something())
	{
	  cerr << "[" << this->name() << "("<< timestamp() <<")"
	       << "CACHE Error: how nothing may have been enabled ???" << endl;
	  abort();
	}
	// TODO NOW !!!! 
	//	cacheQueue_enqueue(mr);
	/*
#ifdef DD_DEBUG_DCACHE_VERB101
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr << "[Cnonfig::"<<cfg<<"][DD_DEBUG_DCACHE("<<timestamp()<<")("<< this->name() <<")]  Before cache enqueue..." << endl;
      cerr << cacheQueue[cfg] << endl;
    }
#endif
	*/
	cacheQueue_enqueue(mr,cfg);
	/*
#ifdef DD_DEBUG_DCACHE_VERB101
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr << "[Cnonfig::"<<cfg<<"][DD_DEBUG_DCACHE("<<timestamp()<<")("<< this->name() <<")]  After cache enqueue..." << endl;
      cerr << cacheQueue[cfg] << endl;
    }
#endif
	*/
#ifdef NOC_THREADS_DISTRIBUTION
	if( ((mr.address) & 0XFF000000) == 0XFF000000)
	cout <<"CAC : receive thread request form CPU " << mr <<endl;
#endif
	
      }
#ifdef DD_DEBUG_DCACHE_VERB101
      else
      {
	if (DD_DEBUG_TIMESTAMP <= timestamp())
	{
	  cerr << "[Cnonfig::"<<cfg<<"][DD_DEBUG_DCACHE("<< this->name() <<")] No data enabled !!! " << endl;	  
	}
      }
#endif
    } // end of foreach Config.
#ifdef DD_DEBUG_DCACHE_VERB101
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr << "[Cnonfig::all][DD_DEBUG_DCACHE("<<timestamp()<<")("<< this->name() <<")]  End of ReadCPUData..." << endl;
    }
#endif
  } // end of ReadCPUData()
    
  /**
   * \brief Read memory snooped hit requests to the cache and puts them in the pipeline 
   */
  void ReadSnoopedData()
  { 
    for(int cfg=0; cfg<nConfig; cfg++)
    {
      if(!Snooping)
      { ERROR << "This process should not be called if Snooping is desactivated" << endl;
	exit(1);
      }
      // Check that data was received from the memory system
      //      if(!inMEM.enable[cfg]) return;
      // Very Bad : return 
      //      if(inMEM.enable[cfg])
      //      {
      if(!inMEM.enable[cfg]) continue;
      
      // Make sure the incoming data is a read to be handled by the snooper
      memreq<INSTRUCTION, nMemtoCacheDataPathSize> mr = inMEM.data[cfg];
      if(mr.message_type==memreq_types::type_ANSWER)
      { // Write are handled by UpdateWriteBuffer
	//	return;
	// DD...
	// If the answer is for another cache but if we are currently trying to answer to
	// this cache we have to discard this answer.
	QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit = cacheSnoopQueue[cfg].SeekAtHead();
	if (cacheit) 
	  {
	    if (cacheit->req_sender == mr.req_sender)
	      {
		if (cacheit->address == mr.address)
		  {
#ifdef DD_DEBUG_DCACHE_SNOOPING_VERB100
	   if (DD_DEBUG_TIMESTAMP <= timestamp())
	     {
	       cerr << "[Cnonfig::"<<cfg<<"][DD_DEBUG_DCACHE("<< this->name() <<")] SNOOPING Removing request from Snooping queue !!!!" << mr << " from CPU" << endl;
	     }
#endif		    
 		    cacheSnoopQueue[cfg].RemoveHead();
		    
		  }
#ifdef DD_DEBUG_DCACHE_SNOOPING_VERB100
		else
		  {
		    if (DD_DEBUG_TIMESTAMP <= timestamp())
		      {
			cerr << "[Cnonfig::"<<cfg<<"][DD_DEBUG_DCACHE("<< this->name() <<")] SNOOPING Not removing because addresses are different !!!!" << mr << " from CPU" << endl;
		      }
		  }
#endif		   
	      }
#ifdef DD_DEBUG_DCACHE_SNOOPING_VERB100
	    else
	      {
		if (DD_DEBUG_TIMESTAMP <= timestamp())
		  {
		    cerr << "[Cnonfig::"<<cfg<<"][DD_DEBUG_DCACHE("<< this->name() <<")] SNOOPING Not removing because req_sender are different !!!!" << mr << " from CPU" << endl;
		  }
	      }
#endif		    
	  }
	if (requestBuffer[cfg].req_sender == mr.req_sender)
	  if (requestBuffer[cfg].address == mr.address)
	    {
	      requestBuffer[cfg].ready = false;
#ifdef DD_DEBUG_DCACHE_SNOOPING_VERB100
	   if (DD_DEBUG_TIMESTAMP <= timestamp())
	     {
	       cerr << "[Cnonfig::"<<cfg<<"][DD_DEBUG_DCACHE("<< this->name() <<")] SNOOPING Removing request from RequestBuffer !!!!" << endl;
	     }
#endif		    
	    }
	continue;
      }
      
      if(mr.command == memreq_types::cmd_FLUSH)
      { // Flush received from memory are invalidating cache lines
	
	
	stringstream sstr;
	sstr << this->name() << cfg;
        if(sstr.str()==mr.req_sender)
        { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit = cacheQueue[cfg].SeekAtHead();
          if(!cacheit)
          { ERROR << "Received a Flush request from this cache without the request being in the queue !" << endl;
	  exit(1);
          }
	  //INFO << "\e[1;35mGOT !!MY!! FLUSH :\e[0m " << mr << endl;
	  //cerr << "\e[1;35mnon matching cacheit: " << hex << cacheit->address << dec << "\e[0m : " << *cacheit << endl;
          if(cacheit->state != WRITE_HIT_FLUSH_REQUESTED)
          { ERROR << "Received a Flush request from this cache without a WRITE_HIT_FLUSH_REQUESTED request being in the queue !" << endl;
            exit(1);
          }
          // The "Flush on a Modified Write Hit" request being received is issued by this cache.
          // This does mean that other caches have also received such a request and that the write 
          // can be performed, and the WRITE_HIT_FLUSH_REQUESTED can be removed from the queue.
          cache[cfg].setCacheLineIndexed(cacheit->set, cacheit->line, cacheit->data,
                                    cacheit->address & ((address_t)(nLineSize-1)), cacheit->size, true);
          cache[cfg].mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrWr_BusRdX,__FILE__,__LINE__,nProg);
          cacheQueue[cfg].RemoveHead();
	  //          return;
	  continue;
        }
	

#ifdef _CACHE_MESSAGES_SVG_H_
        stringstream ss;
        ss << "$" << nProg;
#endif
	
	//INFO << "\e[1;35mGOT A FLUSH :\e[0m " << mr << endl;

        if(cache[cfg].hit(mr.address))
        { // A cache hit on a flush, the line should be invalidated
          int set = cache[cfg].getSet(mr.address);
          int line = cache[cfg].getLine(mr.address);
          cache[cfg].setValid(set, line, false);
          cache[cfg].mesi_transition(set, line, MESI::transition_BusRdX_Flush,__FILE__,__LINE__,nProg);    
#ifdef _CACHE_MESSAGES_SVG_H_
        SVG.add(timestamp(),"BS",ss.str(),"F",mr);
#endif
          if(svg)
          { svg->add_bus_to_cac(timestamp(),name(),"F",mr);
          }
        }
        else
        { // A cache miss on a flush, nothing to do.
#ifdef _CACHE_MESSAGES_SVG_H_
          SVG.add(timestamp(),"BS",ss.str(),"M",mr);
#endif
          if(svg)
          { svg->add_bus_to_cac(timestamp(),name(),"M",mr);
          }
        }
        //return;
	continue;
      } // FLUSH
      
      stringstream sstr;
      sstr << this->name() << cfg;
      //     if(this==mr.sender)
     if(sstr.str()==mr.sender)
      { ERROR << "Receiving a request from myself in ReadSnoopedData : " << mr << endl;
	terminate_now() ;
      }
      
      // Not a flush, must be a read request answer that may be a local hit or miss
#ifdef _CACHE_MESSAGES_SVG_H_
      // Received an answer for this cache
      stringstream ss;
      ss << "$" << nProg;
      if(cache[cfg].hit(mr.address)) SVG.add(timestamp(),"BS",ss.str(),"H",mr);
      else                      SVG.add(timestamp(),"BS",ss.str(),"M",mr);
#endif
      if(svg)
      { if(cache[cfg].hit(mr.address)) svg->add_bus_to_cac(timestamp(),name(),"H",mr);
      else                      svg->add_bus_to_cac(timestamp(),name(),"M",mr);
      }
      
//yyy
      if(inMEM_was_a_cacheQueue_hit[cfg])
      { //INFO << "\e[1;34menabling: " << mr << "\e[0m" << endl;
      }
      
      // This is a read request, discard it if it makes a miss in the local cache
      // and the request queue
      if(!inMEM_was_a_hit[cfg]) continue; //return;
      
      // The request made a hit in the local cache
      accesses[cfg]++;
      
      // TODO NOW !!!
      cacheQueue_from_memory_enqueue(mr, cfg); // Enqueue the snooped read hit request
      
      //INFO << "Enqueued " << mr << " from MEM" << endl;
    } // end of foreach Config.
  } // end of ReadSnoopedData()
    
    /**
     * \brief Performs the read request to the cache checking if the access was a hit or a miss and setting the entry state correctly
     */
  void SetCacheHeadReadState(QueuePointer<CachePipeStage<INSTRUCTION,nLineSize,nStages,nCPUtoCacheDataPathSize>, nStages>& cacheit, bool is_rdx, int cfg)
  { 
    if(!cacheit->cachable) 
    { // If the read request is non cachable store it as a non cachable request
      if(cache[cfg].hit(cacheit->address))
      {
//        ERROR << "Cache hit on a NOT CACHABLE address on a READ !" << endl;
//        exit(1);
      }
      cacheit->hit = false;
      cacheit->evicted = false;
      cacheit->state = READ_UNCACHABLE;
//#ifdef DEBUG_TEST_UNCACHABLE_READ
//      INFO << "state set to read uncachable" << endl;
//#endif
      return;
    }
  
    // The address is cachable, check for hit or miss
    cacheit->hit = cache[cfg].hit(cacheit->address);
    /* the access was a hit */
    if(cacheit->hit)
    { /* modify the history of the cache (needed by the lru and pseudo lru replacement policies */
      cache[cfg].historyAccess(cache[cfg].getSet(cacheit->address), cache[cfg].getLine(cacheit->address));
      /* increment the number of hits */
      hits[cfg]++;
      hits_read[cfg]++;
      /* set the state of the pipeline entry */
      cacheit->state = READ_HIT_READY;
      
      // Update the MESI state accordingly to the read hit request
      if(Snooping)
      { if(cacheit->sender_type==memreq_types::sender_MEM)
        { ERROR << "Snooped request in regular queue !" << endl;
          exit(1);
        }
        // read hit from the cpu side.
        cache[cfg].mesi_transition(cache[cfg].getSet(cacheit->address),cache[cfg].getLine(cacheit->address),MESI::transition_PrRd_XX,__FILE__,__LINE__,nProg);
      }
      
      /* copy the data from the cache to be sent to the cpu */
      if(nCPULineSize>0)
      { cache[cfg].getCacheLineIndexed(cacheit->data,
                                  cache[cfg].getSet(cacheit->address),
                                  cache[cfg].getLine(cacheit->address),
                                  (cacheit->address & (~(address_t)(nCPULineSize - 1))) & ((address_t)(nLineSize - 1)),
                                  cacheit->size);
      }
      else
      { cache[cfg].getCacheLineIndexed(cacheit->data,
                                  cache[cfg].getSet(cacheit->address),
                                  cache[cfg].getLine(cacheit->address),
                                  (cacheit->address & ((address_t)(nLineSize - 1))),
                                  cacheit->size);
      }
    }
    else
    { /* the access was a miss, increment the number of misses */
      misses[cfg]++;
      misses_read[cfg]++;
      /* get the set and line to place the requested line */
      cacheit->set=cache[cfg].getReplaceSet(cacheit->address);
      cacheit->line=cache[cfg].getReplaceLine(cacheit->address);
      /* modify the history of the cache (needed by the lru and pseudo lru replacement policies */
      cache[cfg].historyAccess(cacheit->set, cacheit->line);
      /* check if the line that the access is removing needs to be evicted,
       * a line is evicted if it was valid and dirty */
      if(cache[cfg].getValid(cacheit->set, cacheit->line))
      { /* the line is valid, check if the line is dirty */
        cacheit->evicted = cache[cfg].getWrite(cacheit->set, cacheit->line);
        /* the line needs to be evicted */
        if(cacheit->evicted)
        {  /* get the address of the line to be evicted */
          cacheit->evictedaddress = cache[cfg].getAddress(cacheit->set, cacheit->line);
          /* increment the number of writebacks made by the cache */
          writebacks[cfg]++;
          writebacks_read[cfg]++;
          /* get the data needed to be sent to the memory system  */
          cache[cfg].getCacheLine(cacheit->linedata, cacheit->set, cacheit->line);
          /* set the entry state */
          cacheit->state = READ_MISS_EVICTION;
        }
        else
        { /* no line needs to be evicted, set the entry state */
          cacheit->state = READ_MISS;
        }
      }
      else
      { /* no line needs to be evicted, set the entry state */
        cacheit->evicted = false;
        cacheit->state = READ_MISS;
      }
    }
  }// Endof SetCacheHeadReadState 

  /**
   * \brief Performs the write request to the cache checking if the access was a hit or a miss and setting the entry state correctly 
   */
  void SetCacheHeadWriteState(QueuePointer<CachePipeStage<INSTRUCTION,nLineSize,nStages,nCPUtoCacheDataPathSize>, nStages>& cacheit, int cfg)
  { if(!cacheit->cachable) 
    { // If the read request is non cachable store it as a non cachable request
      if(cache[cfg].hit(cacheit->address))
      { ERROR << "Cache hit on a NOT CACHABLE address on a Write !" << endl;
        exit(1);
      }
      cacheit->hit = false;
      cacheit->evicted = false;
      cacheit->state = WRITE_UNCACHABLE;
      
#ifdef DEBUG_TEST_UNCACHABLE_WRITE
      INFO << "state set to write uncachable: " << *cacheit << endl;
#endif
      return;
    }
  
    /*
    cerr << "[DD_DEBUG_DCACHE] Into SetCacheHeadWriteState..." << endl;
#ifdef DD_DEBUG_DCACHE
    cerr << "[DD_DEBUG_DCACHE] Into SetCacheHeadWriteState..."
    << cacheit->data << endl; 
	    //	    cerr << *this << endl;
#endif
    */
    // The address is cachable, check for hit or miss
    cacheit->hit = cache[cfg].hit(cacheit->address);
    /* the access was a hit */
    if(cacheit->hit)
    { /* modify the history of the cache (needed by the lru and pseudo lru replacement policies */
      cache[cfg].historyAccess(cache[cfg].getSet(cacheit->address), cache[cfg].getLine(cacheit->address));
      /* increment the number of hits */
      hits[cfg]++;
      hits_write[cfg]++;
      cacheit->set = cache[cfg].getSet(cacheit->address);
      cacheit->line = cache[cfg].getLine(cacheit->address);

      if(Snooping)
      { if(cacheit->sender_type==memreq_types::sender_MEM)
        { ERROR << "Snooped request in regular queue !" << endl;
          exit(1);
        }

        switch(cache[cfg].getMESI(cacheit->set,cacheit->line).getState())
        { case MESI::state_MODIFIED:
          case MESI::state_EXCLUSIVE:
            // A local write hit on M & E state does not imply any bus transaction, so the 
            // write hit request can be removed from the queue, once performed
            cache[cfg].setCacheLineIndexed(cacheit->set, cacheit->line, cacheit->data,
                                      cacheit->address & ((address_t)(nLineSize-1)), cacheit->size, true);
            cache[cfg].mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrWr_XX,__FILE__,__LINE__,nProg);
            cacheQueue[cfg].RemoveHead();
#ifdef DD_DEBUG_DCACHE
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
	    cerr << "[DD_DEBUG_DCACHE] SetCacheHeadWriteState: writting in M.or E. state... cacheit:"
		 << cacheit->data << endl; 
	    //	    cerr << *this << endl;
    }
#endif
            break;
          case MESI::state_SHARED:
            // A local write hit on SHARED state should invalidate other caches lines, so the
            // request should become a write back.
/* NOW THE WRITE IN THE CACHE IS DONE WHEN RECEIVING ITS OWN FLUSH
            cache.setCacheLineIndexed(cacheit->set, cacheit->line, cacheit->data,
                                      cacheit->address & ((address_t)(nLineSize-1)), cacheit->size, true);
            cache.mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrWr_BusRdX,__FILE__,__LINE__,nProg);
*/
            cacheit->state = WRITE_HIT_FLUSH;
#ifdef DD_DEBUG_DCACHE
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
	    cerr << "[DD_DEBUG_DCACHE] SetCacheHeadWriteState: not writting in SHARED state... cacheit:"
		 << cacheit->data << endl; 
	    //	    cerr << *this << endl;
    }
#endif
            break;
        }
 

 
      } 
      else // !Snooping
      { 
#ifdef DD_DEBUG_DCACHE
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
	cerr << "[DD_DEBUG_DCACHE] SetCacheHeadWriteState: writting... cacheit:"
	     << *cacheit << endl; 
	//	    cerr << *this << endl;
    }
#endif
	    
	// Modify the contents of the cache with the write request data, set the cache line as dirty
        cache[cfg].setCacheLineIndexed(cache[cfg].getSet(cacheit->address), cache[cfg].getLine(cacheit->address), cacheit->data,
                                  cacheit->address & ((address_t)(nLineSize-1)), cacheit->size, true);
        // Once performed, write hit can be removed from the queue.
        cacheQueue[cfg].RemoveHead();
      }
    }
    else
    { /* the access was a miss, increment the number of misses */
      misses[cfg]++;
      misses_write[cfg]++;
      /* get the set and line to place the requested line */
      cacheit->set = cache[cfg].getReplaceSet(cacheit->address);
      cacheit->line = cache[cfg].getReplaceLine(cacheit->address);
      /* modify the history of the cache (needed by the lru and pseudo lru replacement policies */
      cache[cfg].historyAccess(cacheit->set, cacheit->line);
      /* check if the line that the access is removing needs to be evicted,
       * a line is evicted if it was valid and dirty */
      if(cache[cfg].getValid(cacheit->set, cacheit->line))
      { /* the line is valid, check if the line is dirty */
        cacheit->evicted = cache[cfg].getWrite(cacheit->set, cacheit->line);
        /* the line needs to be evicted */
        if(cacheit->evicted)
        { /* get the address of the line to be evicted */
          cacheit->evictedaddress = cache[cfg].getAddress(cacheit->set, cacheit->line);
          /* increment the number of writebacks made by the cache */
          writebacks[cfg]++;
          writebacks_write[cfg]++;
          /* get the data needed to be sent to the memory system */
          cache[cfg].getCacheLine(cacheit->linedata, cacheit->set, cacheit->line);
          /* set the entry state */
          cacheit->state = WRITE_MISS_EVICTION;
        }
        else
        { /* no line needs to be evicted, set the entry state */
          cacheit->state = WRITE_MISS;
        }
      }
      else
      { /* no line needs to be evicted, set the entry state */
        cacheit->evicted = false;
        cacheit->state = WRITE_MISS;
      }
    }
  }

  /** 
   * \brief Performs the prefetch request to the cache checking if the access was a hit or a miss and setting the entry state correctly
   */
  void SetCacheHeadPrefetchState(QueuePointer<CachePipeStage<INSTRUCTION,nLineSize,nStages,nCPUtoCacheDataPathSize>, nStages>& cacheit, int cfg)
  { /* check if the request hits the cache or misses */
    cacheit->hit = cache[cfg].hit(cacheit->address);
    /* the access was a hit */
    if(cacheit->hit)
    { /* modify the history of the cache (needed by the lru and pseudo lru replacement policies */
      cache[cfg].historyAccess(cache[cfg].getSet(cacheit->address), cache[cfg].getLine(cacheit->address));
      /* increment the number of hits */
      hits[cfg]++;
      hits_prefetch[cfg]++;
      /* the pipeline head can be removed */
      cacheQueue[cfg].RemoveHead();
    }
    else
    { /* the access was a miss, increment the number of misses */
      misses[cfg]++;
      misses_prefetch[cfg]++;
      /* get the set and line to place the requested line */
      cacheit->set = cache[cfg].getReplaceSet(cacheit->address);
      cacheit->line = cache[cfg].getReplaceLine(cacheit->address);
      /* modify the history of the cache (needed by the lru and pseudo lru replacement policies) */
      cache[cfg].historyAccess(cacheit->set, cacheit->line);
      /* check if the line that the access is removing needs to be evicted,
       * a line is evicted if it was valid and dirty */
      if(cache[cfg].getValid(cacheit->set, cacheit->line))
      { /* the line is valid, check if the line is dirty */
        cacheit->evicted = cache[cfg].getWrite(cacheit->set, cacheit->line);
        /* the line needs to be evicted */
        if(cacheit->evicted)
        { /* get the address of the line to be evicted */
          cacheit->evictedaddress = cache[cfg].getAddress(cacheit->set, cacheit->line);
          /* increment the number of writebacks made by the cache */
          writebacks[cfg]++;
          writebacks_prefetch[cfg]++;
          /* get the data needed to be sent to the memory system */
          cache[cfg].getCacheLine(cacheit->linedata, cacheit->set, cacheit->line);
          /*set the entry state */
          cacheit->state = PREFETCH_MISS_EVICTION;
        }
        else
        { /* no line needs to be evicted, set the entry state */
          cacheit->state = PREFETCH_MISS;
        }
      }
      else
      { /* no line needs to be evicted, set the entry state */
        cacheit->evicted = false;
        cacheit->state = PREFETCH_MISS;
      }
    }
  }

  /**
   * \brief Performs the evict request to the cache checking if the access was a hit or a miss and setting the entry state correctly 
   */
  void SetCacheHeadEvictState(QueuePointer<CachePipeStage<INSTRUCTION,nLineSize,nStages,nCPUtoCacheDataPathSize>, nStages>& cacheit, int cfg)
  { /* check if the request hits the cache or misses */
    cacheit->hit = cache[cfg].hit(cacheit->address);
    /* the access was a hit */
    if(cacheit->hit)
    { /* increment the number of hits */
      hits[cfg]++;
      hits_evict[cfg]++;
      /* no history access, we want to reuse the entry of the evicted line */
      /* get the set and line of the cache line to be evicted */
      cacheit->set = cache[cfg].getSet(cacheit->address);
      cacheit->line = cache[cfg].getLine(cacheit->address);
#ifdef DEBUG_CACHEWB
      if(!cache[cfg].getValid(cacheit->set, cacheit->line))
      { /* why did we get a hit if the line is not valid???? */
        cerr << "Error(" << name() << "): trying to evict a line that is not valid" << endl;
        cerr << *this;
        exit(-1);
      }
#endif
      /* the line needs to be evicted and not just removed if the line was valid and dirty */
      /* get the address of the line to be evicted */
      cacheit->evictedaddress = cache[cfg].getAddress(cacheit->set, cacheit->line);
      /* check if the line needs to be evicted */
      cacheit->evicted = cache[cfg].getWrite(cacheit->set, cacheit->line);
      /* set the removed line as not valid and not dirty */
      cache[cfg].setValid(cacheit->set, cacheit->line, false);
      cache[cfg].setWrite(cacheit->set, cacheit->line, false);
      /* if the line needs to be evicted */
      if(cacheit->evicted)
      { /* increment the number of writebacks */
        writebacks[cfg]++;
        writebacks_evict[cfg]++;
        /* get the data needed to be sent to the memory system */
        cache[cfg].getCacheLine(cacheit->linedata, cacheit->set, cacheit->line);
        /* set the entry state */
        cacheit->state = EVICT_READY;
      }
      else
      { /* no line needs to be evicted, the pipeline head can be removed */
        cacheQueue[cfg].RemoveHead();
      }
    }
    else
    { /* do nothing, eviction of a line that does not exist in cache */
      /* increment the number of misses */
      misses[cfg]++;
      misses_evict[cfg]++;
      /* remove the pipeline head */
      cacheQueue[cfg].RemoveHead();
    }
  }

  /**
   * \brief Sets the cache pipeline head state if it is on the last stage of the pipeline
   */
  void SetCacheHeadState()
  { 
    for(int cfg=0; cfg<nConfig; cfg++)
    {
    QueuePointer<CachePipeStage<INSTRUCTION,nLineSize,nStages,nCPUtoCacheDataPathSize>, nStages> cacheit;
    cacheit = cacheQueue[cfg].SeekAtHead();    // Get the head of the pipeline
    if(cacheit)
    { /* check that the head of the pipeline is ready to be serviced (last stage, delay = 0) */
      if(cacheit->stage == nStages - 1 && cacheit->delay[nStages - 1] == 0)
      { /* set the state of the entry depending on its current state */
        switch(cacheit->state)
        { case READ:
            SetCacheHeadReadState(cacheit,false, cfg);
            break;
          case WRITE:
            SetCacheHeadWriteState(cacheit, cfg);
            break;
          case PREFETCH:
            SetCacheHeadPrefetchState(cacheit, cfg);
            break;
          case EVICT:
            SetCacheHeadEvictState(cacheit, cfg);
            break;
          case READX:
            SetCacheHeadReadState(cacheit,true, cfg);
            break;
        }
      }
    }
    }// end of foreach Config.
  } // end of SetCacheHeadState()
  
  /**
   * \brief Sets the cache pipeline head state if it is on the last stage of the snoop pipeline
   */
  void SetSnoopedCacheHeadState()
  { if(!Snooping)
    { ERROR << "Should never append. only called from end_of_cycle if snooping is enabled !" << endl;
      exit(1);
    }

    for(int cfg=0; cfg<nConfig; cfg++)
    {
    QueuePointer<CachePipeStage<INSTRUCTION,nLineSize,nStages,nCPUtoCacheDataPathSize>, nStages> cacheit;
    cacheit = cacheSnoopQueue[cfg].SeekAtHead(); // Get the head of the pipeline
    if(cacheit)
    { /* check that the head of the pipeline is ready to be serviced (last stage, delay = 0) */
      if(cacheit->stage == nStages - 1 && cacheit->delay[nStages - 1] == 0)
      { /* set the state of the entry depending on its current state */
//        INFO << "\e[1;34mGot something in my snoop queue: \e[0m" << *cacheit << endl;
        switch(cacheit->state)
        { case READ:
            SetCacheSnoopHeadReadState(cacheit,false, cfg);
            break;
          case READX:
            SetCacheSnoopHeadReadState(cacheit,true, cfg);
            break;
          case WRITE:
            ERROR << "Should not receive WRITE request from the memory side" << endl;
            exit(1);
//            SetCacheSnoopHeadWriteState(cacheit);
            break;
          case PREFETCH:
            ERROR << "Should not receive PREFETCH request from the memory side" << endl;
            exit(1);
            break;
          case EVICT:
            ERROR << "Should not receive EVICT request from the memory side" << endl;
            exit(1);
            break;
        }
      }
    }
    }// end of foreach Config.
  }

  /**
   * \brief Performs the read request to the cache checking if the access was a hit or a miss and setting the entry state correctly
   */
  void SetCacheSnoopHeadReadState(QueuePointer<CachePipeStage<INSTRUCTION,nLineSize,nStages,nCPUtoCacheDataPathSize>, nStages>& cacheit, bool is_rdx, int cfg)
  { if(!Snooping)
    { ERROR << "Should never append. only called from end_of_cycle if snooping is enabled !" << endl;
      exit(1);
    }
    cacheit->hit = cache[cfg].hit(cacheit->address);

    if(!cacheit->hit)
    { // We are snooping Read request which are either performing a hit in the cache, or in the pending request queue.
      // In case it is a hit on pending request, wait until the request is completed, and the data is available in 
      // the cache.
      return;
    }
    
    if(!cacheit->cachable) 
    { ERROR << "An uncachable read request has been snooped, meaning it is a cache hit in the local cache." << endl;
      exit(1);
    }
    
    if(cacheit->sender_type!=memreq_types::sender_MEM)
    { ERROR << "It seems we snooped a read request not coming from the bus !" << endl;
      exit(1);
    }
  
    // We snooped a request that has made a local read hit.
    
    /* modify the history of the cache (needed by the lru and pseudo lru replacement policies */
    cache[cfg].historyAccess(cache[cfg].getSet(cacheit->address), cache[cfg].getLine(cacheit->address));
    /* increment the number of hits */
    hits[cfg]++;
    hits_read[cfg]++;
    /* set the state of the pipeline entry */
    cacheit->state = READ_HIT_READY;
      
    // read hit from the snooping side, data is wrote back to memory. This write back will also be used as the 
    // other cache answer
    cacheit->set = cache[cfg].getSet(cacheit->address);
    cacheit->line = cache[cfg].getLine(cacheit->address);
    
    if(is_rdx)
    { // If the read request is a READX, then the local copy should be invalidated
      cache[cfg].mesi_transition(cacheit->set,cacheit->line,MESI::transition_BusRdX_Flush,__FILE__,__LINE__,nProg);
      cache[cfg].setWrite(cacheit->set, cacheit->line, false);
      cache[cfg].setValid(cacheit->set, cacheit->line, false);
    }
    else
    { // If it is a simple read request: no invalidation but the data is no more modified (if it was)
      cache[cfg].mesi_transition(cacheit->set,cacheit->line,MESI::transition_BusRd_Flush,__FILE__,__LINE__,nProg);
      cache[cfg].setWrite(cacheit->set, cacheit->line, false);
    }
      
    if(nLineSize!=cacheit->size)
    { ERROR << "snooped read request size is not nLineSize." << endl;
      exit(1);
    }
    if((cacheit->address & ((address_t)(nLineSize - 1)))!=0)
    { ERROR << "snooped read request address is not cache aligned." << endl;
      exit(1);
    }
    //Maybe i should have kept the following, but snooped read request should be cache whole line requests
    //cache.getCacheLineIndexed(cacheit->data, cacheit->set, cacheit->line, 0
    //                          (cacheit->address & ((address_t)(nLineSize - 1))),
    //                          cacheit->size);
    // Kept just in case of one of the previous if to be false.

    //Copy the local cache line to the cacheit data, for it to we write back.
    cache[cfg].getCacheLine(cacheit->data, cacheit->set, cacheit->line);
  }





  /*
   * \brief Performs the write request to the cache checking if the access was a hit or a miss and setting the entry state correctly 
  void SetCacheSnoopHeadWriteState(QueuePointer<CachePipeStage<INSTRUCTION,nLineSize,nStages,nCPUtoCacheDataPathSize>, nStages>& cacheit)
  { if(!Snooping)
    { ERROR << "Should never append. only called from end_of_cycle if snooping is enabled !" << endl;
      exit(1);
    }

    cacheit->hit = cache.hit(cacheit->address);

    if(!cacheit->cachable) 
    { ERROR << "An uncachable write request has been snooped, meaning it is a cache hit in the local cache." << endl;
      exit(1);
    }

    if(!cacheit->hit)
    { ERROR << "A snooped write request has been enqueued whereas it is a lacal miss." << endl;
      exit(1);
    }

    if(cacheit->sender_type!=memreq_types::sender_MEM)
    { ERROR << "It seems we snooped a write request not coming from the bus !" << endl;
      exit(1);
    }

    // We snooped a WRITE doing a local hit

    // modify the history of the cache (needed by the lru and pseudo lru replacement policies 
    cache.historyAccess(cache.getSet(cacheit->address), cache.getLine(cacheit->address));
    // increment the number of hits 
    hits++;
    hits_write++;
    // modify the contents of the cache with the write request data, set the cache line as dirty 
    cache.setCacheLineIndexed(cache.getSet(cacheit->address), cache.getLine(cacheit->address), cacheit->data,
                                cacheit->address & ((address_t)(nLineSize-1)), cacheit->size, true);

    int set = cache.getSet(cacheit->address);
    int line = cache.getLine(cacheit->address);

    // The distant write hit should perform a write back.
    cache.mesi_transition(set,line,MESI::transition_PrWr_BusRdX,__FILE__,__LINE__,nProg);
    cacheit->state = WRITE_HIT_FLUSH;
  }
   */





  /** 
   * \brief Sets the requestBuffer if data can be sent or requested to the memory
   */
  void SendSnoopDatatoMem(int cfg)
  { if(!Snooping)
    { ERROR << "Should never append. only called from start_of_cycle if snooping is enabled !" << endl;
      exit(1);
    }
    QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;
    /* get the head of the pipeline and checks its state to see if it has data to be sent to the memory system */
    cacheit = cacheSnoopQueue[cfg].SeekAtHead();
    if(cacheit)
    { if(cacheit->sender_type==memreq_types::sender_CPU)
      { ERROR << "A request from the CPU is in the snoop queue !" << endl;
        exit(1);
      }

      switch(cacheit->state)
      { case READ_HIT_READY:
          // A snooped hit read is ready to be sent back to the MEM
          requestBuffer[cfg].command = memreq_types::cmd_WRITE; // This is a write back to memory
          requestBuffer[cfg].write = true; // Perform a write-back to memory, this WB will be snooped.
          requestBuffer[cfg].address = cacheit->address;
          requestBuffer[cfg].base    = cacheit->base;
          requestBuffer[cfg].index   = 0;
          requestBuffer[cfg].size    = cacheit->size;
          memcpy(requestBuffer[cfg].data, cacheit->data, requestBuffer[cfg].size);
  
          requestBuffer[cfg].instr = cacheit->instr;             // Instruction that generated the request
          requestBuffer[cfg].size = nLineSize;                   // A full line is being sent
          requestBuffer[cfg].req_sender = cacheit->req_sender;
          requestBuffer[cfg].uid = cacheit->uid;                 // uid of the instruction
          requestBuffer[cfg].memreq_id = cacheit->memreq_id;
          requestBuffer[cfg].message_type = memreq_types::type_ANSWER;
          requestBuffer[cfg].cacheQueueIndex = cacheSnoopQueueIndex;
          requestBuffer[cfg].ready = true;                       // The request is ready to be sent

//INFO << "SNOOPED READ HIT APPEND: 0x" << hex << cacheit->address << dec << ": ";
//print_buffer(cerr, cacheit->data, requestBuffer[cfg].size);

          return;
          break;

        case READ:
        case READX:
        { // The snooped request was a hit in the request queue, but is not a hit in the cache yet.
          // The request is currently delayed until it is a hit.
	  //INFO << "delay..." << endl;
//xxx
        } break;
        default:
          ERROR << "Unexepected state for the head of cacheSnoopQueue: " << *cacheit << endl;
          exit(1);
      }
    }
  }






  /**
   * \brief Checks if a request sent to the memory system has been accepted and modify the cache pipeline state if needed 
   *
   * Used at End of cycle
   */
  void CheckSnoopMemAcceptedDatafromCache()
  { if(!Snooping)
    { ERROR << "Should never append. only called from end_of_cycle if snooping is enabled !" << endl;
      exit(1);
    }
    for(int cfg=0; cfg<nConfig; cfg++)
    {
    QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;

    /* are we waiting for an accept from the memory? */
    if(requestBuffer[cfg].ready)
    { /* check if the memory accepted the request */
      if(requestBuffer[cfg].cacheQueueIndex!=cacheSnoopQueueIndex) continue;//return;
      if(outMEM.accept[cfg])
      { if(requestBuffer[cfg].write)
        { /* this request is a write */
          /* if last request was finished from the cache pipeline (a write request)
           * remove the request from the cache pipeline */
          requestBuffer[cfg].size -= nCachetoMemDataPathSize;
          if(requestBuffer[cfg].size > 0)
          { /* the request has not bben finished, the request has to be keeped,
             * the address has to be increased,
             * and the cache pipeline is not notified */
            requestBuffer[cfg].index += nCachetoMemDataPathSize;
            requestBuffer[cfg].address = requestBuffer[cfg].base + requestBuffer[cfg].index;
          }
          else
          { /* the request has been finished, the cache pipeline has to be updated */
            cacheit = cacheSnoopQueue[cfg].SeekAtHead();

	    //INFO << "cQI=" << requestBuffer.cacheQueueIndex << " state= " << cacheit->state << endl;
	    // DD Check cacheit because cache entry may have been removed if another cache answer before
	    if (cacheit)
	      {
		switch(cacheit->state)
		  { case READ_HIT_READY:
		      // A snooped read hit has been sent and accepted, i can remove it from the queue
		      cacheSnoopQueue[cfg].RemoveHead();
		      break;
		  default:
		    ERROR << "Don't know what to do when a writing request of state " << cacheit->state << " has been sent then accepted." << endl;
		    exit(1);
		  }
	      }
	    /* reset the request buffer */
            requestBuffer[cfg].ready=false;
          }
        }
        else // not a write
        { /* the access is a cache miss
           * the cache entry (the head of the cache queue) 
           * is not removed from the cache pipeline (this is a blocking cache) */
          /* the request buffer is marked as not ready, to be used with the next memory request */
          cacheit = cacheSnoopQueue[cfg].SeekAtHead();
          switch(cacheit->state)
          { 
            default:
              ERROR << "Don't know what to do when a non-writing request of state " << cacheit->state << " has been sent then accepted." << endl;
              exit(1);
          }
          /* reset the request buffer */
          requestBuffer[cfg].ready=false;
        }
      }
    }
    else
    {
#ifdef DEBUG_CACHEWB
      /* if an accept from the memory system that was received that was not expected, error */
      if(outMEM.accept[cfg])
      { cerr << "Error(" << name() << "): an accept was received from the memory system that was not expected" << endl;
        cerr << *this;
        exit(-1);
      }
#endif
    }
    }//Endof foreach Config.
  }


  /**
   * \brief Fill the returnBuffer from the cacheQueue if its head is non-empty and ready.
   */
  void SendCacheDatatoCPU(int cfg)
  { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;
    // get the head of the pipeline and check its state to see if it has data to be sent to the cpu
    cacheit = cacheQueue[cfg].SeekAtHead();
    if(cacheit)
    { switch(cacheit->state)
      { case READ_HIT_READY:
        case READ_MISS_READY:
        case READ_MISS_LINE_READY:
        case READ_MISS_EVICTION_WRITE_DONE_READY:
        case READ_MISS_EVICTION_WRITE_DONE_LINE_READY:
        case READ_UNCACHABLE_READY:
          // A read is ready to be sent back to the CPU, and such a read has to be sent to the CPU
          if(cacheit->sender_type != memreq_types::sender_CPU)
          { ERROR << "Request in cacheQueue of state " << cacheit->state << " not coming from CPU !" << endl;
            exit(1);
          }
          returnBuffer[cfg].port = cacheit->port;
          returnBuffer[cfg].instr = cacheit->instr;
          returnBuffer[cfg].base = cacheit->base;
          returnBuffer[cfg].index = cacheit->index;
          returnBuffer[cfg].address = cacheit->address;
          returnBuffer[cfg].size = cacheit->size;
          // Copy the data to be returned
          memcpy(returnBuffer[cfg].data, cacheit->data, returnBuffer[cfg].size);
          returnBuffer[cfg].id = ID_CACHE; // Data sent from cache pipeline
          returnBuffer[cfg].uid = cacheit->uid;
          returnBuffer[cfg].memreq_id = cacheit->memreq_id;
          returnBuffer[cfg].req_sender = cacheit->req_sender;
          returnBuffer[cfg].cachable = cacheit->cachable;
          // Set returnBuffer as ready (full)
          returnBuffer[cfg].ready = true;
#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
          cerr <<"["<<this->name()<<"("<<timestamp()<<")]: Writting ReturnBuffer: " << returnBuffer[cfg] << endl;
    }
#endif
          break;
      }
    }
#ifdef DD_DEBUG_DCACHE_VERB2
    else
    {
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {

      cerr <<"["<<this->name()<<"("<<timestamp()<<")]: No cache it!!! Not Writting ReturnBuffer: " << returnBuffer[cfg] << endl;
    }
    }
#endif
  }

  /** 
   * \brief Sets the requestBuffer if data can be sent or requested to the memory
   */
  void SendCacheDatatoMem(int cfg)
  { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;
    /* get the head of the pipeline and checks its state to see if it has data to be sent to the memory system */
    cacheit = cacheQueue[cfg].SeekAtHead();
    if(cacheit)
    { switch(cacheit->state)
      { /***************/
        /* write cases */
        /***************/
        case WRITE_MISS: // A line is being requested to the memory system
          requestBuffer[cfg].write = false; // Write miss first performs read
          requestBuffer[cfg].command = memreq_types::cmd_READX; // Write miss first performs a read from memory
          requestBuffer[cfg].address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
          requestBuffer[cfg].base = requestBuffer[cfg].address & (~(address_t)(nLineSize - 1));
          requestBuffer[cfg].size = nLineSize; // A full line is being sent
          requestBuffer[cfg].index = 0;
          break;
        case WRITE_MISS_EVICTION: // A line is being written to the memory system
          requestBuffer[cfg].command = memreq_types::cmd_WRITE; // A line has to be written back to the memory
          requestBuffer[cfg].write = true;
          requestBuffer[cfg].address = cacheit->evictedaddress & (~(address_t)(nLineSize - 1));
          requestBuffer[cfg].base = requestBuffer[cfg].address & (~(address_t)(nLineSize - 1));
          requestBuffer[cfg].index = 0;
          requestBuffer[cfg].size = nLineSize; // A full line is being sent
          memcpy(requestBuffer[cfg].data, cacheit->linedata, requestBuffer[cfg].size); // Copy the data to send
          break;
        case WRITE_MISS_EVICTION_WRITE_DONE: // A line is being requested to the memory system
          requestBuffer[cfg].command = memreq_types::cmd_READX; // Write miss first performs read
          requestBuffer[cfg].write = false;
          requestBuffer[cfg].address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
          requestBuffer[cfg].base = requestBuffer[cfg].address & (~(address_t)(nLineSize - 1));
          requestBuffer[cfg].size = nLineSize; // A full line is being sent
          requestBuffer[cfg].index = 0;
          break;
        case WRITE_UNCACHABLE:
        #ifdef NOC_THREADS_DISTRIBUTION
          requestBuffer[cfg].command = memreq_types::cmd_WRITE; // A line has to be written back to the memory
          requestBuffer[cfg].write = true;
          requestBuffer[cfg].address = cacheit->address;
          requestBuffer[cfg].base = requestBuffer[cfg].address;
          requestBuffer[cfg].index = 0;
          requestBuffer[cfg].size =  cacheit->size;
          memcpy(requestBuffer[cfg].data, cacheit->data, requestBuffer[cfg].size); // Copy the data to send
	  //cout<<"CAC : to request buffer uncachable write "<< requestBuffer <<endl <<endl;
        #else	  
          requestBuffer[cfg].write = false; // Write uncachable first get the line from dram
          requestBuffer[cfg].command = memreq_types::cmd_READX; // first performs a read from memory
          requestBuffer[cfg].address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
          requestBuffer[cfg].base = requestBuffer[cfg].address & (~(address_t)(nLineSize - 1));
          requestBuffer[cfg].size = nLineSize; // A full line is being sent
          requestBuffer[cfg].index = 0;
        #endif	 
          break;
        case WRITE_UNCACHABLE_READY:
          requestBuffer[cfg].command = memreq_types::cmd_WRITE;
          requestBuffer[cfg].write = true;
          requestBuffer[cfg].address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
          requestBuffer[cfg].base = requestBuffer[cfg].address & (~(address_t)(nLineSize - 1));
          requestBuffer[cfg].index = 0;
          requestBuffer[cfg].size = nLineSize; // A full line is being sent
          memcpy(requestBuffer[cfg].data, cacheit->linedata, requestBuffer[cfg].size); // Copy the data to send
//#ifdef DEBUG_TEST_UNCACHABLE_WRITE
//          INFO << "requestBuffer " << requestBuffer << endl;
//          INFO << "cacheit       " << *cacheit << endl;
//#endif
          break;
        /**************/
        /* read cases */
        /**************/
        case READ_MISS: // A line is being requested to the memory system
          requestBuffer[cfg].command = memreq_types::cmd_READ;
          requestBuffer[cfg].write = false;
          requestBuffer[cfg].address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
          requestBuffer[cfg].size = nLineSize; // A full line is being sent
          break;
        case READ_UNCACHABLE:
#ifdef DEBUG_TEST_UNCACHABLE_READ
          INFO << "read uncachable request ready" << endl;
#endif
          requestBuffer[cfg].command = memreq_types::cmd_READ;
          requestBuffer[cfg].write = false;
          requestBuffer[cfg].address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
        #ifdef NOC_THREADS_DISTRIBUTION
          requestBuffer[cfg].size =  cacheit->size;
	  requestBuffer[cfg].index = cacheit->address & ((address_t)(nCachetoMemDataPathSize - 1));
	#else
          requestBuffer[cfg].size = nLineSize; // A full line is being sent
        #endif
          break;
        case READ_MISS_EVICTION: // A line is being written to the memory system
          requestBuffer[cfg].command = memreq_types::cmd_WRITE; // First perform the eviction as a write to the memory
          requestBuffer[cfg].write = true;
          requestBuffer[cfg].address = cacheit->evictedaddress & (~(address_t)(nLineSize - 1));
          requestBuffer[cfg].base = requestBuffer[cfg].address & (~(address_t)(nLineSize - 1));
          requestBuffer[cfg].index = 0;
          requestBuffer[cfg].size = nLineSize; // A full line is being sent
          memcpy(requestBuffer[cfg].data, cacheit->linedata, requestBuffer[cfg].size); // Copy the data to send
          break;
        case READ_MISS_EVICTION_WRITE_DONE:  // A line is being requested to the memory system
          requestBuffer[cfg].command = memreq_types::cmd_READ;
          requestBuffer[cfg].write = false;
          requestBuffer[cfg].address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
          requestBuffer[cfg].size = nLineSize; // A full line is being sent
          break;
        /******************/
        /* prefetch cases */
        /******************/
        case PREFETCH_MISS:
        case PREFETCH_MISS_EVICTION_WRITE_DONE:  // A line is being requested to the memory system
          requestBuffer[cfg].command = memreq_types::cmd_READ; // Prefetch performs read
          requestBuffer[cfg].write = false;
          requestBuffer[cfg].address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
          requestBuffer[cfg].size = nLineSize; // A full line is being sent
          break;
        case PREFETCH_MISS_EVICTION: // A line is being written to the memory system
          requestBuffer[cfg].command = memreq_types::cmd_WRITE; // First perform the eviction as a write
          requestBuffer[cfg].write = true;
          requestBuffer[cfg].address = cacheit->evictedaddress & (~(address_t)(nLineSize - 1));
          requestBuffer[cfg].base = requestBuffer[cfg].address & (~(address_t)(nLineSize - 1));
          requestBuffer[cfg].index = 0;
          requestBuffer[cfg].size = nLineSize; // A full line is being sent
          memcpy(requestBuffer[cfg].data, cacheit->linedata, requestBuffer[cfg].size); // Copy the data to send
          break;
        /***************/
        /* evict cases */
        /***************/
        case EVICT_READY: // A line is being written to the memory system
          requestBuffer[cfg].command = memreq_types::cmd_WRITE; // Evictions are forced write
          requestBuffer[cfg].write = true;
          requestBuffer[cfg].address = cacheit->evictedaddress & (~(address_t)(nLineSize - 1));
          requestBuffer[cfg].base = requestBuffer[cfg].address & (~(address_t)(nLineSize - 1));
          requestBuffer[cfg].index = 0;
          requestBuffer[cfg].size = nLineSize; // A full line is being sent
          memcpy(requestBuffer[cfg].data, cacheit->linedata, requestBuffer[cfg].size); // Copy the data to send
          break;

        /*****************/
        /* snooped cases */
        /*****************/
        case WRITE_HIT_FLUSH:
          // Local write hit on shared data => Flush
          requestBuffer[cfg].command = memreq_types::cmd_FLUSH;
          requestBuffer[cfg].write = false; // Flush does not perform a memory write but some cache invalidations
          requestBuffer[cfg].address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
          requestBuffer[cfg].base = requestBuffer[cfg].address & (~(address_t)(nLineSize - 1));
          requestBuffer[cfg].index = 0;
          requestBuffer[cfg].size = nLineSize; // A full line is being sent
          break;

        default:
          // Nothing is ready to be put in request Buffer.
          return;
      }

      // Fill the remaining fields of requestBuffer
      requestBuffer[cfg].instr = cacheit->instr;             // Instruction that generated the request
    #ifdef NOC_THREADS_DISTRIBUTION
      requestBuffer[cfg].dst_id = cacheit->dst_id;                   // A full line is being sent
    #endif      
      requestBuffer[cfg].req_sender = cacheit->req_sender;
      requestBuffer[cfg].uid = cacheit->uid;                 // uid of the instruction
      requestBuffer[cfg].memreq_id = cacheit->memreq_id;
      requestBuffer[cfg].message_type = memreq_types::type_REQUEST;
      requestBuffer[cfg].cachable = cacheit->cachable;
      requestBuffer[cfg].cacheQueueIndex = cacheQueueIndex;
      requestBuffer[cfg].ready = true;                       // The request is ready to be sent
    }
  }//End of SendCacheDatatoMem()

#ifdef CACHE_CHECK_COHERENCY
  static void check_coherency()
  { ListPointer < module > mod1, mod2;
    for(mod1 = all_caches.Begin(); mod1; mod1++)
    { module *m1 = mod1;
      this_class *c1 = (this_class*) m1;
      for(mod2 = mod1; mod2; mod2++)
      { module *m2 = mod2;
        this_class *c2 = (this_class*) m2;
        if(c1!=c2)
        { c1->cache.check_coherency(c1->name(),c2->name(),c2->cache);
        }
      }
    }
  }
#endif
  
 private:
  char cache_log[255];  ///< Log file name
  static const int cacheQueueIndex      = 1;
  static const int cacheSnoopQueueIndex = 2;
 
  int nCPULineSize; ///< min data size that has to be sent towards the CPU, if 0 then the value is ignored (e.g. connected to the CPU), else the size of the cache line in level -1. it is used to set the base address of the requested data
 
  bool inMEM_was_a_hit[nConfig];
  bool inMEM_was_a_cacheQueue_hit[nConfig];
  /**
   * \brief Dump some debugging information
   */
  void dump()
  { if(timestamp()==0)
    { ofstream os(cache_log,ios_base::trunc);
      os.close();
    }
    ofstream os(cache_log,ios_base::app);
    os << cache;
    os.close();
  }

  void dump(ostream &os)
  { os << cache;
  }

  virtual void ctrlz_hook()
  { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;
    cerr << fsc_object::name() << endl;
    for(int cfg=0; cfg<nConfig; cfg++)
    {
      cacheit = cacheQueue[cfg].SeekAtHead();
      
      cerr << " - cacheQueue.head = ";
      if(cacheit) cerr << *cacheit << endl;
      else cerr << "<none>" << endl;
      cacheit = cacheSnoopQueue[cfg].SeekAtHead();
      cerr << " - cacheQueue.head = ";
      if(cacheit) cerr << *cacheit << endl;
      else cerr << "<none>" << endl;
    }    
  }

// Taj- functions needed by power service
public:
  //  virtual long long int GetReadAccessCount () { return accesses_read; }     ///< Power estimation interface : Retunrs the number of read access so far
  //  virtual long long int GetWriteAccessCount () { return  accesses_write; }  ///< Power estimation interface : Retunrs the number of write access so far
  /**
   * \brief Power estimation interface : provide the initialization parameters to cacti
   */
  /*
  virtual bool GetCacheParams ( cacti_input_parameter_type * params)
  { if ( params == NULL )
    { cerr << "CacheWB : GetCacheParams() passed NULL pointer \n" ;
      return false ;
    }
    else 
    { params->cache_size = nLineSize * nCacheLines * nAssociativity ;
      params->line_size = nLineSize ;
      params->associativity = nAssociativity ;
      params->rw_ports = 1 ;     // ?? NOte: Hardcoded ??
      params->excl_read_ports = 0 ; // ?? -- ditto --
      params->excl_write_ports = 0 ;  // ?? -- ditto --
      params->single_ended_read_ports = 0; // ?? -- ditto --
      params->banks   = 1 ;      // --ditto --
      params->tech_node = 0.07 ; // -- ditto --
      params->output_width = nCachetoCPUDataPathSize ; // differetn for outgoing and incoming data ??
      params->specific_tag = 0;
      params->tag_width  = 0; 
      params->access_mode = 0;
      params->pure_sram = 0;
    }
    return true ;
  }
  */
// Taj - fastforwarding related  
  bool fastforwarding;                                                  ///< Fastforwaring inteface : Wether we are currently forwarding or not
  void inline SetFastForwarding( bool val){ fastforwarding = val;}      ///< Toggle fast-forwarding
  inline void publicdump(ostream &os){ dump(os);}                       ///< Pretty printer

  /**
   * \brief Module interface : Returns true if the module has some pending operations.
   */
  /*
  bool has_pending_ops()
  { if(!cacheQueue.Empty())
    { //cout <<"CACHEWB: cacheQueue not empty "<<endl;
      return true;
    }
    if(!cacheSnoopQueue.Empty() )
    { //cout <<"CACHEWB: cacheSnoopQueue not empty "<<endl;
      return true;
    }
    return false;
  }
  */

 int get_rank()
 { return inCPU.get_connected_module()->get_rank()+1;
 }

};

} // end of namespace cache
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif //__CACHEWB_H__
