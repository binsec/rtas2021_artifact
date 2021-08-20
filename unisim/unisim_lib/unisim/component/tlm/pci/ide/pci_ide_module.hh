/*
 *  Copyright (c) 2007,
 *  Universitat Politecnica de Catalunya (UPC)
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
 *   - Neither the name of UPC nor the names of its contributors may be used to
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
 * Authors: Paula Casero (pcasero@upc.edu), Alejandro Schenzle (schenzle@upc.edu)
 */

#ifndef __UNISIM_COMPONENT_TLM_PCI_IDE_PCI_IDE_MODULE_HH__
#define __UNISIM_COMPONENT_TLM_PCI_IDE_PCI_IDE_MODULE_HH__

#include <unisim/component/cxx/pci/ide/disk_image.hh>
#include <unisim/component/cxx/pci/ide/ide_disk.hh>
#include <unisim/component/cxx/pci/ide/ide_ctrl.hh>
#include <unisim/component/tlm/pci/ide/pci_dev.hh>

namespace unisim {
namespace component {
namespace tlm {
namespace pci {
namespace ide {
	
template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
class PCIDevIde: virtual public PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE>
{
private:
	static const unsigned int NUM_MAPPINGS = 5;
	static const unsigned int MAX_IMAGES = 3;
	typedef PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE> inherited;
	ADDRESS_TYPE base_address[NUM_MAPPINGS];
	ADDRESS_TYPE size[NUM_MAPPINGS];
	uint32_t register_number[NUM_MAPPINGS];
	string disk_image[MAX_IMAGES];
	uint32_t disk_channel[MAX_IMAGES];
	uint32_t disk_num[MAX_IMAGES];
	uint32_t device_number;
		
	ParameterArray<ADDRESS_TYPE> param_base_address;
	ParameterArray<ADDRESS_TYPE> param_size;
	ParameterArray<uint32_t> param_register_number;
	
	Parameter<uint32_t> param_device_number;
	
	ParameterArray<string> param_disk_image;
	ParameterArray<uint32_t> param_disk_channel;
	ParameterArray<uint32_t> param_disk_num;
	
	public:
		
	PCIDevIde(const sc_core::sc_module_name &name, Object *parent = 0);
	~PCIDevIde();
	virtual bool BeginSetup ();
};

} // end of namespace ide
} // end of namespace pci
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif
