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

#ifndef __TLM_UTILS_PEQ_WITH_GET_H__
#define __TLM_UTILS_PEQ_WITH_GET_H__

#include <systemc>

namespace tlm_utils {

template <class PAYLOAD>
class peq_with_get : public sc_core::sc_object
{
public:
	typedef PAYLOAD transaction_type;

	peq_with_get(const char* name);
	void notify(transaction_type& trans, const sc_core::sc_time& t);
	void notify(transaction_type& trans);
	transaction_type* get_next_transaction();
	sc_core::sc_event& get_event();
	void cancel_all();
private:
	std::multimap<sc_core::sc_time, transaction_type *> scheduled_transactions;
	sc_core::sc_event next_scheduled_transaction_event;
};

template <class PAYLOAD>
peq_with_get<PAYLOAD>::peq_with_get(const char* name)
	: sc_core::sc_object(name)
{
}

template <class PAYLOAD>
void peq_with_get<PAYLOAD>::notify(transaction_type& trans, const sc_core::sc_time& t)
{
	sc_core::sc_time absolute_trans_time_stamp = sc_core::sc_time_stamp();
	absolute_trans_time_stamp += t;
	scheduled_transactions.insert(std::pair<sc_core::sc_time, transaction_type *>(absolute_trans_time_stamp, &trans));
	next_scheduled_transaction_event.notify(t);
}

template <class PAYLOAD>
void peq_with_get<PAYLOAD>::notify(transaction_type& trans)
{
	scheduled_transactions.insert(std::pair<sc_core::sc_time, transaction_type *>(sc_core::sc_time_stamp(), &trans));
	next_scheduled_transaction_event.notify(sc_core::SC_ZERO_TIME);
}

template <class PAYLOAD>
PAYLOAD* peq_with_get<PAYLOAD>::get_next_transaction()
{
	if(scheduled_transactions.empty()) return 0;
	
	sc_core::sc_time current_time_stamp = sc_core::sc_time_stamp();
	
	typename std::multimap<sc_core::sc_time, transaction_type *>::iterator it = scheduled_transactions.begin();
	
	const sc_core::sc_time& scheduled_transaction_time_stamp = it->first;
	
	if(scheduled_transaction_time_stamp > current_time_stamp)
	{
		next_scheduled_transaction_event.notify(scheduled_transaction_time_stamp - current_time_stamp);
		return 0;
	}

	transaction_type *trans = it->second;
	scheduled_transactions.erase(it);
	return trans;
}

template <class PAYLOAD>
sc_core::sc_event& peq_with_get<PAYLOAD>::get_event()
{
	return next_scheduled_transaction_event;
}

template <class PAYLOAD>
void peq_with_get<PAYLOAD>::cancel_all()
{
	scheduled_transactions.clear();
	next_scheduled_transaction_event.cancel();
}

} // end of namespace tlm_utils

#endif // __TLM_UTILS_PEQ_WITH_GET_H__
