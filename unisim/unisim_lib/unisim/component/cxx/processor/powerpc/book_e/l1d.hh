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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_L1D_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_L1D_HH__

#include <inttypes.h>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace book_e {

// Level 1 Data Cache
template <typename TYPES, typename CONFIG>
struct L1D
	: unisim::kernel::Object
	, unisim::util::cache::Cache<TYPES, CONFIG, L1D<TYPES, CONFIG> >
{
	typedef typename unisim::util::cache::Cache<TYPES, CONFIG, L1D<TYPES, CONFIG> > SuperCache;
	typedef typename CONFIG::CPU CPU;
	
	typedef typename CPU::CacheVictimRegister CacheVictimRegister;
	typedef typename CPU::DNV0 DNV0;
	typedef typename CPU::DNV1 DNV1;
	typedef typename CPU::DNV2 DNV2;
	typedef typename CPU::DNV3 DNV3;
	typedef typename CPU::DTV0 DTV0;
	typedef typename CPU::DTV1 DTV1;
	typedef typename CPU::DTV2 DTV2;
	typedef typename CPU::DTV3 DTV3;
	typedef typename CPU::DVLIM DVLIM;
	
	static const char *GetCacheName() { return "L1D"; }
	
	L1D(CPU *_cpu)
		: unisim::kernel::Object("L1D", _cpu, "L1 Data Cache")
		, SuperCache()
		, cpu(_cpu)
		, dnv0(_cpu)
		, dnv1(_cpu)
		, dnv2(_cpu)
		, dnv3(_cpu)
		, dtv0(_cpu)
		, dtv1(_cpu)
		, dtv2(_cpu)
		, dtv3(_cpu)
		, dvlim(_cpu)
		, dnv()
		, dtv()
		, verbose(false)
		, param_verbose("verbose", this, verbose, "enable/disable verbosity")
		, stat_num_accesses("num-accesses", this, this->num_accesses, "number of cache accesses")
		, stat_num_misses("num-misses", this, this->num_misses, "number of cache misses")
		, stat_miss_rate("miss-rate", this, "/", &stat_num_misses, &stat_num_accesses, "cache miss rate")
	{
		dnv[0] = &dnv0;
		dnv[1] = &dnv1;
		dnv[2] = &dnv2;
		dnv[3] = &dnv3;
		dtv[0] = &dtv0;
		dtv[1] = &dtv1;
		dtv[2] = &dtv2;
		dtv[3] = &dtv3;
	}

	inline bool IsVerbose() const ALWAYS_INLINE { return verbose; }
	inline bool IsEnabled() const ALWAYS_INLINE { return true; }
	inline bool IsWriteAllocate(typename TYPES::STORAGE_ATTR storage_attr) const ALWAYS_INLINE { return true; }
	inline bool ChooseLineToEvict(unisim::util::cache::CacheAccess<TYPES, L1D>& access) ALWAYS_INLINE
	{
		unsigned ctrl_reg_num = access.index / 4;
		unsigned ctrl_reg_field_num = access.index & 3;

		unsigned int victim_way = (access.storage_attr & TYPES::SA_U1) ? dtv[ctrl_reg_num]->GetVictimIndex(ctrl_reg_field_num) : dnv[ctrl_reg_num]->GetVictimIndex(ctrl_reg_field_num);
		
		access.way = victim_way % SuperCache::ASSOCIATIVITY;
		return true;
	}
	
	inline void UpdateReplacementPolicyOnAccess(unisim::util::cache::CacheAccess<TYPES, L1D>& access) ALWAYS_INLINE {}
	inline void UpdateReplacementPolicyOnFill(unisim::util::cache::CacheAccess<TYPES, L1D>& access) ALWAYS_INLINE
	{
		access.line->Status().storage_attr = access.storage_attr;
		
		unsigned ctrl_reg_num = access.index / 4;
		unsigned ctrl_reg_field_num = access.index & 3;
		
		// extract way from the control register value
		unsigned int victim_way = (access.storage_attr & TYPES::SA_U1) ? dtv[ctrl_reg_num]->GetVictimIndex(ctrl_reg_field_num) : dnv[ctrl_reg_num]->GetVictimIndex(ctrl_reg_field_num);
		
		// increment way of victim line
		victim_way = (victim_way + 1) % SuperCache::ASSOCIATIVITY;
		
		// constraint way
		if(access.storage_attr & TYPES::SA_U1) // transient
		{
			unsigned int tfloor = dvlim.template Get<typename DVLIM::TFLOOR>();
			unsigned int tceiling = dvlim.template Get<typename DVLIM::TCEILING>();
			if(victim_way < tfloor) victim_way = tfloor;
			if(victim_way > tceiling) victim_way = tceiling;
		}
		else // normal
		{
			unsigned int nfloor = dvlim.template Get<typename DVLIM::NFLOOR>();
			if(victim_way < nfloor) victim_way = nfloor;
		}
		
		// write back cache control register
		if(access.storage_attr & TYPES::SA_U1)
			dtv[ctrl_reg_num]->SetVictimIndex(ctrl_reg_field_num, victim_way);
		else
			dnv[ctrl_reg_num]->SetVictimIndex(ctrl_reg_field_num, victim_way);
	}
private:
	CPU *cpu;
	DNV0 dnv0;
	DNV1 dnv1;
	DNV2 dnv2;
	DNV3 dnv3;
	DTV0 dtv0;
	DTV1 dtv1;
	DTV2 dtv2;
	DTV3 dtv3;
	DVLIM dvlim;
	CacheVictimRegister *dnv[4];
	CacheVictimRegister *dtv[4];
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

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_L1D_HH__
