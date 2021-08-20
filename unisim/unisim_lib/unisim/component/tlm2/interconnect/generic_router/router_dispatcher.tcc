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
 
#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_ROUTER_DISPATCHER_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_ROUTER_DISPATCHER_TCC__

#include <string.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace generic_router {

template<typename OWNER, class CONFIG>
RouterDispatcher<OWNER, CONFIG>::
RouterDispatcher(const sc_core::sc_module_name &name, unsigned int id, OWNER *owner, cb_t cb) :
sc_core::sc_module(name),
m_owner(owner),
m_cb(cb),
m_id(id),
m_cycle_time(sc_core::SC_ZERO_TIME),
m_queue(),
m_event("m_event"),
m_complete_event("m_complete_event"),
state(0)
// m_queue(this, &RouterDispatcher<OWNER, CONFIG>::QueueCB)
{
	SC_HAS_PROCESS(RouterDispatcher);
	
	if(threaded_model)
	{
		SC_THREAD(Run);
	}
	else
	{
		SC_METHOD(Run);
	}
}

template<typename OWNER, class CONFIG>
RouterDispatcher<OWNER, CONFIG>::
~RouterDispatcher() 
{
}

template<typename OWNER, class CONFIG>
void
RouterDispatcher<OWNER, CONFIG> ::
SetCycleTime(const sc_core::sc_time &cycle_time)
{
	m_cycle_time = cycle_time;
}

template<typename OWNER, class CONFIG>
void
RouterDispatcher<OWNER, CONFIG> ::
Push(transaction_type &trans, const sc_core::sc_time &time) 
{
	if(trans.has_mm()) trans.acquire();
	sc_core::sc_time time_stamp(sc_core::sc_time_stamp());
	time_stamp += time;
	m_queue.insert(pair_t(time_stamp, &trans));
	m_event.notify(time);
	//phase_type phase = tlm::BEGIN_REQ;
	//m_queue.notify(trans, phase, time);
}

template<typename OWNER, class CONFIG>
void
RouterDispatcher<OWNER, CONFIG>::
Completed(const transaction_type *trans, const sc_core::sc_time &time) 
{
	typename std::multimap<const sc_core::sc_time, transaction_type *>::iterator it;

	it = m_queue.begin();
	if (it == m_queue.end()) return;

	if ( trans == it->second ) m_complete_event.notify(time);
}

template<typename OWNER, class CONFIG>
void
RouterDispatcher<OWNER, CONFIG>::
QueueCB(transaction_type &trans, const phase_type &phase) 
{
	(m_owner->*m_cb)(m_id, trans);
}

template<typename OWNER, class CONFIG>
bool
RouterDispatcher<OWNER, CONFIG>::
ProcessTransaction() 
{
	typename std::multimap<const sc_core::sc_time, transaction_type *>::iterator it;

	sc_core::sc_time now = sc_core::sc_time_stamp();
	transaction_type *trans;

	it = m_queue.begin();
	if (it == m_queue.end()) return false;
	if (it->first > now) {
		m_event.notify(it->first - now);
		return false;
	}
	trans = it->second;
//	m_queue.erase(it);
	(m_owner->*m_cb)(m_id, *trans);
	return true;
}

template<typename OWNER, class CONFIG>
bool
RouterDispatcher<OWNER, CONFIG>::
ProcessTransactionCompletion() 
{
	typename std::multimap<const sc_core::sc_time, transaction_type *>::iterator it;

	sc_core::sc_time now(sc_core::sc_time_stamp());
	transaction_type *trans;

	it = m_queue.begin();
	assert(it != m_queue.end());
	trans = it->second;
	m_queue.erase(it);
	if(trans->has_mm()) trans->release();
	it = m_queue.begin();
	if (it == m_queue.end()) return true;
	if (it->first > now) {
		m_event.notify(it->first - now);
		return true;
	}
	// we need to synchronize
	sc_core::sc_time fut(now);
	unisim::kernel::tlm2::AlignToClock(fut, m_cycle_time);
	fut += m_cycle_time;
	fut -= now;
	m_event.notify(fut);
#if 0
	uint64_t val = now.value() / m_cycle_time.value();
	val = val + 1;
	sc_core::sc_time fut = m_cycle_time * val;
	fut += m_cycle_time;
	m_event.notify(fut - now);
#endif
	return true;
}

