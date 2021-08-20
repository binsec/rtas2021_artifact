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

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_DECODE_DISPATCHER_MC_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_DECODE_DISPATCHER_MC_HH__

#include <unisim/component/clm/processor/ooosim_mc/parameters.hh>
#include <unisim/component/clm/interfaces/instruction_interface.hh>


namespace unisim {
namespace component {
namespace clm {
namespace pipeline {
namespace decode {

using unisim::component::clm::interfaces::InstructionPtr;

using unisim::component::clm::processor::ooosim_mc::nConditionArchitecturalRegisters;

  //using unisim::component::clm::::InstructionPtr;

  //  using unisim::component::clm::utility::hexa;




/** A SystemC module to dispatch instruction in the good issue queue */
//template <class T, int nSources, int Width, int nIntegerIssueQueueWritePorts, int nFloatingPointIssueQueueWritePorts, int nLoadStoreIssueQueueWritePorts>
template <int Width, int nIntegerIssueQueueWritePorts, int nFloatingPointIssueQueueWritePorts, int nLoadStoreIssueQueueWritePorts, int nConfig = 2>
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
        inport<InstructionPtr, nConfig*Width> inInstruction;

	/* From/to the integer issue queue */
	/*
	ml_out_valid outIntegerValid[nIntegerIssueQueueWritePorts];
	ml_out_enable outIntegerEnable[nIntegerIssueQueueWritePorts];
	ml_out_data<InstructionPtr<T, nSources> > outIntegerInstruction[nIntegerIssueQueueWritePorts];
	ml_in_accept inIntegerAccept[nIntegerIssueQueueWritePorts];
	*/
	outport<InstructionPtr, nConfig*nIntegerIssueQueueWritePorts> outIntegerInstruction;
	
	/* From/to the floating point issue queue */
	/*
	ml_out_valid outFloatingPointValid[nFloatingPointIssueQueueWritePorts];
	ml_out_enable outFloatingPointEnable[nFloatingPointIssueQueueWritePorts];
	ml_out_data<InstructionPtr<T, nSources> > outFloatingPointInstruction[nFloatingPointIssueQueueWritePorts];
	ml_in_accept inFloatingPointAccept[nFloatingPointIssueQueueWritePorts];
	*/
	outport<InstructionPtr, nConfig*nFloatingPointIssueQueueWritePorts> outFloatingPointInstruction;

	/* From/to the load/store issue queue */
	/*
	ml_out_valid outLoadStoreValid[nLoadStoreIssueQueueWritePorts];
	ml_out_enable outLoadStoreEnable[nLoadStoreIssueQueueWritePorts];
	ml_out_data<InstructionPtr<T, nSources> > outLoadStoreInstruction[nLoadStoreIssueQueueWritePorts];
	ml_in_accept inLoadStoreAccept[nLoadStoreIssueQueueWritePorts];
	*/
	outport<InstructionPtr, nConfig*nLoadStoreIssueQueueWritePorts> outLoadStoreInstruction;

	int IntegerMapping[nConfig][nIntegerIssueQueueWritePorts];
	int FloatingMapping[nConfig][nFloatingPointIssueQueueWritePorts];
	int LoadStoreMapping[nConfig][nLoadStoreIssueQueueWritePorts];
	int InstructionMapping[nConfig][Width];

	/** the constructor
		@param name the module name
	*/
	Dispatcher(const char * name) : module(name)
	{
		int i, j;

		class_name = " DispatcherClass";
		// Unisim port names ...
		//for (i=0; i<Width; i++)
		//  {
		inInstruction.set_unisim_name(this,"inInstruction");
		//  }		
		//for (i=0; i<nIntegerIssueQueueWritePorts; i++)
		//  {
		outIntegerInstruction.set_unisim_name(this,"outIntegerInstruction");
		//  }
		//for (i=0; i<nFloatingPointIssueQueueWritePorts; i++)
		//  {
		outFloatingPointInstruction.set_unisim_name(this,"outFloatingPointInstruction");
		//  }
		//for (i=0; i<nIntegerIssueQueueWritePorts; i++)
		//  {
		outLoadStoreInstruction.set_unisim_name(this,"outLoadStoreInstruction");
		//  }

		//for(i = 0; i < Width; i++)
		//  {
		sensitive_method(onData) << inInstruction.data;
		//  }
		//for(i = 0; i < Width; i++)
		//  {
		sensitive_method(onEnable) << inInstruction.enable;
		//  }
		//for(i = 0; i < nIntegerIssueQueueWritePorts; i++)
		//  {
		sensitive_method(onAccept) << outIntegerInstruction.accept;
		//  }
		//for(i = 0; i < nFloatingPointIssueQueueWritePorts; i++)
		//  {
		sensitive_method(onAccept) << outFloatingPointInstruction.accept;
		//  }
		//for(i = 0; i < nLoadStoreIssueQueueWritePorts; i++)
		//  {
		sensitive_method(onAccept) << outLoadStoreInstruction.accept;
		//  }

		integerIssueQueueFunction = FnInvalid;
		floatingPointIssueQueueFunction = FnInvalid;
		loadStoreIssueQueueFunction = FnInvalid;
		// --- Latex rendering hints -----------------
		/*
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
		*/
	}

