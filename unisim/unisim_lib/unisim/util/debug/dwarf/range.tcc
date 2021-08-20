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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_RANGE_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_RANGE_TCC__

#include <unisim/util/debug/dwarf/util.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_RangeListEntry<MEMORY_ADDR>::DWARF_RangeListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *_dw_cu)
	: dw_cu(_dw_cu)
	, next(0)
	, offset(0xffffffffffffffffULL)
	, id(0)
	, begin(0)
	, end(0)
{
}

template <class MEMORY_ADDR>
DWARF_RangeListEntry<MEMORY_ADDR>::~DWARF_RangeListEntry()
{
}

template <class MEMORY_ADDR>
bool DWARF_RangeListEntry<MEMORY_ADDR>::IsBaseAddressSelection() const
{
	return begin > end;
}

template <class MEMORY_ADDR>
bool DWARF_RangeListEntry<MEMORY_ADDR>::IsEndOfList() const
{
	return (begin == 0) && (end == 0);
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_RangeListEntry<MEMORY_ADDR>::GetBegin() const
{
	return begin;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_RangeListEntry<MEMORY_ADDR>::GetEnd() const
{
	return end;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_RangeListEntry<MEMORY_ADDR>::GetBaseAddress() const
{
	return end;
}

template <class MEMORY_ADDR>
bool DWARF_RangeListEntry<MEMORY_ADDR>::HasOverlap(MEMORY_ADDR base_addr, MEMORY_ADDR addr, MEMORY_ADDR length) const
{
	return unisim::util::debug::dwarf::HasOverlapEx(base_addr + begin, base_addr + end, addr, addr + length);
}

template <class MEMORY_ADDR>
const DWARF_RangeListEntry<MEMORY_ADDR> *DWARF_RangeListEntry<MEMORY_ADDR>::GetNext() const
{
	return next;
}

template <class MEMORY_ADDR>
uint64_t DWARF_RangeListEntry<MEMORY_ADDR>::GetOffset() const
{
	return offset;
}

template <class MEMORY_ADDR>
void DWARF_RangeListEntry<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
}

template <class MEMORY_ADDR>
std::string DWARF_RangeListEntry<MEMORY_ADDR>::GetHREF() const
{
	std::stringstream sstr;
	sstr << "debug_ranges/" << (id / debug_range_per_file) << ".html#range-" << id;
	return sstr.str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_RangeListEntry<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
int64_t DWARF_RangeListEntry<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	int64_t size = 0;
	
	endian_type file_endianness = dw_cu->GetHandler()->GetFileEndianness();
	
	switch(dw_cu->GetAddressSize())
	{
		case 2:
			{
				uint16_t begin16;
				if(max_size < sizeof(begin16)) return -1;
				memcpy(&begin16, rawdata, sizeof(begin16));
				begin16 = Target2Host(file_endianness, begin16);
				rawdata += sizeof(begin16);
				max_size -= sizeof(begin16);
				size += sizeof(begin16);
				begin = (MEMORY_ADDR) begin16;

				uint16_t end16;
				if(max_size < sizeof(end16)) return -1;
				memcpy(&end16, rawdata, sizeof(end16));
				end16 = Target2Host(file_endianness, end16);
				rawdata += sizeof(end16);
				max_size -= sizeof(end16);
				size += sizeof(end16);
				end = (MEMORY_ADDR) end16;
			}
			break;
		case 4:
			{
				uint32_t begin32;
				if(max_size < sizeof(begin32)) return -1;
				memcpy(&begin32, rawdata, sizeof(begin32));
				begin32 = Target2Host(file_endianness, begin32);
				rawdata += sizeof(begin32);
				max_size -= sizeof(begin32);
				size += sizeof(begin32);
				begin = (MEMORY_ADDR) begin32;

				uint32_t end32;
				if(max_size < sizeof(end32)) return -1;
				memcpy(&end32, rawdata, sizeof(end32));
				end32 = Target2Host(file_endianness, end32);
				rawdata += sizeof(end32);
				max_size -= sizeof(end32);
				size += sizeof(end32);
				end = (MEMORY_ADDR) end32;
			}
			break;
		case 8:
			{
				uint64_t begin64;
				if(max_size < sizeof(begin64)) return -1;
				memcpy(&begin64, rawdata, sizeof(begin64));
				begin64 = Target2Host(file_endianness, begin64);
				rawdata += sizeof(begin64);
				max_size -= sizeof(begin64);
				size += sizeof(begin64);
				begin = (MEMORY_ADDR) begin64;

				uint64_t end64;
				if(max_size < sizeof(end64)) return -1;
				memcpy(&end64, rawdata, sizeof(end64));
				end64 = Target2Host(file_endianness, end64);
				rawdata += sizeof(end64);
				max_size -= sizeof(end64);
				size += sizeof(end64);
				end = (MEMORY_ADDR) end64;
			}
			break;
		default:
			return -1;
	}

	return size;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RangeListEntry<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	if(IsEndOfList()) return os << "<DW_EOL offset=\"" << offset << "\"/>";
	if(IsBaseAddressSelection())
	{
		os << "<DW_BASE_ADDRESS_SELECTION offset=\"" << offset << "\" largest_address_offset=\"0x" << std::hex << begin << std::dec << "\" address=\"0x" << std::hex << end << std::dec << "\"/>";
		return os;
	}
	os << "<DW_RANGE id=\"range-" << id << "\" offset=\"" << offset << "\" begin_address_offset=\"0x" << std::hex << begin << std::dec << "\" end_address_offset=\"0x" << std::hex << end << std::dec << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RangeListEntry<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"range-" << id << "\">" << std::endl;
	if(IsEndOfList())
	{
		os << "<td>End-of-List</td>";
	}
	else if(IsBaseAddressSelection())
	{
		os << "<td>Base address selection</td>";
	}
	else
	{
		os << "<td>Address Range</td>";
	}
	os << "<td>" << id << "</td>" << std::endl;
	os << "<td>" << offset << "</td>" << std::endl;
	os << "<td>";
	const DWARF_RangeListEntry<MEMORY_ADDR> *next = DWARF_RangeListEntry<MEMORY_ADDR>::GetNext();
	if(next)
	{
		os << "<a href=\"../" << next->GetHREF() << "\">range #" << next->GetId() << "</a>" << std::endl;
	}
	else
	{
		os << "-";
	}
	os << "</td>";
	if(IsEndOfList())
	{
		os << "<td>-</td>";
	}
	else if(IsBaseAddressSelection())
	{
		os << "<td><table><tr><th>Largest address offset</th><th>Address</th></tr>";
		os << "<tr><td>0x" << std::hex << begin << std::dec << "</td><td>0x" << std::hex << end << std::dec << "</td></tr></table></td>";
	}
	else
	{
		os << "<td><table><tr><th>Beginning address offset</th><th>End Address offset</th></tr>";
		os << "<tr><td>0x" << std::hex << begin << std::dec << "</td><td>0x" << std::hex << end << std::dec << "</td></tr></table></td>";
	}
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_RangeListEntry<MEMORY_ADDR>& dw_range_list_entry)
{
	if(dw_range_list_entry.IsEndOfList()) return os << "EOL";
	if(dw_range_list_entry.IsBaseAddressSelection()) os << "Base";
	return os << "[0x" << std::hex << dw_range_list_entry.begin << "-0x" << dw_range_list_entry.end << "]";
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_RANGE_TCC__
