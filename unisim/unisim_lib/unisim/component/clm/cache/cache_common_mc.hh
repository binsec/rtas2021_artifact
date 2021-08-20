/***************************************************************************
   CacheCommon.h  -  Defines the cache components (buffers, ...)
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

#ifndef __UNISIM_COMPONENT_CLM_CACHE_CACHE_COMMON_HH__
#define __UNISIM_COMPONENT_CLM_CACHE_CACHE_COMMON_HH__

//#include "mem_common.h"
#include <unisim/component/clm/memory/mem_common.hh>
#include <unisim/component/clm/interfaces/memreq_mc.hh>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace clm {
namespace cache {

  using unisim::component::clm::interfaces::memreq_types;

#define ID_MSHR  0
#define ID_CACHE 1

/* cache request states */

#define NOTHING                                        0

#define READ                                           1
#define READ_HIT_READY                                 2
#define READ_MISS                                      3
#define READ_MISS_REQUESTED                            4
#define READ_MISS_READY                                5
#define READ_MISS_LINE_READY                           6
#define READ_MISS_SERVICED                             7
#define READ_MISS_EVICTION                             8
#define READ_MISS_EVICTION_WRITE_DONE                  9
#define READ_MISS_EVICTION_WRITE_DONE_REQUESTED       10
#define READ_MISS_EVICTION_WRITE_DONE_READY           11
#define READ_MISS_EVICTION_WRITE_DONE_LINE_READY      12
#define READ_MISS_EVICTION_WRITE_DONE_SERVICED        13
#define READ_MISS_WAITING_ACCEPT                      14
#define READ_MISS_EVICTION_WAITING_ACCEPT             15
#define READ_MISS_EVICTION_WRITE_DONE_WAITING_ACCEPT  16
#define READ_MSHR                                     17
#define READX                                         18
#define READ_UNCACHABLE                               19
#define READ_UNCACHABLE_REQUESTED                     20
#define READ_UNCACHABLE_READY                         21
#define READ_HIT_READY_EVICTION                       22

#define WRITE                                        101
#define WRITE_MISS                                   102
#define WRITE_MISS_REQUESTED                         103
#define WRITE_MISS_EVICTION                          104
#define WRITE_MISS_EVICTION_WRITE_DONE               105
#define WRITE_MISS_EVICTION_WRITE_DONE_REQUESTED     106
#define WRITE_MISS_NO_READ                           107
#define WRITE_MISS_WAITING_ACCEPT                    108
#define WRITE_MISS_EVICTION_WAITING_ACCEPT           109
#define WRITE_MISS_EVICTION_WRITE_DONE_WAITING_ACCEPT 110
#define WRITE_MISS_NO_READ_WAITING_ACCEPT            111
#define WRITE_MSHR                                   112
#define WRITE_UNCACHABLE                             113
#define WRITE_UNCACHABLE_REQUESTED                   114
#define WRITE_UNCACHABLE_READY                       115

#define PREFETCH                                     201
#define PREFETCH_MISS                                202
#define PREFETCH_MISS_REQUESTED                      203
#define PREFETCH_MISS_EVICTION                       204
#define PREFETCH_MISS_EVICTION_WRITE_DONE            205
#define PREFETCH_MISS_EVICTION_WRITE_DONE_REQUESTED  206
#define PREFETCH_MISS_WAITING_ACCEPT                 207
#define PREFETCH_MISS_EVICTION_WAITING_ACCEPT        208
#define PREFETCH_MISS_EVICTION_WRITE_DONE_WAITING_ACCEPT 209
#define PREFETCH_MSHR                                210

#define EVICT                                        301
#define EVICT_READY                                  302
#define EVICT_READY_WAITING_ACCEPT                   303

#define WRITE_HIT_FLUSH                              401
#define WRITE_HIT_FLUSH_REQUESTED                    402

