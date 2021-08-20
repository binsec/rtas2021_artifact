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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_MEMORY_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_MEMORY_TCC__

// Declarations in cpu.hh

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template <class CONFIG>
VectorAddress<CONFIG> CPU<CONFIG>::LocalAddress(VectorAddress<CONFIG> const & addr, unsigned int segment)
{
	assert(segment == 0);
	// Local memory interleaved.
	// Across all threads in the GPU * 32-bit? (rounded to warp size)
	// TODO: num_warp can de different for last block!
	unsigned int chunk_size = core_count * num_warps * WARP_SIZE * 4;
	address_t chunk_offset_base = (coreid * num_warps + current_warpid) * WARP_SIZE * 4 + CONFIG::LOCAL_START;

	VecAddr chunk_offset(chunk_offset_base);
	for(unsigned int i = 0; i != WARP_SIZE; ++i) {
		chunk_offset[i] += i * 4;
	}

	// TODO: check bounds	
	return chunk_size * addr + chunk_offset;
}

template <class CONFIG>
typename CONFIG::address_t CPU<CONFIG>::LocalAddress(address_t addr, unsigned int segment)
{
	assert(segment == 0);
	// Local memory interleaved.
	// Across all threads in the GPU * 32-bit? (rounded to warp size)
	// TODO: num_warp can de different for last block!
	unsigned int chunk_size = core_count * num_warps * WARP_SIZE * 4;
	address_t chunk_offset_base = (coreid * num_warps + current_warpid) * WARP_SIZE * 4 + CONFIG::LOCAL_START;
	return chunk_offset_base + addr * chunk_size;
}


template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::ReadConstant(VectorRegister<CONFIG> const & addr, uint32_t seg)
{
	assert(seg < CONFIG::CONST_SEG_NUM);
	VecReg v;
	Gather32(VecAddr(addr), v, 1, CONFIG::CONST_START + seg * CONFIG::CONST_SEG_SIZE);
	return v;
}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::ReadConstant(unsigned int addr, uint32_t seg)
{
	assert(seg < CONFIG::CONST_SEG_NUM);
	VecReg v;
	Broadcast32(addr, v, 1, CONFIG::CONST_START + seg * CONFIG::CONST_SEG_SIZE);
	return v;
}



template <class CONFIG>
VectorAddress<CONFIG> CPU<CONFIG>::EffectiveAddress(uint32_t reg, uint32_t addr_lo, uint32_t addr_hi,
	bool addr_imm, uint32_t shift)
{
	uint32_t addr_reg = (addr_hi << 2) | addr_lo;
	VecAddr offset;
	// [$a#addr_reg + dest]
	if(addr_imm) {
		// Immediate in words
		offset = VecAddr(reg << shift);
	}
	else {
		offset = VecAddr(GetGPR(reg));
	}

	if(addr_reg != 0) {
		offset += GetAddr(addr_reg);
	}
	return offset;
}

template <class CONFIG>
void CPU<CONFIG>::GatherShared(VectorRegister<CONFIG> & output, uint32_t src, uint32_t addr_lo, uint32_t addr_hi, bool addr_imm, std::bitset<CONFIG::WARP_SIZE> mask, SMType type)
{
	uint32_t shift = 0;
	if(type == SM_U16 || type == SM_S16) shift = 1;
	else if(type == SM_U32) shift = 2;

	if(((addr_hi << 2) | addr_lo) == 0 && addr_imm) {
		address_t addr = src;
		if(mask.any()) {
			ReadShared(addr, output, type);
		}
		output.SetScalar();
	}
	else {
		VecAddr offset = EffectiveAddress(src, addr_lo, addr_hi, addr_imm, shift);
		GatherShared(offset, output, mask, type);
	}
}

// For current block
template <class CONFIG>
void CPU<CONFIG>::GatherShared(VectorAddress<CONFIG> const & addr,
	VectorRegister<CONFIG> & data, std::bitset<CONFIG::WARP_SIZE> mask, SMType t)
{
	for(unsigned int i = 0; i != WARP_SIZE; ++i) {
		assert(!mask[i] || addr[i] < sm_size);
	}
	
	switch(t)
	{
	case SM_U32:
		Gather32(addr, data, mask, 1, CurrentWarp().GetSMAddress());
		break;
	case SM_U16:
	case SM_S16:
		Gather16(addr, data, mask, 1, CurrentWarp().GetSMAddress());
		break;
	case SM_U8:
		Gather8(addr, data, mask, 1, CurrentWarp().GetSMAddress());
		break;
	default:
		assert(false);
	}
}

