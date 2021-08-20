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

#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_XBAR_XBAR_HH__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_XBAR_XBAR_HH__

#include <unisim/component/tlm2/interconnect/programmable_router/router.hh>
#include <unisim/component/tlm2/interconnect/freescale/mpc57xx/xbar/smpu/smpu.hh>
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

namespace smpu {

template <typename CONFIG>
class SMPU;

} // end of namespace smpu

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
	static const unsigned int BUSWIDTH = 32;
	static const uint32_t XBAR_PRS_RESET_VALUE = 0x00301425;
	static const uint32_t XBAR_CRS_RESET_VALUE = 0x003f0100;
};
#endif

template <typename CONFIG>
class XBAR
	: public unisim::component::tlm2::interconnect::programmable_router::Router<CONFIG>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Registers>
{
public:
	typedef unisim::component::tlm2::interconnect::programmable_router::Router<CONFIG> Super;
	typedef unisim::component::tlm2::interconnect::freescale::mpc57xx::xbar::smpu::SMPU<CONFIG> SMPU;
	typedef typename Super::transaction_type transaction_type;
	typedef typename Super::MAPPING MAPPING;
	
	static const unsigned int TLM2_IP_VERSION_MAJOR = 1;
	static const unsigned int TLM2_IP_VERSION_MINOR = 0;
	static const unsigned int TLM2_IP_VERSION_PATCH = 0;
	static const unsigned int XBAR_PERIPHERAL_SLAVE_IF = CONFIG::XBAR_PERIPHERAL_SLAVE_IF;
	static const unsigned int SMPU_PERIPHERAL_SLAVE_IF = CONFIG::SMPU_PERIPHERAL_SLAVE_IF;
	static const bool threaded_model                = false;
	
	// services
	unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;
	unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> smpu_registers_export;

	XBAR(const sc_core::sc_module_name& name, unisim::kernel::Object *parent);
	virtual ~XBAR();

	//////////////// unisim::service::interface::Registers ////////////////////
	
	virtual unisim::service::interfaces::Register *GetRegister(const char *name);
	virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);
	
