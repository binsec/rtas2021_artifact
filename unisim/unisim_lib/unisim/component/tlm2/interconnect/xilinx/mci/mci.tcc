/*
 *  Copyright (c) 2010-2011,
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
 
#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_MCI_MCI_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_MCI_MCI_TCC__

#include <unisim/util/endian/endian.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace xilinx {
namespace mci {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG>
MCI<CONFIG>::MCI(const sc_core::sc_module_name& name, Object *parent)
	: Object(name, parent, "A Memory Controller Interface (MCI)")
	, unisim::component::cxx::interconnect::xilinx::mci::MCI<CONFIG>(name, parent)
	, sc_core::sc_module(name)
	, mci_slave_sock("mci-slave-sock")
	, mci_master_sock("mci-master-sock")
	, dcr_slave_sock("dcr-slave-sock")
	, cycle_time(sc_core::SC_ZERO_TIME)
	, burst_latency_lut()
	, param_cycle_time("cycle-time", this, cycle_time, "Enable/Disable verbosity")
{
	mci_fw_redirector = 
		new unisim::kernel::tlm2::FwRedirector<MCI<CONFIG> >(
			inherited::IF_SLAVE_MCI,
			this,
			&MCI<CONFIG>::nb_transport_fw,
			&MCI<CONFIG>::b_transport,
			&MCI<CONFIG>::transport_dbg,
			&MCI<CONFIG>::get_direct_mem_ptr
		);
	
	mci_slave_sock(*mci_fw_redirector);
		
	dcr_redirector =
		new unisim::kernel::tlm2::FwRedirector<MCI<CONFIG> >(
			inherited::IF_DCR,
			this,
			&MCI<CONFIG>::nb_transport_fw,
			&MCI<CONFIG>::b_transport,
			&MCI<CONFIG>::transport_dbg,
			&MCI<CONFIG>::get_direct_mem_ptr
		);
	
	dcr_slave_sock(*dcr_redirector);

	mci_bw_redirector =
		new unisim::kernel::tlm2::BwRedirector<MCI<CONFIG> >(
			inherited::IF_MASTER_MCI,
			this,
			&MCI<CONFIG>::nb_transport_bw,
			&MCI<CONFIG>::invalidate_direct_mem_ptr
		);

	mci_master_sock(*mci_bw_redirector);

	SC_HAS_PROCESS(MCI);
	
	if(threaded_model)
	{
		SC_THREAD(Process);
	}
	else
	{
		SC_METHOD(Process);
	}
}

template <class CONFIG>
MCI<CONFIG>::~MCI()
{
	delete mci_fw_redirector;
	delete dcr_redirector;
	delete mci_bw_redirector;
}

template <class CONFIG>
bool MCI<CONFIG>::BeginSetup()
{
	if(cycle_time == sc_core::SC_ZERO_TIME)
	{
		inherited::logger << DebugError << "Parameter " << param_cycle_time.GetName() << " must be > " << sc_core::SC_ZERO_TIME << EndDebugError;
		return false;
	}
	
	burst_latency_lut.SetBaseLatency(cycle_time);
	
	return true;
}

template <class CONFIG>
tlm::tlm_sync_enum MCI<CONFIG>::nb_transport_fw(unsigned int intf, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			switch(intf)
			{
				case inherited::IF_SLAVE_MCI:
				case inherited::IF_DCR:
					{
						sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
						notify_time_stamp += t;
						Event *event = schedule.AllocEvent();
						event->Initialize(&payload, (typename inherited::Interface) intf, notify_time_stamp);
						schedule.Notify(event);
					}
					phase = tlm::END_REQ;
					return tlm::TLM_UPDATED;
				default:
					inherited::logger << DebugError << "Internal error" << EndDebugError;
					Object::Stop(-1);
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
void MCI<CONFIG>::b_transport(unsigned int intf, tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	switch(intf)
	{
		case inherited::IF_SLAVE_MCI:
		case inherited::IF_DCR:
			{
				sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
				notify_time_stamp += t;
				Event *event = schedule.AllocEvent();
				sc_core::sc_event ev_completed;
				event->Initialize(&payload, (typename inherited::Interface) intf, notify_time_stamp, &ev_completed);
				schedule.Notify(event);
				wait(ev_completed);
				t = sc_core::SC_ZERO_TIME;
			}
			break;
		default:
			inherited::logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
	}
}

template <class CONFIG>
unsigned int MCI<CONFIG>::transport_dbg(unsigned int intf, tlm::tlm_generic_payload& payload)
{
	switch(intf)
	{
		case inherited::IF_SLAVE_MCI:
			return inherited::GetMI_CONTROL_ENABLE() ? mci_master_sock->transport_dbg(payload) : 0;
		case inherited::IF_DCR:
			return 0;
		default:
			inherited::logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
	}
	return 0;
}

template <class CONFIG>
bool MCI<CONFIG>::get_direct_mem_ptr(unsigned int intf, tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	if(intf == inherited::IF_SLAVE_MCI)
	{
		if(inherited::GetMI_CONTROL_ENABLE())
		{
			bool dmi_status = mci_master_sock->get_direct_mem_ptr(payload, dmi_data);
			
			// add MCI latency per byte
			if(dmi_status)
			{
				dmi_data.set_read_latency(dmi_data.get_read_latency() + (cycle_time / CONFIG::MCI_WIDTH));
				dmi_data.set_write_latency(dmi_data.get_write_latency() + (cycle_time / CONFIG::MCI_WIDTH));
			}
			return dmi_status;
		}
	}
	
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address((sc_dt::uint64) -1);
	return false;
}

template <class CONFIG>
tlm::tlm_sync_enum MCI<CONFIG>::nb_transport_bw(unsigned int intf, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_RESP:
			switch(intf)
			{
				case inherited::IF_MASTER_MCI:
					{
						sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
						notify_time_stamp += t;
						Event *event = schedule.AllocEvent();
						event->Initialize(&payload, (typename inherited::Interface) intf, notify_time_stamp);
						schedule.Notify(event);
					}
					return tlm::TLM_COMPLETED;
				default:
					inherited::logger << DebugError << "Internal error" << EndDebugError;
					Object::Stop(-1);
			}
			break;
		case tlm::END_REQ:
			return tlm::TLM_COMPLETED;
		default:
			inherited::logger << DebugError << "protocol error" << EndDebugError;
			Object::Stop(-1);
			break;
	}
	
	return tlm::TLM_COMPLETED;
}

template <class CONFIG>
void MCI<CONFIG>::invalidate_direct_mem_ptr(unsigned int intf, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	if(intf == inherited::IF_MASTER_MCI)
	{
		mci_slave_sock->invalidate_direct_mem_ptr(start_range, end_range);
	}
}

template <class CONFIG>
void MCI<CONFIG>::ProcessEvents()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	if(inherited::IsVerbose())
	{
		inherited::logger << DebugInfo << time_stamp << ": Waking up" << EndDebugInfo;
	}
	
	Event *event = schedule.GetNextEvent();
	
	if(event)
	{
		do
		{
			if(event->GetTimeStamp() != time_stamp)
			{
				inherited::logger << DebugError << "Internal error: unexpected event time stamp (" << event->GetTimeStamp() << " instead of " << time_stamp << ")" << EndDebugError;
				Object::Stop(-1);
			}
			if((time_stamp.value() % cycle_time.value()) != 0)
			{
				inherited::logger << DebugError << "Internal error: time stamp is not aligned on clock (time stamp is " << time_stamp << " while cycle time is " << cycle_time << ")" << EndDebugError;
				Object::Stop(-1);
			}

			switch(event->GetInterface())
			{
				case inherited::IF_SLAVE_MCI:
					ProcessForwardEvent(event);
					break;
				case inherited::IF_MASTER_MCI:
					ProcessBackwardEvent(event);
					break;
				case inherited::IF_DCR:
					ProcessDCREvent(event);
					break;
			}
		}
		while((event = schedule.GetNextEvent()) != 0);
	}
}

template <class CONFIG>
void MCI<CONFIG>::Process()
{
	if(threaded_model)
	{
		while(1)
		{
			ProcessEvents();
			wait(schedule.GetKernelEvent());
		}
	}
	else
	{
		ProcessEvents();
		next_trigger(schedule.GetKernelEvent());
	}
}

template <class CONFIG>
void MCI<CONFIG>::ProcessForwardEvent(Event *event)
{
	sc_core::sc_event *ev_completed = event->GetCompletionEvent();
	tlm::tlm_generic_payload *payload = event->GetPayload();
	sc_core::sc_time t(burst_latency_lut.Lookup((payload->get_data_length() + CONFIG::MCI_WIDTH - 1) / CONFIG::MCI_WIDTH));
	
	if(inherited::GetMI_CONTROL_ENABLE())
	{
		if(inherited::IsVerbose())
		{
			inherited::logger << DebugInfo << "Forward payload @0x" << std::hex << payload->get_address() << std::dec << EndDebugInfo;
		}

		tlm::tlm_phase phase = tlm::BEGIN_REQ;
		tlm::tlm_sync_enum sync = tlm::TLM_ACCEPTED;
		
		sync = mci_master_sock->nb_transport_fw(*payload, phase, t);
		
		if(sync == tlm::TLM_COMPLETED)
		{
			if(ev_completed)
			{
				ev_completed->notify(t);
			}
			else
			{
				phase = tlm::BEGIN_RESP;
				
				mci_slave_sock->nb_transport_bw(*payload, phase, t);
			}
			
			schedule.FreeEvent(event);
		}
	}
	else
	{
		inherited::logger << DebugWarning << "MCI is disabled. Request @0x" << std::hex << payload->get_address() << std::dec << " failed" << EndDebugWarning;
		
		payload->set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
		
		if(ev_completed)
		{
			ev_completed->notify(t);
		}
		else
		{
			tlm::tlm_phase phase = tlm::BEGIN_RESP;
			
			mci_slave_sock->nb_transport_bw(*payload, phase, t);
		}
		
		schedule.FreeEvent(event);
	}
}

template <class CONFIG>
void MCI<CONFIG>::ProcessBackwardEvent(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	
	sc_core::sc_time t(burst_latency_lut.Lookup((payload->get_data_length() + CONFIG::MCI_WIDTH - 1) / CONFIG::MCI_WIDTH));

	if(inherited::IsVerbose())
	{
		inherited::logger << DebugInfo << "Backward payload @0x" << std::hex << payload->get_address() << std::dec << EndDebugInfo;
	}

	sc_core::sc_event *ev_completed = event->GetCompletionEvent();
	if(ev_completed)
	{
		ev_completed->notify(t);
	}
	else
	{
		tlm::tlm_phase phase = tlm::BEGIN_RESP;
		
		mci_slave_sock->nb_transport_bw(*payload, phase, t);
	}
	schedule.FreeEvent(event);
}

template <class CONFIG>
void MCI<CONFIG>::ProcessDCREvent(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	
	tlm::tlm_command cmd = payload->get_command();
	uint64_t addr = payload->get_address();
	unsigned char *data_ptr = payload->get_data_ptr();
	unsigned int data_length = payload->get_data_length();
	unsigned char *byte_enable_ptr = payload->get_byte_enable_ptr();
	unsigned int byte_enable_length = byte_enable_ptr ? payload->get_byte_enable_length() : 0;
	unsigned int streaming_width = payload->get_streaming_width();
	
	tlm::tlm_response_status status = tlm::TLM_OK_RESPONSE;
	
	uint32_t dcrn = addr / 4;
	
	if(cmd != tlm::TLM_IGNORE_COMMAND)
	{
		if((addr & 3) != 0)
		{
			// only 32-bit aligned accesses are supported
			inherited::logger << DebugError << event->GetTimeStamp()
				<< ": DCR address 0x" << std::hex << addr << std::dec << " is not 32-bit aligned"
				<< EndDebugError;
			Object::Stop(-1);
			return;
		}
		else if(data_length != 4)
		{
			// only data length of 4 bytes is supported
			inherited::logger << DebugError << event->GetTimeStamp()
				<< ": data length of " << data_length << " bytes are unsupported"
				<< EndDebugError;
			Object::Stop(-1);
			return;
		}
		else if(streaming_width && (streaming_width != data_length))
		{
			// streaming is not supported if different from data length
			inherited::logger << DebugError << event->GetTimeStamp()
					<< ": data length of " << data_length << " bytes and streaming are unsupported"
					<< EndDebugError;
			Object::Stop(-1);
			return;
		}
		else if(byte_enable_length)
		{
			// byte enable is not supported
			inherited::logger << DebugError << event->GetTimeStamp()
				<< ": byte enable is unsupported"
				<< EndDebugError;
			Object::Stop(-1);
			return;
		}
		else if(!inherited::IsMappedDCR(dcrn))
		{
			inherited::logger << DebugWarning << event->GetTimeStamp()
				<< ": unmapped access at 0x" << std::hex << addr << std::dec << " ( " << data_length << " bytes)"
				<< EndDebugWarning;
			status = tlm::TLM_ADDRESS_ERROR_RESPONSE;
		}
	}
	
	if(status == tlm::TLM_OK_RESPONSE)
	{
		switch(cmd)
		{
			case tlm::TLM_READ_COMMAND:
				{
					uint32_t value = unisim::util::endian::Host2BigEndian(inherited::ReadDCR(dcrn));
					memcpy(data_ptr, &value, sizeof(value));
				}
				break;
			case tlm::TLM_WRITE_COMMAND:
				{
					uint32_t value;
					memcpy(&value, data_ptr, sizeof(value));
					inherited::WriteDCR(dcrn, unisim::util::endian::BigEndian2Host(value));
					if(dcrn == CONFIG::MI_CONTROL)
					{
						mci_slave_sock->invalidate_direct_mem_ptr(0, (sc_dt::uint64) -1);
					}
				}
				break;
			case tlm::TLM_IGNORE_COMMAND:
				inherited::logger << DebugError << event->GetTimeStamp() 
						<< " : received an unexpected TLM_IGNORE_COMMAND command in payload at 0x"
						<< std::hex << addr << std::dec
						<< EndDebugError;
				Object::Stop(-1);
		}
	}
	
	payload->set_response_status(status);
	payload->set_dmi_allowed(false);

	sc_core::sc_event *ev_completed = event->GetCompletionEvent();
	if(ev_completed)
	{
		ev_completed->notify(cycle_time);
	}
	else
	{
		sc_core::sc_time t(cycle_time);
		tlm::tlm_phase phase = tlm::BEGIN_RESP;
		
		dcr_slave_sock->nb_transport_bw(*payload, phase, t);
	}
	
	schedule.FreeEvent(event);
}

} // end of namespace mci
} // end of namespace xilinx
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_MCI_MCI_TCC__
