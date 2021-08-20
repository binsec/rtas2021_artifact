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
                          ReorderBuffer.hh  -  description
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_COMMIT_REORDER_BUFFER_MC_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_COMMIT_REORDER_BUFFER_MC_HH__

#include <unisim/component/clm/processor/ooosim/cpu_emulator.hh>
#include <unisim/component/clm/processor/ooosim/iss_interface.hh>

#include <unisim/component/clm/interfaces/instruction_interface.hh>

#include <unisim/component/clm/utility/common.hh>


namespace unisim {
namespace component {
namespace clm {
namespace pipeline {
namespace commit {

  using unisim::component::clm::memory::address_t;
using unisim::component::clm::interfaces::InstructionPtr;

using unisim::component::clm::processor::ooosim_mc::CPUEmu;

using unisim::component::clm::utility::hexa;

  using unisim::component::clm::utility::exception_type_t;
  using unisim::component::clm::utility::NONE_EXCEPTION;
  using unisim::component::clm::utility::MISALIGNMENT_EXCEPTION;
  using unisim::component::clm::utility::INVALID_OPCODE_EXCEPTION;


/** State of a reorder buffer entry */
enum ReorderBufferEntryState { unallocated_instruction, allocated_instruction, finished_instruction, written_back_instruction, committed_instruction };

/** A Reorder buffer entry */
template <class T, int nSources>
class ReorderBufferEntry
{
public:
	InstructionPtr instruction;	/*< An instruction */
	enum ReorderBufferEntryState state;			/*< The state */

	/** Prints a reorder buffer entry into an output stream
		@param os an output stream
		@param rbe a reorder buffer entry
		@return the output stream
	*/
	friend ostream& operator << (ostream& os, const ReorderBufferEntry<T, nSources>& rbe)
	{
		os << "instruction=" << rbe.instruction << ",state=";
		switch(rbe.state)
		{
			case unallocated_instruction:
				os << "unallocated_instruction";
				break;

			case allocated_instruction:
				os << "allocated_instruction";
				break;

			case finished_instruction:
				os << "finished_instruction";
				break;

			case written_back_instruction:
				os << "written_back_instruction";
				break;
			
			case committed_instruction:
				os << "committed_instruction";
				break;
		}
		return os;
	}

	/** Compares two reorder buffer entries
		@param entry the rhs
		@return non-zero if different
	*/
	int operator == (const ReorderBufferEntry<T, nSources>& entry) const
	{
		return entry.instruction == instruction;
	}
};

/*
template <class T, int nSources>
void sc_trace(sc_trace_file *tf, const ReorderBufferEntry<T, nSources>& e, const sc_string& NAME)
{
//	sc_trace(tf, da.buffer[i], NAME + ".buffer");
}
*/

/** A SystemC module implementing a reorder buffer */
template <class T, int nSources, int ReorderBufferSize, int AllocateWidth, int WriteBackWidth, int RetireWidth, uint32_t nConfig=2 >
class ReorderBuffer : public module//, public StatisticService

{
	public:
  /**************************************
   * Service ports 
   **************************************/
  // Nothing...

  /**************************************
   * Module Statistics
   **************************************/
  uint64_t retired_instructions[nConfig];
  uint64_t flushed_instructions[nConfig];
  
  /**************************************
   * Module ports 
   **************************************/
		inclock inClock;

		/* From/to the allocator/renamer */
		inport<InstructionPtr, nConfig*AllocateWidth > inAllocateInstruction;

		/* From the register file */
		inport<InstructionPtr, nConfig*WriteBackWidth > inWriteBackInstruction;

		/* From the common data bus arbiter */
		inport<InstructionPtr, nConfig*WriteBackWidth > inFinishInstruction;
		
		/* To the fetch (for branch prediction), To the allocator/renamer (to release registers) */
		outport<InstructionPtr, nConfig*RetireWidth > outRetireInstruction;

		/* Flush signal */
                outport<bool, nConfig > outFlush;
		/* From the load/store queue */
                //		ml_in_data<bool> inLoadStoreQueueBusy;
  //		inport<bool> inLoadStoreQueueBusy;

		/* From the L1 Instruction Cache */
  //		inport<bool> inIL1Busy;

		/* From the L1 Data Cache */
  //		inport<bool> inDL1Busy;

		/* From the L2 Data Cache */
  //		inport<bool> inL2Busy;

		/* From the memory */
  //		inport<bool> inMemBusy;

