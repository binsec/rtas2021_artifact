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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_PROGRAMMABLE_ROUTER_ROUTER_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_PROGRAMMABLE_ROUTER_ROUTER_TCC__

#include <unisim/component/tlm2/interconnect/programmable_router/router.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.tcc>
#include <unisim/util/reg/core/register.tcc>
#include <unisim/util/likely/likely.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace programmable_router {

template <typename CONFIG>
const unsigned int Router<CONFIG>::PERIPHERAL_BUSWIDTH;

template <typename CONFIG>
const unsigned int Router<CONFIG>::NUM_PERIPHERAL_SLAVE_IFS;

template <typename CONFIG>
const bool Router<CONFIG>::threaded_model;

template <typename CONFIG>
Router<CONFIG>::Router(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, Super(name, parent)
	, peripheral_slave_if("peripheral_slave_if")
	, m_clk("m_clk")
	, reset_b("reset_b")
	, m_clk_prop_proxy(m_clk)
	, reg_addr_map()
	, schedule()
	, endian(unisim::util::endian::E_BIG_ENDIAN)
	, param_endian("endian", this, endian, "endian")
	, master_clock_period(10.0, sc_core::SC_NS)
	, master_clock_start_time(sc_core::SC_ZERO_TIME)
	, master_clock_posedge_first(true)
	, master_clock_duty_cycle(0.5)
{
	unsigned int i;
	
	for(i = 0; i < NUM_PERIPHERAL_SLAVE_IFS; i++)
	{
		peripheral_slave_if[i].register_nb_transport_fw(this, &Router<CONFIG>::peripheral_nb_transport_fw, i);
		peripheral_slave_if[i].register_b_transport(this, &Router<CONFIG>::peripheral_b_transport, i);
		peripheral_slave_if[i].register_transport_dbg(this, &Router<CONFIG>::peripheral_transport_dbg, i);
		peripheral_slave_if[i].register_get_direct_mem_ptr(this, &Router<CONFIG>::peripheral_get_direct_mem_ptr, i);
	}
	
	SC_HAS_PROCESS(Router);

	// Map Router registers regarding there address offsets
	for(i = 0; i < NUM_PERIPHERAL_SLAVE_IFS; i++)
	{
		reg_addr_map[i].SetWarningStream(this->logger.DebugWarningStream());
		reg_addr_map[i].SetEndian(endian);
	}

	SC_METHOD(RESET_B_Process);
	this->sensitive << reset_b.pos();

	if(threaded_model)
	{
		SC_THREAD(Process);
		this->sensitive << schedule.GetKernelEvent();
	}
	else
	{
		SC_METHOD(Process);
		this->sensitive << schedule.GetKernelEvent();
	}
}

template <typename CONFIG>
Router<CONFIG>::~Router<CONFIG>()
{
}

template <typename CONFIG>
void Router<CONFIG>::end_of_elaboration()
{
	Super::end_of_elaboration();

	// Spawn MasterClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options master_clock_properties_changed_process_spawn_options;
	
	master_clock_properties_changed_process_spawn_options.spawn_method();
	master_clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&Router<CONFIG>::MasterClockPropertiesChangedProcess, this), "MasterClockPropertiesChangedProcess", &master_clock_properties_changed_process_spawn_options);

	Reset();
}

template <typename CONFIG>
void Router<CONFIG>::Reset()
{
	UpdateMasterClock();
}

template <typename CONFIG>
void Router<CONFIG>::MapRegister(sc_dt::uint64 addr, AddressableRegisterBase<> *reg, unsigned int reg_byte_size)
{
	unsigned int i;
	for(i = 0; i < NUM_PERIPHERAL_SLAVE_IFS; i++)
	{
		reg_addr_map[i].MapRegister(addr, reg, reg_byte_size);
	}
}

template <typename CONFIG>
void Router<CONFIG>::MapRegisterFile(sc_dt::uint64 addr, AddressableRegisterFileBase<> *regfile, unsigned int reg_byte_size, unsigned int stride)
{
	unsigned int i;
	for(i = 0; i < NUM_PERIPHERAL_SLAVE_IFS; i++)
	{
		reg_addr_map[i].MapRegisterFile(addr, regfile, reg_byte_size, stride);
	}
}

template <typename CONFIG>
void Router<CONFIG>::UnmapRegistersAt(sc_dt::uint64 addr, unsigned int byte_size)
{
	unsigned int i;
	for(i = 0; i < NUM_PERIPHERAL_SLAVE_IFS; i++)
	{
		reg_addr_map[i].UnmapRegistersAt(addr, byte_size);
	}
}

