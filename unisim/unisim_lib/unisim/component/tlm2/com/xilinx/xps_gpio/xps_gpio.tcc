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

#ifndef __UNISIM_COMPONENT_TLM2_GPIO_XILINX_XPS_GPIO_XPS_GPIO_TCC__
#define __UNISIM_COMPONENT_TLM2_GPIO_XILINX_XPS_GPIO_XPS_GPIO_TCC__

#include <systemc>
#include <unisim/component/cxx/com/xilinx/xps_gpio/xps_gpio.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace xilinx {
namespace xps_gpio {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
	
template <class CONFIG>
XPS_GPIO<CONFIG>::XPS_GPIO(const sc_core::sc_module_name& name, Object *parent)
	: Object(name, parent)
	, sc_core::sc_module(name)
	, unisim::component::cxx::com::xilinx::xps_gpio::XPS_GPIO<CONFIG>(name, parent)
	, slave_sock("slave-sock")
	, cycle_time()
	, interrupt_output(false)
	, param_cycle_time("cycle-time", this, cycle_time, "Cycle time")
	, interrupt_bw_redirector(0)
	, gpio_payload_fabric()
	, interrupt_payload_fabric()
	, schedule()
{
	unsigned int i;
	for(i = 0; i < inherited::NUM_GPIO_CHANNELS; i++)
	{
		gpio_output_data[i] = 0;
	}

	slave_sock(*this); // Bind socket to implementer of interface
	
	for(i = 0; i < CONFIG::C_GPIO_WIDTH; i++)
	{
		std::stringstream gpio_slave_sock_name_sstr;
		gpio_slave_sock_name_sstr << "gpio-slave-sock" << i;
		
		gpio_slave_sock[i] = new gpio_slave_socket(gpio_slave_sock_name_sstr.str().c_str());

		gpio_fw_redirector[i] = 
			new unisim::kernel::tlm2::FwRedirector<XPS_GPIO<CONFIG>, GPIOProtocolTypes >(
				i,
				this,
				&XPS_GPIO<CONFIG>::gpio_nb_transport_fw,
				&XPS_GPIO<CONFIG>::gpio_b_transport,
				&XPS_GPIO<CONFIG>::gpio_transport_dbg,
				&XPS_GPIO<CONFIG>::gpio_get_direct_mem_ptr
			);
		
		(*gpio_slave_sock[i])(*gpio_fw_redirector[i]);

		std::stringstream gpio_master_sock_name_sstr;
		gpio_master_sock_name_sstr << "gpio-master-sock" << i;
		
		gpio_master_sock[i] = new gpio_master_socket(gpio_master_sock_name_sstr.str().c_str());

		gpio_bw_redirector[i] = 
			new unisim::kernel::tlm2::BwRedirector<XPS_GPIO<CONFIG>, GPIOProtocolTypes >(
				i,
				this,
				&XPS_GPIO<CONFIG>::gpio_nb_transport_bw,
				&XPS_GPIO<CONFIG>::gpio_invalidate_direct_mem_ptr
			);
		
		(*gpio_master_sock[i])(*gpio_bw_redirector[i]);
	}

	for(i = 0; i < CONFIG::C_GPIO2_WIDTH; i++)
	{
		std::stringstream gpio2_slave_sock_name_sstr;
		gpio2_slave_sock_name_sstr << "gpio2-slave-sock" << i;
		
		gpio2_slave_sock[i] = new gpio_slave_socket(gpio2_slave_sock_name_sstr.str().c_str());

		gpio2_fw_redirector[i] = 
			new unisim::kernel::tlm2::FwRedirector<XPS_GPIO<CONFIG>, GPIOProtocolTypes >(
				CONFIG::C_GPIO_WIDTH + i,
				this,
				&XPS_GPIO<CONFIG>::gpio_nb_transport_fw,
				&XPS_GPIO<CONFIG>::gpio_b_transport,
				&XPS_GPIO<CONFIG>::gpio_transport_dbg,
				&XPS_GPIO<CONFIG>::gpio_get_direct_mem_ptr
			);
		
		(*gpio2_slave_sock[i])(*gpio2_fw_redirector[i]);

		std::stringstream gpio2_master_sock_name_sstr;
		gpio2_master_sock_name_sstr << "gpio2-master-sock" << i;
		
		gpio2_master_sock[i] = new gpio_master_socket(gpio2_master_sock_name_sstr.str().c_str());

		gpio2_bw_redirector[i] = 
			new unisim::kernel::tlm2::BwRedirector<XPS_GPIO<CONFIG>, GPIOProtocolTypes >(
				CONFIG::C_GPIO_WIDTH + i,
				this,
				&XPS_GPIO<CONFIG>::gpio_nb_transport_bw,
				&XPS_GPIO<CONFIG>::gpio_invalidate_direct_mem_ptr
			);
		
		(*gpio2_master_sock[i])(*gpio2_bw_redirector[i]);
	}
	
	if(CONFIG::C_INTERRUPT_IS_PRESENT)
	{
		interrupt_master_sock = new interrupt_master_socket("interrupt-master-sock");

		interrupt_bw_redirector = 
			new unisim::kernel::tlm2::BwRedirector<XPS_GPIO<CONFIG>, InterruptProtocolTypes>(
				0,
				this,
				&XPS_GPIO<CONFIG>::interrupt_nb_transport_bw,
				&XPS_GPIO<CONFIG>::interrupt_invalidate_direct_mem_ptr
			);

		(*interrupt_master_sock)(*interrupt_bw_redirector);
	}
	
	SC_HAS_PROCESS(XPS_GPIO);

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
XPS_GPIO<CONFIG>::~XPS_GPIO()
{
	unsigned int i;
	for(i = 0; i < CONFIG::C_GPIO_WIDTH; i++)
	{
		delete gpio_slave_sock[i];
		delete gpio_master_sock[i];
		delete gpio_fw_redirector[i];
		delete gpio_bw_redirector[i];
	}
	for(i = 0; i < CONFIG::C_GPIO2_WIDTH; i++)
	{
		delete gpio2_slave_sock[i];
		delete gpio2_master_sock[i];
		delete gpio2_fw_redirector[i];
		delete gpio2_bw_redirector[i];
	}
	if(CONFIG::C_INTERRUPT_IS_PRESENT)
	{
		delete interrupt_master_sock;
		delete interrupt_bw_redirector;
	}
}

template <class CONFIG>
bool XPS_GPIO<CONFIG>::PinToChannelAndBit(unsigned int pin, unsigned int& channel, unsigned int& bit) const
{
	if(pin < CONFIG::C_GPIO_WIDTH)
	{
		channel = 0;
		bit = pin;
		return true;
	}
	if(pin < (CONFIG::C_GPIO_WIDTH + CONFIG::C_GPIO2_WIDTH))
	{
		channel = 1;
		bit = pin - CONFIG::C_GPIO_WIDTH;
		return true;
	}
	return false;
}

template <class CONFIG>
bool XPS_GPIO<CONFIG>::ChannelAndBitToPin(unsigned int channel, unsigned int bit, unsigned int& pin) const
{
	switch(channel)
	{
		case 0:
			if(bit < CONFIG::C_GPIO_WIDTH)
			{
				pin = bit;
				return true;
			}
			break;
		case 1:
			if(bit < CONFIG::C_GPIO2_WIDTH)
			{
				pin = bit + CONFIG::C_GPIO_WIDTH;
				return true;
			}
			break;
	}
	return false;
}

template <class CONFIG>
bool XPS_GPIO<CONFIG>::BeginSetup()
{
	if(cycle_time == sc_core::SC_ZERO_TIME)
	{
		inherited::logger << DebugError << param_cycle_time.GetName() << " must be > " << sc_core::SC_ZERO_TIME << EndDebugError;
		return false;
	}
	return inherited::BeginSetup();
}

template <class CONFIG>
bool XPS_GPIO<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address((sc_dt::uint64) -1);
	return false;
}

template <class CONFIG>
unsigned int XPS_GPIO<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
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
				inherited::logger << DebugInfo << __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "
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
				inherited::logger << DebugInfo << __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "
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
			inherited::logger << DebugInfo << __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "
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
tlm::tlm_sync_enum XPS_GPIO<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				tlm::tlm_command cmd = payload.get_command();
				
