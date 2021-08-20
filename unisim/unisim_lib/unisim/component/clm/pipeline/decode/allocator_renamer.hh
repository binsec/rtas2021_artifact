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
                        AllocatorRenamer.h  -  description
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_DECODE_ALLOCATOR_RENAMER_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_DECODE_ALLOCATOR_RENAMER_HH__

#include <unisim/component/clm/processor/ooosim/parameters.hh>
#include <unisim/component/clm/interfaces/instruction_interface.hh>

namespace unisim {
namespace component {
namespace clm {
namespace pipeline {
namespace decode {


using unisim::component::clm::processor::ooosim::nConditionRegisters;
using unisim::component::clm::processor::ooosim::nFPSCArchitecturalRegisters;
using unisim::component::clm::processor::ooosim::nFPSCRegisters;
using unisim::component::clm::processor::ooosim::nLinkArchitecturalRegisters;
using unisim::component::clm::processor::ooosim::nLinkRegisters;
using unisim::component::clm::processor::ooosim::nCountArchitecturalRegisters;
using unisim::component::clm::processor::ooosim::nCountRegisters;
using unisim::component::clm::processor::ooosim::nXERArchitecturalRegisters;
using unisim::component::clm::processor::ooosim::nXERRegisters;
 
using unisim::component::clm::interfaces::InstructionPtr;

  using unisim::kernel::Object;
  using unisim::kernel::variable::Statistic;

/* A mapping table */
template <int nArchitecturalRegisters, int nRegisters>
class MappingTable
{
public:
	/** Constructor of the mapping table */
	MappingTable()
	{
		Reset();
	}
	
	void Reset()
	{
		int i;
		
		while(freeList.Size() > 0)
		{
			int dummy;
			freeList >> dummy;
		}
		/* initialize the mapping table and the free list */
		/* Map architectural register i to physical register i */
		for(i = 0; i < nArchitecturalRegisters; i++)
		{
			mapping[i] = i;
			table[i].valid = true;
			table[i].committed = true;
			table[i].reg = i;
			table[i].prev = -1;
		}
		/* Add all remaining physical register into the free list */
		for(; i < nRegisters; i++)
		{
			table[i].valid = false;
			table[i].committed = false;
			table[i].reg = -1;
			table[i].prev = -1;
			freeList << i;
		}
	}

	/** Reads last committed the physical register tag on which an architectural register is mapped
		@param reg the number of the architectural register
		@return the physical register tag */
	int Read(int reg)
	{
		/* Get the tag of the physical register */
		int tag = mapping[reg];

		/* Get the tag of the previous physical register on which the architectural register was mapped */
		int prev = table[tag].prev;

		/* Search for the last committed physical register which mapped the arcitectural register */
		bool committed = table[tag].committed;
		if(!committed)
		{
			if(prev >= 0)
			{
				do
				{
					tag = prev;
					committed = table[tag].committed;
					prev = table[tag].prev;
				} while(!committed && prev >= 0);
			}
		}

		if(!committed)
		{
			/* Throw an error if no committed physical register was found */
			cerr << "register " << reg << " is not hold by any physical register" << endl;
			exit(-1);
		}
		return tag;
	}

	/** Allocates a physical register and map an architectural register on it
		@param reg the number of the architectural register
		@return the physical register tag */
	int Allocate(int reg)
	{
		int tag;

		//		cerr << "["<<"("<<timestamp()<<")] ==== ALLOCATE ==== FreeList before : " << freeList;
		//		cerr << endl;
		/* Get a tag from the free list */
		freeList >> tag;

		/* Records into a table the history of the allocations in order to do a rollback if necessary */
		table[tag].valid = false;	/* data was not yet produced */
		table[tag].committed = false;	/* data was not yet committed */
		table[tag].prev = mapping[reg];	/* Remember which physical register previously mapped the architecural register */
		table[tag].reg = reg;  	/* Remember which architectural register is mapped on this physical register */

		/* Map the architectural register on the physical register */
		mapping[reg] = tag;

		//		cerr << "["<<"("<<timestamp()<<")] ==== ALLOCATE ==== FreeList after : " << freeList;
		//		cerr << endl;
		/* Return the physical register tag */
		return tag;
	}

	/** Renames an architectural register
		@param reg the number of the architectural register
		@return the tag of physical register on which the architectural register is mapped */
	int Rename(int reg)
	{
		/* Reads the tag by indexing the mapping table whith the number of the architectural register */
		int tag = mapping[reg];
		/* Returns the physical register tag */
		return tag;
	}

	/** Make the data of the physical register valid
		@param tag the tag of the physical register */
	void Validate(int tag)
	{
		table[tag].valid = true;
	}

	/** Commits a physical register
		@param tag the tag of the physical register */
	void Commit(int tag)
	{
		int prev = table[tag].prev;
		if(table[prev].committed)
		{
			/* Release the previous physical register which mapped the architectural register */
			table[prev].reg = -1;
			table[prev].valid = false;
			table[prev].committed = false;
			freeList << prev;
		}
		else
		{
			cerr << "time stamp = " << timestamp() << endl;
			cerr << "previous commited register doesn't exist" << endl;
			cerr << "Tag: " << tag << ",  Prev: " << prev << endl;
			cerr << *this;
			//			exit(-1);
			abort();
		}
		table[tag].prev = -1;
		table[tag].committed = true;
		if(!table[tag].valid)
		{
			cerr << "time stamp = " << timestamp() << endl;
			cerr << "register " << tag << " is not valid but committed" << endl;
			exit(-1);
		}
	}

	/** Returns the valid bit of a physical register
		@param tag the tag of the physical register
		@return the valid bit
	*/
	bool IsValid(int tag)
	{
		return table[tag].valid;
	}
	
	bool IsCommitted(int tag)
	{
		return table[tag].committed;
	}

	/** Performs a rollback on the mapping table */
	void Rollback()
	{
		int reg;

		for(reg = 0; reg < nArchitecturalRegisters; reg++)
		{
			int tag = mapping[reg];

			if(!table[tag].committed)
			{
				/* Search for the last committed physical register */
				do
				{
					if(table[tag].reg != reg)
					{
						cerr << "inconsistency into the mapping table" << endl;
						exit(-1);
					}
					int prev = table[tag].prev;
					if(prev < 0)
					{
						cerr << "inconsistency into the mapping table" << endl;
						exit(-1);
					}
					freeList << tag;
					table[tag].prev = -1;
					table[tag].reg = -1;
					tag = prev;
				} while(!table[tag].committed);
				mapping[reg] = tag;
			}
		}
	}

	/** Returns the free space in the mapping table
		@returns the free space in the mapping table */
	int GetFreeSpace()
	{
		return freeList.Size();
	}

