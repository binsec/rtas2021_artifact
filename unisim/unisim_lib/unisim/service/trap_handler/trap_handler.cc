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

#include "unisim/service/trap_handler/trap_handler.hh"
#include <sstream>

namespace unisim {
namespace service {
namespace trap_handler {

using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;

TrapHandler::TrapHandler(const char *name, Object *parent)
	: unisim::kernel::Object(name, parent, "Trap handler")
	// , Service<TrapReporting>(name, parent)
	, trap_reporting_export()
	, logger(*this)
	, trap_reporting_export_name()
	, param_trap_reporting_export_name()
	, num_traps(0)
	, param_num_traps("num-traps", this, num_traps, "Total number of traps that will be received.")
	, send_to_logger(false)
	, param_send_to_logger("send-traps-to-logger", this, send_to_logger, "Send the traps to the logger.")
	, external_handler(0)
{
	for ( unsigned int i = 0;
			i < num_traps;
			i++ )
	{
		std::stringstream name;
		name << "trap-reporting-export[" << i << "]";
		trap_reporting_export_name.push_back(new std::string(name.str().c_str()));
		std::stringstream param_name;
		param_name << "trap-reporting-export-name[" << i << "]";
		std::stringstream description;
		description << "The name of the " << i << " trap-reporting-export";
		Parameter<std::string> *param =
				new Parameter<std::string>(param_name.str().c_str(), this,
						*trap_reporting_export_name[i],
						description.str().c_str());
		// TODO: uncomment the following sets
		// param_name->SetMutable(false);
		// param_name->SetVisible(false);
		// param_name->SetSerializable(false);
		param_trap_reporting_export_name.push_back(param);
		ServiceExport<TrapReporting> *exp =
				new ServiceExport<TrapReporting>(((std::string)(*param)).c_str(), this);
		trap_reporting_export.push_back(exp);
		std::stringstream identifier_name;
		identifier_name << "trap_handler_identifier[" << i << "]";
		TrapHandlerIdentifier *identifier =
				new TrapHandlerIdentifier(i, this,
						identifier_name.str().c_str(), this);
		*exp >> identifier->trap_reporting_export;
	}
}

TrapHandler::~TrapHandler()
{
	for ( unsigned int i = 0;
			i < num_traps;
			i++ )
	{
		delete trap_reporting_export[i];
		trap_reporting_export[i] = 0;
		delete param_trap_reporting_export_name[i];
		param_trap_reporting_export_name[i] = 0;
		delete trap_reporting_export_name[i];
		trap_reporting_export_name[i] = 0;
	}
	trap_reporting_export.clear();
	param_trap_reporting_export_name.clear();
	trap_reporting_export_name.clear();
}

bool
TrapHandler::EndSetup()
{
	return true;
}

void
TrapHandler::ReportTrap(int id)
{
	// what to do with non identified traps???
	if ( send_to_logger )
		logger << DebugInfo
			<< "Received trap from unknown source."
			<< " In id = " << id << "."
			<< EndDebugInfo;
	if ( external_handler )
		external_handler->ExternalTrap(id);
}

void
TrapHandler::ReportTrap(int id,
		const unisim::kernel::Object &obj)
{
	if ( send_to_logger )
		logger << DebugInfo
			<< "Received trap from '" << obj.GetName() << "'."
			<< " In id = " << id << "."
			<< " External_handler = " << external_handler << "."
			<< EndDebugInfo;
	if ( external_handler )
		external_handler->ExternalTrap(id);
}

void
TrapHandler::ReportTrap(int id,
		const unisim::kernel::Object &obj,
		const std::string &str)
{
	if ( send_to_logger )
		logger << DebugInfo
			<< "Received trap from '" << obj.GetName() << "' with message: '"
			<< str << "'."
			<< " In id = " << id << "."
			<< EndDebugInfo;
	if ( external_handler )
		external_handler->ExternalTrap(id);
}

void
TrapHandler::ReportTrap(int id,
		const unisim::kernel::Object &obj,
		const char *c_str)
{
	if ( send_to_logger )
		logger << DebugInfo
			<< "Received trap from '" << obj.GetName() << "' with message: '"
			<< c_str << "'."
			<< " In id = " << id << "."
			<< EndDebugInfo;
	if ( external_handler )
		external_handler->ExternalTrap(id);
}

ExternalTrapHandlerInterface *
TrapHandler::
SetExternalTrapHandler(ExternalTrapHandlerInterface *handler)
{
	ExternalTrapHandlerInterface *old;
	old = external_handler;
	external_handler = handler;
	return old;
}

} // end of namespace trap_handler
} // end of namespace service
} // end of namespace unisim