protected:
	friend class smpu::SMPU<CONFIG>;
	
	virtual void end_of_elaboration();
	virtual void Reset();
	
	virtual bool ApplyMap(transaction_type &trans, MAPPING const *&applied_mapping, MemoryRegion *mem_rgn);

	// Common XBAR register representation
	template <typename REGISTER, Access _ACCESS>
	struct XBAR_Register : AddressableRegister<REGISTER, 32, _ACCESS>
	{
		typedef AddressableRegister<REGISTER, 32, _ACCESS> Super;
		
		XBAR_Register(XBAR<CONFIG> *_xbar) : Super(), xbar(_xbar) {}
		XBAR_Register(XBAR<CONFIG> *_xbar, uint32_t value) : Super(value), xbar(_xbar) {}

		inline bool IsVerboseRead() const ALWAYS_INLINE { return xbar->verbose; }
		inline bool IsVerboseWrite() const ALWAYS_INLINE { return xbar->verbose; }
		inline std::ostream& GetInfoStream() ALWAYS_INLINE { return xbar->logger.DebugInfoStream(); }

		using Super::operator =;
		
	protected:
		XBAR<CONFIG> *xbar;
	};
	
	// XBAR Priority Registers Slave (XBAR_PRSn)
	struct XBAR_PRS : XBAR_Register<XBAR_PRS, SW_R>
	{
		typedef XBAR_Register<XBAR_PRS, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;
		static const sc_dt::uint64 ADDRESS_STRIDE = 0x100;
		
		struct M7 : Field<XBAR_PRS, M7,  1,  3, SW_R> {}; // Master 7 priority
		struct M6 : Field<XBAR_PRS, M6,  5,  7, SW_R> {}; // Master 6 priority
		struct M5 : Field<XBAR_PRS, M5,  9, 11, SW_R> {}; // Master 5 priority
		struct M4 : Field<XBAR_PRS, M4, 13, 15, SW_R> {}; // Master 4 priority
		struct M3 : Field<XBAR_PRS, M3, 17, 19, SW_R> {}; // Master 3 priority
		struct M2 : Field<XBAR_PRS, M2, 21, 23, SW_R> {}; // Master 2 priority
		struct M1 : Field<XBAR_PRS, M1, 25, 27, SW_R> {}; // Master 1 priority
		struct M0 : Field<XBAR_PRS, M0, 29, 31, SW_R> {}; // Master 0 priority
		
		SWITCH_ENUM_TRAIT(unsigned int, _);
		CASE_ENUM_TRAIT(1, _) { typedef FieldSet<M0> ALL; };
		CASE_ENUM_TRAIT(2, _) { typedef FieldSet<M1, M0> ALL; };
		CASE_ENUM_TRAIT(3, _) { typedef FieldSet<M2, M1, M0> ALL; };
		CASE_ENUM_TRAIT(4, _) { typedef FieldSet<M3, M2, M1, M0> ALL; };
		CASE_ENUM_TRAIT(5, _) { typedef FieldSet<M4, M3, M2, M1, M0> ALL; };
		CASE_ENUM_TRAIT(6, _) { typedef FieldSet<M5, M4, M3, M2, M1, M0> ALL; };
		CASE_ENUM_TRAIT(7, _) { typedef FieldSet<M6, M5, M4, M3, M2, M1, M0> ALL; };
		CASE_ENUM_TRAIT(8, _) { typedef FieldSet<M7, M6, M5, M4, M3, M2, M1, M0> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::INPUT_SOCKETS, _)::ALL ALL;
		
		XBAR_PRS() : Super(0) {}
		XBAR_PRS(XBAR<CONFIG> *_xbar) : Super(_xbar) {}
		XBAR_PRS(XBAR<CONFIG> *_xbar, uint32_t value) : Super(_xbar, value) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, XBAR<CONFIG> *_xbar)
		{
			this->xbar = _xbar;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "XBAR_PRS" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "XBAR Priority Registers Slave #" << reg_num;
			this->SetDescription(description_sstr.str());
			
			M7::SetName("M7"); M7::SetDescription("Master 7 priority");
			M6::SetName("M6"); M6::SetDescription("Master 6 priority");
			M5::SetName("M5"); M5::SetDescription("Master 5 priority");
			M4::SetName("M4"); M4::SetDescription("Master 4 priority");
			M3::SetName("M3"); M3::SetDescription("Master 3 priority");
			M2::SetName("M2"); M2::SetDescription("Master 2 priority");
			M1::SetName("M1"); M1::SetDescription("Master 1 priority");
			M0::SetName("M0"); M0::SetDescription("Master 0 priority");
		}
		
		void Reset()
		{
			this->Initialize(CONFIG::XBAR_PRS_RESET_VALUE);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			if(this->xbar->xbar_crs[reg_num].template Get<typename XBAR_CRS::RO>()) return RWS_ANA;
			
			return Super::Write(value, bit_enable);
		}
		
	private:
		unsigned int reg_num;
	};
	
	// XBAR Control Register (XBAR_CRSn)
	struct XBAR_CRS : XBAR_Register<XBAR_CRS, SW_RW>
	{
		typedef XBAR_Register<XBAR_CRS, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x10;
		static const sc_dt::uint64 ADDRESS_STRIDE = 0x100;
		
		struct RO    : Field<XBAR_CRS, RO  , 0     > {}; // Read only
		struct HRP   : Field<XBAR_CRS, HRP , 1     > {}; // Halt Request Priority
		struct HPE7  : Field<XBAR_CRS, HPE7, 8     > {}; // High Priority Enable #7
		struct HPE6  : Field<XBAR_CRS, HPE6, 9     > {}; // High Priority Enable #6 
		struct HPE5  : Field<XBAR_CRS, HPE5, 10    > {}; // High Priority Enable #5
		struct HPE4  : Field<XBAR_CRS, HPE4, 11    > {}; // High Priority Enable #4
		struct HPE3  : Field<XBAR_CRS, HPE3, 12    > {}; // High Priority Enable #3
		struct HPE2  : Field<XBAR_CRS, HPE2, 13    > {}; // High Priority Enable #2
		struct HPE1  : Field<XBAR_CRS, HPE1, 14    > {}; // High Priority Enable #1
		struct HPE0  : Field<XBAR_CRS, HPE0, 15    > {}; // High Priority Enable #0
		struct ARB   : Field<XBAR_CRS, ARB , 22, 23> {}; // Arbitration mode
		struct PCTL  : Field<XBAR_CRS, PCTL, 26, 27> {}; // Parking control
		struct PARK  : Field<XBAR_CRS, PARK, 29, 31> {}; // Park
		
		SWITCH_ENUM_TRAIT(unsigned int, _);
		CASE_ENUM_TRAIT(1, _) { typedef FieldSet<RO, HRP, HPE0, ARB, PCTL, PARK> ALL; };
		CASE_ENUM_TRAIT(2, _) { typedef FieldSet<RO, HRP, HPE1, HPE0, ARB, PCTL, PARK> ALL; };
		CASE_ENUM_TRAIT(3, _) { typedef FieldSet<RO, HRP, HPE2, HPE1, HPE0, ARB, PCTL, PARK> ALL; };
		CASE_ENUM_TRAIT(4, _) { typedef FieldSet<RO, HRP, HPE3, HPE2, HPE1, HPE0, ARB, PCTL, PARK> ALL; };
		CASE_ENUM_TRAIT(5, _) { typedef FieldSet<RO, HRP, HPE4, HPE3, HPE2, HPE1, HPE0, ARB, PCTL, PARK> ALL; };
		CASE_ENUM_TRAIT(6, _) { typedef FieldSet<RO, HRP, HPE5, HPE4, HPE3, HPE2, HPE1, HPE0, ARB, PCTL, PARK> ALL; };
		CASE_ENUM_TRAIT(7, _) { typedef FieldSet<RO, HRP, HPE6, HPE5, HPE4, HPE3, HPE2, HPE1, HPE0, ARB, PCTL, PARK> ALL; };
		CASE_ENUM_TRAIT(8, _) { typedef FieldSet<RO, HRP, HPE7, HPE6, HPE5, HPE4, HPE3, HPE2, HPE1, HPE0, ARB, PCTL, PARK> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::INPUT_SOCKETS, _)::ALL ALL;
		
		XBAR_CRS() : Super(0) {}
		XBAR_CRS(XBAR<CONFIG> *_xbar) : Super(_xbar) {}
		XBAR_CRS(XBAR<CONFIG> *_xbar, uint32_t value) : Super(_xbar, value) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, XBAR<CONFIG> *_xbar)
		{
			this->xbar = _xbar;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "XBAR_CRS" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "XBAR Control Register #" << reg_num;
			this->SetDescription(description_sstr.str());
			
			RO  ::SetName("RO");   RO  ::SetDescription("Read only");
			HRP ::SetName("HRP");  HRP ::SetDescription("Halt Request Priority");
			HPE7::SetName("HPE7"); HPE7::SetDescription("High Priority Enable #7");
			HPE6::SetName("HPE6"); HPE6::SetDescription("High Priority Enable #6");
			HPE5::SetName("HPE5"); HPE5::SetDescription("High Priority Enable #5");
			HPE4::SetName("HPE4"); HPE4::SetDescription("High Priority Enable #4");
			HPE3::SetName("HPE3"); HPE3::SetDescription("High Priority Enable #3");
			HPE2::SetName("HPE2"); HPE2::SetDescription("High Priority Enable #2");
			HPE1::SetName("HPE1"); HPE1::SetDescription("High Priority Enable #1");
			HPE0::SetName("HPE0"); HPE0::SetDescription("High Priority Enable #0");
			ARB ::SetName("ARB "); ARB ::SetDescription("Arbitration mode");
			PCTL::SetName("PCTL"); PCTL::SetDescription("Parking control");
			PARK::SetName("PARK"); PARK::SetDescription("Park");
		}
		
		void Reset()
		{
			this->Initialize(CONFIG::XBAR_CRS_RESET_VALUE);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			if(this->template Get<RO>()) return RWS_ANA;
			
			unsigned int output_port = reg_num;
			this->xbar->DMI_Invalidate(output_port);
			
			return Super::Write(value, bit_enable);
		}
		
	private:
		unsigned int reg_num;
	};

	AddressableRegisterFile<XBAR_PRS, CONFIG::OUTPUT_SOCKETS, XBAR<CONFIG> > xbar_prs; // XBAR_PRSn
	AddressableRegisterFile<XBAR_CRS, CONFIG::OUTPUT_SOCKETS, XBAR<CONFIG> > xbar_crs; // XBAR_CRSn
	
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	SMPU smpu;
	
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
};

} // end of namespace xbar
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_XBAR_XBAR_HH__
