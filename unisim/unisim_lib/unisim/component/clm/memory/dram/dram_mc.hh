/* *****************************************************************************
                     dram.sim: dram cycle level module
***************************************************************************** */

/* *****************************************************************************  
                                    BSD LICENSE  
********************************************************************************  
Copyright (c) 2006, Commissariat a l'Energie Atomique (CEA) & INRIA
Authors: Gilles Mouchard (gilles.mouchard@cea.fr) 
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

#ifndef __UNISIM_COMPONENT_CLM_MEMORY_DRAM_DRAM_MC_HH__
#define __UNISIM_COMPONENT_CLM_MEMORY_DRAM_DRAM_MC_HH__

#include <unisim/kernel/clm/engine/unisim.h>
#include <unisim/component/clm/utility/common.hh>
#include <unisim/component/clm/memory/mem_common.hh>
//#include <unisim/component/clm/utility/error.h>
#include <unisim/component/clm/utility/utility.hh>
#include <unisim/component/clm/memory/memory_container.hh>
#include <unisim/component/clm/memory/dram/dram_components.hh>

// MEM Emu ...
// TODOTOMERGE
//#include <generic/memory/MyMemEmulator.hh>
#include <unisim/component/cxx/memory/ram/memory.hh>

#include <unisim/component/clm/interfaces/memreq_mc.hh>
//#include <generic/memory/memory_interface.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>

#include <unisim/component/clm/debug/svg_memreq_interface.hh>
#include <unisim/kernel/kernel.hh>

//#include "statistics/StatisticService.h"

#include <boost/array.hpp>
#include <boost/detail/iterator.hpp>

namespace unisim {
namespace component {
namespace clm {
namespace memory {
namespace dram {

  using std::iterator;

// new usings
using unisim::component::clm::interfaces::memreq;


using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;


using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;

using unisim::component::clm::debug::SVGmemreqInterface;

  //using unisim::component::clm::interface::memreq_types;
  //not now  using unisim::component::cxx::memor

  //using full_system::generic::memory::MyMemEmulator;

// For the MemoryInterface : 
/*
using full_system::utils::services::ServiceExport;
using full_system::utils::services::ServiceImport;
using full_system::utils::services::Service;
using full_system::utils::services::Client;

using full_system::generic::memory::MemoryInterface;
using full_system::plugins::SVGmemreqInterface;
*/


typedef ServiceExport<Memory<address_t> > MI_ServiceExport;
typedef Service<Memory<address_t> > MI_Service;


typedef ServiceExport<MemoryInjection<address_t> > MJI_ServiceExport;
typedef Service<MemoryInjection<address_t> > MJI_Service;

/**
 * \brief A DRAM memory model including a basic memory controler (SDRAM or DDR-SDRAM) 
 *
 * Note: The total memory size is : nBanks * nRows * nCols * nDataPathSize
 */
template
< class INSTRUCTION,
  int nBanks,             // number of banks (typically 2 or 4)
  int nRows,              // number of rows (typically 2048, 4096, or 8192)
  int nCols,              // number of columns (typically 256, 512 or 1024)
  int TRRD,               // Delay Time, ACTV command in one bank to ACTV command in another bank
  int TRAS,               // Delay Time, ACTV command to PRE or PREALL
  int TRCD,               // Delay Time, ACTV command to READ, READP, WRT or WRTP command (for READP and WRTP mTRCD <= mTRAS)
  int CL,                 // Delay Time, CAS Latency, time to wait before Data Out
  int TRP,                // Delay Time, PRE or PREALL command to ACTV, MRS, REFR, or SLFR command
  int TRC,                // Delay Time, ACTV, REFR or SLFR command to ACTV, MRS, REFR, or LSFR command
  uint64_t TREF,          // Delay Time, Refresh Interval
  int nDataPathSize,      // Data Bus width in Bytes (typically 8)
  int nCacheLineSize,     // Cache Line Size, i.e. Burst size in bytes
  int nCtrlQueueSize,
  int nProg = 0,
  bool Snooping = false,
  bool VERBOSE = false,
  int nConfig = 1
  //  const int nConfig = 1
  //  std::size_t nConfig = 1
>
class DRAM : public module
	     , public MI_Service
	     , public Service<MemoryInjection<address_t> >
	     , public Client<SVGmemreqInterface<INSTRUCTION> >
