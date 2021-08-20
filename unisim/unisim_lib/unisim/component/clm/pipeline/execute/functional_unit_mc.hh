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
              FunctionalUnit.hh  -  A pipelined functional unit
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_EXECUTE_FUNCTIONAL_UNIT_MC_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_EXECUTE_FUNCTIONAL_UNIT_MC_HH__

#include <unisim/component/cxx/processor/powerpc/exception.hh>
#include <unisim/component/cxx/processor/powerpc/floating.hh>
#include <unisim/util/simfloat/floating.tcc>

//#include <unisim/component/clm/processor/ooosim/parameters.hh>
#include <unisim/component/clm/interfaces/instruction_interface.hh>


namespace unisim {
namespace component {
namespace clm {
namespace pipeline {
namespace execute {


  //using full_system::processors::powerpc::Exception;

using unisim::component::clm::interfaces::InstructionPtr;


using unisim::component::clm::processor::ooosim_mc::CPUSim;
using unisim::component::cxx::processor::powerpc::Exception;
using unisim::component::cxx::processor::powerpc::SoftDouble;

using unisim::component::clm::utility::branch_direction_t;
using unisim::component::clm::utility::Taken;
using unisim::component::clm::utility::NotTaken;
using unisim::component::clm::utility::DontKnow;


/** A pipeline entry */
template <class T, int nSources, int nStages>
class FunctionalUnitPipelineEntry
{
public:
	int stage;			/*< stage number */
	int delay[nStages];	/*< processing delay in each pipe stage */
	//	InstructionPtr<T, nSources> instruction;	/*< instruction being processed */
	InstructionPtr instruction;	/*< instruction being processed */

	/** Compares two pipeline entry
		@param entry the rhs
		@return non-zero if different
	*/
	int operator == (const FunctionalUnitPipelineEntry& entry) const
	{
		return entry.instruction->inum == instruction->inum;
	}

	/** Prints a pipeline entry into an output stream
		@param os an output stream
		@param entry a pipeline entry
		@return the output stream
	*/
	friend ostream& operator << (ostream& os, const FunctionalUnitPipelineEntry& entry)
	{
		int i;
		os << "stage=" << entry.stage << ",delay=";
		for(i = 0; i < nStages; i++)
			os << entry.delay[i] << " ";
		os << ",instruction=" << entry.instruction;
		return os;
	}
};

/* A SystemC module implementing a functional unit. It is a template class. */
/* T: data type manipulated by the instructions */
/* nSources: maximum number of source operands for an instruction */
/* nStages: number of pipe stages */
template <class T, int nSources, int nStages, int nUnits, int nConfig=2 >
class FunctionalUnit : public module
{
	public:
		/* Clock */
		inclock inClock;

		/* From the Register file (Read Register pipe) */
		inport<InstructionPtr, nConfig*nUnits > inInstruction;

		/* To the register file */
		//		ml_out_accept outAccept;

		/* To the common data bus arbiter */
		outport<InstructionPtr, nConfig*nUnits > outInstruction;

		/* From the common data bus arbiter (Write Back stage) */
		//		ml_in_accept inAccept;

		/* From the reorder buffer */
		//		ml_in_flush inFlush;
		inport<bool, nConfig > inFlush;
		outport<bool, nConfig > outFlush;

		/** The functional unit constructor
			@param name the module name
		*/
		FunctionalUnit(const char *name, CPUSim *spec_state) : module(name)
		{
		  class_name = " FunctionUnitClass";
		  // Unisim port names ...
		  inInstruction.set_unisim_name(this,"inInstruction");
		  outInstruction.set_unisim_name(this,"outInstruction");

		  inFlush.set_unisim_name(this,"inFlush");
		  outFlush.set_unisim_name(this,"outFlush");

		        sensitive_pos_method(start_of_cycle) << inClock;
			sensitive_neg_method(end_of_cycle) << inClock;

			sensitive_method(onAccept) << outInstruction.accept;
			sensitive_method(onEnable) << outInstruction.accept << inInstruction.data;

			sensitive_method(onFlushData) << inFlush.data;
			sensitive_method(onFlushAccept) << outFlush.accept;
			sensitive_method(onFlushEnable) << inFlush.enable;


			function = FnInvalid;
			
			/* Internal state */
			//			changed = true;
			this->state = spec_state;
			/* Statistics */
			for (int cfg=0; cfg<nConfig; cfg++)
			  {
			    for(int unit=0; unit<nUnits; unit++)
			      cumulative_occupancy[cfg][unit] = 0;
			  }
		}

