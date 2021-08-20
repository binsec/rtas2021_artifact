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
                    AddressGenerationUnit.h  -  description
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_EXECUTE_ADDRESS_GENERATION_UNIT_MC_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_EXECUTE_ADDRESS_GENERATION_UNIT_MC_HH__

#include <unisim/component/cxx/processor/powerpc/exception.hh>

//#include <unisim/component/clm/processor/ooosim/parameters.hh>
#include <unisim/component/clm/interfaces/instruction_interface.hh>

namespace unisim {
namespace component {
namespace clm {
namespace pipeline {
namespace execute {

 
using unisim::component::clm::interfaces::InstructionPtr;


using unisim::component::clm::processor::ooosim_mc::CPUSim;
using unisim::component::cxx::processor::powerpc::Exception;

using unisim::component::clm::utility::branch_direction_t;
using unisim::component::clm::utility::Taken;
using unisim::component::clm::utility::NotTaken;
using unisim::component::clm::utility::DontKnow;

using unisim::component::clm::utility::exception_type_t;
using unisim::component::clm::utility::NONE_EXCEPTION;
using unisim::component::clm::utility::MISALIGNMENT_EXCEPTION;
using unisim::component::clm::utility::INVALID_OPCODE_EXCEPTION;

 //using full_system::processors::powerpc::Exception;


/** A pipeline entry */
template <class T, int nSources>
class AddressGenerationUnitPipelineEntry
{
	public:
		int delay;	/*< processing delay  */
  //		InstructionPtr<T, nSources> instruction;	/*< instruction being processed */
  //		Instruction *instruction;	/*< instruction being processed */
		InstructionPtr instruction;	/*< instruction being processed */

  AddressGenerationUnitPipelineEntry()
  {
    //    instruction = new Instruction();
    delay = -1;
  }

		/** Compares two pipeline entry
			@param entry the rhs
			@return non-zero if different
		*/
		int operator == (const AddressGenerationUnitPipelineEntry& entry) const
		{
			return entry.instruction->inum == instruction->inum;
		}

		/** Prints a pipeline entry into an output stream
			@param os an output stream
			@param entry a pipeline entry
			@return the output stream
		*/
		friend ostream& operator << (ostream& os, const AddressGenerationUnitPipelineEntry& entry)
		{
			os << "delay=" << entry.delay;
			os << ",instruction=" << entry.instruction;
			return os;
		}
};

/* A SystemC module implementing an address generation unit. It is a template class. */
/* T: data type manipulated by the instructions */
/* nSources: maximum number of source operands for an instruction */
/* nStages: number of pipe stages */
template <class T, int nSources, int nStages, int nUnits, int nConfig=2 >
class AddressGenerationUnit : public module
{
	public:
		/* Clock */
		inclock inClock;

		/* From the Register file (Read Register pipe) */
		inport<InstructionPtr, nConfig*nUnits > inInstruction;

		/* To the Issue queue */
		//		ml_out_accept outAccept;

		/* To the load/store queue */
		outport<InstructionPtr, nConfig*nUnits > outLSQInstruction;

		/* To the common data bus arbiter */
		outport<InstructionPtr, nConfig*nUnits > outCDBInstruction;
		
		/* From the reorder buffer */
		inport<bool, nConfig > inFlush;
		outport<bool, nConfig > outFlush;

		/** The address generation unit constructor
			@param name the module name
		*/
		AddressGenerationUnit(const char *name, CPUSim *spec_state) :
		  module(name)
		{
		  class_name = " AddressGenerationUnitClass";
		  // Unisim port names ...
		  inInstruction.set_unisim_name(this,"inInstruction");
		  outLSQInstruction.set_unisim_name(this,"outLSQInstruction");
		  outCDBInstruction.set_unisim_name(this,"outCDBInstruction");

		  inFlush.set_unisim_name(this,"inFlush");
		  outFlush.set_unisim_name(this,"outFlush");

		  /*
		  sensitive_method(on_Enable_Data) << inInstruction.data
						   << outCDBInstruction.enable 
						   << outLSQInstruction.enable;
		  sensitive_method(on_Accept) << outLSQInstruction.accept
					      << outCDBInstruction.accept;
		  */
		  
		  sensitive_method(on_Data) << inInstruction.data;
		  sensitive_method(on_Accept) << outLSQInstruction.accept
					      << outCDBInstruction.accept;

		  sensitive_method(on_LSQAccept) << outLSQInstruction.accept;
		  
		  sensitive_method(onDataFlush) << inFlush.data;
		  sensitive_method(onAcceptFlush) << outFlush.accept;
		  sensitive_method(onEnableFlush) << inFlush.enable;

		  sensitive_pos_method(start_of_cycle) << inClock;
		  sensitive_neg_method(end_of_cycle) << inClock;

		  
		  for (int cfg=0; cfg<nConfig; cfg++)
		    {
		      for(int unit=0; unit<nUnits; unit++)
			{
			  cumulative_occupancy[cfg][unit] = 0;
			  instruction_to_remove[cfg][unit] = 0;
			  instruction_to_add[cfg][unit] = 0;
			  
			  Accept_have_been_seen[cfg][unit] = false;
			}
		    }
		  this->state = spec_state;

		}