template <class INSTRUCTION,
int nLineSize,
int nCPUDataPathSize >
class ReturnData
{public:
  INSTRUCTION instr;            ///< the instruction
  char data[nLineSize];         ///< Corresponding data
  int size;                     ///< the size of the requested data
  bool ready;                   ///< true if it contains data to be returned to the CPU
  address_t address;            ///< the address of the data to return
  int id;                       ///< to who is sent the data (mshr, cpu, cachequeue)
  int port;                     ///< Port number the data should be sent to
  int index;                    ///< Associated index
  address_t base;               ///< Corresponding base address
  int uid;                      ///< Associated uid
  uint64_t memreq_id;           ///< Associated unique memreq ID
  int cacheQueueIndex;          ///< Index in the cache queue
  //  module *req_sender;           ///< Pointer to the module that issued the corresponding request
  string req_sender;           ///< Pointer to the module that issued the corresponding request
  bool cachable;                ///< Wether or not the request is cachable
#ifdef NOC_THREADS_DISTRIBUTION
  int dst_id;                  ///< destination tile id
  int src_id;                  ///< source tile id
#endif
  
  /**
   * \brief Class constructor
   */
  ReturnData()
  { Reset();
  }

  /**
   * \brief Reset the return data
   */
  void Reset()
  { for(int i = 0; i < nLineSize; i++) data[i] = 0;
    size = 0;
    ready = false;
    address = 0;
    id = 0;
    port = 0;
    index = 0;
    base = 0;
    uid = 0;
    cacheQueueIndex = 0;
    //    req_sender=NULL;
    req_sender="";
    cachable=true;
    memreq_id = 0;
#ifdef NOC_THREADS_DISTRIBUTION
    dst_id = 0;
    src_id = 0;
#endif
  }

  /**
   * \brief Pretty printer of the ReturnData class
   */
  friend ostream& operator << (ostream& os, const ReturnData<INSTRUCTION, nLineSize, nCPUDataPathSize>& rd)
  { int i;
    os << "instruction={" << rd.instr << "}";
    os << ",data=";
    //    for(i = 0; i < rd.size; i++) {
    for(i = 0; i < nLineSize; i++)
    {
      os << hexa(rd.data[i]);
      //      if(i < rd.size - 1) os << " ";
      if(i < nLineSize - 1)
        os << " ";
    }
    os << ",size=" << rd.size;
    os << ",ready=" << rd.ready;
    os << ",address=" << hexa(rd.address);
    os << ",id=" << rd.id;
    os << ",port=" << rd.port;
    os << ",index=" << rd.index;
    os << ",base=" << hexa(rd.base);
    os << ",uid=" << rd.uid;
    os << ",cacheQueueIndex= " << rd.cacheQueueIndex;
    return os;
  }
};

template <class INSTRUCTION,
int nMemDataPathSize,
int nLineSize>
class RequestData
{
public:
  INSTRUCTION instr;                           ///< the instruction
  char data[nLineSize];                        ///< the requested data
  int size;                                    ///< the size of the requested data
  bool ready;                                  ///< true if it contains data to be returned to the CPU
  bool write;                                  ///< true if it is a write request
  address_t address;                           ///< the address of the data to return
  int id;                                      ///< to who is sent the data (mshr, cpu, cachequeue)
  int index;                                   ///< Associated index
  int cacheQueueIndex;                         ///< Index in the cache queue
  address_t base;                              ///< Corresponding base address
  bool hit;                                    ///< Wether the request is a hit or a miss
  char fvclinedata[nLineSize];                 ///< fields needed for the data in the fvc in case of eviction
  int uid;                                     ///< Associated uid
  uint64_t memreq_id;                          ///< Associated unique memreq ID
  int port;                                    ///< Port number the data is received on
  //  module *req_sender;                          ///< Pointer to the module that issued the corresponding request
  string req_sender;                          ///< Pointer to the module that issued the corresponding request
  memreq_types::message_type_t message_type;   ///< Type of the message (Request, Answser)
  memreq_types::command_t command;             ///< Command tpe (read, write, evict, ...)
  bool cachable;                               ///< Wether or not the request is cachable
#ifdef NOC_THREADS_DISTRIBUTION
  int dst_id;                  ///< destination tile id
  int src_id;                  ///< source tile id
#endif

  /**
   * \brief Class constructor
   */
  RequestData()
  { Reset();
  }

