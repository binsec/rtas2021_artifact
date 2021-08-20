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
 
#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_CROSSBAR_CROSSBAR_HH__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_CROSSBAR_CROSSBAR_HH__

#include <unisim/component/cxx/interconnect/xilinx/crossbar/crossbar.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <systemc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace xilinx {
namespace crossbar {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
  using namespace sc_core;

template <class CONFIG>
class Crossbar
	: public unisim::component::cxx::interconnect::xilinx::crossbar::Crossbar<CONFIG>
	, public sc_core::sc_module
{
public:
	typedef unisim::component::cxx::interconnect::xilinx::crossbar::Crossbar<CONFIG> inherited;
	typedef tlm::tlm_initiator_socket<CONFIG::PLB_WIDTH * 8> plb_master_socket;
	typedef tlm::tlm_target_socket<CONFIG::PLB_WIDTH * 8> plb_slave_socket;
	typedef tlm::tlm_initiator_socket<CONFIG::MCI_WIDTH * 8> mci_master_socket;
	typedef tlm::tlm_target_socket<4> dcr_slave_socket;
	
	static const bool threaded_model = false;
	
	plb_slave_socket icurd_plb_slave_sock;    // Instruction Cache Unit Read PLB interface
	plb_slave_socket dcuwr_plb_slave_sock;    // Data Cache Unit Write PLB interface
	plb_slave_socket dcurd_plb_slave_sock;    // Data Cache Unit Read PLB interface

	plb_slave_socket splb0_slave_sock;        // SPLB0 interface
	plb_slave_socket splb1_slave_sock;        // SPLB1 interface

	plb_master_socket mplb_master_sock;       // MPLB interface
	mci_master_socket mci_master_sock;        // MCI interface

	dcr_slave_socket crossbar_dcr_slave_sock; // Crossbar Device Control Register interface
	dcr_slave_socket plbs0_dcr_slave_sock;    // PLBS0 Device Control Register interface
	dcr_slave_socket plbs1_dcr_slave_sock;    // PLBS1 Device Control Register interface
	dcr_slave_socket plbm_dcr_slave_sock;     // PLBM Device Control Register interface
	
	Crossbar(const sc_core::sc_module_name& name, Object *parent = 0);
	virtual ~Crossbar();
	
	virtual bool BeginSetup();

	void Process();
protected:

private:
	unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> > *icurd_plb_redirector;
	unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> > *dcuwr_plb_redirector;
	unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> > *dcurd_plb_redirector;
	unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> > *splb0_redirector;
	unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> > *splb1_redirector;
	unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> > *crossbar_dcr_redirector;
	unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> > *plbs0_dcr_redirector;
	unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> > *plbs1_dcr_redirector;
	unisim::kernel::tlm2::FwRedirector<Crossbar<CONFIG> > *plbm_dcr_redirector;
	unisim::kernel::tlm2::BwRedirector<Crossbar<CONFIG> > *mplb_redirector;
	unisim::kernel::tlm2::BwRedirector<Crossbar<CONFIG> > *mci_redirector;
	
	class Event
	{
	public:
		class Key
		{
		public:
			Key()
				: time_stamp(sc_core::SC_ZERO_TIME)
				, intf(inherited::IF_ICURD_PLB)
			{
			}
			
			Key(const sc_core::sc_time& _time_stamp, typename inherited::Interface _intf)
				: time_stamp(_time_stamp)
				, intf(_intf)
			{
			}
			
			void Initialize(const sc_core::sc_time& _time_stamp, typename inherited::Interface _intf)
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
				intf = inherited::IF_ICURD_PLB;
			}
			
			int operator < (const Key& sk) const
			{
				return (time_stamp < sk.time_stamp) || ((time_stamp == sk.time_stamp) && (intf < sk.intf));
			}
			
			const sc_core::sc_time& GetTimeStamp() const
			{
				return time_stamp;
			}
			
			typename inherited::Interface GetInterface() const
			{
				return intf;
			}
		private:
			sc_core::sc_time time_stamp;
			typename inherited::Interface intf;
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
		
		void Initialize(tlm::tlm_generic_payload *_payload, typename inherited::Interface intf, const sc_core::sc_time& time_stamp, sc_core::sc_event *_ev_completed = 0)
		{
			_payload->acquire();
			key.Initialize(time_stamp, intf);
			payload = _payload;
			ev_completed = _ev_completed;
		}

		void Clear()
		{
			if(payload) payload->release();
			key.Clear();
			payload = 0;
			ev_completed = 0;
		}
		
		typename inherited::Interface GetInterface() const
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
		
		void SetCompletionEvent(sc_core::sc_event *_ev_completed)
		{
			ev_completed = _ev_completed;
		}
		
	private:
		Key key;
		tlm::tlm_generic_payload *payload;
		sc_core::sc_event *ev_completed;
	};
	
	unisim::kernel::tlm2::Schedule<Event> schedule;
	
	std::map<tlm::tlm_generic_payload *, Event *> pending_requests;
	
	sc_core::sc_time cycle_time;
	Parameter<sc_core::sc_time> param_cycle_time;
	
	unisim::kernel::tlm2::LatencyLookupTable burst_latency_lut;

	tlm::tlm_sync_enum nb_transport_fw(unsigned int intf, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void b_transport(unsigned int intf, tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	unsigned int transport_dbg(unsigned int intf, tlm::tlm_generic_payload& payload);
	bool get_direct_mem_ptr(unsigned int intf, tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	tlm::tlm_sync_enum nb_transport_bw(unsigned int intf, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void invalidate_direct_mem_ptr(unsigned int intf, sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	
	void ProcessEvents();
	void ProcessForwardEvent(Event *event);
	void ProcessBackwardEvent(Event *event);
	void ProcessDCREvent(Event *event);
	void CheckResponseStatus(typename inherited::Interface master_if, typename inherited::Interface slave_if, tlm::tlm_generic_payload *payload);
};

} // end of namespace crossbar
} // end of namespace xilinx
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_XILINX_CROSSBAR_CROSSBAR_HH__
