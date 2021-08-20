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

#include "core/kernel.h"
#include "core/module.h"
#include "core/port.h"
#include "core/export.h"
#include "core/module_name.h"
#include "core/cthread_process.h"
#include "core/process_handle.h"
#include "core/time.h"
#include "core/event_finder.h"

#include "core/stack.h"
#include "core/simple_stack.h"

#if __LIBIEEE1666_POSIX_GUARDED_STACK__
#include "core/sysdep/posix_guarded_stack.h"
#endif

#if __LIBIEEE1666_WINDOWS_GUARDED_STACK__
#include "core/sysdep/windows_guarded_stack.h"
#endif

#if __LIBIEEE1666_FCONTEXT__
#include "core/sysdep/fcontext_coroutine.h"
#endif

#if __LIBIEEE1666_UCONTEXT__
#include "core/sysdep/ucontext_coroutine.h"
#endif

#if __LIBIEEE1666_WINDOWS_FIBER__
#include "core/sysdep/windows_fiber_coroutine.h"
#endif

#if __LIBIEEE1666_PTHREAD__
#include "core/sysdep/pthread_coroutine.h"
#endif

#include <math.h>
#include <string.h>
#include <limits>
#include <sstream>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windows.h>
typedef int (*sc_main_type)(int, char **);
#endif