		/** Returns true if we can accept input instruction. Enable output instruction if it is accepted.
			@return true if we can accept input instruction.
		*/
		bool Accept(int cfg, int unit)
		{
			QueuePointer<FunctionalUnitPipelineEntry<T, nSources, nStages>, nStages> cur;
			int stage;
			//			bool enabled;
			//			outEnable = enabled = inAccept;
			if(inInstruction.data[cfg*nUnits+unit].something())
			{
				/* if queue is empty then the pipeline is ready */
				if(!queue[cfg][unit].Empty())
				{
					/* Walk across the queue from the newest to the oldest entry */
					for(stage = 0, cur = queue[cfg][unit].SeekAtTail(); cur; cur--, stage++)
					{
						/* if there is a bubble into the pipeline then the pipeline is ready */
						if(cur->stage > stage)
						{
							return true;
						}

						/* There is a special case for the last pipe stage */
						if(stage == nStages - 1)
						{
							/* it is ready if the last pipe stage has an instruction to issue and the next module is ready */
							//return cur->delay[stage] == 0 && enabled;
							//return cur->delay[stage] == 0 && outInstruction.enable;
							return cur->delay[stage] == 0 && outInstruction.accept[cfg*nUnits+unit];
						}
						else
						{
							/* If a pipe stage (which is not the last) has not finished its work then functional unit is not ready */
							if(cur->delay[stage] > 0)
							{
								return false;
							}
						}
					}
				}
				/* At this point, we know that the first stage will be empty in the next clock cycle, so the pipeline is ready to get a new data */
				return true;
			}
			return false;
		}

  //		void onFlushData() { outFlush.data = inFlush.data; }
                void onFlushData() 
                {
		  for(int cfg=0; cfg<nConfig; cfg++)
		  {
		    if (inFlush.data[cfg].something())
		      outFlush.data[cfg] = inFlush.data[cfg];
		    else outFlush.data[cfg].nothing(); 
		  }
		  outFlush.data.send();
		}
		void onFlushAccept() 
                {
		  for(int cfg=0; cfg<nConfig; cfg++)
		  {
		    inFlush.accept[cfg] = outFlush.accept[cfg]; 
		  }
		  inFlush.accept.send();
		}

		void onFlushEnable() 
                { 
		  for(int cfg=0; cfg<nConfig; cfg++)
		  {
		    outFlush.enable[cfg] = inFlush.enable[cfg]; 
		  }
		  outFlush.enable.send();
		}
		/*
		void onData()
		{
		  instruction_to_add = true;
		}
		*/
		void onAccept()
		{
		  //		  outInstruction.enable = inInstruction.accept;
		  for (int cfg=0; cfg<nConfig; cfg++)
		    {
		      for(int unit=0; unit<nUnits; unit++)
			{
			  outInstruction.enable[cfg*nUnits+unit] = outInstruction.accept[cfg*nUnits+unit];
			}
		    }
		  outInstruction.enable.send();
		}
	       
		void onEnable()
		{
		  if (inInstruction.data.known() && outInstruction.accept.known())
		    {
		      
		      for (int cfg=0; cfg<nConfig; cfg++)
			{
			  for(int unit=0; unit<nUnits; unit++)
			    {
			      if (inInstruction.data[cfg*nUnits+unit].something())
				{
				  inInstruction.accept[cfg*nUnits+unit] = Accept(cfg,unit);
				}
			      else
				{
				  inInstruction.accept[cfg*nUnits+unit] = false;
				}
			    }
			}
		      inInstruction.accept.send();
		    }
		}

		/** A SystemC process (SC_METHOD) generating the accept signal */
		/*
		void ExternalControl()
		{
			// Accept or reject the incoming instruction
			outAccept = Accept();
		}
		*/
		/** A SystemC process (SC_METHOD) managing the functional unit pipeline */
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

				/* Flush the functional unit pipeline */
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
			FunctionalUnitPipelineEntry<T, nSources, nStages> *entry;