	/** Print the mapping table content on an output stream
		@param os an output stream
		@param mt a mapping table
		@returns the output stream
	*/
	friend ostream& operator << (ostream& os, const MappingTable<nArchitecturalRegisters, nRegisters>& mt)
	{
		int i;
		const int nArchitecturalRegistersPerLine = 8;
		for(i = 0; i < nArchitecturalRegisters; i++)
		{
			os << i << "->" << mt.mapping[i] << "(" << (mt.table[mt.mapping[i]].valid ? "valid" : "not valid") << "," << (mt.table[mt.mapping[i]].committed ? "committed":"not committed") << ")";
			if((i % nArchitecturalRegistersPerLine) == nArchitecturalRegistersPerLine - 1) os << endl; else os << "  ";
		}
		os << endl;
		const int nRegistersPerLine = 8;
		for(i = 0; i < nRegisters; i++)
		{
			os << i << ":reg=" << mt.table[i].reg << ",valid=" << mt.table[i].valid << ",committed=" << mt.table[i].committed << ",prev=" << mt.table[i].prev;
			if((i % nRegistersPerLine) == nRegistersPerLine - 1) os << endl; else os << "  ";
		}
		return os;
	}
private:
	/*       mapping
           +--------+
           |        |
           |        |
           +--------+
    reg -->|  tag --|-----------+
           +--------+           |
           |        |           |
           |        |           |
           +--------+           |
                                |
           +--------------------|------+
           |         table      |      |
           |                    V      |
           |                  +----+   |
           |            prev  |    |   |
           |              +---|--  |   |
           |              |   +----+   |
           |              V            |
           |            +----+         |
           |      prev  |    |         |
           |       +----|--  |         |
           |       |    +----+         |
           |       V                   |
           |     +----+                |
           |     |    |                |
           |     |    |                |
           |     +----+                |
           +---------------------------+
	*/
	int mapping[nArchitecturalRegisters];	/*< mapping of architectural registers on the physical registers */
	struct
	{
		bool valid;	/*< true if the register contains a valid value */
		bool committed;	/*< true if the value into the register has been committed */
		int reg;	/*< architectural register the physical register hold */
		int prev;	/*< previous physical register holding the architectural register (to restore the mapping) */
	} table[nRegisters];	/*< A table for maintaining an history of modification on the mapping table */
	Queue<int, nRegisters> freeList;	/*< physical registers free list */
};

/** A Rename pipeline entry */
template <class T, int nSources>
class RenamePipelineEntry
{
public:
	int delay;		/*< a processing delay */
	//	InstructionPtr<T, nSources> instruction;	/*< the instruction being processed */
	InstructionPtr instruction;	/*< the instruction being processed */

	RenamePipelineEntry()
	{
		Reset();
	}
	
	void Reset()
	{
		delay = 0;
		//		instruction.Reset();
	}
	
	/** Print the pipeline entry into an output stream
		@param os an output stream
		@param entry a pipeline entry
		@returns the output stream
	*/
	friend ostream& operator << (ostream& os, const RenamePipelineEntry& entry)
	{
		os << "delay=" << entry.delay << ",instruction=" << entry.instruction;
		return os;
	}

	/** Compares two pipeline entries
		@param entry the second pipeline entry
		@returns non-zero if different
	*/
	int operator == (const RenamePipelineEntry& entry)
	{
		/* Compare the instruction inum in the two pipeline entries */
		return entry.instruction->inum == instruction->inum;
	}
};

/* A SystemC module allocating reorder buffer entries and physical register and renaming source operands */
template <class T, int nSources, int nIntegerArchitecturalRegisters, int nFloatingPointArchitecturalRegisters, int nIntegerRegisters, int nFloatingPointRegisters, int Width, int WriteBackWidth, int RetireWidth, int ReorderBufferSize, int nStages>
class AllocatorRenamer : public module, public Object
{
public:
	/* Clock */
	inclock inClock;

	/* From the fetcher */
	inport<InstructionPtr> inInstruction[Width];

	/* To the issue queue and the reorder buffer */
	// To the issue queue ...
	outport<InstructionPtr> outInstructionIssue[Width];
	// To the reorder buffer ... (for allocation) 
	outport<InstructionPtr> outInstructionReorder[Width];

	/* from the issue queues */
	/* From the reorder buffer */
	/* To the load/store queue */
	outport<InstructionPtr> outStoreInstruction[Width];
	outport<InstructionPtr> outLoadInstruction[Width];

	/* From the register file */
	inport<InstructionPtr> inWriteBackInstruction[WriteBackWidth];

	/* From the reorder buffer */
	inport<InstructionPtr> inRetireInstruction[RetireWidth];

	inport<bool> inFlush;
  	outport<bool> outFlush;


  uint64_t allocator_no_enough_resources;
  uint64_t stall_sched;
  uint64_t stall_reord;
  uint64_t stall_loadq;
  uint64_t stall_storq;
  
  Statistic<uint64_t> stat_allocator_no_enough_resources;
  Statistic<uint64_t> stat_stall_sched;
  Statistic<uint64_t> stat_stall_reord;
  Statistic<uint64_t> stat_stall_loadq;
  Statistic<uint64_t> stat_stall_storq;