namespace sc_core {

sc_kernel *sc_kernel::kernel = 0;
static int argc;
static char **argv;

static const char *time_unit_strings[SC_SEC + 1] = { "fs", "ps", "ns", "us", "ms", "s" };

sc_kernel::sc_kernel()
	: module_name_stack()
	, module_stack()
	, unique_name_map()
	, top_level_objects()
	, top_level_events()
	, object_registry()
	, event_registry()
	, module_table()
	, port_table()
	, export_table()
	, prim_channel_table()
	, thread_process_table()
	, method_process_table()
	, process_handle_table()
	, coroutine_system(0)
	, stack_system(0)
	, main_coroutine(0)
	, time_resolution_fixed_by_user(false)
	, time_resolution_fixed(false)
	, time_resolution_scale_factors_table_base_index(0)
	, time_resolution_scale_factors_table()
	, max_time()
	, current_object(0)
	, current_writer(0)
	, current_thread_process(0)
	, current_method_process(0)
	, current_time_stamp()
	, kernel_events_allocator()
	, timed_kernel_events_allocator()	
	, runnable_thread_processes()
	, runnable_method_processes()
	, updatable_prim_channels()
	, delta_events() 
	, schedule()
	, terminated_thread_processes()
	, terminated_method_processes()
	, user_requested_stop(false)
	, user_requested_pause(false)
	, stop_mode(SC_STOP_IMMEDIATE)
	, status(SC_ELABORATION)
	, initialized(false)
	, end_of_simulation_invoked(false)
	, start_of_simulation_invoked(false)
	, delta_count(0)
	, debug(false)
{
	char *libieee1666_debug = getenv("LIBIEEE1666_DEBUG");

	if(libieee1666_debug)
	{
		if((strcmp(libieee1666_debug, "yes") == 0) ||
		   (strcmp(libieee1666_debug, "y") == 0) ||
		   (strcmp(libieee1666_debug, "1") == 0))
		{
			std::cerr << "libieee1666: enabling debug messages" << std::endl;
			debug = true;
		}
	}
	
	char *libieee1666_stack_system = getenv("LIBIEEE1666_STACK_SYSTEM");
	
	do
	{
		if(libieee1666_stack_system)
		{
#if __LIBIEEE1666_POSIX_GUARDED_STACK__
			if(strcmp(libieee1666_stack_system, "POSIX_GUARDED") == 0)
			{
				std::cerr << "using POSIX guarded stack system as requested by user" << std::endl;
				stack_system = new sc_posix_guarded_stack_system();
				break;
			}
#endif
#if __LIBIEEE1666_WINDOWS_GUARDED_STACK__
			if(strcmp(libieee1666_stack_system, "WINDOWS_GUARDED") == 0)
			{
				std::cerr << "using Windows guarded stack system as requested by user" << std::endl;
				stack_system = new sc_windows_guarded_stack_system();
				break;
			}
#endif
			if(strcmp(libieee1666_stack_system, "SIMPLE") == 0)
			{
				std::cerr << "using simple stack system as requested by user" << std::endl;
				stack_system = new sc_simple_stack_system();
				break;
			}
		}
#if __LIBIEEE1666_POSIX_GUARDED_STACK__
		stack_system = new sc_posix_guarded_stack_system();
#elif __LIBIEEE1666_WINDOWS_GUARDED_STACK__
		stack_system = new sc_windows_guarded_stack_system();
#else
		stack_system = new sc_simple_stack_system();
#endif
	}
	while(0);

	char *libieee1666_coroutine_system = getenv("LIBIEEE1666_COROUTINE_SYSTEM");
	
	do
	{
		if(libieee1666_coroutine_system)
		{
			if(strcmp(libieee1666_coroutine_system, "FCONTEXT") == 0)
			{
#if __LIBIEEE1666_FCONTEXT__
				std::cerr << "using Boost::Context (fcontext) coroutine system as requested by user" << std::endl;
				coroutine_system = new sc_fcontext_coroutine_system();
				break;
#else
				std::cerr << "Boost::Context (fcontext) coroutine system is not available" << std::endl;
#endif
			}
			else if(strcmp(libieee1666_coroutine_system, "UCONTEXT") == 0)
			{
#if __LIBIEEE1666_UCONTEXT__
				std::cerr << "using ucontext coroutine system as requested by user" << std::endl;
				coroutine_system = new sc_ucontext_coroutine_system();
				break;
#else
				std::cerr << "ucontext coroutine system is not available" << std::endl;
#endif
			}
			else if(strcmp(libieee1666_coroutine_system, "WINDOWS_FIBER") == 0)
			{
#if __LIBIEEE1666_WINDOWS_FIBER__
				std::cerr << "using Windows fiber coroutine system as requested by user" << std::endl;
				coroutine_system = new sc_windows_fiber_coroutine_system();
				break;
#else
				std::cerr << "using Windows fiber coroutine system as requested by user" << std::endl;
#endif
			}
			else if(strcmp(libieee1666_coroutine_system, "PTHREAD") == 0)
			{
#if __LIBIEEE1666_PTHREAD__
				std::cerr << "using POSIX thread coroutine system as requested by user" << std::endl;
				coroutine_system = new sc_pthread_coroutine_system();
				break;
#else
				std::cerr << "POSIX thread coroutine system is not available" << std::endl;
#endif
			}
		}
#if __LIBIEEE1666_FCONTEXT__
		coroutine_system = new sc_fcontext_coroutine_system();
		break;
#elif __LIBIEEE1666_UCONTEXT__
		coroutine_system = new sc_ucontext_coroutine_system();
		break;
#elif __LIBIEEE1666_WINDOWS_FIBER__
		coroutine_system = new sc_windows_fiber_coroutine_system();
		break;
#elif __LIBIEEE1666_PTHREAD__
		coroutine_system = new sc_pthread_coroutine_system();
		break;
#endif
		throw std::runtime_error("Internal error! no coroutine system available");
	}
	while(0);
	
	if(coroutine_system)
	{
		main_coroutine = coroutine_system->get_main_coroutine();
	}
	
	set_time_resolution(1.0, SC_PS, false);
}

sc_kernel::~sc_kernel()
{
	std::vector<sc_process_handle>::size_type num_process_handles = process_handle_table.size();
	std::vector<sc_process_handle>::size_type process_handle_num;
	
	for(process_handle_num = 0; process_handle_num < num_process_handles; process_handle_num++)
	{
		sc_process_handle process_handle = process_handle_table[process_handle_num];
			
		process_handle.kill();
	}
	
	process_handle_table.clear();
	
	std::vector<sc_kernel_event *>::size_type num_delta_events = delta_events.size();
	std::vector<sc_kernel_event *>::size_type delta_event_num;
	
	for(delta_event_num = 0; delta_event_num < num_delta_events; delta_event_num++)
	{
		sc_kernel_event *kernel_event = delta_events[delta_event_num];
		
		kernel_events_allocator.free(kernel_event);
	}

	delta_events.clear();
	
	std::multimap<sc_time, sc_timed_kernel_event *>::iterator it = schedule.begin();

	if(it != schedule.end())
	{
		do
		{
			sc_timed_kernel_event *kernel_event = (*it).second;
			kernel_events_allocator.free(kernel_event);
		}
		while(++it != schedule.end());
	}

	unsigned int i;
	
	unsigned int num_top_level_events = top_level_events.size();
	for(i = 0; i < num_top_level_events; i++)
	{
		sc_event *top_level_event = top_level_events[i];
		unregister_event(top_level_event);
		top_level_event->kernel = 0;
	}
	
	delete coroutine_system;
	delete stack_system;
}

sc_kernel *sc_kernel::get_kernel()
{
	if(!kernel)
	{
		kernel = new sc_kernel();
	}
	
	return kernel;
}

void sc_kernel::push_module_name(sc_module_name *module_name)
{
	module_name_stack.push(module_name);
}

void sc_kernel::pop_module_name()
{
	module_name_stack.pop();
}

sc_module_name *sc_kernel::get_top_of_module_name_stack() const
{
	sc_module_name *module_name = module_name_stack.empty() ? 0 : module_name_stack.top();
	
	if(!module_name) throw std::runtime_error("module name stack is empty");
	
	return module_name;
}

void sc_kernel::begin_module(sc_module *module)
{
	module_stack.push(module);
}

void sc_kernel::end_module()
{
	module_stack.pop();
}

sc_module *sc_kernel::get_current_module() const
{
	return module_stack.empty() ? 0 : module_stack.top();
}

sc_object *sc_kernel::get_current_object() const
{
	if(status >= SC_START_OF_SIMULATION)
	{
		return current_object;
	}
	else
	{
		return module_stack.empty() ? 0 : module_stack.top();
	}
}

sc_object *sc_kernel::get_current_writer() const
{
	return current_writer;
}

sc_process *sc_kernel::get_current_process() const
{
	return current_method_process ? (sc_process *) current_method_process : (sc_process *) current_thread_process;
}

sc_method_process *sc_kernel::get_current_method_process() const
{
	return current_method_process;
}

sc_thread_process *sc_kernel::get_current_thread_process() const
{
	return current_thread_process;
}

sc_process_handle sc_kernel::create_thread_process(const char *name, sc_process_owner *process_owner, sc_process_owner_method_ptr process_owner_method_ptr, const sc_spawn_options *spawn_options)
{
	bool clocked = false;
	sc_thread_process *thread_process = new sc_thread_process(name, process_owner, process_owner_method_ptr, clocked, spawn_options);

	sc_process_handle thread_process_handle(thread_process);
	process_handle_table.push_back(thread_process_handle);
	
	if(status >= SC_START_OF_SIMULATION)
	{
		if(!spawn_options || !spawn_options->get_flag_dont_initialize())
		{
			trigger(thread_process);
		}
	}
	
	return thread_process_handle;
}

sc_process_handle sc_kernel::create_cthread_process(const char *name, sc_process_owner *process_owner, sc_process_owner_method_ptr process_owner_method_ptr, const sc_spawn_options *spawn_options, sc_event_finder& edge_event_finder)
{
	sc_cthread_process *cthread_process = new sc_cthread_process(name, process_owner, process_owner_method_ptr, spawn_options);
	
	sc_process_handle cthread_process_handle(cthread_process);
	process_handle_table.push_back(cthread_process_handle);
	
	edge_event_finder.get_port().add_process_statically_sensitive_to_event_finder((sc_process *) cthread_process, edge_event_finder);
	return cthread_process_handle;
}

sc_process_handle sc_kernel::create_method_process(const char *name, sc_process_owner *process_owner, sc_process_owner_method_ptr process_owner_method_ptr, const sc_spawn_options *spawn_options)
{
	sc_method_process *method_process = new sc_method_process(name, process_owner, process_owner_method_ptr, spawn_options);
	
	sc_process_handle method_process_handle(method_process);
	process_handle_table.push_back(method_process_handle);
	
	if((status >= SC_START_OF_SIMULATION) && (!spawn_options || !spawn_options->get_flag_dont_initialize()))
	{
		trigger(method_process);
	}

	return method_process_handle;
}

sc_stack_system *sc_kernel::get_stack_system()
{
	return stack_system;
}

sc_coroutine_system *sc_kernel::get_coroutine_system()
{
	return coroutine_system;
}

void sc_kernel::report_before_end_of_elaboration()
{
	std::vector<sc_module *> module_table_before_end_of_elaboration = module_table;
	std::vector<sc_port_base *> port_table_before_end_of_elaboration = port_table;
	std::vector<sc_export_base *> export_table_before_end_of_elaboration = export_table;
	std::vector<sc_prim_channel *> prim_channel_table_before_end_of_elaboration = prim_channel_table;

	unsigned int i;
	unsigned int num_modules = module_table_before_end_of_elaboration.size();
	for(i = 0; i < num_modules; i++) module_table_before_end_of_elaboration[i]->before_end_of_elaboration();
	unsigned int num_ports = port_table_before_end_of_elaboration.size();
	for(i = 0; i < num_ports; i++) port_table_before_end_of_elaboration[i]->before_end_of_elaboration();
	unsigned int num_exports = port_table_before_end_of_elaboration.size();
	for(i = 0; i < num_exports; i++) port_table_before_end_of_elaboration[i]->before_end_of_elaboration();
	unsigned int num_prim_channels = prim_channel_table_before_end_of_elaboration.size();
	for(i = 0; i < num_prim_channels; i++) prim_channel_table_before_end_of_elaboration[i]->before_end_of_elaboration();
}

void sc_kernel::report_end_of_elaboration()
{
	unsigned int i;
	unsigned int num_modules = module_table.size();
	for(i = 0; i < num_modules; i++) module_table[i]->end_of_elaboration();
	unsigned int num_ports = port_table.size();
	for(i = 0; i < num_ports; i++) port_table[i]->end_of_elaboration();
	unsigned int num_exports = port_table.size();
	for(i = 0; i < num_exports; i++) port_table[i]->end_of_elaboration();
	unsigned int num_prim_channels = prim_channel_table.size();
	for(i = 0; i < num_prim_channels; i++) prim_channel_table[i]->end_of_elaboration();
}

void sc_kernel::report_start_of_simulation()
{
	unsigned int i;
	unsigned int num_modules = module_table.size();
	for(i = 0; i < num_modules; i++) module_table[i]->start_of_simulation();
	unsigned int num_ports = port_table.size();
	for(i = 0; i < num_ports; i++) port_table[i]->start_of_simulation();
	unsigned int num_exports = port_table.size();
	for(i = 0; i < num_exports; i++) port_table[i]->start_of_simulation();
	unsigned int num_prim_channels = prim_channel_table.size();
	for(i = 0; i < num_prim_channels; i++) prim_channel_table[i]->start_of_simulation();
	start_of_simulation_invoked = true;
}

void sc_kernel::report_end_of_simulation()
{
	unsigned int i;
	unsigned int num_modules = module_table.size();
	for(i = 0; i < num_modules; i++) module_table[i]->end_of_simulation();
	unsigned int num_ports = port_table.size();
	for(i = 0; i < num_ports; i++) port_table[i]->end_of_simulation();
	unsigned int num_exports = port_table.size();
	for(i = 0; i < num_exports; i++) port_table[i]->end_of_simulation();
	unsigned int num_prim_channels = prim_channel_table.size();
	for(i = 0; i < num_prim_channels; i++) prim_channel_table[i]->end_of_simulation();
	end_of_simulation_invoked = true;
}

void sc_kernel::initialize()
{
	unsigned int i;
	
	if(__LIBIEEE1666_UNLIKELY__(debug))
	{
		std::cerr << current_time_stamp << ":before end of elaboration" << std::endl;
	}
	status = SC_BEFORE_END_OF_ELABORATION;
	report_before_end_of_elaboration();
	
	unsigned int num_modules = module_table.size();
	for(i = 0; i < num_modules; i++)
	{
		sc_module *module =	module_table[i];
		
		module->before_end_of_elaboration();
	}
	
	unsigned int num_ports = port_table.size();
	for(i = 0; i < num_ports; i++)
	{
		port_table[i]->finalize_elaboration();
	}

	unsigned int num_thread_processes = thread_process_table.size();
	for(i = 0; i < num_thread_processes; i++)
	{
		sc_thread_process *thread_process = thread_process_table[i];
		thread_process->finalize_elaboration();
	}

	unsigned int num_method_processes = method_process_table.size();
	for(i = 0; i < num_method_processes; i++)
	{
		sc_method_process *method_process = method_process_table[i];
		method_process->finalize_elaboration();
	}

	if(__LIBIEEE1666_UNLIKELY__(debug))
	{
		std::cerr << current_time_stamp << ":end of elaboration" << std::endl;
	}
	status = SC_END_OF_ELABORATION;
	report_end_of_elaboration();
	
	for(i = 0; i < num_modules; i++)
	{
		sc_module *module =	module_table[i];
		
		module->end_of_elaboration();
	}

	// time resolution can no longer change
	time_resolution_fixed = true;

	if(__LIBIEEE1666_UNLIKELY__(debug))
	{
		std::cerr << current_time_stamp << ":start of simulation" << std::endl;
	}
	status = SC_START_OF_SIMULATION;
	report_start_of_simulation();

	// update phase
	do_update_phase();
	
	// make all method processes runnable except those that are 'dont_initialize'
	num_method_processes = method_process_table.size();
	for(i = 0; i < num_method_processes; i++)
	{
		sc_method_process *method_process = method_process_table[i];
		
		if(!method_process->dont_initialize())
		{
			runnable_method_processes.push_back(method_process);
		}
	}

	// make all thread processes runnable except those that are 'dont_initialize'
	num_thread_processes = thread_process_table.size();
	for(i = 0; i < num_thread_processes; i++)
	{
		sc_thread_process *thread_process = thread_process_table[i];

		if(!thread_process->dont_initialize())
		{
			runnable_thread_processes.push_back(thread_process);
		}
	}
	
	do_evaluation_phase();

	// delta notification phase
	do_delta_notification_phase();
	
	initialized = true;
}

void sc_kernel::do_evaluation_phase()
{
	// evaluation phase
	if(__LIBIEEE1666_UNLIKELY__(debug))
	{
		std::cerr << current_time_stamp << ":evaluation phase" << std::endl;
	}
	unsigned int eval_flag = 0; // 0 = no evaluation, 1 = at least one evaluation

	do
	{
		// wake up SC_METHOD processes
		if(runnable_method_processes.size())
		{
			bool stop_immediate = false;
			
			do
			{
				sc_method_process *method_process = runnable_method_processes.back();
				runnable_method_processes.pop_back();
				method_process->trigger_requested = false;
				
				set_current_method_process(method_process);
				eval_flag = 1;
				if(__LIBIEEE1666_UNLIKELY__(debug))
				{
					std::cerr << current_time_stamp << ":invoking " << method_process->name() << std::endl;
				}
				method_process->run();
				method_process->commit_next_trigger();
				stop_immediate = user_requested_stop && (stop_mode == SC_STOP_IMMEDIATE);
			}
			while(!stop_immediate && runnable_method_processes.size());
			set_current_method_process(0);
			if(stop_immediate) return;
		}

		// wake up SC_THREAD/SC_CTHREAD processes
		if(runnable_thread_processes.size())
		{
			eval_flag = 1;
			sc_thread_process *thread_process = runnable_thread_processes.back();
			runnable_thread_processes.pop_back();
			thread_process->trigger_requested = false;
			set_current_thread_process(thread_process);
			if(__LIBIEEE1666_UNLIKELY__(debug))
			{
				std::cerr << current_time_stamp << ":yield to " << thread_process->name() << std::endl;
			}
			main_coroutine->yield(thread_process->coroutine);
			set_current_thread_process(0);
		}
	}
	while(runnable_thread_processes.size() || runnable_method_processes.size());

	release_terminated_method_processes();
	release_terminated_thread_processes();

	if(user_requested_stop && (stop_mode == SC_STOP_IMMEDIATE)) return;
	
	delta_count += eval_flag;
}

void sc_kernel::do_update_phase()
{
	// update phase
	if(__LIBIEEE1666_UNLIKELY__(debug))
	{
		std::cerr << current_time_stamp << ":update phase" << std::endl;
	}
	
	std::vector<sc_prim_channel *>::size_type num_updatable_prim_channels = updatable_prim_channels.size();
	if(num_updatable_prim_channels)
	{
		std::vector<sc_prim_channel *>::size_type i;
		for(i = 0; i < num_updatable_prim_channels; i++)
		{
			sc_prim_channel *prim_channel = updatable_prim_channels[i];
			prim_channel->update_requested = false;
			
			if(__LIBIEEE1666_UNLIKELY__(debug))
			{
				std::cerr << current_time_stamp << ":" << prim_channel->name() << ".update()" << std::endl;
			}
			prim_channel->update();
		}
		
		updatable_prim_channels.clear();
	}
}

void sc_kernel::do_delta_notification_phase()
{
	// delta notification phase
	if(__LIBIEEE1666_UNLIKELY__(debug))
	{
		std::cerr << current_time_stamp << ":delta notifitication phase" << std::endl;
	}
	
	std::vector<sc_kernel_event *>::size_type num_delta_events = delta_events.size();
	if(num_delta_events)
	{
		std::vector<sc_kernel_event *>::size_type i;
		for(i = 0; i < num_delta_events; i++)
		{
			sc_kernel_event *kernel_event = delta_events[i];
			
			sc_event *e = kernel_event->get_event();
			
			if(e)
			{
				if(__LIBIEEE1666_UNLIKELY__(debug))
				{
					std::cerr << current_time_stamp << ":" << e->name() << ".trigger()" << std::endl;
				}
				e->trigger();
			}
			
			kernel_events_allocator.free(kernel_event);
		}
		
		delta_events.clear();
	}
}

void sc_kernel::do_delta_steps(bool once)
{
	do
	{
		// evaluation phase
		do_evaluation_phase();

		if(user_requested_stop && (stop_mode == SC_STOP_IMMEDIATE)) return;
		
		// update phase
		do_update_phase();
		
		if(user_requested_stop) return;
		
		// delta notification phase
		do_delta_notification_phase();
		
		if(user_requested_pause) break;
	}
	while(!once && (runnable_thread_processes.size() || runnable_method_processes.size()));
}

bool sc_kernel::do_timed_notification_phase()
{
	// timed notification phase
	if(__LIBIEEE1666_UNLIKELY__(debug))
	{
		std::cerr << current_time_stamp << ":timed notification phase" << std::endl;
	}
	std::multimap<sc_time, sc_timed_kernel_event *>::iterator it = schedule.begin();

	if(it == schedule.end()) return false;
	
	current_time_stamp = (*it).first;
	
	do
	{
		sc_timed_kernel_event *kernel_event = (*it).second;
		
		sc_event *e = kernel_event->get_event();
		
		if(e)
		{
			if(__LIBIEEE1666_UNLIKELY__(debug))
			{
				std::cerr << current_time_stamp << ":" << e->name() << ".trigger()" << std::endl;
			}
			e->trigger();
		}
		
		kernel_events_allocator.free(kernel_event);
		
		schedule.erase(it);
		it = schedule.begin();			
	}
	while((it != schedule.end()) && ((*it).first == current_time_stamp));
	
	return schedule.size() != 0;
}

void sc_kernel::simulate(const sc_time& duration)
{
	status = SC_RUNNING;
	
	if(duration == SC_ZERO_TIME)
	{
		do_delta_steps(true);
	}
	else
	{
		sc_time until_time(current_time_stamp);
		until_time += duration;

		bool pending_timed_notifications_exist = false;
		
		do
		{
			do_delta_steps(false);
			if(user_requested_stop || user_requested_pause) break;
			pending_timed_notifications_exist = do_timed_notification_phase();
			if(user_requested_stop) break;
		}
		while((current_time_stamp < until_time) && (pending_timed_notifications_exist || runnable_thread_processes.size() || runnable_method_processes.size()));
	}
	
	if(user_requested_stop)
	{
		status = SC_STOPPED;
	}
	else
	{
		status = SC_PAUSED;
	}
}

void sc_kernel::start(const sc_time& duration, sc_starvation_policy p)
{
	if(status == SC_END_OF_SIMULATION) return;
	
	sc_time end_time;
	
	if(p == SC_RUN_TO_TIME)
	{
		end_time = current_time_stamp;
		end_time += duration;
	}

	if(!initialized) initialize();
	simulate(duration);

	if(p == SC_RUN_TO_TIME)
	{
		current_time_stamp = end_time;
	}
}

void sc_kernel::register_object(sc_object *object)
{
	if(object_registry.find(object->name()) != object_registry.end())
	{
		throw std::runtime_error("object or an object with same name is already registered");
	}
	
	object_registry.insert(std::pair<std::string, sc_object *>(object->name(), object));

	if(!object->get_parent_object())
	{
		top_level_objects.push_back(object);
	}
}

void sc_kernel::register_event(sc_event *event)
{
	std::string event_name(event->name());
	if(event_registry.find(event_name) != event_registry.end())
	{
		throw std::runtime_error("event or an event with same name is already registered");
	}

	event_registry.insert(std::pair<std::string, sc_event *>(event_name, event));

	if(!event->get_parent_object())
	{
		top_level_events.push_back(event);
	}
}

void sc_kernel::register_module(sc_module *module)
{
	if(status > SC_END_OF_ELABORATION) throw std::runtime_error("sc_module instantiation is not allowed after the end of elaboration");
	module_table.push_back(module);
}

void sc_kernel::register_port(sc_port_base *port)
{
	if(status > SC_END_OF_ELABORATION) throw std::runtime_error("sc_port instantiation is not allowed after the end of elaboration");
	port_table.push_back(port);
	sc_module *cur_module = get_current_module();
	if(cur_module)
	{
		cur_module->add_port(port);
	}
}

void sc_kernel::register_export(sc_export_base *exp)
{
	if(status > SC_END_OF_ELABORATION) throw std::runtime_error("sc_export instantiation is not allowed after the end of elaboration");
	export_table.push_back(exp);
}

void sc_kernel::register_prim_channel(sc_prim_channel *prim_channel)
{
	if(status > SC_END_OF_ELABORATION) throw std::runtime_error("sc_prim_channel instantiation is not allowed after the end of elaboration");
	prim_channel_table.push_back(prim_channel);
}

void sc_kernel::register_thread_process(sc_thread_process *thread_process)
{
	thread_process_table.push_back(thread_process);
}

void sc_kernel::register_method_process(sc_method_process *method_process)
{
	method_process_table.push_back(method_process);
}

void sc_kernel::unregister_object(sc_object *object)
{
	std::map<std::string, sc_object *>::iterator it = object_registry.find(object->name());
	
	if(it != object_registry.end())
	{
		object_registry.erase(it);
	}
	else
	{
		throw std::runtime_error("Internal error! can't unregister object because it is not registered");
	}
	
	if(!object->get_parent_object())
	{
		unsigned int num_top_level_objects = top_level_objects.size();
		unsigned int i;
		for(i = 0; i < num_top_level_objects; i++)
		{
			if(top_level_objects[i] == object)
			{
				top_level_objects[i] = top_level_objects[num_top_level_objects - 1];
				top_level_objects.pop_back();
				break;
			}
		}
	}
	
	object->kernel = 0;
}

void sc_kernel::unregister_event(sc_event *event)
{
	std::string event_name(event->name());
	std::map<std::string, sc_event *>::iterator it = event_registry.find(event_name);
	
	if(it != event_registry.end())
	{
		event_registry.erase(it);
	}
	else
	{
		throw std::runtime_error("Internal error! can't unregister event because it is not registered");
	}
	
	if(!event->get_parent_object())
	{
		unsigned int num_top_level_events = top_level_events.size();
		unsigned int i;
		for(i = 0; i < num_top_level_events; i++)
		{
			if(top_level_events[i] == event)
			{
				top_level_events[i] = top_level_events[num_top_level_events - 1];
				top_level_events.pop_back();
				break;
			}
		}
	}
}

void sc_kernel::unregister_module(sc_module *module)
{
	unsigned int num_modules = module_table.size();
	unsigned int i;
	
	for(i = 0; i < num_modules; i++)
	{
		if(module_table[i] == module)
		{
			module_table[i] = module_table[num_modules - 1];
			module_table.pop_back();
			break;
		}
	}
}

void sc_kernel::unregister_port(sc_port_base *port)
{
	unsigned int num_ports = port_table.size();
	unsigned int i;
	
	for(i = 0; i < num_ports; i++)
	{
		if(port_table[i] == port)
		{
			port_table[i] = port_table[num_ports - 1];
			port_table.pop_back();
			break;
		}
	}
}

void sc_kernel::unregister_export(sc_export_base *exp)
{
	unsigned int num_exports = export_table.size();
	unsigned int i;
	
	for(i = 0; i < num_exports; i++)
	{
		if(export_table[i] == exp)
		{
			export_table[i] = export_table[num_exports - 1];
			export_table.pop_back();
			break;
		}
	}
}

void sc_kernel::unregister_prim_channel(sc_prim_channel *prim_channel)
{
	unsigned int num_prim_channels = prim_channel_table.size();
	unsigned int i;
	
	for(i = 0; i < num_prim_channels; i++)
	{
		if(prim_channel_table[i] == prim_channel)
		{
			prim_channel_table[i] = prim_channel_table[num_prim_channels - 1];
			prim_channel_table.pop_back();
			break;
		}
	}
}

void sc_kernel::unregister_thread_process(sc_thread_process *thread_process)
{
	unsigned int num_thread_processes = thread_process_table.size();
	unsigned int i;
	
	for(i = 0; i < num_thread_processes; i++)
	{
		if(thread_process_table[i] == thread_process)
		{
			thread_process_table[i] = thread_process_table[num_thread_processes - 1];
			thread_process_table.pop_back();
			break;
		}
	}
}

void sc_kernel::unregister_method_process(sc_method_process *method_process)
{
	unsigned int num_method_processes = method_process_table.size();
	unsigned int i;
	
	for(i = 0; i < num_method_processes; i++)
	{
		if(method_process_table[i] == method_process)
		{
			method_process_table[i] = method_process_table[num_method_processes - 1];
			method_process_table.pop_back();
			break;
		}
	}
}

void sc_kernel::terminate_thread_process(sc_thread_process *thread_process)
{
	terminated_thread_processes.push_back(thread_process);
}

void sc_kernel::terminate_method_process(sc_method_process *method_process)
{
	terminated_method_processes.push_back(method_process);
}

void sc_kernel::disconnect_thread_process(sc_thread_process *thread_process)
{
	std::map<std::string, sc_event *>::iterator event_it;
	
	for(event_it = event_registry.begin(); event_it != event_registry.end(); event_it++)
	{
		sc_event *event = (*event_it).second;
		
		event->remove_dynamically_sensitive_thread_process(thread_process);
		event->remove_statically_sensitive_thread_process(thread_process);
	}

	untrigger(thread_process);
}

void sc_kernel::disconnect_method_process(sc_method_process *method_process)
{
	std::map<std::string, sc_event *>::iterator event_it;
	
	for(event_it = event_registry.begin(); event_it != event_registry.end(); event_it++)
	{
		sc_event *event = (*event_it).second;
		
		event->remove_dynamically_sensitive_method_process(method_process);
		event->remove_statically_sensitive_method_process(method_process);
	}

	untrigger(method_process);
}

void sc_kernel::release_terminated_thread_processes()
{
	std::vector<sc_thread_process *>::size_type num_terminated_thread_processes = terminated_thread_processes.size();
	std::vector<sc_thread_process *>::size_type terminated_thread_process_num;
	for(terminated_thread_process_num = 0; terminated_thread_process_num < num_terminated_thread_processes; terminated_thread_process_num++)
	{
		sc_thread_process *thread_process = terminated_thread_processes[terminated_thread_process_num];

		disconnect_thread_process(thread_process);
		
		sc_process_handle process_handle(thread_process);

		std::vector<sc_thread_process *>::size_type num_process_handles = process_handle_table.size();
		std::vector<sc_thread_process *>::size_type process_handle_num;
		for(process_handle_num = 0; process_handle_num < num_process_handles; process_handle_num++)
		{
			if(process_handle == process_handle_table[process_handle_num])
			{
				process_handle_table[process_handle_num] = process_handle_table[num_process_handles - 1];
				process_handle_table.pop_back();
				break;
			}
		}
	}
	
	terminated_thread_processes.clear();
}

void sc_kernel::release_terminated_method_processes()
{
	std::vector<sc_method_process *>::size_type num_terminated_method_processes = terminated_method_processes.size();
	std::vector<sc_method_process *>::size_type terminated_method_process_num;
	for(terminated_method_process_num = 0; terminated_method_process_num < num_terminated_method_processes; terminated_method_process_num++)
	{
		sc_method_process *method_process = terminated_method_processes[terminated_method_process_num];

		disconnect_method_process(method_process);
		
		sc_process_handle process_handle(method_process);

		std::vector<sc_method_process *>::size_type num_process_handles = process_handle_table.size();
		std::vector<sc_method_process *>::size_type process_handle_num;
		for(process_handle_num = 0; process_handle_num < num_process_handles; process_handle_num++)
		{
			if(process_handle == process_handle_table[process_handle_num])
			{
				process_handle_table[process_handle_num] = process_handle_table[num_process_handles - 1];
				process_handle_table.pop_back();
				break;
			}
		}
	}
	
	terminated_method_processes.clear();
}

void sc_kernel::reset_thread_process(sc_thread_process *target_thread_process)
{
	if(current_thread_process && (current_thread_process != target_thread_process))
	{
		// reset requested by another thread process

		// before yielding to target thread being reset, reschedule the calling process
		trigger(current_thread_process);
		
		// switch to thread being reset and let thread reset (throw)
		set_current_thread_process(target_thread_process);
		current_thread_process->coroutine->yield(target_thread_process->coroutine);
	}
	else if(current_method_process)
	{
		// reset requested by another method process
		
		// switch to target thread being reset and let target thread reset (throw)
		set_current_method_process(0);
		set_current_thread_process(target_thread_process);
		main_coroutine->yield(target_thread_process->coroutine);
		set_current_method_process(current_method_process);

		// check if method process got an exception in the meantime
		current_method_process->check_exceptions();
	}
	else
	{
		throw std::runtime_error("Internal error!");
	}
}

void sc_kernel::reset_method_process(sc_method_process *target_method_process)
{
	if(current_thread_process)
	{
		// reset requested by a thread process

		// immediately run the target method process being reset
		sc_thread_process *calling_thread_process = current_thread_process;
		set_current_thread_process(0);
		set_current_method_process(target_method_process);
		target_method_process->run();
		set_current_thread_process(calling_thread_process);
	}
	else if(current_method_process && (current_method_process != target_method_process))
	{
		// reset by another method process
	}
	else
	{
		throw std::runtime_error("Internal error!");
	}
}

void sc_kernel::kill_thread_process(sc_thread_process *target_thread_process)
{
	if(current_thread_process)
	{
		// kill requested by another thread process

		// before yielding to target thread being reset, reschedule the calling process
		trigger(current_thread_process);
		
		// switch to thread being reset and let thread reset (throw)
		set_current_thread_process(target_thread_process);
		current_thread_process->coroutine->yield(target_thread_process->coroutine);
	}
	else
	{
		// kill requested by another method process or kernel
		main_coroutine->yield(target_thread_process->coroutine);  // switch to thread being killed and let thread die (throw)
	}
}

void sc_kernel::set_time_resolution(double v, sc_time_unit tu, bool user)
{
	if(time_resolution_fixed)
	{
		throw std::runtime_error("time resolution already fixed");
	}
	
	if(user)
	{
		if(time_resolution_fixed_by_user)
		{
			std::runtime_error("time resolution already set");
		}
		time_resolution_fixed_by_user = true;
	}
	
	double log10_v = log10(v);
	double log10_v_int_part;
    if(modf(log10_v, &log10_v_int_part) != 0.0 )
	{
		throw std::runtime_error("time resolution shall be a power of ten");
	}

	time_resolution_scale_factors_table_base_index = static_cast<int>(log10_v) + (3 * tu);

	double time_resolution_sec = (v * pow(10.0, -3 * (SC_SEC - tu)));
	if(time_resolution_sec < 1e-15)
	{
		throw std::runtime_error("time resolution is less than 1 fs");
	}

	int i;
	double scale_factor;
	for(i = time_resolution_scale_factors_table_base_index - 1, scale_factor = 0.1; i >= 0; i--, scale_factor /= 10)
	{
		time_resolution_scale_factors_table[i] = scale_factor;
	}
	for(i = time_resolution_scale_factors_table_base_index, scale_factor = 1; i < TIME_RESOLUTION_SCALE_FACTORS_TABLE_SIZE; i++, scale_factor *= 10)
	{
		time_resolution_scale_factors_table[i] = scale_factor;
	}
	
	if(user) time_resolution_fixed = true;
	
	max_time = sc_time(std::numeric_limits<sc_dt::uint64>::max());
}

sc_dt::uint64 sc_kernel::get_time_discrete_value(double d, sc_time_unit tu) const
{
	time_resolution_fixed = true;
	int time_resolution_scale_factors_table_index = 3 * tu;
	return static_cast<sc_dt::uint64>((d * time_resolution_scale_factors_table[time_resolution_scale_factors_table_index]) + 0.5);
}

void sc_kernel::print_time(std::ostream& os, const sc_time& t) const
{
	sc_dt::uint64 discrete_value = t.value();

	if(!discrete_value)
	{
		os << "0 s";
		return;
	}
	
	sc_time_unit tu = (sc_time_unit)(time_resolution_scale_factors_table_base_index / 3);
	while(((discrete_value % 1000) == 0) && (tu != SC_SEC))
	{
		discrete_value /= 1000;
		tu = (sc_time_unit)(tu + 1);
	}

	int num_zeros = time_resolution_scale_factors_table_base_index % 3;
	
	if(tu != SC_SEC)
	{
		while((discrete_value % 10) == 0)
		{
			discrete_value /= 10;
			num_zeros++;
		}
		
		while((num_zeros >= 3) && (tu != SC_SEC))
		{
			tu = (sc_time_unit)(tu + 1);
			num_zeros -= 3;
		}

		while(num_zeros)
		{
			discrete_value *= 10;
			num_zeros--;
		}
	}
	
	os << discrete_value;
	while(num_zeros)
	{
		os << '0';
		num_zeros--;
	}
	os << " " << time_unit_strings[tu];
}

double sc_kernel::time_discrete_value_to_seconds(sc_dt::uint64 discrete_value) const
{
	if(discrete_value == 0) return 0.0;
	time_resolution_fixed = true;
	
	double scale_factor = time_resolution_scale_factors_table[3 * SC_SEC];
	return (double) discrete_value / scale_factor;
}

sc_time sc_kernel::get_time_resolution() const
{
	return sc_time(1);
}

const sc_time& sc_kernel::get_max_time() const
{
	return max_time;
}

void sc_kernel::untrigger(sc_thread_process *thread_process)
{
	std::vector<sc_thread_process *>::size_type num_runnable_thread_processes = runnable_thread_processes.size();
	std::vector<sc_thread_process *>::size_type runnable_thread_process_num;
	for(runnable_thread_process_num = 0; runnable_thread_process_num < num_runnable_thread_processes; runnable_thread_process_num++)
	{
		if(runnable_thread_processes[runnable_thread_process_num] == thread_process)
		{
			runnable_thread_processes[runnable_thread_process_num] = runnable_thread_processes[runnable_thread_processes.size() - 1];
			runnable_thread_processes.pop_back();
			break;
		}
	}
}

void sc_kernel::untrigger(sc_method_process *method_process)
{
	std::vector<sc_method_process *>::size_type num_runnable_method_processes = runnable_method_processes.size();
	std::vector<sc_method_process *>::size_type runnable_method_process_num;
	for(runnable_method_process_num = 0; runnable_method_process_num < num_runnable_method_processes; runnable_method_process_num++)
	{
		if(runnable_method_processes[runnable_method_process_num] == method_process)
		{
			runnable_method_processes[runnable_method_process_num] = runnable_method_processes[runnable_method_processes.size() - 1];
			runnable_method_processes.pop_back();
			break;
		}
	}
}

sc_process_handle sc_kernel::get_current_process_handle() const
{
	if(current_method_process) return sc_process_handle((sc_process *) current_method_process);
	if(current_thread_process) return sc_process_handle((sc_process *) current_thread_process);
	return sc_process_handle();
}

bool sc_kernel::is_unwinding() const
{
	if(current_method_process) return current_method_process->is_unwinding();
	if(current_thread_process) return current_thread_process->is_unwinding();
	return false;
}

void sc_kernel::set_stop_mode(sc_stop_mode mode)
{
	stop_mode = mode;
}

sc_stop_mode sc_kernel::get_stop_mode() const
{
	return stop_mode;
}

void sc_kernel::stop()
{
	user_requested_stop = true;
	
	if(status == SC_PAUSED)
	{
		if(__LIBIEEE1666_UNLIKELY__(debug))
		{
			std::cerr << current_time_stamp << ":end of simulation" << std::endl;
		}
		status = SC_END_OF_SIMULATION;
		report_end_of_simulation();
	}
}

void sc_kernel::pause()
{
	if(user_requested_stop) return;
	
	user_requested_pause = true;
}

sc_status sc_kernel::get_status() const
{
	return status;
}

bool sc_kernel::is_end_of_simulation_invoked() const
{
	return end_of_simulation_invoked;
}

bool sc_kernel::is_start_of_simulation_invoked() const
{
	return start_of_simulation_invoked;
}

bool sc_kernel::is_running() const
{
	return (status > SC_START_OF_SIMULATION) && (status < SC_END_OF_SIMULATION);
}

sc_dt::uint64 sc_kernel::get_delta_count() const
{
	return delta_count;
}

bool sc_kernel::pending_activity_at_current_time() const
{
	std::multimap<sc_time, sc_timed_kernel_event *>::const_iterator it = schedule.begin();

	if(it != schedule.end())
	{
		const sc_time& time_of_pending_activity = (*it).first;
		return time_of_pending_activity == current_time_stamp;
	}
	
	return false;
}

bool sc_kernel::pending_activity_at_future_time() const
{
	std::multimap<sc_time, sc_timed_kernel_event *>::const_iterator it = schedule.begin();

	if(it != schedule.end())
	{
		const sc_time& time_of_pending_activity = (*it).first;
		return time_of_pending_activity > current_time_stamp;
	}
	
	return false;
}

bool sc_kernel::pending_activity() const
{
	return schedule.size() != 0;
}

sc_time sc_kernel::time_to_pending_activity() const
{
	std::multimap<sc_time, sc_timed_kernel_event *>::const_iterator it = schedule.begin();

	if(it != schedule.end())
	{
		const sc_time& time_of_pending_activity = (*it).first;
		return time_of_pending_activity - current_time_stamp;
	}
	
	return max_time - current_time_stamp;
}

const std::vector<sc_object*>& sc_kernel::get_top_level_objects() const
{
	return top_level_objects;
}

const std::vector<sc_event*>& sc_kernel::get_top_level_events() const
{
	return top_level_events;
}

sc_object *sc_kernel::find_object(const char* name) const
{
	std::map<std::string, sc_object *>::const_iterator it = object_registry.find(name);
	
	return (it != object_registry.end()) ? (*it).second : 0;
// 	unsigned int n_top_level_objects = top_level_objects.size();
// 	unsigned int i;
// 	for(i = 0; i < n_top_level_objects; i++)
// 	{
// 		sc_object *object = top_level_objects[i];
// 		if(strcmp(object->name(), name) == 0) return object;
// 		
// 		sc_object *found_child_object = object->find_child_object(name);
// 		if(found_child_object) return found_child_object;
// 	}
// 	return 0;
}

sc_event *sc_kernel::find_event(const char *name) const
{
	std::map<std::string, sc_event *>::const_iterator it = event_registry.find(name);
	
	return (it != event_registry.end()) ? (*it).second : 0;
}

bool sc_kernel::hierarchical_name_exists(const char *name) const
{
	sc_object *found_object = find_object(name);
	
	if(found_object) return true;
	
	sc_event *found_event = find_event(name);
	
	if(found_event) return true;
	
	return false;
}

const char* sc_kernel::gen_unique_name( const char* seed ) const
{
	static std::string unique_name;
	
	std::string s;
	
	std::map<std::string, unsigned int>::iterator it = unique_name_map.find(seed);
	
	unsigned int id;
	
	if(it != unique_name_map.end())
	{
		id = ++(*it).second;
	}
	else
	{
		id = 0;
		unique_name_map.insert(std::pair<std::string, unsigned int>(seed, id));
	}
	
	std::stringstream sstr;
	sstr << seed << '_' << id;
	unique_name = sstr.str();
	return unique_name.c_str();
}

void sc_kernel::dump_hierarchy(std::ostream& os) const
{
	unsigned int i;
	unsigned int num_top_level_events = top_level_events.size();
	for(i = 0; i < num_top_level_events; i++)
	{
		sc_event *top_level_event = top_level_events[i];
		os << "- event " << top_level_event->name() << std::endl;
	}
	unsigned int num_top_level_objects = top_level_objects.size();
	for(i = 0; i < num_top_level_objects; i++)
	{
		sc_object *object = top_level_objects[i];
		object->dump_hierarchy(os);
	}
}

int sc_elab_and_sim(int _argc, char* _argv[])
{
	argc = _argc;
	argv = _argv;

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	sc_main_type sc_main = (sc_main_type) GetProcAddress(GetModuleHandle(NULL), "sc_main");
	
	if(!sc_main)
	{
		throw std::runtime_error("sc_main not available");
	}
#endif	
	int status = sc_main(argc, argv);
	
	if(sc_kernel::kernel) delete sc_kernel::kernel;
	return status;
}

int sc_argc()
{
	return argc;
}

const char* const* sc_argv()
{
	return argv;
}

void sc_start()
{
    sc_start(sc_max_time() - sc_time_stamp(), SC_EXIT_ON_STARVATION);
}

void sc_start(const sc_time& duration, sc_starvation_policy p)
{
	sc_kernel::get_kernel()->start(duration, p);
}

void sc_start(double _duration,sc_time_unit _tu, sc_starvation_policy _p)
{
	sc_time duration(_duration, _tu);
	
	sc_start(duration, _p);
}

void sc_pause()
{
	sc_kernel::get_kernel()->pause();
}

void sc_set_stop_mode(sc_stop_mode mode)
{
	sc_kernel::get_kernel()->set_stop_mode(mode);
}

sc_stop_mode sc_get_stop_mode()
{
	return sc_kernel::get_kernel()->get_stop_mode();
}

void sc_stop()
{
	sc_kernel::get_kernel()->stop();
}

sc_process_handle sc_get_current_process_handle()
{
	return sc_kernel::get_kernel()->get_current_process_handle();
}

bool sc_is_unwinding()
{
	return sc_kernel::get_kernel()->is_unwinding();
}

const sc_dt::uint64 sc_delta_count()
{
	return sc_kernel::get_kernel()->get_delta_count();
}

bool sc_is_running()
{
	return sc_kernel::get_kernel()->is_running();
}

bool sc_pending_activity_at_current_time()
{
	return sc_kernel::get_kernel()->pending_activity_at_current_time();
}

bool sc_pending_activity_at_future_time()
{
	return sc_kernel::get_kernel()->pending_activity_at_future_time();
}

bool sc_pending_activity()
{
	return sc_kernel::get_kernel()->pending_activity();
}

sc_time sc_time_to_pending_activity()
{
	return sc_kernel::get_kernel()->time_to_pending_activity();
}

sc_status sc_get_status()
{
	return sc_kernel::get_kernel()->get_status();
}

bool sc_start_of_simulation_invoked()
{
	return sc_kernel::get_kernel()->is_start_of_simulation_invoked();
}

bool sc_end_of_simulation_invoked()
{
	return sc_kernel::get_kernel()->is_end_of_simulation_invoked();
}

const std::vector<sc_object*>& sc_get_top_level_objects()
{
	return sc_kernel::get_kernel()->get_top_level_objects();
}

sc_object* sc_find_object( const char* name)
{
	return sc_kernel::get_kernel()->find_object(name);
}

bool sc_hierarchical_name_exists(const char *name)
{
	return sc_kernel::get_kernel()->hierarchical_name_exists(name);
}

const char* sc_gen_unique_name( const char* seed )
{
	return sc_kernel::get_kernel()->gen_unique_name(seed);
}

const std::vector<sc_event*>& sc_get_top_level_events()
{
	return sc_kernel::get_kernel()->get_top_level_events();
}

sc_event *sc_find_event(const char* name)
{
	return sc_kernel::get_kernel()->find_event(name);
}

void next_trigger()
{
	sc_kernel::get_kernel()->next_trigger();
}

void next_trigger(const sc_event& e)
{
	sc_kernel::get_kernel()->next_trigger(e);
}

void next_trigger(const sc_event_or_list& el)
{
	sc_kernel::get_kernel()->next_trigger(el);
}

void next_trigger(const sc_event_and_list& el)
{
	sc_kernel::get_kernel()->next_trigger(el);
}

void next_trigger(const sc_time& t)
{
	sc_kernel::get_kernel()->next_trigger(t);
}

void next_trigger(double d, sc_time_unit tu)
{
	sc_time t = sc_time(d, tu);
	sc_kernel::get_kernel()->next_trigger(t);
}

void next_trigger(const sc_time& t, const sc_event& e)
{
	sc_kernel::get_kernel()->next_trigger(t, e);
}

void next_trigger(double d, sc_time_unit tu, const sc_event& e)
{
	sc_time t = sc_time(d, tu);
	sc_kernel::get_kernel()->next_trigger(t, e);
}

void next_trigger(const sc_time& t, const sc_event_or_list& el)
{
	sc_kernel::get_kernel()->next_trigger(t, el);
}

void next_trigger(double d, sc_time_unit tu, const sc_event_or_list& el)
{
	sc_time t = sc_time(d, tu);
	sc_kernel::get_kernel()->next_trigger(t, el);
}

void next_trigger(const sc_time& t, const sc_event_and_list& el)
{
	sc_kernel::get_kernel()->next_trigger(t, el);
}

void next_trigger(double d, sc_time_unit tu, const sc_event_and_list& el)
{
	sc_time t = sc_time(d, tu);
	sc_kernel::get_kernel()->next_trigger(t, el);
}

void wait()
{
	sc_kernel::get_kernel()->wait();
}

void wait( int )
{
}

void wait(const sc_event& e)
{
	sc_kernel::get_kernel()->wait(e);
}

void wait(const sc_event_or_list & el)
{
	sc_kernel::get_kernel()->wait(el);
}

void wait(const sc_event_and_list& el)
{
	sc_kernel::get_kernel()->wait(el);
}

void wait(const sc_time& t)
{
	sc_kernel::get_kernel()->wait(t);
}

void wait(double d, sc_time_unit tu)
{
	sc_time t = sc_time(d, tu);
	sc_kernel::get_kernel()->wait(t);
}

void wait(const sc_time& t, const sc_event& e)
{
	sc_kernel::get_kernel()->wait(t, e);
}

void wait(double d, sc_time_unit tu, const sc_event& e)
{
	sc_time t = sc_time(d, tu);
	sc_kernel::get_kernel()->wait(t, e);
}

void wait(const sc_time& t, const sc_event_or_list& el)
{
	sc_kernel::get_kernel()->wait(t, el);
}

void wait(double d, sc_time_unit tu, const sc_event_or_list& el)
{
	sc_time t = sc_time(d, tu);
	sc_kernel::get_kernel()->wait(t, el);
}

void wait(const sc_time& t, const sc_event_and_list& el)
{
	sc_kernel::get_kernel()->wait(t, el);
}

void wait(double d, sc_time_unit tu, const sc_event_and_list& el)
{
	sc_time t = sc_time(d, tu);
	sc_kernel::get_kernel()->wait(t, el);
}

} // end of namespace sc_core

int main(int argc, char *argv[], char *envp[])
{
	return sc_core::sc_elab_and_sim(argc, argv);
}
