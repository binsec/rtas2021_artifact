/*
 *  Copyright (c) 2016,
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

#include "tlm_utils/instance_specific_extensions.h"

namespace tlm_utils {

static unsigned int next_instance_specific_extension_id = 0;

//////////////////////////////// embedded_instance_specific_extension //////////////////////////////////////

embedded_instance_specific_extension::embedded_instance_specific_extension(tlm::tlm_generic_payload *_trans)
	: trans(_trans)
{
}

tlm_extension_base* embedded_instance_specific_extension::clone() const
{
	throw std::runtime_error("cloning an instance specific extension is not allowed");
	return 0;
}

void embedded_instance_specific_extension::copy_from(tlm_extension_base const &)
{
	throw std::runtime_error("copying an instance specific extension is not allowed");
}

proxy<instance_specific_extension_set>& embedded_instance_specific_extension::get_proxy(unsigned int accessor_id)
{
	return *instance_specific_extensions->proxies[accessor_id];
}

//////////////////////////////// instance_specific_extension_base //////////////////////////////////////


unsigned int instance_specific_extension_base::allocate_instance_specific_extension_id()
{
	return num_extensions++;
}



//////////////////////////////// instance_specific_extension_accessor //////////////////////////////////////

unsigned int instance_specific_extension_accessor::num_accessor_instances = 0;

unsigned int instance_specific_extension_accessor::allocate_id()
{
	return num_accessor_instances++;
}
	
instance_specific_extension_accessor::instance_specific_extension_accessor()
	: id(allocate_id())
{
}



} // end of namespace tlm_utils


