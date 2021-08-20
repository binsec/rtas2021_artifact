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

#ifndef __LIBIEEE1666_CHANNELS_FIFO_IN_H__
#define __LIBIEEE1666_CHANNELS_FIFO_IN_H__

#include "core/event.h"
#include "core/event_finder.h"
#include "channels/fwd.h"
#include "channels/fifo_if.h"

namespace sc_core {

template <class T>
class sc_fifo_in : public sc_port<sc_fifo_in_if<T>,0>
{
public:
	sc_fifo_in();
	explicit sc_fifo_in( const char* );
	virtual ~sc_fifo_in();
	void read( T& );
	T read();
	bool nb_read( T& );
	const sc_event& data_written_event() const;
	sc_event_finder& data_written() const;
	int num_available() const;
	virtual const char* kind() const;
private:
	// Disabled
	sc_fifo_in( const sc_fifo_in<T>& );
	sc_fifo_in<T>& operator= ( const sc_fifo_in<T>& );
	////////////////////////////////////////////
	sc_event_finder_t<sc_fifo_in_if<T> > *data_written_event_finder;
};

//////////////////////////////////// sc_fifo_in<> /////////////////////////////////////////////

template <class T>
sc_fifo_in<T>::sc_fifo_in()
	: sc_port<sc_fifo_in_if<T>,0>()
	, data_written_event_finder(0)
{
	data_written_event_finder = new sc_event_finder_t<sc_fifo_in_if<T> >(*this, &sc_fifo_in_if<T>::data_written_event);
}

template <class T>
sc_fifo_in<T>::sc_fifo_in(const char *_name)
	: sc_port<sc_fifo_in_if<T>,0>(_name)
	, data_written_event_finder(0)
{
	data_written_event_finder = new sc_event_finder_t<sc_fifo_in_if<T> >(*this, &sc_fifo_in_if<T>::data_written_event);
}

template <class T>
sc_fifo_in<T>::~sc_fifo_in()
{
	delete data_written_event_finder;
}

template <class T>
void sc_fifo_in<T>::read(T& v)
{
	(*this)->read(v);
}

template <class T>
T sc_fifo_in<T>::read()
{
	return (*this)->read();
}

template <class T>
bool sc_fifo_in<T>::nb_read(T& v)
{
	return (*this)->nb_read(v);
}

template <class T>
const sc_event& sc_fifo_in<T>::data_written_event() const
{
	return (*this)->data_written_event();
}

template <class T>
sc_event_finder& sc_fifo_in<T>::data_written() const
{
	return data_written_event_finder;
}

template <class T>
int sc_fifo_in<T>::num_available() const
{
	return (*this)->num_available();
}

template <class T>
const char* sc_fifo_in<T>::kind() const
{
	return "sc_fifo_in";
}

// disabled
template <class T>
sc_fifo_in<T>::sc_fifo_in(const sc_fifo_in<T>&)
{
}

// disabled
template <class T>
sc_fifo_in<T>& sc_fifo_in<T>::operator = (const sc_fifo_in<T>&)
{
	return *this;
}

} // end of namespace sc_core

#endif
