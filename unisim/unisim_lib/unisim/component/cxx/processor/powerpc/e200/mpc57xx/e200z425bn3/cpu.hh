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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_E200Z425BN3_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_E200Z425BN3_HH__

#include <unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.hh>
#include <unisim/component/cxx/processor/powerpc/e200/mpc57xx/cpu.hh>
#include <unisim/component/cxx/processor/powerpc/e200/mpc57xx/mpu.hh>
#include <unisim/component/cxx/processor/powerpc/e200/mpc57xx/imem.hh>
#include <unisim/component/cxx/processor/powerpc/e200/mpc57xx/dmem.hh>
#include <unisim/component/cxx/processor/powerpc/e200/mpc57xx/l1i.hh>
#include <unisim/service/interfaces/http_server.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e200 {
namespace mpc57xx {
namespace e200z425bn3 {

class CPU; // forward declaration

// Memory sub-system types
struct TYPES
{
	typedef enum
	{
		SA_DEFAULT = 0,           // not cache inhibited and not guarded
		SA_I       = 1,           // cache inhibited
		SA_G       = 2,           // guarded
		SA_IG      = SA_I | SA_G, // cache inhibited and guarded
	} STORAGE_ATTR_E;

	typedef uint8_t STORAGE_ATTR;
	typedef uint32_t EFFECTIVE_ADDRESS;
	typedef uint32_t ADDRESS;
	typedef uint32_t PHYSICAL_ADDRESS;
};

struct CONFIG
{
	typedef unisim::component::cxx::processor::powerpc::e200::mpc57xx::e200z425bn3::CPU CPU;

	// Model
	static const Model MODEL = E200Z425BN3;

	// Floating point
	static const bool HAS_FPU = false;

	// Altivec
	static const bool HAS_ALTIVEC = false;
	
	// Front-side bus width
	static const unsigned int DATA_FSB_WIDTH = 4; // Data load/store front-side bus width
	static const unsigned int INSN_FSB_WIDTH = 8; // Instruction fetch front-side bus width
	static const unsigned int INCO_FSB_WIDTH = 4; // Incomming Data load/store front-side bus width
	
	static const unsigned int NUM_EXCEPTIONS = 32;
	static const bool DEBUG_ENABLE = false;
	
	// VLE Decoder and Operation
	typedef unisim::component::cxx::processor::powerpc::e200::mpc57xx::e200z425bn3::isa::vle::Decoder VLE_DECODER;
	typedef unisim::component::cxx::processor::powerpc::e200::mpc57xx::e200z425bn3::isa::vle::Operation VLE_OPERATION;
	typedef unisim::component::cxx::processor::powerpc::e200::mpc57xx::e200z425bn3::isa::vle::CodeType VLE_CODE_TYPE;
	
	// MPU
	struct MPU_CONFIG
	{
		typedef unisim::component::cxx::processor::powerpc::e200::mpc57xx::e200z425bn3::CPU CPU;
		static const unsigned int NUM_INST_MPU_ENTRIES = 6;
		static const unsigned int NUM_DATA_MPU_ENTRIES = 12;
		static const unsigned int NUM_SHARED_MPU_ENTRIES = 6;
	};
	
	// I-MEM
	struct IMEM_CONFIG
	{
		typedef unisim::component::cxx::processor::powerpc::e200::mpc57xx::e200z425bn3::CPU CPU;
		static const unisim::util::cache::LocalMemoryType TYPE = unisim::util::cache::INSTRUCTION_LOC_MEM;
		static const TYPES::PHYSICAL_ADDRESS BASE_ADDRESS = 0; // unused
		static const unsigned int SIZE = 65536; // max size
	};

	// D-MEM
	struct DMEM_CONFIG
	{
		typedef unisim::component::cxx::processor::powerpc::e200::mpc57xx::e200z425bn3::CPU CPU;
		static const unisim::util::cache::LocalMemoryType TYPE = unisim::util::cache::DATA_LOC_MEM;
		static const TYPES::PHYSICAL_ADDRESS BASE_ADDRESS = 0; // unused
		static const unsigned int SIZE = 65536; // max size
	};

