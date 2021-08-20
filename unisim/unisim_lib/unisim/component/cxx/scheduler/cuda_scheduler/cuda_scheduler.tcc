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
 
#ifndef UNISIM_COMPONENT_CXX_SCHEDULER_CUDA_SCHEDULER_TCC
#define UNISIM_COMPONENT_CXX_SCHEDULER_CUDA_SCHEDULER_TCC

#include <unisim/component/cxx/scheduler/cuda_scheduler/cuda_scheduler.hh>
#include <boost/thread.hpp>

namespace unisim {
namespace component {
namespace cxx {
namespace scheduler {
namespace cuda_scheduler {

using boost::thread;
using boost::thread_group;


template<class CPUCONFIG>
CoreSocket<CPUCONFIG>::CoreSocket(const char *name, Object *parent) :
	Object(name, parent),
	Client<TypedRegisters<uint32_t, GPRID> >(name, parent),
	Client<TypedRegisters<uint32_t, ConfigurationRegisterID> >(name, parent),
	Client<TypedRegisters<SamplerBase<typename CPUCONFIG::address_t>, SamplerIndex> >(name, parent),
	Client<Memory<SMAddress> >(name, parent),
	Client<Resetable>(name, parent),
	Client<Runnable>(name, parent),
	Client<InstructionStats<typename CPUCONFIG::stats_t> >(name, parent),
	registers_import("registers-import", this),
	configuration_import("configuration-import", this),
	samplers_import("samplers-import", this),
	shared_memory_import("shared-memory-import", this),
	reset_import("reset-import", this),
	run_import("run-import", this),
	stats_import("stats-import", this)
{
}

template<class CPUCONFIG>
bool CoreSocket<CPUCONFIG>::Setup()
{
	return true;
}

template<class CONFIG>
int ThreadsPerBlock(CUDAGrid<CONFIG> const & kernel)
{
	return kernel.BlockZ() * kernel.BlockY() * kernel.BlockX();
}

template<class CONFIG>
int WarpsPerBlock(CUDAGrid<CONFIG> const & kernel)
{
	return (ThreadsPerBlock(kernel) + CONFIG::WARP_SIZE - 1) / CONFIG::WARP_SIZE;
}


template<class CONFIG>
int BlocksPerCore(CUDAGrid<CONFIG> const & kernel)
{
	int blocksreg = CONFIG::MAX_VGPR / (kernel.GPRs() * WarpsPerBlock<CONFIG>(kernel));
	int blockssm = CONFIG::SHARED_SIZE / kernel.SharedTotal(); // Already aligned
	int blockswarps = CONFIG::MAX_WARPS / WarpsPerBlock<CONFIG>(kernel);
	
	return std::min(std::min(blocksreg, blockssm), std::min(int(CONFIG::MAX_CTAS), blockswarps));
}

inline uint32_t BuildTID(int x, int y, int z)
{
	return (z << 26) | (y << 16) | x;
}

template<class CONFIG>
void SetThreadIDs(CUDAGrid<CONFIG> const & kernel, int bnum, CoreSocket<CONFIG> & cpu)
{
	// Set register 0 of each thread
	int warpsperblock = WarpsPerBlock<CONFIG>(kernel);
	int blockstart = bnum * warpsperblock;

	for(int z = 0; z != kernel.BlockZ(); ++z)
	{
		for(int y = 0; y != kernel.BlockY(); ++y)
		{
			for(int x = 0; x != kernel.BlockX(); ++x)
			{
				int tid = ((z * kernel.BlockY()) + y) * kernel.BlockX() + x;
				int warpid = blockstart + tid / CONFIG::WARP_SIZE;
				int lane = tid % CONFIG::WARP_SIZE;
				
				uint32_t reg = BuildTID(x, y, z);
				GPRID r0id(warpid, 0, lane);
				cpu.registers_import->WriteTypedRegister(r0id, reg);
			}
		}
	}
}

template<class CONFIG>
void InitShared(CUDAGrid<CONFIG> const & kernel, Memory<SMAddress> & mem, int index,
	int bidx, int bidy, int core)
{
	// Header
	uint16_t header[8];	// 10?
	header[0] = 0;	// flags? gridid?
	header[1] = kernel.BlockX();
	header[2] = kernel.BlockY();
	header[3] = kernel.BlockZ();
	header[4] = kernel.GridX();	// gridx
	header[5] = kernel.GridY();	// gridy
	header[6] = bidx;	// bidx
	header[7] = bidy;	// bidy

	//if(trace_loading)
	//	cerr << "Init block " << index << " (" << bidx << ", " << bidy << ") / ("
	//		<< gridx << ", " << gridy << ") shared memory\n";

	if(!mem.WriteMemory(SMAddress(index, 0), header, 16)) {
		assert(false);
		//throw CudaException(CUDA_ERROR_OUT_OF_MEMORY);
	}
	
	//if(trace_loading)
	//	cerr << "Loading " << param_size << "B parameters in core "
	//	     << core << ", block " << index << endl;
	
	// Parameters
	if(!mem.WriteMemory(SMAddress(index, 16), kernel.GetParameters(), kernel.ParametersSize())) {
		assert(false);
		//throw CudaException(CUDA_ERROR_OUT_OF_MEMORY);
	}
}


template<class CONFIG>
struct Runner
{
	CoreSocket<CONFIG> & core;
	int c;
	CUDAGrid<CONFIG> const & kernel;
	unsigned int core_count;
	
