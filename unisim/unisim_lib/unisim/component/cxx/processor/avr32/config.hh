/*
 *  Copyright (c) 2014,
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
 * Authors: Julien Lisita (julien.lisita@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_AVR32_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_AVR32_CONFIG_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace avr32 {

typedef enum
{
	ARCH_AVR32A,
	ARCH_ARV32B
} Architecture;
	
//=====================================================================
//=                    AVR32 common configuration                     =
//=====================================================================

class SRLayout
{
public:
	// SR fields offsets
	static const unsigned int SR_SS_OFFSET = 31;
	static const unsigned int SR_H_OFFSET = 29;
	static const unsigned int SR_J_OFFSET = 28;
	static const unsigned int SR_DM_OFFSET = 27;
	static const unsigned int SR_D_OFFSET = 26;
	static const unsigned int SR_M2_OFFSET = 24;
	static const unsigned int SR_M1_OFFSET = 23;
	static const unsigned int SR_M0_OFFSET = 22;
	static const unsigned int SR_M_OFFSET = 22;
	static const unsigned int SR_EM_OFFSET = 21;
	static const unsigned int SR_I3M_OFFSET = 20;
	static const unsigned int SR_I2M_OFFSET = 19;
	static const unsigned int SR_I1M_OFFSET = 18;
	static const unsigned int SR_I0M_OFFSET = 17;
	static const unsigned int SR_GM_OFFSET = 16;

	static const unsigned int SR_R_OFFSET = 15;
	static const unsigned int SR_T_OFFSET = 14;
	static const unsigned int SR_L_OFFSET = 5;
	static const unsigned int SR_Q_OFFSET = 4;
	static const unsigned int SR_V_OFFSET = 3;
	static const unsigned int SR_N_OFFSET = 2;
	static const unsigned int SR_Z_OFFSET = 1;
	static const unsigned int SR_C_OFFSET = 0;

	// SR fields sizes
	static const unsigned int SR_SS_BITSIZE = 1;
	static const unsigned int SR_H_BITSIZE = 1;
	static const unsigned int SR_J_BITSIZE = 1;
	static const unsigned int SR_DM_BITSIZE = 1;
	static const unsigned int SR_D_BITSIZE = 1;
	static const unsigned int SR_M2_BITSIZE = 1;
	static const unsigned int SR_M1_BITSIZE = 1;
	static const unsigned int SR_M0_BITSIZE = 1;
	static const unsigned int SR_M_BITSIZE = 3;
	static const unsigned int SR_EM_BITSIZE = 1;
	static const unsigned int SR_I3M_BITSIZE = 1;
	static const unsigned int SR_I2M_BITSIZE = 1;
	static const unsigned int SR_I1M_BITSIZE = 1;
	static const unsigned int SR_I0M_BITSIZE = 1;
	static const unsigned int SR_GM_BITSIZE = 1;

	static const unsigned int SR_R_BITSIZE = 1;
	static const unsigned int SR_T_BITSIZE = 1;
	static const unsigned int SR_L_BITSIZE = 1;
	static const unsigned int SR_Q_BITSIZE = 1;
	static const unsigned int SR_V_BITSIZE = 1;
	static const unsigned int SR_N_BITSIZE = 1;
	static const unsigned int SR_Z_BITSIZE = 1;
	static const unsigned int SR_C_BITSIZE = 1;

	static const uint32_t SR_SS_MASK = ((1UL << SR_SS_BITSIZE) - 1) << SR_SS_OFFSET;
	static const uint32_t SR_H_MASK = ((1UL << SR_H_BITSIZE) - 1) << SR_H_OFFSET;
	static const uint32_t SR_J_MASK = ((1UL << SR_J_BITSIZE) - 1) << SR_J_OFFSET;
	static const uint32_t SR_DM_MASK = ((1UL << SR_DM_BITSIZE) - 1) << SR_DM_OFFSET;
	static const uint32_t SR_D_MASK = ((1UL << SR_D_BITSIZE) - 1) << SR_D_OFFSET;
	static const uint32_t SR_M2_MASK = ((1UL << SR_M2_BITSIZE) - 1) << SR_M2_OFFSET;
	static const uint32_t SR_M1_MASK = ((1UL << SR_M1_BITSIZE) - 1) << SR_M1_OFFSET;
	static const uint32_t SR_M0_MASK = ((1UL << SR_M0_BITSIZE) - 1) << SR_M0_OFFSET;
	static const uint32_t SR_M_MASK = ((1UL << SR_M_BITSIZE) - 1) << SR_M_OFFSET;
	static const uint32_t SR_EM_MASK = ((1UL << SR_EM_BITSIZE) - 1) << SR_EM_OFFSET;
	static const uint32_t SR_I3M_MASK = ((1UL << SR_I3M_BITSIZE) - 1) << SR_I3M_OFFSET;
	static const uint32_t SR_I2M_MASK = ((1UL << SR_I2M_BITSIZE) - 1) << SR_I2M_OFFSET;
	static const uint32_t SR_I1M_MASK = ((1UL << SR_I1M_BITSIZE) - 1) << SR_I1M_OFFSET;
	static const uint32_t SR_I0M_MASK = ((1UL << SR_I0M_BITSIZE) - 1) << SR_I0M_OFFSET;
	static const uint32_t SR_GM_MASK = ((1UL << SR_GM_BITSIZE) - 1) << SR_GM_OFFSET;

	static const uint32_t SR_R_MASK = ((1UL << SR_R_BITSIZE) - 1) << SR_R_OFFSET;
	static const uint32_t SR_T_MASK = ((1UL << SR_T_BITSIZE) - 1) << SR_T_OFFSET;
	static const uint32_t SR_L_MASK = ((1UL << SR_L_BITSIZE) - 1) << SR_L_OFFSET;
	static const uint32_t SR_Q_MASK = ((1UL << SR_Q_BITSIZE) - 1) << SR_Q_OFFSET;
	static const uint32_t SR_V_MASK = ((1UL << SR_V_BITSIZE) - 1) << SR_V_OFFSET;
	static const uint32_t SR_N_MASK = ((1UL << SR_N_BITSIZE) - 1) << SR_N_OFFSET;
	static const uint32_t SR_Z_MASK = ((1UL << SR_Z_BITSIZE) - 1) << SR_Z_OFFSET;
	static const uint32_t SR_C_MASK = ((1UL << SR_C_BITSIZE) - 1) << SR_C_OFFSET;

	// SR bitwise AND mask
	static const uint32_t HW_SR_MASK = SR_SS_MASK | SR_H_MASK | SR_J_MASK | SR_DM_MASK | SR_D_MASK | SR_M_MASK | SR_EM_MASK |
	                                   SR_I3M_MASK | SR_I2M_MASK | SR_I1M_MASK | SR_I0M_MASK | SR_GM_MASK |
	                                   SR_R_MASK | SR_T_MASK | SR_L_MASK | SR_Q_MASK | SR_V_MASK | SR_N_MASK |
	                                   SR_Z_MASK | SR_C_MASK;

	// SR bitwise AND mask
	static const uint32_t SR_MASK = SR_H_MASK | SR_DM_MASK | SR_EM_MASK |
	                                SR_I3M_MASK | SR_I2M_MASK | SR_I1M_MASK | SR_I0M_MASK | SR_GM_MASK |
	                                SR_T_MASK | SR_L_MASK | SR_Q_MASK | SR_V_MASK | SR_N_MASK |
	                                SR_Z_MASK | SR_C_MASK;

	// SR bitwise AND mask
	static const uint32_t SR_APP_MASK = SR_T_MASK | SR_L_MASK | SR_Q_MASK | SR_V_MASK | SR_N_MASK |
	                                    SR_Z_MASK | SR_C_MASK;

	// SR reset value
	static const uint32_t SR_RESET_VALUE = SR_M0_MASK | SR_EM_MASK | SR_GM_MASK;
};

class Config
	: public SRLayout
{
public:
	typedef uint32_t address_t;
	typedef uint32_t physical_address_t;

	// Execution Mode
	static const uint32_t EXEC_MODE_NMI         = 7;    // Non-Maskable Interrupt
	static const uint32_t EXEC_MODE_EXC         = 6;    // Exception
	static const uint32_t EXEC_MODE_INT_LEVEL3  = 5;    // Interrupt Level 3
	static const uint32_t EXEC_MODE_INT_LEVEL2  = 4;    // Interrupt Level 2
	static const uint32_t EXEC_MODE_INT_LEVEL1  = 3;    // Interrupt Level 1
	static const uint32_t EXEC_MODE_INT_LEVEL0  = 2;    // Interrupt Level 0
	static const uint32_t EXEC_MODE_SUPERVISOR  = 1;    // Supervisor
	static const uint32_t EXEC_MODE_APPLICATION = 0;    // Application

	// mask byte selector
	static const uint32_t TOP_BYTE_MASK    = 0xFF000000;
	static const uint32_t UPPER_BYTE_MASK  = 0x00FF0000;
	static const uint32_t LOWER_BYTE_MASK  = 0x0000FF00;	
	static const uint32_t BOTTOM_BYTE_MASK = 0x000000FF;

	
	static const uint32_t TOP_HW_MASK    = 0xFFFF0000;	
	static const uint32_t BOTTOM_HW_MASK = 0x0000FFFF;
	
	// Exceptions in order of priority
	static const unsigned int EXC_UNDEFINED_BEHAVIOR = 0;
	static const unsigned int EXC_RESET = 1;
	static const unsigned int EXC_OCD_STOP_CPU = 2;
	static const unsigned int EXC_UNRECOVERABLE = 3;
	static const unsigned int EXC_TLB_MULTIPLE_HIT = 4;
	static const unsigned int EXC_BUS_ERROR_DATA_FETCH = 5;
	static const unsigned int EXC_BUS_ERROR_INSTRUCTION_FETCH = 6;
	static const unsigned int EXC_NMI = 7;
	static const unsigned int EXC_IRQ3 = 8;
	static const unsigned int EXC_IRQ2 = 9;
	static const unsigned int EXC_IRQ1 = 10;
	static const unsigned int EXC_IRQ0 = 11;
	static const unsigned int EXC_INSTRUCTION_ADDR = 12;
	static const unsigned int EXC_ITLB_MISS = 13;
	static const unsigned int EXC_ITLB_PROTECTION = 14;
	static const unsigned int EXC_BREAKPOINT = 15;
	static const unsigned int EXC_ILLEGAL_OPCODE = 16;
	static const unsigned int EXC_UNIMPLEMENTED_INSTRUCTION = 17;
	static const unsigned int EXC_PRIVILEGE_VIOLATION = 18;
	static const unsigned int EXC_FLOATING_POINT = 19;
	static const unsigned int EXC_COPROCESSOR_ABSENT = 20;
	static const unsigned int EXC_SUPERVISOR_CALL = 21;
	static const unsigned int EXC_DATA_ADDRESS_READ = 22;
	static const unsigned int EXC_DATA_ADDRESS_WRITE = 23;
	static const unsigned int EXC_DTLB_MISS_READ = 24;
	static const unsigned int EXC_DTLB_MISS_WRITE = 25;
	static const unsigned int EXC_DTLB_PROTECTION_READ = 26;
	static const unsigned int EXC_DTLB_PROTECTION_WRITE = 27;
	static const unsigned int EXC_DTLB_MODIFIED = 28;
	
	static const unsigned int NUM_EXCEPTIONS = 29;
	
	// Exceptions masks
	static const uint32_t EXC_ENABLE_RESET = 1 << EXC_RESET;
	static const uint32_t EXC_ENABLE_OCD_STOP_CPU = 1 << EXC_OCD_STOP_CPU;
	static const uint32_t EXC_ENABLE_UNRECOVERABLE = 1 << EXC_UNRECOVERABLE;
	static const uint32_t EXC_ENABLE_TLB_MULTIPLE_HIT = 1 << EXC_TLB_MULTIPLE_HIT;
	static const uint32_t EXC_ENABLE_BUS_ERROR_DATA_FETCH = 1 << EXC_BUS_ERROR_DATA_FETCH;
	static const uint32_t EXC_ENABLE_BUS_ERROR_INSTRUCTION_FETCH = 1 << EXC_BUS_ERROR_INSTRUCTION_FETCH;
	static const uint32_t EXC_ENABLE_NMI = 1 << EXC_NMI;
	static const uint32_t EXC_ENABLE_IRQ3 = 1 << EXC_IRQ3;
	static const uint32_t EXC_ENABLE_IRQ2 = 1 << EXC_IRQ2;
	static const uint32_t EXC_ENABLE_IRQ1 = 1 << EXC_IRQ1;
	static const uint32_t EXC_ENABLE_IRQ0 = 1 << EXC_IRQ0;
	static const uint32_t EXC_ENABLE_INSTRUCTION_ADDR = 1 << EXC_INSTRUCTION_ADDR;
	static const uint32_t EXC_ENABLE_ITLB_MISS = 1 << EXC_ITLB_MISS;
	static const uint32_t EXC_ENABLE_ITLB_PROTECTION = 1 << EXC_ITLB_PROTECTION;
	static const uint32_t EXC_ENABLE_BREAKPOINT = 1 << EXC_BREAKPOINT;
	static const uint32_t EXC_ENABLE_ILLEGAL_OPCODE = 1 << EXC_ILLEGAL_OPCODE;
	static const uint32_t EXC_ENABLE_UNIMPLEMENTED_INSTRUCTION = 1 << EXC_UNIMPLEMENTED_INSTRUCTION;
	static const uint32_t EXC_ENABLE_PRIVILEGE_VIOLATION = 1 << EXC_PRIVILEGE_VIOLATION;
	static const uint32_t EXC_ENABLE_FLOATING_POINT = 1 << EXC_FLOATING_POINT;
	static const uint32_t EXC_ENABLE_COPROCESSOR_ABSENT = 1 << EXC_COPROCESSOR_ABSENT;
	static const uint32_t EXC_ENABLE_SUPERVISOR_CALL = 1 << EXC_SUPERVISOR_CALL;
	static const uint32_t EXC_ENABLE_DATA_ADDRESS_READ = 1 << EXC_DATA_ADDRESS_READ;
	static const uint32_t EXC_ENABLE_DATA_ADDRESS_WRITE = 1 << EXC_DATA_ADDRESS_WRITE;
	static const uint32_t EXC_ENABLE_DTLB_MISS_READ = 1 << EXC_DTLB_MISS_READ;
	static const uint32_t EXC_ENABLE_DTLB_MISS_WRITE = 1 << EXC_DTLB_MISS_WRITE;
	static const uint32_t EXC_ENABLE_DTLB_PROTECTION_READ = 1 << EXC_DTLB_PROTECTION_READ;
	static const uint32_t EXC_ENABLE_DTLB_PROTECTION_WRITE = 1 << EXC_DTLB_PROTECTION_WRITE;
	static const uint32_t EXC_ENABLE_DTLB_MODIFIED = 1 << EXC_DTLB_MODIFIED;
	static const uint32_t EXC_ENABLE_UNDEFINED_BEHAVIOR = 1 << EXC_UNDEFINED_BEHAVIOR;

	// IRQ
	static const unsigned int NUM_IRQS = 4;
	
	// Disambiguation of disassemble
	static const bool DISAMBIGUATE_DISASM = false;

};

} // end of namespace avr32
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
