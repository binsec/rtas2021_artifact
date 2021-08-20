
#ifndef __UNISIM_COMPONENT_CLM_PROCESSOR_OOOSIM_CONFIG_HH__
#define __UNISIM_COMPONENT_CLM_PROCESSOR_OOOSIM_CONFIG_HH__

#include <unisim/component/cxx/processor/powerpc/config.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

template <class CONFIG> class CPU;

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

namespace unisim {
namespace component {
namespace clm {
namespace processor {
namespace ooosim {

using unisim::component::cxx::processor::powerpc::MPC7447AConfig;

class CPUSim;

class MyPowerPcConfig : public MPC7447AConfig
{
public:
	typedef unisim::component::cxx::processor::powerpc::CPU<MyPowerPcConfig> STATE;
  //	static const Model MODEL = MyPowerPcConfig;
	
	static const uint32_t PROCESSOR_VERSION = 0x00000001UL;

	// Debug stuff
	static const bool DEBUG_ENABLE = true;
	static const bool DEBUG_STEP_ENABLE = true;
	static const bool DEBUG_DTLB_ENABLE = true;
	static const bool DEBUG_DL1_ENABLE = true;
	static const bool DEBUG_IL1_ENABLE = true;
	static const bool DEBUG_L2_ENABLE = true;
	static const bool DEBUG_LOAD_ENABLE = true;
	static const bool DEBUG_STORE_ENABLE = true;
	static const bool DEBUG_READ_MEMORY_ENABLE = true;
	static const bool DEBUG_WRITE_MEMORY_ENABLE = true;
	static const bool DEBUG_EXCEPTION_ENABLE = true;
	static const bool DEBUG_SET_MSR_ENABLE = true;
	static const bool DEBUG_SET_HID0_ENABLE = true;
	static const bool DEBUG_SET_HID1_ENABLE = true;
	static const bool DEBUG_SET_HID2_ENABLE = true;
	static const bool DEBUG_SET_L2CR_ENABLE = true;

	// L1 Data cache parameters
	class DL1_CONFIG
	{
	public:
		class BLOCK_STATUS
		{
		public:
			bool valid;
			bool dirty;
		};

		class LINE_STATUS
		{
		public:
			bool valid;
		};

		class SET_STATUS
		{
		public:
			uint32_t plru_bits;
		};

		class CACHE_STATUS
		{
		public:
		};

		static const bool ENABLE = false;
		typedef uint32_t ADDRESS;
		static const uint32_t CACHE_SIZE = 32 * 1024; //32 * 1024; // 32 KB
		static const uint32_t CACHE_BLOCK_SIZE = 32;   // 32 bytes
		static const uint32_t CACHE_LOG_ASSOCIATIVITY = 3; // 8-way set associative
		static const uint32_t CACHE_ASSOCIATIVITY = 1 << CACHE_LOG_ASSOCIATIVITY; // 8-way set associative
		static const uint32_t CACHE_LOG_BLOCKS_PER_LINE = 0; // 1 blocks per line
		static const uint32_t CACHE_BLOCKS_PER_LINE = 1 <<  CACHE_LOG_BLOCKS_PER_LINE; // 1 blocks per line
	};

	// L1 Instruction cache parameters
	class IL1_CONFIG
	{
	public:
		class BLOCK_STATUS
		{
		public:
			bool valid;
		};

		class LINE_STATUS
		{
		public:
			bool valid;
		};

		class SET_STATUS
		{
		public:
			uint32_t plru_bits;
		};

		class CACHE_STATUS
		{
		public:
		};

		static const bool ENABLE = false;
		typedef uint32_t ADDRESS;
		static const uint32_t CACHE_SIZE = 32 * 1024; // 32 KB
		static const uint32_t CACHE_BLOCK_SIZE = 32;   // 32 bytes
		static const uint32_t CACHE_LOG_ASSOCIATIVITY = 3; // 8-way set associative
		static const uint32_t CACHE_ASSOCIATIVITY = 1 << CACHE_LOG_ASSOCIATIVITY; // 8-way set associative
		static const uint32_t CACHE_LOG_BLOCKS_PER_LINE = 0; // 1 blocks per line
		static const uint32_t CACHE_BLOCKS_PER_LINE = 1 <<  CACHE_LOG_BLOCKS_PER_LINE; // 1 blocks per line
	};

	// L2 cache parameters
	class L2_CONFIG
	{
	public:
		class BLOCK_STATUS
		{
		public:
			bool valid;
			bool dirty;
		};

		class LINE_STATUS
		{
		public:
			bool valid;
		};

		class SET_STATUS
		{
		public:
			uint32_t plru_bits;
		};

		class CACHE_STATUS
		{
		public:
		};

		static const bool ENABLE = false;
		typedef uint32_t ADDRESS;
		static const uint32_t CACHE_SIZE = 512 * 1024; // 32 KB
		static const uint32_t CACHE_BLOCK_SIZE = 32;   // 32 bytes
		static const uint32_t CACHE_LOG_ASSOCIATIVITY = 3; // 8-way set associative
		static const uint32_t CACHE_ASSOCIATIVITY = 1 << CACHE_LOG_ASSOCIATIVITY; // 8-way set associative
		static const uint32_t CACHE_LOG_BLOCKS_PER_LINE = 1; // 2 blocks per line
		static const uint32_t CACHE_BLOCKS_PER_LINE = 1 <<  CACHE_LOG_BLOCKS_PER_LINE; // 1 blocks per line
	};

	// ITLB Parameters
	class ITLB_CONFIG
	{
	public:
		class ENTRY_STATUS
		{
		public:
			bool valid;
		};

		class SET_STATUS
		{
		public:
			uint32_t plru_bits;
		};

		static const bool ENABLE = false;
		typedef virtual_address_t VIRTUAL_ADDRESS;
		static const uint32_t TLB_NUM_ENTRIES = 128;
		static const uint32_t TLB_LOG_ASSOCIATIVITY = 1; // 2-way set associative
		static const uint32_t TLB_ASSOCIATIVITY = 1 << TLB_LOG_ASSOCIATIVITY;
		static const uint32_t PAGE_SIZE = MPC7447AConfig::MEMORY_PAGE_SIZE;
		typedef MPC7447AConfig::PTE PTE;
	};

	// DTLB Parameters
	class DTLB_CONFIG
	{
	public:
		class ENTRY_STATUS
		{
		public:
			bool valid;
		};

		class SET_STATUS
		{
		public:
			uint32_t plru_bits;
		};

		static const bool ENABLE = false;
		typedef virtual_address_t VIRTUAL_ADDRESS;
		static const uint32_t TLB_NUM_ENTRIES = 128;
		static const uint32_t TLB_LOG_ASSOCIATIVITY = 1; // 2-way set associative
		static const uint32_t TLB_ASSOCIATIVITY = 1 << TLB_LOG_ASSOCIATIVITY;
		static const uint32_t PAGE_SIZE = MPC7447AConfig::MEMORY_PAGE_SIZE;
		typedef MPC7447AConfig::PTE PTE;
	};

  

};

} // end of namespace ooosim
} // end of namespace processor
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
