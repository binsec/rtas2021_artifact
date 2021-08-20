/*
 *  Copyright (c) 2007-2014,
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
 * Authors: Julien Lisita (julien.lisita@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __AVR32EMU_SIMULATOR_TCC__
#define __AVR32EMU_SIMULATOR_TCC__

#include <unisim/kernel/config/xml/xml_config_file_helper.hh>
#include <unisim/kernel/config/ini/ini_config_file_helper.hh>
#include <unisim/kernel/config/json/json_config_file_helper.hh>

// Host machine standard headers
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

template <class CONFIG>
Simulator<CONFIG>::Simulator(int argc, char **argv, const sc_core::sc_module_name& name)
	: unisim::kernel::tlm2::Simulator(name, argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, ram(0)
	, loader(0)
	, avr32_t2h_syscalls(0)
	, debugger(0)
	, gdb_server(0)
	, inline_debugger(0)
	, sim_time(0)
	, host_time(0)
	, instrumenter(0)
	, profiler(0)
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
{
	// Optionally get the program to load and its arguments from the command line arguments
	unisim::kernel::VariableBase *cmd_args = FindVariable("cmd-args");
	unsigned int cmd_args_length = cmd_args->GetLength();
	if(cmd_args_length > 0)
	{
		SetVariable("loader.filename", ((std::string)(*cmd_args)[0]).c_str());
		
		SetVariable("avr32-t2h-syscalls.argc", cmd_args_length);
		
		unsigned int i;
		for(i = 0; i < cmd_args_length; i++)
		{
			std::stringstream sstr;
			sstr << "avr32-t2h-syscalls.argv[" << i << "]";
			SetVariable(sstr.str().c_str(), ((std::string)(*cmd_args)[i]).c_str());
		}
	}

	//=========================================================================
	//===                 Logger Printers instantiations                    ===
	//=========================================================================

	logger_console_printer = new LOGGER_CONSOLE_PRINTER();
	logger_text_file_writer = new LOGGER_TEXT_FILE_WRITER();
	logger_http_writer = new LOGGER_HTTP_WRITER();
	logger_xml_file_writer = new LOGGER_XML_FILE_WRITER();
	logger_netstream_writer = new LOGGER_NETSTREAM_WRITER();
	
	//=========================================================================
	//===                     Instrumenter instantiation                    ===
	//=========================================================================
	instrumenter = new INSTRUMENTER("instrumenter", this);
	
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - AVR32UC processor
	cpu = new CPU("cpu", this);
	//  - RAM
	ram = new RAM("ram", this);
	//  - Memory Router
	memory_router = new MEMORY_ROUTER("memory-router", this);
	//  - NMIREQ Stub
	nmireq_stub = new NMIREQ_STUB("nmireq-stub", this);
	//  - IRQ Stubs
	unsigned int irq;
	for(irq = 0; irq < CONFIG::CPU_CONFIG::NUM_IRQS; irq++)
	{
		std::stringstream sstr_irq_stub_name;
		sstr_irq_stub_name << "irq-stub" << irq;
		irq_stub[irq] = new IRQ_STUB(sstr_irq_stub_name.str().c_str(), this);
	}
	
	//=========================================================================
	//===                          Port registration                        ===
	//=========================================================================
	
	if(memory_router)
	{
		// - Memory Router
		instrumenter->RegisterPort(memory_router->input_if_clock);
		instrumenter->RegisterPort(memory_router->output_if_clock);
	}
	
	//=========================================================================
	//===                         Channels creation                         ===
	//=========================================================================
	
	instrumenter->CreateClock("CLK");
	
	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - Multiformat loader
	loader = new LOADER("loader");
	//  - AVR32 Target to Host system calls
	avr32_t2h_syscalls = new AVR32_T2H_SYSCALLS("avr32-t2h-syscalls");
	//  - debugger
	debugger = (enable_inline_debugger || enable_gdb_server || enable_profiler) ? new DEBUGGER("debugger") : 0;
	//  - GDB server
	gdb_server = enable_gdb_server ? new GDB_SERVER("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = enable_inline_debugger ? new INLINE_DEBUGGER("inline-debugger") : 0;
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - Profiler
	profiler = enable_profiler ? new PROFILER("profiler") : 0;
	//  - HTTP server
	http_server = new HTTP_SERVER("http-server");
	
	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	cpu->dhsb_master_sock(*memory_router->targ_socket[0]); // CPU>DHSB      <-> Memory Router
	cpu->ihsb_master_sock(*memory_router->targ_socket[1]); // CPU>IHSB      <-> Memory Router
	(*memory_router->init_socket[0])(ram->slave_sock);     // Memory Router <-> RAM
	nmireq_stub->master_sock(cpu->nmireq_slave_sock);      // NMIREQ Stub   <-> CPU>NMIREQ
	for(irq = 0; irq < CONFIG::CPU_CONFIG::NUM_IRQS; irq++)
	{
		irq_stub[irq]->master_sock(*cpu->irq_slave_sock[irq]);  // IRQ Stub <-> CPU>IRQ
	}

	instrumenter->Bind("HARDWARE.memory-router.input_if_clock", "HARDWARE.CLK");
	instrumenter->Bind("HARDWARE.memory-router.output_if_clock", "HARDWARE.CLK");
	
	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> memory_router->memory_export;
	*memory_router->memory_import[0] >> ram->memory_export;
	
	avr32_t2h_syscalls->registers_import >> cpu->registers_export;
	avr32_t2h_syscalls->memory_injection_import >> cpu->memory_injection_export;
	avr32_t2h_syscalls->blob_import >> loader->blob_export;
	cpu->avr32_t2h_syscalls_import >> avr32_t2h_syscalls->avr32_t2h_syscalls_export;
	
	*loader->memory_import[0] >> ram->memory_export;
	
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
			*debugger->debug_yielding_import[2]       >> profiler->debug_yielding_export;
			*debugger->debug_event_listener_import[2] >> profiler->debug_event_listener_export;
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
		*http_server->http_server_import[i++] >> instrumenter->http_server_export;
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

template <class CONFIG>
Simulator<CONFIG>::~Simulator()
{
	delete ram;
	delete memory_router;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(profiler) delete profiler;
	if(debugger) delete debugger;
	delete http_server;
	delete cpu;
	delete sim_time;
	delete host_time;
	delete loader;
	delete avr32_t2h_syscalls;
	delete nmireq_stub;
	
	for (unsigned irq = 0; irq < CONFIG::CPU_CONFIG::NUM_IRQS; irq++)
	{
		delete irq_stub[irq];
	}
	delete instrumenter;
	delete logger_console_printer;
	delete logger_text_file_writer;
	delete logger_http_writer;
	delete logger_xml_file_writer;
	delete logger_netstream_writer;
}

template <class CONFIG>
void Simulator<CONFIG>::LoadBuiltInConfig(unisim::kernel::Simulator *simulator)
{
	new unisim::kernel::config::xml::XMLConfigFileHelper(simulator);
	new unisim::kernel::config::ini::INIConfigFileHelper(simulator);
	new unisim::kernel::config::json::JSONConfigFileHelper(simulator);
	
	// meta information
	simulator->SetVariable("program-name", "UNISIM AVR32EMU");
	simulator->SetVariable("copyright", "Copyright (C) 2014, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Julien Lisita <julien.lisita@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM AVR32EMU, AVR32A simulator");
	simulator->SetVariable("schematic", "avr32emu/fig_schematic.pdf");

	int gdb_server_tcp_port = 0;
	const char *gdb_server_arch_filename = "unisim/service/debug/gdb_server/gdb_avr32.xml";
	const char *dwarf_register_number_mapping_filename = "unisim/util/debug/dwarf/avr32_dwarf_register_number_mapping.xml";
	uint64_t maxinst = 0xffffffffffffffffULL; // maximum number of instruction to simulate
	double cpu_frequency = 66.67; // in Mhz
	double cpu_clock_multiplier = 1.0;
	double cpu_ipc = 1.0; // in instructions per cycle: AVR32UC is RISC pipelined processor that can execute up to 1 instruction per cycle
	double cpu_cycle_time = (double)(1.0e6 / cpu_frequency); // in picoseconds
	double hsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	double mem_cycle_time = hsb_cycle_time;

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	//  - CLK
	simulator->SetVariable( "HARDWARE.CLK.clock-period", sc_core::sc_time(hsb_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable( "HARDWARE.CLK.lazy-clock", true);

	//  - PowerPC processor
	// if the following line ("cpu-cycle-time") is commented, the cpu will use the power estimators to find min cpu cycle time
	simulator->SetVariable("HARDWARE.cpu.cpu-cycle-time", sc_core::sc_time(cpu_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.cpu.hsb-cycle-time", sc_core::sc_time(hsb_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.cpu.max-inst", maxinst);
	simulator->SetVariable("HARDWARE.cpu.nice-time", "1 ms");
	simulator->SetVariable("HARDWARE.cpu.ipc", cpu_ipc);
	simulator->SetVariable("HARDWARE.cpu.enable-dmi", true); // Allow CPU to use of SystemC TLM 2.0 DMI

	//  - Memory router
	std::stringstream sstr_memory_router_mapping;
	sstr_memory_router_mapping << "range_start=\"0x" << std::hex << CONFIG::RAM_BASE_ADDR << "\" range_end=\"0x" << (CONFIG::RAM_BYTE_SIZE - 1) << " output_port=\"0\" translation=\"0x0\"";
	simulator->SetVariable("HARDWARE.memory-router.mapping_0", sstr_memory_router_mapping.str().c_str()); // RAM
	//simulator->SetVariable("memory-router.mapping_0", "range_start=\"0x0\" range_end=\"0xffffffff\" output_port=\"0\" translation=\"0x0\""); // RAM

	// - Loader
	simulator->SetVariable("loader.file0.force-use-virtual-address", false);

	// - Loader memory router
	std::stringstream sstr_loader_mapping;
	sstr_loader_mapping << "HARDWARE.ram:0x" << std::hex << CONFIG::RAM_BASE_ADDR << "-0x" << (CONFIG::RAM_BYTE_SIZE - 1);
	simulator->SetVariable("loader.memory-mapper.mapping", sstr_loader_mapping.str().c_str()); // all address space
	//simulator->SetVariable("loader.memory-mapper.mapping", "ram:0x0-0xffffffff"); // all address space

	//  - RAM
	simulator->SetVariable("HARDWARE.ram.cycle-time", sc_core::sc_time(mem_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.ram.read-latency", sc_core::sc_time(mem_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.ram.write-latency", sc_core::SC_ZERO_TIME.to_string().c_str());
	simulator->SetVariable("HARDWARE.ram.org", CONFIG::RAM_BASE_ADDR);
	simulator->SetVariable("HARDWARE.ram.bytesize", CONFIG::RAM_BYTE_SIZE);
	
	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - GDB Server run-time configuration
	simulator->SetVariable("gdb-server.tcp-port", gdb_server_tcp_port);
	simulator->SetVariable("gdb-server.architecture-description-filename", gdb_server_arch_filename);
	
	//  - Debugger run-time configuration
	simulator->SetVariable("debugger.parse-dwarf", false);
	simulator->SetVariable("debugger.dwarf-register-number-mapping-filename", dwarf_register_number_mapping_filename);
	
	// - Http Server
	simulator->SetVariable("http-server.http-port", 12360);
}

template <class CONFIG>
void Simulator<CONFIG>::Run()
{
	std::cerr << "Starting simulation" << std::endl;
	
	double time_start = host_time->GetTime();

	unisim::kernel::tlm2::Simulator::Run();

	double time_stop = host_time->GetTime();
	double spent_time = time_stop - time_start;

	std::cerr << "Simulation run-time parameters:" << std::endl;
	DumpParameters(std::cerr);
	std::cerr << std::endl;
	
	std::cerr << "Simulation statistics:" << std::endl;
	DumpStatistics(std::cerr);
	std::cerr << std::endl;

	std::cerr << "simulation time: " << spent_time << " seconds" << std::endl;
	std::cerr << "simulated time : " << sc_core::sc_time_stamp().to_seconds() << " seconds (exactly " << sc_core::sc_time_stamp() << ")" << std::endl;
	std::cerr << "target speed: " << ((double) (*cpu)["instruction-counter"] / ((double) (*cpu)["run-time"] - (double) (*cpu)["idle-time"]) / 1000000.0) << " MIPS" << std::endl;
	std::cerr << "host simulation speed: " << ((double) (*cpu)["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << std::endl;
	std::cerr << "time dilatation: " << spent_time / sc_core::sc_time_stamp().to_seconds() << " times slower than target machine" << std::endl;
}

template <class CONFIG>
unisim::kernel::Simulator::SetupStatus Simulator<CONFIG>::Setup()
{
	if(inline_debugger || profiler)
	{
		SetVariable("debugger.parse-dwarf", true);
	}
	
	unisim::kernel::Simulator::SetupStatus setup_status = unisim::kernel::Simulator::Setup();
	
	return setup_status;
}

template <class CONFIG>
bool Simulator<CONFIG>::EndSetup()
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
	return true;
}

template <class CONFIG>
void Simulator<CONFIG>::SigInt()
{
	if(!inline_debugger || !inline_debugger->IsStarted())
	{
		unisim::kernel::Simulator::Instance()->Stop(0, 0, true);
	}
}

#endif // __AVR32EMU_SIMULATOR_TCC__
