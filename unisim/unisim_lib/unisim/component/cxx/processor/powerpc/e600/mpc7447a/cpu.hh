/*
 *  Copyright (c) 2017,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_book_e_PPC440_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_book_e_PPC440_CPU_HH__

#include <unisim/component/cxx/processor/powerpc/e600/mpc7447a/isa/mpc7447a.hh>
#include <unisim/component/cxx/processor/powerpc/e600/cpu.hh>
#include <unisim/component/cxx/processor/powerpc/e600/mmu.hh>
#include <unisim/component/cxx/processor/powerpc/e600/l1i.hh>
#include <unisim/component/cxx/processor/powerpc/e600/l1d.hh>
#include <unisim/component/cxx/processor/powerpc/e600/l2u.hh>
#include <unisim/service/interfaces/http_server.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e600 {
namespace mpc7447a {

class CPU; // forward declaration

// Memory sub-system types
struct TYPES
{
	// MMU storage attribute
	typedef enum
	{
		SA_W       = 8, // Write-through
		SA_I       = 4, // Cache inhibited
		SA_M       = 2, // Memory coherency enforced
		SA_G       = 1, // Guarded
		SA_DEFAULT = 0
	} STORAGE_ATTR_E;
	
	typedef uint32_t EFFECTIVE_ADDRESS;   // 32-bit effective address
	typedef uint64_t ADDRESS;             // 52-bit virtual address, only 52 bits are used, all remaining bits *must* be set to zero
	typedef uint64_t PHYSICAL_ADDRESS;    // 36-bit physical address, only 36 bits are used, all remaining bits *must* be set to zero
	typedef uint8_t  STORAGE_ATTR;        // storage attribute: W I M G
	typedef uint32_t VIRTUAL_SEGMENT_ID;
	typedef uint32_t PAGE_INDEX;
	typedef uint32_t ABBREVIATED_PAGE_INDEX;
	
	struct EA
	{
		struct SR         : Field<SR        , 0, 3 > {}; // SR#
		struct PAGE_INDEX : Field<PAGE_INDEX, 4, 19> {}; // Page index
		struct API        : Field<API       , 4, 9 > {}; // Abbreviated Page Index
		struct BEPI_0_3   : Field<BEPI_0_3  , 0, 3>  {}; // Bits 0-3 of Block Effective Page Index
		struct BEPI_4_14  : Field<BEPI_4_14 , 4, 14> {}; // Bits 4-14 of Block Effective Page Index
		struct _0_30      : Field<_0_30     , 0, 30> {}; // Bits 0-30
	};
};

struct CONFIG
{
	typedef unisim::component::cxx::processor::powerpc::e600::mpc7447a::CPU CPU;

	// Model
	static const Model MODEL = MPC7447A;

	// Front-side bus width
	static const unsigned int FSB_WIDTH = 8;              // front-side bus width
	static const unsigned int DATA_FSB_WIDTH = FSB_WIDTH; // front-side bus width
	static const unsigned int FSB_BURST_SIZE = 32;
	
	// Floating point
	static const bool HAS_FPU = true;
	
	// Altivec
	static const bool HAS_ALTIVEC = true;
	
	// Optional floating-point sqrt
	static const bool HAS_FLOATING_POINT_SQRT = false;
	
	// Optional floating-point graphics instructions
	static const bool HAS_FLOATING_POINT_GRAPHICS_INSTRUCTIONS = true;
	
	static const unsigned int NUM_EXCEPTIONS = 34;
	static const bool DEBUG_ENABLE = false;
	
	// Decoder and Operation
	typedef unisim::component::cxx::processor::powerpc::e600::mpc7447a::isa::Decoder DECODER;
	typedef unisim::component::cxx::processor::powerpc::e600::mpc7447a::isa::Operation OPERATION;
	typedef unisim::component::cxx::processor::powerpc::e600::mpc7447a::isa::CodeType CODE_TYPE;
	
	// MMU
	struct MMU_CONFIG
	{
		typedef unisim::component::cxx::processor::powerpc::e600::mpc7447a::CPU CPU;
		
		struct ITLB_CONFIG
		{
			static const unsigned int SIZE          = 128;
			static const unsigned int ASSOCIATIVITY = 2;
		};
		
		struct DTLB_CONFIG
		{
			static const unsigned int SIZE          = 128;
			static const unsigned int ASSOCIATIVITY = 2;
		};
	};
	
	// Level 1 Instruction Cache
	struct L1I_CONFIG
	{
		typedef unisim::component::cxx::processor::powerpc::e600::mpc7447a::CPU CPU;
		static const unsigned int SIZE                                      = 32768;
		static const unisim::util::cache::CacheWritingPolicy WRITING_POLICY = unisim::util::cache::CACHE_WRITE_BACK_AND_WRITE_ALLOCATE_POLICY;
		static const unisim::util::cache::CacheIndexScheme INDEX_SCHEME     = unisim::util::cache::CACHE_PHYSICALLY_INDEXED;
		static const unisim::util::cache::CacheTagScheme TAG_SCHEME         = unisim::util::cache::CACHE_PHYSICALLY_TAGGED;
		static const unisim::util::cache::CacheType TYPE                    = unisim::util::cache::INSTRUCTION_CACHE;
		static const unsigned int ASSOCIATIVITY                             = 8;
		static const unsigned int BLOCK_SIZE                                = 32;
		static const unsigned int BLOCKS_PER_LINE                           = 1;
		
		struct SET_STATUS : unisim::util::cache::CacheSetStatus
		{
			unisim::util::cache::PLRU_ReplacementPolicy<ASSOCIATIVITY> plru;
		};
		
		struct LINE_STATUS : unisim::util::cache::CacheLineStatus {};
		
		struct BLOCK_STATUS : unisim::util::cache::CacheBlockStatus {};
	};

	// Level 1 Data Cache
	struct L1D_CONFIG
	{
		typedef unisim::component::cxx::processor::powerpc::e600::mpc7447a::CPU CPU;
		static const unsigned int SIZE                                      = 32768;
		static const unisim::util::cache::CacheWritingPolicy WRITING_POLICY = unisim::util::cache::CACHE_WRITE_BACK_AND_WRITE_ALLOCATE_POLICY;
		static const unisim::util::cache::CacheIndexScheme INDEX_SCHEME     = unisim::util::cache::CACHE_PHYSICALLY_INDEXED;
		static const unisim::util::cache::CacheTagScheme TAG_SCHEME         = unisim::util::cache::CACHE_PHYSICALLY_TAGGED;
		static const unisim::util::cache::CacheType TYPE                    = unisim::util::cache::DATA_CACHE;
		static const unsigned int ASSOCIATIVITY                             = 8;
		static const unsigned int BLOCK_SIZE                                = 32;
		static const unsigned int BLOCKS_PER_LINE                           = 1;
		
		struct SET_STATUS : unisim::util::cache::CacheSetStatus
		{
			unisim::util::cache::PLRU_ReplacementPolicy<ASSOCIATIVITY> plru;
		};

		struct LINE_STATUS : unisim::util::cache::CacheLineStatus
		{
			TYPES::STORAGE_ATTR storage_attr;
			
			LINE_STATUS() : unisim::util::cache::CacheLineStatus(), storage_attr(TYPES::SA_DEFAULT) {}
			
			inline void Invalidate() ALWAYS_INLINE
			{
				storage_attr = TYPES::SA_DEFAULT;
			}
		};
		
		struct BLOCK_STATUS : unisim::util::cache::CacheBlockStatus {};
	};

	// Level 2 Unified Cache
	struct L2U_CONFIG
	{
		typedef unisim::component::cxx::processor::powerpc::e600::mpc7447a::CPU CPU;
		static const unsigned int SIZE                                      = 512 * 1024;
		static const unisim::util::cache::CacheWritingPolicy WRITING_POLICY = unisim::util::cache::CACHE_WRITE_BACK_AND_WRITE_ALLOCATE_POLICY;
		static const unisim::util::cache::CacheIndexScheme INDEX_SCHEME     = unisim::util::cache::CACHE_PHYSICALLY_INDEXED;
		static const unisim::util::cache::CacheTagScheme TAG_SCHEME         = unisim::util::cache::CACHE_PHYSICALLY_TAGGED;
		static const unisim::util::cache::CacheType TYPE                    = unisim::util::cache::UNIFIED_CACHE;
		static const unsigned int ASSOCIATIVITY                             = 8;
		static const unsigned int BLOCK_SIZE                                = 32;
		static const unsigned int BLOCKS_PER_LINE                           = 2;
		
		struct SET_STATUS : unisim::util::cache::CacheSetStatus
		{
			unisim::util::cache::PLRU_ReplacementPolicy<ASSOCIATIVITY> plru;
		};

		struct LINE_STATUS : unisim::util::cache::CacheLineStatus
		{
			TYPES::STORAGE_ATTR storage_attr;
			
			LINE_STATUS() : unisim::util::cache::CacheLineStatus(), storage_attr(TYPES::SA_DEFAULT) {}
			
			inline void Invalidate() ALWAYS_INLINE
			{
				storage_attr = TYPES::SA_DEFAULT;
			}
		};
		
		struct BLOCK_STATUS : unisim::util::cache::CacheBlockStatus {};
	};
	
	// Instruction Buffer
	static const unsigned int INSTRUCTION_BUFFER_SIZE = L1I_CONFIG::BLOCK_SIZE;
};

class CPU : public unisim::component::cxx::processor::powerpc::e600::CPU<TYPES, CONFIG>
{
public:
	typedef unisim::component::cxx::processor::powerpc::e600::CPU<TYPES, CONFIG> SuperCPU;
	typedef typename TYPES::EFFECTIVE_ADDRESS EFFECTIVE_ADDRESS;
	typedef typename TYPES::ADDRESS ADDRESS;
	static const unsigned int FSB_WIDTH = CONFIG::FSB_WIDTH;
	
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> itlb_http_server_export;
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> dtlb_http_server_export;

	CPU(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~CPU();

	friend struct unisim::component::cxx::processor::powerpc::e600::MMU<TYPES, CONFIG::MMU_CONFIG>;
	friend struct unisim::component::cxx::processor::powerpc::e600::L1I<TYPES, CONFIG::L1I_CONFIG>;
	friend struct unisim::component::cxx::processor::powerpc::e600::L1D<TYPES, CONFIG::L1D_CONFIG>;
	friend struct unisim::component::cxx::processor::powerpc::e600::L2U<TYPES, CONFIG::L2U_CONFIG>;

	typedef unisim::component::cxx::processor::powerpc::e600::MMU<TYPES, CONFIG::MMU_CONFIG> MMU;
	typedef unisim::component::cxx::processor::powerpc::e600::L1I<TYPES, CONFIG::L1I_CONFIG> L1I;
	typedef unisim::component::cxx::processor::powerpc::e600::L1D<TYPES, CONFIG::L1D_CONFIG> L1D;
	typedef unisim::component::cxx::processor::powerpc::e600::L2U<TYPES, CONFIG::L2U_CONFIG> L2U;
	
	typedef unisim::util::cache::LocalMemorySet<TYPES> DATA_LOCAL_MEMORIES;
	typedef unisim::util::cache::LocalMemorySet<TYPES> INSTRUCTION_LOCAL_MEMORIES;
	typedef unisim::util::cache::CacheHierarchy<TYPES, L1D, L2U> DATA_CACHE_HIERARCHY;
	typedef unisim::util::cache::CacheHierarchy<TYPES, L1I, L2U> INSTRUCTION_CACHE_HIERARCHY;
	
	inline L1I *GetCache(const L1I *) ALWAYS_INLINE { return &l1i; }
	inline L1D *GetCache(const L1D *) ALWAYS_INLINE { return &l1d; }
	inline L2U *GetCache(const L2U *) ALWAYS_INLINE { return &l2u; }

	bool Dcba(EFFECTIVE_ADDRESS ea);
	bool Dcbf(EFFECTIVE_ADDRESS ea);
	bool Dcbi(EFFECTIVE_ADDRESS ea);
	bool Dcbst(EFFECTIVE_ADDRESS ea);
	bool Dcbz(EFFECTIVE_ADDRESS ea);
	bool Icbi(EFFECTIVE_ADDRESS ea);
	bool Icbt(EFFECTIVE_ADDRESS ea);
	
	bool Tlbia();
	bool Tlbie(EFFECTIVE_ADDRESS ea);
	bool Tlbld(EFFECTIVE_ADDRESS ea);
	bool Tlbli(EFFECTIVE_ADDRESS ea);

	uint32_t GetSR(unsigned int n) const { return mmu.GetSR(n); }
	void SetSR(unsigned int n, uint32_t value) { mmu.SetSR(n, value); }

	template <bool DEBUG, bool EXEC, bool WRITE>
	inline bool Translate(EFFECTIVE_ADDRESS ea, EFFECTIVE_ADDRESS& size_to_protection_boundary, ADDRESS& virt_addr, PHYSICAL_ADDRESS& phys_addr, STORAGE_ATTR& storage_attr)
	{
		return mmu.template Translate<DEBUG, EXEC, WRITE>(ea, size_to_protection_boundary, virt_addr, phys_addr, storage_attr);
	}
	
protected:
	///////////////////////// Memory Protection Unit //////////////////////////

	MMU mmu;

	/////////////////////////// Memory sub-system /////////////////////////////
	
	L1I l1i;                     // L1 instruction cache
	L1D l1d;                     // L1 data cache
	L2U l2u;                     // L2 unified cache
};

} // end of namespace mpc7447a
} // end of namespace e600
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_book_e_PPC440_CPU_HH__
