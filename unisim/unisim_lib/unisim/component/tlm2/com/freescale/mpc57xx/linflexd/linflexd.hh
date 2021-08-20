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

#ifndef __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_LINFLEXD_LINFLEXD_HH__
#define __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_LINFLEXD_LINFLEXD_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/endian/endian.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/tlm2/tlm_serial.hh>
#include <unisim/kernel/tlm2/clock.hh>
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
namespace com {
namespace freescale {
namespace mpc57xx {
namespace linflexd {

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
using unisim::util::reg::core::SW_R_HW_RO;
using unisim::util::reg::core::SW_R_W1C;
using unisim::util::reg::core::SW_W;
using unisim::util::reg::core::RWS_OK;
using unisim::util::reg::core::RWS_ANA;
using unisim::util::reg::core::Access;
using unisim::util::reg::core::IsReadWriteError;

template <typename REGISTER, typename FIELD, int OFFSET1, int OFFSET2 = -1, Access _ACCESS = SW_RW>
struct Field : unisim::util::reg::core::Field<FIELD
                                             , ((OFFSET1 >= 0) && (OFFSET1 < REGISTER::SIZE)) ? ((OFFSET2 >= 0) ? ((OFFSET2 < REGISTER::SIZE) ? ((OFFSET1 < OFFSET2) ? ((REGISTER::SIZE - 1) - OFFSET2) : ((REGISTER::SIZE - 1) - OFFSET1)) : ((REGISTER::SIZE - 1) - OFFSET1)) : ((REGISTER::SIZE - 1) - OFFSET1)) : 0
                                             , ((OFFSET1 >= 0) && (OFFSET1 < REGISTER::SIZE)) ? ((OFFSET2 >= 0) ? ((OFFSET2 < REGISTER::SIZE) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 0) : 1) : 0
                                             , _ACCESS>
{
};

enum LIN_OperationMode
{
	LIN_MASTER_MODE = 0,
	LIN_SLAVE_MODE  = 1
};

enum LINFlexD_IRQ_Type
{
	LINFLEXD_IRQ_RX  = 0,
	LINFLEXD_IRQ_TX  = 1,
	LINFLEXD_IRQ_ERR = 2
};

enum LIN_State
{
	LINS_SLEEP_MODE                       = 0x0, // 0000
	LINS_INIT_MODE                        = 0x1, // 0001
	LINS_IDLE_MODE                        = 0x2, // 0010
	LINS_SYNC_BREAK                       = 0x3, // 0011
	LINS_SYNC_DEL                         = 0x4, // 0100
	LINS_SYNC_FIELD                       = 0x5, // 0101
	LINS_IDENTIFIER_FIELD                 = 0x6, // 0110
	LINS_HEADER_RECEPTION_TRANSMISSION    = 0x7, // 0111
	LINS_DATA_RECEPTION_DATA_TRANSMISSION = 0x8, // 1000
	LINS_CHECKSUM                         = 0x9  // 1001
};

inline std::ostream& operator << (std::ostream& os, const LIN_State& lins)
{
	switch(lins)
	{
		case LINS_SLEEP_MODE                      : os << "sleep mode LIN state"; break;
		case LINS_INIT_MODE                       : os << "init mode LIN state"; break;
		case LINS_IDLE_MODE                       : os << "idle mode LIN state"; break;
		case LINS_SYNC_BREAK                      : os << "sync break LIN state"; break;
		case LINS_SYNC_DEL                        : os << "sync del LIN state"; break;
		case LINS_SYNC_FIELD                      : os << "sync field LIN state"; break;
		case LINS_IDENTIFIER_FIELD                : os << "identifier field LIN state"; break;
		case LINS_HEADER_RECEPTION_TRANSMISSION   : os << "header reception/transmission LIN state"; break;
		case LINS_DATA_RECEPTION_DATA_TRANSMISSION: os << "data reception/data transmission LIN state"; break;
		case LINS_CHECKSUM                        : os << "checksum LIN state"; break;
		default                                   : os << "unknown LIN state"; break;
	}
	
	return os;
}

#if 0
struct CONFIG
{
	static const unsigned int BUSWIDTH = 32;
	static const unsigned int NUM_IRQS = 3;
	static const unsigned int TX_CH_NUM = 0;
	static const unsigned int RX_CH_NUM = 0;
	static const unsigned int NUM_FILTERS = 16;
	static const bool GENERIC_SLAVE = true; /* false: master-only, true: master/slave */
	static const bool GENERIC_PSI5 = false;
	static const bool HAS_AUTO_SYNCHRONIZATION_SUPPORT = true;
};
#endif

template <typename CONFIG>
class LINFlexD
	: public sc_core::sc_module
	, public tlm::tlm_fw_transport_if<>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Registers>
{
public:
	static const unsigned int TLM2_IP_VERSION_MAJOR    = 1;
	static const unsigned int TLM2_IP_VERSION_MINOR    = 0;
	static const unsigned int TLM2_IP_VERSION_PATCH    = 0;
	static const unsigned int BUSWIDTH                 = CONFIG::BUSWIDTH;
	static const unsigned int NUM_DMA_TX_CHANNELS      = 1 << CONFIG::TX_CH_NUM;
	static const unsigned int NUM_DMA_RX_CHANNELS      = 1 << CONFIG::RX_CH_NUM;
	static const unsigned int NUM_FILTERS              = CONFIG::NUM_FILTERS;
	static const bool GENERIC_SLAVE                    = CONFIG::GENERIC_SLAVE;
	static const bool GENERIC_PSI5                     = CONFIG::GENERIC_PSI5;
	static const bool HAS_AUTO_SYNCHRONIZATION_SUPPORT = CONFIG::HAS_AUTO_SYNCHRONIZATION_SUPPORT;
	static const unsigned int NUM_IRQS                 = CONFIG::NUM_IRQS;
	static const unsigned int NUM_IDENTIFIERS          = (GENERIC_SLAVE == 1) ? 16 : 0;
	static const bool threaded_model                   = false;
	
	enum SerialInterface
	{
		LINTX_IF,
		LINRX_IF
	};
	
	typedef tlm::tlm_target_socket<BUSWIDTH>                     peripheral_slave_if_type;
	typedef unisim::kernel::tlm2::tlm_serial_peripheral_socket_tagged<LINFlexD<CONFIG> > LINTX_type;
	typedef unisim::kernel::tlm2::tlm_serial_peripheral_socket_tagged<LINFlexD<CONFIG> > LINRX_type;
	
	peripheral_slave_if_type                         peripheral_slave_if;         // Peripheral slave interface
	LINTX_type                                       LINTX;                       // Tx Serial interface
	LINRX_type                                       LINRX;                       // Rx Serial interface
	sc_core::sc_in<bool>                             m_clk;                       // clock port
	sc_core::sc_in<bool>                             lin_clk;                     // LIN Clock port
	sc_core::sc_in<bool>                             reset_b;                     // reset
	sc_core::sc_in<bool>                            *DMA_ACK_RX[NUM_DMA_RX_CHANNELS]; // Rx DMA acknowledgement (unused)
	sc_core::sc_in<bool>                            *DMA_ACK_TX[NUM_DMA_TX_CHANNELS]; // Tx DMA acknowledgement (unused)
	sc_core::sc_out<bool>                            INT_RX;                      // Interrupt Request
	sc_core::sc_out<bool>                            INT_TX;                      // Interrupt Request
	sc_core::sc_out<bool>                            INT_ERR;                     // Interrupt Request
	sc_core::sc_out<bool>                           *DMA_RX[NUM_DMA_RX_CHANNELS]; // Rx DMA request
	sc_core::sc_out<bool>                           *DMA_TX[NUM_DMA_TX_CHANNELS]; // Tx DMA request
	
	// services
	unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;

	LINFlexD(const sc_core::sc_module_name& name, unisim::kernel::Object *parent);
	virtual ~LINFlexD();
	
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);

	void nb_receive(int id, unisim::kernel::tlm2::tlm_serial_payload& payload, const sc_core::sc_time& t);
	
	//////////////// unisim::service::interface::Registers ////////////////////
	
	virtual unisim::service::interfaces::Register *GetRegister(const char *name);
	virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);

private:
	virtual bool EndSetup();
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

	// LINFlexD Register
	template <typename REGISTER, Access _ACCESS>
	struct LINFlexD_Register : AddressableRegister<REGISTER, 32, _ACCESS>
	{
		typedef AddressableRegister<REGISTER, 32, _ACCESS> Super;
		
		LINFlexD_Register(LINFlexD<CONFIG> *_linflexd) : Super(), linflexd(_linflexd) {}
		LINFlexD_Register(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(value), linflexd(_linflexd) {}
		
		inline bool IsVerboseRead() const ALWAYS_INLINE { return linflexd->verbose; }
		inline bool IsVerboseWrite() const ALWAYS_INLINE { return linflexd->verbose; }
		inline std::ostream& GetInfoStream() ALWAYS_INLINE { return linflexd->logger.DebugInfoStream(); }
		
		using Super::operator =;
	protected:
		LINFlexD<CONFIG> *linflexd;
	};
	
	//  LIN Control Register 1 (LINFlexD_LINCR1)
	struct LINFlexD_LINCR1 : LINFlexD_Register<LINFlexD_LINCR1, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_LINCR1, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;
		
		struct CCD    : Field<LINFlexD_LINCR1, CCD   , 16>                                         {}; // Checksum Calculation disable
		struct CFD    : Field<LINFlexD_LINCR1, CFD   , 17>                                         {}; // Checksum field disable
		struct LASE   : Field<LINFlexD_LINCR1, LASE  , 18>                                         {}; // LIN Autosynchronization Enable
		struct AUTOWU : Field<LINFlexD_LINCR1, AUTOWU, 19>                                         {}; // Auto Wakeup
		struct MBL    : Field<LINFlexD_LINCR1, MBL   , 20, 23>                                     {}; // Master Break Length
		struct BF     : Field<LINFlexD_LINCR1, BF    , 24>                                         {}; // By-passfilter
		struct LBKM   : Field<LINFlexD_LINCR1, LBKM  , 26>                                         {}; // Loop Back mode
		struct MME    : Field<LINFlexD_LINCR1, MME   , 27, 27, GENERIC_SLAVE ? SW_RW : SW_R_HW_RO> {}; // Master mode enable
		struct SSBL   : Field<LINFlexD_LINCR1, SSBL  , 28>                                         {}; // Slave Mode Sync Break Length
		struct RBLM   : Field<LINFlexD_LINCR1, RBLM  , 29>                                         {}; // Receiver Buffer Locked mode
		struct SLEEP  : Field<LINFlexD_LINCR1, SLEEP , 30>                                         {}; // Sleep Mode Request
		struct INIT   : Field<LINFlexD_LINCR1, INIT  , 31>                                         {}; // Initialization Mode Request
		
		typedef FieldSet<CCD, CFD, LASE, AUTOWU, MBL, BF, LBKM, MME, SSBL, RBLM, SLEEP, INIT> ALL;
		
		typedef FieldSet<CCD, CFD, LASE, AUTOWU, MBL, BF, LBKM, MME, SSBL, RBLM> INIT_MODE_ONLY_WRITEABLE;
		
