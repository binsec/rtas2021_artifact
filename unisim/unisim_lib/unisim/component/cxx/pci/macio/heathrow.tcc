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

#ifndef __UNISIM_COMPONENT_CXX_PCI_MACIO_HEATHROW_TCC__
#define __UNISIM_COMPONENT_CXX_PCI_MACIO_HEATHROW_TCC__

#include <unisim/util/endian/endian.hh>
#include <unisim/util/likely/likely.hh>
#include <sstream>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace pci {
namespace macio {

using namespace unisim::util::endian;
using namespace std;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class ADDRESS>
Heathrow<ADDRESS>::Heathrow(const char *name, Object *parent) :
	Object(name, parent, "Heathrow Programmable Interrupt Controller (PIC)"),
	logger(*this),
	verbose(false),
	
	// heathrow global registers
	heathrow_brightness("heathrow_brightness", "Heathrow Brightness", 0x0, 0x0),
	heathrow_contrast("heathrow_contrast", "Heathrow Contrast", 0x0, 0x0),
	heathrow_media_bay_control("heathrow_media_bay_control", "Heathrow Media Bay Control", 0x0, 0x0),
	heathrow_feature_control("heathrow_feature_control", "Heathrow Feature Control", 0x0, 0x0),
	heathrow_aux_control("heathrow_aux_control", "Heathrow Aux Control", 0x0, 0x0),
	
	// PCI configuration registers initialization
	pci_conf_device_id("pci_conf_device_id", "PCI config Device ID", 0x0, 0x0010), // Heathrow Mac I/O Controller
	pci_conf_vendor_id("pci_conf_vendor_id", "PCI config Vendor ID", 0x0, 0x106b), // Apple Computer Inc.
	pci_conf_status("pci_conf_status", "PCI Config Status", 0x0, 0x0),
	pci_conf_command("pci_conf_command", "PCI Config Command", 0x007f, 0x0),
	pci_conf_class_code("pci_conf_class_code", "PCI Config Class Code", 0x0, 0x0),
	pci_conf_revision_id("pci_conf_revision_id", "PCI Config Revision ID", 0x0, 0x0),
	pci_conf_bist("pci_conf_bist", "PCI Config BIST", 0x0, 0x0),
	pci_conf_header_type("pci_conf_header_type", "PCI Config Header Type", 0x0, 0x0),
	pci_conf_latency_timer("pci_conf_latency_timer", "PCI Config Latency Timer", 0x0, 0x0),
	pci_conf_cache_line_size("pci_conf_cache_line_size", "PCI Config Cache Line Size", 0x0, 0x0),

	pci_conf_carbus_cis_pointer("pci_conf_carbus_cis_pointer", "PCI Config Carbus CIS Pointer", 0x0, 0x0),
	pci_conf_subsystem_id("pci_conf_subsystem_id", "PCI Config Subsystem ID", 0x0, 0x0),
	pci_conf_subsystem_vendor_id("pci_conf_subsystem_vendor_id", "PCI Config Subsystem Vendor ID", 0x0, 0x0),

	pci_device_number(0),
	initial_base_addr(0),
	bus_frequency(0),
	pci_bus_frequency(33),
	level(false),
	
