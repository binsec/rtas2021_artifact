/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */

#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_CONFIG_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_CONFIG_HH

#include <unisim/component/cxx/processor/tesla/simfloat.hh>
#include <unisim/component/cxx/processor/tesla/hostfloat/hostfloat.hh>
#include <unisim/component/cxx/processor/tesla/stats.hh>
#include <unisim/component/cxx/processor/tesla/vectorfp32.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template<class CONFIG>
struct ImplicitFlow;

class BaseConfig
{
public:
	typedef uint32_t address_t;             // 32-bit effective address
	typedef uint64_t virtual_address_t;     // only 32 bits (G80) or 40 bits (GT200) are used, all remaining bits *must* be set to zero
	typedef uint32_t physical_address_t;    // 32-bit physical address
	typedef uint32_t reg_t;                 // register type
	typedef uint64_t insn_t;				// instruction word
//	static const uint32_t MEMORY_PAGE_SIZE = 4096;
	
//	typedef FloatDAZFTZ<SoftFloatIEEE> float_t;
	typedef hostfloat::TeslaBinary32 float_t;
	typedef FloatDAZFTZ<SoftHalfIEEE> half_t;
	
	typedef VectorFP32<BaseConfig> vfp32;
	
	typedef Stats<BaseConfig> stats_t;
	typedef OperationStats<BaseConfig> operationstats_t;
	
	/* A Memory Page Table Entry */
	class PTE
	{
	public:
		physical_address_t base_physical_addr;  /*< 32-bit base physical address */
		uint32_t c;                             /*< 1-bit C field */
//		WIMG wimg;                              /*< 4-bit WIMG field */
		uint32_t pp;                            /*< 2-bit page protection field */
	};

//	typedef ImplicitFlow<BaseConfig> control_flow_t;
	typedef TeslaFlow<BaseConfig> control_flow_t;

	static uint32_t const WARP_SIZE = 32;
	static uint32_t const MAX_WARPS_PER_BLOCK = 16;
	static uint32_t const MAX_WARPS = 24;
	static uint32_t const MAX_VGPR = 256;
	static uint32_t const MAX_CTAS = 8;
	static uint32_t const MAX_THREADS = MAX_WARPS * WARP_SIZE;
	static uint32_t const STACK_DEPTH = 100;	// Random value
//	static uint32_t const BRANCH_STACK_DEPTH = 100;	// Random value
//	static uint32_t const CALL_STACK_DEPTH = 17;	// 17 = Smallest random-looking value
	static uint32_t const MAX_ADDR_REGS = 4;
	static uint32_t const MAX_PRED_REGS = 4;
	static uint32_t const MAX_SAMPLERS = 16;
	
	static uint32_t const TRANSACTION_SIZE = 16;
	static uint32_t const TRANSACTIONS_PER_WARP = WARP_SIZE / TRANSACTION_SIZE;

	// Memory layout
	static address_t const CODE_START = 0x10000000;

	// 16 * 64 K
	static address_t const CONST_START = 0x20000000;	// -> 0x20100000
	static uint32_t const CONST_SEG_SIZE = 64 * 1024;
	static uint32_t const CONST_SEG_NUM = 16;

	static address_t const SHARED_START = 0x20100000;	// -> 0x20104000
	static uint32_t const SHARED_SIZE = 16 * 1024;
	
	static address_t const LOCAL_START = 0x30000000;
	static address_t const GLOBAL_START = 0x40000000;
	static address_t const GLOBAL_RELOC_START = 0x80000000;

	static size_t const LOCAL_SIZE = GLOBAL_START - LOCAL_START;
	static size_t const GLOBAL_SIZE = GLOBAL_RELOC_START - GLOBAL_START;

	
	static int const COMPUTE_CAP_MAJOR = 1;
	static int const COMPUTE_CAP_MINOR = 3;
	static int const SHADER_CLOCK_KHZ = 1350000;
	static int const CORE_COUNT = 1;
	
	static bool const CAP_GLOBALATOMICS = false;
	static bool const CAP_SMLOCKS = false;
	static bool const CAP_DP = true;

	// Default values, can be overriden at run-time
	static bool const TRACE_INSN = false;
	static bool const TRACE_MASK = false;
	static bool const TRACE_REG = false;
	static bool const TRACE_REG_FLOAT = false;
	static bool const TRACE_LOADSTORE = false;
	static bool const TRACE_BRANCH = false;
	static bool const TRACE_SYNC = false;
	static bool const TRACE_RESET = false;

	static bool const STAT_SCALAR_REG = false;
	static bool const STAT_STRIDED_REG = false;
	static bool const STAT_SIMTIME = false;
	static bool const STAT_FPDOMAIN = false;
	
	static bool const DEBUG_NONAN = false;
};


} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
