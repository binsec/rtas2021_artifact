#include <unisim/util/cache/cache.hh>
#include <inttypes.h>
#include <iostream>
#include <map>

namespace sandbox {

using unisim::util::cache::AccessController;
using unisim::util::cache::AccessControl;
using unisim::util::cache::LocalMemoryType;
using unisim::util::cache::DATA_LOC_MEM;
using unisim::util::cache::INSTRUCTION_LOC_MEM;
using unisim::util::cache::LocalMemory;
using unisim::util::cache::LocalMemorySet;
using unisim::util::cache::CacheSetStatus;
using unisim::util::cache::CacheLineStatus;
using unisim::util::cache::CacheBlockStatus;
using unisim::util::cache::MemorySubSystem;
using unisim::util::cache::Cache;
using unisim::util::cache::CacheType;
using unisim::util::cache::DATA_CACHE;
using unisim::util::cache::INSTRUCTION_CACHE;
using unisim::util::cache::UNIFIED_CACHE;
using unisim::util::cache::CacheWritingPolicy;
using unisim::util::cache::CACHE_WRITE_THROUGH_AND_NO_WRITE_ALLOCATE_POLICY;
using unisim::util::cache::CACHE_WRITE_BACK_AND_NO_WRITE_ALLOCATE_POLICY;
using unisim::util::cache::CACHE_WRITE_THROUGH_AND_WRITE_ALLOCATE_POLICY;
using unisim::util::cache::CACHE_WRITE_BACK_AND_WRITE_ALLOCATE_POLICY;
using unisim::util::cache::CacheHierarchy;
using unisim::util::cache::CacheAccess;
using unisim::util::cache::PLRU_ReplacementPolicy;
using unisim::util::cache::LRU_ReplacementPolicy;
using unisim::util::cache::RoundRobinReplacementPolicy;
using unisim::util::cache::PseudoRandomReplacementPolicy;
	
struct MEMORY_SUB_SYSTEM_TYPES
{
	typedef uint32_t ADDRESS;
	typedef uint32_t PHYSICAL_ADDRESS;
	typedef uint32_t STORAGE_ATTR;
};

template <typename MSS_TYPES>
struct MyMemorySubSystem : MemorySubSystem<MSS_TYPES, MyMemorySubSystem<MSS_TYPES> >
{
	typedef MemorySubSystem<MSS_TYPES, MyMemorySubSystem> Super;
	typedef MyMemorySubSystem<MSS_TYPES> ThisMemorySubSystem;
	
	struct MPU : AccessController<MSS_TYPES, MPU>
	{
		template <bool DEBUG> bool ControlAccess(AccessControl<MSS_TYPES>& access_control)
		{
			access_control.size_to_protection_boundary = 0xffffffffUL;
			access_control.phys_addr = access_control.addr;
			access_control.storage_attr = 0;
			return true;
		}
	};
	
	struct IMEM_CONFIG
	{
		static const LocalMemoryType TYPE = INSTRUCTION_LOC_MEM;
		static const typename MSS_TYPES::ADDRESS BASE_ADDRESS = 0x52000000;
		static const uint32_t SIZE = 16384;
		
	};
	
	struct IMEM : LocalMemory<MSS_TYPES, IMEM_CONFIG, IMEM>
	{
		bool IsVerbose() const ALWAYS_INLINE { return true; }
		static const char *GetLocalMemoryName() { return "IMEM"; }
	};

	struct DMEM_CONFIG
	{
		static const LocalMemoryType TYPE = DATA_LOC_MEM;
		static const typename MSS_TYPES::ADDRESS BASE_ADDRESS = 0x50000000;
		static const uint32_t SIZE = 65536;
	};
	
	struct DMEM : LocalMemory<MSS_TYPES, DMEM_CONFIG, DMEM>
	{
		bool IsVerbose() const ALWAYS_INLINE { return true; }
		static const char *GetLocalMemoryName() { return "DMEM"; }
	};

	struct L1I_CONFIG
	{
		static const uint32_t SIZE                     = 32768;
		static const CacheWritingPolicy WRITING_POLICY = CACHE_WRITE_BACK_AND_WRITE_ALLOCATE_POLICY;
		static const CacheType TYPE                    = INSTRUCTION_CACHE;
		static const unsigned int ASSOCIATIVITY        = 8;
		static const unsigned int BLOCK_SIZE           = 32;
		static const unsigned int BLOCKS_PER_LINE      = 1;
		