template <class CONFIG>
void CPU<CONFIG>::ScatterShared(VectorRegister<CONFIG> const & output, uint32_t dest,
	uint32_t addr_lo, uint32_t addr_hi, bool addr_imm,
	std::bitset<CONFIG::WARP_SIZE> mask, SMType type)
{

	uint32_t shift = 0;
	if(type == SM_U16 || type == SM_S16) shift = 1;
	else if(type == SM_U32) shift = 2;
	VecAddr offset = EffectiveAddress(dest, addr_lo, addr_hi, addr_imm, shift);

	for(unsigned int i = 0; i != WARP_SIZE; ++i) {
		assert(!mask[i] || offset[i] < sm_size);
	}
	
	// TODO: check memory range
	address_t base = CurrentWarp().GetSMAddress();
	switch(type)
	{
	case SM_U32:
		Scatter32(offset, output, mask, 1, base);
		break;
	case SM_U16:
	case SM_S16:
		Scatter16(offset, output, mask, 1, base);
		break;
	case SM_U8:
		Scatter8(offset, output, mask, 1, base);
		break;
	default:
		assert(false);
	}
}

// For current block
template <class CONFIG>
void CPU<CONFIG>::ReadShared(typename CONFIG::address_t addr, VectorRegister<CONFIG> & data, SMType t)
{
	switch(t)
	{
	case SM_U32:
		assert(4 * addr < sm_size);
		Broadcast32(addr, data, 4, CurrentWarp().GetSMAddress());
		break;
	case SM_U16:
	case SM_S16:
		assert(2 * addr < sm_size);
		Broadcast16(addr, data, 2, CurrentWarp().GetSMAddress());
		break;
	case SM_U8:
		assert(addr < sm_size);
		Broadcast8(addr, data, 1, CurrentWarp().GetSMAddress());
		break;
	default:
		assert(false);
	}
}

template <class CONFIG>
void CPU<CONFIG>::GatherConstant(VecReg & output, uint32_t src, uint32_t addr_lo,
	uint32_t addr_hi, bool addr_imm, uint32_t segment,
	std::bitset<CONFIG::WARP_SIZE> mask, SMType type)
{
	uint32_t shift = 0;
	if(type == SM_U16 || type == SM_S16) shift = 1;
	else if(type == SM_U32) shift = 2;
	VecAddr addr = EffectiveAddress(src, addr_lo, addr_hi, addr_imm, shift);
	address_t base = CONFIG::CONST_START + segment * CONFIG::CONST_SEG_SIZE;
	switch(type)
	{
	case SM_U32:
		Gather32(addr, output, mask, 1, base);
		break;
	case SM_U16:
	case SM_S16:
		Gather16(addr, output, mask, 1, base);
		break;
	case SM_U8:
		Gather8(addr, output, mask, 1, base);
		break;
	default:
		assert(false);
	}
}

