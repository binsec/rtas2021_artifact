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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_HOSTFLOAT_HOSTFLOAT_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_HOSTFLOAT_HOSTFLOAT_TCC

#include <unisim/component/cxx/processor/tesla/hostfloat/hostfloat.hh>
#include <unisim/component/cxx/processor/tesla/hostfloat/rounding.tcc>


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {
namespace hostfloat {

#if 0
HostFloat<T, Traits> & HostFloat<T, Traits>::assign(BuiltDouble const & source,
	StatusAndControlFlags const & flags)
{
	
}
#endif

template<class T, class Traits>
HostFloat<T, Traits>::HostFloat(IntType source)
{
	// TODO: Endianness conversion
	union {
		T f;
		IntType i;
	} conv;
	conv.i = source;
	impl = conv.f;
}

template<class T, class Traits>
typename HostFloat<T, Traits>::IntType HostFloat<T, Traits>::queryValue() const
{
	// TODO: Endianness conversion
	union {
		T f;
		IntType i;
	} conv;
	conv.f = impl;
	return conv.i;
}

template<class T, class Traits>
HostFloat<T, Traits>& HostFloat<T, Traits>::plusAssign(const thisType& other, const StatusAndControlFlags& flags)
{
	Rounding rnd(flags.roundingMode());
	Denormals dn(flags.isDAZ(), flags.isFTZ());
	T t = other.impl;
	if(flags.isDAZ()) {
		t = DC::ForceDAZ(t);
	}
	t = RC::ForceRounding(impl + t);
	if(flags.isFTZ()) {
		t = DC::ForceFTZ(t);
	}
	impl = t;
	return *this;
}

template<class T, class Traits>
HostFloat<T, Traits>& HostFloat<T, Traits>::minusAssign(const thisType& other, const StatusAndControlFlags& flags)
{
	Rounding rnd(flags.roundingMode());
	Denormals dn(flags.isDAZ(), flags.isFTZ());
	T t = other.impl;
	if(flags.isDAZ()) {
		t = DC::ForceDAZ(t);
	}
	t = RC::ForceRounding(impl - t);
	if(flags.isFTZ()) {
		t = DC::ForceFTZ(t);
	}
	impl = t;
	return *this;
}

template<class T, class Traits>
HostFloat<T, Traits>& HostFloat<T, Traits>::multAssign(const thisType& other, const StatusAndControlFlags& flags)
{
	Rounding rnd(flags.roundingMode());
	Denormals dn(flags.isDAZ(), flags.isFTZ());
	T t = other.impl;
	if(flags.isDAZ()) {
		t = DC::ForceDAZ(t);
	}
	t = RC::ForceRounding(impl * t);
	if(flags.isFTZ()) {
		t = DC::ForceFTZ(t);
	}
	impl = t;
	return *this;
}

template<class T, class Traits>
HostFloat<T, Traits>& HostFloat<T, Traits>::saturate()
{
	if(impl < FT::Zero()) {
		setZero();
	}
	else if(impl > FT::One()) {
		setOne();
	}
	return *this;
}

template<class T, class Traits>
HostFloat<T, Traits>& HostFloat<T, Traits>::signedSaturate()
{
	if(impl < FT::MinusOne()) {
		setMinusOne();
	}
	else if(impl > FT::One()) {
		setOne();
	}
	return *this;
}

template<class T, class Traits>
template<class IntConv>
void HostFloat<T, Traits>::retrieveInteger(IntConv& result, StatusAndControlFlags& flags) const
{
	Rounding rnd(flags.roundingMode());
	T r = RC::ToInt(impl);
	// Saturate
	if(r >= T(numeric_limits<IntConv>::max())){ // may round down, hence >=
		result = numeric_limits<IntConv>::max();
	}
	else if(r <= T(numeric_limits<IntConv>::min())) {
		result = numeric_limits<IntConv>::min();
	}
	else {
		result = IntConv(r);
	}
}

#if 0
template<class T, class Traits>
void HostFloat<T, Traits>::retrieveInteger(uint32_t& result, StatusAndControlFlags& flags) const
{
}

template<class T, class Traits>
void HostFloat<T, Traits>::retrieveInteger(int32_t& result, StatusAndControlFlags& flags) const
{
}

template<class T, class Traits>
void HostFloat<T, Traits>::retrieveInteger(uint16_t& result, StatusAndControlFlags& flags) const
{
}

template<class T, class Traits>
void HostFloat<T, Traits>::retrieveInteger(int16_t& result, StatusAndControlFlags& flags) const
{
}
#endif

template<class T, class Traits>
void HostFloat<T, Traits>::setInteger(uint32_t value, StatusAndControlFlags& flags)
{
	Rounding rnd(flags.roundingMode());
	// Denormals can't appear in conversion from int
	impl = RC::ForceRounding(T(value));
}

template<class T, class Traits>
void HostFloat<T, Traits>::setInteger(int32_t value, StatusAndControlFlags& flags)
{
	Rounding rnd(flags.roundingMode());
	// Denormals can't appear in conversion from int
	impl = RC::ForceRounding(T(value));
}

template<class T, class Traits>
HostFloat<T, Traits>& HostFloat<T, Traits>::roundToInt(StatusAndControlFlags & flags)
{
	Rounding rnd(flags.roundingMode());
	impl = RC::ToInt(impl);
	return *this;
}

template<class T, class Traits>
typename HostFloat<T, Traits>::ComparisonResult HostFloat<T, Traits>::compare(thisType const & other) const
{
	if(impl == other.impl) {
		return CREqual;
	}
	if(impl < other.impl) {
		return CRLess;
	}
	if(impl > other.impl) {
		return CRGreater;
	}
	return CRNaN;
}

} // end of namespace hostfloat
} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
