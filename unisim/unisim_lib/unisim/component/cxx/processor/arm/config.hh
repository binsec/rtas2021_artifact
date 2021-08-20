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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CONFIG_HH__

#include <inttypes.h>

#include "unisim/util/endian/endian.hh"
#include "unisim/component/cxx/processor/arm/cache/config.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

using unisim::util::endian::endian_type;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;

using unisim::component::cxx::processor::arm::cache::ARMCache512bDMWB8bls_Config;
using unisim::component::cxx::processor::arm::cache::ARMCache512bDMWB8bls_DebugConfig;
using unisim::component::cxx::processor::arm::cache::ARMCache512bDMWT8bls_Config;
using unisim::component::cxx::processor::arm::cache::ARMCache512bDMWT8bls_DebugConfig;
using unisim::component::cxx::processor::arm::cache::ARMCache32KBDMWB32bls_Config;
using unisim::component::cxx::processor::arm::cache::ARMCache32KBDMWB32bls_DebugConfig;
using unisim::component::cxx::processor::arm::cache::ARMCache32KBDMWT32bls_Config;
using unisim::component::cxx::processor::arm::cache::ARMCache32KBDMWT32bls_DebugConfig;
using unisim::component::cxx::processor::arm::cache::ARMCache64KBDMWB32bls_Config;
using unisim::component::cxx::processor::arm::cache::ARMCache64KBDMWB32bls_DebugConfig;
using unisim::component::cxx::processor::arm::cache::ARMCache64KBDMWT32bls_Config;
using unisim::component::cxx::processor::arm::cache::ARMCache64KBDMWT32bls_DebugConfig;
using unisim::component::cxx::processor::arm::cache::ARMCache96KBDMWB32bls_Config;
using unisim::component::cxx::processor::arm::cache::ARMCache96KBDMWB32bls_DebugConfig;
using unisim::component::cxx::processor::arm::cache::ARMCache96KBDMWT32bls_Config;
using unisim::component::cxx::processor::arm::cache::ARMCache96KBDMWT32bls_DebugConfig;

/**
 * The different processor models supported.
 * NOTE: for the moment only xxx is supported.
 */
typedef enum {
	/* ARM7 Family */
	ARM7TDMI = 0,
	ARM7TDMIS,
	ARM710T,
	ARM720T,
	ARM740T,
	/* ARM9 Family */
	ARM9TDMI,
	ARM920T,
	ARM922T,
	ARM940T,
	/* ARM9E-S and ARM9EJ-S Families */
	ARM9E_S,
	ARM9EJ_S,
	ARM926EJS,
	ARM946E_S,
	ARM966E_S,
	/* ARM10 Family */
	ARM10T,
	ARM1020E,
	ARM1020T,
	ARM1022E,
	ARM10200,
	ARM1026EJ_S,
	/* ARM11 Family */
	ARM1136
} Model;

/**
 * The different architectures supported.
 * NOTE: for the moment only xxx is supported.
 */
 typedef enum {
 	/* ARMv3 architecture family */
 	ARMV3 = 0,
 	ARMV3M,
 	/* ARMv4 architecture family */
	ARMV4,
	ARMV4XM,
	ARMV4T,
	ARMV4TXM,
 	/* ARMv5 architecture family */
 	ARMV5,
 	ARMV5XM,
 	ARMV5T,
 	ARMV5TXM,
 	ARMV5TE,
 	ARMV5TEXP
 } Architecture;

/**
 *  The different ARM instruction set versions 
 */
typedef enum {
	V3 = 0, 
	V4 = 1, 
	V5 = 2
} InsnSetVersion;

/**
 * The different THUMB instruction set versions.
 * NOTE: 0 means THUMB not supported
 */
typedef enum {
	V0 = 0, 
	V1 = 1, 
	V2 = 2
} THUMBInsnSetVersion;

class ARMBase {
public:
//	static const endian_type ENDIANESS = E_BIG_ENDIAN;

	static const bool DEBUG_ENABLE = true;
	