	// Parameters initialization
	param_verbose("verbose", this, verbose, "enable/disable verbosity"),
	param_initial_base_addr("initial-base-addr", this, initial_base_addr, "initial base address of memory space"),
	param_pci_device_number("pci-device-number", this, pci_device_number, "PCI device number"),
	param_bus_frequency("bus-frequency", this, bus_frequency, "bus frequency in Mhz"),
	param_pci_bus_frequency("pci-bus-frequency", this, pci_bus_frequency, "PCI bus frequency in Mhz")
{
	param_bus_frequency.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_pci_bus_frequency.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	
	unsigned int i;
	
	// Heathrow PIC registers initialization
	
	for(i = 0; i < NUM_PICS; i++)
	{
		stringstream sstr_name;
		stringstream sstr_long_name;
		
		sstr_name << "pic_event[" << i << "]";
		sstr_long_name << "PIC" << i << " Event";
		pic_event[i].Initialize(sstr_name.str().c_str(), sstr_long_name.str().c_str(), 0xffffffffUL, 0x0);
	}
	
	for(i = 0; i < NUM_PICS; i++)
	{
		stringstream sstr_name;
		stringstream sstr_long_name;
		
		sstr_name << "pic_enable[" << i << "]";
		sstr_long_name << "PIC" << i << " Enable";
		pic_enable[i].Initialize(sstr_name.str().c_str(), sstr_long_name.str().c_str(), 0xffffffffUL, 0x0);
	}

	for(i = 0; i < NUM_PICS; i++)
	{
		stringstream sstr_name;
		stringstream sstr_long_name;
		
		sstr_name << "pic_ack[" << i << "]";
		sstr_long_name << "PIC" << i << " Ack";
		pic_ack[i].Initialize(sstr_name.str().c_str(), sstr_long_name.str().c_str(), 0xffffffffUL, 0x0);
	}
	
	for(i = 0; i < NUM_PICS; i++)
	{
		stringstream sstr_name;
		stringstream sstr_long_name;
		
		sstr_name << "pic_level[" << i << "]";
		sstr_long_name << "PIC" << i << " Level";
		pic_level[i].Initialize(sstr_name.str().c_str(), sstr_long_name.str().c_str(), 0xffffffffUL, 0x0);
	}
	
	pic_level_mask[0] = 0;
	pic_level_mask[1] = HEATHROW_LEVEL_MASK;
}
	
template <class ADDRESS>
Heathrow<ADDRESS>::~Heathrow()
{
}
	
template <class ADDRESS>
void Heathrow<ADDRESS>::Reset()
{
	unsigned int i;
	
	// Reset Heathrow PIC registers
	
	for(i = 0; i < NUM_PICS; i++)
	{
		pic_event[i].Reset();
		pic_enable[i].Reset();
		pic_ack[i].Reset();
		pic_level[i].Reset();
	}
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
uint8_t Heathrow<ADDRESS>::ReadConfigByte(unsigned int offset)
{
	switch(offset)
	{
		case 0x00:
			return pci_conf_vendor_id;
			break;
		case 0x01:
			return pci_conf_vendor_id >> 8;
			break;
		case 0x02:
			return pci_conf_device_id;
			break;
		case 0x03:
			return pci_conf_device_id >> 8;
			break;
		case 0x04:
			return pci_conf_command;
			break;
		case 0x05:
			return pci_conf_command >> 8;
			break;
		case 0x06:
			return pci_conf_status;
			break;
		case 0x07:
			return pci_conf_status >> 8;
			break;
		case 0x08:
			return pci_conf_revision_id;
			break;
		case 0x09:
			return pci_conf_class_code;
			break;
		case 0x0a:
			return pci_conf_class_code >> 8;
			break;
		case 0x0b:
			return pci_conf_class_code >> 16;
			break;
		case 0x0c:
			return pci_conf_cache_line_size;
			break;
		case 0x0d:
			return pci_conf_latency_timer;
			break;
		case 0x0e:
			return pci_conf_header_type;
			break;
		case 0x0f:
			return pci_conf_bist;
			break;
		case 0x10:
			return pci_conf_base_addr;
			break;
		case 0x11:
			return pci_conf_base_addr >> 8;
			break;
		case 0x12:
			return pci_conf_base_addr >> 16;
			break;
		case 0x13:
			return pci_conf_base_addr >> 24;
			break;
		case 0x28:
			return 0xf1;
		case 0x29:
			return 0xff;
		case 0x2a:
			return 0xff;
		case 0x2b:
			return 0xff;
	}
	return 0x00;
}
	
template <class ADDRESS>
void Heathrow<ADDRESS>::WriteConfigByte(unsigned int offset, uint8_t value)
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
			pci_conf_base_addr = ((pci_conf_base_addr & 0xffffff00UL) == 0xffffff00UL && value == 0xff) ?
									(~MEMORY_SPACE_SIZE + 1) :
									(pci_conf_base_addr & 0xffffff00UL) | value;
			break;
		case 0x11:
			pci_conf_base_addr = ((pci_conf_base_addr & 0xffff00f0UL) == 0xffff00f0UL && value == 0xff) ?
									(~MEMORY_SPACE_SIZE + 1) :
									(pci_conf_base_addr & 0xffff00f0UL) | ((uint16_t) value << 8);
			break;
		case 0x12:
			pci_conf_base_addr = ((pci_conf_base_addr & 0xff00fff0UL) == 0xff00fff0UL && value == 0xff) ?
									(~MEMORY_SPACE_SIZE + 1) :
									(pci_conf_base_addr & 0xff00fff0UL) | ((uint16_t) value << 16);
			break;
		case 0x13:
			pci_conf_base_addr = ((pci_conf_base_addr & 0x00fffff0UL) == 0x00fffff0UL && value == 0xff) ?
									(~MEMORY_SPACE_SIZE + 1) :
									(pci_conf_base_addr & 0x00fffff0UL) | ((uint16_t) value << 24);
			break;
	}
}

