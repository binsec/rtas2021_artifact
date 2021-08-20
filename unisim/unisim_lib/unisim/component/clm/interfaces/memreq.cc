/***************************************************************************
   memreq.cc  -  Memory request object
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

#include <unisim/component/clm/interfaces/memreq.hh>
#include <unisim/component/clm/interfaces/memreq.tcc>

namespace unisim {
namespace component {
namespace clm {
namespace interfaces {

  using unisim::component::clm::memory::ByteArray;
  using namespace std;

/**
 * \brief Non-templated type class for memreq defining the enums and their pretty printers
 */
  /** 
   * \brief Pretty printer for the command_t enum 
   */
  ostream & operator<<(ostream &os, const memreq_types::command_t &c)
  { switch(c)
    { case memreq_types::cmd_UNKNOWN:
        os << "UNK  ";
        break;
      case memreq_types::cmd_READ:
        os << "READ ";
        break;
      case memreq_types::cmd_READX:
        os << "READX";
        break;
      case memreq_types::cmd_WRITE:
        os << "WRITE";
        break;
      case memreq_types::cmd_PREFETCH:
        os << "PREFETCH";
        break;
      case memreq_types::cmd_EVICT:
        os << "EVICT";
        break;
      case memreq_types::cmd_FLUSH:
        os << "FLUSH";
        break;
      case memreq_types::cmd_BLOCK_INVALIDATE:
        os << "BLOCK_INVALIDATE";
        break;
    }
    return os;
  }

  /** 
   * \brief Pretty printer for the sender_type_t enum 
   */
  ostream & operator<<(ostream &os, const memreq_types::sender_type_t &t)
  { switch(t)
    { case memreq_types::sender_UNKNOWN: 
        os << "\e[1;35mUNK\e[0m";
        break;
      case memreq_types::sender_CPU:
        os << "\e[1;31mCPU\e[0m";
        break;
      case memreq_types::sender_CACHE:
        os << "\e[1;33mCAC\e[0m";
        break;
      case memreq_types::sender_MEM:
        os << "\e[1;34mMEM\e[0m";
        break;
    }
    return os;
  }

  /** 
   * \brief Pretty printer for the sender_type_t enum 
   */
  ostream & operator<<(ostream &os, const memreq_types::message_type_t &m)
  { switch(m)
    { case memreq_types::type_UNKNOWN: 
//        os << "\e[1;35mUNK\e[0m";
        os << "UNK";
        break;
      case memreq_types::type_REQUEST:
//        os << "\e[1;32mREQ\e[0m";
        os << "REQ";
        break;
      case memreq_types::type_ANSWER:
//        os << "\e[1;31mANS\e[0m";
        os << "ANS";
        break;
    }
    return os;
  }
  
  //static uint64_t memreq_types::memreq_id_max;

  //uint64_t memreq_types::memreq_id_max=0;
  uint64_t memreq_types::memreq_id_max=0;

} // end of namespace interfaces
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim
