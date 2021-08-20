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

#ifndef __LIBIEEE1666_CHANNELS_INOUT_H__
#define __LIBIEEE1666_CHANNELS_INOUT_H__

#include "core/port.h"
#include "core/event.h"
#include "core/event_finder.h"
#include "channels/fwd.h"
#include "channels/signal_if.h"
#include "utilities/trace_file.h"
#include <string>

namespace sc_core {

template <class T>
class sc_inout : public sc_port<sc_signal_inout_if<T>,1>
{
public:
	sc_inout();
	explicit sc_inout( const char* );
	virtual ~sc_inout();
	void initialize( const T& );
	void initialize( const sc_signal_in_if<T>& );
	virtual void end_of_elaboration();
	const T& read() const;
	operator const T& () const;
	void write( const T& );
	sc_inout<T>& operator= ( const T& );
	sc_inout<T>& operator= ( const sc_signal_in_if<T>& );
	sc_inout<T>& operator= ( const sc_port< sc_signal_in_if<T>, 1>& );
	sc_inout<T>& operator= ( const sc_port< sc_signal_inout_if<T>, 1>& );
	sc_inout<T>& operator= ( const sc_inout<T>& );
	const sc_event& default_event() const;
	const sc_event& value_changed_event() const;
	bool event() const;
	sc_event_finder& value_changed() const;
	virtual const char* kind() const;
private:
	// Disabled
	sc_inout( const sc_inout<T>& );
	
	////////////////////////////////////////////
	sc_event_finder_t<sc_signal_inout_if<T> > *value_changed_event_finder;
};

template <class T>
inline void sc_trace( sc_trace_file*, const sc_inout<T>&, const std::string& );

template <>
class sc_inout<bool> : public sc_port<sc_signal_inout_if<bool>,1>
{
public:
	sc_inout();
	explicit sc_inout( const char* );
	virtual ~sc_inout();
	void initialize( const bool& );
	void initialize( const sc_signal_in_if<bool>& );
	virtual void end_of_elaboration();
	const bool& read() const;
	operator const bool& () const;
	void write( const bool& );
	sc_inout<bool>& operator= ( const bool& );
	sc_inout<bool>& operator= ( const sc_signal_in_if<bool>& );
	sc_inout<bool>& operator= ( const sc_port< sc_signal_in_if<bool>, 1>& );
	sc_inout<bool>& operator= ( const sc_port< sc_signal_inout_if<bool>, 1>& );
	sc_inout<bool>& operator= ( const sc_inout<bool>& );
	const sc_event& default_event() const;
	const sc_event& value_changed_event() const;
	const sc_event& posedge_event() const;
	const sc_event& negedge_event() const;
	bool event() const;
	bool posedge() const;
	bool negedge() const;
	sc_event_finder& value_changed() const;
	sc_event_finder& pos() const;
	sc_event_finder& neg() const;
	virtual const char* kind() const;
private:
	// Disabled
	sc_inout( const sc_inout<bool>& );

