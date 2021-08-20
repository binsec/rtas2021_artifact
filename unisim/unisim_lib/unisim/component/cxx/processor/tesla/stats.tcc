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

#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_STATS_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_STATS_TCC

#include <unisim/component/cxx/processor/tesla/stats.hh>
#include <unisim/component/cxx/processor/tesla/register.hh>
#include <iomanip>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {


using namespace std;


template<class T, class ACCUM>
StatCounter<T, ACCUM>::StatCounter() :
	n(0), mean(0), m2(0),
	min(numeric_limits<T>::has_infinity ? +numeric_limits<T>::infinity() : numeric_limits<T>::max()),
	max(numeric_limits<T>::has_infinity ? -numeric_limits<T>::infinity() : numeric_limits<T>::min())
{
}

template<class T, class ACCUM>
void StatCounter<T, ACCUM>::Append(T v)
{
	// Don't consider NaNs when computing min and max
	if(v > max)
		max = v;
	if(v < min)
		min = v;
	
	// Knuth inline variance calculation
	++n;
	ACCUM delta = v - mean;
	mean += delta / n;
	m2 += delta * (v - mean);
}

template<class T, class ACCUM>
ACCUM StatCounter<T, ACCUM>::Avg() const
{
	return mean;
}

template<class T, class ACCUM>
ACCUM StatCounter<T, ACCUM>::Variance() const
{
	if(n < 2) {
		return 0;
	}
	return m2 / (n - 1);
}

template<class T, class ACCUM>
T StatCounter<T, ACCUM>::Min() const
{
	return min;
}

template<class T, class ACCUM>
T StatCounter<T, ACCUM>::Max() const
{
	return max;
}

template<class T, class ACCUM>
T StatCounter<T, ACCUM>::N() const
{
	return n;
}

template<class T, class ACCUM>
void StatCounter<T, ACCUM>::Merge(StatCounter<T, ACCUM> const & other)
{
	if(other.n > 0) {
		ACCUM delta = other.mean - mean;
		unsigned int nsum = n + other.n;
		mean += delta * other.n / nsum;
		m2 += other.m2 + delta * delta * (n * other.n) / nsum;
		n = nsum;
		min = std::min(min, other.min);
		max = std::max(max, other.max);
	}
}


template<class CONFIG>
void OperationStats<CONFIG>::Merge(OperationStats<CONFIG> const & other)
{
	name = other.name;
	integer = other.integer;
	mov = other.mov;
	fp = other.fp;
	flow = other.flow;
	memory = other.memory;
	shared = other.shared;
	constant = other.constant;
	for(int i = 0; i != 4; ++i)
		regnum[i] = other.regnum[i];
	
	
	count += other.count;
	scalarcount += other.scalarcount;
	scalarRegInputs += other.scalarRegInputs;
	stridedRegInputs += other.stridedRegInputs;
	scalarRegOutputs += other.scalarRegOutputs;
	stridedRegOutputs += other.stridedRegOutputs;

	scalarRegInputsCaught += other.scalarRegInputsCaught;
	stridedRegInputsCaught += other.stridedRegInputsCaught;
	scalarRegOutputsCaught += other.scalarRegOutputsCaught;
	stridedRegOutputsCaught += other.stridedRegOutputsCaught;
	
	branchTaken += other.branchTaken;
	branchNotTaken += other.branchNotTaken;
	branchDivergent += other.branchDivergent;
	jump += other.jump;
	
	gatherGlobal += other.gatherGlobal;
	loadGlobal += other.loadGlobal;
	scatterGlobal += other.scatterGlobal;
	storeGlobal += other.storeGlobal;

	gatherLocal += other.gatherLocal;
	loadLocal += other.loadLocal;
	scatterLocal += other.scatterLocal;
	storeLocal += other.storeLocal;
	
	time_spent += other.time_spent;
	fpValue.Merge(other.fpValue);
}

template<class CONFIG>
void OperationStats<CONFIG>::SetRegNum(int16_t reg, DataType dt, Operand n)
{
	switch(dt) {
	case DT_U32:
	case DT_S32:
	case DT_F32:
		regnum[n] = reg;
		break;
	case DT_U16:
	case DT_S16:
		regnum[n] = reg / 2;
		break;
	default:
		regnum[n] = reg;	// TODO: count multiple registers for DT_U64/DT_U128
		break;
	}
}

template<class CONFIG>
void OperationStats<CONFIG>::RegRead(VectorRegister<CONFIG> const * regs, DataType dt)
{
	if(CONFIG::STAT_SCALAR_REG) {
		if(dt == DT_U16 || dt == DT_S16) {
			scalarRegInputs += regs[0].CheckScalar16(false);
			scalarRegInputsCaught += regs[0].IsScalar16(false);
		}
		else if(dt == DT_U32 || dt == DT_S32 || dt == DT_F32) {
			scalarRegInputs += regs[0].CheckScalar();
			scalarRegInputsCaught += regs[0].IsScalar();
		}
	}
	if(CONFIG::STAT_STRIDED_REG) {
		if(dt == DT_U16 || dt == DT_S16) {
			stridedRegInputs += regs[0].CheckStrided16(false);
			stridedRegInputsCaught += regs[0].IsStrided16(false);
		}
		else if(dt == DT_U32 || dt == DT_S32 || dt == DT_F32) {
			stridedRegInputs += regs[0].CheckStrided();
			stridedRegInputsCaught += regs[0].IsStrided();
		}
	}
}

template<class CONFIG>
void OperationStats<CONFIG>::RegWrite(VectorRegister<CONFIG> const * regs, DataType dt,
		std::bitset<CONFIG::WARP_SIZE> mask)
{
	if(CONFIG::STAT_SCALAR_REG) {
		if(dt == DT_U16 || dt == DT_S16) {
			if(mask == ~std::bitset<CONFIG::WARP_SIZE>(0)) {
				scalarRegOutputs += regs[0].CheckScalar16(false);
				scalarRegOutputsCaught += regs[0].IsScalar16(false);
			}
		}
		else if(dt == DT_U32 || dt == DT_S32 || dt == DT_F32) {
			if(mask == ~std::bitset<CONFIG::WARP_SIZE>(0)) {
				scalarRegOutputs += regs[0].CheckScalar();
				scalarRegOutputsCaught += regs[0].IsScalar();
			}
		}
	}
	if(CONFIG::STAT_STRIDED_REG) {
		if(dt == DT_U16 || dt == DT_S16) {
			if(mask == ~std::bitset<CONFIG::WARP_SIZE>(0)) {
				stridedRegOutputs += regs[0].CheckStrided16(false);
				stridedRegOutputsCaught += regs[0].IsStrided16(false);
			}
		}
		else if(dt == DT_U32 || dt == DT_S32 || dt == DT_F32) {
			if(mask == ~std::bitset<CONFIG::WARP_SIZE>(0)) {
				stridedRegOutputs += regs[0].CheckStrided();
				stridedRegOutputsCaught += regs[0].IsStrided();
			}
		}
	}
	
	// TODO: should not be here
	if(CONFIG::DEBUG_NONAN && (dt == DT_F32)) {
		for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
		{
			if(mask[i])
				assert(!isnan(regs[0].ReadFloat(i)));
		}
	}
	
	if(CONFIG::STAT_FPDOMAIN && (dt == DT_F32)) {
		for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
		{
			if(mask[i])
				fpValue.Append(regs[0].ReadFloat(i));
		}
		
	}
}

#if defined(__GNUC__) && defined(__i386__)
inline uint64_t rdtsc()
{
	uint64_t clk;
	__asm__ volatile ("rdtsc" : "=A" (clk));
	return clk;
}
#elif defined(__GNUC__) && defined(__x86_64__)

