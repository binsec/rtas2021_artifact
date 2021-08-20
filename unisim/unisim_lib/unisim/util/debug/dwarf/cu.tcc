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
 * Authors: Gilles Mouchard (gilles.mouch.fr)
 */

#ifndef __UNISIM_UTIL_DEBUG_DWARF_CU_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_CU_TCC__

#include <unisim/util/debug/dwarf/cu.hh>
#include <unisim/util/debug/dwarf/ml.hh>
#include <unisim/util/debug/dwarf/version.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_CompilationUnit<MEMORY_ADDR>::DWARF_CompilationUnit(DWARF_Handler<MEMORY_ADDR> *_dw_handler)
	: dw_handler(_dw_handler)
	, debug_info_stream(_dw_handler->GetDebugInfoStream())
	, debug_warning_stream(_dw_handler->GetDebugWarningStream())
	, debug_error_stream(_dw_handler->GetDebugErrorStream())
	, debug(false)
	, dw_fmt(FMT_DWARF_UNKNOWN)
	, dw_ver(DW_VER_UNKNOWN)
	, offset(0xffffffffffffffffULL)
	, unit_length(0)
	, version(0)
	, debug_abbrev_offset(0)
	, address_size(0)
	, dw_die(0)
{
	dw_handler->GetOption(OPT_DEBUG, debug);
}

template <class MEMORY_ADDR>
DWARF_CompilationUnit<MEMORY_ADDR>::~DWARF_CompilationUnit()
{
	if(dw_die)
	{
		UnRegister(dw_die);
		delete dw_die;
	}
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_CompilationUnit<MEMORY_ADDR>::GetDIE() const
{
	return dw_die;
}

template <class MEMORY_ADDR>
DWARF_Format DWARF_CompilationUnit<MEMORY_ADDR>::GetFormat() const
{
	return dw_fmt;
}

template <class MEMORY_ADDR>
uint8_t DWARF_CompilationUnit<MEMORY_ADDR>::GetAddressSize() const
{
	return address_size;
}

template <class MEMORY_ADDR>
uint16_t DWARF_CompilationUnit<MEMORY_ADDR>::GetVersion() const
{
	return version;
}

template <class MEMORY_ADDR>
DWARF_Version DWARF_CompilationUnit<MEMORY_ADDR>::GetDWARFVersion() const
{
	return dw_ver;
}

template <class MEMORY_ADDR>
uint8_t DWARF_CompilationUnit<MEMORY_ADDR>::GetOffsetSize() const
{
	return ((dw_ver == DW_VER3) || (dw_ver == DW_VER4)) ? ((dw_fmt == FMT_DWARF64) ? 8 : 4) : address_size;
}

template <class MEMORY_ADDR>
uint64_t DWARF_CompilationUnit<MEMORY_ADDR>::GetOffset() const
{
	return offset;
}

template <class MEMORY_ADDR>
DWARF_Handler<MEMORY_ADDR> *DWARF_CompilationUnit<MEMORY_ADDR>::GetHandler() const
{
	return dw_handler;
}

template <class MEMORY_ADDR>
const char *DWARF_CompilationUnit<MEMORY_ADDR>::GetString(uint64_t debug_str_offset) const
{
	return dw_handler->GetString(debug_str_offset);
}

template <class MEMORY_ADDR>
const DWARF_Abbrev *DWARF_CompilationUnit<MEMORY_ADDR>::FindAbbrev(const DWARF_LEB128& dw_abbrev_code) const
{
	return dw_handler->FindAbbrev(debug_abbrev_offset, dw_abbrev_code);
}

template <class MEMORY_ADDR>
int64_t DWARF_CompilationUnit<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	endian_type file_endianness = dw_handler->GetFileEndianness();
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
		case DW_DEBUG_INFO_VER2: dw_ver = DW_VER2; break;
		case DW_DEBUG_INFO_VER3: dw_ver = DW_VER3; break;
		case DW_DEBUG_INFO_VER4: dw_ver = DW_VER4; break;
		default: return -1;
	}
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t debug_abbrev_offset64;
	
		if(max_size < sizeof(debug_abbrev_offset64)) return -1;
		memcpy(&debug_abbrev_offset64, rawdata, sizeof(debug_abbrev_offset64));
		debug_abbrev_offset64 = Target2Host(file_endianness, debug_abbrev_offset64);
		rawdata += sizeof(debug_abbrev_offset64);
		max_size -= sizeof(debug_abbrev_offset64);
		size += sizeof(debug_abbrev_offset64);
		debug_abbrev_offset = debug_abbrev_offset64;
	}
	else
	{
		uint32_t debug_abbrev_offset32;
	
		if(max_size < sizeof(debug_abbrev_offset32)) return -1;
		memcpy(&debug_abbrev_offset32, rawdata, sizeof(debug_abbrev_offset32));
		debug_abbrev_offset32 = Target2Host(file_endianness, debug_abbrev_offset32);
		rawdata += sizeof(debug_abbrev_offset32);
		max_size -= sizeof(debug_abbrev_offset32);
		size += sizeof(debug_abbrev_offset32);
		debug_abbrev_offset = debug_abbrev_offset32;
	}

	if(max_size < sizeof(address_size)) return -1;
	memcpy(&address_size, rawdata, sizeof(address_size));
	address_size = Target2Host(file_endianness, address_size);
	rawdata += sizeof(address_size);
	max_size -= sizeof(address_size);
	size += sizeof(address_size);
	
	dw_die = new DWARF_DIE<MEMORY_ADDR>(this);
	
	int64_t sz;
	if((sz = dw_die->Load(rawdata, max_size, offset + size)) < 0)
	{
		delete dw_die;
		dw_die = 0;
		return -1;
	}
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	if(!dw_die->IsNull())
	{
		Register(dw_die);
	}
	else
	{
		delete dw_die;
		dw_die = 0;
		return -1;
	}
	// Each Compilation Unit Header should be followed by exactly one DW_TAG_compile_unit or one DW_TAG_partial_unit,
	// and the children of the DW_TAG_compile_unit or DW_TAG_partial_unit contain Debugging Information Entries for the unit.
	// A DW_TAG_compile_unit or DW_TAG_partial_unit has no sibling entries.
	return (size != ((dw_fmt == FMT_DWARF64) ? (unit_length + sizeof(uint32_t) + sizeof(uint64_t)) : (unit_length + sizeof(uint32_t)))) ? -1 : size;
}

