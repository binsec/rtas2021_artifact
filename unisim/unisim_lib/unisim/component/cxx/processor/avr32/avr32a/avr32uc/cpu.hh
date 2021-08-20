/*
 *  Copyright (c) 2014,
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
 * Authors: Julien Lisita (julien.lisita@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_AVR32_AVR32A_AVR32UC_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_AVR32_AVR32A_AVR32UC_CPU_HH__

#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.hh>
#include <unisim/component/cxx/processor/avr32/avr32a/avr32uc/config.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/debug/simple_register_registry.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/synchronizable.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/avr32_t2h_syscalls.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/inlining/inlining.hh>
#include <map>
#include <iosfwd>

#include <unisim/util/backtrace/backtrace.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace avr32 {
namespace avr32a {
namespace avr32uc {

using unisim::service::interfaces::DebugYielding;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::MemoryAccessReportingType;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::AVR32_T2H_Syscalls;
using namespace unisim::util::endian;
using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::Object;
using unisim::service::interfaces::Loader;
using unisim::util::debug::Symbol;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Synchronizable;
using unisim::service::interfaces::TrapReporting;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Statistic;
using unisim::kernel::variable::ParameterArray;
using unisim::kernel::variable::StatisticFormula;
using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
using namespace std;

template <class CONFIG>
class PCRegisterInterface : public unisim::service::interfaces::Register
{
public:
	PCRegisterInterface(const char *name, typename CONFIG::address_t *r15_value, typename CONFIG::address_t *npc_value);
	virtual ~PCRegisterInterface();
	virtual const char *GetName() const;
	virtual void GetValue(void *buffer) const;
	virtual void SetValue(const void *buffer);
	virtual int GetSize() const;
private:
	std::string name;
	typename CONFIG::address_t *r15_value;
	typename CONFIG::address_t *npc_value;
};

template <class CONFIG>
class CPU :
	public unisim::component::cxx::processor::avr32::avr32a::avr32uc::Decoder<CONFIG>,
	public Client<Loader>,
	public Client<SymbolTableLookup<typename CONFIG::address_t> >,
	public Client<DebugYielding>,
	public Client<MemoryAccessReporting<typename CONFIG::address_t> >,
	public Client<TrapReporting>,
	public Service<MemoryAccessReportingControl>,
	public Service<Disassembly<typename CONFIG::address_t> >,
	public Service<unisim::service::interfaces::Registers>,
	public Service<Memory<typename CONFIG::address_t> >,
	public Service<MemoryInjection<typename CONFIG::address_t> >,
	public Client<Memory<typename CONFIG::physical_address_t> >,
	public Service<Synchronizable>,
	public Client<AVR32_T2H_Syscalls>
{
public:
	//=====================================================================
	//=                  public service imports/exports                   =
	//=====================================================================
	
	ServiceExport<Disassembly<typename CONFIG::address_t> > disasm_export;
	ServiceExport<unisim::service::interfaces::Registers> registers_export;
	ServiceExport<Memory<typename CONFIG::address_t> > memory_export;
	ServiceExport<MemoryInjection<typename CONFIG::address_t> > memory_injection_export;
	ServiceExport<Synchronizable> synchronizable_export;
	ServiceExport<MemoryAccessReportingControl> memory_access_reporting_control_export;

	ServiceImport<Loader> loader_import;
	ServiceImport<DebugYielding> debug_yielding_import;
	ServiceImport<MemoryAccessReporting<typename CONFIG::address_t> > memory_access_reporting_import;
	ServiceImport<SymbolTableLookup<typename CONFIG::address_t> > symbol_table_lookup_import;
	ServiceImport<Memory<typename CONFIG::physical_address_t> > memory_import;
	ServiceImport<TrapReporting> trap_reporting_import;
	ServiceImport<AVR32_T2H_Syscalls> avr32_t2h_syscalls_import;
	
	//=====================================================================
	//=                    Constructor/Destructor                         =
	//=====================================================================

	CPU(const char *name, Object *parent = 0);
	virtual ~CPU();
	
        //=====================================================================
	//=                        Getters//Setters                           =
	//=====================================================================    

	inline uint32_t GetGPR(unsigned int reg_num) ALWAYS_INLINE { return gpr[reg_num]; }
        inline uint32_t GetPC() ALWAYS_INLINE {return gpr[15];}    //return adress of current instruction
	inline uint32_t GetSP() ALWAYS_INLINE {return gpr[13];}
   	inline uint32_t GetLR() ALWAYS_INLINE {return gpr[14];}
	inline uint32_t GetSP_app() ALWAYS_INLINE {return sp_app;}
	inline uint32_t GetSP_sys() ALWAYS_INLINE {return sp_sys;}
	
	string GetMicroArchitecture(){return micro_architecture;}
	inline uint32_t GetNPC() ALWAYS_INLINE {return npc;}

	inline uint32_t GetSR() ALWAYS_INLINE { return sr; }
	inline uint32_t GetSR_SS() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_SS_MASK) >> CONFIG::SR_SS_OFFSET; }
	inline uint32_t GetSR_H() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_H_MASK) >> CONFIG::SR_H_OFFSET; }
	inline uint32_t GetSR_J() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_J_MASK) >> CONFIG::SR_J_OFFSET; }
	inline uint32_t GetSR_DM() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_DM_MASK) >> CONFIG::SR_DM_OFFSET; }
	inline uint32_t GetSR_D() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_D_MASK) >> CONFIG::SR_D_OFFSET; }
	inline uint32_t GetSR_M2() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_M2_MASK) >> CONFIG::SR_M2_OFFSET; }
	inline uint32_t GetSR_M1() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_M1_MASK) >> CONFIG::SR_M1_OFFSET; }
	inline uint32_t GetSR_M0() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_M0_MASK) >> CONFIG::SR_M0_OFFSET; }
	inline uint32_t GetSR_M() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_M_MASK) >> CONFIG::SR_M_OFFSET; }
	inline bool IsPrivilegedMode() ALWAYS_INLINE { return GetSR_M() != CONFIG::EXEC_MODE_APPLICATION; }
	inline uint32_t GetSR_EM() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_EM_MASK) >> CONFIG::SR_EM_OFFSET; }
	inline uint32_t GetSR_I3M() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_I3M_MASK) >> CONFIG::SR_I3M_OFFSET; }
	inline uint32_t GetSR_I2M() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_I2M_MASK) >> CONFIG::SR_I2M_OFFSET; }
	inline uint32_t GetSR_I1M() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_I1M_MASK) >> CONFIG::SR_I1M_OFFSET; }
	inline uint32_t GetSR_I0M() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_I0M_MASK) >> CONFIG::SR_I0M_OFFSET; }
	inline uint32_t GetSR_GM() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_GM_MASK) >> CONFIG::SR_GM_OFFSET; }
	inline uint32_t GetSR_R() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_R_MASK) >> CONFIG::SR_R_OFFSET; }
	inline uint32_t GetSR_T() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_T_MASK) >> CONFIG::SR_T_OFFSET; }
	inline uint32_t GetSR_L() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_L_MASK) >> CONFIG::SR_L_OFFSET; }
	inline uint32_t GetSR_Q() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_Q_MASK) >> CONFIG::SR_Q_OFFSET; }
	inline uint32_t GetSR_V() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_V_MASK) >> CONFIG::SR_V_OFFSET; }
	inline uint32_t GetSR_N() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_N_MASK) >> CONFIG::SR_N_OFFSET; }
	inline uint32_t GetSR_Z() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_Z_MASK) >> CONFIG::SR_Z_OFFSET; }
	inline uint32_t GetSR_C() ALWAYS_INLINE { return (GetSR() & CONFIG::SR_C_MASK) >> CONFIG::SR_C_OFFSET; }
	
	inline uint32_t GetEvba() ALWAYS_INLINE {return evba;}
	inline uint32_t GetAcba() ALWAYS_INLINE {return acba;}
	inline uint32_t GetCpucr() ALWAYS_INLINE {return cpucr;}
	inline uint32_t GetConfig0() ALWAYS_INLINE {return config0;}
	inline uint32_t GetConfig1() ALWAYS_INLINE {return config1;}
	inline uint32_t GetCount() ALWAYS_INLINE {return count;}
	inline uint32_t GetCompare() ALWAYS_INLINE {return compare;}
	
	inline uint32_t GetRSR_DBG() ALWAYS_INLINE {return rsr_dbg;}
	inline uint32_t GetRAR_DBG() ALWAYS_INLINE {return rar_dbg;}
	inline uint32_t GetSS_RSR() ALWAYS_INLINE {return ss_rsr;}
	inline uint32_t GetSS_RAR() ALWAYS_INLINE {return ss_rar;}
		
	void SetGPR(unsigned int reg_num, uint32_t val) ALWAYS_INLINE { if(reg_num != 15) gpr[reg_num] = val; else Branch(val); }
	void SetSP(uint32_t val) ALWAYS_INLINE { gpr[13] = val; }
	void SetLR(uint32_t val) ALWAYS_INLINE { gpr[14]= val; }
	void SetSP_app(uint32_t val) ALWAYS_INLINE {sp_app=val;}
	void SetSP_sys(uint32_t val) ALWAYS_INLINE {sp_sys=val;}

	      
        
	inline void Branch(uint32_t target_addr) ALWAYS_INLINE { npc = target_addr; }

	inline void SetHW_SR(uint32_t val) ALWAYS_INLINE { sr = (val & ~CONFIG::HW_SR_MASK) | (val & CONFIG::HW_SR_MASK); }
	inline void SetHW_SR_SS(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_SS_MASK) | ((val << CONFIG::SR_SS_OFFSET) & CONFIG::SR_SS_MASK)); }
	inline void SetHW_SR_H(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_H_MASK) | ((val << CONFIG::SR_H_OFFSET) & CONFIG::SR_H_MASK)); }
	inline void SetHW_SR_J(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_J_MASK) | ((val << CONFIG::SR_J_OFFSET) & CONFIG::SR_J_MASK)); }
	inline void SetHW_SR_DM(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_DM_MASK) | ((val << CONFIG::SR_DM_OFFSET) & CONFIG::SR_DM_MASK)); }
	inline void SetHW_SR_D(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_D_MASK) | ((val << CONFIG::SR_D_OFFSET) & CONFIG::SR_D_MASK)); }
	inline void SetHW_SR_M2(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_M2_MASK) | ((val << CONFIG::SR_M2_OFFSET) & CONFIG::SR_M2_MASK)); }
	inline void SetHW_SR_M1(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_M1_MASK) | ((val << CONFIG::SR_M1_OFFSET) & CONFIG::SR_M1_MASK)); }
	inline void SetHW_SR_M0(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_M0_MASK) | ((val << CONFIG::SR_M0_OFFSET) & CONFIG::SR_M0_MASK)); }
	inline void SetHW_SR_M(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_M_MASK) | ((val << CONFIG::SR_M_OFFSET) & CONFIG::SR_M_MASK)); }
	inline void SetHW_SR_EM(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_EM_MASK) | ((val << CONFIG::SR_EM_OFFSET) & CONFIG::SR_EM_MASK)); }
	inline void SetHW_SR_I3M(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_I3M_MASK) | ((val << CONFIG::SR_I3M_OFFSET) & CONFIG::SR_I3M_MASK)); }
	inline void SetHW_SR_I2M(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_I2M_MASK) | ((val << CONFIG::SR_I2M_OFFSET) & CONFIG::SR_I2M_MASK)); }
	inline void SetHW_SR_I1M(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_I1M_MASK) | ((val << CONFIG::SR_I1M_OFFSET) & CONFIG::SR_I1M_MASK)); }
	inline void SetHW_SR_I0M(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_I0M_MASK) | ((val << CONFIG::SR_I0M_OFFSET) & CONFIG::SR_I0M_MASK)); }
	inline void SetHW_SR_GM(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_GM_MASK) | ((val << CONFIG::SR_GM_OFFSET) & CONFIG::SR_GM_MASK)); }
	inline void SetHW_SR_R(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_R_MASK) | ((val << CONFIG::SR_R_OFFSET) & CONFIG::SR_R_MASK)); }
	inline void SetHW_SR_T(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_T_MASK) | ((val << CONFIG::SR_T_OFFSET) & CONFIG::SR_T_MASK)); }
	inline void SetHW_SR_L(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_L_MASK) | ((val << CONFIG::SR_L_OFFSET) & CONFIG::SR_L_MASK)); }
	inline void SetHW_SR_Q(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_Q_MASK) | ((val << CONFIG::SR_Q_OFFSET) & CONFIG::SR_Q_MASK)); }
	inline void SetHW_SR_V(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_V_MASK) | ((val << CONFIG::SR_V_OFFSET) & CONFIG::SR_V_MASK)); }
	inline void SetHW_SR_N(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_N_MASK) | ((val << CONFIG::SR_N_OFFSET) & CONFIG::SR_N_MASK)); }
	inline void SetHW_SR_Z(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_Z_MASK) | ((val << CONFIG::SR_Z_OFFSET) & CONFIG::SR_Z_MASK)); }
	inline void SetHW_SR_C(uint32_t val) ALWAYS_INLINE { SetHW_SR((GetSR() & ~CONFIG::SR_C_MASK) | ((val << CONFIG::SR_C_OFFSET) & CONFIG::SR_C_MASK)); }

	inline void SetSR(uint32_t val) ALWAYS_INLINE
	{
		if(IsPrivilegedMode())
		{
			SetHW_SR((GetSR() & ~CONFIG::SR_MASK) | (val & CONFIG::SR_MASK));
		}
		else
		{
			SetHW_SR((GetSR() & ~CONFIG::SR_APP_MASK) | (val & CONFIG::SR_APP_MASK));
		}
	}
	
	inline void SetSRWithExceptionUpdate(uint32_t val) ALWAYS_INLINE
	{
		if(IsPrivilegedMode())
		{
			SetHW_SR((GetSR() & ~CONFIG::SR_MASK) | (val & CONFIG::SR_MASK));
		}
		else
		{
			SetHW_SR((GetSR() & ~CONFIG::SR_APP_MASK) | (val & CONFIG::SR_APP_MASK));
		}
		UpdateExcEnable();
	}

	inline void UpdateExcEnable()
	{
		if(GetSR_EM() || GetSR_GM() || GetSR_I0M())
			exc_enable &= ~CONFIG::EXC_ENABLE_IRQ0;
		else
			exc_enable |= CONFIG::EXC_ENABLE_IRQ0;
		
		if(GetSR_EM() || GetSR_GM() || GetSR_I1M())
			exc_enable &= ~CONFIG::EXC_ENABLE_IRQ1;
		else
			exc_enable |= CONFIG::EXC_ENABLE_IRQ1;

		if(GetSR_EM() || GetSR_GM() || GetSR_I2M())
			exc_enable &= ~CONFIG::EXC_ENABLE_IRQ2;
		else
			exc_enable |= CONFIG::EXC_ENABLE_IRQ2;

		if(GetSR_EM() || GetSR_GM() || GetSR_I3M())
			exc_enable &= ~CONFIG::EXC_ENABLE_IRQ3;
		else
			exc_enable |= CONFIG::EXC_ENABLE_IRQ3;

		if(GetSR_EM())
		{
			exc_enable &= ~0;
		}
		else
		{
			exc_enable |= CONFIG::EXC_ENABLE_RESET | CONFIG::EXC_ENABLE_NMI | CONFIG::EXC_ENABLE_UNRECOVERABLE | CONFIG::EXC_ENABLE_TLB_MULTIPLE_HIT | CONFIG::EXC_ENABLE_BUS_ERROR_DATA_FETCH | CONFIG::EXC_ENABLE_BUS_ERROR_INSTRUCTION_FETCH |
			              CONFIG::EXC_ENABLE_OCD_STOP_CPU | CONFIG::EXC_ENABLE_INSTRUCTION_ADDR | CONFIG::EXC_ENABLE_ITLB_MISS | CONFIG::EXC_ENABLE_ITLB_PROTECTION |
			              CONFIG::EXC_ENABLE_BREAKPOINT | CONFIG::EXC_ENABLE_ILLEGAL_OPCODE | CONFIG::EXC_ENABLE_UNIMPLEMENTED_INSTRUCTION | CONFIG::EXC_ENABLE_PRIVILEGE_VIOLATION |
			              CONFIG::EXC_ENABLE_FLOATING_POINT | CONFIG::EXC_ENABLE_COPROCESSOR_ABSENT | CONFIG::EXC_ENABLE_SUPERVISOR_CALL | CONFIG::EXC_ENABLE_DATA_ADDRESS_READ |
			              CONFIG::EXC_ENABLE_DATA_ADDRESS_WRITE | CONFIG::EXC_ENABLE_DTLB_MISS_READ | CONFIG::EXC_ENABLE_DTLB_MISS_WRITE | CONFIG::EXC_ENABLE_DTLB_PROTECTION_READ |
			              CONFIG::EXC_ENABLE_DTLB_PROTECTION_WRITE | CONFIG::EXC_ENABLE_DTLB_MODIFIED | CONFIG::EXC_ENABLE_UNDEFINED_BEHAVIOR;
		}
	}

	inline void SetSRBit(unsigned int n)
	{
		SetSRWithExceptionUpdate(GetSR() | (1 << n));
	}
	inline void ClearSRBit(unsigned int n)
	{
		SetSRWithExceptionUpdate(GetSR() & ~(1 << n));
	}

	inline void SetSR_SS(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_SS_MASK) | ((val << CONFIG::SR_SS_OFFSET) & CONFIG::SR_SS_MASK)); }
	inline void SetSR_H(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_H_MASK) | ((val << CONFIG::SR_H_OFFSET) & CONFIG::SR_H_MASK)); }
	inline void SetSR_J(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_J_MASK) | ((val << CONFIG::SR_J_OFFSET) & CONFIG::SR_J_MASK)); }
	inline void SetSR_DM(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_DM_MASK) | ((val << CONFIG::SR_DM_OFFSET) & CONFIG::SR_DM_MASK)); }
	inline void SetSR_D(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_D_MASK) | ((val << CONFIG::SR_D_OFFSET) & CONFIG::SR_D_MASK)); }
	inline void SetSR_M2(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_M2_MASK) | ((val << CONFIG::SR_M2_OFFSET) & CONFIG::SR_M2_MASK)); }
	inline void SetSR_M1(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_M1_MASK) | ((val << CONFIG::SR_M1_OFFSET) & CONFIG::SR_M1_MASK)); }
	inline void SetSR_M0(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_M0_MASK) | ((val << CONFIG::SR_M0_OFFSET) & CONFIG::SR_M0_MASK)); }
	inline void SetSR_M(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_M_MASK) | ((val << CONFIG::SR_M_OFFSET) & CONFIG::SR_M_MASK)); }
	inline void SetSR_EM(uint32_t val) ALWAYS_INLINE { SetSRWithExceptionUpdate((GetSR() & ~CONFIG::SR_EM_MASK) | ((val << CONFIG::SR_EM_OFFSET) & CONFIG::SR_EM_MASK)); }
	inline void SetSR_I3M(uint32_t val) ALWAYS_INLINE { SetSRWithExceptionUpdate((GetSR() & ~CONFIG::SR_I3M_MASK) | ((val << CONFIG::SR_I3M_OFFSET) & CONFIG::SR_I3M_MASK)); }
	inline void SetSR_I2M(uint32_t val) ALWAYS_INLINE { SetSRWithExceptionUpdate((GetSR() & ~CONFIG::SR_I2M_MASK) | ((val << CONFIG::SR_I2M_OFFSET) & CONFIG::SR_I2M_MASK)); }
	inline void SetSR_I1M(uint32_t val) ALWAYS_INLINE { SetSRWithExceptionUpdate((GetSR() & ~CONFIG::SR_I1M_MASK) | ((val << CONFIG::SR_I1M_OFFSET) & CONFIG::SR_I1M_MASK)); }
	inline void SetSR_I0M(uint32_t val) ALWAYS_INLINE { SetSRWithExceptionUpdate((GetSR() & ~CONFIG::SR_I0M_MASK) | ((val << CONFIG::SR_I0M_OFFSET) & CONFIG::SR_I0M_MASK)); }
	inline void SetSR_GM(uint32_t val) ALWAYS_INLINE { SetSRWithExceptionUpdate((GetSR() & ~CONFIG::SR_GM_MASK) | ((val << CONFIG::SR_GM_OFFSET) & CONFIG::SR_GM_MASK)); }
	inline void SetSR_R(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_R_MASK) | ((val << CONFIG::SR_R_OFFSET) & CONFIG::SR_R_MASK)); }
	inline void SetSR_T(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_T_MASK) | ((val << CONFIG::SR_T_OFFSET) & CONFIG::SR_T_MASK)); }
	inline void SetSR_L(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_L_MASK) | ((val << CONFIG::SR_L_OFFSET) & CONFIG::SR_L_MASK)); }
	inline void SetSR_Q(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_Q_MASK) | ((val << CONFIG::SR_Q_OFFSET) & CONFIG::SR_Q_MASK)); }
	inline void SetSR_V(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_V_MASK) | ((val << CONFIG::SR_V_OFFSET) & CONFIG::SR_V_MASK)); }
	inline void SetSR_N(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_N_MASK) | ((val << CONFIG::SR_N_OFFSET) & CONFIG::SR_N_MASK)); }
	inline void SetSR_Z(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_Z_MASK) | ((val << CONFIG::SR_Z_OFFSET) & CONFIG::SR_Z_MASK)); }
	inline void SetSR_C(uint32_t val) ALWAYS_INLINE { SetSR((GetSR() & ~CONFIG::SR_C_MASK) | ((val << CONFIG::SR_C_OFFSET) & CONFIG::SR_C_MASK)); }
	
	      
	inline void SetEvba(uint32_t val) ALWAYS_INLINE { evba=val;}
	inline void SetAcba(uint32_t val) ALWAYS_INLINE { acba=val;}
	inline void SetCpucr(uint32_t val) ALWAYS_INLINE {cpucr=val;}
	inline void SetConfig0(uint32_t val) ALWAYS_INLINE {config0=val;}
	inline void SetConfig1(uint32_t val) ALWAYS_INLINE {config1=val;}
	inline void SetCount(uint32_t val) ALWAYS_INLINE {count=val;}
	inline void SetCompare(uint32_t val) ALWAYS_INLINE {compare=val;}
	inline void SetSS_RAR(uint32_t val) ALWAYS_INLINE {ss_rar=val;}
	inline void SetSS_RSR(uint32_t val) ALWAYS_INLINE {ss_rsr=val;}
	void SwitchExecutionMode(uint32_t execution_mode);
	
	bool MoveFromSystemRegister(unsigned int rd, unsigned int sr_num);
	
	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================
	
	virtual bool BeginSetup();
	virtual bool EndSetup();
	virtual void OnDisconnect();
	
	//=====================================================================
	//=                    execution handling methods                     =
	//=====================================================================

	void StepOneInstruction();
	virtual void Synchronize();
	virtual void Reset();
	virtual void Idle();

	//=====================================================================
	//=             memory access reporting control interface methods     =
	//=====================================================================

	virtual void RequiresMemoryAccessReporting(MemoryAccessReportingType type, bool report);

	//=====================================================================
	//=               Programmer view memory access methods               =
	//=====================================================================
	
	virtual void ResetMemory();
	virtual bool ReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size);
	virtual bool InjectReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size);
	virtual bool InjectWriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=                        Debugging stuffs                           =
	//=====================================================================

	virtual unisim::service::interfaces::Register *GetRegister(const char *name);
	virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);
	virtual string Disasm(typename CONFIG::address_t addr, typename CONFIG::address_t& next_addr);
	string GetObjectFriendlyName(typename CONFIG::address_t addr);
	string GetFunctionFriendlyName(typename CONFIG::address_t addr);

	//=====================================================================
	//=               Hardware check/acknowledgement methods              =
	//=====================================================================
	
	void SetNMIREQ();
	void ResetNMIREQ();
	void SetIRQ(unsigned int irq);
	void ResetIRQ(unsigned int irq);
	
	inline void SetException(unsigned int exc_num) ALWAYS_INLINE { SetExceptionFlags(1 << exc_num); }
	inline void ResetException(unsigned int exc_num) ALWAYS_INLINE { ResetExceptionFlags(1 << exc_num); }
	inline void SetExceptionFlags(uint32_t _exc_flags) ALWAYS_INLINE { exc_flags = exc_flags | _exc_flags;  }
	inline void ResetExceptionFlags(uint32_t _exc_flags) ALWAYS_INLINE { exc_flags = exc_flags & ~_exc_flags;  }
	inline void SetExceptionEnable(uint32_t _exc_enable) ALWAYS_INLINE { exc_enable = exc_enable | _exc_enable;  }
	inline void ResetExceptionEnable(uint32_t _exc_enable) ALWAYS_INLINE { exc_enable = exc_enable & ~_exc_enable;  }

	//=====================================================================
	//=                     Memory access methods                         =
	//=====================================================================
	
	bool Fetch(typename CONFIG::address_t addr,void *buffer, uint32_t size);
	
	bool UintLoadByte(unsigned int rd,typename CONFIG::address_t addr);
	bool UintLoadHalfWord(unsigned int rd,typename CONFIG::address_t addr);
	bool IntLoadWord(unsigned int rd,typename CONFIG::address_t addr);
	bool SintLoadByte(unsigned int,typename CONFIG::address_t addr);
	bool SintLoadHalfWord(unsigned int rd,typename CONFIG::address_t addr);
	bool LoadAndInsertByte(unsigned int rd,typename CONFIG::address_t addr,unsigned int part);
	bool LoadAndInsertHalfWord(unsigned int rd,typename CONFIG::address_t addr,unsigned int part);
	bool LoadSR(typename CONFIG::address_t addr);

	bool IntStoreByte(unsigned int rs,typename CONFIG::address_t addr);
	bool IntStoreHalfWord(unsigned int rs,typename CONFIG::address_t addr);
	bool IntStoreWord(unsigned int rs,typename CONFIG::address_t addr);
	bool StoreHalfWordIntoWord(unsigned int rx,unsigned int ry,unsigned int x_part,unsigned int y_part,typename CONFIG::address_t addr);
	bool SwapAndStoreHalfWord(unsigned int rs, typename CONFIG::address_t addr);
	bool SwapAndStoreWord(unsigned int rs, typename CONFIG::address_t addr);
	
	bool UintLoadHalfWordAndSwap(unsigned int rd,typename CONFIG::address_t addr);
	bool IntLoadWordAndSwap(unsigned int rd,typename CONFIG::address_t addr);
	bool SintLoadHalfWordAndSwap(unsigned int rd,typename CONFIG::address_t addr);
	bool ExchangeRegMem(unsigned int rd,unsigned int rx,unsigned int ry);
	bool MemoryBitAccess(typename CONFIG::address_t addr,unsigned int mode, unsigned int pos);
	
	/*
	bool MemReadByte(typename CONFIG::address_t addr,uint32_t& value);
	bool MemReadHalfWord(typename CONFIG::address_t addr,uint32_t& value);
	bool MemReadWord(typename CONFIG::address_t addr,uint32_t& value);
	bool MemWriteByte(uint32_t value,typename CONFIG::address_t addr);
	bool MemWriteHalfWord(uint32_t value,typename CONFIG::address_t addr);
	bool MemWriteWord(uint32_t value,typename CONFIG::address_t addr);
	*/
	//=====================================================================
	//=                         System call                               =
	//=====================================================================

	bool Breakpoint();

	//=====================================================================
	//=                                                                   =
	//=====================================================================

	bool EvaluateCond(uint8_t cond);
	string GetCond(uint8_t cond);	
	
