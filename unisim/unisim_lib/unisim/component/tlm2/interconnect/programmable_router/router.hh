/*
 *  Copyright (c) 2018,
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

#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_PROGRAMMABLE_ROUTER_ROUTER_HH__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_PROGRAMMABLE_ROUTER_ROUTER_HH__

#include <unisim/kernel/variable/endian/endian.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.hh>
#include <unisim/util/reg/core/register.hh>

#define SWITCH_ENUM_TRAIT(ENUM_TYPE, CLASS_NAME) template <ENUM_TYPE, bool __SWITCH_TRAIT_DUMMY__ = true> struct CLASS_NAME {}
#define CASE_ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) template <bool __SWITCH_TRAIT_DUMMY__> struct CLASS_NAME<ENUM_VALUE, __SWITCH_TRAIT_DUMMY__>
#define ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) CLASS_NAME<ENUM_VALUE>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace programmable_router {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

using unisim::util::reg::core::AddressableRegisterBase;
using unisim::util::reg::core::RegisterAddressMap;
using unisim::util::reg::core::AddressableRegisterFileBase;
using unisim::util::reg::core::ReadWriteStatus;
using unisim::util::reg::core::IsReadWriteError;
using unisim::util::reg::core::IsUnmappedAccessError;
using unisim::util::reg::core::IsAccessNotAllowedError;
using unisim::util::reg::core::RWS_OK;

#if 0
struct CONFIG
{
	static const unsigned int PERIPHERAL_BUSWIDTH = 32;
	static const unsigned int NUM_PERIPHERAL_SLAVE_IFS = 1;
};
#endif

template <typename CONFIG>
class Router
	: public unisim::component::tlm2::interconnect::generic_router::Router<CONFIG>
{
public:
	typedef unisim::component::tlm2::interconnect::generic_router::Router<CONFIG> Super;
	typedef typename CONFIG::TYPES TYPES;
	
	static const unsigned int PERIPHERAL_BUSWIDTH      = CONFIG::PERIPHERAL_BUSWIDTH;
	static const unsigned int NUM_PERIPHERAL_SLAVE_IFS = CONFIG::NUM_PERIPHERAL_SLAVE_IFS;
	static const bool threaded_model                   = false;
	
	template <unsigned int N, bool DUMMY = true>
	struct peripheral_slave_if_type
	{
		typedef tlm_utils::passthrough_target_socket_tagged<Router, PERIPHERAL_BUSWIDTH, TYPES> elem_type;
		sc_core::sc_vector<elem_type> vec;
		
		peripheral_slave_if_type(const char *name) : vec(name, N) {}
		elem_type& operator [] (typename sc_core::sc_vector<elem_type>::size_type idx) { return vec[idx]; }
	};
	
	template <bool DUMMY>
	struct peripheral_slave_if_type<1, DUMMY>
	{
		typedef tlm_utils::passthrough_target_socket_tagged<Router, PERIPHERAL_BUSWIDTH, TYPES> elem_type;
		elem_type socket;
		
		peripheral_slave_if_type(const char *name) : socket(name) {}
		elem_type& operator [] (typename sc_core::sc_vector<elem_type>::size_type idx) { return socket; }
		operator elem_type& () { return socket; }
	};

	peripheral_slave_if_type<NUM_PERIPHERAL_SLAVE_IFS> peripheral_slave_if;              // peripheral slave interface(s)
	sc_core::sc_in<bool>                               m_clk;                            // Clock port
	sc_core::sc_in<bool>                               reset_b;                          // reset
	
	Router(const sc_core::sc_module_name& name, unisim::kernel::Object *parent);
	virtual ~Router();
	
	void peripheral_b_transport(int iface, tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	bool peripheral_get_direct_mem_ptr(int iface, tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	unsigned int peripheral_transport_dbg(int iface, tlm::tlm_generic_payload& payload);
	tlm::tlm_sync_enum peripheral_nb_transport_fw(int iface, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);

protected:
	virtual void end_of_elaboration();
	virtual void Reset();

	void MapRegister(sc_dt::uint64 addr, AddressableRegisterBase<> *reg, unsigned int reg_byte_size = 0 /* in bytes (with padding) */);
	void MapRegisterFile(sc_dt::uint64 addr, AddressableRegisterFileBase<> *regfile, unsigned int reg_byte_size = 0 /* in bytes (with padding) */, unsigned int stride = 0 /* in bytes */);
	void UnmapRegistersAt(sc_dt::uint64 addr, unsigned int byte_size);
	void ClearRegisterMap();
	void MapRegister(int iface, sc_dt::uint64 addr, AddressableRegisterBase<> *reg, unsigned int reg_byte_size = 0 /* in bytes (with padding) */);
	void MapRegisterFile(int iface, sc_dt::uint64 addr, AddressableRegisterFileBase<> *regfile, unsigned int reg_byte_size = 0 /* in bytes (with padding) */, unsigned int stride = 0 /* in bytes */);
	void UnmapRegistersAt(int iface, sc_dt::uint64 addr, unsigned int byte_size);
	void ClearRegisterMap(int iface);
private:
	class Event
	{
	public:
		class Key
		{
		public:
			Key()
				: time_stamp(sc_core::SC_ZERO_TIME)
				, iface(0)
			{
			}
			
			Key(const sc_core::sc_time& _time_stamp, int _iface)
				: time_stamp(_time_stamp)
				, iface(_iface)
			{
			}

			void SetTimeStamp(const sc_core::sc_time& _time_stamp)
			{
				time_stamp = _time_stamp;
			}
			
			void SetInterface(int _iface)
			{
				iface = _iface;
			}
			
			const sc_core::sc_time& GetTimeStamp() const
			{
				return time_stamp;
			}
			
			int GetInterface() const
			{
				return iface;
			}
			
			void Clear()
			{
				time_stamp = sc_core::SC_ZERO_TIME;
				iface = 0;
			}
			
			int operator < (const Key& sk) const
			{
				return (time_stamp < sk.time_stamp) || ((time_stamp == sk.time_stamp) && (iface < sk.iface));
			}
			
		private:
			sc_core::sc_time time_stamp;
			int iface;
		};
		
		Event()
			: key()
			, payload(0)
			, release_payload(false)
			, completion_event(0)
		{
		}
		
		~Event()
		{
			Clear();
		}
		
		void Clear()
		{
			key.Clear();
			if(release_payload)
			{
				if(payload && payload->has_mm()) payload->release();
			}
			payload = 0;
			release_payload = false;
			completion_event  = 0;
		}
		
		void SetTimeStamp(const sc_core::sc_time& _time_stamp)
		{
			key.SetTimeStamp(_time_stamp);
		}
		
		void SetInterface(int _iface)
		{
			key.SetInterface(_iface);
		}
		
		void SetPayload(tlm::tlm_generic_payload *_payload, bool _release_payload = false)
		{
			if(release_payload)
			{
				if(payload && payload->has_mm()) payload->release();
			}
			payload = _payload;
			release_payload = _release_payload;
		}
		
		void SetCompletionEvent(sc_core::sc_event *_completion_event)
		{
			completion_event = _completion_event;
		}

		const sc_core::sc_time& GetTimeStamp() const
		{
			return key.GetTimeStamp();
		}
		
		int GetInterface() const
		{
			return key.GetInterface();
		}
		
		tlm::tlm_generic_payload *GetPayload() const
		{
			return payload;
		}
		
		sc_core::sc_event *GetCompletionEvent() const
		{
			return completion_event;
		}

		const Key& GetKey() const
		{
			return key;
		}
		
	private:
		Key key;                             // schedule key (i.e. time stamp)
		tlm::tlm_generic_payload *payload;   // payload
		bool release_payload;                // whether payload must be released using payload memory management
		sc_core::sc_event *completion_event; // completion event (for blocking transport interface)
	};

	unisim::kernel::tlm2::ClockPropertiesProxy m_clk_prop_proxy; // proxy to get clock properties from master clock port
	RegisterAddressMap<sc_dt::uint64> reg_addr_map[NUM_PERIPHERAL_SLAVE_IFS];
	unisim::kernel::tlm2::Schedule<Event> schedule; // Payload (processor requests over AHB interface) schedule
	unisim::util::endian::endian_type endian;
	unisim::kernel::variable::Parameter<unisim::util::endian::endian_type> param_endian;
	sc_core::sc_time master_clock_period;                 // Master clock period
	sc_core::sc_time master_clock_start_time;             // Master clock start time
	bool master_clock_posedge_first;                      // Master clock posedge first ?
	double master_clock_duty_cycle;                       // Master clock duty cycle

	void ProcessEvent(Event *event);
	void ProcessEvents();
	void Process();
	void RESET_B_Process();
	void MasterClockPropertiesChangedProcess();
	void UpdateMasterClock();
};

} // end of namespace programmable_router
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_PROGRAMMABLE_ROUTER_ROUTER_HH__
