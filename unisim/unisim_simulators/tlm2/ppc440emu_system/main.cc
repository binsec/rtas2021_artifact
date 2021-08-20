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

#include <unisim/component/cxx/processor/powerpc/ppc440/config.hh>
#include <unisim/component/tlm2/processor/powerpc/ppc440/cpu.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>
#include <unisim/component/tlm2/interrupt/xilinx/xps_intc/xps_intc.hh>
#include <unisim/component/tlm2/interrupt/xilinx/xps_intc/xps_intc.hh>
#include <unisim/component/cxx/interrupt/xilinx/xps_intc/config.hh>
#include <unisim/component/tlm2/timer/xilinx/xps_timer/xps_timer.hh>
#include <unisim/component/cxx/timer/xilinx/xps_timer/config.hh>
#include <unisim/component/tlm2/timer/xilinx/xps_timer/capture_trigger_stub.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.hh>
#include <unisim/component/tlm2/interconnect/generic_router/config.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.tcc>
#include <unisim/component/tlm2/memory/flash/am29/am29.hh>
#include <unisim/component/cxx/memory/flash/am29/s29gl256p_config.hh>
#include <unisim/component/tlm2/interconnect/xilinx/dcr_controller/dcr_controller.hh>
#include <unisim/component/cxx/interconnect/xilinx/dcr_controller/config.hh>
#include <unisim/component/tlm2/interconnect/xilinx/crossbar/crossbar.hh>
#include <unisim/component/cxx/interconnect/xilinx/crossbar/config.hh>
#include <unisim/component/tlm2/com/xilinx/xps_uart_lite/xps_uart_lite.hh>
#include <unisim/component/cxx/com/xilinx/xps_uart_lite/config.hh>

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/console/console_printer.hh>
#include <unisim/kernel/logger/text_file/text_file_writer.hh>
#include <unisim/kernel/logger/http/http_writer.hh>
#include <unisim/kernel/logger/xml_file/xml_file_writer.hh>
#include <unisim/kernel/logger/netstream/netstream_writer.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/power/cache_power_estimator.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/translator/memory_address/memory/translator.hh>
#include <unisim/service/loader/ppc_linux_kernel_loader/ppc_linux_kernel_loader.hh>
#include <unisim/service/loader/elf_loader/elf32_loader.hh>
#include <unisim/service/telnet/telnet.hh>

#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <signal.h>

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#endif

#ifdef WITH_FPU
#ifdef DEBUG_PPC440EMU_SYSTEM
static const bool DEBUG_INFORMATION = true;
typedef unisim::component::cxx::processor::powerpc::ppc440::DebugConfig_wFPU CPU_CONFIG;
#else
static const bool DEBUG_INFORMATION = false;
typedef unisim::component::cxx::processor::powerpc::ppc440::Config_wFPU CPU_CONFIG;
#endif
#else
#ifdef DEBUG_PPC440EMU_SYSTEM
static const bool DEBUG_INFORMATION = true;
typedef unisim::component::cxx::processor::powerpc::ppc440::DebugConfig CPU_CONFIG;
#else
static const bool DEBUG_INFORMATION = false;
typedef unisim::component::cxx::processor::powerpc::ppc440::Config CPU_CONFIG;
#endif
#endif

void SigIntHandler(int signum)
{
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	sc_stop();
}

using namespace std;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::service::loader::ppc_linux_kernel_loader::PPCLinuxKernelLoader;
using unisim::service::loader::elf_loader::Elf32Loader;
using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::service::power::CachePowerEstimator;
using unisim::service::telnet::Telnet;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Variable;
using unisim::kernel::VariableBase;
using unisim::kernel::Object;

#ifdef DEBUG_PPC440EMU_SYSTEM
class MPLBDebugConfig : public unisim::component::tlm2::interconnect::generic_router::VerboseConfig
{
public:
	static const unsigned int INPUT_SOCKETS = 1;
	static const unsigned int OUTPUT_SOCKETS = 5;
	static const unsigned int MAX_NUM_MAPPINGS = 5;
	static const unsigned int BUSWIDTH = 128;
};

typedef MPLBDebugConfig MPLB_CONFIG;
#else
class MPLBConfig : public unisim::component::tlm2::interconnect::generic_router::Config
{
public:
	static const unsigned int INPUT_SOCKETS = 1;
	static const unsigned int OUTPUT_SOCKETS = 5;
	static const unsigned int MAX_NUM_MAPPINGS = 5;
	static const unsigned int BUSWIDTH = 128;
};

