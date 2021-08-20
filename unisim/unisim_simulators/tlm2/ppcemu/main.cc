/*
 *  Copyright (c) 2007-2010,
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/console/console_printer.hh>
#include <unisim/kernel/logger/text_file/text_file_writer.hh>
#include <unisim/kernel/logger/http/http_writer.hh>
#include <unisim/kernel/logger/xml_file/xml_file_writer.hh>
#include <unisim/kernel/logger/netstream/netstream_writer.hh>
#include <unisim/kernel/config/xml/xml_config_file_helper.hh>
#include <unisim/kernel/config/ini/ini_config_file_helper.hh>
#include <unisim/kernel/config/json/json_config_file_helper.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/debugger/debugger.tcc>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/os/linux_os/powerpc_linux32.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>

#include <unisim/component/tlm2/processor/powerpc/e600/mpc7447a/cpu.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>

#include <iostream>
#include <stdexcept>

#ifdef DEBUG_PPCEMU
static const bool DEBUG_INFORMATION = true;
#else
static const bool DEBUG_INFORMATION = false;
#endif

class IRQStub
	: public sc_core::sc_module
	, tlm::tlm_bw_transport_if<unisim::component::tlm2::interrupt::InterruptProtocolTypes>
{
public:
	tlm::tlm_initiator_socket<0, unisim::component::tlm2::interrupt::InterruptProtocolTypes> irq_master_sock;
	
	IRQStub(const sc_core::sc_module_name& name);

	virtual tlm::tlm_sync_enum nb_transport_bw(unisim::component::tlm2::interrupt::InterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
};

IRQStub::IRQStub(const sc_core::sc_module_name& name)
	: sc_core::sc_module(name)
	, irq_master_sock("irq-master-sock")
{
	irq_master_sock(*this);
}

tlm::tlm_sync_enum IRQStub::nb_transport_bw(unisim::component::tlm2::interrupt::InterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	return tlm::TLM_COMPLETED;
}

void IRQStub::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}


class Simulator : public unisim::kernel::Simulator
{
public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	void Run();
	virtual unisim::kernel::Simulator::SetupStatus Setup();
	virtual void Stop(unisim::kernel::Object *object, int exit_status, bool asynchronous = false);
	int GetExitStatus() const;
	virtual void SigInt();
protected:
private:

	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================

	// Front Side Bus template parameters
	typedef uint32_t CPU_ADDRESS_TYPE;
	typedef uint64_t FSB_ADDRESS_TYPE;
	static const unsigned int FSB_BURST_SIZE = 32;

	struct DEBUGGER_CONFIG
	{
		typedef CPU_ADDRESS_TYPE ADDRESS;
		static const unsigned int NUM_PROCESSORS = 1;
		/* gdb_server, inline_debugger and/or monitor */
		static const unsigned int MAX_FRONT_ENDS = 2;
	};
	
	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm2::processor::powerpc::e600::mpc7447a::CPU CPU;
	typedef unisim::component::tlm2::memory::ram::Memory<CPU::FSB_WIDTH * 8, FSB_ADDRESS_TYPE, FSB_BURST_SIZE / CPU::FSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_INFORMATION> MEMORY;
	typedef IRQStub IRQ_STUB;

	//=========================================================================
	//===                      Aliases for services classes                 ===
	//=========================================================================

	typedef unisim::service::debug::debugger::Debugger<DEBUGGER_CONFIG> DEBUGGER;
	typedef unisim::service::debug::gdb_server::GDBServer<CPU_ADDRESS_TYPE> GDB_SERVER;
	typedef unisim::service::debug::inline_debugger::InlineDebugger<CPU_ADDRESS_TYPE> INLINE_DEBUGGER;
	typedef unisim::service::os::linux_os::PowerPCLinux32<CPU_ADDRESS_TYPE, CPU_ADDRESS_TYPE> LINUS_OS;
	typedef unisim::kernel::logger::console::Printer LOGGER_CONSOLE_PRINTER;
	typedef unisim::kernel::logger::text_file::Writer LOGGER_TEXT_FILE_WRITER;
	typedef unisim::kernel::logger::http::Writer LOGGER_HTTP_WRITER;
	typedef unisim::kernel::logger::xml_file::Writer LOGGER_XML_FILE_WRITER;
	typedef unisim::kernel::logger::netstream::Writer LOGGER_NETSTREAM_WRITER;
	
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	CPU *cpu;
	//  - RAM
	MEMORY *memory;
	// - IRQ stubs
	IRQ_STUB *external_interrupt_stub;
	IRQ_STUB *hard_reset_stub;
	IRQ_STUB *soft_reset_stub;
	IRQ_STUB *mcp_slave_stub;
