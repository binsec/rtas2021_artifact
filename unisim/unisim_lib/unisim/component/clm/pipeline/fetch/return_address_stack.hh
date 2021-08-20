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
                    ReturnAddressStack.hh  -  description
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_FETCH_RETURN_ADDRESS_STACK_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_FETCH_RETURN_ADDRESS_STACK_HH__

#include <unisim/component/clm/utility/common.hh>
#include <unisim/component/clm/interfaces/instruction_interface.hh>

namespace unisim {
namespace component {
namespace clm {
namespace pipeline {
namespace fetch {

  using unisim::component::clm::memory::address_t;
using unisim::component::clm::interfaces::InstructionPtr;

using unisim::component::clm::utility::hexa;


#define RAS_NOP 0
#define RAS_PUSH 1
#define RAS_POP 2

//DDtemplate <class T, int nSources>
class RollbackEntry
{
public:
	int op;
	address_t save_addr;
	//	InstructionPtr<T, nSources> save_instruction;
	InstructionPtr save_instruction;

        //  RollbackEntry(): op(0), save_addr(0), save_instruction(NULL) {}
  //        RollbackEntry(): op(0), save_addr(0) {}
        RollbackEntry() { op = 0; save_addr = 0; save_instruction=InstructionPtr(); }

	friend ostream& operator << (ostream& os, const RollbackEntry& entry)
	{
		switch(entry.op)
		{
			case RAS_NOP: os << "op=NOP"; break;
			case RAS_PUSH: os << "op=PUSH"; break;
			case RAS_POP: os << "op=POP"; break;
		}
		os << ",save_addr=" << hexa(entry.save_addr);
		//		os << ",save_addr=" << entry.save_addr;
		os << ",save_instruction=" << entry.save_instruction;
		return os;
	}

	int operator == (const RollbackEntry& entry) const
	{
		return entry.op == op && entry.save_addr == save_addr && entry.save_instruction == save_instruction;
	}
};

//template <class T, int nSources, int MaxSize, int MaxBranches>
template <int MaxSize, int MaxBranches>
class ReturnAddressStack
{
public:
	ReturnAddressStack()
	{
		size = 0;
		tos = 0;
		rollbackBuffer = Queue<RollbackEntry, MaxBranches>();
	}

	friend ostream& operator << (ostream& os, const ReturnAddressStack& ras)
	{
		int i, sp;

		for(i = 0, sp = ras.tos; i < ras.size; i++, sp = (sp > 0) ? sp - 1 : MaxSize - 1)
		{
			os << hexa(ras.stack[sp].addr) << "   pushed by " << ras.stack[sp].instruction << endl;
		}

		//os << ras.rollbackBuffer;
		return os;
	}

	//DD	void SafePush(address_t nia, const InstructionPtr<T, nSources>& instruction)
  //	void SafePush(address_t nia, Instruction *const instruction)
	void SafePush(address_t nia, const InstructionPtr instruction)
	{
		/* save the RAS state */
		//cerr << "BEFORE PUSH:" << endl;
		//cerr << *this;
		if(rollbackBuffer.Full())
		{
			cerr << *this << endl;
			//DD			cerr << "time stamp = " << Time_stamp() << endl;
			cerr << "RAS rollback buffer is full (occurred during a PUSH on the stack)";
			exit(-1);
		}
		//		RollbackEntry<T, nSources> *entry = rollbackBuffer.New();
		RollbackEntry *entry = rollbackBuffer.New();
		entry->op = RAS_PUSH;
		entry->save_addr = stack[tos].addr;
		entry->save_instruction = stack[tos].instruction;

		/* push the address on the stack */
		tos = (tos < MaxSize - 1) ? tos + 1 : 0;
		stack[tos].addr = nia;
		stack[tos].instruction = instruction;
		if(size < MaxSize) size++;
		//cerr << "AFTER PUSH:" << endl;
		//cerr << *this;
	}

	//DD	void Push(address_t nia, const InstructionPtr<T, nSources>& instruction)
  //	void Push(address_t nia, Instruction *const instruction)
	void Push(address_t nia, const InstructionPtr instruction)
	{
		// push the address on the stack
		tos = (tos < MaxSize - 1) ? tos + 1 : 0;
		stack[tos].addr = nia;
		stack[tos].instruction = (instruction);
		if(size < MaxSize - 1) size++;
	}