template <class CONFIG>
void CPU<CONFIG>::Gather32(VecAddr const & addr, VecReg & data, std::bitset<CONFIG::WARP_SIZE> mask,
	uint32_t factor, address_t offset)
{
	for(unsigned int i = 0; i != WARP_SIZE; ++i)
	{
		if(mask[i]) {
			Read32(addr[i], data[i], factor, offset);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::Scatter32(VecAddr const & addr, VecReg const & data,
	std::bitset<CONFIG::WARP_SIZE> mask,
	uint32_t factor, address_t offset)
{
	// TODO: check overlap and warn
	for(unsigned int i = 0; i != WARP_SIZE; ++i)
	{
		if(mask[i]) {
			Write32(addr[i], data[i], factor, offset);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::Scatter16(VecAddr const & addr, VecReg const & data,
	std::bitset<CONFIG::WARP_SIZE> mask,
	uint32_t factor, address_t offset)
{
	// TODO: check overlap and warn
	for(unsigned int i = 0; i != WARP_SIZE; ++i)
	{
		if(mask[i]) {
			Write16(addr[i], data[i], factor, offset);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::Scatter8(VecAddr const & addr, VecReg const & data,
	std::bitset<CONFIG::WARP_SIZE> mask,
	uint32_t factor, address_t offset)
{
	// TODO: check overlap and warn
	for(unsigned int i = 0; i != WARP_SIZE; ++i)
	{
		if(mask[i]) {
			Write8(addr[i], data[i], factor, offset);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::Gather16(VecAddr const & addr, VecReg & data, std::bitset<CONFIG::WARP_SIZE> mask,
	uint32_t factor, address_t offset)
{
	for(unsigned int i = 0; i != WARP_SIZE; ++i)
	{
		if(mask[i]) {
			Read16(addr[i], data[i], factor, offset);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::Gather8(VecAddr const & addr, VecReg & data, std::bitset<CONFIG::WARP_SIZE> mask,
	uint32_t factor, address_t offset)
{
	for(unsigned int i = 0; i != WARP_SIZE; ++i)
	{
		if(mask[i]) {
			Read8(addr[i], data[i], factor, offset);
		}
	}
}


template <class CONFIG>
void CPU<CONFIG>::Broadcast32(address_t addr, VecReg & data,
	uint32_t factor, address_t offset)
{
	uint32_t val;
	Read32(addr, val, factor, offset);
	data = VecReg(val);
}

// TODO: add mask
template <class CONFIG>
void CPU<CONFIG>::Broadcast16(address_t addr, VecReg & data,
	uint32_t factor, address_t offset)
{
	uint32_t val = 0;
	Read16(addr, val, factor, offset);
	data = VecReg(val);	// mask...
}

template <class CONFIG>
void CPU<CONFIG>::Broadcast8(address_t addr, VecReg & data,
	uint32_t factor, address_t offset)
{
	uint32_t val = 0;
	Read8(addr, val, factor, offset);
	data = VecReg(val);	// mask...
}


template <class CONFIG>
void CPU<CONFIG>::Read32(address_t addr, uint32_t & data,
	uint32_t factor, address_t offset)
{
	assert(!((addr * factor + offset) & 3));	// Check alignment
	if(!ReadMemory(addr * factor + offset, &data, 4)) {
		throw MemoryAccessException<CONFIG>();
	}
	if(TraceLoadstore()) {
		cerr << " Read32 @" << std::hex << offset << "+" << addr << "*" << factor << ": "
			<< data << std::dec << endl;
	}
}

template <class CONFIG>
void CPU<CONFIG>::Write32(address_t addr, uint32_t data,
	uint32_t factor, address_t offset)
{
	assert(!((addr * factor + offset) & 3));
	if(TraceLoadstore()) {
		cerr << " Write32: " << std::hex << data
			<< " @" << offset << "+" << addr << "*" << factor << std::dec << endl;
	}
	if(!WriteMemory(addr * factor + offset, &data, 4)) {
		throw MemoryAccessException<CONFIG>();
	}
}

template <class CONFIG>
void CPU<CONFIG>::Read16(address_t addr, uint32_t & data,
	uint32_t factor, address_t offset)
{
	assert(!((addr * factor + offset) & 1));	// Check alignment
	if(!ReadMemory(addr * factor + offset, &data, 2)) {
		throw MemoryAccessException<CONFIG>();
	}
	if(TraceLoadstore()) {
		cerr << " Read16 @" << std::hex << offset << "+" << addr << "*" << factor << ": "
			<< data << std::dec << endl;
	}
}

template <class CONFIG>
void CPU<CONFIG>::Write16(address_t addr, uint32_t data,
	uint32_t factor, address_t offset)
{
	assert(!((addr * factor + offset) & 1));	// Check alignment
	if(TraceLoadstore()) {
		cerr << " Write16: " << std::hex << data
			<< " @" << offset << "+" << addr << "*" << factor << std::dec << endl;
	}
	if(!WriteMemory(addr * factor + offset, &data, 2)) {
		throw MemoryAccessException<CONFIG>();
	}
}

template <class CONFIG>
void CPU<CONFIG>::Read8(address_t addr, uint32_t & data,
	uint32_t factor, address_t offset)
{
	if(!ReadMemory(addr * factor + offset, &data, 1)) {
		throw MemoryAccessException<CONFIG>();
	}
	if(TraceLoadstore()) {
		cerr << " Read8 @" << std::hex << offset << "+" << addr << "*" << factor << ": "
			<< data << std::dec << endl;
	}
}

template <class CONFIG>
void CPU<CONFIG>::Write8(address_t addr, uint32_t data,
	uint32_t factor, address_t offset)
{
	if(TraceLoadstore()) {
		cerr << " Write8: " << std::hex << data
			<< " @" << offset << "+" << addr << "*" << factor << std::dec << endl;
	}
	if(!WriteMemory(addr * factor + offset, &data, 1)) {
		throw MemoryAccessException<CONFIG>();
	}
}

template <class CONFIG>
void CPU<CONFIG>::Gather(VecAddr const & addr, VecReg data[],
	std::bitset<CONFIG::WARP_SIZE> mask, DataType dt)
{
	switch(dt)
	{
	case DT_U32:
	case DT_S32:
		Gather32(addr, data[0], mask, 1, 0);
		break;
	case DT_U16:
	case DT_S16:
		Gather16(addr, data[0], mask, 1, 0);
		break;
	case DT_U8:
	case DT_S8:
		Gather8(addr, data[0], mask, 1, 0);
		break;
	case DT_U64:
		// Actual Gather64, not bust transfer?
		// TODO: check alignment
		// From little-endian mem to little-endian regs
		Gather32(addr, data[0], mask, 1, 0);
		Gather32(addr, data[1], mask, 1, 4);
		break;
	case DT_U128:
		// TODO: check alignment
		Gather32(addr, data[0], mask, 1, 0);
		Gather32(addr, data[1], mask, 1, 4);
		Gather32(addr, data[2], mask, 1, 8);
		Gather32(addr, data[3], mask, 1, 12);
		break;
	default:
		assert(false);
	}
}

template <class CONFIG>
void CPU<CONFIG>::Scatter(VecAddr const & addr, VecReg const data[],
	std::bitset<CONFIG::WARP_SIZE> mask, DataType dt)
{
	switch(dt)
	{
	case DT_U32:
	case DT_S32:
		Scatter32(addr, data[0], mask, 1, 0);
		break;
	case DT_U16:
	case DT_S16:
		Scatter16(addr, data[0], mask, 1, 0);
		break;
	case DT_U8:
	case DT_S8:
		Scatter8(addr, data[0], mask, 1, 0);
		break;
	case DT_U64:
		Scatter32(addr, data[0], mask, 1, 0);
		Scatter32(addr, data[1], mask, 1, 4);
		break;
	case DT_U128:
		Scatter32(addr, data[0], mask, 1, 0);
		Scatter32(addr, data[1], mask, 1, 4);
		Scatter32(addr, data[2], mask, 1, 8);
		Scatter32(addr, data[3], mask, 1, 12);
		break;
	default:
		assert(false);
	}
}

template <class CONFIG>
void CPU<CONFIG>::ScatterGlobal(VecReg const output[], uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, bool addr_imm, uint32_t segment, std::bitset<CONFIG::WARP_SIZE> mask, DataType dt)
{
	unsigned int shift = 0;
	if(dt == DT_U16 || dt == DT_S16) shift = 1;	// 16-bit
	else if(dt == DT_U32 || dt == DT_S32) shift = 2;	// 32-bit
	else if(dt == DT_U64) shift = 3;
	else if(dt == DT_U128) shift = 4;
	
	// [seg][$a#addr_reg + dest]
	VecAddr offset = EffectiveAddress(dest, addr_lo, addr_hi, addr_imm, shift);

	// TODO: segment
	assert(segment == 14);
	Scatter(offset, output, mask, dt);

	if(export_stats)
		GetOpStats().ScatterGlobal(offset, shift, mask);
}

template <class CONFIG>
void CPU<CONFIG>::GatherGlobal(VecReg output[], uint32_t src, uint32_t addr_lo, uint32_t addr_hi, bool addr_imm, uint32_t segment, std::bitset<CONFIG::WARP_SIZE> mask, DataType dt)
{
	unsigned int shift = 0;
	if(dt == DT_U16 || dt == DT_S16) shift = 1;	// 16-bit
	else if(dt == DT_U32 || dt == DT_S32) shift = 2;	// 32-bit
	else if(dt == DT_U64) shift = 3;
	else if(dt == DT_U128) shift = 4;
	
	// [seg][$a#addr_reg + dest]
	VecAddr offset = EffectiveAddress(src, addr_lo, addr_hi, addr_imm, shift);
	
	// TODO: segment??
	assert(segment == 14);
	Gather(offset, output, mask, dt);
	
	if(export_stats)
		GetOpStats().GatherGlobal(offset, shift, mask);
}

template <class CONFIG>
void CPU<CONFIG>::ScatterLocal(VecReg const output[], uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, bool addr_imm, uint32_t segment, std::bitset<CONFIG::WARP_SIZE> mask, DataType dt)
{
	// Local memory always byte-indexed
	unsigned int shift = 0;
	/*if(dt == DT_U16 || dt == DT_S16) shift = 1;	// 16-bit
	else if(dt == DT_U32 || dt == DT_S32) shift = 2;	// 32-bit
	else if(dt == DT_U64) shift = 3;
	else if(dt == DT_U128) shift = 4;*/
	
	assert(dt != DT_U64 && dt != DT_U128);
	unsigned int addr = (addr_hi << 2) | addr_lo;
	if(addr == 0 && addr_imm && dt == DT_U32) {
		assert(segment == 0);
		StoreLocal32(output[0], dest);
		if(export_stats)
			GetOpStats().StoreLocal(4);
	}
	else {
		// [seg][$a#addr_reg + dest]
		VecAddr offset = EffectiveAddress(dest, addr_lo, addr_hi, addr_imm, shift);

		offset = LocalAddress(offset, segment);

		Scatter(offset, output, mask, dt);
		if(export_stats)
			GetOpStats().ScatterLocal(offset, DataTypeLogSize(dt), mask);
	}
}

template <class CONFIG>
void CPU<CONFIG>::GatherLocal(VecReg output[], uint32_t src, uint32_t addr_lo, uint32_t addr_hi, bool addr_imm, uint32_t segment, std::bitset<CONFIG::WARP_SIZE> mask, DataType dt)
{
	// Local memory always byte-indexed
	unsigned int shift = 0;
	/*if(dt == DT_U16 || dt == DT_S16) shift = 1;	// 16-bit
	else if(dt == DT_U32 || dt == DT_S32) shift = 2;	// 32-bit
	else if(dt == DT_U64) shift = 3;
	else if(dt == DT_U128) shift = 4;*/
	
	assert(dt != DT_U64 && dt != DT_U128);
	unsigned int addr = (addr_hi << 2) | addr_lo;
	if(addr == 0 && addr_imm && dt == DT_U32) {
		assert(segment == 0);
		LoadLocal32(output[0], src);
		if(export_stats)
			GetOpStats().LoadLocal(4);
	}
	else {
		// [seg][$a#addr_reg + dest]
		VecAddr offset = EffectiveAddress(src, addr_lo, addr_hi, addr_imm, shift);
	
		offset = LocalAddress(offset, segment);

		Gather(offset, output, mask, dt);
		if(export_stats)
			GetOpStats().GatherLocal(offset, DataTypeLogSize(dt), mask);
	}
}

template <class CONFIG>
void CPU<CONFIG>::LoadLocal32(VecReg & output, address_t addr)
{
	addr = LocalAddress(addr, 0);
	if(!ReadMemory(addr, &output.v[0], CONFIG::WARP_SIZE * 4)) {
		throw MemoryAccessException<CONFIG>();
	}	
}

template <class CONFIG>
void CPU<CONFIG>::StoreLocal32(VecReg const & output, address_t addr)
{
	addr = LocalAddress(addr, 0);
	if(!WriteMemory(addr, &output.v[0], CONFIG::WARP_SIZE * 4)) {
		throw MemoryAccessException<CONFIG>();
	}
}

template <class CONFIG>
void CPU<CONFIG>::Sample1DS32(unsigned int sampler,
	VectorRegister<CONFIG> dest[],
	VectorRegister<CONFIG> const src[],
	uint32_t destBitfield)
{
	assert(sampler < CONFIG::MAX_SAMPLERS);
	samplers[sampler].Sample1DS32(dest, src, destBitfield);
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