//	IRQ_STUB *tea_slave_stub;
	IRQ_STUB *smi_slave_stub;

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - Linux loader and Linux ABI translator
	LINUS_OS *linux_os;
	
	//  - debugger back-end
	DEBUGGER *debugger;
	//  - GDB server
	GDB_SERVER *gdb_server;
	//  - Inline debugger
	INLINE_DEBUGGER *inline_debugger;
	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *sim_time;
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time;
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
	unisim::kernel::variable::Parameter<bool> param_enable_gdb_server;
	unisim::kernel::variable::Parameter<bool> param_enable_inline_debugger;

	int exit_status;
	static void LoadBuiltInConfig(unisim::kernel::Simulator *simulator);
};



Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, memory(0)
	, external_interrupt_stub(0)
	, hard_reset_stub(0)
	, soft_reset_stub(0)
	, mcp_slave_stub(0)
//	, tea_slave_stub(0)
	, smi_slave_stub(0)
	, debugger(0)
	, gdb_server(0)
	, inline_debugger(0)
	, sim_time(0)
	, host_time(0)
	, logger_console_printer(0)
	, logger_text_file_writer(0)
	, logger_http_writer(0)
	, logger_xml_file_writer(0)
	, logger_netstream_writer(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
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
	external_interrupt_stub = new IRQ_STUB("external-interrupt-stub");
	hard_reset_stub = new IRQ_STUB("hard-reset-stub");
	soft_reset_stub = new IRQ_STUB("soft-reset-stub");
	mcp_slave_stub = new IRQ_STUB("mcp-stub");
	//	tea_slave_stub = new IRQ_STUB("tea-stub");
	smi_slave_stub = new IRQ_STUB("smi-stub");

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - Linux loader and Linux ABI translator
	linux_os = new LINUS_OS("linux-os");
	//  - Debugger
	debugger = (enable_inline_debugger or enable_gdb_server) ? new DEBUGGER("debugger") : 0;
	//  - GDB server
	gdb_server = enable_gdb_server ? new GDB_SERVER("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = enable_inline_debugger ? new INLINE_DEBUGGER("inline-debugger") : 0;
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");

	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	cpu->bus_master_sock(memory->slave_sock); // CPU <-> RAM
	external_interrupt_stub->irq_master_sock(cpu->external_interrupt_slave_sock);
	hard_reset_stub->irq_master_sock(cpu->hard_reset_slave_sock);
	soft_reset_stub->irq_master_sock(cpu->soft_reset_slave_sock);
	mcp_slave_stub->irq_master_sock(cpu->mcp_slave_sock);
	//	tea_slave_stub->irq_master_sock(cpu->tea_slave_sock);
	smi_slave_stub->irq_master_sock(cpu->smi_slave_sock);

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> memory->memory_export;
	
	if (enable_inline_debugger or enable_gdb_server)
	{
		// Debugger <-> CPU Connections
		// Debugger <-> CPU connections
		cpu->debug_yielding_import                            >> *debugger->debug_yielding_export[0];
		cpu->trap_reporting_import                            >> *debugger->trap_reporting_export[0];
		cpu->memory_access_reporting_import                   >> *debugger->memory_access_reporting_export[0];
		*debugger->disasm_import[0]                          >> cpu->disasm_export;
		*debugger->memory_import[0]                          >> cpu->memory_export;
		*debugger->registers_import[0]                       >> cpu->registers_export;
		*debugger->memory_access_reporting_control_import[0] >> cpu->memory_access_reporting_control_export;
			
		// Debugger <-> Loader connections
		debugger->blob_import >> linux_os->blob_export_;
		
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
			inline_debugger->subprogram_lookup_import      >> *debugger->subprogram_lookup_export[0];
		}
		
		if (enable_gdb_server)
		{
			// gdb-server <-> debugger connections
			*debugger->debug_event_listener_import[1] >> gdb_server->debug_event_listener_export;
			*debugger->debug_yielding_import[1]       >> gdb_server->debug_yielding_export;
			gdb_server->debug_yielding_request_import >> *debugger->debug_yielding_request_export[1];
			gdb_server->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[1];
			gdb_server->memory_import                 >> *debugger->memory_export[1];
			gdb_server->registers_import              >> *debugger->registers_export[1];
		}
	}

	cpu->linux_os_import >> linux_os->linux_os_export_;
	linux_os->memory_import_ >> cpu->memory_export;
	linux_os->memory_injection_import_ >> cpu->memory_injection_export;
	linux_os->registers_import_ >> cpu->registers_export;
}

