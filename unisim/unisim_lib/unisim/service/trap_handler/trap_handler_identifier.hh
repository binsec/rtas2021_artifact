/*
 *  Copyright (c) 2010,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_SERVICE_TRAP_HANDLER_IDENTIFIER_HH__
#define __UNISIM_SERVICE_TRAP_HANDLER_IDENTIFIER_HH__

#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/trap_reporting.hh"
#include "unisim/service/trap_handler/trap_handler_identifier_interface.hh"

namespace unisim {
namespace service {
namespace trap_handler {

using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::Object;
using unisim::service::interfaces::TrapReporting;

class TrapHandlerIdentifier
	: public Service<TrapReporting>
{
public:
	ServiceExport<TrapReporting> trap_reporting_export;

	TrapHandlerIdentifier(int id,
			TrapHandlerIdentifierInterface *intf,
			const char *name,
			Object *parent = 0);
	virtual ~TrapHandlerIdentifier();

	virtual bool BeginSetup();

private:
	int id;
	TrapHandlerIdentifierInterface *intf;

	virtual void ReportTrap();
	virtual void ReportTrap(const unisim::kernel::Object &obj);
	virtual void ReportTrap(const unisim::kernel::Object &obj,
			const std::string &str);
	virtual void ReportTrap(const unisim::kernel::Object &obj,
			const char *c_str);
	// the kernel logger
	unisim::kernel::logger::Logger logger;

};

} // end of namespace trap_handler
} // end of namespace service
} // end of namespace unisim


#endif // __UNISIM_SERVICE_TRAP_HANDLER_IDENTIFIER_HH__

