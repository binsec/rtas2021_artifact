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
 * 		Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_SERVICE_TEE_MEMORY_ACCESS_REPORTING_TEE_TCC__
#define __UNISIM_SERVICE_TEE_MEMORY_ACCESS_REPORTING_TEE_TCC__

#include <string>
#include <sstream>

namespace unisim {
namespace service {
namespace tee {
namespace memory_access_reporting {

using std::stringstream;
using std::string;

template <class ADDRESS, unsigned int MAX_IMPORTS>
Tee<ADDRESS, MAX_IMPORTS>::Tee(const char *name, Object *parent) :
	Object(name, parent),
	Client<MemoryAccessReporting<ADDRESS> >(name, parent),
	Service<MemoryAccessReporting<ADDRESS> >(name, parent),
	in("in", this),
	out_control("out_control", this)
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		stringstream sstr;
		sstr << "out[" << i << "]";
		string import_name = sstr.str();
		out[i] = new ServiceImport<MemoryAccessReporting<ADDRESS> >(import_name.c_str(), this);
		in.SetupDependsOn(*out[i]);
		stringstream sstr_control;
		sstr_control << "in_control[" << i << "]";
		string export_name = sstr_control.str();
		in_control[i] = new ServiceExport<MemoryAccessReportingControl>(export_name.c_str(), this);
		in_control[i]->SetupDependsOn(out_control);
		requires_memory_access_reporting[i] = true;
		requires_fetch_instruction_reporting[i] = true;
		requires_commit_instruction_reporting[i] = true;
		stringstream sstr_module;
		sstr_module << name << ".control_selector[" << i << "]";
		string module_name = sstr_module.str();
		control_selector[i] = 
			new ControlSelector<MAX_IMPORTS>(i,
					requires_memory_access_reporting,
					requires_fetch_instruction_reporting,
					requires_commit_instruction_reporting,
					module_name.c_str(), this);
		*in_control[i] >> control_selector[i]->in;
		control_selector[i]->out >> out_control;
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
Tee<ADDRESS, MAX_IMPORTS>::~Tee()
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		delete control_selector[i];
		delete out[i];
		delete in_control[i];
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
bool Tee<ADDRESS, MAX_IMPORTS>::ReportMemoryAccess(typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat, 
		typename MemoryAccessReporting<ADDRESS>::MemoryType mt, 
		ADDRESS addr, uint32_t size)
{
	bool overlook = true;
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(out[i])
			if(*out[i])
			{
				if(!(*out[i])->ReportMemoryAccess(mat, mt, addr, size))
				{
					overlook = false;
				}
			}
	}
	
	return overlook;
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
void Tee<ADDRESS, MAX_IMPORTS>::ReportCommitInstruction(ADDRESS addr, unsigned int size)
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(out[i])
                  if(*out[i]) (*out[i])->ReportCommitInstruction(addr, size);
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
void Tee<ADDRESS, MAX_IMPORTS>::ReportFetchInstruction(ADDRESS next_addr)
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(out[i])
			if(*out[i]) (*out[i])->ReportFetchInstruction(next_addr);
	}
}

template <unsigned int MAX_IMPORTS>
ControlSelector<MAX_IMPORTS>::ControlSelector(
		unsigned int index,
		bool *requires_memory_access_reporting,
		bool *requires_fetch_instruction_reporting,
		bool *requires_commit_instruction_reporting,
		const char *name, Object *parent) :
	Object(name, parent),
	Client<MemoryAccessReportingControl>(name, parent),
	Service<MemoryAccessReportingControl>(name, parent),
	in("in", this),
	out("out_control", this)
{
	this->index = index;
	this->requires_memory_access_reporting = requires_memory_access_reporting;
	this->requires_fetch_instruction_reporting = requires_fetch_instruction_reporting;
	this->requires_commit_instruction_reporting = requires_commit_instruction_reporting;
}

template <unsigned int MAX_IMPORTS>
ControlSelector<MAX_IMPORTS>::~ControlSelector() 
{
}

template <unsigned int MAX_IMPORTS>
void
ControlSelector<MAX_IMPORTS>::RequiresMemoryAccessReporting(unisim::service::interfaces::MemoryAccessReportingType type, bool report) 
{
	bool *requires_reporting = 0;
	
	switch(type)
	{
		case unisim::service::interfaces::REPORT_MEM_ACCESS:
			requires_memory_access_reporting[index] = report;
			requires_reporting = requires_memory_access_reporting;
			break;
		case unisim::service::interfaces::REPORT_FETCH_INSN:
			requires_fetch_instruction_reporting[index] = report;
			requires_reporting = requires_fetch_instruction_reporting;
			break;
		case unisim::service::interfaces::REPORT_COMMIT_INSN:
			requires_commit_instruction_reporting[index] = report;
			requires_reporting = requires_commit_instruction_reporting;
			break;
	}
	
	bool needs_report = false;
	
	for(unsigned int i = 0; !needs_report && i < MAX_IMPORTS; i++) {
		needs_report = requires_reporting[i];
	}
	if(out) {
		out->RequiresMemoryAccessReporting(type, needs_report);
	}
}

} // end of namespace memory_access_reporting
} // end of namespace tee
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_TEE_MEMORY_ACCESS_REPORTING_TEE_TCC__
