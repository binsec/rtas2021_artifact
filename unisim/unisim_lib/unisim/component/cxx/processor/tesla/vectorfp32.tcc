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

#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_VECTORFP32_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_VECTORFP32_TCC

#include <unisim/component/cxx/processor/tesla/vectorfp32.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/component/cxx/processor/tesla/simfloat.hh>

#include <cmath>
#include <cfloat>

#if defined(__SSE2__) && !defined(NO_SSE)
#include <unisim/component/cxx/processor/tesla/vectorfp32sse.tcc>
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {


using namespace unisim::util::arithmetic;



template <class CONFIG>
VectorRegister<CONFIG> & VectorFP32Base<CONFIG>::Mad(VectorRegister<CONFIG> & a,
	VectorRegister<CONFIG> const & b,
	VectorRegister<CONFIG> const & c,
	uint32_t neg_a, uint32_t neg_b, uint32_t neg_c,
	uint32_t rounding_mode, uint32_t sat)
{
	typedef VectorRegister<CONFIG> VecReg;
	typedef typename CONFIG::float_t float_t;
	typedef typename float_t::StatusAndControlFlags FPFlags;

	// G80-GT200: first mul always rounded to zero
	FPFlags tempflags;
	tempflags.setZeroRound();	// cannot use return value from setZeroRound(): wrong type!
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t sa = a.ReadSimfloat(i);
		float_t sb = b.ReadSimfloat(i);
		if(neg_a) {
			sa.opposite();
		}
		if(neg_b) {
			sb.opposite();
		}
		
		
		float_t r = sa.multAssign(sb, tempflags);
		a.WriteSimfloat(r, i);
	}
	
	FPFlags flags;
	if(rounding_mode == RM_RN) {
		flags.setNearestRound();
	}
	else if(rounding_mode == RM_RZ) {
		flags.setZeroRound();
	}
	else {
		assert(false);
	}
	
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t sa = a.ReadSimfloat(i);
		float_t sc = c.ReadSimfloat(i);
		if(neg_c) {
			sc.opposite();
		}
		
		float_t r = sa.plusAssign(sc, flags);
		if(sat) {
			r.saturate();
		}
		a.WriteSimfloat(r, i);
	}
	
	a.SetStrided(false);
	a.SetScalar(a.IsScalar() && b.IsScalar() && c.IsScalar());
	return a;
}

template <class CONFIG>
VectorRegister<CONFIG> & VectorFP32Base<CONFIG>::Mul(VectorRegister<CONFIG> & a,
	VectorRegister<CONFIG> const & b,
	uint32_t neg_a, uint32_t neg_b,
	uint32_t rounding_mode, uint32_t sat)
{
	typedef typename CONFIG::float_t float_t;
	typedef typename float_t::StatusAndControlFlags FPFlags;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t sa = a.ReadSimfloat(i);
		float_t sb = b.ReadSimfloat(i);
		if(neg_a) {
			sa.opposite();
		}
		if(neg_b) {
			sb.opposite();
		}
		
		FPFlags flags;
		if(rounding_mode == RM_RN) {
			flags.setNearestRound();
		}
		else if(rounding_mode == RM_RZ) {
			flags.setZeroRound();
		}
		else {
			assert(false);
		}
		
		float_t r = sa.multAssign(sb, flags);
		
		if(sat) {
			r.saturate();
		}
		a.WriteSimfloat(r, i);
	}
	a.SetStrided(false);
	a.SetScalar(a.IsScalar() && b.IsScalar());
	return a;
}

template <class CONFIG>
VectorRegister<CONFIG> & VectorFP32Base<CONFIG>::Add(VectorRegister<CONFIG> & a,
	VectorRegister<CONFIG> const & b,
	uint32_t neg_a, uint32_t neg_b,
	uint32_t rounding_mode, uint32_t sat)
{
	typedef typename CONFIG::float_t float_t;
	typedef typename float_t::StatusAndControlFlags FPFlags;
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t sa = a.ReadSimfloat(i);
		float_t sb = b.ReadSimfloat(i);
		if(neg_a) {
			sa.opposite();
		}
		if(neg_b) {
			sb.opposite();
		}
		
		FPFlags flags;
		if(rounding_mode == RM_RN) {
			flags.setNearestRound();
			flags.setRoundToEven();
		}
		else if(rounding_mode == RM_RZ) {
			flags.setZeroRound();
		}
		else {
			assert(false);
		}
		
		float_t r = sa.plusAssign(sb, flags);
		
		if(sat) {
			r.saturate();
		}
		a.WriteSimfloat(r, i);
	}
	a.SetScalar(a.IsScalar() && b.IsScalar());
	a.SetStrided(false);
	return a;
}

