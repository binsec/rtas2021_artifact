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

#ifndef __UNISIM_COMPONENT_TLM2_GPIO_XILINX_XPS_GPIO_GPIO_SWITCHES_TCC__
#define __UNISIM_COMPONENT_TLM2_GPIO_XILINX_XPS_GPIO_GPIO_SWITCHES_TCC__

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
	
template <unsigned int NUM_SWITCHES>
GPIO_Switches<NUM_SWITCHES>::GPIO_Switches(const sc_core::sc_module_name& name, Object *parent)
	: Object(name, parent)
	, sc_core::sc_module(name)
	, Client<Keyboard>(name, parent)
	, keyboard_import("keyboard-import", this)
	, logger(*this)
	, verbose(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, polling_period(10, sc_core::SC_MS)
	, param_polling_period("polling-period", this, polling_period, "Polling period of host keyboard")
	, gpio_payload_fabric()
{
	std::stringstream sstr_description;
	sstr_description << "This module implements a " << NUM_SWITCHES << "-switch board (DIP switch or push buttons) on GPIO." << std::endl;
	
	Object::SetDescription(sstr_description.str().c_str());

	unsigned int i;
	for(i = 0; i < NUM_SWITCHES; i++)
	{
		std::stringstream gpio_master_sock_name_sstr;
		gpio_master_sock_name_sstr << "gpio-master-sock" << i;
		
		gpio_master_sock[i] = new gpio_master_socket(gpio_master_sock_name_sstr.str().c_str());

		gpio_bw_redirector[i] = 
			new unisim::kernel::tlm2::BwRedirector<GPIO_Switches<NUM_SWITCHES>, GPIOProtocolTypes >(
				i,
				this,
				&GPIO_Switches<NUM_SWITCHES>::gpio_nb_transport_bw,
				&GPIO_Switches<NUM_SWITCHES>::gpio_invalidate_direct_mem_ptr
			);
		
		(*gpio_master_sock[i])(*gpio_bw_redirector[i]);

		std::stringstream gpio_slave_sock_name_sstr;
		gpio_slave_sock_name_sstr << "gpio-slave-sock" << i;
		
		gpio_slave_sock[i] = new gpio_slave_socket(gpio_slave_sock_name_sstr.str().c_str());

		gpio_fw_redirector[i] = 
			new unisim::kernel::tlm2::FwRedirector<GPIO_Switches<NUM_SWITCHES>, GPIOProtocolTypes >(
				i,
				this,
				&GPIO_Switches<NUM_SWITCHES>::gpio_nb_transport_fw,
				&GPIO_Switches<NUM_SWITCHES>::gpio_b_transport,
				&GPIO_Switches<NUM_SWITCHES>::gpio_transport_dbg,
				&GPIO_Switches<NUM_SWITCHES>::gpio_get_direct_mem_ptr
			);
		
		(*gpio_slave_sock[i])(*gpio_fw_redirector[i]);
	}
	
	SC_HAS_PROCESS(GPIO_Switches);

	if(threaded_model)
	{
		SC_THREAD(Process);
	}
	else
	{
		SC_METHOD(Process);
	}
}

template <unsigned int NUM_SWITCHES>
GPIO_Switches<NUM_SWITCHES>::~GPIO_Switches()
{
	unsigned int i;
	for(i = 0; i < NUM_SWITCHES; i++)
	{
		delete gpio_master_sock[i];
		delete gpio_slave_sock[i];
		delete gpio_bw_redirector[i];
		delete gpio_fw_redirector[i];
	}
}

template <unsigned int NUM_SWITCHES>
bool GPIO_Switches<NUM_SWITCHES>::BeginSetup()
{
	return true;
}

template <unsigned int NUM_SWITCHES>
void GPIO_Switches<NUM_SWITCHES>::gpio_b_transport(unsigned int pin, GPIOPayload& payload, sc_core::sc_time& t)
{
	logger << DebugWarning << "Receiving an input on pin #" << pin << EndDebugWarning;
}

template <unsigned int NUM_SWITCHES>
tlm::tlm_sync_enum GPIO_Switches<NUM_SWITCHES>::gpio_nb_transport_fw(unsigned int pin, GPIOPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			logger << DebugWarning << "Receiving an input on pin #" << pin << EndDebugWarning;
			phase = tlm::END_REQ;
			return tlm::TLM_COMPLETED;
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

template <unsigned int NUM_SWITCHES>
unsigned int GPIO_Switches<NUM_SWITCHES>::gpio_transport_dbg(unsigned int pin, GPIOPayload& payload)
{
	return 0;
}

template <unsigned int NUM_SWITCHES>
bool GPIO_Switches<NUM_SWITCHES>::gpio_get_direct_mem_ptr(unsigned int pin, GPIOPayload& payload, tlm::tlm_dmi& dmi_data)
{
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address((sc_dt::uint64) -1);
	return false;
}

template <unsigned int NUM_SWITCHES>
void GPIO_Switches<NUM_SWITCHES>::gpio_invalidate_direct_mem_ptr(unsigned int pin, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}


template <unsigned int NUM_SWITCHES>
tlm::tlm_sync_enum GPIO_Switches<NUM_SWITCHES>::gpio_nb_transport_bw(unsigned int pin, GPIOPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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

template <unsigned int NUM_SWITCHES>
void GPIO_Switches<NUM_SWITCHES>::Process()
{
	if(threaded_model)
	{
		while(1)
		{
			Poll();
			wait(polling_period);
		}
	}
	else
	{
		Poll();
		next_trigger(polling_period);
	}
}

template <unsigned int NUM_SWITCHES>
bool GPIO_Switches<NUM_SWITCHES>::KeyToPin(uint8_t key_num, unsigned int& pin)
{
	if(key_num > NUM_SWITCHES) return false;
	
	pin = key_num;
	
	return true;
}

template <unsigned int NUM_SWITCHES>
void GPIO_Switches<NUM_SWITCHES>::Poll()
{
	if(keyboard_import)
	{
		Keyboard::KeyAction key_action;
		
		if(keyboard_import->GetKeyAction(key_action))
		{
			do
			{
				unsigned int pin;
				
				if(KeyToPin(key_action.key_num, pin))
				{
					bool pin_level = (key_action.action == Keyboard::KeyAction::KEY_DOWN) ? true : false;
					GPIOPayload *gpio_payload = gpio_payload_fabric.allocate();

					gpio_payload->SetValue(pin_level);
					
					sc_core::sc_time t(sc_core::SC_ZERO_TIME);
					tlm::tlm_phase phase = tlm::BEGIN_REQ;
					
					(*gpio_master_sock[pin])->nb_transport_fw(*gpio_payload, phase, t);
					
					gpio_payload->release();
				}
			}
			while(keyboard_import->GetKeyAction(key_action));
		}
	}
}

} // end of namespace xps_gpio
} // end of namespace xilinx
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
