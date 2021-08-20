/*
 *  Copyright (c) 2007,
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

/**
 * @file transaction_spy.hh
 * @author Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_TLM_DEBUG_TRANSACTION_SPY_HH__
#define __UNISIM_COMPONENT_TLM_DEBUG_TRANSACTION_SPY_HH__

#include "unisim/kernel/tlm/tlm.hh"
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/variable/variable.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/util/garbage_collector/garbage_collector.hh"
#include "unisim/util/likely/likely.hh"
#include <string>

#define LOCATION "In file " << __FILE__ << " function " << __FUNCTION__ << " line #" << __LINE__

namespace unisim {
namespace component {
namespace tlm {
namespace debug {

using unisim::kernel::tlm::TlmSendIf;
using unisim::kernel::tlm::TlmMessage;
using unisim::kernel::tlm::ResponseListener;
using unisim::kernel::tlm::TlmNoResponse;
using unisim::kernel::variable::Parameter;
using unisim::kernel::Object;
using unisim::kernel::Service;
using unisim::kernel::ServiceImport;
using unisim::kernel::Client;
using unisim::util::garbage_collector::Pointer;
using std::string;

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template<class REQ>
class RequestSpy {
public:
	void Dump(unisim::kernel::logger::Logger &os, Pointer<REQ> &req) {
		os << "No info";
	}
};

template<class RSP, class REQ>
class ResponseSpy {
public:
	void Dump(unisim::kernel::logger::Logger &os, Pointer<RSP> &rsp, Pointer<REQ> &req) {
		os << "No info";
	}
};

/** Generic channel logger base class.
 * Generic channel logger base class uses the LoggerInterface to create log 
 *   messages that cross a channel. This class is non-intrusive, it only pushes
 *   an extra event to the message to snoop responses.
 */
template<class REQ, class RSP = TlmNoResponse> 
class TransactionSpy :
	public sc_core::sc_module,
	virtual public Object,
	public TlmSendIf<REQ, RSP>,
	public ResponseListener<REQ, RSP> {
public:
	SC_HAS_PROCESS(TransactionSpy);

	/** Port to receive incomming requests (and send outgoing responses) */
	sc_core::sc_export<TlmSendIf<REQ, RSP> > slave_port;
	/** Port to send outgoing requests (and receive responses) */
	sc_core::sc_port<TlmSendIf<REQ, RSP> > master_port;
	/** ServiceImport to report logs */

	/* constructor & destructor */
	/** Constructor */
	TransactionSpy(const sc_core::sc_module_name &name, Object *parent = 0);
	/** Destructor */
	virtual ~TransactionSpy();

	/* Methods required by the Client class */
	/** Method called when the client is disconnected.
	 * Method required by the Client class.
	 */
	virtual void OnDisconnect();
	/** Method to initialize variables.
	 * Method required by the Client class.
	 * 
	 * @return true if the objet is correctly initialized, false otherwise
	 */
	virtual bool BeginSetup();

	/* Module methods to process incomming requests/responses */
	/** Method to process incomming requests. 
	 * Method required by the TlmSendIf class.
	 * 
	 * @param message the message containing the incomming request 
	 * @return true if the message could be handled, false otherwise */
	virtual bool Send(const Pointer<TlmMessage<REQ, RSP> > &message);
	/** Method to process incomming responses.
	 * Method required by the ResponseListener class.
	 * 
	 * @param message the message containing the incomming response
	 * @param port the port through which the message was received (and 
	 * 		previously sent)
	 */
	virtual void ResponseReceived(const Pointer<TlmMessage<REQ, RSP> > &message, 
		sc_core::sc_port<TlmSendIf<REQ, RSP> > &port);

protected:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	Parameter<bool> param_verbose;
private:
	RequestSpy<REQ> req_spy;
	ResponseSpy<RSP, REQ> rsp_spy;

	string source_module_name;
	string source_port_name;
	string target_module_name;
	string target_port_name;

	Parameter<string> param_source_module_name;
	Parameter<string> param_source_port_name;
	Parameter<string> param_target_module_name;
	Parameter<string> param_target_port_name;
};

template<class REQ, class RSP>
TransactionSpy<REQ, RSP> ::
TransactionSpy(const sc_core::sc_module_name &name, Object *parent) :
	Object(name, parent),
	sc_core::sc_module(name),
	ResponseListener<REQ, RSP>(),
	slave_port("slave_port"),
	master_port("master_port"),
	logger(*this),
	verbose(false),
	param_verbose("verbose", this, verbose),
	source_module_name("source_module_name"),
	source_port_name("source_port_name"),
	target_module_name("target_module_name"),
	target_port_name("target_port_name"),
	param_source_module_name("source_module_name", this, source_module_name),
	param_source_port_name("source_port_name", this, source_port_name),
	param_target_module_name("target_module_name", this, target_module_name),
	param_target_port_name("target_port_name", this, target_port_name) {
	slave_port(*this);
}

template<class REQ, class RSP>
TransactionSpy<REQ, RSP> ::
~TransactionSpy() {
}

template<class REQ, class RSP>
bool
TransactionSpy<REQ, RSP> ::
BeginSetup() {
	return true;
}

template<class REQ, class RSP>
void
TransactionSpy<REQ, RSP> ::
OnDisconnect() {
	/* nothing to do */
}

template<class REQ, class RSP>
bool
TransactionSpy<REQ, RSP> ::
Send(const Pointer<TlmMessage<REQ, RSP> > &message) {
	if(ResponseListener<REQ, RSP>::Send(message, master_port)) {
		/* inform that a request was sent through this channel, and return
		 *   success */
		if(unlikely(verbose)) {
			logger << DebugInfo << LOCATION
				<< "Request sent from " 
				<< source_module_name << ":" << source_port_name << " to "
				<< target_module_name << ":" << target_port_name << std::endl
				<< "Request contents:" << std::endl;
			req_spy.Dump(logger, message->req);
			logger << std::endl << EndDebugInfo;
		}
		return true;
	} else {
		/* the message could not be send through the channel, return failure */
		return false;
	}
}

template<class REQ, class RSP>
void
TransactionSpy<REQ, RSP> ::
ResponseReceived(const Pointer<TlmMessage<REQ, RSP> > &message, 
	sc_core::sc_port<TlmSendIf<REQ, RSP> > &port) {
	/* inform that a message response was sent through this channel,
	 *   forward the response */
	if(unlikely(verbose)) {
		logger << DebugInfo << LOCATION
			<< "Response sent from "
			<< target_module_name << ":" << target_port_name << " to "
			<< source_module_name << ":" << source_port_name << std::endl
			<< "Request contents:" << std::endl;
		req_spy.Dump(logger, message->req);
		logger << std::endl 
			<< "Response contents: " << std::endl;
		rsp_spy.Dump(logger, message->rsp, message->req);
		logger << std::endl << EndDebugInfo;
	}
	message->GetResponseEvent()->notify(sc_core::SC_ZERO_TIME);
}

} // end of namespace debug
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#undef LOCATION

#endif // __UNISIM_COMPONENT_TLM_DEBUG_TRANSACTION_SPY_HH__
