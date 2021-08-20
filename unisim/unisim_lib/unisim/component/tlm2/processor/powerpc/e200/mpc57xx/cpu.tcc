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

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_E200_MPC57XX_CPU_TCC__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_E200_MPC57XX_CPU_TCC__

#include <unisim/component/tlm2/processor/powerpc/e200/mpc57xx/cpu.hh>
#include <unisim/kernel/tlm2/trans_attr.hh>
#include <unistd.h>

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace powerpc {
namespace e200 {
namespace mpc57xx {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::CPU(const sc_core::sc_module_name& name, Object *parent)
	: Object(name, parent, "this module implements a e200 CPU core of MPC57XX SoC")
	, sc_core::sc_module(name)
	, Super(name, parent)
	, i_ahb_if("i_ahb_if")
	, d_ahb_if("d_ahb_if")
	, s_ahb_if("s_ahb_if")
	, m_clk("m_clk")
	, m_por("m_por")
	, p_reset_b("p_reset_b")
	, p_nmi_b("p_nmi_b")
	, p_mcp_b("p_mcp_b")
	, p_rstbase("p_rstbase")
	, p_cpuid("p_cpuid")
	, p_extint_b("p_extint_b")
	, p_crint_b("p_crint_b")
	, p_avec_b("p_avec_b")
	, p_voffset("p_voffset")
	, p_iack("p_iack")
	, m_clk_prop_proxy(m_clk)
	, payload_fabric()
	, time_per_instruction()
	, clock_multiplier(1.0)
	, bus_cycle_time(10.0, sc_core::SC_NS)
	, run_time()
	, idle_time()
	, enable_host_idle(false)
	, external_event("external_event")
	, int_ack_event("int_ack_event")
	, ipc(1.0)
	, one(1.0)
	, enable_dmi(false)
	, debug_dmi(false)
	, ahb_master_id(0)
	, param_clock_multiplier("clock-multiplier", this, clock_multiplier, "clock multiplier")
	, param_ipc("ipc", this, ipc, "maximum instructions per cycle, typically 1 or 2")
	, param_enable_host_idle("enable-host-idle", this, enable_host_idle, "Enable/Disable host idle periods when target is idle")
	, param_enable_dmi("enable-dmi", this, enable_dmi, "Enable/Disable TLM 2.0 DMI (Direct Memory Access) to speed-up simulation")
	, param_debug_dmi("debug-dmi", this, debug_dmi, "Enable/Disable debugging of DMI (Direct Memory Access)")
	, stat_one("one", this, one, "one")
	, stat_run_time("run-time", this, run_time, "run time")
	, stat_idle_time("idle-time", this, idle_time, "idle time")
	, stat_idle_rate("idle-rate", this, "/", &stat_idle_time, &stat_run_time, "idle rate")
	, stat_load_rate("load-rate", this, "-", &stat_one, &stat_idle_rate, "load rate")
	, param_ahb_master_id("ahb-master-id", this, ahb_master_id, "AHB Master ID")
	, i_dmi_region_cache()
	, d_dmi_region_cache()
{
	i_ahb_if(*this);
	d_ahb_if(*this);
	s_ahb_if(*this);
	
	param_clock_multiplier.SetMutable(false);
//	param_nice_time.SetMutable(false);
	param_ipc.SetMutable(false);
	param_enable_host_idle.SetMutable(false);
	param_enable_dmi.SetMutable(false);
	stat_one.SetMutable(false);
	stat_one.SetSerializable(false);
	stat_one.SetVisible(false);
	
	SC_HAS_PROCESS(CPU);
	
	SC_THREAD(Run);

	sc_core::sc_spawn_options p_iack_process_spawn_options;
	p_iack_process_spawn_options.spawn_method();
	p_iack_process_spawn_options.dont_initialize();
	p_iack_process_spawn_options.set_sensitivity(&int_ack_event);
	
	sc_core::sc_spawn(sc_bind(&CPU<TYPES, CONFIG>::P_IACK_Process, this), "P_IACK_Process", &p_iack_process_spawn_options);
// 	SC_METHOD(P_IACK_Process);
// 	sensitive << int_ack_event;
	
	SC_METHOD(ExternalEventProcess);
	sensitive << p_reset_b.pos() << p_nmi_b.neg() << p_mcp_b.neg() << p_extint_b << p_crint_b;
}

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::~CPU()
{
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::end_of_elaboration()
{
	UpdateSpeed();
	
	sc_core::sc_spawn_options clock_properties_changed_process_spawn_options;
	
	clock_properties_changed_process_spawn_options.spawn_method();
	clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&CPU<TYPES, CONFIG>::ClockPropertiesChangedProcess, this), "ClockPropertiesChangedProcess", &clock_properties_changed_process_spawn_options);
	
	p_iack = false;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::EndSetup()
{
	if(!Super::EndSetup()) return false;
	return true;
}

template <typename TYPES, typename CONFIG>
tlm::tlm_sync_enum CPU<TYPES, CONFIG>::nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == tlm::BEGIN_RESP)
	{
		trans.release();
		return tlm::TLM_COMPLETED;
	}
	return tlm::TLM_ACCEPTED;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	i_dmi_region_cache.Invalidate(start_range, end_range);
	d_dmi_region_cache.Invalidate(start_range, end_range);
	if(CONFIG::DEBUG_ENABLE && debug_dmi)
	{
		Super::logger << DebugInfo << "AHB: invalidate granted access for 0x" << std::hex << start_range << "-0x" << end_range << std::dec << EndDebugInfo;
	}
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	tlm::tlm_command cmd = payload.get_command();
	
	if(cmd != tlm::TLM_IGNORE_COMMAND)
	{
		unsigned int streaming_width = payload.get_streaming_width();
		unsigned int data_length = payload.get_data_length();
		unsigned char *data_ptr = payload.get_data_ptr();
		unsigned char *byte_enable_ptr = payload.get_byte_enable_ptr();
		sc_dt::uint64 start_addr = payload.get_address();
		//sc_dt::uint64 end_addr = start_addr + ((streaming_width > data_length) ? data_length : streaming_width) - 1;
		
		if(!data_ptr)
		{
			this->logger << DebugError << "data pointer for TLM-2.0 GP READ/WRITE command is invalid" << EndDebugError;
			this->Stop(-1);
			return;
		}
		else if(!data_length)
		{
			this->logger << DebugError << "data length range for TLM-2.0 GP READ/WRITE command is invalid" << EndDebugError;
			this->Stop(-1);
			return;
		}
		else if(byte_enable_ptr)
		{
			// byte enable is unsupported
			this->logger << DebugWarning << "byte enable for TLM-2.0 GP READ/WRITE command is unsupported" << EndDebugWarning;
			payload.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
		}
		else if(streaming_width < data_length)
		{
			// streaming is unsupported
			this->logger << DebugWarning << "streaming for TLM-2.0 GP READ/WRITE command is unsupported" << EndDebugWarning;
			payload.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
		}
		else
		{
			switch(cmd)
			{
				case tlm::TLM_READ_COMMAND:
				{
					unsigned int read_bytes = this->IncomingLoad(start_addr + this->local_memory_base_addr, data_ptr, data_length);
					if(read_bytes != data_length)
					{
						payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
					}
					else
					{
						payload.set_response_status(tlm::TLM_OK_RESPONSE);
					}
					break;
				}
				case tlm::TLM_WRITE_COMMAND:
				{
					unsigned int written_bytes = this->IncomingStore(start_addr + this->local_memory_base_addr, data_ptr, data_length);
					if(written_bytes != data_length)
					{
						payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
					}
					else
					{
						payload.set_response_status(tlm::TLM_OK_RESPONSE);
					}
					break;
				}
				case tlm::TLM_IGNORE_COMMAND:
					break;
			}
		}
		
		t += bus_cycle_time;
	}
	
	payload.set_dmi_allowed(true);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	PHYSICAL_ADDRESS addr = payload.get_address();
	PHYSICAL_ADDRESS dmi_start_addr = addr;
	PHYSICAL_ADDRESS dmi_end_addr = addr;

	unsigned char *dmi_ptr = this->GetDirectAccess(addr, dmi_start_addr, dmi_end_addr);

	dmi_data.set_start_address(dmi_start_addr);
	dmi_data.set_end_address(dmi_end_addr);
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);

	if(dmi_ptr)
	{
		//std::cerr << sc_module::name() << ": grant 0x" << std::hex << dmi_start_addr << "-0x" << dmi_end_addr << std::dec << std::endl;
		dmi_data.set_dmi_ptr(dmi_ptr);
		dmi_data.set_read_latency(bus_cycle_time);
		dmi_data.set_write_latency(bus_cycle_time);
		return true;
	}

	return false;
}