  /**
   * \brief Reset the request data
   */
  void Reset()
  { for(int i = 0; i < nLineSize; i++)
    { data[i] = 0;
      fvclinedata[i] = 0;
    }
    size = 0;
    ready = false;
    write = false;
    address = 0;
    id = 0;
    index = 0;
    cacheQueueIndex = -1;
    base = 0;
    hit = false;
    uid = 0;
    port = 0;
    req_sender = "";
    message_type = memreq_types::type_UNKNOWN;
    command = memreq_types::cmd_UNKNOWN;
    cachable=true;
    memreq_id = 0;
#ifdef NOC_THREADS_DISTRIBUTION
    dst_id = 0;
    src_id = 0;
#endif
  }

  /**
   * \brief Pretty printer of the RequestData class
   */
  friend ostream& operator << (ostream& os, const RequestData<INSTRUCTION, nMemDataPathSize, nLineSize>& rd)
  { os << "instruction={" << rd.instr << "}";
    os << ",ready=" << rd.ready;
    os << ",write=" << rd.write;
    os << ",address=" << hexa(rd.address);
    os << ",id=" << rd.id;
    os << ",index=" << rd.index;
    os << ",base=" << hexa(rd.base);
    os << ",hit=" << rd.hit;
    os << ",uid=" << rd.uid;
    os << ",port=" << rd.port;
    return os;
  }
};

/* A C++ class (template) modelling a pipeline entry */
template <class INSTRUCTION,
int nLineSize,
int nStages,
int nCPUDataPathSize>
class CachePipeStage
{public:
  INSTRUCTION instr;           ///< the instruction 
  int delay[nStages];          ///< delay for each pipe stage
  int stage;                   ///< the stage in which the request is in the cache pipeline
  char data[nCPUDataPathSize]; ///< the requested data if the access was a hit
  char linedata[nLineSize];    ///< Data of the coressponding cache line
  bool write;                  ///< true if it was a store access
  address_t address;           ///< address that the instruction is looking for
  address_t base;              ///< Corresponding base address
  address_t index;             ///< Corresponding index
  int size;                    ///< size of the data requested
  int port;                    ///< port id that made the request
  bool hit;                    ///< true if the request is a hit
  bool block;                  ///< Corresponding block
  bool ready;                  ///< true if it contains data to be returned to the CPU
  int state;                   ///< State associated with the request (MISS_HIT_READY, WRITE_MISS_EVICTION_DONE, ...)
  int line;                    ///< Corresponding cache line
  int set;                     ///< Cooresponding cache set
  bool evicted;                ///< Wether or not anb eviction is required
  address_t evictedaddress;    ///< Address that should be evicted
  uint64_t fvcdata;            ///< fields needed for the data in the fvc in case of eviction
  bool fvchit;                 ///< fields needed for the data in the fvc in case of hit
  uint64_t memreq_id;          ///< Associated unique memreq ID

  int pdepth;                  ///< field for content directed prefetcher (cdp)

  // fields needed for the data in the fvc in case of eviction
  char fvclinedata[nLineSize];              ///< fields needed for the data in the fvc in case of eviction
  uint64_t fvcaddress;                      ///< fields needed for the data in the fvc in case of eviction
  int uid;                                  ///< Request uid
  //  module *req_sender;                       ///< Pointer to the module that issued the corresponding request
  string req_sender;                       ///< Pointer to the module that issued the corresponding request
  memreq_types::sender_type_t sender_type;  ///< Type of the sender module (cpu, cache, ...)
  bool cachable;                            ///< Wether or not the request is cachable
#ifdef NOC_THREADS_DISTRIBUTION
  int dst_id;                  ///< destination tile id
  int src_id;                  ///< source tile id
#endif