		struct SET_STATUS : CacheSetStatus
		{
			LRU_ReplacementPolicy<ASSOCIATIVITY> lru;
		};
		
		struct LINE_STATUS : CacheLineStatus {};
		
		struct BLOCK_STATUS : CacheBlockStatus {};
	};
	
	struct L1I : Cache<MSS_TYPES, L1I_CONFIG, L1I>
	{
		typedef Cache<MSS_TYPES, L1I_CONFIG, L1I> Super;
		
		L1I(ThisMemorySubSystem *_mss) : Super(), mss(_mss) {}
		
		static const char *GetCacheName() { return "L1I"; }
		inline bool IsVerbose() const ALWAYS_INLINE { return true; }
		inline bool IsEnabled() const ALWAYS_INLINE { return true; }
		inline bool IsWriteAllocate(typename MSS_TYPES::STORAGE_ATTR storage_attr) const  ALWAYS_INLINE { return false; }
		inline bool ChooseLineToEvict(CacheAccess<MSS_TYPES, L1I>& access) ALWAYS_INLINE
		{
			access.way = access.set->Status().lru.Select();
			return true;
		}
		inline void UpdateReplacementPolicyOnAccess(CacheAccess<MSS_TYPES, L1I>& access) ALWAYS_INLINE
		{
			access.set->Status().lru.UpdateOnAccess(access.way);
		}
		
	private:
		ThisMemorySubSystem *mss;
	};

	struct L1D_CONFIG
	{
		static const uint32_t SIZE                     = 32768;
		static const CacheWritingPolicy WRITING_POLICY = CACHE_WRITE_BACK_AND_WRITE_ALLOCATE_POLICY;
		static const CacheType TYPE                    = DATA_CACHE;
		static const unsigned int ASSOCIATIVITY        = 8;
		static const unsigned int BLOCK_SIZE           = 32;
		static const unsigned int BLOCKS_PER_LINE      = 1;
		
		struct SET_STATUS : CacheSetStatus
		{
			LRU_ReplacementPolicy<ASSOCIATIVITY> lru;
		};
		
		struct LINE_STATUS : CacheLineStatus {};
		
		struct BLOCK_STATUS : CacheBlockStatus {};
	};
	
	struct L1D : Cache<MSS_TYPES, L1D_CONFIG, L1D>
	{
		typedef Cache<MSS_TYPES, L1D_CONFIG, L1D> Super;
		
		L1D(ThisMemorySubSystem *_mss) : Super(), mss(_mss) {}
		
		static const char *GetCacheName() { return "L1D"; }
		inline bool IsVerbose() const ALWAYS_INLINE { return true; }
		inline bool IsEnabled() const ALWAYS_INLINE { return true; }
		inline bool IsWriteAllocate(typename MSS_TYPES::STORAGE_ATTR storage_attr) const  ALWAYS_INLINE { return false; }
		inline bool ChooseLineToEvict(CacheAccess<MSS_TYPES, L1D>& access) ALWAYS_INLINE
		{
			access.way = access.set->Status().lru.Select();
			return true;
		}
		inline void UpdateReplacementPolicyOnAccess(CacheAccess<MSS_TYPES, L1D>& access) ALWAYS_INLINE
		{
			access.set->Status().lru.UpdateOnAccess(access.way);
		}
		
	private:
		ThisMemorySubSystem *mss;
	};
	
	struct L2U_CONFIG
	{
		static const uint32_t SIZE                     = 131072;
		static const CacheWritingPolicy WRITING_POLICY = CACHE_WRITE_BACK_AND_WRITE_ALLOCATE_POLICY;
		static const CacheType TYPE                    = UNIFIED_CACHE;
		static const unsigned int ASSOCIATIVITY        = 2;
		static const unsigned int BLOCK_SIZE           = 32;
		static const unsigned int BLOCKS_PER_LINE      = 2;
		
		struct SET_STATUS : CacheSetStatus
		{
			LRU_ReplacementPolicy<ASSOCIATIVITY> lru;
		};
		
		struct LINE_STATUS : CacheLineStatus {};
		
		struct BLOCK_STATUS : CacheBlockStatus {};
	};
	
	struct L2U : Cache<MSS_TYPES, L2U_CONFIG, L2U>
	{
		typedef Cache<MSS_TYPES, L2U_CONFIG, L2U> Super;
		
		L2U(ThisMemorySubSystem *_mss) : Super(), mss(_mss) {}
		