			//if(!inEnable && !inAccept && !inFlush && !changed) return;
			//			changed = false;

			/* Remove accepted instructions */
			if(outInstruction.accept[cfg*nUnits+unit])
			{
				queue[cfg][unit].RemoveHead();
				//changed = true;
			}

			/* Add incoming instructions */
			if(inInstruction.enable[cfg*nUnits+unit])
			{
				/* Get the instruction */
			  //		const InstructionPtr<T, nSources>& instruction = inInstruction;
				InstructionPtr instruction = inInstruction.data[cfg*nUnits+unit];
				
				if(!instruction->must_reschedule)
				{
				  //changed = true;
	
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
	
					/* Compute the results of the instruction, and initialize the processing delay of the instruction */
					if (entry->instruction->fn != FnSysCall)
					  Compute(&(entry->instruction), entry->delay);
	
					/* Instruction is in the first stage of the pipeline */
					entry->stage = 0;
				}
				else
				  {
				    cerr << "(" << timestamp() <<")" << "Instruction must rescheduled !!!" << endl;
				    cerr << instruction << endl;
				    abort();
				  }
			}

			QueuePointer<FunctionalUnitPipelineEntry<T, nSources, nStages>, nStages> cur, prev;

			/* Run */
			/* For each entry into the pipeline */
			for(cur = queue[cfg][unit].SeekAtHead(); cur; cur++)
			{
				/* if the pipe stage work is not finished */
				if(cur->delay[cur->stage] > 0)
				{
				  //changed = true;
					/* then we decrement the delay (to make it work for one clock cycle) */
					--cur->delay[cur->stage];
				}
			}

			/* For each entry into the pipeline */
			for(cur = queue[cfg][unit].SeekAtHead(); cur; cur++)
			{
				/* if the pipe stage work is finished */
				if(cur->delay[cur->stage] == 0)
				{
					if(prev)
					{
						/* and if this is not the last entry into the pipeline, then make the queue entry become the next pipe stage */
						if(prev->stage > cur->stage + 1)
						{
							cur->stage++;
							//changed = true;
						}
					}
					else
					{
						/* if this is the last entry into the pipeline and this is not the last pipe stage then make the queue entry entry become the next pipe stage */
						if(cur->stage < nStages - 1)
						{
							cur->stage++;
							//changed = true;
						}
					}
				}
				/* Remember the previous entry (which is older than the current) */
				prev = cur;
			}

#ifdef DD_DEBUG_PIPELINE_VERB1
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ==== Pipeline Debug" << endl;
		cerr << this << endl;
#endif
		    }//End of foreach Unit.
		  }//End of foreach Config.
		} // end of end_of_cycle

		void start_of_cycle()
		{
		  for (int cfg=0; cfg<nConfig; cfg++)
		  {
		    for(int unit=0; unit<nUnits; unit++)
		    {
			FunctionalUnitPipelineEntry<T, nSources, nStages> *entry;

			// START_OF_CYCLE ...
			/* Common data bus request */
			entry = queue[cfg][unit].GetHead();
			/* If instruction processing is finished */
			if(entry && entry->stage == nStages - 1 && entry->delay[nStages - 1] == 0)
			{
				/* Do a request to the common data bus */
				//outValid = true;
				outInstruction.data[cfg*nUnits+unit] = entry->instruction;
			}
			else
			{
				outInstruction.data[cfg*nUnits+unit].nothing();
			}

			/* If state changed then make the SystemC scheduler call
			the ExternalControl process handling the valid, accept and enable hand-shaking */
			/*
			if(changed)
				outStateChanged = !inStateChanged;
			*/
		    }//Endof foreach Unit.
		  }//Endof foreach Config.
		  outInstruction.data.send();
		}

