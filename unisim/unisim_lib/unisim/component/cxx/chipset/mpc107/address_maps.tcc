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
 
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/util/likely/likely.hh"
#include "unisim/component/cxx/chipset/mpc107/config_regs.hh"

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

#define LOCATION "In function " << __FUNCTION__ << ", file " << __FILE__ << ", line #" << __LINE__ 

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::AddressMap(ConfigurationRegisters &_config_regs,
	ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> &_atu,
	const char *name, Object *parent)
	: Object(name, parent, "MPC107 Address mapper")
	, proc_list(NULL)
	, pci_list(NULL)
	, config_regs(&_config_regs)
	, atu(&_atu)
	, logger(*this)
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
{
}

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::~AddressMap() {
	DeleteEntryList(&proc_list);
	DeleteEntryList(&pci_list);
}

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
bool
AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::BeginSetup() {
	return true;
}

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
void
AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::OnDisconnect() {
}

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
void 
AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::AddEntryNode(AddressMapEntryNode **list, AddressMapEntryNode *node) {
	if(*list == NULL) {
		*list = node;
		node->prev = NULL;
		node->next = NULL;
	} else {
		node->prev = NULL;
		(*list)->prev = node;
		node->next = *list;
		*list = node;
	}
}

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
void
AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::CreateAddressMapEntry(AddressMapEntryNode **list,
								const char *name,
								address_t orig,
								address_t end,
								AddressMapEntry::Type type) {
	AddressMapEntry *entry;
	AddressMapEntryNode *node;

	entry = new AddressMapEntry(name,
								orig,
								end,
								type);
	node = new AddressMapEntryNode(entry);
	AddEntryNode(list, node);
}

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
void
AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::
CreateProcessorPCIView() {
	uint32_t val;
	uint32_t eu_addr = 0;
	bool host_mode;

	/* checking the host mode */	
	host_mode = ((config_regs->pci_command_reg.value >> 2) & (uint32_t)1) == (uint32_t)1;
	/* checking if the embedded utilities are activated */
	val = config_regs->eumbbar.value;
	if((val >= 0x80000000) && (val <= 0xfdf00000))
		eu_addr = val;

	if(eu_addr == 0) {
		/* the embedded utilities are not activated */
		CreateAddressMapEntry(&proc_list,
			"PCI memory space",
			0x80000000,
			0xfdffffff,
			AddressMapEntry::PCI_MEMORY_SPACE);
		CreateAddressMapEntry(&proc_list,
							"PCI I/O space (8 Mbytes) [1]",
							0xfe000000,
							0xfe7fffff,
							AddressMapEntry::PCI_IO_SPACE_1);
		CreateAddressMapEntry(&proc_list,
							"PCI I/O space (4 Mbytes) [2]",
							0xfe800000,
							0xfebfffff,
							AddressMapEntry::PCI_IO_SPACE_2);
	} else {
		if(host_mode == true || !atu->OutboundAddressTranslationEnabled()) {
			CreateAddressMapEntry(&proc_list,
					"PCI memory space",
					(uint32_t)0x80000000,
					eu_addr - 1,
					AddressMapEntry::PCI_MEMORY_SPACE);
			CreateAddressMapEntry(&proc_list,
					"PCI memory space",
					eu_addr + (uint32_t)0x00100000,
					0xfdffffff,
					AddressMapEntry::PCI_MEMORY_SPACE);
			CreateAddressMapEntry(&proc_list,
								"PCI I/O space (8 Mbytes) [1]",
								0xfe000000,
								0xfe7fffff,
								AddressMapEntry::PCI_IO_SPACE_1);
			CreateAddressMapEntry(&proc_list,
								"PCI I/O space (4 Mbytes) [2]",
								0xfe800000,
								0xfebfffff,
								AddressMapEntry::PCI_IO_SPACE_2);
		} else {
			uint32_t outbound_memory_base_address = 0;
			uint32_t outbound_memory_end_address = 0;
			
			outbound_memory_base_address = atu->OutboundMemoryBaseAddress();
			if(outbound_memory_base_address >= eu_addr &&
					outbound_memory_base_address < eu_addr + (uint32_t)0x00100000)
				outbound_memory_base_address = eu_addr + (uint32_t)0x00100000;
			outbound_memory_end_address = outbound_memory_base_address +
				atu->OutboundTranslationWindowSize() - 1;
			if(outbound_memory_end_address > (uint32_t)0xfebfffff)
				outbound_memory_end_address = (uint32_t)0xfebfffff;
			
			if(outbound_memory_base_address >= (uint32_t)0x80000000 &&
					outbound_memory_base_address < eu_addr) {
				if(outbound_memory_base_address > (uint32_t)0x80000000) {
					CreateAddressMapEntry(&proc_list,
							"PCI Memory space",
							(uint32_t)0x80000000,
							outbound_memory_base_address - 1,
							AddressMapEntry::PCI_MEMORY_SPACE);
				}
				if(outbound_memory_end_address >= (uint32_t)0xfe800000 &&
						outbound_memory_end_address <= (uint32_t)0xfebfffff) {
					CreateAddressMapEntry(&proc_list,
							"Outbound Memory Window",
							outbound_memory_base_address,
							eu_addr - 1,
							AddressMapEntry::OUTBOUND_MEMORY_WINDOW);
					CreateAddressMapEntry(&proc_list,
							"Outbound Memory Window",
							eu_addr + (uint32_t)0x00100000,
							outbound_memory_end_address,
							AddressMapEntry::OUTBOUND_MEMORY_WINDOW);
					if(outbound_memory_end_address < (uint32_t)0xfe7fffff)
						CreateAddressMapEntry(&proc_list,
								"PCI I/O space (4 Mbytes) [2]",
								outbound_memory_end_address + 1,
								(uint32_t)0xfebfffff,
								AddressMapEntry::PCI_IO_SPACE_2);
				}
				if(outbound_memory_end_address < (uint32_t)0xfe800000) {
					CreateAddressMapEntry(&proc_list,
							"PCI I/O space (4 Mbytes) [2]",
							(uint32_t)0xfe800000,
							(uint32_t)0xfebfffff,
							AddressMapEntry::PCI_IO_SPACE_2);
				}
				if(outbound_memory_end_address >= (uint32_t)0xfe000000 &&
						outbound_memory_end_address <= (uint32_t)0xfe7fffff) {
					CreateAddressMapEntry(&proc_list,
							"Outbound Memory Window",
							outbound_memory_base_address,
							eu_addr - 1,
							AddressMapEntry::OUTBOUND_MEMORY_WINDOW);
					CreateAddressMapEntry(&proc_list,
							"Outbound Memory Window",
							eu_addr + (uint32_t)0x00100000,
							outbound_memory_end_address,
							AddressMapEntry::OUTBOUND_MEMORY_WINDOW);
					if(outbound_memory_end_address < (uint32_t)0xfe7fffff) {
						CreateAddressMapEntry(&proc_list,
								"PCI I/O space (8 Mbytes) [1]",
								outbound_memory_end_address + 1,
								(uint32_t)0xfe7fffff,
								AddressMapEntry::PCI_IO_SPACE_1);
					}
				}
				if(outbound_memory_end_address < (uint32_t)0xfe000000) {
					CreateAddressMapEntry(&proc_list,
							"PCI I/O space (8 Mbytes) [1]",
							(uint32_t)0xfe000000,
							(uint32_t)0xfe7fffff,
							AddressMapEntry::PCI_IO_SPACE_1);
				}
				if(outbound_memory_end_address >= (eu_addr + (uint32_t)0x00100000) &&
						outbound_memory_end_address  <= (uint32_t)0xfdffffff) {
					CreateAddressMapEntry(&proc_list,
							"Outbound Memory Window",
							outbound_memory_base_address,
							eu_addr - 1,
							AddressMapEntry::OUTBOUND_MEMORY_WINDOW);
					CreateAddressMapEntry(&proc_list,
							"Outbound Memory Window",
							eu_addr + (uint32_t)0x00100000,
							outbound_memory_end_address,
							AddressMapEntry::OUTBOUND_MEMORY_WINDOW);
					if(outbound_memory_end_address < (uint32_t)0xfdffffff) {
						CreateAddressMapEntry(&proc_list,
								"PCI memory space",
								outbound_memory_end_address + 1,
								(uint32_t)0xfdffffff,
								AddressMapEntry::PCI_MEMORY_SPACE);
					}
				}
				if(outbound_memory_end_address >= eu_addr &&
						outbound_memory_end_address < (eu_addr + (uint32_t)0x00100000)) {
					CreateAddressMapEntry(&proc_list,
							"PCI memory space",
							eu_addr + (uint32_t)0x00100000,
							(uint32_t)0xfdffffff,
							AddressMapEntry::PCI_MEMORY_SPACE);
				}
				if(outbound_memory_end_address <= eu_addr - 1) {
					CreateAddressMapEntry(&proc_list,
							"Outbound Memory Window",
							outbound_memory_base_address,
							outbound_memory_end_address,
							AddressMapEntry::OUTBOUND_MEMORY_WINDOW);
					if(outbound_memory_end_address < eu_addr - 1) {
						CreateAddressMapEntry(&proc_list,
								"PCI memory space",
								outbound_memory_end_address + 1,
								eu_addr - 1,
								AddressMapEntry::PCI_MEMORY_SPACE);
					}
				}
			}
			if(outbound_memory_base_address >= eu_addr) {
				CreateAddressMapEntry(&proc_list,
						"PCI Memory space",
						(uint32_t)0x80000000,
						eu_addr - 1,
						AddressMapEntry::PCI_MEMORY_SPACE);
			}
			if(outbound_memory_base_address >= eu_addr + 0x00100000 &&
					outbound_memory_base_address <= (uint32_t)0xfdffffff) {
				if(outbound_memory_base_address > eu_addr + (uint32_t)0x00100000) {
					CreateAddressMapEntry(&proc_list,
							"PCI Memory space",
							eu_addr + (uint32_t)0x00100000,
							outbound_memory_base_address - 1,
							AddressMapEntry::PCI_MEMORY_SPACE);
				}
				if(outbound_memory_end_address >= (uint32_t)0xfe800000 &&
						outbound_memory_end_address <= (uint32_t)0xfebfffff) {
					CreateAddressMapEntry(&proc_list,
							"Outbound Memory Window",
							outbound_memory_base_address,
							outbound_memory_end_address,
							AddressMapEntry::OUTBOUND_MEMORY_WINDOW);
					if(outbound_memory_end_address < (uint32_t)0xfe7fffff)
						CreateAddressMapEntry(&proc_list,
								"PCI I/O space (4 Mbytes) [2]",
								outbound_memory_end_address + 1,
								(uint32_t)0xfebfffff,
								AddressMapEntry::PCI_IO_SPACE_2);
				}
				if(outbound_memory_end_address < (uint32_t)0xfe800000) {
					CreateAddressMapEntry(&proc_list,
							"PCI I/O space (4 Mbytes) [2]",
							(uint32_t)0xfe800000,
							(uint32_t)0xfebfffff,
							AddressMapEntry::PCI_IO_SPACE_2);
				}
				if(outbound_memory_end_address >= (uint32_t)0xfe000000 &&
						outbound_memory_end_address <= (uint32_t)0xfe7fffff) {
					CreateAddressMapEntry(&proc_list,
							"Outbound Memory Window",
							outbound_memory_base_address,
							outbound_memory_end_address,
							AddressMapEntry::OUTBOUND_MEMORY_WINDOW);
					if(outbound_memory_end_address < (uint32_t)0xfe7fffff) {
						CreateAddressMapEntry(&proc_list,
								"PCI I/O space (8 Mbytes) [1]",
								outbound_memory_end_address + 1,
								(uint32_t)0xfe7fffff,
								AddressMapEntry::PCI_IO_SPACE_1);
					}
				}
				if(outbound_memory_end_address < (uint32_t)0xfe000000) {
					CreateAddressMapEntry(&proc_list,
							"PCI I/O space (8 Mbytes) [1]",
							(uint32_t)0xfe000000,
							(uint32_t)0xfe7fffff,
							AddressMapEntry::PCI_IO_SPACE_1);
				}
				if(outbound_memory_end_address >= (eu_addr + (uint32_t)0x00100000) &&
						outbound_memory_end_address  <= (uint32_t)0xfdffffff) {
					CreateAddressMapEntry(&proc_list,
							"Outbound Memory Window",
							outbound_memory_base_address,
							outbound_memory_end_address,
							AddressMapEntry::OUTBOUND_MEMORY_WINDOW);
					if(outbound_memory_end_address < (uint32_t)0xfdffffff) {
						CreateAddressMapEntry(&proc_list,
								"PCI memory space",
								outbound_memory_end_address + 1,
								(uint32_t)0xfdffffff,
								AddressMapEntry::PCI_MEMORY_SPACE);
					}
				}
			}
			if(outbound_memory_base_address >= (uint32_t)0xfe00000) {
				CreateAddressMapEntry(&proc_list,
						"PCI Memory space",
						eu_addr + (uint32_t)0x00100000,
						(uint32_t)0xfdffffff,
						AddressMapEntry::PCI_MEMORY_SPACE);
			}
			if(outbound_memory_base_address >= (uint32_t)0xfe000000 &&
					outbound_memory_base_address <= (uint32_t)0xfe7fffff) {
				if(outbound_memory_base_address > (uint32_t)0xfe000000) {
					CreateAddressMapEntry(&proc_list,
							"PCI I/O space (8 Mbytes) [1]",
							(uint32_t)0xfe000000,
							outbound_memory_base_address - 1,
							AddressMapEntry::PCI_IO_SPACE_1);
				}
				if(outbound_memory_end_address >= (uint32_t)0xfe800000 &&
						outbound_memory_end_address <= (uint32_t)0xfebfffff) {
					CreateAddressMapEntry(&proc_list,
							"Outbound Memory Window",
							outbound_memory_base_address,
							outbound_memory_end_address,
							AddressMapEntry::OUTBOUND_MEMORY_WINDOW);
					if(outbound_memory_end_address < (uint32_t)0xfe7fffff)
						CreateAddressMapEntry(&proc_list,
								"PCI I/O space (4 Mbytes) [2]",
								outbound_memory_end_address + 1,
								(uint32_t)0xfebfffff,
								AddressMapEntry::PCI_IO_SPACE_2);
				}
				if(outbound_memory_end_address < (uint32_t)0xfe800000) {
					CreateAddressMapEntry(&proc_list,
							"PCI I/O space (4 Mbytes) [2]",
							(uint32_t)0xfe800000,
							(uint32_t)0xfebfffff,
							AddressMapEntry::PCI_IO_SPACE_2);
				}
				if(outbound_memory_end_address >= (uint32_t)0xfe000000 &&
						outbound_memory_end_address <= (uint32_t)0xfe7fffff) {
					CreateAddressMapEntry(&proc_list,
							"Outbound Memory Window",
							outbound_memory_base_address,
							outbound_memory_end_address,
							AddressMapEntry::OUTBOUND_MEMORY_WINDOW);
					if(outbound_memory_end_address < (uint32_t)0xfe7fffff) {
						CreateAddressMapEntry(&proc_list,
								"PCI I/O space (8 Mbytes) [1]",
								outbound_memory_end_address + 1,
								(uint32_t)0xfe7fffff,
								AddressMapEntry::PCI_IO_SPACE_1);
					}
				}
			}
			if(outbound_memory_base_address >= (uint32_t)0xfe80000) {
				CreateAddressMapEntry(&proc_list,
						"PCI I/O space (8 Mbytes) [1]",
						(uint32_t)0xfe800000,
						(uint32_t)0xfdffffff,
						AddressMapEntry::PCI_IO_SPACE_1);
			}
			if(outbound_memory_base_address >= (uint32_t)0xfe800000 &&
					outbound_memory_base_address <= (uint32_t)0xfebfffff) {
				if(outbound_memory_base_address > (uint32_t)0xfe800000) {
					CreateAddressMapEntry(&proc_list,
							"PCI I/O space (4 Mbytes) [2]",
							(uint32_t)0xfe800000,
							outbound_memory_base_address - 1,
							AddressMapEntry::PCI_IO_SPACE_2);
				}
				if(outbound_memory_end_address >= (uint32_t)0xfe800000 &&
						outbound_memory_end_address <= (uint32_t)0xfebfffff) {
					CreateAddressMapEntry(&proc_list,
							"Outbound Memory Window",
							outbound_memory_base_address,
							outbound_memory_end_address,
							AddressMapEntry::OUTBOUND_MEMORY_WINDOW);
					if(outbound_memory_end_address < (uint32_t)0xfe7fffff)
						CreateAddressMapEntry(&proc_list,
								"PCI I/O space (4 Mbytes) [2]",
								outbound_memory_end_address + 1,
								(uint32_t)0xfebfffff,
								AddressMapEntry::PCI_IO_SPACE_2);
				}
			}

		}
	}
	CreateAddressMapEntry(&proc_list,
						"PCI configuration address register",
						0xfec00000,
						0xfedfffff,
						AddressMapEntry::PCI_CONFIG_ADDRESS_REG);
	CreateAddressMapEntry(&proc_list,
						"PCI configuration data register",
						0xfee00000,
						0xfeefffff,
						AddressMapEntry::PCI_CONFIG_DATA_REG);
	CreateAddressMapEntry(&proc_list,
						"PCI interrupt acknowledge",
						0xfef00000,
						0xfeffffff,
						AddressMapEntry::PCI_INT_ACK);
}