		static const char *GetCacheName() { return "L2U"; }
		inline bool IsVerbose() const ALWAYS_INLINE { return true; }
		inline bool IsEnabled() const ALWAYS_INLINE { return true; }
		inline bool IsWriteAllocate(typename MSS_TYPES::STORAGE_ATTR storage_attr) const  ALWAYS_INLINE { return false; }
		inline bool ChooseLineToEvict(CacheAccess<MSS_TYPES, L2U>& access) ALWAYS_INLINE
		{
			access.way = access.set->Status().lru.Select();
			return true;
		}
		inline void UpdateReplacementPolicyOnAccess(CacheAccess<MSS_TYPES, L2U>& access) ALWAYS_INLINE
		{
			access.set->Status().lru.UpdateOnAccess(access.way);
		}
		
	private:
		ThisMemorySubSystem *mss;
	};
	
	MPU mpu;
	IMEM imem;
	DMEM dmem;
	L1I l1i;
	L1D l1d;
	L2U l2u;
	
	typedef MPU ACCESS_CONTROLLER;
	typedef LocalMemorySet<MSS_TYPES, DMEM> DATA_LOCAL_MEMORIES;
	typedef LocalMemorySet<MSS_TYPES, IMEM> INSTRUCTION_LOCAL_MEMORIES;
	typedef CacheHierarchy<MSS_TYPES, L1D, L2U> DATA_CACHE_HIERARCHY;
	typedef CacheHierarchy<MSS_TYPES, L1I, L2U> INSTRUCTION_CACHE_HIERARCHY;
	
	MyMemorySubSystem();

	MPU *GetAccessController() ALWAYS_INLINE { return &mpu; }
	IMEM *GetLocalMemory(const IMEM *) ALWAYS_INLINE { return &imem; }
	DMEM *GetLocalMemory(const DMEM *) ALWAYS_INLINE { return &dmem; }
	L1I *GetCache(const L1I *) ALWAYS_INLINE { return &l1i; }
	L1D *GetCache(const L1D *) ALWAYS_INLINE { return &l1d; }
	L2U *GetCache(const L2U *) ALWAYS_INLINE { return &l2u; }

	inline bool IsVerboseDataLoad() const ALWAYS_INLINE { return true; }
	inline bool IsVerboseDataStore() const ALWAYS_INLINE { return true; }
	inline bool IsVerboseInstructionFetch() const ALWAYS_INLINE { return true; }
	inline bool IsVerboseDataBusRead() const ALWAYS_INLINE { return true; }
	inline bool IsVerboseDataBusWrite() const ALWAYS_INLINE { return true; }
	inline bool IsVerboseInstructionBusRead() const ALWAYS_INLINE { return true; }
	bool IsStorageCacheable(typename MSS_TYPES::STORAGE_ATTR storage_attr) const { return true; }
	bool IsDataStoreAccessWriteThrough(typename MSS_TYPES::STORAGE_ATTR storage_attr) const { return false; }
	
	bool DataBusRead(typename MSS_TYPES::ADDRESS addr, void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr, bool rwitm);
	bool DataBusWrite(typename MSS_TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr);
	bool InstructionBusRead(typename MSS_TYPES::ADDRESS addr, void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr);

	bool DebugDataBusRead(typename MSS_TYPES::ADDRESS addr, void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr);
	bool DebugDataBusWrite(typename MSS_TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr);
	bool DebugInstructionBusRead(typename MSS_TYPES::ADDRESS addr, void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr);
	
	void InvalidateLineByAddress(typename MSS_TYPES::ADDRESS addr);
	void CleanBySetAndWay(unsigned int index, unsigned int way);
	void CleanByAddress(typename MSS_TYPES::ADDRESS addr);
	
	std::map<typename MSS_TYPES::ADDRESS, uint8_t> memory;
};

template <typename MSS_TYPES>
MyMemorySubSystem<MSS_TYPES>::MyMemorySubSystem()
	: Super()
	, imem()
	, dmem()
	, l1i(this)
	, l1d(this)
	, l2u(this)
{
}


template <typename MSS_TYPES>
bool MyMemorySubSystem<MSS_TYPES>::DataBusRead(typename MSS_TYPES::ADDRESS addr, void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr, bool rwitm)
{
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		((uint8_t *) buffer)[i] = memory[addr + i];
	}
	return true;
}

template <typename MSS_TYPES>
bool MyMemorySubSystem<MSS_TYPES>::DataBusWrite(typename MSS_TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr)
{
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		memory[addr + i] = ((uint8_t *) buffer)[i];
	}
	return true;
}

