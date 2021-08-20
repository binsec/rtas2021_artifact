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
#include "channels/mutex.h"

namespace sc_core {

/////////////////////////////////// sc_mutex /////////////////////////////////////////////

sc_mutex::sc_mutex()
	: sc_object(sc_gen_unique_name("mutex"))
	, granted_process(0)
	, unlocked_event((std::string(__LIBIEEE1666_KERNEL_PREFIX__) + "_mutex_unlocked_event").c_str())
{
}

sc_mutex::sc_mutex(const char* _name)
	: sc_object(_name)
	, granted_process(0)
	, unlocked_event((std::string(__LIBIEEE1666_KERNEL_PREFIX__) + _name + "_mutex_unlocked_event").c_str())
{
}

int sc_mutex::lock()
{
	sc_process *current_process = kernel->get_current_process();
	if(granted_process == current_process) return 0; // already locked by calling process
	
	if(granted_process)
	{
		do
		{
			kernel->wait(unlocked_event);
		}
		while(granted_process);
	}
	granted_process = current_process;
	return 0;
}

int sc_mutex::trylock()
{
	sc_process *current_process = kernel->get_current_process();
	if(granted_process == current_process) return 0; // already locked by calling process
	
	if(granted_process)
	{
		return -1;
	}
	granted_process = current_process;
	return 0;
}

int sc_mutex::unlock()
{
	sc_process *current_process = kernel->get_current_process();
	if(granted_process != current_process) return -1; // not locked by calling process
	
	granted_process = 0;
	unlocked_event.notify(sc_core::SC_ZERO_TIME);
	
	return 0;
}

const char* sc_mutex::kind() const
{
	return "sc_mutex";
}

// Disabled
sc_mutex::sc_mutex( const sc_mutex& )
{
}

// Disabled
sc_mutex& sc_mutex::operator= ( const sc_mutex& )
{
	return *this;
}

} // end of namespace sc_core
