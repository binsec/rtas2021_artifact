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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_KERNEL_TLM_TLM_HH__
#define __UNISIM_KERNEL_TLM_TLM_HH__

#include <systemc>
#include <unisim/util/garbage_collector/garbage_collector.hh>
#include <list>
#include <vector>
#include <stack>
#include <string>

namespace unisim {
namespace kernel {
namespace tlm {

using std::stack;
using std::list;
using std::vector;
using unisim::util::garbage_collector::Pointer;
using std::string;

template <typename REQ, typename RSP> class TlmMessage;
template <typename REQ, typename RSP> class TlmSendIf;

//=============================================================================
//=                             TlmMessage<REQ, RSP>                          =
//=============================================================================

class TlmNoResponse {
public:
private:
};

template <typename REQ, typename RSP = TlmNoResponse>
class TlmMessage {
private:
	stack<sc_core::sc_event *, vector<sc_core::sc_event *> > event_stack; // Gilles: this implementation is faster and consumes less memory than using stack<sc_core::sc_event *>
public:
	Pointer<REQ> req;
	Pointer<RSP> rsp;

	TlmMessage() : event_stack(), req(0), rsp(0) {
	}

	TlmMessage(const TlmMessage<REQ, RSP> &m) : event_stack(m.event_stack), req(m.req), rsp(m.rsp) {
	}

	TlmMessage(const Pointer<REQ>& _req) : event_stack(), req(_req), rsp(0) {
	}

	TlmMessage(const Pointer<REQ>& _req, sc_core::sc_event& ev) : event_stack(), req(_req), rsp(0) {
			event_stack.push(&ev);
	}
	
	TlmMessage<REQ, RSP> &
	operator =(const TlmMessage<REQ, RSP> &m) {
		req = m.req;
		rsp = m.rsp;
		event_stack = m.event_stack;
	}

    void SetRequest(const Pointer<REQ>& req) {
      this->req = req;
    }
    
    void SetResponse(const Pointer<RSP>& rsp) {
      this->rsp = rsp;
    }
    
    const Pointer<REQ>& GetRequest() const {
      return req;
    }
    
    const Pointer<RSP>& GetResponse() const {
      return rsp;
    }

    void PushResponseEvent(sc_core::sc_event& event) {
      event_stack.push(&event);
    }
    
	bool HasResponseEvent() const {
		return !event_stack.empty();
	}

    sc_core::sc_event *GetResponseEvent() const {
      return event_stack.empty() ? 0 : event_stack.top();
    }

    void PopResponseEvent() {
      if(event_stack.empty()) return;
      event_stack.pop();
    }
    
    const stack<sc_core::sc_event *, vector<sc_core::sc_event *> >& GetEventList() const {
      return event_stack;
    }
    
    void ClearEventStack() {
      while(!event_stack.empty()) 
				event_stack.pop();
    }
};

//=============================================================================
//=                           TlmSendIf<REQ, RSP>                             =
//=============================================================================

template <typename REQ, typename RSP = TlmNoResponse>
class TlmSendIf : public virtual sc_core::sc_interface {
public:
	virtual bool Send(const Pointer<TlmMessage<REQ, RSP> >& message) = 0;
};
  
//=============================================================================
//=                       ResponseListener<REQ, RSP>                          =
//=============================================================================

/**
 * ResponseListener is an utility to facilitate the handling of multiple responses
 *   in a centralized fashion. The parent class uses the Send method to send a message
 *   that expects a response, and he class will notifiy to the parent class that a 
 *   response has been received with the ResponseReceived method, both methods 
 *   have the message and the port that should be/has been used.
 * This class is abstract, and the class that inherits must implement the 
 *   ResponseReceived method.
 * ResponseListener uses a special feature of systemc, to dinamically create processes.
 *   Created processes will wait for the response of a message and notify the parent class
 *   when received (with ResponseReceived). The processes are reused, once a message has
 *   been handled, the process is used for another message. Free processes are keeped
 *   in the free_handlers list.
 *
 * IMPORTANT: when using ResponseListener you should compile with a special systemc
 *   flag: -DSC_INCLUDE_DYNAMIC_PROCESSES
 */
template<class REQ, class RSP>
class ResponseListener {
private:
	/* forward declaration */
	class MessageHandler;
	
public:
	ResponseListener() : free_handlers() {};

	virtual void ResponseReceived(const Pointer<TlmMessage<REQ, RSP> > &msg, 
								sc_core::sc_port<TlmSendIf<REQ, RSP> > &port) = 0;
	
