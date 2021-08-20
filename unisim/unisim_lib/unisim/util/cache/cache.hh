/*
 *  Copyright (c) 2008-2017,
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

#ifndef __UNISIM_UTIL_CACHE_CACHE_HH__
#define __UNISIM_UTIL_CACHE_CACHE_HH__

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(HAVE_CXXABI)
#include <execinfo.h>
#include <cxxabi.h>
#endif

#include <unisim/util/likely/likely.hh>
#include <unisim/util/inlining/inlining.hh>
#include <unisim/component/cxx/memory/sparse/memory.hh>
#include <iostream>
#include <typeinfo>
#include <inttypes.h>
#include <string.h>
#include <stdexcept>
#include <cstdlib>
#include <cassert>

namespace unisim {
namespace util {
namespace cache {

//                                                                    SECTORED ASSOCIATIVE CACHE                                                  
//          INDEX
//            
//               +---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+
//               |             WAY #0            |             WAY #1            |             WAY #2            |             WAY #3            |
//               |             LINE              |             LINE              |             LINE              |             LINE              |
//               |               |               |               |               |               |               |               |               |
//      SET  #0  |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   | 
//               |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |
//               +---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+
//               |             WAY #0            |             WAY #1            |             WAY #2            |             WAY #3            |
//               |             LINE              |             LINE              |             LINE              |             LINE              |
//               |               |               |               |               |               |               |               |               |
//      SET  #1  |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   | 
//               |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |
//               +---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+
//               |             WAY #0            |             WAY #1            |             WAY #2            |             WAY #3            |
//               |             LINE              |             LINE              |             LINE              |             LINE              |
//               |               |               |               |               |               |               |               |               |
//      SET  #2  |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   | 
//               |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |
//               +---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+
//               |             WAY #0            |             WAY #1            |             WAY #2            |             WAY #3            |
//               |             LINE              |             LINE              |             LINE              |             LINE              |
//               |               |               |               |               |               |               |               |               |
//      SET  #3  |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   |   SECTOR #0   |   SECTOR #1   | 
//               |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |    BLOCK      |
//               +---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////// DECLARATIONS //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////// LocalMemoryType /////////////////////////////////

enum LocalMemoryType
{
	NULL_LOC_MEM        = 0, // null local memory
	DATA_LOC_MEM        = 1, // data local memory
	INSTRUCTION_LOC_MEM = 2, // instruction local memory
	UNIFIED_LOC_MEM     = 3, // unified local memory (both data and instruction)
};

inline std::ostream& operator << (std::ostream& os, const LocalMemoryType& lmt);

///////////////////////// CacheType /////////////////////////////

enum CacheType
{
	NULL_CACHE         = 0, // null cache
	DATA_CACHE         = 1, // data cache
	INSTRUCTION_CACHE  = 2, // instruction cache
	UNIFIED_CACHE      = 3  // unified cache (both data and instruction)
};

inline std::ostream& operator << (std::ostream& os, const CacheType& ct);

///////////////////////// CacheWritingPolicyFlags /////////////////////////////

enum CacheWritingPolicyFlags
{
	CACHE_WRITE_BACK     = 1, // Flag write back/write through
	CACHE_WRITE_ALLOCATE = 2  // Flag write allocate/no-write allocate
};

////////////////////////////// CacheWritingPolicy /////////////////////////////

enum CacheWritingPolicy
{
	CACHE_WRITE_THROUGH_AND_NO_WRITE_ALLOCATE_POLICY = 0,                                       // Write-through and No-Write allocate policy
	CACHE_WRITE_BACK_AND_NO_WRITE_ALLOCATE_POLICY    = CACHE_WRITE_BACK,                        // Write-back and Write allocate policy
	CACHE_WRITE_THROUGH_AND_WRITE_ALLOCATE_POLICY    = CACHE_WRITE_ALLOCATE,                    // Write-through and No-Write allocate policy
	CACHE_WRITE_BACK_AND_WRITE_ALLOCATE_POLICY       = CACHE_WRITE_BACK | CACHE_WRITE_ALLOCATE  // Write-back and Write allocate policy
};

inline std::ostream& operator << (std::ostream& os, const CacheWritingPolicy& cwp);

/////////////////////////////// CacheIndexScheme //////////////////////////////

enum CacheIndexScheme
{
	CACHE_VIRTUALLY_INDEXED,
	CACHE_PHYSICALLY_INDEXED
};

inline std::ostream& operator << (std::ostream& os, const CacheIndexScheme& cis);

//////////////////////////////// CacheTagScheme ///////////////////////////////

enum CacheTagScheme
{
	CACHE_VIRTUALLY_TAGGED,
	CACHE_PHYSICALLY_TAGGED
};

inline std::ostream& operator << (std::ostream& os, const CacheTagScheme& cts);

/////////////////////////// Forward declarations //////////////////////////////

template <typename TYPES> struct NullLocalMemory;
template <typename TYPES> struct NullLocalMemory1;
template <typename TYPES> struct NullLocalMemory2;
template <typename TYPES> struct NullLocalMemory3;
template <typename TYPES> struct NullLocalMemory4;
template <typename TYPES, typename _LOC_MEM1, typename _LOC_MEM2, typename _LOC_MEM3, typename _LOC_MEM4> struct LocalMemorySet;
template <typename TYPES, typename CONFIG, typename LOC_MEM> struct LocalMemory;
template <typename TYPES, typename CACHE> struct CacheAccess;
struct CacheBlockStatus;
struct CacheLineStatus;
struct CacheSetStatus;
template <typename TYPES, typename CONFIG> struct CacheBlock;
template <typename TYPES, typename CONFIG> struct CacheLine;
template <typename TYPES, typename CONFIG> struct CacheSet;
template <typename TYPES, typename CONFIG, typename CACHE> struct Cache;
template <typename TYPES> struct NullCache;
template <typename TYPES> struct NullCache1;
template <typename TYPES> struct NullCache2;
template <typename TYPES> struct NullCache3;
template <typename TYPES> struct NullCache4;
template <typename TYPES, typename _L1CACHE, typename _L2CACHE, typename _L3CACHE, typename _L4CACHE> struct CacheHierarchy;
template <typename TYPES, typename MSS> struct MemorySubSystem;

/////////////////////////// NullLocalMemoryConfig /////////////////////////////

struct NullLocalMemoryConfig
{
	static const LocalMemoryType TYPE = NULL_LOC_MEM;
	static const unsigned int BASE_ADDRESS = 0;
	static const unsigned int SIZE = 1;
};

///////////////////////////// NullLocalMemory<> ////////////////////////////////

template <typename TYPES>
struct NullLocalMemory : LocalMemory<TYPES, NullLocalMemoryConfig, NullLocalMemory<TYPES> >
{
	static const char *GetLocalMemoryName() { return "NullLocalMemory"; }
};

//////////////////////////// NullLocalMemory1<> ////////////////////////////////

template <typename TYPES>
struct NullLocalMemory1 : LocalMemory<TYPES, NullLocalMemoryConfig, NullLocalMemory1<TYPES> >
{
	static const char *GetLocalMemoryName() { return "NullLocalMemory1"; }
};

//////////////////////////// NullLocalMemory2<> ////////////////////////////////

template <typename TYPES>
struct NullLocalMemory2 : LocalMemory<TYPES, NullLocalMemoryConfig, NullLocalMemory2<TYPES> >
{
	static const char *GetLocalMemoryName() { return "NullLocalMemory2"; }
};

//////////////////////////// NullLocalMemory3<> ////////////////////////////////

template <typename TYPES>
struct NullLocalMemory3 : LocalMemory<TYPES, NullLocalMemoryConfig, NullLocalMemory3<TYPES> >
{
	static const char *GetLocalMemoryName() { return "NullLocalMemory3"; }
};

//////////////////////////// NullLocalMemory4<> ////////////////////////////////

template <typename TYPES>
struct NullLocalMemory4 : LocalMemory<TYPES, NullLocalMemoryConfig, NullLocalMemory4<TYPES> >
{
	static const char *GetLocalMemoryName() { return "NullLocalMemory4"; }
};

///////////////////////////// LocalMemorySet<> ////////////////////////////////

template <typename TYPES, typename _LOC_MEM1 = NullLocalMemory1<TYPES>, typename _LOC_MEM2 = NullLocalMemory2<TYPES>, typename _LOC_MEM3 = NullLocalMemory3<TYPES>, typename _LOC_MEM4 = NullLocalMemory4<TYPES> >
struct LocalMemorySet
{
	typedef _LOC_MEM1 LOC_MEM1;
	typedef _LOC_MEM2 LOC_MEM2;
	typedef _LOC_MEM3 LOC_MEM3;
	typedef _LOC_MEM4 LOC_MEM5;
	
	template <typename _LOC_MEM, bool dummy = true>
	struct NextLocalMemory
	{
		typedef NullLocalMemory<TYPES> LOC_MEM;
	};
	
	template <bool dummy>
	struct NextLocalMemory<_LOC_MEM1, dummy>
	{
		typedef _LOC_MEM2 LOC_MEM;
	};

	template <bool dummy>
	struct NextLocalMemory<_LOC_MEM2, dummy>
	{
		typedef _LOC_MEM3 LOC_MEM;
	};

	template <bool dummy>
	struct NextLocalMemory<_LOC_MEM3, dummy>
	{
		typedef _LOC_MEM4 LOC_MEM;
	};
};

////////////////////////////// LocalMemory<> //////////////////////////////////

template <typename TYPES, typename CONFIG, typename LOC_MEM>
struct LocalMemory
{
	typedef CONFIG LOC_MEM_CONFIG;
	static const LocalMemoryType TYPE = CONFIG::TYPE;
	static const typename TYPES::PHYSICAL_ADDRESS BASE_ADDRESS = CONFIG::BASE_ADDRESS;
	static const unsigned int SIZE = CONFIG::SIZE;
	
	LocalMemory();
	~LocalMemory();
	
	static inline bool IsNullLocalMemory() ALWAYS_INLINE;
	static inline bool IsDataLocalMemoryOnly() ALWAYS_INLINE;
	static inline bool IsDataLocalMemory() ALWAYS_INLINE;
	static inline bool IsInstructionLocalMemoryOnly() ALWAYS_INLINE;
	static inline bool IsInstructionLocalMemory() ALWAYS_INLINE;
	static inline bool IsUnifiedLocalMemory() ALWAYS_INLINE;
	
	inline uint8_t& GetByteByOffset(unsigned int offset) ALWAYS_INLINE;
	inline uint8_t& operator [] (unsigned int offset) ALWAYS_INLINE;
	
	///////////////// To be overriden by derived class ////////////////////////
	
	inline bool IsVerbose() const ALWAYS_INLINE;
	inline typename TYPES::PHYSICAL_ADDRESS GetBasePhysicalAddress() const;
	inline unsigned int GetSize() const;

	///////////////////////////////////////////////////////////////////////////
protected:
	uint64_t num_load_accesses;
	uint64_t num_store_accesses;
private:
	uint8_t bytes[CONFIG::SIZE];

	template <typename, typename MSS> friend struct MemorySubSystem;
	
	//////////////////// trampoline to overridable methods /////////////////////
	
	static inline const char *__MSS_GetLocalMemoryName__() ALWAYS_INLINE;
	inline bool __MSS_IsVerbose__() const ALWAYS_INLINE;
	inline typename TYPES::PHYSICAL_ADDRESS __MSS_GetBasePhysicalAddress__() const ALWAYS_INLINE;
	inline unsigned int __MSS_GetSize__() const ALWAYS_INLINE;
};

/////////////////////////////// CacheAccess<> /////////////////////////////////

template <typename TYPES, typename CACHE>
struct CacheAccess
{
	///////////////////////////////// input ///////////////////////////////////
	typename TYPES::ADDRESS addr;               // logical or virtual address
	typename TYPES::PHYSICAL_ADDRESS phys_addr; // physical address
	typename TYPES::STORAGE_ATTR storage_attr;  // storage attribute
	bool rwitm;                                 // read with intent to modify
	
	////////////////////////////////////////////////////////////////////////////////////// input ////////////////////// output //////////
	typename TYPES::PHYSICAL_ADDRESS line_base_addr;                           //            X                          Lookup
	typename TYPES::PHYSICAL_ADDRESS block_base_addr;                          //            X                          Lookup
	typename TYPES::PHYSICAL_ADDRESS line_base_phys_addr;                      //            X                          Lookup
	typename TYPES::PHYSICAL_ADDRESS block_base_phys_addr;                     //            X                          Lookup
	unsigned int index;                                                        //            X                  Lookup/ChooseLineToEvict
	unsigned int way;                                                          //            X                          Lookup
	unsigned int sector;                                                       //            X                          Lookup
	unsigned int offset;                                                       //            X                          Lookup
	unsigned int size_to_block_boundary;                                       //            X                          Lookup
	Cache<TYPES, typename CACHE::CACHE_CONFIG, CACHE> *cache;                  // WriteBackLine/EvictLine/FillBlock     Lookup 
	CacheSet<TYPES, typename CACHE::CACHE_CONFIG> *set;                        //            X                          Lookup
	CacheLine<TYPES, typename CACHE::CACHE_CONFIG> *line;                      //            X                          Lookup
	CacheLine<TYPES, typename CACHE::CACHE_CONFIG> *line_to_write_back_evict;  // WriteBackLine/EvictLine                 X
	CacheBlock<TYPES, typename CACHE::CACHE_CONFIG> *block;                    //       FillBlock                       Lookup
};

/////////////////////////// CacheBlockStatus<> //////////////////////////////////

struct CacheBlockStatus
{
	bool valid;
	bool dirty;
	
	CacheBlockStatus() : valid(false), dirty(false) {}
	inline bool IsValid() const ALWAYS_INLINE { return valid; }
	inline bool IsDirty() const ALWAYS_INLINE { return dirty; }
	inline void SetValid(bool v = true) ALWAYS_INLINE { valid = v; }
	inline void SetDirty(bool v = true) ALWAYS_INLINE { dirty = v; }
	inline void Invalidate() ALWAYS_INLINE { SetValid(false); SetDirty(false); }
};

/////////////////////////// CacheLineStatus<> ///////////////////////////////////

struct CacheLineStatus
{
	inline void Invalidate() ALWAYS_INLINE {}
};

//////////////////////////// CacheSetStatus<> ///////////////////////////////////

struct CacheSetStatus
{
	inline void Invalidate() ALWAYS_INLINE {}
};

/////////////////////////////// CacheBlock<> //////////////////////////////////

template <typename TYPES, typename CONFIG>
struct CacheBlock
{
public:
	CacheBlock();
	~CacheBlock();
	inline uint8_t& operator [] (unsigned int offset) ALWAYS_INLINE;
	inline uint8_t& GetByteByOffset(unsigned int offset) ALWAYS_INLINE;
	inline typename TYPES::ADDRESS GetBaseAddress() const ALWAYS_INLINE;
	inline typename TYPES::PHYSICAL_ADDRESS GetBasePhysicalAddress() const ALWAYS_INLINE;
	inline void Zero() ALWAYS_INLINE;
	inline void Write(const void *buffer, unsigned int offset, unsigned int size) ALWAYS_INLINE;
	inline void Read(void *buffer, unsigned int offset, unsigned int size) ALWAYS_INLINE;
	
	inline bool IsValid() const ALWAYS_INLINE;
	inline bool IsDirty() const ALWAYS_INLINE;
	inline void SetValid(bool v = true) ALWAYS_INLINE;
	inline void SetDirty(bool v = true) ALWAYS_INLINE;
	inline void Invalidate() ALWAYS_INLINE;
	inline typename CONFIG::BLOCK_STATUS& Status() ALWAYS_INLINE;

	void SetSector(unsigned int sector);
	inline unsigned int GetSector() const;
protected:
	friend struct CacheLine<TYPES, CONFIG>;
	inline void SetBaseAddress(typename TYPES::ADDRESS addr);
	inline void SetBasePhysicalAddress(typename TYPES::PHYSICAL_ADDRESS base_phys_addr);
private:
	typename TYPES::ADDRESS base_addr;
	typename TYPES::PHYSICAL_ADDRESS base_phys_addr;
	typename CONFIG::BLOCK_STATUS status;
	unsigned int sector;
	uint8_t storage[CONFIG::BLOCK_SIZE];
};

/////////////////////////////// CacheLine<> ///////////////////////////////////

template <typename TYPES, typename CONFIG>
struct CacheLine
{
public:
	CacheLine();
	~CacheLine();

	inline void SetWay(unsigned int way) ALWAYS_INLINE;
	inline void SetBaseAddress(typename TYPES::ADDRESS addr);
	inline void SetBasePhysicalAddress(typename TYPES::PHYSICAL_ADDRESS phys_addr) ALWAYS_INLINE;
	inline typename TYPES::ADDRESS GetBaseAddress() const ALWAYS_INLINE;
	inline typename TYPES::PHYSICAL_ADDRESS GetBasePhysicalAddress() const ALWAYS_INLINE;
	inline CacheBlock<TYPES, CONFIG>& GetBlockBySector(unsigned int sector) ALWAYS_INLINE;
	inline CacheBlock<TYPES, CONFIG>& operator [] (unsigned int sector) ALWAYS_INLINE;

	inline unsigned int GetWay() const ALWAYS_INLINE;
	inline bool IsValid() const ALWAYS_INLINE;
	inline void SetValid() ALWAYS_INLINE;
	inline void Invalidate() ALWAYS_INLINE;
	inline typename CONFIG::LINE_STATUS& Status() ALWAYS_INLINE;
protected:
private:
	friend struct CacheSet<TYPES, CONFIG>;
	
	bool valid;
	CacheLine<TYPES, CONFIG> *next;
	CacheLine<TYPES, CONFIG> *prev;
	typename TYPES::ADDRESS base_addr;
	typename TYPES::PHYSICAL_ADDRESS base_phys_addr;
	unsigned int way;
	typename CONFIG::LINE_STATUS status;
	CacheBlock<TYPES, CONFIG> blocks[CONFIG::BLOCKS_PER_LINE];
};

//////////////////////////////// CacheSet<> ///////////////////////////////////

template <typename TYPES, typename CONFIG>
struct CacheSet
{
public:
	CacheSet();
	~CacheSet();

	inline CacheLine<TYPES, CONFIG>& GetLineByWay(unsigned int way) ALWAYS_INLINE;
	inline CacheLine<TYPES, CONFIG>& operator [] (unsigned int way) ALWAYS_INLINE;

	inline void InvalidateLineByWay(unsigned int way) ALWAYS_INLINE;
	inline void Invalidate() ALWAYS_INLINE;

	void SetIndex(unsigned int index);
	inline unsigned int GetIndex() const ALWAYS_INLINE;
	
	inline typename CONFIG::SET_STATUS& Status() ALWAYS_INLINE;
	
	inline CacheLine<TYPES, CONFIG> *AssociativeSearchByLineBasePhysicalAddress(typename TYPES::PHYSICAL_ADDRESS line_base_phys_addr) ALWAYS_INLINE;
	inline CacheLine<TYPES, CONFIG> *AssociativeSearchByLineBaseAddress(typename TYPES::ADDRESS line_base_addr) ALWAYS_INLINE;

protected:
private:
	typename CONFIG::SET_STATUS status;
	CacheLine<TYPES, CONFIG> *mrs_line;
	CacheLine<TYPES, CONFIG> *lrs_line;
	CacheLine<TYPES, CONFIG> lines[CONFIG::ASSOCIATIVITY];
	unsigned int index;
};
/////////////////////////////////// Cache<> ///////////////////////////////////

template <typename TYPES, typename CONFIG, typename CACHE>
struct Cache
{
	typedef CONFIG CACHE_CONFIG;
	
	static const CacheWritingPolicy WRITING_POLICY = CONFIG::WRITING_POLICY;
	static const CacheIndexScheme INDEX_SCHEME = CONFIG::INDEX_SCHEME;
	static const CacheTagScheme TAG_SCHEME = CONFIG::TAG_SCHEME;
	static const CacheType TYPE = CONFIG::TYPE;
	static const unsigned int ASSOCIATIVITY = CONFIG::ASSOCIATIVITY;
	static const unsigned int BLOCKS_PER_LINE = CONFIG::BLOCKS_PER_LINE;
	static const unsigned int BLOCK_SIZE = CONFIG::BLOCK_SIZE;
	static const unsigned int SIZE = CONFIG::SIZE;
	typedef typename CONFIG::SET_STATUS SET_STATUS;
	typedef typename CONFIG::LINE_STATUS LINE_STATUS;
	typedef typename CONFIG::BLOCK_STATUS BLOCK_STATUS;
	
	Cache();
	~Cache();
	
	static inline typename TYPES::ADDRESS LineBaseAddress(typename TYPES::ADDRESS addr) ALWAYS_INLINE;
	static inline typename TYPES::PHYSICAL_ADDRESS LineBasePhysicalAddress(typename TYPES::PHYSICAL_ADDRESS phys_addr) ALWAYS_INLINE;
	static inline typename TYPES::ADDRESS BlockBaseAddress(typename TYPES::ADDRESS addr) ALWAYS_INLINE;
	static inline typename TYPES::PHYSICAL_ADDRESS BlockBasePhysicalAddress(typename TYPES::PHYSICAL_ADDRESS phys_addr) ALWAYS_INLINE;
	static inline unsigned int SectorByAddress(typename TYPES::ADDRESS addr) ALWAYS_INLINE;
	static inline unsigned int SectorByPhysicalAddress(typename TYPES::PHYSICAL_ADDRESS phys_addr) ALWAYS_INLINE;
	static inline unsigned int IndexByAddress(typename TYPES::ADDRESS addr) ALWAYS_INLINE;
	static inline unsigned int IndexByPhysicalAddress(typename TYPES::PHYSICAL_ADDRESS phys_addr) ALWAYS_INLINE;
	static inline void DecodeAddress(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;
	inline CacheSet<TYPES, CONFIG>& GetSetByIndex(unsigned int index) ALWAYS_INLINE;
	inline CacheSet<TYPES, CONFIG>& operator [] (unsigned int index) ALWAYS_INLINE;

	inline void InvalidateSet(unsigned int index) ALWAYS_INLINE;
	inline void InvalidateLineBySetAndWay(unsigned int index, unsigned int way) ALWAYS_INLINE;
	inline void Invalidate() ALWAYS_INLINE;
	inline CacheSet<TYPES, CONFIG> *LookupSetByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr) ALWAYS_INLINE;
	inline CacheLine<TYPES, CONFIG> *LookupLineByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr) ALWAYS_INLINE;
	inline CacheBlock<TYPES, CONFIG> *LookupBlockByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr) ALWAYS_INLINE;
	inline void Lookup(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;
	
	static inline bool IsNullCache() ALWAYS_INLINE;
	static inline bool IsDataCacheOnly() ALWAYS_INLINE;
	static inline bool IsDataCache() ALWAYS_INLINE;
	static inline bool IsInstructionCacheOnly() ALWAYS_INLINE;
	static inline bool IsInstructionCache() ALWAYS_INLINE;
	static inline bool IsUnifiedCache() ALWAYS_INLINE;
	static inline bool IsStaticWriteBackCache() ALWAYS_INLINE;
	static inline bool IsStaticWriteThroughCache() ALWAYS_INLINE;
	static inline bool IsStaticWriteAllocate() ALWAYS_INLINE;
	static inline bool IsStaticNotWriteAllocate() ALWAYS_INLINE;
	
	static inline unsigned int NumSets() ALWAYS_INLINE;
	static inline unsigned int NumWays() ALWAYS_INLINE;
	static inline unsigned int NumSectors() ALWAYS_INLINE;

	inline void Access() { num_accesses++; }
	
	///////////////// To be overriden by derived class ////////////////////////

	inline bool IsVerbose() const ALWAYS_INLINE;
	inline bool IsEnabled() const ALWAYS_INLINE;
	inline bool IsWriteAllocate(typename TYPES::STORAGE_ATTR storage_attr) const ALWAYS_INLINE;
	inline bool ChooseLineToEvict(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;
	inline void UpdateReplacementPolicyOnAccess(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;
	inline void UpdateReplacementPolicyOnFill(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;
	inline void OnAccess(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;

	///////////////////////////////////////////////////////////////////////////
protected:
	uint64_t num_accesses;
	uint64_t num_misses;
private:
	CacheSet<TYPES, CONFIG> sets[CONFIG::SIZE / CONFIG::BLOCK_SIZE / CONFIG::BLOCKS_PER_LINE / CONFIG::ASSOCIATIVITY];

	template <typename, typename MSS> friend struct MemorySubSystem;
	
	//////////////////// trampoline to overridable methods /////////////////////

	static inline const char *__MSS_GetCacheName__() ALWAYS_INLINE;
	inline bool __MSS_IsVerbose__() const ALWAYS_INLINE;
	inline bool __MSS_IsEnabled__() const ALWAYS_INLINE;
	inline bool __MSS_IsWriteAllocate__(typename TYPES::STORAGE_ATTR storage_attr) const  ALWAYS_INLINE;
	inline bool __MSS_ChooseLineToEvict__(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;
	inline void __MSS_UpdateReplacementPolicyOnAccess__(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;
	inline void __MSS_UpdateReplacementPolicyOnFill__(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;
	inline void __MSS_OnAccess__(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;
};

//////////////////////////// NullCacheConfig //////////////////////////////////

struct NullCacheConfig
{
	static const CacheWritingPolicy WRITING_POLICY = CACHE_WRITE_THROUGH_AND_NO_WRITE_ALLOCATE_POLICY;
	static const CacheIndexScheme INDEX_SCHEME = CACHE_PHYSICALLY_INDEXED;
	static const CacheTagScheme TAG_SCHEME = CACHE_PHYSICALLY_TAGGED;
	static const CacheType TYPE = NULL_CACHE;
	static const unsigned int ASSOCIATIVITY = 1;
	static const unsigned int BLOCKS_PER_LINE = 1;
	static const unsigned int BLOCK_SIZE = 1;
	static const unsigned int SIZE = 1;
	
	struct SET_STATUS : CacheSetStatus {};
	struct LINE_STATUS : CacheLineStatus {};
	struct BLOCK_STATUS : CacheBlockStatus {};
};

////////////////////////////// NullCache<> ///////////////////////////////////

template <typename TYPES> struct NullCache : Cache<TYPES, NullCacheConfig, NullCache<TYPES> >
{
	static const char *GetCacheName() { return "NullCache"; }
};

////////////////////////////// NullCache1<> ///////////////////////////////////

template <typename TYPES>
struct NullCache1 : Cache<TYPES, NullCacheConfig, NullCache1<TYPES> >
{
	static const char *GetCacheName() { return "NullCache1"; }
};

////////////////////////////// NullCache2<> ///////////////////////////////////

template <typename TYPES>
struct NullCache2 : Cache<TYPES, NullCacheConfig, NullCache2<TYPES> >
{
	static const char *GetCacheName() { return "NullCache2"; }
};

////////////////////////////// NullCache3<> ///////////////////////////////////

template <typename TYPES>
struct NullCache3 : Cache<TYPES, NullCacheConfig, NullCache3<TYPES> >
{
	static const char *GetCacheName() { return "NullCache3"; }
};

////////////////////////////// NullCache4<> ///////////////////////////////////

template <typename TYPES>
struct NullCache4 : Cache<TYPES, NullCacheConfig, NullCache4<TYPES> >
{
	static const char *GetCacheName() { return "NullCache4"; }
};

////////////////////////////// CacheHierarchy<> /////////////////////////////////

template <typename TYPES, typename _L1CACHE = NullCache1<TYPES>, typename _L2CACHE = NullCache2<TYPES>, typename _L3CACHE = NullCache3<TYPES>, typename _L4CACHE = NullCache4<TYPES> >
struct CacheHierarchy
{
	typedef _L1CACHE L1CACHE;
	typedef _L2CACHE L2CACHE;
	typedef _L3CACHE L3CACHE;
	typedef _L4CACHE L4CACHE;
	
	template <typename _CACHE, bool dummy = true>
	struct NextLevel
	{
		typedef NullCache<TYPES> CACHE;
	};
	
	template <bool dummy>
	struct NextLevel<_L1CACHE, dummy>
	{
		typedef _L2CACHE CACHE;
	};

	template <bool dummy>
	struct NextLevel<_L2CACHE, dummy>
	{
		typedef _L3CACHE CACHE;
	};

	template <bool dummy>
	struct NextLevel<_L3CACHE, dummy>
	{
		typedef _L4CACHE CACHE;
	};
};

////////////////////////////////// Same<> /////////////////////////////////////

template <typename T1, typename T2>
struct Same
{
	enum { VALUE = false };
};

template <typename T>
struct Same<T, T>
{
	enum { VALUE = true };
};

///////////////////////////// TypeForByteSize<> ///////////////////////////////

template <unsigned int BYTE_SIZE> struct TypeForByteSize {};
template <> struct TypeForByteSize<1> { typedef uint8_t TYPE; };
template <> struct TypeForByteSize<2> { typedef uint16_t TYPE; };
template <> struct TypeForByteSize<3> { typedef uint32_t TYPE; };
template <> struct TypeForByteSize<4> { typedef uint32_t TYPE; };
template <> struct TypeForByteSize<5> { typedef uint64_t TYPE; };
template <> struct TypeForByteSize<6> { typedef uint64_t TYPE; };
template <> struct TypeForByteSize<7> { typedef uint64_t TYPE; };
template <> struct TypeForByteSize<8> { typedef uint64_t TYPE; };

////////////////////////////// TypeForBitSize<> ///////////////////////////////

template <unsigned int SIZE> struct TypeForBitSize
{
	static const unsigned int BYTE_SIZE = (SIZE + 7) / 8;
	typedef typename TypeForByteSize<BYTE_SIZE>::TYPE TYPE;
	static const TYPE MASK = (SIZE == (8 * BYTE_SIZE)) ? (~TYPE(0)) : ((TYPE(1) << SIZE) - 1);
};

////////////////////////////////// LOG2 ///////////////////////////////////////

template <unsigned int N>
struct LOG2
{
    static const unsigned int VALUE = LOG2<N / 2>::VALUE + 1;
};

template <>
struct LOG2<1>
{
    static const unsigned int VALUE = 0;
};

 /////////////////////////////// CEIL_LOG2 ////////////////////////////////////

template <unsigned int N>
struct CEIL_LOG2
{
    static const unsigned int VALUE = (N > (1 << LOG2<N>::VALUE)) ? LOG2<N>::VALUE + 1 : LOG2<N>::VALUE;
};

///////////////////////// PLRU_ReplacementPolicy<> ////////////////////////////

/* associativity up to 65 */
template <unsigned ASSOCIATIVITY>
struct PLRU_ReplacementPolicy
{
	inline PLRU_ReplacementPolicy() : plru_bits(0) {}
	
