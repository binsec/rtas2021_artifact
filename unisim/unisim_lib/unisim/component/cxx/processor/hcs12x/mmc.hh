/*
 *  Copyright (c) 2008,
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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_MMC_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_MMC_HH__

#include <unisim/kernel/variable/variable.hh>
#include <inttypes.h>
#include <iostream>
#include <cmath>
#include <map>

#include "unisim/kernel/kernel.hh"

#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/registers.hh"

#include "unisim/util/debug/simple_register.hh"
#include "unisim/service/interfaces/register.hh"
#include "unisim/util/singleton/singleton.hh"
#include <unisim/util/debug/simple_register_registry.hh>

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>
#include <unisim/component/cxx/processor/hcs12x/exception.hh>
#include <unisim/component/cxx/processor/hcs12x/s12mpu_if.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

using unisim::kernel::Object;
using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::ServiceImport;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::CallBackObject;

using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;

using unisim::util::debug::SimpleRegister;
using unisim::service::interfaces::Register;
using unisim::util::Singleton;

using unisim::component::cxx::processor::hcs12x::S12MPU_IF;

class MMC :
	public CallBackObject
	, public Singleton<MMC>
	, public Service<Memory<physical_address_t> >
	, public Client<Memory<physical_address_t> >
	, public Service<Registers>
{
friend class Singleton<MMC >;

public:

	enum MODE {SS = 0x00, ES = 0x20, ST = 0x40, EX = 0x60, NS = 0x80, NX = 0xA0};
	enum ACCESS {READ, WRITE};

	//==============================================================================
	//=   MEMORY MAP (Logical Memories Offsets) and RESET VALUES OF MMC REGISTERS  =
	//==============================================================================

	static const unsigned int DIRECT_ADDRESS_SIZE	= 8;	// Number of bits used by the CPU to address DIRECT (max=8)
	static const unsigned int RAM_ADDRESS_SIZE		= 12;	// Number of bits used by the CPU to address RAM (max=16)
	static const unsigned int EEPROM_ADDRESS_SIZE	= 10;	// Number of bits used by the CPU to address EEPROM (max=16)
	static const unsigned int FLASH_ADDRESS_SIZE		= 14;	// Number of bits used by the CPU to address FLASH (max=16)

	static const uint16_t DIRECT_PAGE_SIZE	= 0x100;
	static const uint16_t RAM_PAGE_SIZE		= 0x1000;
	static const uint16_t EEPROM_PAGE_SIZE	= 0x400;
	static const uint16_t FLASH_PAGE_SIZE	= 0x4000;

	static const address_t RAM_ADDRESS_BITS		= 0x0FFF;
	static const address_t EEPROM_ADDRESS_BITS	= 0x03FF;
	static const address_t FLASH_ADDRESS_BITS	= 0x3FFF;

	static const physical_address_t CPU12X_RAM_PHYSICAL_ADDRESS_FIXED_BITS		= 0x00000000;
	static const physical_address_t CPU12X_EEPROM_PHYSICAL_ADDRESS_FIXED_BITS	= 0x00100000;
	static const physical_address_t CPU12X_FLASH_PHYSICAL_ADDRESS_FIXED_BITS	= 0x00400000;

	static const address_t REG_LOW_OFFSET	= 0x0000;
	static const address_t REG_HIGH_OFFSET	= 0x07FF;

	static const address_t CPU12X_EEPROM_WIN_LOW_OFFSET= 0x0800;
	static const address_t CPU12X_EEPROM_WIN_HIGH_OFFSET=0x0BFF;
	static const address_t CPU12X_RAM_WIN_LOW_OFFSET	= 0x1000;
	static const address_t CPU12X_RAM_WIN_HIGH_OFFSET	= 0x1FFF;
	static const address_t CPU12X_FLASH_WIN_LOW_OFFSET	= 0x8000;
	static const address_t CPU12X_FLASH_WIN_HIGH_OFFSET=0xBFFF;

	static const address_t CPU12X_EEPROM_LOW_OFFSET= 0x0800;
	static const address_t CPU12X_EEPROM_HIGH_OFFSET=0x0FFF;
	static const address_t CPU12X_RAM_LOW_OFFSET	= 0x1000;
	static const address_t CPU12X_RAM_HIGH_OFFSET	= 0x3FFF;
	static const address_t CPU12X_FLASH_LOW_OFFSET	= 0x4000;
	static const address_t CPU12X_FLASH_HIGH_OFFSET=0xFFFF;

	static const physical_address_t XGATE_RAM_BASE_ADDRESS = 0x0F0000;
	static const physical_address_t XGATE_FLASH_BASE_ADDRESS = 0x780000;

	static const address_t XGATE_FLASH_LOW_OFFSET	= 0x0800;
	static const address_t XGATE_FLASH_HIGH_OFFSET=0x7FFF;
	static const address_t XGATE_RAM_LOW_OFFSET	= 0x8000;
	static const address_t XGATE_RAM_HIGH_OFFSET	= 0xFFFF;

	static const physical_address_t GLOBAL_REG_LOW_OFFSET				= 0x000000;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_REG_HIGH_OFFSET				= 0x0007FF;	// GPAGE_CPUAddress

	static const physical_address_t GLOBAL_RAM_LOW_OFFSET				= 0x000800;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_RAM_HIGH_OFFSET				= 0x0FFFFF;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_RAM_PAGED_LOW_OFFSET			= 0x001000;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_RAM_PAGED_HIGH_OFFSET		= 0x0FDFFF;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_RAM_UNPAGED_LOW_OFFSET		= 0x0FE000;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_RAM_UNPAGED_HIGH_OFFSET		= 0x0FFFFF;	// GPAGE_CPUAddress

	static const physical_address_t GLOBAL_EEPROM_LOW_OFFSET			= 0x100000;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_EEPROM_HIGH_OFFSET			= 0x13FFFF;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_EEPROM_PAGED_LOW_OFFSET		= 0x100000;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_EEPROM_PAGED_HIGH_OFFSET		= 0x13FBFF;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_EEPROM_UNPAGED_LOW_OFFSET	= 0x13FC00;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_EEPROM_UNPAGED_HIGH_OFFSET	= 0x13FFFF;	// GPAGE_CPUAddress

	static const physical_address_t GLOBAL_EXTERNAL_LOW_OFFSET			= 0x140000;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_EXTERNAL_HIGH_OFFSET			= 0x3FFFFF;	// GPAGE_CPUAddress

	static const physical_address_t GLOBAL_FLASH_LOW_OFFSET				= 0x400000;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_FLASH_HIGH_OFFSET			= 0x7FFFFF;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_FLASH_PAGED_LOW_OFFSET		= 0x400000;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_FLASH_PAGED_HIGH_OFFSET		= 0x7F3FFF;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_FLASH_UNPAGED_LOW_OFFSET		= 0x7F4000;	// GPAGE_CPUAddress
	static const physical_address_t GLOBAL_FLASH_UNPAGED_HIGH_OFFSET	= 0x7FFFFF;	// GPAGE_CPUAddress

	enum {MMCCTL0,	MODE, GPAGE, DIRECT, MMCCTL1, RPAGE, EPAGE, PPAGE, RAMWPC, RAMXGU, RAMSHL, RAMSHU};

	static const unsigned int MMC_MEMMAP_SIZE = 12;
	static address_t MMC_REGS_ADDRESSES[MMC_MEMMAP_SIZE];


	static const uint16_t REGISTERS_SPACE_SIZE	= 0x800;	// MCS12XDP512 has 2k-bytes of io-register space
															// mapped at low address of memory

	static const uint8_t GPAGE_LOW			= 0x00;		// low gpage register value
	static const uint8_t GPAGE_HIGH			= 0x7F;		// high gpage register value

	static const uint8_t RPAGE_LOW			= 0xF8;		// low rpage (ram page) register value
	static const uint8_t RPAGE_HIGH			= 0xFF;		// high rpage register value
	static const uint8_t URPAGE_1			= 0xFE;		// unpaged RAM page 0xFE -> 0x1000-0x1FFF
	static const uint8_t URPAGE_2			= 0xFF;		// unpaged RAM page 0xFF -> 0x2000-0x2FFF
	static const uint8_t EPAGE_LOW			= 0xFC;		// low epage (eeprom page) register value
	static const uint8_t EPAGE_HIGH			= 0xFF;		// high epage register value
	static const uint8_t UEPAGE				= 0xFF;		// unpaged EEPROM page 0xFF -> 0x0C00-0x0FFF
	static const uint8_t PPAGE_LOW			= 0xE0;		// low ppage (flash page) register value
	static const uint8_t PPAGE_HIGH			= 0xFF;		// high ppage register value
	static const uint8_t UPPAGE_1			= 0xFD;		// unpaged FLASH page 0xFD -> 0x4000-0x7FFF
	static const uint8_t UPPAGE_2			= 0xFF;		// unpaged FLASH page 0xFF -> 0xC000-0xFFFF

	static const uint8_t XGATE_RAM_PAGE	= 0xF8;
	static const uint8_t XGATE_FLASH_PAGE	= 0xE0;

	static const uint8_t MMCCTL0_RESET		= 0x00;
	// MODC=1, MODB=0, MODA=0 => Normal single-chip
	static const uint8_t MMC_MODE_RESET		= 0x80;

	static const uint8_t GLOBAL_RESET_PAGE	= GPAGE_LOW;// reset gpage register value
	static const uint8_t DIRECT_RESET_PAGE	= 0x00;		// reset direct register value
	// EROMON=EROMCTL=1	=> Enable the emulated flash or ROM in memoryMap
	// ROMHM=0  => The fixed page of Flash or ROM can be accessed in the lower half of the memory map.
	//             Accesses to $4000–$7FFF will be mapped to $7F_4000-$7F_7FFF in the global memory space
	// ROMHM=1  => Disables access to the Flash or ROM in the lower half of the memory map.
	//             These physical locations of the Flash or ROM can still be accessed through the program page window.
	//             Accesses to $4000–$7FFF will be mapped to $14_4000-$14_7FFF in the global memory space (external access).
	// ROMON=ROMCTL=1 => Enable the flash or ROM in memoryMap
	static const uint8_t MMCCTL1_RESET		= 0x05;

	static const uint8_t RAM_RESET_PAGE		= 0xFD;		// reset rpage register value
	static const uint8_t EEPROM_RESET_PAGE	= 0xFE;		// reset epage register value
	static const uint8_t FLASH_RESET_PAGE	= 0xFE;		// reset ppage register value

	static const uint8_t RAMWPC_RESET		= 0x00;
	static const uint8_t RAMXGU_RESET		= 0xFF;
	static const uint8_t RAMSHL_RESET		= 0xFF;
	static const uint8_t RAMSHU_RESET		= 0xFF;


	// ***************************************

	ServiceExport<Memory<physical_address_t> > memory_export;
	ServiceImport<Memory<physical_address_t> > memory_import;

	ServiceExport<Registers> registers_export;

    MMC(const char *name, S12MPU_IF *_mpu = 0, Object *parent = 0);
    ~MMC();

    inline physical_address_t getCPU12XPhysicalAddress(address_t logicalAddress, ADDRESS::MODE type=ADDRESS::EXTENDED, bool isGlobal=false, bool debugload = false, uint8_t debug_page = 0xFF);
    inline physical_address_t getCPU12XPagedAddress(address_t logicalAddress);
    inline bool isPaged(address_t addr);

    inline physical_address_t getXGATEPhysicalAddress(address_t logicalAddress);
    inline physical_address_t getXGATEPagedAddress(address_t logicalAddress);

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================


	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	virtual void OnDisconnect();
	virtual void Reset();


	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================

	virtual void ResetMemory();
	virtual bool ReadMemory(physical_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(physical_address_t addr, const void *buffer, uint32_t size);
	void splitPagedAddress(physical_address_t paged_addr, page_t &page, address_t &cpu_address);

	//=====================================================================
	//=             MMC Registers Interface interface methods               =
	//=====================================================================

	/**
	 * Gets a register interface to the register specified by name.
	 *
	 * @param name The name of the requested register.
	 * @return A pointer to the RegisterInterface corresponding to name.
	 */
    virtual Register *GetRegister(const char *name);
	virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);

	//=====================================================================
	//=             Internal Registers Access methods                     =
	//=====================================================================

    inline uint8_t getMmcctl0 ();
    inline uint8_t getMode ();
    inline uint8_t getGpage ();
    inline uint8_t getDirect ();
    inline uint8_t getMmcctl1 ();
    inline uint8_t getRpage ();
    inline uint8_t getEpage ();
    inline void setPpage(uint8_t page);
    inline uint8_t getPpage ();
    inline uint8_t getRamwpc ();
    inline uint8_t getRamxgu ();
    inline uint8_t getRamshl ();
    inline uint8_t getRamshu ();

    inline physical_address_t getDirectAddress(uint8_t lowByte);

    inline physical_address_t getRamAddress(address_t logicalAddress, bool isGlobal, bool debugload, uint8_t debug_page);
    inline physical_address_t getPagedRamAddress(address_t cpu_address);

    inline physical_address_t getEepromAddress(address_t logicalAddress, bool isGlobal, bool debugload, uint8_t debug_page);
    inline physical_address_t getPagedEepromAddress(address_t cpu_address);

    inline physical_address_t getFlashAddress(address_t logicalAddress, bool isGlobal, bool debugload, uint8_t debug_page);
    inline physical_address_t getPagedFlashAddress(address_t cpu_address);

	//=====================================================================
	//=             memory map access methods                             =
	//=====================================================================
	virtual bool read(unsigned int offset, const void *buffer, unsigned int data_length);
	virtual bool write(unsigned int offset, const void *buffer, unsigned int data_length);