	////////////////////////////////////////////
	sc_event_finder_t<sc_signal_inout_if<bool> > *value_changed_event_finder;
	sc_event_finder_t<sc_signal_inout_if<bool> > *posedge_event_finder;
	sc_event_finder_t<sc_signal_inout_if<bool> > *negedge_event_finder;
};

template <>
inline void sc_trace<bool>( sc_trace_file*, const sc_inout<bool>&, const std::string& );

// template <>
// class sc_inout<sc_dt::sc_logic> : public sc_port<sc_signal_inout_if<sc_dt::sc_logic>,1>
// {
// public:
// 	sc_inout();
// 	explicit sc_inout( const char* );
// 	virtual ~sc_inout();
// 	void initialize( const sc_dt::sc_logic& );
// 	void initialize( const sc_signal_in_if<sc_dt::sc_logic>& );
// 	virtual void end_of_elaboration();
// 	const sc_dt::sc_logic& read() const;
// 	operator const sc_dt::sc_logic& () const;
// 	void write( const sc_dt::sc_logic& );
// 	sc_inout<sc_dt::sc_logic>& operator= ( const sc_dt::sc_logic& );
// 	sc_inout<sc_dt::sc_logic>& operator= ( const sc_signal_in_if<sc_dt::sc_logic>& );
// 	sc_inout<sc_dt::sc_logic>& operator= ( const sc_port< sc_signal_in_if<sc_dt::sc_logic>, 1>& );
// 	sc_inout<sc_dt::sc_logic>& operator= ( const sc_port< sc_signal_inout_if<sc_dt::sc_logic>, 1>&);
// 	sc_inout<sc_dt::sc_logic>& operator= ( const sc_inout<sc_dt::sc_logic>& );
// 	const sc_event& default_event() const;
// 	const sc_event& value_changed_event() const;
// 	const sc_event& posedge_event() const;
// 	const sc_event& negedge_event() const;
// 	bool event() const;
// 	bool posedge() const;
// 	bool negedge() const;
// 	sc_event_finder& value_changed() const;
// 	sc_event_finder& pos() const;
// 	sc_event_finder& neg() const;
// 	virtual const char* kind() const;
// private:
// 	// Disabled
// 	sc_inout( const sc_inout<sc_dt::sc_logic>& );
// };
// 
// template <>
// inline void sc_trace<sc_dt::sc_logic>( sc_trace_file*, const sc_inout<sc_dt::sc_logic>&, const std::string& );
// 
// class sc_inout_resolved : public sc_inout<sc_dt::sc_logic>
// {
// public:
// 	sc_inout_resolved();
// 	explicit sc_inout_resolved( const char* );
// 	virtual ~sc_inout_resolved();
// 	virtual void end_of_elaboration();
// 	sc_inout_resolved& operator= ( const sc_dt::sc_logic& );
// 	sc_inout_resolved& operator= ( const sc_signal_in_if<sc_dt::sc_logic>& );
// 	sc_inout_resolved& operator= ( const sc_port<sc_signal_in_if<sc_dt::sc_logic>, 1>& );
// 	sc_inout_resolved& operator= ( const sc_port<sc_signal_inout_if<sc_dt::sc_logic>, 1>& );
// 	sc_inout_resolved& operator= ( const sc_inout_resolved& );
// 	virtual const char* kind() const;
// private:
// 	// Disabled
// 	sc_inout_resolved( const sc_inout_resolved& );
// };
// 
// template <int W>
// class sc_inout_rv : public sc_inout<sc_dt::sc_lv<W>>
// {
// public:
// 	sc_inout_rv();
// 	explicit sc_inout_rv( const char* );
// 	virtual ~sc_inout_rv();
// 	sc_inout_rv<W>& operator= ( const sc_dt::sc_lv<W>& );
// 	sc_inout_rv<W>& operator= ( const sc_signal_in_if<sc_dt::sc_lv<W>>& );
// 	sc_inout_rv<W>& operator= ( const sc_port<sc_signal_in_if<sc_dt::sc_lv<W>>, 1>& );
// 	sc_inout_rv<W>& operator= ( const sc_port<sc_signal_inout_if<sc_dt::sc_lv<W>>, 1>& );
// 	sc_inout_rv<W>& operator= ( const sc_inout_rv<W>& );
// 	virtual void end_of_elaboration();
// 	virtual const char* kind() const;
// private:
// 	// Disabled
// 	sc_inout_rv( const sc_inout_rv<W>& );
// };

////////////////////////////////// sc_inout<> /////////////////////////////////////////////

template <class T>
sc_inout<T>::sc_inout()
	: sc_port<sc_signal_inout_if<T>,1>()
	, value_changed_event_finder(0)
{
	value_changed_event_finder = new sc_event_finder_t<sc_signal_inout_if<T> >(*this, &sc_signal_inout_if<T>::value_changed_event);
}

template <class T>
sc_inout<T>::sc_inout(const char *_name)
	: sc_port<sc_signal_inout_if<T>,1>(_name)
	, value_changed_event_finder(0)
{
	value_changed_event_finder = new sc_event_finder_t<sc_signal_inout_if<T> >(*this, &sc_signal_inout_if<T>::value_changed_event);
}

template <class T>
sc_inout<T>::~sc_inout()
{
	delete value_changed_event_finder;
}

template <class T>
void sc_inout<T>::initialize( const T& )
{
}

template <class T>
void sc_inout<T>::initialize( const sc_signal_in_if<T>& )
{
}

template <class T>
void sc_inout<T>::end_of_elaboration()
{
}

template <class T>
const T& sc_inout<T>::read() const
{
	return (*this)->read();
}

template <class T>
sc_inout<T>::operator const T& () const
{
	return (*this)->read();
}

template <class T>
void sc_inout<T>::write(const T& v)
{
	(*this)->write(v);
}

template <class T>
sc_inout<T>& sc_inout<T>::operator = (const T& v)
{
	(*this)->write(v);
	return *this;
}

template <class T>
sc_inout<T>& sc_inout<T>::operator = (const sc_signal_in_if<T>& _if)
{
	(*this)->write(_if.read());
	return *this;
}

template <class T>
sc_inout<T>& sc_inout<T>::operator = (const sc_port< sc_signal_in_if<T>, 1>& port)
{
	(*this)->write(port->read());
	return *this;
}

template <class T>
sc_inout<T>& sc_inout<T>::operator = (const sc_port< sc_signal_inout_if<T>, 1>& port)
{
	(*this)->write(port->read());
	return *this;
}

template <class T>
sc_inout<T>& sc_inout<T>::operator = (const sc_inout<T>& port)
{
	(*this)->write(port->read());
	return *this;
}

template <class T>
const sc_event& sc_inout<T>::default_event() const
{
	return (*this)->default_event();
}

template <class T>
const sc_event& sc_inout<T>::value_changed_event() const
{
	return (*this)->value_changed_event();
}

template <class T>
bool sc_inout<T>::event() const
{
	return (*this)->event();
}

template <class T>
sc_event_finder& sc_inout<T>::value_changed() const
{
	return *value_changed_event_finder;
}

template <class T>
const char* sc_inout<T>::kind() const
{
	return "sc_inout";
}

// Disabled
template <class T>
sc_inout<T>::sc_inout( const sc_inout<T>& )
{
}

template <class T>
inline void sc_trace( sc_trace_file*, const sc_inout<T>&, const std::string& )
{
}

} // end of namespace sc_core

#endif
