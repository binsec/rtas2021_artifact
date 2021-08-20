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
 
#ifndef __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_DCR_CONTROLLER_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_DCR_CONTROLLER_CONFIG_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace interconnect {
namespace xilinx {
namespace dcr_controller {

class DCRControllerStatusAndControlRegisterLayout
{
public:
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_LOCK_OFFSET = 31;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_ALOCK_OFFSET = 30;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_LOCK_OFFSET = 29;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ALOCK_OFFSET = 28;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_OFFSET = 27;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ASYN_OFFSET = 26;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XS_ASYN_OFFSET = 25;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TOWAIT_OFFSET = 24;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_TIMEOUT_OFFSET = 1;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TIMEOUT_OFFSET = 0;

	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_LOCK_BITSIZE = 1;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_ALOCK_BITSIZE = 1;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_LOCK_BITSIZE = 1;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ALOCK_BITSIZE = 1;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_BITSIZE = 1;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ASYN_BITSIZE = 1;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XS_ASYN_BITSIZE = 1;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TOWAIT_BITSIZE = 1;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_TIMEOUT_BITSIZE = 1;
	static const unsigned int DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TIMEOUT_BITSIZE = 1;
	
	static const uint32_t DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_LOCK_MASK = ((1UL << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_LOCK_BITSIZE) - 1) << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_LOCK_OFFSET;
	static const uint32_t DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_ALOCK_MASK = ((1UL << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_ALOCK_BITSIZE) - 1) << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_ALOCK_OFFSET;
	static const uint32_t DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_LOCK_MASK = ((1UL << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_LOCK_BITSIZE) - 1) << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_LOCK_OFFSET;
	static const uint32_t DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ALOCK_MASK = ((1UL << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ALOCK_BITSIZE) - 1) << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ALOCK_OFFSET;
	static const uint32_t DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_MASK = ((1UL << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_BITSIZE) - 1) << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_OFFSET;
	static const uint32_t DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ASYN_MASK = ((1UL << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ASYN_BITSIZE) - 1) << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ASYN_OFFSET;
	static const uint32_t DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XS_ASYN_MASK = ((1UL << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XS_ASYN_BITSIZE) - 1) << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XS_ASYN_OFFSET;
	static const uint32_t DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TOWAIT_MASK = ((1UL << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TOWAIT_BITSIZE) - 1) << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TOWAIT_OFFSET;
	static const uint32_t DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_TIMEOUT_MASK = ((1UL << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_TIMEOUT_BITSIZE) - 1) << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_TIMEOUT_OFFSET;
	static const uint32_t DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TIMEOUT_MASK = ((1UL << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TIMEOUT_BITSIZE) - 1) << DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TIMEOUT_OFFSET;

	static const uint32_t DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_MASK = DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_LOCK_MASK |
	                                                                        DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_ALOCK_MASK |
	                                                                        DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_LOCK_MASK |
	                                                                        DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ALOCK_MASK |
	                                                                        DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_MASK |
	                                                                        DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ASYN_MASK |
	                                                                        DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XS_ASYN_MASK |
	                                                                        DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TOWAIT_MASK |
	                                                                        DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_TIMEOUT_MASK |
	                                                                        DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TIMEOUT_MASK;

	static const uint32_t DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_WRITE_MASK = DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_LOCK_MASK |
	                                                                        DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_LOCK_MASK |
	                                                                        DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_MASK |
	                                                                        DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TOWAIT_MASK;
	static const uint32_t DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_WRITE_MASK = DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_LOCK_MASK |
	                                                                        DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_MASK |
	                                                                        DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TOWAIT_MASK;
};

class DCRControllerRoutingTable
{
public:
	static const unsigned int NUM_SLAVES = 11;
	static const unsigned int NUM_MASTERS = 2;
	static const uint32_t DCR_CONTROLLER_LOCATIONS = 256;                  // DCR controller locations

	// DCR Controller registers
	static const uint32_t DCR_CONTROLLER_LOW = 0x0;
	static const uint32_t DCR_CONTROLLER_HIGH = 0x2;
	static const uint32_t INDIRECT_MODE_ADDRESS_REGISTER = 0x00;
	static const uint32_t INDIRECT_MODE_ACCESS_REGISTER = 0x01;
	static const uint32_t DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER = 0x02;
	
	// APU
	static const unsigned int APU_SLAVE_NUM = 0;
	static const uint32_t APU_LOW = 0x04;
	static const uint32_t APU_HIGH = 0x05;
	
	// MCI (Memory Controller Interface)
	static const unsigned int MCI_SLAVE_NUM = 1;
	static const uint32_t MCI_LOW = 0x10;
	static const uint32_t MCI_HIGH = 0x12;
	
	// Crossbar
	static const unsigned int CROSSBAR_SLAVE_NUM = 2;
	static const uint32_t CROSSBAR_LOW = 0x20;
	static const uint32_t CROSSBAR_HIGH = 0x33;
	
	// PLB Slave 0
	static const unsigned int PLBS0_SLAVE_NUM = 3;
	static const uint32_t PLBS0_LOW = 0x34;
	static const uint32_t PLBS0_HIGH = 0x43;

	// PLB Slave 1
	static const unsigned int PLBS1_SLAVE_NUM = 4;
	static const uint32_t PLBS1_LOW = 0x44;
	static const uint32_t PLBS1_HIGH = 0x53;

	// PLB Master
	static const unsigned int PLBM_SLAVE_NUM = 5;
	static const uint32_t PLBM_LOW = 0x54;
	static const uint32_t PLBM_HIGH = 0x5f;

	// DMA engine 0
	static const unsigned int DMAC0_SLAVE_NUM = 6;
	static const uint32_t DMAC0_LOW = 0x80;
	static const uint32_t DMAC0_HIGH = 0x90;

	// DMA engine 1
	static const unsigned int DMAC1_SLAVE_NUM = 7;
	static const uint32_t DMAC1_LOW = 0x98;
	static const uint32_t DMAC1_HIGH = 0xa8;

	// DMA engine 2
	static const unsigned int DMAC2_SLAVE_NUM = 8;
	static const uint32_t DMAC2_LOW = 0xb0;
	static const uint32_t DMAC2_HIGH = 0xc0;

	// DMA engine 3
	static const unsigned int DMAC3_SLAVE_NUM = 9;
	static const uint32_t DMAC3_LOW = 0xc8;
	static const uint32_t DMAC3_HIGH = 0xd8;
	
	// External slave
	static const unsigned int EXTERNAL_SLAVE_NUM = 10;
};

class IndirectModeAddressRegisterLayout
{
public:
	static const uint32_t INDIRECT_MODE_ADDRESS_REGISTER_MASK = 0xfff; // 12-bit address for indirect mode
};

class Config
	: public DCRControllerStatusAndControlRegisterLayout
	, public IndirectModeAddressRegisterLayout
	, public DCRControllerRoutingTable
{
public:
	static const uint8_t TIEDCRBASEADDR = 0x0;              // 2-bit tie-off-pins
	
	static const unsigned int C440_MASTER_NUM = 0;
	static const unsigned int XM_MASTER_NUM = 1;
	
	static const bool DCR_AUTOLOCK_ENABLE = true;
	
};

} // end of namespace dcr_controller
} // end of namespace xilinx
} // end of namespace interconnect
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_DCR_CONTROLLER_CONFIG_HH__
