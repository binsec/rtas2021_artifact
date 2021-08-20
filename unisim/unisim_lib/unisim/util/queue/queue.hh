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

#ifndef __UNISIM_UTIL_QUEUE_QUEUE_HH__
#define __UNISIM_UTIL_QUEUE_QUEUE_HH__

#include <inttypes.h>
#include <string.h>
#include <iosfwd>
#include <stdexcept>

namespace unisim {
namespace util {
namespace queue {

template <class CONFIG> class Queue;
template <class CONFIG> std::ostream& operator << (std::ostream& os, Queue<CONFIG>& q);

class QueueException : public std::exception
{
public:
	QueueException(const char *reason, const char *filename, const char *func, unsigned int lineno);
	virtual ~QueueException() throw();
	virtual const char * what () const throw ();
private:
	std::string what_str;
};

#if 0
class QueueConfig
{
public:
	static const bool DEBUG = true;
	typedef int *ELEMENT;
	static const unsigned int SIZE = 6;
};
#endif

template <unsigned long long N>
struct Log2
{
    static const unsigned int value = Log2<N / 2>::value + 1;
};

template <>
struct Log2<1>
{
    static const unsigned int value = 0;
};

template <unsigned long long N>
struct CeilLog2
{
    static const unsigned int value = (N > (1 << Log2<N>::value)) ? Log2<N>::value + 1 : Log2<N>::value;
};

template <class CONFIG>
class Queue
{
public:
	static const unsigned int BUFFER_SIZE = 1 << CeilLog2<CONFIG::SIZE>::value;

	Queue();
	~Queue();

	typename CONFIG::ELEMENT& operator [] (unsigned int idx);
	const typename CONFIG::ELEMENT& operator [] (unsigned int idx) const;
	typename CONFIG::ELEMENT *Allocate();
	void Push(typename CONFIG::ELEMENT& elt);
	void Push(typename CONFIG::ELEMENT *elt, unsigned int num_elts);
	typename CONFIG::ELEMENT& Front();
	const typename CONFIG::ELEMENT& ConstFront() const;
	void Pop();
	void Pop(unsigned int num_elts);
	unsigned int Size() const;
	unsigned int Available() const;
	bool Empty() const;
	bool Full() const;
	void Remove(unsigned int idx);
	void Clear();

	friend std::ostream& operator << <CONFIG>(std::ostream& os, Queue<CONFIG>& q);
private:
	unsigned int size;
	unsigned int front_idx;
	unsigned int back_idx;

	typename CONFIG::ELEMENT buffer[BUFFER_SIZE];
};

} // end of namespace queue
} // end of namespace util
} // end of namespace unisim

#endif
