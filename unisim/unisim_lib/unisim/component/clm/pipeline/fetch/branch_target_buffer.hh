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
                      BranchTargetBuffer.hh  -  description
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_FETCH_BRANCH_TARGET_BUFFER_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_FETCH_BRANCH_TARGET_BUFFER_HH__

#include <unisim/component/clm/utility/common.hh>

namespace unisim {
namespace component {
namespace clm {
namespace pipeline {
namespace fetch {

  using unisim::component::clm::memory::address_t;

class BranchTargetBufferEntry
{
public:
	bool valid;
	address_t cia;
	address_t nia;
};

template <int nlines, int associativity, int InstructionSize>
class BranchTargetBuffer
{
public:
	BranchTargetBuffer()
	{
		int i, j;
		for(i = 0; i < nlines; i++)
		{
			for(j = 0; j < associativity; j++)
			{
				buffer[i][j].valid = false;
				buffer[i][j].cia = 0;
				buffer[i][j].nia = 0;
			}
		}
	}

	bool Read(address_t cia, address_t& nia)
	{
		int index = (cia / InstructionSize) % nlines;

		int i = Search(index, cia);

		if(i >= 0)
		{
			nia = buffer[index][i].nia;
			return true;
		}
		return false;
	}

	void Update(address_t cia, address_t nia)
	{
		int index = (cia / InstructionSize) % nlines;
		int i;

		for(i = 0; i < associativity; i++)
		{
			if(buffer[index][i].valid && buffer[index][i].cia == cia)
			{
				buffer[index][i].nia = nia;
				return;
			}
		}

		i = Allocate(index);
		buffer[index][i].cia = cia;
		buffer[index][i].nia = nia;
	}

	INLINE int myrand()                    /* returns random number */
	{
#if !defined(__alpha) && !defined(linux) && !defined(__linux) && !defined(__linux__) && !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64) && !defined(__APPLE_CC__)
  		extern long random(void);
#endif

#if defined(__CYGWIN32__) || defined(hpux) || defined(__hpux) || defined(__svr4__) || defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64) || defined(__APPLE_CC__)
		  return rand();
#else
		  return random();
#endif
	}


private:
	BranchTargetBufferEntry buffer[nlines][associativity];

	int Choose(int index)
	{
		int bank = myrand() % associativity;
		return bank;
	}

	int Search(int index, address_t cia)
	{
		int i;

		for(i = 0; i < associativity; i++)
		{
			if(buffer[index][i].valid && buffer[index][i].cia == cia) return i;
		}
		return -1;
	}

	int Allocate(int index)
	{
		int i;

		for(i = 0; i < associativity; i++)
		{
			if(!buffer[index][i].valid)
			{
				buffer[index][i].valid = true;
				return i;
			}
		}

		i = Choose(index);
		buffer[index][i].valid = true;
		return i;
	}

};

} // end of namespace fetch
} // end of namespace pipeline
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
