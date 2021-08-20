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

#include <unisim/util/debug/dwarf/abbrev.hh>
#include <unisim/util/debug/dwarf/ml.hh>
#include <unisim/util/debug/dwarf/encoding.hh>
#include <iostream>
#include <sstream>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

DWARF_Abbrev::DWARF_Abbrev(endian_type _endianness)
	: endianness(_endianness)
	, offset(0xffffffffffffffffULL)
	, abbrev_code()
	, dw_tag()
	, dw_children(DW_CHILDREN_no)
	, dw_abbrev_attributes()
{
}

DWARF_Abbrev::~DWARF_Abbrev()
{
	unsigned int num_attrs = dw_abbrev_attributes.size();
	unsigned int i;
	
	for(i = 0; i < num_attrs; i++)
	{
		delete dw_abbrev_attributes[i];
	}
}

int64_t DWARF_Abbrev::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	int64_t size = 0;
	int64_t sz;

	if((sz = abbrev_code.Load(rawdata, max_size)) < 0) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;

	if(abbrev_code.IsZero()) return size; // Abbreviation code 0 is reserved for null entries
	
	if((sz = dw_tag.Load(rawdata, max_size)) < 0) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;

	if(max_size < sizeof(dw_children)) return -1;
	memcpy(&dw_children, rawdata, sizeof(dw_children));
	dw_children = Target2Host(endianness, dw_children);
	rawdata += sizeof(dw_children);
	max_size -= sizeof(dw_children);
	size += sizeof(dw_children);

	do
	{
		if(!max_size) return -1;
		
		DWARF_AbbrevAttribute *dw_abbrev_attribute = new DWARF_AbbrevAttribute();
			
		int64_t sz;
		if((sz = dw_abbrev_attribute->Load(rawdata, max_size)) < 0)
		{
			delete dw_abbrev_attribute;
			return -1;
		}
		size += sz;
		rawdata += sz;
		max_size -= sz;
		
		if(dw_abbrev_attribute->IsNull())
		{
			delete dw_abbrev_attribute;
			break;
		}
		
		dw_abbrev_attributes.push_back(dw_abbrev_attribute);
	} while(1);
	
	return size;
}

bool DWARF_Abbrev::IsNull() const
{
	return abbrev_code.IsZero();
}

const DWARF_LEB128& DWARF_Abbrev::GetAbbrevCode() const
{
	return abbrev_code;
}

const DWARF_LEB128& DWARF_Abbrev::GetTag() const
{
	return dw_tag;
}

uint64_t DWARF_Abbrev::GetOffset() const
{
	return offset;
}

const std::vector<DWARF_AbbrevAttribute *>& DWARF_Abbrev::GetAbbrevAttributes() const
{
	return dw_abbrev_attributes;
}

bool DWARF_Abbrev::HasChildren() const
{
	return dw_children == DW_CHILDREN_yes;
}

std::ostream& DWARF_Abbrev::to_XML(std::ostream& os) const
{
	os << "<DW_ABBREV id=\"abbrev-" << offset << "\" abbrev_code=\"" << abbrev_code.to_string(false) << "\" tag=\"";
	c_string_to_XML(os, DWARF_GetTagName((uint16_t) dw_tag));
	os << "\" children=\"";
	c_string_to_XML(os, DWARF_GetCHILDRENName(dw_children));
	os << "\">" << std::endl;
	unsigned int num_attrs = dw_abbrev_attributes.size();
	unsigned int i;
	
	for(i = 0; i < num_attrs; i++)
	{
		dw_abbrev_attributes[i]->to_XML(os) << std::endl;
	}
	
	os << "</DW_ABBREV>";
	return os;
}

