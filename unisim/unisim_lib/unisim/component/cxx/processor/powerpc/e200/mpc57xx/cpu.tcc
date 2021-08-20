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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_CPU_TCC__

#include <unisim/component/cxx/processor/powerpc/cpu.tcc>
#include <unisim/util/simfloat/floating.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e200 {
namespace mpc57xx {

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::CPU(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, SuperCPU(name, parent)
	, unisim::kernel::Service<unisim::service::interfaces::Disassembly<EFFECTIVE_ADDRESS> >(name, parent)
	, disasm_export("disasm-export", this)
	, local_memory_export("local-memory-export", this)
	, cpuid(0x0)
	, param_cpuid("cpuid", this, cpuid, "CPU ID at reset")
	, processor_version(0x0)
	, param_processor_version("processor-version", this, processor_version, "Processor Version")
	, system_version(0x0)
	, param_system_version("system-version", this, system_version, "System Version")
	, system_information(0x0)
	, param_system_information("system-information", this, system_information, "System Information")
	, local_memory_base_addr(0)
	, param_local_memory_base_addr("local-memory-base-addr", this, local_memory_base_addr, "local memory base address")
	, local_memory_size(0)
	, param_local_memory_size("local-memory-size", this, local_memory_size, "local memory size")
	, enable_auto_vectored_interrupts(true)
	, vector_offset(0x0)
	, system_reset_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, critical_input_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, machine_check_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, data_storage_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, instruction_storage_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, external_input_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, alignment_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, program_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, performance_monitor_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, system_call_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, debug_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, embedded_floating_point_data_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, embedded_floating_point_round_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, instruction_buffer_base_addr(~EFFECTIVE_ADDRESS(0))
	, instruction_buffer()
	, vle_decoder()
	, operation(0)
	, msr(static_cast<typename CONFIG::CPU *>(this))
	, srr0(static_cast<typename CONFIG::CPU *>(this))
	, srr1(static_cast<typename CONFIG::CPU *>(this))
	, pid0(static_cast<typename CONFIG::CPU *>(this))
	, csrr0(static_cast<typename CONFIG::CPU *>(this))
	, csrr1(static_cast<typename CONFIG::CPU *>(this))
	, dear(static_cast<typename CONFIG::CPU *>(this))
	, esr(static_cast<typename CONFIG::CPU *>(this))
	, ivpr(static_cast<typename CONFIG::CPU *>(this))
	, usprg0(static_cast<typename CONFIG::CPU *>(this))
	, sprg0(static_cast<typename CONFIG::CPU *>(this))
	, sprg1(static_cast<typename CONFIG::CPU *>(this))
	, sprg2(static_cast<typename CONFIG::CPU *>(this))
	, sprg3(static_cast<typename CONFIG::CPU *>(this))
	, pir(static_cast<typename CONFIG::CPU *>(this))
	, pvr(static_cast<typename CONFIG::CPU *>(this))
	, dbsr(static_cast<typename CONFIG::CPU *>(this))
	, dbcr0(static_cast<typename CONFIG::CPU *>(this))
	, dbcr1(static_cast<typename CONFIG::CPU *>(this))
	, dbcr2(static_cast<typename CONFIG::CPU *>(this))
	, iac1(static_cast<typename CONFIG::CPU *>(this))
	, iac2(static_cast<typename CONFIG::CPU *>(this))
	, iac3(static_cast<typename CONFIG::CPU *>(this))
	, iac4(static_cast<typename CONFIG::CPU *>(this))
	, dac1(static_cast<typename CONFIG::CPU *>(this))
	, dac2(static_cast<typename CONFIG::CPU *>(this))
	, dvc1(static_cast<typename CONFIG::CPU *>(this))
	, dvc2(static_cast<typename CONFIG::CPU *>(this))
	, tir(static_cast<typename CONFIG::CPU *>(this))
	, spefscr(static_cast<typename CONFIG::CPU *>(this))
	, npidr(static_cast<typename CONFIG::CPU *>(this))
	, dbcr3(static_cast<typename CONFIG::CPU *>(this))
	, dbcnt(static_cast<typename CONFIG::CPU *>(this))
	, dbcr4(static_cast<typename CONFIG::CPU *>(this))
	, dbcr5(static_cast<typename CONFIG::CPU *>(this))
	, iac5(static_cast<typename CONFIG::CPU *>(this))
	, iac6(static_cast<typename CONFIG::CPU *>(this))
	, iac7(static_cast<typename CONFIG::CPU *>(this))
	, iac8(static_cast<typename CONFIG::CPU *>(this))
	, mcsrr0(static_cast<typename CONFIG::CPU *>(this))
	, mcsrr1(static_cast<typename CONFIG::CPU *>(this))
	, mcsr(static_cast<typename CONFIG::CPU *>(this))
	, mcar(static_cast<typename CONFIG::CPU *>(this))
	, dsrr0(static_cast<typename CONFIG::CPU *>(this))
	, dsrr1(static_cast<typename CONFIG::CPU *>(this))
	, ddam(static_cast<typename CONFIG::CPU *>(this))
	, dac3(static_cast<typename CONFIG::CPU *>(this))
	, dac4(static_cast<typename CONFIG::CPU *>(this))
	, dbcr7(static_cast<typename CONFIG::CPU *>(this))
	, dbcr8(static_cast<typename CONFIG::CPU *>(this))
	, ddear(static_cast<typename CONFIG::CPU *>(this))
	, dvc1u(static_cast<typename CONFIG::CPU *>(this))
	, dvc2u(static_cast<typename CONFIG::CPU *>(this))
	, dbcr6(static_cast<typename CONFIG::CPU *>(this))
	, edbrac0(static_cast<typename CONFIG::CPU *>(this))
	, devent(static_cast<typename CONFIG::CPU *>(this))
	, sir(static_cast<typename CONFIG::CPU *>(this))
	, hid0(static_cast<typename CONFIG::CPU *>(this))
	, hid1(static_cast<typename CONFIG::CPU *>(this))
	, bucsr(static_cast<typename CONFIG::CPU *>(this))
	, svr(static_cast<typename CONFIG::CPU *>(this))
	, e2ectl0(static_cast<typename CONFIG::CPU *>(this))
	, e2eecsr0(static_cast<typename CONFIG::CPU *>(this))
	, pmc0(static_cast<typename CONFIG::CPU *>(this))
	, pmc1(static_cast<typename CONFIG::CPU *>(this))
	, pmc2(static_cast<typename CONFIG::CPU *>(this))
	, pmc3(static_cast<typename CONFIG::CPU *>(this))
	, pmgc0(static_cast<typename CONFIG::CPU *>(this))
	, pmlca0(static_cast<typename CONFIG::CPU *>(this))
	, pmlca1(static_cast<typename CONFIG::CPU *>(this))
	, pmlca2(static_cast<typename CONFIG::CPU *>(this))
	, pmlca3(static_cast<typename CONFIG::CPU *>(this))
	, pmlcb0(static_cast<typename CONFIG::CPU *>(this))
	, pmlcb1(static_cast<typename CONFIG::CPU *>(this))
	, pmlcb2(static_cast<typename CONFIG::CPU *>(this))
	, pmlcb3(static_cast<typename CONFIG::CPU *>(this))
	, upmc0(static_cast<typename CONFIG::CPU *>(this), &pmc0)
	, upmc1(static_cast<typename CONFIG::CPU *>(this), &pmc1)
	, upmc2(static_cast<typename CONFIG::CPU *>(this), &pmc2)
	, upmc3(static_cast<typename CONFIG::CPU *>(this), &pmc3)
	, upmgc0(static_cast<typename CONFIG::CPU *>(this), &pmgc0)
	, upmlca0(static_cast<typename CONFIG::CPU *>(this), &pmlca0)
	, upmlca1(static_cast<typename CONFIG::CPU *>(this), &pmlca1)
	, upmlca2(static_cast<typename CONFIG::CPU *>(this), &pmlca2)
	, upmlca3(static_cast<typename CONFIG::CPU *>(this), &pmlca3)
	, upmlcb0(static_cast<typename CONFIG::CPU *>(this), &pmlcb0)
	, upmlcb1(static_cast<typename CONFIG::CPU *>(this), &pmlcb1)
	, upmlcb2(static_cast<typename CONFIG::CPU *>(this), &pmlcb2)
	, upmlcb3(static_cast<typename CONFIG::CPU *>(this), &pmlcb3)
	, tmcfg0(static_cast<typename CONFIG::CPU *>(this))
	, local_memory_gate("local-memory-gate", this)
{
	disasm_export.SetupDependsOn(this->memory_import);
	
	local_memory_export >> local_memory_gate.local_memory_export;
	
	param_processor_version.SetMutable(false);
	param_system_version.SetMutable(false);
	param_system_information.SetMutable(false);
	
	pvr.Initialize(processor_version);
	svr.Initialize(system_version);
	sir.Initialize(system_information);
	
	this->SuperCPU::template EnableInterrupt<SystemResetInterrupt>();
	this->SuperCPU::template EnableException<typename MachineCheckInterrupt::NMI>();
	this->SuperCPU::template EnableInterrupt<DataStorageInterrupt>();
	this->SuperCPU::template EnableInterrupt<InstructionStorageInterrupt>();
	this->SuperCPU::template EnableInterrupt<AlignmentInterrupt>();
	this->SuperCPU::template EnableInterrupt<ProgramInterrupt>();
	this->SuperCPU::template EnableInterrupt<EmbeddedFloatingPointDataInterrupt>();
	this->SuperCPU::template EnableInterrupt<EmbeddedFloatingPointRoundInterrupt>();
	this->SuperCPU::template EnableInterrupt<SystemCallInterrupt>();
}

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::~CPU()
{
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::EndSetup()
{
	if(!SuperCPU::EndSetup()) return false;
	
	return true;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::Reset()
{
	SuperCPU::Reset();
	msr.Initialize(0x0);
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(SystemResetInterrupt *system_reset_interrupt)
{
	struct B0_29 : Field<void, 0, 29> {};
	
	this->hid0.template Set<typename HID0::NHR>(0); // signal to software that this is a hardware reset
	
	Reset();
	
	esr = 0;

	msr.template Set<typename SystemResetInterrupt::MSR_CLEARED_FIELDS>(0);
	
	// Vector: p_rstbase[0:29] || 2'b00
	this->cia = this->reset_addr & B0_29::template GetMask<EFFECTIVE_ADDRESS>();
	
	this->template AckInterrupt<SystemResetInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(MachineCheckInterrupt *machine_check_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	mcsrr0 = this->cia;
	mcsrr1 = msr;

	if(this->template RecognizedException<typename MachineCheckInterrupt::NMI>()) mcsr.template Set<typename MCSR::NMI>(1);

	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_MCP                                      )) mcsr.template Set<typename MCSR::MCP>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_EXCEPTION_ISI                            )) mcsr.template Set<typename MCSR::EXCP_ERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_EXCEPTION_BUS_ERROR                      )) mcsr.template Set<typename MCSR::EXCP_ERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_INSTRUCTION_CACHE_DATA_ARRAY_PARITY_ERROR)) mcsr.template Set<typename MCSR::IC_DPERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_DATA_CACHE_DATA_ARRAY_PARITY_ERROR       )) mcsr.template Set<typename MCSR::DC_DPERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_INSTRUCTION_CACHE_TAG_PARITY_ERROR       )) mcsr.template Set<typename MCSR::IC_TPERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_DATA_CACHE_TAG_PARITY_ERROR              )) mcsr.template Set<typename MCSR::DC_TPERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_INSTRUCTION_CACHE_LOCK_ERROR             )) mcsr.template Set<typename MCSR::IC_LKERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_DATA_CACHE_LOCK_ERROR                    )) mcsr.template Set<typename MCSR::DC_LKERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_INSTRUCTION_FETCH_ERROR_REPORT           )) mcsr.template Set<typename MCSR::IF>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_LOAD_TYPE_INSTRUCTION_ERROR_REPORT       )) mcsr.template Set<typename MCSR::LD>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_STORE_TYPE_INSTRUCTION_ERROR_REPORT      )) mcsr.template Set<typename MCSR::ST>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_GUARDED_INSTRUCTION_ERROR_REPORT         )) mcsr.template Set<typename MCSR::G>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_STACK_LIMIT_CHECK_FAILURE                )) mcsr.template Set<typename MCSR::STACK_ERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_INSTRUCTION_MEMORY_PARITY_ERROR          )) mcsr.template Set<typename MCSR::IMEM_PERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_DATA_MEMORY_READ_PARITY_ERROR            )) mcsr.template Set<typename MCSR::DMEM_RDPERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_DATA_MEMORY_WRITE_PARITY_ERROR           )) mcsr.template Set<typename MCSR::DMEM_WRPERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_INSTRUCTION_READ_BUS_ERROR               )) mcsr.template Set<typename MCSR::BUS_IRERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_DATA_READ_BUS_ERROR                      )) mcsr.template Set<typename MCSR::BUS_DRERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_DATA_WRITE_BUS_ERROR                     )) mcsr.template Set<typename MCSR::BUS_WRERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_DATA_WRITE_BUS_ERROR_DSI                 )) mcsr.template Set<typename MCSR::BUS_WRDSI>(1);
	
	if(machine_check_interrupt->HasAddress())
	{
		mcsr.template Set<typename MCSR::MAV>(1);
		mcsr.template Set<typename MCSR::U>(msr.template Get<typename MSR::PR>());
		mcsr.template Set<typename MCSR::MEA>(0); // 1=effective address, 0=physical address. Note: there is no MMU in this CPU.
		mcar = machine_check_interrupt->GetAddress();
		machine_check_interrupt->ClearAddress();
	}
	else
	{
		mcsr.template Set<typename MCSR::MAV>(0);
	}

	msr.template Set<typename MachineCheckInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	if(hid0.template Get<typename HID0::MCCLRDE>()) msr.template Set<typename MSR::DE>(0);
	
	// Vector: IVPR[0:23] || 0x10
	this->cia = (ivpr & B0_23::template GetMask<EFFECTIVE_ADDRESS>()) | (MachineCheckInterrupt::OFFSET & B24_31::template GetMask<EFFECTIVE_ADDRESS>());
	
	machine_check_interrupt->ClearEvents();
	
	this->template AckInterrupt<MachineCheckInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(DataStorageInterrupt *data_storage_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = this->cia;
	srr1 = msr;
	
	// ESR: [ST], [SPV], VLEMI. All other bits cleared
	esr = 0;
	operation->update_esr_st(static_cast<typename CONFIG::CPU *>(this));
	operation->update_esr_spv(static_cast<typename CONFIG::CPU *>(this));
	esr.template Set<typename ESR::VLEMI>(1);
	
	assert(data_storage_interrupt->HasAddress());
	dear = data_storage_interrupt->GetAddress();
	
	msr.template Set<typename DataStorageInterrupt::MSR_CLEARED_FIELDS>(0);
	
	// Vector: IVPR[0:23] || 0x20
	this->cia = (ivpr & B0_23::template GetMask<EFFECTIVE_ADDRESS>()) | (DataStorageInterrupt::OFFSET & B24_31::template GetMask<EFFECTIVE_ADDRESS>());

	this->template AckInterrupt<DataStorageInterrupt>();
	
	data_storage_interrupt->ClearAddress();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(InstructionStorageInterrupt *instruction_storage_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = this->cia;
	srr1 = msr;
	
	// ESR: VLEMI. All other bits cleared
	esr = 0;
	esr.template Set<typename ESR::VLEMI>(1);
	
	msr.template Set<typename InstructionStorageInterrupt::MSR_CLEARED_FIELDS>(0);

	// Vector: IVPR[0:23] || 0x30
	this->cia = (ivpr & B0_23::template GetMask<EFFECTIVE_ADDRESS>()) | (InstructionStorageInterrupt::OFFSET & B24_31::template GetMask<EFFECTIVE_ADDRESS>());

	this->template AckInterrupt<InstructionStorageInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(AlignmentInterrupt *alignment_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = this->cia;
	srr1 = msr;
	
	// ESR: [ST], VLEMI. All other bits cleared
	esr = 0;
	operation->update_esr_st(static_cast<typename CONFIG::CPU *>(this));
	esr.template Set<typename ESR::VLEMI>(1);
	
	assert(alignment_interrupt->HasAddress());
	dear = alignment_interrupt->GetAddress();
	
	msr.template Set<typename AlignmentInterrupt::MSR_CLEARED_FIELDS>(0);
	
	// Vector: IVPR[0:23] || 0x50
	this->cia = (ivpr & B0_23::template GetMask<EFFECTIVE_ADDRESS>()) | (AlignmentInterrupt::OFFSET & B24_31::template GetMask<EFFECTIVE_ADDRESS>());

	this->template AckInterrupt<AlignmentInterrupt>();
	
	alignment_interrupt->ClearAddress();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(ProgramInterrupt *program_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = this->cia;
	srr1 = msr;
	
	// ESR: VLEMI. All other bits cleared
	esr = 0;
	esr.template Set<typename ESR::VLEMI>(1);
	if(this->template RecognizedException<typename ProgramInterrupt::IllegalInstruction>() || this->template RecognizedException<typename ProgramInterrupt::UnimplementedInstruction>())
	{
		esr.template Set<typename ESR::PIL>(1);
	}
	else if(this->template RecognizedException<typename ProgramInterrupt::PrivilegeViolation>())
	{
		esr.template Set<typename ESR::PPR>(1);
	}
	else if(this->template RecognizedException<typename ProgramInterrupt::Trap>())
	{
		esr.template Set<typename ESR::PTR>(1);
	}
	
	msr.template Set<typename ProgramInterrupt::MSR_CLEARED_FIELDS>(0);
	
	// Vector: IVPR[0:23] || 0x60
	this->cia = (ivpr & B0_23::template GetMask<EFFECTIVE_ADDRESS>()) | (ProgramInterrupt::OFFSET & B24_31::template GetMask<EFFECTIVE_ADDRESS>());

	this->template AckInterrupt<ProgramInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(EmbeddedFloatingPointDataInterrupt *embedded_floating_point_data_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = this->cia;
	srr1 = msr;
	
	// ESR: SPV, VLEMI. All other bits cleared
	esr = 0;
	esr.template Set<typename ESR::SPV>(1);
	esr.template Set<typename ESR::VLEMI>(1);
	
	msr.template Set<typename EmbeddedFloatingPointDataInterrupt::MSR_CLEARED_FIELDS>(0);
	
	// Vector: IVPR[0:23] || 0xA0
	this->cia = (ivpr & B0_23::template GetMask<EFFECTIVE_ADDRESS>()) | (EmbeddedFloatingPointDataInterrupt::OFFSET & B24_31::template GetMask<EFFECTIVE_ADDRESS>());
	
	this->template AckInterrupt<EmbeddedFloatingPointDataInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(EmbeddedFloatingPointRoundInterrupt *embedded_floating_point_round_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = this->cia;
	srr1 = msr;
	
	// ESR: SPV, VLEMI. All other bits cleared
	esr = 0;
	esr.template Set<typename ESR::SPV>(1);
	esr.template Set<typename ESR::VLEMI>(1);
	
	msr.template Set<typename EmbeddedFloatingPointDataInterrupt::MSR_CLEARED_FIELDS>(0);
	
	// Vector: IVPR[0:23] || 0xB0
	this->cia = (ivpr & B0_23::template GetMask<EFFECTIVE_ADDRESS>()) | (EmbeddedFloatingPointRoundInterrupt::OFFSET & B24_31::template GetMask<EFFECTIVE_ADDRESS>());
	
	this->template AckInterrupt<EmbeddedFloatingPointRoundInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(SystemCallInterrupt *system_call_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = this->nia;
	srr1 = msr;
	
	// ESR: VLEMI. All other bits cleared
	esr = 0;
	esr.template Set<typename ESR::VLEMI>(1);
	
	msr.template Set<typename SystemCallInterrupt::MSR_CLEARED_FIELDS>(0);
	
	// Vector: IVPR[0:23] || 0xB0
	this->cia = (ivpr & B0_23::template GetMask<EFFECTIVE_ADDRESS>()) | (SystemCallInterrupt::OFFSET & B24_31::template GetMask<EFFECTIVE_ADDRESS>());
	
	this->template AckInterrupt<SystemCallInterrupt>();
	
	system_call_interrupt->ClearELEV();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(CriticalInputInterrupt *critical_input_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B0_15  : Field<void,  0, 15> {};
	struct B16_29 : Field<void, 16, 29> {};
	struct B24_31 : Field<void, 24, 31> {};
	
	csrr0 = this->cia;
	csrr1 = msr;
	
	msr.template Set<typename CriticalInputInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	if(hid0.template Get<typename HID0::CICLRDE>()) msr.template Set<typename MSR::DE>(0);
	
	if(enable_auto_vectored_interrupts)
	{
		// Vector: IVPR[0:23] || 0x00 (autovectored)
		this->cia = (ivpr & B0_23::template GetMask<EFFECTIVE_ADDRESS>()) | (CriticalInputInterrupt::OFFSET & B24_31::template GetMask<EFFECTIVE_ADDRESS>());
	}
	else
	{
		// Vector: IVPR[0:15] || (IVPR[16:29] | p_voffset[0:13]) || 2'b00 (non-autovectored)
		this->cia = (ivpr & B0_15::template GetMask<EFFECTIVE_ADDRESS>()) | ((ivpr | vector_offset) & B16_29::template GetMask<EFFECTIVE_ADDRESS>());
		
		InterruptAcknowledge();
	}
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(ExternalInputInterrupt *external_input_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B0_15  : Field<void,  0, 15> {};
	struct B16_29 : Field<void, 16, 29> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename ExternalInputInterrupt::MSR_CLEARED_FIELDS>(0);
	
	if(enable_auto_vectored_interrupts)
	{
		// Vector: IVPR[0:23] || 0x00 (autovectored)
		this->cia = (ivpr & B0_23::template GetMask<EFFECTIVE_ADDRESS>()) | (ExternalInputInterrupt::OFFSET & B24_31::template GetMask<EFFECTIVE_ADDRESS>());
	}
	else
	{
		// Vector: IVPR[0:15] || (IVPR[16:29] | p_voffset[0:13]) || 2'b00 (non-autovectored)
		this->cia = (ivpr & B0_15::template GetMask<EFFECTIVE_ADDRESS>()) | ((ivpr | vector_offset) & B16_29::template GetMask<EFFECTIVE_ADDRESS>());
		
		InterruptAcknowledge();
	}
	
	this->Synchronize(); // Force resampling input signals
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(PerformanceMonitorInterrupt *performance_monitor_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = this->cia;
	srr1 = msr;

#if 0
	if(pmgc0.template Get<typename PMGC0::UDI>())
	{
		dsrr0 = this->cia;
		dsrr1 = msr;
	}
#endif
	msr.template Set<typename PerformanceMonitorInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
#if 0
	if(pmgc0.template Get<typename PMGC0::UDI>())
	{
		if(hid0.template Get<typename HID0::DCLRCE>())
		{
			msr.template Set<typename MSR::CE>(0);
		}
		
		if(hid0.template Get<typename HID0::DCLREE>())
		{
			msr.template Set<typename MSR::EE>(0);
		}
		
		msr.template Set<typename MSR::DE>(0);
	}
#endif
	
	// Vector: IVPR[0:23] || 0x70
	this->cia = (ivpr & B0_23::template GetMask<EFFECTIVE_ADDRESS>()) | (PerformanceMonitorInterrupt::OFFSET & B24_31::template GetMask<EFFECTIVE_ADDRESS>());
	
	this->template AckInterrupt<PerformanceMonitorInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(DebugInterrupt *debug_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	dsrr0 = this->cia;
	dsrr1 = msr;
	
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_UNCONDITIONAL_DEBUG_EVENT           )) dbsr.template Set<typename DBSR::UDE>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_COMPLETE_DEBUG_EVENT    )) dbsr.template Set<typename DBSR::ICMP>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_BRANCH_TAKEN_DEBUG_EVENT            )) dbsr.template Set<typename DBSR::BRT>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INTERRUPT_TAKEN_DEBUG_EVENT         )) dbsr.template Set<typename DBSR::IRPT>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_CRITICAL_INTERRUPT_TAKEN_DEBUG_EVENT)) dbsr.template Set<typename DBSR::CIRPT>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_TRAP_INSTRUCTION_DEBUG_EVENT        )) dbsr.template Set<typename DBSR::TRAP>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_ADDRESS_COMPARE1        )) dbsr.template Set<typename DBSR::IAC1>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_ADDRESS_COMPARE2        )) dbsr.template Set<typename DBSR::IAC2>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_ADDRESS_COMPARE3        )) dbsr.template Set<typename DBSR::IAC3>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_ADDRESS_COMPARE4        )) dbsr.template Set<typename DBSR::IAC4_8>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_ADDRESS_COMPARE5        )) dbsr.template Set<typename DBSR::IAC4_8>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_ADDRESS_COMPARE6        )) dbsr.template Set<typename DBSR::IAC4_8>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_ADDRESS_COMPARE7        )) dbsr.template Set<typename DBSR::IAC4_8>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_ADDRESS_COMPARE8        )) dbsr.template Set<typename DBSR::IAC4_8>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_DATA_ADDRESS_COMPARE1_READ          )) dbsr.template Set<typename DBSR::DAC1R>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_DATA_ADDRESS_COMPARE1_WRITE         )) dbsr.template Set<typename DBSR::DAC1W>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_DATA_ADDRESS_COMPARE2_READ          )) dbsr.template Set<typename DBSR::DAC2R>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_DATA_ADDRESS_COMPARE2_WRITE         )) dbsr.template Set<typename DBSR::DAC2W>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_DEBUG_NOTIFY_INTERRUPT              )) dbsr.template Set<typename DBSR::DNI>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_RETURN_DEBUG_EVENT                  )) dbsr.template Set<typename DBSR::RET>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_CRITICAL_RETURN_DEBUG_EVENT         )) dbsr.template Set<typename DBSR::CRET>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_EXTERNAL_DEBUG_EVENT1               )) dbsr.template Set<typename DBSR::DEVT1>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_EXTERNAL_DEBUG_EVENT2               )) dbsr.template Set<typename DBSR::DEVT2>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_PERFORMANCE_MONITOR_DEBUG_EVENT     )) dbsr.template Set<typename DBSR::PMI>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_MPU_DEBUG_EVENT                     )) dbsr.template Set<typename DBSR::MPU>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_IMPRECISE_DEBUG_EVENT               )) dbsr.template Set<typename DBSR::IDE>(1);
	
	msr.template Set<typename DebugInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	if(hid0.template Get<typename HID0::DCLRCE>())
	{
		msr.template Set<typename MSR::CE>(0);
	}
	
	if(hid0.template Get<typename HID0::DCLREE>())
	{
		msr.template Set<typename MSR::EE>(0);
	}
	
	// Vector: IVPR[0:23] || 0xB0
	this->cia = (ivpr & B0_23::template GetMask<EFFECTIVE_ADDRESS>()) | (DebugInterrupt::OFFSET & B24_31::template GetMask<EFFECTIVE_ADDRESS>());
	
	debug_interrupt->ClearEvents();
	
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
		this->template EnableInterrupt<PerformanceMonitorInterrupt>();
	}
	else
	{
		this->template DisableInterrupt<ExternalInputInterrupt>();
		this->template DisableInterrupt<PerformanceMonitorInterrupt>();
	}
	
	if(msr.template Get<typename MSR::ME>())
	{
		this->template EnableException<typename MachineCheckInterrupt::AsynchronousMachineCheck>();
	}
	else
	{
		this->template DisableException<typename MachineCheckInterrupt::AsynchronousMachineCheck>();
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
void CPU<TYPES, CONFIG>::SetAutoVector(bool value)
{
	if((enable_auto_vectored_interrupts != value))
	{
		if(this->verbose_interrupt)
		{
			this->logger << DebugInfo << "autovector is " << (value ? "enabled" : "disabled") << EndDebugInfo;
		}
		enable_auto_vectored_interrupts = value;
	}
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::SetVectorOffset(EFFECTIVE_ADDRESS value)
{
	vector_offset = value & 0xfffffffcUL;
}

template <typename TYPES, typename CONFIG>
std::string CPU<TYPES, CONFIG>::Disasm(EFFECTIVE_ADDRESS addr, EFFECTIVE_ADDRESS& next_addr)
{
	std::stringstream sstr;
	typename CONFIG::VLE_OPERATION *operation;
	uint32_t insn;

	EFFECTIVE_ADDRESS size_to_protection_boundary;
	ADDRESS virt_addr;
	PHYSICAL_ADDRESS phys_addr;
	STORAGE_ATTR storage_attr;

	if(unlikely((!this->template Translate</* debug */ true, /* exec */ true, /* write */ false>(addr, size_to_protection_boundary, virt_addr, phys_addr, storage_attr))) ||
	   unlikely(!this->SuperMSS::DebugInstructionFetch(virt_addr, phys_addr, &insn, 4, storage_attr)))
	{
		next_addr = addr + 2;
		return std::string("unreadable ?");
	}
	
#if BYTE_ORDER == LITTLE_ENDIAN
	unisim::util::endian::BSwap(insn);
#endif

	operation = vle_decoder.Decode(addr, insn);
	
	// disassemble the instruction
	
	operation->disasm(static_cast<typename CONFIG::CPU *>(this), sstr);
	next_addr = addr + (operation->GetLength() / 8);
#if 0
	sstr << "0x" << std::hex;
	sstr.fill('0');
	switch(operation->GetLength())
	{
		case 0:
		{
			sstr.width(8); 
			sstr << insn << std::dec << " ";
			operation->disasm(static_cast<typename CONFIG::CPU *>(this), sstr);

			next_addr = addr + 4;
			break;
		}
			
		case 16:
		{
			sstr.width(4); 
			sstr << (operation->GetEncoding() >> 16) << std::dec << " ";
			operation->disasm(static_cast<typename CONFIG::CPU *>(this), sstr);

			next_addr = addr + 2;
			break;
		}
		
		case 32:
		{
			sstr.width(8); 
			sstr << operation->GetEncoding() << std::dec << " ";
			operation->disasm(static_cast<typename CONFIG::CPU *>(this), sstr);

			next_addr = addr + 4;
			break;
		}
		
		default:
			throw std::runtime_error("Internal Error! Bad operation length from VLE decoder");
	}
#endif
	
	return sstr.str();
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Lbarx(unsigned int rd, EFFECTIVE_ADDRESS addr)
{
	// TODO: reservation logic
	return this->Int8Load(rd, addr);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Lharx(unsigned int rd, EFFECTIVE_ADDRESS addr)
{
	// TODO: reservation logic
	return this->Int16Load(rd, addr);
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
	this->template ThrowException<typename ProgramInterrupt::UnimplementedInstruction>();
	return false;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Stbcx(unsigned int rs, EFFECTIVE_ADDRESS addr)
{
	// TODO: reservation logic
	// Note: Address match with prior lbarx, lharx, or lwarx not required for store to be performed
	if(!this->Int8Store(rs, addr)) return false;
	// Clear CR0[LT][GT], setCR0[EQ] and copy XER[SO] to CR0[SO]
	this->cr.template Set<typename CR::CR0::LT>(0);
	this->cr.template Set<typename CR::CR0::GT>(0);
	this->cr.template Set<typename CR::CR0::EQ>(1);
	this->cr.template Set<typename CR::CR0::SO>(this->xer.template Get<typename XER::SO>());
	
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Sthcx(unsigned int rs, EFFECTIVE_ADDRESS addr)
{
	// TODO: reservation logic
	// Note: Address match with prior lbarx, lharx, or lwarx not required for store to be performed
	if(!this->Int16Store(rs, addr)) return false;
	// Clear CR0[LT][GT], setCR0[EQ] and copy XER[SO] to CR0[SO]
	this->cr.template Set<typename CR::CR0::LT>(0);
	this->cr.template Set<typename CR::CR0::GT>(0);
	this->cr.template Set<typename CR::CR0::EQ>(1);
	this->cr.template Set<typename CR::CR0::SO>(this->xer.template Get<typename XER::SO>());
	
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
bool CPU<TYPES, CONFIG>::Rfdi()
{
	if(msr.template Get<typename MSR::PR>())
	{
		this->template ThrowException<typename ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	struct B0_30 : Field<void, 0, 30> {};

	this->Branch(dsrr0 & B0_30::template GetMask<EFFECTIVE_ADDRESS>());
	msr = dsrr1;
	
	if(unlikely(this->verbose_interrupt))
	{
		this->logger << DebugInfo << "Returning from " << DebugInterrupt::GetName() << " to 0x" << std::hex << this->nia << std::dec << EndDebugInfo;
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
bool CPU<TYPES, CONFIG>::InstructionFetch(EFFECTIVE_ADDRESS addr, typename CONFIG::VLE_CODE_TYPE& insn)
{
	EFFECTIVE_ADDRESS base_addr = addr & ~(CONFIG::INSTRUCTION_BUFFER_SIZE - 1);
	int instruction_buffer_index = (addr / 2) % (CONFIG::INSTRUCTION_BUFFER_SIZE / 2);
	
	if(base_addr != instruction_buffer_base_addr)
	{
		EFFECTIVE_ADDRESS size_to_protection_boundary;
		ADDRESS base_virt_addr;
		PHYSICAL_ADDRESS base_phys_addr;
		STORAGE_ATTR storage_attr;
		
		if(unlikely((!this->template Translate</* debug */ false, /* exec */ true, /* write */ false>(base_addr, size_to_protection_boundary, base_virt_addr, base_phys_addr, storage_attr)))) return false;
		
		if(unlikely(!this->SuperMSS::InstructionFetch(base_virt_addr, base_phys_addr, instruction_buffer, CONFIG::INSTRUCTION_BUFFER_SIZE, storage_attr))) return false;
		
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
	
	uint16_t insn_hi = instruction_buffer[instruction_buffer_index];
#if BYTE_ORDER == LITTLE_ENDIAN
	unisim::util::endian::BSwap(insn_hi);
#endif

	if(unlikely(((unsigned int) instruction_buffer_index + 1) >= (CONFIG::INSTRUCTION_BUFFER_SIZE / 2)))
	{
		base_addr += CONFIG::INSTRUCTION_BUFFER_SIZE;

		EFFECTIVE_ADDRESS size_to_protection_boundary;
		ADDRESS base_virt_addr;
		PHYSICAL_ADDRESS base_phys_addr;
		STORAGE_ATTR storage_attr;
		
		if(unlikely((!this->template Translate</* debug */ false, /* exec */ true, /* write */ false>(base_addr, size_to_protection_boundary, base_virt_addr, base_phys_addr, storage_attr)))) return false;
		
		if(unlikely(!this->SuperMSS::InstructionFetch(base_virt_addr, base_phys_addr, instruction_buffer, CONFIG::INSTRUCTION_BUFFER_SIZE, storage_attr))) return false;

		instruction_buffer_base_addr = base_addr;
		instruction_buffer_index = -1;
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

	uint16_t insn_lo = instruction_buffer[instruction_buffer_index + 1];
#if BYTE_ORDER == LITTLE_ENDIAN
	unisim::util::endian::BSwap(insn_lo);
#endif
	
	insn = ((uint32_t) insn_hi << 16) | insn_lo;
	
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
	uint32_t insn = 0;
	if(likely(InstructionFetch(addr, insn)))
	{
		operation = vle_decoder.Decode(addr, insn);

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

} // end of namespace mpc57xx
} // end of namespace e200
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_CPU_TCC__
