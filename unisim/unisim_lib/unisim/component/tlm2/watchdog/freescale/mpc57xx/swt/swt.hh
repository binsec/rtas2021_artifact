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

#ifndef __UNISIM_COMPONENT_TLM2_WATCHDOG_FREESCALE_MPC57XX_SWT_SWT_HH__
#define __UNISIM_COMPONENT_TLM2_WATCHDOG_FREESCALE_MPC57XX_SWT_SWT_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/endian/endian.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/tlm2/clock.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <unisim/util/reg/core/register.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/debug/simple_register_registry.hh>
#include <stack>

#define SWITCH_ENUM_TRAIT(ENUM_TYPE, CLASS_NAME) template <ENUM_TYPE, bool __SWITCH_TRAIT_DUMMY__ = true> struct CLASS_NAME {}
#define CASE_ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) template <bool __SWITCH_TRAIT_DUMMY__> struct CLASS_NAME<ENUM_VALUE, __SWITCH_TRAIT_DUMMY__>
#define ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) CLASS_NAME<ENUM_VALUE>

namespace unisim {
namespace component {
namespace tlm2 {
namespace watchdog {
namespace freescale {
namespace mpc57xx {
namespace swt {

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
using unisim::util::reg::core::SW_RW;
using unisim::util::reg::core::SW_R;
using unisim::util::reg::core::SW_W;
using unisim::util::reg::core::SW_R_W1C;
using unisim::util::reg::core::RWS_OK;
using unisim::util::reg::core::RWS_ANA;
using unisim::util::reg::core::RWS_WROR;
using unisim::util::reg::core::Access;

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
	static const unsigned int NUM_MASTERS = 3;
	static const unsigned int BUSWIDTH = 32;
};
#endif

template <typename CONFIG>
class SWT
	: public sc_core::sc_module
	, public tlm::tlm_fw_transport_if<>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Registers>
{
public:
	static const unsigned int TLM2_IP_VERSION_MAJOR                 = 1;
	static const unsigned int TLM2_IP_VERSION_MINOR                 = 0;
	static const unsigned int TLM2_IP_VERSION_PATCH                 = 0;
	static const unsigned int NUM_MASTERS                           = CONFIG::NUM_MASTERS;
	static const unsigned int BUSWIDTH                              = CONFIG::BUSWIDTH;
	static const uint16_t UNLOCK_SEQUENCE_PRIMARY_KEY               = 0xc520;
	static const uint16_t UNLOCK_SEQUENCE_SECONDARY_KEY             = 0xd928;
	static const uint16_t FIXED_SERVICE_SEQUENCE_MODE_PRIMARY_KEY   = 0xa602;
	static const uint16_t FIXED_SERVICE_SEQUENCE_MODE_SECONDARY_KEY = 0xb480;
	static const uint32_t MIN_DOWN_COUNTER_LOAD_VALUE               = 0x100;
	static const bool threaded_model                                = false;
	enum ServiceMode
	{
		SMD_FIXED_SERVICE_SEQUENCE        = 0, // 0b00
		SMD_KEYED_SERVICE_SEQUENCE        = 1, // 0b01
		SMD_FIXED_ADDRESS_EXECUTION       = 2, // 0b10
		SMD_INCREMENTAL_ADDRESS_EXECUTION = 3  // 0b11
	};
	
	typedef tlm::tlm_target_socket<BUSWIDTH>        peripheral_slave_if_type;

	peripheral_slave_if_type                        peripheral_slave_if;       // Peripheral slave interface
	sc_core::sc_in<bool>                            m_clk;                     // clock port
	sc_core::sc_in<bool>                            swt_reset_b;               // reset of watchdog
	sc_core::sc_in<bool>                            stop;                      // stop
	sc_core::sc_in<bool>                            debug;                     // debug
	sc_core::sc_out<bool>                           irq;                       // interrupt request
	sc_core::sc_out<bool>                           reset_b;                   // reset
	
	// services
	unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;

	SWT(const sc_core::sc_module_name& name, unisim::kernel::Object *parent);
	virtual ~SWT();
	
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	
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
				return time_stamp < sk.time_stamp;
			}
			
