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

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_EXECUTE_ADDRESS_GENERATION_UNIT_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_EXECUTE_ADDRESS_GENERATION_UNIT_HH__

/*
#include <systemc>
#include <types.h>
#include <utility.h>
#include <common.h>
*/
#include <unisim/component/cxx/processor/powerpc/exception.hh>

#include <unisim/component/clm/processor/ooosim/parameters.hh>
#include <unisim/component/clm/interfaces/instruction_interface.hh>

namespace unisim {
namespace component {
namespace clm {
namespace pipeline {
namespace execute {

 
using unisim::component::clm::interfaces::InstructionPtr;


using unisim::component::clm::processor::ooosim::CPUSim;
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
template <class T, int nSources, int nStages>
class AddressGenerationUnit : public module
{
	public:
		/* Clock */
		inclock inClock;

		/* From the Register file (Read Register pipe) */
		/*
		ml_in_valid inValid;
		ml_in_enable inEnable;
		ml_in_data<InstructionPtr<T, nSources> > inInstruction;
		*/
  //		inport<InstructionPtr<T, nSources> > inInstruction;
		inport<InstructionPtr> inInstruction;

		/* To the Issue queue */
		//		ml_out_accept outAccept;

		/* To the load/store queue */
		/*
		ml_out_valid outLSQValid;
		ml_out_enable outLSQEnable;
		ml_out_data<InstructionPtr<T, nSources> > outLSQInstruction;
		ml_in_accept inLSQAccept;
		*/
  //		outport<InstructionPtr<T, nSources> > outLSQInstruction;
		outport<InstructionPtr> outLSQInstruction;

		/* To the common data bus arbiter */
		/*
		ml_out_valid outCDBValid;
		ml_out_enable outCDBEnable;
		ml_out_data<InstructionPtr<T, nSources> > outCDBInstruction;
		ml_in_accept inCDBAccept;
		*/
  //		outport<InstructionPtr<T, nSources> > outCDBInstruction;
		outport<InstructionPtr> outCDBInstruction;
		
		/* From the reorder buffer */
		//		ml_in_flush inFlush;
		inport<bool> inFlush;
		outport<bool> outFlush;

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
			outStateChanged(stateChanged);
			inStateChanged(stateChanged);

			SC_HAS_PROCESS(AddressGenerationUnit);

			// Make the ExternalControl process sensitive to the input instructions and acknowledge
			SC_METHOD(ExternalControl);
			sensitive << inStateChanged;
			sensitive << inValid << inCDBAccept << inLSQAccept;

			// Make the InternalControl sensitive to the clock front edge
			SC_METHOD(InternalControl);
			sensitive_pos << inClock;
		  */
		  //changed = true;
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

		  
		  cumulative_occupancy = 0;
		  instruction_to_remove = 0;
		  instruction_to_add = 0;

		  Accept_have_been_seen = false;

		  this->state = spec_state;

		}

                void on_LSQAccept()
                {
		  if (outLSQInstruction.accept.known())
		    {
		      outLSQInstruction.enable = outLSQInstruction.accept;
		    }
		}
		/** Returns void and enable an output instruction if it has been accepted
			@return true and enable an output instruction if it has been accepted
		*/ 
                void on_Accept()
		//void on_Accept_Data()
                {
		  if ( outLSQInstruction.accept.known() && outCDBInstruction.accept.known() )
		    // && inInstruction.data.known() )
		    {
		      /* Get the end of the pipeline */
		      instruction_to_remove = 0;

		      AddressGenerationUnitPipelineEntry<T, nSources> *entry = queue.GetHead();
		      
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
				  if (outLSQInstruction.accept && outCDBInstruction.accept)
				    {
				      //      outLSQInstruction.enable = true;
				      outCDBInstruction.enable = true;
				      instruction_to_remove = 1;
				    }
				  else
				    {
				      //      outLSQInstruction.enable = false;
				      outCDBInstruction.enable = false;
				    }
				}
			      else
				{
				  /* If store get an exception, then store just go to the common data bus */
				  //				  if(inCDBAccept)
				  if (outCDBInstruction.accept)
				    {
				      //      outLSQInstruction.enable = false;
				      outCDBInstruction.enable = true;
				      instruction_to_remove = 1;
				    }
				  else
				    {
				      //      outLSQInstruction.enable = false;
				      outCDBInstruction.enable = false;
				    }
				}
			    }
			  else
			    {
			      /* If instruction is not a store, it only goes to the load queue */
			      if(instruction->exception == NONE_EXCEPTION)
				{
				  if (outLSQInstruction.accept)
				    {
				      //      outLSQInstruction.enable = true;
				      outCDBInstruction.enable = false;
				      instruction_to_remove = 1;
				    }
				  else
				    {
				      //      outLSQInstruction.enable = false;
				      outCDBInstruction.enable = false;
				    }
				}
			      else
				{
				  /* If instruction got an exception, it goes to the common data instead of the load queue */
				  if (outCDBInstruction.accept)
				    {
				      //      outLSQInstruction.enable = false;
				      outCDBInstruction.enable = true;
				      instruction_to_remove = 1;
				    }
				  else
				    {
				      //      outLSQInstruction.enable = false;
				      outCDBInstruction.enable = false;				      
				    }
				}
			    }
			}
		      else
			{
			  //  outLSQInstruction.enable = false;
			  outCDBInstruction.enable = false;				      
			}
		      /*
			if (instruction_to_remove == 0)
			{
			  outLSQInstruction.enable = false;
			  outCDBInstruction.enable = false;
			}
		      */
		      