		LINFlexD_LINCR1(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_LINCR1(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_LINCR1"); this->SetDescription("LIN Control Register 1");
			CCD   ::SetName("CCD");    CCD   ::SetDescription("Checksum Calculation disable");
			CFD   ::SetName("CFD");    CFD   ::SetDescription("Checksum field disable");
			LASE  ::SetName("LASE");   LASE  ::SetDescription("LIN Autosynchronization Enable");
			AUTOWU::SetName("AUTOWU"); AUTOWU::SetDescription("Auto Wakeup");
			MBL   ::SetName("MBL");    MBL   ::SetDescription("Master Break Length");
			BF    ::SetName("BF");     BF    ::SetDescription("By-passfilter");
			LBKM  ::SetName("LBKM");   LBKM  ::SetDescription("Loop Back mode");
			MME   ::SetName("MME");    MME   ::SetDescription("Master mode enable");
			SSBL  ::SetName("SSBL");   SSBL  ::SetDescription("Slave Mode Sync Break Length");
			RBLM  ::SetName("RBLM");   RBLM  ::SetDescription("Receiver Buffer Locked mode");
			SLEEP ::SetName("SLEEP");  SLEEP ::SetDescription("Sleep Mode Request");
			INIT  ::SetName("INIT");   INIT  ::SetDescription("Initialization Mode Request");
		}
		
		void Reset()
		{
			this->template Set<CCD   >(0);
			this->template Set<CFD   >(0);
			this->template Set<LASE  >(0);
			this->template Set<AUTOWU>(0);
			this->template Set<MBL   >(0);
			this->template Set<BF    >(1);
			this->template Set<LBKM  >(0);
			this->template Set<MME   >(GENERIC_SLAVE ? 0 : 1);
			this->template Set<SSBL  >(0);
			this->template Set<RBLM  >(0);
			this->template Set<SLEEP >(1);
			this->template Set<INIT  >(0);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = this->linflexd->InitMode() ? Super::Write(value, bit_enable)
			                                                 : Super::template WritePreserve<INIT_MODE_ONLY_WRITEABLE>(value, bit_enable);
			
			this->linflexd->UpdateState();
			
			return rws;
		}
		
		using Super::operator =;
	};
	
	// LIN Interrupt enable register (LINFlexD_LINIER)
	struct LINFlexD_LINIER : LINFlexD_Register<LINFlexD_LINIER, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_LINIER, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x4;
		
		struct SZIE      : Field<LINFlexD_LINIER, SZIE     , 16>                                         {}; // Stuck at zero Interrupt Enable
		struct OCIE      : Field<LINFlexD_LINIER, OCIE     , 17>                                         {}; // Output Compare Interrupt Enable
		struct BEIE      : Field<LINFlexD_LINIER, BEIE     , 18>                                         {}; // Bit Error Interrupt Enable
		struct CEIE      : Field<LINFlexD_LINIER, CEIE     , 19>                                         {}; // Checksum Error Interrupt Enable
		struct HEIE      : Field<LINFlexD_LINIER, HEIE     , 20>                                         {}; // Header Error Interrupt Enable
		struct FEIE      : Field<LINFlexD_LINIER, FEIE     , 23>                                         {}; // Frame Error Interrupt Enable
		struct BOIE      : Field<LINFlexD_LINIER, BOIE     , 24>                                         {}; // Buffer Overrun Error Interrupt Enable
		struct LSIE      : Field<LINFlexD_LINIER, LSIE     , 25>                                         {}; // LIN state Interrupt enable
		struct WUIE      : Field<LINFlexD_LINIER, WUIE     , 26>                                         {}; // Wakeup interrupt enable
		struct DBFIE     : Field<LINFlexD_LINIER, DBFIE    , 27>                                         {}; // Data Buffer Full Interrupt enable
		struct DBEIETOIE : Field<LINFlexD_LINIER, DBEIETOIE, 28>                                         {}; // Data Buffer Empty Interrupt enable/Timeout Interrupt Enable
		struct DRIE      : Field<LINFlexD_LINIER, DRIE     , 29>                                         {}; // Data Reception complete Interrupt enable
		struct DTIE      : Field<LINFlexD_LINIER, DTIE     , 30>                                         {}; // Data Transmitted Interrupt enable
		struct HRIE      : Field<LINFlexD_LINIER, HRIE     , 31, 31, GENERIC_SLAVE ? SW_RW : SW_R_HW_RO> {}; // Header Received Interrupt
		
		typedef FieldSet<SZIE, OCIE, BEIE, CEIE, HEIE, FEIE, BOIE, LSIE, WUIE, DBFIE, DBEIETOIE, DRIE, DTIE, HRIE> ALL;
		
		LINFlexD_LINIER(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_LINIER(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_LINIER"); this->SetDescription("LIN Interrupt enable register");
			SZIE     ::SetName("SZIE");      SZIE     ::SetDescription("Stuck at zero Interrupt Enable");
			OCIE     ::SetName("OCIE");      OCIE     ::SetDescription("Output Compare Interrupt Enable");
			BEIE     ::SetName("BEIE");      BEIE     ::SetDescription("Bit Error Interrupt Enable");
			CEIE     ::SetName("CEIE");      CEIE     ::SetDescription("Checksum Error Interrupt Enable");
			HEIE     ::SetName("HEIE");      HEIE     ::SetDescription("Header Error Interrupt Enable");
			FEIE     ::SetName("FEIE");      FEIE     ::SetDescription("Frame Error Interrupt Enable");
			BOIE     ::SetName("BOIE");      BOIE     ::SetDescription("Buffer Overrun Error Interrupt Enable");
			LSIE     ::SetName("LSIE");      LSIE     ::SetDescription("LIN state Interrupt enable");
			WUIE     ::SetName("WUIE");      WUIE     ::SetDescription("Wakeup interrupt enable");
			DBFIE    ::SetName("DBFIE");     DBFIE    ::SetDescription("Data Buffer Full Interrupt enable");
			DBEIETOIE::SetName("DBEIETOIE"); DBEIETOIE::SetDescription("Data Buffer Empty Interrupt enable/Timeout Interrupt Enable");
			DRIE     ::SetName("DRIE");      DRIE     ::SetDescription("Data Reception complete Interrupt enable");
			DTIE     ::SetName("DTIE");      DTIE     ::SetDescription("Data Transmitted Interrupt enable");
			HRIE     ::SetName("HRIE");      HRIE     ::SetDescription("Header Received Interrupt");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
			UpdateInterrupts();
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			UpdateInterrupts();
			return rws;
		}
		
		void UpdateInterrupts()
		{
			this->linflexd->UpdateINT_RX();
			this->linflexd->UpdateINT_TX();
			this->linflexd->UpdateINT_ERR();
		}
		
		using Super::operator =;
	};
	
	// LIN Status Register (LINFlexD_LINSR)
	struct LINFlexD_LINSR : LINFlexD_Register<LINFlexD_LINSR, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_LINSR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x8;
		
		struct AUTOSYNC_COMP : Field<LINFlexD_LINSR, AUTOSYNC_COMP, 12, 12, SW_R_W1C> {}; // Autosynchronization complete
		struct RDC           : Field<LINFlexD_LINSR, RDC          , 13, 15, SW_R    > {}; // Receive Data Byte Count
		struct LINS          : Field<LINFlexD_LINSR, LINS         , 16, 19, SW_R    > {}; // LIN state
		struct RMB           : Field<LINFlexD_LINSR, RMB          , 22, 22, SW_R_W1C> {}; // Release Message Buffer
		struct DRBNE         : Field<LINFlexD_LINSR, DRBNE        , 23, 23, SW_R_W1C> {}; // Data Reception Buffer Not Empty Flag
		struct RXbusy        : Field<LINFlexD_LINSR, RXbusy       , 24, 24, SW_R    > {}; // Receiver Busy flag
		struct RDI           : Field<LINFlexD_LINSR, RDI          , 25, 25, SW_R    > {}; // LIN Receive signal
		struct WUF           : Field<LINFlexD_LINSR, WUF          , 26, 26, SW_R_W1C> {}; // Wakeup flag
		struct DBFF          : Field<LINFlexD_LINSR, DBFF         , 27, 27, SW_R_W1C> {}; // Data Buffer full flag
		struct DBEF          : Field<LINFlexD_LINSR, DBEF         , 28, 28, SW_R_W1C> {}; // Data Buffer empty flag
		struct DRF           : Field<LINFlexD_LINSR, DRF          , 29, 29, SW_R_W1C> {}; // Data Reception Completed flag
		struct DTF           : Field<LINFlexD_LINSR, DTF          , 30, 30, SW_R_W1C> {}; // Data Transmission Completed flag
		struct HRF           : Field<LINFlexD_LINSR, HRF          , 31, 31, SW_R_W1C> {}; // Header Received flag
		
		SWITCH_ENUM_TRAIT(bool, _);
		CASE_ENUM_TRAIT(false, _) { typedef FieldSet<RDC, LINS, RMB, DRBNE, RXbusy, RDI, WUF, DBFF, DBEF, DRF, DTF, HRF> ALL; };
		CASE_ENUM_TRAIT(true, _) { typedef FieldSet<AUTOSYNC_COMP, RDC, LINS, RMB, DRBNE, RXbusy, RDI, WUF, DBFF, DBEF, DRF, DTF, HRF> ALL; };
		typedef typename ENUM_TRAIT(HAS_AUTO_SYNCHRONIZATION_SUPPORT, _)::ALL ALL;
		
		LINFlexD_LINSR(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_LINSR(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_LINSR"); this->SetDescription("LIN Status Register");
			AUTOSYNC_COMP::SetName("AUTOSYNC_COMP"); AUTOSYNC_COMP::SetDescription("Autosynchronization complete");
			RDC          ::SetName("RDC");           RDC          ::SetDescription("Receive Data Byte Count");
			LINS         ::SetName("LINS");          LINS         ::SetDescription("LIN state");
			RMB          ::SetName("RMB");           RMB          ::SetDescription("Release Message Buffer");
			DRBNE        ::SetName("DRBNE");         DRBNE        ::SetDescription("Data Reception Buffer Not Empty Flag");
			RXbusy       ::SetName("RXbusy");        RXbusy       ::SetDescription("Receiver Busy flag");
			RDI          ::SetName("RDI");           RDI          ::SetDescription("LIN Receive signal");
			WUF          ::SetName("WUF");           WUF          ::SetDescription("Wakeup flag");
			DBFF         ::SetName("DBFF");          DBFF         ::SetDescription("Data Buffer full flag");
			DBEF         ::SetName("DBEF");          DBEF         ::SetDescription("Data Buffer empty flag");
			DRF          ::SetName("DRF");           DRF          ::SetDescription("Data Reception Completed flag");
			DTF          ::SetName("DTF");           DTF          ::SetDescription("Data Transmission Completed flag");
			HRF          ::SetName("HRF");           HRF          ::SetDescription("Header Received flag");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
			UpdateInterrupts();
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = RWS_OK;
			
			if((LINS::template Get<uint32_t>(value) == 0xf) && this->linflexd->LINS_INT_RX())
			{
				// Clear Rx interrupt, only when set due to the LIN state event
				this->linflexd->lins_int_rx_mask = true;
				rws = Super::template WritePreserve<LINS>(value, bit_enable);
			}
			else
			{
				rws = Super::Write(value, bit_enable);
			}
			
			UpdateInterrupts();
			
			return rws;
		}
		
		void SoftReset()
		{
			//  All fields except RXbusy and AUTOSYNC_COMP are reset
			this->template Set<RDC  >(0);        
			this->template Set<LINS >(0);        
			this->template Set<RMB  >(0);        
			this->template Set<DRBNE>(0);        
			this->template Set<RDI  >(0);        
			this->template Set<WUF  >(0);        
			this->template Set<DBFF >(0);        
			this->template Set<DBEF >(0);        
			this->template Set<DRF  >(0);        
			this->template Set<DTF  >(0);        
			this->template Set<HRF  >(0);        
			UpdateInterrupts();
		}
		
		void UpdateInterrupts()
		{
			this->linflexd->UpdateINT_RX();
			this->linflexd->UpdateINT_TX();
		}
		
		using Super::operator =;
	};
	
