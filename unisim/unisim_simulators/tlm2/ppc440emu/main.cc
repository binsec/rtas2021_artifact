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
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/os/linux_os/powerpc_linux32.hh>
#include <unisim/service/power/cache_power_estimator.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/translator/memory_address/memory/translator.hh>

#include <unisim/component/cxx/processor/powerpc/ppc440/config.hh>
#include <unisim/component/tlm2/processor/powerpc/ppc440/cpu.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.hh>
#include <unisim/component/tlm2/interconnect/generic_router/config.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.tcc>

#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <signal.h>

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#endif

#ifdef DEBUG_PPC440EMU
static const bool DEBUG_INFORMATION = true;
typedef unisim::component::cxx::processor::powerpc::ppc440::DebugConfig_woMMU_wFPU CPU_CONFIG;
#else
static const bool DEBUG_INFORMATION = false;
typedef unisim::component::cxx::processor::powerpc::ppc440::Config_woMMU_wFPU CPU_CONFIG;
#endif

void SigIntHandler(int signum)
{
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	sc_stop();
}

using namespace std;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::service::os::linux_os::Linux;
using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::service::power::CachePowerEstimator;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Variable;
using unisim::kernel::VariableBase;
using unisim::kernel::Object;

#ifdef DEBUG_PPC440EMU
class PLBDebugConfig : public unisim::component::tlm2::interconnect::generic_router::VerboseConfig
{
public:
	static const unsigned int INPUT_SOCKETS = 3;
	static const unsigned int OUTPUT_SOCKETS = 1;
	static const unsigned int MAX_NUM_MAPPINGS = 1;
	static const unsigned int BUSWIDTH = 128;
};

typedef PLBDebugConfig PLB_CONFIG;
#else
class PLBConfig : public unisim::component::tlm2::interconnect::generic_router::Config
{
public:
	static const unsigned int INPUT_SOCKETS = 3;
	static const unsigned int OUTPUT_SOCKETS = 1;
	static const unsigned int MAX_NUM_MAPPINGS = 1;
	static const unsigned int BUSWIDTH = 128;
};

typedef PLBConfig PLB_CONFIG;
#endif

class Simulator : public unisim::kernel::Simulator
{
public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	void Run();
	virtual unisim::kernel::Simulator::SetupStatus Setup();
	virtual void Stop(Object *object, int exit_status);
	int GetExitStatus() const;
protected:
private:

	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================

	// Front Side Bus template parameters
	typedef CPU_CONFIG::address_t CPU_ADDRESS_TYPE;
	typedef CPU_CONFIG::physical_address_t FSB_ADDRESS_TYPE;
	typedef uint32_t CPU_REG_TYPE;

	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm2::memory::ram::Memory<CPU_CONFIG::FSB_WIDTH * 8, FSB_ADDRESS_TYPE, CPU_CONFIG::FSB_BURST_SIZE / CPU_CONFIG::FSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_INFORMATION> MEMORY;
	typedef unisim::component::tlm2::processor::powerpc::ppc440::CPU<CPU_CONFIG> CPU;
	typedef unisim::kernel::tlm2::InitiatorStub<0, unisim::component::tlm2::interrupt::InterruptProtocolTypes> IRQ_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DCR_STUB;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<PLB_CONFIG> PLB;

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	CPU *cpu;
	//  - PLB
	PLB *plb;
	//  - RAM
	MEMORY *memory;
	// - IRQ stubs
	IRQ_STUB *external_input_interrupt_stub;
	IRQ_STUB *critical_input_interrupt_stub;
	// - DCR stub
	DCR_STUB *dcr_stub;

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - Linux OS
	Linux<CPU_ADDRESS_TYPE, CPU_REG_TYPE> *linux_os;
	struct DEBUGGER_CONFIG
	{
		typedef CPU_ADDRESS_TYPE ADDRESS;
		static const unsigned int NUM_PROCESSORS = 1;
		/* gdb_server, inline_debugger and/or monitor */
		static const unsigned int MAX_FRONT_ENDS = 2;
	};
	