protected:

	
	//=====================================================================
	//=                         Timers handling                           =
	//=====================================================================
	
	uint64_t GetMaxIdleTime() const
	{
		// TODO: compute the maximum idle time, i.e. the time without any internal timer interrupts
		return 0xffffffffffffffffULL;
	}

	uint64_t GetTimersDeadline() const
	{
		// TODO: compute the timers deadline, i.e. the time until an internal timer expires
		return 0xffffffffffffffffULL;
	}

	inline void RunTimers(uint64_t delta) ALWAYS_INLINE
	{
		// TODO: run timers
	}
	
	virtual void RunInternalTimers();
	
	//=====================================================================
	//=                        Debugging stuff                            =
	//=====================================================================

	Logger logger;
	
	bool requires_memory_access_reporting;      //< indicates if the memory accesses require to be reported
	bool requires_fetch_instruction_reporting;  //< indicates if the fetched instructions require to be reported
	bool requires_commit_instruction_reporting; //< indicates if the committed instructions require to be reported
	
	inline bool IsVerboseSetup() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_SETUP_ENABLE && (verbose_all || verbose_setup); }
	inline bool IsVerboseInterrupt() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_INTERRUPT_ENABLE && (verbose_all || verbose_interrupt); }
	inline bool IsVerboseStep() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_STEP_ENABLE && (verbose_all || verbose_step); }
	
	//=====================================================================
	//=                      Bus access methods                           =
	//=====================================================================

	virtual bool IHSBRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size) = 0;
	virtual bool DHSBRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size) = 0;
	virtual bool DHSBWrite(typename CONFIG::physical_address_t physical_addr, const void *buffer, uint32_t size) = 0;

