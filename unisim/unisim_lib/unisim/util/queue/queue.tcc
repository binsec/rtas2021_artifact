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

#ifndef __UNISIM_UTIL_QUEUE_QUEUE_TCC__
#define __UNISIM_UTIL_QUEUE_QUEUE_TCC__

#include <iostream>
#include <string.h>

namespace unisim {
namespace util {
namespace queue {

template <class CONFIG>
Queue<CONFIG>::Queue()
{
	front_idx = 0;
	back_idx = 0;
	size = 0;
}

template <class CONFIG>
Queue<CONFIG>::~Queue()
{
}

template <class CONFIG>
void Queue<CONFIG>::Clear()
{
	front_idx = 0;
	back_idx = 0;
	size = 0;
}

template <class CONFIG>
const typename CONFIG::ELEMENT& Queue<CONFIG>::operator [] (unsigned int idx) const
{
	if(CONFIG::DEBUG && idx >= size) throw QueueException("out of range access", __FILE__, __FUNCTION__, __LINE__);
	return buffer[(front_idx + idx) & (BUFFER_SIZE - 1)];
}

template <class CONFIG>
typename CONFIG::ELEMENT& Queue<CONFIG>::operator [] (unsigned int idx)
{
	if(CONFIG::DEBUG && idx >= size) throw QueueException("out of range access", __FILE__, __FUNCTION__, __LINE__);
	return buffer[(front_idx + idx) & (BUFFER_SIZE - 1)];
}

template <class CONFIG>
typename CONFIG::ELEMENT *Queue<CONFIG>::Allocate()
{
	if(CONFIG::DEBUG && size >= CONFIG::SIZE) throw QueueException("overflow", __FILE__, __FUNCTION__, __LINE__);
	typename CONFIG::ELEMENT *elt = &buffer[back_idx];
	back_idx = (back_idx + 1) & (BUFFER_SIZE - 1);
	size++;
	return elt;
}

template <class CONFIG>
void Queue<CONFIG>::Push(typename CONFIG::ELEMENT& elt)
{
	if(CONFIG::DEBUG && size >= CONFIG::SIZE) throw QueueException("overflow", __FILE__, __FUNCTION__, __LINE__);
	buffer[back_idx] = elt;
	back_idx = (back_idx + 1) & (BUFFER_SIZE - 1);
	size++;
}

template <class CONFIG>
void Queue<CONFIG>::Push(typename CONFIG::ELEMENT *elt, unsigned int num_elts)
{
	if(num_elts)
	{
		do
		{
			Push(*elt);
		}
		while(++elt, --num_elts);
	}
}

template <class CONFIG>
typename CONFIG::ELEMENT& Queue<CONFIG>::Front()
{
	return buffer[front_idx];
}

template <class CONFIG>
const typename CONFIG::ELEMENT& Queue<CONFIG>::ConstFront() const
{
	return buffer[front_idx];
}

template <class CONFIG>
void Queue<CONFIG>::Pop()
{
	if(CONFIG::DEBUG && size == 0) throw QueueException("underflow", __FILE__, __FUNCTION__, __LINE__);
	front_idx = (front_idx + 1) & (BUFFER_SIZE - 1);
	size--;
}

template <class CONFIG>
void Queue<CONFIG>::Pop(unsigned int num_elts)
{
	if(num_elts)
	{
		do
		{
			Pop();
		}
		while(--num_elts);
	}
}

template <class CONFIG>
unsigned int Queue<CONFIG>::Size() const
{
	return size;
}

template <class CONFIG>
unsigned int Queue<CONFIG>::Available() const
{
	return CONFIG::SIZE - size;
}

template <class CONFIG>
bool Queue<CONFIG>::Empty() const
{
	return size == 0;
}

template <class CONFIG>
bool Queue<CONFIG>::Full() const
{
	return size >= CONFIG::SIZE;
}

template <class CONFIG>
void Queue<CONFIG>::Remove(unsigned int idx)
{
	unsigned int i, j;
	
	if(CONFIG::DEBUG && idx >= size) throw QueueException("out of range access", __FILE__, __FUNCTION__, __LINE__);
	if(CONFIG::DEBUG && size == 0) throw QueueException("underflow", __FILE__, __FUNCTION__, __LINE__);
	for(i = (front_idx + idx) & (BUFFER_SIZE - 1), j = (i + 1) & (BUFFER_SIZE - 1); j != back_idx; i = (i + 1) & (BUFFER_SIZE - 1), j = (j + 1) & (BUFFER_SIZE - 1))
	{
		buffer[i] = buffer[j];
	}
	back_idx = (back_idx - 1) & (BUFFER_SIZE - 1);
	size--;
}

template <class CONFIG>
std::ostream& operator << (std::ostream& os, Queue<CONFIG>& q)
{
	unsigned int idx;
	for(idx = q.front_idx; idx != q.back_idx; idx = (idx + 1) & (Queue<CONFIG>::BUFFER_SIZE - 1))
	{
		os << q.buffer[idx] << " ";
	}
	os << std::endl;
	return os;
}

} // end of namespace queue
} // end of namespace util
} // end of namespace unisim

#endif