				if(cmd == tlm::TLM_IGNORE_COMMAND)
				{
					inherited::logger << DebugError << __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "
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
void XPS_GPIO<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	tlm::tlm_command cmd = payload.get_command();
			
	if(cmd == tlm::TLM_IGNORE_COMMAND)
	{
		inherited::logger << DebugError << __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "
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
void XPS_GPIO<CONFIG>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// N/A
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::gpio_b_transport(unsigned int pin, GPIOPayload& payload, sc_core::sc_time& t)
{
	bool gpio_pin_value = payload.GetValue();
	// GPIO event
	sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
	notify_time_stamp += t;

	AlignToClock(notify_time_stamp);
	Event *event = schedule.AllocEvent();
	event->InitializeGPIOEvent(pin, gpio_pin_value, notify_time_stamp);
	schedule.Notify(event);
}

template <class CONFIG>
tlm::tlm_sync_enum XPS_GPIO<CONFIG>::gpio_nb_transport_fw(unsigned int pin, GPIOPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				bool gpio_pin_value = payload.GetValue();
				// GPIO event
				sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
				notify_time_stamp += t;

				AlignToClock(notify_time_stamp);
				Event *event = schedule.AllocEvent();
				event->InitializeGPIOEvent(pin, gpio_pin_value, notify_time_stamp);
				schedule.Notify(event);
				
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
unsigned int XPS_GPIO<CONFIG>::gpio_transport_dbg(unsigned int pin, GPIOPayload& payload)
{
	return 0;
}

template <class CONFIG>
bool XPS_GPIO<CONFIG>::gpio_get_direct_mem_ptr(unsigned int pin, GPIOPayload& payload, tlm::tlm_dmi& dmi_data)
{
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address((sc_dt::uint64) -1);
	return false;
}

template <class CONFIG>
tlm::tlm_sync_enum XPS_GPIO<CONFIG>::gpio_nb_transport_bw(unsigned int pin, GPIOPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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
void XPS_GPIO<CONFIG>::gpio_invalidate_direct_mem_ptr(unsigned int pin, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}


template <class CONFIG>
tlm::tlm_sync_enum XPS_GPIO<CONFIG>::interrupt_nb_transport_bw(unsigned int dummy_id, InterruptPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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
void XPS_GPIO<CONFIG>::interrupt_invalidate_direct_mem_ptr(unsigned int dummy_id, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::ProcessEvents()
{
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
				case Event::EV_GPIO:
					ProcessGPIOEvent(event);
					schedule.FreeEvent(event);
					break;
				case Event::EV_CPU:
					ProcessCPUEvent(event);
					schedule.FreeEvent(event);
					break;
			}
			
		}
		while((event = schedule.GetNextEvent()) != 0);
	}
	
	inherited::Update();
	GenerateOutput();      // Generate GPIO, GPIO2, and interrupt output signals
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::Process()
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
void XPS_GPIO<CONFIG>::ProcessGPIOEvent(Event *event)
{
	unsigned int gpio_channel;
	unsigned int gpio_bit;
	unsigned int pin = event->GetGPIOPin();
	
	if(PinToChannelAndBit(pin, gpio_channel, gpio_bit))
	{
		inherited::logger << DebugInfo << time_stamp << ": Processing a GPIO input event on bit #" << gpio_bit << " for channel #" << gpio_channel << EndDebugInfo;
		bool gpio_pin_value = event->GetGPIOPinValue();
		switch(gpio_channel)
		{
			case 0:
				inherited::SetGPIO_DATA(gpio_bit, gpio_pin_value);
				break;
			case 1:
				inherited::SetGPIO2_DATA(gpio_bit, gpio_pin_value);
				break;
		}
	}
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::ProcessCPUEvent(Event *event)
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
			inherited::logger << DebugWarning << __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "
				<< time_stamp
				<< ": unmapped access at 0x" << std::hex << addr << std::dec << " ( " << data_length << " bytes)"
				<< EndDebugWarning;
			status = tlm::TLM_ADDRESS_ERROR_RESPONSE;
		}
		else if(streaming_width && (streaming_width != data_length))
		{
			// streaming is not supported
			inherited::logger << DebugError << __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "
				<< time_stamp
				<< ": streaming width of " << streaming_width << " bytes is unsupported"
				<< EndDebugError;
			Object::Stop(-1);
			return;
		}
		else if(byte_enable_length)
		{
			// byte enable is not supported
			inherited::logger << DebugError << __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "
				<< time_stamp
				<< ": byte enable is unsupported"
				<< EndDebugError;
			Object::Stop(-1);
			return;
		}
		else if((data_length != 4))
		{
			// only data length of 4 bytes is supported
			inherited::logger << DebugWarning << __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "
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
						inherited::logger << DebugInfo << __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "
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
						inherited::logger << DebugInfo << __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "
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
					inherited::logger << DebugInfo << __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "
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
		slave_sock->nb_transport_bw(*payload, phase, t);
	}
	
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::GenerateOutput()
{
	unsigned int channel;
	for(channel = 0; channel < inherited::NUM_GPIO_CHANNELS; channel++)
	{
		uint32_t data = channel ? inherited::GetGPIO2_DATA() : inherited::GetGPIO_DATA();
		uint32_t tri = channel ? inherited::GetGPIO2_TRI() : inherited::GetGPIO_TRI();
		unsigned int i;
		for(i = 0; i < (channel ? CONFIG::C_GPIO2_WIDTH : CONFIG::C_GPIO_WIDTH); i++)
		{
			uint32_t bit_mask = 1 << ((channel ? CONFIG::C_GPIO2_WIDTH : CONFIG::C_GPIO_WIDTH) - 1 - i);
			if(~tri & bit_mask) // tri=0->output, tri=1->input
			{
				if((data ^ gpio_output_data[channel]) & bit_mask)
				{
					bool pin_level = (data & bit_mask) != 0;
					if(inherited::IsVerbose())
					{
						inherited::logger << DebugInfo << time_stamp << ": GPIO data bit #" << i << " signal goes " << (pin_level ? "high" : "low") << EndDebugInfo;
					}
					
					GPIOPayload *gpio_payload = gpio_payload_fabric.allocate();

					gpio_payload->SetValue(pin_level);
					
					sc_core::sc_time t(sc_core::SC_ZERO_TIME);
					tlm::tlm_phase phase = tlm::BEGIN_REQ;
					
					switch(channel)
					{
						case 0:
							(*gpio_master_sock[i])->nb_transport_fw(*gpio_payload, phase, t);
							break;
						case 1:
							(*gpio2_master_sock[i])->nb_transport_fw(*gpio_payload, phase, t);
							break;
						default:
							inherited::logger << DebugError << "Internal error" << EndDebugError;
							Object::Stop(-1);
					}
					gpio_payload->release();
				}
			}
		}
		gpio_output_data[channel] = (gpio_output_data[channel] & tri) | (data & ~tri);
	}
	
	if(CONFIG::C_INTERRUPT_IS_PRESENT)
	{
		bool level = inherited::GetGIER_GLOBAL_INTERRUPT_ENABLE() && ((inherited::GetIP_ISR() & inherited::GetIP_IER()) != 0);
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
			(*interrupt_master_sock)->nb_transport_fw(*interrupt_payload, phase, t);
			
			interrupt_payload->release();
			
			interrupt_output = level;
		}
	}
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::AlignToClock(sc_core::sc_time& t)
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

} // end of namespace xps_gpio
} // end of namespace xilinx
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
