/*
 *  Copyright (c) 2007,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: 
 *     Gilles Mouchard (gilles.mouchard@cea.fr)
 *     David Parello (david.parello@univ-perp.fr)
 *
 */

/***************************************************************************
                           Arbiter.hh  -  description
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_COMMIT_DATA_BUS_ARBITER_MC_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_COMMIT_DATA_BUS_ARBITER_MC_HH__

//#include <unisim/component/clm/processor/ooosim/parameters.hh>
#include <unisim/component/clm/interfaces/instruction_interface.hh>


namespace unisim {
namespace component {
namespace clm {
namespace pipeline {
namespace common {


using unisim::component::clm::interfaces::InstructionPtr;

/** A SystemC module to arbiter a bus access between several instructions. Oldest instructions have priority */
template <class T, int nSources, int INTWidth, int FPUWidth, int AGUWidth, int LSQWidth, int OutputWidth, int nChannels, uint32_t nConfig=2 >
class DataBusArbiter : public module
{
public:
  //	inport<InstructionPtr, nConfig*(INTWidth+FPU > inInstruction[nInputs];
	inport<InstructionPtr, nConfig*INTWidth > inINTInstruction;
	inport<InstructionPtr, nConfig*FPUWidth > inFPUInstruction;
	inport<InstructionPtr, nConfig*AGUWidth > inAGUInstruction;
	inport<InstructionPtr, nConfig*LSQWidth > inLSQInstruction;
  	outport<InstructionPtr, nConfig*OutputWidth > outInstruction[nChannels];
  //	outport<Instruction> outInstruction[OutputWidth * nChannels];

	/** The constructor.
		@param name the module name
	*/
	DataBusArbiter(const char *name) : module(name)
	{	  
		int i, j;
		class_name = " ArbiterClass";
		// Unisim port names ...
		//for (i=0;i<nInputs;i++)
		//  {
		//inInstruction[i].set_unisim_name(this,"inInstruction",i);
		inINTInstruction.set_unisim_name(this,"inINTInstruction");
		inFPUInstruction.set_unisim_name(this,"inFPUInstruction");
		inAGUInstruction.set_unisim_name(this,"inAGUInstruction");
		inLSQInstruction.set_unisim_name(this,"inLSQInstruction");
		//  }
		for (j = 0; j < nChannels; j++)
		  {
		    //  for(i = 0; i < OutputWidth; i++)
		    //  {
		    //  //	int s = i*OutputWidth+j;
		    // int s = j*OutputWidth + i;
		    outInstruction[j].set_unisim_name(this,"outInstruction",j);
		    //			outInstruction[s].set_unisim_name(this,"outInstruction",s);
		    //  }
		  }
		///////////////////////

		//for (i=0;i<nInputs;i++)
		//{
		sensitive_method(on_Data) << inINTInstruction.data;
		sensitive_method(on_Data) << inFPUInstruction.data;
		sensitive_method(on_Data) << inAGUInstruction.data;
		sensitive_method(on_Data) << inLSQInstruction.data;
		//}

		for (j = 0; j < nChannels; j++)
		  {
		    //  for(i = 0; i < nInputs; i++)
		    //  {
		    sensitive_method(on_Accept) << outInstruction[j].accept;
		    //sensitive_method(on_Accept) << outInstruction[ j*OutputWidth+i ].accept;
		    //  }
		  }
		//for (i=0;i<nInputs;i++)
		//  {
		sensitive_method(on_Enable) << inINTInstruction.enable;
		sensitive_method(on_Enable) << inFPUInstruction.enable;
		sensitive_method(on_Enable) << inAGUInstruction.enable;
		sensitive_method(on_Enable) << inLSQInstruction.enable;
		//  }

		// --- Latex rendering hints -----------------
		/*
		for (j = 0; j < nChannels; j++)
		  {
		    //  for(int i=0;i<OutputWidth-1;i++)
		    //  { 
		    outInstruction[i][j].set_fused();
		    //  }
		    latex_right_ports.push_back(&outInstruction[OutputWidth-1][j]);
		  }
		*/
		/*
		latex_left_ports.push_back(&inInstruction[Width-1]);
		
		for(int i=0;i<Width-1;i++)
		{ 
		  outInstruction[i].set_fused();
		}
		latex_right_ports.push_back(&outInstruction[Width-1]);
		
		for (i=0; i<WriteBackWidth-1; i++)
		  {
		    inWriteBackInstruction[i].set_fused();
		  }
		latex_top_ports.push_back(&inWriteBackInstruction[WriteBackWidth-1]);

		for (i=0; i<RetireWidth-1; i++)
		  {
		    inRetireInstruction[i].set_fused();
		  }
		latex_top_ports.push_back(&inRetireInstruction[RetireWidth-1]);

		latex_top_ports.push_back(&inFlush);
		*/
	}

