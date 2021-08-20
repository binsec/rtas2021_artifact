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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_PUB_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_PUB_TCC__

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_Pub<MEMORY_ADDR>::DWARF_Pub(const DWARF_Pubs<MEMORY_ADDR> *_dw_pubs)
	: dw_pubs(_dw_pubs)
	, dw_die(0)
	, id(0)
	, debug_info_offset(0xffffffffffffffffULL)
	, name(0)
{
}

template <class MEMORY_ADDR>
DWARF_Pub<MEMORY_ADDR>::~DWARF_Pub()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_Pub<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size)
{
	DWARF_Format dw_fmt = dw_pubs->GetFormat();
	endian_type file_endianness = dw_pubs->GetHandler()->GetFileEndianness();
	int64_t size = 0;
	
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

	if(debug_info_offset == 0) return size;
	
	name = (const char *) rawdata;
	uint64_t sz = strlen(name) + 1;
	if(max_size < sz) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;

	return size;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_Pub<MEMORY_ADDR>::GetDIE() const
{
	return dw_die;
}

template <class MEMORY_ADDR>
bool DWARF_Pub<MEMORY_ADDR>::IsNull() const
{
	return debug_info_offset == 0;
}

template <class MEMORY_ADDR>
const char *DWARF_Pub<MEMORY_ADDR>::GetName() const
{
	return name;
}

template <class MEMORY_ADDR>
void DWARF_Pub<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
	uint64_t cu_debug_info_offset = dw_pubs->GetDebugInfoOffset();
	dw_die = dw_handler->FindDIE(cu_debug_info_offset + debug_info_offset);
	if(!dw_die)
	{
		dw_handler->GetDebugWarningStream() << "While resolving [pub entry -> DIE] reference, can't find DIE (Debugging Information Entry) in .debug_info at offset " << (cu_debug_info_offset + debug_info_offset) << std::endl;
	}
}

