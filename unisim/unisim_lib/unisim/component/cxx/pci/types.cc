/*
 *  Copyright (c) 2007,
 *  Universitat Politecnica de Catalunya (UPC)
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
 *   - Neither the name of UPC nor the names of its contributors may be used to
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
 * Authors: Paula Casero (pcasero@upc.edu), Alejandro Schenzle (schenzle@upc.edu),
 *    Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#include "unisim/component/cxx/pci/types.hh"
#include <unisim/kernel/variable/variable.hh>

namespace unisim {
namespace kernel {
namespace variable {

using std::stringstream;
using namespace unisim::component::cxx::pci;


template <> Variable<PCISpace>::Variable(const char *_name, Object *_object, PCISpace& _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Initialize();
	AddEnumeratedValue("mem");
	AddEnumeratedValue("i/o");
	AddEnumeratedValue("cfg");
}

template <>
const char *Variable<PCISpace>::GetDataTypeName() const
{
	return "pci space";
}

template <>
VariableBase::DataType Variable<PCISpace>::GetDataType() const
{
	return DT_USER;
}

template <>
unsigned int Variable<PCISpace>::GetBitSize() const
{
	return 2;
}

template <> Variable<PCISpace>::operator bool () const {
	//this should't be implemented 
	return false; 
}

template <> Variable<PCISpace>::operator long long () const {
	switch(*storage) {
	case SP_MEM:
		return 1;
	case SP_IO:
		return 2;
	case SP_CONFIG:
		return 3;
	}
	return 0;
}
    
template <> Variable<PCISpace>::operator unsigned long long () const {   	
  	switch(*storage) {
	case SP_MEM:
		return 1;
	case SP_IO:
		return 2;
	case SP_CONFIG:
		return 3;
	}
	return 0;
}

template <> Variable<PCISpace>::operator double () const { 	
  	switch(*storage) {
  	case SP_MEM:
		return (double)1;
	case SP_IO:
		return (double)2;
	case SP_CONFIG:
		return (double)3;
	}
	
	return 0.0;
}

template <> Variable<PCISpace>::operator std::string () const {
	switch(*storage) {
	case SP_MEM:
		return "mem";
	case SP_IO:
		return "i/o";
	case SP_CONFIG:
		return "cfg";
	}
	
	return std::string();
}
  
template <> VariableBase& Variable<PCISpace>::operator = (bool value) { return *this;}

template <> VariableBase& Variable<PCISpace>::operator = (long long value) { 
	if(IsMutable())
	{
		PCISpace tmp;
		switch(value) {
		case 1:
			tmp = SP_MEM;
			SetModified(*storage != tmp);
			*storage = tmp;
			break;
		case 2:
			tmp = SP_IO;
			SetModified(*storage != tmp);
			*storage = tmp;
			break;
		case 3:
			tmp = SP_CONFIG;
			SetModified(*storage != tmp);
			*storage = tmp;
			break;
		} 
	}
	return *this;
}

template <> VariableBase& Variable<PCISpace>::operator = (unsigned long long value) {   	
	if(IsMutable())
	{
		PCISpace tmp;
		switch(value) {
		case 1:
			tmp = SP_MEM;
			SetModified(*storage != tmp);
			*storage = tmp;
			break;
		case 2:
			tmp = SP_IO;
			SetModified(*storage != tmp);
			*storage = tmp;
			break;
		case 3:
			tmp = SP_CONFIG;
			SetModified(*storage != tmp);
			*storage = tmp;
			break;
		} 
	}
	return *this;
}

template <> VariableBase& Variable<PCISpace>::operator = (double value) {   	
	if(IsMutable())
	{
		PCISpace tmp;
		if(value == 1)
		{
			tmp = SP_MEM;
			SetModified(*storage != tmp);
			*storage = tmp;
		}
		else if(value == 2)
		{
			tmp = SP_IO;
			SetModified(*storage != tmp);
			*storage = tmp;
		}
		else if(value == 3) 
		{
			tmp = SP_CONFIG;
			SetModified(*storage != tmp);
			*storage = tmp;
		}
	}
 	return *this;
}

template <> VariableBase& Variable<PCISpace>::operator = (const char *value) { 
	if(IsMutable())
	{
		PCISpace tmp;
		if (std::string(value) == std::string("mem"))
		{
			tmp = SP_MEM;
			SetModified(*storage != tmp);
			*storage = tmp;
		}
		else if (std::string(value) == std::string("i/o"))
		{
			tmp = SP_IO;
			SetModified(*storage != tmp);
			*storage = tmp;
		}
		else if (std::string(value) == std::string("cfg"))
		{
			tmp = SP_CONFIG;
			SetModified(*storage != tmp);
			*storage = tmp;
		}
	}
 	return *this;
}

template class Parameter<PCISpace>;
template class ParameterArray<PCISpace>;
  
}
}
}


