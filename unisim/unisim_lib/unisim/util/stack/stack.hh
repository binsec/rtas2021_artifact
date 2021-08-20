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

#ifndef __UNISIM_UTIL_STACK_STACK_HH__
#define __UNISIM_UTIL_STACK_STACK_HH__

#include <inttypes.h>
#include <string.h>
#include <iosfwd>
#include <stdexcept>

namespace unisim {
namespace util {
namespace stack {

template <class CONFIG> class Stack;
template <class CONFIG> std::ostream& operator << (std::ostream& os, Stack<CONFIG>& q);

class StackException : public std::exception
{
public:
	StackException(const char *reason, const char *filename, const char *func, unsigned int lineno);
	virtual ~StackException() throw();
	virtual const char * what () const throw ();
private:
	std::string what_str;
};

#if 0
class StackConfig
{
public:
	static const bool DEBUG = true;
	typedef int *ELEMENT;
	static const unsigned int SIZE = 6;
};
#endif

template <class CONFIG>
class Stack
{
public:
	Stack();
	~Stack();

	typename CONFIG::ELEMENT& operator [] (unsigned int idx);
	const typename CONFIG::ELEMENT& operator [] (unsigned int idx) const;
	typename CONFIG::ELEMENT *Allocate();
	void Push(typename CONFIG::ELEMENT& elt);
	typename CONFIG::ELEMENT& Top();
	const typename CONFIG::ELEMENT& ConstTop() const;
	void Pop();
	unsigned int Size() const;
	bool Empty() const;
	bool Full() const;
	void Clear();

	friend std::ostream& operator << <CONFIG>(std::ostream& os, Stack<CONFIG>& q);
private:
	unsigned int top_idx;

	typename CONFIG::ELEMENT buffer[CONFIG::SIZE];
};

} // end of namespace stack
} // end of namespace util
} // end of namespace unisim

#endif
