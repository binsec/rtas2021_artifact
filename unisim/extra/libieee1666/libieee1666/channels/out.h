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

#ifndef __LIBIEEE1666_CHANNELS_OUT_H__
#define __LIBIEEE1666_CHANNELS_OUT_H__

#include "core/port.h"
#include "channels/fwd.h"
#include "channels/inout.h"
#include "channels/signal_if.h"

namespace sc_core {

template <class T>
class sc_out : public sc_inout<T>
{
public:
	sc_out();
	explicit sc_out( const char* );
	virtual ~sc_out();
	sc_out<T>& operator= ( const T& );
	sc_out<T>& operator= ( const sc_signal_in_if<T>& );
	sc_out<T>& operator= ( const sc_port< sc_signal_in_if<T>, 1>& );
	sc_out<T>& operator= ( const sc_port< sc_signal_inout_if<T>, 1>& );
	sc_out<T>& operator= ( const sc_out<T>& );
	virtual const char* kind() const;
private:
	// Disabled
	sc_out( const sc_out<T>& );
};

// class sc_out_resolved : public sc_inout_resolved
// {
// public:
// 	sc_out_resolved();
// 	explicit sc_out_resolved( const char* );
// 	virtual ~sc_out_resolved();
// 	sc_out_resolved& operator= ( const sc_dt::sc_logic& );
// 	sc_out_resolved& operator= ( const sc_signal_in_if<sc_dt::sc_logic>& );
// 	sc_out_resolved& operator= ( const sc_port<sc_signal_in_if<sc_dt::sc_logic>, 1>& );
// 	sc_out_resolved& operator= ( const sc_port<sc_signal_inout_if<sc_dt::sc_logic>, 1>& );
// 	sc_out_resolved& operator= ( const sc_out_resolved& );
// 	virtual const char* kind() const;
// private:
// 	// Disabled
// 	sc_out_resolved( const sc_out_resolved& );
// };
// 
// template <int W>
// class sc_out_rv : public sc_inout_rv<W>
// {
// public:
// 	sc_out_rv();
// 	explicit sc_out_rv( const char* );
// 	virtual ~sc_out_rv();
// 	sc_out_rv<W>& operator= ( const sc_dt::sc_lv<W>& );
// 	sc_out_rv<W>& operator= ( const sc_signal_in_if<sc_dt::sc_lv<W>>& );
// 	sc_out_rv<W>& operator= ( const sc_port<sc_signal_in_if<sc_dt::sc_lv<W>>, 1>& );
// 	sc_out_rv<W>& operator= ( const sc_port<sc_signal_inout_if<sc_dt::sc_lv<W>>, 1>& );
// 	sc_out_rv<W>& operator= ( const sc_out_rv<W>& );
// 	virtual const char* kind() const;
// private:
// 	// Disabled
// 	sc_out_rv( const sc_out_rv<W>& );
// };

//////////////////////////////////// sc_out<> /////////////////////////////////////////////

template <class T>
sc_out<T>::sc_out()
	: sc_inout<T>()
{
}

template <class T>
sc_out<T>::sc_out(const char *_name)
	: sc_inout<T>(_name)
{
}

template <class T>
sc_out<T>::~sc_out()
{
}

template <class T>
sc_out<T>& sc_out<T>::operator = (const T& v)
{
	(*this)->write(v);
	return *this;
}

template <class T>
sc_out<T>& sc_out<T>::operator = (const sc_signal_in_if<T>& _if)
{
	(*this)->write(_if->read());
	return *this;
}

template <class T>
sc_out<T>& sc_out<T>::operator = (const sc_port< sc_signal_in_if<T>, 1>& port)
{
	(*this)->write(port->read());
	return *this;
}

template <class T>
sc_out<T>& sc_out<T>::operator = (const sc_port< sc_signal_inout_if<T>, 1>& port)
{
	(*this)->write(port->read());
	return *this;
}

template <class T>
sc_out<T>& sc_out<T>::operator = (const sc_out<T>& port)
{
	(*this)->write(port->read());
	return *this;
}

template <class T>
const char* sc_out<T>::kind() const
{
	return "sc_out";
}

// Disabled
template <class T>
sc_out<T>::sc_out( const sc_out<T>& )
{
}

} // end of namespace sc_core

#endif