protected:
	S12MPU_IF *mpu;

	bool	debug_enabled;
	Parameter<bool>	param_debug_enabled;

	string version;
	Parameter<string> param_version;

private:
	//=============================================
	//=            MMC REGISTERS                  =
	//=============================================

    uint8_t mmcctl0;
	uint8_t mode;
    uint8_t gpage;
    uint8_t direct;
	uint8_t mmcctl1;
    uint8_t rpage;
    uint8_t epage;
    uint8_t ppage;

    uint8_t ramwpc;
	uint8_t ramxgu;
	uint8_t ramshl;
	uint8_t ramshu;

	bool directSet;

	uint8_t mode_int;
	uint8_t mmcctl1_int;
	Parameter<uint8_t> param_mode;
	Parameter<uint8_t> param_mmcctl1;

	uint8_t	address_encoding;
	Parameter<uint8_t> param_address_encoding;

	address_t ppage_address;
	Parameter<address_t> param_ppage_address;

	// Registers map
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	std::vector<unisim::kernel::VariableBase*> extended_registers_registry;

};



inline bool MMC::isPaged(address_t addr) {

	// EEPROM window
	if ((addr >= CPU12X_EEPROM_WIN_LOW_OFFSET) && (addr <= CPU12X_EEPROM_WIN_HIGH_OFFSET)) {
		return (true);
	}

	// RAM window
	if ((addr >= CPU12X_RAM_WIN_LOW_OFFSET) && (addr <= CPU12X_RAM_WIN_HIGH_OFFSET)) {
		return (true);
	}

	// FLASH window
	if ((addr >= CPU12X_FLASH_WIN_LOW_OFFSET) && (addr <= CPU12X_FLASH_WIN_HIGH_OFFSET)) {
		return (true);
	}

	return (false);
}


