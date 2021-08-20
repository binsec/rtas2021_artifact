/*
 *  Copyright (c) 2011,
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

#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_XPS_UART_LITE_XPS_UART_LITE_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_XPS_UART_LITE_XPS_UART_LITE_TCC__

#include <systemc>
#include <unisim/component/cxx/com/xilinx/xps_uart_lite/xps_uart_lite.hh>

#define LOCATION __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace xilinx {
namespace xps_uart_lite {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
	
template <class CONFIG>
XPS_UARTLite<CONFIG>::XPS_UARTLite(const sc_core::sc_module_name& name, Object *parent)
	: Object(name, parent)
	, sc_core::sc_module(name)
	, unisim::component::cxx::com::xilinx::xps_uart_lite::XPS_UARTLite<CONFIG>(name, parent)
	, slave_sock("slave-sock")
	, interrupt_master_sock("interrupt-master-sock")
	, cycle_time()
	, telnet_refresh_time(10.0, sc_core::SC_MS)
	, interrupt_output(false)
	, param_cycle_time("cycle-time", this, cycle_time, "Cycle time")
	, param_telnet_refresh_time("telnet-refresh-time", this, telnet_refresh_time, "Telnet refresh time")
	, schedule()
{
	slave_sock(*this); // Bind socket to implementer of interface
	interrupt_master_sock(*this);
	
	SC_HAS_PROCESS(XPS_UARTLite);

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
XPS_UARTLite<CONFIG>::~XPS_UARTLite()
{
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::BeginSetup()
{
	if(cycle_time == sc_core::SC_ZERO_TIME)
	{
		inherited::logger << DebugError << param_cycle_time.GetName() << " must be > " << sc_core::SC_ZERO_TIME << EndDebugError;
		return false;
	}
	return inherited::BeginSetup();
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address((sc_dt::uint64) -1);
	return false;
}

template <class CONFIG>
unsigned int XPS_UARTLite<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
{
	payload.set_dmi_allowed(false);

	tlm::tlm_command cmd = payload.get_command();
	uint64_t addr = payload.get_address();
	unsigned char *data_ptr = payload.get_data_ptr();
	unsigned int data_length = payload.get_data_length();
	unsigned char *byte_enable_ptr = payload.get_byte_enable_ptr();
	unsigned int byte_enable_length = byte_enable_ptr ? payload.get_byte_enable_length() : 0;
	unsigned int streaming_width = payload.get_streaming_width();
	bool status = false;

	switch(cmd)
	{
		case tlm::TLM_READ_COMMAND:
			if(inherited::IsVerbose())
			{
				inherited::logger << DebugInfo << LOCATION
					<< sc_core::sc_time_stamp().to_string()
					<< ": received a TLM_READ_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length" << std::endl
					<< EndDebugInfo;
			}

			if(byte_enable_length ||(streaming_width && (streaming_width != data_length)) )
				status = inherited::ReadMemory(addr, data_ptr, data_length, byte_enable_ptr, byte_enable_length, streaming_width);
			else
				status = inherited::ReadMemory(addr, data_ptr, data_length);
			break;
		case tlm::TLM_WRITE_COMMAND:
			if(inherited::IsVerbose())
			{
				inherited::logger << DebugInfo << LOCATION
					<< sc_core::sc_time_stamp()
					<< ": received a TLM_WRITE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length" << std::endl
					<< EndDebugInfo;
			}
			if(byte_enable_length ||(streaming_width && (streaming_width != data_length)) )
				status = inherited::WriteMemory(addr, data_ptr, data_length, byte_enable_ptr, byte_enable_length, streaming_width);
			else
				status = inherited::WriteMemory(addr, data_ptr, data_length);
			break;
		case tlm::TLM_IGNORE_COMMAND:
			// transport_dbg should not receive such a command
			inherited::logger << DebugInfo << LOCATION
					<< sc_core::sc_time_stamp() 
					<< " : received an unexpected TLM_IGNORE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length" << std::endl
					<< EndDebugInfo;
			Object::Stop(-1);
			break;
	}

	if(status)
		payload.set_response_status(tlm::TLM_OK_RESPONSE);
	else
		payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);

	return status ? data_length : 0;
}

template <class CONFIG>
tlm::tlm_sync_enum XPS_UARTLite<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				tlm::tlm_command cmd = payload.get_command();
				
				if(cmd == tlm::TLM_IGNORE_COMMAND)
				{
					inherited::logger << DebugError << LOCATION
							<< (sc_core::sc_time_stamp() + t)
							<< " : received an unexpected TLM_IGNORE_COMMAND payload"
							<< EndDebugError;
					Object::Stop(-1);
					return tlm::TLM_COMPLETED;
				}
				
				sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
				notify_time_stamp += t;
				AlignToClock(notify_time_stamp);
				Event *event = schedule.AllocEvent();
				event->InitializeCPUEvent(&payload, notify_time_stamp);
				schedule.Notify(event);
				phase = tlm::END_REQ;
				return tlm::TLM_UPDATED;
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
void XPS_UARTLite<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	tlm::tlm_command cmd = payload.get_command();
			
	if(cmd == tlm::TLM_IGNORE_COMMAND)
	{
		inherited::logger << DebugError << LOCATION
				<< (sc_core::sc_time_stamp() + t)
				<< " : received an unexpected TLM_IGNORE_COMMAND payload"
				<< EndDebugError;
		Object::Stop(-1);
		return;
	}

	sc_core::sc_event ev_completed;
	sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
	notify_time_stamp += t;
	AlignToClock(notify_time_stamp);
	Event *event = schedule.AllocEvent();
	event->InitializeCPUEvent(&payload, notify_time_stamp, &ev_completed);
	schedule.Notify(event);
	wait(ev_completed);
	t = sc_core::SC_ZERO_TIME;
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// N/A
}

template <class CONFIG>
tlm::tlm_sync_enum XPS_UARTLite<CONFIG>::nb_transport_bw(InterruptPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::END_REQ:
			return tlm::TLM_ACCEPTED;
		case tlm::BEGIN_RESP:
			return tlm::TLM_COMPLETED;
		default:
			inherited::logger << DebugError << "protocol error" << EndDebugError;
			Object::Stop(-1);
			break;
	}
	
	return tlm::TLM_COMPLETED;
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::ProcessEvents()
{
	bool flush_telnet_output = false;
	
	time_stamp = sc_core::sc_time_stamp();
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

			switch(event->GetType())
			{
				case Event::EV_TELNET_IO:
					{
						flush_telnet_output = true;
						schedule.FreeEvent(event);
						// schedule a wake for handling I/O with telnet client
						sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
						notify_time_stamp += telnet_refresh_time;
						AlignToClock(notify_time_stamp);
						event = schedule.AllocEvent();
						event->InitializeTelnetIOEvent(notify_time_stamp);
						schedule.Notify(event);
					}
					break;
				case Event::EV_CPU:
					ProcessCPUEvent(event);
					schedule.FreeEvent(event);
					break;
			}
			
		}
		while((event = schedule.GetNextEvent()) != 0);
	}
	else
	{
		// schedule a first wake for handling I/O with telnet client
		sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
		notify_time_stamp += telnet_refresh_time;
		AlignToClock(notify_time_stamp);
		event = schedule.AllocEvent();
		event->InitializeTelnetIOEvent(notify_time_stamp);
		schedule.Notify(event);
	}
	
	// Handle I/O with telnet client
	inherited::TelnetProcessInput();
	inherited::TelnetProcessOutput(flush_telnet_output);
	
	GenerateOutput();                 // Generate interrupt signal
		
	inherited::ResetTX_FIFO_BecomesEmpty();
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::Process()
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
void XPS_UARTLite<CONFIG>::ProcessCPUEvent(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetCPUPayload();
	tlm::tlm_command cmd = payload->get_command();
	unsigned char *data_ptr = payload->get_data_ptr();
	unsigned int data_length = payload->get_data_length();
	unsigned int byte_enable_length = payload->get_byte_enable_ptr() ? payload->get_byte_enable_length() : 0;
	unsigned int streaming_width = payload->get_streaming_width();
	typename CONFIG::MEMORY_ADDR addr = payload->get_address();

	tlm::tlm_response_status status = tlm::TLM_OK_RESPONSE;
	
	if(cmd != tlm::TLM_IGNORE_COMMAND)
	{
		if(!inherited::IsMapped(addr, data_length))
		{
			inherited::logger << DebugWarning << LOCATION
				<< time_stamp
				<< ": unmapped access at 0x" << std::hex << addr << std::dec << " ( " << data_length << " bytes)"
				<< EndDebugWarning;
			status = tlm::TLM_ADDRESS_ERROR_RESPONSE;
		}
		else if(streaming_width && (streaming_width != data_length))
		{
			// streaming is not supported
			inherited::logger << DebugError << LOCATION
				<< time_stamp
				<< ": streaming width of " << streaming_width << " bytes is unsupported"
				<< EndDebugError;
			Object::Stop(-1);
			return;
		}
		else if(byte_enable_length)
		{
			// byte enable is not supported
			inherited::logger << DebugError << LOCATION
				<< time_stamp
				<< ": byte enable is unsupported"
				<< EndDebugError;
			Object::Stop(-1);
			return;
		}
	}
	
	if(status == tlm::TLM_OK_RESPONSE)
	{
		switch(cmd)
		{
			case tlm::TLM_READ_COMMAND:
				{
					if(inherited::IsVerbose())
					{
						inherited::logger << DebugInfo << LOCATION
							<< time_stamp
							<< ": processing a TLM_READ_COMMAND payload at 0x"
							<< std::hex << addr << std::dec
							<< EndDebugInfo;
					}

					memset(data_ptr, 0, data_length);
					if(!inherited::Read(addr, data_ptr, data_length))
					{
						status = tlm::TLM_GENERIC_ERROR_RESPONSE;
					}
				}
				break;
			case tlm::TLM_WRITE_COMMAND:
				{
					if(inherited::IsVerbose())
					{
						inherited::logger << DebugInfo << LOCATION
							<< time_stamp
							<< ": processing a TLM_WRITE_COMMAND payload at 0x"
							<< std::hex << addr << std::dec
							<< EndDebugInfo;
					}
					
					inherited::Write(addr, data_ptr, data_length);
				}
				break;
			case tlm::TLM_IGNORE_COMMAND:
				if(inherited::IsVerbose())
				{
					inherited::logger << DebugInfo << LOCATION
						<< time_stamp
						<< ": received a TLM_IGNORE_COMMAND payload at 0x"
						<< std::hex << addr << std::dec
						<< EndDebugInfo;
				}
				break;
		}
	}
	
	payload->set_response_status(status);
	payload->set_dmi_allowed(false);

	ready_time_stamp = time_stamp;
	ready_time_stamp += cycle_time;

	sc_core::sc_event *ev_completed = event->GetCompletionEvent();
	if(ev_completed)
	{
		ev_completed->notify(cycle_time);
	}
	else
	{
		sc_core::sc_time t(cycle_time);
		tlm::tlm_phase phase = tlm::BEGIN_RESP;
		/* tlm::tlm_sync_enum sync = */ slave_sock->nb_transport_bw(*payload, phase, t);
	}
	
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::GenerateOutput()
{
	bool level = inherited::HasInterrupt();
	if(interrupt_output != level)
	{
		if(inherited::IsVerbose())
		{
			inherited::logger << DebugInfo << time_stamp << ": Interrupt signal goes " << (level ? "high" : "low") << EndDebugInfo;
		}
		
		InterruptPayload *interrupt_payload = interrupt_payload_fabric.allocate();

		interrupt_payload->SetValue(level);
		
		sc_core::sc_time t(sc_core::SC_ZERO_TIME);
		tlm::tlm_phase phase = tlm::BEGIN_REQ;
		/* tlm::tlm_sync_enum sync = */ interrupt_master_sock->nb_transport_fw(*interrupt_payload, phase, t);
		
		interrupt_payload->release();
		
		interrupt_output = level;
	}
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::AlignToClock(sc_core::sc_time& t)
{
// 	sc_dt::uint64 time_tu = t.value();
// 	sc_dt::uint64 cycle_time_tu = cycle_time.value();
// 	sc_dt::uint64 modulo = time_tu % cycle_time_tu;
// 	if(!modulo) return; // already aligned
// 
// 	time_tu += cycle_time_tu - modulo;
// 	t = sc_time(time_tu, false);

	sc_core::sc_time modulo(t);
	modulo %= cycle_time;
	if(modulo == sc_core::SC_ZERO_TIME) return; // already aligned
	t += cycle_time - modulo;
}

} // end of namespace xps_uart_lite
} // end of namespace xilinx
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
