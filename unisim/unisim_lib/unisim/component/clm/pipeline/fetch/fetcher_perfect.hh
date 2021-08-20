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
                          Fetch.hh  -  description
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_FETCH_FETCHER_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_FETCH_FETCHER_HH__

#include <unisim/component/clm/interfaces/memreq.hh>

//#include <system/memory/cache/CacheCommon.h>
#include <unisim/component/clm/cache/cache_common.hh>
#include <unisim/component/clm/pipeline/fetch/branch_history_table.hh>
#include <unisim/component/clm/pipeline/fetch/branch_target_buffer.hh>
#include <unisim/component/clm/pipeline/fetch/return_address_stack.hh>

#include <unisim/component/clm/processor/ooosim/cpu_emulator.hh>
#include <unisim/component/clm/processor/ooosim/iss_interface.hh>

namespace unisim {
namespace component {
namespace clm {
namespace pipeline {
namespace fetch {


using unisim::component::clm::interfaces::memreq;
using unisim::component::clm::interfaces::memreq_types;
//using unisim::service::interfaces::StatisticReporting;

using unisim::component::clm::memory::ByteArray;
using unisim::component::clm::memory::endianess_t;


using unisim::component::clm::interfaces::Source;
using unisim::component::clm::interfaces::Destination;


  using unisim::kernel::Client;
  //  using unisim::kernel::Service;
  using unisim::kernel::ServiceImport;
  //  using unisim::kernel::ServiceExport;

  //  using unisim::component::clm::processor::ooosim::CPUSim;
  using unisim::component::clm::processor::ooosim::CPUEmu;

  using unisim::component::clm::utility::branch_direction_t;
  using unisim::component::clm::utility::Taken;
  using unisim::component::clm::utility::NotTaken;
  using unisim::component::clm::utility::DontKnow;
 
  using unisim::component::clm::utility::exception_type_t;
  using unisim::component::clm::utility::NONE_EXCEPTION;
  using unisim::component::clm::utility::MISALIGNMENT_EXCEPTION;
  using unisim::component::clm::utility::INVALID_OPCODE_EXCEPTION;


  using unisim::component::cxx::processor::powerpc::INPUT_T;
  using unisim::component::cxx::processor::powerpc::OUTPUT_T;

  using unisim::kernel::Object;
  using unisim::kernel::variable::Statistic;

  using unisim::component::cxx::processor::powerpc::Exception;
  
/* An instruction queue entry */
//template <class T, int nSources>
class InstructionQueueEntry
{
public:
  //DD	InstructionPtr<T, nSources> instruction; /*< An instruction */
  //	Instruction *instruction; /*< An instruction */
  InstructionPtr instruction;
	bool predecoded;	/*< true if instruction was predecoded */
  bool executed_in_fetch;
	InstructionQueueEntry()
	{
		Reset();
	}

	~InstructionQueueEntry()
	{
	  //	  delete instruction;
	}

  
	void Reset()
	{
	  //	        instruction = new Instruction();
	  //	  instruction->Reset();
		predecoded = false;
		executed_in_fetch = false;
	}

	/** Print an instruction queue entry into an output stream
		@param os an output stream
		@param entry an instruction queue entry
		@return the output stream
	*/
  //	friend ostream& operator << (ostream& os, const InstructionQueueEntry<T, nSources>& entry)
	friend ostream& operator << (ostream& os, const InstructionQueueEntry& entry)
	{
		os << (entry.instruction) << ",predecoded=" << entry.predecoded << ", fexecuted=" << entry.executed_in_fetch;
		return os;
	}