	/* Constructor */
	AllocatorRenamer(const char *name) : module(name)
					   , Object(name)
					   , allocator_no_enough_resources(0)
					   , stall_sched(0)
					   , stall_reord(0)
					   , stall_loadq(0)
					   , stall_storq(0)
					   , stat_allocator_no_enough_resources("stat_allocator_no_enough_resources",this,allocator_no_enough_resources)
					   , stat_stall_sched("stall_sched",this,stall_sched)
					   , stat_stall_reord("stall_reord",this,stall_reord)
					   , stat_stall_loadq("stall_loadq",this,stall_loadq)
					   , stat_stall_storq("stall_storq",this,stall_storq)
	{
		int i, j;

		class_name = " AllocatorRenamerClass";
		// Unisim port names ...
		for (i=0; i<Width; i++)
		  {
		    inInstruction[i].set_unisim_name(this,"inInstruction",i);
		  }		
		for (i=0; i<Width; i++)
		  {
		    outInstructionIssue[i].set_unisim_name(this,"outInstructionIssue",i);
		  }
		for (i=0; i<Width; i++)
		  {
		    outInstructionReorder[i].set_unisim_name(this,"outInstructionReorder",i);
		  }
		for (i=0; i<Width; i++)
		  {
		    outStoreInstruction[i].set_unisim_name(this,"outStoreInstruction",i);
		  }
		for (i=0; i<Width; i++)
		  {
		    outLoadInstruction[i].set_unisim_name(this,"outLoadInstruction",i);
		  }
		for (i=0; i<WriteBackWidth; i++)
		  {
		    inWriteBackInstruction[i].set_unisim_name(this,"inWriteBackInstruction",i);
		  }
		for (i=0; i<RetireWidth; i++)
		  {
		    inRetireInstruction[i].set_unisim_name(this,"inRetireInstruction",i);
		  }
		inFlush.set_unisim_name(this,"inFlush");
		outFlush.set_unisim_name(this,"outFlush");

		/*
		outStateChanged(stateChanged);
		inStateChanged(stateChanged);
		*/
		//		SC_HAS_PROCESS(AllocatorRenamer);

		/*
		SC_METHOD(ExternalControl);
		sensitive << inStateChanged;
		for(i = 0; i < Width; i++)
		{
			sensitive << inValid[i] << inAccept[i] << inReorderBufferAccept[i] << inLoadAccept[i] << inStoreAccept[i];
		}

		SC_METHOD(InternalControl);
		sensitive_pos << inClock;
		*/
		sensitive_pos_method(start_of_cycle) << inClock;
		sensitive_neg_method(end_of_cycle) << inClock;

		for(i = 0; i < Width; i++)
		{
		  // sensitive << inValid[i] << inAccept[i] << inReorderBufferAccept[i] << inLoadAccept[i] << inStoreAccept[i];
		  sensitive_method(onAccept) << inInstruction[i].data << outInstructionIssue[i].accept << outInstructionReorder[i].accept << outLoadInstruction[i].accept << outStoreInstruction[i].accept;
		  //sensitive_method(onAccept) << outInstruction[i].accept << outLoadInstruction[i].accept << outStoreInstruction[i].accept;
		  //sensitive_method(onData) << inInstruction[i].data;
		}

		for(i=0; i< WriteBackWidth; i++)
		  {
		    sensitive_method(onWriteBackData) << inWriteBackInstruction[i].data; 
		  }		

		for(i=0; i< RetireWidth; i++)
		  {
		    sensitive_method(onRetireData) << inRetireInstruction[i].data; 
		  }
 
		sensitive_method(onFlushData) << inFlush.data;
		sensitive_method(onFlushAccept) << outFlush.accept;
		sensitive_method(onFlushEnable) << inFlush.enable;

		robHead = -1;
		robTail = -1;
		robSize = 0;
		lock = false;
		//		changed = true;
		// --- Latex rendering hints -----------------

		for(int i=0;i<Width-1;i++)
		{ 
		  inInstruction[i].set_fused();
		}
		latex_left_ports.push_back(&inInstruction[Width-1]);
		
		/*
		for(int i=0;i<Width-1;i++)
		{ 
		  outInstruction[i].set_fused();
		}
		latex_right_ports.push_back(&outInstruction[Width-1]);
		*/
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

  void onWriteBackData()
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
		  if (inWriteBackInstruction[i].data.something())
		    {
		      inWriteBackInstruction[i].accept = true;
		    }
		  else
		    {
		      inWriteBackInstruction[i].accept = false;
		    }
		}
	    }
  }

  void onRetireData()
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
		  if (inRetireInstruction[i].data.something())
		    {
#ifdef DD_DEBUG_SIGNALS
		cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: OnRetireData... >>> " << endl;
		cerr << "Send inRetire.accept = True"<< endl; 
#endif	
		      inRetireInstruction[i].accept = true;
		    }
		  else
		    {
#ifdef DD_DEBUG_SIGNALS
		cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: OnRetireData... >>> " << endl;
		cerr << "Send inRetire.accept = False"<< endl; 
#endif	
		      inRetireInstruction[i].accept = false;
		    }
		}
	    }
  }



/** Compute the number of instruction to enable
	@return the number of instruction to enable
*/
	int Enabled()
	{
		int renamePort, loadPort, storePort;
		int enabled = 0;
		QueuePointer<RenamePipelineEntry<T, nSources>, nStages * Width> renamePipelineEntry;

		/* For each rename pipeline entry which processing is finished, enable instructions that got an accept */
		for(renamePipelineEntry = renamePipeline.SeekAtHead(), renamePort = loadPort = storePort = 0; renamePipelineEntry && renamePort < Width; renamePort++, renamePipelineEntry++)
		{
			/* Is processing finished ? */
			if(renamePipelineEntry->delay > 0) break;

			/* Get the instruction in the rename pipeline entry */
			//const InstructionPtr<T, nSources>& instruction = renamePipelineEntry->instruction;
			const InstructionPtr instruction = renamePipelineEntry->instruction;

			/* We consider that we have an accept if we got an accept from both the issue queues and the reorder buffer.
			If instruction is a load/prefetch load, we need an accept from the load queue
			If instruction is a store, we need an accept from the store queue */
			/*
			bool accept = inAccept[renamePort] &&
			              inReorderBufferAccept[renamePort] &&
			              (!(instruction->fn & (FnLoad | FnPrefetchLoad)) || inLoadAccept[loadPort]) &&
			              (!(instruction->fn & FnStore) || inStoreAccept[storePort]);
			*/
			bool accept = 
			  outInstructionIssue[renamePort].accept &&
			  outInstructionReorder[renamePort].accept &&
			  (!(instruction->fn & (FnLoad | FnPrefetchLoad)) || outLoadInstruction[loadPort].accept) &&
			  (!(instruction->fn & FnStore) || outStoreInstruction[storePort].accept);

			/*
			if (instruction->fn & FnStore)
			  {
			    cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== ALLOCATE ==== (Enable) Inst: "<< *instruction << endl; 
			    if (!outInstructionIssue[renamePort].accept)
			      cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== ALLOCATE ==== (Enable) Out Issue Not Accept !!! " << endl;
			    if (!outInstructionReorder[renamePort].accept)
			      cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== ALLOCATE ==== (Enable) Out Reorder Not Accept !!! " << endl;
			    if (!outStoreInstruction[renamePort].accept)
			      cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== ALLOCATE ==== (Enable) Out StoreQ Not Accept !!! " << endl;
			    
			  }
			*/
			/* We stop enabling instructions if we got a reject */
			if(!accept) break;

			/* Enable the instruction on the issue queues and the reorder buffer */
			outInstructionIssue[renamePort].enable = true;
			outInstructionReorder[renamePort].enable = true;

			/* Count the instruction as enabled */
			enabled++;

			/* Enable the load/prefetch load on the load queue */
			if(instruction->fn & (FnLoad | FnPrefetchLoad))
				outLoadInstruction[loadPort++].enable = true;
			/* Enable the store on the store queue */
			if(instruction->fn & FnStore)
				outStoreInstruction[storePort++].enable = true;
		}
		/* Don't enable instruction on the remaining ports */
		/*
		for(; renamePort < Width; renamePort++)
			outEnable[renamePort] = false;
		for(; loadPort < Width; loadPort++)
			outLoadEnable[loadPort] = false;
		for(; storePort < Width; storePort++)
			outStoreEnable[storePort] = false;
		*/
		for(; renamePort < Width; renamePort++)
		  {
		    outInstructionIssue[renamePort].enable = false;
		    outInstructionReorder[renamePort].enable = false;
		  }
		for(; loadPort < Width; loadPort++)
			outLoadInstruction[loadPort].enable = false;
		for(; storePort < Width; storePort++)
			outStoreInstruction[storePort].enable = false;
		
		/* Return the number of enabled instructions */
		return enabled;
	}

