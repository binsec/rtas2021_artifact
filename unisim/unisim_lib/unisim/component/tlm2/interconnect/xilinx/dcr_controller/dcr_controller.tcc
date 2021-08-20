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
 
#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_DCR_CONTROLLER_DCR_CONTROLLER_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_DCR_CONTROLLER_DCR_CONTROLLER_TCC__

#include <unisim/util/endian/endian.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace xilinx {
namespace dcr_controller {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG>
DCRController<CONFIG>::DCRController(const sc_core::sc_module_name& name, Object *parent)
	: Object(name, parent, "A Device Control Register bus controller")
	, unisim::component::cxx::interconnect::xilinx::dcr_controller::DCRController<CONFIG>(name, parent)
	, sc_core::sc_module(name)
	, cycle_time(sc_core::SC_ZERO_TIME)
	, param_cycle_time("cycle-time", this, cycle_time, "Cycle time")
{
	unsigned int num_slave;
	unsigned int num_master;
	
	for(num_slave = 0; num_slave < CONFIG::NUM_SLAVES; num_slave++)
	{
		std::stringstream sstr_dcr_master_sock_name;
		sstr_dcr_master_sock_name << "dcr-master-sock" << num_slave;
		dcr_master_sock[num_slave] = new dcr_master_socket(sstr_dcr_master_sock_name.str().c_str());
		
		bw_redirector[num_slave] = new unisim::kernel::tlm2::BwRedirector<DCRController<CONFIG> >(num_slave, this, &DCRController<CONFIG>::nb_transport_bw, &DCRController<CONFIG>::invalidate_direct_mem_ptr);
		
		(*dcr_master_sock[num_slave])(*bw_redirector[num_slave]);
	}

	for(num_master = 0; num_master < CONFIG::NUM_MASTERS; num_master++)
	{
		std::stringstream sstr_dcr_slave_sock_name;
		sstr_dcr_slave_sock_name << "dcr-slave-sock" << num_master;
		dcr_slave_sock[num_master] = new dcr_slave_socket(sstr_dcr_slave_sock_name.str().c_str());
		
		fw_redirector[num_master] = new unisim::kernel::tlm2::FwRedirector<DCRController<CONFIG> >(num_master, this, &DCRController<CONFIG>::nb_transport_fw, &DCRController<CONFIG>::b_transport, &DCRController<CONFIG>::transport_dbg, &DCRController<CONFIG>::get_direct_mem_ptr);
		
		(*dcr_slave_sock[num_master])(*fw_redirector[num_master]);
	}

	SC_HAS_PROCESS(DCRController);
	
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
DCRController<CONFIG>::~DCRController()
{
	unsigned int num_slave;
	unsigned int num_master;
	
	for(num_slave = 0; num_slave < CONFIG::NUM_SLAVES; num_slave++)
	{
		delete dcr_master_sock[num_slave];
		delete bw_redirector[num_slave];
	}

	for(num_master = 0; num_master < CONFIG::NUM_MASTERS; num_master++)
	{
		delete dcr_slave_sock[num_master];
		delete fw_redirector[num_master];
	}
}

template <class CONFIG>
bool DCRController<CONFIG>::BeginSetup()
{
	if(cycle_time == sc_core::SC_ZERO_TIME)
	{
		inherited::logger << DebugError << "Parameter " << param_cycle_time.GetName() << " must be > " << sc_core::SC_ZERO_TIME << EndDebugError;
		return false;
	}
	return true;
}

template <class CONFIG>
void DCRController<CONFIG>::b_transport(unsigned int num_master, tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	if(num_master < CONFIG::NUM_MASTERS)
	{
		sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
		notify_time_stamp += t;
		Event *event = schedule.AllocEvent();
		sc_core::sc_event ev_completed;
		int intf = inherited::GetMasterInterface(num_master);
		event->Initialize(&payload, intf, notify_time_stamp, &ev_completed);
		schedule.Notify(event);
		wait(ev_completed);
		t = sc_core::SC_ZERO_TIME;
	}
	else
	{
		inherited::logger << DebugError << "Internal error" << EndDebugError;
		Object::Stop(-1);
	}
}

template <class CONFIG>
tlm::tlm_sync_enum DCRController<CONFIG>::nb_transport_fw(unsigned int num_master, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			if(num_master < CONFIG::NUM_MASTERS)
			{
				sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
				notify_time_stamp += t;
				Event *event = schedule.AllocEvent();
				int intf = inherited::GetMasterInterface(num_master);
				event->Initialize(&payload, intf, notify_time_stamp);
				schedule.Notify(event);
				phase = tlm::END_REQ;
				return tlm::TLM_UPDATED;
			}
			
			inherited::logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
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
unsigned int DCRController<CONFIG>::transport_dbg(unsigned int num_slave, tlm::tlm_generic_payload& payload)
{
	return 0;
}

template <class CONFIG>
bool DCRController<CONFIG>::get_direct_mem_ptr(unsigned int num_slave, tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address((sc_dt::uint64) -1);
	return false;
}

template <class CONFIG>
tlm::tlm_sync_enum DCRController<CONFIG>::nb_transport_bw(unsigned int num_slave, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_RESP:
			if(num_slave < CONFIG::NUM_SLAVES)
			{
				sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
				notify_time_stamp += t;
				Event *event = schedule.AllocEvent();
				int intf = inherited::GetSlaveInterface(num_slave);
				event->Initialize(&payload, intf, notify_time_stamp);
				schedule.Notify(event);
				return tlm::TLM_COMPLETED;
			}
			inherited::logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
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
void DCRController<CONFIG>::invalidate_direct_mem_ptr(unsigned int num_slave, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

template <class CONFIG>
void DCRController<CONFIG>::ProcessEvents()
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

			int intf = event->GetInterface();
			
			if((intf >= 0) && inherited::IsMasterInterface(intf)) // not an event from a master ?
			{
				unsigned int num_master = inherited::GetMaster(intf);
				if(num_master >= CONFIG::NUM_MASTERS)
				{
					inherited::logger << DebugError << "Internal error: in " << __FUNCTION__ << ", can't find master" << EndDebugError;
					Object::Stop(-1);
					return;
				}
				
				switch(num_master)
				{
					case CONFIG::C440_MASTER_NUM:
						if(inherited::GetDCRControllerStatusAndControlRegister_XM_ALOCK() || inherited::GetDCRControllerStatusAndControlRegister_XM_LOCK())
						{
							// XM master has locked bus
							return;
						}
						break;
					case CONFIG::XM_MASTER_NUM:
						if(inherited::GetDCRControllerStatusAndControlRegister_C440_ALOCK() || inherited::GetDCRControllerStatusAndControlRegister_C440_LOCK())
						{
							// C440 master has locked bus
							return;
						}
						break;
				}
			}
			
			if((intf < 0) || inherited::IsMasterInterface(intf)) // internal event or event from a master
			{
				ProcessForwardEvent(event);
			}
			else // event from a target
			{
				ProcessBackwardEvent(event);
			}
		}
		while((event = schedule.GetNextEvent()) != 0);
	}
}

template <class CONFIG>
void DCRController<CONFIG>::Process()
{
	if(threaded_model)
	{
		while(1)
		{
			wait(schedule.GetKernelEvent());
			ProcessEvents();
		}
	}
	else
	{
		ProcessEvents();
		next_trigger(schedule.GetKernelEvent());
	}
}

template <class CONFIG>
void DCRController<CONFIG>::ProcessForwardEvent(Event *event)
{
	int src_if = event->GetInterface();
	
	unsigned int num_master;

	if(src_if >= 0)
	{
		num_master = inherited::GetMaster(src_if);
					
		if(num_master >= CONFIG::NUM_MASTERS)
		{
			inherited::logger << DebugError << "Internal error: in " << __FUNCTION__ << ", can't find master" << EndDebugError;
			Object::Stop(-1);
			return;
		}
	}
	
	tlm::tlm_generic_payload *payload = event->GetPayload();
	
	uint32_t addr = payload->get_address();
	unsigned int data_length = payload->get_data_length();
	unsigned int byte_enable_length = payload->get_byte_enable_length();
	unsigned int streaming_width = payload->get_streaming_width();
	
	if((addr & 3) != 0)
	{
		// only 32-bit aligned accesses are supported
		inherited::logger << DebugError << event->GetTimeStamp()
			<< ": DCR address 0x" << std::hex << addr << std::dec << " is not 32-bit aligned"
			<< EndDebugError;
		Object::Stop(-1);
		return;
	}
	
	if(data_length != 4)
	{
		inherited::logger << DebugError << "data length of " << data_length << " bytes is unsupported" << EndDebugError;
		Object::Stop(-1);
		return;
	}
	
	if(byte_enable_length)
	{
		inherited::logger << DebugError << "byte enable is unsupported" << EndDebugError;
		Object::Stop(-1);
		return;
	}
	
	if(streaming_width && (streaming_width != data_length))
	{
		inherited::logger << DebugError << "streaming width of " << data_length << " bytes is unsupported" << EndDebugError;
		Object::Stop(-1);
		return;
	}

	uint32_t dcrn = addr / 4;
	
	switch(dcrn)
	{
		case inherited::DCR_CONTROLLER_BASEADDR + CONFIG::INDIRECT_MODE_ADDRESS_REGISTER:
		case inherited::DCR_CONTROLLER_BASEADDR + CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER:
			{
				if(src_if < 0)
				{
					payload = ResolveIndirectAccess(payload, num_master);
					inherited::ResetALOCK(num_master);
				}
				
				tlm::tlm_command cmd = payload->get_command();
				
				switch(cmd)
				{
					case tlm::TLM_READ_COMMAND:
						{
							uint32_t value;
							inherited::Read(dcrn, value);
							value = unisim::util::endian::Host2BigEndian(value);
							memcpy(payload->get_data_ptr(), &value, 4);
						}
						break;
					case tlm::TLM_WRITE_COMMAND:
						{
							uint32_t value;
							memcpy(&value, payload->get_data_ptr(), 4);
							value = unisim::util::endian::BigEndian2Host(value);
							inherited::Write(num_master, dcrn, value);
						}
						break;
					default:
						inherited::logger << DebugError << "protocol error: unexpected command" << EndDebugError;
						Object::Stop(-1);
				}

				sc_core::sc_time t(cycle_time);
				sc_core::sc_event *ev_completed = event->GetCompletionEvent();
				
				if(ev_completed)
				{
					ev_completed->notify(t);
				}
				else
				{
					tlm::tlm_phase phase = tlm::BEGIN_RESP;
					
					(*dcr_slave_sock[num_master])->nb_transport_bw(*payload, phase, t);
				}
				if(src_if < 0)
				{
					payload->release(); // Payload has been acquired by BindIndirectAccess
				}
				
				schedule.FreeEvent(event);
			}
			return;

		case inherited::DCR_CONTROLLER_BASEADDR + CONFIG::INDIRECT_MODE_ACCESS_REGISTER:
			{
				uint32_t dcrn = inherited::GetIndirectModeAddressRegister();
				tlm::tlm_generic_payload *indirect_payload = payload_fabric.allocate();
				indirect_payload->set_data_ptr(0); // avoid deep_copy_from from copying existing data that anyway no longer exist
				indirect_payload->deep_copy_from(*payload);
				indirect_payload->set_data_ptr(payload->get_data_ptr());
				indirect_payload->set_address(4 * dcrn);
				
				if((dcrn != (inherited::DCR_CONTROLLER_BASEADDR + CONFIG::INDIRECT_MODE_ADDRESS_REGISTER)) &&
				   (dcrn != (inherited::DCR_CONTROLLER_BASEADDR + CONFIG::INDIRECT_MODE_ACCESS_REGISTER)))
				{
					sc_core::sc_time t(cycle_time);
					sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
					notify_time_stamp += t;
					Event *indirect_event = schedule.AllocEvent();
					int intf = -1;
					sc_core::sc_event *ev_completed = event->GetCompletionEvent();
					indirect_event->Initialize(indirect_payload, intf, notify_time_stamp, ev_completed);
					schedule.Notify(indirect_event);
					BindIndirectAccess(payload, indirect_payload, num_master);
					
					if(inherited::IsVerbose())
					{
						inherited::logger << DebugInfo << "indirect access to DCR #0x" << std::hex << dcrn << std::dec << EndDebugInfo;
					}
				}
				else
				{
					inherited::logger << DebugWarning << "indirect access to DCR #0x" << std::hex << dcrn << std::dec << " is illegal" << EndDebugWarning;
					DoTimeOutAccess(num_master, event->GetCompletionEvent(), payload);
				}
				
				indirect_payload->release();
			}
			schedule.FreeEvent(event);
			return;
	}

	int dst_if = inherited::Route(dcrn);
	
	if(dst_if < 0)
	{
		// Unmapped access
		DoTimeOutAccess(num_master, event->GetCompletionEvent(), payload);
		schedule.FreeEvent(event);
		return;
	}

	unsigned int num_slave = inherited::GetSlave(dst_if);
	if(num_slave >= CONFIG::NUM_SLAVES)
	{
		inherited::logger << DebugError << "Internal error: in " << __FUNCTION__ << ", can't find slave" << EndDebugError;
		Object::Stop(-1);
		return;
	}

	if(inherited::IsVerbose())
	{
		unsigned int num_slave = inherited::GetSlave(dst_if);
		if(src_if < 0)
		{
			inherited::logger << DebugInfo << "Routing (indirect access, DCR #0x" << std::hex << dcrn << std::dec << ") -> (slave #"
				<< num_slave << ", DCR #0x" << std::hex << dcrn << std::dec << ")" << EndDebugInfo;
		}
		else
		{
			unsigned int num_master = inherited::GetMaster(src_if);
			inherited::logger << DebugInfo << "Routing (master #"
				<< num_master << ", DCR #0x" << std::hex << dcrn << std::dec << ") -> (slave #"
				<< num_slave << ", DCR #0x" << std::hex << dcrn << std::dec << ")" << EndDebugInfo;
		}
	}

	sc_core::sc_time t(cycle_time);

	sc_core::sc_event *ev_completed = event->GetCompletionEvent();
	tlm::tlm_phase phase = tlm::BEGIN_REQ;
	tlm::tlm_sync_enum sync = tlm::TLM_ACCEPTED;

	sync = (*dcr_master_sock[num_slave])->nb_transport_fw(*payload, phase, t);

	if(sync == tlm::TLM_COMPLETED)
	{
		if(src_if < 0)
		{
			// indirect access
			tlm::tlm_generic_payload *original_payload = ResolveIndirectAccess(payload, num_master);

			if(payload->get_response_status() != tlm::TLM_OK_RESPONSE)
			{
				// Unmapped access or slave error results in timeout
				DoTimeOutAccess(num_master, ev_completed, original_payload);
			}
			else if(ev_completed)
			{
				ev_completed->notify(t);
			}
			else
			{
				phase = tlm::BEGIN_RESP;
				(*dcr_slave_sock[num_master])->nb_transport_bw(*original_payload, phase, t);
			}
		}
		else
		{
			if(payload->get_response_status() != tlm::TLM_OK_RESPONSE)
			{
				DoTimeOutAccess(num_master, ev_completed, payload);
			}
			else if(ev_completed)
			{
				ev_completed->notify(t);
			}
			else
			{
				phase = tlm::BEGIN_RESP;
				(*dcr_slave_sock[num_master])->nb_transport_bw(*payload, phase, t);
			}
		}
		
		schedule.FreeEvent(event);
	}
	else
	{
		payload->acquire();
		pending_requests.insert(std::pair<tlm::tlm_generic_payload *, Event *>(payload, event));
	}
}

template <class CONFIG>
void DCRController<CONFIG>::ProcessBackwardEvent(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	typename std::map<tlm::tlm_generic_payload *, Event *>::iterator it = pending_requests.find(payload);
	if(it == pending_requests.end())
	{
		inherited::logger << DebugError << "Internal error: in " << __FUNCTION__ << ", can't find original event" << EndDebugError;
		Object::Stop(-1);
		return;
	}
	
	schedule.FreeEvent(event);
	event = (*it).second;
	pending_requests.erase(it);
	int src_if = event->GetInterface();
	sc_core::sc_event *ev_completed = event->GetCompletionEvent();
	
	unsigned int num_master;
	
	if(src_if < 0)
	{
		// indirect access
		tlm::tlm_response_status status = payload->get_response_status();
		
		payload = ResolveIndirectAccess(payload, num_master);
		
		payload->set_response_status(status);
		
		inherited::ResetALOCK(num_master);
	}

	if(payload->get_response_status() != tlm::TLM_OK_RESPONSE)
	{
		DoTimeOutAccess(num_master, 0, payload);
	}
	else
	{
		sc_core::sc_time t(cycle_time);
		
		if(ev_completed)
		{
			ev_completed->notify(t);
		}
		else
		{
			tlm::tlm_phase phase = tlm::BEGIN_RESP;
			(*dcr_slave_sock[num_master])->nb_transport_bw(*payload, phase, t);
		}
	}
	
	payload->release(); // payload has been acquired by ProcessForwardEvent
	
	schedule.FreeEvent(event);
}

template <class CONFIG>
void DCRController<CONFIG>::DoTimeOutAccess(unsigned int num_master, sc_core::sc_event *ev_completed, tlm::tlm_generic_payload *payload)
{
	uint32_t addr = payload->get_address();
	uint32_t dcrn = addr / 4;

	// Unmapped access
	inherited::logger << DebugWarning << "Access to DCR #0x" << dcrn << " results in timeout" << EndDebugWarning;

	switch(num_master)
	{
		case CONFIG::C440_MASTER_NUM:
			inherited::SetDCRControllerStatusAndControlRegister_C440_TIMEOUT();
			break;
		case CONFIG::XM_MASTER_NUM:
			inherited::SetDCRControllerStatusAndControlRegister_XM_TIMEOUT();
			break;
	}
	
	if(payload->get_command() == tlm::TLM_READ_COMMAND)
	{
		memset(payload->get_data_ptr(), 0, payload->get_data_length());
	}
	
	payload->set_response_status(tlm::TLM_OK_RESPONSE);

	sc_core::sc_time t(cycle_time);
	
	if(ev_completed)
	{
		ev_completed->notify(t);
	}
	else
	{
		tlm::tlm_phase phase = tlm::BEGIN_RESP;
		
		(*dcr_slave_sock[num_master])->nb_transport_bw(*payload, phase, t);
	}
}

template <class CONFIG>
void DCRController<CONFIG>::BindIndirectAccess(tlm::tlm_generic_payload *original_payload, tlm::tlm_generic_payload *payload, unsigned int num_master)
{
	indirect_access_master.insert(std::pair<tlm::tlm_generic_payload *, unsigned int>(payload, num_master));
	original_payload->acquire();
	indirect_access_payload_binding.insert(std::pair<tlm::tlm_generic_payload *, tlm::tlm_generic_payload *>(payload, original_payload));
}

template <class CONFIG>
tlm::tlm_generic_payload *DCRController<CONFIG>::ResolveIndirectAccess(tlm::tlm_generic_payload *payload, unsigned int& num_master)
{
	// Try to find an indirect access
	typename std::map<tlm::tlm_generic_payload *, tlm::tlm_generic_payload *>::iterator it_binding = indirect_access_payload_binding.find(payload);
	if(it_binding == indirect_access_payload_binding.end())
	{
		// No indirect access found
		inherited::logger << DebugError << "Internal error: in " << __FUNCTION__ << ", can't find original payload of an indirect access" << EndDebugError;
		Object::Stop(-1);
		return 0;
	}
	
	// original payload found
	tlm::tlm_generic_payload *original_payload = (*it_binding).second;
	indirect_access_payload_binding.erase(it_binding);
	payload->release();

	// Try to find return interface
	typename std::map<tlm::tlm_generic_payload *, unsigned int>::iterator it = indirect_access_master.find(payload);
	if(it == indirect_access_master.end())
	{
		inherited::logger << DebugError << "Internal error: in " << __FUNCTION__ << ", can't find master" << EndDebugError;
		Object::Stop(-1);
		return 0;
	}
	num_master = (*it).second;
	indirect_access_master.erase(it);
	
	return original_payload; // Note: this is the responsability of the caller to release original_payload
}

} // end of namespace dcr_controller
} // end of namespace xilinx
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_DCR_CONTROLLER_DCR_CONTROLLER_TCC__
