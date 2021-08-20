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

#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/debugger/debugger.tcc>
#include <unisim/service/loader/pmac_linux_kernel_loader/pmac_linux_kernel_loader.hh>
#include <unisim/service/translator/memory_address/memory/translator.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/sdl/sdl.hh>
#include <unisim/service/debug/profiler/profiler.hh>
#include <unisim/service/http_server/http_server.hh>
#include <unisim/service/instrumenter/instrumenter.hh>

#include <unisim/component/cxx/memory/flash/am29/am29lv800b_config.hh>
#include <unisim/component/cxx/pci/types.hh>
#include <unisim/component/tlm/processor/powerpc/e600/mpc7447a/cpu.hh>
#include <unisim/component/tlm/memory/ram/memory.hh>
#include <unisim/component/tlm/memory/flash/am29/am29.hh>
#include <unisim/component/tlm/pci/video/display.hh>
#include <unisim/component/tlm/fsb/snooping_bus/bus.hh>
#include <unisim/component/tlm/chipset/mpc107/mpc107.hh>
#include <unisim/component/tlm/pci/bus/bus.hh>
#include <unisim/component/tlm/pci/ide/pci_ide_module.hh>
#include <unisim/component/tlm/pci/macio/heathrow.hh>
#include <unisim/component/tlm/bridge/pci_isa/bridge.hh>
#include <unisim/component/tlm/isa/i8042/i8042.hh>
#include <unisim/component/tlm/debug/transaction_spy.hh>

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

#ifdef DEBUG_PPCEMU_SYSTEM
static const bool DEBUG_INFORMATION = true;
#else
static const bool DEBUG_INFORMATION = false;
#endif

class Simulator : public unisim::kernel::Simulator
{
public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	virtual unisim::kernel::Simulator::SetupStatus Setup();
	void Run();
	virtual void Stop(unisim::kernel::Object *object, int exit_status, bool asynchronous = false);
	int GetExitStatus() const;
	virtual void SigInt();
protected:
private:
	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================

	// Front Side Bus template parameters
	typedef uint64_t FSB_ADDRESS_TYPE;
	typedef uint32_t CPU_ADDRESS_TYPE;
	static const uint32_t FSB_MAX_DATA_SIZE = 32;        // in bytes
	static const uint32_t FSB_NUM_PROCS = 1;

	// Debugger configuration
	struct DEBUGGER_CONFIG
	{
		typedef CPU_ADDRESS_TYPE ADDRESS;
		static const unsigned int NUM_PROCESSORS = 1;
		/* gdb_server, inline_debugger, and profiler */
		static const unsigned int MAX_FRONT_ENDS = 3;
	};

	// PCI Bus template parameters
	typedef unisim::component::cxx::pci::pci32_address_t PCI_ADDRESS_TYPE;
	static const uint32_t PCI_MAX_DATA_SIZE = 32;        // in bytes
	static const unsigned int PCI_NUM_MASTERS = 2;
	static const unsigned int PCI_NUM_TARGETS = 5;
	static const unsigned int PCI_NUM_MAPPINGS = 10;

	// ISA Bus template parameters
	static const uint32_t ISA_MAX_DATA_SIZE = PCI_MAX_DATA_SIZE;

	// PCI device numbers
	static const unsigned int PCI_MPC107_DEV_NUM = 0;
	static const unsigned int PCI_HEATHROW_DEV_NUM = 1;
	static const unsigned int PCI_IDE_DEV_NUM = 2;
	static const unsigned int PCI_DISPLAY_DEV_NUM = 3;
	static const unsigned int PCI_ISA_BRIDGE_DEV_NUM = 4;

	// PCI target port numbers
	static const unsigned int PCI_MPC107_SLAVE_PORT = 0;
	static const unsigned int PCI_HEATHROW_SLAVE_PORT = 1;
	static const unsigned int PCI_IDE_SLAVE_PORT = 2;
	static const unsigned int PCI_DISPLAY_SLAVE_PORT = 3;
	static const unsigned int PCI_ISA_BRIDGE_SLAVE_PORT = 4;

	// PCI master port numbers
	static const unsigned int PCI_MPC107_MASTER_PORT = 0;
	static const unsigned int PCI_IDE_MASTER_PORT = 1;

	// Heathrow PIC interrupts
	static const unsigned int PCI_IDE_IRQ = 47;
	static const unsigned int I8042_KBD_IRQ = 1;
	static const unsigned int I8042_AUX_IRQ = 12;

	// the maximum number of transaction spies (per type of message)
	static const unsigned int MAX_BUS_TRANSACTION_SPY = 4;
	static const unsigned int MAX_MEM_TRANSACTION_SPY = 3;
	static const unsigned int MAX_PCI_TRANSACTION_SPY = 7;
	static const unsigned int MAX_IRQ_TRANSACTION_SPY = 4;

	// Flash memory
	static const uint32_t FLASH_BYTESIZE = 4 * unisim::component::cxx::memory::flash::am29::M; // 4 MB
	static const uint32_t FLASH_IO_WIDTH = 8; // 64 bits
	typedef unisim::component::cxx::memory::flash::am29::AM29LV800BTConfig FLASH_CONFIG;

	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm::fsb::snooping_bus::Bus<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, 1> FRONT_SIDE_BUS;
	typedef unisim::component::tlm::memory::ram::Memory<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> MEMORY;
	typedef unisim::component::tlm::memory::flash::am29::AM29<FLASH_CONFIG, FLASH_BYTESIZE, FLASH_IO_WIDTH, FSB_MAX_DATA_SIZE> FLASH;
	typedef unisim::component::tlm::pci::bus::Bus<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE, PCI_NUM_MASTERS, PCI_NUM_TARGETS, PCI_NUM_MAPPINGS, DEBUG_INFORMATION> PCI_BUS;
	typedef unisim::component::tlm::processor::powerpc::e600::mpc7447a::CPU CPU;
	typedef unisim::component::tlm::chipset::mpc107::MPC107<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE, DEBUG_INFORMATION> MPC107;
	typedef unisim::component::tlm::pci::ide::PCIDevIde<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE> PCI_IDE;
	typedef unisim::component::tlm::pci::macio::Heathrow<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE> HEATHROW;
	typedef unisim::component::tlm::pci::video::Display<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE> PCI_DISPLAY;
	typedef unisim::component::tlm::bridge::pci_isa::Bridge<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE> PCI_ISA_BRIDGE;
	typedef unisim::component::tlm::isa::i8042::I8042<ISA_MAX_DATA_SIZE> I8042;

	//=========================================================================
	//===                      Aliases for services classes                 ===
	//=========================================================================
	
