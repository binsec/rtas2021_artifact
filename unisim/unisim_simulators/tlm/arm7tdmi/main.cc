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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#include <iostream>
#include <getopt.h>
#include <stdlib.h>
#include <signal.h>

#include "unisim/kernel/kernel.hh"

#include "unisim/component/cxx/processor/arm/config.hh"
#include "unisim/component/tlm/processor/arm/arm.hh"
#include "unisim/component/tlm/memory/ram/memory.hh"
#include "unisim/component/tlm/bridge/simple_fsb_to_mem/config.hh"
#include "unisim/component/tlm/bridge/simple_fsb_to_mem/bridge.hh"
#include "unisim/component/tlm/debug/transaction_spy.hh"
#include "unisim/component/tlm/message/simple_fsb.hh"
#include "unisim/component/tlm/message/memory.hh"

#include "unisim/service/time/sc_time/time.hh"
#include "unisim/service/time/host_time/time.hh"
#include "unisim/service/debug/gdb_server/gdb_server.hh"
#include "unisim/service/debug/inline_debugger/inline_debugger.hh"
#include "unisim/service/loader/elf_loader/elf32_loader.hh"


#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#endif

#ifdef ARM7TDMI_DEBUG
	typedef unisim::component::cxx::processor::arm::ARM7TDMI_DebugConfig CPU_CONFIG;
#else
	typedef unisim::component::cxx::processor::arm::ARM7TDMI_Config CPU_CONFIG;
#endif
	
typedef unisim::component::tlm::bridge::simple_fsb_to_mem::Addr32BurstSize32_Config BRIDGE_CONFIG;

//static const bool DEBUG_INFORMATION = true;

bool debug_enabled;

void EnableDebug() {
	debug_enabled = true;
}

void DisableDebug() {
	debug_enabled = false;
}

void SigIntHandler(int signum) {
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	sc_stop();
}


using namespace std;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::garbage_collector::GarbageCollector;
using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::service::loader::elf_loader::Elf32Loader;
using unisim::service::logger::LoggerServer;
using unisim::service::time::sc_time::ScTime;
using unisim::service::time::host_time::HostTime;
using unisim::kernel::ServiceManager;
using unisim::kernel::VariableBase;

void help(char *prog_name) {
	cerr << "Usage: " << prog_name << " [<options>] <binary to simulate>" << endl << endl;
	cerr << "Options:" << endl;
	cerr << " --help" << endl;
	cerr << " -h" << endl;
	cerr << "            displays this help" << endl << endl;
	cerr << " --get-variables <xml file>" << endl;
	cerr << " -v <xml file>" << endl;
	cerr << "            get the simulation default configuration variables on a xml file" << endl << endl;
	cerr << " --config <xml file>" << endl;
	cerr << " -c <xml file>" << endl;
	cerr << "            configures the simulator with the given xml configuration file" << endl << endl;
	cerr << " --get-config <xml file>" << endl;
	cerr << " -g <xml file>" << endl;
	cerr << "            get the simulator default configuration xml file (you can use it to create your own configuration)" << endl << endl;
	cerr << " --logger" << endl;
	cerr << " -l" << endl;
	cerr << "            activate the logger" << endl << endl;
	cerr << " --xml-gdb <file>" << endl;
	cerr << " -x <file>" << endl;
	cerr << "            processor xml description file for gdb" << endl << endl;
	cerr << " --gdb-server <port_number>" << endl;
	cerr << " -d <port_number>" << endl;
	cerr << "            activate the gdb server and use the given port" << endl << endl;
	cerr << " --inline-debugger" << endl;
	cerr << " -i" << endl;
	cerr << "            activate the inline debugger (only active if logger option used)" << endl << endl;
	cerr << " --message-spy" << endl;
	cerr << " -m" << endl;
	cerr << "            activate message spies" << endl << endl;
}

// Front Side Bus template parameters
typedef CPU_CONFIG::address_t FSB_ADDRESS_TYPE;
typedef CPU_CONFIG::address_t CPU_ADDRESS_TYPE;
typedef CPU_CONFIG::reg_t CPU_REG_TYPE;
const uint32_t FSB_MAX_DATA_SIZE = 32;        // in bytes
const uint32_t FSB_NUM_PROCS = 1;

typedef unisim::component::tlm::message::SimpleFSBRequest<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> BusMsgReqType;
typedef unisim::component::tlm::message::SimpleFSBResponse<FSB_MAX_DATA_SIZE> BusMsgRspType;
typedef unisim::component::tlm::debug::TransactionSpy<BusMsgReqType, BusMsgRspType> BusMsgSpyType;
typedef unisim::component::tlm::message::MemoryRequest<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> MemMsgReqType;
typedef unisim::component::tlm::message::MemoryResponse<FSB_MAX_DATA_SIZE> MemMsgRspType;
typedef unisim::component::tlm::debug::TransactionSpy<MemMsgReqType, MemMsgRspType> MemMsgSpyType;

