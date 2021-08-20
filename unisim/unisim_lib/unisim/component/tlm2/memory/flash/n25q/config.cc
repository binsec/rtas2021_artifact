/*
 *  Copyright (c) 2016,
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

#include <unisim/component/tlm2/memory/flash/n25q/config.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace flash {
namespace n25q {

//////////////////////////// Device features /////////////////////////////////////

const uint32_t N25Q032A_CONFIG::SIZE;
const uint32_t N25Q032A_CONFIG::PAGE_SIZE;
const uint32_t N25Q032A_CONFIG::SUBSECTOR_SIZE;
const uint32_t N25Q032A_CONFIG::SECTOR_SIZE;
const uint32_t N25Q032A_CONFIG::OTP_ARRAY_SIZE;
const uint32_t N25Q032A_CONFIG::ID_SIZE;
const uint32_t N25Q032A_CONFIG::SFDP_SIZE;
const uint32_t N25Q032A_CONFIG::MAX_PROGRAM_SUSPEND_OPERATION_NESTING_LEVEL;

////////////////////////////// Max frequency /////////////////////////////////////

const double N25Q032A_CONFIG::MAX_FREQUENCY;

//////////////////// Max supported frequencies (Mhz) /////////////////////////////

const double N25Q032A_CONFIG::FAST_READ_MAX_FREQUENCIES[10][5] = {
	/* --------------+---------------+--------------------------+-------------------------------+--------------------------------+---------------------------------- */
	/* dummy cycles  |   FAST READ   |  DUAL OUTPUT FAST READ   |       DUAL IO FAST READ       |     QUAD OUTPUT FAST READ      |         QUAD IO FAST READ         */
	/* --------------+---------------+--------------------------+-------------------------------+--------------------------------+---------------------------------- */
	/*     1      */ {  90.0 /* MHz*/,      80.0 /* MHz */      ,         50.0 /* MHz */        ,         43.0 /* MHz */         ,           30.0 /* MHz */          },
	/*     2      */ { 100.0 /* MHz*/,      90.0 /* MHz */      ,         70.0 /* MHz */        ,         60.0 /* MHz */         ,           40.0 /* MHz */          },
	/*     3      */ { 108.0 /* MHz*/,     100.0 /* MHz */      ,         80.0 /* MHz */        ,         75.0 /* MHz */         ,           50.0 /* MHz */          },
	/*     4      */ { 108.0 /* MHz*/,     105.0 /* MHz */      ,         90.0 /* MHz */        ,         90.0 /* MHz */         ,           60.0 /* MHz */          },
	/*     5      */ { 108.0 /* MHz*/,     108.0 /* MHz */      ,        100.0 /* MHz */        ,        100.0 /* MHz */         ,           70.0 /* MHz */          },
	/*     6      */ { 108.0 /* MHz*/,     108.0 /* MHz */      ,        105.0 /* MHz */        ,        105.0 /* MHz */         ,           80.0 /* MHz */          },
	/*     7      */ { 108.0 /* MHz*/,     108.0 /* MHz */      ,        108.0 /* MHz */        ,        108.0 /* MHz */         ,           86.0 /* MHz */          },
	/*     8      */ { 108.0 /* MHz*/,     108.0 /* MHz */      ,        108.0 /* MHz */        ,        108.0 /* MHz */         ,           95.0 /* MHz */          },
	/*     9      */ { 108.0 /* MHz*/,     108.0 /* MHz */      ,        108.0 /* MHz */        ,        108.0 /* MHz */         ,          105.0 /* MHz */          },
	/*    10      */ { 108.0 /* MHz*/,     108.0 /* MHz */      ,        108.0 /* MHz */        ,        108.0 /* MHz */         ,          108.0 /* MHz */          }
	/* --------------+---------------+--------------------------+-------------------------------+--------------------------------+---------------------------------- */
};            

////////////////////////// Device identification //////////////////////////////////

