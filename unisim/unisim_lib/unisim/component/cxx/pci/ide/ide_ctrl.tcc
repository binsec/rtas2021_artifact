/*
 *  Copyright (c) 2007,
 *  Universitat Politecnica de Catalunya (UPC)
 *  All rights reserved.
 *
 *  This file was created by Paula Casero and Alejandro Schenzle based on m5 simulator and therefore
retaining the original license:

 * Copyright (c) 2004, 2005
 * The Regents of The University of Michigan
 * All Rights Reserved
 *
 * This code is part of the M5 simulator, developed by Nathan Binkert,
 * Erik Hallnor, Steve Raasch, and Steve Reinhardt, with contributions
 * from Ron Dreslinski, Dave Greene, Lisa Hsu, Kevin Lim, Ali Saidi,
 * and Andrew Schultz.
 *
 * Permission is granted to use, copy, create derivative works and
 * redistribute this software and such derivative works for any
 * purpose, so long as the copyright notice above, this grant of
 * permission, and the disclaimer below appear in all copies made; and
 * so long as the name of The University of Michigan is not used in
 * any advertising or publicity pertaining to the use or distribution
 * of this software without specific, written prior authorization.
 *
 * THIS SOFTWARE IS PROVIDED AS IS, WITHOUT REPRESENTATION FROM THE
 * UNIVERSITY OF MICHIGAN AS TO ITS FITNESS FOR ANY PURPOSE, AND
 * WITHOUT WARRANTY BY THE UNIVERSITY OF MICHIGAN OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE. THE REGENTS OF THE UNIVERSITY OF MICHIGAN SHALL NOT BE
 * LIABLE FOR ANY DAMAGES, INCLUDING DIRECT, SPECIAL, INDIRECT,
 * INCIDENTAL, OR CONSEQUENTIAL DAMAGES, WITH RESPECT TO ANY CLAIM
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OF THE SOFTWARE, EVEN
 * IF IT HAS BEEN OR IS HEREAFTER ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGES.
 */

#include <cstddef>
#include <cassert>
#include <cstdlib>
#include <string>
#include <vector>
#include <string.h>

#include "unisim/component/cxx/pci/ide/ide_ctrl.hh"
#include "unisim/component/cxx/pci/ide/ide_disk.hh"
#include "unisim/component/cxx/pci/ide/pcireg.hh"
#include "unisim/util/endian/endian.hh"
#include "unisim/component/cxx/pci/ide/pcidev.tcc"

using namespace std;

