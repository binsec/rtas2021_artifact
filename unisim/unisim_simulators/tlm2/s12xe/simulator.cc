/*
 * simulator.cc
 *
 *  Created on: 15 mars 2011
 *      Author: rnouacer
 */

#include <simulator.hh>
#include <unisim/kernel/config/xml/xml_config_file_helper.hh>
#include <unisim/kernel/config/ini/ini_config_file_helper.hh>
#include <unisim/kernel/config/json/json_config_file_helper.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/service/debug/debugger/debugger.tcc>

Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, mmc(0)
	, mpu(0)
	, crg(0)
	, ect(0)
	, atd1(0)
	, atd0(0)
	, pwm(0)
	, pit(0)
	, sci0(0)
	, sci1(0)
	, sci2(0)
	, sci3(0)
	, sci4(0)
	, sci5(0)
	, sci6(0)
	, sci7(0)
	, spi0(0)
	, spi1(0)
	, spi2(0)
	, can0(0)
	, can1(0)
	, can2(0)
	, can3(0)
	, can4(0)
	, pim(0)
	, dbg(0)
	, iic0(0)
	, iic1(0)
	, vreg(0)
	, tim(0)
	, reserved(0)
	, global_ram(0)
	, global_flash(0)
	, s12xint(0)
	, registersTee(0)
	, memoryImportExportTee(0)
#ifdef HAVE_RTBCOB
	, rtbStub(0)
#else
	, xml_atd_pwm_stub(0)
#endif

	, can_stub(0)
	, tranceiver0(0)
	, tranceiver1(0)
	, tranceiver2(0)
	, tranceiver3(0)
	, tranceiver4(0)

	, spi1_stub(0)
	, spi2_stub(0)

	, loader(0)

	, debugger(0)
	, gdb_server(0)
	, pim_server(0)
	, inline_debugger(0)
	, monitor(0)
	, sci_telnet(0)
	, spi_telnet(0)
	, http_server(0)
	, instrumenter(0)
	, profiler(0)
	, sci_char_io_tee(0)
	, spi_char_io_tee(0)
	, sci_web_terminal(0)
	, spi_web_terminal(0)

	, sim_time(0)
	, host_time(0)

	, logger_console_printer(0)
	, logger_text_file_writer(0)
	, logger_http_writer(0)
	, logger_xml_file_writer(0)
	, logger_netstream_writer(0)
	
//	, filename("")
//	, symbol_filename("")

	, enable_pim_server(false)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, enable_monitor(false)
	, enable_profiler(false)
	, param_enable_pim_server("enable-pim-server", 0, enable_pim_server, "Enable/Disable PIM server instantiation")
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, param_enable_monitor("enable-monitor", 0, enable_monitor, "Enable/Disable monitoring tool")
	, param_enable_profiler("enable-profiler", 0, enable_profiler, "Enable/Disable profiling tool")

	, endian("")
	, program_counter_name("")

	, exit_status(-1)
//	, isS19(false)

	, dump_parameters(false)
	, dump_formulas(false)
	, dump_statistics(true)

	, param_dump_parameters("dump-parameters", 0, dump_parameters, "")
	, param_dump_formulas("dump-formulas", 0, dump_formulas, "")
	, param_dump_statistics("dump-statistics", 0, dump_statistics, "")

	, null_stat_var(0)
	, stat_data_load_ratio(*this)
	, stat_data_store_ratio(*this)

	, spent_time(0)
	, isStop(false)

	, entry_point(0)
	, param_entry_point("entry-point", 0, entry_point, "Entry point, to use if there is no reset address")

