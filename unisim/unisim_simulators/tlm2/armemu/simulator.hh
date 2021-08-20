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

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/tlm2/simulator.hh>
#include <unisim/kernel/logger/console/console_printer.hh>
#include <unisim/kernel/logger/text_file/text_file_writer.hh>
#include <unisim/kernel/logger/http/http_writer.hh>
#include <unisim/kernel/logger/xml_file/xml_file_writer.hh>
#include <unisim/kernel/logger/netstream/netstream_writer.hh>
#include <unisim/component/tlm2/processor/arm/cortex_a9/cpu.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/os/linux_os/arm_linux32.hh>
#include <unisim/service/trap_handler/trap_handler.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/monitor/monitor.hh>
#include <unisim/service/debug/profiler/profiler.hh>
#include <unisim/service/http_server/http_server.hh>
#include <unisim/service/instrumenter/instrumenter.hh>

#include <iostream>
#include <sstream>
#include <list>
#include <string>
#include <getopt.h>
#include <cstdlib>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WIN32

#include <winsock2.h>
#include <windows.h>

#else
#include <signal.h>
#endif

class Simulator
  : public unisim::kernel::tlm2::Simulator
{
 public:
  Simulator(int argc, char **argv, const sc_core::sc_module_name& name = "HARDWARE");
  virtual ~Simulator();
  int Run();
  virtual unisim::kernel::Simulator::SetupStatus Setup();
  virtual bool EndSetup();
  static void EnableMonitor(int (*monitor_callback)(void));

 protected:
 private:
  static void DefaultConfiguration(unisim::kernel::Simulator *sim);
  typedef unisim::component::tlm2::processor::arm::cortex_a9::CPU CPU;
  typedef unisim::component::tlm2::memory::ram::Memory<32, uint32_t, 8, 1024 * 1024, true> MEMORY;
  typedef unisim::service::os::linux_os::ArmLinux32 ArmLinux32;

  typedef unisim::service::debug::gdb_server::GDBServer<uint32_t> GDB_SERVER;
  typedef unisim::service::debug::inline_debugger::InlineDebugger<uint32_t> INLINE_DEBUGGER;
  typedef unisim::service::debug::profiler::Profiler<uint32_t> PROFILER;
  typedef unisim::service::http_server::HttpServer HTTP_SERVER;
  typedef unisim::service::instrumenter::Instrumenter INSTRUMENTER;
  typedef unisim::kernel::logger::console::Printer LOGGER_CONSOLE_PRINTER;
  typedef unisim::kernel::logger::text_file::Writer LOGGER_TEXT_FILE_WRITER;
  typedef unisim::kernel::logger::http::Writer LOGGER_HTTP_WRITER;
  typedef unisim::kernel::logger::xml_file::Writer LOGGER_XML_FILE_WRITER;
  typedef unisim::kernel::logger::netstream::Writer LOGGER_NETSTREAM_WRITER;

  struct DEBUGGER_CONFIG
  {
    typedef uint32_t ADDRESS;
    static const unsigned int NUM_PROCESSORS = 1;
    /* gdb_server, inline_debugger and/or monitor */
    static const unsigned int MAX_FRONT_ENDS = 4;
  };
  
  typedef unisim::service::debug::debugger::Debugger<DEBUGGER_CONFIG> DEBUGGER;
  typedef unisim::service::debug::monitor::Monitor<uint32_t>          MONITOR;
  typedef unisim::service::time::sc_time::ScTime                      ScTime;
  typedef unisim::service::time::host_time::HostTime                  HostTime;
  
  CPU                   cpu;
  MEMORY                memory;
  ScTime                time;
  HostTime              host_time;
  ArmLinux32            linux_os;

  double                simulation_spent_time;

  DEBUGGER*                debugger;
  GDB_SERVER*              gdb_server;
  INLINE_DEBUGGER*         inline_debugger;
  MONITOR*                 monitor;
  PROFILER*                profiler;
  HTTP_SERVER*             http_server;
  INSTRUMENTER*            instrumenter;
  LOGGER_CONSOLE_PRINTER*  logger_console_printer;
  LOGGER_TEXT_FILE_WRITER* logger_text_file_writer;
  LOGGER_HTTP_WRITER*      logger_http_writer;
  LOGGER_XML_FILE_WRITER*  logger_xml_file_writer;
  LOGGER_NETSTREAM_WRITER* logger_netstream_writer;
  
  bool                                     enable_gdb_server;
  unisim::kernel::variable::Parameter<bool> param_enable_gdb_server;
  bool                                     enable_inline_debugger;
  unisim::kernel::variable::Parameter<bool> param_enable_inline_debugger;
  bool                                     enable_profiler;
  unisim::kernel::variable::Parameter<bool> param_enable_profiler;
  
  virtual void SigInt();
  static bool enable_monitor;
};

#endif /* SIMULATOR_HH_ */
