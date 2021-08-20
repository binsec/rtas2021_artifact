/***************************************************************************
   memreq.h  -  Memory request object
 ***************************************************************************/

/* *****************************************************************************  
                                    BSD LICENSE  
********************************************************************************  
Copyright (c) 2006, INRIA
Authors: Sylvain Girbal (sylvain.girbal@inria.fr)
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

#ifndef __UNISIM_COMPONENT_CLM_INTERFACES_MEMREQ_HH__
#define __UNISIM_COMPONENT_CLM_INTERFACES_MEMREQ_HH__

//#include <unisim/unisim_inttypes.h>
#include <unisim/kernel/clm/engine/unisim_inttypes.h>
#include <iostream>
#include <unisim/component/clm/memory/mem_common.hh>


namespace unisim {
namespace component {
namespace clm {
namespace interfaces {

  using unisim::component::clm::memory::ByteArray;

using namespace std;

/**
 * \brief Non-templated type class for memreq defining the enums and their pretty printers
 */
class memreq_types
{public:
  enum command_t                ///  Command of the memory request (READ, WRITE, ...) 
  { cmd_UNKNOWN,                ///< Unset request type 
    cmd_READ,                   ///< Read request / answer
    cmd_READX,                  ///< Exclusive read request / answer
    cmd_WRITE,                  ///< Write request
    cmd_PREFETCH,               ///< Prefetch request / answer
    cmd_EVICT,                  ///< Evict request
    cmd_FLUSH,                  ///< Cache flush line request
    cmd_BLOCK_INVALIDATE        ///< Block invalidate request
  };
  enum sender_type_t            ///  Type of the module which has sent the request (CPU, CACHE, MEMORY)
  { sender_UNKNOWN,             ///< Unset sender type
    sender_CPU,                 ///< Request / answer issued by a CPU
    sender_CACHE,               ///< Request / answer issued by a CACHE
    sender_MEM                  ///< Answer issued by the Memory
  };
  enum message_type_t           ///  Type of the message (REQUEST, ANSWER)
  { type_UNKNOWN,               ///< Unset message type
    type_REQUEST,               ///< Request message
    type_ANSWER                 ///< Answer message
  };
  /*
class sender_uid_type_t
  {
  public:
    sender_uid_type_t(module *s,uint32_t id): sender(s), uid(id) {}
    module *sender;
    uint32_t uid;
    bool operator == (const sender_uid_type_t& suid)
      {
	return ((sender == suid.sender) && (uid == suid.uid));
      }
    bool operator != (const sender_uid_type_t& suid)
      {
	return !(*this == suid);
      }
  };
  */

  /** 
   * \brief Pretty printer for the command_t enum 
   */
  friend ostream & operator<<(ostream &os, const command_t &c)
  { switch(c)
    { case cmd_UNKNOWN:
        os << "UNK  ";
        break;
      case cmd_READ:
        os << "READ ";
        break;
      case cmd_READX:
        os << "READX";
        break;
      case cmd_WRITE:
        os << "WRITE";
        break;
      case cmd_PREFETCH:
        os << "PREFETCH";
        break;
      case cmd_EVICT:
        os << "EVICT";
        break;
      case cmd_FLUSH:
        os << "FLUSH";
        break;
      case cmd_BLOCK_INVALIDATE:
        os << "BLOCK_INVALIDATE";
        break;
    }
    return os;
  }

  /** 
   * \brief Pretty printer for the sender_type_t enum 
   */
  friend ostream & operator<<(ostream &os, const sender_type_t &t)
  { switch(t)
    { case sender_UNKNOWN: 
        os << "\e[1;35mUNK\e[0m";
        break;
      case sender_CPU:
        os << "\e[1;31mCPU\e[0m";
        break;
      case sender_CACHE:
        os << "\e[1;33mCAC\e[0m";
        break;
      case sender_MEM:
        os << "\e[1;34mMEM\e[0m";
        break;
    }
    return os;
  }

  /** 
   * \brief Pretty printer for the sender_type_t enum 
   */
  friend ostream & operator<<(ostream &os, const message_type_t &m)
  { switch(m)
    { case type_UNKNOWN: 
//        os << "\e[1;35mUNK\e[0m";
        os << "UNK";
        break;
      case type_REQUEST:
//        os << "\e[1;32mREQ\e[0m";
        os << "REQ";
        break;
      case type_ANSWER:
//        os << "\e[1;31mANS\e[0m";
        os << "ANS";
        break;
    }
    return os;
  }

  static uint64_t memreq_id_max;   ///< Maximum memreq_id used so far. 
};


/**
 * \brief Data-less memory request class
 */
template <class INSTRUCTION>
class memreq_dataless : public memreq_types
{public:
  INSTRUCTION instr;           ///< The incoming instruction
  uint32_t address;            ///< Address of the memory access
  int size;                    ///< Size of the retrieved data in bytes
  command_t command;           ///< Type of the memory request (READ, WRITE, ...)
  uint64_t uid;                ///< Unique ID fields used by multi-port caches

  uint64_t memreq_id;              ///< Unique ID. Is kept the same for the corresponding answer.
  
