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
                          Dispatcher.h  -  description
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_DECODE_DISPATCHER_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_DECODE_DISPATCHER_HH__

#include <unisim/component/clm/processor/ooosim/parameters.hh>
#include <unisim/component/clm/interfaces/instruction_interface.hh>


namespace unisim {
namespace component {
namespace clm {
namespace pipeline {
namespace decode {

using unisim::component::clm::interfaces::InstructionPtr;

using unisim::component::clm::processor::ooosim::nConditionArchitecturalRegisters;

  //using unisim::component::clm::::InstructionPtr;

  //  using unisim::component::clm::utility::hexa;




/** A SystemC module to dispatch instruction in the good issue queue */
//template <class T, int nSources, int Width, int nIntegerIssueQueueWritePorts, int nFloatingPointIssueQueueWritePorts, int nLoadStoreIssueQueueWritePorts>
template <int Width, int nIntegerIssueQueueWritePorts, int nFloatingPointIssueQueueWritePorts, int nLoadStoreIssueQueueWritePorts>
class Dispatcher : public module
{
public:
	/* From/to the allocator/renamer */
        /*
	ml_in_valid inValid[Width];
	ml_in_enable inEnable[Width];
	ml_in_data<InstructionPtr<T, nSources> > inInstruction[Width];
	ml_out_accept outAccept[Width];
	*/
  //        inport<InstructionPtr<T, nSources> > inInstruction[Width];
        inport<InstructionPtr> inInstruction[Width];

	/* From/to the integer issue queue */
	/*
	ml_out_valid outIntegerValid[nIntegerIssueQueueWritePorts];
	ml_out_enable outIntegerEnable[nIntegerIssueQueueWritePorts];
	ml_out_data<InstructionPtr<T, nSources> > outIntegerInstruction[nIntegerIssueQueueWritePorts];
	ml_in_accept inIntegerAccept[nIntegerIssueQueueWritePorts];
	*/
	outport<InstructionPtr> outIntegerInstruction[nIntegerIssueQueueWritePorts];
	
	/* From/to the floating point issue queue */
	/*
	ml_out_valid outFloatingPointValid[nFloatingPointIssueQueueWritePorts];
	ml_out_enable outFloatingPointEnable[nFloatingPointIssueQueueWritePorts];
	ml_out_data<InstructionPtr<T, nSources> > outFloatingPointInstruction[nFloatingPointIssueQueueWritePorts];
	ml_in_accept inFloatingPointAccept[nFloatingPointIssueQueueWritePorts];
	*/
	outport<InstructionPtr> outFloatingPointInstruction[nFloatingPointIssueQueueWritePorts];

	/* From/to the load/store issue queue */
	/*
	ml_out_valid outLoadStoreValid[nLoadStoreIssueQueueWritePorts];
	ml_out_enable outLoadStoreEnable[nLoadStoreIssueQueueWritePorts];
	ml_out_data<InstructionPtr<T, nSources> > outLoadStoreInstruction[nLoadStoreIssueQueueWritePorts];
	ml_in_accept inLoadStoreAccept[nLoadStoreIssueQueueWritePorts];
	*/
	outport<InstructionPtr> outLoadStoreInstruction[nLoadStoreIssueQueueWritePorts];

	int IntegerMapping[nIntegerIssueQueueWritePorts];
	int FloatingMapping[nFloatingPointIssueQueueWritePorts];
	int LoadStoreMapping[nLoadStoreIssueQueueWritePorts];
	int InstructionMapping[Width];

