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

#ifndef __LIBIEEE1666_CORE_SYSDEP_WINDOWS_FIBER_COROUTINE_H__
#define __LIBIEEE1666_CORE_SYSDEP_WINDOWS_FIBER_COROUTINE_H__

#include "core/coroutine.h"
#include "core/features.h"

#if __LIBIEEE1666_UNWIND_SJLJ__
#include "core/sysdep/sjlj_except.h"
#endif

#include <windows.h>

namespace sc_core {

class sc_windows_fiber_coroutine_system;

class sc_windows_fiber_coroutine : public sc_coroutine
{
public:
	sc_windows_fiber_coroutine(sc_windows_fiber_coroutine *main_coroutine, std::size_t stack_size, void (*fn)(intptr_t), intptr_t arg);
	virtual ~sc_windows_fiber_coroutine();
	
	virtual void yield(sc_coroutine *next_coroutine);
	virtual void abort(sc_coroutine *next_coroutine);
private:
	explicit sc_windows_fiber_coroutine(); // reserved for main coroutine
	
	friend class sc_windows_fiber_coroutine_system;

	LPVOID fc;
#if __LIBIEEE1666_UNWIND_SJLJ__
	struct SjLj_Function_Context sjlj_fc;
#endif
	sc_windows_fiber_coroutine *main_coroutine;
	void (*fn)(intptr_t);
	intptr_t arg;
	bool main_thread_was_fiber;
	
	static void entry_point(void *);
};

class sc_windows_fiber_coroutine_system : public sc_coroutine_system
{
public:
	sc_windows_fiber_coroutine_system();
	virtual ~sc_windows_fiber_coroutine_system();
	
	virtual sc_coroutine *get_main_coroutine();
	virtual sc_coroutine *create_coroutine(std::size_t stack_size, void (*fn)(intptr_t), intptr_t arg);
private:
	sc_windows_fiber_coroutine *main_coroutine;
};

} // end of namespace sc_core

#endif
