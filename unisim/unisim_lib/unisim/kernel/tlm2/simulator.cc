/*
 *  Copyright (c) 2015-2016,
 *  Commissariat a l'Energie Atomique et aux Energies Alternatives (CEA)
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

#include <unisim/kernel/tlm2/simulator.hh>
#include <unisim/kernel/tlm2/tlm_can.hh>

namespace unisim {
namespace kernel {
namespace tlm2 {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

Simulator::Simulator(sc_core::sc_module_name const& name, int argc, char **argv, void (*LoadBuiltInConfig)(unisim::kernel::Simulator *simulator))
	: unisim::kernel::Simulator(argc, argv, LoadBuiltInConfig)
	, unisim::kernel::Object(name)
	, logger(*this)
	, max_time(sc_core::SC_ZERO_TIME)
	, param_max_time("max-time", 0, max_time, "Maximum time to simulate (zero means forever)")
	, stat_cur_sim_time("cur-sim-time", this, *const_cast<sc_core::sc_time *>(&sc_core::sc_time_stamp()), "SystemC current simulation time (as returned by sc_core::sc_time_stamp()) ")
	, global_quantum(sc_core::SC_ZERO_TIME)
	, param_global_quantum("global-quantum", this, global_quantum, "SystemC TLM-2.0 global quantum")
	, can_global_quantum(sc_core::SC_ZERO_TIME)
	, param_can_global_quantum("can-global-quantum", this, can_global_quantum, "SystemC TLM CAN global quantum")
	, stop_called(false)
	, exit_status(0)
{
	stat_cur_sim_time.SetMutable(false);
	stat_cur_sim_time.SetSerializable(false);
	param_global_quantum.SetMutable(false);
	param_can_global_quantum.SetMutable(false);
	
	tlm::tlm_global_quantum::instance().set(global_quantum);
	tlm_can_global_quantum::instance().set(can_global_quantum);
}

Simulator::~Simulator()
{
}

void Simulator::Run()
{
	if(!stop_called)
	{
		sc_core::sc_report_handler::set_actions(sc_core::SC_INFO, sc_core::SC_DO_NOTHING); // disable SystemC messages
		
		try
		{
			if(max_time != sc_core::SC_ZERO_TIME)
			{
				sc_core::sc_start(max_time);
			}
			else
			{
				sc_core::sc_start();
			}
		}
		catch(std::runtime_error& e)
		{
			std::cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << std::endl;
			std::cerr << e.what() << std::endl;
		}
	}

	std::cerr << "Program exited with status " << GetExitStatus() << std::endl;
	std::cerr << "Simulation finished" << std::endl;
}

void Simulator::Stop(unisim::kernel::Object *object, int _exit_status, bool asynchronous)
{
	if(!stop_called)
	{
		stop_called = true;
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
}

int Simulator::GetExitStatus() const
{
	return exit_status;
}

bool Simulator::IsRunning() const
{
  return sc_core::sc_is_running();
}

bool Simulator::SimulationStarted() const
{
  return sc_core::sc_start_of_simulation_invoked();
}

bool Simulator::SimulationFinished() const
{
  return sc_core::sc_end_of_simulation_invoked();
}

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim
