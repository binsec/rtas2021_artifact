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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_HOSTFLOAT_HOSTFLOAT_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_HOSTFLOAT_HOSTFLOAT_HH

#include <unisim/component/cxx/processor/tesla/hostfloat/rounding.hh>
#include <unisim/component/cxx/processor/tesla/hostfloat/denormals.hh>
#include <unisim/component/cxx/processor/tesla/hostfloat/traits.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {
namespace hostfloat {

using namespace unisim::component::cxx::processor::tesla;

// Dummy flags class to keep (partial) compatibility with simfloat++
struct Flags
{
	typedef Flags StatusAndControlFlags;
	//typedef unisim::component::cxx::processor::tesla::RoundingMode RoundingMode;

private:
	RoundingMode rm;
	bool ftz;
	bool daz;

public:
	Flags() :
		rm(RM_RN), ftz(false), daz(false) {}
	
	StatusAndControlFlags& setRoundToEven() { rm = RM_RN; return *this; }
	StatusAndControlFlags& setNearestRound() { rm = RM_RN; return *this; }
	StatusAndControlFlags& setHighestRound() { rm = RM_RU; return *this; }
	StatusAndControlFlags& setLowestRound() { rm = RM_RD; return *this; }
	StatusAndControlFlags& setZeroRound() { rm = RM_RZ; return *this; }

	bool isLowestRound() const { return rm == RM_RD; }
	bool isNearestRound() const { return rm == RM_RN; }
	bool isHighestRound() const { return rm == RM_RU; }
	bool isZeroRound() const { return rm == RM_RZ; }

	void clear() {}
	
	StatusAndControlFlags& setFTZ() { ftz = true; return *this; }
	StatusAndControlFlags& setDAZ() { daz = true; return *this; }
	StatusAndControlFlags& clearFTZ() { ftz = false; return *this; }
	StatusAndControlFlags& clearDAZ() { daz = false; return *this; }
	bool isFTZ() const { return ftz; }
	bool isDAZ() const { return daz; }
	
	RoundingMode roundingMode() const { return rm; };

};

struct FlagsDAZDTZ : Flags
{
	typedef FlagsDAZDTZ StatusAndControlFlags;
	FlagsDAZDTZ() {
		setFTZ();
		setDAZ();
	}
};

struct DefaultTraits
{
	typedef BaseRoundingControl RoundingControl;
	typedef BaseDenormControl DenormControl;
	typedef Flags StatusAndControlFlags;
};

// Single-precision arithmetic with DAZ and FTZ modes
struct TeslaBinary32Traits
{
	typedef BaseRoundingControl RoundingControl;
	typedef BaseDenormControl DenormControl;
	typedef FlagsDAZDTZ StatusAndControlFlags;
};

// Double-precision arithmetic with denormals
struct TeslaBinary64Traits
{
	typedef BaseRoundingControl RoundingControl;
	typedef BaseDenormControl DenormControl;
	typedef Flags StatusAndControlFlags;
};


template<class IntType>
struct IntConversion
{
	typedef IntConversion<IntType> thisType;
private:
	IntType impl;
	bool fUnsigned;

public:
	IntConversion() : fUnsigned(false) {}
	IntConversion(thisType const & other) :
		impl(other.impl) {}

	thisType& setSigned() { fUnsigned = false; return *this; }
	thisType& setUnsigned() { fUnsigned = true; return *this; }

	// TODO
};

template<class T, class Traits = DefaultTraits>
struct HostFloat
{
	typedef typename Traits::StatusAndControlFlags StatusAndControlFlags;
	typedef typename Traits::RoundingControl RC;
	typedef typename Traits::DenormControl DC;
	typedef TRounding<RC> Rounding;
	typedef TDenormals<DC> Denormals;
	typedef FloatTraits<T> FT;
	typedef typename FT::IntType IntType;

protected:
	typedef HostFloat<T, Traits> thisType;

private:
	T impl;

public:
	HostFloat() {}
	HostFloat(T source) { impl = source; }
	HostFloat(thisType const & source) { impl = source.impl; }
	HostFloat(IntType source);
	
	//thisType & assign(BuiltDouble const & source, StatusAndControlFlags const & flags);

	IntType queryValue() const;
	
	thisType operator+() const {
		return *this;
	}

	thisType operator-() const {
		return thisType(-impl);
	}


#if 0	
	thisType& operator+=(const thisType& other) {
		impl = RC::ForceRounding(impl + other.impl);
		return *this;
	}

	thisType& operator-=(const thisType& other) {
		impl = RC::ForceRounding(impl - other.impl);
		return *this;
	}
	
	thisType& operator*=(const thisType& other) {
		impl = RC::ForceRounding(impl * other.impl);
		return *this;
	}

	thisType operator+(const thisType& other) const {
		return thisType(RC::ForceRounding(impl + other.impl));
	}

	thisType operator-(const thisType& other) const {
		return thisType(RC::ForceRounding(impl - other.impl));
	}

	thisType operator*(const thisType& other) const {
		return thisType(RC::ForceRounding(impl * other.impl));
	}
#endif
	
	thisType& plusAssign(const thisType& other, const StatusAndControlFlags& flags);
	thisType& minusAssign(const thisType& other, const StatusAndControlFlags& flags);
	thisType& multAssign(const thisType& other, const StatusAndControlFlags& flags);

	thisType& opposite() { impl = -impl; return *this; }
	thisType& saturate();
	thisType& signedSaturate();

	thisType& setZero() { impl = FT::Zero(); return *this; }
	thisType& setOne() { impl = FT::One(); return *this; }
	thisType& setMinusOne() { impl = FT::MinusOne(); return *this; }
	thisType& setPositive() { impl = fabs(impl); return *this; }

	bool isNegative() const { return impl < FT::Zero(); }
	bool isNaN() const { return !(impl == impl); }
	
	void setInteger(uint32_t value, StatusAndControlFlags& flags);
	void setInteger(int32_t value, StatusAndControlFlags& flags);

#if 0
	void retrieveInteger(uint32_t& result, StatusAndControlFlags& flags) const;
	void retrieveInteger(int32_t& result, StatusAndControlFlags& flags) const;
	void retrieveInteger(uint16_t& result, StatusAndControlFlags& flags) const;
	void retrieveInteger(int16_t& result, StatusAndControlFlags& flags) const;
#endif
	template<class IntConv>
	void retrieveInteger(IntConv& result, StatusAndControlFlags& flags) const;

	thisType& roundToInt(StatusAndControlFlags & flags);

	enum ComparisonResult { CRNaN=0, CRLess=1, CREqual=2, CRGreater=3 };
	ComparisonResult compare(const thisType& bdSource) const;
	//ComparisonResult compareValue(const thisType& bdSource) const;	// Unordered comparison

	bool operator<(const thisType& other) const {
		return compare(other) == CRLess;
	}

	bool operator>(const thisType& other) const {
		return compare(other) == CRGreater;
	}
	
	bool operator<=(const thisType& other) const {
		ComparisonResult cr = compare(other);
		return cr == CRLess || cr == CREqual;
	}

	bool operator>=(const thisType& other) const {
		ComparisonResult cr = compare(other);
		return cr == CRGreater || cr == CREqual;
	}

};

typedef HostFloat<float, TeslaBinary32Traits> TeslaBinary32;
typedef HostFloat<double, TeslaBinary64Traits> TeslaBinary64;

} // end of namespace hostfloat
} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