template <class ADDRESS>
void Heathrow<ADDRESS>::Read(ADDRESS addr, void *buffer, uint32_t size)
{
	if(!(pci_conf_command & 0x2))
	{
		if(unlikely(verbose))
			logger << DebugWarning << "Attempting to read from memory space while it is disabled !" << std::endl << EndDebugWarning;
		memset(buffer, 0, size);
		return;
	}
	
	uint32_t offset = addr - (pci_conf_base_addr & 0xfffffff0UL);
	
	switch(size)
	{
		case 1:
			{
				uint8_t read_data;
				
				switch(offset)
				{
					case 0x32:
						read_data = heathrow_brightness;
						break;
					case 0x33:
						read_data = heathrow_contrast;
						break;
					default:
						read_data = 0;
						if(unlikely(verbose))
							logger << DebugWarning << "reading an unmapped memory (offset 0x" << std::hex << offset << std::dec << ")" << std::endl << EndDebugWarning;
				}
				*(uint8_t *) buffer = read_data;
			}
			break;

		case 4:
			{
				uint32_t read_data;
				
				switch(offset)
				{
					case 0x10:
						read_data = pic_event[0];
						break;
					case 0x14:
						read_data = pic_enable[0];
						break;
					case 0x18:
						read_data = pic_ack[0];
						break;
					case 0x1c:
						read_data = pic_level[0];
						break;
					case 0x20:
						read_data = pic_event[1];
						break;
					case 0x24:
						read_data = pic_enable[1];
						break;
					case 0x28:
						read_data = pic_ack[1];
						break;
					case 0x2c:
						read_data = pic_level[1];
						break;
					case 0x34:
						read_data = heathrow_media_bay_control;
						break;
					case 0x38:
						read_data = heathrow_feature_control;
						break;
					case 0x3c:
						read_data = heathrow_aux_control;
						break;
						
					default:
						read_data = 0;
						if(unlikely(verbose))
							logger << DebugWarning << "reading an unmapped memory (offset 0x" << std::hex << offset << std::dec << ")" << std::endl << EndDebugWarning;
				}
				*(uint32_t *) buffer = read_data;
			}
			break;
		default:
			if(unlikely(verbose))
				logger << DebugWarning << "bad size while reading" << std::endl << EndDebugWarning;
	}
}

