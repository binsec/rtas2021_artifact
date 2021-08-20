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
 
#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_ROUTER_DISPATCHER_HH__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_ROUTER_DISPATCHER_HH__

#include <systemc>
#include <tlm>
#include <tlm_utils/peq_with_cb_and_phase.h>
#include <map>
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace generic_router {

template<typename OWNER, class CONFIG>
class RouterDispatcher :
	public sc_core::sc_module {
private:
	static const bool threaded_model = false;
	
	typedef unisim::kernel::Object Object;
	static const unsigned int MAX_NUM_MAPPINGS = CONFIG::MAX_NUM_MAPPINGS; 
//	static const unsigned int BUSWIDTH = CONFIG::BUSWIDTH;
	typedef typename CONFIG::TYPES TYPES;
	static const bool VERBOSE = CONFIG::VERBOSE;
	typedef typename TYPES::tlm_payload_type transaction_type;
	typedef typename TYPES::tlm_phase_type   phase_type;
	typedef tlm::tlm_sync_enum               sync_enum_type;
    typedef void (OWNER::*cb_t)(unsigned int, transaction_type &);
	typedef std::pair<const sc_core::sc_time, transaction_type *> pair_t;

public:
	SC_HAS_PROCESS(RouterDispatcher);
	RouterDispatcher(const sc_core::sc_module_name &name, unsigned int id, OWNER *owner, cb_t cb);
	~RouterDispatcher();

	void SetCycleTime(const sc_core::sc_time &cycle_time);
	void Push(transaction_type &trans, const sc_core::sc_time &time);
	void Completed(const transaction_type *trans, const sc_core::sc_time &time);
	inline unsigned int ReadTransportDbg(unsigned int input_port, transaction_type &trans) const;
	inline unsigned int WriteTransportDbg(unsigned int input_port, transaction_type &trans);

private:
	OWNER *m_owner;
	cb_t m_cb;
	unsigned int m_id;
	sc_core::sc_time m_cycle_time;
//	tlm_utils::peq_with_cb_and_phase<RouterDispatcher<OWNER, CONFIG>, TYPES> m_queue;
	std::multimap<sc_core::sc_time, transaction_type *> m_queue;
	sc_core::sc_event m_event, m_complete_event;
	
	int state;
	
	void Run();
	void QueueCB(transaction_type &trans, const phase_type &phase);
	bool ProcessTransaction();
	bool ProcessTransactionCompletion();
};

} // end of namespace generic_router
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_ROUTER_DISPATCHER_HH__