template <class MEMORY_ADDR>
unsigned int DWARF_Pub<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
void DWARF_Pub<MEMORY_ADDR>::EnumerateDataObjectNames(std::set<std::string>& name_set) const
{
	if(dw_die)
	{
		switch(dw_die->GetTag())
		{
			case DW_TAG_variable:
			case DW_TAG_formal_parameter:
			case DW_TAG_constant:
				{
					const char *name = dw_die->GetName();
					if(name)
					{
						name_set.insert(std::string(name));
					}
				}
				break;
		}
	}
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Pub<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	uint64_t cu_debug_info_offset = dw_pubs->GetDebugInfoOffset();
	os << "<DW_PUB id=\"pub-" << dw_pubs->GetId() << "-" << id << "\" abs_debug_info_offset=\"" << (cu_debug_info_offset + debug_info_offset) << "\" debug_info_offset=\"" << debug_info_offset << "\" name=\"" << name << "\" idref=\"die-" << dw_die->GetId() << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Pub<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr><td>" << name << "</td><td><a href=\"../" << dw_die->GetHREF() << "\">die-" << dw_die->GetId() << "</a></td></tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Pub<MEMORY_ADDR>& dw_pub)
{
	os << "   - Offset in .debug_info (rel. to CU): " << dw_pub.debug_info_offset << std::endl;
	os << "   - Name: \"" << dw_pub.name << "\"";
	return os;
}

template <class MEMORY_ADDR>
DWARF_Pubs<MEMORY_ADDR>::DWARF_Pubs(DWARF_Handler<MEMORY_ADDR> *_dw_handler)
	: dw_handler(_dw_handler)
	, dw_fmt(FMT_DWARF32)
	, dw_ver(DW_VER_UNKNOWN)
	, offset(0xffffffffffffffffULL)
	, id(0)
	, dw_cu(0)
	, unit_length(0)
	, version(0)
	, debug_info_offset(0xffffffffffffffffULL)
	, debug_info_length(0)
	, dw_pubs()
{
}

template <class MEMORY_ADDR>
DWARF_Pubs<MEMORY_ADDR>::~DWARF_Pubs()
{
	typename std::map<std::string, DWARF_Pub<MEMORY_ADDR> *>::iterator dw_pub_iter;
	
	for(dw_pub_iter = dw_pubs.begin(); dw_pub_iter != dw_pubs.end(); dw_pub_iter++)
	{
		DWARF_Pub<MEMORY_ADDR> *dw_pub = (*dw_pub_iter).second;
		delete dw_pub;
	}
}

template <class MEMORY_ADDR>
uint64_t DWARF_Pubs<MEMORY_ADDR>::GetDebugInfoOffset() const
{
	return debug_info_offset;
}

template <class MEMORY_ADDR>
DWARF_Handler<MEMORY_ADDR> *DWARF_Pubs<MEMORY_ADDR>::GetHandler() const
{
	return dw_handler;
}

template <class MEMORY_ADDR>
DWARF_Format DWARF_Pubs<MEMORY_ADDR>::GetFormat() const
{
	return dw_fmt;
}

template <class MEMORY_ADDR>
DWARF_Version DWARF_Pubs<MEMORY_ADDR>::GetDWARFVersion() const
{
	return dw_ver;
}

template <class MEMORY_ADDR>
const DWARF_Pub<MEMORY_ADDR> *DWARF_Pubs<MEMORY_ADDR>::FindPub(const char *name) const
{
	typename std::map<std::string, DWARF_Pub<MEMORY_ADDR> *>::const_iterator dw_pub_iter = dw_pubs.find(name);
	return (dw_pub_iter != dw_pubs.end()) ? (*dw_pub_iter).second : 0;
}

template <class MEMORY_ADDR>
void DWARF_Pubs<MEMORY_ADDR>::EnumerateDataObjectNames(std::set<std::string>& name_set) const
{
	typename std::map<std::string, DWARF_Pub<MEMORY_ADDR> *>::const_iterator dw_pub_iter;
	
	for(dw_pub_iter = dw_pubs.begin(); dw_pub_iter != dw_pubs.end(); dw_pub_iter++)
	{
		const DWARF_Pub<MEMORY_ADDR> *dw_pub = (*dw_pub_iter).second;
		
		dw_pub->EnumerateDataObjectNames(name_set);
	}
}

template <class MEMORY_ADDR>
int64_t DWARF_Pubs<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size)
{
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
		case DW_DEBUG_PUBS_VER2: dw_ver = DW_VER2; break;
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

		uint64_t debug_info_length64;
	
		if(max_size < sizeof(debug_info_length64)) return -1;
		memcpy(&debug_info_length64, rawdata, sizeof(debug_info_length64));
		debug_info_length64 = Target2Host(file_endianness, debug_info_length64);
		rawdata += sizeof(debug_info_length64);
		max_size -= sizeof(debug_info_length64);
		size += sizeof(debug_info_length64);
		debug_info_length = debug_info_length64;
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

		uint32_t debug_info_length32;
	
		if(max_size < sizeof(debug_info_length32)) return -1;
		memcpy(&debug_info_length32, rawdata, sizeof(debug_info_length32));
		debug_info_length32 = Target2Host(file_endianness, debug_info_length32);
		rawdata += sizeof(debug_info_length32);
		max_size -= sizeof(debug_info_length32);
		size += sizeof(debug_info_length32);
		debug_info_length = debug_info_length32;
	}

	do
	{
		DWARF_Pub<MEMORY_ADDR> *dw_pub = new DWARF_Pub<MEMORY_ADDR>(this);
		
		int64_t sz;
		if((sz = dw_pub->Load(rawdata, max_size)) < 0)
		{
			delete dw_pub;
			return -1;
		}
		rawdata += sz;
		max_size -= sz;
		size += sz;
		
		if(dw_pub->IsNull())
		{
			delete dw_pub;
			break;
		}
		
		dw_pubs.insert(std::pair<std::string, DWARF_Pub<MEMORY_ADDR> *>(dw_pub->GetName(), dw_pub));
	}
	while(size < ((dw_fmt == FMT_DWARF64) ? (unit_length + sizeof(uint32_t) + sizeof(uint64_t)) : (unit_length + sizeof(uint32_t))));
	
	size = (dw_fmt == FMT_DWARF64) ? (unit_length + sizeof(uint32_t) + sizeof(uint64_t)) : (unit_length + sizeof(uint32_t));
	
	return size;
}

