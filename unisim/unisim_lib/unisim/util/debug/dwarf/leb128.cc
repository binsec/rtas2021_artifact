/*
 *  Copyright (c) 2010,
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

#include <unisim/util/debug/dwarf/leb128.hh>
#include <string.h>
#include <limits>
#include <sstream>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

DWARF_LEB128::DWARF_LEB128()
	: leb128(0)
{
}

DWARF_LEB128::DWARF_LEB128(const DWARF_LEB128& _leb128)
	: leb128(0)
{
	leb128 = _leb128.leb128;
/*	if(_leb128.leb128)
	{
		unsigned int byte_size = _leb128.GetByteSize();
		leb128 = new uint8_t[byte_size];
		memcpy(leb128, _leb128.leb128, byte_size);
	}*/
}

DWARF_LEB128::~DWARF_LEB128()
{
/*	if(leb128)
	{
		delete[] leb128;
		leb128 = 0;
	}*/
}

template <typename T>
bool DWARF_LEB128::Fit(const T *t) const
{
	return 8 * sizeof(T) >= GetBitLength();
}

template <typename T>
DWARF_LEB128::operator T() const
{
	T result = 0;
	if(leb128)
	{
		const uint8_t *p = leb128;
		unsigned int shift = 0;
		unsigned int size = 8 * sizeof(T);
		uint8_t byte;
		do
		{
			byte = *(p++);
			result |= (T)(byte & 0x7f) << shift;
			shift += 7;
		}
		while(byte & 0x80);

		if(std::numeric_limits<T>::is_signed)
		{
			if((shift < size) && (byte & 0x40))
			result |= -((T) 1 << shift);
		}
	}
	return result;
}

DWARF_LEB128& DWARF_LEB128::operator = (const DWARF_LEB128& _leb128)
{
	leb128 = _leb128.leb128;
	
/*	if(leb128)
	{
		delete[] leb128;
		leb128 = 0;
	}

	if(_leb128.leb128)
	{
		unsigned int byte_size = _leb128.GetByteSize();
		leb128 = new uint8_t[byte_size];
		memcpy(leb128, _leb128.leb128, byte_size);
	}*/
	
	return *this;
}

bool DWARF_LEB128::operator == (const DWARF_LEB128& _leb128) const
{
	unsigned int byte_index;
	uint8_t byte1, byte2;

	byte_index = 0;
	do
	{
		byte1 = leb128[byte_index];
		byte2 = _leb128.leb128[byte_index];
		if(byte1 != byte2) return false;
		byte_index++;
	} while(byte1 & 0x80);
		
	return true;

}

unsigned int DWARF_LEB128::GetByteSize() const
{
	unsigned int byte_index;

	byte_index = 0;
	do {} while(leb128[byte_index++] & 0x80);
		
	return byte_index;
}

unsigned int DWARF_LEB128::GetBitLength() const
{
	return GetByteSize() * 7;
}

int64_t DWARF_LEB128::Load(const uint8_t *rawdata, uint64_t max_size)
{
// 	if(leb128)
// 	{
// 		delete[] leb128;
// 		leb128 = 0;
// 	}

	unsigned int byte_index;

	byte_index = 0;
	do
	{
		if(byte_index >= max_size) return -1;
	}
	while(rawdata[byte_index++] & 0x80);
	
	unsigned int byte_size = byte_index;

	leb128 = rawdata;
// 	leb128 = new uint8_t[byte_size];
// 	
// 	for(byte_index = 0; byte_index < byte_size; byte_index++)
// 	{
// 		leb128[byte_index] = rawdata[byte_index];
// 	}

	return byte_size;
}

bool DWARF_LEB128::IsZero() const
{
	return !leb128 || (leb128 && ((*leb128) == 0));
}

std::string DWARF_LEB128::to_hex(bool is_signed) const
{
	if(leb128)
	{
		static char hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
		unsigned int bit_length = GetBitLength();
		unsigned int byte_length = (bit_length + 7) / 8;
		uint8_t data[byte_length];
		memset(data, 0, byte_length);

		unsigned int shift = 0;
		unsigned int i;
		const uint8_t *p = leb128;
		uint8_t byte;
		do
		{
			byte = *(p++);
			for(i = 0; i < 7; i++, shift++)
			{
				data[shift / 8] |= (byte & (1 << i)) << (shift % 8);
			}
		}
		while(byte & 0x80);

		if(is_signed)
		{
			if((shift < bit_length) && (byte & 0x40))
				data[shift / 8] |= -(1 << (shift % 8));
		}
		
		std::string result;
		int j;
		for(j = byte_length - 1; j >= 0; j--)
		{
			result += hex_chars[(data[j] >> 4) & 0xf];
			result += hex_chars[data[j] & 0xf];
		}
		return result;
	}
	return std::string();
}

std::string DWARF_LEB128::to_string(bool is_signed) const
{
	if(leb128)
	{
		if(is_signed)
		{
			if(Fit<int64_t>())
			{
				std::stringstream sstr;
				sstr << (int64_t) *this;
				return sstr.str();
			}
			else
			{
				std::stringstream sstr;
				sstr << "0x" << to_hex(is_signed);
				return sstr.str();
			}
		}
		else
		{
			if(Fit<uint64_t>())
			{
				std::stringstream sstr;
				sstr << (uint64_t) *this;
				return sstr.str();
			}
			else
			{
				std::stringstream sstr;
				sstr << "0x" << to_hex(is_signed);
				return sstr.str();
			}
		}
	}
	return std::string();
}

template bool DWARF_LEB128::Fit<char>(const char *t = 0) const;
template bool DWARF_LEB128::Fit<short>(const short *t = 0) const;
template bool DWARF_LEB128::Fit<int>(const int *t = 0) const;
template bool DWARF_LEB128::Fit<long>(const long *t = 0) const;
template bool DWARF_LEB128::Fit<long long>(const long long *t = 0) const;
template bool DWARF_LEB128::Fit<unsigned char>(const unsigned char *t = 0) const;
template bool DWARF_LEB128::Fit<unsigned short>(const unsigned short *t = 0) const;
template bool DWARF_LEB128::Fit<unsigned int>(const unsigned int *t = 0) const;
template bool DWARF_LEB128::Fit<unsigned long>(const unsigned long *t = 0) const;
template bool DWARF_LEB128::Fit<unsigned long long>(const unsigned long long *t = 0) const;

template DWARF_LEB128::operator char() const;
template DWARF_LEB128::operator short() const;
template DWARF_LEB128::operator int() const;
template DWARF_LEB128::operator long() const;
template DWARF_LEB128::operator long long() const;
template DWARF_LEB128::operator unsigned char() const;
template DWARF_LEB128::operator unsigned short() const;
template DWARF_LEB128::operator unsigned int() const;
template DWARF_LEB128::operator unsigned long() const;
template DWARF_LEB128::operator unsigned long long() const;

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
