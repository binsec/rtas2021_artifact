/* *****************************************************************************
  dram_components.h  -  Defines the DRAM components (queues, ...)
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

#ifndef __UNISIM_COMPONENT_CLM_MEMORY_DRAM_DRAM_COMPONENT_HH__
#define __UNISIM_COMPONENT_CLM_MEMORY_DRAM_DRAM_COMPONENT_HH__

#include <unisim/component/clm/utility/common.hh>
#include <unisim/component/clm/memory/mem_common.hh>
#include <unisim/component/clm/utility/utility.hh>
#include <unisim/component/clm/memory/memory_container.hh>
#include <unisim/component/clm/interfaces/memreq.hh>


namespace unisim {
namespace component {
namespace clm {
namespace memory {
namespace dram {

// new usings
//using unisim::component::clm::interface::memreq;
using unisim::component::clm::interfaces::memreq_types;

using unisim::component::clm::utility::hexa;

/**
 * \brief DRAM Controler Queue Entry
 */
template <class INSTRUCTION, int nCacheLineSize>
class DRAMControlerQueueEntry
{public:
  bool launched;                      ///< True once the request start to be processed

  bool dram_data_transfer_finished;   ///< True once the data transfer is over
  bool write;                         ///< Wether the entry correspond to a write
  address_t address;                  ///< Target address of the entry
  address_t base_address;             ///< Base address of the entry
  int write_offset;                   ///< Entry write offset
  int read_offset;                    ///< Entry read offset
  int row;                            ///< Corresponding row
  int col;                            ///< Corresponding column
  int bank;                           ///< Corresponding bank
  INSTRUCTION instr;                  ///< Instruction that issued this entry
  int size;                           ///< Size of the request (in bytes)
  int read;                           ///< ???
  char data[nCacheLineSize];          ///< Corresponding data
  int tag;                            ///< Associated tag
  uint64_t memreq_id;                 ///< Coresponding memreq_id
  module *req_sender;                 ///< Module that issued the corresponing request
  bool cachable;                      ///< Wether the request was cachable
  memreq_types::command_t command;    ///< Request command type (read, write, ...)
  unsigned long long int time;        ///< ???
 #ifdef NOC_THREADS_DISTRIBUTION
  int src_id;
 #endif

  /**
   * \brief Copy constructor
   */
  DRAMControlerQueueEntry & operator = (const DRAMControlerQueueEntry & entry)
  { launched = entry.launched;

    dram_data_transfer_finished = entry.dram_data_transfer_finished;
    write = entry.write;
    address = entry.address;
    base_address = entry.base_address;
    write_offset = entry.write_offset;
    read_offset = entry.read_offset;
    row = entry.row;
    col = entry.col;
    bank = entry.bank;
    instr = entry.instr;
    size = entry.size;
    read = entry.read;
    memcpy(data, entry.data, nCacheLineSize);
    tag = entry.tag;
    req_sender = entry.req_sender;
    cachable = entry.cachable;
    memreq_id = entry.memreq_id;
  #ifdef NOC_THREADS_DISTRIBUTION
    src_id = entry.src_id;
  #endif
 
    return *this;
  }

  /**
   * \brief Class pretty printer
   */
  friend ostream& operator << (ostream& os, const DRAMControlerQueueEntry& entry)
  { os << "launched=" << entry.launched;
    os << ",dram_data_transfer_finished=" << entry.dram_data_transfer_finished;
    os << ",write=" << entry.write;
    os << ",size=" << entry.size;
    os << ",read=" << entry.read;
    os << ",address=" << hexa(entry.address);
    os << ",base_address=" << hexa(entry.base_address);
    os << ",write_offset=" << entry.write_offset;
    os << ",read_offset=" << entry.read_offset;
    os << ",row=" << entry.row;
    os << ",col=" << entry.col;
    os << ",bank=" << entry.bank;
    os << ",tag=" << entry.tag;
  #ifdef NOC_THREADS_DISTRIBUTION
    os << ",src_id=" << entry.src_id;
  #endif
    os << ",data=";
    for(int i = 0; i < nCacheLineSize; i++) os << hexa(entry.data[i]);
    os << ",instr=" << entry.instr;
    return os;
  }
};


// SDRAM Command summary
//   ACTV: Activate Row
//    PRE: Deactivate (precharge) Current bank
// PREALL: Deactivate (precharge) All Banks
//   REFR: auto Refresh
//   SLFR: Self Refresh
//  SLFRE: Self Refresh Exit
//    MRS: Mode Register Set
//   READ: Read at column address
//    WRT: Write at column address
//  READP: Read with automatic Precharge
//   WRTP: Write with automatic Precharge