	/** Compares two instruction queue entry
		@param entry the rhs
		@return non-zero if different
	*/
  //	int operator == (const InstructionQueueEntry<T, nSources>& entry) const	
	int operator == (const InstructionQueueEntry& entry) const
        {
		return instruction == entry.instruction;
	}
};

/** A SystemC module modeling the fetch stage and the branch prediction */
template <class T, int nSources, int Width, int LineSize, int nCPUDataPathSize, int InstructionQueueSize, int InstructionSize, int BHT_Size, int BHT_nBits, int BHT_nHistorySize, int BTB_Size, int BTB_Associativity, int RAS_Size, int RetireWidth, int WriteBackWidth, int MaxPendingRequests, int MaxBranches>
class Fetcher : public module, public Object
//, public StatisticService
//                , public Client<StatisticReporting>
//              , public Service<StatisticReportingControl>
{
public:
  /**************************************
   * Service ports 
   **************************************/
  // Nothing...
  //  ServiceImport<StatisticReporting> statistic_reporting_import;

  /**************************************
   * Module Statistics
   **************************************/
  uint64_t fetched_instructions;
  uint64_t splitted_instructions;

  uint64_t bht_accesses;
  uint64_t bht_misses;
  uint64_t btb_accesses;
  uint64_t btb_misses;
  uint64_t ras_accesses;
  uint64_t ras_misses;
  uint64_t ras_capacity_misses;
  uint64_t instruction_queue_cumulative_occupancy;
  //  uint64_t flushed_instructions;
  
  Statistic<uint64_t> stat_fetched_instructions;
  /**************************************
   * Module ports 
   **************************************/
  //	sc_in_clk inClock;
  inclock inClock;

	/* From/To L1 Instruction Cache */
  // outport < memreq < Instruction, nIL1CachetoMemDataPathSize > > outIL1Data;   ///< To L1 Instruction Memory
  //DD        outport < memreq < InstructionPtr<T, nSources>, nCPUDataPathSize > > outIL1;   ///< To L1 Instruction Memory
        outport < memreq < InstructionPtr, nCPUDataPathSize > > outIL1;   ///< To L1 Instruction Memory
	
	//	inport  < memreq < InstructionPtr<T, nSources>, nIL1MemtoCacheDataPathSize > > inIL1Data;    ///< From L1 Instruction Cache
	//DD	inport  < memreq < InstructionPtr<T, nSources>, nCPUDataPathSize > > inIL1;    ///< From L1 Instruction Cache
	inport  < memreq < InstructionPtr, nCPUDataPathSize > > inIL1;    ///< From L1 Instruction Cache

	/* To the instruction decoder */
	//DD	outport<InstructionPtr<T, nSources> > outIntruction[Width];
	outport<InstructionPtr> outInstruction[Width];

	/* From the reorder buffer */
	//	ml_in_flush inFlush;
	inport<bool> inFlush;

	//DD	inport<InstructionPtr<T, nSources> > inRetireInstruction[RetireWidth];
  	inport<InstructionPtr> inRetireInstruction[RetireWidth];
  //	inport<InstructionPtr> inRetireInstruction[Width];

	/* From the common data bus */
	//	inport<InstructionPtr<T, nSources> > inWriteBackInstruction[WriteBackWidth];
  //	inport<Instruction *> inWriteBackInstruction[WriteBackWidth];
	inport<InstructionPtr> inWriteBackInstruction[WriteBackWidth];
	/** Constructor of the fetcher module
		@param name the module name
		@param endianess either big_endian or little_endian
		@param emulator a pointer to the emulator wrapper class
	*/
	/*
	Fetcher(const sc_module_name& name, endianess_t endianess, Emulator *emulator) :
	  sc_module(name)
	*/
	//DD	Fetcher(const char *name, endianess_t endianess, Emulator *emulator) :
  //	Fetcher(const char *name, endianess_t endianess, CPUSim *emulator) :
  Fetcher(const char *name, endianess_t endianess, CPUEmu *emulator) ://, CPUEMu *emu2) :
	  module(name)
	  , Object(name)
	  //	       , Client<StatisticReporting>(name, this)
	  //	       , statistic_reporting_import("statistic", this)
	  //	  ,StatisticService(name, this)
	       , fetched_instructions(0)
	       , stat_fetched_instructions("fetched-instructions", this, fetched_instructions)
	  
	{
		int i;
		class_name = " FetcherClass";
		// Unisim port names ...
		outIL1.set_unisim_name(this,"outIL1");
		inIL1.set_unisim_name(this,"inIL1");
		inFlush.set_unisim_name(this,"inFlush");
		for (i=0; i<Width; i++)
		  {
		    outInstruction[i].set_unisim_name(this,"outInstruction",i);
		    //		    inRetireInstruction[i].set_unisim_name(this,"inRetireInstruction",i);
		    //		    inWriteBackInstruction[i].set_unisim_name(this,"inWriteBackInstruction",i);
		  }

		for (i=0; i<WriteBackWidth; i++)
		  {
		    inWriteBackInstruction[i].set_unisim_name(this,"inWriteBackInstruction",i);
		  }

		for (i=0; i<RetireWidth; i++)
		  {
		    inRetireInstruction[i].set_unisim_name(this,"inRetireInstruction",i);
		  }

		sensitive_method(onFlushData) << inFlush.data;

		sensitive_method(onMEMData) << inIL1.data;
		sensitive_method(onMEMAccept) << outIL1.accept;
		
		for(i = 0; i < Width; i++)
		  {
		    sensitive_method(onCPUAccept) << outInstruction[i].accept;
		  }
		for(i = 0; i < WriteBackWidth; i++)
		  {
		    sensitive_method(onCPUDataWriteBack) << inWriteBackInstruction[i].data;
		  }
		for(i = 0; i < RetireWidth; i++)
		  {
		    sensitive_method(onCPUDataRetire)<< inRetireInstruction[i].data;
		  }

		sensitive_pos_method(start_of_cycle) << inClock;
		sensitive_neg_method(end_of_cycle) << inClock;

 
		/* Internal state */
		//		waiting_instr_cache_accept = false;
		cia = 0;

		nia = 0;
		seq_cia = 0;
		previous_cia = 0;

		accessSize = 0;
		this->endianess = endianess;
		inum = 0;
		pending_instr_cache_requests = 0;
		ignore_instr_cache_responses = 0;
		pending_instr_cache_access_size = 0;
		this->emulator = emulator;
		//		this->emulator2 = emulator2;
		btb_miss = false;
		ras_miss = false;
		//		state_init(&transient_emul_state);

		is_terminated = false;

		/* statistics */
		in_flight_branches = 0;
		bht_accesses = 0;
		bht_misses = 0;
		btb_accesses = 0;
		btb_misses = 0;
		ras_accesses = 0;
		ras_misses = 0;
		ras_capacity_misses = 0;
		instruction_queue_cumulative_occupancy = 0;
		
		fetched_instructions = 0;//flushed_instructions = 0;
		splitted_instructions = 0;//flushed_instructions = 0;
		// ---  Registring statistics ----------------
		/*
		statistics.add("fetched_instructions",fetched_instructions);
		statistics.add("splitted_instructions",splitted_instructions);
		statistics.add("bht_accesses",bht_accesses);
		statistics.add("bht_misses",bht_misses);
		statistics.add("btb_accesses",btb_accesses);
		statistics.add("btb_misses",btb_misses);
		statistics.add("ras_accesses",ras_accesses);
		statistics.add("ras_misses",ras_misses);
		statistics.add("ras_capacity_misses",ras_capacity_misses);
		//		statistics.add("flushed_instructions",flushed_instructions);
		*/		
		syscall_in_pipeline = false;
		stall_counter = 0;

		// --- Latex rendering hints -----------------
		latex_left_ports.push_back(&inIL1);
		latex_left_ports.push_back(&outIL1);

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
		//		latex_bottom_ports.push_back(&outFlush);
	}

  void set_terminated()
  {
    is_terminated = true;
  }


  void onFlushData()
  {
    inFlush.accept = inFlush.data.something();
    /*
      #ifdef DD_DEBUG_FETCH_VERB2
      if ( inFlush.data.something() )
      cerr << "[FETCH] ==== We are accepting a Flush (onFlushData) === data: " <<inFlush.data << " ======= ["<< timestamp()<<"]"<< endl;
      else
      cerr << "[FETCH] ==== We are rejecting a Flush (onFlushData) === data: " <<inFlush.data << "======= ["<< timestamp()<<"]"<< endl;
      #endif
    */
  }
  /*
        void onCPUData()
        { 
	  bool areallknown(true);
	  int i;
	  for (i=0;i<WriteBackWidth;i++)
	    {
	      areallknown &= inWriteBackInstruction[i].data.known();
	    }
	  for (i=0;i<RetireWidth;i++)
	    {
	      areallknown &= inRetireInstruction[i].data.known();
	    }
	  if (areallknown)
	    {
	      for (i=0;i<WriteBackWidth;i++)
		{
		  inWriteBackInstruction[i].accept = inWriteBackInstruction[i].data.something();
		}
	      for (i=0;i<RetireWidth;i++)
		{
		  inRetireInstruction[i].accept = inRetireInstruction[i].data.something();
		}
	    }	  
	}
  */
        void onCPUDataWriteBack()
        { 
	  bool areallknown(true);
	  int i;
	  for (i=0;i<WriteBackWidth;i++)
	    {
	      areallknown &= inWriteBackInstruction[i].data.known();
	    }
	  if (areallknown)
	    {
	      for (i=0;i<WriteBackWidth;i++)
		{
		  inWriteBackInstruction[i].accept = inWriteBackInstruction[i].data.something();
		}
	    }	  
	}
        void onCPUDataRetire()
        { 
	  bool areallknown(true);
	  int i;
	  for (i=0;i<RetireWidth;i++)
	    {
	      areallknown &= inRetireInstruction[i].data.known();
	    }
	  if (areallknown)
	    {
	      for (i=0;i<RetireWidth;i++)
		{
		  inRetireInstruction[i].accept = inRetireInstruction[i].data.something();
		}
	    }	  
	}
	/** A SystemC process managing the valid, accept and enable hand-shaking */
	//	void ExternalControl()
	void onMEMAccept()
	{
	  outIL1.enable = outIL1.accept;
	  //	  pending_instr_cache_requests++;
	  //	  pending_instr_cache_access_size += accessSize;
	}
	void onMEMData()
	{
	  inIL1.accept = inIL1.data.something();
#ifdef DD_DEBUG_FETCH_VERB2
	  /// Mourad Modifs
	  /////////////////

	  if (DD_DEBUG_TIMESTAMP < timestamp())
 {	
   if (inIL1.data.something())
     {
       cerr << "[FETCH] ==== We are accepting Data == ("<< inIL1.data <<") ======== ["<< timestamp()<<"]"<< endl;
     }
 }
#endif
	}
	void onCPUAccept()
	{
	  bool areallknown(true);
	  int i;
	  for (i=0;i<Width;i++)
	    {
	      areallknown &= outInstruction[i].accept.known();
		//& inRetireInstruction[i].data.known();
	    }
	  if (areallknown)
	    {
	      /* Enable each accepted instructions */
	      for(i = 0; i < Width; i++)
		{
		  outInstruction[i].enable = outInstruction[i].accept;
		}
	    }
	}

	/** Computes the instruction cache access size
		@param addr the address of the instruction cache access
		@returns the instruction cache access size
	*/
	int GetMaximumAccessSize(address_t addr)
	{
		int accessSize = LineSize - (addr % LineSize);
		if(accessSize > nCPUDataPathSize) accessSize = nCPUDataPathSize;
		// DD : To insure that each fetched instruction must be splitted we only fetch half freespace in fetch queue. 
		//		int instructionQueueFreeSpace = instructionQueue.FreeSpace() * InstructionSize;
		int instructionQueueFreeSpace = ( instructionQueue.FreeSpace()/2 ) * InstructionSize;
		if(accessSize + pending_instr_cache_access_size > instructionQueueFreeSpace) accessSize = instructionQueueFreeSpace - pending_instr_cache_access_size;
		if(accessSize < 0) accessSize = 0;
		
#ifdef DD_DEBUG_FETCH_VERB2
		if (DD_DEBUG_TIMESTAMP < timestamp())
		  {
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== GetMaximumAccessSize ==== " << endl;
		cerr << "      addr               ="<< hexa(addr) << endl;
		cerr << "      LineSize           ="<< LineSize<<"   ("<< hexa(LineSize)<<")"<< endl;
		cerr << "      nCPUDataPathSize   ="<< nCPUDataPathSize<<"   ("<< hexa(nCPUDataPathSize)<<")"<< endl;
		cerr << "      accessSize         ="<< accessSize << endl;
		  }
#endif
		
		// DD : To insure that each fetched instruction must be splitted we only fetch half freespace in fetch queue... 
		//		return (accessSize/2); // It's completly bugged... :-(
		
		return accessSize;
		
	}

	/** Decodes an instruction
		@param instruction an instruction
	*/
	//DD	void Decode(InstructionPtr<T, nSources>& instruction)
	void Decode(InstructionPtr *instruction)
	{
		/* Call the emulator decode function */
	        //	        instruction->operation = emulator->Decode(instruction->cia);
	  //full_system::processors::powerpc::CodeType buffer;
	  //emulator->Fetch(&buffer, instruction->cia, sizeof(buffer));
	  //	  cerr << " INFO [DECODE]: cia = " << hexa(instruction->cia) << " ..." << endl;
	  if (instruction==NULL)
	    { 
	      cerr << "ERROR: No instruction to decode !!! " << endl;
	      exit(-1);
	    }
	  else
	    {
	      /*
	      if (instruction->cia != 0)
		{
		  cerr << "ERROR: Decoding instruction at physical address 0 will core dump !!! " << endl;
		  exit(-1);
		}
	      else
		{
	      */
		  (*instruction)->operation = emulator->Decoder<CPU_CONFIG>::Decode((*instruction)->cia, (*instruction)->binary);
		  (*instruction)->operation_refcount = NULL; 
		  //(*instruction)->operation_refcount = new int(1);
		  /*	}*/
	    }
	  
	    //		  instruction->operation = (static_cast<full_system::processors::powerpc::powerpc755::Decoder *>(emulator))->Decode(instruction->cia);
	      
	  //  instruction->operation = (static_cast<full_system::processors::powerpc::powerpc755::Decoder *>(emulator))->Decode(instruction->cia, buffer);
	  
	  if (!(*instruction)->operation)
	    {
	      cerr << "BUG Decode Fail !!!! " << endl;
	      exit(-1);
	    }
		/* Initialize the function of the instruction */
		(*instruction)->fn = (*instruction)->operation->function;

		/* System and System Call instructions are execution serialized */
		//DD		(*instruction)->execution_serialized = (*instruction)->fn & (FnSysCall | FnSystem);
		(*instruction)->execution_serialized = ((*instruction)->fn == (FnSysCall));

		/* Raise an invalid opcode exception if necessary */
		(*instruction)->exception = ((*instruction)->fn == FnInvalid) ? INVALID_OPCODE_EXCEPTION : NONE_EXCEPTION;

		/* Initialize all remaining fields */
		(*instruction)->btb_miss = false;
		(*instruction)->ras_miss = false;
		(*instruction)->nia = 0;
		(*instruction)->predicted_nia = 0;
		(*instruction)->branch_direction = DontKnow;
		(*instruction)->predicted_branch_direction = DontKnow;
		(*instruction)->tag = -1;
		(*instruction)->ea = 0;
		(*instruction)->must_reschedule = false;
		(*instruction)->replay_trap = false;
		(*instruction)->may_need_replay = false;
		(*instruction)->obq_tag = -1;

		//if ((*instruction)->splitted_instruction) {delete (*instruction)->splitted_instruction;}
		//(*instruction)->splitted_instruction=NULL;
		(*instruction)->mustbechecked = true;
		(*instruction)->binary = (*instruction)->operation->GetEncoding();
		
		(*instruction)->splitted_instruction = NULL;

		//		Source<T> *source = (*instruction)->sources;
		//		Destination<T> *destination = &(*instruction)->destination;
		vector<Source> *source = &(*instruction)->sources;
		//		Destination *destination = &(*instruction)->destination;
		vector<Destination> *destination = &(*instruction)->destinations;
		int i;
		int nbsource=0;
		int nbdestination=0;
		source->clear();
		destination->clear();
#ifdef DD_DEBUG_PREDECODE
		if ((*instruction)->exception == INVALID_OPCODE_EXCEPTION)
		{
		  cerr << timestamp() << " -- PRE-DECODE !!! INVALID INSTRUCTION !!! ..............................................." << endl;
		}
#endif
		if ((*instruction)->exception != INVALID_OPCODE_EXCEPTION)
		{
		/* Fill in instruction source and destination operand informations */
		for(i = 0; i < (*instruction)->operation->noperands; i++)
		{
		  switch((*instruction)->operation->operands[i].dir)
		  {
		    case INPUT_T:
		      //if(source - (*instruction)->sources < nSources)
		      //if(nbsource < nSources)
		      if(nbsource < nSources)
			{
			  //Source *esource = new Source();
			  Source esource;
			  source->push_back(esource);
			  
			  /* Instruction has an integer register source operand */
			  //			  (*source)[nbsource].type = GPR_T;
			  (*source)[nbsource].type = (*instruction)->operation->operands[i].type;
			  (*source)[nbsource].reg = (*instruction)->operation->operands[i].value;
			  (*source)[nbsource].tag = -1;
			  (*source)[nbsource].data = 0;
			  (*source)[nbsource].ready = false;
			  (*source)[nbsource].valid = false;
			  nbsource++;
			}
		      else
			{
			  cerr << "too many input operands for instruction (" << instruction << ")" << endl;
			  cerr << "[timestamp()=" << timestamp() << "]" << endl; 
			  cerr << "Inst: " << *instruction << endl;
			  exit(-1);
			}
		      break;
		      
		  case OUTPUT_T:
		    //if(destination - &(*instruction)->destination < 1)
		    //if (nbdestination < nDestinations)
		    if(1) // Number of destination is now unlimited !!!
		      {
			Destination edestination;
			destination->push_back(edestination);
			
			/* Instruction has an integer register destination operand */
			//			(*destination)[nbdestination]type = GPR_T;
			(*destination)[nbdestination].type = (*instruction)->operation->operands[i].type;
			(*destination)[nbdestination].reg = (*instruction)->operation->operands[i].value;
			(*destination)[nbdestination].tag = -1;
			(*destination)[nbdestination].data = 0;
			nbdestination++;
			//cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== FETCH ==== Decode: destination type = INT" << endl;
			
		      }
		    else
		      {
			cerr << "too many output operands for instruction (" << instruction << ")" << endl;
			cerr << "Inst: " << *instruction << endl;
			cerr << "Noper: " << (*instruction)->operation->noperands << endl;
			cerr << "Oper: " << (*instruction)->operation << endl;
			for (int i=0; i<(*instruction)->sources.size(); i++)
			  cerr << (*instruction)->sources[i];
			cerr << endl;
			//cerr << (*instruction)->destination << endl;
			// cerr << hex << destination << " // " << &(*instruction)->destination << dec << endl;
			exit(-1);
		      }
		    break;
		  }
		  
		  //  default: /* neither GPR_T nor FPR_T */
		  /* instruction that has an operand which is neither an integer or a floating point register is execution serialized */
		  /* State of the input operand will be maintained by the emulator */
		  //if((*instruction)->operation->operands[i].dir == INPUT_T)
		  //  {
		  //    (*instruction)->execution_serialized = true;
		  //  }
		  //cerr << "==== FETCH ==== Decode: instruction serialized !!!!!! " << instruction << ")" << endl;
		  
		  //		  break;
		  //*/
		}
		}
		/* Reset all remaining source operands informations */
		//		while(source - (*instruction)->sources < nSources)
		// DD No more useful since we are now using a dynamique vector ...
		/*
		while(nbsource < nSources)
		{
		  Source esource;
		  source->push_back(esource);
			(*source)[nbsource].type = NONE_REGISTER;
			(*source)[nbsource].reg = -1;
			(*source)[nbsource].tag = -1;
			(*source)[nbsource].data = 0;
			nbsource++;
		}
		*/
		/* Reset all remaining destination operands informations */
		//		while(destination - &(*instruction)->destination < 1)
		// DD No more useful since we are now using a dynamique vector ...
		/*
		while(nbdestination - nDestination[] < 1)
		{
			destination->type = NONE_REGISTER;
			destination->reg = -1;
			destination->tag = -1;
			destination->data = 0;
			destination++;
		}
		*/
		/* By default the next instruction address is the address of the following instruction */
		(*instruction)->nia = (*instruction)->cia + InstructionSize;
	}

	/** Decodes an instruction
		@param instruction an instruction
	*/
	//DD	void Decode(InstructionPtr<T, nSources>& instruction)
	void DecodeSplitted(InstructionPtr *instruction, InstructionPtr *splitted_instruction, Operation<CPU_CONFIG> *op)
	{
		/* Call the emulator decode function */
	        //	        instruction->operation = emulator->Decode(instruction->cia);
	  //full_system::processors::powerpc::CodeType buffer;
	  //emulator->Fetch(&buffer, instruction->cia, sizeof(buffer));
	  //	  cerr << " INFO [DECODE]: cia = " << hexa(instruction->cia) << " ..." << endl;
	  //	  *(instruction->operation) = *op;//emulator->Decoder::Decode(instruction->cia);
	  (**instruction) = (**splitted_instruction);

	  (*instruction)->operation = emulator->Decoder<CPU_CONFIG>::NCDecode((*instruction)->cia,op->GetEncoding());
	  (*instruction)->operation_refcount = new int(1);
	  //	  instruction->operation = emulator->Decoder::Decode(instruction->cia);
	  //instruction->operation = new Operation(*op);
	  //	  *(instruction->operation) = *op;
	  //instruction->operation = op;
	  
	  if (!(*instruction)->operation)
	    {
	      cerr << "BUG Decode Fail !!!! " << endl;
	      exit(-1);
	    }
		/* Initialize the function of the instruction */
		(*instruction)->fn = (*instruction)->operation->function;
		// For splitted instruction We have to change the function
		unisim::component::cxx::processor::powerpc::CodeType code = (*instruction)->operation->GetEncoding();
		if ( ( (code >> 26) == 14) 
		     ||
		     ( ( (code >> 26) == 31) 
		       && ((code >> 1) & 0xff )==266 )
		     )
		  {
		    (*instruction)->fn = FnIntBasic;
		    //		    cerr << "DDDEBUG : Function changed !!!: " << (*instruction) << endl; 
		  }

		/* System and System Call instructions are execution serialized */
		//DD		(*instruction)->execution_serialized = (*instruction)->fn & (FnSysCall | FnSystem);
		(*instruction)->execution_serialized = (*instruction)->fn & (FnSysCall);

		/* Raise an invalid opcode exception if necessary */
		(*instruction)->exception = ((*instruction)->fn == FnInvalid) ? INVALID_OPCODE_EXCEPTION : NONE_EXCEPTION;

		/* Initialize all remaining fields */
		(*instruction)->btb_miss = false;
		(*instruction)->ras_miss = false;
		(*instruction)->nia = 0;
		(*instruction)->predicted_nia = 0;
		(*instruction)->branch_direction = DontKnow;
		(*instruction)->predicted_branch_direction = DontKnow;
		(*instruction)->tag = -1;
		(*instruction)->ea = 0;
		(*instruction)->must_reschedule = false;
		(*instruction)->replay_trap = false;
		(*instruction)->may_need_replay = false;
		(*instruction)->obq_tag = -1;

		(*instruction)->mustbechecked = false;
		//if ((*instruction)->splitted_instruction) {delete (*instruction)->splitted_instruction;}
		
		//InstructionPtr *newInst = new InstructionPtr();
		//((*instruction)->splitted_instruction) = splitted_instruction;
		/*
		if ( ((*instruction)->splitted_instruction) != NULL )
		  {
		    delete ((*instruction)->splitted_instruction);
		  }
		*/
		((*instruction)->splitted_instruction) = NULL;
		//		((*instruction)->splitted_instruction) = new InstructionPtr();
		// Copying InstructionPtr ensure that refcount is updated.
		//		*((*instruction)->splitted_instruction) = *splitted_instruction;
		// The new allocated instruction have to be deleted into IntructionPtr destructor.

		//		(*instruction)->splitted_instruction_refcount = new int(1);
		//*((*instruction)->splitted_instruction) = *(splitted_instruction);
		  //		*((*instruction)->splitted_instruction) = splitted_instruction;
		  /*
		Decode((*instruction)->splitted_instruction);
		(*instruction)->binary = (*instruction)->operation->GetEncoding();
		  */
		//		Source<T> *source = (*instruction)->sources;
		//		Destination<T> *destination = &(*instruction)->destination;
		vector<Source> *source = &(*instruction)->sources;
		vector<Destination> *destination = &(*instruction)->destinations;
		int i;
		int nbsource=0;
		int nbdestination=0;
		source->clear();
		destination->clear();


		/* Fill in instruction source and destination operand informations */
		for(i = 0; i < (*instruction)->operation->noperands; i++)
		{
		  switch((*instruction)->operation->operands[i].dir)
		  {
		    case INPUT_T:
		      //if(source - (*instruction)->sources < nSources)
		      //if(nbsource < nSources)
		      if(nbsource < nSources)
			{
			  //Source *esource = new Source();
			  Source esource;
			  source->push_back(esource);
			  
			  /* Instruction has an integer register source operand */
			  //			  (*source)[nbsource].type = GPR_T;
			  (*source)[nbsource].type = (*instruction)->operation->operands[i].type;
			  (*source)[nbsource].reg = (*instruction)->operation->operands[i].value;
			  (*source)[nbsource].tag = -1;
			  (*source)[nbsource].data = 0;
			  (*source)[nbsource].ready = false;
			  (*source)[nbsource].valid = false;
			  nbsource++;
			}
		      else
			{
			  cerr << "too many input operands for instruction (" << instruction << ")" << endl;
			  cerr << "Inst: " << *instruction << endl;
			  exit(-1);
			}
		      break;
		      
		  case OUTPUT_T:
		    //if(destination - &(*instruction)->destination < 1)
		    //if (nbdestination < nDestinations)
		    if(1) // Number of destination is now unlimited !!!
		      {
			Destination edestination;
			destination->push_back(edestination);
			
			/* Instruction has an integer register destination operand */
			//			(*destination)[nbdestination]type = GPR_T;
			(*destination)[nbdestination].type = (*instruction)->operation->operands[i].type;
			(*destination)[nbdestination].reg = (*instruction)->operation->operands[i].value;
			(*destination)[nbdestination].tag = -1;
			(*destination)[nbdestination].data = 0;
			nbdestination++;
			//cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== FETCH ==== Decode: destination type = INT" << endl;
			
		      }
		    else
		      {
			cerr << "too many output operands for instruction (" << instruction << ")" << endl;
			cerr << "Inst: " << *instruction << endl;
			cerr << "Noper: " << (*instruction)->operation->noperands << endl;
			cerr << "Oper: " << (*instruction)->operation << endl;
			for (int i=0; i<(*instruction)->sources.size(); i++)
			  cerr << (*instruction)->sources[i];
			cerr << endl;
			//cerr << (*instruction)->destination << endl;
			//cerr << hex << destination << " // " << &(*instruction)->destination << dec << endl;
			exit(-1);
		      }
		    break;
		  }
		  
		  //  default: /* neither GPR_T nor FPR_T */
		  /* instruction that has an operand which is neither an integer or a floating point register is execution serialized */
		  /* State of the input operand will be maintained by the emulator */
		  //if(instruction->operation->operands[i].dir == INPUT_T)
		  //  {
		  //    instruction->execution_serialized = true;
		  //  }
		  //cerr << "==== FETCH ==== Decode: instruction serialized !!!!!! " << instruction << ")" << endl;
		  
		  //		  break;
		  //*/
		}
		
		/* Reset all remaining source operands informations */
		//		while(source - instruction->sources < nSources)
		// DD No more useful since we are now using a dynamique vector ...
		/*
		while(nbsource < nSources)
		{
		  Source esource;
		  source->push_back(esource);
			(*source)[nbsource].type = NONE_REGISTER;
			(*source)[nbsource].reg = -1;
			(*source)[nbsource].tag = -1;
			(*source)[nbsource].data = 0;
			nbsource++;
		}
		*/
		/* Reset all remaining destination operands informations */
		//		while(destination - &instruction->destination < 1)
		// DD No more useful since we are now using a dynamique vector ...
		/*
		while(nbdestination - nDestination[] < 1)
		{
			destination->type = NONE_REGISTER;
			destination->reg = -1;
			destination->tag = -1;
			destination->data = 0;
			destination++;
		}
		*/

		/* By default the next instruction address is the address of the following instruction */
		(*instruction)->nia = (*instruction)->cia + InstructionSize;
	}

/////////////////////////////////////// FILL INSTRUCTION QUEUE /////////////////////
	void fill_instruction_queue(uint64_t local_cia)
	{

				int i;
				int offset;
				int n = accessSize / InstructionSize;

				char tmpbuff[nCPUDataPathSize+1];
				uint32_t *tmpint;
				uint32_t ll_cia;

				emulator->ReadMemory(local_cia,tmpbuff,nCPUDataPathSize);

#ifdef DD_DEBUG_FILLING
				//cerr << "---- ---- ---- Decoded InSt : " << (entry->instruction) << endl;
				cerr << "TMPBUF: ";
				for (int i=0; i<nCPUDataPathSize; i++)
				{
					cerr << hex << (tmpbuff[i] & 0xf ) << (tmpbuff[i] & 0xf0 ) << dec << " ";
				}
				cerr << endl;
#endif				

				ByteArray<nCPUDataPathSize> vector;// = tmpmemreq.data;
                                //for(i = 0, offset = 0; i < n; I++, offset += InstructionSize)
                                for(i = 0; i < nCPUDataPathSize; i++)
				{
					vector.Write((uint8_t)tmpbuff[i],i, endianess);
				}


#ifdef DD_DEBUG_FILLING
				//cerr << "---- ---- ---- Decoded InSt : " << (entry->instruction) << endl;
				cerr << "VECTOR: " << vector << endl;
#endif				

				ll_cia = local_cia;
                              //  for(i = 0, offset = 0; i < n; i++, cia += InstructionSize, offset += InstructionSize)
                                for(i = 0, offset = 0;
					 i < n, instructionQueue.Size()<(InstructionQueueSize/2)-1;
					 i++, ll_cia += InstructionSize, offset += InstructionSize)
                                {
#ifdef DD_DEBUG_FILLING
	cerr << "DDDEBUG In loop:  ---  InstructionQueue.Size()      = " << instructionQueue.Size() << endl;
#endif
                                        /* Allocate an instruction queue entry */
                                        //DD//InstructionQueueEntry<T, nSources> *entry = instructionQueue.New();
                                  InstructionQueueEntry *entry = instructionQueue.New();
                                        //QueuePointer<InstructionQueueEntry, InstructionQueueSize > entry;
                                  //                                    entry = instructionQueue.New();

                                        if(!entry)
                                        {
                                                /* Throw an error if instruction queue is full */
                                                cerr << "time stamp = " << timestamp() << endl;
                                                cerr << "warning: instruction queue overflow" << endl;
                                                exit(-1);
                                        }

                                        //      InstructionPtr instruction;
                                        // VALGRIND TODO
                                        InstructionPtr *new_instruction = new InstructionPtr();
                                        entry->instruction = *new_instruction;
                                        // VALGRIND MEMORY LEAK BUGFIXE
                                        delete new_instruction;

                                        entry->instruction->cia = ll_cia;//emulator->GetCIA();
                                        entry->instruction->operation = 0;                                                                                                  entry->instruction->inum = inum++;
                                        entry->predecoded = false;
                                        entry->executed_in_fetch = false;
                                        vector.Read(entry->instruction->binary, offset, endianess);
                                        // For dump
                                        entry->instruction->timing_fetch_cycle = timestamp();

				}
	}
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ALL_PERFECT
  /////////////////////////////////////////////////////////////////////////////////////////////////////////

	/** A SystemC process managing the fetch stage */
	//	void InternalControl()
        void end_of_cycle()
	{
		int i;

		instruction_queue_cumulative_occupancy += instructionQueue.Size();

		/* Remove accepted instructions from the instruction queue */
		for(i = 0; i < Width; i++)
		{
		  //DD			if(!inAccept[i] || instructionQueue.Empty()) break;
			if(!outInstruction[i].accept || instructionQueue.Empty()) break;
			instructionQueue.RemoveHead();
			//			changed = true;
		}

		/* If instruction cache accepted a request */
		//DD		if(inL1Accept && waiting_instr_cache_accept)
		//		if(outIL1.accept && waiting_instr_cache_accept)
		if(outIL1.accept)
		{
			/* increment the current instruction address of the sequential fetcher */
			seq_cia += accessSize;
			//			waiting_instr_cache_accept = false;
			//			changed = true;
			pending_instr_cache_requests++;
			pending_instr_cache_access_size += accessSize;
		}
		
		/* Update the branch predictors according to the retired branches */
		/* For each retire port */
		for(i = 0; i < RetireWidth; i++)
		//		for(i = 0; i < Width; i++)
		{
			/* Is there a retired instruction ? */
		  //DD			if(!inRetireEnable[i]) break;
			if(!inRetireInstruction[i].enable) break;

			/*
			  #ifdef DD_DEBUG_FETCH_VERB2
			  cerr << "[FETCH] ==== We are after the Break; (EOF) ========== ["<< timestamp()<<"]"<< endl;
			  #endif
			*/
			//			changed = true;

			/* Get the retired instruction */
			//DD			const InstructionPtr<T, nSources>& instruction = inRetireInstruction[i];
		        InstructionPtr instruction = inRetireInstruction[i].data;
			/*
			  #ifdef DD_DEBUG_FETCH_VERB2
			  cerr << "[FETCH] ["<< timestamp()<<"]  /// " << instruction << endl;
			  #endif
			*/
			if (instruction->fn == FnSysCall)
			  { 
#ifdef DD_DEBUG_PERFECT_BRANCH_V2
		      if (DD_DEBUG_TIMESTAMP < timestamp())
			{
			  //				cerr << "Entry: " << entry << endl; 
			  cerr << "TimeStamp (Before SYSCALL EXECUTION) = " << timestamp() << endl;
			  cerr << instruction << endl;
			  cerr << "DD_DEBUG_PERFECT: Emulator->CIA() after="<< hex << emulator->GetCIA() << dec << endl;
			  cerr << "DD_DEBUG_PERFECT: Emulator->NIA() after="<< hex << emulator->GetNIA() << dec << endl;
			}
#endif
			    syscall_in_pipeline = false;
			    // When the syscall is retire then we can emulate it 
			    // and continue fetching...
			    emulator->SetCIA(instruction->cia);
			    emulator->SetNIA(instruction->cia+InstructionSize);
			    /*
			    try{
			      instruction->operation->execute(emulator);
			    }
			    catch(Exception &e)
			      {
				// Here we catch some trap exceptions ???
			      }
			    */
			    emulator->StepOneInstruction();
			    cia = emulator->GetNIA();
			    nia = emulator->GetNIA()+InstructionSize;
#ifdef DD_DEBUG_PERFECT_BRANCH_V2
		      if (DD_DEBUG_TIMESTAMP < timestamp())
			{
			  //				cerr << "Entry: " << entry << endl; 
			  cerr << "TimeStamp (SYSCALL EXECUTION) = " << timestamp() << endl;
			  cerr << instruction << endl;
			  cerr << "DD_DEBUG_PERFECT: Emulator->CIA() after="<< hex << emulator->GetCIA() << dec << endl;
			  cerr << "DD_DEBUG_PERFECT: Emulator->NIA() after="<< hex << emulator->GetNIA() << dec << endl;
			}
#endif
#ifdef DD_DEBUG_PERFECT_BRANCH_V2
		      if (DD_DEBUG_TIMESTAMP < timestamp())
			{
			  for (int i=0; i < unisim::component::clm::processor::ooosim::nIntegerArchitecturalRegisters; i++)
			    {
			      cerr << "GPR["<<i<<"]=" << hex << emulator->GetGPR(i) << dec << endl;
			      //if (i%8==0) { cerr << endl;}
			    }
			  cerr << "LR=" << hex << emulator->GetLR() << dec << endl;
			  cerr << "CR=" << hex << emulator->GetCR() << dec << endl;
			}
#endif
			    
			  }

			if(instruction->fn & FnLoad)
			{
				if(instruction->replay_trap)
				{
					nia = instruction->cia;	/* nia is used by flush to redirect fetch on the correct path (ie replay the load) */
// 					cerr << "!!!!! time stamp = " << sc_time_stamp() << ": got replay trap: must continue fetching along " << hexa(nia) << endl;
#ifdef DD_DEBUG_FETCH_VERB2
					if (DD_DEBUG_TIMESTAMP < timestamp())
					  {
			  cerr << "[FETCH] ==== We are setting NIA in Replay (EOF) ========== ["<< timestamp()<<"]"<< endl;
					  }
#endif
				}
			}
			else
			{
				if( instruction->operation->function
				    &
				    (FnBranch | FnConditionalBranch | FnBranchCountReg | FnBranchLinkReg)
				    )
				{
				   if(instruction->predicted_nia != instruction->nia)
				   {
				     
				     cerr << "WARNING(" <<timestamp() <<"): Bad branch prediction..." << endl;
				     cerr << "Instruction: " << instruction << endl;
				     nia = instruction->nia; 
				   }
				}
			}//end of else
		} // end of for(...)
		//	}// end of end_of_cycle


	bool previous_splitting=false;
	//////////////////////////////////////////////////////////////////////////////////////////////////
	///      THE BIG WHILE LOOP
	//////////////////////////////////////////////////////////////////////////////////////////////////
	while ( 
		(instructionQueue.Size()<(InstructionQueueSize/2)-1 )
		&&
		(!syscall_in_pipeline)
//		||
//		previous_splitting
	)
	{

#ifdef DD_DEBUG_FILLING
		cerr << "DDDFILL: CIA Before filling = " << hex << cia << dec << endl;
#endif
#ifdef DD_DEBUG_FILLING
		cerr << "DDDEBUG BF:  ---  InstructionQueueSize         = " << InstructionQueueSize << endl;
		cerr << "DDDEBUG BF:  ---  InstructionQueue.Size()      = " << instructionQueue.Size() << endl;
		cerr << "DDDEBUG BF:  ---  InstructionQueue.FreeSpace() = " << instructionQueue.FreeSpace() << endl;
#endif
		if (!syscall_in_pipeline)
		{ 
		   fill_instruction_queue(cia);
		}
#ifdef DD_DEBUG_FILLING
		cerr << "DDDEBUG AF:  ---  InstructionQueueSize         = " << InstructionQueueSize << endl;
		cerr << "DDDEBUG AF:  ---  InstructionQueue.Size()      = " << instructionQueue.Size() << endl;
		cerr << "DDDEBUG AF:  ---  InstructionQueue.FreeSpace() = " << instructionQueue.FreeSpace() << endl;
#endif

		QueuePointer<InstructionQueueEntry, InstructionQueueSize > entry;

		/* Branch Prediction */
		/* For each instruction queue entry */
		/*
		if (instructionQueue.Size() == 0) 
		  { 
		    //		    cerr << " ----->    BUG : \t qsize   = 0 ; ts = " << timestamp() << endl;
		    //		exit(-1);
		  }
		*/
		//		for(entry = instructionQueue.SeekAtHead(); entry && !syscall_in_pipeline; entry++)
		entry = instructionQueue.SeekAtHead();// entry && !syscall_in_pipeline; entry++)

		while (entry && !syscall_in_pipeline)
		{
   #ifdef DDDFILLING
		  cerr << "DDDFILL: While Entry CIA: " << hex << cia << dec << endl;
		  cerr << "DDDFILL: While Entry INS: " << entry->instruction << endl;
   #endif
		  previous_splitting = false;
		  //		  cerr << " ----->    BUG : \t QueueSize = " << instructionQueue.Size() << endl;
			/* Was instruction predecoded ? */
		  if(!entry->predecoded)
		    {
#ifdef DDDFILLING
		      cerr << "\t --- DDDFILL: While: in Predecoding... "<< endl;
#endif //DDDFILLING
			  fetched_instructions++;
			  //				changed = true;
			  /* Decode the instruction */
			  Decode(&(entry->instruction));
				
#ifdef DD_DEBUG_SPLITTING
				cerr << "---- ---- ---- Decoded Inst : " << (entry->instruction) << endl;
#endif				
				if (entry->instruction->operation->is_splitted())
				  {
				    previous_splitting = true;
				    splitted_instructions++;
				    if (!(instructionQueue.FreeSpace()>0))
				    //  if (!instructionQueue.Full())
				      {
					cerr << "INSTRUCTION SPLITTING: No FreeSpace!!!!!!!!!!!" << endl;
					cerr << "Inst : " << (entry->instruction) << endl;
					cerr << " FreeSpace = " << instructionQueue.FreeSpace() << endl;
					exit(-1);

				      }
				    else
				      {
#ifdef DD_DEBUG_SPLITTING
					cerr << "Splitted instruction: "<< (entry->instruction) << endl;
					cerr << "INSTRUCTION WILL BE SPLITTED !!!!!!!!!!!" << endl;
					//	cerr << "Inst : " << *(entry->instruction) << endl;
					cerr << instructionQueue << endl;
#endif
					QueuePointer<InstructionQueueEntry, InstructionQueueSize > new_entry;
					QueuePointer<InstructionQueueEntry, InstructionQueueSize > old_entry;
					//InstructionQueueEntry *new_entry; 
					//InstructionQueueEntry *old_entry; 
					//new_entry = instructionQueue.New();
					instructionQueue.New();
					new_entry = QueuePointer<InstructionQueueEntry,InstructionQueueSize>(&instructionQueue,instructionQueue.GetIndex(0));
					//	new_entry = new InstructionQueueEntry();
					if (!new_entry)
					  {
					    cerr << "New entry not allocated !!!!!!!!!!!" << endl;
					    cerr << "Inst : " << (entry->instruction) << endl;
					    exit(-1);
					  }
					/*
					InstructionPtr new_instruction;
					new_entry->instruction = new_instruction;
					*/
					old_entry = new_entry;
					while ( ! (&(*new_entry) == &(*entry)) )
					  {
					    //old_entry = (old_entry + InstructionQueueSize-1) % InstructionQueueSize;
					    old_entry--;
					    
					    //*(new_entry->instruction) = *(old_entry->instruction);
					    (new_entry->instruction) = (old_entry->instruction);
					    new_entry->predecoded = old_entry->predecoded;
					    new_entry->executed_in_fetch = old_entry->executed_in_fetch;
					    //new_entry = (new_entry + InstructionQueueSize-1) % InstructionQueueSize;
					    new_entry--;
					  };
#ifdef DD_DEBUG_SPLITTING
					cerr << "INSTRUCTION WILL BE SPLITTED 2 !!!!!!!!!!!" << endl;
					//	cerr << "Inst : " << *(entry->instruction) << endl;
					cerr << instructionQueue << endl;
#endif
					//					inum;
					//new_entry = entry;
					//InstructionPtr *newsplitted_instruction = new InstructionPtr();
					InstructionPtr newsplitted_instruction = entry->instruction;
					//InstructionPtr newsplitted = entry->instruction;

					list <Operation<CPU_CONFIG> *> lops;
					lops = new_entry->instruction->operation->split_into();
					QueuePointer<InstructionQueueEntry, InstructionQueueSize > previous_entry;
					previous_entry=new_entry;
					while (lops.size()>0)
					  {
					    Operation<CPU_CONFIG> *ops = lops.front();
#ifdef DD_DEBUG_SPLITTING
					    cerr << "LOPS[...] : ";
					    ops->disasm(NULL,cerr);
					    cerr << "--- CodeType: "<< hex << ops->GetEncoding() << dec;
					    cerr << endl;
#endif
					    // VALGRIND TODO...
					    InstructionPtr *newInst = new InstructionPtr();// Ensure a new instruction is created
					    new_entry->instruction = *newInst;
					    // VALGRIND BUGFIXE
					    delete newInst;

					    DecodeSplitted(&(new_entry->instruction), &newsplitted_instruction, ops);
					    //    new_entry->instruction->inum = inum++;
					    new_entry->predecoded = true;
					    // FREE ops...
					    delete ops;
					    ops = NULL;

					    lops.pop_front();
					    previous_entry=new_entry;
					    new_entry++;
#ifdef DD_DEBUG_SPLITTING
					    cerr << "INSTRUCTION WILL BE SPLITTED 3 !!!!!!!!!!!" << endl;
					    //	cerr << "Inst : " << *(entry->instruction) << endl;
					    cerr << instructionQueue << endl;
#endif
					  }
					//delete newsplitted_instruction;
					//new_entry--;
					previous_entry->instruction->mustbechecked = true;
					//entry--;
					//DecodeSplitted(entry->instruction, lops.pop_front());
#ifdef DD_DEBUG_SPLITTING
					cerr << "INSTRUCTION HAS BEEN SPLITTED !!!!!!!!!!!" << endl;
					//	cerr << "Inst : " << *(entry->instruction) << endl;
					cerr << instructionQueue << endl;
#endif
				      }
				    /*
				    cerr << "SPLIT THE INSTRUCTION !!!!!!!!!!!" << endl;
				    cerr << "Inst : " << *(entry->instruction) << endl;
				    exit(-1);
				    */

				    //entry--;

				  }

				// If decoded instruction is a branch
				if( entry->instruction->operation->function
				    &
				    (FnBranch | FnConditionalBranch | FnBranchCountReg | FnBranchLinkReg)
				    )
				{
					if(in_flight_branches < MaxBranches)
					  {
					    in_flight_branches++;
					  }
					else	
					  {
#ifdef DD_DEBUG_FETCH_PREDECODE
  if (DD_DEBUG_TIMESTAMP < timestamp())
    {
      cerr << "["<<this->name()<<"("<<timestamp()<<")] MAXIMUM INFLIGHT BRANCHES !!! " << instructionQueue << endl;
      cerr << *(entry->instruction) << endl;
    }
#endif 
                                             //break;
					  }
				}
				entry->predecoded = true;
				//entry->executed_in_fetch=false;
		    } //End of//: if (!entry->predecoded)
		  /*			if ()
					{
					previous_splitting = true;
					}
		  */
		  //////////////////////////////////////////////////////////////////////////////
		  //#ifdef DDDEBUG_USE_PERFECT_BRANCH_PREDICTION
		  /*
		    bool previous_was_splitted = false;	
		    if (entry->instruction->operation->is_splitted())
		    {
		    entry++;
		    previous_was_splitted=true;
		    }
		  */
#ifdef DDDFILLING
		      cerr << "\t\t--- DDDFILL: While: Between Predecoding and Executing... "<< endl;
		      cerr << entry->instruction << endl;
#endif //DDDFILLING
		      //		      if( entry->instruction->operation->function == FnSysCall )
		      //			{ 
		      if (!entry->executed_in_fetch && !(entry->instruction->operation->function == FnSysCall))
		     {
#ifdef DDDFILLING
		      cerr << "\t\t--- DDDFILL: While: in Executing... "<< endl;
#endif //DDDFILLING
		      emulator->SetCIA(cia);
		      emulator->SetNIA(cia+InstructionSize);
#ifdef DD_DEBUG_PERFECT_BRANCH
		      cerr << "DD_DEBUG_PERFECT: Emulator->CIA() before="<< hex << emulator->GetCIA() << dec << endl;
		      cerr << "DD_DEBUG_PERFECT: Emulator->NIA() before="<< hex << emulator->GetNIA() << dec << endl;
#endif
		      try{
			entry->instruction->operation->execute(emulator);
		      }
		      catch(Exception &e)
			{
			  // Here we catch some trap exceptions ???
			}
#ifdef DD_DEBUG_PERFECT_BRANCH_V2
		      if (DD_DEBUG_TIMESTAMP < timestamp())
			{
			  //				cerr << "Entry: " << entry << endl; 
			  cerr << "TimeStamp = " << timestamp() << endl;
			  cerr << entry->instruction << endl;
			  cerr << "DD_DEBUG_PERFECT: Emulator->CIA() after="<< hex << emulator->GetCIA() << dec << endl;
			  cerr << "DD_DEBUG_PERFECT: Emulator->NIA() after="<< hex << emulator->GetNIA() << dec << endl;
			}
#endif
#ifdef DD_DEBUG_PERFECT_BRANCH_V2
		      if (DD_DEBUG_TIMESTAMP < timestamp())
			{
			  for (int i=0; i < unisim::component::clm::processor::ooosim::nIntegerArchitecturalRegisters; i++)
			    {
			      cerr << "GPR["<<i<<"]=" << hex << emulator->GetGPR(i) << dec << endl;
			      //if (i%8==0) { cerr << endl;}
			    }
			  cerr << "LR=" << hex << emulator->GetLR() << dec << endl;
			  cerr << "CR=" << hex << emulator->GetCR() << dec << endl;
			}
#endif

		      // Set perfect prediction of effective address
		      if (entry->instruction->operation->function & FnLoad ||
			  entry->instruction->operation->function & FnStore )
			{
			  entry->instruction->pred_ea = emulator->GetEA();
			}

		      //				if (!previous_was_splitted)
		      //				if (!entry->instruction->operation->is_splitted())
		      if (!previous_splitting)
			{ 
			  cia += InstructionSize;
			}
		      //				emulator->StepOneInstruction();
		      //entry->instruction->predicted_nia = entry->instruction->cia + InstructionSize;
		      if( entry->instruction->operation->function
			  &
			  (FnBranch | FnConditionalBranch | FnBranchCountReg | FnBranchLinkReg)
			  )
			{
			  /* Correct the path */
			  // cia = entry->instruction->nia;
			  cia = emulator->GetNIA();
   #ifdef DD_DEBUG_FILLING
			  cerr << "DDDFILL: cia1 = " << hex << cia << dec << endl;
   #endif
			  //entry->instruction->predicted_nia = entry->instruction->operation->get_target_address(entry->instruction->cia,emulator);
			  //cia = entry->instruction->operation->get_target_address(entry->instruction->cia,emulator);
			  
   #ifdef DD_DEBUG_FILLING
			  cerr << "DDDFILL: cia2 = " << hex << cia << dec << endl;
   #endif
			  //nia = emulator->GetNIA();
			  seq_cia = cia;
			  entry->instruction->predicted_nia = entry->instruction->cia + InstructionSize;
			  if (entry->instruction->predicted_nia != cia)
			    {
#ifdef DD_DEBUG_PERFECT_BRANCH
			      cerr << "DD_DEBUG_PERFECT: Instruction before="<< hex << entry->instruction << dec << endl;
#endif
			      entry->instruction->nia = cia;
			      entry->instruction->predicted_nia = cia;
			      instructionQueue.FlushAfter(entry);
#ifdef DD_DEBUG_PERFECT_BRANCH
			      cerr << "DD_DEBUG_PERFECT: Instruction after ="<< hex << entry->instruction << dec << endl;
#endif
			      
			    }
			}

		      entry->executed_in_fetch = true;

		    }//End of:// if (!executed_in_fetch) ...

		      //#endif // USE_PERFECT_BRANCH_PREDICTION
		      /* If instruction is a SysCall */
		      if( entry->instruction->operation->function == FnSysCall )
			{ 
			  syscall_in_pipeline = true;
			  /* Flush the instruction after the branch */
			  instructionQueue.FlushAfter(entry);
			  
			  /* Correct the path */
			  cia = entry->instruction->nia;
			  seq_cia = cia;
			}
		      
		  ////////////////////////////////////////////////////////////////////////////
		  entry++;
		} // end while(entry && !syscall_in)
		
	} //End of:// THE BIG WHILE (InstructionQueue.Size() < IQS/2-1)
	//////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////
	//...

	///////////////////////////////////////// USE_PERFECT_FETCH ///////////////// ELSE !!!
	//#else		       	
		/* Flush ? */
		//		if(inFlush.data)
		if (inFlush.enable && inFlush.data.something())
		{
		  if (inFlush.enable && inFlush.data)
		  {
		    // DD DEBUGING PERFECT CONFLICT PREDICTION
		    cerr << "Error: a flush have been sent in perfect mode !!!" << endl;
		    abort();

#ifdef DD_DEBUG_FLUSH
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====  Flush !!!" << endl;
#endif
		  //			changed = true;
			/* Flush the instruction queue */
			instructionQueue.Flush();

			//if(waiting_instr_cache_accept) pending_instr_cache_requests--;
			ignore_instr_cache_responses = pending_instr_cache_requests;
			//waiting_instr_cache_accept = false;
			pending_instr_cache_access_size = 0;
			btb_miss = false;
			ras_miss = false;
			in_flight_branches = 0;

			syscall_in_pipeline = false;

#ifdef DD_DEBUG_FETCH_VERB2
			if (DD_DEBUG_TIMESTAMP < timestamp())
			    {
		  cerr << "[FETCH] ==== Correct fetcher on Correct path ======== ["<< timestamp()<<"]"<< endl;
		  //		  cerr << *this << endl;
			    }
#endif
			/* Reset output ports */
			/*
			outL1Valid = false;

			for(i = 0; i < Width; i++)
			{
				outValid[i] = false;
				outEnable[i] = false;
			}
			*/

//			RAS.FlushRollbackBuffer();
// 			BHT.Flush();
			

			/* Continue fetch on the correct path */
			seq_cia = cia = nia;
#ifdef DD_DEBUG_FETCH_VERB2
 if (DD_DEBUG_TIMESTAMP < timestamp())
			    {
			cerr << "!!!!! time stamp = " << timestamp() << ": got flush: continue fetching along " << hexa(nia) << endl;
			cerr << "[FETCH] ==== We are Flushing ... (EOF) ========== ["<< timestamp()<<"]"<< endl;
			//			cerr << *this << endl;
			    }
#endif
			
		  }
		}
		// Keep coherent CIA with emulator ...
//		emulator->SetCIA(cia);
#ifdef DD_DEBUG_FETCH_VERB2
 if (DD_DEBUG_TIMESTAMP < timestamp())
			    {
		cerr << "[FETCH] ==== Fetch state at EOF ========== ["<< timestamp()<<"]"<< endl;
		cerr << *this << endl;
			    }
#endif
#ifdef DD_DEBUG_FETCH_VERB3
 if (DD_DEBUG_TIMESTAMP < timestamp())
			    {
		cerr << "["<<this->name()<<"("<<timestamp()<<")] instruction queue at EOC: " << instructionQueue << endl;
			    }
#endif


#ifdef DD_DEBUG_PIPELINE_VERB1
 if (DD_DEBUG_TIMESTAMP < timestamp())
			    {
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ==== Pipeline Debug" << endl;
		cerr << this << endl;
			    }
#endif
	} /* End of ENDOF_CYCLE */
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
#else // #ifdef ALL_PERFECT
  /////////////////////////////////////////////////////////////////////////////////////////////////////////

	/** A SystemC process managing the fetch stage */
	//	void InternalControl()
        void end_of_cycle()
	{
		int i;

		//#ifdef DD_DEBUG_FETCH_VERB1
#ifdef DD_DEBUG_TEST_MOURAD
	  if (DD_DEBUG_TIMESTAMP < timestamp())
	    {
		  cerr << "===== DEBUG Fetcher (Begin) =====================" << "(" << timestamp() << ")" << endl;
		  //  cerr << "   Instruction Queue Size : " << instructionQueue.Size() << endl;
		  cerr << "   Instruction Queue  : " << endl;
		  cerr << instructionQueue << endl;
		  cerr << "===== DEBUG Fetcher (End) =======================" << "(" << timestamp() << ")" << endl;
	    }
#endif
		instruction_queue_cumulative_occupancy += instructionQueue.Size();

		/* return as soon as possible */
		//		if(!inAccept[0] && !inL1Accept && !inL1Enable && !inRetireEnable[0] && !inWriteBackEnable[0] && !inFlush && !changed) return;
		//		if(!inAccept[0] && !inL1Accept && !inL1Enable && !inRetireEnable[0] && !inWriteBackEnable[0] && !inFlush && !changed) return;

		//changed = false;

		/* Remove accepted instructions from the instruction queue */
		for(i = 0; i < Width; i++)
		{
		  //DD			if(!inAccept[i] || instructionQueue.Empty()) break;
			if(!outInstruction[i].accept || instructionQueue.Empty()) break;
			instructionQueue.RemoveHead();
			//			changed = true;
		}

		/* If instruction cache accepted a request */
		//DD		if(inL1Accept && waiting_instr_cache_accept)
		//		if(outIL1.accept && waiting_instr_cache_accept)
		if(outIL1.accept)
		{
			/* increment the current instruction address of the sequential fetcher */
			seq_cia += accessSize;
			//			waiting_instr_cache_accept = false;
			//			changed = true;
			pending_instr_cache_requests++;
			pending_instr_cache_access_size += accessSize;
		}
		
		/* Update the branch predictors according to the retired branches */
		/* For each retire port */
		for(i = 0; i < RetireWidth; i++)
		//		for(i = 0; i < Width; i++)
		{
			/* Is there a retired instruction ? */
		  //DD			if(!inRetireEnable[i]) break;
			if(!inRetireInstruction[i].enable) break;

			/*
			  #ifdef DD_DEBUG_FETCH_VERB2
			  cerr << "[FETCH] ==== We are after the Break; (EOF) ========== ["<< timestamp()<<"]"<< endl;
			  #endif
			*/
			//			changed = true;

			/* Get the retired instruction */
			//DD			const InstructionPtr<T, nSources>& instruction = inRetireInstruction[i];
		        InstructionPtr instruction = inRetireInstruction[i].data;
			/*
			  #ifdef DD_DEBUG_FETCH_VERB2
			  cerr << "[FETCH] ["<< timestamp()<<"]  /// " << instruction << endl;
			  #endif
			*/
			if (instruction->fn == FnSysCall)
			  { syscall_in_pipeline = false; }

			if(instruction->fn & FnLoad)
			{
				if(instruction->replay_trap)
				{
					nia = instruction->cia;	/* nia is used by flush to redirect fetch on the correct path (ie replay the load) */
// 					cerr << "!!!!! time stamp = " << sc_time_stamp() << ": got replay trap: must continue fetching along " << hexa(nia) << endl;
#ifdef DD_DEBUG_FETCH_VERB2
					if (DD_DEBUG_TIMESTAMP < timestamp())
					  {
			  cerr << "[FETCH] ==== We are setting NIA in Replay (EOF) ========== ["<< timestamp()<<"]"<< endl;
					  }
#endif
				}
			}
			else
			{
#ifndef USE_PERFECT_FETCH
			  /* If the instruction is a branch */
			  //			  if(instruction.fn & FnBranch)
			  if ( instruction->operation->function &
			       (FnBranch | FnConditionalBranch | FnBranchCountReg | FnBranchLinkReg)
			       )
			    {
			      /* If the instruction is a return branch or a call branch */
			      //if((instruction.fn & FnReturnBranch) || ((instruction.fn & FnCallBranch) && instruction.destination.reg >= 0))
			      // TODO DAVID
			      // We only retire on RAS if the call or return is unconditional or well predicted
			      /*
			      if(instruction.fn == FnBranchLinkReg && (instruction.nia == instruction.predicted_nia))
				{
				  // Do a retire on the return address stack
				  RAS.Retire();
				}
			      */
			      // If NIA != PREDICTED_NIA ...
			      if (instruction->nia != instruction->predicted_nia)
				{
				  /* It is a Mispredicted branch */
				  /* Do a rollback on the return address stack */
				  RAS.Rollback();

				  // If it's a conditional branch then Update BHT...
				  if ( instruction->operation->branch_conditioned() )
				    {
				      /* Update the branch history table */
				      bht_misses++;
				      BHT.Update(instruction->cia, instruction->branch_direction);
				    }
				  
				  // If it's a indirect branch (Link or Count)...
				  if ( (instruction->fn & FnBranchCountReg) )//|| (instruction.fn & FnBranchLinkReg) )
				    {
				      // If it's conditional and taken or unconditional then Update BTB
				      if ( !instruction->operation->branch_conditioned() &&
					     instruction->btb_miss
					   )
					{
					  /* Update the branch target buffer */
					  btb_misses++;
					  BTB.Update(instruction->cia, instruction->nia);
					  btb_miss = false;
					  cia = seq_cia = nia = instruction->nia;
					}
				      
				      // If it's conditional and taken or unconditional then Update BTB
				      if ( instruction->operation->branch_conditioned() &&
					   instruction->branch_direction == Taken 
					   )
					{
					  /* Update the branch target buffer */
					  // If we predict an incorrect address it's a btb_miss too...
					  // Thus increment btb_miss counter !!!
					  btb_misses++;
					  BTB.Update(instruction->cia, instruction->nia);
					  cia = seq_cia = nia = instruction->nia;
					}

				    }
				  nia = instruction->nia;/* nia is used by flush to redirect fetch on the correct path */
				} // End of if NIA != PREDICTED_NIA 
			      else
				{
				  
				}
				
			      /*
			      if( ( instruction.operation->branch_conditioned() &&
				    ( (instruction.nia == instruction.predicted_nia) &&
				      ( (instruction.fn == FnBranchLinkReg) ||
					(instruction.operation->branch_linked() )
				      )
				    ) 
				  ) ||
				  ( !instruction.operation->branch_conditioned() &&
				    ( ( instruction.fn == FnBranchLinkReg) ||
				      ( instruction.operation->branch_linked() )
				    )
				  )
				)
			      */
				/*
				  if( ( (instruction.nia == instruction.predicted_nia) &&
				  ( (instruction.fn == FnBranchLinkReg) ||
				  (instruction.operation->branch_linked() )
				  )
				  )
				  )
				*/
			      if ( 
				  ( ( instruction->operation->branch_conditioned() &&
				      (instruction->nia == instruction->predicted_nia) &&
				      (instruction->branch_direction == Taken)
				      ) ||
				    ( !instruction->operation->branch_conditioned() &&
				      (instruction->nia == instruction->predicted_nia)
				      )
				     
				    ) &&
				  ( (instruction->fn == FnBranchLinkReg) ||
				    (instruction->operation->branch_linked() )
				    )
				  )
				{ //// if instruction is conditioned and well predicted
				  //// or instruction is not conditioned 
				  // If branch is a call or a return...
				  // then if is conditioned, taken and well predicted
				  // OR if is not conditioned and well predicted taken of course... 
				  RAS.Retire();
				}
			      
			      if( ( (instruction->nia != instruction->predicted_nia) &&
				    ( (instruction->fn == FnBranchLinkReg)
				    )
				  )
				)
				{ // if instruction is conditioned and well predicted
				  // or instruction is not conditioned 
				  //				  RAS.Retire();
				  // RAS.Pop();
				  address_t addr;
				  RAS.Pop(addr);
				  ras_misses++;
				}
			      
			      if( ( (instruction->nia != instruction->predicted_nia) &&
				    ( (instruction->operation->branch_linked()) 
				    )
				  )
				)
				{ // if instruction is conditioned and well predicted
				  // or instruction is not conditioned 
				  //				  RAS.Retire();
				  RAS.Push(instruction->cia + InstructionSize, instruction);
				}
			      
			      // In all case : if it's a "return" branch... pop RAS
			      /*
			      if(instruction.fn == FnBranchLinkReg)
				{
				  // Do a pop on the return address stack
				  address_t addr;
				  RAS.Pop(addr);
				  ras_misses++;
				}
			      */
			      // In all case : if it's a "call" branch... push RAS
			      /*
			      if (instruction.operation->branch_linked())
				{
				  //if(instruction.destination.reg >= 0)
				  // DD This is not exactly the same...
				  if(instruction.destinations.size() > 0)
				    {
				      // Do a push on the return address stack
				      //RAS.Push(instruction.cia + InstructionSize, &instruction);
				      RAS.Push(instruction.cia + InstructionSize, instruction);
				    }
				}
			      */
			      /*
			      if ( instruction.ras_miss )
				{
				      // Do a pop on the return address stack
				      address_t addr;
				      RAS.Pop(addr);
				      ras_miss = false;
				      //nia = instruction.nia;
				      cia = seq_cia = nia = instruction.nia;
				}
			      */
			      in_flight_branches--;

			    }//end of If ("It's a branch")
#endif // #ifndef USE_PERFECT_FETCH
			}//end of else
		} // end of for(...)
		//	}// end of end_of_cycle


	///////////////////////////////////////// USE_PERFECT_FETCH
#ifdef USE_PERFECT_FETCH

	bool previous_splitting=false;

	while ( 
		(instructionQueue.Size()<(InstructionQueueSize/2)-1 ) 
//		||
//		previous_splitting
	)
	{

   #ifdef DD_DEBUG_FILLING
		cerr << "DDDFILL: CIA Before filling = " << hex << cia << dec << endl;
   #endif
#ifdef DD_DEBUG_FILLING
	cerr << "DDDEBUG BF:  ---  InstructionQueueSize         = " << InstructionQueueSize << endl;
	cerr << "DDDEBUG BF:  ---  InstructionQueue.Size()      = " << instructionQueue.Size() << endl;
	cerr << "DDDEBUG BF:  ---  InstructionQueue.FreeSpace() = " << instructionQueue.FreeSpace() << endl;
#endif
		fill_instruction_queue(cia);

#ifdef DD_DEBUG_FILLING
	cerr << "DDDEBUG AF:  ---  InstructionQueueSize         = " << InstructionQueueSize << endl;
	cerr << "DDDEBUG AF:  ---  InstructionQueue.Size()      = " << instructionQueue.Size() << endl;
	cerr << "DDDEBUG AF:  ---  InstructionQueue.FreeSpace() = " << instructionQueue.FreeSpace() << endl;
#endif

		QueuePointer<InstructionQueueEntry, InstructionQueueSize > entry;

		/* Branch Prediction */
		/* For each instruction queue entry */
		/*
		if (instructionQueue.Size() == 0) 
		  { 
		    //		    cerr << " ----->    BUG : \t qsize   = 0 ; ts = " << timestamp() << endl;
		    //		exit(-1);
		  }
		*/
		//		for(entry = instructionQueue.SeekAtHead(); entry && !syscall_in_pipeline; entry++)
		entry = instructionQueue.SeekAtHead();// entry && !syscall_in_pipeline; entry++)
		while (entry && !syscall_in_pipeline)
		{
   #ifdef DD_DEBUG_FILLING
		cerr << "DDDFILL: While Entry CIA: " << hex << cia << dec << endl;
		cerr << "DDDFILL: While Entry INS: " << entry->instruction << endl;
   #endif
			previous_splitting = false;
		  //		  cerr << " ----->    BUG : \t QueueSize = " << instructionQueue.Size() << endl;
			/* Was instruction predecoded ? */
			if(!entry->predecoded)
			{
			  fetched_instructions++;
			  //				changed = true;
			  /* Decode the instruction */
			  Decode(&(entry->instruction));
				
#ifdef DD_DEBUG_SPLITTING
				cerr << "---- ---- ---- Decoded Inst : " << (entry->instruction) << endl;
#endif				
				if (entry->instruction->operation->is_splitted())
				  {
				    previous_splitting = true;
				    splitted_instructions++;
				    if (!(instructionQueue.FreeSpace()>0))
				    //  if (!instructionQueue.Full())
				      {
					cerr << "INSTRUCTION SPLITTING: No FreeSpace!!!!!!!!!!!" << endl;
					cerr << "Inst : " << (entry->instruction) << endl;
					cerr << " FreeSpace = " << instructionQueue.FreeSpace() << endl;
					exit(-1);

				      }
				    else
				      {
#ifdef DD_DEBUG_SPLITTING
					cerr << "Splitted instruction: "<< (entry->instruction) << endl;
					cerr << "INSTRUCTION WILL BE SPLITTED !!!!!!!!!!!" << endl;
					//	cerr << "Inst : " << *(entry->instruction) << endl;
					cerr << instructionQueue << endl;
#endif
					QueuePointer<InstructionQueueEntry, InstructionQueueSize > new_entry;
					QueuePointer<InstructionQueueEntry, InstructionQueueSize > old_entry;
					//InstructionQueueEntry *new_entry; 
					//InstructionQueueEntry *old_entry; 
					//new_entry = instructionQueue.New();
					instructionQueue.New();
					new_entry = QueuePointer<InstructionQueueEntry,InstructionQueueSize>(&instructionQueue,instructionQueue.GetIndex(0));
					//	new_entry = new InstructionQueueEntry();
					if (!new_entry)
					  {
					    cerr << "New entry not allocated !!!!!!!!!!!" << endl;
					    cerr << "Inst : " << (entry->instruction) << endl;
					    exit(-1);
					  }
					/*
					InstructionPtr new_instruction;
					new_entry->instruction = new_instruction;
					*/
					old_entry = new_entry;
					while ( ! (&(*new_entry) == &(*entry)) )
					  {
					    //old_entry = (old_entry + InstructionQueueSize-1) % InstructionQueueSize;
					    old_entry--;
					    
					    //*(new_entry->instruction) = *(old_entry->instruction);
					    (new_entry->instruction) = (old_entry->instruction);
					    new_entry->predecoded = old_entry->predecoded;
					    new_entry->executed_in_fetch = old_entry->executed_in_fetch;
					    //new_entry = (new_entry + InstructionQueueSize-1) % InstructionQueueSize;
					    new_entry--;
					  };
#ifdef DD_DEBUG_SPLITTING
					cerr << "INSTRUCTION WILL BE SPLITTED 2 !!!!!!!!!!!" << endl;
					//	cerr << "Inst : " << *(entry->instruction) << endl;
					cerr << instructionQueue << endl;
#endif
					//					inum;
					//new_entry = entry;
					//InstructionPtr *newsplitted_instruction = new InstructionPtr();
					InstructionPtr newsplitted_instruction = entry->instruction;
					//InstructionPtr newsplitted = entry->instruction;

					list <Operation<CPU_CONFIG> *> lops;
					lops = new_entry->instruction->operation->split_into();
					QueuePointer<InstructionQueueEntry, InstructionQueueSize > previous_entry;
					previous_entry=new_entry;
					while (lops.size()>0)
					  {
					    Operation<CPU_CONFIG> *ops = lops.front();
#ifdef DD_DEBUG_SPLITTING
					    cerr << "LOPS[...] : ";
					    ops->disasm(NULL,cerr);
					    cerr << "--- CodeType: "<< hex << ops->GetEncoding() << dec;
					    cerr << endl;
#endif
					    // VALGRIND TODO...
					    InstructionPtr *newInst = new InstructionPtr();// Ensure a new instruction is created
					    new_entry->instruction = *newInst;
					    // VALGRIND BUGFIXE
					    delete newInst;

					    DecodeSplitted(&(new_entry->instruction), &newsplitted_instruction, ops);
					    //    new_entry->instruction->inum = inum++;
					    new_entry->predecoded = true;
					    // FREE ops...
					    delete ops;
					    ops = NULL;

					    lops.pop_front();
					    previous_entry=new_entry;
					    new_entry++;
#ifdef DD_DEBUG_SPLITTING
					    cerr << "INSTRUCTION WILL BE SPLITTED 3 !!!!!!!!!!!" << endl;
					    //	cerr << "Inst : " << *(entry->instruction) << endl;
					    cerr << instructionQueue << endl;
#endif
					  }
					//delete newsplitted_instruction;
					//new_entry--;
					previous_entry->instruction->mustbechecked = true;
					//entry--;
					//DecodeSplitted(entry->instruction, lops.pop_front());
#ifdef DD_DEBUG_SPLITTING
					cerr << "INSTRUCTION HAS BEEN SPLITTED !!!!!!!!!!!" << endl;
					//	cerr << "Inst : " << *(entry->instruction) << endl;
					cerr << instructionQueue << endl;
#endif
				      }
				    /*
				    cerr << "SPLIT THE INSTRUCTION !!!!!!!!!!!" << endl;
				    cerr << "Inst : " << *(entry->instruction) << endl;
				    exit(-1);
				    */

				    //entry--;

				  }

				// If decoded instruction is a branch
				if( entry->instruction->operation->function
				    &
				    (FnBranch | FnConditionalBranch | FnBranchCountReg | FnBranchLinkReg)
				    )
				{
					if(in_flight_branches < MaxBranches)
					  {
					    in_flight_branches++;
					  }
					else	
					  {
#ifdef DD_DEBUG_FETCH_PREDECODE
  if (DD_DEBUG_TIMESTAMP < timestamp())
    {
      cerr << "["<<this->name()<<"("<<timestamp()<<")] MAXIMUM INFLIGHT BRANCHES !!! " << instructionQueue << endl;
      cerr << *(entry->instruction) << endl;
    }
#endif 
                                             break;
					  }
				}
				entry->predecoded = true;

			} // if (predecoded)
/*			if ()
			{
				previous_splitting = true;
			}
*/
			//////////////////////////////////////////////////////////////////////////////
#ifndef USE_PERFECT_FETCH
				/* If instruction is branch */
				//				if(entry->instruction->fn & FnBranch)
				if( entry->instruction->operation->function
				    &
				    (FnBranch | FnConditionalBranch | FnBranchCountReg | FnBranchLinkReg)
				    )
				{
#ifdef DD_DEBUG_FETCH_BRANCH
				  cerr << "[F. Branch] Instruction is a Branch : " << endl;
				  cerr << *(entry->instruction) << endl;
#endif 
					/* If branch is conditional */
					//if(entry->instruction->fn & FnConditionalBranch)
					if(entry->instruction->operation->branch_conditioned())
					{
						/* Get the branch direction from the branch history table */
						entry->instruction->predicted_branch_direction = BHT.Read(entry->instruction->cia);
// 						entry->instruction->obq_tag = BHT.AllocateBranch();
						bht_accesses++;
					}
					else
					{
						/* If branch is unconditional */
						//DD//if(entry->instruction->fn & FnUnconditionalBranch)
						//if( (entry->instruction->fn & FnBranch)
					  if ( ( entry->instruction->operation->function
						 &
						 (FnBranch | FnConditionalBranch | FnBranchCountReg | FnBranchLinkReg)
						 )
					       //&& !(entry->instruction->operation->function & FnConditionalBranch) )
					       && ! (entry->instruction->operation->branch_conditioned() )
					       )
					    {
					      entry->instruction->predicted_branch_direction = Taken;
					    }
					  else
					    {
					      /* Throw an error if branch is neither conditional nor conditional */
					      cerr << "branch instruction (" << entry->instruction << ") is neither unconditional nor conditional" << endl; cerr.flush();
					      exit(-1);
					    }
					}
#ifdef DD_DEBUG_FETCH_BRANCH
					cerr << "[F. Branch] Predicted direction : " << endl;
					switch(entry->instruction->predicted_branch_direction)
					  {
					  case Taken:
					    cerr << "Taken";
					    break;
					  case NotTaken:
					    cerr << "Not Taken";
					    break;
					  case DontKnow:
					    cerr << "Dot Know !!!";
					    break;
					  default:
					    cerr << " Unknown predicted direction" << endl; abort();
					  }
					cerr << endl;
#endif


					/* If branch is direct */
					//DD//if(entry->instruction->fn & FnDirectBranch)
					//					if( (entry->instruction->fn & FnBranch)
					if ( ( entry->instruction->operation->function
					       &
					       (FnBranch | FnConditionalBranch | FnBranchCountReg | FnBranchLinkReg)
					       )
					     && !(entry->instruction->operation->function == FnBranchCountReg)
					     && !(entry->instruction->operation->function == FnBranchLinkReg)
					     )
					  {
						/* Get the target address from the emulator */
						//transient_emul_state.cia = entry->instruction->cia;
						//transient_emul_state.nia = transient_emul_state.cia + InstructionSize;
						/* Call the emulator */
					  emulator->SetCIA(entry->instruction->cia);
					  emulator->SetNIA(entry->instruction->cia+InstructionSize);
					  entry->instruction->operation->execute(emulator);//, &transient_emul_state, NULL /* no memory */);
					  
						//entry->instruction->operation->execute(entry->instruction->operation, &transient_emul_state, NULL /* no memory */);
					  if(entry->instruction->predicted_branch_direction == Taken) //;
						  //entry->instruction->predicted_nia = transient_emul_state.target_address;
					    entry->instruction->predicted_nia = entry->instruction->operation->get_target_address(entry->instruction->cia,emulator);
					  
#ifdef DD_DEBUG_FETCH_BRANCH
				  cerr << "[F. Branch] Instruction is a Direct Branch : " << endl;
				  cerr << *(entry->instruction) << endl;
#endif 
					}
					else
					{
					  // DD: BE CARFULL: RAS.SafePop will change predicted_nia !!!
					  // We have to choose if we really want to use a RAS !!!
//////////////////////// BEGINING of  RAS PROBLEM !!! //////////////////////////////////////////
					/* If branch is a return branch */
					//DD//if(entry->instruction->operation->function & FnReturnBranch)
					// DDDD: no more used of RAS...
					
					if(entry->instruction->operation->function == FnBranchLinkReg)
					{
						// If branch is (predicted) taken
						if( entry->instruction->operation->branch_conditioned() &&
						    entry->instruction->predicted_branch_direction == Taken)
						{
							// Taken
							// Get the target address from the Return Address Stack
							ras_accesses++;
							if(!RAS.SafePop(entry->instruction->predicted_nia))
							{
								entry->instruction->nia = 0;
								entry->instruction->ras_miss = true;
								ras_misses++;
							}
						}
					      else if ( !entry->instruction->operation->branch_conditioned() )
						{
							// Taken
							// Get the target address from the Return Address Stack
							ras_accesses++;
							if(!RAS.SafePop(entry->instruction->predicted_nia))
							{
								entry->instruction->nia = 0;
								entry->instruction->ras_miss = true;
								ras_misses++;
							}
						  
						}
#ifdef DD_DEBUG_FETCH_BRANCH
						else
						  {
						    cerr << "WARNING: Branch to LR predicted NotTaken..." << endl;
						  }
#endif
					}
//////////////////////// END of  RAS PROBLEM !!! //////////////////////////////////////////

					  /* If branch is indirect */
					  //if(entry->instruction->fn & FnIndirectBranch)
					  if( (entry->instruction->operation->function & FnBranchCountReg)
					      //|| (entry->instruction->operation->function & FnBranchLinkReg)
					      )
					    {
					      /* If branch is (predicted) taken */
					      //if(entry->instruction->predicted_branch_direction == Taken)
					      if( entry->instruction->operation->branch_conditioned() &&
						  entry->instruction->predicted_branch_direction == Taken )
						{
						  /* Taken */
						  /* Get the target address from the Branch Target Buffer */
						  btb_accesses++;
						  if(!BTB.Read(entry->instruction->cia, entry->instruction->predicted_nia))
						    {
						      btb_misses++;
						      entry->instruction->predicted_nia = 0;
						      entry->instruction->btb_miss = true;
						    }
						}
					      // TODO Else ???!!!
					      else if ( !entry->instruction->operation->branch_conditioned() )
						{
						  /* Taken */
						  /* Get the target address from the Branch Target Buffer */
						  btb_accesses++;
						  if(!BTB.Read(entry->instruction->cia, entry->instruction->predicted_nia))
						    {
						      btb_misses++;
						      entry->instruction->predicted_nia = 0;
						      entry->instruction->btb_miss = true;
						    }
						}
					    }
#ifdef DD_DEBUG_FETCH_BRANCH
					  cerr << "[F. Branch] Instruction is an INDIRECT Branch : " << endl;
					  cerr << *(entry->instruction) << endl;
					  cerr << "intruction->btb_miss="<< entry->instruction->btb_miss << endl;
#endif
					}

					/// DDD /// C'etait la !!!
					/* If branch is a call branch */
					//DD//if(entry->instruction->operation->function & FnCallBranch)
					if (entry->instruction->operation->branch_linked())
					{
						/* If branch is (predicted) taken */
						if(entry->instruction->predicted_branch_direction == Taken)
						{
							/* Taken */
							//if(entry->instruction->destination.reg >= 0)
							if(entry->instruction->destinations.size() > 0)
							{
								/* Push on the return address stack the return address */
							  //								RAS.SafePush(entry->instruction->cia + InstructionSize, entry->instruction);
								RAS.SafePush(entry->instruction->cia + InstructionSize, entry->instruction);
							}
						}
					}

					/* If branch is taken */
					if(entry->instruction->predicted_branch_direction == Taken)
					{
						/* Taken */
						/* Flush the instruction after the branch */
						instructionQueue.FlushAfter(entry);

						/* Correct the path */
						cia = entry->instruction->predicted_nia;
						seq_cia = cia;
						//if(waiting_instr_cache_accept) pending_instr_cache_requests--;
						//waiting_instr_cache_accept = false;
						ignore_instr_cache_responses = pending_instr_cache_requests;
						pending_instr_cache_access_size = 0;

#ifdef DD_DEBUG_FETCH_VERB2
						cerr << "[FETCH] ==== Correct fetcher on Speculative path ======== ["<< timestamp()<<"]"<< endl;
						cerr << *this << endl;
#endif
					      //if((entry->instruction->operation->function & FnIndirectBranch) && entry->instruction->btb_miss)
					      if( (
						   (entry->instruction->operation->function & FnBranchCountReg)
						   || (entry->instruction->operation->function & FnBranchLinkReg)
						   )  
						  && entry->instruction->btb_miss
						  )
						{
#ifdef DD_DEBUG_FETCH_BRANCH
					  cerr << "[F. Branch] Instruction is an INDIRECT Branch : " << endl;
					  cerr << *(entry->instruction) << endl;
					  cerr << "intruction->btb_miss="<< entry->instruction->btb_miss << endl;
					  cerr << " WE ARE SETTING BTB_MISS to TRUE !!!" << endl;
#endif						  
					  
							btb_miss = true;
						}
					   //DD//if((entry->instruction->operation->function & FnReturnBranch) && entry->instruction->ras_miss)
						if((entry->instruction->operation->function & FnBranchLinkReg)
						   && entry->instruction->ras_miss)
						{
							ras_miss = true;
						}
					}
					else
					{
						if(entry->instruction->predicted_branch_direction == NotTaken)
						{
#ifdef DD_DEBUG_FETCH_VERB2
						  if (DD_DEBUG_TIMESTAMP < timestamp())
						    {
						  cerr << "[FETCH] ==== Do not Correct fetcher ... ======== ["<< timestamp()<<"]"<< endl;
						  cerr << *this << endl;
						    }
#endif
							/* Not taken */
							/* The target address is the address of following instruction */
							entry->instruction->predicted_nia = entry->instruction->cia + InstructionSize;
							/*
						seq_cia = cia;
						if(waiting_instr_cache_accept) pending_instr_cache_requests--;
						waiting_instr_cache_accept = false;
						ignore_instr_cache_responses = pending_instr_cache_requests;
						pending_instr_cache_access_size = 0;
							*/
							//cia = entry->instruction->predicted_nia;
						}
						else
						{
							/* Throw an error if branch is neither predicted taken nor not taken */
							cerr << "branch instruction (" << entry->instruction << ") is neither predicted taken nor not taken" << endl; cerr.flush();
							exit(-1);
						}
					}
				} // if instruction is a branch
#endif // #ifndef USE_PERFECT_FETCH
				//#endif // USE_PERFECT_BRANCH_PREDICTION
#ifdef DDDEBUG_USE_PERFECT_BRANCH_PREDICTION
		/*
				bool previous_was_splitted = false;	
				if (entry->instruction->operation->is_splitted())
				  {
				    entry++;
				    previous_was_splitted=true;
				  }
		*/
				if (!entry->executed_in_fetch)
				  {
				emulator->SetCIA(cia);
				emulator->SetNIA(cia+InstructionSize);
#ifdef DD_DEBUG_PERFECT_BRANCH
				cerr << "DD_DEBUG_PERFECT: Emulator->CIA() before="<< hex << emulator->GetCIA() << dec << endl;
				cerr << "DD_DEBUG_PERFECT: Emulator->NIA() before="<< hex << emulator->GetNIA() << dec << endl;
#endif
				try{
				  entry->instruction->operation->execute(emulator);
				}
				catch(Exception &e)
				  {
				    // Here we catch some trap exceptions ???
				  }
#ifdef DD_DEBUG_PERFECT_BRANCH
				//				cerr << "Entry: " << entry << endl; 
				cerr << entry->instruction << endl;
				cerr << "DD_DEBUG_PERFECT: Emulator->CIA() after="<< hex << emulator->GetCIA() << dec << endl;
				cerr << "DD_DEBUG_PERFECT: Emulator->NIA() after="<< hex << emulator->GetNIA() << dec << endl;
#endif
				

//				if (!previous_was_splitted)
//				if (!entry->instruction->operation->is_splitted())
				if (!previous_splitting)
				{ 
				   cia += InstructionSize; 
				}

				//				emulator->StepOneInstruction();
				//entry->instruction->predicted_nia = entry->instruction->cia + InstructionSize;
   #ifdef DD_DEBUG_PERFECT_BRANCH
				cerr << "DD_DEBUG_PERFECT: Inst=" << entry->instruction << endl;
/*				for (int i=0; i<unisim::component::clm::processor::ooosim::nIntegerArchitecturalRegisters; i++)
				{
					cerr << "DD_DEBUG_PERFECT: GPR["<<i<<"]="<< hex << emulator->GetGPR(i) << dec << endl;
				}
*/				cerr << "DD_DEBUG_PERFECT: CR="<< hex << emulator->GetCR() << dec << endl;
   #endif
#ifdef DD_DEBUG_CR0
				cerr << "DD_DEBUG_CR0: Instruction: "<< entry->instruction << endl;
				cerr << "DD_DEBUG_CR0: CR="<< hex << emulator->GetCR() << dec << endl;
				cerr << "DD_DEBUG_CR0: CRF[0]="<< hex << emulator->GetCRF(0) << dec << endl;
				cerr << "DD_DEBUG_CR0: GPR[0]="<< hex << emulator->GetGPR(0) << dec << endl;
				cerr << "DD_DEBUG_CR0: XER[0]="<< hex << emulator->GetXER() << dec << endl;
#endif
				if( entry->instruction->operation->function
				    &
				    (FnBranch | FnConditionalBranch | FnBranchCountReg | FnBranchLinkReg)
				    )
				{
				  /* Correct the path */
				  // cia = entry->instruction->nia;
				  cia = emulator->GetNIA();
   #ifdef DD_DEBUG_FILLING
		cerr << "DDDFILL: cia1 = " << hex << cia << dec << endl;
   #endif
				  //entry->instruction->predicted_nia = entry->instruction->operation->get_target_address(entry->instruction->cia,emulator);
				  //cia = entry->instruction->operation->get_target_address(entry->instruction->cia,emulator);

   #ifdef DD_DEBUG_FILLING
		cerr << "DDDFILL: cia2 = " << hex << cia << dec << endl;
   #endif
				  //nia = emulator->GetNIA();
				  seq_cia = cia;
				  entry->instruction->predicted_nia = entry->instruction->cia + InstructionSize;
				  if (entry->instruction->predicted_nia != cia)
				    {
   #ifdef DD_DEBUG_PERFECT_BRANCH
				      cerr << "DD_DEBUG_PERFECT: Instruction before="<< hex << entry->instruction << dec << endl;
   #endif
				      entry->instruction->nia = cia;
				      entry->instruction->predicted_nia = cia;
				      instructionQueue.FlushAfter(entry);
   #ifdef DD_DEBUG_PERFECT_BRANCH
				      cerr << "DD_DEBUG_PERFECT: Instruction after ="<< hex << entry->instruction << dec << endl;
   #endif

				    }
				}
#endif // USE_PERFECT_BRANCH_PREDICTION

				/* If instruction is a SysCall */
			        if( entry->instruction->operation->function == FnSysCall )
				  { 
				    syscall_in_pipeline = true;
				    /* Flush the instruction after the branch */
				    instructionQueue.FlushAfter(entry);

				    /* Correct the path */
				    cia = entry->instruction->nia;
				    seq_cia = cia;
				  }

				entry->executed_in_fetch = true;
				//			}// end of if (!predecoded) ...
			}// end of if (!executed) ...

			//cerr << " INFO at EO-EOF : cia = " << hexa(entry->instruction->cia) << " \t nia = " << hexa(entry->instruction->nia) << " \t predicted_nia = " << hexa(entry->instruction->predicted_nia) << endl;
			//			if (!entry->instruction->operation->is_splitted())
			//			  {
			entry++;
			//			  }
		} // end while(entry && !syscall_in)



		
	} // end of while (InstructionQueue.Size() < IQS/2-1)
	///////////////////////////////////////// USE_PERFECT_FETCH ///////////////// ELSE !!!
#else		       	
		/* Fill in the instruction queue */
		/* Do we have an enable from the instruction cache ? */
		//		if(inL1Enable && pending_instr_cache_requests > 0)
		if(inIL1.enable)
		{
		  if ( !(pending_instr_cache_requests > 0) )
			  {
			    cerr << " [DD BUG] Pending instruction cache request must be >0 !!!!" << endl;
			    exit(-1);
			  }
			/* Get the size of the response */
			//			int accessSize = inL1Size;
			//		        int accessSize = inIL1.data.size;

			memreq < InstructionPtr, nCPUDataPathSize > tmpmemreq = inIL1.data;
			int accessSize = tmpmemreq.size;

			pending_instr_cache_requests--;
			//changed = true;

#ifdef DD_DEBUG_FETCH_VERB2
			if (DD_DEBUG_TIMESTAMP < timestamp())
			  {
			cerr << "[FETCH] =Fill=== We have recevied Data from IL1 (EOF) ======= ["<< timestamp()<<"]"<< endl;
			  }
#endif
			if(ignore_instr_cache_responses > 0)
			{
			  ignore_instr_cache_responses--;
#ifdef DD_DEBUG_FETCH_VERB2
			  if (DD_DEBUG_TIMESTAMP < timestamp())
			    {
			  cerr << "[FETCH] =Fill=== We are ignoring Data from IL1 (EOF) ======= ["<< timestamp()<<"]"<< endl;
			    }
#endif
			}
			else
			{
				pending_instr_cache_access_size -= accessSize;
				int n = accessSize / InstructionSize;
				int offset;

				/* Get the data response of the instruction cache */
#ifdef DD_DEBUG_FETCH_VERB2
				if (DD_DEBUG_TIMESTAMP < timestamp())
				  {
				cerr << "[FETCH] =Fill=== We are getting Data from IL1 (EOF) ======= ["<< timestamp()<<"]"<< endl;
				  }
#endif
				//ByteArray<nCPUDataPathSize> vector = inL1Data;
				//				ByteArray<nCPUDataPathSize> vector = inIL1.data.Data;
				ByteArray<nCPUDataPathSize> vector = tmpmemreq.data;

				for(i = 0, offset = 0; i < n; i++, cia += InstructionSize, offset += InstructionSize)
				{
					/* Allocate an instruction queue entry */
					//DD//InstructionQueueEntry<T, nSources> *entry = instructionQueue.New();
				  InstructionQueueEntry *entry = instructionQueue.New();
					//QueuePointer<InstructionQueueEntry, InstructionQueueSize > entry;
				  //					entry = instructionQueue.New();

					if(!entry)
					{
						/* Throw an error if instruction queue is full */
						cerr << "time stamp = " << timestamp() << endl;
						cerr << "warning: instruction queue overflow" << endl;
						exit(-1);
					}

					//	InstructionPtr instruction;
					// VALGRIND TODO
					InstructionPtr *new_instruction = new InstructionPtr();
					entry->instruction = *new_instruction;
					// VALGRIND MEMORY LEAK BUGFIXE
					delete new_instruction;
					//					InstructionPtr<T, nSources> instruction;	// this ensure that a new instruction is created
					//Instruction *instruction;	// this ensure that a new instruction is created

					//	instruction->Reset();
					/* Fill in the instruction queue entry */
					// MEMCHECK : memory leak ... we are now doing everything into InstructionQueueEntry ...
					/*
					if (entry->instruction == NULL)
					  entry->instruction = new Instruction();
					*/
					//cerr << " ----->    BUG : \t new cia   = " << cia << "\t ts=" << timestamp() << endl;
					
					//entry->instruction->cia = cia;
					/*
					if (emulator)
					  {
					    cerr << " ----->    BUG : \t EMU cia   = " << emulator->GetCIA() << "\t ts=" << timestamp() << endl;
					  }
					else 
					  cerr << " BUG no emulator !!!" << endl;
					*/
					entry->instruction->cia = cia;//emulator->GetCIA();
					entry->instruction->operation = 0;
					entry->instruction->inum = inum++;
					entry->predecoded = false;
					vector.Read(entry->instruction->binary, offset, endianess);
					// For dump
					entry->instruction->timing_fetch_cycle = timestamp();

#ifdef DD_DEBUG_FETCH_VERB2
 if (DD_DEBUG_TIMESTAMP < timestamp())
			    {
					cerr << "[FETCH] ==== We are Storing Data == ("<< entry->instruction <<") ======== ["<< timestamp()<<"]"<< endl;

			    }
#endif


#ifdef TRACE
					trace_file->Begin("event");
					trace_file->Value("name", "fetch");
					trace_file->Begin("instruction");
					trace_file->Value("number", entry->instruction->inum);
					trace_file->Value("pc", entry->instruction->cia);
					trace_file->End("instruction");
					trace_file->End("event");
#endif
				}
			}
		}// end of inIL1.enable
		//#endif
		//DD//QueuePointer<InstructionQueueEntry<T, nSources>, InstructionQueueSize > entry;
		QueuePointer<InstructionQueueEntry, InstructionQueueSize > entry;

		/* Branch Prediction */
		/* For each instruction queue entry */
		/*
		if (instructionQueue.Size() == 0) 
		  { 
		    //		    cerr << " ----->    BUG : \t qsize   = 0 ; ts = " << timestamp() << endl;
		    //		exit(-1);
		  }
		*/
		//		for(entry = instructionQueue.SeekAtHead(); entry && !syscall_in_pipeline; entry++)
		entry = instructionQueue.SeekAtHead();// entry && !syscall_in_pipeline; entry++)
		while (entry && !syscall_in_pipeline)
		{
		  //		  cerr << " ----->    BUG : \t QueueSize = " << instructionQueue.Size() << endl;
			/* Was instruction predecoded ? */
			if(!entry->predecoded)
			{
			  fetched_instructions++;
			  //				changed = true;
			  /* Decode the instruction */
			  Decode(&(entry->instruction));
				
#ifdef DD_DEBUG_SPLITTING
				cerr << "---- ---- ---- Decoded Inst : " << (entry->instruction) << endl;
#endif				
				if (entry->instruction->operation->is_splitted())
				  {
				    splitted_instructions++;
				    if (!(instructionQueue.FreeSpace()>0))
				    //  if (!instructionQueue.Full())
				      {
					cerr << "INSTRUCTION SPLITTING: No FreeSpace!!!!!!!!!!!" << endl;
					cerr << "Inst : " << (entry->instruction) << endl;
					cerr << " FreeSpace = " << instructionQueue.FreeSpace() << endl;
					exit(-1);

				      }
				    else
				      {
#ifdef DD_DEBUG_SPLITTING
					cerr << "Splitted instruction: "<< (entry->instruction) << endl;
					cerr << "INSTRUCTION WILL BE SPLITTED !!!!!!!!!!!" << endl;
					//	cerr << "Inst : " << *(entry->instruction) << endl;
					cerr << instructionQueue << endl;
#endif
					QueuePointer<InstructionQueueEntry, InstructionQueueSize > new_entry;
					QueuePointer<InstructionQueueEntry, InstructionQueueSize > old_entry;
					//InstructionQueueEntry *new_entry; 
					//InstructionQueueEntry *old_entry; 
					//new_entry = instructionQueue.New();
					instructionQueue.New();
					new_entry = QueuePointer<InstructionQueueEntry,InstructionQueueSize>(&instructionQueue,instructionQueue.GetIndex(0));
					//	new_entry = new InstructionQueueEntry();
					if (!new_entry)
					  {
					    cerr << "New entry not allocated !!!!!!!!!!!" << endl;
					    cerr << "Inst : " << (entry->instruction) << endl;
					    exit(-1);
					  }
					/*
					InstructionPtr new_instruction;
					new_entry->instruction = new_instruction;
					*/
					old_entry = new_entry;
					while ( ! (&(*new_entry) == &(*entry)) )
					  {
					    //old_entry = (old_entry + InstructionQueueSize-1) % InstructionQueueSize;
					    old_entry--;
					    
					    //*(new_entry->instruction) = *(old_entry->instruction);
					    (new_entry->instruction) = (old_entry->instruction);
					    new_entry->predecoded = old_entry->predecoded;
					    //new_entry = (new_entry + InstructionQueueSize-1) % InstructionQueueSize;
					    new_entry--;
					  };
#ifdef DD_DEBUG_SPLITTING
					cerr << "INSTRUCTION WILL BE SPLITTED 2 !!!!!!!!!!!" << endl;
					//	cerr << "Inst : " << *(entry->instruction) << endl;
					cerr << instructionQueue << endl;
#endif
					//					inum;
					//new_entry = entry;
					//InstructionPtr *newsplitted_instruction = new InstructionPtr();
					InstructionPtr newsplitted_instruction = entry->instruction;
					//InstructionPtr newsplitted = entry->instruction;

					list <Operation<CPU_CONFIG> *> lops;
					lops = new_entry->instruction->operation->split_into();
					QueuePointer<InstructionQueueEntry, InstructionQueueSize > previous_entry;
					previous_entry=new_entry;
					while (lops.size()>0)
					  {
					    Operation<CPU_CONFIG> *ops = lops.front();
#ifdef DD_DEBUG_SPLITTING
					    cerr << "LOPS[...] : ";
					    ops->disasm(cerr);
					    cerr << "--- CodeType: "<< hex << ops->GetEncoding() << dec;
					    cerr << endl;
#endif
					    // VALGRIND TODO...
					    InstructionPtr *newInst = new InstructionPtr();// Ensure a new instruction is created
					    new_entry->instruction = *newInst;
					    // VALGRIND BUGFIXE
					    delete newInst;

					    DecodeSplitted(&(new_entry->instruction), &newsplitted_instruction, ops);
					    //    new_entry->instruction->inum = inum++;
					    new_entry->predecoded = true;
					    // FREE ops...
					    delete ops;
					    ops = NULL;

					    lops.pop_front();
					    previous_entry=new_entry;
					    new_entry++;
#ifdef DD_DEBUG_SPLITTING
					    cerr << "INSTRUCTION WILL BE SPLITTED 3 !!!!!!!!!!!" << endl;
					    //	cerr << "Inst : " << *(entry->instruction) << endl;
					    cerr << instructionQueue << endl;
#endif
					  }
					//delete newsplitted_instruction;
					//new_entry--;
					previous_entry->instruction->mustbechecked = true;
					//entry--;
					//DecodeSplitted(entry->instruction, lops.pop_front());
#ifdef DD_DEBUG_SPLITTING
					cerr << "INSTRUCTION HAS BEEN SPLITTED !!!!!!!!!!!" << endl;
					//	cerr << "Inst : " << *(entry->instruction) << endl;
					cerr << instructionQueue << endl;
#endif
				      }
				    /*
				    cerr << "SPLIT THE INSTRUCTION !!!!!!!!!!!" << endl;
				    cerr << "Inst : " << *(entry->instruction) << endl;
				    exit(-1);
				    */
				  }

				// If decoded instruction is a branch
				if( entry->instruction->operation->function
				    &
				    (FnBranch | FnConditionalBranch | FnBranchCountReg | FnBranchLinkReg)
				    )
				{
					if(in_flight_branches < MaxBranches)
					  {
					    in_flight_branches++;
					  }
					else	
					  {
#ifdef DD_DEBUG_FETCH_PREDECODE
  if (DD_DEBUG_TIMESTAMP < timestamp())
    {
      cerr << "["<<this->name()<<"("<<timestamp()<<")] MAXIMUM INFLIGHT BRANCHES !!! " << instructionQueue << endl;
      cerr << *(entry->instruction) << endl;
    }
#endif 
                                             break;
					  }
				}
				entry->predecoded = true;

				/* If instruction is branch */
				//				if(entry->instruction->fn & FnBranch)
				if( entry->instruction->operation->function
				    &
				    (FnBranch | FnConditionalBranch | FnBranchCountReg | FnBranchLinkReg)
				    )
				{
#ifdef DD_DEBUG_FETCH_BRANCH
				  cerr << "[F. Branch] Instruction is a Branch : " << endl;
				  cerr << *(entry->instruction) << endl;
#endif 
					/* If branch is conditional */
					//if(entry->instruction->fn & FnConditionalBranch)
					if(entry->instruction->operation->branch_conditioned())
					{
						/* Get the branch direction from the branch history table */
						entry->instruction->predicted_branch_direction = BHT.Read(entry->instruction->cia);
// 						entry->instruction->obq_tag = BHT.AllocateBranch();
						bht_accesses++;
					}
					else
					{
						/* If branch is unconditional */
						//DD//if(entry->instruction->fn & FnUnconditionalBranch)
						//if( (entry->instruction->fn & FnBranch)
					  if ( ( entry->instruction->operation->function
						 &
						 (FnBranch | FnConditionalBranch | FnBranchCountReg | FnBranchLinkReg)
						 )
					       //&& !(entry->instruction->operation->function & FnConditionalBranch) )
					       && ! (entry->instruction->operation->branch_conditioned() )
					       )
					    {
					      entry->instruction->predicted_branch_direction = Taken;
					    }
					  else
					    {
					      /* Throw an error if branch is neither conditional nor conditional */
					      cerr << "branch instruction (" << entry->instruction << ") is neither unconditional nor conditional" << endl; cerr.flush();
					      exit(-1);
					    }
					}
#ifdef DD_DEBUG_FETCH_BRANCH
					cerr << "[F. Branch] Predicted direction : " << endl;
					switch(entry->instruction->predicted_branch_direction)
					  {
					  case Taken:
					    cerr << "Taken";
					    break;
					  case NotTaken:
					    cerr << "Not Taken";
					    break;
					  case DontKnow:
					    cerr << "Dot Know !!!";
					    break;
					  default:
					    cerr << " Unknown predicted direction" << endl; abort();
					  }
					cerr << endl;
#endif


					/* If branch is direct */
					//DD//if(entry->instruction->fn & FnDirectBranch)
					//					if( (entry->instruction->fn & FnBranch)
					if ( ( entry->instruction->operation->function
					       &
					       (FnBranch | FnConditionalBranch | FnBranchCountReg | FnBranchLinkReg)
					       )
					     && !(entry->instruction->operation->function == FnBranchCountReg)
					     && !(entry->instruction->operation->function == FnBranchLinkReg)
					     )
					  {
						/* Get the target address from the emulator */
						//transient_emul_state.cia = entry->instruction->cia;
						//transient_emul_state.nia = transient_emul_state.cia + InstructionSize;
						/* Call the emulator */
					  emulator->SetCIA(entry->instruction->cia);
					  emulator->SetNIA(entry->instruction->cia+InstructionSize);
					  entry->instruction->operation->execute(emulator);//, &transient_emul_state, NULL /* no memory */);
					  
						//entry->instruction->operation->execute(entry->instruction->operation, &transient_emul_state, NULL /* no memory */);
					  if(entry->instruction->predicted_branch_direction == Taken) //;
						  //entry->instruction->predicted_nia = transient_emul_state.target_address;
					    entry->instruction->predicted_nia = entry->instruction->operation->get_target_address(entry->instruction->cia,emulator);
					  
#ifdef DD_DEBUG_FETCH_BRANCH
				  cerr << "[F. Branch] Instruction is a Direct Branch : " << endl;
				  cerr << *(entry->instruction) << endl;
#endif 
					}
					else
					{
					  // DD: BE CARFULL: RAS.SafePop will change predicted_nia !!!
					  // We have to choose if we really want to use a RAS !!!
//////////////////////// BEGINING of  RAS PROBLEM !!! //////////////////////////////////////////
					/* If branch is a return branch */
					//DD//if(entry->instruction->operation->function & FnReturnBranch)
					// DDDD: no more used of RAS...
					
					if(entry->instruction->operation->function == FnBranchLinkReg)
					{
						// If branch is (predicted) taken
						if( entry->instruction->operation->branch_conditioned() &&
						    entry->instruction->predicted_branch_direction == Taken)
						{
							// Taken
							// Get the target address from the Return Address Stack
							ras_accesses++;
							if(!RAS.SafePop(entry->instruction->predicted_nia))
							{
								entry->instruction->nia = 0;
								entry->instruction->ras_miss = true;
								ras_misses++;
							}
						}
					      else if ( !entry->instruction->operation->branch_conditioned() )
						{
							// Taken
							// Get the target address from the Return Address Stack
							ras_accesses++;
							if(!RAS.SafePop(entry->instruction->predicted_nia))
							{
								entry->instruction->nia = 0;
								entry->instruction->ras_miss = true;
								ras_misses++;
							}
						  
						}
#ifdef DD_DEBUG_FETCH_BRANCH
						else
						  {
						    cerr << "WARNING: Branch to LR predicted NotTaken..." << endl;
						  }
#endif
					}
//////////////////////// END of  RAS PROBLEM !!! //////////////////////////////////////////

					  /* If branch is indirect */
					  //if(entry->instruction->fn & FnIndirectBranch)
					  if( (entry->instruction->operation->function & FnBranchCountReg)
					      //|| (entry->instruction->operation->function & FnBranchLinkReg)
					      )
					    {
					      /* If branch is (predicted) taken */
					      //if(entry->instruction->predicted_branch_direction == Taken)
					      if( entry->instruction->operation->branch_conditioned() &&
						  entry->instruction->predicted_branch_direction == Taken )
						{
						  /* Taken */
						  /* Get the target address from the Branch Target Buffer */
						  btb_accesses++;
						  if(!BTB.Read(entry->instruction->cia, entry->instruction->predicted_nia))
						    {
						      btb_misses++;
						      entry->instruction->predicted_nia = 0;
						      entry->instruction->btb_miss = true;
						    }
						}
					      // TODO Else ???!!!
					      else if ( !entry->instruction->operation->branch_conditioned() )
						{
						  /* Taken */
						  /* Get the target address from the Branch Target Buffer */
						  btb_accesses++;
						  if(!BTB.Read(entry->instruction->cia, entry->instruction->predicted_nia))
						    {
						      btb_misses++;
						      entry->instruction->predicted_nia = 0;
						      entry->instruction->btb_miss = true;
						    }
						}
					    }
#ifdef DD_DEBUG_FETCH_BRANCH
					  cerr << "[F. Branch] Instruction is an INDIRECT Branch : " << endl;
					  cerr << *(entry->instruction) << endl;
					  cerr << "intruction->btb_miss="<< entry->instruction->btb_miss << endl;
#endif
					}

					/// DDD /// C'etait la !!!
					/* If branch is a call branch */
					//DD//if(entry->instruction->operation->function & FnCallBranch)
					if (entry->instruction->operation->branch_linked())
					{
						/* If branch is (predicted) taken */
						if(entry->instruction->predicted_branch_direction == Taken)
						{
							/* Taken */
							//if(entry->instruction->destination.reg >= 0)
							if(entry->instruction->destinations.size() > 0)
							{
								/* Push on the return address stack the return address */
							  //								RAS.SafePush(entry->instruction->cia + InstructionSize, entry->instruction);
								RAS.SafePush(entry->instruction->cia + InstructionSize, entry->instruction);
							}
						}
					}

					/* If branch is taken */
					if(entry->instruction->predicted_branch_direction == Taken)
					{
						/* Taken */
						/* Flush the instruction after the branch */
						instructionQueue.FlushAfter(entry);

						/* Correct the path */
						cia = entry->instruction->predicted_nia;
						seq_cia = cia;
						//if(waiting_instr_cache_accept) pending_instr_cache_requests--;
						//waiting_instr_cache_accept = false;
						ignore_instr_cache_responses = pending_instr_cache_requests;
						pending_instr_cache_access_size = 0;

#ifdef DD_DEBUG_FETCH_VERB2
						cerr << "[FETCH] ==== Correct fetcher on Speculative path ======== ["<< timestamp()<<"]"<< endl;
						cerr << *this << endl;
#endif
					      //if((entry->instruction->operation->function & FnIndirectBranch) && entry->instruction->btb_miss)
					      if( (
						   (entry->instruction->operation->function & FnBranchCountReg)
						   || (entry->instruction->operation->function & FnBranchLinkReg)
						   )  
						  && entry->instruction->btb_miss
						  )
						{
#ifdef DD_DEBUG_FETCH_BRANCH
					  cerr << "[F. Branch] Instruction is an INDIRECT Branch : " << endl;
					  cerr << *(entry->instruction) << endl;
					  cerr << "intruction->btb_miss="<< entry->instruction->btb_miss << endl;
					  cerr << " WE ARE SETTING BTB_MISS to TRUE !!!" << endl;
#endif						  
					  
							btb_miss = true;
						}
					   //DD//if((entry->instruction->operation->function & FnReturnBranch) && entry->instruction->ras_miss)
						if((entry->instruction->operation->function & FnBranchLinkReg)
						   && entry->instruction->ras_miss)
						{
							ras_miss = true;
						}
					}
					else
					{
						if(entry->instruction->predicted_branch_direction == NotTaken)
						{
#ifdef DD_DEBUG_FETCH_VERB2
						  if (DD_DEBUG_TIMESTAMP < timestamp())
						    {
						  cerr << "[FETCH] ==== Do not Correct fetcher ... ======== ["<< timestamp()<<"]"<< endl;
						  cerr << *this << endl;
						    }
#endif
							/* Not taken */
							/* The target address is the address of following instruction */
							entry->instruction->predicted_nia = entry->instruction->cia + InstructionSize;
							/*
						seq_cia = cia;
						if(waiting_instr_cache_accept) pending_instr_cache_requests--;
						waiting_instr_cache_accept = false;
						ignore_instr_cache_responses = pending_instr_cache_requests;
						pending_instr_cache_access_size = 0;
							*/
							//cia = entry->instruction->predicted_nia;
						}
						else
						{
							/* Throw an error if branch is neither predicted taken nor not taken */
							cerr << "branch instruction (" << entry->instruction << ") is neither predicted taken nor not taken" << endl; cerr.flush();
							exit(-1);
						}
					}
				} // if instruction is a branch
				//#endif // USE_PERFECT_BRANCH_PREDICTION
#ifdef DDDEBUG_USE_PERFECT_BRANCH_PREDICTION
#ifdef DD_DEBUG_PERFECT_BRANCH
				cerr << "DD_DEBUG_PERFECT: Emulator->CIA() before="<< hex << emulator->GetCIA() << dec << endl;
				cerr << "DD_DEBUG_PERFECT: Emulator->NIA() before="<< hex << emulator->GetNIA() << dec << endl;
#endif
				
				try{
				  entry->instruction->operation->execute(emulator);
				}
				catch(Exception &e)
				  {
				    // Here we catch some trap exceptions ???
				    //#ifdef DD_DEBUG_ALIGNMENT
				    //cerr << "["<< this->name() << "] (" << timestamp() << ") --- Exception : " << e.what() << endl;
				    //#endif			    
				  }
				
#ifdef DD_DEBUG_PERFECT_BRANCH
				cerr << "DD_DEBUG_PERFECT: Emulator->CIA() after="<< hex << emulator->GetCIA() << dec << endl;
				cerr << "DD_DEBUG_PERFECT: Emulator->NIA() after="<< hex << emulator->GetNIA() << dec << endl;
#endif
				//				emulator->StepOneInstruction();
				//entry->instruction->predicted_nia = entry->instruction->cia + InstructionSize;
#ifdef DD_DEBUG_PERFECT_BRANCH
				cerr << "DD_DEBUG_PERFECT: Inst=" << entry->instruction << endl;
				for (int i=0; i<unisim::component::clm::processor::ooosim::nIntegerArchitecturalRegisters; i++)
				{
					cerr << "DD_DEBUG_PERFECT: GPR["<<i<<"]="<< hex << emulator->GetGPR(i) << dec << endl;
				}
				cerr << "DD_DEBUG_PERFECT: CR="<< hex << emulator->GetCR() << dec << endl;
#endif

				if( entry->instruction->operation->function
				    &
				    (FnBranch | FnConditionalBranch | FnBranchCountReg | FnBranchLinkReg)
				    )
				{
				  /* Correct the path */
				  // cia = entry->instruction->nia;
				  cia = emulator->GetNIA();
				  //nia = emulator->GetNIA();
				  seq_cia = cia;
				  entry->instruction->predicted_nia = entry->instruction->cia + InstructionSize;
				  if (entry->instruction->predicted_nia != cia)
				    {
#ifdef DD_DEBUG_PERFECT_BRANCH
				      cerr << "DD_DEBUG_PERFECT: Instruction before="<< hex << entry->instruction << dec << endl;
#endif
				      entry->instruction->nia = cia;
				      entry->instruction->predicted_nia = cia;
				      instructionQueue.FlushAfter(entry);
#ifdef DD_DEBUG_PERFECT_BRANCH
				      cerr << "DD_DEBUG_PERFECT: Instruction after ="<< hex << entry->instruction << dec << endl;
#endif

				    }
				}
#endif // USE_PERFECT_BRANCH_PREDICTION

				/* If instruction is a SysCall */
			        if( entry->instruction->operation->function == FnSysCall )
				  { 
				    syscall_in_pipeline = true;
				    /* Flush the instruction after the branch */
				    instructionQueue.FlushAfter(entry);

				    /* Correct the path */
				    cia = entry->instruction->nia;
				    seq_cia = cia;
				  }
				
			}// end of if (!predecoded) ...

			//cerr << " INFO at EO-EOF : cia = " << hexa(entry->instruction->cia) << " \t nia = " << hexa(entry->instruction->nia) << " \t predicted_nia = " << hexa(entry->instruction->predicted_nia) << endl;
			entry++;
		}

	///////////////////////////////////////// USE_PERFECT_FETCH ///////////////// ENDIF !!!
#endif
		/* Flush ? */
		//		if(inFlush.data)
		if (inFlush.enable && inFlush.data.something())
		{
		  if (inFlush.enable && inFlush.data)
		  {
		  
#ifdef DD_DEBUG_FLUSH
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====  Flush !!!" << endl;
#endif
		  //			changed = true;
			/* Flush the instruction queue */
			instructionQueue.Flush();

			//if(waiting_instr_cache_accept) pending_instr_cache_requests--;
			ignore_instr_cache_responses = pending_instr_cache_requests;
			//waiting_instr_cache_accept = false;
			pending_instr_cache_access_size = 0;
			btb_miss = false;
			ras_miss = false;
			in_flight_branches = 0;

			syscall_in_pipeline = false;

#ifdef DD_DEBUG_FETCH_VERB2
			if (DD_DEBUG_TIMESTAMP < timestamp())
			    {
		  cerr << "[FETCH] ==== Correct fetcher on Correct path ======== ["<< timestamp()<<"]"<< endl;
		  //		  cerr << *this << endl;
			    }
#endif
			/* Reset output ports */
			/*
			outL1Valid = false;

			for(i = 0; i < Width; i++)
			{
				outValid[i] = false;
				outEnable[i] = false;
			}
			*/

			RAS.FlushRollbackBuffer();
// 			BHT.Flush();

			/* Continue fetch on the correct path */
			seq_cia = cia = nia;
#ifdef DD_DEBUG_FETCH_VERB2
 if (DD_DEBUG_TIMESTAMP < timestamp())
			    {
			cerr << "!!!!! time stamp = " << timestamp() << ": got flush: continue fetching along " << hexa(nia) << endl;
			cerr << "[FETCH] ==== We are Flushing ... (EOF) ========== ["<< timestamp()<<"]"<< endl;
			//			cerr << *this << endl;
			    }
#endif
			
		  }
		}
		// Keep coherent CIA with emulator ...
//		emulator->SetCIA(cia);
#ifdef DD_DEBUG_FETCH_VERB2
 if (DD_DEBUG_TIMESTAMP < timestamp())
			    {
		cerr << "[FETCH] ==== Fetch state at EOF ========== ["<< timestamp()<<"]"<< endl;
		cerr << *this << endl;
			    }
#endif
#ifdef DD_DEBUG_FETCH_VERB3
 if (DD_DEBUG_TIMESTAMP < timestamp())
			    {
		cerr << "["<<this->name()<<"("<<timestamp()<<")] instruction queue at EOC: " << instructionQueue << endl;
			    }
#endif


#ifdef DD_DEBUG_PIPELINE_VERB1
 if (DD_DEBUG_TIMESTAMP < timestamp())
			    {
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ==== Pipeline Debug" << endl;
		cerr << this << endl;
			    }
#endif
			    } /* End of ENDOF_CYCLE */
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif // #ifdef ALL_PERFECT
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
	void start_of_cycle()
	{
	  // Mourad modifs 
#ifdef DD_DEBUG_FETCH_VERB3
	  if (DD_DEBUG_TIMESTAMP < timestamp())
	    {
	  cerr << "["<<this->name()<<"("<<timestamp()<<")] instruction queue at SOC: " << instructionQueue << endl;
	    }
#endif

		/* Instruction Cache Access */
	  //		if(!waiting_instr_cache_accept)
	  //	{
			/* Do not continue fetch if there is a BTB miss or a RAS miss */
	  if(!btb_miss && !ras_miss && !syscall_in_pipeline && !is_terminated)
	    {
	      /* Do not continue if we already reach the maximum number of pending instruction cache requests */
	      if(pending_instr_cache_requests < MaxPendingRequests)
		{
		  /* Get the size of the instruction cache access */
		  accessSize = GetMaximumAccessSize(seq_cia);
		  
		  if(accessSize > 0)
		    {
		      /* Do a request to the instruction cache */
		      memreq < InstructionPtr, nCPUDataPathSize > tmp_memreq;
		      //tmp_memreq.instr = ...
		      tmp_memreq.address = seq_cia;
		      tmp_memreq.size = accessSize;
		      tmp_memreq.command = memreq_types::cmd_READ;
		      //tmp_memreq.uid = ...;
		      tmp_memreq.sender_type = memreq_types::sender_CPU;
		      tmp_memreq.message_type = memreq_types::type_REQUEST;
		      tmp_memreq.sender = this;
		      tmp_memreq.req_sender = this;
		      
		      outIL1.data = tmp_memreq;
		      //Mourad modifs 
#ifdef DD_DEBUG_FETCH_VERB2
		      if (DD_DEBUG_TIMESTAMP < timestamp()) 
			{
		      cerr << "[FETCH] ==== We are accessing IL1 (SOF) ======= ["<< timestamp()<<"]"<< endl;
		      cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== We are accessing IL1 (SOF) ==== address="<<hexa(tmp_memreq.address)<<" / accessSize="<< tmp_memreq.size << endl;
			}
#endif
		    }
		  else
		    {
		      // Mourad modifs
#ifdef DD_DEBUG_FETCH_VERB2
		      if (DD_DEBUG_TIMESTAMP < timestamp())
			{
		      cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== No IL1: accessSize <=0 (SOF) ======= ["<< timestamp()<<"]"<< endl;
			}
#endif
		      /* No request */
		      //outL1Valid = false;
		      outIL1.data.nothing();
		    }
		}
	      else
		{
#ifdef DD_DEBUG_FETCH_VERB2
		  //Mourad modifs
		  if (DD_DEBUG_TIMESTAMP < timestamp())
		    {		
  cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== No IL1: pending_instr_cache_requests >= MaxPendingRequests (SOF) ======= ["<< timestamp()<<"]"<< endl;
		    }
#endif
		  /* No request */
		  //outL1Valid = false;
		  outIL1.data.nothing();
		}
	    }
	  else
	    {
#ifdef DD_DEBUG_FETCH_VERB2
	      //Mourad modifs
	      if (DD_DEBUG_TIMESTAMP < timestamp())
		{	      
cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== No IL1: ! (!btb_miss && !ras_miss) (SOF) ==== btb_miss: " << btb_miss <<" ras_miss: "<< ras_miss << endl;
	      //	      cerr << "[FETCH] ==== No IL1: ! (!btb_miss && !ras_miss) (SOF) ======= ["<< timestamp()<<"]"<< endl;
		}
#endif
	      /* No request */
	      //outL1Valid = false;
	      outIL1.data.nothing();
	    }

	  
	  int i;
	  //InstructionQueueEntry *entry;
	  QueuePointer<InstructionQueueEntry, InstructionQueueSize > entry;
	  
	  /* Do a request to the allocator/renamer for each instructions */
	  for(i = 0, entry = instructionQueue.SeekAtHead(); entry && entry->predecoded && i < Width; entry++, i++)
	    {
	      outInstruction[i].data = entry->instruction;
	      //Mourad modifs
#ifdef DD_DEBUG_FETCH_VERB3
	      if (DD_DEBUG_TIMESTAMP < timestamp())
		{
	      cerr << "["<<this->name()<<"("<<timestamp()<<")] instruction sent at SOC on outInst["<<i<<"]: " << *(entry->instruction) << endl;
		}
#endif
	    }
	  /* Clear the remaining request ports */
	  for(; i < Width; i++)
	    {
	      //			outValid[i] = false;
	      outInstruction[i].data.nothing();
#ifdef DD_DEBUG_FETCH_VERB3
	      if (DD_DEBUG_TIMESTAMP < timestamp())
		{
	      cerr << "["<<this->name()<<"("<<timestamp()<<")] Nothing sent at SOC on outInst["<<i<<"]: " << endl;
		}
#endif
	    }
	  if (previous_cia == cia) { stall_counter++; } else { stall_counter=0; }
	  if ((!is_terminated) && (stall_counter > 10000)) { cerr << "Fetch is stalling at cycle: ("<< timestamp() <<")" << endl;exit(-1); }
	  previous_cia = cia;
 	} // End of "Start of Cycle"


  
	void WriteCIA(address_t pc)
	{
		cia = pc;
		seq_cia = pc;
	}

	friend ostream& operator << (ostream& os, const Fetcher& fetcher)
	{
		os << "=============== FETCH ==============" << endl;
		//		os << "waiting_instr_cache_accept=" << fetcher.waiting_instr_cache_accept << endl;
		os << "   --> CIA: " << hexa(fetcher.cia) 
		   << " \t / --> SeqCIA: " << hexa(fetcher.seq_cia) 
		   << " \t / --> NIA: " << hexa(fetcher.nia) << endl;
		os << "pending_instr_cache_requests=" << fetcher.pending_instr_cache_requests << endl;
		os << "pending_instr_cache_access_size=" << fetcher.pending_instr_cache_access_size << endl;
		os << "ignore_instr_cache_responses=" << fetcher.ignore_instr_cache_responses << endl;
		os << "accessSize=" << fetcher.accessSize << endl;
		os << "btb_miss=" << fetcher.btb_miss << endl;
		os << "ras_miss=" << fetcher.ras_miss << endl;
		os << "syscall_in=" << fetcher.syscall_in_pipeline << endl;
		os << "Instruction Queue:" << endl;
		os << fetcher.instructionQueue;
		os << "Return Address Stack:" << endl;
		os << fetcher.RAS;
		os << "====================================" << endl;
		return os;
	}

	bool Check()
	{
		return instructionQueue.Check();// && RAS.Check();
	}

	void DumpStats(ostream& os, uint64_t sim_total_time, uint64_t sim_cycle)
	{
		os << "bht_accesses " << bht_accesses << " # BHT accesses" << endl;
		os << "bht_misses " << bht_misses << " # BHT misses" << endl;
		os << "bht_miss_rate " << (double) bht_misses / (double) bht_accesses << " # BHT miss rate" << endl;
		os << "btb_accesses " << btb_accesses << " # BTB accesses" << endl;
		os << "btb_misses " << btb_misses << " # BTB misses" << endl;
		os << "btb_miss_rate " << (double) btb_misses / (double) btb_accesses << " # BTB miss rate" << endl;
		os << "ras_accesses " << ras_accesses << " # RAS accesses" << endl;
		os << "ras_misses " << ras_misses << " # RAS misses" << endl;
		os << "ras_miss_rate " << (double) ras_misses / (double) ras_accesses << " # RAS miss rate" << endl;
		os << "ras_capacity_misses " << ras_capacity_misses << " # RAS capacity misses" << endl;
		os << "instruction_queue_cumulative_occupancy " << instruction_queue_cumulative_occupancy << " # instruction queue cumulative occupancy" << endl;
		os << "instruction_queue_occupancy " << ((double) instruction_queue_cumulative_occupancy / (double) sim_cycle) << " # instruction queue occupancy (instructions/cycle)" << endl;
	}
	
	void WarmRestart()
	{
		/* Flush the instruction queue */
		instructionQueue.Reset();

		//		waiting_instr_cache_accept = false;
		pending_instr_cache_requests = 0;
		ignore_instr_cache_responses = 0;
		pending_instr_cache_access_size = 0;
		btb_miss = false;
		ras_miss = false;
		RAS.ResetRollbackBuffer();
		in_flight_branches = 0;
		//		changed = true;
		accessSize = 0;
		inum = 0;
		//		state_init(&transient_emul_state);
	}
	
	void ResetStats()
	{
		bht_accesses = 0;
		bht_misses = 0;
		btb_accesses = 0;
		btb_misses = 0;
		ras_accesses = 0;
		ras_misses = 0;
		ras_capacity_misses = 0;
		instruction_queue_cumulative_occupancy = 0;
	}
  
  void Reset()
  {
    cia = emulator->GetCIA();
  }

private:
  //	bool waiting_instr_cache_accept;		/*< true if fetch is waiting for an instruction cache accept */
	int pending_instr_cache_requests;		/*< number of pending instruction cache requests */
	int pending_instr_cache_access_size;	/*< total size (in bytes) of the pending instruction cache requests */
	int ignore_instr_cache_responses;		/*< number of instruction cache response to ignore (used for flush requests) */
	address_t previous_cia;							/*< current instruction address */
	address_t cia;							/*< current instruction address */
	address_t nia;							/*< nia instruction address */
	address_t seq_cia;						/*< sequential current instruction address (for sequential fetching) */
	int accessSize;							/*< instruction cache access size */
  //DD//	Queue<InstructionQueueEntry<T, nSources>, InstructionQueueSize> instructionQueue;	/* Instruction queue */
	Queue<InstructionQueueEntry, InstructionQueueSize> instructionQueue;	/* Instruction queue */
	endianess_t endianess;					/*< either little_endian or big_endian */
	uint64_t inum;							/*< instruction number */
	//DD	Emulator *emulator;						/*< pointer to the emulator wrapper */
  //	CPUSim *emulator;						/*< pointer to the emulator wrapper */
	CPUEmu *emulator;						/*< pointer to the emulator wrapper */
  //	CPUEmu *emulator2;						/*< pointer to the emulator wrapper */
	bool btb_miss;							/*< true if a BTB miss occured, used to stop fetching until branch resolution */
	bool ras_miss;							/*< true if a RAS miss occured, used to stop fetching until branch resolution */
	int in_flight_branches;
	BranchHistoryTable<BHT_Size, BHT_nBits, BHT_nHistorySize, InstructionSize, MaxBranches> BHT;	/*< The branch history table */
	BranchTargetBuffer<BTB_Size, BTB_Associativity, InstructionSize> BTB;				/*< The branch target buffer */
	//DD	ReturnAddressStack<T, nSources, RAS_Size, MaxBranches> RAS;			/*< The return address stack */
	ReturnAddressStack<RAS_Size, MaxBranches> RAS;			/*< The return address stack */
	//	state_t transient_emul_state;
  //	bool changed;
	
	/* Statistics */
  /*
  uint64_t bht_accesses;					//< number of BHT accesses
  uint64_t bht_misses;						//< number of BHT misses
  uint64_t btb_accesses;					//< number of BTB accesses
  uint64_t btb_misses;						//< number of BTB misses
  uint64_t ras_accesses;					//< number of RAS accesses
  uint64_t ras_misses;						//< number of RAS misses
  uint64_t instruction_queue_cumulative_occupancy;
  uint64_t ras_capacity_misses;
  */
  
        bool syscall_in_pipeline;
  //  bool stall_dueto_unknown_target_address;
        int stall_counter;
  bool is_terminated;
};

} // end of namespace fetch
} // end of namespace pipeline
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
