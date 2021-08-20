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

/** @file
 * Simple PCI IDE controller with bus mastering capability and UDMA
 * modeled after controller in the Intel PIIX4 chip
 */

#ifndef __UNISIM_COMPONENT_CXX_PCI_IDE_CTRL_HH__
#define __UNISIM_COMPONENT_CXX_PCI_IDE_CTRL_HH__

//#include <systemc>
#include <vector>
#include "unisim/component/cxx/pci/ide/pcidev.hh"
//#include "dev/ide_ctrl_abstract.hh"
#include "unisim/component/cxx/pci/ide/pcireg.hh"
#include "unisim/component/cxx/pci/ide/pci_master.hh"
//#include "dev/io_device.hh"
//#include "dev/ide_disk_abstract.hh"
//#include "sim/sim_object_abstract.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace pci {
namespace ide {

#define BMIC0    0x0  // Bus master IDE command register
#define BMIS0    0x2  // Bus master IDE status register
#define BMIDTP0  0x4  // Bus master IDE descriptor table pointer register
#define BMIC1    0x8  // Bus master IDE command register
#define BMIS1    0xa  // Bus master IDE status register
#define BMIDTP1  0xc  // Bus master IDE descriptor table pointer register

// Bus master IDE command register bit fields
#define RWCON 0x08 // Bus master read/write control
#define SSBM  0x01 // Start/stop bus master

// Bus master IDE status register bit fields
#define DMA1CAP 0x40 // Drive 1 DMA capable
#define DMA0CAP 0x20 // Drive 0 DMA capable
#define IDEINTS 0x04 // IDE Interrupt Status
#define IDEDMAE 0x02 // IDE DMA error
#define BMIDEA  0x01 // Bus master IDE active

// IDE Command byte fields
#define IDE_SELECT_OFFSET       (6)
#define IDE_SELECT_DEV_BIT      0x10

#define IDE_FEATURE_OFFSET      IDE_ERROR_OFFSET
#define IDE_COMMAND_OFFSET      IDE_STATUS_OFFSET

// IDE Timing Register bit fields
#define IDETIM_DECODE_EN 0x8000

// PCI device specific register byte offsets
#define IDE_CTRL_CONF_START 0x40
#define IDE_CTRL_CONF_END ((IDE_CTRL_CONF_START) + sizeof(config_regs))

template <class ADDRESS_TYPE>
class IdeDisk;

enum IdeRegType {
    COMMAND_BLOCK,
    CONTROL_BLOCK,
    BMI_BLOCK
};

#define sDPRINTF(x, y) \
		 cerr << "  (" << __FUNCTION__ << ":" \
			<< __FILE__ << ":" \
			<< __LINE__ << ")"; \
					fprintf(stderr, y); 

#define DPRINTF(x, y, args...) \
		cerr << "  (" << __FUNCTION__ << ":" \
			<< __FILE__ << ":" \
			<< __LINE__ << ") ";  \
	fprintf(stderr, y, args);
	//cout << "debug iN " function " << __FUNCTION__ << " line " << __LINE__ << endl;
       //printf(0, name(), args, cp::ArgListNull());
       

#define soft_panic(str) \
 {	cerr << " ERROR(" << __FUNCTION__ \
		<< ":" << __FILE__ \
		<< ":" << __LINE__ << "): " \
		<< str \
		<< endl;  }
 
#define soft_panic2(str, str2) \
	{	cerr << " ERROR(" << __FUNCTION__ \
		<< ":" << __FILE__ \
		<< ":" << __LINE__ << "): " \
		<< str << " " << str2 \
		<< endl; }  

#define panic(str) \
	{	cerr << " ERROR(" << __FUNCTION__ \
		<< ":" << __FILE__ \
		<< ":" << __LINE__ << "): " \
		<< str \
		<< endl; \
	return false; }

#define panic2(str, str2) \
	{	cerr << " ERROR(" << __FUNCTION__ \
		<< ":" << __FILE__ \
		<< ":" << __LINE__ << "): " \
		<< str  << str2 \
		<< endl; \
	return false; }
		
#define panic3(str, str2, str3) \
	   { cerr << " ERROR(" << __FUNCTION__ \
		<< ":" << __FILE__ \
		<< ":" << __LINE__ << "): " \
		<< str  << str2  << " " << str3\
		<< endl; \
	return false; }


/**
 * Device model for an Intel PIIX4 IDE controller
 */

//class IdeController : public sc_module, public virtual PciDev, public virtual IdeControllerAbstract
template <class ADDRESS_TYPE>
class IdeController : public PciDev<ADDRESS_TYPE>//, public virtual IdeControllerAbstract
{
    typedef PciDev<ADDRESS_TYPE> inherited;
    friend class IdeDisk<ADDRESS_TYPE>;