template <class MEMORY_ADDR>
void DWARF_CompilationUnit<MEMORY_ADDR>::Register(DWARF_DIE<MEMORY_ADDR> *_dw_die)
{
	dw_handler->Register(_dw_die);
}

template <class MEMORY_ADDR>
void DWARF_CompilationUnit<MEMORY_ADDR>::UnRegister(DWARF_DIE<MEMORY_ADDR> *_dw_die)
{
	dw_handler->UnRegister(_dw_die);
}

template <class MEMORY_ADDR>
void DWARF_CompilationUnit<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
}

template <class MEMORY_ADDR>
std::string DWARF_CompilationUnit<MEMORY_ADDR>::GetHREF() const
{
	std::stringstream sstr;
	sstr << "debug_info/cus/" << (id / cus_per_file) << ".html#cu-" << id;
	return sstr.str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_CompilationUnit<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_CompilationUnit<MEMORY_ADDR>::to_XML(std::ostream& os)
{
	os << "<DW_CU id=\"cu-" << id
	   << "\" unit_length=\"" << unit_length
	   << "\" version=\"" << version
	   << "\" debug_abbrev_offset=\"" << debug_abbrev_offset
	   << "\" address_size=\"" << (uint32_t) address_size
	   << "\">" << std::endl;
	dw_die->to_XML(os) << std::endl;
	os << "</DW_CU>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_CompilationUnit<MEMORY_ADDR>::to_HTML(std::ostream& os)
{
	os << "<tr id=\"cu-" << id << "\">" << std::endl;
	os << "<td>" << id << "</td><td>" << offset << "</td><td>" << (uint32_t) version << "</td><td>" << (uint32_t) address_size << "</td>" << std::endl;
	os << "<td>";
	os << "<a href=\"../../" << dw_die->GetHREF() << "\">die #" << dw_die->GetId() << "</a>";
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;

	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CompilationUnit<MEMORY_ADDR>& dw_cu)
{
	os << "Compilation Unit Header:" << std::endl
	   << " - Offset: " << dw_cu.offset << std::endl
	   << " - Unit Length: " << dw_cu.unit_length << std::endl
	   << " - DWARF Version: " << dw_cu.version << std::endl
	   << " - Offset in .debug_abbrev: " << dw_cu.debug_abbrev_offset << std::endl
	   << " - Address Size: " << (uint32_t) dw_cu.address_size << std::endl
	   << " - Debug Info Entries:" << std::endl;
	os << *dw_cu.dw_die << std::endl;
	return os;
}

template <class MEMORY_ADDR>
void DWARF_CompilationUnit<MEMORY_ADDR>::BuildStatementMatrix(std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>& stmt_matrix)
{
	dw_die->BuildStatementMatrix(stmt_matrix);
}

template <class MEMORY_ADDR>
bool DWARF_CompilationUnit<MEMORY_ADDR>::HasOverlap(MEMORY_ADDR addr, MEMORY_ADDR length) const
{
	return dw_die->HasOverlap(addr, length);
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_CompilationUnit<MEMORY_ADDR>::FindDIEByAddrRange(unsigned int dw_tag, MEMORY_ADDR addr, MEMORY_ADDR length) const
{
	if(dw_die)
	{
		const DWARF_DIE<MEMORY_ADDR> *dw_found_die = dw_die->FindDIEByAddrRange(dw_tag, addr, length);
		if(debug && dw_found_die)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", found DIE #" << dw_found_die->GetId() << " for address range 0x" << std::hex << addr << "-0x" << (addr + length) << std::dec << std::endl;
		}
		return dw_found_die;
	}
	return 0;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_CompilationUnit<MEMORY_ADDR>::FindDIEByName(unsigned int dw_tag, const char *name, bool external) const
{
	return dw_die->FindDIEByName(dw_tag, name, external);
}

template <class MEMORY_ADDR>
bool DWARF_CompilationUnit<MEMORY_ADDR>::GetDefaultBaseAddress(MEMORY_ADDR& base_addr) const
{
	return dw_die->GetDefaultBaseAddress(base_addr);
}

template <class MEMORY_ADDR>
bool DWARF_CompilationUnit<MEMORY_ADDR>::GetFrameBase(unsigned int prc_num, MEMORY_ADDR pc, MEMORY_ADDR& frame_base) const
{
	const DWARF_DIE<MEMORY_ADDR> *dw_die_code_portion = FindDIEByAddrRange(0 /* any tag */, pc, 1);
	if(!dw_die_code_portion)
	{
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", can't find any DIE matching PC=0x" << std::hex << pc << std::dec << std::endl;
		}
		return 0;
	}
	return dw_die_code_portion->GetFrameBase(prc_num, pc, frame_base);
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_CompilationUnit<MEMORY_ADDR>::FindDataObject(const char *name, MEMORY_ADDR pc) const
{
	const DWARF_DIE<MEMORY_ADDR> *dw_die_code_portion = FindDIEByAddrRange(0 /* any tag */, pc, 1);
	if(!dw_die_code_portion)
	{
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", can't find any DIE matching PC=0x" << std::hex << pc << std::dec << std::endl;
		}
		return 0;
	}
	
	do
	{
		const DWARF_DIE<MEMORY_ADDR> *dw_die_data_object = dw_die_code_portion->FindDataObject(name);
	
		if(dw_die_data_object)
		{
			return dw_die_data_object;
		}
		
		const DWARF_DIE<MEMORY_ADDR> *dw_die_code_portion_parent = dw_die_code_portion->GetParentDIE();
		
		if(dw_die_code_portion_parent && debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", parent of DIE #" << dw_die_code_portion->GetId() << " is DIE #" << dw_die_code_portion_parent->GetId() << std::endl;
		}
		
		dw_die_code_portion = dw_die_code_portion_parent;
	}
	while(dw_die_code_portion);
	
	if(debug)
	{
		debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", can't find any DIE matching data object name \"" << name << "\" and PC=0x" << std::hex << pc << std::dec << std::endl;
	}
	return 0;
}

template <class MEMORY_ADDR>
void DWARF_CompilationUnit<MEMORY_ADDR>::EnumerateDataObjectNames(std::set<std::string>& name_set, MEMORY_ADDR pc, bool local_only) const
{
	const DWARF_DIE<MEMORY_ADDR> *dw_die_code_portion = FindDIEByAddrRange(0 /* any tag */, pc, 1);
	if(!dw_die_code_portion)
	{
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", can't find any DIE matching PC=0x" << std::hex << pc << std::dec << std::endl;
		}
		return;
	}
	
	do
	{
		dw_die_code_portion->EnumerateDataObjectNames(name_set);
		
		const DWARF_DIE<MEMORY_ADDR> *dw_die_code_portion_parent = dw_die_code_portion->GetParentDIE();
		
		if(dw_die_code_portion_parent && debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", parent of DIE #" << dw_die_code_portion->GetId() << " is DIE #" << dw_die_code_portion_parent->GetId() << std::endl;
		}
		
		if(local_only && (dw_die_code_portion->GetTag() == DW_TAG_subprogram)) break;
		
		dw_die_code_portion = dw_die_code_portion_parent;
	}
	while(dw_die_code_portion);
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_CompilationUnit<MEMORY_ADDR>::FindSubProgram(const char *name) const
{
	if(!dw_die) return 0;
	
	return dw_die->FindSubProgram(name);
}

template <class MEMORY_ADDR>
const char *DWARF_CompilationUnit<MEMORY_ADDR>::GetName() const
{
	const DWARF_String<MEMORY_ADDR> *dw_at_name = 0;
	if(!dw_die || !dw_die->GetAttributeValue(DW_AT_name, dw_at_name)) return 0;
	return dw_at_name->GetValue();
}

template <class MEMORY_ADDR>
uint16_t DWARF_CompilationUnit<MEMORY_ADDR>::GetLanguage() const
{
	const DWARF_Constant<MEMORY_ADDR> *dw_at_language = 0;
	if(!dw_die || !dw_die->GetAttributeValue(DW_AT_language, dw_at_language)) return 0;
	return dw_at_language->to_uint();
}

template <class MEMORY_ADDR>
const char *DWARF_CompilationUnit<MEMORY_ADDR>::GetProducer() const
{
	const DWARF_String<MEMORY_ADDR> *dw_at_producer = 0;
	if(!dw_die || !dw_die->GetAttributeValue(DW_AT_producer, dw_at_producer)) return 0;
	return dw_at_producer->GetValue();
}

template <class MEMORY_ADDR>
uint8_t DWARF_CompilationUnit<MEMORY_ADDR>::GetDefaultOrdering() const
{
	uint16_t language = GetLanguage();
	switch(language)
	{
		case DW_LANG_C89:
		case DW_LANG_C:
		case DW_LANG_C_plus_plus:
		case DW_LANG_Java:
		case DW_LANG_C99:
		case DW_LANG_ObjC:
		case DW_LANG_ObjC_plus_plus:
		case DW_LANG_UPC:
		case DW_LANG_Mips_Assembler:
		case DW_LANG_Upc:
		case DW_LANG_Ada83:
		case DW_LANG_Modula2:
		case DW_LANG_Ada95:
		case DW_LANG_Pascal83:
		case DW_LANG_Cobol74:
		case DW_LANG_Cobol85:
		case DW_LANG_PLI:
			return DW_ORD_row_major;
		case DW_LANG_Fortran77:
		case DW_LANG_Fortran90:
		case DW_LANG_Fortran95:
		case DW_LANG_D: // not sure but most probably because 2D-array are written as A[col][row] in D
			return DW_ORD_col_major;
	}
	return DW_ORD_row_major;
}

template <class MEMORY_ADDR>
bool DWARF_CompilationUnit<MEMORY_ADDR>::GetDefaultLowerBound(int64_t& lower_bound) const
{
	// If the lower bound value is missing, the value is assumed to be a language-dependent default
	// constant. The default lower bound is 0 for C, C++, D, Java, Objective C, Objective C++, Python,
	// and UPC. The default lower bound is 1 for Ada, COBOL, Fortran, Modula-2, Pascal and PL/I.

	uint16_t dw_at_language = GetLanguage();
	switch(dw_at_language)
	{
		case DW_LANG_C89:
		case DW_LANG_C:
		case DW_LANG_C_plus_plus:
		case DW_LANG_Java:
		case DW_LANG_C99:
		case DW_LANG_ObjC:
		case DW_LANG_ObjC_plus_plus:
		case DW_LANG_UPC:
		case DW_LANG_Upc:
		case DW_LANG_D:
		case DW_LANG_python:
		case DW_LANG_Mips_Assembler:
			lower_bound = 0;
			return true;
		case DW_LANG_Ada83:
		case DW_LANG_Cobol74:
		case DW_LANG_Cobol85:
		case DW_LANG_Fortran77:
		case DW_LANG_Fortran90:
		case DW_LANG_Pascal83:
		case DW_LANG_Modula2:
		case DW_LANG_Ada95:
		case DW_LANG_Fortran95:
		case DW_LANG_PLI:
			lower_bound = 1;
			return true;
	}
	return false;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
