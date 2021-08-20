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

#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_EBI_EBI_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_EBI_EBI_TCC__

#include <unisim/component/tlm2/interconnect/freescale/mpc57xx/ebi/ebi.hh>
#include <unisim/component/tlm2/interconnect/programmable_router/router.tcc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace freescale {
namespace mpc57xx {
namespace ebi {

using unisim::component::tlm2::interconnect::generic_router::MEM_ACCESS_READ_WRITE;

template <typename CONFIG>
EBI<CONFIG>::EBI(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, Super(name, parent)
	, unisim::kernel::Service<unisim::service::interfaces::Registers>(name, parent)
	, registers_export("registers-export", this)
	, ebi_mcr(this)
	, ebi_br(this)
	, ebi_or(this)
	, registers_registry()
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, mapping_cache()
	, temporary_mapping_keeper()
{
	std::stringstream description_sstr;
	description_sstr << "MPC57XX External Bus Interface (EBI):" << std::endl;
	description_sstr << "  - " << CONFIG::INPUT_BUSWIDTH << "-bit master data bus" << std::endl;
	description_sstr << "  - " << CONFIG::OUTPUT_SOCKETS << " bank(s) on external bus" << std::endl;
	description_sstr << "  - " << CONFIG::OUTPUT_BUSWIDTH << "-bit external data bus" << std::endl;
	description_sstr << "  - " << CONFIG::PERIPHERAL_BUSWIDTH << "-bit peripheral data bus" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 37" << std::endl;
	this->SetDescription(description_sstr.str().c_str());

	this->MapRegister(EBI_MCR::ADDRESS_OFFSET, &ebi_mcr);
	this->MapRegisterFile(EBI_BR::ADDRESS_OFFSET, &ebi_br, 4, EBI_BR::ADDRESS_STRIDE);
	this->MapRegisterFile(EBI_OR::ADDRESS_OFFSET, &ebi_or, 4, EBI_OR::ADDRESS_STRIDE);
	
	registers_registry.AddRegisterInterface(ebi_mcr.CreateRegisterInterface());
	for(unsigned int i = 0; i < CONFIG::OUTPUT_SOCKETS; i++)
	{
		registers_registry.AddRegisterInterface(ebi_br[i].CreateRegisterInterface());
		registers_registry.AddRegisterInterface(ebi_or[i].CreateRegisterInterface());
	}
}

template <typename CONFIG>
EBI<CONFIG>::~EBI()
{
}

template <typename CONFIG>
void EBI<CONFIG>::end_of_elaboration()
{
	Super::end_of_elaboration();
	
	this->logger << DebugInfo << this->GetDescription() << EndDebugInfo;
}

template <typename CONFIG>
void EBI<CONFIG>::Reset()
{
	unsigned int i;
	
	ebi_mcr.Reset();
	for(i = 0; i < CONFIG::OUTPUT_SOCKETS; i++)
	{
		ebi_br[i].Reset();
		ebi_or[i].Reset();
	}
	
	Super::Reset();
}

//////////////// unisim::service::interface::Registers ////////////////////

template <typename CONFIG>
unisim::service::interfaces::Register *EBI<CONFIG>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <typename CONFIG>
void EBI<CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

template <typename CONFIG>
bool EBI<CONFIG>::Match(unsigned int output_port, uint64_t addr, uint64_t& range_start, uint64_t& range_end, uint64_t& translation) const
{
	EBI_BR const& br = ebi_br[output_port];
	
	if(br.template Get<typename EBI_BR::V>()) // valid?
	{
		EBI_OR const& _or = ebi_or[output_port];
		
		uint64_t ba = br.template Get<typename EBI_BR::BA>();
		uint64_t am = _or.template Get<typename EBI_OR::AM>();
		uint64_t base_address = ba << (INPUT_ADDRESS_WIDTH - EBI_BR::BA::BITWIDTH);
		uint64_t address_mask = am << (INPUT_ADDRESS_WIDTH - EBI_OR::AM::BITWIDTH);
		uint64_t masked_address = addr & address_mask;
		uint64_t masked_base_address = base_address & address_mask;
		if(masked_address == masked_base_address)
		{
			range_start = addr & (~uint64_t(0) << (INPUT_ADDRESS_WIDTH - EBI_OR::AM::BITWIDTH));
			range_end = range_start | (~uint64_t(0) >> (64 - (INPUT_ADDRESS_WIDTH - EBI_OR::AM::BITWIDTH)));
			translation = (range_start - masked_base_address) % OutputAddressMask(output_port);
			return true;
		}
	}
	
	return false;
}

template <typename CONFIG>
unsigned int EBI<CONFIG>::OutputAddressWidth(unsigned int output_port) const
{

	EBI_BR const& br = ebi_br[output_port];
	EBI_OR const& _or = ebi_or[output_port];
	// Address / Write Enable Select: 1=drive WE[0:3] to upper address bits, i.e. 20-bit address, 0=24-bit address
	unsigned int output_address_width = _or.template Get<typename EBI_OR::AWE>() ? (OUTPUT_ADDRESS_WIDTH - 4) : OUTPUT_ADDRESS_WIDTH;
	if(_or.template Get<typename EBI_OR::APS>()) // Address by Port Size?
	{
		if(br.template Get<typename EBI_BR::PS>()) // Port Size
		{
			// 16-bit port size
			output_address_width *= 2;
		}
		else
		{
			// 32-bit port size
			output_address_width *= 4;
		}
	}
	
	return output_address_width;
}

template <typename CONFIG>
uint64_t EBI<CONFIG>::OutputAddressMask(unsigned int output_port) const
{
	return ~(~uint64_t(0) << OutputAddressWidth(output_port));
}

template <typename CONFIG>
bool EBI<CONFIG>::ApplyMap(uint64_t addr, uint32_t size, MAPPING const *&applied_mapping, MemoryRegion *mem_rgn)
{
	if(ebi_mcr.template Get<typename EBI_MCR::MDIS>()) return false; // Module disabled?
	
	if(likely(mapping_cache.ApplyMap(addr, size, applied_mapping)))
	{
		if(unlikely(mem_rgn))
		{
			mem_rgn->start_addr = applied_mapping->range_start;
			mem_rgn->end_addr = applied_mapping->range_end;
			mem_rgn->mem_access = MEM_ACCESS_READ_WRITE;
		}
		return true;
	}
	
	unsigned int output_port;
	for(output_port = 0; output_port < CONFIG::OUTPUT_SOCKETS; output_port++)
	{
		uint64_t range_start;
		uint64_t range_end;
		uint64_t translation;
		if(Match(output_port, addr, range_start, range_end, translation))
		{
			mapping_cache.Insert(range_start, range_end, translation, output_port);
			
			applied_mapping = &mapping_cache[output_port];
			
			if(unlikely(mem_rgn))
			{
				mem_rgn->start_addr = applied_mapping->range_start;
				mem_rgn->end_addr = applied_mapping->range_end;
				mem_rgn->mem_access = MEM_ACCESS_READ_WRITE;
			}
		
			return true;
		}
	}
	
	return false;
}

template <typename CONFIG>
void EBI<CONFIG>::ApplyMap(uint64_t addr, uint32_t size, std::vector<MAPPING const *> &port_mappings)
{
	if(ebi_mcr.template Get<typename EBI_MCR::MDIS>()) return; // Module disabled?

	temporary_mapping_keeper.Clear();
	
	while(size)
	{
		bool found = false;
		unsigned int output_port;
		for(output_port = 0; output_port < CONFIG::OUTPUT_SOCKETS; output_port++)
		{
			uint64_t range_start;
			uint64_t range_end;
			uint64_t translation;
			if(Match(output_port, addr, range_start, range_end, translation))
			{
				port_mappings.push_back(temporary_mapping_keeper.CreateMapping(range_start, range_end, translation, output_port));
				
				uint32_t sz = std::min(uint32_t((range_end - addr) + 1), size);
				size -= sz;
				addr += sz;
				found = true;
				break;
			}
		}
		
		if(!found)
		{
			addr++;
			size--;
		}
	}
}

} // end of namespace ebi
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_EBI_EBI_TCC__
