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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_L1D_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_L1D_HH__

#include <inttypes.h>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e200 {
namespace mpc57xx {

// Level 1 Data Cache
template <typename TYPES, typename CONFIG>
struct L1D
	: unisim::kernel::Object
	, unisim::util::cache::Cache<TYPES, CONFIG, L1D<TYPES, CONFIG> >
{
	typedef typename unisim::util::cache::Cache<TYPES, CONFIG, L1D<TYPES, CONFIG> > SuperCache;
	typedef typename CONFIG::CPU CPU;
	
	typedef typename CPU::L1CSR0 L1CSR0;
	
	static const char *GetCacheName() { return "L1D"; }
	
	L1D(CPU *_cpu)
		: unisim::kernel::Object("L1D", _cpu, "L1 Data Cache")
		, SuperCache()
		, cpu(_cpu)
		, l1finv0(_cpu, this)
		, verbose(false)
		, param_verbose("verbose", this, verbose, "enable/disable verbosity")
		, stat_num_accesses("num-accesses", this, this->num_accesses, "number of cache accesses")
		, stat_num_misses("num-misses", this, this->num_misses, "number of cache misses")
		, stat_miss_rate("miss-rate", this, "/", &stat_num_misses, &stat_num_accesses, "cache miss rate")
	{
	}
	
	// L1 Flush and Invalidate Control Register 0
	struct L1FINV0 : CPU::L1FINV0
	{
		typedef typename CPU::L1FINV0 Super;
		
		L1FINV0(CPU *_cpu, L1D *_l1d) : Super(_cpu), l1d(_l1d) {}
		L1FINV0(CPU *_cpu, L1D *_l1d, uint32_t _value) : Super(_cpu, _value), l1d(_l1d) {}
		
		void Effect()
		{
			switch(this->template Get<typename L1FINV0::CCMD>())
			{
				case 0x5: // Flush & invalidate & clear lockout
					l1d->ClearLockoutBySetAndWay(this->template Get<typename L1FINV0::CSET>(), this->template Get<typename L1FINV0::CWAY>());
				case 0x0: // Flush & invalidate
					l1d->InvalidateLineBySetAndWay(this->template Get<typename L1FINV0::CSET>(), this->template Get<typename L1FINV0::CWAY>());
					break;
				default:
					this->cpu->GetDebugWarningStream() << "Cache Command is invalid, see " << l1d->GetCacheName() << " content below for detail:" << std::endl;
					this->LongPrettyPrint(this->cpu->GetDebugWarningStream());
					this->cpu->GetDebugWarningStream() << std::endl;
					break;
			}
		}
		
		L1FINV0& operator = (const uint32_t& value) { this->Super::operator = (value); Effect(); return *this; }
		virtual bool MoveTo(uint32_t value) { if(!this->Super::MoveTo(value)) return false; Effect(); return true; }
	private:
		L1D *l1d;
	};

	inline bool IsVerbose() const ALWAYS_INLINE { return verbose; }
	inline bool IsEnabled() const ALWAYS_INLINE { return cpu->l1csr0.template Get<typename L1CSR0::DCE>(); }
	inline bool IsWriteAllocate(typename TYPES::STORAGE_ATTR storage_attr) const ALWAYS_INLINE { return cpu->l1csr0.template Get<typename L1CSR0::DCWA>(); }
	inline bool ChooseLineToEvict(unisim::util::cache::CacheAccess<TYPES, L1D>& access) ALWAYS_INLINE
	{
		if(cpu->l1csr0.template Get<typename L1CSR0::WDD>() == ((1 << L1D::ASSOCIATIVITY) - 1)) return false; // all data cache ways are locked

		unsigned int& lockout = access.set->Status().lockout;
		if(lockout == ((1 << L1D::ASSOCIATIVITY) - 1)) return false; // all data cache ways are locked-out
		
		unsigned int& victim_way = access.set->Status().victim_way;
		while((cpu->l1csr0.template Get<typename L1CSR0::WDD>() & ((1 << (L1D::ASSOCIATIVITY - 1)) >> victim_way)) || (lockout & ((1 << (L1D::ASSOCIATIVITY - 1)) >> victim_way))) victim_way++;
		
		access.way = victim_way;
		return true;
	}
	
	inline void UpdateReplacementPolicyOnAccess(unisim::util::cache::CacheAccess<TYPES, L1D>& access) ALWAYS_INLINE {}
	inline void UpdateReplacementPolicyOnFill(unisim::util::cache::CacheAccess<TYPES, L1D>& access) ALWAYS_INLINE
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
			cpu->GetDebugInfoStream() << this->GetCacheName() << ":Clearing Data Cache Lockout by Set #" << index << " and Way #" << way << std::endl;
		}

		assert(way < L1D::ASSOCIATIVITY);
		unisim::util::cache::CacheSet<TYPES, typename L1D::CACHE_CONFIG>& set = this->SuperCache::operator [] (index);
		
		unsigned int& lockout = set.Status().lockout;
		lockout = lockout & ~((1 << (L1D::ASSOCIATIVITY - 1)) >> way);
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
	L1FINV0 l1finv0;
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

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_L1D_HH__
