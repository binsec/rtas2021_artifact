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

#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_IMPLICIT_FLOW_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_IMPLICIT_FLOW_TCC

#include <unisim/component/cxx/processor/tesla/implicit_flow.hh>
#include <unisim/component/cxx/processor/tesla/cpu.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template<class CONFIG>
ImplicitFlow<CONFIG>::ImplicitFlow(Warp<CONFIG> & warp) :
	warp(warp)
{
}

template<class CONFIG>
void ImplicitFlow<CONFIG>::Reset(CPU<CONFIG> * cpu, std::bitset<CONFIG::WARP_SIZE> mask)
{
	this->cpu = cpu;
	current_mask = mask;
	join_stack = std::stack<address_t>();
	mask_stack = MaskStack<CONFIG::WARP_SIZE, CONFIG::BRANCH_STACK_DEPTH>();
	loop_stack = std::stack<address_t>();
}

template<class CONFIG>
void ImplicitFlow<CONFIG>::Branch(address_t target_addr, std::bitset<CONFIG::WARP_SIZE> mask)
{
	assert((target_addr & 3) == 0);	// 64-bit-aligned targets
	typename CONFIG::address_t adjpc = GetPC() - CONFIG::CODE_START;

	// Check mask
	// Mask == none -> nop
	// Mask == all  -> jump
	// Mask == other -> argh
	
	// Side effect: take other side of cond branch???
	typename CONFIG::address_t abs_target = target_addr + CONFIG::CODE_START;
	if(cpu->trace_branch) {
		cerr << " Branch mask = " << mask << endl;
	}
	
	assert((mask & ~(current_mask)).none());	// No zombies allowed

	if((mask).none())
	{
		if(cpu->trace_branch) {
			cerr << "  Coherent, not taken." << endl;
		}
		if(adjpc > target_addr)
		{
			// Not-taken backward loop
			// Re-enable threads
			// Compare PC with top of loop address stack
			// if ==, pop
			if(GetLoop() != 0 && GetLoop() == GetPC()) {
				if(cpu->trace_branch) {
					cerr << hex;
					cerr << " End of loop " << GetPC() - CONFIG::CODE_START
						<< "  detected, restoring context\n";
					cerr << dec;
				}
				PopLoop();
				current_mask = PopMask();
				if(cpu->trace_branch) {
					cerr << "  Mask = " << GetCurrentMask() << endl;
				}
			}
		}
	}
	else if(mask == GetCurrentMask())	// Same number of enabled threads as before
	{
		if(cpu->trace_branch) {
			cerr << "  Coherent, taken." << endl;
		}
		warp.npc = abs_target;
	}
	else
	{
		if(adjpc > target_addr)
		{
			// backward jump, loop
			// Compare PC with top of loop address stack
			// if !=, push PC and mask
			if(cpu->trace_branch) {
				cerr << " Conditional non-coherent backward jump from " << hex
					<< adjpc << " to " << target_addr << dec << endl;
			}
			
			if(GetLoop() != GetPC()) {
				if(cpu->trace_branch) {
					cerr << hex;
					cerr << " Start of loop " << GetPC() - CONFIG::CODE_START << " detected, saving context\n";
					cerr << "  Current mask = " << GetCurrentMask() << endl;
					cerr << dec;
				}
				PushLoop(GetPC());
				PushMask(GetCurrentMask());
			}
			current_mask = mask;
			warp.npc = abs_target;
			
		}
		else
		{
			// forward jump
			// Push lots of stuff in stacks
			// then don't take the branch.
			if(cpu->trace_branch) {
				cerr << hex;
				cerr << " Conditional non-coherent forward jump from "
					<< adjpc << " to " << target_addr << endl;
				cerr << dec;
			}
			std::bitset<CONFIG::WARP_SIZE> tos = GetCurrentMask();
			PushMask(tos);
			
			// TODO: CheckJoin *before* pushing new target??

			// Start by executing the 'not taken' branch: negate mask
			current_mask = ~mask & tos;
			PushJoin(abs_target);
			// At the right time (PC >= tos?) go back and
			// take the branch.
			// Keep comparing PC to top of stack, go back (1st pass) or pop (2nd pass) if >=.
		}
		if(cpu->trace_branch) {
			cerr << "  New mask = " << GetCurrentMask() << endl;
		}
	}
}

template<class CONFIG>
void ImplicitFlow<CONFIG>::Meet(address_t addr)
{
	cpu->stats[GetPC() - CONFIG::CODE_START].Unexecute();
}

template<class CONFIG>
void ImplicitFlow<CONFIG>::PreBreak(address_t addr)
{
	cpu->stats[GetPC() - CONFIG::CODE_START].Unexecute();
}


template<class CONFIG>
bool ImplicitFlow<CONFIG>::Join()
{
	return true;
}

template<class CONFIG>
bool ImplicitFlow<CONFIG>::End()
{
	// Make sure stacks are now empty
	assert(mask_stack.empty());
	assert(join_stack.empty());
	assert(loop_stack.empty());
	return true;
}

