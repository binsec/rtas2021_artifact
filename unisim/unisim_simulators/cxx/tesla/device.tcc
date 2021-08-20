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
 * Authors: 
 *     Sylvain Collange (sylvain.collange@univ-perp.fr)
 *
 */
 

#ifndef SIMULATOR_CXX_TESLA_DRIVER_DEVICE_TCC
#define SIMULATOR_CXX_TESLA_DRIVER_DEVICE_TCC

#include "device.hh"
#include <string>
#include <ostream>
#include <vector>
#include "kernel.hh"
#include <unisim/component/cxx/processor/tesla/stats.tcc>
#include <boost/thread.hpp>

using std::string;
using std::endl;
using std::hex;
using std::dec;
using boost::thread;
using boost::thread_group;


template<class CONFIG>
Allocator<CONFIG>::Allocator(unisim::component::cxx::memory::ram::Memory<typename CONFIG::address_t> & memory,
	typename CONFIG::address_t base, size_t size) :
	memory(memory), base(base), limit(base), max_size(size)
{
}


template<class CONFIG>
typename CONFIG::address_t Allocator<CONFIG>::Alloc(size_t size)
{
	assert(!(limit & (alignment - 1)));
	size = (size + (alignment - 1)) & ~(alignment - 1);	// align
	if(limit + size > base + max_size)
	{
		throw CudaException(CUDA_ERROR_OUT_OF_MEMORY);
	}
	typename CONFIG::address_t addr = limit;
	limit += size;

	// Force memory allocation
	Prealloc(addr, size);
	return addr;
}

template<class CONFIG>
void Allocator<CONFIG>::Free(typename CONFIG::address_t addr)
{
}

template<class CONFIG>
void Allocator<CONFIG>::GetInfo(unsigned int & free, unsigned int & total)
{
	total = max_size;
	free = max_size - (limit - base);
}

template<class CONFIG>
void Allocator<CONFIG>::Prealloc(typename CONFIG::address_t addr, size_t size)
{
	typename CONFIG::address_t blockStart, blockEnd;
	typename CONFIG::address_t endAddr = addr + size;
	while(addr < endAddr) {
		memory.GetDirectAccess(addr, blockStart, blockEnd);
		addr = blockEnd + 1;
	}
}



template<class CONFIG>
Device<CONFIG>::Device(unisim::component::cxx::memory::ram::Memory<typename CONFIG::address_t> * memory,
		CUDAScheduler<CONFIG> * scheduler,
		unsigned int core_count,
		CPU<CONFIG> ** cores,
		bool export_stats,
		char const * stats_prefix) :
	Object("device_0"),
	cores(cores),
	memory(memory),
	scheduler(scheduler),
	global_allocator(*memory, CONFIG::GLOBAL_START, CONFIG::GLOBAL_SIZE),
	core_count(core_count),
	export_stats(export_stats),
	stats_prefix(stats_prefix)
{

}

template<class CONFIG>
Device<CONFIG>::~Device()
{
	for(int i = 0; i != core_count; ++i)
	{
		delete cores[i];
	}
}

template<class CONFIG>
void Device<CONFIG>::Load(Kernel<CONFIG> & kernel)
{
	kernel.Load(*memory, global_allocator);
}

template<class CONFIG>
void Device<CONFIG>::DumpCode(Kernel<CONFIG> & kernel, std::ostream & os)
{
	Load(kernel);
	typename CONFIG::address_t pc = CONFIG::CODE_START;
	os << "Dumping code from " << kernel.Name() << " @" << std::hex << pc << endl;
	while(pc < CONFIG::CODE_START + kernel.CodeSize())
	{
		string s = cores[0]->Disasm(pc, pc);
		os << s << endl;
	}
}

template<class CONFIG>
void Device<CONFIG>::Run(Kernel<CONFIG> & kernel, int width, int height)
{
	DumpCode(kernel, cerr);
	Load(kernel);
	
	// Alloc shared memory
	global_allocator.Prealloc(CONFIG::SHARED_START, CONFIG::SHARED_SIZE * core_count);
	
	// Alloc local memory
	// Round up to warp size...
	global_allocator.Prealloc(CONFIG::LOCAL_START,
		core_count * kernel.WarpsPerBlock() * CONFIG::WARP_SIZE * kernel.BlocksPerCore()
		 * core_count * kernel.LocalTotal());
	
	kernel.SetGridShape(width, height);

	scheduler->Schedule(kernel);
	
	if(export_stats) {
		ExportStats(kernel.GetStats(), (kernel.Name() + ".csv").c_str());
	}
}

