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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_REGISTER_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_REGISTER_TCC

#include <unisim/component/cxx/processor/tesla/register.hh>
#include <cassert>
#include <ostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {



template <class CONFIG>
BaseVectorRegister<CONFIG>::BaseVectorRegister()
{
}

template <class CONFIG>
BaseVectorRegister<CONFIG>::BaseVectorRegister(uint32_t val)
{
	std::fill(v, v + WARP_SIZE, val);
}

template <class CONFIG>
BaseVectorRegister<CONFIG>::BaseVectorRegister(BaseVectorRegister<CONFIG> const & other)
{
	std::copy(other.v, other.v + WARP_SIZE, v);
}

template <class CONFIG>
BaseVectorRegister<CONFIG>::BaseVectorRegister(BaseVectorAddress<CONFIG> const & addr)
{
	for(unsigned int i = 0; i != WARP_SIZE; ++i)
	{
		WriteLane(addr[i], i);
	}
}

template <class CONFIG>
void BaseVectorRegister<CONFIG>::Write(BaseVectorRegister<CONFIG> const & vec, bitset<CONFIG::WARP_SIZE> mask)
{
	for(unsigned int i = 0; i != WARP_SIZE; ++i)
	{
		if(mask[i]) {
			WriteLane(vec[i], i);
		}
	}
}

template <class CONFIG>
void BaseVectorRegister<CONFIG>::Write16(BaseVectorRegister<CONFIG> const & vec,
	bitset<CONFIG::WARP_SIZE> mask, int hi)
{
	for(unsigned int i = 0; i != WARP_SIZE; ++i)
	{
		if(mask[i]) {
			if(hi) {
				v[i] = (v[i] & 0x0000ffff) | (vec[i] << 16);
			}
			else {
				v[i] = (v[i] & 0xffff0000) | (vec[i] & 0x0000ffff);
			}
		}
	}
}

template <class CONFIG>
void BaseVectorRegister<CONFIG>::WriteLane(uint32_t val, unsigned int lane)
{
	assert(lane >= 0 && lane < WARP_SIZE);
	v[lane] = val;
}

template <class CONFIG>
uint32_t BaseVectorRegister<CONFIG>::ReadLane(unsigned int lane) const
{
	assert(lane >= 0 && lane < WARP_SIZE);
	return v[lane];
}

template <class CONFIG>
void BaseVectorRegister<CONFIG>::WriteFloat(float val, unsigned int lane)
{
	// TODO: endianness
	union { float f; uint32_t u; } caster;
	caster.f = val;
	WriteLane(caster.u, lane);
}

template <class CONFIG>
float BaseVectorRegister<CONFIG>::ReadFloat(unsigned int lane) const
{
	// TODO: endianness
	union { float f; uint32_t u; } caster;
	caster.u = ReadLane(lane);
	return caster.f;
}

template <class CONFIG>
void BaseVectorRegister<CONFIG>::WriteSimfloat(typename CONFIG::float_t val, unsigned int lane)
{
	WriteLane(val.queryValue(), lane);
}

template <class CONFIG>
typename CONFIG::float_t BaseVectorRegister<CONFIG>::ReadSimfloat(unsigned int lane) const
{
	return typename CONFIG::float_t(ReadLane(lane));
}

template <class CONFIG>
BaseVectorRegister<CONFIG> BaseVectorRegister<CONFIG>::Split(int hilo) const
{
	BaseVectorRegister<CONFIG> vr;
	for(unsigned int i = 0; i != WARP_SIZE; ++i)
	{
		if(hilo) {
			// high part
			vr[i] = (v[i] >> 16);
		}
		else {
			// low part
			vr[i] = (v[i] & 0x0000ffff);
		}
	}
	return vr;
}

template <class CONFIG>
uint32_t BaseVectorRegister<CONFIG>::operator[] (unsigned int lane) const
{
	assert(lane >= 0 && lane < WARP_SIZE);
	return v[lane];
}

template <class CONFIG>
uint32_t & BaseVectorRegister<CONFIG>::operator[] (unsigned int lane)
{
	assert(lane >= 0 && lane < WARP_SIZE);
	return v[lane];
}

template <class CONFIG>
uint16_t BaseVectorRegister<CONFIG>::Read16(unsigned int lane, bool hi) const
{
	if(hi) {
		return v[lane] >> 16;
	}
	else {
		return v[lane] & 0xffff;
	}
}

template <class CONFIG>
void BaseVectorRegister<CONFIG>::DumpFloat(std::ostream & os)
{
	os << "(";
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE-1; ++i)
	{
		os << ReadFloat(i) << ", ";
	}
	os << ReadFloat(CONFIG::WARP_SIZE-1);
	os << ")";
}

template <class CONFIG>
bool BaseVectorRegister<CONFIG>::CheckScalar() const
{
	assert(CONFIG::WARP_SIZE >= 1);
	uint32_t ref = v[0];
	for(unsigned int i = 1; i != CONFIG::WARP_SIZE; ++i) {
		if(ref != v[i]) {
			return false;
		}
	}
	return true;
}

