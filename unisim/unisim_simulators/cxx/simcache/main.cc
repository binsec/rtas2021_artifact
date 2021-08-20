/*
 *  Copyright (c) 2019-2020,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <unisim/util/cache/cache.hh>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <inttypes.h>

struct StorageAttr {};

struct AccessLog
{
  AccessLog(std::ostream& _sink) : sink(_sink) {}
  
  bool DataBusWrite(uint32_t phys_addr, void const* buffer, unsigned int size, StorageAttr storage_attr)
  {
    sink << "Writing @" << std::hex << phys_addr << std::dec << ", size = " << size << "\n";
    return true;
  }

  bool DataBusRead(uint32_t phys_addr, void* buffer, unsigned int size, StorageAttr storage_attr, bool rwitm)
  {
    sink << "Reading @" << std::hex << phys_addr << std::dec << ", size = " << size << (rwitm ? " with intent to modify" : "") << "\n";
    return true;
  }

  std::ostream& sink;
};

struct MSSConfig
{
  typedef StorageAttr STORAGE_ATTR;
  typedef uint32_t PHYSICAL_ADDRESS;
  typedef uint32_t ADDRESS;
};

struct MemorySubSystem : public unisim::util::cache::MemorySubSystem<MSSConfig,MemorySubSystem>, public AccessLog
{
  typedef unisim::util::cache::MemorySubSystem<MSSConfig,MemorySubSystem> BaseMSS;
  using AccessLog::DataBusRead;
  using AccessLog::DataBusWrite;
  
  MemorySubSystem( std::ostream& sink ) : BaseMSS(), AccessLog(sink) {}
  
  bool IsStorageCacheable(StorageAttr storage_attr) const { return true; }

  // Tightly Coupled Memories (none)
  typedef unisim::util::cache::LocalMemorySet<MSSConfig> DATA_LOCAL_MEMORIES;
  typedef unisim::util::cache::LocalMemorySet<MSSConfig> INSTRUCTION_LOCAL_MEMORIES;

  // Cache memories
  struct L1D_CONFIG
  {
    static const unsigned int SIZE                                      = 4096;
    static const unisim::util::cache::CacheWritingPolicy WRITING_POLICY = unisim::util::cache::CACHE_WRITE_BACK_AND_NO_WRITE_ALLOCATE_POLICY;
    static const unisim::util::cache::CacheIndexScheme INDEX_SCHEME     = unisim::util::cache::CACHE_PHYSICALLY_INDEXED;
    static const unisim::util::cache::CacheTagScheme TAG_SCHEME         = unisim::util::cache::CACHE_PHYSICALLY_TAGGED;
    static const unisim::util::cache::CacheType TYPE                    = unisim::util::cache::DATA_CACHE;
    static const unsigned int ASSOCIATIVITY                             = 4;
    static const unsigned int BLOCK_SIZE                                = 4;
    static const unsigned int BLOCKS_PER_LINE                           = 8;
    
    //    typedef CACHE_CPU CPU;
    struct LINE_STATUS : unisim::util::cache::CacheLineStatus {};
    struct BLOCK_STATUS : unisim::util::cache::CacheBlockStatus {};
    struct SET_STATUS : unisim::util::cache::CacheSetStatus { unisim::util::cache::LRU_ReplacementPolicy<ASSOCIATIVITY> lru; };
  };
  
  struct L1D : public unisim::util::cache::Cache<MSSConfig, L1D_CONFIG, L1D>
  {
    static const char *GetCacheName() { return "L1D"; }
    bool IsEnabled() const { return true; }

    static bool IsStaticWriteAllocate() { return true; }

    bool ChooseLineToEvict(unisim::util::cache::CacheAccess<MSSConfig, L1D>& access)
    {
      access.way = access.set->Status().lru.Select();
      return true;
    }

    void UpdateReplacementPolicyOnAccess(unisim::util::cache::CacheAccess<MSSConfig, L1D>& access)
    {
      access.set->Status().lru.UpdateOnAccess(access.way);
    }
  } l1d;
  L1D* GetCache(const L1D* ) ALWAYS_INLINE { return &l1d; }
  
  typedef unisim::util::cache::CacheHierarchy<MSSConfig, L1D> DATA_CACHE_HIERARCHY;

  typedef unisim::util::cache::CacheHierarchy<MSSConfig> INSTRUCTION_CACHE_HIERARCHY;
  // typedef MSSConfig::STORAGE_ATTR STORAGE_ATTR;
  
};

int
main( int argc, char *argv[] )
{
  std::ofstream wcache_log("wcache.txt"), ncache_log("ncache.txt");
  
  MemorySubSystem mss(wcache_log);
  AccessLog alog(ncache_log);
  StorageAttr storage_attr;

  srand(0);

  for (uint32_t step = 0; step < 0x10000; step += 1)
    {
      uint32_t addr =  (uint32_t(rand()) % 0x1000) & -4, value = addr;
      //      if (rand() % 2)
      if (rand() % 2)
        {
          alog.DataBusRead(addr, &value, sizeof(value), storage_attr, false);
          mss.DataLoad(addr, addr, &value, sizeof(value), storage_attr);
        }
      else
        {
          alog.DataBusWrite(addr, &value, sizeof(value), storage_attr);
          mss.DataStore(addr, addr, &value, sizeof(value), storage_attr);
        }
    }
  std::cout << "Hello simcache!\n";
  
  return 0;
}