template<class CONFIG>
void Device<CONFIG>::ExportStats(typename CONFIG::stats_t const & stats, char const * filename)
{
	ofstream ofs((stats_prefix + string(filename)).c_str());
	stats.DumpCSV(ofs);
}

template<class CONFIG>
typename CONFIG::address_t Device<CONFIG>::MAlloc(size_t size)
{
	typename CONFIG::address_t addr = global_allocator.Alloc(size);
	cerr << "Device malloc: " << size << "B @" << hex << addr << dec << endl;
	return addr;
}

template<class CONFIG>
void Device<CONFIG>::Free(typename CONFIG::address_t addr)
{
	cerr << "Device free: @" << hex << addr << dec << endl;
	global_allocator.Free(addr);
}

template<class CONFIG>
void Device<CONFIG>::CopyHtoD(typename CONFIG::address_t dest, void const * src, size_t size)
{
	cerr << "Device copy " << size << "B to @" << hex << dest << dec << endl;
	if(!memory->WriteMemory(dest, src, size)) {
		throw CudaException(CUDA_ERROR_INVALID_VALUE);
	}
}

template<class CONFIG>
void Device<CONFIG>::CopyDtoH(void * dest, typename CONFIG::address_t src, size_t size)
{
	cerr << "Device copy " << size << "B from @" << hex << src << dec << endl;
	if(!memory->ReadMemory(src, dest, size)) {
		throw CudaException(CUDA_ERROR_INVALID_VALUE);
	}
}

template<class CONFIG>
void Device<CONFIG>::CopyDtoD(typename CONFIG::address_t dest, typename CONFIG::address_t src, size_t size)
{
	cerr << "Device copy " << size << "B from @" << hex << src << " to @" << dest << dec << endl;
	std::vector<uint8_t> buffer(size);
	if(!memory->ReadMemory(src, &buffer[0], size)) {
		throw CudaException(CUDA_ERROR_INVALID_VALUE);
	}
	if(!memory->WriteMemory(dest, &buffer[0], size)) {
		throw CudaException(CUDA_ERROR_INVALID_VALUE);
	}
}

template<class CONFIG>
void Device<CONFIG>::Memset(typename CONFIG::address_t dest, uint32_t val, size_t n)
{
	cerr << "Device memset: " << n * sizeof(val) << "B @" << hex << dest << dec << endl;
	for(unsigned int i = 0; i != n; ++i)
	{
		if(!memory->WriteMemory(dest + i * sizeof(val), &val, sizeof(val))) {
			throw CudaException(CUDA_ERROR_INVALID_VALUE);
		}
	}
}

template<class CONFIG>
void Device<CONFIG>::Memset(typename CONFIG::address_t dest, uint16_t val, size_t n)
{
	cerr << "Device memset: " << n * sizeof(val) << "B @" << hex << dest << dec << endl;
	for(unsigned int i = 0; i != n; ++i)
	{
		if(!memory->WriteMemory(dest + i * sizeof(val), &val, sizeof(val))) {
			throw CudaException(CUDA_ERROR_INVALID_VALUE);
		}
	}
}

template<class CONFIG>
void Device<CONFIG>::Memset(typename CONFIG::address_t dest, uint8_t val, size_t n)
{
	cerr << "Device memset: " << n * sizeof(val) << "B @" << hex << dest << dec << endl;
	for(unsigned int i = 0; i != n; ++i)
	{
		if(!memory->WriteMemory(dest + i * sizeof(val), &val, sizeof(val))) {
			throw CudaException(CUDA_ERROR_INVALID_VALUE);
		}
	}
}


template<class CONFIG>
void Device<CONFIG>::Reset()
{
}

template<class CONFIG>
std::string Device<CONFIG>::Name()
{
	return "Barra Simulator";
}

