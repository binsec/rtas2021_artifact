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

#ifndef __UNISIM_COMPONENT_CLM_CACHE_CACHE_WB_MPERFECT_HH__
#define __UNISIM_COMPONENT_CLM_CACHE_CACHE_WB_MPERFECT_HH__

//#include "unisim/unisim.h"
#include <unisim/kernel/clm/engine/unisim.h>
//#include "mem_common.h"
#include <unisim/component/clm/memory/mem_common.hh>
//#include "memreq.h"
#include <unisim/component/clm/interfaces/memreq.hh>
//#include "common.h"
#include <unisim/component/clm/utility/common.hh>
//#include "base/utility.h"
#include <unisim/component/clm/utility/utility.hh>
#include <unisim/component/clm/utility/error.h>
//#include "system/CacheContainer.h"
//#include "system/memory/cache/CacheCommon.h"
#include <unisim/component/clm/cache/cache_container.hh>
#include <unisim/component/clm/cache/cache_common.hh>

//#include <generic/memory/memory_interface.hh>
//#include <unisim/service/interfaces/memory.hh>
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
  int nPorts = 1
>
class CacheWBMPerfect : public module
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

  typedef CacheWBMPerfect < INSTRUCTION, nCPUtoCacheDataPathSize, nCachetoCPUDataPathSize, nMemtoCacheDataPathSize, 
                    nCachetoMemDataPathSize, nLineSize, nCacheLines, nAssociativity, nStages, nDelay, nProg, Snooping, VERBOSE, nPorts
                  > this_class;

  /* --------------------------------------------------------------- */
  /*  Module state                                                   */
  /* --------------------------------------------------------------- */

  static const ReplacementPolicyType replacementPolicy = randomReplacementPolicy;

  /// The structure that will keep the tags and data of the cache
  CacheContainer<nCacheLines, nLineSize, nAssociativity, replacementPolicy, 1> cache;
  /// The cache pipeline
  Queue<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheQueue[nPorts];
  Queue<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheSnoopQueue;
  /// Used as delayed write buffer of the cache
  DelayedWriteBuffer<nLineSize> writeBuffer;
  /// Stock temporaly the data sent towards the CPU
  ReturnData<INSTRUCTION, nLineSize, nCachetoCPUDataPathSize> returnBuffer;
  /// Stock temporaly the data sent towards the memory system
  RequestData<INSTRUCTION, nMemtoCacheDataPathSize, nLineSize> requestBuffer;

  memreq < INSTRUCTION, nCPUtoCacheDataPathSize > cache_request[nPorts];
  bool cache_valid[nPorts];

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

  uint64_t accesses;            ///< Total number of accesses statistic
  uint64_t accesses_read;       ///< Total number of read accesses statistic
  uint64_t accesses_write;      ///< Total number of write accesses statistic
  uint64_t accesses_prefetch;   ///< Total number of prefetch accesses statistic
  uint64_t accesses_evict;      ///< Total number of evict accesses statistic
  uint64_t hits;                ///< Total number of hits statistic
  uint64_t hits_read;           ///< Total number of read hits statistic
  uint64_t hits_write;          ///< Total number of write hits statistic
  uint64_t hits_prefetch;       ///< Total number of pretech hits statistic
  uint64_t hits_evict;          ///< Total number of evict hits statistic
  uint64_t misses;              ///< Total number of misses statistic
  uint64_t misses_read;         ///< Total number of read misses statistic
  uint64_t misses_write;        ///< Total number of write misses statistic
  uint64_t misses_prefetch;     ///< Total number of prefetch misses statistic
  uint64_t misses_evict;        ///< Total number of evict misses statistic
  uint64_t writebacks;          ///< Total number of writebacks statistic
  uint64_t writebacks_read;     ///< Total number of read writebacks statistic
  uint64_t writebacks_write;    ///< Total number of write writebacks statistic
  uint64_t writebacks_prefetch; ///< Total number of prefetch writebacks statistic
  uint64_t writebacks_evict;    ///< Total number of evict writebacks statistic

  /* --------------------------------------------------------------- */
  /*  Module ports                                                   */
  /* --------------------------------------------------------------- */

  inclock inClock;                                                     ///< Clock port

  inport  < memreq < INSTRUCTION, nCPUtoCacheDataPathSize > > inCPU[nPorts];   ///< Input port for requests
  outport < memreq < INSTRUCTION, nCachetoCPUDataPathSize > > outCPU[nPorts];  ///< Output port for sending data toward CPU
  inport  < memreq < INSTRUCTION, nMemtoCacheDataPathSize > > inMEM;   ///< Input port for answers data
  outport < memreq < INSTRUCTION, nCachetoMemDataPathSize > > outMEM;  ///< Output port for sending request toward memory hierarchy

  outport <bool,Snooping> outSharedMEM;                                ///< Output port for the shared bit used by some CMP models

  /* --------------------------------------------------------------- */
  /*  Class constructor & destructor                                 */
  /* --------------------------------------------------------------- */

  /**
   * \brief Module constructor
   */
  CacheWBMPerfect(const char *name, Object *parent = 0) : module(name)
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

    for (int port=0; port<nPorts; port++)
      {
	inCPU[port].set_unisim_name(this,"inCPU",port);
      }
    inMEM.set_unisim_name(this,"inMEM");
    for (int port=0; port<nPorts; port++)
      {
	outCPU[port].set_unisim_name(this,"outCPU",port);
      }
    outMEM.set_unisim_name(this,"outMEM");
    outSharedMEM.set_unisim_name(this,"outSharedMEM");
    // Set the sensitivity list of the module
    sensitive_pos_method(start_of_cycle) << inClock;
    sensitive_neg_method(end_of_cycle) << inClock;
//stf    sensitive_method(on_CPU_data) << inCPU.data << outCPU.accept;
    for (int port=0; port<nPorts; port++)
      {
	//	sensitive_method(on_data) << inCPU[port].data << outCPU[port].accept << inMEM.data;
	sensitive_method(on_data) << inCPU[port].data << inMEM.data;
	
	sensitive_method(on_CPU_accept) << outCPU[port].accept;
      }