  void on_Data()
  {
    /*
    bool areallknown(true);
    int i;
    int j;
    for (i=0;i<InputWidth;i++)
      {
        areallknown &= inInstruction[i].data.known();
      }
    */
    //    if (areallknown)
    if (inINTInstruction.data.known() &&
	inFPUInstruction.data.known() && 
	inAGUInstruction.data.known() && 
	inLSQInstruction.data.known()
	)
      {
	//	for(int i = 0; i < InputWidth; i++)
	//	  {
	for (int cfg=0; cfg<nConfig; cfg++)
	{
	  for (int i=0; i<INTWidth; i++)
	    {
	      if(inINTInstruction.data[cfg*INTWidth+i].something())
		{
		  for (int j=0; j < nChannels; j++)
		    {
#ifdef DD_DEBUG_SIGNALS
		      //cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: on_Data >>> " << endl;
		      //cerr << " Sending instruction on out["<<i<<"]["<<j<<"]: "<< inInstruction[i].data << endl;
#endif	
		      outInstruction[j].data[cfg*OutputWidth+i] = inINTInstruction.data[cfg*INTWidth+i];
		      //outInstruction[j*OutputWidth+i].data = inInstruction[i].data;
		    }
		}
	      else
		{
		  for (int j=0; j < nChannels; j++)
		    {
#ifdef DD_DEBUG_SIGNALS
		      //cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: on_Data >>> " << endl;
		      //cerr << " Sending Nothing on out["<<i<<"]["<<j<<"]" << endl;
#endif	
		      outInstruction[j].data[cfg*OutputWidth+i].nothing();
		      //outInstruction[ j*OutputWidth+i ].data.nothing();
		    }
		}
	    }
	  for (int i=0; i<FPUWidth; i++)
	    {
	      if(inFPUInstruction.data[cfg*FPUWidth+i].something())
		{
		  for (int j=0; j < nChannels; j++)
		    {
#ifdef DD_DEBUG_SIGNALS
		      //cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: on_Data >>> " << endl;
		      //cerr << " Sending instruction on out["<<i<<"]["<<j<<"]: "<< inInstruction[i].data << endl;
#endif	
		      outInstruction[j].data[cfg*OutputWidth+INTWidth+i] = inFPUInstruction.data[cfg*FPUWidth+i];
		      //outInstruction[j*OutputWidth+i].data = inInstruction[i].data;
		    }
		}
	      else
		{
		  for (int j=0; j < nChannels; j++)
		    {
#ifdef DD_DEBUG_SIGNALS
		      //cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: on_Data >>> " << endl;
		      //cerr << " Sending Nothing on out["<<i<<"]["<<j<<"]" << endl;
#endif	
		      outInstruction[j].data[cfg*OutputWidth+INTWidth+i].nothing();
		      //outInstruction[ j*OutputWidth+i ].data.nothing();
		    }
		}
	    }
	  for (int i=0; i<AGUWidth; i++)
	    {
	      if(inAGUInstruction.data[cfg*AGUWidth+i].something())
		{
		  for (int j=0; j < nChannels; j++)
		    {
#ifdef DD_DEBUG_SIGNALS
		      //cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: on_Data >>> " << endl;
		      //cerr << " Sending instruction on out["<<i<<"]["<<j<<"]: "<< inInstruction[i].data << endl;
#endif	
		      outInstruction[j].data[cfg*OutputWidth+INTWidth+FPUWidth+i] = inAGUInstruction.data[cfg*AGUWidth+i];
		      //outInstruction[j*OutputWidth+i].data = inInstruction[i].data;
		    }
		}
	      else
		{
		  for (int j=0; j < nChannels; j++)
		    {
#ifdef DD_DEBUG_SIGNALS
		      //cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: on_Data >>> " << endl;
		      //cerr << " Sending Nothing on out["<<i<<"]["<<j<<"]" << endl;
#endif	
		      outInstruction[j].data[cfg*OutputWidth+INTWidth+FPUWidth+i].nothing();
		      //outInstruction[ j*OutputWidth+i ].data.nothing();
		    }
		}
	    }
	  for (int i=0; i<LSQWidth; i++)
	    {
	      if(inLSQInstruction.data[cfg*LSQWidth+i].something())
		{
		  for (int j=0; j < nChannels; j++)
		    {
#ifdef DD_DEBUG_SIGNALS
		      //cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: on_Data >>> " << endl;
		      //cerr << " Sending instruction on out["<<i<<"]["<<j<<"]: "<< inInstruction[i].data << endl;
#endif	
		      outInstruction[j].data[cfg*OutputWidth+INTWidth+FPUWidth+AGUWidth+i] = inLSQInstruction.data[cfg*LSQWidth+i];
		      //outInstruction[j*OutputWidth+i].data = inInstruction[i].data;
		    }
		}
	      else
		{
		  for (int j=0; j < nChannels; j++)
		    {
#ifdef DD_DEBUG_SIGNALS
		      //cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: on_Data >>> " << endl;
		      //cerr << " Sending Nothing on out["<<i<<"]["<<j<<"]" << endl;
#endif	
		      outInstruction[j].data[cfg*OutputWidth+INTWidth+FPUWidth+AGUWidth+i].nothing();
		      //outInstruction[ j*OutputWidth+i ].data.nothing();
		    }
		}
	    }
	}
	for (int j=0; j < nChannels; j++)
	  {
	    outInstruction[j].data.send();
	  }
      } //end of: if (areallknown)
  } // end of: on_Data...

