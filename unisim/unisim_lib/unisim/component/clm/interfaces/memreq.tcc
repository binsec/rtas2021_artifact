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

#ifndef __UNISIM_COMPONENT_CLM_INTERFACES_MEMREQ_TCC__
#define __UNISIM_COMPONENT_CLM_INTERFACES_MEMREQ_TCC__

#include <unisim/component/clm/interfaces/memreq.hh>

namespace unisim {
namespace component {
namespace clm {
namespace interfaces {

  using unisim::component::clm::memory::ByteArray;
  using namespace std;

/**
 * \brief Data-less memory request class
 */
  /** 
   * \brief Creates a new memory request 
   */
  template <class INSTRUCTION>
  memreq_dataless<INSTRUCTION>::
  memreq_dataless(): memreq_types()
  { size=0;
    address=0;
    command=cmd_UNKNOWN;
    uid=0;
    sender_type=sender_UNKNOWN;
    message_type=type_UNKNOWN;
    sender = NULL;
    req_sender = NULL;
    cachable = true;

    memreq_id = memreq_id_max++;
    valid = false;
#ifdef NOC_THREADS_DISTRIBUTION
    dst_id = 0;
    src_id = 0;
#endif
  }

  template <class INSTRUCTION>
  memreq_dataless<INSTRUCTION>::
  ~memreq_dataless() {}

  /** 
   * \brief Pretty printer for the memreq class 
   */
  template <class INSTRUCTION>
  ostream & operator<<(ostream &os, const memreq_dataless<INSTRUCTION> &req)
  { os << "MEMREQ [ ";
    os << req.command << ", ";
    os << "sender_type=" << req.sender_type << ", ";
    os << "msg_type=" << req.message_type << ", ";
    if(req.sender) os << "sender=" << setw(8) << req.sender->name() << ", ";
    else           os << "sender=" << setw(8) << "NULL" << ", ";
    if(req.req_sender) os << "req_sender=" << setw(8) << req.req_sender->name() << ", ";
    else               os << "req_sender=" << setw(8) << "NULL" << ", ";
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

  template <class INSTRUCTION>
  const char * 
  memreq_dataless<INSTRUCTION>::
  Read() const
  { return NULL;
  }

/**
 * \brief Memory request class
 */
  /** 
   * \brief Creates a new memory request 
   */
  template <class INSTRUCTION, int DATASIZE>
  memreq<INSTRUCTION,DATASIZE>::
  memreq() : memreq_dataless <INSTRUCTION>()
  { 
  }

  /** 
   * \brief Pretty printer for the memreq class 
   */
  template <class INSTRUCTION, int DATASIZE>
  ostream & operator<<(ostream &os, const memreq<INSTRUCTION, DATASIZE> &req)
  { os << "MEMREQ [ " << left;
    os << req.command << ", ";
    os << "sndr_type=" << req.sender_type << ", ";
    os << "msg_type=" << req.message_type << ", ";
        if(req.sender) os << "sndr=" << setw(8) << req.sender->name() << ", ";
        else           os << "sndr=" << setw(8) << "NULL" << ", ";
	//os << "sndr=" << setw(8) << req.sender << ", ";
        if(req.req_sender) os << "req_sndr=" << setw(8) << req.req_sender->name() << ", ";
        else               os << "req_sndr=" << setw(8) << "NULL" << ", ";
	//os << "req_sndr=" << setw(8) << req.req_sender << ", ";
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
  template <class INSTRUCTION, int DATASIZE>
  const char * 
  memreq<INSTRUCTION,DATASIZE>::
  Read() const
  { return (const char *)data;
  }

  /**
   * \brief Write to the message data
   */
  template <class INSTRUCTION, int DATASIZE>
  void
  memreq<INSTRUCTION,DATASIZE>::
  Write(const char *buf, int num_bytes)
  { memcpy(data.buffer, buf, num_bytes);
  }

  /** 
   * \brief Returns the data of the memreq as a 32bit value
   */
  template <class INSTRUCTION, int DATASIZE>
  uint32_t
  memreq<INSTRUCTION,DATASIZE>::
  Read32() const
  { int result;
    memcpy(&result,data.buffer,4);
    return result;
  }

  /**
   * \brief Set the data of the memreq as a 32bit value
   */
  template <class INSTRUCTION, int DATASIZE>
  void
  memreq<INSTRUCTION,DATASIZE>::
  Write32(uint32_t val)
  { memcpy(data.buffer, &val, 4);
  }


} // end of namespace interfaces
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif

// extra fields: 
//  - source_address
//  - hit latency

