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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_BRIDGE_PCI_ISA_BRIDGE_TCC__
#define __UNISIM_COMPONENT_CXX_BRIDGE_PCI_ISA_BRIDGE_TCC__

#include <unisim/util/endian/endian.hh>
#include <sstream>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace bridge {
namespace pci_isa {

using namespace unisim::util::endian;
using namespace std;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class ADDRESS>
Bridge<ADDRESS>::Bridge(const char *name, Object *parent)
	: Object(name, parent, "PCI-to-ISA bridge")

	// PCI configuration registers initialization
	, pci_conf_device_id("pci_conf_device_id", "PCI config Device ID", 0x0, 0x10ad) // Winbond Systems Labs
	, pci_conf_vendor_id("pci_conf_vendor_id", "PCI config Vendor ID", 0x0, 0x0565) // W83C553F PCI-ISA Bridge
	, pci_conf_status("pci_conf_status", "PCI Config Status", 0x0, 0x0)
	, pci_conf_command("pci_conf_command", "PCI Config Command", 0x007f, 0x0)
	, pci_conf_class_code("pci_conf_class_code", "PCI Config Class Code", 0x0, 0x060100) // ISA Bridge
	, pci_conf_revision_id("pci_conf_revision_id", "PCI Config Revision ID", 0x0, 0x0)
	, pci_conf_bist("pci_conf_bist", "PCI Config BIST", 0x0, 0x0)
	, pci_conf_header_type("pci_conf_header_type", "PCI Config Header Type", 0x0, 0x0)
	, pci_conf_latency_timer("pci_conf_latency_timer", "PCI Config Latency Timer", 0x0, 0x0)
	, pci_conf_cache_line_size("pci_conf_cache_line_size", "PCI Config Cache Line Size", 0x0, 0x0)

	, pci_conf_carbus_cis_pointer("pci_conf_carbus_cis_pointer", "PCI Config Carbus CIS Pointer", 0x0, 0x0)
	, pci_conf_subsystem_id("pci_conf_subsystem_id", "PCI Config Subsystem ID", 0x0, 0x0)
	, pci_conf_subsystem_vendor_id("pci_conf_subsystem_vendor_id", "PCI Config Subsystem Vendor ID", 0x0, 0x0)

	, pci_conf_interrupt_line("pci_conf_interrupt_line", "PCI Config Interrupt Line", 0xff, 0)
	, pci_conf_interrupt_pin("pci_conf_interrupt_pin", "PCI Config Interrupt Pin", 0x0, 0x0)

	, pci_device_number(0)
	, initial_base_addr(0)
	, initial_io_base_addr(0)
	, isa_bus_frequency(0)
	, pci_bus_frequency(33)

	// debug stuff
	, logger(*this)
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")