	/** the constructor
		@param name the module name
	*/
	Dispatcher(const char * name) : module(name)
	{
		int i, j;

		class_name = " DispatcherClass";
		// Unisim port names ...
		for (i=0; i<Width; i++)
		  {
		    inInstruction[i].set_unisim_name(this,"inInstruction",i);
		  }		
		for (i=0; i<nIntegerIssueQueueWritePorts; i++)
		  {
		    outIntegerInstruction[i].set_unisim_name(this,"outIntegerInstruction",i);
		  }
		for (i=0; i<nFloatingPointIssueQueueWritePorts; i++)
		  {
		    outFloatingPointInstruction[i].set_unisim_name(this,"outFloatingPointInstruction",i);
		  }
		for (i=0; i<nIntegerIssueQueueWritePorts; i++)
		  {
		    outLoadStoreInstruction[i].set_unisim_name(this,"outLoadStoreInstruction",i);
		  }
		/*
		SC_HAS_PROCESS(Dispatcher);

		SC_METHOD(ExternalControl);
		for(i = 0; i < Width; i++)
			sensitive << inValid[i] << inEnable[i] << inInstruction[i];
		for(i = 0; i < nIntegerIssueQueueWritePorts; i++)
			sensitive << inIntegerAccept[i];
		for(i = 0; i < nFloatingPointIssueQueueWritePorts; i++)
			sensitive << inFloatingPointAccept[i];
		for(i = 0; i < nLoadStoreIssueQueueWritePorts; i++)
			sensitive << inLoadStoreAccept[i];
		*/
		/*
		for(i = 0; i < Width; i++)
			sensitive_method(onDataAccept) << inInstruction[i].data << inInstruction[i].enable;
		for(i = 0; i < nIntegerIssueQueueWritePorts; i++)
			sensitive_method(onDataAccept) << outIntegerInstruction[i].accept;
		for(i = 0; i < nFloatingPointIssueQueueWritePorts; i++)
			sensitive_method(onDataAccept) << outFloatingPointInstruction[i].accept;
		for(i = 0; i < nLoadStoreIssueQueueWritePorts; i++)
			sensitive_method(onDataAccept) << outLoadStoreInstruction[i].accept;
		*/
		for(i = 0; i < Width; i++)
		  {
		    sensitive_method(onData) << inInstruction[i].data;
		  }
		for(i = 0; i < Width; i++)
		  {
		    sensitive_method(onEnable) << inInstruction[i].enable;
		  }
		for(i = 0; i < nIntegerIssueQueueWritePorts; i++)
		  {
		    sensitive_method(onAccept) << outIntegerInstruction[i].accept;
		  }
		for(i = 0; i < nFloatingPointIssueQueueWritePorts; i++)
		  {
		    sensitive_method(onAccept) << outFloatingPointInstruction[i].accept;
		  }
		for(i = 0; i < nLoadStoreIssueQueueWritePorts; i++)
		  {
		    sensitive_method(onAccept) << outLoadStoreInstruction[i].accept;
		  }

		integerIssueQueueFunction = FnInvalid;
		floatingPointIssueQueueFunction = FnInvalid;
		loadStoreIssueQueueFunction = FnInvalid;
		// --- Latex rendering hints -----------------

		for(int i=0;i<Width-1;i++)
		{ 
		  inInstruction[i].set_fused();
		}
		latex_left_ports.push_back(&inInstruction[Width-1]);

		for(int i=0;i<Width-1;i++)
		{ 
		  outIntegerInstruction[i].set_fused();
		}
		latex_right_ports.push_back(&outIntegerInstruction[Width-1]);
		
		for(int i=0;i<Width-1;i++)
		{ 
		  outFloatingPointInstruction[i].set_fused();
		}
		latex_right_ports.push_back(&outFloatingPointInstruction[Width-1]);
		
		for(int i=0;i<Width-1;i++)
		{ 
		  outLoadStoreInstruction[i].set_fused();
		}
		latex_right_ports.push_back(&outLoadStoreInstruction[Width-1]);
		
		/*
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
		*/

		//		latex_top_ports.push_back(&inFlush);
		//		latex_bottom_ports.push_back(&outFlush);


	}