template<typename OWNER, class CONFIG>
void
RouterDispatcher<OWNER, CONFIG>::
Run() 
{
	if(threaded_model)
	{
		while(1)
		{
			switch(state)
			{
				case 0:
					wait(m_event);
					if(ProcessTransaction()) state = 1;
					break;
				case 1:
					wait(m_complete_event);
					if(ProcessTransactionCompletion()) state = 0;
					break;
			}
		}
	}
	else
	{
		switch(state)
		{
			case 0:
				if(ProcessTransaction())
				{
					state = 1;
					next_trigger(m_complete_event);
				}
				else
				{
					next_trigger(m_event);
				}
				break;
			case 1:
				if(ProcessTransactionCompletion())
				{
					state = 0;
					next_trigger(m_event);
				}
				else
				{
					next_trigger(m_complete_event);
				}
				break;
		}
	}
}
	
template<typename OWNER, class CONFIG>
unsigned int
RouterDispatcher<OWNER, CONFIG>::
ReadTransportDbg(unsigned int input_port, transaction_type &trans) const
{
	// check all the write transactions in the queue, starting by the oldest ones
	typename std::multimap<const sc_core::sc_time, transaction_type *>::const_iterator it;
	// min_base and max_top keep the number of bytes modified, found is used to know if any match happened
	bool found = false;
	sc_dt::uint64 min_base = 0;
	sc_dt::uint64 max_top = 0;
	for (it = m_queue.begin(); it != m_queue.end(); it++) {
		sc_dt::uint64 base = 0;
		sc_dt::uint64 top = 0;
		sc_dt::uint64 size = 0;
		if (!it->second->is_write()) continue;
		if (it->second->get_address() >= trans.get_address() + trans.get_data_length() ||
				trans.get_address() >= it->second->get_address() + it->second->get_data_length()) 
			continue;

		// get the base address
		if (trans.get_address() < it->second->get_address()) base = it->second->get_address();
		else base = trans.get_address();

		// get the top address
		if (trans.get_address() + trans.get_data_length() > it->second->get_address() + it->second->get_data_length()) top = it->second->get_address() + it->second->get_data_length();
		else top = trans.get_address() + trans.get_data_length();
		
		// compute the amount of data that can be read
		size = top - base;

		// copy the available data into the transaction
		unsigned char *trans_buffer = trans.get_data_ptr() + (top - trans.get_address());
		unsigned char *cur_buffer = it->second->get_data_ptr() + (top - it->second->get_address());
		memcpy(trans_buffer, cur_buffer, size);

		// update min_base and max_top
		if (!found) {
			min_base = base;
			max_top = top;
		} else {
			if (base < min_base) min_base = base;
			if (top > max_top) max_top = top;
		}
	}
	return max_top - min_base;
}

template<typename OWNER, class CONFIG>
unsigned int
RouterDispatcher<OWNER, CONFIG>::
WriteTransportDbg(unsigned int input_port, transaction_type &trans)
{
	// modify all the transactions
	typename std::multimap<const sc_core::sc_time, transaction_type *>::iterator it;
	// min_base and max_top keep the number of bytes modified, found is used to know if any match happened
	bool found = false;
	sc_dt::uint64 min_base = 0;
	sc_dt::uint64 max_top = 0;
	for (it = m_queue.begin(); it != m_queue.end(); it++) {
		sc_dt::uint64 base = 0;
		sc_dt::uint64 top = 0;
		sc_dt::uint64 size = 0;
		if (!it->second->is_write() || !it->second->is_read()) continue;
		if (it->second->get_address() >= trans.get_address() + trans.get_data_length() ||
				trans.get_address() >= it->second->get_address() + it->second->get_data_length()) 
			continue;

		// get the base address
		if (trans.get_address() < it->second->get_address()) base = it->second->get_address();
		else base = trans.get_address();
		
		// get the top address
		if (trans.get_address() + trans.get_data_length() > it->second->get_address() + it->second->get_data_length()) top = it->second->get_address() + it->second->get_data_length();
		else top = trans.get_address() + trans.get_data_length();
		
		// compute the amount of data that can be modified
		size = top - base;

		// copy the available data into the transaction
		unsigned char *trans_buffer = trans.get_data_ptr() + (top - trans.get_address());
		unsigned char *cur_buffer = it->second->get_data_ptr() + (top - it->second->get_address());
		memcpy(cur_buffer, trans_buffer, size);
		
		// update min_base and max_top
		if (!found) {
			min_base = base;
			max_top = top;
		} else {
			if (base < min_base) min_base = base;
			if (top > max_top) max_top = top;
		}
	}
	return max_top - min_base;
}

} // end of namespace generic_router
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_ROUTER_DISPATCHER_TCC__

