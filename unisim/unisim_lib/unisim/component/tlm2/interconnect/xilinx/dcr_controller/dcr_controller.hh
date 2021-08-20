/*
 *  Copyright (c) 2010-2011,
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
 
#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_DCR_CONTROLLER_DCR_CONTROLLER_HH__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_DCR_CONTROLLER_DCR_CONTROLLER_HH__

#include <unisim/component/cxx/interconnect/xilinx/dcr_controller/dcr_controller.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <systemc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace xilinx {
namespace dcr_controller {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::tlm2::PayloadFabric;
  using namespace sc_core;
  
template <class CONFIG>
class DCRController
	: public unisim::component::cxx::interconnect::xilinx::dcr_controller::DCRController<CONFIG>
	, public sc_core::sc_module
{
public:
	static const bool threaded_model = false;
	
	typedef unisim::component::cxx::interconnect::xilinx::dcr_controller::DCRController<CONFIG> inherited;
	typedef tlm::tlm_initiator_socket<4> dcr_master_socket;
	typedef tlm::tlm_target_socket<4> dcr_slave_socket;
	
	dcr_master_socket *dcr_master_sock[CONFIG::NUM_SLAVES];
	dcr_slave_socket *dcr_slave_sock[CONFIG::NUM_MASTERS];
	
	DCRController(const sc_core::sc_module_name& name, Object *parent = 0);
	virtual ~DCRController();

	virtual bool BeginSetup();
	
	void Process();
protected:

private:
	unisim::kernel::tlm2::BwRedirector<DCRController<CONFIG> > *bw_redirector[CONFIG::NUM_SLAVES];
	unisim::kernel::tlm2::FwRedirector<DCRController<CONFIG> > *fw_redirector[CONFIG::NUM_MASTERS];
	
	class Event
	{
	public:
		class Key
		{
		public:
			Key()
				: time_stamp(sc_core::SC_ZERO_TIME)
				, intf(-1)
			{
			}
			
			Key(const sc_core::sc_time& _time_stamp, int _intf)
				: time_stamp(_time_stamp)
				, intf(_intf)
			{
			}
			
			void Initialize(const sc_core::sc_time& _time_stamp, int _intf)
			{
				time_stamp = _time_stamp;
				intf = _intf;
			}
			
			void SetTimeStamp(const sc_core::sc_time& _time_stamp)
			{
				time_stamp = _time_stamp;
			}
			
			void Clear()
			{
				time_stamp = sc_core::SC_ZERO_TIME;
				intf = -1;
			}
			
			int operator < (const Key& sk) const
			{
				return (time_stamp < sk.time_stamp) || ((time_stamp == sk.time_stamp) && (intf < sk.intf));
			}
			
			const sc_core::sc_time& GetTimeStamp() const
			{
				return time_stamp;
			}
			
			int GetInterface() const
			{
				return intf;
			}
		private:
			sc_core::sc_time time_stamp;
			int intf;
		};

		Event()
			: key()
			, payload(0)
			, ev_completed(0)
		{
		}
		
		~Event()
		{
			Clear();
		}
		
		void Initialize(tlm::tlm_generic_payload *_payload, int intf, const sc_core::sc_time& time_stamp, sc_core::sc_event *_ev_completed = 0)
		{
			_payload->acquire();
			key.Initialize(time_stamp, intf);
			payload = _payload;
			ev_completed = _ev_completed;
		}

		void Clear()
		{
			if(payload)
			{
				payload->release();
			}
			key.Clear();
			payload = 0;
			ev_completed = 0;
		}
		
		int GetInterface() const
		{
			return key.GetInterface();
		}
		
		void SetTimeStamp(const sc_core::sc_time& time_stamp)
		{
			key.SetTimeStamp(time_stamp);
		}
		
		const sc_core::sc_time& GetTimeStamp() const
		{
			return key.GetTimeStamp();
		}
		
		tlm::tlm_generic_payload *GetPayload() const
		{
			return payload;
		}
		
		const Key& GetKey() const
		{
			return key;
		}
		
		sc_core::sc_event *GetCompletionEvent() const
		{
			return ev_completed;
		}
	private:
		Key key;
		tlm::tlm_generic_payload *payload;
		sc_core::sc_event *ev_completed;
	};
	
	unisim::kernel::tlm2::Schedule<Event> schedule;
	
//	std::map<tlm::tlm_generic_payload *, int> return_if;
	std::map<tlm::tlm_generic_payload *, unsigned int> indirect_access_master;
	std::map<tlm::tlm_generic_payload *, tlm::tlm_generic_payload *> indirect_access_payload_binding;
	
	std::map<tlm::tlm_generic_payload *, Event *> pending_requests;
	
	PayloadFabric<tlm::tlm_generic_payload> payload_fabric;
	
	sc_core::sc_time cycle_time;
	Parameter<sc_core::sc_time> param_cycle_time;

	// Forward path
	void b_transport(unsigned int num_master, tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	tlm::tlm_sync_enum nb_transport_fw(unsigned int num_master, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	unsigned int transport_dbg(unsigned int num_slave, tlm::tlm_generic_payload& payload);
	bool get_direct_mem_ptr(unsigned int num_slave, tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);

	// backward path
	tlm::tlm_sync_enum nb_transport_bw(unsigned int num_slave, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void invalidate_direct_mem_ptr(unsigned int num_slave, sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	void ProcessEvents();
	void ProcessForwardEvent(Event *event);
	void ProcessBackwardEvent(Event *event);
	void DoTimeOutAccess(unsigned int num_master, sc_core::sc_event *ev_completed, tlm::tlm_generic_payload *payload);
	void BindIndirectAccess(tlm::tlm_generic_payload *original_payload, tlm::tlm_generic_payload *payload, unsigned int num_master);
	tlm::tlm_generic_payload *ResolveIndirectAccess(tlm::tlm_generic_payload *payload, unsigned int& num_master);
};

} // end of namespace dcr_controller
} // end of namespace xilinx
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_DCR_CONTROLLER_DCR_CONTROLLER_HH__
