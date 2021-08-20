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

#ifndef __TLM_UTILS_SIMPLE_TARGET_SOCKET_H__
#define __TLM_UTILS_SIMPLE_TARGET_SOCKET_H__

#include <tlm_utils/peq_with_get.h>
#include <stack>
#include <vector>
#include <map>

namespace tlm_utils {

template <typename MODULE, unsigned int BUSWIDTH = 32, typename TYPES = tlm::tlm_base_protocol_types>
class simple_target_socket
	: public tlm::tlm_target_socket<BUSWIDTH, TYPES>
{
public:
	typedef typename TYPES::tlm_payload_type transaction_type;
	typedef typename TYPES::tlm_phase_type phase_type;
	typedef tlm::tlm_sync_enum sync_enum_type;

	simple_target_socket();
	
	explicit simple_target_socket(const char *n);
	tlm::tlm_bw_transport_if<TYPES> *operator -> ();
	void register_nb_transport_fw(MODULE* mod, sync_enum_type (MODULE::*cb)(transaction_type&, phase_type&, sc_core::sc_time&));
	void register_b_transport(MODULE* mod, void (MODULE::*cb)(transaction_type&, sc_core::sc_time&));
	void register_transport_dbg(MODULE* mod, unsigned int (MODULE::*cb)(transaction_type&));
	void register_get_direct_mem_ptr(MODULE* mod, bool (MODULE::*cb)(transaction_type&, tlm::tlm_dmi&));
private:
	struct fw_transport_impl_s
		: public tlm::tlm_fw_transport_if<TYPES>
		, public tlm::tlm_mm_interface
	{
		fw_transport_impl_s(simple_target_socket<MODULE, BUSWIDTH, TYPES> *socket);
		~fw_transport_impl_s();
		
		virtual tlm::tlm_sync_enum nb_transport_fw(transaction_type& trans, phase_type& phase, sc_core::sc_time& t);
		virtual void b_transport(transaction_type& trans, sc_core::sc_time& t);
		void b_to_nb_transport(transaction_type& trans, sc_core::sc_time& t);
		virtual unsigned int transport_dbg(transaction_type& trans);
		virtual bool get_direct_mem_ptr(transaction_type& trans, tlm::tlm_dmi& dmi_data);

		simple_target_socket<MODULE, BUSWIDTH, TYPES> *socket;
		MODULE *mod;
		sync_enum_type (MODULE::*nb_transport_fw_cb)(transaction_type&, phase_type&, sc_core::sc_time&);
		void (MODULE::*b_transport_cb)(transaction_type&, sc_core::sc_time&);
		unsigned int (MODULE::*transport_dbg_cb)(transaction_type&);
		bool (MODULE::*get_direct_mem_ptr_cb)(transaction_type&, tlm::tlm_dmi&);
		
		struct nb_to_b_transport_process_s
		{
			nb_to_b_transport_process_s(fw_transport_impl_s *owner);
			
			sc_core::sc_event begin_req_event;
			transaction_type *trans;
			sc_core::sc_process_handle process_handle;
			fw_transport_impl_s *owner;
		};
		
		std::stack<nb_to_b_transport_process_s *> free_nb_to_b_transport_process_pool;
		std::vector<nb_to_b_transport_process_s *> nb_to_b_transport_process_pool;
		
		peq_with_get<transaction_type> b_transport_peq;
		std::map<transaction_type *, sc_core::sc_event *> pending_payload_freed;
		
		void b_to_nb_transport_process();
		void nb_to_b_transport_process(nb_to_b_transport_process_s *self);
		void spawn_nb_to_b_process(transaction_type *trans, sc_core::sc_time& t);
		void reuse_nb_to_b_process(nb_to_b_transport_process_s *p);
		
		virtual void free(tlm::tlm_generic_payload* trans);
	};
	
	struct bw_transport_impl_s : public tlm::tlm_bw_transport_if<TYPES>
	{
		bw_transport_impl_s(simple_target_socket<MODULE, BUSWIDTH, TYPES> *socket);
		
		virtual tlm::tlm_sync_enum nb_transport_bw(transaction_type&, phase_type&, sc_core::sc_time&);
		virtual void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64);

		simple_target_socket<MODULE, BUSWIDTH, TYPES> *socket;
	};
	
	friend struct fw_transport_impl_s;
	friend struct bw_transport_impl_s;

	fw_transport_impl_s fw_transport_impl;
	bw_transport_impl_s bw_transport_impl;
	std::map<transaction_type *, sc_core::sc_event *> pending_b_to_nb_transport_end_resp;
	std::map<transaction_type *, sc_core::sc_event *> pending_b_to_nb_transport_end_req;
	std::map<transaction_type *, sc_core::sc_event *> pending_nb_to_b_transport_end_resp;
};

