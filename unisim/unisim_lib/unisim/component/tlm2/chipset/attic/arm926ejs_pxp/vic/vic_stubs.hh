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
 
#ifndef __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_VIC_VIC_STUBS_HH__
#define __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_VIC_VIC_STUBS_HH__

#include <systemc>
#include <tlm>
#include <tlm_utils/passthrough_target_socket.h>
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace vic {

class VICIntSourceStub
	: public unisim::kernel::Object
	, public sc_module
{
public:
	sc_core::sc_out<bool> vicinttarget;

	SC_HAS_PROCESS(VICIntSourceStub);
	VICIntSourceStub(const sc_module_name &name, Object *parent = 0, 
			bool initvalue = false);
	~VICIntSourceStub();

	virtual bool BeginSetup();

	void Method();

private:
	bool value;

	unisim::kernel::variable::Parameter<bool> param_value;
};

class VICIntTargetStub
	: public unisim::kernel::Object
	, public sc_module
{
public:
	sc_core::sc_in<bool> vicintsource;

	SC_HAS_PROCESS(VICIntTargetStub);
	VICIntTargetStub(const sc_module_name &name, Object *parent = 0);
	~VICIntTargetStub();

	virtual bool BeginSetup();

	void Method();

private:
	bool value;
	uint32_t verbose;

	unisim::kernel::variable::Parameter<bool> param_value;
	unisim::kernel::variable::Parameter<uint32_t> param_verbose;

	unisim::kernel::logger::Logger logger;
};

class VICAddrSourceStub
	: public unisim::kernel::Object
	, public sc_module
{
public:
	sc_core::sc_out<uint32_t> vicaddrtarget;

	SC_HAS_PROCESS(VICAddrSourceStub);
	VICAddrSourceStub(const sc_module_name &name, Object *parent = 0);
	~VICAddrSourceStub();

	virtual bool BeginSetup();

	void Method();

private:
	uint32_t value;

	unisim::kernel::variable::Parameter<uint32_t> param_value;
};

class VICAddrTargetStub
	: public unisim::kernel::Object
	, public sc_module
{
public:
	sc_core::sc_in<uint32_t> vicaddrsource;

	SC_HAS_PROCESS(VICAddrTargetStub);
	VICAddrTargetStub(const sc_module_name &name, Object *parent = 0);
	~VICAddrTargetStub();

	virtual bool BeginSetup();

	void Method();

private:
	uint32_t value;
	uint32_t verbose;

	unisim::kernel::variable::Parameter<uint32_t> param_value;
	unisim::kernel::variable::Parameter<uint32_t> param_verbose;

	unisim::kernel::logger::Logger logger;
};

} // end of namespace unisim
} // end of namespace component
} // end of namespace tlm2
} // end of namespace chipset
} // end of namespace arm926ejs_pxp
} // end of namespace vic

#endif // __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_VIC_VIC_STUBS_HH__
