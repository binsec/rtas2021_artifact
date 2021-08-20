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

#ifndef __UNISIM_COMPONENT_TLM2_TIMER_XILINX_XPS_TIMER_XPS_TIMER_TCC__
#define __UNISIM_COMPONENT_TLM2_TIMER_XILINX_XPS_TIMER_XPS_TIMER_TCC__

#include <systemc>
#include <unisim/component/cxx/timer/xilinx/xps_timer/xps_timer.hh>
#include <math.h>

#define LOCATION __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "

namespace unisim {
namespace component {
namespace tlm2 {
namespace timer {
namespace xilinx {
namespace xps_timer {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
	
template <class CONFIG>
XPS_Timer<CONFIG>::XPS_Timer(const sc_core::sc_module_name& name, Object *parent)
	: Object(name, parent)
	, sc_core::sc_module(name)
	, unisim::component::cxx::timer::xilinx::xps_timer::XPS_Timer<CONFIG>(name, parent)
	, slave_sock("slave-sock")
	, pwm_master_sock("pwm-master-sock")
	, cycle_time()
	, interrupt_output(false)
	, pwm_output(false)
	, tcr0_roll_over(false)
	, tcr1_roll_over(false)
	, param_cycle_time("cycle-time", this, cycle_time, "Cycle time")
	, pwm_redirector()
	, interrupt_redirector()
	, generate_out_payload_fabric()
	, pwm_payload_fabric()
	, schedule()
{
	capture_trigger_input[0] = false;
	capture_trigger_input[1] = false;
	generate_output[0] = false;
	generate_output[1] = false;
	last_generate_output_time_stamp[0] = sc_core::SC_ZERO_TIME;
	last_generate_output_time_stamp[1] = sc_core::SC_ZERO_TIME;
	last_timer_counter_update_time_stamp[0] = sc_core::SC_ZERO_TIME;
	last_timer_counter_update_time_stamp[1] = sc_core::SC_ZERO_TIME;

	slave_sock(*this); // Bind socket to implementer of interface
	
	unsigned int channel;
	for(channel = 0; channel < CONFIG::NUM_TIMERS; channel++)
	{
		std::stringstream capture_trigger_slave_sock_name_sstr;
		capture_trigger_slave_sock_name_sstr << "capture-trigger-slave-sock" << channel;
		
		capture_trigger_slave_sock[channel] = new capture_trigger_slave_socket(capture_trigger_slave_sock_name_sstr.str().c_str());

		capture_trigger_redirector[channel] = 
			new unisim::kernel::tlm2::FwRedirector<XPS_Timer<CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
				channel,
				this,
				&XPS_Timer<CONFIG>::capture_trigger_nb_transport_fw,
				&XPS_Timer<CONFIG>::capture_trigger_b_transport,
				&XPS_Timer<CONFIG>::capture_trigger_transport_dbg,
				&XPS_Timer<CONFIG>::capture_trigger_get_direct_mem_ptr
			);
		
		(*capture_trigger_slave_sock[channel])(*capture_trigger_redirector[channel]);
	}

	for(channel = 0; channel < CONFIG::NUM_TIMERS; channel++)
	{
		std::stringstream generate_out_master_sock_name_sstr;
		generate_out_master_sock_name_sstr << "generate-out-master-sock" << channel;
		
		generate_out_master_sock[channel] = new generate_out_master_socket(generate_out_master_sock_name_sstr.str().c_str());

		generate_out_redirector[channel] = 
			new unisim::kernel::tlm2::BwRedirector<XPS_Timer<CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
				channel,
				this,
				&XPS_Timer<CONFIG>::generate_out_nb_transport_bw,
				&XPS_Timer<CONFIG>::generate_out_invalidate_direct_mem_ptr
			);
		
		(*generate_out_master_sock[channel])(*generate_out_redirector[channel]);
	}

	pwm_redirector = 
		new unisim::kernel::tlm2::BwRedirector<XPS_Timer<CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
			0,
			this,
			&XPS_Timer<CONFIG>::pwm_nb_transport_bw,
			&XPS_Timer<CONFIG>::pwm_invalidate_direct_mem_ptr
		);
	pwm_master_sock(*pwm_redirector);
	
	interrupt_redirector = 
		new unisim::kernel::tlm2::BwRedirector<XPS_Timer<CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> >(
			0,
			this,
			&XPS_Timer<CONFIG>::interrupt_nb_transport_bw,
			&XPS_Timer<CONFIG>::interrupt_invalidate_direct_mem_ptr
		);
	interrupt_master_sock(*interrupt_redirector);
	