	typedef unisim::service::loader::pmac_linux_kernel_loader::PMACLinuxKernelLoader PMAC_LINUX_KERNEL_LOADER;
	typedef unisim::service::debug::gdb_server::GDBServer<CPU_ADDRESS_TYPE> GDB_SERVER;
	typedef unisim::service::debug::inline_debugger::InlineDebugger<CPU_ADDRESS_TYPE> INLINE_DEBUGGER;
	typedef unisim::service::debug::debugger::Debugger<DEBUGGER_CONFIG> DEBUGGER;
	typedef unisim::service::debug::profiler::Profiler<CPU_ADDRESS_TYPE> PROFILER;
	typedef unisim::service::http_server::HttpServer HTTP_SERVER;
	typedef unisim::service::instrumenter::Instrumenter INSTRUMENTER;
	typedef unisim::kernel::logger::console::Printer LOGGER_CONSOLE_PRINTER;
	typedef unisim::kernel::logger::text_file::Writer LOGGER_TEXT_FILE_WRITER;
	typedef unisim::kernel::logger::http::Writer LOGGER_HTTP_WRITER;
	typedef unisim::kernel::logger::xml_file::Writer LOGGER_XML_FILE_WRITER;
	typedef unisim::kernel::logger::netstream::Writer LOGGER_NETSTREAM_WRITER;
	
	//=========================================================================
	//===               Aliases for transaction Spies classes               ===
	//=========================================================================

	typedef unisim::component::tlm::fsb::snooping_bus::Bus<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, 1>::ReqType BusMsgReqType;
	typedef unisim::component::tlm::fsb::snooping_bus::Bus<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE, 1>::RspType BusMsgRspType;
	typedef unisim::component::tlm::debug::TransactionSpy<BusMsgReqType, BusMsgRspType> BusMsgSpyType;
	typedef unisim::component::tlm::message::MemoryRequest<FSB_ADDRESS_TYPE, FSB_MAX_DATA_SIZE> MemMsgReqType;
	typedef unisim::component::tlm::message::MemoryResponse<FSB_MAX_DATA_SIZE> MemMsgRspType;
	typedef unisim::component::tlm::debug::TransactionSpy<MemMsgReqType, MemMsgRspType> MemMsgSpyType;
	typedef unisim::component::tlm::pci::bus::Bus<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE, PCI_NUM_MASTERS, PCI_NUM_TARGETS, PCI_NUM_MAPPINGS, DEBUG_INFORMATION>::ReqType PCIMsgReqType;
	typedef unisim::component::tlm::pci::bus::Bus<PCI_ADDRESS_TYPE, PCI_MAX_DATA_SIZE, PCI_NUM_MASTERS, PCI_NUM_TARGETS, PCI_NUM_MAPPINGS, DEBUG_INFORMATION>::RspType PCIMsgRspType;
	typedef unisim::component::tlm::debug::TransactionSpy<PCIMsgReqType, PCIMsgRspType> PCIMsgSpyType;
	typedef unisim::component::tlm::message::InterruptRequest IRQReqSpyType;
	typedef unisim::component::tlm::debug::TransactionSpy<IRQReqSpyType> IRQMsgSpyType;

	//=========================================================================
	//===                             Components                            ===
	//=========================================================================
	//  - PowerPC processor
	CPU *cpu;
	//  - Front side bus
	FRONT_SIDE_BUS *bus;
	//  - MPC107 chipset
	MPC107 *mpc107;
	//  - RAM
	MEMORY *memory;
	//  - EROM
	MEMORY *erom;
	//  - Flash memory
	FLASH *flash;
	//  - PCI Bus
	PCI_BUS *pci_bus;
	//  - PCI PIIX4 IDE controller
	PCI_IDE *pci_ide;
	//  - PCI Heathrow PIC controller
	HEATHROW *heathrow;
	//  - PCI Display (just a frame buffer for now)
	PCI_DISPLAY *pci_display;
	//  - PCI to ISA Bridge
	PCI_ISA_BRIDGE *pci_isa_bridge;
	//  - i8042 keyboard controller
	I8042 *i8042;
	
	//=========================================================================
	//===            Debugging stuff: Transaction spy instantiations        ===
	//=========================================================================
	BusMsgSpyType *bus_msg_spy[MAX_BUS_TRANSACTION_SPY];
	MemMsgSpyType *mem_msg_spy[MAX_MEM_TRANSACTION_SPY];
	PCIMsgSpyType *pci_msg_spy[MAX_PCI_TRANSACTION_SPY];
	IRQMsgSpyType *irq_msg_spy[MAX_IRQ_TRANSACTION_SPY];

	//=========================================================================
	//===                            Services                               ===
	//=========================================================================
	//  - Simple Direct Media Layer (www.libsdl.org)
	unisim::service::sdl::SDL<PCI_ADDRESS_TYPE> *sdl;
	//  - PowerMac Linux kernel loader
	//    A Linux kernel loader acting as a firmware and a bootloader of a real PowerMac machine
	PMAC_LINUX_KERNEL_LOADER *kloader;
	//  - GDB server
	GDB_SERVER *gdb_server;
	//  - Inline debugger
	INLINE_DEBUGGER *inline_debugger;
	//  - debugger
	DEBUGGER *debugger;
	//  - profiler
	PROFILER *profiler;
	// - Instrumenter
	INSTRUMENTER *instrumenter;
	//  - HTTP server
	HTTP_SERVER *http_server;
	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *sim_time;
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time;
	//  - memory address translator from effective address to physical address
	unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE> *memory_effective_to_physical_address_translator;
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
	bool message_spy;
	unisim::kernel::variable::Parameter<bool> param_enable_gdb_server;
	unisim::kernel::variable::Parameter<bool> param_enable_inline_debugger;
	unisim::kernel::variable::Parameter<bool> param_enable_profiler;
	unisim::kernel::variable::Parameter<bool> param_message_spy;