template <typename TYPES, typename CONFIG>
unsigned int CPU<TYPES, CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
{
	unsigned int xfered = 0;
	tlm::tlm_command cmd = payload.get_command();
	
	//unsigned int streaming_width = payload.get_streaming_width();
	unsigned int data_length = payload.get_data_length();
	unsigned char *data_ptr = payload.get_data_ptr();
	//unsigned char *byte_enable_ptr = payload.get_byte_enable_ptr();
	sc_dt::uint64 start_addr = payload.get_address();
	//sc_dt::uint64 end_addr = start_addr + ((streaming_width > data_length) ? data_length : streaming_width) - 1;
		
	if(!data_ptr)
	{
		this->logger << DebugError << "data pointer for TLM-2.0 GP READ/WRITE command is invalid" << EndDebugError;
		this->Stop(-1);
		return 0;
	}
	else if(!data_length)
	{
		this->logger << DebugError << "data length range for TLM-2.0 GP READ/WRITE command is invalid" << EndDebugError;
		this->Stop(-1);
		return 0;
	}
	else
	{
		switch(cmd)
		{
			case tlm::TLM_READ_COMMAND:
			{
				xfered = this->DebugIncomingLoad(start_addr + this->local_memory_base_addr, data_ptr, data_length);
				break;
			}
			case tlm::TLM_WRITE_COMMAND:
			{
				xfered = this->DebugIncomingStore(start_addr + this->local_memory_base_addr, data_ptr, data_length);
				break;
			}
			case tlm::TLM_IGNORE_COMMAND:
				break;
		}
	}
	
	return xfered;
}

