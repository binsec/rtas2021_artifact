class Memory : public sc_module, public TlmSendIf<MemoryRequest, MemoryResponse> {
public:
	sc_export<TlmSendIf<MemoryRequest, MemoryResponse> > memory_export;
	
	Memory(sc_module_name module_name) : sc_module(module_name), data(0) {
		memory_export(*this);
		SC_HAS_PROCESS(Memory);
	}
	
	virtual bool Send(const Pointer<TlmMessage<MemoryRequest, MemoryResponse> > &message) {
		Pointer<MemoryRequest> req = message->req;   //(1)
		Pointer<MemoryResponse> rsp = new(rsp) MemoryResponse();  //(2)
		sc_event *ev;
		
		switch(req->command) {
			case MemoryRequest::READ_COMMAND:    //(3)
				rsp->data = data;
				message->rsp = rsp;
				ev = message->GetResponseEvent();
				ev->notify_delayed();
				break; 
			case MemoryRequest::WRITE_COMMAND:   //(4)
				data = req->data;
				break;
		}
		return true;
	}

private:
	unsigned int data;
};
