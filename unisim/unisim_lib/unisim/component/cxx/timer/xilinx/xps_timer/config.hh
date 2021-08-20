/*
 *  Copyright (c) 2011,
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

#ifndef __UNISIM_COMPONENT_CXX_TIMER_XILINX_XPS_TIMER_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_TIMER_XILINX_XPS_TIMER_CONFIG_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace timer {
namespace xilinx {
namespace xps_timer {

class AddressMap
{
public:
	static const unsigned int TCSR0 = 0x0;
	static const unsigned int TLR0 = 0x4;
	static const unsigned int TCR0 = 0x8;
	static const unsigned int TCSR1 = 0x10;
	static const unsigned int TLR1 = 0x14;
	static const unsigned int TCR1 = 0x18;
};

class TCSR0Layout
{
public:
	static const unsigned int TCSR0_ENALL_OFFSET = 10;
	static const unsigned int TCSR0_PWMA0_OFFSET = 9;
	static const unsigned int TCSR0_T0INT_OFFSET = 8;
	static const unsigned int TCSR0_ENT0_OFFSET = 7;
	static const unsigned int TCSR0_ENIT0_OFFSET = 6;
	static const unsigned int TCSR0_LOAD0_OFFSET = 5;
	static const unsigned int TCSR0_ARHT0_OFFSET = 4;
	static const unsigned int TCSR0_CAPT0_OFFSET = 3;
	static const unsigned int TCSR0_GENT0_OFFSET = 2;
	static const unsigned int TCSR0_UDT0_OFFSET = 1;
	static const unsigned int TCSR0_MDT0_OFFSET = 0;

	static const unsigned int TCSR0_ENALL_BITSIZE = 1;
	static const unsigned int TCSR0_PWMA0_BITSIZE = 1;
	static const unsigned int TCSR0_T0INT_BITSIZE = 1;
	static const unsigned int TCSR0_ENT0_BITSIZE = 1;
	static const unsigned int TCSR0_ENIT0_BITSIZE = 1;
	static const unsigned int TCSR0_LOAD0_BITSIZE = 1;
	static const unsigned int TCSR0_ARHT0_BITSIZE = 1;
	static const unsigned int TCSR0_CAPT0_BITSIZE = 1;
	static const unsigned int TCSR0_GENT0_BITSIZE = 1;
	static const unsigned int TCSR0_UDT0_BITSIZE = 1;
	static const unsigned int TCSR0_MDT0_BITSIZE = 1;

	static const uint32_t TCSR0_ENALL_MASK = ((1UL << TCSR0_ENALL_BITSIZE) - 1) << TCSR0_ENALL_OFFSET;
	static const uint32_t TCSR0_PWMA0_MASK = ((1UL << TCSR0_PWMA0_BITSIZE) - 1) << TCSR0_PWMA0_OFFSET;
	static const uint32_t TCSR0_T0INT_MASK = ((1UL << TCSR0_T0INT_BITSIZE) - 1) << TCSR0_T0INT_OFFSET;
	static const uint32_t TCSR0_ENT0_MASK = ((1UL << TCSR0_ENT0_BITSIZE) - 1) << TCSR0_ENT0_OFFSET;
	static const uint32_t TCSR0_ENIT0_MASK = ((1UL << TCSR0_ENIT0_BITSIZE) - 1) << TCSR0_ENIT0_OFFSET;
	static const uint32_t TCSR0_LOAD0_MASK = ((1UL << TCSR0_LOAD0_BITSIZE) - 1) << TCSR0_LOAD0_OFFSET;
	static const uint32_t TCSR0_ARHT0_MASK = ((1UL << TCSR0_ARHT0_BITSIZE) - 1) << TCSR0_ARHT0_OFFSET;
	static const uint32_t TCSR0_CAPT0_MASK = ((1UL << TCSR0_CAPT0_BITSIZE) - 1) << TCSR0_CAPT0_OFFSET;
	static const uint32_t TCSR0_GENT0_MASK = ((1UL << TCSR0_GENT0_BITSIZE) - 1) << TCSR0_GENT0_OFFSET;
	static const uint32_t TCSR0_UDT0_MASK = ((1UL << TCSR0_UDT0_BITSIZE) - 1) << TCSR0_UDT0_OFFSET;
	static const uint32_t TCSR0_MDT0_MASK = ((1UL << TCSR0_MDT0_BITSIZE) - 1) << TCSR0_MDT0_OFFSET;

	static const uint32_t TCSR0_MASK = TCSR0_ENALL_MASK | TCSR0_PWMA0_MASK | TCSR0_T0INT_MASK | TCSR0_ENT0_MASK |
	                                   TCSR0_ENIT0_MASK | TCSR0_LOAD0_MASK | TCSR0_ARHT0_MASK | TCSR0_CAPT0_MASK |
	                                   TCSR0_GENT0_MASK | TCSR0_UDT0_MASK | TCSR0_MDT0_MASK;
};

class TCSR1Layout
{
public:
	static const unsigned int TCSR1_ENALL_OFFSET = 10;
	static const unsigned int TCSR1_PWMB0_OFFSET = 9;
	static const unsigned int TCSR1_T1INT_OFFSET = 8;
	static const unsigned int TCSR1_ENT1_OFFSET = 7;
	static const unsigned int TCSR1_ENIT1_OFFSET = 6;
	static const unsigned int TCSR1_LOAD1_OFFSET = 5;
	static const unsigned int TCSR1_ARHT1_OFFSET = 4;
	static const unsigned int TCSR1_CAPT1_OFFSET = 3;
	static const unsigned int TCSR1_GENT1_OFFSET = 2;
	static const unsigned int TCSR1_UDT1_OFFSET = 1;
	static const unsigned int TCSR1_MDT1_OFFSET = 0;

	static const unsigned int TCSR1_ENALL_BITSIZE = 1;
	static const unsigned int TCSR1_PWMB0_BITSIZE = 1;
	static const unsigned int TCSR1_T1INT_BITSIZE = 1;
	static const unsigned int TCSR1_ENT1_BITSIZE = 1;
	static const unsigned int TCSR1_ENIT1_BITSIZE = 1;
	static const unsigned int TCSR1_LOAD1_BITSIZE = 1;
	static const unsigned int TCSR1_ARHT1_BITSIZE = 1;
	static const unsigned int TCSR1_CAPT1_BITSIZE = 1;
	static const unsigned int TCSR1_GENT1_BITSIZE = 1;
	static const unsigned int TCSR1_UDT1_BITSIZE = 1;
	static const unsigned int TCSR1_MDT1_BITSIZE = 1;

	static const uint32_t TCSR1_ENALL_MASK = ((1UL << TCSR1_ENALL_BITSIZE) - 1) << TCSR1_ENALL_OFFSET;
	static const uint32_t TCSR1_PWMB0_MASK = ((1UL << TCSR1_PWMB0_BITSIZE) - 1) << TCSR1_PWMB0_OFFSET;
	static const uint32_t TCSR1_T1INT_MASK = ((1UL << TCSR1_T1INT_BITSIZE) - 1) << TCSR1_T1INT_OFFSET;
	static const uint32_t TCSR1_ENT1_MASK = ((1UL << TCSR1_ENT1_BITSIZE) - 1) << TCSR1_ENT1_OFFSET;
	static const uint32_t TCSR1_ENIT1_MASK = ((1UL << TCSR1_ENIT1_BITSIZE) - 1) << TCSR1_ENIT1_OFFSET;
	static const uint32_t TCSR1_LOAD1_MASK = ((1UL << TCSR1_LOAD1_BITSIZE) - 1) << TCSR1_LOAD1_OFFSET;
	static const uint32_t TCSR1_ARHT1_MASK = ((1UL << TCSR1_ARHT1_BITSIZE) - 1) << TCSR1_ARHT1_OFFSET;
	static const uint32_t TCSR1_CAPT1_MASK = ((1UL << TCSR1_CAPT1_BITSIZE) - 1) << TCSR1_CAPT1_OFFSET;
	static const uint32_t TCSR1_GENT1_MASK = ((1UL << TCSR1_GENT1_BITSIZE) - 1) << TCSR1_GENT1_OFFSET;
	static const uint32_t TCSR1_UDT1_MASK = ((1UL << TCSR1_UDT1_BITSIZE) - 1) << TCSR1_UDT1_OFFSET;
	static const uint32_t TCSR1_MDT1_MASK = ((1UL << TCSR1_MDT1_BITSIZE) - 1) << TCSR1_MDT1_OFFSET;

	static const uint32_t TCSR1_MASK = TCSR1_ENALL_MASK | TCSR1_PWMB0_MASK | TCSR1_T1INT_MASK | TCSR1_ENT1_MASK |
	                                   TCSR1_ENIT1_MASK | TCSR1_LOAD1_MASK | TCSR1_ARHT1_MASK | TCSR1_CAPT1_MASK |
	                                   TCSR1_GENT1_MASK | TCSR1_UDT1_MASK | TCSR1_MDT1_MASK;
};

class Config
	: public AddressMap
	, public TCSR0Layout
	, public TCSR1Layout
{
public:
	typedef uint64_t MEMORY_ADDR;
	
	static const unsigned int C_SPLB_DWITH = 128;        // PLB data with (in bits)
	static const unsigned int C_COUNT_WIDTH = 32;        // The width in bits of the counters in the XPS Timer/Counter
	static const MEMORY_ADDR C_BASEADDR = 0x83c00000ULL; // XPS Timer/Counter Base Address default value (as ML507 board)
	static const MEMORY_ADDR C_HIGHADDR = 0x83c0ffffULL; // XPS Timer/Counter High Address default value (as ML507 board)
	
	// Optional features
	static const bool C_ONE_TIMER_ONLY = false;  // Number of Timer modules
	static const bool C_TRIG0_ASSERT = true;     // Assertion level for CaptureTrig0
	static const bool C_TRIG1_ASSERT = true;     // Assertion level for CaptureTrig1
	static const bool C_GEN0_ASSERT = true;      // Assertion level for GenerateOut0
	static const bool C_GEN1_ASSERT = true;      // Assertion level for GenerateOut1
	
	static const uint32_t COUNTER_MASK = (1ULL << C_COUNT_WIDTH) - 1; // TLR0, TCR0, TLR1, TCR1 bit masks
	static const uint32_t MAX_COUNT = COUNTER_MASK;
	static const unsigned int NUM_TIMERS = C_ONE_TIMER_ONLY ? 1 : 2;
};

} // end of namespace xps_timer
} // end of namespace xilinx
} // end of namespace timer
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
