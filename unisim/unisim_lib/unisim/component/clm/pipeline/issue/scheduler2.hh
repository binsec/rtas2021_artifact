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
                       FastScheduler.hh  -  description
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_ISSUE_SCHEDULER_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_ISSUE_SCHEDULER_HH__

#include <unisim/component/clm/processor/ooosim/parameters.hh>
#include <unisim/component/clm/interfaces/instruction_interface.hh>

namespace unisim {
namespace component {
namespace clm {
namespace pipeline {
namespace issue {

using unisim::component::clm::interfaces::InstructionPtr;

using unisim::component::clm::processor::ooosim::nConditionRegisters;
using unisim::component::clm::processor::ooosim::nFPSCArchitecturalRegisters;
using unisim::component::clm::processor::ooosim::nFPSCRegisters;
using unisim::component::clm::processor::ooosim::nLinkArchitecturalRegisters;
using unisim::component::clm::processor::ooosim::nLinkRegisters;
using unisim::component::clm::processor::ooosim::nCountArchitecturalRegisters;
using unisim::component::clm::processor::ooosim::nCountRegisters;
using unisim::component::clm::processor::ooosim::nXERArchitecturalRegisters;
using unisim::component::clm::processor::ooosim::nXERRegisters;

using unisim::component::clm::processor::ooosim::IssueWidth;


/** A C++ class representing a reservation station
T: the data type used for storing instruction sources and destinations
nSources: number of sources (operands) */
template <class T, int nSources>
class ReservationStation
{
	public:
		bool ready;
		bool issuable;
		InstructionPtr instruction;	/*< the instruction hold by the reservation station */
		bool sourceValid[nSources];

		ReservationStation()
		{
			ready = false;
			issuable = false;
			int i;
			for(i = 0; i < nSources; i++)
				sourceValid[i] = false;
		}

		void Initialize(const InstructionPtr &instruction)
		{
			this->instruction = instruction;
			ready = false;
			issuable = false;
			int i;
			for(i = 0; i < nSources; i++)
				sourceValid[i] = false;
		}

		/* This method updates the reservation station */
		bool Update(const InstructionPtr &instruction, bool updateData)
		{
		  int i;
		  bool changed = false;
		  for (int j=0; j<instruction->destinations.size(); j++)
		  {
			/* We update reservation stations waiting for that tag */
			if(!ready && instruction->destinations[j].tag >= 0)
			{
				int tag = instruction->destinations[j].tag;
				bool ready = true;
				for(i = 0; i < this->instruction->sources.size(); i++)
				{
					if(this->instruction->sources[i].tag == tag &&
					   this->instruction->sources[i].type == instruction->destinations[j].type)
					{
						this->sourceValid[i] = true;
						if(updateData)
							this->instruction->sources[i].data = instruction->destinations[j].data;
						changed = true;
					}
					else
					{
						ready = false;
					}
				}
				this->ready = ready;
			}
		  }
		  return changed;
		}

		friend ostream& operator << (ostream& os, const ReservationStation<T, nSources>& rs)
		{
			os << "ready=" << rs.ready << ",issuable=" << rs.issuable << ",instruction=" << rs.instruction;
			return os;
		}

		int operator == (const ReservationStation<T, nSources>& rs) const
		{
			return rs.instruction->inum == instruction->inum;
		}
};

template <class T,
          int nSources,
          int IntegerIssueQueueSize,
          int IntegerIssueWidth,
          int nIntegerWritePorts,
          int FloatingPointIssueQueueSize,
          int FloatingPointIssueWidth,
          int nFloatingPointWritePorts,
          int LoadStoreIssueQueueSize,
          int LoadStoreIssueWidth,
          int nLoadStoreWritePorts,
          int WriteBackWidth,
	  int nIntegerRegisters,
	  int nFloatingPointRegisters>
class Scheduler : public module
{
public:
	/* input clock */
	inclock inClock;

	/* Write Ports on integer issue queue */
	inport<InstructionPtr > inIntegerInstruction[nIntegerWritePorts];	// < instructions to write

	/* Write Ports on floating point issue queue */
	inport<InstructionPtr > inFloatingPointInstruction[nFloatingPointWritePorts]; //< instructions to write

	/* Write Ports on load/store issue queue */
	inport<InstructionPtr > inLoadStoreInstruction[nLoadStoreWritePorts]; //< instructions to write

	/* From the common data bus */
	inport<InstructionPtr > inWriteBackInstruction[WriteBackWidth];
	/* WriteBack forward to the previous module */
  //	outport<Instruction > outWriteBackInstruction[WriteBackWidth];

	/* To Register File */
	outport<InstructionPtr > outIntegerInstruction[IntegerIssueWidth];	// ready instructions
	outport<InstructionPtr > outFloatingPointInstruction[FloatingPointIssueWidth];	// ready instructions 
	outport<InstructionPtr > outLoadStoreInstruction[LoadStoreIssueWidth];	// ready instructions 

	/* From the reorder buffer */
	inport<bool> inFlush;
	outport<bool> outFlush;

	Scheduler(const char *name) : module(name)
	{
		int i, j;

		class_name = " SchedulerClass";
		// Unisim port names ...
		for (i=0; i<nIntegerWritePorts; i++)
		  {
		    inIntegerInstruction[i].set_unisim_name(this,"inIntegerInstruction",i);
		  }		
		for (i=0; i<nFloatingPointWritePorts; i++)
		  {
		    inFloatingPointInstruction[i].set_unisim_name(this,"inFloatingPointInstruction",i);
		  }		
		for (i=0; i<nLoadStoreWritePorts; i++)
		  {
		    inLoadStoreInstruction[i].set_unisim_name(this,"inLoadStoreInstruction",i);
		  }		
		for (i=0; i<WriteBackWidth; i++)
		  {
		    inWriteBackInstruction[i].set_unisim_name(this,"inWriteBackInstruction",i);
		  }		
		//		for (i=0; i<WriteBackWidth; i++)
		//		  {
		//		    outWriteBackInstruction[i].set_unisim_name(this,"outWriteBackInstruction",i);
		//		  }		
		for (i=0; i<IntegerIssueWidth; i++)
		  {
		    outIntegerInstruction[i].set_unisim_name(this,"outIntegerInstruction",i);
		  }		
		for (i=0; i<FloatingPointIssueWidth; i++)
		  {
		    outFloatingPointInstruction[i].set_unisim_name(this,"outFloatingPointInstruction",i);
		  }		
		for (i=0; i<LoadStoreIssueWidth; i++)
		  {
		    outLoadStoreInstruction[i].set_unisim_name(this,"outLoadStoreInstruction",i);
		  }		
		inFlush.set_unisim_name(this,"inFlush");
		outFlush.set_unisim_name(this,"outFlush");


                sensitive_pos_method(start_of_cycle) << inClock;
                sensitive_neg_method(end_of_cycle) << inClock;

		/*
		// onData sensitivity list
		for(i = 0; i < nIntegerWritePorts; i++)
		  { sensitive_method(onData) << inIntegerInstruction[i].data; }
		for(i = 0; i < nFloatingPointWritePorts; i++)
		  { sensitive_method(onData) << inFloatingPointInstruction[i].data; }
		for(i = 0; i < nLoadStoreWritePorts; i++)
		  { sensitive_method(onData) << inLoadStoreInstruction[i].data; }
		// onAccept sensitivity list
		for(i = 0; i < IntegerIssueWidth; i++)
			{ sensitive_method(onAccept) << outIntegerInstruction[i].accept; }
		for(i = 0; i < FloatingPointIssueWidth; i++)
			{ sensitive_method(onAccept) << outFloatingPointInstruction[i].accept; }
		for(i = 0; i < LoadStoreIssueWidth; i++)
			{ sensitive_method(onAccept) << outLoadStoreInstruction[i].accept; }
		*/
		// onDataAccept sensitivity list
		for(i = 0; i < nIntegerWritePorts; i++)
		  { sensitive_method(onDataAccept) << inIntegerInstruction[i].data; }
		for(i = 0; i < nFloatingPointWritePorts; i++)
		  { sensitive_method(onDataAccept) << inFloatingPointInstruction[i].data; }
		for(i = 0; i < nLoadStoreWritePorts; i++)
		  { sensitive_method(onDataAccept) << inLoadStoreInstruction[i].data; }
		// onDataAccept sensitivity list
		for(i = 0; i < IntegerIssueWidth; i++)
			{ sensitive_method(onDataAccept) << outIntegerInstruction[i].accept; }
		for(i = 0; i < FloatingPointIssueWidth; i++)
			{ sensitive_method(onDataAccept) << outFloatingPointInstruction[i].accept; }
		for(i = 0; i < LoadStoreIssueWidth; i++)
			{ sensitive_method(onDataAccept) << outLoadStoreInstruction[i].accept; }

		// onWriteBackData sensitivity list
		for(i = 0; i < WriteBackWidth; i++)
		        { sensitive_method(onWriteBackData) << inWriteBackInstruction[i].data; }
		// onWriteBackAccept sensitivity list
		//		for(i = 0; i < WriteBackWidth; i++)
		//		        sensitive_method(onWriteBackAccept) << outWriteBackInstruction[i].accept;
		// onWriteBackEnable sensitivity list
		for(i = 0; i < WriteBackWidth; i++)
		        { sensitive_method(onWriteBackEnable) << inWriteBackInstruction[i].enable; }


		sensitive_method(onFlushData) << inFlush.data;
		sensitive_method(onFlushAccept) << outFlush.accept;
		sensitive_method(onFlushEnable) << inFlush.enable;


		for(i = 0; i < nIntegerRegisters; i++)
			integerRegisterTimeStamp[i] = 0;

		for(i = 0; i < nFloatingPointRegisters; i++)
			floatingPointRegisterTimeStamp[i] = 0;

		for(i = 0; i < nConditionRegisters; i++)
			conditionRegisterTimeStamp[i] = 0;

		for(i = 0; i < nFPSCRegisters; i++)
			FPSCRegisterTimeStamp[i] = 0;

		for(i = 0; i < nLinkRegisters; i++)
			linkRegisterTimeStamp[i] = 0;

		for(i = 0; i < nCountRegisters; i++)
			countRegisterTimeStamp[i] = 0;

		for(i = 0; i < nXERRegisters; i++)
			XERRegisterTimeStamp[i] = 0;

		time_stamp = 0;

		// ...
		integer_to_remove = 0;
		floating_point_to_remove = 0;
		loadstore_to_remove = 0;
		//		allAcceptRecevied = false;
		WriteBacksProcessed = false;

		for(i = 0; i < IntegerIssueWidth; i++)
		  { integerIssueBusy[i] = 0; }
		for(i = 0; i < FloatingPointIssueWidth; i++)
		  { floatingPointIssueBusy[i] = 0; }
		for(i = 0; i < LoadStoreIssueWidth; i++)
		  { loadStoreIssueBusy[i] = 0; }

	}