template <typename MODULE, unsigned int BUSWIDTH = 32, typename TYPES = tlm::tlm_base_protocol_types>
class simple_target_socket_tagged : public tlm::tlm_target_socket<BUSWIDTH, TYPES>
{
public:
	typedef typename TYPES::tlm_payload_type transaction_type;
	typedef typename TYPES::tlm_phase_type phase_type;
	typedef tlm::tlm_sync_enum sync_enum_type;
	typedef tlm::tlm_fw_transport_if<TYPES> fw_interface_type;
	typedef tlm::tlm_bw_transport_if<TYPES> bw_interface_type;
	typedef tlm::tlm_target_socket<BUSWIDTH, TYPES> base_type;
	simple_target_socket_tagged();
	explicit simple_target_socket_tagged(const char *n);
	tlm::tlm_bw_transport_if<TYPES> *operator -> ();
	void register_nb_transport_fw(MODULE *mod, sync_enum_type (MODULE::*cb)(int id, transaction_type&, phase_type&, sc_core::sc_time&), int id);
	void register_b_transport(MODULE *mod, void (MODULE::*cb)(int id, transaction_type&, sc_core::sc_time&), int id);
	void register_transport_dbg(MODULE *mod, unsigned int (MODULE::*cb)(int id, transaction_type&), int id);
	void register_get_direct_mem_ptr(MODULE *mod, bool (MODULE::*cb)(int id, transaction_type&, tlm::tlm_dmi&), int id);
private:
	struct fw_transport_impl_s
		: public tlm::tlm_fw_transport_if<TYPES>
		, public tlm::tlm_mm_interface
	{
		fw_transport_impl_s(simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES> *socket);
		~fw_transport_impl_s();
		
		virtual tlm::tlm_sync_enum nb_transport_fw(transaction_type& trans, phase_type& phase, sc_core::sc_time& t);
		virtual void b_transport(transaction_type& trans, sc_core::sc_time& t);
		void b_to_nb_transport(transaction_type& trans, sc_core::sc_time& t);
		virtual unsigned int transport_dbg(transaction_type& trans);
		virtual bool get_direct_mem_ptr(transaction_type& trans, tlm::tlm_dmi& dmi_data);

		simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES> *socket;
		MODULE *mod;
		sync_enum_type (MODULE::*nb_transport_fw_cb)(int, transaction_type&, phase_type&, sc_core::sc_time&);
		int nb_transport_fw_id;
		void (MODULE::*b_transport_cb)(int, transaction_type&, sc_core::sc_time&);
		int b_transport_id;
		unsigned int (MODULE::*transport_dbg_cb)(int, transaction_type&);
		int transport_dbg_id;
		bool (MODULE::*get_direct_mem_ptr_cb)(int, transaction_type&, tlm::tlm_dmi&);
		int get_direct_mem_ptr_id;
		
		struct nb_to_b_transport_process_s
		{
			nb_to_b_transport_process_s(fw_transport_impl_s *owner);
			
			sc_core::sc_event begin_req_event;
			transaction_type *trans;
			sc_core::sc_process_handle process_handle;
			fw_transport_impl_s *owner;
		};
		
		std::stack<nb_to_b_transport_process_s *> free_nb_to_b_transport_process_pool;
		std::vector<nb_to_b_transport_process_s *> nb_to_b_transport_process_pool;
		
		peq_with_get<transaction_type> b_transport_peq;
		std::map<transaction_type *, sc_core::sc_event *> pending_payload_freed;
		
		void b_to_nb_transport_process();
		void nb_to_b_transport_process(nb_to_b_transport_process_s *self);
		void spawn_nb_to_b_process(transaction_type *trans, sc_core::sc_time& t);
		void reuse_nb_to_b_process(nb_to_b_transport_process_s *p);
		
		virtual void free(tlm::tlm_generic_payload* trans);
	};
	
	struct bw_transport_impl_s : public tlm::tlm_bw_transport_if<TYPES>
	{
		bw_transport_impl_s(simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES> *socket);
		
		virtual tlm::tlm_sync_enum nb_transport_bw(transaction_type&, phase_type&, sc_core::sc_time&);
		virtual void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64);

		simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES> *socket;
	};
	
	friend struct fw_transport_impl_s;
	friend struct bw_transport_impl_s;

	fw_transport_impl_s fw_transport_impl;
	bw_transport_impl_s bw_transport_impl;
	std::map<transaction_type *, sc_core::sc_event *> pending_b_to_nb_transport_end_resp;
	std::map<transaction_type *, sc_core::sc_event *> pending_b_to_nb_transport_end_req;
	std::map<transaction_type *, sc_core::sc_event *> pending_nb_to_b_transport_end_resp;
};

