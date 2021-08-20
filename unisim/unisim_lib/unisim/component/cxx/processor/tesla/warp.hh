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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_WARP_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_WARP_HH

#include <bitset>
#include <unisim/component/cxx/processor/tesla/forward.hh>
#include <unisim/component/cxx/processor/tesla/flags.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template<class CONFIG>
struct Warp
{
	typedef typename CONFIG::address_t address_t;

	Warp();

	// wid unique across blocks
	void Reset(unsigned int wid, unsigned int gpr_num, CTA<CONFIG> * cta,
		std::bitset<CONFIG::WARP_SIZE> init_mask, CPU<CONFIG> * cpu);

	uint32_t GetGPRAddress(uint32_t reg) const;
	address_t GetSMAddress(uint32_t sm = 0) const;
	address_t GetLocalAddress(address_t addr, unsigned int segment) const;
	unsigned int CTAID() const;
	
	address_t pc;
	address_t npc;

	VectorFlags<CONFIG> pred_flags[CONFIG::MAX_PRED_REGS];
	VectorAddress<CONFIG> addr[CONFIG::MAX_ADDR_REGS];
	
	uint32_t gpr_window_base;
	uint32_t gpr_window_size;
	
	typename CONFIG::control_flow_t flow;

	enum WarpState {
		Active,
		WaitingFence,
		Finished
	};
	
	WarpState state;
	CTA<CONFIG> * cta;
	
	uint32_t id;	// for debugging purposes only
	//CPU<CONFIG> * cpu;
};


template<class CONFIG>
struct CTA
{
	typedef typename CONFIG::address_t address_t;
	CTA() {}

	void Reset(unsigned int bid, unsigned int sm_size,
		address_t sm_base);
	
	address_t GetSMAddress(uint32_t sm = 0) const;
	
	address_t sm_window_base;
	uint32_t sm_window_size;
	uint32_t id;
};

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
