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

#ifndef __MPC5777M_SIMULATOR_HH__
#define __MPC5777M_SIMULATOR_HH__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// Class definition of components
#include <unisim/component/tlm2/processor/powerpc/e200/mpc57xx/e200z710n3/cpu.hh>
#include <unisim/component/tlm2/processor/powerpc/e200/mpc57xx/e200z425bn3/cpu.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.hh>
#include <unisim/component/tlm2/interconnect/generic_router/config.hh>
#include <unisim/component/tlm2/interconnect/freescale/mpc57xx/xbar/xbar.hh>
#include <unisim/component/tlm2/interconnect/freescale/mpc57xx/pbridge/pbridge.hh>
#include <unisim/component/tlm2/interconnect/freescale/mpc57xx/ebi/ebi.hh>
#include <unisim/component/tlm2/interrupt/freescale/mpc57xx/intc/intc.hh>
#include <unisim/component/tlm2/timer/freescale/mpc57xx/stm/stm.hh>
#include <unisim/component/tlm2/watchdog/freescale/mpc57xx/swt/swt.hh>
#include <unisim/component/tlm2/timer/freescale/mpc57xx/pit/pit.hh>
#include <unisim/component/tlm2/com/freescale/mpc57xx/linflexd/linflexd.hh>
#include <unisim/component/tlm2/com/serial_terminal/serial_terminal.hh>
#include <unisim/component/tlm2/dma/freescale/mpc57xx/dmamux/dmamux.hh>
#include <unisim/component/tlm2/dma/freescale/mpc57xx/edma/edma.hh>
#include <unisim/component/tlm2/operators/associative_operator.hh>
#include <unisim/component/tlm2/com/freescale/mpc57xx/dspi/dspi.hh>
#include <unisim/component/tlm2/com/freescale/mpc57xx/siul2/siul2.hh>
#include <unisim/component/tlm2/com/bosch/m_can/m_can.hh>
#include <unisim/component/tlm2/memory/semaphore/freescale/mpc57xx/sema42/sema42.hh>

// Class definition of kernel, services and interfaces
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/tlm2/simulator.hh>
#include <unisim/kernel/logger/console/console_printer.hh>
#include <unisim/kernel/logger/text_file/text_file_writer.hh>
#include <unisim/kernel/logger/http/http_writer.hh>
#include <unisim/kernel/logger/xml_file/xml_file_writer.hh>
#include <unisim/kernel/logger/netstream/netstream_writer.hh>
#include <unisim/util/backtrace/backtrace.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/debug/profiler/profiler.hh>
#include <unisim/service/loader/multiformat_loader/multiformat_loader.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/netstreamer/netstreamer.hh>
#include <unisim/service/http_server/http_server.hh>
#include <unisim/service/instrumenter/instrumenter.hh>
#include <unisim/service/tee/char_io/tee.hh>
#include <unisim/service/web_terminal/web_terminal.hh>
#include <unisim/kernel/tlm2/tlm.hh>

// Compile time configuration
#include <config.hh>

#if HAVE_TVS
// Timed Value Streams
#include <tvs/tracing.h>
#endif

// Host machine standard headers
#include <fstream>
#include <string>

#ifdef WIN32

#include <winsock2.h>
#include <windows.h>

#else
#include <signal.h>
#endif

//=========================================================================
//===                        Top level class                            ===
//=========================================================================

