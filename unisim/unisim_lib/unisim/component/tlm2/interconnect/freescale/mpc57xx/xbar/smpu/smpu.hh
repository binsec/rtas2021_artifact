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

#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_XBAR_SMPU_SMPU_HH__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_XBAR_SMPU_SMPU_HH__

#include <unisim/util/debug/simple_register_registry.hh>

#define SWITCH_ENUM_TRAIT(ENUM_TYPE, CLASS_NAME) template <ENUM_TYPE, bool __SWITCH_TRAIT_DUMMY__ = true> struct CLASS_NAME {}
#define CASE_ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) template <bool __SWITCH_TRAIT_DUMMY__> struct CLASS_NAME<ENUM_VALUE, __SWITCH_TRAIT_DUMMY__>
#define ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) CLASS_NAME<ENUM_VALUE>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace freescale {
namespace mpc57xx {
namespace xbar {

template <typename CONFIG>
class XBAR;

namespace smpu {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

using unisim::util::reg::core::AddressableRegister;
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
using unisim::util::reg::core::IsReadWriteError;

using unisim::component::tlm2::interconnect::generic_router::MemoryRegion;

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
	static const unsigned int BUSWIDTH = 32;
	static const unsigned int NUM_REGION_DESCRIPTORS = 8;
	static const unsigned int NUM_BUS_MASTERS        = 16;
};
#endif

template <typename CONFIG>
class SMPU
	: public unisim::kernel::Service<typename unisim::service::interfaces::Registers>
{
public:
	typedef unisim::component::tlm2::interconnect::freescale::mpc57xx::xbar::XBAR<CONFIG> XBAR;
	typedef typename XBAR::transaction_type transaction_type;
	
	static const unsigned int TLM2_IP_VERSION_MAJOR = 1;
	static const unsigned int TLM2_IP_VERSION_MINOR = 0;
	static const unsigned int TLM2_IP_VERSION_PATCH = 0;
	static const unsigned int NUM_REGION_DESCRIPTORS = CONFIG::NUM_REGION_DESCRIPTORS;
	static const unsigned int NUM_BUS_MASTERS        = CONFIG::NUM_BUS_MASTERS;
	
	// services
	unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;

	SMPU(const char *name, XBAR *xbar);
	virtual ~SMPU();

	void Reset();
	void end_of_elaboration();
	
	bool Check(transaction_type& trans, MemoryRegion *mem_rgn);
	
	//////////////// unisim::service::interface::Registers ////////////////////
	
	virtual unisim::service::interfaces::Register *GetRegister(const char *name);
	virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);
	
protected:
	XBAR *xbar;
	
	unisim::kernel::logger::Logger logger;
	
	// Common SMPU register representation
	template <typename REGISTER, Access _ACCESS>
	struct SMPU_Register : AddressableRegister<REGISTER, 32, _ACCESS>
	{
		typedef AddressableRegister<REGISTER, 32, _ACCESS> Super;
		
		SMPU_Register(SMPU<CONFIG> *_smpu) : Super(), smpu(_smpu) {}
		SMPU_Register(SMPU<CONFIG> *_smpu, uint32_t value) : Super(value), smpu(_smpu) {}

		inline bool IsVerboseRead() const ALWAYS_INLINE { return smpu->verbose; }
		inline bool IsVerboseWrite() const ALWAYS_INLINE { return smpu->verbose; }
		inline std::ostream& GetInfoStream() ALWAYS_INLINE { return smpu->logger.DebugInfoStream(); }

		using Super::operator =;
		
	protected:
		SMPU<CONFIG> *smpu;
	};
	
	// Control/Error Status Register 0 (SMPU_CESR0)
	struct SMPU_CESR0 : SMPU_Register<SMPU_CESR0, SW_RW>
	{
		typedef SMPU_Register<SMPU_CESR0, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;
		
		struct MERR     : Field<SMPU_CESR0, MERR    , 0 , 15, SW_R_W1C> {}; // Master n error
		struct Reserved : Field<SMPU_CESR0, Reserved, 16, 16, SW_R    > {}; // Reserved
		struct HRL      : Field<SMPU_CESR0, HRL     , 28, 30, SW_R    > {}; // Hardware revision level
		struct GVLD     : Field<SMPU_CESR0, GVLD    , 31              > {}; // Global Valid
		
