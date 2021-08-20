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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_PPC440_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_PPC440_CPU_HH__

#include <unisim/component/cxx/processor/powerpc/book_e/ppc440/isa/ppc440.hh>
#include <unisim/component/cxx/processor/powerpc/book_e/cpu.hh>
#include <unisim/component/cxx/processor/powerpc/book_e/mmu.hh>
#include <unisim/component/cxx/processor/powerpc/book_e/l1i.hh>
#include <unisim/component/cxx/processor/powerpc/book_e/l1d.hh>
#include <unisim/service/interfaces/http_server.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace book_e {
namespace ppc440 {

class CPU; // forward declaration

// Memory sub-system types
struct TYPES
{
	// MMU storage attribute
	typedef enum
	{
		SA_U0      = 256,    // User defined #0
		SA_U1      = 128,    // User defined #1
		SA_U2      = 64,     // User defined #2
		SA_U3      = 32,     // User defined #3
		SA_W       = 16,     // Write-through
		SA_I       = 8,      // Cache inhibited
		SA_M       = 4,      // Memory coherency enforced
		SA_G       = 2,      // Guarded
		SA_E       = 1,      // Endian
		SA_DEFAULT = 0
	} STORAGE_ATTR_E;
	
	// MMU access control
	typedef enum
	{
		AC_UX      = 32,      // user execution permission
		AC_UW      = 16,      // user write permission
		AC_UR      = 8,       // user read permission
		AC_SX      = 4,       // supervisor execution permission
		AC_SW      = 2,       // supervisor write permission
		AC_SR      = 1,       // supervisor read permission
		AC_DEFAULT = 0
	} ACCESS_CTRL_E;

	typedef enum
	{
		AS_SYSTEM = 0,         // system address space
		AS_APPLICATION = 1     // application address space
	} ADDRESS_SPACE_E;         // address space

	typedef uint32_t EFFECTIVE_ADDRESS;   // 32-bit effective address
	typedef uint64_t ADDRESS;             // only 41 bits are used, all remaining bits *must* be set to zero
	typedef uint64_t PHYSICAL_ADDRESS;    // 36-bit physical address, only 36 bits are used, all remaining bits *must* be set to zero
	typedef uint8_t PROCESS_ID;           // 8-bit process ID
	typedef uint16_t STORAGE_ATTR;        // storage attribute: U0 U1 U2 U3 W I M G E
	typedef uint8_t ACCESS_CTRL;          // access control: UX UW UR SX SW SR
	typedef uint8_t ADDRESS_SPACE;        // address space: system or application
	
	struct VA
	{
		struct AS  : Field64<AS , 23>     {}; // Address Space
		struct PID : Field64<PID, 24, 31> {}; // Process ID
		struct EA  : Field64<EA , 32, 63> {}; // Effective Address
	};
};

struct CONFIG
{
	typedef unisim::component::cxx::processor::powerpc::book_e::ppc440::CPU CPU;

	// Model
	static const Model MODEL = PPC440;

	// Front-side bus width
	static const unsigned int FSB_WIDTH = 16; // front-side bus width
	static const unsigned int DATA_FSB_WIDTH = 16; // front-side bus width
	
	// Floating point
	static const bool HAS_FPU = true;
	
	// Altivec
	static const bool HAS_ALTIVEC = false;
	
	// Optional floating-point sqrt
	static const bool HAS_FLOATING_POINT_SQRT = true;
	
	// Optional floating-point graphics instructions
	static const bool HAS_FLOATING_POINT_GRAPHICS_INSTRUCTIONS = false;
	
	static const unsigned int NUM_EXCEPTIONS = 32;
	static const bool DEBUG_ENABLE = false;
	
	// Decoder and Operation
	typedef unisim::component::cxx::processor::powerpc::book_e::ppc440::isa::Decoder DECODER;
	typedef unisim::component::cxx::processor::powerpc::book_e::ppc440::isa::Operation OPERATION;
	typedef unisim::component::cxx::processor::powerpc::book_e::ppc440::isa::CodeType CODE_TYPE;
	
