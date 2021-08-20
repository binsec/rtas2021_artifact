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

#ifndef SIMULATOR_CXX_TESLA_DRIVER_TCC
#define SIMULATOR_CXX_TESLA_DRIVER_TCC

#include <driver.hh>
#include <module.hh>
#include <iostream>
#include <cerrno>
#include "exception.hh"
using std::cerr;

/*****************************************
 *                DRIVER                 *
 *****************************************/
template<class CONFIG>
Driver<CONFIG>::Driver(unisim::component::cxx::memory::ram::Memory<typename CONFIG::address_t> * memory,
		CUDAScheduler<CONFIG> * scheduler,
		unsigned int core_count,
		CPU<CONFIG> ** cores,
		bool export_stats,
		char const * stats_prefix)
{
	device = shared_ptr<Device<CONFIG> >(new Device<CONFIG>(memory, scheduler, core_count, cores,
		export_stats, stats_prefix));
}

//    Initialization
template<class CONFIG>
CUresult Driver<CONFIG>::cuInit(unsigned int&Flags) {

	errno = 0;	// ?
	return CUDA_SUCCESS;
}

//    Device management
template<class CONFIG>
CUresult Driver<CONFIG>::cuDeviceGet(CUdevice *device, int ordinal)
{
	if(ordinal >= MAXDEVICE || ordinal < 0)
	{
		std::cerr << "Invalid device : " << ordinal << std::endl;
		return CUDA_ERROR_INVALID_DEVICE;
	}
	*device = ordinal;
	return CUDA_SUCCESS;
}

template<class CONFIG>
CUresult Driver<CONFIG>::cuDeviceGetCount(int *count)
{
	*count = MAXDEVICE;
	return CUDA_SUCCESS;
}

//    Context management
template<class CONFIG>
CUresult Driver<CONFIG>::cuCtxCreate(CUcontext *pctx, unsigned int flags, CUdevice dev )
{
	try
	{
		current_context = new CUctx_st(dev);
		*pctx = current_context;
	}
	catch(...)
	{
		std::cerr << "Cannot create context\n";
		return CUDA_ERROR_UNKNOWN;
	}
	return CUDA_SUCCESS;
}

template<class CONFIG>
CUresult Driver<CONFIG>::cuCtxDestroy( CUcontext ctx )
{
	try
	{
		delete ctx;
	}
	catch(...)
	{
		std::cerr << "Cannot destroy context\n";
		return CUDA_ERROR_UNKNOWN;
	}
	return CUDA_SUCCESS;
}

//    Module management
template<class CONFIG>
CUresult Driver<CONFIG>::ModuleLoad(Module<CONFIG> *  &module, const char *fname)
{
	try
	{
		module = new Module<CONFIG>(fname);
		//int dev = current_context->GetDevice();
	}
	// TODO: catch specific exceptions, display and return appropriate errors
	catch(...)
	{
		std::cerr << "Cannot load module\n";
		return CUDA_ERROR_UNKNOWN;
	}
	return CUDA_SUCCESS;
}

template<class CONFIG>
CUresult Driver<CONFIG>::ModuleUnload(Module<CONFIG> * hmod)
{
	try
	{
		// Destructor should not rise an exeption...
		delete hmod;
	}
	catch(...)
	{
		std::cerr << "Cannot unload module\n";
		return CUDA_ERROR_UNKNOWN;
	}
	return CUDA_SUCCESS;
}

#if 0
template<class CONFIG>
CUresult Driver<CONFIG>::cuModuleGetFunction(Kernel<CONFIG> **hfunc,
	Module<CONFIG> * hmod, const char *name)
{

}

//    Memory management
template<class CONFIG>
CUresult Driver<CONFIG>::cuMemGetInfo(unsigned int *free, unsigned int *total)
{

}
#endif

template<class CONFIG>
typename CONFIG::address_t Driver<CONFIG>::MAlloc(size_t size)
{
	return CurrentDevice().MAlloc(size);
}

template<class CONFIG>
void Driver<CONFIG>::Free(typename CONFIG::address_t addr)
{
	return CurrentDevice().Free(addr);
}

template<class CONFIG>
void Driver<CONFIG>::FunctionDump(Kernel<CONFIG> & kernel)
{
	CurrentDevice().DumpCode(kernel, std::cout);
}

template<class CONFIG>
void Driver<CONFIG>::Launch(Kernel<CONFIG> & kernel)
{
	CurrentDevice().Run(kernel);	// Synchronous call
}

template<class CONFIG>
void Driver<CONFIG>::LaunchGrid(Kernel<CONFIG> & kernel, int width, int height)
{
	CurrentDevice().Run(kernel, width, height);
}

template<class CONFIG>
void Driver<CONFIG>::CopyHtoD(typename CONFIG::address_t dest, void const * src, size_t size)
{
	CurrentDevice().CopyHtoD(dest, src, size);
}

template<class CONFIG>
void Driver<CONFIG>::CopyDtoH(void * dest, typename CONFIG::address_t src, size_t size)
{
	CurrentDevice().CopyDtoH(dest, src, size);
}

template<class CONFIG>
void Driver<CONFIG>::CopyDtoD(typename CONFIG::address_t dest, typename CONFIG::address_t src, size_t size)
{
	CurrentDevice().CopyDtoD(dest, src, size);
}

template<class CONFIG>
void Driver<CONFIG>::Memset(typename CONFIG::address_t dest, uint32_t val, size_t n)
{
	CurrentDevice().Memset(dest, val, n);
}

template<class CONFIG>
void Driver<CONFIG>::Memset(typename CONFIG::address_t dest, uint16_t val, size_t n)
{
	CurrentDevice().Memset(dest, val, n);
}

template<class CONFIG>
void Driver<CONFIG>::Memset(typename CONFIG::address_t dest, uint8_t val, size_t n)
{
	CurrentDevice().Memset(dest, val, n);
}

template<class CONFIG>
Device<CONFIG> const & Driver<CONFIG>::Dev(int dev) const
{
	if(dev < 0 || dev >= MAXDEVICE)
		throw CudaException(CUDA_ERROR_INVALID_DEVICE);
	//return device[dev];
	return *device;
}

template<class CONFIG>
Device<CONFIG> & Driver<CONFIG>::Dev(int dev)
{
	if(dev < 0 || dev >= MAXDEVICE)
		throw CudaException(CUDA_ERROR_INVALID_DEVICE);
	//return device[dev];
	return *device;
}

template<class CONFIG>
Device<CONFIG> const & Driver<CONFIG>::CurrentDevice() const
{
	int dev = current_context->GetDevice();
	return Dev(dev);
}

template<class CONFIG>
Device<CONFIG> & Driver<CONFIG>::CurrentDevice()
{
	int dev = current_context->GetDevice();
	return Dev(dev);
}

#endif