template <typename CONFIG>
void Router<CONFIG>::ClearRegisterMap()
{
	unsigned int i;
	for(i = 0; i < NUM_PERIPHERAL_SLAVE_IFS; i++)
	{
		reg_addr_map[i].ClearRegisterMap();
	}
}

template <typename CONFIG>
void Router<CONFIG>::MapRegister(int iface, sc_dt::uint64 addr, AddressableRegisterBase<> *reg, unsigned int reg_byte_size)
{
	reg_addr_map[iface].MapRegister(addr, reg, reg_byte_size);
}

template <typename CONFIG>
void Router<CONFIG>::MapRegisterFile(int iface, sc_dt::uint64 addr, AddressableRegisterFileBase<> *regfile, unsigned int reg_byte_size, unsigned int stride)
{
	reg_addr_map[iface].MapRegisterFile(addr, regfile, reg_byte_size, stride);
}

template <typename CONFIG>
void Router<CONFIG>::UnmapRegistersAt(int iface, sc_dt::uint64 addr, unsigned int byte_size)
{
	reg_addr_map[iface].UnmapRegistersAt(addr, byte_size);
}

template <typename CONFIG>
void Router<CONFIG>::ClearRegisterMap(int iface)
{
	reg_addr_map[iface].ClearRegisterMap();
}

template <typename CONFIG>
void Router<CONFIG>::peripheral_b_transport(int iface, tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	sc_core::sc_event completion_event;
	sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
	notify_time_stamp += t;
	unisim::kernel::tlm2::AlignToClock(notify_time_stamp, master_clock_period, master_clock_start_time, master_clock_posedge_first, master_clock_duty_cycle);
	Event *event = schedule.AllocEvent();
	event->SetPayload(&payload);
	event->SetTimeStamp(notify_time_stamp);
	event->SetInterface(iface);
	event->SetCompletionEvent(&completion_event);
	schedule.Notify(event);
	sc_core::wait(completion_event);
	t = sc_core::SC_ZERO_TIME;
}

template <typename CONFIG>
bool Router<CONFIG>::peripheral_get_direct_mem_ptr(int iface, tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	// Deny direct memory interface access
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int Router<CONFIG>::peripheral_transport_dbg(int iface, tlm::tlm_generic_payload& payload)
{
	tlm::tlm_command cmd = payload.get_command();
	unsigned int data_length = payload.get_data_length();
	unsigned char *data_ptr = payload.get_data_ptr();
	sc_dt::uint64 start_addr = payload.get_address();
	sc_core::sc_time time_stamp(sc_core::sc_time_stamp());
	
	if(!data_ptr)
	{
		this->logger << DebugError << "data pointer for TLM-2.0 GP READ/WRITE command is invalid" << EndDebugError;
		unisim::kernel::Object::Stop(-1);
		return 0;
	}
	else if(!data_length)
	{
		return 0;
	}
	
	switch(cmd)
	{
		case tlm::TLM_WRITE_COMMAND:
			return reg_addr_map[iface].DebugWrite(start_addr, data_ptr, data_length);
		case tlm::TLM_READ_COMMAND:
			return reg_addr_map[iface].DebugRead(start_addr, data_ptr, data_length);
		default:
			break;
	}
	
	return 0;
}

template <typename CONFIG>
tlm::tlm_sync_enum Router<CONFIG>::peripheral_nb_transport_fw(int iface, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
				notify_time_stamp += t;
				unisim::kernel::tlm2::AlignToClock(notify_time_stamp, master_clock_period, master_clock_start_time, master_clock_posedge_first, master_clock_duty_cycle);
				Event *event = schedule.AllocEvent();
				event->SetPayload(&payload);
				event->SetTimeStamp(notify_time_stamp);
				event->SetInterface(iface);
				schedule.Notify(event);
				phase = tlm::END_REQ;
				return tlm::TLM_UPDATED;
			}
			break;
		case tlm::END_RESP:
			return tlm::TLM_COMPLETED;
		default:
			this->logger << DebugError << "protocol error" << EndDebugError;
			unisim::kernel::Object::Stop(-1);
			break;
	}
	
	return tlm::TLM_COMPLETED;
}

