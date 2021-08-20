/*
 *  Copyright (c) 2007-2011,
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

#ifndef __VIRTEX5FXT_SIMULATOR_HH__
#define __VIRTEX5FXT_SIMULATOR_HH__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// Class definition of components
#include <unisim/component/tlm2/processor/powerpc/book_e/ppc440/cpu.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>
#include <unisim/component/tlm2/interrupt/xilinx/xps_intc/xps_intc.hh>
#include <unisim/component/tlm2/interrupt/xilinx/xps_intc/xps_intc.hh>
#include <unisim/component/tlm2/timer/xilinx/xps_timer/xps_timer.hh>
#include <unisim/component/tlm2/timer/xilinx/xps_timer/capture_trigger_stub.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.hh>
#include <unisim/component/tlm2/memory/flash/am29/am29.hh>
#include <unisim/component/tlm2/interconnect/xilinx/dcr_controller/dcr_controller.hh>
#include <unisim/component/tlm2/interconnect/xilinx/crossbar/crossbar.hh>
#include <unisim/component/tlm2/interconnect/xilinx/mci/mci.hh>
#include <unisim/component/tlm2/com/xilinx/xps_uart_lite/xps_uart_lite.hh>
#include <unisim/component/tlm2/com/xilinx/xps_gpio/xps_gpio.hh>
#include <unisim/component/tlm2/com/xilinx/xps_gpio/gpio_leds.hh>
#include <unisim/component/tlm2/com/xilinx/xps_gpio/gpio_switches.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.hh>

// Simulator compile time configuration
#include <config.hh>

// Class definition of kernel, services and interfaces
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/tlm2/simulator.hh>
#include <unisim/kernel/logger/console/console_printer.hh>
#include <unisim/kernel/logger/text_file/text_file_writer.hh>
#include <unisim/kernel/logger/http/http_writer.hh>
#include <unisim/kernel/logger/xml_file/xml_file_writer.hh>
#include <unisim/kernel/logger/netstream/netstream_writer.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/debug/user_interface/user_interface.hh>
#include <unisim/service/loader/multiformat_loader/multiformat_loader.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/translator/memory_address/memory/translator.hh>
#include <unisim/service/netstreamer/netstreamer.hh>
#include <unisim/service/debug/profiler/profiler.hh>
#include <unisim/service/http_server/http_server.hh>
#include <unisim/service/instrumenter/instrumenter.hh>
#include <unisim/service/tee/char_io/tee.hh>
#include <unisim/service/web_terminal/web_terminal.hh>
#include <unisim/service/os/linux_os/powerpc_linux32.hh>
#include <unisim/kernel/tlm2/tlm.hh>

// Host machine standard headers
#include <iostream>
#include <stdexcept>
#include <stdlib.h>

//=========================================================================
//===                        Top level class                            ===
//=========================================================================

class Simulator : public unisim::kernel::tlm2::Simulator, Config
{
public:
	Simulator(int argc, char **argv, const sc_core::sc_module_name& name = "HARDWARE");
	virtual ~Simulator();
	void Run();
	virtual unisim::kernel::Simulator::SetupStatus Setup();
	virtual bool EndSetup();
protected:
private:
	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm2::memory::ram::Memory<MCI_CONFIG::MCI_WIDTH * 8, FSB_ADDRESS_TYPE, FSB_BURST_SIZE / MCI_CONFIG::MCI_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_INFORMATION> RAM;
	typedef unisim::component::tlm2::memory::ram::Memory<MPLB_CONFIG::OUTPUT_BUSWIDTH, FSB_ADDRESS_TYPE, FSB_BURST_SIZE / (MPLB_CONFIG::OUTPUT_BUSWIDTH / 8), unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_INFORMATION> BRAM;
	typedef unisim::component::tlm2::processor::powerpc::book_e::ppc440::CPU CPU;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<MPLB_CONFIG> MPLB;
	typedef unisim::component::tlm2::interrupt::xilinx::xps_intc::XPS_IntC<INTC_CONFIG> INTC;
	typedef unisim::component::tlm2::timer::xilinx::xps_timer::XPS_Timer<TIMER_CONFIG> TIMER;
	typedef unisim::component::tlm2::memory::flash::am29::AM29<AM29_CONFIG, 32 * unisim::component::cxx::memory::flash::am29::M, 2, MPLB_CONFIG::OUTPUT_BUSWIDTH> FLASH;
	typedef unisim::component::tlm2::interconnect::xilinx::dcr_controller::DCRController<DCR_CONTROLLER_CONFIG> DCR_CONTROLLER;
	typedef unisim::component::tlm2::interconnect::xilinx::crossbar::Crossbar<CROSSBAR_CONFIG> CROSSBAR;
	typedef unisim::component::tlm2::interconnect::xilinx::mci::MCI<MCI_CONFIG> MCI;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<MEMORY_ROUTER_CONFIG> MEMORY_ROUTER;
	typedef unisim::component::tlm2::com::xilinx::xps_uart_lite::XPS_UARTLite<UART_LITE_CONFIG> UART_LITE;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::XPS_GPIO<GPIO_DIP_SWITCHES_8BIT_CONFIG> GPIO_DIP_SWITCHES_8BIT;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::XPS_GPIO<GPIO_LEDS_8BIT_CONFIG> GPIO_LEDS_8BIT;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::XPS_GPIO<GPIO_5_LEDS_POSITIONS_CONFIG> GPIO_5_LEDS_POSITIONS;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::XPS_GPIO<GPIO_PUSH_BUTTONS_5BIT_CONFIG> GPIO_PUSH_BUTTONS_5BIT;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::GPIO_Switches<8> DIP_SWITCHES_8BIT;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::GPIO_LEDs<8> LEDS_8BIT;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::GPIO_LEDs<5> _5_LEDS_POSITIONS;
	typedef unisim::component::tlm2::com::xilinx::xps_gpio::GPIO_Switches<5> PUSH_BUTTONS_5BIT;
	typedef unisim::kernel::tlm2::TargetStub<0, unisim::component::tlm2::timer::xilinx::xps_timer::PWMProtocolTypes> PWM_STUB;
	typedef unisim::kernel::tlm2::TargetStub<0, unisim::component::tlm2::timer::xilinx::xps_timer::GenerateOutProtocolTypes> GENERATE_OUT_STUB;
	typedef unisim::component::tlm2::timer::xilinx::xps_timer::CaptureTriggerStub CAPTURE_TRIGGER_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<0, unisim::component::tlm2::timer::xilinx::xps_timer::InterruptProtocolTypes> IRQ_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<MPLB_CONFIG::OUTPUT_BUSWIDTH> SPLB0_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<MPLB_CONFIG::OUTPUT_BUSWIDTH> SPLB1_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<4> MASTER1_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> APU_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DMAC0_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DMAC1_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DMAC2_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DMAC3_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> EXTERNAL_SLAVE_DCR_STUB;
	typedef unisim::kernel::tlm2::TargetStub<0, unisim::component::tlm2::com::xilinx::xps_gpio::GPIOProtocolTypes> GPIO_OUTPUT_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<0, unisim::component::tlm2::com::xilinx::xps_gpio::GPIOProtocolTypes> GPIO_INPUT_STUB;
	typedef unisim::kernel::tlm2::TargetStub<4> DCR_SLAVE_STUB;

	//=========================================================================
	//===                      Aliases for services classes                 ===
	//=========================================================================

	typedef unisim::service::debug::debugger::Debugger<DEBUGGER_CONFIG> DEBUGGER;
	typedef unisim::service::debug::inline_debugger::InlineDebugger<CPU_ADDRESS_TYPE> INLINE_DEBUGGER;
	typedef unisim::service::debug::gdb_server::GDBServer<CPU_ADDRESS_TYPE> GDB_SERVER;
	typedef unisim::service::loader::multiformat_loader::MultiFormatLoader<CPU_ADDRESS_TYPE> LOADER;
	typedef unisim::service::netstreamer::NetStreamer NETSTREAMER;
	typedef unisim::service::debug::profiler::Profiler<CPU_ADDRESS_TYPE> PROFILER;
	typedef unisim::service::http_server::HttpServer HTTP_SERVER;
	typedef unisim::service::instrumenter::Instrumenter INSTRUMENTER;
	typedef unisim::service::tee::char_io::Tee<2> CHAR_IO_TEE;
	typedef unisim::service::web_terminal::WebTerminal WEB_TERMINAL;
	typedef unisim::service::os::linux_os::PowerPCLinux32<CPU_ADDRESS_TYPE, CPU_ADDRESS_TYPE> LINUX_OS;
	typedef unisim::service::debug::user_interface::UserInterface<CPU_ADDRESS_TYPE> DEBUG_UI;
	typedef unisim::kernel::logger::console::Printer LOGGER_CONSOLE_PRINTER;
	typedef unisim::kernel::logger::text_file::Writer LOGGER_TEXT_FILE_WRITER;
	typedef unisim::kernel::logger::http::Writer LOGGER_HTTP_WRITER;
	typedef unisim::kernel::logger::xml_file::Writer LOGGER_XML_FILE_WRITER;
	typedef unisim::kernel::logger::netstream::Writer LOGGER_NETSTREAM_WRITER;

	//=========================================================================
	//===                           Components                              ===
	//=========================================================================
	//  - PowerPC processor
	CPU *cpu;
	//  - RAM
	RAM *ram;
	//  - BRAM
	BRAM *bram;
	// - IRQ stubs
	IRQ_STUB *input_interrupt_stub[INTC_CONFIG::C_NUM_INTR_INPUTS];
	IRQ_STUB *critical_input_interrupt_stub;
	IRQ_STUB *external_input_interrupt_stub;
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
	// - MCI
	MCI *mci;
	// - UART Lite
	UART_LITE *uart_lite;
	// - GPIO DIP switches 8 Bit
	GPIO_DIP_SWITCHES_8BIT *gpio_dip_switches_8bit;
	// - GPIO LEDs 8 Bit
	GPIO_LEDS_8BIT *gpio_leds_8bit;
	// - GPIO 5 LEDs Positions
	GPIO_5_LEDS_POSITIONS *gpio_5_leds_positions;
	// - GPIO Push Buttons 5 bit
	GPIO_PUSH_BUTTONS_5BIT *gpio_push_buttons_5bit;
	// - DIP Switches 8 bit
	DIP_SWITCHES_8BIT *dip_switches_8bit;
	// - LEDs 8 bit
	LEDS_8BIT *leds_8bit;
	// - 5 LEDs Positions
	_5_LEDS_POSITIONS *_5_leds_positions;
	// - Push buttons 5 bit
	PUSH_BUTTONS_5BIT *push_buttons_5bit;
	// - DCR stubs
	MASTER1_DCR_STUB *master1_dcr_stub;
	APU_DCR_STUB *apu_dcr_stub;
	DMAC0_DCR_STUB *dmac0_dcr_stub;
	DMAC1_DCR_STUB *dmac1_dcr_stub;
	DMAC2_DCR_STUB *dmac2_dcr_stub;
	DMAC3_DCR_STUB *dmac3_dcr_stub;
	EXTERNAL_SLAVE_DCR_STUB *external_slave_dcr_stub;
	DCR_SLAVE_STUB *dcr_slave_stub; // used in combination with Linux loader and ABI translator
	// - Memory router (used in combination with Linux loader and ABI translator)
	MEMORY_ROUTER *memory_router;
	
	//=========================================================================
	//===                            Services                               ===
	//=========================================================================
	//  - Multiformat loader
	LOADER *loader;
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
	//  - memory address translator from effective address to physical address
	unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE> *ram_effective_to_physical_address_translator;
	unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE> *flash_effective_to_physical_address_translator;
	unisim::service::translator::memory_address::memory::Translator<CPU_ADDRESS_TYPE, FSB_ADDRESS_TYPE> *bram_effective_to_physical_address_translator;
	// - netstreamer
	NETSTREAMER *netstreamer;
	// - profiler
	PROFILER *profiler;
	// - Instrumenter
	INSTRUMENTER *instrumenter;
	// - HTTP server
	HTTP_SERVER *http_server;
	// - Web Terminal
	WEB_TERMINAL *web_terminal;
	// - Char I/O Tee
	CHAR_IO_TEE *char_io_tee;
	// - Linux OS
	LINUX_OS *linux_os;
	// - Debug UI
	DEBUG_UI *debug_ui;
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
	bool enable_linux_os;
	bool enable_debug_ui;
	unisim::kernel::variable::Parameter<bool> param_enable_gdb_server;
	unisim::kernel::variable::Parameter<bool> param_enable_inline_debugger;
	unisim::kernel::variable::Parameter<bool> param_enable_profiler;
	unisim::kernel::variable::Parameter<bool> param_enable_linux_os;
	unisim::kernel::variable::Parameter<bool> param_enable_debug_ui;

	static void LoadBuiltInConfig(unisim::kernel::Simulator *simulator);
	virtual void SigInt();
};

#endif // __VIRTEX5FXT_SIMULATOR_HH__