	/** Update a reservation
		@param rs a reservation station
	*/
	void Update(ReservationStation<T, nSources> *rs)
	{
		if(!rs->ready)
		{
			int writeBackPort;
			/* Each write back port */
			for(writeBackPort = 0; writeBackPort < WriteBackWidth; writeBackPort++)
			{
				/* Is there an instruction being written back */
				if(inWriteBackInstruction[writeBackPort].enable)
				{
					/* Get the instruction */
				        InstructionPtr instruction = inWriteBackInstruction[writeBackPort].data;
					//					const Instruction *instruction = &inst;

					/* Does that instruction produce a result ? */
					//if(instruction->destination.tag >= 0)
					if(instruction->destinations.size() > 0)
					{
						/* Update the reservation station */
						rs->Update(instruction, false);
					}
				}
			}
		}
	}


	/** Checks if instruction will be issuable and removed from th issue queue on the next clock cycle
		@param rs a reservation station
		@return true if it will be issuable
	*/
	bool WillBeIssuable(ReservationStation<T, nSources> *rs)
	{
		/* If instruction is ready, it already is issuable */
		if(rs->ready) return true;
		/* If instruction is already issuable, then return true */
		if(rs->issuable) return true;
		int i;
		int needed = 0;
		int ready = 0;

		/* For each source operand */
		//	for(i = 0; i < nSources; i++)
		for(i = 0; i < rs->instruction->sources.size(); i++)
		{
			/* Get the tag of the source operand */
			int tag = rs->instruction->sources[i].tag;

			if(tag >= 0)
			{
				/* Count the source operand as needed */
				needed++;
				/* If source operand is already available */
				if(rs->sourceValid[i])
				{
					/* then count the source operand as ready */
					ready++;
				}
				else
				{
				  /*

				  // else see if source operand is being written back by another instruction
					bool bypassed = false;
					int writeBackPort;
					// For each write back port
					for(writeBackPort = 0; writeBackPort < WriteBackWidth; writeBackPort++)
					{
					  // Is there an instruction being written back ?
						if(inWriteBackInstruction[writeBackPort].enable)
						{
						  // Get the instruction
						  InstructionPtr instruction = inWriteBackInstruction[writeBackPort].data;
						  //const Instruction *instruction = &inst;
						  
						  //if(instruction->destination.tag >= 0)
						  for (int j=0; j<instruction->destinations.size(); j++)
						    {
							  
						      // See if source operand match the destination operand
						      if(rs->instruction->sources[i].type == instruction->destinations[j].type &&
							 instruction->destinations[j].tag == tag)
							{
							  // Count the source operand as ready
							  ready++;
							  // source operand was bypassed
							  bypassed = true;
							  break;
							}
						    }
						}
					}
				  */
					/* If source operand was not bypassed */
				  //					if(!bypassed)
				  //					{
						/* Check when source operand will be produced */
						switch(rs->instruction->sources[i].type)
						{
							case GPR_T:
								if(integerRegisterTimeStamp[tag] >= time_stamp) return false;
								ready++;
								break;

							case FPR_T:
								if(floatingPointRegisterTimeStamp[tag] >= time_stamp) return false;
								ready++;
								break;

							case CR_T:
								if(conditionRegisterTimeStamp[tag] >= time_stamp) return false;
								ready++;
								break;
							case FPSCR_T:
								if(FPSCRegisterTimeStamp[tag] >= time_stamp) return false;
								ready++;
								break;
							case LR_T:
								if(linkRegisterTimeStamp[tag] >= time_stamp) return false;
								ready++;
								break;
							case CTR_T:
								if(countRegisterTimeStamp[tag] >= time_stamp) return false;
								ready++;
								break;
							case XER_T:
								if(XERRegisterTimeStamp[tag] >= time_stamp) return false;
								ready++;
								break;

							default: ;
						}
						//	}
				}
			}
		}

		/* instruction will be issuable whenever needed operands are all ready */
		return ready == needed;
	}

	/** Search for the next integer instruction in the integer issue queue that will be issuable
		@param integerRs a reservation station pointer
	*/
	void NextIntegerThatWillBeIssuable(OoOQueuePointer<ReservationStation<T, nSources>, IntegerIssueQueueSize>& integerRs)
	{
		if(integerRs)
		{
			do
			{
				integerRs++;
			} while(integerRs && !WillBeIssuable(integerRs));
		}
	}

	/** Search for the next integer instruction in the integer issue queue that is issuable
		@param integerRs a reservation station pointer
	*/
	void NextIssuableInteger(OoOQueuePointer<ReservationStation<T, nSources>, IntegerIssueQueueSize>& integerRs)
	{
		if(integerRs)
		{
			do
			{
				integerRs++;
			} while(integerRs && !Issuable(integerRs));
		}
	}

	/** Search for the next floating point instruction in the floating point issue queue that will be issuable
		@param floating pointRs a reservation station pointer
	*/
	void NextFloatingPointThatWillBeIssuable(OoOQueuePointer<ReservationStation<T, nSources>, FloatingPointIssueQueueSize>& floatingPointRs)
	{
		if(floatingPointRs)
		{
			do
			{
				floatingPointRs++;
			} while(floatingPointRs && !WillBeIssuable(floatingPointRs));
		}
	}

	/** Search for the next floating point instruction in the floating point issue queue that is issuable
		@param floating pointRs a reservation station pointer
	*/
	void NextIssuableFloatingPoint(OoOQueuePointer<ReservationStation<T, nSources>, FloatingPointIssueQueueSize>& floatingPointRs)
	{
		if(floatingPointRs)
		{
			do
			{
				floatingPointRs++;
			} while(floatingPointRs && !Issuable(floatingPointRs));
		}
	}

	/** Search for the next load/store instruction in the load/store issue queue that will be issuable
		@param load/storeRs a reservation station pointer
	*/
	void NextLoadStoreThatWillBeIssuable(OoOQueuePointer<ReservationStation<T, nSources>, LoadStoreIssueQueueSize>& loadStoreRs)
	{
		if(loadStoreRs)
		{
			do
			{
				loadStoreRs++;
			} while(loadStoreRs && !WillBeIssuable(loadStoreRs));
		}
	}

	/** Search for the next load/store instruction in the load/store issue queue that is issuable
		@param load/storeRs a reservation station pointer
	*/
	void NextIssuableLoadStore(OoOQueuePointer<ReservationStation<T, nSources>, LoadStoreIssueQueueSize>& loadStoreRs)
	{
		if(loadStoreRs)
		{
			do
			{
				loadStoreRs++;
			} while(loadStoreRs && !Issuable(loadStoreRs));
		}
	}

	/** Check if instruction is issuable
		@param rs a reservation station
		@return true if it is issuable
	*/
	bool Issuable(ReservationStation<T, nSources> *rs)
	{
		/* If it is already marked as issuable return true */
		if(rs->issuable) return true;
		/* If it is ready, make it issuable and return true */
		if(rs->ready)
		{
			rs->issuable = true;
			return true;
		}
		int i;
		/* For each source operand */
		for(i = 0; i < rs->instruction->sources.size(); i++)
		{
			/* Get the tag of the source operand */
			int tag = rs->instruction->sources[i].tag;
			if(tag >= 0)
			{
				/* If operand is not ready */
				if(!rs->sourceValid[i])
				{
					/* Check if operand is being produced */
					switch(rs->instruction->sources[i].type)
					{
						case GPR_T:
							if(integerRegisterTimeStamp[tag] >= time_stamp) return false;
							break;

						case FPR_T:
							if(floatingPointRegisterTimeStamp[tag] >= time_stamp) return false;
							break;

						case CR_T:
							if(conditionRegisterTimeStamp[tag] >= time_stamp) return false;
							break;
						case FPSCR_T:
							if(FPSCRegisterTimeStamp[tag] >= time_stamp) return false;
							break;
						case LR_T:
							if(linkRegisterTimeStamp[tag] >= time_stamp) return false;
							break;
						case CTR_T:
							if(countRegisterTimeStamp[tag] >= time_stamp) return false;
							break;
						case XER_T:
							if(XERRegisterTimeStamp[tag] >= time_stamp) return false;
							break;

						default: ;
					}
				}
			}
		}
		/* Make the instruction issuable */
		rs->issuable = true;
		/* and return true */
		return true;
	}