typedef MPLBConfig MPLB_CONFIG;
#endif

typedef unisim::component::cxx::interrupt::xilinx::xps_intc::Config INTC_CONFIG;
typedef unisim::component::cxx::timer::xilinx::xps_timer::Config TIMER_CONFIG;
typedef unisim::component::cxx::com::xilinx::xps_uart_lite::Config UART_LITE_CONFIG;
static const unsigned int TIMER_IRQ = 3;
static const unsigned int UART_LITE_IRQ = 2;

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
	typedef unisim::component::cxx::memory::flash::am29::S29GL256PConfig AM29_CONFIG;
	typedef unisim::component::cxx::interconnect::xilinx::dcr_controller::Config DCR_CONTROLLER_CONFIG;
	typedef unisim::component::cxx::interconnect::xilinx::crossbar::Config CROSSBAR_CONFIG;

	struct DEBUGGER_CONFIG
	{
		typedef CPU_ADDRESS_TYPE ADDRESS;
		static const unsigned int NUM_PROCESSORS = 1;
		/* gdb_server, inline_debugger */
		static const unsigned int MAX_FRONT_ENDS = 2;
	};
	
	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm2::memory::ram::Memory<CPU_CONFIG::FSB_WIDTH * 8, FSB_ADDRESS_TYPE, CPU_CONFIG::FSB_BURST_SIZE / CPU_CONFIG::FSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_INFORMATION> RAM;
	typedef unisim::component::tlm2::memory::ram::Memory<CPU_CONFIG::FSB_WIDTH * 8, FSB_ADDRESS_TYPE, CPU_CONFIG::FSB_BURST_SIZE / CPU_CONFIG::FSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_INFORMATION> ROM;
	typedef unisim::component::tlm2::processor::powerpc::ppc440::CPU<CPU_CONFIG> CPU;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<MPLB_CONFIG> MPLB;
	typedef unisim::component::tlm2::interrupt::xilinx::xps_intc::XPS_IntC<INTC_CONFIG> INTC;
	typedef unisim::component::tlm2::timer::xilinx::xps_timer::XPS_Timer<TIMER_CONFIG> TIMER;
	typedef unisim::component::tlm2::memory::flash::am29::AM29<AM29_CONFIG, 32 * unisim::component::cxx::memory::flash::am29::M, 2, CPU_CONFIG::FSB_WIDTH * 8> FLASH;
	typedef unisim::component::tlm2::interconnect::xilinx::dcr_controller::DCRController<DCR_CONTROLLER_CONFIG> DCR_CONTROLLER;
	typedef unisim::component::tlm2::interconnect::xilinx::crossbar::Crossbar<CROSSBAR_CONFIG> CROSSBAR;
	typedef unisim::component::tlm2::com::xilinx::xps_uart_lite::XPS_UARTLite<UART_LITE_CONFIG> UART_LITE;
	typedef unisim::kernel::tlm2::TargetStub<0, unisim::component::tlm2::timer::xilinx::xps_timer::PWMProtocolTypes> PWM_STUB;
	typedef unisim::kernel::tlm2::TargetStub<0, unisim::component::tlm2::timer::xilinx::xps_timer::GenerateOutProtocolTypes> GENERATE_OUT_STUB;
	typedef unisim::component::tlm2::timer::xilinx::xps_timer::CaptureTriggerStub CAPTURE_TRIGGER_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<0, unisim::component::tlm2::timer::xilinx::xps_timer::InterruptProtocolTypes> IRQ_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<CPU_CONFIG::FSB_WIDTH * 8> SPLB0_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<CPU_CONFIG::FSB_WIDTH * 8> SPLB1_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<4> MASTER1_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> APU_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> MCI_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DMAC0_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DMAC1_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DMAC2_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DMAC3_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> EXTERNAL_SLAVE_DCR_STUB;

	//=========================================================================
	//===                      Aliases for services classes                 ===
	//=========================================================================

	typedef PPCLinuxKernelLoader<CPU_ADDRESS_TYPE> PPC_LINUX_KERNEL_LOADER;
	typedef Elf32Loader<CPU_ADDRESS_TYPE> ROM_LOADER;
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
	RAM *ram;
	//  - ROM
	ROM *rom;
	// - IRQ stubs
	IRQ_STUB *input_interrupt_stub[INTC_CONFIG::C_NUM_INTR_INPUTS];
	IRQ_STUB *critical_input_interrupt_stub;
	// - MPLB
	MPLB *mplb;
	// - SPLB0
	SPLB0_STUB *splb0_stub;
	// - SPLB1
	SPLB1_STUB *splb1_stub;
	// - Interrupt controller
	INTC *intc;
	// - Timer
	TIMER *timer;
	// - Flash memory
	FLASH *flash;
	// - Capture trigger stubs
	CAPTURE_TRIGGER_STUB *capture_trigger_stub[TIMER_CONFIG::NUM_TIMERS];
	// - GenerateOutStub
	GENERATE_OUT_STUB *generate_out_stub[TIMER_CONFIG::NUM_TIMERS];
	// - PWM stub
	PWM_STUB *pwm_stub;
	// - DCR controller
	DCR_CONTROLLER *dcr_controller;
	// - Crossbar
	CROSSBAR *crossbar;
	// - UART Lite
	UART_LITE *uart_lite;
	// - DCR stubs
	MASTER1_DCR_STUB *master1_dcr_stub;
	APU_DCR_STUB *apu_dcr_stub;
	MCI_DCR_STUB *mci_dcr_stub;
	DMAC0_DCR_STUB *dmac0_dcr_stub;
	DMAC1_DCR_STUB *dmac1_dcr_stub;
	DMAC2_DCR_STUB *dmac2_dcr_stub;
	DMAC3_DCR_STUB *dmac3_dcr_stub;
	EXTERNAL_SLAVE_DCR_STUB *external_slave_dcr_stub;

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - PowerPC Linux Kernel loader (vmlinux, device tree, initrd and kernel cmd line)
	PPCLinuxKernelLoader<CPU_ADDRESS_TYPE> *ppc_linux_kernel_loader;
	//  - ROM loader (ELF32)
	Elf32Loader<CPU_ADDRESS_TYPE> *rom_loader;
	
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
	unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE> *ram_effective_to_physical_address_translator;
	unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE> *flash_effective_to_physical_address_translator;
	unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE> *rom_effective_to_physical_address_translator;
	// - telnet
	unisim::service::telnet::Telnet *telnet;

	bool enable_gdb_server;
	bool enable_inline_debugger;
	bool estimate_power;
	bool enable_telnet;
	Parameter<bool> param_enable_gdb_server;
	Parameter<bool> param_enable_inline_debugger;
	Parameter<bool> param_estimate_power;
	Parameter<bool> param_enable_telnet;

	int exit_status;
	static void LoadBuiltInConfig(unisim::kernel::Simulator *simulator);
};



Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, ram(0)
	, rom(0)
	, critical_input_interrupt_stub(0)
	, mplb(0)
	, splb0_stub(0)
	, splb1_stub(0)
	, intc(0)
	, timer(0)
	, flash(0)
	, pwm_stub(0)
	, dcr_controller(0)
	, crossbar(0)
	, uart_lite(0)
	, master1_dcr_stub(0)
	, apu_dcr_stub(0)
	, mci_dcr_stub(0)
	, dmac0_dcr_stub(0)
	, dmac1_dcr_stub(0)
	, dmac2_dcr_stub(0)
	, dmac3_dcr_stub(0)
	, external_slave_dcr_stub(0)
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
	, ram_effective_to_physical_address_translator(0)
	, flash_effective_to_physical_address_translator(0)
	, rom_effective_to_physical_address_translator(0)
	, telnet(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, estimate_power(false)
	, enable_telnet(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, param_estimate_power("estimate-power", 0, estimate_power, "Enable/Disable power estimators instantiation")
	, param_enable_telnet("enable-telnet", 0, enable_telnet, "Enable/Disable telnet instantiation")
	, exit_status(0)
{
	unsigned int irq;
	unsigned int channel;
	
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	cpu = new CPU("cpu");
	//  - RAM
	ram = new RAM("ram");
	//  - ROM
	rom = new ROM("rom");
	//  - IRQ Stubs
	for(irq = 0; irq < INTC_CONFIG::C_NUM_INTR_INPUTS; irq++)
	{
		switch(irq)
		{
			case TIMER_IRQ:
			case UART_LITE_IRQ:
				input_interrupt_stub[irq] = 0;
				break;
			default:
				{
					std::stringstream input_interrupt_stub_name_sstr;
					input_interrupt_stub_name_sstr << "input-interrupt-stub" << irq;
					input_interrupt_stub[irq] = new IRQ_STUB(input_interrupt_stub_name_sstr.str().c_str());
				}
				break;
		}
	}
	critical_input_interrupt_stub = new IRQ_STUB("critical-input-interrupt-stub");
	// - MPLB
	mplb = new MPLB("mplb");
	// - SPLB0
	splb0_stub = new SPLB0_STUB("splb0-stub");
	// - SPLB1
	splb1_stub = new SPLB0_STUB("splb1-stub");
	// - Interrupt controller
	intc = new INTC("intc");
	// - Timer
	timer = new TIMER("timer");
	// - Flash memory
	flash = new FLASH("flash");
	// - Capture trigger stubs
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		std::stringstream capture_trigger_stub_name_sstr;
		capture_trigger_stub_name_sstr << "capture-trigger-stub" << channel;
		capture_trigger_stub[channel] = new CAPTURE_TRIGGER_STUB(capture_trigger_stub_name_sstr.str().c_str());
	}
	// - Generate out stubs
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		std::stringstream generate_out_stub_name_sstr;
		generate_out_stub_name_sstr << "generate-out-stub" << channel;
		generate_out_stub[channel] = new GENERATE_OUT_STUB(generate_out_stub_name_sstr.str().c_str());
	}
	// - PWM stub
	pwm_stub = new PWM_STUB("pwm-stub");
	// - DCR controller
	dcr_controller = new DCR_CONTROLLER("dcr-controller");
	// - Crossbar
	crossbar = new CROSSBAR("crossbar");
	// - UART Lite
	uart_lite = new UART_LITE("uart-lite");
	// - DCR stubs
	master1_dcr_stub = new MASTER1_DCR_STUB("master1-dcr-stub");
	apu_dcr_stub = new APU_DCR_STUB("apu-dcr-stub");
	mci_dcr_stub = new MCI_DCR_STUB("mci-dcr-stub");
	dmac0_dcr_stub = new DMAC0_DCR_STUB("dma0-dcr-stub");
	dmac1_dcr_stub = new DMAC1_DCR_STUB("dma1-dcr-stub");
	dmac2_dcr_stub = new DMAC2_DCR_STUB("dma2-dcr-stub");
	dmac3_dcr_stub = new DMAC3_DCR_STUB("dma3-dcr-stub");
	external_slave_dcr_stub = new EXTERNAL_SLAVE_DCR_STUB("external-slave-dcr-stub");

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - PPC Linux Kernel Loader
	ppc_linux_kernel_loader = new PPCLinuxKernelLoader<CPU_ADDRESS_TYPE>("ppc-linux-kernel-loader");
	//  - ROM Loader
	rom_loader = new Elf32Loader<CPU_ADDRESS_TYPE>("rom-loader");
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
	ram_effective_to_physical_address_translator = new unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE>("ram-effective-to-physical-address-translator");
	flash_effective_to_physical_address_translator = new unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE>("flash-effective-to-physical-address-translator");
	rom_effective_to_physical_address_translator = new unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE>("rom-effective-to-physical-address-translator");
	// - telnet
	telnet = enable_telnet ? new unisim::service::telnet::Telnet("telnet") : 0;
	
	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	cpu->icurd_plb_master_sock(crossbar->icurd_plb_slave_sock); // CPU>ICURD <-> ICURD<Crossbar
	cpu->dcuwr_plb_master_sock(crossbar->dcuwr_plb_slave_sock); // CPU>DCUWR <-> DCUWR<Crossbar
	cpu->dcurd_plb_master_sock(crossbar->dcurd_plb_slave_sock); // CPU>DCURD <-> DCURD<Crossbar
	cpu->dcr_master_sock(*dcr_controller->dcr_slave_sock[0]); // (master 0) CPU>DCR <-> DCR controller
	master1_dcr_stub->master_sock(*dcr_controller->dcr_slave_sock[1]); // master 1>DCR <-> DCR controller
	
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::APU_SLAVE_NUM])(apu_dcr_stub->slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::MCI_SLAVE_NUM])(mci_dcr_stub->slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::CROSSBAR_SLAVE_NUM])(crossbar->crossbar_dcr_slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::PLBS0_SLAVE_NUM])(crossbar->plbs0_dcr_slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::PLBS1_SLAVE_NUM])(crossbar->plbs1_dcr_slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::PLBM_SLAVE_NUM])(crossbar->plbm_dcr_slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::DMAC0_SLAVE_NUM])(dmac0_dcr_stub->slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::DMAC1_SLAVE_NUM])(dmac1_dcr_stub->slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::DMAC2_SLAVE_NUM])(dmac2_dcr_stub->slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::DMAC3_SLAVE_NUM])(dmac3_dcr_stub->slave_sock);
	(*dcr_controller->dcr_master_sock[DCR_CONTROLLER_CONFIG::EXTERNAL_SLAVE_NUM])(external_slave_dcr_stub->slave_sock);
	
	crossbar->mplb_master_sock(*mplb->targ_socket[0]);   // crossbar>MPLB <-> MPLB
	crossbar->mci_master_sock(ram->slave_sock);       // crossbar>MCI <-> RAM
	
	splb0_stub->master_sock(crossbar->splb0_slave_sock);  // SPLB0 stub <-> SPLB0<Crossbar
	splb1_stub->master_sock(crossbar->splb1_slave_sock);  // SPLB1 stub <-> SPLB1<Crossbar
	
	(*mplb->init_socket[0])(intc->slave_sock);      // MPLB <-> INTC
	(*mplb->init_socket[1])(timer->slave_sock);     // MPLB <-> TIMER
	(*mplb->init_socket[2])(flash->slave_sock);     // MPLB <-> FLASH
	(*mplb->init_socket[3])(rom->slave_sock);       // MPLB <-> ROM
	(*mplb->init_socket[4])(uart_lite->slave_sock); // MPLB <-> UART Lite
	
	for(irq = 0; irq < INTC_CONFIG::C_NUM_INTR_INPUTS; irq++)
	{
		switch(irq)
		{
			case TIMER_IRQ:
				timer->interrupt_master_sock(*intc->irq_slave_sock[irq]); // TIMER>IRQ <-> INTR<INTC
				break;
			case UART_LITE_IRQ:
				uart_lite->interrupt_master_sock(*intc->irq_slave_sock[irq]); // UART Lite>IRQ <-> INTR<INTC
				break;
			default:
				(input_interrupt_stub[irq]->master_sock)(*intc->irq_slave_sock[irq]); // IRQ stub>IRQ <-> INTR<INTC
				break;
		}
	}
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		capture_trigger_stub[channel]->master_sock(*timer->capture_trigger_slave_sock[channel]); // Capture trigger stub <-> TIMER
	}
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		(*timer->generate_out_master_sock[channel])(generate_out_stub[channel]->slave_sock); // TIMER <-> Generate out stub
	}
	timer->pwm_master_sock(pwm_stub->slave_sock); // TIMER <-> PWM stub
	intc->irq_master_sock(cpu->external_input_interrupt_slave_sock); // INTC>IRQ <-> External Input<CPU
	critical_input_interrupt_stub->master_sock(cpu->critical_input_interrupt_slave_sock); // IRQ Stub <-> CPU

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> crossbar->memory_export;
	
	crossbar->mci_memory_import >> ram->memory_export;
	crossbar->mplb_memory_import >> mplb->memory_export;
	(*mplb->memory_import[0]) >> intc->memory_export;
	(*mplb->memory_import[1]) >> timer->memory_export;
	(*mplb->memory_import[2]) >> flash->memory_export;
	(*mplb->memory_import[3]) >> rom->memory_export;
	cpu->loader_import >> ppc_linux_kernel_loader->loader_export;
	ppc_linux_kernel_loader->registers_import >> cpu->registers_export;
	
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
		debugger->blob_import >> ppc_linux_kernel_loader->blob_export;
		
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

	ram_effective_to_physical_address_translator->memory_import >> ram->memory_export;
	flash_effective_to_physical_address_translator->memory_import >> flash->memory_export;
	rom_effective_to_physical_address_translator->memory_import >> rom->memory_export;
	rom_loader->memory_import >> rom_effective_to_physical_address_translator->memory_export;
	ppc_linux_kernel_loader->memory_import >> ram_effective_to_physical_address_translator->memory_export;
	cpu->symbol_table_lookup_import >> ppc_linux_kernel_loader->symbol_table_lookup_export;
	
	if(enable_telnet)
	{
		uart_lite->char_io_import >> telnet->char_io_export;
	}
}

