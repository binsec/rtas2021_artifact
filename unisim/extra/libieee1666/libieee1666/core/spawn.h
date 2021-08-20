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

#ifndef __LIBIEEE1666_CORE_SPAWN_H__
#define __LIBIEEE1666_CORE_SPAWN_H__

#include "core/fwd.h"
#include "channels/fwd.h"
#include "core/process_handle.h"
#include "core/process.h"
#include "core/kernel.h"
#include <vector>
#include <boost/bind.hpp>

namespace sc_core {

template <typename T>
sc_process_handle sc_spawn(
	T object ,
	const char* name_p = 0 ,
	const sc_spawn_options* opt_p = 0 );

template <typename T>
sc_process_handle sc_spawn(
	typename T::result_type* r_p ,
	T object ,
	const char* name_p = 0 ,
	const sc_spawn_options* opt_p = 0 );

class sc_spawn_options
{
public:
	sc_spawn_options();
	void spawn_method();
	void dont_initialize();
	void set_stack_size( int );
	void set_sensitivity( const sc_event* );
	void set_sensitivity( sc_port_base* );
	void set_sensitivity( sc_export_base* );
	void set_sensitivity( sc_interface* );
	void set_sensitivity( sc_event_finder* );
	void reset_signal_is( const sc_in<bool>& , bool );
	void reset_signal_is( const sc_inout<bool>& , bool );
	void reset_signal_is( const sc_out<bool>& , bool );
	void reset_signal_is( const sc_signal_in_if<bool>& , bool );
	void async_reset_signal_is( const sc_in<bool>& , bool );
	void async_reset_signal_is( const sc_inout<bool>& , bool );
	void async_reset_signal_is( const sc_out<bool>& , bool );
	void async_reset_signal_is( const sc_signal_in_if<bool>& , bool );

	bool get_flag_spawn_method() const;
	bool get_flag_dont_initialize() const;
	int get_stack_size() const;
	const std::vector<const sc_event *>& get_sensitive_events() const;
	const std::vector<const sc_port_base *>& get_sensitive_ports() const;
	const std::vector<const sc_export_base *>& get_sensitive_exports() const;
	const std::vector<const sc_interface *>& get_sensitive_interfaces() const;
	const std::vector<const sc_event_finder *>& get_sensitive_event_finders() const;
private:
	// Disabled
	sc_spawn_options( const sc_spawn_options& );
	sc_spawn_options& operator= ( const sc_spawn_options& );
	
	//////////////////////////////////////////////
	
	bool flag_spawn_method;
	bool flag_dont_initialize;
	int stack_size;
	std::vector<const sc_event *> sensitive_events;
	std::vector<const sc_port_base *> sensitive_ports;
	std::vector<const sc_export_base *> sensitive_exports;
	std::vector<const sc_interface *> sensitive_interfaces;
	std::vector<const sc_event_finder *> sensitive_event_finders;
};

template <typename T>
class sc_spawn_process_owner_trampoline : public sc_process_owner
{
public:
	sc_spawn_process_owner_trampoline(T object);
	virtual ~sc_spawn_process_owner_trampoline();
	
	void trampoline();
	
private:
	T object;
};

template <typename T>
sc_spawn_process_owner_trampoline<T>::sc_spawn_process_owner_trampoline(T _object)
	: sc_process_owner(true /* automatically allocated/freed */)
	, object(_object)
{
}

template <typename T>
sc_spawn_process_owner_trampoline<T>::~sc_spawn_process_owner_trampoline()
{
}

template <typename T>
void sc_spawn_process_owner_trampoline<T>::trampoline()
{
	object();
}

template <typename T>
class sc_spawn_process_owner_trampoline_with_return : public sc_process_owner
{
public:
	sc_spawn_process_owner_trampoline_with_return(T object, typename T::result_type *_result);
	virtual ~sc_spawn_process_owner_trampoline_with_return();
	
	void trampoline();
	
private:
	T object;
	typename T::result_type *result;
};

template <typename T>
sc_spawn_process_owner_trampoline_with_return<T>::sc_spawn_process_owner_trampoline_with_return(T _object, typename T::result_type *_result)
	: sc_process_owner(true /* automatically allocated/freed */)
	, object(_object)
	, result(_result)
{
}

template <typename T>
sc_spawn_process_owner_trampoline_with_return<T>::~sc_spawn_process_owner_trampoline_with_return()
{
}

template <typename T>
void sc_spawn_process_owner_trampoline_with_return<T>::trampoline()
{
	*result = object();
}

template <typename T>
sc_process_handle sc_spawn(
	T object,
	const char* name_p,
	const sc_spawn_options* opt_p)
{
	if(opt_p && opt_p->get_flag_spawn_method())
	{
		return sc_kernel::get_kernel()->create_method_process((!name_p || (*name_p == 0)) ? __LIBIEEE1666_KERNEL_PREFIX__ "_sc_method_process" : name_p, new sc_spawn_process_owner_trampoline<T>(object), static_cast<sc_core::sc_process_owner_method_ptr>(&sc_spawn_process_owner_trampoline<T>::trampoline), opt_p);
	}
	else
	{
		return sc_kernel::get_kernel()->create_thread_process((!name_p || (*name_p == 0)) ? __LIBIEEE1666_KERNEL_PREFIX__ "_sc_thread_process" : name_p, new sc_spawn_process_owner_trampoline<T>(object), static_cast<sc_core::sc_process_owner_method_ptr>(&sc_spawn_process_owner_trampoline<T>::trampoline), opt_p);
	}
}

template <typename T>
sc_process_handle sc_spawn(
	typename T::result_type* r_p,
	T object,
	const char* name_p,
	const sc_spawn_options* opt_p)
{
	if(opt_p->get_flag_spawn_method())
	{
		return sc_kernel::get_kernel()->create_method_process(name_p, new sc_spawn_process_owner_trampoline_with_return<T>(object, r_p), &sc_spawn_process_owner_trampoline_with_return<T>::trampoline, opt_p);
	}
	else
	{
		return sc_kernel::get_kernel()->create_thread_process(name_p, new sc_spawn_process_owner_trampoline_with_return<T>(object, r_p), &sc_spawn_process_owner_trampoline_with_return<T>::trampoline, opt_p);
	}
}

#define sc_bind boost::bind
#define sc_ref(r) boost::ref(r)
#define sc_cref(r) boost::cref(r)

#define SC_FORK \
{\
	sc_core::sc_process_handle spawned_process_handles[] = {
	
#define SC_JOIN \
	};\
	sc_core::sc_event_and_list terminated_event_and_list;\
	unsigned int i;\
	for(i = 0; i < sizeof(spawned_process_handles) / sizeof(sc_core::sc_process_handle); i++)\
	{\
		terminated_event_and_list &= spawned_process_handles[i].terminated_event();\
	}\
	sc_core::wait(terminated_event_and_list);\
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

#endif
