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
 
#ifndef UNISIM_COMPONENT_CXX_COMMAND_PROCESSOR_TESLA_CONTEXT_TCC
#define UNISIM_COMPONENT_CXX_COMMAND_PROCESSOR_TESLA_CONTEXT_TCC

#include <unisim/component/cxx/command_processor/tesla/context.hh>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace command_processor {
namespace tesla {

template<class CONFIG>
Context<CONFIG>::Context(char const * name, Object * parent) :
	Object(name, parent),
	Client<Allocator<typename CONFIG::address_t, uint32_t> >(name, parent),
	Client<Memory<typename CONFIG::address_t> >(name, parent),
	Client<MemoryDirectAccess<typename CONFIG::address_t> >(name, parent),
	Client<Scheduler<CUDAGrid> >(name, parent),
	allocator_import("allocator-import", this),
	memory_import("memory-import", this),
	memory_direct_access_import("memory-direct-access-import", this),
	scheduler_import("scheduler-import", this)
{
}

bool Context<CONFIG>::Setup()
{
	if(!allocator_import) return false;
	if(!memory_import) return false;
	
	return true;
}

template<class CONFIG>
typename CONFIG::address_t Context<CONFIG>::MAlloc(size_t size)
{
	typename CONFIG::address_t addr = allocator_import->Alloc(size);
	Prealloc(addr, size);
	// TODO: use logger
	cerr << "Device malloc: " << size << "B @" << hex << addr << dec << endl;
	return addr;
}

template<class CONFIG>
void Context<CONFIG>::Free(typename CONFIG::address_t addr)
{
	// TODO: logger
	cerr << "Device free: @" << hex << addr << dec << endl;
	allocator_import->Free(addr);
}

template<class CONFIG>
void Context<CONFIG>::Prealloc(typename CONFIG::address_t addr, uint32_t size)
{
	typename CONFIG::address_t blockStart, blockEnd;
	typename CONFIG::address_t endAddr = addr + size;
	while(addr < endAddr) {
		if(memory_direct_access_import)
			memory_direct_access_import->GetDirectAccess(addr, blockStart, blockEnd);
		addr = blockEnd + 1;
	}
}

template<class CONFIG>
void Context<CONFIG>::CopyHtoD(typename CONFIG::address_t dest, void const * src, size_t size)
{
	cerr << "Device copy " << size << "B to @" << hex << dest << dec << endl;
	if(!memory_import->WriteMemory(dest, src, size)) {
		throw CudaException(CUDA_ERROR_INVALID_VALUE);
	}
}

template<class CONFIG>
void Context<CONFIG>::CopyDtoH(void * dest, typename CONFIG::address_t src, size_t size)
{
	cerr << "Device copy " << size << "B from @" << hex << src << dec << endl;
	if(!memory_import->ReadMemory(src, dest, size)) {
		throw CudaException(CUDA_ERROR_INVALID_VALUE);
	}
}

template<class CONFIG>
void Context<CONFIG>::CopyDtoD(typename CONFIG::address_t dest, typename CONFIG::address_t src, size_t size)
{
	cerr << "Device copy " << size << "B from @" << hex << src << " to @" << dest << dec << endl;
	std::vector<uint8_t> buffer(size);
	if(!memory_import->ReadMemory(src, &buffer[0], size)) {
		throw CudaException(CUDA_ERROR_INVALID_VALUE);
	}
	if(!memory_import->WriteMemory(dest, &buffer[0], size)) {
		throw CudaException(CUDA_ERROR_INVALID_VALUE);
	}
}

template<class CONFIG>
void Context<CONFIG>::Memset(typename CONFIG::address_t dest, uint32_t val, size_t n)
{
	cerr << "Device memset: " << n * sizeof(val) << "B @" << hex << dest << dec << endl;
	for(unsigned int i = 0; i != n; ++i)
	{
		if(!memory_import->WriteMemory(dest + i * sizeof(val), &val, sizeof(val))) {
			throw CudaException(CUDA_ERROR_INVALID_VALUE);
		}
	}
}

template<class CONFIG>
void Context<CONFIG>::Memset(typename CONFIG::address_t dest, uint16_t val, size_t n)
{
	cerr << "Device memset: " << n * sizeof(val) << "B @" << hex << dest << dec << endl;
	for(unsigned int i = 0; i != n; ++i)
	{
		if(!memory_import->WriteMemory(dest + i * sizeof(val), &val, sizeof(val))) {
			throw CudaException(CUDA_ERROR_INVALID_VALUE);
		}
	}
}

template<class CONFIG>
void Context<CONFIG>::Memset(typename CONFIG::address_t dest, uint8_t val, size_t n)
{
	cerr << "Device memset: " << n * sizeof(val) << "B @" << hex << dest << dec << endl;
	for(unsigned int i = 0; i != n; ++i)
	{
		if(!memory_import->WriteMemory(dest + i * sizeof(val), &val, sizeof(val))) {
			throw CudaException(CUDA_ERROR_INVALID_VALUE);
		}
	}
}

template<class CONFIG>
void Context<CONFIG>::Run(Kernel<CONFIG> & kernel, int width, int height)
{
	// TODO TODO TODO
	DumpCode(kernel, cerr);
	Load(kernel);
	
	// Alloc shared memory
	Prealloc(CONFIG::SHARED_START, CONFIG::SHARED_SIZE * core_count);
	
	// Alloc local memory
	// Round up to warp size...
	Prealloc(CONFIG::LOCAL_START,
		core_count * kernel.WarpsPerBlock() * CONFIG::WARP_SIZE * kernel.BlocksPerCore()
		 * core_count * kernel.LocalTotal());
	
	kernel.SetGridShape(width, height);

	scheduler->Schedule(kernel);
	
	if(export_stats) {
		ExportStats(kernel.GetStats(), (kernel.Name() + ".csv").c_str());
	}
}

template<class CONFIG>
void Context<CONFIG>::DumpCode(Kernel<CONFIG> & kernel, std::ostream & os)
{
	// TODO TODO TODO
	Load(kernel);
	typename CONFIG::address_t pc = CONFIG::CODE_START;
	os << "Dumping code from " << kernel.Name() << " @" << std::hex << pc << endl;
	while(pc < CONFIG::CODE_START + kernel.CodeSize())
	{
		string s = cores[0]->Disasm(pc, pc);
		os << s << endl;
	}
}



} // end of namespace tesla
} // end of namespace command_processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
