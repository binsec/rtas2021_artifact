/* *****************************************************************************  
            bus.sim  -  Cycle-level memory bus spporting snooping.
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
//#include <unisim/component/clm/utility/error.h>
#include <unisim/component/clm/utility/utility.hh>
#include <sstream>

#include <unisim/component/clm/debug/svg_memreq_interface.hh>
#include <unisim/kernel/kernel.hh>

namespace unisim {
namespace component {
namespace clm {
namespace fsb {

// new usings
using unisim::component::clm::interfaces::memreq;
using unisim::component::clm::interfaces::memreq_types;

using unisim::component::clm::debug::SVGmemreqInterface;

using unisim::kernel::Client;
using unisim::kernel::ServiceImport;

using unisim::kernel::Object;

  /*
using full_system::utils::services::Client;
using full_system::plugins::SVGmemreqInterface;
using full_system::utils::services::ServiceImport;
  */

//#define DD_DEBUG_BUS

template 
< class INSTRUCTION,
  int nCPU, 
  int BufferSize, 
  int RequestWidth,
  bool Snooping = false
>
class Bus : public module, public Client<SVGmemreqInterface<INSTRUCTION> >
{private:
  struct BufferLine
  { memreq < INSTRUCTION, RequestWidth > req;
    int sender_id;   // Used not to send the data to the module sending it
  };

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

  Queue < BufferLine ,BufferSize> buffer;  ///< Bufferized mesages to be sent by the bus
  int round_robin_index;                   ///< Index for the round robin algorithm

