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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_STATS_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_STATS_HH

#include <string>
#include <algorithm>
#include <unisim/component/cxx/processor/tesla/operation.hh>
#include <unisim/component/cxx/processor/tesla/register.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template<class T, class ACCUM>
struct StatCounter
{
	StatCounter();
	void Append(T v);
	ACCUM Avg() const;
	ACCUM Variance() const;
	T Min() const;
	T Max() const;
	T N() const;
	void Merge(StatCounter<T, ACCUM> const & other);

private:
	unsigned int n;
	ACCUM mean;
	ACCUM m2;
	T min;
	T max;
};

template<class CONFIG>
struct OperationStats
{
private:
	std::string name;
	unsigned int count;
	unsigned int scalarcount;
	bool integer;
	bool mov;
	bool fp;
	bool flow;
	bool memory;
	bool shared;
	bool constant;
	int16_t regnum[4];
	unsigned int scalarRegInputs;
	unsigned int stridedRegInputs;
	unsigned int scalarRegOutputs;
	unsigned int stridedRegOutputs;

	unsigned int scalarRegInputsCaught;
	unsigned int stridedRegInputsCaught;
	unsigned int scalarRegOutputsCaught;
	unsigned int stridedRegOutputsCaught;
	
	unsigned int branchTaken, branchNotTaken, branchDivergent;
	unsigned int jump;
	
	unsigned int gatherGlobal, loadGlobal, scatterGlobal, storeGlobal;
	unsigned int gatherLocal, loadLocal, scatterLocal, storeLocal;
	
	uint64_t time_spent;
	uint64_t timestamp;
	
	StatCounter<float, double> fpValue;
	
public:
	OperationStats() :
		count(0), scalarcount(0), integer(false), mov(false),
		fp(false), flow(false), memory(false),
		shared(false), constant(false),
		scalarRegInputs(0), stridedRegInputs(0),
		scalarRegOutputs(0), stridedRegOutputs(0),
		scalarRegInputsCaught(0), stridedRegInputsCaught(0),
		scalarRegOutputsCaught(0), stridedRegOutputsCaught(0),
		branchTaken(0), branchNotTaken(0), branchDivergent(0),
		jump(0),
		gatherGlobal(0), loadGlobal(0), scatterGlobal(0), storeGlobal(0),
		gatherLocal(0), loadLocal(0), scatterLocal(0), storeLocal(0),
		time_spent(0)
	{
		std::fill(regnum, regnum + 4, -1);
	}
	
	void Merge(OperationStats<CONFIG> const & other);

	void SetName(char const * insnname) {
		name = std::string(insnname);
	}
	
	void SetInteger() { integer = true;	}
	void SetFP() { fp = true; }
	void SetFP64() { fp = true; }

	void SetTransc() { fp = true; }
	void SetIMul() { integer = true; }
	void SetFMul() { fp = true; }

	void SetFlow() { flow = true; }
	void SetMov() { mov = true;	}
	void SetGather() { memory = true;	}
	void SetScatter() { memory = true;	}
	void SetInputConst() { constant = true; }
	void SetInputShared() { shared = true; }
	void SetOutputShared() { shared = true; }

	void SetRegNum(int16_t reg, DataType dt, Operand n);
	
	void RegRead(VectorRegister<CONFIG> const * regs, DataType dt);
	void RegWrite(VectorRegister<CONFIG> const * regs, DataType dt,
		std::bitset<CONFIG::WARP_SIZE> mask);
	void Execute(std::bitset<CONFIG::WARP_SIZE> mask) {
		++count;
		scalarcount += mask.count();
	}
	void Unexecute() {
		--count;
	}
	
	void Begin();
	void End();
	
	// Branch instruction
	void BranchUniTaken() { ++branchTaken; }
	void BranchUniNotTaken() { ++branchNotTaken; }
	void BranchDivergent() { ++branchDivergent; }

	// Effective jump, by branch, call, return, join...
	void Jump() { ++jump; }
	//void Gather(VectorAddress<CONFIG> const & addr, std::bitset<CONFIG::WARP_SIZE> mask);

	void GatherGlobal(VectorAddress<CONFIG> const & addr, unsigned int logsize, std::bitset<CONFIG::WARP_SIZE> mask);
	void LoadGlobal(unsigned int logsize) { loadGlobal += (1 << logsize); }
	void ScatterGlobal(VectorAddress<CONFIG> const & addr, unsigned int logsize, std::bitset<CONFIG::WARP_SIZE> mask);
	void StoreGlobal(unsigned int logsize) { storeGlobal += (1 << logsize); }

	void GatherLocal(VectorAddress<CONFIG> const & addr, unsigned int logsize, std::bitset<CONFIG::WARP_SIZE> mask);
	void LoadLocal(unsigned int logsize) { loadLocal += (1 << logsize); }
	void ScatterLocal(VectorAddress<CONFIG> const & addr, unsigned int logsize, std::bitset<CONFIG::WARP_SIZE> mask);
	void StoreLocal(unsigned int logsize) { storeLocal += (1 << logsize); }


	void DumpCSV(std::ostream & os) const;
};

template<class CONFIG>
struct Stats
{
private:
	typedef std::map<typename CONFIG::address_t, OperationStats<CONFIG> > stats_map;	
	stats_map stats;
	
public:
	OperationStats<CONFIG> & operator[] (typename CONFIG::address_t addr) {
		return stats[addr];
	}
	
	void DumpCSV(std::ostream & os) const;
	void Reset() {
		stats.clear();
	}
	void Merge(Stats<CONFIG> const & other);
};

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif

