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

#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_XPS_UART_LITE_XPS_UART_LITE_HH__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_XPS_UART_LITE_XPS_UART_LITE_HH__

#include <systemc>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <unisim/component/cxx/com/xilinx/xps_uart_lite/xps_uart_lite.hh>
#include <unisim/component/tlm2/interrupt/types.hh>
#include <stack>
#include <vector>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace xilinx {
namespace xps_uart_lite {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Statistic;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::tlm2::Schedule;
using unisim::component::tlm2::interrupt::InterruptPayload;
using unisim::component::tlm2::interrupt::InterruptProtocolTypes;

template <class CONFIG>
class XPS_UARTLite
	: public sc_core::sc_module
	, public unisim::component::cxx::com::xilinx::xps_uart_lite::XPS_UARTLite<CONFIG>
	, public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types>
	, public tlm::tlm_bw_transport_if<InterruptProtocolTypes>
{
public:
	static const bool threaded_model = false;
	
	typedef unisim::component::cxx::com::xilinx::xps_uart_lite::XPS_UARTLite<CONFIG> inherited;
	typedef tlm::tlm_initiator_socket<0, InterruptProtocolTypes> interrupt_master_socket;
	
	// PLB slave interface
	tlm::tlm_target_socket<CONFIG::C_SPLB_DWITH, tlm::tlm_base_protocol_types> slave_sock;
	
	// Interrupt
	interrupt_master_socket interrupt_master_sock;

	XPS_UARTLite(const sc_core::sc_module_name& name, Object *parent = 0);
	virtual ~XPS_UARTLite();
	
	virtual bool BeginSetup();
	
	// PLB interface
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	
	// Backward nb_transport callbacks for interrupt
	virtual tlm::tlm_sync_enum nb_transport_bw(InterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

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
			EV_TELNET_IO,   // telnet I/O
			EV_CPU,         // a CPU request occured
		} Type;
		
		class Key
		{
		public:
			Key()
				: time_stamp(sc_core::SC_ZERO_TIME)
				, type(EV_TELNET_IO)
			{
			}
			
			Key(const sc_core::sc_time& _time_stamp, Type _type)
				: time_stamp(_time_stamp)
				, type(_type)
			{
			}
			
			void Initialize(const sc_core::sc_time& _time_stamp, Type _type)
			{
				time_stamp = _time_stamp;
				type = _type;
			}
			
			void SetTimeStamp(const sc_core::sc_time& _time_stamp)
			{
				time_stamp = _time_stamp;
			}
			
			void Clear()
			{
				time_stamp = sc_core::SC_ZERO_TIME;
				type = EV_CPU;
			}
			
			int operator < (const Key& sk) const
			{
				return (time_stamp < sk.time_stamp) || ((time_stamp == sk.time_stamp) && ((type < sk.type)));
			}
			
			const sc_core::sc_time& GetTimeStamp() const
			{
				return time_stamp;
			}
			
			Type GetType() const
			{
				return type;
			}
		private:
			sc_core::sc_time time_stamp;
			typename Event::Type type;
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
		
		void InitializeTelnetIOEvent(const sc_core::sc_time& time_stamp)
		{
			key.Initialize(time_stamp, EV_TELNET_IO);
			cpu_payload = 0;
			ev_completed = 0;
		}

		void InitializeCPUEvent(tlm::tlm_generic_payload *_payload, const sc_core::sc_time& time_stamp, sc_core::sc_event *_ev_completed = 0)
		{
			_payload->acquire();
			key.Initialize(time_stamp, EV_CPU);
			cpu_payload = _payload;
			ev_completed = _ev_completed;
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
	sc_core::sc_time telnet_refresh_time;
	
	sc_core::sc_time time_stamp;
	sc_core::sc_time ready_time_stamp;

	bool interrupt_output;
	
	/** The parameter for the cycle time */
	Parameter<sc_core::sc_time> param_cycle_time;
	Parameter<sc_core::sc_time> param_telnet_refresh_time;

	PayloadFabric<InterruptPayload> interrupt_payload_fabric;
	Schedule<Event> schedule;
	
	void ProcessEvents();
	void ProcessCPUEvent(Event *event);
	void GenerateOutput();
};

} // end of namespace xps_uart_lite
} // end of namespace xilinx
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
