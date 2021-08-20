/*
 *  Copyright (c) 2007-2019
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
 
#include <unisim/util/endian/endian.hh>
#include <unisim/kernel/variable/variable.hh>

namespace unisim {
namespace kernel {
namespace variable {

using unisim::util::endian::endian_type;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_UNKNOWN_ENDIAN;

//   template <> Variable<endian_type>::Variable(const char *_name, Configurable *_configurable, endian_type& _storage) :
//     VariableBase(_name, _configurable), storage(&_storage)
//   {
//   }

template <> Variable<endian_type>::Variable(const char *_name, Object *_object, endian_type& _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Initialize();
	AddEnumeratedValue("little-endian");
	AddEnumeratedValue("big-endian");
}

template <>
const char *Variable<endian_type>::GetDataTypeName() const
{
	return "endianess";
}

template <>
VariableBase::DataType Variable<endian_type>::GetDataType() const
{
	return DT_USER;
}

template <>
unsigned int Variable<endian_type>::GetBitSize() const
{
	return 1;
}

template <> Variable<endian_type>::operator bool () const { return *storage == E_LITTLE_ENDIAN; }
template <> Variable<endian_type>::operator long long () const { return (*storage == E_LITTLE_ENDIAN)?1:0; }
template <> Variable<endian_type>::operator unsigned long long () const { return (*storage == E_LITTLE_ENDIAN)?1:0; }
template <> Variable<endian_type>::operator double () const { return (double)(*storage == E_LITTLE_ENDIAN)?1:0; }
template <> Variable<endian_type>::operator std::string () const { return (*storage == E_LITTLE_ENDIAN)?(std::string("little-endian")):(std::string("big-endian"));}

template <> VariableBase& Variable<endian_type>::operator = (bool value)
{
	if(IsMutable())
	{
		endian_type tmp = (value ? E_LITTLE_ENDIAN : E_BIG_ENDIAN);
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<endian_type>::operator = (long long value)
{
	if(IsMutable())
	{
		endian_type tmp = (value ? E_LITTLE_ENDIAN : E_BIG_ENDIAN);
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<endian_type>::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		endian_type tmp = (value ? E_LITTLE_ENDIAN : E_BIG_ENDIAN);
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<endian_type>::operator = (double value)
{
	if(IsMutable())
	{
		endian_type tmp = (value ? E_LITTLE_ENDIAN : E_BIG_ENDIAN);
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<endian_type>::operator = (const char *value)
{
	if(IsMutable())
	{
		endian_type tmp = (std::string(value) == std::string("little-endian")) ? E_LITTLE_ENDIAN : (std::string(value) == std::string("big-endian")) ? E_BIG_ENDIAN : E_UNKNOWN_ENDIAN;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template class Variable<endian_type>;

} // end of variable namespace
} // end of kernel namespace
} // end of unisim namespace
