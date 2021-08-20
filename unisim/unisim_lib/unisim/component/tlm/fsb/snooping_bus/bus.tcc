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
 
#ifndef __UNISIM_COMPONENT_TLM_FSB_SNOOPINGBUS_BUS_TCC__
#define __UNISIM_COMPONENT_TLM_FSB_SNOOPINGBUS_BUS_TCC__

#include <sstream>

namespace unisim {
namespace component {
namespace tlm {
namespace fsb {
namespace snooping_bus {

using std::stringstream;
using unisim::kernel::Object;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
using std::hex;
using std::dec;
using std::endl;

#define LOCATION "in file " << __FILE__ << ", function " << __FUNCTION__ << ", line #" << __LINE__

/****************************************************/
/* Bus request port controller implementation START */
/****************************************************/

template<class ADDRESS_TYPE, unsigned int DATA_SIZE, unsigned int NUM_PROCS>
BusMasterPortController<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS>::
BusMasterPortController(const char *_name,
	Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> *_bus,
	unsigned int _id) :
	name(_name),
	id(_id),
	bus(_bus) {}

/** TlmSendIf interface implementation.
 * Puts the incomming request in the fifo. The request will be handled
 *   when the bus will be ready.
 * 
 * @param message the received message.
 */
template<class ADDRESS_TYPE, unsigned int DATA_SIZE, unsigned int NUM_PROCS>
bool 
BusMasterPortController<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS>::
Send(const Pointer<TlmMessage<ReqType, RspType> > &message) {
 	return bus->Send(message, id);
}

/****************************************************/
/* Bus request port controller implementation  STOP */
/****************************************************/

/****************************************************/
/* Bus implementation                         START */
/****************************************************/

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS>
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> :: 
Bus(const sc_core::sc_module_name& module_name, Object *parent) :
	Object(module_name, parent, "Front side bus"),
	sc_core::sc_module(module_name),
	ResponseListener<ReqType, RspType>(),		
	Service<Memory<ADDRESS_TYPE> >(module_name, parent),
	Client<Memory<ADDRESS_TYPE> >(module_name, parent),
	memory_export("memory-export", this),
	memory_import("memory-import", this),
	logger(*this),
	verbose(false),
	param_verbose("verbose", this, verbose, "enable/disable verbosity"),
	chipset_req_fifo("chipset_req_fifo"),
	chipset_rsp_fifo("chipset_rsp_fifo"),
	bus_synchro_event(),
	next_serviced(0),
	chipset_snoop(false),
	cpu_snoop(false),
	snoop_event(),
	snoop_counter(0),
	cycle_time(),
	cycle_time_parameter("cycle-time", this, cycle_time, "cycle time")
{
	cycle_time_parameter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	
	/* create input ports and name them */
	for(unsigned int i = 0; i < NUM_PROCS; i++) {
		stringstream s;
		s << "inport[" << i << "]";
		inport[i] = new sc_core::sc_export<TransactionSendIf>(s.str().c_str());
	}

	for(unsigned int i = 0; i < NUM_PROCS; i++) {
		stringstream s;
		s << "inport_controller[" << i << "]";
		inport_controller[i] = 
			new BusMasterPortController<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS>
				(s.str().c_str(), this, i);
		/* associate the input port with its implementation */
		(*inport[i])(*inport_controller[i]);
	}
	
	for(unsigned int i = 0; i < NUM_PROCS; i++) {
		stringstream s;
		s << "req_fifo[" << i << "]";
		req_fifo[i] = new sc_core::sc_fifo<PTransactionMsgType>(s.str().c_str());
	}

	/* create snooping output ports and name them */
	for(unsigned int i = 0; i < NUM_PROCS; i++) {
		stringstream s;
		s << "outport[" << i << "]";
		outport[i] = new sc_core::sc_port<TransactionSendIf>;
	}
		
	/* create the chipset ports and name them */
	{	stringstream s;
		s << "chipset_outport";
		chipset_outport = new sc_core::sc_port<TransactionSendIf>(s.str().c_str());
	}

	{	stringstream s;
		s << "chipset_inport";
		chipset_inport = new sc_core::sc_export<TransactionSendIf>(s.str().c_str());
	}
	(*chipset_inport)(*this);

	SC_THREAD(BusClock);
}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS>
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> :: 
~Bus()
{
	for(unsigned int i = 0; i < NUM_PROCS; i++) {
		delete inport[i];
	}

	for(unsigned int i = 0; i < NUM_PROCS; i++) {
		delete inport_controller[i];
	}
	
	for(unsigned int i = 0; i < NUM_PROCS; i++) {
		delete req_fifo[i];
	}

	/* create snooping output ports and name them */
	for(unsigned int i = 0; i < NUM_PROCS; i++) {
		delete outport[i];
	}
		
	delete chipset_outport;
	delete chipset_inport;
}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS>
bool
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> :: 
SetupMemory() {
	if(!memory_import) {
		logger << DebugError << LOCATION << ", "
			<< "No memory connected" << endl
			<< EndDebugError;
		return false;
	}

	if(unlikely(verbose)) {
		logger << DebugInfo << LOCATION << ", "
			<< "cycle time of " << cycle_time << endl
			<< EndDebugInfo;
	}
	if(cycle_time == sc_core::SC_ZERO_TIME) {
		logger << DebugError << LOCATION << ", "
			<< "cycle_time should be bigger than 0" << EndDebugError;
		return false;
	}
	return true;
}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS>
bool
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> :: 
Setup(ServiceExportBase *srv_export) {
	if(srv_export == &memory_export) return SetupMemory();
	
	logger << DebugError << "Internal error" << EndDebugError;
	
	return false;
}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS>
void Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> :: 
Reset() {
}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS>
void 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> :: 
ResetMemory() {
	if(memory_import) {
		memory_import->ResetMemory();
	}
}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS>
bool 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> :: 
ReadMemory(ADDRESS_TYPE addr, void *buffer, uint32_t size) {
	return memory_import ? 
		memory_import->ReadMemory(addr, buffer, size) : false;
}
	
template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS>
bool 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> :: 
WriteMemory(ADDRESS_TYPE addr, const void *buffer, uint32_t size) {
	return memory_import ? 
		memory_import->WriteMemory(addr, buffer, size) : false;
}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS>
bool 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> :: 
Send(const PTransactionMsgType &msg) {
	/* check if the incomming chipset request can be inserted in its
	 *   request fifo, if no just return returning false, which
	 *   means that the request could not be processed */
	if(!chipset_req_fifo.nb_write(msg))
		return false;

	BusSynchronize();
	return true;
}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS>
bool 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> :: 
Send(const PTransactionMsgType &msg, unsigned int id) {
	if(unlikely(verbose))
		logger << DebugInfo << LOCATION << ", "
			<< "Request received on port " << id << endl
			<< EndDebugInfo;

	/* check if the incomming request can be inserted in its
	 *   request fifo, if no just return returning false, which
	 *   means that the request could not be processed */
	if(!req_fifo[id]->nb_write(msg)) {
		if(unlikely(verbose)) {
			logger << DebugWarning << LOCATION << ", " 
				<< "Could not accept incomming request on port " << id
				<< " because fifo full"
				<< endl << EndDebugWarning;
		}
		return false;
	}
	
	BusSynchronize();
	return true;
}
	
template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS>
void
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> :: 
ResponseReceived(const PTransactionMsgType &msg,
	sc_core::sc_port<TransactionSendIf> &port) {
	/* check if the response comes from the chipset, if so
	 *   put the response in the chipset fifo queue */
	if(&port == chipset_outport) {
		if(unlikely(verbose)) {
			logger << DebugInfo << LOCATION << ", "
				<< "Response received on chipset_outport." << endl
				<< "Request:" << endl << (*(msg->req)) << endl
				<< "Response:" << endl << (*(msg->rsp)) << endl
				<< EndDebugInfo;
		}
		chipset_rsp_fifo.write(msg);

		BusSynchronize();
		return;
	}
	/* the response comes from one of the cpu ports, which means
	 *   that it is a response for a snooping request, process the
	 *   response immediately*/
	ProcessSnoopingResponse(msg);
}
	
template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS>
void 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> :: 
BusSynchronize() {
	/* compute the current bus cycles */
	sc_core::sc_time cur_time = sc_core::sc_time_stamp();
	sc_dt::uint64 cur_time_int = cur_time.value();
	sc_dt::uint64 cycle_time_int = cycle_time.value();
	sc_dt::uint64 cur_cycle = cur_time_int / cycle_time_int;
	sc_dt::uint64 cur_cycle_init_int = cur_cycle * cycle_time_int;
	if(cur_cycle_init_int > cur_time_int) {
		logger << DebugError << LOCATION << ", "
			<< "current cycle time is bigger than simulation time ("
			<< "cur_cycle_init_int = " << cur_cycle_init_int << ", "
			<< "cur_time_int = " << cur_time_int << ")" << endl
			<< EndDebugError;
		Object::Stop(-1);
	}
	// check if a message can be send right now
	if(cur_cycle_init_int == cur_time_int) {
		if(unlikely(verbose))
			logger << DebugInfo << LOCATION << ", "
				<< "bus synchronize in 0 time" << endl
				<< EndDebugInfo;
		// yes a message can be sent right now
		bus_synchro_event.notify(sc_core::SC_ZERO_TIME);
	} else {
		if(unlikely(verbose))
			logger << DebugInfo << LOCATION << ", "
				<< "bus synchronize in " 
				<< ((cycle_time * (cur_cycle + 1)) - cur_time).to_string() 
				<< endl
				<< EndDebugInfo;
		//fprintf(stderr, "sending message in more time ");
		//fprintf(stderr, ((cycle_time * (cur_cycle + 1)) - cur_time).to_string().c_str());
		bus_synchro_event.notify((cycle_time * (cur_cycle + 1)) - cur_time);
		//bus_synchro_event.notify_delayed();
	}
}
	
template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS>
void 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> :: 
BusClock() {
	sc_core::sc_time last_serviced_time;
	sc_core::sc_time current_time;
	while(1) {
		wait(bus_synchro_event);
		current_time = sc_core::sc_time_stamp();
		if(current_time <= last_serviced_time) {
			if(unlikely(verbose))
				logger << DebugInfo << LOCATION << ", "
					<< "Delaying bus cycle execution"
					<< endl << EndDebugInfo;
			wait((last_serviced_time - current_time) + cycle_time);
			last_serviced_time = sc_core::sc_time_stamp();
		} else
			last_serviced_time = current_time;
		last_serviced_time = sc_core::sc_time_stamp();
		if(unlikely(verbose)) {
			logger << DebugInfo << LOCATION << ", "
				<< "Executing bus cycle" << endl
				<< EndDebugInfo;
		}
		
		/* start locking in the queues for which is the next fifo that can
		 *   be serviced */
		bool cont = true;
		unsigned int counter = 0;
		do {
			if(next_serviced < NUM_PROCS) {
				if(req_fifo[next_serviced]->num_available() != 0)
					cont = false;
				else
					next_serviced++;
			} else {
				if(chipset_req_fifo.num_available() != 0 ||
					chipset_rsp_fifo.num_available() != 0) 
					cont = false;
				else
					next_serviced = 0;
			}
			counter++;
		} while(cont && counter < NUM_PROCS + 2);
		/* if we did not found at least one fifo with one message pending
		 *   show a warning message, because it should have never occurred,
		 * and go back to the beginning of the BusClock loop after indicating
		 *   that there is no work in the bus */
		if(counter == NUM_PROCS + 2) {
			if(unlikely(verbose)) {
				logger << DebugWarning << LOCATION << ", "
					<< "Dispatch thread BusClock was requested when there was "
					<< "no message to dispatch, this should never occur" << endl
					<< EndDebugWarning;
			}
			continue;
		}
		/* dispatch the message */
		if(next_serviced == CHIPSET_ID) {
			DispatchChipsetMessage();
		} else {
			DispatchCPUMessage();
		}
		/* the message has been handled:
		 * - set the next_serviced to the next fifo
		 * - check if there are still messages to handle, if so synchronize the bus
		 *     again so it will be handled */
		next_serviced += 1;
		next_serviced = next_serviced % (NUM_PROCS + 1);
		bool msg_found = false;
		for(unsigned int i = 0; !msg_found && i < NUM_PROCS; i++) {
			if(req_fifo[i]->num_available() != 0) {
				if(unlikely(verbose))
					logger << DebugInfo << LOCATION << ", "
						<< req_fifo[i]->num_available()
						<< " messages found in req_fifo[" << i << "]"
						<< " at the end of bus cycle" << endl
						<< EndDebugInfo;
				msg_found = true;
			}
		}
		if(!msg_found) {
			if(chipset_req_fifo.num_available() != 0) {
				if(unlikely(verbose))
					logger << DebugInfo << LOCATION << ", "
						<< chipset_req_fifo.num_available()
						<< " messages found in chipset_req_fifo"
						<< " at the end of bus cycle" << endl
						<< EndDebugInfo;
				msg_found = true;
			}
		}
		if(!msg_found) {
			if(chipset_rsp_fifo.num_available() != 0) {
				if(unlikely(verbose))
					logger << DebugInfo << LOCATION << ", "
						<< chipset_rsp_fifo.num_available()
						<< " messages found in chipset_rsp_fifo"
						<< " at the end of bus cycle" << endl
						<< EndDebugInfo;
				msg_found = true;
			}
		}
		if(msg_found) {
			if(unlikely(verbose)) {
				logger << DebugInfo << LOCATION << ", "
					<< "Finished bus cycle but a message found in one of " 
					<< "the fifos" << endl
					<< EndDebugInfo;
			}
			BusSynchronize();
		} else {
			/* unset the working boolean, there is nothing else to do */
			if(unlikely(verbose)) 
				logger << DebugInfo << LOCATION << ", "
					<< "Finished bus cycle"
					<< endl << EndDebugInfo;
		}
	}
}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS>
void 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> :: 
DispatchChipsetMessage() {
	PTransactionMsgType msg;
	
	/* Responses have higher priority than request comming from the chipset.
	 * Check if a response is available if so, notify the message event. */
	if(chipset_rsp_fifo.num_available() != 0) {
		msg = chipset_rsp_fifo.read();
		if(unlikely(verbose)) 
			logger << DebugInfo << LOCATION << ", "
				<< "Dispatching chipset_rsp_fifo message." << endl
				<< "Request:" << endl << (*(msg->req)) << endl 
				<< "Response:" << endl << (*(msg->rsp)) << endl
				<< EndDebugInfo;
		msg->GetResponseEvent()->notify(sc_core::SC_ZERO_TIME);
		return;
	} 
	/* If there is no response available, then a request can be handled */
	/* check if there is a request from the chipset available, if no, then
	 *   show a warning because the DispatchChipsetMessage method should not have
	 *   been called */
	if(chipset_req_fifo.num_available() == 0) {
		if(unlikely(verbose)) {
			logger << DebugWarning << LOCATION << ", "
				<< "The DispatchChipsetMessage method was called when there were "
				<< "no requests/responses available in the chipset fifos, this should "
				<< "have never occurred" << endl
				<< EndDebugWarning;
		}
		return;
	}
	msg = chipset_req_fifo.read();
	if(unlikely(verbose)) 
		logger << DebugInfo << LOCATION << ", "
			<< "Dispatching chipset_rsp_fifo message." << endl
			<< "Request:" << endl << (*(msg->req)) << endl 
			<< "Response:" << endl << (*(msg->rsp)) << endl
			<< EndDebugInfo;
	/* only two possible requests can arrive from the chipset for the moment,
	 *   a write command, and a read with intent to modify command */
	if(!(msg->req->type == ReqType::READ || msg->req->type == ReqType::READX || msg->req->type == ReqType::WRITE)) {
		logger << DebugError << LOCATION << ", "
			<< "The bus received a request from the chipset that is not "
			<< "a READX, neither a WRITE. Stopping simulation" << endl
			<< EndDebugError;
		Object::Stop(-1);
	}
	/* if the message is just a write, then just forward the message to the cpus */
	if(msg->req->type == ReqType::WRITE) {
		for(unsigned int i = 0; i < NUM_PROCS; i++)
			if(!(*outport[i])->Send(msg))
				wait(cycle_time);
		/* the task is done */
	} else {
	/* the message is a read with intent to modify (READX), this is harder to handle.
	 * First send the message to all the cpus, then wait for all their answers,
	 *   and once they have been received send the reply to the chipset */
		PTransactionMsgType msg_copies[NUM_PROCS];
		sc_core::sc_event ev;
		for(unsigned int i = 0; i < NUM_PROCS; i++)
			msg_copies[i] = new (msg_copies[i]) TransactionMsgType(msg->GetRequest(), ev);
	/* TODO: snoop should be tested for debugging */
		snoop_counter = 0;
		cpu_snoop = false;
		chipset_snoop = true;
		for(unsigned int i = 0; i < NUM_PROCS; i++)
			if(!inheritedRspListener::Send(msg_copies[i], *outport[i]))
				wait(cycle_time);
		wait(snoop_event);
		chipset_snoop = false;
		cpu_snoop = false;
		wait(cycle_time);
	/* all the replies have been received, check if there is any hit, if so send it as
	 *   answer, otherwise, send a miss response */
		msg->rsp = new(msg->rsp) RspType();
		msg->rsp->read_status = RspType::RS_MISS;
		for(unsigned int i = 0; i < NUM_PROCS; i++) { //What about SHARED???
			if(msg_copies[i]->rsp->read_status == RspType::RS_MODIFIED) {
				msg->rsp = msg_copies[i]->rsp;
			}
		}
		msg->GetResponseEvent()->notify();
	}
}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS>
void 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> :: 
DispatchCPUMessage() {
	/* get the message */
	PTransactionMsgType msg;
	
	msg = req_fifo[next_serviced]->read();
	
	if(unlikely(verbose)) 
		logger << DebugInfo << LOCATION << ", "
			<< "Dispatching req_fifo[" << next_serviced << "] message." << endl
			<< "Request:" << endl << (*(msg->req)) << endl 
			<< EndDebugInfo;
	/* if the message is not global it just needs to be send to the chipset */
	if(!msg->req->global) {
		while(!inheritedRspListener::Send(msg, *chipset_outport))
			wait(cycle_time);
		wait(cycle_time);
		return;
	}
	
	/* the message is global, different actions needs to be done depending on the
	 *   request type */
	/* if the request is for the processors caches, it just need to be send to the
	 *   other cpus */
	if(msg->req->type == ReqType::INV_BLOCK ||
		msg->req->type == ReqType::FLUSH_BLOCK ||
		msg->req->type == ReqType::ZERO_BLOCK ||
		msg->req->type == ReqType::INV_TLB) {
		for(unsigned int i = 0; i < NUM_PROCS; i++) 
			if(i != next_serviced)
				while(!(*outport[i])->Send(msg))
					wait(cycle_time);
		wait(cycle_time);
		return;
	}
	/* if the request is a write the request needs to be sent to the other cpus and the
	 *   chipset */
	if(msg->req->type == ReqType::WRITE) {
		for(unsigned int i = 0; i < NUM_PROCS; i++) 
			if(i != next_serviced)
				while(!(*outport[i])->Send(msg))
					wait(cycle_time);
		while(!(*chipset_outport)->Send(msg))
			wait(cycle_time);		
		wait(cycle_time);
		return;
	}

	/* check that the request is a read or a readx */
	if(msg->req->type != ReqType::READ &&
		msg->req->type != ReqType::READX) {
			logger << DebugError << LOCATION << ", "
				<< "Unknown command received" << endl
				<< EndDebugError;
		Object::Stop(-1);
	} 
	/* the message is a read with intent to modify (READX) or a read (READ),
	 *   this is a little bit harder to handle.
	 * First send the message to all the cpus, then wait for all their answers,
	 *   and once they have been received send the reply to the chipset */
	if(NUM_PROCS > 1) {
		PTransactionMsgType msg_copies[NUM_PROCS];
		sc_core::sc_event ev;
		for(unsigned int i = 0; i < NUM_PROCS; i++)
			msg_copies[i] = new (msg_copies[i]) TransactionMsgType(msg->GetRequest(), ev);
		/* TODO: snoop should be tested for debugging */
		snoop_counter = 0;
		cpu_snoop = true;
		chipset_snoop = false;
		for(unsigned int i = 0; i < NUM_PROCS; i++)
			if(i != next_serviced)
				while(!inheritedRspListener::Send(msg, *outport[i]))
					wait(cycle_time);
		wait(snoop_event);
		chipset_snoop = false;
		cpu_snoop = false;
		wait(cycle_time);
		/* all the replies have been received, check if there is any hit, if so send it as
		 *   answer, otherwise, send a miss response */
		bool has_response = false;
		msg->rsp = new(msg->rsp) RspType();
		msg->rsp->read_status = RspType::RS_MISS;
		for(unsigned int i = 0; i < NUM_PROCS; i++) {
			if(!has_response && msg_copies[i]->rsp->read_status == RspType::RS_SHARED) {
				msg->rsp = msg_copies[i]->rsp;
				has_response = true;
			}
			if(msg_copies[i]->rsp->read_status == RspType::RS_MODIFIED) {
				msg->rsp = msg_copies[i]->rsp;
				has_response = true;
			}
		}
		if(has_response)
			msg->GetResponseEvent()->notify();
		else
			/* the request needs to be send to the chipset */
			while(!inheritedRspListener::Send(msg, *chipset_outport))
				wait(cycle_time);
	} else
		while(!inheritedRspListener::Send(msg, *chipset_outport))
			wait(cycle_time);
		
}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS>
void 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS> :: 
ProcessSnoopingResponse(const PTransactionMsgType &msg) {
	if(!chipset_snoop && !cpu_snoop) {
		logger << DebugError << LOCATION << ", "
			<< "Received a snoop response when nothing was expected" << endl
			<< EndDebugError;
		Object::Stop(-1);
	}
	snoop_counter++;
	if(chipset_snoop && snoop_counter == NUM_PROCS)
		snoop_event.notify(sc_core::SC_ZERO_TIME);
	if(cpu_snoop && snoop_counter == (NUM_PROCS - 1))
		snoop_event.notify(sc_core::SC_ZERO_TIME);
}

/****************************************************/
/* Bus implementation                          STOP */
/****************************************************/

#undef LOCATION

} // end of namespace snooping_bus
} // end of namespace fsb
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM_FSB_SNOOPINGBUS_BUS_TCC__