class Simulator : public unisim::kernel::tlm2::Simulator, Config
{
public:
	Simulator(int argc, char **argv, const sc_core::sc_module_name& name = "HARDWARE");
	virtual ~Simulator();
	virtual bool EndSetup();
	void Run();
	virtual unisim::kernel::Simulator::SetupStatus Setup();
protected:
private:
	
	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm2::memory::ram::Memory<XBAR_0_CONFIG::OUTPUT_BUSWIDTH, uint32_t, FSB_BURST_SIZE / (XBAR_0_CONFIG::OUTPUT_BUSWIDTH / 8), unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> SYSTEM_SRAM;
	typedef unisim::component::tlm2::memory::ram::Memory<XBAR_0_CONFIG::OUTPUT_BUSWIDTH, uint32_t, FSB_BURST_SIZE / (XBAR_0_CONFIG::OUTPUT_BUSWIDTH / 8), unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> FLASH;
	typedef unisim::component::tlm2::processor::powerpc::e200::mpc57xx::e200z710n3::CPU Main_Core_0;
	typedef unisim::component::tlm2::processor::powerpc::e200::mpc57xx::e200z710n3::CPU Main_Core_1;
	typedef unisim::component::tlm2::processor::powerpc::e200::mpc57xx::e200z425bn3::CPU Peripheral_Core_2;
	typedef unisim::component::tlm2::interconnect::freescale::mpc57xx::xbar::XBAR<XBAR_0_CONFIG> XBAR_0;
	typedef unisim::component::tlm2::interconnect::freescale::mpc57xx::xbar::XBAR<XBAR_1_CONFIG> XBAR_1;
	typedef unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::PBRIDGE<PBRIDGE_A_CONFIG> PBRIDGE_A;
	typedef unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::PBRIDGE<PBRIDGE_B_CONFIG> PBRIDGE_B;
	typedef unisim::component::tlm2::interconnect::freescale::mpc57xx::ebi::EBI<EBI_CONFIG> EBI;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<XBAR_1_M1_CONCENTRATOR_CONFIG> XBAR_1_M1_CONCENTRATOR;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<IAHBG_0_CONFIG> IAHBG_0;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<IAHBG_1_CONFIG> IAHBG_1;
	typedef unisim::component::tlm2::interrupt::freescale::mpc57xx::intc::INTC<INTC_0_CONFIG> INTC_0;
	typedef unisim::component::tlm2::timer::freescale::mpc57xx::stm::STM<STM_0_CONFIG> STM_0;
	typedef unisim::component::tlm2::timer::freescale::mpc57xx::stm::STM<STM_1_CONFIG> STM_1;
	typedef unisim::component::tlm2::timer::freescale::mpc57xx::stm::STM<STM_2_CONFIG> STM_2;
	typedef unisim::component::tlm2::watchdog::freescale::mpc57xx::swt::SWT<SWT_0_CONFIG> SWT_0;
	typedef unisim::component::tlm2::watchdog::freescale::mpc57xx::swt::SWT<SWT_1_CONFIG> SWT_1;
	typedef unisim::component::tlm2::watchdog::freescale::mpc57xx::swt::SWT<SWT_2_CONFIG> SWT_2;
	typedef unisim::component::tlm2::watchdog::freescale::mpc57xx::swt::SWT<SWT_3_CONFIG> SWT_3;
	typedef unisim::component::tlm2::timer::freescale::mpc57xx::pit::PIT<PIT_0_CONFIG> PIT_0;
	typedef unisim::component::tlm2::timer::freescale::mpc57xx::pit::PIT<PIT_1_CONFIG> PIT_1;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::linflexd::LINFlexD<LINFlexD_0_CONFIG> LINFlexD_0;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::linflexd::LINFlexD<LINFlexD_1_CONFIG> LINFlexD_1;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::linflexd::LINFlexD<LINFlexD_2_CONFIG> LINFlexD_2;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::linflexd::LINFlexD<LINFlexD_14_CONFIG> LINFlexD_14;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::linflexd::LINFlexD<LINFlexD_15_CONFIG> LINFlexD_15;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::linflexd::LINFlexD<LINFlexD_16_CONFIG> LINFlexD_16;
	typedef unisim::component::tlm2::com::serial_terminal::SerialTerminal SERIAL_TERMINAL;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_0_CONFIG> DMAMUX_0;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_1_CONFIG> DMAMUX_1;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_2_CONFIG> DMAMUX_2;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_3_CONFIG> DMAMUX_3;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_4_CONFIG> DMAMUX_4;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_5_CONFIG> DMAMUX_5;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_6_CONFIG> DMAMUX_6;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_7_CONFIG> DMAMUX_7;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_8_CONFIG> DMAMUX_8;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::dmamux::DMAMUX<DMAMUX_9_CONFIG> DMAMUX_9;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::edma::EDMA<EDMA_0_CONFIG> EDMA_0;
	typedef unisim::component::tlm2::dma::freescale::mpc57xx::edma::EDMA<EDMA_1_CONFIG> EDMA_1;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::dspi::DSPI<DSPI_0_CONFIG> DSPI_0;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::dspi::DSPI<DSPI_1_CONFIG> DSPI_1;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::dspi::DSPI<DSPI_2_CONFIG> DSPI_2;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::dspi::DSPI<DSPI_3_CONFIG> DSPI_3;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::dspi::DSPI<DSPI_4_CONFIG> DSPI_4;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::dspi::DSPI<DSPI_5_CONFIG> DSPI_5;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::dspi::DSPI<DSPI_6_CONFIG> DSPI_6;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::dspi::DSPI<DSPI_12_CONFIG> DSPI_12;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_0_TX_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_0_RX_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_1_TX_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_1_RX_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_2_TX_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_2_RX_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_14_TX_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_14_RX_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_15_TX_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_15_RX_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_16_TX_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus LINFlexD_16_RX_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_0_SOUT_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_0_SIN_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_0_PCS_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_0_SS_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_1_SOUT_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_1_SIN_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_1_PCS_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_1_SS_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_2_SOUT_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_2_SIN_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_2_PCS_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_2_SS_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_3_SOUT_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_3_SIN_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_3_PCS_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_3_SS_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_4_SOUT_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_4_SIN_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_4_PCS_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_4_SS_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_5_SOUT_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_5_SIN_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_5_PCS_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_5_SS_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_6_SOUT_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_6_SIN_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_6_PCS_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_6_SS_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_12_SOUT_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_12_SIN_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_12_PCS_SERIAL_BUS;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_12_SS_SERIAL_BUS;
	typedef unisim::component::tlm2::com::freescale::mpc57xx::siul2::SIUL2<SIUL2_CONFIG> SIUL2;
	