template <class CONFIG>
VectorRegister<CONFIG> & VectorFP32Base<CONFIG>::Mov(VectorRegister<CONFIG> & a, bool neg)
{
	typedef typename CONFIG::float_t float_t;
	typedef typename float_t::StatusAndControlFlags FPFlags;
	if(neg) {
		for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
		{
			float_t sa = a.ReadSimfloat(i);
			sa.opposite();
			a.WriteSimfloat(sa, i);
		}
	}
	//a.SetScalar(a.IsScalar());
	return a;
}


// Int to float
template<class CONFIG>
VectorRegister<CONFIG> VectorFP32Base<CONFIG>::ConvertI2F(VectorRegister<CONFIG> const & a, uint32_t rounding_mode, uint32_t cvt_type, bool b32)
{
	typedef typename CONFIG::float_t float_t;
	typedef typename float_t::StatusAndControlFlags FPFlags;
	// cvt_type = *SOURCE* type
	// b32 = ??? type
	assert(b32);

	FPFlags flags;
	switch(rounding_mode)
	{
	case RM_RN:
		flags.setNearestRound();
		break;
	case RM_RZ:
		flags.setZeroRound();
		break;
	case RM_RD:
		flags.setLowestRound();
		break;
	case RM_RU:
		flags.setHighestRound();
		break;
	}

	// TODO: u32/s32???
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t r;
		switch(cvt_type)
		{
		case CT_U32:
		{
			uint32_t ra = a[i];
			r.setInteger(ra, flags);
			break;
		}
		case CT_U16:
		{
			uint16_t ra = a[i] & 0xffff;
			r.setInteger(ra, flags);
			break;
		}
		case CT_U8:
		{
			uint8_t ra = a[i] & 0xff;
			r.setInteger(ra, flags);
			break;
		}
		case CT_S32:
		{
			int32_t ra = a[i];
			r.setInteger(ra, flags);
			break;
		}
		case CT_S16:
		{
			int16_t ra = a[i] & 0xffff;
			r.setInteger(ra, flags);
			break;
		}
		case CT_S8:
		{
			int8_t ra = a[i] & 0xff;
			r.setInteger(ra, flags);
			break;
		}
		default:
			assert(false);
		}
		rv.WriteSimfloat(r, i);
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
void VectorFP32Base<CONFIG>::ConvertF2F(VectorRegister<CONFIG> & a, bool dest_32, ConvType srctype,
	RoundingMode cvt_round, bool cvt_int, AbsSat abssat)
{
	typedef typename CONFIG::float_t float_t;
	typedef typename CONFIG::half_t half_t;	
	typedef typename float_t::StatusAndControlFlags FPFlags;

	FPFlags flags;
	switch(cvt_round)
	{
	case RM_RN:
		flags.setNearestRound();
		//flags.setRoundToEven();
		break;
	case RM_RZ:
		flags.setZeroRound();
		break;
	case RM_RD:
		flags.setLowestRound();
		break;
	case RM_RU:
		flags.setHighestRound();
		break;
	}

	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t f;
		if(srctype == CT_U32) {
			f = a.ReadSimfloat(i);
		}
		else {
			// Upconvert to Binary32
			//half_t h = half_t(a[i]);
			//f.assign(h, flags);	// lossless
			assert(false);	// TODO
		}
		
		AbsSaturate(f, abssat);
		
		
		if(cvt_int /*&& f.queryExponent() < 24*/)
		{
			f.roundToInt(flags);
		}
		
		if(dest_32) {
			a.WriteSimfloat(f, i);
		}
		else {
			// Downconvert to Binary16
			//half_t h;
			//h.assign(f, flags);	// According to rounding mode
								// BUG when rounding from fp32 to fp16, number representable in
								// float but not half and round toward zero?
			assert(false);
		}
		
	}
	a.SetStrided(false);
}