	/** A SystemC process for managing the valid, accept and enable hand-shaking */
	//	void ExternalControl()
	void onData()
	  {
	    bool areallknown(true);

	    for (int i=0; i<Width; i++)
	      {
		if (!inInstruction[i].data.known()) { areallknown = false; break; }
	      }
	    if (areallknown)
	      {
		int i,j,k,l;

		// Initialize the port mapping
		for(i = 0; i < Width; i++)
		{
			InstructionMapping[i] = -1;
		}
		for(i = 0; i < nIntegerIssueQueueWritePorts; i++)
		  {
		    IntegerMapping[i] = Width;
		  }
		for(i = 0; i < nFloatingPointIssueQueueWritePorts; i++)
		  {
		    FloatingMapping[i] = Width;
		  }
		for(i = 0; i < nLoadStoreIssueQueueWritePorts; i++)
		  {
		    LoadStoreMapping[i] = Width;
		  }
		//		for (int i=0; i<Width; i++)

		for(i = j = k = l = 0; i < Width; i++)
		{
		  {
		    if ( !inInstruction[i].data.something() )
		      {
			//Instruction[i].data.nothing();
		      }
		    else
		      {
			InstructionPtr instruction = inInstruction[i].data;
			//const Instruction *instruction = &inst;
			if(instruction->fn & integerIssueQueueFunction)
			  {
			    if(j < nIntegerIssueQueueWritePorts)
			      {
				IntegerMapping[j] = i;
				InstructionMapping[i] = j;
				outIntegerInstruction[j].data = inInstruction[i].data;
				j++;
			      }
			  }
			else if(instruction->fn & floatingPointIssueQueueFunction)
			  {
			    if(k < nFloatingPointIssueQueueWritePorts)
			      {
				FloatingMapping[k] = i;
				InstructionMapping[i] = k+nIntegerIssueQueueWritePorts;
				outFloatingPointInstruction[k].data = inInstruction[i].data;
				k++;
			      }
			  }
			else if(instruction->fn & loadStoreIssueQueueFunction)
			  {
			    if(l < nLoadStoreIssueQueueWritePorts)
			      {
				LoadStoreMapping[l] = i;
				InstructionMapping[i] = l+nIntegerIssueQueueWritePorts+nFloatingPointIssueQueueWritePorts;
				outLoadStoreInstruction[l].data = inInstruction[i].data;
				l++;
			      }
			  }
			/*
			else
			  {
			    //			    accept[i] = true;
			    // throw away the instruction
			    // particularly the NOPs which don't go to any issue queue !
			    
			  }
			*/
		      }
		  }
		}
		for( ; j < nIntegerIssueQueueWritePorts; j++)
		  {
		    outIntegerInstruction[j].data.nothing();
		  }
		for( ; k < nFloatingPointIssueQueueWritePorts; k++)
		  {
		    outFloatingPointInstruction[k].data.nothing();
		  }
		for( ; l < nLoadStoreIssueQueueWritePorts; l++)
		  {
		    outLoadStoreInstruction[l].data.nothing();
		  }
	      }
	  }

	void onAccept()
	  {
	    bool areallknown(true);
	    int i;
	    /*
	    for (int i=0; i<Width; i++)
	      {
		if (!outInstruction[i].accept) { areallknown =false; break; }
		}*/
	    for(i = 0; i < nIntegerIssueQueueWritePorts; i++)
	      {
		areallknown &= outIntegerInstruction[i].accept.known();
	      }
	    for(i = 0; i < nFloatingPointIssueQueueWritePorts; i++)
	      {
		areallknown &= outFloatingPointInstruction[i].accept.known();
	      }
	    for(i = 0; i < nLoadStoreIssueQueueWritePorts; i++)
	      {
		areallknown &= outLoadStoreInstruction[i].accept.known();
	      }
	    if (areallknown)
	      {
		/*
		int MaxMapping;
		for(i = 0; i < nIntegerIssueQueueWritePorts; i++)
		  {
		    if (IntegerMapping[i] < Width)
		      {
			inInstruction[IntegerMapping[i]].accept = outIntegerInstruction[i].accept;
			MaxMapping++;
		      }
		    else
		      inInstruction[IntegerMapping[i]].accept = false;
		  }
		for(i = 0; i < nFloatingPointIssueQueueWritePorts; i++)
		  {
		    if (FloatingMapping[i] < Width)
		      {
			inInstruction[FloatingMapping[i]].accept = outFloatingPointInstruction[i].accept;
			MaxMapping++;
		      }
		    else
		      inInstruction[FloatingMapping[i]].accept = false;
		  }
		MaxMapping = MAX(MaxMapping,FloatingMapping[i]);
		for(i = 0; i < nLoadStoreIssueQueueWritePorts; i++)
		  {
		    if (LoadStoreMapping[i] < Width)
		      {
			inInstruction[LoadStoreMapping[i]].accept = outLoadStoreInstruction[i].accept;
			MaxMapping++;
		      }
		    else
		      inInstruction[LoadStoreMapping[i]].accept = false;

		  }
		//		MaxMapping = MAX(MaxMapping,LoadStoreMapping[i]);

		for (i = MaxMapping; i<Width; i++)
		  {
		    inInstruction[i].accept = false;
		  }
		*/
		for (i = 0; i<Width; i++)
		  {
		    int j;
		    bool isset = false;
		    for (j=0; j<nIntegerIssueQueueWritePorts; j++)
		    {
		      if ( (IntegerMapping[j]==i) && outIntegerInstruction[j].accept )
			{ 
			  inInstruction[i].accept = true;
			  isset = true;
			  break;
			}
		    }
		    //		    if (!isset)
		    //			{ inInstruction[i].accept = false; }
		    //		    bool isset = false;
		    for (j=0; j<nFloatingPointIssueQueueWritePorts; j++)
		    {
		      if ( (FloatingMapping[j]==i) && outFloatingPointInstruction[j].accept )
			{
			  inInstruction[i].accept = true;
			  isset = true;
			  break;
			}
		    }
		    //		    if (!isset)
		    //			{ inInstruction[i].accept = false; }
		    //		    bool isset = false;
		    for (j=0; j<nLoadStoreIssueQueueWritePorts; j++)
		    {
		      if ( (LoadStoreMapping[j]==i) && outLoadStoreInstruction[j].accept )
			{ 
			  inInstruction[i].accept = true; 
			  isset = true;
			  break;
			}
		    }
		    // 
		    if (!isset)
		      { inInstruction[i].accept = false; }
		  }
		/*
		for (int i=0; i<Width; i++)
		  {
		    inInstruction[i].enable = outInstruction[i].accept;
		  }
		*/
	      }// end of if(areallknown)
	  }// end of onAccept()

