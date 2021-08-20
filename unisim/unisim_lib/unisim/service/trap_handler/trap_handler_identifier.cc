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

#include "unisim/service/trap_handler/trap_handler_identifier.hh"

namespace unisim {
namespace service {
namespace trap_handler {

using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;

TrapHandlerIdentifier::TrapHandlerIdentifier(int _id,
		TrapHandlerIdentifierInterface *_interface,
		const char *name,
		Object *parent)
	: unisim::kernel::Object(name, parent, "Trap handler identifier")
	, Service<TrapReporting>(name, parent)
	, trap_reporting_export("trap_reporting_import", this)
	, id(_id)
	, intf(_interface)
	, logger(*this)
{}

TrapHandlerIdentifier::~TrapHandlerIdentifier() { id = 0; intf = 0; }

bool
TrapHandlerIdentifier::BeginSetup()
{
	if ( intf == 0 )
	{
		logger << DebugError
				<< "Could not initialize trap handler identifier because no"
				<< " interface was given."
				<< EndDebugError;
		return false;
	}
	return true;
}

void
TrapHandlerIdentifier::
ReportTrap()
{
	intf->ReportTrap(id);
}

void
TrapHandlerIdentifier::
ReportTrap(const unisim::kernel::Object &obj)
{
	intf->ReportTrap(id, obj);
}

void
TrapHandlerIdentifier::ReportTrap(const unisim::kernel::Object &obj,
						const std::string &str)
{
	intf->ReportTrap(id, obj, str);
}

void
TrapHandlerIdentifier::ReportTrap(const unisim::kernel::Object &obj,
						const char *c_str)
{
	intf->ReportTrap(id, obj, c_str);
}

} // end of namespace trap_handler
} // end of namespace service
} // end of namespace unisim
