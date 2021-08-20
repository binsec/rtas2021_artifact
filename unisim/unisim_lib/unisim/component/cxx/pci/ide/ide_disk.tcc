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
 * Device model implementation for an IDE disk
 */

#include <cerrno>
#include <cassert>
#include <cstring>
#include <deque>
#include <string>

#include "unisim/component/cxx/pci/ide/disk_image.hh"
#include "unisim/component/cxx/pci/ide/ide_disk.hh"
#include "unisim/component/cxx/pci/ide/ide_ctrl.hh"
#include "unisim/component/cxx/pci/ide/pci_master.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace pci {
namespace ide {

using namespace std;

template <class ADDRESS_TYPE>
IdeDisk<ADDRESS_TYPE>::IdeDisk(const string &name, DiskImage *img,
                 int id, int delay, uint8_t _uid)
    : ctrl(NULL)
    , image(img)
    , _name(name)
    , diskDelay(delay)
    , uniqueDiskIdentifier(_uid)
    , pciMaster(0)
    , dmaTransferEvent(this)
    , dmaReadWaitEvent(this)
    , dmaWriteWaitEvent(this)
    , dmaPrdReadEvent(this)
    , dmaReadEvent(this)
    , dmaWriteEvent(this)
{
	
    // Reset the device state
    reset(id);

    // fill out the drive ID structure
    memset(&driveID, 0, sizeof(struct ataparams));

    // Calculate LBA and C/H/S values
    uint16_t cylinders;
    uint8_t heads;
    uint8_t sectors;

    uint32_t lba_size = image->size();
    if (lba_size >= 16383*16*63) {
        cylinders = 16383;
        heads = 16;
        sectors = 63;
    } else {
        if (lba_size >= 63)
            sectors = 63;
        else
            sectors = lba_size;

        if ((lba_size / sectors) >= 16)
            heads = 16;
        else
            heads = (lba_size / sectors);
        
        cylinders = lba_size / (heads * sectors);
    }

    // Setup the model name
    strncpy((char *)driveID.atap_model, "EGENIR CDI EiDks",
            sizeof(driveID.atap_model));
    // Set the maximum multisector transfer size
    driveID.atap_multi = MAX_MULTSECT;
    // IORDY supported, IORDY disabled, LBA enabled, DMA enabled
    driveID.atap_capabilities1 = 0x7;
    // UDMA support, EIDE support
    driveID.atap_extensions = 0x6;
    // Setup default C/H/S settings
    driveID.atap_cylinders = cylinders;
    driveID.atap_sectors = sectors;
    driveID.atap_heads = heads;
    // Setup the current multisector transfer size
    driveID.atap_curmulti = MAX_MULTSECT;
    driveID.atap_curmulti_valid = 0x1;
    // Number of sectors on disk
    driveID.atap_capacity = lba_size;
    // Multiword DMA mode 2 and below supported
    //FIXME, TODO: see wich value we should assign here
    //driveID.atap_dmamode_supp = 0x400;
	driveID.atap_dmamode_supp = 0x00;
    // Set PIO mode 4 and 3 supported
    driveID.atap_piomode_supp = 0x3;
    // Set DMA mode 4 and below supported
    driveID.atap_udmamode_supp = 0x1f;
    // Statically set hardware config word
    driveID.atap_hwreset_res = 0x4001;

    //arbitrary for now...
    driveID.atap_ata_major = WDC_VER_ATA7;
}

template <class ADDRESS_TYPE>
uint8_t IdeDisk<ADDRESS_TYPE>::UniqueDiskIdentifier() {
	return uniqueDiskIdentifier;
}

template <class ADDRESS_TYPE>
IdeDisk<ADDRESS_TYPE>::~IdeDisk()
{
	
    // destroy the data buffer
    delete [] dataBuffer;
    if (image) delete image;
}

template <class ADDRESS_TYPE>
void
IdeDisk<ADDRESS_TYPE>::reset(int id)
{
    // initialize the data buffer and shadow registers
    dataBuffer = new uint8_t[MAX_DMA_SIZE];

    memset(dataBuffer, 0, MAX_DMA_SIZE);
    memset(&cmdReg, 0, sizeof(CommandReg_t));
    memset(&curPrd.entry, 0, sizeof(PrdEntry_t));

    dmaInterfaceBytes = 0;
    curPrdAddr = 0;
    curSector = 0;
    cmdBytes = 0;
    cmdBytesLeft = 0;
    drqBytesLeft = 0;
    dmaRead = false;
    intrPending = false;

    // set the device state to idle
    dmaState = Dma_Idle;

    if (id == DEV0) {
        devState = Device_Idle_S;
        devID = DEV0;
    } else if (id == DEV1) {
        devState = Device_Idle_NS;
        devID = DEV1;
    } else {
        soft_panic("Invalid device");
        cerr << "id :" << id << endl;
    }

    // set the device ready bit
    status = STATUS_DRDY_BIT;

    /* The error register must be set to 0x1 on start-up to
       indicate that no diagnostic error was detected */
    cmdReg.error = 0x1;
}

////
// Utility functions
////

template <class ADDRESS_TYPE>
bool
IdeDisk<ADDRESS_TYPE>::isDEVSelect()
{ 
    return ctrl->isDiskSelected(uniqueDiskIdentifier);
}

template <class ADDRESS_TYPE>
uint32_t
IdeDisk<ADDRESS_TYPE>::bytesInDmaPage(ADDRESS_TYPE curAddr, uint32_t bytesLeft)
{
    uint32_t bytesInPage = 0;

    // First calculate how many bytes could be in the page
    if (bytesLeft > pageBytes())
        bytesInPage = pageBytes();
    else
        bytesInPage = bytesLeft;

    // Next, see if we have crossed a page boundary, and adjust
    ADDRESS_TYPE upperBound = curAddr + bytesInPage;
    ADDRESS_TYPE pageBound = truncPage(curAddr) + pageBytes();

    assert(upperBound >= curAddr && "DMA read wraps around address space!\n");

    if (upperBound >= pageBound)
        bytesInPage = pageBound - curAddr;

    return bytesInPage;
}

////
// Device registers read/write
////

template <class ADDRESS_TYPE>
bool 
IdeDisk<ADDRESS_TYPE>::read(const ADDRESS_TYPE &offset, IdeRegType reg_type, uint8_t *data, int size)
{
	
    DevAction_t action = ACT_NONE;

    switch (reg_type) {
      case COMMAND_BLOCK:
        switch (offset) {
          // Data transfers occur two bytes at a time
          case DATA_OFFSET:
            *(uint16_t*)data = cmdReg.data;
            action = ACT_DATA_READ_SHORT;
            break;
          case ERROR_OFFSET:
            *data = cmdReg.error;
            break;
          case NSECTOR_OFFSET:
            *data = cmdReg.sec_count;
            break;
          case SECTOR_OFFSET:
            *data = cmdReg.sec_num;
            break;
          case LCYL_OFFSET:
            *data = cmdReg.cyl_low;
            break;
          case HCYL_OFFSET:
            *data = cmdReg.cyl_high;
            break;
          case DRIVE_OFFSET:
            *data = cmdReg.drive;
            break;
          case STATUS_OFFSET:
            *data = status;
            action = ACT_STAT_READ;
            break;
          default:
            panic2("Invalid IDE command register offset: %#x\n", offset);
        }
        break;
      case CONTROL_BLOCK:
        if (offset == ALTSTAT_OFFSET)
            *data = status;
        else
            panic2("Invalid IDE control register offset: %#x\n", offset);
        break;
      default:
        panic("Unknown register block!\n");
    }

    if (action != ACT_NONE)
        updateState(action);
    return true;
}

template <class ADDRESS_TYPE>
bool 
IdeDisk<ADDRESS_TYPE>::write(const ADDRESS_TYPE &offset, IdeRegType reg_type, const uint8_t *data, int size)
{
	
    DevAction_t action = ACT_NONE;

    switch (reg_type) {
      case COMMAND_BLOCK:
        switch (offset) {
          case DATA_OFFSET:
            cmdReg.data = *(uint16_t*)data;
            action = ACT_DATA_WRITE_SHORT;
            break;
          case FEATURES_OFFSET:
            break;
          case NSECTOR_OFFSET:
            cmdReg.sec_count = *data;
            break;
          case SECTOR_OFFSET:
            cmdReg.sec_num = *data;
            break;
          case LCYL_OFFSET:
            cmdReg.cyl_low = *data;
            break;
          case HCYL_OFFSET:
            cmdReg.cyl_high = *data;
            break;
          case DRIVE_OFFSET:
            cmdReg.drive = *data;
            action = ACT_SELECT_WRITE;
            break;
          case COMMAND_OFFSET:
            cmdReg.command = *data;
            action = ACT_CMD_WRITE;
            break;
          default:
            panic2("Invalid IDE command register offset: %#x\n", offset);
        }
        break;
      case CONTROL_BLOCK:
        if (offset == CONTROL_OFFSET) {
            if (*data & CONTROL_RST_BIT) {
                // force the device into the reset state
                devState = Device_Srst;
                action = ACT_SRST_SET;
            } else if (devState == Device_Srst && !(*data & CONTROL_RST_BIT))
                action = ACT_SRST_CLEAR;

            nIENBit = (*data & CONTROL_IEN_BIT) ? true : false;
        }
        else
            panic2("Invalid IDE control register offset: %#x\n", offset);
        break;
      default:
        panic("Unknown register block!\n");
    }

    if (action != ACT_NONE)
        updateState(action);
    return true;
}

////
// Perform DMA transactions
////

template <class ADDRESS_TYPE>
bool
IdeDisk<ADDRESS_TYPE>::doDmaTransfer()
{
	
    if (dmaState != Dma_Transfer || devState != Transfer_Data_Dma)
        panic3("Inconsistent DMA transfer state: dmaState = %d devState = %d\n",
              dmaState, devState);

    // first read the current PRD
        dmaPrdReadDone();
    return true;
}

template <class ADDRESS_TYPE>
bool
IdeDisk<ADDRESS_TYPE>::dmaPrdReadDone()
{
	
    // actually copy the PRD from physical memory
	/* TODO: PAU see if we need to add a new code for memory reads TEST */
	  
	dma_read(curPrdAddr, sizeof(PrdEntry_t), (uint8_t *)&curPrd.entry);

    DPRINTF(IdeDisk,
            "%#x PRD: baseAddr:%#x () byteCount:%d (%d) eot:%#x sector:%d\n",
            curPrdAddr, curPrd.getBaseAddr(), curPrd.getByteCount(), (cmdBytesLeft/SectorSize),
            curPrd.getEOT(), curSector);

    // the prd pointer has already been translated, so just do an increment
    curPrdAddr = curPrdAddr + sizeof(PrdEntry_t);

    if (dmaRead)
        return doDmaRead();
    else
        return doDmaWrite();
}

template <class ADDRESS_TYPE>
bool
IdeDisk<ADDRESS_TYPE>::doDmaRead()
{
	

    /** @todo we need to figure out what the delay actually will be */
    int totalDiskDelay = diskDelay + (curPrd.getByteCount() / SectorSize);

    DPRINTF(IdeDisk, "doDmaRead, diskDelay: %d totalDiskDelay: %d\n",
            diskDelay, totalDiskDelay);
        // schedule dmaReadEvent with sectorDelay (dmaReadDone)
     //   dmaReadEvent.schedule(curTick + totalDiskDelay);
     ctrl->eventManager->schedule(&dmaReadEvent);
     return true;
     //return dmaReadDone();
}

template <class ADDRESS_TYPE>
bool
IdeDisk<ADDRESS_TYPE>::dmaReadDone()
{
	

    ADDRESS_TYPE curAddr = 0, dmaAddr = 0;
    uint32_t bytesWritten = 0, bytesInPage = 0, bytesLeft = 0;

    // set initial address
    curAddr = curPrd.getBaseAddr();

    // clear out the data buffer
    memset(dataBuffer, 0, MAX_DMA_SIZE);

    // read the data from memory via DMA into a data buffer
    while (bytesWritten < curPrd.getByteCount()) {
        if (cmdBytesLeft <= 0)
            panic("DMA data is larger than # of sectors specified\n");

		//PAU new code
		dmaAddr = curAddr;

        // calculate how many bytes are in the current page
        bytesLeft = curPrd.getByteCount() - bytesWritten;
        bytesInPage = bytesInDmaPage(curAddr, bytesLeft);

        // copy the data from memory into the data buffer
		//PAU new code TESTIT
		dma_read(dmaAddr, bytesInPage, (uint8_t *)(dataBuffer + bytesWritten));
               
        curAddr += bytesInPage;
        bytesWritten += bytesInPage;
        cmdBytesLeft -= bytesInPage;
    }

    // write the data to the disk image
    for (bytesWritten = 0; 
         bytesWritten < curPrd.getByteCount();
         bytesWritten += SectorSize) {

        writeDisk(curSector++, (uint8_t *)(dataBuffer + bytesWritten));
    }

    // check for the EOT
    if (curPrd.getEOT()) {
        assert(cmdBytesLeft == 0);
        dmaState = Dma_Idle;
        updateState(ACT_DMA_DONE);
    } else {
        doDmaTransfer();
    }
    return true;
}

template <class ADDRESS_TYPE>
bool
IdeDisk<ADDRESS_TYPE>::doDmaWrite()
{
	
    /** @todo we need to figure out what the delay actually will be */
    int totalDiskDelay = diskDelay + (curPrd.getByteCount() / SectorSize);
    
    DPRINTF(IdeDisk, "doDmaWrite, diskDelay: %d totalDiskDelay: %d\n",
            diskDelay, totalDiskDelay);

        // schedule event with disk delay (dmaWriteDone)
       // dmaWriteEvent.schedule(curTick + totalDiskDelay);
       ctrl->eventManager->schedule(&dmaWriteEvent);
       return true;
    //return dmaWriteDone();
       
}

template <class ADDRESS_TYPE>
bool
IdeDisk<ADDRESS_TYPE>::dmaWriteDone()
{
	
    ADDRESS_TYPE curAddr = 0, pageAddr = 0, dmaAddr = 0;
    uint32_t bytesRead = 0, bytesInPage = 0;

    // setup the initial page and DMA address
    curAddr = curPrd.getBaseAddr();
    pageAddr = truncPage(curAddr);
	
	//PAU new code
	dmaAddr = curAddr;
    // clear out the data buffer
    memset(dataBuffer, 0, MAX_DMA_SIZE);

    while (bytesRead < curPrd.getByteCount()) {
        // see if we have crossed into a new page
        if (pageAddr != truncPage(curAddr)) {
            // write the data to memory
			// PAU new code TESTIT
			dma_write(dmaAddr, bytesInPage, (uint8_t *)(dataBuffer + (bytesRead - bytesInPage)));
            // update the DMA address and page address
            pageAddr = truncPage(curAddr);
			//PAU new code:
			dmaAddr = curAddr;
            bytesInPage = 0;
        }

        if (cmdBytesLeft <= 0)
            panic("DMA requested data is larger than # sectors specified\n");

        readDisk(curSector++, (uint8_t *)(dataBuffer + bytesRead));

        curAddr += SectorSize;
        bytesRead += SectorSize;
        bytesInPage += SectorSize;
        cmdBytesLeft -= SectorSize;
    }

    // write the last page worth read to memory
    if (bytesInPage != 0) {
		// PAU new code TESTIT
		dma_write(dmaAddr, bytesInPage, (uint8_t *)(dataBuffer + (bytesRead - bytesInPage)));
    }

    // check for the EOT
    if (curPrd.getEOT()) {
        assert(cmdBytesLeft == 0);
        dmaState = Dma_Idle;
        updateState(ACT_DMA_DONE);
    } else {
        doDmaTransfer();
    }
    return true;
}

//// 
// Dma read/write routines
////

template <class ADDRESS_TYPE>
void IdeDisk<ADDRESS_TYPE>::dma_read(ADDRESS_TYPE addr, int size, uint8_t* data) {

	ctrl->dmaRead(addr, size, data);
	
}

template <class ADDRESS_TYPE>
void IdeDisk<ADDRESS_TYPE>::dma_write(ADDRESS_TYPE addr, int size, uint8_t* data) {

	ctrl->dmaWrite(addr, size, data);

}

////
// Disk utility routines
///

template <class ADDRESS_TYPE>
bool
IdeDisk<ADDRESS_TYPE>::readDisk(uint32_t sector, uint8_t *data)
{
	
    uint32_t bytesRead = image->read(data, sector);

    if (bytesRead != SectorSize) {
        soft_panic("Can't read from ");
        cerr << name() << "Only " << bytesRead << " of " << SectorSize << " read " << endl;
		return false;
    }
    return true;
}

template <class ADDRESS_TYPE>
bool
IdeDisk<ADDRESS_TYPE>::writeDisk(uint32_t sector, uint8_t *data)
{
	
    uint32_t bytesWritten = image->write(data, sector);

    if (bytesWritten != SectorSize) {
       soft_panic("Can't write to ");
        cerr << name() << "Only " << bytesWritten << " of " << SectorSize << " written " << endl;
		return false;
    }
    return true;
}

////
// Setup and handle commands
////

template <class ADDRESS_TYPE>
bool
IdeDisk<ADDRESS_TYPE>::startDma(const uint32_t &prdTableBase)
{

    if (dmaState != Dma_Start)
        panic("Inconsistent DMA state, should be in Dma_Start!\n");

    if (devState != Transfer_Data_Dma)
        panic("Inconsistent device state for DMA start!\n");

    // PRD base address is given by bits 31:2
	// PAU new code:
	curPrdAddr = (prdTableBase & ~(0x3UL));

    dmaState = Dma_Transfer;

    // schedule dma transfer (doDmaTransfer)
    //dmaTransferEvent.schedule(curTick + 1);
    ctrl->eventManager->schedule(&dmaTransferEvent);
    return true;
    //doDmaTransfer();
}

template <class ADDRESS_TYPE>
bool
IdeDisk<ADDRESS_TYPE>::abortDma()
{

    if (dmaState == Dma_Idle)
        panic("Inconsistent DMA state, should be Start or Transfer!");

    if (devState != Transfer_Data_Dma && devState != Prepare_Data_Dma)
        panic("Inconsistent device state, should be Transfer or Prepare!\n");

    updateState(ACT_CMD_ERROR);
    return true;
}

template <class ADDRESS_TYPE>
bool
IdeDisk<ADDRESS_TYPE>::startCommand()
{

    DevAction_t action = ACT_NONE;
    uint32_t size = 0;
    dmaRead = false;

    // Decode commands
    switch (cmdReg.command) {
        // Supported non-data commands
      case WDSF_READ_NATIVE_MAX:
        size = image->size() - 1;
        cmdReg.sec_num = (size & 0xff);
        cmdReg.cyl_low = ((size & 0xff00) >> 8);
        cmdReg.cyl_high = ((size & 0xff0000) >> 16);
        cmdReg.head = ((size & 0xf000000) >> 24);

        devState = Command_Execution;
        action = ACT_CMD_COMPLETE;
        break;

      case WDCC_RECAL:
      case WDCC_IDP:
      case WDCC_STANDBY_IMMED:
      case WDCC_FLUSHCACHE:
      case WDSF_VERIFY:
      case WDSF_SEEK:
      case SET_FEATURES:
      case WDCC_SETMULTI:
        devState = Command_Execution;
        action = ACT_CMD_COMPLETE;
        break;

        // Supported PIO data-in commands
      case WDCC_IDENTIFY:
        cmdBytes = cmdBytesLeft = sizeof(struct ataparams);
        devState = Prepare_Data_In;
        action = ACT_DATA_READY;
        break;

      case WDCC_READMULTI:
      case WDCC_READ:
        if (!(cmdReg.drive & DRIVE_LBA_BIT))
            panic("Attempt to perform CHS access, only supports LBA\n");

        if (cmdReg.sec_count == 0)
            cmdBytes = cmdBytesLeft = (256 * SectorSize);
        else
            cmdBytes = cmdBytesLeft = (cmdReg.sec_count * SectorSize);

        curSector = getLBABase();

        /** @todo make this a scheduled event to simulate disk delay */
        devState = Prepare_Data_In;
        action = ACT_DATA_READY;
        break;

        // Supported PIO data-out commands
      case WDCC_WRITEMULTI:
      case WDCC_WRITE:
        if (!(cmdReg.drive & DRIVE_LBA_BIT))
            panic("Attempt to perform CHS access, only supports LBA\n");

        if (cmdReg.sec_count == 0)
            cmdBytes = cmdBytesLeft = (256 * SectorSize);
        else
            cmdBytes = cmdBytesLeft = (cmdReg.sec_count * SectorSize);

        curSector = getLBABase();

        devState = Prepare_Data_Out;
        action = ACT_DATA_READY;
        break;

        // Supported DMA commands
      case WDCC_WRITEDMA:
        dmaRead = true;  // a write to the disk is a DMA read from memory
      case WDCC_READDMA:
        if (!(cmdReg.drive & DRIVE_LBA_BIT))
            panic("Attempt to perform CHS access, only supports LBA\n");

        if (cmdReg.sec_count == 0)
            cmdBytes = cmdBytesLeft = (256 * SectorSize);
        else
            cmdBytes = cmdBytesLeft = (cmdReg.sec_count * SectorSize);

        curSector = getLBABase();

        devState = Prepare_Data_Dma;
        action = ACT_DMA_READY;
        break;

      default:
        panic2("Unsupported ATA command: ", (unsigned int) cmdReg.command);
    }

    if (action != ACT_NONE) {
        // set the BSY bit
        status |= STATUS_BSY_BIT;
        // clear the DRQ bit
        status &= ~STATUS_DRQ_BIT;
        // clear the DF bit
        status &= ~STATUS_DF_BIT;

        updateState(action);
    }
    return true;
}

////
// Handle setting and clearing interrupts
////

template <class ADDRESS_TYPE>
bool
IdeDisk<ADDRESS_TYPE>::intrPost()
{

 //   DPRINTF(IdeDisk, "Posting Interrupt\n");
    if (intrPending)
        panic("Attempt to post an interrupt with one pending\n");

    intrPending = true;

    // talk to controller to set interrupt
    if (ctrl) {
        ctrl->setInterruptBit();
        ctrl->intrPost();
    }
    return true;
}

template <class ADDRESS_TYPE>
bool
IdeDisk<ADDRESS_TYPE>::intrClear()
{

    //DPRINTF(IdeDisk, "Clearing Interrupt\n");
    if (!intrPending)
        panic("Attempt to clear a non-pending interrupt\n");

    intrPending = false;

    // talk to controller to clear interrupt
    if (ctrl)
        ctrl->intrClear();
    return true;
}


////
// Manage the device internal state machine
////

template <class ADDRESS_TYPE>
bool
IdeDisk<ADDRESS_TYPE>::updateState(DevAction_t action)
{

    switch (devState) {
      case Device_Srst:
        if (action == ACT_SRST_SET) {
            // set the BSY bit
            status |= STATUS_BSY_BIT;
        } else if (action == ACT_SRST_CLEAR) {
            // clear the BSY bit
            status &= ~STATUS_BSY_BIT;

            // reset the device state
            reset(devID);
        }
        break;

      case Device_Idle_S:
        if (action == ACT_SELECT_WRITE && !isDEVSelect()) {
            devState = Device_Idle_NS;
        } else if (action == ACT_CMD_WRITE) {
            startCommand();
        }

        break;

      case Device_Idle_SI:
        if (action == ACT_SELECT_WRITE && !isDEVSelect()) {
            devState = Device_Idle_NS;
            intrClear();
        } else if (action == ACT_STAT_READ || isIENSet()) {
            devState = Device_Idle_S;
            intrClear();
        } else if (action == ACT_CMD_WRITE) {
            intrClear();
            startCommand();
        }

        break;

      case Device_Idle_NS:
        if (action == ACT_SELECT_WRITE && isDEVSelect()) {
            if (!isIENSet() && intrPending) {
                devState = Device_Idle_SI;
                intrPost();
            }
            if (isIENSet() || !intrPending) {
                devState = Device_Idle_S;
            }
        }
        break;

      case Command_Execution:
        if (action == ACT_CMD_COMPLETE) {
            // clear the BSY bit
            setComplete();

            if (!isIENSet()) {
                devState = Device_Idle_SI;
                intrPost();
            } else {
                devState = Device_Idle_S;
            }
        }
        break;

      case Prepare_Data_In:
        if (action == ACT_CMD_ERROR) {
            // clear the BSY bit
            setComplete();

            if (!isIENSet()) {
                devState = Device_Idle_SI;
                intrPost();
            } else {
                devState = Device_Idle_S;
            }
        } else if (action == ACT_DATA_READY) {
            // clear the BSY bit
            status &= ~STATUS_BSY_BIT;
            // set the DRQ bit
            status |= STATUS_DRQ_BIT;

            // copy the data into the data buffer
            if (cmdReg.command == WDCC_IDENTIFY) {
                // Reset the drqBytes for this block
                drqBytesLeft = sizeof(struct ataparams);

                memcpy((void *)dataBuffer, (void *)&driveID, 
                       sizeof(struct ataparams));
            } else {
                // Reset the drqBytes for this block
                drqBytesLeft = SectorSize;

                readDisk(curSector++, dataBuffer);
            }

            // put the first two bytes into the data register
            memcpy((void *)&cmdReg.data, (void *)dataBuffer, 
                   sizeof(uint16_t));

            if (!isIENSet()) {
                devState = Data_Ready_INTRQ_In;
                intrPost();
            } else {
                devState = Transfer_Data_In;
            }
        }
        break;

      case Data_Ready_INTRQ_In:
        if (action == ACT_STAT_READ) {
            devState = Transfer_Data_In;
            intrClear();
        }
        break;

      case Transfer_Data_In:
        if (action == ACT_DATA_READ_BYTE || action == ACT_DATA_READ_SHORT) {
            if (action == ACT_DATA_READ_BYTE) {
                panic("DEBUG: READING DATA ONE BYTE AT A TIME!\n");
            } else {
                drqBytesLeft -= 2;
                cmdBytesLeft -= 2;

                // copy next short into data registers
                if (drqBytesLeft)
                    memcpy((void *)&cmdReg.data, 
                           (void *)&dataBuffer[SectorSize - drqBytesLeft],
                           sizeof(uint16_t));
            }

            if (drqBytesLeft == 0) {
                if (cmdBytesLeft == 0) {
                    // Clear the BSY bit
                    setComplete();
                    devState = Device_Idle_S;
                } else {
                    devState = Prepare_Data_In;
                    // set the BSY_BIT
                    status |= STATUS_BSY_BIT;
                    // clear the DRQ_BIT
                    status &= ~STATUS_DRQ_BIT;

                    /** @todo change this to a scheduled event to simulate
                        disk delay */
                    updateState(ACT_DATA_READY);
                }
            }
        }
        break;

      case Prepare_Data_Out:
        if (action == ACT_CMD_ERROR || cmdBytesLeft == 0) {
            // clear the BSY bit
            setComplete();
            
            if (!isIENSet()) {
                devState = Device_Idle_SI;
                intrPost();
            } else {
                devState = Device_Idle_S;
            }
        } else if (action == ACT_DATA_READY && cmdBytesLeft != 0) {
            // clear the BSY bit
            status &= ~STATUS_BSY_BIT;
            // set the DRQ bit
            status |= STATUS_DRQ_BIT;

            // clear the data buffer to get it ready for writes
            memset(dataBuffer, 0, MAX_DMA_SIZE);

            // reset the drqBytes for this block
            drqBytesLeft = SectorSize;
            
            if (cmdBytesLeft == cmdBytes || isIENSet()) {
                devState = Transfer_Data_Out;
            } else {
                devState = Data_Ready_INTRQ_Out;
                intrPost();
            }
        }
        break;

      case Data_Ready_INTRQ_Out:
        if (action == ACT_STAT_READ) {
            devState = Transfer_Data_Out;
            intrClear();
        }
        break;

      case Transfer_Data_Out:
        if (action == ACT_DATA_WRITE_BYTE || 
            action == ACT_DATA_WRITE_SHORT) {

            if (action == ACT_DATA_READ_BYTE) {
                panic("DEBUG: WRITING DATA ONE BYTE AT A TIME!\n");
            } else {
                // copy the latest short into the data buffer
                memcpy((void *)&dataBuffer[SectorSize - drqBytesLeft],
                       (void *)&cmdReg.data,
                       sizeof(uint16_t));

                drqBytesLeft -= 2;
                cmdBytesLeft -= 2;
            }

            if (drqBytesLeft == 0) {
                // copy the block to the disk
                writeDisk(curSector++, dataBuffer);

                // set the BSY bit
                status |= STATUS_BSY_BIT;
                // set the seek bit
                status |= STATUS_SEEK_BIT;
                // clear the DRQ bit
                status &= ~STATUS_DRQ_BIT;

                devState = Prepare_Data_Out;

                /** @todo change this to a scheduled event to simulate
                    disk delay */
                updateState(ACT_DATA_READY);
            }
        }
        break;

      case Prepare_Data_Dma:
        if (action == ACT_CMD_ERROR) {
            // clear the BSY bit
            setComplete();
            
            if (!isIENSet()) {
                devState = Device_Idle_SI;
                intrPost();
            } else {
                devState = Device_Idle_S;
            }
        } else if (action == ACT_DMA_READY) {
            // clear the BSY bit
            status &= ~STATUS_BSY_BIT;
            // set the DRQ bit
            status |= STATUS_DRQ_BIT;

            devState = Transfer_Data_Dma;

            if (dmaState != Dma_Idle)
                panic("Inconsistent DMA state, should be Dma_Idle\n");

            dmaState = Dma_Start;
            // wait for the write to the DMA start bit
        }
        break;

      case Transfer_Data_Dma:
        if (action == ACT_CMD_ERROR || action == ACT_DMA_DONE) {
            // clear the BSY bit
            setComplete();
            // set the seek bit
            status |= STATUS_SEEK_BIT;
            // clear the controller state for DMA transfer
            ctrl->setDmaComplete(uniqueDiskIdentifier);

            if (!isIENSet()) {
                devState = Device_Idle_SI;
                intrPost();
            } else {
                devState = Device_Idle_S;
            }
        }
        break;

      default:
        panic2("Unknown IDE device state: %#x\n", devState);
    }
    return false;
}

} // end of namespace ide
} // end of namespace pci
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