	bool stop_called;
	int exit_status;
	static void LoadBuiltInConfig(unisim::kernel::Simulator *simulator);
};

Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, bus(0)
	, mpc107(0)
	, memory(0)
	, erom(0)
	, flash(0)
	, pci_bus(0)
	, pci_ide(0)
	, heathrow(0)
	, pci_display(0)
	, pci_isa_bridge(0)
	, i8042(0)
	, sdl(0)
	, kloader(0)
	, gdb_server(0)
	, inline_debugger(0)
	, debugger(0)
	, profiler(0)
	, instrumenter(0)
	, http_server(0)
	, sim_time(0)
	, host_time(0)
	, memory_effective_to_physical_address_translator(0)
	, logger_console_printer(0)
	, logger_text_file_writer(0)
	, logger_http_writer(0)
	, logger_xml_file_writer(0)
	, logger_netstream_writer(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, enable_profiler(false)
	, message_spy(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, param_enable_profiler("enable-profiler", 0, enable_profiler, "Enable/Disable profiler")
	, param_message_spy("message-spy", 0, message_spy, "Enable/Disable message spies instantiation")
	, stop_called(false)
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
	//===                     Instrumenter instantiation                    ===
	//=========================================================================
	instrumenter = new INSTRUMENTER("instrumenter");

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	cpu = new CPU("cpu");
	//  - Front side bus
	bus = new FRONT_SIDE_BUS("bus");
	//  - MPC107 chipset
	mpc107 = new MPC107("mpc107");
	//  - RAM
	memory = new MEMORY("memory");
	//  - EROM
	erom = new MEMORY("erom");
	//  - Flash memory
	flash = new FLASH("flash");
	//  - PCI Bus
	pci_bus = new PCI_BUS("pci-bus");
	//  - PCI PIIX4 IDE controller
	pci_ide = new PCI_IDE("pci-ide");
	//  - PCI Heathrow PIC controller
	heathrow = new HEATHROW("heathrow");
	//  - PCI Display (just a frame buffer for now)
	pci_display = new PCI_DISPLAY("pci-display");
	//  - PCI to ISA Bridge
	pci_isa_bridge = new PCI_ISA_BRIDGE("pci-isa-bridge");
	//  - i8042 keyboard controller
	i8042 = new I8042("i8042");

	//=========================================================================
	//===            Debugging stuff: Transaction spy instantiations        ===
	//=========================================================================

	if(message_spy)
	{
		for(unsigned int i = 0; i < MAX_BUS_TRANSACTION_SPY; i++)
		{
			std::stringstream sstr;
			sstr << "bus_msg_spy[" << i << "]";
			std::string name = sstr.str();
			bus_msg_spy[i] = new BusMsgSpyType(name.c_str());
		}
		for(unsigned int i = 0; i < MAX_MEM_TRANSACTION_SPY; i++)
		{
			std::stringstream sstr;
			sstr << "mem_msg_spy[" << i << "]";
			std::string name = sstr.str();
			mem_msg_spy[i] = new MemMsgSpyType(name.c_str());
		}
		for(unsigned int i = 0; i < MAX_PCI_TRANSACTION_SPY; i++)
		{
			std::stringstream sstr;
			sstr << "pci_msg_spy[" << i << "]";
			std::string name = sstr.str();
			pci_msg_spy[i] = new PCIMsgSpyType(name.c_str());
		}
		for(unsigned int i = 0; i < MAX_IRQ_TRANSACTION_SPY; i++)
		{
			std::stringstream sstr;
			sstr << "irq_msg_spy[" << i << "]";
			std::string name = sstr.str();
			irq_msg_spy[i] = new IRQMsgSpyType(name.c_str());
		}
	}
	else
	{
		for(unsigned int i = 0; i < MAX_BUS_TRANSACTION_SPY; i++) bus_msg_spy[i] = 0;
		for(unsigned int i = 0; i < MAX_MEM_TRANSACTION_SPY; i++) mem_msg_spy[i] = 0;
		for(unsigned int i = 0; i < MAX_PCI_TRANSACTION_SPY; i++) pci_msg_spy[i] = 0;
		for(unsigned int i = 0; i < MAX_IRQ_TRANSACTION_SPY; i++) irq_msg_spy[i] = 0;
	}

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - Simple Direct Media Layer (www.libsdl.org)
	sdl = new unisim::service::sdl::SDL<PCI_ADDRESS_TYPE>("sdl");
	//  - PowerMac Linux kernel loader
	//    A Linux kernel loader acting as a firmware and a bootloader of a real PowerMac machine
	kloader = new PMAC_LINUX_KERNEL_LOADER("pmac-linux-kernel-loader");
	//  - GDB server
	gdb_server = (enable_gdb_server) ? new GDB_SERVER("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = (enable_inline_debugger) ? new INLINE_DEBUGGER("inline-debugger") : 0;
	//  - debugger
	debugger = (enable_inline_debugger || enable_gdb_server || enable_profiler) ? new DEBUGGER("debugger") : 0;
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - memory address translator from effective address to physical address
	memory_effective_to_physical_address_translator = new unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE>("memory-effective-to-physical-address-translator");
	//  - profiler
	profiler = enable_profiler ? new PROFILER("profiler") : 0;
	//  - HTTP server
	http_server = new HTTP_SERVER("http-server");
	
	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	if(message_spy)
	{
		unsigned bus_msg_spy_index = 0;
		unsigned mem_msg_spy_index = 0;
		unsigned pci_msg_spy_index = 0;
		unsigned irq_msg_spy_index = 0;

		cpu->bus_port(bus_msg_spy[bus_msg_spy_index]->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["source_module_name"] = cpu->sc_core::sc_object::name();
		(*bus_msg_spy[bus_msg_spy_index])["source_port_name"] = cpu->bus_port.name();
		bus_msg_spy[bus_msg_spy_index]->master_port(*bus->inport[0]);
		(*bus_msg_spy[bus_msg_spy_index])["target_module_name"] = bus->sc_core::sc_object::name();
		(*bus_msg_spy[bus_msg_spy_index])["target_port_name"] = bus->inport[0]->name();
		bus_msg_spy_index++;

		(*bus->outport[0])(bus_msg_spy[bus_msg_spy_index]->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["source_module_name"] = bus->sc_core::sc_object::name();
		(*bus_msg_spy[bus_msg_spy_index])["source_port_name"] = bus->outport[0]->name();
		bus_msg_spy[bus_msg_spy_index]->master_port(cpu->snoop_port);
		(*bus_msg_spy[bus_msg_spy_index])["target_module_name"] = cpu->sc_core::sc_object::name();
		(*bus_msg_spy[bus_msg_spy_index])["target_port_name"] = cpu->snoop_port.name();
		bus_msg_spy_index++;

		(*bus->chipset_outport)(bus_msg_spy[bus_msg_spy_index]->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["source_module_name"] = bus->sc_core::sc_object::name();
		(*bus_msg_spy[bus_msg_spy_index])["source_port_name"] = bus->chipset_outport->name();
		bus_msg_spy[bus_msg_spy_index]->master_port(mpc107->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["target_module_name"] = mpc107->sc_core::sc_object::name();
		(*bus_msg_spy[bus_msg_spy_index])["target_port_name"] = mpc107->slave_port.name();
		bus_msg_spy_index++;

		mpc107->master_port(bus_msg_spy[bus_msg_spy_index]->slave_port);
		(*bus_msg_spy[bus_msg_spy_index])["source_module_name"] = mpc107->sc_core::sc_object::name();
		(*bus_msg_spy[bus_msg_spy_index])["source_port_name"] = mpc107->master_port.name();
		bus_msg_spy[bus_msg_spy_index]->master_port(*bus->chipset_inport);
		(*bus_msg_spy[bus_msg_spy_index])["target_module_name"] = bus->sc_core::sc_object::name();
		(*bus_msg_spy[bus_msg_spy_index])["target_port_name"] = bus->chipset_inport->name();
		bus_msg_spy_index++;

		mpc107->ram_master_port(mem_msg_spy[mem_msg_spy_index]->slave_port);
		(*mem_msg_spy[mem_msg_spy_index])["source_module_name"] = mpc107->sc_core::sc_object::name();
		(*mem_msg_spy[mem_msg_spy_index])["source_port_name"] = mpc107->ram_master_port.name();
		mem_msg_spy[mem_msg_spy_index]->master_port(memory->slave_port);
		(*mem_msg_spy[mem_msg_spy_index])["target_module_name"] = memory->sc_core::sc_object::name();
		(*mem_msg_spy[mem_msg_spy_index])["target_port_name"] = memory->slave_port.name();
		mem_msg_spy_index++;

		mpc107->rom_master_port(mem_msg_spy[mem_msg_spy_index]->slave_port);
		(*mem_msg_spy[mem_msg_spy_index])["source_module_name"] = mpc107->sc_core::sc_object::name();
		(*mem_msg_spy[mem_msg_spy_index])["source_port_name"] = mpc107->rom_master_port.name();
		mem_msg_spy[mem_msg_spy_index]->master_port(flash->slave_port);
		(*mem_msg_spy[mem_msg_spy_index])["target_module_name"] = flash->sc_core::sc_object::name();
		(*mem_msg_spy[mem_msg_spy_index])["target_port_name"] = flash->slave_port.name();
		mem_msg_spy_index++;

		mpc107->erom_master_port(mem_msg_spy[mem_msg_spy_index]->slave_port);
		(*mem_msg_spy[mem_msg_spy_index])["source_module_name"] = mpc107->sc_core::sc_object::name();
		(*mem_msg_spy[mem_msg_spy_index])["source_port_name"] = mpc107->erom_master_port.name();
		mem_msg_spy[mem_msg_spy_index]->master_port(erom->slave_port);
		(*mem_msg_spy[mem_msg_spy_index])["target_module_name"] = erom->sc_core::sc_object::name();
		(*mem_msg_spy[mem_msg_spy_index])["target_port_name"] = erom->slave_port.name();
		mem_msg_spy_index++;
		
		mpc107->pci_master_port(pci_msg_spy[pci_msg_spy_index]->slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["source_module_name"] = mpc107->sc_core::sc_object::name();
		(*pci_msg_spy[pci_msg_spy_index])["source_port_name"] = mpc107->pci_master_port.name();
		pci_msg_spy[pci_msg_spy_index]->master_port(*pci_bus->input_port[PCI_MPC107_MASTER_PORT]);
		(*pci_msg_spy[pci_msg_spy_index])["target_module_name"] = pci_bus->sc_core::sc_object::name();
		(*pci_msg_spy[pci_msg_spy_index])["target_port_name"] = pci_bus->input_port[PCI_MPC107_MASTER_PORT]->name();
		pci_msg_spy_index++;

		(*pci_bus->output_port[PCI_MPC107_SLAVE_PORT])(pci_msg_spy[pci_msg_spy_index]->slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["source_module_name"] = pci_bus->sc_core::sc_object::name();
		(*pci_msg_spy[pci_msg_spy_index])["source_port_name"] = pci_bus->output_port[PCI_MPC107_SLAVE_PORT]->name();
		pci_msg_spy[pci_msg_spy_index]->master_port(mpc107->pci_slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["target_module_name"] = mpc107->sc_core::sc_object::name();
		(*pci_msg_spy[pci_msg_spy_index])["target_port_name"] = mpc107->pci_slave_port.name();
		pci_msg_spy_index++;

		(*pci_bus->output_port[PCI_HEATHROW_SLAVE_PORT])(pci_msg_spy[pci_msg_spy_index]->slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["source_module_name"] = pci_bus->sc_core::sc_object::name();
		(*pci_msg_spy[pci_msg_spy_index])["source_port_name"] = pci_bus->output_port[PCI_HEATHROW_SLAVE_PORT]->name();
		pci_msg_spy[pci_msg_spy_index]->master_port(heathrow->bus_port);
		(*pci_msg_spy[pci_msg_spy_index])["target_module_name"] = heathrow->sc_core::sc_object::name();
		(*pci_msg_spy[pci_msg_spy_index])["target_port_name"] = heathrow->bus_port.name();
		pci_msg_spy_index++;

		(*pci_bus->output_port[PCI_IDE_SLAVE_PORT])(pci_msg_spy[pci_msg_spy_index]->slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["source_module_name"] = pci_bus->sc_core::sc_object::name();
		(*pci_msg_spy[pci_msg_spy_index])["source_port_name"] = pci_bus->output_port[PCI_IDE_SLAVE_PORT]->name();
		pci_msg_spy[pci_msg_spy_index]->master_port(pci_ide->input_port);
		(*pci_msg_spy[pci_msg_spy_index])["target_module_name"] = pci_ide->sc_core::sc_object::name();
		(*pci_msg_spy[pci_msg_spy_index])["target_port_name"] = pci_ide->input_port.name();
		pci_msg_spy_index++;

		(*pci_bus->output_port[PCI_DISPLAY_SLAVE_PORT])(pci_msg_spy[pci_msg_spy_index]->slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["source_module_name"] = pci_bus->sc_core::sc_object::name();
		(*pci_msg_spy[pci_msg_spy_index])["source_port_name"] = pci_bus->output_port[PCI_DISPLAY_SLAVE_PORT]->name();
		pci_msg_spy[pci_msg_spy_index]->master_port(pci_display->bus_port);
		(*pci_msg_spy[pci_msg_spy_index])["target_module_name"] = pci_display->sc_core::sc_object::name();
		(*pci_msg_spy[pci_msg_spy_index])["target_port_name"] = pci_display->bus_port.name();
		pci_msg_spy_index++;

		(*pci_bus->output_port[PCI_ISA_BRIDGE_SLAVE_PORT])(pci_msg_spy[pci_msg_spy_index]->slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["source_module_name"] = pci_bus->sc_core::sc_object::name();
		(*pci_msg_spy[pci_msg_spy_index])["source_port_name"] = pci_bus->output_port[PCI_ISA_BRIDGE_SLAVE_PORT]->name();
		pci_msg_spy[pci_msg_spy_index]->master_port(pci_isa_bridge->pci_slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["target_module_name"] = pci_isa_bridge->sc_core::sc_object::name();
		(*pci_msg_spy[pci_msg_spy_index])["target_port_name"] = pci_isa_bridge->pci_slave_port.name();
		pci_msg_spy_index++;

		pci_ide->output_port(pci_msg_spy[pci_msg_spy_index]->slave_port);
		(*pci_msg_spy[pci_msg_spy_index])["source_module_name"] = pci_ide->sc_core::sc_object::name();
		(*pci_msg_spy[pci_msg_spy_index])["source_port_name"] = pci_ide->output_port.name();
		pci_msg_spy[pci_msg_spy_index]->master_port(*pci_bus->input_port[PCI_IDE_MASTER_PORT]);
		(*pci_msg_spy[pci_msg_spy_index])["target_module_name"] = pci_bus->sc_core::sc_object::name();
		(*pci_msg_spy[pci_msg_spy_index])["target_port_name"] = pci_bus->input_port[PCI_IDE_MASTER_PORT]->name();
		pci_msg_spy_index++;
		
		pci_ide->irq_port(irq_msg_spy[irq_msg_spy_index]->slave_port);
		(*irq_msg_spy[irq_msg_spy_index])["source_module_name"] = pci_ide->sc_core::sc_object::name();
		(*irq_msg_spy[irq_msg_spy_index])["source_port_name"] = pci_ide->irq_port.name();
		irq_msg_spy[irq_msg_spy_index]->master_port(*heathrow->irq_port[PCI_IDE_IRQ]);
		(*irq_msg_spy[irq_msg_spy_index])["target_module_name"] = heathrow->sc_core::sc_object::name();
		(*irq_msg_spy[irq_msg_spy_index])["target_port_name"] = heathrow->irq_port[PCI_IDE_IRQ]->name();
		irq_msg_spy_index++;

		heathrow->cpu_irq_port(irq_msg_spy[irq_msg_spy_index]->slave_port);
		(*irq_msg_spy[irq_msg_spy_index])["source_module_name"] = heathrow->sc_core::sc_object::name();
		(*irq_msg_spy[irq_msg_spy_index])["source_port_name"] = heathrow->cpu_irq_port.name();
		irq_msg_spy[irq_msg_spy_index]->master_port(*mpc107->irq_slave_port[0]);
		(*irq_msg_spy[irq_msg_spy_index])["target_module_name"] = mpc107->sc_core::sc_object::name();
		(*irq_msg_spy[irq_msg_spy_index])["target_port_name"] = mpc107->irq_slave_port[0]->name();
		irq_msg_spy_index++;

		mpc107->irq_master_port(irq_msg_spy[irq_msg_spy_index]->slave_port);
		(*irq_msg_spy[irq_msg_spy_index])["source_module_name"] = mpc107->sc_core::sc_object::name();
		(*irq_msg_spy[irq_msg_spy_index])["source_port_name"] = mpc107->irq_master_port.name();
		irq_msg_spy[irq_msg_spy_index]->master_port(cpu->external_interrupt_port);
		(*irq_msg_spy[irq_msg_spy_index])["target_module_name"] = cpu->sc_core::sc_object::name();
		(*irq_msg_spy[irq_msg_spy_index])["target_port_name"] = cpu->external_interrupt_port.name();
		irq_msg_spy_index++;

		mpc107->soft_reset_master_port(irq_msg_spy[irq_msg_spy_index]->slave_port);
		(*irq_msg_spy[irq_msg_spy_index])["source_module_name"] = mpc107->sc_core::sc_object::name();
		(*irq_msg_spy[irq_msg_spy_index])["source_port_name"] = mpc107->soft_reset_master_port.name();
		irq_msg_spy[irq_msg_spy_index]->master_port(cpu->soft_reset_port);
		(*irq_msg_spy[irq_msg_spy_index])["target_module_name"] = cpu->sc_core::sc_object::name();
		(*irq_msg_spy[irq_msg_spy_index])["target_port_name"] = cpu->soft_reset_port.name();
		irq_msg_spy_index++;
	}
	else
	{
		// Connect the CPU to the Front Side Bus
		cpu->bus_port(*bus->inport[0]);
		(*bus->outport[0])(cpu->snoop_port);

		(*bus->chipset_outport)(mpc107->slave_port);
		mpc107->master_port(*bus->chipset_inport);
		mpc107->ram_master_port(memory->slave_port);
		mpc107->rom_master_port(flash->slave_port);
		mpc107->erom_master_port(erom->slave_port);
		mpc107->pci_master_port(*pci_bus->input_port[PCI_MPC107_MASTER_PORT]);
	
		(*pci_bus->output_port[PCI_MPC107_SLAVE_PORT])(mpc107->pci_slave_port);
		(*pci_bus->output_port[PCI_HEATHROW_SLAVE_PORT])(heathrow->bus_port);
		(*pci_bus->output_port[PCI_IDE_SLAVE_PORT])(pci_ide->input_port);
		(*pci_bus->output_port[PCI_DISPLAY_SLAVE_PORT])(pci_display->bus_port);
		(*pci_bus->output_port[PCI_ISA_BRIDGE_SLAVE_PORT])(pci_isa_bridge->pci_slave_port);
		pci_ide->output_port (*pci_bus->input_port[PCI_IDE_MASTER_PORT]);
		pci_ide->irq_port (*heathrow->irq_port[PCI_IDE_IRQ]);
		heathrow->cpu_irq_port(*mpc107->irq_slave_port[0]);
		mpc107->irq_master_port(cpu->external_interrupt_port);
		mpc107->soft_reset_master_port(cpu->soft_reset_port);
	}

	pci_isa_bridge->isa_master_port(i8042->bus_port);
	i8042->kbd_irq_port(*heathrow->irq_port[I8042_KBD_IRQ]);
	i8042->aux_irq_port(*heathrow->irq_port[I8042_AUX_IRQ]);

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> bus->memory_export;
	
	if(debugger)
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
		debugger->blob_import >> kloader->blob_export;
		
		if(inline_debugger)
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
		
		if(gdb_server)
		{
			// gdb-server <-> debugger connections
			*debugger->debug_event_listener_import[1] >> gdb_server->debug_event_listener_export;
			*debugger->debug_yielding_import[1]       >> gdb_server->debug_yielding_export;
			gdb_server->debug_yielding_request_import >> *debugger->debug_yielding_request_export[1];
			gdb_server->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[1];
			gdb_server->memory_import                 >> *debugger->memory_export[1];
			gdb_server->registers_import              >> *debugger->registers_export[1];
		}
		
		if(profiler)
		{
			// profiler <-> debugger connections
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

	memory_effective_to_physical_address_translator->memory_import >> memory->memory_export;
	kloader->memory_import >> memory_effective_to_physical_address_translator->memory_export;
	kloader->registers_import >> cpu->registers_export;
	cpu->symbol_table_lookup_import >> kloader->symbol_table_lookup_export;
	bus->memory_import >> mpc107->memory_export;
	pci_display->video_import >> sdl->video_export;
	sdl->memory_import >> pci_display->memory_export;
	mpc107->ram_import >> memory->memory_export;
	mpc107->rom_import >> flash->memory_export;
	mpc107->erom_import >> erom->memory_export;
	mpc107->pci_import >> *pci_bus->memory_export[PCI_MPC107_MASTER_PORT];
	i8042->keyboard_import >> sdl->keyboard_export;
	i8042->mouse_import >> sdl->mouse_export;
	
	{
		unsigned int i = 0;
		*http_server->http_server_import[i++] >> logger_http_writer->http_server_export;
		*http_server->http_server_import[i++] >> instrumenter->http_server_export;
		if(profiler)
		{
			*http_server->http_server_import[i++] >> profiler->http_server_export;
		}
		*http_server->http_server_import[i++] >> cpu->itlb_http_server_export;
		*http_server->http_server_import[i++] >> cpu->dtlb_http_server_export;
	}

	{
		unsigned int i = 0;
		*http_server->registers_import[i++] >> cpu->registers_export;
	}
}

Simulator::~Simulator()
{

	for(unsigned int i = 0; i < MAX_BUS_TRANSACTION_SPY; i++) if(bus_msg_spy[i]) delete bus_msg_spy[i];
	for(unsigned int i = 0; i < MAX_MEM_TRANSACTION_SPY; i++) if(mem_msg_spy[i]) delete mem_msg_spy[i];
	for(unsigned int i = 0; i < MAX_PCI_TRANSACTION_SPY; i++) if(pci_msg_spy[i]) delete pci_msg_spy[i];
	for(unsigned int i = 0; i < MAX_IRQ_TRANSACTION_SPY; i++) if(irq_msg_spy[i]) delete irq_msg_spy[i];

	if(memory) delete memory;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(profiler) delete profiler;
	if(debugger) delete debugger;
	if(http_server) delete http_server;
	if(bus) delete bus;
	if(cpu) delete cpu;
	if(sim_time) delete sim_time;
	if(host_time) delete host_time;
	if(memory_effective_to_physical_address_translator) delete memory_effective_to_physical_address_translator;
	if(flash) delete flash;
	if(erom) delete erom;
	if(pci_display) delete pci_display;
	if(pci_isa_bridge) delete pci_isa_bridge;
	if(i8042) delete i8042;
	if(sdl) delete sdl;
	if(kloader) delete kloader;
	if(pci_ide) delete pci_ide;
	if(mpc107) delete mpc107;
	if(pci_bus) delete pci_bus;
	if(heathrow) delete heathrow;
	if(instrumenter) delete instrumenter;
	if(logger_console_printer) delete logger_console_printer;
	if(logger_text_file_writer) delete logger_text_file_writer;
	if(logger_http_writer) delete logger_http_writer;
	if(logger_xml_file_writer) delete logger_xml_file_writer;
	if(logger_netstream_writer) delete logger_netstream_writer;
}

void Simulator::LoadBuiltInConfig(unisim::kernel::Simulator *simulator)
{
	new unisim::kernel::config::xml::XMLConfigFileHelper(simulator);
	new unisim::kernel::config::ini::INIConfigFileHelper(simulator);
	new unisim::kernel::config::json::JSONConfigFileHelper(simulator);
	
	// meta information
	simulator->SetVariable("program-name", "UNISIM ppcemu-system");
	simulator->SetVariable("copyright", "Copyright (C) 2007-2019, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM ppcemu-system is a full system simulator of a board including a MPC7447A PowerPC processor, a MPC107 chipset, and supporting Linux boot. The simulated board is very similar to a PowerMac G4 PCI machine. Computations on IEEE 754 floating point numbers are emulated using Simfloat++. Altivec instructions are currently decoded but not implemented. The running PowerPC application is a PowerMac Linux Kernel and all the applications installed on the hard disk image and/or the initial RAM disk image. Software running on the simulated hardware can be debugged by connecting a GDB client to the simulator through the GDB serial remote protocol. The GDB client can be either the standard text based client (i.e. command gdb), a graphical front-end to GDB (e.g. ddd), or even Eclipse CDT.");
	simulator->SetVariable("schematic", "ppcemu_system/fig_schematic.pdf");

	// Default run-time configuration
	int gdb_server_tcp_port = 1234;
	const char *filename = "vmlinux";
	const char *kernel_params = "/dev/ram0 rw";
	const char *device_tree_filename = "unisim/service/loader/pmac_bootx/device_tree_pmac_g4.xml";
	const char *gdb_server_arch_filename = "unisim/service/debug/gdb_server/gdb_powerpc_32.xml";
	const char *dwarf_register_number_mapping_filename = "unisim/util/debug/dwarf/powerpc_eabi_gcc_dwarf_register_number_mapping.xml";
	const char *ramdisk_filename = "initrd.img";
	const char *bmp_out_filename = "";
	const char *keymap_filename = "unisim/service/sdl/pc_linux_fr_keymap.xml";
	uint64_t max_inst = 0xffffffffffffffffULL; // maximum number of instruction to simulate
	uint32_t pci_bus_frequency = 33; // in Mhz
	uint32_t isa_bus_frequency = 8; // in Mhz
	double cpu_frequency = 300.0; // in Mhz
	uint32_t cpu_clock_multiplier = 4;
	double fsb_frequency = cpu_frequency / cpu_clock_multiplier; // FIXME: to be removed
	uint32_t display_width = 640; // in pixels
	uint32_t display_height = 480; // in pixels
	uint32_t display_depth = 15; // in bits per pixel
	uint32_t display_vfb_size = 8 * 1024 * 1024; // 8 MB
	uint32_t video_refresh_period = 40; // every 40 ms (25 fps)
	uint32_t memory_size = 512 * 1024 * 1024; // 512 MB
	double cpu_ipc = 1.0; // in instructions per cycle
	double cpu_cycle_time = (double)(1.0e6 / cpu_frequency); // in picoseconds
	double fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	double mem_cycle_time = fsb_cycle_time;

	//  - Front Side Bus
	simulator->SetVariable("bus.cycle-time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());

	//	- PowerPC processor
	// if the following line ("cpu-cycle-time") is commented, the cpu will use the power estimators to find min cpu cycle time
	simulator->SetVariable("cpu.cpu-cycle-time", sc_core::sc_time(cpu_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.bus-cycle-time", sc_core::sc_time(fsb_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.voltage", 1.3 * 1e3); // mV
	simulator->SetVariable("cpu.nice-time", "1 ms"); // 1 ms
	simulator->SetVariable("cpu.max-inst", max_inst);
	simulator->SetVariable("cpu.ipc", cpu_ipc);
	simulator->SetVariable("cpu.processor-version", 0x80030100); // MPC7447A

	//  - RAM
	simulator->SetVariable("memory.cycle-time", sc_core::sc_time(mem_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("memory.org", 0x00000000UL);
	simulator->SetVariable("memory.bytesize", memory_size);

	// MPC107 run-time configuration
	simulator->SetVariable("mpc107.a_address_map", false);
	simulator->SetVariable("mpc107.host_mode", true);
	simulator->SetVariable("mpc107.memory_32bit_data_bus_size", true);
	simulator->SetVariable("mpc107.rom0_8bit_data_bus_size", false);
	simulator->SetVariable("mpc107.rom1_8bit_data_bus_size", false);
	simulator->SetVariable("mpc107.frequency", fsb_frequency);
	simulator->SetVariable("mpc107.sdram_cycle_time", mem_cycle_time);

	//  - EROM run-time configuration
	simulator->SetVariable("erom.org",  0x78000000UL);
	simulator->SetVariable("erom.bytesize", 2 * 8 * 1024 * 1024);
	simulator->SetVariable("erom.cycle-time", sc_core::sc_time(mem_cycle_time, sc_core::SC_PS).to_string().c_str());

	//  - Flash memory run-time configuration
	simulator->SetVariable("flash.org", 0xff800000UL); //0xff000000UL);
	simulator->SetVariable("flash.bytesize", 8 * 1024 * 1024);
	simulator->SetVariable("flash.cycle-time", sc_core::sc_time(mem_cycle_time, sc_core::SC_PS).to_string().c_str());
	simulator->SetVariable("flash.endian", "big-endian");

	// PCI Bus run-time configuration
	simulator->SetVariable("pci-bus.frequency", pci_bus_frequency);

	simulator->SetVariable("pci-bus.base-address[0]", 0);
	simulator->SetVariable("pci-bus.size[0]", 1024 * 1024 * 1024);
	simulator->SetVariable("pci-bus.device-number[0]", PCI_MPC107_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[0]", PCI_MPC107_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[0]", 0x10);
	simulator->SetVariable("pci-bus.addr-type[0]", "mem");

	simulator->SetVariable("pci-bus.base-address[1]", 0xf3000000UL);
	simulator->SetVariable("pci-bus.size[1]", 0x80000);
	simulator->SetVariable("pci-bus.device-number[1]", PCI_HEATHROW_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[1]", PCI_HEATHROW_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[1]", 0x10UL);
	simulator->SetVariable("pci-bus.addr-type[1]", "mem");
	
	simulator->SetVariable("pci-bus.base-address[2]", 0x18100UL);
	simulator->SetVariable("pci-bus.size[2]", 8);
	simulator->SetVariable("pci-bus.device-number[2]", PCI_IDE_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[2]", PCI_IDE_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[2]", 0x10UL);
	simulator->SetVariable("pci-bus.addr-type[2]", "i/o");
	
	simulator->SetVariable("pci-bus.base-address[3]", 0x18108UL);
	simulator->SetVariable("pci-bus.size[3]", 4);
	simulator->SetVariable("pci-bus.device-number[3]", PCI_IDE_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[3]", PCI_IDE_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[3]", 0x14UL);
	simulator->SetVariable("pci-bus.addr-type[3]", "i/o");
	
	simulator->SetVariable("pci-bus.base-address[4]", 0x4UL);
	simulator->SetVariable("pci-bus.size[4]", 8);
	simulator->SetVariable("pci-bus.device-number[4]", PCI_IDE_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[4]", PCI_IDE_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[4]", 0x18UL);
	simulator->SetVariable("pci-bus.addr-type[4]", "i/o");
	
	simulator->SetVariable("pci-bus.base-address[5]", 0xcUL);
	simulator->SetVariable("pci-bus.size[5]", 4);
	simulator->SetVariable("pci-bus.device-number[5]", PCI_IDE_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[5]", PCI_IDE_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[5]", 0x1cUL);
	simulator->SetVariable("pci-bus.addr-type[5]", "i/o");
	
	simulator->SetVariable("pci-bus.base-address[6]", 0x18118UL);
	simulator->SetVariable("pci-bus.size[6]", 16);
	simulator->SetVariable("pci-bus.device-number[6]", PCI_IDE_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[6]", PCI_IDE_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[6]", 0x20UL);
	simulator->SetVariable("pci-bus.addr-type[6]", "i/o");

	simulator->SetVariable("pci-bus.base-address[7]", 0xa0000000UL);
	simulator->SetVariable("pci-bus.size[7]", 0x800000);
	simulator->SetVariable("pci-bus.device-number[7]", PCI_DISPLAY_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[7]", PCI_DISPLAY_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[7]", 0x10UL);
	simulator->SetVariable("pci-bus.addr-type[7]", "mem");
	
	simulator->SetVariable("pci-bus.base-address[8]", 0); //0xfe000000UL); // ISA I/O is at the very beginning of PCI I/O space
	simulator->SetVariable("pci-bus.size[8]", 0x10000); // 64 KB
	simulator->SetVariable("pci-bus.device-number[8]", PCI_ISA_BRIDGE_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[8]", PCI_ISA_BRIDGE_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[8]", 0x10UL); // ISA I/O space mapped by BAR0
	simulator->SetVariable("pci-bus.addr-type[8]", "i/o");

	simulator->SetVariable("pci-bus.base-address[9]", 0x000a0000UL); // ISA Memory is at the very beginning of compatibility hole
	simulator->SetVariable("pci-bus.size[9]", 0x60000); // 384 KB
	simulator->SetVariable("pci-bus.device-number[9]", PCI_ISA_BRIDGE_DEV_NUM);
	simulator->SetVariable("pci-bus.target-port[9]", PCI_ISA_BRIDGE_SLAVE_PORT);
	simulator->SetVariable("pci-bus.register-number[9]", 0x14UL); // ISA Memory space mapped by BAR1
	simulator->SetVariable("pci-bus.addr-type[9]", "mem");

	simulator->SetVariable("pci-bus.num-mappings", 10);

	//  - PCI MAC I/O Heathrow run-time configuration
	simulator->SetVariable("heathrow.initial-base-addr", 0xf3000000UL);
	simulator->SetVariable("heathrow.pci-device-number", PCI_HEATHROW_DEV_NUM);
	simulator->SetVariable("heathrow.bus-frequency", pci_bus_frequency);

	//  - PCI IDE run-time configuration
	simulator->SetVariable("pci-ide.device-number", PCI_IDE_DEV_NUM);
	simulator->SetVariable("pci-ide.base-address[0]", 0x18101);
	simulator->SetVariable("pci-ide.size[0]", 8);
	simulator->SetVariable("pci-ide.register-number[0]", 0x10);
	simulator->SetVariable("pci-ide.base-address[1]", 0x18109);
	simulator->SetVariable("pci-ide.size[1]", 4);
	simulator->SetVariable("pci-ide.register-number[1]", 0x14);
	simulator->SetVariable("pci-ide.base-address[2]", 0x5);
	simulator->SetVariable("pci-ide.size[2]", 8);
	simulator->SetVariable("pci-ide.register-number[2]", 0x18);
	simulator->SetVariable("pci-ide.base-address[3]", 0xd);
	simulator->SetVariable("pci-ide.size[3]", 4);
	simulator->SetVariable("pci-ide.register-number[3]", 0x1c);
	simulator->SetVariable("pci-ide.base-address[4]", 0x18119);
	simulator->SetVariable("pci-ide.size[4]", 16);
	simulator->SetVariable("pci-ide.register-number[4]", 0x20);

	//  - Display run-time configuration
	simulator->SetVariable("pci-display.initial-base-addr", 0xa0000000UL);
	simulator->SetVariable("pci-display.bytesize", display_vfb_size); 
	simulator->SetVariable("pci-display.width", display_width);
	simulator->SetVariable("pci-display.height", display_height);
	simulator->SetVariable("pci-display.depth", display_depth);
	simulator->SetVariable("pci-display.pci-bus-frequency", pci_bus_frequency);
	simulator->SetVariable("pci-display.pci-device-number", PCI_DISPLAY_DEV_NUM);

	// - PCI-ISA Bridge run-time configuration
	simulator->SetVariable("pci-isa-bridge.initial-base-addr", 0x000a0000UL);
	simulator->SetVariable("pci-isa-bridge.initial-io-base-addr", 0); //0xfe000000UL);
	simulator->SetVariable("pci-isa-bridge.pci-bus-frequency", pci_bus_frequency);
	simulator->SetVariable("pci-isa-bridge.isa-bus-frequency", isa_bus_frequency);
	simulator->SetVariable("pci-isa-bridge.pci-device-number", PCI_ISA_BRIDGE_DEV_NUM);

	//  - i8042 run-time configuration
	simulator->SetVariable("i8042.fsb-frequency", fsb_frequency);
	simulator->SetVariable("i8042.isa-bus-frequency", isa_bus_frequency);

	//  - GDB server run-time configuration
	simulator->SetVariable("gdb-server.tcp-port", gdb_server_tcp_port);
	simulator->SetVariable("gdb-server.architecture-description-filename", gdb_server_arch_filename);

	//  - Debugger run-time configuration
	simulator->SetVariable("debugger.parse-dwarf", false);
	simulator->SetVariable("debugger.dwarf-register-number-mapping-filename", dwarf_register_number_mapping_filename);

	//  - SDL run-time configuration
	simulator->SetVariable("sdl.refresh-period", video_refresh_period);
	simulator->SetVariable("sdl.bmp-out-filename", bmp_out_filename);	
	simulator->SetVariable("sdl.keymap-filename", keymap_filename);	

	// - Kernel loader configuration
	simulator->SetVariable("pmac-linux-kernel-loader.pmac-bootx.device-tree-filename", device_tree_filename);
	simulator->SetVariable("pmac-linux-kernel-loader.pmac-bootx.kernel-params", kernel_params); // no kernel parameters
	simulator->SetVariable("pmac-linux-kernel-loader.pmac-bootx.ramdisk-filename", ramdisk_filename);
	simulator->SetVariable("pmac-linux-kernel-loader.pmac-bootx.screen-width", display_width);
	simulator->SetVariable("pmac-linux-kernel-loader.pmac-bootx.screen-height", display_height);
	simulator->SetVariable("pmac-linux-kernel-loader.elf32-loader.filename", filename);
	simulator->SetVariable("pmac-linux-kernel-loader.elf32-loader.base-addr", 0x00400000UL);
	simulator->SetVariable("pmac-linux-kernel-loader.elf32-loader.force-base-addr", true);
	simulator->SetVariable("pmac-linux-kernel-loader.elf32-loader.force-use-virtual-address", true);
	
	// - kernel logger
	simulator->SetVariable("kernel_logger.std_err", true);
	
	// Inline debugger
	simulator->SetVariable("inline-debugger.num-loaders", 1);
	
	// - Http Server
	simulator->SetVariable("http-server.http-port", 12360);
}

void Simulator::Stop(unisim::kernel::Object *object, int _exit_status, bool asynchronous)
{
	if(!stop_called)
	{
		stop_called = true;
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
}

void Simulator::Run()
{
	double time_start = host_time->GetTime();

	if(!stop_called)
	{
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

  // Build the kernel parameters string from the command line arguments
	std::string filename;
	std::string kernel_params;
	
	unisim::kernel::VariableBase *cmd_args = FindVariable("cmd-args");
	unsigned int cmd_args_length = cmd_args->GetLength();
	if(cmd_args_length > 0)
	{
		filename = (std::string) (*cmd_args)[0];
		unsigned int i;
		for(i = 1; i < cmd_args_length; i++)
		{
			kernel_params += (std::string) (*cmd_args)[i];
			if(i < cmd_args_length - 1) kernel_params += " ";
		}
	}
	
	//  - Loader run-time configuration
	if(!filename.empty())
	{
		SetVariable("pmac-linux-kernel-loader.elf32-loader.filename", filename.c_str());
	}

	if(!kernel_params.empty())
	{
		SetVariable("pmac-linux-kernel-loader.pmac-bootx.kernel-params", kernel_params.c_str());
	}

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
	
	return unisim::kernel::Simulator::Setup();
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
			std::cerr << "Starting simulation at supervisor privilege level (kernel mode)" << std::endl;
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

extern "C"
int main(int argc, char *argv[])
{
	return sc_core::sc_elab_and_sim(argc, argv);
}