		typedef FieldSet<MERR, Reserved, HRL, GVLD> ALL;
		
		SMPU_CESR0(SMPU<CONFIG> *_smpu) : Super(_smpu) { Init(); }
		SMPU_CESR0(SMPU<CONFIG> *_smpu, uint32_t value) : Super(_smpu, value) { Init(); }
		
		void Init()
		{
			this->SetName("SMPU_CESR0"); this->SetDescription("Control/Error Status Register 0");
			MERR    ::SetName("MERR");         MERR::SetDescription("Master n error");
			Reserved::SetName("Reserved"); Reserved::SetDescription("Reserved");
			HRL     ::SetName("HRL");          HRL ::SetDescription("Hardware revision level");
			GVLD    ::SetName("GVLD");         GVLD::SetDescription("Global Valid");
		}
		
		void Reset()
		{
			this->Initialize(0x00008002);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			this->smpu->DMI_Invalidate();
			
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				// MEOVR=MEOVR & MERR (clear bits MEOVR that are zeroed in MERR)
				this->smpu->smpu_cesr1.template Set<typename SMPU_CESR1::MEOVR>(this->smpu->smpu_cesr1.template Get<typename SMPU_CESR1::MEOVR>() & this->template Get<MERR>());
			}
			
			return rws;
		}
		
		using Super::operator =;
	};
	
	// Control/Error Status Register 1 (SMPU_CESR1)
	struct SMPU_CESR1 : SMPU_Register<SMPU_CESR1, SW_R>
	{
		typedef SMPU_Register<SMPU_CESR1, SW_R> Super;

		static const sc_dt::uint64 ADDRESS_OFFSET = 0x4;

		struct MEOVR : Field<SMPU_CESR1, MEOVR, 0, 15, SW_R> {}; // Master n error overrun
		struct NRGD  : Field<SMPU_CESR1, NRGD, 28, 31, SW_R> {}; // Number of region descriptors
		
		typedef FieldSet<MEOVR, NRGD> ALL;
		
		SMPU_CESR1(SMPU<CONFIG> *_smpu) : Super(_smpu) { Init(); }
		SMPU_CESR1(SMPU<CONFIG> *_smpu, uint32_t value) : Super(_smpu, value) { Init(); }
		
		void Init()
		{
			this->SetName("SMPU_CESR1"); this->SetDescription("Control/Error Status Register 1");
			MEOVR::SetName("MEOVR"); MEOVR::SetDescription("Master n error overrun");
			NRGD ::SetName("NRGD");  NRGD ::SetDescription("Number of region descriptors");
		};
		
		void Reset()
		{
			uint32_t reset_value = 0x00008000;
			NRGD::template Set<uint32_t>(reset_value, NUM_REGION_DESCRIPTORS / 4);
			this->Initialize(reset_value);
		}
		
		using Super::operator =;
	};
	
	// Error Address Register, Bus Master n (SMPU_EARn)
	struct SMPU_EAR : SMPU_Register<SMPU_EAR, SW_R>
	{
		typedef SMPU_Register<SMPU_EAR, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x100;

		struct EADDR : Field<SMPU_EAR, EADDR, 0, 31> {}; // Error address
		
		typedef FieldSet<EADDR> ALL;
		
		SMPU_EAR() : Super(0), reg_num(0) {}
		SMPU_EAR(SMPU<CONFIG> *_smpu) : Super(_smpu) {}
		SMPU_EAR(SMPU<CONFIG> *_smpu, uint32_t value) : Super(_smpu, value) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, SMPU<CONFIG> *_smpu)
		{
			this->smpu = _smpu;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "SMPU_EAR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "Error Address Register, Bus Master " << reg_num;
			this->SetDescription(name_sstr.str());
			
			EADDR::SetName("EADDR");
			EADDR::SetDescription("Error address");
		}

		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	// Error Detail Register, Bus Master n (SMPU_EDRn)
	struct SMPU_EDR : SMPU_Register<SMPU_EDR, SW_R>
	{
		typedef SMPU_Register<SMPU_EDR, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x104;

