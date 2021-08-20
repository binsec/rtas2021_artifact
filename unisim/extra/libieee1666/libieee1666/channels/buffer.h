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

#ifndef __LIBIEEE1666_CHANNELS_BUFFER_H__
#define __LIBIEEE1666_CHANNELS_BUFFER_H__

#include "channels/fwd.h"
#include "channels/signal.h"
#include "channels/buffer.h"

namespace sc_core {

template <class T, sc_writer_policy WRITER_POLICY>
class sc_buffer : public sc_signal<T,WRITER_POLICY>
{
public:
	sc_buffer();
	explicit sc_buffer(const char *);
	virtual void write(const T&);
	sc_buffer<T,WRITER_POLICY>& operator = (const T&);
	sc_buffer<T,WRITER_POLICY>& operator = (const sc_signal<T,WRITER_POLICY>&);
	sc_buffer<T,WRITER_POLICY>& operator = (const sc_buffer<T,WRITER_POLICY>&);
	virtual const char *kind() const;
protected:
	virtual void update();
private:
	// Disabled
	sc_buffer(const sc_buffer<T,WRITER_POLICY>&);
};

//////////////////////////////////// sc_buffer<> /////////////////////////////////////////////

template <class T, sc_writer_policy WRITER_POLICY>
sc_buffer<T, WRITER_POLICY>::sc_buffer()
	: sc_signal<T, WRITER_POLICY>(sc_gen_unique_name("buffer"))
{
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_buffer<T, WRITER_POLICY>::sc_buffer(const char *_name)
	: sc_signal<T, WRITER_POLICY>(_name)
{
}

template <class T, sc_writer_policy WRITER_POLICY>
void sc_buffer<T, WRITER_POLICY>::write(const T& v)
{
	if(!sc_signal<T,WRITER_POLICY>::check_write())
	{
		throw std::runtime_error("sc_buffer<T, SC_ONE_WRITER> with multiple writers");
	}
	
	sc_signal<T,WRITER_POLICY>::new_value() = v;
	sc_signal<T,WRITER_POLICY>::request_update();
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_buffer<T,WRITER_POLICY>& sc_buffer<T, WRITER_POLICY>::operator = (const T& arg)
{
	write(arg);
	return *this;
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_buffer<T,WRITER_POLICY>& sc_buffer<T, WRITER_POLICY>::operator = (const sc_signal<T,WRITER_POLICY>& arg)
{
	write(arg.read());
	return *this;
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_buffer<T,WRITER_POLICY>& sc_buffer<T, WRITER_POLICY>::operator = (const sc_buffer<T,WRITER_POLICY>& arg)
{
	write(arg.read());
	return *this;
}

template <class T, sc_writer_policy WRITER_POLICY>
const char* sc_buffer<T, WRITER_POLICY>::kind() const
{
	return "sc_buffer";
}

template <class T, sc_writer_policy WRITER_POLICY>
void sc_buffer<T, WRITER_POLICY>::update()
{
	sc_signal<T,WRITER_POLICY>::do_update();
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_buffer<T, WRITER_POLICY>::sc_buffer(const sc_buffer<T,WRITER_POLICY>&)
{
}

} // end of namespace sc_core

#endif
