/*
 *  Copyright (c) 2007,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#include "unisim/component/cxx/chipset/mpc107/config_regs.hh"
#include <iostream>
#include <stdlib.h>

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {

using std::cerr;
using std::endl;
using std::hex;
using std::dec;

ConfigurationRegister::ConfigurationRegister() : 
	name(), long_name(), base_address(0), byte_size(0), 
	permission(ReadAccess), access_size(ByteAccess), value(0) {
}

ConfigurationRegister::ConfigurationRegister(const char *_name,
	const char *_long_name,
	uint32_t _base_address,
	uint32_t _byte_size,
	unsigned int _permission,
	unsigned int _access_size,
	uint32_t _value,
	uint32_t _mask) :
	name(_name), long_name(_long_name), 
	base_address(_base_address), byte_size(_byte_size), 
	permission(_permission), access_size(_access_size),
	value(_value), mask(_mask) {
}

void 
ConfigurationRegister::Set(const char *_name, 
	const char *_long_name,
	uint32_t _base_address,
	uint32_t _byte_size,
	unsigned int _permission,
	unsigned int _access_size,
	uint32_t _value,
	uint32_t _mask) {
	name = _name;
	long_name = _long_name;
	base_address = _base_address;
	byte_size = _byte_size;
	permission = _permission;
	access_size = _access_size;
	value = _value;
	mask = _mask;
}

bool
ConfigurationRegister::AllowedSize(uint32_t address, unsigned int req_size) {
	if(access_size & ByteAccess) return true; // no restriction
	return (access_size & req_size) == req_size && (address == base_address); // check access size and strict access alignment
}

/* IMPORTANT: val must be in host endian and size 1,2 or 4 */
bool 
ConfigurationRegister::Write(uint32_t address, uint32_t val) {
	value = (val & mask) | value;
	
	return true;
}
	
ConfigurationRegisters::ConfigurationRegisters() {
}

ConfigurationRegisters::~ConfigurationRegisters() {
}

