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

#ifndef __UNISIM_COMPONENT_TLM2_GPIO_XILINX_XPS_GPIO_XPS_GPIO_HH__
#define __UNISIM_COMPONENT_TLM2_GPIO_XILINX_XPS_GPIO_XPS_GPIO_HH__

#include <systemc>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <unisim/component/cxx/com/xilinx/xps_gpio/xps_gpio.hh>
#include <unisim/component/tlm2/interrupt/types.hh>
#include <stack>
#include <vector>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace xilinx {
namespace xps_gpio {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::tlm2::Schedule;
using unisim::component::tlm2::interrupt::InterruptPayload;
using unisim::component::tlm2::interrupt::InterruptProtocolTypes;

typedef unisim::kernel::tlm2::SimplePayload<bool> GPIOPayload;

typedef unisim::kernel::tlm2::SimpleProtocolTypes<bool> GPIOProtocolTypes;

template <class CONFIG>
class XPS_GPIO
	: public sc_core::sc_module
	, public unisim::component::cxx::com::xilinx::xps_gpio::XPS_GPIO<CONFIG>
	, public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types>
{
public:
	static const bool threaded_model = false;
	
	typedef unisim::component::cxx::com::xilinx::xps_gpio::XPS_GPIO<CONFIG> inherited;
	typedef tlm::tlm_initiator_socket<0, GPIOProtocolTypes> gpio_master_socket;
	typedef tlm::tlm_target_socket<0, GPIOProtocolTypes> gpio_slave_socket;
	typedef tlm::tlm_initiator_socket<0, InterruptProtocolTypes> interrupt_master_socket;
	
	// PLB slave interface
	tlm::tlm_target_socket<CONFIG::C_SPLB_DWITH, tlm::tlm_base_protocol_types> slave_sock;
	
	// GPIO inputs
	gpio_slave_socket *gpio_slave_sock[CONFIG::C_GPIO_WIDTH];
	gpio_slave_socket *gpio2_slave_sock[CONFIG::C_GPIO2_WIDTH];

	// GPIO output
	gpio_master_socket *gpio_master_sock[CONFIG::C_GPIO_WIDTH];
	gpio_master_socket *gpio2_master_sock[CONFIG::C_GPIO2_WIDTH];

	// Interrupt
	interrupt_master_socket *interrupt_master_sock;

	XPS_GPIO(const sc_core::sc_module_name& name, Object *parent = 0);
	virtual ~XPS_GPIO();
	
	virtual bool BeginSetup();
	
	// PLB interface
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	
	// Forward b_transport/nb_transport callbacks for GPIO
	void gpio_b_transport(unsigned int pin, GPIOPayload& trans, sc_core::sc_time& t);
	tlm::tlm_sync_enum gpio_nb_transport_fw(unsigned int pin, GPIOPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	unsigned int gpio_transport_dbg(unsigned int pin, GPIOPayload& payload);
	bool gpio_get_direct_mem_ptr(unsigned int pin, GPIOPayload& payload, tlm::tlm_dmi& dmi_data);

	// Backward nb_transport callbacks for GPIO
	tlm::tlm_sync_enum gpio_nb_transport_bw(unsigned int pin, GPIOPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void gpio_invalidate_direct_mem_ptr(unsigned int pin, sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	
	// Backward nb_transport callbacks for interrupt
	tlm::tlm_sync_enum interrupt_nb_transport_bw(unsigned int dummy_id, InterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void interrupt_invalidate_direct_mem_ptr(unsigned int dummy_id, sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	void Process();
	
protected:
private:
	void AlignToClock(sc_core::sc_time& t);
	
	class Event
	{
	public:
		typedef enum
		{
			// In order of priority
			EV_GPIO,              // a GPIO input is available
			EV_CPU,               // a CPU request occured
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
			, cpu_payload(0)
			, gpio_pin_value(0)
			, ev_completed(0)
		{
		}
		
		~Event()
		{
			Clear();
		}
		
		void InitializeCPUEvent(tlm::tlm_generic_payload *_payload, const sc_core::sc_time& time_stamp, sc_core::sc_event *_ev_completed = 0)
		{
			_payload->acquire();
			key.Initialize(time_stamp, EV_CPU, 0);
			cpu_payload = _payload;
			gpio_pin_value = 0;
			ev_completed = _ev_completed;
		}
		
		void InitializeGPIOEvent(unsigned int channel, bool _gpio_pin_value, const sc_core::sc_time& time_stamp)
		{
			key.Initialize(time_stamp, EV_GPIO, channel);
			cpu_payload = 0;
			gpio_pin_value = _gpio_pin_value;
			ev_completed = 0;
		}

		void Clear()
		{
			if(cpu_payload) cpu_payload->release();
			key.Clear();
			cpu_payload = 0;
			ev_completed = 0;
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
		
		tlm::tlm_generic_payload *GetCPUPayload() const
		{
			return cpu_payload;
		}
		
		unsigned int GetGPIOPin() const
		{
			return key.GetPin();
		}
		
		bool GetGPIOPinValue() const
		{
			return gpio_pin_value;
		}
		
		sc_core::sc_event *GetCompletionEvent() const
		{
			return ev_completed;
		}
		
		const Key& GetKey() const
		{
			return key;
		}
	private:
		Key key;
		tlm::tlm_generic_payload *cpu_payload;
		bool gpio_pin_value;
		sc_core::sc_event *ev_completed;
	};


	sc_core::sc_time process_local_time_offset;
	/** Cycle time */
	sc_core::sc_time cycle_time;
	
	sc_core::sc_time time_stamp;
	sc_core::sc_time ready_time_stamp;

	uint32_t gpio_output_data[inherited::NUM_GPIO_CHANNELS];
	bool interrupt_output;
	
	/** The parameter for the cycle time */
	Parameter<sc_core::sc_time> param_cycle_time;

	unisim::kernel::tlm2::FwRedirector<XPS_GPIO<CONFIG>, GPIOProtocolTypes> *gpio_fw_redirector[CONFIG::C_GPIO_WIDTH];
	unisim::kernel::tlm2::FwRedirector<XPS_GPIO<CONFIG>, GPIOProtocolTypes> *gpio2_fw_redirector[CONFIG::C_GPIO2_WIDTH];
	unisim::kernel::tlm2::BwRedirector<XPS_GPIO<CONFIG>, GPIOProtocolTypes> *gpio_bw_redirector[CONFIG::C_GPIO_WIDTH];
	unisim::kernel::tlm2::BwRedirector<XPS_GPIO<CONFIG>, GPIOProtocolTypes> *gpio2_bw_redirector[CONFIG::C_GPIO2_WIDTH];
	unisim::kernel::tlm2::BwRedirector<XPS_GPIO<CONFIG>, InterruptProtocolTypes> *interrupt_bw_redirector;

	PayloadFabric<GPIOPayload> gpio_payload_fabric;
	PayloadFabric<InterruptPayload> interrupt_payload_fabric;
	Schedule<Event> schedule;
	
	bool PinToChannelAndBit(unsigned int pin, unsigned int& channel, unsigned int& bit) const;
	bool ChannelAndBitToPin(unsigned int channel, unsigned int bit, unsigned int& pin) const;
	void ProcessEvents();
	void ProcessGPIOEvent(Event *event);
	void ProcessCPUEvent(Event *event);
	void GenerateOutput();
};

} // end of namespace xps_gpio
} // end of namespace xilinx
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
