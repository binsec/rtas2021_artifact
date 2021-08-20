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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CONFIG_HH__

#include <inttypes.h>

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

//=====================================================================
//=                  PowerPC common configuration                     =
//=====================================================================

class CRLayout
{
public:
	// CR fields offsets (here CR0)
	static const unsigned int CR0_LT_OFFSET = 31;
	static const unsigned int CR0_GT_OFFSET = 30;
	static const unsigned int CR0_EQ_OFFSET = 29;
	static const unsigned int CR0_SO_OFFSET = 28;

	// CR fields sizes
	static const unsigned int CR0_LT_BITSIZE = 1;
	static const unsigned int CR0_GT_BITSIZE = 1;
	static const unsigned int CR0_EQ_BITSIZE = 1;
	static const unsigned int CR0_SO_BITSIZE = 1;
	
	static const uint32_t CR0_LT_MASK = ((1UL << CR0_LT_BITSIZE) - 1) << CR0_LT_OFFSET;
	static const uint32_t CR0_GT_MASK = ((1UL << CR0_GT_BITSIZE) - 1) << CR0_GT_OFFSET;
	static const uint32_t CR0_EQ_MASK = ((1UL << CR0_EQ_BITSIZE) - 1) << CR0_EQ_OFFSET;
	static const uint32_t CR0_SO_MASK = ((1UL << CR0_SO_BITSIZE) - 1) << CR0_SO_OFFSET;
	
	static const uint32_t CR0_MASK = CR0_LT_MASK | CR0_GT_MASK | CR0_EQ_MASK | CR0_SO_MASK;
};

class XERLayout
{
public:
	// XER fields offsets
	static const unsigned int XER_SO_OFFSET = 31;
	static const unsigned int XER_OV_OFFSET = 30;
	static const unsigned int XER_CA_OFFSET = 29;
	static const unsigned int XER_LEGACY_OFFSET = 8;
	static const unsigned int XER_BYTE_COUNT_OFFSET = 0;

	// XER fields sizes
	static const unsigned int XER_SO_BITSIZE = 1;
	static const unsigned int XER_OV_BITSIZE = 1;
	static const unsigned int XER_CA_BITSIZE = 1;
	static const unsigned int XER_LEGACY_BITSIZE = 8;
	static const unsigned int XER_BYTE_COUNT_BITSIZE = 7;
	
	static const uint32_t XER_SO_MASK = ((1UL << XER_SO_BITSIZE) - 1) << XER_SO_OFFSET;
	static const uint32_t XER_OV_MASK = ((1UL << XER_OV_BITSIZE) - 1) << XER_OV_OFFSET;
	static const uint32_t XER_CA_MASK = ((1UL << XER_CA_BITSIZE) - 1) << XER_CA_OFFSET;
	static const uint32_t XER_LEGACY_MASK = ((1UL << XER_LEGACY_BITSIZE) - 1) << XER_LEGACY_OFFSET;
	static const uint32_t XER_BYTE_COUNT_MASK = ((1UL << XER_BYTE_COUNT_BITSIZE) - 1) << XER_BYTE_COUNT_OFFSET;

	static const uint32_t XER_MASK = XER_SO_MASK | XER_OV_MASK | XER_CA_MASK | XER_LEGACY_MASK | XER_BYTE_COUNT_MASK;
};

class FPSCRLayout
{
public:
	// Bit fields layout: FX FEX VX OX UX ZX XX VXSNAN VXISI VXIDI VXZDZ VXIMZ VXVC FR FI FPRF - VXSOFT VXSQRT VXCVI VE OE UE ZE XE NI RN

	// FPSCR write only bit fields while floating point computations
	static const unsigned int FPSCR_FX_OFFSET = 31;
	static const unsigned int FPSCR_FEX_OFFSET = 30;
	static const unsigned int FPSCR_VX_OFFSET = 29;
	static const unsigned int FPSCR_OX_OFFSET = 28;
	static const unsigned int FPSCR_UX_OFFSET = 27;
	static const unsigned int FPSCR_ZX_OFFSET = 26;
	static const unsigned int FPSCR_XX_OFFSET = 25;
	static const unsigned int FPSCR_VXSNAN_OFFSET = 24;
	static const unsigned int FPSCR_VXISI_OFFSET = 23;
	static const unsigned int FPSCR_VXIDI_OFFSET = 22;
	static const unsigned int FPSCR_VXZDZ_OFFSET = 21;
	static const unsigned int FPSCR_VXIMZ_OFFSET = 20;
	static const unsigned int FPSCR_VXVC_OFFSET = 19;
	static const unsigned int FPSCR_FR_OFFSET = 18;
	static const unsigned int FPSCR_FI_OFFSET = 17;
	static const unsigned int FPSCR_FPRF_OFFSET = 12;
	static const unsigned int FPSCR_FPCC_OFFSET = 12;
	static const unsigned int FPSCR_VXSOFT_OFFSET = 10;
	static const unsigned int FPSCR_VXSQRT_OFFSET = 9;
	static const unsigned int FPSCR_VXCVI_OFFSET = 8;
	
