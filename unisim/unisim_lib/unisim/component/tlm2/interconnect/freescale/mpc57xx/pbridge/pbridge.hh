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

#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_PBRIDGE_PBRIDGE_HH__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_PBRIDGE_PBRIDGE_HH__

#include <unisim/component/tlm2/interconnect/programmable_router/router.hh>
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
namespace pbridge {

using unisim::component::tlm2::interconnect::generic_router::MemoryRegion;

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

using unisim::util::reg::core::AddressableRegister;
using unisim::util::reg::core::FieldSet;
using unisim::util::reg::core::AddressableRegisterFile;
using unisim::util::reg::core::Access;
using unisim::util::reg::core::SW_RW;
using unisim::util::reg::core::SW_R;
using unisim::util::reg::core::ReadWriteStatus;

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
};
#endif

inline std::string OrdinalToAlpha(unsigned int n)
{
	std::string s;
	
	do
	{
		s += 'A' + (n % 26);
		n /= 26;
	}
	while(n != 0);
	
	std::reverse(s.begin(), s.end());
	
	return s;
}

struct AccessControlRegisterMapping
{
	bool valid;
	bool off_platform;
	unsigned int reg_num;
	
	AccessControlRegisterMapping() : valid(false), off_platform(false), reg_num(0) {}
	int operator != (const AccessControlRegisterMapping& o) { return (valid != o.valid) || (off_platform != o.off_platform) || (reg_num != o.reg_num); }
	int operator == (const AccessControlRegisterMapping& o) { return (valid == o.valid) &&(off_platform == o.off_platform) && (reg_num == o.reg_num); }
	AccessControlRegisterMapping& operator = (const AccessControlRegisterMapping& o) { valid = o.valid; off_platform = o.off_platform; reg_num = o.reg_num; return *this; }
};

inline std::ostream& operator << (std::ostream& os, const AccessControlRegisterMapping& acrm)
{
	return os << ((acrm.off_platform) ? "o" : "") << "pacr" << std::dec << acrm.reg_num;
}

template <typename CONFIG>
class PBRIDGE
	: public unisim::component::tlm2::interconnect::programmable_router::Router<CONFIG>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Registers>
{
public:
	typedef unisim::component::tlm2::interconnect::programmable_router::Router<CONFIG> Super;
	typedef typename Super::transaction_type transaction_type;
	typedef typename Super::MAPPING MAPPING;
	
	static const unsigned int TLM2_IP_VERSION_MAJOR = 1;
	static const unsigned int TLM2_IP_VERSION_MINOR = 0;
	static const unsigned int TLM2_IP_VERSION_PATCH = 0;
	static const unsigned int NUM_MASTER_IDS = 16;
	static const bool threaded_model                = false;
	
	// services
	unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;

	PBRIDGE(const sc_core::sc_module_name& name, unisim::kernel::Object *parent);
	virtual ~PBRIDGE();

	//////////////// unisim::service::interface::Registers ////////////////////
	
	virtual unisim::service::interfaces::Register *GetRegister(const char *name);
	virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);