                void on_LSQAccept()
                {
		  if (outLSQInstruction.accept.known())
		  {
		    for (int cfg=0; cfg<nConfig; cfg++)
		      {
			for(int unit=0; unit<nUnits; unit++)
			  {
			    
			    outLSQInstruction.enable[cfg*nUnits+unit] = outLSQInstruction.accept[cfg*nUnits+unit];
			  }
		      }
		    outLSQInstruction.enable.send();
		  }
		}
		/** Returns void and enable an output instruction if it has been accepted
			@return true and enable an output instruction if it has been accepted
		*/ 
                void on_Accept()  //void on_Accept_Data()
                {
		  if ( outLSQInstruction.accept.known() && outCDBInstruction.accept.known() )
		  {
		    for (int cfg=0; cfg<nConfig; cfg++)
		    {
		      for(int unit=0; unit<nUnits; unit++)
		      {
		      
		      /* Get the end of the pipeline */
		      instruction_to_remove[cfg][unit] = 0;

		      AddressGenerationUnitPipelineEntry<T, nSources> *entry = queue[cfg][unit].GetHead();
		      
		      if(entry)
			{
			  /* Get the instruction */
			  //				const InstructionPtr<T, nSources>& instruction = entry->instruction;
			  const InstructionPtr instruction = entry->instruction;
			  
			  /* If instruction is a store, we need an accept from the common data bus and the store queue */
			  if(instruction->fn & FnStore)
			    {
			      if(instruction->exception == NONE_EXCEPTION)
				{
				  // if(inCDBAccept && inLSQAccept)
				  if (outLSQInstruction.accept[cfg*nUnits+unit] && outCDBInstruction.accept[cfg*nUnits+unit])
				    {
				      //      outLSQInstruction.enable = true;
				      outCDBInstruction.enable[cfg*nUnits+unit] = true;
				      instruction_to_remove[cfg][unit] = 1;
				    }
				  else
				    {
				      //      outLSQInstruction.enable = false;
				      outCDBInstruction.enable[cfg*nUnits+unit] = false;
				    }
				}
			      else
				{
				  /* If store get an exception, then store just go to the common data bus */
				  //				  if(inCDBAccept)
				  if (outCDBInstruction.accept[cfg*nUnits+unit])
				    {
				      //      outLSQInstruction.enable = false;
				      outCDBInstruction.enable[cfg*nUnits+unit] = true;
				      instruction_to_remove[cfg][unit] = 1;
				    }
				  else
				    {
				      //      outLSQInstruction.enable = false;
				      outCDBInstruction.enable[cfg*nUnits+unit] = false;
				    }
				}
			    }
			  else
			    {
			      /* If instruction is not a store, it only goes to the load queue */
			      if(instruction->exception == NONE_EXCEPTION)
				{
				  if (outLSQInstruction.accept[cfg*nUnits+unit])
				    {
				      //      outLSQInstruction.enable = true;
				      outCDBInstruction.enable[cfg*nUnits+unit] = false;
				      instruction_to_remove[cfg][unit] = 1;
				    }
				  else
				    {
				      //      outLSQInstruction.enable = false;
				      outCDBInstruction.enable[cfg*nUnits+unit] = false;
				    }
				}
			      else
				{
				  /* If instruction got an exception, it goes to the common data instead of the load queue */
				  if (outCDBInstruction.accept[cfg*nUnits+unit])
				    {
				      //      outLSQInstruction.enable = false;
				      outCDBInstruction.enable[cfg*nUnits+unit] = true;
				      instruction_to_remove[cfg][unit] = 1;
				    }
				  else
				    {
				      //      outLSQInstruction.enable = false;
				      outCDBInstruction.enable[cfg*nUnits+unit] = false;				      
				    }
				}
			    }
			}
		      else
			{
			  //  outLSQInstruction.enable = false;
			  outCDBInstruction.enable[cfg*nUnits+unit] = false;				      
			}
		      }//Endof foreach Unit.      
		    }//Endof foreach Config.      
		    outCDBInstruction.enable.send();
		  } // end of: if (outLSQInstruction.accept.know() && outCDBInstruction.accept.know())
		  
		} // end of: on_Accept()
		  
