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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_ADDR_RANGE_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_ADDR_RANGE_TCC__

#include <unisim/util/debug/dwarf/encoding.hh>
#include <unisim/util/debug/dwarf/util.hh>
#include <iostream>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_AddressRangeDescriptor<MEMORY_ADDR>::DWARF_AddressRangeDescriptor(const DWARF_AddressRanges<MEMORY_ADDR> *_dw_aranges)
	: dw_aranges(_dw_aranges)
	, addr(0)
	, length(0)
{
}

template <class MEMORY_ADDR>
DWARF_AddressRangeDescriptor<MEMORY_ADDR>::~DWARF_AddressRangeDescriptor()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_AddressRangeDescriptor<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size)
{
	int64_t size = 0;
	endian_type file_endianness = dw_aranges->GetFileEndianness();
	
	uint8_t address_size = dw_aranges->GetAddressSize();
	if(max_size < address_size) return -1;
	
	switch(address_size)
	{
		case sizeof(uint16_t):
			{
				uint16_t addr16;
				memcpy(&addr16, rawdata, sizeof(addr16));
				addr16 = Target2Host(file_endianness, addr16);
				addr = addr16;
			}
			break;
		case sizeof(uint32_t):
			{
				uint32_t addr32;
				memcpy(&addr32, rawdata, sizeof(addr32));
				addr32 = Target2Host(file_endianness, addr32);
				addr = addr32;
			}
			break;
		case sizeof(uint64_t):
			{
				uint64_t addr64;
				memcpy(&addr64, rawdata, sizeof(addr64));
				addr64 = Target2Host(file_endianness, addr64);
				addr = addr64;
			}
			break;
		default:
			return -1;
	}
	
	size += address_size;
	rawdata += address_size;
	max_size -= address_size;

	if(max_size < address_size) return -1;
	
	switch(address_size)
	{
		case sizeof(uint16_t):
			{
				uint16_t length16;
				memcpy(&length16, rawdata, sizeof(length16));
				length16 = Target2Host(file_endianness, length16);
				length = length16;
			}
			break;
		case sizeof(uint32_t):
			{
				uint32_t length32;
				memcpy(&length32, rawdata, sizeof(length32));
				length32 = Target2Host(file_endianness, length32);
				length = length32;
			}
			break;
		case sizeof(uint64_t):
			{
				uint64_t length64;
				memcpy(&length64, rawdata, sizeof(length64));
				length64 = Target2Host(file_endianness, length64);
				length = length64;
			}
			break;
		default:
			return -1;
	}
	
	size += address_size;
	rawdata += address_size;
	max_size -= address_size;
	
	return size;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_AddressRangeDescriptor<MEMORY_ADDR>::GetAddress() const
{
	return addr;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_AddressRangeDescriptor<MEMORY_ADDR>::GetLength() const
{
	return length;
}

template <class MEMORY_ADDR>
bool DWARF_AddressRangeDescriptor<MEMORY_ADDR>::IsNull() const
{
	return (addr == 0) && (length == 0);
}

template <class MEMORY_ADDR>
bool DWARF_AddressRangeDescriptor<MEMORY_ADDR>::HasOverlap(MEMORY_ADDR _addr, MEMORY_ADDR _length) const
{
	return unisim::util::debug::dwarf::HasOverlapEx(addr, addr + length, _addr, _addr + _length);
}

template <class MEMORY_ADDR>
std::ostream& DWARF_AddressRangeDescriptor<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_ARANGE address=\"0x" << std::hex << addr << std::dec << "\" length=\"0x" << std::hex << length << std::dec << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_AddressRangeDescriptor<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr><td>0x" << std::hex << addr << std::dec << "</td><td>0x" << std::hex << length << std::dec << "</td></tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_AddressRangeDescriptor<MEMORY_ADDR>& dw_addr_range_desc)
{
	os << "      - Address: 0x" << std::hex << dw_addr_range_desc.addr << std::dec << std::endl;
	os << "      - Length: 0x" << std::hex << dw_addr_range_desc.length << std::dec;
	return os;
}

template <class MEMORY_ADDR>
DWARF_AddressRanges<MEMORY_ADDR>::DWARF_AddressRanges(DWARF_Handler<MEMORY_ADDR> *_dw_handler)
	: dw_handler(_dw_handler)
	, dw_cu(0)
	, id(0)
	, dw_ver(DW_VER_UNKNOWN)
	, unit_length(0)
	, version(0)
	, debug_info_offset(0)
	, address_size(0)
	, segment_size(0)
	, dw_addr_range_descriptors()
{
}

template <class MEMORY_ADDR>
DWARF_AddressRanges<MEMORY_ADDR>::~DWARF_AddressRanges()
{
	unsigned int num_addr_range_descriptors = dw_addr_range_descriptors.size();
	
	unsigned int i;
	
	for(i = 0; i < num_addr_range_descriptors; i++)
	{
		delete dw_addr_range_descriptors[i];
	}
}

template <class MEMORY_ADDR>
DWARF_Version DWARF_AddressRanges<MEMORY_ADDR>::GetDWARFVersion() const
{
	return dw_ver;
}

template <class MEMORY_ADDR>
endian_type DWARF_AddressRanges<MEMORY_ADDR>::GetFileEndianness() const
{
	return dw_handler->GetFileEndianness();
}

template <class MEMORY_ADDR>
uint8_t DWARF_AddressRanges<MEMORY_ADDR>::GetAddressSize() const
{
	return address_size;
}

template <class MEMORY_ADDR>
uint8_t DWARF_AddressRanges<MEMORY_ADDR>::GetSegmentSize() const
{
	return segment_size;
}

template <class MEMORY_ADDR>
const std::vector<DWARF_AddressRangeDescriptor<MEMORY_ADDR> *>& DWARF_AddressRanges<MEMORY_ADDR>::GetDescriptors() const
{
	return dw_addr_range_descriptors;
}

template <class MEMORY_ADDR>
const DWARF_CompilationUnit<MEMORY_ADDR> *DWARF_AddressRanges<MEMORY_ADDR>::GetCompilationUnit() const
{
	return dw_cu;
}

template <class MEMORY_ADDR>
bool DWARF_AddressRanges<MEMORY_ADDR>::HasOverlap(MEMORY_ADDR addr, MEMORY_ADDR length) const
{
	unsigned int num_addr_range_descriptors = dw_addr_range_descriptors.size();
	
	unsigned int i;
	
	for(i = 0; i < num_addr_range_descriptors; i++)
	{
		if(dw_addr_range_descriptors[i]->HasOverlap(addr, length)) return true;
	}
	return false;
}

template <class MEMORY_ADDR>
void DWARF_AddressRanges<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
	dw_cu = dw_handler->FindCompilationUnit(debug_info_offset);
	if(!dw_cu)
	{
		dw_handler->GetDebugWarningStream() << "While resolving [address ranges entry -> CU] reference, can't find .debug_info CU (Compilation Unit) at offset " << debug_info_offset << std::endl;
	}
}

