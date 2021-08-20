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
 
#include "unisim/component/cxx/memory/flash/am29/am29lv160d_config.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace memory {
namespace flash {
namespace am29 {

//-----------------------------------------------------------------------------------------------------------------------------------
//                                           AM29LV160D Configuration
//-----------------------------------------------------------------------------------------------------------------------------------

const char *AM29LV160DTConfig::DEVICE_NAME = "AM29LV160DT";
const char *AM29LV160DBConfig::DEVICE_NAME = "AM29LV160DB";

const uint64_t AM29LV160DConfig::PROGRAMMING_TIME[AM29LV160DConfig::MAX_IO_WIDTH] = { 5000, 70000 }; // 5 us/7us

// Sector address tables (top boot device)
const SECTOR_ADDRESS_RANGE<AM29LV160DConfig::ADDRESS> AM29LV160DTConfig::SECTOR_MAP[AM29LV160DConfig::NUM_SECTORS] = {
	{ 0x000000, 65536 }, // SA0/64 KB
	{ 0x010000, 65536 }, // SA1/64 KB
	{ 0x020000, 65536 }, // SA2/64 KB
	{ 0x030000, 65536 }, // SA3/64 KB
	{ 0x040000, 65536 }, // SA4/64 KB
	{ 0x050000, 65536 }, // SA5/64 KB
	{ 0x060000, 65536 }, // SA6/64 KB
	{ 0x070000, 65536 }, // SA7/64 KB
	{ 0x080000, 65536 }, // SA8/64 KB
	{ 0x090000, 65536 }, // SA9/64 KB
	{ 0x0a0000, 65536 }, // SA10/64 KB
	{ 0x0b0000, 65536 }, // SA11/64 KB
	{ 0x0c0000, 65536 }, // SA12/64 KB
	{ 0x0d0000, 65536 }, // SA13/64 KB
	{ 0x0e0000, 65536 }, // SA14/64 KB
	{ 0x0f0000, 65536 }, // SA15/64 KB
	{ 0x100000, 65536 }, // SA16/64 KB
	{ 0x110000, 65536 }, // SA17/64 KB
	{ 0x120000, 65536 }, // SA18/64 KB
	{ 0x130000, 65536 }, // SA19/64 KB
	{ 0x140000, 65536 }, // SA20/64 KB
	{ 0x150000, 65536 }, // SA21/64 KB
	{ 0x160000, 65536 }, // SA22/64 KB
	{ 0x170000, 65536 }, // SA23/64 KB
	{ 0x180000, 65536 }, // SA24/64 KB
	{ 0x190000, 65536 }, // SA25/64 KB
	{ 0x1a0000, 65536 }, // SA26/64 KB
	{ 0x1b0000, 65536 }, // SA27/64 KB
	{ 0x1c0000, 65536 }, // SA28/64 KB
	{ 0x1d0000, 65536 }, // SA29/64 KB
	{ 0x1e0000, 65536 }, // SA30/64 KB
	{ 0x1f0000, 32768 }, // SA31/32 KB
	{ 0x1f8000,  8192 }, // SA32/8 KB
	{ 0x1fa000,  8192 }, // SA33/8 KB
	{ 0x1fc000, 16384 }  // SA34/16 KB
};

// Sector address tables (bottom boot device)
const SECTOR_ADDRESS_RANGE<AM29LV160DConfig::ADDRESS> AM29LV160DBConfig::SECTOR_MAP[AM29LV160DConfig::NUM_SECTORS] = {
	{ 0x000000, 16384 }, // SA0/16 KB
	{ 0x004000,  8192 }, // SA1/8 KB
	{ 0x006000,  8192 }, // SA2/8 KB
	{ 0x008000, 32768 }, // SA3/32 KB
	{ 0x010000, 65536 }, // SA4/64 KB
	{ 0x020000, 65536 }, // SA5/64 KB
	{ 0x030000, 65536 }, // SA6/64 KB
	{ 0x040000, 65536 }, // SA7/64 KB
	{ 0x050000, 65536 }, // SA8/64 KB
	{ 0x060000, 65536 }, // SA9/64 KB
	{ 0x070000, 65536 }, // SA10/64 KB
	{ 0x080000, 65536 }, // SA11/64 KB
	{ 0x090000, 65536 }, // SA12/64 KB
	{ 0x0a0000, 65536 }, // SA13/64 KB
	{ 0x0b0000, 65536 }, // SA14/64 KB
	{ 0x0c0000, 65536 }, // SA15/64 KB
	{ 0x0d0000, 65536 }, // SA16/64 KB
	{ 0x0e0000, 65536 }, // SA17/64 KB
	{ 0x0f0000, 65536 }, // SA18/64 KB
	{ 0x100000, 65536 }, // SA19/64 KB
	{ 0x110000, 65536 }, // SA20/64 KB
	{ 0x120000, 65536 }, // SA21/64 KB
	{ 0x130000, 65536 }, // SA22/64 KB
	{ 0x140000, 65536 }, // SA23/64 KB
	{ 0x150000, 65536 }, // SA24/64 KB
	{ 0x160000, 65536 }, // SA25/64 KB
	{ 0x170000, 65536 }, // SA26/64 KB
	{ 0x180000, 65536 }, // SA27/64 KB
	{ 0x190000, 65536 }, // SA28/64 KB
	{ 0x1a0000, 65536 }, // SA29/64 KB
	{ 0x1b0000, 65536 }, // SA30/64 KB
	{ 0x1c0000, 65536 }, // SA31/64 KB
	{ 0x1d0000, 65536 }, // SA32/64 KB
	{ 0x1e0000, 65536 }, // SA33/64 KB
	{ 0x1f0000, 65536 }  // SA34/64 KB
};

const uint8_t AM29LV160DConfig::MANUFACTURER_ID[MAX_IO_WIDTH] = { 0x01, 0x00 };
const AM29LV160DConfig::ADDRESS AM29LV160DConfig::DEVICE_ID_ADDR[1] = { 0x02 };
const uint8_t AM29LV160DTConfig::DEVICE_ID[DEVICE_ID_LENGTH][MAX_IO_WIDTH] = { { 0xc4, 0x22 } };
const uint8_t AM29LV160DBConfig::DEVICE_ID[DEVICE_ID_LENGTH][MAX_IO_WIDTH] = { { 0x49, 0x22 } };
const uint8_t AM29LV160DConfig::PROTECTED[MAX_IO_WIDTH] = { 0x01, 0x00 };
const uint8_t AM29LV160DConfig::UNPROTECTED[MAX_IO_WIDTH] = { 0x00, 0x00 };

// S1-[command,addr,data/action]->S2
const TRANSITION<AM29LV160DConfig::ADDRESS, AM29LV160DConfig::MAX_IO_WIDTH, AM29LV160DConfig::STATE> AM29LV160DConfig::FSM[AM29LV160DConfig::NUM_TRANSITIONS] = {
	{ ST_I0, CMD_WRITE, false, 0xaaa, false, { 0xaa, 0x00 }, ST_I1, ACT_NOP }, // (I0) -[W,AAA,AA/-]-> (I1)
	{ ST_I0, CMD_READ, true, 0, true, { 0,0 }, ST_I0, ACT_READ }, // (I0) -[R,*,*/READ]->(I0)
	{ ST_I0, CMD_WRITE, true, 0, true, { 0,0 }, ST_I0, ACT_NOP }, // (I0) -[W,*,*/-]->(I0)
	{ ST_I1, CMD_WRITE, false, 0x555, false, { 0x55, 0x00 }, ST_I2, ACT_NOP }, // (I1) -[W,555,55/-]->(I2)
	{ ST_I1, CMD_WRITE, true, 0, true, { 0, 0 }, ST_I0, ACT_NOP }, // (I1) -[W,*,*/-]->(I0)
	{ ST_I1, CMD_READ, true, 0, true, { 0, 0 }, ST_I1, ACT_READ }, // (I1) -[R,*,*/READ]->(I1)
	{ ST_I2, CMD_WRITE, false, 0xaaa, false, { 0x90, 0x00 }, ST_AUTOSELECT, ACT_NOP }, // (I2) -[W,AAA,90/-]->(AUTOSELECT)
	{ ST_I2, CMD_WRITE, false, 0xaaa, false, { 0xa0, 0x00 }, ST_PROGRAM, ACT_NOP }, // (I2) -[W,AAA,A0/-]->(PROGRAM)
	{ ST_I2, CMD_WRITE, false, 0xaaa, false, { 0x20, 0x00 }, ST_UNLOCKED, ACT_NOP }, // (I2) -[W,AAA,20/-]->(UNLOCKED)
	{ ST_I2, CMD_WRITE, false, 0xaaa, false, { 0x80, 0x00 }, ST_ERASE0, ACT_NOP }, // (I2) -[W,AAA,80/-]->(ERASE0)
	{ ST_I2, CMD_WRITE, true, 0, true, { 0, 0 }, ST_I0, ACT_NOP }, // (I2) -[W,*,*/-]->(I0)
	{ ST_I2, CMD_READ, true, 0, true, { 0, 0 }, ST_I2, ACT_READ }, // (I2) -[R,*,*/-]->(I2)
	{ ST_AUTOSELECT, CMD_READ, true, 0, true, { 0, 0 }, ST_AUTOSELECT, ACT_READ_AUTOSELECT }, // (AUTOSELECT) -[R,*,*/READ_AUTOSELECT]->(AUTOSELECT)
	{ ST_AUTOSELECT, CMD_WRITE, true, 0, false, { 0xf0, 0 }, ST_I0, ACT_NOP }, // (AUTOSELECT) -[W,*,F0/-]->(I0)
	{ ST_PROGRAM, CMD_WRITE, true, 0, true, { 0, 0 }, ST_I0, ACT_PROGRAM }, // (PROGRAM) -[W,*,*/PROGRAM]->(I0)
	{ ST_PROGRAM, CMD_READ, true, 0, true, { 0, 0 }, ST_PROGRAM, ACT_READ }, // (PROGRAM) -[R,*,*/READ]->(PROGRAM)
	{ ST_UNLOCKED, CMD_WRITE, true, 0, false, { 0xa0, 0x00 }, ST_UNLOCKED_PROGRAM, ACT_NOP }, // (UNLOCKED) -[W,*,A0/-]->(UNLOCKED_PROGRAM)
	{ ST_UNLOCKED, CMD_WRITE, true, 0, false, { 0x90, 0x00 }, ST_UNLOCKED_RESET, ACT_NOP }, // (UNLOCKED) -[W,*,90/-]->(UNLOCKED_RESET)
	{ ST_UNLOCKED, CMD_WRITE, true, 0, true, { 0, 0 }, ST_UNLOCKED, ACT_NOP }, // (UNLOCKED) -[W,*,*/-]->(UNLOCKED)
	{ ST_UNLOCKED, CMD_READ, true, 0, true, { 0, 0 }, ST_UNLOCKED, ACT_READ }, // (UNLOCKED) -[R,*,*/READ]->(UNLOCKED)
	{ ST_UNLOCKED_PROGRAM, CMD_WRITE, true, 0, true, { 0, 0 }, ST_UNLOCKED, ACT_PROGRAM }, // (UNLOCKED_PROGRAM) -[W,*,*/PROGRAM]->(UNLOCKED)
	{ ST_UNLOCKED_PROGRAM, CMD_READ, true, 0, true, { 0, 0 }, ST_UNLOCKED_PROGRAM, ACT_READ }, // (UNLOCKED_PROGRAM) -[R,*,*/READ]->(UNLOCKED_PROGRAM)
	{ ST_UNLOCKED_RESET, CMD_WRITE, true, 0, false, { 0, 0 }, ST_I0, ACT_NOP }, // (UNLOCKED_RESET) -[W,*,00/-]->(I0)
	{ ST_UNLOCKED_RESET, CMD_WRITE, true, 0, true, { 0, 0 }, ST_UNLOCKED_RESET, ACT_NOP }, // (UNLOCKED_RESET) -[W,*,*/-]->(UNLOCKED_RESET)
	{ ST_UNLOCKED_RESET, CMD_WRITE, true, 0, false, { 0x90, 0 }, ST_I0, ACT_NOP }, // (UNLOCKED_RESET) -[W,*,90/-]->(I0)
	{ ST_UNLOCKED_RESET, CMD_READ, true, 0, true, { 0, 0 }, ST_UNLOCKED_RESET, ACT_READ }, // (UNLOCKED_RESET) -[R,*,*/READ]->(UNLOCKED_RESET)
	{ ST_ERASE0, CMD_WRITE, false, 0xaaa, false, { 0xaa, 0x00 }, ST_ERASE1, ACT_NOP }, // (I2) -[W,AAA,AA/-]->(ERASE1)
	{ ST_ERASE1, CMD_WRITE, false, 0x555, false, { 0x55, 0x00 }, ST_ERASE2, ACT_NOP }, // (ERASE1) -[W,555,55/-]->(ERASE2)
	{ ST_ERASE1, CMD_WRITE, true, 0, true, { 0, 0 }, ST_I0, ACT_NOP }, // (ERASE1) -[W,*,*/-]->(I0)
	{ ST_ERASE1, CMD_READ, true, 0, true, { 0, 0 }, ST_ERASE1, ACT_READ }, // (ERASE1) -[R,*,*/READ]->(ERASE1)
	{ ST_ERASE2, CMD_WRITE, false, 0xaaa, false, { 0x10, 0x00 }, ST_I0, ACT_CHIP_ERASE }, // (ERASE2) -[W,AAA,10/CHIP_ERASE]->(I0)
	{ ST_ERASE2, CMD_WRITE, true, 0, false, { 0x30, 0x00 }, ST_I0, ACT_SECTOR_ERASE }, // (ERASE2) -[W,*,30/SECTOR_ERASE]->(I0)
	{ ST_ERASE2, CMD_WRITE, true, 0, true, { 0, 0 }, ST_I0, ACT_NOP }, // (ERASE2) -[W,*,*/-]->(I,0)
	{ ST_ERASE2, CMD_READ, true, 0, true, { 0, 0 }, ST_ERASE2, ACT_READ }, // (ERASE2) -[R,*,*/READ]->(ERASE2)
	{ ST_ANY, CMD_WRITE, true, 0, false, { 0xb0, 0x00 }, ST_ANY, ACT_NOP }, // (*) -[W,*,B0/-]-> (*)
	{ ST_ANY, CMD_WRITE, true, 0, false, { 0x30, 0x00 }, ST_ANY, ACT_NOP } // (*) -[W,*,30/-]-> (*)
};

} // end of namespace am29
} // end of namespace flash
} // end of namespace memory
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