inline physical_address_t MMC::getCPU12XPagedAddress(address_t cpu_address) {

	physical_address_t address = cpu_address;

	if ((cpu_address >= CPU12X_EEPROM_LOW_OFFSET) && (cpu_address <= CPU12X_EEPROM_HIGH_OFFSET)) { // Access to EEPROM
		address = getPagedEepromAddress(cpu_address);
	}

	if ((cpu_address >= CPU12X_RAM_LOW_OFFSET) && (cpu_address <= CPU12X_RAM_HIGH_OFFSET)) { // Access to RAM
		address = getPagedRamAddress(cpu_address);
	}

	if (cpu_address >= CPU12X_FLASH_LOW_OFFSET) { // Access to Flash
		address = getPagedFlashAddress(cpu_address);
	}

	return (address);
}


inline physical_address_t MMC::getCPU12XPhysicalAddress(address_t logicalAddress, ADDRESS::MODE type, bool isGlobal, bool debugload, uint8_t debug_page) {

	static const uint8_t gShift = 16;	// CPU12 address size is 16-bits

	address_t cpu_address =  logicalAddress;
	physical_address_t address = cpu_address;

	if (type == ADDRESS::DIRECT) {
		cpu_address = getDirectAddress((uint8_t) logicalAddress);
	}

	if (cpu_address <= GLOBAL_REG_HIGH_OFFSET) { // Access to registers
		address = cpu_address;
	}

	if (isGlobal) {

		address = ((physical_address_t) (getGpage() & 0x7F) << gShift) | cpu_address;

	} else {

		if ((cpu_address >= CPU12X_EEPROM_LOW_OFFSET) && (cpu_address <= CPU12X_EEPROM_HIGH_OFFSET)) { // Access to EEPROM
			address = getEepromAddress(cpu_address, false, debugload, debug_page);
		}

		if ((cpu_address >= CPU12X_RAM_LOW_OFFSET) && (cpu_address <= CPU12X_RAM_HIGH_OFFSET)) { // Access to RAM
			address = getRamAddress(cpu_address, false, debugload, debug_page);
		}

		if (cpu_address >= CPU12X_FLASH_LOW_OFFSET) { // Access to Flash
			address = getFlashAddress(cpu_address, false, debugload, debug_page);
		}

	}

	return (address);
}