const uint8_t N25Q032A1_CONFIG::ID[N25Q032A_CONFIG::ID_SIZE] = {
	/* @0x00 Manufacturer ID:                      */ 0x20, // JEDEC
	/* @0x01 Device ID (Memory Type):              */ 0xba, // Assigned by Manufacturer
	/* @0x02 Device ID (Memory Capacity):          */ 0x16, // 32 Mb (Assigned by Manufacturer)
	/* @0x03 Unique ID (length of data to follow): */ 0x10, // 16 bytes
	/* @0x04-0x05 Unique ID (ID and information such as uniform architecture, and HOLD or RESET functionality) */
	/*            Bit 7: */ (0 << 7) + // reserved
	/*            Bit 6: */ (0 << 6) + // reserved
	/*            Bit 5: */ (0 << 5) + // Standard BP scheme
	/*            Bit 4: */ (0 << 4) + // Micron XIP
	/*            Bit 3: */ (0 << 3) + // HOLD#
	/*            Bit 2: */ (0 << 2) + // byte addressing
	/*          Bit 1-0: */ (0 << 0),  // uniform architecture
	                            0x00, 
	/* @0x06 Unique ID (customized factory data): */ 'U', 'N', 'I', 'S', 'I', 'M', '-', 'V', 'P', '-', 'N', '2', '5', 'Q' // "UNISIM-VP-N25Q"
};

const uint8_t N25Q032A2_CONFIG::ID[N25Q032A_CONFIG::ID_SIZE] = {
	/* @0x00 Manufacturer ID:                      */ 0x20, // JEDEC
	/* @0x01 Device ID (Memory Type):              */ 0xba, // Assigned by Manufacturer
	/* @0x02 Device ID (Memory Capacity):          */ 0x16, // 32 Mb (Assigned by Manufacturer)
	/* @0x03 Unique ID (length of data to follow): */ 0x10, // 16 bytes
	/* @0x04-0x05 Unique ID (ID and information such as uniform architecture, and HOLD or RESET functionality) */
	/*            Bit 7: */ (0 << 7) + // reserved
	/*            Bit 6: */ (0 << 6) + // reserved
	/*            Bit 5: */ (0 << 5) + // Standard BP scheme
	/*            Bit 4: */ (1 << 4) + // Basic XIP
	/*            Bit 3: */ (0 << 3) + // HOLD#
	/*            Bit 2: */ (0 << 2) + // byte addressing
	/*          Bit 1-0: */ (0 << 0),  // uniform architecture
	                            0x00, 
	/* @0x06 Unique ID (customized factory data): */ 'U', 'N', 'I', 'S', 'I', 'M', '-', 'V', 'P', '-', 'N', '2', '5', 'Q' // "UNISIM-VP-N25Q"
};

const uint8_t N25Q032A3_CONFIG::ID[N25Q032A_CONFIG::ID_SIZE] = {
	/* @0x00 Manufacturer ID:                      */ 0x20, // JEDEC
	/* @0x01 Device ID (Memory Type):              */ 0xba, // Assigned by Manufacturer
	/* @0x02 Device ID (Memory Capacity):          */ 0x16, // 32 Mb (Assigned by Manufacturer)
	/* @0x03 Unique ID (length of data to follow): */ 0x10, // 16 bytes
	/* @0x04-0x05 Unique ID (ID and information such as uniform architecture, and HOLD or RESET functionality) */
	/*            Bit 7: */ (0 << 7) + // reserved
	/*            Bit 6: */ (0 << 6) + // reserved
	/*            Bit 5: */ (0 << 5) + // Standard BP scheme
	/*            Bit 4: */ (0 << 4) + // Micron XIP
	/*            Bit 3: */ (1 << 3) + // RST#
	/*            Bit 2: */ (0 << 2) + // byte addressing
	/*          Bit 1-0: */ (0 << 0),  // uniform architecture
	                            0x00, 
	/* @0x06 Unique ID (customized factory data): */ 'U', 'N', 'I', 'S', 'I', 'M', '-', 'V', 'P', '-', 'N', '2', '5', 'Q' // "UNISIM-VP-N25Q"
};

