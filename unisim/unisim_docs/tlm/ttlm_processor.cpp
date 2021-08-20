template <unsigned int PERCENTAGE_READ, unsigned int PERCENTAGE_WRITE
				unsigned int NUM_MEMORIES, unsigned int FREQUENCY>
class Processor : public sc_module {
public:
	sc_port<TlmSendIf<MemoryRequest, MemoryResponse> > port;

	Processor(sc_module_name name) : sc_module(name) {
		cycle_time = sc_time(1.0 / (double) FREQUENCY, SC_US);
		SC_HAS_PROCESS(Processor);
		SC_THREAD(Run);
	}
	
	void PerformRead() {
		Pointer<MemoryRequest> req = new(req) MemoryRequest();
		Pointer<MemoryResponse> rsp;
		sc_event rsp_event;
		Pointer<TlmMessage<MemoryRequest, MemoryResponse> > message =
			new(message) TlmMessage<MemoryRequest, MemoryResponse>(req, rsp_event);
		req->target_id = GetRandomNumber(NUM_MEMORIES);
		req->command = MemoryRequest::READ_COMMAND;
		req->write_data = 0;	
		while(!port->Send(message)) {
			wait(cycle_time); //(1)
		}
		wait(rsp_event);
		rsp = message->rsp;
	}
	
	void PerformWrite() {
		Pointer<MemoryRequest> req = new(req) MemoryRequest();
		Pointer<MemoryResponse> rsp;
		Pointer<TlmMessage<MemoryRequest, MemoryResponse> > message =
			new(message) TlmMessage<MemoryRequest, MemoryResponse>(req);
		
		req->target_id = GetRandomNumber(NUM_MEMORIES);
		req->command = MemoryRequest::WRITE_COMMAND;
		req->write_data = GetRandomNumber(10);
		while(!port->Send(message)) {
			wait(cycle_time); //(1)
		}
		wait(cycle_time); //(2)
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
				else
					wait(cycle_time); //(2)
			}
		}
	}
private:
	sc_time cycle_time;
	
	unsigned int GetRandomNumber(unsigned int limit) {
		unsigned int random_integer;
		random_integer = rand() % limit;
		return random_integer;
	}
};
