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

#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_WARP_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_WARP_TCC

#include <cassert>

#include <unisim/component/cxx/processor/tesla/warp.hh>
#include <unisim/component/cxx/processor/tesla/cpu.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {


template <class CONFIG>
Warp<CONFIG>::Warp() :
	flow(*this)
{
}

template <class CONFIG>
void Warp<CONFIG>::Reset(unsigned int wid, unsigned int gpr_num, CTA<CONFIG> * cta,
	std::bitset<CONFIG::WARP_SIZE> init_mask, CPU<CONFIG> * cpu)
{
	pc = CONFIG::CODE_START;
	npc = 0;
	
	gpr_window_size = gpr_num;
	gpr_window_base = gpr_num * wid;
	
	this->cta = cta;
	id = wid;
	
	flow.Reset(cpu, init_mask);
	
	for(unsigned int i = 0; i != CONFIG::MAX_PRED_REGS; ++i) {
		pred_flags[i].Reset();
	}
	
	for(unsigned int i = 0; i != CONFIG::MAX_ADDR_REGS; ++i) {
		addr[i].Reset();
	}
	
	
	state = Active;
	if(cpu->TraceReset()) {
		cerr << " Warp " << id << ": reset\n";
		cerr << "  " << gpr_window_size << " GPRs from " << gpr_window_base << "\n";
	}
}

template <class CONFIG>
uint32_t Warp<CONFIG>::GetGPRAddress(uint32_t reg) const
{
	// 32-bit access
	// No special register here
	if(reg >= gpr_window_size) {
		std::cerr << "Warp " << id << ": accessing r" << reg << " on a " << gpr_window_size << " reg window\n";
	}
	assert(reg < gpr_window_size);
	return gpr_window_base + reg;
}

template <class CONFIG>
typename CONFIG::address_t Warp<CONFIG>::GetSMAddress(uint32_t sm) const
{
	return cta->GetSMAddress(sm);
}

template <class CONFIG>
unsigned int Warp<CONFIG>::CTAID() const
{
	return cta->id;
}

//////////////////////////////////////////////////////////////////////


template <class CONFIG>
void CTA<CONFIG>::Reset(unsigned int bid, unsigned int sm_size,
	address_t sm_base)
{
	id = bid;
	sm_window_base = sm_base + sm_size * bid;
	sm_window_size = sm_size;
}

template <class CONFIG>
typename CONFIG::address_t CTA<CONFIG>::GetSMAddress(uint32_t sm) const
{
	assert(sm < sm_window_size);
	return sm_window_base + sm;
}


} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
