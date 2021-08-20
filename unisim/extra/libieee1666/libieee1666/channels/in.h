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

#ifndef __LIBIEEE1666_CHANNELS_IN_H__
#define __LIBIEEE1666_CHANNELS_IN_H__

#include "core/port.h"
#include "core/event.h"
#include "core/event_finder.h"
#include "channels/fwd.h"
#include "channels/signal_if.h"
#include "utilities/fwd.h"

namespace sc_core {

template <class T>
class sc_in : public sc_port<sc_signal_in_if<T>,1>
{
public:
	sc_in();
	explicit sc_in( const char* );
	virtual ~sc_in();
	virtual void bind ( const sc_signal_in_if<T>& );
	void operator() ( const sc_signal_in_if<T>& );
	virtual void bind ( sc_port<sc_signal_in_if<T>, 1>& );
	void operator() ( sc_port<sc_signal_in_if<T>, 1>& );
	virtual void bind ( sc_port<sc_signal_inout_if<T>, 1>& );
	void operator() ( sc_port<sc_signal_inout_if<T>, 1>& );
	virtual void end_of_elaboration();
	const T& read() const;
	operator const T& () const;
	const sc_event& default_event() const;
	const sc_event& value_changed_event() const;
	bool event() const;
	sc_event_finder& value_changed() const;
	virtual const char* kind() const;
private:
	// Disabled
	sc_in( const sc_in<T>& );
	sc_in<T>& operator= ( const sc_in<T>& );
	
	////////////////////////////////////////////
	sc_event_finder_t<sc_signal_in_if<T> > *value_changed_event_finder;
};

template <class T>
inline void sc_trace( sc_trace_file*, const sc_in<T>&, const std::string& );

template <>
class sc_in<bool> : public sc_port<sc_signal_in_if<bool>,1>
{
public:
	sc_in();
	explicit sc_in( const char* );
	virtual ~sc_in();
	virtual void bind ( const sc_signal_in_if<bool>& );
	void operator() ( const sc_signal_in_if<bool>& );
	virtual void bind ( sc_port<sc_signal_in_if<bool>, 1>& );
	void operator() ( sc_port<sc_signal_in_if<bool>, 1>& );
	virtual void bind ( sc_port<sc_signal_inout_if<bool>, 1>& );
	void operator() ( sc_port<sc_signal_inout_if<bool>, 1>& );
	virtual void end_of_elaboration();
	const bool& read() const;
	operator const bool& () const;
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
	sc_in( const sc_in<bool>& );
	sc_in<bool>& operator= ( const sc_in<bool>& );

