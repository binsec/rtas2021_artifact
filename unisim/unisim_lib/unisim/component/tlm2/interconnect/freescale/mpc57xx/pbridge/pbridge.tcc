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

#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_PBRIDGE_PBRIDGE_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_PBRIDGE_PBRIDGE_TCC__

#include <unisim/component/tlm2/interconnect/freescale/mpc57xx/pbridge/pbridge.hh>
#include <unisim/component/tlm2/interconnect/programmable_router/router.tcc>
#include <unisim/kernel/tlm2/trans_attr.hh>
#include <unisim/util/likely/likely.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace freescale {
namespace mpc57xx {
namespace pbridge {

using unisim::component::tlm2::interconnect::generic_router::MEM_ACCESS_NONE;
using unisim::component::tlm2::interconnect::generic_router::MEM_ACCESS_READ;
using unisim::component::tlm2::interconnect::generic_router::MEM_ACCESS_WRITE;
using unisim::component::tlm2::interconnect::generic_router::MEM_ACCESS_READ_WRITE;

template <typename CONFIG>
PBRIDGE<CONFIG>::PBRIDGE(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, Super(name, parent)
	, unisim::kernel::Service<unisim::service::interfaces::Registers>(name, parent)
	, registers_export("registers-export", this)
	, pbridge_mpra(this)
	, pbridge_mprb(this)
	, pbridge_pacra(this)
	, pbridge_pacrb(this)
	, pbridge_pacrc(this)
	, pbridge_pacrd(this)
	, pbridge_pacre(this)
	, pbridge_pacrf(this)
	, pbridge_pacrg(this)
	, pbridge_pacrh(this)
	, pbridge_opacra(this)
	, pbridge_opacrb(this)
	, pbridge_opacrc(this)
	, pbridge_opacrd(this)
	, pbridge_opacre(this)
	, pbridge_opacrf(this)
	, pbridge_opacrg(this)
	, pbridge_opacrh(this)
	, pbridge_opacri(this)
	, pbridge_opacrj(this)
	, pbridge_opacrk(this)
	, pbridge_opacrl(this)
	, pbridge_opacrm(this)
	, pbridge_opacrn(this)
	, pbridge_opacro(this)
	, pbridge_opacrp(this)
	, pbridge_opacrq(this)
	, pbridge_opacrr(this)
	, pbridge_opacrs(this)
	, pbridge_opacrt(this)
	, pbridge_opacru(this)
	, pbridge_opacrv(this)
	, pbridge_opacrw(this)
	, pbridge_opacrx(this)
	, pbridge_opacry(this)
	, pbridge_opacrz(this)
	, pbridge_opacraa(this)
	, pbridge_opacrab(this)
	, pbridge_opacrac(this)
	, pbridge_opacrad(this)
	, pbridge_opacrae(this)
	, pbridge_opacraf(this)
	, registers_registry()
	, acr_mapping()
	, param_acr_mapping()
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
{
	std::stringstream description_sstr;
	description_sstr << "MPC57XX Peripheral Bridge (PBRIDGE):" << std::endl;
	description_sstr << "  - " << CONFIG::INPUT_SOCKETS << " Master ports" << std::endl;
	description_sstr << "  - " << CONFIG::INPUT_BUSWIDTH << "-bit master data bus" << std::endl;
	description_sstr << "  - " << CONFIG::OUTPUT_SOCKETS << " Slave ports" << std::endl;
	description_sstr << "  - " << CONFIG::OUTPUT_BUSWIDTH << "-bit slave data bus" << std::endl;
	description_sstr << "  - " << CONFIG::PERIPHERAL_BUSWIDTH << "-bit peripheral data bus" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 19" << std::endl;
	this->SetDescription(description_sstr.str().c_str());
	
	for(unsigned int i = 0; i < CONFIG::NUM_MAPPINGS; i++)
	{
		std::stringstream sstr_name;
		sstr_name << "acr_mapping_" << i;
		std::stringstream sstr_desc;
		sstr_desc << "access control register for output port #" << i;
		param_acr_mapping[i] = new unisim::kernel::variable::Parameter<AccessControlRegisterMapping>(sstr_name.str().c_str(), this, acr_mapping[i], sstr_desc.str().c_str());
		
		param_acr_mapping[i]->SetMutable(false);
	}
	
	this->MapRegister(pbridge_mpra.ADDRESS_OFFSET,    &pbridge_mpra);
	this->MapRegister(pbridge_mprb.ADDRESS_OFFSET,    &pbridge_mprb);
	this->MapRegister(pbridge_pacra.ADDRESS_OFFSET,   &pbridge_pacra);
	this->MapRegister(pbridge_pacrb.ADDRESS_OFFSET,   &pbridge_pacrb);
	this->MapRegister(pbridge_pacrc.ADDRESS_OFFSET,   &pbridge_pacrc);
	this->MapRegister(pbridge_pacrd.ADDRESS_OFFSET,   &pbridge_pacrd);
	this->MapRegister(pbridge_pacre.ADDRESS_OFFSET,   &pbridge_pacre);
	this->MapRegister(pbridge_pacrf.ADDRESS_OFFSET,   &pbridge_pacrf);
	this->MapRegister(pbridge_pacrg.ADDRESS_OFFSET,   &pbridge_pacrg);
	this->MapRegister(pbridge_pacrh.ADDRESS_OFFSET,   &pbridge_pacrh);
	this->MapRegister(pbridge_opacra.ADDRESS_OFFSET,  &pbridge_opacra);
	this->MapRegister(pbridge_opacrb.ADDRESS_OFFSET,  &pbridge_opacrb);
	this->MapRegister(pbridge_opacrc.ADDRESS_OFFSET,  &pbridge_opacrc);
	this->MapRegister(pbridge_opacrd.ADDRESS_OFFSET,  &pbridge_opacrd);
	this->MapRegister(pbridge_opacre.ADDRESS_OFFSET,  &pbridge_opacre);
	this->MapRegister(pbridge_opacrf.ADDRESS_OFFSET,  &pbridge_opacrf);
	this->MapRegister(pbridge_opacrg.ADDRESS_OFFSET,  &pbridge_opacrg);
	this->MapRegister(pbridge_opacrh.ADDRESS_OFFSET,  &pbridge_opacrh);
	this->MapRegister(pbridge_opacri.ADDRESS_OFFSET,  &pbridge_opacri);
	this->MapRegister(pbridge_opacrj.ADDRESS_OFFSET,  &pbridge_opacrj);
	this->MapRegister(pbridge_opacrk.ADDRESS_OFFSET,  &pbridge_opacrk);
	this->MapRegister(pbridge_opacrl.ADDRESS_OFFSET,  &pbridge_opacrl);
	this->MapRegister(pbridge_opacrm.ADDRESS_OFFSET,  &pbridge_opacrm);
	this->MapRegister(pbridge_opacrn.ADDRESS_OFFSET,  &pbridge_opacrn);
	this->MapRegister(pbridge_opacro.ADDRESS_OFFSET,  &pbridge_opacro);
	this->MapRegister(pbridge_opacrp.ADDRESS_OFFSET,  &pbridge_opacrp);
	this->MapRegister(pbridge_opacrq.ADDRESS_OFFSET,  &pbridge_opacrq);
	this->MapRegister(pbridge_opacrr.ADDRESS_OFFSET,  &pbridge_opacrr);
	this->MapRegister(pbridge_opacrs.ADDRESS_OFFSET,  &pbridge_opacrs);
	this->MapRegister(pbridge_opacrt.ADDRESS_OFFSET,  &pbridge_opacrt);
	this->MapRegister(pbridge_opacru.ADDRESS_OFFSET,  &pbridge_opacru);
	this->MapRegister(pbridge_opacrv.ADDRESS_OFFSET,  &pbridge_opacrv);
	this->MapRegister(pbridge_opacrw.ADDRESS_OFFSET,  &pbridge_opacrw);
	this->MapRegister(pbridge_opacrx.ADDRESS_OFFSET,  &pbridge_opacrx);
	this->MapRegister(pbridge_opacry.ADDRESS_OFFSET,  &pbridge_opacry);
	this->MapRegister(pbridge_opacrz.ADDRESS_OFFSET,  &pbridge_opacrz);
	this->MapRegister(pbridge_opacraa.ADDRESS_OFFSET, &pbridge_opacraa);
	this->MapRegister(pbridge_opacrab.ADDRESS_OFFSET, &pbridge_opacrab);
	this->MapRegister(pbridge_opacrac.ADDRESS_OFFSET, &pbridge_opacrac);
	this->MapRegister(pbridge_opacrad.ADDRESS_OFFSET, &pbridge_opacrad);
	this->MapRegister(pbridge_opacrae.ADDRESS_OFFSET, &pbridge_opacrae);
	this->MapRegister(pbridge_opacraf.ADDRESS_OFFSET, &pbridge_opacraf);
	
	registers_registry.AddRegisterInterface(pbridge_mpra.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_mprb.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_pacra.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_pacrb.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_pacrc.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_pacrd.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_pacre.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_pacrf.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_pacrg.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_pacrh.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacra.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrb.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrc.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrd.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacre.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrf.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrg.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrh.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacri.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrj.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrk.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrl.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrm.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrn.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacro.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrp.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrq.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrs.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrt.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacru.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrv.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrw.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrx.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacry.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrz.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacraa.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrab.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrac.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrad.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacrae.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(pbridge_opacraf.CreateRegisterInterface());
}

template <typename CONFIG>
PBRIDGE<CONFIG>::~PBRIDGE()
{
	unsigned int i;
	
	for(i = 0; i < CONFIG::NUM_MAPPINGS; i++)
	{
		delete param_acr_mapping[i];
	}
}

template <typename CONFIG>
void PBRIDGE<CONFIG>::end_of_elaboration()
{
	Super::end_of_elaboration();
	
	this->logger << DebugInfo << this->GetDescription() << EndDebugInfo;
	
	bool invalid_acr_mapping = false;
	for(unsigned int i = 0; i < CONFIG::NUM_MAPPINGS; i++)
	{
		AccessControlRegisterMapping& acr_m = acr_mapping[i];
		if(!acr_m.valid)
		{
			invalid_acr_mapping = true;
			this->logger << DebugError << "Parameter \"" <<  param_acr_mapping[i]->GetName() << "\" value is out-of-range" << EndDebugError;
		}
	}
	
	if(invalid_acr_mapping)
	{
		this->logger << DebugError << "Invalid ACR mapping configuration" << EndDebugError;
		unisim::kernel::Object::Stop(-1);
	}
}

template <typename CONFIG>
void PBRIDGE<CONFIG>::Reset()
{
	Super::Reset();

	pbridge_mpra.Reset();
	pbridge_mprb.Reset();
	pbridge_pacra.Reset();
	pbridge_pacrb.Reset();
	pbridge_pacrc.Reset();
	pbridge_pacrd.Reset();
	pbridge_pacre.Reset();
	pbridge_pacrf.Reset();
	pbridge_pacrg.Reset();
	pbridge_pacrh.Reset();
	pbridge_opacra.Reset();
	pbridge_opacrb.Reset();
	pbridge_opacrc.Reset();
	pbridge_opacrd.Reset();
	pbridge_opacre.Reset();
	pbridge_opacrf.Reset();
	pbridge_opacrg.Reset();
	pbridge_opacrh.Reset();
	pbridge_opacri.Reset();
	pbridge_opacrj.Reset();
	pbridge_opacrk.Reset();
	pbridge_opacrl.Reset();
	pbridge_opacrm.Reset();
	pbridge_opacrn.Reset();
	pbridge_opacro.Reset();
	pbridge_opacrp.Reset();
	pbridge_opacrq.Reset();
	pbridge_opacrr.Reset();
	pbridge_opacrs.Reset();
	pbridge_opacrt.Reset();
	pbridge_opacru.Reset();
	pbridge_opacrv.Reset();
	pbridge_opacrw.Reset();
	pbridge_opacrx.Reset();
	pbridge_opacry.Reset();
	pbridge_opacrz.Reset();
	pbridge_opacraa.Reset();
	pbridge_opacrab.Reset();
	pbridge_opacrac.Reset();
	pbridge_opacrad.Reset();
	pbridge_opacrae.Reset();
	pbridge_opacraf.Reset();
}

//////////////// unisim::service::interface::Registers ////////////////////

template <typename CONFIG>
unisim::service::interfaces::Register *PBRIDGE<CONFIG>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <typename CONFIG>
void PBRIDGE<CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

template <typename CONFIG>
unsigned int PBRIDGE<CONFIG>::GetMasterProtection(unsigned int master_id) const
{
	unsigned int pbridge_mpr_num = master_id / 8;
	unsigned int mprot_shift = 4 * (7 - (master_id % 8));
	
	uint32_t mpr = 0;
	switch(pbridge_mpr_num)
	{
		case 0: mpr = (uint32_t) pbridge_mpra; break;
		case 1: mpr = (uint32_t) pbridge_mprb; break;
	}
	
	return (mpr >> mprot_shift) & MPROT::ALL::template GetMask<unsigned int>();
}

template <typename CONFIG>
unsigned int PBRIDGE<CONFIG>::GetAccessControl(unsigned int mapping_id) const
{
	//std::cerr << "output_port=" << output_port << std::endl;
	const AccessControlRegisterMapping& _acr_mapping = acr_mapping[mapping_id];
	bool off_platform = _acr_mapping.off_platform;
	unsigned int acr_reg_num = _acr_mapping.reg_num;
	//std::cerr << (off_platform ? "o" : "") << "pacr" << acr_reg_num << std::endl;
	unsigned int reg_num = acr_reg_num / 8;
	//std::cerr << "reg_num=" << reg_num << std::endl;
	unsigned int acr_shift = 4 * (7 - (acr_reg_num % 8));
	//std::cerr << "acr_shift=" << acr_shift << std::endl;
	
	uint32_t pacr = 0;
	if(off_platform)
	{
		switch(reg_num)
		{
			case 0: pacr = (uint32_t) pbridge_opacra; break;
			case 1: pacr = (uint32_t) pbridge_opacrb; break;
			case 2: pacr = (uint32_t) pbridge_opacrc; break;
			case 3: pacr = (uint32_t) pbridge_opacrd; break;
			case 4: pacr = (uint32_t) pbridge_opacre; break;
			case 5: pacr = (uint32_t) pbridge_opacrf; break;
			case 6: pacr = (uint32_t) pbridge_opacrg; break;
			case 7: pacr = (uint32_t) pbridge_opacrh; break;
			case 8: pacr = (uint32_t) pbridge_opacri; break;
			case 9: pacr = (uint32_t) pbridge_opacrj; break;
			case 10: pacr = (uint32_t) pbridge_opacrk; break;
			case 11: pacr = (uint32_t) pbridge_opacrl; break;
			case 12: pacr = (uint32_t) pbridge_opacrm; break;
			case 13: pacr = (uint32_t) pbridge_opacrn; break;
			case 14: pacr = (uint32_t) pbridge_opacro; break;
			case 15: pacr = (uint32_t) pbridge_opacrp; break;
			case 16: pacr = (uint32_t) pbridge_opacrq; break;
			case 17: pacr = (uint32_t) pbridge_opacrr; break;
			case 18: pacr = (uint32_t) pbridge_opacrs; break;
			case 19: pacr = (uint32_t) pbridge_opacrt; break;
			case 20: pacr = (uint32_t) pbridge_opacru; break;
			case 21: pacr = (uint32_t) pbridge_opacrv; break;
			case 22: pacr = (uint32_t) pbridge_opacrw; break;
			case 23: pacr = (uint32_t) pbridge_opacrx; break;
			case 24: pacr = (uint32_t) pbridge_opacry; break;
			case 25: pacr = (uint32_t) pbridge_opacrz; break;
			case 26: pacr = (uint32_t) pbridge_opacraa; break;
			case 27: pacr = (uint32_t) pbridge_opacrab; break;
			case 28: pacr = (uint32_t) pbridge_opacrac; break;
			case 29: pacr = (uint32_t) pbridge_opacrad; break;
			case 30: pacr = (uint32_t) pbridge_opacrae; break;
			case 31: pacr = (uint32_t) pbridge_opacraf; break;
		}
	}
	else
	{
		switch(reg_num)
		{
			case 0: pacr = (uint32_t) pbridge_pacra; break;
			case 1: pacr = (uint32_t) pbridge_pacrb; break;
			case 2: pacr = (uint32_t) pbridge_pacrc; break;
			case 3: pacr = (uint32_t) pbridge_pacrd; break;
			case 4: pacr = (uint32_t) pbridge_pacre; break;
			case 5: pacr = (uint32_t) pbridge_pacrf; break;
			case 6: pacr = (uint32_t) pbridge_pacrg; break;
			case 7: pacr = (uint32_t) pbridge_pacrh; break;
		}
	}
	
	return (pacr >> acr_shift) & PACR::ALL::template GetMask<unsigned int>();
}

template <typename CONFIG>
bool PBRIDGE<CONFIG>::ApplyMap(transaction_type &trans, MAPPING const *&applied_mapping, MemoryRegion *mem_rgn)
{
	if(likely(Super::ApplyMap(trans, applied_mapping, mem_rgn)))
	{
		bool is_ignore = trans.get_command() == tlm::TLM_IGNORE_COMMAND;
		bool is_read = trans.is_read();
		bool is_write = trans.is_write();

		unisim::kernel::tlm2::tlm_trans_attr *trans_attr_ext = trans.template get_extension<unisim::kernel::tlm2::tlm_trans_attr>();
		
		if(likely(trans_attr_ext != 0))
		{
			unsigned int master_id = trans_attr_ext->master_id();
			unsigned int mapping_id = applied_mapping->id;
			
			if(likely(master_id < NUM_MASTER_IDS))
			{
				unsigned int mprot = GetMasterProtection(master_id);
				
				bool master_trusted_for_read = MPROT::MTR::Get(mprot);
				bool master_trusted_for_write = MPROT::MTW::Get(mprot);
				bool master_pivilege_level = MPROT::MPL::Get(mprot) && trans_attr_ext->is_privileged();

				unsigned int pacr = GetAccessControl(mapping_id);
				
				bool supervisor_protect = PACR::SP::Get(pacr);
				bool write_protect = PACR::WP::Get(pacr);
				bool trusted_protect = PACR::TP::Get(pacr);

				if(likely((!supervisor_protect || master_pivilege_level) &&
				          ((is_read && (!trusted_protect || master_trusted_for_read)) ||
				           (is_write && !write_protect && (!trusted_protect || master_trusted_for_write)) ||
				           unlikely(is_ignore))))
				{
					if(mem_rgn)
					{
						bool trusted_read = !trusted_protect || master_trusted_for_read;
						bool trusted_write = !trusted_protect || master_trusted_for_write;
						mem_rgn->mem_access = (is_ignore || (trusted_read && trusted_write)) ? MEM_ACCESS_READ_WRITE : (trusted_read ? MEM_ACCESS_READ : (trusted_write ? MEM_ACCESS_WRITE : MEM_ACCESS_NONE));
					}
					
					return true;
				}
				
				this->logger << DebugWarning << "Master #" << master_id << " can't access port #" << applied_mapping->output_port << " (" << this->output_socket_name[applied_mapping->output_port] << ") with mapping #" << mapping_id << " according " << acr_mapping[mapping_id] << EndDebugWarning;
				trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
			}
			else
			{
				this->logger << DebugError << "Master ID (" << master_id << ") out-of-range" << EndDebugError;
				trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
				unisim::kernel::Object::Stop(-1);
			}
			
			return false;
		}
		
		return true;
	}
	
	return false;
}

} // end of namespace pbridge
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_PBRIDGE_PBRIDGE_TCC__
