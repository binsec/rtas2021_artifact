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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_ADDR_RANGE_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_ADDR_RANGE_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/dwarf/version.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_AddressRangeDescriptor<MEMORY_ADDR>& dw_addr_range_desc);

template <class MEMORY_ADDR>
class DWARF_AddressRangeDescriptor
{
public:
	DWARF_AddressRangeDescriptor(const DWARF_AddressRanges<MEMORY_ADDR> *dw_aranges);
	~DWARF_AddressRangeDescriptor();
	int64_t Load(const uint8_t *rawdata, uint64_t max_size);
	MEMORY_ADDR GetAddress() const;
	MEMORY_ADDR GetLength() const;
	bool IsNull() const;
	bool HasOverlap(MEMORY_ADDR addr, MEMORY_ADDR length) const;
	std::ostream& to_XML(std::ostream& os) const;
	std::ostream& to_HTML(std::ostream& os) const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_AddressRangeDescriptor<MEMORY_ADDR>& dw_addr_range_desc);
private:
	const DWARF_AddressRanges<MEMORY_ADDR> *dw_aranges;
	MEMORY_ADDR addr;
	MEMORY_ADDR length;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_AddressRanges<MEMORY_ADDR>& dw_aranges);

template <class MEMORY_ADDR>
class DWARF_AddressRanges
{
public:
	DWARF_AddressRanges(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	~DWARF_AddressRanges();
	endian_type GetFileEndianness() const;
	uint8_t GetSegmentSize() const;
	uint8_t GetAddressSize() const;
	DWARF_Version GetDWARFVersion() const;
	const std::vector<DWARF_AddressRangeDescriptor<MEMORY_ADDR> *>& GetDescriptors() const;
	const DWARF_CompilationUnit<MEMORY_ADDR> *GetCompilationUnit() const;
	bool HasOverlap(MEMORY_ADDR addr, MEMORY_ADDR length) const;
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int id);
	unsigned int GetId() const;
	int64_t Load(const uint8_t *rawdata, uint64_t max_size);
	std::ostream& to_XML(std::ostream& os) const;
	std::ostream& to_HTML(std::ostream& os) const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_AddressRanges<MEMORY_ADDR>& dw_aranges);
private:
	DWARF_Handler<MEMORY_ADDR> *dw_handler;
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	unsigned int id;
	DWARF_Version dw_ver;
	
	uint64_t unit_length;        // The length of the entries for that set, not including the length field itself (see Section 7.2.2).
	
	uint16_t version;            // A version number. This number is specific to the address lookup table and
	                             // is independent of the DWARF version number.
						
	uint64_t debug_info_offset;  // The offset from the beginning of the .debug_info section of the compilation unit header
	                             // referenced by the set.
						
	uint8_t address_size;        // The size of an address in bytes on the target architecture. For segmented addressing, this is
	                             // the size of the offset portion of the address.

	uint8_t segment_size;        // The size of a segment descriptor in bytes on the target architecture. If the target system uses a
	                             // flat address space, this value is 0.
	
	std::vector<DWARF_AddressRangeDescriptor<MEMORY_ADDR> *> dw_addr_range_descriptors;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