	// Parameters initialization
	, param_initial_base_addr("initial-base-addr", this, initial_base_addr, "intial base address of memory space")
	, param_initial_io_base_addr("initial-io-base-addr", this, initial_io_base_addr, "initial base address of I/O space")
	, param_pci_device_number("pci-device-number", this, pci_device_number, "PCI device number")
	, param_isa_bus_frequency("isa-bus-frequency", this, isa_bus_frequency, "ISA bus frequency in Mhz")
	, param_pci_bus_frequency("pci-bus-frequency", this, pci_bus_frequency, "PCI bus frequency in Mhz")
{
	param_isa_bus_frequency.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_pci_bus_frequency.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
}
	
template <class ADDRESS>
Bridge<ADDRESS>::~Bridge()
{
}
	
template <class ADDRESS>
void Bridge<ADDRESS>::Reset()
{
}
	
// #define PCI_VENDOR_ID		0x00	/* 16 bits */
// #define PCI_DEVICE_ID		0x02	/* 16 bits */
// #define PCI_COMMAND		0x04	/* 16 bits */
// #define PCI_STATUS		0x06	/* 16 bits */
// #define PCI_CLASS_REVISION	0x08	/* High 24 bits are class, low 8 revision */
// #define PCI_REVISION_ID		0x08	/* Revision ID */
// #define PCI_CLASS_PROG		0x09	/* Reg. Level Programming Interface */
// #define PCI_CLASS_DEVICE	0x0a	/* Device class */
// #define PCI_CACHE_LINE_SIZE	0x0c	/* 8 bits */
// #define PCI_LATENCY_TIMER	0x0d	/* 8 bits */
// #define PCI_HEADER_TYPE		0x0e	/* 8 bits */
// #define PCI_BIST		0x0f	/* 8 bits */
// 
// #define PCI_BASE_ADDRESS_0	0x10	/* 32 bits */
// #define PCI_BASE_ADDRESS_1	0x14	/* 32 bits [htype 0,1 only] */
// #define PCI_BASE_ADDRESS_2	0x18	/* 32 bits [htype 0 only] */
// #define PCI_BASE_ADDRESS_3	0x1c	/* 32 bits */
// #define PCI_BASE_ADDRESS_4	0x20	/* 32 bits */
// #define PCI_BASE_ADDRESS_5	0x24	/* 32 bits */
// #define  PCI_BASE_ADDRESS_SPACE		0x01	/* 0 = memory, 1 = I/O */
// #define  PCI_BASE_ADDRESS_SPACE_IO	0x01
// #define  PCI_BASE_ADDRESS_SPACE_MEMORY	0x00
// #define  PCI_BASE_ADDRESS_MEM_TYPE_MASK	0x06
// #define  PCI_BASE_ADDRESS_MEM_TYPE_32	0x00	/* 32 bit address */
// #define  PCI_BASE_ADDRESS_MEM_TYPE_1M	0x02	/* Below 1M [obsolete] */
// #define  PCI_BASE_ADDRESS_MEM_TYPE_64	0x04	/* 64 bit address */
// #define  PCI_BASE_ADDRESS_MEM_PREFETCH	0x08	/* prefetchable? */
// #define  PCI_BASE_ADDRESS_MEM_MASK	(~0x0fUL)
// #define  PCI_BASE_ADDRESS_IO_MASK	(~0x03UL)
// /* bit 1 is reserved if address_space = 1 */
// 
// /* Header type 0 (normal devices) */
// #define PCI_CARDBUS_CIS		0x28
// #define PCI_SUBSYSTEM_VENDOR_ID	0x2c
// #define PCI_SUBSYSTEM_ID	0x2e
// #define PCI_ROM_ADDRESS		0x30	/* Bits 31..11 are address, 10..1 reserved */
// #define  PCI_ROM_ADDRESS_ENABLE	0x01
// #define PCI_ROM_ADDRESS_MASK	(~0x7ffUL)
// 
// #define PCI_CAPABILITY_LIST	0x34	/* Offset of first capability list entry */
// 
// /* 0x35-0x3b are reserved */
// #define PCI_INTERRUPT_LINE	0x3c	/* 8 bits */
// #define PCI_INTERRUPT_PIN	0x3d	/* 8 bits */
// #define PCI_MIN_GNT		0x3e	/* 8 bits */
// #define PCI_MAX_LAT		0x3f	/* 8 bits */

template <class ADDRESS>
uint8_t Bridge<ADDRESS>::ReadConfigByte(unsigned int offset)
{
	switch(offset)
	{
		case 0x00:
			return pci_conf_vendor_id;
		case 0x01:
			return pci_conf_vendor_id >> 8;
		case 0x02:
			return pci_conf_device_id;
		case 0x03:
			return pci_conf_device_id >> 8;
		case 0x04:
			return pci_conf_command;
		case 0x05:
			return pci_conf_command >> 8;
		case 0x06:
			return pci_conf_status;
		case 0x07:
			return pci_conf_status >> 8;
		case 0x08:
			return pci_conf_revision_id;
		case 0x09:
			return pci_conf_class_code;
		case 0x0a:
			return pci_conf_class_code >> 8;
		case 0x0b:
			return pci_conf_class_code >> 16;
		case 0x0c:
			return pci_conf_cache_line_size;
		case 0x0d:
			return pci_conf_latency_timer;
		case 0x0e:
			return pci_conf_header_type;
		case 0x0f:
			return pci_conf_bist;
		case 0x10:
			return pci_conf_base_addr[IO_SPACE_BAR_NUM];
		case 0x11:
			return pci_conf_base_addr[IO_SPACE_BAR_NUM] >> 8;
		case 0x12:
			return pci_conf_base_addr[IO_SPACE_BAR_NUM] >> 16;
		case 0x13:
			return pci_conf_base_addr[IO_SPACE_BAR_NUM] >> 24;
		case 0x14:
			return pci_conf_base_addr[MEMORY_SPACE_BAR_NUM];
		case 0x15:
			return pci_conf_base_addr[MEMORY_SPACE_BAR_NUM] >> 8;
		case 0x16:
			return pci_conf_base_addr[MEMORY_SPACE_BAR_NUM] >> 16;
		case 0x17:
			return pci_conf_base_addr[MEMORY_SPACE_BAR_NUM] >> 24;
		case 0x28:
			return 0xf1;
		case 0x29:
			return 0xff;
		case 0x2a:
			return 0xff;
		case 0x2b:
			return 0xff;
		case 0x3c:
			return pci_conf_interrupt_line;
		case 0x3d:
			return pci_conf_interrupt_pin;
	}
	return 0x00;
}
	
template <class ADDRESS>
void Bridge<ADDRESS>::WriteConfigByte(unsigned int offset, uint8_t value)
{
	switch(offset)
	{
		case 0x04:
			pci_conf_command = (pci_conf_command & 0xff00) | value;
			break;
		case 0x05:
			pci_conf_command = (pci_conf_command & 0x00ff) | ((uint16_t) value << 8);
			break;
		case 0x0f:
			pci_conf_bist = value;
			break;
		case 0x10:
			pci_conf_base_addr[IO_SPACE_BAR_NUM] = ((pci_conf_base_addr[IO_SPACE_BAR_NUM] & 0xffffff00UL) == 0xffffff00UL && value == 0xff) ?
									(~IO_SPACE_SIZE + 1) :
									(pci_conf_base_addr[IO_SPACE_BAR_NUM] & 0xffffff00UL) | value;
			break;
		case 0x11:
			pci_conf_base_addr[IO_SPACE_BAR_NUM] = ((pci_conf_base_addr[IO_SPACE_BAR_NUM] & 0xffff00fcUL) == 0xffff00fcUL && value == 0xff) ?
									(~IO_SPACE_SIZE + 1) :
									(pci_conf_base_addr[IO_SPACE_BAR_NUM] & 0xffff00fcUL) | ((uint16_t) value << 8);
			break;
		case 0x12:
			pci_conf_base_addr[IO_SPACE_BAR_NUM] = ((pci_conf_base_addr[IO_SPACE_BAR_NUM] & 0xff00fffcUL) == 0xff00fffcUL && value == 0xff) ?
									(~IO_SPACE_SIZE + 1) :
									(pci_conf_base_addr[IO_SPACE_BAR_NUM] & 0xff00fffcUL) | ((uint16_t) value << 16);
			break;
		case 0x13:
			pci_conf_base_addr[IO_SPACE_BAR_NUM] = ((pci_conf_base_addr[IO_SPACE_BAR_NUM] & 0x00fffffcUL) == 0x00fffffcUL && value == 0xff) ?
									(~IO_SPACE_SIZE + 1) :
									(pci_conf_base_addr[IO_SPACE_BAR_NUM] & 0x00fffffcUL) | ((uint16_t) value << 24);
			break;
		case 0x14:
			pci_conf_base_addr[MEMORY_SPACE_BAR_NUM] = ((pci_conf_base_addr[MEMORY_SPACE_BAR_NUM] & 0xffffff00UL) == 0xffffff00UL && value == 0xff) ?
									(~MEMORY_SPACE_SIZE + 1) :
									(pci_conf_base_addr[MEMORY_SPACE_BAR_NUM] & 0xffffff00UL) | value;
			break;
		case 0x15:
			pci_conf_base_addr[MEMORY_SPACE_BAR_NUM] = ((pci_conf_base_addr[MEMORY_SPACE_BAR_NUM] & 0xffff00f0UL) == 0xffff00f0UL && value == 0xff) ?
									(~MEMORY_SPACE_SIZE + 1) :
									(pci_conf_base_addr[MEMORY_SPACE_BAR_NUM] & 0xffff00f0UL) | ((uint16_t) value << 8);
			break;
		case 0x16:
			pci_conf_base_addr[MEMORY_SPACE_BAR_NUM] = ((pci_conf_base_addr[MEMORY_SPACE_BAR_NUM] & 0xff00fff0UL) == 0xff00fff0UL && value == 0xff) ?
									(~MEMORY_SPACE_SIZE + 1) :
									(pci_conf_base_addr[MEMORY_SPACE_BAR_NUM] & 0xff00fff0UL) | ((uint16_t) value << 16);
			break;
		case 0x17:
			pci_conf_base_addr[MEMORY_SPACE_BAR_NUM] = ((pci_conf_base_addr[MEMORY_SPACE_BAR_NUM] & 0x00fffff0UL) == 0x00fffff0UL && value == 0xff) ?
									(~MEMORY_SPACE_SIZE + 1) :
									(pci_conf_base_addr[MEMORY_SPACE_BAR_NUM] & 0x00fffff0UL) | ((uint16_t) value << 24);
			break;
	}
}

template <class ADDRESS>
bool Bridge<ADDRESS>::TranslatePCItoISA(unisim::component::cxx::pci::PCISpace pci_space, unisim::component::cxx::pci::TransactionType pci_transaction_type, ADDRESS pci_addr, uint32_t size, unisim::component::cxx::isa::ISASpace& isa_space, unisim::component::cxx::isa::TransactionType& isa_transaction_type, unisim::component::cxx::isa::isa_address_t& isa_addr)
{
	ADDRESS pci_base_addr;
	switch(pci_transaction_type)
	{
		case unisim::component::cxx::pci::TT_READ:
			isa_transaction_type = unisim::component::cxx::isa::TT_READ;
			break;
		case unisim::component::cxx::pci::TT_WRITE:
			isa_transaction_type = unisim::component::cxx::isa::TT_WRITE;
			break;
	}

	switch(pci_space)
	{
		case unisim::component::cxx::pci::SP_IO:
		{
			isa_space = unisim::component::cxx::isa::SP_IO;
			pci_base_addr = pci_conf_base_addr[IO_SPACE_BAR_NUM] & ~((ADDRESS) 0x3);
			isa_addr = pci_addr - pci_base_addr;
			return pci_addr >= pci_base_addr && pci_addr + size < pci_base_addr + IO_SPACE_SIZE;
		}

		case unisim::component::cxx::pci::SP_MEM:
		{
			isa_space = unisim::component::cxx::isa::SP_MEM;
			pci_base_addr = pci_conf_base_addr[MEMORY_SPACE_BAR_NUM] & ~((ADDRESS) 0xf);
			isa_addr = pci_addr - pci_base_addr;
			return pci_addr >= pci_base_addr && pci_addr + size < pci_base_addr + MEMORY_SPACE_SIZE;
		}
		
		default:
			break;
	}
	return false;
}

template <class ADDRESS>
bool Bridge<ADDRESS>::BeginSetup()
{
	// PCI configuration registers initialization	
	pci_conf_base_addr[IO_SPACE_BAR_NUM].Initialize("pci_conf_base_addr[IO_SPACE_BAR_NUM]", "PCI Config Base Address (I/O)", 0, 0); // ISA I/O space is not configurable
	pci_conf_base_addr[MEMORY_SPACE_BAR_NUM].Initialize("pci_conf_base[MEMORY_SPACE_BAR_NUM]", "PCI Config Base Address (Memory)", 0, 0); // ISA Memory space is not configurable

	pci_conf_command = pci_conf_command | 0x3; // active memory space & I/O space
	return true;
}

} // end of namespace pci_isa
} // end of namespace bridge
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