	//typedef unisim::component::tlm2::com::freescale::mpc57xx::can::m_ttcan::m_ttcan<CONFIG::M_TTCAN_0_CONFIG> M_TTCAN_0;
	typedef unisim::component::tlm2::com::bosch::m_can::M_CAN<M_CAN_1_CONFIG> M_CAN_1;
	typedef unisim::component::tlm2::com::bosch::m_can::M_CAN<M_CAN_2_CONFIG> M_CAN_2;
	typedef unisim::component::tlm2::com::bosch::m_can::M_CAN<M_CAN_3_CONFIG> M_CAN_3;
	typedef unisim::component::tlm2::com::bosch::m_can::M_CAN<M_CAN_4_CONFIG> M_CAN_4;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<SHARED_CAN_MESSAGE_RAM_ROUTER_CONFIG> SHARED_CAN_MESSAGE_RAM_ROUTER;
	typedef unisim::component::tlm2::memory::ram::Memory<PBRIDGE_A_CONFIG::OUTPUT_BUSWIDTH, uint32_t, FSB_BURST_SIZE / (PBRIDGE_A_CONFIG::OUTPUT_BUSWIDTH / 8), unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> SHARED_CAN_MESSAGE_RAM;
	
	typedef unisim::kernel::tlm2::tlm_can_bus CAN_BUS;

	typedef unisim::component::tlm2::memory::semaphore::freescale::mpc57xx::sema42::SEMA42<SEMA4_CONFIG> SEMA4;
	
	typedef unisim::component::tlm2::memory::ram::Memory<EBI_CONFIG::OUTPUT_BUSWIDTH, uint32_t, FSB_BURST_SIZE / (EBI_CONFIG::OUTPUT_BUSWIDTH / 8), unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> EBI_MEM;
	//typedef unisim::component::tlm2::memory::ram::Memory<XBAR_0_CONFIG::OUTPUT_BUSWIDTH, uint32_t, FSB_BURST_SIZE / (XBAR_0_CONFIG::OUTPUT_BUSWIDTH / 8), unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> EBI_SPACE_STUB;
	typedef unisim::kernel::tlm2::TargetStub<XBAR_0_CONFIG::OUTPUT_BUSWIDTH> FLASH_PORT1_STUB;
	typedef unisim::kernel::tlm2::TargetStub<XBAR_0_CONFIG::OUTPUT_BUSWIDTH> XBAR_0_S6_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<XBAR_1_CONFIG::OUTPUT_BUSWIDTH> XBAR_1_M2_STUB;
	
	typedef unisim::component::tlm2::memory::ram::Memory<PBRIDGE_A_CONFIG::OUTPUT_BUSWIDTH, uint32_t, 1, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> PCM_STUB;
	typedef unisim::component::tlm2::memory::ram::Memory<PBRIDGE_A_CONFIG::OUTPUT_BUSWIDTH, uint32_t, 1, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> PFLASH_STUB;
	typedef unisim::component::tlm2::memory::ram::Memory<PBRIDGE_A_CONFIG::OUTPUT_BUSWIDTH, uint32_t, 1, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> MC_ME_STUB;
	typedef unisim::component::tlm2::memory::ram::Memory<PBRIDGE_A_CONFIG::OUTPUT_BUSWIDTH, uint32_t, 1, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> MC_CGM_STUB;
	typedef unisim::component::tlm2::memory::ram::Memory<PBRIDGE_A_CONFIG::OUTPUT_BUSWIDTH, uint32_t, 1, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> MC_RGM_STUB;
	typedef unisim::component::tlm2::memory::ram::Memory<PBRIDGE_A_CONFIG::OUTPUT_BUSWIDTH, uint32_t, 1, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> PRAMC_STUB;
	//typedef unisim::component::tlm2::memory::ram::Memory<PBRIDGE_A_CONFIG::OUTPUT_BUSWIDTH, uint32_t, 1, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, DEBUG_ENABLE> EBI_STUB;


	//=========================================================================
	//===                      Aliases for services classes                 ===
	//=========================================================================

	typedef unisim::service::debug::debugger::Debugger<DEBUGGER_CONFIG> DEBUGGER;
	typedef unisim::service::debug::inline_debugger::InlineDebugger<CPU_ADDRESS_TYPE> INLINE_DEBUGGER;
	typedef unisim::service::debug::gdb_server::GDBServer<CPU_ADDRESS_TYPE> GDB_SERVER;
	typedef unisim::service::debug::profiler::Profiler<CPU_ADDRESS_TYPE> PROFILER;
	typedef unisim::service::loader::multiformat_loader::MultiFormatLoader<CPU_ADDRESS_TYPE> LOADER;
	typedef unisim::service::netstreamer::NetStreamer NETSTREAMER;
	typedef unisim::service::http_server::HttpServer HTTP_SERVER;
	typedef unisim::service::instrumenter::Instrumenter INSTRUMENTER;
	typedef unisim::service::tee::char_io::Tee<2> CHAR_IO_TEE;
	typedef unisim::service::web_terminal::WebTerminal WEB_TERMINAL;
	typedef unisim::kernel::logger::console::Printer LOGGER_CONSOLE_PRINTER;
	typedef unisim::kernel::logger::text_file::Writer LOGGER_TEXT_FILE_WRITER;
	typedef unisim::kernel::logger::http::Writer LOGGER_HTTP_WRITER;
	typedef unisim::kernel::logger::xml_file::Writer LOGGER_XML_FILE_WRITER;
	typedef unisim::kernel::logger::netstream::Writer LOGGER_NETSTREAM_WRITER;
	