std::ostream& DWARF_Abbrev::to_HTML(std::ostream& os) const
{
	os << "<tr>" << std::endl;
	os << "<td>" << offset << "</td>" << std::endl;
	os << "<td>" << abbrev_code.to_string(false) << "</td>" << std::endl;
	os << "<td>";
	c_string_to_HTML(os, DWARF_GetTagName((uint16_t) dw_tag));
	os << "</td>" << std::endl;
	os << "<td>";
	c_string_to_HTML(os, DWARF_GetCHILDRENName(dw_children));
	os << "</td>" << std::endl;
	os << "<td>" << std::endl;
	unsigned int num_attrs = dw_abbrev_attributes.size();
	
	if(num_attrs)
	{
		unsigned int i;
		
		os << "<table class=\"abbrev_attr\">" << std::endl;
		os << "<tr>" << std::endl;
		os << "<th>Name</th><th>Form</th>" << std::endl;
		os << "</tr>" << std::endl;
		for(i = 0; i < num_attrs; i++)
		{
			os << "<tr>" << std::endl;
			dw_abbrev_attributes[i]->to_HTML(os);
			os << "</tr>" << std::endl;
		}
		os << "</table>" << std::endl;
	}
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}

std::ostream& operator << (std::ostream& os, const DWARF_Abbrev& dw_abbrev)
{
	os << "Abbreviation:" << std::endl
	   << " - Offset: " << dw_abbrev.offset << std::endl
	   << " - Abbrev code: " << dw_abbrev.abbrev_code.to_string(false) << std::endl
	   << " - Tag: " << DWARF_GetTagName((uint16_t) dw_abbrev.dw_tag) << std::endl
	   << " - Has Children? " << ((dw_abbrev.dw_children == DW_CHILDREN_yes) ? "yes" : ((dw_abbrev.dw_children == DW_CHILDREN_no) ? "no" : "invalid encoding")) << std::endl
	   << " - Attributes:" << std::endl;
	
	unsigned int num_attrs = dw_abbrev.dw_abbrev_attributes.size();
	unsigned int i;
	
	for(i = 0; i < num_attrs; i++)
	{
		os << "   - " << *dw_abbrev.dw_abbrev_attributes[i] << std::endl;
	}
	return os;
}

DWARF_AbbrevAttribute::DWARF_AbbrevAttribute()
	: dw_at()
	, dw_form()
{
}

DWARF_AbbrevAttribute::~DWARF_AbbrevAttribute()
{
}

int64_t DWARF_AbbrevAttribute::Load(const uint8_t *rawdata, uint64_t max_size)
{
	int64_t size = 0;
	int64_t sz;

	if((sz = dw_at.Load(rawdata, max_size)) < 0) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;

	if((sz = dw_form.Load(rawdata, max_size)) < 0) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;
	
	return size;
}

bool DWARF_AbbrevAttribute::IsNull() const
{
	return dw_at.IsZero() && dw_form.IsZero();
}

const DWARF_LEB128& DWARF_AbbrevAttribute::GetForm() const
{
	return dw_form;
}

const char *DWARF_AbbrevAttribute::GetName() const
{
	return DWARF_GetATName(dw_at);
}

const DWARF_LEB128& DWARF_AbbrevAttribute::GetTag() const
{
	return dw_at;
}

std::ostream& DWARF_AbbrevAttribute::to_XML(std::ostream& os) const
{
	os << "<DW_ABBREV_ATTR name=\"";
	c_string_to_XML(os, DWARF_GetATName(dw_at));
	os << "\" form=\"";
	c_string_to_XML(os, DWARF_GetFORMName(dw_form));
	os << "\"/>";
	return os;
}

std::ostream& DWARF_AbbrevAttribute::to_HTML(std::ostream& os) const
{
	os << "<td>";
	c_string_to_HTML(os, DWARF_GetATName(dw_at));
	os << "</td><td>";
	c_string_to_HTML(os, DWARF_GetFORMName(dw_form));
	os << "</td>" << std::endl;
	return os;
}

std::ostream& operator << (std::ostream& os, const DWARF_AbbrevAttribute& dw_abbrev_attribute)
{
	return os << "Name: " << DWARF_GetATName(dw_abbrev_attribute.dw_at) << "  Form: " << DWARF_GetFORMName(dw_abbrev_attribute.dw_form);
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