	/** Selects the oldest instructions
		@param a an instruction
		@param b an instruction
		@param c an instruction
		@return 0 if a is the oldest, 1 if b is the oldest, 2 if c is the oldest, -1 if there is no instructions
	*/
	int Select(const InstructionPtr *a, const InstructionPtr *b, const InstructionPtr *c)
	{
		if(a)
		{
			if(b)
			{
				if(c)
				{
					if(*a < *b)
					{
						if(*a < *c)
						{
							return 0;
						}
						else
						{
							return 2;
						}
					}
					else
					{
						if(*b < *c)
						{
							return 1;
						}
						else
						{
							return 2;
						}
					}
				}
				else
				{
					if(*a < *b)
					{
						return 0;
					}
					else
					{
						return 1;
					}
				}
			}
			else
			{
				if(c)
				{
					if(*a < *c)
					{
						return 0;
					}
					else
					{
						return 2;
					}
				}
				else
				{
					return 0;
				}
			}
		}
		else
		{
			if(b)
			{
				if(c)
				{
					if(*b < *c)
					{
						return 1;
					}
					else
					{
						return 2;
					}
				}
				else
				{
					return 1;
				}
			}
			else
			{
				if(c)
				{
					return 2;
				}
				else
				{
					return -1;
				}
			}
		}
		abort();
	}

	/** Returns the latency of an instruction
		@param instruction an instruction
		@return the latency
	*/
	int GetLatency(const InstructionPtr &instruction)
	{
		int latency = 0;
		int i;
		/* Sum the latency in each pipe stage of the operation */
		/* DD May be better ...
		for(i = 0; i < instruction->operation->nstages; i++)
			latency += instruction->operation->latencies[i];
		*/
		latency = instruction->operation->latency;
		/* and return the latency */
		return latency;
	}