		struct EACD              : Field<SMPU_EDR, EACD             , 0, 23 > {}; // Error access control detail
		struct EATTR             : Field<SMPU_EDR, EATTR            , 25, 26> {}; // Error attributes
		struct EATTR_PRIVILEGED  : Field<SMPU_EDR, EATTR_PRIVILEGED , 25    > {}; // Privileged
		struct EATTR_DATA_ACCESS : Field<SMPU_EDR, EATTR_DATA_ACCESS, 26    > {}; // Data Access
		struct ERW               : Field<SMPU_EDR, ERW              , 27    > {}; // Error read/write
		struct EMN               : Field<SMPU_EDR, EMN              , 28, 31> {}; // Error master number
		
		typedef FieldSet<EACD, EATTR, ERW, EMN> ALL;
		
		SMPU_EDR() : Super(0), reg_num(0) {}
		SMPU_EDR(SMPU<CONFIG> *_smpu) : Super(_smpu), reg_num(0) {}
		SMPU_EDR(SMPU<CONFIG> *_smpu, uint32_t value) : Super(_smpu, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, SMPU<CONFIG> *_smpu)
		{
			this->smpu = _smpu;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "SMPU_EDR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "Error Detail Register, Bus Master " << reg_num;
			this->SetDescription(name_sstr.str());
			
			EACD ::SetName("EACD");
			EATTR::SetName("EATTR");
			ERW  ::SetName("ERW");
			EMN  ::SetName("EMN");
		}

