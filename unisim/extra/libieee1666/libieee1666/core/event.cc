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

#include "core/event.h"
#include "core/kernel.h"
#include "core/object.h"
#include "core/time.h"
#include "core/kernel_event.h"
#include "core/thread_process.h"
#include "core/method_process.h"
#include <string.h>

namespace sc_core {

//////////////////////////////////// sc_event /////////////////////////////////////////////

sc_event::sc_event()
	: kernel(sc_kernel::get_kernel())
	, event_name(create_hierarchical_name(__LIBIEEE1666_KERNEL_PREFIX__ "_event"))
	, parent_object(0)
	, state(NOT_NOTIFIED)
	, kernel_event(0)
	, timed_kernel_event(0)
	, dynamically_sensitive_thread_processes()
	, dynamically_sensitive_method_processes()
	, statically_sensitive_thread_processes()
	, statically_sensitive_method_processes()
{
	init();
}

sc_event::sc_event( const char* _name)
	: kernel(sc_kernel::get_kernel())
	, event_name((_name && *_name) ? create_hierarchical_name(_name) : create_hierarchical_name(__LIBIEEE1666_KERNEL_PREFIX__ "_event"))
	, parent_object(0)
	, state(NOT_NOTIFIED)
	, kernel_event(0)
	, timed_kernel_event(0)
	, dynamically_sensitive_thread_processes()
	, dynamically_sensitive_method_processes()
	, statically_sensitive_thread_processes()
	, statically_sensitive_method_processes()
{
	init();
}

sc_event::~sc_event()
{
	cancel();
	
	if(parent_object) parent_object->remove_child_event(this);
	if(kernel) kernel->unregister_event(this);
}

void sc_event::init()
{
	parent_object = kernel->get_current_object();
	if(parent_object) parent_object->add_child_event(this);
	kernel->register_event(this);
}

std::string sc_event::create_hierarchical_name(const char *_name) const
{
	std::string hierarchical_name;
	
	sc_kernel *kernel = sc_kernel::get_kernel();
	sc_object *_parent_object = kernel->get_current_object();
	if(_parent_object)
	{
		hierarchical_name += _parent_object->name();
		hierarchical_name += '.';
	}

	hierarchical_name += _name;
  
	if(kernel->find_event(hierarchical_name.c_str()))
	{
		// hierarchical name already exists
		std::string new_hierarchical_name = kernel->gen_unique_name(hierarchical_name.c_str());
		if(strncmp(_name, __LIBIEEE1666_KERNEL_PREFIX__, strlen(__LIBIEEE1666_KERNEL_PREFIX__)) != 0)
		{
			std::cerr << "WARNING! event \"" << hierarchical_name << "\" has been renamed \"" << new_hierarchical_name << "\"" << std::endl;
		}
		hierarchical_name = new_hierarchical_name;
	}
	
	return hierarchical_name;
}

const char* sc_event::name() const
{
	return event_name.c_str();
}

const char* sc_event::basename() const
{
	size_t pos = event_name.find_last_of(".", 0);
	return (pos != std::string::npos) ? event_name.c_str() + pos + 1 : event_name.c_str();
}

bool sc_event::in_hierarchy() const
{
	return !event_name.empty();
}

sc_object* sc_event::get_parent_object() const
{
	return parent_object;
}

void sc_event::notify()
{
// 	std::cerr << name() << ".notify()" << std::endl;
	// immediate notification
	cancel();
	trigger();
}

void sc_event::notify(const sc_time& t)
{
//  	std::cerr << name() << ".notify(t=" << t << ")" << std::endl;
	// this is a delta or timed notification
	
	if(state == DELTA_NOTIFIED)
	{
// 		std::cerr << name() << ": there's already a pending delta notification that takes precedence over a delta or timed notification" << std::endl;
		// there's already a pending delta notification that takes precedence over a delta or timed notification
		return;
	}

	if(t == SC_ZERO_TIME)
	{
		// this is a delta notification
		
		if(state == TIMED_NOTIFIED)
		{
			// cancel pending timed notification because delta notification takes precedence
// 			std::cerr << name() << ": cancel pending timed notification because delta notification takes precedence" << std::endl;
			cancel();
		}
		
		state = DELTA_NOTIFIED;
		kernel_event = kernel->notify(this);
	}
	else
	{
		// this is a timed notification
		
		if(state == TIMED_NOTIFIED)
		{
			// there's already a pending timed notification
			
			sc_time event_time(kernel->get_current_time_stamp());
			event_time += t;
			if(timed_kernel_event->get_time() <= event_time)
			{
				// there's already an earlier pending timed notification that takes precedence
// 				std::cerr << name() << ": there's already an earlier pending timed notification that takes precedence" << std::endl;
				return;
			}
		}
		state = TIMED_NOTIFIED;
		timed_kernel_event = kernel->notify(this, t);
	}
}

void sc_event::notify(double d, sc_time_unit tu)
{
// 	std::cerr << name() << ".notify(d=" << d << ", tu=" << tu << ")" << std::endl;
	notify(sc_time(d, tu));
}

void sc_event::cancel()
{
	switch(state)
	{
		case NOT_NOTIFIED:
			break;
		case DELTA_NOTIFIED:
			kernel_event->cancel();
			break;
		case TIMED_NOTIFIED:
			timed_kernel_event->cancel();
			break;
	}
	
	state = NOT_NOTIFIED;
}

void sc_event::trigger()
{
//  	std::cerr << name() << ".trigger()" << std::endl;
	if(statically_sensitive_method_processes.size())
	{
		std::set<sc_method_process *>::iterator it = statically_sensitive_method_processes.begin();
		
		do
		{
			sc_method_process *method_process = *it;
			method_process->trigger_statically();
		}
		while(++it != statically_sensitive_method_processes.end());
	}

	if(statically_sensitive_thread_processes.size())
	{
		std::set<sc_thread_process *>::iterator it = statically_sensitive_thread_processes.begin();
		
		do
		{
			sc_thread_process *thread_process = *it;
			thread_process->trigger_statically();
		}
		while(++it != statically_sensitive_thread_processes.end());
	}

	sc_method_process *current_method_process = kernel->get_current_method_process();
	sc_thread_process *current_thread_process = kernel->get_current_thread_process();

	if(dynamically_sensitive_method_processes.size())
	{
		std::set<sc_method_process *>::iterator it = dynamically_sensitive_method_processes.begin();
		
		do
		{
			sc_method_process *method_process = *it++;
			if(method_process != current_method_process)
			{
				method_process->trigger_dynamically(this);
			}
		}
		while(it != dynamically_sensitive_method_processes.end());
		
		dynamically_sensitive_method_processes.clear();
	}

	if(dynamically_sensitive_thread_processes.size())
	{
		std::set<sc_thread_process *>::iterator it = dynamically_sensitive_thread_processes.begin();

		do
		{
			sc_thread_process *thread_process = *it++;
			if(thread_process != current_thread_process)
			{
				thread_process->trigger_dynamically(this);
			}
		}
		while(it != dynamically_sensitive_thread_processes.end());
		
		dynamically_sensitive_thread_processes.clear();
	}

	state = NOT_NOTIFIED;
}

void sc_event::clear_dynamically_sensitive_processes()
{
	dynamically_sensitive_thread_processes.clear();
	dynamically_sensitive_method_processes.clear();
}

sc_event_and_expr sc_event::operator & (const sc_event& e) const
{
	sc_event_and_expr event_and_expr;
	event_and_expr &= *this;
	event_and_expr &= e;
	return event_and_expr;
}

sc_event_and_expr sc_event::operator & (const sc_event_and_list& el) const
{
	sc_event_and_expr event_and_expr;
	event_and_expr &= *this;
	event_and_expr &= el;
	return event_and_expr;
}

sc_event_or_expr sc_event::operator | (const sc_event& e) const
{
	sc_event_or_expr event_or_expr;
	event_or_expr |= *this;
	event_or_expr |= e;
	return event_or_expr;
}

sc_event_or_expr sc_event::operator | (const sc_event_or_list& el) const
{
	sc_event_or_expr event_or_expr;
	event_or_expr |= *this;
	event_or_expr |= el;
	return event_or_expr;
}

// Disabled
sc_event::sc_event( const sc_event& )
{
}

// Disabled
sc_event& sc_event::operator= ( const sc_event& )
{
	return *this;
}

void sc_event::remove_dynamically_sensitive_thread_process(sc_thread_process *thread_process) const
{
	dynamically_sensitive_thread_processes.erase(thread_process);
}

void sc_event::remove_dynamically_sensitive_method_process(sc_method_process *method_process) const
{
	dynamically_sensitive_method_processes.erase(method_process);
}

void sc_event::add_statically_sensitive_thread_process(sc_thread_process *thread_process) const
{
	statically_sensitive_thread_processes.insert(thread_process);
}

void sc_event::add_statically_sensitive_method_process(sc_method_process *method_process) const
{
	statically_sensitive_method_processes.insert(method_process);
}

void sc_event::remove_statically_sensitive_thread_process(sc_thread_process *thread_process) const
{
	statically_sensitive_thread_processes.erase(thread_process);
}

void sc_event::remove_statically_sensitive_method_process(sc_method_process *method_process) const
{
	statically_sensitive_method_processes.erase(method_process);
}

sc_event::state_t sc_event::get_state() const
{
	return state;
}

sc_kernel_event *sc_event::get_kernel_event() const
{
	return kernel_event;
}

sc_kernel_event *sc_event::get_timed_kernel_event() const
{
	return timed_kernel_event;
}

////////////////////////////// sc_event_list ///////////////////////////////

sc_event_list::sc_event_list(sc_event_list_type_t _type, bool _auto_mm)
	: type(_type)
	, auto_mm(_auto_mm)
	, events()
	, ref_count(0)
{
}

sc_event_list::sc_event_list(const sc_event_list& el)
	: type(el.type)
	, auto_mm(false)
	, events(el.events)
	, ref_count(0)
{
	el.release();
}

sc_event_list::sc_event_list(sc_event_list_type_t _type, bool _auto_mm, const sc_event& e)
	: type(_type)
	, auto_mm(_auto_mm)
	, events()
	, ref_count(0)
{
	events.insert(&e);
}

sc_event_list& sc_event_list::operator= (const sc_event_list& el)
{
	events = el.events;
	return *this;
}

sc_event_list::~sc_event_list()
{
}

int sc_event_list::size() const
{
	return events.size();
}

void sc_event_list::swap( sc_event_list& el)
{
	events.swap(el.events);
}

void sc_event_list::add_dynamically_sensitive_thread_process(sc_thread_process *thread_process) const
{
	std::set<const sc_event *>::const_iterator it;
	for(it = events.begin(); it != events.end(); it++)
	{
		const sc_event *e = *it;
		e->add_dynamically_sensitive_thread_process(thread_process);
	}
}

void sc_event_list::add_dynamically_sensitive_method_process(sc_method_process *method_process) const
{
	std::set<const sc_event *>::const_iterator it;
	for(it = events.begin(); it != events.end(); it++)
	{
		const sc_event *e = *it;
		e->add_dynamically_sensitive_method_process(method_process);
	}
}

void sc_event_list::remove_dynamically_sensitive_thread_process(sc_thread_process *thread_process) const
{
	std::set<const sc_event *>::const_iterator it;
	for(it = events.begin(); it != events.end(); it++)
	{
		const sc_event *e = *it;
		e->remove_dynamically_sensitive_thread_process(thread_process);
	}
}

void sc_event_list::remove_dynamically_sensitive_method_process(sc_method_process *method_process) const
{
	std::set<const sc_event *>::const_iterator it;
	for(it = events.begin(); it != events.end(); it++)
	{
		const sc_event *e = *it;
		e->remove_dynamically_sensitive_method_process(method_process);
	}
}

void sc_event_list::insert(const sc_event & e)
{
	events.insert(&e);
}

void sc_event_list::insert(const sc_event_list& el)
{
	std::set<const sc_event *>::iterator it;
	for(it = events.begin(); it != events.end(); it++)
	{
		const sc_event *e = *it;
		events.insert(e);
	}
}

void sc_event_list::acquire() const
{
	if(auto_mm)
	{
		ref_count++;
	}
}

void sc_event_list::release() const
{
	if(auto_mm)
	{
		if(--ref_count == 0) delete this;
	}
}

//////////////////////////// sc_event_and_list /////////////////////////////

sc_event_and_list::sc_event_and_list()
	: sc_event_list(EVENT_AND_LIST, false)
{
}

sc_event_and_list::sc_event_and_list(bool _auto_mm)
	: sc_event_list(EVENT_AND_LIST, _auto_mm)
{
}

sc_event_and_list::sc_event_and_list(const sc_event_and_list& el)
	: sc_event_list(el)
{
}

sc_event_and_list::sc_event_and_list(const sc_event& e)
	: sc_event_list(EVENT_AND_LIST, false, e)
{
}

sc_event_and_list::~sc_event_and_list()
{
}

sc_event_and_list& sc_event_and_list::operator &= (const sc_event& e)
{
	insert(e);
	return *this;
}

sc_event_and_list& sc_event_and_list::operator &= (const sc_event_and_list& el)
{
	insert(el);
	return *this;	
}

sc_event_and_expr sc_event_and_list::operator & (const sc_event& e) const
{
	sc_event_and_expr event_and_expr;
	event_and_expr &= *this;
	event_and_expr &= e;
	return event_and_expr;
}

sc_event_and_expr sc_event_and_list::operator & (const sc_event_and_list& el) const
{
	sc_event_and_expr event_and_expr;
	event_and_expr &= *this;
	event_and_expr &= el;
	return event_and_expr;
}

//////////////////////////// sc_event_or_list /////////////////////////////


sc_event_or_list::sc_event_or_list()
	: sc_event_list(EVENT_OR_LIST, false)
{
}

sc_event_or_list::sc_event_or_list(bool _auto_mm)
	: sc_event_list(EVENT_OR_LIST, _auto_mm)
{
}

sc_event_or_list::sc_event_or_list(const sc_event_or_list& el)
	: sc_event_list(el)
{
}

sc_event_or_list::sc_event_or_list(const sc_event& e)
	: sc_event_list(EVENT_OR_LIST, false, e)
{
}

sc_event_or_list::~sc_event_or_list()
{
}

sc_event_or_list& sc_event_or_list::operator |= (const sc_event& e)
{
	insert(e);
	return *this;
}

sc_event_or_list& sc_event_or_list::operator |= (const sc_event_or_list& el)
{
	insert(el);
	return *this;
}

sc_event_or_expr sc_event_or_list::operator | (const sc_event& e) const
{
	sc_event_or_expr event_or_expr;
	event_or_expr |= *this;
	event_or_expr |= e;
	return event_or_expr;
}

sc_event_or_expr sc_event_or_list::operator | (const sc_event_or_list& el) const
{
	sc_event_or_expr event_or_expr;
	event_or_expr |= *this;
	event_or_expr |= el;
	return event_or_expr;
}

//////////////////////////// sc_event_and_expr /////////////////////////////

sc_event_and_expr::sc_event_and_expr()
	: event_and_list(new sc_event_and_list(true /* automatic memory management */))
{
	event_and_list->acquire();
}

sc_event_and_expr::~sc_event_and_expr()
{
	event_and_list->release();
}

sc_event_and_expr::operator const sc_event_and_list& () const
{
	return *event_and_list;
}

sc_event_and_expr operator & (sc_event_and_expr event_and_expr, const sc_event& e)
{
	event_and_expr &= e;
	return event_and_expr;
}

sc_event_and_expr operator & (sc_event_and_expr event_and_expr, const sc_event_and_list& el)
{
	event_and_expr &= el;
	return event_and_expr;
}

sc_event_and_expr& sc_event_and_expr::operator &= (const sc_event& e)
{
	*event_and_list &= e;
	return *this;
}

sc_event_and_expr& sc_event_and_expr::operator &= (const sc_event_and_list& el)
{
	*event_and_list &= el;
	return *this;
}

//////////////////////////// sc_event_or_expr /////////////////////////////

sc_event_or_expr::sc_event_or_expr()
	: event_or_list(new sc_event_or_list(true /* automatic memory management */))
{
	event_or_list->acquire();
}

sc_event_or_expr::~sc_event_or_expr()
{
	event_or_list->release();
}

sc_event_or_expr::operator const sc_event_or_list& () const
{
	return *event_or_list;
}

sc_event_or_expr operator | (sc_event_or_expr event_or_expr, const sc_event& e)
{
	event_or_expr |= e;
	return event_or_expr;
}

sc_event_or_expr operator | (sc_event_or_expr event_or_expr, const sc_event_or_list& el)
{
	event_or_expr |= el;
	return event_or_expr;
}

sc_event_or_expr& sc_event_or_expr::operator |= (const sc_event& e)
{
	*event_or_list |= e;
	return *this;
}

sc_event_or_expr& sc_event_or_expr::operator |= (const sc_event_or_list& el)
{
	*event_or_list |= el;
	return *this;
}

} // end of namespace sc_core
