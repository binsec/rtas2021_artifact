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

#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_XBAR_SMPU_SMPU_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_XBAR_SMPU_SMPU_TCC__

#include <unisim/component/tlm2/interconnect/freescale/mpc57xx/xbar/smpu/smpu.hh>
#include <unisim/kernel/tlm2/trans_attr.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace freescale {
namespace mpc57xx {
namespace xbar {
namespace smpu {

using unisim::component::tlm2::interconnect::generic_router::MemoryAccess;
using unisim::component::tlm2::interconnect::generic_router::MEM_ACCESS_NONE;
using unisim::component::tlm2::interconnect::generic_router::MEM_ACCESS_READ;
using unisim::component::tlm2::interconnect::generic_router::MEM_ACCESS_WRITE;

template <typename CONFIG>
SMPU<CONFIG>::SMPU(const char *name, XBAR *_xbar)
	: unisim::kernel::Object(name, _xbar)
	, unisim::kernel::Service<unisim::service::interfaces::Registers>(name, _xbar)
	, registers_export("registers-export", this)
	, xbar(_xbar)
	, logger(*this)
	, smpu_cesr0(this)
	, smpu_cesr1(this)
	, smpu_ear(this)
	, smpu_edr(this)
	, smpu_rgd(this)
	, registers_registry()
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
{
	std::stringstream description_sstr;
	description_sstr << "MPC57XX Crossbar Switch (SMPU):" << std::endl;
	description_sstr << "  - " << NUM_REGION_DESCRIPTORS << " region descriptors" << std::endl;
	description_sstr << "  - " << NUM_BUS_MASTERS << " bus masters" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 20" << std::endl;
	this->SetDescription(description_sstr.str().c_str());

	xbar->MapRegister(XBAR::SMPU_PERIPHERAL_SLAVE_IF, smpu_cesr0.ADDRESS_OFFSET, &smpu_cesr0);
	xbar->MapRegister(XBAR::SMPU_PERIPHERAL_SLAVE_IF, smpu_cesr1.ADDRESS_OFFSET, &smpu_cesr1);
	xbar->MapRegisterFile(XBAR::SMPU_PERIPHERAL_SLAVE_IF, SMPU_EAR::ADDRESS_OFFSET, &smpu_ear, /* size */ 4, /* stride */ 8);
	xbar->MapRegisterFile(XBAR::SMPU_PERIPHERAL_SLAVE_IF, SMPU_EDR::ADDRESS_OFFSET, &smpu_edr, /* size */ 4, /* stride */ 8);
	
	unsigned int region_descriptor_num;
	for(region_descriptor_num = 0; region_descriptor_num < NUM_REGION_DESCRIPTORS; region_descriptor_num++)
	{
		xbar->MapRegister(XBAR::SMPU_PERIPHERAL_SLAVE_IF, RegionDescriptorFile::ADDRESS_OFFSET + SMPU_RGD_WORD0::ADDRESS_OFFSET      + (RegionDescriptor::SIZE * region_descriptor_num), &smpu_rgd[region_descriptor_num].smpu_rgd_word0);
		xbar->MapRegister(XBAR::SMPU_PERIPHERAL_SLAVE_IF, RegionDescriptorFile::ADDRESS_OFFSET + SMPU_RGD_WORD1::ADDRESS_OFFSET      + (RegionDescriptor::SIZE * region_descriptor_num), &smpu_rgd[region_descriptor_num].smpu_rgd_word1);
		xbar->MapRegister(XBAR::SMPU_PERIPHERAL_SLAVE_IF, RegionDescriptorFile::ADDRESS_OFFSET + SMPU_RGD_WORD2_FMT0::ADDRESS_OFFSET + (RegionDescriptor::SIZE * region_descriptor_num), &smpu_rgd[region_descriptor_num].smpu_rgd_word2_fmt0);
		xbar->MapRegister(XBAR::SMPU_PERIPHERAL_SLAVE_IF, RegionDescriptorFile::ADDRESS_OFFSET + SMPU_RGD_WORD3::ADDRESS_OFFSET      + (RegionDescriptor::SIZE * region_descriptor_num), &smpu_rgd[region_descriptor_num].smpu_rgd_word3);
		
		registers_registry.AddRegisterInterface(smpu_rgd[region_descriptor_num].smpu_rgd_word0.CreateRegisterInterface());
		registers_registry.AddRegisterInterface(smpu_rgd[region_descriptor_num].smpu_rgd_word1.CreateRegisterInterface());
		registers_registry.AddRegisterInterface(smpu_rgd[region_descriptor_num].smpu_rgd_word2_fmt0.CreateRegisterInterface());
		registers_registry.AddRegisterInterface(smpu_rgd[region_descriptor_num].smpu_rgd_word3.CreateRegisterInterface());
	}
}

template <typename CONFIG>
SMPU<CONFIG>::~SMPU()
{
}

template <typename CONFIG>
void SMPU<CONFIG>::end_of_elaboration()
{
	this->logger << DebugInfo << this->GetDescription() << EndDebugInfo;
}

template <typename CONFIG>
void SMPU<CONFIG>::Reset()
{
	unsigned int bus_master_num;
	unsigned int region_descriptor_num;
	
	smpu_cesr0.Reset();
	smpu_cesr1.Reset();
	
	for(bus_master_num = 0; bus_master_num < NUM_BUS_MASTERS; bus_master_num++)
	{
		smpu_ear[bus_master_num].Reset();
		smpu_edr[bus_master_num].Reset();
	}
	
	for(region_descriptor_num = 0; region_descriptor_num < NUM_REGION_DESCRIPTORS; region_descriptor_num++)
	{
		smpu_rgd[region_descriptor_num].Reset();
	}
}

//////////////// unisim::service::interface::Registers ////////////////////

template <typename CONFIG>
unisim::service::interfaces::Register *SMPU<CONFIG>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <typename CONFIG>
void SMPU<CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

template <typename CONFIG>
bool SMPU<CONFIG>::Check(transaction_type& trans, MemoryRegion *mem_rgn)
{
	if(!smpu_cesr0.template Get<typename SMPU_CESR0::GVLD>()) return true; // SMPU is disabled. All accesses from all bus masters are allowed. Leave memory region protection unmodified.
	
	unisim::kernel::tlm2::tlm_trans_attr *trans_attr_ext = trans.template get_extension<unisim::kernel::tlm2::tlm_trans_attr>();
	
	if(likely(trans_attr_ext != 0))
	{
		unsigned int master_id = trans_attr_ext->master_id();
		sc_dt::uint64 addr = trans.get_address();
		bool is_read = trans.is_read();
		bool is_write = trans.is_write();
		
		uint32_t eacd = 0;
		
		for(RegionDescriptor *region_descriptor = smpu_rgd.mru_region_descriptor, *prev_region_descriptor = 0; region_descriptor != 0; prev_region_descriptor = region_descriptor, region_descriptor = region_descriptor->next)
		{
			bool region_hit = region_descriptor->IsValid() &&
			                  (addr >= region_descriptor->GetStartAddress()) &&
			                  (addr <= region_descriptor->GetEndAddress());
			
			if(likely(region_hit))
			{
				bool not_error = (is_read && region_descriptor->IsReadAllowed(master_id)) ||
				                 (is_write && region_descriptor->IsWriteAllowed(master_id));
				
				if(likely(not_error))
				{
					if(unlikely(mem_rgn))
					{
						mem_rgn->start_addr = region_descriptor->GetStartAddress();
						mem_rgn->end_addr = region_descriptor->GetEndAddress();
						mem_rgn->mem_access = MemoryAccess((region_descriptor->IsReadAllowed(master_id) ? MEM_ACCESS_READ : MEM_ACCESS_NONE) | (region_descriptor->IsWriteAllowed(master_id) ? MEM_ACCESS_WRITE : MEM_ACCESS_NONE));
					}
					
					if(unlikely(region_descriptor != smpu_rgd.mru_region_descriptor))
					{
						prev_region_descriptor->next = region_descriptor->next;
						region_descriptor->next = smpu_rgd.mru_region_descriptor;
						smpu_rgd.mru_region_descriptor = region_descriptor;
					}
					return true;
				}
				else
				{
					eacd |= (1 << (SMPU_EDR::EACD::BITWIDTH - region_descriptor->region_descriptor_num)) & SMPU_EDR::EACD::template GetMask<uint32_t>();
				}
			}
		}
		
		if(eacd)
		{
			for(unsigned int region_descriptor_num = 0; region_descriptor_num < SMPU_EDR::EACD::BITWIDTH; region_descriptor_num++)
			{
				if(eacd & (1 << (SMPU_EDR::EACD::BITWIDTH - region_descriptor_num)))
				{
					this->logger << DebugWarning << "Master #" << master_id << " access has region protection violation in region #" << region_descriptor_num << EndDebugWarning;
				}
			}
		}
		else
		{
			this->logger << DebugWarning << "Master #" << master_id << " access does not hit in any region descriptor" << EndDebugWarning;
		}
		
		uint32_t merr = 1 << (SMPU_CESR0::MERR::BITWIDTH - master_id);
		
		bool captured_error = ((smpu_cesr0.template Get<typename SMPU_CESR0::MERR>() & merr) != 0);
		bool error_overrun = ((smpu_cesr1.template Get<typename SMPU_CESR1::MEOVR>() & merr) != 0);
		
		if(!captured_error || !error_overrun)
		{
			if(captured_error)
			{
				// error overrun
				smpu_cesr1.template Set<typename SMPU_CESR1::MEOVR>(smpu_cesr1.template Get<typename SMPU_CESR1::MEOVR>() | merr);
			}
			
			bool is_instruction = trans_attr_ext->is_instruction();
			bool is_privileged = trans_attr_ext->is_privileged();
			
			smpu_ear[master_id].template Set<typename SMPU_EAR::EADDR>(addr);
			smpu_edr[master_id].template Set<typename SMPU_EDR::EACD>(eacd);
			smpu_edr[master_id].template Set<typename SMPU_EDR::EATTR_PRIVILEGED>(is_privileged);
			smpu_edr[master_id].template Set<typename SMPU_EDR::EATTR_DATA_ACCESS>(!is_instruction);
			smpu_edr[master_id].template Set<typename SMPU_EDR::ERW>(is_write);
			smpu_edr[master_id].template Set<typename SMPU_EDR::EMN>(master_id);
		}
		
		trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
		return false;
	}
	
	return true;
}

template <typename CONFIG>
void SMPU<CONFIG>::DMI_Invalidate()
{
	xbar->DMI_Invalidate();
}

} // end of namespace smpu
} // end of namespace xbar
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_XBAR_SMPU_SMPU_TCC__
