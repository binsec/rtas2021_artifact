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

#include "core/sysdep/ucontext_coroutine.h"
#include "core/kernel.h"

#include <iostream>

namespace sc_core {

const std::size_t DEFAULT_UCONTEXT_STACK_SIZE = 128 * 1024; // 128 KB

union conv_int_ptr_t
{
	unsigned int i[2];
	void *p;
};

/////////////////////////// sc_ucontext_coroutine //////////////////////////

sc_ucontext_coroutine::sc_ucontext_coroutine()
	: uc()
#if __LIBIEEE1666_UNWIND_SJLJ__
	, sjlj_fc()
#endif
	, fn(0)
	, arg(0)
	, stack(0)
{
}

sc_ucontext_coroutine::sc_ucontext_coroutine(std::size_t stack_size, void (*_fn)(intptr_t), intptr_t _arg)
	: uc()
#if __LIBIEEE1666_UNWIND_SJLJ__
	, sjlj_fc()
#endif
	, fn(_fn)
	, arg(_arg)
	, stack(0)
{
	if(!stack_size) stack_size = DEFAULT_UCONTEXT_STACK_SIZE;
	stack = sc_kernel::get_kernel()->get_stack_system()->create_stack(stack_size);

	getcontext(&uc);
	uc.uc_link = 0;
	uc.uc_stack.ss_size = stack_size;
	uc.uc_stack.ss_sp = stack->get_stack_base();
	
	volatile conv_int_ptr_t conv;
	conv.i[0] = 0;
	conv.i[1] = 0;
	conv.p = this;
	
	makecontext(&uc, reinterpret_cast<void (*)()>(&sc_ucontext_coroutine::entry_point), 2, conv.i[0], conv.i[1]);
}

sc_ucontext_coroutine::~sc_ucontext_coroutine()
{
	if(stack)
	{
		delete stack;
	}
}

void sc_ucontext_coroutine::entry_point(unsigned int arg0, unsigned int arg1)
{
	volatile conv_int_ptr_t conv;
	conv.p = 0;
	conv.i[0] = arg0;
	conv.i[1] = arg1;
	
	sc_ucontext_coroutine *self = reinterpret_cast<sc_ucontext_coroutine *>(conv.p);
	(*self->fn)(self->arg);
}

void sc_ucontext_coroutine::yield(sc_coroutine *next_coroutine)
{
#if __LIBIEEE1666_UNWIND_SJLJ__
	_Unwind_SjLj_Register(&sjlj_fc);
	_Unwind_SjLj_Unregister(&static_cast<sc_ucontext_coroutine *>(next_coroutine)->sjlj_fc);
#endif
	swapcontext(&uc, &static_cast<sc_ucontext_coroutine *>(next_coroutine)->uc);
}

void sc_ucontext_coroutine::abort(sc_coroutine *next_coroutine)
{
#if __LIBIEEE1666_UNWIND_SJLJ__
	_Unwind_SjLj_Register(&sjlj_fc);
	_Unwind_SjLj_Unregister(&static_cast<sc_ucontext_coroutine *>(next_coroutine)->sjlj_fc);
#endif
	swapcontext(&uc, &static_cast<sc_ucontext_coroutine *>(next_coroutine)->uc);
}

///////////////////////// sc_ucontext_coroutine_system /////////////////////

sc_ucontext_coroutine_system::sc_ucontext_coroutine_system()
	: main_coroutine(0)
{
	main_coroutine = new sc_ucontext_coroutine();
}

sc_ucontext_coroutine_system::~sc_ucontext_coroutine_system()
{
	delete main_coroutine;
}

sc_coroutine *sc_ucontext_coroutine_system::get_main_coroutine()
{
	return main_coroutine;
}

sc_coroutine *sc_ucontext_coroutine_system::create_coroutine(std::size_t stack_size, void (*fn)(intptr_t), intptr_t arg)
{
	return new sc_ucontext_coroutine(stack_size, fn, arg);
}

} // end of namespace sc_core