//    sensitive_method(on_MEM_data) << inMEM.data;
    if(Snooping)
    { // With cache snooping, new request can come from the memory side, and may need to be
      // put in the pipeline. So we need to compute if the pipeline will be able to store
      // this new request. This computation may depend on outCPU.accept.
//stf      sensitive_method(on_MEM_data) << outCPU.accept;
      sensitive_method(on_shared_accept) << outSharedMEM.accept;
    }
    // DD: for Snooping
    sensitive_method(on_MEM_accept) << outMEM.accept << inMEM.enable;
    
    // Module state initialization
    accesses = accesses_read = accesses_write = accesses_prefetch = accesses_evict = 0;
    hits = hits_read = hits_write = hits_prefetch = hits_evict = 0;
    misses = misses_read = misses_write = misses_prefetch = misses_evict = 0;
    writebacks = writebacks_read = writebacks_write = writebacks_prefetch = writebacks_evict = 0;
    // Logs
    sprintf(cache_log,"cache_%s.log",name);
    // DD Debug valgrind :
    nCPULineSize = 0;

    for (int port=0; port<nPorts; port++)
    {
      cache_valid[port] = false;
    }

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
  bool FlushMemory(address_t addr, uint32_t size)
  { // Flush the lines matching the addresses. Several lines may be flushed if size is big enough
    for ( address_t word_addr=addr; word_addr<(addr+size); word_addr=((word_addr+nLineSize) & ~(nLineSize - 1)) )
    { if (cache.hit(word_addr))
      { uint32_t t_set = cache.getSet(word_addr);
        uint32_t t_line = cache.getLine(word_addr);
#ifdef DEBUG_CACHE_FLUSHMEMORY
        if (cache.getWrite(t_set,t_line)) 
        { cerr << "\e[1;36m" << timestamp() << ": " << name() << ": "<< "FlushMemory hit on \e[1;31mmodified\e[0m addr=0x" << hex << addr << dec << ", " << size << " word_addr=0x" << hex << word_addr << dec << endl;
        }
        else
        { cerr << "\e[1;36m" << timestamp() << ": " << name() << ": "<< "FlushMemory hit \e[0m addr=0x" << hex << addr << dec << ", " << size << " word_addr=0x" << hex << word_addr << dec << endl;
        }
#endif
        if (cache.getWrite(t_set,t_line)) 
        { // The line to be flushed is modified, we should write back it to the memory first.
          char buffer[nLineSize];
          address_t line_address = cache.getAddress(t_set, t_line);
          cache.getCacheLine(buffer, t_set, t_line);
          //syscall_MemImp->WriteMemory(line_address,buffer,nLineSize);
          memory_injection_import->InjectWriteMemory(line_address,buffer,nLineSize);
        }
        cache.mesi_invalidate(t_set, t_line);
        cache.setValid(t_set, t_line, false);
        cache.setWrite(t_set, t_line, false);
      }
    }

  }

  /**
   * \brief Memory interface : Write all back
   */
  bool WriteAllBack()
  { // Instantly writeback all the modified lines to the memory
  
    for(int t_set = 0; t_set < (nCacheLines / nAssociativity); t_set++)
    { for(int t_line = 0; t_line < nAssociativity; t_line++)
      { if(cache.getValid(t_set,t_line)) 
        { // The line is a local hit
          if (cache.getWrite(t_set,t_line)) 
          { // The line is modified, we should write back it to the memory
            char buffer[nLineSize];
            address_t line_address = cache.getAddress(t_set, t_line);
            cache.getCacheLine(buffer, t_set, t_line);
            //syscall_MemImp->WriteMemory(line_address,buffer,nLineSize);
            memory_injection_import->InjectWriteMemory(line_address,buffer,nLineSize);

            cache.setWrite(t_set, t_line, false);
            cache.setMESI(t_set, t_line, MESI::state_EXCLUSIVE);

//cerr << "Found a line to write back : " << t_set << ", " << t_line << endl;
//exit(1);
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
    for ( address_t word_addr=addr; word_addr<(addr+size); word_addr=((word_addr+nLineSize) & ~(nLineSize - 1)) )
    { if (cache.hit(word_addr))
      { uint32_t t_set = cache.getSet(word_addr);
        uint32_t t_line = cache.getLine(word_addr);
        if (cache.getValid(t_set,t_line))
          if (cache.getWrite(t_set,t_line))
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
            cache.getCacheLineIndexed(((char *)buffer)+write_offset, t_set, t_line, word_addr-base_addr, t_size);
          }
      }
    }
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
    { if (cache.hit(word_addr))
      { uint32_t t_set = cache.getSet(word_addr);
        uint32_t t_line = cache.getLine(word_addr);
//Taj
#ifdef DD_DEBUG_SYSCALL_DCACHE_RM
            cerr << "\t *cacheline read_hit       : " << endl;
#endif
        if (cache.getValid(t_set,t_line)){	// Taj - invalid case handled in else
          if (cache.getWrite(t_set,t_line))
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
            cache.getCacheLineIndexed(((char *)buffer)+write_offset, t_set, t_line, word_addr-base_addr, t_size);


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
			cache.setCacheLine( t_set,t_line, base_addr, ((char*)buffer)+write_offset, true, false); //Taj - replce whole line 
//	        	cache.setWrite(t_set, t_line, false); //u can also modify the last parameter in above set function, see later
			// new state = E or S ?? when procerror reads  how do the other caches know about it ??
			cache.setMESI(t_set, t_line,MESI::state_EXCLUSIVE);
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
		uint32_t t_set = cache.getReplaceSet(word_addr);
	        uint32_t t_line = cache.getReplaceLine(word_addr);

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
		if( cache.getValid (t_set, t_line )){ //see that only valid modified lines are evicted
		if( cache.getWrite(t_set, t_line) ){

#ifdef DD_DEBUG_SYSCALL_DCACHE_RM
	    cerr << "\t cacheline miss Evicting, updating with data from memory" <<endl ;
            //cerr << "\t * word addr    : " << hexa(word_addr) << endl;
            //cerr << "\t * base addr    : " << hexa(base_addr) << endl;
            //cerr << "\t * write_offset : " << write_offset << endl;
            //cerr << "\t * t_size       : " << t_size << endl;
#endif

			//write previous line to memory - eviction ??
			address_t evicted_addr = cache.getAddress( t_set, t_line) ; //addr of prev line in cache
		        char line_buffer[nLineSize];
	        	cache.getCacheLine(line_buffer, t_set, t_line);
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
		cache.setCacheLine( t_set,t_line, base_addr, line_bufferx, true, false); //Taj - get non-buffer data from memory 
		//update line with data from memory 
	        //cache.setCacheLineIndexed(t_set, t_line, ((char *)buffer)+write_offset, word_addr-base_addr, t_size, true);
		//cache.setCacheLine( t_set,t_line, base_addr, ((char*)buffer)+write_offset, true, true ); //Taj - replce whole line 
	
		// setCacheLineIndexed mark the line as modified. 
	        // let's reset the Write field.
        	//cache.setWrite(t_set, t_line, false);  // see last param in above set() function

		//newstate = modified, will be after update
		cache.setMESI(t_set, t_line,MESI::state_EXCLUSIVE);

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

    for ( address_t word_addr=addr; word_addr<(addr+size); word_addr=((word_addr+nLineSize) & ~(nLineSize - 1)) )
    { 
    
      address_t base_addr = (word_addr & ~(nLineSize - 1)); // Address of the cache line
    
#ifdef DEBUG_CACHE_WRITEMEMORY
//cerr << "... line_address=" << hex << base_addr << dec << endl;
#endif

      if (cache.hit(word_addr))
      { uint32_t t_set = cache.getSet(word_addr);
        uint32_t t_line = cache.getLine(word_addr);


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
        cache.setCacheLineIndexed(t_set, t_line, ((char *)buffer)+write_offset, word_addr-base_addr, t_size, true);
        // setCacheLineIndexed mark the line as modified. As we'll also forward the MemoryWrite to the memory,
        // let's reset the Write field.
        cache.setWrite(t_set, t_line, false);
        // The mesi state goes to exclusive as other caches are flushed
        cache.setMESI(t_set, t_line,MESI::state_EXCLUSIVE);


#ifdef DEBUG_CACHE_WRITEMEMORY
//  cerr << "after  : ";
//  cache.dump(cerr,t_set,t_line);
#endif

        //Send the modified cache line to the memory to ensure that line data not bellonging to the
        //buffer is also wrote-back.
        char line_buffer[nLineSize];
        cache.getCacheLine(line_buffer, t_set, t_line);
	//        syscall_MemImp->WriteMemory(base_addr,line_buffer,nLineSize);
        memory_injection_import->InjectWriteMemory(base_addr,line_buffer,nLineSize);
      }
     
    }

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

    for ( address_t word_addr=addr; word_addr<(addr+size); word_addr=((word_addr+nLineSize) & ~(nLineSize - 1)) )
    { 
    
      address_t base_addr = (word_addr & ~(nLineSize - 1)); // Address of the cache line
    
#ifdef DEBUG_CACHE_WRITEMEMORY
//cerr << "... line_address=" << hex << base_addr << dec << endl;
#endif

      if (cache.hit(word_addr))
      { uint32_t t_set = cache.getSet(word_addr);
        uint32_t t_line = cache.getLine(word_addr);


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
        cache.setCacheLineIndexed(t_set, t_line, ((char *)buffer)+write_offset, word_addr-base_addr, t_size, true);
        // setCacheLineIndexed mark the line as modified. As we'll also forward the MemoryWrite to the memory,
        // let's reset the Write field.
//        cache.setWrite(t_set, t_line, false);
        // The mesi state goes to exclusive as other caches are flushed
        cache.setMESI(t_set, t_line,MESI::state_MODIFIED);


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
		uint32_t t_set = cache.getReplaceSet(word_addr);
	        uint32_t t_line = cache.getReplaceLine(word_addr);

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
		if( cache.getValid( t_set, t_line) ){
			//else if ( line_modified )
			if( cache.getWrite(t_set, t_line) ){

			//write existing line to memory, eviction ??
			char line_buffer[nLineSize];
			address_t evicted_addr = cache.getAddress( t_set, t_line) ;//address-in-memory of line to be evicted
	        	cache.getCacheLine(line_buffer, t_set, t_line);
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
		cache.setCacheLine( t_set,t_line, base_addr, line_bufferx, true, false); //Taj - get non-buffer data from memory 
		//write data into cache line
        	cache.setCacheLineIndexed(t_set, t_line, ((char *)buffer)+write_offset, word_addr-base_addr, t_size, true);
		//cache.setCacheLine( t_set,t_line, base_addr, ((char*)buffer)+write_offset, true, true ); //Taj - replce whole line 
		
		//new state = M, E if memory write thorugh
		// setCacheLineIndexed mark the line as modified. As we'll also forward the MemoryWrite to the memory,
	        // let's reset the Write field. //see last param in above set() func
//	        cache.setWrite(t_set, t_line, false); //Taj - why ?? the line content is different unless written to mem below.
	        // The mesi state goes to exclusive as other caches are flushed
	        cache.setMESI(t_set, t_line,MESI::state_MODIFIED);


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
      cerr << "[Cnonfig::all][DD_DEBUG_DCACHE("<<timestamp()<<")]  Begin of start_of_cycle..." << endl;
    }
#endif

#ifdef DD_DEBUG_DCACHE_VERB_1000
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr << "[Cnonfig::all][DD_DEBUG_DCACHE("<<timestamp()<<")]  Begin of start_of_cycle..." << endl;
      for (int port=0; port<nPorts; port++)
	{
	  cerr << "cache_valid[" << port << "]=" << cache_valid[port] << endl;
	}
      for (int port=0; port<nPorts; port++)
	{
	  cerr << "cache_request[" << port << "]=" << cache_request[port] << endl;
	}
    }
#endif


    // checks if there is something to be sent from the cache pipeline and 
    // the MSHR queue and send them to the cpu and memory system
    //    SendData();
    outMEM.data.nothing();
   
    for (int port=0; port<nPorts; port++)
    {
      if (cache_valid[port])
      {
   	outCPU[port].data = cache_request[port];
      }
      else
	{
	  outCPU[port].data.nothing();
	}
    }
 
    inMEM_was_a_hit = false;
    inMEM_was_a_cacheQueue_hit = false;

    if(VERBOSE) dump();
#ifdef DD_DEBUG_DCACHE
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
      cerr << "[DD_DEBUG_DCACHE] Cache Dump : " << endl; 
      cerr << *this << endl;
    }
#endif
#ifdef DD_DEBUG_DCACHE_VERB100
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr << "[Cnonfig::all][DD_DEBUG_DCACHE("<<timestamp()<<")]  End of start_of_cycle..." << endl;
    }
#endif

  }


  /**
   * \brief Process called on clock falling edge.
   */
  void end_of_cycle()
  { 
#ifdef DD_DEBUG_DCACHE_VERB100
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr << "[Cnonfig::all][DD_DEBUG_DCACHE("<<timestamp()<<")]  Begin of end_of_cycle..." << endl;
    }
#endif
    /*
    // checks that the data sent in the previous cycle has been accepted and update the state of 
    // the cache pipeline and the MSHR queue if necessary
    CheckAcceptedData();
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
    */
    for (int port=0; port<nPorts; port++)
    {
	
      //	if (outCPU[port].accept && inCPU[port].enable)
      if (inCPU[port].enable)
      {

#ifdef DD_DEBUG_PERFECT_BRANCH_V2
                      if (DD_DEBUG_TIMESTAMP < timestamp())
                        { 
                          memreq<INSTRUCTION, nCachetoCPUDataPathSize> inreq=inCPU[port].data;
			  //                            cerr << "Entry: " << entry << endl; 
                          cerr << "TimeStamp (DCACHE on_CPU_data) = " << timestamp() << endl;
                          cerr << "DD_DEBUG_PERFECT: memreq ="<< inreq << endl;
                        }
#endif

	cache_request[port] = inCPU[port].data;
	
    
	
	
	memreq<INSTRUCTION, nCachetoCPUDataPathSize> &outreq=cache_request[port];
	memreq<INSTRUCTION, nCachetoCPUDataPathSize> inreq=inCPU[port].data;
	char tmp_buffer[nCachetoCPUDataPathSize];
	switch(inreq.command)
	  { 
          case memreq_types::cmd_WRITE:
            // increment the number of write accesses
            accesses_write++;
            InjectWriteMemory(inreq.address, inreq.Read(), inreq.size);
            //      tmp_buffer = Read
	    //            outCPU[port].data.nothing();
	    cache_valid[port] = false;
            
            break;
          case memreq_types::cmd_READ:
            // increment the number of read accesses
            accesses_read++;
            
            outreq.message_type = memreq_types::type_ANSWER;
            outreq.instr = inreq.instr;
            outreq.size = inreq.size > nCachetoCPUDataPathSize ? nCachetoCPUDataPathSize : inreq.size;
            outreq.address = inreq.address;//inreq.base + inreq.index;                                                                          
            char tmp_buff[nCachetoCPUDataPathSize];
            InjectReadMemory(inreq.address, tmp_buff,
                             (inreq.size > nCachetoCPUDataPathSize) ? nCachetoCPUDataPathSize : inreq.size );
                   
            outreq.Write(tmp_buff, (inreq.size > nCachetoCPUDataPathSize) ? nCachetoCPUDataPathSize : inreq.size);
            outreq.uid = inreq.uid;
            outreq.memreq_id = inreq.memreq_id;                                                                                                 outreq.sender_type = memreq_types::sender_CACHE;
            outreq.sender = this;
            outreq.req_sender = inreq.req_sender;
            outreq.cachable = inreq.cachable;
            outreq.command = memreq_types::cmd_READ; // Only reads are sent back to the CPU
	    //            outCPU[port].data = outreq;
	    cache_valid[port] = true;

            break;
          case memreq_types::cmd_PREFETCH:
            // increment the number of prefetch accesses
            //        accesses_prefetch++;
            //        entry->state = PREFETCH;
            //      break;
            outCPU[port].data.nothing();

          case memreq_types::cmd_EVICT:
            // increment the number of evict accesses
            //        accesses_evict++;
            //        entry->state = EVICT;
            //      break;
            outCPU[port].data.nothing();
          case memreq_types::cmd_READX:
            //        accesses_read++;
            //        entry->state = READ;
            //        ERROR << "Received a READX from the CPU !" << endl; // THIS WAS ONLY TRUE FOR L1 CACHE. L2 CACHES ARE RECEIVING READX FROM L1 CACHES
            //        abort();                                                                                                                  //      break;
            outCPU[port].data.nothing();
          default:
            ERROR << "Undefined mr.command: " << inreq.command << endl;
            exit(1);
            break;
          }
      }
    }
#ifdef DD_DEBUG_DCACHE_VERB100
    if (DD_DEBUG_TIMESTAMP <= timestamp())
    {
      cerr << "[Cnonfig::all][DD_DEBUG_DCACHE("<<timestamp()<<")]  End of end_of_cycle..." << endl;
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
  { os << "==================== " << cache.fsc_object::name() << " =====================" << endl;
    os << "Cache Pipeline:" << endl;
    os << cache.cacheQueue;
    os << "----------------------------------------------------------" << endl;
    if(Snooping)
    { os << "Cache Snoop Pipeline:" << endl;
      os << cache.cacheSnoopQueue;
      os << "----------------------------------------------------------" << endl;
    }
    os << "Delayed Write Buffer:" << endl;
    os << cache.writeBuffer << endl;
    os << "Return Buffer:" << endl;
    os << cache.returnBuffer << endl;
    os << "Request Buffer:" << endl;
    os << cache.requestBuffer << endl;
    os << "==========================================================" << endl;
    return os;
  }

  /* --------------------------------------------------------------- */
  /*  Module private methods                                         */
  /* --------------------------------------------------------------- */

  /**
   * \brief Sends data to the cpu and the memory system if returnBuffer and/or requestBuffer are ready
   */
  /*
  void SendData()
  { // If returnBuffer is empty, cacheQueue is not empty and its head is ready. Then copy from
    // head to return buffer.
#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
    cerr << "["<<this->name()<<"("<<timestamp()<<")]: SendData, start..." << endl;
    }
#endif
    // If returnBuffer is empty , cacheQueue is not empty and its head ready, copy from head to returnBuffer
    if(!returnBuffer.ready) SendCacheDatatoCPU();
#ifdef DD_DEBUG_DCACHE_VERB2
    else
    {
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
      cerr << "["<<this->name()<<"("<<timestamp()<<")]: Warning: returnBuffer.ready == True (no call to SendCacheDatatoCPU) ..." << endl;
    }
    }
#endif
    // if there is nothing in the requestBuffer the cache can try to sent something
    // to the memory system
    if(Snooping)
    { if(!requestBuffer.ready) SendSnoopDatatoMem();
    }
    if(!requestBuffer.ready) SendCacheDatatoMem();

//cerr << "\033[31mCACHE: SendData  returnBuffer.ready="<< returnBuffer.ready << " ,requestBuffer.ready=" << requestBuffer.ready << " \033[0m" << endl;  
    // if the returnBuffer is ready then set output ports to the cpu
    if(returnBuffer.ready)
      { // set the output signals
      memreq<INSTRUCTION, nCachetoCPUDataPathSize> req;
      req.message_type = memreq_types::type_ANSWER;
      req.instr = returnBuffer.instr;
      req.size = returnBuffer.size > nCachetoCPUDataPathSize ? nCachetoCPUDataPathSize : returnBuffer.size;
      req.address = returnBuffer.base + returnBuffer.index;
      req.Write(&returnBuffer.data[returnBuffer.index], (returnBuffer.size > nCachetoCPUDataPathSize) ? nCachetoCPUDataPathSize : returnBuffer.size);
      req.uid = returnBuffer.uid;
      req.memreq_id = returnBuffer.memreq_id;
      req.sender_type = memreq_types::sender_CACHE;
      req.sender = this;
      req.req_sender = returnBuffer.req_sender;
      req.cachable = returnBuffer.cachable;
      req.command = memreq_types::cmd_READ; // Only reads are sent back to the CPU
#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
      cerr <<"["<<this->name()<<"("<<timestamp()<<")]: Sending to CPU: "<< req << endl;
    }
#endif
#ifdef DEBUG_TEST_UNCACHABLE_WRITE
      //if( !req.cachable && req.command==memreq_types::cmd_WRITE ) 
      INFO << "-CPU-> \e[1;31muncachable write\e[0m: " << req << endl;
#endif
      outCPU[port].data = req;
    }
    else outCPU.data.nothing();
    
    // if the requestBuffer is ready then set output signals to the memory system
    if(requestBuffer.ready)
    { memreq<INSTRUCTION, nCachetoMemDataPathSize> req;
      req.message_type = requestBuffer.message_type;
      req.instr = requestBuffer.instr;
#ifdef NOC_THREADS_DISTRIBUTION
      req.address = requestBuffer.address+ requestBuffer.index;
#else
      req.address = requestBuffer.address;

#endif
      req.command = requestBuffer.command;     
      
      int size = requestBuffer.size;
      if(requestBuffer.write && size > nCachetoMemDataPathSize)
      { size = nCachetoMemDataPathSize;
      }
      req.size = size;
      if(requestBuffer.write)
      { req.Write(requestBuffer.data + requestBuffer.index, size);
      }
      req.uid = requestBuffer.uid;
      req.memreq_id = requestBuffer.memreq_id;
      req.sender_type = memreq_types::sender_CACHE;
      req.cachable = requestBuffer.cachable;
      req.sender = this;
      
      if(requestBuffer.message_type==memreq_types::type_REQUEST)
      { // This is a new request, set this module as the original sender
        req.req_sender = this;
      }
      else
      { // This is an answer to an existing request, forward the original sender value
        req.req_sender = requestBuffer.req_sender;
      }
#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP < timestamp())
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
      req.dst_id = requestBuffer.dst_id;
      if ( (req.command == memreq_types::cmd_WRITE) && (req.cachable == true) )
            req.dst_id = 0;
    #endif


      outMEM.data = req;
    }
    else outMEM.data.nothing();
#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
    cerr << "["<<this->name()<<"("<<timestamp()<<")]: SendData, end..." << endl;
    }
#endif
  }
*/

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
  { 

    bool areall_known = true;
    for (int port=0; port<nPorts; port++)
    {
      areall_known &= outCPU[port].accept.known();
    }
    //    if(inCPU.data.known() && outCPU.accept.known())
    if(areall_known)
    { // Upon receiving a request, check if it can be serviced/accepted
      //if(outCPU.accept.known())
      //    {
//if(outCPU.accept) cerr << "\033[31mCACHE: recieved accept from CPU enabling CPU "<< outCPU.accept.known() <<" \033[0m" << endl;  
      for (int port=0; port<nPorts; port++)
      {
	outCPU[port].enable = outCPU[port].accept;
	cache_valid[port] = false;
      }
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
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
      if(outMEM.accept) cerr << "["<<this->name()<<"("<<timestamp()<<"] Cache recieved accept from MEM enabling MEM" << endl;  
      //      else cerr << "\033[31mCACHE: MEM didn't accept the previous request !!!!!!!!!!!! \033[0m" << endl; 
    }
#endif
    // DD : for Snooping
    //      outMEM.enable = outMEM.accept;
	// DD: before enabling outMEM data we have to check if a another processor just answer the same request.
	if (!inMEM.enable)
	{
	  outMEM.enable = outMEM.accept;
	}
	else
	{
	  // We check outMEM.accept is true to be sure that outMEMdata is not an older value sent previous cylce.
	  if (outMEM.accept)
	  {
	    // Make sure the incoming data is a read to be handled by the snooper
	    memreq<INSTRUCTION, nMemtoCacheDataPathSize> mrin = inMEM.data;
	    memreq<INSTRUCTION, nMemtoCacheDataPathSize> mrout = outMEM.data;
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
	      outMEM.enable = false;	
	    }
	    else
	    {
	      outMEM.enable = outMEM.accept;
	    }
	  }
	  else
	  {
	    outMEM.enable = outMEM.accept;
	  }
	}
    //
    }
  }

  /**
   * \brief Returns true if a new request can be accepted (If the cache pipeline allows
   * to insert a new request)
   * 
   * outCPU.accept should be known before calling this method 
   */
  bool can_accept_request(Queue<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> &queue)
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
              return cache.hit(cacheit->address);
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
              if(outCPU.accept) return (returnBuffer.size <= nCachetoCPUDataPathSize);
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
  bool request_queue_hit(address_t addr)
  { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit = cacheQueue.SeekAtHead();
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
  {
    bool areall_known = true;
    for (int port=0; port<nPorts; port++)
    {
      areall_known &= (inCPU[port].data.known());
    }
    //    if(inCPU.data.known() && outCPU.accept.known())
    if(areall_known)
    { // Upon receiving a request, check if it can be serviced/accepted

      // if(inCPU.data.known() && outCPU.accept.known())
      //    { // Upon receiving a request, check if it can be serviced/accepted
      //      INFO << "[DD_DEBUG_DCACHE] <DCACHE> on_CPU_data() REQ: " << inCPU.data << endl;

      for (int port=0; port<nPorts; port++)
      {
	inCPU[port].accept = inCPU[port].data.something();


      }
      /*
      for (int port=0; port<nPorts; port++)
      {
	if(inCPU[port].data.something())
	{
	  memreq<INSTRUCTION, nCachetoCPUDataPathSize> outreq;
	  memreq<INSTRUCTION, nCachetoCPUDataPathSize> inreq=inCPU[port].data;
	  char tmp_buffer[nCachetoCmemreq<INSTRUCTION, nCachetoCPUDataPathSize> inreq=inCPU[port].data;PUDataPathSize];
	  switch(inreq.command)
	  { 
	  case memreq_types::cmd_WRITE:
	    // increment the number of write accesses
	    accesses_write++;
	    InjectWriteMemory(inreq.address, inreq.Read(), inreq.size);
	    //	    tmp_buffer = Read
	    outCPU[port].data.nothing();

	    break;
	  case memreq_types::cmd_READ:
	    // increment the number of read accesses
	    accesses_read++;

	    outreq.message_type = memreq_types::type_ANSWER;
	    outreq.instr = inreq.instr;
	    outreq.size = inreq.size > nCachetoCPUDataPathSize ? nCachetoCPUDataPathSize : inreq.size;
	    outreq.address = inreq.address;//inreq.base + inreq.index;

	    char tmp_buff[nCachetoCPUDataPathSize];
	    InjectReadMemory(inreq.address, tmp_buff,
			     (inreq.size > nCachetoCPUDataPathSize) ? nCachetoCPUDataPathSize : inreq.size );

	    outreq.Write(tmp_buff, (inreq.size > nCachetoCPUDataPathSize) ? nCachetoCPUDataPathSize : inreq.size);
	    outreq.uid = inreq.uid;
	    outreq.memreq_id = inreq.memreq_id;
	    outreq.sender_type = memreq_types::sender_CACHE;
	    outreq.sender = this;
	    outreq.req_sender = inreq.req_sender;
	    outreq.cachable = inreq.cachable;
	    outreq.command = memreq_types::cmd_READ; // Only reads are sent back to the CPU
	    
	    outCPU[port].data = outreq;

	    break;
	  case memreq_types::cmd_PREFETCH:
	    // increment the number of prefetch accesses
	    //        accesses_prefetch++;
	    //        entry->state = PREFETCH;
	    //	    break;
	    outCPU[port].data.nothing();

	  case memreq_types::cmd_EVICT:
	    // increment the number of evict accesses
	    //        accesses_evict++;
	    //        entry->state = EVICT;
	    //	    break;
	    outCPU[port].data.nothing();
	  case memreq_types::cmd_READX:
	    //        accesses_read++;
	    //        entry->state = READ;
	    //        ERROR << "Received a READX from the CPU !" << endl; // THIS WAS ONLY TRUE FOR L1 CACHE. L2 CACHES ARE RECEIVING READX FROM L1 CACHES
	    //        abort();
	    //	    break;
	    outCPU[port].data.nothing();
	  default:
	    ERROR << "Undefined mr.command: " << inreq.command << endl;
	    exit(1);
	    break;
	    }
	  
	  }
    
	  }
      */  
    }
  }

  void on_MEM_data()
  {
    if(!inMEM.data.known()) return;
    inMEM.accept = inMEM.data.something();
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
  void on_MEM_data_old()
  { if(!inMEM.data.known()) return;
    if(Snooping)
    { if(!outCPU.accept.known()) return;
    }
    if(inMEM.data.something())
    { if(Snooping)
      { memreq<INSTRUCTION, nMemtoCacheDataPathSize> mr = inMEM.data;
        if(mr.command == memreq_types::cmd_FLUSH)
        { // Flush requests are always accepted, no need to store the request.
          inMEM.accept = true;
          outSharedMEM.data = false;
          return;
        }

        switch(mr.message_type)
        { case memreq_types::type_REQUEST:
          { // Set the shared if the data seeked by the request is in this cache, 
            // or already requested by the cache
            bool local_hit = cache.hit(mr.address);
            bool can_accept = true;

//zzz
            if(!local_hit)
            { inMEM_was_a_cacheQueue_hit = request_queue_hit(mr.address);
              if(inMEM_was_a_cacheQueue_hit)
              { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit = cacheQueue.SeekAtHead();
                address_t line_addr = mr.address & ~(nLineSize - 1);
                cerr << "[[[---------------------------------------------" << endl;
                INFO << hex << "\e[1;31m" << line_addr << dec << "\e[0m non local hit: " << mr << endl;
                INFO << hex << "\e[1;31m" << line_addr << dec << "\e[0m cacheit: " << *cacheit << endl;
                cerr << " ==>\e[1;32m local hit\e[0m" << endl;
                cerr << "---------------------------------------------]]]" << endl;
                local_hit = true;
              }
            }
            inMEM_was_a_hit = local_hit;

            if(local_hit)
            { // The request made a local cache hit, shared should be set to true.
              // The data can only be accepted if the pipeline has a free slot for
              // this new request.
              outSharedMEM.data = true;
              inMEM.accept = can_accept && can_accept_request(cacheSnoopQueue);
//cerr <<"M ";
            }
            else
            { // Not a local cache hit, shared is to be set to false and 
              // the data can be accepted as it does not create any new request.
              outSharedMEM.data = false;
              inMEM.accept = true; 
            }
          } break;
          case memreq_types::type_ANSWER:
            // Answers from memory should always be accepted, and shared should
            // not be set for memory answers.
            outSharedMEM.data = false;
            inMEM.accept = true;
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
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
        cerr << "["<<this->name()<<"("<<timestamp()<<")]: Accepting Data from Memory (Always)..." << endl;
    }
#endif
        inMEM.accept = true;
      }
    } // !Something
    else 
    {
#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
      cerr << "["<<this->name()<<"("<<timestamp()<<")]: Non-Accepting Data from Memory (!something)..." << endl;
    }
#endif
      inMEM.accept = false;
      if(Snooping) outSharedMEM.data.nothing();
    }
  }
  
  /**
   * \brief Process setting accept signals
   */
  void on_data()
  { // Check that cpu data has arrived

//cerr << name() << ": CPU.data=" << inCPU.data.known()  << " accept=" << outCPU.accept.known() << " MEM.data=" << inMEM.data.known() << endl; STF!!!

    for (int port=0; port<nPorts; port++)
      {
	if(!inCPU[port].data.known()) return;
	//	if(!outCPU[port].accept.known()) return;
      }
    // Check that memory data has arrived
    if(!inMEM.data.known()) return;
    /*
    if(Snooping)
    { if(!outCPU.accept.known()) return;
    };
    */
    // Everything has been received, deal with received values
    // if snnoping, higher priority is given to snooping hit reuests than to cpu requests
    on_MEM_data();
    on_CPU_data();    
  }

  /**
   * \brief Process lauched upon receiving an acept on the shared bit output port
   */
  void on_shared_accept()
  { outSharedMEM.enable = outSharedMEM.accept;
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
  { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;

    /* check if an accept from the cpu is being waited */
    if(returnBuffer.ready)
    { /* check if an accept is received */
      if(outCPU.accept)
      { /* decrement the size of the data being sent to the cpu */
        returnBuffer.size = returnBuffer.size - nCachetoCPUDataPathSize;
        /* if there is still data to be sent set up the returnBuffer,
         * if all the data has been sent modify the state of the pipeline */
        if(returnBuffer.size > 0)
        { /* increment the index of the data being sent */
          returnBuffer.index = returnBuffer.index + nCachetoCPUDataPathSize;
          if(returnBuffer.index == nCPULineSize)
            returnBuffer.index = 0;
          /* set the address of the data being sent */
          returnBuffer.address = returnBuffer.base + returnBuffer.index;
        }
        else
        { /* the returnBuffer is empty */
          /* the cache entry that made the request can be
           * removed, it must be the head the entry to remove */
          cacheit = cacheQueue.SeekAtHead();
          switch(cacheit->state)
          { case READ_HIT_READY:
              /* a read hit can be removed */
            case READ_MISS_LINE_READY:
              /* if it is a miss which line has been completely received it can be removed */
            case READ_MISS_EVICTION_WRITE_DONE_LINE_READY:
              /* if it is a miss (with eviction) which line has been completely received it can be removed */
            case READ_UNCACHABLE_READY:
              // If an uncachable read result has been sent back to the cpu, remove the request
              cacheQueue.RemoveHead();
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
          returnBuffer.ready = false;
        }
      }
    }
  }

  /**
   * \brief Checks if a request sent to the memory system has been accepted and modify the cache pipeline state if needed 
   *
   * Used at End of cycle
   */
  void CheckMemAcceptedDatafromCache()
  { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;
    /* are we waiting for an accept from the memory? */
    if(requestBuffer.ready)
    { /* check if the memory accepted the request */
      if(requestBuffer.cacheQueueIndex!=cacheQueueIndex) return;
      if(outMEM.accept)
      { if(requestBuffer.write)
        { /* this request is a write */
          /* if last request was finished from the cache pipeline (a write request)
           * remove the request from the cache pipeline */
          requestBuffer.size -= nCachetoMemDataPathSize;
          if(requestBuffer.size > 0)
          { /* the request has not bben finished, the request has to be keeped,
             * the address has to be increased,
             * and the cache pipeline is not notified */
            requestBuffer.index += nCachetoMemDataPathSize;
            requestBuffer.address = requestBuffer.base + requestBuffer.index;
          }
          else
          { /* the request has been finished, the cache pipeline has to be updated */
            cacheit = cacheQueue.SeekAtHead();
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
                cacheQueue.RemoveHead();
                break;

              case WRITE_UNCACHABLE_READY:
                // The uncachable write has been performed
                cacheQueue.RemoveHead();
                break;

            #ifdef NOC_THREADS_DISTRIBUTION
              case WRITE_UNCACHABLE:
                // The READX to get the uncachable write has been accepted from memory
                cacheQueue.RemoveHead();
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
                cacheQueue.RemoveHead();
                break;
            }
            /* reset the request buffer */
            requestBuffer.ready=false;
          }
        }
        else // not a write
        { /* the access is a cache miss
           * the cache entry (the head of the cache queue) 
           * is not removed from the cache pipeline (this is a blocking cache) */
          /* the request buffer is marked as not ready, to be used with the next memory request */
          cacheit = cacheQueue.SeekAtHead();
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
          requestBuffer.ready=false;
        }
      }
    }
    else
    {
#ifdef DEBUG_CACHEWB
      /* if an accept from the memory system that was received that was not expected, error */
      if(outMEM.accept)
      { cerr << "Error(" << name() << "): an accept was received from the memory system that was not expected" << endl;
        cerr << *this;
        exit(-1);
      }
#endif
    }
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
  void UpdateWriteBuffer()
  {/* check that data was received from the memory system */
    if(inMEM.enable)
    { 
#ifdef DEBUG_BUS_MQ
INFO << "Receiving from M:   " << inMEM.data << endl;
#endif
#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
      cerr <<"["<<this->name()<<"("<<timestamp()<<")]: Warning inMEM.enable == True ..." << endl;
    }
#endif
      memreq<INSTRUCTION, nMemtoCacheDataPathSize> mr = inMEM.data;
      if(Snooping)
      { if(mr.message_type==memreq_types::type_REQUEST)
        { // Snooped requests should not update the write buffer, ignore those
          return;
        }
        else 
        { // The message type is not an request, it is an answer.
          // Discards answers form which request are not issued by this cache
          if(mr.req_sender!=this) 
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

            return;
          }
        }
      } // if(Snooping)
      
      // David Debug :
      // Do not update the write buffer with data not requested !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      if (mr.req_sender != this) return;

      if(this==mr.sender)
      { ERROR << "Receiving a request from myself in UpdateWriteBuffer : " << mr << endl;
        terminate_now() ;
      }

#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
      cerr << "["<<this->name()<<"("<<timestamp()<<")]: inMEM.enable == True, we are storing into writebuffer..." << endl;
    }
#endif
#ifdef DEBUG_TEST_UNCACHABLE_WRITE
      if( !mr.cachable ) INFO << "<-MEM- \e[1;31muncachable write\e[0m: " << mr << endl;
#endif
 
#ifdef DEBUG_CACHEWB
      /* check that it is not an incorrect line */
      if(writeBuffer.address != (mr.address & ~((address_t)nLineSize-1)) && writeBuffer.size != 0)
      { if(writeBuffer.size == nLineSize)
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
        if(writeBuffer.ready[(int)(i + (mr.address & ((address_t)(nLineSize - 1))))])
        { cerr << "Error(" << name() << "): writeBuffer has received two time the same address" << endl;
          cerr << *this;
          exit(-1);
        }
#endif
        writeBuffer.ready[i + (mr.address & ((address_t)(nLineSize - 1)))] = true;
      }
      // update writeBuffer address, size, address and uid fields with incomming data
      memcpy(&(writeBuffer.buffer[mr.address&((address_t)(nLineSize - 1))]), mr.Read(), nMemtoCacheDataPathSize);
      writeBuffer.size += nMemtoCacheDataPathSize;
      writeBuffer.address = mr.address&(~((address_t)(nLineSize - 1)));
      writeBuffer.uid = mr.uid;
      writeBuffer.memreq_id = mr.memreq_id;
      writeBuffer.cachable = mr.cachable;

     // If the shared bit was not set during the corresponding request, we will get the answer
     // from memory as a READ ANSWER.
     // If the shared bit was set, we will get the answer from a WRITE-BACK to memory of another
     // cache snooped on the bus.
     if(Snooping) writeBuffer.shared_bit = (mr.command==memreq_types::cmd_WRITE);
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
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
      cerr << "["<<this->name()<<"("<<timestamp()<<")]: inMEM.enable == True WriteBuffer :" << endl;
      cerr << writeBuffer << endl;
    }
#endif

    } /* inMEM.enable */ 
    else
    {
#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP < timestamp())
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
  void UpdateCachePipelineWithWriteBuffer()
  { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;

    /* the cache can return a requested data to the cpu if the cache line in the writeBuffer
     * already contains the data, this way the cpu has not to wait for the full line before
     * receiving a cache answer */
    /* does the writeBuffer have data? */
    if(writeBuffer.size != 0)
    { cacheit = cacheQueue.SeekAtHead();
      /* the action is just performed if the cache request was a read */
      if(cacheit)
      { if(!cacheit->write)
        {
#ifdef DEBUG_CACHEWB
          /* check that the head of the pipeline and the writeBuffer have the same address */
          if((cacheit->address & (~(address_t)(nLineSize - 1))) != writeBuffer.address)
          { cerr << "Error(" << name() << "): the cache line request address does not correspond "
            << "with the address in the writeBuffer." << endl;
            cerr << "\t\t\tstate = " << cacheit->state
            << " (" << hexa(cacheit->address & (~(address_t)(nLineSize - 1)))
            << " -- " << hexa(writeBuffer.address) << ")" << endl;
            cerr <<  *this;
            exit(-1);
          }
#endif
          /* get the index of the data in the writeBuffer */
          unsigned int base_address = 0;
          if(nCPULineSize > 0)
          { base_address = cacheit->address & ((address_t)nLineSize - 1);
            base_address = base_address - (cacheit->address & ((address_t)nCPULineSize - 1));
          }
          else
          { base_address = cacheit->address & ((address_t)nLineSize - 1);
          }
#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
          cerr <<"["<<this->name()<<"("<<timestamp()<<")]: " << endl;
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
          { if(!writeBuffer.ready[i]) ok = false;
          }
          /* if all the data is ready, the prepare the head of the pipeline to reply to the cpu */
          if(ok)
          {
#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
            cerr <<"["<<this->name()<<"("<<timestamp()<<")]: Copy writeBuffer to pipeline entry ..." << endl;
    }
#endif
            /* copy the requested data */
            memcpy(cacheit->data, &(writeBuffer.buffer[base_address]), cacheit->size);
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
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
      cerr <<"["<<this->name()<<"("<<timestamp()<<")]: Warning: Not Ok !!!!! ..." << endl;
    }
	  }
#endif
        } // if (!cacheit->write)
#ifdef DD_DEBUG_DCACHE_VERB2
      else
      {
    if (DD_DEBUG_TIMESTAMP < timestamp())
    { 
	cerr <<"["<<this->name()<<"("<<timestamp()<<")]: Warning: (!cacheit->write)==False ..." << endl;
    }
      }
#endif
      } // if (cacheit)
#ifdef DD_DEBUG_DCACHE_VERB2
      else
      { 
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
      cerr <<"["<<this->name()<<"("<<timestamp()<<")]: Warning: no cacheit ..." << endl;
    }
      }
#endif
    } // if(writeBuffer.size != 0)
#ifdef DD_DEBUG_DCACHE_VERB2
    else
    { 
    if (DD_DEBUG_TIMESTAMP < timestamp())
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
  { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;
    // Update the writeBuffer if data is received from the memory system
    UpdateWriteBuffer();
    // Check if the requested data is ready without waiting that the complete line is received
    UpdateCachePipelineWithWriteBuffer();

    /* if writeBuffer has been completed write the cacheline to the cache */
    if(writeBuffer.size == nLineSize || writeBuffer.size == nLineSize)
    { /* check if the cache entry that made the request is a write
       * and use it to update the contents of the writeBuffer before
       * writing it to the cache */
      cacheit = cacheQueue.SeekAtHead();
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
#ifdef DEBUG_CACHEWB
          /* check that the addresses match */
          if((cacheit->address & (~(address_t)(nLineSize - 1))) != writeBuffer.address)
          { cerr << "Error(" << name() << "): the cache line request address does not correspond with the address in the "
            << "writebuffer" << endl;
            cerr << *this;
            exit(-1);
          }
#endif
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
          memcpy(&(writeBuffer.buffer[base_address]), cacheit->data, cacheit->size);
        }
        /* modify the state of the cache entry */

//INFO << "\e[1;33m####\e[0m cacheit->state=" << cacheit->state << " shared_bit=" << writeBuffer.shared_bit<< endl;

        switch(cacheit->state)
        { /* write cases */
          case WRITE_MISS_REQUESTED:
          case WRITE_MISS_EVICTION_WRITE_DONE_REQUESTED:
            /* write writeBuffer into the cache */
            cache.setCacheLine(cacheit->set, cacheit->line, writeBuffer.address,
                               (char *)writeBuffer.buffer,
                               true,      /* valid bit */
                               true);     /* write bit */

            /* [DAVID] set MESI properly */
            if(Snooping)
            { // It's a miss (that may have been evicted). First set the current state to invalidate, as
              // We'll replace the line.
              cache.mesi_invalidate(cacheit->set, cacheit->line);
              bool ret = cache.mesi_transition(cacheit->set, cacheit->line, MESI::transition_PrWr_BusRdX,__FILE__,__LINE__,nProg);
              if(ret)
              { ERROR << "Invalid MESI transition from " << *cacheit << endl;
                exit(1);
              }
            }
    
            /* the request can be removed */
            cacheQueue.RemoveHead();
            break;
          case READ_UNCACHABLE_READY:
            // Uncachable read is back from the memory and will be sent to the cpu. No cache line is modified.
            break;
          case WRITE_UNCACHABLE_REQUESTED:
            // Received the cache line from memory. Now ready to put the new data in the line and to send
            // back the line to the memory.
            cacheit->state = WRITE_UNCACHABLE_READY;
//            memcpy(cacheit->data, &(writeBuffer.buffer[base_address]), cacheit->size);

            memcpy(cacheit->linedata,writeBuffer.buffer,writeBuffer.size);
#ifdef DEBUG_TEST_UNCACHABLE_WRITE
//ZZZZZZZZZZZZZ
cerr << endl;
INFO << "HERE: " << *cacheit << endl;
INFO << "writeBuffer " << writeBuffer << endl;
cerr << endl;
#endif
            break;
          /* read cases */
          case READ_MISS_READY:
            /* write writeBuffer into the cache */
            cache.setCacheLine(cacheit->set, cacheit->line, writeBuffer.address,
                               (char *)writeBuffer.buffer,
                               true,                   /* valid bit */
                               false);                 /* write bit */

            // A processor read miss implied a bus read that may be shared
            if(Snooping)
            { // It's a miss (that may have been evicted). First set the current state to invalidate, as
              // We'll replace the line.
              cache.mesi_invalidate(cacheit->set, cacheit->line);
              bool ret=true;
              if(writeBuffer.shared_bit) ret = cache.mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrRd_BusRd_S,__FILE__,__LINE__,nProg);
              else                       ret = cache.mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrRd_BusRd_NS,__FILE__,__LINE__,nProg);
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
            cache.setCacheLine(cacheit->set, cacheit->line, writeBuffer.address,
                               (char *)writeBuffer.buffer,
                               true,                   /* valid bit */
                               false);                 /* write bit */

            //After the eviction is done, the new data is received. /* [DAVID] set MESI properly */
            if(Snooping)
            { // It's a miss (that may have been evicted). First set the current state to invalidate, as
              // We'll replace the line.
              cache.mesi_invalidate(cacheit->set, cacheit->line);
              bool ret=true;
              if(writeBuffer.shared_bit) ret = cache.mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrRd_BusRd_S,__FILE__,__LINE__,nProg);
              else                       ret = cache.mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrRd_BusRd_NS,__FILE__,__LINE__,nProg);
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
            cache.setCacheLine(cacheit->set, cacheit->line, writeBuffer.address,
                               (char *)writeBuffer.buffer,
                               true,                   /* valid bit */
                               false);                 /* write bit */

            /* [DAVID] set MESI properly */
            if(Snooping)
            { cache.mesi_invalidate(cacheit->set, cacheit->line);
              bool ret=true;
              if(writeBuffer.shared_bit) ret = cache.mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrRd_BusRd_S,__FILE__,__LINE__,nProg);
              else                       ret = cache.mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrRd_BusRd_NS,__FILE__,__LINE__,nProg);
              if(ret)
              { ERROR << "Invalid MESI transition from " << *cacheit << endl;
                exit(1);
              }
            }
    
            /* the request can be removed */
            cacheQueue.RemoveHead();
            break;
            /* prefetch cases */
          case PREFETCH_MISS_REQUESTED:
          case PREFETCH_MISS_EVICTION_WRITE_DONE_REQUESTED:
            /* write writeBuffer into the cache */
            cache.setCacheLine(cacheit->set, cacheit->line, writeBuffer.address,
                               (char *)writeBuffer.buffer,
                               true,      /* valid bit */
                               false);    /* write bit */

            /* [DAVID] set MESI properly 
	     * if(Snooping)
             * { if(writeBuffer.shared_bit) cache.mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrRd_BusRd_S,__FILE__,__LINE__,nProg);
             *   else                       cache.mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrRd_BusRd_NS,__FILE__,__LINE__,nProg);
	     * }
	     */

            /* the request can be removed */
            cacheQueue.RemoveHead();
            break;
          default:
            ERROR << "Don't know how to handle cacheit state " << cacheit->state << " when receiving answer from memory." << endl;
            cerr << "cacheit: " << *cacheit << endl;
            cerr << "writeBuffer " << writeBuffer << endl;
            abort();
        }
        /* reset the writeBuffer */
        writeBuffer.size = 0;
        for(int i = 0; i < nLineSize; i++) writeBuffer.ready[i] = false;
      }
    }
  }

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
  { RunCachePipeline(cacheQueue);
    if(Snooping) RunCachePipeline(cacheSnoopQueue);
  }

  /**
   * \brief Enqueue a new request to the cacheQueue from CPU side
   */
  void cacheQueue_enqueue(const memreq<INSTRUCTION, nCPUtoCacheDataPathSize> &mr)
  { CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize> *entry;
    // check that the cache queue is not full, it should never happen
    if(cacheQueue.Full())
    { ERROR << "CPU request should not have been accepted, the cache is full !" << endl;
      cerr << *this;
      //      exit(-1);
      abort();
    }

    /* create new entry in the cache pipeline and set stage and delay for each of the stages */
    entry = cacheQueue.New();
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
        accesses_write++;
        entry->state = WRITE;
        break;
      case memreq_types::cmd_READ:
        /* increment the number of read accesses */
        accesses_read++;
        entry->state = READ;
        break;
      case memreq_types::cmd_PREFETCH:
        /* increment the number of prefetch accesses */
        accesses_prefetch++;
        entry->state = PREFETCH;
        break;
      case memreq_types::cmd_EVICT:
        /* increment the number of evict accesses */
        accesses_evict++;
        entry->state = EVICT;
        break;
      case memreq_types::cmd_READX:
        accesses_read++;
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
  void cacheQueue_from_memory_enqueue(const memreq<INSTRUCTION, nMemtoCacheDataPathSize> &mr)
  { CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize> *entry;
    // check that the cache queue is not full, it should never happen
    if(cacheSnoopQueue.Full())
    { ERROR << "MEM request should not have been accepted, the cache is full !" << endl;
      cerr << "memreq: " << mr << endl;
      cerr << *this;
      exit(-1);
    }

    /* create new entry in the cache pipeline and set stage and delay for each of the stages */
    entry = cacheSnoopQueue.New();
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
        accesses_read++;
        entry->state = READ;
        break;
      case memreq_types::cmd_READX:
        accesses_read++;
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
  { /* if there is a new request from the CPU it is added to the cache pipeline */
    if(inCPU.enable)
    { memreq<INSTRUCTION, nCPUtoCacheDataPathSize> mr = inCPU.data;
#ifdef DEBUG_BUS_MQ
INFO << "Receiving from C:   " << mr << endl;
#endif
      if(this==mr.sender)
      { ERROR << "Receiving a request from myself on CPU port !\n" << mr << endl;
        exit(1);
      }
#ifdef DD_DEBUG_DCACHE
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
      cerr << "[DD_DEBUG_DCACHE] Stored   " << mr << " from CPU" << endl;
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
      if(cache.hit(mr.address)) SVG.add(timestamp(),ss1.str(),ss2.str(),"H",mr);
      else                      SVG.add(timestamp(),ss1.str(),ss2.str(),"M",mr);
#endif
      if(svg)
      { if(cache.hit(mr.address)) svg->add_cpu_to_cac(timestamp(),name(),"H",mr);
        else                      svg->add_cpu_to_cac(timestamp(),name(),"M",mr);
      }

      /* increment number of accesses to the cache */
      accesses++;
      if (!inCPU.data.something())
      {
	cerr << "[" << this->name() << "("<< timestamp() <<")"
	     << "CACHE Error: how nothing may have been enabled ???" << endl;
	abort();
      }
      cacheQueue_enqueue(mr);

    #ifdef NOC_THREADS_DISTRIBUTION
      if( ((mr.address) & 0XFF000000) == 0XFF000000)
          cout <<"CAC : receive thread request form CPU " << mr <<endl;
    #endif

    }
  }

  /**
   * \brief Read memory snooped hit requests to the cache and puts them in the pipeline 
   */
  void ReadSnoopedData()
  { if(!Snooping)
    { ERROR << "This process should not be called if Snooping is desactivated" << endl;
      exit(1);
    }
    // Check that data was received from the memory system
    if(!inMEM.enable) return;

    // Make sure the incoming data is a read to be handled by the snooper
    memreq<INSTRUCTION, nMemtoCacheDataPathSize> mr = inMEM.data;
    if(mr.message_type==memreq_types::type_ANSWER)
    { // Write are handled by UpdateWriteBuffer
      //      return;
	// DD...
	// If the answer is for another cache but if we are currently trying to answer to
	// this cache we have to discard this answer.
	QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit = cacheSnoopQueue.SeekAtHead();
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
 		    cacheSnoopQueue.RemoveHead();
		    
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
	if (requestBuffer.req_sender == mr.req_sender)
	  if (requestBuffer.address == mr.address)
	    {
	      requestBuffer.ready = false;
#ifdef DD_DEBUG_DCACHE_SNOOPING_VERB100
	   if (DD_DEBUG_TIMESTAMP <= timestamp())
	     {
	       cerr << "[Cnonfig::"<<cfg<<"][DD_DEBUG_DCACHE("<< this->name() <<")] SNOOPING Removing request from RequestBuffer !!!!" << endl;
	     }
#endif		    
	    }

    }

      if(mr.command == memreq_types::cmd_FLUSH)
      { // Flush received from memory are invalidating cache lines


        if(this==mr.req_sender)
        { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit = cacheQueue.SeekAtHead();
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
          cache.setCacheLineIndexed(cacheit->set, cacheit->line, cacheit->data,
                                    cacheit->address & ((address_t)(nLineSize-1)), cacheit->size, true);
          cache.mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrWr_BusRdX,__FILE__,__LINE__,nProg);
          cacheQueue.RemoveHead();
          return;
        }


#ifdef _CACHE_MESSAGES_SVG_H_
        stringstream ss;
        ss << "$" << nProg;
#endif

//INFO << "\e[1;35mGOT A FLUSH :\e[0m " << mr << endl;

        if(cache.hit(mr.address))
        { // A cache hit on a flush, the line should be invalidated
          int set = cache.getSet(mr.address);
          int line = cache.getLine(mr.address);
          cache.setValid(set, line, false);
          cache.mesi_transition(set, line, MESI::transition_BusRdX_Flush,__FILE__,__LINE__,nProg);    
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
        return;
      } // FLUSH

    if(this==mr.sender)
    { ERROR << "Receiving a request from myself in ReadSnoopedData : " << mr << endl;
      terminate_now() ;
    }

    // Not a flush, must be a read request answer that may be a local hit or miss
#ifdef _CACHE_MESSAGES_SVG_H_
    // Received an answer for this cache
    stringstream ss;
    ss << "$" << nProg;
    if(cache.hit(mr.address)) SVG.add(timestamp(),"BS",ss.str(),"H",mr);
    else                      SVG.add(timestamp(),"BS",ss.str(),"M",mr);
#endif
    if(svg)
    { if(cache.hit(mr.address)) svg->add_bus_to_cac(timestamp(),name(),"H",mr);
      else                      svg->add_bus_to_cac(timestamp(),name(),"M",mr);
    }

//yyy
if(inMEM_was_a_cacheQueue_hit)
{ INFO << "\e[1;34menabling: " << mr << "\e[0m" << endl;
}

    // This is a read request, discard it if it makes a miss in the local cache
    // and the request queue
    if(!inMEM_was_a_hit) return;

    // The request made a hit in the local cache
    accesses++;
    
    cacheQueue_from_memory_enqueue(mr); // Enqueue the snooped read hit request

//INFO << "Enqueued " << mr << " from MEM" << endl;
  }

  /**
   * \brief Performs the read request to the cache checking if the access was a hit or a miss and setting the entry state correctly
   */
  void SetCacheHeadReadState(QueuePointer<CachePipeStage<INSTRUCTION,nLineSize,nStages,nCPUtoCacheDataPathSize>, nStages>& cacheit, bool is_rdx)
  { if(!cacheit->cachable) 
    { // If the read request is non cachable store it as a non cachable request
      if(cache.hit(cacheit->address))
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
    cacheit->hit = cache.hit(cacheit->address);
    /* the access was a hit */
    if(cacheit->hit)
    { /* modify the history of the cache (needed by the lru and pseudo lru replacement policies */
      cache.historyAccess(cache.getSet(cacheit->address), cache.getLine(cacheit->address));
      /* increment the number of hits */
      hits++;
      hits_read++;
      /* set the state of the pipeline entry */
      cacheit->state = READ_HIT_READY;
      
      // Update the MESI state accordingly to the read hit request
      if(Snooping)
      { if(cacheit->sender_type==memreq_types::sender_MEM)
        { ERROR << "Snooped request in regular queue !" << endl;
          exit(1);
        }
        // read hit from the cpu side.
        cache.mesi_transition(cache.getSet(cacheit->address),cache.getLine(cacheit->address),MESI::transition_PrRd_XX,__FILE__,__LINE__,nProg);
      }
      
      /* copy the data from the cache to be sent to the cpu */
      if(nCPULineSize>0)
      { cache.getCacheLineIndexed(cacheit->data,
                                  cache.getSet(cacheit->address),
                                  cache.getLine(cacheit->address),
                                  (cacheit->address & (~(address_t)(nCPULineSize - 1))) & ((address_t)(nLineSize - 1)),
                                  cacheit->size);
      }
      else
      { cache.getCacheLineIndexed(cacheit->data,
                                  cache.getSet(cacheit->address),
                                  cache.getLine(cacheit->address),
                                  (cacheit->address & ((address_t)(nLineSize - 1))),
                                  cacheit->size);
      }
    }
    else
    { /* the access was a miss, increment the number of misses */
      misses++;
      misses_read++;
      /* get the set and line to place the requested line */
      cacheit->set=cache.getReplaceSet(cacheit->address);
      cacheit->line=cache.getReplaceLine(cacheit->address);
      /* modify the history of the cache (needed by the lru and pseudo lru replacement policies */
      cache.historyAccess(cacheit->set, cacheit->line);
      /* check if the line that the access is removing needs to be evicted,
       * a line is evicted if it was valid and dirty */
      if(cache.getValid(cacheit->set, cacheit->line))
      { /* the line is valid, check if the line is dirty */
        cacheit->evicted = cache.getWrite(cacheit->set, cacheit->line);
        /* the line needs to be evicted */
        if(cacheit->evicted)
        {  /* get the address of the line to be evicted */
          cacheit->evictedaddress = cache.getAddress(cacheit->set, cacheit->line);
          /* increment the number of writebacks made by the cache */
          writebacks++;
          writebacks_read++;
          /* get the data needed to be sent to the memory system  */
          cache.getCacheLine(cacheit->linedata, cacheit->set, cacheit->line);
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
  }

  /**
   * \brief Performs the write request to the cache checking if the access was a hit or a miss and setting the entry state correctly 
   */
  void SetCacheHeadWriteState(QueuePointer<CachePipeStage<INSTRUCTION,nLineSize,nStages,nCPUtoCacheDataPathSize>, nStages>& cacheit)
  { if(!cacheit->cachable) 
    { // If the read request is non cachable store it as a non cachable request
      if(cache.hit(cacheit->address))
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
    cacheit->hit = cache.hit(cacheit->address);
    /* the access was a hit */
    if(cacheit->hit)
    { /* modify the history of the cache (needed by the lru and pseudo lru replacement policies */
      cache.historyAccess(cache.getSet(cacheit->address), cache.getLine(cacheit->address));
      /* increment the number of hits */
      hits++;
      hits_write++;
      cacheit->set = cache.getSet(cacheit->address);
      cacheit->line = cache.getLine(cacheit->address);

      if(Snooping)
      { if(cacheit->sender_type==memreq_types::sender_MEM)
        { ERROR << "Snooped request in regular queue !" << endl;
          exit(1);
        }

        switch(cache.getMESI(cacheit->set,cacheit->line).getState())
        { case MESI::state_MODIFIED:
          case MESI::state_EXCLUSIVE:
            // A local write hit on M & E state does not imply any bus transaction, so the 
            // write hit request can be removed from the queue, once performed
            cache.setCacheLineIndexed(cacheit->set, cacheit->line, cacheit->data,
                                      cacheit->address & ((address_t)(nLineSize-1)), cacheit->size, true);
            cache.mesi_transition(cacheit->set,cacheit->line,MESI::transition_PrWr_XX,__FILE__,__LINE__,nProg);
            cacheQueue.RemoveHead();
#ifdef DD_DEBUG_DCACHE
    if (DD_DEBUG_TIMESTAMP < timestamp())
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
    if (DD_DEBUG_TIMESTAMP < timestamp())
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
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
	cerr << "[DD_DEBUG_DCACHE] SetCacheHeadWriteState: writting... cacheit:"
	     << *cacheit << endl; 
	//	    cerr << *this << endl;
    }
#endif
	    
	// Modify the contents of the cache with the write request data, set the cache line as dirty
        cache.setCacheLineIndexed(cache.getSet(cacheit->address), cache.getLine(cacheit->address), cacheit->data,
                                  cacheit->address & ((address_t)(nLineSize-1)), cacheit->size, true);
        // Once performed, write hit can be removed from the queue.
        cacheQueue.RemoveHead();
      }
    }
    else
    { /* the access was a miss, increment the number of misses */
      misses++;
      misses_write++;
      /* get the set and line to place the requested line */
      cacheit->set = cache.getReplaceSet(cacheit->address);
      cacheit->line = cache.getReplaceLine(cacheit->address);
      /* modify the history of the cache (needed by the lru and pseudo lru replacement policies */
      cache.historyAccess(cacheit->set, cacheit->line);
      /* check if the line that the access is removing needs to be evicted,
       * a line is evicted if it was valid and dirty */
      if(cache.getValid(cacheit->set, cacheit->line))
      { /* the line is valid, check if the line is dirty */
        cacheit->evicted = cache.getWrite(cacheit->set, cacheit->line);
        /* the line needs to be evicted */
        if(cacheit->evicted)
        { /* get the address of the line to be evicted */
          cacheit->evictedaddress = cache.getAddress(cacheit->set, cacheit->line);
          /* increment the number of writebacks made by the cache */
          writebacks++;
          writebacks_write++;
          /* get the data needed to be sent to the memory system */
          cache.getCacheLine(cacheit->linedata, cacheit->set, cacheit->line);
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
  void SetCacheHeadPrefetchState(QueuePointer<CachePipeStage<INSTRUCTION,nLineSize,nStages,nCPUtoCacheDataPathSize>, nStages>& cacheit)
  { /* check if the request hits the cache or misses */
    cacheit->hit = cache.hit(cacheit->address);
    /* the access was a hit */
    if(cacheit->hit)
    { /* modify the history of the cache (needed by the lru and pseudo lru replacement policies */
      cache.historyAccess(cache.getSet(cacheit->address), cache.getLine(cacheit->address));
      /* increment the number of hits */
      hits++;
      hits_prefetch++;
      /* the pipeline head can be removed */
      cacheQueue.RemoveHead();
    }
    else
    { /* the access was a miss, increment the number of misses */
      misses++;
      misses_prefetch++;
      /* get the set and line to place the requested line */
      cacheit->set = cache.getReplaceSet(cacheit->address);
      cacheit->line = cache.getReplaceLine(cacheit->address);
      /* modify the history of the cache (needed by the lru and pseudo lru replacement policies) */
      cache.historyAccess(cacheit->set, cacheit->line);
      /* check if the line that the access is removing needs to be evicted,
       * a line is evicted if it was valid and dirty */
      if(cache.getValid(cacheit->set, cacheit->line))
      { /* the line is valid, check if the line is dirty */
        cacheit->evicted = cache.getWrite(cacheit->set, cacheit->line);
        /* the line needs to be evicted */
        if(cacheit->evicted)
        { /* get the address of the line to be evicted */
          cacheit->evictedaddress = cache.getAddress(cacheit->set, cacheit->line);
          /* increment the number of writebacks made by the cache */
          writebacks++;
          writebacks_prefetch++;
          /* get the data needed to be sent to the memory system */
          cache.getCacheLine(cacheit->linedata, cacheit->set, cacheit->line);
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
  void SetCacheHeadEvictState(QueuePointer<CachePipeStage<INSTRUCTION,nLineSize,nStages,nCPUtoCacheDataPathSize>, nStages>& cacheit)
  { /* check if the request hits the cache or misses */
    cacheit->hit = cache.hit(cacheit->address);
    /* the access was a hit */
    if(cacheit->hit)
    { /* increment the number of hits */
      hits++;
      hits_evict++;
      /* no history access, we want to reuse the entry of the evicted line */
      /* get the set and line of the cache line to be evicted */
      cacheit->set = cache.getSet(cacheit->address);
      cacheit->line = cache.getLine(cacheit->address);
#ifdef DEBUG_CACHEWB
      if(!cache.getValid(cacheit->set, cacheit->line))
      { /* why did we get a hit if the line is not valid???? */
        cerr << "Error(" << name() << "): trying to evict a line that is not valid" << endl;
        cerr << *this;
        exit(-1);
      }
#endif
      /* the line needs to be evicted and not just removed if the line was valid and dirty */
      /* get the address of the line to be evicted */
      cacheit->evictedaddress = cache.getAddress(cacheit->set, cacheit->line);
      /* check if the line needs to be evicted */
      cacheit->evicted = cache.getWrite(cacheit->set, cacheit->line);
      /* set the removed line as not valid and not dirty */
      cache.setValid(cacheit->set, cacheit->line, false);
      cache.setWrite(cacheit->set, cacheit->line, false);
      /* if the line needs to be evicted */
      if(cacheit->evicted)
      { /* increment the number of writebacks */
        writebacks++;
        writebacks_evict++;
        /* get the data needed to be sent to the memory system */
        cache.getCacheLine(cacheit->linedata, cacheit->set, cacheit->line);
        /* set the entry state */
        cacheit->state = EVICT_READY;
      }
      else
      { /* no line needs to be evicted, the pipeline head can be removed */
        cacheQueue.RemoveHead();
      }
    }
    else
    { /* do nothing, eviction of a line that does not exist in cache */
      /* increment the number of misses */
      misses++;
      misses_evict++;
      /* remove the pipeline head */
      cacheQueue.RemoveHead();
    }
  }

  /**
   * \brief Sets the cache pipeline head state if it is on the last stage of the pipeline
   */
  void SetCacheHeadState()
  { QueuePointer<CachePipeStage<INSTRUCTION,nLineSize,nStages,nCPUtoCacheDataPathSize>, nStages> cacheit;
    cacheit = cacheQueue.SeekAtHead();    // Get the head of the pipeline
    if(cacheit)
    { /* check that the head of the pipeline is ready to be serviced (last stage, delay = 0) */
      if(cacheit->stage == nStages - 1 && cacheit->delay[nStages - 1] == 0)
      { /* set the state of the entry depending on its current state */
        switch(cacheit->state)
        { case READ:
            SetCacheHeadReadState(cacheit,false);
            break;
          case WRITE:
            SetCacheHeadWriteState(cacheit);
            break;
          case PREFETCH:
            SetCacheHeadPrefetchState(cacheit);
            break;
          case EVICT:
            SetCacheHeadEvictState(cacheit);
            break;
          case READX:
            SetCacheHeadReadState(cacheit,true);
            break;
        }
      }
    }
  }
  
  /**
   * \brief Sets the cache pipeline head state if it is on the last stage of the snoop pipeline
   */
  void SetSnoopedCacheHeadState()
  { if(!Snooping)
    { ERROR << "Should never append. only called from end_of_cycle if snooping is enabled !" << endl;
      exit(1);
    }
    QueuePointer<CachePipeStage<INSTRUCTION,nLineSize,nStages,nCPUtoCacheDataPathSize>, nStages> cacheit;
    cacheit = cacheSnoopQueue.SeekAtHead(); // Get the head of the pipeline
    if(cacheit)
    { /* check that the head of the pipeline is ready to be serviced (last stage, delay = 0) */
      if(cacheit->stage == nStages - 1 && cacheit->delay[nStages - 1] == 0)
      { /* set the state of the entry depending on its current state */
//        INFO << "\e[1;34mGot something in my snoop queue: \e[0m" << *cacheit << endl;
        switch(cacheit->state)
        { case READ:
            SetCacheSnoopHeadReadState(cacheit,false);
            break;
          case READX:
            SetCacheSnoopHeadReadState(cacheit,true);
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
  }

  /**
   * \brief Performs the read request to the cache checking if the access was a hit or a miss and setting the entry state correctly
   */
  void SetCacheSnoopHeadReadState(QueuePointer<CachePipeStage<INSTRUCTION,nLineSize,nStages,nCPUtoCacheDataPathSize>, nStages>& cacheit, bool is_rdx)
  { if(!Snooping)
    { ERROR << "Should never append. only called from end_of_cycle if snooping is enabled !" << endl;
      exit(1);
    }
    cacheit->hit = cache.hit(cacheit->address);

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
    cache.historyAccess(cache.getSet(cacheit->address), cache.getLine(cacheit->address));
    /* increment the number of hits */
    hits++;
    hits_read++;
    /* set the state of the pipeline entry */
    cacheit->state = READ_HIT_READY;
      
    // read hit from the snooping side, data is wrote back to memory. This write back will also be used as the 
    // other cache answer
    cacheit->set = cache.getSet(cacheit->address);
    cacheit->line = cache.getLine(cacheit->address);
    
    if(is_rdx)
    { // If the read request is a READX, then the local copy should be invalidated
      cache.mesi_transition(cacheit->set,cacheit->line,MESI::transition_BusRdX_Flush,__FILE__,__LINE__,nProg);
      cache.setWrite(cacheit->set, cacheit->line, false);
      cache.setValid(cacheit->set, cacheit->line, false);
    }
    else
    { // If it is a simple read request: no invalidation but the data is no more modified (if it was)
      cache.mesi_transition(cacheit->set,cacheit->line,MESI::transition_BusRd_Flush,__FILE__,__LINE__,nProg);
      cache.setWrite(cacheit->set, cacheit->line, false);
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
    cache.getCacheLine(cacheit->data, cacheit->set, cacheit->line);
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
  void SendSnoopDatatoMem()
  { if(!Snooping)
    { ERROR << "Should never append. only called from start_of_cycle if snooping is enabled !" << endl;
      exit(1);
    }
    QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;
    /* get the head of the pipeline and checks its state to see if it has data to be sent to the memory system */
    cacheit = cacheSnoopQueue.SeekAtHead();
    if(cacheit)
    { if(cacheit->sender_type==memreq_types::sender_CPU)
      { ERROR << "A request from the CPU is in the snoop queue !" << endl;
        exit(1);
      }

      switch(cacheit->state)
      { case READ_HIT_READY:
          // A snooped hit read is ready to be sent back to the MEM
          requestBuffer.command = memreq_types::cmd_WRITE; // This is a write back to memory
          requestBuffer.write = true; // Perform a write-back to memory, this WB will be snooped.
          requestBuffer.address = cacheit->address;
          requestBuffer.base    = cacheit->base;
          requestBuffer.index   = 0;
          requestBuffer.size    = cacheit->size;
          memcpy(requestBuffer.data, cacheit->data, requestBuffer.size);
  
          requestBuffer.instr = cacheit->instr;             // Instruction that generated the request
          requestBuffer.size = nLineSize;                   // A full line is being sent
          requestBuffer.req_sender = cacheit->req_sender;
          requestBuffer.uid = cacheit->uid;                 // uid of the instruction
          requestBuffer.memreq_id = cacheit->memreq_id;
          requestBuffer.message_type = memreq_types::type_ANSWER;
          requestBuffer.cacheQueueIndex = cacheSnoopQueueIndex;
          requestBuffer.ready = true;                       // The request is ready to be sent

//INFO << "SNOOPED READ HIT APPEND: 0x" << hex << cacheit->address << dec << ": ";
//print_buffer(cerr, cacheit->data, requestBuffer.size);

          return;
          break;

        case READ:
        case READX:
        { // The snooped request was a hit in the request queue, but is not a hit in the cache yet.
          // The request is currently delayed until it is a hit.
INFO << "delay..." << endl;
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
    QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;

    /* are we waiting for an accept from the memory? */
    if(requestBuffer.ready)
    { /* check if the memory accepted the request */
      if(requestBuffer.cacheQueueIndex!=cacheSnoopQueueIndex) return;
      if(outMEM.accept)
      { if(requestBuffer.write)
        { /* this request is a write */
          /* if last request was finished from the cache pipeline (a write request)
           * remove the request from the cache pipeline */
          requestBuffer.size -= nCachetoMemDataPathSize;
          if(requestBuffer.size > 0)
          { /* the request has not bben finished, the request has to be keeped,
             * the address has to be increased,
             * and the cache pipeline is not notified */
            requestBuffer.index += nCachetoMemDataPathSize;
            requestBuffer.address = requestBuffer.base + requestBuffer.index;
          }
          else
          { /* the request has been finished, the cache pipeline has to be updated */
            cacheit = cacheSnoopQueue.SeekAtHead();

//INFO << "cQI=" << requestBuffer.cacheQueueIndex << " state= " << cacheit->state << endl;

            switch(cacheit->state)
            { case READ_HIT_READY:
                // A snooped read hit has been sent and accepted, i can remove it from the queue
                cacheSnoopQueue.RemoveHead();
                break;
              default:
                ERROR << "Don't know what to do when a writing request of state " << cacheit->state << " has been sent then accepted." << endl;
                exit(1);
            }
            /* reset the request buffer */
            requestBuffer.ready=false;
          }
        }
        else // not a write
        { /* the access is a cache miss
           * the cache entry (the head of the cache queue) 
           * is not removed from the cache pipeline (this is a blocking cache) */
          /* the request buffer is marked as not ready, to be used with the next memory request */
          cacheit = cacheSnoopQueue.SeekAtHead();
          switch(cacheit->state)
          { 
            default:
              ERROR << "Don't know what to do when a non-writing request of state " << cacheit->state << " has been sent then accepted." << endl;
              exit(1);
          }
          /* reset the request buffer */
          requestBuffer.ready=false;
        }
      }
    }
    else
    {
#ifdef DEBUG_CACHEWB
      /* if an accept from the memory system that was received that was not expected, error */
      if(outMEM.accept)
      { cerr << "Error(" << name() << "): an accept was received from the memory system that was not expected" << endl;
        cerr << *this;
        exit(-1);
      }
#endif
    }
  }


  /**
   * \brief Fill the returnBuffer from the cacheQueue if its head is non-empty and ready.
   */
  void SendCacheDatatoCPU()
  { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;
    // get the head of the pipeline and check its state to see if it has data to be sent to the cpu
    cacheit = cacheQueue.SeekAtHead();
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
          returnBuffer.port = cacheit->port;
          returnBuffer.instr = cacheit->instr;
          returnBuffer.base = cacheit->base;
          returnBuffer.index = cacheit->index;
          returnBuffer.address = cacheit->address;
          returnBuffer.size = cacheit->size;
          // Copy the data to be returned
          memcpy(returnBuffer.data, cacheit->data, returnBuffer.size);
          returnBuffer.id = ID_CACHE; // Data sent from cache pipeline
          returnBuffer.uid = cacheit->uid;
          returnBuffer.memreq_id = cacheit->memreq_id;
          returnBuffer.req_sender = cacheit->req_sender;
          returnBuffer.cachable = cacheit->cachable;
          // Set returnBuffer as ready (full)
          returnBuffer.ready = true;
#ifdef DD_DEBUG_DCACHE_VERB2
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {
          cerr <<"["<<this->name()<<"("<<timestamp()<<")]: Writting ReturnBuffer: " << returnBuffer << endl;
    }
#endif
          break;
      }
    }
#ifdef DD_DEBUG_DCACHE_VERB2
    else
    {
    if (DD_DEBUG_TIMESTAMP < timestamp())
    {

      cerr <<"["<<this->name()<<"("<<timestamp()<<")]: No cache it!!! Not Writting ReturnBuffer: " << returnBuffer << endl;
    }
    }
#endif
  }

  /** 
   * \brief Sets the requestBuffer if data can be sent or requested to the memory
   */
  void SendCacheDatatoMem()
  { QueuePointer<CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUtoCacheDataPathSize>, nStages> cacheit;
    /* get the head of the pipeline and checks its state to see if it has data to be sent to the memory system */
    cacheit = cacheQueue.SeekAtHead();
    if(cacheit)
    { switch(cacheit->state)
      { /***************/
        /* write cases */
        /***************/
        case WRITE_MISS: // A line is being requested to the memory system
          requestBuffer.write = false; // Write miss first performs read
          requestBuffer.command = memreq_types::cmd_READX; // Write miss first performs a read from memory
          requestBuffer.address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
          requestBuffer.base = requestBuffer.address & (~(address_t)(nLineSize - 1));
          requestBuffer.size = nLineSize; // A full line is being sent
          requestBuffer.index = 0;
          break;
        case WRITE_MISS_EVICTION: // A line is being written to the memory system
          requestBuffer.command = memreq_types::cmd_WRITE; // A line has to be written back to the memory
          requestBuffer.write = true;
          requestBuffer.address = cacheit->evictedaddress & (~(address_t)(nLineSize - 1));
          requestBuffer.base = requestBuffer.address & (~(address_t)(nLineSize - 1));
          requestBuffer.index = 0;
          requestBuffer.size = nLineSize; // A full line is being sent
          memcpy(requestBuffer.data, cacheit->linedata, requestBuffer.size); // Copy the data to send
          break;
        case WRITE_MISS_EVICTION_WRITE_DONE: // A line is being requested to the memory system
          requestBuffer.command = memreq_types::cmd_READX; // Write miss first performs read
          requestBuffer.write = false;
          requestBuffer.address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
          requestBuffer.base = requestBuffer.address & (~(address_t)(nLineSize - 1));
          requestBuffer.size = nLineSize; // A full line is being sent
          requestBuffer.index = 0;
          break;
        case WRITE_UNCACHABLE:
        #ifdef NOC_THREADS_DISTRIBUTION
          requestBuffer.command = memreq_types::cmd_WRITE; // A line has to be written back to the memory
          requestBuffer.write = true;
          requestBuffer.address = cacheit->address;
          requestBuffer.base = requestBuffer.address;
          requestBuffer.index = 0;
          requestBuffer.size =  cacheit->size;
          memcpy(requestBuffer.data, cacheit->data, requestBuffer.size); // Copy the data to send
	  //cout<<"CAC : to request buffer uncachable write "<< requestBuffer <<endl <<endl;
        #else	  
          requestBuffer.write = false; // Write uncachable first get the line from dram
          requestBuffer.command = memreq_types::cmd_READX; // first performs a read from memory
          requestBuffer.address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
          requestBuffer.base = requestBuffer.address & (~(address_t)(nLineSize - 1));
          requestBuffer.size = nLineSize; // A full line is being sent
          requestBuffer.index = 0;
        #endif	 
          break;
        case WRITE_UNCACHABLE_READY:
          requestBuffer.command = memreq_types::cmd_WRITE;
          requestBuffer.write = true;
          requestBuffer.address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
          requestBuffer.base = requestBuffer.address & (~(address_t)(nLineSize - 1));
          requestBuffer.index = 0;
          requestBuffer.size = nLineSize; // A full line is being sent
          memcpy(requestBuffer.data, cacheit->linedata, requestBuffer.size); // Copy the data to send
//#ifdef DEBUG_TEST_UNCACHABLE_WRITE
//          INFO << "requestBuffer " << requestBuffer << endl;
//          INFO << "cacheit       " << *cacheit << endl;
//#endif
          break;
        /**************/
        /* read cases */
        /**************/
        case READ_MISS: // A line is being requested to the memory system
          requestBuffer.command = memreq_types::cmd_READ;
          requestBuffer.write = false;
          requestBuffer.address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
          requestBuffer.size = nLineSize; // A full line is being sent
          break;
        case READ_UNCACHABLE:
#ifdef DEBUG_TEST_UNCACHABLE_READ
          INFO << "read uncachable request ready" << endl;
#endif
          requestBuffer.command = memreq_types::cmd_READ;
          requestBuffer.write = false;
          requestBuffer.address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
        #ifdef NOC_THREADS_DISTRIBUTION
          requestBuffer.size =  cacheit->size;
	  requestBuffer.index = cacheit->address & ((address_t)(nCachetoMemDataPathSize - 1));
	#else
          requestBuffer.size = nLineSize; // A full line is being sent
        #endif
          break;
        case READ_MISS_EVICTION: // A line is being written to the memory system
          requestBuffer.command = memreq_types::cmd_WRITE; // First perform the eviction as a write to the memory
          requestBuffer.write = true;
          requestBuffer.address = cacheit->evictedaddress & (~(address_t)(nLineSize - 1));
          requestBuffer.base = requestBuffer.address & (~(address_t)(nLineSize - 1));
          requestBuffer.index = 0;
          requestBuffer.size = nLineSize; // A full line is being sent
          memcpy(requestBuffer.data, cacheit->linedata, requestBuffer.size); // Copy the data to send
          break;
        case READ_MISS_EVICTION_WRITE_DONE:  // A line is being requested to the memory system
          requestBuffer.command = memreq_types::cmd_READ;
          requestBuffer.write = false;
          requestBuffer.address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
          requestBuffer.size = nLineSize; // A full line is being sent
          break;
        /******************/
        /* prefetch cases */
        /******************/
        case PREFETCH_MISS:
        case PREFETCH_MISS_EVICTION_WRITE_DONE:  // A line is being requested to the memory system
          requestBuffer.command = memreq_types::cmd_READ; // Prefetch performs read
          requestBuffer.write = false;
          requestBuffer.address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
          requestBuffer.size = nLineSize; // A full line is being sent
          break;
        case PREFETCH_MISS_EVICTION: // A line is being written to the memory system
          requestBuffer.command = memreq_types::cmd_WRITE; // First perform the eviction as a write
          requestBuffer.write = true;
          requestBuffer.address = cacheit->evictedaddress & (~(address_t)(nLineSize - 1));
          requestBuffer.base = requestBuffer.address & (~(address_t)(nLineSize - 1));
          requestBuffer.index = 0;
          requestBuffer.size = nLineSize; // A full line is being sent
          memcpy(requestBuffer.data, cacheit->linedata, requestBuffer.size); // Copy the data to send
          break;
        /***************/
        /* evict cases */
        /***************/
        case EVICT_READY: // A line is being written to the memory system
          requestBuffer.command = memreq_types::cmd_WRITE; // Evictions are forced write
          requestBuffer.write = true;
          requestBuffer.address = cacheit->evictedaddress & (~(address_t)(nLineSize - 1));
          requestBuffer.base = requestBuffer.address & (~(address_t)(nLineSize - 1));
          requestBuffer.index = 0;
          requestBuffer.size = nLineSize; // A full line is being sent
          memcpy(requestBuffer.data, cacheit->linedata, requestBuffer.size); // Copy the data to send
          break;

        /*****************/
        /* snooped cases */
        /*****************/
        case WRITE_HIT_FLUSH:
          // Local write hit on shared data => Flush
          requestBuffer.command = memreq_types::cmd_FLUSH;
          requestBuffer.write = false; // Flush does not perform a memory write but some cache invalidations
          requestBuffer.address = cacheit->address & (~(address_t)(nCachetoMemDataPathSize - 1));
          requestBuffer.base = requestBuffer.address & (~(address_t)(nLineSize - 1));
          requestBuffer.index = 0;
          requestBuffer.size = nLineSize; // A full line is being sent
          break;

        default:
          // Nothing is ready to be put in request Buffer.
          return;
      }

      // Fill the remaining fields of requestBuffer
      requestBuffer.instr = cacheit->instr;             // Instruction that generated the request
    #ifdef NOC_THREADS_DISTRIBUTION
      requestBuffer.dst_id = cacheit->dst_id;                   // A full line is being sent
    #endif      
      requestBuffer.req_sender = cacheit->req_sender;
      requestBuffer.uid = cacheit->uid;                 // uid of the instruction
      requestBuffer.memreq_id = cacheit->memreq_id;
      requestBuffer.message_type = memreq_types::type_REQUEST;
      requestBuffer.cachable = cacheit->cachable;
      requestBuffer.cacheQueueIndex = cacheQueueIndex;
      requestBuffer.ready = true;                       // The request is ready to be sent
    }
  }

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
 
  bool inMEM_was_a_hit;
  bool inMEM_was_a_cacheQueue_hit;
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
    cacheit = cacheQueue[0].SeekAtHead();
    cerr << " - cacheQueue.head = ";
    if(cacheit) cerr << *cacheit << endl;
    else cerr << "<none>" << endl;
    cacheit = cacheSnoopQueue.SeekAtHead();
    cerr << " - cacheQueue.head = ";
    if(cacheit) cerr << *cacheit << endl;
    else cerr << "<none>" << endl;    
  }

// Taj- functions needed by power service
public:
  virtual long long int GetReadAccessCount () { return accesses_read; }     ///< Power estimation interface : Retunrs the number of read access so far
  virtual long long int GetWriteAccessCount () { return  accesses_write; }  ///< Power estimation interface : Retunrs the number of write access so far
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
  bool has_pending_ops()
  { if(!cacheQueue[0].Empty())
    { //cout <<"CACHEWB: cacheQueue not empty "<<endl;
      return true;
    }
    if(!cacheSnoopQueue.Empty() )
    { //cout <<"CACHEWB: cacheSnoopQueue not empty "<<endl;
      return true;
    }
    return false;
  }

 int get_rank()
 { return inCPU[0].get_connected_module()->get_rank()+1;
 }

};

} // end of namespace cache
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif //__CACHEWB_H__
