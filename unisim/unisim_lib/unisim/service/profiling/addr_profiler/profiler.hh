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
 
#ifndef __UNISIM_SERVICE_PROFILING_ADDR_PROFILER_PROFILER_HH__
#define __UNISIM_SERVICE_PROFILING_ADDR_PROFILER_PROFILER_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/service/interfaces/profiling.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/util/debug/profile.hh>


namespace unisim {
namespace service {
namespace profiling {
namespace addr_profiler {

using unisim::kernel::Object;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::variable::Parameter;
using unisim::service::interfaces::Profiling;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
	
template <class ADDRESS>
class Profiler
	: public Service<Profiling<ADDRESS> >
	, public Service<MemoryAccessReporting<ADDRESS> >
	, public Client<MemoryAccessReportingControl>
{
public:
	ServiceExport<Profiling<ADDRESS> > profiling_export;
	ServiceExport<MemoryAccessReporting<ADDRESS> > memory_access_reporting_export;
	ServiceImport<MemoryAccessReportingControl> memory_access_reporting_control_import;
	
	Profiler(const char *name, Object *parent = 0);
	virtual ~Profiler();
	
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();
	
	virtual bool ReportMemoryAccess(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size);
	virtual void ReportCommitInstruction(ADDRESS addr, unsigned size);
	virtual void ReportFetchInstruction(ADDRESS next_addr);

	virtual void SetProfileOption(typename unisim::service::interfaces::Profiling<ADDRESS>::ProfileType prof_type, typename unisim::service::interfaces::Profiling<ADDRESS>::Option opt, bool flag);
	virtual void SetProfileOption(typename unisim::service::interfaces::Profiling<ADDRESS>::ProfileType prof_type, typename unisim::service::interfaces::Profiling<ADDRESS>::Option opt, ADDRESS addr);
	virtual void GetProfileOption(typename unisim::service::interfaces::Profiling<ADDRESS>::ProfileType prof_type, typename unisim::service::interfaces::Profiling<ADDRESS>::Option opt, bool& flag) const;
	virtual void GetProfileOption(typename unisim::service::interfaces::Profiling<ADDRESS>::ProfileType prof_type, typename unisim::service::interfaces::Profiling<ADDRESS>::Option opt, ADDRESS& addr) const;
	virtual void ClearProfile(typename unisim::service::interfaces::Profiling<ADDRESS>::ProfileType prof_type);
	virtual const unisim::util::debug::Profile<ADDRESS>& GetProfile(typename unisim::service::interfaces::Profiling<ADDRESS>::ProfileType prof_type) const;
private:
	unisim::kernel::logger::Logger logger;
	ADDRESS min_prof_addr[unisim::service::interfaces::Profiling<ADDRESS>::NUM_PROFILE_TYPES];
	ADDRESS max_prof_addr[unisim::service::interfaces::Profiling<ADDRESS>::NUM_PROFILE_TYPES];
	bool enable_prof[unisim::service::interfaces::Profiling<ADDRESS>::NUM_PROFILE_TYPES];
	bool verbose;
	Parameter<ADDRESS> param_min_data_read_prof_addr;
	Parameter<ADDRESS> param_max_data_read_prof_addr;
	Parameter<ADDRESS> param_min_data_write_prof_addr;
	Parameter<ADDRESS> param_max_data_write_prof_addr;
	Parameter<ADDRESS> param_min_insn_fetch_prof_addr;
	Parameter<ADDRESS> param_max_insn_fetch_prof_addr;
	Parameter<ADDRESS> param_min_insn_exec_prof_addr;
	Parameter<ADDRESS> param_max_insn_exec_prof_addr;
	Parameter<bool> param_enable_data_read_prof;
	Parameter<bool> param_enable_data_write_prof;
	Parameter<bool> param_enable_insn_fetch_prof;
	Parameter<bool> param_enable_insn_exec_prof;
	Parameter<bool> param_verbose;
	
	unisim::util::debug::Profile<ADDRESS> profile[unisim::service::interfaces::Profiling<ADDRESS>::NUM_PROFILE_TYPES];
	
	void ProfileMemoryAccess(typename unisim::service::interfaces::Profiling<ADDRESS>::ProfileType prof_type, ADDRESS addr, uint32_t size);
};

} // end of namesapce addr_profiler
} // end of namespace profiling
} // end of namespace service
} // end of namespace unisim

#endif