template<class CONFIG>
unsigned int Device<CONFIG>::TotalMem()
{
	cerr << "TotalMem: " << CONFIG::GLOBAL_SIZE << endl;
	return CONFIG::GLOBAL_SIZE;
}

template<class CONFIG>
int Device<CONFIG>::ComputeCapabilityMajor()
{
	return CONFIG::COMPUTE_CAP_MAJOR;
}

template<class CONFIG>
int Device<CONFIG>::ComputeCapabilityMinor()
{
	return CONFIG::COMPUTE_CAP_MINOR;
}

template<class CONFIG>
CUdevprop Device<CONFIG>::Properties()
{
	CUdevprop prop = {
		CONFIG::MAX_THREADS,	// maxThreadsPerBlock
		{ 512, 512, 64 },		// maxThreadsDim[3]
		{ 65535, 65535, 1 },	// maxGridSize[3]
		CONFIG::SHARED_SIZE,	// sharedMemPerBlock
		CONFIG::CONST_SEG_SIZE,	// totalConstantMemory
		CONFIG::WARP_SIZE,		// SIMDWidth
		262144,					// memPitch
		CONFIG::MAX_VGPR * CONFIG::WARP_SIZE,	// regsPerBlock
		CONFIG::SHADER_CLOCK_KHZ,	// clockRate
		256	// textureAlign
	};
	return prop;
}

template<class CONFIG>
int Device<CONFIG>::Attribute(int attrib)
{
	switch(attrib)
	{
	case CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK:
		return CONFIG::MAX_THREADS;
	case CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_X:
		return 512;
    case CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Y:
    	return 512;
    case CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Z:
    	return 64;
    case CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_X:
    	return 65535;
    case CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Y:
    	return 65535;
    case CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Z:
    	return 1;
    case CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_BLOCK:
    //case CU_DEVICE_ATTRIBUTE_SHARED_MEMORY_PER_BLOCK:
   		return CONFIG::SHARED_SIZE;
    case CU_DEVICE_ATTRIBUTE_TOTAL_CONSTANT_MEMORY:
    	return CONFIG::CONST_SEG_SIZE;
    case CU_DEVICE_ATTRIBUTE_WARP_SIZE:
    	return CONFIG::WARP_SIZE;
    case CU_DEVICE_ATTRIBUTE_MAX_PITCH:
    	return 262144;
    case CU_DEVICE_ATTRIBUTE_MAX_REGISTERS_PER_BLOCK:
    //case CU_DEVICE_ATTRIBUTE_REGISTERS_PER_BLOCK:
   		return CONFIG::MAX_VGPR * CONFIG::WARP_SIZE;
    case CU_DEVICE_ATTRIBUTE_CLOCK_RATE:
    	return CONFIG::SHADER_CLOCK_KHZ;
    case CU_DEVICE_ATTRIBUTE_TEXTURE_ALIGNMENT:
    	return 256;
	case CU_DEVICE_ATTRIBUTE_GPU_OVERLAP:
		return 0;
    case CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT:
    	return core_count;
	case CU_DEVICE_ATTRIBUTE_KERNEL_EXEC_TIMEOUT:
		return 0;  // No timeout
    case CU_DEVICE_ATTRIBUTE_INTEGRATED:
    	return 0;
    case CU_DEVICE_ATTRIBUTE_CAN_MAP_HOST_MEMORY:
    	return 0;
    case CU_DEVICE_ATTRIBUTE_COMPUTE_MODE:
    	return 0;  // CU_COMPUTEMODE_DEFAULT
	case 20080403:
		return 2030;    // Happy birthday!
	default:
    	throw CudaException(CUDA_ERROR_INVALID_VALUE);
	}
}

template<class CONFIG>
Allocator<CONFIG> & Device<CONFIG>::GlobalAllocator()
{
	return global_allocator;
}

template<class CONFIG>
void Device<CONFIG>::LoadSegment(MemSegment<CONFIG> & seg)
{
	seg.Load(*memory, global_allocator);
}

template<class CONFIG>
void Device<CONFIG>::MemGetInfo(unsigned int * free, unsigned int * total)
{
	global_allocator.GetInfo(*free, *total);
}

#endif
