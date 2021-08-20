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
 
#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_MAPPING_HH__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_MAPPING_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace generic_router {

class Mapping {
public:
	bool used;
	uint64_t range_start;
	uint64_t range_end;
	unsigned int output_port;
	uint64_t translation;
	unsigned int id;

	Mapping()
		: used(false)
		, range_start(0)
		, range_end(0)
		, output_port(0)
		, translation(0)
		, id(0)
	{
	}
	
	Mapping(const Mapping& mapping)
		: used(mapping.used)
		, range_start(mapping.range_start)
		, range_end(mapping.range_end)
		, output_port(mapping.output_port)
		, translation(mapping.translation)
		, id(mapping.id)
	{
	}
	
	int operator != (const Mapping& mapping) const {
		return (used != mapping.used) ||
		       (range_start != mapping.range_start) ||
		       (range_end != mapping.range_end) ||
		       (output_port != mapping.output_port) ||
		       (translation != mapping.translation) ||
		       (id != mapping.id);
	}

	int operator == (const Mapping& mapping) const {
		return (used == mapping.used) &&
		       (range_start == mapping.range_start) &&
		       (range_end == mapping.range_end) &&
		       (output_port == mapping.output_port) &&
		       (translation == mapping.translation) &&
		       (id == mapping.id);
	}

	Mapping& operator = (const Mapping& mapping) {
		used = mapping.used;
		range_start = mapping.range_start;
		range_end = mapping.range_end;
		output_port = mapping.output_port;
		translation = mapping.translation;
		id = mapping.id;
		return *this;
	}
	
	bool Match(uint64_t addr, uint32_t size) const
	{
		return used && (addr >= range_start) && ((addr + size - 1) <= range_end);
	}
};

} // end of namespace unisim
} // end of namespace component
} // end of namespace tlm2
} // end of namespace interconnect
} // end of namespace generic_router

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_MAPPING_HH__
