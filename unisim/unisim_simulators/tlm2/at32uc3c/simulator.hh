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

#ifndef __AT32UC3C_SIMULATOR_HH__
#define __AT32UC3C_SIMULATOR_HH__

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
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/profiling/addr_profiler/profiler.hh>
#include <unisim/service/loader/multiformat_loader/multiformat_loader.hh>
#include <unisim/service/os/avr32_t2h_syscalls/avr32_t2h_syscalls.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>

// Host machine standard headers
#include <iostream>
#include <stdexcept>
#include <stdlib.h>

#ifdef WIN32

#include <windows.h>
#include <winsock2.h>

#else
#include <signal.h>
#endif

using namespace std;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::service::loader::multiformat_loader::MultiFormatLoader;
using unisim::service::os::avr32_t2h_syscalls::AVR32_T2H_Syscalls;
using unisim::service::debug::debugger::Debugger;
using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;
using unisim::service::profiling::addr_profiler::Profiler;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Variable;
using unisim::kernel::VariableBase;
using unisim::kernel::Object;

//=========================================================================
//===                        Top level class                            ===
//=========================================================================

template <class CONFIG>
class Simulator : public unisim::kernel::Simulator
{
public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	void Run();
	virtual unisim::kernel::Simulator::SetupStatus Setup();
	virtual void Stop(Object *object, int exit_status, bool asynchronous = false);
	int GetExitStatus() const;
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
	MultiFormatLoader<CPU_ADDRESS_TYPE> *loader;
	//  - AVR32 Target to Host syscalls
	AVR32_T2H_Syscalls<CPU_ADDRESS_TYPE> *avr32_t2h_syscalls;
	
	struct DEBUGGER_CONFIG
	{
		typedef CPU_ADDRESS_TYPE ADDRESS;
		static const unsigned int NUM_PROCESSORS = 1;
		/* gdb_server, inline_debugger and/or monitor */
		static const unsigned int MAX_FRONT_ENDS = 3;
	};
	
	//  - Debugger
	Debugger<DEBUGGER_CONFIG> *debugger;
	//  - GDB server
	GDBServer<CPU_ADDRESS_TYPE> *gdb_server;
	//  - Inline debugger
	InlineDebugger<CPU_ADDRESS_TYPE> *inline_debugger;
	//  - profiler
	Profiler<CPU_ADDRESS_TYPE> *profiler;
	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *sim_time;
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time;

	bool enable_avr32_t2h_syscalls;
	bool enable_gdb_server;
	bool enable_inline_debugger;
	Parameter<bool> param_enable_avr32_t2h_syscalls;
	Parameter<bool> param_enable_gdb_server;
	Parameter<bool> param_enable_inline_debugger;

	int exit_status;
	static void LoadBuiltInConfig(unisim::kernel::Simulator *simulator);
#ifdef WIN32
	static BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType);
#else
	static void SigIntHandler(int signum);
#endif
};

#endif // __AT32UC3C_SIMULATOR_HH__
