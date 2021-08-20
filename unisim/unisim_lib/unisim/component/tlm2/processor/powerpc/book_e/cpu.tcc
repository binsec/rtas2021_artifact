/*
 *  Copyright (c) 2007-2019,
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

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_BOOK_E_CPU_TCC__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_BOOK_E_CPU_TCC__

#include <unistd.h>
#include <unisim/component/cxx/processor/powerpc/book_e/cpu.tcc>

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace powerpc {
namespace book_e {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::CPU(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent, "PPC440 PowerPC core")
	, sc_core::sc_module(name)
	, Super(name, parent)
	, icurd_plb_master_sock("icurd-plb-master-sock")
	, dcuwr_plb_master_sock("dcuwr-plb-master-sock")
	, dcurd_plb_master_sock("dcurd-plb-master-sock")
	, external_input_interrupt_slave_sock("external-input-interrupt-slave-sock")
	, critical_input_interrupt_slave_sock("critical-input-interrupt-slave-sock")
	, dcr_master_sock("dcr-master-sock")
	, payload_fabric()
	, cpu_cycle_time()
	, bus_cycle_time()
	, ext_timer_cycle_time()
	, cpu_time()
	, nice_time()
	, max_idle_time()
	, run_time()
	, idle_time()
	, timers_update_deadline()
	, enable_host_idle(false)
	, ev_max_idle()
	, ev_irq()
	, ipc(2.0)
	, one(1.0)
	, enable_dmi(false)
	, debug_dmi(false)
	, param_cpu_cycle_time("cpu-cycle-time", this, cpu_cycle_time, "CPU cycle time")
	, param_bus_cycle_time("bus-cycle-time", this, bus_cycle_time, "bus cycle time")
	, param_ext_timer_cycle_time("ext-timer-cycle-time", this, ext_timer_cycle_time, "external timer cycle time")
	, param_nice_time("nice-time", this, nice_time, "maximum time between synchonizations")
	, param_ipc("ipc", this, ipc, "maximum instructions per cycle (should be <= 2.0)")
	, param_enable_host_idle("enable-host-idle", this, enable_host_idle, "Enable/Disable host idle periods when target is idle")
	, param_enable_dmi("enable-dmi", this, enable_dmi, "Enable/Disable TLM 2.0 DMI (Direct Memory Access) to speed-up simulation")
	, param_debug_dmi("debug-dmi", this, debug_dmi, "Enable/Disable debugging of DMI (Direct Memory Access)")
	, stat_one("one", this, one, "one")
	, stat_run_time("run-time", this, run_time, "run time")
	, stat_idle_time("idle-time", this, idle_time, "idle time")
	, stat_idle_rate("idle-rate", this, "/", &stat_idle_time, &stat_run_time, "idle rate")
	, stat_load_rate("load-rate", this, "-", &stat_one, &stat_idle_rate, "load rate")
	, external_event_schedule()
	, critical_input_interrupt_redirector(0)
	, external_input_interrupt_redirector(0)
	, icurd_plb_redirector(0)
	, dcuwr_plb_redirector(0)
	, dcurd_plb_redirector(0)
	, dcr_redirector(0)
	, icurd_dmi_region_cache()
	, dcuwr_dmi_region_cache()
	, dcurd_dmi_region_cache()
{
	stat_one.SetMutable(false);
	stat_one.SetSerializable(false);
	stat_one.SetVisible(false);
	
	icurd_plb_redirector = new unisim::kernel::tlm2::BwRedirector<CPU<TYPES, CONFIG> >(
		IF_ICURD_PLB,
		this,
		&CPU<TYPES, CONFIG>::nb_transport_bw,
		&CPU<TYPES, CONFIG>::invalidate_direct_mem_ptr
	);
	icurd_plb_master_sock(*icurd_plb_redirector); // Bind socket to the interface implementer
	
	dcuwr_plb_redirector = new unisim::kernel::tlm2::BwRedirector<CPU<TYPES, CONFIG> >(
		IF_DCUWR_PLB,
		this,
		&CPU<TYPES, CONFIG>::nb_transport_bw,
		&CPU<TYPES, CONFIG>::invalidate_direct_mem_ptr
	);
	dcuwr_plb_master_sock(*dcuwr_plb_redirector); // Bind socket to the interface implementer

	dcurd_plb_redirector = new unisim::kernel::tlm2::BwRedirector<CPU<TYPES, CONFIG> >(
		IF_DCURD_PLB,
		this,
		&CPU<TYPES, CONFIG>::nb_transport_bw,
		&CPU<TYPES, CONFIG>::invalidate_direct_mem_ptr
	);
	dcurd_plb_master_sock(*dcurd_plb_redirector); // Bind socket to the interface implementer
	
	critical_input_interrupt_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<TYPES, CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_CRITICAL_INPUT,
		this, &CPU<TYPES, CONFIG>::interrupt_nb_transport_fw,
		&CPU<TYPES, CONFIG>::interrupt_b_transport,
		&CPU<TYPES, CONFIG>::interrupt_transport_dbg,
		&CPU<TYPES, CONFIG>::interrupt_get_direct_mem_ptr
	);
	critical_input_interrupt_slave_sock(*critical_input_interrupt_redirector); // Bind socket to implementer of interface
	
	external_input_interrupt_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<TYPES, CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_EXTERNAL_INPUT,
		this,
		&CPU<TYPES, CONFIG>::interrupt_nb_transport_fw,
		&CPU<TYPES, CONFIG>::interrupt_b_transport,
		&CPU<TYPES, CONFIG>::interrupt_transport_dbg,
		&CPU<TYPES, CONFIG>::interrupt_get_direct_mem_ptr
	);
	external_input_interrupt_slave_sock(*external_input_interrupt_redirector); // Bind socket to implementer of interface
	
	dcr_redirector = new unisim::kernel::tlm2::BwRedirector<CPU<TYPES, CONFIG> >(
		IF_DCR,
		this,
		&CPU<TYPES, CONFIG>::nb_transport_bw,
		&CPU<TYPES, CONFIG>::invalidate_direct_mem_ptr
	);
	dcr_master_sock(*dcr_redirector);
	
	SC_HAS_PROCESS(CPU);
	
	SC_THREAD(Run);
}

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::~CPU()
{
	if(icurd_plb_redirector)
	{
		delete icurd_plb_redirector;
	}
	
	if(dcuwr_plb_redirector)
	{
		delete dcuwr_plb_redirector;
	}
	
	if(dcurd_plb_redirector)
	{
		delete dcurd_plb_redirector;
	}
	
	if(critical_input_interrupt_redirector)
	{
		delete critical_input_interrupt_redirector;
	}
	
	if(external_input_interrupt_redirector)
	{
		delete external_input_interrupt_redirector;
	}
	
	if(dcr_redirector)
	{
		delete dcr_redirector;
	}
	
	////std::cerr << "total time=" << total_time << std::endl;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::EndSetup()
{
	if(!Super::EndSetup()) return false;
	return true;
}

template <typename TYPES, typename CONFIG>
tlm::tlm_sync_enum CPU<TYPES, CONFIG>::nb_transport_bw(unsigned int if_id, tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == tlm::BEGIN_RESP)
	{
		trans.release();
		return tlm::TLM_COMPLETED;
	}
	return tlm::TLM_ACCEPTED;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::invalidate_direct_mem_ptr(unsigned int if_id, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	switch(if_id)
	{
		case IF_ICURD_PLB:
			icurd_dmi_region_cache.Invalidate(start_range, end_range);
			if(CONFIG::DEBUG_ENABLE && debug_dmi)
			{
				Super::logger << DebugInfo << "PLB Instruction Read: invalidate granted access for 0x" << std::hex << start_range << "-0x" << end_range << std::dec << EndDebugInfo;
			}
			break;
		case IF_DCUWR_PLB:
			dcuwr_dmi_region_cache.Invalidate(start_range, end_range);
			if(CONFIG::DEBUG_ENABLE && debug_dmi)
			{
				Super::logger << DebugInfo << "PLB Data Write: invalidate granted access for 0x" << std::hex << start_range << "-0x" << end_range << std::dec << EndDebugInfo;
			}
			break;
		case IF_DCURD_PLB:
			dcurd_dmi_region_cache.Invalidate(start_range, end_range);
			if(CONFIG::DEBUG_ENABLE && debug_dmi)
			{
				Super::logger << DebugInfo << "PLB Data Read: invalidate granted access for 0x" << std::hex << start_range << "-0x" << end_range << std::dec << EndDebugInfo;
			}
			break;
	}
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::DebugInstructionBusRead(PHYSICAL_ADDRESS physical_addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr)
{
	if(sc_core::sc_get_status() < sc_core::SC_END_OF_ELABORATION)
	{
		// operator -> of ports is not legal before end of elaboration because
		// an implementation of SystemC can defer complete binding just before end of elaboration
		// Using memory service interface instead
		return this->memory_import->ReadMemory(physical_addr, buffer, size);
	}

	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_read();
	payload->set_data_length(size);
	payload->set_streaming_width(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	unsigned int read_size = icurd_plb_master_sock->transport_dbg(*payload);

	return (payload->is_response_ok() && (read_size == size));
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::DebugDataBusRead(PHYSICAL_ADDRESS physical_addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr)
{
	if(sc_core::sc_get_status() < sc_core::SC_END_OF_ELABORATION)
	{
		// operator -> of ports is not legal before end of elaboration because
		// an implementation of SystemC can defer complete binding just before end of elaboration
		// Using memory service interface instead
		return this->memory_import->ReadMemory(physical_addr, buffer, size);
	}

	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_read();
	payload->set_data_length(size);
	payload->set_streaming_width(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	unsigned int read_size = dcurd_plb_master_sock->transport_dbg(*payload);

	return read_size == size;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::DebugDataBusWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr)
{
	if(sc_core::sc_get_status() < sc_core::SC_END_OF_ELABORATION)
	{
		// operator -> of ports is not legal before end of elaboration because
		// an implementation of SystemC can defer complete binding just before end of elaboration
		// Using memory service interface instead
		return this->memory_import->WriteMemory(physical_addr, buffer, size);
	}

	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_write();
	payload->set_data_length(size);
	payload->set_streaming_width(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	unsigned int write_size = dcuwr_plb_master_sock->transport_dbg(*payload);

	return write_size == size;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::Synchronize()
{
	if(sc_core::sc_get_status() >= sc_core::SC_RUNNING)
	{
		wait(cpu_time);
		cpu_time = sc_core::SC_ZERO_TIME;
		run_time = sc_core::sc_time_stamp();
		RunInternalTimers();
	}
}

template <typename TYPES, typename CONFIG>
inline void CPU<TYPES, CONFIG>::ProcessExternalEvents()
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

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessIRQEvent(Event *event)
{
	if(this->verbose_exception)
	{
		Super::logger << DebugInfo << (sc_core::sc_time_stamp() + cpu_time) << ": processing an IRQ event that occured at " << event->GetTimeStamp() << " (";
		switch(event->GetIRQ())
		{
			case Event::IRQ_EXTERNAL_INPUT:
				Super::logger << "external";
				break;
			case Event::IRQ_CRITICAL_INPUT:
				Super::logger << "critical";
				break;
			default:
				Super::logger << "?";
				break;
		}
		Super::logger << " input goes " << (event->GetLevel() ? "high" : "low") << "). Event skew is " << (sc_core::sc_time_stamp() + cpu_time - event->GetTimeStamp()) << "." << EndDebugInfo;
	}
	switch(event->GetIRQ())
	{
		case Event::IRQ_EXTERNAL_INPUT:
			if(event->GetLevel())
			{
				this->template ThrowException<typename Super::ExternalInputInterrupt::ExternalInput>();
			}
			else
			{
				this->template AckException<typename Super::ExternalInputInterrupt::ExternalInput>();
			}
			break;
		case Event::IRQ_CRITICAL_INPUT:
			if(event->GetLevel())
			{
				this->template ThrowException<typename Super::CriticalInputInterrupt::CriticalInput>();
			}
			else
			{
				this->template AckException<typename Super::CriticalInputInterrupt::CriticalInput>();
			}
			break;
	}
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::interrupt_b_transport(unsigned int irq, InterruptPayload& payload, sc_core::sc_time& t)
{
	if(irq < 2)
	{
		bool level = payload.GetValue();
		sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
		notify_time_stamp += t;

		AlignToBusClock(notify_time_stamp);
		if(this->verbose_exception)
		{
			Super::logger << DebugInfo << notify_time_stamp << ": " << (irq ? "External" : "Critical") << " input interrupt signal goes " << (level ? "high" : "low") << EndDebugInfo;
		}
		external_event_schedule.NotifyIRQEvent(irq, level, notify_time_stamp);
		//ev_irq.notify(t);
	}
	else
	{
		Super::logger << DebugError << "protocol error (invalid IRQ number)" << EndDebugError;
		Object::Stop(-1);
	}
}

template <typename TYPES, typename CONFIG>
tlm::tlm_sync_enum CPU<TYPES, CONFIG>::interrupt_nb_transport_fw(unsigned int irq, InterruptPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				if(irq < 2)
				{
					bool level = payload.GetValue();
					sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
					notify_time_stamp += t;

					AlignToBusClock(notify_time_stamp);
					if(this->verbose_exception)
					{
						Super::logger << DebugInfo << notify_time_stamp << ": " << (irq ? "External" : "Critical") << " input interrupt signal goes " << (level ? "high" : "low") << EndDebugInfo;
					}
					external_event_schedule.NotifyIRQEvent(irq, level, notify_time_stamp);
				}
				else
				{
					Super::logger << DebugError << "protocol error (invalid IRQ number)" << EndDebugError;
					Object::Stop(-1);
				}
				
				phase = tlm::END_REQ;
				return tlm::TLM_COMPLETED;
			}
			break;
		case tlm::END_RESP:
			return tlm::TLM_COMPLETED;
		default:
			Super::logger << DebugError << "protocol error" << EndDebugError;
			Object::Stop(-1);
			break;
	}
	return tlm::TLM_COMPLETED;
}

template <typename TYPES, typename CONFIG>
unsigned int CPU<TYPES, CONFIG>::interrupt_transport_dbg(unsigned int, InterruptPayload& trans)
{
	return 0;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::interrupt_get_direct_mem_ptr(unsigned int, InterruptPayload& payload, tlm::tlm_dmi& dmi_data)
{
	return false;
}

template <typename TYPES, typename CONFIG>
inline void CPU<TYPES, CONFIG>::RunInternalTimers()
{
	const sc_core::sc_time& timer_cycle_time = this->ccr1.template Get<typename Super::CCR1::TCS>() ? ext_timer_cycle_time : cpu_cycle_time;
		
#if 0
	if(run_time >= (timer_time + timer_cycle_time))
	{
		do
		{
			Super::RunTimers(1);
			timer_time += timer_cycle_time;
		}
		while(run_time >= (timer_time + timer_cycle_time));
	}
#else
	// Note: this code brings a slight speed improvement (6 %)
#if 0
	sc_time delta_time(run_time);
	delta_time -= timer_time;
	uint64_t delta = (uint64_t) floor(delta_time / timer_cycle_time);
	Super::RunTimers(delta);
	sc_time t(timer_cycle_time);
	t *= (double) delta;
	timer_time += t;
#else
	sc_dt::uint64 delta_time_tu = run_time.value() - timer_time.value();
	sc_dt::uint64 timer_cycle_time_tu = timer_cycle_time.value();
	uint64_t delta = delta_time_tu / timer_cycle_time_tu;
	Super::RunTimers(delta);
	sc_dt::uint64 t_tu = timer_cycle_time_tu * delta;
	//sc_time t(t_tu, false);
	sc_core::sc_time t(sc_core::sc_get_time_resolution());
	t *= t_tu;
	timer_time += t;
#endif
#endif
	timers_update_deadline = timer_time + (Super::GetTimersDeadline() * timer_cycle_time);
}

template <typename TYPES, typename CONFIG>
inline void CPU<TYPES, CONFIG>::LazyRunInternalTimers()
{
	if(unlikely(run_time >= timers_update_deadline))
	{
		RunInternalTimers();
	}
}

template <typename TYPES, typename CONFIG>
inline void CPU<TYPES, CONFIG>::AlignToBusClock()
{
	sc_dt::uint64 bus_cycle_time_tu = bus_cycle_time.value();
	sc_dt::uint64 run_time_tu = run_time.value();
	sc_dt::uint64 modulo = run_time_tu % bus_cycle_time_tu;
	if(!modulo) return; // already aligned
	
	sc_dt::uint64 time_alignment_tu = bus_cycle_time_tu - modulo;
	//sc_time time_alignment(time_alignment_tu, false);
	sc_core::sc_time time_alignment(sc_core::sc_get_time_resolution());
	time_alignment *= time_alignment_tu;
	cpu_time += time_alignment;
	run_time += time_alignment;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::AlignToBusClock(sc_core::sc_time& t)
{
// 	sc_dt::uint64 bus_cycle_time_tu = bus_cycle_time.value();
// 	sc_dt::uint64 time_tu = t.value();
// 	sc_dt::uint64 modulo = time_tu % bus_cycle_time_tu;
// 	if(!modulo) return; // already aligned
// 	
// 	sc_dt::uint64 time_alignment_tu = bus_cycle_time_tu - modulo;
// 	//sc_time time_alignment(time_alignment_tu, false);
// 	sc_time time_alignment(sc_get_time_resolution());
// 	time_alignment *= time_alignment_tu;
// 
// 	t += time_alignment;
	
	sc_core::sc_time modulo(t);
	modulo %= bus_cycle_time;
	if(modulo == sc_core::SC_ZERO_TIME) return; // already aligned
	t += bus_cycle_time - modulo;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::Idle()
{
	// This is called within thread Run()
	
	// First synchronize with other threads
	Synchronize();
	
	if(!this->HasPendingInterrupts()) // Is there some pending interrupts?
	{
		// Compute the time to consume before an internal timer event occurs
		const sc_core::sc_time& timer_cycle_time = this->ccr1.template Get<typename Super::CCR1::TCS>() ? ext_timer_cycle_time : cpu_cycle_time;
		max_idle_time = timer_cycle_time;
		max_idle_time *= Super::GetMaxIdleTime(); // max idle time (timer_time based)

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
		wait(ev_max_idle | external_event_schedule.GetKernelEvent()/*ev_irq*/);
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
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::Halt()
{
	this->Synchronize();
	wait(sc_core::sc_time(10.0, sc_core::SC_MS));
	this->Stop(0);
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::Run()
{
	sc_core::sc_time time_per_instruction = cpu_cycle_time / ipc;
	
	while(1)
	{
		Super::StepOneInstruction();
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

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::InstructionBusRead(PHYSICAL_ADDRESS physical_addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr)
{
	AlignToBusClock();

	unisim::kernel::tlm2::DMIRegion *dmi_region = 0;
	
	if(likely(enable_dmi))
	{
		dmi_region = icurd_dmi_region_cache.Lookup(physical_addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Instruction Read: DMI region cache hit for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Instruction Read: DMI access allowed for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_READ) == tlm::tlm_dmi::DMI_ACCESS_READ))
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Instruction Read: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(buffer, dmi_data->get_dmi_ptr() + (physical_addr - dmi_data->get_start_address()), size);
					const sc_core::sc_time& read_lat = dmi_region->GetReadLatency(size);
					cpu_time += read_lat;
					run_time += read_lat;
					LazyRunInternalTimers();
					return true;
				}
				else
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Instruction Read: granted DMI access does not allow direct read access for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Instruction Read: DMI access denied for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Instruction Read: DMI region cache miss for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}
	
	if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Instruction Read: traditional way for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_READ_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	icurd_plb_master_sock->b_transport(*payload, cpu_time);
	
	run_time = sc_core::sc_time_stamp();
	run_time += cpu_time;
	LazyRunInternalTimers();
	
	tlm::tlm_response_status status = payload->get_response_status();
	
	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Instruction Read: target allows DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			
			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			unisim::kernel::tlm2::DMIGrant dmi_grant = icurd_plb_master_sock->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			icurd_dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Instruction Read: target does not allow DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}
	
	payload->release();
	
	if(unlikely(status != tlm::TLM_OK_RESPONSE))
	{
		if(this->verbose_exception)
		{
			this->logger << DebugInfo << "Instruction Read Bus Error at @0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
		this->template ThrowException<typename MachineCheckInterrupt::InstructionSynchronousMachineCheck>().SetEvent(MachineCheckInterrupt::MCE_INSTRUCTION_PLB_ERROR);
		return false;
	}
	
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::DataBusRead(PHYSICAL_ADDRESS physical_addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr, bool rwitm)
{
	AlignToBusClock();
	
	unisim::kernel::tlm2::DMIRegion *dmi_region = 0;
	
	if(likely(enable_dmi))
	{
		dmi_region = dcurd_dmi_region_cache.Lookup(physical_addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Read: DMI region cache hit for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Read: DMI access allowed for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_READ) == tlm::tlm_dmi::DMI_ACCESS_READ))
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Read: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(buffer, dmi_data->get_dmi_ptr() + (physical_addr - dmi_data->get_start_address()), size);
					const sc_core::sc_time& read_lat = dmi_region->GetReadLatency(size);
					cpu_time += read_lat;
					run_time += read_lat;
					LazyRunInternalTimers();
					return true;
				}
				else
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Read: granted DMI access does not allow direct read access for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Read: DMI access denied for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Read: DMI region cache miss for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Read: traditional way for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_READ_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	dcurd_plb_master_sock->b_transport(*payload, cpu_time);
	
	run_time = sc_core::sc_time_stamp();
	run_time += cpu_time;
	LazyRunInternalTimers();

	tlm::tlm_response_status status = payload->get_response_status();
	
	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Read: target allows DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;

			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			unisim::kernel::tlm2::DMIGrant dmi_grant = dcurd_plb_master_sock->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			dcurd_dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Read: target does not allow DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	payload->release();

	if(unlikely(status != tlm::TLM_OK_RESPONSE))
	{
		if(this->verbose_exception)
		{
			this->logger << DebugInfo << "Data Read Bus Error at @0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
		this->template ThrowException<typename MachineCheckInterrupt::DataAsynchronousMachineCheck>().SetEvent(MachineCheckInterrupt::MCE_DATA_READ_PLB_ERROR);
		return false;
	}
	
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::DataBusWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr)
{
	AlignToBusClock();
	
	unisim::kernel::tlm2::DMIRegion *dmi_region = 0;

	if(likely(enable_dmi))
	{
		dmi_region = dcuwr_dmi_region_cache.Lookup(physical_addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Write: DMI region cache hit for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Write: DMI access allowed for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_WRITE) == tlm::tlm_dmi::DMI_ACCESS_WRITE))
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Write: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(dmi_data->get_dmi_ptr() + (physical_addr - dmi_data->get_start_address()), buffer, size);
					const sc_core::sc_time& write_lat = dmi_region->GetWriteLatency(size);
					cpu_time += write_lat;
					run_time += write_lat;
					LazyRunInternalTimers();
					return true;
				}
				else
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Write: granted DMI access does not allow direct write access for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Write: DMI access denied for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Write: DMI region cache miss for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Write: traditional way for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_WRITE_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	dcuwr_plb_master_sock->b_transport(*payload, cpu_time);
	
	run_time = sc_core::sc_time_stamp();
	run_time += cpu_time;
	LazyRunInternalTimers();

	tlm::tlm_response_status status = payload->get_response_status();

	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Write: target allows DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			unisim::kernel::tlm2::DMIGrant dmi_grant = icurd_plb_master_sock->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			dcuwr_dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "PLB Data Write: target does not allow DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	payload->release();

	if(unlikely(status != tlm::TLM_OK_RESPONSE))
	{
		if(this->verbose_exception)
		{
			this->logger << DebugInfo << "Data Write Bus Error at @0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
		this->template ThrowException<typename MachineCheckInterrupt::DataAsynchronousMachineCheck>().SetEvent(MachineCheckInterrupt::MCE_DATA_WRITE_PLB_ERROR);
		return false;
	}
	
	return true;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::DCRRead(unsigned int dcrn, uint32_t& value)
{
	AlignToBusClock();
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	uint32_t buffer = 0;
	
	payload->set_address(4 * dcrn);
	payload->set_command(tlm::TLM_READ_COMMAND);
	payload->set_data_length(4);
	payload->set_data_ptr((unsigned char *) &buffer);
	
	dcr_master_sock->b_transport(*payload, cpu_time);
	
	run_time = sc_core::sc_time_stamp();
	run_time += cpu_time;
	LazyRunInternalTimers();

	/* tlm::tlm_response_status status = */ payload->get_response_status();
	
	payload->release();

	value = unisim::util::endian::BigEndian2Host(buffer);
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::DCRWrite(unsigned int dcrn, uint32_t value)
{
	uint32_t buffer = unisim::util::endian::Host2BigEndian(value);
	
	AlignToBusClock();
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(4 * dcrn);
	payload->set_command(tlm::TLM_WRITE_COMMAND);
	payload->set_data_length(4);
	payload->set_data_ptr((unsigned char *) &buffer);
	
	dcr_master_sock->b_transport(*payload, cpu_time);
	
	run_time = sc_core::sc_time_stamp();
	run_time += cpu_time;
	LazyRunInternalTimers();

	/* tlm::tlm_response_status status = */ payload->get_response_status();

	payload->release();

}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::end_of_simulation()
{
	RunInternalTimers(); // make run_time match timer_time
}

} // end of namespace book_e
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
