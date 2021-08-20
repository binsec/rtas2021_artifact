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

#ifndef __LIBIEEE1666_TLM1_CHANNELS_FIFO_H__
#define __LIBIEEE1666_TLM1_CHANNELS_FIFO_H__

#include <tlm1/fwd.h>
#include <systemc>

namespace tlm {

//////////////////////////////// declaration //////////////////////////////////

template <typename T>
class tlm_fifo
	: public virtual tlm_fifo_get_if<T>
	, public virtual tlm_fifo_put_if<T>
	, public sc_core::sc_prim_channel
{
public:
	explicit tlm_fifo(int size_ = 1);
	explicit tlm_fifo(const char *name_, int size_ = 1);
	virtual ~tlm_fifo();
	
	T get(tlm_tag<T> *t = 0);
	bool nb_get(T&);
	bool nb_can_get(tlm_tag<T> *t = 0) const;
	const sc_core::sc_event &ok_to_get(tlm_tag<T> *t = 0) const;
	T peek(tlm_tag<T> *t = 0) const;
	bool nb_peek(T&) const;
	bool nb_can_peek(tlm_tag<T> *t = 0) const;
	const sc_core::sc_event& ok_to_peek(tlm_tag<T> *t = 0) const;
	void put(const T&);
	bool nb_put(const T&);
	bool nb_can_put(tlm_tag<T> *t = 0) const;
	const sc_core::sc_event& ok_to_put(tlm_tag<T> *t = 0) const;
	void nb_expand(unsigned int n = 1);
	void nb_unbound(unsigned int n = 16);
	bool nb_reduce(unsigned int n = 1);
	bool nb_bound(unsigned int n);
	bool nb_peek(T &, int n) const;
	bool nb_poke(const T&, int n = 0);
	int used() const;
	int size() const;
	void debug() const;
	const char *kind() const;
};

///////////////////////////////// definition //////////////////////////////////

template <typename T>
tlm_fifo<T>::tlm_fifo(int size_)
{
}

template <typename T>
tlm_fifo<T>::tlm_fifo(const char *name_, int size_)
{
}

template <typename T>
tlm_fifo<T>::~tlm_fifo()
{
}

template <typename T>
T tlm_fifo<T>::get(tlm_tag<T> *t)
{
}

template <typename T>
bool tlm_fifo<T>::nb_get(T&)
{
}

template <typename T>
bool tlm_fifo<T>::nb_can_get(tlm_tag<T> *t) const
{
}

template <typename T>
const sc_core::sc_event& tlm_fifo<T>::ok_to_get(tlm_tag<T> *t) const
{
}

template <typename T>
T tlm_fifo<T>::peek(tlm_tag<T> *t) const
{
}

template <typename T>
bool tlm_fifo<T>::nb_peek(T&) const
{
}

template <typename T>
bool tlm_fifo<T>::nb_can_peek(tlm_tag<T> *t) const
{
}

template <typename T>
const sc_core::sc_event& tlm_fifo<T>::ok_to_peek(tlm_tag<T> *t) const
{
}

template <typename T>
void tlm_fifo<T>::put(const T&)
{
}

template <typename T>
bool tlm_fifo<T>::nb_put(const T&)
{
}

template <typename T>
bool tlm_fifo<T>::nb_can_put(tlm_tag<T> *t) const
{
}

template <typename T>
const sc_core::sc_event& tlm_fifo<T>::ok_to_put(tlm_tag<T> *t) const
{
}

template <typename T>
void tlm_fifo<T>::nb_expand(unsigned int n)
{
}

template <typename T>
void tlm_fifo<T>::nb_unbound(unsigned int n)
{
}

template <typename T>
bool tlm_fifo<T>::nb_reduce(unsigned int n)
{
}

template <typename T>
bool tlm_fifo<T>::nb_bound(unsigned int n)
{
}

template <typename T>
bool tlm_fifo<T>::nb_peek(T &, int n) const
{
}

template <typename T>
bool tlm_fifo<T>::nb_poke(const T&, int n)
{
}

template <typename T>
int tlm_fifo<T>::used() const
{
}

template <typename T>
int tlm_fifo<T>::size() const
{
}

template <typename T>
void tlm_fifo<T>::debug() const
{
}

template <typename T>
const char *tlm_fifo<T>::kind() const
{
}

} // end of namespace tlm

#endif