	bool Send(const Pointer<TlmMessage<REQ, RSP> > &msg, sc_core::sc_port<TlmSendIf<REQ, RSP> > &port) {
		MessageHandler *handler;
		
		if(!msg->HasResponseEvent()) {
			return port->Send(msg);
		}
		
		if(free_handlers.empty()) {
			handler = new MessageHandler();
			sc_core::sc_spawn(sc_bind(&ResponseListener<REQ, RSP>::MessageHandlerProcess,
											 this,
											 handler));
		} else {
			handler = free_handlers.front();
			free_handlers.pop_front();
		}
		
		msg->PushResponseEvent(handler->event);
		handler->msg = msg;
		handler->port = &port;
		if(port->Send(msg)) {
			return true;
		} else {
			msg->PopResponseEvent();
			handler->msg = 0;
			free_handlers.push_back(handler);
			return false;
		}
	}
	
private:
	
	void MessageHandlerProcess(MessageHandler *handler) {
		while(1) {
			sc_core::wait(handler->event);
			handler->msg->PopResponseEvent();
			ResponseReceived(handler->msg, *(handler->port));
			handler->msg = 0;
			free_handlers.push_back(handler);
		}
	}
	
	class MessageHandler {
	public:
		Pointer<TlmMessage<REQ, RSP> > msg;
		sc_core::sc_event event;
		sc_core::sc_port<TlmSendIf<REQ, RSP> > *port;
	};
	
	list<MessageHandler *> free_handlers;
};

template<class REQ, class RSP,
		class WHO_REQ, class WHO_RSP>
class AdvancedResponseListener {
private:
	/* forward declaration */
	class MessageHandler;
	
public:
	virtual void ResponseReceived(const Pointer<TlmMessage<REQ, RSP> > &msg, 
								sc_core::sc_port<TlmSendIf<REQ, RSP> > &port,
								const Pointer<TlmMessage<WHO_REQ, WHO_RSP> > &orig_msg,
								sc_core::sc_export<TlmSendIf<WHO_REQ, WHO_RSP> > &who_port) = 0;
	
	bool Send(const Pointer<TlmMessage<REQ, RSP> > &msg, 
			sc_core::sc_port<TlmSendIf<REQ, RSP> > &port,
			const Pointer<TlmMessage<WHO_REQ, WHO_RSP> > &orig_msg,
			sc_core::sc_export<TlmSendIf<WHO_REQ, WHO_RSP> > &who_port) {
		MessageHandler *handler;
		
		if(free_handlers.empty()) {
			handler = new MessageHandler();
			sc_core::sc_spawn(sc_bind(&AdvancedResponseListener<REQ, RSP, WHO_REQ, WHO_RSP>::MessageHandlerProcess,
											 this,
											 handler));
		} else {
			handler = free_handlers.front();
			free_handlers.pop_front();
		}
		
		msg->PushResponseEvent(handler->event);
		handler->msg = msg;
		handler->port = &port;
		handler->orig_msg = orig_msg;
		handler->who_port = &who_port;
		if(port->Send(msg)) {
			return true;
		} else {
			msg->PopResponseEvent();
			free_handlers.push_back(handler);
			return false;
		}
	}
	
private:
	
	void MessageHandlerProcess(MessageHandler *handler) {
		while(1) {
			sc_core::wait(handler->event);
			handler->msg->PopResponseEvent();
			ResponseReceived(handler->msg, *(handler->port), 
							handler->orig_msg, *(handler->who_port));
			free_handlers.push_back(handler);
		}
	}
	
	class MessageHandler {
	public:
		sc_core::sc_event event;
		Pointer<TlmMessage<REQ, RSP> > msg;
		Pointer<TlmMessage<WHO_REQ, WHO_RSP> > orig_msg;
		sc_core::sc_port<TlmSendIf<REQ, RSP> > *port;
		sc_core::sc_export<TlmSendIf<WHO_REQ, WHO_RSP> > *who_port;
	};
	
	list<MessageHandler *> free_handlers;
};
	
//=============================================================================
//=                    RequestPortIdentifier<REQ, RSP>                        =
//=                               and                                         =
//=               RequestPortIdentifierInterface<REQ, RSP>                    =
//=============================================================================

template<class REQ, class RSP = TlmNoResponse>
class RequestPortIdentifierInterface {
public:
	virtual bool Send(const Pointer<TlmMessage<REQ, RSP> > &message,
		unsigned int id) = 0;
};

template<class REQ, class RSP = TlmNoResponse>
class RequestPortIdentifier :
	public TlmSendIf<REQ, RSP> {
public:
	RequestPortIdentifier(const char *_name,
		unsigned int _id,
		RequestPortIdentifierInterface<REQ, RSP> &_master) : 
		master(&_master), name(_name), id(_id) {}
	
	~RequestPortIdentifier() {}
	
	virtual bool Send(const Pointer<TlmMessage<REQ, RSP> > &message) {
		return master->Send(message, id);
	}

private:
	RequestPortIdentifierInterface<REQ, RSP> *master;
	string name;
	unsigned int id;
};

} // end of tlm namespace
} // end of kernel namespace
} // end of unisim namespace

#endif // __UNISIM_KERNEL_TLM_TLM_HH__
