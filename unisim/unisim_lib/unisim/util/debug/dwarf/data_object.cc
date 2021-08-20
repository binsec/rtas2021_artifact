/*
 *  Copyright (c) 2013,
 *  Commissariat a l'Energie Atomique (CEA)
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
 *   - Neither the name of CEA nor the names of its contributors may be used to
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <unisim/util/debug/dwarf/data_object.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

DWARF_BitVector::DWARF_BitVector(unisim::util::endian::endian_type _target_endian)
	: target_endian(_target_endian)
	, bit_size(0)
{
}

DWARF_BitVector::~DWARF_BitVector()
{
}

void DWARF_BitVector::Clear()
{
	storage.clear();
	bit_size = 0;
}

// A11 A10 A9 A8 A7 A6 A5 A4 A3 A2 A1 A0 ||  B7 B6 B5 B4 B3 B2 B1 B0 ||  C11 C10 C9 C8 C7 C6 C5 C4 C3 C2 C1 C0
// 
// big endian: A11 A10 A9 A8 A7 A6 A5 A4 ||  A3 A2 A1 A0 B7 B6 B5 B4 ||  B3 B2 B1 B0 C11 C10 C9 C8 || C7 C6 C5 C4 C3 C2 C1 C0
// 
// little endian: A7 A6 A5 A4 A3 A2 A1 A0 || B3 B2 B1 B0 A11 A10 A9 A8 || C3 C2 C1 C0 B7 B6 B5 B4 || C11 C10 C9 C8 C7 C6 C5 C4

void DWARF_BitVector::Append(uint64_t source_value, unsigned int source_bit_offset, unsigned int source_bit_size)
{
	unsigned int remaining_bits = source_bit_size;
	
	while(remaining_bits)
	{
		unsigned int dest_byte_index = bit_size / 8;
		if(dest_byte_index >= storage.size()) storage.resize(dest_byte_index + 1);
		unsigned int dest_bit_offset = bit_size % 8;
		unsigned int bit_size_to_byte_boundary = 8 - dest_bit_offset;
		unsigned int bit_sz = bit_size_to_byte_boundary;
		if(bit_sz > remaining_bits) bit_sz = remaining_bits;
		
		unsigned int source_shift = (target_endian == unisim::util::endian::E_BIG_ENDIAN) ? source_bit_size - source_bit_offset - bit_sz : source_bit_offset;
		unsigned int dest_shift = (target_endian == unisim::util::endian::E_BIG_ENDIAN) ? 8 - dest_bit_offset - bit_sz : dest_bit_offset;
		uint8_t mask = ((1 << bit_sz) - 1) << dest_shift;
		uint8_t v = (source_value >> source_shift) << dest_shift;
		storage[dest_byte_index] = storage[dest_byte_index] | (v & mask);
		source_bit_offset = source_bit_offset + bit_sz;

		bit_size += bit_sz;
		remaining_bits = remaining_bits - bit_sz;
	}
}

void DWARF_BitVector::Append(const uint8_t *source_buffer, unsigned int source_bit_offset, unsigned int source_bit_size)
{
	unsigned int remaining_bits = source_bit_size;
	
	while(remaining_bits)
	{
		unsigned int source_byte_index = source_bit_offset / 8;
		
		unsigned int l_source_bit_offset = source_bit_offset % 8;
		unsigned int source_bit_size_to_byte_boundary = 8 - l_source_bit_offset;

		unsigned int dest_byte_index = bit_size / 8;
		if(dest_byte_index >= storage.size()) storage.resize(dest_byte_index + 1);
		
		unsigned int l_dest_bit_offset = bit_size % 8;
		unsigned int dest_bit_size_to_byte_boundary = 8 - l_dest_bit_offset;
		
		unsigned int bit_sz = remaining_bits;
		if(bit_sz > source_bit_size_to_byte_boundary) bit_sz = source_bit_size_to_byte_boundary;
		if(bit_sz > dest_bit_size_to_byte_boundary) bit_sz = dest_bit_size_to_byte_boundary;
		
		unsigned int source_shift = (target_endian == unisim::util::endian::E_BIG_ENDIAN) ? 8 - l_source_bit_offset - bit_sz : l_source_bit_offset;
		unsigned int dest_shift = (target_endian == unisim::util::endian::E_BIG_ENDIAN) ? 8 - l_dest_bit_offset - bit_sz : l_dest_bit_offset;
		uint8_t mask = ((1 << bit_sz) - 1) << dest_shift;
		uint8_t v = (source_buffer[source_byte_index] >> source_shift) << dest_shift;
		storage[dest_byte_index] = storage[dest_byte_index] | (v & mask);
		
		bit_size += bit_sz;
		source_bit_offset += bit_sz;
		remaining_bits = remaining_bits - bit_sz;
	}
}

bool DWARF_BitVector::Read(unsigned int source_bit_offset, uint64_t& dest_value, unsigned int dest_bit_offset, unsigned int dest_bit_size) const
{
	dest_value = 0;

	unsigned int remaining_bits = dest_bit_size;
	
	while(remaining_bits)
	{
		unsigned int source_byte_index = source_bit_offset / 8;
		
		if(source_byte_index >= storage.size()) return false; // overflow
		unsigned int l_source_bit_offset = source_bit_offset % 8;
		unsigned int bit_size_to_byte_boundary = 8 - l_source_bit_offset;
		unsigned int bit_sz = bit_size_to_byte_boundary;
		if(bit_sz > remaining_bits) bit_sz = remaining_bits;
		
		unsigned source_shift = (target_endian == unisim::util::endian::E_BIG_ENDIAN) ? 8 - l_source_bit_offset - bit_sz : l_source_bit_offset;
		unsigned dest_shift = (target_endian == unisim::util::endian::E_BIG_ENDIAN) ? dest_bit_size - dest_bit_offset - bit_sz : dest_bit_offset;
		uint64_t mask = (uint64_t)((1 << bit_sz) - 1) << dest_shift;
		uint64_t v = (uint64_t)(storage[source_byte_index] >> source_shift) << dest_shift;
		dest_value = dest_value | (v & mask);

		dest_bit_offset = dest_bit_offset + bit_sz;
		source_bit_offset = source_bit_offset + bit_sz;
		remaining_bits = remaining_bits - bit_sz;
	}
	return true;
}

bool DWARF_BitVector::Write(unsigned int dest_bit_offset, uint64_t source_value, unsigned int source_bit_offset, unsigned int source_bit_size)
{
	unsigned int remaining_bits = source_bit_size;
	
	while(remaining_bits)
	{
		unsigned int dest_byte_index = dest_bit_offset / 8;
		
		if(dest_byte_index >= storage.size()) return false; // overflow
		unsigned int l_dest_bit_offset = dest_bit_offset % 8;
		unsigned int bit_size_to_byte_boundary = 8 - l_dest_bit_offset;
		unsigned int bit_sz = bit_size_to_byte_boundary;
		if(bit_sz > remaining_bits) bit_sz = remaining_bits;
		
		unsigned int source_shift = (target_endian == unisim::util::endian::E_BIG_ENDIAN) ? source_bit_size - source_bit_offset - bit_sz : source_bit_offset;
		unsigned int dest_shift = (target_endian == unisim::util::endian::E_BIG_ENDIAN) ? 8 - l_dest_bit_offset - bit_sz : l_dest_bit_offset;
		uint8_t mask = ((1 << bit_sz) - 1) << dest_shift;
		uint8_t v = (source_value >> source_shift) << dest_shift;
		storage[dest_byte_index] = (storage[dest_byte_index] & ~mask) | (v & mask);
		
		source_bit_offset = source_bit_offset + bit_sz;
		dest_bit_offset = dest_bit_offset + bit_sz;
		remaining_bits = remaining_bits - bit_sz;
	}
	return true;
}

bool DWARF_BitVector::Read(unsigned int source_bit_offset, uint8_t *dest_buffer, unsigned int dest_bit_offset, unsigned int dest_bit_size) const
{
	unsigned int remaining_bits = dest_bit_size;
	
	while(remaining_bits)
	{
		unsigned int dest_byte_index = dest_bit_offset / 8;
		
		unsigned int l_dest_bit_offset = dest_bit_offset % 8;
		unsigned int dest_bit_size_to_byte_boundary = 8 - l_dest_bit_offset;

		unsigned int source_byte_index = source_bit_offset / 8;
		if(source_byte_index >= storage.size()) return false; // overflow
		
		unsigned int l_source_bit_offset = source_bit_offset % 8;
		unsigned int source_bit_size_to_byte_boundary = 8 - l_source_bit_offset;
		
		unsigned int bit_sz = remaining_bits;
		if(bit_sz > source_bit_size_to_byte_boundary) bit_sz = source_bit_size_to_byte_boundary;
		if(bit_sz > dest_bit_size_to_byte_boundary) bit_sz = dest_bit_size_to_byte_boundary;

		unsigned int source_shift = (target_endian == unisim::util::endian::E_BIG_ENDIAN) ? 8 - l_source_bit_offset - bit_sz : l_source_bit_offset;
		unsigned int dest_shift = (target_endian == unisim::util::endian::E_BIG_ENDIAN) ? 8 - l_dest_bit_offset - bit_sz : l_dest_bit_offset;
		uint8_t mask = ((1 << bit_sz) - 1) << dest_shift;
		uint8_t v = (storage[source_byte_index] >> source_shift) << dest_shift;
		dest_buffer[dest_byte_index] = (dest_buffer[dest_byte_index] & ~mask) | (v & mask);
		
		source_bit_offset += bit_sz;
		dest_bit_offset += bit_sz;
		remaining_bits = remaining_bits - bit_sz;
	}
	return true;
}

bool DWARF_BitVector::Write(unsigned int dest_bit_offset, const uint8_t *source_buffer, unsigned int source_bit_offset, unsigned int source_bit_size)
{
	unsigned int remaining_bits = source_bit_size;
	
	while(remaining_bits)
	{
		unsigned int source_byte_index = source_bit_offset / 8;
		
		unsigned int l_source_bit_offset = source_bit_offset % 8;
		unsigned int source_bit_size_to_byte_boundary = 8 - l_source_bit_offset;

		unsigned int dest_byte_index = dest_bit_offset / 8;
		if(dest_byte_index >= storage.size()) return false; // overflow
		
		unsigned int l_dest_bit_offset = dest_bit_offset % 8;
		unsigned int dest_bit_size_to_byte_boundary = 8 - l_dest_bit_offset;
		
		unsigned int bit_sz = remaining_bits;
		if(bit_sz > dest_bit_size_to_byte_boundary) bit_sz = dest_bit_size_to_byte_boundary;
		if(bit_sz > source_bit_size_to_byte_boundary) bit_sz = source_bit_size_to_byte_boundary;

		unsigned int source_shift = (target_endian == unisim::util::endian::E_BIG_ENDIAN) ? 8 - l_source_bit_offset - bit_sz : l_source_bit_offset;
		unsigned int dest_shift = (target_endian == unisim::util::endian::E_BIG_ENDIAN) ? 8 - l_dest_bit_offset - bit_sz : l_dest_bit_offset;
		uint8_t mask = ((1 << bit_sz) - 1) << dest_shift;
		uint8_t v = (source_buffer[source_byte_index] >> source_shift) << dest_shift;
		storage[dest_byte_index] = (storage[dest_byte_index] & ~mask) | (v & mask);

		dest_bit_offset += bit_sz;
		source_bit_offset += bit_sz;
		remaining_bits = remaining_bits - bit_sz;
	}
	return true;
}

std::ostream& operator << (std::ostream& os, const DWARF_BitVector& bv)
{
	unsigned int i;
	unsigned int n = bv.storage.size();

	std::ios::fmtflags os_flags(os.flags());
	os << std::hex;
	for(i = 0; i < n; i++)
	{
		if(i) os << " ";
		os << "0x" << std::hex << (unsigned int) bv.storage[i];
	}
	os.flags(os_flags);
	return os;
}


} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