const uint8_t N25Q032A4_CONFIG::ID[N25Q032A_CONFIG::ID_SIZE] = {
	/* @0x00 Manufacturer ID:                      */ 0x20, // JEDEC
	/* @0x01 Device ID (Memory Type):              */ 0xba, // Assigned by Manufacturer
	/* @0x02 Device ID (Memory Capacity):          */ 0x16, // 32 Mb (Assigned by Manufacturer)
	/* @0x03 Unique ID (length of data to follow): */ 0x10, // 16 bytes
	/* @0x04-0x05 Unique ID (ID and information such as uniform architecture, and HOLD or RESET functionality) */
	/*            Bit 7: */ (0 << 7) + // reserved
	/*            Bit 6: */ (0 << 6) + // reserved
	/*            Bit 5: */ (0 << 5) + // Standard BP scheme
	/*            Bit 4: */ (1 << 4) + // Basic XIP
	/*            Bit 3: */ (1 << 3) + // RST#
	/*            Bit 2: */ (0 << 2) + // byte addressing
	/*          Bit 1-0: */ (0 << 0),  // uniform architecture
	0x00, 
	/* @0x06 Unique ID (customized factory data): */ 'U', 'N', 'I', 'S', 'I', 'M', '-', 'V', 'P', '-', 'N', '2', '5', 'Q' // "UNISIM-VP-N25Q"
};