{

	param_endian = new Parameter<string>("endian", 0, endian, "Target endianness");
	param_endian->SetMutable(false);
	param_endian->SetVisible(true);

	param_pc_reg_name = new Parameter<string>("program-counter-name", 0, program_counter_name, "Target CPU program counter name");
	param_pc_reg_name->SetMutable(false);
	param_pc_reg_name->SetVisible(true);

	logger_console_printer = new LOGGER_CONSOLE_PRINTER();
	logger_text_file_writer = new LOGGER_TEXT_FILE_WRITER();
	logger_http_writer = new LOGGER_HTTP_WRITER();
	logger_xml_file_writer = new LOGGER_XML_FILE_WRITER();
	logger_netstream_writer = new LOGGER_NETSTREAM_WRITER();

	// - Instrumenter
	instrumenter = new INSTRUMENTER("instrumenter");

	//=========================================================================
	//===      Handling of file to load passed as command line argument     ===
	//=========================================================================

//	VariableBase *cmd_args = FindVariable("cmd-args");
//	unsigned int cmd_args_length = cmd_args->GetLength();
//	if(cmd_args_length > 0)
//	{
//		filename = (string)(*cmd_args)[0];
//
//		std::cerr << "filename=\"" << filename << "\"" << std::endl;
//	}

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - 68HCS12X processor

	mpu = 	new MPU("MPU");
	mmc = 	new MMC("MMC", mpu);

	cpu =new CPU("CPU", mmc);
	xgate =new XGATE("XGATE", mmc);

	crg = new CRG("CRG");
	ect = new ECT("ECT");
	pit = new PIT("PIT");

	sci0 = new S12SCI("SCI0");
	sci1 = new S12SCI("SCI1");
	sci2 = new S12SCI("SCI2");
	sci3 = new S12SCI("SCI3");
	sci4 = new S12SCI("SCI4");
	sci5 = new S12SCI("SCI5");
	sci6 = new S12SCI("SCI6");
	sci7 = new S12SCI("SCI7");

	can0 = new MSCAN("CAN0");
	can1 = new MSCAN("CAN1");
	can2 = new MSCAN("CAN2");
	can3 = new MSCAN("CAN3");
	can4 = new MSCAN("CAN4");

	spi0 = new S12SPIV5("SPI0");
	spi1 = new S12SPIV5("SPI1");
	spi2 = new S12SPIV5("SPI2");

	spi1_stub = new TLE72XXSL("SPI1-STUB");
	spi2_stub = new TLE72XXSL("SPI2-STUB");

	pim  = new S12XEPIM("PIM");
	dbg  = new S12XDBG("DBG");

	iic0 = new S12IIC("IIC0");
	iic1 = new S12IIC("IIC1");

	vreg = new S12VREGL3V3("VREG");

	tim = new S12TIM16B8C("TIM");

	reserved = new RESERVED("RESERVED");

	atd1 = new ATD1("ATD1");
	atd0 = new ATD0("ATD0");

	pwm = new PWM("PWM");

	//  - Memories
	global_ram = new RAM("RAM");
	global_flash = new FTM("FTM");

	// - Interrupt controller
	s12xint = new XINT("XINT");

	registersTee = new RegistersTee("registersTee");

	memoryImportExportTee = new MemoryImportExportTee("memoryImportExportTee");

#ifdef HAVE_RTBCOB
	rtbStub = new RTBStub("atd-pwm-stub"/*, fsb_cycle_time*/);
#else
	xml_atd_pwm_stub = new XML_ATD_PWM_STUB("atd-pwm-stub"/*, fsb_cycle_time*/);
#endif

	can_stub = new CAN_STUB("CAN-STUB");
	tranceiver0 = new TLE8264_2E("Tranceiver0");
	tranceiver1 = new TLE8264_2E("Tranceiver1");
	tranceiver2 = new TLE8264_2E("Tranceiver2");
	tranceiver3 = new TLE8264_2E("Tranceiver3");
	tranceiver4 = new TLE8264_2E("Tranceiver4");

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================

	//  - Multiformat loader
	loader = new MultiFormatLoader<CPU_ADDRESS_TYPE>("loader");

	//  - debugger
	debugger = (enable_inline_debugger || enable_gdb_server || enable_pim_server || enable_monitor || enable_profiler) ? new Debugger("debugger") : 0;

	//  - GDB server
	gdb_server = enable_gdb_server ? new GDBServer<CPU_ADDRESS_TYPE>("gdb-server") : 0;

	//  - PIM server
	pim_server = enable_pim_server ? new PIMServer<CPU_ADDRESS_TYPE>("pim-server") : 0;

	//  - Inline debugger
	inline_debugger = enable_inline_debugger ? new InlineDebugger<CPU_ADDRESS_TYPE>("inline-debugger") : 0;

	// Monitoring tool: ARTiMon or EACSEL
	monitor = (enable_monitor)? new MONITOR("Monitor"): 0;

	// - telnet
	sci_telnet = new TELNET("sci-telnet");
	spi_telnet = new TELNET("spi-telnet");
	
	// - Http Server
	http_server = new HTTP_SERVER("http-server");
	
	// - Profiler
	profiler = enable_profiler ? new PROFILER("profiler") : 0;
	
	// - Char I/O Tees
	sci_char_io_tee = new CHAR_IO_TEE("sci-char-io-tee");
	spi_char_io_tee = new CHAR_IO_TEE("spi-char-io-tee");
	
	// - Web Terminals
	sci_web_terminal = new WEB_TERMINAL("sci-web-terminal");
	spi_web_terminal = new WEB_TERMINAL("spi-web-terminal");

	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");

	//=========================================================================
	//===                       Service parameterization     ===
	//=========================================================================

//	if (!filename.empty()) {
//		if(isS19) {
//			(*loaderS19)["filename"] = filename.c_str();
//		}
//		else
//		{
//			(*loaderELF)["filename"] = filename.c_str();
//		}
//	}

	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	s12xint->toCPU12X_request(cpu->xint_interrupt_request);
	s12xint->toXGATE_request(xgate->xint_interrupt_request);

	crg->interrupt_request(s12xint->interrupt_request);
	ect->interrupt_request(s12xint->interrupt_request);
	pit->interrupt_request(s12xint->interrupt_request);
	pwm->interrupt_request(s12xint->interrupt_request);
	atd1->interrupt_request(s12xint->interrupt_request);
	atd0->interrupt_request(s12xint->interrupt_request);
	xgate->interrupt_request(s12xint->interrupt_request);

	global_flash->interrupt_request(s12xint->interrupt_request);
	mpu->interrupt_request(s12xint->interrupt_request);

	sci0->interrupt_request(s12xint->interrupt_request);
	sci1->interrupt_request(s12xint->interrupt_request);
	sci2->interrupt_request(s12xint->interrupt_request);
	sci3->interrupt_request(s12xint->interrupt_request);
	sci4->interrupt_request(s12xint->interrupt_request);
	sci5->interrupt_request(s12xint->interrupt_request);
	sci6->interrupt_request(s12xint->interrupt_request);
	sci7->interrupt_request(s12xint->interrupt_request);

	can0->interrupt_request(s12xint->interrupt_request);
	can1->interrupt_request(s12xint->interrupt_request);
	can2->interrupt_request(s12xint->interrupt_request);
	can3->interrupt_request(s12xint->interrupt_request);
	can4->interrupt_request(s12xint->interrupt_request);

	tranceiver0->interrupt_request(s12xint->interrupt_request);
	tranceiver1->interrupt_request(s12xint->interrupt_request);
	tranceiver2->interrupt_request(s12xint->interrupt_request);
	tranceiver3->interrupt_request(s12xint->interrupt_request);
	tranceiver4->interrupt_request(s12xint->interrupt_request);

	spi0->interrupt_request(s12xint->interrupt_request);
	spi1->interrupt_request(s12xint->interrupt_request);
	spi2->interrupt_request(s12xint->interrupt_request);

#ifdef HAVE_RTBCOB
	rtbStub->atd1_master_sock(atd1->anx_socket);
	rtbStub->atd0_master_sock(atd0->anx_socket);
	rtbStub->slave_sock(pwm->master_sock);
#else
	xml_atd_pwm_stub->atd1_master_sock(atd1->anx_socket);
	xml_atd_pwm_stub->atd0_master_sock(atd0->anx_socket);
	xml_atd_pwm_stub->slave_sock(pwm->master_sock);
#endif

	can0->can_tx_sock(tranceiver0->can_slave_rx_sock);
	tranceiver0->can_master_tx_sock(can_stub->can_rx_sock);
	can_stub->can_tx_sock(tranceiver0->can_master_rx_sock);
	tranceiver0->can_slave_tx_sock(can0->can_rx_sock);

	tranceiver0->spi_tx_socket(spi0->rx_socket);
	spi0->tx_socket(tranceiver0->spi_rx_socket);

	can1->can_tx_sock(tranceiver1->can_slave_rx_sock);
	tranceiver1->can_master_tx_sock(can_stub->can_rx_sock);
	can_stub->can_tx_sock(tranceiver1->can_master_rx_sock);
	tranceiver1->can_slave_tx_sock(can1->can_rx_sock);

	tranceiver1->spi_tx_socket(tranceiver1->spi_rx_socket);

	spi1->tx_socket(spi1_stub->rx_socket);
	spi1_stub->tx_socket(spi1->rx_socket);

	can2->can_tx_sock(tranceiver2->can_slave_rx_sock);
	tranceiver2->can_master_tx_sock(can_stub->can_rx_sock);
	can_stub->can_tx_sock(tranceiver2->can_master_rx_sock);
	tranceiver2->can_slave_tx_sock(can2->can_rx_sock);

	tranceiver2->spi_tx_socket(tranceiver2->spi_rx_socket);

	spi2->tx_socket(spi2_stub->rx_socket);
	spi2_stub->tx_socket(spi2->rx_socket);

	can3->can_tx_sock(tranceiver3->can_slave_rx_sock);
	tranceiver3->can_master_tx_sock(can_stub->can_rx_sock);
	can_stub->can_tx_sock(tranceiver3->can_master_rx_sock);
	tranceiver3->can_slave_tx_sock(can3->can_rx_sock);

	tranceiver3->spi_tx_socket(tranceiver3->spi_rx_socket);

	can4->can_tx_sock(tranceiver4->can_slave_rx_sock);
	tranceiver4->can_master_tx_sock(can_stub->can_rx_sock);
	can_stub->can_tx_sock(tranceiver4->can_master_rx_sock);
	tranceiver4->can_slave_tx_sock(can4->can_rx_sock);

	tranceiver4->spi_tx_socket(tranceiver4->spi_rx_socket);

	// This order is mandatory (see the memoryMapping)
	mmc->init_socket(crg->slave_socket);
	mmc->init_socket(ect->slave_socket);
	mmc->init_socket(atd1->slave_socket);
	mmc->init_socket(sci2->slave_socket);
	mmc->init_socket(sci3->slave_socket);
	mmc->init_socket(sci0->slave_socket);
	mmc->init_socket(sci1->slave_socket);
	mmc->init_socket(spi0->slave_socket);
	mmc->init_socket(spi1->slave_socket);
	mmc->init_socket(spi2->slave_socket);
	mmc->init_socket(global_flash->slave_socket);
	mmc->init_socket(mpu->slave_socket);
	mmc->init_socket(s12xint->slave_socket);
	mmc->init_socket(sci4->slave_socket);
	mmc->init_socket(sci5->slave_socket);
	mmc->init_socket(can0->slave_socket);
	mmc->init_socket(can1->slave_socket);
	mmc->init_socket(can2->slave_socket);
	mmc->init_socket(can3->slave_socket);
	mmc->init_socket(can4->slave_socket);
	mmc->init_socket(atd0->slave_socket);
	mmc->init_socket(pwm->slave_socket);
	mmc->init_socket(sci6->slave_socket);
	mmc->init_socket(sci7->slave_socket);
	mmc->init_socket(pit->slave_socket);
	mmc->init_socket(xgate->target_socket);
	mmc->init_socket(global_ram->slave_sock);
	mmc->init_socket(global_flash->slave_sock);
	mmc->init_socket(pim->slave_socket);
	mmc->init_socket(dbg->slave_socket);
	mmc->init_socket(iic0->slave_socket);
	mmc->init_socket(iic1->slave_socket);
	mmc->init_socket(vreg->slave_socket);
	mmc->init_socket(tim->slave_socket);
	mmc->init_socket(reserved->slave_socket);

	crg->bus_clock_socket(cpu->bus_clock_socket);
	crg->bus_clock_socket(ect->bus_clock_socket);
	crg->bus_clock_socket(pit->bus_clock_socket);

	crg->bus_clock_socket(global_flash->bus_clock_socket);

	crg->bus_clock_socket(pwm->bus_clock_socket);
	crg->bus_clock_socket(atd1->bus_clock_socket);
	crg->bus_clock_socket(atd0->bus_clock_socket);
	crg->bus_clock_socket(xgate->bus_clock_socket);
	crg->bus_clock_socket(sci0->bus_clock_socket);
	crg->bus_clock_socket(sci1->bus_clock_socket);
	crg->bus_clock_socket(sci2->bus_clock_socket);
	crg->bus_clock_socket(sci3->bus_clock_socket);
	crg->bus_clock_socket(sci4->bus_clock_socket);
	crg->bus_clock_socket(sci5->bus_clock_socket);
	crg->bus_clock_socket(sci6->bus_clock_socket);
	crg->bus_clock_socket(sci7->bus_clock_socket);
	crg->bus_clock_socket(can0->bus_clock_socket);
	crg->bus_clock_socket(can1->bus_clock_socket);
	crg->bus_clock_socket(can2->bus_clock_socket);
	crg->bus_clock_socket(can3->bus_clock_socket);
	crg->bus_clock_socket(can4->bus_clock_socket);
	crg->bus_clock_socket(tranceiver0->bus_clock_socket);
	crg->bus_clock_socket(tranceiver1->bus_clock_socket);
	crg->bus_clock_socket(tranceiver2->bus_clock_socket);
	crg->bus_clock_socket(tranceiver3->bus_clock_socket);
	crg->bus_clock_socket(tranceiver4->bus_clock_socket);
	crg->bus_clock_socket(spi0->bus_clock_socket);
	crg->bus_clock_socket(spi1->bus_clock_socket);
	crg->bus_clock_socket(spi2->bus_clock_socket);
	crg->bus_clock_socket(spi1_stub->bus_clock_socket);
	crg->bus_clock_socket(spi2_stub->bus_clock_socket);
	crg->bus_clock_socket(pim->bus_clock_socket);
	crg->bus_clock_socket(dbg->bus_clock_socket);
	crg->bus_clock_socket(iic0->bus_clock_socket);
	crg->bus_clock_socket(iic1->bus_clock_socket);
	crg->bus_clock_socket(vreg->bus_clock_socket);
	crg->bus_clock_socket(tim->bus_clock_socket);
	crg->bus_clock_socket(reserved->bus_clock_socket);

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> mmc->memory_export;

	*(memoryImportExportTee->memory_import[0]) >> s12xint->memory_export;
	*(memoryImportExportTee->memory_import[1]) >> crg->memory_export;
	*(memoryImportExportTee->memory_import[2]) >> atd1->memory_export;
	*(memoryImportExportTee->memory_import[3]) >> atd0->memory_export;
	*(memoryImportExportTee->memory_import[4]) >> pwm->memory_export;
	*(memoryImportExportTee->memory_import[5]) >> ect->memory_export;
	*(memoryImportExportTee->memory_import[6]) >> pit->memory_export;
	*(memoryImportExportTee->memory_import[7]) >> xgate->memory_export;
	*(memoryImportExportTee->memory_import[8]) >> global_ram->memory_export;
	*(memoryImportExportTee->memory_import[9]) >> global_flash->memory_export;
	*(memoryImportExportTee->memory_import[10]) >> sci0->memory_export;
	*(memoryImportExportTee->memory_import[11]) >> sci1->memory_export;
	*(memoryImportExportTee->memory_import[12]) >> sci2->memory_export;
	*(memoryImportExportTee->memory_import[13]) >> sci3->memory_export;
	*(memoryImportExportTee->memory_import[14]) >> sci4->memory_export;
	*(memoryImportExportTee->memory_import[15]) >> sci5->memory_export;
	*(memoryImportExportTee->memory_import[16]) >> sci6->memory_export;
	*(memoryImportExportTee->memory_import[17]) >> sci7->memory_export;
	*(memoryImportExportTee->memory_import[18]) >> can0->memory_export;
	*(memoryImportExportTee->memory_import[19]) >> can1->memory_export;
	*(memoryImportExportTee->memory_import[20]) >> can2->memory_export;
	*(memoryImportExportTee->memory_import[21]) >> can3->memory_export;
	*(memoryImportExportTee->memory_import[22]) >> can4->memory_export;
	*(memoryImportExportTee->memory_import[23]) >> spi0->memory_export;
	*(memoryImportExportTee->memory_import[24]) >> spi1->memory_export;
	*(memoryImportExportTee->memory_import[25]) >> spi2->memory_export;
	*(memoryImportExportTee->memory_import[26]) >> mpu->memory_export;
	*(memoryImportExportTee->memory_import[27]) >> pim->memory_export;
	*(memoryImportExportTee->memory_import[28]) >> dbg->memory_export;
	*(memoryImportExportTee->memory_import[29]) >> iic0->memory_export;
	*(memoryImportExportTee->memory_import[30]) >> iic1->memory_export;
	*(memoryImportExportTee->memory_import[31]) >> vreg->memory_export;
	*(memoryImportExportTee->memory_import[32]) >> tim->memory_export;
	*(memoryImportExportTee->memory_import[33]) >> reserved->memory_export;

	mmc->memory_import >> memoryImportExportTee->memory_export;

	*(registersTee->registers_import[0]) >> cpu->registers_export;
	*(registersTee->registers_import[1]) >> mmc->registers_export;
	*(registersTee->registers_import[2]) >> s12xint->registers_export;
	*(registersTee->registers_import[3]) >> crg->registers_export;
	*(registersTee->registers_import[4]) >> atd1->registers_export;
	*(registersTee->registers_import[5]) >> atd0->registers_export;
	*(registersTee->registers_import[6]) >> pwm->registers_export;
	*(registersTee->registers_import[7]) >> ect->registers_export;
	*(registersTee->registers_import[8]) >> pit->registers_export;
	*(registersTee->registers_import[9]) >> xgate->registers_export;

	*(registersTee->registers_import[10]) >> global_flash->registers_export;

	*(registersTee->registers_import[11]) >> sci0->registers_export;
	*(registersTee->registers_import[12]) >> sci1->registers_export;
	*(registersTee->registers_import[13]) >> sci2->registers_export;
	*(registersTee->registers_import[14]) >> sci3->registers_export;
	*(registersTee->registers_import[15]) >> sci4->registers_export;
	*(registersTee->registers_import[16]) >> sci5->registers_export;
	*(registersTee->registers_import[17]) >> sci6->registers_export;
	*(registersTee->registers_import[18]) >> sci7->registers_export;

	*(registersTee->registers_import[19]) >> can0->registers_export;
	*(registersTee->registers_import[20]) >> can1->registers_export;
	*(registersTee->registers_import[21]) >> can2->registers_export;
	*(registersTee->registers_import[22]) >> can3->registers_export;
	*(registersTee->registers_import[23]) >> can4->registers_export;
	*(registersTee->registers_import[24]) >> spi0->registers_export;
	*(registersTee->registers_import[25]) >> spi1->registers_export;
	*(registersTee->registers_import[26]) >> spi2->registers_export;
	*(registersTee->registers_import[27]) >> mpu->registers_export;
	*(registersTee->registers_import[28]) >> pim->registers_export;
	*(registersTee->registers_import[29]) >> dbg->registers_export;
	*(registersTee->registers_import[30]) >> iic0->registers_export;
	*(registersTee->registers_import[31]) >> iic1->registers_export;
	*(registersTee->registers_import[32]) >> vreg->registers_export;
	*(registersTee->registers_import[33]) >> tim->registers_export;
	*(registersTee->registers_import[34]) >> reserved->registers_export;

// ***********************************************************
	cpu->loader_import >> loader->loader_export;

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
            
            // Debugger <-> Peripheral connections
            pwm->trap_reporting_import               >> *debugger->trap_reporting_export[0];
            atd0->trap_reporting_import              >> *debugger->trap_reporting_export[0];
            atd1->trap_reporting_import              >> *debugger->trap_reporting_export[0];
            xgate->trap_reporting_import             >> *debugger->trap_reporting_export[0];
            mmc->trap_reporting_import               >> *debugger->trap_reporting_export[0];
                
           // Debugger <-> Loader connections
            debugger->blob_import >> loader->blob_export;
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
  
	if (enable_pim_server)
	{
          // pim server <-> debugger connections
          *debugger->debug_event_listener_import[3] >> pim_server->debug_event_listener_export;
          *debugger->debug_yielding_import[3] >> pim_server->debug_yielding_export;
          
          pim_server->debug_event_trigger_import >> *debugger->debug_event_trigger_export[3];
          pim_server->disasm_import              >> *debugger->disasm_export[3];
          pim_server->memory_import              >> *debugger->memory_export[3];
          pim_server->registers_import           >> *debugger->registers_export[3];
          pim_server->stmt_lookup_import         >> *debugger->stmt_lookup_export[3];
          pim_server->symbol_table_lookup_import >> *debugger->symbol_table_lookup_export[3];
	}

        if (enable_monitor)
          {
            // monitor <-> debugger connections
            *debugger->debug_event_listener_import[2] >> monitor->debug_event_listener_export;
            monitor->debug_event_trigger_import       >> *debugger->debug_event_trigger_export[2];
            monitor->memory_import                    >> *debugger->memory_export[2];
            monitor->registers_import                 >> *debugger->registers_export[2];
            // monitor->stmt_lookup_import               >> *debugger->stmt_lookup_export[2];
            // monitor->symbol_table_lookup_import       >> *debugger->symbol_table_lookup_export[2];
            // monitor->backtrace_import                 >> *debugger->backtrace_export[2];
            // monitor->debug_info_loading_import        >> *debugger->debug_info_loading_export[2];
            // monitor->data_object_lookup_import        >> *debugger->data_object_lookup_export[2];
            // monitor->subprogram_lookup_import         >> *debugger->subprogram_lookup_export[2];
          }
        
	if(enable_profiler)
	{
		*debugger->debug_yielding_import[4]       >> profiler->debug_yielding_export;
		*debugger->debug_event_listener_import[4] >> profiler->debug_event_listener_export;
		profiler->debug_yielding_request_import      >> *debugger->debug_yielding_request_export[4];
		profiler->debug_event_trigger_import         >> *debugger->debug_event_trigger_export[4];
		profiler->disasm_import                      >> *debugger->disasm_export[4];
		profiler->memory_import                      >> *debugger->memory_export[4];
		profiler->registers_import                   >> *debugger->registers_export[4];
		profiler->stmt_lookup_import                 >> *debugger->stmt_lookup_export[4];
		profiler->symbol_table_lookup_import         >> *debugger->symbol_table_lookup_export[4];
		profiler->backtrace_import                   >> *debugger->backtrace_export[4];
		profiler->debug_info_loading_import          >> *debugger->debug_info_loading_export[4];
		profiler->data_object_lookup_import          >> *debugger->data_object_lookup_export[4];
		profiler->subprogram_lookup_import           >> *debugger->subprogram_lookup_export[4];
	}
	
	sci0->char_io_import >> sci_char_io_tee->char_io_export;
	(*sci_char_io_tee->char_io_import[0]) >> sci_telnet->char_io_export;
	(*sci_char_io_tee->char_io_import[1]) >> sci_web_terminal->char_io_export;

	spi0->char_io_import >> spi_char_io_tee->char_io_export;
	(*spi_char_io_tee->char_io_import[0]) >> spi_telnet->char_io_export;
	(*spi_char_io_tee->char_io_import[1]) >> sci_web_terminal->char_io_export;

	*loader->memory_import[0] >>  mmc->memory_export;
	loader->registers_import >> cpu->registers_export;
	cpu->symbol_table_lookup_import >> loader->symbol_table_lookup_export;

	{
		unsigned int i = 0;
		*http_server->http_server_import[i++] >> instrumenter->http_server_export;
		*http_server->http_server_import[i++] >> logger_http_writer->http_server_export;
		if(profiler) *http_server->http_server_import[i++] >> profiler->http_server_export;
		if(sci_web_terminal) *http_server->http_server_import[i++] >> sci_web_terminal->http_server_export;
		if(spi_web_terminal) *http_server->http_server_import[i++] >> spi_web_terminal->http_server_export;
	}
	
	{
		unsigned int i = 0;
		*http_server->registers_import[i++] >> cpu->registers_export;
		*http_server->registers_import[i++] >> mmc->registers_export;
		*http_server->registers_import[i++] >> s12xint->registers_export;
		*http_server->registers_import[i++] >> crg->registers_export;
		*http_server->registers_import[i++] >> atd1->registers_export;
		*http_server->registers_import[i++] >> atd0->registers_export;
		*http_server->registers_import[i++] >> pwm->registers_export;
		*http_server->registers_import[i++] >> ect->registers_export;
		*http_server->registers_import[i++] >> pit->registers_export;
		*http_server->registers_import[i++] >> xgate->registers_export;
		*http_server->registers_import[i++] >> global_flash->registers_export;
		*http_server->registers_import[i++] >> sci0->registers_export;
		*http_server->registers_import[i++] >> sci1->registers_export;
		*http_server->registers_import[i++] >> sci2->registers_export;
		*http_server->registers_import[i++] >> sci3->registers_export;
		*http_server->registers_import[i++] >> sci4->registers_export;
		*http_server->registers_import[i++] >> sci5->registers_export;
		*http_server->registers_import[i++] >> sci6->registers_export;
		*http_server->registers_import[i++] >> sci7->registers_export;
		*http_server->registers_import[i++] >> can0->registers_export;
		*http_server->registers_import[i++] >> can1->registers_export;
		*http_server->registers_import[i++] >> can2->registers_export;
		*http_server->registers_import[i++] >> can3->registers_export;
		*http_server->registers_import[i++] >> can4->registers_export;
		*http_server->registers_import[i++] >> spi0->registers_export;
		*http_server->registers_import[i++] >> spi1->registers_export;
		*http_server->registers_import[i++] >> spi2->registers_export;
		*http_server->registers_import[i++] >> mpu->registers_export;
		*http_server->registers_import[i++] >> pim->registers_export;
		*http_server->registers_import[i++] >> dbg->registers_export;
		*http_server->registers_import[i++] >> iic0->registers_export;
		*http_server->registers_import[i++] >> iic1->registers_export;
		*http_server->registers_import[i++] >> vreg->registers_export;
		*http_server->registers_import[i++] >> tim->registers_export;
		*http_server->registers_import[i++] >> reserved->registers_export;
	}
}

