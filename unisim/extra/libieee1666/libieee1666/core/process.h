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

#ifndef __LIBIEEE1666_CORE_PROCESS_H__
#define __LIBIEEE1666_CORE_PROCESS_H__

#include "core/fwd.h"
#include "core/object.h"
#include "core/reset.h"
#include "channels/in.h"
#include "channels/out.h"
#include "channels/inout.h"
#include "channels/signal_if.h"
#include <list>

namespace sc_core {

class sc_process_owner 
{
public:
	sc_process_owner(bool automatic = false);
	virtual ~sc_process_owner();
	
	bool is_automatic() const;
private:
	bool automatic;
};

class sc_user_exception
{
public:
	virtual void throw_it() const = 0;
};

class sc_process : public sc_object
{
public:
	sc_process(const char *name, sc_process_owner *process_owner, sc_process_owner_method_ptr process_owner_method_ptr, sc_curr_proc_kind process_kind, const sc_spawn_options *spawn_options);
	virtual ~sc_process();
	
	void call_process_owner_method();
	
	sc_curr_proc_kind proc_kind() const;
	bool dynamic() const;
	bool dont_initialize() const;
	virtual const char *kind() const;
	
	virtual bool terminated() const = 0;
	virtual const sc_event& terminated_event() const = 0;
	virtual bool is_unwinding() const = 0;
	virtual const sc_event& reset_event() const = 0;
	
	virtual void suspend(sc_descendant_inclusion_info include_descendants);
	virtual void resume(sc_descendant_inclusion_info include_descendants);
	virtual void disable(sc_descendant_inclusion_info include_descendants);
	virtual void enable(sc_descendant_inclusion_info include_descendants);
	virtual void kill(sc_descendant_inclusion_info include_descendants);
	virtual void reset(sc_descendant_inclusion_info include_descendants);
	virtual void throw_it(const sc_user_exception& user_defined_exception, sc_descendant_inclusion_info include_descendants);

	virtual void suspend() = 0;
	virtual void resume() = 0;
	virtual void disable() = 0;
	virtual void enable() = 0;
	virtual void kill() = 0;
	virtual void reset(bool async) = 0;
	virtual void throw_it(const sc_user_exception& user_exception) = 0;

	void reset_signal_is(const sc_in<bool>&, bool);
	void reset_signal_is(const sc_inout<bool>&, bool);
	void reset_signal_is(const sc_out<bool>&, bool);
	void reset_signal_is(const sc_signal_in_if<bool>&, bool);
	void async_reset_signal_is(const sc_in<bool>&, bool);
	void async_reset_signal_is(const sc_inout<bool>&, bool);
	void async_reset_signal_is(const sc_out<bool>&, bool);
	void async_reset_signal_is(const sc_signal_in_if<bool>&, bool);
	
	void acquire();
	void release();
	
	void make_statically_sensitive(const sc_event& e);
	void make_statically_sensitive(const sc_interface& itf);
	void make_statically_sensitive(const sc_port_base& port);
	void make_statically_sensitive(const sc_export_base& exp);
	void make_statically_sensitive(const sc_event_finder& event_finder);
	
	void finalize_elaboration();
	
	void reset_signal_value_changed(bool reset_signal_value, bool async, bool active_level);
	
private:
	friend class sc_kernel;
	
	sc_process_owner *process_owner;
	sc_process_owner_method_ptr process_owner_method_ptr;
	sc_curr_proc_kind process_kind;
	bool flag_dynamic;
	bool flag_dont_initialize;
	bool automatic_process_owner;
	bool trigger_requested;
	std::vector<sc_process_reset_bind_info> process_reset_bind_infos;

	unsigned int ref_count;

	void make_statically_sensitive(const sc_spawn_options *spawn_options);
protected:
	bool enabled;
	bool suspended;
	bool runnable_on_resuming;
};

} // end of namespace sc_core

#endif