	void onEnable()
	  {
	    bool areallknown(true);
	    for (int i=0; i<Width; i++)
	      {
		if (!inInstruction[i].enable.known()) { areallknown = false; break; }
	      }
	    if (areallknown)
	      {
		/*
		for (int i=0; i<Width; i++)
		  {
		    if (InstructionMapping[i] < nIntegerIssueQueueWritePorts)
		      {
			outIntegerInstruction[InstructionMapping[i]].enable = inInstruction[i].enable;
		      }
		    else if (InstructionMapping[i] < nIntegerIssueQueueWritePorts+nFloatingPointIssueQueueWritePorts)
		      {
			outFloatingPointInstruction[InstructionMapping[i]-nIntegerIssueQueueWritePorts].enable = inInstruction[i].enable;
		      }
		    else
		      {
			outLoadStoreInstruction[InstructionMapping[i]-(nIntegerIssueQueueWritePorts+nFloatingPointIssueQueueWritePorts)].enable = inInstruction[i].enable;
		      }
		    
		  }
		*/
		int i;
		int j;
		bool isset = false;
		for(i = 0; i < nIntegerIssueQueueWritePorts; i++)
		  {
		    isset = false;
		    for (j=0; j<Width; j++)
		      {
			if (InstructionMapping[j] == i)
			  {
			    outIntegerInstruction[i].enable = inInstruction[j].enable ;
			    isset = true;
			    break;
			  }
		      }
		    if (!isset)
		      { outIntegerInstruction[i].enable = false ; }
		  }
		
		for(i = 0; i < nFloatingPointIssueQueueWritePorts; i++)
		  {
		    isset = false;
		    for (j=0; j<Width; j++)
		      {
			if (InstructionMapping[j] == (i + nIntegerIssueQueueWritePorts) )
			  {
			    outFloatingPointInstruction[i].enable = inInstruction[j].enable ;
			    isset = true;
			    break;
			  }
		      }
		    if (!isset)
		      { outFloatingPointInstruction[i].enable = false ; }
		  }
		
		for(i = 0; i < nLoadStoreIssueQueueWritePorts; i++)
		  {
		    isset = false;
		    for (j=0; j<Width; j++)
		      {
			if (InstructionMapping[j] == (i + nIntegerIssueQueueWritePorts + nFloatingPointIssueQueueWritePorts) )
			  {
			    outLoadStoreInstruction[i].enable = inInstruction[j].enable ;
			    isset = true;
			    break;
			  }
		      }
		    if (!isset)
		      { outLoadStoreInstruction[i].enable = false ; }
		  }
	      }// end of if (areallknown)
	  }// end of onEnable...


	/** Set the function of the integer issue queue
		@param function a function
	*/
	void SetIntegerIssueQueueFunction(function_t function)
	{
		integerIssueQueueFunction = function;
	}

	/** Set the function of the floating point issue queue
		@param function a function
	*/
	void SetFloatingPointIssueQueueFunction(function_t function)
	{
		floatingPointIssueQueueFunction = function;
	}

	/** Set the function of the load/store issue queue
		@param function a function
	*/
	void SetLoadStoreIssueQueueFunction(function_t function)
	{
		loadStoreIssueQueueFunction = function;
	}

private:
	function_t integerIssueQueueFunction;
	function_t floatingPointIssueQueueFunction;
	function_t loadStoreIssueQueueFunction;
};

} // end of namespace decode
} // end of namespace pipeline
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
