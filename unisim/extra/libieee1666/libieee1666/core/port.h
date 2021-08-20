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

#ifndef __LIBIEEE1666_CORE_PORT_H__
#define __LIBIEEE1666_CORE_PORT_H__

#include "core/fwd.h"
#include "core/object.h"
#include "core/interface.h"
#include <list>
#include <typeinfo>

namespace sc_core {

enum bind_type_t
{
	PORT_BIND_TO_OUTER_PORT,
	PORT_BIND_TO_INTERFACE
};

enum static_sensitivity_type_t
{
	PROCESS_STATICALLY_SENSITIVE_TO_PORT,
	PROCESS_STATICALLY_SENSITIVE_TO_EVENT_FINDER
};

struct sc_port_binding
{
	bind_type_t type;
	sc_port_base *bound_outer_port;   // only used if type == PORT_BIND_TO_OUTER_PORT
	sc_interface *bound_interface;    // only used if type == PORT_BIND_TO_INTERFACE
	
	sc_port_binding(sc_port_base *outer_port);
	sc_port_binding(sc_interface *_if);
};

struct sc_process_static_sensitivity
{
	static_sensitivity_type_t type;
	sc_process *process;              
	const sc_event_finder *event_finder;    // only used if type == PROCESS_STATICALLY_SENSITIVE_TO_EVENT_FINDER
	
	sc_process_static_sensitivity(sc_process *process);
	sc_process_static_sensitivity(sc_process *process, const sc_event_finder *event_finder);
};

class sc_port_base : public sc_object
{
protected:
	virtual void before_end_of_elaboration();
	virtual void end_of_elaboration();
	virtual void start_of_simulation();
	virtual void end_of_simulation();

	//////////////////////////////////////////
public: // this is public for event_finder_t<T>::find_event
	virtual sc_interface *get_interface() const = 0;
	virtual sc_interface *get_interface(int if_idx) const = 0;
protected:
	sc_port_base(const char *name, int N, sc_port_policy P);
	sc_port_base(int N, sc_port_policy P);
	virtual ~sc_port_base();
	
	void bind(sc_interface& _if);
	void bind(sc_port_base& port);
	virtual const char *get_interface_typename() const = 0;
	virtual void add_interface(sc_interface *) = 0;
	virtual int get_size() = 0;
private:
	friend class sc_kernel;
	friend class sc_process;
	friend class sc_bind_proxy;

//	std::vector<sc_interface *> interfaces;                     // interfaces as of simulation time

	int max_bindings;
	sc_port_policy port_policy;
	bool terminal_inner_port;                                   // true if not bound to an outer ports (may change during elaboration)
	mutable std::list<sc_port_binding *> port_bindings;         // port binding as of elaboration time
	mutable std::vector<sc_process_static_sensitivity *> processes_static_sensitivity; // process sensitivity as of elaboration time
	bool elaboration_finalized;
	
	void add_process_statically_sensitive_to_port(sc_process *process) const;
	void add_process_statically_sensitive_to_event_finder(sc_process *process, const sc_event_finder& event_finder) const;
	
	void finalize_elaboration();
};

template <class IF>
class sc_port_b : public sc_port_base
{
public:
	void operator() (IF&);
	void operator() (sc_port_b<IF>&);
	virtual void bind(IF&);
	virtual void bind(sc_port_b<IF>&);
	int size() const;
	IF *operator -> ();
	const IF *operator -> () const;
	IF *operator [] (int);
	const IF *operator [] (int) const;

protected:
	explicit sc_port_b(int, sc_port_policy);
	sc_port_b(const char *, int, sc_port_policy);
	virtual ~sc_port_b();
private:
	// Disabled
	sc_port_b();
	sc_port_b(const sc_port_b<IF>&);
	sc_port_b<IF>& operator = (const sc_port_b<IF>&);
	
	//////////////////////////////////////////////////////
	std::vector<IF *> interfaces;