  /**
   * Class constructor
   */
  CachePipeStage()
  { int i;

    for(i = 0; i < nStages; i++)
    { delay[i] = 0;
    }
    stage = 0;
    for(i = 0; i < nCPUDataPathSize; i++)
    { data[i] = 0;
    }
    for(i = 0; i < nLineSize; i++)
    { linedata[i] = 0;
    }
    write = false;
    address = 0;
    base = 0;
    index = 0;
    size = 0;
    port = 0;
    hit = false;
    block = false;
    ready = false;
    state = 0;
    line = 0;
    set = 0;
    evicted = false;
    evictedaddress = 0;
    fvcdata = 0;
    fvchit = false;
    for(i=0; i < nLineSize; i++)
    { fvclinedata[i] = 0;
    }
    fvcaddress = 0;
    uid = 0;
    //    req_sender=NULL;
    req_sender="";
    sender_type=memreq_types::sender_UNKNOWN;
    cachable=true;
    memreq_id=0;
    pdepth = 0;
#ifdef NOC_THREADS_DISTRIBUTION
    dst_id = 0;
    src_id = 0;
#endif
    
  }

  /**
   * \brief Pretty printer of the CachePipeStage class
   */
  friend ostream& operator << (ostream& os, const CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUDataPathSize>& stage)
  { int i;
    switch(stage.state)
    { case NOTHING:
        os << "NOTHING";
        break;
      case READ:
        os << "READ";
        break;
      case READ_HIT_READY:
        os << "READ_HIT_READY";
        break;
      case READ_MISS:
        os << "READ_MISS";
        break;
      case READ_MISS_REQUESTED:
        os << "READ_MISS_REQUESTED";
        break;
      case READ_MISS_READY:
        os << "READ_MISS_READY";
        break;
      case READ_MISS_LINE_READY:
        os << "READ_MISS_LINE_READY";
        break;
      case READ_MISS_SERVICED:
        os << "READ_MISS_SERVICED";
        break;
      case READ_MISS_EVICTION:
        os << "READ_MISS_EVICTION";
        break;
      case READ_MISS_EVICTION_WRITE_DONE:
        os << "READ_MISS_EVICTION_WRITE_DONE";
        break;
      case READ_MISS_EVICTION_WRITE_DONE_REQUESTED:
        os << "READ_MISS_EVICTION_WRITE_DONE_REQUESTED";
        break;
      case READ_MISS_EVICTION_WRITE_DONE_READY:
        os << "READ_MISS_EVICTION_WRITE_DONE_READY";
        break;
      case READ_MISS_EVICTION_WRITE_DONE_LINE_READY:
        os << "READ_MISS_EVICTION_WRITE_DONE_LINE_READY";
        break;
      case READ_MISS_EVICTION_WRITE_DONE_SERVICED:
        os << "READ_MISS_EVICTION_WRITE_DONE_SERVICED";
        break;
      case READ_MISS_WAITING_ACCEPT:
        os << "READ_MISS_WAITING_ACCEPT";
        break;
      case READ_MISS_EVICTION_WAITING_ACCEPT:
        os << "READ_MISS_EVICTION_WAITING_ACCEPT";
        break;
      case READ_MISS_EVICTION_WRITE_DONE_WAITING_ACCEPT:
        os << "READ_MISS_EVICTION_WRITE_DONE_WAITING_ACCEPT";
        break;
      case WRITE:
        os << "WRITE";
        break;
      case WRITE_MISS:
        os << "WRITE_MISS";
        break;
      case WRITE_MISS_REQUESTED:
        os << "WRITE_MISS_REQUESTED";
        break;
      case WRITE_MISS_EVICTION:
        os << "WRITE_MISS_EVICTION";
        break;
      case WRITE_MISS_EVICTION_WRITE_DONE:
        os << "WRITE_MISS_EVICTION_WRITE_DONE";
        break;
      case WRITE_MISS_EVICTION_WRITE_DONE_REQUESTED:
        os << "WRITE_MISS_EVICTION_WRITE_DONE_REQUESTED";
        break;
      case WRITE_MISS_NO_READ:
        os << "WRITE_MISS_NO_READ";
        break;
      case WRITE_MISS_WAITING_ACCEPT:
        os << "WRITE_MISS_WAITING_ACCEPT";
        break;
      case WRITE_MISS_EVICTION_WAITING_ACCEPT:
        os << "WRITE_MISS_EVICTON_WAITING_ACCEPT";
        break;
      case WRITE_MISS_EVICTION_WRITE_DONE_WAITING_ACCEPT:
        os << "WRITE_MISS_EVICTION_WRITE_DONE_WAITING_ACCEPT";
        break;
      case WRITE_MISS_NO_READ_WAITING_ACCEPT:
        os << "WRITE_MISS_NO_READ_WAITING_ACCEPT";
        break;

      case READ_MSHR:
        os << "READ_MSHR";
        break;
      case READX:
        os << "READX";
        break;
      case READ_UNCACHABLE:
        os << "READ_UNCACHABLE";
        break;
      case READ_UNCACHABLE_REQUESTED:
        os << "READ_UNCACHABLE_REQUESTED";
        break;
      case READ_UNCACHABLE_READY:
        os << "READ_UNCACHABLE_READY";
        break;

      case WRITE_MSHR:
        os << "WRITE_MSHR";
        break;
      case WRITE_UNCACHABLE:
        os << "WRITE_UNCACHABLE";
        break;
      case WRITE_UNCACHABLE_REQUESTED:
        os << "WRITE_UNCACHABLE_REQUESTED";
        break;
      case WRITE_UNCACHABLE_READY:
        os << "WRITE_UNCACHABLE_READY";
        break;

      case PREFETCH:
        os << "PREFETCH";
        break;
      case PREFETCH_MISS:
        os << "PREFETCH_MISS";
        break;
      case PREFETCH_MISS_REQUESTED:
        os << "PREFETCH_MISS_REQUESTED";
        break;
      case PREFETCH_MISS_EVICTION:
        os << "PREFETCH_MISS_EVICTION";
        break;
      case PREFETCH_MISS_EVICTION_WRITE_DONE:
        os << "PREFETCH_MISS_EVICTION_WRITE_DONE";
        break;
      case PREFETCH_MISS_EVICTION_WRITE_DONE_REQUESTED:
        os << "PREFETCH_MISS_EVICTION_WRITE_DONE_REQUESTED";
        break;
      case PREFETCH_MISS_WAITING_ACCEPT:
        os << "PREFETCH_MISS_WAITING_ACCEPT";
        break;
      case PREFETCH_MISS_EVICTION_WAITING_ACCEPT:
        os << "PREFETCH_MISS_EVICTION_WAITING_ACCEPT";
        break;
      case PREFETCH_MISS_EVICTION_WRITE_DONE_WAITING_ACCEPT:
        os << "PREFETCH_MISS_EVICTION_WRITE_DONE_WAITING_ACCEPT";
        break;
      case PREFETCH_MSHR:
        os << "PREFETCH_MSHR";
        break;

      case EVICT:
        os << "EVICT";
        break;
      case EVICT_READY:
        os << "EVICT_READY";
        break;
      case EVICT_READY_WAITING_ACCEPT:
        os << "EVICT_READY_WAITING_ACCEPT";
        break;

      case WRITE_HIT_FLUSH:
        os << "WRITE_HIT_FLUSH";
        break;
      case WRITE_HIT_FLUSH_REQUESTED:
        os << "WRITE_HIT_FLUSH_REQUESTED";
        break;

      default:
        os << stage.state;
        break;

    }

    
    os << ": instruction={" << stage.instr << "}";
    os << ",delay=";
    for(i = 0; i < nStages; i++)
    {
      os << stage.delay[i];
      if(i < nStages - 1)
        os << " ";
    }
    os << ",stage=" << stage.stage;
    /*
      os << ",data=";
      for(i = 0; i < stage.size; i++) {
      os << hexa(stage.data[i]);
      if(i < stage.size - 1) os << " ";
      }
    */
    os << ",write=" << stage.write;
    os << ",address=" << hexa(stage.address);
    os << ",base=" << hexa(stage.address);
    os << ",index=" << stage.index;
    os << ",size=" << stage.size;
    os << ",port=" << stage.port;
    os << ",hit=" << stage.hit;
    os << ",block=" << stage.block;
    os << ",ready=" << stage.ready;
    os << ",line=" << stage.line;
    os << ",set=" << stage.set;
    ;
    os << ",evicted=" << stage.evicted;
    os << ",evictedaddress=" << hexa(stage.evictedaddress);
    os << ",uid=" << stage.uid;
    os << ",linesize=" << nLineSize;
    os << ",linedata={";
    for(i = 0; i < nLineSize; i++)
    { char buf[10];
      sprintf(buf,"%02hhx",stage.linedata[i]);
      os << buf;
      if(i<nLineSize-1) os << ",";
    }
    os << "}";

    os << ",data={";
    for(i = 0; i < nCPUDataPathSize; i++)
    { char buf[10];
      sprintf(buf,"%02hhx",stage.data[i]);
      os << buf;
      if(i<nCPUDataPathSize-1) os << ",";
    }
    os << "}";
    return os;
  }

