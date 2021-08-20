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
 * Authors: Julien Lisita (julien.lisita@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __AVR32EMU_SIMULATOR_HH__
#define __AVR32EMU_SIMULATOR_HH__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// Class definition of components
#include <unisim/component/tlm2/processor/avr32/avr32uc/cpu.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.hh>

// Simulator compile time configuration
#include <config.hh>

// Class definition of kernel, services and interfaces
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/console/console_printer.hh>
#include <unisim/kernel/logger/text_file/text_file_writer.hh>
#include <unisim/kernel/logger/http/http_writer.hh>
#include <unisim/kernel/logger/xml_file/xml_file_writer.hh>
#include <unisim/kernel/logger/netstream/netstream_writer.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/debugger/debugger.tcc>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/loader/multiformat_loader/multiformat_loader.hh>
#include <unisim/service/os/avr32_t2h_syscalls/avr32_t2h_syscalls.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/instrumenter/instrumenter.hh>
#include <unisim/service/debug/profiler/profiler.hh>
#include <unisim/service/http_server/http_server.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/tlm2/simulator.hh>

using unisim::util::endian::E_BIG_ENDIAN;

//=========================================================================
//===                        Top level class                            ===
//=========================================================================

template <class CONFIG>
class Simulator : public unisim::kernel::tlm2::Simulator
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
	//===                       Constants definitions                       ===
	//=========================================================================

	typedef typename CONFIG::CPU_CONFIG::address_t CPU_ADDRESS_TYPE;
	typedef typename CONFIG::CPU_CONFIG::physical_address_t HSB_ADDRESS_TYPE;
	
	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm2::memory::ram::Memory<CONFIG::CPU_CONFIG::HSB_WIDTH * 8, HSB_ADDRESS_TYPE, CONFIG::CPU_CONFIG::HSB_BURST_SIZE / CONFIG::CPU_CONFIG::HSB_WIDTH, unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE, CONFIG::DEBUG_INFORMATION> RAM;
	typedef unisim::component::tlm2::processor::avr32::avr32uc::CPU<typename CONFIG::CPU_CONFIG> CPU;
	typedef unisim::component::tlm2::interconnect::generic_router::Router<typename CONFIG::MEMORY_ROUTER_CONFIG> MEMORY_ROUTER;
	typedef unisim::kernel::tlm2::InitiatorStub<0, unisim::component::tlm2::interrupt::InterruptProtocolTypes> NMIREQ_STUB;
	typedef unisim::kernel::tlm2::InitiatorStub<0, unisim::component::tlm2::interrupt::InterruptProtocolTypes> IRQ_STUB;
	
	//=========================================================================
	//===                      Aliases for services classes                 ===
	//=========================================================================

	typedef unisim::service::debug::debugger::Debugger<typename CONFIG::DEBUGGER_CONFIG> DEBUGGER;
	typedef unisim::service::debug::inline_debugger::InlineDebugger<CPU_ADDRESS_TYPE> INLINE_DEBUGGER;
	typedef unisim::service::debug::gdb_server::GDBServer<CPU_ADDRESS_TYPE> GDB_SERVER;
	typedef unisim::service::loader::multiformat_loader::MultiFormatLoader<CPU_ADDRESS_TYPE> LOADER;
	typedef unisim::service::os::avr32_t2h_syscalls::AVR32_T2H_Syscalls<CPU_ADDRESS_TYPE> AVR32_T2H_SYSCALLS;
	typedef unisim::service::instrumenter::Instrumenter INSTRUMENTER;
	typedef unisim::service::debug::profiler::Profiler<CPU_ADDRESS_TYPE> PROFILER;
	typedef unisim::service::http_server::HttpServer HTTP_SERVER;
	typedef unisim::kernel::logger::console::Printer LOGGER_CONSOLE_PRINTER;
	typedef unisim::kernel::logger::text_file::Writer LOGGER_TEXT_FILE_WRITER;
	typedef unisim::kernel::logger::http::Writer LOGGER_HTTP_WRITER;
	typedef unisim::kernel::logger::xml_file::Writer LOGGER_XML_FILE_WRITER;
	typedef unisim::kernel::logger::netstream::Writer LOGGER_NETSTREAM_WRITER;
	
	//=========================================================================
	//===                           Components                              ===
	//=========================================================================
	//  - AVR32UC processor
	CPU *cpu;
	//  - RAM
	RAM *ram;
	//  - Memory Router
	MEMORY_ROUTER *memory_router;
	//  - NMIREQ Stub
	NMIREQ_STUB *nmireq_stub;
	//  - IRQ Stub
	IRQ_STUB *irq_stub[CONFIG::CPU_CONFIG::NUM_IRQS];
	
	//=========================================================================
	//===                            Services                               ===
	//=========================================================================
	//  - Multiformat loader
	LOADER *loader;
	//  - AVR32 Target to Host syscalls
	AVR32_T2H_SYSCALLS *avr32_t2h_syscalls;
	//  - Debugger
	DEBUGGER *debugger;
	//  - GDB server
	GDB_SERVER *gdb_server;
	//  - Inline debugger
	INLINE_DEBUGGER *inline_debugger;
	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *sim_time;
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time;
	//  - Instrumenter
	INSTRUMENTER *instrumenter;
	//  - Profiler
	PROFILER *profiler;
	//  - HTTP server
	HTTP_SERVER *http_server;
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
	unisim::kernel::variable::Parameter<bool> param_enable_gdb_server;
	unisim::kernel::variable::Parameter<bool> param_enable_inline_debugger;
	unisim::kernel::variable::Parameter<bool> param_enable_profiler;

	static void LoadBuiltInConfig(unisim::kernel::Simulator *simulator);
	virtual void SigInt();
};

#endif // __AVR32EMU_SIMULATOR_HH__
