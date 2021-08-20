/*
 *  Copyright (c) 2008,
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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#include <unisim/component/cxx/processor/hcs12x/mmc.hh>

#include <stdio.h>
#include <assert.h>

namespace unisim {
namespace util {

template class Singleton <unisim::component::cxx::processor::hcs12x::MMC>;

}
}


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {


address_t MMC::MMC_REGS_ADDRESSES[MMC::MMC_MEMMAP_SIZE];

MMC::MMC(const char *name, S12MPU_IF *_mpu, Object *parent):
	Object(name, parent)
	, unisim::kernel::Service<Memory<physical_address_t> >(name, parent)
	, Client<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::Service<Registers>(name, parent)
	, memory_export("memory_export", this)
	, memory_import("memory_import", this)
	, registers_export("registers_export", this)

	, mpu(_mpu)

	, debug_enabled(false)
	, param_debug_enabled("debug-enabled", this, debug_enabled)
	, version("V3")
	, param_version("version", this, version, "MMC version. Supported are V3 and V4. Default is V3")

	, mmcctl0(MMCCTL0_RESET)
	, mode(MMC_MODE_RESET)
	, gpage(GLOBAL_RESET_PAGE)
	, direct(DIRECT_RESET_PAGE)
	, mmcctl1(mmcctl1_int)
	, rpage(RAM_RESET_PAGE)
	, epage(EEPROM_RESET_PAGE)
	, ppage(FLASH_RESET_PAGE)
	, ramwpc(RAMWPC_RESET)
	, ramxgu(RAMXGU_RESET)
	, ramshl(RAMSHL_RESET)
	, ramshu(RAMSHU_RESET)

	, directSet(false)

	, mode_int(MMC_MODE_RESET)
	, mmcctl1_int(MMCCTL1_RESET)
	, param_mode("mode", this, mode_int)
	, param_mmcctl1("mmcctl1", this, mmcctl1_int)
	, address_encoding(ADDRESS::BANKED)
	, param_address_encoding("address-encoding",this,address_encoding)
	, ppage_address(0x30)
	, param_ppage_address("ppage-address", this, ppage_address)


{

}


MMC::~MMC() {

	// Release registers_registry
	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

}


void MMC::Reset() {

	directSet = false;

	mmcctl0 = MMCCTL0_RESET;
	mode = mode_int;
	gpage = GLOBAL_RESET_PAGE;
	direct = DIRECT_RESET_PAGE;
	mmcctl1 = mmcctl1_int;

	rpage = RAM_RESET_PAGE;
	epage = EEPROM_RESET_PAGE;
	ppage = FLASH_RESET_PAGE;

	ramwpc = RAMWPC_RESET;
	ramxgu = RAMXGU_RESET;
	ramshl = RAMSHL_RESET;
	ramshu = RAMSHU_RESET;

}


void MMC::ResetMemory() {
	
	Reset();
	
}

bool MMC::BeginSetup() {


	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(GetName()) + ".MMCCTL0", &mmcctl0));

	unisim::kernel::variable::Register<uint8_t> *mmcctl0_var = new unisim::kernel::variable::Register<uint8_t>("MMCCTL0", this, mmcctl0, "MMC Control Register (MMCCTL0)");
	extended_registers_registry.push_back(mmcctl0_var);
	mmcctl0_var->setCallBack(this, MMCCTL0, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(GetName()) + ".MODE", &mode));

	unisim::kernel::variable::Register<uint8_t> *mode_var = new unisim::kernel::variable::Register<uint8_t>("MODE", this, mode, "Mode Register (MODE)");
	extended_registers_registry.push_back(mode_var);
	mode_var->setCallBack(this, MODE, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(GetName()) + ".GPAGE", &gpage));

	unisim::kernel::variable::Register<uint8_t> *gpage_var = new unisim::kernel::variable::Register<uint8_t>("GPAGE", this, gpage, "Global Page Index Register (GPAGE)");
	extended_registers_registry.push_back(gpage_var);
	gpage_var->setCallBack(this, GPAGE, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(GetName()) + ".DIRECT", &direct));

	unisim::kernel::variable::Register<uint8_t> *direct_var = new unisim::kernel::variable::Register<uint8_t>("DIRECT", this, direct, "Direct Page Register (DIRECT)");
	extended_registers_registry.push_back(direct_var);
	direct_var->setCallBack(this, DIRECT, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(GetName()) + ".MMCCTL1", &mmcctl1));

	unisim::kernel::variable::Register<uint8_t> *mmcctl1_var = new unisim::kernel::variable::Register<uint8_t>("MMCCTL1", this, mmcctl1, "MMC Control Register (MMCCTL1)");
	extended_registers_registry.push_back(mmcctl1_var);
	mmcctl1_var->setCallBack(this, MMCCTL1, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(GetName()) + ".RPAGE", &rpage));

	unisim::kernel::variable::Register<uint8_t> *rpage_var = new unisim::kernel::variable::Register<uint8_t>("RPAGE", this, rpage, "RAM Page Index Register (RPAGE)");
	extended_registers_registry.push_back(rpage_var);
	rpage_var->setCallBack(this, RPAGE, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(GetName()) + ".EPAGE", &epage));

	unisim::kernel::variable::Register<uint8_t> *epage_var = new unisim::kernel::variable::Register<uint8_t>("EPAGE", this, epage, "EEPROM Page Index Register (EPAGE)");
	extended_registers_registry.push_back(epage_var);
	epage_var->setCallBack(this, EPAGE, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(GetName()) + ".PPAGE", &ppage));

	unisim::kernel::variable::Register<uint8_t> *ppage_var = new unisim::kernel::variable::Register<uint8_t>("PPAGE", this, ppage, "Program Page Index Register (PPAGE)");
	extended_registers_registry.push_back(ppage_var);
	ppage_var->setCallBack(this, PPAGE, &CallBackObject::write, NULL);

	if (version.compare("V3") == 0) {
		registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(GetName()) + ".RAMWPC", &ramwpc));

		unisim::kernel::variable::Register<uint8_t> *ramwpc_var = new unisim::kernel::variable::Register<uint8_t>("RAMWPC", this, ramwpc, "RAM Write Protection Control Register (RAMWPC)");
		extended_registers_registry.push_back(ramwpc_var);
		ramwpc_var->setCallBack(this, RAMWPC, &CallBackObject::write, NULL);

		registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(GetName()) + ".RAMXGU", &ramxgu));

		unisim::kernel::variable::Register<uint8_t> *ramxgu_var = new unisim::kernel::variable::Register<uint8_t>("RAMXGU", this, ramxgu, "RAM XGATE Upper Boundary Register (RAMXGU)");
		extended_registers_registry.push_back(ramxgu_var);
		ramxgu_var->setCallBack(this, RAMXGU, &CallBackObject::write, NULL);

		registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(GetName()) + ".RAMSHL", &ramshl));

		unisim::kernel::variable::Register<uint8_t> *ramshl_var = new unisim::kernel::variable::Register<uint8_t>("RAMSHL", this, ramshl, "RAM Shared Region Lower Boundary Register (RAMSHL)");
		extended_registers_registry.push_back(ramshl_var);
		ramshl_var->setCallBack(this, RAMSHL, &CallBackObject::write, NULL);

		registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(GetName()) + ".RAMSHU", &ramshu));

		unisim::kernel::variable::Register<uint8_t> *ramshu_var = new unisim::kernel::variable::Register<uint8_t>("RAMSHU", this, ramshu, "RAM Shared Region Upper Boundary Register (RAMSHU)");
		extended_registers_registry.push_back(ramshu_var);
		ramshu_var->setCallBack(this, RAMSHU, &CallBackObject::write, NULL);

	}

	MMC_REGS_ADDRESSES[MMCCTL0] = 0x000A;
	MMC_REGS_ADDRESSES[MODE] = 0x000B;
	MMC_REGS_ADDRESSES[GPAGE] = 0x0010;
	MMC_REGS_ADDRESSES[DIRECT] = 0x0011;
	MMC_REGS_ADDRESSES[MMCCTL1] = 0x0013;
	MMC_REGS_ADDRESSES[RPAGE] = 0x0016;
	MMC_REGS_ADDRESSES[EPAGE] = 0x0017;
//	MMC_REGS_ADDRESSES[PPAGE] = 0x0030;
	MMC_REGS_ADDRESSES[PPAGE] = ppage_address;
	MMC_REGS_ADDRESSES[RAMWPC] = 0x011C;
	MMC_REGS_ADDRESSES[RAMXGU] = 0x011D;
	MMC_REGS_ADDRESSES[RAMSHL] = 0x011E;
	MMC_REGS_ADDRESSES[RAMSHU] = 0x011F;

	Reset();

	return (true);
}


bool MMC::Setup(ServiceExportBase *srv_export) {
	return (true);
}


bool MMC::EndSetup() {
	return (true);
}


Register* MMC::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

void MMC::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

void MMC::OnDisconnect() {
}


void MMC::splitPagedAddress(physical_address_t paged_addr, page_t &page, address_t &cpu_address)
{
	if (address_encoding == ADDRESS::GNUGCC)
	{
		if (paged_addr < 0x10000) {
			page = 0;
			cpu_address = (address_t) paged_addr;
		} else {
			page = (page_t) ((paged_addr - 0x10000) / 0x4000);
			cpu_address = (address_t) ((paged_addr - 0x10000) % 0x4000);
		}
	}
	else
	{
		if (address_encoding == ADDRESS::LINEAR) {
			page = (page_t) (paged_addr / 0x4000);
			cpu_address = (address_t) ((paged_addr % 0x4000) + 0x8000);
		} else { // BANKED
			page = (page_t) (paged_addr / 0x10000);
			cpu_address = (address_t) (paged_addr % 0x10000);
		}
	}

}


bool MMC::ReadMemory(physical_address_t paged_addr, void *buffer, uint32_t size) {

	page_t page;
	address_t cpu_address;
	physical_address_t addr;


	splitPagedAddress(paged_addr, page, cpu_address);
	addr = getCPU12XPhysicalAddress(cpu_address, ADDRESS::EXTENDED, false, true, page);

	if (addr <= REG_HIGH_OFFSET) {
		for (unsigned int i=0; i<MMC_MEMMAP_SIZE; i++) {
			if (MMC_REGS_ADDRESSES[i] == addr) {
				return (read(addr, buffer, 1));
			}
		}

	}


	if (memory_import) {
		return (memory_import->ReadMemory(addr, buffer, size));
	}

	return (false);
}


bool MMC::WriteMemory(physical_address_t paged_addr, const void *buffer, uint32_t size) {

	page_t page;
	address_t cpu_address;
	physical_address_t addr;

	if (size == 0) {
		return (true);
	}

	splitPagedAddress(paged_addr, page, cpu_address);
	addr = getCPU12XPhysicalAddress(cpu_address, ADDRESS::EXTENDED, false, true, page);

	if (addr <= REG_HIGH_OFFSET) {
		for (uint8_t i=0; i<MMC_MEMMAP_SIZE; i++) {
			if (MMC_REGS_ADDRESSES[i] == addr) {
				return (write(addr, buffer, 1));
			}
		}
	}

	if (memory_import) {
		return (memory_import->WriteMemory(addr, buffer, size));
	}

	return (false);
}

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