  /**
   * \brief Test for equal
   */
  int operator == (const CachePipeStage<INSTRUCTION, nLineSize, nStages, nCPUDataPathSize>& stage) const
  { return stage.instr == instr;
  }
};

/* A C++ class (template) modelling a multiported pipeline entry */
template <class INSTRUCTION,
int nLineSize,
int nStages,
int nCPUDataPathSize,
int nPorts>
class CachePipeStageMultiPorted
{
public:
  INSTRUCTION instr[nPorts];              ///< the instruction
  int delay[nStages];                     ///< delay for each pipe stage
  int stage;                              ///< the stage in which the request is in the cache pipeline
  char data[nPorts][nCPUDataPathSize];    ///< the requested data if the access was a hit
  char linedata[nPorts][nLineSize];       ///< Data of the coressponding cache line
  bool write[nPorts];                     ///< true if it was a store access
  address_t address[nPorts];              ///< address that the instruction is looking for
  address_t base[nPorts];                 ///< Corresponding base address
  address_t index[nPorts];                ///< Corresponding index
  int size[nPorts];                       ///< size of the data requested
  int port[nPorts];                       ///< port id that made the request
  bool hit[nPorts];                       ///< Wehter or not the request is a hit
  bool block[nPorts];                     ///< Corresponding block
  bool ready[nPorts];                     ///< true if it contains data to be returned to the CPU
  int state[nPorts];                      ///< State associated with the request in the queue (READ_HIT_READY, ...)
  int line[nPorts];                       ///< Associated cache line
  int set[nPorts];                        ///< Associated cache set
  bool evicted[nPorts];                   ///< Wether or not the request imply an eviction
  address_t evictedaddress[nPorts];       ///< Associated eviction address
  uint64_t fvcdata[nPorts];               ///< fields needed for the data in the fvc in case of eviction
  bool fvchit[nPorts];                    ///< fields needed for the data in the fvc in case of hit
  int outPort[nPorts];                    ///< ???
  bool sent[nPorts];                      ///< ???
  char fvclinedata[nPorts][nLineSize];    ///< fields needed for the data in the fvc in case of eviction
  uint64_t fvcaddress[nPorts];            ///< fields needed for the data in the fvc in case of eviction
  int tag[nPorts];                        ///< Corresponding cache tag