	static const bool HAS_LOAD_STORE_V4 = false;
	static const bool HAS_LOAD_STORE_V5E = false;
	static const bool HAS_INSN_BX = false;
	static const bool HAS_INSN_BLX = false;
	static const bool HAS_INSN_COPROCESSOR_ALTER_MOVES = false;
	static const bool HAS_INSN_COPROCESSOR_ALTER_LOADS = false;
	static const bool HAS_INSN_COPROCESSOR_ALTER_STORES = false;
	static const bool HAS_INSN_COPROCESSOR_ALTER_OPS = false;
	static const bool HAS_COPROCESSOR_V5E = false;
	static const bool HAS_INSN_ARITH_CLZ = false;
	static const bool HAS_INSN_ARITH_QADD = false;
	static const bool HAS_INSN_ARITH_QDADD = false;
	static const bool HAS_INSN_ARITH_QSUB = false;
	static const bool HAS_INSN_ARITH_QDSUB = false;
	static const bool HAS_INSN_ARITH_MULT_UMULL = false;
	static const bool HAS_INSN_ARITH_MULT_UMLAL = false;
	static const bool HAS_INSN_ARITH_MULT_SMULL = false;
	static const bool HAS_INSN_ARITH_MULT_SMLAL = false;
	static const bool HAS_INSN_ARITH_MULT_SMULXY = false;
	static const bool HAS_INSN_ARITH_MULT_SMULWY = false;
	static const bool HAS_INSN_ARITH_MULT_SMLAXY = false;
	static const bool HAS_INSN_ARITH_MULT_SMLAWY = false;
	static const bool HAS_INSN_ARITH_MULT_SMLALWY = false;
	
	static const bool HAS_ITCM = false;
	static const bool HAS_DTCM = false;
	
	static const uint32_t ITCM_SIZE = (uint32_t)0x1;
	static const uint32_t DTCM_SIZE = (uint32_t)0x1;
};

/**
 * Describes the configuration of the ARM7TDMI processor
 */
class ARM7TDMI_Config : 
	public ARMBase {
public:
	typedef uint32_t address_t;             // 32-bit effective address
	typedef uint32_t reg_t;                 // register type
	typedef int32_t sreg_t;                 // signed register type   
	typedef uint32_t insn_t;                // instruction type
	typedef uint16_t thumb_insn_t;          // thumb instruction type

	static const Model MODEL = ARM7TDMI; // !< the model definition
	static const uint32_t PROCESSOR_VERSION = 0; // !< the processor id
	static const Architecture ARCHITECTURE = ARMV4T; // !< the processor architecture
	
	static const InsnSetVersion INSN_SET_VERSION = V4; // !< the supported instruction set version
	static const THUMBInsnSetVersion THUMB_INSN_SET_VERSION = V2; // !< the supported thumb instruction set
	
	static const uint32_t FSB_BURST_SIZE = 32; // !< Front side bus parameters
	
	static const bool HAS_LOAD_STORE_V4 = true;
	static const bool HAS_LOAD_STORE_V5E = true;
	static const bool HAS_INSN_BX = true;
	static const bool HAS_INSN_BLX = true;
	static const bool HAS_INSN_COPROCESSOR_ALTER_MOVES = true;
	static const bool HAS_INSN_COPROCESSOR_ALTER_LOADS = true;
	static const bool HAS_INSN_COPROCESSOR_ALTER_STORES = true;
	static const bool HAS_INSN_COPROCESSOR_ALTER_OPS = true;
	static const bool HAS_INSN_ARITH_CLZ = true;
	static const bool HAS_INSN_ARITH_QADD = false;
	static const bool HAS_INSN_ARITH_QDADD = false;
	static const bool HAS_INSN_ARITH_QSUB = false;
	static const bool HAS_INSN_ARITH_QDSUB = false;
	static const bool HAS_INSN_ARITH_MULT_UMULL = true;
	static const bool HAS_INSN_ARITH_MULT_UMLAL = true;
	static const bool HAS_INSN_ARITH_MULT_SMULL = true;
	static const bool HAS_INSN_ARITH_MULT_SMLAL = true;
	static const bool HAS_INSN_ARITH_MULT_SMULXY = false;
	static const bool HAS_INSN_ARITH_MULT_SMULWY = false;
	static const bool HAS_INSN_ARITH_MULT_SMLAXY = false;
	static const bool HAS_INSN_ARITH_MULT_SMLAWY = false;
	static const bool HAS_INSN_ARITH_MULT_SMLALXY = false;
};

