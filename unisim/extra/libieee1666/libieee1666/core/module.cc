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

#include "core/module.h"
#include "core/kernel.h"
#include "core/thread_process.h"
#include "core/object.h"
#include "core/event.h"
#include "core/time.h"
#include "core/module_name.h"
#include "channels/in.h"
#include "channels/out.h"
#include "channels/inout.h"
#include "channels/signal_if.h"

namespace sc_core {

//////////////////////////////// sc_bind_proxy /////////////////////////////////////////////

const sc_bind_proxy SC_BIND_PROXY_NIL;

sc_bind_proxy::sc_bind_proxy()
	: bound_interface(0)
	, outer_port(0)
{
}

sc_bind_proxy::sc_bind_proxy(sc_interface& _bound_interface)
	: bound_interface(&_bound_interface)
	, outer_port(0)
{
}

sc_bind_proxy::sc_bind_proxy(sc_port_base& _outer_port)
	: bound_interface(0)
	, outer_port(&_outer_port)
{
}

bool sc_bind_proxy::nil() const
{
	return !bound_interface && !outer_port;
}

void sc_bind_proxy::bind(sc_port_base& port) const
{
	if(bound_interface)
	{
		port.bind(*bound_interface);
	}
	else if(outer_port)
	{
		port.bind(*outer_port);
	}
}

/////////////////////////////////// sc_module /////////////////////////////////////////////

sc_module::~sc_module()
{
	kernel->unregister_module(this);
}

const char* sc_module::kind() const
{
	return "sc_module";
}

void sc_module::operator() (const sc_bind_proxy& p001,
                            const sc_bind_proxy& p002,
                            const sc_bind_proxy& p003,
                            const sc_bind_proxy& p004,
                            const sc_bind_proxy& p005,
                            const sc_bind_proxy& p006,
                            const sc_bind_proxy& p007,
                            const sc_bind_proxy& p008,
                            const sc_bind_proxy& p009,
                            const sc_bind_proxy& p010,
                            const sc_bind_proxy& p011,
                            const sc_bind_proxy& p012,
                            const sc_bind_proxy& p013,
                            const sc_bind_proxy& p014,
                            const sc_bind_proxy& p015,
                            const sc_bind_proxy& p016,
                            const sc_bind_proxy& p017,
                            const sc_bind_proxy& p018,
                            const sc_bind_proxy& p019,
                            const sc_bind_proxy& p020,
                            const sc_bind_proxy& p021,
                            const sc_bind_proxy& p022,
                            const sc_bind_proxy& p023,
                            const sc_bind_proxy& p024,
                            const sc_bind_proxy& p025,
                            const sc_bind_proxy& p026,
                            const sc_bind_proxy& p027,
                            const sc_bind_proxy& p028,
                            const sc_bind_proxy& p029,
                            const sc_bind_proxy& p030,
                            const sc_bind_proxy& p031,
                            const sc_bind_proxy& p032,
                            const sc_bind_proxy& p033,
                            const sc_bind_proxy& p034,
                            const sc_bind_proxy& p035,
                            const sc_bind_proxy& p036,
                            const sc_bind_proxy& p037,
                            const sc_bind_proxy& p038,
                            const sc_bind_proxy& p039,
                            const sc_bind_proxy& p040,
                            const sc_bind_proxy& p041,
                            const sc_bind_proxy& p042,
                            const sc_bind_proxy& p043,
                            const sc_bind_proxy& p044,
                            const sc_bind_proxy& p045,
                            const sc_bind_proxy& p046,
                            const sc_bind_proxy& p047,
                            const sc_bind_proxy& p048,
                            const sc_bind_proxy& p049,
                            const sc_bind_proxy& p050,
                            const sc_bind_proxy& p051,
                            const sc_bind_proxy& p052,
                            const sc_bind_proxy& p053,
                            const sc_bind_proxy& p054,
                            const sc_bind_proxy& p055,
                            const sc_bind_proxy& p056,
                            const sc_bind_proxy& p057,
                            const sc_bind_proxy& p058,
                            const sc_bind_proxy& p059,
                            const sc_bind_proxy& p060,
                            const sc_bind_proxy& p061,
                            const sc_bind_proxy& p062,
                            const sc_bind_proxy& p063,
                            const sc_bind_proxy& p064)
{
	const sc_bind_proxy *bind_proxies[64] = {
		&p001, &p002, &p003, &p004, &p005, &p006, &p007, &p008, &p009, &p010,
		&p011, &p012, &p013, &p014, &p015, &p016, &p017, &p018, &p019, &p020,
		&p021, &p022, &p023, &p024, &p025, &p026, &p027, &p028, &p029, &p030,
		&p031, &p032, &p033, &p034, &p035, &p036, &p037, &p038, &p039, &p040,
		&p041, &p042, &p043, &p044, &p045, &p046, &p047, &p048, &p049, &p050,
		&p051, &p052, &p053, &p054, &p055, &p056, &p057, &p058, &p059, &p060,
		&p061, &p062, &p063, &p064
	};
	
	unsigned int i;
	unsigned int num_bind_proxies = sizeof(bind_proxies) / sizeof(bind_proxies[0]);
	for(i = 0; i < num_bind_proxies; i++)
	{
		const sc_bind_proxy *bind_proxy = bind_proxies[i];
		sc_port_base *port = (i < ports.size()) ? ports[i] : 0;
		
		if(!bind_proxy->nil())
		{
			if(port)
			{
				bind_proxy->bind(*port);
			}
		}
	}
}

sc_module::sc_module( const sc_module_name& module_name )
	: sc_object((const char *)(*sc_kernel::get_kernel()->get_top_of_module_name_stack()))
	, sensitive()
	, spawn_options()
	, ports()
{
	init();
}

sc_module::sc_module()
	: sc_object((const char *)(*sc_kernel::get_kernel()->get_top_of_module_name_stack()))
	, sensitive()
	, spawn_options()
	, ports()
{
	init();
}

void sc_module::reset_signal_is(const sc_in<bool>& in, bool active_level)
{
	sc_process *process = kernel->get_current_process();
	if(process)
	{
		process->reset_signal_is(in, active_level);
	}
}

void sc_module::reset_signal_is(const sc_inout<bool>& inout, bool active_level)
{
	sc_process *process = kernel->get_current_process();
	if(process)
	{
		process->reset_signal_is(inout, active_level);
	}
}

void sc_module::reset_signal_is(const sc_out<bool>& out, bool active_level)
{
	sc_process *process = kernel->get_current_process();
	if(process)
	{
		process->reset_signal_is(out, active_level);
	}
}

void sc_module::reset_signal_is( const sc_signal_in_if<bool>& signal_in_if, bool active_level)
{
	sc_process *process = kernel->get_current_process();
	if(process)
	{
		process->reset_signal_is(signal_in_if, active_level);
	}
}

void sc_module::async_reset_signal_is(const sc_in<bool>& in, bool active_level)
{
	sc_process *process = kernel->get_current_process();
	if(process)
	{
		process->async_reset_signal_is(in, active_level);
	}
}

void sc_module::async_reset_signal_is(const sc_inout<bool>& inout, bool active_level)
{
	sc_process *process = kernel->get_current_process();
	if(process)
	{
		process->async_reset_signal_is(inout, active_level);
	}
}

void sc_module::async_reset_signal_is(const sc_out<bool>& out, bool active_level)
{
	sc_process *process = kernel->get_current_process();
	if(process)
	{
		process->async_reset_signal_is(out, active_level);
	}
}

void sc_module::async_reset_signal_is(const sc_signal_in_if<bool>& signal_in_if, bool active_level)
{
	sc_process *process = kernel->get_current_process();
	if(process)
	{
		process->async_reset_signal_is(signal_in_if, active_level);
	}
}

void sc_module::dont_initialize()
{
	spawn_options.dont_initialize();
}

void sc_module::set_stack_size(size_t stack_size)
{
	spawn_options.set_stack_size(stack_size);
}

void sc_module::before_end_of_elaboration()
{
}

void sc_module::end_of_elaboration()
{
}

void sc_module::start_of_simulation()
{
}

void sc_module::end_of_simulation()
{
}

sc_module::sc_module(const sc_module& module)
{
	// DISABLED
}

sc_module& sc_module::operator = ( const sc_module& )
{
	// DISABLED
	return *this;
}

void sc_module::end_module()
{
	sc_kernel *kernel = sc_kernel::get_kernel();
	kernel->end_module();
}

void sc_module::init()
{
	sc_module_name *non_const_module_name = kernel->get_top_of_module_name_stack();
	non_const_module_name->set_module(this);
	kernel->begin_module(this);
	kernel->register_module(this);
}

void sc_module::add_port(sc_port_base *port)
{
	ports.push_back(port);
}

} // end of namespace sc_core