template <class MEMORY_ADDR>
void DWARF_Pubs<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
	dw_cu = dw_handler->FindCompilationUnit(debug_info_offset);
	if(!dw_cu)
	{
		dw_handler->GetDebugWarningStream() << "While resolving [pubs -> CU] reference, can't find CU (Compilation Unit) in .debug_info at offset " << debug_info_offset << std::endl;
	}
	
	unsigned int i;
	typename std::map<std::string, DWARF_Pub<MEMORY_ADDR> *>::iterator dw_pub_iter;
	
	for(dw_pub_iter = dw_pubs.begin(), i = 0; dw_pub_iter != dw_pubs.end(); dw_pub_iter++, i++)
	{
		DWARF_Pub<MEMORY_ADDR> *dw_pub = (*dw_pub_iter).second;
		dw_pub->Fix(dw_handler, i);
	}
}

template <class MEMORY_ADDR>
unsigned int DWARF_Pubs<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Pubs<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_PUBS id=\"pubs-" << id << "\" unit_length=\"" << unit_length << "\" version=\"" << version << "\" debug_info_offset=\"" << debug_info_offset << "\" debug_info_length=\"" << debug_info_length << "\" idref=\"cu-" << dw_cu->GetId() << "\">" << std::endl;
	
	typename std::map<std::string, DWARF_Pub<MEMORY_ADDR> *>::const_iterator dw_pub_iter;
	
	for(dw_pub_iter = dw_pubs.begin(); dw_pub_iter != dw_pubs.end(); dw_pub_iter++)
	{
		DWARF_Pub<MEMORY_ADDR> *dw_pub = (*dw_pub_iter).second;
		dw_pub->to_XML(os);
		os << std::endl;
	}
	os << "</DW_PUBS>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Pubs<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr><td>" << version << "</td><td><a href=\"../" << dw_cu->GetHREF() << "\">cu-" << dw_cu->GetId() << "</a></td><td><table>";
	
	typename std::map<std::string, DWARF_Pub<MEMORY_ADDR> *>::const_iterator dw_pub_iter;
	
	for(dw_pub_iter = dw_pubs.begin(); dw_pub_iter != dw_pubs.end(); dw_pub_iter++)
	{
		DWARF_Pub<MEMORY_ADDR> *dw_pub = (*dw_pub_iter).second;
		dw_pub->to_HTML(os);
		os << std::endl;
	}
	os << "</table></td></tr>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Pubs<MEMORY_ADDR>& dw_pubs)
{
	os << "Pubs:" << std::endl;
	os << " - Unit length: " << dw_pubs.unit_length << std::endl;
	os << " - Version: " << dw_pubs.version << std::endl;
	os << " - Offset in .debug_info: " << dw_pubs.debug_info_offset << std::endl;
	os << " - Length in .debug_info: " << dw_pubs.debug_info_length << std::endl;

	typename std::map<std::string, DWARF_Pub<MEMORY_ADDR> *>::const_iterator dw_pub_iter;
	
	for(dw_pub_iter = dw_pubs.dw_pubs.begin(); dw_pub_iter != dw_pubs.dw_pubs.end(); dw_pub_iter++)
	{
		DWARF_Pub<MEMORY_ADDR> *dw_pub = (*dw_pub_iter).second;
		os << *dw_pub << std::endl;
	}
	return os;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_PUB_TCC__
