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

#include <string.h>
#include <iostream>
#include <set>
#include <sstream>
#include <fstream>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/likely/likely.hh>

#ifndef __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29_AM29_TCC__
#define __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29_AM29_TCC__

namespace unisim {
namespace component {
namespace cxx {
namespace memory {
namespace flash {
namespace am29 {

using namespace std;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::kernel::Object;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
AM29<CONFIG, BYTESIZE, IO_WIDTH>::AM29(const char *name, Object *parent)
	: Object(name, parent, "AM29LVxxx flash memory")
	, Service<Memory<typename CONFIG::ADDRESS> >(name, parent)
	, memory_export("memory-export", this)
	, logger(*this)
	, org(0)
	, bytesize(BYTESIZE)
	, endian(E_LITTLE_ENDIAN)
	, verbose(false)
	, fsm_to_graphviz_output_filename()
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, param_org("org", this, org, "flash memory base address")
	, param_bytesize("bytesize", this, bytesize, "flash memory size in bytes")
	, param_endian("endian", this, endian, "endianness of flash memory")
	, param_sector_protect("sector-protect", this, sector_protect, CONFIG::NUM_SECTORS, "enable/disable sector write protection")
	, param_fsm_to_graphviz_output_filename("fsm-to-graphviz-output-filename", this, fsm_to_graphviz_output_filename, "FSM (finite state machine) to Graphviz output filename")
{
	param_bytesize.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

	uint32_t chip_io_width;
	for(config_addr_shift = 0, chip_io_width = CHIP_IO_WIDTH; chip_io_width > 1; chip_io_width >>= 1, config_addr_shift++);

	uint32_t io_width;
	for(addr_shift = 0, io_width = IO_WIDTH; io_width > 1; io_width >>= 1, addr_shift++);

	unsigned int chip_num;
	for(chip_num = 0; chip_num < NUM_CHIPS; chip_num++)
	{
		state[chip_num] = (typename CONFIG::STATE) 0;
		write_status[chip_num] = (WRITE_STATUS) 0;
	}

	unsigned int sector_num;
	for(sector_num = 0; sector_num < CONFIG::NUM_SECTORS; sector_num++)
	{
		sector_protect[sector_num] = false;
	}

	storage = new uint8_t[BYTESIZE];
	memset(storage, 0xff, BYTESIZE);  // At reset, chip bits are all 1's

	std::stringstream sstr_description;
	sstr_description << "This module implements an " << CONFIG::DEVICE_NAME << " flash memory with the following characteristics:" << std::endl;
	sstr_description << "Manufacturer ID: ";
	PrintData(sstr_description, CONFIG::MANUFACTURER_ID, CONFIG::MAX_IO_WIDTH);
	sstr_description << std::endl;
	unsigned int device_id_word_index;
	for(device_id_word_index = 0; device_id_word_index < CONFIG::DEVICE_ID_LENGTH; device_id_word_index++)
	{
		sstr_description << "Device ID word #" << device_id_word_index << ": ";
		PrintData(sstr_description, CONFIG::DEVICE_ID[device_id_word_index], CONFIG::MAX_IO_WIDTH);
		sstr_description << std::endl;
	}
	sstr_description << "Size: " << BYTESIZE << " bytes" << std::endl;
	sstr_description << "I/O width: " << (8 * IO_WIDTH) << " bits" << std::endl;
	sstr_description << "Number of chips: " << NUM_CHIPS << " chip" << ((NUM_CHIPS > 1) ? "s" : "") << std::endl;
	sstr_description << "I/O width per chip: " << (8 * CHIP_IO_WIDTH) << " bits" << std::endl;
	sstr_description << "Size per chip: " << CONFIG::BYTESIZE << " bytes" << std::endl;
	sstr_description << "Number of Sectors: " << CONFIG::NUM_SECTORS << " sectors" << std::endl;
	sstr_description << "8-bit mode support: " << (CONFIG::MODE_SUPPORT & MODE_X8 ? "yes" : "no") << std::endl;
	sstr_description << "16-bit mode support: " << (CONFIG::MODE_SUPPORT & MODE_X16 ? "yes" : "no") << std::endl;
	sstr_description << "Access time: " << CONFIG::ACCESS_TIME << " ns" << std::endl;
	sstr_description << "Byte programming time: " << CONFIG::PROGRAMMING_TIME[0] << " us" << std::endl;
	sstr_description << "Word programming time: " << CONFIG::PROGRAMMING_TIME[1] << " us" << std::endl;
	sstr_description << "Sector erasing time: " << CONFIG::SECTOR_ERASING_TIME << " us" << std::endl;
	sstr_description << "Chip erasing time: " << CONFIG::CHIP_ERASING_TIME << " us" << std::endl;
	Object::SetDescription(sstr_description.str().c_str());
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
AM29<CONFIG, BYTESIZE, IO_WIDTH>::~AM29()
{
	delete[] storage;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
bool AM29<CONFIG, BYTESIZE, IO_WIDTH>::BeginSetup()
{
	unsigned int chip_num;

	for(chip_num = 0; chip_num < NUM_CHIPS; chip_num++)
	{
		state[chip_num] = (typename CONFIG::STATE) 0;
	}

	memset(storage, 0xff, BYTESIZE); // At reset, chip bits are all 1's

	if((IO_WIDTH / NUM_CHIPS) > CONFIG::MAX_IO_WIDTH)
	{
		logger << DebugError;
		logger << "bad I/O Width (currently: " << IO_WIDTH << ", suggested: " << (NUM_CHIPS * CONFIG::MAX_IO_WIDTH) << ")";
		logger << EndDebugError;
		return false;
	}
	
	if(!(CONFIG::MODE_SUPPORT & (IO_WIDTH / NUM_CHIPS)))
	{
		logger << DebugError;
		logger << "Chip does not support " << (IO_WIDTH / NUM_CHIPS) << " bits I/O: decrease or increase either BYTESIZE (" << BYTESIZE << ") or IO_WIDTH (" << IO_WIDTH << ")";
		logger << EndDebugError;
		return false;
	}

	if(!fsm_to_graphviz_output_filename.empty())
	{
		FSMToGraphviz();
	}
	return true;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::ResetMemory()
{
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
bool AM29<CONFIG, BYTESIZE, IO_WIDTH>::ReverseCompare(const uint8_t *data1, const uint8_t *data2, uint32_t size)
{
	if(size > 0)
	{
		const uint8_t *p1 = data1;
		const uint8_t *p2 = data2 + size - 1;
		do
		{
			if(*p1 != *p2) return false;
		} while(++p1, --p2, --size);
	}
	return true;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::ReverseCopy(uint8_t *dest, const uint8_t *source, uint32_t size)
{
	if(size > 0)
	{
		uint8_t *dst = dest;
		const uint8_t *src = source + size - 1;
		do
		{
			*dst = *src;
		} while(++dst, --src, --size);
	}
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::Combine(uint8_t *dest, const uint8_t *source, uint32_t size)
{
	// Do a AND between destination (flash memory content) and source (data to be programmed)
	// A 1 can be transformed into a 0, but a zero can't be transformed into a 1
	if(size > 0)
	{
		uint8_t *dst = dest;
		const uint8_t *src = source;
		do
		{
			*dst = (*dst) & (*src);
		} while(++dst, ++src, --size);
	}
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
int AM29<CONFIG, BYTESIZE, IO_WIDTH>::GetSector(typename CONFIG::ADDRESS addr)
{
	unsigned int sector_num;

	for(sector_num = 0; sector_num < CONFIG::NUM_SECTORS; sector_num++)
	{
		if((addr >= CONFIG::SECTOR_MAP[sector_num].addr) &&
		   (addr < (CONFIG::SECTOR_MAP[sector_num].addr + CONFIG::SECTOR_MAP[sector_num].size)))
		{
			return sector_num;
		}
	}
	return -1;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
int AM29<CONFIG, BYTESIZE, IO_WIDTH>::GetPage(typename CONFIG::ADDRESS addr)
{
	return addr / (CONFIG::PAGE_SIZE * CHIP_IO_WIDTH);
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::FSM(unsigned int chip_num, COMMAND command, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size)
{
	unsigned int i;

	typename CONFIG::ADDRESS offset = (addr - org) & ADDR_MASK; // keep only needed address bits.
	typename CONFIG::ADDRESS chip_addr = offset >> addr_shift;
	
	if(IsVerbose())
	{
		logger << DebugInfo;
		logger << "Chip #" << chip_num << ": addr=0x" << std::hex << chip_addr << std::dec << ", data=";
		std::stringstream sstr;
		PrintData(sstr, data, CHIP_IO_WIDTH);
		logger << sstr.str();
		logger << EndDebugInfo;
	}

	for(i = 0; i < CONFIG::NUM_TRANSITIONS; i++)
	{
		const TRANSITION<typename CONFIG::ADDRESS, CONFIG::MAX_IO_WIDTH, typename CONFIG::STATE> *transition = &CONFIG::FSM[i];

		if(IsVerbose())
		{
			logger << DebugInfo;
			logger << "Chip #" << chip_num << ": looking at transition (" << GetStateName(transition->initial_state) << ") -[";
			logger << GetCommandName(command) << ",";
			if(transition->wildcard_addr)
			{
				logger << "*";
			}
			else
			{
				logger << "0x" << std::hex << (transition->addr >> config_addr_shift) << std::dec;
			}
			logger << ",";
			if(transition->wildcard_data)
			{
				logger << "*";
			}
			else
			{
				std::stringstream sstr;
				PrintData(sstr, transition->data, CHIP_IO_WIDTH);
				logger << sstr.str();
			}
			logger << "," << GetActionName(transition->action) << "]->";
			logger << GetStateName(transition->final_state);
			logger << EndDebugInfo;
		}

		if(((transition->initial_state == CONFIG::ST_ANY) || (transition->initial_state == state[chip_num])) &&
		   transition->command == command &&
		   (transition->wildcard_addr || (!transition->wildcard_addr && ((transition->addr >> config_addr_shift) == chip_addr))) &&
		   (transition->wildcard_data || (!transition->wildcard_data && size >= CHIP_IO_WIDTH && (endian == E_LITTLE_ENDIAN ? memcmp(transition->data, data, CHIP_IO_WIDTH) == 0 : ReverseCompare(transition->data, data, CHIP_IO_WIDTH)))))
		{
			if(IsVerbose())
			{
				logger << DebugInfo;
				logger << "Chip #" << chip_num << ": selecting transition (" << GetStateName(transition->initial_state) << ") -[";
				logger << GetCommandName(command) << ",";
				if(transition->wildcard_addr)
				{
					logger << "*";
				}
				else
				{
					logger << "0x" << std::hex << (transition->addr >> config_addr_shift) << std::dec;
				}
				logger << ",";
				if(transition->wildcard_data)
				{
					logger << "*";
				}
				else
				{
					std::stringstream sstr;
					PrintData(sstr, transition->data, CHIP_IO_WIDTH);
					logger << sstr.str();
				}
				logger << "," << GetActionName(transition->action) << "]->";
				logger << GetStateName(transition->final_state);
				logger << EndDebugInfo;
			}

			if(transition->final_state != CONFIG::ST_ANY)
			{
				state[chip_num] = transition->final_state;
			}
			switch(transition->action)
			{
				case ACT_NOP: return;
				case ACT_READ: Read(chip_num, offset, data, size); return;
				case ACT_READ_WRITE_STATUS: ReadWriteStatus(chip_num, data, size); return;
				case ACT_READ_AUTOSELECT: ReadAutoselect(chip_num, offset, data, size); return;
				case ACT_CFI_QUERY: CFIQuery(chip_num, offset, data, size); return;
				case ACT_PROGRAM: Program(chip_num, offset, data, size); return;
				case ACT_OPEN_PAGE: OpenPage(chip_num, offset); return;
				case ACT_LOAD_WC: LoadWordCount(chip_num, offset, data, size); return;
				case ACT_WRITE_TO_BUFFER: WriteToBuffer(chip_num, offset, data, size); return;
				case ACT_PROGRAM_BUFFER_TO_FLASH: ProgramBufferToFlash(chip_num, offset); return;
				case ACT_CHIP_ERASE: ChipErase(chip_num); return;
				case ACT_SECTOR_ERASE: SectorErase(chip_num, offset); return;
				case ACT_SECURED_SILICON_SECTOR_ENTRY: SecuredSiliconSectorEntry(chip_num); return;
				case ACT_SECURED_SILICON_SECTOR_EXIT: SecuredSiliconSectorExit(chip_num); return;
			}
		}
	}
	logger << DebugWarning;
	logger << "No transition found:" << std::endl;
	logger << "- command is " << GetCommandName(command) << std::endl;
	logger << "- state is \"" << GetStateName(state[chip_num]) << "\"" << std::endl;
	logger << "- address is 0x" << std::hex << chip_addr << std::dec << std::endl;
	if(command == CMD_WRITE)
	{
		std::stringstream sstr;
		logger << "- data is ";
		PrintData(sstr, data, size);
		logger << sstr.str() << std::endl;
	}
	logger << "No state change or action were performed. You may check the FSM";
	logger << EndDebugWarning;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
bool AM29<CONFIG, BYTESIZE, IO_WIDTH>::FSM(COMMAND command, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size)
{
	if(addr < org || (addr + size - 1) > (org + bytesize - 1) || (addr + size) < addr)
	{
		logger << DebugWarning;
		logger << "out of range address";
		logger << EndDebugWarning;
		return false;
	}

	if(size > IO_WIDTH)
	{
		logger << DebugWarning;
		logger << "invalid transfer size (" << size << " bytes). Transfer size should be <= " << IO_WIDTH;
		logger << EndDebugWarning;
		return false;
	}

	unsigned int chip_num = (addr / CHIP_IO_WIDTH) % NUM_CHIPS;
	int32_t sz = (int32_t) size;

	do
	{
		FSM(chip_num, command, addr, data, sz > (int32_t) CHIP_IO_WIDTH ? CHIP_IO_WIDTH : sz);
	} while(data += CHIP_IO_WIDTH, addr += CHIP_IO_WIDTH, chip_num = (chip_num + 1) % NUM_CHIPS, (sz -= CHIP_IO_WIDTH) > 0);

	return true;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::Read(unsigned int chip_num, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size)
{
	memcpy(data, storage + addr, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::ReadWriteStatus(unsigned int chip_num, uint8_t *data, uint32_t size)
{
	memset(data, 0, size);
	if(endian == E_LITTLE_ENDIAN)
		*data = write_status[chip_num];
	else
		data[size - 1] = write_status[chip_num];
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::ResetFSM(unsigned int chip_num)
{
	state[chip_num] = (typename CONFIG::STATE) 0;
	write_status[chip_num] = (WRITE_STATUS) 0;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::ReadAutoselect(unsigned int chip_num, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size)
{
	typename CONFIG::ADDRESS chip_addr = addr >> addr_shift;

	typename CONFIG::ADDRESS chip_addr_lo = (chip_addr & 0xff) << config_addr_shift;
	
	switch(chip_addr_lo)
	{
		case CONFIG::MANUFACTURER_ID_ADDR:
			if(unlikely(IsVerbose()))
			{
				std::stringstream sstr;
				logger << DebugInfo;
				logger << "Chip #" << chip_num << ": Reading Manufacturer ID (";
				PrintData(sstr, CONFIG::MANUFACTURER_ID, CHIP_IO_WIDTH);
				logger << sstr.str() << ")";
				logger << EndDebugInfo;
			}
			if(endian == E_LITTLE_ENDIAN)
				memcpy(data, CONFIG::MANUFACTURER_ID, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
			else
				ReverseCopy(data, CONFIG::MANUFACTURER_ID, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
			return;
	
		case CONFIG::SECTOR_PROTECT_VERIFY_ADDR:
			{
				if(unlikely(IsVerbose()))
				{
					logger << DebugInfo;
					logger << "Chip #" << chip_num << ": Sector protected verify";
					logger << EndDebugInfo;
				}
				typename CONFIG::ADDRESS sector_addr = (chip_addr >> 8) << config_addr_shift;
				int sector_num = GetSector(sector_addr);
				
				if(sector_num < 0)
				{
					logger << DebugWarning;
					logger << "Chip #" << chip_num << ": Sector #" << sector_num << " does not exist";
					logger << EndDebugWarning;
				}
				
				if(sector_num >= 0 && sector_protect[sector_num])
				{
					if(IsVerbose())
					{
						logger << DebugInfo;
						logger << "Chip #" << chip_num << ": Sector #" << sector_num << " is protected";
						logger << EndDebugInfo;
					}
					if(endian == E_LITTLE_ENDIAN)
						memcpy(data, CONFIG::PROTECTED, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
					else
						ReverseCopy(data, CONFIG::PROTECTED, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
				}
				else
				{
					if(IsVerbose())
					{
						logger << DebugInfo;
						logger << "Chip #" << chip_num << ": Sector #" << sector_num << " is unprotected";
						logger << EndDebugInfo;
					}
					if(endian == E_LITTLE_ENDIAN)
						memcpy(data, CONFIG::UNPROTECTED, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
					else
						ReverseCopy(data, CONFIG::UNPROTECTED, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
				}
			}
			return;
	}

	unsigned int device_id_word_index;
	for(device_id_word_index = 0; device_id_word_index < CONFIG::DEVICE_ID_LENGTH; device_id_word_index++)
	{
		if(CONFIG::DEVICE_ID_ADDR[device_id_word_index] == chip_addr_lo)
		{
			if(unlikely(IsVerbose()))
			{
				std::stringstream sstr;
				logger << DebugInfo;
				logger << "Chip #" << chip_num << ": Reading Device ID word #" << device_id_word_index << " (";
				PrintData(sstr, CONFIG::DEVICE_ID[device_id_word_index], CHIP_IO_WIDTH);
				logger << sstr.str() << ")";
				logger << EndDebugInfo;
			}
			if(endian == E_LITTLE_ENDIAN)
				memcpy(data, CONFIG::DEVICE_ID[device_id_word_index], size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
			else
				ReverseCopy(data, CONFIG::DEVICE_ID[device_id_word_index], size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
			return;
		}
	}
	
	memset(data, 0, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::CFIQuery(unsigned int chip_num, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size)
{
	typename CONFIG::ADDRESS chip_addr = addr >> addr_shift;

	if(IsVerbose())
	{
		logger << DebugInfo << "Chip #" << chip_num << ": CFI Query at 0x" << std::hex << chip_addr << std::dec << EndDebugInfo;
	}
	
	unsigned int i, j;
	for(i = 0; i < size; i++)
	{
		data[i] = 0;

		for(j = 0; j < CONFIG::NUM_CFI_QUERIES; j++)
		{
			if(((addr + i) >= CONFIG::CFI_QUERIES[j].addr) && ((addr + i) < (CONFIG::CFI_QUERIES[j].addr + CONFIG::MAX_IO_WIDTH)))
			{
				if(endian == E_LITTLE_ENDIAN)
					data[i] = CONFIG::CFI_QUERIES[j].data[addr + i - CONFIG::CFI_QUERIES[j].addr];
				else
					data[i] = CONFIG::CFI_QUERIES[j].data[(CONFIG::MAX_IO_WIDTH - 1) - addr - i + CONFIG::CFI_QUERIES[j].addr];
			}
		}
	}

	if(IsVerbose())
	{
		std::stringstream sstr;
		logger << DebugInfo << "Chip #" << chip_num << ": CFI Query at 0x" << std::hex << chip_addr << std::dec << " returning ";
		PrintData(sstr, data, size);
		logger << sstr.str() << EndDebugInfo;
	}
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::Program(unsigned int chip_num, typename CONFIG::ADDRESS addr, const uint8_t *data, uint32_t size)
{
	typename CONFIG::ADDRESS chip_addr = addr >> addr_shift;
	typename CONFIG::ADDRESS sector_addr = chip_addr << config_addr_shift;
	int sector_num = GetSector(sector_addr);

	if(sector_num < 0)
	{
		logger << DebugWarning;
		logger << "Chip #" << chip_num << ": Sector #" << sector_num << " does not exist";
		logger << EndDebugWarning;
		return;
	}

	if(sector_protect[sector_num])
	{
		logger << DebugWarning;
		logger << "Chip #" << chip_num << ", Sector #" << sector_num << ", Addr 0x" << std::hex << chip_addr << std::dec << ": Attempting to program at byte address 0x" << std::hex << addr << std::dec << " while sector is protected";
		logger << EndDebugWarning;
		return;
	}

	if(unlikely(IsVerbose()))
	{
		logger << DebugInfo;
		logger << "Chip #" << chip_num << ", Sector #" << sector_num << ", Addr 0x" << std::hex << chip_addr << std::dec << ": Programming ";
		uint32_t n = size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size;
		std::stringstream sstr;
		PrintData(sstr, data, n);
		logger << sstr.str() << EndDebugInfo;
	}

	Combine(storage + addr, data, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::OpenPage(unsigned int chip_num, typename CONFIG::ADDRESS addr)
{
	typename CONFIG::ADDRESS chip_addr = addr >> addr_shift;
	typename CONFIG::ADDRESS sector_addr = chip_addr << config_addr_shift;
	int sector_num = GetSector(sector_addr);

	if(sector_num < 0)
	{
		logger << DebugWarning;
		logger << "Sector #" << sector_num << " does not exist";
		logger << EndDebugWarning;
		ResetFSM(chip_num);
		return;
	}
	
	page[chip_num] = GetPage(sector_addr);
	
	if(unlikely(IsVerbose()))
	{
		logger << DebugInfo;
		logger << "Chip #" << chip_num << ": Opening page #" << page[chip_num] << " on sector #" << sector_num;
		logger << EndDebugInfo;
	}
	
	word_count[chip_num] = 0;
	word_index[chip_num] = 0;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::LoadWordCount(unsigned int chip_num, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size)
{
	typename CONFIG::ADDRESS chip_addr = addr >> addr_shift;
	typename CONFIG::ADDRESS sector_addr = chip_addr << config_addr_shift;
	int sector_num = GetSector(sector_addr);
	
	if(sector_num < 0)
	{
		logger << DebugWarning;
		logger << "Sector #" << sector_num << " does not exist. Aborting write to buffer";
		logger << EndDebugWarning;
		AbortWriteToBuffer(chip_num);
		return;
	}
	
	typename CONFIG::ADDRESS req_page = GetPage(sector_addr);
	if(page[chip_num] != req_page)
	{
		logger << DebugWarning;
		logger << "Can't load word count because page #" << req_page << " is not opened. Aborting write to buffer";
		logger << EndDebugWarning;
		AbortWriteToBuffer(chip_num);
		return;
	}

	if(unlikely(IsVerbose()))
	{
		logger << DebugInfo;
		logger << "Chip #" << chip_num << ": Loading word count for page #" << page[chip_num] << " on sector #" << sector_num;
		logger << EndDebugInfo;
	}

	uint32_t n = (size > CHIP_IO_WIDTH) ? CHIP_IO_WIDTH : size;

	switch(n)
	{
		case 1:
			word_count[chip_num] = *data;
			break;
		case 2:
			{
				uint16_t wc;
				memcpy(&wc, data, n);
				word_count[chip_num] = unisim::util::endian::Target2Host(endian, wc) + 1;
			}
			break;
		case 4:
			{
				uint32_t wc;
				memcpy(&wc, data, n);
				word_count[chip_num] = unisim::util::endian::Target2Host(endian, wc) + 1;
			}
			break;
		case 8:
			{
				uint64_t wc;
				memcpy(&wc, data, n);
				word_count[chip_num] = unisim::util::endian::Target2Host(endian, wc) + 1;
			}
			break;
		default:
			logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
	}
	
	if(IsVerbose())
	{
		logger << DebugInfo << "Chip #" << chip_num << ": " << word_count[chip_num] << " loaded into word count" << EndDebugInfo;
	}
	
	if(word_count[chip_num] > CONFIG::PAGE_SIZE)
	{
		logger << DebugWarning;
		logger << "Chip #" << chip_num << ": Word count (" << word_count[chip_num] << ") is greater than page size (" << CONFIG::PAGE_SIZE << "). Aborting write to buffer";
		logger << EndDebugWarning;
		AbortWriteToBuffer(chip_num);
		return;
	}
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::WriteToBuffer(unsigned int chip_num, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size)
{
	typename CONFIG::ADDRESS chip_addr = addr >> addr_shift;
	typename CONFIG::ADDRESS sector_addr = chip_addr << config_addr_shift;
	int sector_num = GetSector(sector_addr);
	
	if(sector_num < 0)
	{
		logger << DebugWarning;
		logger << "Chip #" << chip_num << ": Sector #" << sector_num << " does not exist. Aborting write to buffer";
		logger << EndDebugWarning;
		AbortWriteToBuffer(chip_num);
		return;
	}
	
	typename CONFIG::ADDRESS req_page = GetPage(sector_addr);
	if(page[chip_num] != req_page)
	{
		logger << DebugWarning;
		logger << "Chip #" << chip_num << ": Can't write into write buffer because page #" << req_page << " is not opened. Aborting write to buffer";
		logger << EndDebugWarning;
		return;
	}

	if(unlikely(IsVerbose()))
	{
		logger << DebugInfo;
		logger << "Chip #" << chip_num << ": Writing into write buffer for page #" << page[chip_num] << " on sector #" << sector_num;
		logger << EndDebugInfo;
	}

	if(unlikely(IsVerbose()))
	{
		logger << DebugInfo;
		logger << "Chip #" << chip_num << ", Sector #" << sector_num << ", Addr 0x" << std::hex << chip_addr << std::dec << ": Writing to write buffer ";
		uint32_t n = size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size;
		std::stringstream sstr;
		PrintData(sstr, data, n);
		logger << sstr.str() << EndDebugInfo;
	}

	if(word_index[chip_num] >= word_count[chip_num])
	{
		logger << DebugWarning;
		logger << "Chip #" << chip_num << ": write buffer overflow (expecting no more than " << word_count[chip_num] << " words). Aborting write to buffer";
		logger << EndDebugWarning;
		return;
	}

	write_buffer[chip_num][word_index[chip_num]].addr = addr;
	memset(write_buffer[chip_num][word_index[chip_num]].data, 0, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
	memcpy(write_buffer[chip_num][word_index[chip_num]].data, data, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
	word_index[chip_num]++;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::ProgramBufferToFlash(unsigned int chip_num, typename CONFIG::ADDRESS addr)
{
	typename CONFIG::ADDRESS chip_addr = addr >> addr_shift;
	typename CONFIG::ADDRESS sector_addr = chip_addr << config_addr_shift;
	int sector_num = GetSector(sector_addr);
	
	if(sector_num < 0)
	{
		logger << DebugWarning;
		logger << "Chip #" << chip_num << ": Sector #" << sector_num << " does not exist. Aborting write to buffer";
		logger << EndDebugWarning;
		AbortWriteToBuffer(chip_num);
		return;
	}
	
	typename CONFIG::ADDRESS req_page = GetPage(sector_addr);
	if(page[chip_num] != req_page)
	{
		logger << DebugWarning;
		logger << "Chip #" << chip_num << ": Can't write into write buffer because page #" << req_page << " is not opened. Aborting write to buffer";
		logger << EndDebugWarning;
		AbortWriteToBuffer(chip_num);
		return;
	}

	if(IsVerbose())
	{
		logger << DebugInfo;
		logger << "Chip #" << chip_num << ": Programming write buffer to flash for page #" << page[chip_num] << " on sector #" << sector_num;
		logger << EndDebugInfo;
	}
	
	if(word_index[chip_num] != word_count[chip_num])
	{
		logger << DebugWarning;
		logger << "Chip #" << chip_num << ": not enough words loaded into write buffer (got " << word_index[chip_num] << ", expecting " << word_count[chip_num] << "). Aborting write to buffer";
		logger << EndDebugWarning;
		AbortWriteToBuffer(chip_num);
		return;
	}

	unsigned int i;
	
	for(i = 0; i < word_count[chip_num]; i++)
	{
		if(unlikely(IsVerbose()))
		{
			logger << DebugInfo;
			logger << "Chip #" << chip_num << ", Sector #" << sector_num << ", Addr 0x" << std::hex << write_buffer[chip_num][i].addr << std::dec << ": Programming ";
			std::stringstream sstr;
			PrintData(sstr, write_buffer[chip_num][i].data, CHIP_IO_WIDTH);
			logger << sstr.str() << EndDebugInfo;
		}
		Combine(storage + write_buffer[chip_num][i].addr, write_buffer[chip_num][i].data, CHIP_IO_WIDTH);
	}
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::ChipErase(unsigned int chip_num)
{
	if(unlikely(IsVerbose()))
	{
		logger << DebugInfo;
		logger << "Erasing Chip #" << chip_num;
		logger << EndDebugInfo;
	}

	// Erasing chip make chip bits become all 1's
	uint32_t addr;
	for(addr = chip_num * CHIP_IO_WIDTH; addr < BYTESIZE; addr += IO_WIDTH)
	{
		memset(&storage[addr], 0xff, CHIP_IO_WIDTH);
	}
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::SectorErase(unsigned int chip_num, typename CONFIG::ADDRESS addr)
{
	typename CONFIG::ADDRESS chip_addr = addr >> addr_shift;
	typename CONFIG::ADDRESS sector_addr = chip_addr << config_addr_shift;
	int sector_num = GetSector(sector_addr);

	if(sector_num < 0)
	{
		logger << DebugWarning;
		logger << "Sector #" << sector_num << " does not exist";
		logger << EndDebugWarning;
		return;
	}

	if(sector_protect[sector_num])
	{
		logger << DebugWarning;
		logger << "Attempting to erase sector #" << sector_num << " at 0x" << std::hex << addr << std::dec << " while sector is protected";
		logger << EndDebugWarning;
		return;
	}

	if(unlikely(IsVerbose()))
	{
		logger << DebugInfo;
		logger << "Erasing sector #" << sector_num << " at 0x" << std::hex << CONFIG::SECTOR_MAP[sector_num].addr << std::dec << " of chip #" << chip_num;
		logger << EndDebugInfo;
	}

	// Erasing sector make sector bits become all 1's
	memset(&storage[CONFIG::SECTOR_MAP[sector_num].addr], 0xff, CONFIG::SECTOR_MAP[sector_num].size);
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::SecuredSiliconSectorEntry(unsigned int chip_num)
{
	logger << DebugWarning << "Chip #" << chip_num << ": Secured-silicon-sector-entry command is unsupported for now" << EndDebugWarning;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::SecuredSiliconSectorExit(unsigned int chip_num)
{
	logger << DebugWarning << "Chip #" << chip_num << ": Secured-silicon-sector-exit command is unsupported for now" << EndDebugWarning;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::AbortWriteToBuffer(unsigned int chip_num)
{
	state[chip_num] = CONFIG::ST_WRITE_TO_BUFFER_ABORTED;
	write_status[chip_num] = (WRITE_STATUS)(write_status[chip_num] | WST_WRITE_TO_BUFFER_ABORT);
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
bool AM29<CONFIG, BYTESIZE, IO_WIDTH>::WriteMemory(typename CONFIG::ADDRESS addr, const void *buffer, uint32_t size)
{
	if(addr < org || (addr + size - 1) > (org + bytesize - 1) || (addr + size) < addr)
	{
		logger << DebugWarning;
		logger << "out of range address";
		logger << EndDebugWarning;
		return false;
	}
	// the third condition is for testing overwrapping (gdb did it !)

	typename CONFIG::ADDRESS offset = (addr - org) & ADDR_MASK; // keep only needed bits address
	memcpy(storage + offset, buffer, size);
	return true;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
bool AM29<CONFIG, BYTESIZE, IO_WIDTH>::ReadMemory(typename CONFIG::ADDRESS addr, void *buffer, uint32_t size)
{
	if(addr < org || (addr + size - 1) > (org + bytesize - 1) || (addr + size) < addr)
	{
		logger << DebugWarning;
		logger << "out of range address";
		logger << EndDebugWarning;
		return false;
	}
	// the third condition is for testing overwrapping (gdb did it !)

	typename CONFIG::ADDRESS offset = (addr - org) & ADDR_MASK;
	memcpy(buffer, storage + offset, size);
	return true;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
bool AM29<CONFIG, BYTESIZE, IO_WIDTH>::IsVerbose() const
{
	return verbose;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
const char *AM29<CONFIG, BYTESIZE, IO_WIDTH>::GetStateName(typename CONFIG::STATE state) const
{
	return CONFIG::GetStateName(state);
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
const char *AM29<CONFIG, BYTESIZE, IO_WIDTH>::GetCommandName(COMMAND command) const
{
	switch(command)
	{
		case CMD_READ: return "R";
		case CMD_WRITE: return "W";
	}
	return "?";
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
const char *AM29<CONFIG, BYTESIZE, IO_WIDTH>::GetActionName(ACTION action) const
{
	switch(action)
	{
		case ACT_NOP: return "-";
		case ACT_READ: return "READ";
		case ACT_READ_WRITE_STATUS: return "READ_WRITE_STATUS";
		case ACT_READ_AUTOSELECT: return "READ_AUTOSELECT";
		case ACT_CFI_QUERY: return "CFI_QUERY";
		case ACT_PROGRAM: return "PROGRAM";
		case ACT_OPEN_PAGE: return "OPEN_PAGE";
		case ACT_LOAD_WC: return "LOAD_WC";
		case ACT_WRITE_TO_BUFFER: return "WRITE_TO_BUFFER";
		case ACT_PROGRAM_BUFFER_TO_FLASH: return "PROGRAM_BUFFER_TO_FLASH";
		case ACT_CHIP_ERASE: return "CHIP_ERASE";
		case ACT_SECTOR_ERASE: return "SECTOR_ERASE";
		case ACT_SECURED_SILICON_SECTOR_ENTRY: return "SECURED_SILICON_SECTOR_ENTRY";
		case ACT_SECURED_SILICON_SECTOR_EXIT: return "SECURED_SILICON_SECTOR_EXIT";
	}
	return "?";
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::FSMToGraphviz()
{
	if(IsVerbose())
	{
		logger << DebugInfo << "Printing FSM as Graphviz ouput format into File \"" << fsm_to_graphviz_output_filename << "\"" << EndDebugInfo;
	}
	
	std::ofstream f(fsm_to_graphviz_output_filename.c_str(), std::ios::out);

	f << "digraph g {" << std::endl;
	unsigned int i;

	std::set<std::string> state_names;
	for(i = 0; i < CONFIG::NUM_TRANSITIONS; i++)
	{
		const TRANSITION<typename CONFIG::ADDRESS, CONFIG::MAX_IO_WIDTH, typename CONFIG::STATE> *transition = &CONFIG::FSM[i];
		
		std::stringstream initial_state_sstr;
		initial_state_sstr << GetStateName(transition->initial_state);
		
		std::string initial_state_name(initial_state_sstr.str());
		
		state_names.insert(initial_state_name);

		std::stringstream final_state_sstr;
		final_state_sstr << GetStateName(transition->final_state);
		
		std::string final_state_name(final_state_sstr.str());
		
		state_names.insert(final_state_name);
	}
	
	std::set<std::string>::const_iterator it;
	for(it = state_names.begin(); it != state_names.end(); it++)
	{
		f << "\"" << *it << "\" [fontname=Helvetica,fontsize=12.0];" << std::endl;
	}
	
	for(i = 0; i < CONFIG::NUM_TRANSITIONS; i++)
	{
		const TRANSITION<typename CONFIG::ADDRESS, CONFIG::MAX_IO_WIDTH, typename CONFIG::STATE> *transition = &CONFIG::FSM[i];

		// S1-[command,addr,data/action]->S2
		
		f << "\"" << GetStateName(transition->initial_state) << "\" -> \"" << GetStateName(transition->final_state);
		f << "\" [fontname=Helvetica,fontsize=10.0,label=\" [" << i << "] ";
		f << GetCommandName(transition->command) << ",";
		
		if(transition->wildcard_addr)
		{
			f << "*";
		}
		else
		{
			f << "0x" << std::hex << (transition->addr >> config_addr_shift) << std::dec;
		}
		
		f << ",";
		if(transition->wildcard_data)
		{
			f << "*";
		}
		else
		{
			PrintData(f, transition->data, CHIP_IO_WIDTH);
		}
		f << "/" << GetActionName(transition->action);
		f << "\"];" << std::endl;
	}
	
	f << "}" << std::endl;
}

inline char Nibble2HexChar(uint8_t v)
{
	v = v & 0xf; // keep only 4-bits
	return v < 10 ? '0' + v : 'a' + v - 10;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH>::PrintData(std::ostream& os, const uint8_t *data, unsigned int size)
{
	int i;
	
	os << "0x";
	switch(endian)
	{
		case E_LITTLE_ENDIAN:
			for(i = size - 1; i >= 0; i--) os << Nibble2HexChar(data[i] >> 4) << Nibble2HexChar(data[i] & 0xf);
			break;
		case E_BIG_ENDIAN:
			for(i = 0; i <= (int) size; i++) os << Nibble2HexChar(data[i] >> 4) << Nibble2HexChar(data[i] & 0xf);
			break;
		default:
			logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
	}
}

} // end of namespace am29
} // end of namespace flash
} // end of namespace memory
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29_AM29_TCC__
