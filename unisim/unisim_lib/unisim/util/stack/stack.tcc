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

#ifndef __UNISIM_UTIL_STACK_STACK_TCC__
#define __UNISIM_UTIL_STACK_STACK_TCC__

#include <iostream>

namespace unisim {
namespace util {
namespace stack {

template <class CONFIG>
Stack<CONFIG>::Stack()
{
	top_idx = CONFIG::SIZE;
}

template <class CONFIG>
Stack<CONFIG>::~Stack()
{
}

template <class CONFIG>
void Stack<CONFIG>::Clear()
{
	top_idx = CONFIG::SIZE;
}

template <class CONFIG>
const typename CONFIG::ELEMENT& Stack<CONFIG>::operator [] (unsigned int idx) const
{
	unsigned int subscript = top_idx + idx;
	if(CONFIG::DEBUG && subscript >= CONFIG::SIZE) throw StackException("out of range access", __FILE__, __FUNCTION__, __LINE__);
	return buffer[subscript];
}

template <class CONFIG>
typename CONFIG::ELEMENT& Stack<CONFIG>::operator [] (unsigned int idx)
{
	unsigned int subscript = top_idx + idx;
	if(CONFIG::DEBUG && subscript >= CONFIG::SIZE) throw StackException("out of range access", __FILE__, __FUNCTION__, __LINE__);
	return buffer[subscript];
}

template <class CONFIG>
typename CONFIG::ELEMENT *Stack<CONFIG>::Allocate()
{
	if(CONFIG::DEBUG && !top_idx) throw StackException("overflow", __FILE__, __FUNCTION__, __LINE__);
	top_idx--;
	return &buffer[top_idx];
}

template <class CONFIG>
void Stack<CONFIG>::Push(typename CONFIG::ELEMENT& elt)
{
	if(CONFIG::DEBUG && !top_idx) throw StackException("overflow", __FILE__, __FUNCTION__, __LINE__);
	top_idx--;
	buffer[top_idx] = elt;
}

template <class CONFIG>
typename CONFIG::ELEMENT& Stack<CONFIG>::Top()
{
	return buffer[top_idx];
}

template <class CONFIG>
const typename CONFIG::ELEMENT& Stack<CONFIG>::ConstTop() const
{
	return buffer[top_idx];
}

template <class CONFIG>
void Stack<CONFIG>::Pop()
{
	if(CONFIG::DEBUG && (top_idx >= CONFIG::SIZE)) throw StackException("underflow", __FILE__, __FUNCTION__, __LINE__);
	top_idx++;
}

template <class CONFIG>
unsigned int Stack<CONFIG>::Size() const
{
	return CONFIG::SIZE - top_idx;
}

template <class CONFIG>
bool Stack<CONFIG>::Empty() const
{
	return top_idx >= CONFIG::SIZE;
}

template <class CONFIG>
bool Stack<CONFIG>::Full() const
{
	return top_idx == 0;
}

template <class CONFIG>
std::ostream& operator << (std::ostream& os, Stack<CONFIG>& q)
{
	unsigned int idx;
	for(idx = q.top_idx; idx < CONFIG::SIZE; idx++)
	{
		os << q.buffer[idx] << " ";
	}
	os << std::endl;
	return os;
}

} // end of namespace stack
} // end of namespace util
} // end of namespace unisim

#endif
