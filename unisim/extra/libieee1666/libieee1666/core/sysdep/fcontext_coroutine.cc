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

#include "core/sysdep/fcontext_coroutine.h"
#include "core/kernel.h"

#include <iostream>

namespace sc_core {

const std::size_t DEFAULT_FCONTEXT_STACK_SIZE = 128 * 1024; // 128 KB

/////////////////////////// sc_fcontext_coroutine //////////////////////////

static boost::context::fcontext_t fcm;

sc_fcontext_coroutine::sc_fcontext_coroutine()
	: fc()
#if __LIBIEEE1666_UNWIND_SJLJ__
	, sjlj_fc()
#endif
	, fn(0)
	, arg(0)
	, stack(0)
{
#if BOOST_VERSION < 105600 // boost version < 1.56.0
	fc = &fcm;
#endif
}

sc_fcontext_coroutine::sc_fcontext_coroutine(std::size_t stack_size, void (*_fn)(intptr_t), intptr_t _arg)
	: fc()
#if __LIBIEEE1666_UNWIND_SJLJ__
	, sjlj_fc()
#endif
	, fn(_fn)
	, arg(_arg)
	, stack(0)
{
	if(!stack_size) stack_size = DEFAULT_FCONTEXT_STACK_SIZE;
	stack = sc_kernel::get_kernel()->get_stack_system()->create_stack(stack_size);
	fc = boost::context::make_fcontext(stack->get_top_of_the_stack(), stack_size, &sc_fcontext_coroutine::entry_point);
}

sc_fcontext_coroutine::~sc_fcontext_coroutine()
{
	if(stack)
	{
		delete stack;
	}
}

void sc_fcontext_coroutine::entry_point(intptr_t _self)
{
	sc_fcontext_coroutine *self = reinterpret_cast<sc_fcontext_coroutine *>(_self);
	(*self->fn)(self->arg);
}

void sc_fcontext_coroutine::yield(sc_coroutine *next_coroutine)
{
#if __LIBIEEE1666_UNWIND_SJLJ__
	_Unwind_SjLj_Register(&sjlj_fc);
	_Unwind_SjLj_Unregister(&static_cast<sc_fcontext_coroutine *>(next_coroutine)->sjlj_fc);
#endif
	boost::context::jump_fcontext(
#if BOOST_VERSION >= 105600 // boost version >= 1.56.0
		&fc,
#else
		fc,
#endif
		static_cast<sc_fcontext_coroutine *>(next_coroutine)->fc,
		reinterpret_cast<intptr_t>(next_coroutine), true);
}

void sc_fcontext_coroutine::abort(sc_coroutine *next_coroutine)
{
#if __LIBIEEE1666_UNWIND_SJLJ__
	_Unwind_SjLj_Register(&sjlj_fc);
	_Unwind_SjLj_Unregister(&static_cast<sc_fcontext_coroutine *>(next_coroutine)->sjlj_fc);
#endif
	boost::context::jump_fcontext(
#if BOOST_VERSION >= 105600 // boost version >= 1.56.0
		&fc,
#else
		fc,
#endif
		static_cast<sc_fcontext_coroutine *>(next_coroutine)->fc,
		reinterpret_cast<intptr_t>(next_coroutine), true);
}

///////////////////////// sc_fcontext_coroutine_system /////////////////////

sc_fcontext_coroutine_system::sc_fcontext_coroutine_system()
	: main_coroutine(0)
{
	main_coroutine = new sc_fcontext_coroutine();
}

sc_fcontext_coroutine_system::~sc_fcontext_coroutine_system()
{
	delete main_coroutine;
}

sc_coroutine *sc_fcontext_coroutine_system::get_main_coroutine()
{
	return main_coroutine;
}

sc_coroutine *sc_fcontext_coroutine_system::create_coroutine(std::size_t stack_size, void (*fn)(intptr_t), intptr_t arg)
{
	return new sc_fcontext_coroutine(stack_size, fn, arg);
}

} // end of namespace sc_core
