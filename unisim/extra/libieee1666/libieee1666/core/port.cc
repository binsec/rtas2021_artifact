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

#include "core/port.h"
#include "core/kernel.h"
#include "core/process.h"
#include "core/event_finder.h"

namespace sc_core {

sc_port_binding::sc_port_binding(sc_port_base *outer_port)
	: type(PORT_BIND_TO_OUTER_PORT)
	, bound_outer_port(outer_port)
	, bound_interface(0)
{
}

sc_port_binding::sc_port_binding(sc_interface *_if)
	: type(PORT_BIND_TO_INTERFACE)
	, bound_outer_port(0)
	, bound_interface(_if)
{
}

sc_process_static_sensitivity::sc_process_static_sensitivity(sc_process *_process)
	: type(PROCESS_STATICALLY_SENSITIVE_TO_PORT)
	, process(_process)
	, event_finder(0)
{
}

sc_process_static_sensitivity::sc_process_static_sensitivity(sc_process *_process, const sc_event_finder *_event_finder)
	: type(PROCESS_STATICALLY_SENSITIVE_TO_EVENT_FINDER)
	, process(_process)
	, event_finder(_event_finder)
{
}

//////////////////////////////////// sc_port_base /////////////////////////////////////////////

sc_port_base::sc_port_base(const char *_name, int N, sc_port_policy P)
	: sc_object(_name)
	, max_bindings(N)
	, port_policy(P)
	, terminal_inner_port(true)
	, port_bindings()
	, processes_static_sensitivity()
	, elaboration_finalized(false)
{
	kernel->register_port(this);
}

sc_port_base::sc_port_base(int N, sc_port_policy P)
	: sc_object(sc_gen_unique_name("port"))
	, max_bindings(N)
	, port_policy(P)
	, terminal_inner_port(true)
	, port_bindings()
	, processes_static_sensitivity()
	, elaboration_finalized(false)
{
	kernel->register_port(this);
}

sc_port_base::~sc_port_base()
{
	kernel->unregister_port(this);
}

// Note: in IEEE1666-2011 callbacks are misplaced (in sc_port_b<IF>) because kernel don't known user interface passed as template arguments !

void sc_port_base::before_end_of_elaboration()
{
}

void sc_port_base::end_of_elaboration()
{
}

void sc_port_base::start_of_simulation()
{
}

void sc_port_base::end_of_simulation()
{
}

// sc_interface *sc_port_base::get_interface()
// {
// 	return interfaces[0];
// }
// 
// const sc_interface *sc_port_base::get_interface() const
// {
// 	return interfaces[0];
// }

void sc_port_base::bind(sc_interface& _if)
{
	sc_port_binding *port_binding = new sc_port_binding(&_if);
	port_bindings.push_back(port_binding);
}

void sc_port_base::bind(sc_port_base& outer_port)
{
	sc_port_binding *port_binding = new sc_port_binding(&outer_port);
	port_bindings.push_back(port_binding);
	outer_port.terminal_inner_port = false;
}

void sc_port_base::add_process_statically_sensitive_to_port(sc_process *process) const
{
	sc_process_static_sensitivity *process_static_sensitivity = new sc_process_static_sensitivity(process);
	processes_static_sensitivity.push_back(process_static_sensitivity);
}

void sc_port_base::add_process_statically_sensitive_to_event_finder(sc_process *process, const sc_event_finder& event_finder) const
{
	sc_process_static_sensitivity *process_static_sensitivity = new sc_process_static_sensitivity(process, &event_finder);
	processes_static_sensitivity.push_back(process_static_sensitivity);
}

void sc_port_base::finalize_elaboration()
{
	if(elaboration_finalized) return;
	
	int i, j;
	
	std::list<sc_port_binding *>::iterator it;
	
	for(it = port_bindings.begin(); it != port_bindings.end(); it++)
	{
		sc_port_binding *port_binding = *it;
		
		switch(port_binding->type)
		{
			case PORT_BIND_TO_OUTER_PORT:
				{
					sc_port_base *outer_port = port_binding->bound_outer_port;
					outer_port->finalize_elaboration();
					int num_inherited_interfaces = outer_port->get_size();
					for(i = 0; i < num_inherited_interfaces; i++)
					{
						add_interface(outer_port->get_interface(i));
					}
				}
				break;
			case PORT_BIND_TO_INTERFACE:
				add_interface(port_binding->bound_interface);
				break;
		}
		
		delete port_binding; // free unused resource
	}
	
	port_bindings.clear();

	if(terminal_inner_port)
	{
		int num_interfaces = get_size();
		for(i = 0; i < num_interfaces; i++)
		{
			sc_interface *itf = get_interface(i);
			
			itf->register_port(*this, get_interface_typename());
		}
	}

	int num_interfaces = get_size();
	
	switch(port_policy)
	{
		case SC_ONE_OR_MORE_BOUND:
			if(num_interfaces < 1) throw std::runtime_error("port is unbound");
			if(max_bindings && (num_interfaces > max_bindings)) throw std::runtime_error("too many bindings");
			break;
		case SC_ZERO_OR_MORE_BOUND:
			if(max_bindings && (num_interfaces > max_bindings)) throw std::runtime_error("too many bindings");
			break;
		case SC_ALL_BOUND:
			if(num_interfaces < (max_bindings ? max_bindings : 1)) throw std::runtime_error("too few bindings");
			break;
	}
	
	int num_process_static_sensitivity = processes_static_sensitivity.size();

	for(i = 0; i < num_interfaces; i++)
	{
		sc_interface *itf = get_interface(i);
		
		for(j = 0; j < num_process_static_sensitivity; j++)
		{
			sc_process_static_sensitivity *process_static_sensitivity = processes_static_sensitivity[j];
			sc_process *process = process_static_sensitivity->process;
			
			switch(process_static_sensitivity->type)
			{
				case PROCESS_STATICALLY_SENSITIVE_TO_PORT:
					{
						const sc_event& event = itf->default_event();
						process->make_statically_sensitive(event);
					}
					break;
				case PROCESS_STATICALLY_SENSITIVE_TO_EVENT_FINDER:
					{
						const sc_event_finder *event_finder = process_static_sensitivity->event_finder;
						const sc_event& event = event_finder->find_event(itf);
						process->make_statically_sensitive(event);
					}
					break;
			}
			
			delete process_static_sensitivity; // free unused resource
		}
	}
	
	processes_static_sensitivity.clear();
	
	elaboration_finalized = true;
}

} // end of namespace sc_core
