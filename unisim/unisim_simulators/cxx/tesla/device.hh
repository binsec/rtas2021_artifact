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
 
#ifndef SIMULATOR_CXX_TESLA_DRIVER_DEVICE_HH
#define SIMULATOR_CXX_TESLA_DRIVER_DEVICE_HH

#include "driver_objects.hh"
#include "module.hh"
#include <iosfwd>
#include <boost/shared_ptr.hpp>

#include <unisim/component/cxx/processor/tesla/cpu.hh>
//#include <unisim/component/cxx/processor/tesla/cpu.tcc>
#include <unisim/component/cxx/processor/tesla/config.hh>
#include <unisim/component/cxx/memory/ram/memory.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/component/cxx/scheduler/cuda_scheduler/cuda_scheduler.hh>

using boost::shared_ptr;
using unisim::component::cxx::processor::tesla::CPU;
using unisim::component::cxx::processor::tesla::BaseConfig;
//using unisim::component::cxx::memory::ram::Memory;
using unisim::kernel::Object;
using unisim::component::cxx::scheduler::cuda_scheduler::CUDAScheduler;

// Trivial allocator, no deallocation at all
// for the moment
// TODO: implement cuMemGetAddressRange
template<class CONFIG>
struct Allocator
{
	Allocator(unisim::component::cxx::memory::ram::Memory<typename CONFIG::address_t> & memory, 
		typename CONFIG::address_t base, size_t max_size);
	
	typename CONFIG::address_t Alloc(size_t size);
	void Free(typename CONFIG::address_t addr);
	
	void GetInfo(unsigned int & free, unsigned int & total);
	void Prealloc(typename CONFIG::address_t addr, size_t size);

private:
	
	unisim::component::cxx::memory::ram::Memory<typename CONFIG::address_t> & memory;
	typename CONFIG::address_t base;
	typename CONFIG::address_t limit;
	size_t max_size;
	
	static unsigned int const alignment = 256;

};

template<class CONFIG>
struct Device: CUdevice_st, virtual Object
{
	Device(unisim::component::cxx::memory::ram::Memory<typename CONFIG::address_t> * memory,
		CUDAScheduler<CONFIG> * scheduler,
		unsigned int core_count,
		CPU<CONFIG> ** cores,
		bool export_stats,
		char const * stats_prefix);
		
	~Device();

	void DumpCode(Kernel<CONFIG> & kernel, std::ostream & os);
	void Run(Kernel<CONFIG> & kernel, int width = 1, int height = 1);
	
	typename CONFIG::address_t MAlloc(size_t size);
	void Free(typename CONFIG::address_t addr);

	void CopyHtoD(typename CONFIG::address_t dest, void const * src, size_t size);
	void CopyDtoH(void * dest, typename CONFIG::address_t src, size_t size);
	void CopyDtoD(typename CONFIG::address_t dest, typename CONFIG::address_t src, size_t size);

	void Memset(typename CONFIG::address_t dest, uint32_t val, size_t n);
	void Memset(typename CONFIG::address_t dest, uint16_t val, size_t n);
	void Memset(typename CONFIG::address_t dest, uint8_t val, size_t n);
	
	std::string Name();
	unsigned int TotalMem();
	int ComputeCapabilityMajor();
	int ComputeCapabilityMinor();
	CUdevprop Properties();
	int Attribute(int attrib);
	Allocator<CONFIG> & GlobalAllocator();
	
	void LoadSegment(MemSegment<CONFIG> & seg);
	
	void ExportStats(typename CONFIG::stats_t const & stats, char const * filename);
	
	void MemGetInfo(unsigned int * free, unsigned int * total);

private:
	void Load(Kernel<CONFIG> & kernel);
	void Reset();
	//void SetThreadIDs(Kernel<CONFIG> const & kernel, int bnum, int core);
	//uint32_t BuildTID(int x, int y, int z);

	CPU<CONFIG> ** cores;
	unisim::component::cxx::memory::ram::Memory<typename CONFIG::address_t> * memory;
	CUDAScheduler<CONFIG> * scheduler;
	
	Allocator<CONFIG> global_allocator;
	unsigned int core_count;
	
	bool export_stats;
	std::string stats_prefix;
};



#endif
