/*
 *  Copyright (c) 2007-2019,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_CPU_TCC__

#include <unisim/component/cxx/processor/powerpc/book_e/cpu.hh>
#include <unisim/component/cxx/processor/powerpc/cpu.tcc>
#include <unisim/util/simfloat/floating.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace book_e {

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::CPU(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, SuperCPU(name, parent)
	, unisim::kernel::Service<unisim::service::interfaces::Disassembly<EFFECTIVE_ADDRESS> >(name, parent)
	, disasm_export("disasm-export", this)
	, cpuid(0x0)
	, param_cpuid("cpuid", this, cpuid, "CPU ID at reset")
	, processor_version(0x0)
	, param_processor_version("processor-version", this, processor_version, "Processor Version")
	, instruction_buffer_base_addr(~EFFECTIVE_ADDRESS(0))
	, instruction_buffer()
	, decoder()
	, operation(0)
	, critical_input_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, machine_check_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, data_storage_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, instruction_storage_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, external_input_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, alignment_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, program_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, floating_point_unavailable_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, system_call_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, auxiliary_processor_unavailable_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, decrementer_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, fixed_interval_timer_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, watchdog_timer_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, data_tlb_error_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, instruction_tlb_error_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, debug_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, dcdbtrh(static_cast<typename CONFIG::CPU *>(this))
	, dcdbtrl(static_cast<typename CONFIG::CPU *>(this))
	, icdbdr(static_cast<typename CONFIG::CPU *>(this))
	, icdbtrh(static_cast<typename CONFIG::CPU *>(this))
	, icdbtrl(static_cast<typename CONFIG::CPU *>(this))
	, dac1(static_cast<typename CONFIG::CPU *>(this))
	, dac2(static_cast<typename CONFIG::CPU *>(this))
	, dbcr0(static_cast<typename CONFIG::CPU *>(this))
	, dbcr1(static_cast<typename CONFIG::CPU *>(this))
	, dbcr2(static_cast<typename CONFIG::CPU *>(this))
	, dbdr(static_cast<typename CONFIG::CPU *>(this))
	, dbsr(static_cast<typename CONFIG::CPU *>(this))
	, dvc1(static_cast<typename CONFIG::CPU *>(this))
	, dvc2(static_cast<typename CONFIG::CPU *>(this))
	, iac1(static_cast<typename CONFIG::CPU *>(this))
	, iac2(static_cast<typename CONFIG::CPU *>(this))
	, iac3(static_cast<typename CONFIG::CPU *>(this))
	, iac4(static_cast<typename CONFIG::CPU *>(this))
	, csrr0(static_cast<typename CONFIG::CPU *>(this))
	, csrr1(static_cast<typename CONFIG::CPU *>(this))
	, dear(static_cast<typename CONFIG::CPU *>(this))
	, esr(static_cast<typename CONFIG::CPU *>(this))
	, ivor0(static_cast<typename CONFIG::CPU *>(this))
	, ivor1(static_cast<typename CONFIG::CPU *>(this))
	, ivor2(static_cast<typename CONFIG::CPU *>(this))
	, ivor3(static_cast<typename CONFIG::CPU *>(this))
	, ivor4(static_cast<typename CONFIG::CPU *>(this))
	, ivor5(static_cast<typename CONFIG::CPU *>(this))
	, ivor6(static_cast<typename CONFIG::CPU *>(this))
	, ivor7(static_cast<typename CONFIG::CPU *>(this))
	, ivor8(static_cast<typename CONFIG::CPU *>(this))
	, ivor9(static_cast<typename CONFIG::CPU *>(this))
	, ivor10(static_cast<typename CONFIG::CPU *>(this))
	, ivor11(static_cast<typename CONFIG::CPU *>(this))
	, ivor12(static_cast<typename CONFIG::CPU *>(this))
	, ivor13(static_cast<typename CONFIG::CPU *>(this))
	, ivor14(static_cast<typename CONFIG::CPU *>(this))
	, ivor15(static_cast<typename CONFIG::CPU *>(this))
	, ivpr(static_cast<typename CONFIG::CPU *>(this))
	, mcsr(static_cast<typename CONFIG::CPU *>(this))
	, mcsrr0(static_cast<typename CONFIG::CPU *>(this))
	, mcsrr1(static_cast<typename CONFIG::CPU *>(this))
	, srr0(static_cast<typename CONFIG::CPU *>(this))
	, srr1(static_cast<typename CONFIG::CPU *>(this))
	, ccr0(static_cast<typename CONFIG::CPU *>(this))
	, ccr1(static_cast<typename CONFIG::CPU *>(this))
	, msr(static_cast<typename CONFIG::CPU *>(this))
	, pir(static_cast<typename CONFIG::CPU *>(this))
	, pvr(static_cast<typename CONFIG::CPU *>(this))
	, rstcfg(static_cast<typename CONFIG::CPU *>(this))
	, sprg0(static_cast<typename CONFIG::CPU *>(this))
	, sprg1(static_cast<typename CONFIG::CPU *>(this))
	, sprg2(static_cast<typename CONFIG::CPU *>(this))
	, sprg3(static_cast<typename CONFIG::CPU *>(this))
	, sprg4(static_cast<typename CONFIG::CPU *>(this))
	, sprg5(static_cast<typename CONFIG::CPU *>(this))
	, sprg6(static_cast<typename CONFIG::CPU *>(this))
	, sprg7(static_cast<typename CONFIG::CPU *>(this))
	, usprg0(static_cast<typename CONFIG::CPU *>(this))
	, mmucr(static_cast<typename CONFIG::CPU *>(this))
	, pid(static_cast<typename CONFIG::CPU *>(this))
	, dec(static_cast<typename CONFIG::CPU *>(this))
	, decar(static_cast<typename CONFIG::CPU *>(this))
	, tb(static_cast<typename CONFIG::CPU *>(this))
	, tcr(static_cast<typename CONFIG::CPU *>(this))
	, tsr(static_cast<typename CONFIG::CPU *>(this))
{
	disasm_export.SetupDependsOn(this->memory_import);
	
	param_processor_version.SetMutable(false);
	
	pvr.Initialize(processor_version);

	this->SuperCPU::template EnableInterrupt<DataStorageInterrupt>();
	this->SuperCPU::template EnableInterrupt<InstructionStorageInterrupt>();
	this->SuperCPU::template EnableInterrupt<AlignmentInterrupt>();
	this->SuperCPU::template EnableException<typename ProgramInterrupt::IllegalInstruction>();
	this->SuperCPU::template EnableException<typename ProgramInterrupt::PrivilegeViolation>();
	this->SuperCPU::template EnableException<typename ProgramInterrupt::Trap>();
	this->SuperCPU::template EnableException<typename ProgramInterrupt::AuxiliaryProcessor>();
	this->SuperCPU::template EnableException<typename ProgramInterrupt::UnimplementedInstruction>();
	this->SuperCPU::template EnableInterrupt<FloatingPointUnavailableInterrupt>();
	this->SuperCPU::template EnableInterrupt<AuxiliaryProcessorUnavailableInterrupt>();
	this->SuperCPU::template EnableInterrupt<SystemCallInterrupt>();
	this->SuperCPU::template EnableInterrupt<DataTLBErrorInterrupt>();
	this->SuperCPU::template EnableInterrupt<InstructionTLBErrorInterrupt>();
	
	this->Reset();
}

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::~CPU()
{
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::EndSetup()
{
	if(!SuperCPU::EndSetup()) return false;
	
	if(this->linux_os_import)
	{
		msr.template Set<typename MSR::FP>(1); // enable FPU
		msr.template Set<typename MSR::PR>(1); // set user privilege level
	}
	
	return true;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::Reset()
{
	SuperCPU::Reset();
	this->cia = this->reset_addr & -4;
	msr.Initialize(0x0);
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(CriticalInputInterrupt *critical_input_interrupt)
{
	csrr0 = this->cia;
	csrr1 = msr;
	
	msr.template Set<typename CriticalInputInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor0;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(MachineCheckInterrupt *machine_check_interrupt)
{
	mcsrr0 = this->cia;
	mcsrr1 = msr;
	
	msr.template Set<typename MachineCheckInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	mcsr = 0;
	if(machine_check_interrupt->GotAsynchronousEvents())
	{
		mcsr.template Set<typename MCSR::MCS>(1);
		if(machine_check_interrupt->GotAsynchronousEvent(MachineCheckInterrupt::MCE_INSTRUCTION_PLB_ERROR)) mcsr.template Set<typename MCSR::IB>(1);
		if(machine_check_interrupt->GotAsynchronousEvent(MachineCheckInterrupt::MCE_DATA_READ_PLB_ERROR)) mcsr.template Set<typename MCSR::DRB>(1);
		if(machine_check_interrupt->GotAsynchronousEvent(MachineCheckInterrupt::MCE_DATA_WRITE_PLB_ERROR)) mcsr.template Set<typename MCSR::DWB>(1);
		if(machine_check_interrupt->GotAsynchronousEvent(MachineCheckInterrupt::MCE_TLB_PARITY_ERROR)) mcsr.template Set<typename MCSR::TLBP>(1);
		if(machine_check_interrupt->GotAsynchronousEvent(MachineCheckInterrupt::MCE_INSTRUCTION_CACHE_PARITY_ERROR)) mcsr.template Set<typename MCSR::ICP>(1);
		if(machine_check_interrupt->GotAsynchronousEvent(MachineCheckInterrupt::MCE_DATA_CACHE_SEARCH_PARITY_ERROR)) mcsr.template Set<typename MCSR::DCSP>(1);
		if(machine_check_interrupt->GotAsynchronousEvent(MachineCheckInterrupt::MCE_DATA_CACHE_FLUSH_PARITY_ERROR)) mcsr.template Set<typename MCSR::IMPE>(1);
	}

	this->cia = ivpr | ivor1;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(DataStorageInterrupt *data_storage_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	dear = data_storage_interrupt->GetAddress();

	esr = 0;
	operation->update_esr_fp(static_cast<typename CONFIG::CPU *>(this));
	operation->update_esr_st(static_cast<typename CONFIG::CPU *>(this));
	operation->update_esr_ap(static_cast<typename CONFIG::CPU *>(this));
	if(this->template RecognizedException<typename DataStorageInterrupt::ByteOrdering>())
	{
		esr.template Set<typename ESR::BO>(1);
	}
	else if(this->template RecognizedException<typename DataStorageInterrupt::CacheLocking>())
	{
		operation->update_esr_dlk(static_cast<typename CONFIG::CPU *>(this));
	}

	msr.template Set<typename DataStorageInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor2;
	
	this->template AckInterrupt<DataStorageInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(InstructionStorageInterrupt *instruction_storage_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	esr = 0;
	if(this->template RecognizedException<typename InstructionStorageInterrupt::ByteOrdering>())
	{
		esr.template Set<typename ESR::BO>(1);
	}
	
	msr.template Set<typename InstructionStorageInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor3;
	
	this->template AckInterrupt<InstructionStorageInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(ExternalInputInterrupt *external_input_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename ExternalInputInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor4;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(AlignmentInterrupt *alignment_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	dear = alignment_interrupt->GetAddress();
	
	esr = 0;
	operation->update_esr_fp(static_cast<typename CONFIG::CPU *>(this));
	operation->update_esr_st(static_cast<typename CONFIG::CPU *>(this));
	operation->update_esr_ap(static_cast<typename CONFIG::CPU *>(this));
	
	msr.template Set<typename AlignmentInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor5;
	
	this->template AckInterrupt<AlignmentInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(ProgramInterrupt *program_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	esr = 0;
	if(this->template RecognizedException<typename ProgramInterrupt::IllegalInstruction>())
	{
		esr.template Set<typename ESR::PIL>(1);
	}
	if(this->template RecognizedException<typename ProgramInterrupt::PrivilegeViolation>())
	{
		esr.template Set<typename ESR::PPR>(1);
	}
	if(this->template RecognizedException<typename ProgramInterrupt::Trap>())
	{
		this->instruction_counter++;  // account for the trap instruction
		esr.template Set<typename ESR::PTR>(1);
	}
	if(this->template RecognizedException<typename ProgramInterrupt::FloatingPoint>())
	{
		esr.template Set<typename ESR::FP>(1);
	}
	if(this->template RecognizedException<typename ProgramInterrupt::AuxiliaryProcessor>())
	{
		esr.template Set<typename ESR::AP>(1);
	}
	if(this->template RecognizedException<typename ProgramInterrupt::UnimplementedInstruction>())
	{
		esr.template Set<typename ESR::PUO>(1);
	}
	
	msr.template Set<typename ProgramInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor6;
	
	this->template AckInterrupt<ProgramInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(FloatingPointUnavailableInterrupt *floating_point_unavailable_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename FloatingPointUnavailableInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor7;
	
	this->template AckInterrupt<FloatingPointUnavailableInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(SystemCallInterrupt *system_call_interrupt)
{
	if(unlikely(this->enable_linux_syscall_snooping))
	{
		this->LogLinuxSystemCall();
	}
	
	if(this->linux_os_import)
	{
		// Do Linux ABI translation
		this->linux_os_import->ExecuteSystemCall(this->gpr[0]);
	}
	else
	{
		srr0 = this->nia;
		srr1 = msr;
		
		msr.template Set<typename SystemCallInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
		
		this->cia = ivpr | ivor8;
	}
	
	this->template AckInterrupt<SystemCallInterrupt>();
	
	system_call_interrupt->ClearELEV();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(AuxiliaryProcessorUnavailableInterrupt *auxiliary_processor_unavailable_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename AuxiliaryProcessorUnavailableInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor9;
	
	this->template AckInterrupt<AuxiliaryProcessorUnavailableInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(DecrementerInterrupt *decrementer_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename DecrementerInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor10;
	
	this->template AckInterrupt<DecrementerInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(FixedIntervalTimerInterrupt *fixed_interval_timer_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename FixedIntervalTimerInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor11;
	
	this->template AckInterrupt<FixedIntervalTimerInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(WatchdogTimerInterrupt *watchdog_timer_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename WatchdogTimerInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor12;
	
	this->template AckInterrupt<WatchdogTimerInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(DataTLBErrorInterrupt *data_tlb_error_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename DataTLBErrorInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	dear = data_tlb_error_interrupt->GetAddress();
	
	esr = 0;
	operation->update_esr_fp(static_cast<typename CONFIG::CPU *>(this));
	operation->update_esr_st(static_cast<typename CONFIG::CPU *>(this));
	operation->update_esr_ap(static_cast<typename CONFIG::CPU *>(this));
	
	this->cia = ivpr | ivor13;
	
	this->template AckInterrupt<DataTLBErrorInterrupt>();
	
	data_tlb_error_interrupt->ClearAddress();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(InstructionTLBErrorInterrupt *instruction_tlb_error_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename InstructionTLBErrorInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor14;
	
	this->template AckInterrupt<InstructionTLBErrorInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(DebugInterrupt *debug_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename DebugInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor15;
	
	this->template AckInterrupt<DebugInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::UpdateExceptionEnable()
{
	if(msr.template Get<typename MSR::CE>())
	{
		this->template EnableInterrupt<CriticalInputInterrupt>();
	}
	else
	{
		this->template DisableInterrupt<CriticalInputInterrupt>();
	}
	
	if(msr.template Get<typename MSR::EE>())
	{
		this->template EnableInterrupt<ExternalInputInterrupt>();
	}
	else
	{
		this->template DisableInterrupt<ExternalInputInterrupt>();
	}
	
	if(msr.template Get<typename MSR::ME>())
	{
		this->template EnableInterrupt<MachineCheckInterrupt>();
	}
	else
	{
		this->template DisableInterrupt<MachineCheckInterrupt>();
	}
	
	if(msr.template Get<typename MSR::EE>() && tcr.template Get<typename TCR::DIE>())
	{
		this->template EnableInterrupt<DecrementerInterrupt>();
	}
	else
	{
		this->template DisableInterrupt<DecrementerInterrupt>();
	}

	if(msr.template Get<typename MSR::EE>() && tcr.template Get<typename TCR::FIE>())
	{
		this->template EnableInterrupt<FixedIntervalTimerInterrupt>();
	}
	else
	{
		this->template DisableInterrupt<FixedIntervalTimerInterrupt>();
	}

	if(msr.template Get<typename MSR::CE>() && tcr.template Get<typename TCR::WIE>())
	{
		this->template EnableInterrupt<WatchdogTimerInterrupt>();
	}
	else
	{
		this->template DisableInterrupt<WatchdogTimerInterrupt>();
	}

	if(msr.template Get<typename MSR::DE>())
	{
		this->template EnableInterrupt<DebugInterrupt>();
	}
	else
	{
		this->template DisableInterrupt<DebugInterrupt>();
	}
	
	this->Synchronize(); // Force resampling input signals
}

template <typename TYPES, typename CONFIG>
template <typename T, bool REVERSE, unisim::util::endian::endian_type ENDIAN>
void CPU<TYPES, CONFIG>::ConvertDataLoadStoreEndian(T& value, STORAGE_ATTR storage_attr)
{
#if BYTE_ORDER == LITTLE_ENDIAN
	if((!REVERSE && ((ENDIAN == unisim::util::endian::E_BIG_ENDIAN)    || ((ENDIAN == unisim::util::endian::E_UNKNOWN_ENDIAN) &&   likely(!(storage_attr & TYPES::SA_E))))) ||
	   ( REVERSE && ((ENDIAN == unisim::util::endian::E_LITTLE_ENDIAN) || ((ENDIAN == unisim::util::endian::E_UNKNOWN_ENDIAN) && unlikely(  storage_attr & TYPES::SA_E)))))
#elif BYTE_ORDER == BIG_ENDIAN
	if(( REVERSE && ((ENDIAN == unisim::util::endian::E_LITTLE_ENDIAN) || ((ENDIAN == unisim::util::endian::E_UNKNOWN_ENDIAN) && unlikely(  storage_attr & TYPES::SA_E)))) ||
	   (!REVERSE && ((ENDIAN == unisim::util::endian::E_BIG_ENDIAN)    || ((ENDIAN == unisim::util::endian::E_UNKNOWN_ENDIAN) &&   likely(!(storage_attr & TYPES::SA_E))))))
#else
	if(REVERSE)
#endif
	{
		unisim::util::endian::BSwap(value);
	}
}

template <typename TYPES, typename CONFIG>
template <unsigned int SIZE>
bool CPU<TYPES, CONFIG>::CheckIntLoadStoreAlignment(EFFECTIVE_ADDRESS ea)
{
	if(unlikely(ea % SIZE) && unlikely((ccr0.template Get<typename SuperCPU::CCR0::FLSTA>())))
	{
		this->template ThrowException<typename AlignmentInterrupt::UnalignedIntegerLoadStore>().SetAddress(ea);
		return false;
	}
	return true;
}

template <typename TYPES, typename CONFIG>
template <unsigned int SIZE>
bool CPU<TYPES, CONFIG>::CheckFloatingPointLoadStoreAlignment(EFFECTIVE_ADDRESS ea)
{
	if(unlikely(ea % SIZE) && unlikely((ccr0.template Get<typename SuperCPU::CCR0::FLSTA>())))
	{
		this->template ThrowException<typename AlignmentInterrupt::UnalignedFloatingPointLoadStore>().SetAddress(ea);
		return false;
	}
	return true;
}

template <typename TYPES, typename CONFIG>
std::string CPU<TYPES, CONFIG>::Disasm(EFFECTIVE_ADDRESS addr, EFFECTIVE_ADDRESS& next_addr)
{
	std::stringstream sstr;
	typename CONFIG::OPERATION *operation;
	typename CONFIG::CODE_TYPE insn;

	EFFECTIVE_ADDRESS size_to_protection_boundary;
	ADDRESS virt_addr;
	PHYSICAL_ADDRESS phys_addr;
	STORAGE_ATTR storage_attr;

	if(unlikely((!this->template Translate</* debug */ true, /* exec */ true, /* write */ false>(addr, size_to_protection_boundary, virt_addr, phys_addr, storage_attr))) ||
	   unlikely(!this->SuperMSS::DebugInstructionFetch(virt_addr, phys_addr, &insn, 4, storage_attr)))
	{
		next_addr = addr + 4;
		return std::string("unreadable ?");
	}
	