Simulator::~Simulator()
{

// ************
	if (dump_parameters) {
		std::cerr << "Simulation run-time parameters:" << std::endl;
		DumpParameters(std::cerr);
		std::cerr << std::endl;
	}

	if (dump_formulas) {
		
		
		std::cerr << std::endl;
	}

	if (dump_statistics) {

		std::cerr << "Simulation statistics:" << std::endl;
		DumpStatistics(std::cerr);
		std::cerr << std::endl;

		std::cerr << "CPU Clock   (MHz)      : " << (double) (1 / (double) (*cpu)["core-clock"] * 1000000)  << std::endl;
		std::cerr << "CPU CPI                : " << (double) ((uint64_t) (*cpu)["cycles-counter"]) / ((uint64_t) (*cpu)["instruction-counter"]) << std::endl;

		uint64_t total_load = (uint64_t) (*cpu)["instruction-counter"] + (uint64_t) (*cpu)["data-load-counter"];
		uint64_t total_access = total_load + (uint64_t) (*cpu)["store-counter"];
		total_access = ((total_access == 0)? 1: total_access);

		std::cerr << "CPU data-load ratio    : " << (double) ((uint64_t) (*cpu)["data-load-counter"])/(total_access)*100 << " %" << std::endl;
		std::cerr << "CPU data-store ratio   : " << (double) ((uint64_t) (*cpu)["data-store-counter"])/(total_access)*100 << " %" << std::endl;

		std::cerr << endl;

		std::cerr << "XGATE Clock (MHz)      : " << (double) (1 / (double) (*xgate)["core-clock"] * 1000000)  << std::endl;
		std::cerr << "XGATE CPI              : " << (double) ((uint64_t) (*xgate)["cycles-counter"]) / ((uint64_t) (*cpu)["instruction-counter"]) << std::endl;

		total_load = (uint64_t) (*xgate)["instruction-counter"] + (uint64_t) (*xgate)["data-load-counter"];
		total_access = total_load + (uint64_t) (*xgate)["store-counter"];
		total_access = ((total_access == 0)? 1: total_access);

		std::cerr << "XGATE data-load ratio    : " << (double) ((uint64_t) (*xgate)["data-load-counter"])/(total_access)*100 << " %" << std::endl;
		std::cerr << "XGATE data-store ratio   : " << (double) ((uint64_t) (*xgate)["data-store-counter"])/(total_access)*100 << " %" << std::endl;

		std::cerr << std::endl;

		std::cerr << "Target Simulated time  : " << sc_core::sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << std::endl;

		std::cerr << "Host simulation time   : " << spent_time << " seconds" << std::endl;
		std::cerr << "Host simulation speed  : " << (((double) (*cpu)["instruction-counter"] / spent_time) / 1000000.0) << " MIPS" << std::endl;

		std::cerr << "Time dilation          : " << spent_time / sc_core::sc_time_stamp().to_seconds() << " times slower than target machine" << std::endl;
		std::cerr << std::endl;

	}

// ****************************************************


	if (pim_server) { delete pim_server; pim_server = NULL; }

	if (registersTee) { delete registersTee; registersTee = NULL; }
	if (memoryImportExportTee) { delete memoryImportExportTee; memoryImportExportTee = NULL; }

	if(gdb_server) { delete gdb_server; gdb_server = NULL; }
	if(inline_debugger) { delete inline_debugger; inline_debugger = NULL; }
	if(profiler) { delete profiler; profiler = NULL; }
	if(debugger) { delete debugger; debugger = NULL; }

	if (sci_telnet) { delete sci_telnet; sci_telnet = NULL; }
	if (spi_telnet) { delete spi_telnet; spi_telnet = NULL; }
	
	if(http_server) { delete http_server; http_server = NULL; }
	if(profiler) { delete profiler; profiler = NULL; }
	if(sci_web_terminal) { delete sci_web_terminal; sci_web_terminal = NULL; }
	if(spi_web_terminal) { delete spi_web_terminal; spi_web_terminal = NULL; }
	if(sci_char_io_tee) { delete sci_char_io_tee; sci_char_io_tee = NULL; }
	if(spi_char_io_tee) { delete spi_char_io_tee; spi_char_io_tee = NULL; }

	if (host_time) { delete host_time; host_time = NULL; }
	if(sim_time) { delete sim_time; sim_time = NULL; }

	if (monitor) { delete monitor; monitor = NULL; }

	if(loader) delete loader;

#ifdef HAVE_RTBCOB
	if (rtbStub) { delete rtbStub; rtbStub = NULL; }
#else
	if (xml_atd_pwm_stub) { delete xml_atd_pwm_stub; xml_atd_pwm_stub = NULL; }
#endif

	if (can_stub) { delete can_stub; can_stub = NULL; }
	if (tranceiver0) { delete tranceiver0; tranceiver0 = NULL; }
	if (tranceiver1) { delete tranceiver1; tranceiver1 = NULL; }
	if (tranceiver2) { delete tranceiver2; tranceiver2 = NULL; }
	if (tranceiver3) { delete tranceiver3; tranceiver3 = NULL; }
	if (tranceiver4) { delete tranceiver4; tranceiver4 = NULL; }

	if(global_ram) { delete global_ram; global_ram = NULL; }
	if(global_flash) { delete global_flash; global_flash = NULL; }

	if (xgate) { delete xgate; xgate = NULL; }
	if (crg) { delete crg; crg = NULL; }
	if (ect) { delete ect; ect = NULL; }
	if (pit) { delete pit; pit = NULL; }
	if (pwm) { delete pwm; pwm = NULL; }
	if (atd1) { delete atd1; atd1 = NULL; }
	if (atd0) { delete atd0; atd0 = NULL; }
	if (s12xint) { delete s12xint; s12xint = NULL; }
	if (mmc) { delete mmc; mmc = NULL; }
	if (mpu) { delete mpu; mpu = NULL; }
	if (sci0) { delete sci0; sci0 = NULL; }
	if (sci1) { delete sci1; sci1 = NULL; }
	if (sci2) { delete sci2; sci2 = NULL; }
	if (sci3) { delete sci3; sci3 = NULL; }
	if (sci4) { delete sci4; sci4 = NULL; }
	if (sci5) { delete sci5; sci5 = NULL; }
	if (sci6) { delete sci6; sci6 = NULL; }
	if (sci7) { delete sci7; sci7 = NULL; }

	if (can0) { delete can0; can0 = NULL; }
	if (can1) { delete can1; can1 = NULL; }
	if (can2) { delete can2; can2 = NULL; }
	if (can3) { delete can3; can3 = NULL; }
	if (can4) { delete can4; can4 = NULL; }

	if (spi0) { delete spi0; spi0 = NULL; }
	if (spi1) { delete spi1; spi1 = NULL; }
	if (spi2) { delete spi2; spi2 = NULL; }
	if (spi1_stub) { delete spi1_stub; spi1_stub = NULL; }
	if (spi2_stub) { delete spi2_stub; spi2_stub = NULL; }

	if (pim)  { delete pim; pim = NULL; }
	if (dbg)  { delete dbg; dbg = NULL; }
	if (iic0) { delete iic0; iic0 = NULL; }
	if (iic1) { delete iic1; iic1 = NULL; }

	if (vreg) { delete vreg; vreg = NULL; }
	if (tim) { delete tim; tim = NULL; }

	if (reserved) { delete reserved; reserved = NULL; }

	if(cpu) { delete cpu; cpu = NULL; }
	
	if(instrumenter) { delete instrumenter; instrumenter = NULL; }
	
	if(logger_console_printer) { delete logger_console_printer; logger_console_printer = NULL; }
	if(logger_text_file_writer) { delete logger_text_file_writer; logger_text_file_writer = NULL; }
	if(logger_http_writer) { delete logger_http_writer; logger_http_writer = NULL; }
	if(logger_xml_file_writer) { delete logger_xml_file_writer; logger_xml_file_writer = NULL; }
	if(logger_netstream_writer) { delete logger_netstream_writer; logger_netstream_writer = NULL; }
}