		  void on_Data()
		  {
		  //		  if ( Accept_have_been_seen && inInstruction.data.known() )
		  if ( inInstruction.data.known() )
		    {

		      for (int cfg=0; cfg<nConfig; cfg++)
		      {
			for(int unit=0; unit<nUnits; unit++)
			{
			  /* Get the end of the pipeline */
			  //		      instruction_to_remove = 0;
			  
			  if ( inInstruction.data[cfg*nUnits+unit].something()
			       //&& ( (instruction_to_remove == 1) || (!queue.Full()) ) 
			       )
			    {
			      inInstruction.accept[cfg*nUnits+unit] = true;
			      instruction_to_add[cfg][unit] = 1;
			    }
			  else
			    {
			      inInstruction.accept[cfg*nUnits+unit] = false;
			      instruction_to_add[cfg][unit] = 0;			    
			    }
			}
		      }
		      inInstruction.accept.send();
		    } // end of: if (outLSQInstruction.accept.know() && outCDBInstruction.accept.know())
		} // end of: on_Accept()


  /*
		void onEnable()
		{
		  if (inInstruction.data.known() && outInstruction.enable.known())
		    {
		      if (inInstruction.data.something())
			{
			  inInstruction.accept = Accept();
			}
		      else
			{
			  inInstruction.accept = false;
			}
		    }
		}
  */

		/** Returns true if we can accept input instruction. Enable output instruction if it is accepted.
			@return true if we can accept input instruction.
		*/
  /*
		bool Accept()
		{
			bool enabled = Enabled();
			return inValid && (enabled || !queue.Full());
		}
  */
		/* //SODD
		void onAccept()
		{
		  outInstruction.enable = inInstruction.accept;
		}
		*/ //EODD
  //		void onDataFlush() { outFlush.data = inFlush.data; }
  /*
                void onDataFlush() { if (inFlush.data.something()) outFlush.data = inFlush.data; else outFlush.data.nothing(); }
		void onAcceptFlush() { inFlush.accept = outFlush.accept; }
		void onEnableFlush() { outFlush.enable = inFlush.enable; }
  */
                void onDataFlush() 
                {
		  for(int cfg=0; cfg<nConfig; cfg++)
		  {
		    if (inFlush.data[cfg].something())
		      outFlush.data[cfg] = inFlush.data[cfg];
		    else outFlush.data[cfg].nothing(); 
		  }
		  outFlush.data.send();
		}
		void onAcceptFlush() 
                {
		  for(int cfg=0; cfg<nConfig; cfg++)
		  {
		    inFlush.accept[cfg] = outFlush.accept[cfg]; 
		  }
		  inFlush.accept.send();
		}

		void onEnableFlush() 
                { 
		  for(int cfg=0; cfg<nConfig; cfg++)
		  {
		    outFlush.enable[cfg] = inFlush.enable[cfg]; 
		  }
		  outFlush.enable.send();
		}

		/** Returns true if output instruction was accepted
			@return true if output instruction was accepted
		*/
  /*
		void onEnable()
		{
		  if (inInstruction.data.known() && outInstruction.enable.known())
		    {
		      if (inInstruction.data.something())
			{
			  inInstruction.accept = Accept();
			}
		      else
			{
			  inInstruction.accept = false;
			}
		    }
		}
  */