	// LIN Error Status Register (LINFlexD_LINESR)
	struct LINFlexD_LINESR : LINFlexD_Register<LINFlexD_LINESR, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_LINESR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xc;
		
		struct SZF      : Field<LINFlexD_LINESR, SZF  , 16, 16, SW_R_W1C>                              {}; // Stuck at Zero flag
		struct OCF      : Field<LINFlexD_LINESR, OCF  , 17, 17, SW_R_W1C>                              {}; // Output Compare Flag
		struct BEF      : Field<LINFlexD_LINESR, BEF  , 18, 18, SW_R_W1C>                              {}; // Bit Error flag
		struct CEF      : Field<LINFlexD_LINESR, CEF  , 19, 19, SW_R_W1C>                              {}; // Checksum Error flag
		struct SFEF     : Field<LINFlexD_LINESR, SFEF , 20, 20, GENERIC_SLAVE ? SW_R_W1C : SW_R_HW_RO> {}; // Sync Field Error flag
		struct SDEF     : Field<LINFlexD_LINESR, SDEF , 21, 21, GENERIC_SLAVE ? SW_R_W1C : SW_R_HW_RO> {}; // Sync Delimiter Error flag
		struct IDPEF    : Field<LINFlexD_LINESR, IDPEF, 22, 22, GENERIC_SLAVE ? SW_R_W1C : SW_R_HW_RO> {}; // ID Parity Error flag
		struct FEF      : Field<LINFlexD_LINESR, FEF  , 23, 23, SW_R_W1C>                              {}; // Framing Error flag
		struct BOF      : Field<LINFlexD_LINESR, BOF  , 24, 24, SW_R_W1C>                              {}; // Buffer overrun flag
		struct NF       : Field<LINFlexD_LINESR, NF   , 31, 31, SW_R_W1C>                              {}; // Noise flag

		typedef FieldSet<SZF, OCF, BEF, CEF, SFEF, SDEF, IDPEF, FEF, BOF, NF> ALL;
		
		LINFlexD_LINESR(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_LINESR(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_LINESR"); this->SetDescription("LIN Error Status Register");
			SZF     ::SetName("SZF");   SZF     ::SetDescription("Stuck at Zero flag");
			OCF     ::SetName("OCF");   OCF     ::SetDescription("Output Compare Flag");
			BEF     ::SetName("BEF");   BEF     ::SetDescription("Bit Error flag");
			CEF     ::SetName("CEF");   CEF     ::SetDescription("Checksum Error flag");
			SFEF    ::SetName("SFEF");  SFEF    ::SetDescription("Sync Field Error flag");
			SDEF    ::SetName("SDEF");  SDEF    ::SetDescription("Sync Delimiter Error flag");
			IDPEF   ::SetName("IDPEF"); IDPEF   ::SetDescription("ID Parity Error flag");
			FEF     ::SetName("FEF");   FEF     ::SetDescription("Framing Error flag");
			BOF     ::SetName("BOF");   BOF     ::SetDescription("Buffer overrun flag");
			NF      ::SetName("NF");    NF      ::SetDescription("Noise flag");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
			UpdateInterrupts();
		}
		
		void SoftReset()
		{
			this->Initialize(0x0);
			UpdateInterrupts();
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			UpdateInterrupts();
			return rws;
		}
		
		void UpdateInterrupts()
		{
			this->linflexd->UpdateINT_ERR();
		}
		
		using Super::operator =;
	};
	
	// UART Mode Control Register (LINFlexD_UARTCR)
	struct LINFlexD_UARTCR : LINFlexD_Register<LINFlexD_UARTCR, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_UARTCR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x10;
		
		struct MIS        : Field<LINFlexD_UARTCR, MIS       , 0     > {}; // Monitor Idle State
		struct CSP        : Field<LINFlexD_UARTCR, CSP       , 1, 3  > {}; // Configurable Sample Point
		struct OSR        : Field<LINFlexD_UARTCR, OSR       , 4, 7  > {}; // Over Sampling Rate
		struct ROSE       : Field<LINFlexD_UARTCR, ROSE      , 8     > {}; // Reduced Over Sampling Enable
		struct NEF        : Field<LINFlexD_UARTCR, NEF       , 9, 11 > {}; // Number of expected frame
		struct DTU_PCETX  : Field<LINFlexD_UARTCR, DTU_PCETX , 12    > {}; // Disable Timeout in UART mode/Parity transmission and checking
		struct DTU        : Field<LINFlexD_UARTCR, DTU       , 12    > {}; // Disable Timeout in UART mode
		struct PCETX      : Field<LINFlexD_UARTCR, PCETX     , 12    > {}; // Parity transmission and checking
		struct SBUR       : Field<LINFlexD_UARTCR, SBUR      , 13, 14> {}; // Stop bits in UART reception mode
		struct WLS        : Field<LINFlexD_UARTCR, WLS       , 15    > {}; // Special Word Length in UART mode
		struct TDFL_TFC   : Field<LINFlexD_UARTCR, TDFL_TFC  , 16, 18> {}; // Transmitter Data Field Length / TX FIFO Counter
		struct TDFL_TFC_0 : Field<LINFlexD_UARTCR, TDFL_TFC_0, 16, 16> {};
		struct TDFL       : Field<LINFlexD_UARTCR, TDFL      , 17, 18> {}; // Transmitter Data Field Length
		struct TFC        : Field<LINFlexD_UARTCR, TFC       , 16, 18> {}; // TX FIFO Counter
		struct RDFL_RFC   : Field<LINFlexD_UARTCR, RDFL_RFC  , 19, 21> {}; // Reception Data Field Length / RX FIFO Counter
		struct RDFL_RFC_0 : Field<LINFlexD_UARTCR, RDFL_RFC_0, 19, 19> {};
		struct RDFL       : Field<LINFlexD_UARTCR, RDFL      , 20, 21> {}; // Reception Data Field Length
		struct RFC        : Field<LINFlexD_UARTCR, RFC       , 19, 21> {}; // RX FIFO Counter
		struct RFBM       : Field<LINFlexD_UARTCR, RFBM      , 22    > {}; // Rx Fifo/Buffer mode
		struct TFBM       : Field<LINFlexD_UARTCR, TFBM      , 23    > {}; // Tx Fifo/Buffer mode
		struct WL1        : Field<LINFlexD_UARTCR, WL1       , 24    > {}; // Word Length in UART mode
		struct PC1        : Field<LINFlexD_UARTCR, PC1       , 25    > {}; // Parity Control
		struct RxEn       : Field<LINFlexD_UARTCR, RxEn      , 26    > {}; // Receiver Enable
		struct TxEn       : Field<LINFlexD_UARTCR, TxEn      , 27    > {}; // Transmitter Enable
		struct PC0        : Field<LINFlexD_UARTCR, PC0       , 28    > {}; // Parity Control
		struct PCE        : Field<LINFlexD_UARTCR, PCE       , 29    > {}; // Parity Control Enable
		struct WL0        : Field<LINFlexD_UARTCR, WL0       , 30    > {}; // Word Length in UART mode
		struct UART       : Field<LINFlexD_UARTCR, UART      , 31    > {}; // UART Mode
		
		typedef FieldSet<MIS, CSP, OSR, ROSE, NEF, DTU_PCETX, SBUR, WLS, TDFL_TFC, RDFL_RFC, RFBM, TFBM, WL1, PC1, RxEn, TxEn, PC0, PCE, WL0, UART> ALL;
		
		// fields protection per mode:
		//
		//                !INIT & !UART    INIT & !UART   !INIT & UART     INIT & UART &    INIT & UART &     INIT & UART &    INIT & UART &
		//                                                               TXFIFO & RXFIFO    TXBUF & RXFIFO    TXFIFO & RXBUF   TXBUF & RXBUF
		// MIS                 RO               RW             RO               RW               RW                 RW              RW
		// CSP                 RO               RW             RO               RW               RW                 RW              RW
		// OSR                 RO               RW             RO               RW               RW                 RW              RW
		// ROSE                RO               RW             RO               RW               RW                 RW              RW
		// NEF                 RW               RW             RW               RW               RW                 RW              RW
		// DTU_PCETX           RO               RO             RO               RW               RW                 RW              RW
		// SBUR                RO               RW             RO               RW               RW                 RW              RW
		// WLS                 RO               RO             RO               RW               RW                 RW              RW
		// TFDL_TFC[0]         RO               RO             RO               RO               RO                 RO              RO
		// TDFL_TFC[1:2]       RO               RO             RO               RO               RW                 RO              RW
		// RDFL_RFC[0]         RO               RO             RO               RO               RO                 RO              RO
		// RDFL_RFC[1:2]       RO               RO             RO               RO               RO                 RW              RW
		// RFBM                RO               RO             RO               RW               RW                 RW              RW
		// TFBM                RO               RO             RO               RW               RW                 RW              RW
		// WL1                 RO               RO             RO               RW               RW                 RW              RW
		// PC1                 RO               RO             RO               RW               RW                 RW              RW
		// RxEn                RO               RO             RW               RW               RW                 RW              RW
		// TxEn                RO               RO             RW               RW               RW                 RW              RW
		// PC0                 RO               RO             RO               RW               RW                 RW              RW
		// PCE                 RO               RO             RO               RW               RW                 RW              RW
		// WL0                 RO               RO             RO               RW               RW                 RW              RW
		// UART                RO               RW             RO               RW               RW                 RW              RW

		// MIS, CSP, OSR, ROSE, DTU_PCETX, SBUR, WLS, TDFL_TFC, RDFL_RFC, RFBM, TFBM, WL1, PC1, RxEn, TxEn, PC0, PCE, WL0, and UART
		// are read-only when not in init mode and not in UART mode
		typedef FieldSet<MIS, CSP, OSR, ROSE, DTU_PCETX, SBUR, WLS, TDFL_TFC, RDFL_RFC, RFBM, TFBM, WL1, PC1, RxEn, TxEn, PC0, PCE, WL0, UART> NOT_INIT_AND_NOT_UART_PRESERVED_FIELDS;
		