Simulator::SetupStatus Simulator::Setup()
{
	Simulator::SetupStatus result = unisim::kernel::Simulator::Setup();

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
	
	http_server->AddJSAction(
		unisim::service::interfaces::ToolbarOpenTabAction(
		/* name */      sci_web_terminal->GetName(),
		/* label */     "<img src=\"/unisim/service/web_terminal/icon_term0.svg\" alt=\"TERM0\">",
		/* tips */      (*sci_web_terminal)["title"],
		/* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
		/* uri */       sci_web_terminal->URI()
	));

	http_server->AddJSAction(
		unisim::service::interfaces::ToolbarOpenTabAction(
		/* name */      spi_web_terminal->GetName(),
		/* label */     "<img src=\"/unisim/service/web_terminal/icon_term1.svg\" alt=\"TERM1\">",
		/* tips */      (*spi_web_terminal)["title"],
		/* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
		/* uri */       spi_web_terminal->URI()
	));
	
	address_t cpu_address;
	uint8_t page = 0;

	if (entry_point == 0) {
		address_t reset_addr;
		const address_t reset_vect = 0xFFFE;
		cpu->ReadMemory(reset_vect, &reset_addr, 2);

		entry_point = unisim::util::endian::BigEndian2Host(reset_addr);
		mmc->splitPagedAddress(entry_point, page, cpu_address);
	}
	else {
		cpu_address = (address_t) entry_point;
	}

	std::cout << "entry-point 0x" << std::hex << entry_point << std::dec << std::endl;
	cpu->setEntryPoint(cpu_address);

	isStop = false;

	return result;
}