    enum IdeChannel {
        PRIMARY = 0,
        SECONDARY = 1
    };

  public:
    struct Params : public PciDev<ADDRESS_TYPE>::Params
    {
        /** Array of disk objects */
	   std::vector<IdeDisk<ADDRESS_TYPE> *> disks;
    };

  protected:
    /** Primary command block registers */
    uint32_t pri_cmd_addr;
    uint32_t pri_cmd_size;
    /** Primary control block registers */
    uint32_t pri_ctrl_addr;
    uint32_t pri_ctrl_size;
    /** Secondary command block registers */
    uint32_t sec_cmd_addr;
    uint32_t sec_cmd_size;
    /** Secondary control block registers */
    uint32_t sec_ctrl_addr;
    uint32_t sec_ctrl_size;
    /** Bus master interface (BMI) registers */
    uint32_t bmi_addr;
    uint32_t bmi_size;

  protected:
    /** Registers used for bus master interface */
    union {
        uint8_t data[16];

        struct {
            uint8_t bmic0;
            uint8_t reserved_0;
            uint8_t bmis0;
            uint8_t reserved_1;
            uint32_t bmidtp0;
            uint8_t bmic1;
            uint8_t reserved_2;
            uint8_t bmis1;
            uint8_t reserved_3;
            uint32_t bmidtp1;
        };

        struct {
            uint8_t bmic;
            uint8_t reserved_4;
            uint8_t bmis;
            uint8_t reserved_5;
            uint32_t bmidtp;
        } chan[2];

    } bmi_regs;
    /** Shadows of the device select bit */
    uint8_t dev[2];
    /** Registers used in device specific PCI configuration */
    union {
        uint8_t data[22];

        struct {
            uint16_t idetim0;
            uint16_t idetim1;
            uint8_t sidetim;
            uint8_t reserved_0[3];
            uint8_t udmactl;
            uint8_t reserved_1;
            uint16_t udmatim;
            uint8_t reserved_2[8];
            uint16_t ideconfig;
        };
    } config_regs;

    // Internal management variables
    bool io_enabled;
    bool bm_enabled;
    bool cmd_in_progress[4];

  protected:
    /** IDE Disks connected to controller */
    IdeDisk<ADDRESS_TYPE> *disks[4];
    /* Interface to access intr and dma */
	PCIMaster<ADDRESS_TYPE> *pciMaster;
	
  protected:
    /** Parse the access address to pass on to device */
    void parseAddr(const ADDRESS_TYPE &addr, uint32_t &offset, IdeChannel &channel, 
                   IdeRegType &reg_type);
                   
    /** Select the disk based on the channel and device bit */
    int getDisk(IdeChannel channel);

  public:
  	
    /** See if a disk is selected based on its pointer */
    bool isDiskSelected(uint8_t diskId);

	const std::string name() const { 
		return params()->name;}

  private:
    const Params *params() const; 

  public:
    IdeController(Params *p);
    ~IdeController();

    virtual bool writeConfig(ADDRESS_TYPE offset, int size, const uint8_t *data);
    virtual bool readConfig(ADDRESS_TYPE offset, int size, uint8_t *data);

	bool readMem(ADDRESS_TYPE addr, int size, uint8_t *data);
	bool writeMem(ADDRESS_TYPE addr, int size, const uint8_t *data);

	bool readIO(ADDRESS_TYPE addr, int size, uint8_t *data);
	bool writeIO(ADDRESS_TYPE addr, int size, const uint8_t *data);

    bool setDmaComplete(uint8_t diskId);
    void setInterruptBit();

	//PciDev functions
	virtual void intrPost();
	virtual void intrClear();
	virtual uint8_t interruptLine();
	void SetPCIMaster(PCIMaster<ADDRESS_TYPE> *p);
};

} // end of namespace ide
} // end of namespace pci
} // end of cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PCI_IDE_CTRL_HH__