const uint8_t N25Q032A_CONFIG::SFDP[N25Q032A_CONFIG::SFDP_SIZE] = {
	/* @0x00-0x03: SFDP signature */ 0x53, 0x46, 0x44, 0x50,
	/* @0x04: SFDP revision Minor */ 0x00,
	/* @0x05: SFDP revision Major */ 0x01,
	/* @0x06: Number of parameter headers */ 0x00,
	/* @0x07: Unused */ 0xff,
	/* @0x08: Parameter ID (0) */ 0x00,   
	/* @0x09: Parameter minor revision */ 0x00,   
	/* @0x0a: Parameter major revision */ 0x01,   
	/* @0x0b: Parameter length (in DW) */ 0x09,   
	/* @0x0c-0x0e: Parameter table pointer */ 0x30, 0x00, 0x00,   
	/* @0x0f: Unused */ 0xff,   
	/* @0x10-0x2f */ 0xff, 0xff, 0xff, 0xff,
	                 0xff, 0xff, 0xff, 0xff,
	                 0xff, 0xff, 0xff, 0xff,
	                 0xff, 0xff, 0xff, 0xff,
	/* @0x30 Bits 1-0: Minimum block/sector erase sizes                                          */  1 +
	/*          Bit 2: Write granularity                                                         */ (1 << 2) +
	/*          Bit 3: WRITE ENABLE command required for writing to volatile status registers    */ (0 << 3) +
	/*          Bit 4: WRITE ENABLE command code select for writing to volatile status registers */ (0 << 4) +
	/*        Bit 7-5: Unused                                                                    */ (7 << 5),
	/* @0x31: 4KB ERASE command code */ 0x20,
	/* @0x32 Bit 0: Supports 1-1-2 fast read               */ 1 +
	/*    Bits 2-1: Address bytes                          */ (0 << 1) +
	/*       Bit 3: Supports double transfer rate clocking */ (0 << 3) +
	/*       Bit 4: Supports 1-2-2 fast read               */ (1 << 4) +
	/*       Bit 5: Supports 1-4-4 fast read               */ (1 << 5) +
	/*       Bit 6: Supports 1-1-4 fast read               */ (1 << 6) +
	/*       Bit 7: Unused                                 */ (1 << 7),
	/* @0x33: Reserved */ 0xff,
	/* @0x34-0x37: Flash size (in bits) */ 0xff, 0xff, 0xff, 0x7,
	/* @0x38 Bits 4-0: 1-4-4 FAST READ DUMMY cycle count   */ 0x9 +
	/*       Bits 7-5: 1-4-4 fast read number of mode bits */ (1 << 5),
	/* @0x39: 1-4-4 FAST READ command code                 */ 0xeb,
	/* @0x3a Bits 4-0: 1-1-4 FAST READ DUMMY cycle count   */ 0x7 +
	/*       Bits 7-5: 1-1-4 fast read number of mode bits */ (1 << 5),
	/* @0x3b: 1-1-4 FAST READ command code                 */ 0x6b,
	/* @0x3c Bits 4-0: 1-1-2 FAST READ DUMMY cycle count   */ 0x8 +
	/*       Bits 7-5: 1-1-2 fast read number of mode bits */ (1 << 5),
	/* @0x3d: 1-1-2 FAST READ command code                 */ 0x3b,
	/* @0x3e Bits 4-0: 1-2-2 FAST READ DUMMY cycle count   */ 0x7 +
	/*       Bits 7-5: 1-2-2 fast read number of mode bits */ (1 << 5),
	/* @0x3f: 1-2-2 Instruction opcode                     */ 0xbb,
	/* @0x40    Bit 0: Support 2-2-2 fast read */ 1 +
	/*       Bits 3-1: Reserved */ (7 << 1) +
	/*          Bit 4: Support 4-4-4 fast read */ 1 +
	/*        Bit 7-5: Reserved */ (7 << 5),
	/* @0x41-0x43: Reserved */ 0xff, 0xff, 0xff, 0xff,
	/* @0x44-0x45: Reserved */ 0xff, 0xff,
	/* @0x46 Bits 4-0: 2-2-2 FAST READ DUMMY cycle count   */ 0x8 +
	/*       Bits 7-5: 2-2-2 fast read number of mode bits */ (1 << 5),
	/* @0x47: 2-2-2 FAST READ command code                 */ 0xbb,
	/* @0x48-0x49: Reserved */ 0xff, 0xff,
	/* @0x4a Bits 4-0: 4-4-4 FAST READ DUMMY cycle count   */ 0xa +
	/*       Bits 7-5: 4-4-4 fast read number of mode bits */ (1 << 5),
	/* @0x4b: 2-2-2 FAST READ command code                 */ 0xeb,
	/* @0x4c: Section type 1 size */ 0x0c,
	/* @0x4d: Section type 1 command code */ 0x20,
	/* @0x4e: Section type 2 size */ 0x10,
	/* @0x4f: Section type 2 command code */ 0xd8,
	/* @0x50: Section type 3 size */ 0x00,
	/* @0x51: Section type 3 command code */ 0x00,
	/* @0x52: Section type 4 size */ 0x00,
	/* @0x53: Section type 4 command code */ 0x00
};

//////////////// Operating conditions (typical values at 25°C) /////////////////////

const double N25Q032A_CONFIG::tWNVCR;
const double N25Q032A_CONFIG::tW;
const double N25Q032A_CONFIG::tCFSR;
const double N25Q032A_CONFIG::tWVCR;
const double N25Q032A_CONFIG::tWRVECR;
const double N25Q032A_CONFIG::tSHSL2;
const double N25Q032A_CONFIG::tPOTP;
const double N25Q032A_CONFIG::tPP;
const double N25Q032A_CONFIG::tPP_VPP;
const double N25Q032A_CONFIG::tSSE;
const double N25Q032A_CONFIG::tSE;
const double N25Q032A_CONFIG::tSE_VPP;
const double N25Q032A_CONFIG::tBE;
const double N25Q032A_CONFIG::tBE_VPP;
const double N25Q032A_CONFIG::erase_resume_to_suspend;
const double N25Q032A_CONFIG::program_resume_to_suspend;
const double N25Q032A_CONFIG::subsector_erase_resume_to_suspend;
const double N25Q032A_CONFIG::suspend_latency_program;
const double N25Q032A_CONFIG::suspend_latency_subsector_erase;
const double N25Q032A_CONFIG::suspend_latency_erase;

} // end of namespace n25q
} // end of namespace flash
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim
