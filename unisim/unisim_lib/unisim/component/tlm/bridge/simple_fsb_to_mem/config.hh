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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM_BRIDGE_SIMPLEFSBTOMEM_CONFIG_HH__
#define __UNISIM_COMPONENT_TLM_BRIDGE_SIMPLEFSBTOMEM_CONFIG_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm {
namespace bridge {
namespace simple_fsb_to_mem {

/**
 * IMPORTANT
 * This implementation currently only supports busses with the same burst size
 */
class Addr32BurstSize32_Config {
public:
	static const unsigned int FSB_BURST_SIZE = 32;
	static const unsigned int MEM_BURST_SIZE = 32;
	typedef uint32_t fsb_address_t;
	typedef uint32_t mem_address_t;
	static const unsigned int BUFFER_SIZE = 16;
	static const bool DEBUG = false;
};

class Addr32BurstSize32Debug_Config {
public:
	static const unsigned int FSB_BURST_SIZE = 32;
	static const unsigned int MEM_BURST_SIZE = 32;
	typedef uint32_t fsb_address_t;
	typedef uint32_t mem_address_t;
	static const unsigned int BUFFER_SIZE = 16;
	static const bool DEBUG = true;
};

} // end of namespace simple_fsb_to_mem
} // end of namespace bridge
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM_BRIDGE_SIMPLEFSBTOMEM_SIMPLEFSBTOMEM_HH__
