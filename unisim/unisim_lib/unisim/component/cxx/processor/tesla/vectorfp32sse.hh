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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_VECTORFP32SSE_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_VECTORFP32SSE_HH

#include <unisim/component/cxx/processor/tesla/register.hh>


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template<class CONFIG>
struct VectorFP32SSE : VectorFP32Base<CONFIG>
{
	static void Init();
	static VectorRegister<CONFIG> & Mad(VectorRegister<CONFIG> & a,
		VectorRegister<CONFIG> const & b,
		VectorRegister<CONFIG> const & c,
		uint32_t nega, uint32_t negb, uint32_t negc,
		uint32_t rounding_mode, uint32_t sat = 0);

	static VectorRegister<CONFIG> & Mul(VectorRegister<CONFIG> & a, VectorRegister<CONFIG> const & b,
		uint32_t nega, uint32_t negb,
		uint32_t rounding_mode, uint32_t sat = 0);
	static VectorRegister<CONFIG> & Add(VectorRegister<CONFIG> & a, VectorRegister<CONFIG> const & b,
		uint32_t nega, uint32_t negb,
		uint32_t rounding_mode, uint32_t sat = 0);

	static VectorRegister<CONFIG> & Rcp(VectorRegister<CONFIG> & a);
	static VectorRegister<CONFIG> & Rsq(VectorRegister<CONFIG> & a);
	//static VectorRegister<CONFIG> Exp2(VectorRegister<CONFIG> const & a);

private:
	static unsigned int const WARP_BLOCKS = CONFIG::WARP_SIZE / 4;

};

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
