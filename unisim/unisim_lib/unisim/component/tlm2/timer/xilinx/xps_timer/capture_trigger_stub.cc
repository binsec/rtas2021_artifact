/*
 *  Copyright (c) 2011,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <unisim/component/tlm2/timer/xilinx/xps_timer/capture_trigger_stub.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace timer {
namespace xilinx {
namespace xps_timer {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;

CaptureTriggerStub::CaptureTriggerStub(const sc_core::sc_module_name& name, Object *parent)
	: Object(name, parent, "A stub that, if enabled, can generate random inputs for a capture timer")
	, sc_core::sc_module(name)
	, master_sock("master-sock")
	, logger(*this)
	, random()
	, capture_trigger_payload_fabric()
	, cycle_time(sc_core::SC_ZERO_TIME)
	, nice_time(sc_core::sc_time(1, sc_core::SC_MS))
	, time(sc_core::SC_ZERO_TIME)
	, verbose(false)
	, enable(true)
	, randomized_output(false)
	, random_seed(-123456)
	, random_period_min(200)
	, random_period_max(799)
	, param_cycle_time("cycle-time", this, cycle_time, "cycle time")
	, param_nice_time("nice-time", this, nice_time, "nice time")
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_enable("enable", this, enable, "Enable/Disable")
	, param_randomized_output("randomized-output", this, randomized_output, "Enable/Disable randomized output")
	, param_random_seed("random-seed", this, random_seed, "Seed for the random generator")
	, param_random_period_min("random-period-min", this, random_period_min, "Minimum period in cycles of randomized output")
	, param_random_period_max("random-period-max", this, random_period_max, "Maximum period in cycles of randomized output")
{
	param_random_seed.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_random_period_min.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_random_period_max.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_enable.SetMutable(false);
	param_randomized_output.SetMutable(false);
	param_random_seed.SetMutable(false);
	param_random_period_min.SetMutable(false);
	param_random_period_max.SetMutable(false);

	master_sock(*this);
	
	SC_HAS_PROCESS(CaptureTriggerStub);
	
	SC_THREAD(Process);
}

bool CaptureTriggerStub::BeginSetup()
{
	random.Seed(random_seed | 1, random_seed | 2, random_seed | 4, random_seed | 8);
	
	return true;
}

tlm::tlm_sync_enum CaptureTriggerStub::nb_transport_bw(unisim::component::tlm2::timer::xilinx::xps_timer::CaptureTriggerPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if(enable)
	{
		switch(phase)
		{
			case tlm::END_REQ:
				return tlm::TLM_ACCEPTED;
			case tlm::BEGIN_RESP:
				return tlm::TLM_COMPLETED;
			default:
				logger << unisim::kernel::logger::DebugError << "protocol error" << unisim::kernel::logger::EndDebugError;
				Object::Stop(-1);
				break;
		}
	}
	else
	{
		logger << unisim::kernel::logger::DebugError << "nb_transport_bw shall not be called" << unisim::kernel::logger::EndDebugError;
		Object::Stop(-1);
	}
	return tlm::TLM_COMPLETED;
}

void CaptureTriggerStub::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	if(!enable)
	{
		logger << unisim::kernel::logger::DebugError << "invalidate_direct_mem_ptr shall not be called" << unisim::kernel::logger::EndDebugError;
		Object::Stop(-1);
	}
}

void CaptureTriggerStub::Process()
{
	if(!enable || !randomized_output) return;
	
	bool output_level = false;
	int32_t p = 0;   // period in cycles
	int32_t d = 0;   // duty in cycles
	sc_core::sc_time period(sc_core::SC_ZERO_TIME);
	sc_core::sc_time duty(sc_core::SC_ZERO_TIME);
	sc_core::sc_time delay(sc_core::SC_ZERO_TIME);
	
	while(1)
	{
		// Compute next output
		if(output_level)
		{
			d = random.Generate((p / 2) - 1) + (p / 2);
			// d is in [1; p - 2]
			if(d < 1 || d > (p - 1))
			{
				logger << unisim::kernel::logger::DebugError << "Internal error in random generator" << unisim::kernel::logger::EndDebugError;
				Object::Stop(-1);
			}
			duty = cycle_time;
			duty *= d;
			if(verbose)
			{
				logger << unisim::kernel::logger::DebugInfo << "Duty = " << duty << unisim::kernel::logger::EndDebugInfo;
			}
			// duty is in [cycle_time; period - 2 * cycle_time] */
			delay = duty;
		}
		else
		{
			delay = period - duty;

			unsigned int radius = (random_period_max - random_period_min + 1) / 2; /* = 160 with default values */
			p = random.Generate(radius) + radius + random_period_min;
			// p is in [random_period_min; random_period_max]
			if((p < (int32_t) random_period_min) || (p > (int32_t) random_period_max))
			{
				logger << unisim::kernel::logger::DebugError << "Internal error in random generator" << unisim::kernel::logger::EndDebugError;
				Object::Stop(-1);
			}
			period = cycle_time;
			period *= p;
			if(verbose)
			{
				logger << unisim::kernel::logger::DebugInfo << "Period = " << period << unisim::kernel::logger::EndDebugInfo;
			}
			// period is in [random_period_min; random_period_max] cycles ([400; 1995] nanoseconds with default values and with a 5 nanoseconds cycle time) */
		}
		
		output_level = !output_level;
		
		time += delay;
		
		if(verbose)
		{
			logger << unisim::kernel::logger::DebugInfo << (sc_core::sc_time_stamp() + time) << ": Output goes " << (output_level ? "high" : "low") << " after " << delay << unisim::kernel::logger::EndDebugInfo;
		}
		
		unisim::component::tlm2::timer::xilinx::xps_timer::CaptureTriggerPayload *capture_trigger_payload = capture_trigger_payload_fabric.allocate();
		
		capture_trigger_payload->SetValue(output_level);
		
		tlm::tlm_phase phase = tlm::BEGIN_REQ;
		master_sock->nb_transport_fw(*capture_trigger_payload, phase, time);
		
		capture_trigger_payload->release();
		
		if(time >= nice_time)
		{
			wait(nice_time);
			time -= nice_time;
		}
	}
}

} // end of namespace xps_timer
} // end of namespace xilinx
} // end of namespace timer
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim
