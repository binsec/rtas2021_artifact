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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_REGISTER_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_REGISTER_HH

#include <bitset>
#include <iosfwd>

#include <unisim/component/cxx/processor/tesla/forward.hh>
#include <unisim/component/cxx/processor/tesla/enums.hh>
#include <unisim/component/cxx/processor/tesla/strided_register.hh>
#include <unisim/service/interfaces/register.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template <class CONFIG>
struct BaseVectorAddress;

// Inherit from valarray<CONFIG::reg_t>??
template <class CONFIG>
struct BaseVectorRegister
{
	static unsigned int const WARP_SIZE = CONFIG::WARP_SIZE;
	typedef typename CONFIG::reg_t reg_t;
	BaseVectorRegister();
	BaseVectorRegister(uint32_t val);
	BaseVectorRegister(BaseVectorRegister<CONFIG> const & other);
	BaseVectorRegister(BaseVectorAddress<CONFIG> const & addr);
	void Write(BaseVectorRegister<CONFIG> const & vec, std::bitset<CONFIG::WARP_SIZE> mask);
	void Write16(BaseVectorRegister<CONFIG> const & vec, std::bitset<CONFIG::WARP_SIZE> mask, int hi);
	
	void WriteLane(uint32_t val, unsigned int lane);
	uint32_t ReadLane(unsigned int lane) const;
	void WriteFloat(float val, unsigned int lane);
	float ReadFloat(unsigned int lane) const;
	void WriteSimfloat(typename CONFIG::float_t val, unsigned int lane);
	typename CONFIG::float_t ReadSimfloat(unsigned int lane) const;
	BaseVectorRegister<CONFIG> Split(int hilo) const;
	void DumpFloat(std::ostream & os);
	
	uint32_t operator[] (unsigned int lane) const;
	uint32_t & operator[] (unsigned int lane);
	
	uint16_t Read16(unsigned int lane, bool hi) const;

	bool CheckScalar() const;
	bool CheckStrided() const;
	bool CheckScalar16(bool hi) const;
	bool CheckStrided16(bool hi) const;

	reg_t v[WARP_SIZE];
	bool scalar;
	bool strided;
	bool scalar16[2];
	bool strided16[2];
	
	bool IsScalar() const { return false; }
	bool IsStrided() const { return false; }
	bool IsScalar16(bool hi) const { return false; }
	bool IsStrided16(bool hi) const { return false; }

	void SetScalar(bool s = true) {	}
	void SetStrided(bool s = true) { }
	void SetScalar16(bool hi, bool s = true) { }
	void SetStrided16(bool hi, bool s = true) { }
};

template <class CONFIG>
std::ostream & operator << (std::ostream & os, BaseVectorRegister<CONFIG> const & r);

template <class CONFIG>
struct BaseVectorAddress
{
	static unsigned int const WARP_SIZE = CONFIG::WARP_SIZE;
	typedef typename CONFIG::address_t address_t;
	
	BaseVectorAddress();
	BaseVectorAddress(address_t addr);
	BaseVectorAddress(BaseVectorAddress<CONFIG> const & other);
	BaseVectorAddress(BaseVectorRegister<CONFIG> const & vr);

	void Write(BaseVectorAddress<CONFIG> const & vec, std::bitset<CONFIG::WARP_SIZE> mask);
	
	void Reset();
	address_t operator[] (unsigned int lane) const;
	address_t & operator[] (unsigned int lane);
	
	BaseVectorAddress<CONFIG> & operator+=(BaseVectorAddress<CONFIG> const & other);
	
	void Increment(DataType dt, size_t imm, std::bitset<CONFIG::WARP_SIZE> mask);
	
	address_t v[WARP_SIZE];
	bool IsScalar() const { return false; }
	bool IsStrided() const { return false; }
	void SetScalar(bool s = true) { }
	void SetStrided(bool s = true) { }
};

template<class CONFIG>
BaseVectorAddress<CONFIG> operator+(BaseVectorAddress<CONFIG> const & a, BaseVectorAddress<CONFIG> const & b);

template<class CONFIG>
BaseVectorAddress<CONFIG> operator*(unsigned int factor, BaseVectorAddress<CONFIG> const & addr);

template <class CONFIG>
std::ostream & operator << (std::ostream & os, BaseVectorAddress<CONFIG> const & r);


template <bool SEL_A, class A, class B>
struct SelectType
{
};

template <class A, class B>
struct SelectType<false, A, B>
{
	typedef B t;
};

template <class A, class B>
struct SelectType<true, A, B>
{
	typedef A t;
};

template <class CONFIG>
struct VectorRegister :
	SelectType<CONFIG::STAT_SCALAR_REG | CONFIG::STAT_STRIDED_REG,
	           StridedTagVectorRegister<CONFIG, BaseVectorRegister<CONFIG> >,
	           BaseVectorRegister<CONFIG> >::t
{
	typedef typename SelectType<CONFIG::STAT_SCALAR_REG | CONFIG::STAT_STRIDED_REG,
	           StridedTagVectorRegister<CONFIG, BaseVectorRegister<CONFIG> >,
	           BaseVectorRegister<CONFIG> >::t Base;
	VectorRegister() : Base() {}
	VectorRegister(Base const & other) : Base(other) {}
	VectorRegister(uint32_t val) : Base(val) {}
	VectorRegister(VectorRegister<CONFIG> const & other) : Base(other) {}
	VectorRegister(VectorAddress<CONFIG> const & addr) : Base(addr) {}
};

template <class CONFIG>
struct VectorAddress :
	SelectType<CONFIG::STAT_SCALAR_REG | CONFIG::STAT_STRIDED_REG,
	           StridedTagVectorAddress<CONFIG, BaseVectorAddress<CONFIG> >,
	           BaseVectorAddress<CONFIG> >::t
{
	typedef typename SelectType<CONFIG::STAT_SCALAR_REG | CONFIG::STAT_STRIDED_REG,
	           StridedTagVectorAddress<CONFIG, BaseVectorAddress<CONFIG> >,
	           BaseVectorAddress<CONFIG> >::t Base;
	VectorAddress() : Base() {}
	VectorAddress(Base const & other) : Base(other) {}
	VectorAddress(typename CONFIG::address_t addr) : Base(addr) {}
	VectorAddress(VectorAddress<CONFIG> const & other) : Base(other) {}
	VectorAddress(VectorRegister<CONFIG> const & vr) : Base(vr) {}
};

template<class CONFIG>
VectorAddress<CONFIG> operator+(VectorAddress<CONFIG> const & a, VectorAddress<CONFIG> const & b)
{
	return VectorAddress<CONFIG>(operator+((typename VectorAddress<CONFIG>::Base)(a), (typename VectorAddress<CONFIG>::Base)(b)));
}

template<class CONFIG>
VectorAddress<CONFIG> operator*(unsigned int factor, VectorAddress<CONFIG> const & addr)
{
	return VectorAddress<CONFIG>(operator*(factor, (typename VectorAddress<CONFIG>::Base)(addr)));
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
