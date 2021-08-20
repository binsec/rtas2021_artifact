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

#ifndef __UNISIM_COMPONENT_TLM2_GPIO_XILINX_GPIO_LEDs_GPIO_LEDS_TCC__
#define __UNISIM_COMPONENT_TLM2_GPIO_XILINX_GPIO_LEDs_GPIO_LEDS_TCC__

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
	
template <unsigned int NUM_LEDS>
GPIO_LEDs<NUM_LEDS>::GPIO_LEDs(const sc_core::sc_module_name& name, Object *parent)
	: Object(name, parent)
	, sc_core::sc_module(name)
	, Client<LED_Board>(name, parent)
	, led_board_import("led-board-import", this)
	, logger(*this)
	, verbose(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, schedule()
{
	std::stringstream sstr_description;
	sstr_description << "This module implements a " << NUM_LEDS << "-LED board on GPIO." << std::endl;
	
	Object::SetDescription(sstr_description.str().c_str());

	unsigned int i;
	for(i = 0; i < NUM_LEDS; i++)
	{
		led_status[i] = false;
		
		std::stringstream gpio_slave_sock_name_sstr;
		gpio_slave_sock_name_sstr << "gpio-slave-sock" << i;
		
		gpio_slave_sock[i] = new gpio_slave_socket(gpio_slave_sock_name_sstr.str().c_str());

		gpio_fw_redirector[i] = 
			new unisim::kernel::tlm2::FwRedirector<GPIO_LEDs<NUM_LEDS>, GPIOProtocolTypes >(
				i,
				this,
				&GPIO_LEDs<NUM_LEDS>::gpio_nb_transport_fw,
				&GPIO_LEDs<NUM_LEDS>::gpio_b_transport,
				&GPIO_LEDs<NUM_LEDS>::gpio_transport_dbg,
				&GPIO_LEDs<NUM_LEDS>::gpio_get_direct_mem_ptr
			);
		
		(*gpio_slave_sock[i])(*gpio_fw_redirector[i]);

		std::stringstream gpio_master_sock_name_sstr;
		gpio_master_sock_name_sstr << "gpio-master-sock" << i;
		
		gpio_master_sock[i] = new gpio_master_socket(gpio_master_sock_name_sstr.str().c_str());

		gpio_bw_redirector[i] = 
			new unisim::kernel::tlm2::BwRedirector<GPIO_LEDs<NUM_LEDS>, GPIOProtocolTypes >(
				i,
				this,
				&GPIO_LEDs<NUM_LEDS>::gpio_nb_transport_bw,
				&GPIO_LEDs<NUM_LEDS>::gpio_invalidate_direct_mem_ptr
			);
		
		(*gpio_master_sock[i])(*gpio_bw_redirector[i]);
	}
	
	SC_HAS_PROCESS(GPIO_LEDs);

	if(threaded_model)
	{
		SC_THREAD(Process);
	}
	else
	{
		SC_METHOD(Process);
	}
}

template <unsigned int NUM_LEDS>
GPIO_LEDs<NUM_LEDS>::~GPIO_LEDs()
{
	unsigned int i;
	for(i = 0; i < NUM_LEDS; i++)
	{
		delete gpio_slave_sock[i];
		delete gpio_master_sock[i];
		delete gpio_fw_redirector[i];
		delete gpio_bw_redirector[i];
	}
}

template <unsigned int NUM_LEDS>
bool GPIO_LEDs<NUM_LEDS>::IsVerbose() const
{
	return verbose;
}

template <unsigned int NUM_LEDS>
bool GPIO_LEDs<NUM_LEDS>::BeginSetup()
{
	unsigned int i;
	for(i = 0; i < NUM_LEDS; i++) led_status[i] = false;
	
	return true;
}

template <unsigned int NUM_LEDS>
void GPIO_LEDs<NUM_LEDS>::gpio_b_transport(unsigned int pin, GPIOPayload& payload, sc_core::sc_time& t)
{
	bool gpio_pin_value = payload.GetValue();
	// GPIO event
	sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
	notify_time_stamp += t;

	Event *event = schedule.AllocEvent();
	event->InitializeGPIOEvent(pin, gpio_pin_value, notify_time_stamp);
	schedule.Notify(event);
}

template <unsigned int NUM_LEDS>
tlm::tlm_sync_enum GPIO_LEDs<NUM_LEDS>::gpio_nb_transport_fw(unsigned int pin, GPIOPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				bool gpio_pin_value = payload.GetValue();
				// GPIO event
				sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
				notify_time_stamp += t;

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
			logger << DebugError << "protocol error" << EndDebugError;
			Object::Stop(-1);
			break;
	}
	return tlm::TLM_COMPLETED;
}

