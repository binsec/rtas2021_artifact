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
 
#ifndef __UNISIM_COMPONENT_TLM_FSB_SIMPLEBUS_BUS_HH__
#define __UNISIM_COMPONENT_TLM_FSB_SIMPLEBUS_BUS_HH__

#include <inttypes.h>
#include <list>
#include <string>
#include <systemc>
#include "unisim/kernel/tlm/tlm.hh"
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/util/garbage_collector/garbage_collector.hh"
#include "unisim/component/tlm/message/simple_fsb.hh"
#include "unisim/service/interfaces/memory.hh"

namespace unisim {
namespace component {
namespace tlm {
namespace fsb {
namespace simple_bus {

using std::string;
using std::list;
using unisim::kernel::tlm::TlmMessage;
using unisim::kernel::tlm::TlmSendIf;
using unisim::kernel::tlm::ResponseListener;
using unisim::kernel::Object;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::variable::Parameter;
using unisim::util::garbage_collector::Pointer;
using unisim::component::tlm::message::SimpleFSBRequest;
using unisim::component::tlm::message::SimpleFSBResponse;
using unisim::service::interfaces::Memory;
	
/* Forward declaration of classes defined in this file */
template<class ADDRESS_TYPE, unsigned int DATA_SIZE, unsigned int NUM_PROCS, bool DEBUG>
class BusMasterPortController;
template <class ADDRESS_TYPE, unsigned int DATA_SIZE, unsigned int NUM_PROCS, bool DEBUG>
class Bus;
/* End of forward declaration */

/** Bus request port controller
 * This class provides a send interface for a single bus Request port.
 *   When called this implementation puts the incomming request in a fifo
 *   shared with the system bus implementation.
 */
template<class ADDRESS_TYPE, unsigned int DATA_SIZE, unsigned int NUM_PROCS, bool DEBUG = false>
class BusMasterPortController :
	// implements the TlmSendIf interface
	public TlmSendIf<SimpleFSBRequest<ADDRESS_TYPE, DATA_SIZE>, SimpleFSBResponse<DATA_SIZE> > {
private:
	typedef SimpleFSBRequest<ADDRESS_TYPE, DATA_SIZE> ReqType;
	typedef SimpleFSBResponse<DATA_SIZE> RspType;

public:
	/** Constructor 
	 *
	 * @param _name the name of this handler.
	 */
	BusMasterPortController(const char *_name,
		Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG> *_bus,
		unsigned int _id);
		
	/** TlmSendIf interface implementation.
	 * Puts the incomming request in the fifo. The request will be handled
	 *   when the bus will be ready.
	 * 
	 * @param message the received message.
	 */
	 virtual bool Send(const Pointer<TlmMessage<ReqType, RspType> > &message);
	 
private:
	string name;
	unsigned int id;
	Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG> *bus;
};

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS, bool DEBUG = false>
class Bus : 
	public sc_module,
	public ResponseListener<SimpleFSBRequest<ADDRESS_TYPE, DATA_SIZE>,
		SimpleFSBResponse<DATA_SIZE> >,
	public Service<Memory<ADDRESS_TYPE> >,
	public Client<Memory<ADDRESS_TYPE> > {
public:
	typedef SimpleFSBRequest<ADDRESS_TYPE, DATA_SIZE> ReqType;
	typedef Pointer<ReqType> PReqType;
	typedef SimpleFSBResponse<DATA_SIZE> RspType;
	typedef Pointer<RspType> PRspType;
	typedef TlmSendIf<ReqType, RspType> TransactionSendIf;
	typedef TlmMessage<ReqType, RspType> TransactionMsgType;
	typedef Pointer<TransactionMsgType> PTransactionMsgType;
 	typedef ResponseListener<SimpleFSBRequest<ADDRESS_TYPE, DATA_SIZE>,
		SimpleFSBResponse<DATA_SIZE> > inheritedRspListener;
		
	/* Exported services */
	/** Memory service provided by the bus */
	ServiceExport<Memory<ADDRESS_TYPE> > memory_export;
	/* Imported services */
	/** Memory service required by the bus when it itself 
	 * provides a memory service */
	ServiceImport<Memory<ADDRESS_TYPE> > memory_import;
	
	/***********************************
	 *     ports declaration START     *
	 ***********************************/
	/** Bus input ports. */
	sc_export<TransactionSendIf> *inport[NUM_PROCS];
	/** Bus output port for the chipset. */
	sc_port<TransactionSendIf> *chipset_outport;
	/***********************************
	 *     ports declaration END       *
	 ***********************************/

	SC_HAS_PROCESS(Bus);

	/** Constructor 
	 * Creates the input and output ports and name them.
	 * Associates the input ports with their implementation.
	 * @param module_name the name that will be given to this module and used to set
	 *                    the names of the different elements of the bus.
	 */
	Bus(const sc_module_name& module_name, Object *parent = 0);
	
	/** Destructor */
	~Bus();

	virtual bool Setup();
	virtual void Reset();
	
	virtual bool ReadMemory(ADDRESS_TYPE addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(ADDRESS_TYPE addr, const void *buffer, uint32_t size);
	
	/** Processors request handler
	 * Unique method to handle all the request comming from the cpus.
	 * 
	 * @param msg   the request to handle
	 * @param id    the cpu port id from which the request was received
	 * 
	 * @return true if the request could be accepted, false otherwise
	 */ 
	bool Send(const PTransactionMsgType &msg, unsigned int id);
	/** Processors response handler
	 * Unique method to handle all the responses (snooping responses) 
	 *   comming from the cpus.
	 * 
	 * @param msg   the received response
	 * @param port  the port through which it was received
	 */
	virtual void ResponseReceived(const PTransactionMsgType &msg,
		sc_port<TransactionSendIf> &port);

protected:
	/** Debug stuff */
	unisim::kernel::logger::Logger logger;
	bool verbose;
	Parameter<bool> param_verbose;
private:
	/** Method to synchronize main dispatching thread with the bus cycle.
	 * This method is responsible to compute how much time the main dispatching
	 *   thread should wait before sending a request in one of its queues.
	 */
	void BusSynchronize();
	/** Thread responsible of sending requests/responses.
	 * This thread is synchronized with the bus clock. At each clock
	 *   sends the requests that are expecting to be serviced in the fifos.
	 *   This thread should only be working when there are messages in the fifos.
	 */
	void BusClock();
	/** Method responsible of sending chipset requests and responses.
	 */
	void DispatchChipsetMessage();
	/** Method responsible of sending cpu requests.
	 */
	void DispatchCPUMessage();

	BusMasterPortController<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG> *inport_controller[NUM_PROCS];
	
	sc_fifo<PTransactionMsgType> *req_fifo[NUM_PROCS];
	sc_fifo<PTransactionMsgType> chipset_rsp_fifo;
	sc_mutex working_mutex;
	bool working;
	sc_event bus_synchro_event;
	unsigned int next_serviced;
	static const unsigned int CHIPSET_ID = NUM_PROCS;
	sc_time cycle_time;
	uint64_t cycle_time_int;
	Parameter<uint64_t> cycle_time_parameter;
};

} // end of namespace simple_bus
} // end of namespace fsb
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM_FSB_SIMPLEBUS_BUS_HH__