/**
 * Describes the configuration of the ARM9TDMI processor
 */
class ARM9TDMI_Config : 
	public ARMBase {
public:
	typedef uint32_t address_t;             // 32-bit effective address
	typedef uint32_t reg_t;                 // register type
	typedef int32_t sreg_t;                 // signed register type   
	typedef uint32_t insn_t;                // instruction type
	typedef uint16_t thumb_insn_t;          // thumb instruction type

	static const Model MODEL = ARM9TDMI; // !< the model definition
	static const uint32_t PROCESSOR_VERSION = 0; // !< the processor id
	static const Architecture ARCHITECTURE = ARMV5; // !< the processor architecture
	
	static const InsnSetVersion INSN_SET_VERSION = V5; // !< the supported instruction set version
	static const THUMBInsnSetVersion THUMB_INSN_SET_VERSION = V2; // !< the supported thumb instruction set
	
	static const uint32_t FSB_BURST_SIZE = 32; // !< Front side bus parameters
	
	static const bool HAS_LOAD_STORE_V4 = true;
	static const bool HAS_LOAD_STORE_V5E = true;
	static const bool HAS_INSN_BX = true;
	static const bool HAS_INSN_BLX = true;
	static const bool HAS_INSN_COPROCESSOR_ALTER_MOVES = true;
	static const bool HAS_INSN_COPROCESSOR_ALTER_LOADS = true;
	static const bool HAS_INSN_COPROCESSOR_ALTER_STORES = true;
	static const bool HAS_INSN_COPROCESSOR_ALTER_OPS = true;
	static const bool HAS_INSN_ARITH_CLZ = true;
	static const bool HAS_INSN_ARITH_QADD = false;
	static const bool HAS_INSN_ARITH_QDADD = false;
	static const bool HAS_INSN_ARITH_QSUB = false;
	static const bool HAS_INSN_ARITH_QDSUB = false;
	static const bool HAS_INSN_ARITH_MULT_UMULL = true;
	static const bool HAS_INSN_ARITH_MULT_UMLAL = true;
	static const bool HAS_INSN_ARITH_MULT_SMULL = true;
	static const bool HAS_INSN_ARITH_MULT_SMLAL = true;
	static const bool HAS_INSN_ARITH_MULT_SMULXY = false;
	static const bool HAS_INSN_ARITH_MULT_SMULWY = false;
	static const bool HAS_INSN_ARITH_MULT_SMLAXY = false;
	static const bool HAS_INSN_ARITH_MULT_SMLAWY = false;
	static const bool HAS_INSN_ARITH_MULT_SMLALXY = false;
};

/**
 * Describes the configuration of the ARM926EJ-S processor
 */
