/*
 *  Copyright (c) 2017,
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

#ifndef __UNISIM_COMPONENT_TLM2_TIMER_FREESCALE_MPC57XX_PIT_PIT_HH__
#define __UNISIM_COMPONENT_TLM2_TIMER_FREESCALE_MPC57XX_PIT_PIT_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/endian/endian.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/tlm2/clock.hh>
#include <unisim/util/reg/core/register.hh>
#include <unisim/util/debug/simple_register_registry.hh>
#include <tlm_utils/passthrough_target_socket.h>
#include <stack>

#define SWITCH_ENUM_TRAIT(ENUM_TYPE, CLASS_NAME) template <ENUM_TYPE, bool __SWITCH_TRAIT_DUMMY__ = true> struct CLASS_NAME {}
#define CASE_ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) template <bool __SWITCH_TRAIT_DUMMY__> struct CLASS_NAME<ENUM_VALUE, __SWITCH_TRAIT_DUMMY__>
#define ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) CLASS_NAME<ENUM_VALUE>

namespace unisim {
namespace component {
namespace tlm2 {
namespace timer {
namespace freescale {
namespace mpc57xx {
namespace pit {

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
	static const unsigned int MAX_CHANNELS = 8;
	static const unsigned int NUM_CHANNELS = 8;
	static const bool HAS_RTI_SUPPORT = true;
	static const bool HAS_DMA_SUPPORT = false;
	static const bool HAS_64_BIT_TIMER_SUPPORT = false;
	static const unsigned int BUSWIDTH = 32
};
#endif

template <typename CONFIG>
class PIT
	: public sc_core::sc_module
	, public tlm::tlm_fw_transport_if<>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Registers>
{
public:
	static const unsigned int TLM2_IP_VERSION_MAJOR = 1;
	static const unsigned int TLM2_IP_VERSION_MINOR = 0;
	static const unsigned int TLM2_IP_VERSION_PATCH = 1;
	static const unsigned int MAX_CHANNELS          = CONFIG::MAX_CHANNELS;
	static const unsigned int NUM_CHANNELS          = CONFIG::NUM_CHANNELS;
	static const bool HAS_RTI_SUPPORT               = CONFIG::HAS_RTI_SUPPORT;
	static const bool HAS_DMA_SUPPORT               = CONFIG::HAS_DMA_SUPPORT;
	static const bool HAS_64_BIT_TIMER_SUPPORT      = CONFIG::HAS_64_BIT_TIMER_SUPPORT;
	static const unsigned int BUSWIDTH              = CONFIG::BUSWIDTH;
	static const bool threaded_model                = false;
	
	typedef tlm::tlm_target_socket<BUSWIDTH> peripheral_slave_if_type;

	peripheral_slave_if_type  peripheral_slave_if;           // peripheral slave interface
	sc_core::sc_in<bool>      m_clk;                         // Clock port
	sc_core::sc_in<bool>      per_clk;                       // PER Clock port
	sc_core::sc_in<bool>      rti_clk;                       // RTI Clock port
	sc_core::sc_in<bool>      reset_b;                       // reset
	sc_core::sc_in<bool>      debug;                         // debug port
	sc_core::sc_out<bool>    *irq[MAX_CHANNELS];             // IRQ outputs
	sc_core::sc_out<bool>    *dma_trigger[MAX_CHANNELS];     // DMA trigger
	sc_core::sc_out<bool>     rtirq;                         // Real-time interrupt request output
	
	// services
	unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;

	PIT(const sc_core::sc_module_name& name, unisim::kernel::Object *parent);
	virtual ~PIT();
	
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
	
	enum EventType
	{
		EV_NONE = 0,
		EV_WAKE_UP,
		EV_CPU_PAYLOAD
	};
	
	class Event
	{
	public:
		class Key
		{
		public:
			Key()
				: time_stamp(sc_core::SC_ZERO_TIME)
				, event_type()
			{
			}
			
			Key(const sc_core::sc_time& _time_stamp, EventType _event_type)
				: time_stamp(_time_stamp)
				, event_type(_event_type)
			{
			}

			void SetTimeStamp(const sc_core::sc_time& _time_stamp)
			{
				time_stamp = _time_stamp;
			}
			
			void SetEventType(EventType _event_type)
			{
				event_type = _event_type;
			}

			const sc_core::sc_time& GetTimeStamp() const
			{
				return time_stamp;
			}
			
			EventType GetEventType() const
			{
				return event_type;
			}

			void Clear()
			{
				time_stamp = sc_core::SC_ZERO_TIME;
				event_type = EV_NONE;
			}
			
			int operator < (const Key& sk) const
			{
				return (time_stamp < sk.time_stamp) || ((time_stamp == sk.time_stamp) && (event_type < sk.event_type));
			}
			
		private:
			sc_core::sc_time time_stamp;
			EventType event_type;                      // type of event
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
			key.SetEventType(EV_CPU_PAYLOAD);
			if(release_payload)
			{
				if(payload && payload->has_mm()) payload->release();
			}
			payload = _payload;
			release_payload = _release_payload;
		}
		
		void WakeUp()
		{
			key.SetEventType(EV_WAKE_UP);
		}
		
		void SetCompletionEvent(sc_core::sc_event *_completion_event)
		{
			completion_event = _completion_event;
		}

		const sc_core::sc_time& GetTimeStamp() const
		{
			return key.GetTimeStamp();
		}
		
		EventType GetType() const
		{
			return key.GetEventType();
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

	// Common PIT register representation
	template <typename REGISTER, Access _ACCESS>
	struct PIT_Register : AddressableRegister<REGISTER, 32, _ACCESS, sc_core::sc_time>
	{
		typedef AddressableRegister<REGISTER, 32, _ACCESS, sc_core::sc_time> Super;
		
		PIT_Register(PIT<CONFIG> *_pit) : Super(), pit(_pit) {}
		PIT_Register(PIT<CONFIG> *_pit, uint32_t value) : Super(value), pit(_pit) {}

		inline bool IsVerboseRead() const ALWAYS_INLINE { return pit->verbose; }
		inline bool IsVerboseWrite() const ALWAYS_INLINE { return pit->verbose; }
		inline std::ostream& GetInfoStream() ALWAYS_INLINE { return pit->logger.DebugInfoStream(); }

		using Super::operator =;
		
	protected:
		PIT<CONFIG> *pit;
	};
	
	// PIT Module Control Register (PIT_MCR)
	struct PIT_MCR : PIT_Register<PIT_MCR, SW_RW>
	{
		typedef PIT_Register<PIT_MCR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;
		
		struct MDIS_RTI : Field<PIT_MCR, MDIS_RTI, 29> {}; // Module Disable - RTI section
		struct MDIS     : Field<PIT_MCR, MDIS    , 30> {}; // Module Disable
		struct FRZ      : Field<PIT_MCR, FRZ     , 31> {}; // Freeze
		
		SWITCH_ENUM_TRAIT(unsigned int, _);
		CASE_ENUM_TRAIT(0, _) { typedef FieldSet<> ALL; };
		CASE_ENUM_TRAIT(1, _) { typedef FieldSet<MDIS> ALL; };
		CASE_ENUM_TRAIT(2, _) { typedef FieldSet<MDIS_RTI> ALL; };
		CASE_ENUM_TRAIT(3, _) { typedef FieldSet<MDIS_RTI, MDIS> ALL; };
		
		typedef FieldSet<typename ENUM_TRAIT(((NUM_CHANNELS != 0) ? 1 : 0) | (HAS_RTI_SUPPORT ? 2 : 0), _)::ALL, FRZ> ALL;
		
		PIT_MCR(PIT<CONFIG> *_pit) : Super(_pit) { Init(); }
		PIT_MCR(PIT<CONFIG> *_pit, uint32_t value) : Super(_pit, value) { Init(); }
		
		void Init()
		{
			this->SetName("PIT_MCR");      this->SetDescription("PIT Module Control Register");
			MDIS_RTI::SetName("MDIS_RTI"); MDIS_RTI::SetDescription("Module Disable - RTI section");
			MDIS    ::SetName("MDIS");     MDIS    ::SetDescription("Module Disable");
			FRZ     ::SetName("FRZ");      FRZ     ::SetDescription("Freeze");
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			bool old_mdis_rti = this->template Get<typename PIT_MCR::MDIS_RTI>();
			bool old_mdis = this->template Get<typename PIT_MCR::MDIS>();
			
			ReadWriteStatus rws = Super::Write(time_stamp, value, bit_enable);
			
			bool new_mdis_rti = this->template Get<typename PIT_MCR::MDIS_RTI>();
			bool new_mdis = this->template Get<typename PIT_MCR::MDIS>();
			
			if((old_mdis_rti != new_mdis_rti) || (old_mdis != new_mdis))
			{
				this->pit->WakeUp();
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// PIT Upper Lifetime Timer Register (PIT_LTMR64H)
	struct PIT_LTMR64H : PIT_Register<PIT_LTMR64H, SW_R>
	{
		typedef PIT_Register<PIT_LTMR64H, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xe0;

		struct LTH : Field<PIT_LTMR64H, LTH, 0, 31> {}; // Life Timer value
		
		typedef FieldSet<LTH> ALL;
		
		PIT_LTMR64H(PIT<CONFIG> *_pit) : Super(_pit) { Init(); }
		PIT_LTMR64H(PIT<CONFIG> *_pit, uint32_t value) : Super(_pit, value) { Init(); }
		
		void Init()
		{
			this->SetName("PIT_LTMR64H");      this->SetDescription("PIT Upper Lifetime Timer Register");
			LTH::SetName("LTH"); LTH::SetDescription("Life Timer value");
		}
		
		virtual ReadWriteStatus Read(sc_core::sc_time& time_stamp, uint32_t& value, const uint32_t& bit_enable)
		{
			// Make PIT_LTMR64H reflect PIT_CVAL1 value
			uint32_t lth = this->pit->pit_cval[1].template Get<typename PIT_CVAL::TVL>();
			this->operator = (lth);
			
			// Read register
			ReadWriteStatus rws = this->Super::Read(time_stamp, value, bit_enable);
			
			// Make PIT_LTMR64L reflect PIT_CVAL0 value at the time of PIT_LTMR64H read
			uint32_t ltl = this->pit->pit_cval[0].template Get<typename PIT_CVAL::TVL>();
			this->pit->pit_ltmr64l.template Set<typename PIT_LTMR64L::LTL>(ltl);
			
			return rws;
		}

		using Super::operator =;
	};
	
	// PIT Lower Lifetime Timer Register (PIT_LTMR64L)
	struct PIT_LTMR64L : PIT_Register<PIT_LTMR64L, SW_R>
	{
		typedef PIT_Register<PIT_LTMR64L, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xe4;

		struct LTL : Field<PIT_LTMR64L, LTL, 0, 31> {}; // Life Timer value
		
		typedef FieldSet<LTL> ALL;
		
		PIT_LTMR64L(PIT<CONFIG> *_pit) : Super(_pit) { Init(); }
		PIT_LTMR64L(PIT<CONFIG> *_pit, uint32_t value) : Super(_pit, value) { Init(); }
		
		void Init()
		{
			this->SetName("PIT_LTMR64L");      this->SetDescription("PIT Lower Lifetime Timer Register");
			LTL::SetName("LTL"); LTL::SetDescription("Life Timer value");
		}

		using Super::operator =;
	};

	// PIT RTI Timer Load Value Register (PIT_RTI_LDVAL)
	struct PIT_RTI_LDVAL : PIT_Register<PIT_RTI_LDVAL, SW_RW>
	{
		typedef PIT_Register<PIT_RTI_LDVAL, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xf0;

		struct TSV : Field<PIT_RTI_LDVAL, TSV, 0, 31> {}; // Timer Start Value Bits

		typedef FieldSet<TSV> ALL;
		
		PIT_RTI_LDVAL(PIT<CONFIG> *_pit) : Super(_pit) { Init(); }
		PIT_RTI_LDVAL(PIT<CONFIG> *_pit, uint32_t value) : Super(_pit, value) { Init(); }
		
		void Init()
		{
			this->SetName("PIT_RTI_LDVAL"); this->SetDescription("PIT RTI Timer Load Value Register");
			TSV::SetName("TSV"); TSV::SetDescription("Timer Start Value Bits");
		}
		
		using Super::operator =;
	};
	
	//  PIT RTI Current Timer Value Register (PIT_RTI_CVAL)
	struct PIT_RTI_CVAL : PIT_Register<PIT_RTI_CVAL, SW_R>
	{
		typedef PIT_Register<PIT_RTI_CVAL, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xf4;

		struct TVL : Field<PIT_RTI_CVAL, TVL, 0, 31> {}; // Current Timer Value

		typedef FieldSet<TVL> ALL;
		
		PIT_RTI_CVAL(PIT<CONFIG> *_pit) : Super(_pit) { Init(); }
		PIT_RTI_CVAL(PIT<CONFIG> *_pit, uint32_t value) : Super(_pit, value) { Init(); }
		
		void Init()
		{
			this->SetName("PIT_RTI_CVAL"); this->SetDescription("PIT RTI Timer Load Value Register");
			TVL::SetName("TVL"); TVL::SetDescription("Current Timer Value");
		}
		
		using Super::operator =;
	};
	
	// PIT RTI Timer Control Register (PIT_RTI_TCTRL)
	struct PIT_RTI_TCTRL : PIT_Register<PIT_RTI_TCTRL, SW_RW>
	{
		typedef PIT_Register<PIT_RTI_TCTRL, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xf8;

		struct CHN : Field<PIT_RTI_TCTRL, CHN, 29> {}; // Chain Mode Bit
		struct TIE : Field<PIT_RTI_TCTRL, TIE, 30> {}; // Timer Interrupt Enable Bit
		struct TEN : Field<PIT_RTI_TCTRL, TEN, 31> {}; // Timer Enable Bit

		typedef FieldSet<CHN, TIE, TEN> ALL;
		
		PIT_RTI_TCTRL(PIT<CONFIG> *_pit) : Super(_pit) { Init(); }
		PIT_RTI_TCTRL(PIT<CONFIG> *_pit, uint32_t value) : Super(_pit, value) { Init(); }
		
		void Init()
		{
			this->SetName("PIT_RTI_TCTRL"); this->SetDescription("PIT RTI Timer Control Register");
			CHN::SetName("CHN"); CHN::SetDescription("Chain Mode Bit");
			TIE::SetName("TIE"); TIE::SetDescription("Timer Interrupt Enable Bit");
			TEN::SetName("TEN"); TEN::SetDescription("Timer Enable Bit");
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			uint32_t new_value = value;
			
			if(PIT_RTI_TCTRL::CHN::template Get<uint32_t>(new_value & bit_enable) != 0)
			{
				// timer 0 cannot be changed
				PIT_RTI_TCTRL::CHN::template Set<uint32_t>(new_value, 0);
				
				this->pit->logger << DebugWarning << "RTI Timer chain mode bit (" << this->GetName() << "[" << PIT_RTI_TCTRL::CHN::GetName() << "]) can be changed" << EndDebugWarning;
			}
			
			bool old_ten = this->template Get<typename PIT_RTI_TCTRL::TEN>();
			
			ReadWriteStatus rws = Super::Write(time_stamp, new_value, bit_enable);
			
			bool new_ten = this->template Get<typename PIT_RTI_TCTRL::TEN>();
			
			if(!old_ten && new_ten)
			{
				// restart RTI timer
				this->pit->LoadRTITimer();
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	//  PIT RTI Timer Flag Register (PIT_RTI_TFLG)
	struct PIT_RTI_TFLG : PIT_Register<PIT_RTI_TFLG, SW_RW>
	{
		typedef PIT_Register<PIT_RTI_TFLG, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xfc;

		struct TIF : Field<PIT_RTI_TFLG, TIF, 31, 31, SW_R_W1C> {}; // Timer Interrupt Flag

		typedef FieldSet<TIF> ALL;
		
		PIT_RTI_TFLG(PIT<CONFIG> *_pit) : Super(_pit) { Init(); }
		PIT_RTI_TFLG(PIT<CONFIG> *_pit, uint32_t value) : Super(_pit, value) { Init(); }
	
		void Init()
		{
			this->SetName("PIT_RTI_TFLG"); this->SetDescription("PIT RTI Timer Flag Register");
			TIF::SetName("TIF"); TIF::SetDescription("Timer Interrupt Flag");
		}
	
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			// Write PIT register
			ReadWriteStatus rws = this->Super::Write(time_stamp, value, bit_enable); // this may clear TIF
			
			this->pit->SetRTIRQLevel(this->template Get<typename PIT_RTI_TFLG::TIF>());
			return rws;
		}

		using Super::operator =;
	};

	// PIT Timer Load Value Register (PIT_LDVAL)
	struct PIT_LDVAL : PIT_Register<PIT_LDVAL, SW_RW>
	{
		typedef PIT_Register<PIT_LDVAL, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x100;

		struct TSV : Field<PIT_LDVAL, TSV, 0, 31> {}; // Timer Start Value Bits

		typedef FieldSet<TSV> ALL;
		
		PIT_LDVAL() : Super(0), reg_num(0) {}
		PIT_LDVAL(PIT<CONFIG> *_pit) : Super(_pit) {}
		PIT_LDVAL(PIT<CONFIG> *_pit, uint32_t value) : Super(_pit, value) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, PIT<CONFIG> *_pit)
		{
			this->pit = _pit;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "PIT_LDVAL" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "PIT Timer Load Value Register #" << reg_num;
			this->SetDescription(description_sstr.str());
			
			TSV::SetName("TSV");
			TSV::SetDescription("Timer Start Value Bits");
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	//  PIT Current Timer Value Register (PIT_CVAL)
	struct PIT_CVAL : PIT_Register<PIT_CVAL, SW_R>
	{
		typedef PIT_Register<PIT_CVAL, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x104;

		struct TVL : Field<PIT_CVAL, TVL, 0, 31> {}; // Current Timer Value

		typedef FieldSet<TVL> ALL;
		
		PIT_CVAL() : Super(0), reg_num(0) {}
		PIT_CVAL(PIT<CONFIG> *_pit) : Super(_pit) {}
		PIT_CVAL(PIT<CONFIG> *_pit, uint32_t value) : Super(_pit, value) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, PIT<CONFIG> *_pit)
		{
			this->pit = _pit;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "PIT_CVAL" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "PIT Current Timer Value Register #" << reg_num;
			this->SetDescription(description_sstr.str());
			
			TVL::SetName("TVL");
			TVL::SetDescription("Current Timer Value");
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	// PIT Timer Control Register (PIT_TCTRL)
	struct PIT_TCTRL : PIT_Register<PIT_TCTRL, SW_RW>
	{
		typedef PIT_Register<PIT_TCTRL, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x108;

		struct CHN : Field<PIT_TCTRL, CHN, 29> {}; // Chain Mode Bit
		struct TIE : Field<PIT_TCTRL, TIE, 30> {}; // Timer Interrupt Enable Bit
		struct TEN : Field<PIT_TCTRL, TEN, 31> {}; // Timer Enable Bit

		typedef FieldSet<CHN, TIE, TEN> ALL;
		
		PIT_TCTRL() : Super(0), reg_num(0) {}
		PIT_TCTRL(PIT<CONFIG> *_pit) : Super(_pit) {}
		PIT_TCTRL(PIT<CONFIG> *_pit, uint32_t value) : Super(_pit, value) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, PIT<CONFIG> *_pit)
		{
			this->pit = _pit;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "PIT_TCTRL" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "PIT Timer Control Register #" << reg_num;
			this->SetDescription(description_sstr.str());
			
			CHN::SetName("CHN"); CHN::SetDescription("Chain Mode Bit");
			TIE::SetName("TIE"); TIE::SetDescription("Timer Interrupt Enable Bit");
			TEN::SetName("TEN"); TEN::SetDescription("Timer Enable Bit");
		}

		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			uint32_t new_value = value;
			
			if((reg_num == 0) && (PIT_TCTRL::CHN::Get(value & bit_enable) != 0))
			{
				// timer 0 cannot be changed
				PIT_TCTRL::CHN::template Set<uint32_t>(new_value, 0);
				
				this->pit->logger << DebugWarning << "Timer 0 chain mode bit (" << this->GetName() << "[" << PIT_TCTRL::CHN::GetName() << "]) can be changed" << EndDebugWarning;
			}
			
			bool old_ten = this->template Get<typename PIT_TCTRL::TEN>();
			
			ReadWriteStatus rws = Super::Write(time_stamp, new_value, bit_enable);
			
			bool new_ten = this->template Get<typename PIT_TCTRL::TEN>();
			
			if(!old_ten && new_ten)
			{
				// restart timer
				unsigned int channel_num = reg_num;
				this->pit->LoadTimer(channel_num);
			}
			
			return rws;
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	//  PIT Timer Flag Register (PIT_TFLG)
	struct PIT_TFLG : PIT_Register<PIT_TFLG, SW_RW>
	{
		typedef PIT_Register<PIT_TFLG, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x10c;

		struct TIF : Field<PIT_TFLG, TIF, 31, 31, SW_R_W1C> {}; // Timer Interrupt Flag

		typedef FieldSet<TIF> ALL;
		
		PIT_TFLG() : Super(0), reg_num(0) {}
		PIT_TFLG(PIT<CONFIG> *_pit) : Super(_pit) {}
		PIT_TFLG(PIT<CONFIG> *_pit, uint32_t value) : Super(_pit, value) {}
	
		void WithinRegisterFileCtor(unsigned int _reg_num, PIT<CONFIG> *_pit)
		{
			this->pit = _pit;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "PIT_TFLG" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "PIT Timer Flag Register #" << reg_num;
			this->SetDescription(description_sstr.str());
			
			TIF::SetName("TIF"); TIF::SetDescription("Timer Interrupt Flag");
		}
	
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			// Write PIT register
			ReadWriteStatus rws = this->Super::Write(time_stamp, value, bit_enable); // this may clear TIF
			
			unsigned int channel_num = reg_num;
			this->pit->SetIRQLevel(channel_num, this->template Get<typename PIT_TFLG::TIF>());
			return rws;
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	unisim::kernel::tlm2::ClockPropertiesProxy m_clk_prop_proxy; // proxy to get clock properties from master clock port
	unisim::kernel::tlm2::ClockPropertiesProxy *per_clk_prop_proxy; // proxy to get clock properties from PER clock port
	unisim::kernel::tlm2::ClockPropertiesProxy *rti_clk_prop_proxy; // proxy to get clock properties from RTI clock port
	
	PIT_MCR pit_mcr;                                                                           // PIT_MCR
	PIT_LTMR64H pit_ltmr64h;                                                                   // PIT_LTMR64H
	PIT_LTMR64L pit_ltmr64l;                                                                   // PIT_LTMR64L
	PIT_RTI_LDVAL pit_rti_ldval;                                                               // PIT_RTI_LDVAL
	PIT_RTI_CVAL pit_rti_cval;                                                                 // PIT_RTI_CVAL 
	PIT_RTI_TCTRL pit_rti_tctrl;                                                               // PIT_RTI_TCTRL
	PIT_RTI_TFLG pit_rti_tflg;                                                                 // PIT_RTI_TFLG 
	AddressableRegisterFile<PIT_LDVAL, MAX_CHANNELS, PIT<CONFIG>, sc_core::sc_time> pit_ldval; // PIT_LDVALn
	AddressableRegisterFile<PIT_CVAL, MAX_CHANNELS, PIT<CONFIG>, sc_core::sc_time>  pit_cval;  // PIT_CVALn
	AddressableRegisterFile<PIT_TCTRL, MAX_CHANNELS, PIT<CONFIG>, sc_core::sc_time> pit_tctrl; // PIT_TCTRLn
	AddressableRegisterFile<PIT_TFLG, MAX_CHANNELS, PIT<CONFIG>, sc_core::sc_time>  pit_tflg;  // PIT_TFLGn
	
	RegisterAddressMap<sc_dt::uint64, sc_core::sc_time> reg_addr_map;
	
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	unisim::kernel::tlm2::Schedule<Event> schedule; // Payload (processor requests over AHB interface) schedule
	
	unisim::util::endian::endian_type endian;
	unisim::kernel::variable::Parameter<unisim::util::endian::endian_type> param_endian;
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	uint32_t pit_mcr_reset_value;
	unisim::kernel::variable::Parameter<uint32_t> param_pit_mcr_reset_value;
	
	bool irq_level[MAX_CHANNELS];                   // IRQ output level for each channel
	sc_core::sc_event *gen_irq_event[MAX_CHANNELS]; // Event to trigger IRQ output (IRQ_Process)
	bool rtirq_level;
	sc_core::sc_event gen_rtirq_event;
	bool dma_trigger_level[MAX_CHANNELS];                 // DMA trigger output level for each channel
	sc_core::sc_event *gen_dma_pulse_event[MAX_CHANNELS]; // Event to trigger dma pulse (DMA_TRIGGER_Process)
	sc_core::sc_time max_time_to_next_timers_run;
	sc_core::sc_time max_time_to_next_rti_timer_run;
	sc_core::sc_time last_timers_run_time;                // Last time when timers ran
	sc_core::sc_time last_rti_timer_run_time;             // Last time when RTI timer ran
	sc_core::sc_time master_clock_period;                 // Master clock period
	sc_core::sc_time master_clock_start_time;             // Master clock start time
	bool master_clock_posedge_first;                      // Master clock posedge first ?
	double master_clock_duty_cycle;                       // Master clock duty cycle
	sc_core::sc_time rti_clock_period;                    // RTI clock period
	sc_core::sc_time rti_clock_start_time;                // RTI clock start time
	bool rti_clock_posedge_first;                         // RTI clock posedge first ?
	double rti_clock_duty_cycle;                          // RTI clock duty cycle
	sc_core::sc_time per_clock_period;                    // PER clock period
	sc_core::sc_time per_clock_start_time;                // PER clock start time
	bool per_clock_posedge_first;                         // PER clock posedge first ?
	double per_clock_duty_cycle;                          // PER clock duty cycle
	sc_core::sc_event timers_run_event;                   // Event to trigger timers run (RunTimersProcess)
	bool freeze_timers;                                   // Latched value for internal "freeze"
	bool freeze_rti_timer;                                // Latched value for internal "freeze"
	bool load_timer[MAX_CHANNELS];
	bool load_rti_timer;
	
	void Reset();
	void ProcessEvent(Event *event);
	void ProcessEvents();
	void Process();
	void RESET_B_Process();
	void MasterClockPropertiesChangedProcess();
	void UpdateMasterClock();
	void UpdatePERClock();
	void UpdateRTIClock();
	void IRQ_Process(unsigned int channel_num);
	void RTIRQ_Process();
	void DMA_TRIGGER_Process(unsigned int channel_num);
	void WakeUp(const sc_core::sc_time& delay = sc_core::SC_ZERO_TIME);
	void LoadTimer(unsigned int channel_num);
	void LoadRTITimer();
	void SetIRQLevel(unsigned int channel_num, bool level);
	void SetRTIRQLevel(bool level);
	void SetDMA_TRIGGER_Level(unsigned int channel_num, bool level);
	void StartDMAPulse(unsigned int channel_num);
	void LoadAndDecrementTimers(sc_dt::uint64 delta);
	void LoadAndDecrementRTITimer(sc_dt::uint64 delta);
	sc_dt::int64 TicksToNextTimersRun();
	sc_dt::int64 TicksToNextRTITimerRun();
	sc_core::sc_time TimeToNextTimersRun();
	sc_core::sc_time TimeToNextRTITimerRun();
	sc_core::sc_time TimeToNextRun();
	void RunToTime(const sc_core::sc_time& time_stamp);
	void ScheduleRun();
	
	void RefreshFreeze();
};

} // end of namespace pit
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace timer
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_TIMER_FREESCALE_MPC57XX_PIT_PIT_HH__
