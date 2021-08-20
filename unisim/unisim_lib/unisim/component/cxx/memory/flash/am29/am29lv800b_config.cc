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
 
#include "unisim/component/cxx/memory/flash/am29/am29lv800b_config.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace memory {
namespace flash {
namespace am29 {

//-----------------------------------------------------------------------------------------------------------------------------------
//                                           AM29LV800B Configuration
//-----------------------------------------------------------------------------------------------------------------------------------

const unsigned int AM29LV800BConfig::PAGE_SIZE;

const char *AM29LV800BTConfig::DEVICE_NAME = "AM29LV800BT";
const char *AM29LV800BBConfig::DEVICE_NAME = "AM29LV800BB";

const uint64_t AM29LV800BConfig::PROGRAMMING_TIME[AM29LV800BConfig::MAX_IO_WIDTH] = { 9000, 110000 }; // 9 us/11us

// Sector address tables (top boot device)
const SECTOR_ADDRESS_RANGE<AM29LV800BConfig::ADDRESS> AM29LV800BTConfig::SECTOR_MAP[AM29LV800BConfig::NUM_SECTORS] = {
	{ 0x00000, 65536 }, // SA0/64 KB
	{ 0x10000, 65536 }, // SA1/64 KB
	{ 0x20000, 65536 }, // SA2/64 KB
	{ 0x30000, 65536 }, // SA3/64 KB
	{ 0x40000, 65536 }, // SA4/64 KB
	{ 0x50000, 65536 }, // SA5/64 KB
	{ 0x60000, 65536 }, // SA6/64 KB
	{ 0x70000, 65536 }, // SA7/64 KB
	{ 0x80000, 65536 }, // SA8/64 KB
	{ 0x90000, 65536 }, // SA9/64 KB
	{ 0xa0000, 65536 }, // SA10/64 KB
	{ 0xb0000, 65536 }, // SA11/64 KB
	{ 0xc0000, 65536 }, // SA12/64 KB
	{ 0xd0000, 65536 }, // SA13/64 KB
	{ 0xe0000, 65536 }, // SA14/64 KB
	{ 0xf0000, 32768 }, // SA15/32 KB
	{ 0xf8000,  8192 }, // SA16/8 KB
	{ 0xfa000,  8192 }, // SA17/8 KB
	{ 0xfc000, 16384 }  // SA18/16 KB
};

// Sector address tables (bottom boot device)
const SECTOR_ADDRESS_RANGE<AM29LV800BConfig::ADDRESS> AM29LV800BBConfig::SECTOR_MAP[AM29LV800BConfig::NUM_SECTORS] = {
	{ 0x00000, 16384 }, // SA0/16 KB
	{ 0x04000,  8192 }, // SA1/8 KB
	{ 0x06000,  8192 }, // SA2/8 KB
	{ 0x08000, 32768 }, // SA3/32 KB
	{ 0x10000, 65536 }, // SA4/64 KB
	{ 0x20000, 65536 }, // SA5/64 KB
	{ 0x30000, 65536 }, // SA6/64 KB
	{ 0x40000, 65536 }, // SA7/64 KB
	{ 0x50000, 65536 }, // SA8/64 KB
	{ 0x60000, 65536 }, // SA9/64 KB
	{ 0x70000, 65536 }, // SA10/64 KB
	{ 0x80000, 65536 }, // SA11/64 KB
	{ 0x90000, 65536 }, // SA12/64 KB
	{ 0xa0000, 65536 }, // SA13/64 KB
	{ 0xb0000, 65536 }, // SA14/64 KB
	{ 0xc0000, 65536 }, // SA15/64 KB
	{ 0xd0000, 65536 }, // SA16/64 KB
	{ 0xe0000, 65536 }, // SA17/64 KB
	{ 0xf0000, 65536 }  // SA18/64 KB
};

const uint8_t AM29LV800BConfig::MANUFACTURER_ID[MAX_IO_WIDTH] = { 0x01, 0x00 };
const AM29LV800BConfig::ADDRESS AM29LV800BConfig::DEVICE_ID_ADDR[1] = { 0x02 };
const uint8_t AM29LV800BTConfig::DEVICE_ID[DEVICE_ID_LENGTH][MAX_IO_WIDTH] = { { 0xda, 0x22 } };
const uint8_t AM29LV800BBConfig::DEVICE_ID[DEVICE_ID_LENGTH][MAX_IO_WIDTH] = { { 0x5b, 0x22 } };
const uint8_t AM29LV800BConfig::PROTECTED[MAX_IO_WIDTH] = { 0x01, 0x00 };
const uint8_t AM29LV800BConfig::UNPROTECTED[MAX_IO_WIDTH] = { 0x00, 0x00 };

// CFI queries
const AM29LV800BConfig::CFI_QUERY AM29LV800BConfig::CFI_QUERIES[NUM_CFI_QUERIES] = {
	{ 0, { 0, 0 } }
};

// S1-[command,addr,data/action]->S2
const TRANSITION<AM29LV800BConfig::ADDRESS, AM29LV800BConfig::MAX_IO_WIDTH, AM29LV800BConfig::STATE> AM29LV800BConfig::FSM[AM29LV800BConfig::NUM_TRANSITIONS] = {
	// unlock cycles transitions
	{ ST_I0, CMD_WRITE, false, 0xaaa, false, { 0xaa, 0x00 }, ST_I1, ACT_NOP }, // (I0) -[W,AAA,AA/-]-> (I1)
	{ ST_I0, CMD_WRITE, true, 0, true, { 0,0 }, ST_I0, ACT_NOP }, // (I0) -[W,*,*/-]->(I0)
	{ ST_I1, CMD_WRITE, false, 0x555, false, { 0x55, 0x00 }, ST_I2, ACT_NOP }, // (I1) -[W,555,55/-]->(I2)
	{ ST_I1, CMD_WRITE, true, 0, true, { 0, 0 }, ST_I0, ACT_NOP }, // (I1) -[W,*,*/-]->(I0)
	// commands after the two unclock cycles recognition transitions
	{ ST_I2, CMD_WRITE, false, 0xaaa, false, { 0x90, 0x00 }, ST_AUTOSELECT, ACT_NOP }, // (I2) -[W,AAA,90/-]->(AUTOSELECT)
	{ ST_I2, CMD_WRITE, false, 0xaaa, false, { 0xa0, 0x00 }, ST_PROGRAM, ACT_NOP }, // (I2) -[W,AAA,A0/-]->(PROGRAM)
	{ ST_I2, CMD_WRITE, false, 0xaaa, false, { 0x20, 0x00 }, ST_UNLOCKED, ACT_NOP }, // (I2) -[W,AAA,20/-]->(UNLOCKED)
	{ ST_I2, CMD_WRITE, false, 0xaaa, false, { 0x80, 0x00 }, ST_ERASE0, ACT_NOP }, // (I2) -[W,AAA,80/-]->(ERASE0)
	{ ST_I2, CMD_WRITE, true, 0, true, { 0, 0 }, ST_I0, ACT_NOP }, // (I2) -[W,*,*/-]->(I0)
	// autoselect transitions
	{ ST_AUTOSELECT, CMD_READ, true, 0, true, { 0, 0 }, ST_AUTOSELECT, ACT_READ_AUTOSELECT }, // (AUTOSELECT) -[R,*,*/READ_AUTOSELECT]->(AUTOSELECT)
	{ ST_AUTOSELECT, CMD_WRITE, true, 0, false, { 0xf0, 0 }, ST_I0, ACT_NOP }, // (AUTOSELECT) -[W,*,F0/-]->(I0)
	// single program transitions
	{ ST_PROGRAM, CMD_WRITE, true, 0, true, { 0, 0 }, ST_I0, ACT_PROGRAM }, // (PROGRAM) -[W,*,*/PROGRAM]->(I0)
	// unlock bypass transitions
	{ ST_UNLOCKED, CMD_WRITE, true, 0, false, { 0xa0, 0x00 }, ST_UNLOCKED_PROGRAM, ACT_NOP }, // (UNLOCKED) -[W,*,A0/-]->(UNLOCKED_PROGRAM)
	{ ST_UNLOCKED, CMD_WRITE, true, 0, false, { 0x80, 0x00 }, ST_UNLOCKED_ERASE, ACT_NOP }, // (UNLOCKED) -[W,*,80/-]->(UNLOCKED_ERASE)
	{ ST_UNLOCKED, CMD_WRITE, true, 0, false, { 0x90, 0x00 }, ST_UNLOCKED_RESET, ACT_NOP }, // (UNLOCKED) -[W,*,90/-]->(UNLOCKED_RESET)
	{ ST_UNLOCKED, CMD_WRITE, true, 0, true, { 0, 0 }, ST_UNLOCKED, ACT_NOP }, // (UNLOCKED) -[W,*,*/-]->(UNLOCKED)
	{ ST_UNLOCKED_PROGRAM, CMD_WRITE, true, 0, true, { 0, 0 }, ST_UNLOCKED, ACT_PROGRAM }, // (UNLOCKED_PROGRAM) -[W,*,*/PROGRAM]->(UNLOCKED)
	{ ST_UNLOCKED_RESET, CMD_WRITE, true, 0, false, { 0, 0 }, ST_I0, ACT_NOP }, // (UNLOCKED_RESET) -[W,*,00/-]->(I0)
	{ ST_UNLOCKED_RESET, CMD_WRITE, true, 0, true, { 0, 0 }, ST_UNLOCKED_RESET, ACT_NOP }, // (UNLOCKED_RESET) -[W,*,*/-]->(UNLOCKED_RESET)
	{ ST_UNLOCKED_RESET, CMD_WRITE, true, 0, false, { 0x90, 0 }, ST_I0, ACT_NOP }, // (UNLOCKED_RESET) -[W,*,90/-]->(I0)
	{ ST_UNLOCKED_ERASE, CMD_WRITE, true, 0, false, { 0x30, 0 }, ST_UNLOCKED, ACT_SECTOR_ERASE }, // (UNLOCKED_ERASE) -[W,*,30/SECTOR_ERASE]->(UNLOCKED)
	{ ST_UNLOCKED_ERASE, CMD_WRITE, true, 0, false, { 0x10, 0 }, ST_UNLOCKED, ACT_CHIP_ERASE }, // (UNLOCKED_ERASE) -[W,*,10/CHIP_ERASE]->(UNLOCKED)
	// single erase transitions
	{ ST_ERASE0, CMD_WRITE, false, 0xaaa, false, { 0xaa, 0x00 }, ST_ERASE1, ACT_NOP }, // (I2) -[W,AAA,AA/-]->(ERASE1)
	{ ST_ERASE1, CMD_WRITE, false, 0x555, false, { 0x55, 0x00 }, ST_ERASE2, ACT_NOP }, // (ERASE1) -[W,555,55/-]->(ERASE2)
	{ ST_ERASE1, CMD_WRITE, true, 0, true, { 0, 0 }, ST_I0, ACT_NOP }, // (ERASE1) -[W,*,*/-]->(I0)
	{ ST_ERASE2, CMD_WRITE, false, 0xaaa, false, { 0x10, 0x00 }, ST_I0, ACT_CHIP_ERASE }, // (ERASE2) -[W,AAA,10/CHIP_ERASE]->(I0)
	{ ST_ERASE2, CMD_WRITE, true, 0, false, { 0x30, 0x00 }, ST_I0, ACT_SECTOR_ERASE }, // (ERASE2) -[W,*,30/SECTOR_ERASE]->(I0)
	{ ST_ERASE2, CMD_WRITE, true, 0, true, { 0, 0 }, ST_I0, ACT_NOP }, // (ERASE2) -[W,*,*/-]->(I,0)
	// Suspend/Resume transitions
	{ ST_ANY, CMD_WRITE, true, 0, false, { 0xb0, 0x00 }, ST_ANY, ACT_NOP }, // (*) -[W,*,B0/-]-> (*)
	{ ST_ANY, CMD_WRITE, true, 0, false, { 0x30, 0x00 }, ST_ANY, ACT_NOP }, // (*) -[W,*,30/-]-> (*)
	// Fallback transitions
	{ ST_ANY, CMD_WRITE, true, 0, true, { 0,0 }, ST_ANY, ACT_NOP }, // (*) -[W,*,*/-]->(*)
	{ ST_ANY, CMD_READ, true, 0, true, { 0,0 }, ST_ANY, ACT_READ }  // (*) -[R,*,*/READ]->(*)
};

} // end of namespace am29
} // end of namespace flash
} // end of namespace memory
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
