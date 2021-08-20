template <unsigned int NUM_PROCESSORS, unsigned int NUM_MEMORIES>
class Router : public sc_module, public TlmSendIf<MemoryRequest, MemoryResponse> {
public:
	sc_export<TlmSendIf<MemoryRequest, MemoryResponse> > master_export[NUM_PROCESSORS];
	sc_port<TlmSendIf<MemoryRequest, MemoryResponse> > slave_port[NUM_MEMORIES];
	
	Router(sc_module_name module_name) : sc_module(module_name) {
		for(unsigned int i = 0; i < NUM_PROCESSORS; i++) 
			master_export[i](*this);
		SC_HAS_PROCESS(Router);
	}
	
	bool Send(const Pointer<TlmMessage<MemoryRequest, MemoryResponse> > &message) {
		Pointer<MemoryRequest> req = message->req;
		
		return slave_port[req->target_id]->Send(message);
	}
};