	Runner(CoreSocket<CONFIG> & core, CUDAGrid<CONFIG> const & kernel,
		int core_count, int c) :
		core(core), c(c), kernel(kernel),
		core_count(core_count) {}
	void operator() ();
};

template<class CONFIG>
void Runner<CONFIG>::operator() ()
{
	int blocksPerCore = BlocksPerCore<CONFIG>(kernel);
	int blocksPerGPU = blocksPerCore * core_count;
	int width = kernel.GridX();
	int height = kernel.GridY();
	
	// Simple round-robin scheduling policy
	for(int i = c * blocksPerCore; i < height * width; i += blocksPerGPU)
	{
		int blocks = std::min(blocksPerCore, height * width - i);
		
		if(blocks > 0)
		{
			// Set configuration registers
			core.configuration_import->WriteTypedRegister(
				ConfigurationRegisterID(ConfigurationRegisterID::ThreadsPerBlock),
				ThreadsPerBlock(kernel));
			core.configuration_import->WriteTypedRegister(
				ConfigurationRegisterID(ConfigurationRegisterID::Blocks),
				blocks);
			core.configuration_import->WriteTypedRegister(
				ConfigurationRegisterID(ConfigurationRegisterID::GPRsPerWarp),
				kernel.GPRs());
			core.configuration_import->WriteTypedRegister(
				ConfigurationRegisterID(ConfigurationRegisterID::SMSize),
				kernel.SharedTotal());
			
			//cpu->Reset(kernel.ThreadsPerBlock(), blocks, kernel.GPRs(), kernel.SharedTotal());
			core.reset_import->Reset();

			for(int j = 0; j < blocks; ++j)
			{
				int bidx = (i+j) % width;
				int bidy = (i+j) / width;
				InitShared<CONFIG>(kernel, *core.shared_memory_import, j, bidx, bidy, c);
				SetThreadIDs(kernel, j, core);
			}

			// Start execution
			core.run_import->Run();
		}
		
		// TODO: Sync
	}
}


template<class CONFIG>
CUDAScheduler<CONFIG>::CUDAScheduler(unsigned int cores, const char *name, Object *parent) :
	Object(name, parent),
	Service<Scheduler<CUDAGrid<CONFIG> > >(name, parent),
	scheduler_export("scheduler-export", this),
	core_count(cores),
	sockets(cores),
	param_export_stats("export-stats", this, export_stats),
	export_stats(false)
{
	for(unsigned int i = 0; i != cores; ++i)
	{
		std::ostringstream name;
		name << "scheduler_socket_" << i;
		sockets[i] = shared_ptr<CoreSocket<CONFIG> >(new CoreSocket<CONFIG>(name.str().c_str(), this));
	}
}


template<class CONFIG>
bool CUDAScheduler<CONFIG>::Setup()
{
	return true;
}


template<class CONFIG>
void CUDAScheduler<CONFIG>::Schedule(CUDAGrid<CONFIG> & kernel)
{
	thread_group GPUThreads;

	std::vector<Stats<CONFIG> > temp_stats(core_count);
	for(unsigned int c = 0; c != core_count; ++c) {
		Socket(c).stats_import->SetStats(&temp_stats[c]);
		Socket(c).stats_import->InitStats(kernel.CodeSize());	// Even if not exporting
	}

	for(unsigned int c = 0; c != core_count; ++c) {
		for(unsigned int s = 0; s != kernel.SamplersSize(); ++s) {
			SamplerBase<typename CONFIG::address_t> const * smp = kernel.GetSampler(s);
			if(smp != 0) {
				Socket(c).samplers_import->WriteTypedRegister(SamplerIndex(s), *smp);
			}
		}
	}
	
	for(unsigned int c = 1; c < core_count; ++c)
	{
		Runner<CONFIG> runner(Socket(c), kernel, core_count, c);
		GPUThreads.create_thread(runner);
	}
	
	// Run first block on current thread
	Runner<CONFIG>(Socket(0), kernel, core_count, 0)();
	
	GPUThreads.join_all();

	for(unsigned int c = 0; c != core_count; ++c) {
		Socket(c).stats_import->SetStats(0);
	}
	
	if(export_stats) {
		for(int c = 0; c != core_count; ++c)
		{
			kernel.GetStats().Merge(temp_stats[c]);
		}
	}
}

template<class CONFIG>
CoreSocket<CONFIG> & CUDAScheduler<CONFIG>::Socket(unsigned int i)
{
	return *sockets[i];
}

} // end of namespace cuda_scheduler
} // end of namespace scheduler
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
