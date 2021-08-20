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

#ifndef __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_SIUL2_SIUL2_HH__
#define __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_SIUL2_SIUL2_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/endian/endian.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/tlm2/clock.hh>
#include <unisim/util/reg/core/register.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/debug/simple_register_registry.hh>
#include <unisim/component/tlm2/com/freescale/mpc57xx/siul2/defs.hh>

#define SWITCH_ENUM_TRAIT(ENUM_TYPE, CLASS_NAME) template <ENUM_TYPE, bool __SWITCH_TRAIT_DUMMY__ = true> struct CLASS_NAME {}
#define CASE_ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) template <bool __SWITCH_TRAIT_DUMMY__> struct CLASS_NAME<ENUM_VALUE, __SWITCH_TRAIT_DUMMY__>
#define ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) CLASS_NAME<ENUM_VALUE>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace freescale {
namespace mpc57xx {
namespace siul2 {

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
using unisim::util::reg::core::ClearReadWriteStatusFlag;
using unisim::util::reg::core::RWSF_WROB;

template <typename REGISTER, typename FIELD, int OFFSET1, int OFFSET2 = -1, Access _ACCESS = SW_RW>
struct Field : unisim::util::reg::core::Field<FIELD
                                             , ((OFFSET1 >= 0) && (OFFSET1 < REGISTER::SIZE)) ? ((OFFSET2 >= 0) ? ((OFFSET2 < REGISTER::SIZE) ? ((OFFSET1 < OFFSET2) ? ((REGISTER::SIZE - 1) - OFFSET2) : ((REGISTER::SIZE - 1) - OFFSET1)) : ((REGISTER::SIZE - 1) - OFFSET1)) : ((REGISTER::SIZE - 1) - OFFSET1)) : 0
                                             , ((OFFSET1 >= 0) && (OFFSET1 < REGISTER::SIZE)) ? ((OFFSET2 >= 0) ? ((OFFSET2 < REGISTER::SIZE) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 0) : 1) : 0
                                             , _ACCESS>
{
};

template <unsigned int N>
struct LOG2
{
    static const unsigned int VALUE = LOG2<N / 2>::VALUE + 1;
};

template <>
struct LOG2<1>
{
    static const unsigned int VALUE = 0;
};

enum OUTPUT_DRIVE_CONTROL
{
	ODC_OUTPUT_BUFFER_DISABLED   = 0x0,
	ODC_OPEN_DRAIN               = 0x1,
	ODC_PUSH_PULL                = 0x2,
	ODC_OPEN_SOURCE              = 0x3,
	ODC_MICROSECOND_CHANNEL_LVDS = 0x4,
	ODC_LFAST_LVDS               = 0x5,
};

#if 0
struct SIUL2_CONFIG
{
	static const unsigned int BUSWIDTH                               = 0;
	static const unsigned int NUM_MULTIPLEXED_SIGNALS_CONFIGURATIONS = 512;
	static const unsigned int NUM_PADS                               = 341;
	static const unsigned int NUM_INTERRUPT_FILTERS                  = 32;
	static const unsigned int PART_NUMBER                            = 0x5777;
	static const bool ENGINEERING_DEVICE                             = true;
	static const PACKAGE_SETTING_TYPE PACKAGE_SETTING                = BGA512;
	static const unsigned int MAJOR_MASK                             = 1;
	static const unsigned int MINOR_MASK                             = 1;
	static const MANUFACTURER_TYPE MANUFACTURER                      = FREESCALE;
	static const unsigned int FLASH_SIZE                             = 8 * 1024 * 1024;
	static const char FAMILY_NUMBER                                  = 'M';
};
#endif

template <typename CONFIG>
class SIUL2
	: public sc_core::sc_module
	, public tlm::tlm_fw_transport_if<>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Registers>
{
public:
	static const unsigned int TLM2_IP_VERSION_MAJOR                  = 1;
	static const unsigned int TLM2_IP_VERSION_MINOR                  = 0;
	static const unsigned int TLM2_IP_VERSION_PATCH                  = 0;
	static const unsigned int BUSWIDTH                               = CONFIG::BUSWIDTH;
	static const unsigned int NUM_MULTIPLEXED_SIGNALS_CONFIGURATIONS = CONFIG::NUM_MULTIPLEXED_SIGNALS_CONFIGURATIONS;
	static const unsigned int NUM_PADS                               = CONFIG::NUM_PADS;
	static const unsigned int MAX_PADS                               = 512;
	static const unsigned int NUM_INTERRUPT_FILTERS                  = CONFIG::NUM_INTERRUPT_FILTERS;
	static const unsigned int PART_NUMBER                            = CONFIG::PART_NUMBER;
	static const bool ENGINEERING_DEVICE                             = CONFIG::ENGINEERING_DEVICE;
	static const PACKAGE_SETTING_TYPE PACKAGE_SETTING                = CONFIG::PACKAGE_SETTING;
	static const unsigned int MAJOR_MASK                             = CONFIG::MAJOR_MASK;
	static const unsigned int MINOR_MASK                             = CONFIG::MINOR_MASK;
	static const MANUFACTURER_TYPE MANUFACTURER                      = CONFIG::MANUFACTURER;
	static const unsigned int FLASH_SIZE                             = CONFIG::FLASH_SIZE;
	static const char FAMILY_NUMBER                                  = CONFIG::FAMILY_NUMBER;
	static const bool threaded_model                                 = false;
	
	// TLM socket types
	typedef tlm::tlm_target_socket<BUSWIDTH>         peripheral_slave_if_type;
	
	// inputs
	peripheral_slave_if_type                         peripheral_slave_if; // Peripheral slave interface
	sc_core::sc_in<bool>                             m_clk;               // clock port
	sc_core::sc_in<bool>                             reset_b;             // reset
	sc_core::sc_vector<sc_core::sc_in<bool> >        pad_in;              // pad in
	
	// outputs
	sc_core::sc_vector<sc_core::sc_out<bool> >       pad_out;             // pad out
	
	// services
	unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;

	SIUL2(const sc_core::sc_module_name& name, unisim::kernel::Object *parent);
	virtual ~SIUL2();
	
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

	// SIUL2 Register
	template <typename REGISTER, unsigned int _SIZE, Access _ACCESS>
	struct SIUL2_Register : AddressableRegister<REGISTER, _SIZE, _ACCESS>
	{
		typedef AddressableRegister<REGISTER, _SIZE, _ACCESS> Super;
		
		SIUL2_Register(SIUL2<CONFIG> *_siul2) : Super(), siul2(_siul2) {}
		SIUL2_Register(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(value), siul2(_siul2) {}
		
		inline bool IsVerboseRead() const ALWAYS_INLINE { return siul2->verbose; }
		inline bool IsVerboseWrite() const ALWAYS_INLINE { return siul2->verbose; }
		inline std::ostream& GetInfoStream() ALWAYS_INLINE { return siul2->logger.DebugInfoStream(); }
		
		using Super::operator =;
	protected:
		SIUL2<CONFIG> *siul2;
	};

	// MCU ID Register #1 (SIUL2_MIDR1)
	struct SIUL2_MIDR1 : SIUL2_Register<SIUL2_MIDR1, 32, SW_R>
	{
		typedef SIUL2_Register<SIUL2_MIDR1, 32, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x4;
		
		struct PARTNUM    : Field<SIUL2_MIDR1, PARTNUM   , 0, 15 > {}; // MCU Part Number
		struct ED         : Field<SIUL2_MIDR1, ED        , 16    > {}; // Engineering Device
		struct PKG        : Field<SIUL2_MIDR1, PKG       , 17, 21> {}; // Package Settings
		struct MAJOR_MASK : Field<SIUL2_MIDR1, MAJOR_MASK, 24, 27> {}; // Major Mask Revision
		struct MINOR_MASK : Field<SIUL2_MIDR1, MINOR_MASK, 28, 31> {}; // Minor Mask Revision
		
		typedef FieldSet<PARTNUM, ED, PKG, MAJOR_MASK, MINOR_MASK> ALL;
		
		SIUL2_MIDR1(SIUL2<CONFIG> *_siul2) : Super(_siul2) { Init(); }
		SIUL2_MIDR1(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value) { Init(); }
		
		void Init()
		{
			this->SetName("SIUL2_MIDR1"); this->SetDescription("MCU ID Register #1");
			
			PARTNUM   ::SetName("PARTNUM");    PARTNUM   ::SetDescription("MCU Part Number");
			ED        ::SetName("ED");         ED        ::SetDescription("Engineering Device");
			PKG       ::SetName("PKG");        PKG       ::SetDescription("Package Settings");
			MAJOR_MASK::SetName("MAJOR_MASK"); MAJOR_MASK::SetDescription("Major Mask Revision");
			MINOR_MASK::SetName("MINOR_MASK"); MINOR_MASK::SetDescription("Minor Mask Revision");
		}
		
		void Reset()
		{
			this->template Set<PARTNUM>(PART_NUMBER);
			this->template Set<ED>(ENGINEERING_DEVICE);
			this->template Set<PKG>(PACKAGE_SETTING);
			this->template Set<MAJOR_MASK>(SIUL2<CONFIG>::MAJOR_MASK);
			this->template Set<MINOR_MASK>(SIUL2<CONFIG>::MINOR_MASK);
		}
		
		using Super::operator =;
	};
	
	// MCU ID Register #2 (SIUL2_MIDR2)
	struct SIUL2_MIDR2 : SIUL2_Register<SIUL2_MIDR2, 32, SW_R>
	{
		typedef SIUL2_Register<SIUL2_MIDR2, 32, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x8;
		
		struct SF           : Field<SIUL2_MIDR2, SF          , 0     > {}; // Manufacturer
		struct FLASH_SIZE_1 : Field<SIUL2_MIDR2, FLASH_SIZE_1, 1, 4  > {}; // Coarse granularity for Flash memory size
		struct FLASH_SIZE_2 : Field<SIUL2_MIDR2, FLASH_SIZE_2, 5, 8  > {}; // Fine granularity for Flash memory size
		struct FAMILYNUM    : Field<SIUL2_MIDR2, FAMILYNUM   , 16, 23> {}; // ASCII character in MCU Part Number
		
		typedef FieldSet<SF, FLASH_SIZE_1, FLASH_SIZE_2, FAMILYNUM> ALL;
		
		SIUL2_MIDR2(SIUL2<CONFIG> *_siul2) : Super(_siul2) { Init(); }
		SIUL2_MIDR2(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value) { Init(); }
		
		void Init()
		{
			this->SetName("SIUL2_MIDR2"); this->SetDescription("MCU ID Register #2");
			
			SF          ::SetName("SF");           SF          ::SetDescription("Manufacturer");
			FLASH_SIZE_1::SetName("FLASH_SIZE_1"); FLASH_SIZE_1::SetDescription("Coarse granularity for Flash memory size");
			FLASH_SIZE_2::SetName("FLASH_SIZE_2"); FLASH_SIZE_2::SetDescription("Fine granularity for Flash memory size");
			FAMILYNUM   ::SetName("FAMILYNUM");    FAMILYNUM   ::SetDescription("ASCII character in MCU Part Number");
		}
		
		bool ComputeFlashGranularities(unsigned int flash_size, unsigned int& flash_size_1, unsigned int& flash_size_2) const
		{
			unsigned int _flash_size_1;
			unsigned int _flash_size_2;
			for(_flash_size_2 = 0; _flash_size_2 < 15; _flash_size_2++)
			{
				for(_flash_size_1 = 0; _flash_size_1 < 15; _flash_size_1++)
				{
					unsigned int coarse_granularity_size = 16384 << _flash_size_1;
					unsigned int fine_granularity_size = (_flash_size_2 * coarse_granularity_size) / 8;
					unsigned int _flash_size = coarse_granularity_size + fine_granularity_size;
					//std::cerr << "coarse_granularity_size=" << coarse_granularity_size << ", fine_granularity_size=" << fine_granularity_size << ", _flash_size=" << _flash_size << std::endl;
					if(_flash_size == flash_size)
					{
						flash_size_1 = _flash_size_1;
						flash_size_2 = _flash_size_2;
						return true;
					}
				}
			}
			
			return false;
		}
		
		void Reset()
		{
			this->template Set<SF>(MANUFACTURER);
			
			unsigned int flash_size_1 = LOG2<FLASH_SIZE / 16384>::VALUE;
			unsigned int flash_size_2 = 0;
			
			if(!ComputeFlashGranularities(FLASH_SIZE, flash_size_1, flash_size_2))
			{
				this->siul2->logger << DebugWarning << "FLASH size can't be precisely encoded in " << this->GetName() << EndDebugWarning;
			}
			
			this->template Set<FLASH_SIZE_1>(flash_size_1);
			this->template Set<FLASH_SIZE_2>(flash_size_2);
			
			this->template Set<FAMILYNUM>(FAMILY_NUMBER);
		}
		
		using Super::operator =;
	};
	
	// DMA/Interrupt Status Flag Register0 (SIUL2_DISR0)
	struct SIUL2_DISR0 : SIUL2_Register<SIUL2_DISR0, 32, SW_RW>
	{
		typedef SIUL2_Register<SIUL2_DISR0, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x10;
		
		struct EIF11 : Field<SIUL2_DISR0, EIF11, 20, 20, SW_R_W1C> {}; // External Interrupt Status Flag 11
		struct EIF10 : Field<SIUL2_DISR0, EIF10, 21, 21, SW_R_W1C> {}; // External Interrupt Status Flag 10
		struct EIF9  : Field<SIUL2_DISR0, EIF9 , 22, 22, SW_R_W1C> {}; // External Interrupt Status Flag 9
		struct EIF8  : Field<SIUL2_DISR0, EIF8 , 23, 23, SW_R_W1C> {}; // External Interrupt Status Flag 8
		struct EIF5  : Field<SIUL2_DISR0, EIF5 , 26, 26, SW_R_W1C> {}; // External Interrupt Status Flag 5
		struct EIF4  : Field<SIUL2_DISR0, EIF4 , 27, 27, SW_R_W1C> {}; // External Interrupt Status Flag 4
		struct EIF3  : Field<SIUL2_DISR0, EIF3 , 28, 28, SW_R_W1C> {}; // External Interrupt Status Flag 3
		struct EIF2  : Field<SIUL2_DISR0, EIF2 , 29, 29, SW_R_W1C> {}; // External Interrupt Status Flag 2
		struct EIF1  : Field<SIUL2_DISR0, EIF1 , 30, 30, SW_R_W1C> {}; // External Interrupt Status Flag 1
		struct EIF0  : Field<SIUL2_DISR0, EIF0 , 31, 31, SW_R_W1C> {}; // External Interrupt Status Flag 0
		
		typedef FieldSet<EIF11, EIF10, EIF9, EIF8, EIF5, EIF4, EIF3, EIF2, EIF1, EIF0> ALL;
		
		SIUL2_DISR0(SIUL2<CONFIG> *_siul2) : Super(_siul2) { Init(); }
		SIUL2_DISR0(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value) { Init(); }
		
		void Init()
		{
			this->SetName("SIUL2_DISR0"); this->SetDescription("DMA/Interrupt Status Flag Register0");
			
			EIF11::SetName("EIF11"); EIF11::SetDescription("External Interrupt Status Flag 11");
			EIF10::SetName("EIF10"); EIF10::SetDescription("External Interrupt Status Flag 10");
			EIF9 ::SetName("EIF9");  EIF9 ::SetDescription("External Interrupt Status Flag 9");
			EIF8 ::SetName("EIF8");  EIF8 ::SetDescription("External Interrupt Status Flag 8");
			EIF5 ::SetName("EIF5");  EIF5 ::SetDescription("External Interrupt Status Flag 5");
			EIF4 ::SetName("EIF4");  EIF4 ::SetDescription("External Interrupt Status Flag 4");
			EIF3 ::SetName("EIF3");  EIF3 ::SetDescription("External Interrupt Status Flag 3");
			EIF2 ::SetName("EIF2");  EIF2 ::SetDescription("External Interrupt Status Flag 2");
			EIF1 ::SetName("EIF1");  EIF1 ::SetDescription("External Interrupt Status Flag 1");
			EIF0 ::SetName("EIF0");  EIF0 ::SetDescription("External Interrupt Status Flag 0");
		}
		
		void Reset()
		{
			 this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	// DMA/Interrupt Request Enable Register0 (SIUL2_DIRER0)
	struct SIUL2_DIRER0 : SIUL2_Register<SIUL2_DIRER0, 32, SW_RW>
	{
		typedef SIUL2_Register<SIUL2_DIRER0, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x18;
		
		struct EIRE11 : Field<SIUL2_DIRER0, EIRE11, 20> {}; // External Interrupt or DMA Request Enable 11
		struct EIRE10 : Field<SIUL2_DIRER0, EIRE10, 21> {}; // External Interrupt or DMA Request Enable 10
		struct EIRE9  : Field<SIUL2_DIRER0, EIRE9 , 22> {}; // External Interrupt or DMA Request Enable 9
		struct EIRE8  : Field<SIUL2_DIRER0, EIRE8 , 23> {}; // External Interrupt or DMA Request Enable 8
		struct EIRE5  : Field<SIUL2_DIRER0, EIRE5 , 26> {}; // External Interrupt or DMA Request Enable 5
		struct EIRE4  : Field<SIUL2_DIRER0, EIRE4 , 27> {}; // External Interrupt or DMA Request Enable 4
		struct EIRE3  : Field<SIUL2_DIRER0, EIRE3 , 28> {}; // External Interrupt or DMA Request Enable 3
		struct EIRE2  : Field<SIUL2_DIRER0, EIRE2 , 29> {}; // External Interrupt or DMA Request Enable 2
		struct EIRE1  : Field<SIUL2_DIRER0, EIRE1 , 30> {}; // External Interrupt or DMA Request Enable 1
		struct EIRE0  : Field<SIUL2_DIRER0, EIRE0 , 31> {}; // External Interrupt or DMA Request Enable 0
		
		typedef FieldSet<EIRE11, EIRE10, EIRE9, EIRE8, EIRE5, EIRE4, EIRE3, EIRE2, EIRE1, EIRE0> ALL;
		
		SIUL2_DIRER0(SIUL2<CONFIG> *_siul2) : Super(_siul2) { Init(); }
		SIUL2_DIRER0(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value) { Init(); }
		
		void Init()
		{
			this->SetName("SIUL2_DIRER0"); this->SetDescription("DMA/Interrupt Request Enable Register0");
			
			EIRE11::SetName("EIRE11"); EIRE11::SetDescription("External Interrupt or DMA Request Enable 11");
			EIRE10::SetName("EIRE10"); EIRE10::SetDescription("External Interrupt or DMA Request Enable 10");
			EIRE9 ::SetName("EIRE9");  EIRE9 ::SetDescription("External Interrupt or DMA Request Enable 9");
			EIRE8 ::SetName("EIRE8");  EIRE8 ::SetDescription("External Interrupt or DMA Request Enable 8");
			EIRE5 ::SetName("EIRE5");  EIRE5 ::SetDescription("External Interrupt or DMA Request Enable 5");
			EIRE4 ::SetName("EIRE4");  EIRE4 ::SetDescription("External Interrupt or DMA Request Enable 4");
			EIRE3 ::SetName("EIRE3");  EIRE3 ::SetDescription("External Interrupt or DMA Request Enable 3");
			EIRE2 ::SetName("EIRE2");  EIRE2 ::SetDescription("External Interrupt or DMA Request Enable 2");
			EIRE1 ::SetName("EIRE1");  EIRE1 ::SetDescription("External Interrupt or DMA Request Enable 1");
			EIRE0 ::SetName("EIRE0");  EIRE0 ::SetDescription("External Interrupt or DMA Request Enable 0");
		}
		
		void Reset()
		{
			 this->Initialize(0x0);
		}

		using Super::operator =;
	};
	
	// DMA/Interrupt Request Select Register0 (SIUL2_DIRSR0)
	struct SIUL2_DIRSR0 : SIUL2_Register<SIUL2_DIRSR0, 32, SW_RW>
	{
		typedef SIUL2_Register<SIUL2_DIRSR0, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x20;
		
		struct DIRS11 : Field<SIUL2_DIRSR0, DIRS11, 20> {}; // DMA/Interrupt Request Select 11
		struct DIRS10 : Field<SIUL2_DIRSR0, DIRS10, 21> {}; // DMA/Interrupt Request Select 10
		struct DIRS9  : Field<SIUL2_DIRSR0, DIRS9 , 22> {}; // DMA/Interrupt Request Select 9
		struct DIRS8  : Field<SIUL2_DIRSR0, DIRS8 , 23> {}; // DMA/Interrupt Request Select 8
		struct DIRS5  : Field<SIUL2_DIRSR0, DIRS5 , 26> {}; // DMA/Interrupt Request Select 5
		struct DIRS4  : Field<SIUL2_DIRSR0, DIRS4 , 27> {}; // DMA/Interrupt Request Select 4
		struct DIRS3  : Field<SIUL2_DIRSR0, DIRS3 , 28> {}; // DMA/Interrupt Request Select 3
		struct DIRS2  : Field<SIUL2_DIRSR0, DIRS2 , 29> {}; // DMA/Interrupt Request Select 2
		struct DIRS1  : Field<SIUL2_DIRSR0, DIRS1 , 30> {}; // DMA/Interrupt Request Select 1
		struct DIRS0  : Field<SIUL2_DIRSR0, DIRS0 , 31> {}; // DMA/Interrupt Request Select 0
		
		typedef FieldSet<DIRS11, DIRS10, DIRS9, DIRS8, DIRS5, DIRS4, DIRS3, DIRS2, DIRS1, DIRS0> ALL;
		
		SIUL2_DIRSR0(SIUL2<CONFIG> *_siul2) : Super(_siul2) { Init(); }
		SIUL2_DIRSR0(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value) { Init(); }
		
		void Init()
		{
			this->SetName("SIUL2_DIRSR0"); this->SetDescription("DMA/Interrupt Request Select Register0");
			
			DIRS11::SetName("DIRS11"); DIRS11::SetDescription("DMA/Interrupt Request Select 11");
			DIRS10::SetName("DIRS10"); DIRS10::SetDescription("DMA/Interrupt Request Select 10");
			DIRS9 ::SetName("DIRS9");  DIRS9 ::SetDescription("DMA/Interrupt Request Select 9");
			DIRS8 ::SetName("DIRS8");  DIRS8 ::SetDescription("DMA/Interrupt Request Select 8");
			DIRS5 ::SetName("DIRS5");  DIRS5 ::SetDescription("DMA/Interrupt Request Select 5");
			DIRS4 ::SetName("DIRS4");  DIRS4 ::SetDescription("DMA/Interrupt Request Select 4");
			DIRS3 ::SetName("DIRS3");  DIRS3 ::SetDescription("DMA/Interrupt Request Select 3");
			DIRS2 ::SetName("DIRS2");  DIRS2 ::SetDescription("DMA/Interrupt Request Select 2");
			DIRS1 ::SetName("DIRS1");  DIRS1 ::SetDescription("DMA/Interrupt Request Select 1");
			DIRS0 ::SetName("DIRS0");  DIRS0 ::SetDescription("DMA/Interrupt Request Select 0");
		}
		
		void Reset()
		{
			 this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	// Interrupt Rising-Edge Event Enable Register0 (SIUL2_IREER0)
	struct SIUL2_IREER0 : SIUL2_Register<SIUL2_IREER0, 32, SW_RW>
	{
		typedef SIUL2_Register<SIUL2_IREER0, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x28;
		
		struct IREE11 : Field<SIUL2_IREER0, IREE11, 20> {}; // Enable rising-edge events to cause the EIF[11] bit to be set
		struct IREE10 : Field<SIUL2_IREER0, IREE10, 21> {}; // Enable rising-edge events to cause the EIF[10] bit to be set
		struct IREE9  : Field<SIUL2_IREER0, IREE9 , 22> {}; // Enable rising-edge events to cause the EIF[9] bit to be set
		struct IREE8  : Field<SIUL2_IREER0, IREE8 , 23> {}; // Enable rising-edge events to cause the EIF[8] bit to be set
		struct IREE5  : Field<SIUL2_IREER0, IREE5 , 26> {}; // Enable rising-edge events to cause the EIF[5] bit to be set
		struct IREE4  : Field<SIUL2_IREER0, IREE4 , 27> {}; // Enable rising-edge events to cause the EIF[4] bit to be set
		struct IREE3  : Field<SIUL2_IREER0, IREE3 , 28> {}; // Enable rising-edge events to cause the EIF[3] bit to be set
		struct IREE2  : Field<SIUL2_IREER0, IREE2 , 29> {}; // Enable rising-edge events to cause the EIF[2] bit to be set
		struct IREE1  : Field<SIUL2_IREER0, IREE1 , 30> {}; // Enable rising-edge events to cause the EIF[1] bit to be set
		struct IREE0  : Field<SIUL2_IREER0, IREE0 , 31> {}; // Enable rising-edge events to cause the EIF[0] bit to be set
		
		typedef FieldSet<IREE11, IREE10, IREE9, IREE8, IREE5, IREE4, IREE3, IREE2, IREE1, IREE0> ALL;
		
		SIUL2_IREER0(SIUL2<CONFIG> *_siul2) : Super(_siul2) { Init(); }
		SIUL2_IREER0(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value) { Init(); }
		
		void Init()
		{
			this->SetName("SIUL2_IREER0"); this->SetDescription("Interrupt Rising-Edge Event Enable Register0");
			
			IREE11::SetName("IREE11"); IREE11::SetDescription("Enable rising-edge events to cause the EIF[11] bit to be set");
			IREE10::SetName("IREE10"); IREE10::SetDescription("Enable rising-edge events to cause the EIF[10] bit to be set");
			IREE9 ::SetName("IREE9");  IREE9 ::SetDescription("Enable rising-edge events to cause the EIF[9] bit to be set");
			IREE8 ::SetName("IREE8");  IREE8 ::SetDescription("Enable rising-edge events to cause the EIF[8] bit to be set");
			IREE5 ::SetName("IREE5");  IREE5 ::SetDescription("Enable rising-edge events to cause the EIF[5] bit to be set");
			IREE4 ::SetName("IREE4");  IREE4 ::SetDescription("Enable rising-edge events to cause the EIF[4] bit to be set");
			IREE3 ::SetName("IREE3");  IREE3 ::SetDescription("Enable rising-edge events to cause the EIF[3] bit to be set");
			IREE2 ::SetName("IREE2");  IREE2 ::SetDescription("Enable rising-edge events to cause the EIF[2] bit to be set");
			IREE1 ::SetName("IREE1");  IREE1 ::SetDescription("Enable rising-edge events to cause the EIF[1] bit to be set");
			IREE0 ::SetName("IREE0");  IREE0 ::SetDescription("Enable rising-edge events to cause the EIF[0] bit to be set");
		}
		
		void Reset()
		{
			 this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	// Interrupt Falling-Edge Event Enable Register0 (SIUL2_IFEER0)
	struct SIUL2_IFEER0 : SIUL2_Register<SIUL2_IFEER0, 32, SW_RW>
	{
		typedef SIUL2_Register<SIUL2_IFEER0, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x30;
		
		struct IFEE11 : Field<SIUL2_IFEER0, IFEE11, 20> {}; // Enable falling-edge events to cause the EIF[11] bit to be set
		struct IFEE10 : Field<SIUL2_IFEER0, IFEE10, 21> {}; // Enable falling-edge events to cause the EIF[10] bit to be set
		struct IFEE9  : Field<SIUL2_IFEER0, IFEE9 , 22> {}; // Enable falling-edge events to cause the EIF[9] bit to be set
		struct IFEE8  : Field<SIUL2_IFEER0, IFEE8 , 23> {}; // Enable falling-edge events to cause the EIF[8] bit to be set
		struct IFEE5  : Field<SIUL2_IFEER0, IFEE5 , 26> {}; // Enable falling-edge events to cause the EIF[5] bit to be set
		struct IFEE4  : Field<SIUL2_IFEER0, IFEE4 , 27> {}; // Enable falling-edge events to cause the EIF[4] bit to be set
		struct IFEE3  : Field<SIUL2_IFEER0, IFEE3 , 28> {}; // Enable falling-edge events to cause the EIF[3] bit to be set
		struct IFEE2  : Field<SIUL2_IFEER0, IFEE2 , 29> {}; // Enable falling-edge events to cause the EIF[2] bit to be set
		struct IFEE1  : Field<SIUL2_IFEER0, IFEE1 , 30> {}; // Enable falling-edge events to cause the EIF[1] bit to be set
		struct IFEE0  : Field<SIUL2_IFEER0, IFEE0 , 31> {}; // Enable falling-edge events to cause the EIF[0] bit to be set
		
		typedef FieldSet<IFEE11, IFEE10, IFEE9, IFEE8, IFEE5, IFEE4, IFEE3, IFEE2, IFEE1, IFEE0> ALL;
		
		SIUL2_IFEER0(SIUL2<CONFIG> *_siul2) : Super(_siul2) { Init(); }
		SIUL2_IFEER0(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value) { Init(); }
		
		void Init()
		{
			this->SetName("SIUL2_IFEER0"); this->SetDescription("Interrupt Falling-Edge Event Enable Register0");
			
			IFEE11::SetName("IFEE11"); IFEE11::SetDescription("Enable falling-edge events to cause the EIF[11] bit to be set");
			IFEE10::SetName("IFEE10"); IFEE10::SetDescription("Enable falling-edge events to cause the EIF[10] bit to be set");
			IFEE9 ::SetName("IFEE9");  IFEE9 ::SetDescription("Enable falling-edge events to cause the EIF[9] bit to be set");
			IFEE8 ::SetName("IFEE8");  IFEE8 ::SetDescription("Enable falling-edge events to cause the EIF[8] bit to be set");
			IFEE5 ::SetName("IFEE5");  IFEE5 ::SetDescription("Enable falling-edge events to cause the EIF[5] bit to be set");
			IFEE4 ::SetName("IFEE4");  IFEE4 ::SetDescription("Enable falling-edge events to cause the EIF[4] bit to be set");
			IFEE3 ::SetName("IFEE3");  IFEE3 ::SetDescription("Enable falling-edge events to cause the EIF[3] bit to be set");
			IFEE2 ::SetName("IFEE2");  IFEE2 ::SetDescription("Enable falling-edge events to cause the EIF[2] bit to be set");
			IFEE1 ::SetName("IFEE1");  IFEE1 ::SetDescription("Enable falling-edge events to cause the EIF[1] bit to be set");
			IFEE0 ::SetName("IFEE0");  IFEE0 ::SetDescription("Enable falling-edge events to cause the EIF[0] bit to be set");
		}
		
		void Reset()
		{
			 this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	// Interrupt Filter Enable Register0 (SIUL2_IFER0)
	struct SIUL2_IFER0 : SIUL2_Register<SIUL2_IFER0, 32, SW_RW>
	{
		typedef SIUL2_Register<SIUL2_IFER0, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x38;
		
		struct IFE11 : Field<SIUL2_IFER0, IFE11, 20> {}; // Enable digital glitch filter on the interrupt pad input
		struct IFE10 : Field<SIUL2_IFER0, IFE10, 21> {}; // Enable digital glitch filter on the interrupt pad input
		struct IFE9  : Field<SIUL2_IFER0, IFE9 , 22> {}; // Enable digital glitch filter on the interrupt pad input
		struct IFE8  : Field<SIUL2_IFER0, IFE8 , 23> {}; // Enable digital glitch filter on the interrupt pad input
		struct IFE5  : Field<SIUL2_IFER0, IFE5 , 26> {}; // Enable digital glitch filter on the interrupt pad input
		struct IFE4  : Field<SIUL2_IFER0, IFE4 , 27> {}; // Enable digital glitch filter on the interrupt pad input
		struct IFE3  : Field<SIUL2_IFER0, IFE3 , 28> {}; // Enable digital glitch filter on the interrupt pad input
		struct IFE2  : Field<SIUL2_IFER0, IFE2 , 29> {}; // Enable digital glitch filter on the interrupt pad input
		struct IFE1  : Field<SIUL2_IFER0, IFE1 , 30> {}; // Enable digital glitch filter on the interrupt pad input
		struct IFE0  : Field<SIUL2_IFER0, IFE0 , 31> {}; // Enable digital glitch filter on the interrupt pad input
		
		typedef FieldSet<IFE11, IFE10, IFE9, IFE8, IFE5, IFE4, IFE3, IFE2, IFE1, IFE0> ALL;
		
		SIUL2_IFER0(SIUL2<CONFIG> *_siul2) : Super(_siul2) { Init(); }
		SIUL2_IFER0(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value) { Init(); }
		
		void Init()
		{
			this->SetName("SIUL2_IFER0"); this->SetDescription("Enable digital glitch filter on the interrupt pad input");
			
			IFE11::SetName("IFE11"); IFE11::SetDescription("Enable digital glitch filter on the interrupt pad input");
			IFE10::SetName("IFE10"); IFE10::SetDescription("Enable digital glitch filter on the interrupt pad input");
			IFE9 ::SetName("IFE9");  IFE9 ::SetDescription("Enable digital glitch filter on the interrupt pad input");
			IFE8 ::SetName("IFE8");  IFE8 ::SetDescription("Enable digital glitch filter on the interrupt pad input");
			IFE5 ::SetName("IFE5");  IFE5 ::SetDescription("Enable digital glitch filter on the interrupt pad input");
			IFE4 ::SetName("IFE4");  IFE4 ::SetDescription("Enable digital glitch filter on the interrupt pad input");
			IFE3 ::SetName("IFE3");  IFE3 ::SetDescription("Enable digital glitch filter on the interrupt pad input");
			IFE2 ::SetName("IFE2");  IFE2 ::SetDescription("Enable digital glitch filter on the interrupt pad input");
			IFE1 ::SetName("IFE1");  IFE1 ::SetDescription("Enable digital glitch filter on the interrupt pad input");
			IFE0 ::SetName("IFE0");  IFE0 ::SetDescription("Enable digital glitch filter on the interrupt pad input");
		}
		
		void Reset()
		{
			 this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	// Interrupt Filter Maximum Counter Register (SIUL2_IFMCRn)
	struct SIUL2_IFMCR : SIUL2_Register<SIUL2_IFMCR, 32, SW_RW>
	{
		typedef SIUL2_Register<SIUL2_IFMCR, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x40;
		
		struct MAXCNT : Field<SIUL2_IFMCR, MAXCNT, 28, 31> {}; // Maximum Interrupt Filter Counter setting
		
		typedef FieldSet<MAXCNT> ALL;
		
		SIUL2_IFMCR() : Super(0), reg_num(0) {}
		SIUL2_IFMCR(SIUL2<CONFIG> *_siul2) : Super(_siul2), reg_num(0) {}
		SIUL2_IFMCR(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, SIUL2<CONFIG> *_siul2)
		{
			this->siul2 = _siul2;
			reg_num = _reg_num;

			std::stringstream name_sstr;
			name_sstr << "SIUL2_IFMCR" << reg_num;
			this->SetName(name_sstr.str()); this->SetDescription("Interrupt Filter Maximum Counter Register");
			
			std::stringstream maxcnt_name_sstr;
			maxcnt_name_sstr << "MAXCNT" << reg_num;
			MAXCNT::SetName(maxcnt_name_sstr.str()); MAXCNT::SetDescription("Maximum Interrupt Filter Counter setting");
		}
		
		void Reset()
		{
			 this->Initialize(0x0);
		}
		
		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};
	
	//  Interrupt Filter Clock Prescaler (SIUL2_IFCPR)
	struct SIUL2_IFCPR : SIUL2_Register<SIUL2_IFCPR, 32, SW_RW>
	{
		typedef SIUL2_Register<SIUL2_IFCPR, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xc0;
		
		struct IFCP : Field<SIUL2_IFCPR, IFCP, 28, 31> {}; // Interrupt Filter Clock Prescaler setting
		
		typedef FieldSet<IFCP> ALL;
		
		SIUL2_IFCPR() : Super(0), reg_num(0) {}
		SIUL2_IFCPR(SIUL2<CONFIG> *_siul2) : Super(_siul2), reg_num(0) {}
		SIUL2_IFCPR(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, SIUL2<CONFIG> *_siul2)
		{
			this->siul2 = _siul2;
			reg_num = _reg_num;

			std::stringstream name_sstr;
			name_sstr << "SIUL2_IFCPR" << reg_num;
			this->SetName(name_sstr.str()); this->SetDescription("Interrupt Filter Clock Prescaler");
			
			std::stringstream ifcp_name_sstr;
			ifcp_name_sstr << "IFCP" << reg_num;
			IFCP::SetName(ifcp_name_sstr.str()); IFCP::SetDescription("Interrupt Filter Clock Prescaler setting");
		}
		
		void Reset()
		{
			 this->Initialize(0x0);
		}
		
		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};
	
	// SoC Configuration Register0 (SIUL2_SCR0)
	struct SIUL2_SCR0 : SIUL2_Register<SIUL2_SCR0, 32, SW_RW>
	{
		typedef SIUL2_Register<SIUL2_SCR0, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x100;
		
		struct FEC_MODE    : Field<SIUL2_SCR0, FEC_MODE   , 0> {}; // Fast Ethernet Controller mode setting
		struct ESR0_ASSERT : Field<SIUL2_SCR0, ESR0_ASSERT, 8> {}; // Active high signal enabling to force ESR0 as an strong pull-down
		
		typedef FieldSet<FEC_MODE, ESR0_ASSERT> ALL;
		
		SIUL2_SCR0(SIUL2<CONFIG> *_siul2) : Super(_siul2) { Init(); }
		SIUL2_SCR0(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value) { Init(); }
		
		void Init()
		{
			this->SetName("SIUL2_SCR0"); this->SetDescription("SoC Configuration Register0");
			
			FEC_MODE   ::SetName("FEC_MODE");    FEC_MODE   ::SetDescription("Fast Ethernet Controller mode setting");
			ESR0_ASSERT::SetName("ESR0_ASSERT"); ESR0_ASSERT::SetDescription("Active high signal enabling to force ESR0 as an strong pull-down");
		}
		
		void Reset()
		{
			 this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	//  I/O Pin Multiplexed Signal Configuration Registers (SIUL2_MSCR_IO_n)
	struct SIUL2_MSCR_IO : SIUL2_Register<SIUL2_MSCR_IO, 32, SW_RW>
	{
		typedef SIUL2_Register<SIUL2_MSCR_IO, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x240;
		
		struct OERC : Field<SIUL2_MSCR_IO, OERC, 1, 3  > {}; // Output Edge Rate Control
		struct ODC  : Field<SIUL2_MSCR_IO, ODC , 5, 7  > {}; // Output Drive Control
		struct SMC  : Field<SIUL2_MSCR_IO, SMC , 8     > {}; // Safe Mode Control
		struct APC  : Field<SIUL2_MSCR_IO, APC , 9     > {}; // Analog Pad Control
		struct ILS  : Field<SIUL2_MSCR_IO, ILS , 10, 11> {}; // Input Level Selection
		struct IBE  : Field<SIUL2_MSCR_IO, IBE , 12    > {}; // Input Buffer Enable
		struct HYS  : Field<SIUL2_MSCR_IO, HYS , 13    > {}; // Input Hysteresis
		struct WPDE : Field<SIUL2_MSCR_IO, WPDE, 14    > {}; // Weak Pulldown Enable
		struct WPUE : Field<SIUL2_MSCR_IO, WPUE, 15    > {}; // Weak Pullup Enable
		struct INV  : Field<SIUL2_MSCR_IO, INV , 16    > {}; // Invert
		struct SSS  : Field<SIUL2_MSCR_IO, SSS , 24, 31> {}; // Source Signal Select
		
		typedef FieldSet<OERC, ODC, SMC, APC, ILS, IBE, HYS, WPDE, WPUE, INV, SSS> ALL;
		
		SIUL2_MSCR_IO() : Super(0), reg_num(0) {}
		SIUL2_MSCR_IO(SIUL2<CONFIG> *_siul2) : Super(_siul2), reg_num(0) {}
		SIUL2_MSCR_IO(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, SIUL2<CONFIG> *_siul2)
		{
			this->siul2 = _siul2;
			reg_num = _reg_num;

			std::stringstream name_sstr;
			name_sstr << "SIUL2_MSCR_IO_" << reg_num;
			this->SetName(name_sstr.str());
			
			std::stringstream description_sstr;
			description_sstr << "I/O Pin Multiplexed Signal Configuration Register " << reg_num;
			this->SetDescription(description_sstr.str());
			
			OERC::SetName("OERC"); OERC::SetDescription("Output Edge Rate Control");
			ODC ::SetName("ODC");  ODC ::SetDescription("Output Drive Control");
			SMC ::SetName("SMC");  SMC ::SetDescription("Safe Mode Control");
			APC ::SetName("APC");  APC ::SetDescription("Analog Pad Control");
			ILS ::SetName("ILS");  ILS ::SetDescription("Input Level Selection");
			IBE ::SetName("IBE");  IBE ::SetDescription("Input Buffer Enable");
			HYS ::SetName("HYS");  HYS ::SetDescription("Input Hysteresis");
			WPDE::SetName("WPDE"); WPDE::SetDescription("Weak Pulldown Enable");
			WPUE::SetName("WPUE"); WPUE::SetDescription("Weak Pullup Enable");
			INV ::SetName("INV");  INV ::SetDescription("Invert");
			SSS ::SetName("SSS");  SSS ::SetDescription("Source Signal Select");
		}
		
		void Reset()
		{
			 this->Initialize(0x00090000UL);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				// Note: only GPIO is supported corresponding to first entries
				if(reg_num < NUM_PADS)
				{
					this->siul2->UpdateOutputBuffer(reg_num);
				}
			}
			
			return rws;
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	//   Multiplexed Signal Configuration Register for Multiplexed Input Selection (SIUL2_MSCR_MUX_n)
	struct SIUL2_MSCR_MUX : SIUL2_Register<SIUL2_MSCR_MUX, 32, SW_RW>
	{
		typedef SIUL2_Register<SIUL2_MSCR_MUX, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xa40;
		
		struct INV  : Field<SIUL2_MSCR_MUX, INV , 16    > {}; // Invert
		struct SSS  : Field<SIUL2_MSCR_MUX, SSS , 24, 31> {}; // Source Signal Select
		
		typedef FieldSet<INV, SSS> ALL;
		
		SIUL2_MSCR_MUX() : Super(0), reg_num(0) {}
		SIUL2_MSCR_MUX(SIUL2<CONFIG> *_siul2) : Super(_siul2), reg_num(0) {}
		SIUL2_MSCR_MUX(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, SIUL2<CONFIG> *_siul2)
		{
			this->siul2 = _siul2;
			reg_num = 512 + _reg_num; // Note: register numbering start with 512

			std::stringstream name_sstr;
			name_sstr << "SIUL2_MSCR_MUX_" << reg_num;
			this->SetName(name_sstr.str());
			
			std::stringstream description_sstr;
			description_sstr << "Multiplexed Signal Configuration Register for Multiplexed Input Selection " << reg_num;
			this->SetDescription(description_sstr.str());
			
			INV ::SetName("INV");  INV ::SetDescription("Invert");
			SSS ::SetName("SSS");  SSS ::SetDescription("Source Signal Select");
		}
		
		void Reset()
		{
			 this->Initialize(0x0);
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	// GPIO Pad Data Out Register (SIUL2_GPDOn)
	struct SIUL2_GPDO : SIUL2_Register<SIUL2_GPDO, 8, SW_RW>
	{
		typedef SIUL2_Register<SIUL2_GPDO, 8, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x1300;
		
		struct PDO : Field<SIUL2_GPDO, PDO, 7> {}; // Pad Data Out
		
		typedef FieldSet<PDO> ALL;
		
		SIUL2_GPDO() : Super(0), reg_num(0) {}
		SIUL2_GPDO(SIUL2<CONFIG> *_siul2) : Super(_siul2), reg_num(0) {}
		SIUL2_GPDO(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, SIUL2<CONFIG> *_siul2)
		{
			this->siul2 = _siul2;
			reg_num = _reg_num;

			std::stringstream name_sstr;
			name_sstr << "SIUL2_GPDO" << reg_num;
			this->SetName(name_sstr.str()); this->SetDescription("GPIO Pad Data Out Register");
			
			PDO::SetName("PDO"); PDO::SetDescription("Pad Data Out");
		}
		
		void Reset()
		{
			 this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(const uint8_t& value, const uint8_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				Mirror();
			}
			
			return rws;
		}
		
		virtual void DebugWrite(const uint8_t& value, const uint8_t& bit_enable)
		{
			Super::DebugWrite(value, bit_enable);
			Mirror();
		}
		
		void Mirror()
		{
			unsigned int pgpdo_reg_num = reg_num / 16;
			unsigned int pgpdo_bit_ofs = 15 - (reg_num % 16);
			SIUL2_PGPDO& siul2_pgpdo = this->siul2->siul2_pgpdo[pgpdo_reg_num];
			siul2_pgpdo.Set(pgpdo_bit_ofs, this->template Get<PDO>());
			
			this->siul2->UpdateOutputBuffer(reg_num);
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	// GPIO Pad Data In Register (SIUL2_GPDIn)
	struct SIUL2_GPDI : SIUL2_Register<SIUL2_GPDI, 8, SW_R>
	{
		typedef SIUL2_Register<SIUL2_GPDI, 8, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x1500;
		
		struct PDI : Field<SIUL2_GPDI, PDI, 7> {}; // Pad Data In
		
		typedef FieldSet<PDI> ALL;
		
		SIUL2_GPDI() : Super(0), reg_num(0) {}
		SIUL2_GPDI(SIUL2<CONFIG> *_siul2) : Super(_siul2), reg_num(0) {}
		SIUL2_GPDI(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, SIUL2<CONFIG> *_siul2)
		{
			this->siul2 = _siul2;
			reg_num = _reg_num;

			std::stringstream name_sstr;
			name_sstr << "SIUL2_GPDI" << reg_num;
			this->SetName(name_sstr.str()); this->SetDescription("GPIO Pad Data In Register");
			
			PDI::SetName("PDI"); PDI::SetDescription("Pad Data In");
		}
		
		void Reset()
		{
			 this->Initialize(0x0);
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	// Parallel GPIO Pad Data Out Register (SIUL2_PGPDOn)
	struct SIUL2_PGPDO : SIUL2_Register<SIUL2_PGPDO, 16, SW_RW>
	{
		typedef SIUL2_Register<SIUL2_PGPDO, 16, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x1700;
		
		struct PPDO : Field<SIUL2_PGPDO, PPDO, 0, 15> {}; // Parallel Pad Data Out
		
		typedef FieldSet<PPDO> ALL;
		
		SIUL2_PGPDO() : Super(0), reg_num(0) {}
		SIUL2_PGPDO(SIUL2<CONFIG> *_siul2) : Super(_siul2), reg_num(0) {}
		SIUL2_PGPDO(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, SIUL2<CONFIG> *_siul2)
		{
			this->siul2 = _siul2;
			reg_num = _reg_num;

			std::stringstream name_sstr;
			name_sstr << "SIUL2_PGPDO" << reg_num;
			this->SetName(name_sstr.str()); this->SetDescription("Parallel GPIO Pad Data Out Register");
			
			PPDO::SetName("PPDO"); PPDO::SetDescription("Parallel Pad Data Out");
		}
		
		void Reset()
		{
			 this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(const uint16_t& value, const uint16_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!ReadWriteStatus(rws))
			{
				Mirror();
			}
			
			return rws;
		}
		
		virtual void DebugWrite(const uint16_t& value, const uint16_t& bit_enable)
		{
			Super::DebugWrite(value, bit_enable);
			Mirror();
		}
		
		void Mirror()
		{
			unsigned int i;
			for(i = 0; i < 16; i++)
			{
				unsigned int gpdo_reg_num = (reg_num * 16) + i;
				SIUL2_GPDO& siul2_gpdo = this->siul2->siul2_gpdo[gpdo_reg_num];
				siul2_gpdo.template Set<typename SIUL2_GPDO::PDO>(this->Get(i));
				this->siul2->UpdateOutputBuffer(gpdo_reg_num);
			}
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	// Parallel GPIO Pad Data In Register (SIUL2_PGPDIn)
	struct SIUL2_PGPDI : SIUL2_Register<SIUL2_PGPDI, 16, SW_R>
	{
		typedef SIUL2_Register<SIUL2_PGPDI, 16, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x1740;
		
		struct PPDI : Field<SIUL2_PGPDI, PPDI, 0, 15> {}; // Parallel Pad Data In
		
		typedef FieldSet<PPDI> ALL;
		
		SIUL2_PGPDI() : Super(0), reg_num(0) {}
		SIUL2_PGPDI(SIUL2<CONFIG> *_siul2) : Super(_siul2), reg_num(0) {}
		SIUL2_PGPDI(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, SIUL2<CONFIG> *_siul2)
		{
			this->siul2 = _siul2;
			reg_num = _reg_num;

			std::stringstream name_sstr;
			name_sstr << "SIUL2_PGPDI" << reg_num;
			this->SetName(name_sstr.str()); this->SetDescription("Parallel GPIO Pad Data In Register");
			
			PPDI::SetName("PPDI"); PPDI::SetDescription("Parallel Pad Data In");
		}
		
		void Reset()
		{
			 this->Initialize(0x0);
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};

	// Masked Parallel GPIO Pad Data Out Register (SIUL2_MPGPDOn)
	struct SIUL2_MPGPDO : SIUL2_Register<SIUL2_MPGPDO, 32, SW_RW>
	{
		typedef SIUL2_Register<SIUL2_MPGPDO, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x1780;
		
		struct MASK  : Field<SIUL2_MPGPDO, MASK , 0 , 15> {}; // Mask Field
		struct MPPDO : Field<SIUL2_MPGPDO, MPPDO, 16, 31> {}; // Masked Parallel Pad Data Out
		
		typedef FieldSet<MPPDO> ALL;
		
		SIUL2_MPGPDO() : Super(0), reg_num(0) {}
		SIUL2_MPGPDO(SIUL2<CONFIG> *_siul2) : Super(_siul2), reg_num(0) {}
		SIUL2_MPGPDO(SIUL2<CONFIG> *_siul2, uint32_t value) : Super(_siul2, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, SIUL2<CONFIG> *_siul2)
		{
			this->siul2 = _siul2;
			reg_num = _reg_num;

			std::stringstream name_sstr;
			name_sstr << "SIUL2_MPGPDO" << reg_num;
			this->SetName(name_sstr.str()); this->SetDescription("Masked Parallel GPIO Pad Data Out Register");
			
			MASK ::SetName("MASK");  MASK ::SetDescription("Mask Field");
			MPPDO::SetName("MPPDO"); MPPDO::SetDescription("Masked Parallel Pad Data Out");
		}
		
		void Reset()
		{
			 this->Initialize(0x0);
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	unisim::kernel::tlm2::ClockPropertiesProxy m_clk_prop_proxy;    // proxy to get clock properties from master clock port

	// SIUL2 registers
	SIUL2_MIDR1                                                                                     siul2_midr1;
	SIUL2_MIDR2                                                                                     siul2_midr2;
	SIUL2_DISR0                                                                                     siul2_disr0;
	SIUL2_DIRER0                                                                                    siul2_direr0;
	SIUL2_DIRSR0                                                                                    siul2_dirsr0;
	SIUL2_IREER0                                                                                    siul2_ireer0;
	SIUL2_IFEER0                                                                                    siul2_ifeer0;
	SIUL2_IFER0                                                                                     siul2_ifer0;
	AddressableRegisterFile<SIUL2_IFMCR, NUM_INTERRUPT_FILTERS, SIUL2<CONFIG> >                     siul2_ifmcr;
	SIUL2_SCR0                                                                                      siul2_scr0;
	AddressableRegisterFile<SIUL2_MSCR_IO, NUM_MULTIPLEXED_SIGNALS_CONFIGURATIONS, SIUL2<CONFIG> >  siul2_mscr_io;
	AddressableRegisterFile<SIUL2_MSCR_MUX, NUM_MULTIPLEXED_SIGNALS_CONFIGURATIONS, SIUL2<CONFIG> > siul2_mscr_mux;
	AddressableRegisterFile<SIUL2_GPDO, NUM_PADS, SIUL2<CONFIG> >                                   siul2_gpdo;
	AddressableRegisterFile<SIUL2_GPDI, NUM_PADS, SIUL2<CONFIG> >                                   siul2_gpdi;
	AddressableRegisterFile<SIUL2_PGPDO, MAX_PADS / 16, SIUL2<CONFIG> >                             siul2_pgpdo;
	AddressableRegisterFile<SIUL2_PGPDI, MAX_PADS / 16, SIUL2<CONFIG> >                             siul2_pgpdi;
	AddressableRegisterFile<SIUL2_MPGPDO, MAX_PADS / 16, SIUL2<CONFIG> >                            siul2_mpgpdo;
	
	// SIUL2 registers address map
	RegisterAddressMap<sc_dt::uint64> reg_addr_map;

	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	sc_core::sc_vector<sc_core::sc_event> pad_out_event;

	unisim::kernel::tlm2::Schedule<Event> schedule;         // Payload (processor requests over AHB interface) schedule
	
	bool input_buffers[NUM_PADS];
	bool output_buffers[NUM_PADS];
	
	unisim::util::endian::endian_type endian;
	unisim::kernel::variable::Parameter<unisim::util::endian::endian_type> param_endian;
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;

	sc_core::sc_time master_clock_period;                 // Master clock period
	sc_core::sc_time master_clock_start_time;             // Master clock start time
	bool master_clock_posedge_first;                      // Master clock posedge first ?
	double master_clock_duty_cycle;                       // Master clock duty cycle
	
	void Reset();
	void UpdateOutputBuffer(unsigned int pad_num);
	void UpdateInputBuffer(unsigned int pad_num);
	void UpdateGPI(unsigned int pad_num);
	void UpdatePadOut(unsigned int pad_num);

	void UpdateMasterClock();
	void MasterClockPropertiesChangedProcess();
	
	void ProcessEvent(Event *event);
	void ProcessEvents();
	void Process();
	void PAD_In_Process(unsigned int pad_num);
	void PAD_Out_Process(unsigned int pad_num);
};

} // end of namespace siul2
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_SIUL2_SIUL2_HH__