template<class CONFIG>
void VectorFP32Base<CONFIG>::AbsSaturate(typename CONFIG::float_t & f, AbsSat abssat)
{
	switch(abssat)
	{
	case AS_ABS:
		f.setPositive();
		break;
	case AS_SAT:
		// Unsigned saturation
		f.saturate();
		break;
	case AS_SSAT:
		// Signed saturation
		f.signedSaturate();
		break;
	case AS_NONE:
		break;
	}
}

// float to int
template<class CONFIG>
void VectorFP32Base<CONFIG>::ConvertF2I(VectorRegister<CONFIG> & a, bool issigned, bool dest_32, ConvType srctype,
	RoundingMode cvt_round)
{
	typedef typename CONFIG::float_t float_t;
	typedef typename CONFIG::half_t half_t;	
	typedef typename float_t::StatusAndControlFlags FPFlags;
	FPFlags flags;
	switch(cvt_round)
	{
	case RM_RN:
		flags.setNearestRound();
		break;
	case RM_RZ:
		flags.setZeroRound();
		break;
	case RM_RD:
		flags.setLowestRound();
		break;
	case RM_RU:
		flags.setHighestRound();
		break;
	}

	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t f;
		if(srctype == CT_U32) {
			f = a.ReadSimfloat(i);
		}
		else {
			// Upconvert to Binary32
			//half_t h = half_t(a[i]);
			//f.assign(h, flags);	// lossless
			assert(false);
		}

		//typename float_t::IntConversion conv;
		uint32_t result;
		if(issigned) {
			//conv.setSigned();
			int32_t t;
			f.retrieveInteger(t, flags);
			result = t;
		}
		else {
			//conv.setUnsigned();
			f.retrieveInteger(result, flags);
		}
		//f.retrieveInteger(conv, flags);	// Rounded
		if(dest_32) {
			a[i] = result;//conv.queryValue();
		}
		else {
			assert(false);	// TODO: implement... one day
		}
	}
	a.SetStrided(false);
}

template<class CONFIG>
VectorFlags<CONFIG> VectorFP32Base<CONFIG>::ComputePredSetFP32(VectorRegister<CONFIG> & output,
	VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	SetCond sc,
	bool a_abs,
	bool b_abs)
{
	// TODO: check it only outputs boolean values
	// input:
	// SZ
	// 00  01  10  11
	// +1  0   -1  NaN
	static bool const truth_table[16][4] = {
		{ 0, 0, 0, 0 },	// FL
		{ 0, 0, 1, 0 },	// LT
		{ 0, 1, 0, 0 },	// EQ
		{ 0, 1, 1, 0 },	// LE
		{ 1, 0, 0, 0 },	// GT
		{ 1, 0, 1, 0 },	// NE
		{ 1, 1, 0, 0 },	// GE
		{ 1, 1, 1, 0 },	// NUM
		{ 0, 0, 0, 1 }, // NAN
		{ 0, 0, 1, 1 }, // LTU
		{ 0, 1, 0, 1 }, // EQU
		{ 0, 1, 1, 1 }, // LEU
		{ 1, 0, 0, 1 }, // GTU
		{ 1, 0, 1, 1 }, // NEU
		{ 1, 1, 0, 1 }, // GEU
		{ 1, 1, 1, 1 }  // TRU
	};
	
	VectorFlags<CONFIG> flags;
	
	flags.Reset();
	
	assert(sc < 16);
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		typename CONFIG::float_t fa = a.ReadSimfloat(i);
		typename CONFIG::float_t fb = b.ReadSimfloat(i);
		
		if(a_abs) {
			fa.setPositive();
		}
		if(b_abs) {
			fb.setPositive();
		}
		uint8_t cond = Compare(fa, fb);
		// cond in [0,4[
		assert(cond < 4);

		bool r = truth_table[sc][cond];
		output[i] = -r;	// G80Spec -> extend
	
		// TODO: CHECK is this correct (=ComputePredI32)?
		flags.SetZero(output[i] == 0, i);
		flags.SetSign(output[i] < 0, i);
	}
	return flags;
}

