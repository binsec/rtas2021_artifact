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

#ifndef __TLM_UTILS_SIMPLE_INITIATOR_SOCKET_H__
#define __TLM_UTILS_SIMPLE_INITIATOR_SOCKET_H__

#include <tlm>

namespace tlm_utils {
	
template <typename MODULE, unsigned int BUSWIDTH = 32, typename TYPES = tlm::tlm_base_protocol_types>
class simple_initiator_socket
	: public tlm::tlm_initiator_socket<BUSWIDTH, TYPES>
{
public:
	typedef typename TYPES::tlm_payload_type transaction_type;
	typedef typename TYPES::tlm_phase_type phase_type;
	typedef tlm::tlm_sync_enum sync_enum_type;

	simple_initiator_socket();
	explicit simple_initiator_socket(const char *n);
	void register_nb_transport_bw(MODULE *mod, sync_enum_type (MODULE::*cb)(transaction_type&, phase_type&, sc_core::sc_time&));
	void register_invalidate_direct_mem_ptr(MODULE* mod, void (MODULE::*cb)(sc_dt::uint64, sc_dt::uint64));
private:
	struct bw_transport_impl_s : public tlm::tlm_bw_transport_if<TYPES>
	{
		bw_transport_impl_s();
		
		virtual tlm::tlm_sync_enum nb_transport_bw(transaction_type&, phase_type&, sc_core::sc_time&);
		virtual void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64);
		
		MODULE *mod;
		tlm::tlm_sync_enum (MODULE::*nb_transport_bw_cb)(typename TYPES::tlm_payload_type&, typename TYPES::tlm_phase_type&, sc_core::sc_time&);
		void (MODULE::*invalidate_direct_mem_ptr_cb)(sc_dt::uint64, sc_dt::uint64);
	};
	
	bw_transport_impl_s bw_transport_impl;
};

template <typename MODULE, unsigned int BUSWIDTH = 32, typename TYPES = tlm::tlm_base_protocol_types>
class simple_initiator_socket_tagged
	: public tlm::tlm_initiator_socket<BUSWIDTH, TYPES>
{
public:
	typedef typename TYPES::tlm_payload_type transaction_type;
	typedef typename TYPES::tlm_phase_type phase_type;
	typedef tlm::tlm_sync_enum sync_enum_type;

	simple_initiator_socket_tagged();
	explicit simple_initiator_socket_tagged(const char *n);
	void register_nb_transport_bw(MODULE *mod, sync_enum_type (MODULE::*cb)(int,transaction_type&, phase_type&, sc_core::sc_time&), int id);
	void register_invalidate_direct_mem_ptr(MODULE* mod, void (MODULE::*cb)(int,sc_dt::uint64, sc_dt::uint64), int id);
private:
	struct bw_transport_impl_s : public tlm::tlm_bw_transport_if<TYPES>
	{
		bw_transport_impl_s();
		
		virtual tlm::tlm_sync_enum nb_transport_bw(transaction_type&, phase_type&, sc_core::sc_time&);
		virtual void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64);
		
		MODULE *mod;
		int id;
		tlm::tlm_sync_enum (MODULE::*nb_transport_bw_cb)(int, typename TYPES::tlm_payload_type&, typename TYPES::tlm_phase_type&, sc_core::sc_time&);
		void (MODULE::*invalidate_direct_mem_ptr_cb)(int, sc_dt::uint64, sc_dt::uint64);
	};
	
	bw_transport_impl_s bw_transport_impl;
};