	// MMU
	struct MMU_CONFIG
	{
		typedef unisim::component::cxx::processor::powerpc::book_e::ppc440::CPU CPU;
		
		struct ITLB_CONFIG
		{
			static const unsigned int ASSOCIATIVITY = 4;
		};
		
		struct DTLB_CONFIG
		{
			static const unsigned int ASSOCIATIVITY = 8;
		};
		
		struct UTLB_CONFIG
		{
			static const unsigned int ASSOCIATIVITY = 64;
		};
	};
	
	// Level 1 Instruction Cache
	struct L1I_CONFIG
	{
		typedef unisim::component::cxx::processor::powerpc::book_e::ppc440::CPU CPU;
		static const unsigned int SIZE                                      = 32768;
		static const unisim::util::cache::CacheWritingPolicy WRITING_POLICY = unisim::util::cache::CACHE_WRITE_BACK_AND_WRITE_ALLOCATE_POLICY;
		static const unisim::util::cache::CacheIndexScheme INDEX_SCHEME     = unisim::util::cache::CACHE_PHYSICALLY_INDEXED;
		static const unisim::util::cache::CacheTagScheme TAG_SCHEME         = unisim::util::cache::CACHE_VIRTUALLY_TAGGED;
		static const unisim::util::cache::CacheType TYPE                    = unisim::util::cache::INSTRUCTION_CACHE;
		static const unsigned int ASSOCIATIVITY                             = 64;
		static const unsigned int BLOCK_SIZE                                = 32;
		static const unsigned int BLOCKS_PER_LINE                           = 1;
		
		struct SET_STATUS : unisim::util::cache::CacheSetStatus {};
		
		struct LINE_STATUS : unisim::util::cache::CacheLineStatus {};
		
		struct BLOCK_STATUS : unisim::util::cache::CacheBlockStatus {};
	};

	// Level 1 Data Cache
	struct L1D_CONFIG
	{
		typedef unisim::component::cxx::processor::powerpc::book_e::ppc440::CPU CPU;
		static const unsigned int SIZE                                      = 32768;
		static const unisim::util::cache::CacheWritingPolicy WRITING_POLICY = unisim::util::cache::CACHE_WRITE_THROUGH_AND_WRITE_ALLOCATE_POLICY;
		static const unisim::util::cache::CacheIndexScheme INDEX_SCHEME     = unisim::util::cache::CACHE_PHYSICALLY_INDEXED;
		static const unisim::util::cache::CacheTagScheme TAG_SCHEME         = unisim::util::cache::CACHE_PHYSICALLY_TAGGED;
		static const unisim::util::cache::CacheType TYPE                    = unisim::util::cache::DATA_CACHE;
		static const unsigned int ASSOCIATIVITY                             = 64;
		static const unsigned int BLOCK_SIZE                                = 32;
		static const unsigned int BLOCKS_PER_LINE                           = 1;
		
		struct SET_STATUS : unisim::util::cache::CacheSetStatus {};

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

class CPU : public unisim::component::cxx::processor::powerpc::book_e::CPU<TYPES, CONFIG>
{
public:
	typedef unisim::component::cxx::processor::powerpc::book_e::CPU<TYPES, CONFIG> SuperCPU;
	typedef typename TYPES::EFFECTIVE_ADDRESS EFFECTIVE_ADDRESS;
	typedef typename TYPES::ADDRESS ADDRESS;
	typedef TYPES::PROCESS_ID PROCESS_ID;
	typedef TYPES::ADDRESS_SPACE ADDRESS_SPACE;
	typedef SuperCPU::ICDBTRH ICDBTRH;
	typedef SuperCPU::ICDBTRL ICDBTRL;
	
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> itlb_http_server_export;
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> dtlb_http_server_export;
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> utlb_http_server_export;

