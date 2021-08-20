/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */

#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_TESLA_FLOW_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_TESLA_FLOW_TCC

#include <unisim/component/cxx/processor/tesla/tesla_flow.hh>
#include <unisim/component/cxx/processor/tesla/cpu.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template<class CONFIG>
TeslaFlow<CONFIG>::TeslaFlow(Warp<CONFIG> & warp) :
	warp(warp)
{
}

template<class CONFIG>
void TeslaFlow<CONFIG>::Reset(CPU<CONFIG> * cpu, std::bitset<CONFIG::WARP_SIZE> mask)
{
	this->cpu = cpu;
	current_mask = initial_mask = mask;
	stack = stack_t();
	//stack.push(StackToken(mask, ID_BOTTOM, 0));
}

template<class CONFIG>
void TeslaFlow<CONFIG>::Branch(address_t target_addr, std::bitset<CONFIG::WARP_SIZE> mask)
{
	assert((target_addr & 3) == 0);	// 64-bit-aligned targets
	typename CONFIG::address_t adjpc = warp.pc - CONFIG::CODE_START;

	// Check mask
	// Mask == none -> nop
	// Mask == all  -> jump
	// Mask == other -> argh
	
	typename CONFIG::address_t abs_target = target_addr + CONFIG::CODE_START;
	if(cpu->TraceBranch()) {
		cerr << "  Branch mask = " << mask << endl;
	}
	
	assert((mask & ~(current_mask)).none());	// No zombies allowed

	if((mask).none())
	{
		// Uniformally not taken
		if(cpu->TraceBranch()) {
			cerr << "  Coherent, not taken." << endl;
		}
		(*cpu->stats)[warp.pc - CONFIG::CODE_START].BranchUniNotTaken();
		// Nop, just increment PC as usual
	}
	else if(mask == GetCurrentMask())	// Same number of enabled threads as before
	{
		// Uniformally taken
		if(cpu->TraceBranch()) {
			cerr << "  Coherent, taken." << endl;
		}
		(*cpu->stats)[warp.pc - CONFIG::CODE_START].BranchUniTaken();
		(*cpu->stats)[warp.pc - CONFIG::CODE_START].Jump();
		warp.npc = abs_target;
		// current_mask = mask already
	}
	else
	{
		(*cpu->stats)[warp.pc - CONFIG::CODE_START].BranchDivergent();
		(*cpu->stats)[warp.pc - CONFIG::CODE_START].Jump();
		// Start by *taking* the branch ??
	
		// Mixed
		// Push divergence token
		stack.push(StackToken(current_mask & ~mask, ID_DIVERGE, warp.npc));
		if(cpu->TraceBranch()) {
			cerr << hex;
			cerr << " Conditional non-coherent jump from "
				<< adjpc << " to " << target_addr << endl;
			cerr << dec;
			cerr << " Push DIVERGE." << endl;
		}
		warp.npc = abs_target;
		current_mask = mask;

		if(cpu->TraceBranch()) {
			cerr << "  New mask = " << GetCurrentMask() << endl;
		}
	}
	assert(stack.size() < CONFIG::STACK_DEPTH);
}

template<class CONFIG>
void TeslaFlow<CONFIG>::Meet(address_t addr)
{
	// Addr relative address
	// SYNC instruction
	stack.push(StackToken(current_mask, ID_SYNC, addr + CONFIG::CODE_START));	// Does addr mean anything here??
	if(cpu->TraceBranch()) {
		cerr << " Push SYNC." << endl;
		cerr << "  Mask = " << GetCurrentMask() << endl;
	}
	assert(stack.size() < CONFIG::STACK_DEPTH);
}

template<class CONFIG>
void TeslaFlow<CONFIG>::PreBreak(address_t addr)
{
	stack.push(StackToken(current_mask, ID_BREAK, addr + CONFIG::CODE_START));
	if(cpu->TraceBranch()) {
		cerr << " Push BREAK." << endl;
		cerr << "  Mask = " << GetCurrentMask() << endl;
	}
	assert(stack.size() < CONFIG::STACK_DEPTH);
}

template<class CONFIG>
bool TeslaFlow<CONFIG>::Join()
{
	if(stack.empty()) {
		if(cpu->TraceBranch()) {
			cerr << " Reached bottom of stack, restoring initial mask." << endl;
		}
		// implicit SYNC token at the bottom of stack?
		current_mask = initial_mask;
		if(cpu->TraceBranch()) {
			cerr << "  New mask = " << GetCurrentMask() << endl;
		}
		return true;
	}
	StackToken token = stack.top();
	stack.pop();
	
	current_mask = token.mask;
	if(cpu->TraceBranch()) {
		cerr << "  New mask = " << GetCurrentMask() << endl;
	}
	if(token.id != ID_SYNC)
	{
		if(cpu->TraceBranch()) {
			cerr << " Pop. Not a SYNC token. Go back." << endl;
		}
		// Re-branch to address in token
		(*cpu->stats)[warp.pc - CONFIG::CODE_START].Jump();
		warp.npc = token.address;
		
		// Do NOT execute current instruction
		return false;
	}
	else if(cpu->TraceBranch()) {
		cerr << " Pop. SYNC token." << endl;
	}
	return true;
}

