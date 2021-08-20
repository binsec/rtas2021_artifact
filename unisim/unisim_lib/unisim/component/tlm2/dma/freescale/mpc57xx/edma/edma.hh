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

#ifndef __UNISIM_COMPONENT_TLM2_DMA_FREESCALE_MPC57XX_EDMA_EDMA_HH__
#define __UNISIM_COMPONENT_TLM2_DMA_FREESCALE_MPC57XX_EDMA_EDMA_HH__

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
namespace dma {
namespace freescale {
namespace mpc57xx {
namespace edma {

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
	static const unsigned int NUM_DMA_CHANNELS = 16;
	static const unsigned int BUSWIDTH = 32;
};
#endif

template <unsigned long long N>
struct Log2
{
    static const unsigned int value = Log2<N / 2>::value + 1;
};

template <>
struct Log2<1>
{
    static const unsigned int value = 0;
};

template <unsigned long long N>
struct CeilLog2
{
    static const unsigned int value = (N > (1 << Log2<N>::value)) ? Log2<N>::value + 1 : Log2<N>::value;
};

enum EDGE
{
	NEG = 0,
	POS = 1
};

template <typename CONFIG>
class EDMA
	: public sc_core::sc_module
	, public tlm::tlm_fw_transport_if<>
	, public tlm::tlm_bw_transport_if<>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Registers>
{
public:
	static const unsigned int TLM2_IP_VERSION_MAJOR  = 1;
	static const unsigned int TLM2_IP_VERSION_MINOR  = 1;
	static const unsigned int TLM2_IP_VERSION_PATCH  = 0;
	static const unsigned int NUM_DMA_CHANNELS       = CONFIG::NUM_DMA_CHANNELS;
	static const unsigned int BUSWIDTH               = CONFIG::BUSWIDTH;
	static const unsigned int NUM_GROUPS             = 4;
	static const unsigned int NUM_CHANNELS_PER_GROUP = 16;
	static const bool threaded_model                 = false;
	
	typedef tlm::tlm_target_socket<BUSWIDTH> peripheral_slave_if_type;
	typedef tlm::tlm_initiator_socket<BUSWIDTH> master_if_type;

	peripheral_slave_if_type  peripheral_slave_if;              // peripheral slave interface
	master_if_type            master_if;                        // master interface
	sc_core::sc_in<bool>      m_clk;                            // Clock port (peripheral slave interface)
	sc_core::sc_in<bool>      dma_clk;                          // DMA Clock port (master interface)
	sc_core::sc_in<bool>      reset_b;                          // reset
	sc_core::sc_in<bool>     *dma_channel[NUM_DMA_CHANNELS];    // DMA channels (peripheral requests)
	sc_core::sc_out<bool>    *dma_channel_ack[NUM_DMA_CHANNELS];// DMA channel acknowledgements (toward peripheral)
	sc_core::sc_out<bool>    *irq[NUM_DMA_CHANNELS];            // Interrupt requests
	sc_core::sc_out<bool>    *err_irq[NUM_DMA_CHANNELS];        // Error Interrupt requests
	
	// services
	unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;

	EDMA(const sc_core::sc_module_name& name, unisim::kernel::Object *parent);
	virtual ~EDMA();
	
	// Forward (peripheral slave interface)
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	
	// Backward (master interface)
	virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	//////////////// unisim::service::interface::Registers ////////////////////
	
	virtual unisim::service::interfaces::Register *GetRegister(const char *name);
	virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);
private:
	virtual void end_of_elaboration();

	unisim::kernel::logger::Logger logger;
	
	typedef unsigned int MasterID;
	
	enum PrivilegeAccessLevel
	{
		PAL_USER_PROTECTION_LEVEL       = 0,
		PAL_PRIVILEGED_PROTECTION_LEVEL = 1
	};
	
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

	// Common EDMA register representation
	template <typename REGISTER, unsigned int _SIZE, Access _ACCESS>
	struct EDMA_Register : AddressableRegister<REGISTER, _SIZE, _ACCESS, tlm_trans_attr>
	{
		typedef AddressableRegister<REGISTER, _SIZE, _ACCESS, tlm_trans_attr> Super;
		
		EDMA_Register(EDMA<CONFIG> *_edma) : Super(), edma(_edma) {}
		EDMA_Register(EDMA<CONFIG> *_edma, uint32_t value) : Super(value), edma(_edma) {}

		inline bool IsVerboseRead() const ALWAYS_INLINE { return edma->verbose; }
		inline bool IsVerboseWrite() const ALWAYS_INLINE { return edma->verbose; }
		inline std::ostream& GetInfoStream() ALWAYS_INLINE { return edma->logger.DebugInfoStream(); }

		using Super::operator =;
		
	protected:
		EDMA<CONFIG> *edma;
	};
	
	// Control Register (eDMA_CR)
	struct EDMA_CR : EDMA_Register<EDMA_CR, 32, SW_RW>
	{
		typedef EDMA_Register<EDMA_CR, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;
		
		struct CX      : Field<EDMA_CR, CX     , 14    > {}; // Cancel transfer
		struct ECX     : Field<EDMA_CR, ECX    , 15    > {}; // Error cancel transfer
		struct GRP3PRI : Field<EDMA_CR, GRP3PRI, 16, 17> {}; // Channel group 3 priority
		struct GRP2PRI : Field<EDMA_CR, GRP2PRI, 18, 19> {}; // Channel group 2 priority
		struct GRP1PRI : Field<EDMA_CR, GRP1PRI, 20, 21> {}; // Channel group 1 priority
		struct GRP0PRI : Field<EDMA_CR, GRP0PRI, 22, 23> {}; // Channel group 0 priority
		struct GRPPRI  : Field<EDMA_CR, GRPPRI , 16, 23> {};
		struct EMLM    : Field<EDMA_CR, EMLM   , 24    > {}; // Enable minor loop mapping
		struct CLM     : Field<EDMA_CR, CLM    , 25    > {}; // Continuous Link mode
		struct HALT    : Field<EDMA_CR, HALT   , 26    > {}; // Halt DMA operations
		struct HOE     : Field<EDMA_CR, HOE    , 27    > {}; // Halt on error
		struct ERGA    : Field<EDMA_CR, ERGA   , 28    > {}; // Enable round-robin group arbitration
		struct ERCA    : Field<EDMA_CR, ERCA   , 29    > {}; // Enable round-robin channel arbitration
		struct EDBG    : Field<EDMA_CR, EDBG   , 30    > {}; // Enable debug
		
		typedef FieldSet<CX, ECX, GRP3PRI, GRP2PRI, GRP1PRI, GRP0PRI, EMLM, CLM, HALT, HOE, ERGA, ERCA, EDBG> ALL;
		
		EDMA_CR(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_CR(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value) { Init(); }
		
		void Init()
		{
			this->SetName("eDMA_CR"); this->SetDescription("Control Register");
			
			CX     ::SetName("CX");      CX     ::SetDescription("Cancel transfer");
			ECX    ::SetName("ECX");     ECX    ::SetDescription("Error cancel transfer");
			GRP3PRI::SetName("GRP3PRI"); GRP3PRI::SetDescription("Channel group 3 priority");
			GRP2PRI::SetName("GRP2PRI"); GRP2PRI::SetDescription("Channel group 2 priority");
			GRP1PRI::SetName("GRP1PRI"); GRP1PRI::SetDescription("Channel group 1 priority");
			GRP0PRI::SetName("GRP0PRI"); GRP0PRI::SetDescription("Channel group 0 priority");
			EMLM   ::SetName("EMLM");    EMLM   ::SetDescription("Enable minor loop mapping");
			CLM    ::SetName("CLM");     CLM    ::SetDescription("Continuous Link mode");
			HALT   ::SetName("HALT");    HALT   ::SetDescription("Halt DMA operations");
			HOE    ::SetName("HOE");     HOE    ::SetDescription("Halt on error");
			ERGA   ::SetName("ERGA");    ERGA   ::SetDescription("Enable round-robin group arbitration");
			ERCA   ::SetName("ERCA");    ERCA   ::SetDescription("Enable round-robin channel arbitration");
			EDBG   ::SetName("EDBG");    EDBG   ::SetDescription("Enable debug");
		}
		
		void Reset()
		{
			this->Initialize(0x0000e400UL);
			this->edma->UpdateGroupPriority();
		}
		
		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint32_t& value, const uint32_t& bit_enable)
		{
			bool old_halt = this->template Get<HALT>();
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			bool new_halt = this->template Get<HALT>();
			
			if(!IsReadWriteError(rws))
			{
				if(old_halt && !new_halt)
				{
					this->edma->NotifyEngine();
				}
				
				this->edma->UpdateGroupPriority();
			}
			
			return rws;
		}
		
		using Super::operator =;
	};
	
	// Error Status (eDMA_ES)
	struct EDMA_ES : EDMA_Register<EDMA_ES, 32, SW_RW>
	{
		typedef EDMA_Register<EDMA_ES, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x4;

		struct VLD    : Field<EDMA_ES, VLD   ,  0,  0, SW_R > {}; // ERRH and ERRL status bits
		struct UCE    : Field<EDMA_ES, UCE   , 14, 14, SW_R > {}; // Uncorrectable ECC error
		struct ECX    : Field<EDMA_ES, ECX   , 15, 15, SW_R > {}; // Transfer cancelled
		struct GPE    : Field<EDMA_ES, GPE   , 16, 16, SW_RW> {}; // Group priority error
		struct CPE    : Field<EDMA_ES, CPE   , 17, 17, SW_R > {}; // Channel priority error
		struct ERRCHN : Field<EDMA_ES, ERRCHN, 18, 23, SW_R > {}; // Error channel number or cancelled channel number GPE and/or last recorded error cancelled transfer
		struct SAE    : Field<EDMA_ES, SAE   , 24, 24, SW_R > {}; // Source address error
		struct SOE    : Field<EDMA_ES, SOE   , 25, 25, SW_R > {}; // Source offset error
		struct DAE    : Field<EDMA_ES, DAE   , 26, 26, SW_R > {}; // Destination address error
		struct DOE    : Field<EDMA_ES, DOE   , 27, 27, SW_R > {}; // Destination offset error
		struct NCE    : Field<EDMA_ES, NCE   , 28, 28, SW_R > {}; // NBYTES/CITER configuration error
		struct SGE    : Field<EDMA_ES, SGE   , 29, 29, SW_R > {}; // Scatter/gather configuration error
		struct SBE    : Field<EDMA_ES, SBE   , 30, 30, SW_R > {}; // Source bus error
		struct DBE    : Field<EDMA_ES, DBE   , 31, 31, SW_R > {}; // Destination bus error

		typedef FieldSet<VLD, UCE, ECX, GPE, CPE, ERRCHN, SAE, SOE, DAE, DOE, NCE, SGE, SBE, DBE> ALL;
		