	/** Make an instruction issued, and monitor when result will be produced
		@param instruction an instruction
	*/
	void Issued_Old(const InstructionPtr instruction)
	{
	  // DD we assume that the main destination which determine the pipeline flow in the first (destinations[0])...
		/* Get the tag of the destination operand */
	  
	  //		int tag = instruction->destinations[0].tag;
	  //		if(tag >= 0)
	  if (instruction->destinations.size()>0)
	  {
	    for (int j=0; j <instruction->destinations.size(); j++)
	    {
	      int tag = instruction->destinations[j].tag;
	      switch(instruction->destinations[j].type)
	      {
	      case GPR_T:
		/* If instruction is a load then instruction depending
		   on that result will be scheduled only when it will be actually written back */
		if(instruction->fn & FnLoad)
		  {
		    integerRegisterTimeStamp[tag] = -1; // inf
		  }
		else
		  {
		    /* Get the instruction latency */
		    int latency = GetLatency(instruction);
		    //if(latency <= 0 && !instruction->execution_serialized)
		    if(latency <= 0 && !instruction->execution_serialized)
		      {
			cerr << "instruction (" << instruction << ") has a bad latency (<= 0)" << endl;
			cerr << "Nstages: " << instruction->operation->nstages << endl;
			cerr << "Latency: " << latency << endl;
			cerr << "Lat[]=(";
			for (int i=0; i<8; i++)
			  cerr  << instruction->operation->latencies[i] << ", ";
			cerr << ")" << endl;
			cerr << "Inst: " << *instruction << endl;
			//abort();
			exit(-1);
		      }
		    /*
		      cerr << "Inst: " << *instruction << endl;
		      cerr << "Latency: " << latency << endl; 
		      cerr << "Lat[]=(";
		      for (int i=0; i<8; i++)
		      cerr  << instruction->operation->latencies[i] << ", ";
		      cerr << ")" << endl;
		    */
		    /* Records when the operand will be produced */
		    integerRegisterTimeStamp[tag] = time_stamp + latency;
		  }
		break;
		
	      case FPR_T:
		/* If instruction is a load then instruction depending
		   on that result will be scheduled only when it will be actually written back */
		if(instruction->fn & FnLoad)
		  {
		    floatingPointRegisterTimeStamp[tag] = -1; // inf
		  }
		else
		  {
		    /* Get the instruction latency */
		    int latency = GetLatency(instruction);
		    if(latency <= 0 && !instruction->execution_serialized)
		      {
			cerr << "instruction (" << instruction << ") has a bad latency (<= 0)" << endl;
			abort();
		      }
		    /* Records when the operand will be produced */
		    floatingPointRegisterTimeStamp[tag] = time_stamp + latency;
		  }
		break;
		
	      case CR_T:
		/* If instruction is a load then instruction depending
		   on that result will be scheduled only when it will be actually written back */
		if(instruction->fn & FnLoad)
		  {
		    conditionRegisterTimeStamp[tag] = -1; // inf
		  }
		else
		  {
		    /* Get the instruction latency */
		    int latency = GetLatency(instruction);
		    if(latency <= 0 && !instruction->execution_serialized)
		      {
			cerr << "instruction (" << instruction << ") has a bad latency (<= 0)" << endl;
			abort();
		      }
		    /* Records when the operand will be produced */
		    conditionRegisterTimeStamp[tag] = time_stamp + latency;
		  }
		break;
		
	      case FPSCR_T:
		/* If instruction is a load then instruction depending
		   on that result will be scheduled only when it will be actually written back */
		if(instruction->fn & FnLoad)
		  {
		    FPSCRegisterTimeStamp[tag] = -1; // inf
		  }
		else
		  {
		    /* Get the instruction latency */
		    int latency = GetLatency(instruction);
		    if(latency <= 0 && !instruction->execution_serialized)
		      {
			cerr << "instruction (" << instruction << ") has a bad latency (<= 0)" << endl;
			abort();
		      }
		    /* Records when the operand will be produced */
		    FPSCRegisterTimeStamp[tag] = time_stamp + latency;
		  }
		break;
		
	      case LR_T:
		/* If instruction is a load then instruction depending
		   on that result will be scheduled only when it will be actually written back */
		if(instruction->fn & FnLoad)
		  {
		    linkRegisterTimeStamp[tag] = -1; // inf
		  }
		else
		  {
		    /* Get the instruction latency */
		    int latency = GetLatency(instruction);
		    if(latency <= 0 && !instruction->execution_serialized)
		      {
			cerr << "instruction (" << instruction << ") has a bad latency (<= 0)" << endl;
			abort();
		      }
		    /* Records when the operand will be produced */
		    linkRegisterTimeStamp[tag] = time_stamp + latency;
		  }
		break;
		
	      case CTR_T:
		/* If instruction is a load then instruction depending
		   on that result will be scheduled only when it will be actually written back */
		if(instruction->fn & FnLoad)
		  {
		    countRegisterTimeStamp[tag] = -1; // inf
		  }
		else
		  {
		    /* Get the instruction latency */
		    int latency = GetLatency(instruction);
		    if(latency <= 0 && !instruction->execution_serialized)
		      {
			cerr << "instruction (" << instruction << ") has a bad latency (<= 0)" << endl;
			abort();
		      }
		    /* Records when the operand will be produced */
		    countRegisterTimeStamp[tag] = time_stamp + latency;
		  }
		break;
		
	      case XER_T:
		/* If instruction is a load then instruction depending
		   on that result will be scheduled only when it will be actually written back */
		if(instruction->fn & FnLoad)
		  {
		    XERRegisterTimeStamp[tag] = -1; // inf
		  }
		else
		  {
		    /* Get the instruction latency */
		    int latency = GetLatency(instruction);
		    if(latency <= 0 && !instruction->execution_serialized)
		      {
			cerr << "instruction (" << instruction << ") has a bad latency (<= 0)" << endl;
			abort();
		      }
		    /* Records when the operand will be produced */
		    XERRegisterTimeStamp[tag] = time_stamp + latency;
		  }
		break;
		
	      default: ;
	      }
	    }
	  }
	}
	void Issued(const InstructionPtr &instruction)
	{
	  // DD we assume that the main destination which determine the pipeline flow in the first (destinations[0])...
		/* Get the tag of the destination operand */
	  
	  //		int tag = instruction->destinations[0].tag;
	  //		if(tag >= 0)
	  if (instruction->destinations.size()>0)
	  {
	    for (int j=0; j <instruction->destinations.size(); j++)
	    {
	      int tag = instruction->destinations[j].tag;
	      int RegTimeStamp = -1; 
	      int latency;
	      if (!(instruction->fn & FnLoad))
		{
		  latency = GetLatency(instruction);
		  RegTimeStamp = time_stamp + latency;
		  if(latency <= 0 && !instruction->execution_serialized)
		    {
		      cerr << "instruction (" << instruction << ") has a bad latency (<= 0)" << endl;
		      abort();
		    }
		}
	      
	      switch(instruction->destinations[j].type)
		{
		case GPR_T:
		  /* If instruction is a load then instruction depending
		     on that result will be scheduled only when it will be actually written back */
		  /*
		    if(latency <= 0 && !instruction->execution_serialized)
		      {
			cerr << "instruction (" << instruction << ") has a bad latency (<= 0)" << endl;
			cerr << "Nstages: " << instruction->operation->nstages << endl;
			cerr << "Latency: " << latency << endl;
			cerr << "Lat[]=(";
			for (int i=0; i<8; i++)
			  cerr  << instruction->operation->latencies[i] << ", ";
			cerr << ")" << endl;
			cerr << "Inst: " << *instruction << endl;
			//abort();
			exit(-1);
		      }
		  */
		    /* Records when the operand will be produced */
		  integerRegisterTimeStamp[tag] = RegTimeStamp;
		  break;
		
		case FPR_T:
		  /* If instruction is a load then instruction depending
		     on that result will be scheduled only when it will be actually written back */
		  /* Records when the operand will be produced */
		  floatingPointRegisterTimeStamp[tag] = RegTimeStamp;
		  break;
		  
		case CR_T:
		  /* If instruction is a load then instruction depending
		     on that result will be scheduled only when it will be actually written back */
		    /* Records when the operand will be produced */
		    conditionRegisterTimeStamp[tag] = RegTimeStamp;
		break;
		
		case FPSCR_T:
		  /* If instruction is a load then instruction depending
		     on that result will be scheduled only when it will be actually written back */
		  /* Records when the operand will be produced */
		    FPSCRegisterTimeStamp[tag] = RegTimeStamp;
		    break;
		    
		case LR_T:
		  /* If instruction is a load then instruction depending
		     on that result will be scheduled only when it will be actually written back */
		  /* Records when the operand will be produced */
		    linkRegisterTimeStamp[tag] = RegTimeStamp;
		    break;
		    
		case CTR_T:
		  /* If instruction is a load then instruction depending
		     on that result will be scheduled only when it will be actually written back */
		    /* Records when the operand will be produced */
		  countRegisterTimeStamp[tag] = RegTimeStamp;
		  break;
		  
		case XER_T:
		  /* If instruction is a load then instruction depending
		     on that result will be scheduled only when it will be actually written back */
		  /* Records when the operand will be produced */
		  XERRegisterTimeStamp[tag] = RegTimeStamp;
		  break;
		  
		default: ;
		}
	    }
	  }
	}
	// START OF CYCLE
	void start_of_cycle()
	  {
		OoOQueuePointer<ReservationStation<T, nSources>, IntegerIssueQueueSize> integerRs;
		OoOQueuePointer<ReservationStation<T, nSources>, FloatingPointIssueQueueSize> floatingPointRs;
		OoOQueuePointer<ReservationStation<T, nSources>, LoadStoreIssueQueueSize> loadStoreRs;

		int issuePort;
		int integerIssuePort;
		int floatingPointIssuePort;
		int loadStoreIssuePort;
		integer_to_remove = 0;
		floating_point_to_remove = 0;
		loadstore_to_remove = 0;
		//		allAcceptRecevied = false;
		WriteBacksProcessed = false;
		
		// At the beginning of the cycle, we don't send any signals 
		// because we don't know which instruction we have to send 
		// until the writeback complete ...
		//
		// So first signals are sent to next module into the onWriteBackEnable process...
		allAcceptReceived = false;

	  }// end of start_of_cycle...
  /*
  void onData()
  {
    if (allAcceptReceived)
      {
	bool areallknown(false);
	int integer_to_add;
	int floating_point_to_add;
	int loadstore_to_add;
	int writePort;

	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== onData ==== All Accept Received : YES !!!" << endl;
	
	for(int i = 0; i < nIntegerWritePorts; i++)
	  {
	    if(!inIntegerInstruction[i].data.known() || !areallknown) 
		    { areallknown=false; break; }
	  }
	for(int i = 0; i < nFloatingPointWritePorts; i++)
	  {
	    if(!inFloatingPointInstruction[i].data.known() || !areallknown) 
	      { areallknown=false; break; }
	  }
	for(int i = 0; i < nLoadStoreWritePorts; i++)
	  {
	    if(!inLoadStoreInstruction[i].data.known() || !areallknown) 
	      { areallknown=false; break; }
	  }
	
	
	if (areallknown)
	{
	  cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== onData ==== areallknown : YES !!!" << endl;

	  // Count the number of instruction to add into the integer issue queue //
	  for(integer_to_add = 0, writePort = 0; writePort < nIntegerWritePorts; writePort++)
	    {
	      if(!inIntegerInstruction[writePort].data.something()) break;
	      integer_to_add++;
	    }
	  
	  // Count the number of instruction to add into the floating point issue queue //
	  for(floating_point_to_add = 0, writePort = 0; writePort < nFloatingPointWritePorts; writePort++)
	    {
	      if(!inFloatingPointInstruction[writePort].data.something()) break;
	      floating_point_to_add++;
	    }
	  
	  // Count the number of instruction to add into the load/store issue queue //
	  for(loadstore_to_add = 0, writePort = 0; writePort < nLoadStoreWritePorts; writePort++)
	    {
	      if(!inLoadStoreInstruction[writePort].data.something()) break;
	      loadstore_to_add++;
	    }
	  
	  // Get the free space into the integer issue queue //
	  int integer_free_space = integerIssueQueue.FreeSpace();
	  
	  // Get the free space into the floating point issue queue //
	  int floating_point_free_space = floatingPointIssueQueue.FreeSpace();
	  
	  // Get the free space into the load/store issue queue //
	  int loadstore_free_space = loadStoreIssueQueue.FreeSpace();
	  
	  // The number of integer instructions to accept is the minimum of the number of integer instruction to add and the
	     sum of the free space into the integer issue queue and the number of integer instructions to remove //
	  int integer_to_accept = MIN(integer_free_space + integer_to_remove, integer_to_add);
	  
	  // The number of floating point instructions to accept is the minimum of the number of floating point instruction to add and the
	     sum of the free space into the floating point issue queue and the number of floating point instructions to remove //
	  int floating_point_to_accept = MIN(floating_point_free_space + floating_point_to_remove, floating_point_to_add);
	  
	  // The number of load/store instructions to accept is the minimum of the number of load/store instruction to add and the
	     sum of the free space into the load/store issue queue and the number of load/store instructions to remove //
	  int loadstore_to_accept = MIN(loadstore_free_space + loadstore_to_remove, loadstore_to_add);
	  
	  // Accept the integer instructions according to the number of accepted integer instructions //
	  for(writePort = 0; integer_to_accept > 0 && writePort < nIntegerWritePorts; writePort++)
	    {
	      inIntegerInstruction[writePort].accept = true;
	      integer_to_accept--;
	    }
	  // Reject all remaining integer instructions //
	  for(; writePort < nIntegerWritePorts; writePort++)
	    inIntegerInstruction[writePort].accept = false;
	  
	  // Accept the floating point instructions according to the number of accepted floating point instructions //
	  for(writePort = 0; floating_point_to_accept > 0 && writePort < nFloatingPointWritePorts; writePort++)
	    {
	      inFloatingPointInstruction[writePort].accept = true;
	      floating_point_to_accept--;
	    }
	  // Reject all remaining floating point instructions //
	  for(; writePort < nFloatingPointWritePorts; writePort++)
	    inFloatingPointInstruction[writePort].accept = false;
	  
	  // Accept the load/store instructions according to the number of accepted load/store instructions //
	  for(writePort = 0; loadstore_to_accept > 0 && writePort < nLoadStoreWritePorts; writePort++)
	    {
	      inLoadStoreInstruction[writePort].accept = true;
	      loadstore_to_accept--;
	    }
	  // Reject all remaining load/store instructions //
	  for(; writePort < nLoadStoreWritePorts; writePort++)
	    inLoadStoreInstruction[writePort].accept = false;
	  
	} // end of areallknown
      }// end of AllReceived
    else
      {
	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== onData ==== Not all Received : NO !!!" << endl;
      }
  } // end of onData...

  void onAccept()
	  {
	    bool areallknown(true);
	    for (int i=0; i<IntegerIssueWidth; i++)
	      {
		if (!outIntegerInstruction[i].accept.known())
		  {
		    areallknown = false;
		    break;
		  }
	      }
	    for (int i=0; i<FloatingPointIssueWidth; i++)
	      {
		if (!outFloatingPointInstruction[i].accept.known())
		  {
		    areallknown = false;
		    break;
		  }
	      }
	    for (int i=0; i<LoadStoreIssueWidth; i++)
	      {
		if (!outLoadStoreInstruction[i].accept.known())
		  {
		    areallknown = false;
		    break;
		  }
	      }
	    if (areallknown)
	      {
		for (int i=0; i<IntegerIssueWidth; i++)
		  {
		    //		    if (outIntegerInstruction[i].enable = outIntegerInstruction[i].accept) 
		    if (outIntegerInstruction[i].accept) 
		      {
			outIntegerInstruction[i].enable = true;
			integer_to_remove++;
		      }
		    else { outIntegerInstruction[i].enable = false; }
		  }
		for (int i=0; i<FloatingPointIssueWidth; i++)
		  {
		    //		    if (outFloatingPointInstruction[i].enable = outFloatingPointInstruction[i].accept)
		    if (outFloatingPointInstruction[i].accept)
		      {
			outFloatingPointInstruction[i].enable = true;
			floating_point_to_remove++;
		      }
		    else { outFloatingPointInstruction[i].enable = false; }
		  }
		for (int i=0; i<LoadStoreIssueWidth; i++)
		  {
		    //		    if (outLoadStoreInstruction[i].enable = outLoadStoreInstruction[i].accept)
		    if (outLoadStoreInstruction[i].accept)
		      {
			outLoadStoreInstruction[i].enable = true;
			loadstore_to_remove++;
		      }
		    else { outLoadStoreInstruction[i].enable = false; }
		  }
		allAcceptReceived = true;


	      }
	  }// end of onAccept...
*/

