/*
 *  Copyright (c) 2014,
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

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_AVR32_AVR32UC_CPU_TCC__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_AVR32_AVR32UC_CPU_TCC__

#include <unistd.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace avr32 {
namespace avr32uc {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG>
CPU<CONFIG>::CPU(const sc_core::sc_module_name& name, Object *parent)
	: Object(name, parent, "this module implements an AVR32UC CPU core")
	, sc_core::sc_module(name)
	, unisim::component::cxx::processor::avr32::avr32a::avr32uc::CPU<CONFIG>(name, parent)
	, ihsb_master_sock("ihsb-master-sock")
	, dhsb_master_sock("dhsb-master-sock")
	, nmireq_slave_sock("nmireq-slave-sock")
	, irq_slave_sock()
	, payload_fabric()
	, cpu_cycle_time()
	, hsb_cycle_time()
	, cpu_time()
	, nice_time()
	, max_idle_time()
	, run_time()
	, idle_time()
	, timers_update_deadline()
	, enable_host_idle(false)
	, ev_max_idle()
	, ev_nmireq()
	, ev_irq()
	, ipc(1.0)
	, one(1.0)
	, enable_dmi(false)
	, debug_dmi(false)
	, param_cpu_cycle_time("cpu-cycle-time", this, cpu_cycle_time, "CPU cycle time")
	, param_hsb_cycle_time("hsb-cycle-time", this, hsb_cycle_time, "HSB cycle time")
	, param_nice_time("nice-time", this, nice_time, "maximum time between synchonizations")
	, param_ipc("ipc", this, ipc, "maximum instructions per cycle (should be <= 1.0)")
	, param_enable_host_idle("enable-host-idle", this, enable_host_idle, "Enable/Disable host idle periods when target is idle")
	, param_enable_dmi("enable-dmi", this, enable_dmi, "Enable/Disable TLM 2.0 DMI (Direct Memory Access) to speed-up simulation")
	, param_debug_dmi("debug-dmi", this, debug_dmi, "Enable/Disable debugging of DMI (Direct Memory Access)")
	, stat_one("one", this, one, "one")
	, stat_run_time("run-time", this, run_time, "run time")
	, stat_idle_time("idle-time", this, idle_time, "idle time")
	, stat_idle_rate("idle-rate", this, "/", &stat_idle_time, &stat_run_time, "idle rate")
	, stat_load_rate("load-rate", this, "-", &stat_one, &stat_idle_rate, "load rate")
	, external_event_schedule()
	, irq_redirector()
	, nmireq_redirector(0)
	, ihsb_redirector(0)
	, dhsb_redirector(0)
	, ihsb_dmi_region_cache()
	, dhsb_dmi_region_cache()
{
	unsigned int irq;
	
	stat_one.SetMutable(false);
	stat_one.SetSerializable(false);
	stat_one.SetVisible(false);
	
	ihsb_redirector = new unisim::kernel::tlm2::BwRedirector<CPU<CONFIG> >(
		IF_IHSB,
		this,
		&CPU<CONFIG>::hsb_nb_transport_bw,
		&CPU<CONFIG>::hsb_invalidate_direct_mem_ptr
	);
	ihsb_master_sock(*ihsb_redirector); // Bind socket to the interface implementer
	
	dhsb_redirector = new unisim::kernel::tlm2::BwRedirector<CPU<CONFIG> >(
		IF_DHSB,
		this,
		&CPU<CONFIG>::hsb_nb_transport_bw,
		&CPU<CONFIG>::hsb_invalidate_direct_mem_ptr
	);
	dhsb_master_sock(*dhsb_redirector); // Bind socket to the interface implementer

	nmireq_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		0,
		this, &CPU<CONFIG>::nmireq_nb_transport_fw,
		&CPU<CONFIG>::nmireq_b_transport,
		&CPU<CONFIG>::nmireq_transport_dbg,
		&CPU<CONFIG>::nmireq_get_direct_mem_ptr
	);
	nmireq_slave_sock(*nmireq_redirector); // Bind socket to implementer of interface
	
	for(irq = 0; irq < CONFIG::NUM_IRQS; irq++)
	{
		std::stringstream sstr_irq_slave_sock;
		sstr_irq_slave_sock << "irq-slave-slock" << irq;
		
		irq_slave_sock[irq] = new irq_slave_socket(sstr_irq_slave_sock.str().c_str());
		
		irq_redirector[irq] = new unisim::kernel::tlm2::FwRedirector<CPU<CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
			irq,
			this,
			&CPU<CONFIG>::irq_nb_transport_fw,
			&CPU<CONFIG>::irq_b_transport,
			&CPU<CONFIG>::irq_transport_dbg,
			&CPU<CONFIG>::irq_get_direct_mem_ptr
		);
		(*irq_slave_sock[irq])(*irq_redirector[irq]); // Bind socket to implementer of interface
	}
	
	SC_HAS_PROCESS(CPU);
	
	SC_THREAD(Run);
}

template <class CONFIG>
CPU<CONFIG>::~CPU()
{
	unsigned int irq;
	
	if(ihsb_redirector)
	{
		delete ihsb_redirector;
	}
	
	if(dhsb_redirector)
	{
		delete dhsb_redirector;
	}
	
	if(nmireq_redirector)
	{
		delete nmireq_redirector;
	}
	
	for(irq = 0; irq < CONFIG::NUM_IRQS; irq++)
	{
		if(irq_redirector[irq])
		{
			delete irq_redirector[irq];
		}
		if(irq_slave_sock[irq])
		{
			delete irq_slave_sock[irq];
		}
	}
}

template <class CONFIG>
tlm::tlm_sync_enum CPU<CONFIG>::hsb_nb_transport_bw(unsigned int if_id, tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == tlm::BEGIN_RESP)
	{
		trans.release();
		return tlm::TLM_COMPLETED;
	}
	return tlm::TLM_ACCEPTED;
}

template <class CONFIG>
void CPU<CONFIG>::hsb_invalidate_direct_mem_ptr(unsigned int if_id, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	switch(if_id)
	{
		case IF_IHSB:
			ihsb_dmi_region_cache.Invalidate(start_range, end_range);
			if(CONFIG::DEBUG_ENABLE && debug_dmi)
			{
				inherited::logger << DebugInfo << "IHSB: invalidate granted access for 0x" << std::hex << start_range << "-0x" << end_range << std::dec << EndDebugInfo;
			}
			break;
		case IF_DHSB:
			dhsb_dmi_region_cache.Invalidate(start_range, end_range);
			if(CONFIG::DEBUG_ENABLE && debug_dmi)
			{
				inherited::logger << DebugInfo << "DHSB: invalidate granted access for 0x" << std::hex << start_range << "-0x" << end_range << std::dec << EndDebugInfo;
			}
			break;
	}
}

template <class CONFIG>
void CPU<CONFIG>::Synchronize()
{
	wait(cpu_time);
	cpu_time = sc_core::SC_ZERO_TIME;
	run_time = sc_core::sc_time_stamp();
	RunInternalTimers();
}

template <class CONFIG>
inline void CPU<CONFIG>::ProcessExternalEvents()
{
	if(!external_event_schedule.Empty())
	{
		sc_core::sc_time time_stamp = sc_core::sc_time_stamp();
		time_stamp += cpu_time;
		Event *event = external_event_schedule.GetNextEvent(time_stamp);
		
		if(unlikely(event != 0))
		{
			do
			{
				switch(event->GetType())
				{
					case Event::EV_NMIREQ:
						ProcessNMIREQEvent(event);
						external_event_schedule.FreeEvent(event);
						break;
					case Event::EV_IRQ:
						ProcessIRQEvent(event);
						external_event_schedule.FreeEvent(event);
						break;
				}
			}
			while((event = external_event_schedule.GetNextEvent(time_stamp)) != 0);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::ProcessNMIREQEvent(Event *event)
{
	if(inherited::IsVerboseInterrupt())
	{
		inherited::logger << DebugInfo << (sc_core::sc_time_stamp() + cpu_time) << ": processing an NMIREQ event that occured at " << event->GetTimeStamp() << " (";
		inherited::logger << "NMIREQ goes " << (event->GetLevel() ? "high" : "low") << "). Event skew is " << (sc_core::sc_time_stamp() + cpu_time - event->GetTimeStamp()) << "." << EndDebugInfo;
	}
	if(event->GetLevel())
		inherited::SetNMIREQ();
	else
		inherited::ResetNMIREQ();

	ev_nmireq.notify();
}

template <class CONFIG>
void CPU<CONFIG>::ProcessIRQEvent(Event *event)
{
	if(inherited::IsVerboseInterrupt())
	{
		inherited::logger << DebugInfo << (sc_core::sc_time_stamp() + cpu_time) << ": processing an IRQ event that occured at " << event->GetTimeStamp() << " (";
		inherited::logger << "IRQ #" << event->GetIRQ();
		inherited::logger << " input goes " << (event->GetLevel() ? "high" : "low") << "). Event skew is " << (sc_core::sc_time_stamp() + cpu_time - event->GetTimeStamp()) << "." << EndDebugInfo;
	}
	if(event->GetLevel())
		inherited::SetIRQ(event->GetIRQ());
	else
		inherited::ResetIRQ(event->GetIRQ());
	
	ev_irq.notify();
}

template <class CONFIG>
void CPU<CONFIG>::nmireq_b_transport(unsigned int, InterruptPayload& payload, sc_core::sc_time& t)
{
	bool level = payload.GetValue();
	sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
	notify_time_stamp += t;

	AlignToHSBClock(notify_time_stamp);
	if(inherited::IsVerboseInterrupt())
	{
		inherited::logger << DebugInfo << notify_time_stamp << ": NMIREQ signal goes " << (level ? "high" : "low") << EndDebugInfo;
	}
	external_event_schedule.NotifyNMIREQEvent(level, notify_time_stamp);
}

template <class CONFIG>
tlm::tlm_sync_enum CPU<CONFIG>::nmireq_nb_transport_fw(unsigned int, InterruptPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				bool level = payload.GetValue();
				sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
				notify_time_stamp += t;

				AlignToHSBClock(notify_time_stamp);
				if(inherited::IsVerboseInterrupt())
				{
					inherited::logger << DebugInfo << notify_time_stamp << ": NMIREQ signal goes " << (level ? "high" : "low") << EndDebugInfo;
				}
				external_event_schedule.NotifyNMIREQEvent(level, notify_time_stamp);
				
				phase = tlm::END_REQ;
				return tlm::TLM_COMPLETED;
			}
			break;
		case tlm::END_RESP:
			return tlm::TLM_COMPLETED;
		default:
			inherited::logger << DebugError << "protocol error" << EndDebugError;
			Object::Stop(-1);
			break;
	}
	return tlm::TLM_COMPLETED;
}

template <class CONFIG>
unsigned int CPU<CONFIG>::nmireq_transport_dbg(unsigned int, InterruptPayload& trans)
{
	return 0;
}

template <class CONFIG>
bool CPU<CONFIG>::nmireq_get_direct_mem_ptr(unsigned int, InterruptPayload& payload, tlm::tlm_dmi& dmi_data)
{
	return false;
}

template <class CONFIG>
void CPU<CONFIG>::irq_b_transport(unsigned int irq, InterruptPayload& payload, sc_core::sc_time& t)
{
	if(irq < CONFIG::NUM_IRQS)
	{
		bool level = payload.GetValue();
		sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
		notify_time_stamp += t;

		AlignToHSBClock(notify_time_stamp);
		if(inherited::IsVerboseInterrupt())
		{
			inherited::logger << DebugInfo << notify_time_stamp << ": IRQ #" << irq << " signal goes " << (level ? "high" : "low") << EndDebugInfo;
		}
		external_event_schedule.NotifyIRQEvent(irq, level, notify_time_stamp);
	}
	else
	{
		inherited::logger << DebugError << "protocol error (invalid IRQ number)" << EndDebugError;
		Object::Stop(-1);
	}
}

template <class CONFIG>
tlm::tlm_sync_enum CPU<CONFIG>::irq_nb_transport_fw(unsigned int irq, InterruptPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				if(irq < CONFIG::NUM_IRQS)
				{
					bool level = payload.GetValue();
					sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
					notify_time_stamp += t;

					AlignToHSBClock(notify_time_stamp);
					if(inherited::IsVerboseInterrupt())
					{
						inherited::logger << DebugInfo << notify_time_stamp << ": IRQ #" << irq << " signal goes " << (level ? "high" : "low") << EndDebugInfo;
					}
					external_event_schedule.NotifyIRQEvent(irq, level, notify_time_stamp);
				}
				else
				{
					inherited::logger << DebugError << "protocol error (invalid IRQ number)" << EndDebugError;
					Object::Stop(-1);
				}
				
				phase = tlm::END_REQ;
				return tlm::TLM_COMPLETED;
			}
			break;
		case tlm::END_RESP:
			return tlm::TLM_COMPLETED;
		default:
			inherited::logger << DebugError << "protocol error" << EndDebugError;
			Object::Stop(-1);
			break;
	}
	return tlm::TLM_COMPLETED;
}

template <class CONFIG>
unsigned int CPU<CONFIG>::irq_transport_dbg(unsigned int, InterruptPayload& trans)
{
	return 0;
}

template <class CONFIG>
bool CPU<CONFIG>::irq_get_direct_mem_ptr(unsigned int, InterruptPayload& payload, tlm::tlm_dmi& dmi_data)
{
	return false;
}

template <class CONFIG>
inline void CPU<CONFIG>::RunInternalTimers()
{
	const sc_core::sc_time& timer_cycle_time = cpu_cycle_time;
		
#if 0
	if(run_time >= (timer_time + timer_cycle_time))
	{
		do
		{
			inherited::RunTimers(1);
			timer_time += timer_cycle_time;
		}
		while(run_time >= (timer_time + timer_cycle_time));
	}
#else
	// Note: this code brings a slight speed improvement (6 %)
#if 0
	sc_core::sc_time delta_time(run_time);
	delta_time -= timer_time;
	uint64_t delta = (uint64_t) floor(delta_time / timer_cycle_time);
	inherited::RunTimers(delta);
	sc_core::sc_time t(timer_cycle_time);
	t *= (double) delta;
	timer_time += t;
#else
	sc_dt::uint64 delta_time_tu = run_time.value() - timer_time.value();
	sc_dt::uint64 timer_cycle_time_tu = timer_cycle_time.value();
	uint64_t delta = delta_time_tu / timer_cycle_time_tu;
	inherited::RunTimers(delta);
	sc_dt::uint64 t_tu = timer_cycle_time_tu * delta;
	sc_core::sc_time t(sc_core::sc_get_time_resolution());
	t *= t_tu;
	timer_time += t;
#endif
#endif
	timers_update_deadline = timer_time + (inherited::GetTimersDeadline() * timer_cycle_time);
}

template <class CONFIG>
inline void CPU<CONFIG>::LazyRunInternalTimers()
{
	if(unlikely(run_time >= timers_update_deadline))
	{
		RunInternalTimers();
	}
}

template <class CONFIG>
inline void CPU<CONFIG>::AlignToHSBClock()
{
	sc_core::sc_time time_skew(run_time);
	time_skew %= hsb_cycle_time;
	
	if(time_skew == sc_core::SC_ZERO_TIME) return; // already aligned
	
	sc_core::sc_time time_alignment(hsb_cycle_time);
	time_alignment -= time_skew;
	
	cpu_time += time_alignment;
	run_time += time_alignment;
}

template <class CONFIG>
void CPU<CONFIG>::AlignToHSBClock(sc_core::sc_time& t)
{
	sc_core::sc_time time_skew(t);
	time_skew %= hsb_cycle_time;
	
	if(time_skew == sc_core::SC_ZERO_TIME) return; // already aligned
	
	sc_core::sc_time time_alignment(hsb_cycle_time);
	time_alignment -= time_skew;
	
	t += time_alignment;

// 	sc_dt::uint64 hsb_cycle_time_tu = hsb_cycle_time.value();
// 	sc_dt::uint64 time_tu = t.value();
// 	sc_dt::uint64 modulo = time_tu % hsb_cycle_time_tu;
// 	if(!modulo) return; // already aligned
// 	
// 	sc_dt::uint64 time_alignment_tu = hsb_cycle_time_tu - modulo;
// 	sc_core::sc_time time_alignment(time_alignment_tu, false);
// 	t += time_alignment;
}

template <class CONFIG>
void CPU<CONFIG>::Idle()
{
	// This is called within thread Run()
	
	// First synchronize with other threads
	Synchronize();
	
	// Compute the time to consume before an internal timer event occurs
	const sc_core::sc_time& timer_cycle_time = cpu_cycle_time;
	max_idle_time = timer_cycle_time;
	max_idle_time *= inherited::GetMaxIdleTime(); // max idle time (timer_time based)

	max_idle_time += timer_time;
	max_idle_time -= run_time;                    // max idle time (run_time based)
	
	if((run_time - timer_time) > timer_cycle_time)
	{
		std::cerr << "WARNING! (run_time - timer_time)= " << (run_time - timer_time) << ", timer_cycle_time=" << timer_cycle_time << std::endl;
	}
	//std::cerr << "run_time + max_idle_time = " << (run_time + max_idle_time) << ", timers_update_deadline=" << timers_update_deadline << std::endl;
	if(timers_update_deadline > (run_time + max_idle_time))
	{
		std::cerr << "WARNING! timers_update_deadline= " << timers_update_deadline << "> (run_time + max_idle_time)=" << (run_time + max_idle_time) << std::endl;
	}
	// Notify an event
	ev_max_idle.notify(max_idle_time);
	
	// wait for either an internal timer event or an external event
	sc_core::sc_time old_time_stamp(sc_core::sc_time_stamp());
	wait(ev_max_idle | ev_nmireq | ev_irq);
	sc_core::sc_time new_time_stamp(sc_core::sc_time_stamp());
	
	// cancel event because we can't know which wake up condition occured
	ev_max_idle.cancel();
	
	// compute the time spent by the SystemC wait
	sc_core::sc_time delta_time(new_time_stamp);
	delta_time -= old_time_stamp;
	
	if(enable_host_idle)
	{
		usleep(delta_time.to_seconds() * 1.0e6); // leave host CPU when target CPU is idle
	}
	
	idle_time += delta_time;
	
	// update overall run time
	run_time = new_time_stamp;
	
	// run internal timers
	LazyRunInternalTimers();
	
	// check for external events
	ProcessExternalEvents();
}

template <class CONFIG>
void CPU<CONFIG>::Run()
{
	sc_core::sc_time time_per_instruction = cpu_cycle_time / ipc;
	
	while(1)
	{
		inherited::StepOneInstruction();
		// update local time (relative to sc_time_stamp)
		cpu_time += time_per_instruction;
		// update absolute time (local time + sc_time_stamp)
		run_time += time_per_instruction;
		// Run internal timers
		LazyRunInternalTimers();
		// check IRQs
		ProcessExternalEvents();
		// Periodically synchronize with other threads
		if(unlikely(cpu_time >= nice_time))
		{
			Synchronize();
		}
	}
}

template <class CONFIG>
bool CPU<CONFIG>::IHSBRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size)
{
	AlignToHSBClock();

	unisim::kernel::tlm2::DMIRegion *dmi_region = 0;
	
	if(likely(enable_dmi))
	{
		dmi_region = ihsb_dmi_region_cache.Lookup(physical_addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "IHSB: DMI region cache hit for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "IHSB: DMI access allowed for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_READ) == tlm::tlm_dmi::DMI_ACCESS_READ))
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "IHSB: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(buffer, dmi_data->get_dmi_ptr() + (physical_addr - dmi_data->get_start_address()), size);
					const sc_core::sc_time& read_lat = dmi_region->GetReadLatency(size);
					cpu_time += read_lat;
					run_time += read_lat;
					LazyRunInternalTimers();
					return true;
				}
				else
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "IHSB: granted DMI access does not allow direct read access for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "IHSB: DMI access denied for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "IHSB: DMI region cache miss for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}
	
	if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "IHSB: traditional way for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_READ_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	ihsb_master_sock->b_transport(*payload, cpu_time);
	
	run_time = sc_core::sc_time_stamp();
	run_time += cpu_time;
	LazyRunInternalTimers();
	
	tlm::tlm_response_status status = payload->get_response_status();
	
	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "IHSB: target allows DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			
			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			unisim::kernel::tlm2::DMIGrant dmi_grant = ihsb_master_sock->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			ihsb_dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "IHSB: target does not allow DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}
	
	payload->release();
	
	return status == tlm::TLM_OK_RESPONSE;
}

template <class CONFIG>
bool CPU<CONFIG>::DHSBRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size)
{
	AlignToHSBClock();
	
	unisim::kernel::tlm2::DMIRegion *dmi_region = 0;
	
	if(likely(enable_dmi))
	{
		dmi_region = dhsb_dmi_region_cache.Lookup(physical_addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Read: DMI region cache hit for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Read: DMI access allowed for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_READ) == tlm::tlm_dmi::DMI_ACCESS_READ))
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Read: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(buffer, dmi_data->get_dmi_ptr() + (physical_addr - dmi_data->get_start_address()), size);
					const sc_core::sc_time& read_lat = dmi_region->GetReadLatency(size);
					cpu_time += read_lat;
					run_time += read_lat;
					LazyRunInternalTimers();
					return true;
				}
				else
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Read: granted DMI access does not allow direct read access for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Read: DMI access denied for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Read: DMI region cache miss for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Read: traditional way for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_READ_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	dhsb_master_sock->b_transport(*payload, cpu_time);
	
	run_time = sc_core::sc_time_stamp();
	run_time += cpu_time;
	LazyRunInternalTimers();

	tlm::tlm_response_status status = payload->get_response_status();
	
	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Read: target allows DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;

			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			unisim::kernel::tlm2::DMIGrant dmi_grant = dhsb_master_sock->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			dhsb_dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Read: target does not allow DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	payload->release();

	return status == tlm::TLM_OK_RESPONSE;
}

template <class CONFIG>
bool CPU<CONFIG>::DHSBWrite(typename CONFIG::physical_address_t physical_addr, const void *buffer, uint32_t size)
{
	AlignToHSBClock();
	
	unisim::kernel::tlm2::DMIRegion *dmi_region = 0;

	if(likely(enable_dmi))
	{
		dmi_region = dhsb_dmi_region_cache.Lookup(physical_addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Write: DMI region cache hit for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Write: DMI access allowed for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_WRITE) == tlm::tlm_dmi::DMI_ACCESS_WRITE))
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Write: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(dmi_data->get_dmi_ptr() + (physical_addr - dmi_data->get_start_address()), buffer, size);
					const sc_core::sc_time& write_lat = dmi_region->GetWriteLatency(size);
					cpu_time += write_lat;
					run_time += write_lat;
					LazyRunInternalTimers();
					return true;
				}
				else
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Write: granted DMI access does not allow direct write access for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Write: DMI access denied for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Write: DMI region cache miss for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Write: traditional way for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_WRITE_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	dhsb_master_sock->b_transport(*payload, cpu_time);
	
	run_time = sc_core::sc_time_stamp();
	run_time += cpu_time;
	LazyRunInternalTimers();

	tlm::tlm_response_status status = payload->get_response_status();

	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Write: target allows DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			unisim::kernel::tlm2::DMIGrant dmi_grant = dhsb_master_sock->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			dhsb_dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) inherited::logger << DebugInfo << "DHSB Write: target does not allow DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	payload->release();

	return status == tlm::TLM_OK_RESPONSE;
}

template <class CONFIG>
void CPU<CONFIG>::end_of_simulation()
{
	RunInternalTimers(); // make run_time match timer_time
}

} // end of namespace avr32uc
} // end of namespace avr32
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