	//=========================================================================
	//===                           Components                              ===
	//=========================================================================
	//  - PowerPC cores
	Main_Core_0 *main_core_0;
	Main_Core_1 *main_core_1;
	Peripheral_Core_2 *peripheral_core_2;
	//  - System SRAM
	SYSTEM_SRAM *system_sram;
	//  - FLASH
	FLASH *flash;
	//  - Crossbars
	XBAR_0 *xbar_0;
	XBAR_1 *xbar_1;
	//  - Peripheral Bridges
	PBRIDGE_A *pbridge_a;
	PBRIDGE_B *pbridge_b;
	//  - External Bus interface
	EBI *ebi;
	//  - Concentrators
	XBAR_1_M1_CONCENTRATOR *xbar_1_m1_concentrator;
	//  - Intelligent Bus Bridging Gaskets
	IAHBG_0 *iahbg_0;
	IAHBG_1 *iahbg_1;
	//  - Interrupt Controller
	INTC_0 *intc_0;
	//  - System Timer Modules
	STM_0 *stm_0;
	STM_1 *stm_1;
	STM_2 *stm_2;
	//  - Software Watchdog Timers
	SWT_0 *swt_0;
	SWT_1 *swt_1;
	SWT_2 *swt_2;
	SWT_3 *swt_3;
	//  - Periodic Interrupt Timers
	PIT_0 *pit_0;
	PIT_1 *pit_1;
	//  - LINFlexD
	LINFlexD_0 *linflexd_0;
	LINFlexD_1 *linflexd_1;
	LINFlexD_2 *linflexd_2;
	LINFlexD_14 *linflexd_14;
	LINFlexD_15 *linflexd_15;
	LINFlexD_16 *linflexd_16;
	//  - LINFlexD serial buses
	LINFlexD_0_TX_SERIAL_BUS *linflexd_0_tx_serial_bus;
	LINFlexD_0_RX_SERIAL_BUS *linflexd_0_rx_serial_bus;
	LINFlexD_1_TX_SERIAL_BUS *linflexd_1_tx_serial_bus;
	LINFlexD_1_RX_SERIAL_BUS *linflexd_1_rx_serial_bus;
	LINFlexD_2_TX_SERIAL_BUS *linflexd_2_tx_serial_bus;
	LINFlexD_2_RX_SERIAL_BUS *linflexd_2_rx_serial_bus;
	LINFlexD_14_TX_SERIAL_BUS *linflexd_14_tx_serial_bus;
	LINFlexD_14_RX_SERIAL_BUS *linflexd_14_rx_serial_bus;
	LINFlexD_15_TX_SERIAL_BUS *linflexd_15_tx_serial_bus;
	LINFlexD_15_RX_SERIAL_BUS *linflexd_15_rx_serial_bus;
	LINFlexD_16_TX_SERIAL_BUS *linflexd_16_tx_serial_bus;
	LINFlexD_16_RX_SERIAL_BUS *linflexd_16_rx_serial_bus;
	//  - Serial Terminal
	SERIAL_TERMINAL *serial_terminal0;
	SERIAL_TERMINAL *serial_terminal1;
	SERIAL_TERMINAL *serial_terminal2;
	SERIAL_TERMINAL *serial_terminal14;
	SERIAL_TERMINAL *serial_terminal15;
	SERIAL_TERMINAL *serial_terminal16;
	//  - DMAMUX
	DMAMUX_0 *dmamux_0;
	DMAMUX_1 *dmamux_1;
	DMAMUX_2 *dmamux_2;
	DMAMUX_3 *dmamux_3;
	DMAMUX_4 *dmamux_4;
	DMAMUX_5 *dmamux_5;
	DMAMUX_6 *dmamux_6;
	DMAMUX_7 *dmamux_7;
	DMAMUX_8 *dmamux_8;
	DMAMUX_9 *dmamux_9;
	//  - EDMA
	EDMA_0 *edma_0;
	EDMA_1 *edma_1;
	//  - DSPI
	DSPI_0 *dspi_0;
	DSPI_1 *dspi_1;
	DSPI_2 *dspi_2;
	DSPI_3 *dspi_3;
	DSPI_4 *dspi_4;
	DSPI_5 *dspi_5;
	DSPI_6 *dspi_6;
	DSPI_12 *dspi_12;
	//  - DSPI serial buses
	DSPI_0_SOUT_SERIAL_BUS  *dspi_0_sout_serial_bus;
	DSPI_0_SIN_SERIAL_BUS   *dspi_0_sin_serial_bus;
	DSPI_0_PCS_SERIAL_BUS   *dspi_0_pcs_serial_bus[DSPI_0::NUM_CTARS];
	DSPI_0_SS_SERIAL_BUS    *dspi_0_ss_serial_bus;
	DSPI_1_SOUT_SERIAL_BUS  *dspi_1_sout_serial_bus;
	DSPI_1_SIN_SERIAL_BUS   *dspi_1_sin_serial_bus;
	DSPI_1_PCS_SERIAL_BUS   *dspi_1_pcs_serial_bus[DSPI_1::NUM_CTARS];
	DSPI_1_SS_SERIAL_BUS    *dspi_1_ss_serial_bus;
	DSPI_2_SOUT_SERIAL_BUS  *dspi_2_sout_serial_bus;
	DSPI_2_SIN_SERIAL_BUS   *dspi_2_sin_serial_bus;
	DSPI_2_PCS_SERIAL_BUS   *dspi_2_pcs_serial_bus[DSPI_2::NUM_CTARS];
	DSPI_2_SS_SERIAL_BUS    *dspi_2_ss_serial_bus;
	DSPI_3_SOUT_SERIAL_BUS  *dspi_3_sout_serial_bus;
	DSPI_3_SIN_SERIAL_BUS   *dspi_3_sin_serial_bus;
	DSPI_3_PCS_SERIAL_BUS   *dspi_3_pcs_serial_bus[DSPI_3::NUM_CTARS];
	DSPI_3_SS_SERIAL_BUS    *dspi_3_ss_serial_bus;
	DSPI_4_SOUT_SERIAL_BUS  *dspi_4_sout_serial_bus;
	DSPI_4_SIN_SERIAL_BUS   *dspi_4_sin_serial_bus;
	DSPI_4_PCS_SERIAL_BUS   *dspi_4_pcs_serial_bus[DSPI_4::NUM_CTARS];
	DSPI_4_SS_SERIAL_BUS    *dspi_4_ss_serial_bus;
	DSPI_5_SOUT_SERIAL_BUS  *dspi_5_sout_serial_bus;
	DSPI_5_SIN_SERIAL_BUS   *dspi_5_sin_serial_bus;
	DSPI_5_PCS_SERIAL_BUS   *dspi_5_pcs_serial_bus[DSPI_5::NUM_CTARS];
	DSPI_5_SS_SERIAL_BUS    *dspi_5_ss_serial_bus;
	DSPI_6_SOUT_SERIAL_BUS  *dspi_6_sout_serial_bus;
	DSPI_6_SIN_SERIAL_BUS   *dspi_6_sin_serial_bus;
	DSPI_6_PCS_SERIAL_BUS   *dspi_6_pcs_serial_bus[DSPI_6::NUM_CTARS];
	DSPI_6_SS_SERIAL_BUS    *dspi_6_ss_serial_bus;
	DSPI_12_SOUT_SERIAL_BUS *dspi_12_sout_serial_bus;
	DSPI_12_SIN_SERIAL_BUS  *dspi_12_sin_serial_bus;
	DSPI_12_PCS_SERIAL_BUS  *dspi_12_pcs_serial_bus[DSPI_12::NUM_CTARS];
	DSPI_12_SS_SERIAL_BUS   *dspi_12_ss_serial_bus;
	// - SIUL2
	SIUL2 *siul2;
	// - CAN Subsystem
// 	M_TTCAN_0 *m_ttcan_0;
	M_CAN_1 *m_can_1;
	M_CAN_2 *m_can_2;
	M_CAN_3 *m_can_3;
	M_CAN_4 *m_can_4;
	SHARED_CAN_MESSAGE_RAM_ROUTER *shared_can_message_ram_router;
	SHARED_CAN_MESSAGE_RAM *shared_can_message_ram;
	CAN_BUS *can_bus;
	
