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

#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_SAMPLER_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_SAMPLER_TCC

#include <unisim/component/cxx/processor/tesla/sampler.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {


template<class CONFIG>
Sampler<CONFIG>::Sampler()
{
}

template<class CONFIG>
Sampler<CONFIG>::Sampler(SamplerBase<typename CONFIG::address_t> const & other)
{
	this->address = other.GetBaseAddress();
	this->num_packed_components = other.GetNumPackedComponents();
	for(int i = 0; i != 3; ++i) {
		this->address_mode[i] = other.GetAddressMode(i);
	}
	this->filter_mode = other.GetFilterMode();
	this->flags = other.GetFlags();
	this->format = other.GetFormat();
}

inline int ArrayFormatSize(ArrayFormat af)
{
	switch(af)
	{
		case AF_U8:
		case AF_S8:
			return 1;
		case AF_U16:
		case AF_S16:
		case AF_F16:
			return 2;
		case AF_U32:
		case AF_S32:
		case AF_F32:
			return 4;
		default:
			assert(false);
	};
}

template<class CONFIG>
void Sampler<CONFIG>::Sample1DS32(VectorRegister<CONFIG> dest[],
	VectorRegister<CONFIG> const src[],
	uint32_t destBitfield)
{
	assert(!(this->flags & TF_NORMALIZED_COORDINATES));
	assert(this->filter_mode == FM_POINT);

	// Coordinates as int32_t
	VectorAddress<CONFIG> addr(src[0]);
	int elSize = ArrayFormatSize(this->format) * this->num_packed_components;
	
	// TODO: Wrap / Clamp / Mirror
	addr = VectorAddress<CONFIG>(this->address) + elSize * addr;
	
	Fetch(dest, addr, destBitfield);
}

template<class CONFIG>
void Sampler<CONFIG>::Reset(CPU<CONFIG> * cpu)
{
	this->address = 0;
	this->num_packed_components = 0;
	for(int i = 0; i != 3; ++i) {
		this->address_mode[i] = AM_WRAP;
	}
	this->format = AF_U8;
	this->filter_mode = FM_POINT;
	this->flags = 0;
	this->cpu = cpu;
}


template<class CONFIG>
void Sampler<CONFIG>::Fetch(VectorRegister<CONFIG> dest[],
	VectorAddress<CONFIG> const & addr,
	uint32_t destBitfield)
{
	
	int elSize = ArrayFormatSize(this->format);
	
	
	for(unsigned int l = 0; l != CONFIG::WARP_SIZE; ++l)
	{

		int j = 0;
		for(int i = 0; i != 4; ++i)
		{
			if(destBitfield & (1 << i)) {
				uint32_t buffer;
				if(!cpu->ReadMemory(addr[l] + i * elSize, &buffer, elSize))
					assert(false);
				dest[j][l] = Unpack(buffer);
				++j;
			}
		}
	}
}

inline uint32_t FloatAsUint32(float x)
{
	union { float f; uint32_t i; } conv;
	conv.f = x;
	return conv.i;
}

template<class CONFIG>
uint32_t Sampler<CONFIG>::Unpack(uint32_t rawval)
{
	// Assumes little-endian encoding
	if(this->flags & TF_READ_AS_INTEGER) {
		// Int -> Int, Z-extend or S-extend
		uint32_t intval;
		switch(this->format)
		{
		case AF_U8:
			intval = uint8_t(rawval);
			break;
		case AF_S8:
			intval = int8_t(rawval);
			break;
		case AF_U16:
			intval = uint16_t(rawval);
			break;
		case AF_S16:
			intval = int16_t(rawval);
			break;
		case AF_U32:
			intval = uint32_t(rawval);
			break;
		case AF_S32:
			intval = int32_t(rawval);
			break;
		case AF_F16:
		case AF_F32:
		default:
			assert(false);
		}
		return intval;
	}
	else {
		// Int -> Float
		// GLspec 2.19
		float floatval;
		switch(this->format)
		{
		case AF_U8:
			floatval = uint8_t(rawval) / 255.f;
			break;
		case AF_S8:
			floatval = (2.f * int8_t(rawval) + 1.f) / 255.f;
			break;
		case AF_U16:
			floatval = uint16_t(rawval) / 65535.f;
			break;
		case AF_S16:
			floatval = (2.f * int16_t(rawval) + 1.f) / 65535.f;
			break;
		case AF_U32:
			floatval = ldexp(float(uint32_t(rawval)), -32);	// 2^32-1 is rounded to 2^32 in FP32
			break;
		case AF_S32:
			floatval = ldexp(float(int32_t(rawval)), -31);
			break;
		case AF_F16:
			assert(false);	// TODO
		case AF_F32:
			return rawval;
		default:
			assert(false);
		}
		return FloatAsUint32(floatval);
	}
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
