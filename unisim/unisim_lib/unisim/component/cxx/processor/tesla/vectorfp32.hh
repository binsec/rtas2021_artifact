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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_VECTORFP32_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_VECTORFP32_HH

#include <unisim/component/cxx/processor/tesla/register.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template<class CONFIG>
struct VectorFP32Base
{
	static void Init() {}

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
	static VectorRegister<CONFIG> & Mov(VectorRegister<CONFIG> & a, bool do_neg);

	static VectorRegister<CONFIG> ConvertI2F(VectorRegister<CONFIG> const & a, uint32_t cvt_round, uint32_t cvt_type, bool b32);

	// In-place
	static void ConvertF2F(VectorRegister<CONFIG> & a, bool dest_32, ConvType srctype,
		RoundingMode cvt_round, bool cvt_int, AbsSat abssat);

	// In-place
	static void ConvertF2I(VectorRegister<CONFIG> & a, bool issigned, bool dest_32, ConvType srctype,
		RoundingMode cvt_round);

	static uint8_t Compare(typename CONFIG::float_t a, typename CONFIG::float_t b);

	static VectorFlags<CONFIG> ComputePredSetFP32(VectorRegister<CONFIG> & output,
		VectorRegister<CONFIG> const & a,
		VectorRegister<CONFIG> const & b,
		SetCond sc,
		bool a_abs,
		bool b_abs);

	static VectorFlags<CONFIG> ComputePredFP32(VectorRegister<CONFIG> const & output,
		std::bitset<CONFIG::WARP_SIZE> mask);


	static VectorRegister<CONFIG> & Rcp(VectorRegister<CONFIG> & a);
	static VectorRegister<CONFIG> & Rsq(VectorRegister<CONFIG> & a);
	static VectorRegister<CONFIG> Log2(VectorRegister<CONFIG> const & a);
	static VectorRegister<CONFIG> RRExp2(VectorRegister<CONFIG> const & a);
	static VectorRegister<CONFIG> RRTrig(VectorRegister<CONFIG> const & a);
	static VectorRegister<CONFIG> Exp2(VectorRegister<CONFIG> const & a);
	static VectorRegister<CONFIG> Sin(VectorRegister<CONFIG> const & a);
	static VectorRegister<CONFIG> Cos(VectorRegister<CONFIG> const & a);

	static VectorRegister<CONFIG> Min(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b, AbsSat abs_sat);
	static VectorRegister<CONFIG> Max(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b, AbsSat abs_sat);

private:
	static void AbsSaturate(typename CONFIG::float_t & f, AbsSat abssat);
	static uint32_t FPToFX(float f, bool noovf = false);
	static double FXToFP(uint32_t f);
	static uint32_t FloatAsInt(float f);
	static float IntAsFloat(uint32_t i);
};
} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#if defined(__SSE2__) && !defined(NO_SSE)
#include <unisim/component/cxx/processor/tesla/vectorfp32sse.hh>
namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {
template<class CONFIG>
struct VectorFP32 : VectorFP32SSE<CONFIG>
{
};
} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
#else
#warning Not using SSE floating-point
namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {
template<class CONFIG>
struct VectorFP32 : VectorFP32Base<CONFIG>
{
};
} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif

#endif