class ARM926EJS_Config : 
	public ARMBase {
public:
	typedef uint32_t address_t;             // 32-bit effective address
	typedef uint32_t reg_t;                 // register type
	typedef int32_t sreg_t;                 // signed register type   
	typedef uint32_t insn_t;                // instruction type
	typedef uint16_t thumb_insn_t;          // thumb instruction type

	static const Model MODEL = ARM926EJS; // !< the model definition
	static const uint32_t PROCESSOR_VERSION = 0; // !< the processor id
	static const Architecture ARCHITECTURE = ARMV5; // !< the processor architecture
	
	static const InsnSetVersion INSN_SET_VERSION = V5; // !< the supported instruction set version
	static const THUMBInsnSetVersion THUMB_INSN_SET_VERSION = V2; // !< the supported thumb instruction set
	
	static const uint32_t FSB_BURST_SIZE = 32; // !< Front side bus parameters
	
	static const bool HAS_LOAD_STORE_V4 = true;
	static const bool HAS_LOAD_STORE_V5E = true;
	static const bool HAS_INSN_BX = true;
	static const bool HAS_INSN_BLX = true;
	static const bool HAS_INSN_COPROCESSOR_ALTER_MOVES = true;
	static const bool HAS_INSN_COPROCESSOR_ALTER_LOADS = true;
	static const bool HAS_INSN_COPROCESSOR_ALTER_STORES = true;
	static const bool HAS_INSN_COPROCESSOR_ALTER_OPS = true;
	static const bool HAS_INSN_ARITH_CLZ = true;
	static const bool HAS_INSN_ARITH_QADD = false;
	static const bool HAS_INSN_ARITH_QDADD = false;
	static const bool HAS_INSN_ARITH_QSUB = false;
	static const bool HAS_INSN_ARITH_QDSUB = false;
	static const bool HAS_INSN_ARITH_MULT_UMULL = true;
	static const bool HAS_INSN_ARITH_MULT_UMLAL = true;
	static const bool HAS_INSN_ARITH_MULT_SMULL = true;
	static const bool HAS_INSN_ARITH_MULT_SMLAL = true;
	static const bool HAS_INSN_ARITH_MULT_SMULXY = false;
	static const bool HAS_INSN_ARITH_MULT_SMULWY = false;
	static const bool HAS_INSN_ARITH_MULT_SMLAXY = false;
	static const bool HAS_INSN_ARITH_MULT_SMLAWY = false;
	static const bool HAS_INSN_ARITH_MULT_SMLALXY = false;

};

/**
 * Describes the configuration of the ARM966E-S processor
 */
class ARM966E_S_Config :
	public ARMBase {
public:
	typedef uint32_t address_t;
	typedef uint32_t reg_t;
	typedef int32_t sreg_t;
	typedef uint32_t insn_t;                // instruction type
	typedef uint16_t thumb_insn_t;          // thumb instruction type
	
	static const Model MODEL = ARM966E_S;
	static const uint32_t PROCESSOR_VERSION = 0;
	static const Architecture ARCHITECTURE = ARMV5;
	
	static const InsnSetVersion INSN_SET_VERSION = V5; // !< the supported instruction set version
	static const THUMBInsnSetVersion THUMB_INSN_SET_VERSION = V2; // !< the supported thumb instruction set
	
	static const uint32_t FSB_BURST_SIZE = 32; // !< Front side bus parameters

	static const bool HAS_LOAD_STORE_V4 = true;
	static const bool HAS_LOAD_STORE_V5E = true;
	static const bool HAS_INSN_BX = true;
	static const bool HAS_INSN_BLX = true;
	static const bool HAS_INSN_COPROCESSOR_ALTER_MOVES = true;
	static const bool HAS_INSN_COPROCESSOR_ALTER_LOADS = true;
	static const bool HAS_INSN_COPROCESSOR_ALTER_STORES = true;
	static const bool HAS_INSN_COPROCESSOR_ALTER_OPS = true;
	static const bool HAS_COPROCESSOR_V5E = true;
	static const bool HAS_INSN_ARITH_CLZ = true;
	static const bool HAS_INSN_ARITH_QADD = true;
	static const bool HAS_INSN_ARITH_QDADD = true;
	static const bool HAS_INSN_ARITH_QSUB = true;
	static const bool HAS_INSN_ARITH_QDSUB = true;
	static const bool HAS_INSN_ARITH_MULT_UMULL = true;
	static const bool HAS_INSN_ARITH_MULT_UMLAL = true;
	static const bool HAS_INSN_ARITH_MULT_SMULL = true;
	static const bool HAS_INSN_ARITH_MULT_SMLAL = true;
	static const bool HAS_INSN_ARITH_MULT_SMULXY = true;
	static const bool HAS_INSN_ARITH_MULT_SMULWY = true;
	static const bool HAS_INSN_ARITH_MULT_SMLAXY = true;
	static const bool HAS_INSN_ARITH_MULT_SMLAWY = true;
	static const bool HAS_INSN_ARITH_MULT_SMLALXY = true;
	
	static const bool HAS_ITCM = true;
	static const bool HAS_DTCM = true;
	
	static const uint32_t ITCM_SIZE = (uint32_t)0x011;
	static const uint32_t DTCM_SIZE = (uint32_t)0x011;
};

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CONFIG_HH__
