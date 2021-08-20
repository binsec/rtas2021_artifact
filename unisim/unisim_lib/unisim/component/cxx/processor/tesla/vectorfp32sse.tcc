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

#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_VECTORFP32SSE_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_VECTORFP32SSE_TCC

#include <unisim/component/cxx/processor/tesla/vectorfp32.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/component/cxx/processor/tesla/simfloat.hh>

#include <cmath>
#include <cfloat>
#include <xmmintrin.h>
#include <emmintrin.h>	// For DAZ mode

// Workaround for gcc bug 21408
#define MM_DAZ_MASK	0x0040
#define MM_DAZ_ON	0x0040
#define MM_DAZ_OFF	0x0000

inline unsigned int MM_GET_DAZ_MODE ()
{
	return _mm_getcsr() & MM_DAZ_MASK;
}

inline void MM_SET_DAZ_MODE (unsigned int mode)
{
	_mm_setcsr((_mm_getcsr() & ~MM_DAZ_MASK) | mode);
}
// end workaround

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template <class CONFIG>
void VectorFP32SSE<CONFIG>::Init()
{
	// Set FTZ and DAZ globally?
	//_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	//MM_SET_DAZ_MODE(MM_DAZ_ON);
}

inline __m128 Saturate(__m128 x)
{
	// TODO: Saturate(-0) = ?
	// SSE min/max are non-commutative.
	_mm_max_ps(x, _mm_castsi128_ps(_mm_setzero_si128()));
	_mm_min_ps(x, _mm_set1_ps(1.0f));
	return x;
}

inline void SetDAZFTZ()
{
	_mm_setcsr(_mm_getcsr() | (_MM_FLUSH_ZERO_ON | MM_DAZ_ON));
}

inline void ClearDAZFTZ()
{
	_mm_setcsr(_mm_getcsr() & (~_MM_FLUSH_ZERO_MASK & ~MM_DAZ_MASK));
}

struct NoDenorm
{
	NoDenorm() { SetDAZFTZ(); }
	~NoDenorm() { ClearDAZFTZ(); }
};

template <class CONFIG>
VectorRegister<CONFIG> & VectorFP32SSE<CONFIG>::Mad(VectorRegister<CONFIG> & a,
	VectorRegister<CONFIG> const & b,
	VectorRegister<CONFIG> const & c,
	uint32_t neg_a, uint32_t neg_b, uint32_t neg_c,
	uint32_t rounding_mode, uint32_t sat)
{
	assert(CONFIG::WARP_SIZE % 4 == 0);
	assert(!neg_b);
	typedef VectorRegister<CONFIG> VecReg;

	NoDenorm nd;

	// G80-GT200: first mul always rounded toward zero
	_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);
	
	__m128 sign_mask = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; i += 4)
	{
		// TODO: force alignment of VectorRegister (or just switch to Nehalem...)
		__m128 sa = _mm_loadu_ps((float*)a.v + i);
		__m128 sb = _mm_loadu_ps((float*)b.v + i);
		if(neg_a) {
			sa = _mm_xor_ps(sa, sign_mask);
		}
		//if(neg_b) {
		//	sb = _mm_xor_ps(sb, sign_mask);
		//}
		__m128 r = _mm_mul_ps(sa, sb);
		_mm_storeu_ps((float*)a.v + i, r);
	}
	
	if(rounding_mode == RM_RN) {
		_MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST);
	}
	else if(rounding_mode == RM_RZ) {
		// keep current rounding mode (TZ)
	}
	else {
		assert(false);
	}
	
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; i += 4)
	{
		__m128 sa = _mm_loadu_ps((float*)a.v + i);
		__m128 sc = _mm_loadu_ps((float*)c.v + i);
		if(neg_c) {
			sc = _mm_xor_ps(sc, sign_mask);
		}
		
		__m128 r = _mm_add_ps(sa, sc);
		if(sat) {
			r = Saturate(r);
		}
		_mm_storeu_ps((float*)a.v + i, r);
	}
	
	if(rounding_mode != RM_RN) {
		// Restore RM
		_MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST);
	}

	a.SetStrided(false);
	a.SetScalar(a.IsScalar() && b.IsScalar() && c.IsScalar());
	return a;
}

template <class CONFIG>
VectorRegister<CONFIG> & VectorFP32SSE<CONFIG>::Mul(VectorRegister<CONFIG> & a,
	VectorRegister<CONFIG> const & b,
	uint32_t neg_a, uint32_t neg_b,
	uint32_t rounding_mode, uint32_t sat)
{
	assert(CONFIG::WARP_SIZE % 4 == 0);

	NoDenorm nd;
	
	__m128 sign_mask = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));

	if(rounding_mode == RM_RZ) {
		_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);
	}
	else {
		assert(rounding_mode == RM_RN);
	}

	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; i += 4)
	{
		__m128 sa = _mm_loadu_ps((float*)a.v + i);
		__m128 sb = _mm_loadu_ps((float*)b.v + i);

		if(neg_a) {
			sa = _mm_xor_ps(sa, sign_mask);
		}
		if(neg_b) {
			sb = _mm_xor_ps(sb, sign_mask);
		}
		__m128 r = _mm_mul_ps(sa, sb);
		if(sat) {
			r = Saturate(r);
		}
		_mm_storeu_ps((float*)a.v + i, r);
	}
	if(rounding_mode != RM_RN) {
		// Restore RM
		_MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST);
	}
	a.SetStrided(false);
	a.SetScalar(a.IsScalar() && b.IsScalar());
	return a;
}

