/*
 *  Copyright (c) 2009-2010,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr), Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <getopt.h>
#include <stdlib.h>
#include <signal.h>
#include <stdexcept>

#include <unisim/kernel/kernel.hh>
#include <unisim/component/cxx/processor/tms320c3x/config.hh>
#include <unisim/component/cxx/processor/tms320c3x/cpu.hh>
#include <unisim/component/cxx/memory/ram/memory.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/loader/coff_loader/coff_loader.hh>
#include <unisim/service/os/ti_c_io/ti_c_io.hh>
#include <unisim/service/loader/multiformat_loader/multiformat_loader.hh>

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#endif



using namespace std;
using unisim::kernel::VariableBase;
using unisim::kernel::variable::Parameter;
using unisim::kernel::Object;


class Simulator : public unisim::kernel::Simulator
{
public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	virtual unisim::kernel::Simulator::SetupStatus Setup();
	void Run();
	virtual void Stop(Object *object, int _exit_status, bool asynchronous = false);
	int GetExitStatus() const;
protected:
private:

	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================

#ifdef DEBUG_TMS320C3X
	static const bool CPU_DEBUG = true;
#else
	static const bool CPU_DEBUG = false;
#endif

	typedef unisim::component::cxx::processor::tms320c3x::TMS320VC33_Config CPU_CONFIG;
	
	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================
	
	typedef unisim::component::cxx::processor::tms320c3x::CPU<CPU_CONFIG, CPU_DEBUG> CPU;
	typedef unisim::service::loader::multiformat_loader::MultiFormatLoader<CPU_CONFIG::address_t> LOADER;
	typedef unisim::component::cxx::memory::ram::Memory<CPU_CONFIG::address_t> MEMORY;
	
	struct DEBUGGER_CONFIG
	{
		typedef CPU_CONFIG::address_t ADDRESS;
		static const unsigned int NUM_PROCESSORS = 1;
		/* gdb_server, inline_debugger and/or monitor */
		static const unsigned int MAX_FRONT_ENDS = 3;
	};
	
	typedef unisim::service::debug::debugger::Debugger<DEBUGGER_CONFIG> DEBUGGER;
	typedef unisim::service::debug::gdb_server::GDBServer<CPU_CONFIG::address_t> GDB_SERVER;
	typedef unisim::service::debug::inline_debugger::InlineDebugger<CPU_CONFIG::address_t> INLINE_DEBUGGER;
	typedef unisim::service::time::host_time::HostTime HOST_TIME;
	typedef unisim::service::os::ti_c_io::TI_C_IO<CPU_CONFIG::address_t> TI_C_IO;

	//=========================================================================
	//===                             Components                            ===
	//=========================================================================
	CPU *cpu;
	MEMORY *memory;

	//=========================================================================
	//===                              Services                             ===
	//=========================================================================
	HOST_TIME *host_time;
	LOADER *loader;
	TI_C_IO *ti_c_io;
	DEBUGGER *debugger;
	GDB_SERVER *gdb_server;
	INLINE_DEBUGGER *inline_debugger;
	
	bool enable_gdb_server;
	bool enable_inline_debugger;
	Parameter<bool> param_enable_gdb_server;
	Parameter<bool> param_enable_inline_debugger;

	int exit_status;
	bool simulating;
	static void LoadBuiltInConfig(unisim::kernel::Simulator *simulator);
};

