/*
 *  Copyright (c) 2007-2015,
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
 * Authors: Réda Nouacer (reda.nouacer@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <simulator.hh>
#include <unisim/service/debug/debugger/debugger.tcc>
#include <unisim/kernel/config/xml/xml_config_file_helper.hh>
#include <unisim/kernel/config/ini/ini_config_file_helper.hh>
#include <unisim/kernel/config/json/json_config_file_helper.hh>

Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, memory(0)
	, debugger(0)
	, inline_debugger(0)
	, raw_loader(0)
	, logger_console_printer(0)
	, logger_text_file_writer(0)
	, logger_xml_file_writer(0)
	, exit_status(0)
{
	//=========================================================================
	//===                 Logger Printers instantiations                    ===
	//=========================================================================

	logger_console_printer = new unisim::kernel::logger::console::Printer();
	logger_text_file_writer = new unisim::kernel::logger::text_file::Writer();
	logger_xml_file_writer = new unisim::kernel::logger::xml_file::Writer();
	
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	// - RISC16 processor
	cpu = new CPU("cpu");

	// - Memory
	memory = new RAM("memory");

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================

	// - Debugger
	debugger = new Debugger<DEBUGGER_CONFIG>("debugger");
	
	// - Built-in console debugger (Inline-debugger)
	inline_debugger = new InlineDebugger<uint64_t>("inline-debugger");
	
	// - Raw loader
	raw_loader = new RawLoader("raw-loader");
	
	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	cpu->master_sock(memory->slave_sock);

	//=========================================================================
	//===                       Clients/Services connections                ===
	//=========================================================================

	// CPU imports
	cpu->memory_import >> memory->memory_export;                    // memory
	
	// raw-loader imports
	raw_loader->memory_import >> memory->memory_export;    // memory
	
	// Debugger <-> CPU connections
	cpu->debug_yielding_import                           >> *debugger->debug_yielding_export[0];
	cpu->trap_reporting_import                           >> *debugger->trap_reporting_export[0];
	cpu->memory_access_reporting_import                  >> *debugger->memory_access_reporting_export[0];
	*debugger->disasm_import[0]                          >> cpu->disasm_export;
	*debugger->memory_import[0]                          >> cpu->memory_export;
	*debugger->registers_import[0]                       >> cpu->registers_export;
	*debugger->memory_access_reporting_control_import[0] >> cpu->memory_access_reporting_control_export;
	
	// Debugger <-> Loader connections
	debugger->blob_import >> raw_loader->blob_export;

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

Simulator::~Simulator()
{
	delete cpu;
	delete memory;
	delete raw_loader;
	delete inline_debugger;
	delete debugger;
	delete logger_console_printer;
	delete logger_text_file_writer;
	delete logger_xml_file_writer;
}

void Simulator::LoadBuiltInConfig(unisim::kernel::Simulator *simulator)
{
	new unisim::kernel::config::xml::XMLConfigFileHelper(simulator);
	new unisim::kernel::config::ini::INIConfigFileHelper(simulator);
	new unisim::kernel::config::json::JSONConfigFileHelper(simulator);
	
	// meta information
	simulator->SetVariable("program-name", "UNISIM risc16");
	simulator->SetVariable("copyright", "Copyright (C) 2008-2015, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Réda Nouacer <reda.nouacer@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>");

#ifdef HAVE_CONFIG_H
	simulator->SetVariable("version", VERSION);
#endif

	simulator->SetVariable("description", "UNISIM risc16, a simple 16-bit RISC simulator for teaching");

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	simulator->SetVariable("memory.org", 0x0);
	simulator->SetVariable("memory.bytesize", 65536);
	simulator->SetVariable("memory.cycle-time", 10000); // 10 000 ps
	simulator->SetVariable("memory.verbose", false);
	
	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	simulator->SetVariable("inline-debugger.memory-atom-size", 2); // memory is not byte addressable
	simulator->SetVariable("inline-debugger.program-counter-name", "cia");
	simulator->SetVariable("kernel_logger.std_err", true);
	simulator->SetVariable("kernel_logger.std_out", false);
	simulator->SetVariable("kernel_logger.std_err_color", false);
	simulator->SetVariable("kernel_logger.std_out_color", false);
	simulator->SetVariable("kernel_logger.file", false);
	simulator->SetVariable("kernel_logger.filename", "logger_output.txt");
	simulator->SetVariable("kernel_logger.xml_file", false);
	simulator->SetVariable("kernel_logger.xml_filename", "logger_output.xml");
	simulator->SetVariable("kernel_logger.xml_file_gzipped", false);
}

void Simulator::Stop(Object *object, int _exit_status, bool asynchronous)
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

void Simulator::Run() {

	std::cerr << "Starting simulation ..." << std::endl;

	try
	{
		sc_core::sc_start();
	}
	catch(std::runtime_error& e)
	{
		std::cerr << "FATAL ERROR! an abnormal error occurred during simulation. Bailing out..." << std::endl;
		std::cerr << e.what() << std::endl;
	}

	std::cerr << "Simulation finished" << std::endl << std::endl;
}