template<class CONFIG>
void ImplicitFlow<CONFIG>::Return(std::bitset<CONFIG::WARP_SIZE> mask)
{
	if(cpu->trace_branch) {
		cerr << " Return not implemented. Killing thread instead." << endl;
	}
	Kill(mask);
}

template<class CONFIG>
void ImplicitFlow<CONFIG>::Break(std::bitset<CONFIG::WARP_SIZE> mask)
{
	assert(false);
}

template<class CONFIG>
void ImplicitFlow<CONFIG>::Kill(std::bitset<CONFIG::WARP_SIZE> mask)
{
	// mask : threads to kill
	// GetCurrentMask() & ~mask : threads alive
	std::bitset<CONFIG::WARP_SIZE> alive = current_mask & ~mask;
	if(cpu->trace_branch) {
		cerr << " Kill, mask=" << mask << endl;
	}
	if(alive.none())
	{
		// Pop everything.
		while(!mask_stack.empty()) mask_stack.pop();
		while(!join_stack.empty()) join_stack.pop();
		while(!loop_stack.empty()) loop_stack.pop();
		warp.state = Warp<CONFIG>::Finished;
		if(cpu->trace_branch) {
			cerr << "  Warp killed" << endl;
		}
	}
	else if(!mask.none())
	{
		// Remove masked threads from whole stack
		mask_stack.dig(mask, 0);
		current_mask = alive;
		if(cpu->trace_branch) {
			cerr << "  Inactive threads killed. Mask="
				<< GetCurrentMask() << endl;
		}
	}
	else if(cpu->trace_branch) {
		cerr << "  No thread killed. Mask="
			<< GetCurrentMask() << endl;
	}
}

template <class CONFIG>
void ImplicitFlow<CONFIG>::CheckJoin()
{
	// TODO: if jumping outside the current loop, pop loop
	// Merge paths
	for(;InConditional() && GetNPC() == GetJoin();
	    cpu->stats[GetPC() - CONFIG::CODE_START].Execute(0))
	{
		// Joined
		// Restore last mask and join
		current_mask = PopMask();
		PopJoin();
	}
	
	if(InConditional() && GetNPC() > GetJoin())
	{
		// Jumped over join point
		// Go back following the other branch
		std::swap(join_stack.top(), warp.npc);
		// Invert condition
		current_mask = ~GetCurrentMask() & GetNextMask();
	}

	if(cpu->trace_mask) {
		cerr << " " << GetCurrentMask() << endl;
	}
	
}

template <class CONFIG>
std::bitset<CONFIG::WARP_SIZE> ImplicitFlow<CONFIG>::GetCurrentMask() const
{
	return current_mask;
}

template <class CONFIG>
typename CONFIG::address_t ImplicitFlow<CONFIG>::GetLoop() const
{
	if(loop_stack.empty()) {
		return 0;
	}
	return loop_stack.top();
}

template <class CONFIG>
typename CONFIG::address_t ImplicitFlow<CONFIG>::GetPC() const
{
	return warp.pc;
}

template <class CONFIG>
typename CONFIG::address_t ImplicitFlow<CONFIG>::GetNPC() const
{
	return warp.npc;
}

template <class CONFIG>
void ImplicitFlow<CONFIG>::PushLoop(address_t addr)
{
	loop_stack.push(addr);
}

template <class CONFIG>
void ImplicitFlow<CONFIG>::PopLoop()
{
	assert(!loop_stack.empty());
	loop_stack.pop();
}

template <class CONFIG>
std::bitset<CONFIG::WARP_SIZE> ImplicitFlow<CONFIG>::PopMask()
{
	std::bitset<CONFIG::WARP_SIZE> msk = mask_stack.top();
	mask_stack.pop();
	return msk;
}

template <class CONFIG>
std::bitset<CONFIG::WARP_SIZE> ImplicitFlow<CONFIG>::GetNextMask()
{
	return mask_stack.top();
}

template <class CONFIG>
void ImplicitFlow<CONFIG>::PushMask(std::bitset<CONFIG::WARP_SIZE> mask)
{
	mask_stack.push(mask);
}

template <class CONFIG>
bool ImplicitFlow<CONFIG>::InConditional() const
{
	return !join_stack.empty();
}

template <class CONFIG>
typename CONFIG::address_t ImplicitFlow<CONFIG>::GetJoin() const
{
	return join_stack.top();
}

template <class CONFIG>
void ImplicitFlow<CONFIG>::PushJoin(typename CONFIG::address_t addr)
{
	// TODO: raise exn if ovf
	assert(join_stack.size() < CONFIG::BRANCH_STACK_DEPTH);
	join_stack.push(addr);
}

template <class CONFIG>
typename CONFIG::address_t ImplicitFlow<CONFIG>::PopJoin()
{
	address_t addr = join_stack.top();
	join_stack.pop();
	return addr;
}

template<class CONFIG>
void ImplicitFlow<CONFIG>::Call(address_t target_addr)
{
	assert(false);	// TODO
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