  void on_Accept()
  {
    
    bool areallknown(true);
    bool allaccept(true);
    //    for (i=0;i<InputWidth;i++)
    //      {
    for (int j = 0; j < nChannels; j++)
      {
	areallknown &= outInstruction[j].accept.known();
	//areallknown &= outInstruction[ j*OutputWidth+i ].accept.known();
      }
    //  }
    if (areallknown)
      {

	for (int cfg=0; cfg<nConfig; cfg++)
	{
	  // For INT
	  for(int i=0; i < INTWidth; i++)
	  {
	    allaccept = true;
	    for (int j=0; j < nChannels; j++)
	      {
		allaccept &= outInstruction[j].accept[cfg*OutputWidth+i];
		//allaccept &= outInstruction[ j*OutputWidth+i ].accept;
	      }
	    inINTInstruction.accept[cfg*INTWidth+i] = allaccept;
	  }
	  // For FPU
	  for(int i=0; i < FPUWidth; i++)
	  {
	    allaccept = true;
	    for (int j=0; j < nChannels; j++)
	      {
		allaccept &= outInstruction[j].accept[cfg*OutputWidth+INTWidth+i];
		//allaccept &= outInstruction[ j*OutputWidth+i ].accept;
	      }
	    inFPUInstruction.accept[cfg*FPUWidth+i] = allaccept;
	  }
	  // For AGU
	  for(int i=0; i < AGUWidth; i++)
	  {
	    allaccept = true;
	    for (int j=0; j < nChannels; j++)
	      {
		allaccept &= outInstruction[j].accept[cfg*OutputWidth+INTWidth+FPUWidth+i];
		//allaccept &= outInstruction[ j*OutputWidth+i ].accept;
	      }
	    inAGUInstruction.accept[cfg*AGUWidth+i] = allaccept;
	  }
	  // For LSQ
	  for(int i=0; i < LSQWidth; i++)
	  {
	    allaccept = true;
	    for (int j=0; j < nChannels; j++)
	      {
		allaccept &= outInstruction[j].accept[cfg*OutputWidth+INTWidth+FPUWidth+AGUWidth+i];
		//allaccept &= outInstruction[ j*OutputWidth+i ].accept;
	      }
	    inLSQInstruction.accept[cfg*LSQWidth+i] = allaccept;
	  }
	
	}// end of foreach
	inINTInstruction.accept.send();
	inFPUInstruction.accept.send();
	inAGUInstruction.accept.send();
	inLSQInstruction.accept.send();
      }//EO: if (areallknown) ...
  } //EO: on_Accept

