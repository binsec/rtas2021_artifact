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

#ifndef __UNISIM_KERNEL_TLM2_CLOCK_HH__
#define __UNISIM_KERNEL_TLM2_CLOCK_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <systemc>

namespace unisim {
namespace kernel {
namespace tlm2 {

class ClockPropertiesInterface : virtual public sc_core::sc_interface
{
public:
	virtual const sc_core::sc_time& GetClockPeriod() const = 0;
	virtual double GetClockDutyCycle() const = 0;
	virtual const sc_core::sc_time& GetClockStartTime() const = 0;
	virtual bool GetClockPosEdgeFirst() const = 0;
	virtual bool IsClockFrozen() const = 0;
	virtual bool IsClockLazy() const = 0;
	virtual const sc_core::sc_event& GetClockPropertiesChangedEvent() const = 0;
};

class Clock
	: public sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS>
	, public ClockPropertiesInterface
	, virtual public unisim::kernel::Object
	, public unisim::kernel::VariableBaseListener
{
public:
	typedef sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS> Super;
	
	Clock(const char *name, unisim::kernel::Object *parent = 0);

	virtual ~Clock();
	virtual void write( const bool& );
	const sc_core::sc_time& period() const;
	double duty_cycle() const;
	const sc_core::sc_time& start_time() const;
	bool posedge_first() const;
	virtual const char *kind() const;
	
	void FreezeClock();
	void UnfreezeClock();
	void RequestClockAdjustment(const sc_core::sc_time& period_,
	                            double duty_cycle_ = 0.5);
	
	virtual const sc_core::sc_time& GetClockPeriod() const;
	virtual double GetClockDutyCycle() const;
	virtual const sc_core::sc_time& GetClockStartTime() const;
	virtual bool GetClockPosEdgeFirst() const;
	virtual bool IsClockFrozen() const;
	virtual bool IsClockLazy() const;
	virtual const sc_core::sc_event& GetClockPropertiesChangedEvent() const;
	
	virtual void VariableBaseNotify(const unisim::kernel::VariableBase *var);
protected:
	virtual void before_end_of_elaboration();
private:
	void initialize();
	void posedge_process();
	void negedge_process();
	
	void AdjustClock();
	
	sc_core::sc_time clock_period;
	double clock_duty_cycle;
	sc_core::sc_time clock_start_time;
	bool clock_posedge_first;
	sc_core::sc_time clock_posedge_time;
	sc_core::sc_time clock_negedge_time;
	sc_core::sc_event gen_posedge_event;
	sc_core::sc_event gen_negedge_event;
	
	bool frozen_clock;
	bool lazy_clock;
	bool clock_properties_changed;
	sc_core::sc_time pending_clock_period_change;
	double pending_clock_duty_cycle_change;
	sc_core::sc_event clock_properties_changed_event;
	
	unisim::kernel::variable::Parameter<bool> param_lazy_clock;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_clock_period;
	unisim::kernel::variable::Parameter<double> param_clock_duty_cycle;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_clock_start_time;
	unisim::kernel::variable::Parameter<bool> param_clock_posedge_first;
	
};

class ClockPropertiesProxy
{
public:
	ClockPropertiesProxy(sc_core::sc_in<bool>& clk_port);
	
	bool IsClockCompatible() const;
	const sc_core::sc_time& GetClockPeriod() const;
	double GetClockDutyCycle() const;
	const sc_core::sc_time& GetClockStartTime() const;
	bool GetClockPosEdgeFirst() const;
	bool IsClockFrozen() const;
	const sc_core::sc_event& GetClockPropertiesChangedEvent() const;
private:
	sc_core::sc_in<bool>& clk_port;
	mutable unisim::kernel::tlm2::ClockPropertiesInterface *clk_prop_if;
	
	void Initialize() const;
	void CheckCompatibility() const;
};

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_TLM2_CLOCK_HH__