		private:
			sc_core::sc_time time_stamp;
		};
		
		Event()
			: key()
			, master_id(0)
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
		
		void SetMasterID(unsigned int _master_id)
		{
			master_id = _master_id;
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
		
		unsigned int GetMasterID() const
		{
			return master_id;
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
		unsigned int master_id;                // processor number
		tlm::tlm_generic_payload *payload;   // payload
		bool release_payload;                // whether payload must be released using payload memory management
		sc_core::sc_event *completion_event; // completion event (for blocking transport interface)
	};

	typedef unsigned int MasterID;
	
	struct CustomReadWriteArg
	{
		MasterID master_id;
		sc_core::sc_time time_stamp;
	};

	// SWT Register
	template <typename REGISTER, Access _ACCESS>
	struct SWT_Register : AddressableRegister<REGISTER, 32, _ACCESS, CustomReadWriteArg>
	{
		typedef AddressableRegister<REGISTER, 32, _ACCESS, CustomReadWriteArg> Super;
		
		SWT_Register(SWT<CONFIG> *_swt) : Super(), swt(_swt) {}
		SWT_Register(SWT<CONFIG> *_swt, uint32_t value) : Super(value), swt(_swt) {}
		
		inline bool IsVerboseRead() const ALWAYS_INLINE { return swt->verbose; }
		inline bool IsVerboseWrite() const ALWAYS_INLINE { return swt->verbose; }
		inline std::ostream& GetInfoStream() ALWAYS_INLINE { return swt->logger.DebugInfoStream(); }
		
		virtual ReadWriteStatus Read(CustomReadWriteArg& custom_rw_arg, uint32_t& value, const uint32_t& bit_enable)
		{
			// Run down counter until read time so that registers reflect precise state of SWT
			swt->RunDownCounterToTime(custom_rw_arg.time_stamp);
			
			if(!swt->CheckMasterAccess(custom_rw_arg.master_id))
			{
				// Access for the master is not enabled: access not allowed
				return RWS_ANA;
			}

			// Read register
			return this->Super::Read(custom_rw_arg, value, bit_enable);
		}
		
		virtual ReadWriteStatus Write(CustomReadWriteArg& custom_rw_arg, const uint32_t& value, const uint32_t& bit_enable)
		{
			// Run down counter until write time
			swt->RunDownCounterToTime(custom_rw_arg.time_stamp);
			
			if(!swt->CheckMasterAccess(custom_rw_arg.master_id))
			{
				// Access for the master is not enabled: access not allowed
				return RWS_ANA;
			}

			// Write register
			ReadWriteStatus rws = this->Super::Write(custom_rw_arg, value, bit_enable);
			
			// SWT register changes affect IRQ and Reset output, so schedule a run of down counter that may trigger an IRQ or Reset output
			swt->ScheduleDownCounterRun();
			return rws;
		}

		using Super::operator =;
	protected:
		SWT<CONFIG> *swt;
	};

	// SWT Register
	template <typename REGISTER, Access _ACCESS>
	struct SWT_LockableRegister : SWT_Register<REGISTER, _ACCESS>
	{
		typedef SWT_Register<REGISTER, _ACCESS> Super;
		
		SWT_LockableRegister(SWT<CONFIG> *_swt) : Super(_swt) {}
		SWT_LockableRegister(SWT<CONFIG> *_swt, uint32_t value) : Super(_swt, value) {}
		
		virtual ReadWriteStatus Write(CustomReadWriteArg& custom_rw_arg, const uint32_t& value, const uint32_t& bit_enable)
		{
			// Register is read-only if either the SWT_CR[HLK] or SWT_CR[SLK] bits are set
			if(this->swt->IsLocked())
			{
				// Locked
				this->swt->logger << DebugWarning << "While writing to " << this->GetName() << ", " << RWS_WROR << " because register is locked" << EndDebugWarning;
				return RWS_WROR;
			}

			// Write register
			return this->Super::Write(custom_rw_arg, value, bit_enable);
		}

		using Super::operator =;
	};
	
	// SWT Control Register (SWT_CR)
	struct SWT_CR : SWT_LockableRegister<SWT_CR, SW_RW>
	{
		typedef SWT_LockableRegister<SWT_CR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;
		
		struct MAP0     : Field<SWT_CR, MAP0, 0>      {}; // Master Access Protection for Master 0
		struct MAP1     : Field<SWT_CR, MAP1, 1>      {}; // Master Access Protection for Master 1
		struct MAP2     : Field<SWT_CR, MAP2, 2>      {}; // Master Access Protection for Master 2
		struct MAP3     : Field<SWT_CR, MAP3, 3>      {}; // Master Access Protection for Master 3
		struct MAP4     : Field<SWT_CR, MAP4, 4>      {}; // Master Access Protection for Master 4
		struct MAP5     : Field<SWT_CR, MAP5, 5>      {}; // Master Access Protection for Master 5
		struct MAP6     : Field<SWT_CR, MAP6, 6>      {}; // Master Access Protection for Master 6
		struct MAP7     : Field<SWT_CR, MAP7, 7>      {}; // Master Access Protection for Master 7
		struct SMD      : Field<SWT_CR, SMD, 21, 22>  {}; // Service Mode
		struct RIA      : Field<SWT_CR, RIA, 23>      {}; // Reset on Invalid Access
		struct WND      : Field<SWT_CR, WND, 24>      {}; // Window Mode
		struct ITR      : Field<SWT_CR, ITR, 25>      {}; // Interrupt Then Reset
		struct HLK      : Field<SWT_CR, HLK, 26>      {}; // Hard Lock
		struct SLK      : Field<SWT_CR, SLK, 27>      {}; // Soft Lock
		struct Reserved : Field<SWT_CR, Reserved, 28> {}; // Reserved
		struct STP      : Field<SWT_CR, STP, 29>      {}; // Stop Mode Control
		struct FRZ      : Field<SWT_CR, FRZ, 30>      {}; // Debug Mode Control
		struct WEN      : Field<SWT_CR, WEN, 31>      {}; // Watchdog Enabled
		
		SWITCH_ENUM_TRAIT(unsigned int, _);
		CASE_ENUM_TRAIT(1, _) { typedef FieldSet<MAP0> ALL; };
		CASE_ENUM_TRAIT(2, _) { typedef FieldSet<MAP0, MAP1> ALL; };
		CASE_ENUM_TRAIT(3, _) { typedef FieldSet<MAP0, MAP1, MAP2> ALL; };
		CASE_ENUM_TRAIT(4, _) { typedef FieldSet<MAP0, MAP1, MAP2, MAP3> ALL; };
		CASE_ENUM_TRAIT(5, _) { typedef FieldSet<MAP0, MAP1, MAP2, MAP3, MAP4> ALL; };
		CASE_ENUM_TRAIT(6, _) { typedef FieldSet<MAP0, MAP1, MAP2, MAP3, MAP4, MAP5> ALL; };
		CASE_ENUM_TRAIT(7, _) { typedef FieldSet<MAP0, MAP1, MAP2, MAP3, MAP4, MAP5, MAP6> ALL; };
		CASE_ENUM_TRAIT(8, _) { typedef FieldSet<MAP0, MAP1, MAP2, MAP3, MAP4, MAP5, MAP6, MAP7> ALL; };
		typedef FieldSet<typename ENUM_TRAIT(NUM_MASTERS, _)::ALL, SMD, RIA, WND, ITR, HLK, SLK, Reserved, STP, FRZ, WEN> ALL;
		
		SWT_CR(SWT<CONFIG> *_swt) : Super(_swt) { Init(); }
		SWT_CR(SWT<CONFIG> *_swt, uint32_t value) : Super(_swt, value) { Init(); }
		
		void Init()
		{
			this->SetName("SWT_CR"); this->SetDescription("SWT Control Register");
			
			MAP0::SetName("MAP0"); MAP0::SetDescription("Master Access Protection for Master 0");
			MAP1::SetName("MAP1"); MAP1::SetDescription("Master Access Protection for Master 1");
			MAP2::SetName("MAP2"); MAP2::SetDescription("Master Access Protection for Master 2");
			MAP3::SetName("MAP3"); MAP3::SetDescription("Master Access Protection for Master 3");
			MAP4::SetName("MAP4"); MAP4::SetDescription("Master Access Protection for Master 4");
			MAP5::SetName("MAP5"); MAP5::SetDescription("Master Access Protection for Master 5");
			MAP6::SetName("MAP6"); MAP6::SetDescription("Master Access Protection for Master 6");
			MAP7::SetName("MAP7"); MAP7::SetDescription("Master Access Protection for Master 7");
			SMD ::SetName("SMD");  SMD ::SetDescription("Service Mode");
			RIA ::SetName("RIA");  RIA ::SetDescription("Reset on Invalid Access");
			WND ::SetName("WND");  WND ::SetDescription("Window Mode");
			ITR ::SetName("ITR");  ITR ::SetDescription("Interrupt Then Reset");
			HLK ::SetName("HLK");  HLK ::SetDescription("Hard Lock");
			SLK ::SetName("SLK");  SLK ::SetDescription("Soft Lock");
			STP ::SetName("STP");  STP ::SetDescription("Stop Mode Control");
			FRZ ::SetName("FRZ");  FRZ ::SetDescription("Debug Mode Control");
			WEN ::SetName("WEN");  WEN ::SetDescription("Watchdog Enabled");
		}
		
		virtual ReadWriteStatus Write(CustomReadWriteArg& custom_rw_arg, const uint32_t& value, const uint32_t& bit_enable)
		{
			uint32_t new_value = value;
			
			if(this->template Get<SWT_CR::HLK>())
			{
				// hardware lock: HLK is cleared only at reset
				new_value = new_value | SWT_CR::HLK::template GetMask<uint32_t>();
			}
			
			// Write STM register
			ReadWriteStatus rws = this->Super::Write(custom_rw_arg, new_value, bit_enable);
			
			this->swt->CheckCompatibility();
			
			return rws;
		}
		
		using Super::operator =;
	};
	
	// SWT Interrupt Register (SWT_IR)
	struct SWT_IR : SWT_Register<SWT_IR, SW_RW>
	{
		typedef SWT_Register<SWT_IR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x4;
		
		struct TIF : Field<SWT_IR, TIF, 31, 31, SW_R_W1C> {}; // Time-out Interrupt Flag
		
		typedef FieldSet<TIF> ALL;
		
		SWT_IR(SWT<CONFIG> *_swt) : Super(_swt) { Init(); }
		SWT_IR(SWT<CONFIG> *_swt, uint32_t value) : Super(_swt, value) { Init(); }
		
		void Init()
		{
			this->SetName("SWT_IR"); this->SetDescription("SWT Interrupt Register");
			TIF::SetName("TIF"); TIF::SetDescription("Time-out Interrupt Flag");
		}
		
		virtual ReadWriteStatus Write(CustomReadWriteArg& custom_rw_arg, const uint32_t& value, const uint32_t& bit_enable)
		{
			// Write STM register
			ReadWriteStatus rws = this->Super::Write(custom_rw_arg, value, bit_enable); // this may clear TIF
			
			if(IsReadWriteError(rws)) return rws;
			
			this->swt->SetIRQLevel(this->template Get<typename SWT_IR::TIF>());
			return rws;
		}
		
		using Super::operator =;
	};
	
	// SWT Time-out Register (SWT_TO)
	struct SWT_TO : SWT_LockableRegister<SWT_TO, SW_RW>
	{
		typedef SWT_LockableRegister<SWT_TO, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x8;
		
		struct WTO : Field<SWT_TO, WTO, 0, 31> {}; // Watchdog time-out period in clock cycles
		
		typedef FieldSet<WTO> ALL;
		
		SWT_TO(SWT<CONFIG> *_swt) : Super(_swt) { Init(); }
		SWT_TO(SWT<CONFIG> *_swt, uint32_t value) : Super(_swt, value) { Init(); }
		
		void Init()
		{
			this->SetName("SWT_TO"); this->SetDescription("SWT Time-out Register");
			WTO::SetName("WTO"); WTO::SetDescription("Watchdog time-out period in clock cycles");
		}
		
		using Super::operator =;
	};
	
	// SWT Window Register (SWT_WN)
	struct SWT_WN : SWT_LockableRegister<SWT_WN, SW_RW>
	{
		typedef SWT_LockableRegister<SWT_WN, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xc;
		
		struct WST : Field<SWT_WN, WST, 0, 31> {}; // Window Start Value
		
		typedef FieldSet<WST> ALL;
		
		SWT_WN(SWT<CONFIG> *_swt) : Super(_swt) { Init(); }
		SWT_WN(SWT<CONFIG> *_swt, uint32_t value) : Super(_swt, value) { Init(); }
		
		void Init()
		{
			this->SetName("SWT_WN"); this->SetDescription("SWT Window Register");
			WST::SetName("WST"); WST::SetDescription("Window Start Value");
		}
		
		using Super::operator =;
	};
	
	// SWT Service Register (SWT_SR)
	struct SWT_SR : SWT_Register<SWT_SR, SW_W> // Write only
	{
		typedef SWT_Register<SWT_SR, SW_W> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x10;
		
		struct WSC : Field<SWT_SR, WSC, 16, 31> {}; // Watchdog Service Code
		
		typedef FieldSet<WSC> ALL;
		
		SWT_SR(SWT<CONFIG> *_swt) : Super(_swt) { Init(); }
		SWT_SR(SWT<CONFIG> *_swt, uint32_t value) : Super(_swt, value) { Init(); }
		
		void Init()
		{
			this->SetName("SWT_SR"); this->SetDescription("SWT Service Register");
			WSC::SetName("WSC"); WSC::SetDescription("Watchdog Service Code");
		}
		
		virtual ReadWriteStatus Write(CustomReadWriteArg& custom_rw_arg, const uint32_t& value, const uint32_t& bit_enable)
		{
			// Write STM register
			ReadWriteStatus rws = this->Super::Write(custom_rw_arg, value, bit_enable); // this may clear CIF
			
			if(IsReadWriteError(rws)) return rws;
			
			this->swt->UnlockSequence();
			if(!this->swt->ServiceSequence())
			{
				this->swt->logger << DebugWarning << "Attempt to perform service sequence while window is closed" << EndDebugWarning;
				return RWS_ANA;
			}
			
			return rws;
		}
		
		using Super::operator =;
	};
	
	// SWT Counter Output Register (SWT_CO)
	struct SWT_CO : SWT_Register<SWT_CO, SW_R>
	{
		typedef SWT_Register<SWT_CO, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x14;
		
		struct CNT : Field<SWT_CO, CNT, 0, 31> {}; // Watchdog count
		
		typedef FieldSet<CNT> ALL;
		
		SWT_CO(SWT<CONFIG> *_swt) : Super(_swt) { Init(); }
		SWT_CO(SWT<CONFIG> *_swt, uint32_t value) : Super(_swt, value) { Init(); }
		
		void Init()
		{
			this->SetName("SWT_CO"); this->SetDescription("SWT Counter Output Register");
			CNT::SetName("CNT"); CNT::SetDescription("Watchdog count");
		}
		
		using Super::operator =;
	};
	
	// SWT Service Key Register (SWT_SK)
	struct SWT_SK : SWT_LockableRegister<SWT_SK, SW_RW>
	{
		typedef SWT_LockableRegister<SWT_SK, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x18;
		
		struct SK : Field<SWT_SK, SK, 16, 31> {}; // Service Key
		
		typedef FieldSet<SK> ALL;
		
		SWT_SK(SWT<CONFIG> *_swt) : Super(_swt) { Init(); }
		SWT_SK(SWT<CONFIG> *_swt, uint32_t value) : Super(_swt, value) { Init(); }
		
		void Init()
		{
			this->SetName("SWT_SK"); this->SetDescription("SWT Service Key Register");
			SK::SetName("SK"); SK::SetDescription("Service Key");
		}
		
		using Super::operator =;
	};
	
	unisim::kernel::tlm2::ClockPropertiesProxy m_clk_prop_proxy; // proxy to get clock properties from clock port
	
	// SWT Registers
	SWT_CR swt_cr; // SWT_CR
	SWT_IR swt_ir; // SWT_IR
	SWT_TO swt_to; // SWT_TO
	SWT_WN swt_wn; // SWT_WN
	SWT_SR swt_sr; // SWT_SR
	SWT_CO swt_co; // SWT_CO
	SWT_SK swt_sk; // SWT_SK
	
	// SWT registers address map
	RegisterAddressMap<sc_dt::uint64, CustomReadWriteArg> reg_addr_map;
	
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	unisim::kernel::tlm2::Schedule<Event> schedule; // Payload (processor requests over AHB interface) schedule
	
	bool got_initial_timeout;
	unsigned int unlock_sequence_index;
	unsigned int service_sequence_index;
	uint32_t down_counter;
	
	unisim::util::endian::endian_type endian;
	unisim::kernel::variable::Parameter<unisim::util::endian::endian_type> param_endian;
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	uint32_t swt_cr_reset_value;
	unisim::kernel::variable::Parameter<uint32_t> param_swt_cr_reset_value;
	uint32_t swt_to_reset_value;
	unisim::kernel::variable::Parameter<uint32_t> param_swt_to_reset_value;
	
	bool irq_level;
	sc_core::sc_event gen_irq_event;
	bool reset_level;
	sc_core::sc_event gen_reset_event;
	sc_core::sc_time max_time_to_next_down_counter_run;
	sc_core::sc_time last_down_counter_run_time;         // Last time when down counter ran
	sc_core::sc_event down_counter_run_event;            // Event to trigger down counter run (RunDownCounterProcess)
	bool freeze;                                         // Latched value for internal "freeze"
	sc_core::sc_time master_clock_period;                // Master clock period
	sc_core::sc_time master_clock_start_time;            // Master clock start time
	bool master_clock_posedge_first;                     // Master clock posedge first ?
	double master_clock_duty_cycle;                      // Master clock duty cycle
	sc_core::sc_time watchdog_clock_period;              // Watchdog (down counter) clock period
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_watchdog_clock_period;

	void Reset();
	void ProcessEvent(Event *event);
	void ProcessEvents();
	void Process();
	void UpdateMasterClock();
	void ClockPropertiesChangedProcess();
	void SWT_RESET_B_Process();
	void IRQ_Process();
	void RESET_B_Process();

	bool CheckMasterAccess(unsigned master_id) const;
	void SetIRQLevel(bool level);
	void TriggerReset();
	void DecrementDownCounter(sc_dt::uint64 delta);
	sc_dt::int64 TicksToNextDownCounterRun();
	sc_core::sc_time TimeToNextDownCounterRun();
	void RunDownCounterToTime(const sc_core::sc_time& time_stamp);
	void ScheduleDownCounterRun();
	void RunDownCounterProcess();
	void RefreshFreeze();
	
	bool IsLocked() const;
	void UnlockSequence();
	bool CheckWindow() const;
	bool ServiceSequence(); 
	void SetDownCounter(uint32_t value);
	void RearmDownCounter();
	void CheckCompatibility();
};

} // end of namespace swt
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace watchdog
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_HH__
