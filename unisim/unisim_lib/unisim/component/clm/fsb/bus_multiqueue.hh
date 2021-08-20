/* *****************************************************************************  
       bus_multiqueue.sim  -  Cycle-level memory bus supporting snooping.
           Separate queues for cpu requests/answers and memory answers
***************************************************************************** */

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

#ifndef __UNISIM_COMPONENT_CLM_FSB_BUS_HH__
#define __UNISIM_COMPONENT_CLM_FSB_BUS_HH__


#include <unisim/component/clm/interfaces/memreq.hh>
#include <unisim/component/clm/utility/error.h>
#include <unisim/component/clm/utility/utility.hh>
#include <sstream>
#include <iostream>

#include <unisim/component/clm/debug/svg_memreq_interface.hh>
#include <unisim/kernel/kernel.hh>

namespace unisim {
namespace component {
namespace clm {
namespace fsb {

using unisim::component::clm::interfaces::memreq;
using unisim::component::clm::interfaces::memreq_types;

using unisim::component::clm::debug::SVGmemreqInterface;

using unisim::kernel::Client;
using unisim::kernel::ServiceImport;

using unisim::kernel::Object;



//#define DD_DEBUG_BUS
#ifdef NOC_SUPPORT_PARALLELIZATION 
#define THREAD_ADDRESS      0xFF000000
#define THREAD_DESTINATION  0xFF
#endif

template 
< class INSTRUCTION,
  int nCPU, 
  int BufferSize, 
  int RequestWidth,
  bool Snooping = false
#ifdef DISTRIBUTED_SHARED_SYSTEM
  ,int nbMem = 1
#endif  
>
class BusMultiQueue : public module, public Client<SVGmemreqInterface<INSTRUCTION> >
{private:
  struct BufferLine
  { memreq < INSTRUCTION, RequestWidth > req;
    int sender_id;   // Used not to send the data to the module sending it
    friend ostream & operator<<(ostream &os, const BufferLine &bl)
    { os << bl.req << " | sender_id=" << bl.sender_id;
      return os;
    }
  };
  bool sent_cpu_data;
  bool sent_mem_data;
  bool enabled_cpu_data;
  bool enabled_mem_data;

 public:
  /******************/
  /* Clocks & Ports */
  /******************/
  //Bus clock
  inclock clock;                                                 ///< Clock port
  // Memory request & data connections
  inport  < memreq < INSTRUCTION, RequestWidth > > inCPU[nCPU];  ///< CLM Port receiving requests from CPUs or caches
  outport < memreq < INSTRUCTION, RequestWidth > > outCPU[nCPU]; ///< CLM Port sending answers to CPUs or caches
  inport  < memreq < INSTRUCTION, RequestWidth > > inMEM;        ///< CLM Port receiving answes from the memory
  outport < memreq < INSTRUCTION, RequestWidth > > outMEM;       ///< CLM Port sending requests to the memory

  inport  <bool,Snooping> inSharedCPU[nCPU];    ///< Optional CLM Port receiving the shared bits from the CPUs
  outport <bool,Snooping> outSharedMEM;         ///< Optional CLM Port sendiing the shared bit to the memory

  /*****************/
  /* Service Ports */
  /*****************/
  ServiceImport<SVGmemreqInterface<INSTRUCTION> > svg;        ///< Service port to the SVG service

  /*********************/
  /* State Information */
  /*********************/

  Queue <BufferLine,BufferSize> cpu_queue;  ///< Bufferized mesages to be sent by the bus from the CPUs
  Queue <BufferLine,BufferSize> mem_queue;  ///< Bufferized mesages to be sent by the bus from the MEMs
  int cpu_round_robin_index;                ///< Index for the round robin algorithm on the CPU side

