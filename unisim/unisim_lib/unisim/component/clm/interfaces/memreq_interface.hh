/***************************************************************************
        memreq_interface.h  -  Defines common memory ports interface
***************************************************************************/

/* *****************************************************************************  
                                    BSD LICENSE  
********************************************************************************  
Copyright (c) 2006, Commissariat a l'Energie Atomique (CEA) & INRIA
Authors: Sylvain Girbal (sylvain.girbal@inria.fr)
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

#ifndef __UNISIM_COMPONENT_CLM_INTERFACES_MEMREQ_INTERFACE_H__
#define __UNISIM_COMPONENT_CLM_INTERFACES_MEMREQ_INTERFACE_H__

//#include "unisim/unisim.h"
#include <unisim/kernel/clm/engine/include/unisim/unisim.h>
// TODO : moving services...
#include <utils/services/service.hh>
//#include "memreq.h"
#include <unisim/component/clm/interfaces/memreq.hh>
#include <generic/memory/memory_interface.hh>

namespace unisim {
namespace component {
namespace clm {
namespace interfaces {

using full_system::utils::services::ServiceExport;
using full_system::utils::services::ServiceImport;
using full_system::utils::services::Service;
using full_system::utils::services::Client;
using full_system::utils::services::Object;
using full_system::generic::memory::MemoryInterface;

template 
< class INSTRUCTION 
, int inDataPathSize
, int outDataPathSize
>
class inMEMREQitf : public Service < MemoryInterface < address_t > >
{public:
  ServiceExport < MemoryInterface < address_t > > fromCPU;
  inport  < memreq < INSTRUCTION, inDataPathSize > > inCPU;   ///< Input port for requests
  outport < memreq < INSTRUCTION, outDataPathSize > > outCPU;  ///< Output port for sending data toward CPU

  /**
   * \brief Module constructor
   */
  inMEMREQitf(const char *name, unisim_module *mod, Object *obj)
                          : Object(name, 0)
                          , Service < MemoryInterface < address_t > >(name, obj)
                          , fromCPU("fromCPU", obj)
  { // Naming the interface ot enhance debugging
    inCPU.set_unisim_name(mod,"¤inCPU");
    outCPU.set_unisim_name(mod,"¤outCPU");
  }
};

template 
< class INSTRUCTION 
, int inDataPathSize
, int outDataPathSize
>
class outMEMREQitf : public Client < MemoryInterface < address_t > >
{public:
  ServiceImport < MemoryInterface < address_t > > toMEM;
  inport  < memreq < INSTRUCTION, inDataPathSize> > inMEM;   ///< Input port for answers data
  outport < memreq < INSTRUCTION, outDataPathSize > > outMEM;  ///< Output port for sending request toward memory hierarchy

  /**
   * \brief Module constructor
   */
  outMEMREQitf(const char *name, unisim_module *mod, Object *obj)
                           : Object(name, 0)
                           , Client < MemoryInterface < address_t > >(name, obj)
                           , toMEM("toMEM", obj)
  { // Naming the interface ot enhance debugging
    inMEM.set_unisim_name(mod,"¤inMEM");
    outMEM.set_unisim_name(mod,"¤outMEM");
  }

  /**
   * \brief Connecting a outMEMREQitf to a inMEMREQitf, connecting all the related 
   * UNISIM and service ports.
   */
  void operator>>(const inMEMREQitf <INSTRUCTION, inDataPathSize , outDataPathSize> &i)
  { outMEM >> i.inCPU;
    i.outCPU >> inMEM;
    toMEM >> i.fromCPU;
  }
};

} // end of namespace interfaces
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
