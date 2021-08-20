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

#ifndef __UNISIM_COMPONENT_TLM2_INTERRUPT_XILINX_XPS_INTC_XPS_INTC_HH__
#define __UNISIM_COMPONENT_TLM2_INTERRUPT_XILINX_XPS_INTC_XPS_INTC_HH__

#include <systemc>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <unisim/component/cxx/interrupt/xilinx/xps_intc/xps_intc.hh>
#include <unisim/component/tlm2/interrupt/types.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interrupt {
namespace xilinx {
namespace xps_intc {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
using unisim::component::tlm2::interrupt::InterruptPayload;
using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::tlm2::Schedule;

template <class CONFIG>
class XPS_IntC
	: public sc_core::sc_module
	, public unisim::component::cxx::interrupt::xilinx::xps_intc::XPS_IntC<CONFIG>
	, public tlm::tlm_fw_transport_if<>
	, public tlm::tlm_bw_transport_if<InterruptProtocolTypes>
{
public:
	typedef unisim::component::cxx::interrupt::xilinx::xps_intc::XPS_IntC<CONFIG> inherited;
	typedef tlm::tlm_target_socket<0, InterruptProtocolTypes> irq_slave_socket;
	typedef tlm::tlm_initiator_socket<0, InterruptProtocolTypes> irq_master_socket;
	
	static const bool threaded_model = false;
	
	// PLB slave interface
	tlm::tlm_target_socket<CONFIG::C_SPLB_DWITH> slave_sock;
	
	// Input interrupts
	irq_slave_socket *irq_slave_sock[CONFIG::C_NUM_INTR_INPUTS];
	
	// Output interrupt request port
	irq_master_socket irq_master_sock;

	XPS_IntC(const sc_core::sc_module_name& name, Object *parent = 0);
	virtual ~XPS_IntC();
	
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);

	virtual tlm::tlm_sync_enum nb_transport_bw(InterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	virtual void interrupt_b_transport(unsigned int irq, InterruptPayload& trans, sc_core::sc_time& t);
	virtual tlm::tlm_sync_enum interrupt_nb_transport_fw(unsigned int irq, InterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	unsigned int interrupt_transport_dbg(unsigned int, InterruptPayload& trans);
	bool interrupt_get_direct_mem_ptr(unsigned int, InterruptPayload& payload, tlm::tlm_dmi& dmi_data);

	void Process();
protected:
	virtual void SetOutputLevel(bool level);
	virtual void SetOutputEdge(bool final_level);
private:
	bool IsVerbose() const;
	void AlignToClock(sc_core::sc_time& t);

	class Event
	{
	public:
		typedef enum
		{
			// In order of priority
			EV_IRQ,               // an IRQ occured
			EV_CPU,               // a CPU request occured
		} Type;
		
		class Key
		{
		public:
			Key()
				: time_stamp(sc_core::SC_ZERO_TIME)
				, type(EV_IRQ)
				, irq(0)
			{
			}
			
			Key(const sc_core::sc_time& _time_stamp, Type _type, unsigned int _irq)
				: time_stamp(_time_stamp)
				, type(_type)
				, irq(_irq)
			{
			}
			
			void Initialize(const sc_core::sc_time& _time_stamp, Type _type, unsigned int _irq)
			{
				time_stamp = _time_stamp;
				type = _type;
				irq = _irq;
			}
			
			void SetTimeStamp(const sc_core::sc_time& _time_stamp)
			{
				time_stamp = _time_stamp;
			}
			
			void Clear()
			{
				time_stamp = sc_core::SC_ZERO_TIME;
				type = EV_IRQ;
				irq = 0;
			}

			int operator < (const Key& sk) const
			{
				return (time_stamp < sk.time_stamp) || ((time_stamp == sk.time_stamp) && ((type < sk.type) || ((type == sk.type) && (irq < sk.irq))));
			}
			
			const sc_core::sc_time& GetTimeStamp() const
			{
				return time_stamp;
			}
			
			Type GetType() const
			{
				return type;
			}
			
			unsigned int GetIRQ() const
			{
				return irq;
			}
		private:
			sc_core::sc_time time_stamp;
			typename Event::Type type;
			unsigned int irq;
		};

		Event()
			: key()
			, cpu_payload(0)
			, level(false)
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
			level = false;
			ev_completed = _ev_completed;
		}
		
		void InitializeIRQEvent(unsigned int irq, bool _level, const sc_core::sc_time& time_stamp)
		{
			key.Initialize(time_stamp, EV_IRQ, irq);
			level = _level;
		}

		void Clear()
		{
			if(cpu_payload) cpu_payload->release();
			key.Clear();
			cpu_payload = 0;
			level = false;
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
		
		unsigned int GetIRQ() const
		{
			return key.GetIRQ();
		}

		unsigned int GetLevel() const
		{
			return level;
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
		bool level;
		sc_core::sc_event *ev_completed;
	};
	
	/** Cycle time */
	sc_core::sc_time cycle_time;

	sc_core::sc_time time_stamp;
	sc_core::sc_time ready_time_stamp;
	
	/** The parameter for the cycle time */
	Parameter<sc_core::sc_time> param_cycle_time;

	unisim::kernel::tlm2::FwRedirector<XPS_IntC<CONFIG>, InterruptProtocolTypes> *irq_redirector[CONFIG::C_NUM_INTR_INPUTS];
	bool interrupt_input[CONFIG::C_NUM_INTR_INPUTS];
	bool output_level;
	Schedule<Event> schedule;
	
	PayloadFabric<InterruptPayload> interrupt_payload_fabric;

	void ProcessEvents();
	void ProcessCPUEvent(Event *event);
	void ProcessIRQEvent(Event *event);
};

} // end of namespace xps_intc
} // end of namespace xilinx
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
