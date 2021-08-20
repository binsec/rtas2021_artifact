/*
 *  Copyright (c) 2008,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#include <unisim/kernel/variable/variable.hh>
#include "unisim/component/tlm2/interconnect/generic_router/mapping.hh"
#include "unisim/kernel/kernel.hh"

namespace unisim {
namespace kernel {
namespace variable {

using unisim::kernel::variable::Variable;

template <> Variable<unisim::component::tlm2::interconnect::generic_router::Mapping>::Variable(const char *_name, Object *_object, unisim::component::tlm2::interconnect::generic_router::Mapping &_storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage) {
	Initialize();
}

template <>
unsigned int Variable<unisim::component::tlm2::interconnect::generic_router::Mapping>::GetBitSize() const { return 0; }

template <> Variable<unisim::component::tlm2::interconnect::generic_router::Mapping>::operator bool () const { return false; }
template <> Variable<unisim::component::tlm2::interconnect::generic_router::Mapping>::operator long long () const { return 0; }
template <> Variable<unisim::component::tlm2::interconnect::generic_router::Mapping>::operator unsigned long long () const { return 0; }
template <> Variable<unisim::component::tlm2::interconnect::generic_router::Mapping>::operator double () const { return 0; }
template <> Variable<unisim::component::tlm2::interconnect::generic_router::Mapping>::operator std::string () const { 
	std::stringstream buf;
	buf << "range_start=\"0x" << std::hex << storage->range_start << std::dec
		<< "\" range_end=\"0x" << std::hex << storage->range_end << std::dec
		<< "\" output_port=\"" << storage->output_port 
		<< "\" translation=\"0x" << std::hex << storage->translation << std::dec << "\"";
	return buf.str();
}

template <> VariableBase& Variable<unisim::component::tlm2::interconnect::generic_router::Mapping>::operator = (bool value) { return *this;}
template <> VariableBase& Variable<unisim::component::tlm2::interconnect::generic_router::Mapping>::operator = (long long value) { return *this;}
template <> VariableBase& Variable<unisim::component::tlm2::interconnect::generic_router::Mapping>::operator = (unsigned long long value) { return *this;}
template <> VariableBase& Variable<unisim::component::tlm2::interconnect::generic_router::Mapping>::operator = (double value) { return *this;}
template <> VariableBase& Variable<unisim::component::tlm2::interconnect::generic_router::Mapping>::operator = (const char *value) { 
	if(IsMutable())
	{
		uint64_t range_start = 0;
		uint64_t range_end = 0;
		unsigned int output_port = 0;
		uint64_t translation = 0;

		std::stringstream buf(value);
		std::string str(buf.str());
		std::string str_rest;
		size_t pos;
		pos = str.find('"');
		if(pos == std::string::npos) pos = str.find('\'');
		if(pos != std::string::npos)
		{
			str_rest = str.substr(pos + 1);
			str = str_rest;
			pos = str.find('"');
			if(pos == std::string::npos) pos = str.find('\'');
			if(pos != std::string::npos)
			{
				str_rest = str.substr(pos + 1);
				str = str.substr(0, pos);
				std::stringstream range_start_str;
				range_start_str << str;
				range_start_str >> std::hex >> range_start >> std::dec;
				str = str_rest;
				pos = str.find('"');
				if(pos == std::string::npos) pos = str.find('\'');
				if(pos != std::string::npos)
				{
					str = str.substr(pos + 1);
					pos = str.find('"');
					if(pos == std::string::npos) pos = str.find('\'');
					if(pos != std::string::npos)
					{
						str_rest = str.substr(pos + 1);
						str = str.substr(0, pos);
						std::stringstream range_end_str;
						range_end_str << str;
						range_end_str >> std::hex >> range_end >> std::dec;
						str = str_rest;
						pos = str.find('"');
						if(pos == std::string::npos) pos = str.find('\'');
						if(pos != std::string::npos)
						{
							str = str.substr(pos + 1);
							pos = str.find('"');
							if(pos == std::string::npos) pos = str.find('\'');
							if(pos != std::string::npos)
							{
								str_rest = str.substr(pos + 1);
								str = str.substr(0, pos);
								std::stringstream output_port_str;
								output_port_str << str;
								output_port_str >> output_port;
								str = str_rest;
								pos = str.find('"');
								if(pos == std::string::npos) pos = str.find('\'');
								if (pos != std::string::npos)
								{
									// translation available
									std::stringstream translation_str;
									str = str.substr(pos + 1);
									pos = str.find('"');
									if(pos == std::string::npos) pos = str.find('\'');
									if(pos != std::string::npos)
									{
										str = str.substr(0, pos);
										std::stringstream translation_st;
										translation_str << str;
										translation_str >> std::hex >> translation >> std::dec;
									}
								}
								else
								{
									translation = range_start;
								}
							}
						}
					}
				}
			}
		}
		
		unisim::component::tlm2::interconnect::generic_router::Mapping tmp;
		tmp.used = true;
		tmp.range_start = range_start;
		tmp.range_end = range_end;
		tmp.output_port = output_port;
		tmp.translation = translation;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> const char *Variable<unisim::component::tlm2::interconnect::generic_router::Mapping>::GetDataTypeName() const {
	return "unisim::component::tlm2::interconnect::generic_router::Mapping";
}

template <> VariableBase::DataType Variable<unisim::component::tlm2::interconnect::generic_router::Mapping>::GetDataType() const
{
	return DT_USER;
}

template class Variable<unisim::component::tlm2::interconnect::generic_router::Mapping>;

} // end of namespace variable
} // end of namespace kernel
} // end of namespace unisim