	typedef unisim::service::debug::debugger::Debugger<DEBUGGER_CONFIG> Debugger;
	//  - debugger back-end
	Debugger *debugger;
	//  - GDB server
	GDBServer<CPU_ADDRESS_TYPE> *gdb_server;
	//  - Inline debugger
	InlineDebugger<CPU_ADDRESS_TYPE> *inline_debugger;
	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *sim_time;
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time;
	//  - the optional power estimators
	CachePowerEstimator *il1_power_estimator;
	CachePowerEstimator *dl1_power_estimator;
	CachePowerEstimator *itlb_power_estimator;
	CachePowerEstimator *dtlb_power_estimator;
	CachePowerEstimator *utlb_power_estimator;
	//  - memory address translator from effective address to physical address
	unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE> *effective_to_physical_address_translator;

	bool enable_gdb_server;
	bool enable_inline_debugger;
	bool estimate_power;
	Parameter<bool> param_enable_gdb_server;
	Parameter<bool> param_enable_inline_debugger;
	Parameter<bool> param_estimate_power;

	int exit_status;
	static void LoadBuiltInConfig(unisim::kernel::Simulator *simulator);
};



Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, plb(0)
	, memory(0)
	, external_input_interrupt_stub(0)
	, critical_input_interrupt_stub(0)
	, dcr_stub(0)
	, debugger(0)
	, gdb_server(0)
	, inline_debugger(0)
	, sim_time(0)
	, host_time(0)
	, il1_power_estimator(0)
	, dl1_power_estimator(0)
	, itlb_power_estimator(0)
	, dtlb_power_estimator(0)
	, utlb_power_estimator(0)
	, effective_to_physical_address_translator(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, estimate_power(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, param_estimate_power("estimate-power", 0, estimate_power, "Enable/Disable power estimators instantiation")
	, exit_status(0)
{
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	cpu = new CPU("cpu");
	//  - PLB
	plb = new PLB("plb");
	//  - RAM
	memory = new MEMORY("memory");
	//  - IRQ Stubs
	external_input_interrupt_stub = new IRQ_STUB("external-input-interrupt-stub");
	critical_input_interrupt_stub = new IRQ_STUB("critical-input-interrupt-stub");
	//  - DCR stub
	dcr_stub = new DCR_STUB("dcr-stub");

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - Linux OS
	linux_os = new unisim::service::os::linux_os::PowerPCLinux32<CPU_ADDRESS_TYPE, CPU_ADDRESS_TYPE>("linux-os");
	//  - Debugger
	debugger = (enable_inline_debugger or enable_gdb_server) ? new Debugger("debugger") : 0;
	//  - GDB server
	gdb_server = enable_gdb_server ? new GDBServer<CPU_ADDRESS_TYPE>("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = enable_inline_debugger ? new InlineDebugger<CPU_ADDRESS_TYPE>("inline-debugger") : 0;
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - the optional power estimators
	il1_power_estimator = estimate_power ? new CachePowerEstimator("il1-power-estimator") : 0;
	dl1_power_estimator = estimate_power ? new CachePowerEstimator("dl1-power-estimator") : 0;
	itlb_power_estimator = estimate_power ? new CachePowerEstimator("itlb-power-estimator") : 0;
	dtlb_power_estimator = estimate_power ? new CachePowerEstimator("dtlb-power-estimator") : 0;
	utlb_power_estimator = estimate_power ? new CachePowerEstimator("utlb-power-estimator") : 0;
	//  - memory address translator from effective address to physical address
	effective_to_physical_address_translator = new unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE>("effective-to-physical-address-translator");
	
	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	cpu->icurd_plb_master_sock(*plb->targ_socket[0]); // CPU>ICURD <-> PLB
	cpu->dcuwr_plb_master_sock(*plb->targ_socket[1]); // CPU>DCUWR <-> PLB
	cpu->dcurd_plb_master_sock(*plb->targ_socket[2]); // CPU>DCURD <-> PLB
	(*plb->init_socket[0])(memory->slave_sock); // PLB <-> RAM
	external_input_interrupt_stub->master_sock(cpu->external_input_interrupt_slave_sock);
	critical_input_interrupt_stub->master_sock(cpu->critical_input_interrupt_slave_sock);
	cpu->dcr_master_sock(dcr_stub->slave_sock);

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> plb->memory_export;
	(*plb->memory_import[0]) >> memory->memory_export;
	
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

	if(estimate_power)
	{
		// Connect everything related to power estimation
		cpu->il1_power_estimator_import >> il1_power_estimator->power_estimator_export;
		cpu->il1_power_mode_import >> il1_power_estimator->power_mode_export;
		cpu->dl1_power_estimator_import >> dl1_power_estimator->power_estimator_export;
		cpu->dl1_power_mode_import >> dl1_power_estimator->power_mode_export;
		cpu->itlb_power_estimator_import >> itlb_power_estimator->power_estimator_export;
		cpu->itlb_power_mode_import >> itlb_power_estimator->power_mode_export;
		cpu->dtlb_power_estimator_import >> dtlb_power_estimator->power_estimator_export;
		cpu->dtlb_power_mode_import >> dtlb_power_estimator->power_mode_export;
		cpu->utlb_power_estimator_import >> utlb_power_estimator->power_estimator_export;
		cpu->utlb_power_mode_import >> utlb_power_estimator->power_mode_export;

		il1_power_estimator->time_import >> sim_time->time_export;
		dl1_power_estimator->time_import >> sim_time->time_export;
		itlb_power_estimator->time_import >> sim_time->time_export;
		dtlb_power_estimator->time_import >> sim_time->time_export;
		utlb_power_estimator->time_import >> sim_time->time_export;
	}

	effective_to_physical_address_translator->memory_import >> memory->memory_export;
	cpu->linux_os_import >> linux_os->linux_os_export_;
	linux_os->memory_import_ >> cpu->memory_export;
	linux_os->memory_injection_import_ >> cpu->memory_injection_export;
	linux_os->registers_import_ >> cpu->registers_export;
}

Simulator::~Simulator()
{
	delete external_input_interrupt_stub;
	delete critical_input_interrupt_stub;
	delete memory;
	delete debugger;
	delete gdb_server;
	delete inline_debugger;
	delete cpu;
	delete plb;
	delete il1_power_estimator;
	delete dl1_power_estimator;
	delete itlb_power_estimator;
	delete dtlb_power_estimator;
	delete utlb_power_estimator;
	delete sim_time;
	delete linux_os;
	delete effective_to_physical_address_translator;
}

void Simulator::LoadBuiltInConfig(unisim::kernel::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM ppc440emu");
	simulator->SetVariable("copyright", "Copyright (C) 2007-2011, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM ppc440emu, user level PowerPC 440 simulator with support of ELF32 binaries and Linux system call translation");

	const char *filename = "";
	int gdb_server_tcp_port = 0;
	const char *gdb_server_arch_filename = "gdb_powerpc_32.xml";
	uint64_t maxinst = 0xffffffffffffffffULL; // maximum number of instruction to simulate
	double cpu_frequency = 400.0; // in Mhz
	double cpu_clock_multiplier = 2.0;
	double ext_timer_clock_divisor = 2.0;
	uint32_t tech_node = 130; // in nm
	double cpu_ipc = 1.0; // in instructions per cycle
	double cpu_cycle_time = (double)(1.0e6 / cpu_frequency); // in picoseconds
	double fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	double ext_timer_cycle_time = ext_timer_clock_divisor * cpu_cycle_time;
	double mem_cycle_time = fsb_cycle_time;

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	//  - PowerPC processor
	// if the following line ("cpu-cycle-time") is commented, the cpu will use the power estimators to find min cpu cycle time
	simulator->SetVariable("cpu.cpu-cycle-time", sc_time(cpu_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.bus-cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.ext-timer-cycle-time", sc_time(ext_timer_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.voltage", 1.3 * 1e3); // mV
	simulator->SetVariable("cpu.max-inst", maxinst);
	simulator->SetVariable("cpu.nice-time", "1 ms"); // 1 ms
	simulator->SetVariable("cpu.ipc", cpu_ipc);

	//  - PLB
	simulator->SetVariable("plb.cycle_time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("plb.mapping_0", "range_start=\"0x0\" range_end=\"0xffffffffffffffff\" output_port=\"0\" translation=\"0x0\""); // RAM

	//  - RAM
	simulator->SetVariable("memory.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("memory.read-latency", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("memory.write-latency", SC_ZERO_TIME.to_string().c_str());
	simulator->SetVariable("memory.org", 0x00000000UL);
	simulator->SetVariable("memory.bytesize", (uint32_t) -1);

	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - GDB Server run-time configuration
	simulator->SetVariable("gdb-server.tcp-port", gdb_server_tcp_port);
	simulator->SetVariable("gdb-server.architecture-description-filename", gdb_server_arch_filename);
	//  - ELF32 Loader run-time configuration
	simulator->SetVariable("elf32-loader.filename", filename);

	//  - Linux loader run-time configuration
	simulator->SetVariable("linux-loader.endianness", "big-endian");
	simulator->SetVariable("linux-loader.stack-base", 0xc0000000);
	simulator->SetVariable("linux-loader.max-environ", 16 * 1024);
	simulator->SetVariable("linux-loader.argc", 1);
	simulator->SetVariable("linux-loader.argv[0]", filename);
	simulator->SetVariable("linux-loader.envc", 0);

	//  - Linux OS run-time configuration
	simulator->SetVariable("linux-os.system", "powerpc");
	simulator->SetVariable("linux-os.endianness", "big-endian");
	simulator->SetVariable("linux-os.utsname-sysname", "Linux");
	simulator->SetVariable("linux-os.utsname-nodename", "localhost");
	simulator->SetVariable("linux-os.utsname-release", "2.6.27.35");
	simulator->SetVariable("linux-os.utsname-version", "#UNISIM SMP Fri Mar 12 05:23:09 UTC 2010");
	simulator->SetVariable("linux-os.utsname-machine", "powerpc");
	simulator->SetVariable("linux-os.verbose", false);

	//  - Cache/TLB power estimators run-time configuration
	simulator->SetVariable("il1-power-estimator.cache-size", 32 * 1024);
	simulator->SetVariable("il1-power-estimator.line-size", 32);
	simulator->SetVariable("il1-power-estimator.associativity", 8);
	simulator->SetVariable("il1-power-estimator.rw-ports", 0);
	simulator->SetVariable("il1-power-estimator.excl-read-ports", 1);
	simulator->SetVariable("il1-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("il1-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("il1-power-estimator.banks", 4);
	simulator->SetVariable("il1-power-estimator.tech-node", tech_node);
	simulator->SetVariable("il1-power-estimator.output-width", 128);
	simulator->SetVariable("il1-power-estimator.tag-width", 64);
	simulator->SetVariable("il1-power-estimator.access-mode", "fast");

	simulator->SetVariable("dl1-power-estimator.cache-size", 32 * 1024);
	simulator->SetVariable("dl1-power-estimator.line-size", 32);
	simulator->SetVariable("dl1-power-estimator.associativity", 8);
	simulator->SetVariable("dl1-power-estimator.rw-ports", 1);
	simulator->SetVariable("dl1-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("dl1-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("dl1-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("dl1-power-estimator.banks", 4);
	simulator->SetVariable("dl1-power-estimator.tech-node", tech_node);
	simulator->SetVariable("dl1-power-estimator.output-width", 64);
	simulator->SetVariable("dl1-power-estimator.tag-width", 64);
	simulator->SetVariable("dl1-power-estimator.access-mode", "fast");

	simulator->SetVariable("itlb-power-estimator.cache-size", 4 * 2 * 4);
	simulator->SetVariable("itlb-power-estimator.line-size", 4);
	simulator->SetVariable("itlb-power-estimator.associativity", 4);
	simulator->SetVariable("itlb-power-estimator.rw-ports", 1);
	simulator->SetVariable("itlb-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("itlb-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("itlb-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("itlb-power-estimator.banks", 4);
	simulator->SetVariable("itlb-power-estimator.tech-node", tech_node);
	simulator->SetVariable("itlb-power-estimator.output-width", 32);
	simulator->SetVariable("itlb-power-estimator.tag-width", 64);
	simulator->SetVariable("itlb-power-estimator.access-mode", "fast");

	simulator->SetVariable("dtlb-power-estimator.cache-size", 8 * 2 * 4);
	simulator->SetVariable("dtlb-power-estimator.line-size", 4);
	simulator->SetVariable("dtlb-power-estimator.associativity", 4);
	simulator->SetVariable("dtlb-power-estimator.rw-ports", 1);
	simulator->SetVariable("dtlb-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("dtlb-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("dtlb-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("dtlb-power-estimator.banks", 4);
	simulator->SetVariable("dtlb-power-estimator.tech-node", tech_node);
	simulator->SetVariable("dtlb-power-estimator.output-width", 32);
	simulator->SetVariable("dtlb-power-estimator.tag-width", 64);
	simulator->SetVariable("dtlb-power-estimator.access-mode", "fast");

	simulator->SetVariable("utlb-power-estimator.cache-size", 64 * 2 * 4);
	simulator->SetVariable("utlb-power-estimator.line-size", 4);
	simulator->SetVariable("utlb-power-estimator.associativity", 64);
	simulator->SetVariable("utlb-power-estimator.rw-ports", 1);
	simulator->SetVariable("utlb-power-estimator.excl-read-ports", 0);
	simulator->SetVariable("utlb-power-estimator.excl-write-ports", 0);
	simulator->SetVariable("utlb-power-estimator.single-ended-read-ports", 0);
	simulator->SetVariable("utlb-power-estimator.banks", 4);
	simulator->SetVariable("utlb-power-estimator.tech-node", tech_node);
	simulator->SetVariable("utlb-power-estimator.output-width", 32);
	simulator->SetVariable("utlb-power-estimator.tag-width", 64);
	simulator->SetVariable("utlb-power-estimator.access-mode", "fast");

	// Inline debugger
	simulator->SetVariable("inline-debugger.num-loaders", 1);
}

void Simulator::Run()
{
	double time_start = host_time->GetTime();

	try
	{
		sc_start();
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
	cerr << "simulated time : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << endl;
	cerr << "host simulation speed: " << ((double) (*cpu)["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << endl;
	cerr << "time dilatation: " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << endl;
}

unisim::kernel::Simulator::SetupStatus Simulator::Setup()
{
	// Build the Linux OS arguments from the command line arguments
	
	VariableBase *cmd_args = FindVariable("cmd-args");
	unsigned int cmd_args_length = cmd_args->GetLength();
	if(cmd_args_length > 0)
	{
		SetVariable("elf32-loader.filename", ((string)(*cmd_args)[0]).c_str());
		SetVariable("linux-loader.argc", cmd_args_length);
		
		unsigned int i;
		for(i = 0; i < cmd_args_length; i++)
		{
			std::stringstream sstr;
			sstr << "linux-loader.argv[" << i << "]";
			SetVariable(sstr.str().c_str(), ((string)(*cmd_args)[i]).c_str());
		}
	}
	else
	{
		cerr << "WARNING! command line is empty: it may result in an error unless you provide a value for elf32-loader.filename, linux-loader.argc and linux-loader.argv" << endl;
	}

	return unisim::kernel::Simulator::Setup();
}

void Simulator::Stop(Object *object, int _exit_status)
{
	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}
#ifdef DEBUG_PPC440EMU
	std::cerr << "Call stack:" << std::endl;
	std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
#endif
	std::cerr << "Program exited with status " << exit_status << std::endl;
	sc_stop();
	wait();
}

int Simulator::GetExitStatus() const
{
	return exit_status;
}

int sc_main(int argc, char *argv[])
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
			cerr << "Starting simulation at user privilege level (Linux system call translation mode)" << endl;
			simulator->Run();
			break;
		case unisim::kernel::Simulator::ST_ERROR:
			cerr << "Can't start simulation because of previous errors" << endl;
			break;
	}

	int exit_status = simulator->GetExitStatus();
	delete simulator;
#ifdef WIN32
	// releases the winsock2 resources
	WSACleanup();
#endif

	return exit_status;
}
