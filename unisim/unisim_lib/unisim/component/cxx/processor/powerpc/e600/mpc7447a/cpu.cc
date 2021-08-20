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

#include <unisim/component/cxx/processor/powerpc/e600/mpc7447a/cpu.hh>
#include <unisim/component/cxx/processor/powerpc/e600/cpu.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e600 {
namespace mpc7447a {

CPU::CPU(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent, "MPC7447A PowerPC core")
	, SuperCPU(name, parent)
	, itlb_http_server_export("itlb-http-server-export", this)
	, dtlb_http_server_export("dtlb-http-server-export", this)
	, mmu(this)
	, l1i(this)
	, l1d(this)
	, l2u(this)
{
	itlb_http_server_export >> mmu.itlb_http_server_export;
	dtlb_http_server_export >> mmu.dtlb_http_server_export;
}

CPU::~CPU()
{
}

bool CPU::Dcba(EFFECTIVE_ADDRESS ea)
{
	return Dcbz(ea);
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
	
	return GlobalWriteBackLineByAddress<DATA_CACHE_HIERARCHY, L1D, L2U, /* invalidate */ true>(virt_addr, phys_addr); // line is invalidated
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
	
	GlobalInvalidateLineByAddress<DATA_CACHE_HIERARCHY, L1D, L2U>(virt_addr, phys_addr);
	
	return true;
}

bool CPU::Dcbst(EFFECTIVE_ADDRESS ea)
{
	EFFECTIVE_ADDRESS size_to_protection_boundary;
	ADDRESS virt_addr;
	PHYSICAL_ADDRESS phys_addr;
	STORAGE_ATTR storage_attr;
	if(unlikely((!Translate</* debug */ false, /* exec */ false, /* write */ false>(ea, size_to_protection_boundary, virt_addr, phys_addr, storage_attr)))) return false;
	
	if(storage_attr & TYPES::SA_W) return true; // no-op if target address is mapped as write-through
	
	return GlobalWriteBackLineByAddress<DATA_CACHE_HIERARCHY, L1D, L2U, /* invalidate */ false>(virt_addr, phys_addr); // line is not invalidated
}

bool CPU::Dcbz(EFFECTIVE_ADDRESS ea)
{
	if(l1d.IsPresent() && !l1d.IsEnabled())
	{
		ThrowException<AlignmentInterrupt::Alignment>().SetAddress(ea);
		return false;
	}
	
	EFFECTIVE_ADDRESS size_to_protection_boundary;
	ADDRESS virt_addr;
	PHYSICAL_ADDRESS phys_addr;
	STORAGE_ATTR storage_attr;
	if(unlikely((!Translate</* debug */ false, /* exec */ false, /* write */ true>(ea, size_to_protection_boundary, virt_addr, phys_addr, storage_attr)))) return false;
	
	if((storage_attr & (TYPES::SA_W | TYPES::SA_I))) // Write-through or Cache inhibited
	{
		ThrowException<AlignmentInterrupt::Alignment>().SetAddress(ea);
		return false;
	}
	
	if(!l1d.IsEnabled())
	{
		uint8_t zero[32] = {};
		return this->SuperMSS::DataStore(virt_addr & -32, phys_addr & -32, &zero[0], 32, storage_attr);
	}
	return AllocateBlockInCacheHierarchy<DATA_CACHE_HIERARCHY, L1D, /* zeroing */ true>(virt_addr, phys_addr, storage_attr);
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

bool CPU::Icbt(EFFECTIVE_ADDRESS ea)
{
	// Nothing to do: no architecturally visible changed 
	return true;
}

bool CPU::Tlbia()
{
	ThrowException<ProgramInterrupt::IllegalInstruction>();
	return false;
}

bool CPU::Tlbie(EFFECTIVE_ADDRESS ea)
{
	if(msr.template Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	mmu.template InvalidateTLBSetByEffectiveAddress</* exec */ false>(ea);
	mmu.template InvalidateTLBSetByEffectiveAddress</* exec */ true>(ea);
	
	return true;
}

bool CPU::Tlbld(EFFECTIVE_ADDRESS ea)
{
	if(msr.template Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	mmu.template LoadTLBEntryByEffectiveAddress</* exec */ false>(ea);
	
	return true;
}

bool CPU::Tlbli(EFFECTIVE_ADDRESS ea)
{
	if(msr.template Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}

	mmu.template LoadTLBEntryByEffectiveAddress</* exec */ true>(ea);
	
	return false;
}

} // end of namespace mpc7447a
} // end of namespace e600
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