		// DTU_PCETX, WLS, TDFL_TFC, RDFL_RFC, RFBM, TFBM, WL1, PC1, RxEn, TxEn, PC0, PCE, WL0
		// are read-only in init mode and not in UART mode
		typedef FieldSet<DTU_PCETX, WLS, TDFL_TFC, RDFL_RFC, RFBM, TFBM, WL1, PC1, RxEn, TxEn, PC0, PCE, WL0> INIT_AND_NOT_UART_PRESERVED_FIELDS;
		
		// MIS, CSP, OSR, ROSE, DTU_PCETX, SBUR, WLS, TDFL_TFC, RDFL_RFC, RFBM, TFBM, WL1, PC1, PC0, PCE, WL0, and UART
		// are read-only when not in init mode and in UART mode 
		typedef FieldSet<MIS, CSP, OSR, ROSE, DTU_PCETX, SBUR, WLS, TDFL_TFC, RDFL_RFC, RFBM, TFBM, WL1, PC1, PC0, PCE, WL0, UART> NOT_INIT_AND_UART_PRESERVED_FIELDS;
		
		// TFC is read-only is Tx FIFO mode, and RFC is read-only Rx FIFO mode
		typedef FieldSet<TFC, RFC> INIT_AND_UART_AND_TXFIFO_AND_RXFIFO_PRESERVED_FIELDS;
		
		// upper bit of TDFL_TFC is read-only in Tx buffer mode, and RFC is read-only in Rx FIFO mode
		typedef FieldSet<TDFL_TFC_0, RFC> INIT_AND_UART_AND_TXBUF_AND_RXFIFO_PRESERVED_FIELDS;
		
		// TFC is read-only in Tx FIFO mode, and upper bit of RDFL_RFC is read-only is Tx buffer mode
		typedef FieldSet<TFC, RDFL_RFC_0> INIT_AND_UART_AND_TXFIFO_AND_RXBUF_PRESERVED_FIELDS;
		
		// upper bit of TDFL_TFC is read-only in Tx buffer mode, and upper bit of RDFL_RFC is read-only in Rx buffer mode
		typedef FieldSet<TDFL_TFC_0, RDFL_RFC_0> INIT_AND_UART_AND_TXBUF_AND_RXBUF_PRESERVED_FIELDS;

		LINFlexD_UARTCR(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_UARTCR(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_UARTCR"); this->SetDescription("UART Mode Control Register");
			MIS      ::SetName("MIS");       MIS      ::SetDescription("Monitor Idle State");
			CSP      ::SetName("CSP");       CSP      ::SetDescription("Configurable Sample Point");
			OSR      ::SetName("OSR");       OSR      ::SetDescription("Over Sampling Rate");
			ROSE     ::SetName("ROSE");      ROSE     ::SetDescription("Reduced Over Sampling Enable");
			NEF      ::SetName("NEF");       NEF      ::SetDescription("Number of expected frame");
			DTU_PCETX::SetName("DTU_PCETX"); DTU_PCETX::SetDescription("Disable Timeout in UART mode/Parity transmission and checking");
			DTU      ::SetName("DTU");       DTU      ::SetDescription("Disable Timeout in UART mode");
			PCETX    ::SetName("PCETX");     PCETX    ::SetDescription("Parity transmission and checking");
			SBUR     ::SetName("SBUR");      SBUR     ::SetDescription("Stop bits in UART reception mode");
			WLS      ::SetName("WLS");       WLS      ::SetDescription("Special Word Length in UART mode");
			TDFL_TFC ::SetName("TDFL_TFC");  TDFL_TFC ::SetDescription("Transmitter Data Field Length/TX FIFO Counter");
			TDFL     ::SetName("TDFL");      TDFL     ::SetDescription("Transmitter Data Field Length");
			TFC      ::SetName("TFC");       TFC      ::SetDescription("TX FIFO Counter");
			RDFL_RFC ::SetName("RDFL_RFC");  RDFL_RFC ::SetDescription("Reception Data Field Length /RX FIFO Counter");
			RDFL     ::SetName("RDFL");      RDFL     ::SetDescription("Reception Data Field Length");
			RFC      ::SetName("RFC");       RFC      ::SetDescription("RX FIFO Counter");
			RFBM     ::SetName("RFBM");      RFBM     ::SetDescription("Rx Fifo/Buffer mode");
			TFBM     ::SetName("TFBM");      TFBM     ::SetDescription("Tx Fifo/Buffer mode");
			WL1      ::SetName("WL1");       WL1      ::SetDescription("Word Length in UART mode");
			PC1      ::SetName("PC1");       PC1      ::SetDescription("Parity Control");
			RxEn     ::SetName("RxEn");      RxEn     ::SetDescription("Receiver Enable");
			TxEn     ::SetName("TxEn");      TxEn     ::SetDescription("Transmitter Enable");
			PC0      ::SetName("PC0");       PC0      ::SetDescription("Parity Control");
			PCE      ::SetName("PCE");       PCE      ::SetDescription("Parity Control Enable");
			WL0      ::SetName("WL0");       WL0      ::SetDescription("Word Length in UART mode");
			UART     ::SetName("UART");      UART     ::SetDescription("UART Mode");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			bool old_RxEn = this->template Get<RxEn>();
			
			ReadWriteStatus rws =
				/* !INIT && !UART */
				(!this->linflexd->InitMode() && !this->linflexd->UART_Mode()) ?
				Super::template WritePreserve<NOT_INIT_AND_NOT_UART_PRESERVED_FIELDS>(value, bit_enable) :
				/* INIT & !UART */
				((this->linflexd->InitMode() && !this->linflexd->UART_Mode()) ?
				Super::template WritePreserve<INIT_AND_NOT_UART_PRESERVED_FIELDS>(value, bit_enable) :
				/* !INIT & UART */
				((!this->linflexd->InitMode() && this->linflexd->UART_Mode()) ?
				Super::template WritePreserve<NOT_INIT_AND_UART_PRESERVED_FIELDS>(value, bit_enable) :
				/* INIT & UART & TXFIFO & RXFIFO */
				((this->linflexd->InitMode() && this->linflexd->UART_Mode() && this->linflexd->UART_TX_FIFO_Mode() && this->linflexd->UART_RX_FIFO_Mode()) ?
				Super::template WritePreserve<INIT_AND_UART_AND_TXFIFO_AND_RXFIFO_PRESERVED_FIELDS>(value, bit_enable) :
				/* INIT & UART & TXBUF & RXFIFO */
				((this->linflexd->InitMode() && this->linflexd->UART_Mode() && this->linflexd->UART_TX_BufferMode()  && this->linflexd->UART_RX_FIFO_Mode()) ?
				Super::template WritePreserve<INIT_AND_UART_AND_TXBUF_AND_RXFIFO_PRESERVED_FIELDS>(value, bit_enable) :
				/* INIT & UART & TXFIFO & RXBUF */
				((this->linflexd->InitMode() && this->linflexd->UART_Mode() && this->linflexd->UART_TX_FIFO_Mode() && this->linflexd->UART_RX_BufferMode()) ?
				Super::template WritePreserve<INIT_AND_UART_AND_TXFIFO_AND_RXBUF_PRESERVED_FIELDS>(value, bit_enable) :
				/* INIT & UART & TXBUF & RXBUF */
				((this->linflexd->InitMode() && this->linflexd->UART_Mode() && this->linflexd->UART_TX_BufferMode()  && this->linflexd->UART_RX_BufferMode()) ?
				Super::template WritePreserve<INIT_AND_UART_AND_TXBUF_AND_RXBUF_PRESERVED_FIELDS>(value, bit_enable) :
				/* other: always false */
				RWS_ANA))))));
			
			bool new_RxEn = this->template Get<RxEn>();
			
			if(this->template Get<WLS>() && !this->template Get<WL1>())
			{
				this->linflexd->logger << DebugWarning << "In " << this->GetName() << ", when WLS bit = 1, WL = 0 or 1 should not be used, since this will lead to incorrect reception of data" << EndDebugWarning;
			}
			
			if(this->linflexd->UART_RX_BufferMode() && (this->template Get<WLS>() || this->template Get<WL1>()) && ((this->template Get<RDFL_RFC>() == 0) || (this->template Get<RDFL_RFC>() == 2)))
			{
				this->linflexd->logger << DebugWarning << "In " << this->GetName() << ", only configuration " << RDFL_RFC::GetName() << " = x01b or " << RDFL_RFC::GetName() << " = x11b are allowed when WLS bit = 1 or WL = 1xb" << EndDebugWarning;
			}
			
			if(!old_RxEn && new_RxEn)
			{
				this->linflexd->ResetTimeout();
			}
			
			this->linflexd->UpdateLINClock(); // ROSE and OSR affects baud period that also depends on LIN clock
			
			return rws;
		}
		
		void SoftReset()
		{
			// Only TFC & RFC are reset
			this->template Set<TDFL_TFC>(0);
			this->template Set<RDFL_RFC>(0);
		}

		using Super::operator =;
	};
	
	// UART Mode Status Register (LINFlexD_UARTSR)
	struct LINFlexD_UARTSR : LINFlexD_Register<LINFlexD_UARTSR, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_UARTSR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x14;
		
		struct SZF    : Field<LINFlexD_UARTSR, SZF   , 16, 16, SW_R_W1C> {}; // Stuck at Zero flag
		struct OCF    : Field<LINFlexD_UARTSR, OCF   , 17, 17, SW_R_W1C> {}; // Output Compare Flag
		struct PE     : Field<LINFlexD_UARTSR, PE    , 18, 21, SW_R_W1C> {}; // Parity Error flag
		struct PE3    : Field<LINFlexD_UARTSR, PE    , 18>               {};
		struct PE2    : Field<LINFlexD_UARTSR, PE    , 19>               {};
		struct PE1    : Field<LINFlexD_UARTSR, PE    , 20>               {};
		struct PE0    : Field<LINFlexD_UARTSR, PE    , 21>               {};
		struct RMB    : Field<LINFlexD_UARTSR, RMB   , 22, 22, SW_R_W1C> {}; // Release Message Buffer
		struct FEF    : Field<LINFlexD_UARTSR, FEF   , 23, 23, SW_R_W1C> {}; // Framing Error flag
		struct BOF    : Field<LINFlexD_UARTSR, BOF   , 24, 24, SW_R_W1C> {}; // FIFO/Buffer overrun flag
		struct RDI    : Field<LINFlexD_UARTSR, RDI   , 25, 25, SW_R_W1C> {}; // Receiver Data Input signal
		struct WUF    : Field<LINFlexD_UARTSR, WUF   , 26, 26, SW_R_W1C> {}; // Wakeup flag
		struct RFNE   : Field<LINFlexD_UARTSR, RFNE  , 27, 27, SW_R    > {}; // Receive FIFO Not Empty
		struct TO     : Field<LINFlexD_UARTSR, TO    , 28, 28, SW_R_W1C> {}; // Timeout
		struct DRFRFE : Field<LINFlexD_UARTSR, DRFRFE, 29, 29, SW_R_W1C> {}; // Data Reception Completed Flag / Rx FIFO Empty Flag
		struct DRF    : Field<LINFlexD_UARTSR, DRF   , 29>               {}; // Data Reception Completed Flag
		struct RFE    : Field<LINFlexD_UARTSR, RFE   , 29>               {}; // Rx FIFO Empty Flag
		struct DTFTFF : Field<LINFlexD_UARTSR, DTFTFF, 30, 30, SW_R_W1C> {}; // Data Transmission Completed Flag/ TX FIFO Full Flag
		struct DTF    : Field<LINFlexD_UARTSR, DTF   , 30>               {}; // Data Transmission Completed Flag
		struct TFF    : Field<LINFlexD_UARTSR, TFF   , 30>               {}; // TX FIFO Full Flag
		struct NF     : Field<LINFlexD_UARTSR, NF    , 31, 31, SW_R_W1C> {}; // Noise flag

