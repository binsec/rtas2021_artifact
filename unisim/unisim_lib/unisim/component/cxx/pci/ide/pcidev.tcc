/*
 *  Copyright (c) 2007,
 *  Universitat Politecnica de Catalunya (UPC)
 *  All rights reserved.
 *
 *  This file was created by Paula Casero and Alejandro Schenzle based on m5 simulator and therefore
retaining the original license:
 *
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

/* @file
 * Interface for devices using PCI configuration
 */

#ifndef __UNISIM_COMPONENT_CXX_PCI_IDE_PCIDEV_TCC__
#define __UNISIM_COMPONENT_CXX_PCI_IDE_PCIDEV_TCC__

#include <iostream>
#include <string.h>
 
namespace unisim {
namespace component {
namespace cxx {
namespace pci {
namespace ide {

template<class ADDRESS_TYPE>
void PciDev<ADDRESS_TYPE>::SetPCIMaster(PCIMaster<ADDRESS_TYPE> *p) {
	pciMaster = p;
}

template<class ADDRESS_TYPE>
void PciDev<ADDRESS_TYPE>::SetEventManager(EventManager *em) {
  		eventManager = em;
}

template<class ADDRESS_TYPE>
void
PciDev<ADDRESS_TYPE>::intrPost()
{   //TODO find out if we can eliminate intrline param and use the port position if possible. doesnt fit. intr ctrl has 5 ports, and we have 256 possible intr lines?
	pciMaster->postInt(configData->config.interruptLine);

/*	IntrRequest *request = new IntrRequest(full_system::intr::TT_POST, configData->config.interruptLine);
	
	tlm_message<IntrRequest, IntrRequest> message = tlm_message<IntrRequest, IntrRequest>(request);

	intrPort->send(message); */ 
}

template<class ADDRESS_TYPE>
void
PciDev<ADDRESS_TYPE>::intrClear()
{ 
	pciMaster->clearInt(configData->config.interruptLine);
/*	IntrRequest *request = new IntrRequest(full_system::intr::TT_CLEAR, configData->config.interruptLine);
	
	tlm_message<IntrRequest, IntrRequest> message = tlm_message<IntrRequest, IntrRequest>(request);

	intrPort->send(message);
	*/ 
}

template<class ADDRESS_TYPE>
bool PciDev<ADDRESS_TYPE>::dmaRead(ADDRESS_TYPE addr, int len, uint8_t *data) {
	if (pciMaster) {
		pciMaster->dmaRead(addr, len, data);
	} else {
		soft_panic("PCIMaster not set, not using dma");
		return false;
	}
	return true;
}

template<class ADDRESS_TYPE>
bool PciDev<ADDRESS_TYPE>::dmaWrite(ADDRESS_TYPE addr, int len, const uint8_t *data) {
	if (pciMaster) {
		pciMaster->dmaWrite(addr, len, data);
	} else {
		soft_panic("PCIMaster not set, not using dma");
		return false;
	}
	return true;
}

template<class ADDRESS_TYPE>
uint8_t 
PciDev<ADDRESS_TYPE>::interruptLine()
{ return configData->config.interruptLine; }

template<class ADDRESS_TYPE>
bool PciDev<ADDRESS_TYPE>::readMem(ADDRESS_TYPE addr, int size, uint8_t *data) {
	
	cerr << "Implement me!" << endl;
		cerr << "  (" << __FUNCTION__ << ":"
		<< __FILE__ << ":"
		<< __LINE__ << ")" << endl;
	return false;        
}

template<class ADDRESS_TYPE>
bool PciDev<ADDRESS_TYPE>::writeMem(ADDRESS_TYPE addr, int size, const uint8_t *data) {
		cerr << "Implement me!" << endl;
		cerr << "  (" << __FUNCTION__ << ":"
		<< __FILE__ << ":"
		<< __LINE__ << ")" << endl;
	return false;
}

template<class ADDRESS_TYPE>
bool PciDev<ADDRESS_TYPE>::readIO(ADDRESS_TYPE addr, int size, uint8_t *data) {
		cerr << "Implement me!" << endl;
		cerr << "  (" << __FUNCTION__ << ":"
		<< __FILE__ << ":"
		<< __LINE__ << ")" << endl;
	return false;
}

template<class ADDRESS_TYPE>
bool PciDev<ADDRESS_TYPE>::writeIO(ADDRESS_TYPE addr, int size, const uint8_t *data) {
		cerr << "Implement me!" << endl;
		cerr << "  (" << __FUNCTION__ << ":"
		<< __FILE__ << ":"
		<< __LINE__ << ")" << endl;
	return false;
}

template<class ADDRESS_TYPE>
PciDev<ADDRESS_TYPE>::PciDev(PciDev<ADDRESS_TYPE>::Params *p)
	: _params(p),
      configData(p->configData)
{
    // copy the config data from the PciConfigData object
    if (configData) {
        memcpy(config.data, configData->config.data, sizeof(config.data));
        memcpy(BARSize, configData->BARSize, sizeof(BARSize));
    } else {
       cerr << "NULL pointer to configuration data" << endl;
       		cerr << "  (" << __FUNCTION__ << ":"
			<< __FILE__ << ":"
			<< __LINE__ << ")" << endl;
    }
    
    //cerr << "+++ PciDev<ADDRESS_TYPE>: " << this << endl;
}

template<class ADDRESS_TYPE>
PciDev<ADDRESS_TYPE>::~PciDev(){
	
    	if (_params) {
    		if (_params->configData) 
    			delete _params->configData;
    		delete _params;
    	}
}

template<class ADDRESS_TYPE>
bool PciDev<ADDRESS_TYPE>::readConfig(ADDRESS_TYPE offset, int size, uint8_t *data)
{
    if (offset >= PCI_DEVICE_SPECIFIC) {
            cerr << "Device specific PCI config space not implemented!\n" << endl;
       		cerr << "  (" << __FUNCTION__ << ":"
			<< __FILE__ << ":"
			<< __LINE__ << ")" << endl;
			return false;
    }

    switch(size) {
      case sizeof(uint8_t): {
			
		*(uint8_t*)data = *(uint8_t*)&config.data[offset];
        	
    	/* fprintf(stderr, 
            "read device: %#x function: %#x register: %#x %d bytes: data: %#x\n",
            _params->deviceNum, _params->functionNum, offset, size,
            *(uint8_t*)data);
			break; */
		}
      case sizeof(uint16_t): {
        //*(uint16_t*)data = *(uint16_t*)&config.data[offset];
		  memcpy(data, &config.data[offset], sizeof(uint16_t));
		   /* fprintf(stderr, 
            "read device: %#x function: %#x register: %#x %d bytes: data: %#x\n",
            _params->deviceNum, _params->functionNum, offset, size,
            *(uint16_t*)data); */
		}
        break;
      case sizeof(uint32_t): {
        //*(uint32_t*)data = *(uint32_t*)&config.data[offset];
		memcpy(data, &config.data[offset], sizeof(uint32_t));
		   /*  fprintf(stderr, 
            "read device: %#x function: %#x register: %#x %d bytes: data: %#x\n",
            _params->deviceNum, _params->functionNum, offset, size,
            *(uint32_t*)data); */
		}
        break;
      default: {
      	    cerr << "Invalid PCI configuration read size!\n" << endl;
       		cerr << "  (" << __FUNCTION__ << ":"
			<< __FILE__ << ":"
			<< __LINE__ << ")" << endl;
      }
        
    }
	return true;
}

template<class ADDRESS_TYPE>
bool PciDev<ADDRESS_TYPE>::writeConfig(ADDRESS_TYPE offset, int size, const uint8_t *data)
{
    if (offset >= PCI_DEVICE_SPECIFIC) {
            cerr << "Device specific PCI config space not implemented!\n" << endl;
       		cerr << "  (" << __FUNCTION__ << ":"
			<< __FILE__ << ":"
			<< __LINE__ << ")" << endl;
			return false;    	
    }

    uint8_t &data8 = *(uint8_t*)data;
    uint16_t &data16 = *(uint16_t*)data;
    uint32_t &data32 = *(uint32_t*)data;

    switch (size) {
      case sizeof(uint8_t): // 1-byte access
		{ 
 		   /* fprintf(stderr,  
             "write device: %#x function: %#x reg: %#x size: %d data: %#x\n",
             _params->deviceNum, _params->functionNum, offset, size, data8); */

			switch (offset) {
	  		case PCI0_INTERRUPT_LINE:
            	config.interruptLine = data8;
				break;
          	case PCI_CACHE_LINE_SIZE:
            	config.cacheLineSize = data8;
				break;
          	case PCI_LATENCY_TIMER: {
				//cerr << "writing latency timer" << endl;
	    		config.latencyTimer = data8;
	    		break;
			}
          /* Do nothing for these read-only registers */
          	case PCI0_INTERRUPT_PIN:
	  		case PCI0_MINIMUM_GRANT:
	  		case PCI0_MAXIMUM_LATENCY:
	  		case PCI_CLASS_CODE:
	  		case PCI_REVISION_ID:
	    	break;
	  		default: {
		    	  	cerr << "Device specific PCI config space not implemented!\n" << endl;
       				cerr << "  (" << __FUNCTION__ << ":"
						<< __FILE__ << ":"
						<< __LINE__ << ")" << endl;
					return false;
	  		}
		}
		}
		break;
      case sizeof(uint16_t): // 2-byte access
		{ 
 		    /* fprintf(stderr,  
            	"write device: %#x function: %#x reg: %#x size: %d data: %#x\n",
             	_params->deviceNum, _params->functionNum, offset, size, data16); */

			switch (offset) {
	  		case PCI_COMMAND:
            	config.command = data16;
				break;
	  		case PCI_STATUS:
            	config.status = data16;
				break;
          	case PCI_CACHE_LINE_SIZE:
	    		config.cacheLineSize = data16;
	    		break;
	  		default: {
		    	  	cerr << "writing to a read only register" << endl;
       				cerr << "  (" << __FUNCTION__ << ":"
						<< __FILE__ << ":"
						<< __LINE__ << ")" << endl;
					return false;
	  		}
		}
		}
		break;
      case sizeof(uint32_t): // 4-byte access
		{
 			/* fprintf(stderr,  
	   	       	"write device: %#x function: %#x reg: %#x size: %d data: %#x\n",
    	   	   	_params->deviceNum, _params->functionNum, offset, size, data32); */

			switch (offset) {
	  		case PCI0_BASE_ADDR0:
	  		case PCI0_BASE_ADDR1:
	  		case PCI0_BASE_ADDR2:
	  		case PCI0_BASE_ADDR3:
	  		case PCI0_BASE_ADDR4:
	  		case PCI0_BASE_ADDR5:
            
            	uint32_t barnum, bar_mask;
            	barnum = BAR_NUMBER(offset);
            
            	if (BAR_IO_SPACE(Host2LittleEndian(config.baseAddr[barnum]))) {
                	bar_mask = BAR_IO_MASK;
            	} else {
                	bar_mask = BAR_MEM_MASK;
            	}

            	// Writing 0xffffffff to a BAR tells the card to set the 
            	// value of the bar to size of memory it needs
            	if (Host2LittleEndian(data32) == 0xffffffff) {
                	// This is I/O Space, bottom two bits are read only

                	config.baseAddr[barnum] = Host2LittleEndian(
                        (~(BARSize[barnum] - 1) & ~bar_mask) |
                        (Host2LittleEndian(config.baseAddr[barnum]) & bar_mask));
            	} else {

	        	config.baseAddr[barnum] = Host2LittleEndian(
                    (Host2LittleEndian(data32) & ~bar_mask) |
                    (Host2LittleEndian(config.baseAddr[barnum]) & bar_mask));

           		}
			break;

		  case PCI0_ROM_BASE_ADDR:
            if (Host2LittleEndian(data32) == 0xfffffffe)
                config.expansionROM = Host2LittleEndian((uint32_t)0xffffffff); 
            else
                config.expansionROM = data32;
            break;

          case PCI_COMMAND:
            // This could also clear some of the error bits in the Status 
            // register. However they should never get set, so lets ignore 
            // it for now
            config.command = data16;
            break;

	  	  default:
			break;
		}
	  break;

      default:

    	  	cerr << "invalid access size" << endl;
			cerr << "  (" << __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << ")" << endl;
			return false;

		}

       	fprintf(stderr,  "Writing to a read only register");
    }
    return true;
}

} // end of namespace ide
} // end of namespace pci
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PCI_IDE_PCIDEV_TCC__
