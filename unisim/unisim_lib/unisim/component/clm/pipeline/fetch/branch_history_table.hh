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
                    BranchHistoryTable.hh  -  description
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_FETCH_BRANCH_HISTORY_TABLE_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_FETCH_BRANCH_HISTORY_TABLE_HH__

#include <unisim/component/clm/utility/common.hh>

namespace unisim {
namespace component {
namespace clm {
namespace pipeline {
namespace fetch {

  using unisim::component::clm::memory::address_t;

using unisim::component::clm::utility::branch_direction_t;
using unisim::component::clm::utility::Taken;
using unisim::component::clm::utility::NotTaken;


template <int size, int nBits, int nHistorySize, int InstructionSize, int MaxBranches>
class BranchHistoryTable
{
public:
	BranchHistoryTable()
	{
		int i;

		for(i = 0; i < size; i++)
		{
			pht[i] = (1 << nBits) / 2;
		}

		int n;

		for(n = size, indexSize = 0; n > 1; n >>= 1, indexSize++);

		history = 0;
	}

	branch_direction_t Read(address_t cia)
	{
		branch_direction_t direction;
		uint32_t mask = nHistorySize ? history << (indexSize - nHistorySize) : 0;
		uint32_t index = ((cia / InstructionSize) ^ mask) % size;
		//int index = ((cia / InstructionSize) ^ history) & (size - 1);
		direction = (pht[index] >= (1 << nBits) / 2) ? Taken : NotTaken;
		
		return direction;
	}
	
	int AllocateBranch()
	{
		if(nHistorySize)
		{
			uint32_t *entry = OBQ.New();
			
			if(!entry)
			{
				cerr << "OBQ is full !" << endl;
				exit(-1);
			}
			*entry = history;
			return OBQ.GetTailIdent();
		}
		return -1;
	}
	

	void RollbackFrom(int obq_tag)
	{
		if(nHistorySize)
		{
			/* Do a roll back on branch history register */
			OoOQueuePointer<uint32_t, MaxBranches> entry = OBQ.Retrieve(obq_tag);
			history = *entry;
			OBQ.FlushAfter(entry);
			OBQ.Remove(entry);
		}
	}
	
	void Rollback()
	{
		if(nHistorySize)
		{
			/* Do a roll back on branch history register */
			history = *OBQ.GetHead();
			OBQ.Flush();
		}
	}
	
	void Flush()
	{
		if(nHistorySize)
		{
			OBQ.Flush();
		}
	}
	
	void Commit()
	{
		if(nHistorySize)
		{
			OBQ.RemoveHead();
		}
	}

	void Update(address_t cia, branch_direction_t direction)
	{
		uint32_t mask = nHistorySize ? history << (indexSize - nHistorySize) : 0;
		uint32_t index = ((cia / InstructionSize) ^ mask) % size;
		//int index = ((cia / InstructionSize) ^ history) & (size - 1);
		int counter = pht[index];

		if(nHistorySize)
		{
			switch(direction)
			{
				case Taken:
					history >>= 1;
					history |= 1UL << (nHistorySize - 1);
					break;
	
				case NotTaken:
					history >>= 1;
					break;
	
				default:
					cerr << "commiting a branch instruction which has not been resolved (in Update1)" << endl;
					exit(-1);
			}
		}

		switch(direction)
		{
			case Taken:
				if(counter < (1 << nBits) - 1) pht[index] = counter + 1;
				break;

			case NotTaken:
				if(counter > 0) pht[index] = counter - 1;
				break;

			default:
				cerr << "commiting a branch instruction which has not been resolved (in Update2)" << endl;
				exit(-1);
		}
	}

	void SpeculativeUpdate(int obq_tag, address_t cia, branch_direction_t direction)
	{
		uint32_t mask = nHistorySize ? history << (indexSize - nHistorySize) : 0;
		uint32_t index = ((cia / InstructionSize) ^ mask) % size;
		//int index = ((cia / InstructionSize) ^ history) & (size - 1);
		int counter = pht[index];
		
		if(nHistorySize)
		{
			OoOQueuePointer<uint32_t, MaxBranches> entry = OBQ.Retrieve(obq_tag);
			
			*entry = history;
	
			switch(direction)
			{
				case Taken:
					history >>= 1;
					history |= 1UL << (nHistorySize - 1);
					break;
	
				case NotTaken:
					history >>= 1;
					break;
	
				default:
					cerr << "commiting a branch instruction which has not been resolved (in SpecUpdate)" << endl;
					exit(-1);
			}
		}
	}

	private:
	uint32_t history;
	int indexSize;
	int pht[size];
	OoOQueue<uint32_t, MaxBranches> OBQ;
};

} // end of namespace fetch
} // end of namespace pipeline
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim


#endif
