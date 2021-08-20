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

#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_BUS_BUS_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_BUS_BUS_TCC__

#define LOCATION 	" - location = " << __FUNCTION__ << ":unisim_lib/unisim/component/tlm2/interconnect/generic_bus/bus.tcc:" << __LINE__
#define TIME(X) 	" - time = " << sc_time_stamp() + (X)
#define PHASE(X) 	" - phase = " << 	( (X) == tlm::BEGIN_REQ  ? 	"BEGIN_REQ" : \
										( (X) == tlm::END_REQ    ? 	"END_REQ" : \
										( (X) == tlm::BEGIN_RESP ? 	"BEGIN_RESP" : \
																	"END_RESP")))
#define TRANS(X) 	" - trans = " << &(X)

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace generic_bus {

using namespace unisim::kernel::logger;

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
Bus<BUSWIDTH, TYPES, DEBUG>::
Bus(const sc_module_name& name, unisim::kernel::Object *parent) :
sc_module(name),
unisim::kernel::Object(name, parent),
targ_socket("target_socket"),
init_socket("init_socket"),
peq("peq"),
free_peq("free_peq"),
pending_transactions(),
bus_cycle_time(SC_ZERO_TIME),
bus_cycle_time_double(0.0),
param_bus_cycle_time_double("bus_cycle_time", this, bus_cycle_time_double),
logger(*this),
verbose_all(false),
param_verbose_all("verbose_all", this, verbose_all, "Activate all the verbose options"),
verbose_setup(false),
param_verbose_setup("verbose_setup", this, verbose_setup, "Display Object setup information"),
verbose_non_blocking(false),
param_verbose_non_blocking("verbose_non_blocking", this, verbose_non_blocking, "Display non_blocking transactions handling"),
verbose_blocking(false),
param_verbose_blocking("verbose_blocking", this, verbose_blocking, "Display blocking transactions handling") {
	/* register target multi socket callbacks */
 	targ_socket.register_nb_transport_fw(    this, &Bus<BUSWIDTH, TYPES, DEBUG>::T_nb_transport_fw_cb);
 	targ_socket.register_b_transport(        this, &Bus<BUSWIDTH, TYPES, DEBUG>::T_b_transport_cb);
 	targ_socket.register_transport_dbg(      this, &Bus<BUSWIDTH, TYPES, DEBUG>::T_transport_dbg_cb);
 	targ_socket.register_get_direct_mem_ptr( this, &Bus<BUSWIDTH, TYPES, DEBUG>::T_get_direct_mem_ptr_cb);

	/* register initiator socket callbacks */
	init_socket.register_nb_transport_bw(           this, &Bus<BUSWIDTH, TYPES, DEBUG>::I_nb_transport_bw_cb);
	init_socket.register_invalidate_direct_mem_ptr( this, &Bus<BUSWIDTH, TYPES, DEBUG>::I_invalidate_direct_mem_ptr_cb);

	SC_THREAD(Dispatcher);
}

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
Bus<BUSWIDTH, TYPES, DEBUG>::
~Bus() {
}

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
bool
Bus<BUSWIDTH, TYPES, DEBUG>::
Setup() {
	if(bus_cycle_time_double == 0.0) {
		logger << DebugError << "PARAMETER ERROR: the bus_cycle_time parameter  must be bigger than 0" << endl
			<< LOCATION << EndDebug;
		return false;
	}
	bus_cycle_time = sc_time(bus_cycle_time_double, SC_PS);
	if(VerboseSetup())
		logger << DebugInfo << "Setting bus_cycle_time to " << bus_cycle_time << EndDebug;

	return true;
}

/*************************************************************************
 * Multi passtrough target socket callbacks                        START *
 *************************************************************************/

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
tlm::tlm_sync_enum
Bus<BUSWIDTH, TYPES, DEBUG>::
T_nb_transport_fw_cb(int id, 
		typename TYPES::tlm_payload_type &trans, 
		typename TYPES::tlm_phase_type &phase, 
		sc_core::sc_time &time) {
	if(VerboseNonBlocking())
		logger << DebugInfo << "Received nb_transport_fw on port " << id << endl
			<< TIME(time) << endl
			<< PHASE(phase) << endl
			<< TRANS(trans) << EndDebug;
	if(phase == tlm::BEGIN_REQ) {
		trans.acquire();
		BusTlmGenericProtocol<TYPES> *item = 0;
		item = free_peq.get_next_transaction();
		if(item == 0) {
			item = new BusTlmGenericProtocol<TYPES>();
		}
		assert(trans.has_mm());
		trans.acquire();
		item->payload = &trans;
		item->from_initiator = true;
		item->id = id;
		item->send_end_req = true;
		item->send_end_resp = true;
		AddPendingTransaction(trans, item);
		peq.notify(*item, time);
		return tlm::TLM_ACCEPTED;
	} else if(phase == tlm::END_RESP) {
		end_resp_event.notify(time);
		return tlm::TLM_COMPLETED;
	} else {
		logger << DebugError << "Unexpected phase value" << endl
			<< LOCATION << endl
			<< TIME(time) << endl
			<< PHASE(phase) << endl
			<< TRANS(trans) << EndDebug;
		Object::Stop(-1);
	}

	// this should never be executed
	logger << DebugError << "Unreacheable code section reached" << endl
		<< " - time = " << sc_time_stamp() + time << endl
		<< LOCATION << EndDebug;
	Object::Stop(-1);
	return tlm::TLM_ACCEPTED; // unnecessary, but to avoid compilation errors/warnings
}

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
void
Bus<BUSWIDTH, TYPES, DEBUG>::
T_b_transport_cb(int id,
		typename TYPES::tlm_payload_type &trans,
		sc_core::sc_time &time) {
	if(VerboseBlocking())
		logger << DebugInfo << "Received b_transport on port " << id << ", forwarding it" << endl
			<< TIME(time) << endl
			<< TRANS(trans) << EndDebug;
	init_socket->b_transport(trans, time);
}

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
unsigned int
Bus<BUSWIDTH, TYPES, DEBUG>::
T_transport_dbg_cb(int id,
		typename TYPES::tlm_payload_type &trans) {
	return 0;  // Dummy implementation
}

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
bool
Bus<BUSWIDTH, TYPES, DEBUG>::
T_get_direct_mem_ptr_cb(int id,
		typename TYPES::tlm_payload_type &trans,
		tlm::tlm_dmi &dmi) {
	return false;  // Dummy implementation
}

/*************************************************************************
 * Multi passthrough target socket callbacks                         END *
 *************************************************************************/

/*************************************************************************
 * Simple initiator socket callbacks                               START *
 *************************************************************************/
	
template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
tlm::tlm_sync_enum
Bus<BUSWIDTH, TYPES, DEBUG>::
I_nb_transport_bw_cb(typename TYPES::tlm_payload_type &trans, 
		typename TYPES::tlm_phase_type &phase, 
		sc_core::sc_time &time) {
	if(VerboseNonBlocking())
		logger << DebugInfo << "Received nb_transport_bw from output port" << endl
			<< TIME(time) << endl
			<< PHASE(phase) << endl
			<< TRANS(trans) << EndDebug;

	if(phase == tlm::END_REQ) {
		// send the END_REQ to the initiator module and unlock the dispatcher
		BusTlmGenericProtocol<TYPES> *item = pending_transactions[&trans];
		item->send_end_req = true;
		item->send_end_resp = true;
		end_req_event.notify(time);
		return tlm::TLM_ACCEPTED;
	} else if(phase == tlm::BEGIN_RESP) {
		assert(pending_transactions.find(&trans) != pending_transactions.end());
		BusTlmGenericProtocol<TYPES> *item = pending_transactions[&trans];
		item->send_end_req = false;
		item->send_end_resp = true;
		item->from_initiator = false;
		peq.notify(*item, time);
		return tlm::TLM_ACCEPTED;
	} else {
		logger << DebugError << "Unexpected phase value received" << endl
			<< LOCATION << endl
			<< TIME(time) << endl
			<< PHASE(phase) << endl
			<< TRANS(trans) << EndDebug;
		Object::Stop(-1);
	}

	// this should never be executed
	logger << DebugError << "Unreacheable code section reached" << endl
		<< LOCATION << endl
		<< TIME(time) << endl
		<< PHASE(phase) << endl
		<< TRANS(trans) << EndDebug;
	Object::Stop(-1);
	return tlm::TLM_ACCEPTED; // unnecessary, but to avoid compilation errors/warnings
}

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
void
Bus<BUSWIDTH, TYPES, DEBUG>::
I_invalidate_direct_mem_ptr_cb(sc_dt::uint64 start_range, 
		sc_dt::uint64 end_range) {
	// Dummy implementation
}

/*************************************************************************
 * Simple initiator socket callbacks                                 END *
 *************************************************************************/

/*************************************************************************
 * Dispatcher methods and variables                                START *
 *************************************************************************/

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
void
Bus<BUSWIDTH, TYPES, DEBUG>::
Dispatcher() {
	sc_core::sc_event &ev = peq.get_event();
	BusTlmGenericProtocol<TYPES> *item;
	sc_time cur_time;

	while(1) {
		wait(ev);
		item = peq.get_next_transaction();
		while(item != 0) {
			BusSynchronize();
			if(VerboseNonBlocking())
				logger << DebugInfo << "Dispatching transaction from " << (item->from_initiator ? "initiator" : "target") << endl
					<< TIME(SC_ZERO_TIME) << endl
					<< TRANS(*(item->payload)) << endl
					<< " - item = " << item << EndDebug;
			if(item->from_initiator) {
				DispatchFW(item);
			} else {
				DispatchBW(item);
			}
			item = peq.get_next_transaction();
		}
	}
}

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
void
Bus<BUSWIDTH, TYPES, DEBUG>::
DispatchFW(BusTlmGenericProtocol<TYPES> *item) {
	sc_time time(SC_ZERO_TIME);
	sc_time end_req_time(SC_ZERO_TIME);
	typename TYPES::tlm_phase_type phase;

	phase = tlm::BEGIN_REQ;
	if(VerboseNonBlocking())
		logger << DebugInfo << "Sending request to target" << endl
			<< TIME(time) << endl
			<< TRANS(*(item->payload)) << endl
			<< " - item = " << item << EndDebug;
	switch(init_socket->nb_transport_fw(*(item->payload), phase, time)) {
	case tlm::TLM_ACCEPTED:
	case tlm::TLM_UPDATED:
		// transaction not yet finished
		if(phase == tlm::BEGIN_REQ) {
			// request phase not yet finished
			wait(end_req_event);
			// check if the END_REQ message needs to be sent
			if(item->send_end_req) {
				phase = tlm::END_REQ;
				end_req_time = SC_ZERO_TIME;
				targ_socket[item->id]->nb_transport_bw(*(item->payload), phase, end_req_time);
				item->send_end_req = false;
			}
		} else if (phase == tlm::END_REQ) {
			// request phase finished, but respose phase not yet started
			phase = tlm::END_REQ;
			end_req_time = time;
			targ_socket[item->id]->nb_transport_bw(*(item->payload), phase, end_req_time);
			item->send_end_req = false;
			wait(time);
		} else if(phase == tlm::BEGIN_RESP) {
			// send the END_REQ message 
			phase = tlm::END_REQ;
			end_req_time = SC_ZERO_TIME;
			targ_socket[item->id]->nb_transport_bw(*(item->payload), phase, end_req_time);
			// reuse the item to send the response to the initiator module
			item->phase = tlm::BEGIN_RESP;
			item->from_initiator = false;
			item->send_end_req = false;
			item->send_end_resp = true;
			peq.notify(*item, time);
		} else {
			logger << DebugError << "Unhandled phase value" << endl
				<< LOCATION << endl
				<< TIME(time) << endl
				<< PHASE(phase) << endl
				<< TRANS(*(item->payload)) << endl
				<< " - item = " << item << EndDebug;
			Object::Stop(-1);
		}
		break;
	case tlm::TLM_COMPLETED:
		if(VerboseNonBlocking())
			logger << DebugInfo << "Received TLM_COMPLETE to BEGIN_REQ, generating an END_REQ for the source" << endl
				<< TIME(time) << endl
				<< TRANS(*(item->payload)) << endl
				<< " - item = " << item << EndDebug;
		phase = tlm::END_REQ;
		end_req_time = SC_ZERO_TIME;
		switch(targ_socket[item->id]->nb_transport_bw(*(item->payload), phase, time)) {
		case tlm::TLM_ACCEPTED:
			if(VerboseNonBlocking())
				logger << DebugInfo << "END_REQ accepted (TLM_ACCEPTED) by the source, response can be send" << endl
					<< TIME(time) << endl
					<< TRANS(*(item->payload)) << endl
					<< " - item = " << item << EndDebug;
			// the response can be sent, reuse the item for that
			item->from_initiator = false;
			item->send_end_req = false;
			item->send_end_resp = false;
			peq.notify(*item, time);
			break;
		case tlm::TLM_UPDATED:
			logger << DebugError << "Unhandled sync value tlm::TLM_UPDATED" << endl
				<< LOCATION << endl
				<< TIME(time) << endl
				<< TRANS(*(item->payload)) << endl
				<< " - item = " << item << EndDebug;
			Object::Stop(-1);
			break;
		case tlm::TLM_COMPLETED:
			if(VerboseNonBlocking())
				logger << DebugInfo << "END_REQ accepted (TLM_COMPLETED) by the initiator, transaction finished" << endl
					<< TIME(time) << endl
					<< TRANS(*(item->payload)) << endl
					<< " - item = " << item << EndDebug;
			// the item can be removed
			item->payload->release();
			item->payload = 0;
			RemovePendingTransaction(*(item->payload));
			free_peq.notify(*item);
			break;
		}
		break;
	default:
		logger << DebugError << "Unhandled sync value received" << endl
			<< LOCATION << endl
			<< TIME(time) << endl
			<< TRANS(*(item->payload)) << endl
			<< " - item = " << item << EndDebug;
		Object::Stop(-1);
		break;
	}
}

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
void
Bus<BUSWIDTH, TYPES, DEBUG>::
DispatchBW(BusTlmGenericProtocol<TYPES> *item) {
	typename TYPES::tlm_phase_type phase;
	sc_time time(SC_ZERO_TIME);
	sc_time end_resp_time;

	phase = tlm::BEGIN_RESP;
	if(VerboseNonBlocking())
		logger << DebugInfo << "Sending BEGIN_RESP to initiator" << endl
			<< TIME(time) << endl
			<< TRANS(*(item->payload)) << endl
			<< " - item = " << item << EndDebug;
	switch(targ_socket[item->id]->nb_transport_bw(*(item->payload), phase, time)) {
	case tlm::TLM_ACCEPTED:
	case tlm::TLM_UPDATED:
		// the bus must wait for the response (END_RESP) from the initiator to the target
		wait(end_resp_event);
		if(item->send_end_resp) {
			phase = tlm::END_RESP;
			end_resp_time = SC_ZERO_TIME;
			init_socket->nb_transport_fw(*(item->payload), phase, end_resp_time);
			// the transaction can be released and removed from the lookup table
			RemovePendingTransaction(*(item->payload));
			item->payload->release();
			item->payload = 0;
			free_peq.notify(*item);
		}
		break;
	case tlm::TLM_COMPLETED:
		// a response (END_RESP) must be sent to the target
		phase = tlm::END_RESP;
		end_resp_time = time;
		init_socket->nb_transport_fw(*(item->payload), phase, end_resp_time);
		// the transaction can be released and removed from the lookup table
		RemovePendingTransaction(*(item->payload));
		item->payload->release();
		item->payload = 0;
		free_peq.notify(*item);
		break;
	}
}

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
void
Bus<BUSWIDTH, TYPES, DEBUG>::
BusSynchronize() {
	sc_time cur_time = sc_time_stamp();
	sc_dt::uint64 cur_time_int = cur_time.value();
	sc_dt::uint64 bus_cycle_time_int = bus_cycle_time.value();
	sc_dt::uint64 diff_int = bus_cycle_time_int - (cur_time_int % bus_cycle_time_int);
	sc_time diff = sc_time(diff_int, false);
	wait(diff);
}

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
void 
Bus<BUSWIDTH, TYPES, DEBUG>::
PrintPendingTransactions() {
	typename std::map<transaction_type *, BusTlmGenericProtocol<TYPES> *>::iterator it;

	for(it = pending_transactions.begin(); it != pending_transactions.end(); it++) {
		cerr << "\t" << it->first << "\t" << it->second << endl;
	}

}

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
void
Bus<BUSWIDTH, TYPES, DEBUG>::
AddPendingTransaction(typename TYPES::tlm_payload_type &trans, BusTlmGenericProtocol<TYPES> *item) {
	assert(pending_transactions.find(&trans) == pending_transactions.end());
	// cerr << GetName() << ": adding pending transaction " << &trans << endl;
	pending_transactions[&trans] = item;
	// PrintPendingTransactions();
}

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
void
Bus<BUSWIDTH, TYPES, DEBUG>::
RemovePendingTransaction(typename TYPES::tlm_payload_type &trans) {
	typename std::map<transaction_type *, BusTlmGenericProtocol<TYPES> *>::iterator it;
	// cerr << GetName() << ": removing pending transaction " << &trans << endl;
	// PrintPendingTransactions();
	it = pending_transactions.find(&trans);
	assert(it != pending_transactions.end());
	pending_transactions.erase(it);
}

/*************************************************************************
 * Dispatcher methods and variables                                  END *
 *************************************************************************/

/*************************************************************************
 * Verbose methods                                                 START *
 *************************************************************************/

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
inline bool 
Bus<BUSWIDTH, TYPES, DEBUG>::
VerboseSetup() {
	if(DEBUG && (verbose_all || verbose_setup)) 
		return true;
	return false;
}

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
inline bool
Bus<BUSWIDTH, TYPES, DEBUG>::
VerboseNonBlocking() {
	if(DEBUG && (verbose_all || verbose_non_blocking))
		return true;
	return false;
}

template<unsigned int BUSWIDTH, typename TYPES, bool DEBUG>
inline bool
Bus<BUSWIDTH, TYPES, DEBUG>::
VerboseBlocking() {
	if(DEBUG && (verbose_all || verbose_blocking))
		return true;
	return false;
}

/*************************************************************************
 * Verbose methods                                                   END *
 *************************************************************************/

} // end of namespace generic_bus
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#undef LOCATION
#undef TIME
#undef PHASE
#undef TRANS

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_BUS_BUS_TCC___