		/* A SystemC process (SC_METHOD) generating the accept signal */
		//SODD
		/*
		void ExternalControl()
		{
			// Accept or reject the incoming instructions
			outAccept = Accept();
		}
		*/
		//EODD
		/* A SystemC process (SC_METHOD) managing the address generation unit pipeline */
		//		void InternalControl()
		void end_of_cycle()
		{

		    for (int cfg=0; cfg<nConfig; cfg++)
		    {
		      for(int unit=0; unit<nUnits; unit++)
		      {
			cumulative_occupancy[cfg][unit] += queue[cfg][unit].Size();
		      }

		      /* Flush ? */
		      if(inFlush.enable[cfg] && inFlush.data[cfg].something())
			{
			  if(inFlush.data[cfg])
			    {
#ifdef DD_DEBUG_FLUSH
			      cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====  Flush !!!" << endl;
#endif

			      for(int unit=0; unit<nUnits; unit++)
				{
				  /* Flush the address generation unit pipeline */
				  queue[cfg][unit].Flush();
				  //return;
				  //continue;
				}
			      //return;
			      continue;
			    }
			}
		      for(int unit=0; unit<nUnits; unit++)
		      {

			AddressGenerationUnitPipelineEntry<T, nSources> *entry;
			QueuePointer<AddressGenerationUnitPipelineEntry<T, nSources>, nStages> cur;
			/* Remove accepted instructions */
			if (instruction_to_remove[cfg][unit] == 1)
			  queue[cfg][unit].RemoveHead();
			  
			/* Add incoming instructions */
			//			if(inEnable)
			// if (inInstruction.enable)
			if (inInstruction.enable[cfg*nUnits+unit] && instruction_to_add[cfg][unit] == 1) // Why ? Why not...
			{
			  /* Get the instruction */
			  InstructionPtr instruction = inInstruction.data[cfg*nUnits+unit];

			  if(!instruction->must_reschedule)
			    {
					/* Allocate a pipeline entry */
					entry = queue[cfg][unit].New();
					if(!entry)
					{
						/* Throw an error if there is not enough space into the pipeline */
						fprintf(stderr, "Panic");
						exit(1);
					}
					/* Store the instruction into the pipeline entry */
					entry->instruction = instruction;
					/* Compute the results of the instruction */
					Compute(&(entry->instruction));
					/* Set the processing delay of the instruction to the number of pipe stage */
					entry->delay = nStages;
			    }
			  else
			    {
				    cerr << "(" << timestamp() <<")" << "Instruction must rescheduled !!!" << endl;
				    cerr << instruction << endl;
				    abort();
			    }
			}

			/* Run */
			/* For each entry into the pipeline */
			for(cur = queue[cfg][unit].SeekAtHead(); cur; cur++)
			{
				if(cur->delay > 0)
				{
				  //changed = true;
				  /* Decrement the processing delay until it reach zero */
				  cur->delay--;
				}
			}

#ifdef DD_DEBUG_PIPELINE_VERB1
			cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ==== Pipeline Debug" << endl;
			cerr << this << endl;
#endif
		      }//Endof foreach Unit.
		    }//Endof foreach Config..
		    
		}// end of end_of_cycle

		void start_of_cycle()
		{
		  for (int cfg=0; cfg<nConfig; cfg++)
		  {
		    for(int unit=0; unit<nUnits; unit++)
		    {
			AddressGenerationUnitPipelineEntry<T, nSources> *entry;
			/* Common Data Bus request and load/store queue request */
			entry = queue[cfg][unit].GetHead();
			/* If instruction processing is finished */
			if(entry && entry->delay == 0)
			{
			  //	const InstructionPtr<T, nSources>& instruction = entry->instruction;
				const InstructionPtr instruction = entry->instruction;

				/* If instruction dit not get an exception */
				if(instruction->exception == NONE_EXCEPTION)
				{
					/* If instruction is a store */
					if(instruction->fn & FnStore)
					{
					  /* Do a request to the common data bus */
					  //outCDBValid = true;
					  outCDBInstruction.data[cfg*nUnits+unit] = instruction;
					}
					else
					{
					  //outCDBValid = false;
					  outCDBInstruction.data[cfg*nUnits+unit].nothing();
					}
					/* For both load and load, do a request to the load/store queue */
					//outLSQValid = true;
					outLSQInstruction.data[cfg*nUnits+unit] = instruction;
				}
				else
				{
					/* Instruction got an exception */
					/* Do a request to the common data bus */
					//outCDBInstruction = instruction;
					//outCDBValid = true;
					//outLSQValid = false;
					outCDBInstruction.data[cfg*nUnits+unit] = instruction;
					outLSQInstruction.data[cfg*nUnits+unit].nothing();
				}
			}
			else
			{
			  //outCDBValid = false;
			  //outLSQValid = false;
			  outCDBInstruction.data[cfg*nUnits+unit].nothing();
			  outLSQInstruction.data[cfg*nUnits+unit].nothing();
			}

			/* If state changed then make the SystemC scheduler call
			the ExternalControl process handling the valid, accept and enable hand-shaking */
			/*
			if(changed)
				outStateChanged = !inStateChanged;
			*/
		    }//Endof foreach Unit.
		  }//Endof foreach Config.
		  outCDBInstruction.data.send();
		  outLSQInstruction.data.send();
		}

