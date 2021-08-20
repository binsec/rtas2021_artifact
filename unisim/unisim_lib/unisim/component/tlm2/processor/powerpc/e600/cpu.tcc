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

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_E600_CPU_TCC__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_E600_CPU_TCC__

#include <unistd.h>
#include <unisim/component/cxx/processor/powerpc/e600/cpu.tcc>

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace powerpc {
namespace e600 {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::CPU(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent, "e600 PowerPC core")
	, sc_core::sc_module(name)
	, Super(name, parent)
	, bus_master_sock("bus-master-sock")
	, external_interrupt_slave_sock("external-interrupt-slave-sock")
	, hard_reset_slave_sock("hard-reset-slave-sock")
	, soft_reset_slave_sock("soft-reset-slave-sock")
	, mcp_slave_sock("mcp-slave-sock")
	, smi_slave_sock("smi-slave-sock")
	, payload_fabric()
	, cpu_cycle_time()
	, bus_cycle_time()
	, timer_cycle_time()
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
	, external_interrupt_redirector(0)
	, hard_reset_redirector(0)
	, soft_reset_redirector(0)
	, mcp_redirector(0)
	, smi_redirector(0)
	, bus_master_redirector(0)
	, bus_master_dmi_region_cache()
{
	stat_one.SetMutable(false);
	stat_one.SetSerializable(false);
	stat_one.SetVisible(false);
	
	bus_master_redirector = new unisim::kernel::tlm2::BwRedirector<CPU<TYPES, CONFIG> >(
		IF_BUS_MASTER,
		this,
		&CPU<TYPES, CONFIG>::nb_transport_bw,
		&CPU<TYPES, CONFIG>::invalidate_direct_mem_ptr
	);
	bus_master_sock(*bus_master_redirector); // Bind socket to the interface implementer
	
	external_interrupt_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<TYPES, CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_EXTERNAL_INTERRUPT,
		this,
		&CPU<TYPES, CONFIG>::interrupt_nb_transport_fw,
		&CPU<TYPES, CONFIG>::interrupt_b_transport,
		&CPU<TYPES, CONFIG>::interrupt_transport_dbg,
		&CPU<TYPES, CONFIG>::interrupt_get_direct_mem_ptr
	);
	external_interrupt_slave_sock(*external_interrupt_redirector); // Bind socket to implementer of interface
	
	hard_reset_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<TYPES, CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_HARD_RESET,
		this,
		&CPU<TYPES, CONFIG>::interrupt_nb_transport_fw,
		&CPU<TYPES, CONFIG>::interrupt_b_transport,
		&CPU<TYPES, CONFIG>::interrupt_transport_dbg,
		&CPU<TYPES, CONFIG>::interrupt_get_direct_mem_ptr
	);
	hard_reset_slave_sock(*hard_reset_redirector); // Bind socket to implementer of interface

	soft_reset_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<TYPES, CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_SOFT_RESET,
		this,
		&CPU<TYPES, CONFIG>::interrupt_nb_transport_fw,
		&CPU<TYPES, CONFIG>::interrupt_b_transport,
		&CPU<TYPES, CONFIG>::interrupt_transport_dbg,
		&CPU<TYPES, CONFIG>::interrupt_get_direct_mem_ptr
	);
	soft_reset_slave_sock(*soft_reset_redirector); // Bind socket to implementer of interface

	mcp_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<TYPES, CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_MCP,
		this,
		&CPU<TYPES, CONFIG>::interrupt_nb_transport_fw,
		&CPU<TYPES, CONFIG>::interrupt_b_transport,
		&CPU<TYPES, CONFIG>::interrupt_transport_dbg,
		&CPU<TYPES, CONFIG>::interrupt_get_direct_mem_ptr
	);
	mcp_slave_sock(*mcp_redirector); // Bind socket to implementer of interface

	smi_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<TYPES, CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_SMI,
		this,
		&CPU<TYPES, CONFIG>::interrupt_nb_transport_fw,
		&CPU<TYPES, CONFIG>::interrupt_b_transport,
		&CPU<TYPES, CONFIG>::interrupt_transport_dbg,
		&CPU<TYPES, CONFIG>::interrupt_get_direct_mem_ptr
	);
	smi_slave_sock(*smi_redirector); // Bind socket to implementer of interface

	SC_HAS_PROCESS(CPU);
	
	SC_THREAD(Run);
}

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::~CPU()
{
	if(external_interrupt_redirector)
	{
		delete external_interrupt_redirector;
	}
	
	if(hard_reset_redirector)
	{
		delete hard_reset_redirector;
	}
	
	if(soft_reset_redirector)
	{
		delete soft_reset_redirector;
	}
	
	if(mcp_redirector)
	{
		delete mcp_redirector;
	}
	
	if(smi_redirector)
	{
		delete smi_redirector;
	}
	
	////std::cerr << "total time=" << total_time << std::endl;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::EndSetup()
{
	// Timer cycle time = 4 * Bus cycle time
	timer_cycle_time = bus_cycle_time;
	timer_cycle_time += timer_cycle_time;
	timer_cycle_time += timer_cycle_time;
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
		case IF_BUS_MASTER:
			bus_master_dmi_region_cache.Invalidate(start_range, end_range);
			if(CONFIG::DEBUG_ENABLE && debug_dmi)
			{
				Super::logger << DebugInfo << "Bus Master: invalidate granted access for 0x" << std::hex << start_range << "-0x" << end_range << std::dec << EndDebugInfo;
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
	
	unsigned int read_size = bus_master_sock->transport_dbg(*payload);

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
	
	unsigned int read_size = bus_master_sock->transport_dbg(*payload);

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
	
	unsigned int write_size = bus_master_sock->transport_dbg(*payload);

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
			case Event::IRQ_EXTERNAL_INTERRUPT:
				Super::logger << "external interrupt";
				break;
			case Event::IRQ_HARD_RESET:
				Super::logger << "hard reset";
				break;
			case Event::IRQ_SOFT_RESET:
				Super::logger << "soft reset";
				break;
			case Event::IRQ_MCP:
				Super::logger << "MCP";
				break;
			case Event::IRQ_SMI:
				Super::logger << "SMI";
				break;
			default:
				Super::logger << "?";
				break;
		}
		Super::logger << " input goes " << (event->GetLevel() ? "high" : "low") << "). Event skew is " << (sc_core::sc_time_stamp() + cpu_time - event->GetTimeStamp()) << "." << EndDebugInfo;
	}
	switch(event->GetIRQ())
	{
		case Event::IRQ_EXTERNAL_INTERRUPT:
			if(event->GetLevel())
			{
				this->template ThrowException<typename ExternalInterrupt::ExternalInterruptException>();
			}
			else
			{
				this->template AckException<typename ExternalInterrupt::ExternalInterruptException>();
			}
			break;
		case Event::IRQ_HARD_RESET:
			if(event->GetLevel())
			{
				this->template ThrowException<typename SystemResetInterrupt::HardReset>();
			}
			else
			{
				this->template AckException<typename SystemResetInterrupt::HardReset>();
			}
			break;
		case Event::IRQ_SOFT_RESET:
			if(event->GetLevel())
			{
				this->template ThrowException<typename SystemResetInterrupt::SoftReset>();
			}
			else
			{
				this->template AckException<typename SystemResetInterrupt::SoftReset>();
			}
			break;
		case Event::IRQ_MCP:
			if(event->GetLevel())
			{
				this->template ThrowException<typename MachineCheckInterrupt::MCP>();
			}
			else
			{
				this->template AckException<typename MachineCheckInterrupt::MCP>();
			}
			break;
		case Event::IRQ_SMI:
			if(event->GetLevel())
			{
				this->template ThrowException<typename SystemManagementInterrupt::SMI>();
			}
			else
			{
				this->template AckException<typename SystemManagementInterrupt::SMI>();
			}
			break;
	}
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::interrupt_b_transport(unsigned int irq, InterruptPayload& payload, sc_core::sc_time& t)
{
	if(irq < Event::NUM_IRQS)
	{
		bool level = payload.GetValue();
		sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
		notify_time_stamp += t;

		AlignToBusClock(notify_time_stamp);
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
				if(irq < Event::NUM_IRQS)
				{
					bool level = payload.GetValue();
					sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
					notify_time_stamp += t;

					AlignToBusClock(notify_time_stamp);
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
	sc_dt::uint64 delta_time_tu = run_time.value() - timer_time.value();
	sc_dt::uint64 timer_cycle_time_tu = timer_cycle_time.value();
	uint64_t delta = delta_time_tu / timer_cycle_time_tu;
	Super::RunTimers(delta);
	sc_dt::uint64 t_tu = timer_cycle_time_tu * delta;
	sc_core::sc_time t(sc_core::sc_get_time_resolution());
	t *= t_tu;
	timer_time += t;
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
		dmi_region = bus_master_dmi_region_cache.Lookup(physical_addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Instruction Read: DMI region cache hit for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Instruction Read: DMI access allowed for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_READ) == tlm::tlm_dmi::DMI_ACCESS_READ))
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Instruction Read: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(buffer, dmi_data->get_dmi_ptr() + (physical_addr - dmi_data->get_start_address()), size);
					const sc_core::sc_time& read_lat = dmi_region->GetReadLatency(size);
					cpu_time += read_lat;
					run_time += read_lat;
					LazyRunInternalTimers();
					return true;
				}
				else
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Instruction Read: granted DMI access does not allow direct read access for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Instruction Read: DMI access denied for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Instruction Read: DMI region cache miss for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}
	
	if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Instruction Read: traditional way for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_READ_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	bus_master_sock->b_transport(*payload, cpu_time);
	
	run_time = sc_core::sc_time_stamp();
	run_time += cpu_time;
	LazyRunInternalTimers();
	
	tlm::tlm_response_status status = payload->get_response_status();
	
	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Instruction Read: target allows DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			
			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			unisim::kernel::tlm2::DMIGrant dmi_grant = bus_master_sock->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			bus_master_dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Instruction Read: target does not allow DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}
	
	payload->release();
	
	if(unlikely(status != tlm::TLM_OK_RESPONSE))
	{
		if(this->verbose_exception)
		{
			this->logger << DebugInfo << "Instruction Read Bus Error at @0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
		this->template ThrowException<typename MachineCheckInterrupt::TEA>();
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
		dmi_region = bus_master_dmi_region_cache.Lookup(physical_addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Read: DMI region cache hit for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Read: DMI access allowed for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_READ) == tlm::tlm_dmi::DMI_ACCESS_READ))
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Read: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(buffer, dmi_data->get_dmi_ptr() + (physical_addr - dmi_data->get_start_address()), size);
					const sc_core::sc_time& read_lat = dmi_region->GetReadLatency(size);
					cpu_time += read_lat;
					run_time += read_lat;
					LazyRunInternalTimers();
					return true;
				}
				else
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Read: granted DMI access does not allow direct read access for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Read: DMI access denied for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Read: DMI region cache miss for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Read: traditional way for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_READ_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	bus_master_sock->b_transport(*payload, cpu_time);
	
	run_time = sc_core::sc_time_stamp();
	run_time += cpu_time;
	LazyRunInternalTimers();

	tlm::tlm_response_status status = payload->get_response_status();
	
	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Read: target allows DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;

			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			unisim::kernel::tlm2::DMIGrant dmi_grant = bus_master_sock->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			bus_master_dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Read: target does not allow DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	payload->release();

	if(unlikely(status != tlm::TLM_OK_RESPONSE))
	{
		if(this->verbose_exception)
		{
			this->logger << DebugInfo << "Data Read Bus Error at @0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
		this->template ThrowException<typename MachineCheckInterrupt::TEA>();
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
		dmi_region = bus_master_dmi_region_cache.Lookup(physical_addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Write: DMI region cache hit for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Write: DMI access allowed for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_WRITE) == tlm::tlm_dmi::DMI_ACCESS_WRITE))
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Write: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(dmi_data->get_dmi_ptr() + (physical_addr - dmi_data->get_start_address()), buffer, size);
					const sc_core::sc_time& write_lat = dmi_region->GetWriteLatency(size);
					cpu_time += write_lat;
					run_time += write_lat;
					LazyRunInternalTimers();
					return true;
				}
				else
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Write: granted DMI access does not allow direct write access for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Write: DMI access denied for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Write: DMI region cache miss for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Write: traditional way for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_WRITE_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	bus_master_sock->b_transport(*payload, cpu_time);
	
	run_time = sc_core::sc_time_stamp();
	run_time += cpu_time;
	LazyRunInternalTimers();

	tlm::tlm_response_status status = payload->get_response_status();

	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Write: target allows DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			unisim::kernel::tlm2::DMIGrant dmi_grant = bus_master_sock->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			bus_master_dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "Bus Data Write: target does not allow DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	payload->release();

	if(unlikely(status != tlm::TLM_OK_RESPONSE))
	{
		if(this->verbose_exception)
		{
			this->logger << DebugInfo << "Data Write Bus Error at @0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
		this->template ThrowException<typename MachineCheckInterrupt::TEA>();
		return false;
	}
	
	return true;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::end_of_simulation()
{
	RunInternalTimers(); // make run_time match timer_time
}

} // end of namespace e600
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