  void onDataAccept()
  {
    
    int integer_to_add;
    int floating_point_to_add;
    int loadstore_to_add;
    int writePort;
    
    //	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== onData ==== All Accept Received : YES !!!" << endl;
    bool areallknown(true);
    
    // Are Accept known ?
    for (int i=0; i<IntegerIssueWidth; i++)
      {
	if (!outIntegerInstruction[i].accept.known())
	  {
	    areallknown = false;
	    break;
	  }
      }
    for (int i=0; i<FloatingPointIssueWidth; i++)
      {
	if (!outFloatingPointInstruction[i].accept.known())
	  {
	    areallknown = false;
	    break;
	  }
      }
    for (int i=0; i<LoadStoreIssueWidth; i++)
      {
	if (!outLoadStoreInstruction[i].accept.known())
	  {
	    areallknown = false;
	    break;
	  }
      }
    // Are Data known ?
    for(int i = 0; i < nIntegerWritePorts; i++)
      {
	if(!inIntegerInstruction[i].data.known() || !areallknown) 
	  { areallknown=false; break; }
      }
    for(int i = 0; i < nFloatingPointWritePorts; i++)
      {
	if(!inFloatingPointInstruction[i].data.known() || !areallknown) 
	  { areallknown=false; break; }
      }
    for(int i = 0; i < nLoadStoreWritePorts; i++)
      {
	if(!inLoadStoreInstruction[i].data.known() || !areallknown)
	  { areallknown=false; break; }
      }
    
    
    // Are all known ?
    if (areallknown)
      {
#ifdef DD_DEBUG_SCHEDULER_VERB1
	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== onDataAccept ====   Areallknown? YES!!!" << endl;
#endif	
	// Accept Part...
	for (int i=0; i<IntegerIssueWidth; i++)
	  {
	    //		    if (outIntegerInstruction[i].enable = outIntegerInstruction[i].accept) 
	    if (outIntegerInstruction[i].accept) 
	      {
		outIntegerInstruction[i].enable = true;
			integer_to_remove++;
	      }
	    else { outIntegerInstruction[i].enable = false; }
	  }
	for (int i=0; i<FloatingPointIssueWidth; i++)
	  {
	    //		    if (outFloatingPointInstruction[i].enable = outFloatingPointInstruction[i].accept)
	    if (outFloatingPointInstruction[i].accept)
	      {
		outFloatingPointInstruction[i].enable = true;
		floating_point_to_remove++;
	      }
	    else { outFloatingPointInstruction[i].enable = false; }
	  }
	for (int i=0; i<LoadStoreIssueWidth; i++)
	  {
	    //		    if (outLoadStoreInstruction[i].enable = outLoadStoreInstruction[i].accept)
	    if (outLoadStoreInstruction[i].accept)
	      {
		outLoadStoreInstruction[i].enable = true;
		loadstore_to_remove++;
	      }
	    else { outLoadStoreInstruction[i].enable = false; }
	  }
	//	allAcceptReceived = true;
	
	// Data Part...
	//	  cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== onData ==== areallknown : YES !!!" << endl;
	
	// Count the number of instruction to add into the integer issue queue //
	for(integer_to_add = 0, writePort = 0; writePort < nIntegerWritePorts; writePort++)
	  {
	    if(!inIntegerInstruction[writePort].data.something()) break;
	    integer_to_add++;
	  }
	
	// Count the number of instruction to add into the floating point issue queue //
	for(floating_point_to_add = 0, writePort = 0; writePort < nFloatingPointWritePorts; writePort++)
	  {
	    if(!inFloatingPointInstruction[writePort].data.something()) break;
	    floating_point_to_add++;
	  }
	
	// Count the number of instruction to add into the load/store issue queue //
	for(loadstore_to_add = 0, writePort = 0; writePort < nLoadStoreWritePorts; writePort++)
	  {
	    if(!inLoadStoreInstruction[writePort].data.something()) break;
	    loadstore_to_add++;
	  }
	
#ifdef DD_DEBUG_SCHEDULER_VERB1
	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== onDataAccept ====   int_to_remove: "
	     << integer_to_remove << endl;
	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== onDataAccept ====   fp_to_remove : "
	     << floating_point_to_remove << endl;
	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== onDataAccept ====   ls_to_remove : "
	     << loadstore_to_remove << endl;

	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== onDataAccept ====   int_to_add: "
	     << integer_to_add << endl;
	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== onDataAccept ====   fp_to_add : "
	     << floating_point_to_add << endl;
	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== onDataAccept ====   ls_to_add : "
	     << loadstore_to_add << endl;
#endif
	
	// Get the free space into the integer issue queue //
	int integer_free_space = integerIssueQueue.FreeSpace();
	
	// Get the free space into the floating point issue queue //
	int floating_point_free_space = floatingPointIssueQueue.FreeSpace();
	
	// Get the free space into the load/store issue queue //
	int loadstore_free_space = loadStoreIssueQueue.FreeSpace();
	
	// The number of integer instructions to accept is the minimum of the number of integer instruction to add and the sum of the free space into the integer issue queue and the number of integer instructions to remove
	int integer_to_accept = MIN(integer_free_space + integer_to_remove, integer_to_add);
	
	// The number of floating point instructions to accept is the minimum of the number of floating point instruction to add and the sum of the free space into the floating point issue queue and the number of floating point instructions to remove
	int floating_point_to_accept = MIN(floating_point_free_space + floating_point_to_remove, floating_point_to_add);
	
	// The number of load/store instructions to accept is the minimum of the number of load/store instruction to add and the sum of the free space into the load/store issue queue and the number of load/store instructions to remove
	int loadstore_to_accept = MIN(loadstore_free_space + loadstore_to_remove, loadstore_to_add);
	
#ifdef DD_DEBUG_SCHEDULER_VERB1
	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== onDataAccept ====   int_to_accept: "
	     << integer_to_accept << endl;
	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== onDataAccept ====   fp_to_accept : "
	     << floating_point_to_accept << endl;
	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== onDataAccept ====   ls_to_accept : "
	     << loadstore_to_accept << endl;
#endif
	// Accept the integer instructions according to the number of accepted integer instructions
	for(writePort = 0; integer_to_accept > 0 && writePort < nIntegerWritePorts; writePort++)
	  {
	    inIntegerInstruction[writePort].accept = true;
	    integer_to_accept--;
	  }
	// Reject all remaining integer instructions //
	for(; writePort < nIntegerWritePorts; writePort++)
	  inIntegerInstruction[writePort].accept = false;
	
	// Accept the floating point instructions according to the number of accepted floating point instructions //
	for(writePort = 0; floating_point_to_accept > 0 && writePort < nFloatingPointWritePorts; writePort++)
	  {
	    inFloatingPointInstruction[writePort].accept = true;
	    floating_point_to_accept--;
	  }
	// Reject all remaining floating point instructions //
	for(; writePort < nFloatingPointWritePorts; writePort++)
	  inFloatingPointInstruction[writePort].accept = false;
	
	// Accept the load/store instructions according to the number of accepted load/store instructions //
	for(writePort = 0; loadstore_to_accept > 0 && writePort < nLoadStoreWritePorts; writePort++)
	  {
	    inLoadStoreInstruction[writePort].accept = true;
	    loadstore_to_accept--;
	  }
	// Reject all remaining load/store instructions //
	for(; writePort < nLoadStoreWritePorts; writePort++)
	  inLoadStoreInstruction[writePort].accept = false;
	
	
      }// end of if(areallknown)...
  }// end of onAccept...
  
  
  /*
    void onEnable()
    {
    
    }
  */

	// Warning !!! 
	// Flush have to be well handled by a combinatorial process
	// to correctly propagate the Flush signal to previous modules ...
	// TODO  in ALL MODULES !!!
  /*
	void onFlush()
	  {
	    // if we received a Flush then we accept it (sans broncher) ...
	    inFlush.accept = true;
	  }
  */
	void onFlushData()
	  {
	    //    inFlush.accept = inFlush.data.something();
	    if ( inFlush.data.something() )
	      {
		outFlush.data = inFlush.data;
	      }
	    else
	      {
		outFlush.data.nothing();
	      }
	  }
	void onFlushAccept()
	  {
	    inFlush.accept = outFlush.accept;
	  }
	void onFlushEnable()
	  {
	    outFlush.enable = inFlush.enable;
	  }

	void end_of_cycle()
	  {
		OoOQueuePointer<ReservationStation<T, nSources>, IntegerIssueQueueSize> integerRs;
		OoOQueuePointer<ReservationStation<T, nSources>, FloatingPointIssueQueueSize> floatingPointRs;
		OoOQueuePointer<ReservationStation<T, nSources>, LoadStoreIssueQueueSize> loadStoreRs;

		int issuePort;
		int integerIssuePort;
		int floatingPointIssuePort;
		int loadStoreIssuePort;

	    // First check if a flush occured ...
		/* Flush ? */
	    if(inFlush.enable && inFlush.data.something())
	    {
	      if(inFlush.data)
	      {
#ifdef DD_DEBUG_SCHEDULER_VERB1
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====  Flush !!!" << endl;
#endif
#ifdef DD_DEBUG_FLUSH
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====  Flush !!!" << endl;
#endif
		int i;
		/* Flush the integer issue queue */
		integerIssueQueue.Flush();
		
		/* Flush the floating point issue queue */
		floatingPointIssueQueue.Flush();
		
		/* Flush the load/store issue queue */
		loadStoreIssueQueue.Flush();
		
		/* Reset all output ports */
		// NO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		/*
		for(issuePort = 0; issuePort < IntegerIssueWidth; issuePort++)
		  {
		    outIntegerValid[issuePort] = false;
		    outIntegerEnable[issuePort] = false;
		  }
		for(issuePort = 0; issuePort < FloatingPointIssueWidth; issuePort++)
		  {
		    outFloatingPointValid[issuePort] = false;
		    outFloatingPointEnable[issuePort] = false;
		  }
		for(issuePort = 0; issuePort < LoadStoreIssueWidth; issuePort++)
		  {
		    outLoadStoreValid[issuePort] = false;
		    outLoadStoreEnable[issuePort] = false;
		  }
		for(writePort = 0; writePort < nIntegerWritePorts; writePort++)
		  {
		    outIntegerAccept[writePort] = false;
		  }
		for(writePort = 0; writePort < nFloatingPointWritePorts; writePort++)
		  {
		    outFloatingPointAccept[writePort] = false;
		  }
		for(writePort = 0; writePort < nLoadStoreWritePorts; writePort++)
		  {
		    outLoadStoreAccept[writePort] = false;
		  }
		*/
		// End of NO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		for(i = 0; i < nIntegerRegisters; i++)
		  integerRegisterTimeStamp[i] = 0;
		
		for(i = 0; i < nFloatingPointRegisters; i++)
		  floatingPointRegisterTimeStamp[i] = 0;
		
		time_stamp++;
		/* Stop the process */
		return;
	      }
	    }

#ifdef DD_DEBUG_SCHEDULER_VERB1
	    cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====   Start of IssuePipeline !!!" << endl;
	    cerr << integerIssueQueue << endl;
	    cerr << floatingPointIssueQueue << endl;
	    cerr << loadStoreIssueQueue << endl;
	    cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====   End of IssuePipeline !!!" << endl;
#endif
	    //	    if (WriteBacksProcessed)
	    //	      {
	    
	    // WriteBacksProcessed = true;
	    /* Remove instructions that are issuable and accepted */
	    // NO JUST ISSUE INSTRUCTIONS ...
	    /* Peek an issuable integer instruction */
	    integerRs = integerIssueQueue.SeekAtHead();
	    if(integerRs && !Issuable(integerRs)) NextIssuableInteger(integerRs);
	    
	    /* Peek an issuable floating point instruction */
	    floatingPointRs = floatingPointIssueQueue.SeekAtHead();
	    if(floatingPointRs && !Issuable(floatingPointRs)) NextIssuableFloatingPoint(floatingPointRs);
	    
	    /* Peek an issuable load/store instruction */
	    loadStoreRs = loadStoreIssueQueue.SeekAtHead();
	    if(loadStoreRs && !Issuable(loadStoreRs)) NextIssuableLoadStore(loadStoreRs);
	    
	    integerIssuePort = 0;
	    floatingPointIssuePort = 0;
	    loadStoreIssuePort = 0;
	    issuePort = 0;
	    int s;
	    
	    do
	      {
		/* Select which instruction is the oldest from these three instructions */
		s = Select((integerRs && integerIssuePort < IntegerIssueWidth) ? &integerRs->instruction : 0,
			   (floatingPointRs && floatingPointIssuePort < FloatingPointIssueWidth) ? &floatingPointRs->instruction : 0,
			   (loadStoreRs && loadStoreIssuePort < LoadStoreIssueWidth) ? &loadStoreRs->instruction : 0);
		
		switch(s)
		  {
		  case 0:
		  if (integerIssueBusy[integerIssuePort] == 0)
		    {
		    /* The integer instruction was scheduled */
		    //		    if(inIntegerInstruction[integerIssuePort].accept)
		    if(outIntegerInstruction[integerIssuePort].accept)
		      {
			/* Instruction has been accepted */
			/* Records when dependent instructions could be scheduled */
			Issued(integerRs->instruction);

			// DD if latency is greater than the pipeline depth then stall issue on this function unit
			// Here in this case: we assume that FU pipeline depth is 1.
			integerIssueBusy[integerIssuePort] = GetLatency(integerRs->instruction)-1 +1;
			

			/* Remove the instruction from the integer issue queue */
			integerIssueQueue.Remove(integerRs);
			/* Peek another issuable integer instruction */
			if(integerRs && !Issuable(integerRs)) NextIssuableInteger(integerRs);
			//			changed = true;
		      }
		    else
		      {
			/* Peek another issuable integer instruction */
			NextIssuableInteger(integerRs);
		      }
		    integerIssuePort++;
		    issuePort++;
		    }
		  else
		    {
		      integerIssuePort++;
		    }
		    break;
		  case 1:
		  if (floatingPointIssueBusy[floatingPointIssuePort] == 0)
		    {
		    /* The floating point instruction was scheduled */
		    //		    if(inFloatingPointInstruction[floatingPointIssuePort].accept)
		    if(outFloatingPointInstruction[floatingPointIssuePort].accept)
		      {
			/* Instruction has been accepted */
			/* Records when dependent instructions could be scheduled */
			Issued(floatingPointRs->instruction);

			// DD if latency is greater than the pipeline depth then stall issue on this function unit
			// Here in this case: we assume that FU pipeline depth is 1.
			floatingPointIssueBusy[floatingPointIssuePort] = GetLatency(floatingPointRs->instruction)-1 +1;

			/* Remove the instruction from the floating point issue queue */
			floatingPointIssueQueue.Remove(floatingPointRs);
			/* Peek another issuable floating point instruction */
			if(floatingPointRs && !Issuable(floatingPointRs)) NextIssuableFloatingPoint(floatingPointRs);
			//			changed = true;
		      }
		    else
		      {
			/* Peek another issuable floating point instruction */
			NextIssuableFloatingPoint(floatingPointRs);
		      }
		    floatingPointIssuePort++;
		    issuePort++;
		    }
		  else
		    {
		      floatingPointIssuePort++;
		    }
		    break;
		  case 2:
		  if (loadStoreIssueBusy[loadStoreIssuePort] == 0)
		    {
		    /* The load/store instruction was scheduled */
		    //		    if(inLoadStoreInstruction[loadStoreIssuePort].accept)
		    if(outLoadStoreInstruction[loadStoreIssuePort].accept)
		      {
			/* Instruction has been accepted */
			/* Records when dependent instructions could be scheduled */
			Issued(loadStoreRs->instruction);
			// DD if latency is greater than the pipeline depth then stall issue on this function unit
			// Here in this case: we assume that FU pipeline depth is 1.
			loadStoreIssueBusy[loadStoreIssuePort] = GetLatency(loadStoreRs->instruction)-1 +1;
			/* Remove the instruction from the load/store issue queue */
			loadStoreIssueQueue.Remove(loadStoreRs);
			/* Peek another issuable load/store instruction */
			if(loadStoreRs && !Issuable(loadStoreRs)) NextIssuableLoadStore(loadStoreRs);
			//			changed = true;
		      }
		    else
		      {
			/* Peek another issuable load/store instruction */
			NextIssuableLoadStore(loadStoreRs);
		      }
		    loadStoreIssuePort++;
		    issuePort++;
		    }
		  else
		    {
		      loadStoreIssuePort++;
		    }
		    break;
		  }
		/* Loop while there instruction and remaining issue ports */
	      } while(s >= 0 && issuePort < IssueWidth);
	    

	    for(int i = 0; i < IntegerIssueWidth; i++)
	      { if (integerIssueBusy[i]>0) integerIssueBusy[i]--; }
	    for(int i = 0; i < FloatingPointIssueWidth; i++)
	      { if(floatingPointIssueBusy[i]>0) floatingPointIssueBusy[i]--; }
	    for(int i = 0; i < LoadStoreIssueWidth; i++)
	      { if(loadStoreIssueBusy[i]>0) loadStoreIssueBusy[i]--; }

	    time_stamp++; // Do not move above ! be careful about side effects on function Issuable
	    
	    //	    int writeBackPort;
	    for(int writeBackPort = 0; writeBackPort < WriteBackWidth; writeBackPort++)
	      {
		if(inWriteBackInstruction[writeBackPort].enable)
		  {
		    InstructionPtr instruction = inWriteBackInstruction[writeBackPort].data;
		    //		    const Instruction *instruction = &inst;
		    
		    //		    if(instruction->destination.tag >= 0)
		    //		    if(instruction->destinations.size()>0)
		    for (int j=0; j<instruction->destinations.size(); j++)
		      {
			switch(instruction->destinations[j].type)
			  {
			  case GPR_T:
			    integerRegisterTimeStamp[instruction->destinations[j].tag] = time_stamp;
			    break;
			    
			  case FPR_T:
			    floatingPointRegisterTimeStamp[instruction->destinations[j].tag] = time_stamp;
			    break;
			    
			  case CR_T:
			    conditionRegisterTimeStamp[instruction->destinations[j].tag] = time_stamp;
			    break;
			  case FPSCR_T:
			    FPSCRegisterTimeStamp[instruction->destinations[j].tag] = time_stamp;
			    break;
			  case LR_T:
			    linkRegisterTimeStamp[instruction->destinations[j].tag] = time_stamp;
			    break;
			  case CTR_T:
			    countRegisterTimeStamp[instruction->destinations[j].tag] = time_stamp;
			    break;
			  case XER_T:
			    XERRegisterTimeStamp[instruction->destinations[j].tag] = time_stamp;
			    break;
			    
			  default:
			    // TODO NOW...
			    cerr << "register is neither an integer nor a floating point register" << endl;
			    abort();
			  }
		      }
		  }
	      }
	    
	
	    /* Add instructions into the integer issue queue */
	    for(int writePort = 0; writePort < nIntegerWritePorts; writePort++)
	      {
		/* Is there an instruction to write ? */
		if(!inIntegerInstruction[writePort].enable) break;
		/* Get the instruction */
		InstructionPtr instruction = inIntegerInstruction[writePort].data;
		//		const Instruction *instruction = &inst;
		
		/* Allocate a reservation station in the integer issue queue */
		ReservationStation<T, nSources> *new_rs = integerIssueQueue.New();
		
		if(new_rs)
		  {
		    /* Initialize fields of the reservation station */
		    new_rs->Initialize(instruction);
		    
		    /* Get the tag of destination operand */
		    //		    int tag = instruction->destinations[0].tag;
		    //		    if(tag >= 0)
		    //		    if (instruction->destinations.size()>0)
		    for (int j=0; j<instruction->destinations.size(); j++)
		      {
			int tag = instruction->destinations[j].tag;
			/* Make dependent instructions wait */
			if(tag >= 0)
			switch(instruction->destinations[j].type)
			  {
			  case GPR_T:
			    integerRegisterTimeStamp[tag] = (UInt64) -1; /* inf */
			    break;
			  case FPR_T:
			    floatingPointRegisterTimeStamp[tag] = (UInt64) -1;	/* inf */
			    break;
			  case CR_T:
			    conditionRegisterTimeStamp[tag] = (UInt64) -1; /* inf */
			    break;
			  case FPSCR_T:
			    FPSCRegisterTimeStamp[tag] = (UInt64) -1; /* inf */
			    break;
			  case LR_T:
			    linkRegisterTimeStamp[tag] = (UInt64) -1; /* inf */
			    break;
			  case CTR_T:
			    countRegisterTimeStamp[tag] = (UInt64) -1; /* inf */
			    break;
			  case XER_T:
			    XERRegisterTimeStamp[tag] = (UInt64) -1; /* inf */
			    break;
			  default:
			    cerr << "register is neither an integer nor a floating point register" << endl;
			    abort();
			  }
		      }
		    
		    /* Update the reservation station if needed */
		    Update(new_rs);
		    //		    changed = true;
		  }
		else
		  {
		    /* Throw an error if there is not enough space */
		    cerr << "time stamp = " << timestamp() << endl;
		    cerr << "overflow into integer issue queue " << name() << endl;
		    abort();
		  }
	      }
	    
	    /* Add instructions into the floating point issue queue */
	    for(int writePort = 0; writePort < nFloatingPointWritePorts; writePort++)
	      {
		/* Is there an instruction to write ? */
		if(!inFloatingPointInstruction[writePort].enable) break;
		/* Get the instruction */
		InstructionPtr instruction = inFloatingPointInstruction[writePort].data;
		//		const Instruction *instruction = &inst;
		
		/* Allocate a reservation station in the floating point issue queue */
		ReservationStation<T, nSources> *new_rs = floatingPointIssueQueue.New();
		
		if(new_rs)
		  {
		    /* Initialize fields of the reservation station */
		    new_rs->Initialize(instruction);
		    //		    if(tag >= 0)
		    //		    if (instruction->destinations.size()>0)
		    for(int j=0; j<instruction->destinations.size(); j++)
		      {
			/* Get the tag of destination operand */
			int tag = instruction->destinations[j].tag;
			/* Make dependent instructions wait */
			switch(instruction->destinations[j].type)
			  {
			  case GPR_T:
			    integerRegisterTimeStamp[tag] = (UInt64) -1;
			    break;
			  case FPR_T:
			    floatingPointRegisterTimeStamp[tag] = (UInt64) -1;
			    break;
			  case CR_T:
			    conditionRegisterTimeStamp[tag] = (UInt64) -1;
			    break;
			  case FPSCR_T:
			    FPSCRegisterTimeStamp[tag] = (UInt64) -1;
			    break;
			  case LR_T:
			    linkRegisterTimeStamp[tag] = (UInt64) -1;
			    break;
			  case CTR_T:
			    countRegisterTimeStamp[tag] = (UInt64) -1;
			    break;
			  case XER_T:
			    XERRegisterTimeStamp[tag] = (UInt64) -1;
			    break;
			  default:
			    cerr << "register is neither an integer nor a floating point register" << endl;
			    abort();
			  }
		      }
		    /* Update the reservation station if needed */
		    Update(new_rs);
		    //		    changed = true;
		  }
		else
		  {
		    /* Throw an error if there is not enough space */
		    cerr << "time stamp = " << timestamp() << endl;
		    cerr << "overflow into floating point issue queue " << name() << endl;
		    abort();
		  }
	      }
	    
	    /* Add instructions into the load/store issue queue */
	    for(int writePort = 0; writePort < nLoadStoreWritePorts; writePort++)
	      {
		/* Is there an instruction to write ? */
		if(!inLoadStoreInstruction[writePort].enable) break;
		/* Get the instruction */
		InstructionPtr instruction = inLoadStoreInstruction[writePort].data;
		//		const Instruction *instruction = &inst;
		
		/* Allocate a reservation station in the load/store issue queue */
		ReservationStation<T, nSources> *new_rs = loadStoreIssueQueue.New();
		
		if(new_rs)
		  {
		    /* Initialize fields of the reservation station */
		    new_rs->Initialize(instruction);
		    /* Get the tag of destination operand */
		    //		    int tag = instruction->destinations[0].tag;
		    //		    if(tag >= 0)
		    //		    if (instruction->destinations.size()>0)
		    for(int j=0; j<instruction->destinations.size(); j++)
		      {
			int tag = instruction->destinations[j].tag;
			/* Make dependent instructions wait */
			switch(instruction->destinations[j].type)
			  {
			  case GPR_T:
			    integerRegisterTimeStamp[tag] = (UInt64) -1;
			    break;
			  case FPR_T:
			    floatingPointRegisterTimeStamp[tag] = (UInt64) -1;
			    break;
			  case CR_T:
			    conditionRegisterTimeStamp[tag] = (UInt64) -1;
			    break;
			  case FPSCR_T:
			    FPSCRegisterTimeStamp[tag] = (UInt64) -1;
			    break;
			  case LR_T:
			    linkRegisterTimeStamp[tag] = (UInt64) -1;
			    break;
			  case CTR_T:
			    countRegisterTimeStamp[tag] = (UInt64) -1;
			    break;
			  case XER_T:
			    XERRegisterTimeStamp[tag] = (UInt64) -1;
			    break;
			  default:
			    cerr << "register is neither an integer nor a floating point register" << endl;
			    abort();
			  }
		      }
		    /* Update the reservation station if needed */
		    Update(new_rs);
		    //		    changed = true;
		  }
		else
		  {
		    /* Throw an error if there is not enough space */
		    cerr << "time stamp = " << timestamp() << endl;
		    cerr << "overflow into load/store issue queue " << name() << endl;
		    abort();
		  }
	      }

#ifdef DD_DEBUG_PIPELINE_VERB1
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ==== Pipeline Debug" << endl;
		cerr << this << endl;
#endif
	    
	  }// end of end_of_cycle...

