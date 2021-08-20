/*
 *  Copyright (c) 2010-2011,
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
 
#ifndef __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_MCI_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_MCI_CONFIG_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/logger.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace interconnect {
namespace xilinx {
namespace mci {

class DCRMap
{
public:
	static const uint32_t DCR_LOW          = 0x10;
	static const uint32_t DCR_HIGH         = 0x12;
	
	static const uint32_t MI_ROWCONFLICT_MASK  = 0x11;
	static const uint32_t MI_BANKCONFLICT_MASK = 0x12;
	static const uint32_t MI_CONTROL           = 0x10;
};

class MI_CONTROL_Layout
{
public:
	static const unsigned int MI_CONTROL_ENABLE_OFFSET = 31;
	static const unsigned int MI_CONTROL_ROWCONFLICTHOLDENABLE_OFFSET = 30;
	static const unsigned int MI_CONTROL_BANKCONFLICTHOLDENABLE_OFFSET = 29;
	static const unsigned int MI_CONTROL_DIRECTIONCONFLICTHOLDENABLE_OFFSET = 28;
	static const unsigned int MI_CONTROL_AUTOHOLDDURATION_OFFSET = 26;
	static const unsigned int MI_CONTROL_2_3_CLOCK_RATIO_MODE_OFFSET = 25;
	static const unsigned int MI_CONTROL_OVERLAPRDWR_OFFSET = 24;
	static const unsigned int MI_CONTROL_BURSTWIDTH_OFFSET = 22;
	static const unsigned int MI_CONTROL_BURSTLENGTH_OFFSET = 20;
	static const unsigned int MI_CONTROL_WDD_OFFSET = 16;
	static const unsigned int MI_CONTROL_RMW_OFFSET = 15;
	static const unsigned int MI_CONTROL_PLB_PRIORITY_ENABLE_OFFSET = 7;
	static const unsigned int MI_CONTROL_PIPELINED_READ_ENABLE_OFFSET = 3;
	static const unsigned int MI_CONTROL_PIPELINED_WRITE_ENABLE_OFFSET = 2;
	
	static const unsigned int MI_CONTROL_ENABLE_BITSIZE = 1;
	static const unsigned int MI_CONTROL_ROWCONFLICTHOLDENABLE_BITSIZE = 1;
	static const unsigned int MI_CONTROL_BANKCONFLICTHOLDENABLE_BITSIZE = 1;
	static const unsigned int MI_CONTROL_DIRECTIONCONFLICTHOLDENABLE_BITSIZE = 1;
	static const unsigned int MI_CONTROL_AUTOHOLDDURATION_BITSIZE = 2;
	static const unsigned int MI_CONTROL_2_3_CLOCK_RATIO_MODE_BITSIZE = 1;
	static const unsigned int MI_CONTROL_OVERLAPRDWR_BITSIZE = 1;
	static const unsigned int MI_CONTROL_BURSTWIDTH_BITSIZE = 2;
	static const unsigned int MI_CONTROL_BURSTLENGTH_BITSIZE = 2;
	static const unsigned int MI_CONTROL_WDD_BITSIZE = 4;
	static const unsigned int MI_CONTROL_RMW_BITSIZE = 1;
	static const unsigned int MI_CONTROL_PLB_PRIORITY_ENABLE_BITSIZE = 1;
	static const unsigned int MI_CONTROL_PIPELINED_READ_ENABLE_BITSIZE = 1;
	static const unsigned int MI_CONTROL_PIPELINED_WRITE_ENABLE_BITSIZE = 1;

	static const uint32_t MI_CONTROL_ENABLE_MASK = ((1UL << MI_CONTROL_ENABLE_BITSIZE) - 1) << MI_CONTROL_ENABLE_OFFSET;
	static const uint32_t MI_CONTROL_ROWCONFLICTHOLDENABLE_MASK = ((1UL << MI_CONTROL_ROWCONFLICTHOLDENABLE_BITSIZE) - 1) << MI_CONTROL_ROWCONFLICTHOLDENABLE_OFFSET;
	static const uint32_t MI_CONTROL_BANKCONFLICTHOLDENABLE_MASK = ((1UL << MI_CONTROL_BANKCONFLICTHOLDENABLE_BITSIZE) - 1) << MI_CONTROL_BANKCONFLICTHOLDENABLE_OFFSET;
	static const uint32_t MI_CONTROL_DIRECTIONCONFLICTHOLDENABLE_MASK = ((1UL << MI_CONTROL_DIRECTIONCONFLICTHOLDENABLE_BITSIZE) - 1) << MI_CONTROL_DIRECTIONCONFLICTHOLDENABLE_OFFSET;
	static const uint32_t MI_CONTROL_AUTOHOLDDURATION_MASK = ((1UL << MI_CONTROL_AUTOHOLDDURATION_BITSIZE) - 1) << MI_CONTROL_AUTOHOLDDURATION_OFFSET;
	static const uint32_t MI_CONTROL_2_3_CLOCK_RATIO_MODE_MASK = ((1UL << MI_CONTROL_2_3_CLOCK_RATIO_MODE_BITSIZE) - 1) << MI_CONTROL_2_3_CLOCK_RATIO_MODE_OFFSET;
	static const uint32_t MI_CONTROL_OVERLAPRDWR_MASK = ((1UL << MI_CONTROL_OVERLAPRDWR_BITSIZE) - 1) << MI_CONTROL_OVERLAPRDWR_OFFSET;
	static const uint32_t MI_CONTROL_BURSTWIDTH_MASK = ((1UL << MI_CONTROL_BURSTWIDTH_BITSIZE) - 1) << MI_CONTROL_BURSTWIDTH_OFFSET;
	static const uint32_t MI_CONTROL_BURSTLENGTH_MASK = ((1UL << MI_CONTROL_BURSTLENGTH_BITSIZE) - 1) << MI_CONTROL_BURSTLENGTH_OFFSET;
	static const uint32_t MI_CONTROL_WDD_MASK = ((1UL << MI_CONTROL_WDD_BITSIZE) - 1) << MI_CONTROL_WDD_OFFSET;
	static const uint32_t MI_CONTROL_RMW_MASK = ((1UL << MI_CONTROL_RMW_BITSIZE) - 1) << MI_CONTROL_RMW_OFFSET;
	static const uint32_t MI_CONTROL_PLB_PRIORITY_ENABLE_MASK = ((1UL << MI_CONTROL_PLB_PRIORITY_ENABLE_BITSIZE) - 1) << MI_CONTROL_PLB_PRIORITY_ENABLE_OFFSET;
	static const uint32_t MI_CONTROL_PIPELINED_READ_ENABLE_MASK = ((1UL << MI_CONTROL_PIPELINED_READ_ENABLE_BITSIZE) - 1) << MI_CONTROL_PIPELINED_READ_ENABLE_OFFSET;
	static const uint32_t MI_CONTROL_PIPELINED_WRITE_ENABLE_MASK = ((1UL << MI_CONTROL_PIPELINED_WRITE_ENABLE_BITSIZE) - 1) << MI_CONTROL_PIPELINED_WRITE_ENABLE_OFFSET;

	static const uint32_t MI_CONTROL_WRITE_MASK = MI_CONTROL_ENABLE_MASK | MI_CONTROL_ROWCONFLICTHOLDENABLE_MASK | MI_CONTROL_BANKCONFLICTHOLDENABLE_MASK |
	                                              MI_CONTROL_DIRECTIONCONFLICTHOLDENABLE_MASK | MI_CONTROL_AUTOHOLDDURATION_MASK | MI_CONTROL_2_3_CLOCK_RATIO_MODE_MASK |
	                                              MI_CONTROL_OVERLAPRDWR_MASK | MI_CONTROL_BURSTWIDTH_MASK | MI_CONTROL_BURSTLENGTH_MASK | MI_CONTROL_WDD_MASK |
	                                              MI_CONTROL_RMW_MASK | MI_CONTROL_PLB_PRIORITY_ENABLE_MASK | MI_CONTROL_PIPELINED_READ_ENABLE_MASK |
	                                              MI_CONTROL_PIPELINED_WRITE_ENABLE_MASK;
	
	static const uint32_t MI_CONTROL_RESET_VALUE = 0x0087UL;
};

class Config
	: public DCRMap
	, public MI_CONTROL_Layout
{
public:
	typedef uint64_t ADDRESS;
	static const unsigned int MCI_WIDTH = 16; // MCI Width in bytes
};

} // end of namespace mci
} // end of namespace xilinx
} // end of namespace interconnect
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_MCI_CONFIG_HH__
