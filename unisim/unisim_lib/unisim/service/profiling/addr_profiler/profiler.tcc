/*
 *  Copyright (c) 2012,
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
 
#ifndef __UNISIM_SERVICE_PROFILING_ADDR_PROFILER_PROFILER_TCC__
#define __UNISIM_SERVICE_PROFILING_ADDR_PROFILER_PROFILER_TCC__

#include <limits>

namespace unisim {
namespace service {
namespace profiling {
namespace addr_profiler {

template <class ADDRESS>
Profiler<ADDRESS>::Profiler(const char *name, Object *parent)
	: Object(name, parent)
	, Service<Profiling<ADDRESS> >(name, parent)
	, Service<MemoryAccessReporting<ADDRESS> >(name, parent)
	, Client<MemoryAccessReportingControl>(name, parent)
	, profiling_export("profiling-export", this)
	, memory_access_reporting_export("memory-access-reporting-export", this)
	, memory_access_reporting_control_import("memory-access-reporting-control-import", this)
	, logger(*this)
	, verbose(false)
	, param_min_data_read_prof_addr("min-data-read-prof-addr", this, min_prof_addr[unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_READ], "Minimum address for data read profiling")
	, param_max_data_read_prof_addr("max-data-read-prof-addr", this, max_prof_addr[unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_READ], "Maximum address for data read profiling")
	, param_min_data_write_prof_addr("min-data-write-prof-addr", this, min_prof_addr[unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_WRITE], "Minimum address for data write profiling")
	, param_max_data_write_prof_addr("max-data-write-prof-addr", this, max_prof_addr[unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_WRITE], "Maximum address for data write profiling")
	, param_min_insn_fetch_prof_addr("min-insn-fetch-prof-addr", this, min_prof_addr[unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_FETCH], "Minimum address for instruction fetch profiling")
	, param_max_insn_fetch_prof_addr("max-insn-fetch-prof-addr", this, max_prof_addr[unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_FETCH], "Maximum address for instruction fetch profiling")
	, param_min_insn_exec_prof_addr("min-insn-exec-prof-addr", this, min_prof_addr[unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC], "Minimum address for instruction execution profiling")
	, param_max_insn_exec_prof_addr("max-insn-exec-prof-addr", this, max_prof_addr[unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC], "Maximum address for instruction execution profiling")
	, param_enable_data_read_prof("enable-data-read-prof", this, enable_prof[unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_READ], "Enable/Disable data read profiling")
	, param_enable_data_write_prof("enable-data-write-prof", this, enable_prof[unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_WRITE], "Enable/Disable data write profiling")
	, param_enable_insn_fetch_prof("enable-insn-fetch-prof", this, enable_prof[unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_FETCH], "Enable/Disable instruction fetch profiling")
	, param_enable_insn_exec_prof("enable-insn-exec-prof", this, enable_prof[unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC], "Enable/Disable instruction execution profiling")
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, profile()
{
	unsigned int i;
	for(i = 0; i < unisim::service::interfaces::Profiling<ADDRESS>::NUM_PROFILE_TYPES; i++)
	{
		min_prof_addr[i] = std::numeric_limits<ADDRESS>::min();
		max_prof_addr[i] = std::numeric_limits<ADDRESS>::max();
		enable_prof[i] = false;
	}
}

template <class ADDRESS>
Profiler<ADDRESS>::~Profiler()
{
}
	
template <class ADDRESS>
bool Profiler<ADDRESS>::BeginSetup()
{
	unsigned int i;
	for(i = 0; i < unisim::service::interfaces::Profiling<ADDRESS>::NUM_PROFILE_TYPES; i++)
	{
		profile[i].Clear();
	}
	return true;
}

template <class ADDRESS>
bool Profiler<ADDRESS>::Setup(ServiceExportBase *srv_export)
{
	return true;
}

template <class ADDRESS>
bool Profiler<ADDRESS>::EndSetup()
{
	return true;
}

template <class ADDRESS>
void Profiler<ADDRESS>::ProfileMemoryAccess(typename unisim::service::interfaces::Profiling<ADDRESS>::ProfileType prof_type, ADDRESS addr, uint32_t size)
{
	if((size > 0) && enable_prof[prof_type])
	{
		if(addr > max_prof_addr[prof_type]) return;
		ADDRESS max_addr = addr + size - 1;
		if(max_addr < addr) max_addr = std::numeric_limits<ADDRESS>::max(); // in case of overflow
		if(max_addr < min_prof_addr[prof_type]) return;
		if(addr < min_prof_addr[prof_type]) addr = min_prof_addr[prof_type];
		if(max_addr > max_prof_addr[prof_type]) max_addr = max_prof_addr[prof_type];
		
		uint32_t cnt = max_addr - addr + 1;
		do
		{
			profile[prof_type].Accumulate(addr, 1);
		}
		while(++addr,--cnt);
	}
}

template <class ADDRESS>
bool Profiler<ADDRESS>::ReportMemoryAccess(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size)
{
	if(size)
	{
		switch(mt)
		{
			case unisim::util::debug::MT_DATA:
				switch(mat)
				{
					case unisim::util::debug::MAT_READ:
						ProfileMemoryAccess(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_READ, addr, size);
						break;
					case unisim::util::debug::MAT_WRITE:
						ProfileMemoryAccess(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_WRITE, addr, size);
						break;
					default:
						// ignore
						break;
				}
				break;
			case unisim::util::debug::MT_INSN:
				ProfileMemoryAccess(unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_FETCH, addr, size);
				break;
		}
	}
	
	return true;
}

template <class ADDRESS>
void Profiler<ADDRESS>::ReportCommitInstruction(ADDRESS addr, unsigned size)
{
	if(enable_prof[unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC])
	{
		if((addr >= min_prof_addr[unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC]) &&
		   (addr <= max_prof_addr[unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC]))
		{
			profile[unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC].Accumulate(addr, 1);
		}
	}
}

template <class ADDRESS>
void Profiler<ADDRESS>::ReportFetchInstruction(ADDRESS next_addr)
{
}

template <class ADDRESS>
void Profiler<ADDRESS>::SetProfileOption(typename unisim::service::interfaces::Profiling<ADDRESS>::ProfileType prof_type, typename unisim::service::interfaces::Profiling<ADDRESS>::Option opt, bool flag)
{
	switch(opt)
	{
		case unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF:
			enable_prof[prof_type] = flag;
			break;
		default:
			break;
	}
}

template <class ADDRESS>
void Profiler<ADDRESS>::SetProfileOption(typename unisim::service::interfaces::Profiling<ADDRESS>::ProfileType prof_type, typename unisim::service::interfaces::Profiling<ADDRESS>::Option opt, ADDRESS addr)
{
	switch(opt)
	{
		case unisim::service::interfaces::Profiling<ADDRESS>::OPT_MIN_PROF_ADDR:
			min_prof_addr[prof_type] = addr;
			break;
		case unisim::service::interfaces::Profiling<ADDRESS>::OPT_MAX_PROF_ADDR:
			max_prof_addr[prof_type] = addr;
			break;
		default:
			break;
	}
}

template <class ADDRESS>
void Profiler<ADDRESS>::GetProfileOption(typename unisim::service::interfaces::Profiling<ADDRESS>::ProfileType prof_type, typename unisim::service::interfaces::Profiling<ADDRESS>::Option opt, bool& flag) const
{
	switch(opt)
	{
		case unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF:
			flag = enable_prof[prof_type];
			break;
		default:
			break;
	}
}

template <class ADDRESS>
void Profiler<ADDRESS>::GetProfileOption(typename unisim::service::interfaces::Profiling<ADDRESS>::ProfileType prof_type, typename unisim::service::interfaces::Profiling<ADDRESS>::Option opt, ADDRESS& addr) const
{
	switch(opt)
	{
		case unisim::service::interfaces::Profiling<ADDRESS>::OPT_MIN_PROF_ADDR:
			addr = min_prof_addr[prof_type];
			break;
		case unisim::service::interfaces::Profiling<ADDRESS>::OPT_MAX_PROF_ADDR:
			addr = max_prof_addr[prof_type];
			break;
		default:
			break;
	}
}

template <class ADDRESS>
void Profiler<ADDRESS>::ClearProfile(typename unisim::service::interfaces::Profiling<ADDRESS>::ProfileType prof_type)
{
	profile[prof_type].Clear();
}

template <class ADDRESS>
const unisim::util::debug::Profile<ADDRESS>& Profiler<ADDRESS>::GetProfile(typename unisim::service::interfaces::Profiling<ADDRESS>::ProfileType prof_type) const
{
	return profile[prof_type];
}

} // end of namesapce addr_profiler
} // end of namespace profiling
} // end of namespace service
} // end of namespace unisim

#endif