	void PrintPLRU(std::ostream& os) const
	{
		unsigned int i;
		for(i = 0; i < (ASSOCIATIVITY - 1); i++)
		{
			if(i) os << " ";
			os << "B" << i << "=" << ((plru_bits >> i) & 1);
		}
	}
	
	inline void Clear()
	{
		plru_bits = 0;
		//std::cerr << "Clear: plru_bits = 0x" << std::hex << (unsigned long long) plru_bits << std::dec << std::endl;
	}
	
	inline unsigned int Select() const
	{
		unsigned int way;
		unsigned int i;
		unsigned int n;

		//std::cerr << "S: "; PrintPLRU(std::cerr); std::cerr << std::endl;

		for(i = 0, way = 0, n = 0; n < CEIL_LOG2<ASSOCIATIVITY>::VALUE; n++)
		{
			unsigned int bi = (plru_bits >> i) & 1;
			way = (way << 1) | bi;
			i = (i << 1) + 1 + bi;
		}

		//std::cerr << "S: way=" << way << std::endl;
		return way;
	}

	inline void UpdateOnAccess(unsigned int way)
	{
		//std::cerr << "A(" << way << "): "; PrintPLRU(std::cerr); std::cerr << std::endl;
		unsigned int i;
		unsigned int n;

		for(n = CEIL_LOG2<ASSOCIATIVITY>::VALUE, i = 0; n != 0; n--)
		{
			unsigned int bi = (way >> (n - 1)) & 1;
			plru_bits = (plru_bits & ~(1 << i)) | ((1 ^ bi) << i);
			i = (i << 1) + 1 + bi;
		}
		//std::cerr << "A(" << way << ") -> "; PrintPLRU(std::cerr); std::cerr << std::endl;
	}
	
private:
	typename TypeForBitSize<ASSOCIATIVITY - 1>::TYPE plru_bits;
};

////////////////////////// LRU_ReplacementPolicy<> ////////////////////////////

/* associativity up to 16 */
template <unsigned int ASSOCIATIVITY>
struct LRU_ReplacementPolicy
{
	typedef typename TypeForBitSize<ASSOCIATIVITY * CEIL_LOG2<ASSOCIATIVITY>::VALUE>::TYPE LRU_BITS_TYPE;
	static const unsigned int S = CEIL_LOG2<ASSOCIATIVITY>::VALUE;
	static const unsigned int L = (ASSOCIATIVITY - 1) * CEIL_LOG2<ASSOCIATIVITY>::VALUE;
	static const LRU_BITS_TYPE M = (LRU_BITS_TYPE(1) << S) - 1;
	
	inline LRU_ReplacementPolicy() : lru_bits(0)
	{
		Clear();
	}
	
	inline void Clear()
	{
		int i;
		for(i = ASSOCIATIVITY; i >= 0; i--)
		{
			lru_bits = (lru_bits << S) | i;
		}
		//std::cerr << "Clear: lru_bits = 0x" << std::hex << (unsigned long long) lru_bits << std::dec << std::endl;
	}
	
	inline unsigned int Select() const
	{
		unsigned int way = (lru_bits >> L) & M;
		//std::cerr << "S: way=" << way << std::endl;
		return way;
	}

	inline void UpdateOnAccess(unsigned int way)
	{
		//std::cerr << "A(" << way << "): lru_bits = 0x" << std::hex << (unsigned long long) lru_bits << std::dec << std::endl;
		if((lru_bits & M) == way) return;
		
		unsigned int i;

		LRU_BITS_TYPE _lru_bits = lru_bits;
		
		for(i = 0; i < ASSOCIATIVITY; i++, _lru_bits >>= S)
		{
			if((_lru_bits & M) == way)
			{
				lru_bits = ((((_lru_bits >> S) << (i * S)) | (lru_bits & ((LRU_BITS_TYPE(1) << (i * S)) - 1))) << S) | way; 
				//std::cerr << "A(" << way << ") -> 0x" << std::hex << (unsigned long long) lru_bits << std::dec << std::endl;
				return;
			}
		}
	}
	
private:
	LRU_BITS_TYPE lru_bits;
};

/////////////////////// RoundRobinReplacementPolicy<> /////////////////////////

/* unlimited associativity */
template <unsigned int ASSOCIATIVITY>
struct RoundRobinReplacementPolicy
{
	inline RoundRobinReplacementPolicy()
		: way(0)
	{
	}
	
	inline void Clear()
	{
		way = 0;
		//std::cerr << "Clear: way = " << way << std::endl;
	}
	
	inline unsigned int Select() const
	{
		//std::cerr << "S(): way = " << way << std::endl;
		return way;
	}

	inline void Update()
	{
		way = (way + 1) % ASSOCIATIVITY;
		//std::cerr << "F(): way = " << way << std::endl;
	}
	
private:
	unsigned int way;
};

///////////////////// PseudoRandomReplacementPolicy<> /////////////////////////

/* associativity up to 128 */
template <unsigned int ASSOCIATIVITY>
struct PseudoRandomReplacementPolicy
{
	inline PseudoRandomReplacementPolicy()
		: y(ASSOCIATIVITY - 1)
	{
	}
	
	inline void Clear()
	{
		y = ASSOCIATIVITY - 1;
	}
	
	inline unsigned int Select() const
	{
		unsigned int way = y / (256 / ASSOCIATIVITY); // Note: zero has slightly less probability to occur
		//std::cerr << "S(): way = " << way << std::endl;
		return way;
	}

	inline void Update()
	{
		// xorshift algorithm for 8-bit. y is in [1..255]
		y ^= (y << 7);
		y ^= (y >> 5);
		y ^= (y << 3);
		//std::cerr << "F(): way = " << way << std::endl;
	}
	
private:
	uint8_t y;
};

////////////////////////////// MemorySubSystem<> ////////////////////////////////

// TYPES:
//   - ADDRESS         : C++ type for a logical/effective address
//   - PHYSICAL_ADDRESS: C++ type for a physical address
//   - STORAGE_ATTR    : C++ type for storage attribute
template <typename TYPES, typename MSS>
struct MemorySubSystem
{
	/* Constructor */
	MemorySubSystem();
	
	/* Destructor */
	virtual ~MemorySubSystem();
	
	//////////////////// From the inside of the processor  ////////////////////
	
