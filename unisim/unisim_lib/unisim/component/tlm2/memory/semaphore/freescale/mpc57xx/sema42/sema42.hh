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

#ifndef __UNISIM_COMPONENT_TLM2_MEMORY_SEMAPHORE_FREESCALE_MPC57XX_SEMA42_HH__
#define __UNISIM_COMPONENT_TLM2_MEMORY_SEMAPHORE_FREESCALE_MPC57XX_SEMA42_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/endian/endian.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/tlm2/clock.hh>
#include <unisim/kernel/tlm2/trans_attr.hh>
#include <unisim/util/reg/core/register.hh>
#include <unisim/util/debug/simple_register_registry.hh>

#define SWITCH_ENUM_TRAIT(ENUM_TYPE, CLASS_NAME) template <ENUM_TYPE, bool __SWITCH_TRAIT_DUMMY__ = true> struct CLASS_NAME {}
#define CASE_ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) template <bool __SWITCH_TRAIT_DUMMY__> struct CLASS_NAME<ENUM_VALUE, __SWITCH_TRAIT_DUMMY__>
#define ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) CLASS_NAME<ENUM_VALUE>

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace semaphore {
namespace freescale {
namespace mpc57xx {
namespace sema42 {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

using unisim::util::reg::core::Register;
using unisim::util::reg::core::AddressableRegister;
using unisim::util::reg::core::AddressableRegisterHandle;
using unisim::util::reg::core::RegisterAddressMap;
using unisim::util::reg::core::FieldSet;
using unisim::util::reg::core::AddressableRegisterFile;
using unisim::util::reg::core::ReadWriteStatus;
using unisim::util::reg::core::IsReadWriteError;
using unisim::util::reg::core::Access;
using unisim::util::reg::core::SW_RW;
using unisim::util::reg::core::SW_R;
using unisim::util::reg::core::SW_W;
using unisim::util::reg::core::SW_R_W1C;
using unisim::util::reg::core::RWS_OK;
using unisim::util::reg::core::RWS_ANA;

using unisim::kernel::tlm2::tlm_trans_attr;

template <typename REGISTER, typename FIELD, int OFFSET1, int OFFSET2 = -1, Access _ACCESS = SW_RW>
struct Field : unisim::util::reg::core::Field<FIELD
                                             , ((OFFSET1 >= 0) && (OFFSET1 < REGISTER::SIZE)) ? ((OFFSET2 >= 0) ? ((OFFSET2 < REGISTER::SIZE) ? ((OFFSET1 < OFFSET2) ? ((REGISTER::SIZE - 1) - OFFSET2) : ((REGISTER::SIZE - 1) - OFFSET1)) : ((REGISTER::SIZE - 1) - OFFSET1)) : ((REGISTER::SIZE - 1) - OFFSET1)) : 0
                                             , ((OFFSET1 >= 0) && (OFFSET1 < REGISTER::SIZE)) ? ((OFFSET2 >= 0) ? ((OFFSET2 < REGISTER::SIZE) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 0) : 1) : 0
                                             , _ACCESS>
{
};

#if 0
struct CONFIG
{
	static const unsigned int NUM_GATES = 16;
	static const unsigned int BUSWIDTH  = 32;
};
#endif

template <typename CONFIG>
class SEMA42
	: public sc_core::sc_module
	, public tlm::tlm_fw_transport_if<>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Registers>
{
public:
	static const unsigned int TLM2_IP_VERSION_MAJOR = 1;
	static const unsigned int TLM2_IP_VERSION_MINOR = 0;
	static const unsigned int TLM2_IP_VERSION_PATCH = 0;
	static const unsigned int NUM_GATES             = CONFIG::NUM_GATES;
	static const unsigned int MAX_GATES             = 64;
	static const unsigned int BUSWIDTH              = CONFIG::BUSWIDTH;
	static const uint8_t      RESET_GATE_KEY1            = 0xE2;
	static const uint8_t      RESET_GATE_KEY2            = 0x1D;
	static const bool threaded_model                = false;
	
	typedef tlm::tlm_target_socket<BUSWIDTH> peripheral_slave_if_type;

	peripheral_slave_if_type  peripheral_slave_if;              // peripheral slave interface
	sc_core::sc_in<bool>      m_clk;                            // Clock port
	sc_core::sc_in<bool>      reset_b;                          // reset
	
	// services
	unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;

	SEMA42(const sc_core::sc_module_name& name, unisim::kernel::Object *parent);
	virtual ~SEMA42();
	
	void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);

	//////////////// unisim::service::interface::Registers ////////////////////
	
	virtual unisim::service::interfaces::Register *GetRegister(const char *name);
	virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);

private:
	virtual void end_of_elaboration();

	unisim::kernel::logger::Logger logger;
	
	class Event
	{
	public:
		class Key
		{
		public:
			Key()
				: time_stamp(sc_core::SC_ZERO_TIME)
			{
			}
			
			Key(const sc_core::sc_time& _time_stamp)
				: time_stamp(_time_stamp)
			{
			}

			void SetTimeStamp(const sc_core::sc_time& _time_stamp)
			{
				time_stamp = _time_stamp;
			}
			
			const sc_core::sc_time& GetTimeStamp() const
			{
				return time_stamp;
			}
			
			void Clear()
			{
				time_stamp = sc_core::SC_ZERO_TIME;
			}
			
			int operator < (const Key& sk) const
			{
				return (time_stamp < sk.time_stamp);
			}
			
		private:
			sc_core::sc_time time_stamp;
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
		
		tlm_trans_attr *GetAttributes() const
		{
			return payload->template get_extension<unisim::kernel::tlm2::tlm_trans_attr>();
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
	
	enum
	{
		RSTGSM_IDLE_WAITING_1ST_WRITE     = 0,  // 00b: Idle, waiting for the first data pattern write
		RSTGSM_WAITING_2ND_WRITE          = 1,  // 01b: Waiting for the second data pattern write
		RSTGSM_2_WRITE_SEQUENCE_COMPLETED = 2,  // 10b: The 2-write sequence has completed
	};

	// Common SEMA42 register representation
	template <typename REGISTER, unsigned int _SIZE, Access _ACCESS>
	struct SEMA42_Register : AddressableRegister<REGISTER, _SIZE, _ACCESS, tlm_trans_attr>
	{
		typedef AddressableRegister<REGISTER, _SIZE, _ACCESS, tlm_trans_attr> Super;
		
		SEMA42_Register(SEMA42<CONFIG> *_sema42) : Super(), sema42(_sema42) {}
		SEMA42_Register(SEMA42<CONFIG> *_sema42, uint32_t value) : Super(value), sema42(_sema42) {}

		inline bool IsVerboseRead() const ALWAYS_INLINE { return sema42->verbose; }
		inline bool IsVerboseWrite() const ALWAYS_INLINE { return sema42->verbose; }
		inline std::ostream& GetInfoStream() ALWAYS_INLINE { return sema42->logger.DebugInfoStream(); }

		using Super::operator =;
		
	protected:
		SEMA42<CONFIG> *sema42;
	};
	
	//  Gate Register (SEMA42_GATEn)
	struct SEMA42_GATE : SEMA42_Register<SEMA42_GATE, 8, SW_RW>
	{
		typedef SEMA42_Register<SEMA42_GATE, 8, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;
		
		struct GTFSM  : Field<SEMA42_GATE, GTFSM, 4, 7> {}; // Gate Finite State Machine
		
		typedef FieldSet<GTFSM> ALL;
		
		SEMA42_GATE() : Super(0), reg_num(0) {}
		SEMA42_GATE(SEMA42<CONFIG> *_sema42) : Super(_sema42), reg_num(0) {}
		SEMA42_GATE(SEMA42<CONFIG> *_sema42, uint32_t value) : Super(_sema42, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, SEMA42<CONFIG> *_sema42)
		{
			this->sema42 = _sema42;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "SEMA42_GATE" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "Gate Register #" << reg_num;
			this->SetDescription(description_sstr.str());
			
			GTFSM::SetName("GTFSM");
			GTFSM::SetDescription("Gate Finite State Machine");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint8_t& value, const uint8_t& bit_enable)
		{
			return this->sema42->GateFSM(reg_num, trans_attr, value, bit_enable);
		}
		
		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};
	
	// Reset Gate Write (SEMA42_RSTGT_W)
	struct SEMA42_RSTGT_W : SEMA42_Register<SEMA42_RSTGT_W, 16, SW_RW>
	{
		typedef SEMA42_Register<SEMA42_RSTGT_W, 16, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x40;
		
		struct RSTGDP : Field<SEMA42_RSTGT_W, RSTGDP, 0, 7 , SW_W > {}; // Reset Gate Data Pattern
		struct RSTGTN : Field<SEMA42_RSTGT_W, RSTGTN, 8, 15, SW_RW> {}; // Reset Gate Number
		
		typedef FieldSet<RSTGDP, RSTGTN> ALL;
		
		SEMA42_RSTGT_W(SEMA42<CONFIG> *_sema42) : Super(_sema42) { Init(); }
		SEMA42_RSTGT_W(SEMA42<CONFIG> *_sema42, uint32_t value) : Super(_sema42, value) { Init(); }
		
		void Init()
		{
			this->SetName("SEMA42_RSTGT_W"); this->SetDescription("Reset Gate Write");
			
			RSTGDP::SetName("RSTGDP"); RSTGDP::SetDescription("Reset Gate Data Pattern");
			RSTGTN::SetName("RSTGTN"); RSTGTN::SetDescription("Reset Gate Number");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint16_t& value, const uint16_t& bit_enable)
		{
			return this->sema42->ResetGateFSM(trans_attr, value, bit_enable);
		}
	};
	
	// Reset Gate Read (SEMA42_RSTGT_R)
	struct SEMA42_RSTGT_R : SEMA42_Register<SEMA42_RSTGT_R, 16, SW_RW>
	{
		typedef SEMA42_Register<SEMA42_RSTGT_R, 16, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x40;
		
		struct RSTGSM : Field<SEMA42_RSTGT_R, RSTGSM, 2, 3 , SW_R > {}; // Reset Gate Finite State Machine
		struct RSTGMS : Field<SEMA42_RSTGT_R, RSTGMS, 4, 7 , SW_R > {}; // Reset Gate Bus Master
		struct RSTGTN : Field<SEMA42_RSTGT_R, RSTGTN, 8, 15, SW_RW> {}; // Reset Gate Number
		
		typedef FieldSet<RSTGSM, RSTGMS, RSTGTN> ALL;
		
		SEMA42_RSTGT_R(SEMA42<CONFIG> *_sema42) : Super(_sema42) { Init(); }
		SEMA42_RSTGT_R(SEMA42<CONFIG> *_sema42, uint32_t value) : Super(_sema42, value) { Init(); }
		
		void Init()
		{
			this->SetName("SEMA42_RSTGT_R"); this->SetDescription("Reset Gate Read");
			
			RSTGSM::SetName("RSTGSM"); RSTGSM::SetDescription("Reset Gate Finite State Machine");
			RSTGMS::SetName("RSTGMS"); RSTGMS::SetDescription("Reset Gate Bus Master");
			RSTGTN::SetName("RSTGTN"); RSTGTN::SetDescription("Reset Gate Number");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
	};

	unisim::kernel::tlm2::ClockPropertiesProxy m_clk_prop_proxy; // proxy to get clock properties from master clock port
	
	AddressableRegisterFile<SEMA42_GATE, NUM_GATES, SEMA42<CONFIG>, tlm_trans_attr> sema42_gate;    // SEMA42_GATEn
	SEMA42_RSTGT_W                                                                  sema42_rstgt_w; // SEMA42_RSTGT_W
	SEMA42_RSTGT_R                                                                  sema42_rstgt_r; // SEMA42_RSTGT_R
	
	RegisterAddressMap<sc_dt::uint64, tlm_trans_attr> read_reg_addr_map;
	RegisterAddressMap<sc_dt::uint64, tlm_trans_attr> write_reg_addr_map;
	
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	unisim::kernel::tlm2::Schedule<Event> schedule; // Payload (processor requests over AHB interface) schedule
	
	unisim::util::endian::endian_type endian;
	unisim::kernel::variable::Parameter<unisim::util::endian::endian_type> param_endian;
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	
	tlm_trans_attr default_trans_attr;
	
	sc_core::sc_time master_clock_period;                 // Master clock period
	sc_core::sc_time master_clock_start_time;             // Master clock start time
	bool master_clock_posedge_first;                      // Master clock posedge first ?
	double master_clock_duty_cycle;                       // Master clock duty cycle
	
	void Reset();
	void ResetGate(unsigned int gate_num);
	void ResetAllGates();
	ReadWriteStatus ResetGateFSM(tlm_trans_attr& trans_attr, const uint16_t& value, const uint16_t& bit_enable);
	ReadWriteStatus GateFSM(unsigned int gate_num, tlm_trans_attr& trans_attr, const uint16_t& value, const uint16_t& bit_enable);
	void ProcessEvent(Event *event);
	void ProcessEvents();
	void Process();
	void RESET_B_Process();
	void MasterClockPropertiesChangedProcess();
	void UpdateMasterClock();
};

} // end of namespace sema42
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace semaphore
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_MEMORY_SEMAPHORE_FREESCALE_MPC57XX_SEMA42_HH__
