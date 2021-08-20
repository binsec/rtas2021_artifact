/*
 *  Copyright (c) 2011-2019,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */
 
#include <unisim/util/generic_peripheral_register/generic_peripheral_register.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/kernel.hh>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <inttypes.h>

namespace unisim {
namespace kernel {
namespace variable {

using unisim::util::generic_peripheral_register::GenericPeripheralByteRegister;
using unisim::util::generic_peripheral_register::GenericPeripheralHalfWordRegister;
using unisim::util::generic_peripheral_register::GenericPeripheralWordRegister;
using unisim::util::generic_peripheral_register::GenericPeripheralDoubleWordRegister;

// Variable specialization for GenericPeripheralByteRegister

template <> 
Variable<GenericPeripheralByteRegister> ::
Variable(const char *_name, 
		Object *_object, 
		GenericPeripheralByteRegister& _storage, 
		Type type, 
		const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Initialize();
}

template <>
const char *
Variable<GenericPeripheralByteRegister> ::
GetDataTypeName() const
{
	return "generic_peripheral_byte_register";
}

template <>
VariableBase::DataType Variable<GenericPeripheralByteRegister>::GetDataType() const
{
	return DT_USER;
}

template <> 
Variable<GenericPeripheralByteRegister> ::
operator bool () const 
{ 
	return storage->GetValue() != 0; 
}

template <> 
Variable<GenericPeripheralByteRegister> :: 
operator long long () const
{
	return storage->GetValue();
}

template <> 
Variable<GenericPeripheralByteRegister> ::
operator unsigned long long () const
{
	return storage->GetValue();
}

template <> 
Variable<GenericPeripheralByteRegister> :: 
operator double () const
{
	return (double)(storage->GetValue());
}

template <>
Variable<GenericPeripheralByteRegister> ::
operator std::string () const 
{
	std::stringstream sstr;

	switch (GetFormat())
	{
		case FMT_DEFAULT:
		case FMT_HEX:
			sstr << "0x" << std::hex
				<< (unsigned short int)storage->GetValue()
				<< std::dec;
			break;
		case FMT_DEC:
			sstr << (unsigned int)storage->GetValue();
			break;
	}
	return sstr.str();
}

template <>
VariableBase& Variable<GenericPeripheralByteRegister> ::
operator = (bool value) 
{
	if ( IsMutable() )
	{
		if ( value )
			storage->SetValue(1);
		else
			storage->SetValue(0);
	}
	
	return *this;
}

template <>
VariableBase& Variable<GenericPeripheralByteRegister> ::
operator = (long long value)
{
	if ( IsMutable() )
		storage->SetValue((uint8_t)value);
	return *this;
}

template <>
VariableBase& Variable<GenericPeripheralByteRegister> ::
operator = (unsigned long long value) 
{
	if ( IsMutable() ) 
		storage->SetValue((uint8_t)value);
	return *this;
}

template <>
VariableBase& Variable<GenericPeripheralByteRegister> ::
operator = (double value)
{
	if ( IsMutable() )
		storage->SetValue((uint8_t)value);
	return *this;
}

template <>
VariableBase& Variable<GenericPeripheralByteRegister> ::
operator = (const char *value)
{
	if ( IsMutable() )
		storage->SetValue((uint8_t)strtoull(value, 0, 0));
	return *this;
}

template <>
unsigned int Variable<GenericPeripheralByteRegister>::GetBitSize() const
{
	return 8;
}

template class Variable<GenericPeripheralByteRegister>;

// Variable specialization for GenericPeripheralHalfWordRegister

template <> 
Variable<GenericPeripheralHalfWordRegister> ::
Variable(const char *_name, 
		Object *_object, 
		GenericPeripheralHalfWordRegister& _storage, 
		Type type, 
		const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Initialize();
}

template <>
const char *
Variable<GenericPeripheralHalfWordRegister> ::
GetDataTypeName() const
{
	return "generic_peripheral_halfword_register";
}

template <>
VariableBase::DataType Variable<GenericPeripheralHalfWordRegister>::GetDataType() const
{
	return DT_USER;
}

template <> 
Variable<GenericPeripheralHalfWordRegister> ::
operator bool () const 
{ 
	return storage->GetValue() != 0; 
}

template <> 
Variable<GenericPeripheralHalfWordRegister> :: 
operator long long () const
{
	return storage->GetValue();
}

template <> 
Variable<GenericPeripheralHalfWordRegister> ::
operator unsigned long long () const
{
	return storage->GetValue();
}

template <> 
Variable<GenericPeripheralHalfWordRegister> :: 
operator double () const
{
	return (double)(storage->GetValue());
}

template <>
Variable<GenericPeripheralHalfWordRegister> ::
operator std::string () const 
{
	std::stringstream sstr;

	switch (GetFormat())
	{
		case FMT_DEFAULT:
		case FMT_HEX:
			sstr << "0x" << std::hex
				<< (unsigned int)storage->GetValue()
				<< std::dec;
			break;
		case FMT_DEC:
			sstr << (unsigned int)storage->GetValue();
			break;
	}
	return sstr.str();
}

template <>
VariableBase& Variable<GenericPeripheralHalfWordRegister> ::
operator = (bool value) 
{
	if ( IsMutable() )
	{
		if ( value )
			storage->SetValue(1);
		else
			storage->SetValue(0);
	}
	
	return *this;
}

template <>
VariableBase& Variable<GenericPeripheralHalfWordRegister> ::
operator = (long long value)
{
	if ( IsMutable() )
		storage->SetValue((uint16_t)value);
	return *this;
}

template <>
VariableBase& Variable<GenericPeripheralHalfWordRegister> ::
operator = (unsigned long long value) 
{
	if ( IsMutable() ) 
		storage->SetValue((uint16_t)value);
	return *this;
}

template <>
VariableBase& Variable<GenericPeripheralHalfWordRegister> ::
operator = (double value)
{
	if ( IsMutable() )
		storage->SetValue((uint16_t)value);
	return *this;
}

template <>
VariableBase& Variable<GenericPeripheralHalfWordRegister> ::
operator = (const char *value)
{
	if ( IsMutable() )
		storage->SetValue((uint16_t)strtoull(value, 0, 0));
	return *this;
}

template <>
unsigned int Variable<GenericPeripheralHalfWordRegister>::GetBitSize() const
{
	return 16;
}

template class Variable<GenericPeripheralHalfWordRegister>;

// Variable specialization for GenericPeripheralWordRegister

template <> 
Variable<GenericPeripheralWordRegister> ::
Variable(const char *_name, 
		Object *_object, 
		GenericPeripheralWordRegister& _storage, 
		Type type, 
		const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Initialize();
}

template <>
const char *
Variable<GenericPeripheralWordRegister> ::
GetDataTypeName() const
{
	return "generic_peripheral_word_register";
}

template <>
VariableBase::DataType Variable<GenericPeripheralWordRegister>::GetDataType() const
{
	return DT_USER;
}

template <> 
Variable<GenericPeripheralWordRegister> ::
operator bool () const 
{ 
	return storage->GetValue() != 0; 
}

template <> 
Variable<GenericPeripheralWordRegister> :: 
operator long long () const
{
	return storage->GetValue();
}

template <> 
Variable<GenericPeripheralWordRegister> ::
operator unsigned long long () const
{
	return storage->GetValue();
}

template <> 
Variable<GenericPeripheralWordRegister> :: 
operator double () const
{
	return (double)(storage->GetValue());
}

template <>
Variable<GenericPeripheralWordRegister> ::
operator std::string () const 
{
	std::stringstream sstr;

	switch (GetFormat())
	{
		case FMT_DEFAULT:
		case FMT_HEX:
			sstr << "0x" << std::hex
				<< (unsigned long int)storage->GetValue()
				<< std::dec;
			break;
		case FMT_DEC:
			sstr << (unsigned long int)storage->GetValue();
			break;
	}
	return sstr.str();
}

template <>
VariableBase& Variable<GenericPeripheralWordRegister> ::
operator = (bool value) 
{
	if ( IsMutable() )
	{
		if ( value )
			storage->SetValue(1);
		else
			storage->SetValue(0);
	}
	
	return *this;
}

template <>
VariableBase& Variable<GenericPeripheralWordRegister> ::
operator = (long long value)
{
	if ( IsMutable() )
		storage->SetValue((uint32_t)value);
	return *this;
}

template <>
VariableBase& Variable<GenericPeripheralWordRegister> ::
operator = (unsigned long long value) 
{
	if ( IsMutable() ) 
		storage->SetValue((uint32_t)value);
	return *this;
}

template <>
VariableBase& Variable<GenericPeripheralWordRegister> ::
operator = (double value)
{
	if ( IsMutable() )
		storage->SetValue((uint32_t)value);
	return *this;
}

template <>
VariableBase& Variable<GenericPeripheralWordRegister> ::
operator = (const char *value)
{
	if ( IsMutable() )
		storage->SetValue((uint32_t)strtoull(value, 0, 0));
	return *this;
}

template <>
unsigned int Variable<GenericPeripheralWordRegister>::GetBitSize() const
{
	return 32;
}

template class Variable<GenericPeripheralWordRegister>;

// Variable specialization for GenericPeripheralDoubleWordRegister

template <> 
Variable<GenericPeripheralDoubleWordRegister> ::
Variable(const char *_name, 
		Object *_object, 
		GenericPeripheralDoubleWordRegister& _storage, 
		Type type, 
		const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Initialize();
}

template <>
const char *
Variable<GenericPeripheralDoubleWordRegister> ::
GetDataTypeName() const
{
	return "generic_peripheral_doubleword_register";
}

template <>
VariableBase::DataType Variable<GenericPeripheralDoubleWordRegister>::GetDataType() const
{
	return DT_USER;
}

template <> 
Variable<GenericPeripheralDoubleWordRegister> ::
operator bool () const 
{ 
	return storage->GetValue() != 0; 
}

template <> 
Variable<GenericPeripheralDoubleWordRegister> :: 
operator long long () const
{
	return storage->GetValue();
}

template <> 
Variable<GenericPeripheralDoubleWordRegister> ::
operator unsigned long long () const
{
	return storage->GetValue();
}

template <> 
Variable<GenericPeripheralDoubleWordRegister> :: 
operator double () const
{
	return (double)(storage->GetValue());
}

template <>
Variable<GenericPeripheralDoubleWordRegister> ::
operator std::string () const 
{
	std::stringstream sstr;

	switch (GetFormat())
	{
		case FMT_DEFAULT:
		case FMT_HEX:
			sstr << "0x" << std::hex
				<< (unsigned long long int)storage->GetValue()
				<< std::dec;
			break;
		case FMT_DEC:
			sstr << (unsigned long long int)storage->GetValue();
			break;
	}
	return sstr.str();
}

template <>
VariableBase& Variable<GenericPeripheralDoubleWordRegister> ::
operator = (bool value) 
{
	if ( IsMutable() )
	{
		if ( value )
			storage->SetValue(1);
		else
			storage->SetValue(0);
	}
	
	return *this;
}

template <>
VariableBase& Variable<GenericPeripheralDoubleWordRegister> ::
operator = (long long value)
{
	if ( IsMutable() )
		storage->SetValue((uint64_t)value);
	return *this;
}

template <>
VariableBase& Variable<GenericPeripheralDoubleWordRegister> ::
operator = (unsigned long long value) 
{
	if ( IsMutable() ) 
		storage->SetValue((uint64_t)value);
	return *this;
}

template <>
VariableBase& Variable<GenericPeripheralDoubleWordRegister> ::
operator = (double value)
{
	if ( IsMutable() )
		storage->SetValue((uint64_t)value);
	return *this;
}

template <>
VariableBase& Variable<GenericPeripheralDoubleWordRegister> ::
operator = (const char *value)
{
	if ( IsMutable() )
		storage->SetValue((uint64_t)strtoull(value, 0, 0));
	return *this;
}

template <>
unsigned int Variable<GenericPeripheralDoubleWordRegister>::GetBitSize() const
{
	return 64;
}

template class Variable<GenericPeripheralDoubleWordRegister>;

} // end of variable namespace
} // end of kernel namespace
} // end of unisim namespace