	// Level 1 Instruction Cache
	struct L1I_CONFIG
	{
		typedef unisim::component::cxx::processor::powerpc::e200::mpc57xx::e200z425bn3::CPU CPU;
		static const unsigned int SIZE                                      = 8192;
		static const unisim::util::cache::CacheWritingPolicy WRITING_POLICY = unisim::util::cache::CACHE_WRITE_THROUGH_AND_NO_WRITE_ALLOCATE_POLICY;
		static const unisim::util::cache::CacheIndexScheme INDEX_SCHEME     = unisim::util::cache::CACHE_PHYSICALLY_INDEXED;
		static const unisim::util::cache::CacheTagScheme TAG_SCHEME         = unisim::util::cache::CACHE_PHYSICALLY_TAGGED;
		static const unisim::util::cache::CacheType TYPE                    = unisim::util::cache::INSTRUCTION_CACHE;
		static const unsigned int ASSOCIATIVITY                             = 2;
		static const unsigned int BLOCK_SIZE                                = 32;
		static const unsigned int BLOCKS_PER_LINE                           = 1;
		
		struct SET_STATUS : unisim::util::cache::CacheSetStatus
		{
			SET_STATUS() : victim_way(0), lockout(0) {}
			
			unsigned int victim_way;
			unsigned int lockout;
		};
		
		struct LINE_STATUS : unisim::util::cache::CacheLineStatus {};
		
		struct BLOCK_STATUS : unisim::util::cache::CacheBlockStatus {};
	};

	// Instruction Buffer
	static const unsigned int INSTRUCTION_BUFFER_SIZE = L1I_CONFIG::BLOCK_SIZE;
};

class CPU : public unisim::component::cxx::processor::powerpc::e200::mpc57xx::CPU<TYPES, CONFIG>
{
public:
	typedef unisim::component::cxx::processor::powerpc::e200::mpc57xx::CPU<TYPES, CONFIG> SuperCPU;
	
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> mpu_http_server_export;
	
