/*
 *  Copyright (c) 2012,
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

#include <unisim/util/debug/dwarf/register_number_mapping.hh>
#include <unisim/util/xml/xml.hh>
#include <stdlib.h>
#include <iostream>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

DWARF_RegisterNumberMapping::DWARF_RegisterNumberMapping(std::ostream& _debug_info_stream, std::ostream& _debug_warning_stream, std::ostream& _debug_error_stream, unisim::service::interfaces::Registers *_regs_if)
	: debug_info_stream(_debug_info_stream)
	, debug_warning_stream(_debug_warning_stream)
	, debug_error_stream(_debug_error_stream)
	, regs_if(_regs_if)
	, sp_reg_num(0)
{
}

DWARF_RegisterNumberMapping::~DWARF_RegisterNumberMapping()
{
}

bool DWARF_RegisterNumberMapping::Load(const char *filename, const char *architecture)
{
	unisim::util::xml::Parser *parser = new unisim::util::xml::Parser(debug_info_stream, debug_warning_stream, debug_error_stream);
	unisim::util::xml::Node *root_node = parser->Parse(filename);

	delete parser;

	if(!root_node) return false;
		
	if(root_node->Name() != std::string("dw_reg_num_mapping"))
	{
		debug_error_stream << root_node->Filename() << ":" << root_node->LineNo() << ":root node is not named 'dw_reg_num_mapping'" << std::endl;
		delete root_node;
		return false;
	}
	
	const std::list<unisim::util::xml::Node *> *xml_nodes = root_node->Childs();
	std::list<unisim::util::xml::Node *>::const_iterator xml_node;

	for(xml_node = xml_nodes->begin(); xml_node != xml_nodes->end(); xml_node++)
	{
		if((*xml_node)->Name() == std::string("architecture"))
		{
			const std::list<unisim::util::xml::Property *> *xml_node_properties = (*xml_node)->Properties();

			std::list<unisim::util::xml::Property *>::const_iterator xml_node_property;
			for(xml_node_property = xml_node_properties->begin(); xml_node_property != xml_node_properties->end(); xml_node_property++)
			{
				if((*xml_node_property)->Name() == std::string("name"))
				{
					if((*xml_node_property)->Value() == std::string(architecture))
					{
						bool status = Load(*xml_node);
						delete root_node;
						return status;
					}
				}
			}
		}
	}
	
	debug_error_stream << root_node->Filename() << ": no matching architecture tag for Architecture \"" << architecture << "\"" << std::endl;

	delete root_node;
	return false;
}

bool DWARF_RegisterNumberMapping::Load(unisim::util::xml::Node *root_node)
{
	const std::list<unisim::util::xml::Node *> *xml_nodes = root_node->Childs();
	std::list<unisim::util::xml::Node *>::const_iterator xml_node;
	unsigned int dw_reg_num = unsigned(-1);
	std::string arch_reg_name;

	for(xml_node = xml_nodes->begin(); xml_node != xml_nodes->end(); xml_node++)
	{
		if((*xml_node)->Name() == std::string("register"))
		{
			bool has_dw_reg_num = false;
			bool has_arch_reg_name = false;
			const std::list<unisim::util::xml::Property *> *xml_node_properties = (*xml_node)->Properties();

			std::list<unisim::util::xml::Property *>::const_iterator xml_node_property;
			for(xml_node_property = xml_node_properties->begin(); xml_node_property != xml_node_properties->end(); xml_node_property++)
			{
				if((*xml_node_property)->Name() == std::string("dw_reg_num"))
				{
					dw_reg_num = atoi((*xml_node_property)->Value().c_str());
					has_dw_reg_num = true;
				}
				else if((*xml_node_property)->Name() == std::string("arch_reg"))
				{
					arch_reg_name = (*xml_node_property)->Value();
					has_arch_reg_name = true;
				}
			}
			
			if(has_dw_reg_num && has_arch_reg_name)
			{
				unisim::service::interfaces::Register *arch_reg = regs_if->GetRegister(arch_reg_name.c_str());
				
				if(arch_reg)
				{
					Map(dw_reg_num, arch_reg);
				}
				else
				{
					debug_warning_stream << "CPU does not support register " << arch_reg_name << std::endl;
				}
			}
			else
			{
				debug_warning_stream << (*xml_node)->Filename() << ":" << (*xml_node)->LineNo() << ":node '" << (*xml_node)->Name().c_str() << "' has no 'dw_reg_num' or 'arch_reg' property" << std::endl;
				return false;
			}
		}
		else if((*xml_node)->Name() == std::string("stack_pointer"))
		{
			bool has_dw_reg_num = false;
			const std::list<unisim::util::xml::Property *> *xml_node_properties = (*xml_node)->Properties();

			std::list<unisim::util::xml::Property *>::const_iterator xml_node_property;
			for(xml_node_property = xml_node_properties->begin(); xml_node_property != xml_node_properties->end(); xml_node_property++)
			{
				if((*xml_node_property)->Name() == std::string("dw_reg_num"))
				{
					dw_reg_num = atoi((*xml_node_property)->Value().c_str());
					has_dw_reg_num = true;
				}
			}
			
			if(has_dw_reg_num)
			{
				sp_reg_num = dw_reg_num;
			}
			else
			{
				debug_warning_stream << (*xml_node)->Filename() << ":" << (*xml_node)->LineNo() << ":node '" << (*xml_node)->Name().c_str() << "' has no 'dw_reg_num' property" << std::endl;
				return false;
			}
		}
			
	}
	
	return true;
}

void DWARF_RegisterNumberMapping::Map(unsigned int dw_reg_num, unisim::service::interfaces::Register *arch_reg)
{
	reg_num_mapping[dw_reg_num] = arch_reg;
}

const unisim::service::interfaces::Register *DWARF_RegisterNumberMapping::GetArchReg(unsigned int dw_reg_num) const
{
	std::map<unsigned int, unisim::service::interfaces::Register *>::const_iterator iter = reg_num_mapping.find(dw_reg_num);
	
	return (iter != reg_num_mapping.end()) ? (*iter).second : 0;
}

unisim::service::interfaces::Register *DWARF_RegisterNumberMapping::GetArchReg(unsigned int dw_reg_num)
{
	std::map<unsigned int, unisim::service::interfaces::Register *>::const_iterator iter = reg_num_mapping.find(dw_reg_num);
	
	return (iter != reg_num_mapping.end()) ? (*iter).second : 0;
}

void DWARF_RegisterNumberMapping::EnumRegisterNumbers(std::set<unsigned int>& reg_num_set) const
{
	reg_num_set.clear();

	std::map<unsigned int, unisim::service::interfaces::Register *>::const_iterator iter;
	
	for(iter = reg_num_mapping.begin(); iter != reg_num_mapping.end(); iter++)
	{
		unsigned int dw_reg_num = (*iter).first;
		reg_num_set.insert(dw_reg_num);
	}
}

unsigned int DWARF_RegisterNumberMapping::GetSPRegNum() const
{
	return sp_reg_num;
}

std::ostream& operator << (std::ostream& os, const DWARF_RegisterNumberMapping& dw_reg_num_mapping)
{
	std::map<unsigned int, unisim::service::interfaces::Register *>::const_iterator iter;
	
	for(iter = dw_reg_num_mapping.reg_num_mapping.begin(); iter != dw_reg_num_mapping.reg_num_mapping.end(); iter++)
	{
		unsigned int dw_reg_num = (*iter).first;
		unisim::service::interfaces::Register *arch_reg = (*iter).second;
		os << "r" << dw_reg_num << "->" << arch_reg->GetName() << std::endl;
	}
	return os;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
