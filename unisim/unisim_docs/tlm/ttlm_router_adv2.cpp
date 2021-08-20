template <unsigned int NUM_PROCESSORS, unsigned int NUM_MEMORIES, unsigned int FREQUENCY>
class Router : public sc_module {
public:
	sc_export<TlmSendIf<MemoryRequest, MemoryResponse> > master_export[NUM_PROCESSORS];
	sc_port<TlmSendIf<MemoryRequest, MemoryResponse> > slave_port[NUM_MEMORIES];
	
	Router(sc_module_name module_name) : sc_module(module_name) {
		SC_HAS_PROCESS(Router);
		for(unsigned int i = 0; i < NUM_PROCESSORS; i++) {
			master_export_controller[i] = new MasterExportController(..., i);
			master_export[i](master_export_controller[i]->controller_export);
		}
		for(unsigned int i = 0; i < NUM_MEMORIES; i++) {
			slave_port_controller[i] = new SlavePortController(..., i);
			slave_port_controller[i]->slave_port(slave_port[i]);
		}
		for(unsigned int i = 0; i < NUM_PROCESSORS; i++)
			for(unsigned int j = 0; j < NUM_MEMORIES; j++)
				master_export_controller[i]->slave_port[j](slave_port_controller[j]->controller_export);
	}
private:
	class MasterExportController : public sc_module, public TlmSendIf<MemoryRequest, MemoryResponse> { ... };
	class SlavePortController : public sc_module, public TlmSendIf<MemoryRequest, MemoryResponse>,
							public ResponseListener<MemoryRequest, MemoryResponse> { //(2)
	public:
		sc_export<TlmSendIf<MemoryRequest, MemoryResponse> > controller_export;
		sc_port<TlmSendIf<MemoryRequest, MemoryResponse> > slave_port;
		
		SlavePortController(sc_module_name module_name, unsigned int in_id) : sc_module(module_name), id(in_id) {
			controller_export(*this);
			cycle_time = sc_time(1.0 / (double) FREQUENCY, SC_US);
			SC_HAS_PROCESS(SlavePortController); SC_THREAD(Dispatch);
		}

		bool Send(const Pointer<TlmMessage<MemoryRequest, MemoryResponse> > &message) { //(4)
			return !message->HasResponseEvent() ? slave_port->Send(message) : //(5)
				  ResponseListener<MemoryRequest, MemoryResponse>::Send(message, slave_port); //(6)
		}

		virtual void ResponseReceived(const Pointer<TlmMessage<MemoryRequest, MemoryResponse> > &msg, //(3)
										sc_port<TlmSendIf<MemoryRequest, MemoryResponse> > &port) {
			buffer.push_back(msg);
			uint64_t cindex = (uint64_t) ceil(sc_time_stamp() / cycle_time) % (NUM_PROCESSORS + NUM_MEMORIES);
			uint64_t index = (cindex >= (id + NUM_PROCESSORS)) ? cindex - (id + NUM_PROCESSORS) :
								(NUM_PROCESSORS + NUM_MEMORIES + cindex) - (id + NUM_PROCESSORS);
			dispatch_event.notify(index * cycle_time);
		}

		void Dispatch() { //(7)
			while(1) {
				wait(dispatch_event);
				if(!buffer.empty()) {
					Pointer<TlmMessage<MemoryRequest, MemoryResponse> > msg = buffer.front();
					buffer.pop_front();
					msg->GetResponseEvent()->notify(cycle_time * (NUM_PROCESSORS + NUM_MEMORIES));
					if(!buffer.empty()) dispatch_event.notify(cycle_time * (NUM_PROCESSORS + NUM_MEMORIES));
				}
			}
		}
	private:
		sc_time cycle_time;
		unsigned int id;
		sc_event dispatch_event;
		list<Pointer<TlmMessage<MemoryRequest, MemoryResponse> > > buffer;
	};

	MasterExportController *master_export_controller[NUM_PROCESSORS];
	SlavePortController *slave_port_controller[NUM_MEMORIES];
};