int main(int argc, char *argv[], char **envp) {

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

	static struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"get-variables", required_argument, 0, 'v'},
		{"get-config", required_argument, 0, 'g'},
		{"config", required_argument, 0, 'c'},
		{"logger", no_argument, 0, 'l'},
		{"xml-gdb", required_argument, 0, 'x'},
		{"gdb-server", required_argument, 0, 'd'},
		{"inline-debugger", no_argument, 0, 'i'},
		{"message-spy", no_argument, 0, 'm'},
		{0, 0, 0, 0}
	};

	char const *set_config_name = "default_parameters.xml";
	char const *get_config_name = "default_parameters.xml";
	char const *get_variables_name = "default_variables.xml";
	char *filename = 0;
	bool get_variables = false;
	bool get_config = false;
	bool set_config = false;
	bool use_logger = false;
	bool use_gdb_server = false;
	char *gdb_xml = 0;
	int gdb_server_port = 0;
	bool use_inline_debugger = false;
	bool use_message_spy = false;
	

	
	// Parse the command line arguments
//	while((c = getopt_long (argc, argv, "dg:a:hi:zeoml:", long_options, 0)) != -1) {
	int c;
	while((c = getopt_long (argc, argv, "hv:g:c:ld:x:im", long_options, 0)) != -1) {
		switch(c) {
		case 'h':
			help(argv[0]);
			return 0;
			break;
		case 'v':
			get_variables_name = optarg;
			get_variables = true;
			break;
		case 'g':
			get_config_name = optarg;
			get_config = true;
			break;
		case 'x':
			gdb_xml = optarg;
			break;
		case 'c':
			set_config_name = optarg;
			set_config = true;
			break;
		case 'l':
			use_logger = true;
			break;
		case 'd':
			gdb_server_port = atoi(optarg);
			use_gdb_server = true;
			break;
		case 'i':
			use_inline_debugger = true;
			break;
		case 'm':
			use_message_spy = true;
			break;
		}
	}

	if(optind >= argc) {
		help(argv[0]);
		return 0;
	}

	filename = argv[optind];

	if(!use_logger) {
		use_message_spy = false;
	}
	

	// Logger
	LoggerServer *logger = 0;
	if(use_logger)
		logger = new LoggerServer("logger");
	
	// Time
	ScTime *time = new ScTime("time");
	HostTime *host_time = new HostTime("host-time");
	
	if(use_logger)
		logger->time_import >> time->time_export;

	Elf32Loader *elf32_loader = 0;
	unisim::component::tlm::memory::ram::Memory<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> *memory = 
		new unisim::component::tlm::memory::ram::Memory<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE>("memory");
	GDBServer<CPU_ADDRESS_TYPE> *gdb_server = 
		use_gdb_server ? new GDBServer<CPU_ADDRESS_TYPE>("gdb-server") : 0;
	InlineDebugger<CPU_ADDRESS_TYPE> *inline_debugger = 
		use_inline_debugger ? new InlineDebugger<CPU_ADDRESS_TYPE>("inline-debugger") : 0;
	unisim::component::tlm::bridge::simple_fsb_to_mem::Bridge<BRIDGE_CONFIG> *bridge =
		new unisim::component::tlm::bridge::simple_fsb_to_mem::Bridge<BRIDGE_CONFIG>("bridge");
	unisim::component::tlm::processor::arm::ARM<CPU_CONFIG> *cpu =
		new unisim::component::tlm::processor::arm::ARM<CPU_CONFIG>("cpu"); 

	BusMsgSpyType *bus_msg_spy = NULL;
	MemMsgSpyType *mem_msg_spy = NULL;
	if(use_message_spy) {
		bus_msg_spy = new BusMsgSpyType("bus-msg-spy");
		mem_msg_spy = new MemMsgSpyType("mem-msg-spy");
	}
	// In Linux mode, the system is not entirely simulated.
	// This mode allows to run Linux applications without simulating all the peripherals.
	// Every Linux system calls are caught and translated in host system calls.
	
	// Instanciate an ELF32 loader
	elf32_loader = new Elf32Loader("elf32-loader");
	

	

	// Connect the CPU to the Front Side Bus
	if(use_message_spy) {
		cpu->master_port(bus_msg_spy->slave_port);
		bus_msg_spy->master_port(bridge->slave_port);
		bridge->master_port(mem_msg_spy->slave_port);
		mem_msg_spy->master_port(memory->slave_port);
	} else {
		cpu->master_port(bridge->slave_port);
		bridge->master_port(memory->slave_port);
	}
	cpu->memory_import >> bridge->memory_export;

	if(use_logger) {
		unsigned int logger_index = 0;
		bridge->logger_import >> *logger->logger_export[logger_index++];
		if(use_message_spy) {
			bus_msg_spy->logger_import >> *logger->logger_export[logger_index++];
			mem_msg_spy->logger_import >> *logger->logger_export[logger_index++];
		}
	}
	
	if(use_inline_debugger) {
		cpu->debug_yielding_import >> inline_debugger->debug_yielding_export;
		cpu->memory_access_reporting_import >> inline_debugger->memory_access_reporting_export;
		inline_debugger->disasm_import >> cpu->disasm_export;
		inline_debugger->memory_import >> cpu->memory_export;
		inline_debugger->registers_import >> cpu->registers_export;
	} else if(use_gdb_server) {
		// Connect gdb-server to CPU
		cpu->debug_yielding_import >> gdb_server->debug_yielding_export;
		cpu->memory_access_reporting_import >> gdb_server->memory_access_reporting_export;
		gdb_server->memory_import >> cpu->memory_export;
		gdb_server->registers_import >> cpu->registers_export;
		gdb_server->memory_access_reporting_control_import >> cpu->memory_access_reporting_control_export;
	}


	// Connect everything
	elf32_loader->memory_import >> memory->memory_export;

	cpu->symbol_table_lookup_import >> elf32_loader->symbol_table_lookup_export;
	bridge->memory_import >> memory->memory_export;

	if(use_inline_debugger) {
		inline_debugger->symbol_table_lookup_import >> 
			elf32_loader->symbol_table_lookup_export;
	}
	
	/* set the default names for the different message spies */
	if(use_message_spy) {
		(*bus_msg_spy)["source_module_name"] = cpu->name();
		(*bus_msg_spy)["source_port_name"] = cpu->master_port.name();
		(*bus_msg_spy)["target_module_name"] = bridge->name();
		(*bus_msg_spy)["target_port_name"] = bridge->slave_port.name();
		(*mem_msg_spy)["source_module_name"] = bridge->name();
		(*mem_msg_spy)["source_port_name"] = bridge->master_port.name();
		(*mem_msg_spy)["target_module_name"] = memory->name();
		(*mem_msg_spy)["target_port_name"] = memory->slave_port.name();
	}

