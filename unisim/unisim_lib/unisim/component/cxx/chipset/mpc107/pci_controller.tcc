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

#ifndef __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_PCICONTROLLER_TCC__
#define __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_PCICONTROLLER_TCC__

#include <unisim/util/likely/likely.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

#define LOCATION "In function " << __FUNCTION__ << "file " << __FILE__ << ", line #" << __LINE__ << ": "

template <class SYSTEM_BUS_PHYSICAL_ADDR,
	uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
	class PCI_BUS_PHYSICAL_ADDR, 
	uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
	SYSTEM_MAX_TRANSACTION_DATA_SIZE,
	PCI_BUS_PHYSICAL_ADDR, 
	PCI_MAX_TRANSACTION_DATA_SIZE,
	DEBUG>
::PCIController(int _deviceNumber, 
	ConfigurationRegisters &_config_regs, 
	AddressMap<SYSTEM_BUS_PHYSICAL_ADDR, PCI_BUS_PHYSICAL_ADDR, DEBUG> &_addr_map, 
	const char *name, 
	Object *parent):
	Object(name, parent, "MPC107 integrated PCI bus controller"),
	logger(*this),
	verbose(false),
	param_verbose("verbose", this, verbose, "enable/disable verbosity"),
	deviceNumber(_deviceNumber),
	config_regs(&_config_regs), 
	addr_map(&_addr_map){
}
	
template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::BeginSetup() {
	if (config_regs->picr1.value == 1) {
		if(unlikely(verbose))
			logger << DebugError << LOCATION
				<< "Little endian mode not supported" 
				<< std::endl << EndDebugError;
		return false;
	}		
	return true;
}
	
template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
void PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::OnDisconnect() {
	/* TODO */
}
	