		void Reset()
		{
			this->Initialize(0x00000080);
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	// Region Descriptor n, Word 0 (SMPU_RGDn_WORD0)
	struct SMPU_RGD_WORD0 : SMPU_Register<SMPU_RGD_WORD0, SW_RW>
	{
		typedef SMPU_Register<SMPU_RGD_WORD0, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;

		struct SRTADDR : Field<SMPU_RGD_WORD0, SRTADDR, 0, 31> {}; // Start address
		
		typedef FieldSet<SRTADDR> ALL;
		
		SMPU_RGD_WORD0(SMPU<CONFIG> *_smpu, unsigned int region_descriptor_num) : Super(_smpu) { Init(region_descriptor_num); }
		
		void Init(unsigned int region_descriptor_num)
		{
			this->reg_num = region_descriptor_num;
			std::stringstream name_sstr;
			name_sstr << "SMPU_RGD" << region_descriptor_num << "_WORD0";
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "Region Descriptor " << region_descriptor_num << ", Word 0";
			this->SetDescription(name_sstr.str());
			
			SRTADDR::SetName("SRTADDR");
			SRTADDR::SetDescription("Start address");
		}

		void Reset()
		{
			this->Initialize(0x0);
		}

		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			if(this->smpu->smpu_rgd[reg_num].smpu_rgd_word3.template Get<typename SMPU_RGD_WORD3::RO>()) return RWS_WROR;
			  
			this->smpu->DMI_Invalidate();
			
			return Super::Write(value, bit_enable);
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	// Region Descriptor n, Word 1 (SMPU_RGDn_WORD1)
	struct SMPU_RGD_WORD1 : SMPU_Register<SMPU_RGD_WORD1, SW_RW>
	{
		typedef SMPU_Register<SMPU_RGD_WORD1, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x4;

		struct ENDADDR : Field<SMPU_RGD_WORD1, ENDADDR, 0, 31> {}; // End address
		
		typedef FieldSet<ENDADDR> ALL;
		
		SMPU_RGD_WORD1(SMPU<CONFIG> *_smpu, unsigned int region_descriptor_num) : Super(_smpu) { Init(region_descriptor_num); }
		
		void Init(unsigned int region_descriptor_num)
		{
			this->reg_num = region_descriptor_num;
			std::stringstream name_sstr;
			name_sstr << "SMPU_RGD" << region_descriptor_num << "_WORD1";
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "Region Descriptor " << region_descriptor_num << ", Word 1";
			this->SetDescription(name_sstr.str());
			
			ENDADDR::SetName("ENDADDR");
			ENDADDR::SetDescription("End address");
		}

		void Reset()
		{
			this->Initialize(0x0);
		}

		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			if(this->smpu->smpu_rgd[reg_num].smpu_rgd_word3.template Get<typename SMPU_RGD_WORD3::RO>()) return RWS_WROR;
			  
			this->smpu->DMI_Invalidate();
			
			return Super::Write(value, bit_enable);
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	// Region Descriptor n, Word 1 (SMPU_RGDn_WORD2_FMT0)
	struct SMPU_RGD_WORD2_FMT0 : SMPU_Register<SMPU_RGD_WORD2_FMT0, SW_RW>
	{
		typedef SMPU_Register<SMPU_RGD_WORD2_FMT0, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x8;
		
		struct BusMasterPermission
		{
			struct R : unisim::util::reg::core::Field<R, 1> {}; // read
			struct W : unisim::util::reg::core::Field<R, 0> {}; // write
		};

		struct M0P  : Field<SMPU_RGD_WORD2_FMT0, M0P ,  0,  1> {}; // Bus master 0 permissions
		struct M1P  : Field<SMPU_RGD_WORD2_FMT0, M1P ,  2,  3> {}; // Bus master 1 permissions
		struct M2P  : Field<SMPU_RGD_WORD2_FMT0, M2P ,  4,  5> {}; // Bus master 2 permissions
		struct M3P  : Field<SMPU_RGD_WORD2_FMT0, M3P ,  6,  7> {}; // Bus master 3 permissions
		struct M4P  : Field<SMPU_RGD_WORD2_FMT0, M4P ,  8,  9> {}; // Bus master 4 permissions
		struct M5P  : Field<SMPU_RGD_WORD2_FMT0, M5P , 10, 11> {}; // Bus master 5 permissions
		struct M6P  : Field<SMPU_RGD_WORD2_FMT0, M6P , 12, 13> {}; // Bus master 6 permissions
		struct M7P  : Field<SMPU_RGD_WORD2_FMT0, M7P , 14, 15> {}; // Bus master 7 permissions
		struct M8P  : Field<SMPU_RGD_WORD2_FMT0, M8P , 16, 17> {}; // Bus master 8 permissions
		struct M9P  : Field<SMPU_RGD_WORD2_FMT0, M9P , 18, 19> {}; // Bus master 9 permissions
		struct M10P : Field<SMPU_RGD_WORD2_FMT0, M10P, 20, 21> {}; // Bus master 10 permissions
		struct M11P : Field<SMPU_RGD_WORD2_FMT0, M11P, 22, 23> {}; // Bus master 11 permissions
		struct M12P : Field<SMPU_RGD_WORD2_FMT0, M12P, 24, 25> {}; // Bus master 12 permissions
		struct M13P : Field<SMPU_RGD_WORD2_FMT0, M13P, 26, 27> {}; // Bus master 13 permissions
		struct M14P : Field<SMPU_RGD_WORD2_FMT0, M14P, 28, 29> {}; // Bus master 14 permissions
		struct M15P : Field<SMPU_RGD_WORD2_FMT0, M15P, 30, 31> {}; // Bus master 15 permissions
		
		typedef FieldSet<M0P, M1P, M2P, M3P, M4P, M5P, M6P, M7P, M8P, M9P, M10P, M11P, M12P, M13P, M14P, M15P> ALL;
		
		SMPU_RGD_WORD2_FMT0(SMPU<CONFIG> *_smpu, unsigned int region_descriptor_num) : Super(_smpu) { Init(region_descriptor_num); }
		
		void Init(unsigned int region_descriptor_num)
		{
			this->reg_num = region_descriptor_num;
			std::stringstream name_sstr;
			name_sstr << "SMPU_RGD" << region_descriptor_num << "_WORD2_FMT0";
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "Region Descriptor " << region_descriptor_num << ", Word 2 Format 0";
			this->SetDescription(name_sstr.str());

			M0P ::SetName("M0P");  M0P ::SetDescription("Bus master 0 permissions");
			M1P ::SetName("M1P");  M1P ::SetDescription("Bus master 1 permissions");
			M2P ::SetName("M2P");  M2P ::SetDescription("Bus master 2 permissions");
			M3P ::SetName("M3P");  M3P ::SetDescription("Bus master 3 permissions");
			M4P ::SetName("M4P");  M4P ::SetDescription("Bus master 4 permissions");
			M5P ::SetName("M5P");  M5P ::SetDescription("Bus master 5 permissions");
			M6P ::SetName("M6P");  M6P ::SetDescription("Bus master 6 permissions");
			M7P ::SetName("M7P");  M7P ::SetDescription("Bus master 7 permissions");
			M8P ::SetName("M8P");  M8P ::SetDescription("Bus master 8 permissions");
			M9P ::SetName("M9P");  M9P ::SetDescription("Bus master 9 permissions");
			M10P::SetName("M10P"); M10P::SetDescription("Bus master 10 permissions");
			M11P::SetName("M11P"); M11P::SetDescription("Bus master 11 permissions");
			M12P::SetName("M12P"); M12P::SetDescription("Bus master 12 permissions");
			M13P::SetName("M13P"); M13P::SetDescription("Bus master 13 permissions");
			M14P::SetName("M14P"); M14P::SetDescription("Bus master 14 permissions");
			M15P::SetName("M15P"); M15P::SetDescription("Bus master 15 permissions");
		}

		void Reset()
		{
			this->Initialize(0x0);
		}

		bool IsReadAllowed(int bus_master) const { return ((this->Get() >> (2 * (15 - bus_master))) & BusMasterPermission::R::template GetMask<uint32_t>()) != 0; }
		bool IsWriteAllowed(int bus_master) const { return ((this->Get() >> (2 * (15 - bus_master))) & BusMasterPermission::W::template GetMask<uint32_t>()) != 0; }
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			if(this->smpu->smpu_rgd[reg_num].smpu_rgd_word3.template Get<typename SMPU_RGD_WORD3::RO>()) return RWS_WROR;
			  
			this->smpu->DMI_Invalidate();
			
			return Super::Write(value, bit_enable);
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};

	// Region Descriptor n, Word 1 (SMPU_RGDn_WORD3)
	struct SMPU_RGD_WORD3 : SMPU_Register<SMPU_RGD_WORD3, SW_RW>
	{
		typedef SMPU_Register<SMPU_RGD_WORD3, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xc;

		struct FMT : Field<SMPU_RGD_WORD3, FMT, 27, 27, SW_R> {}; // Region Descriptor Format
		struct RO  : Field<SMPU_RGD_WORD3, RO , 28> {}; // Read-Only
		struct CI  : Field<SMPU_RGD_WORD3, CI , 30> {}; // Cache Inhibit
		struct VLD : Field<SMPU_RGD_WORD3, VLD, 31> {}; // Valid
		
		typedef FieldSet<FMT, RO, CI, VLD> ALL;
		
		SMPU_RGD_WORD3(SMPU<CONFIG> *_smpu, unsigned int region_descriptor_num) : Super(_smpu) { Init(region_descriptor_num); }
		
		void Init(unsigned int region_descriptor_num)
		{
			this->reg_num = region_descriptor_num;
			std::stringstream name_sstr;
			name_sstr << "SMPU_RGD" << region_descriptor_num << "_WORD3";
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "Region Descriptor " << region_descriptor_num << ", Word 3";
			this->SetDescription(name_sstr.str());
			
			FMT::SetName("FMT"); FMT::SetDescription("Region Descriptor Format");
			RO ::SetName("RO");  RO ::SetDescription("Read-Only");
			CI ::SetName("CI");  CI ::SetDescription("Cache Inhibit");
			VLD::SetName("VLD"); VLD::SetDescription("Valid");
		}

		void Reset()
		{
			this->Initialize(0x0);
		}

		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			if(this->smpu->smpu_rgd[reg_num].smpu_rgd_word3.template Get<typename SMPU_RGD_WORD3::RO>()) return RWS_WROR;
			  
			this->smpu->DMI_Invalidate();
			
			return Super::Write(value, bit_enable);
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	struct RegionDescriptor
	{
		static const unsigned int SIZE = 16;
		
		RegionDescriptor(SMPU<CONFIG> *_smpu, unsigned int _region_descriptor_num)
			: smpu_rgd_word0(_smpu, _region_descriptor_num)
			, smpu_rgd_word1(_smpu, _region_descriptor_num)
			, smpu_rgd_word2_fmt0(_smpu, _region_descriptor_num)
			, smpu_rgd_word3(_smpu, _region_descriptor_num)
			, region_descriptor_num(_region_descriptor_num)
		{
		}
		
		void Reset()
		{
			smpu_rgd_word0.Reset();
			smpu_rgd_word1.Reset();
			smpu_rgd_word2_fmt0.Reset();
			smpu_rgd_word3.Reset();
		}
		
		bool IsValid() const { return smpu_rgd_word3.template Get<typename SMPU_RGD_WORD3::VLD>(); }
		uint32_t GetStartAddress() const { return smpu_rgd_word0.template Get<typename SMPU_RGD_WORD0::SRTADDR>(); }
		uint32_t GetEndAddress() const { return smpu_rgd_word1.template Get<typename SMPU_RGD_WORD1::ENDADDR>(); }
		bool IsReadAllowed(int bus_master) const { return smpu_rgd_word2_fmt0.IsReadAllowed(bus_master); }
		bool IsWriteAllowed(int bus_master) const { return smpu_rgd_word2_fmt0.IsWriteAllowed(bus_master); }

		RegionDescriptor   *next;
		SMPU_RGD_WORD0      smpu_rgd_word0;      // SMPU_RGD_WORD0
		SMPU_RGD_WORD1      smpu_rgd_word1;      // SMPU_RGD_WORD1
		SMPU_RGD_WORD2_FMT0 smpu_rgd_word2_fmt0; // SMPU_RGD_WORD2_FMT0
		SMPU_RGD_WORD3      smpu_rgd_word3;      // SMPU_RGD_WORD3
		unsigned int        region_descriptor_num;
	};
	
	struct RegionDescriptorFile
	{
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x400;
		
		RegionDescriptorFile(SMPU<CONFIG> *smpu)
			: region_descriptors()
		{
			RegionDescriptor *next_region_descriptor = 0;
			int region_descriptor_num;
			for(region_descriptor_num = NUM_REGION_DESCRIPTORS - 1; region_descriptor_num >= 0; region_descriptor_num--)
			{
				RegionDescriptor *region_descriptor = new RegionDescriptor(smpu, region_descriptor_num);
				region_descriptors[region_descriptor_num] = region_descriptor;
				region_descriptor->next = next_region_descriptor;
				next_region_descriptor = region_descriptor;
			}
			mru_region_descriptor = next_region_descriptor;
		}
		
		~RegionDescriptorFile()
		{
			unsigned int region_descriptor_num;
			for(region_descriptor_num = 0; region_descriptor_num < NUM_REGION_DESCRIPTORS; region_descriptor_num++)
			{
				delete region_descriptors[region_descriptor_num];
			}
		}
		
		void Reset()
		{
			unsigned int region_descriptor_num;
			for(region_descriptor_num = 0; region_descriptor_num < NUM_REGION_DESCRIPTORS; region_descriptor_num++)
			{
				region_descriptors[region_descriptor_num].Reset();
			}
		}
		
		RegionDescriptor& operator [] (unsigned int region_descriptor_num) { return *region_descriptors[region_descriptor_num]; }
		
		RegionDescriptor *mru_region_descriptor;
		RegionDescriptor *region_descriptors[NUM_REGION_DESCRIPTORS];
	};

	// SMPU Registers
	SMPU_CESR0                                                                          smpu_cesr0;          // SMPU_CESR0
	SMPU_CESR1                                                                          smpu_cesr1;          // SMPU_CESR1
	AddressableRegisterFile<SMPU_EAR           , NUM_BUS_MASTERS       , SMPU<CONFIG> > smpu_ear;            // SMPU_EARn
	AddressableRegisterFile<SMPU_EDR           , NUM_BUS_MASTERS       , SMPU<CONFIG> > smpu_edr;            // SMPU_EDRn
	RegionDescriptorFile                                                                smpu_rgd;            // SMPU_RGDn_WORD0, SMPU_RGDn_WORD1, SMPU_RGDn_WORD2_FMT0, SMPU_RGDn_WORD3 
	
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	
	void DMI_Invalidate();
};

} // end of namespace smpu
} // end of namespace xbar
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_XBAR_SMPU_SMPU_HH__
