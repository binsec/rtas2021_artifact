/*
 *  Copyright (c) 2017,
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

#include <unisim/kernel/tlm2/clock.hh>
#include <stdexcept>

namespace unisim {
namespace kernel {
namespace tlm2 {

//////////////////////////////////// Clock /////////////////////////////////////////////

Clock::Clock(const char *name_, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name_, parent, "Clock")
	, Super(name_)
	, clock_period(sc_core::sc_time(1.0, sc_core::SC_NS))
	, clock_duty_cycle(0.5)
	, clock_start_time(sc_core::SC_ZERO_TIME)
	, clock_posedge_first(true)
	, gen_posedge_event(sc_core::sc_gen_unique_name("gen_posedge_event"))
	, gen_negedge_event(sc_core::sc_gen_unique_name("gen_negedge_event"))
	, frozen_clock(false)
	, lazy_clock(false)
	, clock_properties_changed(false)
	, pending_clock_period_change(clock_period)
	, pending_clock_duty_cycle_change(clock_duty_cycle)
	, clock_properties_changed_event(sc_core::sc_gen_unique_name("clock_properties_changed_event"))
	, param_lazy_clock("lazy-clock", this, lazy_clock, "whether clock is lazy (fast) or really toggles (painfully slow)")
	, param_clock_period("clock-period", this, clock_period, "clock period")
	, param_clock_duty_cycle("clock-duty-cycle", this, clock_duty_cycle, "clock duty cycle (between 0.0 and 1.0 exclusive)")
	, param_clock_start_time("clock-start-time", this, clock_start_time, "clock start time")
	, param_clock_posedge_first("clock-posedge-first", this, clock_posedge_first, "whether clocks starts with posedge (rising edge) or falling edge (negedge)")
{
	param_lazy_clock.SetMutable(false);
	param_clock_period.SetMutable(false);
	param_clock_duty_cycle.SetMutable(false);
	param_clock_start_time.SetMutable(false);
	param_clock_posedge_first.SetMutable(false);
	initialize();
}

Clock::~Clock()
{
}

void Clock::write(const bool& value)
{
	throw std::runtime_error("attempt to write clock");
}

const sc_core::sc_time& Clock::period() const
{
	return clock_period;
}

double Clock::duty_cycle() const
{
	return clock_duty_cycle;
}

const sc_core::sc_time& Clock::start_time() const
{
	return clock_start_time;
}

bool Clock::posedge_first() const
{
	return clock_posedge_first;
}

const char* Clock::kind() const
{
	return "unisim::kernel::tlm2::Clock";
}

void Clock::before_end_of_elaboration()
{
	Super::write(!clock_posedge_first);
	
	if(!lazy_clock)
	{
		if(clock_posedge_first)
			gen_posedge_event.notify(clock_start_time);
		else
			gen_negedge_event.notify(clock_start_time);
	
		sc_core::sc_spawn_options posedge_process_spawn_options;
		posedge_process_spawn_options.spawn_method();
		posedge_process_spawn_options.set_sensitivity(&gen_posedge_event);
		posedge_process_spawn_options.dont_initialize();
		sc_core::sc_spawn(sc_bind(&Clock::posedge_process, this), "posedge_process", &posedge_process_spawn_options);
		
		sc_core::sc_spawn_options negedge_process_spawn_options;
		negedge_process_spawn_options.spawn_method();
		negedge_process_spawn_options.set_sensitivity(&gen_negedge_event);
		negedge_process_spawn_options.dont_initialize();
		sc_core::sc_spawn(sc_bind(&Clock::negedge_process, this), "negedge_process", &negedge_process_spawn_options);
	}
	
	clock_properties_changed_event.notify(clock_start_time);
}

void Clock::initialize()
{
	if(clock_period <= sc_core::SC_ZERO_TIME)
	{
		throw std::runtime_error("clock period shall be greater than zero");
	}
	
	if((clock_duty_cycle <= 0.0) || (clock_duty_cycle >= 1.0))
	{
		throw std::runtime_error("clock duty cycle shall lie between the limits 0.0 and 1.0, exclusive");
	}
	
	clock_negedge_time = clock_period * clock_duty_cycle;
	clock_posedge_time = clock_period - clock_negedge_time;
}

void Clock::posedge_process()
{
	if(clock_properties_changed)
	{
		AdjustClock();
		clock_properties_changed = false;
	}
	
	if(frozen_clock)
	{
		return;
	}
	
	Super::write(true);
	gen_negedge_event.notify(clock_negedge_time);
}

void Clock::negedge_process()
{
	if(clock_properties_changed)
	{
		AdjustClock();
		clock_properties_changed = false;
	}
	
	if(frozen_clock)
	{
		return;
	}

	Super::write(false);
	gen_posedge_event.notify(clock_posedge_time);
}

void Clock::FreezeClock()
{
	if(!frozen_clock)
	{
		frozen_clock = true;
		clock_properties_changed = true;
	}
}

void Clock::UnfreezeClock()
{
	if(frozen_clock)
	{
		frozen_clock = false;
		clock_properties_changed = true;

		if(!lazy_clock && (sc_core::sc_time_stamp() >= clock_start_time))
		{
			if(Super::read())
			{
				gen_negedge_event.notify(sc_core::SC_ZERO_TIME);
			}
			else
			{
				gen_posedge_event.notify(sc_core::SC_ZERO_TIME);
			}
		}
	}
}

void Clock::AdjustClock()
{
	clock_period = pending_clock_period_change;
	clock_duty_cycle = pending_clock_duty_cycle_change;
	initialize();
	clock_properties_changed_event.notify(sc_core::SC_ZERO_TIME);
}

void Clock::RequestClockAdjustment(const sc_core::sc_time& period_,
                                   double duty_cycle_)
{
	pending_clock_period_change = period_;
	pending_clock_duty_cycle_change = duty_cycle_;
	
	if(lazy_clock)
	{
		AdjustClock();
	}
	else
	{
		clock_properties_changed = true;
	}
}

const sc_core::sc_time& Clock::GetClockPeriod() const
{
	return clock_period;
}

double Clock::GetClockDutyCycle() const
{
	return clock_duty_cycle;
}

const sc_core::sc_time& Clock::GetClockStartTime() const
{
	return clock_start_time;
}

bool Clock::GetClockPosEdgeFirst() const
{
	return clock_posedge_first;
}

bool Clock::IsClockFrozen() const
{
	return frozen_clock;
}

bool Clock::IsClockLazy() const
{
	return lazy_clock;
}

const sc_core::sc_event& Clock::GetClockPropertiesChangedEvent() const
{
	return clock_properties_changed_event;
}

void Clock::VariableBaseNotify(const unisim::kernel::VariableBase *var)
{
	clock_properties_changed_event.notify(sc_core::SC_ZERO_TIME);
}

ClockPropertiesProxy::ClockPropertiesProxy(sc_core::sc_in<bool>& clk_port_)
	: clk_port(clk_port_)
	, clk_prop_if(0)
{
}

void ClockPropertiesProxy::Initialize() const
{
	clk_prop_if = dynamic_cast<unisim::kernel::tlm2::ClockPropertiesInterface *>(clk_port[0]);
}

bool ClockPropertiesProxy::IsClockCompatible() const
{
	if(!clk_prop_if) Initialize();
	return clk_prop_if != 0;
}

void ClockPropertiesProxy::CheckCompatibility() const
{
	if(!IsClockCompatible())
	{
		throw std::runtime_error("ClockPropertiesProxy: clock port is not bound to a unisim::kernel::tlm2::Clock");
	}
}

const sc_core::sc_time& ClockPropertiesProxy::GetClockPeriod() const
{
	CheckCompatibility();
	return clk_prop_if->GetClockPeriod();
}

double ClockPropertiesProxy::GetClockDutyCycle() const
{
	CheckCompatibility();
	return clk_prop_if->GetClockDutyCycle();
}

const sc_core::sc_time& ClockPropertiesProxy::GetClockStartTime() const
{
	CheckCompatibility();
	return clk_prop_if->GetClockStartTime();
}

bool ClockPropertiesProxy::GetClockPosEdgeFirst() const
{
	CheckCompatibility();
	return clk_prop_if->GetClockPosEdgeFirst();
}

bool ClockPropertiesProxy::IsClockFrozen() const
{
	CheckCompatibility();
	return clk_prop_if->IsClockFrozen();
}

const sc_core::sc_event& ClockPropertiesProxy::GetClockPropertiesChangedEvent() const
{
	CheckCompatibility();
	return clk_prop_if->GetClockPropertiesChangedEvent();
}

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim
