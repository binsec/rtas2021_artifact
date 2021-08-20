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
 
#ifndef __UNISIM_UTIL_DEVICE_REGISTER_HH__
#define __UNISIM_UTIL_DEVICE_REGISTER_HH__

#include <string>
#include <inttypes.h>
#include "unisim/util/endian/endian.hh"

namespace unisim {
namespace util {
namespace device {

using std::string;
using namespace unisim::util::endian;

template <class TYPE>
class Register
{
public:
	Register() :
		name("undefined"),
		long_name("undefined"),
		value(0),
		mask(0),
		reset_value(0)
	{
	}
	
	Register(const char *_name,
	         const char *_long_name,
	         TYPE _mask,
	         TYPE _reset_value) :
		name(_name),
		long_name(_long_name),
		value(_reset_value),
		mask(_mask),
		reset_value(_reset_value)
	{
	}
	
	void Initialize(const char *_name,
	                const char *_long_name,
	                TYPE _mask,
	                TYPE _reset_value)
	{
		name = _name;
		long_name = _long_name;
		value = _reset_value;
		mask = _mask;
		reset_value = _reset_value;
	}
	
	~Register()
	{
	}
	
	void Reset()
	{
		value = reset_value;
	}
	
	operator TYPE () const
	{
		//cerr << "Reading 0x" << hex << (uint64_t) value << dec << " from register " << name << " (" << long_name << ")" << endl;
		return value;
	}
	
	Register<TYPE>& operator = (TYPE _value)
	{
		//cerr << "Writing 0x" << hex << _value << dec << " into register " << name << " (" << long_name << ")" << endl;
		value = (value & ~mask) | (_value & mask);
		return *this;
	}
	
	void SetMask(TYPE _mask)
	{
		mask = _mask;
	}
	
	const char *GetName() const
	{
		return name.c_str();
	}
	
	const char *GetLongName() const
	{
		return long_name.c_str();
	}
	
private:
	string name;
	string long_name;
	TYPE value;
	TYPE mask;
	TYPE reset_value;
};

} // end of namespace device
} // end of namespace util
} // end of namespace unisim

#endif
