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

#ifndef __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_CONFIGREGS_HH__
#define __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_CONFIGREGS_HH__

#include <string>
#include <inttypes.h>
#include "unisim/util/endian/endian.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {

using std::string;
using unisim::util::endian::LittleEndian2Host;
using unisim::util::endian::Host2LittleEndian;

class ConfigurationRegister {
public:
	static const unsigned int ReadAccess = 1;
	static const unsigned int WriteAccess = 2;
	static const unsigned int BitResetAccess = 4;
	static const unsigned int ByteAccess = 1;
	static const unsigned int HalfWordAccess = 2;
	static const unsigned int WordAccess = 4;

	string name;
	string long_name;
	uint32_t base_address;
	uint32_t byte_size;
	unsigned int permission;
	unsigned int access_size;
	uint32_t value;
	uint32_t mask;

	ConfigurationRegister();
	
	ConfigurationRegister(const char *_name,
		const char *_long_name,
		uint32_t _base_address,
		uint32_t _byte_size,
		unsigned int _permission,
		unsigned int _access_size,
		uint32_t _value,
		uint32_t _mask = 0xffffffffUL);
			
	void Set(const char *_name, 
			 const char *_long_name,
			 uint32_t _base_address,
			 uint32_t _byte_size,
			 unsigned int _permission,
			 unsigned int _access_size,
			 uint32_t _value,
			 uint32_t _mask = 0xffffffffUL);
	
	bool AllowedSize(uint32_t address, unsigned int req_size);
	/* IMPORTANT: val must be in host endian and size 1,2 or 4 */
	bool Write(uint32_t address, uint32_t val);
};

class ConfigurationRegisters {
public:
	ConfigurationRegisters();
	~ConfigurationRegisters();

	bool Reset(bool pci_host,
		bool a_address_map,
		bool memory_32bit_data_bus_size,
		bool rom0_8bit_data_bus_size,
		bool rom1_8bit_data_bus_size);
	ConfigurationRegister *GetRegister(uint32_t addr) const;

	bool AddConfigurationRegisterToTable(ConfigurationRegister &reg);
	ConfigurationRegister vendor_id,
		device_id,
		pci_command_reg,
		pci_status_reg,
		revision_id,
		pir,
		subclass_code,
		pbccr,
		pclsr,
		pltr,
		header_type,
		bist_ctrl,
		lmbar,
		pcsrbar,
		bar2,
		bar3,
		bar4,
		bar5,
		subsys_vendor_id,
		subsys_id,
		exp_rom_base_addr,
		ilr,
		interrupt_pin,
		min_gnt,
		max_lat,
		bus_number,
		subor_bus_number,
		pacr,
		pmcr1,
		pmcr2,
		odcr,
		cdcr,
		miocr,
		eumbbar,
		mem_start_addr_reg1,
		mem_start_addr_reg2,
		ext_mem_start_addr_reg1,
		ext_mem_start_addr_reg2,
		mem_end_addr_reg1,
		mem_end_addr_reg2,
		ext_mem_end_addr_reg1,
		ext_mem_end_addr_reg2,
		mem_bank_enable_reg,
		mem_page_mode_reg,
		picr1,
		picr2,
		ecc_sb_err_counter,
		ecc_sb_err_trig_reg,
		errenr1,
		errdr1,
		besr,
		errenr2,
		errdr2,
		pci_besr,
		ear,
		ambor,
		mccr1,
		mccr2,
		mccr3,
		mccr4;
private:
	static const uint32_t MAX_REGS = 256;
	ConfigurationRegister *cfg_reg_table[MAX_REGS];
};

} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
	
#endif // __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_CONFIGREGS_HH__
