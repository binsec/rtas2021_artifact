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
 
#ifndef __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_CONFIG_HH__
#define __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_CONFIG_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace flash {
namespace n25q {

struct N25Q_CONFIG
{
	enum HOLD_RST_PIN_FEATURE
	{
		HOLD_PIN = 0,        // device has HOLD# pin
		RST_PIN  = 1         // device has RESET# pin
	};

	enum XIP_FEATURE
	{
		MICRON_XIP = 0,     // Volatile XIP bit is required
		BASIC_XIP  = 1      // Volatile XIP bit is not required
	};
};

struct N25Q032A_CONFIG : N25Q_CONFIG
{
	//////////////////////////// Device features /////////////////////////////////////
	
	static const uint32_t SIZE           = 4 * 1024 * 1024;    // 4 MB (32 Mb)
	static const uint32_t PAGE_SIZE      = 256;                // 256 B
	static const uint32_t SUBSECTOR_SIZE = 4 * 1024;           // 4 KB
	static const uint32_t SECTOR_SIZE    = 64 * 1024;          // 64 KB
	static const uint32_t OTP_ARRAY_SIZE = 64;                 // 64 B
	static const uint32_t ID_SIZE        = 20;                 // 20 B
	static const uint32_t SFDP_SIZE      = 84;                 // 84 B
	static const unsigned int MAX_PROGRAM_SUSPEND_OPERATION_NESTING_LEVEL = 1;
	
	////////////////////////// Device identification //////////////////////////////////

	static const uint8_t SFDP[SFDP_SIZE];
	
	///////////////////////////// Protected areas /////////////////////////////////////
	
	static bool IsProtected(uint32_t addr, unsigned int top_bottom, unsigned int bp)
	{
		if(bp == 0)  return false; // protected area: none
		
		if(bp >= 8) return false; // N/A
		
		uint32_t protected_area_size = SIZE / (1 << (7 - bp));

		return top_bottom ? addr < protected_area_size            // protected area: lower area
		                  : addr >= (SIZE - protected_area_size); // protected area: upper area
	}
	
	////////////////////////////// Max frequency /////////////////////////////////////
	
	static const double MAX_FREQUENCY = 108.0;             // MHz
	
	//////////////////// Max supported frequencies (Mhz) /////////////////////////////

	static const double FAST_READ_MAX_FREQUENCIES[10][5];
	
	//////////////// Operating conditions (typical values at 25°C) /////////////////////
	
	// Write NONVOLATILE CONFIGURATION REGISTER cycle time
	static const double tWNVCR = 0.2;    // s
	
	// WRITE STATUS REGISTER cycle time
	static const double tW = 1.3;        // ms
	
	// CLEAR FLAG STATUS REGISTER cycle time
	static const double tCFSR = 40;      // ns
	
	// WRITE VOLATILE CONFIGURATION REGISTER cycle time
	static const double tWVCR = 40;      // ns
	
	// WRITE ENHANCED VOLATILE CONFIGURATION REGISTER cycle time
	static const double tWRVECR = 40;    // ns
	
	// S# deselect time after a nonREAD command
	static const double tSHSL2 = 50.0;   // ns
	
	// PROGRAM OTP cycle time (64 bytes)
	static const double tPOTP = 0.2;     // ms
	
	// PAGE PROGRAM cycle time (per indivisible 8-bytes)
	static const double tPP = 0.015625;  // ms
	
	// PAGE PROGRAM cycle time (per indivisible 8-bytes) (VPP = VPPH)
	static const double tPP_VPP = 0.125;  // ms

	// SUBSECTOR ERASE cycle time
	static const double tSSE = 0.25;      // s

	// SECTOR ERASE cycle time
	static const double tSE = 0.7;        // s
	
	// SECTOR ERASE cycle time (VPP = VPPH)
	static const double tSE_VPP = 0.6;   // s
	
	// BULK ERASE cycle time
	static const double tBE = 30;        // s
	
	// BULK ERASE cycle time (VPP = VPPH)
	static const double tBE_VPP = 25;    // s
	
	// Erase to suspend (sector erase or erase resume to erase suspend)
	static const double erase_resume_to_suspend = 700;  // µs
	
	// Program to suspend (program resume to program suspend)
	static const double program_resume_to_suspend = 5; // µs
	
	// Subsector erase to suspend (subsector erase or subsector erase resume to erase suspend)
	static const double subsector_erase_resume_to_suspend = 50; // µs
	
	// Suspend latency (program)
	static const double suspend_latency_program = 7; // µs
	
	// Suspend latency (subsector erase)
	static const double suspend_latency_subsector_erase = 15; // µs
	
	// Suspend latency (erase)
	static const double suspend_latency_erase = 15; // µs
};

struct N25Q032A1_CONFIG : N25Q032A_CONFIG
{
	////////////////////////////////// Features ///////////////////////////////////////
	
	static const HOLD_RST_PIN_FEATURE HOLD_RST_PIN_F = HOLD_PIN;
	static const XIP_FEATURE XIP_F = MICRON_XIP;
	
	////////////////////////// Device identification //////////////////////////////////
	
	static const uint8_t ID[ID_SIZE];
};

struct N25Q032A2_CONFIG : N25Q032A_CONFIG
{
	////////////////////////////////// Features ///////////////////////////////////////
	
	static const HOLD_RST_PIN_FEATURE HOLD_RST_PIN_F = HOLD_PIN;
	static const XIP_FEATURE XIP_F = BASIC_XIP;
	
	////////////////////////// Device identification //////////////////////////////////
	
	static const uint8_t ID[ID_SIZE];
};

struct N25Q032A3_CONFIG : N25Q032A_CONFIG
{
	////////////////////////////////// Features ///////////////////////////////////////
	
	static const HOLD_RST_PIN_FEATURE HOLD_RST_PIN_F = RST_PIN;
	static const XIP_FEATURE XIP_F = MICRON_XIP;
	
	////////////////////////// Device identification //////////////////////////////////
	
	static const uint8_t ID[ID_SIZE];
};

struct N25Q032A4_CONFIG : N25Q032A_CONFIG
{
	////////////////////////////////// Features ///////////////////////////////////////
	
	static const HOLD_RST_PIN_FEATURE HOLD_RST_PIN_F = RST_PIN;
	static const XIP_FEATURE XIP_F = BASIC_XIP;
	
	////////////////////////// Device identification //////////////////////////////////
	
	static const uint8_t ID[ID_SIZE];
};

struct N25Q032A1_CONFIG32 : N25Q032A1_CONFIG
{
	typedef uint32_t ADDRESS;
};

struct N25Q032A2_CONFIG32 : N25Q032A2_CONFIG
{
	typedef uint32_t ADDRESS;
};

struct N25Q032A3_CONFIG32 : N25Q032A3_CONFIG
{
	typedef uint32_t ADDRESS;
};

struct N25Q032A4_CONFIG32 : N25Q032A4_CONFIG
{
	typedef uint32_t ADDRESS;
};

struct N25Q032A1_CONFIG64 : N25Q032A1_CONFIG
{
	typedef uint64_t ADDRESS;
};

struct N25Q032A2_CONFIG64 : N25Q032A2_CONFIG
{
	typedef uint64_t ADDRESS;
};

struct N25Q032A3_CONFIG64 : N25Q032A3_CONFIG
{
	typedef uint64_t ADDRESS;
};

struct N25Q032A4_CONFIG64 : N25Q032A4_CONFIG
{
	typedef uint64_t ADDRESS;
};

} // end of namespace n25q
} // end of namespace flash
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