template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
void 
PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG> ::
TranslateAddressSystemToPCIMem(SYSTEM_BUS_PHYSICAL_ADDR addr,
		PCI_BUS_PHYSICAL_ADDR &translated_addr) {
	if (config_regs->pci_command_reg.value & 0x0004) //we are in host mode
		translated_addr = addr;
	else
		translated_addr = HostToPci(addr);
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
void 
PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG> ::
TranslateAddressSystemToPCIIO(SYSTEM_BUS_PHYSICAL_ADDR addr,
		PCI_BUS_PHYSICAL_ADDR &translated_addr) {
	translated_addr = addr & 0x00FFFFFFUL;
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
void 
PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG> ::
TranslateAddressSystemToPCIConfig(SYSTEM_BUS_PHYSICAL_ADDR addr,
		PCI_BUS_PHYSICAL_ADDR &translated_addr) {
	translated_addr = config_addr + (addr & 0x03);
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool 
PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG> ::
GetReadRequest(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
				uint32_t req_size,
				TransactionType &type,
				PCI_BUS_PHYSICAL_ADDR &addr,
				PCISpace &space,
				uint32_t &size) {
	//normal read access

	type = unisim::component::cxx::pci::TT_READ;
	space = unisim::component::cxx::pci::SP_MEM;
	size = req_size;
	if (config_regs->pci_command_reg.value & 0x0004) //we are in host mode
		addr = req_addr;
	else
		addr = HostToPci(req_addr);
	return true;
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE, 
		DEBUG>
::GetIOReadRequest(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
		uint32_t req_size,
		TransactionType &type,
		PCI_BUS_PHYSICAL_ADDR &addr,
		PCISpace &space,
		uint32_t &size) {
	//io read access

	type = unisim::component::cxx::pci::TT_READ;
	space = unisim::component::cxx::pci::SP_IO;
	size = req_size;
	addr = req_addr & 0x00FFFFFFUL;
	return true;
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::GetReadConfigRequest(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
					uint32_t req_size,
					TransactionType &type,
					PCI_BUS_PHYSICAL_ADDR &addr,
					PCISpace &space,
					uint32_t &size) {		
	//config read access
	type = unisim::component::cxx::pci::TT_READ;
	addr = config_addr + (req_addr & 0x03);
	space = unisim::component::cxx::pci::SP_CONFIG;
	size = req_size;
	return true;
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::GetWriteRequest(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
				uint8_t req_data[SYSTEM_MAX_TRANSACTION_DATA_SIZE],
				uint32_t req_size,
				TransactionType &type,
				PCI_BUS_PHYSICAL_ADDR &addr,
				PCISpace &space,
				uint8_t data[PCI_MAX_TRANSACTION_DATA_SIZE],
				uint32_t &size) {
	//normal write 
	type = unisim::component::cxx::pci::TT_WRITE;
	space = unisim::component::cxx::pci::SP_MEM;
	size = req_size;
	memcpy(data, req_data, size);
	if (config_regs->pci_command_reg.value & 0x0004) //we are in host mode
		addr = req_addr;
	else
		addr = HostToPci(req_addr);

	return true;
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::GetIOWriteRequest(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
						uint8_t req_data[SYSTEM_MAX_TRANSACTION_DATA_SIZE],
						uint32_t req_size,
						TransactionType &type,
						PCI_BUS_PHYSICAL_ADDR &addr,
						PCISpace &space,
						uint8_t data[PCI_MAX_TRANSACTION_DATA_SIZE],
						uint32_t &size) {
	//io write 
	type = unisim::component::cxx::pci::TT_WRITE;
	space = unisim::component::cxx::pci::SP_IO;
	size = req_size;
	memcpy(data, req_data, size);
	addr = req_addr & 0X00FFFFFFUL;
	return true;
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::GetWriteConfigRequest(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
						uint8_t req_data[SYSTEM_MAX_TRANSACTION_DATA_SIZE],
						uint32_t req_size,
						TransactionType &type,
						PCI_BUS_PHYSICAL_ADDR &addr,
						PCISpace &space,
						uint8_t data[PCI_MAX_TRANSACTION_DATA_SIZE],
						uint32_t &size) {
	//config write
	type = unisim::component::cxx::pci::TT_WRITE;
	space = unisim::component::cxx::pci::SP_CONFIG;
	addr = config_addr + (req_addr & 0x03);
	size = req_size;
	memcpy(data, req_data, size);
	return true;
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::SetConfigAddr(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
			uint8_t req_data[SYSTEM_MAX_TRANSACTION_DATA_SIZE],
			uint32_t req_size) {

	bool result = true;
	//Checking the range of the config_addr register
	if (req_addr >= 0xFEC00000 && req_addr <= 0xFEDFFFFC) {
		//Checking the size
		if (req_size == 4) {
			//Checking the word alingment
			if ((req_addr & 0x03) == 0) {
				//config_addr = *(uint32_t *)req_data;
				memcpy(&config_addr, req_data, sizeof(uint32_t));
				config_addr = LittleEndian2Host(config_addr);
				// mask the two low bits of config_addr
				if(config_addr & 0x03) {
					if(unlikely(verbose))
						logger << DebugWarning << LOCATION
							<< "address written in the configuration address register is not 32bit aligned (0x"
							<< std::hex << config_addr << std::dec
							<< "), aligning it (0x" << std::hex 
							<< (config_addr & ~((PCI_BUS_PHYSICAL_ADDR)0x03)) << std::dec << ")"
							<< std::endl << EndDebugWarning;
					config_addr = config_addr & ~((PCI_BUS_PHYSICAL_ADDR)0x03);
				}
			}
		}
	} else {
		result = false;
	}
	return result;
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
PCI_BUS_PHYSICAL_ADDR PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::HostToPci(SYSTEM_BUS_PHYSICAL_ADDR original) {
	PCI_BUS_PHYSICAL_ADDR pciAddr = original;

    uint32_t ombar = config_regs->GetRegister(OMBAR)->value;
	uint32_t otwr = config_regs->GetRegister(OTWR)->value;
	int size = otwr & 0x11111; //The size is n and it means the window size is 2^n bytes
	SYSTEM_BUS_PHYSICAL_ADDR mask = (0xFFFFF000UL << (size - 12));
	uint32_t baseAddr = ombar & mask;
	
	if (original & baseAddr) {
		pciAddr = (original & 0xFFF) | (otwr & mask);
	}
	return pciAddr;
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
SYSTEM_BUS_PHYSICAL_ADDR PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::PciToHost(PCI_BUS_PHYSICAL_ADDR original) {
	SYSTEM_BUS_PHYSICAL_ADDR memAddr = original;

    uint32_t lmbar = config_regs->GetRegister(LMBAR)->value;
	uint32_t itwr = config_regs->GetRegister(ITWR)->value;
	int size = itwr & 0x11111; //The size is n and it means the window size is 2^n bytes
	PCI_BUS_PHYSICAL_ADDR mask = (0xFFFFF000UL << (size - 12));
	uint32_t baseAddr = lmbar & mask;
	
	if (original & baseAddr) {
		memAddr = (original & 0xFFF) | (itwr & mask);
	}
	return memAddr;
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::IsLocalAccess() {
	//check if it's for me
	int device = (config_addr >> 11) & 0x1F;
	return (deviceNumber == device);
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::LocalReadConfigRequest(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
						uint32_t req_size, 
						uint8_t data[SYSTEM_MAX_TRANSACTION_DATA_SIZE]) {
	ConfigurationRegister *creg;
	unsigned int reg = (config_addr & 0xFF) + (req_addr & 0x03);

	unsigned int i;
	for(i = 0; i < req_size;)
	{
		creg = config_regs->GetRegister(reg + i);
		if(creg)
		{
			if((creg->permission & unisim::component::cxx::chipset::mpc107::ConfigurationRegister::ReadAccess))
			{
				if(unlikely(DEBUG && verbose))
				{
					if(!creg->AllowedSize(reg, req_size))
					{
						logger << DebugWarning
							<< LOCATION
							<< "Read access at 0x" << std::hex << reg << std::dec << " to register " << creg->name << " with bad size (got " << req_size
							<< ", expected " << creg->access_size << ")"
							<< EndDebugWarning;
					}
				}
				if(unlikely(DEBUG && verbose))
				{
					logger << DebugInfo
						<< LOCATION
						<< "Read access to register " << creg->name
						<< " (contains value 0x" << std::hex << creg->value << std::dec << ")" << EndDebugInfo;
				}
				unsigned int j;
				for(j = (reg + i) - creg->base_address; i < req_size && j < creg->byte_size; i++, j++)
				{
					unsigned int shift = 8 * j;
					data[i] = (creg->value >> shift) & 0xff;
				}
			}
		}
		else
		{
			data[i] = 0xff;
			if(unlikely(verbose))
			{
				logger << DebugWarning << LOCATION
					<< "Trying to read unmapped register 0x"
					<< std::hex << (reg + i) << std::dec 
					<< std::endl << EndDebugWarning;
			}
			i++;
		}
	}
	if(unlikely(DEBUG && verbose))
	{
		logger << DebugInfo << LOCATION
			<< "Read access to register 0x" << std::hex << reg << std::dec
			<< " with value = " << std::hex;
		for(unsigned int i = 0; i < req_size; i++)
		{
			if(i != 0) logger << " ";
			logger << "0x" << (unsigned int) data[i];
		}
		logger << std::dec << std::endl << EndDebugInfo;
	}
	return true;
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::LocalWriteConfigRequest(SYSTEM_BUS_PHYSICAL_ADDR req_addr,
						uint8_t req_data[SYSTEM_MAX_TRANSACTION_DATA_SIZE],
						uint32_t req_size) {

	bool status = true;
	unsigned int reg = (config_addr & 0xFF) + (req_addr & 0x3); // does not need to be 4 byte aligned (maybe it should be aligned to the size)

	if(unlikely(DEBUG && verbose))
	{
		logger << DebugInfo << LOCATION
			<< "Write access to register 0x" << std::hex << reg << std::dec
			<< " with value = " << std::hex;
		for(unsigned int i = 0; i < req_size; i++)
		{
			if(i != 0) logger << " ";
			logger << "0x" << (unsigned int) req_data[i];
		}
		logger << std::dec << std::endl << EndDebugInfo;
	}
	unsigned int i;
	for(i = 0; i < req_size;)
	{
		ConfigurationRegister *creg = config_regs->GetRegister(reg + i);
		if(!creg)
		{
			if(unlikely(verbose))
			{
				logger << DebugError << LOCATION
					<< "error while trying to write configuration register 0x"
					<< std::hex << (reg + i) << std::dec << std::endl << EndDebugError;
			}
			status = false;
			i++;
			continue;
		}
		if(!(creg->permission & unisim::component::cxx::chipset::mpc107::ConfigurationRegister::WriteAccess))
		{
			if(unlikely(DEBUG && verbose))
			{
				logger << DebugWarning << LOCATION
					<< "Write access to unwritable register " << creg->name
					<< " with value = " << std::hex;
				for(unsigned int j = i; j < req_size; j++)
				{
					if(j != i) logger << " ";
					logger << "0x" << (unsigned int)req_data[j];
				}
				logger << std::dec << std::endl << EndDebugWarning;
			}
			i += creg->byte_size; 
			continue;
		}
		
		unsigned int j;
		uint32_t new_creg_value = creg->value;
		for(j = (reg + i) - creg->base_address; i < req_size && j < creg->byte_size; i++, j++)
		{
			unsigned int shift = 8 * j;
			uint32_t mask = 0xff << shift;
			new_creg_value = (new_creg_value & ~mask) | ((uint32_t) req_data[i] << shift);
		}
		
		if(creg->base_address >= 10 && creg->base_address <= 24)
		{
			if(unlikely(verbose))
			{
				logger << DebugWarning << LOCATION
					<< "Accessing bar registers in host mode (reg_name = " 
					<< creg->name << ", reg_id = " << creg->base_address << ")" << std::endl
					<< EndDebugWarning;
			}
			uint32_t bar_mask;

			bar_mask = (creg->value & 0x1) ? BAR_IO_MASK : BAR_MEM_MASK;
			
			if(new_creg_value == 0xffffffffUL)
			{
				//Say we need 0 size to allocate
				//writing bar address
				//We check the bit that indicates if it's for io or mem
				// we write 0 to indicate we have nothing to map here
				new_creg_value = creg->value & bar_mask;
			}
			else
			{
				new_creg_value = (new_creg_value & ~bar_mask) | (creg->value & bar_mask);
			} 
		}
		
		if(!WriteLocalConfigReg(creg->base_address, new_creg_value)) status = false;
	}
	return status;
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WriteLocalConfigReg(int reg, uint32_t data) {
	//Check if it's a special case
	switch(reg) {
	case EUMBBAR:
		return WriteEUMBBAR(reg, data);
		break;
	case PCI_COMMAND_REGISTER:
		return WritePCICommandRegister(reg, data);
		break;
	case PICR1:
		return WritePICR1(reg, data);
		break;
	case PICR2:
		return WritePICR2(reg, data);
		break;
	case MCCR1:
		return WriteMCCR1(reg, data);
		break;
	case MCCR2:
		return WriteMCCR2(reg, data);
		break;
	case MCCR3:
		return WriteMCCR3(reg, data);
		break;
	case MCCR4:
		return WriteMCCR4(reg, data);
		break;
	case MEM_START_ADDR_REG1:
		return WriteMemStartAddrReg1(reg, data);
		break;
	case MEM_START_ADDR_REG2:
		return WriteMemStartAddrReg2(reg, data);
		break;		
	case EXT_MEM_START_ADDR_REG1:
		return WriteExtMemStartAddrReg1(reg, data);
		break;
	case EXT_MEM_START_ADDR_REG2:
		return WriteExtMemStartAddrReg2(reg, data);
		break;		
	case MEM_END_ADDR_REG1:
		return WriteMemEndAddrReg1(reg, data);
		break;
	case MEM_END_ADDR_REG2:
		return WriteMemEndAddrReg2(reg, data);
		break;		
	case EXT_MEM_END_ADDR_REG1:
		return WriteExtMemEndAddrReg1(reg, data);
		break;
	case EXT_MEM_END_ADDR_REG2:
		return WriteExtMemEndAddrReg2(reg, data);
		break;	
	case MEM_BANK_EN_REG:
		return WriteMemBankEnableReg(reg, data);
		break;
	case MEM_PAGE_MODE_REG:
		return WriteMemPageModeReg(reg, data);
		break;
	default: // Its a normal register, we just write it :)
		//This is a caothic thing
		config_regs->GetRegister(reg)->Write(reg, data);
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "TODO register " << config_regs->GetRegister(reg)->name 
				<< "(" << config_regs->GetRegister(reg)->long_name 
				<< ") written (register number id = 0x" << std::hex << reg << std::dec << ")" 
				<< " with value = 0x" << std::hex << data << std::dec << std::endl
				<< EndDebugWarning;
		return true;
	}
}
		
template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WriteEUMBBAR(int reg, uint32_t data) {
	/* checking reserved bits */
	if(data & (uint32_t)0x000fffff) {
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to write reserved bits 19-0 of the "
				<< "Embedded Utilities Memory Block Base Address Register (EUMBBAR) to 0x"
				<< std::hex << (data & (uint32_t)0xfff00000) << std::dec
				<< ", setting it to 0x0" 
				<< std::endl << EndDebugWarning;
		data = data & (uint32_t)0x000fffff;
	}
	/* showing modified address and updating EUMBBAR */
	uint32_t val;
	val = (data >> 20) & (uint32_t)0x0fff;
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the Embbeded Utilities Memory Block Base Address Register with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "): " << std::endl
			<< "   - setting base address (bits 31-20) to 0x"
			<< std::hex << val << std::dec << " (";
		if((val >= (uint32_t)0x800) && (val <= (uint32_t)0x0fdf))
			logger << "thus enabling the Embedded Utilities from address 0x"
				<< std::hex << (val << 20) << std::dec << " to 0x"
				<< std::hex << ((val << 20) + (uint32_t)0xfffff) << std::dec;
		else
			logger << "thus disabling the Embedded Utilities";
		logger << ")" << std::endl;
	}
	config_regs->GetRegister(reg)->Write(reg, data);
	if(unlikely(DEBUG && verbose)) {
		logger << "checking Embedded Utilities Memory Block Base Address Register = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	/* updating the address map */
	if(!addr_map->Reset()) {
		if(unlikely(verbose))
			logger << DebugError << LOCATION
				<< "Error while resetting the address map configuration when updating the "
				<< "Embedded Utilities Memory Blocak Base Address Register with value = 0x"
				<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl			
				<< EndDebugError;
		return false;
	}
	return true;
} 

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WritePCICommandRegister(int reg, uint32_t data) {
	/* checking reserved/hardwired registers:
	 * - starting with bits from 10 to 15 which are reserved and set to 0,
	 * - the bit 9 fast back to back bit hardwired to 0 
	 * - the reserved bit 7 set to 0 
	 * - the reserved bit 5 set to 0
	 * - the bit 3 special cycles bit hardwired to 0
	 * - the bit 1 i/o space bit hardwired to 0 */
	for(unsigned int i = 10; i < 16; i++) {
		if(data & (((uint32_t)1) << i)) {
			data = data & ~(uint32_t)(((uint32_t)1) << i);
			if(unlikely(DEBUG && verbose))
				logger << DebugWarning << LOCATION
					<< "Trying to set reserved bit " << i << " of the PCI Command Register"
					<< ", reseting it to 0" << std::endl
					<< EndDebugWarning;
		}
	}
	if(data & (((uint32_t)1) << 9)) {
		data = data & ~(uint32_t)(((uint32_t)1) << 9);
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set fast back-to-back bit 9 of the PCI Command Register"
				<< " which is hardwired to 0, reseting it to 0" << std::endl
				<< EndDebugWarning;
	}
	if(data & (((uint32_t)1) << 7)) {
		data = data & ~(uint32_t)(((uint32_t)1) << 7);
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set reserved bit 7 of the PCI Command Register"
				<< ", reseting it to 0" << std::endl
				<< EndDebugWarning;
	}
	if(data & (((uint32_t)1) << 5)) {
		data = data & ~(uint32_t)(((uint32_t)1) << 5);
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set reserved bit 5 of the PCI Command Register"
				<< ", reseting it to 0" << std::endl
				<< EndDebugWarning;
	}
	if(data & (((uint32_t)1) << 3)) {
		data = data & ~(uint32_t)(((uint32_t)1) << 3);
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set special cycles bit 3 of the PCI Command Register"
				<< " which is hardwired to 0, reseting it to 0" << std::endl
				<< EndDebugWarning;
	}
	if(data & (uint32_t)1) {
		data = data & ~(uint32_t)1;
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set I/O space bit 0 of the PCI Command Register"
				<< " which is hardwired to 0, reseting it to 0" << std::endl
				<< EndDebugWarning;
	}
	/* showing the modified bits:
	 * - starting with bit 8 SERR
	 * - then bit 6 parity error response 
	 * - then bit 4 memory-write-and-invalidate
	 * - then bit 2 bus master
	 * - then bit 1 memory space */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the PCI Command Register with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " << std::endl;
		if(data & (((uint32_t)1) << 8))
			logger << "   - setting";
		else
			logger << "   - unsetting";
		logger << " bit 8 SERR"
			<< " (enable on 1, disable on 1 SERR driver bit)" << std::endl;
		if(data & (((uint32_t)1) << 6))
			logger << "   - setting";
		else
			logger << "   - unsetting";
		logger << " bit 6 parity errors response"
			<< "(take on 1, not take actions 0 on parity errors)" << std::endl;
		if(data & (((uint32_t)1) << 4))
			logger << "   - setting";
		else
			logger << "   - unsetting";
		logger << " bit 4 memory-write-and-invalidate"
			<< " (memory-write on 0, memory-write-and-invalidate on 1)" << std::endl;
		if(data & (((uint32_t)1) << 2))
			logger << "   - setting";
		else
			logger << "   - unsetting";
		logger << " bit 2 bus master"
			<< " (host on 1, agent on 0)" << std::endl;
		if(data & (((uint32_t)1) << 1))
			logger << "   - setting";
		else
			logger << "   - unsetting";
		logger << " bit 1 memory space"
			<< " (chipset do not respond on 0, respond on 1 to PCI memory space accesses)" 
			<< std::endl;	 
	}
	config_regs->GetRegister(reg)->Write(reg, data);
	if(unlikely(DEBUG && verbose)) {
		logger << "checking PCI Command Register = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	/* updating the address map */
	if(!addr_map->Reset()) {
		if(unlikely(verbose))
			logger << DebugError << LOCATION
				<< "Error while resetting the address map configuration when updating the "
				<< "PCI Command Register with value = 0x"
				<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl			
				<< EndDebugError;
		return false;
	}
	return true;
} 

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WritePICR1(int reg, uint32_t data) {
	/* checking reserved/hardwired registers:
	 * - starting with bits from 31-24 which are reserved and set to 1,
	 * - the reserved bit 21 set to 0 
	 * - the read-only bit 20 (RCS0) set during the start up
	 * - the reserved bit 19 set to 0
	 * - bits 18-17 which have reserved values and only accepts 0b10
	 * - the address map bit 16 which can not be set to 1 (address map A) if in
	 *     PCI agent mode
	 * - the read-only bits 15-14 which are the multiprocessor identifier
	 * - the bits 9 and 8 which are reserved and set to 0
	 * - the multiprocessor configuration bits 1-0 that can be only set to
	 *     00 (uniprocessor) or 11 (multiprocessor, 2-way MP)
	 */
	for(unsigned int i = 24; i < 31; i++) {
		if((data & (((uint32_t)1) << i)) == 0) {
			data = data | (uint32_t)(((uint32_t)1) << i);
			if(unlikely(verbose))
				logger << DebugWarning << LOCATION
					<< "Trying to unset reserved bit " << i << " of the PICR1"
					<< ", reseting it to 1" << std::endl
					<< EndDebugWarning;
		}
	}
	if(data & (((uint32_t)1) << 21)) {
		data = data & ~(uint32_t)(((uint32_t)1) << 21);
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set reserved bit 21 of the PICR1"
				<< ", reseting it to 0" << std::endl
				<< EndDebugWarning;
	}
	if((data & (((uint32_t)1) << 20)) != (config_regs->picr1.value & (((uint32_t)1) << 20))) {
		if(config_regs->picr1.value & (((uint32_t)1) << 20)) {
			data = data | (uint32_t)(((uint32_t)1) << 20);
			if(unlikely(verbose))
				logger << DebugWarning << LOCATION
					<< "Trying to unset read-only bit 20 (RCS0) of the PICR1"
					<< ", reseting it to 1" << std::endl
					<< EndDebugWarning;
		} else {
			data = data & ~(uint32_t)(((uint32_t)1) << 20);
			if(unlikely(DEBUG && verbose))
				logger << DebugWarning << LOCATION
					<< "Trying to set read-only bit 20 (RCS0) of the PICR1"
					<< ", reseting it to 0" << std::endl
					<< EndDebugWarning;
		}
	}
	if(data & (((uint32_t)1) << 19)) {
		data = data & ~(uint32_t)(((uint32_t)1) << 19);
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set reserved bit 19 of the PICR1"
				<< ", reseting it to 0" << std::endl
				<< EndDebugWarning;
	}
	if(((data & (((uint32_t)1) << 18)) == 0) && (data & (((uint32_t)1) << 17))) {
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set bits 19-19 (processor type) of the PICR1 to "
				<< ((data & (((uint32_t)1) << 18)) ? "1" : "0")
				<< ((data & (((uint32_t)1) << 17)) ? "1" : "0")
				<< " which is a reserved value"
				<< ", resetting it to 10" << std::endl
				<< EndDebugWarning;
		data = data | (uint32_t)(((uint32_t)1) << 18);
		data = data & ~(uint32_t)(((uint32_t)1) << 17);
	}
	if((data & (((uint32_t)1) << 16)) && 
		((config_regs->pci_command_reg.value & (((uint32_t)1) << 2)) == 0)) {
		data = data & (((uint32_t)1) << 16);
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set bit 16 address map to 1 (address map A) while "
				<< "chipset in agent mode (PCI command reg bit 2 is 0), "
				<< "resetting it to 0" << std::endl << EndDebugWarning;
	}
	if(data & (((uint32_t)1) << 15)) {
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set bits 15-14 (multiprocessor identifer) to "
				<< ((data & (((uint32_t)1) << 15)) ? "1" : "0")
				<< ((data & (((uint32_t)1) << 14)) ? "1" : "0")
				<< " which is not an allowed value (00 and 01 are correct values)"
				<< ", resetting it to default value 00"
				<< std::endl << EndDebugWarning;
		data = data & ~(uint32_t)(((uint32_t)1) << 15);
		data = data & ~(uint32_t)(((uint32_t)1) << 14);
	}
	for(unsigned int i = 8; i < 9; i++) {
		if(data & (((uint32_t)1) << i)) {
			data = data & ~(((uint32_t)1) << i);
			if(unlikely(verbose))
				logger << DebugWarning << LOCATION
					<< "Trying to set reserved bit " << i << " of the PICR1"
					<< ", reseting it to 0" << std::endl
					<< EndDebugWarning;
		}
	}
	if(((data >> 1) & ((uint32_t)1)) != (data & ((uint32_t)1))) {
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
					<< "Trying to set bits 1-0 (multiprocessor config) of PICR1 to "
					<< ((data & ((uint32_t)2)) ? "1" : "0")
					<< ((data & ((uint32_t)1)) ? "1" : "0")
					<< " which is a reserved value (only 00 = uniprocessor and 11 = 2-way MP) "
					<< "are allowed values, resetting to default value 00" 
					<< std::endl << EndDebugWarning;
		data = data & ~(uint32_t)3;		
	} 
	/* showing the modified/set bits:
	 * - bits 23-22 burst read wait states (CF_BREAD_WS)
	 * - bit 16 address map (ADDRESS_MAP) (0 = B, 1 = A)
	 * - bits 15-14 multiprocessor identifier (CF_MP_ID)
	 * - bit 13 local bus slave access enable (0 = disabled, 1 = enabled)
	 * - bit 12 flash write enable (0 = disabled, 1 = enabled)
	 * - bit 11 machine check enable (0 = disabled, 1 = enabled)
	 * - bit 10 TEA enabled (0 = disabled, 1 = enabled)
	 * - bit 7 indicates indicates whether a processor write to Flash 
	 *     is restricted to a bus width transaction (0 = restricted, 1 = not restricted)
	 * - bit 6 store gathering (0 = disabled, 1 = enabled)
	 * - bit 5 endian mode (0 = big, 1 = little)
	 * - bit 4 snoop looping operation (0 = disabled, 1 = enabled)
	 * - bit 3 processor address bus parked (0 = no parked, 1 = last processor parked)
	 * - bit 2 speculative PCI reads (0 = disabled, 1 = enabled)
	 * - bits 1-0 multiprocessor configuration (00 = uni, 11 = 2-way MP)
	 */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the PICR1 with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " << std::endl;
		logger << "   - setting bits 23-22 burst read wait states (CF_BREAD_WS)"
			<< " to " << (unsigned int)((data >> 22) & ((uint32_t)3)) << " wait states" << std::endl;
		if(data & (((uint32_t)1) << 16))
			logger << "   - setting (1) (address map A) bit 16 address map";
		else
			logger << "   - unsetting (0) (address map B) bit 16 address map";
		logger << std::endl;
		logger << "   - setting multiprocessor identifier (bits 15-14) to ";
		if(data & (((uint32_t)1) << 14))
			logger << "processor 1 (1)";
		else
			logger << "processor 0 (0)";
		logger << std::endl;
		if(data & (((uint32_t)1) << 13))
			logger << "   - setting (1)";
		else
			logger << "   - unsetting (0)";
		logger << " local bus slave access (bit 13)" << std::endl;
		if(data & (((uint32_t)1) << 12))
			logger << "   - setting (1)";
		else
			logger << "   - unsetting (0)";
		logger << " flash write (bit 12)" << std::endl;
		if(data & (((uint32_t)1) << 11))
			logger << "   - setting (1)";
		else
			logger << "   - unsetting (0)";
		logger << " machine check (bit 11)" << std::endl;
		if(data & (((uint32_t)1) << 10))
			logger << "   - setting (1)";
		else
			logger << "   - unsetting (0)";
		logger << " TEA (bit 10)" << std::endl;
		logger << "   - flash ";
		if(data & (((uint32_t)1) << 7))
			logger << "not restricted (1)";
		else
			logger << "restricted (0)";
		logger << " to bus width writes (NO_BUS_WIDTH_CHECK, bit 7)" << std::endl;
		logger << "   - store gathering (bit 6) is ";
		if(data & (((uint32_t)1) << 6))
			logger << "enabled (1)";
		else
			logger << "disabled (0)";
		logger << std::endl;
		logger << "   - endian mode (bit 5) set to ";
		if(data & (((uint32_t)1) << 5))
			logger << "little-endian (1)";
		else
			logger << "big-endian (0)";
		logger << std::endl;
		if(data & (((uint32_t)1) << 4))
			logger << "   - setting (1) (enabling) ";
		else
			logger << "   - unsetting (0) (disabling) ";
		logger << "snoop looping (bit 4)" << std::endl;
		if(data & (((uint32_t)1) << 3))
			logger << "   - setting (1)";
		else
			logger << "   - unsetting (0)";
		logger << " processor address bus park (bit 3)" << std::endl;
		if(data & (((uint32_t)1) << 2))
			logger << "   - setting (1)";
		else
			logger << "   - unsetting (0)";
		logger << " speculative PCI reads (bit 2)" << std::endl;
		logger << "   - multiprocessor configuration (bits 1-0) set to ";
		if(data & ((uint32_t)1))
			logger << "2-way multiprocessor (11)";
		else
			logger << "uniprocessor (00)";
		logger << std::endl;
	}
	config_regs->GetRegister(reg)->value = data;
	if(unlikely(DEBUG && verbose)) {
		logger << "checking PICR1 = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	/* updating the address map */
	if(!addr_map->Reset()) {
		if(unlikely(verbose))
			logger << DebugError << LOCATION
				<< "Error while resetting the address map configuration when updating the "
				<< "Processor Interface Configuration Register 1 with value = 0x"
				<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl			
				<< EndDebugError;
		return false;
	}
	return true;
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WritePICR2(int reg, uint32_t data) {
	/* checking reserved/hardwired registers:
	 * - reserved bits: 31, 30, 28, 24-20, 17-11, 8-4, 1,0 set to 0
	 * - bits 10-9 can not take the reserved value 00 (default 11)
	 */
	for(unsigned int i = 0; i < 31; i++) {
		switch(i) {
		case 31: case 30: case 28: case 24:
		case 23: case 22: case 21: case 20:
		case 17: case 16: case 15: case 14:
		case 13: case 12: case 11: case 8:
		case 7: case 6: case 5: case 4:
		case 1:	case 0:
			if(data & (((uint32_t)1) << i)) {
				data = data & ~(((uint32_t)1) << i);
				if(unlikely(verbose))
					logger << DebugWarning << LOCATION
						<< "Trying to set reserved bit " << i << " of the PICR2"
						<< ", reseting it to 0" << std::endl
						<< EndDebugWarning;
			}
			break;
		default:
			break;
		}
	}
	if(((data >> 9) & ((uint32_t)3)) == 0) {
		data = data | (((uint32_t)1) << 10);
		data = data | (((uint32_t)1) << 9);
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set bits 10-9 (LBCLAIM wait states) of the PICR2 to 0b00"
				<< ", reseting them to 0b11" << std::endl
				<< EndDebugWarning;
	}
	/* showing the modified/set bits:
	 * - bit 29 (SERIALIZE_ON_CFG) indicates is configuration writes to PCI devices are serialized(1) or not (0)
	 * - bit 27 (NO_SNOOP_EN) snooping enabled(0)/disabled(1)
	 * - bit 26 (CF_FF0_LOCAL) rom remapping enabled(1)/disabled(0)
	 * - bit 25 (FLASH_WR_LOCKOUT) flash write lockout enabled(0)/disabled(1)
	 * - bits 19-18 (CF_SNOOP_WS) snoop wait states
	 * - bits 10-9 (CF_LBCLAIM_WS) LBCLAIM wait states
	 * - bits 3-2 (CF_APHASE_WS) address phase wait states
	 */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the PICR2 with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " << std::endl;
		if(data & (((uint32_t)1) << 29))
			logger << "   - serializing ";
		else
			logger << "   - not serializing ";
		logger << "configuration writes to PCI devices (bit 29, SERIALIZE_ON_CFG)" << std::endl;
		if(data & (((uint32_t)1) << 27))
			logger << "   - setting bit 27 (NO_SNOOP_EN) disabling";
		else 
			logger << "   - unsetting bit 27 (NO_SNOOP_EN) enabling";
		logger << " snooping" << std::endl;
		if(data & (((uint32_t)1) << 26))
			logger << "   - enabling (1)";
		else
			logger << "   - disabling (0)";
		logger << " rom remapping (bit 26, CF_FF0_LOCAL)" << std::endl;
		if(data & (((uint32_t)1) << 25))
			logger << "   - disabling (1)";
		else
			logger << "   - enabling (0)";
		logger << " flash write lockout (bit 25, FLASH_WR_LOCKOUT)" << std::endl;
		logger << "   - setting bits 19-18 (CF_SNOOP_WS) to 0x"
			<< std::hex << (unsigned int)((data >> 18) & ((uint32_t)3)) << std::dec
			<< ", which corresponds to " << (unsigned int)((data >> 18) & ((uint32_t)3)) << " snoop wait states" << std::endl;
		logger << "   - setting bits 10-9 (CF_LBCLAIM_WS, LBCLAIM wait states) to 0x"
			<< std::hex << (unsigned int)((data >> 9) & ((uint32_t)3)) << std::dec
			<< ", which corresponds to " << (unsigned int)((data >> 18) & ((uint32_t)3)) << " clock cycles" << std::endl;
		logger << "   - setting bits 3-2 (CF_APHASE_WS, address phase wait states) to 0x"
			<< std::hex << (unsigned int)((data >> 2) & ((uint32_t)3)) << std::dec
			<< ", which corresponds to " << (unsigned int)((data >> 2) & ((uint32_t)3)) << " wait states" 
			<< std::endl;
	}
//	config_regs->GetRegister(reg)->Write(reg, data);
	config_regs->GetRegister(reg)->value = data;
	if(unlikely(DEBUG && verbose)) {
		logger << "checking PICR2 = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	return true;	
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WriteMCCR1(int reg, uint32_t data) {
	/* checking read-only bits 22-21 DBUS_SIZ[0-1] */
	if(((data >> 21) & ((uint32_t)3)) != ((config_regs->mccr1.value >> 21) & ((uint32_t)3))) {
		if(unlikely(verbose)) {
			logger << DebugWarning << LOCATION
				<< "Trying to modify (write) read-only bits 22-21 (DBUS_SIZ[0-1])"
				<< " of MCCR1 (new value = "
				<< (((data >> 22) & ((uint32_t)1)) ? "1" : "0")
				<< (((data >> 21) & ((uint32_t)1)) ? "1" : "0")
				<< ", orig value = "
				<< (((config_regs->mccr1.value >> 22) & ((uint32_t)1)) ? "1" : "0")
				<< (((config_regs->mccr1.value >> 21) & ((uint32_t)1)) ? "1" : "0")
				<< "), unmodifying values" << std::endl
				<< EndDebugWarning;
		}
		data = data & ~(((uint32_t)3) << 21);
		data = data | (config_regs->mccr1.value & (((uint32_t)3) << 21));
	}
	/* showing a warning if the ram is an SDRAM (RAM_TYPE bit = 0) and bit PCKEN is 1
	 * and MCCR4[BUF_TYPE[0-1]] = 0b10 and MCCR2[INLINE_RD_EN] = 1
	 */
	if(((data & (((uint32_t)1) << 17)) == 0)
		&& (data & (((uint32_t)1) << 16))
		&& (config_regs->mccr4.value & (((uint32_t)1) << 22))
		&& ((config_regs->mccr4.value & (((uint32_t)1) << 20)) == 0)
		&& (config_regs->mccr2.value & (((uint32_t)1) << 18))) {
		if(unlikely(verbose)) {
			logger << DebugWarning << LOCATION
				<< "Setting bit 16 (PCKEN) of register MCCR1, when setting at the "
				<< "same time the SDRAM memory option (bit 17 = 0) and operating"
				<< " in in-line buffer mode and in-line parity/ECC enabled, "
				<< "PCKEN should be unset (no action taken)" 
				<< std::endl << EndDebugWarning;
		}
	}
	/* showing the modified/set bits:
	 * - bits 31-28 ROMNAL controls the next access time for burst-mode ROM and
	 *     Flash reads
	 * - bits 27-23 ROMFAL controls the access time for nonburst ROM and Flash reads,
	 *     and Flash writes
	 * - bit 20 BURST burst more ROM timing: (0) standard (nonburst) ROM access timing,
	 *     (1) burst-mode ROM access timing
	 * - bit 19 MEMGO, RAM interface logic enable (0 = disabled, 1 = enabled)
	 * - bit 18 SREN, DRAM/EDO/SDRAM self refresh during sleep mode control (0 = disabled,
	 *     1 = enabled)
	 * - bit 17 RAM_TYPE, 0 = DRAM (SDRAM), 1 = DRAM or EDO DRAM
	 * - bit 16 PCKEN, memory interface parity checking/generation enable(1)/disable(0)
	 * - bit 15-14 bank 7 row
	 * - bit 13-12 bank 6 row
	 * - bit 11-10 bank 5 row
	 * - bit 9-8 bank 4 row
	 * - bit 7-6 bank 3 row
	 * - bit 5-4 bank 2 row
	 * - bit 3-2 bank 1 row
	 * - bit 1-0 bank 0 row
	 */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the MCCR1 with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " 
			<< std::endl;
		uint32_t val;
		val = (data >> 28) & (uint32_t)0xf;
		logger 
			<< "   - setting ROMNAL (bits 31-28) to 0x" << std::hex << val << std::dec
			<< std::endl;
		val = (data >> 23) & (uint32_t)0x1f;
		logger 
			<< "   - setting ROMFAL (bits 27-23) to 0x" << std::hex << val << std::dec
			<< std::endl;
		logger
			<< "   - setting burst mode ROM timing control (bit 20, BURST) to ";
		if((data >> 20) & ((uint32_t)1))
			logger << "1 (burst mode ROM access timing)";
		else
			logger << "0 (standard (nonburst) ROM access timing)";
		logger << std::endl;
		if((data >> 19) & ((uint32_t)1))
			logger << "   - enabling (1)";
		else 
			logger << "   - disabling (0)";
		logger << " RAM interface logic (bit 19, MEMGO)" << std::endl;
		if((data >> 18) & ((uint32_t)1))
			logger << "   - enabling (1)";
		else
			logger << "   - disabling (0)";
		logger 
			<< " (bit 18, SREN) the DRAM/EDO/SDRAM self refresh during sleep mode" << std::endl;
		logger
			<< "   - setting RAM_TYPE (bit 17) to ";
		if((data >> 17) & ((uint32_t)1))
			logger << "1, which indicates DRAM or EDO DRAM" << std::endl;
		else
			logger << "0, which indicates DRAM (SDRAM)" << std::endl;
		if((data >> 16) & ((uint32_t)1))
			logger << "   - enabling (1)";
		else
			logger << "   - disabling (0)";
		logger
			<< " (bit 16, PCKEN) memory interface parity checking/generation" << std::endl;
		for(unsigned int i = 8; i != 0; i--) {
			val = (data >> ((i * 2) - 2)) & ((uint32_t)3);
			logger 
				<< "   - setting bank "
				<< (i - 1) << " row (bits "
				<< ((i * 2) - 1) << "-" << ((i * 2) - 2)
				<< ") to value 0b" << ((val & 2) >> ((uint32_t)1)) << (val & ((uint32_t)1))
				<< std::endl;
		}
	}
//	config_regs->GetRegister(reg)->Write(reg, data);
	config_regs->GetRegister(reg)->value = data;
	if(unlikely(DEBUG && verbose)) {
		logger << "checking MCCR1 = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	return true;	
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WriteMCCR2(int reg, uint32_t data) {
	/* checking invalid configurations:
	 * - bits 31-29 (TS_WAIT_TIMER) transaction state wait state timers must be set to
	 *     000 if using EDO/FPM DRAM systems MCCR1[RAM_TYPE] = 1
	 * - bit 18 (INLINE_RD_EN) can not be set when MCCR1[PCKEN] is set
	 * - bit 17 (ECC_EN) can only be set for systems using EDO/FPM DRAM (MCCR1[RAM_TYPE] = 1)
	 * - bit 17 (ECC_EN) and bit 0 (RMW_PAR) cannot be both set
	 * - setting bit 17 (ECC_EN) is illegal if bit 16 (EDO) is 1 and REGISTERED is 1
	 *     (MCCR4[BUF_TYPE[0]] = 0 and MCCR4[BUF_TYPE[1]] = 1) (not necessary to check
	 *     MCCR4[BUF_TYPE[0]] because 00 and 11 are impossible)
	 */
	if((config_regs->mccr1.value & (uint32_t)(((uint32_t)1) << 17)) &&
		((data >> 29) & ((uint32_t)7))) {
		if(unlikely(verbose)) 
			logger << DebugWarning << LOCATION
				<< "Trying to set bits 31-29 (TS_WAIT_TIMER) to value 0x"
				<< std::hex << ((data >> 19) & ((uint32_t)7)) << std::dec
				<< " when using a ram type different than SDRAM (MCCR1[RAM_TYPE] == 1), "
				<< "resetting it to value 000"
				<< std::endl << EndDebugWarning;		
		data = data & ~(((uint32_t)7) << 29);
	}
	if((config_regs->mccr1.value & (((uint32_t)1) << 16)) &&
		(data & (((uint32_t)1) << 18))) {
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set bit 18 (INLINE_RD_EN) when MCCR1[PCKEN] is set, "
				<< "resetting it to 0" << std::endl << EndDebugWarning;
		data = data & ~(((uint32_t)1) << 18);
	}
	if(((config_regs->mccr1.value & (((uint32_t)1) << 17)) == 0) &&
		(data & (((uint32_t)1) << 17))) {
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set bit 17 (ECC_EN) when using SDRAM (MCCR1[RAM_TYPE] == 0), "
				<< " resetting it to 0" << std::endl << EndDebugWarning;
		data = data & ~(((uint32_t)1) << 17);
	}
	if((data & (((uint32_t)1) << 17)) && (data & ((uint32_t)1))) {
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set bit 17 (ECC_EN) and bit 0 (RMW_PAR) at the same time, "
				<< "resetting it (ECC_EN) to 0" << std::endl << EndDebugWarning;
		data = data & ~(((uint32_t)1) << 17);
	}
	if((data & (((uint32_t)1) << 17)) && (data & (((uint32_t)1) << 16)) && 
		(config_regs->mccr4.value & (((uint32_t)1) << 20))) {
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set bit 17 (ECC_EN) and bit 16 (EDO) at the same time when "
				<< "REGISTERED = 1, resetting ECC_EN to 0" << std::endl << EndDebugWarning;
		data = data & ~(((uint32_t)1) << 17);
	}
	/* showing the modified/set bits:
	 * - bits 31-29 (TS_WAIT_TIMER) transaction start wait state timers.
	 * - bits 28-25 (ASRISE) AS rise time
	 * - bits 24-21 (ASFALL) AS fall time
	 * - bit 20 (INLINE_PAR_NOT_ECC) determines if ECC or parity on the memory data bus
	 * - bit 19 (INLINE_WR_EN) in-line parity error reporting
	 * - bit 18 (INLINE_RD_EN) in-line read parity or ECC check/correction
	 * - bit 17 (ECC_EN) ECC enabled/disabled
	 * - bit 16 (EDO) 0 standard (FPM) DRAMs / 1 EDO DRAMs
	 * - bits 15-2 (REFINT) refresh interval
	 * - bit 1 (RSV_PG) reserve page register
	 * - bit 0 (RMW_PAR) read-modify-write (RMW) parity
	 */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the MCCR2 with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " 
			<< std::endl;
		uint32_t val;
		val = (data >> 29) & (uint32_t)7;
		logger
			<< "   - setting transaction start wait states timer (bits 31-29, TS_WAIT_TIMER) 0x"
			<< std::hex << val << std::dec << std::endl;
		val = (data >> 25) & (uint32_t)0xf;
		logger
			<< "   - setting AS rise time (bits 28-25, ASRISE) to 0x"
			<< std::hex << val << std::dec << std::endl;
		val = (data >> 21) & (uint32_t)0xf;
		logger
			<< "   - setting AS fall time (bits 24-21, ASFALL) to 0x"
			<< std::hex << val << std::dec << std::endl;
		logger
			<< "   - setting INLINE_PAR_NOT_ECC (bit 20) to ";
		if(data & (((uint32_t)1) << 20))
			logger << "1 (use parity";
		else
			logger << "0 (use ECC";
		logger << " on the memory data bus)" << std::endl;
		if(data & (((uint32_t)1) << 19))
			logger << "   - enabling (1)";
		else
			logger << "   - disabling (0)";
		logger << " (bit 19, INLINE_WR_EN) "
			<< "bus parity error reporting" << std::endl;
		if(data & (((uint32_t)1) << 18))
			logger << "   - enabling (1)";
		else
			logger << "   - disabling (0)";
		logger << " (bit 18, INLINE_RD_EN) "
			<< "in-line read parity or ECC check/correction" << std::endl;
		if(data & (((uint32_t)1) << 17))
			logger << "   - enabling (1)";
		else
			logger << "   - disabling (0)";
		logger << " (bit 17, ECC_EN) ECC" << std::endl;
		logger << "   - setting bit 16 (EDO) to ";
		if(data & (((uint32_t)1) << 16))
			logger << "1 (EDO DRAMs)" << std::endl;
		else
			logger << "0 (standard DRAMs)" << std::endl;
		logger << "   - setting refresh interval (bits 15-2, REFINT) to 0x"
			<< std::hex << ((data >> 2) & (uint32_t)0x3fff) << std::dec << std::endl;
		logger << "   - setting to ";
		if(data & (uint32_t)2)
			logger << "reserve one of the four page registers at all times" 
				<< " (bit 1, RSV_PG = 1)" << std::endl;
		else
			logger << "four open page mode (bit 1, RSV_PG = 0)" << std::endl;
		if(data & (uint32_t)1)
			logger << "   - enabling (1)";
		else 
			logger << "   - disabling (0)";
		logger << " read-modify-write (RMW) parity (bit 0, RMW_PAR)"
			<< std::endl;	
	}	
//	config_regs->GetRegister(reg)->Write(reg, data);
	config_regs->GetRegister(reg)->value = data;
	if(unlikely(DEBUG && verbose)) {
		logger << "checking MCCR2 = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	return true;	
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WriteMCCR3(int reg, uint32_t data) {
	/* checking:
	 * - bits 23-20 (RDLAT) data latency from read command can not be bigger than 0110
	 * - bits 11-9 (CP4) CAS precharge interval can not be bigger than 010 (note that 0
	 *     is reserved, but it is the default value) (for DRAM/EDO only)
	 * - bits 5-3 (RCD2) RAS to CAS delay interval can not be 001
	 * - bits 2-0 (RP1) RAS precharge interval can only be written with 010, 011, 
	 *     110 and 101 (for DRAM/EDO only, accepting 000 if the memory type is SDRAM)
	 */
	uint32_t val;
	val = ((data >> 20) & (uint32_t)0x0f);
	if(val > 6) {
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Register MCCR3 data latency from read command (bits 23-20, RDLAT) "
				<< "can not be bigger than 0x6 (trying to set it to 0x"
				<< std::hex << val << std::dec << "), resetting to default value 0x0"
				<< std::endl << EndDebugWarning;
		data = data & ~(((uint32_t)0x0f) << 20);
	}
	val = (data >> 9) & (uint32_t)7;
	if((config_regs->mccr1.value & (((uint32_t)1) << 17)) &&
		(val == 0)) {
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set CAS precharge interval of MCCR3 (bits 11-9, CP4) "
				<< "to reserved value 0x0 when using a DRAM/EDO memory "
				<< "(MCCR1[RAM_TYPE] == 1), setting it to 0x1 (0x"
				<< std::hex << config_regs->mccr1.value << std::dec << ")"
				<< std::endl << EndDebugWarning;
		data = data | (((uint32_t)1) << 9);			
	}
	if(val > 2) {
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Register MCCR3 CAS precharge interval (bits 11-9, CP4) "
				<< "can not be bigger than 0x2 (trying to set it to 0x"
				<< std::hex << val << std::dec << "), resetting to default value 0x0 (or 0x1 "
				<< "if using DRAM/EDO (MCCR1[RAM_TYPE] == 1)"
				<< std::endl << EndDebugWarning;
		if(config_regs->mccr1.value & (((uint32_t)1) << 17))
			data = data | (((uint32_t)1) << 9);			
		else
			data = data & ~(((uint32_t)7) << 9);
	}
	val = (data >> 3) & (uint32_t)7;
	if(val == 1) {
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set MCCR3 RAS to CAS delay interval (bits 5-3, RCD2) "
				<< "to reserved value 0x1, resetting it 0x0"
				<< std::endl << EndDebugWarning;
		data = data & ~(((uint32_t)7) << 3);
	}
	val = data & (uint32_t)7;
	if((val != 2) && (val != 3) && (val != 6) && (val != 5) && (val != 0)) {
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set MCCR3 RAS precharge interval (bits 2-0, RP1) "
				<< "to reserved value 0x"
				<< std::hex << val << std::dec << ", resetting it 0x0 (to 0x2 if MCCR1[RAM_TYPE] = 1)"
				<< std::endl << EndDebugWarning;
		if(config_regs->mccr1.value & (((uint32_t)1) << 17))
			data = data & ~(uint32_t)5;
		else
			data = data & ~(uint32_t)7;
	}
	if((val == 0) && (config_regs->mccr1.value & (((uint32_t)1) << 17))) {
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set MCCR3 RAS precharge interval (bits 2-0, RP1) "
				<< "to reserved value 0x"
				<< std::hex << val << std::dec << ", resetting it 0x2"
				<< std::endl << EndDebugWarning;
		data = data & ~(uint32_t)5;
	}
	/* showing the new register settings */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the MCCR3 with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " 
			<< std::endl;
		val = (data >> 28) & (uint32_t)0x0f;
		logger
			<< "   - setting burst to precharge bits[2-5] (bits 31-28, BSTOPRE[2-5]) "
			<< "to 0x" << std::hex << val << std::dec << std::endl;
		val = (data >> 24) & (uint32_t)0x0f;
		logger
			<< "   - setting refresh to activate interval (bits 27-24, REFREC) "
			<< "to 0x" << std::hex << val << std::dec << std::endl;
		val = (data >> 20) & (uint32_t)0x0f;
		logger
			<< "   - setting data latency from read command (bits 23-20, RDLAT) "
			<< "to 0x" << std::hex << val << std::dec << std::endl;
		if((data >> 19) & (uint32_t)1)
			logger << "   - enabling (1)";
		else
			logger << "   - disabling (0)";
		logger << " CAS write timing modifier (bit 19, CPX)" << std::endl;
		val = (data >> 15) & (uint32_t)0x0f;
		logger << "   - setting RAS assertion interval for CBR refresh "
			<< "(bits 18-15, RAS6P) to 0x" << std::hex << val << std::dec << std::endl;
		val = (data >> 12) & (uint32_t)7;
		logger << "   - setting CAS assertion interval for page mode access "
			<< "(bits 14-12, CAS5) to 0x" << std::hex << val << std::dec << std::endl;
		val = (data >> 9) & (uint32_t)7;
		logger << "   - setting CAS precharge interval (bits 11-9, CP4) "
			<< "to 0x" << std::hex << val << std::dec << std::endl;
		val = (data >> 6) & (uint32_t)7;
		logger << "   - setting CAS assertion interval for the first access "
			<< "(bits 8-6, CAS3) to 0x" << std::hex << val << std::dec << std::endl;
		val = (data >> 3) & (uint32_t)7;
		logger << "   - setting RAS to CAS delay interval (bits 5-3, RCD2) "
			<< "to 0x" << std::hex << val << std::dec << std::endl;
		val = data & (uint32_t)7;
		logger << "   - setting RAS precharge interval (bits 2-0, RP1) "
			<< "to 0x" << std::hex << val << std::dec << std::endl;
	}
//	config_regs->GetRegister(reg)->Write(reg, data);
	config_regs->GetRegister(reg)->value = data;
	if(unlikely(DEBUG && verbose)) {
		logger << "checking MCCR3 = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	return true;	
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WriteMCCR4(int reg, uint32_t data) {
	/* checking reserved bit 16 */
	if(data & (((uint32_t)1) << 16)) {
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set reserved bit 16 of register MCCR4, resetting it to 0"
				<< std::endl << EndDebugWarning;
		data = data & ~(((uint32_t)1) << 16);
	}
	/* checking that bits 22 and 20 (BUF_TYPE[0] and BUF_TYPE[1]) are different */
	if(((data >> 22) & ((uint32_t)1)) == ((data >> 20) & ((uint32_t)1))) {
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Trying to set bits 22 and 20 (BUF_TYPE[0] and BUF_TYPE[1]) of MCCR4 to the "
				<< "same value, resetting them to 01"
				<< std::endl << EndDebugWarning;
		data = data & ~(((uint32_t)1) << 22);
		data = data | (((uint32_t)1) << 20);
	}
	/* checking that bits 7-4 (ACTORW) are not set to reserved values 0001 */
	if(((data >> 4) & (uint32_t)0xf) == 1) {
		logger << DebugWarning << LOCATION
			<< "Trying to set bits 7-4 (ACTORW) of MCCR4 to the reserved value 0x1, "
			<< "resetting it to 0x0" << std::endl << EndDebugWarning;
		data = data &  ~(((uint32_t)0x0f) << 4);
	}
	uint32_t val;
	/* showing the new register settings */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the MCCR4 with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " 
			<< std::endl;
		val = (data >> 28) & (uint32_t)0x0f;
		logger 
			<< "   - setting precharge to activate interval (bits 31-28, PRETOACT) to 0x"
			<< std::hex << val << std::dec << std::endl;
		val = (data >> 24) & (uint32_t)0x0f;
		logger
			<< "   - setting activate to precharge interval (bits 27-24, ACTTOPRE) to 0x"
			<< std::hex << val << std::dec << std::endl;
		logger << "   - setting length of burst for 32-bit data (bit 23, WMODE) to ";
		if(data & (((uint32_t)1) << 23))
			logger << "(1) eight";
		else
			logger << "(0) four";
		logger << " beats per burst" << std::endl;
		logger << "   - setting internal memory data path buffering scheme "
			<< "(bit 22 and bit 20, BUF_TYPE[0-1]) to ";
		if(data & (((uint32_t)1) << 22))
			logger << "(10) in-line buffer mode (SDRAM only)" << std::endl;
		else
			logger << "(01) registered buffer mode" << std::endl;
		if(data & (((uint32_t)1) << 21))
			logger << "   - enabling extended 128Mbytes of local ROM memory space (1)";
		else
			logger << "   - disabling extended ROM (0)";
		logger << " (bit 21, EXTROM)" << std::endl;
		val = (data >> 18) & (uint32_t)3;
		logger 
			<< "   - setting burst to precharge bits (bits 19-18, BSTOPRE[0-1]) "
			<< "to 0x" << std::hex << val << std::dec;
		logger
			<< "   - setting DBUS_SIZE[2] bit (bit 17) to " << ((data >> 17) & 1) << std::endl;
		logger
			<< "   - setting registered DIMMs option (bit 15, REGDIMM) to ";
		if(data & (((uint32_t)1) << 15))
			logger << "1, registered DIMMs" << std::endl;
		else
			logger << "0, normal DIMMs" << std::endl;
		val = (data >> 8) & (uint32_t)0x07f;
		logger
			<< "   - setting SDRAM mode register (bits 14-8, SDMODE) to 0x"
			<< std::hex << val << std::dec << std::endl;
		val = (data >> 4) & (uint32_t)0x0f;
		logger
			<< "   - setting activate to read/write interval (bits 7-4, ACTORW) to 0x"
			<< std::hex << val << std::dec << std::endl;
		val = data & (uint32_t)0x0f;
		logger
			<< "   - setting burst to precharge bits 6-9 (bits 3-0, BSTOPRE[6-9]) to 0x"
			<< std::hex << val << std::dec << std::endl;
	}
	config_regs->GetRegister(reg)->value = data;
	if(unlikely(DEBUG && verbose)) {
		logger << "checking MCCR3 = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	return true;	
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WriteMemStartAddrReg1(int reg, uint32_t data) {
	/* showing the new register settings */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the Memory Starting Address Register 1 with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " 
			<< std::endl;
		for(uint32_t i = 0; i < 4; i++) {
			uint32_t val;
			val = (data >> (i * 8)) & (uint32_t)0x0ff;
			logger 
				<< "   - setting starting address for bank " << i
				<< " (bits " << ((i * 8) + 7) << "-" <<  (i * 8) 
				<< ", Starting address bank " << i << ") "
				<< "to 0x" << std::hex << val << std::dec << std::endl;
		}
	}
	config_regs->GetRegister(reg)->value = data;
	if(unlikely(DEBUG && verbose)) {
		logger << "checking Memory Starting Address Register 1 = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	return true;	
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WriteMemStartAddrReg2(int reg, uint32_t data) {
	/* showing the new register settings */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the Memory Starting Address Register 2 with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " 
			<< std::endl;
		for(uint32_t i = 0; i < 4; i++) {
			uint32_t val;
			val = (data >> (i * 8)) & (uint32_t)0x0ff;
			logger 
				<< "   - setting starting address for bank " << (i + 4)
				<< " (bits " << ((i * 8) + 7) << "-" <<  (i * 8) 
				<< ", Starting address bank " << (i + 4) << ") "
				<< "to 0x" << std::hex << val << std::dec << std::endl;
		}
	}
	config_regs->GetRegister(reg)->value = data;
	if(unlikely(DEBUG && verbose)) {
		logger << "checking Memory Starting Address Register 2 = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	return true;	
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WriteExtMemStartAddrReg1(int reg, uint32_t data) {
	/* checking reserved bits */
	for(uint32_t i = 0; i < 4; i++) {
		uint32_t val;
		val = (data >> ((i * 8) + 2)) & (uint32_t)0x03f;
		if(val != 0) {
			if(unlikely(verbose))
				logger << DebugWarning << LOCATION
					<< "Trying to set reserved bits "
					<< ((i * 8) + 7) << "-" << ((i * 8) + 2)
					<< " of Extended Memory Starting Address Register 1 with value 0x"
					<< std::hex << val << std::dec << ", resetting it to 0x0" 
					<< std::endl << EndDebugWarning;
			data = data & (((uint32_t)0x03f) << ((i * 8) + 2));
		}
	}
	/* showing the new register settings */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the Extended Memory Starting Address Register 1 with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " 
			<< std::endl;
		for(uint32_t i = 0; i < 4; i++) {
			uint32_t val;
			val = (data >> (i * 8)) & (uint32_t)0x0ff;
			logger 
				<< "   - setting starting address for bank " << i
				<< " (bits " << ((i * 8) + 7) << "-" <<  (i * 8) 
				<< ", Starting address bank " << i << ") "
				<< "to 0x" << std::hex << val << std::dec << std::endl;
		}
	}
	config_regs->GetRegister(reg)->value = data;
	if(unlikely(DEBUG && verbose)) {
		logger << "checking Extended Memory Starting Address Register 1 = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	return true;	
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WriteExtMemStartAddrReg2(int reg, uint32_t data) {
	/* checking reserved bits */
	for(uint32_t i = 0; i < 4; i++) {
		uint32_t val;
		val = (data >> ((i * 8) + 2)) & (uint32_t)0x03f;
		if(val != 0) {
			if(unlikely(verbose)) 
				logger << DebugWarning << LOCATION
					<< "Trying to set reserved bits "
					<< ((i * 8) + 7) << "-" << ((i * 8) + 2)
					<< " of Extended Memory Starting Address Register 2 with value 0x"
					<< std::hex << val << std::dec << ", resetting it to 0x0" 
					<< std::endl << EndDebugWarning;
			data = data & (((uint32_t)0x03f) << ((i * 8) + 2));
		}
	}
	/* showing the new register settings */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the Extended Memory Starting Address Register 2 with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " 
			<< std::endl;
		for(uint32_t i = 0; i < 4; i++) {
			uint32_t val;
			val = (data >> (i * 8)) & (uint32_t)0x0ff;
			logger 
				<< "   - setting starting address for bank " << (i + 4)
				<< " (bits " << ((i * 8) + 7) << "-" <<  (i * 8) 
				<< ", Starting address bank " << (i + 4) << ") "
				<< "to 0x" << std::hex << val << std::dec << std::endl;
		}
	}
	config_regs->GetRegister(reg)->value = data;
	if(unlikely(DEBUG && verbose)) {
		logger << "checking Extended Memory Starting Address Register 2 = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	return true;	
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WriteMemEndAddrReg1(int reg, uint32_t data) {
	/* showing the new register settings */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the Memory Ending Address Register 1 with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " 
			<< std::endl;
		for(uint32_t i = 0; i < 4; i++) {
			uint32_t val;
			val = (data >> (i * 8)) & (uint32_t)0x0ff;
			logger 
				<< "   - setting ending address for bank " << i
				<< " (bits " << ((i * 8) + 7) << "-" <<  (i * 8) 
				<< ", Ending address bank " << i << ") "
				<< "to 0x" << std::hex << val << std::dec << std::endl;
		}
	}
	config_regs->GetRegister(reg)->value = data;
	if(unlikely(DEBUG && verbose)) {
		logger << "checking Memory Ending Address Register 1 = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	return true;	
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WriteMemEndAddrReg2(int reg, uint32_t data) {
	/* showing the new register settings */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the Memory Ending Address Register 2 with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " 
			<< std::endl;
		for(uint32_t i = 0; i < 4; i++) {
			uint32_t val;
			val = (data >> (i * 8)) & (uint32_t)0x0ff;
			logger 
				<< "   - setting starting address for bank " << (i + 4)
				<< " (bits " << ((i * 8) + 7) << "-" <<  (i * 8) 
				<< ", Ending address bank " << (i + 4) << ") "
				<< "to 0x" << std::hex << val << std::dec << std::endl;
		}
	}
	config_regs->GetRegister(reg)->value = data;
	if(unlikely(DEBUG && verbose)) {
		logger << "checking Memory Ending Address Register 2 = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	return true;	
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WriteExtMemEndAddrReg1(int reg, uint32_t data) {
	/* checking reserved bits */
	for(uint32_t i = 0; i < 4; i++) {
		uint32_t val;
		val = (data >> ((i * 8) + 2)) & (uint32_t)0x03f;
		if(val != 0) {
			if(unlikely(verbose))
				logger << DebugWarning << LOCATION
					<< "Trying to set reserved bits "
					<< ((i * 8) + 7) << "-" << ((i * 8) + 2)
					<< " of Extended Memory Ending Address Register 1 with value 0x"
					<< std::hex << val << std::dec << ", resetting it to 0x0" 
					<< std::endl << EndDebugWarning;
			data = data & (((uint32_t)0x03f) << ((i * 8) + 2));
		}
	}
	/* showing the new register settings */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the Extended Memory Ending Address Register 1 with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " 
			<< std::endl;
		for(uint32_t i = 0; i < 4; i++) {
			uint32_t val;
			val = (data >> (i * 8)) & (uint32_t)0x0ff;
			logger 
				<< "   - setting starting address for bank " << i
				<< " (bits " << ((i * 8) + 7) << "-" <<  (i * 8) 
				<< ", Ending address bank " << i << ") "
				<< "to 0x" << std::hex << val << std::dec << std::endl;
		}
	}
	config_regs->GetRegister(reg)->value = data;
	if(unlikely(DEBUG && verbose)) {
		logger << "checking Extended Memory Ending Address Register 1 = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	return true;	
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WriteExtMemEndAddrReg2(int reg, uint32_t data) {
	/* checking reserved bits */
	for(uint32_t i = 0; i < 4; i++) {
		uint32_t val;
		val = (data >> ((i * 8) + 2)) & (uint32_t)0x03f;
		if(val != 0) {
			if(unlikely(verbose)) 
				logger << DebugWarning << LOCATION
					<< "Trying to set reserved bits "
					<< ((i * 8) + 7) << "-" << ((i * 8) + 2)
					<< " of Extended Memory Ending Address Register 2 with value 0x"
					<< std::hex << val << std::dec << ", resetting it to 0x0" 
					<< std::endl << EndDebugWarning;
			data = data & (((uint32_t)0x03f) << ((i * 8) + 2));
		}
	}
	/* showing the new register settings */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the Extended Memory Ending Address Register 2 with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " 
			<< std::endl;
		for(uint32_t i = 0; i < 4; i++) {
			uint32_t val;
			val = (data >> (i * 8)) & (uint32_t)0x0ff;
			logger 
				<< "   - setting starting address for bank " << (i + 4)
				<< " (bits " << ((i * 8) + 7) << "-" <<  (i * 8) 
				<< ", Ending address bank " << (i + 4) << ") "
				<< "to 0x" << std::hex << val << std::dec << std::endl;
		}
	}
	config_regs->GetRegister(reg)->value = data;
	if(unlikely(DEBUG && verbose)) {
		logger << "checking Extended Memory Ending Address Register 2 = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	return true;	
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WriteMemBankEnableReg(int reg, uint32_t data) {
	/* checking that enabled banks have an starting address smaller than
	 *   the ending address */
	for(unsigned int i = 0; i < 8; i++) {
		if(data & (((uint32_t)1) << i)) {
			uint32_t start_address, end_address;
			if(i < 4) {
				start_address = config_regs->mem_start_addr_reg1.value;
				start_address = (start_address >> (i * 8)) & (uint32_t)0x0ff;
				end_address = config_regs->mem_end_addr_reg1.value;
				end_address = (end_address >> (i * 8)) & (uint32_t)0x0ff;
			} else {
				start_address = config_regs->mem_start_addr_reg2.value;
				start_address = (start_address >> ((i - 4) * 8)) & (uint32_t)0x0ff;
				end_address = config_regs->mem_end_addr_reg2.value;
				end_address = (end_address >> ((i - 4) * 8)) & (uint32_t)0x0ff;
			}
			if(start_address >= end_address) {
				if(unlikely(verbose)) {
					logger << DebugWarning << LOCATION
						<< "Trying to enable bank " << i << " on the Memory Bank Enable Register "
						<< "when the start address of the bank is bigger or equal than the end address "
						<< "(start_address = 0x" << std::hex << start_address << std::dec
						<< ", end_address = 0x" << std::hex << end_address << std::dec << ")"
						<< " (no action is taken)"
						<< std::endl << EndDebugWarning;
				}
			}
		}
	}
	/* showing enabled/disabled banks */ 
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the Memory Bank Enable Register with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " 
			<< std::endl;
		for(uint32_t i = 0; i < 8; i++) {
			if(data & (((uint32_t)1) << i))
				logger << "   - enabling (1)"; 
			else 
				logger << "   - disabling (0)";
			logger << " memory bank " << i 
				<< " (bit " << i << ", Bank " << i << ")"
				<< std::endl;
		}
	}
	config_regs->GetRegister(reg)->value = data;
	if(unlikely(DEBUG && verbose)) {
		logger << "checking Memory Bank Enable Register = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	return true;	
}

template <class SYSTEM_BUS_PHYSICAL_ADDR,
		uint32_t SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		class PCI_BUS_PHYSICAL_ADDR, 
		uint32_t PCI_MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool PCIController<SYSTEM_BUS_PHYSICAL_ADDR,
		SYSTEM_MAX_TRANSACTION_DATA_SIZE,
		PCI_BUS_PHYSICAL_ADDR, 
		PCI_MAX_TRANSACTION_DATA_SIZE,
		DEBUG>
::WriteMemPageModeReg(int reg, uint32_t data) {
	/* showing entered value */ 
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Writing the Memory Page Mode Register with value 0x"
			<< std::hex << data << std::dec << " (previous value = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << "), setting values: " 
			<< std::endl;
		if(data & (uint32_t)0x0ff)
			logger
				<< "   - page mode enabled with value to 0x" << std::hex << (data & (uint32_t)0x0ff) << std::dec;
		else
			logger
				<< "   - page mode disabled (value = 0x0)";
		logger << std::endl;
	}
	config_regs->GetRegister(reg)->value = data & (uint32_t)0x0ff;
	if(unlikely(DEBUG && verbose)) {
		logger << "checking Memory Page Mode Register = 0x"
			<< std::hex << config_regs->GetRegister(reg)->value << std::dec << std::endl;
		logger << EndDebugInfo;	 
	}
	return true;	
}

#undef LOCATION

} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_PCICONTROLLER_TCC__