Simulator::~Simulator()
{
	unsigned int irq;
	unsigned int channel;
	if(critical_input_interrupt_stub) delete critical_input_interrupt_stub;
	if(ram) delete ram;
	if(rom) delete rom;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(cpu) delete cpu;
	if(mplb) delete mplb;
	if(splb0_stub) delete splb0_stub;
	if(splb1_stub) delete splb1_stub;
	if(intc) delete intc;
	if(timer) delete timer;
	if(flash) delete flash;
	if(crossbar) delete crossbar;
	if(uart_lite) delete uart_lite;
	if(master1_dcr_stub) delete master1_dcr_stub;
	if(apu_dcr_stub) delete apu_dcr_stub;
	if(mci_dcr_stub) delete mci_dcr_stub;
	if(dmac0_dcr_stub) delete dmac0_dcr_stub;
	if(dmac1_dcr_stub) delete dmac1_dcr_stub;
	if(dmac2_dcr_stub) delete dmac2_dcr_stub;
	if(dmac3_dcr_stub) delete dmac3_dcr_stub;
	if(external_slave_dcr_stub) delete external_slave_dcr_stub;
	for(irq = 0; irq < INTC_CONFIG::C_NUM_INTR_INPUTS; irq++)
	{
		if(input_interrupt_stub[irq]) delete input_interrupt_stub[irq];
	}
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		if(capture_trigger_stub[channel]) delete capture_trigger_stub[channel];
	}
	// - Generate out stubs
	for(channel = 0; channel < TIMER_CONFIG::NUM_TIMERS; channel++)
	{
		if(generate_out_stub[channel]) delete generate_out_stub[channel];
	}
	if(pwm_stub) delete pwm_stub;
	if(dcr_controller) delete dcr_controller;
	if(il1_power_estimator) delete il1_power_estimator;
	if(dl1_power_estimator) delete dl1_power_estimator;
	if(itlb_power_estimator) delete itlb_power_estimator;
	if(dtlb_power_estimator) delete dtlb_power_estimator;
	if(utlb_power_estimator) delete utlb_power_estimator;
	if(sim_time) delete sim_time;
	if(host_time) delete host_time;
	if(rom_loader) delete rom_loader;
	if(ppc_linux_kernel_loader) delete ppc_linux_kernel_loader;
	if(ram_effective_to_physical_address_translator) delete ram_effective_to_physical_address_translator;
	if(rom_effective_to_physical_address_translator) delete rom_effective_to_physical_address_translator;
	if(flash_effective_to_physical_address_translator) delete flash_effective_to_physical_address_translator;
	if(telnet) delete telnet;
}