		typedef FieldSet<SZF, OCF, PE, RMB, FEF, BOF, RDI, WUF, RFNE, TO, DRFRFE, DTFTFF, NF> ALL;
		
		typedef FieldSet<RFE, TFF> RX_TX_FIFO_MODE_PRESERVED_FIELDS; // RFE and TFF are read-only when Rx and Tx are in FIFO mode
		typedef FieldSet<RFE> RX_FIFO_MODE_PRESERVED_FIELDS;         // RFE is read-only when Rx is in FIFO mode
		typedef FieldSet<TFF> TX_FIFO_MODE_PRESERVED_FIELDS;         // TFF is read-only when Tx is in FIFO mode
		
		LINFlexD_UARTSR(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_UARTSR(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_UARTSR"); this->SetDescription("UART Mode Status Register");
			SZF   ::SetName("SZF");    SZF   ::SetDescription("Stuck at Zero flag");
			OCF   ::SetName("OCF");    OCF   ::SetDescription("Output Compare Flag");
			PE    ::SetName("PE");     PE    ::SetDescription("Parity Error flag");
			RMB   ::SetName("RMB");    RMB   ::SetDescription("Release Message Buffer");
			FEF   ::SetName("FEF");    FEF   ::SetDescription("Framing Error flag");
			BOF   ::SetName("BOF");    BOF   ::SetDescription("FIFO/Buffer overrun flag");
			RDI   ::SetName("RDI");    RDI   ::SetDescription("Receiver Data Input signal");
			WUF   ::SetName("WUF");    WUF   ::SetDescription("Wakeup flag");
			RFNE  ::SetName("RFNE");   RFNE  ::SetDescription("Receive FIFO Not Empty");
			TO    ::SetName("TO");     TO    ::SetDescription("Timeout");
			DRFRFE::SetName("DRFRFE"); DRFRFE::SetDescription("Data Reception Completed Flag / Rx FIFO Empty Flag");
			DRF   ::SetName("DRF");    DRF   ::SetDescription("Data Reception Completed Flag");
			RFE   ::SetName("RFE");    RFE   ::SetDescription("Rx FIFO Empty Flag");
			DTFTFF::SetName("DTFTFF"); DTFTFF::SetDescription("Data Transmission Completed Flag / TX FIFO Full Flag");
			DTF   ::SetName("DTF");    DTF   ::SetDescription("Data Transmission Completed Flag");
			TFF   ::SetName("TFF");    TFF   ::SetDescription("TX FIFO Full Flag");
			NF    ::SetName("NF");     NF    ::SetDescription("Noise flag");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
			UpdateInterrupts();
		}
		
		void SoftReset()
		{
			//  All fields except RFE & TFF are reset
			this->template Set<SZF   >(0);
			this->template Set<OCF   >(0);
			this->template Set<PE    >(0);
			this->template Set<RMB   >(0);
			this->template Set<FEF   >(0);
			this->template Set<BOF   >(0);
			this->template Set<RDI   >(0);
			this->template Set<WUF   >(0);
			this->template Set<RFNE  >(0);
			this->template Set<TO    >(0);
			this->template Set<NF    >(0);
			UpdateInterrupts();
		}
		
		uint32_t Compound()
		{
			uint32_t compound = 0;
			SZF   ::template Set<uint32_t>(compound, this->linflexd->linflexd_linesr.template Get<typename LINFlexD_LINESR::SZF>());
			OCF   ::template Set<uint32_t>(compound, this->linflexd->linflexd_linesr.template Get<typename LINFlexD_LINESR::OCF>());
			PE3   ::template Set<uint32_t>(compound, this->linflexd->linflexd_linesr.template Get<typename LINFlexD_LINESR::BEF>());
			PE2   ::template Set<uint32_t>(compound, this->linflexd->linflexd_linesr.template Get<typename LINFlexD_LINESR::CEF>());
			PE1   ::template Set<uint32_t>(compound, this->linflexd->linflexd_linesr.template Get<typename LINFlexD_LINESR::SFEF>());
			PE0   ::template Set<uint32_t>(compound, this->linflexd->linflexd_linesr.template Get<typename LINFlexD_LINESR::SDEF>());
			RMB   ::template Set<uint32_t>(compound, this->linflexd->linflexd_linesr.template Get<typename LINFlexD_LINESR::IDPEF>());
			FEF   ::template Set<uint32_t>(compound, this->linflexd->linflexd_linesr.template Get<typename LINFlexD_LINESR::FEF>());
			BOF   ::template Set<uint32_t>(compound, this->linflexd->linflexd_linesr.template Get<typename LINFlexD_LINESR::BOF>());
			RDI   ::template Set<uint32_t>(compound, this->template Get<RDI>());
			WUF   ::template Set<uint32_t>(compound, this->linflexd->linflexd_linsr.template Get<typename LINFlexD_LINSR::WUF>());
			TO    ::template Set<uint32_t>(compound, this->template Get<TO>());
			DRFRFE::template Set<uint32_t>(compound, this->linflexd->linflexd_linsr.template Get<typename LINFlexD_LINSR::DRF>());
			DTFTFF::template Set<uint32_t>(compound, this->linflexd->linflexd_linsr.template Get<typename LINFlexD_LINSR::DTF>());
			NF    ::template Set<uint32_t>(compound, this->linflexd->linflexd_linesr.template Get<typename LINFlexD_LINESR::NF>());
			return compound;
		}
		
		virtual ReadWriteStatus Read(uint32_t& value, const uint32_t& bit_enable)
		{
			if(this->linflexd->InitMode() && this->linflexd->UART_Mode())
			{
				// Init mode and UART mode: value of UARTSR is a compound because lot of flags reflects flags in LINSR and LINESR
				value = (value & ~bit_enable) | (Compound() & bit_enable);
				return RWS_OK;
			}
			else
			{
				return Super::Read(value, bit_enable);
			}
		}

		virtual void DebugRead(uint32_t& value, const uint32_t& bit_enable)
		{
			if(this->linflexd->InitMode() && this->linflexd->UART_Mode())
			{
				// Init mode and UART mode: value of UARTSR is a compound because lot of UARTSR flags reflects flags in LINSR and LINESR
				value = (value & ~bit_enable) | (Compound() & bit_enable);
			}
			else
			{
				Super::DebugRead(value, bit_enable);
			}
		}

		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = ( this->linflexd->UART_RX_FIFO_Mode() &&  this->linflexd->UART_TX_FIFO_Mode() ) ? Super::template WritePreserve<RX_TX_FIFO_MODE_PRESERVED_FIELDS>(value, bit_enable) :
			                      ((!this->linflexd->UART_RX_FIFO_Mode() &&  this->linflexd->UART_TX_FIFO_Mode()) ? Super::template WritePreserve<TX_FIFO_MODE_PRESERVED_FIELDS>(value, bit_enable)    :
								  (( this->linflexd->UART_RX_FIFO_Mode() && !this->linflexd->UART_TX_FIFO_Mode()) ? Super::template WritePreserve<RX_FIFO_MODE_PRESERVED_FIELDS>(value, bit_enable)    :
								  Super::Write(value, bit_enable)));
			
			UpdateInterrupts();
			return rws;
		}
		
		void UpdateInterrupts()
		{
			this->linflexd->UpdateINT_RX();
			this->linflexd->UpdateINT_TX();
			this->linflexd->UpdateINT_ERR();
		}
		
		using Super::operator =;
	};
	
	// LIN Time-Out Control Status Register (LINFlexD_LINTCSR)
	struct LINFlexD_LINTCSR : LINFlexD_Register<LINFlexD_LINTCSR, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_LINTCSR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x18;
		
		struct MODE : Field<LINFlexD_LINTCSR, MODE, 21, 21, SW_RW> {}; // Time-out counter mode
		struct IOT  : Field<LINFlexD_LINTCSR, IOT , 22, 22, SW_RW> {}; // Idle on timeout
		struct TOCE : Field<LINFlexD_LINTCSR, TOCE, 23, 23, SW_RW> {}; // Time-out counter enable
		struct CNT  : Field<LINFlexD_LINTCSR, CNT , 24, 31, SW_R > {}; // Counter Value
		
		typedef FieldSet<MODE, IOT, TOCE, CNT> ALL;
		
		typedef FieldSet<MODE, IOT> INIT_MODE_ONLY_WRITEABLE;
		
		LINFlexD_LINTCSR(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_LINTCSR(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_LINTCSR"); this->SetDescription("LIN Time-Out Control Status Register");
			MODE::SetName("MODE"); MODE::SetDescription("MODE");
			IOT ::SetName("IOT");  IOT ::SetDescription("IOT");
			TOCE::SetName("TOCE"); TOCE::SetDescription("TOCE");
			CNT ::SetName("CNT");  CNT ::SetDescription("CNT");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = this->linflexd->InitMode()
			            ? (this->linflexd->LINTimeoutCounterMode() ? Super::Write(value, bit_enable)
			                                                       : Super::template WritePreserve<TOCE>(value, bit_enable))
			            : Super::template WritePreserve<INIT_MODE_ONLY_WRITEABLE>(value, bit_enable);
			
			return rws;
		}
		
		void SoftReset()
		{
			this->template Set<CNT>(0);
		}
		
		using Super::operator =;
	};
	
	// LIN Output Compare Register (LINFlexD_LINOCR)
	struct LINFlexD_LINOCR : LINFlexD_Register<LINFlexD_LINOCR, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_LINOCR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x1c;
		
		struct OC1 : Field<LINFlexD_LINOCR, OC1, 16, 23> {}; // Output compare value 2
		struct OC2 : Field<LINFlexD_LINOCR, OC2, 24, 31> {}; // Output compare value 1
		
		typedef FieldSet<OC1, OC2> ALL;
		
		LINFlexD_LINOCR(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_LINOCR(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_LINOCR"); this->SetDescription("LIN Output Compare Register");
			OC1::SetName("OC1"); OC1::SetDescription("OC1");
			OC2::SetName("OC2"); OC2::SetDescription("OC2");
		}
		
		void Reset()
		{
			this->template Set<OC1>(~uint32_t(0));
			this->template Set<OC2>(~uint32_t(0));
		}
		
		using Super::operator =;
	};
	
