template <unsigned int NUM_PROCESSORS, unsigned int NUM_MEMORIES, unsigned int FREQUENCY>
class Router : public sc_module {
public:
	sc_export<TlmSendIf<MemoryRequest, MemoryResponse> > master_export[NUM_PROCESSORS];
	sc_port<TlmSendIf<MemoryRequest, MemoryResponse> > slave_port[NUM_MEMORIES];
	
	Router(sc_module_name module_name) : sc_module(module_name) {
		SC_HAS_PROCESS(Router);
		for(unsigned int i = 0; i < NUM_PROCESSORS; i++) {
			stringstream s;
			s << name() << ":master_export_controller[" << i << "]";
			master_export_controller[i] = new MasterExportController(s.str().c_str(), i);
			master_export[i](master_export_controller[i]->controller_export);
			for(unsigned int j = 0; j < NUM_MEMORIES; j++) {
				master_export_controller[i]->slave_port[j](slave_port[j]);
			}
		}
	}
private:
 	class MasterExportController : public sc_module, public TlmSendIf<MemoryRequest, MemoryResponse> { //(1)
	public:
		sc_export<TlmSendIf<MemoryRequest, MemoryResponse> > controller_export;
		sc_port<TlmSendIf<MemoryRequest, MemoryResponse> > slave_port[NUM_MEMORIES];
		
		MasterExportController(sc_module_name module_name, unsigned int in_id) : sc_module(module_name), id(in_id) {
			controller_export(*this);
			cycle_time = sc_time(1.0 / (double) FREQUENCY, SC_US);
			SC_HAS_PROCESS(MasterExportController);
 			SC_THREAD(Dispatch);
		}

		bool Send(const Pointer<TlmMessage<MemoryRequest, MemoryResponse> > &message) { //(2)
			buffer.push_back(message);
			uint64_t cindex = (uint64_t) ceil(sc_time_stamp() / cycle_time) % NUM_PROCESSORS;
			uint64_t index = (cindex >= id) ? cindex - id : (NUM_PROCESSORS + cindex) - id;
			dispatch_event.notify(index * cycle_time); //(4)
			return true;
		}

		void Dispatch() { //(3)
			while(1) {
				wait(dispatch_event);

				if(!buffer.empty()) {
					Pointer<TlmMessage<MemoryRequest, MemoryResponse> > msg =
						buffer.front();
					if(slave_port[msg->req->target_id]->Send(msg)) {
						buffer.pop_front();
					if(!buffer.empty()) {
						dispatch_event.notify(cycle_time * NUM_PROCESSORS); //(5)
					}
				}
			}
		}
		
	private:
		unsigned int id;
		list<Pointer<TlmMessage<MemoryRequest, MemoryResponse> > > buffer;
		sc_time cycle_time;
		uint64_t cycle_time_value;
		sc_event dispatch_event;
	};

	MasterExportController *master_export_controller[NUM_PROCESSORS];
};
