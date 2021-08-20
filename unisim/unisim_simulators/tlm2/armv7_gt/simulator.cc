/*
 *  Copyright (c) 2010, Commissariat a l'Energie Atomique (CEA) All rights
 *  reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *   endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <unisim/component/tlm2/interconnect/generic_router/router.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.tcc>
#include <unisim/service/debug/debugger/debugger.tcc>
#include <simulator.hh>
#include <stdexcept>
#include <iostream>
#include <inttypes.h>

bool debug_enabled;

Router::Router(const char* name, unisim::kernel::Object* parent)
  : unisim::kernel::Object( name, parent )
  , unisim::component::tlm2::interconnect::generic_router::Router<RouterCFG>( name, parent )
{
  /* Low global memory range */
  this->mapping[0].used = true;
  this->mapping[0].range_start = 0x00000000;
  this->mapping[0].range_end =   0x00000fff;
  this->mapping[0].output_port = 0;
  this->mapping[0].translation = 0;
  /* Timer range */
  this->mapping[1].used = true;
  this->mapping[1].range_start = 0x00001000;
  this->mapping[1].range_end =   0x00001fff;
  this->mapping[1].output_port = 1;
  this->mapping[1].translation = 0;
  // /* High global memory range */
  // this->mapping[2].used = true;
  // this->mapping[2].range_start = 0x00002000;
  // this->mapping[2].range_end =   0xffffffff;
  // this->mapping[2].output_port = 0;
  // this->mapping[2].translation = 0x00002000;
}