/////////////////////////////////////////////// simple_initiator_socket<> ////////////////////////////////////////////////////

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_initiator_socket<MODULE, BUSWIDTH, TYPES>::simple_initiator_socket()
	: tlm::tlm_initiator_socket<BUSWIDTH, TYPES>(sc_core::sc_gen_unique_name("simple_initiator_socket"))
{
	this->bind(bw_transport_impl);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_initiator_socket<MODULE, BUSWIDTH, TYPES>::simple_initiator_socket(const char *n)
	: tlm::tlm_initiator_socket<BUSWIDTH, TYPES>(sc_core::sc_gen_unique_name(n))
{
	this->bind(bw_transport_impl);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_initiator_socket<MODULE, BUSWIDTH, TYPES>::register_nb_transport_bw(MODULE *mod, sync_enum_type (MODULE::*cb)(transaction_type&, phase_type&, sc_core::sc_time&))
{
	if(bw_transport_impl.mod && (bw_transport_impl.mod != mod)) throw "tlm_utils::simple_initiator_socket: a module is already registered";
	bw_transport_impl.mod = mod;
	bw_transport_impl.nb_transport_bw_cb = cb;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_initiator_socket<MODULE, BUSWIDTH, TYPES>::register_invalidate_direct_mem_ptr(MODULE *mod, void (MODULE::*cb)(sc_dt::uint64, sc_dt::uint64))
{
	if(bw_transport_impl.mod && (bw_transport_impl.mod != mod)) throw "tlm_utils::simple_initiator_socket: a module is already registered";
	bw_transport_impl.mod = mod;
	bw_transport_impl.invalidate_direct_mem_ptr_cb = cb;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_initiator_socket<MODULE, BUSWIDTH, TYPES>::bw_transport_impl_s::bw_transport_impl_s()
	: mod(0)
	, nb_transport_bw_cb(0)
	, invalidate_direct_mem_ptr_cb(0)
{
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
tlm::tlm_sync_enum simple_initiator_socket<MODULE, BUSWIDTH, TYPES>::bw_transport_impl_s::nb_transport_bw(transaction_type& trans, phase_type& phase, sc_core::sc_time& t)
{
	if(!mod || !nb_transport_bw_cb) throw std::runtime_error("tlm_utils::simple_initiator_socket: no nb_transport_bw callback registered");
	(mod->*nb_transport_bw_cb)(trans, phase, t);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_initiator_socket<MODULE, BUSWIDTH, TYPES>::bw_transport_impl_s::invalidate_direct_mem_ptr(sc_dt::uint64 start_address, sc_dt::uint64 end_address)
{
	if(!mod || !invalidate_direct_mem_ptr_cb) return;
	
	(mod->*invalidate_direct_mem_ptr_cb)(start_address, end_address);
}

//////////////////////////////////////////// simple_initiator_socket_tagged<> //////////////////////////////////////////////////

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_initiator_socket_tagged<MODULE, BUSWIDTH, TYPES>::simple_initiator_socket_tagged()
	: tlm::tlm_initiator_socket<BUSWIDTH, TYPES>(sc_core::sc_gen_unique_name("simple_initiator_socket_tagged"))
{
	this->bind(bw_transport_impl);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_initiator_socket_tagged<MODULE, BUSWIDTH, TYPES>::simple_initiator_socket_tagged(const char *n)
	: tlm::tlm_initiator_socket<BUSWIDTH, TYPES>(sc_core::sc_gen_unique_name(n))
{
	this->bind(bw_transport_impl);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_initiator_socket_tagged<MODULE, BUSWIDTH, TYPES>::register_nb_transport_bw(MODULE *mod, sync_enum_type (MODULE::*cb)(int, transaction_type&, phase_type&, sc_core::sc_time&), int id)
{
	if(bw_transport_impl.mod && (bw_transport_impl.mod != mod)) throw "tlm_utils::simple_initiator_socket_tagged: a module is already registered";
	bw_transport_impl.mod = mod;
	bw_transport_impl.id = id;	
	bw_transport_impl.nb_transport_bw_cb = cb;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_initiator_socket_tagged<MODULE, BUSWIDTH, TYPES>::register_invalidate_direct_mem_ptr(MODULE *mod, void (MODULE::*cb)(int, sc_dt::uint64, sc_dt::uint64), int id)
{
	if(bw_transport_impl.mod && (bw_transport_impl.mod != mod)) throw "tlm_utils::simple_initiator_socket_tagged: a module is already registered";
	bw_transport_impl.mod = mod;
	bw_transport_impl.id = id;
	bw_transport_impl.invalidate_direct_mem_ptr_cb = cb;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_initiator_socket_tagged<MODULE, BUSWIDTH, TYPES>::bw_transport_impl_s::bw_transport_impl_s()
	: mod(0)
	, nb_transport_bw_cb(0)
	, invalidate_direct_mem_ptr_cb(0)
{
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
tlm::tlm_sync_enum simple_initiator_socket_tagged<MODULE, BUSWIDTH, TYPES>::bw_transport_impl_s::nb_transport_bw(transaction_type& trans, phase_type& phase, sc_core::sc_time& t)
{
	if(!mod || !nb_transport_bw_cb) throw std::runtime_error("tlm_utils::simple_initiator_socket_tagged: no nb_transport_bw callback registered");
	(mod->*nb_transport_bw_cb)(id, trans, phase, t);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_initiator_socket_tagged<MODULE, BUSWIDTH, TYPES>::bw_transport_impl_s::invalidate_direct_mem_ptr(sc_dt::uint64 start_address, sc_dt::uint64 end_address)
{
	if(!mod || !invalidate_direct_mem_ptr_cb) return;
	
	(mod->*invalidate_direct_mem_ptr_cb)(id, start_address, end_address);
}

} // end of namespace tlm_utils

#endif // __LIBIEEE1666_TLM2_UTIL_SIMPLE_INITIATOR_SOCKET_H__