// SDRAM Timing constraints
//      ACTV   <---------- TRAS ----------> PRE, PREALL
//      ACTV   <---------- TRCD ----------> READ, WRT
// PRE, PREALL <----------- TRP ----------> ACTV, MRS, REFR, SLFR
//      REFR   <----------- TRC ----------> ACTV, MRS, REFR, SLFR
//      ACTV   <----------- TRC ----------> ACTV, MRS, REFR, SLFR
//      SLFRE  <----------- TRC ----------> ACTV, MRS, REFR, SLFR
//       ACTV  <---------- TRRD ----------> ACTV (of a different bank)
//        MRS  <---------- TRSA ----------> ACTV, REFR, SLFR, MR

enum SDRAMState {
  IdleState,
  ModeRegisterSetState,
  SelfRefreshState,
  AutoRefreshState,
  RowActiveState,
  ReadState,
  WriteState,
  ReadPState,
  WritePState,
  PrechargeState
};

/**
 * \brief DRAM Bank
 */
template <class INSTRUCTION, int nCacheLineSize>
class DRAMBank
{public:
  SDRAMState state;         ///< Associated state
  //bool valid;
  int col;                  ///< Associated column
  int row;                  ///< Associated row
  int burst_counter;        ///< Burst counter
  uint64_t lastACTV;        ///< Event timestamp to compute latencies
  uint64_t lastPRE;         ///< Event timestamp to compute latencies
  uint64_t lastPREALL;      ///< Event timestamp to compute latencies
  uint64_t lastREFR;        ///< Event timestamp to compute latencies
  uint64_t lastSLFR;        ///< Event timestamp to compute latencies
  uint64_t lastSLFRE;       ///< Event timestamp to compute latencies
  uint64_t lastMRS;         ///< Event timestamp to compute latencies
  uint64_t lastREAD;        ///< Event timestamp to compute latencies
  uint64_t lastWRT;         ///< Event timestamp to compute latencies
  uint64_t lastREADP;       ///< Event timestamp to compute latencies
  uint64_t lastWRTP;        ///< Event timestamp to compute latencies
  DRAMControlerQueueEntry<INSTRUCTION, nCacheLineSize> *entry;  ///< Controller queue entry
  long long int total_activate;       ///< Statistics: total number of activate
  long long int total_precharge;      ///< Statistics: total number of precharge
  long long int total_read;           ///< Statistics: total number of read
  long long int total_write;          ///< Statistics: total number of write

  /**
   * \brief Creates a new DRAM bank
   */
  DRAMBank()
  { state = IdleState;
    row = 0;
    burst_counter = 0;
    //valid = false;
    lastACTV = 0;
    lastPRE = 0;
    lastPREALL = 0;
    lastREFR = 0;
    lastSLFR = 0;
    lastSLFRE = 0;
    lastMRS = 0;
    lastREAD = 0;
    lastWRT = 0;
    lastREADP = 0;
    lastWRTP = 0;
    total_activate = 0;
    total_precharge = 0;
    total_read = 0;
    total_write = 0;
  }

  /**
   * \brief Perform a warm restart
   */
  void WarmRestart()
  { state = IdleState;
    row = 0;
    burst_counter = 0;
    lastACTV = 0;
    lastPRE = 0;
    lastPREALL = 0;
    lastREFR = 0;
    lastSLFR = 0;
    lastSLFRE = 0;
    lastMRS = 0;
    lastREAD = 0;
    lastWRT = 0;
    lastREADP = 0;
    lastWRTP = 0;
  }

  /**
   * \brief Reset all the statistics
   */
  void ResetStats()
  { total_activate = 0;
    total_precharge = 0;
    total_read = 0;
    total_write = 0;
  }

  /**
   * \brief Class pretty printer
   */
  friend ostream& operator << (ostream& os, const DRAMBank& bk)
  { os << "state=";
    switch(bk.state)
    { case RowActiveState:
        os << "Row Active";
        break;
      case PrechargeState:
        os << "Precharge";
        break;
      case ReadState:
        os << "Read";
        break;
        case WriteState:
        os << "Write";
        break;
        case IdleState:
        os << "Idle";
        break;
        default:
        ;
    }
    return os;
  }
};

} // end of namespace dram
} // end of namespace memory
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
