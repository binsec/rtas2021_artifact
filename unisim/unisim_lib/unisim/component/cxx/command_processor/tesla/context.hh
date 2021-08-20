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
 
#ifndef UNISIM_COMPONENT_CXX_COMMAND_PROCESSOR_TESLA_CONTEXT_HH
#define UNISIM_COMPONENT_CXX_COMMAND_PROCESSOR_TESLA_CONTEXT_HH

#include <unisim/service/interfaces/allocator.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_direct_access.hh>
#include <unisim/service/interfaces/scheduler.hh>
#include <unisim/component/cxx/scheduler/cuda_scheduler/cuda_scheduler.hh>


namespace unisim {
namespace component {
namespace cxx {
namespace command_processor {
namespace tesla {

using unisim::service::interfaces::Allocator;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryDirectAccess;
using unisim::service::interfaces::Scheduler;
using unisim::component::cxx::scheduler::cuda_scheduler::CUDAGrid;

template <class CONFIG>
struct Context :
	Client<Allocator<typename CONFIG::address_t, uint32_t> >,
	Client<Memory<typename CONFIG::address_t> >,
	Client<MemoryDirectAccess<typename CONFIG::address_t> >,
	Client<Scheduler<CUDAGrid> >
{
	Context(char const * name, Object * parent = 0);
	bool Setup();
	
	ServiceImport<Allocator<typename CONFIG::address_t, uint32_t> > allocator_import;
	ServiceImport<Memory<typename CONFIG::address_t> > memory_import;
	ServiceImport<MemoryDirectAccess<typename CONFIG::address_t> > memory_direct_access_import;
	ServiceImport<Scheduler<CUDAGrid> > scheduler_import;
	
	typename CONFIG::address_t MAlloc(size_t size);
	void Free(typename CONFIG::address_t addr);

	void CopyHtoD(typename CONFIG::address_t dest, void const * src, size_t size);
	void CopyDtoH(void * dest, typename CONFIG::address_t src, size_t size);
	void CopyDtoD(typename CONFIG::address_t dest, typename CONFIG::address_t src, size_t size);

	void Memset(typename CONFIG::address_t dest, uint32_t val, size_t n);
	void Memset(typename CONFIG::address_t dest, uint16_t val, size_t n);
	void Memset(typename CONFIG::address_t dest, uint8_t val, size_t n);

	void Run(CUDAGrid<CONFIG> & kernel, int width = 1, int height = 1);
private:
	void Prealloc(typename CONFIG::address_t addr, uint32_t size);

	void DumpCode(Kernel<CONFIG> & kernel, std::ostream & os);
	void Load(Kernel<CONFIG> & kernel);
};



} // end of namespace tesla
} // end of namespace command_processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