template <class ADDRESS>
void Heathrow<ADDRESS>::Write(ADDRESS addr, const void *buffer, uint32_t size)
{
	if(!(pci_conf_command & 0x2))
	{
		if(unlikely(verbose))
			logger << DebugWarning << "Attempting to write into memory space while it is disabled !" << std::endl << EndDebugWarning;
		return;
	}
	
	uint32_t offset = addr - (pci_conf_base_addr & 0xfffffff0UL);
	
	switch(size)
	{
		case 1:
			{
				uint8_t write_data = *(uint8_t *) buffer;
					
				switch(offset)
				{
					case 0x32:
						heathrow_brightness = write_data;
						break;
					case 0x33:
						heathrow_contrast = write_data;
						break;
					default:
						if(unlikely(verbose))
							logger << DebugWarning << "writing an unmapped memory (offset 0x" << std::hex << offset << std::dec << ")" << std::endl << EndDebugWarning;
				}
			}
			break;
		case 4:
			{
				uint32_t write_data = LittleEndian2Host(*(uint32_t *) buffer);
				
				switch(offset)
				{
					case 0x10:
						pic_event[0] = write_data;
						break;
					case 0x14:
						pic_enable[0] = write_data;
						UpdatePIC(0);
						break;
					case 0x18:
						pic_ack[0] = write_data;
						write_data &= ~pic_level_mask[0];
						pic_event[0] = pic_event[0] & ~write_data;
						UpdatePIC(0);
						break;
					case 0x1c:
						pic_level[0] = write_data;
						break;
					case 0x20:
						pic_event[1] = write_data;
						break;
					case 0x24:
						pic_enable[1] = write_data;
						UpdatePIC(0);
						break;
					case 0x28:
						pic_ack[1] = write_data;
						write_data &= ~pic_level_mask[1];
						pic_event[1] = pic_event[1] & ~write_data;
						UpdatePIC(0);
						break;
					case 0x2c:
						pic_level[1] = write_data;
						break;
					case 0x34:
						heathrow_media_bay_control = write_data;
						break;
					case 0x38:
						heathrow_feature_control = write_data;
						break;
					case 0x3c:
						heathrow_aux_control = write_data;
						break;
					default:
						if(unlikely(verbose))
							logger << DebugWarning << "writing an unmapped memory (offset 0x" << std::hex << offset << std::dec << ")" << std::endl << EndDebugWarning;
				}
			}
			break;
		default:
			if(unlikely(verbose))
				logger << DebugWarning << "bad size while writing" << std::endl << EndDebugWarning;
	}
}
	
template <class ADDRESS>
void Heathrow<ADDRESS>::UpdatePIC(unsigned int pic_num)
{
	bool level = ((pic_event[pic_num] | (pic_level[pic_num] & pic_level_mask[pic_num])) && pic_enable[pic_num]) != 0;
	
//	if(level)
//	{
		TriggerInterrupt(level);
//	}
}

template <class ADDRESS>
void Heathrow<ADDRESS>::SetIRQ(unsigned int int_num, bool level)
{
	unsigned int irq_bit;
	unsigned int pic_num;
	
	pic_num = 1 - (int_num >> 5);
	irq_bit = 1 << (int_num & 0x1f);
	if(level)
	{
		pic_event[pic_num] = pic_event[pic_num] | (irq_bit & ~pic_level_mask[pic_num]);
		pic_level[pic_num] = pic_level[pic_num] | irq_bit;
	}
	else
	{
		pic_level[pic_num] = pic_level[pic_num] & ~irq_bit;
	}
	UpdatePIC(pic_num);
}
	
template <class ADDRESS>
bool Heathrow<ADDRESS>::BeginSetup()
{
	// PCI configuration registers initialization	
	pci_conf_base_addr.Initialize("pci_conf_base_addr", "PCI Config Base Address", 0xfffffff0UL, initial_base_addr);

	pci_conf_command = pci_conf_command | 0x2; // active memory space
	return true;
}

} // end of namespace macio
} // end of namespace pci
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