template <class CONFIG>
VectorRegister<CONFIG> & VectorFP32SSE<CONFIG>::Add(VectorRegister<CONFIG> & a,
	VectorRegister<CONFIG> const & b,
	uint32_t neg_a, uint32_t neg_b,
	uint32_t rounding_mode, uint32_t sat)
{
	NoDenorm nd;

	__m128 sign_mask = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
	if(rounding_mode == RM_RZ) {
		_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);
	}
	else {
		assert(rounding_mode == RM_RN);
	}
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; i += 4)
	{
		__m128 sa = _mm_loadu_ps((float*)a.v + i);
		__m128 sb = _mm_loadu_ps((float*)b.v + i);

		if(neg_a) {
			sa = _mm_xor_ps(sa, sign_mask);
		}
		if(neg_b) {
			sb = _mm_xor_ps(sb, sign_mask);
		}
		__m128 r = _mm_add_ps(sa, sb);
		if(sat) {
			r = Saturate(r);
		}
		_mm_storeu_ps((float*)a.v + i, r);
	}
	if(rounding_mode != RM_RN) {
		// Restore RM
		_MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST);
	}
	a.SetStrided(false);
	a.SetScalar(a.IsScalar() && b.IsScalar());
	return a;
}

template<class CONFIG>
VectorRegister<CONFIG> & VectorFP32SSE<CONFIG>::Rcp(VectorRegister<CONFIG> & a)
{
	NoDenorm nd;
	// Do NOT use the SSE reciprocal estimate, it's way too unprecise
	// A NR iteration would be faster:
	// On Penryn
	// DIVPS = 12 cycles
	// RCPPS + 2xMULPS + SUBPS < 5 cycles
	__m128 one = _mm_set1_ps(1.0f);
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; i += 4)
	{
		__m128 sa = _mm_loadu_ps((float*)a.v + i);

		__m128 r = _mm_div_ps(one, sa);
		_mm_storeu_ps((float*)a.v + i, r);
	}
	if(!a.IsScalar()) a.SetStrided(false);
	return a;
}

template<class CONFIG>
VectorRegister<CONFIG> & VectorFP32SSE<CONFIG>::Rsq(VectorRegister<CONFIG> & a)
{
	NoDenorm nd;
	__m128 half = _mm_set1_ps(0.5f);
	__m128 three = _mm_set1_ps(3.0f);
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; i += 4)
	{
		__m128 sa = _mm_loadu_ps((float*)a.v + i);

		__m128 x = _mm_rsqrt_ps(sa);
		// Newton-raphson iteration
		// x = .5x * (3 - a * x^2)
		__m128 e = _mm_mul_ps(sa, _mm_mul_ps(x, x));
		__m128 x2 = _mm_mul_ps(_mm_mul_ps(x, half), _mm_sub_ps(three, e));
		__m128 isnan = _mm_cmpunord_ps(x2, x2);
		x2 = _mm_or_ps(_mm_and_ps(x, isnan), _mm_andnot_ps(isnan, x2));
		_mm_storeu_ps((float*)a.v + i, x2);
	}
	if(!a.IsScalar()) a.SetStrided(false);
	return a;
}

#if 0
template<class CONFIG>
VectorRegister<CONFIG> VectorFP32SSE<CONFIG>::Exp2(VectorRegister<CONFIG> const & a)
{
	NoDenorm nd;
	__m128 half = _mm_set1_ps(0.5f);
	__m128 three = _mm_set1_ps(3.0f);
	__m128 one = _mm_set1_ps(1.0f);
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; i += 4)
	{
		__m128 sa = _mm_loadu_ps((float*)a.v + i);
		
		// Input in FX sv7.23
		
		//__m128 x = _mm_slli_epi32(sa, 9);	// Reduced argument. FX .31
		__m128 x = _mm_and_si128(sa, _mm_set1_epi32(0x007fffff));
		
		__m128 exponent = _mm_and_si128(sa, _mm_set1_epi32(0x3f800000));
		
		// if(sa & 0x8000000) {
		//  x = 0x00800000 - x;
		//  exponent = -exponent;

		x = _mm_or_si128(x, _mm_set1_epi32(0x3f800000));
		// x in [1,2[
		
		// Brute-force exp2(x-1) with a Remez polynomial:
		// 8357823 * 2^(-24) + x * (5942233 * 2^(-24) + x * (14392689 * 2^(-27)
		// + x * (10484857 * 2^(-28) + x * (-9265639 * 2^(-34) + x * 16290301 * 2^(-33)))))

		// Should use 0 as 0th-order coeff...
		__m128 c0 = _mm_set1_ps(8357823.f / (1 << 24));
		__m128 c1 = _mm_set1_ps(5942233.f / (1 << 24));
		__m128 c2 = _mm_set1_ps(14392689.f / (1 << 27));
		__m128 c3 = _mm_set1_ps(10484857.f / (1 << 28));
		__m128 c4 = _mm_set1_ps(-9265639.f / (1 << 34));
		__m128 c5 = _mm_set1_ps(16290301.f / (1 << 33));
		
		__m128 y = _mm_add_ps(c4, _mm_mul_ps(x, c5));
		y = _mm_add_ps(_mm_mul_ps(y, x), c3);
		y = _mm_add_ps(_mm_mul_ps(y, x), c2);
		y = _mm_add_ps(_mm_mul_ps(y, x), c1);
		y = _mm_add_ps(_mm_mul_ps(y, x), c0);
		
		// Reconstruction
		// y should be in [1, 2[
		y = _mm_add_epi32(y, exponent);
		
		// If overflow return infinity same sign
		_mm_storeu_ps((float*)rv.v + i, y);
	}


	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		uint32_t ia = a[i];
		double r = FXToFP(ia);
		r = exp2(float(r));
		// Flush denormals to zero
		if(r < ldexp(1.0, FLT_MIN_EXP)) {	// r >= 0
			r = 0;
		}
		rv.WriteFloat(float(r), i);
		
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}
#endif

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif

