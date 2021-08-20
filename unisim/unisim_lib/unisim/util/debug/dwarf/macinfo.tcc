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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_MACINFO_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_MACINFO_TCC__

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_MacInfoListEntry<MEMORY_ADDR>::DWARF_MacInfoListEntry(uint8_t _dw_mac_info_type)
	: next(0)
	, offset(0xffffffffffffffffULL)
	, dw_mac_info_type(_dw_mac_info_type)
{
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntry<MEMORY_ADDR>::~DWARF_MacInfoListEntry()
{
}

template <class MEMORY_ADDR>
void DWARF_MacInfoListEntry<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
}

template <class MEMORY_ADDR>
uint8_t DWARF_MacInfoListEntry<MEMORY_ADDR>::GetType() const
{
	return dw_mac_info_type;
}

template <class MEMORY_ADDR>
uint64_t DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() const
{
	return offset;
}

template <class MEMORY_ADDR>
std::string DWARF_MacInfoListEntry<MEMORY_ADDR>::GetHREF() const
{
	std::stringstream sstr;
	sstr << "debug_macinfo/" << (id / debug_macinfo_per_file) << ".html#mac-" << id;
	return sstr.str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
const DWARF_MacInfoListEntry<MEMORY_ADDR> *DWARF_MacInfoListEntry<MEMORY_ADDR>::GetNext() const
{
	return next;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_MacInfoListEntry<MEMORY_ADDR>& dw_macinfo_list_entry)
{
	return os << dw_macinfo_list_entry.to_string();
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryDefine<MEMORY_ADDR>::DWARF_MacInfoListEntryDefine()
	: DWARF_MacInfoListEntry<MEMORY_ADDR>(DW_MACINFO_define)
	, lineno()
	, preprocessor_symbol_name(0)
{
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryDefine<MEMORY_ADDR>::~DWARF_MacInfoListEntryDefine()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_MacInfoListEntryDefine<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	DWARF_MacInfoListEntry<MEMORY_ADDR>::offset = _offset;
	int64_t size = 0;
	
	uint8_t type;
	if(max_size < sizeof(type)) return -1;
	memcpy(&type, rawdata, sizeof(type));
	rawdata += sizeof(type);
	size += sizeof(type);
	max_size -= sizeof(type);
	
	if(DWARF_MacInfoListEntry<MEMORY_ADDR>::dw_mac_info_type != type) return -1;
	
	int64_t sz;
	if((sz = lineno.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	if(!max_size) return -1;
	preprocessor_symbol_name = (const char *) rawdata;
	sz = strlen(preprocessor_symbol_name) + 1;
	size += sz;
	rawdata += sz;
	max_size -= sz;
	
	return size;
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_MacInfoListEntryDefine<MEMORY_ADDR>::GetLineNo() const
{
	return lineno;
}

template <class MEMORY_ADDR>
const char *DWARF_MacInfoListEntryDefine<MEMORY_ADDR>::GetPreprocessorSymbolName() const
{
	return preprocessor_symbol_name;
}

template <class MEMORY_ADDR>
std::string DWARF_MacInfoListEntryDefine<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << "DW_MACINFO_define " << lineno.to_string(false) << ", \"" << preprocessor_symbol_name << "\"";
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryDefine<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_MACINFO_define id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\" lineno=\"" << lineno.to_string(false) << "\" preprocessor_symbol_name=\"";
	c_string_to_XML(os, preprocessor_symbol_name);
	os << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryDefine<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\">" << std::endl;
	os << "<td>DW_MACINFO_define</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "</td>" << std::endl;
	os << "<td>";
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *next = DWARF_MacInfoListEntry<MEMORY_ADDR>::GetNext();
	if(next)
	{
		os << "<a href=\"../" << next->GetHREF() << "\">mac-" << next->GetId() << "</a>" << std::endl;
	}
	else
	{
		os << "-";
	}
	os << "</td>" << std::endl;
	
	os << "<td>" << std::endl;
	os << "<table class=\"macinfo\">" << std::endl;
	os << "<tr>" << std::endl;
	os << "<th>Line number</th><th>Preprocessor symbol name</th>" << std::endl;
	os << "</tr>" << std::endl;
	os << "<tr>" << std::endl;
	os << "<td>" << lineno.to_string(false) << "</td><td>&quot;";
	c_string_to_HTML(os, preprocessor_symbol_name);
	os << "&quot;</td>" << std::endl;
	os << "</tr>" << std::endl;
	os << "</table>" << std::endl;
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryUndef<MEMORY_ADDR>::DWARF_MacInfoListEntryUndef()
	: DWARF_MacInfoListEntry<MEMORY_ADDR>(DW_MACINFO_undef)
	, lineno()
	, preprocessor_symbol_name(0)
{
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryUndef<MEMORY_ADDR>::~DWARF_MacInfoListEntryUndef()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_MacInfoListEntryUndef<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	DWARF_MacInfoListEntry<MEMORY_ADDR>::offset = _offset;
	int64_t size = 0;
	
	uint8_t type;
	if(max_size < sizeof(type)) return -1;
	memcpy(&type, rawdata, sizeof(type));
	rawdata += sizeof(type);
	size += sizeof(type);
	max_size -= sizeof(type);
	
	if(DWARF_MacInfoListEntry<MEMORY_ADDR>::dw_mac_info_type != type) return -1;

	int64_t sz;
	if((sz = lineno.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	if(!max_size) return -1;
	preprocessor_symbol_name = (const char *) rawdata;
	sz = strlen(preprocessor_symbol_name) + 1;
	size += sz;
	rawdata += sz;
	max_size -= sz;
	
	return size;
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_MacInfoListEntryUndef<MEMORY_ADDR>::GetLineNo() const
{
	return lineno;
}

template <class MEMORY_ADDR>
const char *DWARF_MacInfoListEntryUndef<MEMORY_ADDR>::GetPreprocessorSymbolName() const
{
	return preprocessor_symbol_name;
}

template <class MEMORY_ADDR>
std::string DWARF_MacInfoListEntryUndef<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << "DW_MACINFO_undef " << lineno.to_string(false) << ", \"" << preprocessor_symbol_name << "\"";
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryUndef<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_MACINFO_undef id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\" lineno=\"" << lineno.to_string(false) << "\" preprocessor_symbol_name=\"";
	c_string_to_XML(os, preprocessor_symbol_name);
	os << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryUndef<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\">" << std::endl;
	os << "<td>DW_MACINFO_Undef</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "</td>" << std::endl;
	os << "<td>";
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *next = DWARF_MacInfoListEntry<MEMORY_ADDR>::GetNext();
	if(next)
	{
		os << "<a href=\"../" << next->GetHREF() << "\">mac-" << next->GetId() << "</a>" << std::endl;
	}
	else
	{
		os << "-";
	}
	os << "</td>" << std::endl;
	os << "<td>" << std::endl;
	os << "<table class=\"macinfo\">" << std::endl;
	os << "<tr>" << std::endl;
	os << "<th>LineNo</th><th>Preprocessor symbol name</th>" << std::endl;
	os << "</tr>" << std::endl;
	os << "<tr>" << std::endl;
	os << "<td>" << lineno.to_string(false) << "</td><td>&quot;";
	c_string_to_HTML(os, preprocessor_symbol_name);
	os << "&quot;</td>" << std::endl;
	os << "</tr>" << std::endl;
	os << "</table>" << std::endl;
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>::DWARF_MacInfoListEntryStartFile()
	: DWARF_MacInfoListEntry<MEMORY_ADDR>(DW_MACINFO_start_file)
	, lineno()
	, file_idx()
{
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>::~DWARF_MacInfoListEntryStartFile()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	DWARF_MacInfoListEntry<MEMORY_ADDR>::offset = _offset;
	int64_t size = 0;
	
	uint8_t type;
	if(max_size < sizeof(type)) return -1;
	memcpy(&type, rawdata, sizeof(type));
	rawdata += sizeof(type);
	size += sizeof(type);
	max_size -= sizeof(type);
	
	if(DWARF_MacInfoListEntry<MEMORY_ADDR>::dw_mac_info_type != type) return -1;

	int64_t sz;
	if((sz = lineno.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	if((sz = file_idx.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	return size;
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>::GetLineNo() const
{
	return lineno;
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>::GetFileIndex() const
{
	return file_idx;
}

template <class MEMORY_ADDR>
std::string DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << "DW_MACINFO_start_file " << lineno.to_string(false) << ", " << file_idx.to_string(false);
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_MACINFO_start_file id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\" lineno=\"" << lineno.to_string(false) << "\" file_idx=\"" << file_idx.to_string(false) << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\">" << std::endl;
	os << "<td>DW_MACINFO_start_file</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "</td>" << std::endl;
	os << "<td>";
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *next = DWARF_MacInfoListEntry<MEMORY_ADDR>::GetNext();
	if(next)
	{
		os << "<a href=\"../" << next->GetHREF() << "\">mac-" << next->GetId() << "</a>" << std::endl;
	}
	else
	{
		os << "-";
	}
	os << "</td>" << std::endl;
	os << "<td>" << std::endl;
	os << "<table class=\"macinfo\">" << std::endl;
	os << "<tr>" << std::endl;
	os << "<th>LineNo</th><th>File index</th>" << std::endl;
	os << "</tr>" << std::endl;
	os << "<tr>" << std::endl;
	os << "<td>" << lineno.to_string(false) << "</td><td>" << file_idx.to_string(false) << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	os << "</table>" << std::endl;
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryEndFile<MEMORY_ADDR>::DWARF_MacInfoListEntryEndFile()
	: DWARF_MacInfoListEntry<MEMORY_ADDR>(DW_MACINFO_end_file)
{
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryEndFile<MEMORY_ADDR>::~DWARF_MacInfoListEntryEndFile()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_MacInfoListEntryEndFile<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	DWARF_MacInfoListEntry<MEMORY_ADDR>::offset = _offset;
	int64_t size = 0;
	
	uint8_t type;
	if(max_size < sizeof(type)) return -1;
	memcpy(&type, rawdata, sizeof(type));
	rawdata += sizeof(type);
	size += sizeof(type);
	max_size -= sizeof(type);
	
	if(DWARF_MacInfoListEntry<MEMORY_ADDR>::dw_mac_info_type != type) return -1;
	
	return size;
}

template <class MEMORY_ADDR>
std::string DWARF_MacInfoListEntryEndFile<MEMORY_ADDR>::to_string() const
{
	return std::string("DW_MACINFO_end_file");
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryEndFile<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_MACINFO_end_file id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryEndFile<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\">" << std::endl;
	os << "<td>DW_MACINFO_end_file</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "</td>" << std::endl;
	os << "<td>";
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *next = DWARF_MacInfoListEntry<MEMORY_ADDR>::GetNext();
	if(next)
	{
		os << "<a href=\"../" << next->GetHREF() << "\">mac-" << next->GetId() << "</a>" << std::endl;
	}
	else
	{
		os << "-";
	}
	os << "</td>" << std::endl;
	os << "<td></td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>::DWARF_MacInfoListEntryVendorExtension()
	: DWARF_MacInfoListEntry<MEMORY_ADDR>(DW_MACINFO_define)
	, vendor_ext_constant()
	, vendor_ext_string(0)
{
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>::~DWARF_MacInfoListEntryVendorExtension()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	DWARF_MacInfoListEntry<MEMORY_ADDR>::offset = _offset;
	int64_t size = 0;
	
	uint8_t type;
	if(max_size < sizeof(type)) return -1;
	memcpy(&type, rawdata, sizeof(type));
	rawdata += sizeof(type);
	size += sizeof(type);
	max_size -= sizeof(type);
	
	if(DWARF_MacInfoListEntry<MEMORY_ADDR>::dw_mac_info_type != type) return -1;
	
	int64_t sz;
	if((sz = vendor_ext_constant.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	if(!max_size) return -1;
	vendor_ext_string = (const char *) rawdata;
	sz = strlen(vendor_ext_string) + 1;
	size += sz;
	rawdata += sz;
	max_size -= sz;
	
	return size;
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>::GetConstant() const
{
	return vendor_ext_constant;
}

template <class MEMORY_ADDR>
const char *DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>::GetString() const
{
	return vendor_ext_string;
}

template <class MEMORY_ADDR>
std::string DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << "DW_MACINFO_vendor_ext " << vendor_ext_constant.to_string(false) << ", \"" << vendor_ext_string << "\"";
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_MACINFO_vendor_ext id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\" vendor_ext_constant=\"" << vendor_ext_constant.to_string(false) << "\" vendor_ext_string=\"";
	c_string_to_XML(os, vendor_ext_string);
	os << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\">" << std::endl;
	os << "<td>DW_MACINFO_vendor_ext</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "</td>" << std::endl;
	os << "<td>";
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *next = DWARF_MacInfoListEntry<MEMORY_ADDR>::GetNext();
	if(next)
	{
		os << "<a href=\"../" << next->GetHREF() << "\">mac-" << next->GetId() << "</a>" << std::endl;
	}
	else
	{
		os << "-";
	}
	os << "</td>" << std::endl;
	os << "<td>" << std::endl;
	os << "<table class=\"macinfo\">" << std::endl;
	os << "<tr>" << std::endl;
	os << "<th>Vendor Ext. Constant</th><th>Vendor Ext. String</th>" << std::endl;
	os << "</tr>" << std::endl;
	os << "<tr>" << std::endl;
	os << "<td>" << vendor_ext_constant.to_string(false) << "</td><td>";
	c_string_to_HTML(os, vendor_ext_string);
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	os << "</table>" << std::endl;
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryNull<MEMORY_ADDR>::DWARF_MacInfoListEntryNull()
	: DWARF_MacInfoListEntry<MEMORY_ADDR>(0)
{
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryNull<MEMORY_ADDR>::~DWARF_MacInfoListEntryNull()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_MacInfoListEntryNull<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	DWARF_MacInfoListEntry<MEMORY_ADDR>::offset = _offset;
	int64_t size = 0;
	
	uint8_t type;
	if(max_size < sizeof(type)) return -1;
	memcpy(&type, rawdata, sizeof(type));
	rawdata += sizeof(type);
	size += sizeof(type);
	max_size -= sizeof(type);
	
	if(DWARF_MacInfoListEntry<MEMORY_ADDR>::dw_mac_info_type != type) return -1;
	
	return size;
}

template <class MEMORY_ADDR>
std::string DWARF_MacInfoListEntryNull<MEMORY_ADDR>::to_string() const
{
	return std::string("DW_MACINFO_null");
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryNull<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_MACINFO_null id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryNull<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\">" << std::endl;
	os << "<td>DW_MACINFO_null</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "</td>" << std::endl;
	os << "<td>";
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *next = DWARF_MacInfoListEntry<MEMORY_ADDR>::GetNext();
	if(next)
	{
		os << "<a href=\"../" << next->GetHREF() << "\">mac-" << next->GetId() << "</a>" << std::endl;
	}
	else
	{
		os << "-";
	}
	os << "</td>" << std::endl;
	os << "<td></td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_MACINFO_TCC__
