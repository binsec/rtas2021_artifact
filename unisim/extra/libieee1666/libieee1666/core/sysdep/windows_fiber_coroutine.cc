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

#include "core/sysdep/windows_fiber_coroutine.h"

#include <iostream>
#include <stdexcept>

namespace sc_core {

const std::size_t DEFAULT_WINDOWS_FIBER_STACK_SIZE = 128 * 1024; // 128 KB

/////////////////////////// sc_windows_fiber_coroutine //////////////////////////

sc_windows_fiber_coroutine::sc_windows_fiber_coroutine()
	: fc()
#if __LIBIEEE1666_UNWIND_SJLJ__
	, sjlj_fc()
#endif
	, main_coroutine(this)
	, fn(0)
	, arg(0)
	, main_thread_was_fiber(false)
{
	fc = ::ConvertThreadToFiber(NULL);

	if(!fc && (::GetLastError() == ERROR_ALREADY_FIBER))
	{
		// already a fiber
		fc = ::GetCurrentFiber();
		main_thread_was_fiber = true;
	}
	
	if(!fc) throw std::runtime_error("Failed converting thread to fiber");
}

sc_windows_fiber_coroutine::sc_windows_fiber_coroutine(sc_windows_fiber_coroutine *_main_coroutine, std::size_t stack_size, void (*_fn)(intptr_t), intptr_t _arg)
	: fc()
#if __LIBIEEE1666_UNWIND_SJLJ__
	, sjlj_fc()
#endif
	, main_coroutine(_main_coroutine)
	, fn(_fn)
	, arg(_arg)
	, main_thread_was_fiber(false)
{
	if(!stack_size) stack_size = DEFAULT_WINDOWS_FIBER_STACK_SIZE;

	fc = ::CreateFiber(stack_size, (LPFIBER_START_ROUTINE) &sc_windows_fiber_coroutine::entry_point, this);
}

sc_windows_fiber_coroutine::~sc_windows_fiber_coroutine()
{
	if(this == main_coroutine)
	{
		if(!main_thread_was_fiber)
		{
			::ConvertFiberToThread();
		}
	}
	else
	{
		::DeleteFiber(fc);
	}
}

void sc_windows_fiber_coroutine::entry_point(void *_self)
{
	sc_windows_fiber_coroutine *self = reinterpret_cast<sc_windows_fiber_coroutine *>(_self);
	(*self->fn)(self->arg);
}

void sc_windows_fiber_coroutine::yield(sc_coroutine *next_coroutine)
{
#if __LIBIEEE1666_UNWIND_SJLJ__
	_Unwind_SjLj_Register(&sjlj_fc);
	_Unwind_SjLj_Unregister(&static_cast<sc_windows_fiber_coroutine *>(next_coroutine)->sjlj_fc);
#endif
	SwitchToFiber(static_cast<sc_windows_fiber_coroutine *>(next_coroutine)->fc);
}

void sc_windows_fiber_coroutine::abort(sc_coroutine *next_coroutine)
{
#if __LIBIEEE1666_UNWIND_SJLJ__
	_Unwind_SjLj_Register(&sjlj_fc);
	_Unwind_SjLj_Unregister(&static_cast<sc_windows_fiber_coroutine *>(next_coroutine)->sjlj_fc);
#endif
	SwitchToFiber(static_cast<sc_windows_fiber_coroutine *>(next_coroutine)->fc);
}

///////////////////////// sc_windows_fiber_coroutine_system /////////////////////

sc_windows_fiber_coroutine_system::sc_windows_fiber_coroutine_system()
	: main_coroutine(0)
{
	main_coroutine = new sc_windows_fiber_coroutine();
}

sc_windows_fiber_coroutine_system::~sc_windows_fiber_coroutine_system()
{
	delete main_coroutine;
}

sc_coroutine *sc_windows_fiber_coroutine_system::get_main_coroutine()
{
	return main_coroutine;
}

sc_coroutine *sc_windows_fiber_coroutine_system::create_coroutine(std::size_t stack_size, void (*fn)(intptr_t), intptr_t arg)
{
	return new sc_windows_fiber_coroutine(main_coroutine, stack_size, fn, arg);
}

} // end of namespace sc_core