	// - Semaphores2
	SEMA4 *sema4;
	
	//  - Stubs
	EBI_MEM *ebi_mem_0;
	EBI_MEM *ebi_mem_1;
	EBI_MEM *ebi_mem_2;
	FLASH_PORT1_STUB *flash_port1_stub;
	XBAR_0_S6_STUB *xbar_0_s6_stub;
	XBAR_1_M2_STUB *xbar_1_m2_stub;
	PCM_STUB *pcm_stub;
	PFLASH_STUB *pflash_stub;
	MC_ME_STUB *mc_me_stub;
	MC_CGM_STUB *mc_cgm_stub;
	MC_RGM_STUB *mc_rgm_stub;
	PRAMC_STUB *pram_0_stub;
	
	unisim::component::tlm2::operators::LogicalOrOperator<bool, NUM_DMA_CHANNELS> *dma_err_irq_combinator;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 3> *DSPI0_0;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 3> *DSPI1_0;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 3> *DSPI2_0;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 3> *DSPI3_0;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 3> *DSPI4_0;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 2> *DSPI4_5;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 2> *DSPI4_6;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 2> *DSPI4_7;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 3> *DSPI5_0;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 2> *DSPI5_5;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 2> *DSPI5_6;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 2> *DSPI5_7;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 3> *DSPI6_0;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 2> *DSPI6_5;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 2> *DSPI6_6;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 2> *DSPI6_7;
	unisim::component::tlm2::operators::LogicalOrOperator<bool, 3> *DSPI12_0;
	