////////////////////////////////////////// simple_target_socket<> /////////////////////////////////////////////

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_target_socket<MODULE, BUSWIDTH, TYPES>::simple_target_socket()
	: tlm::tlm_target_socket<BUSWIDTH, TYPES>(sc_core::sc_gen_unique_name("simple_target_socket"))
	, fw_transport_impl(this)
	, bw_transport_impl(this)
	, pending_b_to_nb_transport_end_resp()
	, pending_b_to_nb_transport_end_req()
	, pending_nb_to_b_transport_end_resp()
{
	this->bind(fw_transport_impl);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_target_socket<MODULE, BUSWIDTH, TYPES>::simple_target_socket(const char *n)
	: tlm::tlm_target_socket<BUSWIDTH, TYPES>(n)
	, fw_transport_impl(this)
	, bw_transport_impl(this)
	, pending_b_to_nb_transport_end_resp()
	, pending_b_to_nb_transport_end_req()
	, pending_nb_to_b_transport_end_resp()
{
	this->bind(fw_transport_impl);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
tlm::tlm_bw_transport_if<TYPES> *simple_target_socket<MODULE, BUSWIDTH, TYPES>::operator -> ()
{
	return &bw_transport_impl;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::register_nb_transport_fw(MODULE* mod, sync_enum_type (MODULE::*cb)(transaction_type&, phase_type&, sc_core::sc_time&))
{
	if(fw_transport_impl.mod && (fw_transport_impl.mod != mod)) throw std::runtime_error("tlm_utils::simple_target_socket: a module is already registered");
	fw_transport_impl.mod = mod;
	fw_transport_impl.nb_transport_fw_cb = cb;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::register_b_transport(MODULE* mod, void (MODULE::*cb)(transaction_type&, sc_core::sc_time&))
{
	if(fw_transport_impl.mod && (fw_transport_impl.mod != mod)) throw std::runtime_error("tlm_utils::simple_target_socket: a module is already registered");
	fw_transport_impl.mod = mod;
	fw_transport_impl.b_transport_cb = cb;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::register_transport_dbg(MODULE* mod, unsigned int (MODULE::*cb)(transaction_type&))
{
	if(fw_transport_impl.mod && (fw_transport_impl.mod != mod)) throw std::runtime_error("tlm_utils::simple_target_socket: a module is already registered");
	fw_transport_impl.mod = mod;
	fw_transport_impl.transport_dbg_cb = cb;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::register_get_direct_mem_ptr(MODULE* mod, bool (MODULE::*cb)(transaction_type&, tlm::tlm_dmi&))
{
	if(fw_transport_impl.mod && (fw_transport_impl.mod != mod)) throw std::runtime_error("tlm_utils::simple_target_socket: a module is already registered");
	fw_transport_impl.mod = mod;
	fw_transport_impl.get_direct_mem_ptr_cb = cb;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::fw_transport_impl_s(simple_target_socket<MODULE, BUSWIDTH, TYPES> *_socket)
	: socket(_socket)
	, mod(0)
	, nb_transport_fw_cb(0)
	, b_transport_cb(0)
	, transport_dbg_cb(0)
	, get_direct_mem_ptr_cb(0)
	, free_nb_to_b_transport_process_pool()
	, nb_to_b_transport_process_pool()
	, b_transport_peq(sc_core::sc_gen_unique_name("b_transport_peq"))
	, pending_payload_freed()
{
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::~fw_transport_impl_s()
{
	typename std::vector<nb_to_b_transport_process_s *>::size_type num_processes = nb_to_b_transport_process_pool.size();
	typename std::vector<nb_to_b_transport_process_s *>::size_type process_num;
	
	for(process_num = 0; process_num < num_processes; process_num++)
	{
		nb_to_b_transport_process_s *p = nb_to_b_transport_process_pool[process_num];
		
		delete p;
	}
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
tlm::tlm_sync_enum simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::nb_transport_fw(transaction_type& trans, phase_type& phase, sc_core::sc_time& t)
{
	if(nb_transport_fw_cb)
	{
		// passthrough
		return (mod->*nb_transport_fw_cb)(trans, phase, t);
	}
	
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			if(b_transport_cb)
			{
				spawn_nb_to_b_process(&trans, t);
				return tlm::TLM_ACCEPTED;
			}
			throw std::runtime_error("no b_transport callback registered");
			break;
		case tlm::END_RESP:
			{
				typename std::map<transaction_type *, sc_core::sc_event *>::iterator it = socket->pending_nb_to_b_transport_end_resp.find(&trans);
				
				if(it != socket->pending_nb_to_b_transport_end_resp.end())
				{
					sc_core::sc_event *nb_to_b_transport_end_resp_event = (*it).second;
					socket->pending_nb_to_b_transport_end_resp.erase(it);
					
					nb_to_b_transport_end_resp_event->notify(t);
					
					return tlm::TLM_COMPLETED;
				}
				throw std::runtime_error("no such payload");
			}
			break;
		default:
			// unexpected phase
			break;
	}

	throw std::runtime_error("base protocol error: unexpected phase");
	return tlm::TLM_COMPLETED;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::b_transport(transaction_type& trans, sc_core::sc_time& t)
{
	if(b_transport_cb)
	{
		(mod->*b_transport_cb)(trans, t);
	}
	else
	{
		if(!nb_transport_fw_cb) throw std::runtime_error("tlm_utils::simple_target_socket: no nb_transport_fw callback registered");

		if(trans.has_mm())
		{
			b_to_nb_transport(trans, t);
		}
		else
		{
			sc_core::sc_event payload_freed_event;
			
			trans.set_mm(this);
			trans.acquire();
			pending_payload_freed[&trans] = &payload_freed_event;
			
			b_to_nb_transport(trans, t);
			
			trans.release();
			
			if(trans.get_ref_count())
			{
				sc_core::wait(payload_freed_event);
			}
		}
	}
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::b_to_nb_transport(transaction_type& trans, sc_core::sc_time& t)
{
	sc_core::sc_event b_to_nb_transport_end_resp_event;
	
	socket->pending_b_to_nb_transport_end_resp[&trans] = &b_to_nb_transport_end_resp_event;
	b_transport_peq.notify(trans, t);
	
	sc_core::wait(b_to_nb_transport_end_resp_event);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
unsigned int simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::transport_dbg(transaction_type& trans)
{
	// passthrough
	return (mod->*transport_dbg_cb)(trans);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
bool simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::get_direct_mem_ptr(transaction_type& trans, tlm::tlm_dmi& dmi_data)
{
	// passthrough
	return (mod->*get_direct_mem_ptr_cb)(trans, dmi_data);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::b_to_nb_transport_process()
{
	while(1)
	{
		sc_core::wait(b_transport_peq.get_event());
		
		transaction_type *trans = b_transport_peq.get_next_transaction();
		
		// begin of request
		
		tlm::tlm_phase phase = tlm::BEGIN_REQ;
		sc_core::sc_time t;
		tlm::tlm_sync_enum sync = (mod->*nb_transport_fw)(*trans, phase, t);
		
		switch(sync)
		{
			case tlm::TLM_ACCEPTED:
			case tlm::TLM_UPDATED:
				if(phase == tlm::BEGIN_REQ)
				{
					sc_core::sc_event b_to_nb_transport_end_req_event;
					socket->pending_b_to_nb_transport_end_req[trans] = &b_to_nb_transport_end_req_event;
					sc_core::wait(b_to_nb_transport_end_req_event);
				}
				break;
			case tlm::TLM_COMPLETED:
				{
					typename std::map<transaction_type *, sc_core::sc_event *>::iterator it = socket->pending_b_to_nb_transport_end_resp.find(trans);
					
					if(it != socket->pending_b_to_nb_transport_end_resp.end())
					{
						sc_core::sc_event *b_to_nb_transport_end_resp_event = (*it).second;
						socket->pending_b_to_nb_transport_end_resp.erase(it);
						
						b_to_nb_transport_end_resp_event->notify(t);
						continue;
					}
					throw std::runtime_error("no such pending transaction");
				}
		}
		
		// end of request
		
	}
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::nb_to_b_transport_process(nb_to_b_transport_process_s *self)
{
	while(1)
	{
		sc_core::sc_time t;
		
		b_transport(*self->trans, t);
		
		typename tlm::tlm_phase phase = tlm::BEGIN_RESP;
		
		tlm::tlm_sync_enum sync = (socket->tlm::tlm_target_socket<BUSWIDTH, TYPES>::operator -> ())->nb_transport_bw(*self->trans, phase, t);
		
		switch(sync)
		{
			case tlm::TLM_ACCEPTED:
			case tlm::TLM_UPDATED:
				if(phase == tlm::BEGIN_RESP)
				{
					sc_core::sc_event nb_to_b_transport_end_resp_event;
					
					socket->pending_nb_to_b_transport_end_resp[self->trans] = &nb_to_b_transport_end_resp_event;
					
					sc_core::wait(nb_to_b_transport_end_resp_event);
				}
				break;
			case tlm::TLM_COMPLETED:
				break;
		}
		
		reuse_nb_to_b_process(self);
		sc_core::wait();
	}
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::spawn_nb_to_b_process(transaction_type *trans, sc_core::sc_time& t)
{
	nb_to_b_transport_process_s *p = 0;
	
	if(free_nb_to_b_transport_process_pool.empty())
	{
		p = new nb_to_b_transport_process_s(this);
		nb_to_b_transport_process_pool.push_back(p);
	}
	else
	{
		p = free_nb_to_b_transport_process_pool.top();
		free_nb_to_b_transport_process_pool.pop();
	}
	
	p->trans = trans;
	p->begin_req_event.notify(t);
}
	
template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::reuse_nb_to_b_process(typename simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::nb_to_b_transport_process_s *p)
{
	free_nb_to_b_transport_process_pool.push(p);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::nb_to_b_transport_process_s::nb_to_b_transport_process_s(fw_transport_impl_s *_owner)
	: begin_req_event(sc_core::sc_gen_unique_name("begin_req_event"))
	, trans(0)
	, process_handle()
	, owner(_owner)
{
	sc_core::sc_spawn_options spawn_options;
	spawn_options.dont_initialize();
	spawn_options.set_sensitivity(&begin_req_event);
	
	process_handle = sc_core::sc_spawn(sc_bind(&fw_transport_impl_s::nb_to_b_transport_process, owner, this), sc_core::sc_gen_unique_name("nb_to_b_transport_process"), &spawn_options);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::free(tlm::tlm_generic_payload *trans)
{
	typename std::map<transaction_type *, sc_core::sc_event *>::iterator it = pending_payload_freed.find(trans);
	
	if(it != pending_payload_freed.end())
	{
		sc_core::sc_event *payload_freed_event = (*it).second;
		pending_payload_freed.erase(it);
		
		payload_freed_event->notify();
	}
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_target_socket<MODULE, BUSWIDTH, TYPES>::bw_transport_impl_s::bw_transport_impl_s(simple_target_socket<MODULE, BUSWIDTH, TYPES> *_socket)
	: socket(_socket)
{
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
tlm::tlm_sync_enum simple_target_socket<MODULE, BUSWIDTH, TYPES>::bw_transport_impl_s::nb_transport_bw(transaction_type& trans, phase_type& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::END_REQ:
			{
				typename std::map<transaction_type *, sc_core::sc_event *>::iterator it = socket->pending_b_to_nb_transport_end_req.find(&trans);
				
				if(it != socket->pending_b_to_nb_transport_end_req.end())
				{
					sc_core::sc_event *b_to_nb_transport_end_req_event = (*it).second;
					socket->pending_b_to_nb_transport_end_req.erase(it);
					b_to_nb_transport_end_req_event->notify(t);
					
					return tlm::TLM_ACCEPTED;
				}
			}
			break;
		case tlm::BEGIN_RESP:
			{
				typename std::map<transaction_type *, sc_core::sc_event *>::iterator it = socket->pending_b_to_nb_transport_end_resp.find(&trans);
				
				if(it != socket->pending_b_to_nb_transport_end_resp.end())
				{
					sc_core::sc_event *b_to_nb_transport_end_resp_event = (*it).second;
					socket->pending_b_to_nb_transport_end_resp.erase(it);
					
					b_to_nb_transport_end_resp_event->notify(t);
					
					return tlm::TLM_COMPLETED;
				}
			}
			break;
		case tlm::END_RESP:
			{
				typename std::map<transaction_type *, sc_core::sc_event *>::iterator it = socket->pending_nb_to_b_transport_end_resp.find(&trans);
				
				if(it != socket->pending_nb_to_b_transport_end_resp.end())
				{
					socket->pending_nb_to_b_transport_end_resp.erase(it);
					return tlm::TLM_COMPLETED;
				}
			}
			break;
		default:
			// passthrough
			break;
	}
	
	return (socket->tlm::tlm_target_socket<BUSWIDTH, TYPES>::operator -> ())->nb_transport_bw(trans, phase, t);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket<MODULE, BUSWIDTH, TYPES>::bw_transport_impl_s::invalidate_direct_mem_ptr(sc_dt::uint64 start_addr, sc_dt::uint64 end_addr)
{
	// passthrough
	return (socket->tlm::tlm_target_socket<BUSWIDTH, TYPES>::operator -> ())->invalidate_direct_mem_ptr(start_addr, end_addr);
}

////////////////////////////////////// simple_target_socket_tagged<> /////////////////////////////////////////////

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::simple_target_socket_tagged()
	: tlm::tlm_target_socket<BUSWIDTH, TYPES>(sc_core::sc_gen_unique_name("simple_target_socket_tagged"))
	, fw_transport_impl(this)
	, bw_transport_impl(this)
	, pending_b_to_nb_transport_end_resp()
	, pending_b_to_nb_transport_end_req()
	, pending_nb_to_b_transport_end_resp()
{
	this->bind(fw_transport_impl);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::simple_target_socket_tagged(const char *n)
	: tlm::tlm_target_socket<BUSWIDTH, TYPES>(n)
	, fw_transport_impl(this)
	, bw_transport_impl(this)
	, pending_b_to_nb_transport_end_resp()
	, pending_b_to_nb_transport_end_req()
	, pending_nb_to_b_transport_end_resp()
{
	this->bind(fw_transport_impl);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
tlm::tlm_bw_transport_if<TYPES> *simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::operator -> ()
{
	return &bw_transport_impl;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::register_nb_transport_fw(MODULE* mod, sync_enum_type (MODULE::*cb)(int, transaction_type&, phase_type&, sc_core::sc_time&), int id)
{
	if(fw_transport_impl.mod && (fw_transport_impl.mod != mod)) throw std::runtime_error("tlm_utils::simple_target_socket_tagged: a module is already registered");
	fw_transport_impl.mod = mod;
	fw_transport_impl.nb_transport_fw_cb = cb;
	fw_transport_impl.nb_transport_fw_id = id;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::register_b_transport(MODULE* mod, void (MODULE::*cb)(int, transaction_type&, sc_core::sc_time&), int id)
{
	if(fw_transport_impl.mod && (fw_transport_impl.mod != mod)) throw std::runtime_error("tlm_utils::simple_target_socket_tagged: a module is already registered");
	fw_transport_impl.mod = mod;
	fw_transport_impl.b_transport_cb = cb;
	fw_transport_impl.b_transport_id = id;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::register_transport_dbg(MODULE* mod, unsigned int (MODULE::*cb)(int, transaction_type&), int id)
{
	if(fw_transport_impl.mod && (fw_transport_impl.mod != mod)) throw std::runtime_error("tlm_utils::simple_target_socket_tagged: a module is already registered");
	fw_transport_impl.mod = mod;
	fw_transport_impl.transport_dbg_cb = cb;
	fw_transport_impl.transport_dbg_id = id;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::register_get_direct_mem_ptr(MODULE* mod, bool (MODULE::*cb)(int, transaction_type&, tlm::tlm_dmi&), int id)
{
	if(fw_transport_impl.mod && (fw_transport_impl.mod != mod)) throw std::runtime_error("tlm_utils::simple_target_socket_tagged: a module is already registered");
	fw_transport_impl.mod = mod;
	fw_transport_impl.get_direct_mem_ptr_cb = cb;
	fw_transport_impl.get_direct_mem_ptr_id = id;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::fw_transport_impl_s(simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES> *_socket)
	: socket(_socket)
	, mod(0)
	, nb_transport_fw_cb(0)
	, nb_transport_fw_id(0)
	, b_transport_cb(0)
	, b_transport_id(0)
	, transport_dbg_cb(0)
	, transport_dbg_id(0)
	, get_direct_mem_ptr_cb(0)
	, get_direct_mem_ptr_id(0)
	, free_nb_to_b_transport_process_pool()
	, nb_to_b_transport_process_pool()
	, b_transport_peq(sc_core::sc_gen_unique_name("b_transport_peq"))
	, pending_payload_freed()
{
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::~fw_transport_impl_s()
{
	typename std::vector<nb_to_b_transport_process_s *>::size_type num_processes = nb_to_b_transport_process_pool.size();
	typename std::vector<nb_to_b_transport_process_s *>::size_type process_num;
	
	for(process_num = 0; process_num < num_processes; process_num++)
	{
		nb_to_b_transport_process_s *p = nb_to_b_transport_process_pool[process_num];
		
		delete p;
	}
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
tlm::tlm_sync_enum simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::nb_transport_fw(transaction_type& trans, phase_type& phase, sc_core::sc_time& t)
{
	if(nb_transport_fw_cb)
	{
		// passthrough
		return (mod->*nb_transport_fw_cb)(nb_transport_fw_id, trans, phase, t);
	}
	
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			if(b_transport_cb)
			{
				spawn_nb_to_b_process(&trans, t);
				return tlm::TLM_ACCEPTED;
			}
			throw std::runtime_error("no b_transport callback registered");
			break;
		case tlm::END_RESP:
			{
				typename std::map<transaction_type *, sc_core::sc_event *>::iterator it = socket->pending_nb_to_b_transport_end_resp.find(&trans);
				
				if(it != socket->pending_nb_to_b_transport_end_resp.end())
				{
					sc_core::sc_event *nb_to_b_transport_end_resp_event = (*it).second;
					socket->pending_nb_to_b_transport_end_resp.erase(it);
					
					nb_to_b_transport_end_resp_event->notify(t);
					
					return tlm::TLM_COMPLETED;
				}
				throw std::runtime_error("no such payload");
			}
			break;
		default:
			// unexpected phase
			break;
	}

	throw std::runtime_error("base protocol error: unexpected phase");
	return tlm::TLM_COMPLETED;
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::b_transport(transaction_type& trans, sc_core::sc_time& t)
{
	if(b_transport_cb)
	{
		(mod->*b_transport_cb)(b_transport_id, trans, t);
	}
	else
	{
		if(!nb_transport_fw_cb) throw std::runtime_error("tlm_utils::simple_target_socket_tagged: no nb_transport_fw callback registered");

		if(trans.has_mm())
		{
			b_to_nb_transport(trans, t);
		}
		else
		{
			sc_core::sc_event payload_freed_event;
			
			trans.set_mm(this);
			trans.acquire();
			pending_payload_freed[&trans] = &payload_freed_event;
			
			b_to_nb_transport(trans, t);
			
			trans.release();
			
			if(trans.get_ref_count())
			{
				sc_core::wait(payload_freed_event);
			}
		}
	}
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::b_to_nb_transport(transaction_type& trans, sc_core::sc_time& t)
{
	sc_core::sc_event b_to_nb_transport_end_resp_event;
	
	socket->pending_b_to_nb_transport_end_resp[&trans] = &b_to_nb_transport_end_resp_event;
	b_transport_peq.notify(trans, t);
	
	sc_core::wait(b_to_nb_transport_end_resp_event);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
unsigned int simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::transport_dbg(transaction_type& trans)
{
	// passthrough
	return (mod->*transport_dbg_cb)(transport_dbg_id, trans);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
bool simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::get_direct_mem_ptr(transaction_type& trans, tlm::tlm_dmi& dmi_data)
{
	// passthrough
	return (mod->*get_direct_mem_ptr_cb)(get_direct_mem_ptr_id, trans, dmi_data);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::b_to_nb_transport_process()
{
	while(1)
	{
		sc_core::wait(b_transport_peq.get_event());
		
		transaction_type *trans = b_transport_peq.get_next_transaction();
		
		// begin of request
		
		tlm::tlm_phase phase = tlm::BEGIN_REQ;
		sc_core::sc_time t;
		tlm::tlm_sync_enum sync = (mod->*nb_transport_fw)(*trans, phase, t);
		
		switch(sync)
		{
			case tlm::TLM_ACCEPTED:
			case tlm::TLM_UPDATED:
				if(phase == tlm::BEGIN_REQ)
				{
					sc_core::sc_event b_to_nb_transport_end_req_event;
					socket->pending_b_to_nb_transport_end_req[trans] = &b_to_nb_transport_end_req_event;
					sc_core::wait(b_to_nb_transport_end_req_event);
				}
				break;
			case tlm::TLM_COMPLETED:
				{
					typename std::map<transaction_type *, sc_core::sc_event *>::iterator it = socket->pending_b_to_nb_transport_end_resp.find(trans);
					
					if(it != socket->pending_b_to_nb_transport_end_resp.end())
					{
						sc_core::sc_event *b_to_nb_transport_end_resp_event = (*it).second;
						socket->pending_b_to_nb_transport_end_resp.erase(it);
						
						b_to_nb_transport_end_resp_event->notify(t);
						continue;
					}
					throw std::runtime_error("no such pending transaction");
				}
		}
		
		// end of request
		
	}
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::nb_to_b_transport_process(nb_to_b_transport_process_s *self)
{
	while(1)
	{
		sc_core::sc_time t;
		
		b_transport(*self->trans, t);
		
		typename tlm::tlm_phase phase = tlm::BEGIN_RESP;
		
		tlm::tlm_sync_enum sync = (socket->tlm::tlm_target_socket<BUSWIDTH, TYPES>::operator -> ())->nb_transport_bw(*self->trans, phase, t);
		
		switch(sync)
		{
			case tlm::TLM_ACCEPTED:
			case tlm::TLM_UPDATED:
				if(phase == tlm::BEGIN_RESP)
				{
					sc_core::sc_event nb_to_b_transport_end_resp_event;
					
					socket->pending_nb_to_b_transport_end_resp[self->trans] = &nb_to_b_transport_end_resp_event;
					
					sc_core::wait(nb_to_b_transport_end_resp_event);
				}
				break;
			case tlm::TLM_COMPLETED:
				break;
		}
		
		reuse_nb_to_b_process(self);
		sc_core::wait();
	}
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::spawn_nb_to_b_process(transaction_type *trans, sc_core::sc_time& t)
{
	nb_to_b_transport_process_s *p = 0;
	
	if(free_nb_to_b_transport_process_pool.empty())
	{
		p = new nb_to_b_transport_process_s(this);
		nb_to_b_transport_process_pool.push_back(p);
	}
	else
	{
		p = free_nb_to_b_transport_process_pool.top();
		free_nb_to_b_transport_process_pool.pop();
	}
	
	p->trans = trans;
	p->begin_req_event.notify(t);
}
	
template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::reuse_nb_to_b_process(typename simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::nb_to_b_transport_process_s *p)
{
	free_nb_to_b_transport_process_pool.push(p);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::nb_to_b_transport_process_s::nb_to_b_transport_process_s(fw_transport_impl_s *_owner)
	: begin_req_event(sc_core::sc_gen_unique_name("begin_req_event"))
	, trans(0)
	, process_handle()
	, owner(_owner)
{
	sc_core::sc_spawn_options spawn_options;
	spawn_options.dont_initialize();
	spawn_options.set_sensitivity(&begin_req_event);
	
	process_handle = sc_core::sc_spawn(sc_bind(&fw_transport_impl_s::nb_to_b_transport_process, owner, this), sc_core::sc_gen_unique_name("nb_to_b_transport_process"), &spawn_options);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::fw_transport_impl_s::free(tlm::tlm_generic_payload *trans)
{
	typename std::map<transaction_type *, sc_core::sc_event *>::iterator it = pending_payload_freed.find(trans);
	
	if(it != pending_payload_freed.end())
	{
		sc_core::sc_event *payload_freed_event = (*it).second;
		pending_payload_freed.erase(it);
		
		payload_freed_event->notify();
	}
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::bw_transport_impl_s::bw_transport_impl_s(simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES> *_socket)
	: socket(_socket)
{
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
tlm::tlm_sync_enum simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::bw_transport_impl_s::nb_transport_bw(transaction_type& trans, phase_type& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::END_REQ:
			{
				typename std::map<transaction_type *, sc_core::sc_event *>::iterator it = socket->pending_b_to_nb_transport_end_req.find(&trans);
				
				if(it != socket->pending_b_to_nb_transport_end_req.end())
				{
					sc_core::sc_event *b_to_nb_transport_end_req_event = (*it).second;
					socket->pending_b_to_nb_transport_end_req.erase(it);
					b_to_nb_transport_end_req_event->notify(t);
					
					return tlm::TLM_ACCEPTED;
				}
			}
			break;
		case tlm::BEGIN_RESP:
			{
				typename std::map<transaction_type *, sc_core::sc_event *>::iterator it = socket->pending_b_to_nb_transport_end_resp.find(&trans);
				
				if(it != socket->pending_b_to_nb_transport_end_resp.end())
				{
					sc_core::sc_event *b_to_nb_transport_end_resp_event = (*it).second;
					socket->pending_b_to_nb_transport_end_resp.erase(it);
					
					b_to_nb_transport_end_resp_event->notify(t);
					
					return tlm::TLM_COMPLETED;
				}
			}
			break;
		case tlm::END_RESP:
			{
				typename std::map<transaction_type *, sc_core::sc_event *>::iterator it = socket->pending_nb_to_b_transport_end_resp.find(&trans);
				
				if(it != socket->pending_nb_to_b_transport_end_resp.end())
				{
					socket->pending_nb_to_b_transport_end_resp.erase(it);
					return tlm::TLM_COMPLETED;
				}
			}
			break;
		default:
			// passthrough
			break;
	}
	
	return (socket->tlm::tlm_target_socket<BUSWIDTH, TYPES>::operator -> ())->nb_transport_bw(trans, phase, t);
}

template <typename MODULE, unsigned int BUSWIDTH, typename TYPES>
void simple_target_socket_tagged<MODULE, BUSWIDTH, TYPES>::bw_transport_impl_s::invalidate_direct_mem_ptr(sc_dt::uint64 start_addr, sc_dt::uint64 end_addr)
{
	// passthrough
	return (socket->tlm::tlm_target_socket<BUSWIDTH, TYPES>::operator -> ())->invalidate_direct_mem_ptr(start_addr, end_addr);
}

} // end of namespace tlm_utils

#endif // __LIBIEEE1666_TLM2_UTIL_SIMPLE_INITIATOR_SOCKET_H__
