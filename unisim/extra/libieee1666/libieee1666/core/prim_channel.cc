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

#include "core/prim_channel.h"
#include "core/kernel.h"

namespace sc_core {

//////////////////////////////////// sc_prim_channel /////////////////////////////////////////////
	
const char* sc_prim_channel::kind() const
{
	return "sc_prim_channel";
}

sc_prim_channel::sc_prim_channel()
	: kernel(sc_kernel::get_kernel())
	, update_requested(false)
{
	kernel->register_prim_channel(this);
}

sc_prim_channel::sc_prim_channel(const char *_name)
	: sc_object(_name)
	, kernel(sc_kernel::get_kernel())
	, update_requested(false)
{
	kernel->register_prim_channel(this);
}

sc_prim_channel::~sc_prim_channel()
{
	kernel->unregister_prim_channel(this);
}

void sc_prim_channel::request_update()
{
	kernel->request_update(this);
}

void sc_prim_channel::async_request_update()
{
}

void sc_prim_channel::update()
{
}

void sc_prim_channel::next_trigger()
{
	kernel->next_trigger();
}

void sc_prim_channel::next_trigger(const sc_event& e)
{
	kernel->next_trigger(e);
}

void sc_prim_channel::next_trigger(const sc_event_or_list& el)
{
	kernel->next_trigger(el);
}

void sc_prim_channel::next_trigger(const sc_event_and_list& el)
{
	kernel->next_trigger(el);
}

void sc_prim_channel::next_trigger(const sc_time& t)
{
	kernel->next_trigger(t);
}

void sc_prim_channel::next_trigger(double d, sc_time_unit tu)
{
	sc_time t = sc_time(d, tu);
	kernel->next_trigger(t);
}

void sc_prim_channel::next_trigger(const sc_time& t, const sc_event& e)
{
	kernel->next_trigger(t, e);
}

void sc_prim_channel::next_trigger(double d, sc_time_unit tu, const sc_event& e)
{
	sc_time t = sc_time(d, tu);
	kernel->next_trigger(t, e);
}

void sc_prim_channel::next_trigger(const sc_time& t, const sc_event_or_list& el)
{
	kernel->next_trigger(t, el);
}

void sc_prim_channel::next_trigger(double d, sc_time_unit tu, const sc_event_or_list& el)
{
	sc_time t = sc_time(d, tu);
	kernel->next_trigger(t, el);
}

void sc_prim_channel::next_trigger(const sc_time& t, const sc_event_and_list& el)
{
	kernel->next_trigger(t, el);
}

void sc_prim_channel::next_trigger(double d, sc_time_unit tu, const sc_event_and_list& el)
{
	sc_time t = sc_time(d, tu);
	kernel->next_trigger(t, el);
}

void sc_prim_channel::wait()
{
	kernel->wait();
}

void sc_prim_channel::wait(int n)
{
	kernel->wait(n);
}

void sc_prim_channel::wait(const sc_event& e)
{
	kernel->wait(e);
}

void sc_prim_channel::wait(const sc_event_or_list& el)
{
	kernel->wait(el);
}

void sc_prim_channel::wait(const sc_event_and_list& el)
{
	kernel->wait(el);
}

void sc_prim_channel::wait(const sc_time& t)
{
	kernel->wait(t);
}

void sc_prim_channel::wait(double d, sc_time_unit tu)
{
	sc_time t = sc_time(d, tu);
	kernel->wait(t);
}

void sc_prim_channel::wait(const sc_time& t, const sc_event& e)
{
	kernel->wait(t, e);
}

void sc_prim_channel::wait(double d, sc_time_unit tu, const sc_event& e)
{
	sc_time t = sc_time(d, tu);
	kernel->wait(t, e);
}

void sc_prim_channel::wait(const sc_time& t, const sc_event_or_list& el)
{
	kernel->wait(t, el);
}

void sc_prim_channel::wait(double d, sc_time_unit tu, const sc_event_or_list& el)
{
	sc_time t = sc_time(d, tu);
	kernel->wait(t, el);
}

void sc_prim_channel::wait(const sc_time& t, const sc_event_and_list& el)
{
	kernel->wait(t, el);
}

void sc_prim_channel::wait(double d, sc_time_unit tu, const sc_event_and_list& el)
{
	sc_time t = sc_time(d, tu);
	kernel->wait(t, el);
}

void sc_prim_channel::before_end_of_elaboration()
{
}

void sc_prim_channel::end_of_elaboration()
{
}

void sc_prim_channel::start_of_simulation()
{
}

void sc_prim_channel::end_of_simulation()
{
}

// Disabled
sc_prim_channel::sc_prim_channel( const sc_prim_channel& )
{
}

// Disabled
sc_prim_channel& sc_prim_channel::operator= ( const sc_prim_channel& )
{
	return *this;
}

} // end of namespace sc_core