Simulator::Simulator(int argc, char **argv)
  : unisim::kernel::Simulator(argc, argv, Simulator::DefaultConfiguration)
  , clock("CLK", sc_core::sc_time(10.0, SC_NS))
  , cpu( "cpu" )
  , router( "router" )
  , memory( "memory" )
  , timer( "timer" )
  , timer_reset("RESET")
  , timer_enable("ENABLE")
  , nirq_signal("nIRQm")
  , nfiq_signal("nFIQm")
  , nrst_signal("nRESETm")
  , time("time")
  , host_time("host-time")
  , loader("loader")
  , simulation_spent_time(0.0)
  , debugger(0)
  , gdb_server(0)
  , inline_debugger(0)
  , enable_gdb_server(false)
  , param_enable_gdb_server( "enable-gdb-server", 0, enable_gdb_server, "Enable GDB server." )
  , enable_inline_debugger(false)
  , param_enable_inline_debugger( "enable-inline-debugger", 0, enable_inline_debugger, "Enable inline debugger." )
  , exit_status(0)
{
  // - debugger
  if (enable_inline_debugger or enable_gdb_server)
    debugger = new DEBUGGER( "debugger" );
  if (enable_gdb_server)
    gdb_server = new GDB_SERVER("gdb-server");
  if (enable_inline_debugger)
    inline_debugger = new INLINE_DEBUGGER( "inline-debugger" );

  nfiq_signal = true; 
  nirq_signal = true; 
  nrst_signal = true;
  timer_enable = true;
  timer_reset = false;
  
  cpu.master_socket( *router.targ_socket[0] );
  cpu.nIRQm( nirq_signal );
  cpu.nFIQm( nfiq_signal );
  cpu.nRESETm( nrst_signal );

  // (*router.init_socket[0])( memory.slave_sock );
  (*router.init_socket[0])( memory.slave_sock );
  (*router.init_socket[1])( timer.CTRL );
  timer.IRQ( nirq_signal );
  timer.RST( timer_reset );
  timer.ENABLE( timer_enable );
  timer.CLK(clock);
  /* We disable clock (useless in this model and extremely cpu consuming) */
  clock.disable();
  
  cpu.symbol_table_lookup_import >> loader.symbol_table_lookup_export;
  *loader.memory_import[0] >> memory.memory_export;

  if (enable_inline_debugger or enable_gdb_server)
    {
      // Debugger <-> CPU+peripherals  connections
      cpu.debug_yielding_import                            >> *debugger->debug_yielding_export[0];
      cpu.memory_access_reporting_import                   >> *debugger->memory_access_reporting_export[0];
      cpu.trap_reporting_import                            >> *debugger->trap_reporting_export[0];
      *debugger->disasm_import[0]                          >> cpu.disasm_export;
      *debugger->memory_import[0]                          >> cpu.memory_export;
      *debugger->registers_import[0]                       >> cpu.registers_export;
      *debugger->memory_access_reporting_control_import[0] >> cpu.memory_access_reporting_control_export;
      
      // debug
      debugger->blob_import >> loader.blob_export;
    }

  if (enable_inline_debugger)
    {
      // Connect inline-debugger to debugger
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
  
  if (enable_gdb_server)
    {
      // Connect gdb-server to debugger
      *debugger->debug_yielding_import[0]       >> gdb_server->debug_yielding_export;
      *debugger->debug_event_listener_import[0] >> gdb_server->debug_event_listener_export;
      gdb_server->debug_yielding_request_import >> *debugger->debug_yielding_request_export[0];
      gdb_server->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[0];
      gdb_server->memory_import                 >> *debugger->memory_export[0];
      gdb_server->registers_import              >> *debugger->registers_export[0];
    }
}

Simulator::~Simulator()
{
  delete debugger;
  delete gdb_server;
  delete inline_debugger;
}

int
Simulator::Run()
{
  if ( unlikely(SimulationFinished()) ) return 0;

  double time_start = host_time.GetTime();

  sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING); // disable SystemC messages
  
  try
    {
      sc_start();
    }
  catch(std::runtime_error& e)
    {
      cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << endl;
      cerr << e.what() << endl;
    }

  cerr << "Simulation finished" << endl;

  double time_stop = host_time.GetTime();
  double spent_time = time_stop - time_start;
  simulation_spent_time += spent_time;

  cerr << "Simulation run-time parameters:" << endl;
  DumpParameters(cerr);
  cerr << endl;
  cerr << "Simulation formulas:" << endl;
  DumpFormulas(cerr);
  cerr << endl;
  cerr << "Simulation statistics:" << endl;
  DumpStatistics(cerr);
  cerr << endl;

  cerr << "simulation time: " << simulation_spent_time << " seconds" << endl;
  cerr << "simulated time : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << endl;
  cerr << "host simulation speed: " << ((double) cpu["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << endl;
  cerr << "time dilatation: " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << endl;

  return exit_status;
}

int
Simulator::Run(double time, sc_time_unit unit)
{
  if ( unlikely(SimulationFinished()) ) return 0;

  double time_start = host_time.GetTime();

  sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING); // disable SystemC messages

  try
    {
      sc_start(time, unit);
    }
  catch(std::runtime_error& e)
    {
      cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << endl;
      cerr << e.what() << endl;
    }

  double time_stop = host_time.GetTime();
  double spent_time = time_stop - time_start;
  simulation_spent_time += spent_time;

  cerr << "Simulation statistics:" << endl;
  DumpStatistics(cerr);
  cerr << endl;

  return exit_status;
}

bool
Simulator::IsRunning() const
{
  return sc_is_running();
}

bool
Simulator::SimulationStarted() const
{
  return sc_start_of_simulation_invoked();
}

bool
Simulator::SimulationFinished() const
{
  return sc_end_of_simulation_invoked();
}

unisim::kernel::Simulator::SetupStatus Simulator::Setup()
{
  if(enable_inline_debugger)
    {
      SetVariable("debugger.parse-dwarf", true);
    }
  
  // Build the Loader arguments from the command line arguments
  
  unisim::kernel::VariableBase *cmd_args = FindVariable("cmd-args");
  unsigned int cmd_args_length = cmd_args->GetLength();
  if(cmd_args_length > 0)
    {
      SetVariable( "loader.filename", ((std::string)(*cmd_args)[0]).c_str() );
    }

  unisim::kernel::Simulator::SetupStatus setup_status = unisim::kernel::Simulator::Setup();
  
  return setup_status;
}

void Simulator::Stop(unisim::kernel::Object *object, int _exit_status, bool asynchronous)
{
  exit_status = _exit_status;
  if(object)
    {
      std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
    }
#ifdef DEBUG_ARMEMU
  std::cerr << "Call stack:" << std::endl;
  std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
#endif
  std::cerr << "Program exited with status " << exit_status << std::endl;
  sc_stop();
  if(!asynchronous)
    {
      switch(sc_get_curr_simcontext()->get_curr_proc_info()->kind)
        {
        case SC_THREAD_PROC_: 
        case SC_CTHREAD_PROC_:
          wait();
          break;
        default:
          break;
        }
    }
}

void
Simulator::DefaultConfiguration(unisim::kernel::Simulator *sim)
{
  sim->SetVariable( "program-name", SIM_PROGRAM_NAME );
  sim->SetVariable( "authors", SIM_AUTHOR );
  sim->SetVariable( "version", SIM_VERSION );
  sim->SetVariable( "copyright", SIM_COPYRIGHT );
  sim->SetVariable( "license", SIM_LICENSE );
  sim->SetVariable( "description", SIM_DESCRIPTION );
  sim->SetVariable( "schematic", SIM_SCHEMATIC );


  sim->SetVariable( "kernel_logger.std_err", true );
  sim->SetVariable( "kernel_logger.std_err_color", true );
  
  sim->SetVariable( "router.cycle_time",        "10 ns" );
  
  sim->SetVariable( "cpu.default-endianness",   "little-endian" );
  sim->SetVariable( "cpu.cpu-cycle-time",       "10 ns" ); // 32Mhz
  sim->SetVariable( "cpu.bus-cycle-time",       "10 ns" ); // 32Mhz
  sim->SetVariable( "cpu.icache.size",          0x020000 ); // 128 KB
  sim->SetVariable( "cpu.dcache.size",          0x020000 ); // 128 KB
  sim->SetVariable( "cpu.nice-time",            "1 us" ); // 1us
  sim->SetVariable( "cpu.ipc",                  1.0  );
  sim->SetVariable( "cpu.voltage",              1.8 * 1e3 ); // 1800 mV
  sim->SetVariable( "cpu.enable-dmi",           true ); // Enable SystemC TLM 2.0 DMI
  sim->SetVariable( "cpu.verbose",              true );
  sim->SetVariable( "cpu.verbose-tlm",          false );
  sim->SetVariable( "memory.bytesize",          0xffffffffUL ); 
  sim->SetVariable( "memory.cycle-time",        "10 ns" );
  sim->SetVariable( "memory.read-latency",      "10 ns" );
  sim->SetVariable( "memory.write-latency",     "0 ps" );
  // sim->SetVariable( "linux-os.system",          "arm-eabi" );
  // sim->SetVariable( "linux-os.endianness",      "little-endian" );
  // sim->SetVariable( "linux-os.memory-page-size",0x01000UL );
  // sim->SetVariable( "linux-os.stack-base",      0x40000000UL );
  // sim->SetVariable( "linux-os.envc",            0 );
  // sim->SetVariable( "linux-os.utsname-sysname", "Linux" );
  // sim->SetVariable( "linux-os.utsname-nodename","localhost" );
  // sim->SetVariable( "linux-os.utsname-release", "2.6.27.35" );
  // sim->SetVariable( "linux-os.utsname-version", "#UNISIM SMP Fri Mar 12 05:23:09 UTC 2010" );
  // sim->SetVariable( "linux-os.utsname-machine", "armv7" );
  // sim->SetVariable( "linux-os.utsname-domainname","localhost" );
  // sim->SetVariable( "linux-os.apply-host-environment", false );
  // sim->SetVariable( "linux-os.hwcap", "swp half fastmult" );
	sim->SetVariable("loader.filename" ,         "boot.elf");
	sim->SetVariable("loader.verbose" ,          true);
	sim->SetVariable("loader.memory-mapper.mapping", "memory=memory:0x0-0x00000fff");
	// sim->SetVariable("loader.file0.base-addr",    0x0);
	// sim->SetVariable("loader.file1.force-base-addr", true);
	// sim->SetVariable("loader.file1.base-addr",    0x10000);
	// sim->SetVariable("loader.file1.force-use-virtual-address",
	//                                               true);
	// sim->SetVariable("loader.file2.base-addr",    0x110000);
	// sim->SetVariable("loader.file3.base-addr",    0x1110000);
  
  
  sim->SetVariable( "gdb-server.architecture-description-filename", "unisim/service/debug/gdb_server/gdb_arm_with_neon.xml" );
  sim->SetVariable( "debugger.parse-dwarf", false );
  sim->SetVariable( "debugger.dwarf-register-number-mapping-filename", "unisim/util/debug/dwarf/arm_eabi_dwarf_register_number_mapping.xml" );

  sim->SetVariable( "inline-debugger.num-loaders", 1 );
  sim->SetVariable( "inline-debugger.search-path", "" );
}

void Simulator::SigInt()
{
	if(!inline_debugger || !inline_debugger->IsStarted())
	{
		unisim::kernel::Simulator::Instance()->Stop(0, 0, true);
	}
}