namespace unisim {
namespace component {
namespace cxx {
namespace pci {
namespace ide {

////
// Initialization and destruction
////

template <class ADDRESS_TYPE>
IdeController<ADDRESS_TYPE>::IdeController(IdeController<ADDRESS_TYPE>::Params *p)
//    :sc_module(p->name), PciDev(p)
    : PciDev<ADDRESS_TYPE>(p)
{
    // initialize the PIO interface addresses
    pri_cmd_addr = 0;
    pri_cmd_size = inherited::BARSize[0];

    pri_ctrl_addr = 0;
    pri_ctrl_size = inherited::BARSize[1];

    sec_cmd_addr = 0;
    sec_cmd_size = inherited::BARSize[2];

    sec_ctrl_addr = 0;
    sec_ctrl_size = inherited::BARSize[3];

    // initialize the bus master interface (BMI) address to be configured
    // via PCI
    bmi_addr = 0;
    bmi_size = inherited::BARSize[4];

    // zero out all of the registers
    memset(bmi_regs.data, 0, sizeof(bmi_regs));
    memset(config_regs.data, 0, sizeof(config_regs.data));

    // setup initial values
    // enable both channels
    config_regs.idetim0 = Host2LittleEndian((uint16_t)IDETIM_DECODE_EN);
    config_regs.idetim1 = Host2LittleEndian((uint16_t)IDETIM_DECODE_EN);
    bmi_regs.bmis0 = DMA1CAP | DMA0CAP;
    bmi_regs.bmis1 = DMA1CAP | DMA0CAP;

    // reset all internal variables
    io_enabled = false;
    bm_enabled = false;
    memset(cmd_in_progress, 0, sizeof(cmd_in_progress));

    // setup the disks attached to controller
    memset(disks, 0, sizeof(disks));
    dev[0] = 0;
    dev[1] = 0;

    if (params()->disks.size() > 3) {
        soft_panic("IDE controllers support a maximum of 4 devices attached!\n");
        
    } else {
    	for (unsigned int i = 0; i < params()->disks.size(); i++) {
        	disks[i] = params()->disks[i]; //[pau] comment la siguiente linea en caso de usar wrappers
			//disks[i]->setController(this, dmaInterface);
			disks[i]->setController(this);//, NULL);
    	}
	}
}

template <class ADDRESS_TYPE>
IdeController<ADDRESS_TYPE>::~IdeController()
{
    for (int i = 0; i < 4; i++)
        if (disks[i])
            delete disks[i];
}

template <class ADDRESS_TYPE>
const typename IdeController<ADDRESS_TYPE>::Params *IdeController<ADDRESS_TYPE>::params() const { 
	return (const typename IdeController<ADDRESS_TYPE>::Params *) inherited::_params; 
}

////
// Utility functions
///

template <class ADDRESS_TYPE>
void 
IdeController<ADDRESS_TYPE>::parseAddr(const ADDRESS_TYPE &addr, uint32_t &offset, IdeChannel &channel, 
                         IdeRegType &reg_type)
{
    offset = addr & 0xffffff; // we take the last 24 bits

    if (offset >= pri_cmd_addr && offset < (pri_cmd_addr + pri_cmd_size)) {
        offset -= pri_cmd_addr;
        reg_type = COMMAND_BLOCK;
        channel = PRIMARY;
    } else if (offset >= pri_ctrl_addr && 
               offset < (pri_ctrl_addr + pri_ctrl_size)) {
        offset -= pri_ctrl_addr;
        reg_type = CONTROL_BLOCK;
        channel = PRIMARY;
    } else if (offset >= sec_cmd_addr &&
               offset < (sec_cmd_addr + sec_cmd_size)) {
        offset -= sec_cmd_addr;
        reg_type = COMMAND_BLOCK;
        channel = SECONDARY;
    } else if (offset >= sec_ctrl_addr &&
               offset < (sec_ctrl_addr + sec_ctrl_size)) {
        offset -= sec_ctrl_addr;
        reg_type = CONTROL_BLOCK;
        channel = SECONDARY;
    } else if (offset >= bmi_addr && offset < (bmi_addr + bmi_size)) {
        offset -= bmi_addr;
        reg_type = BMI_BLOCK;
        channel = (offset < BMIC1) ? PRIMARY : SECONDARY;
    } else {
        soft_panic2("IDE controller access to invalid address: ", offset);
    }   
}

template <class ADDRESS_TYPE>
int 
IdeController<ADDRESS_TYPE>::getDisk(IdeChannel channel) 
{
    int disk = 0;
    uint8_t *devBit = &dev[0];
    
    if (channel == SECONDARY) {
        disk += 2;
        devBit = &dev[1];
    }
    
    disk += *devBit;
    
    assert(*devBit == 0 || *devBit == 1);
    
    return disk;
}

template <class ADDRESS_TYPE>
bool
IdeController<ADDRESS_TYPE>::isDiskSelected(uint8_t diskId)
{
	return dev[diskId/2] == diskId%2;
}
////
// Command completion
////

template <class ADDRESS_TYPE>
bool
IdeController<ADDRESS_TYPE>::setDmaComplete(uint8_t diskNum)
{
	if (!disks[diskNum])
		panic2("Unable to find disk based on pointer %i\n", diskNum);

    if (diskNum < 2) {
        // clear the start/stop bit in the command register
        bmi_regs.bmic0 &= ~SSBM;
        // clear the bus master active bit in the status register
        bmi_regs.bmis0 &= ~BMIDEA;
        // set the interrupt bit
        bmi_regs.bmis0 |= IDEINTS;
    } else {
        // clear the start/stop bit in the command register
        bmi_regs.bmic1 &= ~SSBM;
        // clear the bus master active bit in the status register
        bmi_regs.bmis1 &= ~BMIDEA;
        // set the interrupt bit
        bmi_regs.bmis1 |= IDEINTS;
    }
    return true;
}

template <class ADDRESS_TYPE>
void IdeController<ADDRESS_TYPE>::setInterruptBit() 
{
	bmi_regs.bmis0 |= IDEINTS;
}

////
// Read and write handling
////

template <class ADDRESS_TYPE>
bool
IdeController<ADDRESS_TYPE>::readConfig(ADDRESS_TYPE offset, int size, uint8_t *data)
{
    int config_offset;
	//cerr << "offset: " << offset << endl;
    if (offset < PCI_DEVICE_SPECIFIC) {
        PciDev<ADDRESS_TYPE>::readConfig(offset, size, data);
    } else if (offset >= IDE_CTRL_CONF_START &&
               (offset + size) <= IDE_CTRL_CONF_END) {

        config_offset = offset - IDE_CTRL_CONF_START;

        switch (size) {
          case sizeof(uint8_t): {
            *data = config_regs.data[config_offset];
	        //DPRINTF(IdeCtrl, "PCI read offset: %#x size: %#x data: %#x\n",
            //    offset, size, (uint32_t)*(uint8_t*)data);
			}
            break;
          case sizeof(uint16_t): {
            //*(uint16_t*)data = *(uint16_t*)&config_regs.data[config_offset];
			memcpy(data, &config_regs.data[config_offset], sizeof(uint16_t));
	        //DPRINTF(IdeCtrl, "PCI read offset: %#x size: %#x data: %#x\n",
            //    offset, size, *(uint16_t*)data);
			}
            break;
          case sizeof(uint32_t): {
            //*(uint32_t*)data = *(uint32_t*)&config_regs.data[config_offset]; 
			memcpy(data, &config_regs.data[config_offset], sizeof(uint32_t));
        	//DPRINTF(IdeCtrl, "PCI read offset: %#x size: %#x data: %#x\n",
            //    offset, size, *(uint32_t*)data);
			}
            break;
          default:
            panic("Invalid PCI configuration read size!\n");
        }

    } else {
		memset(data, 0xff, size);
        soft_panic2("Read of unimplemented PCI config. register: ", offset);
    }
    return true;
}

template <class ADDRESS_TYPE>
bool
IdeController<ADDRESS_TYPE>::writeConfig(ADDRESS_TYPE offset, int size, const uint8_t *data)
{
    int config_offset;

    if (offset < PCI_DEVICE_SPECIFIC) {
        PciDev<ADDRESS_TYPE>::writeConfig(offset, size, data);
    } else if (offset >= IDE_CTRL_CONF_START &&
               (offset + size) <= IDE_CTRL_CONF_END) {

        config_offset = offset - IDE_CTRL_CONF_START;

        switch(size) {
          case sizeof(uint8_t):
            config_regs.data[config_offset] = *data;
            break;
          case sizeof(uint16_t):
            //*(uint16_t*)&config_regs.data[config_offset] = *(uint16_t*)data;
			memcpy(&config_regs.data[config_offset], data, sizeof(uint16_t));
            break;
          case sizeof(uint32_t):
            //*(uint32_t*)&config_regs.data[config_offset] = *(uint32_t*)data;
			memcpy(&config_regs.data[config_offset], data, sizeof(uint32_t));
            break;
          default:
            panic("Invalid PCI configuration write size!\n");
        }
    } else {
        soft_panic2("Write of unimplemented PCI config. register: ", offset);
    }
/*	if (size == 1) {
    	DPRINTF(IdeCtrl, "PCI write offset: %#x size: %#x data: %#x\n",
            offset, size, (uint32_t)*(uint8_t*)data);
	}
	else if (size ==2) {
		DPRINTF(IdeCtrl, "PCI write offset: %#x size: %#x data: %#x\n",
            offset, size, *(uint16_t*)data);
	}
	else {
		DPRINTF(IdeCtrl, "PCI write offset: %#x size: %#x data: %#x\n",
        offset, size, *(uint32_t*)data);
	}*/
		
    // Catch the writes to specific PCI registers that have side affects
    // (like updating the PIO ranges)
	uint32_t bar_mask;

    switch (offset) {
      case PCI_COMMAND:
        if (Host2LittleEndian(inherited::config.command) & PCI_CMD_IOSE)
            io_enabled = true;
        else
            io_enabled = false;
        
        if (Host2LittleEndian(inherited::config.command) & PCI_CMD_BME)
            bm_enabled = true;
        else
            bm_enabled = false;
        break;
        
      case PCI0_BASE_ADDR0:
        if (Host2LittleEndian(inherited::config.baseAddr[0]) != 0) {
            	if (BAR_IO_SPACE(Host2LittleEndian(inherited::config.baseAddr[0]))) {
                	bar_mask = BAR_IO_MASK;
                	//space_base = TSUNAMI_PCI0_IO;
            	} else {
                	bar_mask = BAR_MEM_MASK;
                	//space_base = TSUNAMI_PCI0_MEMORY;
            	}

            pri_cmd_addr = Host2LittleEndian(inherited::config.baseAddr[0]) & ~bar_mask;
/*            if (pioInterface)
                pioInterface->addAddrRange(RangeSize(pri_cmd_addr,
						     pri_cmd_size));
   */         
           // pri_cmd_addr &= EV5::PAddrUncachedMask; // this has no sense any more, pau
        }
        break;
        
      case PCI0_BASE_ADDR1:
        if (Host2LittleEndian(inherited::config.baseAddr[1]) != 0) {
            	if (BAR_IO_SPACE(Host2LittleEndian(inherited::config.baseAddr[1]))) {
                	bar_mask = BAR_IO_MASK;
                	//space_base = TSUNAMI_PCI0_IO;
            	} else {
                	bar_mask = BAR_MEM_MASK;
                	//space_base = TSUNAMI_PCI0_MEMORY;
            	}

            pri_ctrl_addr = Host2LittleEndian(inherited::config.baseAddr[1]) & ~bar_mask;
/*            if (pioInterface)
                pioInterface->addAddrRange(RangeSize(pri_ctrl_addr, 
						     pri_ctrl_size));
   */         
            //pri_ctrl_addr &= EV5::PAddrUncachedMask; // this has no sense anymore since now we deal with 32 bit addr here
        }
        break;

      case PCI0_BASE_ADDR2:
        if (Host2LittleEndian(inherited::config.baseAddr[2]) != 0) {
            	if (BAR_IO_SPACE(Host2LittleEndian(inherited::config.baseAddr[2]))) {
                	bar_mask = BAR_IO_MASK;
                	//space_base = TSUNAMI_PCI0_IO;
            	} else {
                	bar_mask = BAR_MEM_MASK;
                	//space_base = TSUNAMI_PCI0_MEMORY;
            	}
            sec_cmd_addr = Host2LittleEndian(inherited::config.baseAddr[2]) & ~bar_mask;
    /*        if (pioInterface)
                pioInterface->addAddrRange(RangeSize(sec_cmd_addr, 
						     sec_cmd_size));
      */      
            //sec_cmd_addr &= EV5::PAddrUncachedMask;
        }
        break;

      case PCI0_BASE_ADDR3:
        if (Host2LittleEndian(inherited::config.baseAddr[3]) != 0) {
            	if (BAR_IO_SPACE(Host2LittleEndian(inherited::config.baseAddr[3]))) {
                	bar_mask = BAR_IO_MASK;
                	//space_base = TSUNAMI_PCI0_IO;
            	} else {
                	bar_mask = BAR_MEM_MASK;
                	//space_base = TSUNAMI_PCI0_MEMORY;
            	}
    		sec_ctrl_addr = Host2LittleEndian(inherited::config.baseAddr[3]) & ~bar_mask;
/*            if (pioInterface)
                pioInterface->addAddrRange(RangeSize(sec_ctrl_addr, 
						     sec_ctrl_size));
   */         
            //sec_ctrl_addr &= EV5::PAddrUncachedMask;
        }
        break;

      case PCI0_BASE_ADDR4:
	        if (Host2LittleEndian(inherited::config.baseAddr[4]) != 0) {
            	if (BAR_IO_SPACE(Host2LittleEndian(inherited::config.baseAddr[4]))) {
                	bar_mask = BAR_IO_MASK;
                	//space_base = TSUNAMI_PCI0_IO;
            	} else {
                	bar_mask = BAR_MEM_MASK;
                	//space_base = TSUNAMI_PCI0_MEMORY;
            	}

            bmi_addr = Host2LittleEndian(inherited::config.baseAddr[4]) & ~bar_mask;
/*            if (pioInterface)
                pioInterface->addAddrRange(RangeSize(bmi_addr, bmi_size));
  */          
            //bmi_addr &= EV5::PAddrUncachedMask;
        }
        break;
    }
    return true;
}
/*
Fault
IdeController<ADDRESS_TYPE>::read(MemReqPtr &req, uint8_t *data)
{
	ReqData r = ReqData(req->paddr, req->size);
	return read(r, data);
}

Fault
IdeController<ADDRESS_TYPE>::write(MemReqPtr &req, const uint8_t *data)
{
	ReqData r = ReqData(req->paddr, req->size);
	return write(r, data);
}*/

template <class ADDRESS_TYPE>
bool
IdeController<ADDRESS_TYPE>::readMem(ADDRESS_TYPE addr, int size, uint8_t *data) {
	
    uint32_t offset;
    IdeChannel channel;
    IdeRegType reg_type;
    int disk;

    parseAddr(addr, offset, channel, reg_type);

    if (!io_enabled)
        return true;

    switch (reg_type) {
      case BMI_BLOCK:
        switch (size) {
          case sizeof(uint8_t):
            *(uint8_t*)data = *(uint8_t*)&bmi_regs.data[offset];
            break;
          case sizeof(uint16_t):
            //*(uint16_t*)data = *(uint16_t*)&bmi_regs.data[offset];
			memcpy(data, &bmi_regs.data[offset], sizeof(uint16_t));
            break;
          case sizeof(uint32_t):
            //*(uint32_t*)data = *(uint32_t*)&bmi_regs.data[offset];        
			memcpy(data, &bmi_regs.data[offset], sizeof(uint32_t));
            break;
          default:
            panic2("IDE read of BMI reg invalid size: %#x\n", size);
        }
        break;

      case COMMAND_BLOCK:
      case CONTROL_BLOCK:
        disk = getDisk(channel);

        if (disks[disk] == NULL) {
			*data = 0x00;
            break;
		}

        switch (offset) {
          case DATA_OFFSET:
            switch (size) {
              case sizeof(uint16_t):
                disks[disk]->read(offset, reg_type, data, 2);
                break;

              case sizeof(uint32_t):
                disks[disk]->read(offset, reg_type, data, 2);
                disks[disk]->read(offset, reg_type, &data[2], 2);
                break;

              default:
                panic2("IDE read of data reg invalid size: %#x\n", size);
            }
            break;
          default:
            if (size == sizeof(uint8_t)) {
               return disks[disk]->read(offset, reg_type, data, 1);
            } else
                panic2("IDE read of command reg of invalid size: %#x\n", size);
        }
        break;
      default:
        panic("IDE controller read of unknown register block type!\n");
    }

//	if (size==1) {
//    	DPRINTF(IdeCtrl, "read from offset: %#x size: %#x data: %#x\n",
//            offset, size, (uint32_t)*(uint8_t*)data);
//	} else if (size==2) {
//		DPRINTF(IdeCtrl, "read from offset: %#x size: %#x data: %#x\n",
//            offset, size, *(uint16_t*)data);
//		} else { DPRINTF(IdeCtrl, "read from offset: %#x size: %#x data: %#x\n",
//            offset, size, *(uint32_t*)data);
//            
//		}
  /*  if (size == 1)
		cerr << "read ide_ctrl dir: " <<  hex << addr << "size: " << size << "data: " << hex << *(uint8_t*)data << endl;
    if (size == 2)
		cerr << "read ide_ctrl dir: " <<  hex << addr << "size: " << size << "data: " << hex << *(uint16_t*)data << endl;
    if (size == 4)
		cerr << "read ide_ctrl dir: " <<  hex << addr << "size: " << size << "data: " << hex << *(uint32_t*)data << endl; */
    return true;
}

template <class ADDRESS_TYPE>
bool
IdeController<ADDRESS_TYPE>::readIO(ADDRESS_TYPE addr, int size, uint8_t *data) {
	return readMem(addr, size, data);
}

template <class ADDRESS_TYPE>
bool
IdeController<ADDRESS_TYPE>::writeIO(ADDRESS_TYPE addr, int size, const uint8_t *data) {
	return writeMem(addr, size, data);
}

template <class ADDRESS_TYPE>
bool
IdeController<ADDRESS_TYPE>::writeMem(ADDRESS_TYPE addr, int size, const uint8_t *data) {

    uint32_t offset;
    IdeChannel channel;
    IdeRegType reg_type;
    int disk;
    uint8_t oldVal, newVal;

    parseAddr(addr, offset, channel, reg_type);
    
    if (!io_enabled)
        return true;

    switch (reg_type) {
      case BMI_BLOCK:
        if (!bm_enabled)
            return true;

        switch (offset) {
            // Bus master IDE command register
          case BMIC1:
          case BMIC0:
            if (size != sizeof(uint8_t))
                panic2("Invalid BMIC write size: %x\n", size);

            // select the current disk based on DEV bit
            disk = getDisk(channel);

            oldVal = bmi_regs.chan[channel].bmic;
            newVal = *data;

            // if a DMA transfer is in progress, R/W control cannot change
            if (oldVal & SSBM) {
                if ((oldVal & RWCON) ^ (newVal & RWCON)) {
                    (oldVal & RWCON) ? newVal |= RWCON : newVal &= ~RWCON;
                }
            }

            // see if the start/stop bit is being changed
            if ((oldVal & SSBM) ^ (newVal & SSBM)) {
                if (oldVal & SSBM) {
                    // stopping DMA transfer
                    fprintf(stderr, "Stopping DMA transfer\n");
                    
                    // clear the BMIDEA bit
                    bmi_regs.chan[channel].bmis = 
		        bmi_regs.chan[channel].bmis & ~BMIDEA;
                    
                    if (disks[disk] == NULL)
                        panic2("DMA stop for disk %d which does not exist\n", 
                              disk);

                    // inform the disk of the DMA transfer abort
                    disks[disk]->abortDma();
                } else {
                    // starting DMA transfer
                    fprintf(stderr,  "Starting DMA transfer\n");
                    
                    // set the BMIDEA bit
                    bmi_regs.chan[channel].bmis = 
                        bmi_regs.chan[channel].bmis | BMIDEA;
                    
                    if (disks[disk] == NULL)
                        panic2("DMA start for disk %d which does not exist\n", 
                              disk);

                    // inform the disk of the DMA transfer start
                    disks[disk]->startDma(Host2LittleEndian(bmi_regs.chan[channel].bmidtp));
                }
            }

            // update the register value
            bmi_regs.chan[channel].bmic = newVal;
            break;

            // Bus master IDE status register
          case BMIS0:
          case BMIS1:
            if (size != sizeof(uint8_t))
                panic2("Invalid BMIS write size: %x\n", size);
            
            oldVal = bmi_regs.chan[channel].bmis;
            newVal = *data;
            
            // the BMIDEA bit is RO
            newVal |= (oldVal & BMIDEA);
            
            // to reset (set 0) IDEINTS and IDEDMAE, write 1 to each
            if ((oldVal & IDEINTS) && (newVal & IDEINTS))
                newVal &= ~IDEINTS; // clear the interrupt?
            else
                (oldVal & IDEINTS) ? newVal |= IDEINTS : newVal &= ~IDEINTS;
            
            if ((oldVal & IDEDMAE) && (newVal & IDEDMAE))
                newVal &= ~IDEDMAE;
            else
                (oldVal & IDEDMAE) ? newVal |= IDEDMAE : newVal &= ~IDEDMAE;
            
            bmi_regs.chan[channel].bmis = newVal;
            break;

            // Bus master IDE descriptor table pointer register
          case BMIDTP0:
          case BMIDTP1:
	    {
	        if (size != sizeof(uint32_t))
		    panic2("Invalid BMIDTP write size: %x\n", size);

		uint32_t host_data =  Host2LittleEndian(*(uint32_t*)data);
		host_data &= ~0x3;
		bmi_regs.chan[channel].bmidtp = Host2LittleEndian(host_data);
	    }
            break;

          default:
            if (size != sizeof(uint8_t) &&
                size != sizeof(uint16_t) &&
                size != sizeof(uint32_t))
                panic2("IDE controller write of invalid write size: %x\n", 
                      size);
            
            // do a default copy of data into the registers
            memcpy(&bmi_regs.data[offset], data, size);
        }
        break;
      case COMMAND_BLOCK:
        if (offset == IDE_SELECT_OFFSET) {
            uint8_t *devBit = &dev[channel];
            *devBit = (Host2LittleEndian(*data) & IDE_SELECT_DEV_BIT) ? 1 : 0;
        }
        // fall-through ok!
      case CONTROL_BLOCK:
        disk = getDisk(channel);

        if (disks[disk] == NULL)
            break;

        switch (offset) {
          case DATA_OFFSET:
            switch (size) {
              case sizeof(uint16_t):
                disks[disk]->write(offset, reg_type, data, 2);
                break;

              case sizeof(uint32_t):
                disks[disk]->write(offset, reg_type, data, 2);
                disks[disk]->write(offset, reg_type, &data[2], 2);
                break;
              default:
                panic2("IDE write of data reg invalid size: %#x\n", size);
            }
            break;
          default:
            if (size == sizeof(uint8_t)) {
                disks[disk]->write(offset, reg_type, data, 1);
            } else
                panic2("IDE write of command reg of invalid size: %#x\n", size);
        }
        break;
      default:
        panic("IDE controller write of unknown register block type!\n");
    }
//	if (size == 1) {
//    	DPRINTF(IdeCtrl, "write to offset: %#x size: %#x data: %#x\n",
//            offset, size, (uint32_t)*(uint8_t*)data);
//	}
//	else if (size==2) { 
//		DPRINTF(IdeCtrl, "write from offset: %#x size: %#x data: %#x\n",
//            offset, size, *(uint16_t*)data);
//	}
//		else { 
//			DPRINTF(IdeCtrl, "write from offset: %#x size: %#x data: %#x\n",
//            offset, size, *(uint32_t*)data);
//		}
    /* if (size == 1)
		cerr << "write ide_ctrl dir: " <<  hex << addr << "size: " << size << "data: " << hex << *(uint8_t*)data << endl;
    if (size == 2)
		cerr << "write ide_ctrl dir: " <<  hex << addr << "size: " << size << "data: " << hex << *(uint16_t*)data << endl;
    if (size == 4)
		cerr << "write ide_ctrl dir: " <<  hex << addr << "size: " << size << "data: " << hex << *(uint32_t*)data << endl; */
    return true;
}

template <class ADDRESS_TYPE>
void IdeController<ADDRESS_TYPE>::intrPost() {
	PciDev<ADDRESS_TYPE>::intrPost();
}

template <class ADDRESS_TYPE>
void IdeController<ADDRESS_TYPE>::intrClear() {
	PciDev<ADDRESS_TYPE>::intrClear();
}

template <class ADDRESS_TYPE>
uint8_t IdeController<ADDRESS_TYPE>::interruptLine() {
	return PciDev<ADDRESS_TYPE>::interruptLine();
}

template <class ADDRESS_TYPE>
void IdeController<ADDRESS_TYPE>::SetPCIMaster(PCIMaster<ADDRESS_TYPE> *p) {
  		PciDev<ADDRESS_TYPE>::pciMaster = p;
		for (unsigned int i = 0; i < params()->disks.size(); i++) {
			disks[i]->SetPCIMaster(p);	
		}
}
  	

} // end of namespace ide
} // end of namespace pci
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