	// LIN Time-Out Control Register (LINFlexD_LINTOCR)
	struct LINFlexD_LINTOCR : LINFlexD_Register<LINFlexD_LINTOCR, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_LINTOCR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x20;
		
		struct RTO : Field<LINFlexD_LINTOCR, RTO, 20, 23> {}; // Response timeout value
		struct HTO : Field<LINFlexD_LINTOCR, HTO, 25, 31> {}; // Header timeout value
		
		typedef FieldSet<RTO, HTO> ALL;
		
		LINFlexD_LINTOCR(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_LINTOCR(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_LINTOCR"); this->SetDescription("LIN Time-Out Control Register");
			RTO::SetName("RTO"); RTO::SetDescription("Response timeout value");
			HTO::SetName("HTO"); HTO::SetDescription("Header timeout value");
		}
		
		void Reset()
		{
			this->template Set<RTO>(0xe);
			this->template Set<HTO>(GENERIC_SLAVE ? 0x2c : 0x1c);
		}
		
		using Super::operator =;
	};
	
	// LIN Fractional Baud Rate Register (LINFlexD_LINFBRR)
	struct LINFlexD_LINFBRR : LINFlexD_Register<LINFlexD_LINFBRR, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_LINFBRR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x24;
		
		struct FBR : Field<LINFlexD_LINFBRR, FBR, 28, 31> {}; // Fractional Baud rates
		
		typedef FieldSet<FBR> ALL;
		
		LINFlexD_LINFBRR(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_LINFBRR(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_LINFBRR"); this->SetDescription("LIN Fractional Baud Rate Register");
			FBR::SetName("FBR"); FBR::SetDescription("Fractional Baud rates");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = this->linflexd->InitMode() ? Super::Write(value, bit_enable)
			                                                 : Super::template WritePreserve<FBR>(value, bit_enable);
			
			return rws;
		}

		using Super::operator =;
	};
	
	// LIN Integer Baud Rate Register (LINFlexD_LINIBRR)
	struct LINFlexD_LINIBRR : LINFlexD_Register<LINFlexD_LINIBRR, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_LINIBRR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x28;
		
		struct IBR : Field<LINFlexD_LINIBRR, IBR, 12, 31> {}; // Integer Baud rates

		typedef FieldSet<IBR> ALL;
		
		LINFlexD_LINIBRR(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_LINIBRR(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_LINIBRR"); this->SetDescription("LIN Integer Baud Rate Register");
			IBR::SetName("IBR"); IBR::SetDescription("Integer Baud rates");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
			this->linflexd->UpdateLINClock();
		}

		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = this->linflexd->InitMode() ? Super::Write(value, bit_enable)
			                                                 : Super::template WritePreserve<IBR>(value, bit_enable);
			
			this->linflexd->UpdateLINClock();
			
			return rws;
		}

		using Super::operator =;
	};
	
	// LIN Checksum Field Register (LINFlexD_LINCFR)
	struct LINFlexD_LINCFR : LINFlexD_Register<LINFlexD_LINCFR, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_LINCFR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x2c;
		
		struct CF : Field<LINFlexD_LINCFR, CF, 24, 31> {}; //  Checksum bits
		
		typedef FieldSet<CF> ALL;
		
		LINFlexD_LINCFR(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_LINCFR(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_LINCFR"); this->SetDescription("LIN Checksum Field Register");
			CF::SetName("CF"); CF::SetDescription("LIN Checksum Field Register");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
			this->linflexd->UpdateLINClock();
		}

		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = this->linflexd->SoftwareChecksumCalculation() ? Super::Write(value, bit_enable)
			                                                                    : Super::template WritePreserve<CF>(value, bit_enable);
			
			this->linflexd->UpdateLINClock();
			
			return rws;
		}

		using Super::operator =;
	};
	
	// LIN Control Register 2 (LINFlexD_LINCR2)
	struct LINFlexD_LINCR2 : LINFlexD_Register<LINFlexD_LINCR2, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_LINCR2, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x30;
		
		struct TBDE : Field<LINFlexD_LINCR2, TBDE, 16> {}; // Two Bit delimiter bit
		struct IOBE : Field<LINFlexD_LINCR2, IOBE, 17> {}; // Idle on Bit Error
		struct IOPE : Field<LINFlexD_LINCR2, IOPE, 18> {}; // Idle on Identifier Parity Error
		struct WURQ : Field<LINFlexD_LINCR2, WURQ, 19> {}; // Wakeup Generate Request
		struct DDRQ : Field<LINFlexD_LINCR2, DDRQ, 20> {}; // Data Discard request
		struct DTRQ : Field<LINFlexD_LINCR2, DTRQ, 21> {}; // Data Transmission Request
		struct ABRQ : Field<LINFlexD_LINCR2, ABRQ, 22> {}; // Abort Request
		struct HTRQ : Field<LINFlexD_LINCR2, HTRQ, 23> {}; // Header Transmission Request
		
		typedef FieldSet<TBDE, IOBE, IOPE, WURQ, DDRQ, DTRQ, ABRQ, HTRQ> ALL;
		
		typedef FieldSet<TBDE, IOBE, IOPE> INIT_MODE_ONLY_WRITEABLE;
		
		LINFlexD_LINCR2(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_LINCR2(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_LINCR2"); this->SetDescription("LIN Control Register 2");
			TBDE::SetName("TBDE"); TBDE::SetDescription("Two Bit delimiter bit");
			IOBE::SetName("IOBE"); IOBE::SetDescription("Idle on Bit Error");
			IOPE::SetName("IOPE"); IOPE::SetDescription("Idle on Identifier Parity Error");
			WURQ::SetName("WURQ"); WURQ::SetDescription("Wakeup Generate Request");
			DDRQ::SetName("DDRQ"); DDRQ::SetDescription("Data Discard request");
			DTRQ::SetName("DTRQ"); DTRQ::SetDescription("Data Transmission Request");
			ABRQ::SetName("ABRQ"); ABRQ::SetDescription("Abort Request");
			HTRQ::SetName("HTRQ"); HTRQ::SetDescription("Header Transmission Request");
		}
		
		void Reset()
		{
			this->template Set<TBDE>(0);
			this->template Set<IOBE>(1);
			this->template Set<IOPE>(GENERIC_SLAVE ? 1 : 0);
			this->template Set<WURQ>(0);
			this->template Set<DDRQ>(0);
			this->template Set<DTRQ>(0);
			this->template Set<ABRQ>(0);
			this->template Set<HTRQ>(0);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = this->linflexd->InitMode() ? Super::Write(value, bit_enable)
			                                                 : Super::template WritePreserve<INIT_MODE_ONLY_WRITEABLE>(value, bit_enable);
			
			return rws;
		}

		using Super::operator =;
	};
	
	// Buffer Identifier Register (LINFlexD_BIDR)
	struct LINFlexD_BIDR : LINFlexD_Register<LINFlexD_BIDR, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_BIDR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x34;
		
		struct DFL : Field<LINFlexD_BIDR, DFL, 16, 21> {}; // Data Field Length
		struct DIR : Field<LINFlexD_BIDR, DIR, 22    > {}; // Direction
		struct CCS : Field<LINFlexD_BIDR, CCS, 23    > {}; // Classic Checksum
		struct ID  : Field<LINFlexD_BIDR, ID , 26, 31> {}; // Identifier

		typedef FieldSet<DFL, DIR, CCS, ID> ALL;
		
		LINFlexD_BIDR(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_BIDR(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_BIDR"); this->SetDescription("Buffer Identifier Register");
			DFL::SetName("DFL"); DFL::SetDescription("Data Field Length");
			DIR::SetName("DIR"); DIR::SetDescription("Direction");
			CCS::SetName("CCS"); CCS::SetDescription("Classic Checksum");
			ID ::SetName("ID");  ID ::SetDescription("Identifier");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = this->linflexd->LIN_MasterMode() ? Super::Write(value, bit_enable)
			                                                   : Super::template WritePreserve<ID>(value, bit_enable);
			
			return rws;
		}
		
		using Super::operator =;
	};
	
	// Buffer Data Register Least Significant (LINFlexD_BDRL)
	struct LINFlexD_BDRL : LINFlexD_Register<LINFlexD_BDRL, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_BDRL, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x38;
		
		struct DATA3 : Field<LINFlexD_BDRL, DATA3,  0, 7 > {}; // Data Byte 3
		struct DATA2 : Field<LINFlexD_BDRL, DATA2,  8, 15> {}; // Data Byte 2
		struct DATA1 : Field<LINFlexD_BDRL, DATA1, 16, 23> {}; // Data Byte 1
		struct DATA0 : Field<LINFlexD_BDRL, DATA0, 24, 31> {}; // Data Byte 0
		
		typedef FieldSet<DATA3, DATA2, DATA1, DATA0> ALL;
		
		LINFlexD_BDRL(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_BDRL(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_BDRL"); this->SetDescription("Buffer Data Register Least Significant");
			DATA3::SetName("DATA3"); DATA3::SetDescription("Data Byte 3");
			DATA2::SetName("DATA2"); DATA2::SetDescription("Data Byte 2");
			DATA1::SetName("DATA1"); DATA1::SetDescription("Data Byte 1");
			DATA0::SetName("DATA0"); DATA0::SetDescription("Data Byte 0");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		virtual ReadWriteStatus Read(uint32_t& value, const uint32_t& bit_enable)
		{
			if(this->linflexd->UART_Mode())
			{
				if(this->linflexd->UART_TX_FIFO_Mode())
				{
					// In UART FIFO mode (UARTCR[TFBM] = 1), any read operation causes an IPS transfer error.
					return RWS_ANA;
				}
			}
			
			ReadWriteStatus rws = Super::Read(value, bit_enable);
			
			return rws;
		}

		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			if(this->linflexd->UART_Mode())
			{
				if(this->linflexd->UART_TX_FIFO_Mode())
				{
					switch(this->linflexd->UARTWordLength())
					{
						case 1: if(bit_enable & FieldSet<DATA3, DATA2, DATA1>::template GetMask<uint32_t>()) return RWS_ANA; break;
						case 2: if(bit_enable & FieldSet<DATA3, DATA2>::template GetMask<uint32_t>()) return RWS_ANA; break;
					}
					
					this->linflexd->TX_FIFO_Push();
				}
			}
			
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws) && this->linflexd->UART_Mode())
			{
				// Writing to BDRL make UART transmit buffer if Tx is enabled
				this->linflexd->Transmit();
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// Buffer Data Register Most Significant (LINFlexD_BDRM)
	struct LINFlexD_BDRM : LINFlexD_Register<LINFlexD_BDRM, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_BDRM, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x3c;
		
		struct DATA7 : Field<LINFlexD_BDRM, DATA7,  0,  7> {}; // Data Byte 7
		struct DATA6 : Field<LINFlexD_BDRM, DATA6,  8, 15> {}; // Data Byte 6
		struct DATA5 : Field<LINFlexD_BDRM, DATA5, 16, 23> {}; // Data Byte 5
		struct DATA4 : Field<LINFlexD_BDRM, DATA4, 24, 31> {}; // Data Byte 4
		
		typedef FieldSet<DATA7, DATA6, DATA5, DATA4> ALL;
		
		LINFlexD_BDRM(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }		
		LINFlexD_BDRM(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_BDRM"); this->SetDescription("Buffer Data Register Most Significant");
			DATA7::SetName("DATA7"); DATA7::SetDescription("Data Byte 7");
			DATA6::SetName("DATA6"); DATA6::SetDescription("Data Byte 6");
			DATA5::SetName("DATA5"); DATA5::SetDescription("Data Byte 5");
			DATA4::SetName("DATA4"); DATA4::SetDescription("Data Byte 4");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		virtual ReadWriteStatus Read(uint32_t& value, const uint32_t& bit_enable)
		{
			if(this->linflexd->UART_Mode())
			{
				if(this->linflexd->UART_RX_FIFO_Mode())
				{
					switch(this->linflexd->UARTWordLength())
					{
						case 1: if(bit_enable & FieldSet<DATA7, DATA6, DATA5>::template GetMask<uint32_t>()) return RWS_ANA; break;
						case 2: if(bit_enable & FieldSet<DATA7, DATA6>::template GetMask<uint32_t>()) return RWS_ANA; break;
					}
				}
			}
			
			ReadWriteStatus rws = Super::Read(value, bit_enable);
			
			if(this->linflexd->UART_Mode())
			{
				if(this->linflexd->UART_RX_FIFO_Mode())
				{
					this->linflexd->RX_FIFO_Pop();
				}
			}
			
			return rws;
		}

		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = this->linflexd->UART_Mode() ? RWS_ANA : Super::Write(value, bit_enable);
			
			return rws;
		}
		
		using Super::operator =;
	};
	
