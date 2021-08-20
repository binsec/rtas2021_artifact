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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E600_L1I_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E600_L1I_HH__

#include <inttypes.h>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e600 {

// Level 1 Instruction Cache
template <typename TYPES, typename CONFIG>
struct L1I
	: unisim::kernel::Object
	, unisim::util::cache::Cache<TYPES, CONFIG, L1I<TYPES, CONFIG> >
{
	typedef unisim::util::cache::Cache<TYPES, CONFIG, L1I<TYPES, CONFIG> > SuperCache;
	typedef typename CONFIG::CPU CPU;
	
	static const char *GetCacheName() { return "L1I"; }

	L1I(CPU *_cpu)
		: unisim::kernel::Object("L1I", _cpu, "L1 Instruction Cache")
		, SuperCache()
		, cpu(_cpu)
		, enable(true)
		, verbose(false)
		, param_enable("enable", this, enable, "enable/disable cache")
		, param_verbose("verbose", this, verbose, "enable/disable verbosity")
		, stat_num_accesses("num-accesses", this, this->num_accesses, "number of cache accesses")
		, stat_num_misses("num-misses", this, this->num_misses, "number of cache misses")
		, stat_miss_rate("miss-rate", this, "/", &stat_num_misses, &stat_num_accesses, "cache miss rate")
	{
	}

	inline bool IsVerbose() const ALWAYS_INLINE { return verbose; }
	inline bool IsEnabled() const ALWAYS_INLINE { return enable && (!cpu->GetMSR().template Get<typename CPU::MSR::IR>() || cpu->GetHID0().template Get<typename CPU::HID0::ICE>()); }
	inline bool IsWriteAllocate(typename TYPES::STORAGE_ATTR storage_attr) const ALWAYS_INLINE { return false; }
	inline bool ChooseLineToEvict(unisim::util::cache::CacheAccess<TYPES, L1I>& access) ALWAYS_INLINE
	{
		access.way = access.set->Status().plru.Select();
		return true;
	}
	inline void UpdateReplacementPolicyOnAccess(unisim::util::cache::CacheAccess<TYPES, L1I>& access) ALWAYS_INLINE
	{
		access.set->Status().plru.UpdateOnAccess(access.way);
	}
	inline void UpdateReplacementPolicyOnFill(unisim::util::cache::CacheAccess<TYPES, L1I>& access) ALWAYS_INLINE {}

private:
	CPU *cpu;
	bool enable;
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_enable;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_accesses;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_misses;
	unisim::kernel::variable::StatisticFormula<double> stat_miss_rate;
};

} // end of namespace e600
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E600_L1I_HH__
