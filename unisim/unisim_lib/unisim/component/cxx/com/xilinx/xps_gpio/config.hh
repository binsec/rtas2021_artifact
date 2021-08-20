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

#ifndef __UNISIM_COMPONENT_CXX_GPIO_XILINX_XPS_GPIO_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_GPIO_XILINX_XPS_GPIO_CONFIG_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace com {
namespace xilinx {
namespace xps_gpio {

class AddressMap
{
public:
	static const unsigned int GPIO_DATA = 0x0;
	static const unsigned int GPIO_TRI = 0x4;
	static const unsigned int GPIO2_DATA = 0x8;
	static const unsigned int GPIO2_TRI = 0xc;
	static const unsigned int GIER = 0x11c;
	static const unsigned int IP_IER = 0x128;
	static const unsigned int IP_ISR = 0x120;
};

class GIERLayout
{
public:
	static const unsigned int GIER_GLOBAL_INTERRUPT_ENABLE_OFFSET = 31;

	static const unsigned int GIER_GLOBAL_INTERRUPT_ENABLE_BITSIZE = 1;

	static const uint32_t GIER_GLOBAL_INTERRUPT_ENABLE_MASK = ((1UL << GIER_GLOBAL_INTERRUPT_ENABLE_BITSIZE) - 1) << GIER_GLOBAL_INTERRUPT_ENABLE_OFFSET;

	static const uint32_t GIER_MASK = GIER_GLOBAL_INTERRUPT_ENABLE_MASK;
};

class IP_IER_Layout
{
public:
	static const unsigned int IP_IER_CHANNEL_2_INTERRUPT_ENABLE_OFFSET = 1;
	static const unsigned int IP_IER_CHANNEL_1_INTERRUPT_ENABLE_OFFSET = 0;

	static const unsigned int IP_IER_CHANNEL_2_INTERRUPT_ENABLE_BITSIZE = 1;
	static const unsigned int IP_IER_CHANNEL_1_INTERRUPT_ENABLE_BITSIZE = 0;

	static const uint32_t IP_IER_CHANNEL_2_INTERRUPT_ENABLE_MASK = ((1UL << IP_IER_CHANNEL_2_INTERRUPT_ENABLE_BITSIZE) - 1) << IP_IER_CHANNEL_2_INTERRUPT_ENABLE_OFFSET;
	static const uint32_t IP_IER_CHANNEL_1_INTERRUPT_ENABLE_MASK = ((1UL << IP_IER_CHANNEL_1_INTERRUPT_ENABLE_BITSIZE) - 1) << IP_IER_CHANNEL_1_INTERRUPT_ENABLE_OFFSET;

	static const uint32_t IP_IER_MASK = IP_IER_CHANNEL_2_INTERRUPT_ENABLE_MASK | IP_IER_CHANNEL_1_INTERRUPT_ENABLE_MASK;
};

class IP_ISR_Layout
{
public:
	static const unsigned int IP_ISR_CHANNEL_2_INTERRUPT_STATUS_OFFSET = 1;
	static const unsigned int IP_ISR_CHANNEL_1_INTERRUPT_STATUS_OFFSET = 0;

	static const unsigned int IP_ISR_CHANNEL_2_INTERRUPT_STATUS_BITSIZE = 1;
	static const unsigned int IP_ISR_CHANNEL_1_INTERRUPT_STATUS_BITSIZE = 0;

	static const uint32_t IP_ISR_CHANNEL_2_INTERRUPT_STATUS_MASK = ((1UL << IP_ISR_CHANNEL_2_INTERRUPT_STATUS_BITSIZE) - 1) << IP_ISR_CHANNEL_2_INTERRUPT_STATUS_OFFSET;
	static const uint32_t IP_ISR_CHANNEL_1_INTERRUPT_STATUS_MASK = ((1UL << IP_ISR_CHANNEL_1_INTERRUPT_STATUS_BITSIZE) - 1) << IP_ISR_CHANNEL_1_INTERRUPT_STATUS_OFFSET;

	static const uint32_t IP_ISR_MASK = IP_ISR_CHANNEL_2_INTERRUPT_STATUS_MASK | IP_ISR_CHANNEL_1_INTERRUPT_STATUS_MASK;
	static const uint32_t IP_ISR_TOGGLE_ON_WRITE_MASK = IP_ISR_CHANNEL_2_INTERRUPT_STATUS_MASK | IP_ISR_CHANNEL_1_INTERRUPT_STATUS_MASK;
};

class Config
	: public AddressMap
	, public GIERLayout
	, public IP_IER_Layout
	, public IP_ISR_Layout
{
public:
	typedef uint64_t MEMORY_ADDR;
	
	static const unsigned int C_SPLB_DWITH = 128;       // PLB data with (in bits)
	static const unsigned int C_GPIO_WIDTH = 0;         // The width in bits of GPIO Channel 1
	static const unsigned int C_GPIO2_WIDTH = 0;        // The width in bits of GPIO Channel 2
	static const MEMORY_ADDR C_BASEADDR = 0x0ULL;       // XPS GPIO Base Address default value
	static const MEMORY_ADDR C_HIGHADDR = 0x0ULL;       // XPS GPIO High Address default value
	
	// default values
	static const uint32_t C_DOUT_DEFAULT = 0x00000000UL;   // GPIO_DATA reset value
	static const uint32_t C_TRI_DEFAULT = 0xffffffffUL;    // GPIO_TRI reset value
	static const uint32_t C_DOUT_DEFAULT_2 = 0x00000000UL; // GPIO2_DATA reset value
	static const uint32_t C_TRI_DEFAULT_2 = 0xffffffffUL;  // GPIO2_TRI reset value
	
	// Optional features
	static const bool C_IS_DUAL = false;              // Whether it is a dual channel GPIO
	static const bool C_INTERRUPT_IS_PRESENT = false; // Whether interrupt is present or not
};

} // end of namespace xps_gpio
} // end of namespace xilinx
} // end of namespace com
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