		      //Accept_have_been_seen = true;
		    } // end of: if (outLSQInstruction.accept.know() && outCDBInstruction.accept.know())
		  
		  } // end of: on_Accept()
		  
		  void on_Data()
		  {
		  //		  if ( Accept_have_been_seen && inInstruction.data.known() )
		  if ( inInstruction.data.known() )
		    {
		      /* Get the end of the pipeline */
		      //		      instruction_to_remove = 0;

			if ( inInstruction.data.something()
			     //&& ( (instruction_to_remove == 1) || (!queue.Full()) ) 
			     )
			  {
			    inInstruction.accept = true;
			    instruction_to_add = 1;
			  }
			else
			  {
			    inInstruction.accept = false;
			    instruction_to_add = 0;			    
			  }
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
                void onDataFlush() { if (inFlush.data.something()) outFlush.data = inFlush.data; else outFlush.data.nothing(); }
		void onAcceptFlush() { inFlush.accept = outFlush.accept; }
		void onEnableFlush() { outFlush.enable = inFlush.enable; }

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
			AddressGenerationUnitPipelineEntry<T, nSources> *entry;
			QueuePointer<AddressGenerationUnitPipelineEntry<T, nSources>, nStages> cur;

			cumulative_occupancy += queue.Size();

			//if(!inCDBAccept && !inLSQAccept && !inEnable && !inFlush && !changed) return;
			//changed = false;

			/* Flush ? */
			if(inFlush.enable && inFlush.data.something())
			{
			  if(inFlush.data)
			  {
#ifdef DD_DEBUG_FLUSH
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====  Flush !!!" << endl;
#endif
			    //changed = true;
				/* Flush the address generation unit pipeline */
				queue.Flush();

				/* Reset all output ports */
				/*
				outCDBValid = false;
				outLSQValid = false;
				outCDBEnable = false;
				outLSQEnable = false;
				outAccept = false;
				*/
				return;
			  }
			}

			/* Remove accepted instructions */
			/*
			if(HasAccept())
			{
				queue.RemoveHead();
				//changed = true;
			}
			*/
			if (instruction_to_remove == 1)
			  queue.RemoveHead();
			  
			/* Add incoming instructions */
			//			if(inEnable)
			// if (inInstruction.enable)
			if (inInstruction.enable && instruction_to_add == 1) // Why ? Why not...
			{
			  //changed = true;
				/* Get the instruction */
			  InstructionPtr instruction = inInstruction.data;
			  //			  const Instruction *instruction = &inst;

			  // For Dump
			  instruction->timing_execute_cycle = timestamp();	

				if(!instruction->must_reschedule)
				{
					/* Allocate a pipeline entry */
					entry = queue.New();
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
					
/*					if(entry->instruction->fn & FnStore && entry->instruction->ea == 0x000000014036eee8ULL)
					{
						cerr << "!!!!!!! time stamp = " << sc_time_stamp() << ": store (" << entry->instruction << ") at 0x000000014036eee8" << endl;
					}*/
	
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
			for(cur = queue.SeekAtHead(); cur; cur++)
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
		}// end of end_of_cycle

		void start_of_cycle()
		{
			AddressGenerationUnitPipelineEntry<T, nSources> *entry;
			/* Common Data Bus request and load/store queue request */
			entry = queue.GetHead();
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
					  outCDBInstruction.data = instruction;
					}
					else
					{
					  //outCDBValid = false;
					  outCDBInstruction.data.nothing();
					}
					/* For both load and load, do a request to the load/store queue */
					//outLSQValid = true;
					outLSQInstruction.data = instruction;
				}
				else
				{
					/* Instruction got an exception */
					/* Do a request to the common data bus */
					//outCDBInstruction = instruction;
					//outCDBValid = true;
					//outLSQValid = false;
					outCDBInstruction.data = instruction;
					outLSQInstruction.data.nothing();
				}
			}
			else
			{
			  //outCDBValid = false;
			  //outLSQValid = false;
			  outCDBInstruction.data.nothing();
			  outLSQInstruction.data.nothing();
			}

			/* If state changed then make the SystemC scheduler call
			the ExternalControl process handling the valid, accept and enable hand-shaking */
			/*
			if(changed)
				outStateChanged = !inStateChanged;
			*/
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
						  state->SetFPR(srcreg, unisim::component::cxx::processor::powerpc::SoftDouble(*(uint64_t *)&(*instruction)->sources[i].data));
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
			os << "=====" << agu.name() << "=====" << endl;
			os << "pipeline:" << endl;
			os << agu.queue << endl;
			os << "==========================" << endl;
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
			queue.Reset();
			//			changed = true;
		}
		
		void ResetStats()
		{
			cumulative_occupancy = 0;
		}
	
	private:
		/* Some ports to make the SystemC scheduler call the ExternalControl process on state changes */
		/*
		ml_out_data<bool> outStateChanged;
		ml_signal_data<bool> stateChanged;
		ml_in_data<bool> inStateChanged;
		*/
		/* A queue modeling the address generation unit pipeline */
		Queue<AddressGenerationUnitPipelineEntry<T, nSources>, nStages> queue;

		//bool changed;
		uint64_t cumulative_occupancy;
  
                int instruction_to_remove;
                int instruction_to_add;
  
  bool Accept_have_been_seen;
  
                CPUSim *state;
};

} // end of namespace execute
} // end of namespace pipeline
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