  /**
   * \brief Create a new Bus module
   */
 #ifdef DISTRIBUTED_SHARED_SYSTEM
  int local_id;
  BusMultiQueue(const char *name,int id) 
  : module(name),local_id(id)
 #else	
  BusMultiQueue(const char *name) 
  : module(name)
 #endif	
  , Object(name, 0)
  , Client<SVGmemreqInterface<INSTRUCTION> >(name, this)
  , svg("svg", this)
  { //Interface naming to enhance debugging info
    class_name = " BusMultiQueue";
    category = category_INTERCONNECT;
    inMEM.set_unisim_name(this,"inMEM");
    outMEM.set_unisim_name(this,"outMEM");
    outSharedMEM.set_unisim_name(this,"outSharedMEM");
    for(int i=0;i<nCPU;i++)
    { inCPU[i].set_unisim_name(this,"inCPU",i);
      inSharedCPU[i].set_unisim_name(this,"inSharedCPU",i);
      outCPU[i].set_unisim_name(this,"outCPU",i);
    }
    //State initialization
    cpu_round_robin_index = 0;
    /* process Sensitivity list */
    sensitive_method(on_data) << inMEM.data;
    sensitive_method(on_accept) << outMEM.accept;
    if(Snooping)
    { sensitive_method(on_shared_accept) << outSharedMEM.accept;
    }
    for(int i=0;i<nCPU;i++)
    { sensitive_method(on_data) << inCPU[i].data;
      sensitive_method(on_accept) << outCPU[i].accept;
      if(Snooping)
      { sensitive_method(on_shared_data) << inSharedCPU[i].data;
      }
    }
    sensitive_pos_method(begin_of_cycle) << clock;
    sensitive_neg_method(end_of_cycle) << clock;
    // ---  Registring parameters ----------------
    parameters.add("nCPU",nCPU);
    parameters.add("BufferSize",BufferSize,1,100);
    parameters.add("RequestWidth",RequestWidth);
    parameters.add("Snooping",Snooping);
    // --- Latex rendering hints -----------------
    for(int i=0;i<nCPU;i++)
    { latex_left_ports.push_back(&inCPU[i]);
      latex_left_ports.push_back(&outCPU[i]);
    }
    latex_right_ports.push_back(&outMEM);
    latex_right_ports.push_back(&inMEM);
    // -------------------------------------------
    sent_cpu_data = false;
    sent_mem_data = false;
    enabled_cpu_data = false;
    enabled_mem_data = false;
  }

  /**
   * \brief Process called at the beggining of the cycle.
   *
   * Broadcast a value from the queues.
   */
  void begin_of_cycle()
  {
    sent_cpu_data = false;
    sent_mem_data = false;
    enabled_cpu_data = false;
    enabled_mem_data = false;
    if(!mem_queue.Empty())
    { // Try to send an answer from the memory side to every cpu port
      BufferLine *head = mem_queue.GetHead();
//INFO << "Sending to all cpu: " << head->req << endl;
#ifdef DEBUG_BUS_MQ
INFO << "Sending (M) to all: " << head->req << endl;
#endif
      outMEM.data.nothing();
      for(int i=0;i<nCPU;i++)
      { outCPU[i].data = head->req;
      }  
      sent_mem_data = true;
    }
    else if(!cpu_queue.Empty())
    { // Try to send a new cpu request / answer to every other cpu & memory
      BufferLine *head = cpu_queue.GetHead();
//INFO << "Sending to all cpu + mem: " << head->req << endl;
#ifdef DEBUG_BUS_MQ
INFO << "Sending (C) to all: " << head->req << endl;
#endif
      outMEM.data = head->req;
      for(int i=0;i<nCPU;i++)
      { if((head->sender_id!=i) || is_cpu_flush()) outCPU[i].data = head->req;
        else outCPU[i].data.nothing();
      }  
      sent_cpu_data = true;
    }
    else
    { // Else sends nothing to every output
//INFO << "Sending to all: NOTHING" << endl;
      outMEM.data.nothing();
      for(int i=0;i<nCPU;i++)
      { outCPU[i].data.nothing();
      }  
    }
  }
  
  /**
   * \brief Process called at the end of the cycle.
   *
   * Discard the first bufferized value if all outputs accepted it.
   * 
   * Bufferize new incoming values with higher priority to memory, then
   * round robin between CPUs.
   */
  void end_of_cycle()
  {
#ifdef DD_DEBUG_BUS
    cerr << "[DD_DEBUG_BUS] ------------------------------------> EOF (beginning of end of cycle)<---------------------------------" << endl;
#endif
    //If sent message is accepted remove it from the buffer

    if(enabled_mem_data)
    {
      mem_queue.RemoveHead();
    }
    else if(enabled_cpu_data)
    {
#ifdef DATA_ROUTING_DEBUG
      // zheng tmp test 
      BufferLine *head = cpu_queue.GetHead();
      if(head->req.address == 0x1007d4e0)
        cout<<name() <<"remove :" <<head->req <<endl;
      // zheng tmp test end
#endif
      cpu_queue.RemoveHead();

    }
    else
    { //if(!buffer.Empty()) INFO << "All bus outputs DID NOT accept the data" << endl;
    }
    //If incoming message is enabled store it to the buffer
    if(inMEM.enable)
    { BufferLine bl;
      bl.req = inMEM.data;
      bl.sender_id = nCPU;
      mem_queue << bl;

#ifdef DEBUG_BUS_RECEIVE
INFO << "\e[1;31mMM\e[0m: " << bl.req << endl;
#endif

      if(svg)
      { svg->add_mem_to_bus(timestamp(),name(),"",bl.req);
      }
    }
    for(int i=0;i<nCPU;i++)
    { if(inCPU[i].enable)
      { //memreq<RequestWidth> msg = inCPU[i].data;
        BufferLine bl;
        bl.req = inCPU[i].data;
        bl.sender_id = i;
      #ifdef DISTRIBUTED_SHARED_SYSTEM
	bl.req.dst_id = getMemReqDstId(bl.req.address);
        #ifdef THREAD_ROUTING_DEBUG
	if( ((bl.req.address) & 0XFF000000) == 0XFF000000)
	    cout<<name() <<" receive thread request : " <<bl.req<<" dst :"<<bl.req.dst_id <<endl;
        #endif
      #endif
        cpu_queue << bl;

#ifdef DATA_ROUTING_DEBUG
        // zheng test
        if(i==0)
           cout<<name() <<"receive cpu request : " <<bl.req <<endl;
        if(i==1)
           cout<<name() <<"receive noc data : " <<bl.req <<endl;
        //zheng test end
#endif

#ifdef DEBUG_BUS_RECEIVE
INFO << "\e[1;31mC" << i << "\e[0m: " << bl.req << endl;
#endif
        if(svg)
        { svg->add_cac_to_bus(timestamp(),i+1,name(),"",bl.req);
        }
      }  
    }
  }