public:
	inline void ProcessExceptions(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation) ALWAYS_INLINE;



private:	
	//=====================================================================
	//=                      Debugging stuffs                             =
	//=====================================================================
	bool verbose_all;
	bool verbose_setup;
	bool verbose_interrupt;
	bool verbose_step;
	uint64_t trap_on_instruction_counter;
	bool enable_trap_on_exception;
	typename CONFIG::address_t halt_on_addr;
	std::string halt_on;
	uint64_t max_inst;                                         //!< maximum number of instructions to execute

	unisim::util::debug::SimpleRegisterRegistry registers_registry;       //!< Every CPU register interfaces
	uint64_t instruction_counter;                              //!< Number of executed instructions

	inline uint64_t GetInstructionCounter() const ALWAYS_INLINE { return instruction_counter; }
	inline void MonitorLoad(typename CONFIG::address_t ea, uint32_t size) ALWAYS_INLINE;
	inline void MonitorStore(typename CONFIG::address_t ea, uint32_t size) ALWAYS_INLINE;

	//=====================================================================
	//=                         AVR32A registers                          =
	//=====================================================================


	uint32_t gpr[16];   // register file gpr[15]=pc,gpr[14]=lr , gpr[13]=sp
	uint32_t npc;       //address of next instruction
	uint32_t sp_app;      // sp shadow register for application mode
	uint32_t sp_sys;      // sp shadow register for privileged mode


        //required system registers

	uint32_t sr;      // stack register
        uint32_t evba;
	uint32_t acba;
	uint32_t cpucr;
	uint32_t config0;
	uint32_t config1;
	uint32_t count;
	uint32_t compare;
	 

	//optional system registers
	uint32_t ecr;
	uint32_t rsr_dbg;
	uint32_t rar_dbg;	
	uint32_t jecr;
	
 	uint32_t josp;
	uint32_t java_lv0,java_lv1,java_lv2,java_lv3,java_lv4,java_lv5,java_lv6,java_lv7;
	uint32_t jtba;
	uint32_t jtcr;
	uint32_t tlbehi;
	uint32_t tlbelo;
	uint32_t ptbr;
	uint32_t tlbear;
	uint32_t mmucr;
	uint32_t tlbarlo;
	uint32_t tlbarhi;
	uint32_t pccnt;
	uint32_t pcnt0,pcnt1;
	uint32_t pccr;
	uint32_t bear;
	uint32_t mpuar0,mpuar1,mpuar2,mpuar3,mpuar4,mpuar5,mpuar6,mpuar7;
	uint32_t mpupsr0,mpupsr1,mpupsr2,mpupsr3,mpupsr4,mpupsr5,mpupsr6,mpupsr7;
	uint32_t mpucra,mpucrb;
	uint32_t mpubra,mpubrb;
	uint32_t mpuapra,mpuaprb;
	uint32_t mpcr;
	uint32_t ss_status;
	uint32_t ss_adrf;
	uint32_t ss_adrr;
	uint32_t ss_adr0,ss_adr1;
	uint32_t ss_sp_sys;
	uint32_t ss_sp_app;
	uint32_t ss_rar;
	uint32_t ss_rsr;

	string micro_architecture;

	//=====================================================================
	//=                            Exceptions                             =
	//=====================================================================
	
	uint32_t exc_flags;                                       //!< exception flags (bits are ordered according to exception priority)
	uint32_t exc_enable;                                      //!< exception enable  (bits are ordered according to exception priority)