	/** A SystemC process for managing the valid, accept and enable hand-shaking */
	//	void ExternalControl()
	void onData()
	  {
	    /*
	    bool areallknown(true);

	    for (int i=0; i<Width; i++)
	      {
		if (!inInstruction[i].data.known()) { areallknown = false; break; }
	      }
	    */
	    //if (areallknown)
	    if (inInstruction.data.known())
	      {
		for(int cfg=0; cfg<nConfig; cfg++)
		{
		int i,j,k,l;

		// Initialize the port mapping
		for(i = 0; i < Width; i++)
		{
			InstructionMapping[cfg][i] = -1;
		}
		for(i = 0; i < nIntegerIssueQueueWritePorts; i++)
		  {
		    IntegerMapping[cfg][i] = Width;
		  }
		for(i = 0; i < nFloatingPointIssueQueueWritePorts; i++)
		  {
		    FloatingMapping[cfg][i] = Width;
		  }
		for(i = 0; i < nLoadStoreIssueQueueWritePorts; i++)
		  {
		    LoadStoreMapping[cfg][i] = Width;
		  }
		//		for (int i=0; i<Width; i++)

		for(i = j = k = l = 0; i < Width; i++)
		{
		  {
		    if ( !inInstruction.data[cfg*Width+i].something() )
		      {
			//Instruction[i].data.nothing();
		      }
		    else
		      {
			InstructionPtr instruction = inInstruction.data[cfg*Width+i];
			//const Instruction *instruction = &inst;
			if(instruction->fn & integerIssueQueueFunction)
			  {
			    if(j < nIntegerIssueQueueWritePorts)
			      {
				IntegerMapping[cfg][j] = i;
				InstructionMapping[cfg][i] = j;
				outIntegerInstruction.data[cfg*nIntegerIssueQueueWritePorts+j]
				  = inInstruction.data[cfg*Width+i];
				j++;
			      }
			  }
			else if(instruction->fn & floatingPointIssueQueueFunction)
			  {
			    if(k < nFloatingPointIssueQueueWritePorts)
			      {
				FloatingMapping[cfg][k] = i;
				InstructionMapping[cfg][i] = k+nIntegerIssueQueueWritePorts;
				outFloatingPointInstruction.data[cfg*nFloatingPointIssueQueueWritePorts+k]
				  = inInstruction.data[cfg*Width+i];
				k++;
			      }
			  }
			else if(instruction->fn & loadStoreIssueQueueFunction)
			  {
			    if(l < nLoadStoreIssueQueueWritePorts)
			      {
				LoadStoreMapping[cfg][l] = i;
				InstructionMapping[cfg][i] = l+nIntegerIssueQueueWritePorts+nFloatingPointIssueQueueWritePorts;
				outLoadStoreInstruction.data[cfg*nLoadStoreIssueQueueWritePorts+l] 
				  = inInstruction.data[cfg*Width+i];
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
		    outIntegerInstruction.data[cfg*nIntegerIssueQueueWritePorts+j].nothing();
		  }
		for( ; k < nFloatingPointIssueQueueWritePorts; k++)
		  {
		    outFloatingPointInstruction.data[cfg*nFloatingPointIssueQueueWritePorts+k].nothing();
		  }
		for( ; l < nLoadStoreIssueQueueWritePorts; l++)
		  {
		    outLoadStoreInstruction.data[cfg*nLoadStoreIssueQueueWritePorts+l].nothing();
		  }
		}// endof foreach Config.
		outIntegerInstruction.data.send();
		outFloatingPointInstruction.data.send();
		outLoadStoreInstruction.data.send();
	      }// endof if(areallknown)
	  } // endof onData()

	void onAccept()
	  {
	    bool areallknown(true);
	    int i;
	    /*
	    for (int i=0; i<Width; i++)
	      {
		if (!outInstruction[i].accept) { areallknown =false; break; }
		}*/
	    /*
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
	    */
	    //	    if (areallknown)
	    if ( outIntegerInstruction.accept.known() &&
		 outFloatingPointInstruction.accept.known() &&
		 outLoadStoreInstruction.accept.known()
		 )
	      {
		for(int cfg=0; cfg<nConfig; cfg++)
		{
		for (i = 0; i<Width; i++)
		  {
		    int j;
		    bool isset = false;
		    for (j=0; j<nIntegerIssueQueueWritePorts; j++)
		    {
		      if ( (IntegerMapping[cfg][j]==i) && 
			   outIntegerInstruction.accept[cfg*nIntegerIssueQueueWritePorts+j] )
			{ 
			  inInstruction.accept[cfg*Width+i] = true;
			  isset = true;
			  break;
			}
		    }
		    //		    if (!isset)
		    //			{ inInstruction[i].accept = false; }
		    //		    bool isset = false;
		    if (!isset)
		    for (j=0; j<nFloatingPointIssueQueueWritePorts; j++)
		    {
		      if ( (FloatingMapping[cfg][j]==i) && 
			   outFloatingPointInstruction.accept[cfg*nFloatingPointIssueQueueWritePorts+j] )
			{
			  inInstruction.accept[cfg*Width+i] = true;
			  isset = true;
			  break;
			}
		    }
		    //		    if (!isset)
		    //			{ inInstruction[i].accept = false; }
		    //		    bool isset = false;
		    if (!isset)
		    for (j=0; j<nLoadStoreIssueQueueWritePorts; j++)
		    {
		      if ( (LoadStoreMapping[cfg][j]==i) &&
			   outLoadStoreInstruction.accept[cfg*nLoadStoreIssueQueueWritePorts+j] )
			{ 
			  inInstruction.accept[cfg*Width+i] = true; 
			  isset = true;
			  break;
			}
		    }
		    // 
		    if (!isset)
		      { inInstruction.accept[cfg*Width+i] = false; }
		  }
		/*
		for (int i=0; i<Width; i++)
		  {
		    inInstruction[i].enable = outInstruction[i].accept;
		  }
		*/
		} //End of foreach Config.
		inInstruction.accept.send();
	      }// end of if(areallknown)
	  }// end of onAccept()

	void onEnable()
	  {
	    /*
	    bool areallknown(true);
	    for (int i=0; i<Width; i++)
	      {
		if (!inInstruction[i].enable.known()) { areallknown = false; break; }
	      }
	    */
	    //	    if (areallknown)
	    if ( inInstruction.enable.known() )
	      {
		for(int cfg=0; cfg<nConfig; cfg++)
		{
		int i;
		int j;
		bool isset = false;
		for(i = 0; i < nIntegerIssueQueueWritePorts; i++)
		  {
		    isset = false;
		    for (j=0; j<Width; j++)
		      {
			if (InstructionMapping[cfg][j] == i)
			  {
			    outIntegerInstruction.enable[cfg*nIntegerIssueQueueWritePorts+i]
			      = inInstruction.enable[cfg*Width+j] ;
			    isset = true;
			    break;
			  }
		      }
		    if (!isset)
		      { outIntegerInstruction.enable[cfg*nIntegerIssueQueueWritePorts+i] = false ; }
		  }
		
		for(i = 0; i < nFloatingPointIssueQueueWritePorts; i++)
		  {
		    isset = false;
		    for (j=0; j<Width; j++)
		      {
			if (InstructionMapping[cfg][j] == (i + nIntegerIssueQueueWritePorts) )
			  {
			    outFloatingPointInstruction.enable[cfg*nFloatingPointIssueQueueWritePorts+i]
			      = inInstruction.enable[cfg*Width+j] ;
			    isset = true;
			    break;
			  }
		      }
		    if (!isset)
		      { outFloatingPointInstruction.enable[cfg*nFloatingPointIssueQueueWritePorts+i] = false ; }
		  }
		
		for(i = 0; i < nLoadStoreIssueQueueWritePorts; i++)
		  {
		    isset = false;
		    for (j=0; j<Width; j++)
		      {
			if (InstructionMapping[cfg][j] == (i + nIntegerIssueQueueWritePorts + nFloatingPointIssueQueueWritePorts) )
			  {
			    outLoadStoreInstruction.enable[cfg*nLoadStoreIssueQueueWritePorts+i]
			      = inInstruction.enable[cfg*Width+j] ;
			    isset = true;
			    break;
			  }
		      }
		    if (!isset)
		      { outLoadStoreInstruction.enable[cfg*nLoadStoreIssueQueueWritePorts+i] = false ; }
		  }
		}// Endof foareach Config.
		outIntegerInstruction.enable.send();
		outFloatingPointInstruction.enable.send();
		outLoadStoreInstruction.enable.send();
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