Simulator::~Simulator()
{
	delete external_interrupt_stub;
	delete hard_reset_stub;
	delete soft_reset_stub;
	delete mcp_slave_stub;
//	delete tea_slave_stub;
	delete smi_slave_stub;
	delete memory;
	delete debugger;
	delete gdb_server;
	delete inline_debugger;
	delete cpu;
	delete sim_time;
	delete linux_os;
	if(logger_console_printer) delete logger_console_printer;
	if(logger_text_file_writer) delete logger_text_file_writer;
	if(logger_http_writer) delete logger_http_writer;
	if(logger_xml_file_writer) delete logger_xml_file_writer;
	if(logger_netstream_writer) delete logger_netstream_writer;
}

void Simulator::LoadBuiltInConfig(unisim::kernel::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM ppcemu");
	simulator->SetVariable("copyright", "Copyright (C) 2007-2010, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM ppcemu, user level PowerPC simulator with support of ELF32 binaries and Linux system call translation");
	simulator->SetVariable("schematic", "ppcemu/fig_schematic.pdf");

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	//  - PowerPC processor
	// if the following line ("cpu-cycle-time") is commented, the cpu will use the power estimators to find min cpu cycle time
	simulator->SetVariable("cpu.cpu-cycle-time", "3333 ps"); // 300 Mhz
	simulator->SetVariable("cpu.bus-cycle-time", "13333 ps"); // 75 Mhz
	simulator->SetVariable("cpu.nice-time", "1 ms"); // 1 ms
	simulator->SetVariable("cpu.ipc", 1.0);
	simulator->SetVariable("cpu.enable-dmi", true); // Allow CPU to use SystemC TLM 2.0 DMI

	//  - RAM
	simulator->SetVariable("memory.cycle-time", "13333 ps");
	simulator->SetVariable("memory.read-latency", "13333 ps");
	simulator->SetVariable("memory.write-latency", "13333 ps");
	simulator->SetVariable("memory.org", 0x00000000UL);
	simulator->SetVariable("memory.bytesize", (uint32_t) -1);

	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - GDB Server run-time configuration
	simulator->SetVariable("gdb-server.tcp-port", "1234");
	simulator->SetVariable("gdb-server.architecture-description-filename", "unisim/service/debug/gdb_server/gdb_powerpc_32.xml");
	
	//  - Debugger run-time configuration
	simulator->SetVariable("debugger.parse-dwarf", false);
	simulator->SetVariable("debugger.dwarf-register-number-mapping-filename", "unisim/util/debug/dwarf/powerpc_eabi_gcc_dwarf_register_number_mapping.xml");

	//  - Linux OS run-time configuration
	simulator->SetVariable("linux-os.endianness", "big-endian");
	simulator->SetVariable("linux-os.stack-base", 0xc0000000);
	simulator->SetVariable("linux-os.envc", 0);
//	simulator->SetVariable("linux-os.system", "ppc");
// 	simulator->SetVariable("linux-os.endianness", "big-endian");
	simulator->SetVariable("linux-os.utsname-sysname", "Linux");
 	simulator->SetVariable("linux-os.utsname-nodename", "(none)");
 	simulator->SetVariable("linux-os.utsname-release", "5.1.20");
 	simulator->SetVariable("linux-os.utsname-version", "#1 PREEMPT Thu Jan 1 00:00:00 CEST 1970");
	simulator->SetVariable("linux-os.utsname-machine", "ppc");
	simulator->SetVariable("linux-os.utsname-domainname", "(none)");
// 	simulator->SetVariable("linux-os.apply-host-environment", false);
	
	// Inline debugger
	simulator->SetVariable("inline-debugger.num-loaders", 1);
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
		std::cerr << e.what() << std::endl;
	}

	std::cerr << "Simulation finished" << std::endl;

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

unisim::kernel::Simulator::SetupStatus Simulator::Setup()
{
	if(enable_inline_debugger)
	{
		SetVariable("debugger.parse-dwarf", true);
	}
	
	unisim::kernel::Simulator::SetupStatus setup_status = unisim::kernel::Simulator::Setup();

	return setup_status;
}

void Simulator::Stop(unisim::kernel::Object *object, int _exit_status, bool asynchronous)
{
	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}
	std::cerr << "Program exited with status " << exit_status << std::endl;
	sc_core::sc_stop();
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
	if(!inline_debugger || !inline_debugger->IsStarted())
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
			std::cerr << "Some warnings occurred during setup" << std::endl;
		case unisim::kernel::Simulator::ST_OK_TO_START:
			std::cerr << "Starting simulation at user privilege level (Linux system call translation mode)" << std::endl;
			simulator->Run();
			break;
		case unisim::kernel::Simulator::ST_ERROR:
			std::cerr << "Can't start simulation because of previous errors" << std::endl;
			break;
	}

	int exit_status = simulator->GetExitStatus();
	if(simulator) delete simulator;

	return exit_status;
}