		/** A Method doing the computation for an instruction */
		void Compute(InstructionPtr *instruction, int latencies[nStages])
		{
		  //			static state_t state;
			int i;
			//			state->Reset();
			state->SetCR(0);
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
							//state->SetGPR(srcreg) = (*instruction)->sources[i].data;
							state->SetGPR(srcreg, (*instruction)->sources[i].data);
							break;

						case FPR_T:
							/* Copy the floating point register value of the simulator into the state of the emulator */
							//state.fpr[srcreg] = *(double *) &(*instruction)->sources[i].data;
							//state->SetFPR(srcreg) = *(double *) &(*instruction)->sources[i].data;
							//state->SetFPR(srcreg, *(double *) &(*instruction)->sources[i].data);
							state->SetFPR(srcreg, SoftDouble((T)(*instruction)->sources[i].data));
							break;
					case CR_T:
							state->SetCRF(srcreg, (*instruction)->sources[i].data);
							break;
					case FPSCR_T:
					  //state->SetFPSCR(srcreg, (*instruction)->sources[i].data);
							state->SetFPSCR((*instruction)->sources[i].data);
							break;
					case LR_T:
							state->SetLR((*instruction)->sources[i].data);
							break;
					case CTR_T:
							state->SetCTR((*instruction)->sources[i].data);
							break;
					case XER_T:
					  //state->SetXER((*instruction)->sources[i].data);
					  
					  switch(srcreg)
					    {
					    case(0):
					      if ((*instruction)->sources[i].data) state->SetXER_SO();
					      else state->ResetXER_SO();
					      break;
					    case(1):
					      if ((*instruction)->sources[i].data) state->SetXER_OV();
					      else state->ResetXER_OV();
					      break;
					    case(2):
					      if ((*instruction)->sources[i].data) state->SetXER_CA();
					      else state->ResetXER_CA();
					      break;
					    case(3):
					      //if ((*instruction)->sources[i].data) state->SetXER_BYTE_COUNT();
					      //else state->ResetXER_BYTE_COUNT();
					      break;
					    }
					  
					    break;
					  
					default:
					  cerr << "register is neither an integer nor a floating point register" << endl;
					  abort();//exit(-1);
					}
				}
			}
			/* Compute the next instruction address of the instruction */
			//state.nia = (*instruction)->cia + 4;
			state->SetCIA((*instruction)->cia);
			state->SetNIA((*instruction)->cia + 4);
			/* Call the emulator to execute the instruction and compute the results and the next instruction address for branch instructions */
			//	(*instruction)->operation->execute((*instruction)->operation, &state, NULL /* no memory */);
			try{
			  (*instruction)->operation->execute(state);
			}
			catch(Exception &e)
			  {
			    // Here we catch some trap exceptions ???
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
					  (*instruction)->destinations[j].data = state->GetGPR(dstreg);
					  //(*instruction)->destinations[j].data = (0x00000000ffffffff & state->GetGPR(dstreg));
					  break;
					  
					case FPR_T:
					  /* Copy the floating point register value back to the instruction */
					  //(*instruction)->destination.data = *(T *) &state.fpr[dstreg];
					  //(*instruction)->destination.data = *(T *) state->GetFPR(dstreg);
					  //    (*instruction)->destinations[j].data = *(uint64_t *)&state->GetFPR(dstreg);
					  //  (*instruction)->destinations[j].data = (state->GetFp64(dstreg)).queryValue();
					  (*instruction)->destinations[j].data = state->GetFPR(dstreg).queryValue();
					  break;
					case CR_T:
					  //(*instruction)->destinations[j].data = state->GetCRF(dstreg);
							(*instruction)->destinations[j].data = state->GetCRF(dstreg) & 0xf;
							break;
					case FPSCR_T:
							(*instruction)->destinations[j].data = state->GetFPSCR();
							break;
					case LR_T:
							(*instruction)->destinations[j].data = state->GetLR();
							break;
					case CTR_T:
							(*instruction)->destinations[j].data = state->GetCTR();
							break;
					case XER_T:
					  //(*instruction)->destinations[j].data = state->GetXER();
					  switch(dstreg)
					    {
					    case(0):
					      (*instruction)->destinations[j].data = state->GetXER_SO();
					      break;
					    case(1):
					      (*instruction)->destinations[j].data = state->GetXER_OV();
					      break;
					    case(2):
					      (*instruction)->destinations[j].data = state->GetXER_CA();
					      break;
					    case(3):
					      (*instruction)->destinations[j].data = state->GetXER_BYTE_COUNT();
					      break;
					    }
					  break;
					  
					  

						// TODO Case for CR, XER, LR, and others Special purpose registers
						default:
							cerr << "register is neither an integer nor a floating point register" << endl;
							abort();//exit(-1);
					}
			  }
			}
			/* Copy back the effective address */
			//(*instruction)->ea = state.ea;
			//			(*instruction)->ea = state->get_effective_address();
			//			(*instruction)->ea = state->GetEA();
			(*instruction)->ea = (*instruction)->operation->loadstore_effective_address(state);
			
			/* Copy back the next instruction address */
			//(*instruction)->nia = state.nia;
			(*instruction)->nia = state->GetNIA();

			/* If the instruction is a branch */
			//			if((*instruction)->fn & FnBranch)