bool
ConfigurationRegisters::Reset(bool pci_host,
	bool a_address_map,
	bool memory_32bit_data_bus_size,
	bool rom0_8bit_data_bus_size,
	bool rom1_8bit_data_bus_size) {
	typedef ConfigurationRegister CR;
	
	vendor_id.Set("vendor_id", "Vendor ID", 0x00, 2, 
		CR::ReadAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x1057,
		0x0000); // mask: the register is read only
	device_id.Set("device_id", "Device ID", 0x02, 2,
		CR::ReadAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x0004,
		0x0000); // mask: the register is read only
	pci_command_reg.Set("pci_command_register", "PCI command register", 0x04, 2,
		CR::ReadAccess + CR::WriteAccess,
		CR::HalfWordAccess,
		pci_host ? 0x0004 : 0x0000);
	pci_status_reg.Set("pci_status_reg", "PCI status register", 0x06, 2,
		CR::ReadAccess + CR::BitResetAccess,
		CR::HalfWordAccess,
		0x00a0);
	revision_id.Set("revision_id", "Revision ID", 0x08, 1,
		CR::ReadAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00,
		0x00); // mask: the register is read only
	pir.Set("pir", "Standard programming interface", 0x09, 1,
		CR::ReadAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		pci_host ? 0x00 : 0x01,
		0x00); // mask: the register is read only
	subclass_code.Set("subclass_code", "Subclass code", 0x0a, 1,
		CR::ReadAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00,
		0x00); // mask: the register is read only
	pbccr.Set("pbccr", "Base Class code", 0x0b, 1,
		CR::ReadAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		pci_host ? 0x06 : 0x0e,
		0x00); // mask: the register is read only
	pclsr.Set("pclsr", "Cache line size", 0x0c, 1,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00);
	pltr.Set("pltr", "Latency timer", 0x0d, 1,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00);
	header_type.Set("header_type", "Header type", 0x0e, 1,
		CR::ReadAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00,
		0x00); // mask: the register is read only
	bist_ctrl.Set("bis_ctrl", "BIST control", 0x0f, 1,
		CR::ReadAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00,
		0x00); // mask: the register is read only
	lmbar.Set("lmbar", "Local memory base address register", 0x10, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00000008);
	pcsrbar.Set("pcsrbar", "Peripheral control and status register base address register", 0x14, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00000000);
//start compatibility for pci
	bar2.Set("bar2", "3rd base address register", 0x18, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00000000);
	bar3.Set("bar3", "4th base address register", 0x1c, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00000000);
	bar4.Set("bar4", "5th base address register", 0x20, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00000000);
	bar5.Set("bar5", "6th base address register", 0x24, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00000000);
//end compatibility code
	subsys_vendor_id.Set("subsys_vendor_id", "Subsystem vendor id", 0x2c, 2,
		CR::ReadAccess,
		CR::WordAccess + CR::HalfWordAccess + CR::ByteAccess,
		0x00000000,
		0x00000000); // mask: the register is read only
	subsys_id.Set("subsys_id", "Subsystem id", 0x2e, 2,
		CR::ReadAccess,
		CR::WordAccess + CR::HalfWordAccess + CR::ByteAccess,
		0x00000000,
		0x00000000); // mask: the register is read only
	exp_rom_base_addr.Set("exp_rom_base_addr", "Expansion ROM base address", 0x30, 4,
		CR::ReadAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00000000,
		0x00000000); // mask: the register is read only
	ilr.Set("ilr", "Interrupt line", 0x3c, 1,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00);
	interrupt_pin.Set("interrupt_pin", "Interrupt pin", 0x3d, 1,
		CR::ReadAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x01,
		0x00); // mask: the register is read only
	min_gnt.Set("min_gnt", "MIN_GNT", 0x3e, 1,
		CR::ReadAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00,
		0x00); // mask: the register is read only
	max_lat.Set("max_lat", "MAX_LAT", 0x3f, 1,
		CR::ReadAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00,
		0x00); // mask: the reigster is read only
	bus_number.Set("bus_number", "Bus number", 0x40, 1,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00);
	subor_bus_number.Set("subor_bus_number", "Subordinate bus number", 0x41, 1,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00);
	pacr.Set("pacr", "PCI arbiter control register", 0x46, 2,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x0000);
	pmcr1.Set("pmcr1", "Power management configuration register 1", 0x70, 2,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x0000);
	pmcr2.Set("pmcr2", "Power management configuration register 2", 0x72, 1,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00);
	odcr.Set("odcr2", "Output driver control register", 0x73, 1,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0xff);
	cdcr.Set("cdcr", "CLK driver control register", 0x74, 2,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x0300);
	miocr.Set("miocr", "Miscellaneous driver control register", 0x76, 1,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0X00);
	eumbbar.Set("eumbbar", "Embedded utilities memory block base address register", 0x78, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00000000);
	mem_start_addr_reg1.Set("mem_start_addr_reg1", "Memory starting address register 1", 0x80, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00000000);
	mem_start_addr_reg2.Set("mem_start_addr_reg2", "Memory starting address register 2", 0x84, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00000000);
	ext_mem_start_addr_reg1.Set("ext_mem_start_addr_reg1", "Extended memory starting address register 1", 0x88, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00000000);
	ext_mem_start_addr_reg2.Set("ext_mem_start_addr_reg2", "Extended memory starting address register 2", 0x8c, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00000000);
	mem_end_addr_reg1.Set("mem_end_addr_reg1", "Memory ending address register 1", 0x90, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00000000);
	mem_end_addr_reg2.Set("mem_end_addr_reg2", "Memory ending address register 2", 0x94, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00000000);
	ext_mem_end_addr_reg1.Set("ext_mem_end_addr_reg1", "Extended memory ending address register 1", 0x98, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00000000);
	ext_mem_end_addr_reg2.Set("ext_mem_end_addr_reg2", "Extended memory ending address register 2", 0x9c, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00000000);
	mem_bank_enable_reg.Set("mem_bank_enable_reg", "Memory banck enable register", 0xa0, 1,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00);
	mem_page_mode_reg.Set("mem_page_mode_reg", "Page mode counter/timer register", 0xa3, 1,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00);
	picr1.Set("picr1", "Processor interface configuration 1", 0xa8, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0xff040010);
	picr2.Set("picr2", "Processor interface configuration 2", 0xac, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x000c000c);
	ecc_sb_err_counter.Set("ecc_sb_err_counter", "ECC single bit error counter", 0xb8, 1,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00);
	ecc_sb_err_trig_reg.Set("ecc_sb_err_trig_reg", "ECC single bit error trigger register", 0xb9, 1,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00);
	errenr1.Set("errenr1", "Error enabling register 1", 0xc0, 1,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x01);
	errdr1.Set("errdr1", "Error detection register 1", 0xc1, 1,
		CR::ReadAccess + CR::BitResetAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00);
	besr.Set("besr", "Processor bus error status register", 0xc3, 1,
		CR::ReadAccess + CR::BitResetAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00);
	errenr2.Set("errenr2", "Error enabling register 2", 0xc4, 1,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00);
	errdr2.Set("errdr2", "Error detection register 2", 0xc5, 1,
		CR::ReadAccess + CR::BitResetAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00);
	pci_besr.Set("pci_besr", "PCI bus error status register", 0xc7, 1,
		CR::ReadAccess + CR::BitResetAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00);
	ear.Set("ear", "Processor/PCI error address register", 0xc8, 4,
		CR::ReadAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0x00);
	ambor.Set("ambor", "Address map B options register", 0xe0, 1,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess,
		0xc0);
	
	mccr1.Set("mccr1", "MCCR1", 0xf0, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess, 
		0xff820000);
	mccr2.Set("mccr2", "MCCR2", 0xf4, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess, 
		0x00000000);
	mccr3.Set("mccr3", "MCCR3", 0xf8, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess, 
		0x00000000);
	mccr4.Set("mccr4", "MCCR4", 0xfc, 4,
		CR::ReadAccess + CR::WriteAccess,
		CR::ByteAccess + CR::HalfWordAccess + CR::WordAccess, 
		0x00100000);

	/* Configure data bus sizes of (S)DRAM and rom chips. Touches mccr1 and mccr4 */
	if(memory_32bit_data_bus_size) {
		mccr4.value &= ~(0x00010000);
		if(rom0_8bit_data_bus_size) {
			mccr1.value |= 0x00400000;
		} else {
			mccr1.value &= ~(0x00400000);
		}

		if(rom1_8bit_data_bus_size) {
			mccr1.value |= 0x00200000;
		} else {
			mccr1.value &= ~(0x00200000);
		}
	} else {
		mccr4.value |= 0x00010000;
		if(rom0_8bit_data_bus_size) {
			mccr1.value |= 0x00400000;
		} else {
			mccr1.value &= ~(0x00400000);
		}

		if(rom1_8bit_data_bus_size) {
			mccr1.value |= 0x00200000;
		} else {
			mccr1.value &= ~(0x00200000);
		}
	}

	// Set configuration register map
	for(unsigned int i = 0; i < MAX_REGS; i++)
		cfg_reg_table[i] = NULL;
	AddConfigurationRegisterToTable(vendor_id);
	AddConfigurationRegisterToTable(device_id);
	AddConfigurationRegisterToTable(pci_command_reg);
	AddConfigurationRegisterToTable(pci_status_reg);
	AddConfigurationRegisterToTable(revision_id);
	AddConfigurationRegisterToTable(pir);
	AddConfigurationRegisterToTable(subclass_code);
	AddConfigurationRegisterToTable(pbccr);
	AddConfigurationRegisterToTable(pclsr);
	AddConfigurationRegisterToTable(pltr);
	AddConfigurationRegisterToTable(header_type);
	AddConfigurationRegisterToTable(bist_ctrl);
	AddConfigurationRegisterToTable(lmbar);
	AddConfigurationRegisterToTable(pcsrbar);
	AddConfigurationRegisterToTable(bar2);
	AddConfigurationRegisterToTable(bar3);
	AddConfigurationRegisterToTable(bar4);
	AddConfigurationRegisterToTable(bar5);
	AddConfigurationRegisterToTable(exp_rom_base_addr);
	AddConfigurationRegisterToTable(ilr);
	AddConfigurationRegisterToTable(interrupt_pin);
	AddConfigurationRegisterToTable(min_gnt);
	AddConfigurationRegisterToTable(max_lat);
	AddConfigurationRegisterToTable(bus_number);
	AddConfigurationRegisterToTable(subor_bus_number);
	AddConfigurationRegisterToTable(pacr);
	AddConfigurationRegisterToTable(pmcr1);
	AddConfigurationRegisterToTable(pmcr2);
	AddConfigurationRegisterToTable(odcr);
	AddConfigurationRegisterToTable(cdcr);
	AddConfigurationRegisterToTable(miocr);
	AddConfigurationRegisterToTable(eumbbar);
	AddConfigurationRegisterToTable(mem_start_addr_reg1);
	AddConfigurationRegisterToTable(mem_start_addr_reg2);
	AddConfigurationRegisterToTable(ext_mem_start_addr_reg1);
	AddConfigurationRegisterToTable(ext_mem_start_addr_reg2);
	AddConfigurationRegisterToTable(mem_end_addr_reg1);
	AddConfigurationRegisterToTable(mem_end_addr_reg2);
	AddConfigurationRegisterToTable(ext_mem_end_addr_reg1);
	AddConfigurationRegisterToTable(ext_mem_end_addr_reg2);
	AddConfigurationRegisterToTable(mem_bank_enable_reg);
	AddConfigurationRegisterToTable(mem_page_mode_reg);
	AddConfigurationRegisterToTable(picr1);
	AddConfigurationRegisterToTable(picr2);
	AddConfigurationRegisterToTable(ecc_sb_err_counter);
	AddConfigurationRegisterToTable(ecc_sb_err_trig_reg);
	AddConfigurationRegisterToTable(errenr1);
	AddConfigurationRegisterToTable(errdr1);
	AddConfigurationRegisterToTable(besr);
	AddConfigurationRegisterToTable(errenr2);
	AddConfigurationRegisterToTable(errdr2);
	AddConfigurationRegisterToTable(pci_besr);
	AddConfigurationRegisterToTable(ear);
	AddConfigurationRegisterToTable(ambor);
	AddConfigurationRegisterToTable(mccr1);
	AddConfigurationRegisterToTable(mccr2);
	AddConfigurationRegisterToTable(mccr3);
	AddConfigurationRegisterToTable(mccr4);

	return true;
}

bool
ConfigurationRegisters::AddConfigurationRegisterToTable(ConfigurationRegister &reg) {
	for(unsigned int i = 0; i < reg.byte_size; i++) {
		if(cfg_reg_table[i + reg.base_address] != NULL) {
			cerr << "ERROR(" 
				 << __FUNCTION__ << ":"
				 << __FILE__ << ":"
				 << __LINE__ << "): "
				 << "overwritting configuration register table entry" << endl;
			cerr << "  Conflict between: " << cfg_reg_table[i + reg.base_address]->name
				 << ", " << reg.name << endl;
			return false;
		}
		cfg_reg_table[i + reg.base_address] = &reg;
	}
	return true;
}

ConfigurationRegister *
ConfigurationRegisters::GetRegister(uint32_t addr) const {
	if(addr > 255) {
		cerr << "ERROR("
			<< __FUNCTION__ << ":"
			<< __FILE__ << ":"
			<< __LINE__ << "): "
			<< "Accessing an unknown configuration register (addr = " 
			<< hex << addr << dec << ")" << endl;
		return NULL;
	}

	return cfg_reg_table[addr];
}

} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