  /**
   * \brief Class constructor
   */
  CachePipeStageMultiPorted()
  { int i, lindex;
    for(i = 0; i < nStages; i++)
    { delay[i] = 0;
    }
    stage = 0;
    for(lindex = 0; lindex < nPorts; lindex++)
    {
      for(i = 0; i < nCPUDataPathSize; i++)
      {
        data[lindex][i] = 0;
      }
      for(i = 0; i < nLineSize; i++)
      {
        linedata[lindex][i] = 0;
      }
      write[lindex] = false;
      address[lindex] = 0;
      base[lindex] = 0;
      index[lindex] = 0;
      size[lindex] = 0;
      port[lindex] = 0;
      hit[lindex] = false;
      block[lindex] = false;
      ready[lindex] = false;
      state[lindex] = 0;
      line[lindex] = 0;
      set [lindex] = 0;
      evicted[lindex] = false;
      evictedaddress[lindex] = 0;
      fvcdata[lindex] = 0;
      fvchit[lindex] = false;
      for(i=0; i < nLineSize; i++)
      { fvclinedata[lindex][i] = 0;
      }
      fvcaddress[lindex] = 0;
      tag[lindex] = 0;
      outPort[lindex] = -1;
      sent[lindex] = false;
    }
  }

  /**
   * \brief Pretty printer of the CachePipeStageMultiPorted class
   */
  friend ostream& operator << (ostream& os, const CachePipeStageMultiPorted<INSTRUCTION, nLineSize, nStages, nCPUDataPathSize, nPorts>& stage)
  { int i, index;

    os << "Delay=";
    for(i = 0; i < nStages; i++)
    {
      os << stage.delay[i];
      if(i < nStages - 1)
        os << " ";
    }
    os << ",Stage=" << stage.stage;

    for(index = 0; index < nPorts; index++)
    {
      if(stage.state[index] == NOTHING)
      {
        os << endl << "\tNOTHING";
      }
      else
      {
        os << endl << "\tinstruction={" << stage.instr[index] << "}";
        /*
        	os << ",data=";
        	for(i = 0; i < stage.size; i++) {
        	os << hexa(stage.data[i]);
        	if(i < stage.size - 1) os << " ";
        	}
        */
        os << ",write=" << stage.write[index];
        os << ",address=" << hexa(stage.address[index]);
        os << ",base=" << hexa(stage.address[index]);
        os << ",index=" << stage.index[index];
        os << ",size=" << stage.size[index];
        os << ",port=" << stage.port[index];
        os << ",hit=" << stage.hit[index];
        os << ",block=" << stage.block[index];
        os << ",ready=" << stage.ready[index];
        os << ",state=";
        switch(stage.state[index])
        {
            case READ:
            os << "READ";
            break;
            case READ_HIT_READY:
            os << "READ_HIT_READY";
            break;
            case READ_MISS:
            os << "READ_MISS";
            break;
            case READ_MISS_REQUESTED:
            os << "READ_MISS_REQUESTED";
            break;
            case READ_MISS_READY:
            os << "READ_MISS_READY";
            break;
            case READ_MISS_LINE_READY:
            os << "READ_MISS_LINE_READY";
            break;
            case READ_MISS_SERVICED:
            os << "READ_MISS_SERVICED";
            break;
            case READ_MISS_EVICTION:
            os << "READ_MISS_EVICTION";
            break;
            case READ_MISS_EVICTION_WRITE_DONE:
            os << "READ_MISS_EVICTION_WRITE_DONE";
            break;
            case READ_MISS_EVICTION_WRITE_DONE_REQUESTED:
            os << "READ_MISS_EVICTION_WRITE_DONE_REQUESTED";
            break;
            case READ_MISS_EVICTION_WRITE_DONE_READY:
            os << "READ_MISS_EVICTION_WRITE_DONE_READY";
            break;
            case READ_MISS_EVICTION_WRITE_DONE_LINE_READY:
            os << "READ_MISS_EVICTION_WRITE_DONE_LINE_READY";
            break;
            case READ_MISS_EVICTION_WRITE_DONE_SERVICED:
            os << "READ_MISS_EVICTION_WRITE_DONE_SERVICED";
            break;
            case READ_MISS_WAITING_ACCEPT:
            os << "READ_MISS_WAITING_ACCEPT";
            break;
            case READ_MISS_EVICTION_WAITING_ACCEPT:
            os << "READ_MISS_EVICTION_WAITING_ACCEPT";
            break;
            case READ_MISS_EVICTION_WRITE_DONE_WAITING_ACCEPT:
            os << "READ_MISS_EVICTION_WRITE_DONE_WAITING_ACCEPT";
            break;
            case READ_MSHR:
            os << "READ_MSHR";
            break;
            case WRITE:
            os << "WRITE";
            break;
            case WRITE_MISS:
            os << "WRITE_MISS";
            break;
            case WRITE_MISS_REQUESTED:
            os << "WRITE_MISS_REQUESTED";
            break;
            case WRITE_MISS_EVICTION:
            os << "WRITE_MISS_EVICTION";
            break;
            case WRITE_MISS_EVICTION_WRITE_DONE:
            os << "WRITE_MISS_EVICTION_WRITE_DONE";
            break;
            case WRITE_MISS_EVICTION_WRITE_DONE_REQUESTED:
            os << "WRITE_MISS_EVICTION_WRITE_DONE_REQUESTED";
            break;
            case WRITE_MISS_NO_READ:
            os << "WRITE_MISS_NO_READ";
            break;
            case WRITE_MISS_WAITING_ACCEPT:
            os << "WRITE_MISS_WAITING_ACCEPT";
            break;
            case WRITE_MISS_EVICTION_WAITING_ACCEPT:
            os << "WRITE_MISS_EVICTON_WAITING_ACCEPT";
            break;
            case WRITE_MISS_EVICTION_WRITE_DONE_WAITING_ACCEPT:
            os << "WRITE_MISS_EVICTION_WRITE_DONE_WAITING_ACCEPT";
            break;
            case WRITE_MISS_NO_READ_WAITING_ACCEPT:
            os << "WRITE_MISS_NO_READ_WAITING_ACCEPT";
            break;
            case WRITE_MSHR:
            os << "WRITE_MSHR";
            break;
            case WRITE_HIT_FLUSH:
            os << "WRITE_HIT_FLUSH";
            case WRITE_HIT_FLUSH_REQUESTED:
            os << "WRITE_HIT_FLUSH_REQUESTED";
            break;
        }
        os << ",line=" << stage.line[index];
        os << ",set=" << stage.set[index];
        os << ",evicted=" << stage.evicted[index];
        os << ",evictedaddress=" << hexa(stage.evictedaddress[index]);
        os << ",tag=" << stage.tag[index];
        os << ",sent=" << stage.sent[index];
        os << ",outPort=" << stage.outPort[index];
      }
    }
    os << endl;
    return os;
  }

