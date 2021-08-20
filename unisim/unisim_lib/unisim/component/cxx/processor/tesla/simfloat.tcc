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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_SIMFLOAT_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_SIMFLOAT_TCC__

#include <unisim/component/cxx/processor/tesla/simfloat.hh>

#include <unisim/util/simfloat/floating.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {


inline SoftFloatIEEE& SoftFloatIEEE::assign(const SoftHalfIEEE& sfHalf,
	inherited::StatusAndControlFlags & flags)
{
	// Always exact
	FloatConversion fcConversion;
	fcConversion.setSizeMantissa(10).setSizeExponent(5);
	fcConversion.setNegative(sfHalf.isNegative());
	fcConversion.exponent()[0] = sfHalf.queryBasicExponent()[0];
	fcConversion.mantissa()[0] = sfHalf.queryMantissa()[0];
	inherited source(fcConversion, flags);
	return (SoftFloatIEEE&) inherited::operator=(source);
}

inline SoftFloatIEEE& SoftFloatIEEE::saturate()
{
	if(isNegative()) {
		setZero();
	}
	else if(hasPositiveOrNullExponent()) {	// >= 1
		setOne();
	}
	return *this;
}

inline SoftFloatIEEE& SoftFloatIEEE::signedSaturate()
{
	if(hasPositiveOrNullExponent()) {	// >= 1.0?
		setOnePreserveSign();
	}
	return *this;
}

inline void SoftFloatIEEE::retrieveInteger(uint32_t& result, StatusAndControlFlags& flags) const
{
	flags.clear();
	IntConversion ic;
	ic.setSize(32);
	ic.setUnsigned();
	inherited::retrieveInteger(ic, flags);
	result = ic.queryValue();
}

inline void SoftFloatIEEE::retrieveInteger(int32_t& result, StatusAndControlFlags& flags) const
{
	flags.clear();
	IntConversion ic;
	ic.setSize(32);
	ic.setSigned();
	inherited::retrieveInteger(ic, flags);
	result = ic.queryValue();
}

inline void SoftFloatIEEE::retrieveInteger(uint16_t& result, StatusAndControlFlags& flags) const
{
	flags.clear();
	IntConversion ic;
	ic.setSize(16);
	ic.setUnsigned();
	inherited::retrieveInteger(ic, flags);
	result = ic.queryValue();
}

inline void SoftFloatIEEE::retrieveInteger(int16_t& result, StatusAndControlFlags& flags) const
{
	flags.clear();
	IntConversion ic;
	ic.setSize(16);
	ic.setSigned();
	inherited::retrieveInteger(ic, flags);
	result = ic.queryValue();
}

SoftHalfIEEE& SoftHalfIEEE::assign(const SoftFloatIEEE& sfFloat,
	inherited::StatusAndControlFlags & flags)
{
	FloatConversion fcConversion;
	fcConversion.setSizeMantissa(23).setSizeExponent(8);
	fcConversion.setNegative(sfFloat.isNegative());
	fcConversion.exponent()[0] = sfFloat.queryBasicExponent()[0];
	fcConversion.mantissa()[0] = sfFloat.queryMantissa()[0];
	inherited source(fcConversion, flags);
	return (SoftHalfIEEE&) inherited::operator=(source);

}

template<class BaseFloat>
FloatDAZFTZ<BaseFloat>&
FloatDAZFTZ<BaseFloat>::assign(const SoftFloatIEEE& sfFloat,
	FloatDAZFTZ<BaseFloat>::Flags & flags)
{
	BaseFloat::assign(sfFloat, flags);
	flushDenormals();
	return *this; 
}

template<class BaseFloat>
FloatDAZFTZ<BaseFloat>&
FloatDAZFTZ<BaseFloat>::plusAssign(const FloatDAZFTZ<BaseFloat>& bdSource,
	FloatDAZFTZ<BaseFloat>::Flags& scfFlags)
{
	flushDenormals();
	thisType source = bdSource;
	source.flushDenormals();
	// can be zero
//	BaseFloat::plusAssignSureNN(source, scfFlags);
	BaseFloat::plusAssign(source, scfFlags);
	flushDenormals();
	return *this;
}


template<class BaseFloat>
FloatDAZFTZ<BaseFloat>&
FloatDAZFTZ<BaseFloat>::minusAssign(const FloatDAZFTZ<BaseFloat>& bdSource,
	FloatDAZFTZ<BaseFloat>::Flags& scfFlags)
{
	flushDenormals();
	thisType source = bdSource;
	source.flushDenormals();
	BaseFloat::minusAssign(source, scfFlags);
	flushDenormals();
	return *this;
}

template<class BaseFloat>
FloatDAZFTZ<BaseFloat>&
FloatDAZFTZ<BaseFloat>::multAssign(const FloatDAZFTZ<BaseFloat>& bdSource,
	FloatDAZFTZ<BaseFloat>::Flags& scfFlags)
{
	flushDenormals();
	thisType source = bdSource;
	source.flushDenormals();
	BaseFloat::multAssign(source, scfFlags);
	flushDenormals();
	return *this;
}

template<class BaseFloat>
FloatMAD<BaseFloat>&
FloatMAD<BaseFloat>::teslaMADAssign(const FloatMAD<BaseFloat>& bdMult,
	const FloatMAD<BaseFloat>& bdAdd, typename BaseFloat::StatusAndControlFlags& scfFlags)
{
	BaseFloat::multAssign(bdMult, typename BaseFloat::StatusAndControlFlags().setZeroRound());
	return BaseFloat::addAssign(bdAdd, scfFlags);
}


} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
