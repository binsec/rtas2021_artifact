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

#ifndef __LIBIEEE1666_CORE_THREAD_PROCESS_H__
#define __LIBIEEE1666_CORE_THREAD_PROCESS_H__

#include "core/fwd.h"
#include "core/object.h"
#include "core/process.h"
#include "core/time.h"
#include "core/coroutine.h"

namespace sc_core {

class sc_thread_process : public sc_process
{
public:
	
	sc_thread_process(const char *name, sc_process_owner *process_owner, sc_process_owner_method_ptr process_owner_method_ptr, bool clocked, const sc_spawn_options *spawn_options);
	virtual ~sc_thread_process();
	
	void set_stack_size(int stack_size);
	
	void trigger_dynamically(const sc_event *triggered_event);
	void trigger_statically();
	
	void wait();
	void wait(int n);
	void wait(const sc_event& e);
	void wait(const sc_event_and_list& el);
	void wait(const sc_event_or_list& el);
	void wait(const sc_time& t);
	void wait(const sc_time& t, const sc_event& e);
	void wait(const sc_time& t, const sc_event_and_list& el);
	void wait(const sc_time& t, const sc_event_or_list& el);
	
	virtual bool terminated() const;
	virtual const sc_event& terminated_event() const;
	virtual bool is_unwinding() const;
	virtual const sc_event& reset_event() const;
	virtual void suspend();
	virtual void resume();
	virtual void disable();
	virtual void enable();
	virtual void kill();
	virtual void reset(bool async);
	virtual void throw_it(const sc_user_exception& user_exception);
	
	virtual const char *kind() const;
private:
	friend class sc_kernel;

	sc_coroutine *coroutine;
	int stack_size;
	bool started;
	bool flag_killed;
	bool flag_reset;
	bool flag_is_unwinding;
	bool flag_throw_it;
	bool thread_process_terminated;
	sc_event thread_process_terminated_event;
	sc_event thread_process_reset_event;
	const sc_user_exception *user_exception;
	
	enum wait_type_t
	{
		WAIT_DEFAULT,                    // wait() or wait(n)
		WAIT_EVENT,                      // wait(e)
		WAIT_EVENT_AND_LIST,             // wait(e1 & ... & en)
		WAIT_EVENT_OR_LIST,              // wait(e1 | ... | en)
		WAIT_TIME_OUT,                   // wait(t)
		WAIT_TIME_OUT_OR_EVENT,          // wait(t, e)
		WAIT_TIME_OUT_OR_EVENT_AND_LIST, // wait(t, e1 & ... & en)
		WAIT_TIME_OUT_OR_EVENT_OR_LIST   // wait(t, e1 | ... | en)
	};
	
	wait_type_t wait_type;
	int wait_count;
	const sc_event *wait_event;
	const sc_event_list *wait_event_list;
	unsigned int wait_and_event_list_remaining_count;
	sc_time wait_time_out;
	sc_event wait_time_out_event;

	static void coroutine_work(intptr_t self);
	void yield(sc_coroutine *next_coroutine);
	void yield(sc_thread_process *next_thread_process);
	void yield();
	
	void terminate();
};

} // end of namespace sc_core

#endif
