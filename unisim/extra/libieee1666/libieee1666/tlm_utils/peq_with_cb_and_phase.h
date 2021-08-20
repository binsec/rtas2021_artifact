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

#ifndef __TLM_UTILS_PEQ_WITH_CB_AND_PHASE_H__
#define __TLM_UTILS_PEQ_WITH_CB_AND_PHASE_H__

#include <systemc>

namespace tlm_utils {

template<typename OWNER, typename TYPES=tlm::tlm_base_protocol_types>
class peq_with_cb_and_phase : public sc_core::sc_object
{
public:
	typedef typename TYPES::tlm_payload_type tlm_payload_type;
	typedef typename TYPES::tlm_phase_type tlm_phase_type;
	typedef void (OWNER::*cb)(tlm_payload_type&, const tlm_phase_type&);

	peq_with_cb_and_phase(OWNER *, cb);
	peq_with_cb_and_phase(const char *, OWNER *, cb);
	~peq_with_cb_and_phase();
	void notify(tlm_payload_type&, const tlm_phase_type&, const sc_core::sc_time&);
	void notify(tlm_payload_type&, const tlm_phase_type&);
	void cancel_all();
private:
	struct transaction_and_phase
	{
		tlm_payload_type *trans;
		const tlm_phase_type phase;
	};
	
	void initialize();
	void caller_process();
	transaction_and_phase *allocate_transaction_and_phase();
	void reuse_transaction_and_phase(transaction_and_phase *element);
	
	OWNER *owner;
	cb *callee;
	std::multimap<sc_core::sc_time, transaction_and_phase> scheduled_transactions_and_phases;
	std::stack<transaction_and_phase *> transaction_and_phase_free_list;
	sc_core::sc_event next_scheduled_transaction_and_phase_event;
};

template<typename OWNER, typename TYPES>
peq_with_cb_and_phase<OWNER, TYPES>::peq_with_cb_and_phase(OWNER *_owner, cb _cb)
	: sc_core::sc_object(sc_core::sc_gen_unique_name("peq_with_cb_and_phase"))
	, owner(_owner)
	, callee(_cb)
	, scheduled_transactions_and_phases()
	, transaction_and_phase_free_list()
	, next_scheduled_transaction_and_phase_event("next_scheduled_transaction_and_phase_event")
{
	initialize();
}

template<typename OWNER, typename TYPES>
peq_with_cb_and_phase<OWNER, TYPES>::peq_with_cb_and_phase(const char *name, OWNER *_owner, cb _cb)
	: sc_core::sc_object(name)
	, owner(_owner)
	, callee(_cb)
	, scheduled_transactions_and_phases()
	, transaction_and_phase_free_list()
	, next_scheduled_transaction_and_phase_event("next_scheduled_transaction_and_phase_event")
{
	initialize();
}

template<typename OWNER, typename TYPES>
void peq_with_cb_and_phase<OWNER, TYPES>::initialize()
{
	sc_core::sc_spawn_options spawn_options;
	
	spawn_options.set_sensitivity(&next_scheduled_transaction_and_phase_event);
	spawn_options.dont_initialize();
	spawn_options.spawn_method();
	
	sc_core::sc_spawn(sc_bind(&caller_process, this), &spawn_options);
}

template<typename OWNER, typename TYPES>
peq_with_cb_and_phase<OWNER, TYPES>::~peq_with_cb_and_phase()
{
	typename std::multimap<sc_core::sc_time, transaction_and_phase *>::iterator it;
	
	for(it = scheduled_transactions_and_phases.begin(); it != scheduled_transactions_and_phases.end(); it++)
	{
		transaction_and_phase *element = it->second;
		delete element;
	}
	
	while(!transaction_and_phase_free_list.empty())
	{
		transaction_and_phase *element = transaction_and_phase_free_list.front();
		delete element;
		transaction_and_phase_free_list.pop();
	}
}

template<typename OWNER, typename TYPES>
void peq_with_cb_and_phase<OWNER, TYPES>::notify(tlm_payload_type& trans, const tlm_phase_type& phase, const sc_core::sc_time& t)
{
	sc_core::sc_time time_stamp = sc_core::sc_time_stamp();
	time_stamp += t;
	transaction_and_phase *element = allocate_transaction_and_phase();
	element->trans = &trans;
	element->phase = phase;
	scheduled_transactions_and_phases.insert(std::pair<sc_core::sc_time, transaction_and_phase *>(time_stamp, element));	
	next_scheduled_transaction_and_phase_event.notify(t);
}

template<typename OWNER, typename TYPES>
void peq_with_cb_and_phase<OWNER, TYPES>::notify(tlm_payload_type& trans, const tlm_phase_type& phase)
{
	sc_core::sc_time time_stamp = sc_core::sc_time_stamp();
	transaction_and_phase *element = allocate_transaction_and_phase();
	element->trans = &trans;
	element->phase = phase;
	scheduled_transactions_and_phases.insert(std::pair<sc_core::sc_time, transaction_and_phase *>(time_stamp, element));	
	next_scheduled_transaction_and_phase_event.notify(sc_core::SC_ZERO_TIME);
}

template<typename OWNER, typename TYPES>
void peq_with_cb_and_phase<OWNER, TYPES>::cancel_all()
{
	next_scheduled_transaction_and_phase_event.cancel();
	
	typename std::multimap<sc_core::sc_time, transaction_and_phase *>::iterator it;
	
	for(it = scheduled_transactions_and_phases.begin(); it != scheduled_transactions_and_phases.end(); it++)
	{
		reuse_transaction_and_phase(it->second);
	}
	
	scheduled_transactions_and_phases.clear();
}

template<typename OWNER, typename TYPES>
void peq_with_cb_and_phase<OWNER, TYPES>::caller_process()
{
	if(scheduled_transactions_and_phases.empty()) return;
	
	sc_core::sc_time current_time_stamp = sc_core::sc_time_stamp();
	
	typename std::multimap<sc_core::sc_time, transaction_and_phase *>::iterator it = scheduled_transactions_and_phases.begin();
	
	const sc_core::sc_time& transaction_and_phase_time_stamp = it->first;
	
	if(transaction_and_phase_time_stamp > current_time_stamp)
	{
		next_scheduled_transaction_and_phase_event.notify(transaction_and_phase_time_stamp - current_time_stamp);
		return;
	}

	transaction_and_phase *element = it->second;
	tlm_payload_type *trans = element->trans;
	const tlm_phase_type& phase = element->phase;
	(owner->*callee)(*trans, phase);
	reuse_transaction_and_phase(element);
	scheduled_transactions_and_phases.erase(it);
}

template<typename OWNER, typename TYPES>
typename peq_with_cb_and_phase<OWNER, TYPES>::transaction_and_phase *peq_with_cb_and_phase<OWNER, TYPES>::allocate_transaction_and_phase()
{
	if(transaction_and_phase_free_list.empty())
	{
		return new transaction_and_phase;
	}
	
	transaction_and_phase *element = transaction_and_phase_free_list.front();
	transaction_and_phase_free_list.pop();
	return element;
}

template<typename OWNER, typename TYPES>
void peq_with_cb_and_phase<OWNER, TYPES>::reuse_transaction_and_phase(transaction_and_phase *element)
{
	transaction_and_phase_free_list.push(element);
}

} // end of namespace tlm_utils

#endif // __TLM_UTILS_PEQ_WITH_CB_AND_PHASE_H__