template <class CONFIG>
VectorFlags<CONFIG> VectorFP32Base<CONFIG>::ComputePredFP32(VectorRegister<CONFIG> const & output, std::bitset<CONFIG::WARP_SIZE> mask)
{
	// As tested on G80 and G86:
	//  +normal-> 0
	//  -normal-> 2
	//  +zero  -> 1
	//  -zero  -> 1
	//  -inf   -> 2
	//  +inf   -> 0
	//  NaN    -> 3
	// ZF = (Zero | NaN)
	// SF = (Neg | NaN)
	// Matches g80_spec, p. 348.
	VectorFlags<CONFIG> flags;
	flags.Reset();
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		// TODO: use softfloats
		// Just ignore flags
		float f = output.ReadFloat(i);
		uint8_t fl = 0;
		if(f > 0) {	// strictly positive, not NaN
			fl = 0;
		}
		else if(f == 0) {
			fl = 1;
		}
		else if(f < 0) {
			fl = 2;
		}
		else {
			fl = 3;	// NaN
		}
		flags.v[i] = fl;
	}
	return flags;
}



template<class CONFIG>
inline uint8_t VectorFP32Base<CONFIG>::Compare(typename CONFIG::float_t a, typename CONFIG::float_t b)
{
	switch(a.compare(b))
	{
	case CONFIG::float_t::CRNaN:
		return 3;
	case CONFIG::float_t::CRLess:
		return 2;
	case CONFIG::float_t::CREqual:
		return 1;
	case CONFIG::float_t::CRGreater:
		return 0;
	default:
		assert(false);
	}
}

//////////////////////////////////////////////////////////////////////
// Transcendantals, rcp, rsq
// Currently based on host libm
// Deviates from actual hardware (~round to nearest)
// Assumes host is little-endian!
// TODO: Rewrite from scratch using softfloats and detailed models

template<class CONFIG>
VectorRegister<CONFIG> & VectorFP32Base<CONFIG>::Rcp(VectorRegister<CONFIG> & a)
{
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float r = 1.f / a.ReadFloat(i);
		if(r > -ldexp(1.0, FLT_MIN_EXP) && r < ldexp(1.0, FLT_MIN_EXP)) {
			r = 0;
		}
		a.WriteFloat(r, i);
	}
	a.SetStrided(false);
	a.SetScalar(a.IsScalar());
	return a;
}

template<class CONFIG>
VectorRegister<CONFIG> & VectorFP32Base<CONFIG>::Rsq(VectorRegister<CONFIG> & a)
{
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		// Intermediate comp. in double
		float r = float(1. / sqrt(double(a.ReadFloat(i))));
		a.WriteFloat(r, i);
	}
	a.SetStrided(false);
	a.SetScalar(a.IsScalar());
	return a;
}