  /**
   * \brief Process launched upon receiving a request/data from the CPU & memory, and
   * setting the corrsponding accept signals.
   **/
  void on_data()
  { // Exit if one of the input signal is not known
    if(!inMEM.data.known()) return;
    for(int i=0;i<nCPU;i++)
    { if(!inCPU[i].data.known()) return;
    }

#ifdef DEBUG_BUS_MQ
if(inMEM.data.something())
{ INFO << "Receiving(M):       " << inMEM.data << endl;
}
for(int i=0;i<nCPU;i++)
{ if(inCPU[i].data.something())
  { INFO << "Receiving(" << i << "):       " << inCPU[i].data << endl;
  }
}
#endif

    // If an answer is received from memory side, try to put it in answer queue
    if(inMEM.data.something())
    { inMEM.accept = !mem_queue.Full();
      for(int j=0;j<nCPU;j++)
      { inCPU[j].accept = false;
      }    
      return;
    }
    inMEM.accept = false;

    // Nothing from memory, not a full cpu queue => 
    if(!cpu_queue.Full())
    { for(int k=0+cpu_round_robin_index;k<nCPU+cpu_round_robin_index;k++)
      { int i = k % nCPU;
        if(inCPU[i].data.something())
        { for(int j=0;j<nCPU;j++)
          { if(i==j) inCPU[j].accept = true;
            else     inCPU[j].accept = false;
          }
          cpu_round_robin_index++;
          if(cpu_round_robin_index==nCPU) cpu_round_robin_index = 0;
          return;
        }
      }
    }
    //Every signal is set to nothing, refuse every signal
    for(int j=0;j<nCPU;j++)
    { inCPU[j].accept = false;
    }    
  }

  /**
   * \brief Process launched upon receiving accepts and setting the corrsponding 
   * enable signals.
   **/
  void on_accept()
  { // Exit if one of the input signal is not known
    if(!outMEM.accept.known()) return;
    for(int i=0;i<nCPU;i++)
    { if(!outCPU[i].accept.known()) return;
    }

    if(sent_cpu_data)
    { BufferLine *head = cpu_queue.GetHead();
//      INFO << "data was sent from cpu: " << head->req << endl;
      bool all_accept = outMEM.accept;
      for(int i=0;i<nCPU;i++)
      { if((head->sender_id!=i) || is_cpu_flush()) all_accept &= outCPU[i].accept;
      }
      outMEM.enable = all_accept;
      for(int i=0;i<nCPU;i++)
      { if((head->sender_id!=i) || is_cpu_flush()) outCPU[i].enable = all_accept;
        else outCPU[i].enable = false;
      }
      enabled_cpu_data = all_accept;
    }
    else if(sent_mem_data)
    { //INFO << "data was sent from mem" << endl;
      bool all_accept = true;
      for(int i=0;i<nCPU;i++)
      { all_accept &= outCPU[i].accept;
      }
      outMEM.enable = false;
      for(int i=0;i<nCPU;i++)
      { outCPU[i].enable = all_accept;
      }
      enabled_mem_data = all_accept;      
    }
    else
    { //Nothings were sent, let's disable everyone
      outMEM.enable = false;
      for(int i=0;i<nCPU;i++) outCPU[i].enable = false;
    }
  }

  /**
   * \brief Process launched upon receiving a shared flag from the CPU
   *
   * Shared is set to true on every output, if it set to true to one of the input, and
   * none of the input is nothing.
   */
  void on_shared_data()
  { if(Snooping)
    { // Exit if one of the input signal is not known
      for(int i=0;i<nCPU;i++)
      { if(!inSharedCPU[i].data.known()) return;
      }
      //All the signal are known, check is a message has been sent
      if(!sent_cpu_data && !sent_mem_data)
      { // We did not send any data, we should have nothing on each shared input
        // and send nothing on each shared output
        for(int i=0;i<nCPU;i++)
        { if(inSharedCPU[i].data.something())
          { ERROR << "Every inSharedCPU should be nothing when no data is sent, and inSharedCPU[" << i << "] is " << (inSharedCPU[i].data?"true":"false") << endl;
            exit(1);
          }
        }
        for(int i=0;i<nCPU;i++)
        { //outSharedCPU[i].data.nothing(); caches no more have a shared bit input
          inSharedCPU[i].accept = false;
        }
        outSharedMEM.data.nothing();
        return;
      }    
    
      // A message was sent at start of cycle, check inShared of each CPU it has been sent to
      // None of thoe outShared should be nothing.
      BufferLine *head;
      if(sent_cpu_data) head = cpu_queue.GetHead();
      if(sent_mem_data) head = mem_queue.GetHead();
      bool is_shared = false;

      for(int i=0;i<nCPU;i++)
      { if(i!=head->sender_id)
        { if(!inSharedCPU[i].data.something())
          { ERROR << "Every inSharedCPU should be someting when data is sent" << endl;
            exit(1);
          }
          is_shared |= inSharedCPU[i].data;
        }
      }
      //Send the shared information to the ouput ports
      if(svg)
      { if(is_shared) svg->add_box_info(timestamp(),name(),"S");
      }
      for(int j=0;j<nCPU;j++)
      { inSharedCPU[j].accept = true;
      }
      outSharedMEM.data = is_shared;
    } // if(Snooping)
  }

  /**
   * \brief Process setting the shared bit enable signal
   */
  void on_shared_accept()
  { if(Snooping)
    { if(outSharedMEM.accept.known()) outSharedMEM.enable = outSharedMEM.accept;
    } // if(Snooping)
  }

  /**
   * \brief Module interface : Returns true if the module has some pending operations.
   */
  bool has_pending_ops()
  { if (!cpu_queue.Empty()) return true;
    if (!mem_queue.Empty()) return true;
    return false;
  }

  int get_rank()
  { return inCPU[0].get_connected_module()->get_rank()+1;
  }

  virtual ModuleParameter get_parameter(const string &pname)
  { if(pname=="nLineSize")
    { return inCPU[0].get_connected_module()->get_parameter("nLineSize");
    }
    return module::get_parameter(pname);
  }

 private:
  virtual bool ClientIndependentSetup()
  { if(svg) svg->register_bus(name());
    return true;
  }
  
  inline bool is_cpu_flush()
  { BufferLine *head = cpu_queue.GetHead();
    return (head->req.command == memreq_types::cmd_FLUSH);
  }
  
 #ifdef DISTRIBUTED_SHARED_SYSTEM
  //the 8 hight bits of the address is used to identify the routing destination
  uint32_t getMemReqDstId(uint32_t addr)
  {
	  
   #ifdef NOC_SUPPORT_PARALLELIZATION 
      if( (addr & THREAD_ADDRESS) == THREAD_ADDRESS )
         return THREAD_DESTINATION;
   #endif
      uint32_t addr_msb = addr>>24; 
      uint32_t dst_id = (addr_msb * nbMem) / 256 + 1;
      if ( dst_id == local_id )
         return 0;

      return dst_id;
  }	
 #endif

 virtual void ctrlz_hook()
  { cerr << "\e[1;37;44m" << name() << "\e[0m" << endl;
    cerr << " - cpu_queue: " << cpu_queue.Size() << endl;
    cerr << " - mem_queue: " << mem_queue.Size() << endl;
    cerr << " - cpu_queue:\n" << cpu_queue << endl;
    cerr << " - mem_queue:\n" << mem_queue << endl;
//  Queue <BufferLine,BufferSize> cpu_queue;  ///< Bufferized mesages to be sent by the bus from the CPUs
//  Queue <BufferLine,BufferSize> mem_queue;  ///< Bufferized mesages to be sent by the bus from the MEMs

//    cerr << " - pending_MFA_requests: " << pending_MFA_requests << endl;
//   cerr << " - ciruit_state: " << circuit_state << endl;
//    cerr << " - mem_queue:\n" << mem_queue << endl;
//  Queue <BufferLine,BufferSize> cpu_queue;  ///< Bufferized mesages to be sent by the bus from the CPUs
//  Queue <BufferLine,BufferSize> mem_queue;  ///< Bufferized mesages to be sent by the bus from the MEMs
  }

};

} // end of namespace fsb
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