	/** Allocate a reorder buffer tag and a physical registers for the destination of an instruction
		@param instruction an instruction
	*/
	//	void Allocate(InstructionPtr<T, nSources>& instruction)
	void Allocate(InstructionPtr *instruction)
	{
	  /* Reorder buffer tag are allocated in a first in/first out manner */
	  robTail = robTail < ReorderBufferSize - 1 ? robTail + 1 : 0;
	  /* Allocate a reorder buffer tag */
	  (*instruction)->tag = robTail;

	  /* If reorder buffer was empty, update the head pointer */
	  if (robHead < 0) robHead = robTail;
	  robSize++;
	  
	  /* Allocate a physical register tag for each destination operands */
	  for (int i=0; i<(*instruction)->destinations.size(); i++)
	    {
	      switch((*instruction)->destinations[i].type)
		{
		        case GPR_T:
				/* if destination register is an integer register, allocate an integer register */
				(*instruction)->destinations[i].tag = integerMappingTable.Allocate((*instruction)->destinations[i].reg);
				break;

			case FPR_T:
				/* if destination register is a floating point register, allocate a floating point register */
				(*instruction)->destinations[i].tag = floatingPointMappingTable.Allocate((*instruction)->destinations[i].reg);
				break;
		        case CR_T:
				/* if destination register is an condition register, allocate an integer register */
				(*instruction)->destinations[i].tag = conditionMappingTable.Allocate((*instruction)->destinations[i].reg);
				break;
		        case FPSCR_T:
				/* if destination register is an FPSCR register, allocate an FPSCR register */
				(*instruction)->destinations[i].tag = FPSCRMappingTable.Allocate((*instruction)->destinations[i].reg);
				break;
		        case LR_T:
				/* if destination register is an link register, allocate an link register */
				(*instruction)->destinations[i].tag = linkMappingTable.Allocate((*instruction)->destinations[i].reg);
				break;
		        case CTR_T:
				/* if destination register is an count register, allocate an count register */
				(*instruction)->destinations[i].tag = countMappingTable.Allocate((*instruction)->destinations[i].reg);
				break;
		        case XER_T:
				/* if destination register is an XER register, allocate an XER register */
				(*instruction)->destinations[i].tag = XERMappingTable.Allocate((*instruction)->destinations[i].reg);
				break;


				// TODOTODOTODO
				//			case CONDITIONAL_REGISTER:
				/* if destination register is an integer register, allocate an integer register */
				// DD TODO !!!
				//	(*instruction)->destination.tag = conditionalFieldMappingTable.Allocate((*instruction)->destination.reg);
				//				break;


			default: 
			  //cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== ALLOCATE ==== Invalid Destination Type !!!!!!!!!!!!!!" << endl;
			  //exit(-1);
			  break;
		}
		/*
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== ALLOCATE ==== " << endl;
		cerr << "["<<this->name()<<"("<<timestamp()<<")] inst       : "<< *instruction << endl;
		cerr << "["<<this->name()<<"("<<timestamp()<<")] dest.tag   : "<< (*instruction)->destination.tag << endl;
		*/
	    }
	}
	/** Rename sources of an instruction
		@param instruction an instruction
	*/
	//	void Rename(InstructionPtr<T, nSources>& instruction)
	void Rename(InstructionPtr *instruction)
	{
		int i;

		/* Rename each source operand by reading into the mapping tables
		   the tag of the physical register corresponding the architural register */
		//		for(i = 0; i < nSources; i++)
		for(i = 0; i < (*instruction)->sources.size(); i++)
		{
			/* Get the architectural register number of the source operand */
			int reg = (*instruction)->sources[i].reg;

			if(reg >= 0 && (*instruction)->sources[i].tag < 0)
			{
				switch((*instruction)->sources[i].type)
				{
					case GPR_T:
						/* If source operand is an integer register, then read the integer mapping table */
						(*instruction)->sources[i].tag = integerMappingTable.Rename(reg);
						(*instruction)->sources[i].ready = integerMappingTable.IsValid((*instruction)->sources[i].tag) || integerMappingTable.IsCommitted((*instruction)->sources[i].tag);
						break;

					case FPR_T:
						/* If source operand is a floating point register, then read the floating point mapping table */
						(*instruction)->sources[i].tag = floatingPointMappingTable.Rename(reg);
						(*instruction)->sources[i].ready = floatingPointMappingTable.IsValid((*instruction)->sources[i].tag) || floatingPointMappingTable.IsCommitted((*instruction)->sources[i].tag);
						break;

					case CR_T:
						/* If source operand is an condition register, then read the condition mapping table */
						(*instruction)->sources[i].tag = conditionMappingTable.Rename(reg);
						(*instruction)->sources[i].ready = conditionMappingTable.IsValid((*instruction)->sources[i].tag) || conditionMappingTable.IsCommitted((*instruction)->sources[i].tag);
						break;
					case FPSCR_T:
						/* If source operand is an FPSCR register, then read the FPSCR mapping table */
						(*instruction)->sources[i].tag = FPSCRMappingTable.Rename(reg);
						(*instruction)->sources[i].ready = FPSCRMappingTable.IsValid((*instruction)->sources[i].tag) || FPSCRMappingTable.IsCommitted((*instruction)->sources[i].tag);
						break;

					case LR_T:
						/* If source operand is an link register, then read the link mapping table */
						(*instruction)->sources[i].tag = linkMappingTable.Rename(reg);
						(*instruction)->sources[i].ready = linkMappingTable.IsValid((*instruction)->sources[i].tag) || linkMappingTable.IsCommitted((*instruction)->sources[i].tag);
						break;

					case CTR_T:
						/* If source operand is an count register, then read the count mapping table */
						(*instruction)->sources[i].tag = countMappingTable.Rename(reg);
						(*instruction)->sources[i].ready = countMappingTable.IsValid((*instruction)->sources[i].tag) || countMappingTable.IsCommitted((*instruction)->sources[i].tag);
						break;

					case XER_T:
						/* If source operand is an XER register, then read the XER mapping table */
						(*instruction)->sources[i].tag = XERMappingTable.Rename(reg);
						(*instruction)->sources[i].ready = XERMappingTable.IsValid((*instruction)->sources[i].tag) || XERMappingTable.IsCommitted((*instruction)->sources[i].tag);
						break;


					default: ;
				}
			}
		}
	}