	void onWriteBackData()
	  {
	    bool areallknown(true);
	    // are WriteBack signals known ?
	    for(int writeBackPort = 0; writeBackPort < WriteBackWidth; writeBackPort++)
	      { if (!inWriteBackInstruction[writeBackPort].data.known())
		{ areallknown = false; break; }
	      }
	    if (areallknown)
	      {
		for(int writeBackPort = 0; writeBackPort < WriteBackWidth; writeBackPort++)
		  { 
		    /*
		    if (inWriteBackInstruction[writeBackPort].data.something())
		      outWriteBackInstruction[writeBackPort].data=inWriteBackInstruction[writeBackPort].data;
		    else
		      outWriteBackInstruction[writeBackPort].data.nothing();
		    */
		    if (inWriteBackInstruction[writeBackPort].data.something())
		      { 
			inWriteBackInstruction[writeBackPort].accept = true;
		      }
		    else
		      { 
			inWriteBackInstruction[writeBackPort].accept = false;
		      }
		  }
	      }
	  }

  /*
	void onWriteBackAccept()
	  {
	    bool areallknown(true);
	    // are WriteBack signals known ?
	    for(int writeBackPort = 0; writeBackPort < WriteBackWidth; writeBackPort++)
	      { //if (!outWriteBackInstruction[writeBackPort].accept.known())
		{ areallknown = false; break; }
	      }
	    if (areallknown)
	      {
		for(int writeBackPort = 0; writeBackPort < WriteBackWidth; writeBackPort++)
		  { 
		    //    inWriteBackInstruction[writeBackPort].accept = outWriteBackInstruction[writeBackPort].accept;
		    //		    inWriteBackInstruction[writeBackPort].accept = inWriteBackInstruction[writeBackPort].data.something();
		  }
	      }
	  }
  */
	void onWriteBackEnable()
	  {
	    OoOQueuePointer<ReservationStation<T, nSources>, IntegerIssueQueueSize> integerRs;
	    OoOQueuePointer<ReservationStation<T, nSources>, FloatingPointIssueQueueSize> floatingPointRs;
	    OoOQueuePointer<ReservationStation<T, nSources>, LoadStoreIssueQueueSize> loadStoreRs;

		int issuePort;
		int integerIssuePort;
		int floatingPointIssuePort;
		int loadStoreIssuePort;


	    bool areallknown(true);
	    // are WriteBack signals known ?
	    for(int writeBackPort = 0; writeBackPort < WriteBackWidth; writeBackPort++)
	      { if (!inWriteBackInstruction[writeBackPort].enable.known())
		{ areallknown = false; break; }
	      }
	    if (areallknown)
	      {
		/* Update the reservation stations */
		/* Each write back port */
		InstructionPtr instruction;
		for(int writeBackPort = 0; writeBackPort < WriteBackWidth; writeBackPort++)
		  { 
		    if (inWriteBackInstruction[writeBackPort].enable)
		      {
			//	outWriteBackInstruction[writeBackPort].enable=true;
			/* Get the instruction */
			instruction = inWriteBackInstruction[writeBackPort].data;
			//			const Instruction *instruction = &inst;
			
			/* Does that instruction produce a result ? */
			//			if(instruction->destination.tag >= 0)
			if (instruction->destinations.size()>0)
			  {
			    /* For each reservation in the integer issue queue */
			    for(integerRs = integerIssueQueue.SeekAtHead(); integerRs; integerRs++)
			      {
				/* Update the reservation station */
				if(!integerRs->ready)
				  integerRs->Update(instruction, false);
			      }
			    /* For each reservation in the floating point issue queue */
			    for(floatingPointRs = floatingPointIssueQueue.SeekAtHead(); floatingPointRs; floatingPointRs++)
			      {
				/* Update the reservation station */
				if(!floatingPointRs->ready)
				  floatingPointRs->Update(instruction, false);
			      }
			    /* For each reservation in the load/store issue queue */
			    for(loadStoreRs = loadStoreIssueQueue.SeekAtHead(); loadStoreRs; loadStoreRs++)
			      {
				/* Update the reservation station */
				if(!loadStoreRs->ready)
				  loadStoreRs->Update(instruction, false);
			      }
			  }
		      }
		    else
		      {
			//outWriteBackInstruction[writeBackPort].enable=false;			
		      }
		  }

		/* Schedule the instructions */
		/* Peek an issuable integer instruction */
		integerRs = integerIssueQueue.SeekAtHead();
		if(integerRs && !WillBeIssuable(integerRs)) NextIntegerThatWillBeIssuable(integerRs);

		/* Peek an issuable floating point instruction */
		floatingPointRs = floatingPointIssueQueue.SeekAtHead();
		if(floatingPointRs && !WillBeIssuable(floatingPointRs)) NextFloatingPointThatWillBeIssuable(floatingPointRs);

		/* Peek an issuable load/store instruction */
		loadStoreRs = loadStoreIssueQueue.SeekAtHead();
		if(loadStoreRs && !WillBeIssuable(loadStoreRs)) NextLoadStoreThatWillBeIssuable(loadStoreRs);

		integerIssuePort = 0;
		floatingPointIssuePort = 0;
		loadStoreIssuePort = 0;
		issuePort = 0;
		int s;
		do
		{
			/* Select which instruction is the oldest from these three instructions */
			s = Select((integerRs && integerIssuePort < IntegerIssueWidth) ? &integerRs->instruction : 0,
					(floatingPointRs && floatingPointIssuePort < FloatingPointIssueWidth) ? &floatingPointRs->instruction : 0,
					(loadStoreRs && loadStoreIssuePort < LoadStoreIssueWidth) ? &loadStoreRs->instruction : 0);

			switch(s)
			{
			case 0:
			       /* The integer instruction has been selected */
			       /* Request the issue of the instruction */
			  if (integerIssueBusy[integerIssuePort] == 0)
			    {
			       outIntegerInstruction[integerIssuePort].data = integerRs->instruction;
			       integerIssuePort++;
			       issuePort++;
			       /* Peek another issuable integer instruction */
			       NextIntegerThatWillBeIssuable(integerRs);
			    }
			  else
			    {
			      outIntegerInstruction[integerIssuePort].data.nothing();
			      integerIssuePort++;			      
			    }
			       break;
			       
			case 1:
			      /* The floating point instruction has been selected */
			      /* Request the issue of the instruction */
			  if (floatingPointIssueBusy[floatingPointIssuePort] == 0)
			    {
			      outFloatingPointInstruction[floatingPointIssuePort].data = floatingPointRs->instruction;
			      floatingPointIssuePort++;
			      issuePort++;
			      /* Peek another issuable floating point instruction */
			      NextFloatingPointThatWillBeIssuable(floatingPointRs);
			    }
			  else
			    {
			      outFloatingPointInstruction[floatingPointIssuePort].data.nothing();
			      floatingPointIssuePort++;			      
			    }
			      break;

			case 2:
			      /* The load/store instruction has been selected */
			      /* Request the issue of the instruction */
			  if (loadStoreIssueBusy[loadStoreIssuePort] == 0)
			    {
			      outLoadStoreInstruction[loadStoreIssuePort].data = loadStoreRs->instruction;
			      loadStoreIssuePort++;
			      issuePort++;
			      /* Peek another issuable load/store instruction */
			      NextLoadStoreThatWillBeIssuable(loadStoreRs);
			    }
			  else
			    {
			      outLoadStoreInstruction[loadStoreIssuePort].data.nothing();
			      loadStoreIssuePort++;			      
			    }
			      break;
			}
			/* loop while there is enough issue ports and there are some issuable instructions */
		} while(s >= 0 && issuePort < IssueWidth);

		/* Reset the remaining integer issue ports */
		while(integerIssuePort < IntegerIssueWidth)
		{
		        outIntegerInstruction[integerIssuePort].data.nothing();
			integerIssuePort++;
		}
		/* Reset the remaining floating point issue ports */
		while(floatingPointIssuePort < FloatingPointIssueWidth)
		{
		        outFloatingPointInstruction[floatingPointIssuePort].data.nothing();
			floatingPointIssuePort++;
		}
		/* Reset the remaining load/store issue ports */
		while(loadStoreIssuePort < LoadStoreIssueWidth)
		{
		        outLoadStoreInstruction[loadStoreIssuePort].data.nothing();
			loadStoreIssuePort++;
		}


	      }
	    else
	      {
#ifdef DD_DEBUG_SCHEDULER_VERB1
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== onWriteBackEnable ==== areallknown : NO !!!" << endl;
#endif
	      }
	  }