	CPU(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~CPU();

	friend struct unisim::component::cxx::processor::powerpc::e200::mpc57xx::MPU<TYPES, CONFIG::MPU_CONFIG>;
	friend struct unisim::component::cxx::processor::powerpc::e200::mpc57xx::IMEM<TYPES, CONFIG::IMEM_CONFIG>;
	friend struct unisim::component::cxx::processor::powerpc::e200::mpc57xx::DMEM<TYPES, CONFIG::DMEM_CONFIG>;
	friend struct unisim::component::cxx::processor::powerpc::e200::mpc57xx::L1I<TYPES, CONFIG::L1I_CONFIG>;

	typedef unisim::component::cxx::processor::powerpc::e200::mpc57xx::MPU<TYPES, CONFIG::MPU_CONFIG> MPU;
	typedef unisim::component::cxx::processor::powerpc::e200::mpc57xx::IMEM<TYPES, CONFIG::IMEM_CONFIG> IMEM;
	typedef unisim::component::cxx::processor::powerpc::e200::mpc57xx::DMEM<TYPES, CONFIG::DMEM_CONFIG> DMEM;
	typedef unisim::component::cxx::processor::powerpc::e200::mpc57xx::L1I<TYPES, CONFIG::L1I_CONFIG> L1I;
	
	typedef unisim::util::cache::LocalMemorySet<TYPES, DMEM> DATA_LOCAL_MEMORIES;
	typedef unisim::util::cache::LocalMemorySet<TYPES, IMEM> INSTRUCTION_LOCAL_MEMORIES;
	typedef unisim::util::cache::CacheHierarchy<TYPES> DATA_CACHE_HIERARCHY;
	typedef unisim::util::cache::CacheHierarchy<TYPES, L1I> INSTRUCTION_CACHE_HIERARCHY;
	
	inline IMEM *GetLocalMemory(const IMEM *) ALWAYS_INLINE { return &imem; }
	inline DMEM *GetLocalMemory(const DMEM *) ALWAYS_INLINE { return &dmem; }
	inline L1I *GetCache(const L1I *) ALWAYS_INLINE { return &l1i; }
	
	bool Dcba(EFFECTIVE_ADDRESS addr);
	bool Dcbi(EFFECTIVE_ADDRESS addr);
	bool Dcbf(EFFECTIVE_ADDRESS addr);
	bool Dcbst(EFFECTIVE_ADDRESS addr);
	bool Dcbt(EFFECTIVE_ADDRESS addr);
	bool Dcbtst(EFFECTIVE_ADDRESS addr);
	bool Dcbz(EFFECTIVE_ADDRESS addr);
	bool Icbi(EFFECTIVE_ADDRESS addr);
	bool Icbt(EFFECTIVE_ADDRESS addr);
	bool Mpure();
	bool Mpuwe();
	bool Mpusync();

	template <bool DEBUG, bool EXEC, bool WRITE>
	inline bool Translate(EFFECTIVE_ADDRESS ea, EFFECTIVE_ADDRESS& size_to_protection_boundary, ADDRESS& virt_addr, PHYSICAL_ADDRESS& phys_addr, STORAGE_ATTR& storage_attr)
	{
		if(unlikely((!mpu.template ControlAccess<DEBUG, EXEC, WRITE>(ea, size_to_protection_boundary, storage_attr)))) return false;

		virt_addr = ea;
		phys_addr = ea;
		return true;
	}
	
	////////////////////// Special Purpose Registers //////////////////////////
	
	// L1 Cache Configuration Register 0
	struct L1CFG0 : SuperCPU::L1CFG0
	{
		typedef SuperCPU::L1CFG0 Super;
		
		L1CFG0(CPU *_cpu) : Super(_cpu) {}
		L1CFG0(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual void Reset()
		{
			Set<CARCH>(CARCH_INSTRUCTION_ONLY);
			Set<CWPA>(0);
			Set<DCFAHA>(0);
			Set<DCFISWA>(0);
			Set<DCBSIZE>(0);
			Set<DCREPL>(0);
			Set<DCLA>(0);
			Set<DCECA>(0);
			Set<DCNWAY>(0);
			Set<DCSIZE>(0);
		}
	};
	
	// L1 Cache Configuration Register 1
	struct L1CFG1 : SuperCPU::L1CFG1
	{
		typedef SuperCPU::L1CFG1 Super;
		
		L1CFG1(CPU *_cpu) : Super(_cpu) {}
		L1CFG1(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual void Reset()
		{
			Set<ICFISWA>(1);  // The instruction cache supports invalidation by Set and Way via L1FINV1
			Set<ICBSIZE>(CEIL_LOG2<L1I::BLOCK_SIZE>::VALUE - 5);  // The instruction cache implements a block size of 32 bytes
			Set<ICREPL>(CREPL_FIFO);   // The instruction cache implements a FIFO replacement policy
			Set<ICLA>(0);     // The instruction cache does not implement the line locking APU
			Set<ICECA>(1);    // The instruction cache implements error checking
			Set<ICNWAY>(L1I::ASSOCIATIVITY - 1);   // The instruction cache is 2-way set-associative
			Set<ICSIZE>(L1I::SIZE / 1024);         // The size of the instruction cache is 16 KB
		}
	};
	
	// L1 Cache Control and Status Register 0
	typedef SuperCPU::L1CSR0 L1CSR0;
	
protected:
	
	///////////////////////// Memory Protection Unit //////////////////////////

	MPU mpu;

	/////////////////////////// Memory sub-system /////////////////////////////
	
	L1I l1i;                     // L1 instruction cache
	
	IMEM imem;                   // I-MEM
	DMEM dmem;                   // D-MEM
	
	////////////////////// Special Purpose Registers //////////////////////////
	
	L1CFG0 l1cfg0;
	L1CFG1 l1cfg1;
	L1CSR0 l1csr0;
};

} // end of namespace e200z425bn3
} // end of namespace mpc57xx
} // end of namespace e200
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_E200Z425BN3_HH__
