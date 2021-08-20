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

#include "core/method_process.h"
#include "core/kernel.h"

namespace sc_core {

sc_method_process::sc_method_process(const char *_name, sc_process_owner *_process_owner, sc_process_owner_method_ptr _process_owner_method_ptr, const sc_spawn_options *spawn_options)
	: sc_process(_name, _process_owner, _process_owner_method_ptr, SC_METHOD_PROC_, spawn_options)
	, started(false)
	, flag_killed(false)
	, flag_reset(false)
	, flag_is_unwinding(false)
	, flag_throw_it(false)
	, method_process_terminated(false)
	, method_process_terminated_event(__LIBIEEE1666_KERNEL_PREFIX__ "_terminated_event")
	, method_process_reset_event(__LIBIEEE1666_KERNEL_PREFIX__ "_reset_event")
	, user_exception(0)
	, next_trigger_type(NEXT_TRIGGER_DEFAULT)
	, next_trigger_event(0)
	, next_trigger_event_list(0)
	, next_trigger_and_event_list_remaining_count(0)
	, next_trigger_time_out_event(__LIBIEEE1666_KERNEL_PREFIX__ "_next_trigger_time_out_event")
{
	kernel->register_method_process(this);
}

sc_method_process::~sc_method_process()
{
	kernel->unregister_method_process(this);
}

void sc_method_process::run()
{
	started = true;

	while(true)
	{
		try
		{
			call_process_owner_method();
		}
		catch(sc_unwind_exception& exc)
		{
			flag_is_unwinding = false;
			if(exc.is_reset()) continue;
		}
		catch(std::exception& exc)
		{
			std::cerr << "unhandled exception: " << exc.what() << std::endl;
		}
		catch(sc_user_exception& exc)
		{
			std::cerr << "unhandled user exception" << std::endl;
		}
		catch(...)
		{
			std::cerr << "unhandled exception" << std::endl;
		}
		
		break;
	}
}

void sc_method_process::check_exceptions()
{
	if(flag_is_unwinding) return;
	
	if(flag_killed)
	{
		flag_killed = false;
		flag_is_unwinding = true;
		throw sc_unwind_exception(SC_UNWIND_EXCEPTION_KILL);
	}
	else if(flag_reset)
	{
		flag_reset = false;
		flag_is_unwinding = true;
		throw sc_unwind_exception(SC_UNWIND_EXCEPTION_RESET);
	}
	else if(flag_throw_it)
	{
		flag_throw_it = false;
		user_exception->throw_it();
	}
}

void sc_method_process::trigger_statically()
{
	if(next_trigger_type != NEXT_TRIGGER_DEFAULT)
	{
		// call to next_trigger(...) with one or more arguments overrides static sensitivity
		return;
	}
	
	if(suspended)
	{
		runnable_on_resuming = true;
	}
	else
	{
		kernel->trigger(this);
	}
}

void sc_method_process::trigger_dynamically(const sc_event *triggered_event)
{
	bool trigger_cond = false;
	
	if(enabled)
	{
		switch(next_trigger_type)
		{
			case NEXT_TRIGGER_DEFAULT: // next_trigger() => static sensitivity
				// should not reach that point
				break;

			case NEXT_TRIGGER_EVENT: // next_trigger(e)
				next_trigger_type = NEXT_TRIGGER_DEFAULT;
				trigger_cond = true;
				break;

			case NEXT_TRIGGER_EVENT_AND_LIST: // next_trigger(e1 & ... & en)
				if(--next_trigger_and_event_list_remaining_count == 0)
				{
					// got all events in the event AND list
					next_trigger_event_list->release();
					next_trigger_type = NEXT_TRIGGER_DEFAULT;
					trigger_cond = true;
				}
				break;

			case NEXT_TRIGGER_EVENT_OR_LIST: // next_trigger(e1 | ... | en)
				// got one event from the event OR list
				// avoid all events in the event OR list to trigger again the method process
				next_trigger_event_list->remove_dynamically_sensitive_method_process(this);
				next_trigger_event_list->release();
				next_trigger_type = NEXT_TRIGGER_DEFAULT;
				trigger_cond = true;
				break;

			case NEXT_TRIGGER_TIME_OUT: // next_trigger(t)
				next_trigger_type = NEXT_TRIGGER_DEFAULT;
				trigger_cond = true;
				break;

			case NEXT_TRIGGER_TIME_OUT_OR_EVENT: // next_trigger(t, e)
				if(triggered_event == &next_trigger_time_out_event)
				{
					// got time out event first
					next_trigger_event->remove_dynamically_sensitive_method_process(this);
				}
				else // triggered_event == next_trigger_event
				{
					// got event first: canceling time out event
					next_trigger_time_out_event.cancel();
					next_trigger_time_out_event.clear_dynamically_sensitive_processes();
				}
				next_trigger_type = NEXT_TRIGGER_DEFAULT;
				trigger_cond = true;
				break;

			case NEXT_TRIGGER_TIME_OUT_OR_EVENT_AND_LIST: // next_trigger(t, e1 & ... & en)
				if(triggered_event == &next_trigger_time_out_event)
				{
					// got time out event first
					next_trigger_event_list->remove_dynamically_sensitive_method_process(this);
					next_trigger_type = NEXT_TRIGGER_DEFAULT;
					trigger_cond = true;
				}
				else if(--next_trigger_and_event_list_remaining_count == 0)
				{
					// got all events in the event AND list first: canceling time out event
					next_trigger_time_out_event.cancel();
					next_trigger_time_out_event.clear_dynamically_sensitive_processes();
					
					next_trigger_event_list->release();
					next_trigger_type = NEXT_TRIGGER_DEFAULT;
					trigger_cond = true;
				}
				break;

			case NEXT_TRIGGER_TIME_OUT_OR_EVENT_OR_LIST: // next_trigger(t, e1 | ... | en)
				if(triggered_event == &next_trigger_time_out_event)
				{
					// got time out event first
					next_trigger_event_list->remove_dynamically_sensitive_method_process(this);
				}
				else
				{
					// got an event from event OR list first: canceling time out event
					next_trigger_time_out_event.cancel();
					next_trigger_time_out_event.clear_dynamically_sensitive_processes();

					next_trigger_event_list->remove_dynamically_sensitive_method_process(this);
					next_trigger_event_list->release();
				}
				next_trigger_type = NEXT_TRIGGER_DEFAULT;
				trigger_cond = true;
				break;
		}
	}
	
	if(trigger_cond)
	{
		if(suspended)
		{
			runnable_on_resuming = true;
		}
		else
		{
			sc_kernel::get_kernel()->trigger(this);
		}
	}
}

void sc_method_process::next_trigger()
{
// 	std::cerr << name() << ".next_trigger()" << std::endl;
	cancel_next_trigger();
}

void sc_method_process::next_trigger(const sc_event& e)
{
// 	std::cerr << name() << ".next_trigger(e=" << e.name() << ")" << std::endl;
	cancel_next_trigger();
	next_trigger_type = NEXT_TRIGGER_EVENT;
	next_trigger_event = &e;
}

void sc_method_process::next_trigger(const sc_event_and_list& el)
{
	cancel_next_trigger();
	next_trigger_type = NEXT_TRIGGER_EVENT_AND_LIST;
	next_trigger_event_list = &el;
	next_trigger_event_list->acquire();
	next_trigger_and_event_list_remaining_count = next_trigger_event_list->size();
}

void sc_method_process::next_trigger(const sc_event_or_list& el)
{
	cancel_next_trigger();
	next_trigger_type = NEXT_TRIGGER_EVENT_OR_LIST;
	next_trigger_event_list = &el;
	next_trigger_event_list->acquire();
}

void sc_method_process::next_trigger(const sc_time& t)
{
	cancel_next_trigger();
	next_trigger_type = NEXT_TRIGGER_TIME_OUT;
	next_trigger_time_out = t;
}

void sc_method_process::next_trigger(const sc_time& t, const sc_event& e)
{
	cancel_next_trigger();
	next_trigger_type = NEXT_TRIGGER_TIME_OUT_OR_EVENT;
	next_trigger_time_out = t;
	next_trigger_event = &e;	
}

void sc_method_process::next_trigger(const sc_time& t, const sc_event_and_list& el)
{
	cancel_next_trigger();
	next_trigger_type = NEXT_TRIGGER_TIME_OUT_OR_EVENT_AND_LIST;
	next_trigger_time_out = t;
	next_trigger_event_list = &el;
	next_trigger_event_list->acquire();
}

void sc_method_process::next_trigger(const sc_time& t, const sc_event_or_list& el)
{
	cancel_next_trigger();
	next_trigger_type = NEXT_TRIGGER_TIME_OUT_OR_EVENT_OR_LIST;
	next_trigger_time_out = t;
	next_trigger_event_list = &el;
	next_trigger_event_list->acquire();
}

void sc_method_process::cancel_next_trigger()
{
	switch(next_trigger_type)
	{
		case NEXT_TRIGGER_DEFAULT:
			break;
		case NEXT_TRIGGER_EVENT:
			break;
		case NEXT_TRIGGER_EVENT_AND_LIST:
		case NEXT_TRIGGER_EVENT_OR_LIST:
		case NEXT_TRIGGER_TIME_OUT_OR_EVENT_AND_LIST:
		case NEXT_TRIGGER_TIME_OUT_OR_EVENT_OR_LIST:
			next_trigger_event_list->release();
			break;
		case NEXT_TRIGGER_TIME_OUT:
			break;
		case NEXT_TRIGGER_TIME_OUT_OR_EVENT:
			break;
	}
	
	next_trigger_type = NEXT_TRIGGER_DEFAULT;
}

void sc_method_process::commit_next_trigger()
{
	switch(next_trigger_type)
	{
		case NEXT_TRIGGER_DEFAULT:
			break;
		case NEXT_TRIGGER_EVENT:
			next_trigger_event->add_dynamically_sensitive_method_process(this);
			break;
		case NEXT_TRIGGER_EVENT_AND_LIST:
			next_trigger_event_list->add_dynamically_sensitive_method_process(this);
			break;
		case NEXT_TRIGGER_EVENT_OR_LIST:
			next_trigger_event_list->add_dynamically_sensitive_method_process(this);
			break;
		case NEXT_TRIGGER_TIME_OUT:
			next_trigger_time_out_event.add_dynamically_sensitive_method_process(this);
			next_trigger_time_out_event.notify(next_trigger_time_out);
			break;
		case NEXT_TRIGGER_TIME_OUT_OR_EVENT:
			next_trigger_time_out_event.add_dynamically_sensitive_method_process(this);
			next_trigger_time_out_event.notify(next_trigger_time_out);
			next_trigger_event->add_dynamically_sensitive_method_process(this);
			break;
		case NEXT_TRIGGER_TIME_OUT_OR_EVENT_AND_LIST:
			next_trigger_time_out_event.add_dynamically_sensitive_method_process(this);
			next_trigger_time_out_event.notify(next_trigger_time_out);
			next_trigger_event_list->add_dynamically_sensitive_method_process(this);
			break;
		case NEXT_TRIGGER_TIME_OUT_OR_EVENT_OR_LIST:
			next_trigger_time_out_event.add_dynamically_sensitive_method_process(this);
			next_trigger_time_out_event.notify(next_trigger_time_out);
			next_trigger_event_list->add_dynamically_sensitive_method_process(this);
			break;
	}
}

bool sc_method_process::terminated() const
{
	return method_process_terminated;
}

const sc_event& sc_method_process::terminated_event() const
{
	return method_process_terminated_event;
}

bool sc_method_process::is_unwinding() const
{
	return flag_is_unwinding;
}

const sc_event& sc_method_process::reset_event() const
{
	return method_process_reset_event;
}

void sc_method_process::suspend()
{
	suspended = true;
}

void sc_method_process::resume()
{
	suspended = false;

	if(runnable_on_resuming)
	{
		runnable_on_resuming = false;
		
		kernel->trigger(this);
	}
}

void sc_method_process::disable()
{
	if(method_process_terminated) return;
	enabled = false;
}

void sc_method_process::enable()
{
	if(method_process_terminated) return;
	enabled = true;
}

void sc_method_process::kill()
{
	if(!started) return;
	
	enabled = false;

	if(!method_process_terminated)
	{
		if(kernel->get_current_method_process() == this)
		{
			// suicide
			flag_is_unwinding = true;
			throw sc_unwind_exception(SC_UNWIND_EXCEPTION_KILL);
		}
		else
		{
			// kill requested by another process
		}
	}
	method_process_terminated = true;
	method_process_terminated_event.notify();
	kernel->terminate_method_process(this);
	
	started = false;
}

void sc_method_process::reset(bool async)
{
	if(!started) return;
	
	next_trigger_type = NEXT_TRIGGER_DEFAULT; // restore static sensitivity

	// immediate notification of reset
	method_process_reset_event.notify();

	if(!method_process_terminated)
	{
		// deschedule target process
		kernel->untrigger(this);

		if(kernel->get_current_method_process() == this)
		{
			// self reset
			flag_is_unwinding = true;
			throw sc_unwind_exception(SC_UNWIND_EXCEPTION_RESET);
		}
		else
		{
			kernel->reset_method_process(this);
		}
	}
}

void sc_method_process::throw_it(const sc_user_exception& _user_exception)
{
	next_trigger_type = NEXT_TRIGGER_DEFAULT; // restore static sensitivity

	if(!method_process_terminated)
	{
		sc_method_process *current_method_process = kernel->get_current_method_process();
		
		if(current_method_process == this)
		{
			// self throw
			_user_exception.throw_it();
		}
		else
		{
			// throw by another process
		}
	}
}

const char *sc_method_process::kind() const
{
	return "sc_method_process";
}

} // end of namespace sc_core