void SigIntHandler(int signum)
{
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	unisim::kernel::Simulator::Instance()->Stop(0, 0, true);
}

Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, memory(0)
	, host_time(0)
	, loader(0)
	, ti_c_io(0)
	, debugger(0)
	, gdb_server(0)
	, inline_debugger(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, exit_status(0)
	, simulating(false)
{
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	cpu = new CPU("cpu"); 
	memory = new MEMORY("memory");

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================

	VariableBase *program = FindVariable("cmd-args[0]");
	std::string program_name = std::string(*program);
	if(!program_name.empty())
		{
			SetVariable("loader.filename", (program_name + ",c31boot.out").c_str());
		}

	//  - Host Time
	host_time = new HOST_TIME("host-time");
	//  - GDB server
	gdb_server = enable_gdb_server ? new GDB_SERVER("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = enable_inline_debugger ? new INLINE_DEBUGGER("inline-debugger") : 0;
	//  - Multiformat Loader
	loader = new LOADER("loader");
	//  - TI C I/O
	ti_c_io = new TI_C_IO("ti-c-io");
	//  - Debugger
	debugger = (enable_gdb_server || enable_inline_debugger) ? new DEBUGGER("debugger") : 0;

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> memory->memory_export;
	cpu->ti_c_io_import >> ti_c_io->ti_c_io_export;
	ti_c_io->memory_import >> cpu->memory_export;
	ti_c_io->memory_injection_import >> cpu->memory_injection_export;
	ti_c_io->registers_import >> cpu->registers_export;
	ti_c_io->symbol_table_lookup_import >> loader->symbol_table_lookup_export;
	ti_c_io->blob_import >> loader->blob_export;
	*loader->memory_import[0] >> memory->memory_export;
	// cpu->symbol_table_lookup_import >> ?->symbol_table_lookup_export;
	
	if (enable_inline_debugger or enable_gdb_server)
	{
		// Debugger <-> CPU connections
		cpu->debug_yielding_import                           >> *debugger->debug_yielding_export[0];
		cpu->trap_reporting_import                           >> *debugger->trap_reporting_export[0];
		cpu->memory_access_reporting_import                  >> *debugger->memory_access_reporting_export[0];
		*debugger->disasm_import[0]                          >> cpu->disasm_export;
		*debugger->memory_import[0]                          >> cpu->memory_export;
		*debugger->registers_import[0]                       >> cpu->registers_export;
		*debugger->memory_access_reporting_control_import[0] >> cpu->memory_access_reporting_control_export;
		
		// Debugger <-> Loader connections
		debugger->blob_import >> loader->blob_export;
	
		if (enable_inline_debugger)
		{
			// inline-debugger <-> debugger connections
			*debugger->debug_event_listener_import[0]      >> inline_debugger->debug_event_listener_export;
			*debugger->debug_yielding_import[0]            >> inline_debugger->debug_yielding_export;
			inline_debugger->debug_yielding_request_import >> *debugger->debug_yielding_request_export[0];
			inline_debugger->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[0];
			inline_debugger->disasm_import                 >> *debugger->disasm_export[0];
			inline_debugger->memory_import                 >> *debugger->memory_export[0];
			inline_debugger->registers_import              >> *debugger->registers_export[0];
			inline_debugger->stmt_lookup_import            >> *debugger->stmt_lookup_export[0];
			inline_debugger->symbol_table_lookup_import    >> *debugger->symbol_table_lookup_export[0];
			inline_debugger->backtrace_import              >> *debugger->backtrace_export[0];
			inline_debugger->debug_info_loading_import     >> *debugger->debug_info_loading_export[0];
			inline_debugger->data_object_lookup_import     >> *debugger->data_object_lookup_export[0];
		}
		
		if (enable_gdb_server)
		{
			// Connect gdb-server to debugger
			*debugger->debug_yielding_import[1]        >> gdb_server->debug_yielding_export;
			*debugger->debug_event_listener_import[1]  >> gdb_server->debug_event_listener_export;
			gdb_server->debug_yielding_request_import  >> *debugger->debug_yielding_request_export[1];
			gdb_server->debug_event_trigger_import     >> *debugger->debug_event_trigger_export[1];
			gdb_server->memory_import                  >> *debugger->memory_export[1];
			gdb_server->registers_import               >> *debugger->registers_export[1];
		}
	}
}

Simulator::~Simulator()
{
	if(loader) delete loader;
	if(memory) delete memory;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(debugger) delete debugger;
	if(cpu) delete cpu;
	if(host_time) delete host_time;
	if(ti_c_io) delete ti_c_io;
}
	
void Simulator::LoadBuiltInConfig(unisim::kernel::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM tms320c3x");
	simulator->SetVariable("copyright", "Copyright (C) 2009-2013, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM tms320c3x, a TMS320C3X DSP simulator with support of TI COFF binaries, and TI C I/O (RTS run-time)");
	
	// - Loader
	simulator->SetVariable("loader.filename", "c31boot.out");

	// - CPU
	simulator->SetVariable("cpu.mimic-dev-board", "true");

	//  - RAM
	simulator->SetVariable("memory.org", 0x00000000UL);
	simulator->SetVariable("memory.bytesize", (uint32_t) -1);
	
	// - Loader memory router
	simulator->SetVariable("loader.memory-mapper.mapping", "memory=memory:0x0-0xffffffff");

	// - TI C I/O
	simulator->SetVariable("ti-c-io.enable", "true");
	simulator->SetVariable("ti-c-io.pc-register-name", "PC");

	//  - Debugger run-time configuration
	simulator->SetVariable("debugger.parse-dwarf", false);

	//  - Inline debugger
	simulator->SetVariable("inline-debugger.memory-atom-size", 4);
	
	//  - GDB server
	simulator->SetVariable("gdb-server.architecture-description-filename", "gdb_tms320c3x.xml");
	simulator->SetVariable("gdb-server.memory-atom-size", 4);
}

void Simulator::Run()
{
	double time_start = host_time->GetTime();

	try
	{
		simulating = true;
		do
		{
			cpu->StepInstruction();
		} while(simulating);
	}
	catch(std::runtime_error& e)
	{
		cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << endl;
		cerr << e.what() << endl;
	}

	cerr << "Simulation finished" << endl;
	
	double time_stop = host_time->GetTime();
	double spent_time = time_stop - time_start;

	cerr << "Simulation run-time parameters:" << endl;
	DumpParameters(cerr);
	cerr << endl;
	cerr << "Simulation formulas:" << endl;
	DumpFormulas(cerr);
	cerr << endl;
	cerr << "Simulation statistics:" << endl;
	DumpStatistics(cerr);
	cerr << endl;

	cerr << "simulation time: " << spent_time << " seconds" << endl;
	cerr << "host simulation speed: " << ((double) (*cpu)["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << endl;

}

unisim::kernel::Simulator::SetupStatus Simulator::Setup()
{
	if(enable_inline_debugger)
	{
		SetVariable("debugger.parse-dwarf", true);
	}
	
	if(enable_gdb_server)
	{
		SetVariable("cpu.trap-on-trap-instruction", 0x7400003f); // TRAP 0x1f
	}
	
	unisim::kernel::Simulator::SetupStatus setup_status = unisim::kernel::Simulator::Setup();

	return setup_status;
}

void Simulator::Stop(Object *object, int _exit_status, bool _asynchronous)
{
	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}
#ifdef DEBUG_TMS320C3X
	std::cerr << "Call stack:" << std::endl;
	std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
#endif
	std::cerr << "Program exited with status " << exit_status << std::endl;
	simulating = false;
}

int Simulator::GetExitStatus() const
{
	return exit_status;
}

int main(int argc, char *argv[])
{
#ifdef WIN32
	// Loads the winsock2 dll
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	WSADATA wsaData;
	if(WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		cerr << "WSAStartup failed" << endl;
		return -1;
	}
#endif
	Simulator *simulator = new Simulator(argc, argv);

	switch(simulator->Setup())
	{
		case unisim::kernel::Simulator::ST_OK_DONT_START:
			break;
		case unisim::kernel::Simulator::ST_WARNING:
			cerr << "Some warnings occurred during setup" << endl;
		case unisim::kernel::Simulator::ST_OK_TO_START:
			cerr << "Starting simulation" << endl;
			simulator->Run();
			break;
		case unisim::kernel::Simulator::ST_ERROR:
			cerr << "Can't start simulation because of previous errors" << endl;
			break;
	}

	int exit_status = simulator->GetExitStatus();

	if(simulator) delete simulator;
#ifdef WIN32
	// releases the winsock2 resources
	WSACleanup();
#endif

	return exit_status;
}
