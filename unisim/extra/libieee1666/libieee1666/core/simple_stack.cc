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

#include "core/simple_stack.h"

#if __LIBIEEE1666_VALGRIND__
#include <valgrind/valgrind.h>
#endif

#include <stdlib.h>
#include <stdexcept>

namespace sc_core {

sc_simple_stack::sc_simple_stack(std::size_t _stack_size)
	: stack_size(_stack_size)
	, buffer(0)
#if __LIBIEEE1666_VALGRIND__
	, valgrind_stack_id(0)
#endif
{
	buffer = ::calloc(stack_size, sizeof(char));
	
	if(!buffer) throw std::bad_alloc();
	
#if __LIBIEEE1666_VALGRIND__
	valgrind_stack_id = VALGRIND_STACK_REGISTER(buffer, reinterpret_cast<char *>(buffer) + stack_size);
#endif
}

sc_simple_stack::~sc_simple_stack()
{
#if __LIBIEEE1666_VALGRIND__
	VALGRIND_STACK_DEREGISTER(valgrind_stack_id);
#endif
	if(buffer)
	{
		free(buffer);
	}
}

void *sc_simple_stack::get_top_of_the_stack() const
{
	return reinterpret_cast<void *>(reinterpret_cast<char *>(buffer) + stack_size);
}

void *sc_simple_stack::get_stack_base() const
{
	return buffer;
}

sc_stack *sc_simple_stack_system::create_stack(std::size_t stack_size)
{
	return new sc_simple_stack(stack_size);
}

} // end of namespace sc_core