inline physical_address_t MMC::getXGATEPhysicalAddress(address_t cpu_address)
{
	/**
	 * The XGATE memory map is linear and static. There are no mapping or page registers.
	 */

	physical_address_t address = cpu_address;

	if ((cpu_address >= XGATE_RAM_LOW_OFFSET) && (cpu_address <= XGATE_RAM_HIGH_OFFSET)) { // Access to RAM
		uint8_t current_ram_page = XGATE_RAM_PAGE + (cpu_address - XGATE_RAM_LOW_OFFSET) / RAM_PAGE_SIZE;
		address = ((physical_address_t) current_ram_page << RAM_ADDRESS_SIZE) | ((address_t) RAM_ADDRESS_BITS & cpu_address);
	}

	if ((cpu_address >= XGATE_FLASH_LOW_OFFSET) && (cpu_address <= XGATE_FLASH_HIGH_OFFSET)) { // Access to Flash
		static const physical_address_t shifted_gpage = 0x1 << (FLASH_ADDRESS_SIZE + 8); // 1 PPAGE CPUAddr

		uint8_t current_flash_page = XGATE_FLASH_PAGE + (cpu_address - XGATE_FLASH_LOW_OFFSET) / FLASH_PAGE_SIZE;

		address = (shifted_gpage | ((physical_address_t) current_flash_page << FLASH_ADDRESS_SIZE) | ((address_t) FLASH_ADDRESS_BITS & cpu_address));
	}

	return (address);
}


inline physical_address_t MMC::getXGATEPagedAddress(address_t cpu_address) {

	physical_address_t address = cpu_address;

	if ((cpu_address >= XGATE_RAM_LOW_OFFSET) && (cpu_address <= XGATE_RAM_HIGH_OFFSET)) { // Access to RAM
		uint8_t current_ram_page = XGATE_RAM_PAGE + (cpu_address - XGATE_RAM_LOW_OFFSET) / RAM_PAGE_SIZE;
		address = (((physical_address_t) current_ram_page << (sizeof(address_t) * 8)) | cpu_address);
	}

	if ((cpu_address >= XGATE_FLASH_LOW_OFFSET) && (cpu_address <= XGATE_FLASH_HIGH_OFFSET)) { // Access to Flash
		uint8_t current_flash_page = XGATE_FLASH_PAGE + (cpu_address - XGATE_FLASH_LOW_OFFSET) / FLASH_PAGE_SIZE;
		address = (((physical_address_t) current_flash_page << (sizeof(address_t) * 8)) | cpu_address);
	}

	return (address);
}


//=====================================================================
//=             registers setters and getters                         =
//=====================================================================


inline bool MMC::read(unsigned int address, const void *buffer, unsigned int data_length)
{

	if (address == MMC_REGS_ADDRESSES[MMCCTL0]) { *((uint8_t *) buffer) = mmcctl0; return (true); }
	if (address == MMC_REGS_ADDRESSES[MODE]) { *((uint8_t *) buffer) = mode; return (true); }
	if (address == MMC_REGS_ADDRESSES[GPAGE]) { *((uint8_t *) buffer) = gpage; return (true); }
	if (address == MMC_REGS_ADDRESSES[DIRECT]) { *((uint8_t *) buffer) = direct; return (true); }
	if (address == MMC_REGS_ADDRESSES[MMCCTL1]) { *((uint8_t *) buffer) = mmcctl1; return (true); }
	if (address == MMC_REGS_ADDRESSES[RPAGE]) { *((uint8_t *) buffer) = rpage; return (true); }
	if (address == MMC_REGS_ADDRESSES[EPAGE]) { *((uint8_t *) buffer) = epage; return (true); }
	if (address == MMC_REGS_ADDRESSES[PPAGE]) { *((uint8_t *) buffer) = ppage; return (true); }
	if (address == MMC_REGS_ADDRESSES[RAMWPC]) { *((uint8_t *) buffer) = ramwpc; return (true); }
	if (address == MMC_REGS_ADDRESSES[RAMXGU]) { *((uint8_t *) buffer) = ramxgu; return (true); }
	if (address == MMC_REGS_ADDRESSES[RAMSHL]) { *((uint8_t *) buffer) = ramshl; return (true); }
	if (address == MMC_REGS_ADDRESSES[RAMSHU]) { *((uint8_t *) buffer) = ramshu; return (true); }

	std::stringstream sstr;
	sstr << "MMC::read: unhandled address 0x" << std::hex << address << std::dec;
	return (false);
}


