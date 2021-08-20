/*
 *  Copyright (c) 2007-2010,
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

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_TMS320C3X_CPU_TCC__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_TMS320C3X_CPU_TCC__

#include <unistd.h>

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace tms320c3x {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG, bool DEBUG>
CPU<CONFIG, DEBUG>::CPU(const sc_core::sc_module_name& name, Object *parent)
	: Object(name, parent, "this module implements a TMS320C3X DSP core")
	, sc_core::sc_module(name)
	, unisim::component::cxx::processor::tms320c3x::CPU<CONFIG, DEBUG>(name, parent)
	, bus_master_sock("bus-master-sock")
	, int0_slave_sock("int0-slave-sock")
	, int1_slave_sock("int1-slave-sock")
	, int2_slave_sock("int2-slave-sock")
	, int3_slave_sock("int3-slave-sock")
	, xint0_slave_sock("xint0-slave-sock")
	, rint0_slave_sock("rint0-slave-sock")
	, xint1_slave_sock("xint1-slave-sock")
	, rint1_slave_sock("rint1-slave-sock")
	, tint0_slave_sock("tint0-slave-sock")
	, tint1_slave_sock("tint1-slave-sock")
	, dint_slave_sock("dint-slave-sock")
	, payload_fabric()
	, cpu_cycle_time()
	, cpu_time()
	, nice_time()
	, ev_irq()
	, ipc(1.0)
	, enable_dmi(false)
	, debug_dmi(false)
	, param_cpu_cycle_time("cpu-cycle-time", this, cpu_cycle_time, "cpu cycle time")
	, param_nice_time("nice-time", this, nice_time, "maximum time between synchonizations")
	, param_ipc("ipc", this, ipc, "targeted average instructions per second")
	, param_enable_dmi("enable-dmi", this, enable_dmi, "Enable/Disable TLM 2.0 DMI (Direct Memory Access) to speed-up simulation")
	, param_debug_dmi("debug-dmi", this, debug_dmi, "Enable/Disable debugging of DMI (Direct Memory Access)")
	, external_event_schedule()
	, int0_redirector(0)
	, int1_redirector(0)
	, int2_redirector(0)
	, int3_redirector(0)
	, xint0_redirector(0)
	, rint0_redirector(0)
	, xint1_redirector(0)
	, rint1_redirector(0)
	, tint0_redirector(0)
	, tint1_redirector(0)
	, dint_redirector(0)
	, bus_master_redirector(0)
	, dmi_region_cache()
{
	bus_master_redirector = new unisim::kernel::tlm2::BwRedirector<CPU<CONFIG, DEBUG> >(
		0,
		this,
		&CPU<CONFIG, DEBUG>::nb_transport_bw,
		&CPU<CONFIG, DEBUG>::invalidate_direct_mem_ptr
	);
	bus_master_sock(*bus_master_redirector); // Bind socket to the interface implementer
	
	int0_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_INT0,
		this, &CPU<CONFIG, DEBUG>::interrupt_nb_transport_fw,
		&CPU<CONFIG, DEBUG>::interrupt_b_transport,
		&CPU<CONFIG, DEBUG>::interrupt_transport_dbg,
		&CPU<CONFIG, DEBUG>::interrupt_get_direct_mem_ptr
	);
	int0_slave_sock(*int0_redirector); // Bind socket to implementer of interface

	int1_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_INT1,
		this, &CPU<CONFIG, DEBUG>::interrupt_nb_transport_fw,
		&CPU<CONFIG, DEBUG>::interrupt_b_transport,
		&CPU<CONFIG, DEBUG>::interrupt_transport_dbg,
		&CPU<CONFIG, DEBUG>::interrupt_get_direct_mem_ptr
	);
	int1_slave_sock(*int1_redirector); // Bind socket to implementer of interface

	int2_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_INT2,
		this, &CPU<CONFIG, DEBUG>::interrupt_nb_transport_fw,
		&CPU<CONFIG, DEBUG>::interrupt_b_transport,
		&CPU<CONFIG, DEBUG>::interrupt_transport_dbg,
		&CPU<CONFIG, DEBUG>::interrupt_get_direct_mem_ptr
	);
	int2_slave_sock(*int2_redirector); // Bind socket to implementer of interface
	
	int3_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_INT3,
		this, &CPU<CONFIG, DEBUG>::interrupt_nb_transport_fw,
		&CPU<CONFIG, DEBUG>::interrupt_b_transport,
		&CPU<CONFIG, DEBUG>::interrupt_transport_dbg,
		&CPU<CONFIG, DEBUG>::interrupt_get_direct_mem_ptr
	);
	int3_slave_sock(*int3_redirector); // Bind socket to implementer of interface

	xint0_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_XINT0,
		this, &CPU<CONFIG, DEBUG>::interrupt_nb_transport_fw,
		&CPU<CONFIG, DEBUG>::interrupt_b_transport,
		&CPU<CONFIG, DEBUG>::interrupt_transport_dbg,
		&CPU<CONFIG, DEBUG>::interrupt_get_direct_mem_ptr
	);
	xint0_slave_sock(*xint0_redirector); // Bind socket to implementer of interface

	rint0_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_RINT0,
		this, &CPU<CONFIG, DEBUG>::interrupt_nb_transport_fw,
		&CPU<CONFIG, DEBUG>::interrupt_b_transport,
		&CPU<CONFIG, DEBUG>::interrupt_transport_dbg,
		&CPU<CONFIG, DEBUG>::interrupt_get_direct_mem_ptr
	);
	rint0_slave_sock(*rint0_redirector); // Bind socket to implementer of interface

	xint1_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_XINT1,
		this, &CPU<CONFIG, DEBUG>::interrupt_nb_transport_fw,
		&CPU<CONFIG, DEBUG>::interrupt_b_transport,
		&CPU<CONFIG, DEBUG>::interrupt_transport_dbg,
		&CPU<CONFIG, DEBUG>::interrupt_get_direct_mem_ptr
	);
	xint1_slave_sock(*xint1_redirector); // Bind socket to implementer of interface

	rint1_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_RINT1,
		this, &CPU<CONFIG, DEBUG>::interrupt_nb_transport_fw,
		&CPU<CONFIG, DEBUG>::interrupt_b_transport,
		&CPU<CONFIG, DEBUG>::interrupt_transport_dbg,
		&CPU<CONFIG, DEBUG>::interrupt_get_direct_mem_ptr
	);
	rint1_slave_sock(*rint1_redirector); // Bind socket to implementer of interface

	tint0_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_TINT0,
		this, &CPU<CONFIG, DEBUG>::interrupt_nb_transport_fw,
		&CPU<CONFIG, DEBUG>::interrupt_b_transport,
		&CPU<CONFIG, DEBUG>::interrupt_transport_dbg,
		&CPU<CONFIG, DEBUG>::interrupt_get_direct_mem_ptr
	);
	tint0_slave_sock(*tint0_redirector); // Bind socket to implementer of interface

	tint1_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_TINT1,
		this, &CPU<CONFIG, DEBUG>::interrupt_nb_transport_fw,
		&CPU<CONFIG, DEBUG>::interrupt_b_transport,
		&CPU<CONFIG, DEBUG>::interrupt_transport_dbg,
		&CPU<CONFIG, DEBUG>::interrupt_get_direct_mem_ptr
	);
	tint1_slave_sock(*tint1_redirector); // Bind socket to implementer of interface

	dint_redirector = new unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
		Event::IRQ_DINT,
		this, &CPU<CONFIG, DEBUG>::interrupt_nb_transport_fw,
		&CPU<CONFIG, DEBUG>::interrupt_b_transport,
		&CPU<CONFIG, DEBUG>::interrupt_transport_dbg,
		&CPU<CONFIG, DEBUG>::interrupt_get_direct_mem_ptr
	);
	dint_slave_sock(*dint_redirector); // Bind socket to implementer of interface

	SC_HAS_PROCESS(CPU);
	
	SC_THREAD(Run);
}

template <class CONFIG, bool DEBUG>
CPU<CONFIG, DEBUG>::~CPU()
{
	if(bus_master_redirector) delete bus_master_redirector;
	if(int0_redirector) delete int0_redirector;
	if(int1_redirector) delete int1_redirector;
	if(int2_redirector) delete int2_redirector;
	if(int3_redirector) delete int3_redirector;
	if(xint0_redirector) delete xint0_redirector;
	if(rint0_redirector) delete rint0_redirector;
	if(xint1_redirector) delete xint1_redirector;
	if(rint1_redirector) delete rint1_redirector;
	if(tint0_redirector) delete tint0_redirector;
	if(tint1_redirector) delete tint1_redirector;
	if(dint_redirector) delete dint_redirector;
}

template <class CONFIG, bool DEBUG>
tlm::tlm_sync_enum CPU<CONFIG, DEBUG>::nb_transport_bw(unsigned int if_id, tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == tlm::BEGIN_RESP)
	{
		trans.release();
		return tlm::TLM_COMPLETED;
	}
	return tlm::TLM_ACCEPTED;
}

template <class CONFIG, bool DEBUG>
void CPU<CONFIG, DEBUG>::invalidate_direct_mem_ptr(unsigned int if_id, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	dmi_region_cache.Invalidate(start_range, end_range);
}

template <class CONFIG, bool DEBUG>
void CPU<CONFIG, DEBUG>::Synchronize()
{
	wait(cpu_time);
	cpu_time = sc_core::SC_ZERO_TIME;
}

template <class CONFIG, bool DEBUG>
void CPU<CONFIG, DEBUG>::ProcessExternalEvents()
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

template <class CONFIG, bool DEBUG>
void CPU<CONFIG, DEBUG>::ProcessIRQEvent(Event *event)
{
	if(inherited::VerboseAll())
	{
		inherited::logger << DebugInfo << (sc_core::sc_time_stamp() + cpu_time) << ": processing an IRQ event that occured at " << event->GetTimeStamp() << " (IRQ #" << event->GetIRQ() << " goes " << (event->GetLevel() ? "high" : "low") << "). Event skew is " << (sc_core::sc_time_stamp() + cpu_time - event->GetTimeStamp()) << "." << EndDebugInfo;
	}
	inherited::SetIRQLevel(event->GetIRQ(), event->GetLevel());
}

template <class CONFIG, bool DEBUG>
void CPU<CONFIG, DEBUG>::interrupt_b_transport(unsigned int irq, InterruptPayload& payload, sc_core::sc_time& t)
{
	if(irq < 2)
	{
		bool level = payload.GetValue();
		sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
		notify_time_stamp += t;

		if(inherited::VerboseAll())
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

template <class CONFIG, bool DEBUG>
tlm::tlm_sync_enum CPU<CONFIG, DEBUG>::interrupt_nb_transport_fw(unsigned int irq, InterruptPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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

					if(inherited::VerboseAll())
					{
						inherited::logger << DebugInfo << notify_time_stamp << ": IRQ #" << irq << " input interrupt signal goes " << (level ? "high" : "low") << EndDebugInfo;
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

template <class CONFIG, bool DEBUG>
unsigned int CPU<CONFIG, DEBUG>::interrupt_transport_dbg(unsigned int, InterruptPayload& trans)
{
	return 0;
}

template <class CONFIG, bool DEBUG>
bool CPU<CONFIG, DEBUG>::interrupt_get_direct_mem_ptr(unsigned int, InterruptPayload& payload, tlm::tlm_dmi& dmi_data)
{
	return false;
}

template <class CONFIG, bool DEBUG>
void CPU<CONFIG, DEBUG>::Run()
{
	sc_core::sc_time time_per_instruction = cpu_cycle_time * ipc;
	
	while(1)
	{
		inherited::StepInstruction();
		cpu_time += time_per_instruction;
		ProcessExternalEvents();
		if(unlikely(cpu_time >= nice_time))
		{
			Synchronize();
		}
	}
}

template <class CONFIG, bool DEBUG>
bool CPU<CONFIG, DEBUG>::PrRead(typename CONFIG::address_t addr, void *buffer, uint32_t size, bool interlocked)
{
	unisim::kernel::tlm2::DMIRegion *dmi_region = 0;
	
	if(likely(enable_dmi))
	{
		dmi_region = dmi_region_cache.Lookup(addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Read: DMI region cache hit for 0x" << std::hex << addr << std::dec << EndDebugInfo;
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Read: DMI access allowed for 0x" << std::hex << addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_READ) == tlm::tlm_dmi::DMI_ACCESS_READ))
				{
					if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Read: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(buffer, dmi_data->get_dmi_ptr() + (addr - dmi_data->get_start_address()), size);
					cpu_time += dmi_region->GetReadLatency(size);
					return true;
				}
				else
				{
					if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Read: granted DMI access does not allow direct read access for 0x" << std::hex << addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Read: DMI access denied for 0x" << std::hex << addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Read: DMI region cache miss for 0x" << std::hex << addr << std::dec << EndDebugInfo;
		}
	}

	if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Read: traditional way for 0x" << std::hex << addr << std::dec << EndDebugInfo;
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(addr);
	payload->set_command(tlm::TLM_READ_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	bus_master_sock->b_transport(*payload, cpu_time);
	
	tlm::tlm_response_status status = payload->get_response_status();
	
	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Read: target allows DMI for 0x" << std::hex << addr << std::dec << EndDebugInfo;

			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			unisim::kernel::tlm2::DMIGrant dmi_grant = bus_master_sock->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Read: target does not allow DMI for 0x" << std::hex << addr << std::dec << EndDebugInfo;
		}
	}

	payload->release();

	return status == tlm::TLM_OK_RESPONSE;
}

template <class CONFIG, bool DEBUG>
bool CPU<CONFIG, DEBUG>::PrWrite(typename CONFIG::address_t addr, const void *buffer, uint32_t size, bool interlocked)
{
	unisim::kernel::tlm2::DMIRegion *dmi_region = 0;
	
	if(likely(enable_dmi))
	{
		dmi_region = dmi_region_cache.Lookup(addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Write: DMI region cache hit for 0x" << std::hex << addr << std::dec << EndDebugInfo;
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Write: DMI access allowed for 0x" << std::hex << addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_WRITE) == tlm::tlm_dmi::DMI_ACCESS_WRITE))
				{
					if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Write: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(dmi_data->get_dmi_ptr() + (addr - dmi_data->get_start_address()), buffer, size);
					cpu_time += dmi_region->GetWriteLatency(size);
					return true;
				}
				else
				{
					if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Write: granted DMI access does not allow direct write access for 0x" << std::hex << addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Write: DMI access denied for 0x" << std::hex << addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Write: DMI region cache miss for 0x" << std::hex << addr << std::dec << EndDebugInfo;
		}
	}

	if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Write: traditional way for 0x" << std::hex << addr << std::dec << EndDebugInfo;
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(addr);
	payload->set_command(tlm::TLM_WRITE_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	bus_master_sock->b_transport(*payload, cpu_time);
	
	tlm::tlm_response_status status = payload->get_response_status();
	
	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Write: target allows DMI for 0x" << std::hex << addr << std::dec << EndDebugInfo;

			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			unisim::kernel::tlm2::DMIGrant dmi_grant = bus_master_sock->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(DEBUG && debug_dmi)) inherited::logger << DebugInfo << "Bus Write: target does not allow DMI for 0x" << std::hex << addr << std::dec << EndDebugInfo;
		}
	}

	payload->release();
	return status == tlm::TLM_OK_RESPONSE;
}

} // end of namespace tms320c3x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