	bool SafePop(address_t& nia)
	{
		//cerr << "BEFORE POP:" << endl;
		//cerr << *this;

		/* pop the address on the top of the stack */
		if(size > 0)
		{
			/* save the RAS state */
			if(rollbackBuffer.Full())
			{
				cerr << "RAS Rollback buffer is full (occurred during a POP on the stack)";
				exit(-1);
			}
			//DD			RollbackEntry<T, nSources> *entry = rollbackBuffer.New();
			RollbackEntry *entry = rollbackBuffer.New();

			entry->op = RAS_POP;
			entry->save_addr = stack[tos].addr;
			entry->save_instruction = stack[tos].instruction;
			nia = stack[tos].addr;
			tos = (tos > 0) ? tos - 1 : MaxSize - 1;
			size--;
		}
		else
		{
			/* miss on the RAS */
			if(rollbackBuffer.Full())
			{
				cerr << "RAS Rollback buffer is full (occurred during a POP on an empty stack)";
				exit(-1);
			}
			//DD			RollbackEntry<T, nSources> *entry = rollbackBuffer.New();
			RollbackEntry *entry = rollbackBuffer.New();

			entry->op = RAS_NOP;
			entry->save_addr = stack[tos].addr;
			entry->save_instruction = stack[tos].instruction;

			//cerr << "AFTER POP:" << endl;
			//cerr << *this;
			return false;
		}
		//cerr << "AFTER POP:" << endl;
		//cerr << *this;
		return true;
	}

	bool Pop(address_t& nia)
	{
		//exit(-1);
		// pop the address on the top of the stack
		if(size > 0)
		{
			nia = stack[tos].addr;
			tos = (tos > 0) ? tos - 1 : MaxSize - 1;
			size--;
		}
		else
		{
			// miss on the RAS
			return false;
		}
		return true;
	}

	void Retire()
	{
		if(rollbackBuffer.Empty())
		{
			cerr << "RAS Rollback buffer is empty";
			exit(-1);
		}
		rollbackBuffer.RemoveHead();
	}

	void Rollback()
	{
		/* do a rollback */
		if(!rollbackBuffer.Empty())
		{
		  //DD			RollbackEntry<T, nSources> *entry;
			RollbackEntry *entry;

			for(entry = rollbackBuffer.GetTail(); entry; entry = rollbackBuffer.GetTail())
			{
				if(entry->op & RAS_PUSH)
				{
					stack[tos].addr = entry->save_addr;
					stack[tos].instruction = entry->save_instruction;
					tos = (tos > 0) ? tos - 1 : MaxSize - 1;
					if(size > 0) size--;
				}
				if(entry->op & RAS_POP)
				{
					tos = (tos < MaxSize - 1) ? tos + 1 : 0;
					stack[tos].addr = entry->save_addr;
					stack[tos].instruction = entry->save_instruction;
					if(size < MaxSize - 1) size++;
				}
				rollbackBuffer.RemoveTail();
			}
		}
	}
	
	void FlushRollbackBuffer()
	{
		rollbackBuffer.Flush();
	}
	
	void ResetRollbackBuffer()
	{
		rollbackBuffer.Reset();
	}

	bool Check()
	{
		int i, j, sp1, sp2;

		for(i = 0, sp1 = tos; i < size; i++, sp1 = (sp1 > 0) ? sp1 - 1 : MaxSize - 1)
		{
			for(j = 0, sp2 = tos; j < size; j++, sp2 = (sp2 > 0) ? sp2 - 1 : MaxSize - 1)
			{
				if(sp1 != sp2 && stack[sp1].instruction == stack[sp2].instruction)
				{
					return false;
				}
			}
		}

//		return rollbackBuffer.Check();
		return true;
	}



private:
	int tos;
	int size;
	struct
	{
		address_t addr;
	  //DD		InstructionPtr<T, nSources> instruction;
		InstructionPtr instruction;
	} stack[MaxSize];
	//DD	Queue<RollbackEntry<T, nSources>, MaxBranches> rollbackBuffer;
	Queue<RollbackEntry, MaxBranches> rollbackBuffer;
};

} // end of namespace fetch
} // end of namespace pipeline
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
