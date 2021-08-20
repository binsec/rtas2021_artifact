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

#ifndef __LIBIEEE1666_CORE_ALLOCATOR_H__
#define __LIBIEEE1666_CORE_ALLOCATOR_H__

#include <stack>
#include <vector>

namespace sc_core {

template <class T>
class sc_allocator
{
public:
	sc_allocator();
	~sc_allocator();

	T *allocate();
	void free(T *e);

private:
	std::stack<T *, std::vector<T *> > free_list;
};

template <class T>
sc_allocator<T>::sc_allocator()
	: free_list()
{
}

template <class T>
sc_allocator<T>::~sc_allocator()
{
	while(!free_list.empty())
	{
		T *e = free_list.top();
		delete e;
		free_list.pop();
	}
}

template <class T>
T *sc_allocator<T>::allocate()
{
	if(!free_list.empty())
	{
		T *e = free_list.top();
		free_list.pop();
		return e;
	}
	
	return new T();
}

template <class T>
void sc_allocator<T>::free(T *e)
{
	free_list.push(e);
}

} // end of namespace sc_core

#endif // __LIBIEEE1666_CORE_ALLOCATOR_H__
