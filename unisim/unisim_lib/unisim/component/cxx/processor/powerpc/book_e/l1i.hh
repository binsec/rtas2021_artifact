/*
 *  Copyright (c) 2007-2019,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_L1I_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_L1I_HH__

#include <inttypes.h>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace book_e {

// Level 1 Instruction Cache
template <typename TYPES, typename CONFIG>
struct L1I
	: unisim::kernel::Object
	, unisim::util::cache::Cache<TYPES, CONFIG, L1I<TYPES, CONFIG> >
{
	typedef unisim::util::cache::Cache<TYPES, CONFIG, L1I<TYPES, CONFIG> > SuperCache;
	typedef typename CONFIG::CPU CPU;
	
	typedef typename CPU::CacheVictimRegister CacheVictimRegister;
	typedef typename CPU::INV0 INV0;
	typedef typename CPU::INV1 INV1;
	typedef typename CPU::INV2 INV2;
	typedef typename CPU::INV3 INV3;
	typedef typename CPU::ITV0 ITV0;
	typedef typename CPU::ITV1 ITV1;
	typedef typename CPU::ITV2 ITV2;
	typedef typename CPU::ITV3 ITV3;
	typedef typename CPU::IVLIM IVLIM;
	
	static const char *GetCacheName() { return "L1I"; }

	L1I(CPU *_cpu)
		: unisim::kernel::Object("L1I", _cpu, "L1 Instruction Cache")
		, SuperCache()
		, cpu(_cpu)
		, inv0(_cpu)
		, inv1(_cpu)
		, inv2(_cpu)
		, inv3(_cpu)
		, itv0(_cpu)
		, itv1(_cpu)
		, itv2(_cpu)
		, itv3(_cpu)
		, ivlim(_cpu)
		, inv()
		, itv()
		, verbose(false)
		, param_verbose("verbose", this, verbose, "enable/disable verbosity")
		, stat_num_accesses("num-accesses", this, this->num_accesses, "number of cache accesses")
		, stat_num_misses("num-misses", this, this->num_misses, "number of cache misses")
		, stat_miss_rate("miss-rate", this, "/", &stat_num_misses, &stat_num_accesses, "cache miss rate")
	{
		inv[0] = &inv0;
		inv[1] = &inv1;
		inv[2] = &inv2;
		inv[3] = &inv3;
		itv[0] = &itv0;
		itv[1] = &itv1;
		itv[2] = &itv2;
		itv[3] = &itv3;
	}

	inline bool IsVerbose() const ALWAYS_INLINE { return verbose; }
	inline bool IsEnabled() const ALWAYS_INLINE { return true; }
	inline bool IsWriteAllocate(typename TYPES::STORAGE_ATTR storage_attr) const ALWAYS_INLINE { return false; }
	inline bool ChooseLineToEvict(unisim::util::cache::CacheAccess<TYPES, L1I>& access) ALWAYS_INLINE
	{
		unsigned ctrl_reg_num = access.index / 4;
		unsigned ctrl_reg_field_num = access.index & 3;

		unsigned int victim_way = (access.storage_attr & TYPES::SA_U1) ? itv[ctrl_reg_num]->GetVictimIndex(ctrl_reg_field_num) : inv[ctrl_reg_num]->GetVictimIndex(ctrl_reg_field_num);
		
		access.way = victim_way % SuperCache::ASSOCIATIVITY;
		return true;
	}
	inline void UpdateReplacementPolicyOnAccess(unisim::util::cache::CacheAccess<TYPES, L1I>& access) ALWAYS_INLINE {}
	inline void UpdateReplacementPolicyOnFill(unisim::util::cache::CacheAccess<TYPES, L1I>& access) ALWAYS_INLINE
	{
		unsigned ctrl_reg_num = access.index / 4;
		unsigned ctrl_reg_field_num = access.index & 3;
		
		// extract way from the control register value
		unsigned int victim_way = (access.storage_attr & TYPES::SA_U1) ? itv[ctrl_reg_num]->GetVictimIndex(ctrl_reg_field_num) : inv[ctrl_reg_num]->GetVictimIndex(ctrl_reg_field_num);
		
		// increment way of victim line
		victim_way = (victim_way + 1) % SuperCache::ASSOCIATIVITY;
		
		// constraint way
		if(access.storage_attr & TYPES::SA_U1) // transient
		{
			unsigned int tfloor = ivlim.template Get<typename IVLIM::TFLOOR>();
			unsigned int tceiling = ivlim.template Get<typename IVLIM::TCEILING>();
			if(victim_way < tfloor) victim_way = tfloor;
			if(victim_way > tceiling) victim_way = tceiling;
		}
		else // normal
		{
			unsigned int nfloor = ivlim.template Get<typename IVLIM::NFLOOR>();
			if(victim_way < nfloor) victim_way = nfloor;
		}
		
		// write back cache control register
		if(access.storage_attr & TYPES::SA_U1)
			itv[ctrl_reg_num]->SetVictimIndex(ctrl_reg_field_num, victim_way);
		else
			inv[ctrl_reg_num]->SetVictimIndex(ctrl_reg_field_num, victim_way);
	}

private:
	CPU *cpu;
	INV0 inv0;
	INV1 inv1;
	INV2 inv2;
	INV3 inv3;
	ITV0 itv0;
	ITV1 itv1;
	ITV2 itv2;
	ITV3 itv3;
	IVLIM ivlim;
	CacheVictimRegister *inv[4];
	CacheVictimRegister *itv[4];
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_accesses;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_misses;
	unisim::kernel::variable::StatisticFormula<double> stat_miss_rate;
};

} // end of namespace book_e
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_L1I_HH__
