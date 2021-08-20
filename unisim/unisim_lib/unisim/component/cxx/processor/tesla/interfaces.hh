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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_INTERFACES_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_INTERFACES_HH

#include <cassert>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

// VectorRegister<CONFIG> is not part of the public interface!

// Scalar register
struct GPRID
{
	uint16_t warpid;
	uint16_t regid;
	uint16_t laneid;
	
	GPRID(uint16_t warpid, uint16_t regid, uint16_t laneid) :
		warpid(warpid), regid(regid), laneid(laneid) {}
};

struct SMAddress
{
	uint16_t blockid;
	uint16_t address;
	
	SMAddress(uint16_t blockid, uint16_t address) :
		blockid(blockid), address(address) {}
};

struct ConfigurationRegisterID
{
	enum id_t {
		ThreadsPerBlock,
		Blocks,
		GPRsPerWarp,
		SMSize
	};
	id_t id;
	
	ConfigurationRegisterID(id_t id) : id(id) {}
};

struct SamplerIndex
{
	uint32_t id;
	
	SamplerIndex(uint32_t id) : id(id) {}
};

enum ArrayFormat
{
    AF_U8  = 0x01,
    AF_U16 = 0x02,
    AF_U32 = 0x03,
    AF_S8 = 0x08,
    AF_S16 = 0x09,
    AF_S32 = 0x0a,
    AF_F16 = 0x10,
    AF_F32 = 0x20
};

enum AddressMode
{
    AM_WRAP = 0,
    AM_CLAMP = 1,
    AM_MIRROR = 2
};

enum FilterMode
{
    FM_POINT = 0,
    FM_LINEAR = 1
};

enum TextureFlags
{
	TF_READ_AS_INTEGER = 0x01,
	TF_NORMALIZED_COORDINATES = 0x02
};



template<typename address_t = uint32_t>
struct SamplerBase
{
protected:
	int texunit;
	address_t address;
	uint32_t bytes;
	ArrayFormat format;
	int num_packed_components;
	AddressMode address_mode[3];
	FilterMode filter_mode;
	unsigned int flags;


public:
	SamplerBase() :
		texunit(-1),
		address(0),
		bytes(0),
		format(AF_U32),
		num_packed_components(1),
		filter_mode(FM_POINT),
		flags(0)
	{
		for(int d = 0; d != 3; ++d) {
			address_mode[d] = AM_WRAP;
		}
	}
	
	int GetTexUnit() const {
		return texunit; }
	address_t GetBaseAddress() const {
		return address; }
	int GetNumPackedComponents() const {
		return num_packed_components; }
	ArrayFormat GetFormat() const {
		return ArrayFormat(format); }
	AddressMode GetAddressMode(unsigned int i) const {
		assert(i < 3); return address_mode[i]; }
	FilterMode GetFilterMode() const {
		return filter_mode; }
	uint32_t GetFlags() const {
		return flags; }

	void SetAddress(address_t addr, unsigned int bytes) {
		address = addr;
		this->bytes = bytes;
	}
		
	void SetFormat(ArrayFormat fmt, int NumPackedComponents) {
		format = fmt;
		num_packed_components = NumPackedComponents;
	}
	
	void SetAddressMode(int dim, AddressMode am) {
		assert(dim >= 0 && dim < 3);
		address_mode[dim] = am;
	}
	
	void SetFilterMode(FilterMode fm) {
		filter_mode = fm;		
	}
	void SetFlags(unsigned int Flags) {
		flags = Flags;
	}
};

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