  /**
   * \brief Create a new Bus module
   */
  Bus(const char *name) : module(name)
			, Client<SVGmemreqInterface<INSTRUCTION> >(name, 0)
			, svg("svg", this)
    			, Object(name, 0)
  { //Interface naming to enhance debugging info
    class_name = " Bus";
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
    round_robin_index = 0;
    /* process Sensitivity list */
    sensitive_method(on_data) << inMEM.data << outMEM.accept;
    if(Snooping)
    { sensitive_method(on_shared_accept) << outSharedMEM.accept;
    }
    for(int i=0;i<nCPU;i++)
    { sensitive_method(on_data) << inCPU[i].data << outCPU[i].accept;
      if(Snooping)
      { sensitive_method(on_shared) << inSharedCPU[i].data;
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
    { latex_top_ports.push_back(&inCPU[i]);
      latex_top_ports.push_back(&outCPU[i]);
    }
    latex_bottom_ports.push_back(&outMEM);
    latex_bottom_ports.push_back(&inMEM);
  }

  /**
   * \brief Process called at the beggining of the cycle.
   *
   * Broadcast the first bufferized value.
   */
  void begin_of_cycle()
  {
#ifdef DD_DEBUG_BUS
    cerr << "[DD_DEBUG_BUS] ------------------------------------> BOF (Beginning)<---------------------------------" << endl;
#endif
    if(!buffer.Empty())
    { // If the buffer is not empty, send its first request to every output (but the sender itself)
      BufferLine *head = buffer.GetHead();
#ifdef DD_DEBUG_BUS
	cerr << "[DD_DEBUG_BUS] Bus Buffer Sending : " <<  head->req << " to all " << endl;
#endif

#ifdef DEBUG_BUS_MQ
INFO << "Sending to all: " << head->req << endl;
#endif

      if(head->sender_id!=nCPU) outMEM.data = head->req;
      else outMEM.data.nothing();
      for(int i=0;i<nCPU;i++)
      { if((head->sender_id!=i) || is_flush()) outCPU[i].data = head->req;
        else outCPU[i].data.nothing();
      }
    }
    else
    { // Else sends nothing to every output
//INFO << "Sending NOTHING to all " << endl;
      outMEM.data.nothing();
      for(int i=0;i<nCPU;i++)
      { outCPU[i].data.nothing();
      }  
    }
#ifdef DD_DEBUG_BUS
    cerr << "[DD_DEBUG_BUS] ------------------------------------> BOF (End)<---------------------------------" << endl;
#endif
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
    if(all_output_accepted())
    { // A data was sent, and every output it was sent to accepted it,
      // so remove it from the buffer
      // INFO << "All bus outputs accepted the data" << endl;
      buffer.RemoveHead();
    }
    else
    { //if(!buffer.Empty()) INFO << "All bus outputs DID NOT accept the data" << endl;
    }
    //If incoming message is enabled store it to the buffer
    if(inMEM.enable)
    { BufferLine bl;
      bl.req = inMEM.data;
      bl.sender_id = nCPU;
      buffer << bl;
#ifdef DD_DEBUG_BUS
      INFO << "[DD_DEBUG_BUS] Stored   " << bl.req << " from mem" << endl;
#endif
#ifdef _CACHE_MESSAGES_SVG_H_
          SVG.add(timestamp(),"MM","BS","",bl.req);
#endif
      if(svg)
      { svg->add_mem_to_bus(timestamp(),name(),"",bl.req);
      }
    }
    else
    { for(int i=0;i<nCPU;i++)
      { if(inCPU[i].enable)
        { //memreq<RequestWidth> msg = inCPU[i].data;
          BufferLine bl;
          bl.req = inCPU[i].data;
          bl.sender_id = i;
          buffer << bl;
#ifdef DD_DEBUG_BUS
          INFO << "[DD_DEBUG_BUS] Stored   " << bl.req << " from cpu" << endl;
#endif
#ifdef _CACHE_MESSAGES_SVG_H_
          stringstream ss;
          ss << "$" << (i+1);
          SVG.add(timestamp(),ss.str(),"BS","",bl.req);
#endif
          if(svg)
          { svg->add_cac_to_bus(timestamp(),i+1,name(),"",bl.req);
          }
        }
      }  
    }
#ifdef DD_DEBUG_BUS
    cerr << "[DD_DEBUG_BUS] ------------------------------------> EOF (end of end of cycle)<---------------------------------" << endl;
#endif
  }

  /**
   * \brief Process launched upon receiving a request/data from the CPU & memory, and
   * setting the accept signals.
   **/
  void on_data()
  { 
#ifdef DD_DEBUG_BUS
    cerr << "[DD_DEBUG_BUS] ------------------------------------> Beginning of ON_DATA() <---------------------------------" << endl;
#endif
    // Exit if one of the input signal is not known
    if(!inMEM.data.known()) return;
    if(!outMEM.accept.known()) return;
    for(int i=0;i<nCPU;i++)
    { if(!inCPU[i].data.known()) return;
      if(!outCPU[i].accept.known()) return;
    }

#ifdef DEBUG_BUS_MQ
for(int i=0;i<nCPU;i++)
{ if(inCPU[i].data.something())
  { INFO << "Receiving(" << i << "):   " << inCPU[i].data << endl;
  }
}
#endif

    bool is_full = buffer.Full();
    bool all_accept = all_output_accepted();
    
    //All the accepts are known, set the corresponding enables
    if(all_accept)
    { 
      // David Debug
#ifdef DD_DEBUG_BUS      
        cerr << "[DD_DEBUG_BUS] Buffer recevied all accepts !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << endl;
#endif
      
      // When every output the data was sent to accepted it,
      // Enable everyof those outputs.
      BufferLine *head = buffer.GetHead();
      if(head->sender_id!=nCPU) outMEM.enable = true;
      else                      outMEM.enable = false;
      for(int i=0;i<nCPU;i++)
      { if((head->sender_id!=i) || is_flush()) outCPU[i].enable = true;
        else                                   outCPU[i].enable = false;
      }
    }
    else
    { // When no data was sent,or one of the expected out did not accept,
      // do not enable.
      outMEM.enable = false;
      for(int i=0;i<nCPU;i++)
      { outCPU[i].enable = false;
      }
    }
        
    //All the signal are known, first accept memrory request with higher priority
    if(inMEM.data.something())
    { 
      // David Debug
      
#ifdef DD_DEBUG_BUS
        cerr << "[DD_DEBUG_BUS] Buffer receving SOMETHING from memory !!! " << endl;
#endif
      
      if( (!is_full) || all_accept)
      { inMEM.accept = true;
#ifdef DD_DEBUG_BUS
          cerr << "[DD_DEBUG_BUS] Buffer accepting SOMETHING from memory !!! " << endl;
#endif
        for(int j=0;j<nCPU;j++)
        { inCPU[j].accept = false;
        }
      }
#ifdef DD_DEBUG_BUS
      cerr << "[DD_DEBUG_BUS] ------------------------------------> End of ON_DATA() after accepting something from Memory <---------------------------------" << endl;
#endif
      return;
    }
    //If nothing has to be sent back from memory, accept the data from the first non-nothing CPU
    for(int k=0+round_robin_index;k<nCPU+round_robin_index;k++)
    { int i = k % nCPU;
      if(inCPU[i].data.something())
      { 
#ifdef DD_DEBUG_BUS
        cerr << "[DD_DEBUG_BUS] Buffer receving SOMETHING from CPU !!! " << inCPU[i].data << endl;
#endif
        if( (!is_full) || all_accept)
        { inMEM.accept = false;
#ifdef DD_DEBUG_BUS
          cerr << "[DD_DEBUG_BUS] Buffer accepting SOMETHING from CPU !!! " << inCPU[i].data << endl;
#endif
          for(int j=0;j<nCPU;j++)
          { if(i==j) inCPU[j].accept = true;
            else     inCPU[j].accept = false;
          }
          round_robin_index++;
          if(round_robin_index==nCPU) round_robin_index = 0;
#ifdef DD_DEBUG_BUS
          cerr << "[DD_DEBUG_BUS] ------------------------------------> End of ON_DATA() after Accepting something from CPU <--------------" << endl;
#endif
          return;
        }
      }
    }
    //Every signal is set to nothing, refuse every signal
    inMEM.accept = false;
    for(int j=0;j<nCPU;j++)
    { inCPU[j].accept = false;
    }    
#ifdef DD_DEBUG_BUS
    cerr << "[DD_DEBUG_BUS] ------------------------------------> Real End of ON_DATA() (after refusing every thing)<---------------------------------" << endl;
#endif

  }

  /**
   * \brief Process launched upon receiving a shared flag from the CPU
   *
   * Shared is set to true on every output, if it set to true to one of the input, and
   * none of the input is nothing.
   */
  void on_shared()
  { if(Snooping)
    { // Exit if one of the input signal is not known
      for(int i=0;i<nCPU;i++)
      { if(!inSharedCPU[i].data.known()) return;
      }
      //All the signal are known, check is a message has been sent
      if(buffer.Empty())
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
      BufferLine *head = buffer.GetHead();
      bool is_shared = false;

/*
      if( (head->req.message_type==memreq_types::type_ANSWER) && (head->sender_id<nCPU) )
      { // An answer from a request is not coming from the memory, but from a cache,
        // so the shared bit should be set.
        is_shared = true;
      }
*/

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
#ifdef _CACHE_MESSAGES_SVG_H_
if(is_shared) SVG.add_box_info(timestamp(),"BS","S");
#endif
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
  { if (buffer.Size()) return true;
    else return false;
  }

 int get_rank()
 { return inCPU[0].get_connected_module()->get_rank()+1;
 }

 private:
  /**
   * \brief Returns true if all the output a data was sent to accepted it.
   */
  bool all_output_accepted()
  { if(buffer.Empty())
    { //No data was sent this cycle, return false
      return false;
    }
    else
    { BufferLine *head = buffer.GetHead();
      bool all_accept = true;
      
      if(head->sender_id!=nCPU) all_accept &= outMEM.accept;
      for(int i=0;i<nCPU;i++)
      { if((head->sender_id!=i) || is_flush()) all_accept &= outCPU[i].accept;
      }
      return all_accept;
    }
  }

  virtual bool ClientIndependentSetup()
  { if(svg) svg->register_bus(name());
    return true;
  }
  
  inline bool is_flush()
  { BufferLine *head = buffer.GetHead();
    return (head->req.command == memreq_types::cmd_FLUSH);
  }
  
};

} // end of namespace fsb
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