  void on_Enable()
  {
    bool areallknown(true);
    /*
    int i;
    int j;
    
    for (i=0;i<InputWidth;i++)
      {
        areallknown &= inInstruction[i].enable.known();
      }
    */
    //    if (areallknown)
    if ( inINTInstruction.enable.known() && 
	 inFPUInstruction.enable.known() && 
	 inAGUInstruction.enable.known() && 
	 inLSQInstruction.enable.known()
	 )
      {
	for(int cfg=0; cfg<nConfig; cfg++)
	{
	  // For INT
	  for(int i=0; i < INTWidth; i++)
	  {
	    if(inINTInstruction.enable[cfg*INTWidth+i])
	    {
	      for (int j=0; j < nChannels; j++)
	      {
		outInstruction[j].enable[cfg*OutputWidth+i] = true;
	      }
	    }
	    else
	    {
	      for (int j=0; j < nChannels; j++)
	      {
		outInstruction[j].enable[cfg*OutputWidth+i] = false;
	      }
	    }
	  }

	  // For FPU
	  for(int i=0; i < FPUWidth; i++)
	  {
	    if(inFPUInstruction.enable[cfg*FPUWidth+i])
	    {
	      for (int j=0; j < nChannels; j++)
	      {
		outInstruction[j].enable[cfg*OutputWidth+INTWidth+i] = true;
	      }
	    }
	    else
	    {
	      for (int j=0; j < nChannels; j++)
	      {
		outInstruction[j].enable[cfg*OutputWidth+INTWidth+i] = false;
	      }
	    }
	  }

	  // For AGU
	  for(int i=0; i < AGUWidth; i++)
	  {
	    if(inAGUInstruction.enable[cfg*AGUWidth+i])
	    {
	      for (int j=0; j < nChannels; j++)
	      {
		outInstruction[j].enable[cfg*OutputWidth+INTWidth+FPUWidth+i] = true;
	      }
	    }
	    else
	    {
	      for (int j=0; j < nChannels; j++)
	      {
		outInstruction[j].enable[cfg*OutputWidth+INTWidth+FPUWidth+i] = false;
	      }
	    }
	  }

	  // For LSQ
	  for(int i=0; i < LSQWidth; i++)
	  {
	    if(inLSQInstruction.enable[cfg*LSQWidth+i])
	    {
	      for (int j=0; j < nChannels; j++)
	      {
		outInstruction[j].enable[cfg*OutputWidth+INTWidth+FPUWidth+AGUWidth+i] = true;
	      }
	    }
	    else
	    {
	      for (int j=0; j < nChannels; j++)
	      {
		outInstruction[j].enable[cfg*OutputWidth+INTWidth+FPUWidth+AGUWidth+i] = false;
	      }
	    }
	  }

	} // end of foreach Config.

	for (int j=0; j < nChannels; j++)
	  {
	    outInstruction[j].enable.send();
	  }
	
      } // end of if(areallknown)
  }


private:
  
};

} // end of namespace common
} // end of namespace pipeline
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim


#endif