template <typename MSS_TYPES>
bool MyMemorySubSystem<MSS_TYPES>::InstructionBusRead(typename MSS_TYPES::ADDRESS addr, void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr)
{
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		((uint8_t *) buffer)[i] = memory[addr + i];
	}
	return true;
}

template <typename MSS_TYPES>
bool MyMemorySubSystem<MSS_TYPES>::DebugDataBusRead(typename MSS_TYPES::ADDRESS addr, void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr)
{
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		((uint8_t *) buffer)[i] = memory[addr + i];
	}
	return true;
}

template <typename MSS_TYPES>
bool MyMemorySubSystem<MSS_TYPES>::DebugDataBusWrite(typename MSS_TYPES::ADDRESS addr, const void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr)
{
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		memory[addr + i] = ((uint8_t *) buffer)[i];
	}
	return true;
}

template <typename MSS_TYPES>
bool MyMemorySubSystem<MSS_TYPES>::DebugInstructionBusRead(typename MSS_TYPES::ADDRESS addr, void *buffer, unsigned int size, typename MSS_TYPES::STORAGE_ATTR storage_attr)
{
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		((uint8_t *) buffer)[i] = memory[addr + i];
	}
	return true;
}

template <typename MSS_TYPES>
void MyMemorySubSystem<MSS_TYPES>::InvalidateLineByAddress(typename MSS_TYPES::ADDRESS addr)
{
	Super::template GlobalInvalidateLineByAddress<DATA_CACHE_HIERARCHY, L1D, L2U>(addr);
}

template <typename MSS_TYPES>
void MyMemorySubSystem<MSS_TYPES>::CleanBySetAndWay(unsigned int index, unsigned int way)
{
	Super::template WriteBackLineBySetAndWay<DATA_CACHE_HIERARCHY, L1D, true>(index, way);
}

template <typename MSS_TYPES>
void MyMemorySubSystem<MSS_TYPES>::CleanByAddress(typename MSS_TYPES::ADDRESS addr)
{
	Super::template GlobalWriteBackBlockByAddress<DATA_CACHE_HIERARCHY, L1D, L2U, true>(addr);
}

} // end of namespace sandbox

int main()
{
	uint8_t dbg_init_buf[64] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
	                           , 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
	                           , 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f
	                           , 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f };
	uint8_t buf[4] = { 0xab, 0xcd, 0xef, 0x10 };
	
	sandbox::MyMemorySubSystem<sandbox::MEMORY_SUB_SYSTEM_TYPES> mss;
	
	mss.DataStore(0x50000000, dbg_init_buf, 64);
	mss.DebugDataStore(0x1200, dbg_init_buf, 64);
	
	mss.DataStore(0x1234, buf, 4);
	mss.DataLoad(0x1234, buf, 4);
	mss.DataLoad(0x1260, buf, 4);
	mss.DataLoad(0x1280, buf, 4);
	mss.DataLoad(0x12a0, buf, 4);
	
	uint8_t dbg_buf[64];
	
	bool dbg_data_load_status = mss.DebugDataLoad(0x1200, dbg_buf, 64);
	
	unsigned int i;
	std::cout << "dbg_buf (" << dbg_data_load_status << ") = ";
	for(i = 0; i < 64; i++)
	{
		uint8_t value = dbg_buf[i];
		uint8_t l = value & 15;
		uint8_t h = value >> 4;
		std::cout << (i ? " ": "") << "0x" << (char)((h < 10) ? '0' + h : 'a' + h - 10) << (char)((l < 10) ? '0' + l : 'a' + l - 10);
	}
	std::cout << std::endl;
	//mss.DataLoad(0x1234, buf, 4, 0);
	//mss.InstructionFetch(0x1234, buf, 4, 0);
	//mss.CleanBySetAndWay(0x12, 0x0);
	mss.CleanByAddress(0x1234);
	
	//mss.WriteBackCacheLine<sandbox::MyMemorySubSystem::DATA_CACHE_HIERARCHY, sandbox::MyMemorySubSystem::L1D, sandbox::MyMemorySubSystem::L2U>(4, 1);
	//mss.WriteBackCacheSet<sandbox::MyMemorySubSystem::DATA_CACHE_HIERARCHY, sandbox::MyMemorySubSystem::L1D, sandbox::MyMemorySubSystem::Memory, false>(0x12);
	
	//sandbox::MyMemorySubSystem::L2U *l2u = mss.GetCache<sandbox::MyMemorySubSystem::L2U>();
	//l2u->InvalidateLine(0x12, 0);
	return 0;
}
