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

#ifndef __UNISIM_COMPONENT_CXX_PCI_MACIO_HEATHROW_HH__
#define __UNISIM_COMPONENT_CXX_PCI_MACIO_HEATHROW_HH__

#include <unisim/kernel/variable/variable.hh>
#include <unisim/util/device/register.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/logger.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace pci {
namespace macio {

using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceExport;
using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::util::device::Register;
using unisim::kernel::Client;
using unisim::kernel::ServiceImport;
using unisim::kernel::Object;

template <class ADDRESS>
class Heathrow : virtual public Object
{
public:
	typedef Register<uint32_t> reg32_t;
	typedef Register<uint32_t> reg24_t;
	typedef Register<uint16_t> reg16_t;
	typedef Register<uint8_t> reg8_t;
	
	static const unsigned int NUM_PICS = 2;
	static const unsigned int NUM_IRQS = NUM_PICS * 32;
	static const uint32_t MEMORY_SPACE_SIZE = 512 * 1024; // 512 KB
	static const uint32_t HEATHROW_LEVEL_MASK = 0x1ff00000UL;

	Heathrow(const char *name, Object *parent = 0);
	virtual ~Heathrow();
	void Reset();
	uint8_t ReadConfigByte(unsigned int offset);
	void WriteConfigByte(unsigned int offset, uint8_t value);
	void Read(ADDRESS addr, void *buffer, uint32_t size);
	void Write(ADDRESS addr, const void *buffer, uint32_t size);
	virtual void TriggerInterrupt(bool in_level) = 0; // Callback
	void UpdatePIC(unsigned int pic_num);
	void SetIRQ(unsigned int int_num, bool level);
	virtual bool BeginSetup();
protected:
	// debug stuff
	unisim::kernel::logger::Logger logger;
	bool verbose;

	// heathrow global registers
	reg8_t heathrow_brightness;
	reg8_t heathrow_contrast;
	reg32_t heathrow_media_bay_control;
	reg32_t heathrow_feature_control;
	reg32_t heathrow_aux_control;
	
	// PCI configuration registers
	reg16_t pci_conf_device_id;
	reg16_t pci_conf_vendor_id;
	reg16_t pci_conf_status;
	reg16_t pci_conf_command;
	reg24_t pci_conf_class_code;
	reg8_t pci_conf_revision_id;
	reg8_t pci_conf_bist;
	reg8_t pci_conf_header_type;
	reg8_t pci_conf_latency_timer;
	reg8_t pci_conf_cache_line_size;
	reg32_t pci_conf_base_addr;

	reg32_t pci_conf_carbus_cis_pointer;
	reg16_t pci_conf_subsystem_id;
	reg16_t pci_conf_subsystem_vendor_id;
	
	// Heathrow PIC registers
	reg32_t pic_event[NUM_PICS];
	reg32_t pic_enable[NUM_PICS];
	reg32_t pic_ack[NUM_PICS];
	reg32_t pic_level[NUM_PICS];
	
	uint32_t pci_device_number;
	ADDRESS initial_base_addr;
	unsigned int bus_frequency; // in Mhz
	unsigned int pci_bus_frequency; // in Mhz
	
	bool level;
	
	uint32_t pic_level_mask[NUM_PICS];
	
	Parameter<bool> param_verbose;
	Parameter<ADDRESS> param_initial_base_addr;
	Parameter<uint32_t> param_pci_device_number;
	Parameter<unsigned int> param_bus_frequency;
	Parameter<unsigned int> param_pci_bus_frequency;
};

} // end of namespace macio
} // end of namespace pci
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