void Simulator::LoadBuiltInConfig(unisim::kernel::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM ppc440emu-system");
	simulator->SetVariable("copyright", "Copyright (C) 2007-2010, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM ppc440emu-system, full system Xilinx-Virtex-5-FXT-like simulator with Linux kernel loading support");

	const char *kernel_filename = "vmlinux";
	const char *device_tree_filename = "device_tree.dtb";
	const char *initrd_filename = "initrd.img";
	const char *rom_filename = "boot.elf";
	int gdb_server_tcp_port = 1234;
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

	//  - DCR controller
	simulator->SetVariable("dcr-controller.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - Crossbar
	simulator->SetVariable("crossbar.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - UART Lite
	simulator->SetVariable("uart-lite.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - MPLB
	simulator->SetVariable("mplb.cycle_time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("mplb.mapping_0", "range_start=\"0x81800000\" range_end=\"0x8180ffff\" output_port=\"0\" translation=\"0x81800000\""); // XPS IntC
	simulator->SetVariable("mplb.mapping_1", "range_start=\"0x83c00000\" range_end=\"0x83c0ffff\" output_port=\"1\" translation=\"0x83c00000\""); // XPS Timer/Counter
	simulator->SetVariable("mplb.mapping_2", "range_start=\"0xfc000000\" range_end=\"0xfdffffff\" output_port=\"2\" translation=\"0xfc000000\""); // 32 MB Flash memory (i.e. 1 * 256 Mbits S29GL256P flash memory chips)
	simulator->SetVariable("mplb.mapping_3", "range_start=\"0xff800000\" range_end=\"0xffffffff\" output_port=\"3\" translation=\"0xff800000\""); // 8 MB ROM (i.e. 2 * 32 Mbits XCF32P platform flash memory)
	simulator->SetVariable("mplb.mapping_4", "range_start=\"0x84000000\" range_end=\"0x8400ffff\" output_port=\"4\" translation=\"0x84000000\""); // XPS UART Lite

	//  - RAM
	simulator->SetVariable("ram.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("ram.read-latency", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("ram.write-latency", SC_ZERO_TIME.to_string().c_str());
	simulator->SetVariable("ram.org", 0x00000000UL);
	simulator->SetVariable("ram.bytesize", 256 * 1024 * 1024); // 256 MB

	//  - ROM
	simulator->SetVariable("rom.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("rom.read-latency", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("rom.write-latency", SC_ZERO_TIME.to_string().c_str());
	simulator->SetVariable("rom.org", 0xff800000UL);
	simulator->SetVariable("rom.bytesize", 8 * 1024 * 1024); // 8 MB

	//  - Interrupt controller
	simulator->SetVariable("intc.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - Timer
	simulator->SetVariable("timer.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - Flash
	simulator->SetVariable("flash.org", 0xfc000000UL);
	simulator->SetVariable("flash.bytesize", 32 * 1024 * 1024); // 32 MB
	simulator->SetVariable("flash.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	
	//  - Capture Trigger stubs
	simulator->SetVariable("capture-trigger-stub0.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("capture-trigger-stub0.nice-time", "1 ms");
	simulator->SetVariable("capture-trigger-stub1.cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("capture-trigger-stub1.nice-time", "1 ms");

	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - GDB Server run-time configuration
	simulator->SetVariable("gdb-server.tcp-port", gdb_server_tcp_port);
	simulator->SetVariable("gdb-server.architecture-description-filename", gdb_server_arch_filename);
	//  - ROM Loader run-time configuration
	simulator->SetVariable("rom-loader.filename", rom_filename);
	//  - PPC Linux Kernel Loader run-time configuration
	simulator->SetVariable("ppc-linux-kernel-loader.kernel-loader.filename", kernel_filename);
	simulator->SetVariable("ppc-linux-kernel-loader.kernel-loader.base-addr", 0x0);
	simulator->SetVariable("ppc-linux-kernel-loader.kernel-loader.force-base-addr", true);
	simulator->SetVariable("ppc-linux-kernel-loader.kernel-loader.force-use-virtual-address", true);
	simulator->SetVariable("ppc-linux-kernel-loader.device-tree-loader.filename", device_tree_filename);
	simulator->SetVariable("ppc-linux-kernel-loader.device-tree-loader.base-addr", 0x00800000);
	simulator->SetVariable("ppc-linux-kernel-loader.uboot.kernel-cmd-line", "root=/dev/ram0 rw");
	simulator->SetVariable("ppc-linux-kernel-loader.uboot.kernel-cmd-line-addr", 0x00840000);
	simulator->SetVariable("ppc-linux-kernel-loader.initrd-loader.filename", initrd_filename);
	simulator->SetVariable("ppc-linux-kernel-loader.initrd-loader.base-addr", 0x00900000);

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
/*		SetVariable("linux-loader.argc", cmd_args_length);
		
		unsigned int i;
		for(i = 0; i < cmd_args_length; i++)
		{
			std::stringstream sstr;
			sstr << "linux-loader.argv[" << i << "]";
			SetVariable(sstr.str().c_str(), ((string)(*cmd_args)[i]).c_str());
		}*/
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
#ifdef DEBUG_PPC440EMU_SYSTEM
	std::cerr << "Call stack:" << std::endl;
	std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
#endif
	std::cerr << "Program exited with status " << exit_status << std::endl;
	sc_stop();
	switch(sc_get_curr_simcontext()->get_curr_proc_info()->kind)
	{
		case SC_THREAD_PROC_: 
		case SC_CTHREAD_PROC_:
			wait();
			break;
		default:
			break;
	}
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
			cerr << "Starting simulation at supervisor privilege level" << endl;
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