		/** Check for misalignment memory accesses
			@param instruction an instruction
			@return true if memory access is misaligned
		*/
  //		bool Misalignment(const InstructionPtr<T, nSources>& instruction)
  //		bool Misalignment(const Instruction *instruction)
		bool Misalignment(InstructionPtr instruction)
		{
		  //return instruction->ea & (instruction->operation->memory_access_size - 1);
			return instruction->ea & (instruction->operation->memory_access_size() - 1);
		}

		/** A Method doing the computation for an instruction */
  //		void Compute(InstructionPtr<T, nSources>& instruction)
		void Compute(InstructionPtr *instruction)
		{
			/* Here, we must call the emulator */
		  //			static state_t state;
			int i;
			/* Copy the register values into the temporary state used by the emulator */
			/* For each source operands */
			for(i = 0; i < (*instruction)->sources.size(); i++)
			{
				/* Get the architectural register number of the source operand */
				int srcreg = (*instruction)->sources[i].reg;
				if(srcreg >= 0)
				{
					switch((*instruction)->sources[i].type)
					{
						case GPR_T:
							/* Copy the integer register value of the simulator into the state of the emulator */
							//state.gpr[srcreg] = (*instruction)->sources[i].data;
						  state->SetGPR(srcreg,(*instruction)->sources[i].data);
							break;

						case FPR_T:
							/* Copy the floating point register value of the simulator into the state of the emulator */
							//state.fpr[srcreg] = *(double *) &(*instruction)->sources[i].data;
						  state->SetFPR(srcreg, SoftDouble(*(uint64_t *)&(*instruction)->sources[i].data));
							break;

						default:
							cerr << "register is neither an integer nor a floating point register" << endl;
							abort();//exit(-1);
					}
				}
			}
			/* Compute the next instruction address of the instruction */
			//state.nia = (*instruction)->cia + 4;
			state->SetNIA((*instruction)->cia + 4);
			/* Call the emulator to execute the instruction and compute the results and the next instruction address for branch instructions */
			//(*instruction)->operation->execute((*instruction)->operation, &state, NULL /* no memory */);
			try{
			  (*instruction)->operation->execute(state);//(*instruction)->operation, &state, NULL /* no memory */);
			}
			catch(Exception &e)
			  {
			    // Normaly we catch a Alignment Exception
#ifdef DD_DEBUG_ALIGNMENT
			    cerr << "["<< this->name() << "] (" << timestamp() << ") --- Exception : " << e.what() << endl;
#endif
			  }
			/* Copy the results */
			for (int j=0; j<(*instruction)->destinations.size(); j++)
			{
			  int dstreg = (*instruction)->destinations[j].reg;
			  if(dstreg >= 0)
			  {
					switch((*instruction)->destinations[j].type)
					{
						case GPR_T:
							/* Copy the integer register value back to the instruction */
							//(*instruction)->destination.data = state.gpr[dstreg];
							(*instruction)->destinations[j].data = state->GetGPR(dstreg);
							break;

						case FPR_T:
							/* Copy the floating point register value back to the instruction */
							//(*instruction)->destination.data = *(T *) &state.fpr[dstreg];
							//(*instruction)->destination.data = *((T *) (state->GetFPR(dstreg)));
							//(*instruction)->destinations[j].data = *(uint64_t *)&state->GetFPR(dstreg);
							(*instruction)->destinations[j].data = state->GetFPR(dstreg).queryValue();
							break;

						default:
							cerr << "register is neither an integer nor a floating point register" << endl;
							abort();//exit(-1);
					}
			  }
			}
			/* Copy back the effective address */
			//			(*instruction)->ea = state.ea;
			(*instruction)->ea = (*instruction)->operation->loadstore_effective_address(state);

			/* Copy back the next instruction address */
			//			(*instruction)->nia = state.nia;
			(*instruction)->nia = state->GetNIA();
			
			if ( ((*instruction)->operation->function == FnStoreFloat) 
			     && ((*instruction)->operation->memory_access_size() == 4)
			     && ((*instruction)->operation->store_need_conversion())
			     )
			  {
			    (*instruction)->singleprecision = (uint32_t)((*instruction)->operation->get_single(state));
			  }
			/*
			if ( ((*instruction)->operation->function == FnLoadFloat) 
			     && ((*instruction)->operation->memory_access_size() == 4)
			     && ((*instruction)->operation->load_need_conversion())
			     )
			  {
			    (*instruction)->singleprecision = (uint32_t)((*instruction)->operation->get_single(state));
			  }
			*/
			/* If instruction memory access is misaligned */
			// We ignore misalignment because memory sub-system is able to handle them... 
			/*
			if(Misalignment(instruction))
			{
			  // Ignore the misalignment if it is a prefetch load
				if((*instruction)->fn & FnPrefetchLoad)
				{
					// Align the prefetch load
					(*instruction)->ea = (*instruction)->ea & ~((*instruction)->operation->memory_access_size() - 1);
				}
				else
				{
				  // load/store got a misalignment exception
					(*instruction)->exception = MISALIGNMENT_EXCEPTION;
				}
			}
			*/
		}

