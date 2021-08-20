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

#ifndef __UNISIM_COMPONENT_CXX_INTERRUPT_XILINX_XPS_INTC_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_INTERRUPT_XILINX_XPS_INTC_CONFIG_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace interrupt {
namespace xilinx {
namespace xps_intc {

class AddressMap
{
public:
	static const unsigned int ISR = 0x0;
	static const unsigned int IPR = 0x4;
	static const unsigned int IER = 0x8;
	static const unsigned int IAR = 0xc;
	static const unsigned int SIE = 0x10;
	static const unsigned int CIE = 0x14;
	static const unsigned int IVR = 0x18;
	static const unsigned int MER = 0x1c;
};

class MERLayout
{
public:
	static const unsigned int MER_ME_OFFSET = 0;
	static const unsigned int MER_HIE_OFFSET = 1;

	static const unsigned int MER_ME_BITSIZE = 1;
	static const unsigned int MER_HIE_BITSIZE = 1;
	
	static const uint32_t MER_ME_MASK = ((1UL << MER_ME_BITSIZE) - 1) << MER_ME_OFFSET;
	static const uint32_t MER_HIE_MASK = ((1UL << MER_HIE_BITSIZE) - 1) << MER_HIE_OFFSET;

	static const uint32_t MER_MASK = MER_ME_MASK | MER_HIE_MASK;
};
	
class Config
	: public AddressMap
	, public MERLayout
{
public:
	typedef uint64_t MEMORY_ADDR;
	
	static const unsigned int C_SPLB_DWITH = 128;        // PLB data with (in bits)
	static const unsigned int C_NUM_INTR_INPUTS = 32;    // Number of interrupt inputs
	static const MEMORY_ADDR C_BASEADDR = 0x81800000ULL; // XPS INTC Base Address default value (as ML507 board)
	static const MEMORY_ADDR C_HIGHADDR = 0x8180ffffULL; // XPS INTC Base Address default value (as ML507 board)
	
	// Optional features
	static const bool C_HAS_IPR = true; // Indicates the presence of IPR
	static const bool C_HAS_SIE = true; // Indicates the presence of SIE
	static const bool C_HAS_CIE = true; // Indicates the presence of CIE
	static const bool C_HAS_IVR = true; // Indicates the presence of IVR

	// Input sensitivity
	static const uint32_t C_KIND_OF_INTR = 0xffffffffUL; // Type of interrupt for each input: 1=Edge, 0=Level
	static const uint32_t C_KIND_OF_EDGE = 0xffffffffUL; // Type of edge sensitive input: 1=Rising, 0=Falling    Note: Valid if C_KIND_OF_INTR=1
	static const uint32_t C_KIND_OF_LVL = 0xffffffffUL;  // Type of level sensitive input: 1=High, 0=Low         Note: Valid if C_KIND_OF_INTR=0
	
	// Output sensitivity
	static const bool C_IRQ_IS_LEVEL = true;  // Indicates active or edge active IRQ: 0=Active Edge, 1=Active Level
	static const bool C_IRQ_ACTIVE = true;    // Indicates the sense of the IRQ output: 0=Falling/Low 1=Rising/High
};

} // end of namespace xps_intc
} // end of namespace xilinx
} // end of namespace interrupt
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
