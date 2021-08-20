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

#ifndef __SIMULATOR_HH__
#define __SIMULATOR_HH__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/console/console_printer.hh>
#include <unisim/kernel/logger/text_file/text_file_writer.hh>
#include <unisim/kernel/logger/xml_file/xml_file_writer.hh>

#include <unisim/util/debug/breakpoint_registry.hh>
#include <unisim/util/debug/watchpoint_registry.hh>
#include <unisim/util/blob/blob.hh>
#include <unisim/util/loader/elf_loader/elf_loader.hh>
#include <unisim/service/debug/debugger/debugger.hh>

#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/loader/raw_loader/raw_loader.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>

#include <unisim/component/tlm2/processor/risc16/cpu.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>

#include <tlm>

#include <stdint.h>
#include <iostream>
#include <stdexcept>

using unisim::kernel::Object;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Statistic;
using unisim::kernel::VariableBase;

using unisim::service::debug::debugger::Debugger;
using unisim::service::debug::inline_debugger::InlineDebugger;
//using unisim::service::profiling::addr_profiler::Profiler;

class Simulator
	: public unisim::kernel::Simulator

{
private:
	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================

public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	virtual void Stop(Object *object, int _exit_status, bool asynchronous);

	void Run();

private:

	//=========================================================================
	//===               Aliases for components/Service classes              ===
	//=========================================================================

	typedef unisim::component::tlm2::memory::ram::Memory<32, uint64_t, 1, 1024 * 1024, false> RAM;

	typedef unisim::component::tlm2::processor::risc16::CPU CPU;

	typedef unisim::service::loader::raw_loader::RawLoader<uint64_t> RawLoader;

	//=========================================================================
	//===                           Components                              ===
	//=========================================================================
	
	// - RISC16 processor
	CPU *cpu;

	// - Memory
	RAM *memory;

	//=========================================================================
	//===                            Services                               ===
	//=========================================================================
	// - Debugger
	
	struct DEBUGGER_CONFIG
	{
		typedef uint64_t ADDRESS;
		static const unsigned int NUM_PROCESSORS = 1;
		/* gdb_server, inline_debugger and/or monitor */
		static const unsigned int MAX_FRONT_ENDS = 1;
	};
	
	Debugger<DEBUGGER_CONFIG> *debugger;
	
	// - Inline-debugger
	InlineDebugger<uint64_t> *inline_debugger;

	// - Raw loader
	RawLoader* raw_loader;

	//  - Logger Console Printer
	unisim::kernel::logger::console::Printer *logger_console_printer;
	//  - Logger Text File Writer
	unisim::kernel::logger::text_file::Writer *logger_text_file_writer;
	//  - Logger XML File Writer
	unisim::kernel::logger::xml_file::Writer *logger_xml_file_writer;
	
	//==========================================================================
	
	int exit_status;

	static void LoadBuiltInConfig(unisim::kernel::Simulator *simulator);
};

#endif /* __SIMULATOR_HH__ */