inline bool MMC::write(unsigned int address, const void *buffer, unsigned int data_length)
{

	uint8_t val = *((uint8_t *) buffer);

	if (address == MMC_REGS_ADDRESSES[MMCCTL0]) { mmcctl0 = val; return (true); }
	if (address == MMC_REGS_ADDRESSES[MODE]) { mode = val; return (true); }
	if (address == MMC_REGS_ADDRESSES[GPAGE]) { gpage = val; return (true); }
	if (address == MMC_REGS_ADDRESSES[DIRECT]) {
		if (!directSet) {
			direct = val;
			directSet = true;
		}
		return (true);
	}
	if (address == MMC_REGS_ADDRESSES[MMCCTL1]) { mmcctl1 = val; return (true); }
	if (address == MMC_REGS_ADDRESSES[RPAGE]) { rpage = val; return (true); }
	if (address == MMC_REGS_ADDRESSES[EPAGE]) { epage = val; return (true); }
	if (address == MMC_REGS_ADDRESSES[PPAGE]) { ppage = val; return (true); }
	if (address == MMC_REGS_ADDRESSES[RAMWPC]) { ramwpc = val; return (true); }
	if (address == MMC_REGS_ADDRESSES[RAMXGU]) { ramxgu = val; return (true); }
	if (address == MMC_REGS_ADDRESSES[RAMSHL]) { ramshl = val; return (true); }
	if (address == MMC_REGS_ADDRESSES[RAMSHU]) { ramshu = val; return (true); }

	return (false);
}


inline uint8_t MMC::getMmcctl0 () { return (mmcctl0); }


inline uint8_t MMC::getMode () { return (mode); }


inline uint8_t MMC::getMmcctl1 () { return (mmcctl1); }