template <class CONFIG>
bool BaseVectorRegister<CONFIG>::CheckScalar16(bool hi) const
{
	assert(CONFIG::WARP_SIZE >= 1);
	uint16_t ref = Read16(0, hi);
	for(unsigned int i = 1; i != CONFIG::WARP_SIZE; ++i) {
		if(ref != Read16(i, hi)) {
			return false;
		}
	}
	return true;
}

template <class CONFIG>
bool BaseVectorRegister<CONFIG>::CheckStrided() const
{
	assert(CONFIG::WARP_SIZE >= 2);
	uint32_t base = v[0];
	int32_t stride = v[1] - base;
	for(unsigned int i = 2; i != CONFIG::WARP_SIZE; ++i)
	{
		if(v[i] != base + i * stride) {
			return false;
		}
	}
	return true;
}

template <class CONFIG>
bool BaseVectorRegister<CONFIG>::CheckStrided16(bool hi) const
{
	assert(CONFIG::WARP_SIZE >= 2);
	uint16_t base = Read16(0, hi);
	int16_t stride = Read16(1, hi) - base;
	for(unsigned int i = 2; i != CONFIG::WARP_SIZE; ++i)
	{
		if(Read16(i, hi) != base + i * stride) {
			return false;
		}
	}
	return true;
}

template <class CONFIG>
std::ostream & operator << (std::ostream & os, BaseVectorRegister<CONFIG> const & r)
{
	os << "(";
	os << std::hex;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE-1; ++i)
	{
		os << r[i] << ", ";
	}
	os << r[CONFIG::WARP_SIZE-1];
	os << std::dec;
	os << ")";
	return os;
}

template <class CONFIG>
BaseVectorAddress<CONFIG>::BaseVectorAddress()
{
}

template <class CONFIG>
BaseVectorAddress<CONFIG>::BaseVectorAddress(BaseVectorAddress<CONFIG>::address_t addr)
{
	std::fill(v, v + WARP_SIZE, addr);
}

template <class CONFIG>
BaseVectorAddress<CONFIG>::BaseVectorAddress(BaseVectorAddress<CONFIG> const & other)
{
	std::copy(other.v, other.v + WARP_SIZE, v);
}

template <class CONFIG>
BaseVectorAddress<CONFIG>::BaseVectorAddress(BaseVectorRegister<CONFIG> const & vr)
{
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i) {
		v[i] = vr[i];
	}
}

template <class CONFIG>
void BaseVectorAddress<CONFIG>::Write(BaseVectorAddress<CONFIG> const & vec, std::bitset<CONFIG::WARP_SIZE> mask)
{
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i) {
		if(mask[i])
			v[i] = vec[i];
	}
}

template <class CONFIG>
void BaseVectorAddress<CONFIG>::Reset()
{
	std::fill(v, v + WARP_SIZE, 0);
}

template <class CONFIG>
void BaseVectorAddress<CONFIG>::Increment(DataType dt, size_t imm, std::bitset<CONFIG::WARP_SIZE> mask)
{
	size_t inc = DataTypeSize(dt) * imm;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i) {
		if(mask[i])
			v[i] += inc;
	}
}

template <class CONFIG>
typename BaseVectorAddress<CONFIG>::address_t BaseVectorAddress<CONFIG>::operator[] (unsigned int lane) const
{
	assert(lane < WARP_SIZE);
	return v[lane];
}

template <class CONFIG>
typename BaseVectorAddress<CONFIG>::address_t & BaseVectorAddress<CONFIG>::operator[] (unsigned int lane)
{
	assert(lane < WARP_SIZE);
	return v[lane];
}

template <class CONFIG>
BaseVectorAddress<CONFIG> & BaseVectorAddress<CONFIG>::operator+=(BaseVectorAddress<CONFIG> const & other)
{
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i) {
		v[i] += other[i];
	}
	return *this;
}

template<class CONFIG>
BaseVectorAddress<CONFIG> operator+(BaseVectorAddress<CONFIG> const & a, BaseVectorAddress<CONFIG> const & b)
{
	BaseVectorAddress<CONFIG> dest;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i) {
		dest[i] = a[i] + b[i];
	}
	return dest;
}

template<class CONFIG>
BaseVectorAddress<CONFIG> operator*(unsigned int factor, BaseVectorAddress<CONFIG> const & addr)
{
	BaseVectorAddress<CONFIG> dest;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i) {
		dest[i] = factor * addr[i];
	}
	return dest;
}

template <class CONFIG>
std::ostream & operator << (std::ostream & os, BaseVectorAddress<CONFIG> const & r)
{
	os << "(";
	os << std::hex;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE-1; ++i)
	{
		os << r[i] << ", ";
	}
	os << r[CONFIG::WARP_SIZE-1];
	os << std::dec;
	os << ")";
	return os;
}


} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