	//=========================================================================
	//===                            Services                               ===
	//=========================================================================
	//  - Loader
	LOADER *loader;
	//  - Debugger
	DEBUGGER *debugger;
	//  - GDB server
	GDB_SERVER *gdb_server[NUM_PROCESSORS];
	//  - Inline debugger
	INLINE_DEBUGGER *inline_debugger[NUM_PROCESSORS];
	//  - profiler
	PROFILER *profiler[NUM_PROCESSORS];
	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *sim_time;
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time;
	//  - NetStreamer
	NETSTREAMER *netstreamer0;
	NETSTREAMER *netstreamer1;
	NETSTREAMER *netstreamer2;
	NETSTREAMER *netstreamer14;
	NETSTREAMER *netstreamer15;
	NETSTREAMER *netstreamer16;
	//  - Http Server
	HTTP_SERVER *http_server;
	//  - Instrumenter
	INSTRUMENTER *instrumenter;
	//  - Char I/O tees
	CHAR_IO_TEE *char_io_tee0;
	CHAR_IO_TEE *char_io_tee1;
	CHAR_IO_TEE *char_io_tee2;
	CHAR_IO_TEE *char_io_tee14;
	CHAR_IO_TEE *char_io_tee15;
	CHAR_IO_TEE *char_io_tee16;
	//  - Web Terminals
	WEB_TERMINAL *web_terminal0;
	WEB_TERMINAL *web_terminal1;
	WEB_TERMINAL *web_terminal2;
	WEB_TERMINAL *web_terminal14;
	WEB_TERMINAL *web_terminal15;
	WEB_TERMINAL *web_terminal16;
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
	
	bool enable_core0_reset;
	bool enable_core1_reset;
	bool enable_core2_reset;
	sc_core::sc_time core0_reset_time;
	sc_core::sc_time core1_reset_time;
	sc_core::sc_time core2_reset_time;
	bool enable_gdb_server;
	bool enable_inline_debugger;
	bool enable_profiler0;
	bool enable_profiler1;
	bool enable_profiler2;
	bool enable_serial_terminal0;
	bool enable_serial_terminal1;
	bool enable_serial_terminal2;
	bool enable_serial_terminal14;
	bool enable_serial_terminal15;
	bool enable_serial_terminal16;
	bool enable_web_terminal0;
	bool enable_web_terminal1;
	bool enable_web_terminal2;
	bool enable_web_terminal14;
	bool enable_web_terminal15;
	bool enable_web_terminal16;
	
	bool dspi_0_is_slave;
	bool dspi_1_is_slave;
	bool dspi_2_is_slave;
	bool dspi_3_is_slave;
	bool dspi_4_is_slave;
	bool dspi_5_is_slave;
	bool dspi_6_is_slave;
	bool dspi_12_is_slave;
	unsigned int dspi_0_master;
	unsigned int dspi_1_master;
	unsigned int dspi_2_master;
	unsigned int dspi_3_master;
	unsigned int dspi_4_master;
	unsigned int dspi_5_master;
	unsigned int dspi_6_master;
	unsigned int dspi_12_master;
	unsigned int dspi_0_slave;
	unsigned int dspi_1_slave;
	unsigned int dspi_2_slave;
	unsigned int dspi_3_slave;
	unsigned int dspi_4_slave;
	unsigned int dspi_5_slave;
	unsigned int dspi_6_slave;
	unsigned int dspi_12_slave;
	
	unisim::kernel::variable::Parameter<bool> param_enable_core0_reset;
	unisim::kernel::variable::Parameter<bool> param_enable_core1_reset;
	unisim::kernel::variable::Parameter<bool> param_enable_core2_reset;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_core0_reset_time;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_core1_reset_time;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_core2_reset_time;
	unisim::kernel::variable::Parameter<bool> param_enable_gdb_server;
	unisim::kernel::variable::Parameter<bool> param_enable_inline_debugger;
	unisim::kernel::variable::Parameter<bool> param_enable_profiler0;
	unisim::kernel::variable::Parameter<bool> param_enable_profiler1;
	unisim::kernel::variable::Parameter<bool> param_enable_profiler2;
	unisim::kernel::variable::Parameter<bool> param_enable_serial_terminal0;
	unisim::kernel::variable::Parameter<bool> param_enable_serial_terminal1;
	unisim::kernel::variable::Parameter<bool> param_enable_serial_terminal2;
	unisim::kernel::variable::Parameter<bool> param_enable_serial_terminal14;
	unisim::kernel::variable::Parameter<bool> param_enable_serial_terminal15;
	unisim::kernel::variable::Parameter<bool> param_enable_serial_terminal16;
	unisim::kernel::variable::Parameter<bool> param_enable_web_terminal0;
	unisim::kernel::variable::Parameter<bool> param_enable_web_terminal1;
	unisim::kernel::variable::Parameter<bool> param_enable_web_terminal2;
	unisim::kernel::variable::Parameter<bool> param_enable_web_terminal14;
	unisim::kernel::variable::Parameter<bool> param_enable_web_terminal15;
	unisim::kernel::variable::Parameter<bool> param_enable_web_terminal16;

