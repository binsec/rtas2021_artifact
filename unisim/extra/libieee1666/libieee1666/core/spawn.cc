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

#include "core/spawn.h"
#include "core/kernel.h"
#include "core/event.h"
#include "core/interface.h"
#include "core/port.h"
#include "core/export.h"
#include "channels/in.h"
#include "channels/out.h"
#include "channels/inout.h"

namespace sc_core {

sc_spawn_options::sc_spawn_options()
	: flag_spawn_method(false)
	, flag_dont_initialize(false)
	, stack_size(0)
	, sensitive_events()
	, sensitive_ports()
	, sensitive_exports()
	, sensitive_interfaces()
	, sensitive_event_finders()
{
}

void sc_spawn_options::spawn_method()
{
	flag_spawn_method = true;
}

void sc_spawn_options::dont_initialize()
{
	flag_dont_initialize = true;
}

void sc_spawn_options::set_stack_size(int _stack_size)
{
	stack_size = _stack_size;
}

void sc_spawn_options::set_sensitivity( const sc_event *e)
{
	sensitive_events.push_back(e);
}

void sc_spawn_options::set_sensitivity( sc_port_base *p)
{
	sensitive_ports.push_back(p);
}

void sc_spawn_options::set_sensitivity(sc_export_base *exp)
{
	sensitive_exports.push_back(exp);
}

void sc_spawn_options::set_sensitivity(sc_interface* itf)
{
	sensitive_interfaces.push_back(itf);
}

void sc_spawn_options::set_sensitivity( sc_event_finder *ef)
{
	sensitive_event_finders.push_back(ef);
}

void sc_spawn_options::reset_signal_is( const sc_in<bool>& , bool )
{
}

void sc_spawn_options::reset_signal_is( const sc_inout<bool>& , bool )
{
}

void sc_spawn_options::reset_signal_is( const sc_out<bool>& , bool )
{
}

void sc_spawn_options::reset_signal_is( const sc_signal_in_if<bool>& , bool )
{
}

void sc_spawn_options::async_reset_signal_is( const sc_in<bool>& , bool )
{
}

void sc_spawn_options::async_reset_signal_is( const sc_inout<bool>& , bool )
{
}

void sc_spawn_options::async_reset_signal_is( const sc_out<bool>& , bool )
{
}

void sc_spawn_options::async_reset_signal_is( const sc_signal_in_if<bool>& , bool )
{
}

sc_spawn_options::sc_spawn_options( const sc_spawn_options& )
{
}

sc_spawn_options& sc_spawn_options::operator = (const sc_spawn_options& opt)
{
	flag_spawn_method = opt.flag_spawn_method;
	flag_dont_initialize = opt.flag_dont_initialize;
	stack_size = opt.stack_size;
	sensitive_events = opt.sensitive_events;
	sensitive_ports = opt.sensitive_ports;
	sensitive_exports = opt.sensitive_exports;
	sensitive_interfaces = opt.sensitive_interfaces;
	sensitive_event_finders = opt.sensitive_event_finders;
	return *this;
}

bool sc_spawn_options::get_flag_spawn_method() const
{
	return flag_spawn_method;
}

bool sc_spawn_options::get_flag_dont_initialize() const
{
	return flag_dont_initialize;
}

int sc_spawn_options::get_stack_size() const
{
	return stack_size;
}

const std::vector<const sc_event *>& sc_spawn_options::get_sensitive_events() const
{
	return sensitive_events;
}

const std::vector<const sc_port_base *>& sc_spawn_options::get_sensitive_ports() const
{
	return sensitive_ports;
}

const std::vector<const sc_export_base *>& sc_spawn_options::get_sensitive_exports() const
{
	return sensitive_exports;
}

const std::vector<const sc_interface *>& sc_spawn_options::get_sensitive_interfaces() const
{
	return sensitive_interfaces;
}

const std::vector<const sc_event_finder *>& sc_spawn_options::get_sensitive_event_finders() const
{
	return sensitive_event_finders;
}

// namespace sc_unnamed {
// /* implementation-defined */ _1;
// /* implementation-defined */ _2;
// /* implementation-defined */ _3;
// /* implementation-defined */ _4;
// /* implementation-defined */ _5;
// /* implementation-defined */ _6;
// /* implementation-defined */ _7;
// /* implementation-defined */ _8;
// /* implementation-defined */ _9;
// } // end of namespace sc_unnamed

} // end of namespace sc_core