  /**
   * \brief Test for equal
   */
  int operator == (const CachePipeStageMultiPorted<INSTRUCTION, nLineSize, nStages, nCPUDataPathSize, nPorts>& stage) const
  { bool egal;
    int index;
    for(egal = true, index = 0; egal && (index < nPorts); index++)
    { egal = (stage.instr[index] == instr[index]);
    }
    return egal;
  }
};

template <int nLineSize>
class DelayedWriteBuffer
{public:
  uint8_t buffer[nLineSize];   ///< the received data from the cache line
  address_t address;           ///< the address of the cache line being received
  int size;                    ///< bytes already received from the memory system
  bool ready[nLineSize];       ///< bytes already received from the memory system
  int set;                     ///< Corresponding cache set
  int line;                    ///< Corresponding cache line
  bool write;                  ///< True it the request is modifying the memory (write request)
  int uid;                     ///< Associated uid
  uint64_t memreq_id;          ///< Associated unique memreq ID
  
  bool shared_bit;             ///< Value of the shared bit when the request was received
  bool cachable;               ///< Wether or not the request is cachable

  /**
   * \brief Class constructor
   */
  DelayedWriteBuffer()
  { Reset();
  }

  /**
   * \brief Reset the delayed write buffer
   */
  void Reset()
  { for(int i = 0; i < nLineSize; i++)
    { buffer[i] = 0;
      ready[i] = false;
    }
    address = 0;
    size = 0;
    set  = 0;
    line = 0;
    write = false;
    shared_bit = false;
    cachable = true;
    memreq_id = 0;
  }

  /**
   * \brief Pretty printer of the DelayedWriteBuffer class
   */
  friend ostream& operator << (ostream& os, const DelayedWriteBuffer<nLineSize>& dwb)
  { os << "buffer={" << hex << right;
    //    for(i = 0; i < dwb.size; i++)
    for(int i = 0; i < nLineSize; i++)
    { //os << hexa(dwb.buffer[i]);
      int tmp = dwb.buffer[i];
      os << setfill('0') << setw(2) << tmp;
      if(i < dwb.size - 1) os << ",";
    }
    os << dec << setfill(' ') << "}";
    os << ",address=" << hexa(dwb.address);
    os << ",size=" << dwb.size;
    os << ",ready=";
    for(int i = 0; i < nLineSize; i++) os << dwb.ready[i];
    os << ",set=" << dwb.set;
    os << ",line=" << dwb.line;
    os << ",write=" << dwb.write;
    return os;
  }
};

} // end of namespace cache
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif // __CACHECOMMON_H__