  sender_type_t sender_type;   ///< Type of the sender (CPU, CACHE, MEMORY, ...)
  message_type_t message_type; ///< Type of the message (Request, Answser)
  //  module *sender;              ///< Module that has sent this message
  //  sender_uid_type_t sender;
  string sender;
  //  module *req_sender;          ///< Module that has sent the request this message is about
  //  sender_uid_type_t req_sender;
  string req_sender;
  bool cachable;               ///< Wether the request address is cachable

  bool valid;  //< Used with unisim signal array when something() can't be used
#ifdef NOC_THREADS_DISTRIBUTION
  int dst_id;                  ///< destination tile id
  int src_id;                  ///< source tile id
#endif

  /** 
   * \brief Creates a new memory request 
   */
  memreq_dataless()
  { size=0;
    address=0;
    command=cmd_UNKNOWN;
    uid=0;
    sender_type=sender_UNKNOWN;
    message_type=type_UNKNOWN;
    //    sender = NULL;
    //    req_sender = NULL;
    sender = "";
    req_sender = "";
    cachable = true;
    memreq_id = memreq_id_max++;
    valid = false;
#ifdef NOC_THREADS_DISTRIBUTION
    dst_id = 0;
    src_id = 0;
#endif
  }

  /** 
   * \brief Pretty printer for the memreq class 
   */
  friend ostream & operator<<(ostream &os, const memreq_dataless &req)
  { os << "MEMREQ [ ";
    os << req.command << ", ";
    os << "sender_type=" << req.sender_type << ", ";
    os << "msg_type=" << req.message_type << ", ";
    //    if(req.sender) os << "sender=" << setw(8) << req.sender->name() << ", ";
    //    else           os << "sender=" << setw(8) << "NULL" << ", ";
    os << "sender=" << setw(8) << req.sender << ", ";
    //    if(req.req_sender) os << "req_sender=" << setw(8) << req.req_sender->name() << ", ";
    //    else               os << "req_sender=" << setw(8) << "NULL" << ", ";
    os << "req_sender=" << setw(8) << req.req_sender << ", ";

    os << "@=0x" << hex << req.address << dec << ", ";
    os << "sz=" << setw(2) << req.size << ", ";
    os << "uid=" << req.uid << ", ";
    os << req.instr.disasm() << ", ";
    if(req.cachable) os << "cachable ";
    else os << "not cachable ";
    os << "]";
    return os;
  }

  /** 
   * \brief Returns the data of the memreq
   */
  virtual const char * Read() const
  { return NULL;
  }

};

/**
 * \brief Memory request class
 */
template <class INSTRUCTION, int DATASIZE>
class memreq : public memreq_dataless <INSTRUCTION>
{public:
  ByteArray<DATASIZE> data;    ///< The data in the message
  
  /** 
   * \brief Creates a new memory request 
   */
  memreq() : memreq_dataless <INSTRUCTION>()
  { 
  }

  /** 
   * \brief Pretty printer for the memreq class 
   */
  friend ostream & operator<<(ostream &os, const memreq &req)
  { os << "MEMREQ [ " << left;
    os << req.command << ", ";
    os << "sndr_type=" << req.sender_type << ", ";
    os << "msg_type=" << req.message_type << ", ";
    //    if(req.sender) os << "sndr=" << setw(8) << req.sender->name() << ", ";
    //    else           os << "sndr=" << setw(8) << "NULL" << ", ";
    os << "sndr=" << setw(8) << req.sender << ", ";
    //    if(req.req_sender) os << "req_sndr=" << setw(8) << req.req_sender->name() << ", ";
    //    else               os << "req_sndr=" << setw(8) << "NULL" << ", ";
    os << "req_sndr=" << setw(8) << req.req_sender << ", ";
    os << "@=0x" << hex << setw(8) << req.address << dec << ", ";
    os << "sz=" << setw(2) << req.size << ", ";
//    os << "memreq_id=" << req.memreq_id << ", ";
//    if(req.message_type==memreq_types::type_ANSWER) os << "data=" << req.data << ", ";
//    os << req.instr << " ";
//    req.instr.operation->disasm(os);
    if(req.instr->operation)
    { os << "instr={0x" << hex << req.instr->cia << dec << ": ";
      req.instr->operation->disasm(0,os);
      os << "}";
    }
    os << " uid=" << req.uid << ", ";
#ifdef NOC_THREADS_DISTRIBUTION
    os << " dst_id =" << req.dst_id<<", ";
    os << " src_id=" << req.src_id<<", ";
#endif
    if(req.cachable) os << "cachable ";
    else os << "not cachable ";
    os << " ]";
    if( (req.message_type==memreq_types::type_ANSWER) || (req.command==memreq_types::cmd_WRITE) )
    { os << " data=" << req.data;
    }
    return os;
  }


  /**
   * \brief Read from the message data
   */
  const char * Read() const
  { return (const char *)data;
  }

  /**
   * \brief Write to the message data
   */
  void Write(const char *buf, int num_bytes)
  { memcpy(data.buffer, buf, num_bytes);
  }

  /** 
   * \brief Returns the data of the memreq as a 32bit value
   */
  uint32_t Read32() const
  { int result;
    memcpy(&result,data.buffer,4);
    return result;
  }

  /**
   * \brief Set the data of the memreq as a 32bit value
   */
  void Write32(uint32_t val)
  { memcpy(data.buffer, &val, 4);
  }

};

uint64_t memreq_types::memreq_id_max;

} // end of namespace interfaces
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif

// extra fields: 
//  - source_address
//  - hit latency

