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

#include <simulator.hh>
#include <unisim/kernel/config/xml/xml_config_file_helper.hh>
#include <unisim/kernel/config/ini/ini_config_file_helper.hh>
#include <unisim/kernel/config/json/json_config_file_helper.hh>
#include <sstream>
#include <stdexcept>
#include <map>

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

using unisim::service::instrumenter::OUTPUT_INSTRUMENTATION;

Simulator::Simulator(int argc, char **argv, const sc_core::sc_module_name& name)
	: unisim::kernel::tlm2::Simulator(name, argc, argv, LoadBuiltInConfig)
	, main_core_0(0)
	, main_core_1(0)
	, peripheral_core_2(0)
	, system_sram(0)
	, flash(0)
	, xbar_0(0)
	, xbar_1(0)
	, pbridge_a(0)
	, pbridge_b(0)
	, ebi(0)
	, xbar_1_m1_concentrator(0)
	, intc_0(0)
	, stm_0(0)
	, stm_1(0)
	, stm_2(0)
	, swt_0(0)
	, swt_1(0)
	, swt_2(0)
	, swt_3(0)
	, pit_0(0)
	, pit_1(0)
	, linflexd_0(0)
	, linflexd_1(0)
	, linflexd_2(0)
	, linflexd_14(0)
	, linflexd_15(0)
	, linflexd_16(0)
	, linflexd_0_tx_serial_bus(0)
	, linflexd_0_rx_serial_bus(0)
	, linflexd_1_tx_serial_bus(0)
	, linflexd_1_rx_serial_bus(0)
	, linflexd_2_tx_serial_bus(0)
	, linflexd_2_rx_serial_bus(0)
	, linflexd_14_tx_serial_bus(0)
	, linflexd_14_rx_serial_bus(0)
	, linflexd_15_tx_serial_bus(0)
	, linflexd_15_rx_serial_bus(0)
	, linflexd_16_tx_serial_bus(0)
	, linflexd_16_rx_serial_bus(0)
	, serial_terminal0(0)
	, serial_terminal1(0)
	, serial_terminal2(0)
	, serial_terminal14(0)
	, serial_terminal15(0)
	, serial_terminal16(0)
	, dmamux_0(0)
	, dmamux_1(0)
	, dmamux_2(0)
	, dmamux_3(0)
	, dmamux_4(0)
	, dmamux_5(0)
	, dmamux_6(0)
	, dmamux_7(0)
	, dmamux_8(0)
	, dmamux_9(0)
	, edma_0(0)
	, edma_1(0)
	, dspi_0(0)
	, dspi_1(0)
	, dspi_2(0)
	, dspi_3(0)
	, dspi_4(0)
	, dspi_5(0)
	, dspi_6(0)
	, dspi_12(0)
	, dspi_0_sout_serial_bus(0)
	, dspi_0_sin_serial_bus(0)
	, dspi_0_pcs_serial_bus()
	, dspi_0_ss_serial_bus(0)
	, dspi_1_sout_serial_bus(0)
	, dspi_1_sin_serial_bus(0)
	, dspi_1_pcs_serial_bus()
	, dspi_1_ss_serial_bus(0)
	, dspi_2_sout_serial_bus(0)
	, dspi_2_sin_serial_bus(0)
	, dspi_2_pcs_serial_bus()
	, dspi_2_ss_serial_bus(0)
	, dspi_3_sout_serial_bus(0)
	, dspi_3_sin_serial_bus(0)
	, dspi_3_pcs_serial_bus()
	, dspi_3_ss_serial_bus(0)
	, dspi_4_sout_serial_bus(0)
	, dspi_4_sin_serial_bus(0)
	, dspi_4_pcs_serial_bus()
	, dspi_4_ss_serial_bus(0)
	, dspi_5_sout_serial_bus(0)
	, dspi_5_sin_serial_bus(0)
	, dspi_5_pcs_serial_bus()
	, dspi_5_ss_serial_bus(0)
	, dspi_6_sout_serial_bus(0)
	, dspi_6_sin_serial_bus(0)
	, dspi_6_pcs_serial_bus()
	, dspi_6_ss_serial_bus(0)
	, dspi_12_sout_serial_bus(0)
	, dspi_12_sin_serial_bus(0)
	, dspi_12_pcs_serial_bus()
	, dspi_12_ss_serial_bus(0)
	, siul2(0)
	//,m_ttcan_0(0)
	, m_can_1(0)
	, m_can_2(0)
	, m_can_3(0)
	, m_can_4(0)
	, shared_can_message_ram_router(0)
	, shared_can_message_ram(0)
	, can_bus(0)
	, sema4(0)
	, ebi_mem_0(0)
	, ebi_mem_1(0)
	, ebi_mem_2(0)
	, flash_port1_stub(0)
	, xbar_0_s6_stub(0)
	, xbar_1_m2_stub(0)
	, pcm_stub(0)
	, pflash_stub(0)
	, mc_me_stub(0)
	, mc_cgm_stub(0)
	, mc_rgm_stub(0)
	, pram_0_stub(0)
	, dma_err_irq_combinator(0)
	, DSPI0_0(0)
	, DSPI1_0(0)
	, DSPI2_0(0)
	, DSPI3_0(0)
	, DSPI4_0(0)
	, DSPI4_5(0)
	, DSPI4_6(0)
	, DSPI4_7(0)
	, DSPI5_0(0)
	, DSPI5_5(0)
	, DSPI5_6(0)
	, DSPI5_7(0)
	, DSPI6_0(0)
	, DSPI6_5(0)
	, DSPI6_6(0)
	, DSPI6_7(0)
	, DSPI12_0(0)
	, loader(0)
	, debugger(0)
	, gdb_server()
	, inline_debugger()
	, profiler()
	, sim_time(0)
	, host_time(0)
	, netstreamer0(0)
	, netstreamer1(0)
	, netstreamer2(0)
	, netstreamer14(0)
	, netstreamer15(0)
	, netstreamer16(0)
	, http_server(0)
	, instrumenter(0)
	, char_io_tee0(0)
	, char_io_tee1(0)
	, char_io_tee2(0)
	, char_io_tee14(0)
	, char_io_tee15(0)
	, char_io_tee16(0)
	, web_terminal0(0)
	, web_terminal1(0)
	, web_terminal2(0)
	, web_terminal14(0)
	, web_terminal15(0)
	, web_terminal16(0)
	, logger_console_printer(0)
	, logger_text_file_writer(0)
	, logger_http_writer(0)
	, logger_xml_file_writer(0)
	, logger_netstream_writer(0)
	, enable_core0_reset(true)
	, enable_core1_reset(true)
	, enable_core2_reset(true)
	, core0_reset_time(sc_core::sc_time(1000, sc_core::SC_NS))
	, core1_reset_time(sc_core::sc_time(1000, sc_core::SC_NS))
	, core2_reset_time(sc_core::SC_ZERO_TIME)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, enable_profiler0(false)
	, enable_profiler1(false)
	, enable_profiler2(false)
	, enable_serial_terminal0(false)
	, enable_serial_terminal1(false)
	, enable_serial_terminal2(false)
	, enable_serial_terminal14(false)
	, enable_serial_terminal15(false)
	, enable_serial_terminal16(false)
	, enable_web_terminal0(false)
	, enable_web_terminal1(false)
	, enable_web_terminal2(false)
	, enable_web_terminal14(false)
	, enable_web_terminal15(false)
	, enable_web_terminal16(false)
	, dspi_0_is_slave(false)
	, dspi_1_is_slave(false)
	, dspi_2_is_slave(false)
	, dspi_3_is_slave(false)
	, dspi_4_is_slave(false)
	, dspi_5_is_slave(false)
	, dspi_6_is_slave(false)
	, dspi_12_is_slave(false)
	, dspi_0_master(0)
	, dspi_1_master(0)
	, dspi_2_master(0)
	, dspi_3_master(0)
	, dspi_4_master(0)
	, dspi_5_master(0)
	, dspi_6_master(0)
	, dspi_12_master(0)
	, dspi_0_slave(0)
	, dspi_1_slave(0)
	, dspi_2_slave(0)
	, dspi_3_slave(0)
	, dspi_4_slave(0)
	, dspi_5_slave(0)
	, dspi_6_slave(0)
	, dspi_12_slave(0)
	, param_enable_core0_reset("enable-core0-reset", this, enable_core0_reset, "Enable/Disable Core #0 reset")
	, param_enable_core1_reset("enable-core1-reset", this, enable_core1_reset, "Enable/Disable Core #1 reset")
	, param_enable_core2_reset("enable-core2-reset", this, enable_core2_reset, "Enable/Disable Core #2 reset")
	, param_core0_reset_time("core0-reset-time", this, core0_reset_time, "When Core #0 receives reset signal")
	, param_core1_reset_time("core1-reset-time", this, core1_reset_time, "When Core #1 receives reset signal")
	, param_core2_reset_time("core2-reset-time", this, core2_reset_time, "When Core #2 receives reset signal")
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, param_enable_profiler0("enable-profiler0", 0, enable_profiler0, "Enable/Disable profiling of Core #0")
	, param_enable_profiler1("enable-profiler1", 0, enable_profiler1, "Enable/Disable profiling of Core #1")
	, param_enable_profiler2("enable-profiler2", 0, enable_profiler2, "Enable/Disable profiling of Core #2")
	, param_enable_serial_terminal0("enable-serial-terminal0", 0, enable_serial_terminal0, "Enable/Disable serial terminal over LINFlexD_0 UART serial interface")
	, param_enable_serial_terminal1("enable-serial-terminal1", 0, enable_serial_terminal1, "Enable/Disable serial terminal over LINFlexD_1 UART serial interface")
	, param_enable_serial_terminal2("enable-serial-terminal2", 0, enable_serial_terminal2, "Enable/Disable serial terminal over LINFlexD_2 UART serial interface")
	, param_enable_serial_terminal14("enable-serial-terminal14", 0, enable_serial_terminal14, "Enable/Disable serial terminal over LINFlexD_14 UART serial interface")
	, param_enable_serial_terminal15("enable-serial-terminal15", 0, enable_serial_terminal15, "Enable/Disable serial terminal over LINFlexD_15 UART serial interface")
	, param_enable_serial_terminal16("enable-serial-terminal16", 0, enable_serial_terminal16, "Enable/Disable serial terminal over LINFlexD_16 UART serial interface")
	, param_enable_web_terminal0("enable-web-terminal0", 0, enable_web_terminal0, "Enable/Disable Web terminal over LINFlexD_0 UART serial interface")
	, param_enable_web_terminal1("enable-web-terminal1", 0, enable_web_terminal1, "Enable/Disable Web terminal over LINFlexD_1 UART serial interface")
	, param_enable_web_terminal2("enable-web-terminal2", 0, enable_web_terminal2, "Enable/Disable Web terminal over LINFlexD_2 UART serial interface")
	, param_enable_web_terminal14("enable-web-terminal14", 0, enable_web_terminal14, "Enable/Disable Web terminal over LINFlexD_14 UART serial interface")
	, param_enable_web_terminal15("enable-web-terminal15", 0, enable_web_terminal15, "Enable/Disable Web terminal over LINFlexD_15 UART serial interface")
	, param_enable_web_terminal16("enable-web-terminal16", 0, enable_web_terminal16, "Enable/Disable Web terminal over LINFlexD_16 UART serial interface")
	, param_dspi_0_is_slave("dspi_0-is-slave", 0, dspi_0_is_slave, "Whether to wire DSPI_0 as a slave")
	, param_dspi_1_is_slave("dspi_1-is-slave", 0, dspi_1_is_slave, "Whether to wire DSPI_1 as a slave")
	, param_dspi_2_is_slave("dspi_2-is-slave", 0, dspi_2_is_slave, "Whether to wire DSPI_2 as a slave")
	, param_dspi_3_is_slave("dspi_3-is-slave", 0, dspi_3_is_slave, "Whether to wire DSPI_3 as a slave")
	, param_dspi_4_is_slave("dspi_4-is-slave", 0, dspi_4_is_slave, "Whether to wire DSPI_4 as a slave")
	, param_dspi_5_is_slave("dspi_5-is-slave", 0, dspi_5_is_slave, "Whether to wire DSPI_5 as a slave")
	, param_dspi_6_is_slave("dspi_6-is-slave", 0, dspi_6_is_slave, "Whether to wire DSPI_6 as a slave")
	, param_dspi_12_is_slave("dspi_12-is-slave", 0, dspi_12_is_slave, "Whether to wire DSPI_12 as a slave")
	, param_dspi_0_master("dspi_0-master", 0, dspi_0_master, "Master number of DSPI_0 when DSPI_0 is wired as a slave")
	, param_dspi_1_master("dspi_1-master", 0, dspi_1_master, "Master number of DSPI_1 when DSPI_1 is wired as a slave")
	, param_dspi_2_master("dspi_2-master", 0, dspi_2_master, "Master number of DSPI_2 when DSPI_2 is wired as a slave")
	, param_dspi_3_master("dspi_3-master", 0, dspi_3_master, "Master number of DSPI_3 when DSPI_3 is wired as a slave")
	, param_dspi_4_master("dspi_4-master", 0, dspi_4_master, "Master number of DSPI_4 when DSPI_4 is wired as a slave")
	, param_dspi_5_master("dspi_5-master", 0, dspi_5_master, "Master number of DSPI_5 when DSPI_5 is wired as a slave")
	, param_dspi_6_master("dspi_6-master", 0, dspi_6_master, "Master number of DSPI_6 when DSPI_6 is wired as a slave")
	, param_dspi_12_master("dspi_12-master", 0, dspi_12_master, "Master number of DSPI_12 when DSPI_12 is wired as a slave")
	, param_dspi_0_slave("dspi_0-slave", 0, dspi_0_slave, "Assigned slave number (in master) of DSPI_0 when DSPI_0 is wired as a slave")
	, param_dspi_1_slave("dspi_1-slave", 0, dspi_1_slave, "Assigned slave number (in master) of DSPI_1 when DSPI_1 is wired as a slave")
	, param_dspi_2_slave("dspi_2-slave", 0, dspi_2_slave, "Assigned slave number (in master) of DSPI_2 when DSPI_2 is wired as a slave")
	, param_dspi_3_slave("dspi_3-slave", 0, dspi_3_slave, "Assigned slave number (in master) of DSPI_3 when DSPI_3 is wired as a slave")
	, param_dspi_4_slave("dspi_4-slave", 0, dspi_4_slave, "Assigned slave number (in master) of DSPI_4 when DSPI_4 is wired as a slave")
	, param_dspi_5_slave("dspi_5-slave", 0, dspi_5_slave, "Assigned slave number (in master) of DSPI_5 when DSPI_5 is wired as a slave")
	, param_dspi_6_slave("dspi_6-slave", 0, dspi_6_slave, "Assigned slave number (in master) of DSPI_6 when DSPI_6 is wired as a slave")
	, param_dspi_12_slave("dspi_12-slave", 0, dspi_12_slave, "Assigned slave number (in master) of DSPI_12 when DSPI_12 is wired as a slave")
#if HAVE_TVS
	, bandwidth_vcd_filename("bandwidth.vcd")
	, param_bandwidth_vcd_filename("bandwidth-vcd-filename", this, bandwidth_vcd_filename, "Filename of VCD file where to trace interconnects bandwidth")
	, bandwidth_gtkwave_init_script()
	, param_bandwidth_gtkwave_init_script("bandwidth-gtkwave-init-script", this, bandwidth_gtkwave_init_script, "GTKWave initialization script that simulators must automatically generate at startup for best viewing interconnects bandwidth VCD trace")
	, bandwidth_vcd_file(0)
	, bandwidth_vcd(0)
#endif
{
	SetDescription("MPC5777M Simulator");
	
	param_dspi_0_master.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_dspi_1_master.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_dspi_2_master.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_dspi_3_master.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_dspi_4_master.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_dspi_5_master.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_dspi_6_master.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_dspi_12_master.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_dspi_0_slave.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_dspi_1_slave.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_dspi_2_slave.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_dspi_3_slave.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_dspi_4_slave.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_dspi_5_slave.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_dspi_6_slave.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_dspi_12_slave.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

	param_enable_gdb_server.SetMutable(false);
	param_enable_inline_debugger.SetMutable(false);
	param_enable_profiler0.SetMutable(false);
	param_enable_profiler1.SetMutable(false);
	param_enable_profiler2.SetMutable(false);
	param_enable_serial_terminal0.SetMutable(false);
	param_enable_serial_terminal1.SetMutable(false);
	param_enable_serial_terminal2.SetMutable(false);
	param_enable_serial_terminal14.SetMutable(false);
	param_enable_serial_terminal15.SetMutable(false);
	param_enable_serial_terminal16.SetMutable(false);
	param_dspi_0_is_slave.SetMutable(false);
	param_dspi_1_is_slave.SetMutable(false);
	param_dspi_2_is_slave.SetMutable(false);
	param_dspi_3_is_slave.SetMutable(false);
	param_dspi_4_is_slave.SetMutable(false);
	param_dspi_5_is_slave.SetMutable(false);
	param_dspi_6_is_slave.SetMutable(false);
	param_dspi_12_is_slave.SetMutable(false);
	param_dspi_0_master.SetMutable(false);
	param_dspi_1_master.SetMutable(false);
	param_dspi_2_master.SetMutable(false);
	param_dspi_3_master.SetMutable(false);
	param_dspi_4_master.SetMutable(false);
	param_dspi_5_master.SetMutable(false);
	param_dspi_6_master.SetMutable(false);
	param_dspi_12_master.SetMutable(false);
	param_dspi_0_slave.SetMutable(false);
	param_dspi_1_slave.SetMutable(false);
	param_dspi_2_slave.SetMutable(false);
	param_dspi_3_slave.SetMutable(false);
	param_dspi_4_slave.SetMutable(false);
	param_dspi_5_slave.SetMutable(false);
	param_dspi_6_slave.SetMutable(false);
	param_dspi_12_slave.SetMutable(false);
#if HAVE_TVS
	param_bandwidth_vcd_filename.SetMutable(false);
	param_bandwidth_gtkwave_init_script.SetMutable(false);
#endif
	
	if(enable_web_terminal0) enable_serial_terminal0 = true;
	if(enable_web_terminal1) enable_serial_terminal1 = true;
	if(enable_web_terminal2) enable_serial_terminal2 = true;
	if(enable_web_terminal14) enable_serial_terminal14 = true;
	if(enable_web_terminal15) enable_serial_terminal15 = true;
	if(enable_web_terminal16) enable_serial_terminal16 = true;
	
	unsigned int channel_num;
	unsigned int hw_irq_num;
	unsigned int prc_num;
	unsigned int dma_req_num;
	unsigned int dma_source_num;
	unsigned int dma_always_num;
	unsigned int dma_trigger_num;
	unsigned int dma_channel_num;
	unsigned int i;

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
	//  - PowerPC e200 processors
	main_core_0 = new Main_Core_0("Main_Core_0", this);
	main_core_1 = new Main_Core_1("Main_Core_1", this);
	peripheral_core_2 = new Peripheral_Core_2("Peripheral_Core_2", this);

	//  - System SRAM
	system_sram = new SYSTEM_SRAM("SYSTEM-SRAM", this);
	
	//  - FLASH
	flash = new FLASH("FLASH", this);
	
	//  - Crossbars
	xbar_0 = new XBAR_0("XBAR_0", this);
	xbar_1 = new XBAR_1("XBAR_1", this);
	
	//  - Peripheral Bridges
	pbridge_a = new PBRIDGE_A("PBRIDGE_A", this); 
	pbridge_b = new PBRIDGE_B("PBRIDGE_B", this); 
	
	//  - External Bus Interface
	ebi = new EBI("EBI", this);
	
	//  - Concentrators
	xbar_1_m1_concentrator = new XBAR_1_M1_CONCENTRATOR("XBAR_1_M1_CONCENTRATOR", this);
	
	//  - Intelligent Bus Bridging Gaskets
	iahbg_0 = new IAHBG_0("IAHBG_0", this);
	iahbg_1 = new IAHBG_1("IAHBG_1", this);
	
	//  - Interrupt Controller
	intc_0 = new INTC_0("INTC_0", this);
	
	//  - System Timer Module
	stm_0 = new STM_0("STM_0", this);
	stm_1 = new STM_1("STM_1", this);
	stm_2 = new STM_2("STM_2", this);

	//  - Software Watchdog Timers
	swt_0 = new SWT_0("SWT_0", this);
	swt_1 = new SWT_1("SWT_1", this);
	swt_2 = new SWT_2("SWT_2", this);
	swt_3 = new SWT_3("SWT_3", this);
	
	//  - Periodic Interrupt Timers
	pit_0 = new PIT_0("PIT_0", this);
	pit_1 = new PIT_1("PIT_1", this);
	
	//  - LINFlexD
	linflexd_0 = new LINFlexD_0("LINFlexD_0", this);
	linflexd_1 = new LINFlexD_1("LINFlexD_1", this);
	linflexd_2 = new LINFlexD_2("LINFlexD_2", this);
	linflexd_14 = new LINFlexD_14("LINFlexD_14", this);
	linflexd_15 = new LINFlexD_15("LINFlexD_15", this);
	linflexd_16 = new LINFlexD_16("LINFlexD_16", this);
	
	//  - Serial Terminal
	serial_terminal0 = enable_serial_terminal0 ? new SERIAL_TERMINAL("SERIAL_TERMINAL0", this) : 0;
	serial_terminal1 = enable_serial_terminal1 ? new SERIAL_TERMINAL("SERIAL_TERMINAL1", this) : 0;
	serial_terminal2 = enable_serial_terminal2 ? new SERIAL_TERMINAL("SERIAL_TERMINAL2", this) : 0;
	serial_terminal14 = enable_serial_terminal14 ? new SERIAL_TERMINAL("SERIAL_TERMINAL14", this) : 0;
	serial_terminal15 = enable_serial_terminal15 ? new SERIAL_TERMINAL("SERIAL_TERMINAL15", this) : 0;
	serial_terminal16 = enable_serial_terminal16 ? new SERIAL_TERMINAL("SERIAL_TERMINAL16", this) : 0;
	
	//  - DMAMUX
	dmamux_0 = new DMAMUX_0("DMAMUX_0", this);
	dmamux_1 = new DMAMUX_1("DMAMUX_1", this);
	dmamux_2 = new DMAMUX_2("DMAMUX_2", this);
	dmamux_3 = new DMAMUX_3("DMAMUX_3", this);
	dmamux_4 = new DMAMUX_4("DMAMUX_4", this);
	dmamux_5 = new DMAMUX_5("DMAMUX_5", this);
	dmamux_6 = new DMAMUX_6("DMAMUX_6", this);
	dmamux_7 = new DMAMUX_7("DMAMUX_7", this);
	dmamux_8 = new DMAMUX_8("DMAMUX_8", this);
	dmamux_9 = new DMAMUX_9("DMAMUX_9", this);
	
	//  - EDMA
	edma_0 = new EDMA_0("eDMA_0", this);
	edma_1 = new EDMA_1("eDMA_1", this);
	
	//  - DSPI
	dspi_0 = new DSPI_0("DSPI_0", this);
	dspi_1 = new DSPI_1("DSPI_1", this);
	dspi_2 = new DSPI_2("DSPI_2", this);
	dspi_3 = new DSPI_3("DSPI_3", this);
	dspi_4 = new DSPI_4("DSPI_4", this);
	dspi_5 = new DSPI_5("DSPI_5", this);
	dspi_6 = new DSPI_6("DSPI_6", this);
	dspi_12 = new DSPI_12("DSPI_12", this);
	
	// - SIUL2
	siul2 = new SIUL2("SIUL2", this);
	
	// - CAN subsystem
	//m_ttcan_0 = new M_TTCAN_0("M_TTCAN_0", this);
	m_can_1 = new M_CAN_1("M_CAN_1", this);
	m_can_2 = new M_CAN_2("M_CAN_2", this);
	m_can_3 = new M_CAN_3("M_CAN_3", this);
	m_can_4 = new M_CAN_4("M_CAN_4", this);
	shared_can_message_ram_router = new SHARED_CAN_MESSAGE_RAM_ROUTER("SHARED_CAN_MESSAGE_RAM_ROUTER", this);
	shared_can_message_ram = new SHARED_CAN_MESSAGE_RAM("SHARED_CAN_MESSAGE_RAM", this);
	
	std::vector<sc_core::sc_signal<bool> *> CAN_TX;
	CAN_TX.push_back(&instrumenter->CreateSignal("CAN_TX_1", true, OUTPUT_INSTRUMENTATION));
	CAN_TX.push_back(&instrumenter->CreateSignal("CAN_TX_2", true, OUTPUT_INSTRUMENTATION));
	CAN_TX.push_back(&instrumenter->CreateSignal("CAN_TX_3", true, OUTPUT_INSTRUMENTATION));
	CAN_TX.push_back(&instrumenter->CreateSignal("CAN_TX_4", true, OUTPUT_INSTRUMENTATION));
	
	can_bus = new CAN_BUS("CAN_BUS", instrumenter->CreateSignal("CAN_RX", true, OUTPUT_INSTRUMENTATION), CAN_TX, this);
	
	// - Semaphores2
	sema4 = new SEMA4("SEMA4", this);
	
	//  - Stubs
	ebi_mem_0 = new EBI_MEM("EBI_MEM_0", this);
	ebi_mem_1 = new EBI_MEM("EBI_MEM_1", this);
	ebi_mem_2 = new EBI_MEM("EBI_MEM_2", this);
	flash_port1_stub = new FLASH_PORT1_STUB("FLASH_PORT1", this);
	xbar_0_s6_stub = new XBAR_0_S6_STUB("XBAR_0_S6", this);
	xbar_1_m2_stub = new XBAR_1_M2_STUB("XBAR_1_M2", this);
	pcm_stub = new PCM_STUB("PCM", this);
	pflash_stub = new PFLASH_STUB("PFLASH", this);
	mc_me_stub = new MC_ME_STUB("MC_ME", this);
	mc_cgm_stub = new MC_CGM_STUB("MC_CGM", this);
	mc_rgm_stub = new MC_RGM_STUB("MC_RGM", this);
	pram_0_stub = new PRAMC_STUB("PRAM_0", this);

	dma_err_irq_combinator = new unisim::component::tlm2::operators::LogicalOrOperator<bool, NUM_DMA_CHANNELS>("DMA_ERR_IRQ_COMBINATOR");

	DSPI0_0 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 3>("DSPI0_0");
	DSPI1_0 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 3>("DSPI1_0");
	DSPI2_0 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 3>("DSPI2_0");
	DSPI3_0 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 3>("DSPI3_0");
	DSPI4_0 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 3>("DSPI4_0");
	DSPI4_5 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 2>("DSPI4_5");
	DSPI4_6 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 2>("DSPI4_6");
	DSPI4_7 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 2>("DSPI4_7");
	DSPI5_0 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 3>("DSPI5_0");
	DSPI5_5 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 2>("DSPI5_5");
	DSPI5_6 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 2>("DSPI5_6");
	DSPI5_7 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 2>("DSPI5_7");
	DSPI6_0 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 3>("DSPI6_0");
	DSPI6_5 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 2>("DSPI6_5");
	DSPI6_6 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 2>("DSPI6_6");
	DSPI6_7 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 2>("DSPI6_7");
	DSPI12_0 = new unisim::component::tlm2::operators::LogicalOrOperator<bool, 3>("DSPI12_0");
	
	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - Multiformat loader
	loader = new LOADER("loader");
	//  - debugger
	debugger = (enable_inline_debugger || enable_gdb_server || (enable_profiler0 || enable_profiler1 || enable_profiler2)) ? new DEBUGGER("debugger") : 0;
	//  - GDB server
	gdb_server[0] = enable_gdb_server ? new GDB_SERVER("gdb-server0") : 0;
	gdb_server[1] = enable_gdb_server ? new GDB_SERVER("gdb-server1") : 0;
	gdb_server[2] = enable_gdb_server ? new GDB_SERVER("gdb-server2") : 0;
	//  - Inline debugger
	inline_debugger[0] = enable_inline_debugger ? new INLINE_DEBUGGER("inline-debugger0") : 0;
	inline_debugger[1] = enable_inline_debugger ? new INLINE_DEBUGGER("inline-debugger1") : 0;
	inline_debugger[2] = enable_inline_debugger ? new INLINE_DEBUGGER("inline-debugger2") : 0;
	//  - profiler
	profiler[0] = enable_profiler0 ? new PROFILER("profiler0") : 0;
	profiler[1] = enable_profiler1 ? new PROFILER("profiler1") : 0;
	profiler[2] = enable_profiler2 ? new PROFILER("profiler2") : 0;
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - NetStreamer
	netstreamer0 = enable_serial_terminal0 ? new NETSTREAMER("netstreamer0") : 0;
	netstreamer1 = enable_serial_terminal1 ? new NETSTREAMER("netstreamer1") : 0;
	netstreamer2 = enable_serial_terminal2 ? new NETSTREAMER("netstreamer2") : 0;
	netstreamer14 = enable_serial_terminal14 ? new NETSTREAMER("netstreamer14") : 0;
	netstreamer15 = enable_serial_terminal15 ? new NETSTREAMER("netstreamer15") : 0;
	netstreamer16 = enable_serial_terminal16 ? new NETSTREAMER("netstreamer16") : 0;
	//  - Http Server
	http_server = new HTTP_SERVER("http-server");
	//  - Char I/O Tees
	char_io_tee0 = new CHAR_IO_TEE("char-io-tee0");
	char_io_tee1 = new CHAR_IO_TEE("char-io-tee1");
	char_io_tee2 = new CHAR_IO_TEE("char-io-tee2");
	char_io_tee14 = new CHAR_IO_TEE("char-io-tee14");
	char_io_tee15 = new CHAR_IO_TEE("char-io-tee15");
	char_io_tee16 = new CHAR_IO_TEE("char-io-tee16");
	//  - Web Terminals
	web_terminal0 = enable_web_terminal0 ? new WEB_TERMINAL("web-terminal0") : 0;
	web_terminal1 = enable_web_terminal1 ? new WEB_TERMINAL("web-terminal1") : 0;
	web_terminal2 = enable_web_terminal2 ? new WEB_TERMINAL("web-terminal2") : 0;
	web_terminal14 = enable_web_terminal14 ? new WEB_TERMINAL("web-terminal14") : 0;
	web_terminal15 = enable_web_terminal15 ? new WEB_TERMINAL("web-terminal15") : 0;
	web_terminal16 = enable_web_terminal16 ? new WEB_TERMINAL("web-terminal16") : 0;
	
	//=========================================================================
	//===                          Port registration                        ===
	//=========================================================================
	// - Main_Core_0
	instrumenter->RegisterPort(main_core_0->m_clk);
	instrumenter->RegisterPort(main_core_0->m_por);
	instrumenter->RegisterPort(main_core_0->p_reset_b);
	instrumenter->RegisterPort(main_core_0->p_nmi_b);
	instrumenter->RegisterPort(main_core_0->p_mcp_b);
	instrumenter->RegisterPort(main_core_0->p_rstbase);
	instrumenter->RegisterPort(main_core_0->p_cpuid);
	instrumenter->RegisterPort(main_core_0->p_extint_b);
	instrumenter->RegisterPort(main_core_0->p_crint_b);
	instrumenter->RegisterPort(main_core_0->p_avec_b);
	instrumenter->RegisterPort(main_core_0->p_voffset);
	instrumenter->RegisterPort(main_core_0->p_iack);
	
	// - Main_Core_1
	instrumenter->RegisterPort(main_core_1->m_clk);
	instrumenter->RegisterPort(main_core_1->m_por);
	instrumenter->RegisterPort(main_core_1->p_reset_b);
	instrumenter->RegisterPort(main_core_1->p_nmi_b);
	instrumenter->RegisterPort(main_core_1->p_mcp_b);
	instrumenter->RegisterPort(main_core_1->p_rstbase);
	instrumenter->RegisterPort(main_core_1->p_cpuid);
	instrumenter->RegisterPort(main_core_1->p_extint_b);
	instrumenter->RegisterPort(main_core_1->p_crint_b);
	instrumenter->RegisterPort(main_core_1->p_avec_b);
	instrumenter->RegisterPort(main_core_1->p_voffset);
	instrumenter->RegisterPort(main_core_1->p_iack);

	// - Peripheral_Core_2
	instrumenter->RegisterPort(peripheral_core_2->m_clk);
	instrumenter->RegisterPort(peripheral_core_2->m_por);
	instrumenter->RegisterPort(peripheral_core_2->p_reset_b);
	instrumenter->RegisterPort(peripheral_core_2->p_nmi_b);
	instrumenter->RegisterPort(peripheral_core_2->p_mcp_b);
	instrumenter->RegisterPort(peripheral_core_2->p_rstbase);
	instrumenter->RegisterPort(peripheral_core_2->p_cpuid);
	instrumenter->RegisterPort(peripheral_core_2->p_extint_b);
	instrumenter->RegisterPort(peripheral_core_2->p_crint_b);
	instrumenter->RegisterPort(peripheral_core_2->p_avec_b);
	instrumenter->RegisterPort(peripheral_core_2->p_voffset);
	instrumenter->RegisterPort(peripheral_core_2->p_iack);
	
	// - XBAR_0
	instrumenter->RegisterPort(xbar_0->m_clk);
	instrumenter->RegisterPort(xbar_0->reset_b);
	instrumenter->RegisterPort(xbar_0->input_if_clock);
	instrumenter->RegisterPort(xbar_0->output_if_clock);
	
	// - XBAR_1
	instrumenter->RegisterPort(xbar_1->m_clk);
	instrumenter->RegisterPort(xbar_1->reset_b);
	instrumenter->RegisterPort(xbar_1->input_if_clock);
	instrumenter->RegisterPort(xbar_1->output_if_clock);

	// - PBRIDGE_A
	instrumenter->RegisterPort(pbridge_a->m_clk);
	instrumenter->RegisterPort(pbridge_a->reset_b);
	instrumenter->RegisterPort(pbridge_a->input_if_clock);
	instrumenter->RegisterPort(pbridge_a->output_if_clock);

	// - PBRIDGE_B
	instrumenter->RegisterPort(pbridge_b->m_clk);
	instrumenter->RegisterPort(pbridge_b->reset_b);
	instrumenter->RegisterPort(pbridge_b->input_if_clock);
	instrumenter->RegisterPort(pbridge_b->output_if_clock);

	// - EBI
	instrumenter->RegisterPort(ebi->m_clk);
	instrumenter->RegisterPort(ebi->reset_b);
	instrumenter->RegisterPort(ebi->input_if_clock);
	instrumenter->RegisterPort(ebi->output_if_clock);

	// - IAHBG_0
	instrumenter->RegisterPort(iahbg_0->input_if_clock);
	instrumenter->RegisterPort(iahbg_0->output_if_clock);

	// - IAHBG_1
	instrumenter->RegisterPort(iahbg_1->input_if_clock);
	instrumenter->RegisterPort(iahbg_1->output_if_clock);

	// - XBAR_1_M1_CONCENTRATOR
	instrumenter->RegisterPort(xbar_1_m1_concentrator->input_if_clock);
	instrumenter->RegisterPort(xbar_1_m1_concentrator->output_if_clock);

	// - INTC_0
	instrumenter->RegisterPort(intc_0->m_clk);
	instrumenter->RegisterPort(intc_0->reset_b);
	
	for(hw_irq_num = 0; hw_irq_num < INTC_0_CONFIG::NUM_HW_IRQS; hw_irq_num++)
	{
		instrumenter->RegisterPort(*intc_0->hw_irq[hw_irq_num]);
	}
	for(prc_num = 0; prc_num < INTC_0_CONFIG::NUM_PROCESSORS; prc_num++)
	{
		instrumenter->RegisterPort(*intc_0->p_iack[prc_num]);
		instrumenter->RegisterPort(*intc_0->p_irq_b[prc_num]);
		instrumenter->RegisterPort(*intc_0->p_avec_b[prc_num]);
		instrumenter->RegisterPort(*intc_0->p_voffset[prc_num]);
	}
	
	// - STM_0
	instrumenter->RegisterPort(stm_0->m_clk);
	instrumenter->RegisterPort(stm_0->reset_b);
	instrumenter->RegisterPort(stm_0->debug);
	for(channel_num = 0; channel_num < STM_0_CONFIG::NUM_CHANNELS; channel_num++)
	{
		instrumenter->RegisterPort(*stm_0->irq[channel_num]);
	}

	// - STM_1
	instrumenter->RegisterPort(stm_1->m_clk);
	instrumenter->RegisterPort(stm_1->reset_b);
	instrumenter->RegisterPort(stm_1->debug);
	for(channel_num = 0; channel_num < STM_1_CONFIG::NUM_CHANNELS; channel_num++)
	{
		instrumenter->RegisterPort(*stm_1->irq[channel_num]);
	}

	// - STM_2
	instrumenter->RegisterPort(stm_2->m_clk);
	instrumenter->RegisterPort(stm_2->reset_b);
	instrumenter->RegisterPort(stm_2->debug);
	for(channel_num = 0; channel_num < STM_2_CONFIG::NUM_CHANNELS; channel_num++)
	{
		instrumenter->RegisterPort(*stm_2->irq[channel_num]);
	}

	// - SWT_0
	instrumenter->RegisterPort(swt_0->m_clk);
	instrumenter->RegisterPort(swt_0->swt_reset_b);
	instrumenter->RegisterPort(swt_0->stop);
	instrumenter->RegisterPort(swt_0->debug);
	instrumenter->RegisterPort(swt_0->irq);
	instrumenter->RegisterPort(swt_0->reset_b);

	// - STM_1
	instrumenter->RegisterPort(swt_1->m_clk);
	instrumenter->RegisterPort(swt_1->swt_reset_b);
	instrumenter->RegisterPort(swt_1->stop);
	instrumenter->RegisterPort(swt_1->debug);
	instrumenter->RegisterPort(swt_1->irq);
	instrumenter->RegisterPort(swt_1->reset_b);

	// - STM_2
	instrumenter->RegisterPort(swt_2->m_clk);
	instrumenter->RegisterPort(swt_2->swt_reset_b);
	instrumenter->RegisterPort(swt_2->stop);
	instrumenter->RegisterPort(swt_2->debug);
	instrumenter->RegisterPort(swt_2->irq);
	instrumenter->RegisterPort(swt_2->reset_b);
	
	// - STM_3
	instrumenter->RegisterPort(swt_3->m_clk);
	instrumenter->RegisterPort(swt_3->swt_reset_b);
	instrumenter->RegisterPort(swt_3->stop);
	instrumenter->RegisterPort(swt_3->debug);
	instrumenter->RegisterPort(swt_3->irq);
	instrumenter->RegisterPort(swt_3->reset_b);

	// - PIT_0
	instrumenter->RegisterPort(pit_0->m_clk);
	instrumenter->RegisterPort(pit_0->per_clk);
	instrumenter->RegisterPort(pit_0->rti_clk);
	instrumenter->RegisterPort(pit_0->reset_b);
	instrumenter->RegisterPort(pit_0->debug);
	for(channel_num = 0; channel_num < PIT_0_CONFIG::MAX_CHANNELS; channel_num++)
	{
		instrumenter->RegisterPort(*pit_0->irq[channel_num]);
		instrumenter->RegisterPort(*pit_0->dma_trigger[channel_num]);
	}
	instrumenter->RegisterPort(pit_0->rtirq);
	
	// - PIT_1
	instrumenter->RegisterPort(pit_1->m_clk);
	instrumenter->RegisterPort(pit_1->per_clk);
	instrumenter->RegisterPort(pit_1->rti_clk);
	instrumenter->RegisterPort(pit_1->reset_b);
	instrumenter->RegisterPort(pit_1->debug);
	for(channel_num = 0; channel_num < PIT_0_CONFIG::MAX_CHANNELS; channel_num++)
	{
		instrumenter->RegisterPort(*pit_1->irq[channel_num]);
		instrumenter->RegisterPort(*pit_1->dma_trigger[channel_num]);
	}
	instrumenter->RegisterPort(pit_1->rtirq);
	
	// - LINFlexD_0
	instrumenter->RegisterPort(linflexd_0->m_clk);
	instrumenter->RegisterPort(linflexd_0->lin_clk);
	instrumenter->RegisterPort(linflexd_0->reset_b);
	instrumenter->RegisterPort(linflexd_0->INT_RX);
	instrumenter->RegisterPort(linflexd_0->INT_TX);
	instrumenter->RegisterPort(linflexd_0->INT_ERR);
	for(dma_req_num = 0; dma_req_num < LINFlexD_0::NUM_DMA_TX_CHANNELS; dma_req_num++)
	{
		instrumenter->RegisterPort(*linflexd_0->DMA_TX[dma_req_num]);
		instrumenter->RegisterPort(*linflexd_0->DMA_ACK_TX[dma_req_num]);
	}
	for(dma_req_num = 0; dma_req_num < LINFlexD_0::NUM_DMA_RX_CHANNELS; dma_req_num++)
	{
		instrumenter->RegisterPort(*linflexd_0->DMA_RX[dma_req_num]);
		instrumenter->RegisterPort(*linflexd_0->DMA_ACK_RX[dma_req_num]);
	}

	// - LINFlexD_1
	instrumenter->RegisterPort(linflexd_1->m_clk);
	instrumenter->RegisterPort(linflexd_1->lin_clk);
	instrumenter->RegisterPort(linflexd_1->reset_b);
	instrumenter->RegisterPort(linflexd_1->INT_RX);
	instrumenter->RegisterPort(linflexd_1->INT_TX);
	instrumenter->RegisterPort(linflexd_1->INT_ERR);
	for(dma_req_num = 0; dma_req_num < LINFlexD_1::NUM_DMA_TX_CHANNELS; dma_req_num++)
	{
		instrumenter->RegisterPort(*linflexd_1->DMA_TX[dma_req_num]);
		instrumenter->RegisterPort(*linflexd_1->DMA_ACK_TX[dma_req_num]);
	}
	for(dma_req_num = 0; dma_req_num < LINFlexD_1::NUM_DMA_RX_CHANNELS; dma_req_num++)
	{
		instrumenter->RegisterPort(*linflexd_1->DMA_RX[dma_req_num]);
		instrumenter->RegisterPort(*linflexd_1->DMA_ACK_RX[dma_req_num]);
	}

	// - LINFlexD_2
	instrumenter->RegisterPort(linflexd_2->m_clk);
	instrumenter->RegisterPort(linflexd_2->lin_clk);
	instrumenter->RegisterPort(linflexd_2->reset_b);
	instrumenter->RegisterPort(linflexd_2->INT_RX);
	instrumenter->RegisterPort(linflexd_2->INT_TX);
	instrumenter->RegisterPort(linflexd_2->INT_ERR);
	for(dma_req_num = 0; dma_req_num < LINFlexD_2::NUM_DMA_TX_CHANNELS; dma_req_num++)
	{
		instrumenter->RegisterPort(*linflexd_2->DMA_TX[dma_req_num]);
		instrumenter->RegisterPort(*linflexd_2->DMA_ACK_TX[dma_req_num]);
	}
	for(dma_req_num = 0; dma_req_num < LINFlexD_2::NUM_DMA_RX_CHANNELS; dma_req_num++)
	{
		instrumenter->RegisterPort(*linflexd_2->DMA_RX[dma_req_num]);
		instrumenter->RegisterPort(*linflexd_2->DMA_ACK_RX[dma_req_num]);
	}

	// - LINFlexD_14
	instrumenter->RegisterPort(linflexd_14->m_clk);
	instrumenter->RegisterPort(linflexd_14->lin_clk);
	instrumenter->RegisterPort(linflexd_14->reset_b);
	instrumenter->RegisterPort(linflexd_14->INT_RX);
	instrumenter->RegisterPort(linflexd_14->INT_TX);
	instrumenter->RegisterPort(linflexd_14->INT_ERR);
	for(dma_req_num = 0; dma_req_num < LINFlexD_14::NUM_DMA_TX_CHANNELS; dma_req_num++)
	{
		instrumenter->RegisterPort(*linflexd_14->DMA_TX[dma_req_num]);
		instrumenter->RegisterPort(*linflexd_14->DMA_ACK_TX[dma_req_num]);
	}
	for(dma_req_num = 0; dma_req_num < LINFlexD_14::NUM_DMA_RX_CHANNELS; dma_req_num++)
	{
		instrumenter->RegisterPort(*linflexd_14->DMA_RX[dma_req_num]);
		instrumenter->RegisterPort(*linflexd_14->DMA_ACK_RX[dma_req_num]);
	}

	// - LINFlexD_15
	instrumenter->RegisterPort(linflexd_15->m_clk);
	instrumenter->RegisterPort(linflexd_15->lin_clk);
	instrumenter->RegisterPort(linflexd_15->reset_b);
	instrumenter->RegisterPort(linflexd_15->INT_RX);
	instrumenter->RegisterPort(linflexd_15->INT_TX);
	instrumenter->RegisterPort(linflexd_15->INT_ERR);
	for(dma_req_num = 0; dma_req_num < LINFlexD_15::NUM_DMA_TX_CHANNELS; dma_req_num++)
	{
		instrumenter->RegisterPort(*linflexd_15->DMA_TX[dma_req_num]);
		instrumenter->RegisterPort(*linflexd_15->DMA_ACK_TX[dma_req_num]);
	}
	for(dma_req_num = 0; dma_req_num < LINFlexD_15::NUM_DMA_RX_CHANNELS; dma_req_num++)
	{
		instrumenter->RegisterPort(*linflexd_15->DMA_RX[dma_req_num]);
		instrumenter->RegisterPort(*linflexd_15->DMA_ACK_RX[dma_req_num]);
	}

	// - LINFlexD_16
	instrumenter->RegisterPort(linflexd_16->m_clk);
	instrumenter->RegisterPort(linflexd_16->lin_clk);
	instrumenter->RegisterPort(linflexd_16->reset_b);
	instrumenter->RegisterPort(linflexd_16->INT_RX);
	instrumenter->RegisterPort(linflexd_16->INT_TX);
	instrumenter->RegisterPort(linflexd_16->INT_ERR);
	for(dma_req_num = 0; dma_req_num < LINFlexD_16::NUM_DMA_TX_CHANNELS; dma_req_num++)
	{
		instrumenter->RegisterPort(*linflexd_16->DMA_TX[dma_req_num]);
		instrumenter->RegisterPort(*linflexd_16->DMA_ACK_TX[dma_req_num]);
	}
	for(dma_req_num = 0; dma_req_num < LINFlexD_16::NUM_DMA_RX_CHANNELS; dma_req_num++)
	{
		instrumenter->RegisterPort(*linflexd_16->DMA_RX[dma_req_num]);
		instrumenter->RegisterPort(*linflexd_16->DMA_ACK_RX[dma_req_num]);
	}
	
	// - SERIAL_TERMINAL
	if(serial_terminal0)
	{
		instrumenter->RegisterPort(serial_terminal0->CLK);
	}
	if(serial_terminal1)
	{
		instrumenter->RegisterPort(serial_terminal1->CLK);
	}
	if(serial_terminal2)
	{
		instrumenter->RegisterPort(serial_terminal2->CLK);
	}
	if(serial_terminal14)
	{
		instrumenter->RegisterPort(serial_terminal14->CLK);
	}
	if(serial_terminal15)
	{
		instrumenter->RegisterPort(serial_terminal15->CLK);
	}
	if(serial_terminal16)
	{
		instrumenter->RegisterPort(serial_terminal16->CLK);
	}

	// - DMAMUX_0
	instrumenter->RegisterPort(dmamux_0->m_clk);
	instrumenter->RegisterPort(dmamux_0->reset_b);
	for(dma_source_num = 0; dma_source_num < DMAMUX_0::NUM_DMA_SOURCES; dma_source_num++)
	{
		instrumenter->RegisterPort(*dmamux_0->dma_source[dma_source_num]);
		instrumenter->RegisterPort(*dmamux_0->dma_source_ack[dma_source_num]);
	}
	for(dma_always_num = 0; dma_always_num < DMAMUX_0::NUM_DMA_ALWAYS_ON; dma_always_num++)
	{
		instrumenter->RegisterPort(*dmamux_0->dma_always_on[dma_always_num]);
	}
	for(dma_trigger_num = 0; dma_trigger_num < DMAMUX_0::NUM_DMA_TRIGGERS; dma_trigger_num++)
	{
		instrumenter->RegisterPort(*dmamux_0->dma_trigger[dma_trigger_num]);
	}
	for(dma_channel_num = 0; dma_channel_num < DMAMUX_0::NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(*dmamux_0->dma_channel[dma_channel_num]);
		instrumenter->RegisterPort(*dmamux_0->dma_channel_ack[dma_channel_num]);
	}
	
	// - DMAMUX_1
	instrumenter->RegisterPort(dmamux_1->m_clk);
	instrumenter->RegisterPort(dmamux_1->reset_b);
	for(dma_source_num = 0; dma_source_num < DMAMUX_1::NUM_DMA_SOURCES; dma_source_num++)
	{
		instrumenter->RegisterPort(*dmamux_1->dma_source[dma_source_num]);
		instrumenter->RegisterPort(*dmamux_1->dma_source_ack[dma_source_num]);
	}
	for(dma_always_num = 0; dma_always_num < DMAMUX_1::NUM_DMA_ALWAYS_ON; dma_always_num++)
	{
		instrumenter->RegisterPort(*dmamux_1->dma_always_on[dma_always_num]);
	}
	for(dma_trigger_num = 0; dma_trigger_num < DMAMUX_1::NUM_DMA_TRIGGERS; dma_trigger_num++)
	{
		instrumenter->RegisterPort(*dmamux_1->dma_trigger[dma_trigger_num]);
	}
	for(dma_channel_num = 0; dma_channel_num < DMAMUX_1::NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(*dmamux_1->dma_channel[dma_channel_num]);
		instrumenter->RegisterPort(*dmamux_1->dma_channel_ack[dma_channel_num]);
	}

	// - DMAMUX_2
	instrumenter->RegisterPort(dmamux_2->m_clk);
	instrumenter->RegisterPort(dmamux_2->reset_b);
	for(dma_source_num = 0; dma_source_num < DMAMUX_2::NUM_DMA_SOURCES; dma_source_num++)
	{
		instrumenter->RegisterPort(*dmamux_2->dma_source[dma_source_num]);
		instrumenter->RegisterPort(*dmamux_2->dma_source_ack[dma_source_num]);
	}
	for(dma_always_num = 0; dma_always_num < DMAMUX_2::NUM_DMA_ALWAYS_ON; dma_always_num++)
	{
		instrumenter->RegisterPort(*dmamux_2->dma_always_on[dma_always_num]);
	}
	for(dma_trigger_num = 0; dma_trigger_num < DMAMUX_2::NUM_DMA_TRIGGERS; dma_trigger_num++)
	{
		instrumenter->RegisterPort(*dmamux_2->dma_trigger[dma_trigger_num]);
	}
	for(dma_channel_num = 0; dma_channel_num < DMAMUX_2::NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(*dmamux_2->dma_channel[dma_channel_num]);
		instrumenter->RegisterPort(*dmamux_2->dma_channel_ack[dma_channel_num]);
	}

	// - DMAMUX_3
	instrumenter->RegisterPort(dmamux_3->m_clk);
	instrumenter->RegisterPort(dmamux_3->reset_b);
	for(dma_source_num = 0; dma_source_num < DMAMUX_3::NUM_DMA_SOURCES; dma_source_num++)
	{
		instrumenter->RegisterPort(*dmamux_3->dma_source[dma_source_num]);
		instrumenter->RegisterPort(*dmamux_3->dma_source_ack[dma_source_num]);
	}
	for(dma_always_num = 0; dma_always_num < DMAMUX_3::NUM_DMA_ALWAYS_ON; dma_always_num++)
	{
		instrumenter->RegisterPort(*dmamux_3->dma_always_on[dma_always_num]);
	}
	for(dma_trigger_num = 0; dma_trigger_num < DMAMUX_3::NUM_DMA_TRIGGERS; dma_trigger_num++)
	{
		instrumenter->RegisterPort(*dmamux_3->dma_trigger[dma_trigger_num]);
	}
	for(dma_channel_num = 0; dma_channel_num < DMAMUX_3::NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(*dmamux_3->dma_channel[dma_channel_num]);
		instrumenter->RegisterPort(*dmamux_3->dma_channel_ack[dma_channel_num]);
	}

	// - DMAMUX_4
	instrumenter->RegisterPort(dmamux_4->m_clk);
	instrumenter->RegisterPort(dmamux_4->reset_b);
	for(dma_source_num = 0; dma_source_num < DMAMUX_4::NUM_DMA_SOURCES; dma_source_num++)
	{
		instrumenter->RegisterPort(*dmamux_4->dma_source[dma_source_num]);
		instrumenter->RegisterPort(*dmamux_4->dma_source_ack[dma_source_num]);
	}
	for(dma_always_num = 0; dma_always_num < DMAMUX_4::NUM_DMA_ALWAYS_ON; dma_always_num++)
	{
		instrumenter->RegisterPort(*dmamux_4->dma_always_on[dma_always_num]);
	}
	for(dma_trigger_num = 0; dma_trigger_num < DMAMUX_4::NUM_DMA_TRIGGERS; dma_trigger_num++)
	{
		instrumenter->RegisterPort(*dmamux_4->dma_trigger[dma_trigger_num]);
	}
	for(dma_channel_num = 0; dma_channel_num < DMAMUX_4::NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(*dmamux_4->dma_channel[dma_channel_num]);
		instrumenter->RegisterPort(*dmamux_4->dma_channel_ack[dma_channel_num]);
	}

	// - DMAMUX_5
	instrumenter->RegisterPort(dmamux_5->m_clk);
	instrumenter->RegisterPort(dmamux_5->reset_b);
	for(dma_source_num = 0; dma_source_num < DMAMUX_5::NUM_DMA_SOURCES; dma_source_num++)
	{
		instrumenter->RegisterPort(*dmamux_5->dma_source[dma_source_num]);
		instrumenter->RegisterPort(*dmamux_5->dma_source_ack[dma_source_num]);
	}
	for(dma_always_num = 0; dma_always_num < DMAMUX_5::NUM_DMA_ALWAYS_ON; dma_always_num++)
	{
		instrumenter->RegisterPort(*dmamux_5->dma_always_on[dma_always_num]);
	}
	for(dma_trigger_num = 0; dma_trigger_num < DMAMUX_5::NUM_DMA_TRIGGERS; dma_trigger_num++)
	{
		instrumenter->RegisterPort(*dmamux_5->dma_trigger[dma_trigger_num]);
	}
	for(dma_channel_num = 0; dma_channel_num < DMAMUX_5::NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(*dmamux_5->dma_channel[dma_channel_num]);
		instrumenter->RegisterPort(*dmamux_5->dma_channel_ack[dma_channel_num]);
	}
	
	// - DMAMUX_6
	instrumenter->RegisterPort(dmamux_6->m_clk);
	instrumenter->RegisterPort(dmamux_6->reset_b);
	for(dma_source_num = 0; dma_source_num < DMAMUX_6::NUM_DMA_SOURCES; dma_source_num++)
	{
		instrumenter->RegisterPort(*dmamux_6->dma_source[dma_source_num]);
		instrumenter->RegisterPort(*dmamux_6->dma_source_ack[dma_source_num]);
	}
	for(dma_always_num = 0; dma_always_num < DMAMUX_6::NUM_DMA_ALWAYS_ON; dma_always_num++)
	{
		instrumenter->RegisterPort(*dmamux_6->dma_always_on[dma_always_num]);
	}
	for(dma_trigger_num = 0; dma_trigger_num < DMAMUX_6::NUM_DMA_TRIGGERS; dma_trigger_num++)
	{
		instrumenter->RegisterPort(*dmamux_6->dma_trigger[dma_trigger_num]);
	}
	for(dma_channel_num = 0; dma_channel_num < DMAMUX_6::NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(*dmamux_6->dma_channel[dma_channel_num]);
		instrumenter->RegisterPort(*dmamux_6->dma_channel_ack[dma_channel_num]);
	}

	// - DMAMUX_7
	instrumenter->RegisterPort(dmamux_7->m_clk);
	instrumenter->RegisterPort(dmamux_7->reset_b);
	for(dma_source_num = 0; dma_source_num < DMAMUX_7::NUM_DMA_SOURCES; dma_source_num++)
	{
		instrumenter->RegisterPort(*dmamux_7->dma_source[dma_source_num]);
		instrumenter->RegisterPort(*dmamux_7->dma_source_ack[dma_source_num]);
	}
	for(dma_always_num = 0; dma_always_num < DMAMUX_7::NUM_DMA_ALWAYS_ON; dma_always_num++)
	{
		instrumenter->RegisterPort(*dmamux_7->dma_always_on[dma_always_num]);
	}
	for(dma_trigger_num = 0; dma_trigger_num < DMAMUX_7::NUM_DMA_TRIGGERS; dma_trigger_num++)
	{
		instrumenter->RegisterPort(*dmamux_7->dma_trigger[dma_trigger_num]);
	}
	for(dma_channel_num = 0; dma_channel_num < DMAMUX_7::NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(*dmamux_7->dma_channel[dma_channel_num]);
		instrumenter->RegisterPort(*dmamux_7->dma_channel_ack[dma_channel_num]);
	}

	// - DMAMUX_8
	instrumenter->RegisterPort(dmamux_8->m_clk);
	instrumenter->RegisterPort(dmamux_8->reset_b);
	for(dma_source_num = 0; dma_source_num < DMAMUX_8::NUM_DMA_SOURCES; dma_source_num++)
	{
		instrumenter->RegisterPort(*dmamux_8->dma_source[dma_source_num]);
		instrumenter->RegisterPort(*dmamux_8->dma_source_ack[dma_source_num]);
	}
	for(dma_always_num = 0; dma_always_num < DMAMUX_8::NUM_DMA_ALWAYS_ON; dma_always_num++)
	{
		instrumenter->RegisterPort(*dmamux_8->dma_always_on[dma_always_num]);
	}
	for(dma_trigger_num = 0; dma_trigger_num < DMAMUX_8::NUM_DMA_TRIGGERS; dma_trigger_num++)
	{
		instrumenter->RegisterPort(*dmamux_8->dma_trigger[dma_trigger_num]);
	}
	for(dma_channel_num = 0; dma_channel_num < DMAMUX_8::NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(*dmamux_8->dma_channel[dma_channel_num]);
		instrumenter->RegisterPort(*dmamux_8->dma_channel_ack[dma_channel_num]);
	}

	// - DMAMUX_9
	instrumenter->RegisterPort(dmamux_9->m_clk);
	instrumenter->RegisterPort(dmamux_9->reset_b);
	for(dma_source_num = 0; dma_source_num < DMAMUX_9::NUM_DMA_SOURCES; dma_source_num++)
	{
		instrumenter->RegisterPort(*dmamux_9->dma_source[dma_source_num]);
		instrumenter->RegisterPort(*dmamux_9->dma_source_ack[dma_source_num]);
	}
	for(dma_always_num = 0; dma_always_num < DMAMUX_9::NUM_DMA_ALWAYS_ON; dma_always_num++)
	{
		instrumenter->RegisterPort(*dmamux_9->dma_always_on[dma_always_num]);
	}
	for(dma_trigger_num = 0; dma_trigger_num < DMAMUX_9::NUM_DMA_TRIGGERS; dma_trigger_num++)
	{
		instrumenter->RegisterPort(*dmamux_9->dma_trigger[dma_trigger_num]);
	}
	for(dma_channel_num = 0; dma_channel_num < DMAMUX_9::NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(*dmamux_9->dma_channel[dma_channel_num]);
		instrumenter->RegisterPort(*dmamux_9->dma_channel_ack[dma_channel_num]);
	}
	
	// - EDMA_0
	instrumenter->RegisterPort(edma_0->m_clk);
	instrumenter->RegisterPort(edma_0->dma_clk);
	instrumenter->RegisterPort(edma_0->reset_b);
	for(dma_channel_num = 0; dma_channel_num < EDMA_0::NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(*edma_0->dma_channel[dma_channel_num]);
		instrumenter->RegisterPort(*edma_0->dma_channel_ack[dma_channel_num]);
	}
	for(dma_channel_num = 0; dma_channel_num < EDMA_0::NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(*edma_0->irq[dma_channel_num]);
	}
	for(dma_channel_num = 0; dma_channel_num < EDMA_0::NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(*edma_0->err_irq[dma_channel_num]);
	}

	// - EDMA_1
	instrumenter->RegisterPort(edma_1->m_clk);
	instrumenter->RegisterPort(edma_1->dma_clk);
	instrumenter->RegisterPort(edma_1->reset_b);
	for(dma_channel_num = 0; dma_channel_num < EDMA_1::NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(*edma_1->dma_channel[dma_channel_num]);
		instrumenter->RegisterPort(*edma_1->dma_channel_ack[dma_channel_num]);
	}
	for(dma_channel_num = 0; dma_channel_num < EDMA_1::NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(*edma_1->irq[dma_channel_num]);
	}
	for(dma_channel_num = 0; dma_channel_num < EDMA_1::NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(*edma_1->err_irq[dma_channel_num]);
	}
	
	// - DSPI_0
	instrumenter->RegisterPort(dspi_0->m_clk);
	instrumenter->RegisterPort(dspi_0->dspi_clk);
	instrumenter->RegisterPort(dspi_0->reset_b);
	instrumenter->RegisterPort(dspi_0->debug);
	instrumenter->RegisterPort(dspi_0->HT);
	instrumenter->RegisterPort(dspi_0->DMA_ACK_RX);
	instrumenter->RegisterPort(dspi_0->DMA_ACK_TX);
	instrumenter->RegisterPort(dspi_0->DMA_ACK_CMD);
	instrumenter->RegisterPort(dspi_0->DMA_ACK_DD);
	instrumenter->RegisterPort(dspi_0->INT_EOQF);
	instrumenter->RegisterPort(dspi_0->INT_TFFF);
	instrumenter->RegisterPort(dspi_0->INT_CMDFFF);
	instrumenter->RegisterPort(dspi_0->INT_TFIWF);
	instrumenter->RegisterPort(dspi_0->INT_TCF);
	instrumenter->RegisterPort(dspi_0->INT_CMDTCF);
	instrumenter->RegisterPort(dspi_0->INT_SPITCF);
	instrumenter->RegisterPort(dspi_0->INT_DSITCF);
	instrumenter->RegisterPort(dspi_0->INT_TFUF);
	instrumenter->RegisterPort(dspi_0->INT_RFDF);
	instrumenter->RegisterPort(dspi_0->INT_RFOF);
	instrumenter->RegisterPort(dspi_0->INT_SPEF);
	instrumenter->RegisterPort(dspi_0->INT_DPEF);
	instrumenter->RegisterPort(dspi_0->INT_DDIF);
	instrumenter->RegisterPort(dspi_0->DMA_RX);
	instrumenter->RegisterPort(dspi_0->DMA_TX);
	instrumenter->RegisterPort(dspi_0->DMA_CMD);
	instrumenter->RegisterPort(dspi_0->DMA_DD);
	
	for(i = 0; i < DSPI_0::NUM_DSI_INPUTS; i++)
	{
		instrumenter->RegisterPort(dspi_0->DSI_INPUT[i]);
	}
	for(i = 0; i < DSPI_0::NUM_DSI_OUTPUTS; i++)
	{
		instrumenter->RegisterPort(dspi_0->DSI_OUTPUT[i]);
	}

	// - DSPI_1
	instrumenter->RegisterPort(dspi_1->m_clk);
	instrumenter->RegisterPort(dspi_1->dspi_clk);
	instrumenter->RegisterPort(dspi_1->reset_b);
	instrumenter->RegisterPort(dspi_1->debug);
	instrumenter->RegisterPort(dspi_1->HT);
	instrumenter->RegisterPort(dspi_1->DMA_ACK_RX);
	instrumenter->RegisterPort(dspi_1->DMA_ACK_TX);
	instrumenter->RegisterPort(dspi_1->DMA_ACK_CMD);
	instrumenter->RegisterPort(dspi_1->DMA_ACK_DD);
	instrumenter->RegisterPort(dspi_1->INT_EOQF);
	instrumenter->RegisterPort(dspi_1->INT_TFFF);
	instrumenter->RegisterPort(dspi_1->INT_CMDFFF);
	instrumenter->RegisterPort(dspi_1->INT_TFIWF);
	instrumenter->RegisterPort(dspi_1->INT_TCF);
	instrumenter->RegisterPort(dspi_1->INT_CMDTCF);
	instrumenter->RegisterPort(dspi_1->INT_SPITCF);
	instrumenter->RegisterPort(dspi_1->INT_DSITCF);
	instrumenter->RegisterPort(dspi_1->INT_TFUF);
	instrumenter->RegisterPort(dspi_1->INT_RFDF);
	instrumenter->RegisterPort(dspi_1->INT_RFOF);
	instrumenter->RegisterPort(dspi_1->INT_SPEF);
	instrumenter->RegisterPort(dspi_1->INT_DPEF);
	instrumenter->RegisterPort(dspi_1->INT_DDIF);
	instrumenter->RegisterPort(dspi_1->DMA_RX);
	instrumenter->RegisterPort(dspi_1->DMA_TX);
	instrumenter->RegisterPort(dspi_1->DMA_CMD);
	instrumenter->RegisterPort(dspi_1->DMA_DD);
	
	for(i = 0; i < DSPI_1::NUM_DSI_INPUTS; i++)
	{
		instrumenter->RegisterPort(dspi_1->DSI_INPUT[i]);
	}
	for(i = 0; i < DSPI_1::NUM_DSI_OUTPUTS; i++)
	{
		instrumenter->RegisterPort(dspi_1->DSI_OUTPUT[i]);
	}
	
	// - DSPI_2
	instrumenter->RegisterPort(dspi_2->m_clk);
	instrumenter->RegisterPort(dspi_2->dspi_clk);
	instrumenter->RegisterPort(dspi_2->reset_b);
	instrumenter->RegisterPort(dspi_2->debug);
	instrumenter->RegisterPort(dspi_2->HT);
	instrumenter->RegisterPort(dspi_2->DMA_ACK_RX);
	instrumenter->RegisterPort(dspi_2->DMA_ACK_TX);
	instrumenter->RegisterPort(dspi_2->DMA_ACK_CMD);
	instrumenter->RegisterPort(dspi_2->DMA_ACK_DD);
	instrumenter->RegisterPort(dspi_2->INT_EOQF);
	instrumenter->RegisterPort(dspi_2->INT_TFFF);
	instrumenter->RegisterPort(dspi_2->INT_CMDFFF);
	instrumenter->RegisterPort(dspi_2->INT_TFIWF);
	instrumenter->RegisterPort(dspi_2->INT_TCF);
	instrumenter->RegisterPort(dspi_2->INT_CMDTCF);
	instrumenter->RegisterPort(dspi_2->INT_SPITCF);
	instrumenter->RegisterPort(dspi_2->INT_DSITCF);
	instrumenter->RegisterPort(dspi_2->INT_TFUF);
	instrumenter->RegisterPort(dspi_2->INT_RFDF);
	instrumenter->RegisterPort(dspi_2->INT_RFOF);
	instrumenter->RegisterPort(dspi_2->INT_SPEF);
	instrumenter->RegisterPort(dspi_2->INT_DPEF);
	instrumenter->RegisterPort(dspi_2->INT_DDIF);
	instrumenter->RegisterPort(dspi_2->DMA_RX);
	instrumenter->RegisterPort(dspi_2->DMA_TX);
	instrumenter->RegisterPort(dspi_2->DMA_CMD);
	instrumenter->RegisterPort(dspi_2->DMA_DD);
	
	for(i = 0; i < DSPI_2::NUM_DSI_INPUTS; i++)
	{
		instrumenter->RegisterPort(dspi_2->DSI_INPUT[i]);
	}
	for(i = 0; i < DSPI_2::NUM_DSI_OUTPUTS; i++)
	{
		instrumenter->RegisterPort(dspi_2->DSI_OUTPUT[i]);
	}

	// - DSPI_3
	instrumenter->RegisterPort(dspi_3->m_clk);
	instrumenter->RegisterPort(dspi_3->dspi_clk);
	instrumenter->RegisterPort(dspi_3->reset_b);
	instrumenter->RegisterPort(dspi_3->debug);
	instrumenter->RegisterPort(dspi_3->HT);
	instrumenter->RegisterPort(dspi_3->DMA_ACK_RX);
	instrumenter->RegisterPort(dspi_3->DMA_ACK_TX);
	instrumenter->RegisterPort(dspi_3->DMA_ACK_CMD);
	instrumenter->RegisterPort(dspi_3->DMA_ACK_DD);
	instrumenter->RegisterPort(dspi_3->INT_EOQF);
	instrumenter->RegisterPort(dspi_3->INT_TFFF);
	instrumenter->RegisterPort(dspi_3->INT_CMDFFF);
	instrumenter->RegisterPort(dspi_3->INT_TFIWF);
	instrumenter->RegisterPort(dspi_3->INT_TCF);
	instrumenter->RegisterPort(dspi_3->INT_CMDTCF);
	instrumenter->RegisterPort(dspi_3->INT_SPITCF);
	instrumenter->RegisterPort(dspi_3->INT_DSITCF);
	instrumenter->RegisterPort(dspi_3->INT_TFUF);
	instrumenter->RegisterPort(dspi_3->INT_RFDF);
	instrumenter->RegisterPort(dspi_3->INT_RFOF);
	instrumenter->RegisterPort(dspi_3->INT_SPEF);
	instrumenter->RegisterPort(dspi_3->INT_DPEF);
	instrumenter->RegisterPort(dspi_3->INT_DDIF);
	instrumenter->RegisterPort(dspi_3->DMA_RX);
	instrumenter->RegisterPort(dspi_3->DMA_TX);
	instrumenter->RegisterPort(dspi_3->DMA_CMD);
	instrumenter->RegisterPort(dspi_3->DMA_DD);
	
	for(i = 0; i < DSPI_3::NUM_DSI_INPUTS; i++)
	{
		instrumenter->RegisterPort(dspi_3->DSI_INPUT[i]);
	}
	for(i = 0; i < DSPI_3::NUM_DSI_OUTPUTS; i++)
	{
		instrumenter->RegisterPort(dspi_3->DSI_OUTPUT[i]);
	}

	// - DSPI_4
	instrumenter->RegisterPort(dspi_4->m_clk);
	instrumenter->RegisterPort(dspi_4->dspi_clk);
	instrumenter->RegisterPort(dspi_4->reset_b);
	instrumenter->RegisterPort(dspi_4->debug);
	instrumenter->RegisterPort(dspi_4->HT);
	instrumenter->RegisterPort(dspi_4->DMA_ACK_RX);
	instrumenter->RegisterPort(dspi_4->DMA_ACK_TX);
	instrumenter->RegisterPort(dspi_4->DMA_ACK_CMD);
	instrumenter->RegisterPort(dspi_4->DMA_ACK_DD);
	instrumenter->RegisterPort(dspi_4->INT_EOQF);
	instrumenter->RegisterPort(dspi_4->INT_TFFF);
	instrumenter->RegisterPort(dspi_4->INT_CMDFFF);
	instrumenter->RegisterPort(dspi_4->INT_TFIWF);
	instrumenter->RegisterPort(dspi_4->INT_TCF);
	instrumenter->RegisterPort(dspi_4->INT_CMDTCF);
	instrumenter->RegisterPort(dspi_4->INT_SPITCF);
	instrumenter->RegisterPort(dspi_4->INT_DSITCF);
	instrumenter->RegisterPort(dspi_4->INT_TFUF);
	instrumenter->RegisterPort(dspi_4->INT_RFDF);
	instrumenter->RegisterPort(dspi_4->INT_RFOF);
	instrumenter->RegisterPort(dspi_4->INT_SPEF);
	instrumenter->RegisterPort(dspi_4->INT_DPEF);
	instrumenter->RegisterPort(dspi_4->INT_DDIF);
	instrumenter->RegisterPort(dspi_4->DMA_RX);
	instrumenter->RegisterPort(dspi_4->DMA_TX);
	instrumenter->RegisterPort(dspi_4->DMA_CMD);
	instrumenter->RegisterPort(dspi_4->DMA_DD);
	
	for(i = 0; i < DSPI_4::NUM_DSI_INPUTS; i++)
	{
		instrumenter->RegisterPort(dspi_4->DSI_INPUT[i]);
	}
	for(i = 0; i < DSPI_4::NUM_DSI_OUTPUTS; i++)
	{
		instrumenter->RegisterPort(dspi_4->DSI_OUTPUT[i]);
	}
	
	// - DSPI_5
	instrumenter->RegisterPort(dspi_5->m_clk);
	instrumenter->RegisterPort(dspi_5->dspi_clk);
	instrumenter->RegisterPort(dspi_5->reset_b);
	instrumenter->RegisterPort(dspi_5->debug);
	instrumenter->RegisterPort(dspi_5->HT);
	instrumenter->RegisterPort(dspi_5->DMA_ACK_RX);
	instrumenter->RegisterPort(dspi_5->DMA_ACK_TX);
	instrumenter->RegisterPort(dspi_5->DMA_ACK_CMD);
	instrumenter->RegisterPort(dspi_5->DMA_ACK_DD);
	instrumenter->RegisterPort(dspi_5->INT_EOQF);
	instrumenter->RegisterPort(dspi_5->INT_TFFF);
	instrumenter->RegisterPort(dspi_5->INT_CMDFFF);
	instrumenter->RegisterPort(dspi_5->INT_TFIWF);
	instrumenter->RegisterPort(dspi_5->INT_TCF);
	instrumenter->RegisterPort(dspi_5->INT_CMDTCF);
	instrumenter->RegisterPort(dspi_5->INT_SPITCF);
	instrumenter->RegisterPort(dspi_5->INT_DSITCF);
	instrumenter->RegisterPort(dspi_5->INT_TFUF);
	instrumenter->RegisterPort(dspi_5->INT_RFDF);
	instrumenter->RegisterPort(dspi_5->INT_RFOF);
	instrumenter->RegisterPort(dspi_5->INT_SPEF);
	instrumenter->RegisterPort(dspi_5->INT_DPEF);
	instrumenter->RegisterPort(dspi_5->INT_DDIF);
	instrumenter->RegisterPort(dspi_5->DMA_RX);
	instrumenter->RegisterPort(dspi_5->DMA_TX);
	instrumenter->RegisterPort(dspi_5->DMA_CMD);
	instrumenter->RegisterPort(dspi_5->DMA_DD);
	
	for(i = 0; i < DSPI_5::NUM_DSI_INPUTS; i++)
	{
		instrumenter->RegisterPort(dspi_5->DSI_INPUT[i]);
	}
	for(i = 0; i < DSPI_5::NUM_DSI_OUTPUTS; i++)
	{
		instrumenter->RegisterPort(dspi_5->DSI_OUTPUT[i]);
	}

	// - DSPI_6
	instrumenter->RegisterPort(dspi_6->m_clk);
	instrumenter->RegisterPort(dspi_6->dspi_clk);
	instrumenter->RegisterPort(dspi_6->reset_b);
	instrumenter->RegisterPort(dspi_6->debug);
	instrumenter->RegisterPort(dspi_6->HT);
	instrumenter->RegisterPort(dspi_6->DMA_ACK_RX);
	instrumenter->RegisterPort(dspi_6->DMA_ACK_TX);
	instrumenter->RegisterPort(dspi_6->DMA_ACK_CMD);
	instrumenter->RegisterPort(dspi_6->DMA_ACK_DD);
	instrumenter->RegisterPort(dspi_6->INT_EOQF);
	instrumenter->RegisterPort(dspi_6->INT_TFFF);
	instrumenter->RegisterPort(dspi_6->INT_CMDFFF);
	instrumenter->RegisterPort(dspi_6->INT_TFIWF);
	instrumenter->RegisterPort(dspi_6->INT_TCF);
	instrumenter->RegisterPort(dspi_6->INT_CMDTCF);
	instrumenter->RegisterPort(dspi_6->INT_SPITCF);
	instrumenter->RegisterPort(dspi_6->INT_DSITCF);
	instrumenter->RegisterPort(dspi_6->INT_TFUF);
	instrumenter->RegisterPort(dspi_6->INT_RFDF);
	instrumenter->RegisterPort(dspi_6->INT_RFOF);
	instrumenter->RegisterPort(dspi_6->INT_SPEF);
	instrumenter->RegisterPort(dspi_6->INT_DPEF);
	instrumenter->RegisterPort(dspi_6->INT_DDIF);
	instrumenter->RegisterPort(dspi_6->DMA_RX);
	instrumenter->RegisterPort(dspi_6->DMA_TX);
	instrumenter->RegisterPort(dspi_6->DMA_CMD);
	instrumenter->RegisterPort(dspi_6->DMA_DD);
	
	for(i = 0; i < DSPI_6::NUM_DSI_INPUTS; i++)
	{
		instrumenter->RegisterPort(dspi_6->DSI_INPUT[i]);
	}
	for(i = 0; i < DSPI_6::NUM_DSI_OUTPUTS; i++)
	{
		instrumenter->RegisterPort(dspi_6->DSI_OUTPUT[i]);
	}

	// - DSPI_12
	instrumenter->RegisterPort(dspi_12->m_clk);
	instrumenter->RegisterPort(dspi_12->dspi_clk);
	instrumenter->RegisterPort(dspi_12->reset_b);
	instrumenter->RegisterPort(dspi_12->debug);
	instrumenter->RegisterPort(dspi_12->HT);
	instrumenter->RegisterPort(dspi_12->DMA_ACK_RX);
	instrumenter->RegisterPort(dspi_12->DMA_ACK_TX);
	instrumenter->RegisterPort(dspi_12->DMA_ACK_CMD);
	instrumenter->RegisterPort(dspi_12->DMA_ACK_DD);
	instrumenter->RegisterPort(dspi_12->INT_EOQF);
	instrumenter->RegisterPort(dspi_12->INT_TFFF);
	instrumenter->RegisterPort(dspi_12->INT_CMDFFF);
	instrumenter->RegisterPort(dspi_12->INT_TFIWF);
	instrumenter->RegisterPort(dspi_12->INT_TCF);
	instrumenter->RegisterPort(dspi_12->INT_CMDTCF);
	instrumenter->RegisterPort(dspi_12->INT_SPITCF);
	instrumenter->RegisterPort(dspi_12->INT_DSITCF);
	instrumenter->RegisterPort(dspi_12->INT_TFUF);
	instrumenter->RegisterPort(dspi_12->INT_RFDF);
	instrumenter->RegisterPort(dspi_12->INT_RFOF);
	instrumenter->RegisterPort(dspi_12->INT_SPEF);
	instrumenter->RegisterPort(dspi_12->INT_DPEF);
	instrumenter->RegisterPort(dspi_12->INT_DDIF);
	instrumenter->RegisterPort(dspi_12->DMA_RX);
	instrumenter->RegisterPort(dspi_12->DMA_TX);
	instrumenter->RegisterPort(dspi_12->DMA_CMD);
	instrumenter->RegisterPort(dspi_12->DMA_DD);
	
	for(i = 0; i < DSPI_12::NUM_DSI_INPUTS; i++)
	{
		instrumenter->RegisterPort(dspi_12->DSI_INPUT[i]);
	}
	for(i = 0; i < DSPI_12::NUM_DSI_OUTPUTS; i++)
	{
		instrumenter->RegisterPort(dspi_12->DSI_OUTPUT[i]);
	}

	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		instrumenter->RegisterPort(dma_err_irq_combinator->in[dma_channel_num]);
	}
	instrumenter->RegisterPort(dma_err_irq_combinator->out);
	
	for(i = 0; i < 3; i++)
	{
		instrumenter->RegisterPort(DSPI0_0->in[i]);
		instrumenter->RegisterPort(DSPI1_0->in[i]);
		instrumenter->RegisterPort(DSPI2_0->in[i]);
		instrumenter->RegisterPort(DSPI3_0->in[i]);
		instrumenter->RegisterPort(DSPI4_0->in[i]);
		instrumenter->RegisterPort(DSPI5_0->in[i]);
		instrumenter->RegisterPort(DSPI6_0->in[i]);
		instrumenter->RegisterPort(DSPI12_0->in[i]);
	}
	
	for(i = 0; i < 2; i++)
	{
		instrumenter->RegisterPort(DSPI4_5->in[i]);
		instrumenter->RegisterPort(DSPI4_6->in[i]);
		instrumenter->RegisterPort(DSPI4_7->in[i]);
		instrumenter->RegisterPort(DSPI5_5->in[i]);
		instrumenter->RegisterPort(DSPI5_6->in[i]);
		instrumenter->RegisterPort(DSPI5_7->in[i]);
		instrumenter->RegisterPort(DSPI6_5->in[i]);
		instrumenter->RegisterPort(DSPI6_6->in[i]);
		instrumenter->RegisterPort(DSPI6_7->in[i]);
	}
	
	instrumenter->RegisterPort(DSPI0_0->out);
	instrumenter->RegisterPort(DSPI1_0->out);
	instrumenter->RegisterPort(DSPI2_0->out);
	instrumenter->RegisterPort(DSPI3_0->out);
	instrumenter->RegisterPort(DSPI4_0->out);
	instrumenter->RegisterPort(DSPI4_5->out);
	instrumenter->RegisterPort(DSPI4_6->out);
	instrumenter->RegisterPort(DSPI4_7->out);
	instrumenter->RegisterPort(DSPI5_0->out);
	instrumenter->RegisterPort(DSPI5_5->out);
	instrumenter->RegisterPort(DSPI5_6->out);
	instrumenter->RegisterPort(DSPI5_7->out);
	instrumenter->RegisterPort(DSPI6_0->out);
	instrumenter->RegisterPort(DSPI6_5->out);
	instrumenter->RegisterPort(DSPI6_6->out);
	instrumenter->RegisterPort(DSPI6_7->out);
	instrumenter->RegisterPort(DSPI12_0->out);
	
	// - SIUL2
	instrumenter->RegisterPort(siul2->m_clk);
	instrumenter->RegisterPort(siul2->reset_b);
	for(i = 0; i < SIUL2::NUM_PADS; i++)
	{
		instrumenter->RegisterPort(siul2->pad_in[i]);
		instrumenter->RegisterPort(siul2->pad_out[i]);
	}
	
	// - CAN subsystem
	instrumenter->RegisterPort(m_can_1->m_clk);
	instrumenter->RegisterPort(m_can_1->can_clk);
	instrumenter->RegisterPort(m_can_1->reset_b);
	instrumenter->RegisterPort(m_can_1->DMA_ACK);
	instrumenter->RegisterPort(m_can_1->INT0);
	instrumenter->RegisterPort(m_can_1->INT1);
	instrumenter->RegisterPort(m_can_1->DMA_REQ);
	for(i = 0; i < M_CAN_1::NUM_FILTER_EVENTS; i++)
	{
		instrumenter->RegisterPort(m_can_1->FE[i]);
	}
	
	instrumenter->RegisterPort(m_can_2->m_clk);
	instrumenter->RegisterPort(m_can_2->can_clk);
	instrumenter->RegisterPort(m_can_2->reset_b);
	instrumenter->RegisterPort(m_can_2->DMA_ACK);
	instrumenter->RegisterPort(m_can_2->INT0);
	instrumenter->RegisterPort(m_can_2->INT1);
	instrumenter->RegisterPort(m_can_2->DMA_REQ);
	for(i = 0; i < M_CAN_2::NUM_FILTER_EVENTS; i++)
	{
		instrumenter->RegisterPort(m_can_2->FE[i]);
	}

	instrumenter->RegisterPort(m_can_3->m_clk);
	instrumenter->RegisterPort(m_can_3->can_clk);
	instrumenter->RegisterPort(m_can_3->reset_b);
	instrumenter->RegisterPort(m_can_3->DMA_ACK);
	instrumenter->RegisterPort(m_can_3->INT0);
	instrumenter->RegisterPort(m_can_3->INT1);
	instrumenter->RegisterPort(m_can_3->DMA_REQ);
	for(i = 0; i < M_CAN_3::NUM_FILTER_EVENTS; i++)
	{
		instrumenter->RegisterPort(m_can_3->FE[i]);
	}

	instrumenter->RegisterPort(m_can_4->m_clk);
	instrumenter->RegisterPort(m_can_4->can_clk);
	instrumenter->RegisterPort(m_can_4->reset_b);
	instrumenter->RegisterPort(m_can_4->DMA_ACK);
	instrumenter->RegisterPort(m_can_4->INT0);
	instrumenter->RegisterPort(m_can_4->INT1);
	instrumenter->RegisterPort(m_can_4->DMA_REQ);
	for(i = 0; i < M_CAN_4::NUM_FILTER_EVENTS; i++)
	{
		instrumenter->RegisterPort(m_can_4->FE[i]);
	}
	
	instrumenter->RegisterPort(shared_can_message_ram_router->input_if_clock);
	instrumenter->RegisterPort(shared_can_message_ram_router->output_if_clock);
	
	// - Semaphores2
	instrumenter->RegisterPort(sema4->m_clk);
	instrumenter->RegisterPort(sema4->reset_b);

	//=========================================================================
	//===                         Channels creation                         ===
	//=========================================================================
	
	instrumenter->CreateClock("EBI_CLK");
	instrumenter->CreateClock("IOP_CLK");
	instrumenter->CreateClock("COMP_CLK");
	instrumenter->CreateClock("FXBAR_CLK");
	instrumenter->CreateClock("SXBAR_CLK");
	instrumenter->CreateClock("PBRIDGEA_CLK");
	instrumenter->CreateClock("PBRIDGEB_CLK");
	instrumenter->CreateClock("PER_CLK");
	instrumenter->CreateClock("RTI_CLK");
	instrumenter->CreateClock("LIN_CLK");
	instrumenter->CreateClock("SERIAL_TERMINAL0_CLK");
	instrumenter->CreateClock("SERIAL_TERMINAL1_CLK");
	instrumenter->CreateClock("SERIAL_TERMINAL2_CLK");
	instrumenter->CreateClock("SERIAL_TERMINAL14_CLK");
	instrumenter->CreateClock("SERIAL_TERMINAL15_CLK");
	instrumenter->CreateClock("SERIAL_TERMINAL16_CLK");
	instrumenter->CreateClock("DSPI_CLK0");
	instrumenter->CreateClock("DSPI_CLK1");
	instrumenter->CreateClock("CAN_CLK");
	
	instrumenter->CreateSignal("m_por", false);
	instrumenter->CreateSignal("stop", false);
	instrumenter->CreateSignal("debug", false);
	instrumenter->CreateSignal("reset_b", true);
	instrumenter->CreateSignal<bool, sc_core::SC_MANY_WRITERS>("p_reset_b_0", true);
	instrumenter->CreateSignal<bool, sc_core::SC_MANY_WRITERS>("p_reset_b_1", true);
	instrumenter->CreateSignal<bool, sc_core::SC_MANY_WRITERS>("p_reset_b_2", true);
	instrumenter->CreateSignal<bool, sc_core::SC_MANY_WRITERS>("p_reset_b_3", true);
	instrumenter->CreateSignal("p_nmi_b", true);
	instrumenter->CreateSignal("p_mcp_b", true);
	instrumenter->CreateSignal("p_rstbase", sc_dt::sc_uint<30>(0));
	instrumenter->CreateSignal("p_cpuid", sc_dt::sc_uint<8>(0));
	instrumenter->CreateSignal("p_crint_b", true);
	instrumenter->CreateSignalArray(INTC_0_CONFIG::NUM_PROCESSORS, "p_irq_b", true);
	instrumenter->CreateSignalArray(INTC_0_CONFIG::NUM_PROCESSORS, "p_avec_b", true);
	instrumenter->CreateSignalArray(INTC_0_CONFIG::NUM_PROCESSORS, "p_voffset", sc_dt::sc_uint<INTC_0_CONFIG::VOFFSET_WIDTH>(0));
	instrumenter->CreateSignalArray(INTC_0_CONFIG::NUM_PROCESSORS, "p_iack", false);
	
	instrumenter->CreateSignalArray(PIT_0::NUM_CHANNELS, "PIT_0_DMA_TRIGGER", false);
	
	instrumenter->CreateSignalArray(INTC_0::NUM_IRQS, "irq", false);
	instrumenter->CreateSignalArray(STM_2_CONFIG::NUM_CHANNELS, "stm_2_cir", false);

	instrumenter->CreateSignalArray(LINFlexD_0::NUM_DMA_RX_CHANNELS, "LINFlexD_0_DMA_RX", false);
	instrumenter->CreateSignalArray(LINFlexD_0::NUM_DMA_TX_CHANNELS, "LINFlexD_0_DMA_TX", false);
	instrumenter->CreateSignalArray(LINFlexD_0::NUM_DMA_RX_CHANNELS, "LINFlexD_0_DMA_ACK_RX", false);
	instrumenter->CreateSignalArray(LINFlexD_0::NUM_DMA_TX_CHANNELS, "LINFlexD_0_DMA_ACK_TX", false);
	
	instrumenter->CreateSignalArray(LINFlexD_1::NUM_DMA_RX_CHANNELS, "LINFlexD_1_DMA_RX", false);
	instrumenter->CreateSignalArray(LINFlexD_1::NUM_DMA_TX_CHANNELS, "LINFlexD_1_DMA_TX", false);
	instrumenter->CreateSignalArray(LINFlexD_1::NUM_DMA_RX_CHANNELS, "LINFlexD_1_DMA_ACK_RX", false);
	instrumenter->CreateSignalArray(LINFlexD_1::NUM_DMA_TX_CHANNELS, "LINFlexD_1_DMA_ACK_TX", false);
	
	instrumenter->CreateSignalArray(LINFlexD_2::NUM_DMA_RX_CHANNELS, "LINFlexD_2_DMA_RX", false);
	instrumenter->CreateSignalArray(LINFlexD_2::NUM_DMA_TX_CHANNELS, "LINFlexD_2_DMA_TX", false);
	instrumenter->CreateSignalArray(LINFlexD_2::NUM_DMA_RX_CHANNELS, "LINFlexD_2_DMA_ACK_RX", false);
	instrumenter->CreateSignalArray(LINFlexD_2::NUM_DMA_TX_CHANNELS, "LINFlexD_2_DMA_ACK_TX", false);
	
	instrumenter->CreateSignalArray(LINFlexD_14::NUM_DMA_RX_CHANNELS, "LINFlexD_14_DMA_RX", false);
	instrumenter->CreateSignalArray(LINFlexD_14::NUM_DMA_TX_CHANNELS, "LINFlexD_14_DMA_TX", false);
	instrumenter->CreateSignalArray(LINFlexD_14::NUM_DMA_RX_CHANNELS, "LINFlexD_14_DMA_ACK_RX", false);
	instrumenter->CreateSignalArray(LINFlexD_14::NUM_DMA_TX_CHANNELS, "LINFlexD_14_DMA_ACK_TX", false);

	instrumenter->CreateSignalArray(LINFlexD_15::NUM_DMA_RX_CHANNELS, "LINFlexD_15_DMA_RX", false);
	instrumenter->CreateSignalArray(LINFlexD_15::NUM_DMA_TX_CHANNELS, "LINFlexD_15_DMA_TX", false);
	instrumenter->CreateSignalArray(LINFlexD_15::NUM_DMA_RX_CHANNELS, "LINFlexD_15_DMA_ACK_RX", false);
	instrumenter->CreateSignalArray(LINFlexD_15::NUM_DMA_TX_CHANNELS, "LINFlexD_15_DMA_ACK_TX", false);

	instrumenter->CreateSignalArray(LINFlexD_16::NUM_DMA_RX_CHANNELS, "LINFlexD_16_DMA_RX", false);
	instrumenter->CreateSignalArray(LINFlexD_16::NUM_DMA_TX_CHANNELS, "LINFlexD_16_DMA_TX", false);
	instrumenter->CreateSignalArray(LINFlexD_16::NUM_DMA_RX_CHANNELS, "LINFlexD_16_DMA_ACK_RX", false);
	instrumenter->CreateSignalArray(LINFlexD_16::NUM_DMA_TX_CHANNELS, "LINFlexD_16_DMA_ACK_TX", false);

	instrumenter->CreateSignal("DSPI_0_INT_DDIF", false);
	instrumenter->CreateSignal("DSPI_0_INT_DPEF", false);
	instrumenter->CreateSignal("DSPI_0_INT_SPITCF", false);
	instrumenter->CreateSignal("DSPI_0_INT_DSITCF", false);
	
	instrumenter->CreateSignal("DSPI_1_INT_DDIF", false);
	instrumenter->CreateSignal("DSPI_1_INT_DPEF", false);
	instrumenter->CreateSignal("DSPI_1_INT_SPITCF", false);
	instrumenter->CreateSignal("DSPI_1_INT_DSITCF", false);

	instrumenter->CreateSignal("DSPI_2_INT_DDIF", false);
	instrumenter->CreateSignal("DSPI_2_INT_DPEF", false);
	instrumenter->CreateSignal("DSPI_2_INT_SPITCF", false);
	instrumenter->CreateSignal("DSPI_2_INT_DSITCF", false);

	instrumenter->CreateSignal("DSPI_3_INT_DDIF", false);
	instrumenter->CreateSignal("DSPI_3_INT_DPEF", false);
	instrumenter->CreateSignal("DSPI_3_INT_SPITCF", false);
	instrumenter->CreateSignal("DSPI_3_INT_DSITCF", false);

	instrumenter->CreateSignal("DSPI_12_INT_DDIF", false);
	instrumenter->CreateSignal("DSPI_12_INT_DPEF", false);
	instrumenter->CreateSignal("DSPI_12_INT_SPITCF", false);
	instrumenter->CreateSignal("DSPI_12_INT_DSITCF", false);

	instrumenter->CreateSignalArray(NUM_DMA_CHANNELS, "DMA_CHANNEL", false);
	
	instrumenter->CreateSignalArray(NUM_DMA_CHANNELS, "DMA_CHANNEL_ACK", false);
	
	instrumenter->CreateSignalArray(NUM_DMA_ALWAYS_ON, "DMA_ALWAYS_ON", true);
	
	instrumenter->CreateSignalArray(NUM_DMA_CHANNELS, "DMA_ERR_IRQ", false);
	
	instrumenter->CreateSignal("DSPI_0_DMA_DD", false);
	
	instrumenter->CreateSignal("DSPI_0_DMA_ACK_DD", false);

	instrumenter->CreateSignal("DSPI_1_DMA_DD", false);
	instrumenter->CreateSignal("DSPI_1_DMA_ACK_DD", false);

	instrumenter->CreateSignal("DSPI_2_DMA_DD", false);
	instrumenter->CreateSignal("DSPI_2_DMA_ACK_DD", false);
	
	instrumenter->CreateSignal("DSPI_3_DMA_DD", false);
	instrumenter->CreateSignal("DSPI_3_DMA_ACK_DD", false);

	instrumenter->CreateSignal("DSPI_4_DMA_DD", false);
	instrumenter->CreateSignal("DSPI_4_DMA_ACK_DD", false);
	
	instrumenter->CreateSignal("DSPI_5_DMA_DD", false);
	instrumenter->CreateSignal("DSPI_5_DMA_ACK_DD", false);
	
	instrumenter->CreateSignal("DSPI_6_DMA_DD", false);
	instrumenter->CreateSignal("DSPI_6_DMA_ACK_DD", false);
	
	instrumenter->CreateSignal("DSPI_12_DMA_DD", false);
	instrumenter->CreateSignal("DSPI_12_DMA_ACK_DD", false);
	
	instrumenter->CreateSignal("DSPI_0_INT_TFUF", false);
	instrumenter->CreateSignal("DSPI_0_INT_RFOF", false);
	instrumenter->CreateSignal("DSPI_0_INT_TFIWF", false);
	instrumenter->CreateSignal("DSPI_1_INT_TFUF", false);
	instrumenter->CreateSignal("DSPI_1_INT_RFOF", false);
	instrumenter->CreateSignal("DSPI_1_INT_TFIWF", false);
	instrumenter->CreateSignal("DSPI_2_INT_TFUF", false);
	instrumenter->CreateSignal("DSPI_2_INT_RFOF", false);
	instrumenter->CreateSignal("DSPI_2_INT_TFIWF", false);
	instrumenter->CreateSignal("DSPI_3_INT_TFUF", false);
	instrumenter->CreateSignal("DSPI_3_INT_RFOF", false);
	instrumenter->CreateSignal("DSPI_3_INT_TFIWF", false);
	instrumenter->CreateSignal("DSPI_4_INT_TFUF", false);
	instrumenter->CreateSignal("DSPI_4_INT_RFOF", false);
	instrumenter->CreateSignal("DSPI_4_INT_TFIWF", false);
	instrumenter->CreateSignal("DSPI_4_INT_SPITCF", false);
	instrumenter->CreateSignal("DSPI_4_INT_CMDTCF", false);
	instrumenter->CreateSignal("DSPI_4_INT_DSITCF", false);
	instrumenter->CreateSignal("DSPI_4_INT_CMDFFF", false);
	instrumenter->CreateSignal("DSPI_4_INT_SPEF", false);
	instrumenter->CreateSignal("DSPI_4_INT_DPEF", false);
	instrumenter->CreateSignal("DSPI_5_INT_TFUF", false);
	instrumenter->CreateSignal("DSPI_5_INT_RFOF", false);
	instrumenter->CreateSignal("DSPI_5_INT_TFIWF", false);
	instrumenter->CreateSignal("DSPI_5_INT_SPITCF", false);
	instrumenter->CreateSignal("DSPI_5_INT_CMDTCF", false);
	instrumenter->CreateSignal("DSPI_5_INT_DSITCF", false);
	instrumenter->CreateSignal("DSPI_5_INT_CMDFFF", false);
	instrumenter->CreateSignal("DSPI_5_INT_SPEF", false);
	instrumenter->CreateSignal("DSPI_5_INT_DPEF", false);
	instrumenter->CreateSignal("DSPI_6_INT_TFUF", false);
	instrumenter->CreateSignal("DSPI_6_INT_RFOF", false);
	instrumenter->CreateSignal("DSPI_6_INT_TFIWF", false);
	instrumenter->CreateSignal("DSPI_6_INT_SPITCF", false);
	instrumenter->CreateSignal("DSPI_6_INT_CMDTCF", false);
	instrumenter->CreateSignal("DSPI_6_INT_DSITCF", false);
	instrumenter->CreateSignal("DSPI_6_INT_CMDFFF", false);
	instrumenter->CreateSignal("DSPI_6_INT_SPEF", false);
	instrumenter->CreateSignal("DSPI_6_INT_DPEF", false);
	instrumenter->CreateSignal("DSPI_12_INT_TFUF", false);
	instrumenter->CreateSignal("DSPI_12_INT_RFOF", false);
	instrumenter->CreateSignal("DSPI_12_INT_TFIWF", false);
	
	instrumenter->CreateSignalArray<bool, sc_core::SC_MANY_WRITERS>(SIUL2::NUM_PADS, "pad", false);
	
	instrumenter->CreateSignalArray(M_CAN_1::NUM_FILTER_EVENTS, "M_CAN_1_FE", false);

	instrumenter->CreateSignalArray(M_CAN_2::NUM_FILTER_EVENTS, "M_CAN_2_FE", false);
	
	instrumenter->CreateSignal("M_CAN_3_DMA_REQ", false);
	instrumenter->CreateSignal("M_CAN_3_DMA_ACK", false);
	instrumenter->CreateSignalArray(M_CAN_3::NUM_FILTER_EVENTS, "M_CAN_3_FE", false);

	instrumenter->CreateSignal("M_CAN_4_DMA_REQ", false);
	instrumenter->CreateSignal("M_CAN_4_DMA_ACK", false);
	instrumenter->CreateSignalArray(M_CAN_4::NUM_FILTER_EVENTS, "M_CAN_4_FE", false);
	
	//  - LIN Serial Buses
	linflexd_0_tx_serial_bus = new LINFlexD_0_TX_SERIAL_BUS("LINFlexD_0_TX_SERIAL_BUS", instrumenter->CreateSignal("LINFlexD_0_TX", true, OUTPUT_INSTRUMENTATION), this);
	linflexd_0_rx_serial_bus = new LINFlexD_0_RX_SERIAL_BUS("LINFlexD_0_RX_SERIAL_BUS", instrumenter->CreateSignal("LINFlexD_0_RX", true, OUTPUT_INSTRUMENTATION), this);
	linflexd_1_tx_serial_bus = new LINFlexD_1_TX_SERIAL_BUS("LINFlexD_1_TX_SERIAL_BUS", instrumenter->CreateSignal("LINFlexD_1_TX", true, OUTPUT_INSTRUMENTATION), this);
	linflexd_1_rx_serial_bus = new LINFlexD_1_RX_SERIAL_BUS("LINFlexD_1_RX_SERIAL_BUS", instrumenter->CreateSignal("LINFlexD_1_RX", true, OUTPUT_INSTRUMENTATION), this);
	linflexd_2_tx_serial_bus = new LINFlexD_2_TX_SERIAL_BUS("LINFlexD_2_TX_SERIAL_BUS", instrumenter->CreateSignal("LINFlexD_2_TX", true, OUTPUT_INSTRUMENTATION), this);
	linflexd_2_rx_serial_bus = new LINFlexD_2_RX_SERIAL_BUS("LINFlexD_2_RX_SERIAL_BUS", instrumenter->CreateSignal("LINFlexD_2_RX", true, OUTPUT_INSTRUMENTATION), this);
	linflexd_14_tx_serial_bus = new LINFlexD_14_TX_SERIAL_BUS("LINFlexD_14_TX_SERIAL_BUS", instrumenter->CreateSignal("LINFlexD_14_TX", true, OUTPUT_INSTRUMENTATION), this);
	linflexd_14_rx_serial_bus = new LINFlexD_14_RX_SERIAL_BUS("LINFlexD_14_RX_SERIAL_BUS", instrumenter->CreateSignal("LINFlexD_14_RX", true, OUTPUT_INSTRUMENTATION), this);
	linflexd_15_tx_serial_bus = new LINFlexD_15_TX_SERIAL_BUS("LINFlexD_15_TX_SERIAL_BUS", instrumenter->CreateSignal("LINFlexD_15_TX", true, OUTPUT_INSTRUMENTATION), this);
	linflexd_15_rx_serial_bus = new LINFlexD_15_RX_SERIAL_BUS("LINFlexD_15_RX_SERIAL_BUS", instrumenter->CreateSignal("LINFlexD_15_RX", true, OUTPUT_INSTRUMENTATION), this);
	linflexd_16_tx_serial_bus = new LINFlexD_16_TX_SERIAL_BUS("LINFlexD_16_TX_SERIAL_BUS", instrumenter->CreateSignal("LINFlexD_16_TX", true, OUTPUT_INSTRUMENTATION), this);
	linflexd_16_rx_serial_bus = new LINFlexD_16_RX_SERIAL_BUS("LINFlexD_16_RX_SERIAL_BUS", instrumenter->CreateSignal("LINFlexD_16_RX", true, OUTPUT_INSTRUMENTATION), this);

	//  - DSPI serial buses
	dspi_0_sout_serial_bus  = new DSPI_0_SOUT_SERIAL_BUS ("DSPI_0_SOUT_SERIAL_BUS" , instrumenter->CreateSignal("DSPI_0_SOUT" , true, OUTPUT_INSTRUMENTATION), this);
	dspi_0_sin_serial_bus   = new DSPI_0_SIN_SERIAL_BUS  ("DSPI_0_SIN_SERIAL_BUS"  , instrumenter->CreateSignal("DSPI_0_SIN"  , true, OUTPUT_INSTRUMENTATION), this);
	for(i = 0; i < DSPI_0::NUM_CTARS; i++)
	{
		std::stringstream dspi_0_pcs_serial_bus_name_sstr;
		dspi_0_pcs_serial_bus_name_sstr << "DSPI_0_PCS_SERIAL_BUS_" << i;
		std::stringstream dspi_0_pcs_name_sstr;
		dspi_0_pcs_name_sstr << "DSPI_0_PCS_" << i;
		dspi_0_pcs_serial_bus[i] = new DSPI_0_PCS_SERIAL_BUS(dspi_0_pcs_serial_bus_name_sstr.str().c_str(), instrumenter->CreateSignal(dspi_0_pcs_name_sstr.str(), true, OUTPUT_INSTRUMENTATION), this);
	}
	dspi_0_ss_serial_bus = new DSPI_0_SS_SERIAL_BUS("DSPI_0_SS_SERIAL_BUS", instrumenter->CreateSignal("DSPI_0_SS", true, OUTPUT_INSTRUMENTATION), this);
	
	dspi_1_sout_serial_bus  = new DSPI_1_SOUT_SERIAL_BUS ("DSPI_1_SOUT_SERIAL_BUS" , instrumenter->CreateSignal("DSPI_1_SOUT" , true, OUTPUT_INSTRUMENTATION), this);
	dspi_1_sin_serial_bus   = new DSPI_1_SIN_SERIAL_BUS  ("DSPI_1_SIN_SERIAL_BUS"  , instrumenter->CreateSignal("DSPI_1_SIN"  , true, OUTPUT_INSTRUMENTATION), this);
	for(i = 0; i < DSPI_1::NUM_CTARS; i++)
	{
		std::stringstream dspi_1_pcs_serial_bus_name_sstr;
		dspi_1_pcs_serial_bus_name_sstr << "DSPI_1_PCS_SERIAL_BUS_" << i;
		std::stringstream dspi_1_pcs_name_sstr;
		dspi_1_pcs_name_sstr << "DSPI_1_PCS_" << i;
		dspi_1_pcs_serial_bus[i] = new DSPI_1_PCS_SERIAL_BUS(dspi_1_pcs_serial_bus_name_sstr.str().c_str(), instrumenter->CreateSignal(dspi_1_pcs_name_sstr.str(), true, OUTPUT_INSTRUMENTATION), this);
	}
	dspi_1_ss_serial_bus = new DSPI_1_SS_SERIAL_BUS("DSPI_1_SS_SERIAL_BUS", instrumenter->CreateSignal("DSPI_1_SS", true, OUTPUT_INSTRUMENTATION), this);
	
	dspi_2_sout_serial_bus  = new DSPI_2_SOUT_SERIAL_BUS ("DSPI_2_SOUT_SERIAL_BUS" , instrumenter->CreateSignal("DSPI_2_SOUT" , true, OUTPUT_INSTRUMENTATION), this);
	dspi_2_sin_serial_bus   = new DSPI_2_SIN_SERIAL_BUS  ("DSPI_2_SIN_SERIAL_BUS"  , instrumenter->CreateSignal("DSPI_2_SIN"  , true, OUTPUT_INSTRUMENTATION), this);
	for(i = 0; i < DSPI_2::NUM_CTARS; i++)
	{
		std::stringstream dspi_2_pcs_serial_bus_name_sstr;
		dspi_2_pcs_serial_bus_name_sstr << "DSPI_2_PCS_SERIAL_BUS_" << i;
		std::stringstream dspi_2_pcs_name_sstr;
		dspi_2_pcs_name_sstr << "DSPI_2_PCS_" << i;
		dspi_2_pcs_serial_bus[i] = new DSPI_2_PCS_SERIAL_BUS(dspi_2_pcs_serial_bus_name_sstr.str().c_str(), instrumenter->CreateSignal(dspi_2_pcs_name_sstr.str(), true, OUTPUT_INSTRUMENTATION), this);
	}
	dspi_2_ss_serial_bus = new DSPI_2_SS_SERIAL_BUS("DSPI_2_SS_SERIAL_BUS", instrumenter->CreateSignal("DSPI_2_SS", true, OUTPUT_INSTRUMENTATION), this);
	
	dspi_3_sout_serial_bus  = new DSPI_3_SOUT_SERIAL_BUS ("DSPI_3_SOUT_SERIAL_BUS" , instrumenter->CreateSignal("DSPI_3_SOUT" , true, OUTPUT_INSTRUMENTATION), this);
	dspi_3_sin_serial_bus   = new DSPI_3_SIN_SERIAL_BUS  ("DSPI_3_SIN_SERIAL_BUS"  , instrumenter->CreateSignal("DSPI_3_SIN"  , true, OUTPUT_INSTRUMENTATION), this);
	for(i = 0; i < DSPI_3::NUM_CTARS; i++)
	{
		std::stringstream dspi_3_pcs_serial_bus_name_sstr;
		dspi_3_pcs_serial_bus_name_sstr << "DSPI_3_PCS_SERIAL_BUS_" << i;
		std::stringstream dspi_3_pcs_name_sstr;
		dspi_3_pcs_name_sstr << "DSPI_3_PCS_" << i;
		dspi_3_pcs_serial_bus[i] = new DSPI_3_PCS_SERIAL_BUS(dspi_3_pcs_serial_bus_name_sstr.str().c_str(), instrumenter->CreateSignal(dspi_3_pcs_name_sstr.str(), true, OUTPUT_INSTRUMENTATION), this);
	}
	dspi_3_ss_serial_bus = new DSPI_3_SS_SERIAL_BUS("DSPI_3_SS_SERIAL_BUS", instrumenter->CreateSignal("DSPI_3_SS", true, OUTPUT_INSTRUMENTATION), this);
	
	dspi_4_sout_serial_bus  = new DSPI_4_SOUT_SERIAL_BUS ("DSPI_4_SOUT_SERIAL_BUS" , instrumenter->CreateSignal("DSPI_4_SOUT" , true, OUTPUT_INSTRUMENTATION), this);
	dspi_4_sin_serial_bus   = new DSPI_4_SIN_SERIAL_BUS  ("DSPI_4_SIN_SERIAL_BUS"  , instrumenter->CreateSignal("DSPI_4_SIN"  , true, OUTPUT_INSTRUMENTATION), this);
	for(i = 0; i < DSPI_4::NUM_CTARS; i++)
	{
		std::stringstream dspi_4_pcs_serial_bus_name_sstr;
		dspi_4_pcs_serial_bus_name_sstr << "DSPI_4_PCS_SERIAL_BUS_" << i;
		std::stringstream dspi_4_pcs_name_sstr;
		dspi_4_pcs_name_sstr << "DSPI_4_PCS_" << i;
		dspi_4_pcs_serial_bus[i] = new DSPI_4_PCS_SERIAL_BUS(dspi_4_pcs_serial_bus_name_sstr.str().c_str(), instrumenter->CreateSignal(dspi_4_pcs_name_sstr.str(), true, OUTPUT_INSTRUMENTATION), this);
	}
	dspi_4_ss_serial_bus = new DSPI_4_SS_SERIAL_BUS("DSPI_4_SS_SERIAL_BUS", instrumenter->CreateSignal("DSPI_4_SS", true, OUTPUT_INSTRUMENTATION), this);
	
	dspi_5_sout_serial_bus  = new DSPI_5_SOUT_SERIAL_BUS ("DSPI_5_SOUT_SERIAL_BUS" , instrumenter->CreateSignal("DSPI_5_SOUT" , true, OUTPUT_INSTRUMENTATION), this);
	dspi_5_sin_serial_bus   = new DSPI_5_SIN_SERIAL_BUS  ("DSPI_5_SIN_SERIAL_BUS"  , instrumenter->CreateSignal("DSPI_5_SIN"  , true, OUTPUT_INSTRUMENTATION), this);
	for(i = 0; i < DSPI_5::NUM_CTARS; i++)
	{
		std::stringstream dspi_5_pcs_serial_bus_name_sstr;
		dspi_5_pcs_serial_bus_name_sstr << "DSPI_5_PCS_SERIAL_BUS_" << i;
		std::stringstream dspi_5_pcs_name_sstr;
		dspi_5_pcs_name_sstr << "DSPI_5_PCS_" << i;
		dspi_5_pcs_serial_bus[i] = new DSPI_5_PCS_SERIAL_BUS(dspi_5_pcs_serial_bus_name_sstr.str().c_str(), instrumenter->CreateSignal(dspi_5_pcs_name_sstr.str(), true, OUTPUT_INSTRUMENTATION), this);
	}
	dspi_5_ss_serial_bus = new DSPI_5_SS_SERIAL_BUS("DSPI_5_SS_SERIAL_BUS", instrumenter->CreateSignal("DSPI_5_SS", true, OUTPUT_INSTRUMENTATION), this);
	
	dspi_6_sout_serial_bus  = new DSPI_6_SOUT_SERIAL_BUS ("DSPI_6_SOUT_SERIAL_BUS" , instrumenter->CreateSignal("DSPI_6_SOUT" , true, OUTPUT_INSTRUMENTATION), this);
	dspi_6_sin_serial_bus   = new DSPI_6_SIN_SERIAL_BUS  ("DSPI_6_SIN_SERIAL_BUS"  , instrumenter->CreateSignal("DSPI_6_SIN"  , true, OUTPUT_INSTRUMENTATION), this);
	for(i = 0; i < DSPI_6::NUM_CTARS; i++)
	{
		std::stringstream dspi_6_pcs_serial_bus_name_sstr;
		dspi_6_pcs_serial_bus_name_sstr << "DSPI_6_PCS_SERIAL_BUS_" << i;
		std::stringstream dspi_6_pcs_name_sstr;
		dspi_6_pcs_name_sstr << "DSPI_6_PCS_" << i;
		dspi_6_pcs_serial_bus[i] = new DSPI_6_PCS_SERIAL_BUS(dspi_6_pcs_serial_bus_name_sstr.str().c_str(), instrumenter->CreateSignal(dspi_6_pcs_name_sstr.str(), true, OUTPUT_INSTRUMENTATION), this);
	}
	dspi_6_ss_serial_bus = new DSPI_6_SS_SERIAL_BUS("DSPI_6_SS_SERIAL_BUS", instrumenter->CreateSignal("DSPI_6_SS", true, OUTPUT_INSTRUMENTATION), this);
	
	dspi_12_sout_serial_bus = new DSPI_12_SOUT_SERIAL_BUS("DSPI_12_SOUT_SERIAL_BUS", instrumenter->CreateSignal("DSPI_12_SOUT", true, OUTPUT_INSTRUMENTATION), this);
	dspi_12_sin_serial_bus  = new DSPI_12_SIN_SERIAL_BUS ("DSPI_12_SIN_SERIAL_BUS" , instrumenter->CreateSignal("DSPI_12_SIN" , true, OUTPUT_INSTRUMENTATION), this);
	for(i = 0; i < DSPI_12::NUM_CTARS; i++)
	{
		std::stringstream dspi_12_pcs_serial_bus_name_sstr;
		dspi_12_pcs_serial_bus_name_sstr << "DSPI_12_PCS_SERIAL_BUS_" << i;
		std::stringstream dspi_12_pcs_name_sstr;
		dspi_12_pcs_name_sstr << "DSPI_12_PCS_" << i;
		dspi_12_pcs_serial_bus[i] = new DSPI_12_PCS_SERIAL_BUS(dspi_12_pcs_serial_bus_name_sstr.str().c_str(), instrumenter->CreateSignal(dspi_12_pcs_name_sstr.str(), true, OUTPUT_INSTRUMENTATION), this);
	}
	dspi_12_ss_serial_bus = new DSPI_12_SS_SERIAL_BUS("DSPI_12_SS_SERIAL_BUS", instrumenter->CreateSignal("DSPI_12_SS", true, OUTPUT_INSTRUMENTATION), this);

	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	// TLM sockets
	main_core_0->i_ahb_if(*xbar_0->targ_socket[0]);                     //       Main_Core_0>I_AHB_IF <-> XBAR_0 M0
	main_core_0->d_ahb_if(*xbar_0->targ_socket[1]);                     //       Main_Core_0>D_AHB_IF <-> XBAR_0 M1
	main_core_1->i_ahb_if(*xbar_0->targ_socket[2]);                     //       Main_Core_1>I_AHB_IF <-> XBAR_0 M2
	main_core_1->d_ahb_if(*xbar_0->targ_socket[3]);                     //       Main_Core_1>D_AHB_IF <-> XBAR_0 M3
	peripheral_core_2->i_ahb_if(*xbar_0->targ_socket[5]);               // Peripheral_Core_2>I_AHB_IF <-> XBAR_0 M5
	
	peripheral_core_2->d_ahb_if(*xbar_1->targ_socket[0]);               // Peripheral_Core_2>D_AHB_IF <-> XBAR_1 M0
	(*xbar_1_m1_concentrator->init_socket[0])(*xbar_1->targ_socket[1]); //       XBAR_1 M1 <-> DMA and HSM
	xbar_1_m2_stub->master_sock(*xbar_1->targ_socket[2]);               // TODO: XBAR_1 M2 <-> FEC, LFAST and FlexRay
	
	(*xbar_0->init_socket[0])(flash->slave_sock);                       //       XBAR_0 S0 <-> FLASH Port 0
	(*xbar_0->init_socket[1])(flash_port1_stub->slave_sock);            // TODO: XBAR_0 S1 <-> FLASH Port 1
	(*xbar_0->init_socket[2])(main_core_0->s_ahb_if);                   //       XBAR_0 S2 <-> S_AHB_IF<Main_Core_0
	(*xbar_0->init_socket[3])(main_core_1->s_ahb_if);                   //       XBAR_0 S3 <-> S_AHB_IF<Main_Core_1
	(*xbar_0->init_socket[4])(system_sram->slave_sock);                 //       XBAR_0 S4 <-> System SRAM
	(*xbar_0->init_socket[5])(*ebi->targ_socket[0]);                    //       XBAR_0 S5 <-> EBI
	(*xbar_0->init_socket[6])(xbar_0_s6_stub->slave_sock);              //       XBAR_0 S6 <-> unused
	(*xbar_0->init_socket[7])(*iahbg_0->targ_socket[0]);                //       XBAR_0 S7 <-> XBAR_1 M3 (through IAHBG_0)
                                                                     
	(*xbar_1->init_socket[0])(*iahbg_1->targ_socket[0]);                //       XBAR_1 S0 <-> XBAR_0 M4 (through IAHBG_1)
	(*xbar_1->init_socket[1])(peripheral_core_2->s_ahb_if);             //       XBAR_1 S1 <-> S_AHB_IF<Peripheral_Core_2
	(*xbar_1->init_socket[2])(*pbridge_b->targ_socket[0]);              //       XBAR_1 S2 <-> PBRIDGE_B
	(*xbar_1->init_socket[3])(*pbridge_a->targ_socket[0]);              //       XBAR_1 S3 <-> PBRIDGE_A
	
	(*iahbg_0->init_socket[0])(*xbar_1->targ_socket[3]);                // IAHBG_0: XBAR_0 S7 -> XBAR_1 M3
	(*iahbg_1->init_socket[0])(*xbar_0->targ_socket[4]);                // IAHBG_1: XBAR_1 S0 -> XBAR_0 M4
	
	(*pbridge_a->init_socket[0])(intc_0->peripheral_slave_if);          // PBRIDGE_A <-> INTC_0
	(*pbridge_a->init_socket[1])(stm_0->peripheral_slave_if);           // PBRIDGE_A <-> STM_0
	(*pbridge_a->init_socket[2])(stm_1->peripheral_slave_if);           // PBRIDGE_A <-> STM_1
	(*pbridge_a->init_socket[3])(stm_2->peripheral_slave_if);           // PBRIDGE_A <-> STM_2
	(*pbridge_a->init_socket[4])(swt_0->peripheral_slave_if);           // PBRIDGE_A <-> SWT_0
	(*pbridge_a->init_socket[5])(swt_1->peripheral_slave_if);           // PBRIDGE_A <-> SWT_1
	(*pbridge_a->init_socket[6])(swt_2->peripheral_slave_if);           // PBRIDGE_A <-> SWT_2
	(*pbridge_a->init_socket[7])(swt_3->peripheral_slave_if);           // PBRIDGE_A <-> SWT_3
	(*pbridge_a->init_socket[8])(pit_0->peripheral_slave_if);           // PBRIDGE_A <-> PIT_0
	(*pbridge_a->init_socket[9])(pit_1->peripheral_slave_if);           // PBRIDGE_A <-> PIT_1
	(*pbridge_a->init_socket[10])(linflexd_0->peripheral_slave_if);     // PBRIDGE_A <-> LINFlexD_0
	(*pbridge_a->init_socket[11])(linflexd_1->peripheral_slave_if);     // PBRIDGE_A <-> LINFlexD_1
	(*pbridge_a->init_socket[12])(linflexd_14->peripheral_slave_if);    // PBRIDGE_A <-> LINFlexD_14
	(*pbridge_a->init_socket[13])(linflexd_16->peripheral_slave_if);    // PBRIDGE_A <-> LINFlexD_16
	(*pbridge_a->init_socket[14])(dmamux_0->peripheral_slave_if);       // PBRIDGE_A <-> DMAMUX_0
	(*pbridge_a->init_socket[15])(dmamux_1->peripheral_slave_if);       // PBRIDGE_A <-> DMAMUX_1
	(*pbridge_a->init_socket[16])(dmamux_2->peripheral_slave_if);       // PBRIDGE_A <-> DMAMUX_2
	(*pbridge_a->init_socket[17])(dmamux_3->peripheral_slave_if);       // PBRIDGE_A <-> DMAMUX_3
	(*pbridge_a->init_socket[18])(dmamux_4->peripheral_slave_if);       // PBRIDGE_A <-> DMAMUX_4
	(*pbridge_a->init_socket[19])(dmamux_5->peripheral_slave_if);       // PBRIDGE_A <-> DMAMUX_5
	(*pbridge_a->init_socket[20])(dmamux_6->peripheral_slave_if);       // PBRIDGE_A <-> DMAMUX_6
	(*pbridge_a->init_socket[21])(dmamux_7->peripheral_slave_if);       // PBRIDGE_A <-> DMAMUX_7
	(*pbridge_a->init_socket[22])(dmamux_8->peripheral_slave_if);       // PBRIDGE_A <-> DMAMUX_8
	(*pbridge_a->init_socket[23])(dmamux_9->peripheral_slave_if);       // PBRIDGE_A <-> DMAMUX_9
	(*pbridge_a->init_socket[24])(edma_0->peripheral_slave_if);         // PBRIDGE_A <-> EDMA_0
	(*pbridge_a->init_socket[25])(edma_1->peripheral_slave_if);         // PBRIDGE_A <-> EDMA_1
	(*pbridge_a->init_socket[26])(dspi_0->peripheral_slave_if);         // PBRIDGE_A <-> DSPI_0
	(*pbridge_a->init_socket[27])(dspi_1->peripheral_slave_if);         // PBRIDGE_A <-> DSPI_1
	(*pbridge_a->init_socket[28])(dspi_4->peripheral_slave_if);         // PBRIDGE_A <-> DSPI_4
	(*pbridge_a->init_socket[29])(dspi_6->peripheral_slave_if);         // PBRIDGE_A <-> DSPI_6
	(*pbridge_a->init_socket[30])(dspi_12->peripheral_slave_if);        // PBRIDGE_A <-> DSPI_12
	(*pbridge_a->init_socket[31])(pcm_stub->slave_sock);                // PBRIDGE_A <-> PCM
	(*pbridge_a->init_socket[32])(pflash_stub->slave_sock);             // PBRIDGE_A <-> PFLASH
	(*pbridge_a->init_socket[33])(mc_me_stub->slave_sock);              // PBRIDGE_A <-> MC_ME
	(*pbridge_a->init_socket[34])(mc_cgm_stub->slave_sock);             // PBRIDGE_A <-> MC_CGM
	(*pbridge_a->init_socket[35])(xbar_0->peripheral_slave_if[XBAR_0_CONFIG::XBAR_PERIPHERAL_SLAVE_IF]);         // PBRIDGE_A <-> XBAR_0
	(*pbridge_a->init_socket[36])(xbar_1->peripheral_slave_if[XBAR_1_CONFIG::XBAR_PERIPHERAL_SLAVE_IF]);         // PBRIDGE_A <-> XBAR_1
	(*pbridge_a->init_socket[37])(pbridge_a->peripheral_slave_if);      // PBRIDGE_A <-> PBRIDGE_A
	(*pbridge_a->init_socket[38])(siul2->peripheral_slave_if);          // PBRIDGE_A <-> SIUL2
	(*pbridge_a->init_socket[39])(ebi->peripheral_slave_if);            // PBRIDGE_A <-> EBI
	(*pbridge_a->init_socket[40])(xbar_0->peripheral_slave_if[XBAR_0_CONFIG::SMPU_PERIPHERAL_SLAVE_IF]);         // PBRIDGE_A <-> SMPU_0
	(*pbridge_a->init_socket[41])(xbar_1->peripheral_slave_if[XBAR_1_CONFIG::SMPU_PERIPHERAL_SLAVE_IF]);         // PBRIDGE_A <-> SMPU_1
	(*pbridge_a->init_socket[42])(mc_rgm_stub->slave_sock);             // PBRIDGE_A <-> MC_RGM
	(*pbridge_a->init_socket[43])(pram_0_stub->slave_sock);             // PBRIDGE_A <-> PRAM_0
	(*pbridge_a->init_socket[44])(m_can_1->peripheral_slave_if);        // PBRIDGE_A <-> M_CAN_1
	(*pbridge_a->init_socket[45])(m_can_2->peripheral_slave_if);        // PBRIDGE_A <-> M_CAN_2
	(*pbridge_a->init_socket[46])(m_can_3->peripheral_slave_if);        // PBRIDGE_A <-> M_CAN_3
	(*pbridge_a->init_socket[47])(m_can_4->peripheral_slave_if);        // PBRIDGE_A <-> M_CAN_4
	(*pbridge_a->init_socket[48])(*shared_can_message_ram_router->targ_socket[0]); // PBRIDGE_A <-> Shared CAN message RAM Controller/Access Arbiter
	(*pbridge_a->init_socket[49])(sema4->peripheral_slave_if);          // PBRIDGE_A <-> SEMA4
	
	(*pbridge_b->init_socket[0])(linflexd_2->peripheral_slave_if);      // PBRIDGE_B <-> LINFlexD_2
	(*pbridge_b->init_socket[1])(linflexd_15->peripheral_slave_if);     // PBRIDGE_B <-> LINFlexD_15
	(*pbridge_b->init_socket[2])(dspi_2->peripheral_slave_if);          // PBRIDGE_B <-> DSPI_2
	(*pbridge_b->init_socket[3])(dspi_3->peripheral_slave_if);          // PBRIDGE_B <-> DSPI_3
	(*pbridge_b->init_socket[4])(dspi_5->peripheral_slave_if);          // PBRIDGE_B <-> DSPI_5
	(*pbridge_b->init_socket[5])(pbridge_b->peripheral_slave_if);       // PBRIDGE_B <-> PBRIDGE_B

	(*ebi->init_socket[0])(ebi_mem_0->slave_sock);
	(*ebi->init_socket[1])(ebi_mem_1->slave_sock);
	(*ebi->init_socket[2])(ebi_mem_2->slave_sock);
	
	m_can_1->can_message_ram_if(*shared_can_message_ram_router->targ_socket[1]);
	m_can_2->can_message_ram_if(*shared_can_message_ram_router->targ_socket[2]);
	m_can_3->can_message_ram_if(*shared_can_message_ram_router->targ_socket[3]);
	m_can_4->can_message_ram_if(*shared_can_message_ram_router->targ_socket[4]);
	(*shared_can_message_ram_router->init_socket[0])(shared_can_message_ram->slave_sock);  // Shared CAN message RAM Controller/Access Arbiter <-> Shared CAN message RAM
	
	m_can_1->CAN_TX(can_bus->CAN_TX);
	m_can_1->CAN_RX(can_bus->CAN_RX);
	m_can_2->CAN_TX(can_bus->CAN_TX);
	m_can_2->CAN_RX(can_bus->CAN_RX);
	m_can_3->CAN_TX(can_bus->CAN_TX);
	m_can_3->CAN_RX(can_bus->CAN_RX);
	m_can_4->CAN_TX(can_bus->CAN_TX);
	m_can_4->CAN_RX(can_bus->CAN_RX);
	
	edma_0->master_if(*xbar_1_m1_concentrator->targ_socket[0]);         // EDMA_0 <-> XBAR_1 M1 Concentrator
	edma_1->master_if(*xbar_1_m1_concentrator->targ_socket[1]);         // EDMA_1 <-> XBAR_1 M1 Concentrator
	
	linflexd_0->LINTX(linflexd_0_tx_serial_bus->serial_socket);         //  LINFlexD_0 TX <->  LINFlexD_0 TX serial bus
	linflexd_0->LINRX(linflexd_0_rx_serial_bus->serial_socket);         //  LINFlexD_0 RX <->  LINFlexD_0 RX serial bus
	linflexd_1->LINTX(linflexd_1_tx_serial_bus->serial_socket);         //  LINFlexD_1 TX <->  LINFlexD_1 TX serial bus
	linflexd_1->LINRX(linflexd_1_rx_serial_bus->serial_socket);         //  LINFlexD_1 RX <->  LINFlexD_1 RX serial bus
	linflexd_2->LINTX(linflexd_2_tx_serial_bus->serial_socket);         //  LINFlexD_2 TX <->  LINFlexD_2 TX serial bus
	linflexd_2->LINRX(linflexd_2_rx_serial_bus->serial_socket);         //  LINFlexD_2 RX <->  LINFlexD_2 RX serial bus
	linflexd_14->LINTX(linflexd_14_tx_serial_bus->serial_socket);       // LINFlexD_14 TX <-> LINFlexD_14 TX serial bus
	linflexd_14->LINRX(linflexd_14_rx_serial_bus->serial_socket);       // LINFlexD_14 RX <-> LINFlexD_14 RX serial bus
	linflexd_15->LINTX(linflexd_15_tx_serial_bus->serial_socket);       // LINFlexD_15 TX <-> LINFlexD_15 TX serial bus
	linflexd_15->LINRX(linflexd_15_rx_serial_bus->serial_socket);       // LINFlexD_15 RX <-> LINFlexD_15 RX serial bus
	linflexd_16->LINTX(linflexd_16_tx_serial_bus->serial_socket);       // LINFlexD_16 TX <-> LINFlexD_16 TX serial bus
	linflexd_16->LINRX(linflexd_16_rx_serial_bus->serial_socket);       // LINFlexD_16 RX <-> LINFlexD_16 RX serial bus
	
	if(serial_terminal0)
	{
		serial_terminal0->RX(linflexd_0_tx_serial_bus->serial_socket);
		serial_terminal0->TX(linflexd_0_rx_serial_bus->serial_socket);
	}
	if(serial_terminal1)
	{
		serial_terminal1->RX(linflexd_1_tx_serial_bus->serial_socket);
		serial_terminal1->TX(linflexd_1_rx_serial_bus->serial_socket);
	}
	if(serial_terminal2)
	{
		serial_terminal2->RX(linflexd_2_tx_serial_bus->serial_socket);
		serial_terminal2->TX(linflexd_2_rx_serial_bus->serial_socket);
	}
	if(serial_terminal14)
	{
		serial_terminal14->RX(linflexd_14_tx_serial_bus->serial_socket);
		serial_terminal14->TX(linflexd_14_rx_serial_bus->serial_socket);
	}
	if(serial_terminal15)
	{
		serial_terminal15->RX(linflexd_15_tx_serial_bus->serial_socket);
		serial_terminal15->TX(linflexd_15_rx_serial_bus->serial_socket);
	}
	if(serial_terminal16)
	{
		serial_terminal16->RX(linflexd_16_tx_serial_bus->serial_socket);
		serial_terminal16->TX(linflexd_16_rx_serial_bus->serial_socket);
	}
	
	typedef unisim::kernel::tlm2::tlm_serial_peripheral_socket<> DSPI_SOUT_type;
	typedef unisim::kernel::tlm2::tlm_serial_peripheral_socket<> DSPI_SIN_type;
	typedef unisim::kernel::tlm2::tlm_serial_peripheral_socket<> DSPI_SS_type;
	typedef unisim::kernel::tlm2::tlm_serial_peripheral_socket<> DSPI_PCS_type;
	typedef unisim::kernel::tlm2::tlm_simple_serial_bus DSPI_SERIAL_BUS;

	std::map<unsigned int, DSPI_SOUT_type *> dspi_sout;
	std::map<unsigned int, DSPI_SIN_type *> dspi_sin;
	std::map<unsigned int, DSPI_SS_type *> dspi_ss;
	std::map<unsigned int, std::map<unsigned int, DSPI_PCS_type *> > dspi_pcs;
	
	std::map<unsigned int, DSPI_SERIAL_BUS *> dspi_sout_serial_bus;
	std::map<unsigned int, DSPI_SERIAL_BUS *> dspi_sin_serial_bus;
	std::map<unsigned int, DSPI_SERIAL_BUS *> dspi_ss_serial_bus;
	std::map<unsigned int, std::map<unsigned int, DSPI_SERIAL_BUS *> > dspi_pcs_serial_bus;
	
	dspi_sout[0] = &dspi_0->SOUT;
	dspi_sout[1] = &dspi_1->SOUT;
	dspi_sout[2] = &dspi_2->SOUT;
	dspi_sout[3] = &dspi_3->SOUT;
	dspi_sout[4] = &dspi_4->SOUT;
	dspi_sout[5] = &dspi_5->SOUT;
	dspi_sout[6] = &dspi_6->SOUT;
	dspi_sout[12] = &dspi_12->SOUT;
	
	dspi_sin[0] = &dspi_0->SIN;
	dspi_sin[1] = &dspi_1->SIN;
	dspi_sin[2] = &dspi_2->SIN;
	dspi_sin[3] = &dspi_3->SIN;
	dspi_sin[4] = &dspi_4->SIN;
	dspi_sin[5] = &dspi_5->SIN;
	dspi_sin[6] = &dspi_6->SIN;
	dspi_sin[12] = &dspi_12->SIN;

	dspi_ss[0] = &dspi_0->SS;
	dspi_ss[1] = &dspi_1->SS;
	dspi_ss[2] = &dspi_2->SS;
	dspi_ss[3] = &dspi_3->SS;
	dspi_ss[4] = &dspi_4->SS;
	dspi_ss[5] = &dspi_5->SS;
	dspi_ss[6] = &dspi_6->SS;
	dspi_ss[12] = &dspi_12->SS;
	
	dspi_sout_serial_bus[0] = dspi_0_sout_serial_bus;
	dspi_sout_serial_bus[1] = dspi_1_sout_serial_bus;
	dspi_sout_serial_bus[2] = dspi_2_sout_serial_bus;
	dspi_sout_serial_bus[3] = dspi_3_sout_serial_bus;
	dspi_sout_serial_bus[4] = dspi_4_sout_serial_bus;
	dspi_sout_serial_bus[5] = dspi_5_sout_serial_bus;
	dspi_sout_serial_bus[6] = dspi_6_sout_serial_bus;
	dspi_sout_serial_bus[12] = dspi_12_sout_serial_bus;
	
	dspi_sin_serial_bus[0] = dspi_0_sin_serial_bus;
	dspi_sin_serial_bus[1] = dspi_1_sin_serial_bus;
	dspi_sin_serial_bus[2] = dspi_2_sin_serial_bus;
	dspi_sin_serial_bus[3] = dspi_3_sin_serial_bus;
	dspi_sin_serial_bus[4] = dspi_4_sin_serial_bus;
	dspi_sin_serial_bus[5] = dspi_5_sin_serial_bus;
	dspi_sin_serial_bus[6] = dspi_6_sin_serial_bus;
	dspi_sin_serial_bus[12] = dspi_12_sin_serial_bus;

	dspi_ss_serial_bus[0] = dspi_0_ss_serial_bus;
	dspi_ss_serial_bus[1] = dspi_1_ss_serial_bus;
	dspi_ss_serial_bus[2] = dspi_2_ss_serial_bus;
	dspi_ss_serial_bus[3] = dspi_3_ss_serial_bus;
	dspi_ss_serial_bus[4] = dspi_4_ss_serial_bus;
	dspi_ss_serial_bus[5] = dspi_5_ss_serial_bus;
	dspi_ss_serial_bus[6] = dspi_6_ss_serial_bus;
	dspi_ss_serial_bus[12] = dspi_12_ss_serial_bus;

	for(i = 0; i < DSPI_0::NUM_CTARS; i++) dspi_pcs_serial_bus[0][i] = dspi_0_pcs_serial_bus[i];
	for(i = 0; i < DSPI_1::NUM_CTARS; i++) dspi_pcs_serial_bus[1][i] = dspi_1_pcs_serial_bus[i];
	for(i = 0; i < DSPI_2::NUM_CTARS; i++) dspi_pcs_serial_bus[2][i] = dspi_2_pcs_serial_bus[i];
	for(i = 0; i < DSPI_3::NUM_CTARS; i++) dspi_pcs_serial_bus[3][i] = dspi_3_pcs_serial_bus[i];
	for(i = 0; i < DSPI_4::NUM_CTARS; i++) dspi_pcs_serial_bus[4][i] = dspi_4_pcs_serial_bus[i];
	for(i = 0; i < DSPI_5::NUM_CTARS; i++) dspi_pcs_serial_bus[5][i] = dspi_5_pcs_serial_bus[i];
	for(i = 0; i < DSPI_6::NUM_CTARS; i++) dspi_pcs_serial_bus[6][i] = dspi_6_pcs_serial_bus[i];
	for(i = 0; i < DSPI_12::NUM_CTARS; i++) dspi_pcs_serial_bus[12][i] = dspi_12_pcs_serial_bus[i];

	for(i = 0; i < DSPI_0::NUM_CTARS; i++) dspi_pcs[0][i] = &dspi_0->PCS[i];
	for(i = 0; i < DSPI_1::NUM_CTARS; i++) dspi_pcs[1][i] = &dspi_1->PCS[i];
	for(i = 0; i < DSPI_2::NUM_CTARS; i++) dspi_pcs[2][i] = &dspi_2->PCS[i];
	for(i = 0; i < DSPI_3::NUM_CTARS; i++) dspi_pcs[3][i] = &dspi_3->PCS[i];
	for(i = 0; i < DSPI_4::NUM_CTARS; i++) dspi_pcs[4][i] = &dspi_4->PCS[i];
	for(i = 0; i < DSPI_5::NUM_CTARS; i++) dspi_pcs[5][i] = &dspi_5->PCS[i];
	for(i = 0; i < DSPI_6::NUM_CTARS; i++) dspi_pcs[6][i] = &dspi_6->PCS[i];
	for(i = 0; i < DSPI_12::NUM_CTARS; i++) dspi_pcs[12][i] = &dspi_12->PCS[i];
	
	std::map<unsigned int, bool> dspi_is_slave;
	std::map<unsigned int, unsigned int> dspi_master;
	std::map<unsigned int, unsigned int> dspi_slave;
	
	dspi_is_slave[0] = dspi_0_is_slave;
	dspi_is_slave[1] = dspi_1_is_slave;
	dspi_is_slave[2] = dspi_2_is_slave;
	dspi_is_slave[3] = dspi_3_is_slave;
	dspi_is_slave[4] = dspi_4_is_slave;
	dspi_is_slave[5] = dspi_5_is_slave;
	dspi_is_slave[6] = dspi_6_is_slave;
	dspi_is_slave[12] = dspi_12_is_slave;
	
	dspi_master[0] = dspi_0_master;
	dspi_master[1] = dspi_1_master;
	dspi_master[2] = dspi_2_master;
	dspi_master[3] = dspi_3_master;
	dspi_master[4] = dspi_4_master;
	dspi_master[5] = dspi_5_master;
	dspi_master[6] = dspi_6_master;
	dspi_master[12] = dspi_12_master;
	
	dspi_slave[0] = dspi_0_slave;
	dspi_slave[1] = dspi_1_slave;
	dspi_slave[2] = dspi_2_slave;
	dspi_slave[3] = dspi_3_slave;
	dspi_slave[4] = dspi_4_slave;
	dspi_slave[5] = dspi_5_slave;
	dspi_slave[6] = dspi_6_slave;
	dspi_slave[12] = dspi_12_slave;
	
	for(i = 0; i <= 12; i++)
	{
		if(((i >= 0) && (i <= 6)) || (i == 12))
		{
			unsigned int s = dspi_slave[i];
			unsigned int m = dspi_master[i];
			
			if(dspi_is_slave[i] && (((s >= 0) && (s <= 6)) || (s == 12)) && (((m >= 0) && (m <= 6)) || (m == 12)))
			{
				logger << DebugInfo << "DSPI_" << i << " is slave #" << s << " of Master #" << m << EndDebugInfo;
				dspi_sout[i]->bind(dspi_sin_serial_bus[m]->serial_socket);    //  DSPI_0 SOUT  <->  DSPI_2 SIN serial bus
				dspi_sin[i]->bind(dspi_sout_serial_bus[m]->serial_socket);   //  DSPI_0 SIN   <->  DSPI_2 SOUT serial bus
				dspi_ss[i]->bind(dspi_pcs_serial_bus[m][s]->serial_socket);   //  DSPI_0_SS    <->  DSPI_2_0 PCS serial bus
			}
			else
			{
				logger << DebugInfo << "DSPI_" << i << " is master" << EndDebugInfo;
				dspi_sout[i]->bind(dspi_sout_serial_bus[i]->serial_socket);                                               //  DSPI_0 SOUT  <->  DSPI_0 SOUT serial bus
				dspi_sin[i]->bind(dspi_sin_serial_bus[i]->serial_socket);                                                //  DSPI_0 SIN   <->  DSPI_0 SIN serial bus
				dspi_ss[i]->bind(dspi_ss_serial_bus[i]->serial_socket);                                                 //  DSPI_0_SS   <->  DSPI_0 SS serial bus
			}
				
			unsigned int num_ctars = 0;
			switch(i)
			{
				case 0: num_ctars = DSPI_0::NUM_CTARS; break;
				case 1: num_ctars = DSPI_1::NUM_CTARS; break;
				case 2: num_ctars = DSPI_2::NUM_CTARS; break;
				case 3: num_ctars = DSPI_3::NUM_CTARS; break;
				case 4: num_ctars = DSPI_4::NUM_CTARS; break;
				case 5: num_ctars = DSPI_5::NUM_CTARS; break;
				case 6: num_ctars = DSPI_6::NUM_CTARS; break;
				case 12: num_ctars = DSPI_12::NUM_CTARS; break;
			}
			
			unsigned int j;
			for(j = 0; j < num_ctars; j++) dspi_pcs[i][j]->bind(dspi_pcs_serial_bus[i][j]->serial_socket);    //  DSPI_0_PCS   <->  DSPI_0 PCS serial bus
		}
	}
	
	// RTL ports
	instrumenter->Bind("HARDWARE.Main_Core_0.m_clk"           , "HARDWARE.COMP_CLK");
	instrumenter->Bind("HARDWARE.Main_Core_0.m_por"           , "HARDWARE.m_por");
	instrumenter->Bind("HARDWARE.Main_Core_0.p_reset_b"       , "HARDWARE.p_reset_b_0");
	instrumenter->Bind("HARDWARE.Main_Core_0.p_nmi_b"         , "HARDWARE.p_nmi_b");
	instrumenter->Bind("HARDWARE.Main_Core_0.p_mcp_b"         , "HARDWARE.p_mcp_b");
	instrumenter->Bind("HARDWARE.Main_Core_0.p_rstbase"       , "HARDWARE.p_rstbase");
	instrumenter->Bind("HARDWARE.Main_Core_0.p_cpuid"         , "HARDWARE.p_cpuid");
	instrumenter->Bind("HARDWARE.Main_Core_0.p_extint_b"      , "HARDWARE.p_irq_b_0");
	instrumenter->Bind("HARDWARE.Main_Core_0.p_crint_b"       , "HARDWARE.p_crint_b");
	instrumenter->Bind("HARDWARE.Main_Core_0.p_avec_b"        , "HARDWARE.p_avec_b_0");
	instrumenter->Bind("HARDWARE.Main_Core_0.p_voffset"       , "HARDWARE.p_voffset_0");
	instrumenter->Bind("HARDWARE.Main_Core_0.p_iack"          , "HARDWARE.p_iack_0");

	instrumenter->Bind("HARDWARE.Main_Core_1.m_clk"           , "HARDWARE.COMP_CLK");
	instrumenter->Bind("HARDWARE.Main_Core_1.m_por"           , "HARDWARE.m_por");
	instrumenter->Bind("HARDWARE.Main_Core_1.p_reset_b"       , "HARDWARE.p_reset_b_1");
	instrumenter->Bind("HARDWARE.Main_Core_1.p_nmi_b"         , "HARDWARE.p_nmi_b");
	instrumenter->Bind("HARDWARE.Main_Core_1.p_mcp_b"         , "HARDWARE.p_mcp_b");
	instrumenter->Bind("HARDWARE.Main_Core_1.p_rstbase"       , "HARDWARE.p_rstbase");
	instrumenter->Bind("HARDWARE.Main_Core_1.p_cpuid"         , "HARDWARE.p_cpuid");
	instrumenter->Bind("HARDWARE.Main_Core_1.p_extint_b"      , "HARDWARE.p_irq_b_1");
	instrumenter->Bind("HARDWARE.Main_Core_1.p_crint_b"       , "HARDWARE.p_crint_b");
	instrumenter->Bind("HARDWARE.Main_Core_1.p_avec_b"        , "HARDWARE.p_avec_b_1");
	instrumenter->Bind("HARDWARE.Main_Core_1.p_voffset"       , "HARDWARE.p_voffset_1");
	instrumenter->Bind("HARDWARE.Main_Core_1.p_iack"          , "HARDWARE.p_iack_1");

	instrumenter->Bind("HARDWARE.Peripheral_Core_2.m_clk"           , "HARDWARE.IOP_CLK");
	instrumenter->Bind("HARDWARE.Peripheral_Core_2.m_por"           , "HARDWARE.m_por");
	instrumenter->Bind("HARDWARE.Peripheral_Core_2.p_reset_b"       , "HARDWARE.p_reset_b_2");
	instrumenter->Bind("HARDWARE.Peripheral_Core_2.p_nmi_b"         , "HARDWARE.p_nmi_b");
	instrumenter->Bind("HARDWARE.Peripheral_Core_2.p_mcp_b"         , "HARDWARE.p_mcp_b");
	instrumenter->Bind("HARDWARE.Peripheral_Core_2.p_rstbase"       , "HARDWARE.p_rstbase");
	instrumenter->Bind("HARDWARE.Peripheral_Core_2.p_cpuid"         , "HARDWARE.p_cpuid");
	instrumenter->Bind("HARDWARE.Peripheral_Core_2.p_extint_b"      , "HARDWARE.p_irq_b_2");
	instrumenter->Bind("HARDWARE.Peripheral_Core_2.p_crint_b"       , "HARDWARE.p_crint_b");
	instrumenter->Bind("HARDWARE.Peripheral_Core_2.p_avec_b"        , "HARDWARE.p_avec_b_2");
	instrumenter->Bind("HARDWARE.Peripheral_Core_2.p_voffset"       , "HARDWARE.p_voffset_2");
	instrumenter->Bind("HARDWARE.Peripheral_Core_2.p_iack"          , "HARDWARE.p_iack_2");
	
	instrumenter->Bind("HARDWARE.XBAR_0.m_clk"                          , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.XBAR_0.reset_b"                        , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.XBAR_0.input_if_clock"                 , "HARDWARE.FXBAR_CLK");
	instrumenter->Bind("HARDWARE.XBAR_0.output_if_clock"                , "HARDWARE.FXBAR_CLK");
	
	instrumenter->Bind("HARDWARE.XBAR_1.m_clk"                          , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.XBAR_1.reset_b"                        , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.XBAR_1.input_if_clock"                 , "HARDWARE.SXBAR_CLK");
	instrumenter->Bind("HARDWARE.XBAR_1.output_if_clock"                , "HARDWARE.SXBAR_CLK");
	
	instrumenter->Bind("HARDWARE.PBRIDGE_A.m_clk"                       , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.PBRIDGE_A.reset_b"                     , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.PBRIDGE_A.input_if_clock"              , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.PBRIDGE_A.output_if_clock"             , "HARDWARE.PBRIDGEA_CLK");
	
	instrumenter->Bind("HARDWARE.PBRIDGE_B.m_clk"                       , "HARDWARE.PBRIDGEB_CLK");
	instrumenter->Bind("HARDWARE.PBRIDGE_B.reset_b"                     , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.PBRIDGE_B.input_if_clock"              , "HARDWARE.PBRIDGEB_CLK");
	instrumenter->Bind("HARDWARE.PBRIDGE_B.output_if_clock"             , "HARDWARE.PBRIDGEB_CLK");
	
	instrumenter->Bind("HARDWARE.EBI.m_clk"                             , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.EBI.reset_b"                           , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.EBI.input_if_clock"                    , "HARDWARE.FXBAR_CLK");
	instrumenter->Bind("HARDWARE.EBI.output_if_clock"                   , "HARDWARE.EBI_CLK");

	instrumenter->Bind("HARDWARE.IAHBG_0.input_if_clock"                , "HARDWARE.FXBAR_CLK");
	instrumenter->Bind("HARDWARE.IAHBG_0.output_if_clock"               , "HARDWARE.SXBAR_CLK");
	
	instrumenter->Bind("HARDWARE.IAHBG_1.input_if_clock"                , "HARDWARE.SXBAR_CLK");
	instrumenter->Bind("HARDWARE.IAHBG_1.output_if_clock"               , "HARDWARE.FXBAR_CLK");
	
	instrumenter->Bind("HARDWARE.XBAR_1_M1_CONCENTRATOR.input_if_clock" , "HARDWARE.SXBAR_CLK");
	instrumenter->Bind("HARDWARE.XBAR_1_M1_CONCENTRATOR.output_if_clock", "HARDWARE.SXBAR_CLK");

	instrumenter->Bind("HARDWARE.INTC_0.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.INTC_0.reset_b"         , "HARDWARE.reset_b");
	instrumenter->BindArray(INTC_0_CONFIG::NUM_PROCESSORS, "HARDWARE.INTC_0.p_irq_b"  , "HARDWARE.p_irq_b"  );
	instrumenter->BindArray(INTC_0_CONFIG::NUM_PROCESSORS, "HARDWARE.INTC_0.p_avec_b" , "HARDWARE.p_avec_b" );
	instrumenter->BindArray(INTC_0_CONFIG::NUM_PROCESSORS, "HARDWARE.INTC_0.p_voffset", "HARDWARE.p_voffset");
	instrumenter->BindArray(INTC_0_CONFIG::NUM_PROCESSORS, "HARDWARE.INTC_0.p_iack"   , "HARDWARE.p_iack"   );
	
	instrumenter->Bind("HARDWARE.STM_0.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.STM_0.reset_b"         , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.STM_0.debug"           , "HARDWARE.debug");

	instrumenter->Bind("HARDWARE.STM_1.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.STM_1.reset_b"         , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.STM_1.debug"           , "HARDWARE.debug");

	instrumenter->Bind("HARDWARE.STM_2.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.STM_2.reset_b"         , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.STM_2.debug"           , "HARDWARE.debug");
	
	instrumenter->Bind("HARDWARE.SWT_0.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.SWT_0.swt_reset_b"     , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.SWT_0.stop"            , "HARDWARE.stop");
	instrumenter->Bind("HARDWARE.SWT_0.debug"           , "HARDWARE.debug");
	instrumenter->Bind("HARDWARE.SWT_0.reset_b"         , "HARDWARE.p_reset_b_0");

	instrumenter->Bind("HARDWARE.SWT_1.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.SWT_1.swt_reset_b"     , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.SWT_1.stop"            , "HARDWARE.stop");
	instrumenter->Bind("HARDWARE.SWT_1.debug"           , "HARDWARE.debug");
	instrumenter->Bind("HARDWARE.SWT_1.reset_b"         , "HARDWARE.p_reset_b_1");

	instrumenter->Bind("HARDWARE.SWT_2.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.SWT_2.swt_reset_b"     , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.SWT_2.stop"            , "HARDWARE.stop");
	instrumenter->Bind("HARDWARE.SWT_2.debug"           , "HARDWARE.debug");
	instrumenter->Bind("HARDWARE.SWT_2.reset_b"         , "HARDWARE.p_reset_b_2");
	
	instrumenter->Bind("HARDWARE.SWT_3.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.SWT_3.swt_reset_b"     , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.SWT_3.stop"            , "HARDWARE.stop");
	instrumenter->Bind("HARDWARE.SWT_3.debug"           , "HARDWARE.debug");
	instrumenter->Bind("HARDWARE.SWT_3.reset_b"         , "HARDWARE.p_reset_b_3");
	
	instrumenter->Bind("HARDWARE.PIT_0.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.PIT_0.per_clk"         , "HARDWARE.PER_CLK");
	instrumenter->Bind("HARDWARE.PIT_0.rti_clk"         , "HARDWARE.RTI_CLK");
	instrumenter->Bind("HARDWARE.PIT_0.reset_b"         , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.PIT_0.debug"           , "HARDWARE.debug");
	
	instrumenter->Bind("HARDWARE.PIT_0.dma_trigger_0"   , "HARDWARE.PIT_0_DMA_TRIGGER_0");
	instrumenter->Bind("HARDWARE.PIT_0.dma_trigger_1"   , "HARDWARE.PIT_0_DMA_TRIGGER_1");
	instrumenter->Bind("HARDWARE.PIT_0.dma_trigger_2"   , "HARDWARE.PIT_0_DMA_TRIGGER_2");
	instrumenter->Bind("HARDWARE.PIT_0.dma_trigger_3"   , "HARDWARE.PIT_0_DMA_TRIGGER_3");
	instrumenter->Bind("HARDWARE.PIT_0.dma_trigger_4"   , "HARDWARE.PIT_0_DMA_TRIGGER_4");
	instrumenter->Bind("HARDWARE.PIT_0.dma_trigger_5"   , "HARDWARE.PIT_0_DMA_TRIGGER_5");
	instrumenter->Bind("HARDWARE.PIT_0.dma_trigger_6"   , "HARDWARE.PIT_0_DMA_TRIGGER_6");
	instrumenter->Bind("HARDWARE.PIT_0.dma_trigger_7"   , "HARDWARE.PIT_0_DMA_TRIGGER_7");
	
	instrumenter->Bind("HARDWARE.PIT_1.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.PIT_1.per_clk"         , "HARDWARE.PER_CLK");
	instrumenter->Bind("HARDWARE.PIT_1.rti_clk"         , "HARDWARE.RTI_CLK");
	instrumenter->Bind("HARDWARE.PIT_1.reset_b"         , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.PIT_1.debug"           , "HARDWARE.debug");
	
	instrumenter->Bind("HARDWARE.PIT_1.irq_2"           , "HARDWARE.unused");
	instrumenter->Bind("HARDWARE.PIT_1.irq_3"           , "HARDWARE.unused");
	instrumenter->Bind("HARDWARE.PIT_1.irq_4"           , "HARDWARE.unused");
	instrumenter->Bind("HARDWARE.PIT_1.irq_5"           , "HARDWARE.unused");
	instrumenter->Bind("HARDWARE.PIT_1.irq_6"           , "HARDWARE.unused");
	instrumenter->Bind("HARDWARE.PIT_1.irq_7"           , "HARDWARE.unused");
	
	instrumenter->Bind("HARDWARE.PIT_1.rtirq"           , "HARDWARE.unused");

	instrumenter->Bind("HARDWARE.PIT_1.dma_trigger_0"   , "HARDWARE.unused");
	instrumenter->Bind("HARDWARE.PIT_1.dma_trigger_1"   , "HARDWARE.unused");
	instrumenter->Bind("HARDWARE.PIT_1.dma_trigger_2"   , "HARDWARE.unused");
	instrumenter->Bind("HARDWARE.PIT_1.dma_trigger_3"   , "HARDWARE.unused");
	instrumenter->Bind("HARDWARE.PIT_1.dma_trigger_4"   , "HARDWARE.unused");
	instrumenter->Bind("HARDWARE.PIT_1.dma_trigger_5"   , "HARDWARE.unused");
	instrumenter->Bind("HARDWARE.PIT_1.dma_trigger_6"   , "HARDWARE.unused");
	instrumenter->Bind("HARDWARE.PIT_1.dma_trigger_7"   , "HARDWARE.unused");
	
	instrumenter->Bind("HARDWARE.LINFlexD_0.m_clk"    , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.LINFlexD_0.lin_clk"  , "HARDWARE.LIN_CLK");
	instrumenter->Bind("HARDWARE.LINFlexD_0.reset_b"  , "HARDWARE.reset_b");

	instrumenter->Bind("HARDWARE.LINFlexD_1.m_clk"    , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.LINFlexD_1.lin_clk"  , "HARDWARE.LIN_CLK");
	instrumenter->Bind("HARDWARE.LINFlexD_1.reset_b"  , "HARDWARE.reset_b");

	instrumenter->Bind("HARDWARE.LINFlexD_14.m_clk"   , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.LINFlexD_14.lin_clk" , "HARDWARE.LIN_CLK");
	instrumenter->Bind("HARDWARE.LINFlexD_14.reset_b" , "HARDWARE.reset_b");

	instrumenter->Bind("HARDWARE.LINFlexD_16.m_clk"   , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.LINFlexD_16.lin_clk" , "HARDWARE.LIN_CLK");
	instrumenter->Bind("HARDWARE.LINFlexD_16.reset_b" , "HARDWARE.reset_b");

	instrumenter->Bind("HARDWARE.LINFlexD_2.m_clk"    , "HARDWARE.PBRIDGEB_CLK");
	instrumenter->Bind("HARDWARE.LINFlexD_2.lin_clk"  , "HARDWARE.LIN_CLK");
	instrumenter->Bind("HARDWARE.LINFlexD_2.reset_b"  , "HARDWARE.reset_b");

	instrumenter->Bind("HARDWARE.LINFlexD_15.m_clk"   , "HARDWARE.PBRIDGEB_CLK");
	instrumenter->Bind("HARDWARE.LINFlexD_15.lin_clk" , "HARDWARE.LIN_CLK");
	instrumenter->Bind("HARDWARE.LINFlexD_15.reset_b" , "HARDWARE.reset_b");
	
	if(serial_terminal0)
	{
		instrumenter->Bind("HARDWARE.SERIAL_TERMINAL0.CLK", "HARDWARE.SERIAL_TERMINAL0_CLK");
	}
	if(serial_terminal1)
	{
		instrumenter->Bind("HARDWARE.SERIAL_TERMINAL1.CLK", "HARDWARE.SERIAL_TERMINAL1_CLK");
	}
	if(serial_terminal2)
	{
		instrumenter->Bind("HARDWARE.SERIAL_TERMINAL2.CLK", "HARDWARE.SERIAL_TERMINAL2_CLK");
	}
	if(serial_terminal14)
	{
		instrumenter->Bind("HARDWARE.SERIAL_TERMINAL14.CLK", "HARDWARE.SERIAL_TERMINAL14_CLK");
	}
	if(serial_terminal15)
	{
		instrumenter->Bind("HARDWARE.SERIAL_TERMINAL15.CLK", "HARDWARE.SERIAL_TERMINAL15_CLK");
	}
	if(serial_terminal16)
	{
		instrumenter->Bind("HARDWARE.SERIAL_TERMINAL16.CLK", "HARDWARE.SERIAL_TERMINAL16_CLK");
	}
	
	instrumenter->BindArray(DMAMUX_0::NUM_DMA_ALWAYS_ON, "HARDWARE.DMAMUX_0.dma_always_on", 0, "HARDWARE.DMA_ALWAYS_ON", 63); // Always 63
	instrumenter->BindArray(DMAMUX_1::NUM_DMA_ALWAYS_ON, "HARDWARE.DMAMUX_1.dma_always_on", 0, "HARDWARE.DMA_ALWAYS_ON", 59); // Always 59 - 63
	instrumenter->BindArray(DMAMUX_2::NUM_DMA_ALWAYS_ON, "HARDWARE.DMAMUX_2.dma_always_on", 0, "HARDWARE.DMA_ALWAYS_ON", 63); // Always 63
	instrumenter->BindArray(DMAMUX_3::NUM_DMA_ALWAYS_ON, "HARDWARE.DMAMUX_3.dma_always_on", 0, "HARDWARE.DMA_ALWAYS_ON", 63); // Always 63
	instrumenter->BindArray(DMAMUX_4::NUM_DMA_ALWAYS_ON, "HARDWARE.DMAMUX_4.dma_always_on", 0, "HARDWARE.DMA_ALWAYS_ON", 63); // Always 63
	instrumenter->BindArray(DMAMUX_5::NUM_DMA_ALWAYS_ON, "HARDWARE.DMAMUX_5.dma_always_on", 0, "HARDWARE.DMA_ALWAYS_ON", 63); // Always 63
	instrumenter->BindArray(DMAMUX_6::NUM_DMA_ALWAYS_ON, "HARDWARE.DMAMUX_6.dma_always_on", 0, "HARDWARE.DMA_ALWAYS_ON", 63); // Always 63
	instrumenter->BindArray(DMAMUX_7::NUM_DMA_ALWAYS_ON, "HARDWARE.DMAMUX_7.dma_always_on", 0, "HARDWARE.DMA_ALWAYS_ON", 63); // Always 63
	instrumenter->BindArray(DMAMUX_8::NUM_DMA_ALWAYS_ON, "HARDWARE.DMAMUX_8.dma_always_on", 0, "HARDWARE.DMA_ALWAYS_ON", 63); // Always 63
	instrumenter->BindArray(DMAMUX_9::NUM_DMA_ALWAYS_ON, "HARDWARE.DMAMUX_9.dma_always_on", 0, "HARDWARE.DMA_ALWAYS_ON", 63); // Always 63
	
	instrumenter->BindArray(DMAMUX_1::NUM_DMA_TRIGGERS, "HARDWARE.DMAMUX_1.dma_trigger", 0, "HARDWARE.PIT_0_DMA_TRIGGER", 0); // PIT_0 Trigger 0 - 4
	instrumenter->BindArray(DMAMUX_2::NUM_DMA_TRIGGERS, "HARDWARE.DMAMUX_2.dma_trigger", 0, "HARDWARE.PIT_0_DMA_TRIGGER", 5); // PIT_0 Trigger 5
	instrumenter->BindArray(DMAMUX_4::NUM_DMA_TRIGGERS, "HARDWARE.DMAMUX_4.dma_trigger", 0, "HARDWARE.PIT_0_DMA_TRIGGER", 6); // PIT_0 Trigger 6
	instrumenter->BindArray(DMAMUX_5::NUM_DMA_TRIGGERS, "HARDWARE.DMAMUX_5.dma_trigger", 0, "HARDWARE.PIT_0_DMA_TRIGGER", 7); // PIT_0 Trigger 7
	
	instrumenter->BindArray(DMAMUX_0::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_0.dma_channel", 0, "HARDWARE.DMA_CHANNEL", 0);   // DMA channels 0 - 7
	instrumenter->BindArray(DMAMUX_1::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_1.dma_channel", 0, "HARDWARE.DMA_CHANNEL", 8);   // DMA channels 0 - 15
	instrumenter->BindArray(DMAMUX_2::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_2.dma_channel", 0, "HARDWARE.DMA_CHANNEL", 16);  // DMA channels 16 - 23
	instrumenter->BindArray(DMAMUX_3::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_3.dma_channel", 0, "HARDWARE.DMA_CHANNEL", 24);  // DMA channels 24 - 31
	instrumenter->BindArray(DMAMUX_4::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_4.dma_channel", 0, "HARDWARE.DMA_CHANNEL", 32);  // DMA channels 32 - 47
	instrumenter->BindArray(DMAMUX_5::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_5.dma_channel", 0, "HARDWARE.DMA_CHANNEL", 48);  // DMA channels 48 - 63
	instrumenter->BindArray(DMAMUX_6::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_6.dma_channel", 0, "HARDWARE.DMA_CHANNEL", 64);  // DMA channels 64 - 79
	instrumenter->BindArray(DMAMUX_7::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_7.dma_channel", 0, "HARDWARE.DMA_CHANNEL", 80);  // DMA channels 80 - 95
	instrumenter->BindArray(DMAMUX_8::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_8.dma_channel", 0, "HARDWARE.DMA_CHANNEL", 96);  // DMA channels 96 - 111
	instrumenter->BindArray(DMAMUX_9::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_9.dma_channel", 0, "HARDWARE.DMA_CHANNEL", 112); // DMA channels 112 - 127
	
	instrumenter->BindArray(DMAMUX_0::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_0.dma_channel_ack", 0, "HARDWARE.DMA_CHANNEL_ACK", 0);   // DMA channels 0 - 7
	instrumenter->BindArray(DMAMUX_1::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_1.dma_channel_ack", 0, "HARDWARE.DMA_CHANNEL_ACK", 8);   // DMA channels 0 - 15
	instrumenter->BindArray(DMAMUX_2::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_2.dma_channel_ack", 0, "HARDWARE.DMA_CHANNEL_ACK", 16);  // DMA channels 16 - 23
	instrumenter->BindArray(DMAMUX_3::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_3.dma_channel_ack", 0, "HARDWARE.DMA_CHANNEL_ACK", 24);  // DMA channels 24 - 31
	instrumenter->BindArray(DMAMUX_4::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_4.dma_channel_ack", 0, "HARDWARE.DMA_CHANNEL_ACK", 32);  // DMA channels 32 - 47
	instrumenter->BindArray(DMAMUX_5::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_5.dma_channel_ack", 0, "HARDWARE.DMA_CHANNEL_ACK", 48);  // DMA channels 48 - 63
	instrumenter->BindArray(DMAMUX_6::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_6.dma_channel_ack", 0, "HARDWARE.DMA_CHANNEL_ACK", 64);  // DMA channels 64 - 79
	instrumenter->BindArray(DMAMUX_7::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_7.dma_channel_ack", 0, "HARDWARE.DMA_CHANNEL_ACK", 80);  // DMA channels 80 - 95
	instrumenter->BindArray(DMAMUX_8::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_8.dma_channel_ack", 0, "HARDWARE.DMA_CHANNEL_ACK", 96);  // DMA channels 96 - 111
	instrumenter->BindArray(DMAMUX_9::NUM_DMA_CHANNELS, "HARDWARE.DMAMUX_9.dma_channel_ack", 0, "HARDWARE.DMA_CHANNEL_ACK", 112); // DMA channels 112 - 127

	instrumenter->Bind("HARDWARE.eDMA_0.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.eDMA_0.dma_clk"         , "HARDWARE.SXBAR_CLK");
	instrumenter->Bind("HARDWARE.eDMA_0.reset_b"         , "HARDWARE.reset_b");
	instrumenter->BindArray(EDMA_0::NUM_DMA_CHANNELS, "HARDWARE.eDMA_0.dma_channel", 0, "HARDWARE.DMA_CHANNEL", 0);  // DMA channels  0 - 63
	instrumenter->BindArray(EDMA_0::NUM_DMA_CHANNELS, "HARDWARE.eDMA_0.dma_channel_ack", 0, "HARDWARE.DMA_CHANNEL_ACK", 0);  // DMA channels  0 - 63
	
	instrumenter->Bind("HARDWARE.eDMA_1.m_clk"           , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.eDMA_1.dma_clk"         , "HARDWARE.SXBAR_CLK");
	instrumenter->Bind("HARDWARE.eDMA_1.reset_b"         , "HARDWARE.reset_b");
	instrumenter->BindArray(EDMA_1::NUM_DMA_CHANNELS, "HARDWARE.eDMA_1.dma_channel", 0, "HARDWARE.DMA_CHANNEL", 64); // DMA channels 64 - 127
	instrumenter->BindArray(EDMA_1::NUM_DMA_CHANNELS, "HARDWARE.eDMA_1.dma_channel_ack", 0, "HARDWARE.DMA_CHANNEL_ACK", 64); // DMA channels 64 - 127
	
	instrumenter->Bind("HARDWARE.DMAMUX_0.reset_b", "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DMAMUX_1.reset_b", "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DMAMUX_2.reset_b", "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DMAMUX_3.reset_b", "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DMAMUX_4.reset_b", "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DMAMUX_5.reset_b", "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DMAMUX_6.reset_b", "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DMAMUX_7.reset_b", "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DMAMUX_8.reset_b", "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DMAMUX_9.reset_b", "HARDWARE.reset_b");
	
	instrumenter->Bind("HARDWARE.DMAMUX_0.m_clk", "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.DMAMUX_1.m_clk", "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.DMAMUX_2.m_clk", "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.DMAMUX_3.m_clk", "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.DMAMUX_4.m_clk", "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.DMAMUX_5.m_clk", "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.DMAMUX_6.m_clk", "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.DMAMUX_7.m_clk", "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.DMAMUX_8.m_clk", "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.DMAMUX_9.m_clk", "HARDWARE.PBRIDGEA_CLK");
	
	instrumenter->Bind("HARDWARE.DSPI_0.m_clk"    , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.DSPI_0.dspi_clk" , "HARDWARE.DSPI_CLK1");
	instrumenter->Bind("HARDWARE.DSPI_0.reset_b"  , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DSPI_0.debug"    , "HARDWARE.debug");
	instrumenter->Bind("HARDWARE.DSPI_0.HT"       , "HARDWARE.pull_down");
	instrumenter->Bind("HARDWARE.DSPI_0.INT_TFUF" , "HARDWARE.DSPI_0_INT_TFUF");
	instrumenter->Bind("HARDWARE.DSPI_0.INT_RFOF" , "HARDWARE.DSPI_0_INT_RFOF");
	instrumenter->Bind("HARDWARE.DSPI_0.INT_TFIWF", "HARDWARE.DSPI_0_INT_TFIWF");
	instrumenter->Bind("HARDWARE.DSPI_0.INT_DDIF" , "HARDWARE.DSPI_0_INT_DDIF");
	instrumenter->Bind("HARDWARE.DSPI_0.INT_DPEF" , "HARDWARE.DSPI_0_INT_DPEF");
	instrumenter->Bind("HARDWARE.DSPI_0.INT_SPITCF", "HARDWARE.DSPI_0_INT_SPITCF");
	instrumenter->Bind("HARDWARE.DSPI_0.INT_DSITCF", "HARDWARE.DSPI_0_INT_DSITCF");
	instrumenter->BindArray(DSPI_0::NUM_DSI_INPUTS, "HARDWARE.DSPI_0.DSI_INPUT", 0, 1, "HARDWARE.pull_down", 0, 0);
	instrumenter->BindArray(DSPI_0::NUM_DSI_OUTPUTS, "HARDWARE.DSPI_0.DSI_OUTPUT", 0, 1, "HARDWARE.unused", 0, 0);
	instrumenter->Bind("HARDWARE.DSPI_0.DMA_DD"   , "HARDWARE.DSPI_0_DMA_DD");
	instrumenter->Bind("HARDWARE.DSPI_0.DMA_ACK_DD", "HARDWARE.DSPI_0_DMA_ACK_DD");
	
	instrumenter->Bind("HARDWARE.DSPI_1.m_clk"    , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.DSPI_1.dspi_clk" , "HARDWARE.DSPI_CLK1");
	instrumenter->Bind("HARDWARE.DSPI_1.reset_b"  , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DSPI_1.debug"    , "HARDWARE.debug");
	instrumenter->Bind("HARDWARE.DSPI_1.HT"       , "HARDWARE.pull_down");
	instrumenter->Bind("HARDWARE.DSPI_1.INT_TFUF" , "HARDWARE.DSPI_1_INT_TFUF");
	instrumenter->Bind("HARDWARE.DSPI_1.INT_RFOF" , "HARDWARE.DSPI_1_INT_RFOF");
	instrumenter->Bind("HARDWARE.DSPI_1.INT_TFIWF", "HARDWARE.DSPI_1_INT_TFIWF");
	instrumenter->Bind("HARDWARE.DSPI_1.INT_DDIF" , "HARDWARE.DSPI_1_INT_DDIF");
	instrumenter->Bind("HARDWARE.DSPI_1.INT_DPEF" , "HARDWARE.DSPI_1_INT_DPEF");
	instrumenter->Bind("HARDWARE.DSPI_1.INT_SPITCF","HARDWARE.DSPI_1_INT_SPITCF");
	instrumenter->Bind("HARDWARE.DSPI_1.INT_DSITCF","HARDWARE.DSPI_1_INT_DSITCF");
	instrumenter->BindArray(DSPI_1::NUM_DSI_INPUTS, "HARDWARE.DSPI_1.DSI_INPUT", 0, 1, "HARDWARE.pull_down", 0, 0);
	instrumenter->BindArray(DSPI_1::NUM_DSI_OUTPUTS, "HARDWARE.DSPI_1.DSI_OUTPUT", 0, 1, "HARDWARE.unused", 0, 0);
	instrumenter->Bind("HARDWARE.DSPI_1.DMA_DD"   , "HARDWARE.DSPI_1_DMA_DD");
	instrumenter->Bind("HARDWARE.DSPI_1.DMA_ACK_DD", "HARDWARE.DSPI_1_DMA_ACK_DD");
	
	instrumenter->Bind("HARDWARE.DSPI_2.m_clk"    , "HARDWARE.PBRIDGEB_CLK");
	instrumenter->Bind("HARDWARE.DSPI_2.dspi_clk" , "HARDWARE.DSPI_CLK1");
	instrumenter->Bind("HARDWARE.DSPI_2.reset_b"  , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DSPI_2.debug"    , "HARDWARE.debug");
	instrumenter->Bind("HARDWARE.DSPI_2.HT"       , "HARDWARE.pull_down");
	instrumenter->Bind("HARDWARE.DSPI_2.INT_TFUF" , "HARDWARE.DSPI_2_INT_TFUF");
	instrumenter->Bind("HARDWARE.DSPI_2.INT_RFOF" , "HARDWARE.DSPI_2_INT_RFOF");
	instrumenter->Bind("HARDWARE.DSPI_2.INT_TFIWF", "HARDWARE.DSPI_2_INT_TFIWF");
	instrumenter->Bind("HARDWARE.DSPI_2.INT_DDIF" , "HARDWARE.DSPI_2_INT_DDIF");
	instrumenter->Bind("HARDWARE.DSPI_2.INT_DPEF" , "HARDWARE.DSPI_2_INT_DPEF");
	instrumenter->Bind("HARDWARE.DSPI_2.INT_SPITCF", "HARDWARE.DSPI_2_INT_SPITCF");
	instrumenter->Bind("HARDWARE.DSPI_2.INT_DSITCF", "HARDWARE.DSPI_2_INT_DSITCF");
	instrumenter->BindArray(DSPI_2::NUM_DSI_INPUTS, "HARDWARE.DSPI_2.DSI_INPUT", 0, 1, "HARDWARE.pull_down", 0, 0);
	instrumenter->BindArray(DSPI_2::NUM_DSI_OUTPUTS, "HARDWARE.DSPI_2.DSI_OUTPUT", 0, 1, "HARDWARE.unused", 0, 0);
	instrumenter->Bind("HARDWARE.DSPI_2.DMA_DD"   , "HARDWARE.DSPI_2_DMA_DD");
	instrumenter->Bind("HARDWARE.DSPI_2.DMA_ACK_DD", "HARDWARE.DSPI_2_DMA_ACK_DD");
	
	instrumenter->Bind("HARDWARE.DSPI_3.m_clk"    , "HARDWARE.PBRIDGEB_CLK");
	instrumenter->Bind("HARDWARE.DSPI_3.dspi_clk" , "HARDWARE.DSPI_CLK1");
	instrumenter->Bind("HARDWARE.DSPI_3.reset_b"  , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DSPI_3.debug"    , "HARDWARE.debug");
	instrumenter->Bind("HARDWARE.DSPI_3.HT"       , "HARDWARE.pull_down");
	instrumenter->Bind("HARDWARE.DSPI_3.INT_TFUF" , "HARDWARE.DSPI_3_INT_TFUF");
	instrumenter->Bind("HARDWARE.DSPI_3.INT_RFOF" , "HARDWARE.DSPI_3_INT_RFOF");
	instrumenter->Bind("HARDWARE.DSPI_3.INT_TFIWF", "HARDWARE.DSPI_3_INT_TFIWF");
	instrumenter->Bind("HARDWARE.DSPI_3.INT_DDIF" , "HARDWARE.DSPI_3_INT_DDIF");
	instrumenter->Bind("HARDWARE.DSPI_3.INT_DPEF" , "HARDWARE.DSPI_3_INT_DPEF");
	instrumenter->Bind("HARDWARE.DSPI_3.INT_SPITCF", "HARDWARE.DSPI_3_INT_SPITCF");
	instrumenter->Bind("HARDWARE.DSPI_3.INT_DSITCF", "HARDWARE.DSPI_3_INT_DSITCF");
	instrumenter->BindArray(DSPI_3::NUM_DSI_INPUTS, "HARDWARE.DSPI_3.DSI_INPUT", 0, 1, "HARDWARE.pull_down", 0, 0);
	instrumenter->BindArray(DSPI_3::NUM_DSI_OUTPUTS, "HARDWARE.DSPI_3.DSI_OUTPUT", 0, 1, "HARDWARE.unused", 0, 0);
	instrumenter->Bind("HARDWARE.DSPI_3.DMA_DD"   , "HARDWARE.DSPI_3_DMA_DD");
	instrumenter->Bind("HARDWARE.DSPI_3.DMA_ACK_DD", "HARDWARE.DSPI_3_DMA_ACK_DD");
	
	instrumenter->Bind("HARDWARE.DSPI_4.m_clk"     , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.DSPI_4.dspi_clk"  , "HARDWARE.DSPI_CLK0");
	instrumenter->Bind("HARDWARE.DSPI_4.reset_b"   , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DSPI_4.debug"     , "HARDWARE.debug");
	instrumenter->Bind("HARDWARE.DSPI_4.HT"        , "HARDWARE.pull_down");
	instrumenter->Bind("HARDWARE.DSPI_4.INT_TFUF"  , "HARDWARE.DSPI_4_INT_TFUF");
	instrumenter->Bind("HARDWARE.DSPI_4.INT_RFOF"  , "HARDWARE.DSPI_4_INT_RFOF");
	instrumenter->Bind("HARDWARE.DSPI_4.INT_TFIWF" , "HARDWARE.DSPI_4_INT_TFIWF");
	instrumenter->Bind("HARDWARE.DSPI_4.INT_SPITCF", "HARDWARE.DSPI_4_INT_SPITCF");
	instrumenter->Bind("HARDWARE.DSPI_4.INT_DSITCF", "HARDWARE.DSPI_4_INT_DSITCF");
	instrumenter->Bind("HARDWARE.DSPI_4.INT_CMDTCF", "HARDWARE.DSPI_4_INT_CMDTCF");
	instrumenter->Bind("HARDWARE.DSPI_4.INT_CMDFFF", "HARDWARE.DSPI_4_INT_CMDFFF");
	instrumenter->Bind("HARDWARE.DSPI_4.INT_SPEF"  , "HARDWARE.DSPI_4_INT_SPEF");
	instrumenter->Bind("HARDWARE.DSPI_4.INT_DPEF"  , "HARDWARE.DSPI_4_INT_DPEF");
	instrumenter->BindArray(DSPI_4::NUM_DSI_INPUTS, "HARDWARE.DSPI_4.DSI_INPUT", 0, 1, "HARDWARE.pull_down", 0, 0);
	instrumenter->BindArray(DSPI_4::NUM_DSI_OUTPUTS, "HARDWARE.DSPI_4.DSI_OUTPUT", 0, 1, "HARDWARE.unused", 0, 0);
	instrumenter->Bind("HARDWARE.DSPI_4.DMA_DD"   , "HARDWARE.DSPI_4_DMA_DD");
	instrumenter->Bind("HARDWARE.DSPI_4.DMA_ACK_DD", "HARDWARE.DSPI_4_DMA_ACK_DD");
	
	instrumenter->Bind("HARDWARE.DSPI_5.m_clk"     , "HARDWARE.PBRIDGEB_CLK");
	instrumenter->Bind("HARDWARE.DSPI_5.dspi_clk"  , "HARDWARE.DSPI_CLK0");
	instrumenter->Bind("HARDWARE.DSPI_5.reset_b"   , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DSPI_5.debug"     , "HARDWARE.debug");
	instrumenter->Bind("HARDWARE.DSPI_5.HT"        , "HARDWARE.pull_down");
	instrumenter->Bind("HARDWARE.DSPI_5.INT_TFUF"  , "HARDWARE.DSPI_5_INT_TFUF");
	instrumenter->Bind("HARDWARE.DSPI_5.INT_RFOF"  , "HARDWARE.DSPI_5_INT_RFOF");
	instrumenter->Bind("HARDWARE.DSPI_5.INT_TFIWF" , "HARDWARE.DSPI_5_INT_TFIWF");
	instrumenter->Bind("HARDWARE.DSPI_5.INT_SPITCF", "HARDWARE.DSPI_5_INT_SPITCF");
	instrumenter->Bind("HARDWARE.DSPI_5.INT_DSITCF", "HARDWARE.DSPI_5_INT_DSITCF");
	instrumenter->Bind("HARDWARE.DSPI_5.INT_CMDTCF", "HARDWARE.DSPI_5_INT_CMDTCF");
	instrumenter->Bind("HARDWARE.DSPI_5.INT_CMDFFF", "HARDWARE.DSPI_5_INT_CMDFFF");
	instrumenter->Bind("HARDWARE.DSPI_5.INT_SPEF"  , "HARDWARE.DSPI_5_INT_SPEF");
	instrumenter->Bind("HARDWARE.DSPI_5.INT_DPEF"  , "HARDWARE.DSPI_5_INT_DPEF");
	instrumenter->BindArray(DSPI_5::NUM_DSI_INPUTS, "HARDWARE.DSPI_5.DSI_INPUT", 0, 1, "HARDWARE.pull_down", 0, 0);
	instrumenter->BindArray(DSPI_5::NUM_DSI_OUTPUTS, "HARDWARE.DSPI_5.DSI_OUTPUT", 0, 1, "HARDWARE.unused", 0, 0);
	instrumenter->Bind("HARDWARE.DSPI_5.DMA_DD"   , "HARDWARE.DSPI_5_DMA_DD");
	instrumenter->Bind("HARDWARE.DSPI_5.DMA_ACK_DD", "HARDWARE.DSPI_5_DMA_ACK_DD");
	
	instrumenter->Bind("HARDWARE.DSPI_6.m_clk"     , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.DSPI_6.dspi_clk"  , "HARDWARE.DSPI_CLK0");
	instrumenter->Bind("HARDWARE.DSPI_6.reset_b"   , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DSPI_6.debug"     , "HARDWARE.debug");
	instrumenter->Bind("HARDWARE.DSPI_6.HT"        , "HARDWARE.pull_down");
	instrumenter->Bind("HARDWARE.DSPI_6.INT_TFUF"  , "HARDWARE.DSPI_6_INT_TFUF");
	instrumenter->Bind("HARDWARE.DSPI_6.INT_RFOF"  , "HARDWARE.DSPI_6_INT_RFOF");
	instrumenter->Bind("HARDWARE.DSPI_6.INT_TFIWF" , "HARDWARE.DSPI_6_INT_TFIWF");
	instrumenter->Bind("HARDWARE.DSPI_6.INT_SPITCF", "HARDWARE.DSPI_6_INT_SPITCF");
	instrumenter->Bind("HARDWARE.DSPI_6.INT_DSITCF", "HARDWARE.DSPI_6_INT_DSITCF");
	instrumenter->Bind("HARDWARE.DSPI_6.INT_CMDTCF", "HARDWARE.DSPI_6_INT_CMDTCF");
	instrumenter->Bind("HARDWARE.DSPI_6.INT_CMDFFF", "HARDWARE.DSPI_6_INT_CMDFFF");
	instrumenter->Bind("HARDWARE.DSPI_6.INT_SPEF"  , "HARDWARE.DSPI_6_INT_SPEF");
	instrumenter->Bind("HARDWARE.DSPI_6.INT_DPEF"  , "HARDWARE.DSPI_6_INT_DPEF");
	instrumenter->BindArray(DSPI_6::NUM_DSI_INPUTS, "HARDWARE.DSPI_6.DSI_INPUT", 0, 1, "HARDWARE.pull_down", 0, 0);
	instrumenter->BindArray(DSPI_6::NUM_DSI_OUTPUTS, "HARDWARE.DSPI_6.DSI_OUTPUT", 0, 1, "HARDWARE.unused", 0, 0);
	instrumenter->Bind("HARDWARE.DSPI_6.DMA_DD"   , "HARDWARE.DSPI_6_DMA_DD");
	instrumenter->Bind("HARDWARE.DSPI_6.DMA_ACK_DD", "HARDWARE.DSPI_6_DMA_ACK_DD");
	
	instrumenter->Bind("HARDWARE.DSPI_12.m_clk"    , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.DSPI_12.dspi_clk" , "HARDWARE.DSPI_CLK1");
	instrumenter->Bind("HARDWARE.DSPI_12.reset_b"  , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.DSPI_12.debug"    , "HARDWARE.debug");
	instrumenter->Bind("HARDWARE.DSPI_12.HT"       , "HARDWARE.pull_down");
	instrumenter->Bind("HARDWARE.DSPI_12.INT_TFUF" , "HARDWARE.DSPI_12_INT_TFUF");
	instrumenter->Bind("HARDWARE.DSPI_12.INT_RFOF" , "HARDWARE.DSPI_12_INT_RFOF");
	instrumenter->Bind("HARDWARE.DSPI_12.INT_TFIWF", "HARDWARE.DSPI_12_INT_TFIWF");
	instrumenter->Bind("HARDWARE.DSPI_12.INT_DDIF" , "HARDWARE.DSPI_12_INT_DDIF");
	instrumenter->Bind("HARDWARE.DSPI_12.INT_DPEF" , "HARDWARE.DSPI_12_INT_DPEF");
	instrumenter->Bind("HARDWARE.DSPI_12.INT_SPITCF", "HARDWARE.DSPI_12_INT_SPITCF");
	instrumenter->Bind("HARDWARE.DSPI_12.INT_DSITCF", "HARDWARE.DSPI_12_INT_DSITCF");
	instrumenter->BindArray(DSPI_12::NUM_DSI_INPUTS, "HARDWARE.DSPI_12.DSI_INPUT", 0, 1, "HARDWARE.pull_down", 0, 0);
	instrumenter->BindArray(DSPI_12::NUM_DSI_OUTPUTS, "HARDWARE.DSPI_12.DSI_OUTPUT", 0, 1, "HARDWARE.unused", 0, 0);
	instrumenter->Bind("HARDWARE.DSPI_12.DMA_DD"   , "HARDWARE.DSPI_12_DMA_DD");
	instrumenter->Bind("HARDWARE.DSPI_12.DMA_ACK_DD", "HARDWARE.DSPI_12_DMA_ACK_DD");
	
	instrumenter->BindArray(EDMA_0::NUM_DMA_CHANNELS, "HARDWARE.eDMA_0.err_irq", 0, "HARDWARE.DMA_ERR_IRQ", 0);
	instrumenter->BindArray(EDMA_1::NUM_DMA_CHANNELS, "HARDWARE.eDMA_1.err_irq", 0, "HARDWARE.DMA_ERR_IRQ", 64);
	instrumenter->BindArray(NUM_DMA_CHANNELS, "HARDWARE.DMA_ERR_IRQ_COMBINATOR.in", "HARDWARE.DMA_ERR_IRQ");

	instrumenter->Bind("HARDWARE.DSPI0_0.in_0" , "HARDWARE.DSPI_0_INT_TFUF");
	instrumenter->Bind("HARDWARE.DSPI0_0.in_1" , "HARDWARE.DSPI_0_INT_RFOF");
	instrumenter->Bind("HARDWARE.DSPI0_0.in_2" , "HARDWARE.DSPI_0_INT_TFIWF");
	instrumenter->Bind("HARDWARE.DSPI1_0.in_0" , "HARDWARE.DSPI_1_INT_TFUF");
	instrumenter->Bind("HARDWARE.DSPI1_0.in_1" , "HARDWARE.DSPI_1_INT_RFOF");
	instrumenter->Bind("HARDWARE.DSPI1_0.in_2" , "HARDWARE.DSPI_1_INT_TFIWF");
	instrumenter->Bind("HARDWARE.DSPI2_0.in_0" , "HARDWARE.DSPI_2_INT_TFUF");
	instrumenter->Bind("HARDWARE.DSPI2_0.in_1" , "HARDWARE.DSPI_2_INT_RFOF");
	instrumenter->Bind("HARDWARE.DSPI2_0.in_2" , "HARDWARE.DSPI_2_INT_TFIWF");
	instrumenter->Bind("HARDWARE.DSPI3_0.in_0" , "HARDWARE.DSPI_3_INT_TFUF");
	instrumenter->Bind("HARDWARE.DSPI3_0.in_1" , "HARDWARE.DSPI_3_INT_RFOF");
	instrumenter->Bind("HARDWARE.DSPI3_0.in_2" , "HARDWARE.DSPI_3_INT_TFIWF");
	instrumenter->Bind("HARDWARE.DSPI4_0.in_0" , "HARDWARE.DSPI_4_INT_TFUF");
	instrumenter->Bind("HARDWARE.DSPI4_0.in_1" , "HARDWARE.DSPI_4_INT_RFOF");
	instrumenter->Bind("HARDWARE.DSPI4_0.in_2" , "HARDWARE.DSPI_4_INT_TFIWF");
	instrumenter->Bind("HARDWARE.DSPI4_5.in_0" , "HARDWARE.DSPI_4_INT_SPITCF");
	instrumenter->Bind("HARDWARE.DSPI4_5.in_1" , "HARDWARE.DSPI_4_INT_CMDTCF");
	instrumenter->Bind("HARDWARE.DSPI4_6.in_0" , "HARDWARE.DSPI_4_INT_DSITCF");
	instrumenter->Bind("HARDWARE.DSPI4_6.in_1" , "HARDWARE.DSPI_4_INT_CMDFFF");
	instrumenter->Bind("HARDWARE.DSPI4_7.in_0" , "HARDWARE.DSPI_4_INT_SPEF");
	instrumenter->Bind("HARDWARE.DSPI4_7.in_1" , "HARDWARE.DSPI_4_INT_DPEF");
	instrumenter->Bind("HARDWARE.DSPI5_0.in_0" , "HARDWARE.DSPI_5_INT_TFUF");
	instrumenter->Bind("HARDWARE.DSPI5_0.in_1" , "HARDWARE.DSPI_5_INT_RFOF");
	instrumenter->Bind("HARDWARE.DSPI5_0.in_2" , "HARDWARE.DSPI_5_INT_TFIWF");
	instrumenter->Bind("HARDWARE.DSPI5_5.in_0" , "HARDWARE.DSPI_5_INT_SPITCF");
	instrumenter->Bind("HARDWARE.DSPI5_5.in_1" , "HARDWARE.DSPI_5_INT_CMDTCF");
	instrumenter->Bind("HARDWARE.DSPI5_6.in_0" , "HARDWARE.DSPI_5_INT_DSITCF");
	instrumenter->Bind("HARDWARE.DSPI5_6.in_1" , "HARDWARE.DSPI_5_INT_CMDFFF");
	instrumenter->Bind("HARDWARE.DSPI5_7.in_0" , "HARDWARE.DSPI_5_INT_SPEF");
	instrumenter->Bind("HARDWARE.DSPI5_7.in_1" , "HARDWARE.DSPI_5_INT_DPEF");
	instrumenter->Bind("HARDWARE.DSPI6_0.in_0" , "HARDWARE.DSPI_6_INT_TFUF");
	instrumenter->Bind("HARDWARE.DSPI6_0.in_1" , "HARDWARE.DSPI_6_INT_RFOF");
	instrumenter->Bind("HARDWARE.DSPI6_0.in_2" , "HARDWARE.DSPI_6_INT_TFIWF");
	instrumenter->Bind("HARDWARE.DSPI6_5.in_0" , "HARDWARE.DSPI_6_INT_SPITCF");
	instrumenter->Bind("HARDWARE.DSPI6_5.in_1" , "HARDWARE.DSPI_6_INT_CMDTCF");
	instrumenter->Bind("HARDWARE.DSPI6_6.in_0" , "HARDWARE.DSPI_6_INT_DSITCF");
	instrumenter->Bind("HARDWARE.DSPI6_6.in_1" , "HARDWARE.DSPI_6_INT_CMDFFF");
	instrumenter->Bind("HARDWARE.DSPI6_7.in_0" , "HARDWARE.DSPI_6_INT_SPEF");
	instrumenter->Bind("HARDWARE.DSPI6_7.in_1" , "HARDWARE.DSPI_6_INT_DPEF");
	instrumenter->Bind("HARDWARE.DSPI12_0.in_0", "HARDWARE.DSPI_12_INT_TFUF");
	instrumenter->Bind("HARDWARE.DSPI12_0.in_1", "HARDWARE.DSPI_12_INT_RFOF");
	instrumenter->Bind("HARDWARE.DSPI12_0.in_2", "HARDWARE.DSPI_12_INT_TFIWF");
	
	instrumenter->Bind("HARDWARE.SIUL2.m_clk"   , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.SIUL2.reset_b" , "HARDWARE.reset_b");
	instrumenter->BindArray(SIUL2::NUM_PADS, "HARDWARE.SIUL2.pad_in", "HARDWARE.pad");
	instrumenter->BindArray(SIUL2::NUM_PADS, "HARDWARE.SIUL2.pad_out", "HARDWARE.pad");

	instrumenter->Bind("HARDWARE.M_CAN_1.m_clk"   , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.M_CAN_1.can_clk" , "HARDWARE.CAN_CLK");
	instrumenter->Bind("HARDWARE.M_CAN_1.reset_b" , "HARDWARE.reset_b");
	instrumenter->BindArray(M_CAN_1::NUM_FILTER_EVENTS, "HARDWARE.M_CAN_1.FE", "HARDWARE.M_CAN_1_FE");
	
	instrumenter->Bind("HARDWARE.M_CAN_2.m_clk"   , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.M_CAN_2.can_clk" , "HARDWARE.CAN_CLK");
	instrumenter->Bind("HARDWARE.M_CAN_2.reset_b" , "HARDWARE.reset_b");
	instrumenter->BindArray(M_CAN_2::NUM_FILTER_EVENTS, "HARDWARE.M_CAN_2.FE", "HARDWARE.M_CAN_2_FE");
	
	instrumenter->Bind("HARDWARE.M_CAN_3.m_clk"   , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.M_CAN_3.can_clk" , "HARDWARE.CAN_CLK");
	instrumenter->Bind("HARDWARE.M_CAN_3.reset_b" , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.M_CAN_3.DMA_REQ" , "HARDWARE.M_CAN_3_DMA_REQ");
	instrumenter->Bind("HARDWARE.M_CAN_3.DMA_ACK" , "HARDWARE.M_CAN_3_DMA_ACK");
	instrumenter->BindArray(M_CAN_3::NUM_FILTER_EVENTS, "HARDWARE.M_CAN_3.FE", "HARDWARE.M_CAN_3_FE");
	
	instrumenter->Bind("HARDWARE.M_CAN_4.m_clk"   , "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.M_CAN_4.can_clk" , "HARDWARE.CAN_CLK");
	instrumenter->Bind("HARDWARE.M_CAN_4.reset_b" , "HARDWARE.reset_b");
	instrumenter->Bind("HARDWARE.M_CAN_4.DMA_REQ" , "HARDWARE.M_CAN_4_DMA_REQ");
	instrumenter->Bind("HARDWARE.M_CAN_4.DMA_ACK" , "HARDWARE.M_CAN_4_DMA_ACK");
	instrumenter->BindArray(M_CAN_4::NUM_FILTER_EVENTS, "HARDWARE.M_CAN_4.FE", "HARDWARE.M_CAN_4_FE");
	
	instrumenter->Bind("HARDWARE.SHARED_CAN_MESSAGE_RAM_ROUTER.input_if_clock", "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.SHARED_CAN_MESSAGE_RAM_ROUTER.output_if_clock", "HARDWARE.PBRIDGEA_CLK");
	
	instrumenter->Bind("HARDWARE.SEMA4.m_clk", "HARDWARE.PBRIDGEA_CLK");
	instrumenter->Bind("HARDWARE.SEMA4.reset_b", "HARDWARE.reset_b");

	// Interrupt sources
	
	// IRQ # ---- Source name ------------ Description ------------------------- Note --------------
	//   0     INTC SSCIR0[CLR]     software settable flag 0            internally routed in INTC
	//  ..           ..                       ..                                   ..
	//  31    INTC SSCIR31[CLR]    software settable flag 31            internally routed in INTC
	
	//  32      SWT_0 IR[TIF]
	//  33      SWT_1 IR[TIF]
	//  34      SWT_2 IR[TIF]
	//  35      SWT_3 IR[TIF]
	
	InterruptSource(32, "HARDWARE.SWT_0.irq");
	InterruptSource(33, "HARDWARE.SWT_1.irq");
	InterruptSource(34, "HARDWARE.SWT_2.irq");
	InterruptSource(35, "HARDWARE.SWT_3.irq");

	//  36      STM_0 CIR0[CIF]     platform period timer 0_0
	//  37      STM_0 CIR1[CIF]     platform period timer 0_1
	//  38      STM_0 CIR2[CIF]     platform period timer 0_2
	//  39      STM_0 CIR3[CIF]     platform period timer 0_3
	//  40      STM_1 CIR0[CIF]     platform period timer 1_0
	//  41      STM_1 CIR1[CIF]     platform period timer 1_1
	//  42      STM_1 CIR2[CIF]     platform period timer 1_2
	//  43      STM_1 CIR3[CIF]     platform period timer 1_3
	//  44      STM_2 CIR0[CIF]     platform period timer 2_0
	//  45      STM_2 CIR1[CIF]     platform period timer 2_1
	//  46      STM_2 CIR2[CIF]     platform period timer 2_2
	//  47      STM_2 CIR3[CIF]     platform period timer 2_3
	
	InterruptSource(36, "HARDWARE.STM_0.irq_0");
	InterruptSource(37, "HARDWARE.STM_0.irq_1");
	InterruptSource(38, "HARDWARE.STM_0.irq_2");
	InterruptSource(39, "HARDWARE.STM_0.irq_3");
	InterruptSource(40, "HARDWARE.STM_1.irq_0");
	InterruptSource(41, "HARDWARE.STM_1.irq_1");
	InterruptSource(42, "HARDWARE.STM_1.irq_2");
	InterruptSource(43, "HARDWARE.STM_1.irq_3");
	InterruptSource(44, "HARDWARE.STM_2.irq_0");
	InterruptSource(45, "HARDWARE.STM_2.irq_1");
	InterruptSource(46, "HARDWARE.STM_2.irq_2");
	InterruptSource(47, "HARDWARE.STM_2.irq_3");

	// 48           TODO
	// ..           TODO
	// 52           TODO

	InterruptSource(48);
	InterruptSource(49);
	InterruptSource(50);
	InterruptSource(51);
	
	// 52       eDMA ERR            eDMA combined error 127-0
	InterruptSource(52, "HARDWARE.DMA_ERR_IRQ_COMBINATOR.out");
	
	// 53       eDMA_0 INT[0]       eDMA Channel 0
	// 54       eDMA_0 INT[1]       eDMA Channel 1
	// 55       eDMA_0 INT[2]       eDMA Channel 2
	// 56       eDMA_0 INT[3]       eDMA Channel 3
	// 57       eDMA_0 INT[4]       eDMA Channel 4
	// 58       eDMA_0 INT[5]       eDMA Channel 5
	// 59       eDMA_0 INT[6]       eDMA Channel 6
	// 60       eDMA_0 INT[7]       eDMA Channel 7
	// 61       eDMA_0 INT[8]       eDMA Channel 8
	// 62       eDMA_0 INT[9]       eDMA Channel 9
	// 63       eDMA_0 INT[10]      eDMA Channel 10
	// 64       eDMA_0 INT[11]      eDMA Channel 11
	// 65       eDMA_0 INT[12]      eDMA Channel 12
	// 66       eDMA_0 INT[13]      eDMA Channel 13
	// 67       eDMA_0 INT[14]      eDMA Channel 14
	// 68       eDMA_0 INT[15]      eDMA Channel 15
	// 69       eDMA_0 INT[16]      eDMA Channel 16
	// 70       eDMA_0 INT[17]      eDMA Channel 17
	// 71       eDMA_0 INT[18]      eDMA Channel 18
	// 72       eDMA_0 INT[19]      eDMA Channel 19
	// 73       eDMA_0 INT[20]      eDMA Channel 20
	// 74       eDMA_0 INT[21]      eDMA Channel 21
	// 75       eDMA_0 INT[22]      eDMA Channel 22
	// 76       eDMA_0 INT[23]      eDMA Channel 23
	// 77       eDMA_0 INT[24]      eDMA Channel 24
	// 78       eDMA_0 INT[25]      eDMA Channel 25
	// 79       eDMA_0 INT[26]      eDMA Channel 26
	// 80       eDMA_0 INT[27]      eDMA Channel 27
	// 81       eDMA_0 INT[28]      eDMA Channel 28
	// 82       eDMA_0 INT[29]      eDMA Channel 29
	// 83       eDMA_0 INT[30]      eDMA Channel 30
	// 84       eDMA_0 INT[31]      eDMA Channel 31
	// 85       eDMA_0 INT[32]      eDMA Channel 32
	// 86       eDMA_0 INT[33]      eDMA Channel 33
	// 87       eDMA_0 INT[34]      eDMA Channel 34
	// 88       eDMA_0 INT[35]      eDMA Channel 35
	// 89       eDMA_0 INT[36]      eDMA Channel 36
	// 90       eDMA_0 INT[37]      eDMA Channel 37
	// 91       eDMA_0 INT[38]      eDMA Channel 38
	// 92       eDMA_0 INT[39]      eDMA Channel 39
	// 93       eDMA_0 INT[40]      eDMA Channel 40
	// 94       eDMA_0 INT[41]      eDMA Channel 41
	// 95       eDMA_0 INT[42]      eDMA Channel 42
	// 96       eDMA_0 INT[43]      eDMA Channel 43
	// 97       eDMA_0 INT[44]      eDMA Channel 44
	// 98       eDMA_0 INT[45]      eDMA Channel 45
	// 99       eDMA_0 INT[46]      eDMA Channel 46
	// 100       eDMA_0 INT[47]     eDMA Channel 47
	// 101       eDMA_0 INT[48]     eDMA Channel 48
	// 102       eDMA_0 INT[49]     eDMA Channel 49
	// 103       eDMA_0 INT[50]     eDMA Channel 50
	// 104       eDMA_0 INT[51]     eDMA Channel 51
	// 105       eDMA_0 INT[52]     eDMA Channel 52
	// 106       eDMA_0 INT[53]     eDMA Channel 53
	// 107       eDMA_0 INT[54]     eDMA Channel 54
	// 108       eDMA_0 INT[55]     eDMA Channel 55
	// 109       eDMA_0 INT[56]     eDMA Channel 56
	// 110       eDMA_0 INT[57]     eDMA Channel 57
	// 111       eDMA_0 INT[58]     eDMA Channel 58
	// 112       eDMA_0 INT[59]     eDMA Channel 59
	// 113       eDMA_0 INT[60]     eDMA Channel 60
	// 114       eDMA_0 INT[61]     eDMA Channel 61
	// 115       eDMA_0 INT[62]     eDMA Channel 62
	// 116       eDMA_0 INT[63]     eDMA Channel 63
	// 117       eDMA_1 INT[0]      eDMA Channel 64
	// 118       eDMA_1 INT[1]      eDMA Channel 65
	// 119       eDMA_1 INT[2]      eDMA Channel 66
	// 120       eDMA_1 INT[3]      eDMA Channel 67
	// 121       eDMA_1 INT[4]      eDMA Channel 68
	// 122       eDMA_1 INT[5]      eDMA Channel 69
	// 123       eDMA_1 INT[6]      eDMA Channel 70
	// 124       eDMA_1 INT[7]      eDMA Channel 71
	// 125       eDMA_1 INT[8]      eDMA Channel 72
	// 126       eDMA_1 INT[9]      eDMA Channel 73
	// 127       eDMA_1 INT[10]     eDMA Channel 74
	// 128       eDMA_1 INT[11]     eDMA Channel 75
	// 129       eDMA_1 INT[12]     eDMA Channel 76
	// 130       eDMA_1 INT[13]     eDMA Channel 77
	// 131       eDMA_1 INT[14]     eDMA Channel 78
	// 132       eDMA_1 INT[15]     eDMA Channel 79
	// 133       eDMA_1 INT[16]     eDMA Channel 80
	// 134       eDMA_1 INT[17]     eDMA Channel 81
	// 135       eDMA_1 INT[18]     eDMA Channel 82
	// 136       eDMA_1 INT[19]     eDMA Channel 83
	// 137       eDMA_1 INT[20]     eDMA Channel 84
	// 138       eDMA_1 INT[21]     eDMA Channel 85
	// 139       eDMA_1 INT[22]     eDMA Channel 86
	// 140       eDMA_1 INT[23]     eDMA Channel 87
	// 141       eDMA_1 INT[24]     eDMA Channel 88
	// 142       eDMA_1 INT[25]     eDMA Channel 89
	// 143       eDMA_1 INT[26]     eDMA Channel 90
	// 144       eDMA_1 INT[27]     eDMA Channel 91
	// 145       eDMA_1 INT[28]     eDMA Channel 92
	// 146       eDMA_1 INT[29]     eDMA Channel 93
	// 147       eDMA_1 INT[30]     eDMA Channel 94
	// 148       eDMA_1 INT[31]     eDMA Channel 95
	// 149       eDMA_1 INT[32]     eDMA Channel 96
	// 150       eDMA_1 INT[33]     eDMA Channel 97
	// 151       eDMA_1 INT[34]     eDMA Channel 98
	// 152       eDMA_1 INT[35]     eDMA Channel 99
	// 153       eDMA_1 INT[36]     eDMA Channel 100
	// 154       eDMA_1 INT[37]     eDMA Channel 101
	// 155       eDMA_1 INT[38]     eDMA Channel 102
	// 156       eDMA_1 INT[39]     eDMA Channel 103
	// 157       eDMA_1 INT[40]     eDMA Channel 104
	// 158       eDMA_1 INT[41]     eDMA Channel 105
	// 159       eDMA_1 INT[42]     eDMA Channel 106
	// 160       eDMA_1 INT[43]     eDMA Channel 107
	// 161       eDMA_1 INT[44]     eDMA Channel 108
	// 162       eDMA_1 INT[45]     eDMA Channel 109
	// 163       eDMA_1 INT[46]     eDMA Channel 110
	// 164       eDMA_1 INT[47]     eDMA Channel 111
	// 165       eDMA_1 INT[48]     eDMA Channel 112
	// 166       eDMA_1 INT[49]     eDMA Channel 113
	// 167       eDMA_1 INT[50]     eDMA Channel 114
	// 168       eDMA_1 INT[51]     eDMA Channel 115
	// 169       eDMA_1 INT[52]     eDMA Channel 116
	// 170       eDMA_1 INT[53]     eDMA Channel 117
	// 171       eDMA_1 INT[54]     eDMA Channel 118
	// 172       eDMA_1 INT[55]     eDMA Channel 119
	// 173       eDMA_1 INT[56]     eDMA Channel 120
	// 174       eDMA_1 INT[57]     eDMA Channel 121
	// 175       eDMA_1 INT[58]     eDMA Channel 122
	// 176       eDMA_1 INT[59]     eDMA Channel 123
	// 177       eDMA_1 INT[60]     eDMA Channel 124
	// 178       eDMA_1 INT[61]     eDMA Channel 125
	// 179       eDMA_1 INT[62]     eDMA Channel 126
	// 180       eDMA_1 INT[63]     eDMA Channel 127

	InterruptSource(53, "HARDWARE.eDMA_0.irq_0");
	InterruptSource(54, "HARDWARE.eDMA_0.irq_1");
	InterruptSource(55, "HARDWARE.eDMA_0.irq_2");
	InterruptSource(56, "HARDWARE.eDMA_0.irq_3");
	InterruptSource(57, "HARDWARE.eDMA_0.irq_4");
	InterruptSource(58, "HARDWARE.eDMA_0.irq_5");
	InterruptSource(59, "HARDWARE.eDMA_0.irq_6");
	InterruptSource(60, "HARDWARE.eDMA_0.irq_7");
	InterruptSource(61, "HARDWARE.eDMA_0.irq_8");
	InterruptSource(62, "HARDWARE.eDMA_0.irq_9");
	InterruptSource(63, "HARDWARE.eDMA_0.irq_10");
	InterruptSource(64, "HARDWARE.eDMA_0.irq_11");
	InterruptSource(65, "HARDWARE.eDMA_0.irq_12");
	InterruptSource(66, "HARDWARE.eDMA_0.irq_13");
	InterruptSource(67, "HARDWARE.eDMA_0.irq_14");
	InterruptSource(68, "HARDWARE.eDMA_0.irq_15");
	InterruptSource(69, "HARDWARE.eDMA_0.irq_16");
	InterruptSource(70, "HARDWARE.eDMA_0.irq_17");
	InterruptSource(71, "HARDWARE.eDMA_0.irq_18");
	InterruptSource(72, "HARDWARE.eDMA_0.irq_19");
	InterruptSource(73, "HARDWARE.eDMA_0.irq_20");
	InterruptSource(74, "HARDWARE.eDMA_0.irq_21");
	InterruptSource(75, "HARDWARE.eDMA_0.irq_22");
	InterruptSource(76, "HARDWARE.eDMA_0.irq_23");
	InterruptSource(77, "HARDWARE.eDMA_0.irq_24");
	InterruptSource(78, "HARDWARE.eDMA_0.irq_25");
	InterruptSource(79, "HARDWARE.eDMA_0.irq_26");
	InterruptSource(80, "HARDWARE.eDMA_0.irq_27");
	InterruptSource(81, "HARDWARE.eDMA_0.irq_28");
	InterruptSource(82, "HARDWARE.eDMA_0.irq_29");
	InterruptSource(83, "HARDWARE.eDMA_0.irq_30");
	InterruptSource(84, "HARDWARE.eDMA_0.irq_31");
	InterruptSource(85, "HARDWARE.eDMA_0.irq_32");
	InterruptSource(86, "HARDWARE.eDMA_0.irq_33");
	InterruptSource(87, "HARDWARE.eDMA_0.irq_34");
	InterruptSource(88, "HARDWARE.eDMA_0.irq_35");
	InterruptSource(89, "HARDWARE.eDMA_0.irq_36");
	InterruptSource(90, "HARDWARE.eDMA_0.irq_37");
	InterruptSource(91, "HARDWARE.eDMA_0.irq_38");
	InterruptSource(92, "HARDWARE.eDMA_0.irq_39");
	InterruptSource(93, "HARDWARE.eDMA_0.irq_40");
	InterruptSource(94, "HARDWARE.eDMA_0.irq_41");
	InterruptSource(95, "HARDWARE.eDMA_0.irq_42");
	InterruptSource(96, "HARDWARE.eDMA_0.irq_43");
	InterruptSource(97, "HARDWARE.eDMA_0.irq_44");
	InterruptSource(98, "HARDWARE.eDMA_0.irq_45");
	InterruptSource(99, "HARDWARE.eDMA_0.irq_46");
	InterruptSource(100, "HARDWARE.eDMA_0.irq_47");
	InterruptSource(101, "HARDWARE.eDMA_0.irq_48");
	InterruptSource(102, "HARDWARE.eDMA_0.irq_49");
	InterruptSource(103, "HARDWARE.eDMA_0.irq_50");
	InterruptSource(104, "HARDWARE.eDMA_0.irq_51");
	InterruptSource(105, "HARDWARE.eDMA_0.irq_52");
	InterruptSource(106, "HARDWARE.eDMA_0.irq_53");
	InterruptSource(107, "HARDWARE.eDMA_0.irq_54");
	InterruptSource(108, "HARDWARE.eDMA_0.irq_55");
	InterruptSource(109, "HARDWARE.eDMA_0.irq_56");
	InterruptSource(110, "HARDWARE.eDMA_0.irq_57");
	InterruptSource(111, "HARDWARE.eDMA_0.irq_58");
	InterruptSource(112, "HARDWARE.eDMA_0.irq_59");
	InterruptSource(113, "HARDWARE.eDMA_0.irq_60");
	InterruptSource(114, "HARDWARE.eDMA_0.irq_61");
	InterruptSource(115, "HARDWARE.eDMA_0.irq_62");
	InterruptSource(116, "HARDWARE.eDMA_0.irq_63");
	InterruptSource(117, "HARDWARE.eDMA_1.irq_0");                                                                                                                                                       
	InterruptSource(118, "HARDWARE.eDMA_1.irq_1");                                                                                                                                                       
	InterruptSource(119, "HARDWARE.eDMA_1.irq_2");                                                                                                                                                       
	InterruptSource(120, "HARDWARE.eDMA_1.irq_3");                                                                                                                                                       
	InterruptSource(121, "HARDWARE.eDMA_1.irq_4");                                                                                                                                                       
	InterruptSource(122, "HARDWARE.eDMA_1.irq_5");                                                                                                                                                       
	InterruptSource(123, "HARDWARE.eDMA_1.irq_6");                                                                                                                                                       
	InterruptSource(124, "HARDWARE.eDMA_1.irq_7");                                                                                                                                                       
	InterruptSource(125, "HARDWARE.eDMA_1.irq_8");                                                                                                                                                       
	InterruptSource(126, "HARDWARE.eDMA_1.irq_9");                                                                                                                                                       
	InterruptSource(127, "HARDWARE.eDMA_1.irq_10");                                                                                                                                                      
	InterruptSource(128, "HARDWARE.eDMA_1.irq_11");                                                                                                                                                      
	InterruptSource(129, "HARDWARE.eDMA_1.irq_12");                                                                                                                                                      
	InterruptSource(130, "HARDWARE.eDMA_1.irq_13");                                                                                                                                                      
	InterruptSource(131, "HARDWARE.eDMA_1.irq_14");                                                                                                                                                      
	InterruptSource(132, "HARDWARE.eDMA_1.irq_15");                                                                                                                                                      
	InterruptSource(133, "HARDWARE.eDMA_1.irq_16");                                                                                                                                                      
	InterruptSource(134, "HARDWARE.eDMA_1.irq_17");
	InterruptSource(135, "HARDWARE.eDMA_1.irq_18");
	InterruptSource(136, "HARDWARE.eDMA_1.irq_19");
	InterruptSource(137, "HARDWARE.eDMA_1.irq_20");
	InterruptSource(138, "HARDWARE.eDMA_1.irq_21");
	InterruptSource(139, "HARDWARE.eDMA_1.irq_22");
	InterruptSource(140, "HARDWARE.eDMA_1.irq_23");
	InterruptSource(141, "HARDWARE.eDMA_1.irq_24");
	InterruptSource(142, "HARDWARE.eDMA_1.irq_25");
	InterruptSource(143, "HARDWARE.eDMA_1.irq_26");
	InterruptSource(144, "HARDWARE.eDMA_1.irq_27");
	InterruptSource(145, "HARDWARE.eDMA_1.irq_28");
	InterruptSource(146, "HARDWARE.eDMA_1.irq_29");
	InterruptSource(147, "HARDWARE.eDMA_1.irq_30");
	InterruptSource(148, "HARDWARE.eDMA_1.irq_31");
	InterruptSource(149, "HARDWARE.eDMA_1.irq_32");
	InterruptSource(150, "HARDWARE.eDMA_1.irq_33");
	InterruptSource(151, "HARDWARE.eDMA_1.irq_34");
	InterruptSource(152, "HARDWARE.eDMA_1.irq_35");
	InterruptSource(153, "HARDWARE.eDMA_1.irq_36");
	InterruptSource(154, "HARDWARE.eDMA_1.irq_37");
	InterruptSource(155, "HARDWARE.eDMA_1.irq_38");
	InterruptSource(156, "HARDWARE.eDMA_1.irq_39");
	InterruptSource(157, "HARDWARE.eDMA_1.irq_40");
	InterruptSource(158, "HARDWARE.eDMA_1.irq_41");
	InterruptSource(159, "HARDWARE.eDMA_1.irq_42");
	InterruptSource(160, "HARDWARE.eDMA_1.irq_43");
	InterruptSource(161, "HARDWARE.eDMA_1.irq_44");
	InterruptSource(162, "HARDWARE.eDMA_1.irq_45");
	InterruptSource(163, "HARDWARE.eDMA_1.irq_46");
	InterruptSource(164, "HARDWARE.eDMA_1.irq_47");
	InterruptSource(165, "HARDWARE.eDMA_1.irq_48");
	InterruptSource(166, "HARDWARE.eDMA_1.irq_49");
	InterruptSource(167, "HARDWARE.eDMA_1.irq_50");
	InterruptSource(168, "HARDWARE.eDMA_1.irq_51");
	InterruptSource(169, "HARDWARE.eDMA_1.irq_52");
	InterruptSource(170, "HARDWARE.eDMA_1.irq_53");
	InterruptSource(171, "HARDWARE.eDMA_1.irq_54");
	InterruptSource(172, "HARDWARE.eDMA_1.irq_55");
	InterruptSource(173, "HARDWARE.eDMA_1.irq_56");
	InterruptSource(174, "HARDWARE.eDMA_1.irq_57");
	InterruptSource(175, "HARDWARE.eDMA_1.irq_58");
	InterruptSource(176, "HARDWARE.eDMA_1.irq_59");
	InterruptSource(177, "HARDWARE.eDMA_1.irq_60");
	InterruptSource(178, "HARDWARE.eDMA_1.irq_61");
	InterruptSource(179, "HARDWARE.eDMA_1.irq_62");
	InterruptSource(180, "HARDWARE.eDMA_1.irq_63");
	
	// 181          TODO
	// ..           TODO
	// 964          TODO

	InterruptSource(181);
	InterruptSource(182);
	InterruptSource(183);
	InterruptSource(184);
	InterruptSource(185);
	InterruptSource(186);
	InterruptSource(187);
	InterruptSource(188);
	InterruptSource(189);
	InterruptSource(190);
	InterruptSource(191);
	InterruptSource(192);
	InterruptSource(193);
	InterruptSource(194);
	InterruptSource(195);
	InterruptSource(196);
	InterruptSource(197);
	InterruptSource(198);
	InterruptSource(199);
	InterruptSource(200);
	InterruptSource(201);
	InterruptSource(202);
	InterruptSource(203);
	InterruptSource(204);
	InterruptSource(205);
	InterruptSource(206);
	InterruptSource(207);
	InterruptSource(208);
	InterruptSource(209);
	InterruptSource(210);
	InterruptSource(211);
	InterruptSource(212);
	InterruptSource(213);
	InterruptSource(214);
	InterruptSource(215);
	InterruptSource(216);
	InterruptSource(217);
	InterruptSource(218);
	InterruptSource(219);
	InterruptSource(220);
	InterruptSource(221);
	InterruptSource(222);
	InterruptSource(223);
	InterruptSource(224);
	InterruptSource(225);
	
	// 226      PIT_0_TFLG0[TIF]    periodic interrupt timer 0_0
	// 227      PIT_0_TFLG1[TIF]    periodic interrupt timer 0_1
	// 228      PIT_0_TFLG2[TIF]    periodic interrupt timer 0_2
	// 229      PIT_0_TFLG3[TIF]    periodic interrupt timer 0_3
	// 230      PIT_0_TFLG4[TIF]    periodic interrupt timer 0_4
	// 231      PIT_0_TFLG5[TIF]    periodic interrupt timer 0_5
	// 232      PIT_0_TFLG6[TIF]    periodic interrupt timer 0_6
	// 233      PIT_0_TFLG7[TIF]    periodic interrupt timer 0_7
	
	InterruptSource(226, "HARDWARE.PIT_0.irq_0");
	InterruptSource(227, "HARDWARE.PIT_0.irq_1");
	InterruptSource(228, "HARDWARE.PIT_0.irq_2");
	InterruptSource(229, "HARDWARE.PIT_0.irq_3");
	InterruptSource(230, "HARDWARE.PIT_0.irq_4");
	InterruptSource(231, "HARDWARE.PIT_0.irq_5");
	InterruptSource(232, "HARDWARE.PIT_0.irq_6");
	InterruptSource(233, "HARDWARE.PIT_0.irq_7");

	// 234          UNUSED
	//  ..          UNUSED
	// 238          UNUSED
	
	InterruptSource(234);
	InterruptSource(235);
	InterruptSource(236);
	InterruptSource(237);
	InterruptSource(238);
	
	// 239   PIT_0_RTI_TFLG[TIF]    periodic interrupt timer RTI

	InterruptSource(239, "HARDWARE.PIT_0.rtirq");
	
	// 240      PIT_1_TFLG0[TIF]    periodic interrupt timer 1_0
	// 241      PIT_1_TFLG1[TIF]    periodic interrupt timer 1_1
	
	InterruptSource(240, "HARDWARE.PIT_1.irq_0");
	InterruptSource(241, "HARDWARE.PIT_1.irq_1");

	// 242          TODO
	//  ..          TODO	
	// 258          TODO

	InterruptSource(242);
	InterruptSource(243);
	InterruptSource(244);
	InterruptSource(245);
	InterruptSource(246);
	InterruptSource(247);
	InterruptSource(248);
	InterruptSource(249);
	InterruptSource(250);
	InterruptSource(251);
	InterruptSource(252);
	InterruptSource(253);
	InterruptSource(254);
	InterruptSource(255);
	InterruptSource(256);
	InterruptSource(257);
	InterruptSource(258);
	
	// 259       DSPI_0_SR[TFUF] | DSPI_0_SR[RFOF] | DSPI_0_SR[TFIWF]
	// 260       DSPI_0_SR[EOQF]
	// 261       DSPI_0_SR[TFFF]
	// 262       DSPI_0_SR[TCF]
	// 263       DDSPI_0_SR[RFDF]
	// 264       DSPI_0_SR[CMDTCF]
	// 265       DSPI_0_SR[CMDFFF]
	// 266       DSPI_0_SR[SPEF]

	InterruptSource(259, "HARDWARE.DSPI0_0.out");
	InterruptSource(260, "HARDWARE.DSPI_0.INT_EOQF");
	InterruptSource(261, "HARDWARE.DSPI_0.INT_TFFF");
	InterruptSource(262, "HARDWARE.DSPI_0.INT_TCF");
	InterruptSource(263, "HARDWARE.DSPI_0.INT_RFDF");
	InterruptSource(264, "HARDWARE.DSPI_0.INT_CMDTCF");
	InterruptSource(265, "HARDWARE.DSPI_0.INT_CMDFFF");
	InterruptSource(266, "HARDWARE.DSPI_0.INT_SPEF");
	
	// 267       unused
	InterruptSource(267);
	
	// 268       DSPI_1_SR[TFUF] | DSPI_1_SR[RFOF] | DSPI_1_SR[TFIWF]
	// 269       DSPI_1_SR[EOQF]
	// 270       DSPI_1_SR[TFFF]
	// 271       DSPI_1_SR[TCF]
	// 272       DDSPI_1_SR[RFDF]
	// 273       DSPI_1_SR[CMDTCF]
	// 274       DSPI_1_SR[CMDFFF]
	// 275       DSPI_1_SR[SPEF]

	InterruptSource(268, "HARDWARE.DSPI1_0.out");
	InterruptSource(269, "HARDWARE.DSPI_1.INT_EOQF");
	InterruptSource(270, "HARDWARE.DSPI_1.INT_TFFF");
	InterruptSource(271, "HARDWARE.DSPI_1.INT_TCF");
	InterruptSource(272, "HARDWARE.DSPI_1.INT_RFDF");
	InterruptSource(273, "HARDWARE.DSPI_1.INT_CMDTCF");
	InterruptSource(274, "HARDWARE.DSPI_1.INT_CMDFFF");
	InterruptSource(275, "HARDWARE.DSPI_1.INT_SPEF");

	// 276       unused
	InterruptSource(276);
	
	// 277       DSPI_2_SR[TFUF] | DSPI_2_SR[RFOF] | DSPI_2_SR[TFIWF]
	// 278       DSPI_2_SR[EOQF]
	// 279       DSPI_2_SR[TFFF]
	// 280       DSPI_2_SR[TCF]
	// 281       DDSPI_2_SR[RFDF]
	// 282       DSPI_2_SR[CMDTCF]
	// 283       DSPI_2_SR[CMDFFF]
	// 284       DSPI_2_SR[SPEF]

	InterruptSource(277, "HARDWARE.DSPI2_0.out");
	InterruptSource(278, "HARDWARE.DSPI_2.INT_EOQF");
	InterruptSource(279, "HARDWARE.DSPI_2.INT_TFFF");
	InterruptSource(280, "HARDWARE.DSPI_2.INT_TCF");
	InterruptSource(281, "HARDWARE.DSPI_2.INT_RFDF");
	InterruptSource(282, "HARDWARE.DSPI_2.INT_CMDTCF");
	InterruptSource(283, "HARDWARE.DSPI_2.INT_CMDFFF");
	InterruptSource(284, "HARDWARE.DSPI_2.INT_SPEF");
	
	// 285       unused
	
	InterruptSource(285);
	
	// 286       DSPI_3_SR[TFUF] | DSPI_3_SR[RFOF] | DSPI_3_SR[TFIWF]
	// 287       DSPI_3_SR[EOQF]
	// 288       DSPI_3_SR[TFFF]
	// 289       DSPI_3_SR[TCF]
	// 290       DDSPI_3_SR[RFDF]
	// 291       DSPI_3_SR[CMDTCF]
	// 292       DSPI_3_SR[CMDFFF]
	// 293       DSPI_3_SR[SPEF]

	InterruptSource(286, "HARDWARE.DSPI3_0.out");
	InterruptSource(287, "HARDWARE.DSPI_3.INT_EOQF");
	InterruptSource(288, "HARDWARE.DSPI_3.INT_TFFF");
	InterruptSource(289, "HARDWARE.DSPI_3.INT_TCF");
	InterruptSource(290, "HARDWARE.DSPI_3.INT_RFDF");
	InterruptSource(291, "HARDWARE.DSPI_3.INT_CMDTCF");
	InterruptSource(292, "HARDWARE.DSPI_3.INT_CMDFFF");
	InterruptSource(293, "HARDWARE.DSPI_3.INT_SPEF");

	// 294       unused
	
	InterruptSource(294);
	
	// 295       DSPI_4_SR[TFUF] | DSPI_4_SR[RFOF] | DSPI_4_SR[TFIWF]
	// 296       DSPI_4_SR[EOQF]
	// 297       DSPI_4_SR[TFFF]
	// 298       DSPI_4_SR[TCF]
	// 299       DSPI_4_SR[RFDF]
	// 300       DSPI_4_SR[SPITCF] | DSPI_4_SR[CMDTCF]
	// 301       DSPI_4_SR[DSITCF] | DSPI_4_SR[CMDFFF]
	// 302       DSPI_4_SR[SPEF] | DSPI_4_SR[DPEF]
	// 303       DSPI_4_SR[DDIF]

	InterruptSource(295, "HARDWARE.DSPI4_0.out");
	InterruptSource(296, "HARDWARE.DSPI_4.INT_EOQF");
	InterruptSource(297, "HARDWARE.DSPI_4.INT_TFFF");
	InterruptSource(298, "HARDWARE.DSPI_4.INT_TCF");
	InterruptSource(299, "HARDWARE.DSPI_4.INT_RFDF");
	InterruptSource(300, "HARDWARE.DSPI4_5.out");
	InterruptSource(301, "HARDWARE.DSPI4_6.out");
	InterruptSource(302, "HARDWARE.DSPI4_7.out");
	InterruptSource(303, "HARDWARE.DSPI_4.INT_DDIF");

	// 304       DSPI_5_SR[TFUF] | DSPI_5_SR[RFOF] |DSPI_5_SR[TFIWF]
	// 305       DSPI_5_SR[EOQF]
	// 306       DSPI_5_SR[TFFF]
	// 307       DSPI_5_SR[TCF]
	// 308       DSPI_5_SR[RFDF]
	// 309       DSPI_5_SR[SPITCF] | DSPI_5_SR[CMDTCF]
	// 310       DSPI_5_SR[DSITCF] | DSPI_5_SR[CMDFFF]
	// 311       DSPI_5_SR[SPEF] | DSPI_5_SR[DPEF]
	// 312       DSPI_5_SR[DDIF]

	InterruptSource(304, "HARDWARE.DSPI5_0.out");
	InterruptSource(305, "HARDWARE.DSPI_5.INT_EOQF");
	InterruptSource(306, "HARDWARE.DSPI_5.INT_TFFF");
	InterruptSource(307, "HARDWARE.DSPI_5.INT_TCF");
	InterruptSource(308, "HARDWARE.DSPI_5.INT_RFDF");
	InterruptSource(309, "HARDWARE.DSPI5_5.out");
	InterruptSource(310, "HARDWARE.DSPI5_6.out");
	InterruptSource(311, "HARDWARE.DSPI5_7.out");
	InterruptSource(312, "HARDWARE.DSPI_5.INT_DDIF");
	
	// 313       DSPI_6_SR[TFUF] | DSPI_6_SR[RFOF] | DSPI_6_SR[TFIWF]
	// 314       DSPI_6_SR[EOQF]
	// 315       DSPI_6_SR[TFFF]
	// 316       DSPI_6_SR[TCF]
	// 317       DSPI_6_SR[RFDF]
	// 318       DSPI_6_SR[SPITCF] | DSPI_6_SR[CMDTCF]
	// 319       DSPI_6_SR[DSITCF] | DSPI_6_SR[CMDFFF]
	// 320       DSPI_6_SR[SPEF] | DSPI_6_SR[DPEF]
	// 321       DSPI_6_SR[DDIF]

	InterruptSource(313, "HARDWARE.DSPI6_0.out");
	InterruptSource(314, "HARDWARE.DSPI_6.INT_EOQF");
	InterruptSource(315, "HARDWARE.DSPI_6.INT_TFFF");
	InterruptSource(316, "HARDWARE.DSPI_6.INT_TCF");
	InterruptSource(317, "HARDWARE.DSPI_6.INT_RFDF");
	InterruptSource(318, "HARDWARE.DSPI6_5.out");
	InterruptSource(319, "HARDWARE.DSPI6_6.out");
	InterruptSource(320, "HARDWARE.DSPI6_7.out");
	InterruptSource(321, "HARDWARE.DSPI_6.INT_DDIF");

	// 322       unused
	// ...
	// 366       unused
	
	InterruptSource(322);
	InterruptSource(323);
	InterruptSource(324);
	InterruptSource(325);
	InterruptSource(326);
	InterruptSource(327);
	InterruptSource(328);
	InterruptSource(329);
	InterruptSource(330);
	InterruptSource(331);
	InterruptSource(332);
	InterruptSource(333);
	InterruptSource(334);
	InterruptSource(335);
	InterruptSource(336);
	InterruptSource(337);
	InterruptSource(338);
	InterruptSource(339);
	InterruptSource(340);
	InterruptSource(341);
	InterruptSource(342);
	InterruptSource(343);
	InterruptSource(344);
	InterruptSource(345);
	InterruptSource(346);
	InterruptSource(347);
	InterruptSource(348);
	InterruptSource(349);
	InterruptSource(350);
	InterruptSource(351);
	InterruptSource(352);
	InterruptSource(353);
	InterruptSource(354);
	InterruptSource(355);
	InterruptSource(356);
	InterruptSource(357);
	InterruptSource(358);
	InterruptSource(359);
	InterruptSource(360);
	InterruptSource(361);
	InterruptSource(362);
	InterruptSource(363);
	InterruptSource(364);
	InterruptSource(365);
	InterruptSource(366);
	
	// 367       DSPI_12_SR[TFUF] | DSPI_12_SR[RFOF] | DSPI_12_SR[TFIWF]
	// 368       DSPI_12_SR[EOQF]
	// 369       DSPI_12_SR[TFFF]
	// 370       DSPI_12_SR[TCF]
	// 371       DDSPI_12_SR[RFDF]
	// 372       DSPI_12_SR[CMDTCF]
	// 373       DSPI_12_SR[CMDFFF]
	// 374       DSPI_12_SR[SPEF]

	InterruptSource(367, "HARDWARE.DSPI12_0.out");
	InterruptSource(368, "HARDWARE.DSPI_12.INT_EOQF");
	InterruptSource(369, "HARDWARE.DSPI_12.INT_TFFF");
	InterruptSource(370, "HARDWARE.DSPI_12.INT_TCF");
	InterruptSource(371, "HARDWARE.DSPI_12.INT_RFDF");
	InterruptSource(372, "HARDWARE.DSPI_12.INT_CMDTCF");
	InterruptSource(373, "HARDWARE.DSPI_12.INT_CMDFFF");
	InterruptSource(374, "HARDWARE.DSPI_12.INT_SPEF");

	// 375      unused
	
	InterruptSource(375);
	
	// 376      LINFlexD_0 RXI
	// 377      LINFlexD_0 TXI
	// 378      LINFlexD_0 ERR

	InterruptSource(376, "HARDWARE.LINFlexD_0.INT_RX");
	InterruptSource(377, "HARDWARE.LINFlexD_0.INT_TX");
	InterruptSource(378, "HARDWARE.LINFlexD_0.INT_ERR");
	
	// 379      unused
	
	InterruptSource(379);
	
	// 380      LINFlexD_1 RXI
	// 381      LINFlexD_1 TXI
	// 382      LINFlexD_1 ERR

	InterruptSource(380, "HARDWARE.LINFlexD_1.INT_RX");
	InterruptSource(381, "HARDWARE.LINFlexD_1.INT_TX");
	InterruptSource(382, "HARDWARE.LINFlexD_1.INT_ERR");

	// 383      unused
	
	InterruptSource(383);

	// 384      LINFlexD_2 RXI
	// 385      LINFlexD_2 TXI
	// 386      LINFlexD_2 ERR
	
	InterruptSource(384, "HARDWARE.LINFlexD_2.INT_RX");
	InterruptSource(385, "HARDWARE.LINFlexD_2.INT_TX");
	InterruptSource(386, "HARDWARE.LINFlexD_2.INT_ERR");

	// 387      TODO
	// ..       TODO
	// 415      TODO

	InterruptSource(387);
	InterruptSource(388);
	InterruptSource(389);
	InterruptSource(390);
	InterruptSource(391);
	InterruptSource(392);
	InterruptSource(393);
	InterruptSource(394);
	InterruptSource(395);
	InterruptSource(396);
	InterruptSource(397);
	InterruptSource(398);
	InterruptSource(399);
	InterruptSource(400);
	InterruptSource(401);
	InterruptSource(402);
	InterruptSource(403);
	InterruptSource(404);
	InterruptSource(405);
	InterruptSource(406);
	InterruptSource(407);
	InterruptSource(408);
	InterruptSource(409);
	InterruptSource(410);
	InterruptSource(411);
	InterruptSource(412);
	InterruptSource(413);
	InterruptSource(414);
	InterruptSource(415);
	
	// 416      LINFlexD_16 RXI
	// 417      LINFlexD_16 TXI
	// 418      LINFlexD_16 ERR

	InterruptSource(416, "HARDWARE.LINFlexD_16.INT_RX");
	InterruptSource(417, "HARDWARE.LINFlexD_16.INT_TX");
	InterruptSource(418, "HARDWARE.LINFlexD_16.INT_ERR");

	// 419      unused
	// ..       unused
	// 431      unused
	
	InterruptSource(419);
	InterruptSource(420);
	InterruptSource(421);
	InterruptSource(422);
	InterruptSource(423);
	InterruptSource(424);
	InterruptSource(425);
	InterruptSource(426);
	InterruptSource(427);
	InterruptSource(428);
	InterruptSource(429);
	InterruptSource(430);
	InterruptSource(431);
	
	// 432      LINFlexD_14 RXI
	// 433      LINFlexD_14 TXI
	// 434      LINFlexD_14 ERR
	
	InterruptSource(432, "HARDWARE.LINFlexD_14.INT_RX");
	InterruptSource(433, "HARDWARE.LINFlexD_14.INT_TX");
	InterruptSource(434, "HARDWARE.LINFlexD_14.INT_ERR");

	// 435      unused
	
	InterruptSource(435);
	
	// 436      LINFlexD_14 RXI
	// 437      LINFlexD_14 TXI
	// 438      LINFlexD_14 ERR
	
	InterruptSource(436, "HARDWARE.LINFlexD_15.INT_RX");
	InterruptSource(437, "HARDWARE.LINFlexD_15.INT_TX");
	InterruptSource(438, "HARDWARE.LINFlexD_15.INT_ERR");

	// 439      TODO
	// ..       TODO
	// 687
	
	InterruptSource(439);
	InterruptSource(440);
	InterruptSource(441);
	InterruptSource(442);
	InterruptSource(443);
	InterruptSource(444);
	InterruptSource(445);
	InterruptSource(446);
	InterruptSource(447);
	InterruptSource(448);
	InterruptSource(449);
	InterruptSource(450);
	InterruptSource(451);
	InterruptSource(452);
	InterruptSource(453);
	InterruptSource(454);
	InterruptSource(455);
	InterruptSource(456);
	InterruptSource(457);
	InterruptSource(458);
	InterruptSource(459);
	InterruptSource(460);
	InterruptSource(461);
	InterruptSource(462);
	InterruptSource(463);
	InterruptSource(464);
	InterruptSource(465);
	InterruptSource(466);
	InterruptSource(467);
	InterruptSource(468);
	InterruptSource(469);
	InterruptSource(470);
	InterruptSource(471);
	InterruptSource(472);
	InterruptSource(473);
	InterruptSource(474);
	InterruptSource(475);
	InterruptSource(476);
	InterruptSource(477);
	InterruptSource(478);
	InterruptSource(479);
	InterruptSource(480);
	InterruptSource(481);
	InterruptSource(482);
	InterruptSource(483);
	InterruptSource(484);
	InterruptSource(485);
	InterruptSource(486);
	InterruptSource(487);
	InterruptSource(488);
	InterruptSource(489);
	InterruptSource(490);
	InterruptSource(491);
	InterruptSource(492);
	InterruptSource(493);
	InterruptSource(494);
	InterruptSource(495);
	InterruptSource(496);
	InterruptSource(497);
	InterruptSource(498);
	InterruptSource(499);
	InterruptSource(500);
	InterruptSource(501);
	InterruptSource(502);
	InterruptSource(503);
	InterruptSource(504);
	InterruptSource(505);
	InterruptSource(506);
	InterruptSource(507);
	InterruptSource(508);
	InterruptSource(509);
	InterruptSource(510);
	InterruptSource(511);
	InterruptSource(512);
	InterruptSource(513);
	InterruptSource(514);
	InterruptSource(515);
	InterruptSource(516);
	InterruptSource(517);
	InterruptSource(518);
	InterruptSource(519);
	InterruptSource(520);
	InterruptSource(521);
	InterruptSource(522);
	InterruptSource(523);
	InterruptSource(524);
	InterruptSource(525);
	InterruptSource(526);
	InterruptSource(527);
	InterruptSource(528);
	InterruptSource(529);
	InterruptSource(530);
	InterruptSource(531);
	InterruptSource(532);
	InterruptSource(533);
	InterruptSource(534);
	InterruptSource(535);
	InterruptSource(536);
	InterruptSource(537);
	InterruptSource(538);
	InterruptSource(539);
	InterruptSource(540);
	InterruptSource(541);
	InterruptSource(542);
	InterruptSource(543);
	InterruptSource(544);
	InterruptSource(545);
	InterruptSource(546);
	InterruptSource(547);
	InterruptSource(548);
	InterruptSource(549);
	InterruptSource(550);
	InterruptSource(551);
	InterruptSource(552);
	InterruptSource(553);
	InterruptSource(554);
	InterruptSource(555);
	InterruptSource(556);
	InterruptSource(557);
	InterruptSource(558);
	InterruptSource(559);
	InterruptSource(560);
	InterruptSource(561);
	InterruptSource(562);
	InterruptSource(563);
	InterruptSource(564);
	InterruptSource(565);
	InterruptSource(566);
	InterruptSource(567);
	InterruptSource(568);
	InterruptSource(569);
	InterruptSource(570);
	InterruptSource(571);
	InterruptSource(572);
	InterruptSource(573);
	InterruptSource(574);
	InterruptSource(575);
	InterruptSource(576);
	InterruptSource(577);
	InterruptSource(578);
	InterruptSource(579);
	InterruptSource(580);
	InterruptSource(581);
	InterruptSource(582);
	InterruptSource(583);
	InterruptSource(584);
	InterruptSource(585);
	InterruptSource(586);
	InterruptSource(587);
	InterruptSource(588);
	InterruptSource(589);
	InterruptSource(590);
	InterruptSource(591);
	InterruptSource(592);
	InterruptSource(593);
	InterruptSource(594);
	InterruptSource(595);
	InterruptSource(596);
	InterruptSource(597);
	InterruptSource(598);
	InterruptSource(599);
	InterruptSource(600);
	InterruptSource(601);
	InterruptSource(602);
	InterruptSource(603);
	InterruptSource(604);
	InterruptSource(605);
	InterruptSource(606);
	InterruptSource(607);
	InterruptSource(608);
	InterruptSource(609);
	InterruptSource(610);
	InterruptSource(611);
	InterruptSource(612);
	InterruptSource(613);
	InterruptSource(614);
	InterruptSource(615);
	InterruptSource(616);
	InterruptSource(617);
	InterruptSource(618);
	InterruptSource(619);
	InterruptSource(620);
	InterruptSource(621);
	InterruptSource(622);
	InterruptSource(623);
	InterruptSource(624);
	InterruptSource(625);
	InterruptSource(626);
	InterruptSource(627);
	InterruptSource(628);
	InterruptSource(629);
	InterruptSource(630);
	InterruptSource(631);
	InterruptSource(632);
	InterruptSource(633);
	InterruptSource(634);
	InterruptSource(635);
	InterruptSource(636);
	InterruptSource(637);
	InterruptSource(638);
	InterruptSource(639);
	InterruptSource(640);
	InterruptSource(641);
	InterruptSource(642);
	InterruptSource(643);
	InterruptSource(644);
	InterruptSource(645);
	InterruptSource(646);
	InterruptSource(647);
	InterruptSource(648);
	InterruptSource(649);
	InterruptSource(650);
	InterruptSource(651);
	InterruptSource(652);
	InterruptSource(653);
	InterruptSource(654);
	InterruptSource(655);
	InterruptSource(656);
	InterruptSource(657);
	InterruptSource(658);
	InterruptSource(659);
	InterruptSource(660);
	InterruptSource(661);
	InterruptSource(662);
	InterruptSource(663);
	InterruptSource(664);
	InterruptSource(665);
	InterruptSource(666);
	InterruptSource(667);
	InterruptSource(668);
	InterruptSource(669);
	InterruptSource(670);
	InterruptSource(671);
	InterruptSource(672);
	InterruptSource(673);
	InterruptSource(674);
	InterruptSource(675);
	InterruptSource(676);
	InterruptSource(677);
	InterruptSource(678);
	InterruptSource(679);
	InterruptSource(680);
	InterruptSource(681);
	InterruptSource(682);
	InterruptSource(683);
	InterruptSource(684);
	InterruptSource(685);
	InterruptSource(686);
	InterruptSource(687);
	
	// 688      M_CAN_1 INT0
	// 689      M_CAN_1 INT1
	// 690      M_CAN_2 INT0
	// 691      M_CAN_2 INT1
	// 692      M_CAN_3 INT0
	// 693      M_CAN_3 INT1
	// 694      M_CAN_4 INT0
	// 695      M_CAN_4 INT1
	
	InterruptSource(688, "HARDWARE.M_CAN_1.INT0");
	InterruptSource(689, "HARDWARE.M_CAN_1.INT1");
	InterruptSource(690, "HARDWARE.M_CAN_2.INT0");
	InterruptSource(691, "HARDWARE.M_CAN_2.INT1");
	InterruptSource(692, "HARDWARE.M_CAN_3.INT0");
	InterruptSource(693, "HARDWARE.M_CAN_3.INT1");
	InterruptSource(694, "HARDWARE.M_CAN_4.INT0");
	InterruptSource(695, "HARDWARE.M_CAN_4.INT1");
	
	// 696      TODO
	// ..       TODO
	// 964
	
	InterruptSource(696);
	InterruptSource(697);
	InterruptSource(698);
	InterruptSource(699);
	InterruptSource(700);
	InterruptSource(701);
	InterruptSource(702);
	InterruptSource(703);
	InterruptSource(704);
	InterruptSource(705);
	InterruptSource(706);
	InterruptSource(707);
	InterruptSource(708);
	InterruptSource(709);
	InterruptSource(710);
	InterruptSource(711);
	InterruptSource(712);
	InterruptSource(713);
	InterruptSource(714);
	InterruptSource(715);
	InterruptSource(716);
	InterruptSource(717);
	InterruptSource(718);
	InterruptSource(719);
	InterruptSource(720);
	InterruptSource(721);
	InterruptSource(722);
	InterruptSource(723);
	InterruptSource(724);
	InterruptSource(725);
	InterruptSource(726);
	InterruptSource(727);
	InterruptSource(728);
	InterruptSource(729);
	InterruptSource(730);
	InterruptSource(731);
	InterruptSource(732);
	InterruptSource(733);
	InterruptSource(734);
	InterruptSource(735);
	InterruptSource(736);
	InterruptSource(737);
	InterruptSource(738);
	InterruptSource(739);
	InterruptSource(740);
	InterruptSource(741);
	InterruptSource(742);
	InterruptSource(743);
	InterruptSource(744);
	InterruptSource(745);
	InterruptSource(746);
	InterruptSource(747);
	InterruptSource(748);
	InterruptSource(749);
	InterruptSource(750);
	InterruptSource(751);
	InterruptSource(752);
	InterruptSource(753);
	InterruptSource(754);
	InterruptSource(755);
	InterruptSource(756);
	InterruptSource(757);
	InterruptSource(758);
	InterruptSource(759);
	InterruptSource(760);
	InterruptSource(761);
	InterruptSource(762);
	InterruptSource(763);
	InterruptSource(764);
	InterruptSource(765);
	InterruptSource(766);
	InterruptSource(767);
	InterruptSource(768);
	InterruptSource(769);
	InterruptSource(770);
	InterruptSource(771);
	InterruptSource(772);
	InterruptSource(773);
	InterruptSource(774);
	InterruptSource(775);
	InterruptSource(776);
	InterruptSource(777);
	InterruptSource(778);
	InterruptSource(779);
	InterruptSource(780);
	InterruptSource(781);
	InterruptSource(782);
	InterruptSource(783);
	InterruptSource(784);
	InterruptSource(785);
	InterruptSource(786);
	InterruptSource(787);
	InterruptSource(788);
	InterruptSource(789);
	InterruptSource(790);
	InterruptSource(791);
	InterruptSource(792);
	InterruptSource(793);
	InterruptSource(794);
	InterruptSource(795);
	InterruptSource(796);
	InterruptSource(797);
	InterruptSource(798);
	InterruptSource(799);
	InterruptSource(800);
	InterruptSource(801);
	InterruptSource(802);
	InterruptSource(803);
	InterruptSource(804);
	InterruptSource(805);
	InterruptSource(806);
	InterruptSource(807);
	InterruptSource(808);
	InterruptSource(809);
	InterruptSource(810);
	InterruptSource(811);
	InterruptSource(812);
	InterruptSource(813);
	InterruptSource(814);
	InterruptSource(815);
	InterruptSource(816);
	InterruptSource(817);
	InterruptSource(818);
	InterruptSource(819);
	InterruptSource(820);
	InterruptSource(821);
	InterruptSource(822);
	InterruptSource(823);
	InterruptSource(824);
	InterruptSource(825);
	InterruptSource(826);
	InterruptSource(827);
	InterruptSource(828);
	InterruptSource(829);
	InterruptSource(830);
	InterruptSource(831);
	InterruptSource(832);
	InterruptSource(833);
	InterruptSource(834);
	InterruptSource(835);
	InterruptSource(836);
	InterruptSource(837);
	InterruptSource(838);
	InterruptSource(839);
	InterruptSource(840);
	InterruptSource(841);
	InterruptSource(842);
	InterruptSource(843);
	InterruptSource(844);
	InterruptSource(845);
	InterruptSource(846);
	InterruptSource(847);
	InterruptSource(848);
	InterruptSource(849);
	InterruptSource(850);
	InterruptSource(851);
	InterruptSource(852);
	InterruptSource(853);
	InterruptSource(854);
	InterruptSource(855);
	InterruptSource(856);
	InterruptSource(857);
	InterruptSource(858);
	InterruptSource(859);
	InterruptSource(860);
	InterruptSource(861);
	InterruptSource(862);
	InterruptSource(863);
	InterruptSource(864);
	InterruptSource(865);
	InterruptSource(866);
	InterruptSource(867);
	InterruptSource(868);
	InterruptSource(869);
	InterruptSource(870);
	InterruptSource(871);
	InterruptSource(872);
	InterruptSource(873);
	InterruptSource(874);
	InterruptSource(875);
	InterruptSource(876);
	InterruptSource(877);
	InterruptSource(878);
	InterruptSource(879);
	InterruptSource(880);
	InterruptSource(881);
	InterruptSource(882);
	InterruptSource(883);
	InterruptSource(884);
	InterruptSource(885);
	InterruptSource(886);
	InterruptSource(887);
	InterruptSource(888);
	InterruptSource(889);
	InterruptSource(890);
	InterruptSource(891);
	InterruptSource(892);
	InterruptSource(893);
	InterruptSource(894);
	InterruptSource(895);
	InterruptSource(896);
	InterruptSource(897);
	InterruptSource(898);
	InterruptSource(899);
	InterruptSource(900);
	InterruptSource(901);
	InterruptSource(902);
	InterruptSource(903);
	InterruptSource(904);
	InterruptSource(905);
	InterruptSource(906);
	InterruptSource(907);
	InterruptSource(908);
	InterruptSource(909);
	InterruptSource(910);
	InterruptSource(911);
	InterruptSource(912);
	InterruptSource(913);
	InterruptSource(914);
	InterruptSource(915);
	InterruptSource(916);
	InterruptSource(917);
	InterruptSource(918);
	InterruptSource(919);
	InterruptSource(920);
	InterruptSource(921);
	InterruptSource(922);
	InterruptSource(923);
	InterruptSource(924);
	InterruptSource(925);
	InterruptSource(926);
	InterruptSource(927);
	InterruptSource(928);
	InterruptSource(929);
	InterruptSource(930);
	InterruptSource(931);
	InterruptSource(932);
	InterruptSource(933);
	InterruptSource(934);
	InterruptSource(935);
	InterruptSource(936);
	InterruptSource(937);
	InterruptSource(938);
	InterruptSource(939);
	InterruptSource(940);
	InterruptSource(941);
	InterruptSource(942);
	InterruptSource(943);
	InterruptSource(944);
	InterruptSource(945);
	InterruptSource(946);
	InterruptSource(947);
	InterruptSource(948);
	InterruptSource(949);
	InterruptSource(950);
	InterruptSource(951);
	InterruptSource(952);
	InterruptSource(953);
	InterruptSource(954);
	InterruptSource(955);
	InterruptSource(956);
	InterruptSource(957);
	InterruptSource(958);
	InterruptSource(959);
	InterruptSource(960);
	InterruptSource(961);
	InterruptSource(962);
	InterruptSource(963);
	InterruptSource(964);

	// DMA sources
	DMASource("HARDWARE.LINFlexD_0.DMA_RX_0", "HARDWARE.LINFlexD_0.DMA_ACK_RX_0", 0, 16, 1, 3, 4, 7);
	DMASource("HARDWARE.LINFlexD_0.DMA_TX_0", "HARDWARE.LINFlexD_0.DMA_ACK_TX_0", 0, 17, 1, 4, 4, 8);
	
	DMASource("HARDWARE.LINFlexD_1.DMA_RX_0", "HARDWARE.LINFlexD_1.DMA_ACK_RX_0", 1, 5, 5, 6);
	DMASource("HARDWARE.LINFlexD_1.DMA_TX_0", "HARDWARE.LINFlexD_1.DMA_ACK_TX_0", 1, 6, 5, 7);

	DMASource("HARDWARE.LINFlexD_2.DMA_RX_0", "HARDWARE.LINFlexD_2.DMA_ACK_RX_0", 2, 45, 3, 4, 5, 39);
	DMASource("HARDWARE.LINFlexD_2.DMA_TX_0", "HARDWARE.LINFlexD_2.DMA_ACK_TX_0", 2, 46, 3, 5, 5, 40);

	DMASource("HARDWARE.LINFlexD_14.DMA_RX_0", "HARDWARE.LINFlexD_14.DMA_ACK_RX_0", 0, 18, 1, 7, 4, 9);
	DMASource("HARDWARE.LINFlexD_14.DMA_TX_0", "HARDWARE.LINFlexD_14.DMA_ACK_TX_0", 1, 8, 4, 10);

	DMASource("HARDWARE.LINFlexD_15.DMA_RX_0", "HARDWARE.LINFlexD_15.DMA_ACK_RX_0", 1, 45, 5, 8);
	DMASource("HARDWARE.LINFlexD_15.DMA_TX_0", "HARDWARE.LINFlexD_15.DMA_ACK_TX_0", 1, 46, 5, 9);

	DMASource("HARDWARE.LINFlexD_16.DMA_RX_0", "HARDWARE.LINFlexD_16.DMA_ACK_RX_0", 6, 16);
	DMASource("HARDWARE.LINFlexD_16.DMA_TX_0", "HARDWARE.LINFlexD_16.DMA_ACK_TX_0", 6, 17);

	DMASource("HARDWARE.DSPI_0.DMA_RX", "HARDWARE.DSPI_0.DMA_ACK_RX", 0, 5, 1, 53, 4, 40);
	DMASource("HARDWARE.DSPI_0.DMA_TX", "HARDWARE.DSPI_0.DMA_ACK_TX", 0, 6, 1, 54, 4, 41);
	DMASource("HARDWARE.DSPI_0.DMA_CMD", "HARDWARE.DSPI_0.DMA_ACK_CMD", 0, 19, 4, 39);
	
	DMASource("HARDWARE.DSPI_1.DMA_RX", "HARDWARE.DSPI_1.DMA_ACK_RX", 2, 3, 3, 46, 6, 25);
	DMASource("HARDWARE.DSPI_1.DMA_TX", "HARDWARE.DSPI_1.DMA_ACK_TX", 2, 4, 3, 47, 6, 26);
	DMASource("HARDWARE.DSPI_1.DMA_CMD", "HARDWARE.DSPI_1.DMA_ACK_CMD", 2, 42, 6, 24);

	DMASource("HARDWARE.DSPI_2.DMA_RX", "HARDWARE.DSPI_2.DMA_ACK_RX", 2, 43, 3, 2, 9, 42);
	DMASource("HARDWARE.DSPI_2.DMA_TX", "HARDWARE.DSPI_2.DMA_ACK_TX", 2, 44, 3, 3, 9, 43);
	DMASource("HARDWARE.DSPI_2.DMA_CMD", "HARDWARE.DSPI_2.DMA_ACK_CMD", 3, 45, 9, 41);
	
	DMASource("HARDWARE.DSPI_3.DMA_RX", "HARDWARE.DSPI_3.DMA_ACK_RX", 4, 5, 5, 36, 7, 46);
	DMASource("HARDWARE.DSPI_3.DMA_TX", "HARDWARE.DSPI_3.DMA_ACK_TX", 4, 6, 5, 37, 7, 47);
	DMASource("HARDWARE.DSPI_3.DMA_CMD", "HARDWARE.DSPI_3.DMA_ACK_CMD", 4, 34, 7, 45);

	DMASource("HARDWARE.DSPI_4.DMA_RX", "HARDWARE.DSPI_4.DMA_ACK_RX", 0, 7, 8, 44); 
	DMASource("HARDWARE.DSPI_4.DMA_TX", "HARDWARE.DSPI_4.DMA_ACK_TX", 0, 8, 8, 45);
	DMASource("HARDWARE.DSPI_4.DMA_CMD", "HARDWARE.DSPI_4.DMA_ACK_CMD", 0, 20, 8, 43);

	DMASource("HARDWARE.DSPI_5.DMA_RX", "HARDWARE.DSPI_5.DMA_ACK_RX", 1, 47, 5, 4, 7, 49);
	DMASource("HARDWARE.DSPI_5.DMA_TX", "HARDWARE.DSPI_5.DMA_ACK_TX", 1, 48, 5, 5, 7, 50);
	DMASource("HARDWARE.DSPI_5.DMA_CMD", "HARDWARE.DSPI_5.DMA_ACK_CMD", 1, 49, 5, 33, 7, 48);
	  
	DMASource("HARDWARE.DSPI_6.DMA_RX", "HARDWARE.DSPI_6.DMA_ACK_RX", 6, 13);
	DMASource("HARDWARE.DSPI_6.DMA_TX", "HARDWARE.DSPI_6.DMA_ACK_TX", 6, 14);
	DMASource("HARDWARE.DSPI_6.DMA_CMD", "HARDWARE.DSPI_6.DMA_ACK_CMD", 6, 15);
	  
	DMASource("HARDWARE.DSPI_12.DMA_RX", "HARDWARE.DSPI_12.DMA_ACK_RX", 1, 1, 6, 28);
	DMASource("HARDWARE.DSPI_12.DMA_TX", "HARDWARE.DSPI_12.DMA_ACK_TX", 1, 2, 6, 29);
	DMASource("HARDWARE.DSPI_12.DMA_CMD", "HARDWARE.DSPI_12.DMA_ACK_CMD", 1, 50, 6, 27);
	
	DMASource("HARDWARE.M_CAN_1.DMA_REQ", "HARDWARE.M_CAN_1.DMA_ACK", 0, 12, 5, 34, 9, 34);
	DMASource("HARDWARE.M_CAN_2.DMA_REQ", "HARDWARE.M_CAN_2.DMA_ACK", 0, 13, 5, 35, 9, 35);
	  
	// DMAMUX_0
	DMASource(0, 0);
	DMASource(0, 1);
	DMASource(0, 2);
	DMASource(0, 3);
	DMASource(0, 4);
	DMASource(0, 9);
	DMASource(0, 10);
	DMASource(0, 11);
//	DMASource(0, 12);
//	DMASource(0, 13);
	DMASource(0, 14);
	DMASource(0, 15);
	DMASource(0, 21);
	DMASource(0, 22);
	DMASource(0, 23);
	DMASource(0, 24);
	DMASource(0, 25);
	DMASource(0, 26);
	DMASource(0, 27);
	DMASource(0, 28);
	DMASource(0, 29);
	DMASource(0, 30);
	DMASource(0, 31);
	DMASource(0, 32);
	DMASource(0, 33);
	DMASource(0, 34);
	DMASource(0, 35);
	DMASource(0, 36);
	DMASource(0, 37);
	DMASource(0, 38);
	DMASource(0, 39);
	DMASource(0, 40);
	DMASource(0, 41);
	DMASource(0, 42);
	DMASource(0, 43);
	DMASource(0, 44);
	DMASource(0, 45);
	DMASource(0, 46);
	DMASource(0, 47);
	DMASource(0, 48);
	DMASource(0, 49);
	DMASource(0, 50);
	DMASource(0, 51);
	DMASource(0, 52);
	DMASource(0, 53);
	DMASource(0, 54);
	DMASource(0, 55);
	DMASource(0, 56);
	DMASource(0, 57);
	DMASource(0, 58);
	DMASource(0, 59);
	DMASource(0, 60);
	DMASource(0, 61);
	DMASource(0, 62);
	DMASource(0, 63);
	
	// DMAMUX_1
	DMASource(1, 0);                                                                                                                                                                                        
	DMASource(1, 9);
	DMASource(1, 10);
	DMASource(1, 11);
	DMASource(1, 12);
	DMASource(1, 13);
	DMASource(1, 14);
	DMASource(1, 15);
	DMASource(1, 16);
	DMASource(1, 17);
	DMASource(1, 18);
	DMASource(1, 19);
	DMASource(1, 20);
	DMASource(1, 21);
	DMASource(1, 22);
	DMASource(1, 23);
	DMASource(1, 24);
	DMASource(1, 25);
	DMASource(1, 26);
	DMASource(1, 27);
	DMASource(1, 28);
	DMASource(1, 29);
	DMASource(1, 30);
	DMASource(1, 31);
	DMASource(1, 32);
	DMASource(1, 33);
	DMASource(1, 34);
	DMASource(1, 35);
	DMASource(1, 36);
	DMASource(1, 37);
	DMASource(1, 38);
	DMASource(1, 39);
	DMASource(1, 40);
	DMASource(1, 41);
	DMASource(1, 42);
	DMASource(1, 43);
	DMASource(1, 44);
	DMASource(1, 51);
	DMASource(1, 52);
	DMASource(1, 55);
	DMASource(1, 56);
	DMASource(1, 57);
	DMASource(1, 58);
	DMASource(1, 59);
	DMASource(1, 60);
	DMASource(1, 61);
	DMASource(1, 62);
	DMASource(1, 63);
	
	// DMAMUX_2
	DMASource(2, 0);
	DMASource(2, 1);
	DMASource(2, 2);
	DMASource(2, 5);
	DMASource(2, 6);
	DMASource(2, 7);
	DMASource(2, 8);
	DMASource(2, 9);
	DMASource(2, 10);
	DMASource(2, 11);
	DMASource(2, 12);
	DMASource(2, 13);
	DMASource(2, 14);
	DMASource(2, 15);
	DMASource(2, 16);
	DMASource(2, 17);
	DMASource(2, 18);
	DMASource(2, 19);
	DMASource(2, 20);
	DMASource(2, 21);
	DMASource(2, 22);
	DMASource(2, 23);
	DMASource(2, 24);
	DMASource(2, 25);
	DMASource(2, 26);
	DMASource(2, 27);
	DMASource(2, 28);
	DMASource(2, 29);
	DMASource(2, 30);
	DMASource(2, 31);
	DMASource(2, 32);
	DMASource(2, 33);
	DMASource(2, 34);
	DMASource(2, 35);
	DMASource(2, 36);
	DMASource(2, 37);
	DMASource(2, 38);
	DMASource(2, 39);
	DMASource(2, 40);
	DMASource(2, 41);
	DMASource(2, 47);
	DMASource(2, 48);
	DMASource(2, 49);
	DMASource(2, 50);
	DMASource(2, 51);
	DMASource(2, 52);
	DMASource(2, 53);
	DMASource(2, 54);
	DMASource(2, 55);
	DMASource(2, 56);
	DMASource(2, 57);
	DMASource(2, 58);
	DMASource(2, 59);
	DMASource(2, 60);
	DMASource(2, 61);
	DMASource(2, 62);
	DMASource(2, 63);
	
	// DMAMUX_3
	DMASource(3, 0);
	DMASource(3, 1);
	DMASource(3, 6);
	DMASource(3, 7);
	DMASource(3, 8);
	DMASource(3, 9);
	DMASource(3, 10);
	DMASource(3, 11);
	DMASource(3, 12);
	DMASource(3, 13);
	DMASource(3, 14);
	DMASource(3, 15);
	DMASource(3, 16);
	DMASource(3, 17);
	DMASource(3, 18);
	DMASource(3, 19);
	DMASource(3, 20);
	DMASource(3, 21);
	DMASource(3, 22);
	DMASource(3, 23);
	DMASource(3, 24);
	DMASource(3, 25);
	DMASource(3, 26);
	DMASource(3, 27);
	DMASource(3, 28);
	DMASource(3, 29);
	DMASource(3, 30);
	DMASource(3, 31);
	DMASource(3, 32);
	DMASource(3, 33);
	DMASource(3, 34);
	DMASource(3, 35);
	DMASource(3, 36);
	DMASource(3, 37);
	DMASource(3, 38);
	DMASource(3, 39);
	DMASource(3, 40);
	DMASource(3, 41);
	DMASource(3, 42);
	DMASource(3, 43);
	DMASource(3, 44);
	DMASource(3, 48);
	DMASource(3, 49);
	DMASource(3, 50);
	DMASource(3, 51);
	DMASource(3, 52);
	DMASource(3, 53);
	DMASource(3, 54);
	DMASource(3, 55);
	DMASource(3, 56);
	DMASource(3, 57);
	DMASource(3, 58);
	DMASource(3, 59);
	DMASource(3, 60);
	DMASource(3, 61);
	DMASource(3, 62);
	DMASource(3, 63);
	
	// DMAMUX_4
	DMASource(4, 0);
	DMASource(4, 1);
	DMASource(4, 2);
	DMASource(4, 3);
	DMASource(4, 4);
	DMASource(4, 11);
	DMASource(4, 12);
	DMASource(4, 13);
	DMASource(4, 14);
	DMASource(4, 15);
	DMASource(4, 16);
	DMASource(4, 17);
	DMASource(4, 18);
	DMASource(4, 19);
	DMASource(4, 20);
	DMASource(4, 21);
	DMASource(4, 22);
	DMASource(4, 23);
	DMASource(4, 24);
	DMASource(4, 25);
	DMASource(4, 26);
	DMASource(4, 27);
	DMASource(4, 28);
	DMASource(4, 29);
	DMASource(4, 30);
	DMASource(4, 31);
	DMASource(4, 32);
	DMASource(4, 33);
	DMASource(4, 35);
	DMASource(4, 36);
	DMASource(4, 37);
	DMASource(4, 38);
	DMASource(4, 42);
	DMASource(4, 43);
	DMASource(4, 44);
	DMASource(4, 45);
	DMASource(4, 46);
	DMASource(4, 47);
	DMASource(4, 48);
	DMASource(4, 49);
	DMASource(4, 50);
	DMASource(4, 51);
	DMASource(4, 52);
	DMASource(4, 53);
	DMASource(4, 54);
	DMASource(4, 55);
	DMASource(4, 56);
	DMASource(4, 57);
	DMASource(4, 58);
	DMASource(4, 59);
	DMASource(4, 60);
	DMASource(4, 61);
	DMASource(4, 62);
	DMASource(4, 63);
	
	// DMAMUX_5
	DMASource(5, 0);
	DMASource(5, 1);
	DMASource(5, 2);
	DMASource(5, 3);
	DMASource(5, 10);
	DMASource(5, 11);
	DMASource(5, 12);
	DMASource(5, 13);
	DMASource(5, 14);
	DMASource(5, 15);
	DMASource(5, 16);
	DMASource(5, 17);
	DMASource(5, 18);
	DMASource(5, 19);
	DMASource(5, 20);
	DMASource(5, 21);
	DMASource(5, 22);
	DMASource(5, 23);
	DMASource(5, 24);
	DMASource(5, 25);
	DMASource(5, 26);
	DMASource(5, 27);
	DMASource(5, 28);
	DMASource(5, 29);
	DMASource(5, 30);
	DMASource(5, 31);
	DMASource(5, 32);
// 	DMASource(5, 34);
// 	DMASource(5, 35);
	DMASource(5, 38);
	DMASource(5, 41);
	DMASource(5, 42);
	DMASource(5, 43);
	DMASource(5, 44);
	DMASource(5, 45);
	DMASource(5, 46);
	DMASource(5, 47);
	DMASource(5, 48);
	DMASource(5, 49);
	DMASource(5, 50);
	DMASource(5, 51);
	DMASource(5, 52);
	DMASource(5, 53);
	DMASource(5, 54);
	DMASource(5, 55);
	DMASource(5, 56);
	DMASource(5, 57);
	DMASource(5, 58);
	DMASource(5, 59);
	DMASource(5, 60);
	DMASource(5, 61);
	DMASource(5, 62);
	DMASource(5, 63);
	
	// DMAMUX_6
	DMASource(6, 0);
	DMASource(6, 1);
	DMASource(6, 2);
	DMASource(6, 3);
	DMASource(6, 4);
	DMASource(6, 5);
	DMASource(6, 6);
	DMASource(6, 7);
	DMASource(6, 8);
	DMASource(6, 9);
	DMASource(6, 10);
	DMASource(6, 11);
	DMASource(6, 12);
	DMASource(6, 18);
	DMASource(6, 19);
	DMASource(6, 20);
	DMASource(6, 21);
	DMASource(6, 22);
	DMASource(6, 23);
	DMASource(6, 30);
	DMASource(6, 31);
	DMASource(6, 32);
	DMASource(6, 33);
	DMASource(6, 34);
	DMASource(6, 35);
	DMASource(6, 36);
	DMASource(6, 37);
	DMASource(6, 38);
	DMASource(6, 39);
	DMASource(6, 40);
	DMASource(6, 41);
	DMASource(6, 42);
	DMASource(6, 43);
	DMASource(6, 44);
	DMASource(6, 45);
	DMASource(6, 46);
	DMASource(6, 47);
	DMASource(6, 48);
	DMASource(6, 49);
	DMASource(6, 50);
	DMASource(6, 51);
	DMASource(6, 52);
	DMASource(6, 53);
	DMASource(6, 54);
	DMASource(6, 55);
	DMASource(6, 56);
	DMASource(6, 57);
	DMASource(6, 58);
	DMASource(6, 59);
	DMASource(6, 60);
	DMASource(6, 61);
	DMASource(6, 62);
	DMASource(6, 63);
	
	// DMAMUX_7
	DMASource(7, 0);
	DMASource(7, 1);
	DMASource(7, 2);
	DMASource(7, 3);
	DMASource(7, 4);
	DMASource(7, 5);
	DMASource(7, 6);
	DMASource(7, 7);
	DMASource(7, 8);
	DMASource(7, 9);
	DMASource(7, 10);
	DMASource(7, 11);
	DMASource(7, 12);
	DMASource(7, 13);
	DMASource(7, 14);
	DMASource(7, 15);
	DMASource(7, 16);
	DMASource(7, 17);
	DMASource(7, 18);
	DMASource(7, 19);
	DMASource(7, 20);
	DMASource(7, 21);
	DMASource(7, 22);
	DMASource(7, 23);
	DMASource(7, 24);
	DMASource(7, 25);
	DMASource(7, 26);
	DMASource(7, 27);
	DMASource(7, 28);
	DMASource(7, 29);
	DMASource(7, 30);
	DMASource(7, 31);
	DMASource(7, 32);
	DMASource(7, 33);
	DMASource(7, 34);
	DMASource(7, 35);
	DMASource(7, 36);
	DMASource(7, 37);
	DMASource(7, 38);
	DMASource(7, 39);
	DMASource(7, 40);
	DMASource(7, 41);
	DMASource(7, 42);
	DMASource(7, 43);
	DMASource(7, 44);
	DMASource(7, 51);
	DMASource(7, 52);
	DMASource(7, 53);
	DMASource(7, 54);
	DMASource(7, 55);
	DMASource(7, 56);
	DMASource(7, 57);
	DMASource(7, 58);
	DMASource(7, 59);
	DMASource(7, 60);
	DMASource(7, 61);
	DMASource(7, 62);
	DMASource(7, 63);
	
	// DMAMUX_8
	DMASource(8, 0);
	DMASource(8, 1);
	DMASource(8, 2);
	DMASource(8, 3);
	DMASource(8, 4);
	DMASource(8, 5);
	DMASource(8, 6);
	DMASource(8, 7);
	DMASource(8, 8);
	DMASource(8, 9);
	DMASource(8, 10);
	DMASource(8, 11);
	DMASource(8, 12);
	DMASource(8, 13);
	DMASource(8, 14);
	DMASource(8, 15);
	DMASource(8, 16);
	DMASource(8, 17);
	DMASource(8, 18);
	DMASource(8, 19);
	DMASource(8, 20);
	DMASource(8, 21);
	DMASource(8, 22);
	DMASource(8, 23);
	DMASource(8, 24);
	DMASource(8, 25);
	DMASource(8, 26);
	DMASource(8, 27);
	DMASource(8, 28);
	DMASource(8, 29);
	DMASource(8, 30);
	DMASource(8, 31);
	DMASource(8, 32);
	DMASource(8, 33);
	DMASource(8, 34);
	DMASource(8, 35);
	DMASource(8, 36);
	DMASource(8, 37);
	DMASource(8, 38);
	DMASource(8, 39);
	DMASource(8, 40);
	DMASource(8, 41);
	DMASource(8, 42);
	DMASource(8, 46);
	DMASource(8, 47);
	DMASource(8, 48);
	DMASource(8, 49);
	DMASource(8, 50);
	DMASource(8, 51);
	DMASource(8, 52);
	DMASource(8, 53);
	DMASource(8, 54);
	DMASource(8, 55);
	DMASource(8, 56);
	DMASource(8, 57);
	DMASource(8, 58);
	DMASource(8, 59);
	DMASource(8, 60);
	DMASource(8, 61);
	DMASource(8, 62);
	DMASource(8, 63);
	
	// DMAMUX_9
	DMASource(9, 0);
	DMASource(9, 1);
	DMASource(9, 2);
	DMASource(9, 3);
	DMASource(9, 4);
	DMASource(9, 5);
	DMASource(9, 6);
	DMASource(9, 7);
	DMASource(9, 8);
	DMASource(9, 9);
	DMASource(9, 10);
	DMASource(9, 11);
	DMASource(9, 12);
	DMASource(9, 13);
	DMASource(9, 14);
	DMASource(9, 15);
	DMASource(9, 16);
	DMASource(9, 17);
	DMASource(9, 18);
	DMASource(9, 19);
	DMASource(9, 20);
	DMASource(9, 21);
	DMASource(9, 22);
	DMASource(9, 23);
	DMASource(9, 24);
	DMASource(9, 25);
	DMASource(9, 26);
	DMASource(9, 27);
	DMASource(9, 28);
	DMASource(9, 29);
	DMASource(9, 30);
	DMASource(9, 31);
	DMASource(9, 32);
	DMASource(9, 33);
// 	DMASource(9, 34);
// 	DMASource(9, 35);
	DMASource(9, 36);
	DMASource(9, 37);
	DMASource(9, 38);
	DMASource(9, 39);
	DMASource(9, 40);
	DMASource(9, 44);
	DMASource(9, 45);
	DMASource(9, 46);
	DMASource(9, 47);
	DMASource(9, 48);
	DMASource(9, 49);
	DMASource(9, 50);
	DMASource(9, 51);
	DMASource(9, 52);
	DMASource(9, 53);
	DMASource(9, 54);
	DMASource(9, 55);
	DMASource(9, 56);
	DMASource(9, 57);
	DMASource(9, 58);
	DMASource(9, 59);
	DMASource(9, 60);
	DMASource(9, 61);
	DMASource(9, 62);
	DMASource(9, 63);
			  
	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	main_core_0->memory_import >> xbar_0->memory_export;
	main_core_1->memory_import >> xbar_0->memory_export;
	peripheral_core_2->memory_import >> xbar_1->memory_export;

	(*xbar_0->memory_import[0]) >> flash->memory_export;
	(*xbar_0->memory_import[2]) >> main_core_0->local_memory_export;
	(*xbar_0->memory_import[3]) >> main_core_1->local_memory_export;
	(*xbar_0->memory_import[4]) >> system_sram->memory_export;
	(*xbar_0->memory_import[7]) >> xbar_1->memory_export;
	
	(*xbar_1->memory_import[1]) >> peripheral_core_2->local_memory_export;
		
	if(debugger)
	{
		// Connect debugger to CPUs
		
		// Main Core 0
		main_core_0->debug_yielding_import                   >> *debugger->debug_yielding_export[0];
		main_core_0->trap_reporting_import                   >> *debugger->trap_reporting_export[0];
		main_core_0->memory_access_reporting_import          >> *debugger->memory_access_reporting_export[0];
		*debugger->disasm_import                         [0] >> main_core_0->disasm_export;
		*debugger->memory_import                         [0] >> main_core_0->memory_export;
		*debugger->registers_import                      [0] >> main_core_0->registers_export;
		*debugger->memory_access_reporting_control_import[0] >> main_core_0->memory_access_reporting_control_export;

		// Main Core 1
		main_core_1->debug_yielding_import                   >> *debugger->debug_yielding_export[1];
		main_core_1->trap_reporting_import                   >> *debugger->trap_reporting_export[1];
		main_core_1->memory_access_reporting_import          >> *debugger->memory_access_reporting_export[1];
		*debugger->disasm_import                         [1] >> main_core_1->disasm_export;
		*debugger->memory_import                         [1] >> main_core_1->memory_export;
		*debugger->registers_import                      [1] >> main_core_1->registers_export;
		*debugger->memory_access_reporting_control_import[1] >> main_core_1->memory_access_reporting_control_export;

		// Peripheral Core 2
		peripheral_core_2->debug_yielding_import             >> *debugger->debug_yielding_export[2];
		peripheral_core_2->trap_reporting_import             >> *debugger->trap_reporting_export[2];
		peripheral_core_2->memory_access_reporting_import    >> *debugger->memory_access_reporting_export[2];
		*debugger->disasm_import                         [2] >> peripheral_core_2->disasm_export;
		*debugger->memory_import                         [2] >> peripheral_core_2->memory_export;
		*debugger->registers_import                      [2] >> peripheral_core_2->registers_export;
		*debugger->memory_access_reporting_control_import[2] >> peripheral_core_2->memory_access_reporting_control_export;
		
		// Connect debugger to loader
		debugger->blob_import >> loader->blob_export;
	}
	
	unsigned int front_end_num = 0;
	
	// Connect gdb-server to debugger
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++, front_end_num++)
	{
		if(gdb_server[prc_num])
		{
			*debugger->debug_yielding_import[front_end_num]       >> gdb_server[prc_num]->debug_yielding_export;
			*debugger->debug_event_listener_import[front_end_num] >> gdb_server[prc_num]->debug_event_listener_export;
			gdb_server[prc_num]->debug_yielding_request_import    >> *debugger->debug_yielding_request_export[front_end_num];
			gdb_server[prc_num]->debug_selecting_import           >> *debugger->debug_selecting_export[front_end_num];
			gdb_server[prc_num]->debug_event_trigger_import       >> *debugger->debug_event_trigger_export[front_end_num];
			gdb_server[prc_num]->memory_import                    >> *debugger->memory_export[front_end_num];
			gdb_server[prc_num]->registers_import                 >> *debugger->registers_export[front_end_num];
		}
	}

	// Connect inline-debugger to debugger
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++, front_end_num++)
	{
		if(inline_debugger[prc_num])
		{
			*debugger->debug_event_listener_import[front_end_num]   >> inline_debugger[prc_num]->debug_event_listener_export;
			*debugger->debug_yielding_import[front_end_num]         >> inline_debugger[prc_num]->debug_yielding_export;
			inline_debugger[prc_num]->debug_yielding_request_import >> *debugger->debug_yielding_request_export[front_end_num];
			inline_debugger[prc_num]->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[front_end_num];
			inline_debugger[prc_num]->disasm_import                 >> *debugger->disasm_export[front_end_num];
			inline_debugger[prc_num]->memory_import                 >> *debugger->memory_export[front_end_num];
			inline_debugger[prc_num]->registers_import              >> *debugger->registers_export[front_end_num];
			inline_debugger[prc_num]->stmt_lookup_import            >> *debugger->stmt_lookup_export[front_end_num];
			inline_debugger[prc_num]->symbol_table_lookup_import    >> *debugger->symbol_table_lookup_export[front_end_num];
			inline_debugger[prc_num]->backtrace_import              >> *debugger->backtrace_export[front_end_num];
			inline_debugger[prc_num]->debug_info_loading_import     >> *debugger->debug_info_loading_export[front_end_num];
			inline_debugger[prc_num]->data_object_lookup_import     >> *debugger->data_object_lookup_export[front_end_num];
			inline_debugger[prc_num]->subprogram_lookup_import      >> *debugger->subprogram_lookup_export[front_end_num];
		}
	}

	// Connect profiler to debugger
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++, front_end_num++)
	{
		if(profiler[prc_num])
		{
			*debugger->debug_event_listener_import[front_end_num] >> profiler[prc_num]->debug_event_listener_export;
			*debugger->debug_yielding_import[front_end_num]       >> profiler[prc_num]->debug_yielding_export;
			profiler[prc_num]->debug_yielding_request_import      >> *debugger->debug_yielding_request_export[front_end_num];
			profiler[prc_num]->debug_event_trigger_import         >> *debugger->debug_event_trigger_export[front_end_num];
			profiler[prc_num]->disasm_import                      >> *debugger->disasm_export[front_end_num];
			profiler[prc_num]->memory_import                      >> *debugger->memory_export[front_end_num];
			profiler[prc_num]->registers_import                   >> *debugger->registers_export[front_end_num];
			profiler[prc_num]->stmt_lookup_import                 >> *debugger->stmt_lookup_export[front_end_num];
			profiler[prc_num]->symbol_table_lookup_import         >> *debugger->symbol_table_lookup_export[front_end_num];
			profiler[prc_num]->backtrace_import                   >> *debugger->backtrace_export[front_end_num];
			profiler[prc_num]->debug_info_loading_import          >> *debugger->debug_info_loading_export[front_end_num];
			profiler[prc_num]->data_object_lookup_import          >> *debugger->data_object_lookup_export[front_end_num];
			profiler[prc_num]->subprogram_lookup_import           >> *debugger->subprogram_lookup_export[front_end_num];
		}
	}

	(*loader->memory_import[0]) >> flash->memory_export;
	(*loader->memory_import[1]) >> system_sram->memory_export;
	(*loader->memory_import[2]) >> main_core_0->memory_export;
	(*loader->memory_import[3]) >> main_core_1->memory_export;
	(*loader->memory_import[4]) >> peripheral_core_2->memory_export;
	(*loader->memory_import[5]) >> ebi_mem_0->memory_export;
	(*loader->memory_import[6]) >> ebi_mem_1->memory_export;
	(*loader->memory_import[7]) >> ebi_mem_2->memory_export;
	loader->registers_import >> peripheral_core_2->registers_export;
	main_core_0->symbol_table_lookup_import >> loader->symbol_table_lookup_export;
	main_core_1->symbol_table_lookup_import >> loader->symbol_table_lookup_export;
	peripheral_core_2->symbol_table_lookup_import >> loader->symbol_table_lookup_export;
	
	if(serial_terminal0 && char_io_tee0)
	{
		serial_terminal0->char_io_import >> char_io_tee0->char_io_export;
		if(netstreamer0)
		{
			(*char_io_tee0->char_io_import[0]) >> netstreamer0->char_io_export;
		}
		if(web_terminal0)
		{
			(*char_io_tee0->char_io_import[1]) >> web_terminal0->char_io_export;
		}
	}
	if(serial_terminal1 && char_io_tee1)
	{
		serial_terminal1->char_io_import >> char_io_tee1->char_io_export;
		if(netstreamer1)
		{
			(*char_io_tee1->char_io_import[0]) >> netstreamer1->char_io_export;
		}
		if(web_terminal1)
		{
			(*char_io_tee1->char_io_import[1]) >> web_terminal1->char_io_export;
		}
	}
	if(serial_terminal2 && char_io_tee2)
	{
		serial_terminal2->char_io_import >> char_io_tee2->char_io_export;
		if(netstreamer2)
		{
			(*char_io_tee2->char_io_import[0]) >> netstreamer2->char_io_export;
		}
		if(web_terminal2)
		{
			(*char_io_tee2->char_io_import[1]) >> web_terminal2->char_io_export;
		}
	}
	if(serial_terminal14 && char_io_tee14)
	{
		serial_terminal14->char_io_import >> char_io_tee14->char_io_export;
		if(netstreamer14)
		{
			(*char_io_tee14->char_io_import[0]) >> netstreamer14->char_io_export;
		}
		if(web_terminal14)
		{
			(*char_io_tee14->char_io_import[1]) >> web_terminal14->char_io_export;
		}
	}
	if(serial_terminal15 && char_io_tee15)
	{
		serial_terminal15->char_io_import >> char_io_tee15->char_io_export;
		if(netstreamer15)
		{
			(*char_io_tee15->char_io_import[0]) >> netstreamer15->char_io_export;
		}
		if(web_terminal15)
		{
			(*char_io_tee15->char_io_import[1]) >> web_terminal15->char_io_export;
		}
	}
	if(serial_terminal16 && char_io_tee16)
	{
		serial_terminal16->char_io_import >> char_io_tee16->char_io_export;
		if(netstreamer16)
		{
			(*char_io_tee16->char_io_import[0]) >> netstreamer16->char_io_export;
		}
		if(web_terminal16)
		{
			(*char_io_tee16->char_io_import[1]) >> web_terminal16->char_io_export;
		}
	}
	
	{
		unsigned int i = 0;
		*http_server->http_server_import[i++] >> instrumenter->http_server_export;
		*http_server->http_server_import[i++] >> logger_http_writer->http_server_export;
		for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++, front_end_num++)
		{
			if(profiler[prc_num])
			{
				*http_server->http_server_import[i++] >> profiler[prc_num]->http_server_export;
			}
		}
		if(web_terminal0) *http_server->http_server_import[i++] >> web_terminal0->http_server_export;
		if(web_terminal1) *http_server->http_server_import[i++] >> web_terminal1->http_server_export;
		if(web_terminal2) *http_server->http_server_import[i++] >> web_terminal2->http_server_export;
		if(web_terminal14) *http_server->http_server_import[i++] >> web_terminal14->http_server_export;
		if(web_terminal15) *http_server->http_server_import[i++] >> web_terminal15->http_server_export;
		if(web_terminal16) *http_server->http_server_import[i++] >> web_terminal16->http_server_export;
		*http_server->http_server_import[i++] >> main_core_0->mpu_http_server_export;
		*http_server->http_server_import[i++] >> main_core_1->mpu_http_server_export;
		*http_server->http_server_import[i++] >> peripheral_core_2->mpu_http_server_export;
	}
	
	{
		unsigned int i = 0;
		*http_server->registers_import[i++] >> main_core_0->registers_export;
		*http_server->registers_import[i++] >> main_core_1->registers_export;
		*http_server->registers_import[i++] >> peripheral_core_2->registers_export;
		*http_server->registers_import[i++] >> m_can_1->registers_export;
		*http_server->registers_import[i++] >> m_can_2->registers_export;
		*http_server->registers_import[i++] >> m_can_3->registers_export;
		*http_server->registers_import[i++] >> m_can_4->registers_export;
		*http_server->registers_import[i++] >> dmamux_0->registers_export;
		*http_server->registers_import[i++] >> dmamux_1->registers_export;
		*http_server->registers_import[i++] >> dmamux_2->registers_export;
		*http_server->registers_import[i++] >> dmamux_3->registers_export;
		*http_server->registers_import[i++] >> dmamux_4->registers_export;
		*http_server->registers_import[i++] >> dmamux_5->registers_export;
		*http_server->registers_import[i++] >> dmamux_6->registers_export;
		*http_server->registers_import[i++] >> dmamux_7->registers_export;
		*http_server->registers_import[i++] >> dmamux_8->registers_export;
		*http_server->registers_import[i++] >> dmamux_9->registers_export;
		*http_server->registers_import[i++] >> edma_0->registers_export;
		*http_server->registers_import[i++] >> edma_1->registers_export;
		*http_server->registers_import[i++] >> dspi_0->registers_export;
		*http_server->registers_import[i++] >> dspi_1->registers_export;
		*http_server->registers_import[i++] >> dspi_2->registers_export;
		*http_server->registers_import[i++] >> dspi_3->registers_export;
		*http_server->registers_import[i++] >> dspi_4->registers_export;
		*http_server->registers_import[i++] >> dspi_5->registers_export;
		*http_server->registers_import[i++] >> dspi_6->registers_export;
		*http_server->registers_import[i++] >> dspi_12->registers_export;
		*http_server->registers_import[i++] >> ebi->registers_export;
		*http_server->registers_import[i++] >> xbar_0->registers_export;
		*http_server->registers_import[i++] >> xbar_0->smpu_registers_export;
		*http_server->registers_import[i++] >> xbar_1->registers_export;
		*http_server->registers_import[i++] >> xbar_1->smpu_registers_export;
		*http_server->registers_import[i++] >> pbridge_a->registers_export;
		*http_server->registers_import[i++] >> pbridge_b->registers_export;
		*http_server->registers_import[i++] >> intc_0->registers_export;
		*http_server->registers_import[i++] >> linflexd_0->registers_export;
		*http_server->registers_import[i++] >> linflexd_1->registers_export;
		*http_server->registers_import[i++] >> linflexd_2->registers_export;
		*http_server->registers_import[i++] >> linflexd_14->registers_export;
		*http_server->registers_import[i++] >> linflexd_15->registers_export;
		*http_server->registers_import[i++] >> linflexd_16->registers_export;
		*http_server->registers_import[i++] >> siul2->registers_export;
		*http_server->registers_import[i++] >> pit_0->registers_export;
		*http_server->registers_import[i++] >> pit_1->registers_export;
		*http_server->registers_import[i++] >> stm_0->registers_export;
		*http_server->registers_import[i++] >> stm_1->registers_export;
		*http_server->registers_import[i++] >> stm_2->registers_export;
		*http_server->registers_import[i++] >> swt_0->registers_export;
		*http_server->registers_import[i++] >> swt_1->registers_export;
		*http_server->registers_import[i++] >> swt_2->registers_export;
		*http_server->registers_import[i++] >> swt_3->registers_export;
		*http_server->registers_import[i++] >> sema4->registers_export;
	}
	
#if HAVE_TVS
	bool enable_bandwidth_tracing = (*xbar_0)["enable-bandwidth-tracing"] ||
	                                (*xbar_1)["enable-bandwidth-tracing"] ||
	                                (*pbridge_a)["enable-bandwidth-tracing"] ||
	                                (*pbridge_b)["enable-bandwidth-tracing"] ||
	                                (*xbar_1_m1_concentrator)["enable-bandwidth-tracing"] ||
	                                (*iahbg_0)["enable-bandwidth-tracing"] ||
	                                (*iahbg_1)["enable-bandwidth-tracing"] ||
	                                (*ebi)["enable-bandwidth-tracing"];
	if(enable_bandwidth_tracing)
	{
		if((*peripheral_core_2)["enable-dmi"] || (*main_core_0)["enable-dmi"] || (*main_core_1)["enable-dmi"])
		{
			logger << DebugWarning << "As tracing is enabled at interconnect level, you should consider preventing Processors "
			       << peripheral_core_2->GetName() << ", " << main_core_0->GetName() << " and " << main_core_1->GetName()
			       << " from using SystemC TLM-2.0 DMI in order to capture all interconnect bandwidth in File \""
			       << bandwidth_vcd_filename << "\"" << EndDebugWarning;
		}
		
		if(!bandwidth_vcd_filename.empty())
		{
			bandwidth_vcd_file = new std::ofstream(bandwidth_vcd_filename.c_str());
			
			if(bandwidth_vcd_file->is_open())
			{
				bandwidth_vcd = new tracing::timed_stream_vcd_processor("bandwidth", *bandwidth_vcd_file);
			}
			else
			{
				logger << DebugWarning << "Can't open File \"" << bandwidth_vcd_filename << "\"" << EndDebugWarning;
				delete bandwidth_vcd;
			}
		}
		
		std::ofstream *bandwidth_gtkwave_init_script_file = 0;
		if(!bandwidth_gtkwave_init_script.empty())
		{
			bandwidth_gtkwave_init_script_file = new std::ofstream(bandwidth_gtkwave_init_script.c_str());
			
			if(bandwidth_gtkwave_init_script_file->is_open())
			{
				(*bandwidth_gtkwave_init_script_file) << "# Added some signals to view" << std::endl;
				(*bandwidth_gtkwave_init_script_file) << "set my_sig_list [list]" << std::endl;
			}
			else
			{
				logger << DebugWarning << "Can't open output \"" << bandwidth_gtkwave_init_script << "\"" << EndDebugWarning;
				delete bandwidth_gtkwave_init_script_file;
			}
		}
		
		if(bandwidth_vcd || bandwidth_gtkwave_init_script_file)
		{
			typedef tracing::timed_stream<double, unisim::component::tlm2::interconnect::generic_router::timed_bandwidth_traits> stream_type;
			
			if((*xbar_0)["enable-bandwidth-tracing"])
			{
				for(unsigned int i = 0; i < XBAR_0_CONFIG::OUTPUT_SOCKETS; i++)
				{
					std::stringstream param_name_sstr;
					param_name_sstr << "output_socket_name_" << i;
					std::stringstream sstr;
					sstr << "HARDWARE.XBAR_0." << (std::string)(*xbar_0)[param_name_sstr.str()] << "_bw";
					if(bandwidth_vcd) bandwidth_vcd->add(tracing::stream_by_name<stream_type>(sstr.str().c_str()));
					if(bandwidth_gtkwave_init_script_file) (*bandwidth_gtkwave_init_script_file) << "lappend my_sig_list \"" << sstr.str() << "\"" << std::endl;
				}
				for(unsigned int i = 0; i < XBAR_0_CONFIG::INPUT_SOCKETS; i++)
				{
					std::stringstream param_name_sstr;
					param_name_sstr << "input_socket_name_" << i;
					std::stringstream sstr;
					sstr << "HARDWARE.XBAR_0." << (std::string)(*xbar_0)[param_name_sstr.str()] << "_bw";
					if(bandwidth_vcd) bandwidth_vcd->add(tracing::stream_by_name<stream_type>(sstr.str().c_str()));
					if(bandwidth_gtkwave_init_script_file) (*bandwidth_gtkwave_init_script_file) << "lappend my_sig_list \"" << sstr.str() << "\"" << std::endl;
				}
			}
			if((*xbar_1)["enable-bandwidth-tracing"])
			{
				for(unsigned int i = 0; i < XBAR_1_CONFIG::OUTPUT_SOCKETS; i++)
				{
					std::stringstream param_name_sstr;
					param_name_sstr << "output_socket_name_" << i;
					std::stringstream sstr;
					sstr << "HARDWARE.XBAR_1." << (std::string)(*xbar_1)[param_name_sstr.str()] << "_bw";
					if(bandwidth_vcd) bandwidth_vcd->add(tracing::stream_by_name<stream_type>(sstr.str().c_str()));
					if(bandwidth_gtkwave_init_script_file) (*bandwidth_gtkwave_init_script_file) << "lappend my_sig_list \"" << sstr.str() << "\"" << std::endl;
				}
				for(unsigned int i = 0; i < XBAR_1_CONFIG::INPUT_SOCKETS; i++)
				{
					std::stringstream param_name_sstr;
					param_name_sstr << "input_socket_name_" << i;
					std::stringstream sstr;
					sstr << "HARDWARE.XBAR_1." << (std::string)(*xbar_1)[param_name_sstr.str()] << "_bw";
					if(bandwidth_vcd) bandwidth_vcd->add(tracing::stream_by_name<stream_type>(sstr.str().c_str()));
					if(bandwidth_gtkwave_init_script_file) (*bandwidth_gtkwave_init_script_file) << "lappend my_sig_list \"" << sstr.str() << "\"" << std::endl;
				}
			}
			if((*pbridge_a)["enable-bandwidth-tracing"])
			{
				for(unsigned int i = 0; i < PBRIDGE_A_CONFIG::OUTPUT_SOCKETS; i++)
				{
					std::stringstream param_name_sstr;
					param_name_sstr << "output_socket_name_" << i;
					std::stringstream sstr;
					sstr << "HARDWARE.PBRIDGE_A." << (std::string)(*pbridge_a)[param_name_sstr.str()] << "_bw";
					if(bandwidth_vcd) bandwidth_vcd->add(tracing::stream_by_name<stream_type>(sstr.str().c_str()));
					if(bandwidth_gtkwave_init_script_file) (*bandwidth_gtkwave_init_script_file) << "lappend my_sig_list \"" << sstr.str() << "\"" << std::endl;
				}
				for(unsigned int i = 0; i < PBRIDGE_A_CONFIG::INPUT_SOCKETS; i++)
				{
					std::stringstream param_name_sstr;
					param_name_sstr << "input_socket_name_" << i;
					std::stringstream sstr;
					sstr << "HARDWARE.PBRIDGE_A." << (std::string)(*pbridge_a)[param_name_sstr.str()] << "_bw";
					if(bandwidth_vcd) bandwidth_vcd->add(tracing::stream_by_name<stream_type>(sstr.str().c_str()));
					if(bandwidth_gtkwave_init_script_file) (*bandwidth_gtkwave_init_script_file) << "lappend my_sig_list \"" << sstr.str() << "\"" << std::endl;
				}
			}
			if((*pbridge_b)["enable-bandwidth-tracing"])
			{
				for(unsigned int i = 0; i < PBRIDGE_B_CONFIG::OUTPUT_SOCKETS; i++)
				{
					std::stringstream param_name_sstr;
					param_name_sstr << "output_socket_name_" << i;
					std::stringstream sstr;
					sstr << "HARDWARE.PBRIDGE_B." << (std::string)(*pbridge_b)[param_name_sstr.str()] << "_bw";
					if(bandwidth_vcd) bandwidth_vcd->add(tracing::stream_by_name<stream_type>(sstr.str().c_str()));
					if(bandwidth_gtkwave_init_script_file) (*bandwidth_gtkwave_init_script_file) << "lappend my_sig_list \"" << sstr.str() << "\"" << std::endl;
				}
				for(unsigned int i = 0; i < PBRIDGE_B_CONFIG::INPUT_SOCKETS; i++)
				{
					std::stringstream param_name_sstr;
					param_name_sstr << "input_socket_name_" << i;
					std::stringstream sstr;
					sstr << "HARDWARE.PBRIDGE_B." << (std::string)(*pbridge_b)[param_name_sstr.str()] << "_bw";
					if(bandwidth_vcd) bandwidth_vcd->add(tracing::stream_by_name<stream_type>(sstr.str().c_str()));
					if(bandwidth_gtkwave_init_script_file) (*bandwidth_gtkwave_init_script_file) << "lappend my_sig_list \"" << sstr.str() << "\"" << std::endl;
				}
			}
			if((*xbar_1_m1_concentrator)["enable-bandwidth-tracing"])
			{
				for(unsigned int i = 0; i < XBAR_1_M1_CONCENTRATOR_CONFIG::OUTPUT_SOCKETS; i++)
				{
					std::stringstream param_name_sstr;
					param_name_sstr << "output_socket_name_" << i;
					std::stringstream sstr;
					sstr << "HARDWARE.XBAR_1_M1_CONCENTRATOR." << (std::string)(*xbar_1_m1_concentrator)[param_name_sstr.str()] << "_bw";
					if(bandwidth_vcd) bandwidth_vcd->add(tracing::stream_by_name<stream_type>(sstr.str().c_str()));
					if(bandwidth_gtkwave_init_script_file) (*bandwidth_gtkwave_init_script_file) << "lappend my_sig_list \"" << sstr.str() << "\"" << std::endl;
				}
				for(unsigned int i = 0; i < XBAR_1_M1_CONCENTRATOR_CONFIG::INPUT_SOCKETS; i++)
				{
					std::stringstream param_name_sstr;
					param_name_sstr << "input_socket_name_" << i;
					std::stringstream sstr;
					sstr << "HARDWARE.XBAR_1_M1_CONCENTRATOR." << (std::string)(*xbar_1_m1_concentrator)[param_name_sstr.str()] << "_bw";
					if(bandwidth_vcd) bandwidth_vcd->add(tracing::stream_by_name<stream_type>(sstr.str().c_str()));
					if(bandwidth_gtkwave_init_script_file) (*bandwidth_gtkwave_init_script_file) << "lappend my_sig_list \"" << sstr.str() << "\"" << std::endl;
				}
			}
			if((*iahbg_0)["enable-bandwidth-tracing"])
			{
				for(unsigned int i = 0; i < IAHBG_0_CONFIG::OUTPUT_SOCKETS; i++)
				{
					std::stringstream param_name_sstr;
					param_name_sstr << "output_socket_name_" << i;
					std::stringstream sstr;
					sstr << "HARDWARE.IAHBG_0." << (std::string)(*iahbg_0)[param_name_sstr.str()] << "_bw";
					if(bandwidth_vcd) bandwidth_vcd->add(tracing::stream_by_name<stream_type>(sstr.str().c_str()));
					if(bandwidth_gtkwave_init_script_file) (*bandwidth_gtkwave_init_script_file) << "lappend my_sig_list \"" << sstr.str() << "\"" << std::endl;
				}
				for(unsigned int i = 0; i < IAHBG_0_CONFIG::INPUT_SOCKETS; i++)
				{
					std::stringstream param_name_sstr;
					param_name_sstr << "input_socket_name_" << i;
					std::stringstream sstr;
					sstr << "HARDWARE.IAHBG_0." << (std::string)(*iahbg_0)[param_name_sstr.str()] << "_bw";
					if(bandwidth_vcd) bandwidth_vcd->add(tracing::stream_by_name<stream_type>(sstr.str().c_str()));
					if(bandwidth_gtkwave_init_script_file) (*bandwidth_gtkwave_init_script_file) << "lappend my_sig_list \"" << sstr.str() << "\"" << std::endl;
				}
			}
			if((*iahbg_1)["enable-bandwidth-tracing"])
			{
				for(unsigned int i = 0; i < IAHBG_1_CONFIG::OUTPUT_SOCKETS; i++)
				{
					std::stringstream param_name_sstr;
					param_name_sstr << "output_socket_name_" << i;
					std::stringstream sstr;
					sstr << "HARDWARE.IAHBG_1." << (std::string)(*iahbg_1)[param_name_sstr.str()] << "_bw";
					if(bandwidth_vcd) bandwidth_vcd->add(tracing::stream_by_name<stream_type>(sstr.str().c_str()));
					if(bandwidth_gtkwave_init_script_file) (*bandwidth_gtkwave_init_script_file) << "lappend my_sig_list \"" << sstr.str() << "\"" << std::endl;
				}
				for(unsigned int i = 0; i < IAHBG_1_CONFIG::INPUT_SOCKETS; i++)
				{
					std::stringstream param_name_sstr;
					param_name_sstr << "input_socket_name_" << i;
					std::stringstream sstr;
					sstr << "HARDWARE.IAHBG_1." << (std::string)(*iahbg_1)[param_name_sstr.str()] << "_bw";
					if(bandwidth_vcd) bandwidth_vcd->add(tracing::stream_by_name<stream_type>(sstr.str().c_str()));
					if(bandwidth_gtkwave_init_script_file) (*bandwidth_gtkwave_init_script_file) << "lappend my_sig_list \"" << sstr.str() << "\"" << std::endl;
				}
			}
			if((*ebi)["enable-bandwidth-tracing"])
			{
				for(unsigned int i = 0; i < EBI_CONFIG::OUTPUT_SOCKETS; i++)
				{
					std::stringstream param_name_sstr;
					param_name_sstr << "output_socket_name_" << i;
					std::stringstream sstr;
					sstr << "HARDWARE.EBI." << (std::string)(*ebi)[param_name_sstr.str()] << "_bw";
					if(bandwidth_vcd) bandwidth_vcd->add(tracing::stream_by_name<stream_type>(sstr.str().c_str()));
					if(bandwidth_gtkwave_init_script_file) (*bandwidth_gtkwave_init_script_file) << "lappend my_sig_list \"" << sstr.str() << "\"" << std::endl;
				}
				for(unsigned int i = 0; i < EBI_CONFIG::INPUT_SOCKETS; i++)
				{
					std::stringstream param_name_sstr;
					param_name_sstr << "input_socket_name_" << i;
					std::stringstream sstr;
					sstr << "HARDWARE.EBI." << (std::string)(*ebi)[param_name_sstr.str()] << "_bw";
					if(bandwidth_vcd) bandwidth_vcd->add(tracing::stream_by_name<stream_type>(sstr.str().c_str()));
					if(bandwidth_gtkwave_init_script_file) (*bandwidth_gtkwave_init_script_file) << "lappend my_sig_list \"" << sstr.str() << "\"" << std::endl;
				}
			}
		}
		
		if(bandwidth_gtkwave_init_script_file)
		{
			(*bandwidth_gtkwave_init_script_file) << "set num_added [ gtkwave::addSignalsFromList $my_sig_list ]" << std::endl;
			(*bandwidth_gtkwave_init_script_file) << "# Analog data format" << std::endl;
			(*bandwidth_gtkwave_init_script_file) << "gtkwave::/Edit/Highlight_All" << std::endl;
			(*bandwidth_gtkwave_init_script_file) << "gtkwave::/Edit/Data_Format/Analog/Step" << std::endl;
			(*bandwidth_gtkwave_init_script_file) << "gtkwave::/Edit/UnHighlight_All" << std::endl;
			(*bandwidth_gtkwave_init_script_file) << "# Adjust zoom" << std::endl;
			(*bandwidth_gtkwave_init_script_file) << "gtkwave::/Time/Zoom/Zoom_Full" << std::endl;
			//(*bandwidth_gtkwave_init_script_file) << "gtkwave::setZoomFactor 34" << std::endl;
			
			delete bandwidth_gtkwave_init_script_file;
		}
	}
#endif
	
	SC_HAS_PROCESS(Simulator);
	
	SC_THREAD(Core0ResetProcess);
	SC_THREAD(Core1ResetProcess);
	SC_THREAD(Core2ResetProcess);
}

Simulator::~Simulator()
{
	unsigned int i;
	
	if(main_core_0) delete main_core_0;
	if(main_core_1) delete main_core_1;
	if(peripheral_core_2) delete peripheral_core_2;
	if(system_sram) delete system_sram;
	if(flash) delete flash;
	if(xbar_0) delete xbar_0;
	if(xbar_1) delete xbar_1;
	if(pbridge_a) delete pbridge_a;
	if(pbridge_b) delete pbridge_b;
	if(ebi) delete ebi;
	if(xbar_1_m1_concentrator) delete xbar_1_m1_concentrator;
	if(iahbg_0) delete iahbg_0;
	if(iahbg_1) delete iahbg_1;
	if(intc_0) delete intc_0;
	if(stm_0) delete stm_0;
	if(stm_1) delete stm_1;
	if(stm_2) delete stm_2;
	if(swt_0) delete swt_0;
	if(swt_1) delete swt_1;
	if(swt_2) delete swt_2;
	if(swt_3) delete swt_3;
	if(pit_0) delete pit_0;
	if(pit_1) delete pit_1;
	if(linflexd_0) delete linflexd_0; 
	if(linflexd_1) delete linflexd_1; 
	if(linflexd_2) delete linflexd_2; 
	if(linflexd_14) delete linflexd_14;
	if(linflexd_15) delete linflexd_15;
	if(linflexd_16) delete linflexd_16;
	if(linflexd_0_tx_serial_bus) delete linflexd_0_tx_serial_bus;
	if(linflexd_0_rx_serial_bus) delete linflexd_0_rx_serial_bus;
	if(linflexd_1_tx_serial_bus) delete linflexd_1_tx_serial_bus;
	if(linflexd_1_rx_serial_bus) delete linflexd_1_rx_serial_bus;
	if(linflexd_2_tx_serial_bus) delete linflexd_2_tx_serial_bus;
	if(linflexd_2_rx_serial_bus) delete linflexd_2_rx_serial_bus;
	if(linflexd_14_tx_serial_bus) delete linflexd_14_tx_serial_bus;
	if(linflexd_14_rx_serial_bus) delete linflexd_14_rx_serial_bus;
	if(linflexd_15_tx_serial_bus) delete linflexd_15_tx_serial_bus;
	if(linflexd_15_rx_serial_bus) delete linflexd_15_rx_serial_bus;
	if(linflexd_16_tx_serial_bus) delete linflexd_16_tx_serial_bus;
	if(linflexd_16_rx_serial_bus) delete linflexd_16_rx_serial_bus;
	if(serial_terminal0) delete serial_terminal0;
	if(serial_terminal1) delete serial_terminal1;
	if(serial_terminal2) delete serial_terminal2;
	if(serial_terminal14) delete serial_terminal14;
	if(serial_terminal15) delete serial_terminal15;
	if(serial_terminal16) delete serial_terminal16;
	if(dmamux_0) delete dmamux_0;
	if(dmamux_1) delete dmamux_1;
	if(dmamux_2) delete dmamux_2;
	if(dmamux_3) delete dmamux_3;
	if(dmamux_4) delete dmamux_4;
	if(dmamux_5) delete dmamux_5;
	if(dmamux_6) delete dmamux_6;
	if(dmamux_7) delete dmamux_7;
	if(dmamux_8) delete dmamux_8;
	if(dmamux_9) delete dmamux_9;
	if(edma_0) delete edma_0;
	if(edma_1) delete edma_1;
	if(dspi_0) delete dspi_0;
	if(dspi_1) delete dspi_1;
	if(dspi_2) delete dspi_2;
	if(dspi_3) delete dspi_3;
	if(dspi_4) delete dspi_4;
	if(dspi_5) delete dspi_5;
	if(dspi_6) delete dspi_6;
	if(dspi_12) delete dspi_12;
	if(dspi_0_sout_serial_bus) delete dspi_0_sout_serial_bus; 
	if(dspi_0_sin_serial_bus) delete dspi_0_sin_serial_bus; 
	for(i = 0; i < DSPI_0::NUM_CTARS; i++) if(dspi_0_pcs_serial_bus[i])  delete dspi_0_pcs_serial_bus[i];
	if(dspi_0_ss_serial_bus) delete dspi_0_ss_serial_bus;
	if(dspi_1_sout_serial_bus) delete dspi_1_sout_serial_bus; 
	if(dspi_1_sin_serial_bus) delete dspi_1_sin_serial_bus; 
	for(i = 0; i < DSPI_1::NUM_CTARS; i++) if(dspi_1_pcs_serial_bus[i])  delete dspi_1_pcs_serial_bus[i];
	if(dspi_1_ss_serial_bus) delete dspi_1_ss_serial_bus;
	if(dspi_2_sout_serial_bus) delete dspi_2_sout_serial_bus; 
	if(dspi_2_sin_serial_bus) delete dspi_2_sin_serial_bus; 
	for(i = 0; i < DSPI_2::NUM_CTARS; i++) if(dspi_2_pcs_serial_bus[i])  delete dspi_2_pcs_serial_bus[i];
	if(dspi_2_ss_serial_bus) delete dspi_2_ss_serial_bus;
	if(dspi_3_sout_serial_bus) delete dspi_3_sout_serial_bus; 
	if(dspi_3_sin_serial_bus) delete dspi_3_sin_serial_bus; 
	for(i = 0; i < DSPI_3::NUM_CTARS; i++) if(dspi_3_pcs_serial_bus[i])  delete dspi_3_pcs_serial_bus[i];
	if(dspi_3_ss_serial_bus) delete dspi_3_ss_serial_bus;
	if(dspi_4_sout_serial_bus) delete dspi_4_sout_serial_bus; 
	if(dspi_4_sin_serial_bus) delete dspi_4_sin_serial_bus; 
	for(i = 0; i < DSPI_4::NUM_CTARS; i++) if(dspi_4_pcs_serial_bus[i])  delete dspi_4_pcs_serial_bus[i];
	if(dspi_4_ss_serial_bus) delete dspi_4_ss_serial_bus;
	if(dspi_5_sout_serial_bus) delete dspi_5_sout_serial_bus; 
	if(dspi_5_sin_serial_bus) delete dspi_5_sin_serial_bus; 
	for(i = 0; i < DSPI_5::NUM_CTARS; i++) if(dspi_5_pcs_serial_bus[i])  delete dspi_5_pcs_serial_bus[i];
	if(dspi_5_ss_serial_bus) delete dspi_5_ss_serial_bus;
	if(dspi_6_sout_serial_bus) delete dspi_6_sout_serial_bus; 
	if(dspi_6_sin_serial_bus) delete dspi_6_sin_serial_bus; 
	for(i = 0; i < DSPI_6::NUM_CTARS; i++) if(dspi_6_pcs_serial_bus[i])  delete dspi_6_pcs_serial_bus[i];
	if(dspi_6_ss_serial_bus) delete dspi_6_ss_serial_bus;
	if(dspi_12_sout_serial_bus) delete dspi_12_sout_serial_bus; 
	if(dspi_12_sin_serial_bus) delete dspi_12_sin_serial_bus; 
	for(i = 0; i < DSPI_12::NUM_CTARS; i++) if(dspi_12_pcs_serial_bus[i]) delete dspi_12_pcs_serial_bus[i];
	if(dspi_12_ss_serial_bus) delete dspi_12_ss_serial_bus;
	if(siul2) delete siul2;
	//if(m_ttcan_0) delete m_ttcan_0;
	if(m_can_1) delete m_can_1;
	if(m_can_2) delete m_can_2;
	if(m_can_3) delete m_can_3;
	if(m_can_4) delete m_can_4;
	if(shared_can_message_ram_router) delete shared_can_message_ram_router;
	if(shared_can_message_ram) delete shared_can_message_ram;
	if(can_bus) delete can_bus;
	if(sema4) delete sema4;
	if(ebi_mem_0) delete ebi_mem_0;
	if(ebi_mem_1) delete ebi_mem_1;
	if(ebi_mem_2) delete ebi_mem_2;
	if(flash_port1_stub) delete flash_port1_stub;
	if(xbar_0_s6_stub) delete xbar_0_s6_stub;
	if(xbar_1_m2_stub) delete xbar_1_m2_stub;
	if(pcm_stub) delete pcm_stub;
	if(pflash_stub) delete pflash_stub;
	if(mc_me_stub) delete mc_me_stub;
	if(mc_cgm_stub) delete mc_cgm_stub;
	if(mc_rgm_stub) delete mc_rgm_stub;
	if(pram_0_stub) delete pram_0_stub;
	if(dma_err_irq_combinator) delete dma_err_irq_combinator;
	if(DSPI0_0) delete DSPI0_0;
	if(DSPI1_0) delete DSPI1_0;
	if(DSPI2_0) delete DSPI2_0;
	if(DSPI3_0) delete DSPI3_0;
	if(DSPI4_0) delete DSPI4_0;
	if(DSPI4_5) delete DSPI4_5;
	if(DSPI4_6) delete DSPI4_6;
	if(DSPI4_7) delete DSPI4_7;
	if(DSPI5_0) delete DSPI5_0;
	if(DSPI5_5) delete DSPI5_5;
	if(DSPI5_6) delete DSPI5_6;
	if(DSPI5_7) delete DSPI5_7;
	if(DSPI6_0) delete DSPI6_0;
	if(DSPI6_5) delete DSPI6_5;
	if(DSPI6_6) delete DSPI6_6;
	if(DSPI6_7) delete DSPI6_7;
	if(DSPI12_0) delete DSPI12_0;
	if(gdb_server[0]) delete gdb_server[0];
	if(gdb_server[1]) delete gdb_server[1];
	if(gdb_server[2]) delete gdb_server[2];
	if(inline_debugger[0]) delete inline_debugger[0];
	if(inline_debugger[1]) delete inline_debugger[1];
	if(inline_debugger[2]) delete inline_debugger[2];
	if(http_server) delete http_server;
	if(profiler[0]) delete profiler[0];
	if(profiler[1]) delete profiler[1];
	if(profiler[2]) delete profiler[2];
	if(debugger) delete debugger;
	if(sim_time) delete sim_time;
	if(host_time) delete host_time;
	if(loader) delete loader;
	if(netstreamer0) delete netstreamer0;
	if(netstreamer1) delete netstreamer1;
	if(netstreamer2) delete netstreamer2;
	if(netstreamer14) delete netstreamer14;
	if(netstreamer15) delete netstreamer15;
	if(netstreamer16) delete netstreamer16;
	if(char_io_tee0) delete char_io_tee0;
	if(char_io_tee1) delete char_io_tee1;
	if(char_io_tee2) delete char_io_tee2;
	if(char_io_tee14) delete char_io_tee14;
	if(char_io_tee15) delete char_io_tee15;
	if(char_io_tee16) delete char_io_tee16;
	if(web_terminal0) delete web_terminal0;
	if(web_terminal1) delete web_terminal1;
	if(web_terminal2) delete web_terminal2;
	if(web_terminal14) delete web_terminal14;
	if(web_terminal15) delete web_terminal15;
	if(web_terminal16) delete web_terminal16;
	if(instrumenter) delete instrumenter;
#if HAVE_TVS
	if(bandwidth_vcd) delete bandwidth_vcd;
	if(bandwidth_vcd_file) delete bandwidth_vcd_file;
#endif
	if(logger_console_printer) delete logger_console_printer;
	if(logger_text_file_writer) delete logger_text_file_writer;
	if(logger_http_writer) delete logger_http_writer;
	if(logger_xml_file_writer) delete logger_xml_file_writer;
	if(logger_netstream_writer) delete logger_netstream_writer;
}

void Simulator::Core0ResetProcess()
{
	if(enable_core0_reset)
	{
		sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS>& p_reset_b_0 = instrumenter->GetSignal<bool, sc_core::SC_MANY_WRITERS>("HARDWARE.p_reset_b_0");
		wait(core0_reset_time);
		p_reset_b_0 = false;
		wait(sc_core::sc_time(10.0, sc_core::SC_NS));
		p_reset_b_0 = true;
	}
}

void Simulator::Core1ResetProcess()
{
	if(enable_core1_reset)
	{
		sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS>& p_reset_b_1 = instrumenter->GetSignal<bool, sc_core::SC_MANY_WRITERS>("HARDWARE.p_reset_b_1");
		wait(core1_reset_time);
		p_reset_b_1 = false;
		wait(sc_core::sc_time(10.0, sc_core::SC_NS));
		p_reset_b_1 = true;
	}
}

void Simulator::Core2ResetProcess()
{
	if(enable_core2_reset)
	{
		sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS>& p_reset_b_2 = instrumenter->GetSignal<bool, sc_core::SC_MANY_WRITERS>("HARDWARE.p_reset_b_2");
		wait(core2_reset_time);
		p_reset_b_2 = false;
		wait(sc_core::sc_time(10.0, sc_core::SC_NS));
		p_reset_b_2 = true;
	}
}

void Simulator::InterruptSource(unsigned int irq_num, const std::string& source)
{
	std::stringstream irq_signal_name_sstr;
	irq_signal_name_sstr << "HARDWARE.irq_" << irq_num;
	if(!source.empty())
	{
		instrumenter->Bind(source, irq_signal_name_sstr.str());
	}
	std::stringstream intc_port_name_sstr;
	intc_port_name_sstr << "HARDWARE.INTC_0.hw_irq_" << (irq_num - INTC_0::NUM_SW_IRQS);
	instrumenter->Bind(intc_port_name_sstr.str() , irq_signal_name_sstr.str());
}

void Simulator::DMASource(unsigned int dmamux_num, unsigned int dma_source_num)
{
	std::stringstream source_req_signal_basename_sstr;
	source_req_signal_basename_sstr << "DMAMUX_" << dmamux_num << "_SOURCE_REQ_" << dma_source_num;
	instrumenter->CreateSignal(source_req_signal_basename_sstr.str(), false);

	std::stringstream dmamux_source_req_port_name_sstr;
	dmamux_source_req_port_name_sstr << this->sc_core::sc_object::name() << ".DMAMUX_" << dmamux_num << ".dma_source_" << dma_source_num;

	instrumenter->Bind(dmamux_source_req_port_name_sstr.str(), std::string(this->sc_core::sc_object::name()) + '.' + source_req_signal_basename_sstr.str());

	std::stringstream source_ack_signal_basename_sstr;
	source_ack_signal_basename_sstr << "DMAMUX_" << dmamux_num << "_SOURCE_ACK_" << dma_source_num;
	instrumenter->CreateSignal(source_ack_signal_basename_sstr.str(), false);

	std::stringstream dmamux_source_ack_port_name_sstr;
	dmamux_source_ack_port_name_sstr << this->sc_core::sc_object::name() << ".DMAMUX_" << dmamux_num << ".dma_source_ack_" << dma_source_num;

	instrumenter->Bind(dmamux_source_ack_port_name_sstr.str(), std::string(this->sc_core::sc_object::name()) + '.' + source_ack_signal_basename_sstr.str());
}

void Simulator::DMASource(const std::string& source_req,
                          const std::string& source_ack,
                          unsigned int dmamux_num,
                          unsigned int dma_source_num)
{
	std::stringstream source_req_signal_basename_sstr;
	source_req_signal_basename_sstr << "DMAMUX_" << dmamux_num << "_SOURCE_REQ_" << dma_source_num;
	instrumenter->CreateSignal(source_req_signal_basename_sstr.str(), false);

	std::stringstream dmamux_source_req_port_name_sstr;
	dmamux_source_req_port_name_sstr << this->sc_core::sc_object::name() << ".DMAMUX_" << dmamux_num << ".dma_source_" << dma_source_num;

	instrumenter->Bind(source_req, std::string(this->sc_core::sc_object::name()) + '.' + source_req_signal_basename_sstr.str());
	instrumenter->Bind(dmamux_source_req_port_name_sstr.str(), std::string(this->sc_core::sc_object::name()) + '.' + source_req_signal_basename_sstr.str());

	std::stringstream source_ack_signal_basename_sstr;
	source_ack_signal_basename_sstr << "DMAMUX_" << dmamux_num << "_SOURCE_ACK_" << dma_source_num;
	instrumenter->CreateSignal(source_ack_signal_basename_sstr.str(), false);

	std::stringstream dmamux_source_ack_port_name_sstr;
	dmamux_source_ack_port_name_sstr << this->sc_core::sc_object::name() << ".DMAMUX_" << dmamux_num << ".dma_source_ack_" << dma_source_num;

	instrumenter->Bind(dmamux_source_ack_port_name_sstr.str(), std::string(this->sc_core::sc_object::name()) + '.' + source_ack_signal_basename_sstr.str());
	instrumenter->Bind(source_ack, std::string(this->sc_core::sc_object::name()) + '.' + source_ack_signal_basename_sstr.str());
}

void Simulator::LogicalOr2(sc_core::sc_signal<bool>& in0, sc_core::sc_signal<bool>& in1, sc_core::sc_signal<bool>& out)
{
	out = in0 || in1;
}

void Simulator::LogicalOr3(sc_core::sc_signal<bool>& in0, sc_core::sc_signal<bool>& in1, sc_core::sc_signal<bool>& in2, sc_core::sc_signal<bool>& out)
{
	out = in0 || in1 || in2;
}

void Simulator::DMASource(const std::string& source_req,
                          const std::string& source_ack,
                          unsigned int dmamux_num0,
                          unsigned int dma_source_num0,
                          unsigned int dmamux_num1,
                          unsigned int dma_source_num1)
{
	std::stringstream source_req_signal_basename_sstr;
	source_req_signal_basename_sstr << "DMAMUX_" << dmamux_num0 << "_" << dmamux_num1 << "_SOURCE_REQ_" << dma_source_num0 << "_" << dma_source_num1;
	instrumenter->CreateSignal(source_req_signal_basename_sstr.str(), false);

	std::stringstream dmamux_source_req0_port_name_sstr;
	dmamux_source_req0_port_name_sstr << this->sc_core::sc_object::name() << ".DMAMUX_" << dmamux_num0 << ".dma_source_" << dma_source_num0;

	std::stringstream dmamux_source_req1_port_name_sstr;
	dmamux_source_req1_port_name_sstr << this->sc_core::sc_object::name() << ".DMAMUX_" << dmamux_num1 << ".dma_source_" << dma_source_num1;

	instrumenter->Bind(source_req, std::string(this->sc_core::sc_object::name()) + '.' + source_req_signal_basename_sstr.str());
	instrumenter->Bind(dmamux_source_req0_port_name_sstr.str(), std::string(this->sc_core::sc_object::name()) + '.' + source_req_signal_basename_sstr.str());
	instrumenter->Bind(dmamux_source_req1_port_name_sstr.str(), std::string(this->sc_core::sc_object::name()) + '.' + source_req_signal_basename_sstr.str());

	std::stringstream dmamux_ack0_signal_basename_sstr;
	dmamux_ack0_signal_basename_sstr << "DMAMUX_" << dmamux_num0 << "_DMA_SOURCE_ACK_" << dma_source_num0;
	
	std::stringstream dmamux_ack1_signal_basename_sstr;
	dmamux_ack1_signal_basename_sstr << "DMAMUX_" << dmamux_num1 << "_DMA_SOURCE_ACK_" << dma_source_num1;

	std::stringstream source_ack_signal_basename_sstr;
	source_ack_signal_basename_sstr << "DMAMUX_" << dmamux_num0 << "_" << dmamux_num1 << "_SOURCE_ACK_" << dma_source_num0 << "_" << dma_source_num1;
	
	sc_core::sc_signal<bool>& dmamux_ack0_signal = instrumenter->CreateSignal(dmamux_ack0_signal_basename_sstr.str(), false);
	sc_core::sc_signal<bool>& dmamux_ack1_signal = instrumenter->CreateSignal(dmamux_ack1_signal_basename_sstr.str(), false);
	sc_core::sc_signal<bool>& source_ack_signal = instrumenter->CreateSignal(source_ack_signal_basename_sstr.str(), false);

	std::stringstream dmamux_source_ack0_port_name_sstr;
	dmamux_source_ack0_port_name_sstr << this->sc_core::sc_object::name() << ".DMAMUX_" << dmamux_num0 << ".dma_source_ack_" << dma_source_num0;

	std::stringstream dmamux_source_ack1_port_name_sstr;
	dmamux_source_ack1_port_name_sstr << this->sc_core::sc_object::name() << ".DMAMUX_" << dmamux_num1 << ".dma_source_ack_" << dma_source_num1;

	instrumenter->Bind(dmamux_source_ack0_port_name_sstr.str(), std::string(this->sc_core::sc_object::name()) + '.' + dmamux_ack0_signal_basename_sstr.str());
	instrumenter->Bind(dmamux_source_ack1_port_name_sstr.str(), std::string(this->sc_core::sc_object::name()) + '.' + dmamux_ack1_signal_basename_sstr.str());

	sc_core::sc_spawn_options or_process_spawn_options;
	or_process_spawn_options.spawn_method();
	or_process_spawn_options.set_sensitivity(&dmamux_ack0_signal);
	or_process_spawn_options.set_sensitivity(&dmamux_ack1_signal);
	
	sc_core::sc_spawn(sc_bind(&Simulator::LogicalOr2, this, sc_ref(dmamux_ack0_signal), sc_ref(dmamux_ack1_signal), sc_ref(source_ack_signal)), sc_core::sc_gen_unique_name("Or2Process"), &or_process_spawn_options);

	instrumenter->Bind(source_ack, std::string(this->sc_core::sc_object::name()) + '.' + source_ack_signal_basename_sstr.str());
}

void Simulator::DMASource(const std::string& source_req,
                          const std::string& source_ack,
                          unsigned int dmamux_num0,
                          unsigned int dma_source_num0,
                          unsigned int dmamux_num1,
                          unsigned int dma_source_num1,
                          unsigned int dmamux_num2,
                          unsigned int dma_source_num2)
{
	std::stringstream source_req_signal_basename_sstr;
	source_req_signal_basename_sstr << "DMAMUX_" << dmamux_num0 << "_" << dmamux_num1 << "_" << dmamux_num2 << "_SOURCE_REQ_" << dma_source_num0 << "_" << dma_source_num1 << "_" << dma_source_num2;
	instrumenter->CreateSignal(source_req_signal_basename_sstr.str(), false);

	std::stringstream dmamux_source_req0_port_name_sstr;
	dmamux_source_req0_port_name_sstr << this->sc_core::sc_object::name() << ".DMAMUX_" << dmamux_num0 << ".dma_source_" << dma_source_num0;

	std::stringstream dmamux_source_req1_port_name_sstr;
	dmamux_source_req1_port_name_sstr << this->sc_core::sc_object::name() << ".DMAMUX_" << dmamux_num1 << ".dma_source_" << dma_source_num1;

	std::stringstream dmamux_source_req2_port_name_sstr;
	dmamux_source_req2_port_name_sstr << this->sc_core::sc_object::name() << ".DMAMUX_" << dmamux_num2 << ".dma_source_" << dma_source_num2;

	instrumenter->Bind(source_req, std::string(this->sc_core::sc_object::name()) + '.' + source_req_signal_basename_sstr.str());
	instrumenter->Bind(dmamux_source_req0_port_name_sstr.str(), std::string(this->sc_core::sc_object::name()) + '.' + source_req_signal_basename_sstr.str());
	instrumenter->Bind(dmamux_source_req1_port_name_sstr.str(), std::string(this->sc_core::sc_object::name()) + '.' + source_req_signal_basename_sstr.str());
	instrumenter->Bind(dmamux_source_req2_port_name_sstr.str(), std::string(this->sc_core::sc_object::name()) + '.' + source_req_signal_basename_sstr.str());

	std::stringstream dmamux_ack0_signal_basename_sstr;
	dmamux_ack0_signal_basename_sstr << "DMAMUX_" << dmamux_num0 << "_DMA_SOURCE_ACK_" << dma_source_num0;
	
	std::stringstream dmamux_ack1_signal_basename_sstr;
	dmamux_ack1_signal_basename_sstr << "DMAMUX_" << dmamux_num1 << "_DMA_SOURCE_ACK_" << dma_source_num1;

	std::stringstream dmamux_ack2_signal_basename_sstr;
	dmamux_ack2_signal_basename_sstr << "DMAMUX_" << dmamux_num2 << "_DMA_SOURCE_ACK_" << dma_source_num2;
	
	std::stringstream source_ack_signal_basename_sstr;
	source_ack_signal_basename_sstr << "DMAMUX_" << dmamux_num0 << "_" << dmamux_num1 << "_" << dmamux_num2 << "_SOURCE_ACK_" << dma_source_num0 << "_" << dma_source_num1 << "_" << dma_source_num2;

	sc_core::sc_signal<bool>& dmamux_ack0_signal = instrumenter->CreateSignal(dmamux_ack0_signal_basename_sstr.str(), false);
	sc_core::sc_signal<bool>& dmamux_ack1_signal = instrumenter->CreateSignal(dmamux_ack1_signal_basename_sstr.str(), false);
	sc_core::sc_signal<bool>& dmamux_ack2_signal = instrumenter->CreateSignal(dmamux_ack2_signal_basename_sstr.str(), false);
	sc_core::sc_signal<bool>& source_ack_signal = instrumenter->CreateSignal(source_ack_signal_basename_sstr.str(), false);
	
	std::stringstream dmamux_source_ack0_port_name_sstr;
	dmamux_source_ack0_port_name_sstr << this->sc_core::sc_object::name() << ".DMAMUX_" << dmamux_num0 << ".dma_source_ack_" << dma_source_num0;

	std::stringstream dmamux_source_ack1_port_name_sstr;
	dmamux_source_ack1_port_name_sstr << this->sc_core::sc_object::name() << ".DMAMUX_" << dmamux_num1 << ".dma_source_ack_" << dma_source_num1;

	std::stringstream dmamux_source_ack2_port_name_sstr;
	dmamux_source_ack2_port_name_sstr << this->sc_core::sc_object::name() << ".DMAMUX_" << dmamux_num2 << ".dma_source_ack_" << dma_source_num2;

	instrumenter->Bind(dmamux_source_ack0_port_name_sstr.str(), std::string(this->sc_core::sc_object::name()) + '.' + dmamux_ack0_signal_basename_sstr.str());
	instrumenter->Bind(dmamux_source_ack1_port_name_sstr.str(), std::string(this->sc_core::sc_object::name()) + '.' + dmamux_ack1_signal_basename_sstr.str());
	instrumenter->Bind(dmamux_source_ack2_port_name_sstr.str(), std::string(this->sc_core::sc_object::name()) + '.' + dmamux_ack2_signal_basename_sstr.str());

	sc_core::sc_spawn_options or_process_spawn_options;
	or_process_spawn_options.spawn_method();
	or_process_spawn_options.set_sensitivity(&dmamux_ack0_signal);
	or_process_spawn_options.set_sensitivity(&dmamux_ack1_signal);
	or_process_spawn_options.set_sensitivity(&dmamux_ack2_signal);
	
	sc_core::sc_spawn(sc_bind(&Simulator::LogicalOr3, this, sc_ref(dmamux_ack0_signal), sc_ref(dmamux_ack1_signal), sc_ref(dmamux_ack2_signal), sc_ref(source_ack_signal)), sc_core::sc_gen_unique_name("Or3Process"), &or_process_spawn_options);

	instrumenter->Bind(source_ack, std::string(this->sc_core::sc_object::name()) + '.' + source_ack_signal_basename_sstr.str());
}

void Simulator::LoadBuiltInConfig(unisim::kernel::Simulator *simulator)
{
	new unisim::kernel::config::xml::XMLConfigFileHelper(simulator);
	new unisim::kernel::config::ini::INIConfigFileHelper(simulator);
	new unisim::kernel::config::json::JSONConfigFileHelper(simulator);
	
	// meta information
	simulator->SetVariable("program-name", "UNISIM MPC5777M");
	simulator->SetVariable("copyright", "Copyright (C) 2007-2019, Commissariat à l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>, Yves Lhuillier <yves.lhuillier@cea.fr>, Franck Vedrine <franck.vedrine@cea.fr>, Réda Nouacer <reda.nouacer@cea.fr>, Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM MPC5777M, MPC5777M SoC simulator");
	simulator->SetVariable("schematic", "mpc5777m/fig_schematic.pdf");

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	//  - Clocks
	simulator->SetVariable("HARDWARE.EBI_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.EBI_CLK.clock-period", "10 ns");      // EBI clock: 100 Mhz
	simulator->SetVariable("HARDWARE.COMP_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.COMP_CLK.clock-period", "3333 ps");   // Computationnal Shell: 300 MHz
	simulator->SetVariable("HARDWARE.IOP_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.IOP_CLK.clock-period", "5 ns");       // I/O processor: 200 MHz
	simulator->SetVariable("HARDWARE.FXBAR_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.FXBAR_CLK.clock-period", "5 ns");     // Fast crossbar: 200 Mhz
	simulator->SetVariable("HARDWARE.SXBAR_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.SXBAR_CLK.clock-period", "10 ns");    // Slow crossbar: 100 Mhz
	simulator->SetVariable("HARDWARE.PBRIDGEA_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.PBRIDGEA_CLK.clock-period", "20 ns"); // PBRIDGE_A: 50 Mhz
	simulator->SetVariable("HARDWARE.PBRIDGEB_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.PBRIDGEB_CLK.clock-period", "20 ns"); // PBRIDGE_B: 50 Mhz
	simulator->SetVariable("HARDWARE.PER_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.PER_CLK.clock-period", "12500 ps");   // PER_CLK: 80 Mhz
	simulator->SetVariable("HARDWARE.RTI_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.RTI_CLK.clock-period", "1 us");       // RTI_CLK: 1 Mhz
	simulator->SetVariable("HARDWARE.LIN_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.LIN_CLK.clock-period", "12500 ps");      // LIN_CLK: 80 Mhz ((2/3) * LIN_CLK > PBRIDGEx_CLK > (1/3) * LIN_CLK)
	simulator->SetVariable("HARDWARE.SERIAL_TERMINAL0_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.SERIAL_TERMINAL0_CLK.clock-period", "4340278 ps"); // SERIAL_TERMINAL0_CLK: 230400 baud
	simulator->SetVariable("HARDWARE.SERIAL_TERMINAL1_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.SERIAL_TERMINAL1_CLK.clock-period", "4340278 ps"); // SERIAL_TERMINAL1_CLK: 230400 baud
	simulator->SetVariable("HARDWARE.SERIAL_TERMINAL2_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.SERIAL_TERMINAL2_CLK.clock-period", "4340278 ps"); // SERIAL_TERMINAL2_CLK: 230400 baud
	simulator->SetVariable("HARDWARE.SERIAL_TERMINAL14_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.SERIAL_TERMINAL14_CLK.clock-period", "4340278 ps"); // SERIAL_TERMINAL14_CLK: 230400 baud
	simulator->SetVariable("HARDWARE.SERIAL_TERMINAL15_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.SERIAL_TERMINAL15_CLK.clock-period", "4340278 ps"); // SERIAL_TERMINAL15_CLK: 230400 baud
	simulator->SetVariable("HARDWARE.SERIAL_TERMINAL16_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.SERIAL_TERMINAL16_CLK.clock-period", "4340278 ps"); // SERIAL_TERMINAL16_CLK: 230400 baud
	simulator->SetVariable("HARDWARE.DSPI_CLK0.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.DSPI_CLK0.clock-period", "12500 ps");               // DSPI_CLK0: 80 Mhz
	simulator->SetVariable("HARDWARE.DSPI_CLK1.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.DSPI_CLK1.clock-period", "12500 ps");               // DSPI_CLK1: 80 Mhz
	simulator->SetVariable("HARDWARE.CAN_CLK.lazy-clock", "true");
	simulator->SetVariable("HARDWARE.CAN_CLK.clock-period", "25 ns");                   // CAN_CLK: 40 Mhz

	// TLM-2.0 global quantum
	simulator->SetVariable("HARDWARE.global-quantum", "200 ns");
	
	// TLM CAN global quantum
	simulator->SetVariable("HARDWARE.can-global-quantum", "100 us");
  
	//  - e200 PowerPC cores

	// Peripheral_Core_2
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.ahb-master-id", 2);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.clock-multiplier", 1.0);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.max-inst", ~uint64_t(0));
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.ipc", 2.0);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.enable-dmi", false);

	simulator->SetVariable("HARDWARE.Peripheral_Core_2.local-memory-base-addr", 0x52000000);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.local-memory-size", 8 * 1024 * 1024);

	simulator->SetVariable("HARDWARE.Peripheral_Core_2.DMEM.base-addr", 0x52800000);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.DMEM.size", 64 * 1024);

	simulator->SetVariable("HARDWARE.Peripheral_Core_2.IMEM.base-addr", 0x52000000);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.IMEM.size", 16 * 1024);
	
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.processor-version", 0x815f8000);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.system-version", 0x0);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.system-information", 0x2);
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.cpuid", 0x2);

	//FIXME: reset address
	simulator->SetVariable("HARDWARE.Peripheral_Core_2.reset-addr", 0x1500000);
	simulator->SetVariable("HARDWARE.Main_Core_0.reset-addr", 0x1000000);
	simulator->SetVariable("HARDWARE.Main_Core_1.reset-addr", 0x1280000);
	
	// Main_Core_0
	simulator->SetVariable("HARDWARE.Main_Core_0.ahb-master-id", 0);
	simulator->SetVariable("HARDWARE.Main_Core_0.clock-multiplier", 1.0);
	simulator->SetVariable("HARDWARE.Main_Core_0.max-inst", ~uint64_t(0));
	simulator->SetVariable("HARDWARE.Main_Core_0.ipc", 1.0);
	simulator->SetVariable("HARDWARE.Main_Core_0.enable-dmi", false);

	simulator->SetVariable("HARDWARE.Main_Core_0.local-memory-base-addr", 0x50000000);
	simulator->SetVariable("HARDWARE.Main_Core_0.local-memory-size", 8 * 1024 * 1024);

	simulator->SetVariable("HARDWARE.Main_Core_0.DMEM.base-addr", 0x50800000);
	simulator->SetVariable("HARDWARE.Main_Core_0.DMEM.size", 64 * 1024);

	simulator->SetVariable("HARDWARE.Main_Core_0.IMEM.base-addr", 0x50000000);
	simulator->SetVariable("HARDWARE.Main_Core_0.IMEM.size", 16 * 1024);
	
	simulator->SetVariable("HARDWARE.Main_Core_0.processor-version", 0x81b00000);
	simulator->SetVariable("HARDWARE.Main_Core_0.system-version", 0x0);
	simulator->SetVariable("HARDWARE.Main_Core_0.system-information", 0x0);
	simulator->SetVariable("HARDWARE.Main_Core_0.cpuid", 0x0);
	
	// Main_Core_1
	simulator->SetVariable("HARDWARE.Main_Core_1.ahb-master-id", 1);
	simulator->SetVariable("HARDWARE.Main_Core_1.clock-multiplier", 1.0);
	simulator->SetVariable("HARDWARE.Main_Core_1.max-inst", ~uint64_t(0));
	simulator->SetVariable("HARDWARE.Main_Core_1.ipc", 1.0);
	simulator->SetVariable("HARDWARE.Main_Core_1.enable-dmi", false);

	simulator->SetVariable("HARDWARE.Main_Core_1.local-memory-base-addr", 0x51000000);
	simulator->SetVariable("HARDWARE.Main_Core_1.local-memory-size", 8 * 1024 * 1024);

	simulator->SetVariable("HARDWARE.Main_Core_1.DMEM.base-addr", 0x51800000);
	simulator->SetVariable("HARDWARE.Main_Core_1.DMEM.size", 64 * 1024);

	simulator->SetVariable("HARDWARE.Main_Core_1.IMEM.base-addr", 0x51000000);
	simulator->SetVariable("HARDWARE.Main_Core_1.IMEM.size", 16 * 1024);
	
	simulator->SetVariable("HARDWARE.Main_Core_1.processor-version", 0x81b00000);
	simulator->SetVariable("HARDWARE.Main_Core_1.system-version", 0x0);
	simulator->SetVariable("HARDWARE.Main_Core_1.system-information", 0x1);
	simulator->SetVariable("HARDWARE.Main_Core_1.cpuid", 0x1);

	//  - XBAR_0
	simulator->SetVariable("HARDWARE.XBAR_0.input_socket_name_0", "M0");
	simulator->SetVariable("HARDWARE.XBAR_0.input_socket_name_1", "M1");
	simulator->SetVariable("HARDWARE.XBAR_0.input_socket_name_2", "M2");
	simulator->SetVariable("HARDWARE.XBAR_0.input_socket_name_3", "M3");
	simulator->SetVariable("HARDWARE.XBAR_0.input_socket_name_4", "M4");
	simulator->SetVariable("HARDWARE.XBAR_0.input_socket_name_5", "M5");
	
	simulator->SetVariable("HARDWARE.XBAR_0.output_socket_name_0", "S0");
	simulator->SetVariable("HARDWARE.XBAR_0.output_socket_name_1", "S1");
	simulator->SetVariable("HARDWARE.XBAR_0.output_socket_name_2", "S2");
	simulator->SetVariable("HARDWARE.XBAR_0.output_socket_name_3", "S3");
	simulator->SetVariable("HARDWARE.XBAR_0.output_socket_name_4", "S4");
	simulator->SetVariable("HARDWARE.XBAR_0.output_socket_name_5", "S5");
	simulator->SetVariable("HARDWARE.XBAR_0.output_socket_name_6", "S6");
	simulator->SetVariable("HARDWARE.XBAR_0.output_socket_name_7", "S7");

	simulator->SetVariable("HARDWARE.XBAR_0.cycle_time", "5 ns");
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_0",  "range_start=\"0x00400000\" range_end=\"0x00407fff\" output_port=\"0\" translation=\"0x0\"");        //   0 KB: UTest NVM Block (32 KB)         -> FLASH (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_1",  "range_start=\"0x0060c000\" range_end=\"0x0062ffff\" output_port=\"0\" translation=\"0x8000\"");     //  32 KB: HSM Code (144 KB)               -> FLASH (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_2",  "range_start=\"0x00680000\" range_end=\"0x00687fff\" output_port=\"0\" translation=\"0x2c000\"");    // 176 KB: HSM Data (32 KB)                -> FLASH (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_3",  "range_start=\"0x00800000\" range_end=\"0x0087ffff\" output_port=\"0\" translation=\"0x34000\"");    // 208 KB: Data Flash (512 KB)             -> FLASH (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_4",  "range_start=\"0x00fc0000\" range_end=\"0x00ffffff\" output_port=\"0\" translation=\"0xb4000\"");    // 720 KB: Low & Mid Flash Blocks (256 KB) -> FLASH (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_5",  "range_start=\"0x01000000\" range_end=\"0x0177ffff\" output_port=\"0\" translation=\"0xf4000\"");    // 976 KB: Large Flash Blocks (7680 KB)    -> FLASH (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_6",  "range_start=\"0x50000000\" range_end=\"0x50ffffff\" output_port=\"2\" translation=\"0x0\"");        // Main_Core_0 Local Memory                -> Main_Core_0 (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_7",  "range_start=\"0x51000000\" range_end=\"0x51ffffff\" output_port=\"3\" translation=\"0x0\"");        // Main_Core_1 Local Memory                -> Main_Core_1 (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_8",  "range_start=\"0x40000000\" range_end=\"0x401fffff\" output_port=\"4\" translation=\"0x0\"");        // System SRAM                             -> SYSTEM_SRAM (rel address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_9",  "range_start=\"0x52000000\" range_end=\"0x5fffffff\" output_port=\"7\" translation=\"0x52000000\""); // Peripheral_Core_2 Local Memory          -> XBAR_1 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_10", "range_start=\"0xf0000000\" range_end=\"0xffffffff\" output_port=\"7\" translation=\"0xf0000000\""); // Peripherals PBRIDGE_A, PBRIDGE_B        -> XBAR_1 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_0.mapping_11", "range_start=\"0x20000000\" range_end=\"0x2fffffff\" output_port=\"5\" translation=\"0x20000000\""); // EBI                                     -> EBI (abs address)
	
	//  - XBAR_1
	simulator->SetVariable("HARDWARE.XBAR_1.input_socket_name_0", "M0");
	simulator->SetVariable("HARDWARE.XBAR_1.input_socket_name_1", "M1");
	simulator->SetVariable("HARDWARE.XBAR_1.input_socket_name_2", "M2");
	simulator->SetVariable("HARDWARE.XBAR_1.input_socket_name_3", "M3");
	
	simulator->SetVariable("HARDWARE.XBAR_1.output_socket_name_0", "S0");
	simulator->SetVariable("HARDWARE.XBAR_1.output_socket_name_1", "S1");
	simulator->SetVariable("HARDWARE.XBAR_1.output_socket_name_2", "S2");
	simulator->SetVariable("HARDWARE.XBAR_1.output_socket_name_3", "S3");

	simulator->SetVariable("HARDWARE.XBAR_1.cycle_time", "10 ns");
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_0",  "range_start=\"0x00400000\" range_end=\"0x00407fff\" output_port=\"0\" translation=\"0x00400000\""); // UTest NVM Block (32 KB)         -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_1",  "range_start=\"0x0060c000\" range_end=\"0x0062ffff\" output_port=\"0\" translation=\"0x0060c000\""); // HSM Code (144 KB)               -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_2",  "range_start=\"0x00680000\" range_end=\"0x00687fff\" output_port=\"0\" translation=\"0x00680000\""); // HSM Data (32 KB)                -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_3",  "range_start=\"0x00800000\" range_end=\"0x0087ffff\" output_port=\"0\" translation=\"0x00800000\""); // Data Flash (512 KB)             -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_4",  "range_start=\"0x00fc0000\" range_end=\"0x00ffffff\" output_port=\"0\" translation=\"0x00fc0000\""); // Low & Mid Flash Blocks (256 KB) -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_5",  "range_start=\"0x01000000\" range_end=\"0x0177ffff\" output_port=\"0\" translation=\"0x01000000\""); // Large Flash Blocks (7680 KB)    -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_6",  "range_start=\"0x50000000\" range_end=\"0x50ffffff\" output_port=\"0\" translation=\"0x50000000\""); // Main_Core_0 Local Memory        -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_7",  "range_start=\"0x51000000\" range_end=\"0x51ffffff\" output_port=\"0\" translation=\"0x51000000\""); // Main_Core_1 Local Memory        -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_8",  "range_start=\"0x40000000\" range_end=\"0x401fffff\" output_port=\"0\" translation=\"0x40000000\""); // System SRAM                     -> XBAR_0 (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_9",  "range_start=\"0x52000000\" range_end=\"0x5fffffff\" output_port=\"1\" translation=\"0x0\"");        // Peripheral_Core_2 Local Memory  -> Peripheral_Core_2 (rel address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_10", "range_start=\"0xf0000000\" range_end=\"0xfbffffff\" output_port=\"2\" translation=\"0xf0000000\""); // PBRIDGE_B                       -> PBRIDGE_B (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_11", "range_start=\"0xfc000000\" range_end=\"0xffffffff\" output_port=\"3\" translation=\"0xfc000000\""); // PBRIDGE_A                       -> PBRIDGE_B (abs address)
	simulator->SetVariable("HARDWARE.XBAR_1.mapping_12", "range_start=\"0x20000000\" range_end=\"0x2fffffff\" output_port=\"0\" translation=\"0x20000000\""); // EBI                             -> XBAR_0 (abs address)
	
	//  - IAHBG_0
	simulator->SetVariable("HARDWARE.IAHBG_0.input_socket_name_0", "XBAR_0_S7");
	simulator->SetVariable("HARDWARE.IAHBG_0.output_socket_name_0", "XBAR_1_M3");
	
	simulator->SetVariable("HARDWARE.IAHBG_0.cycle_time", "0 ns");
	simulator->SetVariable("HARDWARE.IAHBG_0.mapping_0", "range_start=\"0x0\" range_end=\"0xffffffff\" output_port=\"0\" translation=\"0x0\"");
	
	//  - IAHBG_1
	simulator->SetVariable("HARDWARE.IAHBG_1.input_socket_name_0", "XBAR_1_S0");
	simulator->SetVariable("HARDWARE.IAHBG_1.output_socket_name_0", "XBAR_0_M4");

	simulator->SetVariable("HARDWARE.IAHBG_1.cycle_time", "0 ns");
	simulator->SetVariable("HARDWARE.IAHBG_1.mapping_0", "range_start=\"0x0\" range_end=\"0xffffffff\" output_port=\"0\" translation=\"0x0\"");

	//  - PBRIDGE_A
	simulator->SetVariable("HARDWARE.PBRIDGE_A.input_socket_name_0", "XBAR_1_S3");
	
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_0" , "INTC");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_1" , "STM_0");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_2" , "STM_1");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_3" , "STM_2");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_4" , "SWT_0");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_5" , "SWT_1");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_6" , "SWT_2");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_7" , "SWT_3");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_8" , "PIT_0");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_9" , "PIT_1");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_10", "LINFlexD_0");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_11", "LINFlexD_1");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_12", "LINFlexD_14");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_13", "LINFlexD_16");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_14", "DMAMUX_0");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_15", "DMAMUX_1");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_16", "DMAMUX_2");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_17", "DMAMUX_3");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_18", "DMAMUX_4");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_19", "DMAMUX_5");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_20", "DMAMUX_6");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_21", "DMAMUX_7");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_22", "DMAMUX_8");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_23", "DMAMUX_9");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_24", "EDMA_0");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_25", "EDMA_1");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_26", "DSPI_0");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_27", "DSPI_1");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_28", "DSPI_4");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_29", "DSPI_6");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_30", "DSPI_12");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_31", "PCM");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_32", "PFLASH");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_33", "MC_ME");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_34", "MC_CGM");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_35", "XBAR_0");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_36", "XBAR_1");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_37", "PBRIDGE_A");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_38", "SIUL2");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_39", "EBI");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_40", "SMPU_0");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_41", "SMPU_1");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_42", "MC_RGM");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_43", "PRAM_0");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_44", "M_CAN_1");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_45", "M_CAN_2");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_46", "M_CAN_3");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_47", "M_CAN_4");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_48", "SHARED_CAN_MESSAGE_RAM");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.output_socket_name_49", "SEMA4");
	
	simulator->SetVariable("HARDWARE.PBRIDGE_A.cycle_time", "20 ns");
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_0",  "range_start=\"0xfc040000\" range_end=\"0xfc04ffff\" output_port=\"0\"  translation=\"0x0\""); // INTC        -> INTC  (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_1",  "range_start=\"0xfc068000\" range_end=\"0xfc06bfff\" output_port=\"1\"  translation=\"0x0\""); // STM_0       -> STM_0 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_2",  "range_start=\"0xfc06c000\" range_end=\"0xfc06ffff\" output_port=\"2\"  translation=\"0x0\""); // STM_1       -> STM_1 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_3",  "range_start=\"0xfc070000\" range_end=\"0xfc073fff\" output_port=\"3\"  translation=\"0x0\""); // STM_2       -> STM_2 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_4",  "range_start=\"0xfc050000\" range_end=\"0xfc053fff\" output_port=\"4\"  translation=\"0x0\""); // SWT_0       -> SWT_0 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_5",  "range_start=\"0xfc054000\" range_end=\"0xfc057fff\" output_port=\"5\"  translation=\"0x0\""); // SWT_1       -> SWT_1 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_6",  "range_start=\"0xfc058000\" range_end=\"0xfc05bfff\" output_port=\"6\"  translation=\"0x0\""); // SWT_2       -> SWT_2 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_7",  "range_start=\"0xfc05c000\" range_end=\"0xfc05ffff\" output_port=\"7\"  translation=\"0x0\""); // SWT_3       -> SWT_3 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_8",  "range_start=\"0xfff84000\" range_end=\"0xfff87fff\" output_port=\"8\"  translation=\"0x0\""); // PIT_0       -> PIT_0 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_9",  "range_start=\"0xfff80000\" range_end=\"0xfff83fff\" output_port=\"9\"  translation=\"0x0\""); // PIT_1       -> PIT_1 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_10", "range_start=\"0xffe8c000\" range_end=\"0xffe8ffff\" output_port=\"10\" translation=\"0x0\""); // LINFlexD_0  -> LINFlexD_0 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_11", "range_start=\"0xffe90000\" range_end=\"0xffe93fff\" output_port=\"11\" translation=\"0x0\""); // LINFlexD_1  -> LINFlexD_1 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_12", "range_start=\"0xffea8000\" range_end=\"0xffeabfff\" output_port=\"12\" translation=\"0x0\""); // LINFlexD_14 -> LINFlexD_14 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_13", "range_start=\"0xffeac000\" range_end=\"0xffeaffff\" output_port=\"13\" translation=\"0x0\""); // LINFlexD_16 -> LINFlexD_16 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_14", "range_start=\"0xfff6c000\" range_end=\"0xfff6c1ff\" output_port=\"14\" translation=\"0x0\""); // DMAMUX_0    -> DMAMUX_0 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_15", "range_start=\"0xfff6c200\" range_end=\"0xfff6c3ff\" output_port=\"15\" translation=\"0x0\""); // DMAMUX_1    -> DMAMUX_1 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_16", "range_start=\"0xfff6c400\" range_end=\"0xfff6c5ff\" output_port=\"16\" translation=\"0x0\""); // DMAMUX_2    -> DMAMUX_2 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_17", "range_start=\"0xfff6c600\" range_end=\"0xfff6c7ff\" output_port=\"17\" translation=\"0x0\""); // DMAMUX_3    -> DMAMUX_3 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_18", "range_start=\"0xfff6c800\" range_end=\"0xfff6c9ff\" output_port=\"18\" translation=\"0x0\""); // DMAMUX_4    -> DMAMUX_4 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_19", "range_start=\"0xfff6ca00\" range_end=\"0xfff6cbff\" output_port=\"19\" translation=\"0x0\""); // DMAMUX_5    -> DMAMUX_5 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_20", "range_start=\"0xfff6cc00\" range_end=\"0xfff6cdff\" output_port=\"20\" translation=\"0x0\""); // DMAMUX_6    -> DMAMUX_6 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_21", "range_start=\"0xfff6ce00\" range_end=\"0xfff6cfff\" output_port=\"21\" translation=\"0x0\""); // DMAMUX_7    -> DMAMUX_7 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_22", "range_start=\"0xfff6d000\" range_end=\"0xfff6d1ff\" output_port=\"22\" translation=\"0x0\""); // DMAMUX_8    -> DMAMUX_8 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_23", "range_start=\"0xfff6d200\" range_end=\"0xfff6d3ff\" output_port=\"23\" translation=\"0x0\""); // DMAMUX_9    -> DMAMUX_9 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_24", "range_start=\"0xfc0a0000\" range_end=\"0xfc0a3fff\" output_port=\"24\" translation=\"0x0\""); // EDMA_0      -> EDMA_0   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_25", "range_start=\"0xfc0a4000\" range_end=\"0xfc0a7fff\" output_port=\"25\" translation=\"0x0\""); // EDMA_1      -> EDMA_1   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_26", "range_start=\"0xffe70000\" range_end=\"0xffe73fff\" output_port=\"26\" translation=\"0x0\""); // DSPI_0      -> DSPI_0   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_27", "range_start=\"0xffe74000\" range_end=\"0xffe77fff\" output_port=\"27\" translation=\"0x0\""); // DSPI_1      -> DSPI_1   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_28", "range_start=\"0xffe78000\" range_end=\"0xffe7bfff\" output_port=\"28\" translation=\"0x0\""); // DSPI_4      -> DSPI_4   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_29", "range_start=\"0xffe7c000\" range_end=\"0xffe7ffff\" output_port=\"29\" translation=\"0x0\""); // DSPI_6      -> DSPI_6   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_30", "range_start=\"0xffe88000\" range_end=\"0xffe8bfff\" output_port=\"30\" translation=\"0x0\""); // DSPI_12     -> DSPI_12  (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_31", "range_start=\"0xfc028000\" range_end=\"0xfc02bfff\" output_port=\"31\" translation=\"0x0\""); // PCM         -> PCM      (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_32", "range_start=\"0xfc030000\" range_end=\"0xfc033fff\" output_port=\"32\" translation=\"0x0\""); // PFLASH      -> PFLASH   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_33", "range_start=\"0xfffb8000\" range_end=\"0xfffbbfff\" output_port=\"33\" translation=\"0x0\""); // MC_ME       -> MC_ME    (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_34", "range_start=\"0xfffb0000\" range_end=\"0xfffb3fff\" output_port=\"34\" translation=\"0x0\""); // MC_CGM      -> MC_CGM   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_35", "range_start=\"0xfc004000\" range_end=\"0xfc007fff\" output_port=\"35\" translation=\"0x0\""); // XBAR_0      -> XBAR_0   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_36", "range_start=\"0xfc008000\" range_end=\"0xfc00bfff\" output_port=\"36\" translation=\"0x0\""); // XBAR_1      -> XBAR_1   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_37", "range_start=\"0xfc000000\" range_end=\"0xfc003fff\" output_port=\"37\" translation=\"0x0\""); // PBRIDGE_A   -> PBRIDGE_A (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_38", "range_start=\"0xfffc0000\" range_end=\"0xfffc3fff\" output_port=\"38\" translation=\"0x0\""); // SIUL2       -> SIUL2    (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_39", "range_start=\"0xffff0000\" range_end=\"0xffff3fff\" output_port=\"39\" translation=\"0x0\""); // EBI         -> EBI      (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_40", "range_start=\"0xfc010000\" range_end=\"0xfc013fff\" output_port=\"40\" translation=\"0x0\""); // SMPU_0      -> SMPU_0   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_41", "range_start=\"0xfc014000\" range_end=\"0xfc017fff\" output_port=\"41\" translation=\"0x0\""); // SMPU_1      -> SMPU_1   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_42", "range_start=\"0xfffa8000\" range_end=\"0xfffabfff\" output_port=\"42\" translation=\"0x0\""); // MC_RGM      -> MC_RGM   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_43", "range_start=\"0xfc020000\" range_end=\"0xfc023fff\" output_port=\"43\" translation=\"0x0\""); // PRAM_0      -> PRAM_0   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_44", "range_start=\"0xffee4000\" range_end=\"0xffee7fff\" output_port=\"44\" translation=\"0x0\"");  // M_CAN_1    -> M_CAN_1   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_45", "range_start=\"0xffee8000\" range_end=\"0xffeebfff\" output_port=\"45\" translation=\"0x0\"");  // M_CAN_2    -> M_CAN_2   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_46", "range_start=\"0xffeec000\" range_end=\"0xffeeffff\" output_port=\"46\" translation=\"0x0\"");  // M_CAN_3    -> M_CAN_3   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_47", "range_start=\"0xffef0000\" range_end=\"0xffef3fff\" output_port=\"47\" translation=\"0x0\"");  // M_CAN_4    -> M_CAN_4   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_48", "range_start=\"0xffed4000\" range_end=\"0xffed7fff\" output_port=\"48\" translation=\"0x0\"");  // Shared CAN message RAM -> Shared CAN message RAM   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_49", "range_start=\"0xffed8000\" range_end=\"0xffedbfff\" output_port=\"48\" translation=\"0x4000\"");  // Shared CAN message RAM (extended) -> Shared CAN message RAM (extended)   (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.mapping_50", "range_start=\"0xfc03c000\" range_end=\"0xfc03ffff\" output_port=\"49\" translation=\"0x0\"");  // SEMA4      -> SEMA4   (rel address)

	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_0", "pacr16");   // INTC_0
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_1", "pacr26");   // STM_0
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_2", "pacr27");   // STM_1
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_3", "pacr28");   // STM_2
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_4", "pacr20");   // SWT_0
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_5", "pacr21");   // SWT_1
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_6", "pacr22");   // SWT_2
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_7", "pacr23");   // SWT_3
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_8", "opacr30");  // PIT_0
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_9", "opacr31");  // PIT_1
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_10", "opacr92"); // LINFlexD_0
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_11", "opacr91"); // LINFlexD_1
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_12", "opacr85"); // LINFlexD_14
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_13", "opacr84"); // LINFlexD_16
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_14", "opacr36"); // DMAMUX_0
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_15", "opacr36"); // DMAMUX_1
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_16", "opacr36"); // DMAMUX_2
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_17", "opacr36"); // DMAMUX_3
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_18", "opacr36"); // DMAMUX_4
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_19", "opacr36"); // DMAMUX_5
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_20", "opacr36"); // DMAMUX_6
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_21", "opacr36"); // DMAMUX_7
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_22", "opacr36"); // DMAMUX_8
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_23", "opacr36"); // DMAMUX_9
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_24", "pacr40");  // EDMA_0
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_25", "pacr41");  // EDMA_1
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_26", "opacr99"); // DSPI_0
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_27", "opacr98"); // DSPI_1
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_28", "opacr97"); // DSPI_4
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_29", "opacr96"); // DSPI_6
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_30", "opacr93"); // DSPI_12
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_31", "pacr10");  // PCM
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_32", "pacr12");  // PFLASH
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_33", "opacr17"); // MC_ME
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_34", "opacr19"); // MC_CGM
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_35", "pacr1");   // XBAR_0
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_36", "pacr2");   // XBAR_1
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_37", "pacr0");   // PBRIDGE_A
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_38", "opacr15"); // SIUL2
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_39", "opacr3");  // EBI
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_40", "pacr4");   // SMPU_0
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_41", "pacr5");   // SMPU_1
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_42", "opacr21"); // MC_RGM
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_43", "pacr8");   // PRAM_0
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_44", "opacr70"); // M_CAN_1
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_45", "opacr69"); // M_CAN_2
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_46", "opacr68"); // M_CAN_3
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_47", "opacr67"); // M_CAN_4
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_48", "opacr74"); // Shared CAN Message RAM
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_49", "opacr73"); // Shared CAN Message RAM (extended)
	simulator->SetVariable("HARDWARE.PBRIDGE_A.acr_mapping_50", "pacr15");  // SEMA4
	
	//  - PBRIDGE_B
	simulator->SetVariable("HARDWARE.PBRIDGE_B.input_socket_name_0", "XBAR_1_S2");
	
	simulator->SetVariable("HARDWARE.PBRIDGE_B.output_socket_name_0", "LINFlexD_2");
	simulator->SetVariable("HARDWARE.PBRIDGE_B.output_socket_name_1", "LINFlexD_15");
	simulator->SetVariable("HARDWARE.PBRIDGE_B.output_socket_name_2", "DSPI_2");
	simulator->SetVariable("HARDWARE.PBRIDGE_B.output_socket_name_3", "DSPI_3");
	simulator->SetVariable("HARDWARE.PBRIDGE_B.output_socket_name_4", "DSPI_5");
	simulator->SetVariable("HARDWARE.PBRIDGE_B.output_socket_name_5", "PBRIDGE_B");
	
	simulator->SetVariable("HARDWARE.PBRIDGE_B.cycle_time", "20 ns");
	simulator->SetVariable("HARDWARE.PBRIDGE_B.mapping_0",  "range_start=\"0xfbe8c000\" range_end=\"0xfbe8ffff\" output_port=\"0\" translation=\"0x0\""); // LINFlexD_2  -> LINFlexD_2 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_B.mapping_1",  "range_start=\"0xfbea8000\" range_end=\"0xfbeabfff\" output_port=\"1\" translation=\"0x0\""); // LINFlexD_15 -> LINFlexD_15 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_B.mapping_2",  "range_start=\"0xfbe70000\" range_end=\"0xfbe73fff\" output_port=\"2\" translation=\"0x0\""); // DSPI_2      -> DSPI_2 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_B.mapping_3",  "range_start=\"0xfbe74000\" range_end=\"0xfbe77fff\" output_port=\"3\" translation=\"0x0\""); // DSPI_3      -> DSPI_3 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_B.mapping_4",  "range_start=\"0xfbe78000\" range_end=\"0xfbe7bfff\" output_port=\"4\" translation=\"0x0\""); // DSPI_5      -> DSPI_5 (rel address)
	simulator->SetVariable("HARDWARE.PBRIDGE_B.mapping_5",  "range_start=\"0xf8000000\" range_end=\"0xf8003fff\" output_port=\"5\" translation=\"0x0\""); // PBRIDGE_B   -> PBRIDGE_B (rel address)
	
	simulator->SetVariable("HARDWARE.PBRIDGE_B.acr_mapping_0", "opacr92"); // LINFlexD_2
	simulator->SetVariable("HARDWARE.PBRIDGE_B.acr_mapping_1", "opacr85"); // LINFlexD_15
	simulator->SetVariable("HARDWARE.PBRIDGE_B.acr_mapping_2", "opacr99"); // DSPI_2
	simulator->SetVariable("HARDWARE.PBRIDGE_B.acr_mapping_3", "opacr98"); // DSPI_3
	simulator->SetVariable("HARDWARE.PBRIDGE_B.acr_mapping_4", "opacr97"); // DSPI_5
	simulator->SetVariable("HARDWARE.PBRIDGE_B.acr_mapping_5", "pacr0");   // PBRIDGE_B
	
	//  - XBAR_1_M1_CONCENTRATOR
	simulator->SetVariable("HARDWARE.XBAR_1_M1_CONCENTRATOR.input_socket_name_0", "eDMA_0");
	simulator->SetVariable("HARDWARE.XBAR_1_M1_CONCENTRATOR.input_socket_name_1", "eDMA_1");
	simulator->SetVariable("HARDWARE.XBAR_1_M1_CONCENTRATOR.output_socket_name_0", "XBAR_1_M1");
	
	simulator->SetVariable("HARDWARE.XBAR_1_M1_CONCENTRATOR.cycle_time", "10 ns");
	simulator->SetVariable("HARDWARE.XBAR_1_M1_CONCENTRATOR.mapping_0", "range_start=\"0x0\" range_end=\"0xffffffff\" output_port=\"0\" translation=\"0x0\"");
	
	// - EBI
	simulator->SetVariable("HARDWARE.EBI.input_socket_name_0", "XBAR_0_S5");
	simulator->SetVariable("HARDWARE.EBI.output_socket_name_0", "EBI_MEM_0");
	simulator->SetVariable("HARDWARE.EBI.output_socket_name_1", "EBI_MEM_1");
	simulator->SetVariable("HARDWARE.EBI.output_socket_name_2", "EBI_MEM_2");
	
	// - Loader memory router
	simulator->SetVariable("loader.memory-mapper.mapping",
	                       "HARDWARE.SYSTEM-SRAM:0x40000000-0x401fffff:+0x0"               // System SRAM                             -> SYSTEM_SRAM       (rel address)
	                       ",HARDWARE.FLASH:0x00400000-0x00407fff:+0x0"                    //   0 KB: UTest NVM Block (32 KB)         -> FLASH             (rel address)
	                       ",HARDWARE.FLASH:0x0060c000-0x0062ffff:+0x8000"                 //  32 KB: HSM Code (144 KB)               -> FLASH             (rel address)
	                       ",HARDWARE.FLASH:0x00680000-0x00687fff:+0x2c000"                // 176 KB: HSM Data (32 KB)                -> FLASH             (rel address)
	                       ",HARDWARE.FLASH:0x00800000-0x0087ffff:+0x34000"                // 208 KB: Data Flash (512 KB)             -> FLASH             (rel address)
	                       ",HARDWARE.FLASH:0x00fc0000-0x00ffffff:+0xb4000"                // 720 KB: Low & Mid Flash Blocks (256 KB) -> FLASH             (rel address)
	                       ",HARDWARE.FLASH:0x01000000-0x0177ffff:+0xf4000"                // 976 KB: Large Flash Blocks (7680 KB)    -> FLASH             (rel address)
	                       ",HARDWARE.Main_Core_0:0x50000000-0x50ffffff:+0x50000000"       // Main_Core_0 Local Memory                -> Main_Core_0       (rel address)
	                       ",HARDWARE.Main_Core_1:0x51000000-0x51ffffff:+0x51000000"       // Main_Core_1 Local Memory                -> Main_Core_1       (rel address)
	                       ",HARDWARE.Peripheral_Core_2:0x52000000-0x5fffffff:+0x52000000" // Peripheral_Core_2 Local Memory          -> Peripheral_Core_2 (rel address)
	                       ",HARDWARE.EBI_MEM_0:0x20000000-0x23ffffff:+0x0"                // EBI_MEM_0                               -> EBI_MEM_0         (rel address)
	                       ",HARDWARE.EBI_MEM_1:0x24000000-0x27ffffff:+0x0"                // EBI_MEM_1                               -> EBI_MEM_1         (rel address)
	                       ",HARDWARE.EBI_MEM_2:0x28000000-0x2bffffff:+0x0"                // EBI_MEM_2                               -> EBI_MEM_2         (rel address)
	                      );

	//  - System SRAM
	simulator->SetVariable("HARDWARE.SYSTEM-SRAM.cycle-time", "10 ns");
	simulator->SetVariable("HARDWARE.SYSTEM-SRAM.read-latency", "10 ns");
	simulator->SetVariable("HARDWARE.SYSTEM-SRAM.write-latency", "10 ns");
	simulator->SetVariable("HARDWARE.SYSTEM-SRAM.org", 0x0);
	simulator->SetVariable("HARDWARE.SYSTEM-SRAM.bytesize", 404 * 1024);
	
	//  - FLASH
	simulator->SetVariable("HARDWARE.FLASH.cycle-time", "10 ns");
	simulator->SetVariable("HARDWARE.FLASH.read-latency", "10 ns");
	simulator->SetVariable("HARDWARE.FLASH.write-latency", "10 ns");
	simulator->SetVariable("HARDWARE.FLASH.org", 0x0);
	simulator->SetVariable("HARDWARE.FLASH.bytesize", (8640 + 16) * 1024); // Note: 8640 KB (main space) + 16 KB OTP (UTEST space)
	simulator->SetVariable("HARDWARE.FLASH.read-only", true);

	//  - SWT_0
	simulator->SetVariable("HARDWARE.SWT_0.swt-cr-reset-value", 0xff00010a);
	simulator->SetVariable("HARDWARE.SWT_0.swt-to-reset-value", 0x0005fcd0);

	//  - SWT_1
	simulator->SetVariable("HARDWARE.SWT_1.swt-cr-reset-value", 0xff00010a);
	simulator->SetVariable("HARDWARE.SWT_1.swt-to-reset-value", 0x0005fcd0);

	//  - SWT_2
	simulator->SetVariable("HARDWARE.SWT_2.swt-cr-reset-value", 0xff00011b);
	simulator->SetVariable("HARDWARE.SWT_2.swt-to-reset-value", 0x0003fde0);
	
	//  - SWT_3
	simulator->SetVariable("HARDWARE.SWT_3.swt-cr-reset-value", 0xff00010a);
	simulator->SetVariable("HARDWARE.SWT_3.swt-to-reset-value", 0x0005fcd0);

	//  - PIT_0
	simulator->SetVariable("HARDWARE.PIT_0.pit-mcr-reset-value", 0x06);
	
	//  - PIT_1
	simulator->SetVariable("HARDWARE.PIT_1.pit-mcr-reset-value", 0x02);

	//  - DMAMUX_0
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[0]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[21]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[22]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[23]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[24]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[25]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[26]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[27]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[28]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[29]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[30]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[31]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[32]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[33]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[34]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[35]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[36]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[37]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[38]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[39]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[40]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[41]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[42]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[43]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[44]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[45]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[46]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[47]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[48]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[49]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[50]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[51]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[52]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[53]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[54]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[55]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[56]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[57]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[58]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[59]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[60]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[61]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_0.disable-dma-source[62]", true);
	
	//  - DMAMUX_1
	simulator->SetVariable("HARDWARE.DMAMUX_1.disable-dma-source[0]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_1.disable-dma-source[51]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_1.disable-dma-source[52]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_1.disable-dma-source[58]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_1.disable-dma-source[59]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_1.disable-dma-source[60]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_1.disable-dma-source[61]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_1.disable-dma-source[62]", true);

	//  - DMAMUX_2
	simulator->SetVariable("HARDWARE.DMAMUX_2.disable-dma-source[0]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_2.disable-dma-source[51]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_2.disable-dma-source[52]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_2.disable-dma-source[53]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_2.disable-dma-source[54]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_2.disable-dma-source[55]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_2.disable-dma-source[56]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_2.disable-dma-source[57]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_2.disable-dma-source[58]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_2.disable-dma-source[59]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_2.disable-dma-source[60]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_2.disable-dma-source[61]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_2.disable-dma-source[62]", true);

	//  - DMAMUX_3
	simulator->SetVariable("HARDWARE.DMAMUX_3.disable-dma-source[0]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_3.disable-dma-source[51]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_3.disable-dma-source[52]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_3.disable-dma-source[53]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_3.disable-dma-source[54]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_3.disable-dma-source[55]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_3.disable-dma-source[56]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_3.disable-dma-source[57]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_3.disable-dma-source[58]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_3.disable-dma-source[59]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_3.disable-dma-source[60]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_3.disable-dma-source[61]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_3.disable-dma-source[62]", true);

	//  - DMAMUX_4
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[0]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[42]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[43]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[44]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[45]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[46]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[47]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[48]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[49]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[50]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[51]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[52]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[53]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[54]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[55]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[56]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[57]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[58]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[59]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[60]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[61]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_4.disable-dma-source[62]", true);

	//  - DMAMUX_5
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[0]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[42]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[43]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[44]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[45]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[46]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[47]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[48]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[49]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[50]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[51]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[52]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[53]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[54]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[55]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[56]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[57]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[58]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[59]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[60]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[61]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_5.disable-dma-source[62]", true);

	//  - DMAMUX_6
	simulator->SetVariable("HARDWARE.DMAMUX_6.disable-dma-source[0]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_6.disable-dma-source[48]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_6.disable-dma-source[49]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_6.disable-dma-source[50]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_6.disable-dma-source[51]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_6.disable-dma-source[52]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_6.disable-dma-source[53]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_6.disable-dma-source[54]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_6.disable-dma-source[55]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_6.disable-dma-source[56]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_6.disable-dma-source[57]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_6.disable-dma-source[58]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_6.disable-dma-source[59]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_6.disable-dma-source[60]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_6.disable-dma-source[61]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_6.disable-dma-source[62]", true);

	//  - DMAMUX_7
	simulator->SetVariable("HARDWARE.DMAMUX_7.disable-dma-source[0]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_7.disable-dma-source[51]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_7.disable-dma-source[52]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_7.disable-dma-source[53]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_7.disable-dma-source[54]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_7.disable-dma-source[55]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_7.disable-dma-source[56]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_7.disable-dma-source[57]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_7.disable-dma-source[58]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_7.disable-dma-source[59]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_7.disable-dma-source[60]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_7.disable-dma-source[61]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_7.disable-dma-source[62]", true);

	//  - DMAMUX_8
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[0]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[46]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[47]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[48]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[49]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[50]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[51]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[52]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[53]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[54]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[55]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[56]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[57]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[58]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[59]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[60]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[61]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_8.disable-dma-source[62]", true);

	//  - DMAMUX_9
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[0]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[45]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[46]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[47]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[48]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[49]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[50]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[51]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[52]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[53]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[54]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[55]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[56]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[57]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[58]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[59]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[60]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[61]", true);
	simulator->SetVariable("HARDWARE.DMAMUX_9.disable-dma-source[62]", true);

	//  - eDMA_0
	simulator->SetVariable("HARDWARE.eDMA_0.master-id", 3);

	//  - eDMA_1
	simulator->SetVariable("HARDWARE.eDMA_1.master-id", 11);
	
	//  - EBI_MEM_0
	simulator->SetVariable("HARDWARE.EBI_MEM_0.cycle-time", "10 ns");
	simulator->SetVariable("HARDWARE.EBI_MEM_0.read-latency", "10 ns");
	simulator->SetVariable("HARDWARE.EBI_MEM_0.write-latency", "10 ns");
	simulator->SetVariable("HARDWARE.EBI_MEM_0.org", 0x0);
	simulator->SetVariable("HARDWARE.EBI_MEM_0.bytesize", 64 * 1024 * 1024);

	//  - EBI_MEM_1
	simulator->SetVariable("HARDWARE.EBI_MEM_1.cycle-time", "10 ns");
	simulator->SetVariable("HARDWARE.EBI_MEM_1.read-latency", "10 ns");
	simulator->SetVariable("HARDWARE.EBI_MEM_1.write-latency", "10 ns");
	simulator->SetVariable("HARDWARE.EBI_MEM_1.org", 0x0);
	simulator->SetVariable("HARDWARE.EBI_MEM_1.bytesize", 64 * 1024 * 1024);

	//  - EBI_MEM_2
	simulator->SetVariable("HARDWARE.EBI_MEM_2.cycle-time", "10 ns");
	simulator->SetVariable("HARDWARE.EBI_MEM_2.read-latency", "10 ns");
	simulator->SetVariable("HARDWARE.EBI_MEM_2.write-latency", "10 ns");
	simulator->SetVariable("HARDWARE.EBI_MEM_2.org", 0x0);
	simulator->SetVariable("HARDWARE.EBI_MEM_2.bytesize", 64 * 1024 * 1024);

	//  - PCM_STUB
	simulator->SetVariable("HARDWARE.PCM.cycle-time", "20 ns");
	simulator->SetVariable("HARDWARE.PCM.read-latency", "20 ns");
	simulator->SetVariable("HARDWARE.PCM.write-latency", "20 ns");
	simulator->SetVariable("HARDWARE.PCM.org", 0x0);
	simulator->SetVariable("HARDWARE.PCM.bytesize", 16 * 1024);
	
	//  - PFLASH_STUB
	simulator->SetVariable("HARDWARE.PFLASH.cycle-time", "20 ns");
	simulator->SetVariable("HARDWARE.PFLASH.read-latency", "20 ns");
	simulator->SetVariable("HARDWARE.PFLASH.write-latency", "20 ns");
	simulator->SetVariable("HARDWARE.PFLASH.org", 0x0);
	simulator->SetVariable("HARDWARE.PFLASH.bytesize", 16 * 1024);

	//  - MC_ME_STUB
	simulator->SetVariable("HARDWARE.MC_ME.cycle-time", "20 ns");
	simulator->SetVariable("HARDWARE.MC_ME.read-latency", "20 ns");
	simulator->SetVariable("HARDWARE.MC_ME.write-latency", "20 ns");
	simulator->SetVariable("HARDWARE.MC_ME.org", 0x0);
	simulator->SetVariable("HARDWARE.MC_ME.bytesize", 16 * 1024);

	//  - MC_CGM_STUB
	simulator->SetVariable("HARDWARE.MC_CGM.cycle-time", "20 ns");
	simulator->SetVariable("HARDWARE.MC_CGM.read-latency", "20 ns");
	simulator->SetVariable("HARDWARE.MC_CGM.write-latency", "20 ns");
	simulator->SetVariable("HARDWARE.MC_CGM.org", 0x0);
	simulator->SetVariable("HARDWARE.MC_CGM.bytesize", 16 * 1024);

	//  - MC_RGM_STUB
	simulator->SetVariable("HARDWARE.MC_RGM.cycle-time", "20 ns");
	simulator->SetVariable("HARDWARE.MC_RGM.read-latency", "20 ns");
	simulator->SetVariable("HARDWARE.MC_RGM.write-latency", "20 ns");
	simulator->SetVariable("HARDWARE.MC_RGM.org", 0x0);
	simulator->SetVariable("HARDWARE.MC_RGM.bytesize", 16 * 1024);

	//  - PRAM_0 stub
	simulator->SetVariable("HARDWARE.PRAM_0.cycle-time", "20 ns");
	simulator->SetVariable("HARDWARE.PRAM_0.read-latency", "20 ns");
	simulator->SetVariable("HARDWARE.PRAM_0.write-latency", "20 ns");
	simulator->SetVariable("HARDWARE.PRAM_0.org", 0x0);
	simulator->SetVariable("HARDWARE.PRAM_0.bytesize", 16 * 1024);
	
	//  - Modular CAN
	simulator->SetVariable("HARDWARE.M_CAN_1.endian", "big-endian");
	simulator->SetVariable("HARDWARE.M_CAN_2.endian", "big-endian");
	simulator->SetVariable("HARDWARE.M_CAN_3.endian", "big-endian");
	simulator->SetVariable("HARDWARE.M_CAN_4.endian", "big-endian");

	//  - Shared CAN message RAM
	simulator->SetVariable("HARDWARE.SHARED_CAN_MESSAGE_RAM.cycle-time", "20 ns");
	simulator->SetVariable("HARDWARE.SHARED_CAN_MESSAGE_RAM.read-latency", "20 ns");
	simulator->SetVariable("HARDWARE.SHARED_CAN_MESSAGE_RAM.write-latency", "20 ns");
	simulator->SetVariable("HARDWARE.SHARED_CAN_MESSAGE_RAM.org", 0x0);
	simulator->SetVariable("HARDWARE.SHARED_CAN_MESSAGE_RAM.bytesize", 20 * 1024);

	//  - Shared CAN message RAM Controller/Access Arbiter
	simulator->SetVariable("HARDWARE.SHARED_CAN_MESSAGE_RAM_ROUTER.input_socket_name_0", "PBRIDGE_A");
	simulator->SetVariable("HARDWARE.SHARED_CAN_MESSAGE_RAM_ROUTER.input_socket_name_1", "M_CAN_1");
	simulator->SetVariable("HARDWARE.SHARED_CAN_MESSAGE_RAM_ROUTER.input_socket_name_2", "M_CAN_2");
	simulator->SetVariable("HARDWARE.SHARED_CAN_MESSAGE_RAM_ROUTER.input_socket_name_3", "M_CAN_3");
	simulator->SetVariable("HARDWARE.SHARED_CAN_MESSAGE_RAM_ROUTER.input_socket_name_4", "M_CAN_4");
	simulator->SetVariable("HARDWARE.SHARED_CAN_MESSAGE_RAM_ROUTER.output_socket_name_0", "SHARED_CAN_MESSAGE_RAM");
	
	simulator->SetVariable("HARDWARE.SHARED_CAN_MESSAGE_RAM_ROUTER.cycle_time", "20 ns");
	simulator->SetVariable("HARDWARE.SHARED_CAN_MESSAGE_RAM_ROUTER.mapping_0", "range_start=\"0x0\" range_end=\"0x7fff\" output_port=\"0\" translation=\"0x0\"");

	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - GDB Server run-time configuration
	simulator->SetVariable("gdb-server0.tcp-port", 12345);
	simulator->SetVariable("gdb-server0.architecture-description-filename", "unisim/service/debug/gdb_server/gdb_powerpc_vle.xml");
	simulator->SetVariable("gdb-server1.tcp-port", 12346);
	simulator->SetVariable("gdb-server1.architecture-description-filename", "unisim/service/debug/gdb_server/gdb_powerpc_vle.xml");
	simulator->SetVariable("gdb-server2.tcp-port", 12347);
	simulator->SetVariable("gdb-server2.architecture-description-filename", "unisim/service/debug/gdb_server/gdb_powerpc_vle.xml");
	
	//  - Debugger run-time configuration
	simulator->SetVariable("debugger.parse-dwarf", false);
	simulator->SetVariable("debugger.dwarf-register-number-mapping-filename", "unisim/util/debug/dwarf/powerpc_e500_gcc_dwarf_register_number_mapping.xml");
	simulator->SetVariable("debugger.sel-cpu[0]", 0); // gdb-server
	simulator->SetVariable("debugger.sel-cpu[1]", 1); // gdb-server
	simulator->SetVariable("debugger.sel-cpu[2]", 2); // gdb-server
	simulator->SetVariable("debugger.sel-cpu[3]", 0); // inline-debugger
	simulator->SetVariable("debugger.sel-cpu[4]", 1); // inline-debugger
	simulator->SetVariable("debugger.sel-cpu[5]", 2); // inline-debugger
	simulator->SetVariable("debugger.sel-cpu[6]", 0); // profiler
	simulator->SetVariable("debugger.sel-cpu[7]", 1); // profiler
	simulator->SetVariable("debugger.sel-cpu[8]", 2); // profiler
	
	//  - Netstreamer
	simulator->SetVariable("netstreamer0.tcp-port", 12348);
	simulator->SetVariable("netstreamer1.tcp-port", 12349);
	simulator->SetVariable("netstreamer2.tcp-port", 12350);
	simulator->SetVariable("netstreamer14.tcp-port", 12351);
	simulator->SetVariable("netstreamer15.tcp-port", 12352);
	simulator->SetVariable("netstreamer16.tcp-port", 12353);
	
	//  - DSPI netlist
	simulator->SetVariable("dspi_0-is-slave", true);
	simulator->SetVariable("dspi_0-master", 2);
	simulator->SetVariable("dspi_0-slave", 0);
	
	// HTTP server
	simulator->SetVariable("http-server.http-port", 12360);
	
	// Web terminals
	simulator->SetVariable("web-terminal0.title", "Serial Terminal over LINFlexD_0");
	simulator->SetVariable("web-terminal1.title", "Serial Terminal over LINFlexD_1");
	simulator->SetVariable("web-terminal2.title", "Serial Terminal over LINFlexD_2");
	simulator->SetVariable("web-terminal14.title", "Serial Terminal over LINFlexD_14");
	simulator->SetVariable("web-terminal15.title", "Serial Terminal over LINFlexD_15");
	simulator->SetVariable("web-terminal16.title", "Serial Terminal over LINFlexD_16");
}

void Simulator::Run()
{
	std::cerr << "Starting simulation at supervisor privilege level" << std::endl;
	
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

	uint64_t instruction_counter = (uint64_t)(*peripheral_core_2)["instruction-counter"] + (uint64_t)(*main_core_0)["instruction-counter"] + (uint64_t)(*main_core_1)["instruction-counter"];
	double run_time = (double)(*peripheral_core_2)["run-time"] + (double)(*main_core_0)["run-time"] + (double)(*main_core_1)["run-time"];
	double idle_time = (double)(*peripheral_core_2)["idle-time"] + (double)(*main_core_0)["idle-time"] + (double)(*main_core_1)["idle-time"];
	
	std::cerr << "simulation time: " << spent_time << " seconds" << std::endl;
	std::cerr << "simulated time: " << sc_core::sc_time_stamp().to_seconds() << " seconds (exactly " << sc_core::sc_time_stamp() << ")" << std::endl;
	std::cerr << "target speed: " << (instruction_counter / (run_time - idle_time) / 1000000.0) << " MIPS" << std::endl;
	std::cerr << "host simulation speed: " << ((double) instruction_counter / spent_time / 1000000.0) << " MIPS" << std::endl;
	std::cerr << "time dilatation: " << spent_time / sc_core::sc_time_stamp().to_seconds() << " times slower than target machine" << std::endl;

	for(unsigned int prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		if(profiler[prc_num])
		{
			profiler[prc_num]->Output();
		}
	}
}

bool Simulator::EndSetup()
{
	if(http_server)
	{
		unsigned int prc_num;
		for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
		{
			if(profiler[prc_num])
			{
				std::string tab_title("Profile of ");
				switch(prc_num)
				{
					case 0: tab_title += main_core_0->GetName(); break;
					case 1: tab_title += main_core_1->GetName(); break;
					case 2: tab_title += peripheral_core_2->GetName(); break;
				}
				std::stringstream label_sstr;
				label_sstr << "<img src=\"/unisim/service/debug/profiler/icon_profile_cpu" << prc_num << ".svg\" alt=\"Profile CPU#" << prc_num << "\">";
				http_server->AddJSAction(
					unisim::service::interfaces::ToolbarOpenTabAction(
						/* name */      profiler[prc_num]->GetName(), 
						/* label */     label_sstr.str(),
						/* tips */      tab_title,
						/* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
						/* uri */       profiler[prc_num]->URI()
				));
			}
		}
		
		if(web_terminal0)
		{
			http_server->AddJSAction(
				unisim::service::interfaces::ToolbarOpenTabAction(
					/* name */      web_terminal0->GetName(),
					/* label */     "<img src=\"/unisim/service/web_terminal/icon_term0.svg\" alt=\"TERM0\">",
					/* tips */      "Serial Terminal over LINFlexD_0",
					/* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
					/* uri */       web_terminal0->URI()
			));
		}
		if(web_terminal1)
		{
			http_server->AddJSAction(
				unisim::service::interfaces::ToolbarOpenTabAction(
					/* name */      web_terminal1->GetName(),
					/* label */     "<img src=\"/unisim/service/web_terminal/icon_term1.svg\" alt=\"TERM1\">",
					/* tips */      "Serial Terminal over LINFlexD_1",
					/* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
					/* uri */       web_terminal1->URI()
			));
		}
		if(web_terminal2)
		{
			http_server->AddJSAction(
				unisim::service::interfaces::ToolbarOpenTabAction(
					/* name */      web_terminal2->GetName(),
					/* label */     "<img src=\"/unisim/service/web_terminal/icon_term2.svg\" alt=\"TERM2\">",
					/* tips */      "Serial Terminal over LINFlexD_2",
					/* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
					/* uri */       web_terminal2->URI()
			));
		}
		if(web_terminal14)
		{
			http_server->AddJSAction(
				unisim::service::interfaces::ToolbarOpenTabAction(
					/* name */      web_terminal14->GetName(),
					/* label */     "<img src=\"/unisim/service/web_terminal/icon_term14.svg\" alt=\"TERM14\">",
					/* tips */      "Serial Terminal over LINFlexD_14",
					/* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
					/* uri */       web_terminal14->URI()
			));
		}
		if(web_terminal15)
		{
			http_server->AddJSAction(
				unisim::service::interfaces::ToolbarOpenTabAction(
					/* name */      web_terminal15->GetName(),
					/* label */     "<img src=\"/unisim/service/web_terminal/icon_term15.svg\" alt=\"TERM15\">",
					/* tips */      "Serial Terminal over LINFlexD_15",
					/* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
					/* uri */       web_terminal15->URI()
			));
		}
		if(web_terminal16)
		{
			http_server->AddJSAction(
				unisim::service::interfaces::ToolbarOpenTabAction(
					/* name */      web_terminal16->GetName(),
					/* label */     "<img src=\"/unisim/service/web_terminal/icon_term16.svg\" alt=\"TERM16\">",
					/* tips */      "Serial Terminal over LINFlexD_16",
					/* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
					/* uri */       web_terminal16->URI()
			));
		}
	}
	
	return true;
}

unisim::kernel::Simulator::SetupStatus Simulator::Setup()
{
	if(inline_debugger[0] || inline_debugger[1] || inline_debugger[2] || profiler[0] || profiler[1] || profiler[2])
	{
		SetVariable("debugger.parse-dwarf", true);
	}
	
	// Optionally get the program to load from the command line arguments
	unisim::kernel::VariableBase *cmd_args = FindVariable("cmd-args");
	unsigned int cmd_args_length = cmd_args->GetLength();
	if(cmd_args_length > 0)
	{
		SetVariable("loader.filename", ((std::string)(*cmd_args)[0]).c_str());
	}

	unisim::kernel::Simulator::SetupStatus setup_status = unisim::kernel::Simulator::Setup();
	
	return setup_status;
}

void Simulator::SigInt()
{
	if((!inline_debugger[0] || !inline_debugger[0]->IsStarted()) &&
	   (!inline_debugger[1] || !inline_debugger[1]->IsStarted()) &&
	   (!inline_debugger[2] || !inline_debugger[2]->IsStarted()))
	{
		unisim::kernel::Simulator::Instance()->Stop(0, 0, true);
	}
}