		EDMA_ES(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_ES(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value) { Init(); }

		void Init()
		{
			this->SetName("eDMA_ES"); this->SetDescription("Error Status");
			
			VLD   ::SetName("VLD");    VLD   ::SetDescription("ERRH and ERRL status bits");
			UCE   ::SetName("UCE");    UCE   ::SetDescription("Uncorrectable ECC error");
			ECX   ::SetName("ECX");    ECX   ::SetDescription("Transfer cancelled");
			GPE   ::SetName("GPE");    GPE   ::SetDescription("Group priority error");
			CPE   ::SetName("CPE");    CPE   ::SetDescription("Channel priority error");
			ERRCHN::SetName("ERRCHN"); ERRCHN::SetDescription("Error channel number or cancelled channel number GPE and/or last recorded error cancelled transfer");
			SAE   ::SetName("SAE");    SAE   ::SetDescription("Source address error");
			SOE   ::SetName("SOE");    SOE   ::SetDescription("Source offset error");
			DAE   ::SetName("DAE");    DAE   ::SetDescription("Destination address error");
			DOE   ::SetName("DOE");    DOE   ::SetDescription("Destination offset error");
			NCE   ::SetName("NCE");    NCE   ::SetDescription("NBYTES/CITER configuration error");
			SGE   ::SetName("SGE");    SGE   ::SetDescription("Scatter/gather configuration error");
			SBE   ::SetName("SBE");    SBE   ::SetDescription("Source bus error");
			DBE   ::SetName("DBE");    DBE   ::SetDescription("Destination bus error");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		using Super::operator =;
	};

	// Enable Request Register High (eDMA_ERQH)
	struct EDMA_ERQH : EDMA_Register<EDMA_ERQH, 32, SW_RW>
	{
		typedef EDMA_Register<EDMA_ERQH, 32, SW_RW> Super;

		static const sc_dt::uint64 ADDRESS_OFFSET = 0x8;

		struct ERQ63 : Field<EDMA_ERQH, ERQ63,  0> {}; // Enable DMA Request 63                                                                                       
		struct ERQ62 : Field<EDMA_ERQH, ERQ62,  1> {}; // Enable DMA Request 62                                                                                       
		struct ERQ61 : Field<EDMA_ERQH, ERQ61,  2> {}; // Enable DMA Request 61                                                                                       
		struct ERQ60 : Field<EDMA_ERQH, ERQ60,  3> {}; // Enable DMA Request 60                                                                                       
		struct ERQ59 : Field<EDMA_ERQH, ERQ59,  4> {}; // Enable DMA Request 59                                                                                       
		struct ERQ58 : Field<EDMA_ERQH, ERQ58,  5> {}; // Enable DMA Request 58                                                                                       
		struct ERQ57 : Field<EDMA_ERQH, ERQ57,  6> {}; // Enable DMA Request 57                                                                                       
		struct ERQ56 : Field<EDMA_ERQH, ERQ56,  7> {}; // Enable DMA Request 56                                                                                       
		struct ERQ55 : Field<EDMA_ERQH, ERQ55,  8> {}; // Enable DMA Request 55                                                                                       
		struct ERQ54 : Field<EDMA_ERQH, ERQ54,  9> {}; // Enable DMA Request 54                                                                                       
		struct ERQ53 : Field<EDMA_ERQH, ERQ53, 10> {}; // Enable DMA Request 53                                                                                      
		struct ERQ52 : Field<EDMA_ERQH, ERQ52, 11> {}; // Enable DMA Request 52                                                                                      
		struct ERQ51 : Field<EDMA_ERQH, ERQ51, 12> {}; // Enable DMA Request 51                                                                                      
		struct ERQ50 : Field<EDMA_ERQH, ERQ50, 13> {}; // Enable DMA Request 50                                                                                      
		struct ERQ49 : Field<EDMA_ERQH, ERQ49, 14> {}; // Enable DMA Request 49                                                                                      
		struct ERQ48 : Field<EDMA_ERQH, ERQ48, 15> {}; // Enable DMA Request 48                                                                                      
		struct ERQ47 : Field<EDMA_ERQH, ERQ47, 16> {}; // Enable DMA Request 47
		struct ERQ46 : Field<EDMA_ERQH, ERQ46, 17> {}; // Enable DMA Request 46
		struct ERQ45 : Field<EDMA_ERQH, ERQ45, 18> {}; // Enable DMA Request 45
		struct ERQ44 : Field<EDMA_ERQH, ERQ44, 19> {}; // Enable DMA Request 44
		struct ERQ43 : Field<EDMA_ERQH, ERQ43, 20> {}; // Enable DMA Request 43
		struct ERQ42 : Field<EDMA_ERQH, ERQ42, 21> {}; // Enable DMA Request 42
		struct ERQ41 : Field<EDMA_ERQH, ERQ41, 22> {}; // Enable DMA Request 41
		struct ERQ40 : Field<EDMA_ERQH, ERQ40, 23> {}; // Enable DMA Request 40
		struct ERQ39 : Field<EDMA_ERQH, ERQ39, 24> {}; // Enable DMA Request 39
		struct ERQ38 : Field<EDMA_ERQH, ERQ38, 25> {}; // Enable DMA Request 38
		struct ERQ37 : Field<EDMA_ERQH, ERQ37, 26> {}; // Enable DMA Request 37
		struct ERQ36 : Field<EDMA_ERQH, ERQ36, 27> {}; // Enable DMA Request 36
		struct ERQ35 : Field<EDMA_ERQH, ERQ35, 28> {}; // Enable DMA Request 35
		struct ERQ34 : Field<EDMA_ERQH, ERQ34, 29> {}; // Enable DMA Request 34
		struct ERQ33 : Field<EDMA_ERQH, ERQ33, 30> {}; // Enable DMA Request 33
		struct ERQ32 : Field<EDMA_ERQH, ERQ32, 31> {}; // Enable DMA Request 32

		typedef FieldSet< ERQ63, ERQ62, ERQ61, ERQ60, ERQ59, ERQ58, ERQ57, ERQ56
		                , ERQ55, ERQ54, ERQ53, ERQ52, ERQ51, ERQ50, ERQ49, ERQ48
		                , ERQ47, ERQ46, ERQ45, ERQ44, ERQ43, ERQ42, ERQ41, ERQ40
		                , ERQ39, ERQ38, ERQ37, ERQ36, ERQ35, ERQ34, ERQ33, ERQ32> ALL;

		EDMA_ERQH(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_ERQH(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value) { Init(); }

		void Init()
		{
			this->SetName("eDMA_ERQH"); this->SetDescription("Enable Request Register High");
			
			ERQ63::SetName("ERQ63"); ERQ63::SetDescription("Enable DMA Request 63");
			ERQ62::SetName("ERQ62"); ERQ62::SetDescription("Enable DMA Request 62");
			ERQ61::SetName("ERQ61"); ERQ61::SetDescription("Enable DMA Request 61");
			ERQ60::SetName("ERQ60"); ERQ60::SetDescription("Enable DMA Request 60");
			ERQ59::SetName("ERQ59"); ERQ59::SetDescription("Enable DMA Request 59");
			ERQ58::SetName("ERQ58"); ERQ58::SetDescription("Enable DMA Request 58");
			ERQ57::SetName("ERQ57"); ERQ57::SetDescription("Enable DMA Request 57");
			ERQ56::SetName("ERQ56"); ERQ56::SetDescription("Enable DMA Request 56");
			ERQ55::SetName("ERQ55"); ERQ55::SetDescription("Enable DMA Request 55");
			ERQ54::SetName("ERQ54"); ERQ54::SetDescription("Enable DMA Request 54");
			ERQ53::SetName("ERQ53"); ERQ53::SetDescription("Enable DMA Request 53");
			ERQ52::SetName("ERQ52"); ERQ52::SetDescription("Enable DMA Request 52");
			ERQ51::SetName("ERQ51"); ERQ51::SetDescription("Enable DMA Request 51");
			ERQ50::SetName("ERQ50"); ERQ50::SetDescription("Enable DMA Request 50");
			ERQ49::SetName("ERQ49"); ERQ49::SetDescription("Enable DMA Request 49");
			ERQ48::SetName("ERQ48"); ERQ48::SetDescription("Enable DMA Request 48");
			ERQ47::SetName("ERQ47"); ERQ47::SetDescription("Enable DMA Request 47");
			ERQ46::SetName("ERQ46"); ERQ46::SetDescription("Enable DMA Request 46");
			ERQ45::SetName("ERQ45"); ERQ45::SetDescription("Enable DMA Request 45");
			ERQ44::SetName("ERQ44"); ERQ44::SetDescription("Enable DMA Request 44");
			ERQ43::SetName("ERQ43"); ERQ43::SetDescription("Enable DMA Request 43");
			ERQ42::SetName("ERQ42"); ERQ42::SetDescription("Enable DMA Request 42");
			ERQ41::SetName("ERQ41"); ERQ41::SetDescription("Enable DMA Request 41");
			ERQ40::SetName("ERQ40"); ERQ40::SetDescription("Enable DMA Request 40");
			ERQ39::SetName("ERQ39"); ERQ39::SetDescription("Enable DMA Request 39");
			ERQ38::SetName("ERQ38"); ERQ38::SetDescription("Enable DMA Request 38");
			ERQ37::SetName("ERQ37"); ERQ37::SetDescription("Enable DMA Request 37");
			ERQ36::SetName("ERQ36"); ERQ36::SetDescription("Enable DMA Request 36");
			ERQ35::SetName("ERQ35"); ERQ35::SetDescription("Enable DMA Request 35");
			ERQ34::SetName("ERQ34"); ERQ34::SetDescription("Enable DMA Request 34");
			ERQ33::SetName("ERQ33"); ERQ33::SetDescription("Enable DMA Request 33");
			ERQ32::SetName("ERQ32"); ERQ32::SetDescription("Enable DMA Request 32");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
			this->edma->UpdateAllHardwareRequestStatus();
		}

		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			
			this->edma->UpdateAllHardwareRequestStatus();
			
			return rws;
		}
		
		using Super::operator =;
	};

	// Enable Request Register Low (eDMA_ERQL)
	struct EDMA_ERQL : EDMA_Register<EDMA_ERQL, 32, SW_RW>
	{
		typedef EDMA_Register<EDMA_ERQL, 32, SW_RW> Super;

		static const sc_dt::uint64 ADDRESS_OFFSET = 0xc;
	
		struct ERQ31 : Field<EDMA_ERQL, ERQ31,  0> {}; // Enable DMA Request 31
		struct ERQ30 : Field<EDMA_ERQL, ERQ30,  1> {}; // Enable DMA Request 30
		struct ERQ29 : Field<EDMA_ERQL, ERQ29,  2> {}; // Enable DMA Request 29
		struct ERQ28 : Field<EDMA_ERQL, ERQ28,  3> {}; // Enable DMA Request 28
		struct ERQ27 : Field<EDMA_ERQL, ERQ27,  4> {}; // Enable DMA Request 27
		struct ERQ26 : Field<EDMA_ERQL, ERQ26,  5> {}; // Enable DMA Request 26
		struct ERQ25 : Field<EDMA_ERQL, ERQ25,  6> {}; // Enable DMA Request 25
		struct ERQ24 : Field<EDMA_ERQL, ERQ24,  7> {}; // Enable DMA Request 24
		struct ERQ23 : Field<EDMA_ERQL, ERQ23,  8> {}; // Enable DMA Request 23
		struct ERQ22 : Field<EDMA_ERQL, ERQ22,  9> {}; // Enable DMA Request 22
		struct ERQ21 : Field<EDMA_ERQL, ERQ21, 10> {}; // Enable DMA Request 21
		struct ERQ20 : Field<EDMA_ERQL, ERQ20, 11> {}; // Enable DMA Request 20
		struct ERQ19 : Field<EDMA_ERQL, ERQ19, 12> {}; // Enable DMA Request 19
		struct ERQ18 : Field<EDMA_ERQL, ERQ18, 13> {}; // Enable DMA Request 18
		struct ERQ17 : Field<EDMA_ERQL, ERQ17, 14> {}; // Enable DMA Request 17
		struct ERQ16 : Field<EDMA_ERQL, ERQ16, 15> {}; // Enable DMA Request 16
		struct ERQ15 : Field<EDMA_ERQL, ERQ15, 16> {}; // Enable DMA Request 15
		struct ERQ14 : Field<EDMA_ERQL, ERQ14, 17> {}; // Enable DMA Request 14
		struct ERQ13 : Field<EDMA_ERQL, ERQ13, 18> {}; // Enable DMA Request 13
		struct ERQ12 : Field<EDMA_ERQL, ERQ12, 19> {}; // Enable DMA Request 12
		struct ERQ11 : Field<EDMA_ERQL, ERQ11, 20> {}; // Enable DMA Request 11
		struct ERQ10 : Field<EDMA_ERQL, ERQ10, 21> {}; // Enable DMA Request 10
		struct  ERQ9 : Field<EDMA_ERQL,  ERQ9, 22> {}; // Enable DMA Request 9
		struct  ERQ8 : Field<EDMA_ERQL,  ERQ8, 23> {}; // Enable DMA Request 8
		struct  ERQ7 : Field<EDMA_ERQL,  ERQ7, 24> {}; // Enable DMA Request 7
		struct  ERQ6 : Field<EDMA_ERQL,  ERQ6, 25> {}; // Enable DMA Request 6
		struct  ERQ5 : Field<EDMA_ERQL,  ERQ5, 26> {}; // Enable DMA Request 5
		struct  ERQ4 : Field<EDMA_ERQL,  ERQ4, 27> {}; // Enable DMA Request 4
		struct  ERQ3 : Field<EDMA_ERQL,  ERQ3, 28> {}; // Enable DMA Request 3
		struct  ERQ2 : Field<EDMA_ERQL,  ERQ2, 29> {}; // Enable DMA Request 2
		struct  ERQ1 : Field<EDMA_ERQL,  ERQ1, 30> {}; // Enable DMA Request 1
		struct  ERQ0 : Field<EDMA_ERQL,  ERQ0, 31> {}; // Enable DMA Request 0

		typedef FieldSet< ERQ31, ERQ30, ERQ29, ERQ28, ERQ27, ERQ26, ERQ25, ERQ24
		                , ERQ23, ERQ22, ERQ21, ERQ20, ERQ19, ERQ18, ERQ17, ERQ16
		                , ERQ15, ERQ14, ERQ13, ERQ12, ERQ11, ERQ10,  ERQ9,  ERQ8
		                ,  ERQ7,  ERQ6,  ERQ5,  ERQ4,  ERQ3,  ERQ2,  ERQ1,  ERQ0> ALL;

		EDMA_ERQL(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_ERQL(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value) { Init(); }

		void Init()
		{
			this->SetName("eDMA_ERQL"); this->SetDescription("Enable Request Register Low");
			
			ERQ31::SetName("ERQ31"); ERQ31::SetDescription("Enable DMA Request 31");
			ERQ30::SetName("ERQ30"); ERQ30::SetDescription("Enable DMA Request 30");
			ERQ29::SetName("ERQ29"); ERQ29::SetDescription("Enable DMA Request 29");
			ERQ28::SetName("ERQ28"); ERQ28::SetDescription("Enable DMA Request 28");
			ERQ27::SetName("ERQ27"); ERQ27::SetDescription("Enable DMA Request 27");
			ERQ26::SetName("ERQ26"); ERQ26::SetDescription("Enable DMA Request 26");
			ERQ25::SetName("ERQ25"); ERQ25::SetDescription("Enable DMA Request 25");
			ERQ24::SetName("ERQ24"); ERQ24::SetDescription("Enable DMA Request 24");
			ERQ23::SetName("ERQ23"); ERQ23::SetDescription("Enable DMA Request 23");
			ERQ22::SetName("ERQ22"); ERQ22::SetDescription("Enable DMA Request 22");
			ERQ21::SetName("ERQ21"); ERQ21::SetDescription("Enable DMA Request 21");
			ERQ20::SetName("ERQ20"); ERQ20::SetDescription("Enable DMA Request 20");
			ERQ19::SetName("ERQ19"); ERQ19::SetDescription("Enable DMA Request 19");
			ERQ18::SetName("ERQ18"); ERQ18::SetDescription("Enable DMA Request 18");
			ERQ17::SetName("ERQ17"); ERQ17::SetDescription("Enable DMA Request 17");
			ERQ16::SetName("ERQ16"); ERQ16::SetDescription("Enable DMA Request 16");
			ERQ15::SetName("ERQ15"); ERQ15::SetDescription("Enable DMA Request 15");
			ERQ14::SetName("ERQ14"); ERQ14::SetDescription("Enable DMA Request 14");
			ERQ13::SetName("ERQ13"); ERQ13::SetDescription("Enable DMA Request 13");
			ERQ12::SetName("ERQ12"); ERQ12::SetDescription("Enable DMA Request 12");
			ERQ11::SetName("ERQ11"); ERQ11::SetDescription("Enable DMA Request 11");
			ERQ10::SetName("ERQ10"); ERQ10::SetDescription("Enable DMA Request 10");
			ERQ9 ::SetName("ERQ9");  ERQ9 ::SetDescription("Enable DMA Request 9");
			ERQ8 ::SetName("ERQ8");  ERQ8 ::SetDescription("Enable DMA Request 8");
			ERQ7 ::SetName("ERQ7");  ERQ7 ::SetDescription("Enable DMA Request 7");
			ERQ6 ::SetName("ERQ6");  ERQ6 ::SetDescription("Enable DMA Request 6");
			ERQ5 ::SetName("ERQ5");  ERQ5 ::SetDescription("Enable DMA Request 5");
			ERQ4 ::SetName("ERQ4");  ERQ4 ::SetDescription("Enable DMA Request 4");
			ERQ3 ::SetName("ERQ3");  ERQ3 ::SetDescription("Enable DMA Request 3");
			ERQ2 ::SetName("ERQ2");  ERQ2 ::SetDescription("Enable DMA Request 2");
			ERQ1 ::SetName("ERQ1");  ERQ1 ::SetDescription("Enable DMA Request 1");
			ERQ0 ::SetName("ERQ0");  ERQ0 ::SetDescription("Enable DMA Request 0");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
			this->edma->UpdateAllHardwareRequestStatus();
		}

		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			
			this->edma->UpdateAllHardwareRequestStatus();
			
			return rws;
		}

		using Super::operator =;
	};
	
	// Enable Error Interrupt Register High (eDMA_EEIH)
	struct EDMA_EEIH : EDMA_Register<EDMA_EEIH, 32, SW_RW>
	{
		typedef EDMA_Register<EDMA_EEIH, 32, SW_RW> Super;

		static const sc_dt::uint64 ADDRESS_OFFSET = 0x10;
	
		struct EEI63 : Field<EDMA_EEIH, EEI63, 0 > {}; // Enable error interrupt 63
		struct EEI62 : Field<EDMA_EEIH, EEI62, 1 > {}; // Enable error interrupt 62
		struct EEI61 : Field<EDMA_EEIH, EEI61, 2 > {}; // Enable error interrupt 61
		struct EEI60 : Field<EDMA_EEIH, EEI60, 3 > {}; // Enable error interrupt 60
		struct EEI59 : Field<EDMA_EEIH, EEI59, 4 > {}; // Enable error interrupt 59
		struct EEI58 : Field<EDMA_EEIH, EEI58, 5 > {}; // Enable error interrupt 58
		struct EEI57 : Field<EDMA_EEIH, EEI57, 6 > {}; // Enable error interrupt 57
		struct EEI56 : Field<EDMA_EEIH, EEI56, 7 > {}; // Enable error interrupt 56
		struct EEI55 : Field<EDMA_EEIH, EEI55, 8 > {}; // Enable error interrupt 55
		struct EEI54 : Field<EDMA_EEIH, EEI54, 9 > {}; // Enable error interrupt 54
		struct EEI53 : Field<EDMA_EEIH, EEI53, 10> {}; // Enable error interrupt 53
		struct EEI52 : Field<EDMA_EEIH, EEI52, 11> {}; // Enable error interrupt 52
		struct EEI51 : Field<EDMA_EEIH, EEI51, 12> {}; // Enable error interrupt 51
		struct EEI50 : Field<EDMA_EEIH, EEI50, 13> {}; // Enable error interrupt 50
		struct EEI49 : Field<EDMA_EEIH, EEI49, 14> {}; // Enable error interrupt 49
		struct EEI48 : Field<EDMA_EEIH, EEI48, 15> {}; // Enable error interrupt 48
		struct EEI47 : Field<EDMA_EEIH, EEI47, 16> {}; // Enable error interrupt 47
		struct EEI46 : Field<EDMA_EEIH, EEI46, 17> {}; // Enable error interrupt 46
		struct EEI45 : Field<EDMA_EEIH, EEI45, 18> {}; // Enable error interrupt 45
		struct EEI44 : Field<EDMA_EEIH, EEI44, 19> {}; // Enable error interrupt 44
		struct EEI43 : Field<EDMA_EEIH, EEI43, 20> {}; // Enable error interrupt 43
		struct EEI42 : Field<EDMA_EEIH, EEI42, 21> {}; // Enable error interrupt 42
		struct EEI41 : Field<EDMA_EEIH, EEI41, 22> {}; // Enable error interrupt 41
		struct EEI40 : Field<EDMA_EEIH, EEI40, 23> {}; // Enable error interrupt 40
		struct EEI39 : Field<EDMA_EEIH, EEI39, 24> {}; // Enable error interrupt 39
		struct EEI38 : Field<EDMA_EEIH, EEI38, 25> {}; // Enable error interrupt 38
		struct EEI37 : Field<EDMA_EEIH, EEI37, 26> {}; // Enable error interrupt 37
		struct EEI36 : Field<EDMA_EEIH, EEI36, 27> {}; // Enable error interrupt 36
		struct EEI35 : Field<EDMA_EEIH, EEI35, 28> {}; // Enable error interrupt 35
		struct EEI34 : Field<EDMA_EEIH, EEI34, 29> {}; // Enable error interrupt 34
		struct EEI33 : Field<EDMA_EEIH, EEI33, 30> {}; // Enable error interrupt 33
		struct EEI32 : Field<EDMA_EEIH, EEI32, 31> {}; // Enable error interrupt 32

		typedef FieldSet< EEI63, EEI62, EEI61, EEI60, EEI59, EEI58, EEI57, EEI56
		                , EEI55, EEI54, EEI53, EEI52, EEI51, EEI50, EEI49, EEI48
		                , EEI47, EEI46, EEI45, EEI44, EEI43, EEI42, EEI41, EEI40
		                , EEI39, EEI38, EEI37, EEI36, EEI35, EEI34, EEI33, EEI32> ALL;
	
		EDMA_EEIH(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_EEIH(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value) { Init(); }

		void Init()
		{
			this->SetName("eDMA_EEIH"); this->SetDescription("Enable Error Interrupt Register High");
			
			EEI63::SetName("EEI63"); EEI63::SetDescription("Enable error interrupt 63");
			EEI62::SetName("EEI62"); EEI62::SetDescription("Enable error interrupt 62");
			EEI61::SetName("EEI61"); EEI61::SetDescription("Enable error interrupt 61");
			EEI60::SetName("EEI60"); EEI60::SetDescription("Enable error interrupt 60");
			EEI59::SetName("EEI59"); EEI59::SetDescription("Enable error interrupt 59");
			EEI58::SetName("EEI58"); EEI58::SetDescription("Enable error interrupt 58");
			EEI57::SetName("EEI57"); EEI57::SetDescription("Enable error interrupt 57");
			EEI56::SetName("EEI56"); EEI56::SetDescription("Enable error interrupt 56");
			EEI55::SetName("EEI55"); EEI55::SetDescription("Enable error interrupt 55");
			EEI54::SetName("EEI54"); EEI54::SetDescription("Enable error interrupt 54");
			EEI53::SetName("EEI53"); EEI53::SetDescription("Enable error interrupt 53");
			EEI52::SetName("EEI52"); EEI52::SetDescription("Enable error interrupt 52");
			EEI51::SetName("EEI51"); EEI51::SetDescription("Enable error interrupt 51");
			EEI50::SetName("EEI50"); EEI50::SetDescription("Enable error interrupt 50");
			EEI49::SetName("EEI49"); EEI49::SetDescription("Enable error interrupt 49");
			EEI48::SetName("EEI48"); EEI48::SetDescription("Enable error interrupt 48");
			EEI47::SetName("EEI47"); EEI47::SetDescription("Enable error interrupt 47");
			EEI46::SetName("EEI46"); EEI46::SetDescription("Enable error interrupt 46");
			EEI45::SetName("EEI45"); EEI45::SetDescription("Enable error interrupt 45");
			EEI44::SetName("EEI44"); EEI44::SetDescription("Enable error interrupt 44");
			EEI43::SetName("EEI43"); EEI43::SetDescription("Enable error interrupt 43");
			EEI42::SetName("EEI42"); EEI42::SetDescription("Enable error interrupt 42");
			EEI41::SetName("EEI41"); EEI41::SetDescription("Enable error interrupt 41");
			EEI40::SetName("EEI40"); EEI40::SetDescription("Enable error interrupt 40");
			EEI39::SetName("EEI39"); EEI39::SetDescription("Enable error interrupt 39");
			EEI38::SetName("EEI38"); EEI38::SetDescription("Enable error interrupt 38");
			EEI37::SetName("EEI37"); EEI37::SetDescription("Enable error interrupt 37");
			EEI36::SetName("EEI36"); EEI36::SetDescription("Enable error interrupt 36");
			EEI35::SetName("EEI35"); EEI35::SetDescription("Enable error interrupt 35");
			EEI34::SetName("EEI34"); EEI34::SetDescription("Enable error interrupt 34");
			EEI33::SetName("EEI33"); EEI33::SetDescription("Enable error interrupt 33");
			EEI32::SetName("EEI32"); EEI32::SetDescription("Enable error interrupt 32");
		}

		void Reset()
		{
			this->Initialize(0x0);
		}

		using Super::operator =;
	};
	
	// Enable Error Interrupt Register Low (eDMA_EEIL)
	struct EDMA_EEIL : EDMA_Register<EDMA_EEIL, 32, SW_RW>
	{
		typedef EDMA_Register<EDMA_EEIL, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x14;
		
		struct EEI31 : Field<EDMA_EEIL, EEI31, 0>  {}; // Enable error interrupt 31
		struct EEI30 : Field<EDMA_EEIL, EEI30, 1>  {}; // Enable error interrupt 30
		struct EEI29 : Field<EDMA_EEIL, EEI29, 2>  {}; // Enable error interrupt 29
		struct EEI28 : Field<EDMA_EEIL, EEI28, 3>  {}; // Enable error interrupt 28
		struct EEI27 : Field<EDMA_EEIL, EEI27, 4>  {}; // Enable error interrupt 27
		struct EEI26 : Field<EDMA_EEIL, EEI26, 5>  {}; // Enable error interrupt 26
		struct EEI25 : Field<EDMA_EEIL, EEI25, 6>  {}; // Enable error interrupt 25
		struct EEI24 : Field<EDMA_EEIL, EEI24, 7>  {}; // Enable error interrupt 24
		struct EEI23 : Field<EDMA_EEIL, EEI23, 8>  {}; // Enable error interrupt 23
		struct EEI22 : Field<EDMA_EEIL, EEI22, 9>  {}; // Enable error interrupt 22
		struct EEI21 : Field<EDMA_EEIL, EEI21, 10> {}; // Enable error interrupt 21
		struct EEI20 : Field<EDMA_EEIL, EEI20, 11> {}; // Enable error interrupt 20
		struct EEI19 : Field<EDMA_EEIL, EEI19, 12> {}; // Enable error interrupt 19
		struct EEI18 : Field<EDMA_EEIL, EEI18, 13> {}; // Enable error interrupt 18
		struct EEI17 : Field<EDMA_EEIL, EEI17, 14> {}; // Enable error interrupt 17
		struct EEI16 : Field<EDMA_EEIL, EEI16, 15> {}; // Enable error interrupt 16
		struct EEI15 : Field<EDMA_EEIL, EEI15, 16> {}; // Enable error interrupt 15
		struct EEI14 : Field<EDMA_EEIL, EEI14, 17> {}; // Enable error interrupt 14
		struct EEI13 : Field<EDMA_EEIL, EEI13, 18> {}; // Enable error interrupt 13
		struct EEI12 : Field<EDMA_EEIL, EEI12, 19> {}; // Enable error interrupt 12
		struct EEI11 : Field<EDMA_EEIL, EEI11, 20> {}; // Enable error interrupt 11
		struct EEI10 : Field<EDMA_EEIL, EEI10, 21> {}; // Enable error interrupt 10
		struct EEI9  : Field<EDMA_EEIL,  EEI9, 22> {}; // Enable error interrupt 9
		struct EEI8  : Field<EDMA_EEIL,  EEI8, 23> {}; // Enable error interrupt 8
		struct EEI7  : Field<EDMA_EEIL,  EEI7, 24> {}; // Enable error interrupt 7
		struct EEI6  : Field<EDMA_EEIL,  EEI6, 25> {}; // Enable error interrupt 6
		struct EEI5  : Field<EDMA_EEIL,  EEI5, 26> {}; // Enable error interrupt 5
		struct EEI4  : Field<EDMA_EEIL,  EEI4, 27> {}; // Enable error interrupt 4
		struct EEI3  : Field<EDMA_EEIL,  EEI3, 28> {}; // Enable error interrupt 3
		struct EEI2  : Field<EDMA_EEIL,  EEI2, 29> {}; // Enable error interrupt 2
		struct EEI1  : Field<EDMA_EEIL,  EEI1, 30> {}; // Enable error interrupt 1
		struct EEI0  : Field<EDMA_EEIL,  EEI0, 31> {}; // Enable error interrupt 0

		typedef FieldSet< EEI31, EEI30, EEI29, EEI28, EEI27, EEI26, EEI25, EEI24
		                , EEI23, EEI22, EEI21, EEI20, EEI19, EEI18, EEI17, EEI16
		                , EEI15, EEI14, EEI13, EEI12, EEI11, EEI10,  EEI9,  EEI8
		                ,  EEI7,  EEI6,  EEI5,  EEI4,  EEI3,  EEI2,  EEI1,  EEI0> ALL;

		EDMA_EEIL(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_EEIL(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value) { Init(); }

		void Init()
		{
			this->SetName("eDMA_EEIL"); this->SetDescription("Enable Error Interrupt Register Low");
			
			EEI31::SetName("EEI31"); EEI31::SetDescription("Enable error interrupt 31");
			EEI30::SetName("EEI30"); EEI30::SetDescription("Enable error interrupt 30");
			EEI29::SetName("EEI29"); EEI29::SetDescription("Enable error interrupt 29");
			EEI28::SetName("EEI28"); EEI28::SetDescription("Enable error interrupt 28");
			EEI27::SetName("EEI27"); EEI27::SetDescription("Enable error interrupt 27");
			EEI26::SetName("EEI26"); EEI26::SetDescription("Enable error interrupt 26");
			EEI25::SetName("EEI25"); EEI25::SetDescription("Enable error interrupt 25");
			EEI24::SetName("EEI24"); EEI24::SetDescription("Enable error interrupt 24");
			EEI23::SetName("EEI23"); EEI23::SetDescription("Enable error interrupt 23");
			EEI22::SetName("EEI22"); EEI22::SetDescription("Enable error interrupt 22");
			EEI21::SetName("EEI21"); EEI21::SetDescription("Enable error interrupt 21");
			EEI20::SetName("EEI20"); EEI20::SetDescription("Enable error interrupt 20");
			EEI19::SetName("EEI19"); EEI19::SetDescription("Enable error interrupt 19");
			EEI18::SetName("EEI18"); EEI18::SetDescription("Enable error interrupt 18");
			EEI17::SetName("EEI17"); EEI17::SetDescription("Enable error interrupt 17");
			EEI16::SetName("EEI16"); EEI16::SetDescription("Enable error interrupt 16");
			EEI15::SetName("EEI15"); EEI15::SetDescription("Enable error interrupt 15");
			EEI14::SetName("EEI14"); EEI14::SetDescription("Enable error interrupt 14");
			EEI13::SetName("EEI13"); EEI13::SetDescription("Enable error interrupt 13");
			EEI12::SetName("EEI12"); EEI12::SetDescription("Enable error interrupt 12");
			EEI11::SetName("EEI11"); EEI11::SetDescription("Enable error interrupt 11");
			EEI10::SetName("EEI10"); EEI10::SetDescription("Enable error interrupt 10");
			EEI9 ::SetName("EEI9");  EEI9 ::SetDescription("Enable error interrupt 9");
			EEI8 ::SetName("EEI8");  EEI8 ::SetDescription("Enable error interrupt 8");
			EEI7 ::SetName("EEI7");  EEI7 ::SetDescription("Enable error interrupt 7");
			EEI6 ::SetName("EEI6");  EEI6 ::SetDescription("Enable error interrupt 6");
			EEI5 ::SetName("EEI5");  EEI5 ::SetDescription("Enable error interrupt 5");
			EEI4 ::SetName("EEI4");  EEI4 ::SetDescription("Enable error interrupt 4");
			EEI3 ::SetName("EEI3");  EEI3 ::SetDescription("Enable error interrupt 3");
			EEI2 ::SetName("EEI2");  EEI2 ::SetDescription("Enable error interrupt 2");
			EEI1 ::SetName("EEI1");  EEI1 ::SetDescription("Enable error interrupt 1");
			EEI0 ::SetName("EEI0");  EEI0 ::SetDescription("Enable error interrupt 0");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		using Super::operator =;
	};
	
	// Set Enable Request Register (eDMA_SERQ)
	struct EDMA_SERQ : EDMA_Register<EDMA_SERQ, 8, SW_W>
	{
		typedef EDMA_Register<EDMA_SERQ, 8, SW_W> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x18;

		struct NOP  : Field<EDMA_SERQ, NOP , 0, 0, SW_W> {}; // No operation
		struct SAER : Field<EDMA_SERQ, SAER, 1, 1, SW_W> {}; // Set all enable request
		struct SERQ : Field<EDMA_SERQ, SERQ, 2, 7, SW_W> {}; // Set enable request
		
		typedef FieldSet<NOP, SAER, SERQ> ALL;
		
		EDMA_SERQ(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_SERQ(EDMA<CONFIG> *_edma, uint8_t value) : Super(_edma, value) { Init(); }
		
		void Init()
		{
			this->SetName("eDMA_SERQ"); this->SetDescription("Set Enable Request Register");
			NOP ::SetName("NOP");  NOP ::SetDescription("No operation");
			SAER::SetName("SAER"); SAER::SetDescription("Set all enable request");
			SERQ::SetName("SERQ"); SERQ::SetDescription("Set enable request");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint8_t& value, const uint8_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				if(!this->template Get<NOP>())
				{
					if(this->template Get<SAER>())
					{
						this->edma->EnableAllRequests();
					}
					else
					{
						this->edma->EnableRequest(this->template Get<SERQ>());
					}
				}
			
				this->Reset();
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// Clear Enable Request Register (eDMA_CERQ)
	struct EDMA_CERQ : EDMA_Register<EDMA_CERQ, 8, SW_W>
	{
		typedef EDMA_Register<EDMA_CERQ, 8, SW_W> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x19;
		
		struct NOP  : Field<EDMA_CERQ, NOP , 0, 0, SW_W> {}; // No operation
		struct CAER : Field<EDMA_CERQ, CAER, 1, 1, SW_W> {}; // Clear all enable requests
		struct CERQ : Field<EDMA_CERQ, CERQ, 2, 7, SW_W> {}; // Clear enable request
		
		typedef FieldSet<NOP, CAER, CERQ> ALL;
		
		EDMA_CERQ(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_CERQ(EDMA<CONFIG> *_edma, uint8_t value) : Super(_edma, value) { Init(); }
		
		void Init()
		{
			this->SetName("eDMA_CERQ"); this->SetDescription("Clear Enable Request Register");
			
			NOP ::SetName("NOP");  NOP ::SetDescription("No operation");
			CAER::SetName("CAER"); CAER::SetDescription("Clear all enable requests");
			CERQ::SetName("CERQ"); CERQ::SetDescription("Clear enable request");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint8_t& value, const uint8_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				if(!this->template Get<NOP>())
				{
					if(this->template Get<CAER>())
					{
						this->edma->DisableAllRequests();
					}
					else
					{
						this->edma->DisableRequest(this->template Get<CERQ>());
					}
				}
			
				this->Reset();
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// Set Enable Error Interrupt Register (eDMA_SEEI)
	struct EDMA_SEEI : EDMA_Register<EDMA_SEEI, 8, SW_W>
	{
		typedef EDMA_Register<EDMA_SEEI, 8, SW_W> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x1a;
		
		struct NOP  : Field<EDMA_SEEI, NOP , 0, 0, SW_W> {}; // No operation
		struct SAEE : Field<EDMA_SEEI, SAEE, 1, 1, SW_W> {}; // Set all enable error interrupts
		struct SEEI : Field<EDMA_SEEI, SEEI, 2, 7, SW_W> {}; // Set enable error interrupt
		
		typedef FieldSet<NOP, SAEE, SEEI> ALL;
		
		EDMA_SEEI(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_SEEI(EDMA<CONFIG> *_edma, uint8_t value) : Super(_edma, value) { Init(); }
		
		void Init()
		{
			this->SetName("eDMA_SEEI"); this->SetDescription("Set Enable Error Interrupt Register");
			
			NOP ::SetName("NOP");  NOP ::SetDescription("No operation");
			SAEE::SetName("SAEE"); SAEE::SetDescription("Set all enable error interrupts");
			SEEI::SetName("SEEI"); SEEI::SetDescription("Set enable error interrupt");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint8_t& value, const uint8_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				if(!this->template Get<NOP>())
				{
					if(this->template Get<SAEE>())
					{
						this->edma->EnableAllErrorInterrupts();
					}
					else
					{
						this->edma->EnableErrorInterrupt(this->template Get<SEEI>());
					}
				}
			
				this->Reset();
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// Clear Enable Error Interrupt Register (eDMA_CEEI)
	struct EDMA_CEEI : EDMA_Register<EDMA_CEEI, 8, SW_W>
	{
		typedef EDMA_Register<EDMA_CEEI, 8, SW_W> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x1b;
		
		struct NOP  : Field<EDMA_CEEI, NOP , 0, 0, SW_W> {}; // No operation
		struct CAEE : Field<EDMA_CEEI, CAEE, 1, 1, SW_W> {}; // Clear all enable error interrupts
		struct CEEI : Field<EDMA_CEEI, CEEI, 2, 7, SW_W> {}; // Clear enable error interrupt

		typedef FieldSet<NOP, CAEE, CEEI> ALL;
		
		EDMA_CEEI(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_CEEI(EDMA<CONFIG> *_edma, uint8_t value) : Super(_edma, value) { Init(); }
		
		void Init()
		{
			this->SetName("eDMA_CEEI"); this->SetDescription("Clear Enable Error Interrupt Register");
			
			NOP ::SetName("NOP");  NOP ::SetDescription("No operation");
			CAEE::SetName("CAEE"); CAEE::SetDescription("Clear all enable error interrupts");
			CEEI::SetName("CEEI"); CEEI::SetDescription("Clear enable error interrupt");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint8_t& value, const uint8_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				if(!this->template Get<NOP>())
				{
					if(this->template Get<CAEE>())
					{
						this->edma->DisableAllErrorInterrupts();
					}
					else
					{
						this->edma->DisableErrorInterrupt(this->template Get<CEEI>());
					}
				}
			
				this->Reset();
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// Clear Interrupt Request Register (eDMA_CINT)
	struct EDMA_CINT : EDMA_Register<EDMA_CINT, 8, SW_W>
	{
		typedef EDMA_Register<EDMA_CINT, 8, SW_W> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x1c;
		
		struct NOP  : Field<EDMA_CINT, NOP , 0, 0, SW_W> {}; // No operation
		struct CAIR : Field<EDMA_CINT, CAIR, 1, 1, SW_W> {}; // Clear all interrupt requests
		struct CINT : Field<EDMA_CINT, CINT, 2, 7, SW_W> {}; // Clear interrupt request

		typedef FieldSet<NOP, CAIR, CINT> ALL;
		
		EDMA_CINT(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_CINT(EDMA<CONFIG> *_edma, uint8_t value) : Super(_edma, value) { Init(); }
		
		void Init()
		{
			this->SetName("eDMA_CINT"); this->SetDescription("Clear Interrupt Request Register");
			
			NOP ::SetName("NOP");  NOP ::SetDescription("No operation");
			CAIR::SetName("CAIR"); CAIR::SetDescription("Clear all interrupt requests");
			CINT::SetName("CINT"); CINT::SetDescription("Clear interrupt request");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint8_t& value, const uint8_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				if(!this->template Get<NOP>())
				{
					if(this->template Get<CAIR>())
					{
						this->edma->ClearAllInterruptRequests();
					}
					else
					{
						this->edma->ClearInterruptRequest(this->template Get<CINT>());
					}
				}
			
				this->Reset();
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// Clear Error Register (eDMA_CERR)
	struct EDMA_CERR : EDMA_Register<EDMA_CERR, 8, SW_W>
	{
		typedef EDMA_Register<EDMA_CERR, 8, SW_W> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x1d;
		
		struct NOP  : Field<EDMA_CERR, NOP , 0, 0, SW_W> {}; // No operation
		struct CAEI : Field<EDMA_CERR, CAEI, 1, 1, SW_W> {}; // Clear all error indicators
		struct CERR : Field<EDMA_CERR, CERR, 2, 7, SW_W> {}; // Clear error indicator

		typedef FieldSet<NOP, CAEI, CERR> ALL;
		
		EDMA_CERR(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_CERR(EDMA<CONFIG> *_edma, uint8_t value) : Super(_edma, value) { Init(); }
		
		void Init()
		{
			this->SetName("eDMA_CERR"); this->SetDescription("Clear Error Register");
			
			NOP ::SetName("NOP");  NOP ::SetDescription("No operation");
			CAEI::SetName("CAEI"); CAEI::SetDescription("Clear all error indicators");
			CERR::SetName("CERR"); CERR::SetDescription("Clear error indicator");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint8_t& value, const uint8_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				if(!this->template Get<NOP>())
				{
					if(this->template Get<CAEI>())
					{
						this->edma->ClearAllErrorIndicators();
					}
					else
					{
						this->edma->ClearErrorIndicator(this->template Get<CERR>());
					}
				}
			
				this->Reset();
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// Set START Bit Register (eDMA_SSRT)
	struct EDMA_SSRT : EDMA_Register<EDMA_SSRT, 8, SW_W>
	{
		typedef EDMA_Register<EDMA_SSRT, 8, SW_W> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x1e;
		
		struct NOP  : Field<EDMA_SSRT, NOP , 0, 0, SW_W> {}; // No operation
		struct SAST : Field<EDMA_SSRT, SAST, 1, 1, SW_W> {}; // Set all START bits
		struct SSRT : Field<EDMA_SSRT, SSRT, 2, 7, SW_W> {}; // Set START bit

		typedef FieldSet<NOP, SAST, SSRT> ALL;
		
		EDMA_SSRT(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_SSRT(EDMA<CONFIG> *_edma, uint8_t value) : Super(_edma, value) { Init(); }
		
		void Init()
		{
			this->SetName("eDMA_SSRT"); this->SetDescription("Set START Bit Register");
			
			NOP ::SetName("NOP");  NOP ::SetDescription("No operation");
			SAST::SetName("SAST"); SAST::SetDescription("Set all START bits");
			SSRT::SetName("SSRT"); SSRT::SetDescription("Set START bit");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint8_t& value, const uint8_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				if(!this->template Get<NOP>())
				{
					if(this->template Get<SAST>())
					{
						this->edma->SetAllStartBits();
					}
					else
					{
						this->edma->SetStartBit(this->template Get<SSRT>());
					}
				}
			
				this->Reset();
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// Clear DONE Status Bit Register (eDMA_CDNE)
	struct EDMA_CDNE : EDMA_Register<EDMA_CDNE, 8, SW_W>
	{
		typedef EDMA_Register<EDMA_CDNE, 8, SW_W> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x1f;
		
		struct NOP  : Field<EDMA_CDNE, NOP , 0, 0, SW_W> {}; // No operation
		struct CADN : Field<EDMA_CDNE, CADN, 1, 1, SW_W> {}; // Clears all DONE bits
		struct CDNE : Field<EDMA_CDNE, CDNE, 2, 7, SW_W> {}; // Clear DONE bit

		typedef FieldSet<NOP, CADN, CDNE> ALL;
		
		EDMA_CDNE(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_CDNE(EDMA<CONFIG> *_edma, uint8_t value) : Super(_edma, value) { Init(); }
		
		void Init()
		{
			this->SetName("eDMA_CDNE"); this->SetDescription("Clear DONE Status Bit Register");
			
			NOP ::SetName("NOP");  NOP ::SetDescription("No operation");
			CADN::SetName("CADN"); CADN::SetDescription("Clears all DONE bits");
			CDNE::SetName("CDNE"); CDNE::SetDescription("Clear DONE bit");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint8_t& value, const uint8_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				if(!this->template Get<NOP>())
				{
					if(this->template Get<CADN>())
					{
						this->edma->ClearAllDoneBits();
					}
					else
					{
						this->edma->ClearDoneBit(this->template Get<CDNE>());
					}
				}
			
				this->Reset();
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// Interrupt Request Register High (eDMA_INTH)
	struct EDMA_INTH : EDMA_Register<EDMA_INTH, 32, SW_R_W1C>
	{
		typedef EDMA_Register<EDMA_INTH, 32, SW_R_W1C> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x20;
		
		struct INT63 : Field<EDMA_INTH, INT63, 0, 0, SW_R_W1C>   {}; // Interrupt request 63
		struct INT62 : Field<EDMA_INTH, INT62, 1, 1, SW_R_W1C>   {}; // Interrupt request 62
		struct INT61 : Field<EDMA_INTH, INT61, 2, 2, SW_R_W1C>   {}; // Interrupt request 61
		struct INT60 : Field<EDMA_INTH, INT60, 3, 3, SW_R_W1C>   {}; // Interrupt request 60
		struct INT59 : Field<EDMA_INTH, INT59, 4, 4, SW_R_W1C>   {}; // Interrupt request 59
		struct INT58 : Field<EDMA_INTH, INT58, 5, 5, SW_R_W1C>   {}; // Interrupt request 58
		struct INT57 : Field<EDMA_INTH, INT57, 6, 6, SW_R_W1C>   {}; // Interrupt request 57
		struct INT56 : Field<EDMA_INTH, INT56, 7, 7, SW_R_W1C>   {}; // Interrupt request 56
		struct INT55 : Field<EDMA_INTH, INT55, 8, 8, SW_R_W1C>   {}; // Interrupt request 55
		struct INT54 : Field<EDMA_INTH, INT54, 9, 9, SW_R_W1C>   {}; // Interrupt request 54
		struct INT53 : Field<EDMA_INTH, INT53, 10, 10, SW_R_W1C> {}; // Interrupt request 53
		struct INT52 : Field<EDMA_INTH, INT52, 11, 11, SW_R_W1C> {}; // Interrupt request 52
		struct INT51 : Field<EDMA_INTH, INT51, 12, 12, SW_R_W1C> {}; // Interrupt request 51
		struct INT50 : Field<EDMA_INTH, INT50, 13, 13, SW_R_W1C> {}; // Interrupt request 50
		struct INT49 : Field<EDMA_INTH, INT49, 14, 14, SW_R_W1C> {}; // Interrupt request 49
		struct INT48 : Field<EDMA_INTH, INT48, 15, 15, SW_R_W1C> {}; // Interrupt request 48
		struct INT47 : Field<EDMA_INTH, INT47, 16, 16, SW_R_W1C> {}; // Interrupt request 47
		struct INT46 : Field<EDMA_INTH, INT46, 17, 17, SW_R_W1C> {}; // Interrupt request 46
		struct INT45 : Field<EDMA_INTH, INT45, 18, 18, SW_R_W1C> {}; // Interrupt request 45
		struct INT44 : Field<EDMA_INTH, INT44, 19, 19, SW_R_W1C> {}; // Interrupt request 44
		struct INT43 : Field<EDMA_INTH, INT43, 20, 20, SW_R_W1C> {}; // Interrupt request 43
		struct INT42 : Field<EDMA_INTH, INT42, 21, 21, SW_R_W1C> {}; // Interrupt request 42
		struct INT41 : Field<EDMA_INTH, INT41, 22, 22, SW_R_W1C> {}; // Interrupt request 41
		struct INT40 : Field<EDMA_INTH, INT40, 23, 23, SW_R_W1C> {}; // Interrupt request 40
		struct INT39 : Field<EDMA_INTH, INT39, 24, 24, SW_R_W1C> {}; // Interrupt request 39
		struct INT38 : Field<EDMA_INTH, INT38, 25, 25, SW_R_W1C> {}; // Interrupt request 38
		struct INT37 : Field<EDMA_INTH, INT37, 26, 26, SW_R_W1C> {}; // Interrupt request 37
		struct INT36 : Field<EDMA_INTH, INT36, 27, 27, SW_R_W1C> {}; // Interrupt request 36
		struct INT35 : Field<EDMA_INTH, INT35, 28, 28, SW_R_W1C> {}; // Interrupt request 35
		struct INT34 : Field<EDMA_INTH, INT34, 29, 29, SW_R_W1C> {}; // Interrupt request 34
		struct INT33 : Field<EDMA_INTH, INT33, 30, 30, SW_R_W1C> {}; // Interrupt request 33
		struct INT32 : Field<EDMA_INTH, INT32, 31, 31, SW_R_W1C> {}; // Interrupt request 32
		
		typedef FieldSet< INT63, INT62, INT61, INT60, INT59, INT58, INT57, INT56
		                , INT55, INT54, INT53, INT52, INT51, INT50, INT49, INT48
		                , INT47, INT46, INT45, INT44, INT43, INT42, INT41, INT40
		                , INT39, INT38, INT37, INT36, INT35, INT34, INT33, INT32> ALL;
		
		EDMA_INTH(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_INTH(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value) { Init(); }
		
		void Init()
		{
			this->SetName("eDMA_INTH"); this->SetDescription("Interrupt Request Register High");
			
			INT63::SetName("INT63"); INT63::SetDescription("Interrupt request 63");
			INT62::SetName("INT62"); INT62::SetDescription("Interrupt request 62");
			INT61::SetName("INT61"); INT61::SetDescription("Interrupt request 61");
			INT60::SetName("INT60"); INT60::SetDescription("Interrupt request 60");
			INT59::SetName("INT59"); INT59::SetDescription("Interrupt request 59");
			INT58::SetName("INT58"); INT58::SetDescription("Interrupt request 58");
			INT57::SetName("INT57"); INT57::SetDescription("Interrupt request 57");
			INT56::SetName("INT56"); INT56::SetDescription("Interrupt request 56");
			INT55::SetName("INT55"); INT55::SetDescription("Interrupt request 55");
			INT54::SetName("INT54"); INT54::SetDescription("Interrupt request 54");
			INT53::SetName("INT53"); INT53::SetDescription("Interrupt request 53");
			INT52::SetName("INT52"); INT52::SetDescription("Interrupt request 52");
			INT51::SetName("INT51"); INT51::SetDescription("Interrupt request 51");
			INT50::SetName("INT50"); INT50::SetDescription("Interrupt request 50");
			INT49::SetName("INT49"); INT49::SetDescription("Interrupt request 49");
			INT48::SetName("INT48"); INT48::SetDescription("Interrupt request 48");
			INT47::SetName("INT47"); INT47::SetDescription("Interrupt request 47");
			INT46::SetName("INT46"); INT46::SetDescription("Interrupt request 46");
			INT45::SetName("INT45"); INT45::SetDescription("Interrupt request 45");
			INT44::SetName("INT44"); INT44::SetDescription("Interrupt request 44");
			INT43::SetName("INT43"); INT43::SetDescription("Interrupt request 43");
			INT42::SetName("INT42"); INT42::SetDescription("Interrupt request 42");
			INT41::SetName("INT41"); INT41::SetDescription("Interrupt request 41");
			INT40::SetName("INT40"); INT40::SetDescription("Interrupt request 40");
			INT39::SetName("INT39"); INT39::SetDescription("Interrupt request 39");
			INT38::SetName("INT38"); INT38::SetDescription("Interrupt request 38");
			INT37::SetName("INT37"); INT37::SetDescription("Interrupt request 37");
			INT36::SetName("INT36"); INT36::SetDescription("Interrupt request 36");
			INT35::SetName("INT35"); INT35::SetDescription("Interrupt request 35");
			INT34::SetName("INT34"); INT34::SetDescription("Interrupt request 34");
			INT33::SetName("INT33"); INT33::SetDescription("Interrupt request 33");
			INT32::SetName("INT32"); INT32::SetDescription("Interrupt request 32");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
			this->edma->UpdateInterruptRequests();
		}

		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->edma->UpdateInterruptRequests();
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// Interrupt Request Register Low (eDMA_INTL)
	struct EDMA_INTL : EDMA_Register<EDMA_INTL, 32, SW_R_W1C>
	{
		typedef EDMA_Register<EDMA_INTL, 32, SW_R_W1C> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x24;
		
		struct INT31 : Field<EDMA_INTL, INT31, 0, 0, SW_R_W1C>   {}; // Interrupt request 31
		struct INT30 : Field<EDMA_INTL, INT30, 1, 1, SW_R_W1C>   {}; // Interrupt request 30
		struct INT29 : Field<EDMA_INTL, INT29, 2, 2, SW_R_W1C>   {}; // Interrupt request 29
		struct INT28 : Field<EDMA_INTL, INT28, 3, 3, SW_R_W1C>   {}; // Interrupt request 28
		struct INT27 : Field<EDMA_INTL, INT27, 4, 4, SW_R_W1C>   {}; // Interrupt request 27
		struct INT26 : Field<EDMA_INTL, INT26, 5, 5, SW_R_W1C>   {}; // Interrupt request 26
		struct INT25 : Field<EDMA_INTL, INT25, 6, 6, SW_R_W1C>   {}; // Interrupt request 25
		struct INT24 : Field<EDMA_INTL, INT24, 7, 7, SW_R_W1C>   {}; // Interrupt request 24
		struct INT23 : Field<EDMA_INTL, INT23, 8, 8, SW_R_W1C>   {}; // Interrupt request 23
		struct INT22 : Field<EDMA_INTL, INT22, 9, 9, SW_R_W1C>   {}; // Interrupt request 22
		struct INT21 : Field<EDMA_INTL, INT21, 10, 10, SW_R_W1C> {}; // Interrupt request 21
		struct INT20 : Field<EDMA_INTL, INT20, 11, 11, SW_R_W1C> {}; // Interrupt request 20
		struct INT19 : Field<EDMA_INTL, INT19, 12, 12, SW_R_W1C> {}; // Interrupt request 19
		struct INT18 : Field<EDMA_INTL, INT18, 13, 13, SW_R_W1C> {}; // Interrupt request 18
		struct INT17 : Field<EDMA_INTL, INT17, 14, 14, SW_R_W1C> {}; // Interrupt request 17
		struct INT16 : Field<EDMA_INTL, INT16, 15, 15, SW_R_W1C> {}; // Interrupt request 16
		struct INT15 : Field<EDMA_INTL, INT15, 16, 16, SW_R_W1C> {}; // Interrupt request 15
		struct INT14 : Field<EDMA_INTL, INT14, 17, 17, SW_R_W1C> {}; // Interrupt request 14
		struct INT13 : Field<EDMA_INTL, INT13, 18, 18, SW_R_W1C> {}; // Interrupt request 13
		struct INT12 : Field<EDMA_INTL, INT12, 19, 19, SW_R_W1C> {}; // Interrupt request 12
		struct INT11 : Field<EDMA_INTL, INT11, 20, 20, SW_R_W1C> {}; // Interrupt request 11
		struct INT10 : Field<EDMA_INTL, INT10, 21, 21, SW_R_W1C> {}; // Interrupt request 10
		struct INT9  : Field<EDMA_INTL, INT9, 22, 22, SW_R_W1C>  {}; // Interrupt request 9
		struct INT8  : Field<EDMA_INTL, INT8, 23, 23, SW_R_W1C>  {}; // Interrupt request 8
		struct INT7  : Field<EDMA_INTL, INT7, 24, 24, SW_R_W1C>  {}; // Interrupt request 7
		struct INT6  : Field<EDMA_INTL, INT6, 25, 25, SW_R_W1C>  {}; // Interrupt request 6
		struct INT5  : Field<EDMA_INTL, INT5, 26, 26, SW_R_W1C>  {}; // Interrupt request 5
		struct INT4  : Field<EDMA_INTL, INT4, 27, 27, SW_R_W1C>  {}; // Interrupt request 4
		struct INT3  : Field<EDMA_INTL, INT3, 28, 28, SW_R_W1C>  {}; // Interrupt request 3
		struct INT2  : Field<EDMA_INTL, INT2, 29, 29, SW_R_W1C>  {}; // Interrupt request 2
		struct INT1  : Field<EDMA_INTL, INT1, 30, 30, SW_R_W1C>  {}; // Interrupt request 1
		struct INT0  : Field<EDMA_INTL, INT0, 31, 31, SW_R_W1C>  {}; // Interrupt request 0
		
		typedef FieldSet< INT31, INT30, INT29, INT28, INT27, INT26, INT25, INT24
		                , INT23, INT22, INT21, INT20, INT19, INT18, INT17, INT16
		                , INT15, INT14, INT13, INT12, INT11, INT10, INT9, INT8
		                , INT7, INT6, INT5, INT4, INT3, INT2, INT1, INT0> ALL;
		
		EDMA_INTL(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_INTL(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value) { Init(); }

		void Init()
		{
			this->SetName("eDMA_INTL"); this->SetDescription("Interrupt Request Register Low");
			
			INT31::SetName("INT31"); INT31::SetDescription("Interrupt request 31");
			INT30::SetName("INT30"); INT30::SetDescription("Interrupt request 30");
			INT29::SetName("INT29"); INT29::SetDescription("Interrupt request 29");
			INT28::SetName("INT28"); INT28::SetDescription("Interrupt request 28");
			INT27::SetName("INT27"); INT27::SetDescription("Interrupt request 27");
			INT26::SetName("INT26"); INT26::SetDescription("Interrupt request 26");
			INT25::SetName("INT25"); INT25::SetDescription("Interrupt request 25");
			INT24::SetName("INT24"); INT24::SetDescription("Interrupt request 24");
			INT23::SetName("INT23"); INT23::SetDescription("Interrupt request 23");
			INT22::SetName("INT22"); INT22::SetDescription("Interrupt request 22");
			INT21::SetName("INT21"); INT21::SetDescription("Interrupt request 21");
			INT20::SetName("INT20"); INT20::SetDescription("Interrupt request 20");
			INT19::SetName("INT19"); INT19::SetDescription("Interrupt request 19");
			INT18::SetName("INT18"); INT18::SetDescription("Interrupt request 18");
			INT17::SetName("INT17"); INT17::SetDescription("Interrupt request 17");
			INT16::SetName("INT16"); INT16::SetDescription("Interrupt request 16");
			INT15::SetName("INT15"); INT15::SetDescription("Interrupt request 15");
			INT14::SetName("INT14"); INT14::SetDescription("Interrupt request 14");
			INT13::SetName("INT13"); INT13::SetDescription("Interrupt request 13");
			INT12::SetName("INT12"); INT12::SetDescription("Interrupt request 12");
			INT11::SetName("INT11"); INT11::SetDescription("Interrupt request 11");
			INT10::SetName("INT10"); INT10::SetDescription("Interrupt request 10");
			INT9 ::SetName("INT9 "); INT9 ::SetDescription("Interrupt request 9");
			INT8 ::SetName("INT8 "); INT8 ::SetDescription("Interrupt request 8");
			INT7 ::SetName("INT7 "); INT7 ::SetDescription("Interrupt request 7");
			INT6 ::SetName("INT6 "); INT6 ::SetDescription("Interrupt request 6");
			INT5 ::SetName("INT5 "); INT5 ::SetDescription("Interrupt request 5");
			INT4 ::SetName("INT4 "); INT4 ::SetDescription("Interrupt request 4");
			INT3 ::SetName("INT3 "); INT3 ::SetDescription("Interrupt request 3");
			INT2 ::SetName("INT2 "); INT2 ::SetDescription("Interrupt request 2");
			INT1 ::SetName("INT1 "); INT1 ::SetDescription("Interrupt request 1");
			INT0 ::SetName("INT0 "); INT0 ::SetDescription("Interrupt request 0");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
			this->edma->UpdateInterruptRequests();
		}

		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->edma->UpdateInterruptRequests();
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// Error Register High (eDMA_ERRH)
	struct EDMA_ERRH : EDMA_Register<EDMA_ERRH, 32, SW_R_W1C>
	{
		typedef EDMA_Register<EDMA_ERRH, 32, SW_R_W1C> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x28;
		
		struct ERR63 : Field<EDMA_ERRH, ERR63, 0, 0, SW_R_W1C>   {}; // Error in channel 63
		struct ERR62 : Field<EDMA_ERRH, ERR62, 1, 1, SW_R_W1C>   {}; // Error in channel 62
		struct ERR61 : Field<EDMA_ERRH, ERR61, 2, 2, SW_R_W1C>   {}; // Error in channel 61
		struct ERR60 : Field<EDMA_ERRH, ERR60, 3, 3, SW_R_W1C>   {}; // Error in channel 60
		struct ERR59 : Field<EDMA_ERRH, ERR59, 4, 4, SW_R_W1C>   {}; // Error in channel 59
		struct ERR58 : Field<EDMA_ERRH, ERR58, 5, 5, SW_R_W1C>   {}; // Error in channel 58
		struct ERR57 : Field<EDMA_ERRH, ERR57, 6, 6, SW_R_W1C>   {}; // Error in channel 57
		struct ERR56 : Field<EDMA_ERRH, ERR56, 7, 7, SW_R_W1C>   {}; // Error in channel 56
		struct ERR55 : Field<EDMA_ERRH, ERR55, 8, 8, SW_R_W1C>   {}; // Error in channel 55
		struct ERR54 : Field<EDMA_ERRH, ERR54, 9, 9, SW_R_W1C>   {}; // Error in channel 54
		struct ERR53 : Field<EDMA_ERRH, ERR53, 10, 10, SW_R_W1C> {}; // Error in channel 53
		struct ERR52 : Field<EDMA_ERRH, ERR52, 11, 11, SW_R_W1C> {}; // Error in channel 52
		struct ERR51 : Field<EDMA_ERRH, ERR51, 12, 12, SW_R_W1C> {}; // Error in channel 51
		struct ERR50 : Field<EDMA_ERRH, ERR50, 13, 13, SW_R_W1C> {}; // Error in channel 50
		struct ERR49 : Field<EDMA_ERRH, ERR49, 14, 14, SW_R_W1C> {}; // Error in channel 49
		struct ERR48 : Field<EDMA_ERRH, ERR48, 15, 15, SW_R_W1C> {}; // Error in channel 48
		struct ERR47 : Field<EDMA_ERRH, ERR47, 16, 16, SW_R_W1C> {}; // Error in channel 47
		struct ERR46 : Field<EDMA_ERRH, ERR46, 17, 17, SW_R_W1C> {}; // Error in channel 46
		struct ERR45 : Field<EDMA_ERRH, ERR45, 18, 18, SW_R_W1C> {}; // Error in channel 45
		struct ERR44 : Field<EDMA_ERRH, ERR44, 19, 19, SW_R_W1C> {}; // Error in channel 44
		struct ERR43 : Field<EDMA_ERRH, ERR43, 20, 20, SW_R_W1C> {}; // Error in channel 43
		struct ERR42 : Field<EDMA_ERRH, ERR42, 21, 21, SW_R_W1C> {}; // Error in channel 42
		struct ERR41 : Field<EDMA_ERRH, ERR41, 22, 22, SW_R_W1C> {}; // Error in channel 41
		struct ERR40 : Field<EDMA_ERRH, ERR40, 23, 23, SW_R_W1C> {}; // Error in channel 40
		struct ERR39 : Field<EDMA_ERRH, ERR39, 24, 24, SW_R_W1C> {}; // Error in channel 39
		struct ERR38 : Field<EDMA_ERRH, ERR38, 25, 25, SW_R_W1C> {}; // Error in channel 38
		struct ERR37 : Field<EDMA_ERRH, ERR37, 26, 26, SW_R_W1C> {}; // Error in channel 37
		struct ERR36 : Field<EDMA_ERRH, ERR36, 27, 27, SW_R_W1C> {}; // Error in channel 36
		struct ERR35 : Field<EDMA_ERRH, ERR35, 28, 28, SW_R_W1C> {}; // Error in channel 35
		struct ERR34 : Field<EDMA_ERRH, ERR34, 29, 29, SW_R_W1C> {}; // Error in channel 34
		struct ERR33 : Field<EDMA_ERRH, ERR33, 30, 30, SW_R_W1C> {}; // Error in channel 33
		struct ERR32 : Field<EDMA_ERRH, ERR32, 31, 31, SW_R_W1C> {}; // Error in channel 32

		typedef FieldSet< ERR63, ERR62, ERR61, ERR60, ERR59, ERR58, ERR57, ERR56
		                , ERR55, ERR54, ERR53, ERR52, ERR51, ERR50, ERR49, ERR48
		                , ERR47, ERR46, ERR45, ERR44, ERR43, ERR42, ERR41, ERR40
		                , ERR39, ERR38, ERR37, ERR36, ERR35, ERR34, ERR33, ERR32> ALL;
		
		EDMA_ERRH(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_ERRH(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value) { Init(); }
		
		void Init()
		{
			this->SetName("eDMA_ERRH"); this->SetDescription("Error Register High");
			
			ERR63::SetName("ERR63"); ERR63::SetDescription("Error in channel 63");
			ERR62::SetName("ERR62"); ERR62::SetDescription("Error in channel 62");
			ERR61::SetName("ERR61"); ERR61::SetDescription("Error in channel 61");
			ERR60::SetName("ERR60"); ERR60::SetDescription("Error in channel 60");
			ERR59::SetName("ERR59"); ERR59::SetDescription("Error in channel 59");
			ERR58::SetName("ERR58"); ERR58::SetDescription("Error in channel 58");
			ERR57::SetName("ERR57"); ERR57::SetDescription("Error in channel 57");
			ERR56::SetName("ERR56"); ERR56::SetDescription("Error in channel 56");
			ERR55::SetName("ERR55"); ERR55::SetDescription("Error in channel 55");
			ERR54::SetName("ERR54"); ERR54::SetDescription("Error in channel 54");
			ERR53::SetName("ERR53"); ERR53::SetDescription("Error in channel 53");
			ERR52::SetName("ERR52"); ERR52::SetDescription("Error in channel 52");
			ERR51::SetName("ERR51"); ERR51::SetDescription("Error in channel 51");
			ERR50::SetName("ERR50"); ERR50::SetDescription("Error in channel 50");
			ERR49::SetName("ERR49"); ERR49::SetDescription("Error in channel 49");
			ERR48::SetName("ERR48"); ERR48::SetDescription("Error in channel 48");
			ERR47::SetName("ERR47"); ERR47::SetDescription("Error in channel 47");
			ERR46::SetName("ERR46"); ERR46::SetDescription("Error in channel 46");
			ERR45::SetName("ERR45"); ERR45::SetDescription("Error in channel 45");
			ERR44::SetName("ERR44"); ERR44::SetDescription("Error in channel 44");
			ERR43::SetName("ERR43"); ERR43::SetDescription("Error in channel 43");
			ERR42::SetName("ERR42"); ERR42::SetDescription("Error in channel 42");
			ERR41::SetName("ERR41"); ERR41::SetDescription("Error in channel 41");
			ERR40::SetName("ERR40"); ERR40::SetDescription("Error in channel 40");
			ERR39::SetName("ERR39"); ERR39::SetDescription("Error in channel 39");
			ERR38::SetName("ERR38"); ERR38::SetDescription("Error in channel 38");
			ERR37::SetName("ERR37"); ERR37::SetDescription("Error in channel 37");
			ERR36::SetName("ERR36"); ERR36::SetDescription("Error in channel 36");
			ERR35::SetName("ERR35"); ERR35::SetDescription("Error in channel 35");
			ERR34::SetName("ERR34"); ERR34::SetDescription("Error in channel 34");
			ERR33::SetName("ERR33"); ERR33::SetDescription("Error in channel 33");
			ERR32::SetName("ERR32"); ERR32::SetDescription("Error in channel 32");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
			this->edma->UpdateErrors();
		}

		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->edma->UpdateVLD();
				this->edma->UpdateErrors();
			}
			
			return rws;
		}
		
		using Super::operator =;
	};
	
	// Error Register Low (eDMA_ERRL)
	struct EDMA_ERRL : EDMA_Register<EDMA_ERRL, 32, SW_R_W1C>
	{
		typedef EDMA_Register<EDMA_ERRL, 32, SW_R_W1C> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x2c;

		struct ERR31 : Field<EDMA_ERRL, ERR31, 0, 0, SW_R_W1C>   {}; // Error in channel 31
		struct ERR30 : Field<EDMA_ERRL, ERR30, 1, 1, SW_R_W1C>   {}; // Error in channel 30
		struct ERR29 : Field<EDMA_ERRL, ERR29, 2, 2, SW_R_W1C>   {}; // Error in channel 29
		struct ERR28 : Field<EDMA_ERRL, ERR28, 3, 3, SW_R_W1C>   {}; // Error in channel 28
		struct ERR27 : Field<EDMA_ERRL, ERR27, 4, 4, SW_R_W1C>   {}; // Error in channel 27
		struct ERR26 : Field<EDMA_ERRL, ERR26, 5, 5, SW_R_W1C>   {}; // Error in channel 26
		struct ERR25 : Field<EDMA_ERRL, ERR25, 6, 6, SW_R_W1C>   {}; // Error in channel 25
		struct ERR24 : Field<EDMA_ERRL, ERR24, 7, 7, SW_R_W1C>   {}; // Error in channel 24
		struct ERR23 : Field<EDMA_ERRL, ERR23, 8, 8, SW_R_W1C>   {}; // Error in channel 23
		struct ERR22 : Field<EDMA_ERRL, ERR22, 9, 9, SW_R_W1C>   {}; // Error in channel 22
		struct ERR21 : Field<EDMA_ERRL, ERR21, 10, 10, SW_R_W1C> {}; // Error in channel 21
		struct ERR20 : Field<EDMA_ERRL, ERR20, 11, 11, SW_R_W1C> {}; // Error in channel 20
		struct ERR19 : Field<EDMA_ERRL, ERR19, 12, 12, SW_R_W1C> {}; // Error in channel 19
		struct ERR18 : Field<EDMA_ERRL, ERR18, 13, 13, SW_R_W1C> {}; // Error in channel 18
		struct ERR17 : Field<EDMA_ERRL, ERR17, 14, 14, SW_R_W1C> {}; // Error in channel 17
		struct ERR16 : Field<EDMA_ERRL, ERR16, 15, 15, SW_R_W1C> {}; // Error in channel 16
		struct ERR15 : Field<EDMA_ERRL, ERR15, 16, 16, SW_R_W1C> {}; // Error in channel 15
		struct ERR14 : Field<EDMA_ERRL, ERR14, 17, 17, SW_R_W1C> {}; // Error in channel 14
		struct ERR13 : Field<EDMA_ERRL, ERR13, 18, 18, SW_R_W1C> {}; // Error in channel 13
		struct ERR12 : Field<EDMA_ERRL, ERR12, 19, 19, SW_R_W1C> {}; // Error in channel 12
		struct ERR11 : Field<EDMA_ERRL, ERR11, 20, 20, SW_R_W1C> {}; // Error in channel 11
		struct ERR10 : Field<EDMA_ERRL, ERR10, 21, 21, SW_R_W1C> {}; // Error in channel 10
		struct ERR9  : Field<EDMA_ERRL, ERR9, 22, 22, SW_R_W1C>  {}; // Error in channel 9
		struct ERR8  : Field<EDMA_ERRL, ERR8, 23, 23, SW_R_W1C>  {}; // Error in channel 8
		struct ERR7  : Field<EDMA_ERRL, ERR7, 24, 24, SW_R_W1C>  {}; // Error in channel 7
		struct ERR6  : Field<EDMA_ERRL, ERR6, 25, 25, SW_R_W1C>  {}; // Error in channel 6
		struct ERR5  : Field<EDMA_ERRL, ERR5, 26, 26, SW_R_W1C>  {}; // Error in channel 5
		struct ERR4  : Field<EDMA_ERRL, ERR4, 27, 27, SW_R_W1C>  {}; // Error in channel 4
		struct ERR3  : Field<EDMA_ERRL, ERR3, 28, 28, SW_R_W1C>  {}; // Error in channel 3
		struct ERR2  : Field<EDMA_ERRL, ERR2, 29, 29, SW_R_W1C>  {}; // Error in channel 2
		struct ERR1  : Field<EDMA_ERRL, ERR1, 30, 30, SW_R_W1C>  {}; // Error in channel 1
		struct ERR0  : Field<EDMA_ERRL, ERR0, 31, 31, SW_R_W1C>  {}; // Error in channel 0
		
		typedef FieldSet< ERR31, ERR30, ERR29, ERR28, ERR27, ERR26, ERR25, ERR24
		                , ERR23, ERR22, ERR21, ERR20, ERR19, ERR18, ERR17, ERR16
		                , ERR15, ERR14, ERR13, ERR12, ERR11, ERR10, ERR9, ERR8
		                , ERR7, ERR6, ERR5, ERR4, ERR3, ERR2, ERR1, ERR0> ALL;
		
		EDMA_ERRL(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_ERRL(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value) { Init(); }
		
		void Init()
		{
			this->SetName("eDMA_ERRL"); this->SetDescription("Error Register Low");
			
			ERR31::SetName("ERR31"); ERR31::SetDescription("Error in channel 31");
			ERR30::SetName("ERR30"); ERR30::SetDescription("Error in channel 30");
			ERR29::SetName("ERR29"); ERR29::SetDescription("Error in channel 29");
			ERR28::SetName("ERR28"); ERR28::SetDescription("Error in channel 28");
			ERR27::SetName("ERR27"); ERR27::SetDescription("Error in channel 27");
			ERR26::SetName("ERR26"); ERR26::SetDescription("Error in channel 26");
			ERR25::SetName("ERR25"); ERR25::SetDescription("Error in channel 25");
			ERR24::SetName("ERR24"); ERR24::SetDescription("Error in channel 24");
			ERR23::SetName("ERR23"); ERR23::SetDescription("Error in channel 23");
			ERR22::SetName("ERR22"); ERR22::SetDescription("Error in channel 22");
			ERR21::SetName("ERR21"); ERR21::SetDescription("Error in channel 21");
			ERR20::SetName("ERR20"); ERR20::SetDescription("Error in channel 20");
			ERR19::SetName("ERR19"); ERR19::SetDescription("Error in channel 19");
			ERR18::SetName("ERR18"); ERR18::SetDescription("Error in channel 18");
			ERR17::SetName("ERR17"); ERR17::SetDescription("Error in channel 17");
			ERR16::SetName("ERR16"); ERR16::SetDescription("Error in channel 16");
			ERR15::SetName("ERR15"); ERR15::SetDescription("Error in channel 15");
			ERR14::SetName("ERR14"); ERR14::SetDescription("Error in channel 14");
			ERR13::SetName("ERR13"); ERR13::SetDescription("Error in channel 13");
			ERR12::SetName("ERR12"); ERR12::SetDescription("Error in channel 12");
			ERR11::SetName("ERR11"); ERR11::SetDescription("Error in channel 11");
			ERR10::SetName("ERR10"); ERR10::SetDescription("Error in channel 10");
			ERR9 ::SetName("ERR9 "); ERR9 ::SetDescription("Error in channel 9");
			ERR8 ::SetName("ERR8 "); ERR8 ::SetDescription("Error in channel 8");
			ERR7 ::SetName("ERR7 "); ERR7 ::SetDescription("Error in channel 7");
			ERR6 ::SetName("ERR6 "); ERR6 ::SetDescription("Error in channel 6");
			ERR5 ::SetName("ERR5 "); ERR5 ::SetDescription("Error in channel 5");
			ERR4 ::SetName("ERR4 "); ERR4 ::SetDescription("Error in channel 4");
			ERR3 ::SetName("ERR3 "); ERR3 ::SetDescription("Error in channel 3");
			ERR2 ::SetName("ERR2 "); ERR2 ::SetDescription("Error in channel 2");
			ERR1 ::SetName("ERR1 "); ERR1 ::SetDescription("Error in channel 1");
			ERR0 ::SetName("ERR0 "); ERR0 ::SetDescription("Error in channel 0");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
			this->edma->UpdateErrors();
		}

		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->edma->UpdateVLD();
				this->edma->UpdateErrors();
			}
			
			return rws;
		}

		using Super::operator =;
	};

	// Hardware Request Status Register High (eDMA_HRSH)
	struct EDMA_HRSH : EDMA_Register<EDMA_HRSH, 32, SW_RW>
	{
		typedef EDMA_Register<EDMA_HRSH, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x30;

		struct HRS63 : Field<EDMA_HRSH, HRS63, 0>  {}; // Hardware request status channel 63
		struct HRS62 : Field<EDMA_HRSH, HRS62, 1>  {}; // Hardware request status channel 62
		struct HRS61 : Field<EDMA_HRSH, HRS61, 2>  {}; // Hardware request status channel 61
		struct HRS60 : Field<EDMA_HRSH, HRS60, 3>  {}; // Hardware request status channel 60
		struct HRS59 : Field<EDMA_HRSH, HRS59, 4>  {}; // Hardware request status channel 59
		struct HRS58 : Field<EDMA_HRSH, HRS58, 5>  {}; // Hardware request status channel 58
		struct HRS57 : Field<EDMA_HRSH, HRS57, 6>  {}; // Hardware request status channel 57
		struct HRS56 : Field<EDMA_HRSH, HRS56, 7>  {}; // Hardware request status channel 56
		struct HRS55 : Field<EDMA_HRSH, HRS55, 8>  {}; // Hardware request status channel 55
		struct HRS54 : Field<EDMA_HRSH, HRS54, 9>  {}; // Hardware request status channel 54
		struct HRS53 : Field<EDMA_HRSH, HRS53, 10> {}; // Hardware request status channel 53
		struct HRS52 : Field<EDMA_HRSH, HRS52, 11> {}; // Hardware request status channel 52
		struct HRS51 : Field<EDMA_HRSH, HRS51, 12> {}; // Hardware request status channel 51
		struct HRS50 : Field<EDMA_HRSH, HRS50, 13> {}; // Hardware request status channel 50
		struct HRS49 : Field<EDMA_HRSH, HRS49, 14> {}; // Hardware request status channel 49
		struct HRS48 : Field<EDMA_HRSH, HRS48, 15> {}; // Hardware request status channel 48
		struct HRS47 : Field<EDMA_HRSH, HRS47, 16> {}; // Hardware request status channel 47
		struct HRS46 : Field<EDMA_HRSH, HRS46, 17> {}; // Hardware request status channel 46
		struct HRS45 : Field<EDMA_HRSH, HRS45, 18> {}; // Hardware request status channel 45
		struct HRS44 : Field<EDMA_HRSH, HRS44, 19> {}; // Hardware request status channel 44
		struct HRS43 : Field<EDMA_HRSH, HRS43, 20> {}; // Hardware request status channel 43
		struct HRS42 : Field<EDMA_HRSH, HRS42, 21> {}; // Hardware request status channel 42
		struct HRS41 : Field<EDMA_HRSH, HRS41, 22> {}; // Hardware request status channel 41
		struct HRS40 : Field<EDMA_HRSH, HRS40, 23> {}; // Hardware request status channel 40
		struct HRS39 : Field<EDMA_HRSH, HRS39, 24> {}; // Hardware request status channel 39
		struct HRS38 : Field<EDMA_HRSH, HRS38, 25> {}; // Hardware request status channel 38
		struct HRS37 : Field<EDMA_HRSH, HRS37, 26> {}; // Hardware request status channel 37
		struct HRS36 : Field<EDMA_HRSH, HRS36, 27> {}; // Hardware request status channel 36
		struct HRS35 : Field<EDMA_HRSH, HRS35, 28> {}; // Hardware request status channel 35
		struct HRS34 : Field<EDMA_HRSH, HRS34, 29> {}; // Hardware request status channel 34
		struct HRS33 : Field<EDMA_HRSH, HRS33, 30> {}; // Hardware request status channel 33
		struct HRS32 : Field<EDMA_HRSH, HRS32, 31> {}; // Hardware request status channel 32

		typedef FieldSet< HRS63, HRS62, HRS61, HRS60, HRS59, HRS58, HRS57, HRS56
		                , HRS55, HRS54, HRS53, HRS52, HRS51, HRS50, HRS49, HRS48
		                , HRS47, HRS46, HRS45, HRS44, HRS43, HRS42, HRS41, HRS40
		                , HRS39, HRS38, HRS37, HRS36, HRS35, HRS34, HRS33, HRS32> ALL;
		
		EDMA_HRSH(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_HRSH(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value) { Init(); }
		
		void Init()
		{
			this->SetName("eDMA_HRSH"); this->SetDescription("Hardware Request Status Register High");
			
			HRS63::SetName("HRS63"); HRS63::SetDescription("Hardware request status channel 63");
			HRS62::SetName("HRS62"); HRS62::SetDescription("Hardware request status channel 62");
			HRS61::SetName("HRS61"); HRS61::SetDescription("Hardware request status channel 61");
			HRS60::SetName("HRS60"); HRS60::SetDescription("Hardware request status channel 60");
			HRS59::SetName("HRS59"); HRS59::SetDescription("Hardware request status channel 59");
			HRS58::SetName("HRS58"); HRS58::SetDescription("Hardware request status channel 58");
			HRS57::SetName("HRS57"); HRS57::SetDescription("Hardware request status channel 57");
			HRS56::SetName("HRS56"); HRS56::SetDescription("Hardware request status channel 56");
			HRS55::SetName("HRS55"); HRS55::SetDescription("Hardware request status channel 55");
			HRS54::SetName("HRS54"); HRS54::SetDescription("Hardware request status channel 54");
			HRS53::SetName("HRS53"); HRS53::SetDescription("Hardware request status channel 53");
			HRS52::SetName("HRS52"); HRS52::SetDescription("Hardware request status channel 52");
			HRS51::SetName("HRS51"); HRS51::SetDescription("Hardware request status channel 51");
			HRS50::SetName("HRS50"); HRS50::SetDescription("Hardware request status channel 50");
			HRS49::SetName("HRS49"); HRS49::SetDescription("Hardware request status channel 49");
			HRS48::SetName("HRS48"); HRS48::SetDescription("Hardware request status channel 48");
			HRS47::SetName("HRS47"); HRS47::SetDescription("Hardware request status channel 47");
			HRS46::SetName("HRS46"); HRS46::SetDescription("Hardware request status channel 46");
			HRS45::SetName("HRS45"); HRS45::SetDescription("Hardware request status channel 45");
			HRS44::SetName("HRS44"); HRS44::SetDescription("Hardware request status channel 44");
			HRS43::SetName("HRS43"); HRS43::SetDescription("Hardware request status channel 43");
			HRS42::SetName("HRS42"); HRS42::SetDescription("Hardware request status channel 42");
			HRS41::SetName("HRS41"); HRS41::SetDescription("Hardware request status channel 41");
			HRS40::SetName("HRS40"); HRS40::SetDescription("Hardware request status channel 40");
			HRS39::SetName("HRS39"); HRS39::SetDescription("Hardware request status channel 39");
			HRS38::SetName("HRS38"); HRS38::SetDescription("Hardware request status channel 38");
			HRS37::SetName("HRS37"); HRS37::SetDescription("Hardware request status channel 37");
			HRS36::SetName("HRS36"); HRS36::SetDescription("Hardware request status channel 36");
			HRS35::SetName("HRS35"); HRS35::SetDescription("Hardware request status channel 35");
			HRS34::SetName("HRS34"); HRS34::SetDescription("Hardware request status channel 34");
			HRS33::SetName("HRS33"); HRS33::SetDescription("Hardware request status channel 33");
			HRS32::SetName("HRS32"); HRS32::SetDescription("Hardware request status channel 32");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
// 		virtual ReadWriteStatus Read(uint32_t& value, const uint32_t& bit_enable)
// 		{
// 			return this->edma->HardwareRequestStatus(this->
// 		}

		using Super::operator =;
	};
	
	// Hardware Request Status Register Low (eDMA_HRSL)
	struct EDMA_HRSL : EDMA_Register<EDMA_HRSL, 32, SW_RW>
	{
		typedef EDMA_Register<EDMA_HRSL, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x34;
		
		struct HRS31 : Field<EDMA_HRSL, HRS31, 0>  {}; // Hardware request status channel 31
		struct HRS30 : Field<EDMA_HRSL, HRS30, 1>  {}; // Hardware request status channel 30
		struct HRS29 : Field<EDMA_HRSL, HRS29, 2>  {}; // Hardware request status channel 29
		struct HRS28 : Field<EDMA_HRSL, HRS28, 3>  {}; // Hardware request status channel 28
		struct HRS27 : Field<EDMA_HRSL, HRS27, 4>  {}; // Hardware request status channel 27
		struct HRS26 : Field<EDMA_HRSL, HRS26, 5>  {}; // Hardware request status channel 26
		struct HRS25 : Field<EDMA_HRSL, HRS25, 6>  {}; // Hardware request status channel 25
		struct HRS24 : Field<EDMA_HRSL, HRS24, 7>  {}; // Hardware request status channel 24
		struct HRS23 : Field<EDMA_HRSL, HRS23, 8>  {}; // Hardware request status channel 23
		struct HRS22 : Field<EDMA_HRSL, HRS22, 9>  {}; // Hardware request status channel 22
		struct HRS21 : Field<EDMA_HRSL, HRS21, 10> {}; // Hardware request status channel 21
		struct HRS20 : Field<EDMA_HRSL, HRS20, 11> {}; // Hardware request status channel 20
		struct HRS19 : Field<EDMA_HRSL, HRS19, 12> {}; // Hardware request status channel 19
		struct HRS18 : Field<EDMA_HRSL, HRS18, 13> {}; // Hardware request status channel 18
		struct HRS17 : Field<EDMA_HRSL, HRS17, 14> {}; // Hardware request status channel 17
		struct HRS16 : Field<EDMA_HRSL, HRS16, 15> {}; // Hardware request status channel 16
		struct HRS15 : Field<EDMA_HRSL, HRS15, 16> {}; // Hardware request status channel 15
		struct HRS14 : Field<EDMA_HRSL, HRS14, 17> {}; // Hardware request status channel 14
		struct HRS13 : Field<EDMA_HRSL, HRS13, 18> {}; // Hardware request status channel 13
		struct HRS12 : Field<EDMA_HRSL, HRS12, 19> {}; // Hardware request status channel 12
		struct HRS11 : Field<EDMA_HRSL, HRS11, 20> {}; // Hardware request status channel 11
		struct HRS10 : Field<EDMA_HRSL, HRS10, 21> {}; // Hardware request status channel 10
		struct HRS9  : Field<EDMA_HRSL, HRS9, 22>  {}; // Hardware request status channel 9
		struct HRS8  : Field<EDMA_HRSL, HRS8, 23>  {}; // Hardware request status channel 8
		struct HRS7  : Field<EDMA_HRSL, HRS7, 24>  {}; // Hardware request status channel 7
		struct HRS6  : Field<EDMA_HRSL, HRS6, 25>  {}; // Hardware request status channel 6
		struct HRS5  : Field<EDMA_HRSL, HRS5, 26>  {}; // Hardware request status channel 5
		struct HRS4  : Field<EDMA_HRSL, HRS4, 27>  {}; // Hardware request status channel 4
		struct HRS3  : Field<EDMA_HRSL, HRS3, 28>  {}; // Hardware request status channel 3
		struct HRS2  : Field<EDMA_HRSL, HRS2, 29>  {}; // Hardware request status channel 2
		struct HRS1  : Field<EDMA_HRSL, HRS1, 30>  {}; // Hardware request status channel 1
		struct HRS0  : Field<EDMA_HRSL, HRS0, 31>  {}; // Hardware request status channel 0

		typedef FieldSet< HRS31, HRS30, HRS29, HRS28, HRS27, HRS26, HRS25, HRS24
		                , HRS23, HRS22, HRS21, HRS20, HRS19, HRS18, HRS17, HRS16
		                , HRS15, HRS14, HRS13, HRS12, HRS11, HRS10, HRS9, HRS8
		                , HRS7, HRS6, HRS5, HRS4, HRS3, HRS2, HRS1, HRS0> ALL;
		
		EDMA_HRSL(EDMA<CONFIG> *_edma) : Super(_edma) { Init(); }
		EDMA_HRSL(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value) { Init(); }
		
		void Init()
		{
			this->SetName("eDMA_HRSL"); this->SetDescription("Hardware Request Status Register Low");
			
			HRS31::SetName("HRS31"); HRS31::SetDescription("Hardware request status channel 31");
			HRS30::SetName("HRS30"); HRS30::SetDescription("Hardware request status channel 30");
			HRS29::SetName("HRS29"); HRS29::SetDescription("Hardware request status channel 29");
			HRS28::SetName("HRS28"); HRS28::SetDescription("Hardware request status channel 28");
			HRS27::SetName("HRS27"); HRS27::SetDescription("Hardware request status channel 27");
			HRS26::SetName("HRS26"); HRS26::SetDescription("Hardware request status channel 26");
			HRS25::SetName("HRS25"); HRS25::SetDescription("Hardware request status channel 25");
			HRS24::SetName("HRS24"); HRS24::SetDescription("Hardware request status channel 24");
			HRS23::SetName("HRS23"); HRS23::SetDescription("Hardware request status channel 23");
			HRS22::SetName("HRS22"); HRS22::SetDescription("Hardware request status channel 22");
			HRS21::SetName("HRS21"); HRS21::SetDescription("Hardware request status channel 21");
			HRS20::SetName("HRS20"); HRS20::SetDescription("Hardware request status channel 20");
			HRS19::SetName("HRS19"); HRS19::SetDescription("Hardware request status channel 19");
			HRS18::SetName("HRS18"); HRS18::SetDescription("Hardware request status channel 18");
			HRS17::SetName("HRS17"); HRS17::SetDescription("Hardware request status channel 17");
			HRS16::SetName("HRS16"); HRS16::SetDescription("Hardware request status channel 16");
			HRS15::SetName("HRS15"); HRS15::SetDescription("Hardware request status channel 15");
			HRS14::SetName("HRS14"); HRS14::SetDescription("Hardware request status channel 14");
			HRS13::SetName("HRS13"); HRS13::SetDescription("Hardware request status channel 13");
			HRS12::SetName("HRS12"); HRS12::SetDescription("Hardware request status channel 12");
			HRS11::SetName("HRS11"); HRS11::SetDescription("Hardware request status channel 11");
			HRS10::SetName("HRS10"); HRS10::SetDescription("Hardware request status channel 10");
			HRS9 ::SetName("HRS9 "); HRS9 ::SetDescription("Hardware request status channel 9");
			HRS8 ::SetName("HRS8 "); HRS8 ::SetDescription("Hardware request status channel 8");
			HRS7 ::SetName("HRS7 "); HRS7 ::SetDescription("Hardware request status channel 7");
			HRS6 ::SetName("HRS6 "); HRS6 ::SetDescription("Hardware request status channel 6");
			HRS5 ::SetName("HRS5 "); HRS5 ::SetDescription("Hardware request status channel 5");
			HRS4 ::SetName("HRS4 "); HRS4 ::SetDescription("Hardware request status channel 4");
			HRS3 ::SetName("HRS3 "); HRS3 ::SetDescription("Hardware request status channel 3");
			HRS2 ::SetName("HRS2 "); HRS2 ::SetDescription("Hardware request status channel 2");
			HRS1 ::SetName("HRS1 "); HRS1 ::SetDescription("Hardware request status channel 1");
			HRS0 ::SetName("HRS0 "); HRS0 ::SetDescription("Hardware request status channel 0");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		using Super::operator =;
	};
	
	// Channel Priority Register (eDMA_DCHPRIn)
	struct EDMA_DCHPRI : EDMA_Register<EDMA_DCHPRI, 8, SW_RW>
	{
		typedef EDMA_Register<EDMA_DCHPRI, 8, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x100;
		
		struct ECP    : Field<EDMA_DCHPRI, ECP   , 0         > {}; // Enable channel preemption
		struct DPA    : Field<EDMA_DCHPRI, DPA   , 1         > {}; // Disable preempt ability
		struct GRPPRI : Field<EDMA_DCHPRI, GRPPRI, 2, 3, SW_R> {}; // Channel n current group priority
		struct CHPRI  : Field<EDMA_DCHPRI, CHPRI , 4, 7      > {}; // Channel n arbitration priority
		
		typedef FieldSet<ECP, DPA, GRPPRI, CHPRI> ALL;
		
		EDMA_DCHPRI() : Super(0), reg_num(0) {}
		EDMA_DCHPRI(EDMA<CONFIG> *_edma) : Super(_edma), reg_num(0) {}
		EDMA_DCHPRI(EDMA<CONFIG> *_edma, uint8_t value) : Super(_edma, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, EDMA<CONFIG> *_edma)
		{
			this->edma = _edma;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "eDMA_DCHPRI" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "Channel Priority Register #" << reg_num;
			this->SetDescription(description_sstr.str());
			
			ECP   ::SetName("ECP");    ECP   ::SetDescription("Enable channel preemption");
			DPA   ::SetName("DPA");    DPA   ::SetDescription("Disable preempt ability");
			GRPPRI::SetName("GRPPRI"); GRPPRI::SetDescription("Channel n current group priority");
			CHPRI ::SetName("CHPRI");  CHPRI ::SetDescription("Channel n arbitration priority");
		}
		
		void Reset()
		{
			this->template Set<ECP>(0);
			this->template Set<DPA>(0);
			this->template Set<GRPPRI>(reg_num / NUM_CHANNELS_PER_GROUP);
			this->template Set<CHPRI>(reg_num % NUM_CHANNELS_PER_GROUP);
			this->edma->UpdateChannelPriority(reg_num);
		}
		
		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint8_t& value, const uint8_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->edma->UpdateChannelPriority(reg_num);
			}
			
			return rws;
		}

		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};
	
	// Channel Master ID Register (eDMA_DCHMIDn)
	struct EDMA_DCHMID : EDMA_Register<EDMA_DCHMID, 8, SW_RW>
	{
		typedef EDMA_Register<EDMA_DCHMID, 8, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x140;
		
		struct EMI : Field<EDMA_DCHMID, EMI, 0, 0, SW_RW> {}; // Enable MasterID replication
		struct PAL : Field<EDMA_DCHMID, PAL, 1, 1, SW_R > {}; // Privileged Access Level
		struct MID : Field<EDMA_DCHMID, MID, 4, 7, SW_R > {}; // Master ID
		
		typedef FieldSet<EMI, PAL, MID> ALL;
		
		EDMA_DCHMID() : Super(0), reg_num(0) {}
		EDMA_DCHMID(EDMA<CONFIG> *_edma) : Super(_edma), reg_num(0) {}
		EDMA_DCHMID(EDMA<CONFIG> *_edma, uint8_t value) : Super(_edma, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, EDMA<CONFIG> *_edma)
		{
			this->edma = _edma;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "eDMA_DCHMID" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "Channel Master ID Register #" << reg_num;
			this->SetDescription(description_sstr.str());
			
			EMI::SetName("EMI"); EMI::SetDescription("Enable MasterID replication");
			PAL::SetName("PAL"); PAL::SetDescription("Privileged Access Level");
			MID::SetName("MID"); MID::SetDescription("Master ID");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};
	
	// TCD Source Address (eDMA_TCDn_SADDR)
	struct EDMA_TCD_SADDR : EDMA_Register<EDMA_TCD_SADDR, 32, SW_RW>
	{
		typedef EDMA_Register<EDMA_TCD_SADDR, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;
		
		struct SADDR : Field<EDMA_TCD_SADDR, SADDR, 0, 31> {}; // Source address
		
		typedef FieldSet<SADDR> ALL;
		
		EDMA_TCD_SADDR() : Super(0), reg_num(0) {}
		EDMA_TCD_SADDR(EDMA<CONFIG> *_edma) : Super(_edma), reg_num(0) {}
		EDMA_TCD_SADDR(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, EDMA<CONFIG> *_edma)
		{
			this->edma = _edma;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "eDMA_TCD" << reg_num << "_SADDR";
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "TCD #" << reg_num << " Source Address";
			this->SetDescription(description_sstr.str());
			
			SADDR::SetName("SADDR"); SADDR::SetDescription("Source address");
		}
		
		void Reset()
		{
			// undefined
		}

		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};
	
	// TCD Transfer Attributes (eDMA_TCDn_ATTR)
	struct EDMA_TCD_ATTR : EDMA_Register<EDMA_TCD_ATTR, 16, SW_RW>
	{
		typedef EDMA_Register<EDMA_TCD_ATTR, 16, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x4;
		
		struct SMOD  : Field<EDMA_TCD_ATTR, SMOD ,  0, 4 > {}; // Source address modulo
		struct SSIZE : Field<EDMA_TCD_ATTR, SSIZE,  5, 7 > {}; // Source data transfer size
		struct DMOD  : Field<EDMA_TCD_ATTR, DMOD ,  8, 12> {}; // Destination address modulo
		struct DSIZE : Field<EDMA_TCD_ATTR, DSIZE, 13, 15> {}; // Destination data transfer size
		
		typedef FieldSet<SMOD, SSIZE, DMOD, DSIZE> ALL;
		
		EDMA_TCD_ATTR() : Super(0), reg_num(0) {}
		EDMA_TCD_ATTR(EDMA<CONFIG> *_edma) : Super(_edma), reg_num(0) {}
		EDMA_TCD_ATTR(EDMA<CONFIG> *_edma, uint16_t value) : Super(_edma, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, EDMA<CONFIG> *_edma)
		{
			this->edma = _edma;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "eDMA_TCD" << reg_num << "_ATTR";
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "TCD #" << reg_num << " Transfer Attributes";
			this->SetDescription(description_sstr.str());
			
			SMOD ::SetName("SMOD");  SMOD ::SetDescription("Source address modulo");
			SSIZE::SetName("SSIZE"); SSIZE::SetDescription("Source data transfer size");
			DMOD ::SetName("DMOD");  DMOD ::SetDescription("Destination address modulo");
			DSIZE::SetName("DSIZE"); DSIZE::SetDescription("Destination data transfer size");
		}
		
		void Reset()
		{
			// undefined
		}
		
		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};
	
	// TCD Signed Source Address Offset (eDMA_TCDn_SOFF)
	struct EDMA_TCD_SOFF : EDMA_Register<EDMA_TCD_SOFF, 16, SW_RW>
	{
		typedef EDMA_Register<EDMA_TCD_SOFF, 16, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x6;
		
		struct SOFF : Field<EDMA_TCD_SOFF, SOFF, 0, 15> {}; // Source address signed offset
		
		typedef FieldSet<SOFF> ALL;
		
		EDMA_TCD_SOFF() : Super(0), reg_num(0) {}
		EDMA_TCD_SOFF(EDMA<CONFIG> *_edma) : Super(_edma), reg_num(0) {}
		EDMA_TCD_SOFF(EDMA<CONFIG> *_edma, uint16_t value) : Super(_edma, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, EDMA<CONFIG> *_edma)
		{
			this->edma = _edma;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "eDMA_TCD" << reg_num << "_SOFF";
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "TCD #" << reg_num << " Signed Source Address Offset";
			this->SetDescription(description_sstr.str());
			
			SOFF::SetName("SOFF"); SOFF::SetDescription("TCD Signed Source Address Offset");
		}
		
		void Reset()
		{
			// undefined
		}

		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};
	
	// TCD BYTES (eDMA_TCDn_NBYTES_MLNO/eDMA_TCDn_NBYTES_MLOFFNO/eDMA_TCDn_NBYTES_MLOFFYES)
	struct EDMA_TCD_NBYTES : EDMA_Register<EDMA_TCD_NBYTES, 32, SW_RW>
	{
		typedef EDMA_Register<EDMA_TCD_NBYTES, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x8;
		
		struct ALL : Field<EDMA_TCD_NBYTES, ALL, 0, 31> {};
		
		struct MLNO
		{
			struct NBYTES : Field<EDMA_TCD_NBYTES, NBYTES, 0, 31> {}; // Minor byte transfer count
		};
		
		struct MLOFFNO
		{
			struct SMLOE  : Field<EDMA_TCD_NBYTES, SMLOE , 0    > {}; // Source minor loop offset enable
			struct DMLOE  : Field<EDMA_TCD_NBYTES, DMLOE , 1    > {}; // Destination minor loop offset enable
			struct NBYTES : Field<EDMA_TCD_NBYTES, NBYTES, 2, 31> {}; // Minor byte transfer count
		};
		
		struct MLOFFYES
		{
			struct SMLOE  : Field<EDMA_TCD_NBYTES, SMLOE , 0    > {};  // Source minor loop offset enable
			struct DMLOE  : Field<EDMA_TCD_NBYTES, DMLOE , 1    > {};  // Destination minor loop offset enable
			struct MLOFF  : Field<EDMA_TCD_NBYTES, MLOFF , 2, 21 > {}; // Minor loop offset
			struct NBYTES : Field<EDMA_TCD_NBYTES, NBYTES, 22, 31> {}; // Minor byte transfer count
		};
		
		EDMA_TCD_NBYTES() : Super(0), reg_num(0) {}
		EDMA_TCD_NBYTES(EDMA<CONFIG> *_edma) : Super(_edma), reg_num(0) {}
		EDMA_TCD_NBYTES(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, EDMA<CONFIG> *_edma)
		{
			this->edma = _edma;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "eDMA_TCD" << reg_num << "_NBYTES";
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "TCD #" << reg_num << " Minor byte transfert count / Minor loop offset";
			this->SetDescription(description_sstr.str());
		}
		
		void Reset()
		{
			// undefined
		}

		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};
	
	// TCD Last Source Address Adjustment (eDMA_TCDn_SLAST)
	struct EDMA_TCD_SLAST : EDMA_Register<EDMA_TCD_SLAST, 32, SW_RW>
	{
		typedef EDMA_Register<EDMA_TCD_SLAST, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xc;
		
		struct SLAST : Field<EDMA_TCD_SLAST, SLAST, 0, 31> {}; // Last source address adjustment
		
		typedef FieldSet<SLAST> ALL;
		
		EDMA_TCD_SLAST() : Super(0), reg_num(0) {}
		EDMA_TCD_SLAST(EDMA<CONFIG> *_edma) : Super(_edma), reg_num(0) {}
		EDMA_TCD_SLAST(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, EDMA<CONFIG> *_edma)
		{
			this->edma = _edma;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "eDMA_TCD" << reg_num << "_SLAST";
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "TCD #" << reg_num << " Last Source Address Adjustment";
			this->SetDescription(description_sstr.str());
			
			SLAST::SetName("SLAST"); SLAST::SetDescription("Last source address adjustment");
		}
		
		void Reset()
		{
			// undefined
		}

		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};
	
	// TCD Destination Address (eDMA_TCDn_DADDR)
	struct EDMA_TCD_DADDR : EDMA_Register<EDMA_TCD_DADDR, 32, SW_RW>
	{
		typedef EDMA_Register<EDMA_TCD_DADDR, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x10;
		
		struct DADDR : Field<EDMA_TCD_DADDR, DADDR, 0, 31> {}; // Destination address
		
		typedef FieldSet<DADDR> ALL;
		
		EDMA_TCD_DADDR() : Super(0), reg_num(0) {}
		EDMA_TCD_DADDR(EDMA<CONFIG> *_edma) : Super(_edma), reg_num(0) {}
		EDMA_TCD_DADDR(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, EDMA<CONFIG> *_edma)
		{
			this->edma = _edma;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "eDMA_TCD" << reg_num << "_DADDR";
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "TCD #" << reg_num << " Destination Address";
			this->SetDescription(description_sstr.str());
			
			DADDR::SetName("DADDR"); DADDR::SetDescription("Destination address");
		}
		
		void Reset()
		{
			// undefined
		}

		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};
	
	// TCD Current Minor Loop Link (eDMA_TCDn_CITER_ELINKYES/eDMA_TCDn_CITER_ELINKNO)
	struct EDMA_TCD_CITER : EDMA_Register<EDMA_TCD_CITER, 16, SW_RW>
	{
		typedef EDMA_Register<EDMA_TCD_CITER, 16, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x14;
		
		struct ELINKYES
		{
			struct ELINK  : Field<EDMA_TCD_CITER, ELINK , 0>     {}; // Enable channel-to-channel linking on minor-loop complete
			struct LINKCH : Field<EDMA_TCD_CITER, LINKCH, 1, 6 > {}; // Link channel number
			struct CITER  : Field<EDMA_TCD_CITER, CITER , 7, 15> {}; // Current major iterator count
		};
		
		struct ELINKNO
		{
			struct ELINK  : Field<EDMA_TCD_CITER, ELINK, 0>       {}; // Enable channel-to-channel linking on minor-loop complete
			struct CITER  : Field<EDMA_TCD_CITER, CITER , 1, 15>  {}; // Current major iterator count
		};
		
		struct ALL : Field<EDMA_TCD_CITER, ALL, 0, 15> {};
		
		EDMA_TCD_CITER() : Super(0), reg_num(0) {}
		EDMA_TCD_CITER(EDMA<CONFIG> *_edma) : Super(_edma), reg_num(0) {}
		EDMA_TCD_CITER(EDMA<CONFIG> *_edma, uint16_t value) : Super(_edma, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, EDMA<CONFIG> *_edma)
		{
			this->edma = _edma;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "eDMA_TCD" << reg_num << "_CITER";
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "TCD #" << reg_num << " Current Minor Loop Link";
			this->SetDescription(description_sstr.str());
		}
		
		void Reset()
		{
			// undefined
		}

		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};

	// TCD Signed Destination Address Offset (eDMA_TCDn_DOFF)
	struct EDMA_TCD_DOFF : EDMA_Register<EDMA_TCD_DOFF, 16, SW_RW>
	{
		typedef EDMA_Register<EDMA_TCD_DOFF, 16, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x16;
		
		struct DOFF : Field<EDMA_TCD_DOFF, DOFF, 0, 15> {}; // Destination address signed offset
		
		typedef FieldSet<DOFF> ALL;
		
		EDMA_TCD_DOFF() : Super(0), reg_num(0) {}
		EDMA_TCD_DOFF(EDMA<CONFIG> *_edma) : Super(_edma), reg_num(0) {}
		EDMA_TCD_DOFF(EDMA<CONFIG> *_edma, uint16_t value) : Super(_edma, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, EDMA<CONFIG> *_edma)
		{
			this->edma = _edma;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "eDMA_TCD" << reg_num << "_DOFF";
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "TCD #" << reg_num << " Signed Destination Address Offset";
			this->SetDescription(description_sstr.str());
			
			DOFF::SetName("DOFF"); DOFF::SetDescription("Destination address signed offset");
		}
		
		void Reset()
		{
			// undefined
		}

		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};
	
	// TCD Last Destination Address Adjustment/Scatter Gather Address (eDMA_TCDn_DLASTSGA)
	struct EDMA_TCD_DLASTSGA : EDMA_Register<EDMA_TCD_DLASTSGA, 32, SW_RW>
	{
		typedef EDMA_Register<EDMA_TCD_DLASTSGA, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x18;
		
		struct DLASTSGA : Field<EDMA_TCD_DLASTSGA, DLASTSGA, 0, 31> {}; // Destination last address adjustment or the memory address for the next transfer control descriptor to be loaded into this channel (scatter/gather)
		
		typedef FieldSet<DLASTSGA> ALL;
		
		EDMA_TCD_DLASTSGA() : Super(0), reg_num(0) {}
		EDMA_TCD_DLASTSGA(EDMA<CONFIG> *_edma) : Super(_edma), reg_num(0) {}
		EDMA_TCD_DLASTSGA(EDMA<CONFIG> *_edma, uint32_t value) : Super(_edma, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, EDMA<CONFIG> *_edma)
		{
			this->edma = _edma;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "eDMA_TCD" << reg_num << "_DLASTSGA";
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "TCD #" << reg_num << " Last Destination Address Adjustment/Scatter Gather Address";
			this->SetDescription(description_sstr.str());
			
			DLASTSGA::SetName("DLASTSGA"); DLASTSGA::SetDescription("Destination last address adjustment or the memory address for the next transfer control descriptor to be loaded into this channel (scatter/gather)");
		}
		
		void Reset()
		{
			// undefined
		}

		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};
	
	// TCD Beginning Minor Loop Link (eDMA_TCDn_BITER_ELINKYES/eDMA_TCDn_BITER_ELINKNO)
	struct EDMA_TCD_BITER : EDMA_Register<EDMA_TCD_BITER, 16, SW_RW>
	{
		typedef EDMA_Register<EDMA_TCD_BITER, 16, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x1c;
		
		struct ELINKYES
		{
			struct ELINK  : Field<EDMA_TCD_BITER, ELINK , 0>    {}; // Enables channel-to-channel linking on minor loop complete
			struct LINKCH : Field<EDMA_TCD_BITER, LINKCH, 1, 6> {}; // Link channel number
			struct BITER  : Field<EDMA_TCD_BITER, BITER, 7, 15> {}; // Starting major iteration count
		};
		
		struct ELINKNO
		{
			struct ELINK  : Field<EDMA_TCD_BITER, ELINK, 0>     {}; // Enables channel-to-channel linking on minor loop complete
			struct BITER  : Field<EDMA_TCD_BITER, BITER, 1, 15> {}; // Starting major iteration count
		};
		
		struct ALL : Field<EDMA_TCD_BITER, ALL, 0, 15> {};
		
		EDMA_TCD_BITER() : Super(0), reg_num(0) {}
		EDMA_TCD_BITER(EDMA<CONFIG> *_edma) : Super(_edma), reg_num(0) {}
		EDMA_TCD_BITER(EDMA<CONFIG> *_edma, uint16_t value) : Super(_edma, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, EDMA<CONFIG> *_edma)
		{
			this->edma = _edma;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "eDMA_TCD" << reg_num << "_BITER";
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "TCD #" << reg_num << " Beginning Minor Loop Link";
			this->SetDescription(description_sstr.str());
		}
		
		void Reset()
		{
			// undefined
		}

		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};

	// TCD Control and Status (eDMA_TCDn_CSR)
	struct EDMA_TCD_CSR : EDMA_Register<EDMA_TCD_CSR, 16, SW_RW>
	{
		typedef EDMA_Register<EDMA_TCD_CSR, 16, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x1e;
		
		struct BWC         : Field<EDMA_TCD_CSR, BWC        , 0, 1> {}; // Bandwidth control
		struct MAJORLINKCH : Field<EDMA_TCD_CSR, MAJORLINKCH, 2, 7> {}; // Link channel number
		struct DONE        : Field<EDMA_TCD_CSR, DONE       , 8   > {}; // Channel done
		struct ACTIVE      : Field<EDMA_TCD_CSR, ACTIVE     , 9   > {}; // Channel active
		struct MAJORELINK  : Field<EDMA_TCD_CSR, MAJORELINK , 10  > {}; // Enable channel-to-channel linking on major loop complete
		struct ESG         : Field<EDMA_TCD_CSR, ESG        , 11  > {}; // Enable scatter/gather processing
		struct DREQ        : Field<EDMA_TCD_CSR, DREQ       , 12  > {}; // Disable request {H,L}
		struct INTHALF     : Field<EDMA_TCD_CSR, INTHALF    , 13  > {}; // Enable an interrupt when major counter is half complete
		struct INTMAJOR    : Field<EDMA_TCD_CSR, INTMAJOR   , 14  > {}; // Enable an interrupt when major iteration count completes
		struct START       : Field<EDMA_TCD_CSR, START      , 15  > {}; // Channel start
		
		typedef FieldSet<BWC, MAJORLINKCH, DONE, ACTIVE, MAJORELINK, ESG, DREQ, INTHALF, INTMAJOR, START> ALL;
		
		EDMA_TCD_CSR() : Super(0), reg_num(0) {}
		EDMA_TCD_CSR(EDMA<CONFIG> *_edma) : Super(_edma), reg_num(0) {}
		EDMA_TCD_CSR(EDMA<CONFIG> *_edma, uint16_t value) : Super(_edma, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, EDMA<CONFIG> *_edma)
		{
			this->edma = _edma;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "eDMA_TCD" << reg_num << "_CSR";
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "TCD #" << reg_num << " Control and Status";
			this->SetDescription(description_sstr.str());
			
			BWC        ::SetName("BWC");         BWC        ::SetDescription("Bandwidth control");
			MAJORLINKCH::SetName("MAJORLINKCH"); MAJORLINKCH::SetDescription("Link channel number");
			DONE       ::SetName("DONE");        DONE       ::SetDescription("Channel done");
			ACTIVE     ::SetName("ACTIVE");      ACTIVE     ::SetDescription("Channel active");
			MAJORELINK ::SetName("MAJORELINK");  MAJORELINK ::SetDescription("Enable channel-to-channel linking on major loop complete");
			ESG        ::SetName("ESG");         ESG        ::SetDescription("Enable scatter/gather processing");
			DREQ       ::SetName("DREQ");        DREQ       ::SetDescription("Disable request {H,L}");
			INTHALF    ::SetName("INTHALF");     INTHALF    ::SetDescription("Enable an interrupt when major counter is half complete");
			INTMAJOR   ::SetName("INTMAJOR");    INTMAJOR   ::SetDescription("Enable an interrupt when major iteration count completes");
			START      ::SetName("START");       START      ::SetDescription("Channel start");
		}
		
		void Reset()
		{
			// undefined
		}

		virtual ReadWriteStatus Write(tlm_trans_attr& trans_attr, const uint16_t& value, const uint16_t& bit_enable)
		{
			bool old_start = this->template Get<START>();
			ReadWriteStatus rws = Super::Write(trans_attr, value, bit_enable);
			bool new_start = this->template Get<START>();
			
			if(!IsReadWriteError(rws))
			{
				// Capture Master ID and Privilege Access Level used to write CSR
				this->edma->SetMasterID(reg_num, trans_attr.master_id());
				this->edma->SetPrivilegeAccessLevel(reg_num, trans_attr.is_privileged() ? PAL_PRIVILEGED_PROTECTION_LEVEL : PAL_USER_PROTECTION_LEVEL);
				
				if(!old_start && new_start)
				{
					this->edma->NotifyEngine();
				}
			}
			
			return rws;
		}

		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};
	
	struct EDMA_TCD
	{
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x1000;
		static const unsigned int SIZE = 32;
		
		EDMA_TCD()
			: edma(0)
			, edma_tcd_saddr   ()
			, edma_tcd_attr    ()
			, edma_tcd_soff    ()
			, edma_tcd_nbytes  ()
			, edma_tcd_slast   ()
			, edma_tcd_daddr   ()
			, edma_tcd_citer   ()
			, edma_tcd_doff    ()
			, edma_tcd_dlastsga()
			, edma_tcd_biter   ()
			, edma_tcd_csr     ()
		{
		}
		
		EDMA_TCD(unsigned int _dma_channel_num, EDMA<CONFIG> *_edma)
			: edma(_edma)
			, dma_channel_num(_dma_channel_num)
			, edma_tcd_saddr()
			, edma_tcd_attr()
			, edma_tcd_soff()
			, edma_tcd_nbytes()
			, edma_tcd_slast()
			, edma_tcd_daddr()
			, edma_tcd_citer()
			, edma_tcd_doff()
			, edma_tcd_dlastsga()
			, edma_tcd_biter()
			, edma_tcd_csr()
		{
			edma_tcd_saddr   .WithinRegisterFileCtor(dma_channel_num, edma);
			edma_tcd_attr    .WithinRegisterFileCtor(dma_channel_num, edma);
			edma_tcd_soff    .WithinRegisterFileCtor(dma_channel_num, edma);
			edma_tcd_nbytes  .WithinRegisterFileCtor(dma_channel_num, edma);
			edma_tcd_slast   .WithinRegisterFileCtor(dma_channel_num, edma);
			edma_tcd_daddr   .WithinRegisterFileCtor(dma_channel_num, edma);
			edma_tcd_citer   .WithinRegisterFileCtor(dma_channel_num, edma);
			edma_tcd_doff    .WithinRegisterFileCtor(dma_channel_num, edma);
			edma_tcd_dlastsga.WithinRegisterFileCtor(dma_channel_num, edma);
			edma_tcd_biter   .WithinRegisterFileCtor(dma_channel_num, edma);
			edma_tcd_csr     .WithinRegisterFileCtor(dma_channel_num, edma);
		}
		
		EDMA_TCD& operator = (const EDMA_TCD& edma_tcd)
		{
			edma              = edma_tcd.edma;
			dma_channel_num   = edma_tcd.dma_channel_num;
			edma_tcd_saddr    = edma_tcd.edma_tcd_saddr;
			edma_tcd_attr     = edma_tcd.edma_tcd_attr;
			edma_tcd_soff     = edma_tcd.edma_tcd_soff;
			edma_tcd_nbytes   = edma_tcd.edma_tcd_nbytes;
			edma_tcd_slast    = edma_tcd.edma_tcd_slast;
			edma_tcd_daddr    = edma_tcd.edma_tcd_daddr;
			edma_tcd_citer    = edma_tcd.edma_tcd_citer;
			edma_tcd_doff     = edma_tcd.edma_tcd_doff;
			edma_tcd_dlastsga = edma_tcd.edma_tcd_dlastsga;
			edma_tcd_biter    = edma_tcd.edma_tcd_biter;
			edma_tcd_csr      = edma_tcd.edma_tcd_csr;
			return *this;
		}
		
		void Reset()
		{
			edma_tcd_saddr   .Reset();
			edma_tcd_attr    .Reset();
			edma_tcd_soff    .Reset();
			edma_tcd_nbytes  .Reset();
			edma_tcd_slast   .Reset();
			edma_tcd_daddr   .Reset();
			edma_tcd_citer   .Reset();
			edma_tcd_doff    .Reset();
			edma_tcd_dlastsga.Reset();
			edma_tcd_biter   .Reset();
			edma_tcd_csr     .Reset();
		}
		
		uint64_t GetNBYTES() const
		{
			// Note: An NBYTES value of 0x0000_0000 is interpreted as a 4 GB transfer.
			
			if(edma->edma_cr.template Get<typename EDMA_CR::EMLM>()) // minor loop link mode ?
			{
				if(edma_tcd_nbytes.template Get<typename EDMA_TCD_NBYTES::MLOFFNO::SMLOE>() || edma_tcd_nbytes.template Get<typename EDMA_TCD_NBYTES::MLOFFNO::DMLOE>())
				{
					uint32_t nbytes = edma_tcd_nbytes.template Get<typename EDMA_TCD_NBYTES::MLOFFYES::NBYTES>();
					return nbytes ? nbytes : (uint64_t(1) << 32);
				}
				else
				{
					uint32_t nbytes = edma_tcd_nbytes.template Get<typename EDMA_TCD_NBYTES::MLOFFNO::NBYTES>();
					return nbytes ? nbytes : (uint64_t(1) << 32);
				}
			}
			else
			{
				uint32_t nbytes = edma_tcd_nbytes.template Get<typename EDMA_TCD_NBYTES::MLNO::NBYTES>();
				return nbytes ? nbytes : (uint64_t(1) << 32);
			}
		}
		
		void SetNBYTES(uint32_t v)
		{
			if(edma->edma_cr.template Get<typename EDMA_CR::EMLM>()) // minor loop link mode ?
			{
				if(edma_tcd_nbytes.template Get<typename EDMA_TCD_NBYTES::MLOFFNO::SMLOE>() || edma_tcd_nbytes.template Get<typename EDMA_TCD_NBYTES::MLOFFNO::DMLOE>())
				{
					edma_tcd_nbytes.template Set<typename EDMA_TCD_NBYTES::MLOFFYES::NBYTES>(v);
				}
				else
				{
					edma_tcd_nbytes.template Set<typename EDMA_TCD_NBYTES::MLOFFNO::NBYTES>(v);
				}
			}
			else
			{
				edma_tcd_nbytes.template Set<typename EDMA_TCD_NBYTES::MLNO::NBYTES>(v);
			}
		}
		
		uint32_t GetCITER() const
		{
			if(edma_tcd_citer.template Get<typename EDMA_TCD_CITER::ELINKNO::ELINK>()) //  channel-to-channel linking on minor-loop complete ?
			{
				return edma_tcd_citer.template Get<typename EDMA_TCD_CITER::ELINKYES::CITER>();
			}
			else
			{
				return edma_tcd_citer.template Get<typename EDMA_TCD_CITER::ELINKNO::CITER>();
			}
		}
		
		void SetCITER(uint32_t v)
		{
			if(edma_tcd_citer.template Get<typename EDMA_TCD_CITER::ELINKNO::ELINK>()) //  channel-to-channel linking on minor-loop complete ?
			{
				edma_tcd_citer.template Set<typename EDMA_TCD_CITER::ELINKYES::CITER>(v);
			}
			else
			{
				edma_tcd_citer.template Set<typename EDMA_TCD_CITER::ELINKNO::CITER>(v);
			}
		}

		uint32_t GetBITER() const
		{
			if(edma_tcd_biter.template Get<typename EDMA_TCD_BITER::ELINKNO::ELINK>()) //  channel-to-channel linking on minor-loop complete ?
			{
				return edma_tcd_biter.template Get<typename EDMA_TCD_BITER::ELINKYES::BITER>();
			}
			else
			{
				return edma_tcd_biter.template Get<typename EDMA_TCD_BITER::ELINKNO::BITER>();
			}
		}
		
		bool CheckSourceDataTransferSize() const
		{
			switch(edma_tcd_attr.template Get<typename EDMA_TCD_ATTR::SSIZE>())
			{
				case 0: case 1: case 2: case 4: return true;
			}
			
			return false;
		}
		
		bool CheckDestinationDataTransferSize() const
		{
			switch(edma_tcd_attr.template Get<typename EDMA_TCD_ATTR::DSIZE>())
			{
				case 0: case 1: case 2: case 4: return true;
			}
			
			return false;
		}

		unsigned int GetSourceDataTransferSize() const
		{
			return 1 << edma_tcd_attr.template Get<typename EDMA_TCD_ATTR::SSIZE>();
		}
		
		unsigned int GetDestinationDataTransferSize() const
		{
			return 1 << edma_tcd_attr.template Get<typename EDMA_TCD_ATTR::DSIZE>();
		}
		
		bool CheckSourceAddress() const
		{
			return CheckSourceDataTransferSize() && ((edma_tcd_saddr.template Get<typename EDMA_TCD_SADDR::SADDR>() & (GetSourceDataTransferSize() - 1)) == 0);
		}
		
		bool CheckSourceOffset() const
		{
			return CheckSourceDataTransferSize() && ((edma_tcd_soff.template Get<typename EDMA_TCD_SOFF::SOFF>() & (GetSourceDataTransferSize() - 1)) == 0);
		}

		bool CheckDestinationAddress() const
		{
			return GetDestinationDataTransferSize() && ((edma_tcd_daddr.template Get<typename EDMA_TCD_DADDR::DADDR>() & (GetDestinationDataTransferSize() - 1)) == 0);
		}

		bool CheckDestinationOffset() const
		{
			return GetDestinationDataTransferSize() && ((edma_tcd_doff.template Get<typename EDMA_TCD_DOFF::DOFF>() & (GetDestinationDataTransferSize() - 1)) == 0);
		}
		
		bool CheckMinorLoopByteCount() const
		{
			uint64_t nbytes = GetNBYTES();
			return CheckSourceDataTransferSize() && GetDestinationDataTransferSize() && ((nbytes & (GetSourceDataTransferSize() - 1)) == 0) && ((nbytes & (GetDestinationDataTransferSize() - 1)) == 0);
		}
		
		bool CheckMinorLoopChannelLinking() const
		{
			return edma_tcd_citer.template Get<typename EDMA_TCD_CITER::ELINKNO::ELINK>() == edma_tcd_biter.template Get<typename EDMA_TCD_BITER::ELINKNO::ELINK>();
		}
		
		bool CheckScatterGatherAddress() const
		{
			// Check TCD alignment
			return (edma_tcd_dlastsga.template Get<typename EDMA_TCD_DLASTSGA::DLASTSGA>() % SIZE) == 0;
		}

		EDMA<CONFIG> *edma;
		unsigned int dma_channel_num;
		EDMA_TCD_SADDR    edma_tcd_saddr;    // eDMA_TCDn_SADDR   
		EDMA_TCD_ATTR     edma_tcd_attr;     // eDMA_TCDn_ATTR    
		EDMA_TCD_SOFF     edma_tcd_soff;     // eDMA_TCDn_SOFF    
		EDMA_TCD_NBYTES   edma_tcd_nbytes;   // eDMA_TCDn_NBYTES_MLNO / eDMA_TCDn_NBYTES_MLOFFNO / eDMA_TCDn_NBYTES_MLOFFYES
		EDMA_TCD_SLAST    edma_tcd_slast;    // eDMA_TCDn_SLAST   
		EDMA_TCD_DADDR    edma_tcd_daddr;    // eDMA_TCDn_DADDR   
		EDMA_TCD_CITER    edma_tcd_citer;    // eDMA_TCDn_CITER_ELINKYES / eDMA_TCDn_CITER_ELINKNO
		EDMA_TCD_DOFF     edma_tcd_doff;     // eDMA_TCDn_DOFF    
		EDMA_TCD_DLASTSGA edma_tcd_dlastsga; // eDMA_TCDn_DLASTSGA
		EDMA_TCD_BITER    edma_tcd_biter;    // eDMA_TCDn_BITER_ELINKYES / eDMA_TCDn_BITER_ELINKNO
		EDMA_TCD_CSR      edma_tcd_csr;      // eDMA_TCDn_CSR     
	};
	
	struct EDMA_TCD_File
	{
		EDMA_TCD_File(EDMA<CONFIG> *edma)
		{
			unsigned int dma_channel_num;
			for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
			{
				edma_tcd[dma_channel_num] = new EDMA_TCD(dma_channel_num, edma);
			}
		}
		
		~EDMA_TCD_File()
		{
			unsigned int dma_channel_num;
			for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
			{
				delete edma_tcd[dma_channel_num];
			}
		}
		
		void Reset()
		{
			unsigned int dma_channel_num;
			for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
			{
				edma_tcd[dma_channel_num]->Reset();
			}
		}
		
		EDMA_TCD& operator [] (unsigned int dma_channel_num) { return *edma_tcd[dma_channel_num]; }
		
		EDMA_TCD *edma_tcd[NUM_DMA_CHANNELS];
	};
	
	unisim::kernel::tlm2::ClockPropertiesProxy m_clk_prop_proxy; // proxy to get clock properties from master clock port
	unisim::kernel::tlm2::ClockPropertiesProxy dma_clk_prop_proxy; // proxy to get clock properties from dma clock port
	
	EDMA_CR                                                                        edma_cr;           // eDMA_CR  
	EDMA_ES                                                                        edma_es;           // eDMA_ES  
	EDMA_ERQH                                                                      edma_erqh;         // eDMA_ERQH
	EDMA_ERQL                                                                      edma_erql;         // eDMA_ERQL
	EDMA_EEIH                                                                      edma_eeih;         // eDMA_EEIH
	EDMA_EEIL                                                                      edma_eeil;         // eDMA_EEIL
	EDMA_SERQ                                                                      edma_serq;         // eDMA_SERQ
	EDMA_CERQ                                                                      edma_cerq;         // eDMA_CERQ
	EDMA_SEEI                                                                      edma_seei;         // eDMA_SEEI
	EDMA_CEEI                                                                      edma_ceei;         // eDMA_CEEI
	EDMA_CINT                                                                      edma_cint;         // eDMA_CINT
	EDMA_CERR                                                                      edma_cerr;         // eDMA_CERR
	EDMA_SSRT                                                                      edma_ssrt;         // eDMA_SSRT
	EDMA_CDNE                                                                      edma_cdne;         // eDMA_CDNE
	EDMA_INTH                                                                      edma_inth;         // eDMA_INTH
	EDMA_INTL                                                                      edma_intl;         // eDMA_INTL
	EDMA_ERRH                                                                      edma_errh;         // eDMA_ERRH
	EDMA_ERRL                                                                      edma_errl;         // eDMA_ERRL
	EDMA_HRSH                                                                      edma_hrsh;         // eDMA_HRSH
	EDMA_HRSL                                                                      edma_hrsl;         // eDMA_HRSL
	AddressableRegisterFile<EDMA_DCHPRI, NUM_DMA_CHANNELS, EDMA<CONFIG>, tlm_trans_attr> edma_dchpri; // eDMA_DCHPRIn      
	AddressableRegisterFile<EDMA_DCHMID, NUM_DMA_CHANNELS, EDMA<CONFIG>, tlm_trans_attr> edma_dchmid; // eDMA_DCHMIDn      
	EDMA_TCD_File                                                                  edma_tcd_file;     // eDMA TCDs
	
	RegisterAddressMap<sc_dt::uint64, tlm_trans_attr> reg_addr_map;
	
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	unisim::kernel::tlm2::PayloadFabric<tlm::tlm_generic_payload> payload_fabric;
	
	unisim::kernel::tlm2::Schedule<Event> schedule; // Payload (processor requests over AHB interface) schedule
	
	unisim::util::endian::endian_type endian;
	unisim::kernel::variable::Parameter<unisim::util::endian::endian_type> param_endian;
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	MasterID master_id;
	unisim::kernel::variable::Parameter<MasterID> param_master_id;
	
	tlm_trans_attr default_trans_attr;
	
	sc_core::sc_time master_clock_period;                 // Master clock period
	sc_core::sc_time master_clock_start_time;             // Master clock start time
	bool master_clock_posedge_first;                      // Master clock posedge first ?
	double master_clock_duty_cycle;                       // Master clock duty cycle
	
	sc_core::sc_time dma_clock_period;                 // DMA clock period
	sc_core::sc_time dma_clock_start_time;             // DMA clock start time
	bool dma_clock_posedge_first;                      // DMA clock posedge first ?
	double dma_clock_duty_cycle;                       // DMA clock duty cycle

	sc_core::sc_time dma_engine_time;
	sc_core::sc_event dma_engine_event;
	sc_core::sc_event *gen_irq_event[NUM_DMA_CHANNELS];
	sc_core::sc_event *gen_err_event[NUM_DMA_CHANNELS];
	sc_core::sc_event *gen_dma_channel_ack_event[NUM_DMA_CHANNELS];
	EDGE gen_dma_channel_ack_edge[NUM_DMA_CHANNELS];
	
	bool grp_per_prio_error;
	bool ch_per_prio_error[NUM_GROUPS];
	
	unsigned int sel_grp;                                         // round-robin
	unsigned int sel_ch_per_grp[NUM_CHANNELS_PER_GROUP];          // robin-robin
	unsigned int grp_per_prio[NUM_GROUPS];                        // fixed-priority
	unsigned int ch_per_prio[NUM_GROUPS][NUM_CHANNELS_PER_GROUP]; // fixed-priority
	
	EDMA_TCD channel_x_tcd;
	EDMA_TCD channel_y_tcd;
	EDMA_TCD *channel_tcd; // points to either channel x or y
	
	void Reset();
	void EnableAllRequests();
	void EnableRequest(unsigned int dma_channel_num);
	void DisableAllRequests();
	void DisableRequest(unsigned int dma_channel_num);
	void EnableAllErrorInterrupts();
	void EnableErrorInterrupt(unsigned int dma_channel_num);
	void DisableAllErrorInterrupts();
	void DisableErrorInterrupt(unsigned int dma_channel_num);
	void SetInterruptRequest(unsigned int dma_channel_num);
	void ClearAllInterruptRequests();
	void ClearInterruptRequest(unsigned int dma_channel_num);
	void SetErrorIndicator(unsigned int dma_channel_num);
	bool ErrorIndicator(unsigned int dma_channel_num);
	void ClearAllErrorIndicators();
	void ClearErrorIndicator(unsigned int dma_channel_num);
	void SetAllStartBits();
	void SetStartBit(unsigned int dma_channel_num);
	void ClearAllDoneBits();
	void ClearDoneBit(unsigned int dma_channel_num);
	bool HardwareRequestStatus(unsigned int dma_channel_num);
	void UpdateAllHardwareRequestStatus();
	void UpdateHardwareRequestStatus(unsigned int dma_channel_num);
	bool RequestStatus(unsigned int dma_channel_num);
	bool InterruptRequestStatus(unsigned int dma_channel_num);
	bool ErrorStatus(unsigned int dma_channel_num);
	void UpdateInterruptRequests();
	void UpdateInterruptRequest(unsigned int dma_channel_num);
	void UpdateErrors();
	void UpdateError(unsigned int dma_channel_num);
	void UpdateAcknowledge(unsigned int dma_channel_num, const sc_core::sc_time& delay = sc_core::SC_ZERO_TIME);
	void Acknowledge(unsigned int dma_channel_num);
	void NotifyEngine();
	void UpdateVLD();
	void UpdateChannelPriority(unsigned int dma_channel_num);
	void UpdateGroupPriority();
	bool SelectChannel(unsigned int dma_grp_num, unsigned int& dma_channel_num, bool preempt = false);
	bool SelectChannel(unsigned int& dma_channel_num, bool preempt = false);
	void ProcessEvent(Event *event);
	void ProcessEvents();
	void Process();
	void RESET_B_Process();
	void DMA_CHANNEL_Process(unsigned int dma_channel_num);
	void IRQ_Process(unsigned int dma_channel_num);
	void ERR_IRQ_Process(unsigned int dma_channel_num);
	void DMA_CHANNEL_ACK_Process(unsigned int dma_channel_num);
	void SetMasterID(unsigned int dma_channel_num, MasterID mid);
	void SetPrivilegeAccessLevel(unsigned int dma_channel_num, PrivilegeAccessLevel pal);
	MasterID GetMasterID(unsigned int dma_channel_num);
	PrivilegeAccessLevel GetPrivilegeAccessLevel(unsigned int dma_channel_num);
	bool Transfer(tlm::tlm_command cmd, MasterID mid, PrivilegeAccessLevel pal, uint32_t& addr, uint8_t *data_ptr, unsigned int size, unsigned int tsize, int32_t addr_signed_offset, uint32_t addr_mask, sc_core::sc_time& t);
	bool LoadTCD(unsigned int dma_channel_num, sc_dt::uint64 addr, sc_core::sc_time& t);
	bool CheckTCD(EDMA_TCD& tcd);
	void DMA_Engine_Process();

	void UpdateMasterClock();
	void UpdateDMAClock();
	void MasterClockPropertiesChangedProcess();
	void DMAClockPropertiesChangedProcess();
};

} // end of namespace edma
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace dma
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_DMA_FREESCALE_MPC57XX_EDMA_EDMA_HH__
