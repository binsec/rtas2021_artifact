/*
 *  Copyright (c) 2010,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef SIMULATOR_HH_
#define SIMULATOR_HH_

#include <GenericTimer.hh>
#include <unisim/component/tlm2/processor/arm/cortex_a9/cpu.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.hh>
#include <unisim/component/tlm2/interconnect/generic_router/config.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/loader/multiformat_loader/multiformat_loader.hh>
#include <unisim/service/trap_handler/trap_handler.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/profiling/addr_profiler/profiler.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/util/likely/likely.hh>
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <cstdlib>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

struct RouterCFG : unisim::component::tlm2::interconnect::generic_router::Config
{
  typedef uint32_t ADDRESS;
  static unsigned const INPUT_SOCKETS = 1;
  static unsigned const OUTPUT_SOCKETS = 2;
  static unsigned const NUM_MAPPINGS = 2;
  static unsigned const BUSWIDTH = 32;
  typedef tlm::tlm_base_protocol_types TYPES;
  static const bool VERBOSE = false;
};

struct Router : public unisim::component::tlm2::interconnect::generic_router::Router<RouterCFG>
{
  Router( char const* name, unisim::kernel::Object* parent = 0 );
};

struct Simulator : public unisim::kernel::Simulator
{
  Simulator( int argc, char **argv );
  virtual ~Simulator();
  
  int Run();
  int Run(double time, sc_time_unit unit);
  bool IsRunning() const;
  bool SimulationStarted() const;
  bool SimulationFinished() const;
  virtual unisim::kernel::Simulator::SetupStatus Setup();
  virtual void Stop(unisim::kernel::Object *object, int exit_status, bool asynchronous = false);
  int GetExitStatus() const;
  
 private:
  static void DefaultConfiguration(unisim::kernel::Simulator *sim);
  typedef unisim::component::tlm2::processor::arm::cortex_a9::CPU CPU;
  typedef unisim::component::tlm2::memory::ram::Memory<32, uint32_t, 8, 1024 * 1024, true> MEMORY;
  //typedef unisim::service::os::linux_os::Linux<uint32_t, uint32_t> LINUX_OS;
  typedef unisim::service::loader::multiformat_loader::MultiFormatLoader<uint32_t> LOADER;
  
  struct DEBUGGER_CONFIG
  {
    typedef uint32_t ADDRESS;
    static const unsigned int NUM_PROCESSORS = 1;
    /* gdb_server, inline_debugger and/or monitor */
    static const unsigned int MAX_FRONT_ENDS = 3;
  };
  
  typedef unisim::service::debug::debugger::Debugger<DEBUGGER_CONFIG> DEBUGGER;
  typedef unisim::service::debug::gdb_server::GDBServer<uint32_t> GDB_SERVER;
  typedef unisim::service::debug::inline_debugger::InlineDebugger<uint32_t> INLINE_DEBUGGER;
  
  typedef unisim::service::profiling::addr_profiler::Profiler<uint32_t> PROFILER;
  typedef unisim::service::time::sc_time::ScTime ScTime;
  typedef unisim::service::time::host_time::HostTime HostTime;
  
  scml_clock                   clock;
  CPU                          cpu;
  Router                       router;
  MEMORY                       memory;
  GenericTimer<>               timer;
  sc_signal<bool>              timer_reset;
  sc_signal<bool>              timer_enable;
  sc_signal<bool>              nirq_signal;
  sc_signal<bool>              nfiq_signal;
  sc_signal<bool>              nrst_signal;
  
  ScTime                       time;
  HostTime                     host_time;
  //LINUX_OS*                    linux_os;
  LOADER                       loader;

  double                       simulation_spent_time;

  DEBUGGER*                    debugger;
  GDB_SERVER*                  gdb_server;
  INLINE_DEBUGGER*             inline_debugger;
  
  bool                                     enable_gdb_server;
  unisim::kernel::variable::Parameter<bool> param_enable_gdb_server;
  bool                                     enable_inline_debugger;
  unisim::kernel::variable::Parameter<bool> param_enable_inline_debugger;

  int exit_status;
  virtual void SigInt();
};

#endif /* SIMULATOR_HH_ */