	SC_HAS_PROCESS(XPS_Timer);

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
XPS_Timer<CONFIG>::~XPS_Timer()
{
	unsigned int channel;
	for(channel = 0; channel < CONFIG::NUM_TIMERS; channel++)
	{
		delete capture_trigger_slave_sock[channel];
		delete capture_trigger_redirector[channel];
	}

	for(channel = 0; channel < CONFIG::NUM_TIMERS; channel++)
	{
		delete generate_out_master_sock[channel];
		delete generate_out_redirector[channel];
	}

	delete pwm_redirector;
	delete interrupt_redirector;
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::BeginSetup()
{
	if(cycle_time == sc_core::SC_ZERO_TIME)
	{
		inherited::logger << DebugError << param_cycle_time.GetName() << " must be > " << sc_core::SC_ZERO_TIME << EndDebugError;
		return false;
	}
	return inherited::BeginSetup();
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address((sc_dt::uint64) -1);
	return false;
}

template <class CONFIG>
unsigned int XPS_Timer<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
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
tlm::tlm_sync_enum XPS_Timer<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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
void XPS_Timer<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
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
void XPS_Timer<CONFIG>::capture_trigger_b_transport(unsigned int channel, CaptureTriggerPayload& payload, sc_core::sc_time& t)
{
	if((!CONFIG::C_ONE_TIMER_ONLY && (channel < 2)) || (channel == 0))
	{
		bool level = payload.GetValue();
		if((((channel == 0) ? CONFIG::C_TRIG0_ASSERT : CONFIG::C_TRIG1_ASSERT) && !capture_trigger_input[channel] && level) ||
		(((channel == 0) ? !CONFIG::C_TRIG0_ASSERT : !CONFIG::C_TRIG1_ASSERT) && capture_trigger_input[channel] && !level))
		{
			// capture event
			sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
			notify_time_stamp += t;

			AlignToClock(notify_time_stamp);
			Event *event = schedule.AllocEvent();
			event->InitializeCaptureTriggerEvent(channel, notify_time_stamp);
			schedule.Notify(event);
		}
		capture_trigger_input[channel] = level;
	}
	else
	{
		inherited::logger << DebugError << "protocol error (invalid channel number)" << EndDebugError;
		Object::Stop(-1);
	}
}

template <class CONFIG>
tlm::tlm_sync_enum XPS_Timer<CONFIG>::capture_trigger_nb_transport_fw(unsigned int channel, CaptureTriggerPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				if((!CONFIG::C_ONE_TIMER_ONLY && (channel < 2)) || (channel == 0))
				{
					bool level = payload.GetValue();
					if((((channel == 0) ? CONFIG::C_TRIG0_ASSERT : CONFIG::C_TRIG1_ASSERT) && !capture_trigger_input[channel] && level) ||
					(((channel == 0) ? !CONFIG::C_TRIG0_ASSERT : !CONFIG::C_TRIG1_ASSERT) && capture_trigger_input[channel] && !level))
					{
						// capture event
						sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
						notify_time_stamp += t;

						AlignToClock(notify_time_stamp);
						Event *event = schedule.AllocEvent();
						event->InitializeCaptureTriggerEvent(channel, notify_time_stamp);
						schedule.Notify(event);
					}
					capture_trigger_input[channel] = level;
				}
				else
				{
					inherited::logger << DebugError << "protocol error (invalid channel number)" << EndDebugError;
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
unsigned int XPS_Timer<CONFIG>::capture_trigger_transport_dbg(unsigned int channel, CaptureTriggerPayload& payload)
{
	return 0;
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::capture_trigger_get_direct_mem_ptr(unsigned int channel, CaptureTriggerPayload& payload, tlm::tlm_dmi& dmi_data)
{
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address((sc_dt::uint64) -1);
	return false;
}

template <class CONFIG>
tlm::tlm_sync_enum XPS_Timer<CONFIG>::generate_out_nb_transport_bw(unsigned int channel, GenerateOutPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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
void XPS_Timer<CONFIG>::generate_out_invalidate_direct_mem_ptr(unsigned int channel, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

template <class CONFIG>
tlm::tlm_sync_enum XPS_Timer<CONFIG>::pwm_nb_transport_bw(unsigned int channel, PWMPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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
void XPS_Timer<CONFIG>::pwm_invalidate_direct_mem_ptr(unsigned int channel, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

template <class CONFIG>
tlm::tlm_sync_enum XPS_Timer<CONFIG>::interrupt_nb_transport_bw(unsigned int channel, InterruptPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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
void XPS_Timer<CONFIG>::interrupt_invalidate_direct_mem_ptr(unsigned int channel, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

template <class CONFIG>
void XPS_Timer<CONFIG>::ProcessEvents()
{
	time_stamp = sc_core::sc_time_stamp();
	if(inherited::IsVerbose())
	{
		inherited::logger << DebugInfo << time_stamp << ": Waking up" << EndDebugInfo;
	}
	RunCounters(); // Compute the timer/counter values at the event time stamp
	
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
				case Event::EV_WAKE_UP:
					// Nothing to do
					schedule.FreeEvent(event);
					break;
				case Event::EV_LOAD:
					ProcessLoadEvent(event);
					schedule.FreeEvent(event);
					break;
				case Event::EV_CAPTURE_TRIGGER:
					ProcessCaptureTriggerEvent(event);
					schedule.FreeEvent(event);
					break;
				case Event::EV_CPU:
					if(time_stamp >= ready_time_stamp)
					{
						ProcessCPUEvent(event);
						schedule.FreeEvent(event);
					}
					else
					{
						// delay the CPU event later
						// Note: this doesn't work if more than one CPU event is scheduled
						event->SetTimeStamp(ready_time_stamp);
						schedule.Notify(event);
					}
					break;
			}
			
		}
		while((event = schedule.GetNextEvent()) != 0);
	}
		
	Update();              // Update state
	GenerateOutput();      // Generate "generate out", PWM and interrupt signals
}

template <class CONFIG>
void XPS_Timer<CONFIG>::Process()
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
void XPS_Timer<CONFIG>::ProcessLoadEvent(Event *event)
{
	switch(event->GetChannel())
	{
		case 0:
			if(inherited::IsVerbose())
			{
				inherited::logger << DebugInfo << time_stamp << ": Processing a TCR0 load/reload (TCR0 <- 0x" << std::hex << inherited::GetTLR0() << std::dec << ")" << EndDebugInfo;
			}
			inherited::SetTCR0(inherited::GetTLR0());
			break;
		case 1:
			if(inherited::IsVerbose())
			{
				inherited::logger << DebugInfo << time_stamp << ": Processing a TCR1 load/reload (TCR1 <- 0x" << std::hex << inherited::GetTLR1() << std::dec << ")" << EndDebugInfo;
			}
			inherited::SetTCR1(inherited::GetTLR1());
			break;
	}
}

template <class CONFIG>
void XPS_Timer<CONFIG>::ProcessCaptureTriggerEvent(Event *event)
{
	switch(event->GetChannel())
	{
		case 0:
			if(inherited::IsVerbose())
			{
				inherited::logger << DebugInfo << time_stamp << ": Processing a capture trigger event for timer 0" << EndDebugInfo;
			}
			inherited::CaptureTrigger0();
			break;
		case 1:
			if(inherited::IsVerbose())
			{
				inherited::logger << DebugInfo << time_stamp << ": Processing a capture trigger event for timer 1" << EndDebugInfo;
			}
			inherited::CaptureTrigger1();
			break;
	}
}

template <class CONFIG>
void XPS_Timer<CONFIG>::ProcessCPUEvent(Event *event)
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
		else if((data_length != 4))
		{
			// only data length of 4 bytes is supported
			inherited::logger << DebugWarning << LOCATION
				<< time_stamp
				<< ": data length of " << data_length << " bytes is unsupported"
				<< EndDebugWarning;
			status = tlm::TLM_BURST_ERROR_RESPONSE;
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

					uint32_t value;
					inherited::Read(addr, value);
					memcpy(data_ptr, &value, 4);
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
					
					uint32_t value;
					memcpy(&value, data_ptr, 4);
					inherited::Write(addr, value);
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
void XPS_Timer<CONFIG>::GenerateOutput()
{
	if(!CONFIG::C_ONE_TIMER_ONLY && !inherited::GetTCSR0_MDT0() && !inherited::GetTCSR1_MDT1() && inherited::GetTCSR0_PWMA0() && inherited::GetTCSR1_PWMB0())
	{
		// PWM mode
		if(tcr0_roll_over && !pwm_output)
		{
			// PWM goes from low to high
			if(inherited::IsVerbose())
			{
				inherited::logger << DebugInfo << time_stamp << ": PWM0 signal goes high" << EndDebugInfo;
			}
			PWMPayload *pwm_payload = pwm_payload_fabric.allocate();

			pwm_payload->SetValue(true);
			
			sc_core::sc_time t(sc_core::SC_ZERO_TIME);
			//pwm_master_sock->b_transport(*pwm_payload, t);
			
			tlm::tlm_phase phase = tlm::BEGIN_REQ;
			pwm_master_sock->nb_transport_fw(*pwm_payload, phase, t);
			
			pwm_payload->release();
			
			pwm_output = true;
		}
		else if(tcr1_roll_over && pwm_output)
		{
			// PWM goes from high to low
			if(inherited::IsVerbose())
			{
				inherited::logger << DebugInfo << time_stamp << ": PWM0 signal goes low" << EndDebugInfo;
			}
			PWMPayload *pwm_payload = pwm_payload_fabric.allocate();

			pwm_payload->SetValue(false);
			
			sc_core::sc_time t(sc_core::SC_ZERO_TIME);
			//pwm_master_sock->b_transport(*pwm_payload, t);
			tlm::tlm_phase phase = tlm::BEGIN_REQ;
			pwm_master_sock->nb_transport_fw(*pwm_payload, phase, t);
			
			pwm_payload->release();
			
			pwm_output = false;
		}
	}
	else
	{
		if(!inherited::GetTCSR0_MDT0())
		{
			// Generate mode (channel 0)
			if(tcr0_roll_over && (generate_output[0] != CONFIG::C_GEN0_ASSERT))
			{
				// Start generating a pulse for one clock cycle
				if(inherited::IsVerbose())
				{
					inherited::logger << DebugInfo << time_stamp << ": GenerateOut0 signal goes " << (CONFIG::C_GEN0_ASSERT ? "high" : "low") << EndDebugInfo;
				}
				GenerateOutPayload *generate_out_payload = generate_out_payload_fabric.allocate();

				generate_out_payload->SetValue(CONFIG::C_GEN0_ASSERT);
				
				tlm::tlm_phase phase = tlm::BEGIN_REQ;
				sc_core::sc_time t(sc_core::SC_ZERO_TIME);
				/* tlm::tlm_sync_enum sync = */ (*generate_out_master_sock[0])->nb_transport_fw(*generate_out_payload, phase, t);
				
				generate_out_payload->release();
				
				last_generate_output_time_stamp[0] = time_stamp;
				generate_output[0] = CONFIG::C_GEN0_ASSERT;
				
				sc_core::sc_time end_of_pulse_time(time_stamp);
				end_of_pulse_time += cycle_time;
				Event *event = schedule.AllocEvent();
				event->InitializeWakeUpEvent(end_of_pulse_time); // pulse ends after one clock cycle
				schedule.Notify(event);
			}
			else if(generate_output[0] == CONFIG::C_GEN0_ASSERT)
			{
				// A pulse is being generated
				sc_core::sc_time end_of_pulse_time(time_stamp);
				end_of_pulse_time -= last_generate_output_time_stamp[0];
				if(end_of_pulse_time >= cycle_time)
				{
					// End of pulse
					if(inherited::IsVerbose())
					{
						inherited::logger << DebugInfo << time_stamp << ": GenerateOut0 was " << (CONFIG::C_GEN0_ASSERT ? "high" : "low") << " for " << end_of_pulse_time << EndDebugInfo;
						inherited::logger << DebugInfo << time_stamp << ": GenerateOut0 signal goes " << (CONFIG::C_GEN0_ASSERT ? "low" : "high") << EndDebugInfo;
					}

					GenerateOutPayload *generate_out_payload = generate_out_payload_fabric.allocate();

					generate_out_payload->SetValue(!CONFIG::C_GEN0_ASSERT);
					
					tlm::tlm_phase phase = tlm::BEGIN_REQ;
					sc_core::sc_time t(sc_core::SC_ZERO_TIME);
					/* tlm::tlm_sync_enum sync = */ (*generate_out_master_sock[0])->nb_transport_fw(*generate_out_payload, phase, t);
					
					generate_out_payload->release();

					last_generate_output_time_stamp[0] = time_stamp;
					generate_output[0] = !CONFIG::C_GEN0_ASSERT;
				}
			}
		}
		if(!inherited::GetTCSR1_MDT1())
		{
			// Generate mode (channel 1)
			if(tcr1_roll_over && (generate_output[1] != CONFIG::C_GEN1_ASSERT))
			{
				// Start generating a pulse for one clock cycle
				GenerateOutPayload *generate_out_payload = generate_out_payload_fabric.allocate();

				generate_out_payload->SetValue(CONFIG::C_GEN1_ASSERT);
				
				tlm::tlm_phase phase = tlm::BEGIN_REQ;
				sc_core::sc_time t(sc_core::SC_ZERO_TIME);
				/* tlm::tlm_sync_enum sync = */ (*generate_out_master_sock[1])->nb_transport_fw(*generate_out_payload, phase, t);
				
				generate_out_payload->release();
				
				last_generate_output_time_stamp[1] = time_stamp;
				generate_output[1] = CONFIG::C_GEN1_ASSERT;
				
				sc_core::sc_time end_of_pulse_time(time_stamp);
				end_of_pulse_time += cycle_time;
				Event *event = schedule.AllocEvent();
				event->InitializeWakeUpEvent(end_of_pulse_time); // pulse ends after one clock cycle
				schedule.Notify(event);
			}
			else if(generate_output[1] == CONFIG::C_GEN1_ASSERT)
			{
				sc_core::sc_time pulse_time(time_stamp);
				pulse_time -= last_generate_output_time_stamp[1];
				if(pulse_time >= cycle_time)
				{
					// End of pulse
					GenerateOutPayload *generate_out_payload = generate_out_payload_fabric.allocate();

					generate_out_payload->SetValue(!CONFIG::C_GEN1_ASSERT);
					
					tlm::tlm_phase phase = tlm::BEGIN_REQ;
					sc_core::sc_time t(sc_core::SC_ZERO_TIME);
					/* tlm::tlm_sync_enum sync = */ (*generate_out_master_sock[1])->nb_transport_fw(*generate_out_payload, phase, t);
					
					generate_out_payload->release();

					generate_output[1] = !CONFIG::C_GEN1_ASSERT;
				}
			}
		}
	}
	
	bool level = (inherited::GetTCSR0_ENIT0() && inherited::GetTCSR0_T0INT()) || (inherited::GetTCSR1_ENIT1() && inherited::GetTCSR1_T1INT());
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
void XPS_Timer<CONFIG>::AlignToClock(sc_core::sc_time& t)
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

template <class CONFIG>
void XPS_Timer<CONFIG>::RunCounters()
{
	// Note: time_stamp is supposed to be a multiple of cycle_time
	// Update TCR0 according to spent simulation time
	// Also detect TCR0 roll over
	if(inherited::GetTCSR0_ENT0())
	{
		// Timer0 enabled

		// Compute delta time since last update
		sc_core::sc_time delta_time(time_stamp);
		delta_time -= last_timer_counter_update_time_stamp[0];
		double delta = floor(delta_time / cycle_time);
		if(delta > (double) CONFIG::MAX_COUNT)
		{
			inherited::logger << DebugError << time_stamp << ": Internal error: TCR0 count overflow" << EndDebugError;
			Object::Stop(-1);
		}
		uint32_t delta_count = (uint32_t) delta;
		if(inherited::IsVerbose())
		{
			inherited::logger << DebugInfo << time_stamp << ": " << delta_count << " cycles elapsed since last update of TCR0" << EndDebugInfo;
		}

		tcr0_roll_over = inherited::RunCounter0(delta_count);
		
		if(inherited::IsVerbose() && tcr0_roll_over)
		{
			inherited::logger << DebugInfo << time_stamp << ": TCR0 rolls over (0x" << std::hex << inherited::GetTCR0() << std::dec << ")" << EndDebugInfo;
		}
		
		last_timer_counter_update_time_stamp[0] = time_stamp;
	}
	
	if(!CONFIG::C_ONE_TIMER_ONLY)
	{
		// Update TCR1 according to spent simulation time
		// Also detect TCR1 roll over
		if(inherited::GetTCSR1_ENT1())
		{
			// Timer1 enabled
			
			// Compute delta time since last update
			sc_core::sc_time delta_time(time_stamp);
			delta_time -= last_timer_counter_update_time_stamp[1];
			double delta = floor(delta_time / cycle_time);
			if(delta > (double) CONFIG::MAX_COUNT)
			{
				inherited::logger << DebugError << time_stamp << ": Internal error: TCR1 count overflow" << EndDebugError;
				Object::Stop(-1);
			}
			uint32_t delta_count = (uint32_t) delta;
			if(inherited::IsVerbose())
			{
				inherited::logger << DebugInfo << time_stamp << ": " << delta_count << " cycles elapsed since last update of TCR1" << EndDebugInfo;
			}

			tcr1_roll_over = inherited::RunCounter1(delta_count);

			if(inherited::IsVerbose() && tcr1_roll_over)
			{
				inherited::logger << DebugInfo << time_stamp << ": TCR1 rolls over (0x" << std::hex << inherited::GetTCR1() << std::dec << ")" << EndDebugInfo;
			}
			
			last_timer_counter_update_time_stamp[1] = time_stamp;
		}
	}
}

template <class CONFIG>
void XPS_Timer<CONFIG>::Update()
{
	inherited::Update(); // Write back modified registers

	if(inherited::GetTCSR0_ENT0())
	{
		// timer 0 runs
		last_timer_counter_update_time_stamp[0] = time_stamp;
							
		// Notify roll over
		sc_core::sc_time notify_time_stamp(cycle_time);
		notify_time_stamp *= inherited::GetTCSR0_UDT0() ? ((inherited::GetTCR0() != CONFIG::MAX_COUNT) ? inherited::GetTCR0() + 1 : 0) : ((inherited::GetTCR0() != 0) ? CONFIG::MAX_COUNT - inherited::GetTCR0() + 1 : 0);
		notify_time_stamp += time_stamp;
		
		if(notify_time_stamp != time_stamp)
		{
			if(inherited::IsVerbose())
			{
				inherited::logger << DebugInfo << time_stamp << ": TCR0 (0x" << std::hex << inherited::GetTCR0() << std::dec << ") should roll over at " << notify_time_stamp << EndDebugInfo;
			}
			Event *event = schedule.AllocEvent();
			event->InitializeWakeUpEvent(notify_time_stamp); // schedule a wakup when counter should roll over
			schedule.Notify(event);
		}
	}
	
	if(inherited::GetTCSR1_ENT1())
	{
		// timer 1 runs
		last_timer_counter_update_time_stamp[1] = time_stamp;
							
		// Notify roll over
		sc_core::sc_time notify_time_stamp(cycle_time);
		notify_time_stamp *= inherited::GetTCSR1_UDT1() ? ((inherited::GetTCR1() != CONFIG::MAX_COUNT) ? inherited::GetTCR1() + 1 : 0) : ((inherited::GetTCR1() != 0) ? CONFIG::MAX_COUNT - inherited::GetTCR1() + 1 : 0);
		notify_time_stamp += time_stamp;
		if(notify_time_stamp != time_stamp)
		{
			if(inherited::IsVerbose())
			{
				inherited::logger << DebugInfo << time_stamp << ": TCR1 (0x" << std::hex << inherited::GetTCR1() << std::dec << ") should roll over at " << notify_time_stamp << EndDebugInfo;
			}
			Event *event = schedule.AllocEvent();
			event->InitializeWakeUpEvent(notify_time_stamp); // schedule a wakup when counter should roll over
			schedule.Notify(event);
		}
	}
	
	if(inherited::NeedsLoadingTCR0())
	{
		sc_core::sc_time notify_time_stamp(cycle_time);
		notify_time_stamp += time_stamp;
		Event *event = schedule.AllocEvent();
		event->InitializeLoadEvent(0, notify_time_stamp); // schedule a load on next cycle
		schedule.Notify(event);
		if(inherited::IsVerbose())
		{
			inherited::logger << DebugInfo << time_stamp << ": TCR0 will be loaded at " << notify_time_stamp << EndDebugInfo;
		}
	}

	if(inherited::NeedsLoadingTCR1())
	{
		sc_core::sc_time notify_time_stamp(cycle_time);
		notify_time_stamp += time_stamp;
		Event *event = schedule.AllocEvent();
		event->InitializeLoadEvent(1, notify_time_stamp); // schedule a load on next cycle
		schedule.Notify(event);
		if(inherited::IsVerbose())
		{
			inherited::logger << DebugInfo << time_stamp << ": TCR1 will be loaded at " << notify_time_stamp << EndDebugInfo;
		}
	}
}

} // end of namespace xps_timer
} // end of namespace xilinx
} // end of namespace timer
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