		/** Constructor
			@param name the name of the module
			@param emulator a pointer to the emulator wrapper
		*/
  //		ReorderBuffer(const char *name, Emulator *emulator) :
		ReorderBuffer(const char *name, CPUEmu *emulator[nConfig]) :
		  module(name)
		  //		  ,Object(name)
		  //		  ,StatisticService(name, this)
		{
			int i;

			class_name = " ReorderBufferClass";
			// Unisim port names ...
			//for (i=0; i<AllocateWidth; i++)
			//  {
			inAllocateInstruction.set_unisim_name(this,"inAllocateInstruction");
			//  }		
			//for (i=0; i<WriteBackWidth; i++)
			//  {
			inWriteBackInstruction.set_unisim_name(this,"inWriteBackInstruction");
			//  }		
			//for (i=0; i<WriteBackWidth; i++)
			//  {
			inFinishInstruction.set_unisim_name(this,"inFinishInstruction");
			//  }		
			//for (i=0; i<RetireWidth; i++)
			//  {
			outRetireInstruction.set_unisim_name(this,"outRetireInstruction");
			//  }		
			outFlush.set_unisim_name(this,"outFlush");

			// SENSITIVITY
			//for (i=0;i<RetireWidth;i++)
			//  {
			sensitive_method(onRetireAcceptandonAllocData) << outRetireInstruction.accept;
			//  }
			//for (i=0;i<AllocateWidth;i++)
			//  {
			sensitive_method(onRetireAcceptandonAllocData) << inAllocateInstruction.data;
			//  }
			//for (i=0;i<WriteBackWidth;i++)
			//  {
			sensitive_method(onWriteBackData) << inWriteBackInstruction.data;
			sensitive_method(onFinishData) << inFinishInstruction.data;
			//  }

			sensitive_pos_method(start_of_cycle) << inClock;
			sensitive_neg_method(end_of_cycle) << inClock;
			
			sensitive_method(onFlushAccept) << outFlush.accept;
			

			/* internal state */
			for(int cfg=0; cfg<nConfig; cfg++)
			{
			  this->emulator[cfg] = emulator[cfg];
			  head[cfg] = -1;
			  tail[cfg] = -1;
			  size[cfg] = 0;
			  mispredicted_branch[cfg] = false;
			  time_stamp[cfg] = 0.0;
			  //			changed = true;
			  replay_trap[cfg] = 0;
			  flush_previous_cycle[cfg] = false;
			  
			  for(i = 0; i < ReorderBufferSize; i++)
			    {
			      entries[cfg][i].state = unallocated_instruction;
			    }
			  
			  /* statistics */
			  sim_num_insn[cfg] = 0;
			  sim_num_loads[cfg] = 0;
			  sim_num_refs[cfg] = 0;
			  sim_num_replay_traps[cfg] = 0;
			  rob_cumulative_occupancy[cfg] = 0;
			  sim_num_spec_loads[cfg] = 0;
			  
			  skip_reg_checking[cfg] = false;
			  syscall_retired[cfg] = false;

			  retired_instructions[cfg] = flushed_instructions[cfg] = 0;
			    
			}
			// ---  Registring statistics ----------------
			/*
			statistics.add("retired_instructions",retired_instructions);
			statistics.add("flushed_instructions",flushed_instructions);
			*/
		}

   /******************************************************************************
   * How ROB work 
   ******************************************************************************
   *                                   
   *                         +---------------------+
   *                         |                     |
   *          inAllocate --->|                     |---> outRetire
   *                         |                     |
   *                         |                     |
   *                         |                     |---> ouFlush
   *                         |                     |
   *                         |                     |
   *         inWriteBack --->|                     |
   *                         |                     |
   *                         |                     |
   *            inFinish --->|                     |<--- inxBusy...
   *                         |                     |
   *                         +---------------------+
   *  
   ************************************************************************************************************
   * 0 -- at start_of_cycle:
   *         0.1 -- check for commited instruction set mispredicted or trap booleen.
   *         0.2 -- send eventualy a flush signal.
   *         0.3 -- send retired instructions.
   *
   * 1 -- on inFinish Data: always accept finished instructions.
   * 
   * 2 -- on inWriteBack Data: always accept written back instructions.
   * 
   * 3 -- on outRetire Accept and on inAllocate Data: 
   *         3.1 -- enable out Retire instruction.
   *         3.2 -- compute futur free space in ROB and eventualy accept inAllocate requests.
   *
   * 4 -- at end_of_cycle:
   *         4.1 -- in case of flush: flush the ROB !!!
   *      else
   *         4.2 -- Remove commited instructions from the ROB.
   *         4.3 -- move written back instruction from finished state to written back state.
   *         4.4 -- move finished instruction from allocated state to finished state.
   *         4.5 -- allocate incoming instruction and move them to allocated state.
   *
   ***********************************************************************************************************/
  /***********************************************************************************************************
   * 0 -- start_of_cycle...
   ***********************************************************************************************************/
  void start_of_cycle()
  {
    for(int cfg=0; cfg<nConfig; cfg++)
    {
      /////////////////////////////////////////////
      // SOC
      /////////////////////////////////////////////
      int i=0;
      int tag;
      /////////////////////
      //   flush_previous_cycle = mispredicted_branch || replay_trap;
      //    mispredicted_branch = false;
      //    replay_trap = false;
      /* For each written back instruction in the reorder buffer beginning by the oldest */
      for(tag = head[cfg], i = 0; i < RetireWidth && i < size[cfg]; tag = tag < ReorderBufferSize - 1 ? tag + 1 : 0, i++)
	{
	  ReorderBufferEntry<T, nSources>& entry = entries[cfg][tag];
	  if(entry.state != committed_instruction) break;	
	  /* Make instruction as committed */
	  //entry.state = committed_instruction;
	  /* Tell to other module that the instruction is retired (for updating branch prediction, unblocking stores, releasing registers) */
	  outRetireInstruction.data[cfg*RetireWidth+i] = entry.instruction;
	}
      /* Send a flush signal if a branch misprediction occurs */
      //    outFlush.data = mispredicted_branch || replay_trap;
      if (mispredicted_branch[cfg] || replay_trap[cfg])
	//    if (flush_previous_cycle)
	{
	  outFlush.data[cfg] = true;
	}
      else
	{
	  outFlush.data[cfg].nothing();
	}
      /* No more instructions were retired at this clock cycle */
      for(; i < RetireWidth; i++)
	{
	  //      outRetireEnable[i] = false;
	  outRetireInstruction.data[cfg*RetireWidth+i].nothing();
	}

    }//Endof foreach Config.
    outRetireInstruction.data.send();
    outFlush.data.send();
  }//Endof start_of_cycle