	// FPSCR read only bit fields while floating point computations
	static const unsigned int FPSCR_VE_OFFSET = 7;
	static const unsigned int FPSCR_OE_OFFSET = 6;
	static const unsigned int FPSCR_UE_OFFSET = 5;
	static const unsigned int FPSCR_ZE_OFFSET = 4;
	static const unsigned int FPSCR_XE_OFFSET = 3;
	static const unsigned int FPSCR_NI_OFFSET = 2;
	static const unsigned int FPSCR_RN_OFFSET = 0;

	// bit sizes of FPSCR write only bit fields while floating point computations
	static const unsigned int FPSCR_FX_BITSIZE = 1;
	static const unsigned int FPSCR_FEX_BITSIZE = 1;
	static const unsigned int FPSCR_VX_BITSIZE = 1;
	static const unsigned int FPSCR_OX_BITSIZE = 1;
	static const unsigned int FPSCR_UX_BITSIZE = 1;
	static const unsigned int FPSCR_ZX_BITSIZE = 1;
	static const unsigned int FPSCR_XX_BITSIZE = 1;
	static const unsigned int FPSCR_VXSNAN_BITSIZE = 1;
	static const unsigned int FPSCR_VXISI_BITSIZE = 1;
	static const unsigned int FPSCR_VXIDI_BITSIZE = 1;
	static const unsigned int FPSCR_VXZDZ_BITSIZE = 1;
	static const unsigned int FPSCR_VXIMZ_BITSIZE = 1;
	static const unsigned int FPSCR_VXVC_BITSIZE = 1;
	static const unsigned int FPSCR_FR_BITSIZE = 1;
	static const unsigned int FPSCR_FI_BITSIZE = 1;
	static const unsigned int FPSCR_FPRF_BITSIZE = 5;
	static const unsigned int FPSCR_FPCC_BITSIZE = 4;
	static const unsigned int FPSCR_VXSOFT_BITSIZE = 1;
	static const unsigned int FPSCR_VXSQRT_BITSIZE = 1;
	static const unsigned int FPSCR_VXCVI_BITSIZE = 1;

	// bit sizes of FPSCR read only bit fields while floating point computations
	static const unsigned int FPSCR_VE_BITSIZE = 1;
	static const unsigned int FPSCR_OE_BITSIZE = 1;
	static const unsigned int FPSCR_UE_BITSIZE = 1;
	static const unsigned int FPSCR_ZE_BITSIZE = 1;
	static const unsigned int FPSCR_XE_BITSIZE = 1;
	static const unsigned int FPSCR_NI_BITSIZE = 1;
	static const unsigned int FPSCR_RN_BITSIZE = 2;

