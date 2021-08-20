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

#include <unisim/component/cxx/processor/powerpc/book_e/ppc440/cpu.hh>
#include <unisim/component/cxx/processor/powerpc/book_e/cpu.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace book_e {
namespace ppc440 {

CPU::CPU(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent, "PPC440 PowerPC core")
	, SuperCPU(name, parent)
	, itlb_http_server_export("itlb-http-server-export", this)
	, dtlb_http_server_export("dtlb-http-server-export", this)
	, utlb_http_server_export("utlb-http-server-export", this)
	, mmu(this)
	, l1i(this)
	, l1d(this)
{
	itlb_http_server_export >> mmu.itlb_http_server_export;
	dtlb_http_server_export >> mmu.dtlb_http_server_export;
	utlb_http_server_export >> mmu.utlb_http_server_export;
}

CPU::~CPU()
{
}

void CPU::ProcessInterrupt(CriticalInputInterrupt *critical_input_interrupt)
{
	mmu.InvalidateShadowTLBs();
	SuperCPU::ProcessInterrupt(critical_input_interrupt);
}

void CPU::ProcessInterrupt(MachineCheckInterrupt *machine_check_interrupt)
{
	mmu.InvalidateShadowTLBs();
	SuperCPU::ProcessInterrupt(machine_check_interrupt);
}

void CPU::ProcessInterrupt(DataStorageInterrupt *data_storage_interrupt)
{
	mmu.InvalidateShadowTLBs();
	SuperCPU::ProcessInterrupt(data_storage_interrupt);
}

void CPU::ProcessInterrupt(InstructionStorageInterrupt *instruction_storage_interrupt)
{
	mmu.InvalidateShadowTLBs();
	SuperCPU::ProcessInterrupt(instruction_storage_interrupt);
}

void CPU::ProcessInterrupt(ExternalInputInterrupt *external_input_interrupt)
{
	mmu.InvalidateShadowTLBs();
	SuperCPU::ProcessInterrupt(external_input_interrupt);
}

void CPU::ProcessInterrupt(AlignmentInterrupt *alignment_interrupt)
{
	mmu.InvalidateShadowTLBs();
	SuperCPU::ProcessInterrupt(alignment_interrupt);
}

void CPU::ProcessInterrupt(ProgramInterrupt *program_interrupt)
{
	mmu.InvalidateShadowTLBs();
	SuperCPU::ProcessInterrupt(program_interrupt);
	operation->update_esr_pcre(static_cast<typename CONFIG::CPU *>(this));
	operation->update_esr_pcmp(static_cast<typename CONFIG::CPU *>(this));
	operation->update_esr_pcrf(static_cast<typename CONFIG::CPU *>(this));
}

void CPU::ProcessInterrupt(FloatingPointUnavailableInterrupt *floating_point_unavailable_interrupt)
{
	mmu.InvalidateShadowTLBs();
	SuperCPU::ProcessInterrupt(floating_point_unavailable_interrupt);
}

void CPU::ProcessInterrupt(SystemCallInterrupt *system_call_interrupt)
{
	mmu.InvalidateShadowTLBs();
	SuperCPU::ProcessInterrupt(system_call_interrupt);
}

void CPU::ProcessInterrupt(AuxiliaryProcessorUnavailableInterrupt *auxiliary_processor_unavailable_interrupt)
{
	mmu.InvalidateShadowTLBs();
	SuperCPU::ProcessInterrupt(auxiliary_processor_unavailable_interrupt);
}

void CPU::ProcessInterrupt(DecrementerInterrupt *decrementer_interrupt)
{
	mmu.InvalidateShadowTLBs();
	SuperCPU::ProcessInterrupt(decrementer_interrupt);
}

void CPU::ProcessInterrupt(FixedIntervalTimerInterrupt *fixed_interval_timer_interrupt)
{
	mmu.InvalidateShadowTLBs();
	SuperCPU::ProcessInterrupt(fixed_interval_timer_interrupt);
}

void CPU::ProcessInterrupt(WatchdogTimerInterrupt *watchdog_interrupt)
{
	mmu.InvalidateShadowTLBs();
	SuperCPU::ProcessInterrupt(watchdog_interrupt);
}

void CPU::ProcessInterrupt(DataTLBErrorInterrupt *data_tlb_error_interrupt)
{
	mmu.InvalidateShadowTLBs();
	SuperCPU::ProcessInterrupt(data_tlb_error_interrupt);
}

void CPU::ProcessInterrupt(InstructionTLBErrorInterrupt *instruction_tlb_error_interrupt)
{
	mmu.InvalidateShadowTLBs();
	SuperCPU::ProcessInterrupt(instruction_tlb_error_interrupt);
}

void CPU::ProcessInterrupt(DebugInterrupt *debug_interrupt)
{
	mmu.InvalidateShadowTLBs();
	SuperCPU::ProcessInterrupt(debug_interrupt);
}

bool CPU::Rfi()
{
	mmu.InvalidateShadowTLBs();
	return SuperCPU::Rfi();
}

bool CPU::Rfci()
{
	mmu.InvalidateShadowTLBs();
	return SuperCPU::Rfci();
}

bool CPU::Rfmci()
{
	mmu.InvalidateShadowTLBs();
	return SuperCPU::Rfmci();
}

bool CPU::Dcbi(EFFECTIVE_ADDRESS ea)
{
	if(msr.Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	EFFECTIVE_ADDRESS size_to_protection_boundary;
	ADDRESS virt_addr;
	PHYSICAL_ADDRESS phys_addr;
	STORAGE_ATTR storage_attr;
	if(unlikely((!Translate</* debug */ false, /* exec */ false, /* write */ true>(ea, size_to_protection_boundary, virt_addr, phys_addr, storage_attr)))) return false;
	
	GlobalInvalidateLineByAddress<DATA_CACHE_HIERARCHY, L1D, L1D>(virt_addr, phys_addr);
	
	return true;
}

bool CPU::Dcba(EFFECTIVE_ADDRESS ea)
{
	// Dcba is treated as a no-op by PPC440x5
	return true;
}

bool CPU::Dcbf(EFFECTIVE_ADDRESS ea)
{
	if(msr.Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	EFFECTIVE_ADDRESS size_to_protection_boundary;
	ADDRESS virt_addr;
	PHYSICAL_ADDRESS phys_addr;
	STORAGE_ATTR storage_attr;
	if(unlikely((!Translate</* debug */ false, /* exec */ false, /* write */ false>(ea, size_to_protection_boundary, virt_addr, phys_addr, storage_attr)))) return false;
	
	return GlobalWriteBackLineByAddress<DATA_CACHE_HIERARCHY, L1D, L1D, /* invalidate */ true>(virt_addr, phys_addr); // line is invalidated
}


bool CPU::Dcbst(EFFECTIVE_ADDRESS ea)
{
	EFFECTIVE_ADDRESS size_to_protection_boundary;
	ADDRESS virt_addr;
	PHYSICAL_ADDRESS phys_addr;
	STORAGE_ATTR storage_attr;
	if(unlikely((!Translate</* debug */ false, /* exec */ false, /* write */ false>(ea, size_to_protection_boundary, virt_addr, phys_addr, storage_attr)))) return false;
	
	return GlobalWriteBackLineByAddress<DATA_CACHE_HIERARCHY, L1D, L1D, /* invalidate */ false>(virt_addr, phys_addr); // line is not invalidated
}

bool CPU::Dcbz(EFFECTIVE_ADDRESS ea)
{
	EFFECTIVE_ADDRESS size_to_protection_boundary;
	ADDRESS virt_addr;
	PHYSICAL_ADDRESS phys_addr;
	STORAGE_ATTR storage_attr;
	if(unlikely((!Translate</* debug */ false, /* exec */ false, /* write */ true>(ea, size_to_protection_boundary, virt_addr, phys_addr, storage_attr)))) return false;
	
	return AllocateBlockInCacheHierarchy<DATA_CACHE_HIERARCHY, L1D, /* zeroing */ true>(virt_addr, phys_addr, storage_attr);
}

bool CPU::Dccci(EFFECTIVE_ADDRESS addr)
{
	// Note: it's normal to ignore 'addr' as it's unused on PPC440
	if(msr.Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}

	InvalidateAll<L1D>();
	
	return true;
}

bool CPU::Dcread(EFFECTIVE_ADDRESS ea, unsigned int rd)
{
	uint32_t way = ea / L1D::BLOCK_SIZE;
	uint32_t offset = (ea % L1D::BLOCK_SIZE) & 0xfffffffcUL;
	uint32_t index = 0;
	uint32_t sector = 0;
	
	unisim::util::cache::CacheSet<TYPES, CONFIG::L1D_CONFIG>& l1_set = l1d.GetSetByIndex(index);
	unisim::util::cache::CacheLine<TYPES, CONFIG::L1D_CONFIG>& l1_line = l1_set.GetLineByWay(way);
	unisim::util::cache::CacheBlock<TYPES, CONFIG::L1D_CONFIG>& l1_block = l1_line.GetBlockBySector(sector);
	
	uint32_t data;
	l1_block.Read(&data, offset, sizeof(data));
	
	gpr[rd] = data;
	
	TYPES::PHYSICAL_ADDRESS base_phys_addr = l1_block.GetBasePhysicalAddress();

	dcdbtrh.Set<DCDBTRH::TRA>(base_phys_addr >> 8);
	dcdbtrh.Set<DCDBTRH::V>(l1_block.IsValid());
	dcdbtrh.Set<DCDBTRH::TERA>(base_phys_addr >> 32);
	
	dcdbtrl.Set<DCDBTRL::D>(l1_block.IsDirty());
	dcdbtrl.Set<DCDBTRL::U0>((l1_line.Status().storage_attr & TYPES::SA_U0) != 0);
	dcdbtrl.Set<DCDBTRL::U1>((l1_line.Status().storage_attr & TYPES::SA_U1) != 0);
	dcdbtrl.Set<DCDBTRL::U2>((l1_line.Status().storage_attr & TYPES::SA_U2) != 0);
	dcdbtrl.Set<DCDBTRL::U3>((l1_line.Status().storage_attr & TYPES::SA_U3) != 0);
	
	return true;
}

bool CPU::Icbi(EFFECTIVE_ADDRESS ea)
{
	EFFECTIVE_ADDRESS size_to_protection_boundary;
	ADDRESS virt_addr;
	PHYSICAL_ADDRESS phys_addr;
	STORAGE_ATTR storage_attr;
	if(unlikely((!Translate</* debug */ false, /* exec */ false, /* write */ false>(ea, size_to_protection_boundary, virt_addr, phys_addr, storage_attr)))) return false;
	
	decoder.InvalidateDecodingCacheEntry(ea);
	
	GlobalInvalidateLineByAddress<INSTRUCTION_CACHE_HIERARCHY, L1I, L1I>(virt_addr, phys_addr);
	
	return true;
}

bool CPU::Icbt(EFFECTIVE_ADDRESS addr)
{
	// Nothing to do: no architecturally visible changed 
	return true;
}

bool CPU::Iccci(EFFECTIVE_ADDRESS addr)
{
	// Note: it's normal to ignore 'addr' as it's unused on PPC440
	if(msr.Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}

	InvalidateAll<L1I>();
	
	return true;
}

bool CPU::Icread(EFFECTIVE_ADDRESS ea)
{
	uint32_t way = (ea / (L1I::SIZE / L1I::ASSOCIATIVITY)) % L1I::ASSOCIATIVITY;
	uint32_t offset = (ea % L1I::BLOCK_SIZE) & 0xfffffffcUL;
	uint32_t index = (ea / L1I::BLOCK_SIZE) % (L1I::SIZE / L1I::BLOCK_SIZE / L1I::BLOCKS_PER_LINE / L1I::ASSOCIATIVITY);
	uint32_t sector = 0;
	
	unisim::util::cache::CacheSet<TYPES, CONFIG::L1I_CONFIG>& l1_set = l1i.GetSetByIndex(index);
	unisim::util::cache::CacheLine<TYPES, CONFIG::L1I_CONFIG>& l1_line = l1_set.GetLineByWay(way);
	unisim::util::cache::CacheBlock<TYPES, CONFIG::L1I_CONFIG>& l1_block = l1_line.GetBlockBySector(sector);
	
	uint32_t data;
	l1_block.Read(&data, offset, sizeof(data));
	
	icdbdr = data;
	
	ADDRESS base_virtual_addr = l1_block.GetBaseAddress();
	EFFECTIVE_ADDRESS block_ea = EFFECTIVE_ADDRESS(TYPES::VA::EA::template Get<ADDRESS>(base_virtual_addr));
	PROCESS_ID pid = PROCESS_ID(TYPES::VA::PID::template Get<ADDRESS>(base_virtual_addr));
	ADDRESS_SPACE as = ADDRESS_SPACE(TYPES::VA::AS::template Get<ADDRESS>(base_virtual_addr));
	
	icdbtrh.template Set<typename ICDBTRH::TEA>(block_ea >> 8);
	icdbtrh.template Set<typename ICDBTRH::V>(l1_block.IsValid());
	icdbtrh.template Set<typename ICDBTRH::TPAR>(0);
	icdbtrh.template Set<typename ICDBTRH::DAPAR>(0);
	
	icdbtrl.template Set<typename ICDBTRL::TS>(as);
	icdbtrl.template Set<typename ICDBTRL::TD>(pid == 0);
	icdbtrl.template Set<typename ICDBTRL::TID>(pid);
	
	return true;
}

bool CPU::Tlbre(unsigned int rd, uint32_t way, uint8_t ws)
{
	if(msr.Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	if(ws > 2)
	{
		logger << DebugWarning << "At 0x" << std::hex << GetCIA() << std::dec << ", operand \'ws\' of instruction tlbre is > 2" << EndDebugWarning;
		return true;
	}
	
	const TLB_ENTRY<TYPES>& tlb_entry = mmu.LookupByWay(way);
	
	switch(ws)
	{
		case 0:
			gpr[rd] = tlb_entry.GetWord0(ccr0.Get<CCR0::CRPE>());
			mmucr.Set<MMUCR::STID>(tlb_entry.GetTID());
			break;
		case 1:
			gpr[rd] = tlb_entry.GetWord1(ccr0.Get<CCR0::CRPE>());
			break;
		case 2:
			gpr[rd] = tlb_entry.GetWord2(ccr0.Get<CCR0::CRPE>());
			break;
	}
	
	return true;
}

bool CPU::Tlbsx(unsigned int rd, typename TYPES::EFFECTIVE_ADDRESS ea, unsigned int rc)
{
	TYPES::ADDRESS_SPACE as = TYPES::ADDRESS_SPACE(mmucr.Get<MMUCR::STS>());
	TYPES::PROCESS_ID pid = TYPES::PROCESS_ID(mmucr.Get<MMUCR::STID>());
	TLB_ENTRY<TYPES> *tlb_entry = mmu.Lookup</* debug */ false>(as, pid, ea);

	if(likely(tlb_entry))
	{
		gpr[rd] = mmu.GetWay(tlb_entry);
		if(unlikely(rc))
		{
			// Generate CR0: CR0[SO]=XER[SO] CR0[LT]=0 CR0[GT]=0 CR0[EQ]=1
			cr.Set<CR::CR0::SO>(xer.Get<XER::SO>());
			cr.Set<CR::CR0::LT>(0);
			cr.Set<CR::CR0::GT>(0);
			cr.Set<CR::CR0::EQ>(1);
		}
	}
	else
	{
		// (rd) is undefined
		if(unlikely(rc))
		{
			// Generate CR0: CR0[SO]=XER[SO] CR0[LT]=0 CR0[GT]=0 CR0[EQ]=0
			cr.Set<CR::CR0::SO>(xer.Get<XER::SO>());
			cr.Set<CR::CR0::LT>(0);
			cr.Set<CR::CR0::GT>(0);
			cr.Set<CR::CR0::EQ>(0);
		}
	}
	
	return true;
}

bool CPU::Tlbwe(uint32_t s, uint32_t way, uint8_t ws)
{
	if(msr.Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	if(ws > 2)
	{
		logger << DebugWarning << "At 0x" << std::hex << GetCIA() << std::dec << ", operand \'ws\' of instruction tlbwe is > 2" << EndDebugWarning;
		return true;
	}

	TLB_ENTRY<TYPES>& tlb_entry = mmu.LookupByWay(way);

	switch(ws)
	{
		case 0:
			tlb_entry.SetWord0(s);
			tlb_entry.SetTID(mmucr.Get<MMUCR::STID>());
			break;
		case 1:
			tlb_entry.SetWord1(s);
			break;
		case 2:
			tlb_entry.SetWord2(s);
			break;
	}
	
// 	std::cerr << "#" << instruction_counter << ":tlbwe way #" << +way << ", word #" << +ws << ":" << std::endl << mmu << std::endl;
	
	return true;
}

} // end of namespace ppc440
} // end of namespace book_e
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