template <typename CONFIG>
void Router<CONFIG>::ProcessEvent(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	int iface = event->GetInterface();
	tlm::tlm_command cmd = payload->get_command();

	if(cmd != tlm::TLM_IGNORE_COMMAND)
	{
		unsigned int streaming_width = payload->get_streaming_width();
		unsigned int data_length = payload->get_data_length();
		unsigned char *data_ptr = payload->get_data_ptr();
		unsigned char *byte_enable_ptr = payload->get_byte_enable_ptr();
		sc_dt::uint64 start_addr = payload->get_address();
//		sc_dt::uint64 end_addr = start_addr + ((streaming_width > data_length) ? data_length : streaming_width) - 1;
		
		if(!data_ptr)
		{
			this->logger << DebugError << "data pointer for TLM-2.0 GP READ/WRITE command is invalid" << EndDebugError;
			unisim::kernel::Object::Stop(-1);
			return;
		}
		else if(!data_length)
		{
			this->logger << DebugError << "data length range for TLM-2.0 GP READ/WRITE command is invalid" << EndDebugError;
			unisim::kernel::Object::Stop(-1);
			return;
		}
		else if(byte_enable_ptr)
		{
			// byte enable is unsupported
			this->logger << DebugWarning << "byte enable for TLM-2.0 GP READ/WRITE command is unsupported" << EndDebugWarning;
			payload->set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
		}
		else if(streaming_width < data_length)
		{
			// streaming is unsupported
			this->logger << DebugWarning << "streaming for TLM-2.0 GP READ/WRITE command is unsupported" << EndDebugWarning;
			payload->set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
		}
		else
		{
			ReadWriteStatus rws = RWS_OK;
			
			switch(cmd)
			{
				case tlm::TLM_WRITE_COMMAND:
					rws = reg_addr_map[iface].Write(start_addr, data_ptr, data_length);
					break;
				case tlm::TLM_READ_COMMAND:
					rws = reg_addr_map[iface].Read(start_addr, data_ptr, data_length);
					break;
				default:
					break;
			}
			
			if(IsReadWriteError(rws))
			{
				this->logger << DebugError << "while mapped read/write access, " << std::hex << rws << std::dec << EndDebugError;
				if(IsUnmappedAccessError(rws)) payload->set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
				else if(IsAccessNotAllowedError(rws)) payload->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
				else payload->set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
			}
			else
			{
				payload->set_response_status(tlm::TLM_OK_RESPONSE);
			}
		}
	}

	payload->set_dmi_allowed(false);
	
	sc_core::sc_time completion_time(sc_core::SC_ZERO_TIME);
	sc_core::sc_event *completion_event = event->GetCompletionEvent();
	
	if(completion_event)
	{
		completion_event->notify(completion_time);
	}
	else
	{
		tlm::tlm_phase phase = tlm::BEGIN_RESP;
		
		tlm::tlm_sync_enum sync = peripheral_slave_if[iface]->nb_transport_bw(*payload, phase, completion_time);
		
		switch(sync)
		{
			case tlm::TLM_ACCEPTED:
				break;
			case tlm::TLM_UPDATED:
				break;
			case tlm::TLM_COMPLETED:
				break;
		}
	}
}

template <typename CONFIG>
void Router<CONFIG>::ProcessEvents()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	Event *event = schedule.GetNextEvent();
	
	if(event)
	{
		do
		{
			if(event->GetTimeStamp() != time_stamp)
			{
				this->logger << DebugError << "Internal error: unexpected event at time stamp (" << event->GetTimeStamp() << " instead of " << time_stamp << ")" << EndDebugError;
				unisim::kernel::Object::Stop(-1);
			}
			
			ProcessEvent(event);
			schedule.FreeEvent(event);
		}
		while((event = schedule.GetNextEvent()) != 0);
	}
}

template <typename CONFIG>
void Router<CONFIG>::Process()
{
	if(threaded_model)
	{
		while(1)
		{
			this->wait();
			ProcessEvents();
		}
	}
	else
	{
		ProcessEvents();
		this->next_trigger();
	}
}

template <typename CONFIG>
void Router<CONFIG>::RESET_B_Process()
{
	if(reset_b.posedge())
	{
		Reset();
	}
}

template <typename CONFIG>
void Router<CONFIG>::MasterClockPropertiesChangedProcess()
{
	UpdateMasterClock();
}

template <typename CONFIG>
void Router<CONFIG>::UpdateMasterClock()
{
	master_clock_period = m_clk_prop_proxy.GetClockPeriod();
	master_clock_start_time = m_clk_prop_proxy.GetClockStartTime();
	master_clock_posedge_first = m_clk_prop_proxy.GetClockPosEdgeFirst();
	master_clock_duty_cycle = m_clk_prop_proxy.GetClockDutyCycle();
}

} // end of namespace programmable_router
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_PROGRAMMABLE_ROUTER_ROUTER_TCC__