	// binary masks of FPSCR write only bit fields while floating point computations
	static const uint32_t FPSCR_FX_MASK = ((1UL << FPSCR_FX_BITSIZE) - 1) << FPSCR_FX_OFFSET;
	static const uint32_t FPSCR_FEX_MASK = ((1UL << FPSCR_FEX_BITSIZE) - 1) << FPSCR_FEX_OFFSET;
	static const uint32_t FPSCR_VX_MASK = ((1UL << FPSCR_VX_BITSIZE) - 1) << FPSCR_VX_OFFSET;
	static const uint32_t FPSCR_OX_MASK = ((1UL << FPSCR_OX_BITSIZE) - 1) << FPSCR_OX_OFFSET;
	static const uint32_t FPSCR_UX_MASK = ((1UL << FPSCR_UX_BITSIZE) - 1) << FPSCR_UX_OFFSET;
	static const uint32_t FPSCR_ZX_MASK = ((1UL << FPSCR_ZX_BITSIZE) - 1) << FPSCR_ZX_OFFSET;
	static const uint32_t FPSCR_XX_MASK = ((1UL << FPSCR_XX_BITSIZE) - 1) << FPSCR_XX_OFFSET;
	static const uint32_t FPSCR_VXSNAN_MASK = ((1UL << FPSCR_VXSNAN_BITSIZE) - 1) << FPSCR_VXSNAN_OFFSET;
	static const uint32_t FPSCR_VXISI_MASK = ((1UL << FPSCR_VXISI_BITSIZE) - 1) << FPSCR_VXISI_OFFSET;
	static const uint32_t FPSCR_VXIDI_MASK = ((1UL << FPSCR_VXIDI_BITSIZE) - 1) << FPSCR_VXIDI_OFFSET;
	static const uint32_t FPSCR_VXZDZ_MASK = ((1UL << FPSCR_VXZDZ_BITSIZE) - 1) << FPSCR_VXZDZ_OFFSET;
	static const uint32_t FPSCR_VXIMZ_MASK = ((1UL << FPSCR_VXIMZ_BITSIZE) - 1) << FPSCR_VXIMZ_OFFSET;
	static const uint32_t FPSCR_VXVC_MASK = ((1UL << FPSCR_VXVC_BITSIZE) - 1) << FPSCR_VXVC_OFFSET;
	static const uint32_t FPSCR_FR_MASK = ((1UL << FPSCR_FR_BITSIZE) - 1) << FPSCR_FR_OFFSET;
	static const uint32_t FPSCR_FI_MASK = ((1UL << FPSCR_FI_BITSIZE) - 1) << FPSCR_FI_OFFSET;
	static const uint32_t FPSCR_FPRF_MASK = ((1UL << FPSCR_FPRF_BITSIZE) - 1) << FPSCR_FPRF_OFFSET;
	static const uint32_t FPSCR_FPCC_MASK = ((1UL << FPSCR_FPCC_BITSIZE) - 1) << FPSCR_FPCC_OFFSET;
	static const uint32_t FPSCR_VXSOFT_MASK = ((1UL << FPSCR_VXSOFT_BITSIZE) - 1) << FPSCR_VXSOFT_OFFSET;
	static const uint32_t FPSCR_VXSQRT_MASK = ((1UL << FPSCR_VXSQRT_BITSIZE) - 1) << FPSCR_VXSQRT_OFFSET;
	static const uint32_t FPSCR_VXCVI_MASK = ((1UL << FPSCR_VXCVI_BITSIZE) - 1) << FPSCR_VXCVI_OFFSET;

	// binary masks of FPSCR read only bit fields while floating point computations
	static const uint32_t FPSCR_VE_MASK = ((1UL << FPSCR_VE_BITSIZE) - 1) << FPSCR_VE_OFFSET;
	static const uint32_t FPSCR_OE_MASK = ((1UL << FPSCR_OE_BITSIZE) - 1) << FPSCR_OE_OFFSET;
	static const uint32_t FPSCR_UE_MASK = ((1UL << FPSCR_UE_BITSIZE) - 1) << FPSCR_UE_OFFSET;
	static const uint32_t FPSCR_ZE_MASK = ((1UL << FPSCR_ZE_BITSIZE) - 1) << FPSCR_ZE_OFFSET;
	static const uint32_t FPSCR_XE_MASK = ((1UL << FPSCR_XE_BITSIZE) - 1) << FPSCR_XE_OFFSET;
	static const uint32_t FPSCR_NI_MASK = ((1UL << FPSCR_NI_BITSIZE) - 1) << FPSCR_NI_OFFSET;
	static const uint32_t FPSCR_RN_MASK = ((1UL << FPSCR_RN_BITSIZE) - 1) << FPSCR_RN_OFFSET;

	// binary mask of FPSCR write only bit fields while floating point computations
	static const uint32_t FPSCR_WRONLY_MASK = FPSCR_FX_MASK | FPSCR_FEX_MASK | FPSCR_VX_MASK | FPSCR_OX_MASK | FPSCR_UX_MASK | FPSCR_ZX_MASK |
	                                          FPSCR_XX_MASK | FPSCR_VXSNAN_MASK | FPSCR_VXISI_MASK | FPSCR_VXIDI_MASK | FPSCR_VXZDZ_MASK |
	                                          FPSCR_VXIMZ_MASK | FPSCR_VXVC_MASK | FPSCR_FR_MASK | FPSCR_FI_MASK | FPSCR_FPRF_MASK | FPSCR_FPCC_MASK |
	                                          FPSCR_VXSOFT_MASK | FPSCR_VXSQRT_MASK | FPSCR_VXCVI_MASK;

	// binary mask of FPSCR read only bit fields while floating point computations
	static const uint32_t FPSCR_RDONLY_MASK = FPSCR_VE_MASK | FPSCR_OE_MASK | FPSCR_UE_MASK | FPSCR_ZE_MASK | FPSCR_XE_MASK | FPSCR_NI_MASK | FPSCR_RN_MASK;