inline uint64_t rdtsc()
{
	uint32_t hi, lo;
	__asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
	return lo | ((uint64_t)hi << 32);
}

#else
#warning "No TSC on this platform"
inline uint64_t rdtsc()
{
	return 0ULL;
}
#endif


template<class CONFIG>
void OperationStats<CONFIG>::Begin()
{
	if(CONFIG::STAT_SIMTIME) {
		timestamp = rdtsc();
	}
}

template<class CONFIG>
void OperationStats<CONFIG>::End()
{
	if(CONFIG::STAT_SIMTIME) {
		time_spent += rdtsc() - timestamp;
	}
}

template<class CONFIG>
bool Coalesceable(VectorAddress<CONFIG> const & addr, unsigned int logsize, std::bitset<CONFIG::WARP_SIZE> mask)
{
	for(unsigned int j = 0; j != CONFIG::WARP_SIZE; j += CONFIG::TRANSACTION_SIZE)
	{
		unsigned int i = 0;
		for(;i != CONFIG::TRANSACTION_SIZE && !mask[j + i];++i) {}
		if(i != CONFIG::TRANSACTION_SIZE) {
			typename CONFIG::address_t base = addr[j + i];
			if((base & ((1 << logsize) * CONFIG::TRANSACTION_SIZE - 1)) != (i << logsize)) {
				return false;	// Unaligned
			}
			for(; i != CONFIG::TRANSACTION_SIZE; ++i)
			{
				if(mask[j + i]) {
					if(addr[j + i] != base + i * (1 << logsize)) {
						return false;
					}
				}
			}
		}
	}
	return true;
}

template<class CONFIG>
void OperationStats<CONFIG>::GatherGlobal(VectorAddress<CONFIG> const & addr, unsigned int logsize,
	std::bitset<CONFIG::WARP_SIZE> mask)
{
	if(Coalesceable(addr, logsize, mask)) {
		LoadGlobal(logsize);
	}
	else {
		gatherGlobal += (1 << logsize);
	}
}

template<class CONFIG>
void OperationStats<CONFIG>::ScatterGlobal(VectorAddress<CONFIG> const & addr, unsigned int logsize,
	std::bitset<CONFIG::WARP_SIZE> mask)
{
	if(Coalesceable(addr, logsize, mask)) {
		StoreGlobal(logsize);
	}
	else {
		scatterGlobal += (1 << logsize);
	}
}

template<class CONFIG>
void OperationStats<CONFIG>::GatherLocal(VectorAddress<CONFIG> const & addr, unsigned int logsize,
	std::bitset<CONFIG::WARP_SIZE> mask)
{
	if(Coalesceable(addr, logsize, mask)) {
		LoadLocal(logsize);
	}
	else {
		gatherLocal += (1 << logsize);
	}
}

template<class CONFIG>
void OperationStats<CONFIG>::ScatterLocal(VectorAddress<CONFIG> const & addr, unsigned int logsize,
	std::bitset<CONFIG::WARP_SIZE> mask)
{
	if(Coalesceable(addr, logsize, mask)) {
		StoreLocal(logsize);
	}
	else {
		scatterLocal += (1 << logsize);
	}
}