	/* Data Load */
	bool DataLoad(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Data Store */
	bool DataStore(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Instruction Fetch */
	bool InstructionFetch(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Debug Data Load */
	bool DebugDataLoad(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);

	/* Debug Data Store */
	bool DebugDataStore(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Debug Instruction Fetch */
	bool DebugInstructionFetch(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/////////////////// From the outside of the processor  ////////////////////
	
	/* Incoming Load */
	unsigned int IncomingLoad(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size);
	
	/* Incoming Store */
	unsigned int IncomingStore(typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size);

	/* Incoming Debug Load */
	unsigned int DebugIncomingLoad(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size);
	
	/* Incoming Debug Store */
	unsigned int DebugIncomingStore(typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size);
	
	/**** DMI */
	
	uint8_t *GetDirectAccess(typename TYPES::PHYSICAL_ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS& start_addr, typename TYPES::PHYSICAL_ADDRESS& end_addr);
	
protected:
	uint64_t num_data_load_accesses;
	uint64_t num_data_store_accesses;
	uint64_t num_instruction_fetch_accesses;
	uint64_t num_incoming_load_accesses;
	uint64_t num_incoming_store_accesses;
	uint64_t num_data_bus_read_accesses;
	uint64_t num_data_bus_write_accesses;
	uint64_t num_instruction_bus_read_accesses;
	uint64_t num_data_load_xfered_bytes;
	uint64_t num_data_store_xfered_bytes;
	uint64_t num_instruction_fetch_xfered_bytes;
	uint64_t num_incoming_load_xfered_bytes;
	uint64_t num_incoming_store_xfered_bytes;
	uint64_t num_data_bus_read_xfered_bytes;
	uint64_t num_data_bus_write_xfered_bytes;
	uint64_t num_instruction_bus_read_xfered_bytes;
	bool enable_shadow_memory;
	unisim::component::cxx::memory::sparse::Memory<typename TYPES::PHYSICAL_ADDRESS, 12, 12> shadow_memory;
	
	///////////////// To be overriden by derived class ////////////////////////

	// mandatory
#if 0
	inline LOC_MEM *GetLocalMemory(const LOC_MEM *null) ALWAYS_INLINE;
	inline CACHE *GetCache(const CACHE *null) ALWAYS_INLINE;
#endif
	
	// optional
	
	/* Get debug info stream */
	inline std::ostream& GetDebugInfoStream() ALWAYS_INLINE;
	
	/* Get debug warning stream */
	inline std::ostream& GetDebugWarningStream() ALWAYS_INLINE;

	/* Get debug error stream */
	inline std::ostream& GetDebugErrorStream() ALWAYS_INLINE;

	/* Enable/Disable verbosity of data loads */
	inline bool IsVerboseDataLoad() const ALWAYS_INLINE;
	
	/* Enable/Disable verbosity of data store */
	inline bool IsVerboseDataStore() const ALWAYS_INLINE;
	
	/* Enable/Disable verbosity of instruction fetch */
	inline bool IsVerboseInstructionFetch() const ALWAYS_INLINE;
	
	/* Enable/Disable verbosity of data bus reads */
	inline bool IsVerboseDataBusRead() const ALWAYS_INLINE;
	
	/* Enable/Disable verbosity of data bus writes */
	inline bool IsVerboseDataBusWrite() const ALWAYS_INLINE;
	
	/* Enable/Disable verbosity of instruction bus reads */
	inline bool IsVerboseInstructionBusRead() const ALWAYS_INLINE;

	/* Test whether storage attribute allow store to be cacheable  */
	inline bool IsStorageCacheable(typename TYPES::STORAGE_ATTR storage_attr) const ALWAYS_INLINE;
	
	/* Test whether storage attribute allow store are dynamically write-through (even with write back caches) */
	inline bool IsDataStoreAccessWriteThrough(typename TYPES::STORAGE_ATTR storage_attr) const ALWAYS_INLINE;
	
	/**** output interface */
	/* Data Bus Read */
	bool DataBusRead(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr, bool rwitm);

	/* Data Bus Write */
	bool DataBusWrite(typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);

	/* Instruction Bus Read */
	bool InstructionBusRead(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Debug Data Bus Read */
	bool DebugDataBusRead(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);

	/* Debug Data Bus Write */
	bool DebugDataBusWrite(typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);

	/* Debug Instruction Bus Read */
	bool DebugInstructionBusRead(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Trap */
	void Trap();

	////////////////////////////// Helper functions ///////////////////////////
	
	/* Load from memory sub-system (local memories then cache hierarchy) */
	template <typename LOC_MEM_SET, typename CACHE_HIERARCHY, typename LOC_MEM, typename CACHE>
	inline bool LoadFromMSS(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Store in memory sub-system (local memories then cache hierarchy) */
	template <typename LOC_MEM_SET, typename CACHE_HIERARCHY, typename LOC_MEM, typename CACHE>
	inline bool StoreInMSS(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);

	/* Debug Data Load from memory sub-system (local memories then cache hierarchy) with address translation through access controller */
	template <typename LOC_MEM_SET, typename CACHE_HIERARCHY, typename LOC_MEM, typename CACHE>
	inline bool DebugLoadFromMSS(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Debug Store in memory sub-system (local memories then cache hierarchy) with address translation through access controller */
	template <typename LOC_MEM_SET, typename CACHE_HIERARCHY, typename LOC_MEM, typename CACHE>
	inline bool DebugStoreInMSS(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);

	/* Debug Load (Data Load or Instruction Fetch) from local memories (returns number of bytes transfered) */
	template <typename LOC_MEM_SET, typename LOC_MEM>
	inline unsigned int DebugLoadFromLocalMemory(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size);
	
	/* Debug Store (Data Store) in local memories (returns number of bytes transfered) */
	template <typename LOC_MEM_SET, typename LOC_MEM>
	inline unsigned int DebugStoreInLocalMemory(typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size);

	/* Incoming Load (Data Load or Instruction Fetch) from local memories (returns number of bytes transfered) */
	template <typename LOC_MEM_SET, typename LOC_MEM>
	inline unsigned int IncomingLoadFromLocalMemory(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size);
	
	/* Incoming Store (Data Store) in local memories (returns number of bytes transfered) */
	template <typename LOC_MEM_SET, typename LOC_MEM>
	inline unsigned int IncomingStoreInLocalMemory(typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size);

	template <typename LOC_MEM_SET, typename LOC_MEM>
	uint8_t *GetDirectAccessToLocalMemory(typename TYPES::PHYSICAL_ADDRESS phys_addr, typename TYPES::PHYSICAL_ADDRESS& start_phys_addr, typename TYPES::PHYSICAL_ADDRESS& end_phys_addr);
	
	/* Load data/instruction from Cache CACHE */
	template <typename CACHE_HIERARCHY, typename CACHE>
	inline bool LoadFromCacheHierarchy(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);

	/* Store data into Cache CACHE. If cache or access is write-through then store also in next level caches, until memory */
	template <typename CACHE_HIERARCHY, typename CACHE>
	inline bool StoreInCacheHierarchy(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Debug Load data/instruction from Cache CACHE or, if missing, in next levels */
	template <typename CACHE_HIERARCHY, typename CACHE>
	bool DebugLoadFromCacheHierarchy(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);

	/* Debug Store data into Cache CACHE or, if missing, in next levels */
	template <typename CACHE_HIERARCHY, typename CACHE>
	bool DebugStoreInCacheHierarchy(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Allocate block in Cache CACHE */
	template <typename CACHE_HIERARCHY, typename CACHE, bool ZEROING>
	bool AllocateBlockInCacheHierarchy(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, typename TYPES::STORAGE_ATTR storage_attr);
	
	/* Fill block from Cache TO with content from Cache FROM. If Cache FROM is disabled, then try with next level caches, until memory */
	template <typename CACHE_HIERARCHY, typename TO, typename FROM>
	bool FillBlock(CacheAccess<TYPES, TO>& to_access);

	/* Write back dirty block from Cache FROM to Cache TO. If Cache TO is disabled, then try with next level caches, until memory */
	template <typename CACHE_HIERARCHY, typename FROM, typename TO, bool INVALIDATE>
	bool WriteBackDirtyBlock(CacheAccess<TYPES, FROM>& from_access, unsigned int from_sector);
	
	/* Write back line from Cache CACHE to first enabled next level cache, or if none are enabled to memory, then optionnally invalidate line in Cache CACHE. */
	template <typename CACHE_HIERARCHY, typename CACHE, bool INVALIDATE>
	bool WriteBackLine(CacheAccess<TYPES, CACHE>& from_access);

	/* Evict line (write back and invalidate) from Cache CACHE to first enabled next level cache, or if none are enabled to memory */
	template <typename CACHE_HIERARCHY, typename CACHE>
	bool EvictLine(CacheAccess<TYPES, CACHE>& from_access);

	/* Lookup set by phys_address */
	template <typename CACHE>
	inline CacheSet<TYPES, typename CACHE::CACHE_CONFIG> *LookupSetByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr) ALWAYS_INLINE;
	
	/* Lookup line by phys_address */
	template <typename CACHE>
	inline CacheLine<TYPES, typename CACHE::CACHE_CONFIG> *LookupLineByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr) ALWAYS_INLINE;
	
	/* Lookup block by phys_address */
	template <typename CACHE>
	inline CacheBlock<TYPES, typename CACHE::CACHE_CONFIG> *LookupBlockByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr) ALWAYS_INLINE;
	
	/* Lookup by phys_address */
	template <typename CACHE>
	inline void Lookup(CacheAccess<TYPES, CACHE>& access) ALWAYS_INLINE;
	
	/* Invalidate block by phys_address */
	template <typename CACHE>
	inline void InvalidateBlockByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr);

	/* Invalidate line by phys_address */
	template <typename CACHE>
	inline void InvalidateLineByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr);
	
	/* Invalidate block by set, way, and sector */
	template <typename CACHE>
	inline void InvalidateBlockBySetWayAndSector(unsigned int index, unsigned int way, unsigned int sector);

	/* Invalidate line by set and way */
	template <typename CACHE>
	inline void InvalidateLineBySetAndWay(unsigned int index, unsigned int way);
	
	/* Invalidate set */
	template <typename CACHE>
	inline void InvalidateSet(unsigned int index);
	
	/* Invalidate all */
	template <typename CACHE>
	inline void InvalidateAll();
	
	/* Invalidate blocks by phys_address in caches [CACHE..UNTIL] */
	template <typename CACHE_HIERARCHY, typename CACHE, typename UNTIL>
	inline void GlobalInvalidateBlockByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr);

	/* Invalidate lines by phys_address in cache [CACHE..UNTIL] */
	template <typename CACHE_HIERARCHY, typename CACHE, typename UNTIL>
	inline void GlobalInvalidateLineByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr);

	/* Write back block by set, way and sector, and optionnally invalidate block */
	template <typename CACHE_HIERARCHY, typename CACHE, bool INVALIDATE>
	inline bool WriteBackBlockBySetWayAndSector(unsigned int index, unsigned int way, unsigned int sector);

	/* Write back block by set and way, and optionnally invalidate line */
	template <typename CACHE_HIERARCHY, typename CACHE, bool INVALIDATE>
	inline bool WriteBackLineBySetAndWay(unsigned int index, unsigned int way);

	/* Write back blocks by phys_address in caches [CACHE..UNTIL], and optionnaly invalidate blocks */
	template <typename CACHE_HIERARCHY, typename CACHE, typename UNTIL, bool INVALIDATE>
	inline bool GlobalWriteBackBlockByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr);

	/* Write back lines by phys_address in caches [CACHE..UNTIL], and optionnaly invalidate lines */
	template <typename CACHE_HIERARCHY, typename CACHE, typename UNTIL, bool INVALIDATE>
	inline bool GlobalWriteBackLineByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr);
private:

	//////////////////// trampoline to overridable methods /////////////////////
	
	template <typename LOC_MEM, bool dummy = true>
	struct __MSS_LOC_MEM_INTERFACE__
	{
		static inline LOC_MEM *GetLocalMemory(MSS *mss, const LOC_MEM *null) ALWAYS_INLINE { return mss->GetLocalMemory(null); }
	};
	
	template <bool dummy>
	struct __MSS_LOC_MEM_INTERFACE__<NullLocalMemory<TYPES>, dummy>
	{
		static inline NullLocalMemory<TYPES> *GetLocalMemory(MSS *mss, const NullLocalMemory<TYPES> *null) ALWAYS_INLINE { return 0; }
	};

	template <bool dummy>
	struct __MSS_LOC_MEM_INTERFACE__<NullLocalMemory1<TYPES>, dummy>
	{
		static inline NullLocalMemory1<TYPES> *GetLocalMemory(MSS *mss, const NullLocalMemory1<TYPES> *null) ALWAYS_INLINE { return 0; }
	};

	template <bool dummy>
	struct __MSS_LOC_MEM_INTERFACE__<NullLocalMemory2<TYPES>, dummy>
	{
		static inline NullLocalMemory2<TYPES> *GetLocalMemory(MSS *mss, const NullLocalMemory2<TYPES> *null) ALWAYS_INLINE { return 0; }
	};

	template <bool dummy>
	struct __MSS_LOC_MEM_INTERFACE__<NullLocalMemory3<TYPES>, dummy>
	{
		static inline NullLocalMemory3<TYPES> *GetLocalMemory(MSS *mss, const NullLocalMemory3<TYPES> *null) ALWAYS_INLINE { return 0; }
	};

	template <bool dummy>
	struct __MSS_LOC_MEM_INTERFACE__<NullLocalMemory4<TYPES>, dummy>
	{
		static inline NullLocalMemory4<TYPES> *GetLocalMemory(MSS *mss, const NullLocalMemory4<TYPES> *null) ALWAYS_INLINE { return 0; }
	};

	template <typename CACHE, bool dummy = true>
	struct __MSS_CACHE_INTERFACE__
	{
		static inline CACHE *GetCache(MSS *mss, const CACHE *null)  ALWAYS_INLINE { return mss->GetCache(null); }
	};
	
	template <bool dummy>
	struct __MSS_CACHE_INTERFACE__<NullCache<TYPES>, dummy>
	{
		static inline NullCache<TYPES> *GetCache(MSS *mss, const NullCache<TYPES> *null) ALWAYS_INLINE { return 0; }
	};

	template <bool dummy>
	struct __MSS_CACHE_INTERFACE__<NullCache1<TYPES>, dummy>
	{
		static inline NullCache1<TYPES> *GetCache(MSS *mss, const NullCache1<TYPES> *null) ALWAYS_INLINE { return 0; }
	};

	template <bool dummy>
	struct __MSS_CACHE_INTERFACE__<NullCache2<TYPES>, dummy>
	{
		static inline NullCache2<TYPES> *GetCache(MSS *mss, const NullCache2<TYPES> *null) ALWAYS_INLINE { return 0; }
	};

	template <bool dummy>
	struct __MSS_CACHE_INTERFACE__<NullCache3<TYPES>, dummy>
	{
		static inline NullCache3<TYPES> *GetCache(MSS *mss, const NullCache3<TYPES> *null) ALWAYS_INLINE { return 0; }
	};

	template <bool dummy>
	struct __MSS_CACHE_INTERFACE__<NullCache4<TYPES>, dummy>
	{
		static inline NullCache4<TYPES> *GetCache(MSS *mss, const NullCache4<TYPES> *null) ALWAYS_INLINE { return 0; }
	};
	
	template <typename CACHE> inline CACHE *__MSS_GetCache__() ALWAYS_INLINE;
	
	template <typename LOC_MEM> inline LOC_MEM *__MSS_GetLocalMemory__() ALWAYS_INLINE;
	
	inline std::ostream& __MSS_GetDebugInfoStream__() ALWAYS_INLINE;
	
	inline std::ostream& __MSS_GetDebugWarningStream__() ALWAYS_INLINE;

	inline std::ostream& __MSS_GetDebugErrorStream__() ALWAYS_INLINE;

	inline bool __MSS_IsStorageCacheable__(typename TYPES::STORAGE_ATTR storage_attr) const ALWAYS_INLINE;
	
	inline bool __MSS_IsDataStoreAccessWriteThrough__(typename TYPES::STORAGE_ATTR storage_attr) const ALWAYS_INLINE;

	inline bool __MSS_IsVerboseDataLoad__() const ALWAYS_INLINE;

	inline bool __MSS_IsVerboseDataStore__() const ALWAYS_INLINE;

	inline bool __MSS_IsVerboseInstructionFetch__() const ALWAYS_INLINE;

	inline bool __MSS_IsVerboseDataBusRead__() const ALWAYS_INLINE;

	inline bool __MSS_IsVerboseDataBusWrite__() const ALWAYS_INLINE;

	inline bool __MSS_IsVerboseInstructionBusRead__() const ALWAYS_INLINE;
	
	inline bool __MSS_DataBusRead__(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr, bool rwitm) ALWAYS_INLINE;

	inline bool __MSS_DataBusWrite__(typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr) ALWAYS_INLINE;
	
	inline bool __MSS_InstructionBusRead__(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr) ALWAYS_INLINE;
	
	inline bool __MSS_DebugDataBusRead__(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr) ALWAYS_INLINE;

	inline bool __MSS_DebugDataBusWrite__(typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr) ALWAYS_INLINE;
	
	inline bool __MSS_DebugInstructionBusRead__(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr) ALWAYS_INLINE;
	
	inline void __MSS_Trap__() ALWAYS_INLINE;

	///////////////////////////////////////////////////////////////////////////
	
protected:
	void Trace(const char *type, typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size);
	void Trace(const char *type, typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size);
	void Check(const char *type, const void *buffer, typename TYPES::PHYSICAL_ADDRESS phys_addr, unsigned int size);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// DEFINITIONS /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

////////////////////// LocalMemoryType //////////////////////////

inline std::ostream& operator << (std::ostream& os, const LocalMemoryType& lmt)
{
	switch(lmt)
	{
		case NULL_LOC_MEM       : os << "null local memory"; break;
		case DATA_LOC_MEM       : os << "data local memory"; break;
		case INSTRUCTION_LOC_MEM: os << "instruction local memory"; break;
		case UNIFIED_LOC_MEM    : os << "unified local memory"; break;
		default: os << "?"; break;
	};
	
	return os;
}

///////////////////////// CacheType /////////////////////////////

inline std::ostream& operator << (std::ostream& os, const CacheType& ct)
{
	switch(ct)
	{
		case NULL_CACHE       : os << "null cache"; break;
		case DATA_CACHE       : os << "data cache"; break;
		case INSTRUCTION_CACHE: os << "instruction cache"; break;
		case UNIFIED_CACHE    : os << "unified cache"; break;
		default: os << "?"; break;
	}
	return os;
}

////////////////////////////// CacheWritingPolicy /////////////////////////////

inline std::ostream& operator << (std::ostream& os, const CacheWritingPolicy& cwp)
{
	switch(cwp)
	{
		case CACHE_WRITE_THROUGH_AND_NO_WRITE_ALLOCATE_POLICY: os << "Write-through and No-Write allocate policy"; break;
		case CACHE_WRITE_BACK_AND_NO_WRITE_ALLOCATE_POLICY   : os << "Write-back and Write allocate policy"; break;
		case CACHE_WRITE_THROUGH_AND_WRITE_ALLOCATE_POLICY   : os << "Write-through and No-Write allocate policy"; break;
		case CACHE_WRITE_BACK_AND_WRITE_ALLOCATE_POLICY      : os << "Write-back and Write allocate policy"; break;
		default: os << "?"; break;
	}
	return os;
}

/////////////////////////////// CacheIndexScheme //////////////////////////////

inline std::ostream& operator << (std::ostream& os, const CacheIndexScheme& cis)
{
	switch(cis)
	{
		case CACHE_VIRTUALLY_INDEXED : os << "Virtually indexed scheme"; break;
		case CACHE_PHYSICALLY_INDEXED: os << "Physically indexed scheme"; break;
		default                      : os << "?"; break;
	}
	return os;
}

//////////////////////////////// CacheTagScheme ///////////////////////////////

inline std::ostream& operator << (std::ostream& os, const CacheTagScheme& cts)
{
	switch(cts)
	{
		case CACHE_VIRTUALLY_TAGGED : os << "Virtually tagged scheme"; break;
		case CACHE_PHYSICALLY_TAGGED: os << "Physically tagged scheme"; break;
		default                     : os << "?"; break;
	}
	return os;
}

////////////////////////////// LocalMemory<> //////////////////////////////////

template <typename TYPES, typename CONFIG, typename LOC_MEM>
LocalMemory<TYPES, CONFIG, LOC_MEM>::LocalMemory()
	: num_load_accesses(0)
	, num_store_accesses(0)
	, bytes()
{
	memset(bytes, 0, CONFIG::SIZE);
}

template <typename TYPES, typename CONFIG, typename LOC_MEM>
LocalMemory<TYPES, CONFIG, LOC_MEM>::~LocalMemory()
{
}

template <typename TYPES, typename CONFIG, typename LOC_MEM>
inline uint8_t& LocalMemory<TYPES, CONFIG, LOC_MEM>::GetByteByOffset(unsigned int offset)
{
	assert(offset < __MSS_GetSize__());
	return bytes[offset];
}

template <typename TYPES, typename CONFIG, typename LOC_MEM>
inline uint8_t& LocalMemory<TYPES, CONFIG, LOC_MEM>::operator [] (unsigned int offset)
{
	return GetByteByOffset(offset);
}

template <typename TYPES, typename CONFIG, typename LOC_MEM>
inline bool LocalMemory<TYPES, CONFIG, LOC_MEM>::IsNullLocalMemory()
{
	return CONFIG::TYPE == NULL_LOC_MEM;
}

template <typename TYPES, typename CONFIG, typename LOC_MEM>
inline bool LocalMemory<TYPES, CONFIG, LOC_MEM>::IsDataLocalMemoryOnly()
{
	return CONFIG::TYPE == DATA_LOC_MEM;
}

template <typename TYPES, typename CONFIG, typename LOC_MEM>
inline bool LocalMemory<TYPES, CONFIG, LOC_MEM>::IsDataLocalMemory()
{
	return CONFIG::TYPE & DATA_LOC_MEM;
}

template <typename TYPES, typename CONFIG, typename LOC_MEM>
inline bool LocalMemory<TYPES, CONFIG, LOC_MEM>::IsInstructionLocalMemoryOnly()
{
	return CONFIG::TYPE == INSTRUCTION_LOC_MEM;
}

template <typename TYPES, typename CONFIG, typename LOC_MEM>
inline bool LocalMemory<TYPES, CONFIG, LOC_MEM>::IsInstructionLocalMemory()
{
	return CONFIG::TYPE & INSTRUCTION_LOC_MEM;
}

template <typename TYPES, typename CONFIG, typename LOC_MEM>
inline bool LocalMemory<TYPES, CONFIG, LOC_MEM>::IsUnifiedLocalMemory()
{
	return CONFIG::TYPE == UNIFIED_LOC_MEM;
}

template <typename TYPES, typename CONFIG, typename LOC_MEM>
bool LocalMemory<TYPES, CONFIG, LOC_MEM>::IsVerbose() const
{
	return false;
}

template <typename TYPES, typename CONFIG, typename LOC_MEM>
typename TYPES::PHYSICAL_ADDRESS LocalMemory<TYPES, CONFIG, LOC_MEM>::GetBasePhysicalAddress() const
{
	return CONFIG::BASE_ADDRESS;
}

template <typename TYPES, typename CONFIG, typename LOC_MEM>
inline unsigned int LocalMemory<TYPES, CONFIG, LOC_MEM>::GetSize() const
{
	return CONFIG::SIZE;
}

template <typename TYPES, typename CONFIG, typename LOC_MEM>
inline const char *LocalMemory<TYPES, CONFIG, LOC_MEM>::__MSS_GetLocalMemoryName__()
{
	return LOC_MEM::GetLocalMemoryName();
}

template <typename TYPES, typename CONFIG, typename LOC_MEM>
inline bool LocalMemory<TYPES, CONFIG, LOC_MEM>::__MSS_IsVerbose__() const
{
	return static_cast<const LOC_MEM *>(this)->IsVerbose();
}

template <typename TYPES, typename CONFIG, typename LOC_MEM>
inline typename TYPES::PHYSICAL_ADDRESS LocalMemory<TYPES, CONFIG, LOC_MEM>::__MSS_GetBasePhysicalAddress__() const
{
	return static_cast<const LOC_MEM *>(this)->GetBasePhysicalAddress();
}

template <typename TYPES, typename CONFIG, typename LOC_MEM>
inline unsigned int LocalMemory<TYPES, CONFIG, LOC_MEM>::__MSS_GetSize__() const
{
	unsigned int size = static_cast<const LOC_MEM *>(this)->GetSize();
	return (size < CONFIG::SIZE) ? size : CONFIG::SIZE;
}

/////////////////////////////// CacheBlock<> //////////////////////////////////

template <typename TYPES, typename CONFIG>
CacheBlock<TYPES, CONFIG>::CacheBlock()
	: base_addr(0)
	, base_phys_addr(0)
	, status()
	, sector(0)
	, storage()
{
	memset(storage, 0, CONFIG::BLOCK_SIZE);
}

template <typename TYPES, typename CONFIG>
CacheBlock<TYPES, CONFIG>::~CacheBlock()
{
}

template <typename TYPES, typename CONFIG>
inline uint8_t& CacheBlock<TYPES, CONFIG>::GetByteByOffset(unsigned int offset)
{
	assert(offset < CONFIG::BLOCK_SIZE);
	return storage[offset];
}

template <typename TYPES, typename CONFIG>
inline uint8_t& CacheBlock<TYPES, CONFIG>::operator [] (unsigned int offset)
{
	return GetByteByOffset(offset);
}

template <typename TYPES, typename CONFIG>
inline void CacheBlock<TYPES, CONFIG>::SetBaseAddress(typename TYPES::ADDRESS addr)
{
	base_addr = addr;
}

template <typename TYPES, typename CONFIG>
inline void CacheBlock<TYPES, CONFIG>::SetBasePhysicalAddress(typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	base_phys_addr = phys_addr;
}

template <typename TYPES, typename CONFIG>
inline typename TYPES::ADDRESS CacheBlock<TYPES, CONFIG>::GetBaseAddress() const
{
	return base_addr;
}

template <typename TYPES, typename CONFIG>
inline typename TYPES::PHYSICAL_ADDRESS CacheBlock<TYPES, CONFIG>::GetBasePhysicalAddress() const
{
	return base_phys_addr;
}

template <typename TYPES, typename CONFIG>
inline void CacheBlock<TYPES, CONFIG>::Zero()
{
	memset(storage, 0, CONFIG::BLOCK_SIZE);
}

template <typename TYPES, typename CONFIG>
inline void CacheBlock<TYPES, CONFIG>::Write(const void *buffer, unsigned int offset, unsigned int size)
{
	assert(CONFIG::BLOCK_SIZE >= (offset + size));
	memcpy(storage + offset, buffer, size);
}

template <typename TYPES, typename CONFIG>
inline void CacheBlock<TYPES, CONFIG>::Read(void *buffer, unsigned int offset, unsigned int size)
{
	assert(CONFIG::BLOCK_SIZE >= (offset + size));
	memcpy(buffer, storage + offset, size);
}

template <typename TYPES, typename CONFIG>
inline bool CacheBlock<TYPES, CONFIG>::IsValid() const
{
	return status.IsValid();
}

template <typename TYPES, typename CONFIG>
inline bool CacheBlock<TYPES, CONFIG>::IsDirty() const
{
	return status.IsDirty();
}

template <typename TYPES, typename CONFIG>
inline void CacheBlock<TYPES, CONFIG>::SetValid(bool v)
{
	status.SetValid(v);
}

template <typename TYPES, typename CONFIG>
inline void CacheBlock<TYPES, CONFIG>::SetDirty(bool v)
{
	status.SetDirty(v);
}

template <typename TYPES, typename CONFIG>
inline void CacheBlock<TYPES, CONFIG>::Invalidate()
{
	status.Invalidate();
}

template <typename TYPES, typename CONFIG>
inline typename CONFIG::BLOCK_STATUS& CacheBlock<TYPES, CONFIG>::Status()
{
	return status;
}

template <typename TYPES, typename CONFIG>
void CacheBlock<TYPES, CONFIG>::SetSector(unsigned int _sector)
{
	sector = _sector;
}

template <typename TYPES, typename CONFIG>
inline unsigned int CacheBlock<TYPES, CONFIG>::GetSector() const
{
	return sector;
}

/////////////////////////////// CacheLine<> ///////////////////////////////////

template <typename TYPES, typename CONFIG>
CacheLine<TYPES, CONFIG>::CacheLine()
	: valid(false)
	, next(0)
	, prev(0)
	, base_addr(0)
	, base_phys_addr(0)
	, way(0)
	, status()
	, blocks()
{
	unsigned int sector;
	for(sector = 0; sector < CONFIG::BLOCKS_PER_LINE; sector++)
	{
		blocks[sector].SetSector(sector);
	}
}

template <typename TYPES, typename CONFIG>
CacheLine<TYPES, CONFIG>::~CacheLine()
{
}

template <typename TYPES, typename CONFIG>
inline typename TYPES::ADDRESS CacheLine<TYPES, CONFIG>::GetBaseAddress() const
{
	return base_addr;
}

template <typename TYPES, typename CONFIG>
inline typename TYPES::PHYSICAL_ADDRESS CacheLine<TYPES, CONFIG>::GetBasePhysicalAddress() const
{
	return base_phys_addr;
}

template <typename TYPES, typename CONFIG>
inline void CacheLine<TYPES, CONFIG>::SetWay(unsigned int _way)
{
	way = _way;
}

template <typename TYPES, typename CONFIG>
inline void CacheLine<TYPES, CONFIG>::SetBaseAddress(typename TYPES::ADDRESS base_addr)
{
	this->base_addr = base_addr;
	unsigned int sector;
	for(sector = 0; sector < CONFIG::BLOCKS_PER_LINE; sector++)
	{
		blocks[sector].SetBaseAddress(base_addr + (sector * CONFIG::BLOCK_SIZE));
	}
}

template <typename TYPES, typename CONFIG>
inline void CacheLine<TYPES, CONFIG>::SetBasePhysicalAddress(typename TYPES::PHYSICAL_ADDRESS base_phys_addr)
{
	this->base_phys_addr = base_phys_addr;
	unsigned int sector;
	for(sector = 0; sector < CONFIG::BLOCKS_PER_LINE; sector++)
	{
		blocks[sector].SetBasePhysicalAddress(base_phys_addr + (sector * CONFIG::BLOCK_SIZE));
	}
}

template <typename TYPES, typename CONFIG>
inline CacheBlock<TYPES, CONFIG>& CacheLine<TYPES, CONFIG>::GetBlockBySector(unsigned int sector)
{
	return blocks[sector % CONFIG::BLOCKS_PER_LINE];
}

template <typename TYPES, typename CONFIG>
inline CacheBlock<TYPES, CONFIG>& CacheLine<TYPES, CONFIG>::operator [] (unsigned int sector)
{
	return GetBlockBySector(sector);
}

template <typename TYPES, typename CONFIG>
inline unsigned int CacheLine<TYPES, CONFIG>::GetWay() const
{
	return way;
}

template <typename TYPES, typename CONFIG>
inline bool CacheLine<TYPES, CONFIG>::IsValid() const
{
	return valid;
}

template <typename TYPES, typename CONFIG>
inline void CacheLine<TYPES, CONFIG>::SetValid()
{
	valid = true;
}

template <typename TYPES, typename CONFIG>
inline void CacheLine<TYPES, CONFIG>::Invalidate()
{
	unsigned int sector;

	for(sector = 0; sector < CONFIG::BLOCKS_PER_LINE; sector++)
	{
		CacheBlock<TYPES, CONFIG>& block = blocks[sector];

		block.Invalidate();
	}
	
	valid = false;
	base_phys_addr = 0;
	status.Invalidate();
}

template <typename TYPES, typename CONFIG>
inline typename CONFIG::LINE_STATUS& CacheLine<TYPES, CONFIG>::Status()
{
	return status;
}

//////////////////////////////// CacheSet<> ///////////////////////////////////

template <typename TYPES, typename CONFIG>
CacheSet<TYPES, CONFIG>::CacheSet()
	: status()
	, mrs_line(0)
	, lrs_line(0)
	, lines()
	, index(0)
{
	unsigned int way;
	
	for(way = 0; way < CONFIG::ASSOCIATIVITY; way++)
	{
		CacheLine<TYPES, CONFIG>& line = lines[way];
		line.SetWay(way);
		
		line.prev = (way > 0) ? &lines[way - 1] : 0;
		line.next = (way < (CONFIG::ASSOCIATIVITY - 1)) ? &lines[way + 1] : 0;
	}
	
	mrs_line = &lines[0];
	lrs_line = &lines[CONFIG::ASSOCIATIVITY - 1];
}

template <typename TYPES, typename CONFIG>
CacheSet<TYPES, CONFIG>::~CacheSet()
{
}

template <typename TYPES, typename CONFIG>
inline CacheLine<TYPES, CONFIG>& CacheSet<TYPES, CONFIG>::GetLineByWay(unsigned int way)
{
	return lines[way % CONFIG::ASSOCIATIVITY];
}

template <typename TYPES, typename CONFIG>
inline CacheLine<TYPES, CONFIG>& CacheSet<TYPES, CONFIG>::operator [] (unsigned int way)
{
	return GetLineByWay(way);
}

template <typename TYPES, typename CONFIG>
inline void CacheSet<TYPES, CONFIG>::InvalidateLineByWay(unsigned int way)
{
	CacheLine<TYPES, CONFIG>& line = lines[way % CONFIG::ASSOCIATIVITY];
	line.Invalidate();
}

template <typename TYPES, typename CONFIG>
inline void CacheSet<TYPES, CONFIG>::Invalidate()
{
	unsigned int way;

	for(way = 0; way < CONFIG::ASSOCIATIVITY; way++)
	{
		CacheLine<TYPES, CONFIG>& line = lines[way];
		
		line.Invalidate();

		line.prev = (way > 0) ? &lines[way - 1] : 0;
		line.next = (way < (CONFIG::ASSOCIATIVITY - 1)) ? &lines[way + 1] : 0;
	}
	mrs_line = &lines[0];
	lrs_line = &lines[CONFIG::ASSOCIATIVITY - 1];
	
	status.Invalidate();
}

template <typename TYPES, typename CONFIG>
inline void CacheSet<TYPES, CONFIG>::SetIndex(unsigned int _index)
{
	index = _index;
}

template <typename TYPES, typename CONFIG>
inline unsigned int CacheSet<TYPES, CONFIG>::GetIndex() const
{
	return index;
}

template <typename TYPES, typename CONFIG>
inline typename CONFIG::SET_STATUS& CacheSet<TYPES, CONFIG>::Status()
{
	return status;
}

template <typename TYPES, typename CONFIG>
inline CacheLine<TYPES, CONFIG> *CacheSet<TYPES, CONFIG>::AssociativeSearchByLineBasePhysicalAddress(typename TYPES::PHYSICAL_ADDRESS line_base_phys_addr)
{
	CacheLine<TYPES, CONFIG> *line;
	
	// Associative search from most recently searched to least recently searched
	for(line = mrs_line; line != 0; line = line->next)
	{
		if(likely(line->IsValid() && (line->GetBasePhysicalAddress() == line_base_phys_addr)))
		{
			// Hit
			
			// Update MRS/LRS scheme for fast line search
			if(unlikely(line->prev != 0))
			{
				if(line->next)
				{
					line->next->prev = line->prev;
				}
				else
				{
					lrs_line = line->prev;
				}
				line->prev->next = line->next;
				line->prev = 0;
				line->next = mrs_line;
				mrs_line->prev = line;
				mrs_line = line;
			}
			
			return line;
		}
	}
	
	return 0;
}

template <typename TYPES, typename CONFIG>
inline CacheLine<TYPES, CONFIG> *CacheSet<TYPES, CONFIG>::AssociativeSearchByLineBaseAddress(typename TYPES::ADDRESS line_base_addr)
{
	CacheLine<TYPES, CONFIG> *line;
	
	// Associative search from most recently searched to least recently searched
	for(line = mrs_line; line != 0; line = line->next)
	{
		if(likely(line->IsValid() && (line->GetBaseAddress() == line_base_addr)))
		{
			// Hit
			
			// Update MRS/LRS scheme for fast line search
			if(unlikely(line->prev != 0))
			{
				if(line->next)
				{
					line->next->prev = line->prev;
				}
				else
				{
					lrs_line = line->prev;
				}
				line->prev->next = line->next;
				line->prev = 0;
				line->next = mrs_line;
				mrs_line->prev = line;
				mrs_line = line;
			}
			
			return line;
		}
	}
	
	return 0;
}

////////////////////////////////// Cache<> ////////////////////////////////////

template <typename TYPES, typename CONFIG, typename CACHE>
Cache<TYPES, CONFIG, CACHE>::Cache()
	: num_accesses(0)
	, num_misses(0)
	, sets()
{
	unsigned int index;
	for(index = 0; index < (CONFIG::SIZE / CONFIG::BLOCK_SIZE / CONFIG::BLOCKS_PER_LINE / CONFIG::ASSOCIATIVITY); index++)
	{
		sets[index].SetIndex(index);
	}
}

template <typename TYPES, typename CONFIG, typename CACHE>
Cache<TYPES, CONFIG, CACHE>::~Cache()
{
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline typename TYPES::ADDRESS Cache<TYPES, CONFIG, CACHE>::LineBaseAddress(typename TYPES::ADDRESS addr)
{
	return addr & ~((CONFIG::BLOCK_SIZE * CONFIG::BLOCKS_PER_LINE) - 1);
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline typename TYPES::PHYSICAL_ADDRESS Cache<TYPES, CONFIG, CACHE>::LineBasePhysicalAddress(typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	return phys_addr & ~((CONFIG::BLOCK_SIZE * CONFIG::BLOCKS_PER_LINE) - 1);
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline typename TYPES::ADDRESS Cache<TYPES, CONFIG, CACHE>::BlockBaseAddress(typename TYPES::ADDRESS addr)
{
	return addr & ~(CONFIG::BLOCK_SIZE - 1);
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline typename TYPES::PHYSICAL_ADDRESS Cache<TYPES, CONFIG, CACHE>::BlockBasePhysicalAddress(typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	return phys_addr & ~(CONFIG::BLOCK_SIZE - 1);
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline unsigned int Cache<TYPES, CONFIG, CACHE>::SectorByAddress(typename TYPES::ADDRESS addr)
{
	return (addr / CONFIG::BLOCK_SIZE) % CONFIG::BLOCKS_PER_LINE;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline unsigned int Cache<TYPES, CONFIG, CACHE>::SectorByPhysicalAddress(typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	return (phys_addr / CONFIG::BLOCK_SIZE) % CONFIG::BLOCKS_PER_LINE;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline unsigned int Cache<TYPES, CONFIG, CACHE>::IndexByAddress(typename TYPES::ADDRESS addr)
{
	return (addr % CONFIG::SIZE) / CONFIG::BLOCK_SIZE / CONFIG::BLOCKS_PER_LINE / CONFIG::ASSOCIATIVITY;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline unsigned int Cache<TYPES, CONFIG, CACHE>::IndexByPhysicalAddress(typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	return (phys_addr % CONFIG::SIZE) / CONFIG::BLOCK_SIZE / CONFIG::BLOCKS_PER_LINE / CONFIG::ASSOCIATIVITY;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline void Cache<TYPES, CONFIG, CACHE>::DecodeAddress(CacheAccess<TYPES, CACHE>& access)
{
	access.offset = access.addr % CONFIG::BLOCK_SIZE;
// 	access.offset = access.phys_addr % CONFIG::BLOCK_SIZE;
	access.block_base_addr = BlockBaseAddress(access.addr);
	access.block_base_phys_addr = BlockBasePhysicalAddress(access.phys_addr);
	access.size_to_block_boundary = CONFIG::BLOCK_SIZE - access.offset;
	access.line_base_addr = LineBaseAddress(access.addr);
	access.line_base_phys_addr = LineBasePhysicalAddress(access.phys_addr);
	access.sector = SectorByAddress(access.addr);
// 	access.sector = (access.phys_addr / CONFIG::BLOCK_SIZE) % CONFIG::BLOCKS_PER_LINE;
// 	access.index = (access.phys_addr % CONFIG::SIZE) / CONFIG::BLOCK_SIZE / CONFIG::BLOCKS_PER_LINE / CONFIG::ASSOCIATIVITY;
	switch(CONFIG::INDEX_SCHEME)
	{
		case CACHE_VIRTUALLY_INDEXED : access.index = IndexByAddress(access.addr);              break;
		case CACHE_PHYSICALLY_INDEXED: access.index = IndexByPhysicalAddress(access.phys_addr); break;
	}
}

// template <typename TYPES, typename CONFIG, typename CACHE>
// inline void Cache<TYPES, CONFIG, CACHE>::DecodeAddress(typename TYPES::PHYSICAL_ADDRESS phys_addr, typename TYPES::PHYSICAL_ADDRESS& line_base_phys_addr, typename TYPES::PHYSICAL_ADDRESS& block_base_phys_addr, unsigned int& index, unsigned int& sector, unsigned int& offset, unsigned int& size_to_block_boundary)
// {
// 	offset = phys_addr % CONFIG::BLOCK_SIZE;
// 	block_base_phys_addr = phys_addr & ~(CONFIG::BLOCK_SIZE - 1);
// 	size_to_block_boundary = CONFIG::BLOCK_SIZE - offset;
// 	line_base_phys_addr = phys_addr & ~((CONFIG::BLOCK_SIZE * CONFIG::BLOCKS_PER_LINE) - 1);
// 	sector = (phys_addr / CONFIG::BLOCK_SIZE) % CONFIG::BLOCKS_PER_LINE;
// 	index = (phys_addr % CONFIG::SIZE) / CONFIG::BLOCK_SIZE / CONFIG::BLOCKS_PER_LINE / CONFIG::ASSOCIATIVITY;
// }

template <typename TYPES, typename CONFIG, typename CACHE>
inline CacheSet<TYPES, CONFIG>& Cache<TYPES, CONFIG, CACHE>::GetSetByIndex(unsigned int index)
{
	return sets[index % (CONFIG::SIZE / CONFIG::BLOCK_SIZE / CONFIG::BLOCKS_PER_LINE / CONFIG::ASSOCIATIVITY)];
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline CacheSet<TYPES, CONFIG>& Cache<TYPES, CONFIG, CACHE>::operator [] (unsigned int index)
{
	return GetSetByIndex(index);
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline void Cache<TYPES, CONFIG, CACHE>::InvalidateSet(unsigned int index)
{
	CacheSet<TYPES, CONFIG>& set = sets[index];

	set.Invalidate();
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline void Cache<TYPES, CONFIG, CACHE>::InvalidateLineBySetAndWay(unsigned int index, unsigned int way)
{
	CacheSet<TYPES, CONFIG>& set = sets[index % (CONFIG::SIZE / CONFIG::BLOCK_SIZE / CONFIG::BLOCKS_PER_LINE / CONFIG::ASSOCIATIVITY)];
	
	set.InvalidateLineByWay(way);
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline void Cache<TYPES, CONFIG, CACHE>::Invalidate()
{
	unsigned int index;
	
	for(index = 0; index < (CONFIG::SIZE / CONFIG::BLOCK_SIZE / CONFIG::BLOCKS_PER_LINE / CONFIG::ASSOCIATIVITY); index++)
	{
		CacheSet<TYPES, CONFIG>& set = sets[index];
		set.Invalidate();
	}
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline CacheSet<TYPES, CONFIG> *Cache<TYPES, CONFIG, CACHE>::LookupSetByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	unsigned int index = 0;
	switch(CONFIG::TAG_SCHEME)
	{
		case CACHE_VIRTUALLY_TAGGED:
			index = IndexByAddress(addr);
			break;
		case CACHE_PHYSICALLY_TAGGED:
			index = IndexByPhysicalAddress(phys_addr);
			break;
	}
	CacheSet<TYPES, CONFIG> *set = &sets[index];
	
	return set;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline CacheLine<TYPES, CONFIG> *Cache<TYPES, CONFIG, CACHE>::LookupLineByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	CacheSet<TYPES, CONFIG> *set = LookupSetByAddress(addr, phys_addr);
	CacheLine<TYPES, CONFIG> *line = 0;
	switch(CONFIG::TAG_SCHEME)
	{
		case CACHE_VIRTUALLY_TAGGED:
			line = set->AssociativeSearchByLineBaseAddress(LineBaseAddress(addr));
			break;
		case CACHE_PHYSICALLY_TAGGED:
			line = set->AssociativeSearchByLineBasePhysicalAddress(LineBasePhysicalAddress(phys_addr));
			break;
	}
	
	return line;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline CacheBlock<TYPES, CONFIG> *Cache<TYPES, CONFIG, CACHE>::LookupBlockByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	unsigned int sector = SectorByPhysicalAddress(phys_addr);
	CacheLine<TYPES, CONFIG> *line = LookupLineByAddress(addr, phys_addr);
	
	if(line)
	{
		CacheBlock<TYPES, CONFIG> *block = &line->blocks[sector];
		
		return block->IsValid() ? block : 0;
	}
	
	return 0;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline void Cache<TYPES, CONFIG, CACHE>::Lookup(CacheAccess<TYPES, CACHE>& access)
{
	CacheSet<TYPES, CONFIG> *set;
	CacheLine<TYPES, CONFIG> *line;
	CacheBlock<TYPES, CONFIG> *block;
	
	num_accesses++;
	// Decode the phys_address
	DecodeAddress(access);
	access.set = set = &sets[access.index];
	access.cache = this;

	switch(CONFIG::TAG_SCHEME)
	{
		case CACHE_VIRTUALLY_TAGGED : line = set->AssociativeSearchByLineBaseAddress(access.line_base_addr);              break;
		case CACHE_PHYSICALLY_TAGGED: line = set->AssociativeSearchByLineBasePhysicalAddress(access.line_base_phys_addr); break;
	}
	
	if(line)
	{
		// line hit: block may need a fill if not yet present in the line
		access.line = line;
		access.line_to_write_back_evict = 0;
		access.way = line->GetWay();
		block = &line->GetBlockBySector(access.sector);
		access.block = block->IsValid() ? block : 0;
	}
	else
	{
		// line miss
		access.line = 0;
		access.block = 0;
		num_misses++;
	}
	
	this->__MSS_OnAccess__(access);
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::IsVerbose() const
{
	return false;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::IsEnabled() const
{
	return false;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::IsWriteAllocate(typename TYPES::STORAGE_ATTR storage_attr) const
{
	return false;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::ChooseLineToEvict(CacheAccess<TYPES, CACHE>& access)
{
	return false;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline void Cache<TYPES, CONFIG, CACHE>::UpdateReplacementPolicyOnAccess(CacheAccess<TYPES, CACHE>& access)
{
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline void Cache<TYPES, CONFIG, CACHE>::UpdateReplacementPolicyOnFill(CacheAccess<TYPES, CACHE>& access)
{
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline void Cache<TYPES, CONFIG, CACHE>::OnAccess(CacheAccess<TYPES, CACHE>& access)
{
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline const char *Cache<TYPES, CONFIG, CACHE>::__MSS_GetCacheName__()
{
	return CACHE::GetCacheName();
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::__MSS_IsVerbose__() const
{
	return static_cast<const CACHE *>(this)->IsVerbose();
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::__MSS_IsEnabled__() const
{
	return static_cast<const CACHE *>(this)->IsEnabled();
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::__MSS_IsWriteAllocate__(typename TYPES::STORAGE_ATTR storage_attr) const
{
	return CACHE::IsStaticWriteAllocate() || static_cast<const CACHE *>(this)->IsWriteAllocate(storage_attr);
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::__MSS_ChooseLineToEvict__(CacheAccess<TYPES, CACHE>& access)
{
	if(likely(static_cast<CACHE *>(this)->ChooseLineToEvict(access)))
	{
		access.line_to_write_back_evict = &access.set->GetLineByWay(access.way);
		return true;
	}
	return false;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline void Cache<TYPES, CONFIG, CACHE>::__MSS_UpdateReplacementPolicyOnAccess__(CacheAccess<TYPES, CACHE>& access)
{
	static_cast<CACHE *>(this)->UpdateReplacementPolicyOnAccess(access);
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline void Cache<TYPES, CONFIG, CACHE>::__MSS_UpdateReplacementPolicyOnFill__(CacheAccess<TYPES, CACHE>& access)
{
	static_cast<CACHE *>(this)->UpdateReplacementPolicyOnFill(access);
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline void Cache<TYPES, CONFIG, CACHE>::__MSS_OnAccess__(CacheAccess<TYPES, CACHE>& access)
{
	static_cast<CACHE *>(this)->OnAccess(access);
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::IsNullCache()
{
	return CONFIG::TYPE == NULL_CACHE;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::IsDataCacheOnly()
{
	return CONFIG::TYPE == DATA_CACHE;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::IsDataCache()
{
	return CONFIG::TYPE & DATA_CACHE;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::IsInstructionCacheOnly()
{
	return CONFIG::TYPE == INSTRUCTION_CACHE;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::IsInstructionCache()
{
	return CONFIG::TYPE & INSTRUCTION_CACHE;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::IsUnifiedCache()
{
	return CONFIG::TYPE == UNIFIED_CACHE;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::IsStaticWriteBackCache()
{
	return CONFIG::WRITING_POLICY & CACHE_WRITE_BACK;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::IsStaticWriteThroughCache()
{
	return !(CONFIG::WRITING_POLICY & CACHE_WRITE_BACK);
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::IsStaticWriteAllocate()
{
	return CONFIG::WRITING_POLICY & CACHE_WRITE_ALLOCATE;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline bool Cache<TYPES, CONFIG, CACHE>::IsStaticNotWriteAllocate()
{
	return !(CONFIG::WRITING_POLICY & CACHE_WRITE_ALLOCATE);
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline unsigned int Cache<TYPES, CONFIG, CACHE>::NumSets()
{
	return CONFIG::SIZE / CONFIG::BLOCK_SIZE / CONFIG::BLOCKS_PER_LINE / CONFIG::ASSOCIATIVITY;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline unsigned int Cache<TYPES, CONFIG, CACHE>::NumWays()
{
	return CONFIG::ASSOCIATIVITY;
}

template <typename TYPES, typename CONFIG, typename CACHE>
inline unsigned int Cache<TYPES, CONFIG, CACHE>::NumSectors()
{
	return CONFIG::BLOCKS_PER_LINE;
}

////////////////////////////// MemorySubSystem<> ////////////////////////////////

template <typename TYPES, typename MSS>
MemorySubSystem<TYPES, MSS>::MemorySubSystem()
	: num_data_load_accesses(0)
	, num_data_store_accesses(0)
	, num_instruction_fetch_accesses(0)
	, num_incoming_load_accesses(0)
	, num_incoming_store_accesses(0)
	, num_data_bus_read_accesses(0)
	, num_data_bus_write_accesses(0)
	, num_instruction_bus_read_accesses(0)
	, num_data_load_xfered_bytes(0)
	, num_data_store_xfered_bytes(0)
	, num_instruction_fetch_xfered_bytes(0)
	, num_incoming_load_xfered_bytes(0)
	, num_incoming_store_xfered_bytes(0)
	, num_data_bus_read_xfered_bytes(0)
	, num_data_bus_write_xfered_bytes(0)
	, num_instruction_bus_read_xfered_bytes(0)
	, enable_shadow_memory(false)
	, shadow_memory()
{
}

template <typename TYPES, typename MSS>
MemorySubSystem<TYPES, MSS>::~MemorySubSystem()
{
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DataLoad(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	if(unlikely((!LoadFromMSS<typename MSS::DATA_LOCAL_MEMORIES, typename MSS::DATA_CACHE_HIERARCHY, typename MSS::DATA_LOCAL_MEMORIES::LOC_MEM1, typename MSS::DATA_CACHE_HIERARCHY::L1CACHE>(addr, phys_addr, buffer, size, storage_attr)))) return false;

	if(unlikely(__MSS_IsVerboseDataLoad__())) Trace("Loading Data", addr, phys_addr, buffer, size);
		
	num_data_load_accesses++;
	num_data_load_xfered_bytes += size;
	
	if(unlikely(enable_shadow_memory))
	{
		Check("Data Load", (const void *) buffer, phys_addr, size);
	}
		
	return true;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DataStore(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	if(unlikely(__MSS_IsVerboseDataStore__())) Trace("Storing Data", addr, phys_addr, buffer, size);
	
	if(unlikely((!StoreInMSS<typename MSS::DATA_LOCAL_MEMORIES, typename MSS::DATA_CACHE_HIERARCHY, typename MSS::DATA_LOCAL_MEMORIES::LOC_MEM1, typename MSS::DATA_CACHE_HIERARCHY::L1CACHE>(addr, phys_addr, buffer, size, storage_attr)))) return false;
	
	num_data_store_accesses++;
	num_data_store_xfered_bytes += size;
	
	if(unlikely(enable_shadow_memory))
	{
		shadow_memory.write(phys_addr, (const uint8_t *) buffer, size);
	}
	
	return true;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::InstructionFetch(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	if(unlikely((!LoadFromMSS<typename MSS::INSTRUCTION_LOCAL_MEMORIES, typename MSS::INSTRUCTION_CACHE_HIERARCHY, typename MSS::INSTRUCTION_LOCAL_MEMORIES::LOC_MEM1, typename MSS::INSTRUCTION_CACHE_HIERARCHY::L1CACHE>(addr, phys_addr, buffer, size, storage_attr)))) return false;
	
	if(unlikely(__MSS_IsVerboseInstructionFetch__())) Trace("Fetching Instruction", addr, phys_addr, buffer, size);
	
	num_instruction_fetch_accesses++;
	num_instruction_fetch_xfered_bytes += size;
	
	if(unlikely(enable_shadow_memory))
	{
		Check("Instruction Fetch", (const void *) buffer, phys_addr, size);
	}

	return true;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DebugDataLoad(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	if(!DebugLoadFromMSS<typename MSS::DATA_LOCAL_MEMORIES, typename MSS::DATA_CACHE_HIERARCHY, typename MSS::DATA_LOCAL_MEMORIES::LOC_MEM1, typename MSS::DATA_CACHE_HIERARCHY::L1CACHE>(addr, phys_addr, buffer, size, storage_attr)) return false;
	
	if(unlikely(enable_shadow_memory))
	{
		Check("Debug Data load", (const void *) buffer, phys_addr, size);
	}
	
	return true;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DebugDataStore(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	if(!DebugStoreInMSS<typename MSS::DATA_LOCAL_MEMORIES, typename MSS::DATA_CACHE_HIERARCHY, typename MSS::DATA_LOCAL_MEMORIES::LOC_MEM1, typename MSS::DATA_CACHE_HIERARCHY::L1CACHE>(addr, phys_addr, buffer, size, storage_attr)) return false;

	if(unlikely(enable_shadow_memory))
	{
		shadow_memory.write(phys_addr, (const uint8_t *) buffer, size);
	}
	
	return true;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DebugInstructionFetch(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	if(!DebugLoadFromMSS<typename MSS::INSTRUCTION_LOCAL_MEMORIES, typename MSS::INSTRUCTION_CACHE_HIERARCHY, typename MSS::INSTRUCTION_LOCAL_MEMORIES::LOC_MEM1, typename MSS::INSTRUCTION_CACHE_HIERARCHY::L1CACHE>(addr, phys_addr, buffer, size, storage_attr)) return false;
	
	if(unlikely(enable_shadow_memory))
	{
		Check("Debug Instruction Fetch", (const void *) buffer, phys_addr, size);
	}

	return true;
}

template <typename TYPES, typename MSS>
unsigned int MemorySubSystem<TYPES, MSS>::IncomingLoad(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size)
{
	unsigned int read_bytes = 0;
	
	if(size)
	{
		do
		{
			unsigned int sz = 0;
			
			sz = IncomingLoadFromLocalMemory<typename MSS::INSTRUCTION_LOCAL_MEMORIES, typename MSS::INSTRUCTION_LOCAL_MEMORIES::LOC_MEM1>(phys_addr, buffer, size);
			
			if(!sz)
			{
				sz = IncomingLoadFromLocalMemory<typename MSS::DATA_LOCAL_MEMORIES, typename MSS::DATA_LOCAL_MEMORIES::LOC_MEM1>(phys_addr, buffer, size);
			
				if(!sz)
				{
					return read_bytes;
				}
			}
			
			buffer = (uint8_t *) buffer + sz;
			phys_addr += sz;
			size -= sz;
			read_bytes += sz;
		}
		while(size);
	}
	
	num_incoming_load_accesses++;
	num_incoming_load_xfered_bytes += read_bytes;
	
	return read_bytes;
}

template <typename TYPES, typename MSS>
unsigned int MemorySubSystem<TYPES, MSS>::IncomingStore(typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size)
{
	unsigned int written_bytes = 0;
	
	if(size)
	{
		do
		{
			unsigned int sz = 0;
			
			sz = IncomingStoreInLocalMemory<typename MSS::INSTRUCTION_LOCAL_MEMORIES, typename MSS::INSTRUCTION_LOCAL_MEMORIES::LOC_MEM1>(phys_addr, buffer, size);
			
			if(!sz)
			{
				sz = IncomingStoreInLocalMemory<typename MSS::DATA_LOCAL_MEMORIES, typename MSS::DATA_LOCAL_MEMORIES::LOC_MEM1>(phys_addr, buffer, size);
				
				if(!sz)
				{
					return written_bytes;
				}
			}
			
			buffer = (uint8_t *) buffer + sz;
			phys_addr += sz;
			size -= sz;
			written_bytes += sz;
		}
		while(size);
	}
	
	num_incoming_store_accesses++;
	num_incoming_store_xfered_bytes += written_bytes;
	
	return written_bytes;
}

template <typename TYPES, typename MSS>
unsigned int MemorySubSystem<TYPES, MSS>::DebugIncomingLoad(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size)
{
	return IncomingLoad(phys_addr, buffer, size);
}

template <typename TYPES, typename MSS>
unsigned int MemorySubSystem<TYPES, MSS>::DebugIncomingStore(typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size)
{
	return IncomingStore(phys_addr, buffer, size);
}

template <typename TYPES, typename MSS>
uint8_t *MemorySubSystem<TYPES, MSS>::GetDirectAccess(typename TYPES::PHYSICAL_ADDRESS phys_addr, typename TYPES::PHYSICAL_ADDRESS& start_phys_addr, typename TYPES::PHYSICAL_ADDRESS& end_phys_addr)
{
	uint8_t *p = GetDirectAccessToLocalMemory<typename MSS::INSTRUCTION_LOCAL_MEMORIES, typename MSS::INSTRUCTION_LOCAL_MEMORIES::LOC_MEM1>(phys_addr, start_phys_addr, end_phys_addr);
	if(!p)
	{
		p = GetDirectAccessToLocalMemory<typename MSS::DATA_LOCAL_MEMORIES, typename MSS::DATA_LOCAL_MEMORIES::LOC_MEM1>(phys_addr, start_phys_addr, end_phys_addr);
	}
	return p;
}

template <typename TYPES, typename MSS>
template <typename LOC_MEM_SET, typename CACHE_HIERARCHY, typename LOC_MEM, typename CACHE>
inline bool MemorySubSystem<TYPES, MSS>::LoadFromMSS(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	// 3 cases:
	//     - (1) Load from cache hierarchy because access does not target local memories
	//     - (2) Load from that local memory because access target that local memory
	//     - (3) Try to load from next local memory because access does not target that local memory
	
	if(unlikely(LOC_MEM::IsNullLocalMemory()))
	{
		// (1) Load from cache hierarchy
		return LoadFromCacheHierarchy<CACHE_HIERARCHY, CACHE>(addr, phys_addr, buffer, size, storage_attr);
	}

	LOC_MEM *loc_mem = __MSS_GetLocalMemory__<LOC_MEM>();
	typename TYPES::PHYSICAL_ADDRESS loc_mem_base_phys_addr = loc_mem->__MSS_GetBasePhysicalAddress__();
	
	if(likely(phys_addr >= loc_mem_base_phys_addr))
	{
		typename TYPES::PHYSICAL_ADDRESS loc_mem_high_phys_addr = loc_mem_base_phys_addr + loc_mem->__MSS_GetSize__();
	
		if(likely(phys_addr < loc_mem_high_phys_addr))
		{
			// (2) Load from that local memory
			if(unlikely(loc_mem->__MSS_IsVerbose__()))
			{
				__MSS_GetDebugInfoStream__() << LOC_MEM::__MSS_GetLocalMemoryName__() << ": load at @0x" << std::hex << phys_addr << std::dec << std::endl;
			}
			assert((phys_addr + size) <= loc_mem_high_phys_addr);
			memcpy(buffer, &loc_mem->GetByteByOffset(phys_addr - loc_mem_base_phys_addr), size);
			loc_mem->num_load_accesses++;
			return true;
		}
	}

	// (3) Try to load from next local memory
	typedef typename LOC_MEM_SET::template NextLocalMemory<LOC_MEM>::LOC_MEM NLM;

	return LoadFromMSS<LOC_MEM_SET, CACHE_HIERARCHY, NLM, CACHE>(addr, phys_addr, buffer, size, storage_attr);
}

template <typename TYPES, typename MSS>
template <typename LOC_MEM_SET, typename CACHE_HIERARCHY, typename LOC_MEM, typename CACHE>
inline bool MemorySubSystem<TYPES, MSS>::StoreInMSS(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	// 3 cases:
	//     - (1) Store in cache hierarchy because access does not target local memories
	//     - (2) Store in that local memory because access target that local memory
	//     - (3) Try to store in next local memory because access does not target that local memory
	
	if(unlikely(LOC_MEM::IsNullLocalMemory()))
	{
		// (1) Load from cache hierarchy
		return StoreInCacheHierarchy<CACHE_HIERARCHY, CACHE>(addr, phys_addr, buffer, size, storage_attr);
	}

	LOC_MEM *loc_mem = __MSS_GetLocalMemory__<LOC_MEM>();
	typename TYPES::PHYSICAL_ADDRESS loc_mem_base_phys_addr = loc_mem->__MSS_GetBasePhysicalAddress__();
	
	if(likely(phys_addr >= loc_mem_base_phys_addr))
	{
		typename TYPES::PHYSICAL_ADDRESS loc_mem_high_phys_addr = loc_mem_base_phys_addr + loc_mem->__MSS_GetSize__();
	
		if(likely(phys_addr < loc_mem_high_phys_addr))
		{
			// (2) Store in that local memory
			if(unlikely(loc_mem->__MSS_IsVerbose__()))
			{
				__MSS_GetDebugInfoStream__() << LOC_MEM::__MSS_GetLocalMemoryName__() << ": store at @0x" << std::hex << phys_addr << std::dec << std::endl;
			}
			assert((phys_addr + size) <= loc_mem_high_phys_addr);
			memcpy(&loc_mem->GetByteByOffset(phys_addr - loc_mem_base_phys_addr), buffer, size);
			loc_mem->num_store_accesses++;
			return true;
		}
	}

	// (3) Try to store in next local memory
	typedef typename LOC_MEM_SET::template NextLocalMemory<LOC_MEM>::LOC_MEM NLM;

	return StoreInMSS<LOC_MEM_SET, CACHE_HIERARCHY, NLM, CACHE>(addr, phys_addr, buffer, size, storage_attr);
}

template <typename TYPES, typename MSS>
template <typename LOC_MEM_SET, typename CACHE_HIERARCHY, typename LOC_MEM, typename CACHE>
inline bool MemorySubSystem<TYPES, MSS>::DebugLoadFromMSS(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	bool status = true;
	
	if(size)
	{
		do
		{
			unsigned int sz = DebugLoadFromLocalMemory<LOC_MEM_SET, LOC_MEM>(phys_addr, buffer, size);
			
			if(!sz)
			{
				// External
				sz = std::min(size, (unsigned int)(CACHE_HIERARCHY::L1CACHE::BLOCK_SIZE - (addr % CACHE_HIERARCHY::L1CACHE::BLOCK_SIZE)));
				if(!DebugLoadFromCacheHierarchy<CACHE_HIERARCHY, CACHE>(addr, phys_addr, buffer, sz, storage_attr))
				{
					status = false;
					memset(buffer, 0, sz);
				}
			}
			
			buffer = (uint8_t *) buffer + sz;
			addr += sz;
			phys_addr += sz;
			size -= sz;
		}
		while(size);
	}
	
	return status;
}

template <typename TYPES, typename MSS>
template <typename LOC_MEM_SET, typename CACHE_HIERARCHY, typename LOC_MEM, typename CACHE>
inline bool MemorySubSystem<TYPES, MSS>::DebugStoreInMSS(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	bool status = true;
	
	if(size)
	{
		do
		{
			unsigned int sz = DebugStoreInLocalMemory<LOC_MEM_SET, LOC_MEM>(phys_addr, buffer, size);
			
			if(!sz)
			{
				// External
				sz = std::min(size, (unsigned int)(CACHE_HIERARCHY::L1CACHE::BLOCK_SIZE - (addr % CACHE_HIERARCHY::L1CACHE::BLOCK_SIZE)));
				if(!DebugStoreInCacheHierarchy<CACHE_HIERARCHY, CACHE>(addr, phys_addr, buffer, sz, storage_attr))
				{
					status = false;
				}
			}
			
			buffer = (uint8_t *) buffer + sz;
			addr += sz;
			phys_addr += sz;
			size -= sz;
		}
		while(size);
	}
	
	return status;
}

template <typename TYPES, typename MSS>
template <typename LOC_MEM_SET, typename LOC_MEM>
inline unsigned int MemorySubSystem<TYPES, MSS>::DebugLoadFromLocalMemory(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size)
{
	// 2 cases:
	//     - (1) return 0 because access does not target local memories
	//     - (2) Load from that local memory because access target that local memory
	//     - (3) Try to load from next local memory because access does not target that local memory
	
	if(unlikely(LOC_MEM::IsNullLocalMemory()))
	{
		// (1) access does not target local memories
		return 0;
	}

	LOC_MEM *loc_mem = __MSS_GetLocalMemory__<LOC_MEM>();
	typename TYPES::PHYSICAL_ADDRESS loc_mem_base_phys_addr = loc_mem->__MSS_GetBasePhysicalAddress__();
	
	if(likely(phys_addr >= loc_mem_base_phys_addr))
	{
		typename TYPES::PHYSICAL_ADDRESS loc_mem_high_phys_addr = loc_mem_base_phys_addr + loc_mem->__MSS_GetSize__();
	
		if(likely(phys_addr < loc_mem_high_phys_addr))
		{
			// (2) Load from that local memory
			if(unlikely(loc_mem->__MSS_IsVerbose__()))
			{
				__MSS_GetDebugInfoStream__() << LOC_MEM::__MSS_GetLocalMemoryName__() << ": debug load at @0x" << std::hex << phys_addr << std::dec << std::endl;
			}
			
			unsigned int sz = std::min((unsigned int)(loc_mem_high_phys_addr - phys_addr), size);
			
			memcpy(buffer, &loc_mem->GetByteByOffset(phys_addr - loc_mem_base_phys_addr), sz);
			return sz;
		}
	}

	// (3) Try to debug load from next local memory
	typedef typename LOC_MEM_SET::template NextLocalMemory<LOC_MEM>::LOC_MEM NLM;

	return DebugLoadFromLocalMemory<LOC_MEM_SET, NLM>(phys_addr, buffer, size);
}

template <typename TYPES, typename MSS>
template <typename LOC_MEM_SET, typename LOC_MEM>
inline unsigned int MemorySubSystem<TYPES, MSS>::DebugStoreInLocalMemory(typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size)
{
	// 2 cases:
	//     - (1) return 0 because access does not target local memories
	//     - (2) debug store in that local memory because access target that local memory
	//     - (3) Try to debug store in next local memory because access does not target that local memory
	
	if(unlikely(LOC_MEM::IsNullLocalMemory()))
	{
		// (1) access does not target local memories
		return 0;
	}

	LOC_MEM *loc_mem = __MSS_GetLocalMemory__<LOC_MEM>();
	typename TYPES::PHYSICAL_ADDRESS loc_mem_base_phys_addr = loc_mem->__MSS_GetBasePhysicalAddress__();
	
	if(likely(phys_addr >= loc_mem_base_phys_addr))
	{
		typename TYPES::PHYSICAL_ADDRESS loc_mem_high_phys_addr = loc_mem_base_phys_addr + loc_mem->__MSS_GetSize__();
	
		if(likely(phys_addr < loc_mem_high_phys_addr))
		{
			// (2) Load from that local memory
			if(unlikely(loc_mem->__MSS_IsVerbose__()))
			{
				__MSS_GetDebugInfoStream__() << LOC_MEM::__MSS_GetLocalMemoryName__() << ": debug store at @0x" << std::hex << phys_addr << std::dec << std::endl;
			}
			
			unsigned int sz = std::min((unsigned int)(loc_mem_high_phys_addr - phys_addr), size);
			
			memcpy(&loc_mem->GetByteByOffset(phys_addr - loc_mem_base_phys_addr), buffer, sz);
			return sz;
		}
	}

	// (3) Try to debug load from next local memory
	typedef typename LOC_MEM_SET::template NextLocalMemory<LOC_MEM>::LOC_MEM NLM;

	return DebugStoreInLocalMemory<LOC_MEM_SET, NLM>(phys_addr, buffer, size);
}

template <typename TYPES, typename MSS>
template <typename LOC_MEM_SET, typename LOC_MEM>
inline unsigned int MemorySubSystem<TYPES, MSS>::IncomingLoadFromLocalMemory(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size)
{
	// 2 cases:
	//     - (1) return 0 because access does not target local memories
	//     - (2) Load from that local memory because access target that local memory
	//     - (3) Try to load from next local memory because access does not target that local memory
	
	if(unlikely(LOC_MEM::IsNullLocalMemory()))
	{
		// (1) access does not target local memories
		return 0;
	}

	LOC_MEM *loc_mem = __MSS_GetLocalMemory__<LOC_MEM>();
	typename TYPES::PHYSICAL_ADDRESS loc_mem_base_phys_addr = loc_mem->__MSS_GetBasePhysicalAddress__();
	
	if(likely(phys_addr >= loc_mem_base_phys_addr))
	{
		typename TYPES::PHYSICAL_ADDRESS loc_mem_high_phys_addr = loc_mem_base_phys_addr + loc_mem->__MSS_GetSize__();
	
		if(likely(phys_addr < loc_mem_high_phys_addr))
		{
			// (2) Load from that local memory
			if(unlikely(loc_mem->__MSS_IsVerbose__()))
			{
				__MSS_GetDebugInfoStream__() << LOC_MEM::__MSS_GetLocalMemoryName__() << ": incoming load at @0x" << std::hex << phys_addr << std::dec << std::endl;
			}
			
			unsigned int sz = std::min((unsigned int)(loc_mem_high_phys_addr - phys_addr), size);
			
			memcpy(buffer, &loc_mem->GetByteByOffset(phys_addr - loc_mem_base_phys_addr), sz);
			loc_mem->num_load_accesses++;
			return sz;
		}
	}

	// (3) Try to debug load from next local memory
	typedef typename LOC_MEM_SET::template NextLocalMemory<LOC_MEM>::LOC_MEM NLM;

	return IncomingLoadFromLocalMemory<LOC_MEM_SET, NLM>(phys_addr, buffer, size);
}

template <typename TYPES, typename MSS>
template <typename LOC_MEM_SET, typename LOC_MEM>
inline unsigned int MemorySubSystem<TYPES, MSS>::IncomingStoreInLocalMemory(typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size)
{
	// 2 cases:
	//     - (1) return 0 because access does not target local memories
	//     - (2) debug store in that local memory because access target that local memory
	//     - (3) Try to debug store in next local memory because access does not target that local memory
	
	if(unlikely(LOC_MEM::IsNullLocalMemory()))
	{
		// (1) access does not target local memories
		return 0;
	}

	LOC_MEM *loc_mem = __MSS_GetLocalMemory__<LOC_MEM>();
	typename TYPES::PHYSICAL_ADDRESS loc_mem_base_phys_addr = loc_mem->__MSS_GetBasePhysicalAddress__();
	
	if(likely(phys_addr >= loc_mem_base_phys_addr))
	{
		typename TYPES::PHYSICAL_ADDRESS loc_mem_high_phys_addr = loc_mem_base_phys_addr + loc_mem->__MSS_GetSize__();
	
		if(likely(phys_addr < loc_mem_high_phys_addr))
		{
			// (2) Load from that local memory
			if(unlikely(loc_mem->__MSS_IsVerbose__()))
			{
				__MSS_GetDebugInfoStream__() << LOC_MEM::__MSS_GetLocalMemoryName__() << ": incoming store at @0x" << std::hex << phys_addr << std::dec << std::endl;
			}
			
			unsigned int sz = std::min((unsigned int)(loc_mem_high_phys_addr - phys_addr), size);
			
			memcpy(&loc_mem->GetByteByOffset(phys_addr - loc_mem_base_phys_addr), buffer, sz);
			loc_mem->num_store_accesses++;
			return sz;
		}
	}

	// (3) Try to debug load from next local memory
	typedef typename LOC_MEM_SET::template NextLocalMemory<LOC_MEM>::LOC_MEM NLM;

	return IncomingStoreInLocalMemory<LOC_MEM_SET, NLM>(phys_addr, buffer, size);
}

template <typename TYPES, typename MSS>
template <typename LOC_MEM_SET, typename LOC_MEM>
uint8_t *MemorySubSystem<TYPES, MSS>::GetDirectAccessToLocalMemory(typename TYPES::PHYSICAL_ADDRESS phys_addr, typename TYPES::PHYSICAL_ADDRESS& start_phys_addr, typename TYPES::PHYSICAL_ADDRESS& end_phys_addr)
{
	// 2 cases:
	//     - (1) return 0 because access does not target local memories
	//     - (2) return a valid pointer from that local memory because access target that local memory
	//     - (3) Try to get a valid pointer from next local memory because access does not target that local memory
	
	if(unlikely(LOC_MEM::IsNullLocalMemory()))
	{
		// (1) access does not target local memories
		return 0;
	}

	LOC_MEM *loc_mem = __MSS_GetLocalMemory__<LOC_MEM>();
	typename TYPES::PHYSICAL_ADDRESS loc_mem_base_phys_addr = loc_mem->__MSS_GetBasePhysicalAddress__();
	
	if(likely(phys_addr >= loc_mem_base_phys_addr))
	{
		typename TYPES::PHYSICAL_ADDRESS loc_mem_high_phys_addr = loc_mem_base_phys_addr + loc_mem->__MSS_GetSize__();
	
		if(likely(phys_addr < loc_mem_high_phys_addr))
		{
			// (2) return a valid pointer from that local memory 
			start_phys_addr = loc_mem_base_phys_addr;
			end_phys_addr = loc_mem_high_phys_addr - 1;
			if(unlikely(loc_mem->__MSS_IsVerbose__()))
			{
				__MSS_GetDebugInfoStream__() << LOC_MEM::__MSS_GetLocalMemoryName__() << ": direct memory access granted for @0x" << std::hex << start_phys_addr << "-0x" << end_phys_addr << std::dec << std::endl;
			}
			return &loc_mem->GetByteByOffset(0);
		}
	}

	// (3) Try to debug load from next local memory
	typedef typename LOC_MEM_SET::template NextLocalMemory<LOC_MEM>::LOC_MEM NLM;

	return GetDirectAccessToLocalMemory<LOC_MEM_SET, NLM>(phys_addr, start_phys_addr, end_phys_addr);
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE>
inline bool MemorySubSystem<TYPES, MSS>::LoadFromCacheHierarchy(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	// 3 cases:
	//     - (1) Load from memory because access is not cacheable or access is to latest level (memory)
	//     - (2) If cache is disabled, try with next level cache in the hierarchy
	//     - (3) If cache is enable, load from this cache
	if(unlikely(CACHE::IsNullCache() || !__MSS_IsStorageCacheable__(storage_attr)))
	{
		// (1) Load from memory
		if(Same<CACHE_HIERARCHY, typename MSS::INSTRUCTION_CACHE_HIERARCHY>::VALUE)
		{
			return __MSS_InstructionBusRead__(phys_addr, buffer, size, storage_attr);
		}
		else
		{
			return __MSS_DataBusRead__(phys_addr, buffer, size, storage_attr, false);
		}
	}
	
	CACHE *cache = __MSS_GetCache__<CACHE>();

	if(unlikely(!cache->__MSS_IsEnabled__()))
	{
		// (2) Load from next level cache
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		if(!LoadFromCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, phys_addr, buffer, size, storage_attr)) return false;
	}
	else
	{
		// (3) Load from this cache
		CacheAccess<TYPES, CACHE> access;

		access.addr = addr;
		access.phys_addr = phys_addr;
		access.storage_attr = storage_attr;
		access.rwitm = false;
		
		cache->Lookup(access);
		if(unlikely(cache->__MSS_IsVerbose__()))
		{
			__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Lookup at @0x"
			                             << std::hex << access.addr << std::dec << " (physically at @0x"
			                             << std::hex << access.phys_addr << std::dec << "): "
			                             << "line_base_addr=0x" << std::hex << access.line_base_addr << std::dec << ","
			                             << "line_base_phys_addr=0x" << std::hex << access.line_base_phys_addr << std::dec << ","
			                             << "index=0x" << std::hex << access.index << std::dec << ","
			                             << "sector=0x" << std::hex << access.sector << std::dec << ","
			                             << "offset=0x" << std::hex << access.offset << std::dec << ","
			                             << "size_to_block_boundary=0x" << std::hex << access.size_to_block_boundary << std::dec
			                             << std::endl;
		}
		
		if(unlikely(!access.line))
		{
			// Line miss
			if(unlikely(cache->__MSS_IsVerbose__()))
			{
				__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Line miss at @0x" << std::hex << access.addr << std::dec << " (physically at @0x" << std::hex << access.phys_addr << std::dec << ")" << std::endl;
			}

			if(likely(cache->__MSS_ChooseLineToEvict__(access)))
			{
				if(unlikely(cache->__MSS_IsVerbose__()))
				{
					__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Line miss: choosen way=" << access.way << std::endl;
				}
				
				if(unlikely((!EvictLine<CACHE_HIERARCHY, CACHE>(access)))) return false;
			}
		}
		else
		{
			if(unlikely(cache->__MSS_IsVerbose__()))
			{
				__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Line hit: index=" << access.index << ", way=" << access.line->GetWay() << std::endl;
			}
		}

		if(likely(access.line))
		{
			if(unlikely(!access.block))
			{
				// Block miss
				if(unlikely(cache->__MSS_IsVerbose__()))
				{
					__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Block miss at @0x" << std::hex << access.addr << std::dec << " (physically at @0x" << std::hex << access.phys_addr << std::dec << ")" << std::endl;
				}
				
				typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
				
				if(unlikely((!FillBlock<CACHE_HIERARCHY, CACHE, NLC>(access)))) return false;
				cache->__MSS_UpdateReplacementPolicyOnFill__(access);
			}
			else
			{
				if(unlikely(cache->__MSS_IsVerbose__()))
				{
					__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Block hit at @0x" << std::hex << access.addr << std::dec << " (physically at @0x" << std::hex << access.phys_addr << std::dec << ")" << std::endl;
				}
			}

			assert(size <= access.size_to_block_boundary);
			memcpy(buffer, &access.block->GetByteByOffset(access.offset), size);
			cache->__MSS_UpdateReplacementPolicyOnAccess__(access);
		}
		else
		{
			// Line miss and locked way: Load from next level cache
			typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
			
			if(!LoadFromCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, phys_addr, buffer, size, storage_attr)) return false;
		}
	}
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE>
inline bool MemorySubSystem<TYPES, MSS>::StoreInCacheHierarchy(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	// 3 cases:
	//     - (1) Store to memory because access is not cacheable or access is to latest level (memory)
	//     - (2) If cache is disabled, try with next level cache in the hierarchy
	//     - (3) If cache is enable, store in this cache
	if(unlikely(CACHE::IsNullCache() || !__MSS_IsStorageCacheable__(storage_attr)))
	{
		// (1) Store to memory
		return __MSS_DataBusWrite__(phys_addr, buffer, size, storage_attr);
	}
	
	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	if(unlikely(!cache->__MSS_IsEnabled__()))
	{
		// (2) Store in next level cache
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		if(!StoreInCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, phys_addr, buffer, size, storage_attr)) return false;
	}
	else
	{
		// (3) store in this cache
		CacheAccess<TYPES, CACHE> access;

		access.addr = addr;
		access.phys_addr = phys_addr;
		access.storage_attr = storage_attr;
		access.rwitm = true;
		
		cache->Lookup(access);
		if(unlikely(cache->__MSS_IsVerbose__()))
		{
			__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Lookup at @0x"
			                             << std::hex << access.addr << std::dec << " (physically at @0x"
			                             << std::hex << access.phys_addr << std::dec << ") : "
			                             << "line_base_addr=0x" << std::hex << access.line_base_addr << std::dec << ","
			                             << "line_base_phys_addr=0x" << std::hex << access.line_base_phys_addr << std::dec << ","
			                             << "index=0x" << std::hex << access.index << std::dec << ","
			                             << "sector=0x" << std::hex << access.sector << std::dec << ","
			                             << "offset=0x" << std::hex << access.offset << std::dec << ","
			                             << "size_to_block_boundary=0x" << std::hex << access.size_to_block_boundary << std::dec
			                             << std::endl;
		}
		
		if(unlikely(!access.line))
		{
			// Line miss
			if(unlikely(cache->__MSS_IsVerbose__()))
			{
				__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Line miss at @0x" << std::hex << access.addr << std::dec << " (physically at @0x" << std::hex << access.phys_addr << std::dec << ")" << std::endl;
			}

			if(cache->__MSS_IsWriteAllocate__(storage_attr))
			{
				// write-allocate: evict before fill
				if(likely(cache->__MSS_ChooseLineToEvict__(access)))
				{
					if(unlikely(cache->__MSS_IsVerbose__()))
					{
						__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Line miss: choosen way=" << access.way << std::endl;
					}

					if(unlikely((!EvictLine<CACHE_HIERARCHY, CACHE>(access)))) return false;
				}
			}
		}
		else
		{
			if(unlikely(cache->__MSS_IsVerbose__()))
			{
				__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Line hit: index=" << access.index << ", way=" << access.line->GetWay() << std::endl;
			}
		}
		
		if(likely(access.line))
		{
			if(unlikely(!access.block))
			{
				// Block miss
				if(unlikely(cache->__MSS_IsVerbose__()))
				{
					__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Block miss at @0x" << std::hex << access.addr << std::dec << " (physically at @0x" << std::hex << access.phys_addr << std::dec << ")" << std::endl;
				}
				
				if(cache->__MSS_IsWriteAllocate__(storage_attr))
				{
					// write-allocate: fill if missing block
					typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
					
					if(unlikely((!FillBlock<CACHE_HIERARCHY, CACHE, NLC>(access)))) return false;
					cache->__MSS_UpdateReplacementPolicyOnFill__(access);
				}
			}
			else
			{
				if(unlikely(cache->__MSS_IsVerbose__()))
				{
					__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Block hit at @0x" << std::hex << access.addr << std::dec << " (physically at @0x" << std::hex << access.phys_addr << std::dec << ")" << std::endl;
				}
			}
			
			bool is_access_write_through = CACHE::IsStaticWriteThroughCache() || __MSS_IsDataStoreAccessWriteThrough__(storage_attr);
			
			if(access.block)
			{
				// write-allocate or cache hit
				assert(size <= access.size_to_block_boundary);
				memcpy(&access.block->GetByteByOffset(access.offset), buffer, size);
				if(!is_access_write_through)
				{
					// write-back
					access.block->SetDirty();
				}
				cache->__MSS_UpdateReplacementPolicyOnAccess__(access);
			}
			
			if(!access.block || is_access_write_through)
			{
				typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
				
				if(unlikely((!StoreInCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, phys_addr, buffer, size, storage_attr)))) return false;
			}
		}
		else
		{
			// Line miss and locked way: Store to next level cache
			typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
			
			if(!StoreInCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, phys_addr, buffer, size, storage_attr)) return false;
		}
	}

	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE>
bool MemorySubSystem<TYPES, MSS>::DebugLoadFromCacheHierarchy(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	// 3 cases:
	//     - (1) Debug Load from memory because access is not cacheable or access is to latest level (memory)
	//     - (2) If cache is disabled, try with next level cache in the hierarchy
	//     - (3) If cache is enable, debug load from this cache
	if(unlikely(CACHE::IsNullCache() || !__MSS_IsStorageCacheable__(storage_attr)))
	{
		// (1) Load from memory
		if(Same<CACHE_HIERARCHY, typename MSS::INSTRUCTION_CACHE_HIERARCHY>::VALUE)
		{
			return __MSS_DebugInstructionBusRead__(phys_addr, buffer, size, storage_attr);
		}
		else
		{
			return __MSS_DebugDataBusRead__(phys_addr, buffer, size, storage_attr);
		}
	}
	
	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	if(unlikely(!cache->__MSS_IsEnabled__()))
	{
		// (2) Load from next level cache
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		return DebugLoadFromCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, phys_addr, buffer, size, storage_attr);
	}
	else
	{
		// (3) Debug Load from this cache
		CacheAccess<TYPES, CACHE> access;

		access.addr = addr;
		access.phys_addr = phys_addr;
		
		cache->Lookup(access);

		if(unlikely(!access.block))
		{
			// miss: try debug loading from next level cache
			typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
			
			return DebugLoadFromCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, phys_addr, buffer, size, storage_attr);
		}

		assert(size <= access.size_to_block_boundary);
		memcpy(buffer, &access.block->GetByteByOffset(access.offset), size);
	}
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE>
bool MemorySubSystem<TYPES, MSS>::DebugStoreInCacheHierarchy(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	// 3 cases:
	//     - (1) Debug Store to memory because access is not cacheable or access is to latest level (memory)
	//     - (2) If cache is disabled, try with next level cache in the hierarchy
	//     - (3) If cache is enable, debug store into this cache
	if(unlikely(CACHE::IsNullCache() || !__MSS_IsStorageCacheable__(storage_attr)))
	{
		// (1) debug store to memory
		return __MSS_DebugDataBusWrite__(phys_addr, buffer, size, storage_attr);
	}
	
	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	if(unlikely(!cache->__MSS_IsEnabled__()))
	{
		// (2) Store into next level cache
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		return DebugStoreInCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, phys_addr, buffer, size, storage_attr);
	}
	else
	{
		// (3) Debug Store into this cache
		CacheAccess<TYPES, CACHE> access;

		access.addr = addr;
		access.phys_addr = phys_addr;
		
		cache->Lookup(access);

		if(unlikely(!access.block))
		{
			// miss: try debug loading from next level cache
			typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
			
			return DebugStoreInCacheHierarchy<CACHE_HIERARCHY, NLC>(addr, phys_addr, buffer, size, storage_attr);
		}

		assert(size <= access.size_to_block_boundary);
		memcpy(&access.block->GetByteByOffset(access.offset), buffer, size);
	}
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE, bool ZEROING>
inline bool MemorySubSystem<TYPES, MSS>::AllocateBlockInCacheHierarchy(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, typename TYPES::STORAGE_ATTR storage_attr)
{
	// 3 cases:
	//     - (1) If memory location is not cacheable or allocation reaches the latest level (memory), optionnaly zero bytes in memory and return
	//     - (2) If cache is disabled, try with next level cache in the hierarchy
	//     - (3) If cache is enable, allocate block in this cache, optionnally zero bytes in block
	if(unlikely(CACHE::IsNullCache() || !__MSS_IsStorageCacheable__(storage_attr)))
	{
		// (1) If memory location is not cacheable or allocation reaches the latest level (memory), optionnaly zero bytes in memory and return
		uint8_t zero[CACHE::BLOCK_SIZE] = {};
		return !ZEROING || __MSS_DataBusWrite__(phys_addr & -MSS::DATA_CACHE_HIERARCHY::L1CACHE::BLOCK_SIZE, zero, CACHE::BLOCK_SIZE, storage_attr);
	}
	
	CACHE *cache = __MSS_GetCache__<CACHE>();

	if(unlikely(!cache->__MSS_IsEnabled__()))
	{
		// (2) Cache is disabled, try with next level cache in the hierarchy
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		return AllocateBlockInCacheHierarchy<CACHE_HIERARCHY, NLC, ZEROING>(addr, phys_addr, storage_attr);
	}
	
	// (3) cache is enabled: allocate block in this cache, and optionnally zero bytes in block
	CacheAccess<TYPES, CACHE> access;

	access.addr = addr;
	access.phys_addr = phys_addr;
	access.storage_attr = storage_attr;
	access.rwitm = false;
	
	cache->Lookup(access);
	if(unlikely(cache->__MSS_IsVerbose__()))
	{
		__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Lookup at @0x"
										<< std::hex << access.addr << std::dec << " (physically at @0x"
										<< std::hex << access.phys_addr << std::dec << "): "
										<< "line_base_addr=0x" << std::hex << access.line_base_addr << std::dec << ","
										<< "line_base_phys_addr=0x" << std::hex << access.line_base_phys_addr << std::dec << ","
										<< "index=0x" << std::hex << access.index << std::dec << ","
										<< "sector=0x" << std::hex << access.sector << std::dec << ","
										<< "offset=0x" << std::hex << access.offset << std::dec << ","
										<< "size_to_block_boundary=0x" << std::hex << access.size_to_block_boundary << std::dec
										<< std::endl;
	}
	
	if(unlikely(!access.line))
	{
		// Line miss
		if(unlikely(cache->__MSS_IsVerbose__()))
		{
			__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Line miss at @0x" << std::hex << access.addr << std::dec << " (physically at @0x" << std::hex << access.phys_addr << std::dec << ")" << std::endl;
		}

		if(likely(cache->__MSS_ChooseLineToEvict__(access)))
		{
			if(unlikely(cache->__MSS_IsVerbose__()))
			{
				__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Line miss: choosen way=" << access.way << std::endl;
			}
			
			if(unlikely((!EvictLine<CACHE_HIERARCHY, CACHE>(access)))) return false;
		}
	}
	else
	{
		if(unlikely(cache->__MSS_IsVerbose__()))
		{
			__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Line hit: index=" << access.index << ", way=" << access.line->GetWay() << std::endl;
		}
	}
	
	if(unlikely(!access.block))
	{
		access.block = &access.line->GetBlockBySector(access.sector);
	}
	
	access.line->SetValid();
	access.line->SetBaseAddress(access.line_base_addr);
	access.line->SetBasePhysicalAddress(access.line_base_phys_addr);
	
	if(ZEROING)
	{
		access.block->Zero();
		access.block->SetDirty();
		if(unlikely(enable_shadow_memory))
		{
			shadow_memory.clear(access.block->GetBasePhysicalAddress(), CACHE::BLOCK_SIZE);
		}
	}
	else
	{
		access.block->SetDirty(false);
	}
	
	access.block->SetValid();
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename TO, typename FROM>
inline bool MemorySubSystem<TYPES, MSS>::FillBlock(CacheAccess<TYPES, TO>& to_access)
{
	// 3 cases:
	//     - (1) Fill block from memory
	//     - (2) if cache is disabled, try to fill block from next level cache
	//     - (3) if cache is enabled, fill block from cache
	
	to_access.block = &to_access.line->GetBlockBySector(to_access.sector);
	
	if(unlikely(FROM::IsNullCache()))
	{
		// (1) Block Fill from Memory
		if(unlikely(to_access.cache->__MSS_IsVerbose__()))
		{
			__MSS_GetDebugInfoStream__() << TO::__MSS_GetCacheName__() << ": Filling block from memory at @0x" << std::hex << to_access.block_base_addr << std::dec << " (physically at @0x" << std::hex << to_access.block_base_phys_addr << std::dec << ")" << std::endl;
		}
		
		if(TO::IsDataCache())
		{
			if(unlikely(!__MSS_DataBusRead__(to_access.block_base_phys_addr, &to_access.block->GetByteByOffset(0), TO::BLOCK_SIZE, to_access.storage_attr, to_access.rwitm)))
			{
				return false;
			}
		}
		else
		{
			if(unlikely(!__MSS_InstructionBusRead__(to_access.block_base_phys_addr, &to_access.block->GetByteByOffset(0), TO::BLOCK_SIZE, to_access.storage_attr)))
			{
				return false;
			}
		}
	}
	else
	{
		FROM *from = __MSS_GetCache__<FROM>();
			
		if(unlikely(!from->__MSS_IsEnabled__()))
		{
			// (2) fill block from next level cache
			typedef typename CACHE_HIERARCHY::template NextLevel<FROM>::CACHE NLC;
			
			if(unlikely((!FillBlock<CACHE_HIERARCHY, TO, NLC>(to_access)))) return false;
			to_access.cache->__MSS_UpdateReplacementPolicyOnFill__(to_access);
		}
		else
		{
			// (3) cache is enabled, try to fill block from cache
			
			// 2 sub-cases:
			//   - (3.1) if block fill hits then fill block from cache
			//   - (3.2) if block fill misses then try to fill block from next level cache

			if(unlikely(to_access.cache->__MSS_IsVerbose__()))
			{
				__MSS_GetDebugInfoStream__() << TO::__MSS_GetCacheName__() << ": Filling block from " << FROM::__MSS_GetCacheName__() << " at @0x" << std::hex << to_access.block_base_addr << std::dec << " (physically at @0x" << std::hex << to_access.block_base_phys_addr << std::dec << ")" << std::endl;
			}
			
			CacheAccess<TYPES, FROM> from_access;
			
			from_access.addr = to_access.block_base_addr;
			from_access.phys_addr = to_access.block_base_phys_addr;
			from_access.storage_attr = to_access.storage_attr;
			from_access.rwitm = to_access.rwitm;

			from->Lookup(from_access);
			if(unlikely(from->__MSS_IsVerbose__()))
			{
				__MSS_GetDebugInfoStream__() << FROM::__MSS_GetCacheName__() << ": Lookup at @0x"
											<< std::hex << from_access.addr << std::dec << " (physically at @"
											<< std::hex << from_access.phys_addr << std::dec << "): "
											<< "line_base_phys_addr=0x" << std::hex << from_access.line_base_phys_addr << std::dec << ","
											<< "index=0x" << std::hex << from_access.index << std::dec << ","
											<< "sector=0x" << std::hex << from_access.sector << std::dec << ","
											<< "offset=0x" << std::hex << from_access.offset << std::dec << ","
											<< "size_to_block_boundary=0x" << std::hex << from_access.size_to_block_boundary << std::dec
											<< std::endl;
			}

			if(unlikely(!from_access.line))
			{
				// Line miss
				if(unlikely(from->__MSS_IsVerbose__()))
				{
					__MSS_GetDebugInfoStream__() << FROM::__MSS_GetCacheName__() << ": Line miss at @0x" << std::hex << from_access.addr << std::dec << " (physically at @0x" << std::hex << from_access.phys_addr << std::dec << ")" << std::endl;
				}
				
				if(likely(from->__MSS_ChooseLineToEvict__(from_access)))
				{
					if(unlikely(from->__MSS_IsVerbose__()))
					{
						__MSS_GetDebugInfoStream__() << FROM::__MSS_GetCacheName__() << ": Line miss: choosen way=" << from_access.way << std::endl;
					}
					
					if(unlikely((!EvictLine<CACHE_HIERARCHY, FROM>(from_access)))) return false;
				}
			}
			else
			{
				if(unlikely(from->__MSS_IsVerbose__()))
				{
					__MSS_GetDebugInfoStream__() << FROM::__MSS_GetCacheName__() << ": Line hit: index=" << from_access.index << ", way=" << from_access.line->GetWay() << std::endl;
				}
			}

			if(likely(from_access.line))
			{
				if(unlikely(!from_access.block))
				{
					// (3.2) fill block from next level cache
					// Block miss
					if(unlikely(from->__MSS_IsVerbose__()))
					{
						__MSS_GetDebugInfoStream__() << FROM::__MSS_GetCacheName__() << ": Block miss at @0x" << std::hex << from_access.addr << std::dec << " (physically at @0x" << std::hex << from_access.phys_addr << std::dec << ")" << std::endl;
					}
					
					typedef typename CACHE_HIERARCHY::template NextLevel<FROM>::CACHE NLC;
					
					if(unlikely((!FillBlock<CACHE_HIERARCHY, FROM, NLC>(from_access)))) return false;
					from->__MSS_UpdateReplacementPolicyOnFill__(from_access);
				}
				else
				{
					// (3.1) fill block from cache
					if(unlikely(from->__MSS_IsVerbose__()))
					{
						__MSS_GetDebugInfoStream__() << FROM::__MSS_GetCacheName__() << ": Block hit at @0x" << std::hex << from_access.addr << std::dec << " (physically at @0x" << std::hex << from_access.phys_addr << std::dec << ")" << std::endl;
					}
				}

				assert(TO::BLOCK_SIZE <= from_access.size_to_block_boundary);
				memcpy(&to_access.block->GetByteByOffset(0), &from_access.block->GetByteByOffset(from_access.offset), TO::BLOCK_SIZE); // <-- block fill
				from->__MSS_UpdateReplacementPolicyOnAccess__(from_access);
			}
			else
			{
				// Line miss and locked way: fill from next level cache
				typedef typename CACHE_HIERARCHY::template NextLevel<FROM>::CACHE NLC;
				
				if(unlikely((!FillBlock<CACHE_HIERARCHY, TO, NLC>(to_access)))) return false;
				to_access.cache->__MSS_UpdateReplacementPolicyOnFill__(to_access);
			}
		}
	}
	
	to_access.line->SetValid();
	to_access.line->SetBaseAddress(to_access.line_base_addr);
	to_access.line->SetBasePhysicalAddress(to_access.line_base_phys_addr);
	to_access.block->SetValid();
	to_access.block->SetDirty(false);
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename FROM, typename TO, bool INVALIDATE>
bool MemorySubSystem<TYPES, MSS>::WriteBackDirtyBlock(CacheAccess<TYPES, FROM>& from_access, unsigned int from_sector)
{
	CacheBlock<TYPES, typename FROM::CACHE_CONFIG>& from_dirty_block_to_write_back = from_access.line_to_write_back_evict->GetBlockBySector(from_sector);
	
	// 3 cases:
	//   - (1) write back to memory
	//   - (2) if cache is disabled, try to write back to next level cache
	//   - (3) if cache is enabled, write back to cache
	if(unlikely(TO::IsNullCache()))
	{
		// (1) Write back block to memory
		if(unlikely(from_access.cache->__MSS_IsVerbose__()))
		{
			__MSS_GetDebugInfoStream__() << FROM::__MSS_GetCacheName__() << ": Writing back dirty block to memory 0x" << std::hex << from_dirty_block_to_write_back.GetBaseAddress() << std::dec << " (physically at @0x" << std::hex << from_dirty_block_to_write_back.GetBasePhysicalAddress() << std::dec << ")" << std::endl;
		}
		if(unlikely(!__MSS_DataBusWrite__(from_dirty_block_to_write_back.GetBasePhysicalAddress(), &from_dirty_block_to_write_back.GetByteByOffset(0), FROM::BLOCK_SIZE, typename TYPES::STORAGE_ATTR())))
		{
			return false;
		}
	}
	else
	{
		TO *to = __MSS_GetCache__<TO>();
			
		if(unlikely(!to->__MSS_IsEnabled__()))
		{
			// (2) try to write back to next level cache
			typedef typename CACHE_HIERARCHY::template NextLevel<TO>::CACHE NLC;
			
			if(!WriteBackDirtyBlock<CACHE_HIERARCHY, FROM, NLC, INVALIDATE>(from_access, from_sector)) return false;
		}
		else
		{
			// (3) cache is enabled, try to write back to cache
			
			// 2 sub-cases:
			//   - (3.1) if write back hits then write back to cache
			//   - (3.2) if write back misses then try to write back in next level cache
			CacheAccess<TYPES, TO> to_access;

			to_access.addr = from_dirty_block_to_write_back.GetBaseAddress();
			to_access.phys_addr = from_dirty_block_to_write_back.GetBasePhysicalAddress();
			
			to->Lookup(to_access);
			if(unlikely(to->__MSS_IsVerbose__()))
			{
				__MSS_GetDebugInfoStream__() << TO::__MSS_GetCacheName__() << ": Lookup at @0x"
											<< std::hex << to_access.addr << std::dec << " (physically at @0x"
											<< std::hex << to_access.phys_addr << std::dec << "): "
											<< "line_base_phys_addr=0x" << std::hex << to_access.line_base_phys_addr << std::dec << ","
											<< "index=0x" << std::hex << to_access.index << std::dec << ","
											<< "sector=0x" << std::hex << to_access.sector << std::dec << ","
											<< "offset=0x" << std::hex << to_access.offset << std::dec << ","
											<< "size_to_block_boundary=0x" << std::hex << to_access.size_to_block_boundary << std::dec
											<< std::endl;
			}

			if(to_access.block)
			{
				// (3.1) write back to cache
				// block hit
				if(unlikely(to->__MSS_IsVerbose__()))
				{
					__MSS_GetDebugInfoStream__() << FROM::__MSS_GetCacheName__() << ": Writing back dirty block to " << TO::__MSS_GetCacheName__() << " at @0x" << std::hex << from_dirty_block_to_write_back.GetBaseAddress() << std::dec << " (physically at @0x" << std::hex << from_dirty_block_to_write_back.GetBasePhysicalAddress() << std::dec << ")" << std::endl;
				}
				assert(FROM::BLOCK_SIZE <= to_access.size_to_block_boundary);
				memcpy(&to_access.block->GetByteByOffset(to_access.offset), &from_dirty_block_to_write_back.GetByteByOffset(0), FROM::BLOCK_SIZE); // <-- write back
				to_access.block->SetDirty();
			}
			else
			{
				// (3.2) try to write back in next level cache
				typedef typename CACHE_HIERARCHY::template NextLevel<TO>::CACHE NLC;
				
				if(!WriteBackDirtyBlock<CACHE_HIERARCHY, FROM, NLC, INVALIDATE>(from_access, from_sector)) return false;
			}
		}
	}
	
	// optionnally invalidate block
	if(INVALIDATE)
	{
		if(unlikely(from_access.cache->__MSS_IsVerbose__()))
		{
			__MSS_GetDebugInfoStream__() << FROM::__MSS_GetCacheName__() << ": Invalidating block at 0x" << std::hex << from_dirty_block_to_write_back.GetBaseAddress() << std::dec << " (physically at @0x" << std::hex << from_dirty_block_to_write_back.GetBasePhysicalAddress() << std::dec << ")" << std::endl;
		}
		from_dirty_block_to_write_back.Invalidate();
	}
	else
	{
		from_dirty_block_to_write_back.SetDirty(false);
	}
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE, bool INVALIDATE>
bool MemorySubSystem<TYPES, MSS>::WriteBackLine(CacheAccess<TYPES, CACHE>& access)
{
	if(access.line_to_write_back_evict->IsValid())
	{
		if(unlikely(access.cache->__MSS_IsVerbose__()))
		{
			__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Writing back Line at @0x" << std::hex << access.line_to_write_back_evict->GetBaseAddress() << std::dec << " (physically at @0x" << std::hex << access.line_to_write_back_evict->GetBasePhysicalAddress() << std::dec << ")" << std::endl;
		}
		unsigned int sector;
		
		for(sector = 0; sector < CACHE::BLOCKS_PER_LINE; sector++)
		{
			CacheBlock<TYPES, typename CACHE::CACHE_CONFIG>& block_to_evict = access.line_to_write_back_evict->GetBlockBySector(sector);
			
			if(block_to_evict.IsValid() && block_to_evict.IsDirty())
			{
				// write back dirty block
				typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
				
				if(!WriteBackDirtyBlock<CACHE_HIERARCHY, CACHE, NLC, INVALIDATE>(access, sector)) return false;
			}
		}
		
		if(INVALIDATE && access.line_to_write_back_evict->IsValid())
		{
			if(unlikely(access.cache->__MSS_IsVerbose__()))
			{
				__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Invalidating Line at @0x" << std::hex << access.line_to_write_back_evict->GetBaseAddress() << std::dec << " (physically at @0x" << std::hex << access.line_to_write_back_evict->GetBasePhysicalAddress() << std::dec << ")" << std::endl;
			}
			access.line_to_write_back_evict->Invalidate();
		}
	}
	else
	{
		if(unlikely(access.cache->__MSS_IsVerbose__()))
		{
			__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Nothing to write back because Line is invalid" << std::endl;
		}
	}
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE>
bool MemorySubSystem<TYPES, MSS>::EvictLine(CacheAccess<TYPES, CACHE>& access)
{
	if(CACHE::IsDataCache())
	{
		WriteBackLine<CACHE_HIERARCHY, CACHE, /* invalidate */ true >(access);
	}
	else if(access.line_to_write_back_evict->IsValid())
	{
		if(unlikely(access.cache->__MSS_IsVerbose__()))
		{
			__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Invalidating Line at @0x" << std::hex << access.line_to_write_back_evict->GetBaseAddress() << std::dec << " (physically at @0x" << std::hex << access.line_to_write_back_evict->GetBasePhysicalAddress() << std::dec << ")" << std::endl;
		}
		access.line_to_write_back_evict->Invalidate();
	}
	
	access.line = access.line_to_write_back_evict;
	access.line_to_write_back_evict = 0;
	
	return true;
}

template <typename TYPES, typename MSS>
inline std::ostream& MemorySubSystem<TYPES, MSS>::GetDebugInfoStream()
{
	return std::cout;
}

template <typename TYPES, typename MSS>
inline std::ostream& MemorySubSystem<TYPES, MSS>::GetDebugWarningStream()
{
	return std::cerr;
}

template <typename TYPES, typename MSS>
inline std::ostream& MemorySubSystem<TYPES, MSS>::GetDebugErrorStream()
{
	return std::cerr;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::IsVerboseDataLoad() const
{
	return false;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::IsVerboseDataStore() const
{
	return false;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::IsVerboseInstructionFetch() const
{
	return false;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::IsVerboseDataBusRead() const
{
	return false;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::IsVerboseDataBusWrite() const
{
	return false;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::IsVerboseInstructionBusRead() const
{
	return false;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::IsStorageCacheable(typename TYPES::STORAGE_ATTR storage_attr) const
{
	return false;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::IsDataStoreAccessWriteThrough(typename TYPES::STORAGE_ATTR storage_attr) const
{
	return false;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DataBusRead(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr, bool rwitm)
{
	return false;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DataBusWrite(typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	return false;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::InstructionBusRead(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	return false;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DebugDataBusRead(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	return false;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DebugDataBusWrite(typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	return false;
}

template <typename TYPES, typename MSS>
bool MemorySubSystem<TYPES, MSS>::DebugInstructionBusRead(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	return false;
}

template <typename TYPES, typename MSS>
void MemorySubSystem<TYPES, MSS>::Trap()
{
}

template <typename TYPES, typename MSS>
void MemorySubSystem<TYPES, MSS>::Trace(const char *type, typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size)
{
	unsigned int i;
	__MSS_GetDebugInfoStream__() << "Memory Sub-System: " << type << " [";
	for(i = 0; i < size; i++)
	{
		uint8_t value = ((uint8_t *) buffer)[i];
		uint8_t l = value & 15;
		uint8_t h = value >> 4;
		__MSS_GetDebugInfoStream__() << (i ? " ": "") << "0x" << (char)((h < 10) ? '0' + h : 'a' + h - 10) << (char)((l < 10) ? '0' + l : 'a' + l - 10);
	}
	__MSS_GetDebugInfoStream__() << "] (" << size << " bytes) at @0x" << std::hex << addr << std::dec << " (physically at @0x" << std::hex << phys_addr << std::dec << ")" << std::endl;
}

template <typename TYPES, typename MSS>
void MemorySubSystem<TYPES, MSS>::Trace(const char *type, typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size)
{
	unsigned int i;
	__MSS_GetDebugInfoStream__() << "Memory Sub-System: " << type << " [";
	for(i = 0; i < size; i++)
	{
		uint8_t value = ((uint8_t *) buffer)[i];
		uint8_t l = value & 15;
		uint8_t h = value >> 4;
		__MSS_GetDebugInfoStream__() << (i ? " ": "") << "0x" << (char)((h < 10) ? '0' + h : 'a' + h - 10) << (char)((l < 10) ? '0' + l : 'a' + l - 10);
	}
	__MSS_GetDebugInfoStream__() << "] (" << size << " bytes) at @0x" << std::hex << phys_addr << std::dec << std::endl;
}

template <typename TYPES, typename MSS>
void MemorySubSystem<TYPES, MSS>::Check(const char *type, const void *buffer, typename TYPES::PHYSICAL_ADDRESS phys_addr, unsigned int size)
{
	uint8_t shadow_buffer[size];
	shadow_memory.read(&shadow_buffer[0], phys_addr, size);
	if(memcmp((const void *) buffer, (const void *) &shadow_buffer[0], size) != 0)
	{
		__MSS_GetDebugWarningStream__() << type << " coherency check failed at 0x" << std::hex << phys_addr << std::endl;
		__MSS_GetDebugWarningStream__() << "Got [";
		const uint8_t *p = (const uint8_t *) buffer;
		for(unsigned int i = 0; i < size; ++i, ++p)
		{
			__MSS_GetDebugWarningStream__() << (i ? " ": "") << std::hex << "0x" << +(*p);
		}
		__MSS_GetDebugWarningStream__() << "]" << std::endl << "Expecting [";
		p = &shadow_buffer[0];
		for(unsigned int i = 0; i < size; ++i, ++p)
		{
			__MSS_GetDebugWarningStream__() << (i ? " ": "") << std::hex << "0x" << +(*p);
		}
		__MSS_GetDebugWarningStream__() << "]" << std::endl;
		__MSS_Trap__();
	}
}

template <typename TYPES, typename MSS>
template <typename CACHE> inline CACHE *MemorySubSystem<TYPES, MSS>::__MSS_GetCache__()
{
	CACHE *cache = __MSS_CACHE_INTERFACE__<CACHE>::GetCache(static_cast<MSS *>(this), (const CACHE *) 0);
	return cache;
}

template <typename TYPES, typename MSS>
template <typename LOC_MEM> inline LOC_MEM *MemorySubSystem<TYPES, MSS>::__MSS_GetLocalMemory__()
{
	LOC_MEM *loc_mem = __MSS_LOC_MEM_INTERFACE__<LOC_MEM>::GetLocalMemory(static_cast<MSS *>(this), (const LOC_MEM *) 0);
	return loc_mem;
}

template <typename TYPES, typename MSS>
inline std::ostream& MemorySubSystem<TYPES, MSS>::__MSS_GetDebugInfoStream__()
{
	return static_cast<MSS *>(this)->GetDebugInfoStream();
}

template <typename TYPES, typename MSS>
inline std::ostream& MemorySubSystem<TYPES, MSS>::__MSS_GetDebugWarningStream__()
{
	return static_cast<MSS *>(this)->GetDebugWarningStream();
}

template <typename TYPES, typename MSS>
inline std::ostream& MemorySubSystem<TYPES, MSS>::__MSS_GetDebugErrorStream__()
{
	return static_cast<MSS *>(this)->GetDebugErrorStream();
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsStorageCacheable__(typename TYPES::STORAGE_ATTR storage_attr) const
{
	return static_cast<const MSS *>(this)->IsStorageCacheable(storage_attr);
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsDataStoreAccessWriteThrough__(typename TYPES::STORAGE_ATTR storage_attr) const
{
	return static_cast<const MSS *>(this)->IsDataStoreAccessWriteThrough(storage_attr);
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsVerboseDataLoad__() const
{
	return static_cast<const MSS *>(this)->IsVerboseDataLoad();
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsVerboseDataStore__() const
{
	return static_cast<const MSS *>(this)->IsVerboseDataStore();
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsVerboseInstructionFetch__() const
{
	return static_cast<const MSS *>(this)->IsVerboseInstructionFetch();
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsVerboseDataBusRead__() const
{
	return static_cast<const MSS *>(this)->IsVerboseDataBusRead();
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsVerboseDataBusWrite__() const
{
	return static_cast<const MSS *>(this)->IsVerboseDataBusWrite();
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_IsVerboseInstructionBusRead__() const
{
	return static_cast<const MSS *>(this)->IsVerboseInstructionBusRead();
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_DataBusRead__(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr, bool rwitm)
{
	if(!static_cast<MSS *>(this)->DataBusRead(phys_addr, buffer, size, storage_attr, rwitm)) return false;

	if(unlikely(__MSS_IsVerboseDataBusRead__())) Trace("Reading from Bus Data", phys_addr, buffer, size);
	
	num_data_bus_read_accesses++;
	num_data_bus_read_xfered_bytes += size;

	if(unlikely(enable_shadow_memory))
	{
		shadow_memory.write(phys_addr, (const uint8_t *) buffer, size);
	}

	return true;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_DataBusWrite__(typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	if(unlikely(__MSS_IsVerboseDataBusWrite__())) Trace("Writing to Bus Data", phys_addr, buffer, size);

	if(!static_cast<MSS *>(this)->DataBusWrite(phys_addr, buffer, size, storage_attr)) return false;
	
	num_data_bus_write_accesses++;
	num_data_bus_write_xfered_bytes += size;

	return true;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_InstructionBusRead__(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	if(!static_cast<MSS *>(this)->InstructionBusRead(phys_addr, buffer, size, storage_attr)) return false;

	if(unlikely(__MSS_IsVerboseInstructionBusRead__())) Trace("Reading from Bus Instruction", phys_addr, buffer, size);
	
	num_instruction_bus_read_accesses++;
	num_instruction_bus_read_xfered_bytes += size;
	
	if(unlikely(enable_shadow_memory))
	{
		shadow_memory.write(phys_addr, (const uint8_t *) buffer, size);
	}
	
	return true;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_DebugDataBusRead__(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	if(!static_cast<MSS *>(this)->DebugDataBusRead(phys_addr, buffer, size, storage_attr)) return false;
	
	if(unlikely(enable_shadow_memory))
	{
		shadow_memory.write(phys_addr, (const uint8_t *) buffer, size);
	}

	return true;
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_DebugDataBusWrite__(typename TYPES::PHYSICAL_ADDRESS phys_addr, const void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	return static_cast<MSS *>(this)->DebugDataBusWrite(phys_addr, buffer, size, storage_attr);
}

template <typename TYPES, typename MSS>
inline bool MemorySubSystem<TYPES, MSS>::__MSS_DebugInstructionBusRead__(typename TYPES::PHYSICAL_ADDRESS phys_addr, void *buffer, unsigned int size, typename TYPES::STORAGE_ATTR storage_attr)
{
	if(!static_cast<MSS *>(this)->DebugInstructionBusRead(phys_addr, buffer, size, storage_attr)) return false;
	
	if(unlikely(enable_shadow_memory))
	{
		shadow_memory.write(phys_addr, (const uint8_t *) buffer, size);
	}

	return true;
}

template <typename TYPES, typename MSS>
inline void MemorySubSystem<TYPES, MSS>::__MSS_Trap__()
{
	static_cast<MSS *>(this)->Trap();
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline CacheSet<TYPES, typename CACHE::CACHE_CONFIG> *MemorySubSystem<TYPES, MSS>::LookupSetByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	if(CACHE::IsNullCache()) return 0;

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	return cache->LookupSetByAddress(addr, phys_addr);
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline CacheLine<TYPES, typename CACHE::CACHE_CONFIG> *MemorySubSystem<TYPES, MSS>::LookupLineByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	if(CACHE::IsNullCache()) return 0;

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	return cache->LookupLineByAddress(addr, phys_addr);
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline CacheBlock<TYPES, typename CACHE::CACHE_CONFIG> *MemorySubSystem<TYPES, MSS>::LookupBlockByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	if(CACHE::IsNullCache()) return 0;

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	return cache->LookupBlockByAddress(addr, phys_addr);
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::Lookup(CacheAccess<TYPES, CACHE>& access)
{
	if(unlikely(CACHE::IsNullCache()))
	{
		access.line_base_phys_addr = 0;
		access.block_base_phys_addr = 0;
		access.index = 0;
		access.way = 0;
		access.sector = 0;
		access.offset = 0;
		access.size_to_block_boundary = 0;
		access.set = 0;
		access.line = 0;
		access.line_to_write_back_evict = 0;
		access.block = 0;
		return;
	}

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	cache->Lookup(access);
	
	if(unlikely(cache->__MSS_IsVerbose__()))
	{
		__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Lookup at @0x"
		                             << std::hex << access.addr << std::dec << " (physically at @0x"
		                             << std::hex << access.phys_addr << std::dec << ") : "
		                             << "line_base_phys_addr=0x" << std::hex << access.line_base_phys_addr << std::dec << ","
		                             << "index=0x" << std::hex << access.index << std::dec << ","
		                             << "sector=0x" << std::hex << access.sector << std::dec << ","
		                             << "offset=0x" << std::hex << access.offset << std::dec << ","
		                             << "size_to_block_boundary=0x" << std::hex << access.size_to_block_boundary << std::dec
		                             << std::endl;
	}
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::InvalidateBlockByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	if(CACHE::IsNullCache()) return;

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	CacheBlock<TYPES, typename CACHE::CACHE_CONFIG> *block = cache->LookupBlockByAddress(addr, phys_addr);
	
	if(block)
	{
		if(unlikely(cache->__MSS_IsVerbose__()))
		{
			__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Invalidating Block at @0x" << std::hex << block->GetBaseAddress() << std::dec << " (physically at @0x" << std::hex << block->GetBasePhysicalAddress() << std::dec << ")" << std::endl;
		}
		block->Invalidate();
	}
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::InvalidateLineByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	if(CACHE::IsNullCache()) return;

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	CacheLine<TYPES, typename CACHE::CACHE_CONFIG> *line = cache->LookupLineByAddress(addr, phys_addr);
	
	if(line)
	{
		if(unlikely(cache->__MSS_IsVerbose__()))
		{
			__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Invalidating Line at @0x" << std::hex << line->GetBaseAddress() << std::dec << " (physically at @0x" << std::hex << line->GetBasePhysicalAddress() << std::dec << ")" << std::endl;
		}
		line->Invalidate();
	}
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::InvalidateBlockBySetWayAndSector(unsigned int index, unsigned int way, unsigned int sector)
{
	if(CACHE::IsNullCache()) return;

	if(index >= CACHE::NumSets())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetCacheName__() << ": Attempt to invalidate a block in a set with an invalid index 0x" << std::hex << index << std::dec << std::endl;
	}

	if(way >= CACHE::NumWays())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetCacheName__() << ": Attempt to invalidate a block in a line with an invalid way 0x" << std::hex << way << std::dec << std::endl;
	}
	
	if(sector >= CACHE::NumSectors())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetCacheName__() << ": Attempt to invalidate a block with an invalid sector 0x" << std::hex << sector << std::dec << std::endl;
	}

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	CacheSet<TYPES, typename CACHE::CACHE_CONFIG>& set = cache->GetSetByIndex(index);
	
	CacheLine<TYPES, typename CACHE::CACHE_CONFIG>& line = set->GetLineByWay(way);
	
	CacheBlock<TYPES, typename CACHE::CACHE_CONFIG>& block = line->GetBlockBySector(sector);
		
	if(unlikely(cache->__MSS_IsVerbose__()))
	{
		__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Invalidating Block in set #0x" << std::hex << set.GetIndex() << ", way #0x" << line.GetWay() << ", sector #0x" << block.GetSector() << std::dec << std::endl;
	}
		
	block.Invalidate();
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::InvalidateLineBySetAndWay(unsigned int index, unsigned int way)
{
	if(CACHE::IsNullCache()) return;

	if(index >= CACHE::NumSets())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetCacheName__() << ": Attempt to invalidate a line in a set with an invalid index 0x" << std::hex << index << std::dec << std::endl;
	}

	if(way >= CACHE::NumWays())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetCacheName__() << ": Attempt to invalidate a line with an invalid way 0x" << std::hex << way << std::dec << std::endl;
	}
	
	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	CacheSet<TYPES, typename CACHE::CACHE_CONFIG>& set = cache->GetSetByIndex(index);
	
	CacheLine<TYPES, typename CACHE::CACHE_CONFIG>& line = set->GetLineByWay(way);
	
	if(unlikely(cache->__MSS_IsVerbose__()))
	{
		__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Invalidating Line in set #0x" << std::hex << set.GetIndex() << ", way #0x" << line.GetWay() << std::dec << std::endl;
	}
	
	line.Invalidate();
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::InvalidateSet(unsigned int index)
{
	if(CACHE::IsNullCache()) return;

	if(index >= CACHE::NumSets())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetCacheName__() << ": Attempt to invalidate a set with an invalid index 0x" << std::hex << index << std::dec << std::endl;
	}

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	CacheSet<TYPES, typename CACHE::CACHE_CONFIG>& set = cache->GetSetByIndex(index);

	if(unlikely(cache->__MSS_IsVerbose__()))
	{
		__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Invalidating Set #0x" << std::hex << set.GetIndex() << std::dec << std::endl;
	}
	
	set.Invalidate();
}

template <typename TYPES, typename MSS>
template <typename CACHE>
inline void MemorySubSystem<TYPES, MSS>::InvalidateAll()
{
	if(CACHE::IsNullCache()) return;

	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	if(unlikely(cache->__MSS_IsVerbose__()))
	{
		__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Invalidating all" << std::endl;
	}

	cache->Invalidate();
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE, typename UNTIL>
inline void MemorySubSystem<TYPES, MSS>::GlobalInvalidateBlockByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	if(CACHE::IsNullCache()) return;

	InvalidateBlockByAddress<CACHE>(addr, phys_addr);

	if(!Same<CACHE, UNTIL>::VALUE)
	{
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		GlobalInvalidateBlockByAddress<CACHE_HIERARCHY, NLC, UNTIL>(addr, phys_addr);
	}
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE, typename UNTIL>
inline void MemorySubSystem<TYPES, MSS>::GlobalInvalidateLineByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	if(CACHE::IsNullCache()) return;

	InvalidateLineByAddress<CACHE>(addr, phys_addr);

	if(!Same<CACHE, UNTIL>::VALUE)
	{
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		GlobalInvalidateLineByAddress<CACHE_HIERARCHY, NLC, UNTIL>(addr, phys_addr);
	}
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE, bool INVALIDATE>
inline bool MemorySubSystem<TYPES, MSS>::WriteBackBlockBySetWayAndSector(unsigned int index, unsigned int way, unsigned int sector)
{
	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	if(index >= CACHE::NumSets())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetCacheName__() << ": Attempt to write back" << (INVALIDATE ? " and invalidate" :"") << " a block in a set with an invalid index 0x" << std::hex << index << std::dec << std::endl;
	}

	if(way >= CACHE::NumWays())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetCacheName__() << ": Attempt to write back" << (INVALIDATE ? " and invalidate" :"") << " a block in a line with an invalid way 0x" << std::hex << way << std::dec << std::endl;
	}
	
	if(sector >= CACHE::NumSectors())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetCacheName__() << ": Attempt to write back" << (INVALIDATE ? " and invalidate" :"") << " a block with an invalid sector 0x" << std::hex << sector << std::dec << std::endl;
	}

	CacheSet<TYPES, typename CACHE::CACHE_CONFIG>& set = cache->GetSetByIndex(index);
	
	CacheLine<TYPES, typename CACHE::CACHE_CONFIG>& line = set->GetLineByWay(way);

	if(line.IsValid())
	{
		CacheBlock<TYPES, typename CACHE::CACHE_CONFIG>& block = line->GetBlockBySector(sector);
		
		if(block.IsValid() && block.IsDirty())
		{
			CacheAccess<TYPES, CACHE> access;
			access.line_to_write_back_evict = &line;
		
			if(!WriteBackDirtyBlock<CACHE_HIERARCHY, CACHE, INVALIDATE>(access, block.GetSector())) return false;
		}
	}
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE, bool INVALIDATE>
inline bool MemorySubSystem<TYPES, MSS>::WriteBackLineBySetAndWay(unsigned int index, unsigned int way)
{
	CACHE *cache = __MSS_GetCache__<CACHE>();
	
	if(index >= CACHE::NumSets())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetCacheName__() << ": Attempt to write back" << (INVALIDATE ? " and invalidate" :"") << " a line in a set with an invalid index 0x" << std::hex << index << std::dec << std::endl;
	}

	if(way >= CACHE::NumWays())
	{
		__MSS_GetDebugWarningStream__() << CACHE::__MSS_GetCacheName__() << ": Attempt to write back" << (INVALIDATE ? " and invalidate" :"") << " a line with an invalid way 0x" << std::hex << way << std::dec << std::endl;
	}

	CacheSet<TYPES, typename CACHE::CACHE_CONFIG>& set = cache->GetSetByIndex(index);
	
	CacheLine<TYPES, typename CACHE::CACHE_CONFIG>& line = set->GetLineByWay(way);

	if(line.IsValid())
	{
		CacheAccess<TYPES, CACHE> access;
		access.line_to_write_back_evict = &line;
		
		if(!WriteBackLine<CACHE_HIERARCHY, CACHE, INVALIDATE>(access)) return false;
	}
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE, typename UNTIL, bool INVALIDATE>
inline bool MemorySubSystem<TYPES, MSS>::GlobalWriteBackBlockByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	if(CACHE::IsNullCache())
	{
		return true;
	}
	else
	{
		CACHE *cache = __MSS_GetCache__<CACHE>();
		
		unsigned int sector = SectorByPhysicalAddress(phys_addr);
		CacheLine<TYPES, typename CACHE::CACHE_CONFIG> *line = cache->LookupLineByAddress(addr, phys_addr);
		
		if(line)
		{
			CacheBlock<TYPES, typename CACHE::CACHE_CONFIG>& block = (*line)[sector];
			
			CacheAccess<TYPES, CACHE> access;
			
			access.line_to_write_back_evict = line;
			
			if(block.IsValid() && block.IsDirty())
			{
				// write back dirty block
				typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
				
				if(!WriteBackDirtyBlock<CACHE_HIERARCHY, CACHE, NLC, INVALIDATE>(access, block.GetSector())) return false;
			}
			
			if(INVALIDATE)
			{
				if(unlikely(cache->__MSS_IsVerbose__()))
				{
					__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Invalidating Block at @0x" << std::hex << block.GetBaseAddress() << std::dec << " (physically at @0x" << std::hex << block.GetBasePhysicalAddress() << std::dec << ")" << std::endl;
				}
				block.Invalidate();
			}
		}
	}

	if(!Same<CACHE, UNTIL>::VALUE)
	{
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		if(!GlobalWriteBackBlockByAddress<CACHE_HIERARCHY, NLC, UNTIL, INVALIDATE>(addr, phys_addr)) return false;
	}
	
	return true;
}

template <typename TYPES, typename MSS>
template <typename CACHE_HIERARCHY, typename CACHE, typename UNTIL, bool INVALIDATE>
inline bool MemorySubSystem<TYPES, MSS>::GlobalWriteBackLineByAddress(typename TYPES::ADDRESS addr, typename TYPES::PHYSICAL_ADDRESS phys_addr)
{
	if(CACHE::IsNullCache())
	{
		return true;
	}
	else
	{
		CACHE *cache = __MSS_GetCache__<CACHE>();
		
		CacheLine<TYPES, typename CACHE::CACHE_CONFIG> *line = cache->LookupLineByAddress(addr, phys_addr);
		
		if(line)
		{
			CacheAccess<TYPES, CACHE> access;
			
			// dummy initializations to avoid compilation warning
			access.addr = 0;
			access.phys_addr = 0;
			access.storage_attr = typename TYPES::STORAGE_ATTR();
			access.rwitm = false;
			access.line_base_addr = 0;
			access.line_base_phys_addr = 0;
			access.block_base_addr = 0;
			access.block_base_phys_addr = 0;
			access.index = 0;
			access.way = 0;
			access.sector = 0;
			access.offset = 0;
			access.size_to_block_boundary = 0;
			access.cache = cache;
			access.set = 0;
			access.line = line;
			access.block = 0; 
			
			access.line_to_write_back_evict = line;
			
			if(!WriteBackLine<CACHE_HIERARCHY, CACHE, INVALIDATE>(access)) return false;

			if(INVALIDATE)
			{
				if(unlikely(cache->__MSS_IsVerbose__()))
				{
					__MSS_GetDebugInfoStream__() << CACHE::__MSS_GetCacheName__() << ": Invalidating Line at @0x" << std::hex << line->GetBaseAddress() << std::dec << " (physically at @0x" << std::hex << line->GetBasePhysicalAddress() << std::dec << ")" << std::endl;
				}
				line->Invalidate();
			}
		}
	}

	if(!Same<CACHE, UNTIL>::VALUE)
	{
		typedef typename CACHE_HIERARCHY::template NextLevel<CACHE>::CACHE NLC;
		
		if(!GlobalWriteBackLineByAddress<CACHE_HIERARCHY, NLC, UNTIL, INVALIDATE>(addr, phys_addr)) return false;
	}
	
	return true;
}

} // end of namespace cache
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_CACHE_CACHE_HH__
