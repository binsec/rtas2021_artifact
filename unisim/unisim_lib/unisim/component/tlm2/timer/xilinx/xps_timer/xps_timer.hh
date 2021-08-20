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

#ifndef __UNISIM_COMPONENT_TLM2_TIMER_XILINX_XPS_TIMER_XPS_TIMER_HH__
#define __UNISIM_COMPONENT_TLM2_TIMER_XILINX_XPS_TIMER_XPS_TIMER_HH__

#include <systemc>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <unisim/component/cxx/timer/xilinx/xps_timer/xps_timer.hh>
#include <unisim/component/tlm2/interrupt/types.hh>
#include <stack>
#include <vector>

namespace unisim {
namespace component {
namespace tlm2 {
namespace timer {
namespace xilinx {
namespace xps_timer {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::tlm2::Schedule;
using unisim::component::tlm2::interrupt::InterruptPayload;
using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
using namespace sc_core;

typedef unisim::kernel::tlm2::SimplePayload<bool> GenerateOutPayload;
typedef unisim::kernel::tlm2::SimplePayload<bool> CaptureTriggerPayload;
typedef unisim::kernel::tlm2::SimplePayload<bool> PWMPayload;

typedef unisim::kernel::tlm2::SimpleProtocolTypes<bool> GenerateOutProtocolTypes;
typedef unisim::kernel::tlm2::SimpleProtocolTypes<bool> CaptureTriggerProtocolTypes;
typedef unisim::kernel::tlm2::SimpleProtocolTypes<bool> PWMProtocolTypes;

template <class CONFIG>
class XPS_Timer
	: public sc_core::sc_module
	, public unisim::component::cxx::timer::xilinx::xps_timer::XPS_Timer<CONFIG>
	, public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types>
{
public:
	static const bool threaded_model = false;
	
	typedef unisim::component::cxx::timer::xilinx::xps_timer::XPS_Timer<CONFIG> inherited;
	typedef tlm::tlm_target_socket<0, CaptureTriggerProtocolTypes> capture_trigger_slave_socket;
	typedef tlm::tlm_initiator_socket<0, GenerateOutProtocolTypes> generate_out_master_socket;
	typedef tlm::tlm_initiator_socket<0, PWMProtocolTypes> pwm_master_socket;
	typedef tlm::tlm_initiator_socket<0, InterruptProtocolTypes> interrupt_master_socket;
	
	// PLB slave interface
	tlm::tlm_target_socket<CONFIG::C_SPLB_DWITH, tlm::tlm_base_protocol_types> slave_sock;
	
	// Capture trigger inputs
	capture_trigger_slave_socket *capture_trigger_slave_sock[CONFIG::C_ONE_TIMER_ONLY ? 1 : 2];
	
	// GenerateOut outputs
	generate_out_master_socket *generate_out_master_sock[CONFIG::C_ONE_TIMER_ONLY ? 1 : 2];
	
	// PWM output
	pwm_master_socket pwm_master_sock;
	
	// Interrupt
	interrupt_master_socket interrupt_master_sock;

	XPS_Timer(const sc_core::sc_module_name& name, Object *parent = 0);
	virtual ~XPS_Timer();
	
	virtual bool BeginSetup();
	
	// PLB interface
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	
	// Forward b_transport/nb_transport callbacks for CaptureTrigger
	void capture_trigger_b_transport(unsigned int channel, CaptureTriggerPayload& trans, sc_core::sc_time& t);
	tlm::tlm_sync_enum capture_trigger_nb_transport_fw(unsigned int channel, CaptureTriggerPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	unsigned int capture_trigger_transport_dbg(unsigned int channel, CaptureTriggerPayload& payload);
	bool capture_trigger_get_direct_mem_ptr(unsigned int channel, CaptureTriggerPayload& payload, tlm::tlm_dmi& dmi_data);

	// Backward nb_transport callbacks for GenerateOut, PWM, and interrupt
	tlm::tlm_sync_enum generate_out_nb_transport_bw(unsigned int channel, GenerateOutPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void generate_out_invalidate_direct_mem_ptr(unsigned int channel, sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	
	tlm::tlm_sync_enum pwm_nb_transport_bw(unsigned int channel, PWMPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void pwm_invalidate_direct_mem_ptr(unsigned int channel, sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	
	tlm::tlm_sync_enum interrupt_nb_transport_bw(unsigned int channel, InterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void interrupt_invalidate_direct_mem_ptr(unsigned int channel, sc_dt::uint64 start_range, sc_dt::uint64 end_range);

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
			EV_WAKE_UP,           // a simple event to make process wake up
			EV_LOAD,              // an event to make load or reload counters
			EV_CAPTURE_TRIGGER,   // a capture trigger input is available
			EV_CPU,               // a CPU request occured
		} Type;
		
		class Key
		{
		public:
			Key()
				: time_stamp(sc_core::SC_ZERO_TIME)
				, type(EV_WAKE_UP)
				, channel(0)
			{
			}
			
			Key(const sc_core::sc_time& _time_stamp, Type _type, unsigned int _channel)
				: time_stamp(_time_stamp)
				, type(_type)
				, channel(_channel)
			{
			}
			
			void Initialize(const sc_core::sc_time& _time_stamp, Type _type, unsigned int _channel)
			{
				time_stamp = _time_stamp;
				type = _type;
				channel = _channel;
			}
			
			void SetTimeStamp(const sc_core::sc_time& _time_stamp)
			{
				time_stamp = _time_stamp;
			}
			
			void Clear()
			{
				time_stamp = sc_core::SC_ZERO_TIME;
				type = EV_WAKE_UP;
				channel = 0;
			}
			
			int operator < (const Key& sk) const
			{
				return (time_stamp < sk.time_stamp) || ((time_stamp == sk.time_stamp) && ((type < sk.type) || ((type == sk.type) && (channel < sk.channel))));
			}
			
			const sc_core::sc_time& GetTimeStamp() const
			{
				return time_stamp;
			}
			
			Type GetType() const
			{
				return type;
			}
			
			unsigned int GetChannel() const
			{
				return channel;
			}
		private:
			sc_core::sc_time time_stamp;
			typename Event::Type type;
			unsigned int channel;
		};

		Event()
			: key()
			, cpu_payload(0)
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
			ev_completed = _ev_completed;
		}
		
		void InitializeCaptureTriggerEvent(unsigned int channel, const sc_core::sc_time& time_stamp)
		{
			key.Initialize(time_stamp, EV_CAPTURE_TRIGGER, channel);
		}

		void InitializeLoadEvent(unsigned int channel, const sc_core::sc_time& time_stamp)
		{
			key.Initialize(time_stamp, EV_LOAD, channel);
		}

		void InitializeWakeUpEvent(const sc_core::sc_time& time_stamp)
		{
			key.Initialize(time_stamp, EV_WAKE_UP, 0);
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
		
		unsigned int GetChannel() const
		{
			return key.GetChannel();
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
		sc_core::sc_event *ev_completed;
	};


	sc_core::sc_time process_local_time_offset;
	/** Cycle time */
	sc_core::sc_time cycle_time;
	
	sc_core::sc_time time_stamp;
	sc_core::sc_time ready_time_stamp;

	bool capture_trigger_input[2];
	bool interrupt_output;
	bool generate_output[2];
	bool pwm_output;
	sc_core::sc_time last_timer_counter_update_time_stamp[2];
	sc_core::sc_time last_generate_output_time_stamp[2];
	bool tcr0_roll_over;
	bool tcr1_roll_over;
	
	/** The parameter for the cycle time */
	Parameter<sc_core::sc_time> param_cycle_time;

	unisim::kernel::tlm2::FwRedirector<XPS_Timer<CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *capture_trigger_redirector[2];
	unisim::kernel::tlm2::BwRedirector<XPS_Timer<CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *generate_out_redirector[2];
	unisim::kernel::tlm2::BwRedirector<XPS_Timer<CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *pwm_redirector;
	unisim::kernel::tlm2::BwRedirector<XPS_Timer<CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *interrupt_redirector;

	PayloadFabric<GenerateOutPayload> generate_out_payload_fabric;
	PayloadFabric<PWMPayload> pwm_payload_fabric;
	PayloadFabric<InterruptPayload> interrupt_payload_fabric;
	Schedule<Event> schedule;
	
	void ProcessEvents();
	void ProcessLoadEvent(Event *event);
	void ProcessCaptureTriggerEvent(Event *event);
	void ProcessCPUEvent(Event *event);
	void RunCounters();
	void Update();
	void GenerateOutput();
};

} // end of namespace xps_timer
} // end of namespace xilinx
} // end of namespace timer
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
