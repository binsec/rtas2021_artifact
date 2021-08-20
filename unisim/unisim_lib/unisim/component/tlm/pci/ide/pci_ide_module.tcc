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

#ifndef __UNISIM_COMPONENT_TLM_PCI_IDE_PCI_IDE_MODULE_TCC__
#define __UNISIM_COMPONENT_TLM_PCI_IDE_PCI_IDE_MODULE_TCC__

#include <unisim/component/tlm/pci/ide/pci_dev.tcc>
#include <string.h>

namespace unisim {
namespace component {
namespace tlm {
namespace pci {
namespace ide {

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
PCIDevIde<ADDRESS_TYPE, MAX_DATA_SIZE>::PCIDevIde(const sc_core::sc_module_name &name, Object *parent): Object(name, parent, "PIIX4 IDE controller"),
	PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE>(name, parent),
	param_base_address("base-address", this, base_address, NUM_MAPPINGS, "initial base address of memory space"),
	param_size("size", this, size, NUM_MAPPINGS, "size in bytes of memory space"),
	param_register_number("register-number", this, register_number, NUM_MAPPINGS, "BAR offset in PCI configuration space"),
	param_device_number("device-number", this, device_number, "PCI device number"),
	param_disk_image("disk-image", this, disk_image, MAX_IMAGES, "Raw disk image filename"),
	param_disk_channel("disk-channel", this, disk_channel, MAX_IMAGES, "disk channel"),
	param_disk_num("disk-num", this, disk_num, MAX_IMAGES, "disk number (0=master 1=slave)")
{
}

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
PCIDevIde<ADDRESS_TYPE, MAX_DATA_SIZE>::~PCIDevIde()
{
}

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
bool PCIDevIde<ADDRESS_TYPE, MAX_DATA_SIZE>::BeginSetup () {
	std::vector<unisim::component::cxx::pci::ide::IdeDisk<ADDRESS_TYPE> *> disks;
	unisim::component::cxx::pci::ide::PciConfigData *pci_config_data;
	typename unisim::component::cxx::pci::ide::IdeController<ADDRESS_TYPE>::Params *ide_ctrl_params;
	
	for(unsigned int i = 0; i < MAX_IMAGES; i++) {
		if (strcmp(disk_image[i].c_str(), "") != 0) {
			unisim::component::cxx::pci::ide::DiskImage *image = new unisim::component::cxx::pci::ide::RawDiskImage("diskImage" + i,
					disk_image[i],
					false);	
			
			unisim::component::cxx::pci::ide::IdeDisk<ADDRESS_TYPE> *disk = new unisim::component::cxx::pci::ide::IdeDisk<ADDRESS_TYPE>("myDisk", image, disk_channel[i], 0, disk_num[i]);
		
			disks.resize(disks.size() + 1);
			disks[i] = disk;
		}
	}

	pci_config_data = new unisim::component::cxx::pci::ide::PciConfigData("pciConfigData");

	for(unsigned int i = 0; i < NUM_MAPPINGS; i++) {
		pci_config_data->BARSize[i] = size[i];//BARSize;
		//*((uint32_t*)&pci_config_data->config.data[register_number[i]]) = (uint32_t)base_address[i]; 
		memcpy(&pci_config_data->config.data[register_number[i]], &base_address[i], sizeof(uint32_t));
	}
	
	pci_config_data->config.vendor = 32902;//htole(VendorID);
	pci_config_data->config.device = 28945;//htole(DeviceID);
	pci_config_data->config.command = 0;//htole(Command);
	pci_config_data->config.status = 640;//htole(Status);
	pci_config_data->config.revision = 0;//htole(Revision);
	pci_config_data->config.progIF = 133;//htole(ProgIF);
	pci_config_data->config.subClassCode = 1;//htole(SubClassCode);
	pci_config_data->config.classCode = 1;//htole(ClassCode);
	pci_config_data->config.cacheLineSize = 0;//htole(CacheLineSize);
	pci_config_data->config.latencyTimer = 0;//htole(LatencyTimer);
	pci_config_data->config.headerType = 0;//htole(HeaderType);
	pci_config_data->config.bist = 0;//htole(BIST); 
	pci_config_data->config.cardbusCIS = 0;//htole(CardbusCIS);
	pci_config_data->config.subsystemVendorID = 0;//htole(SubsystemVendorID);
	pci_config_data->config.subsystemID = 0;//htole(SubsystemVendorID);
	pci_config_data->config.expansionROM = 0;//htole(ExpansionROM);
	pci_config_data->config.interruptLine = 47;//htole(InterruptLine);
	pci_config_data->config.interruptPin = 1;//htole(InterruptPin);
	pci_config_data->config.minimumGrant = 0;//htole(MinimumGrant);
	pci_config_data->config.maximumLatency = 0;//htole(MaximumLatency);


	ide_ctrl_params = new typename unisim::component::cxx::pci::ide::IdeController<ADDRESS_TYPE>::Params;

	ide_ctrl_params->name = "ideController";
	ide_ctrl_params->configData = pci_config_data;
	ide_ctrl_params->busNum = 0;
	ide_ctrl_params->deviceNum = device_number;
	ide_ctrl_params->functionNum = 0;
	ide_ctrl_params->disks = disks;

	inherited::pciDev = new unisim::component::cxx::pci::ide::IdeController<ADDRESS_TYPE>(ide_ctrl_params);
	return inherited::BeginSetup();
}

} // end of namespace ide
} // end of namespace pci
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif
