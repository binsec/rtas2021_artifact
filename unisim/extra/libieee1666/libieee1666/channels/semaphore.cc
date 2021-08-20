/*
 *  Copyright (c) 2014,
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

#include "core/kernel.h"
#include "channels/semaphore.h"
#include <stdexcept>

namespace sc_core {

//////////////////////////////// sc_semaphore ////////////////////////////////////////
	
sc_semaphore::sc_semaphore(int _value)
	: sc_object(sc_gen_unique_name("semaphore"))
	, value(_value)
	, value_incremented_event((std::string(__LIBIEEE1666_KERNEL_PREFIX__) + "_semaphore_value_incremented_event").c_str())
{
	if(value < 0) throw std::runtime_error("semaphore value shall be non-negative");
}

sc_semaphore::sc_semaphore(const char *_name, int _value)
	: sc_object(_name)
	, value(_value)
	, value_incremented_event((std::string(__LIBIEEE1666_KERNEL_PREFIX__) + _name + "_semaphore_value_incremented_event").c_str())
{
	if(value < 0) throw std::runtime_error("semaphore value shall be non-negative");
}

int sc_semaphore::wait()
{
	if(value <= 0)
	{
		do
		{
			kernel->wait(value_incremented_event);
		}
		while(value <= 0);
	}
	--value;
	
	return 0;
}

int sc_semaphore::trywait()
{
	if(value <= 0)
	{
		return -1;
	}
	
	--value;
	return 0;
}

int sc_semaphore::post()
{
	++value;
	value_incremented_event.notify(); // immediate notification
	return 0;
}

int sc_semaphore::get_value() const
{
	return value;
}

const char* sc_semaphore::kind() const
{
	return "sc_semaphore";
}

// Disabled
sc_semaphore::sc_semaphore( const sc_semaphore& )
{
}

// Disabled
sc_semaphore& sc_semaphore::operator= ( const sc_semaphore& )
{
	return *this;
}

} // end of namespace sc_core
