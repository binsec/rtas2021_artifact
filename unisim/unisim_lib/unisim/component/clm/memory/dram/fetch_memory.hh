#ifndef __UNISIM_COMPONENT_CLM_MEMORY_DRAM_FETCH_MEMORY_HH__
#define __UNISIM_COMPONENT_CLM_MEMORY_DRAM_FETCH_MEMORY_HH__

#include <unisim/component/cxx/memory/ram/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/kernel/kernel.hh>

namespace unisim {
namespace component {
namespace clm {
namespace memory {
namespace dram {

  //using unisim::component::cxx::memory::ram::Memory;
// *fetchmemory = new unisim::component::cxx::memory::ram::Memory<address_t>("fetchmem");
using unisim::kernel::Service;
using unisim::service::interfaces::MemoryInjection;
using unisim::kernel::ServiceExport;
  using unisim::kernel::Object;

template < class TYPE_ADDR >
class Fetch_Memory: 
    public unisim::component::cxx::memory::ram::Memory<TYPE_ADDR>
    , public Service< MemoryInjection< TYPE_ADDR> >
{
public:
  //  ServiceExport<MemoryInjection<address_t> > memory_injection_export;
  ServiceExport<MemoryInjection<TYPE_ADDR> > memory_injection_export;

  Fetch_Memory(const char *name) :
    //    module(name)
    Object(name, 0)
    , unisim::component::cxx::memory::ram::Memory<TYPE_ADDR>(name)
    , Service<MemoryInjection<TYPE_ADDR> >(name, 0)
    , memory_injection_export("memory_injection_export", this)
  {
    // Nothing to do...
  }

  bool InjectReadMemory(TYPE_ADDR addr, void *buffer, uint32_t size)
  {
    return ReadMemory(addr, buffer, size);
  }

  bool InjectWriteMemory(TYPE_ADDR addr, const void *buffer, uint32_t size)
  {
    return WriteMemory(addr, buffer, size);
  }

};

} // end of namespace dram
} // end of namespace memory
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif //  __UNISIM_COMPONENT_CLM_MEMORY_DRAM_FETCH_MEMORY_HH__
