/*
 *  Copyright (c) 2008,
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
 
#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_HH__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_HH__

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/passthrough_target_socket.h>

#if HAVE_TVS

#include <tvs/tracing.h>

#endif

#include <inttypes.h>
#include <queue>
#include <vector>
#include <map>
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/kernel/tlm2/clock.hh"
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include "unisim/component/tlm2/interconnect/generic_router/router_dispatcher.hh"
#include "unisim/component/tlm2/interconnect/generic_router/mapping.hh"
#include "unisim/component/tlm2/interconnect/generic_router/protection.hh"
#include "unisim/service/interfaces/memory.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace generic_router {

#if HAVE_TVS
struct timed_bandwidth_traits
{
  typedef double value_type;
  typedef tracing::timed_empty_policy_silence<value_type> empty_policy;
  typedef tracing::timed_split_policy_decay<value_type> split_policy;
  typedef tracing::timed_join_policy_combine<value_type> join_policy;
  typedef tracing::timed_merge_policy_error<value_type> merge_policy;
};
#endif

class RouterPayloadExtension :
	public tlm::tlm_extension<RouterPayloadExtension> {
public:
	RouterPayloadExtension() : fifo() {}

	virtual tlm_extension_base *clone() const {
		RouterPayloadExtension *clone = new RouterPayloadExtension(fifo);
		return clone;
	}

	virtual void copy_from(tlm_extension_base const &ext) {
		fifo = static_cast<RouterPayloadExtension const &>(ext).fifo;
	}

	void Push(unsigned int id) {
		fifo.push(id);
	}

	bool Empty() const {
		return fifo.empty();
	}

	unsigned int Front() const {
		return fifo.front();
	}

	void Pop() {
		fifo.pop();
	}

private:
	std::queue<unsigned int> fifo;

	RouterPayloadExtension(std::queue<unsigned int> const &fifo_clone) : fifo(fifo_clone) {}
};

template<class CONFIG>
class Router
	: public unisim::kernel::Service<unisim::service::interfaces::Memory<typename CONFIG::ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Memory<typename CONFIG::ADDRESS> >
	, public sc_core::sc_module
{
public:
	typedef typename CONFIG::MAPPING MAPPING;
	static const unsigned int INPUT_SOCKETS = CONFIG::INPUT_SOCKETS;
	static const unsigned int OUTPUT_SOCKETS = CONFIG::OUTPUT_SOCKETS;
	static const unsigned int NUM_MAPPINGS = CONFIG::NUM_MAPPINGS;
	static const unsigned int INPUT_BUSWIDTH = CONFIG::INPUT_BUSWIDTH;
	static const unsigned int OUTPUT_BUSWIDTH = CONFIG::OUTPUT_BUSWIDTH;
	static const bool VERBOSE = CONFIG::VERBOSE;
private:
	typedef unisim::kernel::Object Object;
	typedef typename CONFIG::TYPES TYPES;

	typedef tlm_utils::simple_initiator_socket_tagged<Router, OUTPUT_BUSWIDTH, TYPES> InitSocket;
	typedef tlm_utils::passthrough_target_socket_tagged<Router, INPUT_BUSWIDTH, TYPES> TargSocket;

public:
	SC_HAS_PROCESS(Router);
	Router(const sc_core::sc_module_name &name, Object *parent = 0);
	~Router();

	sc_core::sc_in<bool> output_if_clock; // clock of output (master) interface (init_socket)
	sc_core::sc_in<bool> input_if_clock;  // clock of input (slave) interface (targ_socket)
	
	/** Router initiator port sockets */
	InitSocket *init_socket[OUTPUT_SOCKETS];
	/** Router target port sockets */
	TargSocket *targ_socket[INPUT_SOCKETS];

	/** Incomming memory interface for incomming debugging/loading requests 
	 * One single memory interface for incomming requests is enough, as there is no concurrency */
	unisim::kernel::ServiceExport<unisim::service::interfaces::Memory<typename CONFIG::ADDRESS> > memory_export;
	/** Outgoing memory interfaces (one per output port) for outgoing debugging/loading requests */
	unisim::kernel::ServiceImport<unisim::service::interfaces::Memory<typename CONFIG::ADDRESS> > *memory_import[OUTPUT_SOCKETS];

	virtual void end_of_elaboration();
	virtual void end_of_simulation();
	
	/** Object setup method
	 * This method is required by the inherited Object class.
	 *
	 * @return true if setup is performed with success, false otherwise
	 */
	virtual bool BeginSetup();

