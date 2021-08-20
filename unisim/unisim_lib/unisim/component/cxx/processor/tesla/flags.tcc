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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_FLAGS_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_FLAGS_TCC__

#include <unisim/component/cxx/processor/tesla/flags.hh>

#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cmath>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

using namespace std;

template <class CONFIG>
bitset<CONFIG::WARP_SIZE> IsPredSet(uint32_t cond, VectorFlags<CONFIG> flags)
{
	assert(cond < 32);
	if(cond == CD_TR) {
		return bitset<CONFIG::WARP_SIZE>().set();	// fast path
	}
	bitset<CONFIG::WARP_SIZE> bs;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		bs[i] = IsPredSet(cond, flags[i]);
	}
	return bs;
}

template<class CONFIG>
VectorFlags<CONFIG> ComputePredI32(VectorRegister<CONFIG> const & output, VectorFlags<CONFIG> flags)
{
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		flags.SetZero((output[i] == 0), i);
		flags.SetSign((int32_t(output[i]) < 0), i);
	}
	return flags;
}

template<class CONFIG>
VectorFlags<CONFIG> ComputePredI16(VectorRegister<CONFIG> const & output, VectorFlags<CONFIG> flags)
{
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		flags.SetZero(((output[i] & 0xffff) == 0), i);
		flags.SetSign((int16_t(output[i]) < 0), i);
	}
	return flags;
}


template<class CONFIG>
VectorFlags<CONFIG> ComputePredSetI(VectorRegister<CONFIG> & output,
	VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	SetCond sc,
	bool is_signed,
	bool b32)
{
	// TODO: check it only outputs boolean values
	// input:
	// SZ
	// 00  01  10
	// +1  0   -1
/*	uint8 truth_table[8][3] = 
		{ 0, 0, 0 },	// FL
		{ 0, 0, 1 },	// LT
		{ 0, 1, 0 },	// EQ
		{ 0, 1, 1 },	// LE
		{ 1, 0, 0 },	// GT
		{ 1, 0, 1 },	// NE
		{ },	// GE
		{ },	// TR*/
	
	bitset<3> lut = bitset<3>(sc);

	VectorFlags<CONFIG> flags;
	flags.Reset();
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		uint8_t cond = 0;
		if(b32) {
			if(a[i] == b[i]) cond |= 1;
			if((is_signed && (int32_t(a[i]) < int32_t(b[i])))
				|| (!is_signed && (a[i] < b[i]))) {
				cond |= 2;
			}
		}
		else {
			uint16_t sa = a[i];// & 0x0000ffff;
			uint16_t sb = b[i];// & 0x0000ffff;
			if(sa == sb) cond |= 1;
			if((is_signed && (int16_t(sa) < int16_t(sb)))
				|| (!is_signed && (sa < sb))) {
				cond |= 2;
			}
		}
		
		//cerr << "  cond[" << i << "] = " << uint32_t(cond) << endl;
		assert(cond <= 2);
		bool r = lut[2 - cond];	// addressed from the left
		output[i] = -r;	// G80Spec -> extend
		
		// TODO: CHECK is this correct (=ComputePredI32)?
		flags.SetZero(output[i] == 0, i);
		flags.SetSign(output[i] < 0, i);
	}
	return flags;
}


template <class CONFIG>
void VectorFlags<CONFIG>::Write(VectorFlags<CONFIG> const & f, bitset<CONFIG::WARP_SIZE> mask)
{
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		if(mask[i])
		{
			v[i] = f.v[i];
		}
	}
}

template <class CONFIG>
VectorFlags<CONFIG> VectorFlags<CONFIG>::Reset()
{
	std::fill(v, v + CONFIG::WARP_SIZE, 0);
	return *this;
}

template <class CONFIG>
void VectorFlags<CONFIG>::SetZero(int z, int lane)
{
	v[lane][0] = z;
}

template <class CONFIG>
int VectorFlags<CONFIG>::GetZero(int lane) const
{
	return int(v[lane][0]);
}
template <class CONFIG>
void VectorFlags<CONFIG>::SetSign(int s, int lane)
{
	v[lane][1] = s;
}

template <class CONFIG>
int VectorFlags<CONFIG>::GetSign(int lane) const
{
	return int(v[lane][1]);
}


template <class CONFIG>
void VectorFlags<CONFIG>::SetCarry(int c, int lane)
{
	v[lane][2] = c;
}

template <class CONFIG>
int VectorFlags<CONFIG>::GetCarry(int lane) const
{
	return int(v[lane][2]);
}

template <class CONFIG>
void VectorFlags<CONFIG>::SetOvf(int o, int lane)
{
	v[lane][3] = o;
}
template <class CONFIG>
int VectorFlags<CONFIG>::GetOvf(int lane) const
{
	return int(v[lane][3]);
}

template <class CONFIG>
bitset<4> VectorFlags<CONFIG>::operator[](unsigned int i) const
{
	return v[i];
}

template <class CONFIG>
bitset<4> & VectorFlags<CONFIG>::operator[](unsigned int i)
{
	return v[i];
}

template <class CONFIG>
std::ostream & operator << (std::ostream & os, VectorFlags<CONFIG> const & r)
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

