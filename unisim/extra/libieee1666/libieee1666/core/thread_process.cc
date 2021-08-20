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

#include "core/thread_process.h"
#include "core/kernel.h"
#include "core/spawn.h"
#include "core/event.h"
#include <stdexcept>

namespace sc_core {

sc_thread_process::sc_thread_process(const char *_name, sc_process_owner *_process_owner, sc_process_owner_method_ptr _process_owner_method_ptr, bool clocked, const sc_spawn_options *spawn_options)
	: sc_process(_name, _process_owner, _process_owner_method_ptr, clocked ? SC_CTHREAD_PROC_ : SC_THREAD_PROC_, spawn_options)
	, coroutine(0)
	, stack_size(spawn_options ? spawn_options->get_stack_size() : 0)
	, started(false)
	, flag_killed(false)
	, flag_reset(false)
	, flag_is_unwinding(false)
	, flag_throw_it(false)
	, thread_process_terminated(false)
	, thread_process_terminated_event(__LIBIEEE1666_KERNEL_PREFIX__ "_terminated_event")
	, thread_process_reset_event(__LIBIEEE1666_KERNEL_PREFIX__ "_reset_event")
	, user_exception(0)
	, wait_type(WAIT_DEFAULT)
	, wait_count(0)
	, wait_event(0)
	, wait_event_list(0)
	, wait_and_event_list_remaining_count(0)
	, wait_time_out_event(__LIBIEEE1666_KERNEL_PREFIX__ "_wait_time_out_event")
{
	coroutine = kernel->get_coroutine_system()->create_coroutine(stack_size, &sc_thread_process::coroutine_work, reinterpret_cast<intptr_t>(this));
	kernel->register_thread_process(this);
}

sc_thread_process::~sc_thread_process()
{
	kill();
	delete coroutine;

	kernel->unregister_thread_process(this);
}

void sc_thread_process::set_stack_size(int _stack_size)
{
	stack_size = _stack_size;
}

void sc_thread_process::yield(sc_coroutine *next_coroutine)
{
	coroutine->yield(next_coroutine);
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

void sc_thread_process::yield(sc_thread_process *next_thread_process)
{
	kernel->set_current_thread_process(next_thread_process);
	yield(next_thread_process->coroutine);
}

void sc_thread_process::yield()
{
	yield(kernel->get_next_coroutine());
}

void sc_thread_process::trigger_statically()
{
	if(wait_type != WAIT_DEFAULT)
	{
		// call to wait(...) with one or more non-integer arguments overrides static sensitivity
		return;
	}
	
	if(wait_count > 0)
	{
		wait_count--;
		return;
	}
	
	if(enabled)
	{
		if(suspended)
		{
			runnable_on_resuming = true;
		}
		else
		{
			kernel->trigger(this);
		}
	}
}

void sc_thread_process::trigger_dynamically(const sc_event *triggered_event)
{
	bool trigger_cond = false;
	
	if(enabled)
	{
		switch(wait_type)
		{
			case WAIT_DEFAULT: // wait() => static sensitivity
				// should not reach that point
				break;
			case WAIT_EVENT: // wait(e)
				wait_type = WAIT_DEFAULT;
				trigger_cond = true;
				break;
			case WAIT_EVENT_AND_LIST: // wait(e1 & ... & en)
				if(--wait_and_event_list_remaining_count == 0)
				{
					// got all events in the event AND list
					wait_event_list->release();
					wait_type = WAIT_DEFAULT;
					trigger_cond = true;
				}
				break;
			case WAIT_EVENT_OR_LIST: // wait(e1 | ... | en)
				// got one event from the event OR list
				// avoid all events in the event OR list to trigger again the thread process
				wait_event_list->remove_dynamically_sensitive_thread_process(this);
				wait_event_list->release();
				wait_type = WAIT_DEFAULT;
				trigger_cond = true;
				break;
			case WAIT_TIME_OUT: // wait(t)
				wait_type = WAIT_DEFAULT;
				trigger_cond = true;
				break;
			case WAIT_TIME_OUT_OR_EVENT: // wait(t, e)
				if(triggered_event == &wait_time_out_event)
				{
					// got time out event first
					wait_event->remove_dynamically_sensitive_thread_process(this);
				}
				else // triggered_event == wait_event
				{
					// got event first: canceling time out event
					wait_time_out_event.cancel();
					wait_time_out_event.clear_dynamically_sensitive_processes();
				}
				wait_type = WAIT_DEFAULT;
				trigger_cond = true;
				break;

			case WAIT_TIME_OUT_OR_EVENT_AND_LIST: // wait(t, e1 & ... & en)
				if(triggered_event == &wait_time_out_event)
				{
					// got time out event first
					wait_event_list->remove_dynamically_sensitive_thread_process(this);
					wait_type = WAIT_DEFAULT;
					trigger_cond = true;
				}
				else if(--wait_and_event_list_remaining_count == 0)
				{
					// got all events in the event AND list first: canceling time out event
					wait_time_out_event.cancel();
					wait_time_out_event.clear_dynamically_sensitive_processes();
					
					wait_event_list->release();
					wait_type = WAIT_DEFAULT;
					trigger_cond = true;
				}
				break;

			case WAIT_TIME_OUT_OR_EVENT_OR_LIST: // wait(t, e1 | ... \ en)
				if(triggered_event == &wait_time_out_event)
				{
					// got time out event first
					wait_event_list->remove_dynamically_sensitive_thread_process(this);
				}
				else
				{
					// got an event from event OR list first: canceling time out event
					wait_time_out_event.cancel();
					wait_time_out_event.clear_dynamically_sensitive_processes();

					wait_event_list->remove_dynamically_sensitive_thread_process(this);
					wait_event_list->release();
				}
				wait_type = WAIT_DEFAULT;
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
			kernel->trigger(this);
		}
	}
}

void sc_thread_process::wait()
{
	wait_type = WAIT_DEFAULT;
//	std::cerr << name() << "::wait()" << std::endl;
	yield();
}

void sc_thread_process::wait(int n)
{
	if(n <= 0) throw std::runtime_error("wait argument shall be greater than zero");
	
	wait_type = WAIT_DEFAULT;
	wait_count = n - 1;
//	std::cerr << name() << "::wait(" << n << ")" << std::endl;
	yield();
}

void sc_thread_process::wait(const sc_event& e)
{
	wait_type = WAIT_EVENT;
	wait_event = &e;
	wait_event->add_dynamically_sensitive_thread_process(this);
//	std::cerr << name() << "::wait(" << e.name() << ")" << std::endl;
	yield();
}

void sc_thread_process::wait(const sc_event_and_list& el)
{
	wait_type = WAIT_EVENT_AND_LIST;
	wait_event_list = &el;
	wait_event_list->acquire();
	wait_event_list->add_dynamically_sensitive_thread_process(this);
	wait_and_event_list_remaining_count = wait_event_list->size();
//	std::cerr << name() << "::wait(and_list)" << std::endl;
	yield();
}

void sc_thread_process::wait(const sc_event_or_list& el)
{
	wait_type = WAIT_EVENT_OR_LIST;
	wait_event_list = &el;
	wait_event_list->acquire();
	wait_event_list->add_dynamically_sensitive_thread_process(this);
	yield();
//	std::cerr << name() << "::wait(or_list)" << std::endl;
}

void sc_thread_process::wait(const sc_time& t)
{
	wait_type = WAIT_TIME_OUT;
	wait_time_out = t;
	wait_time_out_event.add_dynamically_sensitive_thread_process(this);
	wait_time_out_event.notify(wait_time_out);
//	std::cerr << name() << "::wait(" << t << ")" << std::endl;
	yield();
}

void sc_thread_process::wait(const sc_time& t, const sc_event& e)
{
	wait_type = WAIT_TIME_OUT_OR_EVENT;
	wait_time_out = t;
	wait_time_out_event.add_dynamically_sensitive_thread_process(this);
	wait_time_out_event.notify(wait_time_out);
	wait_event = &e;
	wait_event->add_dynamically_sensitive_thread_process(this);
//	std::cerr << name() << "::wait(" << t << ", " << e.name() << ")" << std::endl;
	yield();
}

void sc_thread_process::wait(const sc_time& t, const sc_event_and_list& el)
{
	wait_type = WAIT_TIME_OUT_OR_EVENT_AND_LIST;
	wait_time_out = t;
	wait_time_out_event.add_dynamically_sensitive_thread_process(this);
	wait_time_out_event.notify(wait_time_out);
	wait_event_list = &el;
	wait_event_list->acquire();
	wait_event_list->add_dynamically_sensitive_thread_process(this);
	yield();
//	std::cerr << name() << "::wait(" << t << ", and_list)" << std::endl;
}

void sc_thread_process::wait(const sc_time& t, const sc_event_or_list& el)
{
	wait_type = WAIT_TIME_OUT_OR_EVENT_OR_LIST;
	wait_time_out = t;
	wait_time_out_event.add_dynamically_sensitive_thread_process(this);
	wait_time_out_event.notify(wait_time_out);
	wait_event_list = &el;
	wait_event_list->acquire();
	wait_event_list->add_dynamically_sensitive_thread_process(this);
	yield();
//	std::cerr << name() << "::wait(" << t << ", or_list)" << std::endl;
}

void sc_thread_process::terminate()
{
	thread_process_terminated = true;
	thread_process_terminated_event.notify();
	started = false;
	
	kernel->terminate_thread_process(this);
	
	coroutine->abort(kernel->get_next_coroutine());
}

void sc_thread_process::coroutine_work(intptr_t _self)
{
	sc_thread_process *self = reinterpret_cast<sc_thread_process *>(_self);
	self->started = true;
	
	// SC_THREAD/SC_CTHREAD process method is run once unless process is reset
	while(true)
	{
		try
		{
			self->call_process_owner_method();
		}
		catch(sc_unwind_exception& exc)
		{
			self->flag_is_unwinding = false;
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

	self->terminate();
}

bool sc_thread_process::terminated() const
{
	return thread_process_terminated;
}

const sc_event& sc_thread_process::terminated_event() const
{
	return thread_process_terminated_event;
}

bool sc_thread_process::is_unwinding() const
{
	return flag_is_unwinding;
}

const sc_event& sc_thread_process::reset_event() const
{
	return thread_process_reset_event;
}

void sc_thread_process::suspend()
{
	suspended = true;
}

void sc_thread_process::resume()
{
    suspended = false;

	if(runnable_on_resuming)
	{
		runnable_on_resuming = false;
		
		kernel->trigger(this);
	}
}

void sc_thread_process::disable()
{
	enabled = false;
}

void sc_thread_process::enable()
{
	enabled = true;
}

void sc_thread_process::kill()
{
	if(!started) return;
	
	enabled = false;
	
	if(!thread_process_terminated)
	{
		flag_killed = true;
		
		// deschedule target process
		kernel->untrigger(this);

		if(kernel->get_current_thread_process() == this)
		{
			// suicide
			flag_is_unwinding = true;
			throw sc_unwind_exception(SC_UNWIND_EXCEPTION_KILL);
		}
		else
		{
			// kill requested by another method process or kernel
			kernel->kill_thread_process(this);
		}
	}
	
	started = false;
}

void sc_thread_process::reset(bool async)
{
	if(!started) return;
	
	wait_type = WAIT_DEFAULT; // restore static sensitivity
	
	// immediate notification of reset
	thread_process_reset_event.notify();

	flag_reset = true;
	
	if(started)
	{
		if(!thread_process_terminated)
		{
			// deschedule target process
			kernel->untrigger(this);

			if(kernel->get_current_thread_process() == this)
			{
				// self reset
				flag_is_unwinding = true;
				throw sc_unwind_exception(SC_UNWIND_EXCEPTION_RESET);
			}
			else
			{
				kernel->reset_thread_process(this);
			}
		}
	}
}

void sc_thread_process::throw_it(const sc_user_exception& user_exception)
{
	wait_type = WAIT_DEFAULT; // restore static sensitivity
	flag_throw_it = true;
	
	if(started)
	{
		if(!thread_process_terminated)
		{
			sc_thread_process *current_thread_process = kernel->get_current_thread_process();
			
			if(current_thread_process == this)
			{
				// self throw
				user_exception.throw_it(); 
			}
			else if(current_thread_process)
			{
				// throw requested by another thread process
				this->user_exception = &user_exception;
				current_thread_process->yield(this); // switch to thread being thrown and let thread throw itself
			}
			else
			{
				// reset requested by another method process or kernel
				this->user_exception = &user_exception;
				kernel->get_coroutine_system()->get_main_coroutine()->yield(coroutine);  // switch to thread being thrown and let thread throw itself
			}
		}
	}
}

const char *sc_thread_process::kind() const
{
	return "sc_thread_process";
}

} // end of namespace sc_core
