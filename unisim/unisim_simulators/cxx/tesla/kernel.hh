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
 
#ifndef SIMULATOR_CXX_TESLA_KERNEL_HH
#define SIMULATOR_CXX_TESLA_KERNEL_HH

#include <string>
#include <iosfwd>
#include <vector>
#include <list>
#include <map>
#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/component/cxx/processor/tesla/interfaces.hh>
#include <unisim/component/cxx/processor/tesla/stats.hh>
#include <unisim/component/cxx/scheduler/cuda_scheduler/cuda_scheduler.hh>

using unisim::kernel::Service;
using unisim::component::cxx::processor::tesla::CPU;
using unisim::component::cxx::processor::tesla::Stats;
using unisim::component::cxx::scheduler::cuda_scheduler::CUDAGrid;
using unisim::component::cxx::processor::tesla::SMAddress;

//using unisim::service::interfaces::Memory;
// Conflicts with ram::Memory, do NOT use in headers

template<class CONFIG>
struct MemSegment;

template<class CONFIG>
struct Module;

template<class CONFIG>
struct Sampler;

template<class CONFIG>
struct Allocator;

template<class CONFIG>
struct Loadable
{
	virtual void Load(Service<unisim::service::interfaces::Memory<typename CONFIG::address_t> > & mem,
		Allocator<CONFIG> & allocator, uint32_t offset = 0) = 0;
};

template<class CONFIG>
struct Kernel : CUfunc_st, CUDAGrid<CONFIG>, Loadable<CONFIG>
{
	Kernel(Module<CONFIG> * module, std::istream & is);
	Kernel();	// Default constructor needed for std containers
	virtual uint32_t CodeSize() const;

	void Load(Service<unisim::service::interfaces::Memory<typename CONFIG::address_t> > & mem,
		Allocator<CONFIG> & allocator, uint32_t offset = 0);
	std::string const & Name() const;
	void Dump(std::ostream & os) const;
	void SetBlockShape(int x, int y, int z);
	void SetGridShape(int x, int y);
	virtual int BlockX() const;
	virtual int BlockY() const;
	virtual int BlockZ() const;
	virtual int GridX() const;
	virtual int GridY() const;
	virtual int ThreadsPerBlock() const;
	virtual int WarpsPerBlock() const;
	virtual int GPRs() const;
	virtual Stats<CONFIG> & GetStats();
	
	void ParamSeti(int offset, uint32_t value);
	void ParamSetf(int offest, float value);
	void ParamSetv(int offset, void * data, int size);
	void ParamSetSize(int size);
	void SetSharedSize(int size);
	void SetTexRef(::Sampler<CONFIG> * sampler);
	
	virtual uint32_t SharedTotal() const;
	uint32_t LocalTotal() const;
	virtual uint8_t const * GetParameters() const;
	virtual uint32_t ParametersSize() const;

	virtual unsigned int SamplersSize() const;
	virtual SamplerBase<typename CONFIG::address_t> const * GetSampler(unsigned int i) const;

	// TODO: To be removed someday, duplicated with CUDAScheduler
	int BlocksPerCore() const;	// Max blocks that can run on a SM of target architecture
	
	
	static bool trace_loading;
	static bool trace_parsing;
private:
	Stats<CONFIG> stats;
	void SetAttribute(std::string const & name, std::string const & value);

	Module<CONFIG> * module;
	std::string name;
	int lmem;
	int smem;
	int reg;
	int bar;
	int ctaidZUsed;
	std::vector<uint32_t> bincode;
	//std::vector<uint32_t> const_mem;
	typedef std::list<MemSegment<CONFIG> > ConstList;
	ConstList const_segs;
	std::vector<uint8_t> parameters;
	int param_size;
	int dyn_smem;
	
	int blockx, blocky, blockz;
	int gridx, gridy;
	::Sampler<CONFIG> * samplers[CONFIG::MAX_SAMPLERS];
};



#endif
