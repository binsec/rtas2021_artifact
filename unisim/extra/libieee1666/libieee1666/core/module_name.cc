/*
 *  Copyright (c) 2014,
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

#include "core/module_name.h"
#include "core/module.h"
#include "core/kernel.h"

namespace sc_core {

// Notes for the developper. The rationale of Class sc_module_name is:
//  (1) to avoid, for the user, explicit calls to sc_module constructor with a name passed as argument.
//      implicit calls to default sc_module constructor are sufficient.
//  (2) to keep track of current object being constructed, from the construction of sc_object,
//      base class of sc_module, to the destruction of the sc_module_name instance passed as argument
//      to the user's module constructor.

sc_module_name::sc_module_name(const char *module_name_c_str)
	: module_name_str(module_name_c_str)
	, pushed(false)
	, module(0)
{
	sc_kernel::get_kernel()->push_module_name(this);
	pushed = true;
}

sc_module_name::sc_module_name(const sc_module_name& module_name)
	: module_name_str(module_name.module_name_str)
	, pushed(false)
	, module(module_name.module)
{
}

sc_module_name::~sc_module_name()
{
	if(module) module->end_module();
	if(pushed) sc_kernel::get_kernel()->pop_module_name();
}

sc_module_name::operator const char * () const
{
	return module_name_str.c_str();
}

sc_module_name::sc_module_name()
{
}

sc_module_name& sc_module_name::operator = (const sc_module_name& module_name)
{
	return *this;
}

void sc_module_name::set_module(sc_module *_module)
{
	module = _module;
}

} // end of namespace sc_core