//	typename CONFIG::address_t exc_addr;                      //!< effective address, if any, that caused the exception
//	typename CONFIG::MemoryAccessType exc_memory_access_type; //!< memory access type, if any, that caused the exception
	void (unisim::component::cxx::processor::avr32::avr32a::avr32uc::CPU<CONFIG>::*enter_isr_table[CONFIG::NUM_EXCEPTIONS])(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	
	void EnterUndefinedBehaviorException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterResetException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterOCDStopCPUException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterUnrecoverableException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterTLBMultipleHitException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterBusErrorDataFetchException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterBusErrorInstructionFetchException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterNMIException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterIRQ3Exception(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterIRQ2Exception(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterIRQ1Exception(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterIRQ0Exception(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterInstructionAddrException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterITLBMissException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterITLBProtectionException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterBreakpointException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterIllegalOpcodeException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterUnimplementedInstructionException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterPrivilegeViolationException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterFloatingPointException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterCoprocessorAbsentException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterSupervisorCallException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterDataAddressReadException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterDataAddressWriteException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterDTLBMissReadException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterDTLBMissWriteException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterDTLBProtectionReadException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterDTLBProtectionWriteException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	void EnterDTLBModifiedException(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation);
	
	//=====================================================================
	//=                    CPU run-time parameters                        =
	//=====================================================================
	
	Parameter<uint64_t> param_max_inst;                   //!< linked to member max_inst
	Parameter<bool> param_verbose_all;
	Parameter<bool> param_verbose_setup;
	Parameter<bool> param_verbose_interrupt;
	Parameter<bool> param_verbose_step;
	Parameter<uint64_t> param_trap_on_instruction_counter;
	Parameter<bool> param_enable_trap_on_exception;
	Parameter<std::string> param_halt_on;

	//=====================================================================
	//=                    CPU run-time statistics                        =
	//=====================================================================

	Statistic<uint64_t> stat_instruction_counter;
};

} // end of namespace avr32uc
} // end of namespace avr32a
} // end of namespace avr32
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
