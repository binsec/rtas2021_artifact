/*
 *  Copyright (c) 2011,
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
 
#ifndef __UNISIM_UTIL_GENERIC_PERIPHERAL_REGISTER_GENERIC_PERIPHERAL_REGISTER_HH__
#define __UNISIM_UTIL_GENERIC_PERIPHERAL_REGISTER_GENERIC_PERIPHERAL_REGISTER_HH__

#include <inttypes.h>

namespace unisim {
namespace util {
namespace generic_peripheral_register {

template <class TYPE>
class GenericPeripheralRegisterInterface
{
public:
	GenericPeripheralRegisterInterface() {};
	virtual ~GenericPeripheralRegisterInterface() {};
	virtual TYPE GetPeripheralRegister(uint64_t addr) = 0;
	virtual void SetPeripheralRegister(uint64_t addr, TYPE data) = 0;
};

typedef GenericPeripheralRegisterInterface<uint8_t>
	GenericPeripheralByteRegisterInterface;

typedef GenericPeripheralRegisterInterface<uint16_t>
	GenericPeripheralHalfWordRegisterInterface;

typedef GenericPeripheralRegisterInterface<uint32_t>
	GenericPeripheralWordRegisterInterface;

typedef GenericPeripheralRegisterInterface<uint64_t>
	GenericPeripheralDoubleWordRegisterInterface;
	
template <class TYPE>
class GenericPeripheralRegister
{
public:
	GenericPeripheralRegister<TYPE>(
			GenericPeripheralRegisterInterface<TYPE> *_peripheral, 
			uint64_t _addr)
		: peripheral(_peripheral)
		, addr(_addr)
	{
	};

	~GenericPeripheralRegister<TYPE>()
	{
		peripheral = 0;
	};

	TYPE GetValue()
	{
		return peripheral->GetPeripheralRegister(addr);
	};

	void SetValue(TYPE value)
	{
		peripheral->SetPeripheralRegister(addr, value);
	};
	
	bool operator != (GenericPeripheralRegister<TYPE> const& gpr) const { return true; }
private:
	GenericPeripheralRegisterInterface<TYPE> *peripheral;
	uint64_t addr;
};

typedef GenericPeripheralRegister<uint8_t>
	GenericPeripheralByteRegister;

typedef GenericPeripheralRegister<uint16_t> 
	GenericPeripheralHalfWordRegister;

typedef	GenericPeripheralRegister<uint32_t>
	GenericPeripheralWordRegister;

typedef GenericPeripheralRegister<uint64_t>
	GenericPeripheralDoubleWordRegister;

} // end of namespace generic_peripheral_register
} // end of namespace util
} // end of namespace unisim

	
#endif