template<class CONFIG>
bool TeslaFlow<CONFIG>::End()
{
	// Implicit sync
	if(!stack.empty()) {
		if(cpu->TraceBranch()) {
			cerr << " End reached, stack not empty." << endl;
		}
		StackToken token = stack.top();
		stack.pop();
		if(/*cpu->TraceBranch() && */token.id != ID_DIVERGE) {
			cerr << " Warning: Unexpected " << token.id << " token remaining." << endl;
		}
		// Re-branch to address in token
		(*cpu->stats)[warp.pc - CONFIG::CODE_START].Jump();
		warp.npc = token.address;
		current_mask = token.mask;
		return false;
	}
	return true;
}

template<class CONFIG>
void TeslaFlow<CONFIG>::Return(std::bitset<CONFIG::WARP_SIZE> mask)
{
	if((mask).none()) {
		(*cpu->stats)[warp.pc - CONFIG::CODE_START].BranchUniNotTaken();
	}
	else if(mask == GetCurrentMask()) {
		(*cpu->stats)[warp.pc - CONFIG::CODE_START].BranchUniTaken();
	}
	else {
		(*cpu->stats)[warp.pc - CONFIG::CODE_START].BranchDivergent();
	}
	// mask : threads to return
	if(stack.empty()) {
		if(cpu->TraceBranch()) {
			cerr << " Reached bottom of stack, killing thread." << endl;
		}
		current_mask = GetCurrentMask() & ~mask;
		initial_mask = current_mask;
		if(initial_mask.none()) {
			// Kill warp
			warp.state = Warp<CONFIG>::Finished;
		}
	}
	else
	{
		if(cpu->TraceBranch()) {
			cerr << " Pop." << endl;
		}
		StackToken token = stack.top();
		assert(token.id = ID_CALL);
		stack.pop();
		current_mask = token.mask;
		(*cpu->stats)[warp.pc - CONFIG::CODE_START].Jump();
		warp.npc = token.address;	// always go back?
		// what if some threads are still enabled?
		// Patent: "no thread diverge when a call/return branch is encountered"
	}
	if(cpu->TraceBranch()) {
		cerr << "  New mask = " << GetCurrentMask() << endl;
	}
}

template<class CONFIG>
void TeslaFlow<CONFIG>::Break(std::bitset<CONFIG::WARP_SIZE> mask)
{
	if(stack.empty()) {
		if(cpu->TraceBranch()) {
			cerr << " Reached bottom of stack, killing threads." << endl;
		}
		current_mask = GetCurrentMask() & ~mask;
		initial_mask = current_mask;
	}
	else if(mask == GetCurrentMask())
	{
		// All threads take a break
		if(cpu->TraceBranch()) {
			cerr << " Coherent break." << endl;
			cerr << " Pop." << endl;
		}
		StackToken token = stack.top();
		assert(token.id = ID_BREAK);
		stack.pop();
		current_mask = token.mask;
		(*cpu->stats)[warp.pc - CONFIG::CODE_START].Jump();
		warp.npc = token.address;	// always go back?
		// what if some threads are still enabled?
		// handled by software?
		// Patent: ???
	}
	else
	{
		// Just disable breaking threads
		current_mask = GetCurrentMask() & ~mask;
	}
	if(cpu->TraceBranch()) {
		cerr << "  New mask = " << GetCurrentMask() << endl;
	}
}

template<class CONFIG>
void TeslaFlow<CONFIG>::Kill(std::bitset<CONFIG::WARP_SIZE> mask)
{
	assert(false);
}

template<class CONFIG>
void TeslaFlow<CONFIG>::Call(address_t target_addr)
{
	stack.push(StackToken(current_mask, ID_CALL, warp.npc));
	(*cpu->stats)[warp.pc - CONFIG::CODE_START].Jump();
	warp.npc = target_addr + CONFIG::CODE_START;
	assert(stack.size() < CONFIG::STACK_DEPTH);
}

template <class CONFIG>
void TeslaFlow<CONFIG>::CheckJoin()
{
	if(cpu->TraceMask()) {
		cerr << " " << GetCurrentMask() << endl;
	}
}

template <class CONFIG>
std::bitset<CONFIG::WARP_SIZE> TeslaFlow<CONFIG>::GetCurrentMask() const
{
	return current_mask;
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