	// Identifier Filter Enable Register (LINFlexD_IFER)
	struct LINFlexD_IFER : LINFlexD_Register<LINFlexD_IFER, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_IFER, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x40;
		
		struct FACT : Field<LINFlexD_IFER, FACT, (32 - NUM_FILTERS), 31> {}; // Filter active
		
		typedef FieldSet<FACT> ALL;
		
		LINFlexD_IFER(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_IFER(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_IFER"); this->SetDescription("Identifier Filter Enable Register");
			FACT::SetName("FACT"); FACT::SetDescription("Filter active");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = this->linflexd->InitMode() ? Super::Write(value, bit_enable)
			                                                 : Super::template WritePreserve<FACT>(value, bit_enable);
			
			return rws;
		}

		using Super::operator =;
	};
	
	// Identifier Filter Match Index (LINFlexD_IFMI)
	struct LINFlexD_IFMI : LINFlexD_Register<LINFlexD_IFMI, SW_R>
	{
		typedef LINFlexD_Register<LINFlexD_IFMI, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x44;
		
		struct IFMI : Field<LINFlexD_IFMI, IFMI, 27, 31, SW_R> {}; // Filter match index
		
		typedef FieldSet<IFMI> ALL;
		
		LINFlexD_IFMI(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_IFMI(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }

		void Init()
		{
			this->SetName("LINFlexD_IFMI"); this->SetDescription("Identifier Filter Match Index");
			IFMI::SetName("IFMI"); IFMI::SetDescription("Filter match index");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		using Super::operator =;
	};
	
	// Identifier Filter Mode Register (LINFlexD_IFMR)
	struct LINFlexD_IFMR : LINFlexD_Register<LINFlexD_IFMR, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_IFMR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x48;
		
		struct IFM : Field<LINFlexD_IFMR, IFM, 24, 31> {}; // Filter mode
		
		typedef FieldSet<IFM> ALL;
		
		LINFlexD_IFMR(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_IFMR(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }

		void Init()
		{
			this->SetName("LINFlexD_IFMR"); this->SetDescription("Identifier Filter Mode Register");
			IFM::SetName("IFM"); IFM::SetDescription("Filter mode");
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = this->linflexd->InitMode() ? Super::Write(value, bit_enable)
			                                                 : Super::template WritePreserve<IFM>(value, bit_enable);
			
			return rws;
		}

		void Reset()
		{
			this->Initialize(0x0);
		}

		using Super::operator =;
	};
	
	// Identifier Filter Control Register (LINFlexD_IFCRn)
	struct LINFlexD_IFCR : LINFlexD_Register<LINFlexD_IFCR, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_IFCR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x4c;
		
		struct DFL : Field<LINFlexD_IFCR, DFL, 16, 21> {}; // Data Field Length
		struct DIR : Field<LINFlexD_IFCR, DIR, 22    > {}; // Direction
		struct CCS : Field<LINFlexD_IFCR, CCS, 23    > {}; // Classic Checksum
		struct ID  : Field<LINFlexD_IFCR, ID , 26, 31> {}; // Identifier
		
		typedef FieldSet<DFL, DIR, CCS, ID> ALL;
		
		typedef FieldSet<DFL, DIR, CCS, ID> INIT_MODE_ONLY_WRITEABLE;
		
		LINFlexD_IFCR() : Super(0), reg_num(0) {}
		LINFlexD_IFCR(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd), reg_num(0) {}
		LINFlexD_IFCR(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, LINFlexD<CONFIG> *_linflexd)
		{
			this->linflexd = _linflexd;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "LINFlexD_IFCR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "Identifier Filter Control Register " << reg_num;
			this->SetDescription(description_sstr.str());
			
			DFL::SetName("DFL"); DFL::SetDescription("Data Field Length");
			DIR::SetName("DIR"); DIR::SetDescription("Direction");
			CCS::SetName("CCS"); CCS::SetDescription("Classic Checksum");
			ID ::SetName("ID "); ID ::SetDescription("Identifier");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = this->linflexd->InitMode() ? Super::Write(value, bit_enable)
			                                                 : Super::template WritePreserve<INIT_MODE_ONLY_WRITEABLE>(value, bit_enable);
			
			return rws;
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	// Global Control Register (LINFlexD_GCR)
	struct LINFlexD_GCR : LINFlexD_Register<LINFlexD_GCR, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_GCR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = (GENERIC_SLAVE == 0) ? 0x4c : 0x8c;
		
		struct TDFBM : Field<LINFlexD_GCR, TDFBM, 26>           {}; // Transmit data first bit MSB
		struct RDFBM : Field<LINFlexD_GCR, RDFBM, 27>           {}; // Received data first bit MSB
		struct TDLIS : Field<LINFlexD_GCR, TDLIS, 28>           {}; // Transmit data level inversion selection
		struct RDLIS : Field<LINFlexD_GCR, RDLIS, 29>           {}; // Received data level inversion selection
		struct STOP  : Field<LINFlexD_GCR, STOP , 30>           {}; // 1/2 stop bit configuration
		struct SR    : Field<LINFlexD_GCR, SR   , 31, 31, SW_W> {}; // Soft reset
	
		typedef FieldSet<TDFBM, RDFBM, TDLIS, RDLIS, STOP, SR> ALL;
		
		typedef FieldSet<TDFBM, RDFBM, TDLIS, RDLIS, STOP, SR> INIT_MODE_ONLY_WRITEABLE;
		
		LINFlexD_GCR(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_GCR(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_GCR"); this->SetDescription("Global Control Register");
			TDFBM::SetName("TDFBM"); TDFBM::SetDescription("Transmit data first bit MSB");
			RDFBM::SetName("RDFBM"); RDFBM::SetDescription("Received data first bit MSB");
			TDLIS::SetName("TDLIS"); TDLIS::SetDescription("Transmit data level inversion selection");
			RDLIS::SetName("RDLIS"); RDLIS::SetDescription("Received data level inversion selection");
			STOP ::SetName("STOP");  STOP ::SetDescription("1/2 stop bit configuration");
			SR   ::SetName("SR");    SR   ::SetDescription("Soft reset");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = this->linflexd->InitMode() ? Super::Write(value, bit_enable)
			                                                 : Super::template WritePreserve<INIT_MODE_ONLY_WRITEABLE>(value, bit_enable);
			
			if(this->template Get<SR>())
			{
				this->linflexd->SoftReset();
			}
			return rws;
		}

		using Super::operator =;
	};
	
	// UART Preset Timeout Register (LINFlexD_UARTPTO)
	struct LINFlexD_UARTPTO : LINFlexD_Register<LINFlexD_UARTPTO, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_UARTPTO, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = (GENERIC_SLAVE == 0) ? 0x50 : 0x90;
		
		struct PTO : Field<LINFlexD_UARTPTO, PTO, 20, 31> {}; // Preset Timeout
		
		typedef FieldSet<PTO> ALL;
		
		LINFlexD_UARTPTO(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_UARTPTO(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_UARTPTO"); this->SetDescription("UART Preset Timeout Register");
			PTO::SetName("PTO"); PTO::SetDescription("Preset Timeout");
		}
		
		void Reset()
		{
			this->template Set<PTO>(~uint32_t(0));
		}

		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			if((value == 0) & ((bit_enable & PTO::template GetMask<uint32_t>()) == PTO::template GetMask<uint32_t>())) // writing zero to PTO field ?
			{
				// zero is forbidden
				this->linflexd->Timeout(); // timeout occurs immediately
				return RWS_OK;
			}

			ReadWriteStatus rws = Super::Write(value, bit_enable);

			if(!IsReadWriteError(rws))
			{
				this->linflexd->linflexd_uartcto.Reset();
			}
			
			return rws;
		}

		using Super::operator =;
	};

	// UART Current Timeout Register (LINFlexD_UARTCTO)
	struct LINFlexD_UARTCTO : LINFlexD_Register<LINFlexD_UARTCTO, SW_R>
	{
		typedef LINFlexD_Register<LINFlexD_UARTCTO, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = (GENERIC_SLAVE == 0) ? 0x54 : 0x94;
		
		struct CTO : Field<LINFlexD_UARTCTO, CTO, 20, 31, SW_R> {}; // Current Timeout
		
		typedef FieldSet<CTO> ALL;
		
		LINFlexD_UARTCTO(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_UARTCTO(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }
		
		void Init()
		{
			this->SetName("LINFlexD_UARTCTO"); this->SetDescription("UART Current Timeout Register");
			CTO::SetName("CTO"); CTO::SetDescription("Current Timeout");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		void SoftReset()
		{
			//  All fields are reset
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	// DMA Tx Enable Register (LINFlexD_DMATXE)
	struct LINFlexD_DMATXE : LINFlexD_Register<LINFlexD_DMATXE, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_DMATXE, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = (GENERIC_SLAVE == 0) ? 0x58 : 0x98;
		
		struct DTE : Field<LINFlexD_DMATXE, DTE, (32 - NUM_DMA_TX_CHANNELS), 31> {}; // DMA Tx channel Y enable

		typedef FieldSet<DTE> ALL;
		
		LINFlexD_DMATXE(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_DMATXE(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }

		void Init()
		{
			this->SetName("LINFlexD_DMATXE"); this->SetDescription("DMA Tx Enable Register");
			DTE::SetName("DTE"); DTE::SetDescription("DMA Tx channel Y enable");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
			this->linflexd->UpdateDMA_TX();
		}

		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(value, bit_enable);

			if(!IsReadWriteError(rws))
			{
				this->linflexd->UpdateDMA_TX();
			}
			
			return rws;
		}
		
		using Super::operator =;
	};
	
	// DMA Rx Enable Register (LINFlexD_DMARXE)
	struct LINFlexD_DMARXE : LINFlexD_Register<LINFlexD_DMARXE, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_DMARXE, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = (GENERIC_SLAVE == 0) ? 0x5c : 0x9c;
		
		struct DRE : Field<LINFlexD_DMARXE, DRE, (32 - NUM_DMA_RX_CHANNELS), 31> {}; // DMA Rx channel Y enable

		typedef FieldSet<DRE> ALL;
		
		LINFlexD_DMARXE(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_DMARXE(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }

		void Init()
		{
			this->SetName("LINFlexD_DMARXE"); this->SetDescription("DMA Rx Enable Register");
			DRE::SetName("DRE"); DRE::SetDescription("DMA Rx channel Y enable");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
			this->linflexd->UpdateDMA_TX();
		}

		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(value, bit_enable);

			if(!IsReadWriteError(rws))
			{
				this->linflexd->UpdateDMA_RX();
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// PSI5-S Tx Delay register (LINFlexD_PTD)
	struct LINFlexD_PTD : LINFlexD_Register<LINFlexD_PTD, SW_RW>
	{
		typedef LINFlexD_Register<LINFlexD_PTD, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xa0;
		
		struct IFD    : Field<LINFlexD_PTD, IFD, 27, 30, GENERIC_PSI5 ? SW_RW : SW_R_HW_RO> {}; // Interframe Delay
		struct EN     : Field<LINFlexD_PTD, EN , 31, 31, GENERIC_PSI5 ? SW_RW : SW_R_HW_RO> {}; // Enable
		
		typedef FieldSet<IFD, EN> ALL;
		
		LINFlexD_PTD(LINFlexD<CONFIG> *_linflexd) : Super(_linflexd) { Init(); }
		LINFlexD_PTD(LINFlexD<CONFIG> *_linflexd, uint32_t value) : Super(_linflexd, value) { Init(); }

		void Init()
		{
			this->SetName("LINFlexD_PTD"); this->SetDescription("PSI5-S Tx Delay register");
			IFD::SetName("IFD"); IFD::SetDescription("Interframe Delay");
			EN ::SetName("EN");  EN ::SetDescription("Enable");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		using Super::operator =;
	};
	
	unisim::kernel::tlm2::ClockPropertiesProxy m_clk_prop_proxy; // proxy to get clock properties from master clock port
	unisim::kernel::tlm2::ClockPropertiesProxy lin_clk_prop_proxy; // proxy to get clock properties from LIN clock port
	
	// LINFlexD registers
	LINFlexD_LINCR1                                                                                  linflexd_lincr1;   // LINFlexD_LINCR1
	LINFlexD_LINIER                                                                                  linflexd_linier;   // LINFlexD_LINIER
	LINFlexD_LINSR                                                                                   linflexd_linsr;    // LINFlexD_LINSR
	LINFlexD_LINESR                                                                                  linflexd_linesr;   // LINFlexD_LINESR
	LINFlexD_UARTCR                                                                                  linflexd_uartcr;   // LINFlexD_UARTCR
	LINFlexD_UARTSR                                                                                  linflexd_uartsr;   // LINFlexD_UARTSR
	LINFlexD_LINTCSR                                                                                 linflexd_lintcsr;  // LINFlexD_LINTCSR
	LINFlexD_LINOCR                                                                                  linflexd_linocr;   // LINFlexD_LINOCR
	LINFlexD_LINTOCR                                                                                 linflexd_lintocr;  // LINFlexD_LINTOCR
	LINFlexD_LINFBRR                                                                                 linflexd_linfbrr;  // LINFlexD_LINFBRR
	LINFlexD_LINIBRR                                                                                 linflexd_linibrr;  // LINFlexD_LINIBRR
	LINFlexD_LINCFR                                                                                  linflexd_lincfr;   // LINFlexD_LINCFR
	LINFlexD_LINCR2                                                                                  linflexd_lincr2;   // LINFlexD_LINCR2
	LINFlexD_BIDR                                                                                    linflexd_bidr;     // LINFlexD_BIDR
	LINFlexD_BDRL                                                                                    linflexd_bdrl;     // LINFlexD_BDRL
	LINFlexD_BDRM                                                                                    linflexd_bdrm;     // LINFlexD_BDRM
	LINFlexD_IFER                                                                                    linflexd_ifer;     // LINFlexD_IFER
	LINFlexD_IFMI                                                                                    linflexd_ifmi;     // LINFlexD_IFMI
	LINFlexD_IFMR                                                                                    linflexd_ifmr;     // LINFlexD_IFMR
	AddressableRegisterFile<LINFlexD_IFCR, NUM_IDENTIFIERS ? NUM_IDENTIFIERS : 1, LINFlexD<CONFIG> > linflexd_ifcr;     // LINFlexD_IFCRn
	LINFlexD_GCR                                                                                     linflexd_gcr;      // LINFlexD_GCR
	LINFlexD_UARTPTO                                                                                 linflexd_uartpto;  // LINFlexD_UARTPTO
	LINFlexD_UARTCTO                                                                                 linflexd_uartcto;  // LINFlexD_UARTCTO
	LINFlexD_DMATXE                                                                                  linflexd_dmatxe;   // LINFlexD_DMATXE
	LINFlexD_DMARXE                                                                                  linflexd_dmarxe;   // LINFlexD_DMARXE
	LINFlexD_PTD                                                                                     linflexd_ptd;      // LINFlexD_PTD
	
	unisim::kernel::tlm2::tlm_input_bitstream rx_input; // Rx timed input bit stream
	bool rx_prev_input_status;                    // Previous status of Rx pin
	bool rx_input_status;                         // Current status of Rx pin
	unsigned int rx_dominant_bits_cnt;            // number of consecutive dominant bits (i.e zero bits) received
	unsigned int rx_fifo_cnt;                     // Rx FIFO counter
	uint8_t rx_parity_error_reg;                  // 4-bit parity error register
	sc_core::sc_event gen_int_rx_event;           // INT_RX event
	sc_core::sc_event gen_int_tx_event;           // INT_TX event
	sc_core::sc_event gen_int_err_event;          // INT_ERR event
	sc_core::sc_event *gen_dma_rx_event[NUM_DMA_RX_CHANNELS]; // DMA_RX event
	sc_core::sc_event *gen_dma_tx_event[NUM_DMA_TX_CHANNELS]; // DMA_TX event
	sc_core::sc_event tx_event;                   // Tx event
	bool pending_tx_request;                      // whether there's a pending transmission request
	bool lins_int_rx_mask;                        // whether to mask INT_RX due to LINS
	sc_core::sc_time last_run_time;               // last time timeout counter was run
	sc_core::sc_time rx_time;                     // Rx time offset relative to current simulation time
	bool data_reception_in_progress;
	bool data_transmission_in_progress;
	
	bool dma_rx[NUM_DMA_RX_CHANNELS];
	bool dma_tx[NUM_DMA_TX_CHANNELS];
	
	// LINFlexD registers address map
	RegisterAddressMap<sc_dt::uint64> reg_addr_map;
	
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	unisim::kernel::tlm2::Schedule<Event> schedule;         // Payload (processor requests over AHB interface) schedule
	
	unisim::util::endian::endian_type endian;
	unisim::kernel::variable::Parameter<unisim::util::endian::endian_type> param_endian;
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	double baud_tolerance;
	unisim::kernel::variable::Parameter<double> param_baud_tolerance;
	
	sc_core::sc_time master_clock_period;                 // Master clock period
	sc_core::sc_time master_clock_start_time;             // Master clock start time
	bool master_clock_posedge_first;                      // Master clock posedge first ?
	double master_clock_duty_cycle;                       // Master clock duty cycle

	sc_core::sc_time lin_clock_period;                    // LIN clock period
	sc_core::sc_time lin_clock_start_time;                // LIN clock start time
	bool lin_clock_posedge_first;                         // LIN clock posedge first ?
	double lin_clock_duty_cycle;                          // LIN clock duty cycle
	
	sc_core::sc_time baud_period_lower_bound;
	sc_core::sc_time baud_period;
	sc_core::sc_time baud_period_upper_bound;
	
	void EnableLINS_INT_TX();
	void DisableLINS_INT_TX();
	bool InitMode() const;
	bool SleepMode() const;
	bool NormalMode() const;
	bool UART_Mode() const;
	bool LINTimeoutCounterMode() const;
	bool OutputCompareTimeoutCounterMode() const;
	bool HardwareChecksumCalculation() const;
	bool SoftwareChecksumCalculation() const;
	bool LIN_MasterMode() const;
	bool LIN_SlaveMode() const;
	bool UART_RX_BufferMode() const;
	bool UART_RX_FIFO_Mode() const;
	bool UART_TX_BufferMode() const;
	bool UART_TX_FIFO_Mode() const;
	unsigned int UARTWordLength() const;
	bool LINS_INT_RX() const;
	bool LIN_ClockEnabled() const;

	void Reset();
	void SoftReset();
	void ResetTimeout();
	LIN_State GetState();
	void SetState(LIN_State lin_state);
	void UpdateState();
	
	void UpdateINT_RX();
	void UpdateINT_TX();
	void UpdateINT_ERR();
	void UpdateDMA_RX(unsigned int dma_rx_num, const sc_core::sc_time& delay = sc_core::SC_ZERO_TIME);
	void UpdateDMA_TX(unsigned int dma_tx_num, const sc_core::sc_time& delay = sc_core::SC_ZERO_TIME);
	void UpdateDMA_RX(const sc_core::sc_time& delay = sc_core::SC_ZERO_TIME);
	void UpdateDMA_TX(const sc_core::sc_time& delay = sc_core::SC_ZERO_TIME);
	
	void RequestDMA_RX();
	void RequestDMA_TX();
	
	void Timeout();
	void IncrementTimeoutCounter(sc_dt::uint64 delta);
	sc_dt::int64 TicksToNextTimeoutCounterRun();
	sc_core::sc_time TimeToNextTimeoutCounterRun();
	void RunTimeoutCounterToTime(const sc_core::sc_time& time_stamp);
	void ScheduleTimeoutCounterRun();

	void ProcessEvent(Event *event);
	void ProcessEvents();
	void Process();
	void UpdateMasterClock();
	void UpdateLINClock();
	void MasterClockPropertiesChangedProcess();
	void RESET_B_Process();
	
	void INT_RX_Process();
	void INT_TX_Process();
	void INT_ERR_Process();
	void DMA_RX_Process(unsigned int dma_rx_num);
	void DMA_TX_Process(unsigned int dma_tx_num);
	void DMA_ACK_RX_Process(unsigned int dma_rx_num);
	void DMA_ACK_TX_Process(unsigned int dma_rx_num);
	void RX_FIFO_Pop();
	void IncrementRxTime();
	bool RX_InputStatus();
	bool RX_FallingEdge();
	void RX_Process();
	void TX_FIFO_Push();
	void Transmit();
	void TX_Process();
};

} // end of namespace linflexd
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_LINFLEXD_LINFLEXD_HH__
