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

#ifndef __UNISIM_COMPONENT_TLM2_GPIO_XILINX_XPS_GPIO_GPIO_LEDS_HH__
#define __UNISIM_COMPONENT_TLM2_GPIO_XILINX_XPS_GPIO_GPIO_LEDS_HH__

#include <systemc>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/component/tlm2/com/xilinx/xps_gpio/xps_gpio.hh>
#include <unisim/service/interfaces/led_board.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace xilinx {
namespace xps_gpio {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::Client;
using unisim::kernel::ServiceImport;
using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::tlm2::Schedule;
using unisim::component::tlm2::interrupt::InterruptPayload;
using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
using unisim::service::interfaces::LED_Board;

template <unsigned int NUM_LEDS>
class GPIO_LEDs
	: public sc_core::sc_module
	, public Client<LED_Board>
{
public:
	static const bool threaded_model = false;
	
	typedef tlm::tlm_target_socket<0, GPIOProtocolTypes> gpio_slave_socket;
	typedef tlm::tlm_initiator_socket<0, GPIOProtocolTypes> gpio_master_socket;
	
	gpio_slave_socket *gpio_slave_sock[NUM_LEDS];
	gpio_master_socket *gpio_master_sock[NUM_LEDS];
	
	ServiceImport<LED_Board> led_board_import;

	GPIO_LEDs(const sc_core::sc_module_name& name, Object *parent = 0);
	virtual ~GPIO_LEDs();
	
	virtual bool BeginSetup();
	
	// Forward b_transport/nb_transport callbacks for GPIO
	void gpio_b_transport(unsigned int pin, GPIOPayload& trans, sc_core::sc_time& t);
	tlm::tlm_sync_enum gpio_nb_transport_fw(unsigned int pin, GPIOPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	unsigned int gpio_transport_dbg(unsigned int pin, GPIOPayload& payload);
	bool gpio_get_direct_mem_ptr(unsigned int pin, GPIOPayload& payload, tlm::tlm_dmi& dmi_data);

	// Backward nb_transport callbacks for GPIO
	tlm::tlm_sync_enum gpio_nb_transport_bw(unsigned int pin, GPIOPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void gpio_invalidate_direct_mem_ptr(unsigned int pin, sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	void Process();
	
protected:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	
	Parameter<bool> param_verbose;
	
	bool IsVerbose() const;
private:
	void AlignToClock(sc_core::sc_time& t);
	
	class Event
	{
	public:
		typedef enum
		{
			// In order of priority
			EV_GPIO              // a GPIO input is available
		} Type;
		
		class Key
		{
		public:
			Key()
				: time_stamp(sc_core::SC_ZERO_TIME)
				, type(EV_GPIO)
				, pin(0)
			{
			}
			
			Key(const sc_core::sc_time& _time_stamp, Type _type, unsigned int _pin)
				: time_stamp(_time_stamp)
				, type(_type)
				, pin(_pin)
			{
			}
			
			void Initialize(const sc_core::sc_time& _time_stamp, Type _type, unsigned int _pin)
			{
				time_stamp = _time_stamp;
				type = _type;
				pin = _pin;
			}
			
			void SetTimeStamp(const sc_core::sc_time& _time_stamp)
			{
				time_stamp = _time_stamp;
			}
			
			void Clear()
			{
				time_stamp = sc_core::SC_ZERO_TIME;
				type = EV_GPIO;
				pin = 0;
			}
			
			int operator < (const Key& sk) const
			{
				return (time_stamp < sk.time_stamp) || ((time_stamp == sk.time_stamp) && ((type < sk.type) || ((type == sk.type) && (pin < sk.pin))));
			}
			
			const sc_core::sc_time& GetTimeStamp() const
			{
				return time_stamp;
			}
			
			Type GetType() const
			{
				return type;
			}
			
			unsigned int GetPin() const
			{
				return pin;
			}
		private:
			sc_core::sc_time time_stamp;
			typename Event::Type type;
			unsigned int pin;
		};

		Event()
			: key()
			, gpio_pin_value(0)
		{
		}
		
		~Event()
		{
			Clear();
		}
		
		void InitializeGPIOEvent(unsigned int channel, bool _gpio_pin_value, const sc_core::sc_time& time_stamp)
		{
			key.Initialize(time_stamp, EV_GPIO, channel);
			gpio_pin_value = _gpio_pin_value;
		}

		void Clear()
		{
			key.Clear();
		}
		
		Type GetType() const
		{
			return key.GetType();
		}
		
		void SetTimeStamp(const sc_core::sc_time& time_stamp)
		{
			key.SetTimeStamp(time_stamp);
		}
		
		const sc_core::sc_time& GetTimeStamp() const
		{
			return key.GetTimeStamp();
		}
		
		unsigned int GetGPIOPin() const
		{
			return key.GetPin();
		}
		
		bool GetGPIOPinValue() const
		{
			return gpio_pin_value;
		}
		
		const Key& GetKey() const
		{
			return key;
		}
	private:
		Key key;
		bool gpio_pin_value;
	};

	sc_core::sc_time time_stamp;
	
	bool led_status[NUM_LEDS];

	unisim::kernel::tlm2::FwRedirector<GPIO_LEDs<NUM_LEDS>, GPIOProtocolTypes> *gpio_fw_redirector[NUM_LEDS];
	unisim::kernel::tlm2::BwRedirector<GPIO_LEDs<NUM_LEDS>, GPIOProtocolTypes> *gpio_bw_redirector[NUM_LEDS];
	unisim::kernel::tlm2::BwRedirector<GPIO_LEDs<NUM_LEDS>, InterruptProtocolTypes> *interrupt_bw_redirector;

	Schedule<Event> schedule;
	
	void ProcessEvents();
	void ProcessGPIOEvent(Event *event);
};

} // end of namespace xps_gpio
} // end of namespace xilinx
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
