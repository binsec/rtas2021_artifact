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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 *          Yves Lhuillier (yves.lhuillier@cea.fr)
 */
 
#ifndef __UNISIM_SERVICE_INTERFACES_MEMORY_ACCESS_REPORTING_HH__
#define __UNISIM_SERVICE_INTERFACES_MEMORY_ACCESS_REPORTING_HH__

#include <inttypes.h>

#include "unisim/service/interfaces/interface.hh"
#include "unisim/util/debug/memory_access_type.hh"

namespace unisim {
namespace service {
namespace interfaces {

template <class ADDRESS>
class MemoryAccessReporting : public ServiceInterface
{
public:
	typedef unisim::util::debug::MemoryAccessType MemoryAccessType;
	typedef unisim::util::debug::MemoryType MemoryType;

	virtual bool ReportMemoryAccess(MemoryAccessType mat, MemoryType mt, ADDRESS addr, uint32_t size) = 0;
	virtual void ReportFetchInstruction(ADDRESS next_addr) = 0;
	virtual void ReportCommitInstruction(ADDRESS addr, unsigned int length /* in bytes */) = 0;
};

enum MemoryAccessReportingType
{
	REPORT_MEM_ACCESS  = 0,
	REPORT_FETCH_INSN  = 1,
	REPORT_COMMIT_INSN = 2
};

class MemoryAccessReportingControl : public ServiceInterface
{
public:
	virtual void RequiresMemoryAccessReporting(MemoryAccessReportingType type, bool report) = 0;
};

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

#endif
