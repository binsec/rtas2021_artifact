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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_STRIDED_REGISTER_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_STRIDED_REGISTER_HH

#include <unisim/component/cxx/processor/tesla/register.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template <class CONFIG>
struct VectorAddress;

template <class CONFIG, class Base>
struct StridedTagVectorAddress;

template <class CONFIG, class Base>
struct StridedTagVectorRegister : Base
{
	static unsigned int const WARP_SIZE = CONFIG::WARP_SIZE;
	typedef StridedTagVectorRegister<CONFIG, Base> ThisType;

	StridedTagVectorRegister();
	StridedTagVectorRegister(uint32_t val);
	StridedTagVectorRegister(ThisType const & other);
	StridedTagVectorRegister(Base const & other);
	
	template <class Base2>
	StridedTagVectorRegister(StridedTagVectorAddress<CONFIG, Base2> const & addr);
	void Write(ThisType const & vec, std::bitset<CONFIG::WARP_SIZE> mask);
	void Write16(ThisType const & vec, std::bitset<CONFIG::WARP_SIZE> mask, int hi);
	
	StridedTagVectorRegister<CONFIG, Base> Split(int hilo) const;
	
	bool scalar;
	bool strided;
	bool scalar16[2];
	bool strided16[2];
	
	bool IsScalar() const { return scalar; }
	bool IsStrided() const { return strided; }
	bool IsScalar16(bool hi) const { return scalar16[hi]; }
	bool IsStrided16(bool hi) const { return strided16[hi]; }

	void SetScalar(bool s = true) {
		scalar = s;
		if(s) {
			scalar16[0] = scalar16[1] = true;
			strided = strided16[0] = strided16[1] = true;
		}
	}
	void SetStrided(bool s = true) { strided = s; }
	void SetScalar16(bool hi, bool s = true) { scalar16[hi] = s; }
	void SetStrided16(bool hi, bool s = true) { strided16[hi] = s; }
};

template <class CONFIG, class Base>
struct StridedTagVectorAddress : Base
{
	static unsigned int const WARP_SIZE = CONFIG::WARP_SIZE;
	typedef typename CONFIG::address_t address_t;
	typedef StridedTagVectorAddress<CONFIG, Base> ThisType;
	
	StridedTagVectorAddress();
	StridedTagVectorAddress(address_t addr);
	StridedTagVectorAddress(Base const & other);
	StridedTagVectorAddress(ThisType const & other);

	template <class Base2>
	StridedTagVectorAddress(StridedTagVectorRegister<CONFIG, Base2> const & vr);

	void Write(ThisType const & vec, std::bitset<CONFIG::WARP_SIZE> mask);
	
	void Reset();
	
	ThisType & operator+=(ThisType const & other);
	
	bool scalar;
	bool strided;

	bool IsScalar() const { return scalar; }
	bool IsStrided() const { return strided; }
	void SetScalar(bool s = true) { scalar = s; }
	void SetStrided(bool s = true) { strided = s; }
};

template<class CONFIG, class Base>
StridedTagVectorAddress<CONFIG, Base> operator+(StridedTagVectorAddress<CONFIG, Base> const & a, StridedTagVectorAddress<CONFIG, Base> const & b);

template<class CONFIG, class Base>
StridedTagVectorAddress<CONFIG, Base> operator*(unsigned int factor, StridedTagVectorAddress<CONFIG, Base> const & addr);

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
