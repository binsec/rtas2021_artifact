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

#ifndef __UNISIM_COMPONENT_CXX_PCI_IDE_PCIDEV_HH__
#define __UNISIM_COMPONENT_CXX_PCI_IDE_PCIDEV_HH__
 
#include <iosfwd>
//#include <stack>
#include <list>
#include <string>

#include "unisim/component/cxx/pci/ide/pcireg.hh"
#include "unisim/component/cxx/pci/ide/event.hh"
#include <unisim/util/endian/endian.hh>
#include "unisim/component/cxx/pci/ide/pci_master.hh"
#include "unisim/component/cxx/pci/ide/event_manager.hh"
//#include "dev/pcidev_abstract.hh"
//#include "comm/tlm_message.hh"
//#include "comm/masterPortAbstract.hh"
//#include "base/miscFunc.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace pci {
namespace ide {

using unisim::util::endian::Host2LittleEndian;
using std::list;
using namespace std;

#define BAR_IO_MASK 0x3
#define BAR_MEM_MASK 0xF
#define BAR_IO_SPACE_BIT 0x1
#define BAR_IO_SPACE(x) ((x) & BAR_IO_SPACE_BIT)
#define BAR_NUMBER(x) (((x) - PCI0_BASE_ADDR0) >> 0x2);

//class MemoryController;

class PciConfigData //: public SimObjectAbstract
{
  public:
    /**
     * Constructor to initialize the devices config space to 0.
     */
    PciConfigData(const std::string &name);
	virtual ~PciConfigData();

	virtual const std::string name() const;

    /** The first 64 bytes */
    PCIConfig config;
 
    /** The size of the BARs */
    uint32_t BARSize[6];
 
	private:
	std::string _name;
};

/**
 * PCI device, base implemnation is only config space.
 * Each device is connected to a PCIBus device
 * which returns -1 for everything but the pcidevs that
 * are attached to it. 
 */
template<class ADDRESS_TYPE>
class PciDev 
{
   public:
    struct Params
    {
        std::string name;
	//	MasterPortAbstract<tlm_message<IntrRequest, IntrRequest> > *intrPort;

        /**
         * A pointer to the object that contains the first 64 bytes of
         * config space
         */
        PciConfigData *configData;

        /** The bus number we are on */
        uint32_t busNum;

        /** The device number we have */
        uint32_t deviceNum;

        /** The function number */
        uint32_t functionNum;
    };
    
    list<Event *> event_stack;
	
//  protected:
    Params *_params;

    /** The current config space. Unlike the PciConfigData this is
     * updated during simulation while continues to reflect what was
     * in the config file.
     */
    PCIConfig config;
 
    /** The size of the BARs */
    uint32_t BARSize[6];

    PciConfigData *configData;
    /* Interface for dma and interrupts */
    protected:
    PCIMaster<ADDRESS_TYPE> *pciMaster;
    public:
    EventManager *eventManager;
    
	//MasterPortAbstract<tlm_message<IntrRequest, IntrRequest> > *intrPort;

  public:
  	virtual void SetEventManager(EventManager *em);
  	virtual void SetPCIMaster(PCIMaster<ADDRESS_TYPE> *p);

    void
    intrPost();

    void
    intrClear();

    uint8_t 
    interruptLine();

  public:
    /**
     * Constructor for PCI Dev. This function copies data from the
     * config file object PCIConfigData and registers the device with
     * a PciConfigAll object.
     */
    //PciDev(std::string name);
    PciDev(Params *params);
    virtual ~PciDev();
    
    virtual bool readMem(ADDRESS_TYPE addr, int size, uint8_t *data);

    virtual bool writeMem(ADDRESS_TYPE addr, int size, const uint8_t *data);

    virtual bool readIO(ADDRESS_TYPE addr, int size, uint8_t *data);

    virtual bool writeIO(ADDRESS_TYPE addr, int size, const uint8_t *data);

	virtual bool dmaWrite(ADDRESS_TYPE addr, int len, const uint8_t *data);
	
	virtual bool dmaRead(ADDRESS_TYPE addr, int len, uint8_t *data);
	
    //virtual bool read(ReqData &reqData, uint8_t *data) = 0;
    //virtual bool write(ReqData &reqData, const uint8_t *data) = 0;

    /**
     * Write to the PCI config space data that is stored locally. This may be
     * overridden by the device but at some point it will eventually call this
     * for normal operations that it does not need to override.
     * @param offset the offset into config space
     * @param size the size of the write
     * @param data the data to write
     */
    virtual bool writeConfig(ADDRESS_TYPE offset, int size, const uint8_t* data);

    /**
     * Read from the PCI config space data that is stored locally. This may be
     * overridden by the device but at some point it will eventually call this
     * for normal operations that it does not need to override.
     * @param offset the offset into config space
     * @param size the size of the read
     * @param data pointer to the location where the read value should be stored
     */
    virtual bool readConfig(ADDRESS_TYPE offset, int size, uint8_t *data);

};

} // end of namespace ide
} // end of namespace pci
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PCI_IDE_PCIDEV_HH__
