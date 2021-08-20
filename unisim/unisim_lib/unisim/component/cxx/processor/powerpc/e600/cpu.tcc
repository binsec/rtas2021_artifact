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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E600_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E600_CPU_TCC__

#include <unisim/component/cxx/processor/powerpc/e600/cpu.hh>
#include <unisim/component/cxx/processor/powerpc/cpu.tcc>
#include <unisim/util/simfloat/floating.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e600 {

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
	, system_reset_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, machine_check_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, data_storage_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, instruction_storage_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, external_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, alignment_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, program_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, floating_point_unavailable_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, decrementer_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, system_call_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, trace_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, performance_monitor_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, altivec_unavailable_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, instruction_tlb_miss_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, data_tlb_miss_on_load_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, data_tlb_miss_on_store_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, instruction_address_breakpoint_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, system_management_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, altivec_assist_interrupt(static_cast<typename CONFIG::CPU *>(this))
	, powersave(false)
	, tb(static_cast<typename CONFIG::CPU *>(this))
	, bamr(static_cast<typename CONFIG::CPU *>(this))
	, dabr(static_cast<typename CONFIG::CPU *>(this))
	, dar(static_cast<typename CONFIG::CPU *>(this))
	, dec(static_cast<typename CONFIG::CPU *>(this))
	, dsisr(static_cast<typename CONFIG::CPU *>(this))
	, ear(static_cast<typename CONFIG::CPU *>(this))
	, hid0(static_cast<typename CONFIG::CPU *>(this))
	, hid1(static_cast<typename CONFIG::CPU *>(this))
	, iabr(static_cast<typename CONFIG::CPU *>(this))
	, ictc(static_cast<typename CONFIG::CPU *>(this))
	, ictrl(static_cast<typename CONFIG::CPU *>(this))
	, l2cr(static_cast<typename CONFIG::CPU *>(this))
	, ldstcr(static_cast<typename CONFIG::CPU *>(this))
	, ldstdb(static_cast<typename CONFIG::CPU *>(this))
	, mmcr0(static_cast<typename CONFIG::CPU *>(this))
	, mmcr1(static_cast<typename CONFIG::CPU *>(this))
	, mmcr2(static_cast<typename CONFIG::CPU *>(this))
	, msr(static_cast<typename CONFIG::CPU *>(this))
	, msscr0(static_cast<typename CONFIG::CPU *>(this))
	, msssr0(static_cast<typename CONFIG::CPU *>(this))
	, pir(static_cast<typename CONFIG::CPU *>(this))
	, pmc1(static_cast<typename CONFIG::CPU *>(this))
	, pmc2(static_cast<typename CONFIG::CPU *>(this))
	, pmc3(static_cast<typename CONFIG::CPU *>(this))
	, pmc4(static_cast<typename CONFIG::CPU *>(this))
	, pmc5(static_cast<typename CONFIG::CPU *>(this))
	, pmc6(static_cast<typename CONFIG::CPU *>(this))
	, ptehi(static_cast<typename CONFIG::CPU *>(this))
	, ptelo(static_cast<typename CONFIG::CPU *>(this))
	, pvr(static_cast<typename CONFIG::CPU *>(this))
	, siar(static_cast<typename CONFIG::CPU *>(this))
	, sprgs(static_cast<typename CONFIG::CPU *>(this))
	, srr0(static_cast<typename CONFIG::CPU *>(this))
	, srr1(static_cast<typename CONFIG::CPU *>(this))
	, tlbmiss(static_cast<typename CONFIG::CPU *>(this))
{
	disasm_export.SetupDependsOn(this->memory_import);
	
	param_processor_version.SetMutable(false);
	
	UpdatePowerSave();
	
	pvr.Initialize(processor_version);

	this->SuperCPU::template EnableInterrupt<SystemResetInterrupt>();
	this->SuperCPU::template EnableInterrupt<DataStorageInterrupt>();
	this->SuperCPU::template EnableInterrupt<InstructionStorageInterrupt>();
	this->SuperCPU::template EnableInterrupt<AlignmentInterrupt>();
	this->SuperCPU::template EnableInterrupt<ProgramInterrupt>();
	this->SuperCPU::template EnableInterrupt<FloatingPointUnavailableInterrupt>();
	this->SuperCPU::template EnableInterrupt<SystemCallInterrupt>();
	this->SuperCPU::template EnableInterrupt<AltivecUnavailableInterrupt>();
	this->SuperCPU::template EnableInterrupt<InstructionTLBMissInterrupt>();
	this->SuperCPU::template EnableInterrupt<DataTLBMissOnLoadInterrupt>();
	this->SuperCPU::template EnableInterrupt<DataTLBMissOnStoreInterrupt>();
	this->SuperCPU::template EnableInterrupt<InstructionAddressBreakpointInterrupt>();
	this->SuperCPU::template EnableInterrupt<AltivecAssistInterrupt>();
	
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
	
	msr.template Set<typename MSR::FP>(1);
	hid0.template Set<typename HID0::DCE>(1);
	hid0.template Set<typename HID0::ICE>(1);
	hid1.template Set<typename HID1::ABE>(1);
	l2cr.template Set<typename L2CR::L2E>(1);
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(SystemResetInterrupt *system_reset_interrupt)
{
	// save CIA
	srr0 = this->cia;
	
	// SRR1[0-5]=0, SRR1[6]=MSR[6], SSR1[7-15]=0, SRR1[16-31]=MSR[16-31]
	struct _0_5 : Field<_0_5, 0, 5> {};
	struct _7_15 : Field<_7_15, 7, 15> {};
	struct SRR1_CLEARED_FIELDS : FieldSet<_0_5, _7_15> {};
	srr1 = msr;
	srr1.template Set<SRR1_CLEARED_FIELDS>(0);
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());

	// clear some MSR bits
	msr.template Set<typename SystemResetInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Set HID0[NHR]
	hid0.template Set<typename HID0::NHR>(!this->template RecognizedException<typename SystemResetInterrupt::HardReset>());
	
	// Branch to interrupt handler
	this->cia = SystemResetInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(MachineCheckInterrupt *machine_check_interrupt)
{
	// save CIA
	srr0 = this->cia;
	
	srr1 = 0;
	// SRR1[12]=MCP
	srr1.template Set<typename SRR1::_12>(this->template RecognizedException<typename MachineCheckInterrupt::MCP>());
	// SRR1[13]=TEA
	srr1.template Set<typename SRR1::_13>(this->template RecognizedException<typename MachineCheckInterrupt::TEA>());
	// SRR1[6]=MSR[6], SRR1[16-29]=MSR[16-29]
	struct _6 : Field<_6, 6> {};
	struct _16_29 : Field<_16_29, 16, 29> {};
	struct MSR_SAVED_FIELDS : FieldSet<_6, _16_29> {};
	srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
	
	msssr0 = 0;
	// MSSSR0[19]=TEA
	msssr0.template Set<typename MSSSR0::_19>(this->template RecognizedException<typename MachineCheckInterrupt::TEA>());
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());

	// clear some MSR bits
	msr.template Set<typename MachineCheckInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = MachineCheckInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(DataStorageInterrupt *data_storage_interrupt)
{
	// save CIA
	srr0 = this->cia;
	
	// DAR indicates the faulting address to the operating system
	dar = data_storage_interrupt->GetAddress();
	
	// DSISR indicates to the operating system the cause of the data storage exception:
	//
	//   0   1   2   3   4   5   6   7   8   9   10  11  12 .... 31
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	// |   |   | 0 | 0 |   |   |   | 0 | 0 |   | 0 |   | 0  .... 0 |
	// +-+-+-+-+---+---+-+-+-+-+-+-+---+---+-+-+---+-+-+---+---+---+
	//   |   |           |   |   |           |       |
	//   |   |           |   |   |           |       +----> eciwx/ecowx and EAR[E]=0
	//   |   |           |   |   |           |
	//   |   |           |   |   |           +------------> DABR match
	//   |   |           |   |   |
	//   |   |           |   |   +------------------------> 1 for a store, 0 for a load
	//   |   |           |   |
	//   |   |           |   +----------------------------> write through or cache inhibited (lwarx, stwcx.)
	//   |   |           |
	//   |   |           +--------------------------------> protection violation
	//   |   |
	//   |   +--------------------------------------------> page fault
	//   |
	//   +------------------------------------------------> direct store exception (load/store)
	
	dsisr = 0;
	dsisr.template Set<typename DSISR::_11>(this->template RecognizedException<typename DataStorageInterrupt::ExternalAccessDisabled>());
	dsisr.template Set<typename DSISR::_9>(this->template RecognizedException<typename DataStorageInterrupt::DataAddressBreakpoint>());
	dsisr.template Set<typename DSISR::_6>(data_storage_interrupt->IsWrite());
	dsisr.template Set<typename DSISR::_5>(this->template RecognizedException<typename DataStorageInterrupt::WriteThroughLinkedLoadStore>());
	dsisr.template Set<typename DSISR::_4>(this->template RecognizedException<typename DataStorageInterrupt::ProtectionViolation>());
	dsisr.template Set<typename DSISR::_1>(this->template RecognizedException<typename DataStorageInterrupt::PageFault>());
	dsisr.template Set<typename DSISR::_0>(this->template RecognizedException<typename DataStorageInterrupt::DirectStore>());

	// SRR1[1-4]=0, SRR1[10-15]=0, SRR1[16-23]=MSR[16-23], SRR1[25-27]=MSR[25-27], SRR1[30-31]=MSR[30-31]
	struct _1_4 : Field<_1_4, 1, 4> {};
	struct _10_15 : Field<_10_15, 10, 15> {};
	struct _16_23 : Field<_16_23, 16, 23> {};
	struct _25_27 : Field<_25_27, 25, 27> {};
	struct _30_31 : Field<_30_31, 30, 31> {};
	struct SRR1_CLEARED_FIELDS : FieldSet<_1_4, _10_15> {};
	struct MSR_SAVED_FIELDS : FieldSet<_16_23, _25_27, _30_31> {};
	srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
	srr1.template Set<SRR1_CLEARED_FIELDS>(0);
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
	
	// clear some MSR bits
	msr.template Set<typename DataStorageInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = DataStorageInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	
	this->template AckInterrupt<DataStorageInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(InstructionStorageInterrupt *instruction_storage_interrupt)
{
	// save CIA
	srr0 = this->cia;
	
	// SRR1[2]=0, SRR1[10-15]=0
	struct _2 : Field<_2, 2> {};
	struct _10_15 : Field<_10_15, 10, 15> {};
	struct SRR1_CLEARED_FIELDS : FieldSet<_2, _10_15> {};
	srr1.template Set<SRR1_CLEARED_FIELDS>(0);
	
	// SRR1[1]=page fault
	srr1.template Set<typename SRR1::_1>(this->template RecognizedException<typename InstructionStorageInterrupt::PageFault>());
	
	// SRR1[3]=DirectStore | NoExecute | GuardedMemory
	srr1.template Set<typename SRR1::_3>(
		this->template RecognizedException<typename InstructionStorageInterrupt::DirectStore>() ||
		this->template RecognizedException<typename InstructionStorageInterrupt::NoExecute>() ||
		this->template RecognizedException<typename InstructionStorageInterrupt::GuardedMemory>());
	
	// SRR1[4]=ProtectionViolation
	srr1.template Set<typename SRR1::_4>(this->template RecognizedException<typename InstructionStorageInterrupt::ProtectionViolation>());
	
	// SRR1[16-23]=MSR[16-23]
	struct _16_23 : Field<_16_23, 16, 23> {};
	srr1.template Set<_16_23>(msr.template Get<_16_23>());
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
	
	// clear some MSR bits
	msr.template Set<typename DataStorageInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = InstructionStorageInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	
	this->template AckInterrupt<InstructionStorageInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(ExternalInterrupt *external_interrupt)
{
	// save CIA
	srr0 = external_interrupt->SoftwareInterruptRequest() ? (this->cia + 4) : this->cia;
	
	// SRR1[0]=0, SRR1[2-5]=0, SRR1[6]=MSR[6], SRR1[7-9]=0, SRR1[11-15]=0, SRR1[16-31]=MSR[16-31]
	struct _0 : Field<_0, 0> {};
	struct _2_5 : Field<_2_5, 2, 5> {};
	struct _6 : Field<_6, 6> {};
	struct _7_9 : Field<_7_9, 7, 9> {};
	struct _11_15 : Field<_11_15, 11, 15> {};
	struct _16_31 : Field<_16_31, 16, 31> {};
	struct SRR1_CLEARED_FIELDS : FieldSet<_0, _2_5, _7_9, _11_15> {};
	struct MSR_SAVED_FIELDS : FieldSet<_6, _16_31> {};
	srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
	srr1.template Set<SRR1_CLEARED_FIELDS>(0);

	// if exception caused by SW setting ICTRL[CIRQ] then SRR1[1]=1
	// if exception caused by external HW signal then SRR1[10]=1
	if(external_interrupt->SoftwareInterruptRequest())
	{
		srr1.template Set<typename SRR1::_1>(1);
	}
	else
	{
		srr1.template Set<typename SRR1::_10>(1);
	}
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
	
	// clear some MSR bits
	msr.template Set<typename DataStorageInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = ExternalInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	
	external_interrupt->Clear();
  
	this->template AckInterrupt<ExternalInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(AlignmentInterrupt *alignment_interrupt)
{
	// save CIA
	srr0 = this->cia;
	
	// DAR indicates the faulting address to the operating system
	dar = alignment_interrupt->GetAddress();

	// DSISR helps the operating system to implement misaligned memory accesses not handled in the hardware:
	//
	//   0 ..... 14  15  16  17  18  29  20  21  22  23  24  25  26  27  28  29  30  31
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	// | 0 ..... 0 |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
	// +---+---+---+-+-+-+-+-+-+-+-+---+---+-+-+-+-+---+---+---+-+-+-+-+---+---+---+-+-+
	//               |   |   |   |           |   |               |   |               |
	//               +-+-+   |   +-----+-----+   +-------+-------+   +-------+-------+
	//                 |     |         |                 |                   |
	//                 |     |         |                 |                   +-----------> bits 11-15 of instruction encoding
	//                 |     |         |                 |                                 for update forms, and lmw, lswi, lswx
	//                 |     |         |                 |
	//                 |     |         |                 +-------------------------------> bits 6-10 of instruction encoding. undefined for dcbz
	//                 |     |         |
	//                 |     |         +-------------------------------------------------> index addressing: bits 21-24 of instruction encoding
	//                 |     |                                                             immediate index addressing: bits 1-4 of instruction encoding
	//                 |     |
	//                 |     +-----------------------------------------------------------> index addressing: bits 25 of instruction encoding
	//                 |                                                                   immediate index addressing: bits 5 of instruction encoding
	//                 |
	//                 +-----------------------------------------------------------------> index addressing: bits 29-30 of instruction encoding
	//                                                                                     immediate index addressing: cleared

	// clear DSISR[0-14]
	struct _0_14 : Field<_0_14, 0, 14> {};
	dsisr.template Set<_0_14>(0);
	
	// Look at primary opcode (bits 0-5)
	uint32_t instruction_encoding = operation->GetEncoding();
	struct OPC : Field<OPC, 0, 5> {};
	bool xform = OPC::template Get<uint32_t>(instruction_encoding) == 31;
//	bool xform = ((instruction_encoding >> 26) & 0x3fUL) == 31;
		
	if(xform)
	{
		// X-Form: index addressing
		
		// copy bits 29-30 of instruction encoding to DSISR[15-16]
		struct _29_30 : Field<_29_30, 29, 30> {};
		struct _15_16 : Field<_15_16, 15, 16> {};
		dsisr.template Set<_29_30>(_15_16::template Get<uint32_t>(instruction_encoding));
		
		 // copy bit 25 of instruction encoding to bits 17 of DSISR
		struct _25 : Field<_25, 25> {};
		struct _17 : Field<_17, 17> {};
		dsisr.template Set<_17>(_25::template Get<uint32_t>(instruction_encoding));
		
		 // copy bits 21-24 of instruction encoding to bits 18-21 of DSISR
		struct _21_24 : Field<_21_24, 21, 24> {};
		struct _18_21 : Field<_18_21, 18, 21> {};
		dsisr.template Set<_18_21>(_21_24::template Get<uint32_t>(instruction_encoding));
	}
	else
	{
		// immediate index addressing
		
		// clear DSISR[15-16]
		struct _15_16 : Field<_15_16, 15, 16> {};
		dsisr.template Set<_15_16>(0);
		
		// copy bit 5 of instruction encoding to DSISR[17]
		struct _5 : Field<_5, 5> {};
		struct _16 : Field<_16, 16> {};
		dsisr.template Set<_16>(_5::template Get<uint32_t>(instruction_encoding));
		
		// copy bits 1-4 of instruction encoding to DSISR[18-21]
		struct _1_4 : Field<_1_4, 1, 4> {};
		struct _18_21 : Field<_18_21, 18, 21> {};
		dsisr.template Set<_18_21>(_1_4::template Get<uint32_t>(instruction_encoding));
	}
	
	// copy bits 6-10 of instruction encoding to DSISR[22-26]
	struct _6_10 : Field<_6_10, 6, 10> {};
	struct _22_26 : Field<_22_26, 22, 26> {};
	dsisr.template Set<_22_26>(_6_10::template Get<uint32_t>(instruction_encoding));
		
	// copy bits 11-15 of instruction encoding into DSISR[27-31]
	struct _11_15 : Field<_11_15, 11, 15> {};
	struct _27_31 : Field<_27_31, 27, 31> {};
	dsisr.template Set<_27_31>(_11_15::template Get<uint32_t>(instruction_encoding));
	
	// SRR1[1-4]=0, SRR1[10-15]=0, SRR1[16-23]=MSR[16-23], SRR1[25-27]=MSR[25-27], SRR1[30-31]=MSR[30-31]
	struct _1_4 : Field<_1_4, 1, 4> {};
	struct _10_15 : Field<_10_15, 10, 15> {};
	struct _16_23 : Field<_16_23, 16, 23> {};
	struct _25_27 : Field<_25_27, 25, 27> {};
	struct _30_31 : Field<_30_31, 30, 31> {};
	struct SRR1_CLEARED_FIELDS : FieldSet<_1_4, _10_15> {};
	struct MSR_SAVED_FIELDS : FieldSet<_16_23, _25_27, _30_31> {};
	srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
	srr1.template Set<SRR1_CLEARED_FIELDS>(0);
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
	
	// clear some MSR bits
	msr.template Set<typename AlignmentInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = AlignmentInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	
	this->template AckInterrupt<AlignmentInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(ProgramInterrupt *program_interrupt)
{
	if(this->template RecognizedException<typename ProgramInterrupt::UnimplementedInstruction>())
	{
		this->logger << DebugError << "Unimplemented instruction" << EndDebugError;
		this->Stop(-1);
		return;
	}
	
	// save CIA
	srr0 = this->cia;
	
	srr1 = 0;
	// SRR1[11]=FloatingPoint
	srr1.template Set<typename SRR1::_11>(this->template RecognizedException<typename ProgramInterrupt::FloatingPoint>());
	
	// SRR1[12]=IllegalInstruction
	srr1.template Set<typename SRR1::_12>(this->template RecognizedException<typename ProgramInterrupt::IllegalInstruction>());
	
	// SRR1[13]=PrivilegeViolation
	srr1.template Set<typename SRR1::_13>(this->template RecognizedException<typename ProgramInterrupt::PrivilegeViolation>());
	
	// SRR1[14]=Trap
	srr1.template Set<typename SRR1::_14>(this->template RecognizedException<typename ProgramInterrupt::Trap>());

	// SRR1[16-23]=MSR[16-23], SRR1[25-27]=MSR[25-27], SRR1[30-31]=MSR[30-31]
	struct _16_23 : Field<_16_23, 16, 23> {};
	struct _25_27 : Field<_25_27, 25, 27> {};
	struct _30_31 : Field<_30_31, 30, 31> {};
	struct MSR_SAVED_FIELDS : FieldSet<_16_23, _25_27, _30_31> {};
	srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
	
	// clear some MSR bits
	msr.template Set<typename ProgramInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = ProgramInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	
	this->template AckInterrupt<ProgramInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(FloatingPointUnavailableInterrupt *floating_point_unavailable_interrupt)
{
	// save CIA
	srr0 = this->cia;
	
	// SRR1[1-4]=0, SRR1[10-15]=0, SRR1[16-23]=MSR[16-23], SRR1[25-27]=MSR[25-27], SRR1[30-31]=MSR[30-31]
	struct _1_4 : Field<_1_4, 1, 4> {};
	struct _10_15 : Field<_10_15, 10, 15> {};
	struct _16_23 : Field<_16_23, 16, 23> {};
	struct _25_27 : Field<_25_27, 25, 27> {};
	struct _30_31 : Field<_30_31, 30, 31> {};
	struct SRR1_CLEARED_FIELDS : FieldSet<_1_4, _10_15> {};
	struct MSR_SAVED_FIELDS : FieldSet<_16_23, _25_27, _30_31> {};
	srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
	srr1.template Set<SRR1_CLEARED_FIELDS>(0);
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
	
	// clear some MSR bits
	msr.template Set<typename FloatingPointUnavailableInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = FloatingPointUnavailableInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	
	this->template AckInterrupt<FloatingPointUnavailableInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(DecrementerInterrupt *decrementer_interrupt)
{
	// save CIA
	srr0 = this->cia;
	
	// SRR1[1-4]=0, SRR1[10-15]=0, SRR1[16-23]=MSR[16-23], SRR1[25-27]=MSR[25-27], SRR1[30-31]=MSR[30-31]
	struct _1_4 : Field<_1_4, 1, 4> {};
	struct _10_15 : Field<_10_15, 10, 15> {};
	struct _16_23 : Field<_16_23, 16, 23> {};
	struct _25_27 : Field<_25_27, 25, 27> {};
	struct _30_31 : Field<_30_31, 30, 31> {};
	struct SRR1_CLEARED_FIELDS : FieldSet<_1_4, _10_15> {};
	struct MSR_SAVED_FIELDS : FieldSet<_16_23, _25_27, _30_31> {};
	srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
	srr1.template Set<SRR1_CLEARED_FIELDS>(0);
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
	
	// clear some MSR bits
	msr.template Set<typename DecrementerInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = DecrementerInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	
	this->template AckInterrupt<DecrementerInterrupt>();
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
		// save CIA
		srr0 = this->cia;
		
		// SRR1[1-4]=0, SRR1[10-15]=0, SRR1[16-23]=MSR[16-23], SRR1[25-27]=MSR[25-27], SRR1[30-31]=MSR[30-31]
		struct _1_4 : Field<_1_4, 1, 4> {};
		struct _10_15 : Field<_10_15, 10, 15> {};
		struct _16_23 : Field<_16_23, 16, 23> {};
		struct _25_27 : Field<_25_27, 25, 27> {};
		struct _30_31 : Field<_30_31, 30, 31> {};
		struct SRR1_CLEARED_FIELDS : FieldSet<_1_4, _10_15> {};
		struct MSR_SAVED_FIELDS : FieldSet<_16_23, _25_27, _30_31> {};
		srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
		srr1.template Set<SRR1_CLEARED_FIELDS>(0);
		
		// MSR[LE]=MSR[ILE]
		msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
		
		// clear some MSR bits
		msr.template Set<typename SystemCallInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
		
		// Branch to interrupt handler
		this->cia = SystemCallInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	}
	
	this->template AckInterrupt<SystemCallInterrupt>();
	
	system_call_interrupt->ClearELEV();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(TraceInterrupt *trace_interrupt)
{
	// save CIA
	srr0 = this->cia;
	
	// SRR1[1-4]=0, SRR1[10-15]=0, SRR1[16-23]=MSR[16-23], SRR1[25-27]=MSR[25-27], SRR1[30-31]=MSR[30-31]
	struct _1_4 : Field<_1_4, 1, 4> {};
	struct _10_15 : Field<_10_15, 10, 15> {};
	struct _16_23 : Field<_16_23, 16, 23> {};
	struct _25_27 : Field<_25_27, 25, 27> {};
	struct _30_31 : Field<_30_31, 30, 31> {};
	struct SRR1_CLEARED_FIELDS : FieldSet<_1_4, _10_15> {};
	struct MSR_SAVED_FIELDS : FieldSet<_16_23, _25_27, _30_31> {};
	srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
	srr1.template Set<SRR1_CLEARED_FIELDS>(0);
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
	
	// clear some MSR bits
	msr.template Set<typename TraceInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = TraceInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	
	this->template AckInterrupt<TraceInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(PerformanceMonitorInterrupt *performance_monitor_interrupt)
{
	// save CIA
	srr0 = this->cia;
	
	// SRR1[1-4]=0, SRR1[10-15]=0, SRR1[16-23]=MSR[16-23], SRR1[25-27]=MSR[25-27], SRR1[30-31]=MSR[30-31]
	struct _1_4 : Field<_1_4, 1, 4> {};
	struct _10_15 : Field<_10_15, 10, 15> {};
	struct _16_23 : Field<_16_23, 16, 23> {};
	struct _25_27 : Field<_25_27, 25, 27> {};
	struct _30_31 : Field<_30_31, 30, 31> {};
	struct SRR1_CLEARED_FIELDS : FieldSet<_1_4, _10_15> {};
	struct MSR_SAVED_FIELDS : FieldSet<_16_23, _25_27, _30_31> {};
	srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
	srr1.template Set<SRR1_CLEARED_FIELDS>(0);
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
	
	// clear some MSR bits
	msr.template Set<typename PerformanceMonitorInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = PerformanceMonitorInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	
	this->template AckInterrupt<PerformanceMonitorInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(AltivecUnavailableInterrupt *altivec_unavailable_interrupt)
{
	// save CIA
	srr0 = this->cia;
	
	// SRR1[1-4]=0, SRR1[10-15]=0, SRR1[16-23]=MSR[16-23], SRR1[25-27]=MSR[25-27], SRR1[30-31]=MSR[30-31]
	struct _1_4 : Field<_1_4, 1, 4> {};
	struct _10_15 : Field<_10_15, 10, 15> {};
	struct _16_23 : Field<_16_23, 16, 23> {};
	struct _25_27 : Field<_25_27, 25, 27> {};
	struct _30_31 : Field<_30_31, 30, 31> {};
	struct SRR1_CLEARED_FIELDS : FieldSet<_1_4, _10_15> {};
	struct MSR_SAVED_FIELDS : FieldSet<_16_23, _25_27, _30_31> {};
	srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
	srr1.template Set<SRR1_CLEARED_FIELDS>(0);
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
	
	// clear some MSR bits
	msr.template Set<typename AltivecUnavailableInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = AltivecUnavailableInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	
	this->template AckInterrupt<AltivecUnavailableInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(InstructionTLBMissInterrupt *instruction_tlb_miss_interrupt)
{
	// save CIA
	srr0 = this->cia;

	// Set TLBMISS
	tlbmiss = instruction_tlb_miss_interrupt->GetTLBMISS();
	
	// Set PTEHI
	ptehi = instruction_tlb_miss_interrupt->GetPTEHI();
	
	// SRR1[0-5]=0, SRR1[6]=MSR[6], SRR1[7-11]=0, SRR1[12]=key, SRR1[13-15]=0, SRR1[16-31]=MSR[16-31]
	struct _0_5 : Field<_0_5, 0, 5> {};
	struct _6 : Field<_6, 6> {};
	struct _7_11 : Field<_7_11, 7, 11> {};
	struct _13_15 : Field<_13_15, 13, 15> {};
	struct _16_31 : Field<_16_31, 16, 31> {};
	struct SRR1_CLEARED_FIELDS : FieldSet<_0_5, _7_11, _13_15> {};
	struct MSR_SAVED_FIELDS : FieldSet<_6, _16_31> {};
	srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
	srr1.template Set<typename SRR1::_12>(instruction_tlb_miss_interrupt->GetKey());
	srr1.template Set<SRR1_CLEARED_FIELDS>(0);
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
	
	// clear some MSR bits
	msr.template Set<typename InstructionTLBMissInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = InstructionTLBMissInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	
	this->template AckInterrupt<InstructionTLBMissInterrupt>();
	
	instruction_tlb_miss_interrupt->Clear();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(DataTLBMissOnLoadInterrupt *data_tlb_miss_on_load_interrupt)
{
	// save CIA
	srr0 = this->cia;

	// Set TLBMISS
	tlbmiss = data_tlb_miss_on_load_interrupt->GetTLBMISS();
	
	// Set PTEHI
	ptehi = data_tlb_miss_on_load_interrupt->GetPTEHI();
	
	// SRR1[0-5]=0, SRR1[6]=MSR[6], SRR1[7-11]=0, SRR1[12]=key, SRR1[13-15]=0, SRR1[16-31]=MSR[16-31]
	struct _0_5 : Field<_0_5, 0, 5> {};
	struct _6 : Field<_6, 6> {};
	struct _7_11 : Field<_7_11, 7, 11> {};
	struct _13_15 : Field<_13_15, 13, 15> {};
	struct _16_31 : Field<_16_31, 16, 31> {};
	struct SRR1_CLEARED_FIELDS : FieldSet<_0_5, _7_11, _13_15> {};
	struct MSR_SAVED_FIELDS : FieldSet<_6, _16_31> {};
	srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
	srr1.template Set<typename SRR1::_12>(data_tlb_miss_on_load_interrupt->GetKey());
	srr1.template Set<SRR1_CLEARED_FIELDS>(0);
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
	
	// clear some MSR bits
	msr.template Set<typename DataTLBMissOnLoadInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = DataTLBMissOnLoadInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	
	this->template AckInterrupt<DataTLBMissOnLoadInterrupt>();
	
	data_tlb_miss_on_load_interrupt->Clear();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(DataTLBMissOnStoreInterrupt *data_tlb_miss_on_store_interrupt)
{
	// save CIA
	srr0 = this->cia;

	// Set TLBMISS
	tlbmiss = data_tlb_miss_on_store_interrupt->GetTLBMISS();
	
	// Set PTEHI
	ptehi = data_tlb_miss_on_store_interrupt->GetPTEHI();
	
	// SRR1[0-5]=0, SRR1[6]=MSR[6], SRR1[7-10]=0, SRR1[11]=C SRR1[12]=key, SRR1[13-15]=0, SRR1[16-31]=MSR[16-31]
	struct _0_5 : Field<_0_5, 0, 5> {};
	struct _6 : Field<_6, 6> {};
	struct _7_10 : Field<_7_10, 7, 10> {};
	struct _13_15 : Field<_13_15, 13, 15> {};
	struct _16_31 : Field<_16_31, 16, 31> {};
	struct SRR1_CLEARED_FIELDS : FieldSet<_0_5, _7_10, _13_15> {};
	struct MSR_SAVED_FIELDS : FieldSet<_6, _16_31> {};
	srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
	srr1.template Set<typename SRR1::_11>(data_tlb_miss_on_store_interrupt->GetC());
	srr1.template Set<typename SRR1::_12>(data_tlb_miss_on_store_interrupt->GetKey());
	srr1.template Set<SRR1_CLEARED_FIELDS>(0);
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
	
	// clear some MSR bits
	msr.template Set<typename DataTLBMissOnStoreInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = DataTLBMissOnStoreInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	
	this->template AckInterrupt<DataTLBMissOnStoreInterrupt>();
	
	data_tlb_miss_on_store_interrupt->Clear();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(InstructionAddressBreakpointInterrupt *instruction_address_breakpoint_interrupt)
{
	// save CIA
	srr0 = this->cia;
	
	// SRR1[1-4]=0, SRR1[10-15]=0, SRR1[16-23]=MSR[16-23], SRR1[25-27]=MSR[25-27], SRR1[30-31]=MSR[30-31]
	struct _1_4 : Field<_1_4, 1, 4> {};
	struct _10_15 : Field<_10_15, 10, 15> {};
	struct _16_23 : Field<_16_23, 16, 23> {};
	struct _25_27 : Field<_25_27, 25, 27> {};
	struct _30_31 : Field<_30_31, 30, 31> {};
	struct SRR1_CLEARED_FIELDS : FieldSet<_1_4, _10_15> {};
	struct MSR_SAVED_FIELDS : FieldSet<_16_23, _25_27, _30_31> {};
	srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
	srr1.template Set<SRR1_CLEARED_FIELDS>(0);
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
	
	// clear some MSR bits
	msr.template Set<typename InstructionAddressBreakpointInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = InstructionAddressBreakpointInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	
	this->template AckInterrupt<InstructionAddressBreakpointInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(SystemManagementInterrupt *system_management_interrupt)
{
	// save CIA
	srr0 = this->cia;
	
	// SRR1[1-4]=0, SRR1[10-15]=0, SRR1[16-23]=MSR[16-23], SRR1[25-27]=MSR[25-27], SRR1[30-31]=MSR[30-31]
	struct _1_4 : Field<_1_4, 1, 4> {};
	struct _10_15 : Field<_10_15, 10, 15> {};
	struct _16_23 : Field<_16_23, 16, 23> {};
	struct _25_27 : Field<_25_27, 25, 27> {};
	struct _30_31 : Field<_30_31, 30, 31> {};
	struct SRR1_CLEARED_FIELDS : FieldSet<_1_4, _10_15> {};
	struct MSR_SAVED_FIELDS : FieldSet<_16_23, _25_27, _30_31> {};
	srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
	srr1.template Set<SRR1_CLEARED_FIELDS>(0);
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
	
	// clear some MSR bits
	msr.template Set<typename SystemManagementInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = SystemManagementInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	
	this->template AckInterrupt<SystemManagementInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(AltivecAssistInterrupt *altivec_assist_interrupt)
{
	// save CIA
	srr0 = this->cia;
	
	// SRR1[1-4]=0, SRR1[10-15]=0, SRR1[16-23]=MSR[16-23], SRR1[25-27]=MSR[25-27], SRR1[30-31]=MSR[30-31]
	struct _1_4 : Field<_1_4, 1, 4> {};
	struct _10_15 : Field<_10_15, 10, 15> {};
	struct _16_23 : Field<_16_23, 16, 23> {};
	struct _25_27 : Field<_25_27, 25, 27> {};
	struct _30_31 : Field<_30_31, 30, 31> {};
	struct SRR1_CLEARED_FIELDS : FieldSet<_1_4, _10_15> {};
	struct MSR_SAVED_FIELDS : FieldSet<_16_23, _25_27, _30_31> {};
	srr1.template Set<MSR_SAVED_FIELDS>(msr.template Get<MSR_SAVED_FIELDS>());
	srr1.template Set<SRR1_CLEARED_FIELDS>(0);
	
	// MSR[LE]=MSR[ILE]
	msr.template Set<typename MSR::LE>(msr.template Get<typename MSR::ILE>());
	
	// clear some MSR bits
	msr.template Set<typename AltivecAssistInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	// Branch to interrupt handler
	this->cia = AltivecAssistInterrupt::OFFSET | (msr.template Get<typename MSR::IP>() ? 0xfff00000UL : 0x00000000UL);
	
	this->template AckInterrupt<AltivecAssistInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::UpdateExceptionEnable()
{
	this->SuperCPU::template EnableException<typename MachineCheckInterrupt::MCP>(msr.template Get<typename MSR::ME>() && hid1.template Get<typename HID1::EMCP>());
	this->SuperCPU::template EnableException<typename MachineCheckInterrupt::TEA>(msr.template Get<typename MSR::ME>());
	this->SuperCPU::template EnableInterrupt<ExternalInterrupt>(msr.template Get<typename MSR::EE>());
	this->SuperCPU::template EnableInterrupt<DecrementerInterrupt>(msr.template Get<typename MSR::EE>());

	this->Synchronize(); // Force resampling input signals
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::UpdatePowerSave()
{
	powersave = msr.template Get<typename MSR::POW>() && hid0.template Get<typename HID0::NAP>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::PowerManagement()
{
	if(unlikely(powersave))
	{
		this->Idle();
	}
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
	
#if BYTE_ORDER == LITTLE_ENDIAN
	unisim::util::endian::BSwap(insn);
#endif

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
bool CPU<TYPES, CONFIG>::Stwcx(unsigned int rs, EFFECTIVE_ADDRESS addr)
{
	// TODO: reservation logic
	// Note: Address match with prior lwarx not required for store to be performed
	if(!this->Int32Store(rs, addr)) return false;
	// Clear CR0[LT][GT], setCR0[EQ] and copy XER[SO] to CR0[SO]
	this->cr.template Set<typename CR::CR0::LT>(0);
	this->cr.template Set<typename CR::CR0::GT>(0);
	this->cr.template Set<typename CR::CR0::EQ>(1);
	this->cr.template Set<typename CR::CR0::SO>(this->xer.template Get<typename XER::SO>());
	
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Sync()
{
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Isync()
{
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::TLB_Sync()
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
	
	struct B0_29 : Field<B0_29, 0, 29> {};

	this->Branch(srr0 & B0_29::template GetMask<EFFECTIVE_ADDRESS>());
	struct _16_23 : Field<_16_23, 16, 23> {};
	struct _25_27 : Field<_25_27, 25, 27> {};
	struct _30_31 : Field<_30_31, 30, 31> {};
	struct MSR_RESTORED_FIELDS : FieldSet<_16_23, _25_27, _30_31> {};
	msr.template Set<MSR_RESTORED_FIELDS>(srr1.template Get<MSR_RESTORED_FIELDS>());
	
	if(unlikely(this->verbose_interrupt))
	{
		this->logger << DebugInfo << "instruction #" << this->instruction_counter << ":Returning from interrupt to 0x" << std::hex << this->nia << std::dec << EndDebugInfo;
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
	
	if(unlikely(base_addr != instruction_buffer_base_addr))
	{
		EFFECTIVE_ADDRESS size_to_protection_boundary;
		ADDRESS base_virt_addr;
		PHYSICAL_ADDRESS base_phys_addr;
		STORAGE_ATTR storage_attr;
		
		if(unlikely((!this->template Translate</* debug */ false, /* exec */ true, /* write */ false>(base_addr, size_to_protection_boundary, base_virt_addr, base_phys_addr, storage_attr)))) return false;
		
		if(unlikely(!this->SuperMSS::InstructionFetch(base_virt_addr, base_phys_addr, instruction_buffer, CONFIG::INSTRUCTION_BUFFER_SIZE, storage_attr))) return false;
		
#if BYTE_ORDER == LITTLE_ENDIAN
		unisim::util::endian::BSwap(instruction_buffer, CONFIG::INSTRUCTION_BUFFER_SIZE / 4);
#endif
		
		instruction_buffer_base_addr = base_addr;
	}
	else
	{
		// Account for level 1 cache access
		typename CONFIG::CPU::INSTRUCTION_CACHE_HIERARCHY::L1CACHE *l1i = static_cast<typename CONFIG::CPU *>(this)->GetCache((typename CONFIG::CPU::INSTRUCTION_CACHE_HIERARCHY::L1CACHE *) 0);
		if(likely(l1i && l1i->IsEnabled()))
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
	this->PowerManagement();
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
				std::stringstream msg_sstr;
				msg_sstr << "Instruction counter reached " << this->instruction_counter;
				this->trap_reporting_import->ReportTrap(*this, msg_sstr.str());
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
void CPU<TYPES, CONFIG>::RunTimers(uint64_t delta)
{
	tb = uint64_t(tb) + delta;
	dec = uint32_t(dec) - delta;
}

template <typename TYPES, typename CONFIG>
uint64_t CPU<TYPES, CONFIG>::GetMaxIdleTime() const
{
	return msr.template Get<typename MSR::EE>() ? (uint64_t(dec) + 1) : 0xffffffffffffffffULL;
}

template <typename TYPES, typename CONFIG>
uint64_t CPU<TYPES, CONFIG>::GetTimersDeadline() const
{
	return uint64_t(dec) + 1;
}

} // end of namespace e600
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E600_CPU_TCC__
