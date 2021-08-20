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

#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_BUS_BUS_TYPES_HH__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_BUS_BUS_TYPES_HH__

#include <tlm_utils/peq_with_get.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace generic_bus {

template<typename PAYLOAD_TYPE = tlm::tlm_generic_payload>
class BusTlmGenericPayload {
public:
	PAYLOAD_TYPE *payload;
	bool from_initiator;
	unsigned int id;
	bool send_end_req;
	bool send_end_resp;
};

template<typename TYPES = tlm::tlm_base_protocol_types>
class BusTlmBaseProtocolTypes {
public:
	typedef BusTlmGenericPayload<typename TYPES::tlm_payload_type> tlm_payload_type;
	typedef typename TYPES::tlm_phase_type tlm_phase_type;
};

template<typename TYPES = tlm::tlm_base_protocol_types>
class BusTlmGenericProtocol {
public:
	typename TYPES::tlm_payload_type *payload;
	bool from_initiator;
	unsigned int id;
	typename TYPES::tlm_phase_type phase;
	bool send_end_req;
	bool send_end_resp;
};

template<typename TYPES = tlm::tlm_base_protocol_types>
class BusPeq : public tlm_utils::peq_with_get<BusTlmGenericProtocol<TYPES> > {
public:
	BusPeq(const char *name) : tlm_utils::peq_with_get<BusTlmGenericProtocol<TYPES> >(name) {}
};

} // end of namespace generic_bus
} // end of namespace simple_bus
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_BUS_BUS_TYPES_HH__