//           , public StatisticService
{public:
  MI_ServiceExport memory_export;                                 ///< Service port from CPU
  MJI_ServiceExport memory_injection_export;                                 ///< Service port from CPU
  ServiceImport<SVGmemreqInterface<INSTRUCTION> > svg;                           ///< Service port to SVG builder 

  /* clock */
  inclock inClock;                                                 ///< clock port
  /* ports */
  //  inport < memreq < INSTRUCTION, nDataPathSize >[nConfig] > in;             ///< Input port for memory requests
  inport < memreq < INSTRUCTION, nDataPathSize >, nConfig > in;             ///< Input port for memory requests
  //  outport < memreq < INSTRUCTION, nDataPathSize >[nConfig] > out;           ///< Output port for sending data toward CPU
  outport < memreq < INSTRUCTION, nDataPathSize > ,nConfig > out;           ///< Output port for sending data toward CPU

  // DD : template < bool, nConfig, false > doesn't exist yet ...
  //  inport < bool, nConfig , Snooping > inShared;             ///< Inport port for the shared bit of some CMP models
  inport < bool, Snooping > inShared;             ///< Inport port for the shared bit of some CMP models
 //  inport < bool, Snooping > inDirty;

  /** The constructor
  		@param name the module name
  */
  DRAM(const char *name) :
    module(name),
    Object(name, 0),
    MJI_Service(name, 0),
    MI_Service(name, 0),
    //    syscall_MemExp("syscall_MemExp", this),
    memory_export("memory_export", this),
    memory_injection_export("memory_injection_export", this),
    Client<SVGmemreqInterface<INSTRUCTION> >(name, this),
			 svg("svg", this)//,
    //    StatisticService(name, this)
  { // Interface naming to enhance debugging
    class_name = " DRAM";
    category = category_MEMORY;
    in.set_unisim_name(this,"in");
    inShared.set_unisim_name(this,"inShared");
    out.set_unisim_name(this,"out");
    // Set sensitivity lists
    sensitive_pos_method(start_of_cycle) << inClock;
    sensitive_neg_method(end_of_cycle) << inClock;
    if(Snooping)
    { 
      sensitive_method(onInData) << in.data << inShared.data;
    }
    else 
    { sensitive_method(onInData) << in.data;
    }
    sensitive_method(onOutAccept) << out.accept;

    /* Setting global memory pointer */
    //    global_memory = &container;
    /* Internal state */
    for (int i=0; i < nConfig; i++)
      {
	time_stamp[i] = 0;
	refresh_counter[i] = TREF;
	row_refresh[i] = 0;
	refreshing[i] = false;
	dataBusOwner[i] = 0;
	lastBank[i] = -1;
	
	/* Statistics */
	total_precharge[i] = 0;
	total_activate[i] = 0;
	total_read[i] = 0;
	total_write[i] = 0;
	total_refresh[i] = 0;
	num_read_requests[i] = 0;
	cumulative_load_time[i] = 0;
	reordered_requests[i] = 0;
	min_load_time[i] = 10000000000LL;
	max_load_time[i] = 0;
	average_latency[i] = 0;
      }
    //    memory_emulator = new MyMemEmulator("memory_emulator", this);
    memory_emulator = new unisim::component::cxx::memory::ram::Memory<address_t>("memory_emulator", this);

    // ---  Registring parameters ----------------
    parameters.add("nBanks",nBanks,2,4);
    parameters.add("nRows",nRows,2048,8192);
    parameters.add("nCols",nCols,256,1024);
    parameters.add("TRRD",TRRD,2,2);
    parameters.add("TRAS",TRAS,5,5);
    parameters.add("TRCD",TRCD,2,2);
    parameters.add("CL",CL,2,2);
    parameters.add("TRP",TRP,2,2);
    parameters.add("TRC",TRC,7,7);
    parameters.add("TREF",TREF,UINT64_C(10000000),UINT64_C(10000000));
    parameters.add("nDataPathSize",nDataPathSize);
    parameters.add("nCacheLineSize",nCacheLineSize);
    parameters.add("nCtrlQueueSize",nCtrlQueueSize,8,32);
    parameters.add("nProg",nProg);
    parameters.add("Snooping",Snooping);
    parameters.add("VERBOSE",VERBOSE);
    // ---  Registring statistics ----------------
    /*
    statistics.add("average_latency",average_latency);
    statistics.add("total_precharge",total_precharge);
    statistics.add("total_activate",total_activate);
    statistics.add("total_read",total_read);
    statistics.add("total_write",total_write);
    statistics.add("total_refresh",total_refresh);
    statistics.add("num_read_requests",num_read_requests);
    statistics.add("cumulative_load_time",cumulative_load_time);
    statistics.add("reordered_requests",reordered_requests);
    statistics.add("min_load_time",min_load_time);
    statistics.add("max_load_time",max_load_time);
    */
    // -------------------------------------------

#ifdef DUMP_DRAM_IN_REQUESTS
      ofstream os("dump_dram_in.log",ios_base::trunc);
      os.close();
#endif
  }

  /**
   * \brief Service interface : initialization
   */
  virtual bool ClientIndependentSetup()
  { if(svg) svg->register_mem(name());
    return true;
  }

  /**
   * \brief Memory interface : reset
   */
  void Reset()
  {
    // We don't need to reset DRAM for the moment ...
  }

  /**
   * \brief Memory interface : flush
   */
  bool FlushMemory(address_t addr, uint32_t size)
  { ERROR << "DRAM Can't be flushed !" << endl;
    exit(1);
  }

  bool ReadMemory(address_t addr, void *buffer, uint32_t size)
  {
    return InjectReadMemory(addr, buffer, size);
  }
  bool WriteMemory(address_t addr, const void *buffer, uint32_t size)
  {
    return InjectWriteMemory(addr, buffer, size);
  }

  /**
   * \brief Memory interface : read from memory
   */
  //  bool ReadMemory(address_t addr, void *buffer, uint32_t size)
  bool InjectReadMemory(address_t addr, void *buffer, uint32_t size)
  {
#ifdef DD_DEBUG_SYSCALL
    cerr << "[DD_DEBUG_SYSCALL]: in DRAM ... ReadMemory() ..." << endl;
#endif
#ifdef SYSCALL_DISPATCH_WITHOUT_MIB
    memory_emulator->ReadMemory(addr,buffer,size);
#endif
    Read(buffer,addr,size);
#ifdef DD_DEBUG_SYSCALL_VERB1
    cerr << "["<<this->name()<<"("<<timestamp()<<")]: ReadMemory(): "<< endl;
    cerr << "           addr   : "<<hexa(addr)<<endl;
    cerr << "           size   : "<<size<<endl;
    char tmpbuff[40];
    memcpy(tmpbuff,buffer,40);
    cerr << "           buffer : "<<tmpbuff<<endl;
#endif

    return true;
  }

  /**
   * \brief Memory interface : write to memory
   */
  bool InjectWriteMemory(address_t addr, const void *buffer, uint32_t size)
  {
/*
#ifdef DEBUG_CACHE_WRITEMEMORY
  if(timestamp()>0)
  { cerr << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
    cerr << "\e[1;33m" << timestamp() << ": " << name() << ": "<< "WriteMemory \e[0m Ox" << hex << addr << dec << ", " << size << endl;
  }
#endif
*/
#ifdef DD_DEBUG_SYSCALL
    cerr << "[DD_DEBUG_SYSCALL]: in DRAM ... WriteMemory() ..." << endl;
#endif
#ifdef DD_DEBUG_SYSCALL_VERB1
    cerr << "["<<this->name()<<"("<<timestamp()<<")]: WriteMemory(): "<< endl;
    cerr << "           addr   : "<<hexa(addr)<<endl;
    cerr << "           size   : "<<size<<endl;
    char tmpbuff[40];
    memcpy(tmpbuff,buffer,40);
    cerr << "           buffer : "<<tmpbuff<<endl;
#endif
    Write(addr,buffer,size);
    memory_emulator->WriteMemory(addr, buffer, size);//Taj - write in both memories(Sim+Emu). read, however, is done on one.

#ifdef SYSCALL_DISPATCH_WITHOUT_MIB
    memory_emulator->WriteMemory(addr,buffer,size);
#ifdef DD_DEBUG_SYSCALL
    cerr << "[DD_DEBUG_SYSCALL]: in DRAM ... WriteMemory() into memory emulator..." << endl;
    cerr << "["<<this->name()<<"("<<timestamp()<<")]: WriteMemory(): "<< endl;
    cerr << "           addr   : "<<hexa(addr)<<endl;
    cerr << "           size   : "<<size<<endl;
    char tmpbuff2[40];
    memcpy(tmpbuff2,buffer,40);
    cerr << "           buffer : "<<tmpbuff2<<endl;    
#endif
#endif
#ifdef TMP_DRAM_DUMP_WM
INFO << "WriteMemory 0x" << hex << addr << dec << "," << size << endl;
#endif
    return true;
  }

  /**
   * \brief Activate a bank (open a page)
   * \param bank the bank number
   * \param row the row number
   * \param entry Controler queue entry
   * \return true if activation was performed
   */
  bool Activate(int bank, int row, DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize> *entry)
  { /* Checks if bank has the good state */
    /* Is the bank idle ? */
    if(banks[bank].state != IdleState)
      return false;
    /* Checks if timing constraints are met */
    /*    ACTV <---------- TRRD ----------> ACTV (of a different bank) ? */
    if(bank != lastBank && time_stamp - banks[bank].lastACTV < TRRD)
      return false;
    /*    ACTV <----------- TRC ----------> ACTV ? */
    if(time_stamp - banks[bank].lastACTV < TRC)
      return false;
    /*     PRE <----------- TRP ----------> ACTV ? */
    if(time_stamp - banks[bank].lastPRE < TRP)
      return false;
    /*  PREALL <----------- TRP ----------> ACTV ? */
    if(time_stamp - banks[bank].lastPREALL < TRP)
      return false;
    total_activate++;
    banks[bank].lastACTV = time_stamp;
    banks[bank].row = row;
    banks[bank].state = RowActiveState;
    banks[bank].entry = entry;
    banks[bank].total_activate++;
#ifdef DEBUG_DRAM
    cerr << timestamp() << ":B#" << bank << ":row#" << row << ":ACTV:" << *entry << endl;
#endif
    return true;
  }

  /**
   * \brief Precharge a bank (close a page)
   * \param bank the bank number
   * \param entry Controler queue entry
   * \return true if precharge was performed
   */
  bool Precharge(int bank, DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize> *entry)
  { /* Checks if bank has the good state */
    /* Was the bank activated ? */
    if(banks[bank].state != RowActiveState)
      return false;
    /* Checks if the timing constraints are met */
    /* ACTV   <---------- TRAS ----------> PRE ?*/
    if(time_stamp - banks[bank].lastACTV < TRAS)
      return false;
    total_precharge++;
    banks[bank].lastPRE = time_stamp;
    banks[bank].state = IdleState;
    banks[bank].entry = entry;
    banks[bank].total_precharge++;
#ifdef DEBUG_DRAM
    cerr << timestamp() << ":B#" << bank << ":PRE:" << *entry << endl;
#endif
    return true;
  }

  /** Precharge all banks (close all pages)
  		@return true if precharge was performed
  */
  bool PrechargeAll()
  { int bank;
    /* For each bank */
    for(bank = 0; bank < nBanks; bank++)
    { /* Checks if timing constraints are met */
      /* ACTV   <---------- TRAS ----------> PREALL ? */
      if(time_stamp - banks[bank].lastACTV < TRAS)
      { return false;
      }
    }

    /* For each bank */
    for(bank = 0; bank < nBanks; bank++)
    { /* Precharge the bank */
      total_precharge++;
      banks[bank].lastPREALL = time_stamp;
      banks[bank].state = IdleState;
      banks[bank].total_precharge++;
#ifdef DEBUG_DRAM
      cerr << timestamp() << ":B#" << bank << ":PRE" << endl;
#endif
    }
    return true;
  }

  /** 
   * \brief Do a read command
   * \param bank the bank number
   * \param col the column number
   * \param entry Controler queue entry
   * \return true if read command was performed
   */
  bool Read(int bank, int col, DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize> *entry)
  {
    /* Checks if bank has the good state */
    /* Was the row activated ? */
    if(banks[bank].state != RowActiveState)
      return false;

    /* Checks if the timing constraints are met */
    /* ACTV   <---------- TRCD ----------> READ ? */
    if(time_stamp - banks[bank].lastACTV < TRCD)
      return false;
    total_read++;
    banks[bank].lastREAD = time_stamp;
    banks[bank].col = col;
    banks[bank].burst_counter = 0;
    banks[bank].state = ReadState;
    banks[bank].entry = entry;
    banks[bank].total_read++;
#ifdef DEBUG_DRAM
    cerr << timestamp() << ":B#" << bank << ":col#" << col << ":READ:" << *entry << endl;
#endif
    return true;
  }

  /**
   * \brief Do a write command
   * \param bank the bank number
   * \param col the column number
   * \param entry Controler queue entry
   * \return true if write command was performed
   */
  bool Write(int bank, int col, DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize> *entry)
  {
    /* Checks if bank has the good state */
    /* Was the row activated ? */
    if(banks[bank].state != RowActiveState)
      return false;
    /* Checks if the timing constraints are met */
    /* ACTV   <---------- TRCD ----------> WRT ? */
    if(time_stamp - banks[bank].lastACTV < TRCD)
      return false;
    total_write++;
    banks[bank].lastWRT = time_stamp;
    banks[bank].col = col;
    banks[bank].burst_counter = 0;
    banks[bank].state = WriteState;
    banks[bank].entry = entry;
    banks[bank].total_read++;
#ifdef DEBUG_DRAM
    cerr << timestamp() << ":B#" << bank << ":col#" << col << ":WRT:" << *entry  << endl;
#endif
    return true;
  }

  /**
   * \brief Do a read data transfer from the DRAM
   * \param bank the bank number
   * \param entry Controler queue entry
   * \return true if performed
   */
  bool ReadData(int bank, DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize> *entry)
  { /* Checks if bank has the good state */
    /* Is the bank in read state ? */
    if(banks[bank].state != ReadState)
    { return false;
    }
    if(banks[bank].entry != entry)
    { return false;
    }

    /* Is the burst finished ? */
    if(banks[bank].burst_counter == nCacheLineSize / nDataPathSize)
    { cerr << "Read Burst is already finished" << endl;
      exit(-1);
    }

    /* Checks if timing constraints are met */
    /* READ <---------- CL ----------> Data out ? */
    if(time_stamp - banks[bank].lastREAD < CL)
    { return false;
    }

    /* Increment the burst counter */
    if(++banks[bank].burst_counter == nCacheLineSize / nDataPathSize)
    {
      /* If burst is finished, left the bank activated */
      banks[bank].state = RowActiveState;
    }
    banks[bank].entry = entry;
#ifdef DEBUG_DRAM
    cerr << timestamp() << ":B#" << bank << ":RDDATA:" << *entry  << endl;
#endif
    return true;
  }

  /**
   * \brief Do a write data transfer from the DRAM
   * \param bank the bank number
   * \param entry Controler queue entry
   * \return true if performed
   */
  bool WriteData(int bank, DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize> *entry)
  {
    /* Checks if bank has the good state */
    /* Is the bank in write state ? */
    if(banks[bank].state != WriteState)
      return false;
    if(banks[bank].entry != entry)
      return false;

    /* Is the burst finished ? */
    if(banks[bank].burst_counter == nCacheLineSize / nDataPathSize)
    { cerr << "Write Burst is already finished" << endl;
      exit(-1);
    }

    /* Increment the burst counter */
    if(++banks[bank].burst_counter == nCacheLineSize / nDataPathSize)
    {
      /* If burst is finished, left the bank activated */
      banks[bank].state = RowActiveState;
    }
    banks[bank].entry = entry;
#ifdef DEBUG_DRAM
    cerr << timestamp() << ":B#" << bank << ":WRDATA:" << *entry  << endl;
#endif
    return true;
  }

  /**
   * \brief Performs a refresh of a row on all banks
   * \param row the row number
   * \return true if refresh was performed
   */
  bool Refresh(int row)
  { int bank;
    total_refresh++;
    /* For each bank */
    for(bank = 0; bank < nBanks; bank++)
    {
      /* Checks if timing constraints are met */

      /* REFR   <----------- TRC ----------> REFR ? */
      if(time_stamp - banks[bank].lastREFR < TRC)
        return false;
      /* ACTV   <----------- TRC ----------> REFR ? */
      if(time_stamp - banks[bank].lastACTV < TRC)
        return false;
      /* PRE <----------- TRP ----------> REFR ? */
      if(time_stamp - banks[bank].lastPRE < TRP)
        return false;
      /* PREALL <----------- TRP ----------> REFR ? */
      if(time_stamp - banks[bank].lastPREALL < TRP)
        return false;
    }

    /* For each bank */
    for(bank = 0; bank < nBanks; bank++)
    {
      /* Perform a refresh */
      banks[bank].lastREFR = time_stamp;
    }
#ifdef DEBUG_DRAM
    cerr << timestamp() << ":row#" << row << ":REFR" << endl;
#endif
    return true;
  }

  /**
   * \brief Performs data transfer between memory controler and the DRAM
   * \return true if performed
   */
  bool DRAMDataTransfer()
  { OoOQueuePointer<DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize>, nCtrlQueueSize> entry;

    /* For each entry in the controler queue */
    entry = queue.SeekAtHead();
    if(entry)
    { do
      { 

#ifdef DD_DEBUG_DRAM_DATATRANS_IN
	cerr << "[DD_DEBUG_DRAM_DATATRANS_IN] Dump in DRAMDataTransfer() : " << endl;
	cerr << "    Entry : " << *entry << endl;
	dump(cerr);
#endif


	if(!entry->dram_data_transfer_finished)
        { switch(banks[entry->bank].state)
          { case ReadState:
              /* bank is in read state */
              if(ReadData(entry->bank, entry))
              { if(entry->write)
                { cerr << "it is a store" << endl;
                  cerr << "time stamp = " << timestamp() << endl;
                  cerr << queue << endl;
                  exit(-1);
                }
                // Read(entry->data + entry->write_offset, entry->base_address + entry->write_offset, nDataPathSize);
                Read(entry->data, entry->base_address + entry->write_offset, nDataPathSize);
                entry->size += nDataPathSize;
                entry->write_offset += nDataPathSize;
                if(entry->write_offset >= nCacheLineSize)
                  entry->write_offset = 0;
                if(entry->size == nCacheLineSize)
                {
                  entry->dram_data_transfer_finished = true;
                }
#ifdef DD_DEBUG_DRAM_DATATRANS_IN
		cerr << "[DD_DEBUG_DRAM_DATATRANS_IN] Dump in DRAMDataTransfer() : after READSTATE " << endl;
		cerr << "    Entry : " << *entry << endl;
	dump(cerr);
#endif
                return false;
              }
              break;

            case WriteState:
              /* bank is in write state */
              /* If all bytes were not already written back to DRAM ? */
              if(entry->read < entry->size/*nCacheLineSize*/)
              {
                /* Write the data */
                if(WriteData(entry->bank, entry))
                {
                  Write(entry->base_address/*+ entry->read_offset*/, entry->data + entry->read_offset, nDataPathSize);
                  entry->read += nDataPathSize;
                  entry->read_offset += nDataPathSize;
                  if(entry->read_offset >= nCacheLineSize)
                    entry->read_offset = 0;
                  if(entry->read == nCacheLineSize)
                  {
                    entry->dram_data_transfer_finished = true;
                  }
                  return true;
                }
              }
              break;

            default:
#ifdef DD_DEBUG_DRAM_DATATRANS_IN
	cerr << "[DD_DEBUG_DRAM_DATATRANS_IN] Dump in DRAMDataTransfer() : after DEFAULT" << endl;
	cerr << "    Entry : " << *entry << endl;
	dump(cerr);
#endif
              return false; /* do nothing */
          }
        }
        entry++;
      }
      while(entry);
    }
    return false;
  }

  /** Controls the DRAM banks */
  void DRAMControl()
  { OoOQueuePointer<DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize>, nCtrlQueueSize> entry;

    /* For each entry in the memory controler queue */
    entry = queue.SeekAtHead();
    if(entry)
    { do
      { if(!entry->dram_data_transfer_finished)
        { switch(banks[entry->bank].state)
          { case IdleState:
              /* if bank is idle then activate it */
              if(Activate(entry->bank, entry->row, entry))
              { entry->launched = true;
                return;
              }
              break;

            case RowActiveState:
              /* If a row is already opened then close it */
              if(banks[entry->bank].row != entry->row)
              { /* Do a precharge */
                if(Precharge(entry->bank, entry))
                { entry->launched = true;
                  return;
                }
              }
              else
              { /* If it is a write then initiate a write command to the DRAM */
                if(entry->write)
                { if(Write(entry->bank, entry->col, entry))
                  { entry->launched = true;
                    return;
                  }
                }
                else
                { /* If is a read then initiate a read command to the DRAM */
                  if(Read(entry->bank, entry->col, entry))
                  { entry->launched = true;
                    return;
                  }
                }
              }
              break;

            default:
              ; /* do nothing */
          }
        }
        entry++;
      }
      while(entry);
    }
  }

  /**
   * \brief Perform DRAM Queue pipeline
   */
  void ScheduleDRAMQueue()
  { DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize> *new_entry, act_entry;
    OoOQueuePointer<DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize>, nCtrlQueueSize> act, entry;
    bool moved;

    act = queue.SeekAtTail();
    act_entry = *act;
    queue.Remove(act);
    entry = queue.SeekAtTail();
    moved = false;
    while(entry)
    { if(entry->bank == act_entry.bank)
      { if(entry->row == act_entry.row)
        { new_entry = queue.NewAfter(entry);
          *new_entry = act_entry;
          if(moved) reordered_requests++;
          return;
        }
        else
        { if(entry->launched)
          { new_entry = queue.New();
            *new_entry = act_entry;
            return;
          }
          entry--;
          moved = true;
        }
      }
      else
      { if(entry->launched)
        { new_entry = queue.New();
          *new_entry = act_entry;
          return;
        }
        entry--;
        moved = true;
      }
    }
    new_entry = queue.New();
    *new_entry = act_entry;
  }

  /** A SystemC process for managing the valid, accept and enable hand-shaking */
  void onInData()
  { if(!in.data.known()) return;
    if(Snooping)
    { if(!inShared.data.known()) return;
      inShared.accept = inShared.data.something();
    }
    if(refresh_counter == 0)
    { /* We're refreshing the DRAM, so don't accept any request */
      in.accept = false;
    }
    else
    { if(in.data.something())
      { memreq <INSTRUCTION, nDataPathSize> mr = in.data;

//INFO << mr << endl;

#ifdef DD_DEBUG_DRAM
	  cerr << "[DD_DEBUG_DRAM] DRAM receving a new request from : " <<  mr.req_sender << endl;
	  cerr << "[DD_DEBUG_DRAM] DRAM receving a new request      : " <<  mr << endl;
	  dump(cerr);
#endif

     #ifdef NOC_THREADS_DISTRIBUTION
       //if( (mr.address & 0XFF000000) == 0XFF000000)
       if( (mr.dst_id != 0) || (mr.sender_type == memreq_types::sender_MEM) )
       {
          in.accept = true;
          return;
       }
     #endif 
	
       if(mr.command == memreq_types::cmd_FLUSH)
        { // The memory has nothing to do with cache flush request.
          // Just accept it, and ignore it when it is enabled.
          in.accept = true;
          return;
        }

        if(Snooping)
        { if(inShared.data==true)
          { // If a data is coming with shared bit set, accept it as it will
            // not be stored nor replied by the memory, but by one of the caches.
            in.accept=true;
//INFO << "2Discard " << mr << endl;
            return;
          }
        }
      
//INFO << "Received " << mr << endl;
        /* Get the latest entry of DRAM controler queue */
        DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize> *entry = queue.GetTail();
        if(entry && entry->write && entry->size < nCacheLineSize)
        { /* If the latest entry is a write which has not received all its data, then accept an incoming write request */
          /* We do that because a burst write correspond to several consecutive write requests */
          in.accept = (mr.command == memreq_types::cmd_WRITE);
        }
        else
        { OoOQueuePointer<DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize>, nCtrlQueueSize> entry;
          int to_remove = 0;

          entry = queue.SeekAtHead();
          if(entry)
          { do
            { if(entry->write)
              { if(entry->read >= nCacheLineSize)
                { to_remove++;
                }
                else
                { break;
                }
              }
              else
              { if(entry->read >= nCacheLineSize - nDataPathSize && out.accept)
                { to_remove++;
                }
                else
                { break;
                }
              }
              entry++;
            }
            while(entry);
          }

          int freeSpace = queue.FreeSpace() + to_remove;

	  // David Debug
#ifdef DD_DEBUG_DRAM
	    cerr << "[DD_DEBUG_DRAM] DRAM Free Space : " <<  freeSpace << endl;
#endif
	  
          if(mr.command == memreq_types::cmd_WRITE)
          { if(dataBusOwner)
            { in.accept = false;
            }
            else
            { in.accept = freeSpace > 0;
            }
          }
          else
          { 
	    // David Debug
#ifdef DD_DEBUG_DRAM
	    //cerr << "[DD_DEBUG_DRAM] DRAM in the last else : FreeSpace=" <<  freeSpace << endl;
#endif

	    //	    in.accept = static_cast<bool> (freeSpace > 0);
	    in.accept = (freeSpace > 0);
	    // David Debug
#ifdef DD_DEBUG_DRAM
	      //cerr << "[DD_DEBUG_DRAM] DRAM : (FreeSpace>0)=" <<  (freeSpace>0) << endl;
#endif
          }
        }
      }
      else
      { in.accept = false;
      }
    }
#ifdef DD_DEBUG_DRAM
    // The following line is commented because in.accept is uninitialized (as said valgrind)
    //    cerr << "[DD_DEBUG_DRAM] DRAM at the end : in.accept = " <<  in.accept << endl;
#endif

  }
  
  /**
   * \brief Process lauched upon receiving an accept for the output port
   */
  void onOutAccept()
  { out.enable = out.accept;
  }

  /**
   * \brief Process launched at the beggining of the cycle
   */
  void start_of_cycle()
  { 

#ifdef DD_DEBUG_DRAM
    cerr << "[DD_DEBUG_DRAM] DRAM : ------------------------------------------ Start of cycle (" << timestamp() << ")"<< endl;
    cerr << "[DD_DEBUG_DRAM] DRAM dump() : " << endl;
    dump(cerr);
#endif

    //    boost::array< memreq <INSTRUCTION, nDataPathSize>, nConfig> memreq_array;
    //    inport< memreq<INSTRUCTION, nDataPathSize>, nConfig>::unisim_type_array_t memreq_array;
    boost::array< SuperData<memreq <INSTRUCTION, nDataPathSize> >, nConfig> memreq_array;

    bool valid = false;

    for (int i=0; i<nConfig; i++)
    {
    OoOQueuePointer<DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize>, nCtrlQueueSize> entry;
    /* Search for a read */
    entry = queue[i].SeekAtHead();
    while(entry)
    { /* Is it a read ? */
      if(!entry->write && (!dataBusOwner[i] || dataBusOwner[i] == entry))
      { /* Is there some data to send to CPU ? */
        if(entry->read < entry->size)
        { valid = true;
          memreq <INSTRUCTION, nDataPathSize> mr;
          mr.command = entry->command;
          mr.message_type = memreq_types::type_ANSWER;
          mr.address = entry->base_address + entry->read_offset;
          mr.uid = entry->tag;
          mr.memreq_id = entry->memreq_id;
          mr.instr = entry->instr;
          mr.size = nDataPathSize;
          mr.Write(entry->data + entry->read_offset, nDataPathSize);
          dataBusOwner[i] = entry;
          mr.sender_type = memreq_types::sender_MEM;
          mr.sender = this;
          mr.req_sender = entry->req_sender;
          mr.cachable = entry->cachable;
	  mr.valid = true;
//INFO << "Sending  " << mr << endl;
#ifdef DD_DEBUG_DRAM
	  cerr << "[DD_DEBUG_DRAM] Sending : " << entry->req_sender << endl;
	  cerr << "[DD_DEBUG_DRAM] Sending : " << mr << endl;
#endif

        #ifdef NOC_THREADS_DISTRIBUTION
	  mr.dst_id = entry->src_id;
	  if(mr.dst_id != 0)
	  {
	      cout<<"DRAM outdata: "<< mr <<endl;
	  }
        #endif

	  //          out.data[i] = mr;
	  memreq_array[i] = mr;
        }
        break;
      }
      entry++;
    }
    }
    if(valid) out.data = memreq_array;
    if(!valid) out.data.nothing();
    if(VERBOSE) dump();
    /*
      #ifdef DD_DEBUG_DRAM
      cerr << "[DD_DEBUG_DRAM] DRAM dump() : " << endl;
      dump(cerr);
      #endif
    */
  }

  /**
   * \brief Process called at the end of the cycle
   */
  void end_of_cycle()
  { 
#ifdef DD_DEBUG_DRAM
    cerr << "[DD_DEBUG_DRAM] DRAM : -------------- End of cycle (" << timestamp() << ") START"<< endl;
#endif
#ifdef DD_DEBUG_DRAM_DATATRANS
    cerr << "[DD_DEBUG_DRAM] DRAM dump() Just Before EOF: " << endl;
    dump(cerr);
#endif


    OoOQueuePointer<DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize>, nCtrlQueueSize> entry;

    //    boost::array<bool, 0>::iterator iter;

    //    iter = ((boost::array<bool, nConfig> *)(&out.accept))->begin();
    //    for (int i=0;iter=)
    // Foreach configuartion
    for (int i=0; i<nConfig; i++)
    {
    /* For each entry in the memory controler queue */
    entry = queue[i].SeekAtHead();
    if(entry)
    { do
      { if(!entry->write)
        { /* It is a read */
          /* Is there a data sended to CPU ? */
          if(entry->read < entry->size && dataBusOwner[i] == entry)
          { /* Did the CPU accept the data ? */
            if(out.accept[i])
            { entry->read += nDataPathSize;
              entry->read_offset += nDataPathSize;
              if(entry->read_offset >= nCacheLineSize) entry->read_offset = 0;
              if(entry->read >= nCacheLineSize) dataBusOwner[i] = 0;
            }
            else
            { ERROR << "L2 cache did not accept a response (" << *entry << ")" << endl;
        //      exit(-1);
            }
          }
          break;
        }
        entry++;
      }
      while(entry);
    }

    /* Remove each served requests from the memory controler queue */
    entry = queue[i].SeekAtHead();
    if(entry)
    { do
      { /* A request is served if the whole burst was read either by CPU or DRAM banks */
        if(entry->read < nCacheLineSize) break;
        if(entry->time != (unsigned long long int)-1)
        { entry->time = (uint64_t)timestamp() - entry->time;
          cumulative_load_time[i] += entry->time;
          if(entry->time < min_load_time[i]) min_load_time[i] = entry->time;
          if(entry->time > max_load_time[i]) max_load_time[i] = entry->time;
          num_read_requests[i]++;
          average_latency[i] = cumulative_load_time[i] / num_read_requests[i];
        }
        queue[i].Remove(entry);
      }
      while(entry);
    }
    } // End of foreach configuration

    for (int i=0; i<nConfig; i++)
    {

    /* Is there a new memory request ? */
    if(in.enable[i])
    { memreq<INSTRUCTION, nDataPathSize> mr = in.data;

#ifdef DD_DEBUG_DRAM
      cerr << "[DD_DEBUG_DRAM] DRAM processing a new request : " <<  mr << endl;
#endif
//#ifdef DEBUG_TEST_UNCACHABLE_WRITE
//      INFO << "receiving request" << mr << endl;
//#endif
#ifdef DUMP_DRAM_IN_REQUESTS
      ofstream os("dump_dram_in.log",ios_base::app);
      os << left << setw(10) << timestamp() << mr << endl;
      os.close();
#endif

  #ifdef NOC_THREADS_DISTRIBUTION
     if( (mr.dst_id == 0) && (mr.sender_type != memreq_types::sender_MEM) )
  #endif
     {
    #ifdef NOC_THREADS_DISTRIBUTION
      if(mr.src_id != 0 )
          cout<<"DRAM receives "<<mr <<endl;
    #endif
 
      if((mr.command != memreq_types::cmd_FLUSH) && (!Snooping || !inShared.data))
      { // Don't care about cache flush requests, and Request with the Shared bit set to true in case
        // of snooping.
        
        if(this==mr.sender)
        { // First ensure i am not receiving what I am sending
          ERROR << "Receiving a request from myself !\n" << mr << endl;
          exit(1);
        }

//        if(mr.message_type==memreq_types::type_REQUEST)
//        { // Memory should only answers to (and store) requests

      
//INFO << "Stored   " << mr << endl;
#ifdef _CACHE_MESSAGES_SVG_H_
          SVG.add(timestamp(),"BS","MM","",mr);
#endif
          if(svg)
          { svg->add_bus_to_mem(timestamp(),name(),"",mr);
          }

    
          /* Get the lastest request in the queue */
          DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize> *entry = queue.GetTail();
          /* If there is a write request and the buffer is not full */
          if(entry && entry->write && entry->size < nCacheLineSize)
          { int size = mr.size;
            /* Copy the CPU data into the buffer */
            memcpy(entry->data + entry->write_offset, mr.Read(), size);
            /* Increment the number of bytes in the buffer */
            entry->size += size;
            entry->write_offset += size;
            if(entry->write_offset >= nCacheLineSize) entry->write_offset = 0;
            dataBusOwner = entry->size < nCacheLineSize ? entry : 0;
          }
          else
          { /* Create a new queue entry because this is a new request (start of a burst) */
            entry = queue.New();
            if(entry)
            { entry->launched = false;
              entry->address = mr.address;
              int size = mr.size;
              entry->req_sender = mr.req_sender;
              entry->cachable = mr.cachable;
              entry->size = 0;
              entry->read = 0;
              entry->instr = mr.instr;
              entry->tag = mr.uid;
              entry->memreq_id = mr.memreq_id;
              entry->write = mr.command == memreq_types::cmd_WRITE;
              entry->command = mr.command;
              entry->dram_data_transfer_finished = false;
              entry->base_address = entry->address & ~(nCacheLineSize - 1);
              //entry->write_offset = entry->address & (nCacheLineSize - 1);  STF: WHY WAS IT CHANGED ? DAVID BUGFIX ?
              entry->write_offset = (entry->address & ~(nDataPathSize - 1)) - entry->base_address;
              entry->read_offset = entry->write_offset;
              address_t word_address = entry->address / nDataPathSize;
              entry->bank = (word_address / nCols) & (nBanks - 1);
              entry->col = word_address & (nCols - 1);
              entry->row = (word_address / nCols / nBanks) & (nRows - 1);

            #ifdef NOC_THREADS_DISTRIBUTION
              entry->src_id = mr.src_id;
            #endif

              if(entry->write)
              { memcpy(entry->data + entry->write_offset, mr.Read(), size);
                entry->size += size;
                entry->write_offset += size;
                if(entry->write_offset >= nCacheLineSize) entry->write_offset = 0;
                entry->time = -1;
              }
              else
              { entry->time = (unsigned long long int)timestamp();
              }
              dataBusOwner = entry->write && entry->size < nCacheLineSize ? entry : 0;
            }
            else
            { ERROR << "DRAM controler queue overflow" << endl;
              exit(-1);
            }
#ifdef DD_DEBUG_DRAM
	    if (entry)
	      {
		cerr << "[DD_DEBUG_DRAM] Store a new entry : " <<  *entry << endl;
		dump(cerr);
	      }
#endif	    
          }
          if(entry && !entry->launched && (!entry->write || (entry->write && entry->size == nCacheLineSize)))
          { ScheduleDRAMQueue();
#ifdef DD_DEBUG_DRAM_DATATRANS
	  cerr << "[DD_DEBUG_DRAM] DRAM dump() Just After Scheduling : " << endl;
	  dump(cerr);
#endif
          }
//        } // type = request
      } // !Snooping || !inShared.data
      else 
      { // Discarding enabled message because either of:
        // - the shared bit was set while snooping is enabled
        // - the request is a cache flush
#ifdef _CACHE_MESSAGES_SVG_H_
          SVG.add(timestamp(),"BS","MM","X",mr);
#endif
        if(svg)
        { svg->add_bus_to_mem(timestamp(),name(),"X",mr);
        }

      }
     }
    } //in.enable

    } // End of foreach configuration



    /* Manage the DRAM control */
    DRAMControl();
#ifdef DD_DEBUG_DRAM_DATATRANS
	  cerr << "[DD_DEBUG_DRAM] DRAM dump() Just After DRAMControl : " << endl;
	  dump(cerr);
#endif

    /* Manage the data transfer between the controler buffers and the DRAM */
    DRAMDataTransfer();
#ifdef DD_DEBUG_DRAM_DATATRANS
	  cerr << "[DD_DEBUG_DRAM] DRAM dump() Just After DRAMDataTransfer : " << endl;
	  dump(cerr);
#endif

    /* Is it the moment to do a memory refresh ? */
    if(refresh_counter == 0 && queue.Empty())
    { /* Was a refresh initiated ? */
      if(!refreshing)
      { /* No */
        /* Precharge all banks */
        if(PrechargeAll())
        { refreshing = true;
        }
      }
      else
      { /* Yes */
        /* Continue refresh the rows of the banks */
        if(Refresh(row_refresh))
        { /* Increment the row number */
          /* If all row have been refreshed */
          if(++row_refresh == nRows)
          { /* set the refresh counter to the refresh period TREF */
            row_refresh = 0;
            refreshing = false;
            refresh_counter = TREF;
          }
        }
      }
    }

    if(refresh_counter > 0) refresh_counter--;
    time_stamp++;
#ifdef DD_DEBUG_DRAM_DATATRANS
    cerr << "[DD_DEBUG_DRAM] DRAM dump() Just After EOF : " << endl;
    dump(cerr);
#endif
#ifdef DD_DEBUG_DRAM
    cerr << "[DD_DEBUG_DRAM] DRAM : -------------- End of cycle (" << timestamp() << ") END"<< endl;
#endif
  }


  /** Read data from the memory
  		@param buffer a buffer where to put the data
  		@param address the starting address
  		@param size the size in bytes
  */
 void Read(void *buffer, address_t address, int size)
 { container.Read((uint8_t *)buffer, address, size);
 }

  /** Write data into the memory
  		@param address the starting address
  		@param buffer a buffer to copy into memory
  		@param size the size in bytes
  */
  void Write(address_t address, const void *buffer, int size)
  { container.Write(address, (uint8_t *)buffer, size);
/*
if(VERBOSE)
{
#ifdef DRAM_DUMP_ONLY_ADDRESS_MIN
if(address>=DRAM_DUMP_ONLY_ADDRESS_MIN)
#endif
#ifdef DRAM_DUMP_ONLY_ADDRESS_MAX
if(address<=DRAM_DUMP_ONLY_ADDRESS_MAX)
#endif
{
cerr << "STF:::: 0x" << hex << address << dec << ", " << size << " = ";
print_buffer(cerr,buffer,size);
}
}
*/
  }

  /** Initialize several byte into the memory
  		@param address the starting address
  		@param data the value to copie into the memory
  		@param size the size in bytes
  */
  void Set(address_t address, uint8_t data, int size)
  { container.Set(address, data, size);
  }

  /** Dumps the DRAM and the DRAM controler state into an output stream
  		@param os an output stream
  		@param dram a DRAM module
  		@return the output stream
  */
  friend ostream& operator << (ostream& os, const DRAM& dram)
  { int bank;
    os << dram.queue;
    { os << "Debug queue:" << endl;
      OoOQueuePointer<DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize>, nCtrlQueueSize> entry;

      /* For each entry in the controler queue */
      entry = dram.queue.SeekAtTail();
      if(entry)
      { do
        { 
#ifdef DD_DEBUG_DRAM_DATATRANS
	  cerr << "[DD_DEBUG_DRAM] <Debug Queue> Entry Req_Sender: " << entry->req_sender << endl;
#endif
	  os << *entry << endl;
          entry--;
        }
        while(entry);
      }
    }
    if(dram.dataBusOwner) os << "Data Bus Owner : " << *dram.dataBusOwner << endl;
    for(bank = 0; bank < nBanks; bank++)
    { os << "-- bank #" << bank << " --" << endl;
      os << dram.banks[bank] << endl;
    }
    return os;
  }

  /**
   * \brief  Dumps some statistics into an output stream
   */
  void DumpStats(ostream& os, uint64_t sim_total_time, uint64_t sim_cycle)
  { int bank;
    for(bank = 0; bank < nBanks; bank++)
    { os << fsc_object::name() << "_bank" << bank << "_total_precharge " << banks[bank].total_precharge << " # total number of precharge of bank #" << bank << endl;
      os << fsc_object::name() << "_bank" << bank << "_total_activate " << banks[bank].total_activate << " # total number of activate of bank #" << bank << endl;
      os << fsc_object::name() << "_bank" << bank << "_total_read " << banks[bank].total_read << " # total number of burst read of bank #" << bank << endl;
      os << fsc_object::name() << "_bank" << bank << "_total_write " << banks[bank].total_write << " # total number of burst write of bank #" << bank << endl;
    }
    os << fsc_object::name() << "_total_precharge " << total_precharge << " # total number of precharge" << endl;
    os << fsc_object::name() << "_total_activate " << total_activate << " # total number of activate" << endl;
    os << fsc_object::name() << "_total_read " << total_read << " # total number of burst read" << endl;
    os << fsc_object::name() << "_total_write " << total_write << " # total number of burst write" << endl;
    os << fsc_object::name() << "_total_refresh " << total_refresh << " # total number of refresh" << endl;
    os << fsc_object::name() << "_cumulative_load_time " << cumulative_load_time << " # total cumulative load time" << endl;
    if(total_read > 0) os << fsc_object::name() << "_average_load_time " << cumulative_load_time/total_read << " # average load time" << endl;
    os << fsc_object::name() << "_reordered_requests " << reordered_requests << " # number of reordered requests" << endl;
    os << fsc_object::name() << "_min_load_time " << min_load_time << " # minimum load time" << endl;
    os << fsc_object::name() << "_max_load_time " << max_load_time << " # maximum load time" << endl;
  }

  /**
   * \brief perform a check on the controle entries
   */
  bool Check()
  { return queue.Check();
  }

  /**
   * \brief Perform a warm restart of the memory
   */
  void WarmRestart()
  { int bank;
    queue.Reset();
    for(bank = 0; bank < nBanks; bank++)
    { banks[bank].WarmRestart();
    }
    time_stamp = 0;
    refresh_counter = TREF;
    row_refresh = 0;
    refreshing = false;
    dataBusOwner = 0;
    lastBank = -1;
  }

  /**
   * \brief Reset the statistic counters
   */
  void ResetStats()
  { int bank;
    for(bank = 0; bank < nBanks; bank++)
    { banks[bank].ResetStats();
    }
    num_read_requests = 0;
    cumulative_load_time = 0;
    reordered_requests = 0;
    min_load_time = 10000000000LL;
    max_load_time = 0;
    average_latency = 0;
  }

  uint64_t average_latency[nConfig];              ///< average latency statistic
  uint64_t total_precharge[nConfig];              ///< total number of precharge statisic
  uint64_t total_activate[nConfig];               ///< total number of activate statisic
  uint64_t total_read[nConfig];                   ///< total number of read statisic
  uint64_t total_write[nConfig];                  ///< total number of write statisic
  uint64_t total_refresh[nConfig];                ///< total number of refresh statisic
  uint64_t num_read_requests[nConfig];            ///< total number of read request statistic
  uint64_t cumulative_load_time[nConfig];         ///< total cumulative load time statistic
  uint64_t reordered_requests[nConfig];           ///< total number of reordered request statistic
  uint64_t min_load_time[nConfig];                ///< minimum load time statistic
  uint64_t max_load_time[nConfig];                ///< maximum load time statistic

  DRAMBank<INSTRUCTION, nCacheLineSize> banks[nBanks][nConfig]; ///< Memory banks
private:
  MemoryContainer <VERBOSE> container[nConfig];
  OoOQueue<DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize>, nCtrlQueueSize> queue[nConfig];
  DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize> *dataBusOwner[nConfig];
  uint64_t time_stamp[nConfig];
  uint64_t refresh_counter[nConfig];
  int row_refresh[nConfig];
  bool refreshing[nConfig];
  int lastBank[nConfig];
  
 public:
  //  MyMemEmulator *memory_emulator;         ///< Validation memory
  // Do we need multiple memory emulator ?
  //  unisim::component::cxx::memory::ram::Memory<address_t> *memory_emulator[nConfig];
  unisim::component::cxx::memory::ram::Memory<address_t> *memory_emulator;

 private:
  /**
   * \brief Dump some debugging information
   */
  void dump()
  { if(timestamp()==0)
    { ofstream os("dram.log",ios_base::trunc);
      os.close();
    }
    ofstream os("dram.log",ios_base::app);
    os << container;
    os.close();
  }
  
  void dump(ostream &os)
  { 
    os << "\n-----------------------------------------------------------------------" << endl;
    os << "  Cycle " << left << timestamp() << endl;
    os << "-----------------------------------------------------------------------" << endl;
    os << *this;
    os << "-----------------------------------------------------------------------" << endl;
  }

  /**
   * \brief Program loader
   */
  void LoadProgram(char *filename, address_t image_addr)
  { FILE *fobj = fopen(filename, "rb");
    char *buffer;

    if(!fobj)
    { fprintf(stderr, "Can't open image %s\n", filename);
      abort();
    }

    fseek(fobj, 0, SEEK_END);

    int sz = ftell(fobj);
    if(sz >= 0)
    { buffer = (char *) malloc(sz);
      fseek(fobj, 0, SEEK_SET);
      fread(buffer, sz, 1, fobj);
      container.Write(image_addr, buffer, sz);
      free(buffer);
    }
    else
    { fprintf(stderr, "Warning: Can't load image %s\n", filename);
    }

    fclose(fobj);
  }

  // --- Checkpointing feature ------------------------------------------------------ 

 public:

  /**
   * \brief Loads the par corresponding to the memory from a checkpoint file
   */
  void load_checkpoint(istream &is)
  { streampos pos = is.tellg();
    container.load_checkpoint(is);
    is.seekg(pos);
    //    memory_emulator->load_checkpoint(is);
  }

  /**
   * \brief Module interface : Returns true if the module has some pending operations.
   */
  bool has_pending_ops()
  { if(queue.Size()) return true;
    else return false;
  }

 int get_rank()
 { return in.get_connected_module()->get_rank()+1;
 }

};

} // end of namespace dram
} // end of namespace memory
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
