/*
 *  Copyright (c) 2007,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr),
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_SERVICE_TEE_MEMORY_ACCESS_REPORTING_TEE_HH__
#define __UNISIM_SERVICE_TEE_MEMORY_ACCESS_REPORTING_TEE_HH__

#include <inttypes.h>
#include "unisim/kernel/kernel.hh"
#include "unisim/service/interfaces/memory_access_reporting.hh"

namespace unisim {
namespace service {
namespace tee {
namespace memory_access_reporting {

using unisim::kernel::Object;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;

template <unsigned int MAX_IMPORTS>
class ControlSelector;

template <class ADDRESS, unsigned int MAX_IMPORTS = 16>
class Tee :
	public Client<MemoryAccessReporting<ADDRESS> >,
	public Service<MemoryAccessReporting<ADDRESS> >
{
public:
	ServiceExport<MemoryAccessReporting<ADDRESS> > in;
	ServiceImport<MemoryAccessReporting<ADDRESS> > *out[MAX_IMPORTS];
	ServiceImport<MemoryAccessReportingControl> out_control;
	ServiceExport<MemoryAccessReportingControl> *in_control[MAX_IMPORTS];
	
	Tee(const char *name, Object *parent = 0);
	virtual ~Tee();
	virtual bool ReportMemoryAccess(typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat, 
			typename MemoryAccessReporting<ADDRESS>::MemoryType mt, 
			ADDRESS addr, uint32_t size);
	virtual void ReportCommitInstruction(ADDRESS addr, unsigned int length /* in bytes */);
	virtual void ReportFetchInstruction(ADDRESS next_addr);

private:
	ControlSelector<MAX_IMPORTS> *control_selector[MAX_IMPORTS];
	bool requires_memory_access_reporting[MAX_IMPORTS];
	bool requires_fetch_instruction_reporting[MAX_IMPORTS];
	bool requires_commit_instruction_reporting[MAX_IMPORTS];
};

template <unsigned int MAX_IMPORTS>
class ControlSelector :
	public Client<MemoryAccessReportingControl>,
	public Service<MemoryAccessReportingControl> 
{
public:
	ServiceExport<MemoryAccessReportingControl> in;
	ServiceImport<MemoryAccessReportingControl> out;

	ControlSelector(unsigned int index,
			bool *requires_memory_access_reporting,
			bool *requires_fetch_instruction_reporting,
			bool *requires_commit_instruction_reporting,
			const char *name, Object *parent = 0);
	~ControlSelector();
	
	virtual void RequiresMemoryAccessReporting(unisim::service::interfaces::MemoryAccessReportingType, bool report);
private:
	unsigned int index;
	bool *requires_memory_access_reporting;
	bool *requires_fetch_instruction_reporting;
	bool *requires_commit_instruction_reporting;
};

} // end of namespace memory_access_reporting
} // end of namespace tee
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_TEE_MEMORY_ACCESS_REPORTING_TEE_HH__