template<class CONFIG>
VectorRegister<CONFIG> VectorFP32Base<CONFIG>::Log2(VectorRegister<CONFIG> const & a)
{
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float r = log2(a.ReadFloat(i));
		rv.WriteFloat(r, i);
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> VectorFP32Base<CONFIG>::RRExp2(VectorRegister<CONFIG> const & a)
{
	// Convert to FX sv7.23
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float f = a.ReadFloat(i);
		rv[i] = FPToFX(f);
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> VectorFP32Base<CONFIG>::RRTrig(VectorRegister<CONFIG> const & a)
{
	// Dirty range reduction (like actual hardware)
	// Multiply by 2/Pi
	// Convert to FX sv7.23
	// In which order??
	// 2/Pi on 24 (FP)
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float f = a.ReadFloat(i) * float(M_2_PI);	// Convert 2/pi to float first
		// Only keep fractional part??
		//f = f - floorf(f);
		rv[i] = FPToFX(f, true);	// sin(MAX_FLOAT) = 0, not NaN.
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> VectorFP32Base<CONFIG>::Exp2(VectorRegister<CONFIG> const & a)
{
	// Input in FX sv7.23
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

template<class CONFIG>
VectorRegister<CONFIG> VectorFP32Base<CONFIG>::Sin(VectorRegister<CONFIG> const & a)
{
	// Input in FX sv7.23
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		uint32_t ia = a[i];
		double r = FXToFP(ia);
		r = sin(r * .5 * M_PI);
		// Flush denormals to zero
		if(r > -ldexp(1.0, FLT_MIN_EXP) && r < ldexp(1.0, FLT_MIN_EXP)) {
			r = 0;
		}
		rv.WriteFloat(r, i);
		// In double precision
		// (until sinpif and cospif functions are available...)
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> VectorFP32Base<CONFIG>::Cos(VectorRegister<CONFIG> const & a)
{
	// Input in FX sv7.23
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		uint32_t ia = a[i];
		double r = FXToFP(ia);
		r = cos(r * .5 * M_PI);
		if(r > -ldexp(1.0, FLT_MIN_EXP) && r < ldexp(1.0, FLT_MIN_EXP)) {
			r = 0;
		}		
		rv.WriteFloat(r, i);
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
uint32_t VectorFP32Base<CONFIG>::FPToFX(float f, bool noovf)
{
	if(noovf)
		f = fmod(f, float(1 << 7));
		
	uint32_t r = uint32_t(lrint(fabs(ldexp(f, 23))));	// Round to nearest
	r &= 0x3fffffff;
	if(!(fabs(f) < float(1 << 7))) {
		// overflow
		if(!noovf)
			r |= 0x40800000;
	}

	if(isnan(f)) {
		r = 0x40000000;
	}
	
	r |= (FloatAsInt(f) & 0x80000000);	// <0 ?
	return r;
}

template<class CONFIG>
double VectorFP32Base<CONFIG>::FXToFP(uint32_t f)
{
	float r;
	if(f & 0x40000000) {
		if(f & ~0xc0000000) {	// ovf
			r = (f & 0x80000000) ? -INFINITY : INFINITY;	// Portable??
		}
		else {
			r = nanf((f & 0x80000000) ? "NAN(0xffc00000)" : "NAN(0x7fc00000)");
		}
	}
	else if(f & 0x80000000) {
		r = -ldexp(double(f & ~0x80000000), -23);
	}
	else {
		r = ldexp(double(f), -23);
	}
	return r;
}

template<class CONFIG>
uint32_t VectorFP32Base<CONFIG>::FloatAsInt(float f)
{
	union { float f; uint32_t i; } conv;
	conv.f = f;
	return conv.i;
}

template<class CONFIG>
float VectorFP32Base<CONFIG>::IntAsFloat(uint32_t i)
{
	union { float f; uint32_t i; } conv;
	conv.i = i;
	return conv.f;
}

// IEEE-754:2008-compliant min and max
template<class CONFIG>
VectorRegister<CONFIG> VectorFP32Base<CONFIG>::Min(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
	AbsSat abs_sat)
{
	typedef typename CONFIG::float_t float_t;
	typedef typename float_t::StatusAndControlFlags FPFlags;
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t sa = a.ReadSimfloat(i);
		float_t sb = b.ReadSimfloat(i);
		AbsSaturate(sa, abs_sat);	// Only applies to 1st operand (?)
		float_t r;
		if(sb < sa || sa.isNaN()) {
			r = sb;
		} else {
			r = sa;	// Return first argument if both args are +/- zero
		}
		rv.WriteSimfloat(r, i);
	}
	rv.SetScalar(a.IsScalar() && b.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> VectorFP32Base<CONFIG>::Max(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
	AbsSat abs_sat)
{
	typedef typename CONFIG::float_t float_t;
	typedef typename float_t::StatusAndControlFlags FPFlags;
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t sa = a.ReadSimfloat(i);
		float_t sb = b.ReadSimfloat(i);
		AbsSaturate(sa, abs_sat);	// Only applies to 1st operand (?)
		float_t r;
		if(sb > sa || sa.isNaN()) {
			r = sb;
		} else {
			r = sa;	// Return first argument if both args are +/- zero
		}
		rv.WriteSimfloat(r, i);
	}
	rv.SetScalar(a.IsScalar() && b.IsScalar());
	return rv;
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
