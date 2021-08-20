/*
 *  Copyright (c) 2018,
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

#include <arch.hh>
#include <linuxsystem.hh>
#include <debugger.hh>
#include <unisim/kernel/logger/console/console_printer.hh>
#include <iostream>
#include <iomanip>
#include <stdexcept>

struct Simulator
  : public unisim::kernel::Simulator
{
  typedef unisim::kernel::Simulator BaseSimulator;
  
  Simulator(char* self)
    : BaseSimulator(1, &self, Simulator::DefaultConfiguration)
    , exit_status(-1)
  {}

  //  virtual ~Simulator();
  //  int Run();
  //  int Run(double time, sc_core::sc_time_unit unit);
  //  bool IsRunning() const;
  //  bool SimulationStarted() const;
  //  bool SimulationFinished() const;
  virtual BaseSimulator::SetupStatus Setup()
  {
    // if (enable_inline_debugger or enable_monitor)
    //   {
    //     SetVariable("debugger.parse-dwarf", true);
    //   }

    if (BaseSimulator::Setup() != unisim::kernel::Simulator::ST_OK_TO_START)
      {
        std::cerr << "Something wrong happened" << std::endl;
        throw std::runtime_error("stop");
      }
    return unisim::kernel::Simulator::ST_OK_TO_START;
  }
  
  virtual void Stop(unisim::kernel::Object *object, int exit_status, bool asynchronous = false)
  {
    throw std::runtime_error("stop");
  }
  
  int GetExitStatus() const { return exit_status; }

  static void DefaultConfiguration(unisim::kernel::Simulator* sim)
  {
    sim->SetVariable("gdb-server.architecture-description-filename", "unisim/service/debug/gdb_server/gdb_power64.xml");
    sim->SetVariable("debug_hub.dwarf-register-number-mapping-filename", "unisim/util/debug/dwarf/power64_dwarf_register_number_mapping.xml");
  }
  
  int exit_status;
  unisim::kernel::logger::console::Printer logger_console_printer;
};

struct Disasm
{
  Disasm( Arch::Operation* _op ) : op(*_op) {}
  Arch::Operation& op;
  friend std::ostream& operator << (std::ostream& sink, Disasm const& d) { d.op.disasm( sink ); return sink; }
};

int
main( int argc, char* argv[] )
{
  Simulator simulator(argv[0]);

  uintptr_t simargs_idx = 1;
  std::vector<std::string> simargs(&argv[simargs_idx], &argv[argc]);
  
  if (simargs.size() == 0)
    {
      std::cerr << "Simulation command line empty." << std::endl;
      return 1;
    }
  else
    {
      std::cerr << "arguments:\n";

      for (int idx = simargs.size(); --idx >= 0;)
        {
          std::cerr << "  args[" << idx << "]: " << simargs[idx] << '\n';
        }
    }

  std::vector<std::string> envs;
  envs.push_back( "LANG=C" );
  
  Arch cpu;
  
  if (char* fpsat = getenv("FP_SHUFFLING_AT"))
    {
      char* range = fpsat;
      cpu.fp_shuffler.addr_range.first  = strtoull(range, &range, 0);
      if (*range++ != ':')
        { std::cerr << "error: expected FP_SHUFFLING_AT=<start address>:<end address>, got: " << fpsat << std::endl; return 1; }
      cpu.fp_shuffler.addr_range.second = strtoull(range, &range, 0);
      if (*range)
        { std::cerr << "error: expected FP_SHUFFLING_AT=<start address>:<end address>, got: " << fpsat << std::endl; return 1; }
      std::cerr << "FP_SHUFFLING_AT=" << std::hex << cpu.fp_shuffler.addr_range.first << ":" << cpu.fp_shuffler.addr_range.second << std::dec << std::endl;
    }
  
  if (char* fpsof = getenv("FP_SHUFFLING_OF"))
    {
      int32_t seeds[4];
      char* seed = const_cast<char*>("");
      for (int i = 0; i < 4; ++i)
        {
          seed = (*seed == ':') ? seed+1 : fpsof;
          seeds[i] = strtol(seed,&seed,0);
        }
      if (*seed)
        { std::cerr << "error: expected FP_SHUFFLING_OF=<seed0>:<seed1>:<seed2>:<seed3>, got: " << fpsof << std::endl; return 1; }
      cpu.fp_shuffler.random.Seed(seeds[0],seeds[1],seeds[2],seeds[3]);
      std::cerr << "FP_SHUFFLING_OF" << std::hex;
      char const* sep = "=";
      for (int i = 0; i < 4; ++i)
        {
          std::cerr << sep << seeds[i];
          sep = ":";
        }
      std::cerr << std::dec << std::endl;
    }
      
  // Loading image
  std::cerr << "*** Loading elf image: " << simargs[0] << " ***" << std::endl;
  
  LinuxOS linux64( std::cerr, &cpu, &cpu, &cpu );
  cpu.SetLinuxOS( &linux64 );
  
  linux64.Setup( simargs, envs );

  //Debugger debugger( cpu, linux64 );

  simulator.Setup();
  
  //  cpu.disasm = false;
  std::cerr << "\n*** Run ***" << std::endl;

  uintptr_t const tail_trace_size = 32;
  uint64_t lastaddrs[tail_trace_size];
  uint64_t stop_address = 0;
  if (char const* stop_addr_env = getenv("YVES_STOP_ADDRESS"))
  {
    stop_address = strtoull(stop_addr_env, 0, 16);
  }
  uint64_t debug_address = 0;
  if (char const* debug_addr_env = getenv("YVES_DEBUG_ADDRESS"))
  {
    debug_address = strtoull(debug_addr_env, 0, 16);
  }
  uint64_t max_instructions = uint64_t(-1);
  if (char const* debug_addr_env = getenv("YVES_MAX_INSTRUCTIONS"))
  {
    max_instructions = strtoull(debug_addr_env, 0, 0);
  }
  
  
  try
    {
      do
        {
          Arch::Operation* op = cpu.fetch();
          //std::cerr << std::hex << op->GetAddr() << std::dec << ": " << Disasm( op ) << std::endl;
          uint64_t current_address = lastaddrs[cpu.insn_count%tail_trace_size] = op->GetAddr();
          if (current_address == stop_address)
            {
              std::cerr << "Stop address hit.\n";
              break;
            }
          if (current_address == debug_address)
            {
              std::cerr << "Debug address hit!\n";
            }
          asm volatile ("operation_execute:");
          bool success = op->execute( &cpu );
          if (not success)
            throw 0;

          cpu.commit();
          
          //{ uint64_t chksum = 0; for (unsigned idx = 0; idx < 8; ++idx) chksum ^= cpu.regread32( idx ); std::cerr << '[' << std::hex << chksum << std::dec << ']'; }

          if (cpu.insn_count >= max_instructions)
            {
              std::cerr << "Max instructions reached (" << max_instructions << ")\n";
              break;
            }
          // if ((cpu.m_instcount % 0x1000000) == 0)
          //   { std::cerr << "Executed instructions: " << std::dec << cpu.m_instcount << " (" << std::hex << op->address << std::dec << ")"<< std::endl; }
        }
      while (not linux64.exited);      
    }
  catch (MisInsn& insn)
    {
      std::cerr << std::hex << insn.addr << ":	";
      for (int idx = 4; --idx >= 0;)
        std::cerr << std::hex << std::setw(2) << std::setfill('0') << ((insn.code >> idx*8) & 0xff) << ' ';
      std::cerr << std::endl;
    }

  std::cerr << "Program exited with status:" << std::dec << linux64.app_ret_status << std::endl;
  std::cerr << "Executed instructions: " << cpu.insn_count << std::endl;

  // for (unsigned idx = 1; idx <= tail_trace_size; ++idx )
  //   {
  //     std::cout << std::hex << lastaddrs[(cpu.insn_count + idx) % tail_trace_size] << std::endl;
  //   }

  return 0;
}