template <unsigned int NUM_LEDS>
unsigned int GPIO_LEDs<NUM_LEDS>::gpio_transport_dbg(unsigned int pin, GPIOPayload& payload)
{
	return 0;
}

template <unsigned int NUM_LEDS>
bool GPIO_LEDs<NUM_LEDS>::gpio_get_direct_mem_ptr(unsigned int pin, GPIOPayload& payload, tlm::tlm_dmi& dmi_data)
{
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address((sc_dt::uint64) -1);
	return false;
}

template <unsigned int NUM_LEDS>
void GPIO_LEDs<NUM_LEDS>::gpio_invalidate_direct_mem_ptr(unsigned int pin, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}


template <unsigned int NUM_LEDS>
tlm::tlm_sync_enum GPIO_LEDs<NUM_LEDS>::gpio_nb_transport_bw(unsigned int pin, GPIOPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::END_REQ:
			return tlm::TLM_ACCEPTED;
		case tlm::BEGIN_RESP:
			return tlm::TLM_COMPLETED;
		default:
			logger << DebugError << "protocol error" << EndDebugError;
			Object::Stop(-1);
			break;
	}
	
	return tlm::TLM_COMPLETED;
}

template <unsigned int NUM_LEDS>
void GPIO_LEDs<NUM_LEDS>::ProcessEvents()
{
	time_stamp = sc_core::sc_time_stamp();
	if(IsVerbose())
	{
		logger << DebugInfo << time_stamp << ": Waking up" << EndDebugInfo;
	}
	
	Event *event = schedule.GetNextEvent();
	
	if(event)
	{
		do
		{
			if(event->GetTimeStamp() != time_stamp)
			{
				logger << DebugError << "Internal error: unexpected event time stamp (" << event->GetTimeStamp() << " instead of " << time_stamp << ")" << EndDebugError;
				Object::Stop(-1);
			}

			switch(event->GetType())
			{
				case Event::EV_GPIO:
					ProcessGPIOEvent(event);
					schedule.FreeEvent(event);
					break;
			}
			
		}
		while((event = schedule.GetNextEvent()) != 0);
	}
}

template <unsigned int NUM_LEDS>
void GPIO_LEDs<NUM_LEDS>::Process()
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

template <unsigned int NUM_LEDS>
void GPIO_LEDs<NUM_LEDS>::ProcessGPIOEvent(Event *event)
{
	unsigned int pin = event->GetGPIOPin();
	
	if(IsVerbose())
	{
		logger << DebugInfo << time_stamp << ": Processing a GPIO input event on pin #" << pin << EndDebugInfo;
	}
	bool gpio_pin_value = event->GetGPIOPinValue();
	
	if(led_status[pin] != gpio_pin_value)
	{
		logger << DebugInfo << "At " << event->GetTimeStamp() << ", LED #" << pin << " [" << (gpio_pin_value ? "*" : " ") << "]" << EndDebugInfo;
		
		if(led_board_import) led_board_import->SetLEDStatus(pin, gpio_pin_value);
	}
	led_status[pin] = gpio_pin_value;
}

} // end of namespace xps_gpio
} // end of namespace xilinx
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