bool Simulator::RunSample(double inVal) {

	if (!isStop) {
		std::cerr << "Starting simulation RunSample ..." << std::endl;

		double time_start = host_time->GetTime();

		try
		{
			sc_core::sc_start(inVal, SC_MS);
		}
		catch(std::runtime_error& e)
		{
			std::cerr << "FATAL ERROR! an abnormal error occurred during simulation. Bailing out..." << std::endl;
			std::cerr << e.what() << std::endl;
		}

		double time_stop = host_time->GetTime();

		spent_time += time_stop - time_start;

		std::cerr << "Finishing simulation RunSample " << std::endl;

		return true;
	}

	return false;
}

void Simulator::Run() {

	std::cerr << "Starting simulation ..." << std::endl;

	double time_start = host_time->GetTime();

	try
	{
		sc_core::sc_start();
	}
	catch(std::runtime_error& e)
	{
		std::cerr << "FATAL ERROR! an abnormal error occurred during simulation. Bailing out..." << std::endl;
		std::cerr << e.what() << std::endl;
	}

	double time_stop = host_time->GetTime();

	spent_time += time_stop - time_start;

	std::cerr << "Simulation finished" << endl << endl;


}

void Simulator::Stop(Object *object, int _exit_status, bool asynchronous)
{
	isStop = true;

	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}
	std::cerr << "Program exited with status " << exit_status << std::endl;
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

Simulator::LoadRatioStatistic::LoadRatioStatistic(Simulator& _sim)
  : Variable<double>("data-load-ratio %", 0, _sim.null_stat_var, VariableBase::VAR_STATISTIC, "Data Load Ratio"), sim(_sim) {}

void
Simulator::LoadRatioStatistic::Get(double& value)
{
  CPU& cpu( *sim.cpu );
  double total_access = double(cpu["instruction-counter"]) + double(cpu["data-load-counter"]) + double(cpu["store-counter"]);
  value = double(cpu["data-load-counter"])/total_access*100;
}

Simulator::StoreRatioStatistic::StoreRatioStatistic(Simulator& _sim)
  : Variable<double>("data-store-ratio %", 0, _sim.null_stat_var, VariableBase::VAR_STATISTIC, "Data Store Ratio"), sim(_sim) {}

void
Simulator::StoreRatioStatistic::Get(double& value)
{
  CPU& cpu( *sim.cpu );
  double total_access = double(cpu["instruction-counter"]) + double(cpu["data-load-counter"]) + double(cpu["store-counter"]);
  value = double(cpu["data-store-counter"])/total_access*100;
}

void Simulator::LoadBuiltInConfig(unisim::kernel::Simulator *simulator)
{
	new unisim::kernel::config::xml::XMLConfigFileHelper(simulator);
	new unisim::kernel::config::ini::INIConfigFileHelper(simulator);
	new unisim::kernel::config::json::JSONConfigFileHelper(simulator);
	
	// meta information
	simulator->SetVariable("program-name", "UNISIM star12x");
	simulator->SetVariable("copyright", "Copyright (C) 2008-2010, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Reda Nouacer <reda.nouacer@cea.fr>");

#ifdef HAVE_CONFIG_H
	simulator->SetVariable("version", VERSION);
#endif

	simulator->SetVariable("description", "UNISIM star12x, a Star12X System-on-Chip simulator with support of ELF32 binaries and s19 hex files, and targeted for automotive applications");

	simulator->SetVariable("enable-pim-server", false);
	simulator->SetVariable("enable-gdb-server", false);
	simulator->SetVariable("enable-inline-debugger", false);
	simulator->SetVariable("dump-parameters", false);
	simulator->SetVariable("dump-formulas", false);
	simulator->SetVariable("dump-statistics", true);

	simulator->SetVariable("endian", "big");
	simulator->SetVariable("program-counter-name", "CPU.PC");

	simulator->SetVariable("entry-point", 0x0000);

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================



	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - PIM Server run-time configuration
	simulator->SetVariable("pim.host", "127.0.0.1");	// 127.0.0.1 is the default localhost-name
	simulator->SetVariable("pim.tcp-port", 1234);
	simulator->SetVariable("pim.filename", "pim.xml");

	simulator->SetVariable("pim-server.tcp-port", 0);
	simulator->SetVariable("pim-server.architecture-description-filename", "unisim/service/debug/gdb_server/gdb_hcs12x.xml");
	simulator->SetVariable("pim-server.host", "127.0.0.1");	// 127.0.0.1 is the default localhost-name

	//  - GDB Server run-time configuration
	simulator->SetVariable("gdb-server.tcp-port", 0);
	simulator->SetVariable("gdb-server.architecture-description-filename", "unisim/service/debug/gdb_server/gdb_hcs12x.xml");
	simulator->SetVariable("gdb-server.host", "127.0.0.1");	// 127.0.0.1 is the default localhost-name

	simulator->SetVariable("debugger.parse-dwarf", true);
	simulator->SetVariable("debugger.dwarf-register-number-mapping-filename", "unisim/util/debug/dwarf/68hc12_dwarf_register_number_mapping.xml");

	simulator->SetVariable("Monitor.xml-spec-file-path", "xml_spec_file_path.xml");
	simulator->SetVariable("Monitor.property-list", "");

	// - Loader memory router
	std::stringstream sstr_loader_mapping;
	sstr_loader_mapping << "MMC:0x000000-0xFFFFFF" << std::dec;
	simulator->SetVariable("loader.memory-mapper.mapping", sstr_loader_mapping.str().c_str()); // 256 MB RAM / 256 KB BRAM / 32 MB Flash memory

//	simulator->SetVariable("S19_Loader.filename", "");
//	simulator->SetVariable("elf32-loader.filename", "");
//	simulator->SetVariable("elf32-loader.force-use-virtual-address", true);
//	simulator->SetVariable("elf32-loader.initialize-extra-segment-bytes", false);

	simulator->SetVariable("atd-pwm-stub.anx-stimulus-period", 80000000); // 80 us
	simulator->SetVariable("atd-pwm-stub.pwm-fetch-period", 1e9); // 1 ms
	simulator->SetVariable("atd-pwm-stub.atd0-anx-stimulus-file", "ATD.xml");
	simulator->SetVariable("atd-pwm-stub.atd0-anx-start-channel", 0);
	simulator->SetVariable("atd-pwm-stub.atd0-anx-wrap-around-channel", 7);
	simulator->SetVariable("atd-pwm-stub.atd1-anx-stimulus-file", "ATD.xml");
	simulator->SetVariable("atd-pwm-stub.atd1-anx-start-channel", 0);
	simulator->SetVariable("atd-pwm-stub.atd1-anx-wrap-around-channel", 15);
	simulator->SetVariable("atd-pwm-stub.trace-enabled", false);
	simulator->SetVariable("atd-pwm-stub.cosim-enabled", false);
	simulator->SetVariable("atd-pwm-stub.atd0-xml-enabled", false);
	simulator->SetVariable("atd-pwm-stub.atd1-xml-enabled", false);
	simulator->SetVariable("atd-pwm-stub.atd0-rand-enabled", false);
	simulator->SetVariable("atd-pwm-stub.atd1-rand-enabled", false);

	simulator->SetVariable("CAN-STUB.trace-enabled", false);
	simulator->SetVariable("CAN-STUB.cosim-enabled", false);
	simulator->SetVariable("CAN-STUB.xml-enabled", false);
	simulator->SetVariable("CAN-STUB.rand-enabled", false);
	simulator->SetVariable("CAN-STUB.can-rx-stimulus-period", 20000);
	simulator->SetVariable("CAN-STUB.can-rx-stimulus-file", "");
	simulator->SetVariable("CAN-STUB.broadcast-enabled", true);

	simulator->SetVariable("ATD0.bus-cycle-time", 250000);
	simulator->SetVariable("ATD0.base-address", 0x2c0);
	simulator->SetVariable("ATD0.interrupt-offset", 0xd2);
	simulator->SetVariable("ATD0.vrl", 0.000000e+00);
	simulator->SetVariable("ATD0.vrh", 5.120000e+00);
	simulator->SetVariable("ATD0.debug-enabled", false);
	simulator->SetVariable("ATD0.vih", 3.250000e+00);
	simulator->SetVariable("ATD0.vil", 1.750000e+00);
	simulator->SetVariable("ATD0.Has-External-Trigger", false);

	simulator->SetVariable("ATD1.bus-cycle-time", 250000);
	simulator->SetVariable("ATD1.base-address", 0x80);
	simulator->SetVariable("ATD1.interrupt-offset", 0xd0);
	simulator->SetVariable("ATD1.vrl", 0.000000e+00);
	simulator->SetVariable("ATD1.vrh", 5.120000e+00);
	simulator->SetVariable("ATD1.debug-enabled", false);
	simulator->SetVariable("ATD1.vih", 3.250000e+00);
	simulator->SetVariable("ATD1.vil", 1.750000e+00);
	simulator->SetVariable("ATD1.Has-External-Trigger", false);

	simulator->SetVariable("XGATE.version", "V3");
	simulator->SetVariable("XGATE.base-address", 0x0380);
	simulator->SetVariable("XGATE.software_channel_id[0]", 0x39);
	simulator->SetVariable("XGATE.software_channel_id[1]", 0x38);
	simulator->SetVariable("XGATE.software_channel_id[2]", 0x37);
	simulator->SetVariable("XGATE.software_channel_id[3]", 0x36);
	simulator->SetVariable("XGATE.software_channel_id[4]", 0x35);
	simulator->SetVariable("XGATE.software_channel_id[5]", 0x34);
	simulator->SetVariable("XGATE.software_channel_id[6]", 0x33);
	simulator->SetVariable("XGATE.software_channel_id[7]", 0x32);
	simulator->SetVariable("XGATE.software-error-interrupt", 0x62);

	simulator->SetVariable("XGATE.enable-trace", false);
	simulator->SetVariable("XGATE.enable-file-trace", false),
	simulator->SetVariable("XGATE.verbose-all", false);
	simulator->SetVariable("XGATE.verbose-setup", false);
	simulator->SetVariable("XGATE.verbose-step", false);
	simulator->SetVariable("XGATE.verbose-dump-regs-start", false);
	simulator->SetVariable("XGATE.verbose-dump-regs-end", false);
	simulator->SetVariable("XGATE.verbose-exception", false);
	simulator->SetVariable("XGATE.requires-memory-access-reporting", false);
	simulator->SetVariable("XGATE.requires-finished-instruction-reporting", false);
	simulator->SetVariable("XGATE.debug-enabled", false);
	simulator->SetVariable("XGATE.max-inst", 0xffffffffffffffffULL);
	simulator->SetVariable("XGATE.nice-time", "1 ms");
	simulator->SetVariable("XGATE.core-clock", 125000);
	simulator->SetVariable("XGATE.verbose-tlm-bus-synchronize", false);
	simulator->SetVariable("XGATE.verbose-tlm-run-thread", false);
	simulator->SetVariable("XGATE.verbose-tlm-commands", false);
	simulator->SetVariable("XGATE.trap-on-instruction-counter", -1);
	simulator->SetVariable("XGATE.enable-fine-timing", true);

	simulator->SetVariable("CPU.enable-trace", false);
	simulator->SetVariable("CPU.enable-file-trace", false),
	simulator->SetVariable("CPU.verbose-all", false);
	simulator->SetVariable("CPU.verbose-setup", false);
	simulator->SetVariable("CPU.verbose-step", false);
	simulator->SetVariable("CPU.verbose-dump-regs-start", false);
	simulator->SetVariable("CPU.verbose-dump-regs-end", false);
	simulator->SetVariable("CPU.verbose-exception", false);
	simulator->SetVariable("CPU.requires-memory-access-reporting", false);
	simulator->SetVariable("CPU.requires-finished-instruction-reporting", false);
	simulator->SetVariable("CPU.debug-enabled", false);
	simulator->SetVariable("CPU.max-inst", 0xffffffffffffffffULL);
	simulator->SetVariable("CPU.nice-time", "1 ms");
	simulator->SetVariable("CPU.core-clock", 125000);
	simulator->SetVariable("CPU.verbose-tlm-bus-synchronize", false);
	simulator->SetVariable("CPU.verbose-tlm-run-thread", false);
	simulator->SetVariable("CPU.verbose-tlm-commands", false);
	simulator->SetVariable("CPU.trap-on-instruction-counter", -1);
	simulator->SetVariable("CPU.enable-fine-timing", true);

	simulator->SetVariable("CRG.oscillator-clock", 125000); // 8 MHz
	simulator->SetVariable("CRG.base-address", 0x34);
	simulator->SetVariable("CRG.interrupt-offset-rti", 0xf0);
	simulator->SetVariable("CRG.interrupt-offset-pll-lock", 0xc6);
	simulator->SetVariable("CRG.interrupt-offset-self-clock-mode", 0xc4);
	simulator->SetVariable("CRG.debug-enabled", false);
	simulator->SetVariable("CRG.pll-stabilization-delay", 0.24);
	simulator->SetVariable("CRG.self-clock-mode-clock", 125000);

	simulator->SetVariable("ECT.bus-cycle-time", 250000);
	simulator->SetVariable("ECT.base-address", 0x40);
	simulator->SetVariable("ECT.interrupt-offset-channel0", 0xee);
	simulator->SetVariable("ECT.interrupt-offset-timer-overflow", 0xde);
	simulator->SetVariable("ECT.pulse-accumulatorA-overflow-interrupt", 0xDC);
	simulator->SetVariable("ECT.pulse-accumulatorB-overflow-interrupt", 0xC8);
	simulator->SetVariable("ECT.pulse-accumulatorA-input-edge-interrupt", 0xDA);
	simulator->SetVariable("ECT.modulus-counter-interrupt", 0xCA);
	simulator->SetVariable("ECT.debug-enabled", false);

	simulator->SetVariable("ECT.built-in-signal-generator-enable", false);
	simulator->SetVariable("ECT.built-in-signal-generator-period", 80000);

	simulator->SetVariable("PIT.bus-cycle-time", 250000);
	simulator->SetVariable("PIT.base-address", 0x0340);
	simulator->SetVariable("PIT.interrupt-offset-channel[0]", 0x7A);
	simulator->SetVariable("PIT.interrupt-offset-channel[1]", 0x78);
	simulator->SetVariable("PIT.interrupt-offset-channel[2]", 0x76);
	simulator->SetVariable("PIT.interrupt-offset-channel[3]", 0x74);
	simulator->SetVariable("PIT.interrupt-offset-channel[4]", 0x5E);
	simulator->SetVariable("PIT.interrupt-offset-channel[5]", 0x5C);
	simulator->SetVariable("PIT.interrupt-offset-channel[6]", 0x5A);
	simulator->SetVariable("PIT.interrupt-offset-channel[7]", 0x58);
	simulator->SetVariable("PIT.debug-enabled", false);

	simulator->SetVariable("sci-telnet.tcp-port", 1235);
	simulator->SetVariable("sci-enable-telnet", false);

	simulator->SetVariable("SCI2.bus-cycle-time", 250000);
	simulator->SetVariable("SCI2.base-address", 0x00B8);
	simulator->SetVariable("SCI2.interrupt-offset", 0x8A);
	simulator->SetVariable("SCI2.txd-pin-enable", true);
	simulator->SetVariable("SCI2.telnet-enabled", false);
	simulator->SetVariable("SCI2.debug-enabled", false);
	simulator->SetVariable("SCI2.TXD", true);
	simulator->SetVariable("SCI2.RXD", true);

	simulator->SetVariable("SCI3.bus-cycle-time", 250000);
	simulator->SetVariable("SCI3.base-address", 0x00C0);
	simulator->SetVariable("SCI3.interrupt-offset", 0x88);
	simulator->SetVariable("SCI3.txd-pin-enable", true);
	simulator->SetVariable("SCI3.telnet-enabled", false);
	simulator->SetVariable("SCI3.debug-enabled", false);
	simulator->SetVariable("SCI3.TXD", true);
	simulator->SetVariable("SCI3.RXD", true);

	simulator->SetVariable("SCI0.bus-cycle-time", 250000);
	simulator->SetVariable("SCI0.base-address", 0x00C8);
	simulator->SetVariable("SCI0.interrupt-offset", 0xD6);
	simulator->SetVariable("SCI0.txd-pin-enable", true);
	simulator->SetVariable("SCI0.telnet-enabled", false);
	simulator->SetVariable("SCI0.debug-enabled", false);
	simulator->SetVariable("SCI0.TXD", true);
	simulator->SetVariable("SCI0.RXD", true);

	simulator->SetVariable("SCI1.bus-cycle-time", 250000);
	simulator->SetVariable("SCI1.base-address", 0x00D0);
	simulator->SetVariable("SCI1.interrupt-offset", 0xD4);
	simulator->SetVariable("SCI1.txd-pin-enable", true);
	simulator->SetVariable("SCI1.telnet-enabled", false);
	simulator->SetVariable("SCI1.debug-enabled", false);
	simulator->SetVariable("SCI1.TXD", true);
	simulator->SetVariable("SCI1.RXD", true);

	simulator->SetVariable("SCI4.bus-cycle-time", 250000);
	simulator->SetVariable("SCI4.base-address", 0x0130);
	simulator->SetVariable("SCI4.interrupt-offset", 0x86);
	simulator->SetVariable("SCI4.txd-pin-enable", true);
	simulator->SetVariable("SCI4.telnet-enabled", false);
	simulator->SetVariable("SCI4.debug-enabled", false);
	simulator->SetVariable("SCI4.TXD", true);
	simulator->SetVariable("SCI4.RXD", true);

	simulator->SetVariable("SCI5.bus-cycle-time", 250000);
	simulator->SetVariable("SCI5.base-address", 0x0138);
	simulator->SetVariable("SCI5.interrupt-offset", 0x84);
	simulator->SetVariable("SCI5.txd-pin-enable", true);
	simulator->SetVariable("SCI5.telnet-enabled", false);
	simulator->SetVariable("SCI5.debug-enabled", false);
	simulator->SetVariable("SCI5.TXD", true);
	simulator->SetVariable("SCI5.RXD", true);

	simulator->SetVariable("CAN0.bus-cycle-time", 250000);
	simulator->SetVariable("CAN0.base-address", 0x0140);
	simulator->SetVariable("CAN0.transmit-interrupt-offset", 0xB0);
	simulator->SetVariable("CAN0.receive-interrupt-offset", 0xB2);
	simulator->SetVariable("CAN0.errors-interrupt-offset", 0xB4);
	simulator->SetVariable("CAN0.wakeup-interrupt-offset", 0xB6);
	simulator->SetVariable("CAN0.telnet-enabled", false);
	simulator->SetVariable("CAN0.debug-enabled", false);
	simulator->SetVariable("CAN0.TXD", true);
	simulator->SetVariable("CAN0.RXD", true);

	simulator->SetVariable("Tranceiver0.bus-cycle-time", 250000);
	simulator->SetVariable("Tranceiver0.reset-interrupt", 0xFE);
	simulator->SetVariable("Tranceiver0.int-interrupt", 0xF2);
	simulator->SetVariable("Tranceiver0.debug-enabled", false);

	simulator->SetVariable("CAN1.bus-cycle-time", 250000);
	simulator->SetVariable("CAN1.base-address", 0x0180);
	simulator->SetVariable("CAN1.transmit-interrupt-offset", 0xA8);
	simulator->SetVariable("CAN1.receive-interrupt-offset", 0xAA);
	simulator->SetVariable("CAN1.errors-interrupt-offset", 0xAC);
	simulator->SetVariable("CAN1.wakeup-interrupt-offset", 0xAE);
	simulator->SetVariable("CAN1.telnet-enabled", false);
	simulator->SetVariable("CAN1.debug-enabled", false);
	simulator->SetVariable("CAN1.TXD", true);
	simulator->SetVariable("CAN1.RXD", true);

	simulator->SetVariable("Tranceiver1.bus-cycle-time", 250000);
	simulator->SetVariable("Tranceiver1.reset-interrupt", 0xFE);
	simulator->SetVariable("Tranceiver1.int-interrupt", 0xF2);
	simulator->SetVariable("Tranceiver1.debug-enabled", false);

	simulator->SetVariable("CAN2.bus-cycle-time", 250000);
	simulator->SetVariable("CAN2.base-address", 0x01C0);
	simulator->SetVariable("CAN2.transmit-interrupt-offset", 0xA0);
	simulator->SetVariable("CAN2.receive-interrupt-offset", 0xA2);
	simulator->SetVariable("CAN2.errors-interrupt-offset", 0xA4);
	simulator->SetVariable("CAN2.wakeup-interrupt-offset", 0xA6);
	simulator->SetVariable("CAN2.telnet-enabled", false);
	simulator->SetVariable("CAN2.debug-enabled", false);
	simulator->SetVariable("CAN2.TXD", true);
	simulator->SetVariable("CAN2.RXD", true);

	simulator->SetVariable("Tranceiver2.bus-cycle-time", 250000);
	simulator->SetVariable("Tranceiver2.reset-interrupt", 0xFE);
	simulator->SetVariable("Tranceiver2.int-interrupt", 0xF2);
	simulator->SetVariable("Tranceiver2.debug-enabled", false);

	simulator->SetVariable("CAN3.bus-cycle-time", 250000);
	simulator->SetVariable("CAN3.base-address", 0x0200);
	simulator->SetVariable("CAN3.transmit-interrupt-offset", 0x98);
	simulator->SetVariable("CAN3.receive-interrupt-offset", 0x9A);
	simulator->SetVariable("CAN3.errors-interrupt-offset", 0x9C);
	simulator->SetVariable("CAN3.wakeup-interrupt-offset", 0x9E);
	simulator->SetVariable("CAN3.telnet-enabled", false);
	simulator->SetVariable("CAN3.debug-enabled", false);
	simulator->SetVariable("CAN3.TXD", true);
	simulator->SetVariable("CAN3.RXD", true);

	simulator->SetVariable("Tranceiver3.bus-cycle-time", 250000);
	simulator->SetVariable("Tranceiver3.reset-interrupt", 0xFE);
	simulator->SetVariable("Tranceiver3.int-interrupt", 0xF2);
	simulator->SetVariable("Tranceiver3.debug-enabled", false);

	simulator->SetVariable("CAN4.bus-cycle-time", 250000);
	simulator->SetVariable("CAN4.base-address", 0x0280);
	simulator->SetVariable("CAN4.transmit-interrupt-offset", 0x48);
	simulator->SetVariable("CAN4.receive-interrupt-offset", 0x4A);
	simulator->SetVariable("CAN4.errors-interrupt-offset", 0x4C);
	simulator->SetVariable("CAN4.wakeup-interrupt-offset", 0x4E);
	simulator->SetVariable("CAN4.telnet-enabled", false);
	simulator->SetVariable("CAN4.debug-enabled", false);
	simulator->SetVariable("CAN4.TXD", true);
	simulator->SetVariable("CAN4.RXD", true);

	simulator->SetVariable("Tranceiver4.bus-cycle-time", 250000);
	simulator->SetVariable("Tranceiver4.reset-interrupt", 0xFE);
	simulator->SetVariable("Tranceiver4.int-interrupt", 0xF2);
	simulator->SetVariable("Tranceiver4.debug-enabled", false);

	simulator->SetVariable("spi-telnet.tcp-port", 1236);
	simulator->SetVariable("spi-enable-telnet", false);

	simulator->SetVariable("SPI0.bus-cycle-time", 250000);
	simulator->SetVariable("SPI0.base-address", 0x00D8);
	simulator->SetVariable("SPI0.interrupt-offset", 0xD8);
	simulator->SetVariable("SPI0.debug-enabled", false);
	simulator->SetVariable("SPI0.txd-pin-enable", true);
	simulator->SetVariable("SPI0.telnet-enabled", false);

	simulator->SetVariable("SPI1.bus-cycle-time", 250000);
	simulator->SetVariable("SPI1.base-address", 0x00F0);
	simulator->SetVariable("SPI1.interrupt-offset", 0xBE);
	simulator->SetVariable("SPI1.debug-enabled", false);
	simulator->SetVariable("SPI1.txd-pin-enable", true);
	simulator->SetVariable("SPI1.telnet-enabled", false);

	simulator->SetVariable("SPI2.bus-cycle-time", 250000);
	simulator->SetVariable("SPI2.base-address", 0x00F8);
	simulator->SetVariable("SPI2.interrupt-offset", 0xBC);
	simulator->SetVariable("SPI2.debug-enabled", false);
	simulator->SetVariable("SPI2.txd-pin-enable", true);
	simulator->SetVariable("SPI2.telnet-enabled", false);

	simulator->SetVariable("SPI1-STUB.bus-cycle-time", 250000);
	simulator->SetVariable("SPI1-STUB.debug-enabled", false);
	simulator->SetVariable("SPI1-STUB.txd-pin-enable", true);
	simulator->SetVariable("SPI1-STUB.telnet-enabled", false);

	simulator->SetVariable("SPI2-STUB.bus-cycle-time", 250000);
	simulator->SetVariable("SPI2-STUB.debug-enabled", false);
	simulator->SetVariable("SPI2-STUB.txd-pin-enable", true);
	simulator->SetVariable("SPI2-STUB.telnet-enabled", false);

	simulator->SetVariable("MMC.version", "V4");
	simulator->SetVariable("MMC.debug-enabled", false);
	simulator->SetVariable("MMC.mode", 0x80);
	simulator->SetVariable("MMC.mmcctl1", 0x5);
	simulator->SetVariable("MMC.address-encoding", 0x0);
	simulator->SetVariable("MMC.ppage-address", 0x15); // ppage address for S12XE is 0x15

	simulator->SetVariable("PIM.debug-enabled", false);
	simulator->SetVariable("PIM.bus-cycle-time", 250000);

	simulator->SetVariable("DBG.debug-enabled", false);
	simulator->SetVariable("DBG.bus-cycle-time", 250000);

	simulator->SetVariable("IIC0.base-address", 0x00E0);
	simulator->SetVariable("IIC0.debug-enabled", false);
	simulator->SetVariable("IIC0.bus-cycle-time", 250000);

	simulator->SetVariable("IIC1.base-address", 0x00B0);
	simulator->SetVariable("IIC1.debug-enabled", false);
	simulator->SetVariable("IIC1.bus-cycle-time", 250000);

	simulator->SetVariable("VREG.base-address", 0x02F0);
	simulator->SetVariable("VREG.debug-enabled", false);
	simulator->SetVariable("VREG.bus-cycle-time", 250000);

	simulator->SetVariable("TIM.base-address", 0x03D0);
	simulator->SetVariable("TIM.debug-enabled", false);
	simulator->SetVariable("TIM.bus-cycle-time", 250000);

	simulator->SetVariable("RESERVED.debug-enabled", false);
	simulator->SetVariable("RESERVED.bus-cycle-time", 250000);

/*
	simulator->SetVariable("MMC.memory-map",
"0,0034,003F;1,0040,007F;2,0080,00AF;3,00B8,00BF;4,00C0,00C7;5,00C8,00CF;\
6,00D0,00D7;7,00D8,00DF;8,00F0,00F7;9,00F8,00FF;10,0100,0113;11,0114,011F;\
12,0120,012F;13,0130,0137;14,0138,013F;15,0140,017F;16,0180,01BF;17,01C0,01FF;\
18,0200,023F;19,0280,02BF;20,02C0,02EF;21,0300,0327;22,330,337;23,338,33F;\
24,0340,0367;25,0380,03BF;26,0007FF,0FFFFF;27,100000,13FFFF;27,400000,7FFFFF");
*/

/** VERRY IMPORTANT: The MMC memory map indexing (i.e. order) depend on the binding to "mmc->init_socket" **/

	// index 27 reference two memory regions (EEPROM, FLASH). For S12XE, the eeprom is emulated by flash
	simulator->SetVariable("MMC.memory-map",
"0,0034,003F;\
1,0040,007F;\
2,0080,00AF;\
3,00B8,00BF;\
4,00C0,00C7;\
5,00C8,00CF;\
6,00D0,00D7;\
7,00D8,00DF;\
8,00F0,00F7;\
9,00F8,00FF;\
10,0100,0113;\
11,0114,011F;\
12,0120,012F;\
13,0130,0137;\
14,0138,013F;\
15,0140,017F;\
16,0180,01BF;\
17,01C0,01FF;\
18,0200,023F;\
19,0280,02BF;\
20,02C0,02EF;\
21,0300,0327;\
22,0330,0337;\
23,0338,033F;\
24,0340,0367;\
25,0380,03BF;\
26,0007FF,0FFFFF;\
27,100000,13FFFF;\
27,400000,7FFFFF;\
28,0000,0009;\
28,000C,000D;\
28,001C,001F;\
28,0032,0033;\
28,0240,027F;\
28,0368,037F;\
29,0020,002F;\
30,00E0,00EF;\
31,00B0,00BF;\
32,02F0,02F7;\
33,03D0,03FF;\
34,0018,0019;\
34,0030,0031;\
34,00E8,00EF;\
34,00E8,00EF;\
34,02F8,02FF;\
34,0328,032F;\
34,03C0,03CF;\
34,0400,07FF\
"
	);

	simulator->SetVariable("MPU.debug-enabled", false);
	simulator->SetVariable("MPU.base-address", 0x0114);
	simulator->SetVariable("MPU.interrupt-offset", 0x14);

	simulator->SetVariable("PWM.bus-cycle-time", 250000);
	simulator->SetVariable("PWM.base-address", 0x300);
	simulator->SetVariable("PWM.interrupt-offset", 0x8c);
	simulator->SetVariable("PWM.debug-enabled", false);

	simulator->SetVariable("SCI6.bus-cycle-time", 250000);
	simulator->SetVariable("SCI6.base-address", 0x0330);
	simulator->SetVariable("SCI6.interrupt-offset", 0xC2);
	simulator->SetVariable("SCI6.txd-pin-enable", true);
	simulator->SetVariable("SCI6.telnet-enabled", false);
	simulator->SetVariable("SCI6.debug-enabled", false);
	simulator->SetVariable("SCI6.TXD", true);
	simulator->SetVariable("SCI6.RXD", true);

	simulator->SetVariable("SCI7.bus-cycle-time", 250000);
	simulator->SetVariable("SCI7.base-address", 0x0338);
	simulator->SetVariable("SCI7.interrupt-offset", 0x56);
	simulator->SetVariable("SCI7.txd-pin-enable", true);
	simulator->SetVariable("SCI7.telnet-enabled", false);
	simulator->SetVariable("SCI7.debug-enabled", false);
	simulator->SetVariable("SCI7.TXD", true);
	simulator->SetVariable("SCI7.RXD", true);

	simulator->SetVariable("XINT.debug-enabled", false);
	simulator->SetVariable("XINT.base-address", 0x0120);

	simulator->SetVariable("RAM.org", 0x000800);
	simulator->SetVariable("RAM.bytesize", 1024*1024); // 1MByte
	simulator->SetVariable("RAM.initial-byte-value", 0x00);
	simulator->SetVariable("RAM.cycle-time", 250000);
	simulator->SetVariable("RAM.verbose", false);

	simulator->SetVariable("FTM.org", 0x100000);
	simulator->SetVariable("FTM.bytesize", 7*1024*1024); // 7MByte
	simulator->SetVariable("FTM.initial-byte-value", 0xFF);
	simulator->SetVariable("FTM.cycle-time", 250000);
	simulator->SetVariable("FTM.bus-cycle-time", 250000);
	simulator->SetVariable("FTM.oscillator-cycle-time", 125000);
	simulator->SetVariable("FTM.base-address", 0x0100);
	simulator->SetVariable("FTM.erase-fail-ratio", 0.01);
	simulator->SetVariable("FTM.flash-Security-Byte-Address", 0x7FFF0F);
	simulator->SetVariable("FTM.blackdoor-comparison-key-address", 0x7FFF00);
	simulator->SetVariable("FTM.Protection-Byte-Address", 0x7FFF0C);
	simulator->SetVariable("FTM.EEE-Protection-Byte-Address", 0x7FFF0D);
	simulator->SetVariable("FTM.Flash-Option-Byte-Address", 0x7FFF0E);
	simulator->SetVariable("FTM.flash-fault-detect-interrupt", 0xBA);
	simulator->SetVariable("FTM.flash-interrupt", 0xB8);
	simulator->SetVariable("FTM.verbose", false);
	simulator->SetVariable("FTM.dflash-start-address", 0x100000);
	simulator->SetVariable("FTM.dflash-end-address", 0x107FFF);
	simulator->SetVariable("FTM.eee-nonvolatile-information-register-start-address", 0x120000);
	simulator->SetVariable("FTM.eee-nonvolatile-information-register-size", 128);
	simulator->SetVariable("FTM.eee_tag-ram-start-address", 0x122000);
	simulator->SetVariable("FTM.EEE-tag-RAM-size", 256);
	simulator->SetVariable("FTM.eee-protectable-high-address", 0x13FFFF);
	simulator->SetVariable("FTM.memory-controller-scratch-ram-start-address", 0x124000);
	simulator->SetVariable("FTM.memory-controller-scratch-ram-size", 1024);
	simulator->SetVariable("FTM.buffer-ram-start-address", 0x13F000);
	simulator->SetVariable("FTM.buffer-ram-end-address", 0x13FFFF);
	simulator->SetVariable("FTM.pflash-start-address", 0x700000);
	simulator->SetVariable("FTM.pflash-end-address", 0x7FFFFF);
	simulator->SetVariable("FTM.pflash-protectable-high-address", 0x7FFFFF);
	simulator->SetVariable("FTM.pflash-protectable-low-address", 0x7F8000);
	simulator->SetVariable("FTM.pflash-blocks-description", "7C0000,7FFFFF;7A0000,7BFFFF;780000,79FFFF;740000,77FFFF;700000,73FFFF");
	simulator->SetVariable("FTM.pflash-protection-byte-address", 0x7FFF0C);
	simulator->SetVariable("FTM.eee-protection-byte-address", 0x7FFF0D);
	simulator->SetVariable("FTM.flash-nonvolatile-byte-address", 0x7FFF0E);
	simulator->SetVariable("FTM.flash-security-byte-address", 0x7FFF0F);
	simulator->SetVariable("FTM.dflash-partition-user-access-address", 0x120000);
	simulator->SetVariable("FTM.dflash-partition-user-access-address-duplicate", 0x120002);
	simulator->SetVariable("FTM.buffer-ram-partition-eee-operation-address", 0x120004);
	simulator->SetVariable("FTM.buffer-ram-partition-eee-operation-address-duplicate", 0x120006);
	simulator->SetVariable("FTM.max-number-sectors-dflash", 128);
	simulator->SetVariable("FTM.max-number-sectors-buffer-ram", 16);
	simulator->SetVariable("FTM.min-number-sectors-in-dflash-for-eee", 12);
	simulator->SetVariable("FTM.min-ratio-dflash-buffer-ram", 8);
	simulator->SetVariable("FTM.min-fclk-time", 1250000); // 0.8 MHz
	simulator->SetVariable("FTM.max-fclk-time", 952000);   // 1.05 MHz

	simulator->SetVariable("kernel_logger.std_err", true);
	simulator->SetVariable("kernel_logger.std_out", false);
	simulator->SetVariable("kernel_logger.std_err_color", false);
	simulator->SetVariable("kernel_logger.std_out_color", false);
	simulator->SetVariable("kernel_logger.file", false);
	simulator->SetVariable("kernel_logger.filename", "logger_output.txt");
	simulator->SetVariable("kernel_logger.xml_file", true);
	simulator->SetVariable("kernel_logger.xml_filename", "logger_output.xml");
	simulator->SetVariable("kernel_logger.xml_file_gzipped", false);

	// Inline debugger
	simulator->SetVariable("inline-debugger.num-loaders", 1);
	simulator->SetVariable("inline-debugger.search-path", "");
	simulator->SetVariable("inline-debugger.program-counter-name", "PC");

	// Http Server
	simulator->SetVariable("http-server.http-port", 12360);
	
	// Web Terminals
	simulator->SetVariable("sci-web-terminal.title", "Serial Terminal over SCI");
	simulator->SetVariable("spi-web-terminal.title", "Serial Terminal over SPI");
}

void Simulator::SigInt()
{
	std::cerr << "Interrupted by Ctrl-C or SIGINT signal" << std::endl;
	unisim::kernel::Simulator::Instance()->Stop(0, 0, true);
}