protected:
	virtual void end_of_elaboration();
	virtual void Reset();
	
	virtual bool ApplyMap(transaction_type &trans, MAPPING const *&applied_mapping, MemoryRegion *mem_rgn);

	// Common PBRIDGE register representation
	template <typename REGISTER, Access _ACCESS>
	struct PBRIDGE_Register : AddressableRegister<REGISTER, 32, _ACCESS>
	{
		typedef AddressableRegister<REGISTER, 32, _ACCESS> Super;
		
		PBRIDGE_Register(PBRIDGE<CONFIG> *_pbridge) : Super(), pbridge(_pbridge) {}
		PBRIDGE_Register(PBRIDGE<CONFIG> *_pbridge, uint32_t value) : Super(value), pbridge(_pbridge) {}

		inline bool IsVerboseRead() const ALWAYS_INLINE { return pbridge->verbose; }
		inline bool IsVerboseWrite() const ALWAYS_INLINE { return pbridge->verbose; }
		inline std::ostream& GetInfoStream() ALWAYS_INLINE { return pbridge->logger.DebugInfoStream(); }

		using Super::operator =;
		
	protected:
		PBRIDGE<CONFIG> *pbridge;
	};
	
	struct A { static const unsigned int reg_num = 0; static const unsigned int field_num = 0; };
	struct B { static const unsigned int reg_num = 1; static const unsigned int field_num = 0; };
	struct C { static const unsigned int reg_num = 2; static const unsigned int field_num = 0; };
	struct D { static const unsigned int reg_num = 3; static const unsigned int field_num = 0; };
	struct E { static const unsigned int reg_num = 4; static const unsigned int field_num = 0; };
	struct F { static const unsigned int reg_num = 5; static const unsigned int field_num = 0; };
	struct G { static const unsigned int reg_num = 6; static const unsigned int field_num = 0; };
	struct H { static const unsigned int reg_num = 7; static const unsigned int field_num = 0; };
	struct I { static const unsigned int reg_num = 8; static const unsigned int field_num = 0; };
	struct J { static const unsigned int reg_num = 9; static const unsigned int field_num = 0; };
	struct K { static const unsigned int reg_num = 10; static const unsigned int field_num = 0; };
	struct L { static const unsigned int reg_num = 11; static const unsigned int field_num = 0; };
	struct M { static const unsigned int reg_num = 12; static const unsigned int field_num = 0; };
	struct N { static const unsigned int reg_num = 13; static const unsigned int field_num = 0; };
	struct O { static const unsigned int reg_num = 14; static const unsigned int field_num = 0; };
	struct P { static const unsigned int reg_num = 15; static const unsigned int field_num = 0; };
	struct Q { static const unsigned int reg_num = 16; static const unsigned int field_num = 0; };
	struct R { static const unsigned int reg_num = 17; static const unsigned int field_num = 0; };
	struct S { static const unsigned int reg_num = 18; static const unsigned int field_num = 0; };
	struct T { static const unsigned int reg_num = 19; static const unsigned int field_num = 0; };
	struct U { static const unsigned int reg_num = 20; static const unsigned int field_num = 0; };
	struct V { static const unsigned int reg_num = 21; static const unsigned int field_num = 0; };
	struct W { static const unsigned int reg_num = 22; static const unsigned int field_num = 0; };
	struct X { static const unsigned int reg_num = 23; static const unsigned int field_num = 0; };
	struct Y { static const unsigned int reg_num = 24; static const unsigned int field_num = 0; };
	struct Z { static const unsigned int reg_num = 25; static const unsigned int field_num = 0; };
	struct AA { static const unsigned int reg_num = 26; static const unsigned int field_num = 0; };
	struct AB { static const unsigned int reg_num = 27; static const unsigned int field_num = 0; };
	struct AC { static const unsigned int reg_num = 28; static const unsigned int field_num = 0; };
	struct AD { static const unsigned int reg_num = 29; static const unsigned int field_num = 0; };
	struct AE { static const unsigned int reg_num = 30; static const unsigned int field_num = 0; };
	struct AF { static const unsigned int reg_num = 31; static const unsigned int field_num = 0; };
	
	// Master Privilege Register (PBRIDGE_MPR)
	template <typename Trait>
	struct PBRIDGE_MPR : PBRIDGE_Register<PBRIDGE_MPR<Trait>, SW_RW>
	{
		typedef PBRIDGE_Register<PBRIDGE_MPR<Trait>, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0 + (4 * Trait::reg_num);
		
		struct MPROT0
		{
			struct MTR : Field<PBRIDGE_MPR, MTR, 1> {}; // Master trusted for read
			struct MTW : Field<PBRIDGE_MPR, MTW, 2> {}; // Master trusted for write
			struct MPL : Field<PBRIDGE_MPR, MPL, 3> {}; // Master privilege level
		};
		
		struct MPROT1
		{
			struct MTR : Field<PBRIDGE_MPR, MTR, 5> {}; // Master trusted for read
			struct MTW : Field<PBRIDGE_MPR, MTW, 6> {}; // Master trusted for write
			struct MPL : Field<PBRIDGE_MPR, MPL, 7> {}; // Master privilege level
		};

		struct MPROT2
		{
			struct MTR : Field<PBRIDGE_MPR, MTR, 9 > {}; // Master trusted for read
			struct MTW : Field<PBRIDGE_MPR, MTW, 10> {}; // Master trusted for write
			struct MPL : Field<PBRIDGE_MPR, MPL, 11> {}; // Master privilege level
		};

		struct MPROT3
		{
			struct MTR : Field<PBRIDGE_MPR, MTR, 13> {}; // Master trusted for read
			struct MTW : Field<PBRIDGE_MPR, MTW, 14> {}; // Master trusted for write
			struct MPL : Field<PBRIDGE_MPR, MPL, 15> {}; // Master privilege level
		};

		struct MPROT4
		{
			struct MTR : Field<PBRIDGE_MPR, MTR, 17> {}; // Master trusted for read
			struct MTW : Field<PBRIDGE_MPR, MTW, 18> {}; // Master trusted for write
			struct MPL : Field<PBRIDGE_MPR, MPL, 19> {}; // Master privilege level
		};
		
		struct MPROT5
		{
			struct MTR : Field<PBRIDGE_MPR, MTR, 21> {}; // Master trusted for read
			struct MTW : Field<PBRIDGE_MPR, MTW, 22> {}; // Master trusted for write
			struct MPL : Field<PBRIDGE_MPR, MPL, 23> {}; // Master privilege level
		};

		struct MPROT6
		{
			struct MTR : Field<PBRIDGE_MPR, MTR, 25> {}; // Master trusted for read
			struct MTW : Field<PBRIDGE_MPR, MTW, 26> {}; // Master trusted for write
			struct MPL : Field<PBRIDGE_MPR, MPL, 27> {}; // Master privilege level
		};

		struct MPROT7
		{
			struct MTR : Field<PBRIDGE_MPR, MTR, 29> {}; // Master trusted for read
			struct MTW : Field<PBRIDGE_MPR, MTW, 30> {}; // Master trusted for write
			struct MPL : Field<PBRIDGE_MPR, MPL, 31> {}; // Master privilege level
		};

		typedef FieldSet< typename MPROT0::MTR, typename MPROT0::MTW, typename MPROT0::MPL
		                , typename MPROT1::MTR, typename MPROT1::MTW, typename MPROT1::MPL
		                , typename MPROT2::MTR, typename MPROT2::MTW, typename MPROT2::MPL
		                , typename MPROT3::MTR, typename MPROT3::MTW, typename MPROT3::MPL
		                , typename MPROT4::MTR, typename MPROT4::MTW, typename MPROT4::MPL
		                , typename MPROT5::MTR, typename MPROT5::MTW, typename MPROT5::MPL
		                , typename MPROT6::MTR, typename MPROT6::MTW, typename MPROT6::MPL
		                , typename MPROT7::MTR, typename MPROT7::MTW, typename MPROT7::MPL > ALL;
		
		PBRIDGE_MPR(PBRIDGE<CONFIG> *_pbridge) : Super(_pbridge) { Init(); }
		PBRIDGE_MPR(PBRIDGE<CONFIG> *_pbridge, uint32_t value) : Super(_pbridge, value) { Init(); }
		
		void Init()
		{
			std::string suffix(OrdinalToAlpha(Trait::reg_num));
			
			std::stringstream name_sstr;
			name_sstr << "PBRIDGE_MPR" << suffix;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "Master Privilege Register " << suffix;
			this->SetDescription(description_sstr.str());
			
			unsigned int base_field_num = 8 * Trait::reg_num;
			
			unsigned int i;
			for(i = 0; i < 8; i++)
			{
				unsigned int field_num = base_field_num + i;
				
				std::stringstream mprot_mtr_name_sstr;
				mprot_mtr_name_sstr << "MPROT" << field_num << "[MTR]";
				std::string mprot_mtr_name(mprot_mtr_name_sstr.str());
				
				std::stringstream mprot_mtr_description_sstr;
				mprot_mtr_description_sstr << "Master #" << field_num << " trusted for read";
				std::string mprot_mtr_description(mprot_mtr_description_sstr.str());
				
				std::stringstream mprot_mtw_name_sstr;
				mprot_mtw_name_sstr << "MPROT" << field_num << "[MTW]";
				std::string mprot_mtw_name(mprot_mtw_name_sstr.str());
				
				std::stringstream mprot_mtw_description_sstr;
				mprot_mtw_description_sstr << "Master #" << field_num << " trusted for write";
				std::string mprot_mtw_description(mprot_mtw_description_sstr.str());

				std::stringstream mprot_mpl_name_sstr;
				mprot_mpl_name_sstr << "MPROT" << field_num << "[MPL]";
				std::string mprot_mpl_name(mprot_mpl_name_sstr.str());
				
				std::stringstream mprot_mpl_description_sstr;
				mprot_mpl_description_sstr << "Master #" << field_num << " privilege level";
				std::string mprot_mpl_description(mprot_mpl_description_sstr.str());

				switch(i)
				{
					case 0:
						MPROT0::MTR::SetName(mprot_mtr_name); MPROT0::MTR::SetDescription(mprot_mtr_description);
						MPROT0::MTW::SetName(mprot_mtw_name); MPROT0::MTW::SetDescription(mprot_mtw_description);
						MPROT0::MPL::SetName(mprot_mpl_name); MPROT0::MPL::SetDescription(mprot_mpl_description);
						break;
					case 1:
						MPROT1::MTR::SetName(mprot_mtr_name); MPROT1::MTR::SetDescription(mprot_mtr_description);
						MPROT1::MTW::SetName(mprot_mtw_name); MPROT1::MTW::SetDescription(mprot_mtw_description);
						MPROT1::MPL::SetName(mprot_mpl_name); MPROT1::MPL::SetDescription(mprot_mpl_description);
						break;
					case 2:
						MPROT2::MTR::SetName(mprot_mtr_name); MPROT2::MTR::SetDescription(mprot_mtr_description);
						MPROT2::MTW::SetName(mprot_mtw_name); MPROT2::MTW::SetDescription(mprot_mtw_description);
						MPROT2::MPL::SetName(mprot_mpl_name); MPROT2::MPL::SetDescription(mprot_mpl_description);
						break;
					case 3:
						MPROT3::MTR::SetName(mprot_mtr_name); MPROT3::MTR::SetDescription(mprot_mtr_description);
						MPROT3::MTW::SetName(mprot_mtw_name); MPROT3::MTW::SetDescription(mprot_mtw_description);
						MPROT3::MPL::SetName(mprot_mpl_name); MPROT3::MPL::SetDescription(mprot_mpl_description);
						break;
					case 4:
						MPROT4::MTR::SetName(mprot_mtr_name); MPROT4::MTR::SetDescription(mprot_mtr_description);
						MPROT4::MTW::SetName(mprot_mtw_name); MPROT4::MTW::SetDescription(mprot_mtw_description);
						MPROT4::MPL::SetName(mprot_mpl_name); MPROT4::MPL::SetDescription(mprot_mpl_description);
						break;
					case 5:
						MPROT5::MTR::SetName(mprot_mtr_name); MPROT5::MTR::SetDescription(mprot_mtr_description);
						MPROT5::MTW::SetName(mprot_mtw_name); MPROT5::MTW::SetDescription(mprot_mtw_description);
						MPROT5::MPL::SetName(mprot_mpl_name); MPROT5::MPL::SetDescription(mprot_mpl_description);
						break;
					case 6:
						MPROT6::MTR::SetName(mprot_mtr_name); MPROT6::MTR::SetDescription(mprot_mtr_description);
						MPROT6::MTW::SetName(mprot_mtw_name); MPROT6::MTW::SetDescription(mprot_mtw_description);
						MPROT6::MPL::SetName(mprot_mpl_name); MPROT6::MPL::SetDescription(mprot_mpl_description);
						break;
					case 7:
						MPROT7::MTR::SetName(mprot_mtr_name); MPROT7::MTR::SetDescription(mprot_mtr_description);
						MPROT7::MTW::SetName(mprot_mtw_name); MPROT7::MTW::SetDescription(mprot_mtw_description);
						MPROT7::MPL::SetName(mprot_mpl_name); MPROT7::MPL::SetDescription(mprot_mpl_description);
						break;
				}
			}
		}
		
		void Reset()
		{
			this->Initialize(0x77777777ul);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			// modifying MPR may affect memory protection, so it's safer to invalidate SystemC TLM-2.0 direct memory pointers before modification
			this->pbridge->DMI_Invalidate();
			
			return Super::Write(value, bit_enable);
		}
	};

	// Peripheral Access Control Register (PBRIDGE_PACRn)
	template <typename Trait>
	struct PBRIDGE_PACR : PBRIDGE_Register<PBRIDGE_PACR<Trait>, SW_RW>
	{
		typedef PBRIDGE_Register<PBRIDGE_PACR<Trait>, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x100 + (4 * Trait::reg_num);
		
		struct PACR0
		{
			struct SP : Field<PBRIDGE_PACR, SP, 1> {}; // Supervisor protect
			struct WP : Field<PBRIDGE_PACR, WP, 2> {}; // Write protect
			struct TP : Field<PBRIDGE_PACR, TP, 3> {}; // Trusted protect
		};
		
		struct PACR1
		{
			struct SP : Field<PBRIDGE_PACR, SP, 5> {}; // Supervisor protect
			struct WP : Field<PBRIDGE_PACR, WP, 6> {}; // Write protect
			struct TP : Field<PBRIDGE_PACR, TP, 7> {}; // Trusted protect
		};
		
		struct PACR2
		{
			struct SP : Field<PBRIDGE_PACR, SP, 9 > {}; // Supervisor protect
			struct WP : Field<PBRIDGE_PACR, WP, 10> {}; // Write protect
			struct TP : Field<PBRIDGE_PACR, TP, 11> {}; // Trusted protect
		};
		
		struct PACR3
		{
			struct SP : Field<PBRIDGE_PACR, SP, 13> {}; // Supervisor protect
			struct WP : Field<PBRIDGE_PACR, WP, 14> {}; // Write protect
			struct TP : Field<PBRIDGE_PACR, TP, 15> {}; // Trusted protect
		};

		struct PACR4
		{
			struct SP : Field<PBRIDGE_PACR, SP, 17> {}; // Supervisor protect
			struct WP : Field<PBRIDGE_PACR, WP, 18> {}; // Write protect
			struct TP : Field<PBRIDGE_PACR, TP, 19> {}; // Trusted protect
		};

		struct PACR5
		{
			struct SP : Field<PBRIDGE_PACR, SP, 21> {}; // Supervisor protect
			struct WP : Field<PBRIDGE_PACR, WP, 22> {}; // Write protect
			struct TP : Field<PBRIDGE_PACR, TP, 23> {}; // Trusted protect
		};

		struct PACR6
		{
			struct SP : Field<PBRIDGE_PACR, SP, 25> {}; // Supervisor protect
			struct WP : Field<PBRIDGE_PACR, WP, 26> {}; // Write protect
			struct TP : Field<PBRIDGE_PACR, TP, 27> {}; // Trusted protect
		};

		struct PACR7
		{
			struct SP : Field<PBRIDGE_PACR, SP, 29> {}; // Supervisor protect
			struct WP : Field<PBRIDGE_PACR, WP, 30> {}; // Write protect
			struct TP : Field<PBRIDGE_PACR, TP, 31> {}; // Trusted protect
		};

		typedef FieldSet< typename PACR0::SP, typename PACR0::WP, typename PACR0::TP
		                , typename PACR1::SP, typename PACR1::WP, typename PACR1::TP
		                , typename PACR2::SP, typename PACR2::WP, typename PACR2::TP
		                , typename PACR3::SP, typename PACR3::WP, typename PACR3::TP
		                , typename PACR4::SP, typename PACR4::WP, typename PACR4::TP
		                , typename PACR5::SP, typename PACR5::WP, typename PACR5::TP
		                , typename PACR6::SP, typename PACR6::WP, typename PACR6::TP
		                , typename PACR7::SP, typename PACR7::WP, typename PACR7::TP > ALL;
		
		PBRIDGE_PACR(PBRIDGE<CONFIG> *_pbridge) : Super(_pbridge) { Init(); }
		PBRIDGE_PACR(PBRIDGE<CONFIG> *_pbridge, uint32_t value) : Super(_pbridge, value) { Init(); }
		
		void Init()
		{
			std::string suffix(OrdinalToAlpha(Trait::reg_num));

			std::stringstream name_sstr;
			name_sstr << "PBRIDGE_PACR" << suffix;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "Peripheral Access Control Register " << suffix;
			this->SetDescription(description_sstr.str());
			
			unsigned int base_field_num = 8 * Trait::reg_num;
			
			unsigned int i;
			for(i = 0; i < 8; i++)
			{
				unsigned int field_num = base_field_num + i;
				std::stringstream pacr_sp_name_sstr;
				pacr_sp_name_sstr << "PACR" << field_num << "[SP]";
				std::string pacr_sp_name(pacr_sp_name_sstr.str());
				
				std::stringstream pacr_sp_description_sstr;
				pacr_sp_description_sstr << "Peripheral #" << field_num << " Supervisor protect";
				std::string pacr_sp_description(pacr_sp_description_sstr.str());
				
				std::stringstream pacr_wp_name_sstr;
				pacr_wp_name_sstr << "PACR" << field_num << "[WP]";
				std::string pacr_wp_name(pacr_wp_name_sstr.str());
				
				std::stringstream pacr_wp_description_sstr;
				pacr_wp_description_sstr << "Peripheral #" << field_num << " Write protect";
				std::string pacr_wp_description(pacr_wp_description_sstr.str());

				std::stringstream pacr_tp_name_sstr;
				pacr_tp_name_sstr << "PACR" << field_num << "[TP]";
				std::string pacr_tp_name(pacr_tp_name_sstr.str());
				
				std::stringstream pacr_tp_description_sstr;
				pacr_tp_description_sstr << "Peripheral #" << field_num << " Trusted protect";
				std::string pacr_tp_description(pacr_tp_description_sstr.str());

				switch(i)
				{
					case 0:
						PACR0::SP::SetName(pacr_sp_name); PACR0::SP::SetDescription(pacr_sp_description);
						PACR0::WP::SetName(pacr_wp_name); PACR0::WP::SetDescription(pacr_wp_description);
						PACR0::TP::SetName(pacr_tp_name); PACR0::TP::SetDescription(pacr_tp_description);
						break;
					case 1:
						PACR1::SP::SetName(pacr_sp_name); PACR1::SP::SetDescription(pacr_sp_description);
						PACR1::WP::SetName(pacr_wp_name); PACR1::WP::SetDescription(pacr_wp_description);
						PACR1::TP::SetName(pacr_tp_name); PACR1::TP::SetDescription(pacr_tp_description);
						break;
					case 2:
						PACR2::SP::SetName(pacr_sp_name); PACR2::SP::SetDescription(pacr_sp_description);
						PACR2::WP::SetName(pacr_wp_name); PACR2::WP::SetDescription(pacr_wp_description);
						PACR2::TP::SetName(pacr_tp_name); PACR2::TP::SetDescription(pacr_tp_description);
						break;
					case 3:
						PACR3::SP::SetName(pacr_sp_name); PACR3::SP::SetDescription(pacr_sp_description);
						PACR3::WP::SetName(pacr_wp_name); PACR3::WP::SetDescription(pacr_wp_description);
						PACR3::TP::SetName(pacr_tp_name); PACR3::TP::SetDescription(pacr_tp_description);
						break;
					case 4:
						PACR4::SP::SetName(pacr_sp_name); PACR4::SP::SetDescription(pacr_sp_description);
						PACR4::WP::SetName(pacr_wp_name); PACR4::WP::SetDescription(pacr_wp_description);
						PACR4::TP::SetName(pacr_tp_name); PACR4::TP::SetDescription(pacr_tp_description);
						break;
					case 5:
						PACR5::SP::SetName(pacr_sp_name); PACR5::SP::SetDescription(pacr_sp_description);
						PACR5::WP::SetName(pacr_wp_name); PACR5::WP::SetDescription(pacr_wp_description);
						PACR5::TP::SetName(pacr_tp_name); PACR5::TP::SetDescription(pacr_tp_description);
						break;
					case 6:
						PACR6::SP::SetName(pacr_sp_name); PACR6::SP::SetDescription(pacr_sp_description);
						PACR6::WP::SetName(pacr_wp_name); PACR6::WP::SetDescription(pacr_wp_description);
						PACR6::TP::SetName(pacr_tp_name); PACR6::TP::SetDescription(pacr_tp_description);
						break;
					case 7:
						PACR7::SP::SetName(pacr_sp_name); PACR7::SP::SetDescription(pacr_sp_description);
						PACR7::WP::SetName(pacr_wp_name); PACR7::WP::SetDescription(pacr_wp_description);
						PACR7::TP::SetName(pacr_tp_name); PACR7::TP::SetDescription(pacr_tp_description);
						break;
				}
			}
		}
		
		void Reset()
		{
			this->Initialize(0x44444444ul);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			// modifying PACR may affect memory protection, so it's safer to invalidate SystemC TLM-2.0 direct memory pointers before modification
			this->pbridge->DMI_Invalidate();
			
			return Super::Write(value, bit_enable);
		}

		using Super::operator =;
	};

	// Off-platform Peripheral Access Control Register (PBRIDGE_OPACRn)
	template <typename Trait>
	struct PBRIDGE_OPACR : PBRIDGE_Register<PBRIDGE_OPACR<Trait>, SW_RW>
	{
		typedef PBRIDGE_Register<PBRIDGE_OPACR<Trait>, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x140 + (4 * Trait::reg_num);
		
		struct OPACR0
		{
			struct SP : Field<PBRIDGE_OPACR, SP, 1> {}; // Supervisor protect
			struct WP : Field<PBRIDGE_OPACR, WP, 2> {}; // Write protect
			struct TP : Field<PBRIDGE_OPACR, TP, 3> {}; // Trusted protect
		};
		
		struct OPACR1
		{
			struct SP : Field<PBRIDGE_OPACR, SP, 5> {}; // Supervisor protect
			struct WP : Field<PBRIDGE_OPACR, WP, 6> {}; // Write protect
			struct TP : Field<PBRIDGE_OPACR, TP, 7> {}; // Trusted protect
		};
		
		struct OPACR2
		{
			struct SP : Field<PBRIDGE_OPACR, SP, 9 > {}; // Supervisor protect
			struct WP : Field<PBRIDGE_OPACR, WP, 10> {}; // Write protect
			struct TP : Field<PBRIDGE_OPACR, TP, 11> {}; // Trusted protect
		};
		
		struct OPACR3
		{
			struct SP : Field<PBRIDGE_OPACR, SP, 13> {}; // Supervisor protect
			struct WP : Field<PBRIDGE_OPACR, WP, 14> {}; // Write protect
			struct TP : Field<PBRIDGE_OPACR, TP, 15> {}; // Trusted protect
		};

		struct OPACR4
		{
			struct SP : Field<PBRIDGE_OPACR, SP, 17> {}; // Supervisor protect
			struct WP : Field<PBRIDGE_OPACR, WP, 18> {}; // Write protect
			struct TP : Field<PBRIDGE_OPACR, TP, 19> {}; // Trusted protect
		};

		struct OPACR5
		{
			struct SP : Field<PBRIDGE_OPACR, SP, 21> {}; // Supervisor protect
			struct WP : Field<PBRIDGE_OPACR, WP, 22> {}; // Write protect
			struct TP : Field<PBRIDGE_OPACR, TP, 23> {}; // Trusted protect
		};

		struct OPACR6
		{
			struct SP : Field<PBRIDGE_OPACR, SP, 25> {}; // Supervisor protect
			struct WP : Field<PBRIDGE_OPACR, WP, 26> {}; // Write protect
			struct TP : Field<PBRIDGE_OPACR, TP, 27> {}; // Trusted protect
		};

		struct OPACR7
		{
			struct SP : Field<PBRIDGE_OPACR, SP, 29> {}; // Supervisor protect
			struct WP : Field<PBRIDGE_OPACR, WP, 30> {}; // Write protect
			struct TP : Field<PBRIDGE_OPACR, TP, 31> {}; // Trusted protect
		};

		typedef FieldSet< typename OPACR0::SP, typename OPACR0::WP, typename OPACR0::TP
		                , typename OPACR1::SP, typename OPACR1::WP, typename OPACR1::TP
		                , typename OPACR2::SP, typename OPACR2::WP, typename OPACR2::TP
		                , typename OPACR3::SP, typename OPACR3::WP, typename OPACR3::TP
		                , typename OPACR4::SP, typename OPACR4::WP, typename OPACR4::TP
		                , typename OPACR5::SP, typename OPACR5::WP, typename OPACR5::TP
		                , typename OPACR6::SP, typename OPACR6::WP, typename OPACR6::TP
		                , typename OPACR7::SP, typename OPACR7::WP, typename OPACR7::TP > ALL;
		
		PBRIDGE_OPACR(PBRIDGE<CONFIG> *_pbridge) : Super(_pbridge) { Init(); }
		PBRIDGE_OPACR(PBRIDGE<CONFIG> *_pbridge, uint32_t value) : Super(_pbridge, value) { Init(); }
		
		void Init()
		{
			std::string suffix(OrdinalToAlpha(Trait::reg_num));

			std::stringstream name_sstr;
			name_sstr << "PBRIDGE_OPACR" << suffix;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "Off-platform Peripheral Access Control Register " << suffix;
			this->SetDescription(description_sstr.str());
			
			unsigned int base_field_num = 8 * Trait::reg_num;
			
			unsigned int i;
			for(i = 0; i < 8; i++)
			{
				unsigned int field_num = base_field_num + i;
				
				std::stringstream opacr_sp_name_sstr;
				opacr_sp_name_sstr << "OPACR" << field_num << "[SP]";
				std::string opacr_sp_name(opacr_sp_name_sstr.str());
				
				std::stringstream opacr_sp_description_sstr;
				opacr_sp_description_sstr << "Off-platform Peripheral #" << field_num << " Supervisor protect";
				std::string opacr_sp_description(opacr_sp_description_sstr.str());
				
				std::stringstream opacr_wp_name_sstr;
				opacr_wp_name_sstr << "OPACR" << field_num << "[WP]";
				std::string opacr_wp_name(opacr_wp_name_sstr.str());
				
				std::stringstream opacr_wp_description_sstr;
				opacr_wp_description_sstr << "Off-platform Peripheral #" << field_num << " Write protect";
				std::string opacr_wp_description(opacr_wp_description_sstr.str());

				std::stringstream opacr_tp_name_sstr;
				opacr_tp_name_sstr << "OPACR" << field_num << "[TP]";
				std::string opacr_tp_name(opacr_tp_name_sstr.str());
				
				std::stringstream opacr_tp_description_sstr;
				opacr_tp_description_sstr << "Off-platform Peripheral #" << field_num << " Trusted protect";
				std::string opacr_tp_description(opacr_tp_description_sstr.str());

				switch(i)
				{
					case 0:
						OPACR0::SP::SetName(opacr_sp_name); OPACR0::SP::SetDescription(opacr_sp_description);
						OPACR0::WP::SetName(opacr_wp_name); OPACR0::WP::SetDescription(opacr_wp_description);
						OPACR0::TP::SetName(opacr_tp_name); OPACR0::TP::SetDescription(opacr_tp_description);
						break;
					case 1:
						OPACR1::SP::SetName(opacr_sp_name); OPACR1::SP::SetDescription(opacr_sp_description);
						OPACR1::WP::SetName(opacr_wp_name); OPACR1::WP::SetDescription(opacr_wp_description);
						OPACR1::TP::SetName(opacr_tp_name); OPACR1::TP::SetDescription(opacr_tp_description);
						break;
					case 2:
						OPACR2::SP::SetName(opacr_sp_name); OPACR2::SP::SetDescription(opacr_sp_description);
						OPACR2::WP::SetName(opacr_wp_name); OPACR2::WP::SetDescription(opacr_wp_description);
						OPACR2::TP::SetName(opacr_tp_name); OPACR2::TP::SetDescription(opacr_tp_description);
						break;
					case 3:
						OPACR3::SP::SetName(opacr_sp_name); OPACR3::SP::SetDescription(opacr_sp_description);
						OPACR3::WP::SetName(opacr_wp_name); OPACR3::WP::SetDescription(opacr_wp_description);
						OPACR3::TP::SetName(opacr_tp_name); OPACR3::TP::SetDescription(opacr_tp_description);
						break;
					case 4:
						OPACR4::SP::SetName(opacr_sp_name); OPACR4::SP::SetDescription(opacr_sp_description);
						OPACR4::WP::SetName(opacr_wp_name); OPACR4::WP::SetDescription(opacr_wp_description);
						OPACR4::TP::SetName(opacr_tp_name); OPACR4::TP::SetDescription(opacr_tp_description);
						break;
					case 5:
						OPACR5::SP::SetName(opacr_sp_name); OPACR5::SP::SetDescription(opacr_sp_description);
						OPACR5::WP::SetName(opacr_wp_name); OPACR5::WP::SetDescription(opacr_wp_description);
						OPACR5::TP::SetName(opacr_tp_name); OPACR5::TP::SetDescription(opacr_tp_description);
						break;
					case 6:
						OPACR6::SP::SetName(opacr_sp_name); OPACR6::SP::SetDescription(opacr_sp_description);
						OPACR6::WP::SetName(opacr_wp_name); OPACR6::WP::SetDescription(opacr_wp_description);
						OPACR6::TP::SetName(opacr_tp_name); OPACR6::TP::SetDescription(opacr_tp_description);
						break;
					case 7:
						OPACR7::SP::SetName(opacr_sp_name); OPACR7::SP::SetDescription(opacr_sp_description);
						OPACR7::WP::SetName(opacr_wp_name); OPACR7::WP::SetDescription(opacr_wp_description);
						OPACR7::TP::SetName(opacr_tp_name); OPACR7::TP::SetDescription(opacr_tp_description);
						break;
				}
			}
		}
		
		void Reset()
		{
			this->Initialize(0x44444444ul);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			// modifying PACR may affect memory protection, so it's safer to invalidate SystemC TLM-2.0 direct memory pointers before modification
			this->pbridge->DMI_Invalidate();
			
			return Super::Write(value, bit_enable);
		}

		using Super::operator =;
	};

	typedef PBRIDGE_MPR<A>    PBRIDGE_MPRA;
	typedef PBRIDGE_MPR<B>    PBRIDGE_MPRB;
	typedef PBRIDGE_PACR<A>   PBRIDGE_PACRA;
	typedef PBRIDGE_PACR<B>   PBRIDGE_PACRB;
	typedef PBRIDGE_PACR<C>   PBRIDGE_PACRC;
	typedef PBRIDGE_PACR<D>   PBRIDGE_PACRD;
	typedef PBRIDGE_PACR<E>   PBRIDGE_PACRE;
	typedef PBRIDGE_PACR<F>   PBRIDGE_PACRF;
	typedef PBRIDGE_PACR<G>   PBRIDGE_PACRG;
	typedef PBRIDGE_PACR<H>   PBRIDGE_PACRH;
	typedef PBRIDGE_OPACR<A>  PBRIDGE_OPACRA;
	typedef PBRIDGE_OPACR<B>  PBRIDGE_OPACRB;
	typedef PBRIDGE_OPACR<C>  PBRIDGE_OPACRC;
	typedef PBRIDGE_OPACR<D>  PBRIDGE_OPACRD;
	typedef PBRIDGE_OPACR<E>  PBRIDGE_OPACRE;
	typedef PBRIDGE_OPACR<F>  PBRIDGE_OPACRF;
	typedef PBRIDGE_OPACR<G>  PBRIDGE_OPACRG;
	typedef PBRIDGE_OPACR<H>  PBRIDGE_OPACRH;
	typedef PBRIDGE_OPACR<I>  PBRIDGE_OPACRI;
	typedef PBRIDGE_OPACR<J>  PBRIDGE_OPACRJ;
	typedef PBRIDGE_OPACR<K>  PBRIDGE_OPACRK;
	typedef PBRIDGE_OPACR<L>  PBRIDGE_OPACRL;
	typedef PBRIDGE_OPACR<M>  PBRIDGE_OPACRM;
	typedef PBRIDGE_OPACR<N>  PBRIDGE_OPACRN;
	typedef PBRIDGE_OPACR<O>  PBRIDGE_OPACRO;
	typedef PBRIDGE_OPACR<P>  PBRIDGE_OPACRP;
	typedef PBRIDGE_OPACR<Q>  PBRIDGE_OPACRQ;
	typedef PBRIDGE_OPACR<R>  PBRIDGE_OPACRR;
	typedef PBRIDGE_OPACR<S>  PBRIDGE_OPACRS;
	typedef PBRIDGE_OPACR<T>  PBRIDGE_OPACRT;
	typedef PBRIDGE_OPACR<U>  PBRIDGE_OPACRU;
	typedef PBRIDGE_OPACR<V>  PBRIDGE_OPACRV;
	typedef PBRIDGE_OPACR<W>  PBRIDGE_OPACRW;
	typedef PBRIDGE_OPACR<X>  PBRIDGE_OPACRX;
	typedef PBRIDGE_OPACR<Y>  PBRIDGE_OPACRY;
	typedef PBRIDGE_OPACR<Z>  PBRIDGE_OPACRZ;
	typedef PBRIDGE_OPACR<AA> PBRIDGE_OPACRAA;
	typedef PBRIDGE_OPACR<AB> PBRIDGE_OPACRAB;
	typedef PBRIDGE_OPACR<AC> PBRIDGE_OPACRAC;
	typedef PBRIDGE_OPACR<AD> PBRIDGE_OPACRAD;
	typedef PBRIDGE_OPACR<AE> PBRIDGE_OPACRAE;
	typedef PBRIDGE_OPACR<AF> PBRIDGE_OPACRAF;

	PBRIDGE_MPRA    pbridge_mpra;
	PBRIDGE_MPRB    pbridge_mprb;
	PBRIDGE_PACRA   pbridge_pacra;
	PBRIDGE_PACRB   pbridge_pacrb;
	PBRIDGE_PACRC   pbridge_pacrc;
	PBRIDGE_PACRD   pbridge_pacrd;
	PBRIDGE_PACRE   pbridge_pacre;
	PBRIDGE_PACRF   pbridge_pacrf;
	PBRIDGE_PACRG   pbridge_pacrg;
	PBRIDGE_PACRH   pbridge_pacrh;
	PBRIDGE_OPACRA  pbridge_opacra;
	PBRIDGE_OPACRB  pbridge_opacrb;
	PBRIDGE_OPACRC  pbridge_opacrc;
	PBRIDGE_OPACRD  pbridge_opacrd;
	PBRIDGE_OPACRE  pbridge_opacre;
	PBRIDGE_OPACRF  pbridge_opacrf;
	PBRIDGE_OPACRG  pbridge_opacrg;
	PBRIDGE_OPACRH  pbridge_opacrh;
	PBRIDGE_OPACRI  pbridge_opacri;
	PBRIDGE_OPACRJ  pbridge_opacrj;
	PBRIDGE_OPACRK  pbridge_opacrk;
	PBRIDGE_OPACRL  pbridge_opacrl;
	PBRIDGE_OPACRM  pbridge_opacrm;
	PBRIDGE_OPACRN  pbridge_opacrn;
	PBRIDGE_OPACRO  pbridge_opacro;
	PBRIDGE_OPACRP  pbridge_opacrp;
	PBRIDGE_OPACRQ  pbridge_opacrq;
	PBRIDGE_OPACRR  pbridge_opacrr;
	PBRIDGE_OPACRS  pbridge_opacrs;
	PBRIDGE_OPACRT  pbridge_opacrt;
	PBRIDGE_OPACRU  pbridge_opacru;
	PBRIDGE_OPACRV  pbridge_opacrv;
	PBRIDGE_OPACRW  pbridge_opacrw;
	PBRIDGE_OPACRX  pbridge_opacrx;
	PBRIDGE_OPACRY  pbridge_opacry;
	PBRIDGE_OPACRZ  pbridge_opacrz;
	PBRIDGE_OPACRAA pbridge_opacraa;
	PBRIDGE_OPACRAB pbridge_opacrab;
	PBRIDGE_OPACRAC pbridge_opacrac;
	PBRIDGE_OPACRAD pbridge_opacrad;
	PBRIDGE_OPACRAE pbridge_opacrae;
	PBRIDGE_OPACRAF pbridge_opacraf;
	
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	struct MPROT
	{
		struct MTR : unisim::util::reg::core::Field<MTR, 2> {}; // Master trusted for read
		struct MTW : unisim::util::reg::core::Field<MTW, 1> {}; // Master trusted for write
		struct MPL : unisim::util::reg::core::Field<MPL, 0> {}; // Master privilege level
		
		typedef FieldSet<MTR, MTW, MPL> ALL;
	};
	
	struct PACR
	{
		struct SP : unisim::util::reg::core::Field<SP, 2> {}; // Supervisor protect
		struct WP : unisim::util::reg::core::Field<SP, 1> {}; // Write protect
		struct TP : unisim::util::reg::core::Field<SP, 0> {}; // Trusted protect
		
		typedef FieldSet<SP, WP, TP> ALL;
	};

	AccessControlRegisterMapping acr_mapping[CONFIG::NUM_MAPPINGS];
	unisim::kernel::variable::Parameter<AccessControlRegisterMapping> *param_acr_mapping[CONFIG::NUM_MAPPINGS];
	
	unsigned int GetMasterProtection(unsigned int master_id) const;
	unsigned int GetAccessControl(unsigned int mapping_id) const;
	
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
};

} // end of namespace pbridge
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_PBRIDGE_PBRIDGE_HH__
