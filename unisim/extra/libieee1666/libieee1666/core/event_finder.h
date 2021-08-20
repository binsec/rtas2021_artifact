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

#ifndef __LIBIEEE1666_CORE_EVENT_FINDER_H__
#define __LIBIEEE1666_CORE_EVENT_FINDER_H__

#include "core/fwd.h"
#include "core/port.h"
#include "core/event.h"
#include <stdexcept>

namespace sc_core {

class sc_event_finder // implementation-defined
{
public:
	
protected:
	friend class sc_process;
	friend class sc_port_base;
	friend class sc_kernel;
	
	sc_event_finder(const sc_port_base& _port);

	const sc_port_base& get_port() const;
	const sc_port_base& port() const;
	virtual const sc_event& find_event(sc_interface *_if) const = 0;
private:
	const sc_port_base& base_port;	
};

template <class IF>
class sc_event_finder_t : public sc_event_finder
{
public:
	sc_event_finder_t(const sc_port_base& _port, const sc_event& (IF::*_event_method)() const);
	// Other members
	virtual const sc_event& find_event(sc_interface *_if = 0) const;
private:
	const sc_event& (IF::*event_method)() const;
};

//////////////////////////////////// sc_event_finder_t<> /////////////////////////////////////////////

template <class IF>
sc_event_finder_t<IF>::sc_event_finder_t(const sc_port_base& _port, const sc_event& (IF::*_event_method) () const)
	: sc_event_finder(_port)
	, event_method(_event_method)
{
}

template <class IF>
const sc_event& sc_event_finder_t<IF>::find_event(sc_interface *_if) const
{
	const IF *interf = _if ? dynamic_cast<const IF *>(_if) : dynamic_cast<const IF *>(get_port().get_interface());
	if(!interf) throw std::runtime_error("port is unbound");
	
	return (const_cast<IF *>(interf)->*event_method)();
}

} // end of namespace sc_core

#endif