inline uint8_t MMC::getRamwpc () { return (ramwpc); }


inline uint8_t MMC::getRamxgu () { return (ramxgu); }


inline uint8_t MMC::getRamshl () { return (ramshl); }


inline uint8_t MMC::getRamshu () { return (ramshu); }


inline uint8_t MMC::getGpage () { return (gpage); }


inline uint8_t MMC::getDirect () { return (direct); }


inline physical_address_t MMC::getDirectAddress(uint8_t lowByte) {

	uint8_t _direct = getDirect ();

	if (_direct != 0)
	{
		return ((((address_t) getDirect()) << DIRECT_ADDRESS_SIZE) | ((address_t) lowByte & 0x00FF));
	} else
	{
		return (lowByte);
	}

}


inline uint8_t MMC::getRpage () { return (rpage); }


inline physical_address_t MMC::getRamAddress(address_t logicalAddress, bool isGlobal, bool debugload, uint8_t debug_page) {

	uint8_t _rpage;
	physical_address_t shifted_gpage = 0x0 << (RAM_ADDRESS_SIZE + 8); // 000 RPAGE CPUAddr

	if (debugload) {
		if (debug_page != 0x00) {
			_rpage = debug_page;
		} else {
			if ((logicalAddress > 0x1FFF) && (logicalAddress < 0x3000)) {
				_rpage = URPAGE_1;
			}
			else if ((logicalAddress > 0x2FFF) && (logicalAddress < 0x4000)) {
				_rpage = URPAGE_2;
			} else {
				_rpage = getRpage();
			}
		}

	} else {
		_rpage = getRpage();
	}

	if ((_rpage > 0x00) && (_rpage < RPAGE_LOW)) {
		throw NonMaskableAccessErrorInterrupt(NonMaskableAccessErrorInterrupt::INVALIDE_RPAGE);
	}

	if (isPaged(logicalAddress)) {
		return (shifted_gpage | ((physical_address_t) _rpage << RAM_ADDRESS_SIZE) | ((address_t) RAM_ADDRESS_BITS & logicalAddress));
	} else {
		if ((logicalAddress > 0x1FFF) && (logicalAddress < 0x3000)) {
			return (shifted_gpage | ((physical_address_t) URPAGE_1 << RAM_ADDRESS_SIZE) | ((address_t) RAM_ADDRESS_BITS & logicalAddress));
		}
		if ((logicalAddress > 0x2FFF) && (logicalAddress < 0x4000)) {
			return (shifted_gpage | ((physical_address_t) URPAGE_2 << RAM_ADDRESS_SIZE) | ((address_t) RAM_ADDRESS_BITS & logicalAddress));
		}
	}

	return (logicalAddress);
}


inline physical_address_t MMC::getPagedRamAddress(address_t logicalAddress) {


	if ((logicalAddress > 0x1FFF) && (logicalAddress < 0x3000)) {
		return (logicalAddress);
	}
	else if ((logicalAddress > 0x2FFF) && (logicalAddress < 0x4000)) {
		return (logicalAddress);
	}
	else {
		return (((physical_address_t) getRpage() << (sizeof(address_t) * 8)) | logicalAddress);
	}

}


inline uint8_t MMC::getEpage () { return (epage); }


inline physical_address_t MMC::getEepromAddress(address_t logicalAddress, bool isGlobal, bool debugload, uint8_t debug_page) {

	uint8_t _epage;
	physical_address_t shifted_gpage = 0x4 << (EEPROM_ADDRESS_SIZE + 8); // 00100 RPAGE CPUAddr

	if (debugload) {
		if (debug_page != 0x00) {
			_epage = debug_page;
		} else {
			if ((logicalAddress > 0x0BFF) && (logicalAddress < 0x1000)) {
				_epage = UEPAGE;
			} else {
				_epage = getEpage();
			}
		}
	} else {
		_epage = getEpage();
	}

	if ((_epage > 0) && (_epage < EPAGE_LOW)) {
		throw NonMaskableAccessErrorInterrupt(NonMaskableAccessErrorInterrupt::INVALIDE_EPAGE);
	}

	if (isPaged(logicalAddress)) {
		return (shifted_gpage | ((physical_address_t) _epage << EEPROM_ADDRESS_SIZE) | ((address_t) EEPROM_ADDRESS_BITS & logicalAddress));
	} else {
		if ((logicalAddress > 0x0BFF) && (logicalAddress < 0x1000)) {
			return (shifted_gpage | ((physical_address_t) UEPAGE << EEPROM_ADDRESS_SIZE) | ((address_t) EEPROM_ADDRESS_BITS & logicalAddress));
		}
	}

	return (logicalAddress);
}