protected:
	typedef typename TYPES::tlm_payload_type transaction_type;
	typedef typename TYPES::tlm_phase_type   phase_type;
	typedef tlm::tlm_sync_enum               sync_enum_type;

	/*************************************************************************
	 * Payload fabric                                                  START *
	 *************************************************************************/

	unisim::kernel::tlm2::PayloadFabric<tlm::tlm_generic_payload> payload_fabric;

	/*************************************************************************
	 * Payload fabric                                                    END *
	 *************************************************************************/

	/*************************************************************************
	 * Multi passtrough initiator socket callbacks                     START *
	 *************************************************************************/

	sync_enum_type  I_nb_transport_bw_cb(int id, transaction_type &trans, phase_type &phase, sc_core::sc_time &time);
	void            I_invalidate_direct_mem_ptr_cb(int id, sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	/*************************************************************************
	 * Multi passtrough initiator socket callbacks                       END *
	 *************************************************************************/

	/*************************************************************************
	 * Multi passtrough target socket callbacks                        START *
	 *************************************************************************/

	sync_enum_type	T_nb_transport_fw_cb(int id, transaction_type &trans, phase_type &phase, sc_core::sc_time &time);
	void			T_b_transport_cb(int id, transaction_type &trans, sc_core::sc_time &time);
	unsigned int	T_transport_dbg_cb(int id, transaction_type &trans);
	bool			T_get_direct_mem_ptr_cb(int id, transaction_type &trans, tlm::tlm_dmi &dmi);

	/*************************************************************************
	 * Multi passthrough target socket callbacks                         END *
	 *************************************************************************/

	/*************************************************************************
	 * Ready times for each incomming queue                            START *
	 *************************************************************************/

	std::vector<sc_core::sc_time> m_targ_req_ready;
	std::vector<sc_core::sc_time> m_init_rsp_ready;

	/*************************************************************************
	 * Ready times for each incomming queue                              END *
	 *************************************************************************/

	/*************************************************************************
	 * Dispatch handlers                                               START *
	 *************************************************************************/

	std::vector<RouterDispatcher<Router<CONFIG>, CONFIG> *> m_req_dispatcher;
	std::vector<RouterDispatcher<Router<CONFIG>, CONFIG> *> m_rsp_dispatcher;
	void SendReq(unsigned int id, transaction_type &trans);
	void SendRsp(unsigned int id, transaction_type &trans);

	/*************************************************************************
	 * Dispatch handlers                                                 END *
	 *************************************************************************/

	/*************************************************************************
	 * Helper methods                                                  START *
	 *************************************************************************/

	/**
	 * Apply mapping function over an address range (purpose: simulation; should be overloaded if mapping is dynamic)
	 *
	 * @param addr		the base address
	 * @param size		the size of the range (positive)
	 * @param applied_mapping	the applied mapping pointer
	 * @param mem_rgn	if not zero, a pointer to the memory region protection attributes to be filled when function returns true
	 * @return			true if a map was found, false otherwise
	 */
	virtual bool ApplyMap(uint64_t addr, uint32_t size, MAPPING const *&applied_mapping, MemoryRegion *mem_rgn = 0);
	
	/** 
	 * Apply mapping function over the given transaction (purpose: simulation; should be overloaded for specific behaviors and/or mapping is dynamic)
	 *
	 * @param trans		the transaction to apply the mapping over, transaction response status may be set when function returns false
	 * @param applied_mapping	the applied mapping pointer
	 * @param mem_rgn	if not zero, a pointer to the memory region protection attributes to be filled when function returns true
	 * @return        true if a map was found, false otherwise
	 */
	virtual bool ApplyMap(transaction_type &trans, MAPPING const *&applied_mapping, MemoryRegion *mem_rgn = 0);

	/**
	 * Apply mapping function over an address range and return the mapping division (purpose: debug accesses; should be overloaded if mapping is dynamic)
	 *
	 * @param addr		the base address
	 * @param size		the size of the range (positive)
	 * @param mappings	the mapping pointers that should be applied
	 */
	virtual void ApplyMap(uint64_t addr, uint32_t size, std::vector<MAPPING const *> &mappings);

	/**
	 * Apply mapping function over a given transaction and return the mapping division (purpose: debug accesses; should be overloaded if mapping is dynamic)
	 *
	 * @param trans		the transaction to apply the mapping over
	 * @param mappings	the mapping pointers that should be applied
	 */
	virtual void ApplyMap(const transaction_type &trans, std::vector<MAPPING const *> &mappings);
	
	/**
	 * Set the incomming port into the transaction using the tlm2.0 extension
	 *   mechanism.
	 *
	 * @param  trans  the transaction to mark
	 * @param  port   the incomming port
	 */
	void SetRouterExtension(transaction_type &trans, unsigned int port);

	/**
	 * Get the incomming port from the transaction using the tlm2.0 extension
	 *   mechanism. It removes the port from the transaction.
	 *
	 * @param  trans  the transaction to handle
	 * @param  port   the incomming port found in the transaction extension
	 * @return        true if a transaction extension was found, false otherwise
	 */
	bool GetRouterExtension(transaction_type &trans, unsigned int &port);

	/*************************************************************************
	 * Helper methods                                                    END *
	 *************************************************************************/

	/*************************************************************************
	 * Transport debugging helper methods                              START *
	 *************************************************************************/

	/**
	 * Transport debugging method to handle read requests.
	 *
	 * @param  id    the incomming port identifier
	 * @param  trans the read transaction to handle
	 * @return       the total number of bytes that were read
	 */
	unsigned int ReadTransportDbg(unsigned int id, transaction_type &trans);
	
	/**
	 * Transport debugging method to handle read requests.
	 *
	 * @param  id    the incomming port identifier
	 * @param  trans the read transaction to handle
	 * @return       the total number of bytes that were read
	 */
	unsigned int WriteTransportDbg(unsigned int id, transaction_type &trans);

	/*************************************************************************
	 * Transport debugging helper methods                                END *
	 *************************************************************************/

	/*************************************************************************
	 * Memory interface methods                                        START *
	 *************************************************************************/
	
	/**
	 * Reset memory state (do nothing in our case).
	 */
	virtual void ResetMemory() {}

	/**
	 * Memory interface method to handle read requests.
	 *
	 * @param addr		the address to read
	 * @param buffer	the buffer to fill
	 * @param size		the amount of data to read
	 * @return			true if succeded, false otherwise
	 */
	virtual bool ReadMemory(typename CONFIG::ADDRESS addr, void *buffer, uint32_t size);

	/**
	 * Memory interface method to handle write requests.
	 *
	 * @param addr		the address to write
	 * @param buffer 	the buffer to fill the memory with
	 * @param size		the amount of data to write
	 * @return 			true if succeded, false otherwise
	 */
	virtual bool WriteMemory(typename CONFIG::ADDRESS addr, const void *buffer, uint32_t size);

	/*************************************************************************
	 * Memory interface methods                                          END *
	 *************************************************************************/
	
	/*************************************************************************
	 * Parameters                                                      START *
	 *************************************************************************/

	sc_core::sc_time cycle_time;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_cycle_time;
	std::string input_socket_name[INPUT_SOCKETS];
	unisim::kernel::variable::Parameter<std::string> *param_input_socket_name[INPUT_SOCKETS];
	std::string output_socket_name[OUTPUT_SOCKETS];
	unisim::kernel::variable::Parameter<std::string> *param_output_socket_name[OUTPUT_SOCKETS];

	class MappingTableEntry : public MAPPING
	{
	public:
		MappingTableEntry()
			: MAPPING()
			, next(0)
		{
		}
		
		MappingTableEntry *next;
	};

	std::vector<MappingTableEntry> mapping;
	mutable MappingTableEntry *mru_mapping;
	std::vector<unisim::kernel::variable::Parameter<MAPPING> *> param_mapping;
#if HAVE_TVS
	bool enable_bandwidth_tracing;
	unisim::kernel::variable::Parameter<bool> param_enable_bandwidth_tracing;
	bool verbose_bandwidth_tracing;
	unisim::kernel::variable::Parameter<bool> *param_verbose_bandwidth_tracing;
	sc_core::sc_time bandwidth_tracing_start_time;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_bandwidth_tracing_start_time;
	sc_core::sc_time bandwidth_tracing_end_time;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_bandwidth_tracing_end_time;
#endif
	unisim::kernel::tlm2::ClockPropertiesProxy in_if_clk_prop_proxy;  // proxy to get clock properties from input interface clock port
	unisim::kernel::tlm2::ClockPropertiesProxy out_if_clk_prop_proxy; // proxy to get clock properties from output interface clock port
	unisim::kernel::tlm2::LatencyLookupTable input_lat_lut;  // latency lookup table based on input cycle time: number of cycle -> latency time
	unisim::kernel::tlm2::LatencyLookupTable output_lat_lut; // latency lookup table based on output cycle time: number of cycle -> latency time
	
	void InputInterfaceClockPropertyChangedProcess();
	void OutputInterfaceClockPropertyChangedProcess();

	/*************************************************************************
	 * Parameters                                                        END *
	 *************************************************************************/
	
	/*************************************************************************
	 * DMI                                                             START *
	 *************************************************************************/
	
	unisim::kernel::tlm2::DMIRegionCache dmi_region_cache[INPUT_SOCKETS];
	
	void DMI_Invalidate();
	void DMI_Invalidate(unsigned int output_port);

	/*************************************************************************
	 * DMI                                                               END *
	 *************************************************************************/
	
	
	const sc_core::sc_time& TargetTransferDuration(unsigned int data_length) const { return input_lat_lut.Lookup((data_length + (INPUT_BUSWIDTH / 8) - 1) / (INPUT_BUSWIDTH / 8)); }
	const sc_core::sc_time& InitTransferDuration(unsigned int data_length) const { return output_lat_lut.Lookup((data_length + (OUTPUT_BUSWIDTH / 8) - 1) / (OUTPUT_BUSWIDTH / 8)); }
#if HAVE_TVS
	/*************************************************************************
	 * Tracing                                                         START *
	 *************************************************************************/
	typedef tracing::timed_writer<double, timed_bandwidth_traits> bandwidth_trace_writer_type;
	typedef tracing::timed_value<double> bandwidth_trace_tuple_type;
	sc_core::sc_time init_bw_trace_writer_push_end_time[OUTPUT_SOCKETS];
	sc_core::sc_time targ_bw_trace_writer_push_end_time[INPUT_SOCKETS];
	sc_core::sc_time init_bw_trace_writer_commit_time[OUTPUT_SOCKETS];
	sc_core::sc_time targ_bw_trace_writer_commit_time[INPUT_SOCKETS];
	bandwidth_trace_writer_type *init_bw_trace_writer[OUTPUT_SOCKETS];
	bandwidth_trace_writer_type *targ_bw_trace_writer[INPUT_SOCKETS];
	sc_core::sc_time trace_commit_period;
	sc_core::sc_event trace_commit_event;
	
	template <unsigned int BUSWIDTH> static unsigned int CeilDataLength(unsigned int data_length) { unsigned int mod = data_length % (BUSWIDTH / 8); return mod ? data_length + ((BUSWIDTH / 8) - mod) : data_length; }
	void TraceTargetPortWrite(unsigned int targ_id, unsigned int data_length, const sc_core::sc_time& xfer_start_time, const sc_core::sc_time& xfer_duration);
	void TraceTargetPortRead(unsigned int targ_id, unsigned int data_length, const sc_core::sc_time& xfer_end_time, const sc_core::sc_time& xfer_duration);
	void TraceInitPortWrite(unsigned int init_id, unsigned int data_length, const sc_core::sc_time& xfer_start_time, const sc_core::sc_time& xfer_duration);
	void TraceInitPortRead(unsigned int init_id, unsigned int data_length, const sc_core::sc_time& xfer_end_time, const sc_core::sc_time& xfer_duration);
	void TraceCommitProcess();
	void TraceCommit();
	/*************************************************************************
	 * Tracing                                                           END *
	 *************************************************************************/
#endif
	
	/*************************************************************************
	 * Statistics                                                      START *
	 *************************************************************************/

	/*************************************************************************
	 * Statistics                                                        END *
	 *************************************************************************/

	/*************************************************************************
	 * Logger and verbose parameters                                   START *
	 *************************************************************************/

	mutable unisim::kernel::logger::Logger logger;
	bool verbose_all;
	unisim::kernel::variable::Parameter<bool> *param_verbose_all;
	bool verbose_setup;
	unisim::kernel::variable::Parameter<bool> *param_verbose_setup;
	bool verbose_tlm;
	unisim::kernel::variable::Parameter<bool> *param_verbose_tlm;
	bool verbose_tlm_debug;
	unisim::kernel::variable::Parameter<bool> *param_verbose_tlm_debug;
	bool verbose_memory_interface;
	unisim::kernel::variable::Parameter<bool> *param_verbose_memory_interface;

	/*************************************************************************
	 * Logger and verbose parameters                                     END *
	 *************************************************************************/

	/*************************************************************************
	 * Verbose methods                                                 START *
	 *************************************************************************/

	void SetVerboseAll();
	bool VerboseSetup();
	bool VerboseTLM();
	bool VerboseTLMDebug();
	bool VerboseMemoryInterface();

	/*************************************************************************
	 * Verbose methods                                                   END *
	 *************************************************************************/
};

} // end of namespace unisim
} // end of namespace component
} // end of namespace tlm2
} // end of namespace interconnect
} // end of namespace generic_router

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_HH__