	/** Perform changes on the mapping tables following an instruction retirement
		@param retiredInstruction a retired instruction */
	//	void Retire(const InstructionPtr<T, nSources>& retiredInstruction)
	void Retire(const InstructionPtr *retiredInstruction)
	{
#ifdef DD_DEBUG_ALLOCATOR_RETIRE_VERB1
		    cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== RETIRE ==== !!!" << endl;
		    cerr << *retiredInstruction << endl;
#endif
		if(!(*retiredInstruction)->replay_trap)
		{
			/* Free the reorder buffer tag */
			/* As reorder buffer tag are allocated in a first in/first out manner, we just move the head of the fifo */
			robHead = robHead < ReorderBufferSize - 1 ? robHead + 1 : 0;
			robSize--;
	
			/* Commit the destination operand */
			for (int i=0; i<(*retiredInstruction)->destinations.size(); i++)
			{
			  switch((*retiredInstruction)->destinations[i].type)
			  {
				case GPR_T:
					/* If destination operand is an integer register, then commit the register in the integer mapping table */
					integerMappingTable.Commit((*retiredInstruction)->destinations[i].tag);
					break;
	
				case FPR_T:
					/* If destination operand is a floating point register, then commit the register in the floating point mapping table */
					floatingPointMappingTable.Commit((*retiredInstruction)->destinations[i].tag);
					break;
				case CR_T:
					/* If destination operand is an condition register, then commit the register in the condition mapping table */
					conditionMappingTable.Commit((*retiredInstruction)->destinations[i].tag);
					break;
				case FPSCR_T:
					/* If destination operand is an FPSCR register, then commit the register in the FPSCR mapping table */
					FPSCRMappingTable.Commit((*retiredInstruction)->destinations[i].tag);
					break;
				case LR_T:
					/* If destination operand is an link register, then commit the register in the link mapping table */
					linkMappingTable.Commit((*retiredInstruction)->destinations[i].tag);
					break;
				case CTR_T:
					/* If destination operand is an count register, then commit the register in the count mapping table */
					countMappingTable.Commit((*retiredInstruction)->destinations[i].tag);
					break;
				case XER_T:
					/* If destination operand is an XER register, then commit the register in the XER mapping table */
					XERMappingTable.Commit((*retiredInstruction)->destinations[i].tag);
					break;
	
				default: ;
			  }
			}
			/* If instruction was execution serialized then clear the lock bit to allow new instruction to be dispatched */
			if((*retiredInstruction)->execution_serialized) lock = false;
		}
	}
	
	/** Perform chnages on the mapping table following an instruction write back
		@param writeBackInstruction instruction that was written back */
	//	void WriteBack(const InstructionPtr<T, nSources>& writeBackInstruction)
	void WriteBack(const InstructionPtr *writeBackInstruction)
	{
	  for (int i=0; i<(*writeBackInstruction)->destinations.size(); i++)
	  {
		/* Get the physical register tag of the destination operand */
		int tag = (*writeBackInstruction)->destinations[i].tag;

		if(tag >= 0)
		{
			switch((*writeBackInstruction)->destinations[i].type)
			{
				case GPR_T:
					/* If physical register is an integer register, make it valid into the integer mapping table */
					integerMappingTable.Validate(tag);
					break;

				case FPR_T:
					/* If physical register is a floating point register, make it valid into the floating point mapping table */
					floatingPointMappingTable.Validate(tag);
					break;
				case CR_T:
					/* If physical register is an condition register, make it valid into the condition mapping table */
					conditionMappingTable.Validate(tag);
					break;
				case FPSCR_T:
					/* If physical register is an FPSCR register, make it valid into the FPSCR mapping table */
					FPSCRMappingTable.Validate(tag);
					break;
				case LR_T:
					/* If physical register is an link register, make it valid into the link mapping table */
					linkMappingTable.Validate(tag);
					break;
				case CTR_T:
					/* If physical register is an count register, make it valid into the count mapping table */
					countMappingTable.Validate(tag);
					break;
				case XER_T:
					/* If physical register is an XER register, make it valid into the XER mapping table */
					XERMappingTable.Validate(tag);
					break;

				default: ;
			}
		}
		//  }
		QueuePointer<RenamePipelineEntry<T, nSources>, nStages * Width> renamePipelineEntry;

		/* For each rename pipeline entry, update valid bit for each source operands */
		for(renamePipelineEntry = renamePipeline.SeekAtHead(); renamePipelineEntry; renamePipelineEntry++)
		{
		  //	renamePipelineEntry->instruction->Validate(*writeBackInstruction);
		  renamePipelineEntry->instruction->Validate(*writeBackInstruction);
		}
	  }
	}
	/** Combinational process managing the valid, accept, enable hand-shaking */
	//	void ExternalControl()
	void onAccept()
	{
	  bool areallknown(true);
	  int i;
	  for (i=0;i<Width;i++)
	    {
	      areallknown &= inInstruction[i].data.known() &&
		outInstructionIssue[i].accept.known() &&
		outInstructionReorder[i].accept.known() &&
		outLoadInstruction[i].accept.known() &&
		outStoreInstruction[i].accept.known();
	    }
	  //	  sensitive_method(onAccept) << inInstruction[i].data << outInstruction[i].accept << outLoadInstruction[i].accept << outStoreInstruction[i].accept;
	  if (areallknown)
	    {
		int renamePort, rename_to_add;

		/* Count the number of instruction to add into the rename pipeline */
		for(rename_to_add = 0; rename_to_add < Width; rename_to_add++)
		  //if(!inValid[rename_to_add]) break;
			if(!inInstruction[rename_to_add].data.something()) break;

		/* Get the number of instructions to enable */
		int rename_to_remove = Enabled();
		//		cerr << "    DDDD Enabled: " << rename_to_remove << endl;

		/* Get the free space into the rename pipeline */
		int rename_pipeline_free_space = renamePipeline.FreeSpace();

		/* The number of instructions to accept from the fetcher is the minimal of the number of instructions to add
		and the sum of the number of instructions to remove and the free space into the rename pipeline */
		int rename_to_accept = MIN(rename_pipeline_free_space + rename_to_remove, rename_to_add);

		// Compute requiered ressources
		int GPR_FreeSpace=integerMappingTable.GetFreeSpace();
		int FPR_FreeSpace=floatingPointMappingTable.GetFreeSpace();
		int CR_FreeSpace=conditionMappingTable.GetFreeSpace();
		int FPSCR_FreeSpace=FPSCRMappingTable.GetFreeSpace();
		int LR_FreeSpace=linkMappingTable.GetFreeSpace();
		int CTR_FreeSpace=countMappingTable.GetFreeSpace();
		int XER_FreeSpace=XERMappingTable.GetFreeSpace();
		bool enough_ressources=true;

		/* Get the free space into the reorder buffer */
		int robFreeSpace = ReorderBufferSize - robSize;

		if(robFreeSpace < rename_to_accept) rename_to_accept = robFreeSpace;

		//		for (int i=0; i<rename_to_accept && enought_ressources; i++)
		//		for(renamePort = 0; renamePort < rename_to_accept && enough_ressources; renamePort++)
		renamePort = 0;
		while ((renamePort < rename_to_accept) && enough_ressources)
		{
		    
		    InstructionPtr inInst = inInstruction[renamePort].data;

		    for (int j=0; j<inInst->destinations.size() && enough_ressources; j++)
		      {
			switch (inInst->destinations[j].type)
			  {
			  case GPR_T:
			    GPR_FreeSpace--; break;
			  case FPR_T:
			    FPR_FreeSpace--; break;
			  case CR_T:
			    CR_FreeSpace--; break;
			  case FPSCR_T:
			    FPSCR_FreeSpace--; break;
			  case LR_T:
			    LR_FreeSpace--; break;
			  case CTR_T:
			    CTR_FreeSpace--; break;
			  case XER_T:
			    XER_FreeSpace--; break;
			  default:
			    cerr << "Error: unknow destination register type..." << endl;
			    cerr << inInst;
			    abort();
			  }
			enough_ressources = ( (GPR_FreeSpace>=0) &&
					      (FPR_FreeSpace>=0) &&
					      (CR_FreeSpace>=0) &&
					      (FPSCR_FreeSpace>=0) &&
					      (LR_FreeSpace>=0) &&
					      (CTR_FreeSpace>=0) &&
					      (XER_FreeSpace>=0)
					      );
			if (!enough_ressources) break;
			
		      }
		    if (!enough_ressources) break;
		    
		    // Here we are sure that we have enought resources to allocate current instruction !!!
		    inInstruction[renamePort].accept = true;
		    renamePort++;

		  }
	
		/* Get the number of free integer physical registers */
		//		int integerFreeSpace = integerMappingTable.GetFreeSpace();

		/* Get the number of free floating point physical registers */
		//		int floatingPointFreeSpace = floatingPointMappingTable.GetFreeSpace();

		/* Get the free space into the reorder buffer */
		//		int robFreeSpace = ReorderBufferSize - robSize;

		/* The number of accepted instructions is the minimum of the three preceding values */
		/*
		if(integerFreeSpace < rename_to_accept) rename_to_accept = integerFreeSpace;
		if(floatingPointFreeSpace < rename_to_accept) rename_to_accept = floatingPointFreeSpace;
		if(robFreeSpace < rename_to_accept) rename_to_accept = robFreeSpace;
		*/
		/* Accept the instructions according to the number of accepted instructions */
		/*
		for(renamePort = 0; renamePort < rename_to_accept; renamePort++)
		  //outAccept[renamePort] = true;
		  inInstruction[renamePort].accept = true;
		*/
		/* Reject instructions on all remaining ports */
		for(; renamePort < Width; renamePort++)
		  //outAccept[renamePort] = false;
		  inInstruction[renamePort].accept = false;

	    }
	}

	/** Sequential process managing state changes on the rename pipeline and the mapping tables */
	//	void InternalControl()
	void end_of_cycle()
	{
#ifdef DD_DEBUG_SIGNALS
	cerr << "["<<this->name()<<"("<<timestamp()<<")] <<< DEBUG SIGNALS: Starting EOC >>> " << endl;
#endif	
		/* This is a boolean value indicating if change was performed on the module state */
		int i;

		//#ifdef DD_DEBUG_ALLOCATOR_VERB1
#ifdef DD_DEBUG_ALLOCATOR_RETIRE_VERB1
		    cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====  BEGIN !!!" << endl;
		for(i = 0; i < RetireWidth; i++)
		{
		  if (inRetireInstruction[i].data.something())
		    {
		      cerr << " Something : " << inRetireInstruction[i].data << endl;
		    }
		  else
		    {
		      cerr << " Nothing   : " << inRetireInstruction[i].data << endl;
		    }
		}
#endif
		// DD: TODO something on this if statement ...
		// DD: At end_of_cycle all signals have been received ...
		//if(!inLoadAccept[0] && !inEnable[0] && !inRetireEnable[0] && !inWriteBackEnable[0] && !inStoreAccept[0] && !inReorderBufferAccept[0] && !inAccept[0] && !changed) return;
		//		changed = false;

		/* For each write back port, performs some stuff for each written back instruction */
		for(i = 0; i < WriteBackWidth; i++)
		{
		  //			if(inWriteBackEnable[i])
		  if (inWriteBackInstruction[i].enable)
		    {
		      InstructionPtr tmpinst = inWriteBackInstruction[i].data;
		      WriteBack(&tmpinst);
		      //changed = true;
		    }
		}

		/* For each retire port, performs some stuff for each retired instruction */
		for(i = 0; i < RetireWidth; i++)
		{
			/* Terminate the loop as soon as possible */
		  //			if(!inRetireEnable[i]) break;
		  // DD : BUG : Comment nothing a pu etre Enabled ???
#ifdef DD_DEBUG_ALLOCATOR_RETIRE_VERB1
		  if ( (inRetireInstruction[i].enable) && !inRetireInstruction[i].data.something())
		    {
		      cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== ERROR Nothing have been enabled !!!" << endl;
		      abort();
		    }
#endif
		  if (inRetireInstruction[i].enable)// && inRetireInstruction[i].data.something())
		    {
		      InstructionPtr tmpinst = inRetireInstruction[i].data;
		      if ( !((tmpinst->fn & FnLoad) && (tmpinst->replay_trap)) )
			{
#ifdef DD_DEBUG_ALLOCATOR_RETIRE_VERB3
	if (DD_DEBUG_TIMESTAMP < timestamp())
	  {
		      cerr << "["<<this->name()<<"("<<timestamp()<<")] = RETIRE() === !!!" << endl;
		      cerr << tmpinst << endl;
	  }
#endif
			  Retire(&tmpinst);
			}
		      else
			{
#ifdef DD_DEBUG_ALLOCATOR_RETIRE_VERB3
	if (DD_DEBUG_TIMESTAMP < timestamp())
	  {
		      cerr << "["<<this->name()<<"("<<timestamp()<<")] = RETIRE() === REPLAY_TRAP !!!" << endl;
		      cerr << tmpinst << endl;
	  }
#endif
			}
		    }
		}

		/* Flush ? */
		//		if(inFlush)
		if(inFlush.enable && inFlush.data.something())
		{
		  if(inFlush.data)
		  {
#ifdef DD_DEBUG_ALLOCATOR_VERB1
		    cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====  Flush !!!" << endl;
#endif
#ifdef DD_DEBUG_FLUSH
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====  Flush !!!" << endl;
#endif
	  //changed = true;
  		    /* Perform a roll back on the integer mapping table */
		    integerMappingTable.Rollback();

			/* Perform a roll back on the floating point mapping table */
			floatingPointMappingTable.Rollback();

			/* Perform a roll back on the condition mapping table */
			conditionMappingTable.Rollback();
			/* Perform a roll back on the FPSCR mapping table */
			FPSCRMappingTable.Rollback();
			/* Perform a roll back on the link mapping table */
			linkMappingTable.Rollback();
			/* Perform a roll back on the count mapping table */
			countMappingTable.Rollback();
			/* Perform a roll back on the XER mapping table */
			XERMappingTable.Rollback();


			/* Reset the reorder buffer tag allocator */
			robSize = 0;
			robHead = -1;
			robTail = -1;

			/* Reset the lock bit */
			lock = false;

			/* Flush the rename pipeline */
			renamePipeline.Flush();

			/* Reset the valid, enable and accept ports */
			/*
			for(i = 0; i < Width; i++)
			{
				outValid[i] = false;
				outEnable[i] = false;
				outLoadValid[i] = false;
				outLoadEnable[i] = false;
				outStoreValid[i] = false;
				outStoreEnable[i] = false;
				outAccept[i] = false;
			}
			*/
			/* Stop the process */
			return;
		  }
		}

		QueuePointer<RenamePipelineEntry<T, nSources>, nStages * Width> renamePipelineEntry;
		int renamePort, loadPort, storePort;

		// DD: The following loop must be in a Check Accept to Enable Process !!!
#ifdef DD_DEBUG_ALLOCATOR_VERB1
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====   Start of RenamePipeline !!!" << endl;
		cerr << renamePipeline << endl;
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====   End of RenamePipeline !!!" << endl;
#endif
		/* For each rename pipeline entry which processing is finished */
		for(renamePipelineEntry = renamePipeline.SeekAtHead(), renamePort = loadPort = storePort = 0; renamePipelineEntry && renamePort < Width; renamePipelineEntry++, renamePort++)
		{
			/* Is processing finished ? */
			if(renamePipelineEntry->delay != 0) break;

			/* Get the instruction in the pipeline entry */
			//const InstructionPtr<T, nSources>& instruction = renamePipelineEntry->instruction;
			InstructionPtr instruction = renamePipelineEntry->instruction;

			/* We consider that we have an accept if we got an accept from both the issue queues and the reorder buffer.
			If instruction is a load/prefetch load, we need an accept from the load queue
			If instruction is a store, we need an accept from the store queue */
			/*
			bool accept = inAccept[renamePort] &&
			              inReorderBufferAccept[renamePort] &&
			              (!(instruction->fn & (FnLoad | FnPrefetchLoad)) || inLoadAccept[loadPort]) &&
			              (!(instruction->fn & FnStore) || inStoreAccept[storePort]);
			*/
			bool accept = 
			  outInstructionIssue[renamePort].accept &&
			  outInstructionReorder[renamePort].accept &&
			  (!(instruction->fn & (FnLoad | FnPrefetchLoad)) || outLoadInstruction[loadPort].accept) &&
			  (!(instruction->fn & FnStore) || outStoreInstruction[storePort].accept);
			  

                        // For Dump
                        if (!accept)
                          {
                            if (!outInstructionIssue[renamePort].accept)
                              //{ instruction->alloc_stall_sched_reject++; stall_sched++; }
                              { stall_sched++; }
                            if (!outInstructionReorder[renamePort].accept)
                              //{ stall_reord++; instruction->alloc_stall_reord_reject = stall_reord;}
                              { stall_reord++; }
                            if (! (!(instruction->fn & (FnLoad | FnPrefetchLoad)) || outLoadInstruction[loadPort].accept) )
                              //{ instruction->alloc_stall_loadq_reject++; stall_loadq++; }
                              {  stall_loadq++; }
                            if (! (!(instruction->fn & FnStore) || outStoreInstruction[storePort].accept) )
                              //{ instruction->alloc_stall_storq_reject++; stall_storq++; }
                              { stall_storq++; }
                          }
                        instruction->alloc_stall_sched_reject = stall_sched;
                        instruction->alloc_stall_reord_reject = stall_reord;
                        instruction->alloc_stall_loadq_reject = stall_loadq;
                        instruction->alloc_stall_storq_reject = stall_storq;

			/* Instruction was rejected */
			if(!accept) break;

			/* Instruction was accepted */
			if(instruction->fn & (FnLoad | FnPrefetchLoad)) loadPort++;
			if(instruction->fn & FnStore) storePort++;

			/* If instruction is execution serialized, set the lock bit */
			/* This avoid starting new instruction until the lock bit is reseted */
			if(instruction->execution_serialized) lock = true;

			/* Remove the rename pipeline entry */
			renamePipeline.RemoveHead();
			//			changed = true;
		}

		// The following loop must be into EOF ...
		/* For each rename port */
		for(renamePort = 0; renamePort < Width; renamePort++)
		{
			/* Is the instruction enabled ? */
			if(!inInstruction[renamePort].enable) break;
			/* Allocate a rename pipeline entry */
			RenamePipelineEntry<T, nSources> *renamePipelineEntry = renamePipeline.New();

			if(renamePipelineEntry)
			{
				/* Initialize the processing delay to the number of pipe stage */
				renamePipelineEntry->delay = nStages;

				/* Store the instruction into the pipeline entry */
				renamePipelineEntry->instruction = inInstruction[renamePort].data;
				
				/* Rename the source operands of the instruction */
				Rename(&(renamePipelineEntry->instruction));

				/* Allocate ressources for the instruction */
				Allocate(&(renamePipelineEntry->instruction));
				//changed = true;
                                // For Dump
                                renamePipelineEntry->instruction->timing_allocate_cycle=timestamp();
			}
			else
			{
				/* Throw an error if there is not enough space into the pipeline */
				cerr << "time stamp = " << timestamp() << endl;
				cerr << "Rename pipeline overflow" << endl;
				exit(-1);
			}
		}

		// SOF ???
		/* For each rename pipeline entry */
		for(renamePipelineEntry = renamePipeline.SeekAtHead(); renamePipelineEntry; renamePipelineEntry++)
		{
			/* Is processing finished ? */
			if(renamePipelineEntry->delay > 0)
			{
				/* Decrement the processing delay */
				renamePipelineEntry->delay--;
				//changed = true;
			}
		}
#ifdef DD_DEBUG_PIPELINE_VERB1
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ==== Pipeline Debug" << endl;
		cerr << this << endl;
#endif

	}// end of end_of_cycle
  
        void start_of_cycle()
        {
		bool execution_serialized = false;

		QueuePointer<RenamePipelineEntry<T, nSources>, nStages * Width> renamePipelineEntry;
		int renamePort, loadPort, storePort;

		// SOF
		/* For each rename pipeline entry */
		for(renamePipelineEntry = renamePipeline.SeekAtHead(), renamePort = 0, loadPort = 0, storePort = 0; renamePipelineEntry && renamePort < Width && !lock && !execution_serialized; renamePort++, renamePipelineEntry++)
		{
			/* Get the instruction into the pipeline entry */
			//const InstructionPtr<T, nSources>& instruction = renamePipelineEntry->instruction;
			const InstructionPtr instruction = renamePipelineEntry->instruction;

			/* Is instruction execution serialized ? */
			execution_serialized = instruction->execution_serialized;

			/* If instruction is execution serialized and it is the last instruction
			in the rename pipeline then stop dispatch of further instructions */
			if(execution_serialized && (renamePort > 0 || robSize > renamePipeline.Size())) break;
			
			// If rob don't have enough free space then don't allocate !!!
			//			if (robSize < )

			/* Request for a dispatch to the issue queue and the reorder buffer */
			//			outValid[renamePort] = true;

			/* Write the instruction on output */
			//			outInstruction[renamePort] = instruction;
			outInstructionIssue[renamePort].data = instruction;
			outInstructionReorder[renamePort].data = instruction;

			/* If instruction is a store */
			if(instruction->fn & FnStore)
			{
				/* Request for a dispatch to the store queue */
			  // outStoreValid[storePort] = true;

				/* Output the instruction toward the store queue */
			  //	outStoreInstruction[storePort] = instruction;
			  outStoreInstruction[storePort].data = instruction;
				storePort++;
			}
			/* If instruction is a load/prefetch load */
			if(instruction->fn & (FnLoad | FnPrefetchLoad))
			{
				/* Request for a dispatch to the load queue */
			  //	outLoadValid[loadPort] = true;

				/* Output the instruction toward the load queue */
			  //	outLoadInstruction[loadPort] = instruction;
			  outLoadInstruction[loadPort].data = instruction;

				loadPort++;
			}
		}

		/* Clear the remaining request ports */
		for(; renamePort < Width; renamePort++)
		  {
			outInstructionIssue[renamePort].data.nothing();
			outInstructionReorder[renamePort].data.nothing();
		  }
		for(; loadPort < Width; loadPort++)
			outLoadInstruction[loadPort].data.nothing();
		for(; storePort < Width; storePort++)
			outStoreInstruction[storePort].data.nothing();

		/* If state changed then make the SystemC scheduler call
		the ExternalControl process handling the valid, accept and enable hand-shaking */
		/*		if(changed)
			outStateChanged = !inStateChanged;
		*/
	}

