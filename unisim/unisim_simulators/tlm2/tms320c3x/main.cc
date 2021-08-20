/*
 *  Copyright (c) 2009-2013,
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

#include <unisim/component/cxx/processor/tms320c3x/config.hh>
#include <unisim/component/tlm2/processor/tms320c3x/cpu.hh>
#include <unisim/service/os/ti_c_io/ti_c_io.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/debugger/debugger.tcc>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/debug/profiler/profiler.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/loader/multiformat_loader/multiformat_loader.hh>
#include <unisim/service/http_server/http_server.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/console/console_printer.hh>
#include <unisim/kernel/logger/text_file/text_file_writer.hh>
#include <unisim/kernel/logger/http/http_writer.hh>
#include <unisim/kernel/logger/xml_file/xml_file_writer.hh>
#include <unisim/kernel/logger/netstream/netstream_writer.hh>
#include <unisim/kernel/config/xml/xml_config_file_helper.hh>
#include <unisim/kernel/config/ini/ini_config_file_helper.hh>
#include <unisim/kernel/config/json/json_config_file_helper.hh>

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Variable;
using unisim::kernel::VariableBase;
using unisim::kernel::Object;

class Simulator : public unisim::kernel::Simulator
{
public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	void Run();
	virtual unisim::kernel::Simulator::SetupStatus Setup();
	virtual void Stop(Object *object, int exit_status, bool asynchronous = false);
	int GetExitStatus() const;
protected:
private:

	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================

#ifdef DEBUG_TMS320C3X
	static const bool CPU_DEBUG = true;
	static const bool MEMORY_DEBUG = true;
#else
	static const bool CPU_DEBUG = false;
	static const bool MEMORY_DEBUG = false;
#endif

	typedef unisim::component::cxx::processor::tms320c3x::TMS320VC33_Config CPU_CONFIG;
	
	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm2::processor::tms320c3x::CPU<CPU_CONFIG, CPU_DEBUG> CPU;
	typedef unisim::kernel::tlm2::InitiatorStub<0, unisim::component::tlm2::interrupt::InterruptProtocolTypes> IRQ_STUB;
	typedef unisim::service::loader::multiformat_loader::MultiFormatLoader<CPU_CONFIG::address_t> LOADER;
	typedef unisim::component::tlm2::memory::ram::Memory<32, CPU_CONFIG::address_t, 1, 1024 * 1024, MEMORY_DEBUG> MEMORY;
	
	struct DEBUGGER_CONFIG
	{
		typedef CPU_CONFIG::address_t ADDRESS;
		static const unsigned int NUM_PROCESSORS = 1;
		/* gdb_server, inline_debugger, profiler */
		static const unsigned int MAX_FRONT_ENDS = 3;
	};
	
	typedef unisim::service::debug::debugger::Debugger<DEBUGGER_CONFIG> DEBUGGER;
	typedef unisim::service::debug::gdb_server::GDBServer<CPU_CONFIG::address_t> GDB_SERVER;
	typedef unisim::service::debug::inline_debugger::InlineDebugger<CPU_CONFIG::address_t> INLINE_DEBUGGER;
	typedef unisim::service::debug::profiler::Profiler<CPU_CONFIG::address_t> PROFILER;
	typedef unisim::service::time::sc_time::ScTime SC_TIME;
	typedef unisim::service::time::host_time::HostTime HOST_TIME;
	typedef unisim::service::os::ti_c_io::TI_C_IO<CPU_CONFIG::address_t> TI_C_IO;
	typedef unisim::service::http_server::HttpServer HTTP_SERVER;
	typedef unisim::kernel::logger::console::Printer LOGGER_CONSOLE_PRINTER;
	typedef unisim::kernel::logger::text_file::Writer LOGGER_TEXT_FILE_WRITER;
	typedef unisim::kernel::logger::http::Writer LOGGER_HTTP_WRITER;
	typedef unisim::kernel::logger::xml_file::Writer LOGGER_XML_FILE_WRITER;
	typedef unisim::kernel::logger::netstream::Writer LOGGER_NETSTREAM_WRITER;

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - TMS320C3X processor
	CPU *cpu;
	//  - RAM
	MEMORY *memory;
	//  - IRQ Stubs
	IRQ_STUB *int0_stub;
	IRQ_STUB *int1_stub;
	IRQ_STUB *int2_stub;
	IRQ_STUB *int3_stub;
	IRQ_STUB *xint0_stub;
	IRQ_STUB *rint0_stub;
	IRQ_STUB *xint1_stub;
	IRQ_STUB *rint1_stub;
	IRQ_STUB *tint0_stub;
	IRQ_STUB *tint1_stub;
	IRQ_STUB *dint_stub;

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	// - Multiformat Loader
	LOADER *loader;
	// - TI C I/O
	TI_C_IO *ti_c_io;
	//  - GDB server
	GDB_SERVER *gdb_server;
	//  - Inline debugger
	INLINE_DEBUGGER *inline_debugger;
	//  - debugger
	DEBUGGER *debugger;
	//  - Profiler
	PROFILER *profiler;
	//  - SystemC Time
	SC_TIME *sim_time;
	//  - Host Time
	HOST_TIME *host_time;
	//  - Http Server
	HTTP_SERVER *http_server;
	//  - Logger Console Printer
	LOGGER_CONSOLE_PRINTER *logger_console_printer;
	//  - Logger Text File Writer
	LOGGER_TEXT_FILE_WRITER *logger_text_file_writer;
	//  - Logger HTTP Writer
	LOGGER_HTTP_WRITER *logger_http_writer;
	//  - Logger XML File Writer
	LOGGER_XML_FILE_WRITER *logger_xml_file_writer;
	//  - Logger TCP Network Stream Writer
	LOGGER_NETSTREAM_WRITER *logger_netstream_writer;

	bool enable_gdb_server;
	bool enable_inline_debugger;
	bool enable_profiler;
	Parameter<bool> param_enable_gdb_server;
	Parameter<bool> param_enable_inline_debugger;
	Parameter<bool> param_enable_profiler;

	int exit_status;
	static void LoadBuiltInConfig(unisim::kernel::Simulator *simulator);
	virtual void SigInt();
};



Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, memory(0)
	, int0_stub(0)
	, int1_stub(0)
	, int2_stub(0)
	, int3_stub(0)
	, xint0_stub(0)
	, rint0_stub(0)
	, xint1_stub(0)
	, rint1_stub(0)
	, tint0_stub(0)
	, tint1_stub(0)
	, dint_stub(0)
	, loader(0)
	, ti_c_io(0)
	, gdb_server(0)
	, inline_debugger(0)
	, debugger(0)
	, profiler(0)
	, sim_time(0)
	, host_time(0)
	, http_server(0)
	, logger_console_printer(0)
	, logger_text_file_writer(0)
	, logger_http_writer(0)
	, logger_xml_file_writer(0)
	, logger_netstream_writer(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, enable_profiler(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, param_enable_profiler("enable-profiler", 0, enable_profiler, "Enable/Disable profiler")
	, exit_status(0)
{
	//=========================================================================
	//===                 Logger Printers instantiations                    ===
	//=========================================================================

	logger_console_printer = new LOGGER_CONSOLE_PRINTER();
	logger_text_file_writer = new LOGGER_TEXT_FILE_WRITER();
	logger_http_writer = new LOGGER_HTTP_WRITER();
	logger_xml_file_writer = new LOGGER_XML_FILE_WRITER();
	logger_netstream_writer = new LOGGER_NETSTREAM_WRITER();
	
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	cpu = new CPU("cpu");
	//  - RAM
	memory = new MEMORY("memory");
	//  - IRQ Stubs
	int0_stub = new IRQ_STUB("int0-stub");
	int1_stub = new IRQ_STUB("int1-stub");
	int2_stub = new IRQ_STUB("int2-stub");
	int3_stub = new IRQ_STUB("int3-stub");
	xint0_stub = new IRQ_STUB("xint0-stub");
	rint0_stub = new IRQ_STUB("rint0-stub");
	xint1_stub = new IRQ_STUB("xint1-stub");
	rint1_stub = new IRQ_STUB("rint1-stub");
	tint0_stub = new IRQ_STUB("tint0-stub");
	tint1_stub = new IRQ_STUB("tint1-stub");
	dint_stub = new IRQ_STUB("dint-stub");

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================

	VariableBase *program = FindVariable("cmd-args[0]");
	std::string program_name = std::string(*program);
	if(!program_name.empty())
	{
		SetVariable("loader.filename", (program_name + ",c31boot.out").c_str());
	}

	//  - Debugger
	debugger = (enable_gdb_server or enable_inline_debugger or enable_profiler) ? new DEBUGGER("debugger") : 0;
	//  - GDB server
	gdb_server = enable_gdb_server ? new GDB_SERVER("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = enable_inline_debugger ? new INLINE_DEBUGGER("inline-debugger") : 0;
	//  - Profiler
	profiler = enable_profiler ? new PROFILER("profiler") : 0;
	//  - Multiformat Loader
	loader = new LOADER("loader");
	//  - TI C I/O
	ti_c_io = new TI_C_IO("ti-c-io");
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - Http Server
	http_server = new HTTP_SERVER("http-server");

	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	cpu->bus_master_sock(memory->slave_sock); // CPU <-> RAM
	int0_stub->master_sock(cpu->int0_slave_sock); // INT0 STUB <-> CPU
	int1_stub->master_sock(cpu->int1_slave_sock); // INT1 STUB <-> CPU
	int2_stub->master_sock(cpu->int2_slave_sock); // INT2 STUB <-> CPU
	int3_stub->master_sock(cpu->int3_slave_sock); // INT3 STUB <-> CPU
	xint0_stub->master_sock(cpu->xint0_slave_sock); // XINT0 STUB <-> CPU
	rint0_stub->master_sock(cpu->rint0_slave_sock); // RINT0 STUB <-> CPU
	xint1_stub->master_sock(cpu->xint1_slave_sock); // XINT1 STUB <-> CPU
	rint1_stub->master_sock(cpu->rint1_slave_sock); // RINT1 STUB <-> CPU
	tint0_stub->master_sock(cpu->tint0_slave_sock); // TINT0 STUB <-> CPU
	tint1_stub->master_sock(cpu->tint1_slave_sock); // TINT1 STUB <-> CPU
	dint_stub->master_sock(cpu->dint_slave_sock); // DINT STUB <-> CPU

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
	
	if (debugger)
	{
		// Debugger <-> CPU connections
		cpu->debug_yielding_import                            >> *debugger->debug_yielding_export[0];
		cpu->trap_reporting_import                            >> *debugger->trap_reporting_export[0];
		cpu->memory_access_reporting_import                   >> *debugger->memory_access_reporting_export[0];
		*debugger->disasm_import[0]                          >> cpu->disasm_export;
		*debugger->memory_import[0]                          >> cpu->memory_export;
		*debugger->registers_import[0]                       >> cpu->registers_export;
		*debugger->memory_access_reporting_control_import[0] >> cpu->memory_access_reporting_control_export;
		
		// Debugger <-> Loader connections
		debugger->blob_import >> loader->blob_export;

		if (inline_debugger)
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
			inline_debugger->subprogram_lookup_import      >> *debugger->subprogram_lookup_export[0];
		}
	
		if (gdb_server)
		{
			// gdb-server <-> debugger connections
			*debugger->debug_event_listener_import[1] >> gdb_server->debug_event_listener_export;
			*debugger->debug_yielding_import[1]       >> gdb_server->debug_yielding_export;
			gdb_server->debug_yielding_request_import >> *debugger->debug_yielding_request_export[1];
			gdb_server->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[1];
			gdb_server->memory_import                 >> *debugger->memory_export[1];
			gdb_server->registers_import              >> *debugger->registers_export[1];
		}
		
		if (profiler)
		{
			*debugger->debug_event_listener_import[2] >> profiler->debug_event_listener_export;
			*debugger->debug_yielding_import[2]       >> profiler->debug_yielding_export;
			profiler->debug_yielding_request_import   >> *debugger->debug_yielding_request_export[2];
			profiler->debug_event_trigger_import      >> *debugger->debug_event_trigger_export[2];
			profiler->disasm_import                   >> *debugger->disasm_export[2];
			profiler->memory_import                   >> *debugger->memory_export[2];
			profiler->registers_import                >> *debugger->registers_export[2];
			profiler->stmt_lookup_import              >> *debugger->stmt_lookup_export[2];
			profiler->symbol_table_lookup_import      >> *debugger->symbol_table_lookup_export[2];
			profiler->backtrace_import                >> *debugger->backtrace_export[2];
			profiler->debug_info_loading_import       >> *debugger->debug_info_loading_export[2];
			profiler->data_object_lookup_import       >> *debugger->data_object_lookup_export[2];
			profiler->subprogram_lookup_import        >> *debugger->subprogram_lookup_export[2];
		}
	}
	
	{
		unsigned int i = 0;
		*http_server->http_server_import[i++] >> logger_http_writer->http_server_export;
		if(profiler)
		{
			*http_server->http_server_import[i++] >> profiler->http_server_export;
		}
	}
	
	{
		unsigned int i = 0;
		*http_server->registers_import[i++] >> cpu->registers_export;
	}
}

Simulator::~Simulator()
{
	delete memory;
	delete gdb_server;
	delete inline_debugger;
	delete profiler;
	delete debugger;
	delete loader;
	delete ti_c_io;
	delete cpu;
	delete int0_stub;
	delete int1_stub;
	delete int2_stub;
	delete int3_stub;
	delete xint0_stub;
	delete rint0_stub;
	delete xint1_stub;
	delete rint1_stub;
	delete tint0_stub;
	delete tint1_stub;
	delete dint_stub;
	delete sim_time;
	delete host_time;
	delete http_server;
	delete logger_console_printer;
	delete logger_text_file_writer;
	delete logger_http_writer;
	delete logger_xml_file_writer;
	delete logger_netstream_writer;
}

void Simulator::LoadBuiltInConfig(unisim::kernel::Simulator *simulator)
{
	new unisim::kernel::config::xml::XMLConfigFileHelper(simulator);
	new unisim::kernel::config::ini::INIConfigFileHelper(simulator);
	new unisim::kernel::config::json::JSONConfigFileHelper(simulator);
	
	double cpu_frequency = 75.0; // in Mhz
	double cpu_ipc = 1.0; // in instructions per cycle
	double cpu_cycle_time = (double)(1.0e6 / cpu_frequency); // in picoseconds
	double mem_cycle_time = cpu_cycle_time;

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
	simulator->SetVariable("cpu.mimic-dev-board", true);
	simulator->SetVariable("cpu.ipc", cpu_ipc);
	simulator->SetVariable("cpu.cpu-cycle-time", sc_core::sc_time(cpu_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.nice-time", "1 us");
	simulator->SetVariable("cpu.enable-dmi", true);
	
	//  - RAM
	simulator->SetVariable("memory.cycle-time", sc_core::sc_time(mem_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("memory.read-latency", sc_core::sc_time(mem_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("memory.write-latency", sc_core::SC_ZERO_TIME.to_string().c_str());
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
	simulator->SetVariable("gdb-server.architecture-description-filename", "unisim/service/debug/gdb_server/gdb_tms320c3x.xml");
	simulator->SetVariable("gdb-server.memory-atom-size", 4);
	
	// HTTP server
	simulator->SetVariable("http-server.http-port", 12360);
}

void Simulator::Run()
{
	double time_start = host_time->GetTime();

	sc_core::sc_report_handler::set_actions(sc_core::SC_INFO, sc_core::SC_DO_NOTHING); // disable SystemC messages
	
	try
	{
		sc_core::sc_start();
	}
	catch(std::runtime_error& e)
	{
		std::cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << std::endl;
		std::cerr << e.what() << endl;
	}

	std::cerr << "Simulation finished" << std::endl;

	double time_stop = host_time->GetTime();
	double spent_time = time_stop - time_start;

	std::cerr << "Simulation run-time parameters:" << endl;
	DumpParameters(std::cerr);
	std::cerr << endl;
	
	std::cerr << "Simulation statistics:" << endl;
	DumpStatistics(std::cerr);
	std::cerr << endl;

	std::cerr << "simulation time: " << spent_time << " seconds" << std::endl;
	std::cerr << "simulated time : " << sc_core::sc_time_stamp().to_seconds() << " seconds (exactly " << sc_core::sc_time_stamp() << ")" << std::endl;
	std::cerr << "host simulation speed: " << ((double) (*cpu)["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << std::endl;
	std::cerr << "time dilatation: " << spent_time / sc_core::sc_time_stamp().to_seconds() << " times slower than target machine" << std::endl;
}

unisim::kernel::Simulator::SetupStatus Simulator::Setup()
{
	if(profiler)
	{
		http_server->AddJSAction(
		unisim::service::interfaces::ToolbarOpenTabAction(
			/* name */      profiler->GetName(), 
			/* label */     "<img src=\"/unisim/service/debug/profiler/icon_profile_cpu0.svg\" alt=\"Profile\">",
			/* tips */      std::string("Profile of ") + cpu->GetName(),
			/* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
			/* uri */       profiler->URI()
		));
	}
	
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

void Simulator::Stop(Object *object, int _exit_status, bool asynchronous)
{
	exit_status = _exit_status;
	if(sc_core::sc_get_status() != sc_core::SC_STOPPED)
	{
		sc_core::sc_stop();
	}
	if(!asynchronous)
	{
		sc_core::sc_process_handle h = sc_core::sc_get_current_process_handle();
		switch(h.proc_kind())
		{
			case sc_core::SC_THREAD_PROC_: 
			case sc_core::SC_CTHREAD_PROC_:
				sc_core::wait();
				break;
			default:
				break;
		}
	}
	unisim::kernel::Simulator::Kill();
}

int Simulator::GetExitStatus() const
{
	return exit_status;
}

void Simulator::SigInt()
{
	if(!enable_inline_debugger)
	{
		unisim::kernel::Simulator::Instance()->Stop(0, 0, true);
	}
}

int sc_main(int argc, char *argv[])
{
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
	delete simulator;

	return exit_status;
}
