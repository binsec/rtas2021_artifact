/*
 *  Copyright (c) 2017,
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
 *          Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <unisim/util/reg/core/register.hh>
#include <unisim/util/reg/core/register.tcc>

namespace unisim {
namespace util {
namespace reg {
namespace core {

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////// DECLARATIONS //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// DEFINITIONS /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////// Access ////////////////////////////////////

std::ostream& operator << (std::ostream& os, const Access& access)
{
	switch(access)
	{
		case HW_RO      : os << "hardware read-only/no software access"; break;
		case HW_RW      : os << "hardware read-write/no software access"; break;
		case SW_R_HW_RO : os << "hardware read-only/software read access"; break;
		case SW_R       : os << "hardware read-write/software read access"; break;
		case SW_W       : os << "hardware read-write/software write access"; break;
		case SW_RW      : os << "hardware read-write/software read-write access"; break;
		case SW_W1      : os << "hardware write/software write once access"; break;
		case SW_R_W1    : os << "hardware read-write/software read-write once access"; break;
		case SW_W1C     : os << "hardware read-write/software write 1 clear access"; break;
		case SW_W1_W1C  : os << "hardware read-write/software write one and 1 clear access"; break;
		case SW_R_W1C   : os << "hardware read-write/software read-write 1 clear access"; break;
		case SW_R_W1_W1C: os << "hardware read-write/software read-write once and write 1 clear access"; break;
		default: os << "?"; break;
	}
	
	return os;
};

//////////////////////////////// ReadWriteStatus ////////////////////////////////

std::ostream& operator << (std::ostream& os, const ReadWriteStatus& rws)
{
	bool warn = false;
	
	if(rws == RWS_OK)
	{
		os << "OK";
	}
	else if(rws == RWS_UA)
	{
		os << "unmapped access";
	}
	else
	{
		if(rws & RWS_WOORV)
		{
			os << "writing out-of-range value";
			warn = true;
		}	

		if(rws & RWS_WROR)
		{
			os << (warn ? "and" : "") << "writing read-only register";
			warn = true;
		}

		if(rws & RWS_WROB)
		{
			os << (warn ? "and" : "") << "writing read-only bits";
			warn = true;
		}

		if(rws & RWS_RWOR)
		{
			os << (warn ? "and" : "") << "reading write-only register";
			warn = true;
		}
		
		if(rws & RWS_ANA)
		{
			os << (warn ? "and" : "") << "access not allowed";
			warn = true;
		}
	}
	
	return os;
}

///////////////////////////// PropertyRegistry ////////////////////////////////

std::map<intptr_t, std::string> PropertyRegistry::string_prop_registry[3][3];

void PropertyRegistry::SetStringProperty(ElementType el_type, StringPropertyType str_prop_type, intptr_t key, const std::string& value)
{
	if(!value.empty())
	{
		std::map<intptr_t, std::string>::const_iterator iter = string_prop_registry[el_type][str_prop_type].find(key);
		
		if(iter != string_prop_registry[el_type][str_prop_type].end())
		{
			return; // already exists
		}

		string_prop_registry[el_type][str_prop_type][key] = value;
	}
}

const std::string& PropertyRegistry::GetStringProperty(ElementType el_type, StringPropertyType str_prop_type, intptr_t key)
{
	std::map<intptr_t, std::string>::const_iterator iter = string_prop_registry[el_type][str_prop_type].find(key);
	
	if(iter != string_prop_registry[el_type][str_prop_type].end())
	{
		return (*iter).second;
	}
	
	static std::string null_string;
	
	return null_string;
}

///////////////////////////// LongPrettyPrinter ///////////////////////////////

const unsigned int LongPrettyPrinter::COLUMN_SPACING_PRINT_WIDTH;
const unsigned int LongPrettyPrinter::KIND_PRINT_WIDTH;
const unsigned int LongPrettyPrinter::RANGE_PRINT_WIDTH;
const unsigned int LongPrettyPrinter::NAME_PRINT_WIDTH;
const unsigned int LongPrettyPrinter::VALUE_PRINT_WIDTH;
const unsigned int LongPrettyPrinter::ACCESS_PRINT_WIDTH;

void LongPrettyPrinter::Print(std::ostream& os, const std::string& s, unsigned int column_width)
{
	std::size_t i;
	std::string truncated_string = s;
	if(s.length() > column_width)
	{
		truncated_string.resize(column_width - 3);
		truncated_string.append("...");
	}
	os << truncated_string;
	
	column_width += COLUMN_SPACING_PRINT_WIDTH;
	
	for(i = truncated_string.length(); i < column_width; i++)
	{
		os << ' ';
	}
}

void LongPrettyPrinter::PrintHeader(std::ostream& os)
{
	Print(os, "--KIND--", KIND_PRINT_WIDTH);
	Print(os, "-RANGE--", RANGE_PRINT_WIDTH);
	Print(os, "------NAME------", NAME_PRINT_WIDTH);
	Print(os, "---------VALUE---------", VALUE_PRINT_WIDTH);
	Print(os, "-------------------ACCESS---------------------", ACCESS_PRINT_WIDTH);
	os << "--------------------DESCRIPTION-------------------" << std::endl;
}

void LongPrettyPrinter::PrintKind(std::ostream& os, const std::string& kind)
{
	Print(os, kind, KIND_PRINT_WIDTH);
}

void LongPrettyPrinter::PrintRegisterSize(std::ostream& os, unsigned int size)
{
	std::stringstream sstr_register_size;
	
	sstr_register_size << size << "-bit";

	std::string str_register_size(sstr_register_size.str());
	
	Print(os, str_register_size, RANGE_PRINT_WIDTH);
}

void LongPrettyPrinter::PrintFieldRange(std::ostream& os, unsigned int bitoffset, unsigned int bitwidth)
{
	std::stringstream sstr_field_range;
	
	sstr_field_range << '[';
	if(bitwidth)
	{
		if(bitwidth != 1)
		{
			sstr_field_range << (bitoffset + bitwidth - 1);
			sstr_field_range << "..";
		}
		sstr_field_range << bitoffset;
	}
	sstr_field_range << ']';
	
	std::string str_field_range(sstr_field_range.str());
	
	Print(os, str_field_range, RANGE_PRINT_WIDTH);
}

void LongPrettyPrinter::PrintDisplayName(std::ostream& os, const std::string& display_name)
{
	Print(os, display_name, NAME_PRINT_WIDTH);
}

void LongPrettyPrinter::PrintAccess(std::ostream& os, const Access& access)
{
	std::stringstream sstr_access;
	
	sstr_access << access;
	
	std::string str_access(sstr_access.str());
	
	Print(os, str_access, ACCESS_PRINT_WIDTH);
}

void LongPrettyPrinter::PrintDescription(std::ostream& os, const std::string& s)
{
	os << s;
}

//////////////////////////////// FieldID ///////////////////////////////////

unsigned int FieldID()
{
	static unsigned int field_id = 0;
	return field_id++;
}

/////////////////////////////// NullField //////////////////////////////////

void NullField::SetName(const std::string& name)
{
}

void NullField::SetDisplayName(const std::string& disp_name)
{
}

void NullField::SetDescription(const std::string& desc)
{
}

const std::string& NullField::GetName()
{
	static std::string null_string;
	return null_string;
}

const std::string& NullField::GetDescription()
{
	static std::string null_string;
	return null_string;
}

} // end of namespace core
} // end of namespace reg
} // end of namespace util
} // end of namespace unisim
