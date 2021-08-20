template <unsigned int FREQUENCY, unsigned int READ_CYCLES>
class Memory : public sc_module, public TlmSendIf<MemoryRequest, MemoryResponse> {
public:
	sc_export<TlmSendIf<MemoryRequest, MemoryResponse> > memory_export;
	
	Memory(sc_module_name module_name) : sc_module(module_name), data(0) {
		memory_export(*this);
		cycle_time = sc_time(1.0 / (double) FREQUENCY, SC_US);
		read_time = sc_time((double) READ_CYCLES / (double) FREQUENCY, SC_US);
		SC_HAS_PROCESS(Memory);
	}
	
	bool Send(const Pointer<TlmMessage<MemoryRequest, MemoryResponse> > &message) {
		Pointer<MemoryRequest> req = message->req;
		Pointer<MemoryResponse> rsp = new(rsp) MemoryResponse();
		sc_event *ev;

		switch(req->command) {
			case MemoryRequest::READ_COMMAND:
				rsp->data = data;
				message->rsp = rsp;
				ev = message->GetResponseEvent();
				ev->notify(read_time);
				break; 
			case MemoryRequest::WRITE_COMMAND:
				data = req->data;
			break;
		}

		return true;
	}
private:
	unsigned int data;

	sc_time cycle_time;
	sc_time read_time;
};
