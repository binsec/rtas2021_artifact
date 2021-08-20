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

#include "channels/inout.h"

namespace sc_core {

//////////////////////////////// sc_inout<bool> /////////////////////////////////////////////

sc_inout<bool>::sc_inout()
	: sc_port<sc_signal_inout_if<bool>,1>()
	, value_changed_event_finder(0)
	, posedge_event_finder(0)
	, negedge_event_finder(0)
{
	value_changed_event_finder = new sc_event_finder_t<sc_signal_inout_if<bool> >(*this, &sc_signal_inout_if<bool>::value_changed_event);
	posedge_event_finder = new sc_event_finder_t<sc_signal_inout_if<bool> >(*this, &sc_signal_inout_if<bool>::posedge_event);
	negedge_event_finder = new sc_event_finder_t<sc_signal_inout_if<bool> >(*this, &sc_signal_inout_if<bool>::negedge_event);
}

sc_inout<bool>::sc_inout(const char *_name)
	: sc_port<sc_signal_inout_if<bool>,1>(_name)
	, value_changed_event_finder(0)
	, posedge_event_finder(0)
	, negedge_event_finder(0)
{
	value_changed_event_finder = new sc_event_finder_t<sc_signal_inout_if<bool> >(*this, &sc_signal_inout_if<bool>::value_changed_event);
	posedge_event_finder = new sc_event_finder_t<sc_signal_inout_if<bool> >(*this, &sc_signal_inout_if<bool>::posedge_event);
	negedge_event_finder = new sc_event_finder_t<sc_signal_inout_if<bool> >(*this, &sc_signal_inout_if<bool>::negedge_event);
}

sc_inout<bool>::~sc_inout()
{
	delete value_changed_event_finder;
	delete posedge_event_finder;
	delete negedge_event_finder;
}

void sc_inout<bool>::initialize( const bool& )
{
}

void sc_inout<bool>::initialize( const sc_signal_in_if<bool>& )
{
}

void sc_inout<bool>::end_of_elaboration()
{
}

const bool& sc_inout<bool>::read() const
{
	return (*this)->read();
}

sc_inout<bool>::operator const bool& () const
{
	return (*this)->read();
}

void sc_inout<bool>::write(const bool& v)
{
	(*this)->write(v);
}

sc_inout<bool>& sc_inout<bool>::operator = (const bool& v)
{
	(*this)->write(v);
	return *this;
}

sc_inout<bool>& sc_inout<bool>::operator = (const sc_signal_in_if<bool>& _if)
{
	(*this)->write(_if.read());
	return *this;
}

sc_inout<bool>& sc_inout<bool>::operator = (const sc_port< sc_signal_in_if<bool>, 1>& port)
{
	(*this)->write(port->read());
	return *this;
}

sc_inout<bool>& sc_inout<bool>::operator = (const sc_port< sc_signal_inout_if<bool>, 1>& port)
{
	(*this)->write(port->read());
	return *this;
}

sc_inout<bool>& sc_inout<bool>::operator = (const sc_inout<bool>& port)
{
	(*this)->write(port->read());
	return *this;
}

const sc_event& sc_inout<bool>::default_event() const
{
	return (*this)->default_event();
}

const sc_event& sc_inout<bool>::value_changed_event() const
{
	return (*this)->value_changed_event();
}

const sc_event& sc_inout<bool>::posedge_event() const
{
	return (*this)->posedge_event();
}

const sc_event& sc_inout<bool>::negedge_event() const
{
	return (*this)->negedge_event();
}

bool sc_inout<bool>::event() const
{
	return (*this)->event();
}

bool sc_inout<bool>::posedge() const
{
	return (*this)->posedge();
}

bool sc_inout<bool>::negedge() const
{
	return (*this)->posedge();
}

sc_event_finder& sc_inout<bool>::value_changed() const
{
	return *value_changed_event_finder;
}

sc_event_finder& sc_inout<bool>::pos() const
{
	return *posedge_event_finder;
}

sc_event_finder& sc_inout<bool>::neg() const
{
	return *negedge_event_finder;
}

const char* sc_inout<bool>::kind() const
{
	return "sc_inout";
}

sc_inout<bool>::sc_inout( const sc_inout<bool>& )
{
}

template <>
inline void sc_trace<bool>( sc_trace_file*, const sc_inout<bool>&, const std::string& )
{
}

} // end of namespace sc_core