	// binary mask of FPSCR sticky write only bit fields while floating point computations
	static const uint32_t FPSCR_STICKY_MASK = FPSCR_FX_MASK | FPSCR_OX_MASK | FPSCR_UX_MASK | FPSCR_ZX_MASK | FPSCR_XX_MASK | FPSCR_VXSNAN_MASK |
	                                          FPSCR_VXISI_MASK | FPSCR_VXIDI_MASK | FPSCR_VXZDZ_MASK | FPSCR_VXIMZ_MASK | FPSCR_VXVC_MASK |
	                                          FPSCR_VXSOFT_MASK | FPSCR_VXSQRT_MASK | FPSCR_VXCVI_MASK;

	// binary mask of FPSCR not sticky write only bit fields while floating point computations
	static const uint32_t FPSCR_NOT_STICKY_MASK = FPSCR_FEX_MASK | FPSCR_VX_MASK | FPSCR_FR_MASK | FPSCR_FI_MASK | FPSCR_FPRF_MASK;

	// meaning of FPRF bit field
	static const uint32_t FPRF_FOR_QNAN = 0x11UL;
	static const uint32_t FPRF_FOR_NEGATIVE_INFINITY = 0x9UL;
	static const uint32_t FPRF_FOR_NEGATIVE_NORMAL = 0x8UL;
	static const uint32_t FPRF_FOR_NEGATIVE_DENORMAL = 0x18UL;
	static const uint32_t FPRF_FOR_NEGATIVE_ZERO = 0x12UL;
	static const uint32_t FPRF_FOR_POSITIVE_ZERO = 0x2UL;
	static const uint32_t FPRF_FOR_POSITIVE_DENORMAL = 0x14UL;
	static const uint32_t FPRF_FOR_POSITIVE_NORMAL = 0x4UL;
	static const uint32_t FPRF_FOR_POSITIVE_INFINITY = 0x5UL;
};


class Config : public CRLayout, public XERLayout, public FPSCRLayout
{
public:
	typedef enum { MAT_READ = 0, MAT_WRITE = 1 } MemoryAccessType;
	
	typedef enum { MT_DATA = 0, MT_INSN = 1 } MemoryType;
	
	typedef enum { PR_SUPERVISOR = 0, PR_USER = 1 } PrivilegeLevel;
	
	typedef enum {
		// Freescale models
		// G1 processors
		MPC601 = 0,
		// G2 processors
		MPC603E,
		MPC604E,
		// G3 processors
		MPC740,
		MPC745,
		MPC750,
		MPC755,
		// G4 processors
		MPC7400,
		MPC7410,
		MPC7441,
		MPC7445,
		MPC7447,
		MPC7447A,
		MPC7448,
		MPC7450,
		MPC7451,
		MPC7455,
		MPC7457,
		// IBM models
		PPC405,
		PPC440,
		// Freescale models
		E200Z,
		NUM_MODELS
	} Model;
	
	typedef enum operand_type_t
	{
		GPR_T,    // General Purpose Register
		GPR_ST_T, // General Purpose Register to be stored in memory
		FPR_T,    // Floating Point Register
		FPR_ST_T, // Floating Point Register to be stored in memory
		CR_T,     // Condition Register
		FPSCR_T,  // Floating Point Status and Control Register
		XER_T,    // XER Register
		LR_T,     // Link Register
		CTR_T,    // Count Register
		SPR_T     // Special Purpose Register
	} operand_type_t;

	typedef enum operand_direction_t
	{
		INPUT_T, // Input Operand
		OUTPUT_T // Output Operand
	} operand_direction_t;

	typedef enum execution_unit_type_t
	{
		NO_UNIT_T, // No Unit
		LSU_T,     // Load/Store Unit
		IU1_T,     // Simple Integer Unit
		IU2_T,     // Complex Integer Unit
		FPU_T,     // Floating Point Unit
		BPU_T,     // Branch Processing Unit
		VFPU_T,    // Vector Floating Point Unit
		VPU_T,     // vector Processing Unit
		VIU1_T,    // Simple Vector Integer
		VIU2_T     // Complex Vector Integer
	} execution_unit_type_t;

	typedef enum serialization_t
	{
		NO_SERIALIZATION = 0,
		EXECUTION_SERIALIZATION = 1,
		REFETCH_SERIALIZATION = 2,
		STORE_SERIALIZATION = 4
	} serialization_t;

	typedef struct operand_t
	{
		operand_type_t type;
		operand_direction_t dir;
		unsigned int reg_num; // when type is GPR_T or FPR_T
	} operand_t;

	static const unsigned int MAX_OPERANDS = 8;

	typedef operand_t operands_t[MAX_OPERANDS];
};

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
