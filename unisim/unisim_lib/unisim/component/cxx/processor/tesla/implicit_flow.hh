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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_IMPLICIT_FLOW_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_IMPLICIT_FLOW_HH

//#include <unisim/component/cxx/processor/tesla/cpu.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template<class CONFIG> struct CPU;

template<class CONFIG> struct Warp;


template<class CONFIG>
struct ImplicitFlow
{
	typedef typename CONFIG::address_t address_t;
	
	ImplicitFlow(Warp<CONFIG> & warp);
	
	void Reset(CPU<CONFIG> * cpu, std::bitset<CONFIG::WARP_SIZE> mask);
	void Branch(address_t target, std::bitset<CONFIG::WARP_SIZE> mask);
	void Meet(address_t addr);
	bool Join();
	bool End();
	void Return(std::bitset<CONFIG::WARP_SIZE> mask);
	void Kill(std::bitset<CONFIG::WARP_SIZE> mask);
	void CheckJoin();
	std::bitset<CONFIG::WARP_SIZE> GetCurrentMask() const;
	void PreBreak(address_t addr);
	void Break(std::bitset<CONFIG::WARP_SIZE> mask);
	void Call(address_t addr);
	
private:
	address_t GetLoop() const;
	void PushLoop(address_t addr);
	void PopLoop();
	std::bitset<CONFIG::WARP_SIZE> PopMask();
	void PushMask(std::bitset<CONFIG::WARP_SIZE> mask);
	std::bitset<CONFIG::WARP_SIZE> GetNextMask();
	address_t GetPC() const;
	address_t GetNPC() const;
	bool InConditional() const;
	void PushJoin(address_t addr);
	address_t PopJoin();
	address_t GetJoin() const;


	CPU<CONFIG> * cpu;
	Warp<CONFIG> & warp;

	bitset<CONFIG::WARP_SIZE> current_mask;
	
	std::stack<address_t> join_stack;
	//std::stack<bitset<WARP_SIZE> > mask_stack;
	MaskStack<CONFIG::WARP_SIZE, CONFIG::BRANCH_STACK_DEPTH> mask_stack;
	std::stack<address_t> loop_stack;
	

};

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
