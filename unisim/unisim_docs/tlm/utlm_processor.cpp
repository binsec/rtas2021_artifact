template <unsigned int PERCENTAGE_READ, unsigned int PERCENTAGE_WRITE>
class Processor : public sc_module {
public:
	sc_port<TlmSendIf<MemoryRequest, MemoryResponse> > port;

	Processor(sc_module_name name) : sc_module(name) {
		SC_HAS_PROCESS(Processor);
		SC_THREAD(Run);
	}
	
	void PerformRead() {
		Pointer<MemoryRequest> req = new(req) MemoryRequest();  //(1)
		Pointer<MemoryResponse> rsp;
		sc_event rsp_event;
		sc_event wait_ev;
		Pointer<TlmMessage<MemoryRequest, MemoryResponse> > message =
			new(message) TlmMessage<MemoryRequest, MemoryResponse>(req, rsp_event);  //(2)
		
		req->command = MemoryRequest::READ_COMMAND;   //(3)
		req->data = 0;
		while(!port->Send(message)) {   //(4)
			wait_ev.notify_delayed();
			wait(wait_ev);              //(5)
		}
		wait(rsp_event);     //(6)
		rsp = message->rsp;  //(7)
	}
	
	void PerformWrite() {
		Pointer<MemoryRequest> req = new(req) MemoryRequest();  //(1)
		Pointer<MemoryResponse> rsp;
		Pointer<TlmMessage<MemoryRequest, MemoryResponse> > message =
			new(message) TlmMessage<MemoryRequest, MemoryResponse>(req);  //(2)
		sc_event wait_ev;
		
		req->command = MemoryRequest::WRITE_COMMAND;  //(3)
		req->data = GetRandomNumber(10);
		
		while(!port->Send(message)) {  //(4)
			wait_ev.notify_delayed();
			wait(wait_ev);             //(5)
		}
	}
	
	void Run() {
		while(1) {
		unsigned int inst = GetRandomNumber(100);
		if(inst < PERCENTAGE_READ) {
			PerformRead();
	    } else {
			inst = inst - PERCENTAGE_READ;
			if(inst < PERCENTAGE_WRITE)
				PerformWrite();
		}
	}

private:
	unsigned int GetRandomNumber(unsigned int limit) {
		unsigned int random_integer;
		
		random_integer = rand() % limit;
		return random_integer;
	}
};