		/** Returns the function supported by the address generation unit
			@return the function supported by the address generation unit
		*/
		function_t GetFunction()
		{
		  // ??? What is the new Function ?
		  //			return FnLoadStore | FnPrefetchLoad;
			return FnLoad | FnStore | FnPrefetchLoad;
		}

		/** Prints the address generation unit state into an output stream
			@param os an output stream
			@param agu an address generation unit module
			@return the output stream
		*/
		friend ostream& operator << (ostream& os, const AddressGenerationUnit& agu)
		{
		  for (int cfg=0; cfg<nConfig; cfg++)
		  {
		    os << "========= [Config::"<<cfg<<"] " << agu.name() << "==========" << endl;
		    for (int unit=0; unit<nUnits; unit++)
		      {
			os << "pipeline["<<unit<<"]:" << endl;
			os << agu.queue[cfg][unit] << endl;
		      }
		    os << "==========================" << endl;
		  }
		  return os;
		}

		/** Performs a sanity check on the address generation unit
			@return true if ok
		*/
		bool Check()
		{
			return true;
		}

		/** Prints statistics into an output stream
			@param os an output stream
		*/
		void DumpStats(ostream& os, uint64_t sim_total_time, uint64_t sim_cycle)
		{
			os << name() << "_cumulative_occupancy " << cumulative_occupancy << " # " << name() << " cumulative occupancy" << endl;
			os << name() << "_occupancy " << ((double) cumulative_occupancy / (double) sim_cycle) << " # " << name() << " occupancy (instructions/cycles)" << endl;
		}

		void WarmRestart()
		{
			for (int cfg=0; cfg<nConfig; cfg++)
			  {
			    for(int unit=0; unit<nUnits; unit++)
			      queue[cfg][unit].Reset();
			  }
		}
		
		void ResetStats()
		{
			for (int cfg=0; cfg<nConfig; cfg++)
			  {
			    for(int unit=0; unit<nUnits; unit++)
			      cumulative_occupancy[cfg][unit] = 0;
			  }
		}
	
	private:
		/* Some ports to make the SystemC scheduler call the ExternalControl process on state changes */
		/*
		ml_out_data<bool> outStateChanged;
		ml_signal_data<bool> stateChanged;
		ml_in_data<bool> inStateChanged;
		*/
		/* A queue modeling the address generation unit pipeline */
		Queue<AddressGenerationUnitPipelineEntry<T, nSources>, nStages> queue[nConfig][nUnits];

		//bool changed;
		uint64_t cumulative_occupancy[nConfig][nUnits];
  
                int instruction_to_remove[nConfig][nUnits];
                int instruction_to_add[nConfig][nUnits];
  
                bool Accept_have_been_seen[nConfig][nUnits];
  
                CPUSim *state;
};

} // end of namespace execute
} // end of namespace pipeline
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