	/** Performs sanity checks */
	bool Check()
	{
		return integerIssueQueue.Check() && floatingPointIssueQueue.Check() && loadStoreIssueQueue.Check();
	}

  
  void _print(ostream& os) const
  {
		os << "============ Integer Issue Queue =============" << endl;
		os << integerIssueQueue;
		os << "=========================================" << endl;
		os << "integerBusy: ";
		for (int i=0; i<IntegerIssueWidth; i++) 
		  { os << "  iuB[" << i << "]="<<integerIssueBusy[i]; }
		os << endl;
		os << "============ Floating Point Issue Queue =============" << endl;
		os << floatingPointIssueQueue;
		os << "=========================================" << endl;
		os << "floatingPointBusy: ";
		for (int i=0; i<FloatingPointIssueWidth; i++) 
		  { os << " fpuB[" << i << "] "<<floatingPointIssueBusy[i]; }
		os << endl;
		os << "============ Load/Store Issue Queue =============" << endl;
		os << loadStoreIssueQueue;
		os << "=========================================" << endl;
		os << "loadStoreBusy: ";
		for (int i=0; i<LoadStoreIssueWidth; i++) 
		  { os << "  lsB[" << i << "] "<<loadStoreIssueBusy[i]; }
		os << endl;
  
		/*
		int i;
		const int nRegistersPerLine = 8;
		for(i = 0; i < nIntegerRegisters; i++)
		{
			os << "r" << i << ":" << scheduler.integerRegisterTimeStamp[i];
			if((i % nRegistersPerLine) == nRegistersPerLine - 1) os << endl; else os << "  ";
		}
		*/
  
		os << endl;
		os << "=========================================" << endl;
		//		return os;
  }
  
	/** Print the scheduler state into an output stream
		@param os an output stream
		@param scheduler a schedule module
		@return the output stream
	*/
  friend ostream& operator << (ostream& os, const Scheduler& scheduler)
  {
    scheduler._print(os);
    return os;
  }
    
/*
	friend ostream& operator << (ostream& os, const Scheduler& scheduler)

	{
		os << "============ Integer Issue Queue =============" << endl;
		os << scheduler.integerIssueQueue;
		os << "=========================================" << endl;
		os << "integerBusy: ";
		for (int i=0; i<scheduler.IntegerIssueWidth; i++) 
		  { os << "  iuB[" << i << "]="<<scheduler.integerIssueBusy[i]; }
		os << endl;
		os << "============ Floating Point Issue Queue =============" << endl;
		os << scheduler.floatingPointIssueQueue;
		os << "=========================================" << endl;
		os << "floatingPointBusy: ";
		for (int i=0; i<scheduler.FloatingPointIssueWidth; i++) 
		  { os << " fpuB[" << i << "] "<<scheduler.floatingPointIssueBusy[i]; }
		os << endl;
		os << "============ Load/Store Issue Queue =============" << endl;
		os << scheduler.loadStoreIssueQueue;
		os << "=========================================" << endl;
		os << "loadStoreBusy: ";
		for (int i=0; i<scheduler.LoadStoreIssueWidth; i++) 
		  { os << "  lsB[" << i << "] "<<scheduler.loadStoreIssueBusy[i]; }
		os << endl;
		os << endl;
		os << "=========================================" << endl;
		return os;
	}
*/

private:
	OoOQueue<ReservationStation<T, nSources>, IntegerIssueQueueSize> integerIssueQueue;		/* A queue of reservation stations */
	OoOQueue<ReservationStation<T, nSources>, FloatingPointIssueQueueSize> floatingPointIssueQueue;		/* A queue of reservation stations */
	OoOQueue<ReservationStation<T, nSources>, LoadStoreIssueQueueSize> loadStoreIssueQueue;		/* A queue of reservation stations */

	UInt64 time_stamp;											/*< the current date */
	UInt64 integerRegisterTimeStamp[nIntegerRegisters];			/*< when dependent instruction could be scheduled ? */
	UInt64 floatingPointRegisterTimeStamp[nIntegerRegisters];	/*< when dependent instruction could be scheduled ? */

	UInt64 conditionRegisterTimeStamp[nConditionRegisters];
	UInt64 FPSCRegisterTimeStamp[nFPSCRegisters];
	UInt64 linkRegisterTimeStamp[nLinkRegisters];
	UInt64 countRegisterTimeStamp[nCountRegisters];
	UInt64 XERRegisterTimeStamp[nXERRegisters];
	/* Some ports to make the SystemC scheduler call the ExternalControl process on state changes */
	/*
	ml_out_data<bool> outStateChanged;
	ml_signal_data<bool> stateChanged;
	ml_in_data<bool> inStateChanged;
	*/

	int integer_to_remove;
	int floating_point_to_remove;
	int loadstore_to_remove;
	
	bool allAcceptReceived;
	bool WriteBacksProcessed;

  int integerIssueBusy[IntegerIssueWidth];
  int floatingPointIssueBusy[FloatingPointIssueWidth];
  int loadStoreIssueBusy[LoadStoreIssueWidth];

};

} // end of namespace issue
} // end of namespace pipeline
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