  /***********************************************************************************************************
   * 1 -- onFinishData...
   ***********************************************************************************************************/
  void onFinishData()
  {
    /*
    bool areallknown(true);
    int i;
    for (i=0;i<WriteBackWidth;i++)
      {
        areallknown &= inFinishInstruction[i].data.known();
      }
    if (areallknown)
    */
    if( inFinishInstruction.data.known() )
      {
      for(int cfg=0; cfg<nConfig; cfg++)
	{
	  // Always accept finished instructions...
	  for (int i=0;i<WriteBackWidth;i++)
	  {
	     inFinishInstruction.accept[cfg*WriteBackWidth+i] = inFinishInstruction.data[cfg*WriteBackWidth+i].something();
	  }
	}
      inFinishInstruction.accept.send();
      }
  }
  
  /***********************************************************************************************************
   * 2 -- onWriteBackData...
   ***********************************************************************************************************/
  void onWriteBackData()
  {
    /*
    bool areallknown(true);
    int i;
    for (i=0;i<WriteBackWidth;i++)
      {
        areallknown &= inWriteBackInstruction[i].data.known();
      }
    if (areallknown)
    */
    if ( inWriteBackInstruction.data.known() )
      {
	for(int cfg=0; cfg<nConfig; cfg++)
	{
	  // Always accept written back instructions...
	  for (int i=0;i<WriteBackWidth;i++)
	  {
	    if (inWriteBackInstruction.data[cfg*WriteBackWidth+i].something())
	      //	     inWriteBackInstruction[i].accept = inWriteBackInstruction[i].data.something();
	      inWriteBackInstruction.accept[cfg*WriteBackWidth+i] = true;
	    else
	      inWriteBackInstruction.accept[cfg*WriteBackWidth+i] = false;
	  }
	}
	inWriteBackInstruction.accept.send();
      }
  }
 
  /***********************************************************************************************************
   * 3 -- onRetireAccept...and...onAllocData
   ***********************************************************************************************************/
  void onRetireAcceptandonAllocData()
  {
    /*
    bool areallknown(true);
    int i;
    for (i=0;i<RetireWidth;i++)
      {
        areallknown &= outRetireInstruction[i].accept.known();
      }
    for (i=0;i<AllocateWidth;i++)
      {
        areallknown &= inAllocateInstruction[i].data.known();
      }

    if (areallknown)
    */
    if( outRetireInstruction.accept.known() && inAllocateInstruction.data.known() ) 
    {
      for(int cfg=0; cfg<nConfig; cfg++)
      {

	int entry_to_remove=0;
	// Con-piouteur # of futur removed ROB entries.
	for (int i=0;i<RetireWidth;i++)
	  {
	    if (outRetireInstruction.accept[cfg*RetireWidth+i])
	      {
#ifdef DD_DEBUG_SIGNALS
		cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: OnRetireAccept... >>> " << endl;
		cerr << "Send outRetire.enable = True"<< endl; 
#endif	
		outRetireInstruction.enable[cfg*RetireWidth+i] = true;
		entry_to_remove++;
	      }
	    else
	      {
#ifdef DD_DEBUG_SIGNALS
		cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: OnRetireAccept... >>> " << endl;
		cerr << "Send outRetire.enable = False"<< endl; 
#endif	
		outRetireInstruction.enable[cfg*RetireWidth+i] = false;
	      }
	  }

	int available_entries = ReorderBufferSize-size[cfg]+entry_to_remove;
	int i;
	for (i=0; i<AllocateWidth && available_entries>0; i++)
	  {
	    if (inAllocateInstruction.data[cfg*AllocateWidth+i].something() && !flush_previous_cycle[cfg])
	      {
		inAllocateInstruction.accept[cfg*AllocateWidth+i] = true;
		available_entries--;
	      }
	    else
	      {
		inAllocateInstruction.accept[cfg*AllocateWidth+i] = false;
	      }
	  }
	// if no more entry is available then DON'T forget to not accept !!!
	for (; i < AllocateWidth; i++)
	  inAllocateInstruction.accept[cfg*AllocateWidth+i] = false;

      }//Endof foreach Config.
      outRetireInstruction.enable.send();
      inAllocateInstruction.accept.send();
    }//End of if areallknown
  }// 

