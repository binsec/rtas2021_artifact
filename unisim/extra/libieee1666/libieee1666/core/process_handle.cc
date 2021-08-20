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

#include "core/process_handle.h"
#include "core/process.h"

namespace sc_core {

//////////////////////////////// sc_unwind_exception //////////////////////////////////////////

const char* sc_unwind_exception::what() const throw()
{
	switch(type)
	{
		case SC_UNWIND_EXCEPTION_RESET:
			return "reset";
		case SC_UNWIND_EXCEPTION_KILL:
			return "kill";
	}
	return "";
}

bool sc_unwind_exception::is_reset() const
{
	return type == SC_UNWIND_EXCEPTION_RESET;
}

sc_unwind_exception::sc_unwind_exception()
	: type(SC_UNWIND_EXCEPTION_KILL)
{
}

sc_unwind_exception::sc_unwind_exception(sc_unwind_exception_type _type)
	: type(_type)
{
}

sc_unwind_exception::sc_unwind_exception(const sc_unwind_exception& exc)
	: type(exc.type)
{
}

sc_unwind_exception::~sc_unwind_exception() throw()
{
}

///////////////////////////////// sc_process_handle ///////////////////////////////////////////

unsigned int sc_process_handle::num_instances = 0;
sc_event *sc_process_handle::null_event = 0;
std::vector<sc_object*> sc_process_handle::no_child_objects;
std::vector<sc_event*> sc_process_handle::no_child_events;

sc_process_handle::sc_process_handle()
	: process(0)
{
	if(!null_event)
	{
		null_event = new sc_event(__LIBIEEE1666_KERNEL_PREFIX__ "process_handle_null_event");
	}
	num_instances++;
}

sc_process_handle::sc_process_handle(const sc_process_handle& process_handle)
	: process(process_handle.process)
{
	if(!null_event)
	{
		null_event = new sc_event(__LIBIEEE1666_KERNEL_PREFIX__ "process_handle_null_event");
	}
	num_instances++;

	if(process) process->acquire();
}

sc_process_handle::sc_process_handle(sc_object* object)
	: process((sc_process *) object)
{
	if(!null_event)
	{
		null_event = new sc_event(__LIBIEEE1666_KERNEL_PREFIX__ "process_handle_null_event");
	}
	num_instances++;

	if(process) process->acquire();
}

sc_process_handle::sc_process_handle(sc_process *_process)
	: process(_process)
{
	if(!null_event)
	{
		null_event = new sc_event(__LIBIEEE1666_KERNEL_PREFIX__ "process_handle_null_event");
	}
	num_instances++;

	if(process) process->acquire();
}

sc_process_handle::~sc_process_handle()
{
	if(process) process->release();
	
	if((--num_instances == 0) && null_event)
	{
		delete null_event;
	}
}

bool sc_process_handle::valid() const
{
	return process && !process->terminated();
}

sc_process_handle& sc_process_handle::operator = (const sc_process_handle& process_handle)
{
	if(process) process->release();
	process = process_handle.process;
	process->acquire();
	return *this;
}

bool sc_process_handle::operator == (const sc_process_handle& process_handle) const
{
	return process == process_handle.process;
}

bool sc_process_handle::operator != (const sc_process_handle& process_handle) const
{
	return process != process_handle.process;
}

bool sc_process_handle::operator < (const sc_process_handle& process_handle) const
{
	return process < process_handle.process;
}

void sc_process_handle::swap(sc_process_handle& process_handle)
{
	sc_process *p = process;
	process = process_handle.process;
	process_handle.process = p;
}

const char* sc_process_handle::name() const
{
	return process ? process->name() : "";
}

sc_curr_proc_kind sc_process_handle::proc_kind() const
{
	return process ? process->proc_kind() : SC_NO_PROC_;
}

const std::vector<sc_object*>& sc_process_handle::get_child_objects() const
{
	return process ? process->get_child_objects() : no_child_objects;
}

const std::vector<sc_event*>& sc_process_handle::get_child_events() const
{
	return process ? process->get_child_events() : no_child_events;
}

sc_object* sc_process_handle::get_parent_object() const
{
	return process ? process->get_parent_object() : 0;
}

sc_object* sc_process_handle::get_process_object() const
{
	return (process && !process->terminated()) ? process : 0;
}

bool sc_process_handle::dynamic() const
{
	return process ? process->dynamic() : false;
}

bool sc_process_handle::terminated() const
{
	return process ? process->terminated() : false;
}

const sc_event& sc_process_handle::terminated_event() const
{
	return process ? process->terminated_event() : *null_event;
}

void sc_process_handle::suspend(sc_descendant_inclusion_info include_descendants)
{
	if(process) process->suspend(include_descendants);
}

void sc_process_handle::resume(sc_descendant_inclusion_info include_descendants)
{
	if(process) process->resume(include_descendants);
}

void sc_process_handle::disable(sc_descendant_inclusion_info include_descendants)
{
	if(process) process->disable(include_descendants);
}

void sc_process_handle::enable(sc_descendant_inclusion_info include_descendants)
{
	if(process) process->enable(include_descendants);
}

void sc_process_handle::kill(sc_descendant_inclusion_info include_descendants)
{
	if(process) process->kill(include_descendants);
}

void sc_process_handle::reset(sc_descendant_inclusion_info include_descendants)
{
	if(process) process->reset(include_descendants);
}

bool sc_process_handle::is_unwinding() const
{
	return process ? process->is_unwinding() : false;
}

const sc_event& sc_process_handle::reset_event() const
{
	return process ? process->reset_event() : *null_event;
}

void sc_process_handle::sync_reset_on( sc_descendant_inclusion_info include_descendants)
{
}

void sc_process_handle::sync_reset_off( sc_descendant_inclusion_info include_descendants)
{
}

} // end of namespace sc_core
