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

#include "core/process.h"
#include "core/spawn.h"
#include "core/kernel.h"
#include "core/event.h"
#include "core/interface.h"
#include "core/port.h"
#include "core/event_finder.h"
namespace sc_core {

sc_process_owner::sc_process_owner(bool _automatic)
	: automatic(_automatic)
{
}

sc_process_owner::~sc_process_owner()
{
}

bool sc_process_owner::is_automatic() const
{
	return automatic;
}

sc_process::sc_process(const char *_name, sc_process_owner *_process_owner, sc_process_owner_method_ptr _process_owner_method_ptr, sc_curr_proc_kind _process_kind, const sc_spawn_options *spawn_options)
	: sc_object(_name)
	, process_owner(_process_owner)
	, process_owner_method_ptr(_process_owner_method_ptr)
	, process_kind(_process_kind)
	, flag_dynamic(sc_kernel::get_kernel()->get_status() > SC_END_OF_ELABORATION)
	, flag_dont_initialize((process_kind == SC_CTHREAD_PROC_) ? true : (spawn_options ? spawn_options->get_flag_dont_initialize() : false))
	, automatic_process_owner(process_owner->is_automatic())
	, trigger_requested(false)
	, ref_count(0)
	, enabled(true)
	, suspended(false)
	, runnable_on_resuming(false)
{
	make_statically_sensitive(spawn_options);
}

sc_process::~sc_process()
{
	if(process_owner)
	{
		if(automatic_process_owner) delete process_owner;
	}
}

sc_curr_proc_kind sc_process::proc_kind() const
{
	return process_kind;
}

bool sc_process::dynamic() const
{
	return flag_dynamic;
}

bool sc_process::dont_initialize() const
{
	return flag_dont_initialize;
}

const char *sc_process::kind() const
{
	return "sc_process";
}

void sc_process::call_process_owner_method()
{
	(process_owner->*process_owner_method_ptr)();
}

void sc_process::acquire()
{
	ref_count++;
}

void sc_process::release()
{
	ref_count--;
	if(ref_count == 0)
	{
		delete this;
	}
}

void sc_process::suspend(sc_descendant_inclusion_info include_descendants)
{
	if(include_descendants == SC_INCLUDE_DESCENDANTS)
	{
		const std::vector<sc_object *> child_objects = get_child_objects();
		unsigned int num_child_objects = child_objects.size();
		unsigned int i;

		for(i = 0; i < num_child_objects; i++)
		{
			sc_object *child_object = child_objects[i];
			
			sc_process *child_process = dynamic_cast<sc_process *>(child_object);
			
			if(child_process)
			{
				// child object is confirmed to be an sc_process instance
				child_process->suspend(include_descendants);
			}
		}
	}
	
	suspend();
}

void sc_process::resume(sc_descendant_inclusion_info include_descendants)
{
	if(include_descendants == SC_INCLUDE_DESCENDANTS)
	{
		const std::vector<sc_object *> child_objects = get_child_objects();
		unsigned int num_child_objects = child_objects.size();
		unsigned int i;

		for(i = 0; i < num_child_objects; i++)
		{
			sc_object *child_object = child_objects[i];
			
			sc_process *child_process = dynamic_cast<sc_process *>(child_object);
			
			if(child_process)
			{
				// child object is confirmed to be an sc_process instance
				child_process->resume(include_descendants);
			}
		}
	}
	
	resume();
}

void sc_process::disable(sc_descendant_inclusion_info include_descendants)
{
	if(include_descendants == SC_INCLUDE_DESCENDANTS)
	{
		const std::vector<sc_object *> child_objects = get_child_objects();
		unsigned int num_child_objects = child_objects.size();
		unsigned int i;

		for(i = 0; i < num_child_objects; i++)
		{
			sc_object *child_object = child_objects[i];
			
			sc_process *child_process = dynamic_cast<sc_process *>(child_object);
			
			if(child_process)
			{
				// child object is confirmed to be an sc_process instance
				child_process->disable(include_descendants);
			}
		}
	}
	
	disable();
}

void sc_process::enable(sc_descendant_inclusion_info include_descendants)
{
	if(include_descendants == SC_INCLUDE_DESCENDANTS)
	{
		const std::vector<sc_object *> child_objects = get_child_objects();
		unsigned int num_child_objects = child_objects.size();
		unsigned int i;

		for(i = 0; i < num_child_objects; i++)
		{
			sc_object *child_object = child_objects[i];
			
			sc_process *child_process = dynamic_cast<sc_process *>(child_object);
			
			if(child_process)
			{
				// child object is confirmed to be an sc_process instance
				child_process->enable(include_descendants);
			}
		}
	}
	
	enable();
}

void sc_process::kill(sc_descendant_inclusion_info include_descendants)
{
	if(include_descendants == SC_INCLUDE_DESCENDANTS)
	{
		const std::vector<sc_object *> child_objects = get_child_objects();
		unsigned int num_child_objects = child_objects.size();
		unsigned int i;

		for(i = 0; i < num_child_objects; i++)
		{
			sc_object *child_object = child_objects[i];
			
			sc_process *child_process = dynamic_cast<sc_process *>(child_object);
			
			if(child_process)
			{
				// child object is confirmed to be an sc_process instance
				child_process->kill(include_descendants);
			}
		}
	}
	
	kill();
}

void sc_process::reset(sc_descendant_inclusion_info include_descendants)
{
	if(include_descendants == SC_INCLUDE_DESCENDANTS)
	{
		const std::vector<sc_object *> child_objects = get_child_objects();
		unsigned int num_child_objects = child_objects.size();
		unsigned int i;

		for(i = 0; i < num_child_objects; i++)
		{
			sc_object *child_object = child_objects[i];
			
			sc_process *child_process = dynamic_cast<sc_process *>(child_object);
			
			if(child_process)
			{
				// child object is confirmed to be an sc_process instance
				child_process->reset(include_descendants);
			}
		}
	}
	
	reset(true);
}

void sc_process::throw_it(const sc_user_exception& user_defined_exception, sc_descendant_inclusion_info include_descendants)
{
	if(include_descendants == SC_INCLUDE_DESCENDANTS)
	{
		const std::vector<sc_object *> child_objects = get_child_objects();
		unsigned int num_child_objects = child_objects.size();
		unsigned int i;

		for(i = 0; i < num_child_objects; i++)
		{
			sc_object *child_object = child_objects[i];
			
			sc_process *child_process = dynamic_cast<sc_process *>(child_object);
			
			if(child_process)
			{
				// child object is confirmed to be an sc_process instance
				child_process->throw_it(user_defined_exception, include_descendants);
			}
		}
	}
	
	throw_it(user_defined_exception);
}

void sc_process::reset_signal_is(const sc_in<bool>& in, bool active_level)
{
	process_reset_bind_infos.push_back(sc_process_reset_bind_info(this, false, in, active_level));
}

void sc_process::reset_signal_is(const sc_inout<bool>& inout, bool active_level)
{
	process_reset_bind_infos.push_back(sc_process_reset_bind_info(this, false, inout, active_level));
}

void sc_process::reset_signal_is(const sc_out<bool>& out, bool active_level)
{
	process_reset_bind_infos.push_back(sc_process_reset_bind_info(this, false, out, active_level));
}

void sc_process::reset_signal_is(const sc_signal_in_if<bool>& signal_in_if, bool active_level)
{
	process_reset_bind_infos.push_back(sc_process_reset_bind_info(this, false, signal_in_if, active_level));
}

void sc_process::async_reset_signal_is(const sc_in<bool>& in, bool active_level)
{
	process_reset_bind_infos.push_back(sc_process_reset_bind_info(this, true, in, active_level));
}

void sc_process::async_reset_signal_is(const sc_inout<bool>& inout, bool active_level)
{
	process_reset_bind_infos.push_back(sc_process_reset_bind_info(this, true, inout, active_level));
}

void sc_process::async_reset_signal_is(const sc_out<bool>& out, bool active_level)
{
	process_reset_bind_infos.push_back(sc_process_reset_bind_info(this, true, out, active_level));
}

void sc_process::async_reset_signal_is(const sc_signal_in_if<bool>& signal_in_if, bool active_level)
{
	process_reset_bind_infos.push_back(sc_process_reset_bind_info(this, true, signal_in_if, active_level));
}

void sc_process::make_statically_sensitive(const sc_event& event)
{
	switch(process_kind)
	{
		case SC_NO_PROC_:
			break;
		case SC_METHOD_PROC_:
			event.add_statically_sensitive_method_process((sc_method_process *) this);
			break;
		case SC_THREAD_PROC_:
		case SC_CTHREAD_PROC_:
			event.add_statically_sensitive_thread_process((sc_thread_process *) this);
			break;
	}
}

void sc_process::make_statically_sensitive(const sc_interface& itf)
{
	const sc_event& event = itf.default_event();
	make_statically_sensitive(event);
}

void sc_process::make_statically_sensitive(const sc_port_base& port)
{
	port.add_process_statically_sensitive_to_port(this);
}

void sc_process::make_statically_sensitive(const sc_export_base& exp)
{
	throw std::runtime_error("unimplemented function: sc_process sensitive to sc_export");
}

void sc_process::make_statically_sensitive(const sc_event_finder& event_finder)
{
	event_finder.get_port().add_process_statically_sensitive_to_event_finder(this, event_finder);
}

void sc_process::make_statically_sensitive(const sc_spawn_options *spawn_options)
{
	unsigned int i;
	
	const std::vector<const sc_event *>& sensitive_events = spawn_options->get_sensitive_events();
	unsigned int num_sensitive_events = sensitive_events.size();
	for(i = 0; i <num_sensitive_events; i++)
	{
		make_statically_sensitive(*sensitive_events[i]);
	}

	const std::vector<const sc_port_base *>& sensitive_ports = spawn_options->get_sensitive_ports();
	unsigned int num_sensitive_ports = sensitive_ports.size();
	for(i = 0; i <num_sensitive_ports; i++)
	{
		make_statically_sensitive(*sensitive_ports[i]);
	}

	const std::vector<const sc_export_base *>& sensitive_exports = spawn_options->get_sensitive_exports();
	unsigned int num_sensitive_exports = sensitive_exports.size();
	for(i = 0; i <num_sensitive_exports; i++)
	{
		make_statically_sensitive(*sensitive_exports[i]);
	}

	const std::vector<const sc_interface *>& sensitive_interfaces = spawn_options->get_sensitive_interfaces();
	unsigned int num_sensitive_interfaces = sensitive_interfaces.size();
	for(i = 0; i <num_sensitive_interfaces; i++)
	{
		make_statically_sensitive(*sensitive_interfaces[i]);
	}

	const std::vector<const sc_event_finder *>& sensitive_event_finders = spawn_options->get_sensitive_event_finders();
	unsigned int num_sensitive_event_finders = sensitive_event_finders.size();
	for(i = 0; i <num_sensitive_event_finders; i++)
	{
		make_statically_sensitive(*sensitive_event_finders[i]);
	}
}

void sc_process::finalize_elaboration()
{
	unsigned int num_process_reset_bind_infos = process_reset_bind_infos.size();
	unsigned int i;
	
	for(i = 0; i < num_process_reset_bind_infos; i++)
	{
		const sc_process_reset_bind_info& process_reset_bind_info = process_reset_bind_infos[i];
		
		sc_signal_in_if<bool> *signal_in_if = 0;
		
		if(process_reset_bind_info.in) signal_in_if = const_cast<sc_signal_in_if<bool> *>(dynamic_cast<const sc_signal_in_if<bool> *>(process_reset_bind_info.in->get_interface()));
		else if(process_reset_bind_info.inout) signal_in_if = const_cast<sc_signal_in_if<bool> *>(dynamic_cast<const sc_signal_in_if<bool> *>(process_reset_bind_info.inout->get_interface()));
		else if(process_reset_bind_info.out) signal_in_if = const_cast<sc_signal_in_if<bool> *>(dynamic_cast<const sc_signal_in_if<bool> *>(process_reset_bind_info.out->get_interface()));
		else if(process_reset_bind_info.signal_in_if) signal_in_if = const_cast<sc_signal_in_if<bool> *>(dynamic_cast<const sc_signal_in_if<bool> *>(process_reset_bind_info.signal_in_if));
		
		if(signal_in_if)
		{
			signal_in_if->is_reset(sc_process_reset(this, process_reset_bind_info.async, process_reset_bind_info.active_level));
		}
	}
}

void sc_process::reset_signal_value_changed(bool reset_signal_value, bool async, bool active_level)
{
	if(reset_signal_value == active_level)
	{
		if(async)
		{
			reset(async);
		}
	}
	else
	{
	}
}

} // end of namespace sc_core
