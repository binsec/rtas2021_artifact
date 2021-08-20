/*
 *  Copyright (c) 2008,
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
 
#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_CONFIG_HH__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_CONFIG_HH__

#include <tlm>
#include "unisim/component/tlm2/interconnect/generic_router/mapping.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace generic_router {

class Config {
public:
	typedef uint64_t ADDRESS;
	static const unsigned int INPUT_SOCKETS = 1;
	static const unsigned int OUTPUT_SOCKETS = 1;
	static const unsigned int MAX_NUM_MAPPINGS = 256;
	static const unsigned int INPUT_BUSWIDTH = 32;
	static const unsigned int OUTPUT_BUSWIDTH = 32;
	typedef tlm::tlm_base_protocol_types TYPES;
	static const bool VERBOSE = false;
	static const unsigned int BURST_LENGTH = 1;
	typedef unisim::component::tlm2::interconnect::generic_router::Mapping MAPPING;
};

class VerboseConfig : public Config {
public:
	static const bool VERBOSE = true;
};

} // end of namespace generic_router
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_CONFIG_HH__

