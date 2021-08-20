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

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_COMMIT_SIMPLE_ARBITER_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_COMMIT_SIMPLE_ARBITER_HH__

#include <unisim/component/clm/processor/ooosim/parameters.hh>
#include <unisim/component/clm/interfaces/instruction_interface.hh>


namespace unisim {
namespace component {
namespace clm {
namespace pipeline {
namespace common {


using unisim::component::clm::interfaces::InstructionPtr;

/** A SystemC module to arbiter a bus access between several instructions. Oldest instructions have priority */
template <class T, int nSources, int InputWidth, int OutputWidth, int nChannels>
class Arbiter : public module
{
public:
  /*
	ml_in_valid inValid[InputWidth];
	ml_in_enable inEnable[InputWidth];
	ml_in_data<InstructionPtr<T, nSources> > inInstruction[InputWidth];
	ml_out_accept outAccept[InputWidth];
  */
	inport<InstructionPtr> inInstruction[InputWidth];
  /*
	ml_out_valid outValid[OutputWidth];
	ml_out_enable outEnable[OutputWidth];
	ml_out_data<InstructionPtr<T, nSources> > outInstruction[OutputWidth];
	ml_in_accept inAccept[OutputWidth];
  */
  	outport<InstructionPtr> outInstruction[OutputWidth][nChannels];
  //	outport<Instruction> outInstruction[OutputWidth * nChannels];

	/** The constructor.
		@param name the module name
	*/
	Arbiter(const char *name) : module(name)
	{	  
		int i, j;
		class_name = " ArbiterClass";
		// Unisim port names ...
		for (i=0;i<InputWidth;i++)
		  {
		    inInstruction[i].set_unisim_name(this,"inInstruction",i);
		  }
		for (j = 0; j < nChannels; j++)
		  {
		    for(i = 0; i < OutputWidth; i++)
		      {
			//	int s = i*OutputWidth+j;
			int s = j*OutputWidth + i;
			outInstruction[i][j].set_unisim_name(this,"outInstruction",s);
			//			outInstruction[s].set_unisim_name(this,"outInstruction",s);
		      }
		  }
		///////////////////////

		for (i=0;i<InputWidth;i++)
		  {
		    sensitive_method(on_Data) << inInstruction[i].data;
		  }

		for (j = 0; j < nChannels; j++)
		  {
		    for(i = 0; i < InputWidth; i++)
		      {
			sensitive_method(on_Accept) << outInstruction[i][j].accept;
			//sensitive_method(on_Accept) << outInstruction[ j*OutputWidth+i ].accept;
		      }
		  }
		for (i=0;i<InputWidth;i++)
		  {
		    sensitive_method(on_Enable) << inInstruction[i].enable;
		  }

		// --- Latex rendering hints -----------------

		for (j = 0; j < nChannels; j++)
		  {
		    for(int i=0;i<OutputWidth-1;i++)
		      { 
			outInstruction[i][j].set_fused();
		      }
		    latex_right_ports.push_back(&outInstruction[OutputWidth-1][j]);
		  }
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
    bool areallknown(true);
    int i;
    int j;
    for (i=0;i<InputWidth;i++)
      {
        areallknown &= inInstruction[i].data.known();
      }
    if (areallknown)
      {
	for(i = 0; i < InputWidth; i++)
	  {
	    if(inInstruction[i].data.something())
	      {
		for (j = 0; j < nChannels; j++)
		  {
#ifdef DD_DEBUG_SIGNALS
		    cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: on_Data >>> " << endl;
		    cerr << " Sending instruction on out["<<i<<"]["<<j<<"]: "<< inInstruction[i].data << endl;
#endif	
		    outInstruction[i][j].data = inInstruction[i].data;
		    //outInstruction[j*OutputWidth+i].data = inInstruction[i].data;
		  }
	      }
	    else
	      {
		for (j = 0; j < nChannels; j++)
		  {
#ifdef DD_DEBUG_SIGNALS
		    cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: on_Data >>> " << endl;
		    cerr << " Sending Nothing on out["<<i<<"]["<<j<<"]" << endl;
#endif	
		    outInstruction[i][j].data.nothing();
		    //outInstruction[ j*OutputWidth+i ].data.nothing();
		  }
	      }
	  }
      } //end of: if (areallknown)
  } // end of: on_Data...

  void on_Accept()
  {
    bool areallknown(true);
    int i;
    int j;
    bool allaccept(true);
    for (i=0;i<InputWidth;i++)
      {
	for (j = 0; j < nChannels; j++)
	  {
	    areallknown &= outInstruction[i][j].accept.known();
	    //areallknown &= outInstruction[ j*OutputWidth+i ].accept.known();
	  }
      }
    if (areallknown)
      {
	for(i = 0; i < InputWidth; i++)
	  {
	    allaccept = true;
	    for (j = 0; j < nChannels; j++)
	      {
		allaccept &= outInstruction[i][j].accept;
		//allaccept &= outInstruction[ j*OutputWidth+i ].accept;

	      }
#ifdef DD_DEBUG_SIGNALS
	    cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: on_Accept >>> " << endl;
	    cerr << " Sending accept="<< (allaccept?"True":"False") << " on in["<<i<<"]" << endl;
#endif	
	    inInstruction[i].accept = allaccept;
	  }
      }//EO: if (areallknown) ...
  } //EO: on_Accept

  void on_Enable()
  {
    bool areallknown(true);
    int i;
    int j;
    for (i=0;i<InputWidth;i++)
      {
        areallknown &= inInstruction[i].enable.known();
      }
    if (areallknown)
      {
	for(i = 0; i < InputWidth; i++)
	  {
	    if(inInstruction[i].enable)
	      {
		for (j = 0; j < nChannels; j++)
		  {
#ifdef DD_DEBUG_SIGNALS
		    cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: on_Enable >>> " << endl;
		    cerr << " Sending enable="<< "True" << " on out["<<i<<"]["<<j<<"]" << endl;
#endif	
		    outInstruction[i][j].enable = true;
		    //outInstruction[ j*OutputWidth+i ].enable = true;
		  }
	      }
	    else
	      {
		for (j = 0; j < nChannels; j++)
		  {
#ifdef DD_DEBUG_SIGNALS
		    cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: on_Enable >>> " << endl;
		    cerr << " Sending enable="<< "True" << " on out["<<i<<"]["<<j<<"]" << endl;
#endif	
		    outInstruction[i][j].enable = false;
		    //outInstruction[ j*OutputWidth+i ].enable = false;
		  }
	      }
	  }
      }
  }


private:
  
};

} // end of namespace common
} // end of namespace pipeline
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim


#endif