#ifdef DD_DEBUG_FUNCTIONALUNIT_VERB1
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==COMPUTE==" << endl;
		cerr << "We are computing a unknown instruction: " << *instruction << endl;
#endif
			if( ((*instruction)->fn == FnBranch) || 
			    ((*instruction)->fn == FnConditionalBranch) || 
			    ((*instruction)->fn == FnBranchCountReg) ||
			    ((*instruction)->fn == FnBranchLinkReg)
			    )
			{
#ifdef DD_DEBUG_FUNCTIONALUNIT_VERB1
			  cerr << "["<<this->name()<<"("<<timestamp()<<")] ==COMPUTE==" << endl;
			  cerr << "We are computing a branch instruction: " << *instruction << endl;
#endif
				/* and if branch is conditional */
				//if((*instruction)->fn == FnConditionalBranch)
			  if ( (*instruction)->operation->branch_conditioned() )
				{
				  /* the compute the branch direction from the next instruction address */
				  //(*instruction)->branch_direction = (state.nia == state.target_address) ? Taken : NotTaken;
				  // (*instruction)->branch_direction = (state->GetNIA() == state->get_target_address()) ? Taken : NotTaken;
				  //				  (*instruction)->branch_direction = (state->GetNIA() == (*instruction)->operation->get_target_address(state->GetCIA(),state)) ? Taken : NotTaken;
				  (*instruction)->branch_direction = (state->GetNIA() == (*instruction)->operation->get_target_address((*instruction)->cia,state)) ? Taken : NotTaken;
				}
				else
				{
					/* branch is assumed to be unconditional, and obviously taken */
					(*instruction)->branch_direction = Taken;
				}
#ifdef DD_DEBUG_FUNCTIONALUNIT_VERB1
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==COMPUTE==" << endl;
		cerr << "We are computing a branch instruction:" << *instruction << endl;
#endif

			}

			/* Copy the latencies from the emulator */
			for(i = 0; i < nStages && i < (*instruction)->operation->nstages; i++)
				latencies[i] = (*instruction)->operation->latencies[i];
			for(; i < nStages && i < (*instruction)->operation->nstages; i++)
				latencies[i] = 1;
		}

		/** Returns the function supported by the functional unit
			@return the function supported by the functional unit
		*/
		function_t GetFunction()
		{
			return function;
		}

		/** Set the function supported by the functional unit */
		void SetFunction(function_t function)
		{
			this->function = function;
		}

		/** Prints the functional unit state into an output stream
			@param os an output stream
			@param fu a functional unit module
			@return the output stream
		*/
		friend ostream& operator << (ostream& os, const FunctionalUnit& fu)
		{
		  for (int cfg=0; cfg<nConfig; cfg++)
		  {
		    os << "========= [Config::"<<cfg<<"] " << fu.name() << "==========" << endl;
		    for (int unit=0; unit<nUnits; unit++)
		    {
		      os << "pipeline["<<unit<<"]:" << endl;
		      os << fu.queue[cfg][unit] << endl;
		    }
			os << "===================================" << endl;
		  }
			return os;
		}

		/** Performs a sanity check on the functional unit
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
			cumulative_occupancy = 0;
		    }
		}

	private:
		function_t function;		/*< the function supported by the functional unit */

		/* Some ports to make the SystemC scheduler call the ExternalControl process on state changes */
		/*
		ml_out_data<bool> outStateChanged;
		ml_signal_data<bool> stateChanged;
		ml_in_data<bool> inStateChanged;
		*/
		/* A queue modeling the functional unit pipeline */
		Queue<FunctionalUnitPipelineEntry<T, nSources, nStages>, nStages> queue[nConfig][nUnits];

		//		bool changed;
		uint64_t cumulative_occupancy[nConfig][nUnits];

                CPUSim *state;
};

} // end of namespace execute
} // end of namespace pipeline
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