	/** Read physical register tag of an integer architectural register in the integer mapping table
		@param reg the number of the integer architectural register
		@return the tag of the integer physical register on which the integer architectural register is mapped */
	int ReadIntegerMappingTable(int reg)
	{
		return integerMappingTable.Read(reg);
	}

	/** Read physical register tag of an floating point architectural register in the floating point mapping table
		@param reg the number of the floating point architectural register
		@return the tag of the floating point physical register on which the floating point architectural register is mapped */
	int ReadFloatingPointMappingTable(int reg)
	{
		return floatingPointMappingTable.Read(reg);
	}

	int ReadConditionMappingTable(int reg)
	{
		return conditionMappingTable.Read(reg);
	}
	int ReadFPSCRMappingTable(int reg)
	{
		return FPSCRMappingTable.Read(reg);
	}
	int ReadLinkMappingTable(int reg)
	{
		return linkMappingTable.Read(reg);
	}
	int ReadCountMappingTable(int reg)
	{
		return countMappingTable.Read(reg);
	}

	int ReadXERMappingTable(int reg)
	{
		return XERMappingTable.Read(reg);
	}

	/** Print the module state on an output stream
		@param os an output stream
		@param allocatorRenamer a allocatorRenamer module
		@return the output stream
	*/
	friend ostream& operator << (ostream& os, const AllocatorRenamer<T, nSources, nIntegerArchitecturalRegisters, nFloatingPointArchitecturalRegisters, nIntegerRegisters, nFloatingPointRegisters, Width, WriteBackWidth, RetireWidth, ReorderBufferSize, nStages>& allocatorRenamer)
	{
		os << "========= ALLOCATE/RENAME ==========" << endl;
		os << "lock=" << allocatorRenamer.lock << endl;
		os << "(alloc)robSize=" << allocatorRenamer.robSize << endl;
		os << allocatorRenamer.renamePipeline;
		os << "------------------------------------------------------------------------" << endl;
		os << "integer mapping table:" << endl;
		os << allocatorRenamer.integerMappingTable;
		os << "------------------------------------------------------------------------" << endl;
		os << "floating point mapping table:" << endl;
		os << allocatorRenamer.floatingPointMappingTable;
		os << "------------------------------------------------------------------------" << endl;
		os << "condition mapping table:" << endl;
		os << allocatorRenamer.conditionMappingTable;
		os << "------------------------------------------------------------------------" << endl;
		os << "FPSCR mapping table:" << endl;
		os << allocatorRenamer.FPSCRMappingTable;
		os << "------------------------------------------------------------------------" << endl;
		os << "link mapping table:" << endl;
		os << allocatorRenamer.linkMappingTable;
		os << "------------------------------------------------------------------------" << endl;
		os << "count mapping table:" << endl;
		os << allocatorRenamer.countMappingTable;
		os << "------------------------------------------------------------------------" << endl;
		os << "XER mapping table:" << endl;
		os << allocatorRenamer.XERMappingTable;
		os << "====================================" << endl;
		return os;
	}

	/** Perform sanity checks on the module */
	bool Check()
	{
		return renamePipeline.Check();
	}
	
	void WarmRestart()
	{
		robHead = -1;
		robTail = -1;
		robSize = 0;
		lock = false;
		//changed = true;
		
		integerMappingTable.Reset();
		floatingPointMappingTable.Reset();

		conditionMappingTable.Reset();
		FPSCRMappingTable.Reset();
		linkMappingTable.Reset();
		countMappingTable.Reset();
		XERMappingTable.Reset();

		/* Reset the rename pipeline */
		renamePipeline.Reset();
	}
	
	void ResetStats()
	{
	}
	
		int robSize;

private:
	/* Members for managing the reorder buffer tag with a fifo allocation policy */
	int robHead;
	int robTail;
//	int robSize;

	/* The lock bit. It is set to avoid dispatching further instructions */
	bool lock;

	/* A queue to model the rename pipeline */
	Queue<RenamePipelineEntry<T, nSources>, nStages * Width> renamePipeline;

	/* The integer mapping table */
	MappingTable<nIntegerArchitecturalRegisters, nIntegerRegisters> integerMappingTable;

	/* The floating point mapping table */
	MappingTable<nFloatingPointArchitecturalRegisters, nFloatingPointRegisters> floatingPointMappingTable;

	/* The Condition register mapping table */
        MappingTable<nConditionArchitecturalRegisters, nConditionRegisters> conditionMappingTable;

	/* The FPSC register mapping table */
        MappingTable<nFPSCArchitecturalRegisters, nFPSCRegisters> FPSCRMappingTable;

	/* The Link register mapping table */
        MappingTable<nLinkArchitecturalRegisters, nLinkRegisters> linkMappingTable;

	/* The Count register mapping table */
        MappingTable<nCountArchitecturalRegisters, nCountRegisters> countMappingTable;

	/* The XER register mapping table */
        MappingTable<nXERArchitecturalRegisters, nXERRegisters> XERMappingTable;

	/* Some ports to make the SystemC scheduler call the ExternalControl process on state changes */
	/*
	ml_out_data<bool> outStateChanged;
	ml_signal_data<bool> stateChanged;
	ml_in_data<bool> inStateChanged;

	bool changed;
	*/
};

} // end of namespace decode
} // end of namespace pipeline
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