// Format:
// address,name,count,scalarcount,integer,fp,flow,memory
template<class CONFIG>
void OperationStats<CONFIG>::DumpCSV(std::ostream & os) const
{
	bool outputregs = (regnum[OpDest] != -1);
	int inputregs = 0;
	// Count unique registers only
	if(regnum[OpSrc1] != -1) { ++inputregs; }
	if(regnum[OpSrc2] != -1 && regnum[OpSrc2] != regnum[OpSrc1]) { ++inputregs; }
	if(regnum[OpSrc3] != -1 && regnum[OpSrc3] != regnum[OpSrc2]
		&& regnum[OpSrc3] != regnum[OpSrc2]) { ++inputregs; }
	
	os << "\"" << name << "\","
	   << count << ","
	   << scalarcount << ","
	   << integer << ","
	   << fp << ","
	   << flow << ","
	   << mov << ","
	   << memory << ","
	   << shared << ","
	   << constant << ","
	   << inputregs << ","
	   << outputregs;
	if(CONFIG::STAT_SCALAR_REG) {
		os << "," << scalarRegInputs
		   << "," << scalarRegOutputs;
		os << "," << scalarRegInputsCaught
		   << "," << scalarRegOutputsCaught;
	}
	if(CONFIG::STAT_STRIDED_REG) {
		os << "," << stridedRegInputs
		   << "," << stridedRegOutputs;
		os << "," << stridedRegInputsCaught
		   << "," << stridedRegOutputsCaught;
	}
	if(CONFIG::STAT_SIMTIME) {
		os << "," << time_spent;
	}
	os << "," << branchTaken
	   << "," << branchNotTaken
	   << "," << branchDivergent
	   << "," << jump;

	os << "," << gatherGlobal
	   << "," << loadGlobal
	   << "," << scatterGlobal
	   << "," << storeGlobal;

	os << "," << gatherLocal
	   << "," << loadLocal
	   << "," << scatterLocal
	   << "," << storeLocal;


	if(CONFIG::STAT_FPDOMAIN) {
		if(fpValue.N() > 0) {
			os << "," << fpValue.Avg()
			   << "," << fpValue.Variance()
			   << "," << fpValue.Min()
			   << "," << fpValue.Max();
		}
		else {
			os << ",,,,";
		}
	}

	os << endl;
}

template<class CONFIG>
void Stats<CONFIG>::DumpCSV(std::ostream & os) const
{
	os << "\"Address\","
	   << "\"Name\","
	   << "\"Executed\","
	   << "\"Exec. scalar\","
	   << "\"Integer\","
	   << "\"FP32\","
	   << "\"Flow\","
	   << "\"Mov\","
	   << "\"Memory\","
	   << "\"Shared\","
	   << "\"Constant\","
	   << "\"Input regs\","
	   << "\"Output regs\"";
	if(CONFIG::STAT_SCALAR_REG) {
		os << ",\"Scalar inputs\""
		   << ",\"Scalar outputs\"";
		os << ",\"Scalar inputs caught\""
		   << ",\"Scalar outputs caught\"";
	}
	if(CONFIG::STAT_STRIDED_REG) {
		os << ",\"Strided inputs\""
		   << ",\"Strided outputs\"";
		os << ",\"Strided inputs caught\""
		   << ",\"Strided outputs caught\"";
	}
	if(CONFIG::STAT_SIMTIME) {
		os << ",\"Simulation time\"";
	}
	os << ",\"Branch taken\""
	   << ",\"Branch not taken\""
	   << ",\"Branch mixed\""
	   << ",\"Jump\"";

	os << ",\"Gather global\""
	   << ",\"Load global\""
	   << ",\"Scatter global\""
	   << ",\"Store global\"";

	os << ",\"Gather local\""
	   << ",\"Load local\""
	   << ",\"Scatter local\""
	   << ",\"Store local\"";

	if(CONFIG::STAT_FPDOMAIN) {
		os << ",\"fp-avg\""
		   << ",\"fp-var\""
		   << ",\"fp-min\""
		   << ",\"fp-max\"";
	}

	os << endl;

	typedef typename stats_map::const_iterator it_t;
	for(it_t it = stats.begin(); it != stats.end(); ++it)
	{
		os << it->first << ",";
		it->second.DumpCSV(os);
	}
}

template<class CONFIG>
void Stats<CONFIG>::Merge(Stats<CONFIG> const & other)
{
	typedef typename stats_map::const_iterator it_t;
	for(it_t it = other.stats.begin();
		it != other.stats.end();
		++it)
	{
		stats[it->first].Merge(it->second);
	}
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
