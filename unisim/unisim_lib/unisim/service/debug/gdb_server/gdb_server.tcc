/*
 *  Copyright (c) 2007,
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
 *          Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_SERVICE_DEBUG_GDB_SERVER_GDB_SERVER_TCC__
#define __UNISIM_SERVICE_DEBUG_GDB_SERVER_GDB_SERVER_TCC__

#include <iostream>
#include <sstream>
#include <list>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <stdexcept>

#include <unisim/util/likely/likely.hh>

namespace unisim {
namespace service {
namespace debug {
namespace gdb_server {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class ADDRESS>
const bool GDBServer<ADDRESS>::ALWAYS_ACCEPT_MULTIPROCESS_NEW_THREAD_ID_SYNTAX;

template <class ADDRESS>
GDBServer<ADDRESS>::GDBServer(const char *_name, Object *_parent)
	: Object(_name, _parent,
		"this service implements the GDB server remote serial protocol over TCP/IP. "
		"Standards GDB clients (e.g. gdb, eclipse, ddd) can connect to the simulator to debug the target application "
		"that runs within the simulator.")
	, GDBServerBase(_name, _parent)
	, unisim::kernel::Service<unisim::service::interfaces::DebugYielding>(_name, _parent)
	, unisim::kernel::Service<unisim::service::interfaces::DebugEventListener<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugSelecting>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugYieldingRequest>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Disassembly<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Registers>(_name, _parent)
	, debug_yielding_export("debug-yielding-export", this)
	, debug_event_listener_export("debug-event-listener-export", this)
	, debug_yielding_request_import("debug-yielding-request-import", this)
	, debug_selecting_import("debug-selecting-import", this)
	, debug_event_trigger_import("debug-event-trigger-import", this)
	, memory_import("memory-import", this)
	, registers_import("cpu-registers-import", this)
	, disasm_import("disasm-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, architecture_description_filename()
	, gdb_registers()
	, gdb_pc(0)
	, num_processors(0)
	, g_prc_num(0)
	, c_prc_num(0)
	, session_num(0)
	, session_terminated(false)
	, detached(false)
	, listening_fetch()
	, trap(false)
	, prc_trap()
	, synched(false)
	, extended_mode(false)
	, gdb_client_feature_multiprocess(false)
	, gdb_client_feature_xmlregisters(false)
	, gdb_client_feature_qrelocinsn(false)
	, gdb_client_feature_swbreak(false)
	, gdb_client_feature_hwbreak(false)
	, gdb_client_feature_fork_events(false)
	, gdb_client_feature_vfork_events(false)
	, gdb_client_feature_exec_events(false)
	, gdb_client_feature_vcont(false)
	, gdb_client_feature_t32extensions(false)
	, gdb_client_feature_qxfer_features_read(false)
	, gdb_client_feature_qxfer_threads_read(false)
	, no_ack_mode(false)
	, arch_specific_breakpoint_kinds()
	, fetch_insn_events()
	, stop_events()
	, memory_atom_size(1)
	, monitor_internals()
	, wait_connection_mode(GDB_WAIT_CONNECTION_STARTUP_ONLY)
	, enable_multiprocess_extension(false)
	, enable_interrupt(true)
	, interrupt_poll_period_ms(100)
	, mode(GDB_MODE_SINGLE_THREAD)
	, param_memory_atom_size("memory-atom-size", this, memory_atom_size, "size of the smallest addressable element in memory")
	, param_architecture_description_filename("architecture-description-filename", this, architecture_description_filename, "filename of a XML description of the connected processor")
	, param_monitor_internals("monitor-internals", this, monitor_internals, "List of internal simulator variables to monitor in GDB Front-End")
	, param_wait_connection_mode("wait-connection-mode", this, wait_connection_mode, "Whether to wait for connection of GDB client (never, [startup-only], always)")
	, param_enable_multiprocess_extension("enable-multiprocess-extension", this, enable_multiprocess_extension, "Whether to enable GDB multiprocess extension")
	, param_enable_interrupt("enable-interrupt", this, enable_interrupt, "enable/disable GDB interrupt (Ctrl-C)")
	, param_interrupt_poll_period_ms("interrupt-poll-period-ms", this, interrupt_poll_period_ms, "GDB interrupt (Ctrl-C) polling period (in milliseconds)")
	, param_mode("mode", this, mode, "debugging mode (single-thread or multi-thread)")
	, thrd_process_cmd()
	, thrd_process_int()
	, thrd_process_cmd_create_mutex()
	, thrd_process_cmd_create_cond()
	, thrd_process_cmd_alive(false)
	, stop_process_cmd_thrd(false)
	, thrd_process_int_create_mutex()
	, thrd_process_int_create_cond()
	, thrd_process_int_alive(false)
	, stop_process_int_thrd(false)
	, thrd_run_mutex()
	, thrd_run_cond()
	, run_cond(false)
	, thrd_process_cmd_mutex()
	, thrd_process_cmd_cond()
	, process_cmd_cond(false)
	, thrd_mutex()
	, wait_for_command_processing(true)
{
	param_memory_atom_size.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_interrupt_poll_period_ms.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	
	param_memory_atom_size.SetMutable(false);
	param_architecture_description_filename.SetMutable(false);
	param_wait_connection_mode.SetMutable(false);
	param_enable_multiprocess_extension.SetMutable(false);
	param_enable_interrupt.SetMutable(false);
	
	pthread_mutex_init(&thrd_process_cmd_create_mutex, NULL);
	pthread_mutex_init(&thrd_process_int_create_mutex, NULL);
	pthread_mutex_init(&thrd_run_mutex, NULL);
	pthread_mutex_init(&thrd_process_cmd_mutex, NULL);
	pthread_mutex_init(&thrd_mutex, NULL);
	
	pthread_cond_init(&thrd_process_cmd_create_cond, NULL);
	pthread_cond_init(&thrd_process_int_create_cond, NULL);
	pthread_cond_init(&thrd_run_cond, NULL);
	pthread_cond_init(&thrd_process_cmd_cond, NULL);
}

template <class ADDRESS>
GDBServer<ADDRESS>::~GDBServer()
{
	KillFromSimulationRun();
	
	if(!StopProcessCmdThrd())
	{
		logger << DebugError << "Simulation thread: Can't stop thread processing GDB commands" << EndDebugError;
	}

	pthread_mutex_destroy(&thrd_process_cmd_create_mutex);
	pthread_mutex_destroy(&thrd_process_int_create_mutex);
	pthread_mutex_destroy(&thrd_run_mutex);
	pthread_mutex_destroy(&thrd_process_cmd_mutex);
	pthread_mutex_destroy(&thrd_mutex);
	
	pthread_cond_destroy(&thrd_process_cmd_create_cond);
	pthread_cond_destroy(&thrd_process_int_create_cond);
	pthread_cond_destroy(&thrd_run_cond);
	pthread_cond_destroy(&thrd_process_cmd_cond);

	unsigned int i;
	for(i = 0; i < gdb_registers.size(); i++)
	{
		GDBRegister *gdb_reg = gdb_registers[i];
		if(gdb_reg)
		{
			delete gdb_reg;
		}
	}

	for(i = 0; i < gdb_features.size(); i++)
	{
		GDBFeature *gdb_feature = gdb_features[i];
		if(gdb_feature)
		{
			delete gdb_feature;
		}
	}
	
	unsigned int num_fetch_insn_events = fetch_insn_events.size();
	for(i = 0; i < num_fetch_insn_events; i++)
	{
		unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = fetch_insn_events[i];
		fetch_insn_event->Release();
	}
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::EndSetup()
{
	if(memory_atom_size != 1 &&
	   memory_atom_size != 2 &&
	   memory_atom_size != 4 &&
	   memory_atom_size != 8 &&
	   memory_atom_size != 16)
	{
		logger << DebugError << "ERROR! memory-atom-size must be either 1, 2, 4, 8 or 16" << EndDebugError;
		return false;
	}

	if(!registers_import) return false;
	if(!debug_selecting_import) return false;
	
	c_prc_num = g_prc_num = debug_selecting_import->DebugGetSelected();
	
	if(debug)
	{
		logger << DebugInfo << "Current processor is Processor #" << g_prc_num << EndDebugInfo;
	}
	
	unsigned int prc_num;
	for(prc_num = 0; debug_selecting_import->DebugSelect(prc_num); prc_num++)
	{
		unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = new unisim::util::debug::FetchInsnEvent<ADDRESS>();
		fetch_insn_event->SetProcessorNumber(prc_num);
		fetch_insn_event->Catch();
		
		fetch_insn_events.push_back(fetch_insn_event);
	}
	
	num_processors = prc_num;
	prc_actions.resize(num_processors);
	listening_fetch.resize(num_processors);
	prc_trap.resize(num_processors);
	debug_selecting_import->DebugSelect(g_prc_num);

	if(!GDBServerBase::EndSetup()) return false;

	gdb_arch_reg_num = 0;
	
	unisim::util::xml::Parser *parser = new unisim::util::xml::Parser(logger.DebugInfoStream(), logger.DebugWarningStream(), logger.DebugErrorStream());
	unisim::util::xml::Node *root_node = parser->Parse(Object::GetSimulator()->SearchSharedDataFile(architecture_description_filename.c_str()));

	delete parser;

	if(root_node)
	{
		if(root_node->Name() == std::string("architecture"))
		{
			if(!VisitArchitecture(root_node))
			{
				delete root_node;
				return false;
			}
		}
		else
		{
			logger << DebugError << root_node->Filename() << ":" << root_node->LineNo() << ":root node is not named 'architecture'" << EndDebugError;
			delete root_node;
			return false;
		}

		delete root_node;
	}
	else
	{
		logger << DebugError << architecture_description_filename << ":no root node" << EndDebugError;
		return false;
	}

	if(!StartProcessCmdThrd())
	{
		logger << DebugError << "Failed to start thread processing GDB commands" << EndDebugError;
	}

	// we're in simulation thread: give us a chance to block simulation waiting for a client connection
	wait_for_command_processing = true;
	TriggerDebugYield();
	
	return true;
}

template <class ADDRESS>
void GDBServer<ADDRESS>::Kill()
{
	if(!run_cond) Run();
	unisim::kernel::Object::Kill();
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::StartServer()
{
	// Simulation is currently stalled (in WaitForCommandProcessing)
	switch(wait_connection_mode)
	{
		case GDB_WAIT_CONNECTION_NEVER:
			// let simulation run
			wait_for_command_processing = false;
			Run();
			break;
		case GDB_WAIT_CONNECTION_STARTUP_ONLY:
			// let simulation run and wait for command processing if it's first session
			if(session_num != 0)
			{
				wait_for_command_processing = false;
				Run();
			}
			else
			{
				logger << DebugInfo << "Simulation is waiting for a GDB client connection" << EndDebugInfo;
			}
			break;
		case GDB_WAIT_CONNECTION_ALWAYS:
			// don't let simulation run and make simulation wait for command processing
			logger << DebugInfo << "Simulation is waiting for a GDB client connection" << EndDebugInfo;
			break;
	}
	
	GDBServerBase::StartServer();

	c_prc_num = g_prc_num = debug_selecting_import->DebugGetSelected();
	ClearStopEvents();
	
	gdb_client_feature_multiprocess = false;
	gdb_client_feature_xmlregisters = false;
	gdb_client_feature_qrelocinsn = false;
	gdb_client_feature_swbreak = false;
	gdb_client_feature_hwbreak = false;
	gdb_client_feature_fork_events = false;
	gdb_client_feature_vfork_events = false;
	gdb_client_feature_exec_events = false;
	gdb_client_feature_vcont = false;
	gdb_client_feature_t32extensions = false;
	
	detached = false;
	no_ack_mode = false;
	session_terminated = false;
	extended_mode = false;
	synched = false;
	wait_for_command_processing = true;
	session_num++;
	return true;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::StopServer()
{
	if(IsConnected() && !Killed() && !session_terminated && !detached)
	{
		ReportProgramExit();
	}
	
	if(!GDBServerBase::StopServer()) return false;

	wait_for_command_processing = true;

	// make simulation call DebugYield another time waiting for us
	TriggerDebugYield();
		
	// let simulation run and finish waiting for command processing, so that we can return from DebugYield
	Run();

	return true;
}

template <class ADDRESS>
void GDBServer<ADDRESS>::Lock()
{
	pthread_mutex_lock(&thrd_mutex);
}

template <class ADDRESS>
void GDBServer<ADDRESS>::Unlock()
{
	pthread_mutex_unlock(&thrd_mutex);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::StartProcessCmdThrd()
{
	bool status = true;
	
	if(thrd_process_cmd_alive)
	{
		logger << DebugError << "Simulation thread: thread that process GDB commands has already started" << EndDebugError;
		status = false;
	}
	else
	{
		pthread_attr_t thrd_attr;
		pthread_attr_init(&thrd_attr);
		
// 		if(stack_size)
// 		{
// 			pthread_attr_setstacksize(&thrd_attr, stack_size);
// 		}
		
		pthread_mutex_lock(&thrd_process_cmd_create_mutex);

		thrd_process_cmd_alive = false;

		// Create a communication thread 
		if(unlikely(debug))
		{
			logger << DebugInfo << "Simulation thread: creating thread that process GDB commands" << EndDebugInfo;
		}
		if(pthread_create(&thrd_process_cmd, &thrd_attr, &GDBServer<ADDRESS>::ProcessCmdThrdEntryPoint, this) == 0)
		{
			// wait for creation of thread that process GDB commands
			do
			{
				pthread_cond_wait(&thrd_process_cmd_create_cond, &thrd_process_cmd_create_mutex);
			}
			while(!thrd_process_cmd_alive);
			
			if(unlikely(debug))
			{
				logger << DebugInfo << "Simulation thread: thread that process GDB command has started" << EndDebugInfo;
			}
		}
		else
		{
			// can't create thread that process GDB commands
			if(unlikely(debug))
			{
				logger << DebugInfo << "Simulation thread: can't create thread that process GDB commands" << EndDebugInfo;
			}
			status = false;
		}
		
		pthread_mutex_unlock(&thrd_process_cmd_create_mutex);

		pthread_attr_destroy(&thrd_attr);
	}
	
	return status;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::StopProcessCmdThrd()
{
	bool status = true;
	
	if(thrd_process_cmd_alive)
	{
		stop_process_cmd_thrd = true;

		if(unlikely(debug))
		{
			logger << DebugInfo << "Simulation thread: joining thread that process GDB commands" << EndDebugInfo;
		}
		
		if(pthread_join(thrd_process_cmd, NULL) == 0)
		{
			// thread that process GDB commands has gracefully exited
			if(unlikely(debug))
			{
				logger << DebugInfo << "Simulation thread: thread that process GDB commands has gracefully exited" << EndDebugInfo;
			}
			thrd_process_cmd_alive = false;
		}
		else
		{
			// can't join thread that process GDB commands
			if(unlikely(debug))
			{
				logger << DebugInfo << "Simulation thread: can't join thread that process GDB commands" << EndDebugInfo;
			}
			status = false;
		}
	}
	
	return status;
}

template <class ADDRESS>
void *GDBServer<ADDRESS>::ProcessCmdThrdEntryPoint(void *_self)
{
	GDBServer<ADDRESS> *self = static_cast<GDBServer<ADDRESS> *>(_self);
	pthread_mutex_lock(&self->thrd_process_cmd_create_mutex);
	self->thrd_process_cmd_alive = true;
	pthread_cond_signal(&self->thrd_process_cmd_create_cond);
	pthread_mutex_unlock(&self->thrd_process_cmd_create_mutex);
	
	self->ProcessCmdThrd();
	
	return 0;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::StartProcessIntThrd()
{
	bool status = true;
	
	if(thrd_process_int_alive)
	{
		logger << DebugError << "GDB command processing thread: thread that process GDB interrupt has already started" << EndDebugError;
		status = false;
	}
	else
	{
		pthread_attr_t thrd_attr;
		pthread_attr_init(&thrd_attr);
		
// 		if(stack_size)
// 		{
// 			pthread_attr_setstacksize(&thrd_attr, stack_size);
// 		}
		
		pthread_mutex_lock(&thrd_process_int_create_mutex);

		thrd_process_int_alive = false;

		// Create a thread that process GDB interrupt
		
		if(unlikely(debug))
		{
			logger << DebugInfo << "GDB command processing thread: creating thread that process GDB interrupts" << EndDebugInfo;
		}
		
		if(pthread_create(&thrd_process_int, &thrd_attr, &GDBServer<ADDRESS>::ProcessIntThrdEntryPoint, this) == 0)
		{
			// wait for creation of thread that process GDB interrupt
			do
			{
				pthread_cond_wait(&thrd_process_int_create_cond, &thrd_process_int_create_mutex);
			}
			while(!thrd_process_int_alive);
			
			if(unlikely(debug))
			{
				logger << DebugInfo << "GDB command processing thread: thread that process GDB interrupts has started" << EndDebugInfo;
			}
		}
		else
		{
			// can't create thread that process GDB interrupt
			if(unlikely(debug))
			{
				logger << DebugInfo << "GDB command processing thread: can't create thread that process GDB interrupt" << EndDebugInfo;
			}
			status = false;
		}
		
		pthread_mutex_unlock(&thrd_process_int_create_mutex);

		pthread_attr_destroy(&thrd_attr);
	}
	
	return status;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::StopProcessIntThrd()
{
	bool status = true;
	
	if(thrd_process_int_alive)
	{
		stop_process_int_thrd = true;
		
		if(unlikely(debug))
		{
			logger << DebugInfo << "GDB command processing thread: joining thread that process GDB interrupts" << EndDebugInfo;
		}

		if(pthread_join(thrd_process_int, NULL) == 0)
		{
			// thread has gracefully exited
			if(unlikely(debug))
			{
				logger << DebugInfo << "GDB command processing thread: thread that process GDB interrupts has gracefully exited" << EndDebugInfo;
			}
			thrd_process_int_alive = false;
		}
		else
		{
			// can't join communication thread
			if(unlikely(debug))
			{
				logger << DebugInfo << "GDB command processing thread: can't join thread that process GDB interrupts" << EndDebugInfo;
			}
			status = false;
		}
	}
	
	return status;
}

template <class ADDRESS>
void *GDBServer<ADDRESS>::ProcessIntThrdEntryPoint(void *_self)
{
	GDBServer<ADDRESS> *self = static_cast<GDBServer<ADDRESS> *>(_self);
	pthread_mutex_lock(&self->thrd_process_int_create_mutex);
	self->thrd_process_int_alive = true;
	pthread_cond_signal(&self->thrd_process_int_create_cond);
	pthread_mutex_unlock(&self->thrd_process_int_create_mutex);
	
	self->ProcessIntThrd();
	
	return 0;
}

template <class ADDRESS>
void GDBServer<ADDRESS>::UnblockCommandProcessing()
{
	if(unlikely(debug))
	{
		logger << DebugInfo << "Simulation thread: unblocking GDB command processing thread" << EndDebugInfo;
	}
	pthread_mutex_lock(&thrd_process_cmd_mutex);
	process_cmd_cond = true;
	pthread_cond_signal(&thrd_process_cmd_cond);
	pthread_mutex_unlock(&thrd_process_cmd_mutex);
}

template <class ADDRESS>
void GDBServer<ADDRESS>::WaitForCommandProcessing()
{
	if(unlikely(debug))
	{
		logger << DebugInfo << "Simulation thread: waiting for GDB command processing (start)" << EndDebugInfo;
	}
	if(!Killed()) // to avoid an interlock in case we've been Killed() before being able to process any GDB commands (e.g. accept failed in StartServer)
	{
		pthread_mutex_lock(&thrd_process_cmd_mutex);
		process_cmd_cond = true;
		pthread_cond_signal(&thrd_process_cmd_cond);
		pthread_mutex_lock(&thrd_run_mutex);
		run_cond = false;
		pthread_mutex_unlock(&thrd_process_cmd_mutex);
		
		do
		{
			pthread_cond_wait(&thrd_run_cond, &thrd_run_mutex);
		}
		while(!run_cond);
		
		pthread_mutex_unlock(&thrd_run_mutex);
	}
	if(unlikely(debug))
	{
		logger << DebugInfo << "Simulation thread: waiting for GDB command processing (end)" << EndDebugInfo;
	}
}

template <class ADDRESS>
void GDBServer<ADDRESS>::Run()
{
	if(unlikely(debug))
	{
		logger << DebugInfo << "GDB command processing thread: let simulation run" << EndDebugInfo;
	}
	process_cmd_cond = false;
	pthread_mutex_lock(&thrd_run_mutex);
	run_cond = true;
	pthread_cond_signal(&thrd_run_cond);
	pthread_mutex_unlock(&thrd_run_mutex);
}

template <class ADDRESS>
void GDBServer<ADDRESS>::WaitForSimulationRun()
{
	if(unlikely(debug))
	{
		logger << DebugInfo << "GDB command processing thread: waiting for simulation run (start)" << EndDebugInfo;
	}
	if(!process_cmd_cond)
	{
		pthread_mutex_lock(&thrd_process_cmd_mutex);
	
		do
		{
			pthread_cond_wait(&thrd_process_cmd_cond, &thrd_process_cmd_mutex);
		}
		while(!process_cmd_cond);
		
		pthread_mutex_unlock(&thrd_process_cmd_mutex);
	}
	if(unlikely(debug))
	{
		logger << DebugInfo << "GDB command processing thread: waiting for simulation run (end)" << EndDebugInfo;
	}
}

template <class ADDRESS>
void GDBServer<ADDRESS>::ProcessCmdThrd()
{
	// Don't allow ProcessIntThrd polling the network socket;
	// Make it block to avoid concurrent read of network socket
	if(unlikely(debug))
	{
		logger << DebugInfo << "GDB command processing thread: Locking" << EndDebugInfo;
	}
	Lock();
	
	if(enable_interrupt)
	{
		// start ProcessIntThrd
		if(!StartProcessIntThrd())
		{
			KillFromThrdProcessCmd();
			return;
		}
	}
	
	// Loop until there's stop or kill requests
	while(!stop_process_cmd_thrd && !Killed())
	{
		// start TCP/IP server and wait for GDB client connection
		if(!StartServer())
		{
			// server can't be started: kill ourself
			KillFromThrdProcessCmd();
			// make simulation call DebugYield so that it can stop simulation
			break;
		}
		
		// make simulation call us a first time
		TriggerDebugYield();
		
		// process GDB commands (GDB remote serial protocol dialect)
		ProcessCommands();
		
		// Unlisten all events
		ClearEvents();
		
		// stop TCP/IP server
		StopServer();
	}
	
	// Make ProcessIntThrd unblock to allow it to gracefully exit
	if(unlikely(debug))
	{
		logger << DebugInfo << "GDB command processing thread: Unlocking" << EndDebugInfo;
	}
	Unlock();
	
	if(enable_interrupt)
	{
		// Stop ProcessIntThrd
		StopProcessIntThrd();
	}
}

template <class ADDRESS>
void GDBServer<ADDRESS>::ProcessCommands()
{
	bool extended_mode = false;
	ADDRESS addr = 0;
	ADDRESS kind = 0;
	ADDRESS size = 0;
	ADDRESS reg_num = 0;
	ADDRESS type = 0;
	char ch = 0;
	char op = 0;
	long thread_id = 0;
	unsigned int prc_num = 0;

	std::string query;

	while(!stop_process_cmd_thrd && !session_terminated)
	{
		if(unlikely(debug))
		{
			logger << DebugInfo << "GDB command processing thread: pseudo-blocking read" << EndDebugInfo;
		}
		if(!GetQuery(query, true))
		{
			break;
		}
		
		std::size_t pos = 0;
		std::size_t len = query.length();

		if(!ParseChar(query, pos, ch)) { session_terminated = !PutReply(""); continue; }
		
		switch(ch)
		{
			case 'g':
				session_terminated = !ReadRegisters();
				break;

			case 'p':
				if(!ParseHex(query, pos, reg_num)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_HEX, pos); break; }
				session_terminated = !ReadRegister(reg_num);
				break;

			case 'G':
				session_terminated = !WriteRegisters(query.substr(1));
				break;

			case 'P':
				if(!ParseHex(query, pos, reg_num)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_HEX, pos); break; }
				if(!MatchChar(query, pos, '=')) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_ASSIGNMENT); break; }
				session_terminated = !WriteRegister(reg_num, query.substr(pos));
				break;

			case 'm':
				if(!ParseHex(query, pos, addr)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_HEX, pos); break; }
				if(!MatchChar(query, pos, ',')) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_COMMA, pos); break; }
				if(!ParseHex(query, pos, size)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_HEX, pos); break; }
				if(pos != len) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_GARBAGE, pos); break; }
				session_terminated = !ReadMemoryHex(addr * memory_atom_size, size);
				break;

			case 'M':
				if(!ParseHex(query, pos, addr)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_HEX, pos); break; }
				if(!MatchChar(query, pos, ',')) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_COMMA, pos); break; }
				if(!ParseHex(query, pos, size)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_HEX, pos); break; }
				if(!MatchChar(query, pos, ':')) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_COLON, pos); break; }
				session_terminated = !WriteMemoryHex(addr * memory_atom_size, query.substr(pos), size);
				break;
				
			case 'X':
				if(!ParseHex(query, pos, addr)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_HEX, pos); break; }
				if(!MatchChar(query, pos, ',')) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_COMMA, pos); break; }
				if(!ParseHex(query, pos, size)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_HEX, pos); break; }
				if(!MatchChar(query, pos, ':')) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_COLON, pos); break; }
				session_terminated = !WriteMemoryBin(addr * memory_atom_size, query.substr(pos), size);
				break;

			case 's':
				if(!ParseHex(query, pos, addr))
				{
					synched = false;
					session_terminated = !Step();
					break;
				}
				if(gdb_pc)
				{
					Barrier();
					gdb_pc->SetValue(c_prc_num, &addr);
				}
				else
				{
					if(verbose)
					{
						logger << DebugWarning << "CPU has no program counter" << EndDebugWarning;
					}
				}
				synched = false;
				session_terminated = !Step();
				break;

			case 'c':
				if(!ParseHex(query, pos, addr))
				{
					session_terminated = !Continue();
					break;
				}
				addr *= memory_atom_size;
				if(gdb_pc)
				{
					Barrier();
					gdb_pc->SetValue(c_prc_num, &addr);
				}
				else
				{
					if(verbose)
					{
						logger << DebugWarning << "CPU has no program counter" << EndDebugWarning;
					}
				}
				session_terminated = !Continue();
				break;

			case 'H':
				if(mode == GDB_MODE_MULTI_THREAD)
				{
					if(!MatchChar(query, pos, 'c', 'g', op)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_OPERATION, pos); break; }
					if(!ParseThreadId(query, pos, thread_id)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_THREAD_ID, pos); break; }
					if(pos != len) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_GARBAGE, pos); break; }
					if((thread_id == -1) || (thread_id == 0))
					{
						session_terminated = !PutReply("OK");
						break;
					}
					
					if(op == 'c')
					{
						session_terminated = !SetCThread(thread_id);
						break;
					}
					else if(op == 'g')
					{
						session_terminated = !SetGThread(thread_id);
						break;
					}
					else
					{
						session_terminated = !PutErrorReply(GDB_SERVER_ERROR_INVALID_OPERATION);
						break;
					}
				}
				else
				{
					if(verbose)
					{
						logger << DebugWarning << "Multi-threading is currently disabled" << EndDebugWarning;
					}
					session_terminated = !PutReply("");
				}
				break;
				
			case 'T':
				if(mode == GDB_MODE_MULTI_THREAD)
				{
					if(!ParseThreadId(query, pos, thread_id)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_THREAD_ID, pos); break; }
					if(pos != len) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_GARBAGE, pos); break; }
					if(!ThreadIdToProcessorNumber(thread_id, prc_num)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_INVALID_THREAD_ID); break; }
					session_terminated = !PutReply("OK");
				}
				else
				{
					if(verbose)
					{
						logger << DebugWarning << "Multi-threading is currently disabled" << EndDebugWarning;
					}
					session_terminated = !PutReply("");
				}
				break;

			case 'k':
				KillFromThrdProcessCmd();
				break;
				
			case 'D':
				detached = true;
				session_terminated = !PutReply("OK");
				break;

			case 'R':
				if(extended_mode)
				{
					// FIXME: should reset
					// Note 'R' is only available in extended mode and has no reply
					break;
				}
				else
				{
					session_terminated = !PutReply("");
				}
				break;

			case '?':
				session_terminated = !ReportSignal(2 /* SIGINT */);
				break;

			case '!':
				extended_mode = true;
				session_terminated = !PutReply("OK");
				break;

			case 'Z':
				if(!ParseHex(query, pos, type)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_HEX, pos); break; }
				if(!MatchChar(query, pos, ',')) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_COMMA, pos); break; }
				if(!ParseHex(query, pos, addr)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_HEX, pos); break; }
				if(!MatchChar(query, pos, ',')) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_COMMA, pos); break; }
				if(!ParseHex(query, pos, kind)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_HEX, pos); break; }
				session_terminated = !SetBreakpointWatchpoint(type, addr * memory_atom_size, kind);
				break;

			case 'z':
				if(!ParseHex(query, pos, type)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_HEX, pos); break; }
				if(!MatchChar(query, pos, ',')) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_COMMA, pos); break; }
				if(!ParseHex(query, pos, addr)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_HEX, pos); break; }
				if(!MatchChar(query, pos, ',')) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_COMMA, pos); break; }
				if(!ParseHex(query, pos, kind)) { session_terminated = !PutErrorReply(GDB_SERVER_ERROR_EXPECTING_HEX, pos); break; }
				session_terminated = !RemoveBreakpointWatchpoint(type, addr * memory_atom_size, kind);
				break;
				
			case 'q':
				if((query == "qSymbol::") || (query == "qSymbol::"))
				{
					session_terminated = !PutReply("OK");
				}
				else if(MatchString(query, pos, "Rcmd,"))
				{
					session_terminated = !HandleQRcmd(query, pos);
				}
				else if(MatchString(query, pos, "Supported"))
				{
					session_terminated = !HandleQSupported(query, pos);
				}
				else if(query == "qC")
				{
					session_terminated = !HandleQC();
				}
				else if(MatchString(query, pos, "Attached"))
				{
					session_terminated = !HandleQAttached(query, pos);
				}
				else if(MatchString(query, pos, "TStatus"))
				{
					session_terminated = !HandleQTStatus();
				}
				else if(MatchString(query, pos, "Xfer:features:read:"))
				{
					session_terminated = !HandleQXferFeaturesRead(query, pos);
				}
				else if(MatchString(query, pos, "Xfer:threads:read::"))
				{
					session_terminated = !HandleQXferThreadsRead(query, pos);
				}
				else if(query == "qfThreadInfo")
				{
					session_terminated = !HandleQfThreadInfo();
				}
				else if(query == "qsThreadInfo")
				{
					session_terminated = !HandleQsThreadInfo();
				}
				else if(MatchString(query, pos, "ThreadExtraInfo,"))
				{
					session_terminated = !HandleQThreadExtraInfo(query, pos);
				}
				else if(MatchString(query, pos, "RegisterInfo"))
				{
					session_terminated = !HandleQRegisterInfo(query, pos);
				}
				else
				{
					if(verbose)
					{
						logger << DebugWarning << "Received an unknown GDB remote protocol general query" << EndDebugWarning;
					}
					session_terminated = !PutReply("");
				}
				break;
				
			case 'Q':
				if(MatchString(query, pos, "StartNoAckMode"))
				{
					session_terminated = !HandleQStartNoAckMode();
				}
				else
				{
					if(verbose)
					{
						logger << DebugWarning << "Received an unknown GDB remote protocol set packet" << EndDebugWarning;
					}
					session_terminated = !PutReply("");
				}
				break;

			case 'v':
				if(query == "vCont?")
				{
					session_terminated = !PutReply("vCont;c;C;s;S");
				}
				else if(MatchString(query, pos, "Cont"))
				{
					session_terminated = !HandleVCont(query, pos);
				}
				else if(MatchString(query, pos, "Kill"))
				{
					KillFromThrdProcessCmd();
				}
				else
				{
					if(verbose)
					{
						logger << DebugWarning << "Received an unknown GDB remote protocol 'v' query" << EndDebugWarning;
					}
					session_terminated = !PutReply("");
				}
				break;
				
			default:
				if(verbose)
				{
					logger << DebugWarning << "Received an unknown GDB remote protocol query" << EndDebugWarning;
				}
				session_terminated = !PutReply("");
				break;
		} // end of switch
	}
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::Step()
{
	Barrier();

	bool status = true;
	ListenFetch(c_prc_num);
	if(unlikely(debug))
	{
		logger << DebugInfo << "GDB command processing thread: Unlocking" << EndDebugInfo;
	}
	Unlock();
	do
	{
		Run();
		WaitForSimulationRun();
	}
	while(!trap && !Killed());
	if(unlikely(debug))
	{
		logger << DebugInfo << "GDB command processing thread: Locking" << EndDebugInfo;
	}
	Lock();
	if(!Killed())
	{
		UnlistenFetch(c_prc_num);
		if(status) status = DisplayMonitoredInternals();
		if(status) status = ReportSignal(5 /* SIGTRAP */);
		if(trap) ClearStopEvents();
	}
	return status;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::Continue()
{
	Barrier();

	bool status = true;
	if(unlikely(debug))
	{
		logger << DebugInfo << "GDB command processing thread: Unlocking" << EndDebugInfo;
	}
	Unlock();
	do
	{
		Run();
		WaitForSimulationRun();
	}
	while(!trap && !Killed());
	if(unlikely(debug))
	{
		logger << DebugInfo << "GDB command processing thread: Locking" << EndDebugInfo;
	}
	Lock();
	if(!Killed())
	{
		if(status) status = DisplayMonitoredInternals();
		if(status) status = ReportSignal(5 /* SIGTRAP */);
		if(trap) ClearStopEvents();
	}
	return status;
}

template <class ADDRESS>
void GDBServer<ADDRESS>::ClearEvents()
{
	if(debug_event_trigger_import)
	{
		debug_event_trigger_import->ClearEvents();
	}
}

template <class ADDRESS>
void GDBServer<ADDRESS>::ProcessIntThrd()
{
	// poll network socket every XX ms, keep alive between sessions
	while(!stop_process_int_thrd && !Killed())
	{
		if(!session_terminated)
		{
			if(unlikely(debug))
			{
				logger << DebugInfo << "GDB interrupt processing thread: Locking" << EndDebugInfo;
			}
			Lock();
			if(IsConnected())
			{
				char c;
				if(unlikely(debug))
				{
					logger << DebugInfo << "GDB interrupt processing thread: non-blocking read" << EndDebugInfo;
				}
				if(GetChar(c, false))
				{
					if(c == 0x03)
					{
						// Interrupted by GDB client
						if(unlikely(debug))
						{
							logger << DebugInfo << "GDB interrupt processing thread: interrupted by GDB client" << EndDebugInfo;
						}
						Interrupt();
					}
				}
			}
			
			if(unlikely(debug))
			{
				logger << DebugInfo << "GDB interrupt processing thread: Unlocking" << EndDebugInfo;
			}
			Unlock();
		}
		
		WaitTime(interrupt_poll_period_ms); // retry later
	}
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::VisitArchitecture(unisim::util::xml::Node *xml_node)
{
	bool has_architecture_name = false;
	bool has_architecture_endian = false;
	std::string architecture_name;
	endian = GDB_BIG_ENDIAN;

	const std::list<unisim::util::xml::Property *> *xml_node_properties = xml_node->Properties();

	typename std::list<unisim::util::xml::Property *>::const_iterator xml_node_property;
	for(xml_node_property = xml_node_properties->begin(); xml_node_property != xml_node_properties->end(); xml_node_property++)
	{
		if((*xml_node_property)->Name() == std::string("name"))
		{
			architecture_name = (*xml_node_property)->Value();
			has_architecture_name = true;
		}
		else if((*xml_node_property)->Name() == std::string("endian"))
		{
			const std::string& architecture_endian = (*xml_node_property)->Value();
			if(architecture_endian == "little")
			{
				endian = GDB_LITTLE_ENDIAN;
				has_architecture_endian = true;
			}
			else if(architecture_endian == "big")
			{
				endian = GDB_BIG_ENDIAN;
				has_architecture_endian = true;
			}
		}
		else
		{
			logger << DebugWarning << (*xml_node_property)->Filename() << ":" << (*xml_node_property)->LineNo() << ":ignoring property '" << (*xml_node_property)->Name() << "'" << EndDebugWarning;
		}
	}

	if(!has_architecture_name)
	{
		logger << DebugWarning << xml_node->Filename() << ":" << xml_node->LineNo() << ":architecture has no property 'name'" << EndDebugWarning;
		return false;
	}

	if(!has_architecture_endian)
	{
		logger << DebugWarning << xml_node->Filename() << ":" << xml_node->LineNo() << ":assuming target architecture endian is big endian" << EndDebugWarning;
	}
	
	typename std::list<unisim::util::xml::Node *>::const_iterator xml_program_counter_node;
	const std::list<unisim::util::xml::Node *> *xml_child_nodes = xml_node->Childs();
	typename std::list<unisim::util::xml::Node *>::const_iterator xml_child_node;
	bool has_program_counter = false;

	for(xml_child_node = xml_child_nodes->begin(); xml_child_node != xml_child_nodes->end(); xml_child_node++)
	{
		if((*xml_child_node)->Name() == std::string("feature"))
		{
			if(!VisitFeature(*xml_child_node)) return false;
		}
		else if((*xml_child_node)->Name() == std::string("program_counter"))
		{
			has_program_counter = true;
			xml_program_counter_node = xml_child_node;
			if(!VisitProgramCounter(*xml_child_node)) return false;
		}
		else if((*xml_child_node)->Name() == std::string("breakpoint_kind"))
		{
			if(!VisitBreakPointKind(*xml_child_node)) return false;
		}
		else
		{
			logger << DebugWarning << (*xml_child_node)->Filename() << ":" << (*xml_child_node)->LineNo() << ":ignoring tag '" << (*xml_child_node)->Name() << "'" << EndDebugWarning;
		}
	}
	
	if(!has_program_counter)
	{
		logger << DebugError << xml_node->Filename() << ":" << xml_node->LineNo() << ":architecture has no program counter" << EndDebugError;
		return false;
	}

	typename std::vector<GDBRegister *>::iterator reg_iter;

	for(reg_iter = gdb_registers.begin(); reg_iter != gdb_registers.end(); reg_iter++)
	{
		if((*reg_iter)->GetName() == program_counter_name)
		{
			gdb_pc = *reg_iter;
			break;
		}
	}

	if(!gdb_pc)
	{
		logger << DebugError << (*xml_program_counter_node)->Filename() << ":" << (*xml_program_counter_node)->LineNo() << ":program counter does not belong to registers" << EndDebugError;
		return false;
	}
	
	return true;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::VisitFeature(unisim::util::xml::Node *xml_node)
{
	std::string feature_name;
	
	const std::list<unisim::util::xml::Property *> *xml_node_properties = xml_node->Properties();

	typename std::list<unisim::util::xml::Property *>::const_iterator xml_node_property;
	for(xml_node_property = xml_node_properties->begin(); xml_node_property != xml_node_properties->end(); xml_node_property++)
	{
		if((*xml_node_property)->Name() == std::string("name"))
		{
			feature_name = (*xml_node_property)->Value();
		}
		else
		{
			logger << DebugWarning << (*xml_node_property)->Filename() << ":" << (*xml_node_property)->LineNo() << ":ignoring property '" << (*xml_node_property)->Name() << "'" << EndDebugWarning;
		}
	}
	
	GDBFeature *gdb_feature = new GDBFeature(feature_name, gdb_features.size());
	gdb_features.push_back(gdb_feature);
	
	const std::list<unisim::util::xml::Node *> *xml_child_nodes = xml_node->Childs();
	typename std::list<unisim::util::xml::Node *>::const_iterator xml_child_node;
	for(xml_child_node = xml_child_nodes->begin(); xml_child_node != xml_child_nodes->end(); xml_child_node++)
	{
		if((*xml_child_node)->Name() == std::string("register"))
		{
			if(!VisitRegister(*xml_child_node, gdb_feature)) return false;
		}
	}
	
	return true;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::VisitBreakPointKind(unisim::util::xml::Node *xml_node)
{
	bool has_kind = false;
	bool has_size = false;
	uint32_t kind = 0;
	uint32_t size = 0;
	
	const std::list<unisim::util::xml::Property *> *xml_node_properties = xml_node->Properties();
	
	typename std::list<unisim::util::xml::Property *>::const_iterator xml_node_property;
	for(xml_node_property = xml_node_properties->begin(); xml_node_property != xml_node_properties->end(); xml_node_property++)
	{
		if((*xml_node_property)->Name() == std::string("kind"))
		{
			kind = atoi((*xml_node_property)->Value().c_str());
			has_kind = true;
		}
		else if((*xml_node_property)->Name() == std::string("size"))
		{
			size = atoi((*xml_node_property)->Value().c_str());
			has_size = true;
		}
	}
	
	if(!has_kind)
	{
		logger << DebugWarning << xml_node->Filename() << ":" << xml_node->LineNo() << ":node '" << xml_node->Name() << "' has no 'kind'" << EndDebugWarning;
		return false;
	}
	if(!has_size)
	{
		logger << DebugWarning << xml_node->Filename() << ":" << xml_node->LineNo() << ":node '" << xml_node->Name() << "' has no 'size'" << EndDebugWarning;
		return false;
	}
	
	if(arch_specific_breakpoint_kinds.find(kind) != arch_specific_breakpoint_kinds.end())
	{
		logger << DebugWarning << xml_node->Filename() << ":" << xml_node->LineNo() << ":'kind' in node '" << xml_node->Name() << "' is already declared in a previous node" << EndDebugWarning;
		return false;
	}
	
	arch_specific_breakpoint_kinds[kind] = size;
	
	return true;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::VisitRegister(unisim::util::xml::Node *xml_node, GDBFeature *gdb_feature)
{
	std::string reg_name;
	bool has_reg_name = false;
	int reg_bitsize = 0;
	bool has_reg_bitsize = false;
	std::string reg_type;
	std::string reg_group;

	const std::list<unisim::util::xml::Property *> *xml_node_properties = xml_node->Properties();

	typename std::list<unisim::util::xml::Property *>::const_iterator xml_node_property;
	for(xml_node_property = xml_node_properties->begin(); xml_node_property != xml_node_properties->end(); xml_node_property++)
	{
		if((*xml_node_property)->Name() == std::string("name"))
		{
			reg_name = (*xml_node_property)->Value();
			has_reg_name = true;
		}
		else
		{
			if((*xml_node_property)->Name() == std::string("bitsize"))
			{
				reg_bitsize = atoi((*xml_node_property)->Value().c_str());
				has_reg_bitsize = true;
				if(reg_bitsize == 0)
				{
					logger << DebugWarning << (*xml_node_property)->Filename() << ":" << (*xml_node_property)->LineNo() << ": bitsize should not be 0" << EndDebugWarning;
				}
			}
			else
			{
				if((*xml_node_property)->Name() == std::string("regnum"))
				{
					gdb_arch_reg_num = atoi((*xml_node_property)->Value().c_str());
				}
				else if((*xml_node_property)->Name() == std::string("type"))
				{
					reg_type = (*xml_node_property)->Value().c_str();
				}
				else if((*xml_node_property)->Name() == std::string("group"))
				{
					reg_group = (*xml_node_property)->Value().c_str();
				}
				else
				{
					logger << DebugWarning << (*xml_node_property)->Filename() << ":" << (*xml_node_property)->LineNo() << ":ignoring property '" << (*xml_node_property)->Name() << "'" << EndDebugWarning;
				}
			}
		}
	}

	if(has_reg_name && has_reg_bitsize)
	{
		if(!reg_name.empty())
		{
			if(gdb_arch_reg_num >= gdb_registers.size())
			{
				gdb_registers.resize(gdb_arch_reg_num + 1);
			}
		
			gdb_registers[gdb_arch_reg_num] = new GDBRegister(reg_name, reg_bitsize, endian, gdb_arch_reg_num, reg_type, reg_group);
		
			unsigned int prc_num;
			
			for(prc_num = 0; prc_num < num_processors; prc_num++)
			{
				bool cpu_has_reg = true;
				bool cpu_has_right_reg_bitsize = true;
				
				unsigned int save_cur_prc_num = debug_selecting_import->DebugGetSelected();
				
				unisim::service::interfaces::Register *reg = 0;
				
				if(debug_selecting_import->DebugSelect(prc_num))
				{
					reg = registers_import->GetRegister(reg_name.c_str());
					debug_selecting_import->DebugSelect(save_cur_prc_num);
				}
				
				if(!reg)
				{
					cpu_has_reg = false;
					if(verbose)
					{
						logger << DebugWarning << "CPU #" << prc_num << " does not support register " << reg_name << EndDebugWarning;
					}
				}
				else
				{
					if((8 * reg->GetSize()) != reg_bitsize)
					{
						cpu_has_right_reg_bitsize = false;
						if(verbose)
						{
							logger << DebugWarning << ": register size (" << reg_bitsize << " bits) doesn't match with size (" << 8 * reg->GetSize() << " bits) reported by CPU #" << prc_num << EndDebugWarning;
						}
					}
				}
				
				if(cpu_has_reg && cpu_has_right_reg_bitsize)
				{
					gdb_registers[gdb_arch_reg_num]->SetRegisterInterface(prc_num, reg);
				}
			}
		}

#if 0
		bool cpu_has_reg = true;
		bool cpu_has_right_reg_bitsize = true;

		unisim::service::interfaces::Register *reg = 0;
		
		if(!reg_name.empty())
		{
			reg = registers_import->GetRegister(reg_name.c_str());

			if(!reg)
			{
				cpu_has_reg = false;
				if(verbose)
				{
					logger << DebugWarning << "CPU does not support register " << reg_name << EndDebugWarning;
				}
			}
			else
			{
				if((8 * reg->GetSize()) != reg_bitsize)
				{
					cpu_has_right_reg_bitsize = false;
					if(verbose)
					{
						logger << DebugWarning << ": register size (" << reg_bitsize << " bits) doesn't match with size (" << 8 * reg->GetSize() << " bits) reported by CPU" << EndDebugWarning;
					}
				}
			}
		}

		if(gdb_arch_reg_num >= gdb_registers.size())
		{
			gdb_registers.resize(gdb_arch_reg_num + 1);
		}
		
		if(cpu_has_reg && cpu_has_right_reg_bitsize)
		{
			gdb_registers[gdb_arch_reg_num] = new GDBRegister(reg, reg_name, endian, gdb_arch_reg_num, reg_type, reg_group);
		}
		else
		{
			gdb_registers[gdb_arch_reg_num] = new GDBRegister(reg_name, reg_bitsize, endian, gdb_arch_reg_num, reg_type, reg_group);
		}
#endif
		
		gdb_feature->AddRegister(gdb_registers[gdb_arch_reg_num]);
	}
	else
	{
		logger << DebugWarning << xml_node->Filename() << ":" << xml_node->LineNo() << ":node '" << xml_node->Name() << "' has no 'name' or 'bitsize' property" << EndDebugWarning;
	}
	
	gdb_arch_reg_num++;
	
	return true;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::VisitProgramCounter(unisim::util::xml::Node *xml_node)
{
	bool has_program_counter_name = false;
	const std::list<unisim::util::xml::Property *> *xml_node_properties = xml_node->Properties();

	typename std::list<unisim::util::xml::Property *>::const_iterator xml_node_property;
	for(xml_node_property = xml_node_properties->begin(); xml_node_property != xml_node_properties->end(); xml_node_property++)
	{
		if((*xml_node_property)->Name() == std::string("name"))
		{
			program_counter_name = (*xml_node_property)->Value();
			has_program_counter_name = true;
		}
	}

	if(!has_program_counter_name)
	{
		logger << DebugWarning << xml_node->Filename() << ":" << xml_node->LineNo() << ":node '" << xml_node->Name() << "' has no 'name'" << EndDebugWarning;
		return false;
	}
	
	return true;
}

template <class ADDRESS>
void GDBServer<ADDRESS>::OnDisconnect()
{
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ParseChar(const std::string& s, std::size_t& pos, char& value)
{
	std::size_t len = s.length();

	if(pos < len)
	{
		value = s[pos++];
		return true;
	}
	
	return false;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::MatchChar(const std::string& s, std::size_t& pos, char value)
{
	char ch = 0;
	if(!ParseChar(s, pos, ch)) return false;
	if(ch != value)
	{
		pos--;
		return false;
	}
	
	return true;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::MatchChar(const std::string& s, std::size_t& pos, char value1, char value2, char& value)
{
	char ch = 0;
	if(!ParseChar(s, pos, ch)) return false;
	if((ch != value1) && (ch != value2))
	{
		pos--;
		return false;
	}
	value = ch;
	return true;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::MatchString(const std::string& s, std::size_t& _pos, const std::string& value)
{
	std::size_t pos = _pos;
	std::size_t i = 0;
	std::size_t len = s.length();
	std::size_t value_len = value.length();
	
	for(i = 0; (i < value_len) && (pos < len); i++, pos++)
	{
		if(s[pos] != value[i])
		{
			return false;
		}
	}
	
	if(i < value_len)
	{
		return false;
	}

	_pos = pos;
	return true;
}

template <class ADDRESS>
template <typename T>
bool GDBServer<ADDRESS>::ParseHex(const std::string& s, std::size_t& pos, T& value)
{
	std::size_t len = s.length();
	std::size_t n = 0;

	value = 0;
	while((pos < len) && (n < (2 * sizeof(T))))
	{
		uint8_t nibble;
		if(!IsHexChar(s[pos])) break;
		nibble = HexChar2Nibble(s[pos]);
		value <<= 4;
		value |= nibble;
		pos++;
		n++;
	}
	return n > 0;
}

template <class ADDRESS>
template <typename T>
bool GDBServer<ADDRESS>::ParseSignedHex(const std::string& s, std::size_t& pos, T& value)
{
	char sign = '+';
	if(!ParseChar(s, pos, sign)) return false;
	if((sign != '+') && (sign != '-')) { sign = '+'; pos--; }
	if(!ParseHex(s, pos, value)) return false;
	if(sign == '-') value = -value;
	return true;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ParseThreadId(const std::string& s, std::size_t& pos, long& thread_id)
{
	if(ALWAYS_ACCEPT_MULTIPROCESS_NEW_THREAD_ID_SYNTAX || (enable_multiprocess_extension && gdb_client_feature_multiprocess))
	{
		char c = 0;
		// eat an optional 'p'
		if(ParseChar(s, pos, c))
		{
			if(c != 'p')
			{
				pos--;
			}
		}
		
		if(!ParseSignedHex(s, pos, thread_id))
		{
			return false;
		}
		
		if(thread_id < -1) return false;

		if(!ParseChar(s, pos, c))
		{
			return true;
		}
		
		if(c != '.')
		{
			pos--;
			return true;
		}
		
		long process_id = thread_id;
		
		if(process_id == PROCESS_ID)
		{
			if(!ParseSignedHex(s, pos, thread_id))
			{
				return false;
			}
		
			if(thread_id < -1) return false;
		
			return true;
		}
		
		return false;
	}
	else
	{
		if(!ParseSignedHex(s, pos, thread_id))
		{
			return false;
		}
	}
	
	return true;
}

template <class ADDRESS>
void GDBServer<ADDRESS>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
{
	typename unisim::util::debug::Event<ADDRESS>::Type event_type = event->GetType();
	
	unsigned int prc_num = event->GetProcessorNumber();
	
	if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_FETCH_INSN))
	{
		const unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = static_cast<const unisim::util::debug::FetchInsnEvent<ADDRESS> *>(event);
		if(unlikely(verbose))
		{
			logger << DebugInfo << "/\\/\\/\\ " << *fetch_insn_event << EndDebugInfo;
		}
		
		prc_trap[prc_num] = true;
		trap = true;
		return;
	}
	else if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_COMMIT_INSN))
	{
		if(unlikely(verbose))
		{
			const unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event = static_cast<const unisim::util::debug::CommitInsnEvent<ADDRESS> *>(event);
			logger << DebugInfo << "/\\/\\/\\ " << *commit_insn_event << EndDebugInfo;
		}
	}
	else if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT))
	{
		if(unlikely(verbose))
		{
			const unisim::util::debug::Breakpoint<ADDRESS> *breakpoint = static_cast<const unisim::util::debug::Breakpoint<ADDRESS> *>(event);
			logger << DebugInfo << "/\\/\\/\\ " << *breakpoint << EndDebugInfo;
		}
		event->Catch();
		stop_events.push_back(event);
		prc_trap[prc_num] = true;
		trap = true;
	}
	else if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT))
	{
		if(unlikely(verbose))
		{
			const unisim::util::debug::Watchpoint<ADDRESS> *watchpoint = static_cast<const unisim::util::debug::Watchpoint<ADDRESS> *>(event);
			logger << DebugInfo << "/\\/\\/\\ " << *watchpoint << EndDebugInfo;
		}
		event->Catch();
		stop_events.push_back(event);
		prc_trap[prc_num] = true;
		trap = true;
	}
	else if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_TRAP))
	{
		if(unlikely(verbose))
		{
			const unisim::util::debug::TrapEvent<ADDRESS> *trap_event = static_cast<const unisim::util::debug::TrapEvent<ADDRESS> *>(event);
			logger << DebugInfo << "/\\/\\/\\ " << *trap_event << EndDebugInfo;
		}
		event->Catch();
		stop_events.push_back(event);
		prc_trap[prc_num] = true;
		trap = true;
	}
	else
	{
		if(unlikely(debug))
		{
			logger << DebugInfo << "/\\/\\/\\ unknown event" << EndDebugInfo;
		}
		// ignore event
		return;
	}

	synched = true;
}

template <class ADDRESS>
void GDBServer<ADDRESS>::DebugYield()
{
	if(unlikely(debug))
	{
		logger << DebugInfo << "debug yield (start)" << EndDebugInfo;
	}
	if(likely(wait_for_command_processing))
	{
		WaitForCommandProcessing();
		if(unlikely(Killed()))
		{
			if(unlikely(debug))
			{
				logger << DebugInfo << "debug yield (end)" << EndDebugInfo;
			}
			unisim::kernel::Object::Stop(-1);
			return;
		}
	}
	if(unlikely(debug))
	{
		logger << DebugInfo << "debug yield (end)" << EndDebugInfo;
	}
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::GetPacket(char prefix, std::string& s, bool blocking)
{
	uint8_t checksum;
	uint8_t received_checksum;
	char c;
	s.erase();

	while(1)
	{
		while(1)
		{
			if(!GetChar(c, blocking)) return false;
			if(c == prefix) break;
		}

		checksum = 0;

		while(1)
		{
			if(!GetChar(c, true)) return false;
			if(c == prefix)
			{
				checksum = 0;
				s.erase();
				continue;
			}

			if(c == '#') break;
			checksum = checksum + (uint8_t ) c;
			s.append(1, c); // Note: don't use += because null characters can't be appended this way to the string
		}

		if(c == '#')
		{
			if(!GetChar(c, true)) break;
			received_checksum = HexChar2Nibble(c) << 4;
			if(!GetChar(c, true)) break;
			received_checksum += HexChar2Nibble(c);

			if(verbose)
			{
				logger << DebugInfo << "receiving " << prefix;
				// Note: packet may have characters that can't be printed
				std::size_t length = s.length();
				std::size_t pos;
				for(pos = 0; pos < length; pos++)
				{
					c = s[pos];
					if((c >= 32) && (c != 127))
					{
						logger << c;
					}
					else
					{
						logger << ' ';
					}
				}
				logger << "#" << Nibble2HexChar((received_checksum >> 4) & 0xf) << Nibble2HexChar(received_checksum & 0xf) << EndDebugInfo;
			}
			if(checksum != received_checksum)
			{
				if(!no_ack_mode)
				{
					if(!PutChar('-')) return false;
				}
				if(!FlushOutput()) return false;
			}
			else
			{
				if(!no_ack_mode)
				{
					if(!PutChar('+')) return false;
				}
				if(!FlushOutput()) return false;

				if(s.length() >= 3 && s[2] == ':')
				{
					if(!PutChar(s[0])) return false;
					if(!PutChar(s[1])) return false;
					if(!FlushOutput()) return false;
					s.erase(0, 3);
				}
				return true;
			}
		}
	}
	return false;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::GetQuery(std::string& s, bool blocking)
{
	return GetPacket('$', s, blocking);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::PutPacket(char prefix, const std::string& s)
{
	uint8_t checksum;
	unsigned int pos;
	unsigned int len;
	char c;

	do
	{
		if(!PutChar(prefix)) return false;
		checksum = 0;
		pos = 0;
		len = s.length();

		while(pos < len)
		{
			c = s[pos];
			if(!PutChar(c)) return false;
			checksum += (uint8_t) c;
			pos++;
		}
		if(!PutChar('#')) return false;
		if(!PutChar(Nibble2HexChar(checksum >> 4))) return false;
		if(!PutChar(Nibble2HexChar(checksum & 0xf))) return false;
		if(verbose)
		{
			logger << DebugInfo << "sending " << prefix;
			// Note: packet may have characters that can't be printed
			std::size_t length = s.length();
			std::size_t pos;
			for(pos = 0; pos < length; pos++)
			{
				c = s[pos];
				if((c >= 32) && (c != 127))
				{
					logger << c;
				}
				else
				{
					logger << ' ';
				}
			}
			logger << "#" << Nibble2HexChar(checksum >> 4) << Nibble2HexChar(checksum & 0xf) << EndDebugInfo;
		}
		if(!FlushOutput()) return false;
	} while(!no_ack_mode && GetChar(c, true) && (c != '+'));
	return true;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::PutReply(const std::string& s)
{
	return PutPacket('$', s);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::PutNotification(const std::string& s)
{
	return PutPacket('%', s);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::PutErrorReply(GDBServerError gdb_server_error)
{
	if(verbose)
	{
		logger << DebugInfo << "remote serial protocol error: " << gdb_server_error << EndDebugInfo;
	}
	
	std::stringstream sstr;
	sstr << "E" << std::hex;
	sstr.width(2);
	sstr.fill('0');
	sstr << (unsigned int) gdb_server_error;
	return PutReply(sstr.str());
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::PutErrorReply(GDBServerError gdb_server_error, unsigned int pos)
{
	if(verbose)
	{
		logger << DebugInfo << "remote serial protocol error: " << gdb_server_error << " at character #" << pos << EndDebugInfo;
	}
	
	std::stringstream sstr;
	sstr << "E" << std::hex;
	sstr.width(2);
	sstr.fill('0');
	sstr << (unsigned int) gdb_server_error;
	return PutReply(sstr.str());
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::OutputText(const char *s, int count)
{
	int i;
	uint8_t packet[1 + 2 * count + 1];
	uint8_t *p = packet;

	*p = 'O';
	p++;
	for(i = 0; i < count; i++, p += 2)
	{
		p[0] = Nibble2HexChar((uint8_t) s[i] >> 4);
		p[1] = Nibble2HexChar((uint8_t) s[i] & 0xf);
	}
	*p = 0;
	return PutPacket('$', (const char *) packet);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::SetCThread(long thread_id)
{
	if(!ThreadIdToProcessorNumber(thread_id, c_prc_num))
	{
		return PutErrorReply(GDB_SERVER_ERROR_INVALID_THREAD_ID);
	}
	
	return PutReply("OK");
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::SetGThread(long thread_id)
{
	Barrier();
	
	if(!ThreadIdToProcessorNumber(thread_id, g_prc_num))
	{
		return PutErrorReply(GDB_SERVER_ERROR_INVALID_THREAD_ID);
	}

	if(!debug_selecting_import->DebugSelect(g_prc_num))
	{
		return PutErrorReply(GDB_SERVER_ERROR_CANT_DEBUG_PROCESSOR);
	}

	return PutReply("OK");
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ReadRegisters()
{
	unsigned int reg_num;
	std::string packet;
	typename std::vector<GDBRegister *>::const_iterator gdb_reg_iter;

	for(reg_num = 0, gdb_reg_iter = gdb_registers.begin(); gdb_reg_iter != gdb_registers.end(); reg_num++, gdb_reg_iter++)
	{
		GDBRegister *gdb_reg = *gdb_reg_iter;
		if(gdb_reg) // there are some holes in the register map
		{
			std::string hex;
			gdb_reg->GetValue(g_prc_num, hex);
			if(verbose)
			{
				logger << DebugInfo << "prc #" << g_prc_num << " reg #" << reg_num << ":" << gdb_reg->GetName() << "=" << hex << EndDebugInfo;
			}
			packet += hex;
		}
	}
	return PutReply(packet);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::WriteRegisters(const std::string& hex)
{
	bool write_error = false;
	typename std::vector<GDBRegister *>::iterator gdb_reg_iter;
	unsigned int pos = 0;

	for(gdb_reg_iter = gdb_registers.begin(); gdb_reg_iter != gdb_registers.end(); gdb_reg_iter++)
	{
		GDBRegister *gdb_reg = *gdb_reg_iter;
		if(gdb_reg) // there are some holes in the register map
		{
			if(!gdb_reg->SetValue(g_prc_num, hex.substr(pos, gdb_reg->GetHexLength())))
			{
				write_error = true;
				break;
			}
			pos += gdb_reg->GetHexLength();
		}
	}

	return write_error ? PutErrorReply(GDB_SERVER_ERROR_CANT_WRITE_REGISTER) : PutReply("OK");
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ReadRegister(unsigned int regnum)
{
	const GDBRegister *gdb_reg = (regnum < gdb_registers.size()) ? gdb_registers[regnum] : 0;
	if(gdb_reg)
	{
		std::string packet;
		if(gdb_reg->GetValue(g_prc_num,packet))
		{
			return PutReply(packet);
		}
		else
		{
			logger << DebugError << "Failed to read Register #" << regnum << EndDebugError;
			return PutErrorReply(GDB_SERVER_ERROR_CANT_READ_REGISTER);
		}
	}

	logger << DebugError << "Register #" << regnum << " can't be read because it is unknown" << EndDebugError;
	return PutErrorReply(GDB_SERVER_ERROR_UNKNOWN_REGISTER);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::WriteRegister(unsigned int regnum, const std::string& hex)
{
	GDBRegister *gdb_reg = (regnum < gdb_registers.size()) ? gdb_registers[regnum] : 0;
	
	if(gdb_reg)
	{
		if(gdb_reg->SetValue(g_prc_num, hex))
		{
			return PutReply("OK");
		}
		else
		{
			logger << DebugError << "Failed to write Register #" << regnum << EndDebugError;
			return PutErrorReply(GDB_SERVER_ERROR_CANT_WRITE_REGISTER);
		}
	}
	
	logger << DebugError << "Register #" << regnum << " can't be written because it is unknown" << EndDebugError;
	return PutErrorReply(GDB_SERVER_ERROR_UNKNOWN_REGISTER);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ReadMemoryHex(ADDRESS addr, uint32_t size)
{
	bool read_error = false;
	bool overwrapping = false;
	std::string packet;
	char c;

	if(size > 0)
	{
		Barrier();
		
		do
		{
			uint8_t value = 0;
			if(!overwrapping)
			{
				if(!memory_import->ReadMemory(addr, &value, 1))
				{
					read_error = true;
					value = 0;
				}
			}
			c = Nibble2HexChar(value >> 4);
			packet.append(1, c);
			c = Nibble2HexChar(value & 0xf);
			packet.append(1, c);
			if((addr + 1) == 0) overwrapping = true;
		} while(++addr, --size);
	}

	if(read_error)
	{
		if(verbose)
		{
			logger << DebugWarning << memory_import.GetName() << "->ReadMemory has reported an error" << EndDebugWarning;
		}
	}

	return /*read_error ? PutErrorReply(GDB_SERVER_ERROR_CANT_READ_MEMORY) : PutReply(packet); */PutReply(packet);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::WriteMemoryHex(ADDRESS addr, const std::string& hex, uint32_t size)
{
	bool write_error = false;
	bool overwrapping = false;
	unsigned int pos = 0;
	int len = hex.length();

	if(size > 0 && len > 0)
	{
		Barrier();
		
		do
		{
			uint8_t value;
			value = (HexChar2Nibble(hex[pos]) << 4) | HexChar2Nibble(hex[pos + 1]);
			if(!overwrapping)
			{
				if(!memory_import->WriteMemory(addr, &value, 1))
				{
					write_error = true;
				}
			}
			if((addr + 1) == 0) overwrapping = true;
		} while(pos += 2, ++addr, (--size) && ((len -= 2) >= 0));
	}

	if(write_error)
	{
		if(verbose)
		{
			logger << DebugWarning << memory_import.GetName() << "->WriteMemory has reported an error" << EndDebugWarning;
		}
	}

	return write_error ? PutErrorReply(GDB_SERVER_ERROR_CANT_WRITE_MEMORY) :  PutReply("OK");
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ReadMemoryBin(ADDRESS addr, uint32_t size)
{
	bool read_error = false;
	bool overwrapping = false;
	std::string packet;

	if(size > 0)
	{
		Barrier();
		
		do
		{
			char value = 0;
			if(!overwrapping)
			{
				if(!memory_import->ReadMemory(addr, &value, 1))
				{
					read_error = true;
					value = 0;
				}
			}
			
			switch(value)
			{
				case '#':
				case '$':
				case '}':
				case '*':
					packet += '}';   // escape
					value = value ^ 0x20;
					break;
			}
			
			packet += value;
			
			if((addr + 1) == 0) overwrapping = true;
		} while(++addr, --size);
	}

	if(read_error)
	{
		if(verbose)
		{
			logger << DebugWarning << memory_import.GetName() << "->ReadMemory has reported an error" << EndDebugWarning;
		}
	}

	return /*read_error ? PutErrorReply(GDB_SERVER_ERROR_CANT_READ_MEMORY) : PutReply(packet); */PutReply(packet);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::WriteMemoryBin(ADDRESS addr, const std::string& bin, uint32_t size)
{
	bool write_error = false;
	bool malformed_binary_data = false;
	bool overwrapping = false;
	unsigned int repeat = 0;
	std::size_t pos = 0;
	std::size_t len = bin.length();

	if(size > 0 && len > 0)
	{
		uint8_t value = 0;
		char c = 0;
		
		do
		{
			if(repeat)
			{
				repeat--;
			}
			else
			{
				if(pos >= len)
				{
					malformed_binary_data = true;
					break;
				}
				c = bin[pos++];
		
				if(c == '}')
				{
					if(pos >= len)
					{
						malformed_binary_data = true;
						break;
					}
					c = bin[pos++];
					value = (uint8_t) c ^ 0x20;
				}
				else
				{
					value = c;
				}
			}
			
			if(!overwrapping)
			{
				if(!memory_import->WriteMemory(addr, &value, 1))
				{
					write_error = true;
				}
			}
		}
		while(++addr, (--size));
	}
	
	if(pos != len)
	{
		logger << DebugWarning << "X packet has extra data !" << EndDebugWarning;
	}
	
	return malformed_binary_data ? PutErrorReply(GDB_SERVER_ERROR_MALFORMED_BINARY_DATA) : (write_error ? PutErrorReply(GDB_SERVER_ERROR_CANT_WRITE_MEMORY) : PutReply("OK"));
}

template <class ADDRESS>
void GDBServer<ADDRESS>::KillFromThrdProcessCmd()
{
	if(unlikely(debug))
	{
		logger << DebugInfo << "GDB command processing thread: Kill" << EndDebugInfo;
	}
	Kill();
	session_terminated = true;
	wait_for_command_processing = false;
	Run();
}


template <class ADDRESS>
void GDBServer<ADDRESS>::KillFromSimulationRun()
{
	if(unlikely(debug))
	{
		logger << DebugInfo << "Simulation thread: Kill" << EndDebugInfo;
	}
	Kill();
	stop_process_cmd_thrd = true;
	session_terminated = true;
	UnblockCommandProcessing();
}

template <class ADDRESS>
void GDBServer<ADDRESS>::TriggerDebugYield()
{
	if(debug_yielding_request_import)
	{
		debug_yielding_request_import->DebugYieldRequest();
	}
}

template <class ADDRESS>
void GDBServer<ADDRESS>::Barrier()
{
	return; // FIXME: Currently disabled as it introduces more problems than it solves
	// In rare condition, at startup, server is interacting with GDB client, and simulation is still running.
	// before sending commands to simulation, we should ask for simulation to yield and make it gracefully wait command processing
	if(!process_cmd_cond)
	{
		wait_for_command_processing = true;
		TriggerDebugYield();
		WaitForSimulationRun();
	}
}

template <class ADDRESS>
void GDBServer<ADDRESS>::Interrupt()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << "/\\/\\/\\ GDB interrupt (Ctrl-C)" << EndDebugInfo;
	}
	unsigned int prc_num;
	for(prc_num = 0; prc_num < num_processors; prc_num++)
	{
		prc_trap[prc_num] = true;
	}
	trap = true;
	wait_for_command_processing = true;
	TriggerDebugYield();
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ListenFetch(unsigned int prc_num)
{
	if(listening_fetch[prc_num]) return true;
	
	unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = fetch_insn_events[prc_num];
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << "prc #" << prc_num << ": stepi on" << EndDebugInfo;
	}
	if(!debug_event_trigger_import->Listen(fetch_insn_event))
	{
		logger << DebugError << "Can't track fetched instructions for Processor #" << prc_num << EndDebugError;
		return false;
	}
	
	listening_fetch[prc_num] = true;
	
	return true;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::UnlistenFetch(unsigned int prc_num)
{
	if(!listening_fetch[prc_num]) return true;

	unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = fetch_insn_events[prc_num];
		
	if(unlikely(verbose))
	{
		logger << DebugInfo << "prc #" << prc_num << ": stepi off" << EndDebugInfo;
	}
	if(!debug_event_trigger_import->Unlisten(fetch_insn_event))
	{
		logger << DebugError << "Can't untrack fetched instructions for Processor #" << prc_num << EndDebugError;
		return false;
	}
	
	listening_fetch[prc_num] = false;

	return true;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ThreadIdToProcessorNumber(long thread_id, unsigned int& _prc_num) const
{
	if(thread_id >= 1)
	{
		unsigned int prc_num = thread_id - 1;
		
		if(prc_num < num_processors)
		{
			_prc_num = prc_num;
			return true;
		}
	}
	
	return false;
}

template <class ADDRESS>
long GDBServer<ADDRESS>::ProcessorNumberToThreadId(unsigned int prc_num) const
{
	return prc_num + 1;
}


template <class ADDRESS>
bool GDBServer<ADDRESS>::ReportProgramExit()
{
	return PutReply("W00");
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ReportSignal(unsigned int signum)
{
	unsigned int prc_num;
	std::string packet("T");
	
	packet += Nibble2HexChar((signum >> 4) & 0xf);
	packet += Nibble2HexChar(signum & 0xf);
	
	if(trap && (mode == GDB_MODE_MULTI_THREAD))
	{
		// we have a stop event
		for(prc_num = 0; prc_num < num_processors; prc_num++)
		{
			if(prc_trap[prc_num])
			{
				break; // pick first processor with a stop event
			}
		}
	}
	else
	{
		prc_num = c_prc_num;
	}
	
	if(mode == GDB_MODE_MULTI_THREAD)
	{
		packet += "thread:";
		
		std::stringstream thread_id_sstr;
		thread_id_sstr << std::hex;
		
		if(gdb_client_feature_multiprocess && enable_multiprocess_extension)
		{
			thread_id_sstr << "p" << PROCESS_ID << "." << ProcessorNumberToThreadId(prc_num);
		}
		else
		{
			thread_id_sstr << ProcessorNumberToThreadId(prc_num);
		}
		
		packet += thread_id_sstr.str();
		packet += ';';
		
		packet += "core:";
		
		std::stringstream core_id_sstr;
		core_id_sstr << std::hex << prc_num;
		
		packet += core_id_sstr.str();
		packet += ';';
	}
	
	if(gdb_pc)
	{
		std::stringstream sstr;
		std::string hex;
		unsigned int reg_num = gdb_pc->GetRegNum();
		gdb_pc->GetValue(prc_num, hex);
		sstr << std::hex << reg_num;
		packet += sstr.str();
		packet += ":";
		packet += hex;
		packet += ";";
	}

	unsigned int num_stop_events = stop_events.size();
	unsigned int i;
	for(i = 0; i < num_stop_events; i++)
	{
		const unisim::util::debug::Event<ADDRESS> *event = stop_events[i];
		
		typename unisim::util::debug::Event<ADDRESS>::Type event_type = event->GetType();
		
		if((unsigned int) event->GetProcessorNumber() == prc_num)
		{
			if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT))
			{
				const unisim::util::debug::Watchpoint<ADDRESS> *watchpoint = static_cast<const unisim::util::debug::Watchpoint<ADDRESS> *>(event);
				
				unisim::util::debug::MemoryAccessType mat = watchpoint->GetMemoryAccessType();
				
				switch(mat)
				{
					case unisim::util::debug::MAT_READ:
					case unisim::util::debug::MAT_WRITE:
					{
						switch(watchpoint->GetMemoryAccessType())
						{
							case unisim::util::debug::MAT_READ:
								packet += 'r';
								break;
							case unisim::util::debug::MAT_WRITE:
								packet += 'a';
								break;
							default:
								break;
						}
						
						packet += "watch:";
						
						std::stringstream sstr_watchpoint_address;
						sstr_watchpoint_address << std::hex << watchpoint->GetAddress();
						
						packet += sstr_watchpoint_address.str();
						
						packet += ';';
						break;
					}
						
					default:
						break;
				}
			}
		}
	}
	
	return PutReply(packet);
}

template <class ADDRESS>
void GDBServer<ADDRESS>::ClearStopEvents()
{
	trap = false;
	
	unsigned int prc_num;
	for(prc_num = 0; prc_num < num_processors; prc_num++)
	{
		prc_trap[prc_num] = false;
	}
	
	unsigned int num_stop_events = stop_events.size();
	unsigned int i;
	for(i = 0; i < num_stop_events; i++)
	{
		const unisim::util::debug::Event<ADDRESS> *event = stop_events[i];
		event->Release();
	}
	
	stop_events.clear();
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::SetBreakpointWatchpoint(uint32_t type, ADDRESS addr, uint32_t kind)
{
	Barrier();
	
	uint32_t size = 0;
	uint32_t i;
	unsigned int prc_num;
	bool status = true;
	unsigned int save_cur_prc_num = debug_selecting_import->DebugGetSelected();
	
	typename std::map<uint32_t, uint32_t>::const_iterator arch_specific_breakpoint_kind = arch_specific_breakpoint_kinds.find(kind);
	
	if(arch_specific_breakpoint_kind != arch_specific_breakpoint_kinds.end())
	{
		size = (*arch_specific_breakpoint_kind).second;
	}
	else
	{
		size = kind;
	}
	
	size *= memory_atom_size;
	
	switch(type)
	{
		case 0: // software breakpoint
		case 1: // hardware breakpoint
			for(i = 0; i < size; i++)
			{
				switch(mode)
				{
					case GDB_MODE_SINGLE_THREAD:
						if(!debug_event_trigger_import->SetBreakpoint(addr + i))
						{
							logger << DebugWarning << "Can't listen breakpoint event for address 0x" << std::hex << (addr + i) << std::dec << EndDebugWarning;
							status = false;
						}
						break;
						
					case GDB_MODE_MULTI_THREAD:
						for(prc_num = 0; prc_num < num_processors; prc_num++)
						{
							debug_selecting_import->DebugSelect(prc_num);
							if(!debug_event_trigger_import->SetBreakpoint(addr + i))
							{
								logger << DebugWarning << "Can't listen breakpoint event for address 0x" << std::hex << (addr + i) << std::dec << " and processor #" << prc_num << EndDebugWarning;
								status = false;
							}
						}
						break;
				}
			}
			break;
		
		case 2: // write watchpoint
			switch(mode)
			{
				case GDB_MODE_SINGLE_THREAD:
					if(!debug_event_trigger_import->SetWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size, false))
					{
						logger << DebugWarning << "Can't listen write watchpoint event for address range 0x" << std::hex << addr << "-0x" << (addr + size - 1) << std::dec << EndDebugWarning;
						status = false;
					}
					break;
				
				case GDB_MODE_MULTI_THREAD:
					for(prc_num = 0; prc_num < num_processors; prc_num++)
					{
						debug_selecting_import->DebugSelect(prc_num);
						if(!debug_event_trigger_import->SetWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size, false))
						{
							logger << DebugWarning << "Can't listen write watchpoint event for address range 0x" << std::hex << addr << "-0x" << (addr + size - 1) << std::dec << " and Processor #" << prc_num << EndDebugWarning;
							status = false;
						}
					}
					break;
			}
			break;
		
		case 3: // read watchpoint
			switch(mode)
			{
				case GDB_MODE_SINGLE_THREAD:
					if(!debug_event_trigger_import->SetWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size, false))
					{
						logger << DebugWarning << "Can't listen read watchpoint event for address range 0x" << std::hex << addr << "-0x" << (addr + size - 1) << std::dec << EndDebugWarning;
						status = false;
					}
					break;
				
				case GDB_MODE_MULTI_THREAD:
					for(prc_num = 0; prc_num < num_processors; prc_num++)
					{
						debug_selecting_import->DebugSelect(prc_num);
						if(!debug_event_trigger_import->SetWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size, false))
						{
							logger << DebugWarning << "Can't listen read watchpoint event for address range 0x" << std::hex << addr << "-0x" << (addr + size - 1) << std::dec << " and Processor #" << prc_num << EndDebugWarning;
							status = false;
						}
					}
					break;
			}
			break;

		case 4: // access watchpoint
			switch(mode)
			{
				case GDB_MODE_SINGLE_THREAD:
					if(!debug_event_trigger_import->SetWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size, false))
					{
						logger << DebugWarning << "Can't listen read watchpoint event for address range 0x" << std::hex << addr << "-0x" << (addr + size - 1) << std::dec << EndDebugWarning;
						status = false;
					}
					
					if(!debug_event_trigger_import->SetWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size, false))
					{
						logger << DebugWarning << "Can't listen write watchpoint event for address range 0x" << std::hex << addr << "-0x" << (addr + size - 1) << std::dec << EndDebugWarning;
						status = false;
					}
					break;
				
				case GDB_MODE_MULTI_THREAD:
					for(prc_num = 0; prc_num < num_processors; prc_num++)
					{
						debug_selecting_import->DebugSelect(prc_num);
						
						if(!debug_event_trigger_import->SetWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size, false))
						{
							logger << DebugWarning << "Can't listen read watchpoint event for address range 0x" << std::hex << addr << "-0x" << (addr + size - 1) << std::dec << " and Processor #" << prc_num << EndDebugWarning;
							status = false;
						}
						
						if(!debug_event_trigger_import->SetWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size, false))
						{
							logger << DebugWarning << "Can't listen write watchpoint event for address range 0x" << std::hex << addr << "-0x" << (addr + size - 1) << std::dec << " and Processor #" << prc_num << EndDebugWarning;
							status = false;
						}
					}
					break;
			}
			break;
	}
	
	debug_selecting_import->DebugSelect(save_cur_prc_num);
	
	return status ? PutReply("OK") : PutErrorReply(GDB_SERVER_CANT_SET_BREAKPOINT_WATCHPOINT);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::RemoveBreakpointWatchpoint(uint32_t type, ADDRESS addr, uint32_t kind)
{
	Barrier();
	
	uint32_t size = 0;
	uint32_t i;
	unsigned int prc_num;
	bool status = true;
	unsigned int save_cur_prc_num = debug_selecting_import->DebugGetSelected();
	
	typename std::map<uint32_t, uint32_t>::const_iterator arch_specific_breakpoint_kind = arch_specific_breakpoint_kinds.find(kind);
	
	if(arch_specific_breakpoint_kind != arch_specific_breakpoint_kinds.end())
	{
		size = (*arch_specific_breakpoint_kind).second;
	}
	else
	{
		size = kind;
	}
	
	size *= memory_atom_size;
	
	switch(type)
	{
		case 0: // software breakpoint
		case 1: // hardware breakpoint
			for(i = 0; i < size; i++)
			{
				switch(mode)
				{
					case GDB_MODE_SINGLE_THREAD:
						if(!debug_event_trigger_import->RemoveBreakpoint(addr + i))
						{
							logger << DebugWarning << "Can't listen breakpoint event for address 0x" << std::hex << (addr + i) << std::dec << EndDebugWarning;
							status = false;
						}
						break;
						
					case GDB_MODE_MULTI_THREAD:
						for(prc_num = 0; prc_num < num_processors; prc_num++)
						{
							debug_selecting_import->DebugSelect(prc_num);
							if(!debug_event_trigger_import->RemoveBreakpoint(addr + i))
							{
								logger << DebugWarning << "Can't listen breakpoint event for address 0x" << std::hex << (addr + i) << std::dec << " and processor #" << prc_num << EndDebugWarning;
								status = false;
							}
						}
						break;
				}
			}
			break;
		
		case 2: // write watchpoint
			switch(mode)
			{
				case GDB_MODE_SINGLE_THREAD:
					if(!debug_event_trigger_import->RemoveWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size))
					{
						logger << DebugWarning << "Can't listen write watchpoint event for address range 0x" << std::hex << addr << "-0x" << (addr + size - 1) << std::dec << EndDebugWarning;
						status = false;
					}
					break;
				
				case GDB_MODE_MULTI_THREAD:
					for(prc_num = 0; prc_num < num_processors; prc_num++)
					{
						debug_selecting_import->DebugSelect(prc_num);
						if(!debug_event_trigger_import->RemoveWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size))
						{
							logger << DebugWarning << "Can't listen write watchpoint event for address range 0x" << std::hex << addr << "-0x" << (addr + size - 1) << std::dec << " and Processor #" << prc_num << EndDebugWarning;
							status = false;
						}
					}
					break;
			}
			break;
		
		case 3: // read watchpoint
			switch(mode)
			{
				case GDB_MODE_SINGLE_THREAD:
					if(!debug_event_trigger_import->RemoveWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size))
					{
						logger << DebugWarning << "Can't listen read watchpoint event for address range 0x" << std::hex << addr << "-0x" << (addr + size - 1) << std::dec << EndDebugWarning;
						status = false;
					}
					break;
				
				case GDB_MODE_MULTI_THREAD:
					for(prc_num = 0; prc_num < num_processors; prc_num++)
					{
						debug_selecting_import->DebugSelect(prc_num);
						if(!debug_event_trigger_import->RemoveWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size))
						{
							logger << DebugWarning << "Can't listen read watchpoint event for address range 0x" << std::hex << addr << "-0x" << (addr + size - 1) << std::dec << " and Processor #" << prc_num << EndDebugWarning;
							status = false;
						}
					}
					break;
			}
			break;

		case 4: // access watchpoint
			switch(mode)
			{
				case GDB_MODE_SINGLE_THREAD:
					if(!debug_event_trigger_import->RemoveWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size))
					{
						logger << DebugWarning << "Can't listen read watchpoint event for address range 0x" << std::hex << addr << "-0x" << (addr + size - 1) << std::dec << EndDebugWarning;
						status = false;
					}
					
					if(!debug_event_trigger_import->RemoveWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size))
					{
						logger << DebugWarning << "Can't listen write watchpoint event for address range 0x" << std::hex << addr << "-0x" << (addr + size - 1) << std::dec << EndDebugWarning;
						status = false;
					}
					break;
				
				case GDB_MODE_MULTI_THREAD:
					for(prc_num = 0; prc_num < num_processors; prc_num++)
					{
						debug_selecting_import->DebugSelect(prc_num);
						
						if(!debug_event_trigger_import->RemoveWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size))
						{
							logger << DebugWarning << "Can't listen read watchpoint event for address range 0x" << std::hex << addr << "-0x" << (addr + size - 1) << std::dec << " and Processor #" << prc_num << EndDebugWarning;
							status = false;
						}
						
						if(!debug_event_trigger_import->RemoveWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size))
						{
							logger << DebugWarning << "Can't listen write watchpoint event for address range 0x" << std::hex << addr << "-0x" << (addr + size - 1) << std::dec << " and Processor #" << prc_num << EndDebugWarning;
							status = false;
						}
					}
					break;
			}
			break;
	}
	
	debug_selecting_import->DebugSelect(save_cur_prc_num);
	
	return status ? PutReply("OK") : PutErrorReply(GDB_SERVER_CANT_REMOVE_BREAKPOINT_WATCHPOINT);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::HandleVCont(const std::string& query, std::size_t& pos)
{
	Barrier();

	std::vector<GDBServerAction> new_prc_actions(num_processors);
	unsigned int prc_num = 0;
	char ch = 0;
	unsigned int len = query.length();
	GDBServerAction action = GDB_SERVER_NO_ACTION;
	long thread_id;
	
	// For each inferior thread, the leftmost action with a matching thread-id is applied
	do
	{
		if(!ParseChar(query, pos, ch) || (ch != ';')) return PutErrorReply(GDB_SERVER_ERROR_EXPECTING_SEMICOLON, pos);
		if(!ParseChar(query, pos, ch) || ((ch != 'c') && (ch != 's'))) return PutErrorReply(GDB_SERVER_ERROR_EXPECTING_ACTION, pos);
		action =  (ch == 'c') ? GDB_SERVER_ACTION_CONTINUE :
		         ((ch == 's') ? GDB_SERVER_ACTION_STEP     : GDB_SERVER_NO_ACTION);
		
		thread_id = -1;
		if(ParseChar(query, pos, ch))
		{
			if(ch == ':')
			{
				if(!ParseThreadId(query, pos, thread_id)) return PutErrorReply(GDB_SERVER_ERROR_EXPECTING_THREAD_ID, pos);
			}
			else
			{
				pos--;
			} 
		}

		switch(mode)
		{
			case GDB_MODE_SINGLE_THREAD:
				if(new_prc_actions[c_prc_num] == GDB_SERVER_NO_ACTION)
				{
					new_prc_actions[c_prc_num] = action;
				}
				break;
				
			case GDB_MODE_MULTI_THREAD:
				if(thread_id == -1)
				{
					for(prc_num = 0; prc_num < num_processors; prc_num++)
					{
						if(new_prc_actions[prc_num] == GDB_SERVER_NO_ACTION)
						{
							new_prc_actions[prc_num] = action;
						}
					}
				}
				else if(thread_id == 0)
				{
					for(prc_num = 0; prc_num < num_processors; prc_num++)
					{
						if(new_prc_actions[prc_num] == GDB_SERVER_NO_ACTION)
						{
							new_prc_actions[prc_num] = action;
							break;
						}
					}
				}
				else
				{
					if(!ThreadIdToProcessorNumber(thread_id, prc_num)) return PutErrorReply(GDB_SERVER_ERROR_INVALID_THREAD_ID);
					new_prc_actions[prc_num] = action;
				}
				break;
		}
	}
	while(pos < len);

	// Threads that don't match any action remain in their current state
	switch(mode)
	{
		case GDB_MODE_SINGLE_THREAD:
			if(new_prc_actions[c_prc_num] != GDB_SERVER_NO_ACTION) prc_actions[c_prc_num] = new_prc_actions[c_prc_num];
			break;
			
		case GDB_MODE_MULTI_THREAD:
			for(prc_num = 0; prc_num < num_processors; prc_num++)
			{
				if(new_prc_actions[prc_num] != GDB_SERVER_NO_ACTION) prc_actions[prc_num] = new_prc_actions[prc_num];
			}
			break;
	}
	
	switch(mode)
	{
		case GDB_MODE_SINGLE_THREAD:
		{
			action = prc_actions[c_prc_num];
			if(unlikely(verbose))
			{
				logger << DebugInfo << "prc #" << c_prc_num << ": " << action << EndDebugInfo;
			}
			
			if(action == GDB_SERVER_ACTION_STEP)
			{
				ListenFetch(c_prc_num);
			}
			break;
		}
		
		case GDB_MODE_MULTI_THREAD:
		{
			// For threads stepping, listen for instruction fetch
			for(prc_num = 0; prc_num < num_processors; prc_num++)
			{
				action = prc_actions[prc_num];
				if(unlikely(verbose))
				{
					logger << DebugInfo << "prc #" << prc_num << ": " << action << EndDebugInfo;
				}
				
				if(action == GDB_SERVER_ACTION_STEP)
				{
					ListenFetch(prc_num);
				}
			}
			break;
		}
	}
	
	bool status = true;
	if(unlikely(debug))
	{
		logger << DebugInfo << "GDB command processing thread: Unlocking" << EndDebugInfo;
	}
	Unlock();
	do
	{
		Run();
		WaitForSimulationRun();
	}
	while(!trap && !Killed());
	if(unlikely(debug))
	{
		logger << DebugInfo << "GDB command processing thread: Locking" << EndDebugInfo;
	}
	Lock();
	if(!Killed())
	{
		switch(mode)
		{
			case GDB_MODE_SINGLE_THREAD:
			{
				action = prc_actions[c_prc_num];
				
				if(action == GDB_SERVER_ACTION_STEP)
				{
					UnlistenFetch(c_prc_num);
				}
				break;
			}
			
			case GDB_MODE_MULTI_THREAD:
			{
				// For threads stepping, unlisten instruction fetch
				for(prc_num = 0; prc_num < num_processors; prc_num++)
				{
					action = prc_actions[prc_num];
					
					if(action == GDB_SERVER_ACTION_STEP)
					{
						UnlistenFetch(prc_num);
					}
				}
				break;
			}
		}
		if(status) status = DisplayMonitoredInternals();
		if(status) status = ReportSignal(5 /* SIGTRAP */);
		if(trap) ClearStopEvents();
	}

	return status;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::HandleQRcmd(const std::string& query, std::size_t& pos)
{
	Barrier();

	std::size_t len = query.length();
	
	char c;
	std::string variable_name;
	std::string variable_value;
	// skip white characters
	do
	{
		c = (HexChar2Nibble(query[pos]) << 4) | HexChar2Nibble(query[pos + 1]);
		if(c != ' ') break;
		pos += 2;
	} while(pos < len);

	if(pos < len)
	{
		unisim::kernel::VariableBase *variable = 0;

		// fill-in parameter name
		do
		{
			c = (HexChar2Nibble(query[pos]) << 4) | HexChar2Nibble(query[pos + 1]);
			pos += 2;
			if(c == '=') break;
			variable_name.append(1, c);
		} while(pos < len);

		variable = Object::GetSimulator()->FindVariable(variable_name.c_str());
		if(variable->IsVoid())
		{
			std::string msg("unknown variable\n");
			if(!OutputText(msg.c_str(), msg.length())) return false;
			return PutReply("OK");
		}

		if(pos >= len)
		{
			// it's a get!
			std::string msg(variable_name + "=" + ((std::string) *variable) + "\n");
			if(!OutputText(msg.c_str(), msg.length())) return false;
			return PutReply("OK");
		}

		// fill-in parameter value and remove trailing space
		while(pos < len)
		{
			c = (HexChar2Nibble(query[pos]) << 4) | HexChar2Nibble(query[pos + 1]);
			if(c == ' ') break;
			pos += 2;
			variable_value.append(1, c);
		}

		std::string msg(variable_name + "<-" + variable_value + "\n");
		if(!OutputText(msg.c_str(), msg.length())) return false;
		*variable = variable_value.c_str();
		return PutReply("OK");
	}

	if(verbose)
	{
		logger << DebugInfo << "Unknown custom remote command" << EndDebugInfo;
	}
	return PutReply("");
}


template <class ADDRESS>
bool GDBServer<ADDRESS>::HandleQRegisterInfo(const std::string& query, std::size_t& pos)
{
	Barrier();

	unsigned int reg_order_num = 0;
	std::stringstream sstr(query.substr(pos));
	sstr >> std::hex;
	sstr >> reg_order_num;
	
	typename std::vector<GDBRegister *>::iterator gdb_reg_iter;
	unsigned int i;
	unsigned int offset = 0;

	for(gdb_reg_iter = gdb_registers.begin(), i = 0; gdb_reg_iter != gdb_registers.end(); gdb_reg_iter++, i++)
	{
		GDBRegister *gdb_reg = *gdb_reg_iter;
		if(gdb_reg) // there are some holes in the register map
		{
			if(i == reg_order_num)
			{
				std::stringstream packet_sstr;
				
				packet_sstr << "name:" << gdb_reg->GetName();
				packet_sstr << ";bitsize:" << std::dec << gdb_reg->GetBitSize();
				packet_sstr << ";offset:" << std::dec << offset;
				packet_sstr << ";encoding:";
				
				if(strncmp(gdb_reg->GetType(), "uint", 4) == 0)
				{
					packet_sstr << "uint;format:hex";
				}
				else if(strncmp(gdb_reg->GetType(), "int", 3) == 0)
				{
					packet_sstr << "sint;format:hex";
				}
				else if(strncmp(gdb_reg->GetType(), "ieee_", 5) == 0)
				{
					packet_sstr << "ieee754;format:float";
				}
				else if(strncmp(gdb_reg->GetType(), "vec", 3) == 0)
				{
					packet_sstr << "vector;format:hex";
				}
				else
				{
					packet_sstr << "uint;format:hex";
				}
				
				if(strlen(gdb_reg->GetGroup()) != 0)
				{
					packet_sstr << ";set:" << gdb_reg->GetGroup();
				}
				
				return PutReply(packet_sstr.str());
			}
			offset += gdb_reg->GetByteSize();
		}
	}
	
	return PutErrorReply(GDB_SERVER_ERROR_UNKNOWN_REGISTER);
}

template <class ADDRESS>
void GDBServer<ADDRESS>::SetGDBClientFeature(std::string gdb_client_feature)
{
	struct
	{
		const char *name;
		bool *flag;
	}
	features[] = {
		{ "multiprocess", &gdb_client_feature_multiprocess },
		{ "xmlRegisters", &gdb_client_feature_xmlregisters },
		{ "qRelocInsn", &gdb_client_feature_qrelocinsn },
		{ "swbreak", &gdb_client_feature_swbreak },
		{ "hwbreak", &gdb_client_feature_hwbreak },
		{ "fork-events", &gdb_client_feature_fork_events },
		{ "vfork-events", &gdb_client_feature_vfork_events },
		{ "exec-events", &gdb_client_feature_exec_events },
		{ "vContSupported", &gdb_client_feature_vcont },
		{ "t32extensions", &gdb_client_feature_vcont },
		{ "qXfer:features:read", &gdb_client_feature_qxfer_features_read },
		{ "qXfer:threads:read", &gdb_client_feature_qxfer_threads_read },
	};
	
	unsigned int n = sizeof(features) / sizeof(features[0]);
	unsigned int i;
	for(i = 0; i < n; i++)
	{
		std::size_t name_len = strlen(features[i].name);
		if((gdb_client_feature.length() == (name_len + 1)) && gdb_client_feature.compare(0, name_len, features[i].name) == 0)
		{
			if(gdb_client_feature[name_len] == '+')
			{
				if(verbose)
				{
					logger << DebugInfo << "GDB client supports Feature \"" << features[i].name << "\": yes" << EndDebugInfo;
				}
				*features[i].flag = true;
			}
			else if(gdb_client_feature[name_len] == '-')
			{
				if(verbose)
				{
					logger << DebugInfo << "GDB client supports Feature \"" << features[i].name << "\": no" << EndDebugInfo;
				}
				*features[i].flag = false;
			}
			return;
		}
	}
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::HandleQSupported(const std::string& query, std::size_t& pos)
{
	if((pos < query.length()) && (query[pos] == ':'))
	{
		pos++;
	
		std::size_t delim_pos = 0;
		
		do
		{
			delim_pos = query.find_first_of(';', pos);
			
			std::string gdb_client_feature((delim_pos != std::string::npos) ? query.substr(pos, delim_pos - pos) : query.substr(pos));
			
			SetGDBClientFeature(gdb_client_feature);
			
			pos = (delim_pos != std::string::npos) ? delim_pos + 1 : std::string::npos;
		}
		while(pos != std::string::npos);
	}
	
	std::stringstream sstr;
	sstr << "PacketSize=0fff;vContSupported+;QStartNoAckMode+;qXfer:features:read+";
	
	if(mode == GDB_MODE_MULTI_THREAD)
	{
		sstr << ";qXfer:threads:read+";
	}
	
	if(enable_multiprocess_extension)
	{
		sstr << ";multiprocess+";
	}
	return PutReply(sstr.str());
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::HandleQC()
{
	switch(mode)
	{
		case GDB_MODE_SINGLE_THREAD:
			return PutReply("QC0");
			
		case GDB_MODE_MULTI_THREAD:
		{
			std::stringstream sstr;
			sstr << "QC" << std::hex;
			if(gdb_client_feature_multiprocess && enable_multiprocess_extension)
			{
				sstr << "p" << PROCESS_ID << "." << ProcessorNumberToThreadId(g_prc_num);
			}
			else
			{
				sstr << ProcessorNumberToThreadId(g_prc_num);
			}
			return PutReply(sstr.str());
		}
	}
	
	return PutReply("");
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::HandleQTStatus()
{
	return PutReply("T0"); // there's no experiments running currently
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::HandleQAttached(const std::string& query, std::size_t& pos)
{
	if(mode == GDB_MODE_MULTI_THREAD)
	{
		if(ALWAYS_ACCEPT_MULTIPROCESS_NEW_THREAD_ID_SYNTAX || (gdb_client_feature_multiprocess && enable_multiprocess_extension))
		{
			if(MatchChar(query, pos, ':'))
			{
				long thread_id = 0;
				
				if(!ParseThreadId(query, pos, thread_id))
				{
					return PutErrorReply(GDB_SERVER_ERROR_EXPECTING_THREAD_ID, pos);
				}
				
				unsigned int prc_num = 0;
				if(!ThreadIdToProcessorNumber(thread_id, prc_num))
				{
					return PutErrorReply(GDB_SERVER_ERROR_INVALID_THREAD_ID);
				}
			}
		}
	}
	
	return PutReply("1"); // The remote server attached to an existing process
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::HandleQStartNoAckMode()
{
	no_ack_mode = true;
	return PutReply("OK");
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::HandleQXferFeaturesRead(const std::string& query, std::size_t& pos)
{
	std::size_t delim_pos = query.find_first_of(':', pos);
	
	if(delim_pos == std::string::npos)
	{
		logger << DebugWarning << "malformed qXfer:features:read request (expecting annex)" << EndDebugWarning;
		return PutErrorReply(GDB_SERVER_ERROR_EXPECTING_COLON, pos);
	}
	
	std::string annex(query.substr(pos, delim_pos - pos));
	
	if(verbose)
	{
		logger << DebugInfo << "GDB client wants annex \"" << annex << "\"" << std::endl;
	}
	
	pos = delim_pos + 1;
	delim_pos = query.find_first_of(',', pos);
	
	if(delim_pos == std::string::npos)
	{
		logger << DebugWarning << "malformed qXfer:features:read request (expecting offset)" << EndDebugWarning;
		return PutErrorReply(GDB_SERVER_ERROR_EXPECTING_COMMA, pos);
	}

	std::stringstream offset_sstr(query.substr(pos, delim_pos - pos));
	unsigned int offset;
	offset_sstr >> std::hex >> offset;

	if(verbose)
	{
		logger << DebugInfo << "GDB client wants annex from offset " << offset << std::endl;
	}

	pos = delim_pos + 1;

	std::stringstream length_sstr(query.substr(pos));
	unsigned int length;
	length_sstr >> std::hex >> length;
	
	if(verbose)
	{
		logger << DebugInfo << "GDB client wants at most " << length << " characters from annex" << std::endl;
	}

	std::stringstream sstr;
	
	sstr << "<?xml version=\"1.0\"?>";
	
	if(annex == "target.xml")
	{
		sstr << "<!DOCTYPE target SYSTEM \"gdb-target.dtd\">";
		sstr << "<target version=\"1.0\">" ;
		unsigned int n_features = gdb_features.size();
		unsigned int i;
		for(i = 0; i < n_features; i++)
		{
			GDBFeature *gdb_feature = gdb_features[i];
			sstr << "<xi:include href=\"feature" << gdb_feature->GetId() << ".xml\"/>";
		}
		sstr << "</target>";
	}
	else
	{
		unsigned int n_features = gdb_features.size();
		unsigned int i;
		for(i = 0; i < n_features; i++)
		{
			GDBFeature *gdb_feature = gdb_features[i];
			gdb_feature->ToXML(sstr, annex);
		}
	}
	
	std::string packet("l");
	packet.append(EscapeString(sstr.str()), offset, length);
	
	return PutReply(packet);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::HandleQXferThreadsRead(const std::string& query, std::size_t& pos)
{
	std::size_t delim_pos = query.find_first_of(',', pos);
	
	if(delim_pos == std::string::npos)
	{
		logger << DebugWarning << "malformed qXfer:threads:read request (expecting offset)" << EndDebugWarning;
		return PutErrorReply(GDB_SERVER_ERROR_EXPECTING_COMMA, pos);
	}

	std::stringstream offset_sstr(query.substr(pos, delim_pos - pos));
	unsigned int offset;
	offset_sstr >> std::hex >> offset;

	if(verbose)
	{
		logger << DebugInfo << "GDB client wants thread list from offset " << offset << std::endl;
	}

	pos = delim_pos + 1;

	std::stringstream length_sstr(query.substr(pos));
	unsigned int length;
	length_sstr >> std::hex >> length;
	
	if(verbose)
	{
		logger << DebugInfo << "GDB client wants at most " << length << " characters from thread list" << std::endl;
	}

	std::stringstream sstr;
	
	sstr << "<?xml version=\"1.0\"?>";
	sstr << "<threads>" << std::hex;
	unsigned int prc_num;
	for(prc_num = 0; prc_num < num_processors; prc_num++)
	{
		sstr << "<thread id=\"";
		if(gdb_client_feature_multiprocess && enable_multiprocess_extension)
		{
			sstr << "p" << PROCESS_ID << "." << ProcessorNumberToThreadId(prc_num);
		}
		else
		{
			sstr << ProcessorNumberToThreadId(prc_num);
		}
		
		sstr << "\" core=\"" << prc_num << "\" name=\"CPU #" << prc_num << "\"/>";
	}
	sstr << "</threads>";
	
	std::string packet("l");
	packet.append(EscapeString(sstr.str()), offset, length);
	
	return PutReply(packet);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::HandleQfThreadInfo()
{
	if(mode == GDB_MODE_MULTI_THREAD)
	{
		std::stringstream sstr;
		
		sstr << "m";
		
		unsigned int prc_num;
		for(prc_num = 0; prc_num < num_processors; prc_num++)
		{
			if(prc_num != 0) sstr << ",";
			sstr << std::hex;
			if(gdb_client_feature_multiprocess && enable_multiprocess_extension)
			{
				sstr << "p" << PROCESS_ID << "." << ProcessorNumberToThreadId(prc_num);
			}
			else
			{
				sstr << ProcessorNumberToThreadId(prc_num);
			}
		}
		
		return PutReply(sstr.str()); // report all threads
	}
	else
	{
		return PutReply("");
	}
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::HandleQsThreadInfo()
{
	return PutReply("l"); // report end of list
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::HandleQThreadExtraInfo(const std::string& query, std::size_t& pos)
{
	if(mode == GDB_MODE_MULTI_THREAD)
	{
		long thread_id = 0;
		if(ParseThreadId(query, pos, thread_id))
		{
			unsigned int prc_num = 0;
				
			if(ThreadIdToProcessorNumber(thread_id, prc_num))
			{
				std::stringstream sstr;
		
				sstr << "CPU #" << prc_num;
				
				return PutReply(HexEncodeString(sstr.str()));
			}
		}

		return PutReply(HexEncodeString("invalid thread ID"));
	}
	
	return PutReply("");
}

template <class ADDRESS>
std::string GDBServer<ADDRESS>::EscapeString(const std::string& s) const
{
	std::string r;
	std::size_t pos;
	std::size_t len = s.length();
	
	for(pos = 0; pos < len; pos++)
	{
		char c = s[pos];
		switch(c)
		{
			case '#':
			case '$':
			case '}':
			case '*':
				r.append(1, '}');   // escape
				c = c ^ 0x20;
				break;
		}
		
		r.append(1, c);
	}
	
	return r;
}

template <class ADDRESS>
std::string GDBServer<ADDRESS>::HexEncodeString(const std::string& s) const
{
	std::string result;
	std::size_t pos = 0;
	std::size_t len = s.length();
	
	for(pos = 0; pos < len; pos++)
	{
		char c = s[pos];
		result.append(1, Nibble2HexChar(c >> 4));
		result.append(1, Nibble2HexChar(c & 0xf));
	}
	
	return result;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::DisplayMonitoredInternals()
{
	std::string variable_name;
	std::stringstream sstr(monitor_internals);

	while(sstr >> variable_name)
	{
		unisim::kernel::VariableBase *variable = Object::GetSimulator()->FindVariable(variable_name.c_str());
		
		if(!variable->IsVoid())
		{
			std::string msg(variable_name + "=" + ((std::string) *variable));
			if(!OutputText(msg.c_str(), msg.length())) return false;
		}
	}
	
	return true;
}

} // end of namespace gdb_server
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