	CPU(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~CPU();

	friend struct unisim::component::cxx::processor::powerpc::book_e::MMU<TYPES, CONFIG::MMU_CONFIG>;
	friend struct unisim::component::cxx::processor::powerpc::book_e::L1I<TYPES, CONFIG::L1I_CONFIG>;
	friend struct unisim::component::cxx::processor::powerpc::book_e::L1D<TYPES, CONFIG::L1D_CONFIG>;

	typedef unisim::component::cxx::processor::powerpc::book_e::MMU<TYPES, CONFIG::MMU_CONFIG> MMU;
	typedef unisim::component::cxx::processor::powerpc::book_e::L1I<TYPES, CONFIG::L1I_CONFIG> L1I;
	typedef unisim::component::cxx::processor::powerpc::book_e::L1D<TYPES, CONFIG::L1D_CONFIG> L1D;
	
	typedef unisim::util::cache::LocalMemorySet<TYPES> DATA_LOCAL_MEMORIES;
	typedef unisim::util::cache::LocalMemorySet<TYPES> INSTRUCTION_LOCAL_MEMORIES;
	typedef unisim::util::cache::CacheHierarchy<TYPES, L1D> DATA_CACHE_HIERARCHY;
	typedef unisim::util::cache::CacheHierarchy<TYPES, L1I> INSTRUCTION_CACHE_HIERARCHY;
	
	inline L1I *GetCache(const L1I *) ALWAYS_INLINE { return &l1i; }
	inline L1D *GetCache(const L1D *) ALWAYS_INLINE { return &l1d; }
	
	void ProcessInterrupt(CriticalInputInterrupt *);
	void ProcessInterrupt(MachineCheckInterrupt *);
	void ProcessInterrupt(DataStorageInterrupt *);
	void ProcessInterrupt(InstructionStorageInterrupt *);
	void ProcessInterrupt(ExternalInputInterrupt *);
	void ProcessInterrupt(AlignmentInterrupt *);
	void ProcessInterrupt(ProgramInterrupt *);
	void ProcessInterrupt(FloatingPointUnavailableInterrupt *);
	void ProcessInterrupt(SystemCallInterrupt *);
	void ProcessInterrupt(AuxiliaryProcessorUnavailableInterrupt *);
	void ProcessInterrupt(DecrementerInterrupt *);
	void ProcessInterrupt(FixedIntervalTimerInterrupt *);
	void ProcessInterrupt(WatchdogTimerInterrupt *);
	void ProcessInterrupt(DataTLBErrorInterrupt *);
	void ProcessInterrupt(InstructionTLBErrorInterrupt *);
	void ProcessInterrupt(DebugInterrupt *);
	
	bool Rfi();
	bool Rfci();
	bool Rfmci();
	
	bool Dcbi(EFFECTIVE_ADDRESS ea);
	bool Dcba(EFFECTIVE_ADDRESS ea);
	bool Dcbf(EFFECTIVE_ADDRESS ea);
	bool Dcbst(EFFECTIVE_ADDRESS ea);
	bool Dcbz(EFFECTIVE_ADDRESS ea);
	bool Dccci(EFFECTIVE_ADDRESS ea);
	bool Dcread(EFFECTIVE_ADDRESS ea, unsigned int rd);
	bool Icbi(EFFECTIVE_ADDRESS ea);
	bool Icbt(EFFECTIVE_ADDRESS ea);
	bool Iccci(EFFECTIVE_ADDRESS ea);
	bool Icread(EFFECTIVE_ADDRESS ea);
	
	bool Tlbre(unsigned int rd, uint32_t way, uint8_t ws);
	bool Tlbsx(unsigned int rd, typename TYPES::EFFECTIVE_ADDRESS ea, unsigned int rc);
	bool Tlbwe(uint32_t s, uint32_t way, uint8_t ws);
	
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
};

} // end of namespace ppc440
} // end of namespace book_e
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_PPC440_CPU_HH__