#if BYTE_ORDER == BIG_ENDIAN
	if(unlikely(storage_attr & TYPES::SA_E)) // little-endian ?
#elif BYTE_ORDER == LITTLE_ENDIAN
	if(likely(!(storage_attr & TYPES::SA_E))) // big-endian ?
#else
	if(0)
#endif
	{
		unisim::util::endian::BSwap(insn);
	}

	operation = decoder.Decode(addr, insn);
	
	// disassemble the instruction
	
	operation->disasm(static_cast<typename CONFIG::CPU *>(this), sstr);
	next_addr = addr + (operation->GetLength() / 8);
	
	return sstr.str();
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Lwarx(unsigned int rd, EFFECTIVE_ADDRESS addr)
{
	// TODO: reservation logic
	return this->Int32Load(rd, addr);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Mbar(EFFECTIVE_ADDRESS addr)
{
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Stwcx(unsigned int rs, EFFECTIVE_ADDRESS addr)
{
	// TODO: reservation logic
	// Note: Address match with prior lbarx, lharx, or lwarx not required for store to be performed
	if(!this->Int32Store(rs, addr)) return false;
	// Clear CR0[LT][GT], setCR0[EQ] and copy XER[SO] to CR0[SO]
	this->cr.template Set<typename CR::CR0::LT>(0);
	this->cr.template Set<typename CR::CR0::GT>(0);
	this->cr.template Set<typename CR::CR0::EQ>(1);
	this->cr.template Set<typename CR::CR0::SO>(this->xer.template Get<typename XER::SO>());
	
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Wait()
{
	Idle();
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Msync()
{
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Isync()
{
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Rfi()
{
	if(msr.template Get<typename MSR::PR>())
	{
		this->template ThrowException<typename ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	struct B0_30 : Field<void, 0, 30> {};

	this->Branch(srr0 & B0_30::template GetMask<EFFECTIVE_ADDRESS>());
	msr = srr1;
	
	if(unlikely(this->verbose_interrupt))
	{
		this->logger << DebugInfo << "Returning from interrupt to 0x" << std::hex << this->nia << std::dec << EndDebugInfo;
	}

	this->FlushInstructionBuffer();
	this->Synchronize();
	
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Rfci()
{
	if(msr.template Get<typename MSR::PR>())
	{
		this->template ThrowException<typename ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	struct B0_30 : Field<void, 0, 30> {};

	this->Branch(csrr0 & B0_30::template GetMask<EFFECTIVE_ADDRESS>());
	msr = csrr1;
	
	if(unlikely(this->verbose_interrupt))
	{
		this->logger << DebugInfo << "Returning from critical interrupt to 0x" << std::hex << this->nia << std::dec << EndDebugInfo;
	}

	this->FlushInstructionBuffer();
	this->Synchronize();

	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Rfmci()
{
	if(msr.template Get<typename MSR::PR>())
	{
		this->template ThrowException<typename ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	struct B0_30 : Field<void, 0, 30> {};
	
	this->Branch(mcsrr0 & B0_30::template GetMask<EFFECTIVE_ADDRESS>());
	msr = mcsrr1;
	
	if(unlikely(this->verbose_interrupt))
	{
		this->logger << DebugInfo << "Returning from machine check interrupt to 0x" << std::hex << this->nia << std::dec << EndDebugInfo;
	}

	this->FlushInstructionBuffer();
	this->Synchronize();

	return true;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::FlushInstructionBuffer()
{
	instruction_buffer_base_addr = ~EFFECTIVE_ADDRESS(0);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::InstructionFetch(EFFECTIVE_ADDRESS addr, typename CONFIG::CODE_TYPE& insn)
{
	EFFECTIVE_ADDRESS base_addr = addr & ~(CONFIG::INSTRUCTION_BUFFER_SIZE - 1);
	int instruction_buffer_index = (addr / 4) % (CONFIG::INSTRUCTION_BUFFER_SIZE / 4);
	
	if(base_addr != instruction_buffer_base_addr)
	{
		EFFECTIVE_ADDRESS size_to_protection_boundary;
		ADDRESS base_virt_addr;
		PHYSICAL_ADDRESS base_phys_addr;
		STORAGE_ATTR storage_attr;
		
		if(unlikely((!this->template Translate</* debug */ false, /* exec */ true, /* write */ false>(base_addr, size_to_protection_boundary, base_virt_addr, base_phys_addr, storage_attr)))) return false;
		
		if(unlikely(!this->SuperMSS::InstructionFetch(base_virt_addr, base_phys_addr, instruction_buffer, CONFIG::INSTRUCTION_BUFFER_SIZE, storage_attr))) return false;
		
#if BYTE_ORDER == BIG_ENDIAN
		if(unlikely(storage_attr & TYPES::SA_E)) // little-endian ?
#elif BYTE_ORDER == LITTLE_ENDIAN
		if(likely(!(storage_attr & TYPES::SA_E))) // big-endian ?
#else
		if(0)
#endif
		{
			unisim::util::endian::BSwap(instruction_buffer, CONFIG::INSTRUCTION_BUFFER_SIZE / 4);
		}
		
		instruction_buffer_base_addr = base_addr;
	}
	else
	{
		// Account for level 1 cache access
		typename CONFIG::CPU::INSTRUCTION_CACHE_HIERARCHY::L1CACHE *l1i = static_cast<typename CONFIG::CPU *>(this)->GetCache((typename CONFIG::CPU::INSTRUCTION_CACHE_HIERARCHY::L1CACHE *) 0);
		if(l1i && l1i->IsEnabled())
		{
			l1i->Access();
		}
	}
	
	insn = instruction_buffer[instruction_buffer_index];
	
	return true;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::StepOneInstruction()
{
	this->ProcessExceptions();

	if(unlikely(this->requires_fetch_instruction_reporting))
	{
		if(unlikely(this->memory_access_reporting_import != 0))
		{
			this->memory_access_reporting_import->ReportFetchInstruction(this->cia);
		}
	}
	
	if(unlikely(this->debug_yielding_import != 0))
	{
		this->debug_yielding_import->DebugYield();
	}

	EFFECTIVE_ADDRESS addr = this->cia;
	typename CONFIG::CODE_TYPE insn = 0;
	if(likely(InstructionFetch(addr, insn)))
	{
		operation = decoder.Decode(addr, insn);

		unsigned int length = operation->GetLength() / 8;
		this->nia = this->cia + length;
		if(unlikely(this->enable_insn_trace))
		{
			std::stringstream sstr;
			operation->disasm(static_cast<typename CONFIG::CPU *>(this), sstr);
			this->logger << DebugInfo << "executing instruction #" << this->instruction_counter << ":0x" << std::hex << addr << std::dec << ":" << sstr.str() << EndDebugInfo;
		}
		
		/* execute the instruction */
		if(likely(operation->execute(static_cast<typename CONFIG::CPU *>(this))))
		{
			/* update the instruction counter */
			this->instruction_counter++;
			
			/* report a finished instruction */
			if(unlikely(this->requires_commit_instruction_reporting))
			{
				if(unlikely(this->memory_access_reporting_import != 0))
				{
					this->memory_access_reporting_import->ReportCommitInstruction(this->cia, length);
				}
			}

			/* go to the next instruction */
			this->cia = this->nia;

			if(unlikely(this->trap_reporting_import && (this->instruction_counter == this->trap_on_instruction_counter)))
			{
				this->trap_reporting_import->ReportTrap();
			}
			
			if(unlikely((this->instruction_counter >= this->max_inst) || (this->cia == this->halt_on_addr))) this->Halt();
		}
		else if(unlikely(this->verbose_exception))
		{
			std::stringstream sstr;
			operation->disasm(static_cast<typename CONFIG::CPU *>(this), sstr);
			this->logger << DebugInfo << "Aborted instruction #" << this->instruction_counter << ":0x" << std::hex << addr << std::dec << ":" << sstr.str() << EndDebugInfo;
		}
	}
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::TimeBaseValueChanged(uint64_t old_value)
{
	//std::cerr << "tb: " << old_value << " -> " << uint64_t(tb) << std::endl;
	uint64_t new_value = uint64_t(tb);
	uint32_t tb_fit_mask = 4096 << (4 * tcr.template Get<typename TCR::FP>());
	if(!(old_value & tb_fit_mask) && (new_value & tb_fit_mask)) // selected bit toggle from 0 to 1
	{
		//std::cerr << "tb_fit_mask=0x" << std::hex << tb_fit_mask << std::dec << std::endl;
		if(tsr.template Get<typename TSR::ENW>())
		{
			if(tsr.template Get<typename TSR::WIS>())
			{
				if(tcr.template Get<typename TCR::WRC>() != 0)
				{
					tsr.template Set<typename TSR::WRS>(tcr.template Get<typename TCR::WRC>()); // TSR[WRS]=TCR[WRC]
					this->logger << DebugWarning << "A reset because of watchdog timer occured" << EndDebugWarning;
					this->Stop(0);
				}
				else
				{
					// nothing
				}
			}
			else
			{
				tsr.template Set<typename TSR::WIS>(1);
			}
		}
		else
		{
			tsr.template Set<typename TSR::ENW>(1);
		}
		
		this->template ThrowException<typename FixedIntervalTimerInterrupt::FixedIntervalTimer>();
	}
	
	uint64_t tb_watchdog_mask = uint64_t(1048576) << (4 * tcr.template Get<typename TCR::WP>());
	if(!(old_value & tb_watchdog_mask) && (new_value & tb_watchdog_mask)) // selected bit toggle from 0 to 1
	{
		//std::cerr << "cnt=" << cnt << ", tb_watchdog_mask=0x" << std::hex << tb_watchdog_mask << std::dec << std::endl;
		tsr.template Set<typename TSR::WIS>(1);
		this->template ThrowException<typename WatchdogTimerInterrupt::WatchdogTimer>();
	}
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::RunTimers(uint64_t delta)
{
	tb = uint64_t(tb) + delta;
	
	uint32_t old_dec_value = uint32_t(dec);
	if(old_dec_value > 0)
	{
		uint32_t new_dec_value = (delta <= old_dec_value) ? old_dec_value - delta : 0;
		
		dec = new_dec_value;
		
		if(unlikely(new_dec_value == 0))
		{
			tsr.template Set<typename TSR::DIS>(1);
			this->template ThrowException<typename DecrementerInterrupt::Decrementer>();
			
			if(tcr.template Get<typename TCR::ARE>())
			{
				//std::cerr << "autoreloading" << std::endl;
				if((delta > old_dec_value) && uint32_t(decar)) // several autoreload occured during instruction execution
				{
					dec = uint32_t(decar) - ((delta - old_dec_value) % uint32_t(decar));
				}
				else
				{
					dec = uint32_t(decar);
				}
			}
		}
	}
}

template <typename TYPES, typename CONFIG>
uint64_t CPU<TYPES, CONFIG>::GetMaxIdleTime() const
{
	uint64_t delay_dec = 0xffffffffffffffffULL;
	uint64_t delay_fit = 0xffffffffffffffffULL;
	uint64_t delay_watchdog = 0xffffffffffffffffULL;
	
	if(msr.template Get<typename MSR::EE>())
	{
		if(tcr.template Get<typename TCR::DIE>())
		{
			uint32_t dec_value = uint32_t(dec);
			if(dec_value) delay_dec = dec_value;
			//std::cerr << "Time for DEC to reach zero: " << delay_dec << std::endl;
		}
		
		if(tcr.template Get<typename TCR::FIE>())
		{
			uint64_t tb_value = uint64_t(tb);
			
			uint32_t tb_fit_mask = 4096 << (4 * tcr.template Get<typename TCR::FP>());
			
			if(tb_value & tb_fit_mask)
			{
				// time to toogle from 1 to 0
				delay_fit = tb_fit_mask - (tb_value & (tb_fit_mask - 1));
				// time to toogle from 0 to 1
				delay_fit += tb_fit_mask;
			}
			else
			{
				// time to toogle from 0 to 1
				delay_fit = tb_fit_mask - (tb_value & (tb_fit_mask - 1));
			}
			//std::cerr << "Time for FIT: " << delay_fit << " (tb=0x" << std::hex << tb_value << std::dec << ", FIT mask=" << std::hex << tb_fit_mask << std::dec << ")" << std::endl;
		}
	}
	
	if(tcr.template Get<typename TCR::WIE>() && msr.template Get<typename MSR::CE>())
	{
		uint64_t tb_value = uint64_t(tb);

		uint64_t tb_watchdog_mask = uint64_t(1048576) << (4 * tcr.template Get<typename TCR::WP>());
		
		if(tb & tb_watchdog_mask)
		{
			// time to toogle from 1 to 0
			delay_watchdog = tb_watchdog_mask - (tb_value & (tb_watchdog_mask - 1));
			// time to toogle from 0 to 1
			delay_watchdog += tb_watchdog_mask;
		}
		else
		{
			// time to toogle from 0 to 1
			delay_watchdog = tb_watchdog_mask - (tb_value & (tb_watchdog_mask - 1));
		}
		//std::cerr << "Time for WDG: " << delay_watchdog << " (tb=0x" << std::hex << GetTB() << std::dec << ", WDG mask=" << std::hex << tb_watchdog_mask << std::dec << ")" << std::endl;
	}
	
	uint64_t max_idle_time = delay_dec < delay_fit ? (delay_dec < delay_watchdog ? delay_dec : delay_watchdog) : (delay_fit < delay_watchdog ? delay_fit : delay_watchdog);
	return max_idle_time;
}

template <typename TYPES, typename CONFIG>
uint64_t CPU<TYPES, CONFIG>::GetTimersDeadline() const
{
	uint64_t delay_dec = 0xffffffffffffffffULL;
	uint64_t delay_fit = 0xffffffffffffffffULL;
	uint64_t delay_watchdog = 0xffffffffffffffffULL;
	
	uint32_t dec_value = uint32_t(dec);
	if(dec_value) delay_dec = dec_value;
	
	uint64_t tb_value = uint64_t(tb);
	
	uint32_t tb_fit_mask = 4096 << (4 * tcr.template Get<typename TCR::FP>());
	
	if(tb_value & tb_fit_mask)
	{
		// time to toogle from 1 to 0
		delay_fit = tb_fit_mask - (tb_value & (tb_fit_mask - 1));
		// time to toogle from 0 to 1
		delay_fit += tb_fit_mask;
	}
	else
	{
		// time to toogle from 0 to 1
		delay_fit = tb_fit_mask - (tb_value & (tb_fit_mask - 1));
	}
	
	uint32_t tb_watchdog_mask = 1048576 << (4 * tcr.template Get<typename TCR::WP>());
	
	if(tb & tb_watchdog_mask)
	{
		// time to toogle from 1 to 0
		delay_watchdog = tb_watchdog_mask - (tb_value & (tb_watchdog_mask - 1));
		// time to toogle from 0 to 1
		delay_watchdog += tb_watchdog_mask;
	}
	else
	{
		// time to toogle from 0 to 1
		delay_watchdog = tb_watchdog_mask - (tb_value & (tb_watchdog_mask - 1));
	}
	
	uint64_t timers_deadline = delay_dec < delay_fit ? (delay_dec < delay_watchdog ? delay_dec : delay_watchdog) : (delay_fit < delay_watchdog ? delay_fit : delay_watchdog);
	return timers_deadline;
}

} // end of namespace book_e
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_CPU_TCC__