  /***********************************************************************************************************
   * 3 -- end_of_cycle...
   ***********************************************************************************************************/
  void end_of_cycle()
  {

    for(int cfg=0; cfg<nConfig; cfg++)
    {

    int i, j, tag, tag2;
    
    rob_cumulative_occupancy[cfg] += size[cfg];
    

    /**********************************************
     * EOC-1 : Removing Committed instructions
     **********************************************/
    /* For each commited instruction in the reorder buffer beginning by the oldest */
    //    for(tag = head, i = 0; !mispredicted_branch && !replay_trap && !flush_previous_cycle && i < RetireWidth && i < size; tag = tag < ReorderBufferSize - 1 ? tag + 1 : 0, i++)
    for(i = 0; /* running && */ size[cfg] > 0; i++, size[cfg]--, head[cfg] = head[cfg] < ReorderBufferSize - 1 ? head[cfg] + 1 : 0)
      {
	ReorderBufferEntry<T, nSources>& entry = entries[cfg][head[cfg]];
	if(entry.state != committed_instruction) break;


	// INSTRUCTION CHECK
	/* Execute the instruction in the emulator */
	//	emulator->Execute(entry.instruction->operation);
	//	entry.instruction.operation->execute(emulator);
	//	changed = true;

	/* If the instruction is not on the good path */
	//	if(entry.instruction->cia != emulator->ReadCIA())

	//	if (entry.instruction->mustbechecked)
	//	  {
	    if(entry.instruction->cia != emulator[cfg]->GetCIA())
	      {
		/* throw an error */
		cerr << "WARNING:" << endl;
		cerr << "time stamp = " << timestamp() << endl;
		cerr << entry.instruction << endl;
		cerr << "Not on the right path (cia must be " << hexa(emulator[cfg]->GetCIA()) << ")" << endl;
		//	    sc_stop();
		exit(-1);
		//return;
	      }

	    /* If the instruction got an exception */
	    if(entry.instruction->exception != NONE_EXCEPTION)
	      {
		/* throw an error */
		cerr << "instruction (" << entry.instruction << ") got an exception (" 
		     << get_exception_name(entry.instruction->exception) << ")" << endl;
		exit(-1);
	      }
	    
	    retired_instructions[cfg]++;

	    syscall_retired[cfg] = false;
	    
	    //	if (entry.instruction->mustbechecked)
	    if( !( (entry.instruction->fn & FnLoad) && (entry.instruction->replay_trap) )
		&& (entry.instruction->mustbechecked) 
		)
	      {
		skip_reg_checking[cfg] = false;
#ifdef DD_DEBUG_EMULATOR
		if (DD_DEBUG_TIMESTAMP<timestamp())
		  {
		    cerr << "DD_DEBUG_EMULATOR : Before Step... CIA: " << hexa(emulator[cfg]->GetCIA()) << endl;
		  }
#endif	 
		//	    cerr << "Emulator CIA: " << hexa(emulator->GetNIA()) << endl;
		emulator[cfg]->StepOneInstruction();
#ifdef DD_DEBUG_EMULATOR
		if (DD_DEBUG_TIMESTAMP<timestamp())
		  {
		    cerr << "DD_DEBUG_EMULATOR : After Step...  CIA: " << hexa(emulator[cfg]->GetCIA()) << endl;
		  }
#endif	 
		// 				/* Stop if we got an exit system call */
		// 				if(exiting)
		// 				{
		// 					sc_stop();
		// 				}
	    
		/* If branch has not been correctly resolved */
		if(entry.instruction->nia != emulator[cfg]->GetNIA())
		  {
		    /* Throw an error */
		    cerr << "WARNING:" << endl;
		    cerr << "time stamp = " << timestamp() << endl;
		    cerr << entry.instruction << endl;
		    cerr << "Not on the right path (nia must be " << hexa(emulator[cfg]->GetNIA()) << ")" << endl;
		    //	    sc_stop();
		    exit(-1);
		    //return;
		  }
		
		/* If load/store instruction has an incorrect effective address */
		
		if(( (entry.instruction->fn & FnLoad) || (entry.instruction->fn & FnStore) )
		   && entry.instruction->ea != emulator[cfg]->GetEA())
		  {
		    // Throw an error
		    cerr << "[Config::"<<cfg<<"]WARNING:" << endl;
		    cerr << "time stamp = " << timestamp() << endl;
		    cerr << entry.instruction << endl;
		    cerr << "has an incorrect effective address (ea must be " << hexa(emulator[cfg]->GetEA()) << ")" << endl;
		    //	    sc_stop();
		    exit(-1);
		    //return;
		  }
		
		/* Update the (committed) current instruction address */
		cia[cfg] = entry.instruction->nia;
		/* Count the instruction as retired */
		sim_num_insn[cfg]++;
		/*
		  #ifdef FASTSYSC
		  time_stamp = timestamp();
		  #else
		  time_stamp = timestamp().to_double();
		  #endif
		*/
		time_stamp[cfg] = timestamp();
		
		/* If the instruction is a load/store */
		if((entry.instruction->fn & FnLoad) || (entry.instruction->fn & FnStore))
		  {
		    /* Count the load/store as committed */
		    sim_num_refs[cfg]++;
		    if(entry.instruction->fn & FnLoad)
		      sim_num_loads[cfg]++;
		  }
		
		// If instruction is a Syscall we need to copy emulator registers into simulator...
		// So we set a boolean to inform CpuPPC.sim
		if (entry.instruction->fn == FnSysCall)
		  { syscall_retired[cfg] = true; }
		
		//	    else
		//	      { syscall_retired = false; }
		
	      } // if(...mustbechecked)
	else
	  {
	    skip_reg_checking[cfg] = true;
	  }
	//      }
	
	if ( ! outRetireInstruction.accept[cfg*RetireWidth+i] )
	  {
	    cerr << "time stamp = " << timestamp() << endl;
	    cerr << "A module didn't accept a committed instruction !!!" << endl;
	    abort();
	  }
	else
	  {
	    entry.state = unallocated_instruction;
	  }
      }

    if (flush_previous_cycle[cfg])
      {
	for(; size[cfg] > 0; size[cfg]--, head[cfg] = head[cfg] < ReorderBufferSize - 1 ? head[cfg] + 1 : 0)
	  {
	    entries[cfg][head[cfg]].state = unallocated_instruction;
	    flushed_instructions[cfg]++;
	  }
	size[cfg] = 0; head[cfg] = -1; tail[cfg] = -1; //flush = true;
      }

    /**********************************************************
     * EOC-2 : Moving WrittenBack to Committed instructions
     *         + instruction check...
     **********************************************************/
    //     bool flush = false;
    //    flush_previous_cycle = false;
    mispredicted_branch[cfg] = false;
    replay_trap[cfg] = false;
    /* Search for committed instructions beginning by the oldest */
    //    for(i = 0; /* running && */ size > 0; i++, size--, head = head < ReorderBufferSize - 1 ? head + 1 : 0)
    for(tag = head[cfg], i = 0; !mispredicted_branch[cfg] && !replay_trap[cfg] && i < RetireWidth && i < size[cfg]; tag = tag < ReorderBufferSize - 1 ? tag + 1 : 0, i++)
      {
	ReorderBufferEntry<T, nSources>& entry = entries[cfg][tag];
	
	if(entry.state != written_back_instruction) break;
		
	entry.state = committed_instruction;
	/* If the instruction is a load */
	if(entry.instruction->fn & FnLoad)
	  {
	    /* If load must replay, flush the reorder buffer */
	    if(entry.instruction->replay_trap)
	      {
		sim_num_replay_traps[cfg]++;
		/*
		for(; size > 0; size--, head = head < ReorderBufferSize - 1 ? head + 1 : 0)
		  entries[head].state = unallocated_instruction;
		size = 0; head = -1; tail = -1; flush = true;
		*/
		replay_trap[cfg] = true;
		break;
	      }
	  }
	
	/* If the instruction is a branch */
	if( ( (entry.instruction->fn == FnBranch) || 
	      (entry.instruction->fn == FnConditionalBranch) ||
	      (entry.instruction->fn == FnBranchLinkReg) ||
	      (entry.instruction->fn == FnBranchCountReg)
	      ) //&&
	    //(entry.instruction->operation->branch_conditioned())
	    )
	  {
	    /* If branch prediction was bad, flush the reorder buffer */
	    if(entry.instruction->predicted_nia != entry.instruction->nia)
	      {
		/*
		  for(; size > 0; size--, head = head < ReorderBufferSize - 1 ? head + 1 : 0)
		  entries[head].state = unallocated_instruction;
		  size = 0; head = -1; tail = -1; flush = true;
		*/
		mispredicted_branch[cfg] = true;
		break;
	      }
	  }
	    
	/* If the instruction is a store */
	if(entry.instruction->fn & FnStore)
	  {
#ifdef DD_DEBUG_REORDERBUFFER_REPLAY1
	    if (DD_DEBUG_TIMESTAMP < timestamp())
	      {
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== REPLAY1 ==== a Store !" << endl;
		cerr << entry.instruction << endl;
	      }
#endif	
	    /* See whether a load after that store is already finished => load replay trap */
	    for(tag2 = entry.instruction->tag < ReorderBufferSize - 1 ?  entry.instruction->tag + 1 : 0, j = i + 1;
		j < size[cfg]; tag2 = tag2 < ReorderBufferSize - 1 ? tag2 + 1 : 0, j++)
	      {
		ReorderBufferEntry<T, nSources>& entry2 = entries[cfg][tag2];
						
		if(entry2.instruction->fn & FnLoad && entry2.instruction->may_need_replay)
		  {
		    if(entry2.state == finished_instruction ||
		       entry2.state == written_back_instruction ||
		       entry2.state == committed_instruction)
		      {
#ifdef DD_DEBUG_REORDERBUFFER_REPLAY1
			    if (DD_DEBUG_TIMESTAMP < timestamp())
			      {
				cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== REPLAY1 ==== a Load !" << endl;
				cerr << entry2.instruction << endl;
			      }
			    if ( entry2.instruction > entry.instruction )
			      {
				cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== REPLAY1 ==== LD > ST !!!" << endl;
			      }
#endif	

			if(LoadStoreConflict(entry2.instruction, entry.instruction))
			  {
			    entry2.instruction->replay_trap = true; 
#ifdef DD_DEBUG_REORDERBUFFER_REPLAY1
			    if (DD_DEBUG_TIMESTAMP < timestamp())
			      {
				cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== REPLAY1 ==== YES !!!" << endl;
				cerr << entry2.instruction << endl;
			      }
#endif	
			  }
		      }
		  }
	      }
	    
	  }
      }

    //    if(!inAllocateEnable[0] && !inWriteBackEnable[0] && !inFinishEnable[0] && !changed) return;
    //    changed = false;
    if ( (!mispredicted_branch[cfg] && !replay_trap[cfg]) && !flush_previous_cycle[cfg] )
    {
    /**********************************************************
     * EOC-3 : Moving Finished to WrittenBack instructions
     **********************************************************/
    /* See for written back instructions on each write back ports */
    /* For each write back port */
    for(i = 0; i < WriteBackWidth; i++)
      {
#ifdef DD_DEBUG_REORDER_VERB100
	if (DD_DEBUG_TIMESTAMP <= timestamp()) 
	  {
	    cerr << "[Cnonfig::"<<cfg<<"][REORDER(loop:"<<i<<")] ==== WriteBack loop ======= ["<< timestamp()<<"]"<< endl;
	  }
#endif
	/* Is there a finished instructions on the write back port */
	if(inWriteBackInstruction.enable[cfg*WriteBackWidth+i])
	  {
	    /* Get the instruction */
	    InstructionPtr instruction=inWriteBackInstruction.data[cfg*WriteBackWidth+i];
	    //	    const InstructionPtr<T, nSources>& instruction = inWriteBackInstruction[i];
	    //	    const Instruction *instruction = &inst;
	    
	    /* Get the reorder buffer entry */
	    ReorderBufferEntry<T, nSources>& entry = entries[cfg][instruction->tag];
	    
#ifdef DD_DEBUG_REORDER_VERB100
		if (DD_DEBUG_TIMESTAMP <= timestamp()) 
		  {
		    cerr << "[Cnonfig::"<<cfg<<"][REORDER(loop:"<<i<<")] ==== WriteBack inst ======= ["<< timestamp()<<"]"<< endl;
		    cerr << "[Cnonfig::"<<cfg<<"]["<<this->name()<<"("<<timestamp()<<")] "<< entry << endl;
		  }
#endif
	    /* If instruction is in reorder buffer and not finished */
	    if(entry.state == finished_instruction)
	      {
		/* then check if instruction number is correct */
		if(entry.instruction->inum != instruction->inum)
		  {
		    /* Throw an error */
		    cerr << "time stamp = " << timestamp() << endl;
		    cerr << "Attempting to write back an instruction (" << *instruction <<") with a bad inum" << endl;
		    exit(-1);
		  }
		/* Update the reorder buffer */
		bool replay_trap_tmp = entry.instruction->replay_trap;
		entry.instruction = instruction;
		if(replay_trap_tmp) entry.instruction->replay_trap = true;
		
		/* instruction is finished */
		entry.state = written_back_instruction;
		
	      }
	    else
	      {
		if (!instruction->replay_trap)
		  {
		/* else check if instruction is already finished */
		if(entry.state == written_back_instruction)
		  {
		    /* Throw an error */
		    cerr << "time stamp = " << timestamp() << endl;
		    cerr << "Attempting to write back an instruction (" << *instruction 
			 <<") which is already written back" << endl;
		    exit(-1);
		  }
		else
		  {
		    /* Throw an error */
		    cerr << "time stamp = " << timestamp() << endl;
		    cerr << "Attempting to write back an instruction (" << *instruction 
			 <<") which is not into the reorder buffer" << endl;
		    exit(-1);
		  }
		  }
	      }
	  }
      }
    /**********************************************************
     * EOC-4 : Moving Allocated to Finished instructions
     **********************************************************/
    /* See for finished instructions on each write back ports */
    /* For each write back port */
    for(i = 0; i < WriteBackWidth; i++)
      {
#ifdef DD_DEBUG_REORDER_VERB100
	if (DD_DEBUG_TIMESTAMP <= timestamp()) 
	  {
	    cerr << "[Cnonfig::"<<cfg<<"][REORDER(loop:"<<i<<")] ==== Finish loop ======= ["<< timestamp()<<"]"<< endl;
	  }
#endif
	/* Is there a finished instructions on the write back port */
	if(inFinishInstruction.enable[cfg*WriteBackWidth+i])
	  {
	    /* Get the instruction */
	    InstructionPtr instruction = inFinishInstruction.data[cfg*WriteBackWidth+i];
	    //	    const Instruction *instruction = &inst;
	    
	    /* Get the reorder buffer entry */
	    ReorderBufferEntry<T, nSources>& entry = entries[cfg][instruction->tag];

	    /* If instruction is in reorder buffer and not finished */
	    if(entry.state == allocated_instruction)
	      {
		/* then check if instruction number is correct */
		if(entry.instruction->inum != instruction->inum)
		  {
		    /* Throw an error */
		    cerr << "time stamp = " << timestamp() << endl;
		    cerr << "Attempting to finish an instruction (" << instruction 
			 <<") with a bad inum" << endl;
		    exit(-1);
		  }
		/* Update the reorder buffer */
		entry.instruction = instruction;
		
		/* instruction is finished */
		entry.state = finished_instruction;
#ifdef DD_DEBUG_REORDER_VERB100
		if (DD_DEBUG_TIMESTAMP <= timestamp()) 
		  {
		    cerr << "[Cnonfig::"<<cfg<<"][REORDER(loop:"<<i<<")] ==== Finishing inst ======= ["<< timestamp()<<"]"<< endl;
		    cerr << "[Cnonfig::"<<cfg<<"]["<<this->name()<<"("<<timestamp()<<")] "<< entry << endl;
		  }
#endif
	      }
	    else
	      {
		if (instruction->replay_trap)
		  {
		    entry.instruction = instruction;
		  }
		else
		  {
		    /* else check if instruction is already finished */
		    if(entry.state == finished_instruction)
		      {
			/* Throw an error */
			cerr << "time stamp = " << timestamp() << endl;
			cerr << "Attempting to finish an instruction (" << instruction 
			     <<") which is already finished" << endl;
			cerr << "Inst: " << instruction << endl;
			exit(-1);
		      }
		    else
		      {
			/* Throw an error */
			cerr << "time stamp = " << timestamp() << endl;
			cerr << "Attempting to finish an instruction (" << instruction 
			     <<") which is not into the reorder buffer" << endl;
			cerr << "Inst: " << instruction << endl;
			cerr << "Entry: " << entry << endl;
			exit(-1);
		      }
		  }
	      }
	  }
      }
    
    /**********************************************************
     * EOC-5 : Allocat incoming instructions
     **********************************************************/
    /* If no branch misprediction, replay trap and flush occur */
    /* For each allocate ports */
    for(i = 0; i < AllocateWidth; i++)
      {
	/* Is there an instruction on this allocate port ? */
	if(!inAllocateInstruction.enable[cfg*AllocateWidth+i]) break;
	/* Get the instruction */
	InstructionPtr instruction = inAllocateInstruction.data[cfg*AllocateWidth+i];
	//	const Instruction *instruction = &inst;
	
	/* Allocate space in the reorder buffer */
	tail[cfg] = (tail[cfg] < ReorderBufferSize - 1) ? tail[cfg] + 1: 0;
	
	/* If tag allocated by the allocator/renamer does not match the tail tag of the reorder buffer */
	if(instruction->tag != tail[cfg])
	  {
	    /* throw an error */
	    cerr << "time stamp = " << timestamp() << endl;
	    cerr << "out-of-order allocation (" << instruction << ") into reorder buffer" << endl;
	    cerr << "reorder buffer tail : " << tail << endl;
	    exit(-1);
	  }
	
	/* If reorder buffer was empty, update the head pointer */
	if(head[cfg] < 0) head[cfg] = tail[cfg];
	
	/* If reorder was already full */
	if(size[cfg] >= ReorderBufferSize)
	  {
	    /* throw an error */
	    cerr << "time stamp = " << timestamp() << endl;
	    cerr << "Reorder Buffer overflow" << endl;
	    exit(-1);
	  }
	/* Increment the number of instruction into the reorder buffer */
	size[cfg]++;
	//	    changed = true;
	/* Get the reorder buffer entry identified by the reorder buffer tag */
	ReorderBufferEntry<T, nSources>& entry = entries[cfg][instruction->tag];
	
	/* Store the instruction into the reorder buffer entry */
	entry.instruction = instruction;
	
	/* If instruction is a NOP or prefetch/load then don't wait it, finish it immediately */
	if(entry.instruction->fn & (FnNop | FnPrefetchLoad))
	  entry.state = written_back_instruction;
	else
	  entry.state = allocated_instruction;
      }
    }
    else
      {
#ifdef DD_DEBUG_FLUSH
    if (DD_DEBUG_TIMESTAMP < timestamp())
      {
	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====  Flush !!!" << endl;
	if (mispredicted_branch)
	  {  cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====  Flush on mispredicted branch !!!" << endl; }
	else if (replay_trap)
	  { cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====  Flush on replay_trap !!!" << endl; }
	else
	  {
	    cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====  Flush on flush_previous_cycle !!!" << endl;
	    //abort();
	  }
      }
#endif
	// a mispredicted_branch or a replay_trap occurs... 
	// We need to flush the ROB from the instruction after the flushed instruction to rob tail.
	/*
	tag = tag < ReorderBufferSize - 1 ? tag + 1 : 0;
	for(; tail != tag && 0 < size; tail = tail > 0 ? tail-1 : ReorderBufferSize - 1, size--)
	  {
	    entries[tail].state = unallocated_instruction;
	  }
	*/
      }
#ifdef DD_DEBUG_PIPELINE_VERB1
    cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ==== Pipeline Debug" << endl;
    cerr << this << endl;
#endif

#ifdef DD_DEBUG_REORDERBUFFER_VERB2
    if (DD_DEBUG_TIMESTAMP < timestamp())
      {
	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC End ==== " << endl;
	cerr << *this << endl;
      }
#endif	
    flush_previous_cycle[cfg] = mispredicted_branch[cfg] || replay_trap[cfg];

    }//Endof foreach Config.

  } // end of end_of_cycle

  void onFlushAccept()
  {
    for(int cfg=0; cfg<nConfig; cfg++)
      {
	outFlush.enable[cfg] = outFlush.accept[cfg];
      }
    outFlush.enable.send();
  }
  
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////


  //		bool LoadStoreConflict(const InstructionPtr<T, nSources>& load, const InstructionPtr<T, nSources>& store)
		bool LoadStoreConflict(const InstructionPtr load, const InstructionPtr store)
		{
			if( *load > *store)
			{
				if((store->ea >= load->ea && store->ea < load->ea + load->operation->memory_access_size()) ||
				   (load->ea >= store->ea && load->ea < store->ea + store->operation->memory_access_size()))
				{
					return true;
				}
			}
			return false;
		}
		

		/** Writes the current instruction address
			@param pc the value to write
		*/
		void WriteCIA(address_t pc, int cfg)
		{
			cia[cfg] = pc;
		}

		/** Reads the current instruction address
			@return the current instruction address */
		address_t ReadCIA(int cfg)
		{
			return cia[cfg];
		}

		/** Prints the reorder buffer contents into an output stream
			@param os an output stream
			@param rob a reorder buffer module
			@return the output stream
		*/
		friend ostream& operator << (ostream& os, const ReorderBuffer<T, nSources, ReorderBufferSize, AllocateWidth, WriteBackWidth, RetireWidth, nConfig>& rob)
		{
		  int i;
		  int tag;
		  for(int cfg=0; cfg<nConfig; cfg++)
		    {
			os << "============= [Config::"<<cfg<<"] REORDERBUFFER =============" << endl;
			os << "(rob)robSize=" << rob.size[cfg] << endl;
			os << "(rob)robHead=" << rob.head[cfg] << endl;
			os << "(rob)robTail=" << rob.tail[cfg] << endl;
			for(tag = rob.tail[cfg], i = 0; i < rob.size[cfg]; tag = tag > 0 ? tag - 1 : ReorderBufferSize - 1, i++)
			{
				const ReorderBufferEntry<T, nSources>& entry = rob.entries[cfg][tag];
				os << entry << endl;
			}
			os << "========================================" << endl;
		    }
		  return os;
		}

		/** Returns the number of retired instructions
			@return the number of retired instructions
		*/
		UInt64 GetRetiredInstructions()
		{
			return sim_num_insn;
		}

		/** Perform a sanity check of the reorder buffer
			@return true if ok
		*/
  /*
		bool Check()
		{
			int n, m;
			int tag, tag1, tag2;
			SInt64 inum;

			for(tag1 = head, n = 0; n < size; tag1 = tag1 < ReorderBufferSize - 1 ? tag1 + 1 : 0, n++)
			{
				const ReorderBufferEntry<T, nSources>& entry1 = entries[tag1];
				for(tag2 = head, m = 0; m < size; tag2 = tag2 < ReorderBufferSize - 1 ? tag2 + 1 : 0, m++)
				{
					const ReorderBufferEntry<T, nSources>& entry2 = entries[tag2];

					if(tag1 != tag2 && entry1 == entry2)
					{
						return false;
					}
				}
			}

			inum = -1;
			for(tag = head, n = 0; n < size; tag = tag < ReorderBufferSize - 1 ? tag + 1 : 0, n++)
			{
				const ReorderBufferEntry<T, nSources>& entry = entries[tag];
				if(entry.instruction->inum <= inum)
				{
					return false;
				}
				inum = entry.instruction->inum;
			}
			return true;
		}
  */
		/** Return a time stamp
			@return a time stamp
		*/
		double GetTimeStamp()
		{
			return time_stamp;
		}

		/** Prints statistics into an output stream
			@param os an output stream
		*/
		void DumpStats(ostream& os, uint64_t sim_total_time, uint64_t sim_cycle)
		{
			os << "sim_num_insn " << sim_num_insn << " # total number of instructions committed" << endl;
			os << "sim_inst_rate " << sim_num_insn / sim_total_time << " # simulation speed (in insts/sec)" << endl;
			os << "sim_ipc " << (double) sim_num_insn / (double) sim_cycle << " # instructions per cycle" << endl;
			os << "sim_num_replay_traps " << sim_num_replay_traps << " # number of committed replay traps" << endl;
			os << "sim_num_refs " << sim_num_refs << " # total number of loads and stores committed" << endl;
			os << "sim_num_loads " << sim_num_loads << " # total number of loads committed" << endl;
			os << "sim_num_stores " << sim_num_refs - sim_num_loads << " # total number of stores committed" << endl;
			os << "rob_occupancy " << ((double) rob_cumulative_occupancy / (double) timestamp()) << " # " << name() << " rob_cumulative_occupancy (instruction/cycles)" << endl;
			os << "sim_num_spec_loads " << sim_num_spec_loads << " # " << name() << " number of committed speculative loads" << endl;
		}

		void WarmRestart()
		{
		  for(int cfg=0; cfg<nConfig; cfg++)
		    {
			head[cfg] = -1;
			tail[cfg] = -1;
			size[cfg] = 0;
			mispredicted_branch[cfg] = false;
			time_stamp[cfg] = 0.0;
			//			changed = true;
			replay_trap[cfg] = false;

			for(int i = 0; i < ReorderBufferSize; i++)
			{
				entries[cfg][i].state = unallocated_instruction;
			}
		    }
		}
		void ResetStats()
		{
		  for(int cfg=0; cfg<nConfig; cfg++)
		    {
			sim_num_insn[cfg] = 0;
			rob_cumulative_occupancy[cfg] = 0;
			sim_num_spec_loads[cfg] = 0;
			sim_num_loads[cfg] = 0;
			sim_num_refs[cfg] = 0;
			sim_num_replay_traps[cfg] = 0;
		    }
		}
public:
		/* Members for managing the reorder buffer tag with a fifo allocation policy */
		int head[nConfig];
		int tail[nConfig];
		int size[nConfig];

		/* Reorder buffer content */
		ReorderBufferEntry<T, nSources> entries[nConfig][ReorderBufferSize];

		/* Pointer to the emulator wrapper */
  //		Emulator *emulator;
		CPUEmu *emulator[nConfig];

		/* committed current instruction address */
		address_t cia[nConfig];

		/* Does a mispredicted branch occur ? */
		bool mispredicted_branch[nConfig];
		
		/* Does a replay trap occur ? */
		bool replay_trap[nConfig];

                /* Does a Flush occur previous cycle */
                bool flush_previous_cycle[nConfig];

		/* current time stamp */
		double time_stamp[nConfig];
		
		/* Number of retired instructions */
		uint64_t sim_num_insn[nConfig];

		/* Number of committed loads */
		uint64_t sim_num_loads[nConfig];

		/* Number of commmitted memory accesses */
		uint64_t sim_num_refs[nConfig];
			
		/* Number of replay traps */
		uint64_t sim_num_replay_traps[nConfig];
		
		uint64_t rob_cumulative_occupancy[nConfig];
		
		uint64_t sim_num_spec_loads[nConfig];

	
  bool skip_reg_checking[nConfig];
  bool syscall_retired[nConfig];
		/* Some ports to make the SystemC scheduler call the ExternalControl process on state changes */
  /*
		ml_out_data<bool> outStateChanged;
		ml_signal_data<bool> stateChanged;
		ml_in_data<bool> inStateChanged;
  */
  //		bool changed;
};

} // end of namespace commit
} // end of namespace pipeline
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