#ifdef DEBUG_SERVICE
	ServiceManager::Dump(cerr);
#endif

	if(get_variables)
		ServiceManager::XmlfyVariables(get_variables_name);
	if(get_config)
		ServiceManager::XmlfyParameters(get_config_name);
	if(!set_config) {
		if(!get_config || !get_variables) help(argv[0]);
		return 0;
	}

	ServiceManager::LoadXmlParameters(set_config_name);
	if(use_gdb_server) {
		cerr << "gdb_server_port = " << gdb_server_port << endl;
		VariableBase *var =	ServiceManager::GetParameter("gdb-server.tcp-port");
		*var = gdb_server_port;
		if(gdb_xml != 0) {
			cerr << "gdb_xml = " << gdb_xml << endl;
			var = ServiceManager::GetParameter("gdb-server.architecture-description-filename");
			*var = gdb_xml;
		}
	}
	{
		cerr << "filename = " << filename << endl;
		VariableBase *var = ServiceManager::GetParameter("elf32-loader.filename");
		*var = filename;
	}
	
	if(ServiceManager::Setup())
	{
		cerr << "Starting simulation at system privilege level" << endl;

		double time_start = host_time->GetTime();

		EnableDebug();
		void (*prev_sig_int_handler)(int);

		if(!use_inline_debugger)
			prev_sig_int_handler = signal(SIGINT, SigIntHandler);

		try
		{
			sc_start();
		}
		catch(std::runtime_error& e)
		{
			cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << endl;
			cerr << e.what() << endl;
		}

		if(!use_inline_debugger)
			signal(SIGINT, prev_sig_int_handler);

		cerr << "Simulation finished" << endl;
		cerr << "Simulation statistics:" << endl;

		double time_stop = host_time->GetTime();
		double spent_time = time_stop - time_start;

		cerr << "simulation time: " << spent_time << " seconds" << endl;
		cerr << "simulated time : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << endl;
		cerr << "simulated instructions : " << cpu->GetInstructionCounter() << " instructions" << endl;
		cerr << "host simulation speed: " << ((double) cpu->GetInstructionCounter() / spent_time / 1000000.0) << " MIPS" << endl;
		cerr << "time dilatation: " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << endl;
	}
	else
	{
		cerr << "Can't start simulation because of previous errors" << endl;
	}


	if(elf32_loader) delete elf32_loader;
	if(memory) delete memory;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(bridge) delete bridge;
	if(cpu) delete cpu;
	if(time) delete time;
	if(logger) delete logger;
	if(bus_msg_spy) delete bus_msg_spy;
	if(mem_msg_spy) delete mem_msg_spy;

#ifdef WIN32
	// releases the winsock2 resources
	WSACleanup();
#endif

	return 0;
}
