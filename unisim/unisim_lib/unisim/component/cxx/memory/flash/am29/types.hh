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
 
#ifndef __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29_TYPES_HH__
#define __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29_TYPES_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace memory {
namespace flash {
namespace am29 {

typedef enum { MODE_X8 = 1, MODE_X16 = 2, MODE_X8_X16 = 3 } MODE;

typedef enum
{
	CMD_READ = 0,
	CMD_WRITE = 1
} COMMAND;

typedef enum
{
	ACT_NOP,
	ACT_READ,
	ACT_READ_WRITE_STATUS,
	ACT_READ_AUTOSELECT,
	ACT_CFI_QUERY,
	ACT_PROGRAM,
	ACT_OPEN_PAGE,
	ACT_LOAD_WC,
	ACT_WRITE_TO_BUFFER,
	ACT_PROGRAM_BUFFER_TO_FLASH,
	ACT_CHIP_ERASE,
	ACT_SECTOR_ERASE,
	ACT_SECURED_SILICON_SECTOR_ENTRY,
	ACT_SECURED_SILICON_SECTOR_EXIT
} ACTION;

template <class ADDRESS>
class SECTOR_ADDRESS_RANGE
{
public:
	ADDRESS addr;
	uint32_t size;
};

template <typename ADDRESS, unsigned int IO_WIDTH, typename STATE>
class TRANSITION
{
public:
	STATE initial_state;
	COMMAND command;
	bool wildcard_addr;
	ADDRESS addr;
	bool wildcard_data;
	uint8_t data[IO_WIDTH];
	STATE final_state;
	ACTION action;
};

typedef enum
{
	WST_DATA_POLLING = 1 << 7,
	WST_TOGGLE_BIT1 = 1 << 6,
	WST_EXCEEDED_TIMING_LIMITS = 1 << 5,
	WST_SECTOR_ERASE_TIMEOUT_STATE_INDICATOR = 1 << 3,
	WST_TOGGLE_BIT2 = 1 << 2,
	WST_WRITE_TO_BUFFER_ABORT = 1 << 1
} WRITE_STATUS;

} // end of namespace am29
} // end of namespace flash
} // end of namespace memory
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29_TYPES_HH__
