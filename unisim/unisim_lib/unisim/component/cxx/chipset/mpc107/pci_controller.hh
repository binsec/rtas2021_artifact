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
 
#ifndef __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_PCICONTROLLER_HH__
#define __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_PCICONTROLLER_HH__

#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/component/cxx/chipset/mpc107/config_regs.hh"
#include "unisim/component/cxx/chipset/mpc107/address_maps.hh"
#include "unisim/component/cxx/pci/types.hh"
#include "unisim/util/endian/endian.hh"
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {

using unisim::component::cxx::pci::TransactionType;
using unisim::component::cxx::pci::PCISpace;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::LittleEndian2Host;
using unisim::kernel::Object;
using unisim::kernel::Client;

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG = false>
class PCIController :
	virtual public Object {
public:

protected:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	Parameter<bool> param_verbose;
private:	
	enum {
		LMBAR = 0x10,
		ITWR = 0x2310,
		OMBAR = 0x2300,
		OTWR = 0x2308,
		PICR1 = 0xA8,
		PICR2 = 0xAC,
		MCCR1 = 0xF0,
		MCCR2 = 0xF4,
		MCCR3 = 0xF8,
		MCCR4 = 0xFC,
		EUMBBAR = 0x78,
		BAR_IO_MASK = 0x3,
		BAR_MEM_MASK = 0xF,
		PCI_COMMAND_REGISTER = 0x4,
		MEM_START_ADDR_REG1 = 0x80,
		MEM_START_ADDR_REG2 = 0x84,
		EXT_MEM_START_ADDR_REG1 = 0x88,
		EXT_MEM_START_ADDR_REG2 = 0x8C,
		MEM_END_ADDR_REG1 = 0x90,
		MEM_END_ADDR_REG2 = 0x94,
		EXT_MEM_END_ADDR_REG1 = 0x98,
		EXT_MEM_END_ADDR_REG2 = 0x9C,
		MEM_BANK_EN_REG = 0xA0,
		MEM_PAGE_MODE_REG = 0xA3,
	};

	/* To know which pci device I am*/
	int deviceNumber;

	/* CONFIG_ADDR register */
	PCI_BUS_PHYSICAL_ADDR config_addr;
	ConfigurationRegisters *config_regs;

	//Addr translation functions
	PCI_BUS_PHYSICAL_ADDR HostToPci(SYSTEM_BUS_PHYSICAL_ADDR original);
	SYSTEM_BUS_PHYSICAL_ADDR PciToHost(PCI_BUS_PHYSICAL_ADDR original);
	/* Address map */
	AddressMap<SYSTEM_BUS_PHYSICAL_ADDR, PCI_BUS_PHYSICAL_ADDR, DEBUG> *addr_map;

public:
	PCIController(int _deviceNumber, 
		ConfigurationRegisters &_config_regs, 
		AddressMap<SYSTEM_BUS_PHYSICAL_ADDR, PCI_BUS_PHYSICAL_ADDR, DEBUG> &_addr_map, 
		const char *name, 
		Object *parent = NULL);
	virtual bool BeginSetup();
	virtual void OnDisconnect();
	
	void TranslateAddressSystemToPCIMem(SYSTEM_BUS_PHYSICAL_ADDR addr,
			PCI_BUS_PHYSICAL_ADDR &translated_addr);
	void TranslateAddressSystemToPCIIO(SYSTEM_BUS_PHYSICAL_ADDR addr,
			PCI_BUS_PHYSICAL_ADDR &translated_addr);
	void TranslateAddressSystemToPCIConfig(SYSTEM_BUS_PHYSICAL_ADDR addr,
			PCI_BUS_PHYSICAL_ADDR &translated_addr);

	bool GetReadRequest(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
						uint32_t req_size,
						TransactionType &type,
						PCI_BUS_PHYSICAL_ADDR &addr,
						PCISpace &space,
						uint32_t &size);
	bool GetIOReadRequest(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
						uint32_t req_size,
						TransactionType &type,
						PCI_BUS_PHYSICAL_ADDR &addr,
						PCISpace &space,
						uint32_t &size);
	bool GetReadConfigRequest(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
						uint32_t req_size,
						TransactionType &type,
						PCI_BUS_PHYSICAL_ADDR &addr,
						PCISpace &space,
						uint32_t &size);
	bool LocalReadConfigRequest(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
						uint32_t req_size,
						uint8_t data[PCI_MAX_TRANSACTION_DATA_SIZE]);
	bool GetWriteRequest(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
						uint8_t req_data[SYSTEM_MAX_TRANSACTION_DATA_SIZE],
						uint32_t req_size,
						TransactionType &type,
						PCI_BUS_PHYSICAL_ADDR &addr,
						PCISpace &space,
						uint8_t data[PCI_MAX_TRANSACTION_DATA_SIZE],
						uint32_t &size);
	bool GetIOWriteRequest(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
						uint8_t req_data[SYSTEM_MAX_TRANSACTION_DATA_SIZE],
						uint32_t req_size,
						TransactionType &type,
						PCI_BUS_PHYSICAL_ADDR &addr,
						PCISpace &space,
						uint8_t data[PCI_MAX_TRANSACTION_DATA_SIZE],
						uint32_t &size);
	bool GetWriteConfigRequest(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
						uint8_t req_data[SYSTEM_MAX_TRANSACTION_DATA_SIZE],
						uint32_t req_size,
						TransactionType &type,
						PCI_BUS_PHYSICAL_ADDR &addr,
						PCISpace &space,
						uint8_t data[PCI_MAX_TRANSACTION_DATA_SIZE],
						uint32_t &size);
	bool LocalWriteConfigRequest(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
						uint8_t req_data[SYSTEM_MAX_TRANSACTION_DATA_SIZE],
						uint32_t req_size);

	bool SetConfigAddr(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
					uint8_t req_data[SYSTEM_MAX_TRANSACTION_DATA_SIZE],
					uint32_t req_size);
	bool IsLocalAccess();

private:
	bool WriteLocalConfigReg(int reg, uint32_t data);
	bool WriteEUMBBAR(int reg, uint32_t data);
	bool WritePCICommandRegister(int reg, uint32_t data);
	bool WritePICR1(int reg, uint32_t data);
	bool WritePICR2(int reg, uint32_t data);
	bool WriteMCCR1(int reg, uint32_t data);
	bool WriteMCCR2(int reg, uint32_t data);
	bool WriteMCCR3(int reg, uint32_t data);
	bool WriteMCCR4(int reg, uint32_t data);
	bool WriteMemStartAddrReg1(int reg, uint32_t data);
	bool WriteMemStartAddrReg2(int reg, uint32_t data);
	bool WriteExtMemStartAddrReg1(int reg, uint32_t data);
	bool WriteExtMemStartAddrReg2(int reg, uint32_t data);
	bool WriteMemEndAddrReg1(int reg, uint32_t data);
	bool WriteMemEndAddrReg2(int reg, uint32_t data);
	bool WriteExtMemEndAddrReg1(int reg, uint32_t data);
	bool WriteExtMemEndAddrReg2(int reg, uint32_t data);
	bool WriteMemBankEnableReg(int reg, uint32_t data);
	bool WriteMemPageModeReg(int reg, uint32_t data);
};

} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_PCICONTROLLER_HH__