	unisim::kernel::variable::Parameter<bool> param_dspi_0_is_slave;
	unisim::kernel::variable::Parameter<bool> param_dspi_1_is_slave;
	unisim::kernel::variable::Parameter<bool> param_dspi_2_is_slave;
	unisim::kernel::variable::Parameter<bool> param_dspi_3_is_slave;
	unisim::kernel::variable::Parameter<bool> param_dspi_4_is_slave;
	unisim::kernel::variable::Parameter<bool> param_dspi_5_is_slave;
	unisim::kernel::variable::Parameter<bool> param_dspi_6_is_slave;
	unisim::kernel::variable::Parameter<bool> param_dspi_12_is_slave;
	unisim::kernel::variable::Parameter<unsigned int> param_dspi_0_master;
	unisim::kernel::variable::Parameter<unsigned int> param_dspi_1_master;
	unisim::kernel::variable::Parameter<unsigned int> param_dspi_2_master;
	unisim::kernel::variable::Parameter<unsigned int> param_dspi_3_master;
	unisim::kernel::variable::Parameter<unsigned int> param_dspi_4_master;
	unisim::kernel::variable::Parameter<unsigned int> param_dspi_5_master;
	unisim::kernel::variable::Parameter<unsigned int> param_dspi_6_master;
	unisim::kernel::variable::Parameter<unsigned int> param_dspi_12_master;
	unisim::kernel::variable::Parameter<unsigned int> param_dspi_0_slave;
	unisim::kernel::variable::Parameter<unsigned int> param_dspi_1_slave;
	unisim::kernel::variable::Parameter<unsigned int> param_dspi_2_slave;
	unisim::kernel::variable::Parameter<unsigned int> param_dspi_3_slave;
	unisim::kernel::variable::Parameter<unsigned int> param_dspi_4_slave;
	unisim::kernel::variable::Parameter<unsigned int> param_dspi_5_slave;
	unisim::kernel::variable::Parameter<unsigned int> param_dspi_6_slave;
	unisim::kernel::variable::Parameter<unsigned int> param_dspi_12_slave;

#if HAVE_TVS
	std::string bandwidth_vcd_filename;
	unisim::kernel::variable::Parameter<std::string> param_bandwidth_vcd_filename;
	std::string bandwidth_gtkwave_init_script;
	unisim::kernel::variable::Parameter<std::string> param_bandwidth_gtkwave_init_script;
	std::ofstream *bandwidth_vcd_file;
	tracing::timed_stream_vcd_processor *bandwidth_vcd;
#endif
	
	static void LoadBuiltInConfig(unisim::kernel::Simulator *simulator);
	
	void Core0ResetProcess();
	void Core1ResetProcess();
	void Core2ResetProcess();
	
	void InterruptSource(unsigned int irq_num, const std::string& source = std::string());
	void LogicalOr2(sc_core::sc_signal<bool>& in0, sc_core::sc_signal<bool>& in1, sc_core::sc_signal<bool>& out);
	void LogicalOr3(sc_core::sc_signal<bool>& in0, sc_core::sc_signal<bool>& in1, sc_core::sc_signal<bool>& in2, sc_core::sc_signal<bool>& out);
	void DMASource(unsigned int dmamux_num, unsigned int dma_source_num);
	void DMASource(const std::string& source_req, const std::string& source_ack, unsigned int dmamux_num, unsigned int dma_source_num);
	void DMASource(const std::string& source_req, const std::string& source_ack, unsigned int dmamux_num0, unsigned int dma_source_num0, unsigned int dmamux_num1, unsigned int dma_source_num1);
	void DMASource(const std::string& source_req, const std::string& source_ack, unsigned int dmamux_num0, unsigned int dma_source_num0, unsigned int dmamux_num1, unsigned int dma_source_num1, unsigned int dmamux_num2, unsigned int dma_source_num2);
	
	virtual void SigInt();
};

#endif // __MPC5777M_SIMULATOR_HH__