//template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
//bool
//AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::SetEmbeddedUtilities() {
//	uint32_t eu_addr = 0;
//
//	/* getting base address */
//	eu_addr = config_regs->eumbbar.value;
//
//	if(unlikely(DEBUG && verbose)) 
//		logger << DebugInfo
//			<< LOCATION
//			<< "Setting the embedded utilities at address 0x"
//			<< std::hex << eu_addr << std::dec 
//			<< std::endl
//			<< EndDebugInfo;
//
//	CreateAddressMapEntry(&proc_list,
//		"EUMB message unit space",
//		eu_addr,
//		eu_addr + (uint32_t)0x0fff,
//		AddressMapEntry::EUMB_MSG_UNIT_SPACE);
//	CreateAddressMapEntry(&proc_list,
//		"EUMB DMA space",
//		eu_addr + (uint32_t)0x01000,
//		eu_addr + (uint32_t)0x01fff,
//		AddressMapEntry::EUMB_DMA_SPACE);
//	CreateAddressMapEntry(&proc_list,
//		"EUMB ATU space",
//		eu_addr + (uint32_t)0x02000,
//		eu_addr + (uint32_t)0x02fff,
//		AddressMapEntry::EUMB_ATU_SPACE);
//	CreateAddressMapEntry(&proc_list,
//		"EUMB I2C space",
//		eu_addr + (uint32_t)0x03000,
//		eu_addr + (uint32_t)0x03fff,
//		AddressMapEntry::EUMB_I2C_SPACE);
//	CreateAddressMapEntry(&proc_list,
//		"Reserved",
//		eu_addr + (uint32_t)0x04000,
//		eu_addr + (uint32_t)0x03ffff,
//		AddressMapEntry::RESERVED);
//	CreateAddressMapEntry(&proc_list,
//		"EUMB EPIC space",
//		eu_addr + (uint32_t)0x040000,
//		eu_addr + (uint32_t)0x07ffff,
//		AddressMapEntry::EUMB_EPIC_SPACE);
//	CreateAddressMapEntry(&proc_list,
//		"Reserved",
//		eu_addr + (uint32_t)0x080000,
//		eu_addr + (uint32_t)0x0fefff,
//		AddressMapEntry::RESERVED);
//	CreateAddressMapEntry(&proc_list,
//		"EUMB data path diagnostics space",
//		eu_addr + (uint32_t)0x0ff000,
//		eu_addr + (uint32_t)0x0ff047,
//		AddressMapEntry::EUMB_DATA_PATH_DIAGNOSTICS_SPACE);
//	CreateAddressMapEntry(&proc_list,
//		"Reserved",
//		eu_addr + (uint32_t)0x0ff048,
//		eu_addr + (uint32_t)0x0fffff,
//		AddressMapEntry::RESERVED);
//	
//	CreateAddressMapEntry(&pci_list,
//		"EUMB message unit space",
//		eu_addr,
//		eu_addr + (uint32_t)0x0fff,
//		AddressMapEntry::EUMB_MSG_UNIT_SPACE);
//	CreateAddressMapEntry(&pci_list,
//		"EUMB DMA space",
//		eu_addr + (uint32_t)0x01000,
//		eu_addr + (uint32_t)0x01fff,
//		AddressMapEntry::EUMB_DMA_SPACE);
//	CreateAddressMapEntry(&pci_list,
//		"EUMB ATU space",
//		eu_addr + (uint32_t)0x02000,
//		eu_addr + (uint32_t)0x02fff,
//		AddressMapEntry::EUMB_ATU_SPACE);
//	CreateAddressMapEntry(&pci_list,
//		"EUMB I2C space",
//		eu_addr + (uint32_t)0x03000,
//		eu_addr + (uint32_t)0x03fff,
//		AddressMapEntry::EUMB_I2C_SPACE);
//	CreateAddressMapEntry(&pci_list,
//		"Reserved",
//		eu_addr + (uint32_t)0x04000,
//		eu_addr + (uint32_t)0x03ffff,
//		AddressMapEntry::RESERVED);
//	CreateAddressMapEntry(&pci_list,
//		"EUMB EPIC space",
//		eu_addr + (uint32_t)0x040000,
//		eu_addr + (uint32_t)0x07ffff,
//		AddressMapEntry::EUMB_EPIC_SPACE);
//	CreateAddressMapEntry(&pci_list,
//		"Reserved",
//		eu_addr + (uint32_t)0x080000,
//		eu_addr + (uint32_t)0x0fefff,
//		AddressMapEntry::RESERVED);
//	CreateAddressMapEntry(&pci_list,
//		"EUMB data path diagnostics space",
//		eu_addr + (uint32_t)0x0ff000,
//		eu_addr + (uint32_t)0x0ff047,
//		AddressMapEntry::EUMB_DATA_PATH_DIAGNOSTICS_SPACE);
//	CreateAddressMapEntry(&pci_list,
//		"Reserved",
//		eu_addr + (uint32_t)0x0ff048,
//		eu_addr + (uint32_t)0x0fffff,
//		AddressMapEntry::RESERVED);
//
//	return true;
//}

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
bool 
AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::SetAddressMapA() {
	if(unlikely(verbose)) {
		logger << DebugError
			<< LOCATION
			<< "Address map A not implemented" 
			<< std::endl
			<< EndDebugError;
	}
	return false;
}

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
void
AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::
CreateMapBPCIViewHostMode() {
	uint32_t val = 0;
	uint32_t eu_addr = 0;
	uint32_t pcsr_addr = 0;

	/* checking if the embedded utilities are activated */
	val = config_regs->eumbbar.value;
	if((val >= 0x80000000) && (val <= 0xfdf00000)) {
		eu_addr = val;
		val = config_regs->pcsrbar.value;
		pcsr_addr = val;
	}

	CreateAddressMapEntry(&pci_list,
						"Local memory space",
						0x00000000,
						0x3fffffff,
						AddressMapEntry::LOCAL_MEMORY_SPACE);
	CreateAddressMapEntry(&pci_list,
						"Reserved",
						0x40000000,
						0x77ffffff,
						AddressMapEntry::RESERVED);
	CreateAddressMapEntry(&pci_list,
						"64-bit extended ROM/Flash (64 Mbytes) [1]",
						0x78000000,
						0x7bffffff,
						AddressMapEntry::EXTENDED_ROM_1);
	CreateAddressMapEntry(&pci_list,
						"64-bit extended ROM/Flash (64 Mbytes) [2]",
						0x7c000000,
						0x7fffffff,
						AddressMapEntry::EXTENDED_ROM_2);
	if(eu_addr == 0)
		/* the embedded utilities are not activated */
		CreateAddressMapEntry(&pci_list,
			"PCI memory space",
			0x80000000,
			0xfdffffff,
			AddressMapEntry::PCI_MEMORY_SPACE);
	else {
		/* the embedded utilities are activated */
		/* set the embedded utilities mapping using the pcsrbar */
		CreateAddressMapEntry(&pci_list,
			"EUMB message unit space",
			pcsr_addr,
			pcsr_addr + (uint32_t)0x0ff,
			AddressMapEntry::EUMB_MSG_UNIT_SPACE);
		CreateAddressMapEntry(&pci_list,
			"EUMB DMA space",
			eu_addr + (uint32_t)0x0100,
			eu_addr + (uint32_t)0x02ff,
			AddressMapEntry::EUMB_DMA_SPACE);
		CreateAddressMapEntry(&pci_list,
			"EUMB ATU space",
			eu_addr + (uint32_t)0x0300,
			eu_addr + (uint32_t)0x03ff,
			AddressMapEntry::EUMB_ATU_SPACE);
		CreateAddressMapEntry(&pci_list,
			"Reserved",
			eu_addr + (uint32_t)0x0400,
			eu_addr + (uint32_t)0x0eff,
			AddressMapEntry::RESERVED);
		CreateAddressMapEntry(&pci_list,
			"EUMB data path diagnostics space",
			eu_addr + (uint32_t)0x0f00,
			eu_addr + (uint32_t)0x0f47,
			AddressMapEntry::EUMB_DATA_PATH_DIAGNOSTICS_SPACE);
		CreateAddressMapEntry(&pci_list,
			"Reserved",
			eu_addr + (uint32_t)0x0f48,
			eu_addr + (uint32_t)0x0fff,
			AddressMapEntry::RESERVED);
		/* set the pci memory space mapping that doesn't overlap the 
		 *   embedded utilities */
		if(eu_addr > (uint32_t)0x80000000)
			CreateAddressMapEntry(&pci_list,
				"PCI memory space",
				(uint32_t)0x80000000,
				pcsr_addr - 1,
				AddressMapEntry::PCI_MEMORY_SPACE);
		if(eu_addr < (uint32_t)0xfdf00000)
			CreateAddressMapEntry(&pci_list,
					"PCI memory space",
					pcsr_addr + (uint32_t)0x001000,
					0xfdffffff,
					AddressMapEntry::PCI_MEMORY_SPACE);
	}
	CreateAddressMapEntry(&pci_list,
						"Reserved",
						0xfe000000,
						0xfeffffff,
						AddressMapEntry::RESERVED);
	CreateAddressMapEntry(&pci_list,
						"8-, 32- or 64-bit Flash/ROM space (8 Mbytes), read-only area (writes causes Flash write error) [1]",
						0xff000000,
						0xff7fffff,
						AddressMapEntry::ROM_SPACE_1);
	CreateAddressMapEntry(&pci_list,
						"8-, 32- or 64-bit Flash/ROM space (8 Mbytes), read-only area (writes causes Flash write error) [2]",
						0xff800000,
						0xffffffff,
						AddressMapEntry::ROM_SPACE_1);
}

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
void
AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::
CreateMapBPCIViewAgentMode() {
	uint32_t val = 0;
	uint32_t eu_addr = 0;
	uint32_t pcsr_addr = 0;

	/* checking if the embedded utilities are activated */
	val = config_regs->eumbbar.value;
	if((val >= 0x80000000) && (val <= 0xfdf00000)) {
		eu_addr = val;
		val = config_regs->pcsrbar.value;
		pcsr_addr = val;
	}
	
	if(eu_addr == 0) {
		/* the embedded utilities are not activated */
		/* nothing to do, nothing mapped in the pci space */
	} else {
		/* the embedded utilities are activated */
		/* set the embedded utilities mapping using the pcsrbar */
		CreateAddressMapEntry(&pci_list,
			"EUMB message unit space",
			pcsr_addr,
			pcsr_addr + (uint32_t)0x0ff,
			AddressMapEntry::EUMB_MSG_UNIT_SPACE);
		CreateAddressMapEntry(&pci_list,
			"EUMB DMA space",
			eu_addr + (uint32_t)0x0100,
			eu_addr + (uint32_t)0x02ff,
			AddressMapEntry::EUMB_DMA_SPACE);
		CreateAddressMapEntry(&pci_list,
			"EUMB ATU space",
			eu_addr + (uint32_t)0x0300,
			eu_addr + (uint32_t)0x03ff,
			AddressMapEntry::EUMB_ATU_SPACE);
		CreateAddressMapEntry(&pci_list,
			"Reserved",
			eu_addr + (uint32_t)0x0400,
			eu_addr + (uint32_t)0x0eff,
			AddressMapEntry::RESERVED);
		CreateAddressMapEntry(&pci_list,
			"EUMB data path diagnostics space",
			eu_addr + (uint32_t)0x0f00,
			eu_addr + (uint32_t)0x0f47,
			AddressMapEntry::EUMB_DATA_PATH_DIAGNOSTICS_SPACE);
		CreateAddressMapEntry(&pci_list,
			"Reserved",
			eu_addr + (uint32_t)0x0f48,
			eu_addr + (uint32_t)0x0fff,
			AddressMapEntry::RESERVED);
		/* set the inbound pci address translation if activated */
		/* the address translation unit is activated for the inbound window, add the window space
		 * to the pci view
		 * check possible collisions between the embedded utilities and the inbound window */
		uint32_t local_memory_base_address = config_regs->lmbar.value;
		uint32_t local_memory_end_address = local_memory_base_address + atu->InboundTranslationWindowSize() - 1;
		uint32_t eu_end_address = eu_addr + (uint32_t)0x001000 - 1;
		if((eu_addr <= local_memory_base_address && local_memory_end_address <= eu_end_address) ||
				(eu_addr >= local_memory_base_address && local_memory_end_address <= eu_end_address) ||
				(eu_addr <= local_memory_base_address && local_memory_end_address >= eu_end_address) ||
				(eu_addr >= local_memory_base_address && local_memory_end_address >= eu_end_address)) {
			/* there are collisions between the embedded utilities and the inbound window, the embedded utilities
			 *   have preference over the inbound window */
			if(local_memory_base_address < eu_addr) {
				CreateAddressMapEntry(&pci_list,
						"Inbound memory window",
						local_memory_base_address,
						eu_addr - 1,
						AddressMapEntry::INBOUND_MEMORY_WINDOW);
				if(local_memory_end_address > eu_end_address) {
					CreateAddressMapEntry(&pci_list,
							"Inbound memory window",
							eu_end_address + 1,
							local_memory_end_address,
							AddressMapEntry::INBOUND_MEMORY_WINDOW);
				}
			} else {
				if(local_memory_end_address > eu_end_address) {
					CreateAddressMapEntry(&pci_list,
							"Inbound memory window",
							eu_end_address + 1,
							local_memory_end_address,
							AddressMapEntry::INBOUND_MEMORY_WINDOW);
				}
			}
		} else {
			/* no collisions found */
			CreateAddressMapEntry(&pci_list,
					"Inbound memory window",
					local_memory_base_address,
					local_memory_end_address,
					AddressMapEntry::INBOUND_MEMORY_WINDOW);
		}
	}

}

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
bool 
AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::SetAddressMapB() {
	uint32_t val;
	uint32_t eu_addr = 0;
	bool host_mode;

	/* checking the host mode */	
	host_mode = ((config_regs->pci_command_reg.value >> 2) & (uint32_t)1) == (uint32_t)1;
	/* checking if the embedded utilities are activated */
	val = config_regs->eumbbar.value;
	if((val >= 0x80000000) && (val <= 0xfdf00000))
		eu_addr = val;
	
	if(unlikely(DEBUG && verbose)) 
		logger << DebugInfo
			<< LOCATION
			<< "Setting address map B" 
			<< std::endl
			<< EndDebugInfo;

	/* Create the processor view */
	CreateAddressMapEntry(&proc_list,
						"Local memory space",
						0,
						0x3fffffff,
						AddressMapEntry::LOCAL_MEMORY_SPACE);
	CreateAddressMapEntry(&proc_list,
						"Reserved",
						0x40000000,
						0x77ffffff,
						AddressMapEntry::RESERVED);
	CreateAddressMapEntry(&proc_list,
						"64-bit extended ROM/Flash (64 Mbytes) [1]",
						0x78000000,
						0x7bffffff,
						AddressMapEntry::EXTENDED_ROM_1);
	CreateAddressMapEntry(&proc_list,
						"64-bit extended ROM/Flash (64 Mbytes) [2]",
						0x7c000000,
						0x7fffffff,
						AddressMapEntry::EXTENDED_ROM_2);
	
	CreateProcessorPCIView();
	CreateAddressMapEntry(&proc_list,
						"8-, 32- or 64-bit Flash/ROM space (8 Mbytes) [1]",
						0xff000000,
						0xff7fffff,
						AddressMapEntry::ROM_SPACE_1);
	CreateAddressMapEntry(&proc_list,
						"8-, 32- or 64-bit Flash/ROM space (8 Mbytes) [2]",
						0xff800000,
						0xffffffff,
						AddressMapEntry::ROM_SPACE_2);

	/* set the processor embedded utilities memory mapping if activated */
	if(eu_addr != 0) {
		CreateAddressMapEntry(&proc_list,
			"EUMB message unit space",
			eu_addr,
			eu_addr + (uint32_t)0x0fff,
			AddressMapEntry::EUMB_MSG_UNIT_SPACE);
		CreateAddressMapEntry(&proc_list,
			"EUMB DMA space",
			eu_addr + (uint32_t)0x01000,
			eu_addr + (uint32_t)0x01fff,
			AddressMapEntry::EUMB_DMA_SPACE);
		CreateAddressMapEntry(&proc_list,
			"EUMB ATU space",
			eu_addr + (uint32_t)0x02000,
			eu_addr + (uint32_t)0x02fff,
			AddressMapEntry::EUMB_ATU_SPACE);
		CreateAddressMapEntry(&proc_list,
			"EUMB I2C space",
			eu_addr + (uint32_t)0x03000,
			eu_addr + (uint32_t)0x03fff,
			AddressMapEntry::EUMB_I2C_SPACE);
		CreateAddressMapEntry(&proc_list,
			"Reserved",
			eu_addr + (uint32_t)0x04000,
			eu_addr + (uint32_t)0x03ffff,
			AddressMapEntry::RESERVED);
		CreateAddressMapEntry(&proc_list,
			"EUMB EPIC space",
			eu_addr + (uint32_t)0x040000,
			eu_addr + (uint32_t)0x07ffff,
			AddressMapEntry::EUMB_EPIC_SPACE);
		CreateAddressMapEntry(&proc_list,
			"Reserved",
			eu_addr + (uint32_t)0x080000,
			eu_addr + (uint32_t)0x0fefff,
			AddressMapEntry::RESERVED);
		CreateAddressMapEntry(&proc_list,
			"EUMB data path diagnostics space",
			eu_addr + (uint32_t)0x0ff000,
			eu_addr + (uint32_t)0x0ff047,
			AddressMapEntry::EUMB_DATA_PATH_DIAGNOSTICS_SPACE);
		CreateAddressMapEntry(&proc_list,
			"Reserved",
			eu_addr + (uint32_t)0x0ff048,
			eu_addr + (uint32_t)0x0fffff,
			AddressMapEntry::RESERVED);
	}
						
	/* Create the PCI view */
	if(host_mode) {
		CreateMapBPCIViewHostMode();
	} else {
		CreateMapBPCIViewAgentMode();
	}

	return true;
}

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
AddressMapEntry *
AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::GetEntry(AddressMapEntryNode *list, address_t addr, const char *list_name) {
	/* this condition is only needed for debugging,
	 *   it should be removed later */
	if(list == NULL) {
		if(unlikely(verbose))
			logger << DebugError
				<< LOCATION
				<< "Something went wrong, the " << list_name
				<< " address map doesn't exist"
				<< std::endl
				<< EndDebugError;
		return NULL;
	}

	AddressMapEntryNode *entry;
	/* the test could be replaced to remove the test for entry,
	 *   because all the address range should be covered by the 
	 *   address table */
	for(entry = list;
		(entry != NULL) && ((entry->entry->orig > addr) || entry->entry->end < addr); 
		entry = entry->next);
	if(entry == NULL) {
		if(unlikely(verbose))
			logger << DebugError
				<< LOCATION
				<< "Something went wrong, address 0x" << std::hex << addr << std::dec 
				<< " was not find in the " << list_name
				<< " address table"
				<< std::endl
				<< EndDebugError;
		return NULL;
	}
	
	/* replace the entry to the head of the list,
	 *   this should enhance future searches */
	if(entry != list) {
		entry->prev->next = entry->next;
		if(entry->next != NULL) {
			entry->next->prev = entry->prev;
		}
		entry->next = proc_list;
		entry->prev = NULL;
		proc_list->prev = entry;
		proc_list = entry;
	}

	return entry->entry;
}

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
AddressMapEntry *
AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::GetEntryProc(address_t addr) {
	return GetEntry(proc_list, addr, "processor");
}

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
AddressMapEntry *
AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::GetEntryPCI(address_t addr) {
	return GetEntry(pci_list, addr, "pci");
}

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
void
AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::DeleteEntryList(AddressMapEntryNode **list) {
	while(*list) {
		AddressMapEntryNode *node = *list;
		AddressMapEntry *entry = node->entry;
		*list  = node->next;
		delete entry;
		delete node;
	}
	*list = NULL;
}

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
bool
AddressMap<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG>::Reset() {
	/* first the proc and pci list must be resetted */
	DeleteEntryList(&proc_list);
	DeleteEntryList(&pci_list);
	
	/* check which address map must be set up */
	bool a_address_map;
	a_address_map = (config_regs->picr1.value & (((uint32_t)1) << 16)) != 0;
	if(a_address_map)
		return SetAddressMapA();
	else
		return SetAddressMapB();
}

#undef LOCATION

} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
