/*
 *  Copyright (c) 2007-2017,
 *  Commissariat a l'Energie Atomique (CEA)
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
 *   - Neither the name of CEA nor the names of its contributors may be used to
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_L1I_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_L1I_HH__

#include <inttypes.h>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e200 {
namespace mpc57xx {

// Level 1 Instruction Cache
template <typename TYPES, typename CONFIG>
struct L1I
	: unisim::kernel::Object
	, unisim::util::cache::Cache<TYPES, CONFIG, L1I<TYPES, CONFIG> >
{
	typedef unisim::util::cache::Cache<TYPES, CONFIG, L1I<TYPES, CONFIG> > SuperCache;
	typedef typename CONFIG::CPU CPU;
	
	typedef typename CPU::L1CSR0 L1CSR0;
	
	static const char *GetCacheName() { return "L1I"; }

	L1I(CPU *_cpu)
		: unisim::kernel::Object("L1I", _cpu, "L1 Instruction Cache")
		, SuperCache()
		, cpu(_cpu)
		, l1csr1(_cpu, this)
		, l1finv1(_cpu, this)
		, verbose(false)
		, param_verbose("verbose", this, verbose, "enable/disable verbosity")
		, stat_num_accesses("num-accesses", this, this->num_accesses, "number of cache accesses")
		, stat_num_misses("num-misses", this, this->num_misses, "number of cache misses")
		, stat_miss_rate("miss-rate", this, "/", &stat_num_misses, &stat_num_accesses, "cache miss rate")
	{
	}
	
	// L1 Cache Control and Status Register 1
	struct L1CSR1 : CPU::L1CSR1
	{
		typedef typename CPU::L1CSR1 Super;
		
		L1CSR1(CPU *_cpu, L1I *_l1i) : Super(_cpu), l1i(_l1i) {}
		L1CSR1(CPU *_cpu, L1I *_l1i, uint32_t _value) : Super(_cpu, _value), l1i(_l1i) {}
		
		void Effect()
		{
			if(unlikely(l1i->verbose))
			{
				this->cpu->GetDebugInfoStream() << "Instruction Cache is " << (this->template Get<typename L1CSR1::ICE>() ? "enabled" : "disabled") << std::endl;
			}
			if(this->template Get<typename L1CSR1::ICINV>())
			{
				l1i->Invalidate();
				this->template Set<typename L1CSR1::ICINV>(0);
			}
		}
		
		L1CSR1& operator = (const uint32_t& value) { this->Super::operator = (value); Effect(); return *this; }
		virtual bool MoveTo(uint32_t value) { if(!this->Super::MoveTo(value)) return false; Effect(); return true; }
	private:
		L1I *l1i;
	};

	// L1 Flush and Invalidate Control Register 1
	struct L1FINV1 : CPU::L1FINV1
	{
		typedef typename CPU::L1FINV1 Super;
		
		L1FINV1(CPU *_cpu, L1I *_l1i) : Super(_cpu), l1i(_l1i) {}
		L1FINV1(CPU *_cpu, L1I *_l1i, uint32_t _value) : Super(_cpu, _value), l1i(_l1i) {}
		
		void Effect()
		{
			switch(this->template Get<typename L1FINV1::CCMD>())
			{
				case 0x5: // Invalidate & clear lockout
					l1i->ClearLockoutBySetAndWay(this->template Get<typename L1FINV1::CSET>(), this->template Get<typename L1FINV1::CWAY>());
				case 0x0: // Invalidate
					l1i->InvalidateLineBySetAndWay(this->template Get<typename L1FINV1::CSET>(), this->template Get<typename L1FINV1::CWAY>());
					break;
				default:
					this->cpu->GetDebugWarningStream() << "Cache Command is invalid, see " << l1i->GetCacheName() << " content below for detail:" << std::endl;
					this->LongPrettyPrint(this->cpu->GetDebugWarningStream());
					this->cpu->GetDebugWarningStream() << std::endl;
					break;
			}
		}
		
		L1FINV1& operator = (const uint32_t& value) { this->Super::operator = (value); Effect(); return *this; }
		virtual bool MoveTo(uint32_t value) { if(!this->Super::MoveTo(value)) return false; Effect(); return true; }
	private:
		L1I *l1i;
	};

	inline bool IsVerbose() const ALWAYS_INLINE { return verbose; }
	inline bool IsEnabled() const ALWAYS_INLINE { return l1csr1.template Get<typename L1CSR1::ICE>(); }
	inline bool IsWriteAllocate(typename TYPES::STORAGE_ATTR storage_attr) const ALWAYS_INLINE { return false; }
	inline bool ChooseLineToEvict(unisim::util::cache::CacheAccess<TYPES, L1I>& access) ALWAYS_INLINE
	{
		if(cpu->l1csr0.template Get<typename L1CSR0::WID>() == ((1 << L1I::ASSOCIATIVITY) - 1)) return false; // all instruction cache ways are locked
		
		unsigned int& lockout = access.set->Status().lockout;
		if(lockout == ((1 << L1I::ASSOCIATIVITY) - 1)) return false; // all data cache ways are locked-out
		
		unsigned int& victim_way = access.set->Status().victim_way;
		while((cpu->l1csr0.template Get<typename L1CSR0::WID>() & ((1 << (L1I::ASSOCIATIVITY - 1)) >> victim_way)) || (lockout & ((1 << (L1I::ASSOCIATIVITY - 1)) >> victim_way))) victim_way++;
		access.way = victim_way;
		return true;
	}
	inline void UpdateReplacementPolicyOnAccess(unisim::util::cache::CacheAccess<TYPES, L1I>& access) ALWAYS_INLINE {}
	inline void UpdateReplacementPolicyOnFill(unisim::util::cache::CacheAccess<TYPES, L1I>& access) ALWAYS_INLINE
	{
		access.set->Status().victim_way++;
	}
	
	void Invalidate()
	{
		if(unlikely(verbose))
		{
			cpu->GetDebugInfoStream() << this->GetCacheName() << ":Invalidating all" << std::endl;
		}
		SuperCache::Invalidate();
	}

	void ClearLockoutBySetAndWay(unsigned int index, unsigned int way)
	{
		if(unlikely(verbose))
		{
			cpu->GetDebugInfoStream() << this->GetCacheName() <<  ":Clearing Data Cache Lockout by Set #" << index << " and Way #" << way << std::endl;
		}

		assert(way < L1I::ASSOCIATIVITY);
		unisim::util::cache::CacheSet<TYPES, typename L1I::CACHE_CONFIG>& set = this->SuperCache::operator [] (index);
		
		unsigned int& lockout = set.Status().lockout;
		lockout = lockout & ~((1 << (L1I::ASSOCIATIVITY - 1)) >> way);
	}
	
	void InvalidateLineBySetAndWay(unsigned int index, unsigned int way)
	{
		if(unlikely(verbose))
		{
			cpu->GetDebugInfoStream() << this->GetCacheName() << ":Invalidating Data Cache by Set #" << index << " and Way #" << way << std::endl;
		}
		SuperCache::InvalidateLineBySetAndWay(index, way);
	}

private:
	CPU *cpu;
	L1CSR1 l1csr1;
	L1FINV1 l1finv1;
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_accesses;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_misses;
	unisim::kernel::variable::StatisticFormula<double> stat_miss_rate;
};

} // end of namespace mpc57xx
} // end of namespace e200
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_L1I_HH__