template <class MEMORY_ADDR>
unsigned int DWARF_AddressRanges<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
int64_t DWARF_AddressRanges<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size)
{
	endian_type file_endianness = dw_handler->GetFileEndianness();
	DWARF_Format dw_fmt;
	uint32_t unit_length32;
	
	uint64_t size = 0;
	if(max_size < sizeof(unit_length32)) return -1;
	memcpy(&unit_length32, rawdata, sizeof(unit_length32));
	unit_length32 = Target2Host(file_endianness, unit_length32);
	rawdata += sizeof(unit_length32);
	max_size -= sizeof(unit_length32);
	size += sizeof(unit_length32);
	
	dw_fmt = (unit_length32 == 0xffffffffUL) ? FMT_DWARF64 : FMT_DWARF32;
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t unit_length64;
		if(max_size < sizeof(unit_length64)) return -1;
		memcpy(&unit_length64, rawdata, sizeof(unit_length64));
		unit_length64 = Target2Host(file_endianness, unit_length64);
		rawdata += sizeof(unit_length64);
		max_size -= sizeof(unit_length64);
		size += sizeof(unit_length64);
		unit_length = unit_length64;
	}
	else
	{
		unit_length = unit_length32;
	}
	
	if(max_size < unit_length) return -1;
	
	if(max_size < sizeof(version)) return -1;
	memcpy(&version, rawdata, sizeof(version));
	version = Target2Host(file_endianness, version);
	rawdata += sizeof(version);
	max_size -= sizeof(version);
	size += sizeof(version);

	switch(version)
	{
		case DW_DEBUG_ARANGES_VER2: dw_ver = DW_VER2; break;
		default: return -1;
	}
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t debug_info_offset64;
	
		if(max_size < sizeof(debug_info_offset64)) return -1;
		memcpy(&debug_info_offset64, rawdata, sizeof(debug_info_offset64));
		debug_info_offset64 = Target2Host(file_endianness, debug_info_offset64);
		rawdata += sizeof(debug_info_offset64);
		max_size -= sizeof(debug_info_offset64);
		size += sizeof(debug_info_offset64);
		debug_info_offset = debug_info_offset64;
	}
	else
	{
		uint32_t debug_info_offset32;
	
		if(max_size < sizeof(debug_info_offset32)) return -1;
		memcpy(&debug_info_offset32, rawdata, sizeof(debug_info_offset32));
		debug_info_offset32 = Target2Host(file_endianness, debug_info_offset32);
		rawdata += sizeof(debug_info_offset32);
		max_size -= sizeof(debug_info_offset32);
		size += sizeof(debug_info_offset32);
		debug_info_offset = debug_info_offset32;
	}

	if(max_size < sizeof(address_size)) return -1;
	memcpy(&address_size, rawdata, sizeof(address_size));
	address_size = Target2Host(file_endianness, address_size);
	rawdata += sizeof(address_size);
	max_size -= sizeof(address_size);
	size += sizeof(address_size);
	
	if(max_size < sizeof(segment_size)) return -1;
	memcpy(&segment_size, rawdata, sizeof(segment_size));
	segment_size = Target2Host(file_endianness, segment_size);
	rawdata += sizeof(segment_size);
	max_size -= sizeof(segment_size);
	size += sizeof(segment_size);
	
	uint64_t tuple_size = 2 * address_size;
	uint64_t header_padding = tuple_size - (size % tuple_size); // quite inefficient
	if(max_size < header_padding) return -1;
	rawdata += header_padding;
	max_size -= header_padding;
	size += header_padding;

	do
	{
		DWARF_AddressRangeDescriptor<MEMORY_ADDR> *dw_addr_range_descriptor = new DWARF_AddressRangeDescriptor<MEMORY_ADDR>(this);
		
		int64_t sz;
		if((sz = dw_addr_range_descriptor->Load(rawdata, max_size)) < 0)
		{
			delete dw_addr_range_descriptor;
			return -1;
		}
		rawdata += sz;
		max_size -= sz;
		size += sz;
		
		if(dw_addr_range_descriptor->IsNull())
		{
			delete dw_addr_range_descriptor;
			break;
		}
		
		dw_addr_range_descriptors.push_back(dw_addr_range_descriptor);
	}
	while(size < ((dw_fmt == FMT_DWARF64) ? (unit_length + sizeof(uint32_t) + sizeof(uint64_t)) : (unit_length + sizeof(uint32_t))));
	
	return size;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_AddressRanges<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_RANGES unit_length=\"" << unit_length << "\" version=\"" << version << "\" debug_info_offset=\"" << debug_info_offset << "\" address_size=\"" << (uint32_t) address_size << "\" segment_size=\"" << (uint32_t) segment_size << "\" idref=\"cu-" << dw_cu->GetId() << "\">" << std::endl;
	unsigned int num_addr_range_descriptors = dw_addr_range_descriptors.size();
	
	unsigned int i;
	
	for(i = 0; i < num_addr_range_descriptors; i++)
	{
		 dw_addr_range_descriptors[i]->to_XML(os) << std::endl;
	}
	os << "</DW_RANGES>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_AddressRanges<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr>" << std::endl;
	os << "<td>" << version << "</td><td><a href=\"../" << dw_cu->GetHREF() << "\">cu-" << dw_cu->GetId() << "</a></td>";
	os << "<td>" <<  (uint32_t) address_size << "</td><td>" << (uint32_t) segment_size << "</td>" << std::endl;
	os << "<td><table><tr><th>Address</th><th>Length</th></tr>";
	unsigned int num_addr_range_descriptors = dw_addr_range_descriptors.size();
	
	unsigned int i;
	
	for(i = 0; i < num_addr_range_descriptors; i++)
	{
		 dw_addr_range_descriptors[i]->to_HTML(os) << std::endl;
	}
	os << "</table></td>";
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_AddressRanges<MEMORY_ADDR>& dw_aranges)
{
	os << "Address Ranges:" << std::endl;
	os << " - Unit length: " << dw_aranges.unit_length << std::endl;
	os << " - Version: " << dw_aranges.version << std::endl;
	os << " - Offset in .debug_info: " << dw_aranges.debug_info_offset << std::endl;
	os << " - Address size: " << (uint32_t) dw_aranges.address_size << std::endl;
	os << " - Segment size: " << (uint32_t) dw_aranges.segment_size << std::endl;
	os << " - Address range descriptors: " << std::endl;

	unsigned int num_addr_range_descriptors = dw_aranges.dw_addr_range_descriptors.size();
	
	unsigned int i;
	
	for(i = 0; i < num_addr_range_descriptors; i++)
	{
		 os << "   - Descriptor #" << i << ":" << std::endl;
		 os << *dw_aranges.dw_addr_range_descriptors[i] << std::endl;
	}
	return os;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
