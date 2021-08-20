/*
 *  Copyright (c) 2015,
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

#ifndef __LIBIEEE1666_TLM2_INTEROPERABILITY_LAYER_FWD_H__
#define __LIBIEEE1666_TLM2_INTEROPERABILITY_LAYER_FWD_H__

#include "core/fwd.h"

namespace tlm {

template <typename TRANS> class tlm_blocking_transport_if;
template <typename TRANS, typename PHASE> class tlm_fw_nonblocking_transport_if;
template <typename TRANS, typename PHASE> class tlm_bw_nonblocking_transport_if;
template <typename TRANS> class tlm_transport_dbg_if;
class tlm_dmi;
template <typename TRANS> class tlm_fw_direct_mem_if;
class tlm_bw_direct_mem_if;
struct tlm_base_protocol_types;
template <typename TYPES> class tlm_fw_transport_if;
template <typename TYPES> class tlm_bw_transport_if;
template <unsigned int BUSWIDTH, typename FW_IF, typename BW_IF> class tlm_base_initiator_socket_b;
template <unsigned int BUSWIDTH, typename FW_IF, typename BW_IF, int N, sc_core::sc_port_policy POL> class tlm_base_initiator_socket;
template <unsigned int BUSWIDTH, typename TYPES, int N, sc_core::sc_port_policy POL> class tlm_initiator_socket;
template <unsigned int BUSWIDTH, typename FW_IF, typename BW_IF> class tlm_base_target_socket_b;
template <unsigned int BUSWIDTH, typename FW_IF, typename BW_IF, int N, sc_core::sc_port_policy POL> class tlm_base_target_socket;
template <unsigned int BUSWIDTH, typename TYPES, int N, sc_core::sc_port_policy POL> class tlm_target_socket;
class tlm_phase;
class tlm_mm_interface;
class tlm_extension_base;
template <typename T> class tlm_extension;
class tlm_generic_payload;
class tlm_global_quantum;

} // end of namespace tlm

#endif // __LIBIEEE1666_TLM2_INTEROPERABILITY_LAYER_FWD_H__
