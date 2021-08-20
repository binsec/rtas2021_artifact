/*
 *  Copyright (c) 2007,
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

#include "unisim/kernel/kernel.hh"
#include <unisim/kernel/variable/variable.hh>
#include "unisim/component/cxx/memory/flash/am29/types.hh"

namespace unisim {
namespace kernel {
namespace variable {

using unisim::component::cxx::memory::flash::am29::MODE;
using unisim::component::cxx::memory::flash::am29::MODE_X8;
using unisim::component::cxx::memory::flash::am29::MODE_X16;

template <> Variable<MODE>::Variable(const char *_name, Object *_object, MODE& _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
}

template <>
const char *Variable<MODE>::GetDataTypeName() const
{
	return "am29lv flash memory transfer mode";
}

template <> Variable<MODE>::operator bool () const { return *storage == MODE_X8; }
template <> Variable<MODE>::operator long long () const { return (*storage == MODE_X8) ? 1: 0; }
template <> Variable<MODE>::operator unsigned long long () const { return (*storage == MODE_X8) ? 1 : 0; }
template <> Variable<MODE>::operator double () const { return (double)(*storage == MODE_X8) ? 1 : 0; }
template <> Variable<MODE>::operator std::string () const { return (*storage == MODE_X8)?(std::string("x8")):(std::string("x16"));}

template <> VariableBase& Variable<MODE>::operator = (bool value)
{
	if(IsMutable())
	{
		MODE tmp = value ? MODE_X8 : MODE_X16;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}
template <> VariableBase& Variable<MODE>::operator = (long long value)
{
	if(IsMutable())
	{
		MODE tmp = value ? MODE_X8 : MODE_X16;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<MODE>::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		MODE tmp = value ? MODE_X8 : MODE_X16;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<MODE>::operator = (double value)
{
	if(IsMutable())
	{
		MODE tmp = value ? MODE_X8 : MODE_X16;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}
template <> VariableBase& Variable<MODE>::operator = (const char *value)
{
	if(IsMutable())
	{
		MODE tmp = (std::string(value) == std::string("x8")) ? MODE_X8 : MODE_X16;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template class Variable<MODE>;

} // end of namespace variable
} // end of namespace kernel
} // end of namespace unisim
