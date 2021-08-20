/*
 *  Copyright (c) 2007-2017,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CPU_TCC__

#include <unisim/component/cxx/processor/powerpc/cpu.hh>
#include <unisim/util/reg/core/register.tcc>
#include <unisim/util/debug/simple_register.hh>
#include <cctype>

#ifdef BitScanForward
#undef BitScanForward
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

////////////////////////////// TypeForBitSize<> ///////////////////////////////

template <unsigned int SIZE> const unsigned int TypeForBitSize<SIZE>::BYTE_SIZE;
template <unsigned int SIZE> const typename TypeForBitSize<SIZE>::TYPE TypeForBitSize<SIZE>::MASK;

/////////////////////////////////// CPU<> /////////////////////////////////////

#if 0
template <typename TYPES, typename CONFIG>
template <typename SLR_REGISTER, typename CPU<TYPES, CONFIG>::SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM, typename CPU<TYPES, CONFIG>::SLR_Access_Type _SLR_ACCESS, typename CPU<TYPES, CONFIG>::SLR_Privilege_Type _SLR_PRIVILEGE>
const typename CPU<TYPES, CONFIG>::SLR_Space_Type CPU<TYPES, CONFIG>::SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, _SLR_ACCESS, _SLR_PRIVILEGE>::SLR_SPACE;

template <typename TYPES, typename CONFIG>
template <typename SLR_REGISTER, typename CPU<TYPES, CONFIG>::SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM, typename CPU<TYPES, CONFIG>::SLR_Access_Type _SLR_ACCESS, typename CPU<TYPES, CONFIG>::SLR_Privilege_Type _SLR_PRIVILEGE>
const unsigned int CPU<TYPES, CONFIG>::SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, _SLR_ACCESS, _SLR_PRIVILEGE>::SLR_NUM;

template <typename TYPES, typename CONFIG>
template <typename SLR_REGISTER, typename CPU<TYPES, CONFIG>::SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM, typename CPU<TYPES, CONFIG>::SLR_Access_Type _SLR_ACCESS, typename CPU<TYPES, CONFIG>::SLR_Privilege_Type _SLR_PRIVILEGE>
const typename CPU<TYPES, CONFIG>::SLR_Access_Type CPU<TYPES, CONFIG>::SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, _SLR_ACCESS, _SLR_PRIVILEGE>::SLR_ACCESS;

template <typename TYPES, typename CONFIG>
template <typename SLR_REGISTER, typename CPU<TYPES, CONFIG>::SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM, typename CPU<TYPES, CONFIG>::SLR_Access_Type _SLR_ACCESS, typename CPU<TYPES, CONFIG>::SLR_Privilege_Type _SLR_PRIVILEGE>
const typename CPU<TYPES, CONFIG>::SLR_Privilege_Type CPU<TYPES, CONFIG>::SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, _SLR_ACCESS, _SLR_PRIVILEGE>::SLR_PRIVILEGE;
#endif

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::CPU(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, SuperMSS()
	, unisim::kernel::Client<unisim::service::interfaces::Memory<PHYSICAL_ADDRESS> >(name, parent)
	, unisim::kernel::Client<typename unisim::service::interfaces::SymbolTableLookup<typename TYPES::EFFECTIVE_ADDRESS> >(name, parent)
	, unisim::kernel::Client<typename unisim::service::interfaces::DebugYielding>(name, parent)
	, unisim::kernel::Client<typename unisim::service::interfaces::MemoryAccessReporting<typename TYPES::EFFECTIVE_ADDRESS> >(name, parent)
	, unisim::kernel::Client<typename unisim::service::interfaces::TrapReporting>(name, parent)
	, unisim::kernel::Client<typename unisim::service::interfaces::LinuxOS>(name, parent)
	, unisim::kernel::Service<unisim::service::interfaces::Memory<EFFECTIVE_ADDRESS> >(name, parent)
	, unisim::kernel::Service<typename unisim::service::interfaces::MemoryAccessReportingControl>(name, parent)
	, unisim::kernel::Service<typename unisim::service::interfaces::Registers>(name, parent)
	, unisim::kernel::Service<typename unisim::service::interfaces::Synchronizable>(name, parent)
	, unisim::kernel::Service<typename unisim::service::interfaces::MemoryInjection<EFFECTIVE_ADDRESS> >(name, parent)
	, memory_import("memory-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, debug_yielding_import("debug-yielding-import", this)
	, memory_access_reporting_import("memory-access-reporting-import", this)
	, trap_reporting_import("trap-reporting-import", this)
	, linux_os_import("linux-os-import", this)
	, memory_export("memory-export", this)
	, memory_access_reporting_control_export("memory-access-reporting-control-export", this)
	, registers_export("registers-export", this)
	, synchronizable_export("synchronizable-export", this)
	, memory_injection_export("memory-injection-export", this)
	, logger(*this)
	, requires_memory_access_reporting(false)
	, requires_fetch_instruction_reporting(false)
	, requires_commit_instruction_reporting(false)
	, instruction_counter(0)
	, stat_instruction_counter("instruction-counter", this, instruction_counter, "number of simulated instructions")
	, stat_num_data_load_accesses("num-data-load-accesses", this, this->num_data_load_accesses, "Number of data load accesses (inside core)")
	, stat_num_data_store_accesses("num-data-store-accesses", this, this->num_data_store_accesses, "Number of data store accesses (inside core)")
	, stat_num_instruction_fetch_accesses("num-instruction-fetch-accesses", this, this->num_instruction_fetch_accesses, "Number of instruction fetch accesses (inside core)")
	, stat_num_incoming_load_accesses("num-incoming-load-accesses", this, this->num_incoming_load_accesses, "Number of incoming load accesses (from other masters)")
	, stat_num_incoming_store_accesses("num-incoming-store-accesses", this, this->num_incoming_store_accesses, "Number of incoming store accesses (from other masters)")
	, stat_num_data_bus_read_accesses("num-data-bus-read-accesses", this, this->num_data_bus_read_accesses, "Number of data bus read accesses")
	, stat_num_data_bus_write_accesses("num-data-bus-write-accesses", this, this->num_data_bus_write_accesses, "Number of data bus write accesses")
	, stat_num_instruction_bus_read_accesses("num-instruction-bus-read-accesses", this, this->num_instruction_bus_read_accesses, "Number of instruction bus read accesses")
	, stat_num_data_load_xfered_bytes("num-data-load-xfered-bytes", this, this->num_data_load_xfered_bytes, "Number of data load transfered bytes")
	, stat_num_data_store_xfered_bytes("num-data-store-xfered-bytes", this, this->num_data_store_xfered_bytes, "Number of data store transfered bytes")
	, stat_num_instruction_fetch_xfered_bytes("num-instruction-fetch-xfered-bytes", this, this->num_instruction_fetch_xfered_bytes, "Number of instruction fetch transfered bytes")
	, stat_num_incoming_load_xfered_bytes("num-incoming-load-xfered-bytes", this, this->num_incoming_load_xfered_bytes, "Number of incoming load transfered bytes")
	, stat_num_incoming_store_xfered_bytes("num-incoming-store-xfered-bytes", this, this->num_incoming_store_xfered_bytes, "Number of incoming store transfered bytes")
	, stat_num_data_bus_read_xfered_bytes("num-data-bus-read-xfered-bytes", this, this->num_data_bus_read_xfered_bytes, "Number of data bus read transfered bytes")
	, stat_num_data_bus_write_xfered_bytes("num-data-bus-write-xfered-bytes", this, this->num_data_bus_write_xfered_bytes, "Number of data bus write transfered bytes")
	, stat_num_instruction_bus_read_xfered_bytes("num-instruction-bus-read-xfered-bytes", this, this->num_instruction_bus_read_xfered_bytes, "Number of instruction bus read transfered bytes")
	, trap_on_instruction_counter(0xffffffffffffffffULL)
	, param_trap_on_instruction_counter("trap-on-instruction-counter", this, trap_on_instruction_counter, "number of simulated instruction before traping")
	, max_inst(0xffffffffffffffffULL)
	, param_max_inst("max-inst", this, max_inst, "maximum number of instructions to simulate")
	, halt_on_addr(~typename TYPES::EFFECTIVE_ADDRESS(0))
	, halt_on()
	, param_halt_on("halt-on", this, halt_on, "Symbol or address where to stop simulation")
	, verbose_setup(false)
	, param_verbose_setup("verbose-setup", this, verbose_setup, "enable/disable verbosity while setup")
	, verbose_exception(false)
	, param_verbose_exception("verbose-exception", this, verbose_exception, "enable/disable verbosity of exception")
	, verbose_interrupt(false)
	, param_verbose_interrupt("verbose-interrupt", this, verbose_interrupt, "enable/disable verbosity of interrupt")
	, verbose_move_to_slr(false)
	, param_verbose_move_to_slr("verbose-move-to-slr", this, verbose_move_to_slr, "enable/disable verbosity of move to system level registers (SPRs, and so on)")
	, verbose_move_from_slr(false)
	, param_verbose_move_from_slr("verbose-move-from-slr", this, verbose_move_from_slr, "enable/disable verbosity of move from system level registers (SPRs, and so on)")
	, enable_insn_trace(false)
	, param_enable_insn_trace("enable-insn-trace", this, enable_insn_trace, "enable/disable instruction trace")
	, verbose_data_load(false)
	, param_verbose_data_load("verbose-data-load", this, verbose_data_load, "enable/disable verbosity of data load")
	, verbose_data_store(false)
	, param_verbose_data_store("verbose-data-store", this, verbose_data_store, "enable/disable verbosity of data store")
	, verbose_instruction_fetch(false)
	, param_verbose_instruction_fetch("verbose-instruction-fetch", this, verbose_instruction_fetch, "enable/disable verbosity of instruction fetch")
	, verbose_data_bus_read(false)
	, param_verbose_data_bus_read("verbose-data-bus-read", this, verbose_data_bus_read, "enable/disable verbosity of data bus read")
	, verbose_data_bus_write(false)
	, param_verbose_data_bus_write("verbose-data-bus-write", this, verbose_data_bus_write, "enable/disable verbosity of data bus write")
	, verbose_instruction_bus_read(false)
	, param_verbose_instruction_bus_read("verbose-instruction-bus-read", this, verbose_instruction_bus_read, "enable/disable verbosity of instruction bus read")
	, enable_linux_syscall_snooping(false)
	, param_enable_linux_syscall_snooping("enable-linux-syscall-snooping", this, enable_linux_syscall_snooping, "enable/disable Linux system call snooping")
	, param_enable_shadow_memory("enable-shadow-memory", this, this->enable_shadow_memory, "enable/disable shadow memory")
	, registers_registry()
	, exception_dispatcher(static_cast<typename CONFIG::CPU *>(this))
	, invalid_spr(static_cast<typename CONFIG::CPU *>(this))
	, invalid_tbr(static_cast<typename CONFIG::CPU *>(this))
	, invalid_pmr(static_cast<typename CONFIG::CPU *>(this))
	, invalid_tmr(static_cast<typename CONFIG::CPU *>(this))
	, external_dcr(static_cast<typename CONFIG::CPU *>(this))
	, slr()
	, reset_addr(0x0)
	, param_reset_addr("reset-addr", this, reset_addr, "reset address")
	, cia(0)
	, nia(0)
	, gpr()
	, xer()
	, lr()
	, ctr()
	, cr()
	, fpu(static_cast<typename CONFIG::CPU *>(this))
	, vector_unit(static_cast<typename CONFIG::CPU *>(this))
	, powerpc_linux32(this)
{
	memory_export.SetupDependsOn(memory_import);
	
	stat_instruction_counter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_trap_on_instruction_counter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_max_inst.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

	unsigned int i;
	
	for(i = 0; i < 32; i++)
	{
		GPR& ith_gpr = gpr[i];
		ith_gpr.Init(i);
		AddRegisterInterface(ith_gpr.CreateRegisterInterface());
	}
	AddRegisterInterface(xer.CreateRegisterInterface());
	AddRegisterInterface(lr.CreateRegisterInterface());
	AddRegisterInterface(ctr.CreateRegisterInterface());
	AddRegisterInterface(cr.CreateRegisterInterface());

	AddRegisterInterface(new unisim::util::debug::SimpleRegister<uint32_t>("pc", "Program Counter", &cia));
	AddRegisterInterface(new unisim::util::debug::SimpleRegister<uint32_t>("cia", "Current Instruction Address", &cia));
}

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::~CPU()
{
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::BeginSetup()
{
	unsigned int slr_space_idx;
	for(slr_space_idx = SLR_SPR_SPACE; slr_space_idx <= SLR_TMR_SPACE; slr_space_idx++)
	{
		unsigned int slr_reg_num;
		for(slr_reg_num = 0; slr_reg_num < 1024; slr_reg_num++)
		{
			SLRBase *slr_p = slr[slr_space_idx][slr_reg_num];
			
			if(slr_p)
			{
				slr_p->AddRegisterInterface();
			}
		}
	}
	
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::EndSetup()
{
	if(!halt_on.empty())
	{
		const unisim::util::debug::Symbol<typename TYPES::EFFECTIVE_ADDRESS> *halt_on_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByName(halt_on.c_str(), unisim::util::debug::Symbol<typename TYPES::EFFECTIVE_ADDRESS>::SYM_FUNC) : 0;
		
		if(halt_on_symbol)
		{
			halt_on_addr = halt_on_symbol->GetAddress();
			if(verbose_setup)
			{
				logger << DebugInfo << "Simulation will halt at '" << halt_on_symbol->GetName() << "' (0x" << std::hex << halt_on_addr << std::dec << ")" << EndDebugInfo;
			}
		}
		else
		{
			std::stringstream sstr(halt_on);
			sstr >> std::hex;
			if(sstr >> halt_on_addr)
			{
				if(verbose_setup)
				{
					logger << DebugInfo <<  "Simulation will halt at 0x" << std::hex << halt_on_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				logger << DebugWarning << "Invalid address (" << halt_on << ") in Parameter " << param_halt_on.GetName() << EndDebugWarning;
				halt_on_addr = (typename TYPES::EFFECTIVE_ADDRESS) -1;
			}
		}
	}
	
	return true;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::Reset()
{
	// GPRs and CR are unaffected
	
	unsigned int slr_space_idx;
	for(slr_space_idx = SLR_SPR_SPACE; slr_space_idx <= SLR_TMR_SPACE; slr_space_idx++)
	{
		unsigned int slr_reg_num;
		for(slr_reg_num = 0; slr_reg_num < 1024; slr_reg_num++)
		{
			SLRBase *slr_p = slr[slr_space_idx][slr_reg_num];
			
			if(slr_p)
			{
				slr_p->Reset();
			}
		}
	}
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::RegisterSLR(unsigned int n, SLRBase *slr_p)
{
	//std::cerr << "registering #" << slr_p->GetRegNum() << " of space #" << slr_p->GetSpace() << std::endl;
	SLR_Space_Type slr_space = slr_p->GetSpace();
	assert(slr[slr_space][n] == 0);
	slr[slr_space][n] = slr_p;
}

template <typename TYPES, typename CONFIG>
typename CPU<TYPES, CONFIG>::SPRBase& CPU<TYPES, CONFIG>::GetSPR(unsigned int n)
{
	SLRBase *slr_p = slr[SLR_SPR_SPACE][n];
	
	if(slr_p) return *slr_p;
	
	return invalid_spr;
}

template <typename TYPES, typename CONFIG>
typename CPU<TYPES, CONFIG>::TBRBase& CPU<TYPES, CONFIG>::GetTBR(unsigned int n)
{
	SLRBase *slr_p = slr[SLR_TBR_SPACE][n];
	
	if(slr_p) return *slr_p;
	
	return invalid_tbr;
}

template <typename TYPES, typename CONFIG>
typename CPU<TYPES, CONFIG>::DCRBase& CPU<TYPES, CONFIG>::GetDCR(unsigned int n)
{
	SLRBase *slr_p = slr[SLR_DCR_SPACE][n];

	if(slr_p) return *slr_p;
	
	external_dcr.Attach(n);
	
	return external_dcr;
}

template <typename TYPES, typename CONFIG>
typename CPU<TYPES, CONFIG>::PMRBase& CPU<TYPES, CONFIG>::GetPMR(unsigned int n)
{
	SLRBase *slr_p = slr[SLR_PMR_SPACE][n];

	if(slr_p) return *slr_p;
	
	return invalid_pmr;
}

template <typename TYPES, typename CONFIG>
typename CPU<TYPES, CONFIG>::TMRBase& CPU<TYPES, CONFIG>::GetTMR(unsigned int n)
{
	SLRBase *slr_p = slr[SLR_TMR_SPACE][n];

	if(slr_p) return *slr_p;
	
	return invalid_tmr;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveFromSPR(unsigned int n, uint32_t& value)
{
	switch(n)
	{
		case 1:
			value = xer;
			return true;
		case 8:
			value = lr;
			return true;
		case 9:
			value = ctr;
			return true;
	}
	
	SPRBase& spr = GetSPR(n);
	if(!spr.CheckMoveFromLegality()) return false;
	return spr.MoveFrom(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveToSPR(unsigned int n, uint32_t value)
{
	switch(n)
	{
		case 1:
			xer = value;
			return true;
		case 8:
			lr = value;
			return true;
		case 9:
			ctr = value;
			return true;
	}

	SPRBase& spr = GetSPR(n);
	if(!spr.CheckMoveToLegality()) return false;
	return spr.MoveTo(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveFromTBR(unsigned int n, uint32_t& value)
{
	TBRBase& tbr = GetTBR(n);
	if(!tbr.CheckMoveFromLegality()) return false;
	return tbr.MoveFrom(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveToTBR(unsigned int n, uint32_t value)
{
	TBRBase& tbr = GetTBR(n);
	if(!tbr.CheckMoveToLegality()) return false;
	return tbr.MoveTo(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveFromDCR(unsigned int n, uint32_t& value)
{
	DCRBase& dcr = GetDCR(n);
	if(!dcr.CheckMoveFromLegality()) return false;
	return dcr.MoveFrom(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveToDCR(unsigned int n, uint32_t value)
{
	DCRBase& dcr = GetDCR(n);
	if(!dcr.CheckMoveToLegality()) return false;
	return dcr.MoveTo(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveFromPMR(unsigned int n, uint32_t& value)
{
	PMRBase& pmr = GetPMR(n);
	if(!pmr.CheckMoveFromLegality()) return false;
	return pmr.MoveFrom(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveToPMR(unsigned int n, uint32_t value)
{
	PMRBase& pmr = GetPMR(n);
	if(!pmr.CheckMoveToLegality()) return false;
	return pmr.MoveTo(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveFromTMR(unsigned int n, uint32_t& value)
{
	TMRBase& tmr = GetTMR(n);
	if(!tmr.CheckMoveFromLegality()) return false;
	return tmr.MoveFrom(value);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::MoveToTMR(unsigned int n, uint32_t value)
{
	TMRBase& tmr = GetTMR(n);
	if(!tmr.CheckMoveToLegality()) return false;
	return tmr.MoveTo(value);
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::AddRegisterInterface(unisim::service::interfaces::Register *reg_if)
{
	registers_registry.AddRegisterInterface(reg_if);
}

/////////////////////////// ExceptionDispatcher<> /////////////////////////////

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::ExceptionDispatcher(typename CONFIG::CPU *_cpu)
	: cpu(_cpu)
	, exc_flags(0)
	, exc_mask(0)
	, interrupts()
{
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
void CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::InstallInterrupt(InterruptBase *interrupt, unsigned int priority)
{
	assert(interrupts[priority] == 0);
	interrupts[priority] = interrupt;
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
template <typename EXCEPTION>
typename EXCEPTION::INTERRUPT& CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::ThrowException()
{
	if(cpu->verbose_exception && !(exc_flags & CPU::template GetExceptionMask<EXCEPTION, MASK_TYPE>()))
	{
		cpu->GetDebugInfoStream() << "instruction #" << cpu->instruction_counter << ":Throwing " << EXCEPTION::GetName() << std::endl;
	}
	exc_flags = exc_flags | CPU::template GetExceptionMask<EXCEPTION, MASK_TYPE>();
	
	unsigned int priority = CPU::template GetExceptionPriority<EXCEPTION>();
	InterruptBase *interrupt = interrupts[priority];
	
	assert(interrupt);

	return *static_cast<typename EXCEPTION::INTERRUPT *>(interrupt);
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
template <typename EXCEPTION> void CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::AckException()
{
	if(cpu->verbose_exception && (exc_flags & CPU::template GetExceptionMask<EXCEPTION, MASK_TYPE>()))
	{
		cpu->GetDebugInfoStream() << "Acknowledging " <<EXCEPTION::GetName() << std::endl;
	}
	exc_flags = exc_flags & ~CPU::template GetExceptionMask<EXCEPTION, MASK_TYPE>();
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
template <typename INTERRUPT> void CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::AckInterrupt()
{
	if(cpu->verbose_exception && (exc_flags & INTERRUPT::template GetMask<MASK_TYPE>()))
	{
		cpu->GetDebugInfoStream() << "instruction #" << cpu->instruction_counter << ":Acknowledging " <<INTERRUPT::GetName() << std::endl;
	}
	exc_flags = exc_flags & ~INTERRUPT::template GetMask<MASK_TYPE>();
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
template <typename EXCEPTION> void CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::EnableException(bool v)
{
	if(cpu->verbose_interrupt)
	{
		if(v && ((exc_mask & CPU::template GetExceptionMask<EXCEPTION, MASK_TYPE>()) == 0))
		{
			cpu->GetDebugInfoStream() << "instruction #" << cpu->instruction_counter << ":Enabling " << EXCEPTION::GetName() << std::endl;
		}
		else if(!v && ((exc_mask & CPU::template GetExceptionMask<EXCEPTION, MASK_TYPE>()) != 0))
		{
			cpu->GetDebugInfoStream() << "instruction #" << cpu->instruction_counter << ":Disabling " << EXCEPTION::GetName() << std::endl;
		}
	}
	if(v)
	{
		exc_mask = exc_mask | CPU::template GetExceptionMask<EXCEPTION, MASK_TYPE>();
	}
	else
	{
		exc_mask = exc_mask & ~CPU::template GetExceptionMask<EXCEPTION, MASK_TYPE>();
	}
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
template <typename INTERRUPT> void CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::EnableInterrupt(bool v)
{
	if(cpu->verbose_interrupt)
	{
		if(v && ((exc_mask & INTERRUPT::template GetMask<MASK_TYPE>()) == 0))
		{
			cpu->GetDebugInfoStream() << "instruction #" << cpu->instruction_counter << ":Enabling " << INTERRUPT::GetName() << std::endl;
		}
		else if(!v && ((exc_mask & INTERRUPT::template GetMask<MASK_TYPE>()) != 0))
		{
			cpu->GetDebugInfoStream() << "instruction #" << cpu->instruction_counter << ":Disabling " << INTERRUPT::GetName() << std::endl;
		}
	}
	if(v)
	{
		exc_mask = exc_mask | INTERRUPT::template GetMask<MASK_TYPE>();
	}
	else
	{
		exc_mask = exc_mask & ~INTERRUPT::template GetMask<MASK_TYPE>();
	}
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
inline bool CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::HasPendingInterrupts() const
{
	unsigned int exception_priority;
	return unisim::util::arithmetic::BitScanForward(exception_priority, exc_flags & exc_mask);
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
inline void CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::ProcessExceptions()
{
	unsigned int exception_priority;
	if(unlikely(unisim::util::arithmetic::BitScanForward(exception_priority, exc_flags & exc_mask)))
	{
		InterruptBase *interrupt = interrupts[exception_priority];

		assert(interrupt != 0);
		if(unlikely(cpu->verbose_interrupt))
		{
			cpu->GetDebugInfoStream() << "instruction #" << cpu->instruction_counter << ":" << interrupt->GetInterruptName() << " (offset 0x" << std::hex << interrupt->GetOffset() << std::dec << ") is interrupting execution at @0x" << std::hex << cpu->GetCIA() << std::dec << std::endl;
		}
		
		if(unlikely(interrupt->Trap()))
		{
			if(interrupt->Trapped())
			{
				interrupt->Trapped(false);
			}
			else
			{
				cpu->trap_reporting_import->ReportTrap(*cpu, interrupt->GetInterruptName());
				interrupt->Trapped(true);
				return;
			}
		}
			
		interrupt->ProcessInterrupt(cpu);
	}
}

template <typename TYPES, typename CONFIG>
template <unsigned int NUM_EXCEPTIONS>
template <typename EXCEPTION> bool CPU<TYPES, CONFIG>::ExceptionDispatcher<NUM_EXCEPTIONS>::RecognizedException() const
{
	return (exc_flags & exc_mask) & CPU::template GetExceptionMask<EXCEPTION, MASK_TYPE>();
}

template <typename TYPES, typename CONFIG>
inline bool CPU<TYPES, CONFIG>::MonitorLoad(typename TYPES::EFFECTIVE_ADDRESS ea, unsigned int size)
{
	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		return memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, ea, size);
	}
	
	return true;
}

template <typename TYPES, typename CONFIG>
inline bool CPU<TYPES, CONFIG>::MonitorStore(typename TYPES::EFFECTIVE_ADDRESS ea, unsigned int size)
{
	// Memory access reporting
	if(unlikely(requires_memory_access_reporting && memory_access_reporting_import))
	{
		return memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, ea, size);
	}
	
	return true;
}

template <typename TYPES, typename CONFIG>
template <bool DEBUG, bool EXEC, bool WRITE>
inline bool CPU<TYPES, CONFIG>::Translate(EFFECTIVE_ADDRESS ea, EFFECTIVE_ADDRESS& size_to_protection_boundary, ADDRESS& virt_addr, PHYSICAL_ADDRESS& phys_addr, STORAGE_ATTR& storage_attr)
{
	if(unlikely(this->linux_os_import))
	{
		size_to_protection_boundary = 0;
		virt_addr = ea;
		phys_addr = ea;
		storage_attr = TYPES::SA_DEFAULT;
		return true;
	}
	
	return static_cast<typename CONFIG::CPU *>(this)->template Translate<DEBUG, EXEC, WRITE>(ea, size_to_protection_boundary, virt_addr, phys_addr, storage_attr);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::DataBusRead(PHYSICAL_ADDRESS physical_addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr, bool rwitm)
{
	return memory_import->ReadMemory(physical_addr, buffer, size);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::DataBusWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr)
{
	return memory_import->WriteMemory(physical_addr, buffer, size);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::InstructionBusRead(PHYSICAL_ADDRESS physical_addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr)
{
	return memory_import->ReadMemory(physical_addr, buffer, size);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::DebugDataBusRead(PHYSICAL_ADDRESS physical_addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr)
{
	return memory_import->ReadMemory(physical_addr, buffer, size);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::DebugDataBusWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr)
{
	return memory_import->WriteMemory(physical_addr, buffer, size);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::DebugInstructionBusRead(PHYSICAL_ADDRESS physical_addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr)
{
	return memory_import->ReadMemory(physical_addr, buffer, size);
}

template <typename TYPES, typename CONFIG>
template <typename T, bool REVERSE, unisim::util::endian::endian_type ENDIAN>
void CPU<TYPES, CONFIG>::ConvertDataLoadStoreEndian(T& value, STORAGE_ATTR storage_attr)
{
#if BYTE_ORDER == LITTLE_ENDIAN
	if(!REVERSE || (ENDIAN == unisim::util::endian::E_BIG_ENDIAN))
#elif BYTE_ORDER == BIG_ENDIAN
	if(!REVERSE || (ENDIAN == unisim::util::endian::E_LITTLE_ENDIAN))
#else
	if(REVERSE)
#endif
	{
		unisim::util::endian::BSwap(value);
	}
}

template <typename TYPES, typename CONFIG>
template <typename T, bool REVERSE, bool CONVERT_ENDIAN, unisim::util::endian::endian_type ENDIAN>
inline bool CPU<TYPES, CONFIG>::DataLoad(T& value, EFFECTIVE_ADDRESS ea)
{
	EFFECTIVE_ADDRESS munged_ea = static_cast<typename CONFIG::CPU *>(this)->template MungEffectiveAddress<T>(ea);
	
	EFFECTIVE_ADDRESS size_to_protection_boundary;
	ADDRESS virt_addr;
	PHYSICAL_ADDRESS phys_addr;
	STORAGE_ATTR storage_attr;
	if(unlikely((!this->template Translate</* debug */ false, /* exec */ false, /* write */ false>(munged_ea, size_to_protection_boundary, virt_addr, phys_addr, storage_attr)))) return false;
	
	uint32_t size_to_fsb_boundary = CONFIG::DATA_FSB_WIDTH - (munged_ea % CONFIG::DATA_FSB_WIDTH);
	
	// Ensure that memory access does not cross a FSB boundary
	if(likely(size_to_fsb_boundary >= sizeof(T)))
	{
		// Memory load does not cross a FSB boundary
		if(unlikely(!this->SuperMSS::DataLoad(virt_addr, phys_addr, &value, sizeof(T), storage_attr))) return false;
	}
	else
	{
		// Memory load crosses a FSB boundary
		if(unlikely(!this->SuperMSS::DataLoad(virt_addr, phys_addr, &value, size_to_fsb_boundary, storage_attr))) return false;
		
		// Ensure that memory access does not cross a protection boundary
		if(unlikely(!size_to_protection_boundary) || likely(size_to_protection_boundary >= sizeof(T)))
		{
			// Memory load does not cross a protection boundary
			virt_addr += size_to_fsb_boundary;
			phys_addr += size_to_fsb_boundary;
		}
		else
		{
			// Memory load does cross a protection boundary
			EFFECTIVE_ADDRESS munged_ea2 = munged_ea + size_to_fsb_boundary;
			if(unlikely((!this->template Translate</* debug */ false, /* exec */ false, /* write */ false>(munged_ea2, size_to_protection_boundary, virt_addr, phys_addr, storage_attr)))) return false;
		}
		
		if(unlikely(!this->SuperMSS::DataLoad(virt_addr, phys_addr, (uint8_t *) &value + size_to_fsb_boundary, sizeof(T) - size_to_fsb_boundary, storage_attr))) return false;
	}

	if(CONVERT_ENDIAN)
	{
		static_cast<typename CONFIG::CPU *>(this)->template ConvertDataLoadStoreEndian<T, REVERSE, ENDIAN>(value, storage_attr);
	}
	
	return true;
}

template <typename TYPES, typename CONFIG>
template <typename T, bool REVERSE, bool CONVERT_ENDIAN, unisim::util::endian::endian_type ENDIAN>
inline bool CPU<TYPES, CONFIG>::DataStore(T& value, EFFECTIVE_ADDRESS ea)
{
	EFFECTIVE_ADDRESS munged_ea = static_cast<typename CONFIG::CPU *>(this)->template MungEffectiveAddress<T>(ea);

	EFFECTIVE_ADDRESS size_to_protection_boundary;
	ADDRESS virt_addr;
	PHYSICAL_ADDRESS phys_addr;
	STORAGE_ATTR storage_attr;
	if(unlikely((!this->template Translate</* debug */ false, /* exec */ false, /* write */ true>(munged_ea, size_to_protection_boundary, virt_addr, phys_addr, storage_attr)))) return false;

	if(CONVERT_ENDIAN)
	{
		static_cast<typename CONFIG::CPU *>(this)->template ConvertDataLoadStoreEndian<T, REVERSE, ENDIAN>(value, storage_attr);
	}
	
	uint32_t size_to_fsb_boundary = CONFIG::DATA_FSB_WIDTH - (munged_ea % CONFIG::DATA_FSB_WIDTH);

	// Ensure that memory access does not cross a FSB boundary
	if(likely(size_to_fsb_boundary >= sizeof(T)))
	{
		// Memory store does not cross a FSB boundary
		if(unlikely(!this->SuperMSS::DataStore(virt_addr, phys_addr, &value, sizeof(T), storage_attr))) return false;
	}
	else
	{
		// Memory store crosses a FSB boundary
		if(unlikely(!this->SuperMSS::DataStore(virt_addr, phys_addr, &value, size_to_fsb_boundary, storage_attr))) return false;
		
		// Ensure that memory access does not cross a protection boundary
		if(unlikely(!size_to_protection_boundary) || likely(size_to_protection_boundary >= sizeof(T)))
		{
			// Memory store does not cross a protection boundary
			virt_addr += size_to_fsb_boundary;
			phys_addr += size_to_fsb_boundary;
		}
		else
		{
			// Memory store does cross a protection boundary
			EFFECTIVE_ADDRESS munged_ea2 = munged_ea + size_to_fsb_boundary;
			if(unlikely((!this->template Translate</* debug */ false, /* exec */ false, /* write */ true>(munged_ea2, size_to_protection_boundary, virt_addr, phys_addr, storage_attr)))) return false;
		}

		if(unlikely(!this->SuperMSS::DataStore(virt_addr, phys_addr, (uint8_t *) &value + size_to_fsb_boundary, sizeof(T) - size_to_fsb_boundary, storage_attr))) return false;
	}
	
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::DebugDataLoad(EFFECTIVE_ADDRESS ea, void *buffer, unsigned int size)
{
	do
	{
		EFFECTIVE_ADDRESS size_to_protection_boundary;
		ADDRESS virt_addr;
		PHYSICAL_ADDRESS phys_addr;
		STORAGE_ATTR storage_attr;
		if(unlikely((!this->template Translate</* debug */ true, /* exec */ false, /* write */ false>(ea, size_to_protection_boundary, virt_addr, phys_addr, storage_attr)))) return false;
		
		unsigned int sz = size_to_protection_boundary ? std::min(size, size_to_protection_boundary) : size;

		if(unlikely(!this->SuperMSS::DebugDataLoad(virt_addr, phys_addr, buffer, sz, storage_attr))) return false;
	
		ea += sz;
		size -= sz;
		buffer = (uint8_t *) buffer + sz;
	}
	while(unlikely(size));
	
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::DebugDataStore(EFFECTIVE_ADDRESS ea, const void *buffer, unsigned int size)
{
	do
	{
		EFFECTIVE_ADDRESS size_to_protection_boundary;
		ADDRESS virt_addr;
		PHYSICAL_ADDRESS phys_addr;
		STORAGE_ATTR storage_attr;
		if(unlikely((!this->template Translate</* debug */ true, /* exec */ false, /* write */ true>(ea, size_to_protection_boundary, virt_addr, phys_addr, storage_attr)))) return false;
		
		unsigned int sz = size_to_protection_boundary ? std::min(size, size_to_protection_boundary) : size;

		if(unlikely(!this->SuperMSS::DebugDataStore(virt_addr, phys_addr, buffer, sz, storage_attr))) return false;
	
		ea += sz;
		size -= sz;
		buffer = (const uint8_t *) buffer + sz;
	}
	while(unlikely(size));
	
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int8Load(unsigned int rd, typename TYPES::EFFECTIVE_ADDRESS ea)
{
	uint8_t value;
	if(unlikely(!MonitorLoad(ea, sizeof(value)))) return false;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint8_t, false, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
	if(unlikely(!status)) return false;
	gpr[rd] = (uint32_t) value; // 8-bit to 32-bit zero extension
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int16Load(unsigned int rd, typename TYPES::EFFECTIVE_ADDRESS ea)
{
	if(unlikely(!static_cast<typename CONFIG::CPU *>(this)->CheckInt16LoadAlignment(ea))) return false;
	uint16_t value;
	if(unlikely(!MonitorLoad(ea, sizeof(value)))) return false;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint16_t, false, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
	if(unlikely(!status)) return false;
	gpr[rd] = (uint32_t) value; // 16-bit to 32-bit zero extension
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::SInt16Load(unsigned int rd, typename TYPES::EFFECTIVE_ADDRESS ea)
{
	if(unlikely(!static_cast<typename CONFIG::CPU *>(this)->CheckSInt16LoadAlignment(ea))) return false;
	uint16_t value;
	if(unlikely(!MonitorLoad(ea, sizeof(value)))) return false;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint16_t, false, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
	if(unlikely(!status)) return false;
	gpr[rd] = (uint32_t) (int16_t) value; // 16-bit to 32-bit sign extension
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int32Load(unsigned int rd, typename TYPES::EFFECTIVE_ADDRESS ea)
{
	if(unlikely(!static_cast<typename CONFIG::CPU *>(this)->CheckInt32LoadAlignment(ea))) return false;
	uint32_t value;
	if(unlikely(!MonitorLoad(ea, sizeof(value)))) return false;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint32_t, false, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
	if(unlikely(!status)) return false;
	gpr[rd] = value;
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int16LoadByteReverse(unsigned int rd, typename TYPES::EFFECTIVE_ADDRESS ea)
{
	if(unlikely(!static_cast<typename CONFIG::CPU *>(this)->CheckInt16LoadByteReverseAlignment(ea))) return false;
	uint16_t value;
	if(unlikely(!MonitorLoad(ea, sizeof(value)))) return false;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint16_t, true, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea); // reversed
	if(unlikely(!status)) return false;
	gpr[rd] = (uint32_t) value; // 16-bit to 32-bit zero extension
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int32LoadByteReverse(unsigned int rd, typename TYPES::EFFECTIVE_ADDRESS ea)
{
	if(unlikely(!static_cast<typename CONFIG::CPU *>(this)->CheckInt32LoadByteReverseAlignment(ea))) return false;
	uint32_t value;
	if(unlikely(!MonitorLoad(ea, sizeof(value)))) return false;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint32_t, true, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea); // reversed
	if(unlikely(!status)) return false;
	gpr[rd] = value;
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::IntLoadMSBFirst(unsigned int rd, typename TYPES::EFFECTIVE_ADDRESS ea, uint32_t size)
{
	if(unlikely(!MonitorLoad(ea, size))) return false;

	switch(size)
	{
		case 1:
		{
			uint8_t value;
			bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint8_t, false, true, unisim::util::endian::E_BIG_ENDIAN>(value, ea);
			if(unlikely(!status)) return false;
			gpr[rd] = (uint32_t) value << 24;
			break;
		}

		case 2:
		{
			uint16_t value;
			bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint16_t, false, true, unisim::util::endian::E_BIG_ENDIAN>(value, ea);
			if(unlikely(!status)) return false;
			gpr[rd] = (uint32_t) unisim::util::endian::BigEndian2Host(value) << 16;
			break;
		}

		case 3:
		{
			typedef uint8_t array_uint8_3_t[3];
			uint8_t buffer[3];
			bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<array_uint8_3_t, false, true, unisim::util::endian::E_BIG_ENDIAN>(buffer, ea);
			if(unlikely(!status)) return false;
			uint32_t value = ((uint32_t) buffer[0] << 24) | ((uint32_t) buffer[1] << 16) | ((uint32_t) buffer[2] << 8);
			gpr[rd] = value;
			break;
		}

		case 4:
		{
			uint32_t value;
			bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint32_t, false, true, unisim::util::endian::E_BIG_ENDIAN>(value, ea);
			if(unlikely(!status)) return false;
			gpr[rd] = unisim::util::endian::BigEndian2Host(value);
			break;
		}
		
		default:
			return false;
	}
	return true;
}

template <typename TYPES, typename CONFIG>
template <typename REGISTER>
bool CPU<TYPES, CONFIG>::SpecialLoad(REGISTER& reg, typename TYPES::EFFECTIVE_ADDRESS ea)
{
	if(unlikely((!static_cast<typename CONFIG::CPU *>(this)->template CheckSpecialLoadAlignment<REGISTER>(ea)))) return false;
	uint32_t value;
	if(unlikely(!MonitorLoad(ea, sizeof(value)))) return false;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint32_t, false, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
	if(unlikely(!status)) return false;
	reg = value;
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int8Store(unsigned int rs, typename TYPES::EFFECTIVE_ADDRESS ea)
{
	uint8_t value = gpr[rs];
	if(unlikely(!MonitorStore(ea, sizeof(value)))) return false;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint8_t, false, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
	if(unlikely(!status)) return false;
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int16Store(unsigned int rs, typename TYPES::EFFECTIVE_ADDRESS ea)
{
	if(unlikely(!static_cast<typename CONFIG::CPU *>(this)->CheckInt16StoreAlignment(ea))) return false;
	uint16_t value = (uint16_t) gpr[rs];
	if(unlikely(!MonitorStore(ea, sizeof(value)))) return false;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint16_t, false, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
	if(unlikely(!status)) return false;
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int32Store(unsigned int rs, typename TYPES::EFFECTIVE_ADDRESS ea)
{
	if(unlikely(!static_cast<typename CONFIG::CPU *>(this)->CheckInt32StoreAlignment(ea))) return false;
	uint32_t value = gpr[rs];
	if(unlikely(!MonitorStore(ea, sizeof(value)))) return false;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint32_t, false, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
	if(unlikely(!status)) return false;
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int16StoreByteReverse(unsigned int rs, typename TYPES::EFFECTIVE_ADDRESS ea)
{
	if(unlikely(!static_cast<typename CONFIG::CPU *>(this)->CheckInt16StoreByteReverseAlignment(ea))) return false;
	uint16_t value = (uint16_t) gpr[rs];
	if(unlikely(!MonitorStore(ea, sizeof(value)))) return false;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint16_t, true, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea); // reversed
	if(unlikely(!status)) return false;
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Int32StoreByteReverse(unsigned int rs, typename TYPES::EFFECTIVE_ADDRESS ea)
{
	if(unlikely(!static_cast<typename CONFIG::CPU *>(this)->CheckInt32StoreByteReverseAlignment(ea))) return false;
	uint32_t value = gpr[rs];
	if(unlikely(!MonitorStore(ea, sizeof(value)))) return false;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint32_t, true, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea); // reversed
	if(unlikely(!status)) return false;
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::IntStoreMSBFirst(unsigned int rs, typename TYPES::EFFECTIVE_ADDRESS ea, uint32_t size)
{
	if(unlikely(!MonitorStore(ea, size))) return false;
	
	switch(size)
	{
		case 1:
			{
				uint8_t value = gpr[rs] >> 24;
				bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint8_t, false, true, unisim::util::endian::E_BIG_ENDIAN>(value, ea);
				if(unlikely(!status)) return false;
				break;
			}

		case 2:
			{
				uint16_t value = unisim::util::endian::Host2BigEndian((uint16_t)(gpr[rs] >> 16));
				bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint16_t, false, true, unisim::util::endian::E_BIG_ENDIAN>(value, ea);
				if(unlikely(!status)) return false;
				break;
			}

		case 3:
			{
				typedef uint8_t array_uint8_3_t[3];
				uint32_t value = gpr[rs];
				array_uint8_3_t buffer;
				buffer[0] = value >> 24;
				buffer[1] = value >> 16;
				buffer[2] = value >> 8;
				bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<array_uint8_3_t, false, true, unisim::util::endian::E_BIG_ENDIAN>(buffer, ea);
				if(unlikely(!status)) return false;
				break;
			}

		case 4:
			{
				uint32_t value = unisim::util::endian::Host2BigEndian(gpr[rs]);
				bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint32_t, false, true, unisim::util::endian::E_BIG_ENDIAN>(value, ea);
				if(unlikely(!status)) return false;
				break;
			}
			
		default:
			return false;
	}

	return true;
}

template <typename TYPES, typename CONFIG>
template <typename REGISTER>
bool CPU<TYPES, CONFIG>::SpecialStore(const REGISTER& reg, typename TYPES::EFFECTIVE_ADDRESS ea)
{
	if(unlikely((!static_cast<typename CONFIG::CPU *>(this)->template CheckSpecialStoreAlignment<REGISTER>(ea)))) return false;
	uint32_t value = reg;
	if(unlikely(!MonitorStore(ea, sizeof(value)))) return false;
	bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint32_t, false, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
	if(unlikely(!status)) return false;
	return true;
}

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::LegacyFPU::LegacyFPU(typename CONFIG::CPU *cpu)
	: fpr()
	, fpscr()
{
	unsigned int i;
	
	for(i = 0; i < 32; i++)
	{
		FPR& ith_fpr = fpr[i];
		ith_fpr.Init(i);
		cpu->AddRegisterInterface(ith_fpr.CreateRegisterInterface());
	}
	
	cpu->AddRegisterInterface(fpscr.CreateRegisterInterface());
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::LegacyFPU::Fp32Load(typename CONFIG::CPU *cpu, unsigned int fd, EFFECTIVE_ADDRESS ea)
{
	// check alignment
	if(unlikely(!cpu->CheckFp32LoadAlignment(ea))) return false;
	uint32_t value;
	if(unlikely(!cpu->MonitorLoad(ea, sizeof(value)))) return false;
	bool status = cpu->template DataLoad<uint32_t, false, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
	if(unlikely(!status)) return false;
	Flags flags( Flags::RoundingMode(fpscr.Get<FPSCR::RN>()) );
	fpr[fd].convertAssign(SoftFloat(SoftFloat::FromRawBits, value), flags);
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::LegacyFPU::Fp64Load(typename CONFIG::CPU *cpu, unsigned int fd, EFFECTIVE_ADDRESS ea)
{
	// check alignment
	if(unlikely(!cpu->CheckFp64LoadAlignment(ea))) return false;
	uint64_t value;
	if(unlikely(!cpu->MonitorLoad(ea, sizeof(value)))) return false;
	bool status = cpu->template DataLoad<uint64_t, false, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
	if(unlikely(!status)) return false;
	fpr[fd].fromRawBitsAssign(value);
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::LegacyFPU::Fp32Store(typename CONFIG::CPU *cpu, unsigned int fs, EFFECTIVE_ADDRESS ea)
{
	// check alignment
	if(unlikely(!cpu->CheckFp32StoreAlignment(ea))) return false;
	Flags flags(Flags::RoundingMode(1)); // Zero Rounding
	uint32_t value = SoftFloat(fpr[fs], flags).queryRawBits();
	if(unlikely(!cpu->MonitorStore(ea, sizeof(value)))) return false;
	return cpu->template DataStore<uint32_t, false, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::LegacyFPU::Fp64Store(typename CONFIG::CPU *cpu, unsigned int fs, EFFECTIVE_ADDRESS ea)
{
	// check alignment
	if(unlikely(!cpu->CheckFp64StoreAlignment(ea))) return false;
	uint64_t value = fpr[fs].queryRawBits();
	if(unlikely(!cpu->MonitorStore(ea, sizeof(value)))) return false;
	return cpu->template DataStore<uint64_t, false, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::LegacyFPU::FpStoreLSW(typename CONFIG::CPU *cpu, unsigned int fs, EFFECTIVE_ADDRESS ea)
{
	// check alignment
	if(unlikely(!cpu->CheckFpStoreLSWAlignment(ea))) return false;
	uint32_t value = uint32_t(fpr[fs].queryRawBits());
	if(unlikely(!cpu->MonitorStore(ea, sizeof(value)))) return false;
	return cpu->template DataStore<uint32_t, false, true, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::LegacyFPU::CheckFloatingPointException(typename CONFIG::CPU *cpu)
{
	// Check for floating point exception condition: FPSCR[FEX] and (MSR[FE0] or MSR[FE1])
	if((fpscr.template Get<typename FPSCR::FEX>()) and (cpu->GetMSR().template Get<typename MSR::FE0>() or cpu->GetMSR().template Get<typename MSR::FE1>()))
	{
		// Raise a floating point exception if FPSCR[FEX] is set
		cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::FloatingPoint>();
		return false;
	}
	return true;
}

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::AltivecVectorUnit::AltivecVectorUnit(typename CONFIG::CPU *cpu)
	: vrsave(cpu)
	, vscr()
	, vr()
{
	unsigned int i;
	
	for(i = 0; i < 32; i++)
	{
		VR& ith_vr = vr[i];
		ith_vr.Init(i);
		cpu->AddRegisterInterface(ith_vr.CreateRegisterInterface());
	}
	
	cpu->AddRegisterInterface(vscr.CreateRegisterInterface());
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ResetMemory()
{
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::ReadMemory(EFFECTIVE_ADDRESS addr, void *buffer, uint32_t size)
{
	return this->DebugDataLoad(addr, buffer, size);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::WriteMemory(EFFECTIVE_ADDRESS addr, const void *buffer, uint32_t size)
{
	return this->DebugDataStore(addr, buffer, size);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::InjectReadMemory(EFFECTIVE_ADDRESS ea, void *buffer, uint32_t size)
{
	if(likely(size))
	{
		do
		{
			if(likely(!(ea & 3) && (size >= 4)))
			{
				uint32_t value;
				bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint32_t, false, false, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
				if(unlikely(!status)) return false;
				*(uint32_t *) buffer = value;
				size -= 4;
				ea += 4;
				buffer = (uint32_t *) buffer + 1;
			}
			else if(!(ea & 1) && (size >= 2))
			{
				uint16_t value;
				bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint16_t, false, false, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
				if(unlikely(!status)) return false;
				*(uint16_t *) buffer = value;
				size -= 2;
				ea += 2;
				buffer = (uint16_t *) buffer + 1;
			}
			else
			{
				uint8_t value;
				bool status = static_cast<typename CONFIG::CPU *>(this)->template DataLoad<uint8_t, false, false, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
				if(unlikely(!status)) return false;
				*(uint8_t *) buffer = value;
				--size;
				++ea;
				buffer = (uint8_t *) buffer + 1;
			}
		}
		while(likely(size));
	}
	
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::InjectWriteMemory(EFFECTIVE_ADDRESS ea, const void *buffer, uint32_t size)
{
	if(likely(size))
	{
		do
		{
			if(likely(!(ea & 3) && (size >= 4)))
			{
				uint32_t value = *(uint32_t *) buffer;
				bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint32_t, false, false, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
				if(unlikely(!status)) return false;
				size -= 4;
				ea += 4;
				buffer = (uint32_t *) buffer + 1;
			}
			else if(!(ea & 1) && (size >= 2))
			{
				uint16_t value = *(uint16_t *) buffer;
				bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint16_t, false, false, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
				if(unlikely(!status)) return false;
				size -= 2;
				ea += 2;
				buffer = (uint16_t *) buffer + 1;
			}
			else
			{
				uint8_t value = *(uint8_t *) buffer;
				bool status = static_cast<typename CONFIG::CPU *>(this)->template DataStore<uint8_t, false, false, unisim::util::endian::E_UNKNOWN_ENDIAN>(value, ea);
				if(unlikely(!status)) return false;
				--size;
				++ea;
				buffer = (uint8_t *) buffer + 1;
			}
		}
		while(likely(size));
	}
	
	return true;
}

template <typename TYPES, typename CONFIG>
unisim::service::interfaces::Register *CPU<TYPES, CONFIG>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::Synchronize()
{
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::RequiresMemoryAccessReporting(unisim::service::interfaces::MemoryAccessReportingType type, bool report)
{
	switch(type)
	{
		case unisim::service::interfaces::REPORT_MEM_ACCESS:
			requires_memory_access_reporting = report;
			break;
		case unisim::service::interfaces::REPORT_FETCH_INSN:
			requires_fetch_instruction_reporting = report;
			break;
		case unisim::service::interfaces::REPORT_COMMIT_INSN:
			requires_commit_instruction_reporting = report;
			break;
	}
}

template <typename TYPES, typename CONFIG>
std::string CPU<TYPES, CONFIG>::GetObjectFriendlyName(typename TYPES::EFFECTIVE_ADDRESS addr)
{
	std::stringstream sstr;
	
	const unisim::util::debug::Symbol<typename TYPES::EFFECTIVE_ADDRESS> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(addr, unisim::util::debug::Symbol<typename TYPES::EFFECTIVE_ADDRESS>::SYM_OBJECT) : 0;
	if(symbol)
		sstr << symbol->GetFriendlyName(addr);
	else
		sstr << "0x" << std::hex << addr << std::dec;

	return sstr.str();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::LogLinuxSystemCall()
{
	if(unlikely(enable_linux_syscall_snooping))
	{
		powerpc_linux32.LogSystemCall(gpr[0]);
	}
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::Trap()
{
	if(trap_reporting_import)
	{
		trap_reporting_import->ReportTrap();
	}
}

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CPU_TCC__


