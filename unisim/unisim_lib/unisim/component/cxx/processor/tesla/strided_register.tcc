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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_STRIDED_REGISTER_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_STRIDED_REGISTER_TCC

#include <unisim/component/cxx/processor/tesla/strided_register.hh>
#include <cassert>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template <class CONFIG, class Base>
StridedTagVectorRegister<CONFIG, Base>::StridedTagVectorRegister()
{
	SetScalar(false);
	SetStrided(false);
	SetScalar16(0, false);
	SetScalar16(1, false);
	SetStrided16(0, false);
	SetStrided16(1, false);
}

template <class CONFIG, class Base>
StridedTagVectorRegister<CONFIG, Base>::StridedTagVectorRegister(uint32_t val) :
	Base(val)
{
	SetScalar(true);
	//SetStrided(true);
	//SetScalar16(0, true);
	//SetScalar16(1, true);
	//SetStrided16(0, true);
	//SetStrided16(1, true);
}

template <class CONFIG, class Base>
StridedTagVectorRegister<CONFIG, Base>::StridedTagVectorRegister(ThisType const & other) :
	Base(other)
{
	SetScalar(other.IsScalar());
	SetStrided(other.IsStrided());
	SetScalar16(false, other.IsScalar16(false));
	SetScalar16(true, other.IsScalar16(true));
	SetStrided16(false, other.IsStrided16(false));
	SetStrided16(true, other.IsStrided16(true));
}

template <class CONFIG, class Base>
StridedTagVectorRegister<CONFIG, Base>::StridedTagVectorRegister(Base const & other) :
	Base(other)
{
	SetScalar(false);
	SetStrided(false);
	SetScalar16(0, false);
	SetScalar16(1, false);
	SetStrided16(0, false);
	SetStrided16(1, false);
}


template <class CONFIG, class Base>
template<class Base2>
StridedTagVectorRegister<CONFIG, Base>::StridedTagVectorRegister(StridedTagVectorAddress<CONFIG, Base2> const & addr) :
	Base(addr)
{
	SetScalar(addr.IsScalar());
	SetStrided(addr.IsStrided());
	SetScalar16(0, addr.IsScalar());
	SetScalar16(1, addr.IsScalar());
	SetStrided16(0, false);
	SetStrided16(1, false);
}

template <class CONFIG, class Base>
void StridedTagVectorRegister<CONFIG, Base>::Write(ThisType const & vec, bitset<CONFIG::WARP_SIZE> mask)
{
	Base::Write(vec, mask);
	if(mask == ~bitset<CONFIG::WARP_SIZE>(0)) {
		SetScalar(vec.IsScalar());
		SetStrided(vec.IsStrided());
		SetScalar16(false, vec.IsScalar16(false));
		SetStrided16(false, vec.IsStrided16(false));
		SetScalar16(true, vec.IsScalar16(true));
		SetStrided16(true, vec.IsStrided16(true));
	}
	else if(mask != 0) {
		SetScalar(false);
		SetStrided(false);
		SetScalar16(false, false);
		SetStrided16(false, false);
		SetScalar16(true, false);
		SetStrided16(true, false);
		
	}
}

template <class CONFIG, class Base>
void StridedTagVectorRegister<CONFIG, Base>::Write16(ThisType const & vec,
	bitset<CONFIG::WARP_SIZE> mask, int hi)
{
	Base::Write16(vec, mask, hi);
	SetScalar(false);
	SetStrided(false);
	if(mask == ~bitset<CONFIG::WARP_SIZE>(0)) {
		SetScalar16(hi, vec.IsScalar16(false));
		SetStrided16(hi, vec.IsStrided16(false));
	}
	else if(mask != 0) {
		SetScalar16(hi, false);
		SetStrided16(hi, false);
	}
}

template <class CONFIG, class Base>
StridedTagVectorRegister<CONFIG, Base> StridedTagVectorRegister<CONFIG, Base>::Split(int hilo) const
{
	StridedTagVectorRegister<CONFIG, Base> vr(Base::Split(hilo));
	vr.SetScalar(scalar16[hilo]);
	vr.SetScalar16(0, scalar16[hilo]);
	vr.SetStrided(strided16[hilo]);
	vr.SetStrided16(0, strided16[hilo]);
	return vr;
}


template <class CONFIG, class Base>
StridedTagVectorAddress<CONFIG, Base>::StridedTagVectorAddress()
{
	SetScalar(false);
	SetStrided(false);
}

template <class CONFIG, class Base>
StridedTagVectorAddress<CONFIG, Base>::StridedTagVectorAddress(StridedTagVectorAddress<CONFIG, Base>::address_t addr) :
	Base(addr)
{
	SetScalar(true);
	SetStrided(true);
}

template <class CONFIG, class Base>
StridedTagVectorAddress<CONFIG, Base>::StridedTagVectorAddress(Base const & other) :
	Base(other)
{
	SetScalar(false);
	SetStrided(false);
}

template <class CONFIG, class Base>
StridedTagVectorAddress<CONFIG, Base>::StridedTagVectorAddress(StridedTagVectorAddress<CONFIG, Base> const & other) :
	Base(other)
{
	SetScalar(other.IsScalar());
	SetStrided(other.IsStrided());
}

template <class CONFIG, class Base>
template <class Base2>
StridedTagVectorAddress<CONFIG, Base>::StridedTagVectorAddress(StridedTagVectorRegister<CONFIG, Base2> const & vr) :
	Base(vr)
{
	SetScalar(vr.IsScalar());
	SetStrided(vr.IsStrided());
}

template <class CONFIG, class Base>
void StridedTagVectorAddress<CONFIG, Base>::Write(ThisType const & vec, std::bitset<CONFIG::WARP_SIZE> mask)
{
	Base::Write(vec, mask);
	if(mask == ~bitset<CONFIG::WARP_SIZE>(0)) {
		SetScalar(vec.IsScalar());
		SetStrided(vec.IsStrided());
	}
	else if(mask != 0) {
		SetScalar(false);
		SetStrided(false);
	}
}

template <class CONFIG, class Base>
void StridedTagVectorAddress<CONFIG, Base>::Reset()
{
	Base::Reset();
	SetScalar(false);
	SetStrided(false);
}

template <class CONFIG, class Base>
StridedTagVectorAddress<CONFIG, Base> & StridedTagVectorAddress<CONFIG, Base>::operator+=(ThisType const & other)
{
	Base::operator+=(other);
	SetScalar(IsScalar() && other.IsScalar());
	SetStrided(IsStrided() && other.IsStrided());
	return *this;
}

template<class CONFIG, class Base>
StridedTagVectorAddress<CONFIG, Base> operator+(StridedTagVectorAddress<CONFIG, Base> const & a, StridedTagVectorAddress<CONFIG, Base> const & b)
{
	StridedTagVectorAddress<CONFIG, Base> dest(operator+(Base(a), Base(b)));
	dest.SetScalar(a.IsScalar() && b.IsScalar());
	dest.SetStrided(a.IsStrided() && b.IsStrided());
	return dest;
}

template<class CONFIG, class Base>
StridedTagVectorAddress<CONFIG, Base> operator*(unsigned int factor, StridedTagVectorAddress<CONFIG, Base> const & addr)
{
	StridedTagVectorAddress<CONFIG, Base> dest(operator*(factor, Base(addr)));
	dest.SetScalar(addr.IsScalar());
	dest.SetStrided(addr.IsStrided());
	return dest;
}


} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