inline physical_address_t MMC::getPagedEepromAddress(address_t logicalAddress) {

	if ((logicalAddress > 0x0BFF) && (logicalAddress < 0x1000)) {
		return (logicalAddress);
	}
	else {
		return (((physical_address_t) getEpage() << (sizeof(address_t) * 8)) | logicalAddress);
	}

}


inline uint8_t MMC::getPpage () { return (ppage); }


inline void MMC::setPpage(uint8_t page) { ppage = page; }


inline physical_address_t MMC::getFlashAddress(address_t logicalAddress, bool isGlobal, bool debugload, uint8_t debug_page) {

	static const uint8_t ROMHM_MASK = 0x02;

	uint8_t _ppage;
	physical_address_t shifted_gpage = 0x1 << (FLASH_ADDRESS_SIZE + 8); // 1 PPAGE CPUAddr

	if (debugload) {
		if (debug_page != 0x00) {
			_ppage = debug_page;
		} else {
			if ((logicalAddress > 0x3FFF) && (logicalAddress < 0x8000)) {
				_ppage = UPPAGE_1;
			} else if ((logicalAddress > 0xBFFF) && (logicalAddress <= 0xFFFF)) {
				_ppage = UPPAGE_2;
			} else {
				_ppage = getPpage();
			}
		}

	} else {
		// if ((ROMHM == 1) && logical_address in [0x4000-0x8000]) then  External Access window [0x14_4000, 0x14_8000]
		if ((logicalAddress > 0x3FFF) && (logicalAddress < 0x8000)) {
			if ((getMmcctl1() & ROMHM_MASK) != 0) { // 0x4000-0x7FFF is mapped to 0x14_4000-0x14_7FFF (external access)
				return ((physical_address_t) (0x14 << 16) | logicalAddress);
			} else { // 0x4000-0x7FFF is mapped to 0x7F_4000-0x7F_7FFF (page = 0xFD)
				_ppage = 0xFD;
			}
		} else {
			_ppage = getPpage();
		}
	}

	if (isPaged(logicalAddress)) {
		return (shifted_gpage | ((physical_address_t) _ppage << FLASH_ADDRESS_SIZE) | ((address_t) FLASH_ADDRESS_BITS & logicalAddress));
	} else {
		if ((logicalAddress > 0x3FFF) && (logicalAddress < 0x8000)) {
			return (shifted_gpage | ((physical_address_t) UPPAGE_1 << FLASH_ADDRESS_SIZE) | ((address_t) FLASH_ADDRESS_BITS & logicalAddress));
		}
		if ((logicalAddress > 0xBFFF) && (logicalAddress <= 0xFFFF)) {
			return (shifted_gpage | ((physical_address_t) UPPAGE_2 << FLASH_ADDRESS_SIZE) | ((address_t) FLASH_ADDRESS_BITS & logicalAddress));
		}
	}

	return (logicalAddress);

}


inline physical_address_t MMC::getPagedFlashAddress(address_t logicalAddress) {

	if ((logicalAddress > 0x3FFF) && (logicalAddress < 0x8000)) {
		return (logicalAddress);
	}
	else if ((logicalAddress > 0xBFFF) && (logicalAddress <= 0xFFFF)) {
		return (logicalAddress);
	} else {
		return (((physical_address_t) getPpage() << (sizeof(address_t) * 8)) | logicalAddress);
	}

}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_MMC_HH__


