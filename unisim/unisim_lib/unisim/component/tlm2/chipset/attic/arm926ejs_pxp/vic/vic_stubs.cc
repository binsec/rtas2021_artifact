/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#include <systemc>
#include <tlm>
#include <tlm_utils/passthrough_target_socket.h>
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include <inttypes.h>
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic_stubs.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace vic {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;

VICIntSourceStub ::
VICIntSourceStub(const sc_module_name &name, Object *parent, bool initvalue)
	: unisim::kernel::Object(name, parent)
	, sc_module(name)
	, vicinttarget("vicinttarget")
	, value(initvalue)
	, param_value("value", this, value,
			"Value to be sent by the interrupt source stub.")
{
	SC_METHOD(Method);
	vicinttarget.initialize(value);
}

VICIntSourceStub ::
~VICIntSourceStub()
{
}

bool
VICIntSourceStub ::
BeginSetup()
{
	return true;
}

void
VICIntSourceStub ::
Method()
{
	vicinttarget = value;
}

VICIntTargetStub ::
VICIntTargetStub(const sc_module_name &name, Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_module(name)
	, vicintsource()
	, value(false)
	, verbose(false)
	, param_value("value", this, value,
			"Value received by the interrupt target stub.")
	, param_verbose("verbose", this, verbose,
			"Display verbose if different than 0 (display changes in the"
			" interrupt value)")
	, logger(*this)
{
	SC_METHOD(Method);
	sensitive << vicintsource;
}

VICIntTargetStub ::
~VICIntTargetStub()
{
}

bool
VICIntTargetStub ::
BeginSetup()
{
	return true;
}

void
VICIntTargetStub ::
Method()
{
	bool old_value = value;

	value = vicintsource;

	if ( verbose )
		logger << DebugInfo
			<< "Received change on interrupt:" << std::endl
			<< " - old value = " << old_value << std::endl
			<< " - new value = " << value
			<< EndDebugInfo;
}

VICAddrSourceStub ::
VICAddrSourceStub(const sc_module_name &name, Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_module(name)
	, vicaddrtarget()
	, value(0)
	, param_value("value", this, value,
			"Value to send by the interrupt address source stub.")
{
	SC_METHOD(Method);
}

VICAddrSourceStub ::
~VICAddrSourceStub()
{
}

bool
VICAddrSourceStub ::
BeginSetup()
{
	return true;
}

void
VICAddrSourceStub ::
Method()
{
	vicaddrtarget = value;
}

VICAddrTargetStub ::
VICAddrTargetStub(const sc_module_name &name, Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_module(name)
	, vicaddrsource()
	, value(0)
	, verbose(0)
	, param_value("value", this, value,
			"Value received by the interrupt address target stub.")
	, param_verbose("verbose", this, verbose,
			"Display verbose if different than 0 (display changes in the"
			" interrupt address value)")
	, logger(*this)
{
	SC_METHOD(Method);
	sensitive << vicaddrsource;
}

VICAddrTargetStub ::
~VICAddrTargetStub()
{
}

bool
VICAddrTargetStub ::
BeginSetup()
{
	return true;
}

void
VICAddrTargetStub ::
Method()
{
	uint32_t old_value = value;

	value = vicaddrsource;

	if ( verbose )
		logger << DebugInfo
			<< "Received change on interrupt address:" << std::endl
			<< " - old value = " << old_value 
			<< " (0x" << std::hex << old_value << std::dec << ")" << std::endl
			<< " - new value = " << value
			<< " (0x" << std::hex << value << std::dec << ")"
			<< EndDebugInfo;
}

} // end of namespace unisim
} // end of namespace component
} // end of namespace tlm2
} // end of namespace chipset
} // end of namespace arm926ejs_pxp
} // end of namespace vic