	virtual const char *get_interface_typename() const;
protected:
	virtual void add_interface(sc_interface *);
	virtual int get_size();
public:
	virtual sc_interface *get_interface() const;
	virtual sc_interface *get_interface(int if_idx) const;
};

template <class IF, int N = 1, sc_port_policy P = SC_ONE_OR_MORE_BOUND>
class sc_port : public sc_port_b<IF>
{
public:
	sc_port();
	explicit sc_port(const char *);
	virtual ~sc_port();
	virtual const char *kind() const;
private:
	// Disabled
	sc_port(const sc_port<IF,N,P>&);
	sc_port<IF,N,P>& operator = (const sc_port<IF,N,P>&);
};

//////////////////////////////////// sc_port_b<> /////////////////////////////////////////////

template <class IF>
void sc_port_b<IF>::operator () (IF& _if)
{
	bind(_if);
}

template <class IF>
void sc_port_b<IF>::operator () (sc_port_b<IF>& port)
{
	bind(port);
}

template <class IF>
void sc_port_b<IF>::bind(IF& itf)
{
	sc_port_base::bind(itf);
}

template <class IF>
void sc_port_b<IF>::bind(sc_port_b<IF>& port)
{
	sc_port_base::bind(port);
}

template <class IF>
int sc_port_b<IF>::size() const
{
	return interfaces.size();
}

template <class IF>
IF* sc_port_b<IF>::operator-> ()
{
	return interfaces[0];
}

template <class IF>
const IF* sc_port_b<IF>::operator-> () const
{
	return interfaces[0];
}

template <class IF>
IF* sc_port_b<IF>::operator[] (int if_idx)
{
	return interfaces[if_idx];
}

template <class IF>
const IF* sc_port_b<IF>::operator[] (int if_idx) const
{
	return interfaces[if_idx];
}

template <class IF>
sc_interface *sc_port_b<IF>::get_interface() const
{
	return interfaces[0];
}

template <class IF>
sc_interface *sc_port_b<IF>::get_interface(int if_idx) const
{
	return interfaces[if_idx];
}

template <class IF>
sc_port_b<IF>::sc_port_b(int N, sc_port_policy P)
	: sc_port_base(N, P)
	, interfaces()
{
}

template <class IF>
sc_port_b<IF>::sc_port_b(const char *_name, int N, sc_port_policy P)
	: sc_port_base(_name, N, P)
	, interfaces()
{
}

template <class IF>
sc_port_b<IF>::~sc_port_b()
{
}

// disabled
template <class IF>
sc_port_b<IF>::sc_port_b()
{
}

// disabled
template <class IF>
sc_port_b<IF>::sc_port_b( const sc_port_b<IF>& )
{
}

// disabled
template <class IF>
sc_port_b<IF>& sc_port_b<IF>::operator = ( const sc_port_b<IF>& )
{
}

template <class IF>
const char *sc_port_b<IF>::get_interface_typename() const
{
	return typeid(IF).name();
}

template <class IF>
void sc_port_b<IF>::add_interface(sc_interface *_if)
{
	interfaces.push_back(dynamic_cast<IF *>(_if));
}

template <class IF>
int sc_port_b<IF>::get_size()
{
	return interfaces.size();
}

//////////////////////////////////// sc_port<> /////////////////////////////////////////////

template <class IF, int N, sc_port_policy P>
sc_port<IF, N, P>::sc_port()
	: sc_port_b<IF>(N, P)
{
}

template <class IF, int N, sc_port_policy P>
sc_port<IF, N, P>::sc_port(const char *_name)
	: sc_port_b<IF>(_name, N, P)
{
}

template <class IF, int N, sc_port_policy P>
sc_port<IF, N, P>::~sc_port()
{
}

template <class IF, int N, sc_port_policy P>
const char* sc_port<IF, N, P>::kind() const
{
	return "sc_port";
}

template <class IF, int N, sc_port_policy P>
sc_port<IF, N, P>::sc_port( const sc_port<IF,N,P>& )
{
}

template <class IF, int N, sc_port_policy P>
sc_port<IF,N,P>& sc_port<IF, N, P>::operator= ( const sc_port<IF,N,P>& )
{
}

} // end of namespace sc_core

#endif
