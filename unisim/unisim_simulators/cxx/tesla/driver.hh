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
 *     David Parello (david.parello@univ-perp.fr)
 *     Sylvain Collange (sylvain.collange@univ-perp.fr)
 *
 */
 
#ifndef SIMULATOR_CXX_TESLA_DRIVER_HH
#define SIMULATOR_CXX_TESLA_DRIVER_HH

#include <boost/shared_ptr.hpp>
#include <driver_objects.hh>
#include <module.hh>
#include <device.hh>
#include <unisim/component/cxx/processor/tesla/cpu.hh>
#include <unisim/component/cxx/processor/tesla/config.hh>
#include <unisim/component/cxx/memory/ram/memory.hh>
#include <unisim/component/cxx/scheduler/cuda_scheduler/cuda_scheduler.hh>

using unisim::component::cxx::processor::tesla::CPU;
using unisim::component::cxx::processor::tesla::BaseConfig;
//using unisim::component::cxx::memory::ram::Memory;
using unisim::kernel::Object;
using unisim::component::cxx::scheduler::cuda_scheduler::CUDAScheduler;
using boost::shared_ptr;

/*****************************************
 *                DRIVER                 *
 *****************************************/
template<class CONFIG>
struct Driver
{
public:
	//  const usigned int MAX_CONTEXT=1;
	static const int MAXDEVICE=1;

	Driver(unisim::component::cxx::memory::ram::Memory<typename CONFIG::address_t> * memory,
		CUDAScheduler<CONFIG> * scheduler,
		unsigned int core_count,
		CPU<CONFIG> ** cores,
		bool export_stats,
		char const * stats_prefix);

	//    Initialization
	CUresult  cuInit(unsigned int&Flags);

	//    Device management
	CUresult  cuDeviceGet(CUdevice *device, int ordinal);

	CUresult  cuDeviceGetCount(int *count);

	//    Context management
	CUresult  cuCtxCreate(CUcontext *pctx, unsigned int flags, CUdevice dev );
	CUresult  cuCtxDestroy( CUcontext ctx );

	//    Module management
	CUresult ModuleLoad(Module<CONFIG> * & module, const char *fname);
	CUresult ModuleUnload(Module<CONFIG> * hmod);
	
	//  CUresult cuModuleGetFunction(Kernel<CONFIG> **hfunc, Module<CONFIG> * hmod, const char *name);

	//    Memory management
	typename CONFIG::address_t MAlloc(size_t size);
	void Free(typename CONFIG::address_t addr);
 
	void FunctionDump(Kernel<CONFIG> & kernel);
	void Launch(Kernel<CONFIG> & kernel);
	void LaunchGrid(Kernel<CONFIG> & kernel, int width, int height);
	
	void CopyHtoD(typename CONFIG::address_t dest, void const * src, size_t size);
	void CopyDtoH(void * dest, typename CONFIG::address_t src, size_t size);
	void CopyDtoD(typename CONFIG::address_t dest, typename CONFIG::address_t src, size_t size);
	
	void Memset(typename CONFIG::address_t dest, uint32_t val, size_t n);
	void Memset(typename CONFIG::address_t dest, uint16_t val, size_t n);
	void Memset(typename CONFIG::address_t dest, uint8_t val, size_t n);
	
	Device<CONFIG> const & Dev(int dev) const;
	Device<CONFIG> & Dev(int dev);

	Device<CONFIG> const & CurrentDevice() const;
	Device<CONFIG> & CurrentDevice();

private:
	CUcontext current_context;
//	Device<CONFIG> device[MAXDEVICE];
	shared_ptr<Device<CONFIG> > device;

};

#endif