template <typename TYPES, typename CONFIG>
tlm::tlm_sync_enum CPU<TYPES, CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	b_transport(payload, t);
	return tlm::TLM_COMPLETED;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::Synchronize()
{
	qk.sync();
	run_time = qk.get_current_time();
	
	SampleInputs();
}

template <typename TYPES, typename CONFIG>
inline void CPU<TYPES, CONFIG>::AlignToBusClock()
{
	qk.align_to_clock(bus_cycle_time);
	run_time = qk.get_current_time();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::Idle()
{
	// This is called within thread Run()
	
	// First synchronize with other threads
	Synchronize();
	
	if(!this->HasPendingInterrupts()) // Is there some pending interrupts?
	{
		// no: wait for an external event
		sc_core::sc_time old_time_stamp(sc_core::sc_time_stamp());
		wait(external_event);
		sc_core::sc_time new_time_stamp(sc_core::sc_time_stamp());
		
		// compute the time spent by the SystemC wait
		sc_core::sc_time delta_time(new_time_stamp);
		delta_time -= old_time_stamp;
		
		if(enable_host_idle)
		{
			usleep(delta_time.to_seconds() * 1.0e6); // leave host CPU when target CPU is idle
		}
		
		idle_time += delta_time;
		
		// update overall run time
		qk.reset();
		run_time = qk.get_current_time();
		//run_time = new_time_stamp;
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
void CPU<TYPES, CONFIG>::InterruptAcknowledge()
{
	int_ack_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::InvalidateDirectMemPtr(PHYSICAL_ADDRESS start_addr, PHYSICAL_ADDRESS end_addr)
{
	s_ahb_if->invalidate_direct_mem_ptr(start_addr, end_addr);
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
	
	unsigned int read_size = i_ahb_if->transport_dbg(*payload);

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
	
	unsigned int read_size = d_ahb_if->transport_dbg(*payload);

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
	
	unsigned int write_size = d_ahb_if->transport_dbg(*payload);

	return write_size == size;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::P_IACK_Process()
{
	if(p_iack)
	{
		p_iack = false;
	}
	else
	{
		p_iack = true;
		int_ack_event.notify(bus_cycle_time);
	}
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ExternalEventProcess()
{
	if(p_reset_b.posedge()
	|| p_nmi_b.negedge()
	|| (p_mcp_b.negedge() && this->hid0.template Get<typename Super::HID0::EMCP>())
	|| p_extint_b.negedge()
	|| p_crint_b.negedge())
	{
		SampleInputs();
		external_event.notify(sc_core::SC_ZERO_TIME);
	}
}


template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::SampleInputs()
{
	if(p_reset_b.posedge())
	{
		this->template ThrowException<typename SystemResetInterrupt::Reset>();
		//SetResetAddress(sc_dt::sc_uint<30>(p_rstbase).to_uint64() << 2);
	}
	
	if(p_nmi_b.negedge())
	{
		this->template ThrowException<typename MachineCheckInterrupt::NMI>();
	}
	
	if(p_mcp_b.negedge() && this->hid0.template Get<typename Super::HID0::EMCP>())
	{
		this->template ThrowException<typename MachineCheckInterrupt::AsynchronousMachineCheck>().SetEvent(MachineCheckInterrupt::MCE_MCP);
	}
	
	if(p_extint_b)
	{
		this->template AckInterrupt<ExternalInputInterrupt>();
	}
	else
	{
		this->template ThrowException<typename ExternalInputInterrupt::ExternalInput>();
	}
	
	if(p_crint_b)
	{
		this->template AckInterrupt<CriticalInputInterrupt>();
	}
	else
	{
		this->template ThrowException<typename CriticalInputInterrupt::CriticalInput>();
	}
	this->SetAutoVector(!p_avec_b); // if p_avec_b is negated when interrupt signal is asserted, interrupt is not autovectored
	this->SetVectorOffset(sc_dt::sc_uint<14>(p_voffset).to_uint64() << 2);
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::UpdateSpeed()
{
	bus_cycle_time = m_clk_prop_proxy.GetClockPeriod();
	sc_core::sc_time cpu_cycle_time = bus_cycle_time / clock_multiplier;
	time_per_instruction = cpu_cycle_time / ipc;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ClockPropertiesChangedProcess()
{
	UpdateSpeed();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::Run()
{
	wait(p_reset_b.posedge_event()); // wait for reset signal
	
	SampleInputs();
	
	while(1)
	{
		this->StepOneInstruction();
		// update local time (relative to sc_time_stamp)
		qk.inc(time_per_instruction);
		// update absolute time (local time + sc_time_stamp)
		run_time = qk.get_current_time();
		// Periodically synchronize with other threads
		if(unlikely(qk.need_sync()))
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
		dmi_region = i_dmi_region_cache.Lookup(physical_addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: DMI region cache hit for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: DMI access allowed for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_READ) == tlm::tlm_dmi::DMI_ACCESS_READ))
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(buffer, dmi_data->get_dmi_ptr() + (physical_addr - dmi_data->get_start_address()), size);
					const sc_core::sc_time& read_lat = dmi_region->GetReadLatency(size);
					qk.inc(read_lat);
					run_time = qk.get_current_time();
					return true;
				}
				else
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: granted DMI access does not allow direct read access for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: DMI access denied for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: DMI region cache miss for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}
	
	if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: traditional way for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	unisim::kernel::tlm2::tlm_trans_attr *trans_attr = 0;
	payload->get_extension(trans_attr);
	if(!trans_attr)
	{
		trans_attr = new unisim::kernel::tlm2::tlm_trans_attr();
		payload->set_extension(trans_attr);
	}
	trans_attr->set_master_id(ahb_master_id);
	trans_attr->set_instruction();
	trans_attr->set_privileged(this->msr.template Get<typename Super::MSR::PR>() == 0);
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_READ_COMMAND);
	payload->set_streaming_width(size);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	i_ahb_if->b_transport(*payload, qk.get_local_time());
	run_time = qk.get_current_time();
	
	tlm::tlm_response_status status = payload->get_response_status();
	
	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: target allows DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			
			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			payload->set_address(physical_addr);
			payload->set_data_length(size);
			unisim::kernel::tlm2::DMIGrant dmi_grant = i_ahb_if->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			i_dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: target does not allow DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}
	
	payload->release();
	
	if(unlikely(status != tlm::TLM_OK_RESPONSE))
	{
		switch(status)
		{
			case tlm::TLM_OK_RESPONSE: return true;
			case tlm::TLM_COMMAND_ERROR_RESPONSE:
				if(this->verbose_exception)
				{
					this->logger << DebugInfo << "Instruction Read Access Control Error at @0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
				this->template ThrowException<typename InstructionStorageInterrupt::AccessControl>();
				break;
			case tlm::TLM_INCOMPLETE_RESPONSE:
			case tlm::TLM_GENERIC_ERROR_RESPONSE:
			case tlm::TLM_ADDRESS_ERROR_RESPONSE:
			case tlm::TLM_BURST_ERROR_RESPONSE:
			case tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE:
				if(this->verbose_exception)
				{
					this->logger << DebugInfo << "Instruction Read Bus Error at @0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
				this->template ThrowException<typename MachineCheckInterrupt::AsynchronousMachineCheck>().SetEvent(MachineCheckInterrupt::MCE_INSTRUCTION_READ_BUS_ERROR);
				break;
		}
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
		dmi_region = d_dmi_region_cache.Lookup(physical_addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: DMI region cache hit for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: DMI access allowed for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_READ) == tlm::tlm_dmi::DMI_ACCESS_READ))
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(buffer, dmi_data->get_dmi_ptr() + (physical_addr - dmi_data->get_start_address()), size);
					const sc_core::sc_time& read_lat = dmi_region->GetReadLatency(size);
					qk.inc(read_lat);
					run_time = qk.get_current_time();
					return true;
				}
				else
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: granted DMI access does not allow direct read access for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: DMI access denied for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: DMI region cache miss for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: traditional way for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	unisim::kernel::tlm2::tlm_trans_attr *trans_attr = 0;
	payload->get_extension(trans_attr);
	if(!trans_attr)
	{
		trans_attr = new unisim::kernel::tlm2::tlm_trans_attr();
		payload->set_extension(trans_attr);
	}
	trans_attr->set_master_id(ahb_master_id);
	trans_attr->set_privileged(this->msr.template Get<typename Super::MSR::PR>() == 0);

	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_READ_COMMAND);
	payload->set_streaming_width(size);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	d_ahb_if->b_transport(*payload, qk.get_local_time());
	run_time = qk.get_current_time();
	
	tlm::tlm_response_status status = payload->get_response_status();
	
	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: target allows DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;

			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			payload->set_address(physical_addr);
			payload->set_data_length(size);
			unisim::kernel::tlm2::DMIGrant dmi_grant = d_ahb_if->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			d_dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: target does not allow DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	payload->release();

	if(unlikely(status != tlm::TLM_OK_RESPONSE))
	{
		switch(status)
		{
			case tlm::TLM_OK_RESPONSE: return true;
			case tlm::TLM_COMMAND_ERROR_RESPONSE:
				if(this->verbose_exception)
				{
					this->logger << DebugInfo << "Data Read Access Control Error at @0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
				this->template ThrowException<typename DataStorageInterrupt::AccessControl>().SetAddress(physical_addr); // FIXME: physical address != logical address
				break;
			case tlm::TLM_INCOMPLETE_RESPONSE:
			case tlm::TLM_GENERIC_ERROR_RESPONSE:
			case tlm::TLM_ADDRESS_ERROR_RESPONSE:
			case tlm::TLM_BURST_ERROR_RESPONSE:
			case tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE:
				if(this->verbose_exception)
				{
					this->logger << DebugInfo << "Data Read Bus Error at @0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
				this->template ThrowException<typename MachineCheckInterrupt::AsynchronousMachineCheck>().SetEvent(MachineCheckInterrupt::MCE_DATA_READ_BUS_ERROR);
				break;
		}
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
		dmi_region = d_dmi_region_cache.Lookup(physical_addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: DMI region cache hit for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: DMI access allowed for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_WRITE) == tlm::tlm_dmi::DMI_ACCESS_WRITE))
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(dmi_data->get_dmi_ptr() + (physical_addr - dmi_data->get_start_address()), buffer, size);
					const sc_core::sc_time& write_lat = dmi_region->GetWriteLatency(size);
					qk.inc(write_lat);
					run_time = qk.get_current_time();
					return true;
				}
				else
				{
					if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: granted DMI access does not allow direct write access for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: DMI access denied for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: DMI region cache miss for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: traditional way for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	unisim::kernel::tlm2::tlm_trans_attr *trans_attr = 0;
	payload->get_extension(trans_attr);
	if(!trans_attr)
	{
		trans_attr = new unisim::kernel::tlm2::tlm_trans_attr();
		payload->set_extension(trans_attr);
	}
	trans_attr->set_master_id(ahb_master_id);
	trans_attr->set_privileged(this->msr.template Get<typename Super::MSR::PR>() == 0);
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_WRITE_COMMAND);
	payload->set_streaming_width(size);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	d_ahb_if->b_transport(*payload, qk.get_local_time());
	run_time = qk.get_current_time();
	
	tlm::tlm_response_status status = payload->get_response_status();

	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: target allows DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			payload->set_address(physical_addr);
			payload->set_data_length(size);
			unisim::kernel::tlm2::DMIGrant dmi_grant = d_ahb_if->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			d_dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(CONFIG::DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: target does not allow DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	payload->release();

	if(unlikely(status != tlm::TLM_OK_RESPONSE))
	{
		switch(status)
		{
			case tlm::TLM_OK_RESPONSE: return true;
			case tlm::TLM_COMMAND_ERROR_RESPONSE:
				if(this->verbose_exception)
				{
					this->logger << DebugInfo << "Data Write Access Control Error at @0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
				this->template ThrowException<typename DataStorageInterrupt::AccessControl>().SetAddress(physical_addr); // FIXME: physical address != logical address
				break;
			case tlm::TLM_INCOMPLETE_RESPONSE:
			case tlm::TLM_GENERIC_ERROR_RESPONSE:
			case tlm::TLM_ADDRESS_ERROR_RESPONSE:
			case tlm::TLM_BURST_ERROR_RESPONSE:
			case tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE:
				if(this->verbose_exception)
				{
					this->logger << DebugInfo << "Data Write Bus Error at @0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
				this->template ThrowException<typename MachineCheckInterrupt::AsynchronousMachineCheck>().SetEvent(MachineCheckInterrupt::MCE_DATA_WRITE_BUS_ERROR);
				break;
		}
		return false;
	}
	
	return true;
}

} // end of namespace mpc57xx
} // end of namespace e200
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_E200_MPC57XX_CPU_TCC__