	////////////////////////////////////////////
	sc_event_finder_t<sc_signal_in_if<bool> > *value_changed_event_finder;
	sc_event_finder_t<sc_signal_in_if<bool> > *posedge_event_finder;
	sc_event_finder_t<sc_signal_in_if<bool> > *negedge_event_finder;
};

template <>
inline void sc_trace<bool>( sc_trace_file*, const sc_in<bool>&, const std::string& );

// template <>
// class sc_in<sc_dt::sc_logic> : public sc_port<sc_signal_in_if<sc_dt::sc_logic>,1>
// {
// public:
// 	sc_in();
// 	explicit sc_in( const char* );
// 	virtual ~sc_in();
// 	virtual void bind ( const sc_signal_in_if<sc_dt::sc_logic>& );
// 	void operator() ( const sc_signal_in_if<sc_dt::sc_logic>& );
// 	virtual void bind ( sc_port<sc_signal_in_if<sc_dt::sc_logic>, 1>& );
// 	void operator() ( sc_port<sc_signal_in_if<sc_dt::sc_logic>, 1>& );
// 	virtual void bind ( sc_port<sc_signal_inout_if<sc_dt::sc_logic>, 1>& );
// 	void operator() ( sc_port<sc_signal_inout_if<sc_dt::sc_logic>, 1>& );
// 	virtual void end_of_elaboration();
// 	const sc_dt::sc_logic& read() const;
// 	operator const sc_dt::sc_logic& () const;
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
// 	sc_in( const sc_in<sc_dt::sc_logic>& );
// 	sc_in<sc_dt::sc_logic>& operator= ( const sc_in<sc_dt::sc_logic>& );
// };
// 
// template <>
// inline void sc_trace<sc_dt::sc_logic>( sc_trace_file*, const sc_in<sc_dt::sc_logic>&, const std::string& );
// 
// class sc_in_resolved : public sc_in<sc_dt::sc_logic>
// {
// public:
// 	sc_in_resolved();
// 	explicit sc_in_resolved( const char* );
// 	virtual ~sc_in_resolved();
// 	virtual void end_of_elaboration();
// 	virtual const char* kind() const;
// private:
// 	// Disabled
// 	sc_in_resolved( const sc_in_resolved& );
// 	sc_in_resolved& operator= (const sc_in_resolved& );
// };
// 
// template <int W>
// class sc_in_rv : public sc_in<sc_dt::sc_lv<W>>
// {
// public:
// 	sc_in_rv();
// 	explicit sc_in_rv( const char* );
// 	virtual ~sc_in_rv();
// 	virtual void end_of_elaboration();
// 	virtual const char* kind() const;
// private:
// 	// Disabled
// 	sc_in_rv( const sc_in_rv<W>& );
// 	sc_in_rv<W>& operator= ( const sc_in_rv<W>& );
// };

//////////////////////////////////// sc_in<> /////////////////////////////////////////////

template <class T>
sc_in<T>::sc_in()
	: sc_port<sc_signal_in_if<T>,1>()
	, value_changed_event_finder(0)
{
	value_changed_event_finder = new sc_event_finder_t<sc_signal_in_if<T> >(*this, &sc_signal_in_if<T>::value_changed_event);
}

template <class T>
sc_in<T>::sc_in(const char* _name)
	: sc_port<sc_signal_in_if<T>,1>(_name)
	, value_changed_event_finder(0)
{
	value_changed_event_finder = new sc_event_finder_t<sc_signal_in_if<T> >(*this, &sc_signal_in_if<T>::value_changed_event);
}

template <class T>
sc_in<T>::~sc_in()
{
	delete value_changed_event_finder;
}

template <class T>
void sc_in<T>::bind(const sc_signal_in_if<T>& _if)
{
	sc_port_base::bind(const_cast<sc_signal_in_if<T>&>(_if));
}

template <class T>
void sc_in<T>::operator() (const sc_signal_in_if<T>& _if)
{
	sc_in<T>::bind(_if);
}

template <class T>
void sc_in<T>::bind(sc_port<sc_signal_in_if<T>, 1>& port)
{
	sc_port_base::bind(port);
}

template <class T>
void sc_in<T>::operator() (sc_port<sc_signal_in_if<T>, 1>& port)
{
	sc_in<T>::bind(port);
}

template <class T>
void sc_in<T>::bind(sc_port<sc_signal_inout_if<T>, 1>& port)
{
	sc_port_base::bind(port);
}

template <class T>
void sc_in<T>::operator() (sc_port<sc_signal_inout_if<T>, 1>& port)
{
	sc_in<T>::bind(port);
}

template <class T>
void sc_in<T>::end_of_elaboration()
{
}

template <class T>
const T& sc_in<T>::read() const
{
	return (*this)->read();
}

template <class T>
sc_in<T>::operator const T& () const
{
	return (*this)->read();
}

template <class T>
const sc_event& sc_in<T>::default_event() const
{
	return (*this)->default_event();
}

template <class T>
const sc_event& sc_in<T>::value_changed_event() const
{
	return (*this)->value_changed_event();
}

template <class T>
bool sc_in<T>::event() const
{
	return (*this)->event();
}

template <class T>
sc_event_finder& sc_in<T>::value_changed() const
{
	return *value_changed_event_finder;
}

template <class T>
const char* sc_in<T>::kind() const
{
	return "sc_in";
}

// Disabled
template <class T>
sc_in<T>::sc_in( const sc_in<T>& )
{
}

// Disabled
template <class T>
sc_in<T>& sc_in<T>::operator= ( const sc_in<T>& )
{
}

template <class T>
inline void sc_trace( sc_trace_file*, const sc_in<T>&, const std::string& )
{
}

} // end of namespace sc_core

#endif
