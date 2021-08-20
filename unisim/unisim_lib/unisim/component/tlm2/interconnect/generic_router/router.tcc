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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_TCC__

#include "unisim/component/tlm2/interconnect/generic_router/router.hh"
#include "unisim/component/tlm2/interconnect/generic_router/router_dispatcher.tcc"
#include "unisim/util/likely/likely.hh"
#include <cmath>
#include <sstream>
#include <iostream>

#define LOCATION 		" - location = " << __FUNCTION__ << ":unisim_lib/unisim/component/tlm2/interconnect/generic_router/router.tcc:" << __LINE__
#define TIME(X)			" - time = " << (X)
#define LOCAL_TIME(X) 	" - time = " << sc_core::sc_time_stamp() + (X)
#define PHASE(X) 		" - phase = " << 	( (X) == tlm::BEGIN_REQ  ? 	"BEGIN_REQ" : \
											( (X) == tlm::END_REQ    ? 	"END_REQ" : \
											( (X) == tlm::BEGIN_RESP ? 	"BEGIN_RESP" : \
											( (X) == tlm::END_RESP   ?  "END_RESP" : \
										  							"UNINITIALIZED_PHASE"))))
#define TRANS(L,X) \
{ \
	(L) << " - trans = " << &(X) << std::endl \
		<< "   - " << ((X).is_read()?"read":"write") << std::endl \
		<< "   - address = 0x" << std::hex << (X).get_address() << std::dec << std::endl \
		<< "   - data_length = " << (X).get_data_length(); \
	if((X).is_write()) { \
		(L) << std::endl; \
		(L) << "   - data ="; \
		for(unsigned int _trans_i = 0; _trans_i < (X).get_data_length(); _trans_i++) { \
			(L) << " " << std::hex << (unsigned int)((X).get_data_ptr()[_trans_i]) << std::dec; \
		} \
	} \
	RouterPayloadExtension *_extension = 0; \
	(X).get_extension(_extension); \
	if(_extension) { \
		(L) << std::endl; \
		(L) << "   - extension(" << _extension << "):" << std::endl; \
		if(_extension->Empty()) \
			(L) << "     - empty" << std::endl; \
		else \
			(L) << "     - input port = " << _extension->Front(); \
	} \
}

#define ETRANS(L,X) \
{ \
	(L) << " - trans = " << &(X) << std::endl \
		<< "   - " << ((X).is_read()?"read":"write") << std::endl \
		<< "   - address = 0x" << std::hex << (X).get_address() << std::dec << std::endl \
		<< "   - data_length = " << (X).get_data_length() << std::endl \
	    << "   - response_status = "; \
	switch((X).get_response_status()) { \
	case tlm::TLM_OK_RESPONSE: \
		(L) << "TLM_OK_RESPONSE"; \
		break; \
	case tlm::TLM_INCOMPLETE_RESPONSE: \
		(L) << "TLM_INCOMPLETE_RESPONSE"; \
		break; \
	case tlm::TLM_GENERIC_ERROR_RESPONSE: \
		(L) << "TLM_GENERIC_ERROR_RESPONSE"; \
		break; \
	case tlm::TLM_ADDRESS_ERROR_RESPONSE: \
		(L) << "TLM_ADDRESS_ERROR_RESPONSE"; \
		break; \
	case tlm::TLM_COMMAND_ERROR_RESPONSE: \
		(L) << "TLM_COMMAND_ERROR_RESPONSE"; \
		break; \
	case tlm::TLM_BURST_ERROR_RESPONSE: \
		(L) << "TLM_BURST_ERROR_RESPONSE"; \
		break; \
	case tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE: \
		(L) << "TLM_BYTE_ENABLE_ERROR_RESPONSE"; \
		break; \
	} \
	if((X).get_response_status() == tlm::TLM_OK_RESPONSE) { \
		(L) << std::endl; \
		(L) << "   - data ="; \
		for(unsigned int _trans_i = 0; _trans_i < (X).get_data_length(); _trans_i++) { \
			(L) << " " << std::hex << (unsigned int)((X).get_data_ptr()[_trans_i]) << std::dec; \
		} \
	} \
	RouterPayloadExtension *_extension = 0; \
	(X).get_extension(_extension); \
	if(_extension) { \
		(L) << std::endl; \
		(L) << "   - extension(" << _extension << "):" << std::endl; \
		if(_extension->Empty()) \
			(L) << "     - empty" << std::endl; \
		else \
			(L) << "     - input port = " << _extension->Front(); \
	} \
}

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace generic_router {

using namespace unisim::kernel::logger;

template <class CONFIG>
const unsigned int Router<CONFIG>::INPUT_SOCKETS;

template <class CONFIG>
const unsigned int Router<CONFIG>::OUTPUT_SOCKETS;

template <class CONFIG>
const unsigned int Router<CONFIG>::NUM_MAPPINGS;

template <class CONFIG>
const unsigned int Router<CONFIG>::INPUT_BUSWIDTH;

template <class CONFIG>
const unsigned int Router<CONFIG>::OUTPUT_BUSWIDTH;

template <class CONFIG>
const bool Router<CONFIG>::VERBOSE;

template<class CONFIG>
Router<CONFIG>::Router(const sc_core::sc_module_name &name, Object *parent)
	: unisim::kernel::Object(name, parent, "A memory-mapped router")
	, unisim::kernel::Service<unisim::service::interfaces::Memory<typename CONFIG::ADDRESS> >(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::Memory<typename CONFIG::ADDRESS> >(name, parent)
	, sc_core::sc_module(name)
	, output_if_clock("output_if_clock")
	, input_if_clock("input_if_clock")
	, init_socket()
	, targ_socket()
	, memory_export("memory-export", this)
	, m_req_dispatcher()
	, m_rsp_dispatcher()
	, cycle_time(sc_core::SC_ZERO_TIME)
	, param_cycle_time("cycle_time", this, cycle_time, "Time to process a request/response by the router")
	, input_socket_name()
	, param_input_socket_name()
	, output_socket_name()
	, param_output_socket_name()
	, mapping(NUM_MAPPINGS)
	, mru_mapping(0)
	, param_mapping(NUM_MAPPINGS)
#if HAVE_TVS
	, enable_bandwidth_tracing(false)
	, param_enable_bandwidth_tracing("enable-bandwidth-tracing", this, enable_bandwidth_tracing, "Enable/Disable bandwidth tracing")
	, verbose_bandwidth_tracing(false)
	, param_verbose_bandwidth_tracing(0)
	, bandwidth_tracing_start_time(sc_core::SC_ZERO_TIME)
	, param_bandwidth_tracing_start_time("bandwidth-tracing-start-time", this, bandwidth_tracing_start_time, "bandwidth tracing start time")
	, bandwidth_tracing_end_time(sc_core::sc_max_time())
	, param_bandwidth_tracing_end_time("bandwidth-tracing-end-time", this, bandwidth_tracing_end_time, "bandwidth tracing end time")
#endif
	, in_if_clk_prop_proxy(input_if_clock)
	, out_if_clk_prop_proxy(output_if_clock)
	, input_lat_lut()
	, output_lat_lut()
	, dmi_region_cache()
#if HAVE_TVS
	, init_bw_trace_writer_push_end_time()
	, targ_bw_trace_writer_push_end_time()
	, init_bw_trace_writer_commit_time()
	, targ_bw_trace_writer_commit_time()
	, init_bw_trace_writer()
	, targ_bw_trace_writer()
	, trace_commit_period(100.0, sc_core::SC_US)
	, trace_commit_event("trace_commit_event")
#endif
	, logger(*this)
	, verbose_all(false)
	, param_verbose_all(0)
	, verbose_setup(false)
	, param_verbose_setup(0)
	, verbose_tlm(false)
	, param_verbose_tlm(0)
	, verbose_tlm_debug(false)
	, param_verbose_tlm_debug(0)
	, verbose_memory_interface(false)
	, param_verbose_memory_interface(0)
{
	param_cycle_time.SetMutable(false);
	
	// initialize default socket names (overridden by parameters)
	for (unsigned int i = 0; i < INPUT_SOCKETS; i++)
	{
		std::stringstream name_sstr;
		name_sstr << "targ_socket_" << i;
		input_socket_name[i] = name_sstr.str();
	}
	
	for (unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
	{
		std::stringstream name_sstr;
		name_sstr << "init_socket_" << i;
		output_socket_name[i] = name_sstr.str();
	}

	if (VERBOSE)
	{
		param_verbose_all = new unisim::kernel::variable::Parameter<bool>("verbose_all", this, verbose_all, "Activate all the verbose options");
		param_verbose_setup = new unisim::kernel::variable::Parameter<bool>("verbose_setup", this, verbose_setup, "Display Object setup information");
		param_verbose_tlm = new unisim::kernel::variable::Parameter<bool>("verbose_tlm", this, verbose_tlm, "Display TLM blocking/non-blocking transactions information");
		param_verbose_tlm_debug = new unisim::kernel::variable::Parameter<bool>("verbose_tlm_debug", this, verbose_tlm_debug, "Display TLM debug transactions information");
		param_verbose_memory_interface = new unisim::kernel::variable::Parameter<bool>("verbose_memory_interface", this, verbose_memory_interface, "Display memory interface operations information");
#if HAVE_TVS
		param_verbose_bandwidth_tracing = new unisim::kernel::variable::Parameter<bool>("verbose-tracing", this, verbose_bandwidth_tracing, "Enable/Disable verbosity while tracing I/O activity over Initiator and Target TLM-2.0 sockets");
#endif
	}

	char const * const mapping_desc =
		"Defined a mapping of the router with format \"[range_start]\",\"[range_end]\",\"[outport_index]\" where [range_start], [range_end] and [outport_index] are to be replaced with the initial address, end address (= range_start + range_size - 1) and the output port index respectively";
		
	for(unsigned int i = 0; i < NUM_MAPPINGS; i++) 
	{
		mapping[i].id = i;
	}
	
	/* instantiate the mapping parameters */
	for(unsigned int i = 0; i < NUM_MAPPINGS; i++) 
	{
		std::stringstream buf;
		buf << "mapping_" << i;
		param_mapping[i] = new unisim::kernel::variable::Parameter<MAPPING>(buf.str().c_str(), this, mapping[i], mapping_desc);
		param_mapping[i]->SetMutable(false);
	}
	
	/* instantiate the input socket name parameters */
	for(unsigned int i = 0; i < INPUT_SOCKETS; i++)
	{
		std::stringstream name_sstr;
		name_sstr << "input_socket_name_" << i;
		std::stringstream desc_sstr;
		desc_sstr << "Name of input socket #" << i;
		param_input_socket_name[i] = new unisim::kernel::variable::Parameter<std::string>(name_sstr.str().c_str(), this, input_socket_name[i], desc_sstr.str().c_str());
		param_input_socket_name[i]->SetMutable(false);
	}
	
	/* instantiate the output socket name parameters */
	for(unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
	{
		std::stringstream name_sstr;
		name_sstr << "output_socket_name_" << i;
		std::stringstream desc_sstr;
		desc_sstr << "Name of output socket #" << i;
		param_output_socket_name[i] = new unisim::kernel::variable::Parameter<std::string>(name_sstr.str().c_str(), this, output_socket_name[i], desc_sstr.str().c_str());
		param_output_socket_name[i]->SetMutable(false);
	}

	/* create target sockets */
	for (unsigned int i = 0; i < INPUT_SOCKETS; i++)
	{
		targ_socket[i] = new TargSocket(input_socket_name[i].c_str());
	}

	/* register target sockets callbacks */
	for (unsigned int i = 0; i < INPUT_SOCKETS; i++)
	{
		(*targ_socket[i]).register_nb_transport_fw(	this, &Router<CONFIG>::T_nb_transport_fw_cb, i);
		(*targ_socket[i]).register_b_transport(		this, &Router<CONFIG>::T_b_transport_cb, i);
		(*targ_socket[i]).register_transport_dbg(		this, &Router<CONFIG>::T_transport_dbg_cb, i);
		(*targ_socket[i]).register_get_direct_mem_ptr(this, &Router<CONFIG>::T_get_direct_mem_ptr_cb, i);
	}
//	/* create target sockets and register socket callbacks */
//	for (unsigned int i = 0; i < MAX_INPUT_SOCKETS; i++)
//	{
//		std::stringstream buf;
//		buf << "targ_socket[" << i << "]";
//		targ_socket[i] = new tlm_utils::passthrough_target_socket_tagged<Router, BUSWIDTH, TYPES>(buf.str().c_str());
//		targ_socket[i]->register_nb_transport_fw(	this, &Router<CONFIG>::T_nb_transport_fw_cb, i);
//		targ_socket[i]->register_b_transport(		this, &Router<CONFIG>::T_b_transport_cb, i);
//		targ_socket[i]->register_transport_dbg(		this, &Router<CONFIG>::T_transport_dbg_cb, i);
//		targ_socket[i]->register_get_direct_mem_ptr(this, &Router<CONFIG>::T_get_direct_mem_ptr_cb, i);
//	}

	// TODO: remove ==>	/* register target multi socket callbacks */
 	// TODO: remove ==>	targ_socket.register_nb_transport_fw(    this, &Router<CONFIG>::T_nb_transport_fw_cb);
 	// TODO: remove ==>	targ_socket.register_b_transport(        this, &Router<CONFIG>::T_b_transport_cb);
 	// TODO: remove ==>	targ_socket.register_transport_dbg(      this, &Router<CONFIG>::T_transport_dbg_cb);
 	// TODO: remove ==>	targ_socket.register_get_direct_mem_ptr( this, &Router<CONFIG>::T_get_direct_mem_ptr_cb);

	/* create initiator sockets */
	for (unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
	{
		init_socket[i] = new InitSocket(output_socket_name[i].c_str());
	}
	
	/* create initiator sockets and register initiator sockets callbacks */
	for (unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
	{
		(*init_socket[i]).register_nb_transport_bw(			this, &Router<CONFIG>::I_nb_transport_bw_cb, i);
		(*init_socket[i]).register_invalidate_direct_mem_ptr(	this, &Router<CONFIG>::I_invalidate_direct_mem_ptr_cb, i);
	}

	/* create memory_imports */
	for (unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
	{
		std::stringstream str;
		str << "memory-import[" << i << "]";
		memory_import[i] = new unisim::kernel::ServiceImport<unisim::service::interfaces::Memory<typename CONFIG::ADDRESS> >(str.str().c_str(), this);
		memory_export.SetupDependsOn(*memory_import[i]);
	}
//	/* create initiator sockets and register socket callbacks */
//	for (unsigned int i = 0; i < MAX_OUTPUT_SOCKETS; i++)
//	{
//		std::stringstream buf;
//		buf << "init_socket[" << i << "]";
//		init_socket[i] = new tlm_utils::simple_initiator_socket_tagged<Router, BUSWIDTH, TYPES>(buf.str().c_str());
//		init_socket[i]->register_nb_transport_bw(			this, &Router<CONFIG>::I_nb_transport_bw_cb, i);
//		init_socket[i]->register_invalidate_direct_mem_ptr(	this, &Router<CONFIG>::I_invalidate_direct_mem_ptr_cb, i);
//	}

	// TODO: remove ==>	/* register initiator socket callbacks */
	// TODO: remove ==>	init_socket.register_nb_transport_bw(           this, &Router<CONFIG>::I_nb_transport_bw_cb);
	// TODO: remove ==>	init_socket.register_invalidate_direct_mem_ptr( this, &Router<CONFIG>::I_invalidate_direct_mem_ptr_cb);

//	SC_THREAD(Dispatch);
	
	for (unsigned int i = 0; i < OUTPUT_SOCKETS; i++) 
	{
		RouterDispatcher<Router<CONFIG>, CONFIG> *dispatcher;
		std::stringstream sstr;
		sstr << "router_request_dispatcher[" << i << "]";
		dispatcher = new RouterDispatcher<Router<CONFIG>, CONFIG>(sstr.str().c_str(), i, this, &Router<CONFIG>::SendReq);
		m_req_dispatcher.push_back(dispatcher);
	}

	for (unsigned int i = 0; i < INPUT_SOCKETS; i++) 
	{
		RouterDispatcher<Router<CONFIG>, CONFIG> *dispatcher;
		std::stringstream sstr;
		sstr << "router_response_dispatcher[" << i << "]";
		dispatcher = new RouterDispatcher<Router<CONFIG>, CONFIG>(sstr.str().c_str(), i, this, &Router<CONFIG>::SendRsp);
		m_rsp_dispatcher.push_back(dispatcher);
	}

#if HAVE_TVS
	/* create timed value streams to record interconnect traffic */
	for(unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
	{
		std::stringstream sstr;
		sstr << output_socket_name[i] << "_bw";
		if(unlikely(verbose_bandwidth_tracing))
		{
			logger << DebugInfo << ": creating Timed-Value Stream \"" << sstr.str() << "\"" << EndDebugInfo;
		}
		init_bw_trace_writer[i] = new bandwidth_trace_writer_type(sstr.str().c_str(), tracing::STREAM_CREATE);
	}
	
	for(unsigned int i = 0; i < INPUT_SOCKETS; i++)
	{
		std::stringstream sstr;
		sstr << input_socket_name[i] << "_bw";
		if(unlikely(verbose_bandwidth_tracing))
		{
			logger << DebugInfo << ": creating Timed-Value Stream \"" << sstr.str() << "\"" << EndDebugInfo;
		}
		targ_bw_trace_writer[i] = new bandwidth_trace_writer_type(sstr.str().c_str(), tracing::STREAM_CREATE);
	}
	
	SC_METHOD(TraceCommitProcess);
	sensitive << trace_commit_event;
#endif
}

template<class CONFIG>
Router<CONFIG>::~Router()
{
#if HAVE_TVS
	for(unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
	{
		delete init_bw_trace_writer[i];
	}
	
	for(unsigned int i = 0; i < INPUT_SOCKETS; i++)
	{
		delete targ_bw_trace_writer[i];
	}
#endif

	typename std::vector<RouterDispatcher<Router<CONFIG>, CONFIG> *>::iterator m_req_dispatcher_iter;

	for (m_req_dispatcher_iter = m_req_dispatcher.begin(); m_req_dispatcher_iter != m_req_dispatcher.end(); m_req_dispatcher_iter++)
	{
		delete *m_req_dispatcher_iter;
	}

	typename std::vector<RouterDispatcher<Router<CONFIG>, CONFIG> *>::iterator m_rsp_dispatcher_iter;

	for (m_rsp_dispatcher_iter = m_rsp_dispatcher.begin(); m_rsp_dispatcher_iter != m_rsp_dispatcher.end(); m_rsp_dispatcher_iter++)
	{
		delete *m_rsp_dispatcher_iter;
	}

	for (unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
	{
		delete init_socket[i];
	}

	for (unsigned int i = 0; i < INPUT_SOCKETS; i++)
	{
		delete targ_socket[i];
	}

	for (unsigned int i = 0; i < NUM_MAPPINGS; i++) 
	{
		delete param_mapping[i];
	}

	for(unsigned int i = 0; i < INPUT_SOCKETS; i++)
	{
		delete param_input_socket_name[i];
	}
	
	for(unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
	{
		delete param_output_socket_name[i];
	}

	for (unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
	{
		delete memory_import[i];
	}

	if (param_verbose_all) delete param_verbose_all;
	if (param_verbose_setup) delete param_verbose_setup;
	if (param_verbose_tlm) delete param_verbose_tlm;
	if (param_verbose_tlm_debug) delete param_verbose_tlm_debug;
	if (param_verbose_memory_interface) delete param_verbose_memory_interface;
#if HAVE_TVS
	if (param_verbose_bandwidth_tracing) delete param_verbose_bandwidth_tracing;
#endif
}

template<class CONFIG>
bool Router<CONFIG>::BeginSetup()
{
	SetVerboseAll();

// 	if (cycle_time == sc_core::SC_ZERO_TIME) 
// 	{
// 		logger << DebugError << "PARAMETER ERROR: the " << param_cycle_time.GetName() << " parameter must be bigger than 0" << std::endl
// 			<< LOCATION << EndDebug;
// 		return false;
// 	}
	
	bool has_mapping = false;
	MappingTableEntry *next = 0;
	for (int i = NUM_MAPPINGS - 1; i >= 0; i--)
	{
		if(mapping[i].used)
		{
			has_mapping = true;
			mapping[i].next = next;
			next = &mapping[i];
		}
	}
	
	mru_mapping = next;
	
	if (NUM_MAPPINGS && !has_mapping) 
	{
		logger << DebugError << "PARAMETER ERROR: no mapping was defined" << std::endl
			<< LOCATION << EndDebug;
		return false;
	}
	for (unsigned int i = 0; i < NUM_MAPPINGS; i++) 
	{
		if (mapping[i].used && (mapping[i].output_port >= OUTPUT_SOCKETS)) 
		{
			logger << DebugWarning << "PARAMETER ERROR: mapping will not be used because it has a bigger output_port than available ports" << std::endl
				<< LOCATION << std::endl;
			logger << "  mapping_" << i << ": " << std::endl;
			logger << "    - start_range = 0x" << std::hex << mapping[i].range_start << std::dec << std::endl;
			logger << "    - end_range   = 0x" << std::hex << mapping[i].range_end << std::dec << std::endl;
			logger << "    - output_port = " << mapping[i].output_port;
			logger << EndDebug;
			mapping[i].used = false;
		}
	}

	/* initialize ready queues */
	for (unsigned int i = 0; i < INPUT_SOCKETS; i++)
		m_targ_req_ready.push_back(sc_core::SC_ZERO_TIME);
	for (unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
		m_init_rsp_ready.push_back(sc_core::SC_ZERO_TIME);
	
	/* display the configuration of the router */
	if (VerboseSetup()) 
	{
		logger << DebugInfo << "Setting cycle_time to " << cycle_time << std::endl;
		logger << "Mappings (NUM_MAPPINGS = " << NUM_MAPPINGS << "): ";
		for (unsigned int i = 0; i < NUM_MAPPINGS; i++) 
		{
			if (mapping[i].used) 
			{
				logger << std::endl << " - " << i << ": start_range = 0x" << std::hex << mapping[i].range_start << std::dec << std::endl;
				logger << "      end_range   = 0x" << std::hex << mapping[i].range_end << std::dec << std::endl;
				logger << "      output_port = " << mapping[i].output_port;
			}
		}
		logger << std::endl;
		logger << "Number of incomming ports = " << INPUT_SOCKETS << std::endl;
		logger << "Number of outgoing ports = " << OUTPUT_SOCKETS << std::endl;
		logger << EndDebug;
	}
	
	return true;
}

template <class CONFIG>
void Router<CONFIG>::end_of_elaboration()
{
	// Spawn InputInterfaceClockPropertyChangedProcess
	sc_core::sc_spawn_options in_if_clk_prop_changed_process_spawn_options;
	in_if_clk_prop_changed_process_spawn_options.spawn_method();
	in_if_clk_prop_changed_process_spawn_options.set_sensitivity(&in_if_clk_prop_proxy.GetClockPropertiesChangedEvent());
	
	sc_core::sc_spawn(sc_bind(&Router<CONFIG>::InputInterfaceClockPropertyChangedProcess, this), "InputInterfaceClockPropertyChangedProcess", &in_if_clk_prop_changed_process_spawn_options);

	// Spawn OutputInterfaceClockPropertyChangedProcess
	sc_core::sc_spawn_options out_if_clk_prop_changed_process_spawn_options;
	out_if_clk_prop_changed_process_spawn_options.spawn_method();
	out_if_clk_prop_changed_process_spawn_options.set_sensitivity(&out_if_clk_prop_proxy.GetClockPropertiesChangedEvent());
	
	sc_core::sc_spawn(sc_bind(&Router<CONFIG>::OutputInterfaceClockPropertyChangedProcess, this), "OutputInterfaceClockPropertyChangedProcess", &out_if_clk_prop_changed_process_spawn_options);
}

template<class CONFIG>
void Router<CONFIG>::end_of_simulation()
{
#if HAVE_TVS
	TraceCommit();
#endif
}

template <class CONFIG>
void Router<CONFIG>::InputInterfaceClockPropertyChangedProcess()
{
	input_lat_lut.SetBaseLatency(in_if_clk_prop_proxy.GetClockPeriod());
}

template <class CONFIG>
void Router<CONFIG>::OutputInterfaceClockPropertyChangedProcess()
{
	output_lat_lut.SetBaseLatency(out_if_clk_prop_proxy.GetClockPeriod());
}

/*************************************************************************
 * Multi passtrough initiator socket callbacks                     START *
 *************************************************************************/

template<class CONFIG>
tlm::tlm_sync_enum Router<CONFIG>::I_nb_transport_bw_cb(int id, transaction_type &trans, phase_type &phase, sc_core::sc_time &time)
{
	switch (phase) {
		case tlm::BEGIN_REQ:
		case tlm::END_RESP:
			logger << DebugError << "Received nb_transport_bw on init_socket[" << id << "], with unexpected phase" << std::endl
				<< LOCAL_TIME(time) << std::endl
				<< PHASE(phase) << std::endl;
			TRANS(logger, trans);
			logger << EndDebug;
			Object::Stop(-1);
			break;
		case tlm::BEGIN_RESP:
			/* a response has been received through the init_socket */
			{
				m_req_dispatcher[id]->Completed(&trans, time);
				if(trans.has_mm()) trans.acquire();
				const sc_core::sc_time& input_if_cycle_time = input_lat_lut.GetBaseLatency();
				const sc_core::sc_time& output_if_cycle_time = output_lat_lut.GetBaseLatency();
				unsigned int data_length = trans.get_data_length();
				const sc_core::sc_time& init_xfer_duration = InitTransferDuration(data_length);
				/* check when the response can be queued into the response queue through the handled port (recovering the router extension)
				 * we must send an end response message */
				unsigned int targ_id;
				if(unlikely(!GetRouterExtension(trans, targ_id)))
				{
					logger << DebugError << "Could not find the router extension from transaction response" << std::endl
						<< LOCATION << std::endl
						<< LOCAL_TIME(time) << std::endl;
					ETRANS(logger, trans);
					logger << EndDebug;
					Object::Stop(-1);
					return tlm::TLM_COMPLETED; // should never occur
				}
				if (unlikely(VerboseTLM()))
				{
					logger << DebugInfo << "Received response through init_socket[" << id << "], queueing it to be sent through targ_socket[" << targ_id << "]"  << std::endl
						<< LOCAL_TIME(time) << std::endl;
					ETRANS(logger, trans);
					logger << EndDebug;
				}
				/* check when the request can be accepted by the router */
				sc_core::sc_time cur_time(sc_core::sc_time_stamp());
				cur_time += time;
				cur_time -= init_xfer_duration; // go back at the time data transfer started
				if (cur_time <= m_init_rsp_ready[id])
				{
					/* the init port is not ready to receive the response, get the time when it will be ready */
					cur_time = m_init_rsp_ready[id];
				}
				else
				{
					/* the inti port is ready for the time the response is received, however we have to make sure
					 *   that the incomming transactions is synchronized with the router cycle_time */
					unisim::kernel::tlm2::AlignToClock(cur_time, output_if_cycle_time);
				}
				cur_time += init_xfer_duration; // advance to the time data transfer ended
				m_init_rsp_ready[id] = cur_time; // take care of bandwidth limit of init port by limiting next transaction acceptance
				
#if HAVE_TVS
				if(unlikely(enable_bandwidth_tracing))
				{
					if(trans.is_write())
					{
						TraceInitPortWrite(id, data_length, cur_time, init_xfer_duration);
					}
					else if(trans.is_read())
					{
						TraceInitPortRead(id, data_length, cur_time, init_xfer_duration);
					}
				}
#endif

				if(unlikely(cycle_time != sc_core::SC_ZERO_TIME))
				{
					if(unlikely(cycle_time != output_if_cycle_time))
					{
						unisim::kernel::tlm2::AlignToClock(cur_time, cycle_time);
					}
					cur_time += cycle_time; // account for latency of router
					if(unlikely(cycle_time != input_if_cycle_time))
					{
						unisim::kernel::tlm2::AlignToClock(cur_time, input_if_cycle_time);
					}
				}
				else
				{
					if(unlikely(input_if_cycle_time != output_if_cycle_time))
					{
						unisim::kernel::tlm2::AlignToClock(cur_time, input_if_cycle_time);
					}
				}

				/* push the response into the response dispatcher */
				time = cur_time;
				time -= sc_core::sc_time_stamp();
				m_rsp_dispatcher[targ_id]->Push(trans, time);
				/* change the phase and signal that the response is completed */
				phase = tlm::END_RESP;
				if(trans.has_mm()) trans.release();
				return tlm::TLM_COMPLETED;
			}
			break;
		case tlm::END_REQ:
			/* just signal that the socket can be used again */
			m_req_dispatcher[id]->Completed(&trans, time);
			return tlm::TLM_COMPLETED;
			break;
	}
	return tlm::TLM_ACCEPTED;
}

template<class CONFIG>
void Router<CONFIG>::I_invalidate_direct_mem_ptr_cb(int id, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	if (unlikely(VerboseTLM())) 
	{
		logger << DebugInfo << "Received invalidate_direct_mem_ptr on port " << id << ", forwarding it" << std::endl;
		logger << "   - start_range = 0x" << std::hex << start_range << std::dec << std::endl;
		logger << "   - end_range = 0x" << std::hex << end_range << std::dec << std::endl;
		logger << EndDebug;
	}

	struct DMI_InvalidatorTagged : public unisim::kernel::tlm2::DMI_InvalidateInterface
	{
		Router<CONFIG> *router;
		int init_id;
		
		DMI_InvalidatorTagged(Router<CONFIG> *_router, int _init_id) : router(_router), init_id(_init_id) {}
		
		virtual void DMI_Invalidate(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
		{
			if (unlikely(router->VerboseTLM())) 
			{
				router->logger << DebugInfo << "Sending invalidate_direct_mem_ptr on port " << init_id << std::endl;
				router->logger << "   - start_range = 0x" << std::hex << start_range << std::dec << std::endl;
				router->logger << "   - end_range = 0x" << std::hex << end_range << std::dec << std::endl;
				router->logger << EndDebug;
			}
			(*router->targ_socket[init_id])->invalidate_direct_mem_ptr(start_range, end_range);
		}
	};
	
	for(unsigned int init_id = 0; init_id < INPUT_SOCKETS; init_id++)
	{
		DMI_InvalidatorTagged dmi_invalidator(this, init_id);
		
		dmi_region_cache[init_id].Invalidate(start_range, end_range, &dmi_invalidator);
	}
}

#if 0
template<class CONFIG>
void Router<CONFIG>::I_invalidate_direct_mem_ptr_cb(int id, sc_dt::uint64 _start_range, sc_dt::uint64 _end_range)
{
	for(MappingTableEntry *mte = mru_mapping; mte != 0; mte = mte->next)
	{
		if(mte->output_port == (unsigned int) id)
		{
			// do reverse translation on DMI invalidation range
			start_range -= mte->translation;
			start_range += mte->range_start;
			end_range -= mte->translation;
			end_range += mte->range_start;

			sc_dt::uint64 mapping_start_addr = mte->range_start;
			sc_dt::uint64 mapping_end_addr = mte->range_end;
			
			if((mapping_end_addr >= start_range) && (mapping_start_addr <= end_range))
			{
				// collision
				if(mapping_start_addr < start_range) mapping_start_addr = start_range; // cut lower region
				if(mapping_end_addr > end_range) mapping_end_addr = end_range; // cut upper region
			
				// invalidate direct memory pointers on all input sockets
				unsigned int i;
				for(i = 0; i < INPUT_SOCKETS; i++)
				{
					(*targ_socket[i])->invalidate_direct_mem_ptr(mapping_start_addr, mapping_end_addr);
				}
			}
		}
	}
}
#endif

/*************************************************************************
 * Multi passtrough initiator socket callbacks                       END *
 *************************************************************************/

/*************************************************************************
 * Multi passtrough target socket callbacks                        START *
 *************************************************************************/

template<class CONFIG>
tlm::tlm_sync_enum Router<CONFIG>::T_nb_transport_fw_cb(int id, transaction_type &trans, phase_type &phase, sc_core::sc_time &time)
{
	if (unlikely(trans.get_command() == tlm::TLM_IGNORE_COMMAND))
	{
		logger << DebugWarning << "Received nb_transport_fw on port " << id << ", with ignore command, which the router doesn't know how to handle" << std::endl
			<< LOCAL_TIME(time) << std::endl
			<< PHASE(phase) << std::endl;
		TRANS(logger, trans);
		logger << EndDebug;
		return tlm::TLM_ACCEPTED;
	}
	switch (phase) {
	case tlm::END_REQ:
	case tlm::BEGIN_RESP:
		logger << DebugError << "Received nb_transport_fw on port " << id << ", with unexpected phase" << std::endl
			<< LOCAL_TIME(time) << std::endl
			<< PHASE(phase) << std::endl;
		TRANS(logger, trans);
		logger << EndDebug;
		Object::Stop(-1);
		break;
	case tlm::BEGIN_REQ:
		{
			if(trans.has_mm()) trans.acquire();
			if (unlikely(VerboseTLM()))
			{
				logger << DebugInfo << "Received nb_transport_fw on port " << id << ", queueing it for future dispatching" << std::endl
					<< LOCAL_TIME(time) << std::endl;
				TRANS(logger, trans);
				logger << EndDebug;
			}
			const sc_core::sc_time& input_if_cycle_time = input_lat_lut.GetBaseLatency();
			const sc_core::sc_time& output_if_cycle_time = output_lat_lut.GetBaseLatency();
			unsigned int data_length = trans.get_data_length();
			const sc_core::sc_time& targ_xfer_duration = TargetTransferDuration(data_length);
			
			/* check when the request can be accepted by the router */
			sc_core::sc_time cur_time(sc_core::sc_time_stamp());
			cur_time += time;
			if (cur_time <= m_targ_req_ready[id]) {
				/* the target port is not ready, get the time when it will be ready */
				cur_time = m_targ_req_ready[id];
			} else {
				/* the target port is ready for the time the request is received, however we have to make sure
				 *   that the incomming transactions is synchronized with the router cycle_time */
				unisim::kernel::tlm2::AlignToClock(cur_time, input_if_cycle_time);
				m_targ_req_ready[id] = cur_time;
			}
			m_targ_req_ready[id] += input_if_cycle_time;
			
			/* get the init port through which the transaction should be forwarded */
			MAPPING const *applied_mapping = 0;
			if (unlikely(!ApplyMap(trans, applied_mapping)))
			{
				time = m_targ_req_ready[id];
				time -= sc_core::sc_time_stamp();
				logger << DebugWarning << "When handling received transaction could not apply any mapping" << std::endl
					<< LOCAL_TIME(time) << std::endl
					<< PHASE(phase) << std::endl
					<< LOCATION << std::endl;
				TRANS(logger, trans);
				logger << EndDebug;
				return tlm::TLM_COMPLETED;
			}

#if HAVE_TVS
			if(unlikely(enable_bandwidth_tracing))
			{
				if(trans.is_write())
				{
					TraceTargetPortWrite(id, data_length, cur_time, targ_xfer_duration);
				}
			}
#endif
			if(unlikely(cycle_time != sc_core::SC_ZERO_TIME))
			{
				if(unlikely(cycle_time != input_if_cycle_time))
				{
					unisim::kernel::tlm2::AlignToClock(cur_time, cycle_time);
				}
				cur_time += cycle_time; // account for latency of router
				if(unlikely(cycle_time != output_if_cycle_time))
				{
					unisim::kernel::tlm2::AlignToClock(cur_time, output_if_cycle_time);
				}
			}
			else
			{
				if(unlikely(input_if_cycle_time != output_if_cycle_time))
				{
					unisim::kernel::tlm2::AlignToClock(cur_time, output_if_cycle_time);
				}
			}
			
			if(trans.is_write())
			{
				m_targ_req_ready[id] += targ_xfer_duration; // take care of bandwidth limit of target port by limiting next transaction acceptance
				m_targ_req_ready[id] -= input_if_cycle_time; // accounting for request over input interface is already included in transfer duration for a write
			}

			/* perform the address translation */
			uint64_t translated_addr = trans.get_address();
			translated_addr -= applied_mapping->range_start;
			translated_addr += applied_mapping->translation;
			trans.set_address(translated_addr);

			/* checking command */
			/* insert the input port id into the transaction */
			SetRouterExtension(trans, id);
			/* push the transaction to the corresponding init port queue */
			time = cur_time;
			time -= sc_core::sc_time_stamp();
			m_req_dispatcher[applied_mapping->output_port]->Push(trans, time);
			if(trans.has_mm()) trans.release();
			/* change the phase and return */
			phase = tlm::END_REQ;
			return tlm::TLM_UPDATED;
		}
		break;
	case tlm::END_RESP:
		/* just signal that the socket can be used again */
		m_rsp_dispatcher[id]->Completed(&trans, time);
		/* the transaction is now finished for us, release it */
		if(trans.has_mm()) trans.release();
		return tlm::TLM_COMPLETED;
		break;
	}
	return tlm::TLM_ACCEPTED;
}

#if HAVE_TVS
template <class CONFIG>
void Router<CONFIG>::TraceTargetPortWrite(unsigned int targ_id, unsigned int data_length, const sc_core::sc_time& xfer_start_time, const sc_core::sc_time& xfer_duration)
{
	sc_core::sc_time xfer_end_time(xfer_start_time);
	xfer_end_time += xfer_duration;
	
	if((xfer_start_time >= bandwidth_tracing_start_time) && (xfer_end_time <= bandwidth_tracing_end_time))
	{
		if(xfer_start_time >= targ_bw_trace_writer_push_end_time[targ_id])
		{
			sc_core::sc_time xfer_start_time_offset(xfer_start_time);
			xfer_start_time_offset -= targ_bw_trace_writer_push_end_time[targ_id];
			
			targ_bw_trace_writer_push_end_time[targ_id] = xfer_end_time;
			bandwidth_trace_tuple_type value(CeilDataLength<INPUT_BUSWIDTH>(data_length) / xfer_duration.to_seconds(), xfer_duration);
			if(unlikely(verbose_bandwidth_tracing))
			{
				logger << DebugInfo << "targ port #" << targ_id << " transfer:" << xfer_start_time << " -> " << xfer_duration << ":push(" << xfer_start_time_offset << ", " << value << ");" << EndDebugInfo;
			}
			targ_bw_trace_writer[targ_id]->push(xfer_start_time_offset, value);
			if(unlikely(verbose_bandwidth_tracing))
			{
				logger << DebugInfo << "targ port #" << targ_id << " transfer:commit(" << xfer_end_time << ");" << EndDebugInfo;
			}
			targ_bw_trace_writer[targ_id]->commit(xfer_end_time);
			targ_bw_trace_writer_commit_time[targ_id] = xfer_end_time;
		}
	}
}

template <class CONFIG>
void Router<CONFIG>::TraceTargetPortRead(unsigned int targ_id, unsigned int data_length, const sc_core::sc_time& xfer_end_time, const sc_core::sc_time& xfer_duration)
{
	if(xfer_end_time >= xfer_duration)
	{
		sc_core::sc_time xfer_start_time(xfer_end_time);
		xfer_start_time -= xfer_duration;

		if((xfer_start_time >= bandwidth_tracing_start_time) && (xfer_end_time <= bandwidth_tracing_end_time))
		{
			if(xfer_start_time >= targ_bw_trace_writer_push_end_time[targ_id])
			{
				sc_core::sc_time xfer_start_time_offset(xfer_start_time);
				xfer_start_time_offset -= targ_bw_trace_writer_push_end_time[targ_id];

				targ_bw_trace_writer_push_end_time[targ_id] = xfer_end_time;
				bandwidth_trace_tuple_type value(CeilDataLength<INPUT_BUSWIDTH>(data_length) / xfer_duration.to_seconds(), xfer_duration);
				if(unlikely(verbose_bandwidth_tracing))
				{
					logger << DebugInfo << "targ port #" << targ_id << " transfer:" << xfer_start_time << " -> " << xfer_duration << ":push(" << xfer_start_time_offset << ", " << value << ");" << EndDebugInfo;
				}
				targ_bw_trace_writer[targ_id]->push(xfer_start_time_offset, value);
				
				if(unlikely(verbose_bandwidth_tracing))
				{
					logger << DebugInfo << "targ port #" << targ_id << " transfer:commit(" << xfer_end_time << ");" << EndDebugInfo;
				}
				targ_bw_trace_writer[targ_id]->commit(xfer_end_time);
				targ_bw_trace_writer_commit_time[targ_id] = xfer_end_time;
			}
		}
	}
}

template <class CONFIG>
void Router<CONFIG>::TraceInitPortWrite(unsigned int init_id, unsigned int data_length, const sc_core::sc_time& xfer_start_time, const sc_core::sc_time& xfer_duration)
{
	sc_core::sc_time xfer_end_time(xfer_start_time);
	xfer_end_time += xfer_duration;
	
	if((xfer_start_time >= bandwidth_tracing_start_time) && (xfer_end_time <= bandwidth_tracing_end_time))
	{
		if(xfer_start_time >= init_bw_trace_writer_push_end_time[init_id])
		{
			sc_core::sc_time xfer_start_time_offset(xfer_start_time);
			xfer_start_time_offset -= init_bw_trace_writer_push_end_time[init_id];

			init_bw_trace_writer_push_end_time[init_id] = xfer_end_time;
			bandwidth_trace_tuple_type value(CeilDataLength<OUTPUT_BUSWIDTH>(data_length) / xfer_duration.to_seconds(), xfer_duration);
			if(unlikely(verbose_bandwidth_tracing))
			{
				logger << DebugInfo << "init port #" << init_id << " transfer:" << xfer_start_time << " -> " << xfer_duration << ":push(" << xfer_start_time_offset << ", " << value << ");" << EndDebugInfo;
			}
			init_bw_trace_writer[init_id]->push(xfer_start_time_offset, value);
			if(unlikely(verbose_bandwidth_tracing))
			{
				logger << DebugInfo << "init port #" << init_id << " transfer:commit(" << xfer_end_time << ");" << EndDebugInfo;
			}
			init_bw_trace_writer[init_id]->commit(xfer_end_time);
			init_bw_trace_writer_commit_time[init_id] = xfer_end_time;
		}
	}
}

template <class CONFIG>
void Router<CONFIG>::TraceInitPortRead(unsigned int init_id, unsigned int data_length, const sc_core::sc_time& xfer_end_time, const sc_core::sc_time& xfer_duration)
{
	if(xfer_end_time >= xfer_duration)
	{
		sc_core::sc_time xfer_start_time(xfer_end_time);
		xfer_start_time -= xfer_duration;
		
		if((xfer_start_time >= bandwidth_tracing_start_time) && (xfer_end_time <= bandwidth_tracing_end_time))
		{
			if(xfer_start_time >= init_bw_trace_writer_push_end_time[init_id])
			{
				sc_core::sc_time xfer_start_time_offset(xfer_start_time);
				xfer_start_time_offset -= init_bw_trace_writer_push_end_time[init_id];
				
				init_bw_trace_writer_push_end_time[init_id] = xfer_end_time;
				bandwidth_trace_tuple_type value(CeilDataLength<OUTPUT_BUSWIDTH>(data_length) / xfer_duration.to_seconds(), xfer_duration);
				if(unlikely(verbose_bandwidth_tracing))
				{
					logger << DebugInfo << "init port #" << init_id << " transfer:" << xfer_start_time << " -> " << xfer_duration << ":push(" << xfer_start_time_offset << ", " << value << ");" << EndDebugInfo;
				}
				init_bw_trace_writer[init_id]->push(xfer_start_time_offset, value);
				if(unlikely(verbose_bandwidth_tracing))
				{
					logger << DebugInfo << "init port #" << init_id << " transfer:commit(" << xfer_end_time << ");" << EndDebugInfo;
				}
				init_bw_trace_writer[init_id]->commit(xfer_end_time);
				init_bw_trace_writer_commit_time[init_id] = xfer_end_time;
			}
		}
	}
}

template <class CONFIG>
void Router<CONFIG>::TraceCommitProcess()
{
	TraceCommit();
}

template <class CONFIG>
void Router<CONFIG>::TraceCommit()
{
	const sc_core::sc_time& time_stamp(sc_core::sc_time_stamp());
	
	for(unsigned int i = 0; i < INPUT_SOCKETS; i++)
	{
		if(time_stamp > targ_bw_trace_writer_commit_time[i])
		{
			if(time_stamp > targ_bw_trace_writer_push_end_time[i])
			{
				sc_core::sc_time dur(time_stamp);
				dur -= targ_bw_trace_writer_push_end_time[i];
				bandwidth_trace_tuple_type value(0, dur);
				if(unlikely(verbose_bandwidth_tracing))
				{
					logger << DebugInfo << "targ port #" << i << " transfer:push(" << value << ");" << EndDebugInfo;
				}
				targ_bw_trace_writer[i]->push(value);
				targ_bw_trace_writer_push_end_time[i] = time_stamp;
			}
			if(unlikely(verbose_bandwidth_tracing))
			{
				logger << DebugInfo << "targ port #" << i << " transfer:commit(" << time_stamp << ");" << EndDebugInfo;
			}
			targ_bw_trace_writer[i]->commit(time_stamp);
			targ_bw_trace_writer_commit_time[i] = time_stamp;
		}
	}
	for(unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
	{
		if(time_stamp > init_bw_trace_writer_commit_time[i])
		{
			if(time_stamp > init_bw_trace_writer_push_end_time[i])
			{
				sc_core::sc_time dur(time_stamp);
				dur -= init_bw_trace_writer_push_end_time[i];
				bandwidth_trace_tuple_type value(0, dur);
				if(unlikely(verbose_bandwidth_tracing))
				{
					logger << DebugInfo << "init port #" << i << " transfer:push(" << value << ");" << EndDebugInfo;
				}
				init_bw_trace_writer[i]->push(value);
				init_bw_trace_writer_push_end_time[i] = time_stamp;
			}
			if(unlikely(verbose_bandwidth_tracing))
			{
				logger << DebugInfo << "init port #" << i << " transfer:commit(" << time_stamp << ");" << EndDebugInfo;
			}
			init_bw_trace_writer[i]->commit(time_stamp);
			init_bw_trace_writer_commit_time[i] = time_stamp;
		}
	}
	
	trace_commit_event.notify(trace_commit_period);
}

#endif

template<class CONFIG>
void Router<CONFIG>::T_b_transport_cb(int id, transaction_type &trans, sc_core::sc_time &time)
{
	/* the first thing that must be done is the translation from the mapping table */
	if (unlikely(VerboseTLM())) 
	{
		logger << DebugInfo << "Received b_transport on port " << id << ", forwarding it" << std::endl
			<< LOCAL_TIME(time) << std::endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}

	const sc_core::sc_time& input_if_cycle_time = input_lat_lut.GetBaseLatency();
	const sc_core::sc_time& output_if_cycle_time = output_lat_lut.GetBaseLatency();
	unsigned int data_length = trans.get_data_length();
	const sc_core::sc_time& targ_xfer_duration = TargetTransferDuration(data_length);
	const sc_core::sc_time& init_xfer_duration = InitTransferDuration(data_length);

	/* check when the request can be accepted by the router */
	sc_core::sc_time cur_time(sc_core::sc_time_stamp());
	cur_time += time;

	if (cur_time <= m_targ_req_ready[id])
	{
		/* the target port is not ready, get the time when it will be ready */
		cur_time = m_targ_req_ready[id];
	}
	else
	{
		/* the target port is ready for the time the request is received, however we have to make sure
			*   that the incomming transactions is synchronized with the router cycle_time */
		unisim::kernel::tlm2::AlignToClock(cur_time, input_if_cycle_time);
		m_targ_req_ready[id] = cur_time;
	}
	m_targ_req_ready[id] += input_if_cycle_time; // take care of bandwidth limit of target port by limiting next transaction acceptance: here accounting for request over input interface
	
	/* check the address of the transaction to perform the port routing */
	MAPPING const *applied_mapping = 0;
	if (unlikely(!ApplyMap(trans, applied_mapping))) 
	{
		time = m_targ_req_ready[id];
		time -= sc_core::sc_time_stamp();
		logger << DebugWarning << "Received transaction on port " << id << " has an unmapped address"  << std::endl
			<< LOCATION << std::endl
			<< TIME(cur_time) << std::endl;
		TRANS(logger, trans);
		logger << EndDebug;
		return;
	}
	
#if HAVE_TVS
	if(unlikely(enable_bandwidth_tracing))
	{
		if(trans.is_write())
		{
			TraceTargetPortWrite(id, data_length, cur_time, targ_xfer_duration);
		}
	}
#endif

	if(unlikely(cycle_time != sc_core::SC_ZERO_TIME))
	{
		if(unlikely(cycle_time != input_if_cycle_time))
		{
			unisim::kernel::tlm2::AlignToClock(cur_time, cycle_time);
		}
		cur_time += cycle_time; // account for latency of router
		if(unlikely(cycle_time != output_if_cycle_time))
		{
			unisim::kernel::tlm2::AlignToClock(cur_time, output_if_cycle_time);
		}
	}
	else
	{
		if(unlikely(input_if_cycle_time != output_if_cycle_time))
		{
			unisim::kernel::tlm2::AlignToClock(cur_time, output_if_cycle_time);
		}
	}
	
	unsigned int init_id = applied_mapping->output_port;
	
	if(trans.is_write())
	{
		m_targ_req_ready[id] += targ_xfer_duration; // take care of bandwidth limit of target port by limiting next transaction acceptance
		m_targ_req_ready[id] -= input_if_cycle_time; // accounting for request over input interface is already included in transfer duration for a write
	}
	
	/* perform the address translation */
	uint64_t translated_addr = trans.get_address();
	translated_addr -= applied_mapping->range_start;
	translated_addr += applied_mapping->translation;
	if (unlikely(VerboseTLM() && (translated_addr != trans.get_address())))
	{
		logger << DebugInfo << "Performing address translation on transaction:" << std::endl;
		TRANS(logger, trans);
		logger << std::endl
			<< "--> address translated to 0x" << std::hex << translated_addr << std::dec
			<< EndDebug;
	}
	trans.set_address(translated_addr);
	/* forward the transaction to the selected output port */
	if(unlikely(VerboseTLM()))
	{
		logger << DebugInfo << "Forwarding transaction received on port " << id << " to port " << applied_mapping->output_port << std::endl
			<< TIME(cur_time) << std::endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	
	sc_core::sc_time next(cur_time);
	next += init_xfer_duration;
	time = cur_time;
	time -= sc_core::sc_time_stamp();
	(*init_socket[applied_mapping->output_port])->b_transport(trans, time);
	cur_time = sc_core::sc_time_stamp();
	cur_time += time;
	if(unlikely(cur_time < next)) cur_time = next; // make sure transfer is not too fast for our interface
	if (unlikely(VerboseTLM())) 
	{
		logger << DebugInfo << "Forwarding transaction reply to port " << id << std::endl
			<< TIME(cur_time) << std::endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	
	cur_time -= init_xfer_duration; // go back at the time data transfer started
	if (cur_time <= m_init_rsp_ready[init_id])
	{
		/* the init port is not ready to receive the response, get the time when it will be ready */
		cur_time = m_init_rsp_ready[init_id];
	}
	else
	{
		/* the init port is ready for the time the response is received, however we have to make sure
		 *   that the incomming transactions is synchronized with the router cycle_time */
		unisim::kernel::tlm2::AlignToClock(cur_time, output_if_cycle_time);
	}
	cur_time += init_xfer_duration; // advance to the time data transfer ended
	m_init_rsp_ready[init_id] = cur_time; // take care of bandwidth limit of init port by limiting next transaction acceptance
	
#if HAVE_TVS
	if(unlikely(enable_bandwidth_tracing))
	{
		if(trans.is_read())
		{
			TraceInitPortRead(init_id, data_length, cur_time, init_xfer_duration);
		}
		else if(trans.is_write())
		{
			TraceInitPortWrite(init_id, data_length, cur_time, init_xfer_duration);
		}
	}
#endif

	if(unlikely(cycle_time != sc_core::SC_ZERO_TIME))
	{
		if(unlikely(cycle_time != output_if_cycle_time))
		{
			unisim::kernel::tlm2::AlignToClock(cur_time, cycle_time);
		}
		cur_time += cycle_time; // account for latency of router
		if(unlikely(cycle_time != input_if_cycle_time))
		{
			unisim::kernel::tlm2::AlignToClock(cur_time, input_if_cycle_time);
		}
	}
	else
	{
		if(unlikely(input_if_cycle_time != output_if_cycle_time))
		{
			unisim::kernel::tlm2::AlignToClock(cur_time, input_if_cycle_time);
		}
	}

#if HAVE_TVS
	if(unlikely(enable_bandwidth_tracing))
	{
		if(trans.is_read())
		{
			TraceTargetPortRead(id, data_length, cur_time, targ_xfer_duration);
		}
	}
#endif

	time = cur_time;
	time -= sc_core::sc_time_stamp();
}

template<class CONFIG>
unsigned int Router<CONFIG>::T_transport_dbg_cb(int id, transaction_type &trans) 
{
	if (unlikely(VerboseTLMDebug()))
	{
		logger << DebugInfo << "Received transport_dbg on port " << id << " forwarding it" << std::endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	// only handle reads and writes
	if (!trans.is_read() && !trans.is_write()) return 0;

	// modify request queues and send the transaction trhough the init_sockets
	unsigned int counter = 0;
	unsigned char *trans_initial_buffer = trans.get_data_ptr();
	unsigned char *trans_buffer = trans.get_data_ptr();
	unsigned int trans_size = trans.get_data_length();
	sc_dt::uint64 trans_addr = trans.get_address();
	std::vector<MAPPING const *> mappings;
	typename std::vector<MAPPING const *>::iterator it;
	ApplyMap(trans, mappings);
	for(it = mappings.begin(); it != mappings.end(); it++)  {
		sc_dt::uint64 buffer_index;
		sc_dt::uint64 buffer_addr;
		unsigned int buffer_size;
		MAPPING const *applied_mapping = *it;
		if (applied_mapping->range_start > trans_addr)
		{
			buffer_addr = applied_mapping->range_start;
			buffer_index = applied_mapping->range_start - trans_addr;
			if (applied_mapping->range_end >= trans_addr + trans_size - 1)
				buffer_size = 1 + (trans_addr + trans_size - 1) - applied_mapping->range_start;
			else
				buffer_size = 1 + applied_mapping->range_end - applied_mapping->range_start;
		}
		else
		{
			buffer_addr = trans_addr;
			buffer_index = 0;
			if (applied_mapping->range_end >= trans_addr + trans_size - 1)
				buffer_size = trans_size;
			else
				buffer_size = 1 + applied_mapping->range_end - trans_addr;
		}
		trans.set_data_ptr(trans_buffer + buffer_index);
		trans.set_data_length(buffer_size);
		uint64_t translated_addr = buffer_addr - applied_mapping->range_start + applied_mapping->translation;
		trans.set_address(translated_addr);
		if (unlikely(VerboseTLMDebug()))
		{
			logger << DebugInfo << "Sending transport_dbg on port " << applied_mapping->output_port << std::endl;
			TRANS(logger, trans);
			logger << EndDebug;
		}
		counter += (*init_socket[applied_mapping->output_port])->transport_dbg(trans);
		if (trans.is_read())
			m_req_dispatcher[applied_mapping->output_port]->ReadTransportDbg(id, trans);
		else
			m_req_dispatcher[applied_mapping->output_port]->WriteTransportDbg(id, trans);
		// if the request is a write then be sure to modify the response queue
		if (trans.is_write())
			m_rsp_dispatcher[id]->WriteTransportDbg(id, trans);
	}
	if (unlikely(VerboseTLMDebug()))
	{
		if (trans.is_read())
		{
			logger << DebugInfo << "Finished transport_dbg reading " << counter << " bytes" << std::endl
				<< "  with read data = " << std::hex;
			for (unsigned int i = 0; i < counter; i++)
			{
				logger << (unsigned int)trans_initial_buffer[i] << " ";
			}
			logger << EndDebug;
		}
		else
		{
			logger << DebugInfo << "Finished transport_dbg writing " << counter << "bytes" << EndDebug;
		}
	}
	return counter;
}

template<class CONFIG>
unsigned int Router<CONFIG>::ReadTransportDbg(unsigned int id, transaction_type &trans)
{
	unsigned int counter = 0;
	unsigned char *trans_buffer = trans.get_data_ptr();
	unsigned int trans_size = trans.get_data_length();
	sc_dt::uint64 trans_addr = trans.get_address();
	std::vector<MAPPING const *> mappings;
	typename std::vector<MAPPING const *>::iterator it;
	ApplyMap(trans, mappings);
	for(it = mappings.begin(); it != mappings.end(); it++)
	{
		sc_dt::uint64 buffer_index;
		sc_dt::uint64 buffer_addr;
		unsigned int buffer_size;
		MAPPING const *applied_mapping = *it;
		if (applied_mapping->range_start > trans_addr)
		{
			buffer_addr = applied_mapping->range_start;
			buffer_index = applied_mapping->range_start - trans_addr;
			if (applied_mapping->range_end >= trans_addr + trans_size - 1)
				buffer_size = 1 + (trans_addr + trans_size - 1) - applied_mapping->range_start;
			else
				buffer_size = 1 + applied_mapping->range_end - applied_mapping->range_start;
		}
		else
		{
			buffer_addr = trans_addr;
			buffer_index = 0;
			if (applied_mapping->range_end >= trans_addr + trans_size - 1)
				buffer_size = trans_size;
			else
				buffer_size = 1 + applied_mapping->range_end - trans_addr;
		}
		trans.set_data_ptr(trans_buffer + buffer_index);
		trans.set_data_length(buffer_size);
		uint64_t translated_addr = buffer_addr - applied_mapping->range_start + applied_mapping->translation;
		trans.set_address(translated_addr);
		counter += (*init_socket[applied_mapping->output_port])->transport_dbg(trans);
		m_req_dispatcher[applied_mapping->output_port]->ReadTransportDbg(id, trans);	
	}
	logger << DebugInfo << "ReadTransportDbg of address 0x" << std::hex << trans_addr << std::dec
			<< " with size " << trans_size << ", " << counter << " bytes successfully read" << std::endl;
	return counter;
}

template<class CONFIG>
unsigned int Router<CONFIG>::WriteTransportDbg(unsigned int id, transaction_type &trans)
{
	unsigned int counter = 0;
	unsigned char *trans_buffer = trans.get_data_ptr();
	unsigned int trans_size = trans.get_data_length();
	sc_dt::uint64 trans_addr = trans.get_address();
	std::vector<MAPPING const *> mappings;
	typename std::vector<MAPPING const *>::iterator it;
	ApplyMap(trans, mappings);
	for(it = mappings.begin(); it != mappings.end(); it++)
	{
		sc_dt::uint64 buffer_index;
		sc_dt::uint64 buffer_addr;
		unsigned int buffer_size;
		MAPPING const *applied_mapping = *it;
		if (applied_mapping->range_start > trans_addr)
		{
			buffer_addr = applied_mapping->range_start;
			buffer_index = applied_mapping->range_start - trans_addr;
			if (applied_mapping->range_end >= trans_addr + trans_size - 1)
				buffer_size = 1 + (trans_addr + trans_size - 1) - applied_mapping->range_start;
			else
				buffer_size = 1 + applied_mapping->range_end - applied_mapping->range_start;
		}
		else
		{
			buffer_addr = trans_addr;
			buffer_index = 0;
			if (applied_mapping->range_end >= trans_addr + trans_size - 1)
				buffer_size = trans_size; 
			else
				buffer_size = 1 + applied_mapping->range_end - trans_addr;
		}
		trans.set_data_ptr(trans_buffer + buffer_index);
		trans.set_data_length(buffer_size);
		uint64_t translated_addr = buffer_addr - applied_mapping->range_start + applied_mapping->translation;
		trans.set_address(translated_addr);
		counter += (*init_socket[applied_mapping->output_port])->transport_dbg(trans);
		m_req_dispatcher[applied_mapping->output_port]->WriteTransportDbg(id, trans);	
	}
	return counter;
}

template <class CONFIG>
bool Router<CONFIG>::ReadMemory(typename CONFIG::ADDRESS addr, void *buffer, uint32_t size)
{
	if (unlikely(VerboseMemoryInterface()))
	{
		logger << DebugInfo << "Received memory interface ReadMemory with address=0x" << std::hex << addr << std::dec
			<< " and size=" << size
			<< ", forwarding it" << EndDebug;
	}
	std::vector<MAPPING const *> mappings;
	typename std::vector<MAPPING const *>::iterator it;
	ApplyMap(addr, size, mappings);
	if(unlikely(!mappings.size())) return false;

	for(it = mappings.begin(); it != mappings.end(); it++)
	{
		sc_dt::uint64 buffer_index;
		sc_dt::uint64 buffer_addr;
		unsigned int buffer_size;
		MAPPING const *applied_mapping = *it;
		if (applied_mapping->range_start > addr) {
			buffer_addr = applied_mapping->range_start;
			buffer_index = applied_mapping->range_start - addr;
			if (applied_mapping->range_end >= addr + size - 1)
				buffer_size = 1 + (addr + size - 1) - applied_mapping->range_start;
			else
				buffer_size = 1 + applied_mapping->range_end - applied_mapping->range_start;
		}
		else
		{
			buffer_addr = addr;
			buffer_index = 0;
			if (applied_mapping->range_end >= addr + size - 1)
				buffer_size = size;
			else
				buffer_size = 1 + applied_mapping->range_end - addr;
		}
		uint64_t translated_addr = buffer_addr - applied_mapping->range_start + applied_mapping->translation;
		if (unlikely(VerboseMemoryInterface()))
		{
			logger << DebugInfo << "Sending ReadMemory on port " << applied_mapping->output_port << " with addr 0x" << std::hex << buffer_addr << std::dec
				<< "(translated to 0x" << std::hex << translated_addr << std::dec << ")"
				<< " and size " << buffer_size << "(" << size << ")" << EndDebugInfo;
		}
		if (unlikely(!(*memory_import[applied_mapping->output_port]))) return false;
		if (unlikely(!(*memory_import[applied_mapping->output_port])->ReadMemory(translated_addr, ((uint8_t *)buffer) + buffer_index, buffer_size)))
			return false;
	}
	return true;
}

template <class CONFIG>
bool Router<CONFIG>::WriteMemory(typename CONFIG::ADDRESS addr, const void *buffer, uint32_t size)
{
	if (unlikely(VerboseMemoryInterface()))
	{
		logger << DebugInfo << "Received memory interface WriteMemory with address=0x" << std::hex << addr << std::dec
			<< " and size=" << size
			<< ", forwarding it" << EndDebug;
	}
	std::vector<MAPPING const *> mappings;
	typename std::vector<MAPPING const *>::iterator it;
	ApplyMap(addr, size, mappings);
	if(unlikely(!mappings.size())) return false;

	for(it = mappings.begin(); it != mappings.end(); it++)
	{
		sc_dt::uint64 buffer_index;
		sc_dt::uint64 buffer_addr;
		unsigned int buffer_size;
		MAPPING const *applied_mapping = *it;
		if (applied_mapping->range_start > addr) {
			buffer_addr = applied_mapping->range_start;
			buffer_index = applied_mapping->range_start - addr;
			if (applied_mapping->range_end >= addr + size - 1)
				buffer_size = 1 + (addr + size - 1) - applied_mapping->range_start;
			else
				buffer_size = 1 + applied_mapping->range_end - applied_mapping->range_start;
		}
		else
		{
			buffer_addr = addr;
			buffer_index = 0;
			if (applied_mapping->range_end >= addr + size - 1)
				buffer_size = size;
			else
				buffer_size = 1 + applied_mapping->range_end - addr;
		}
		uint64_t translated_addr = buffer_addr - applied_mapping->range_start + applied_mapping->translation;
		if (unlikely(VerboseMemoryInterface()))
		{
			logger << DebugInfo << "Sending WriteMemory on port " << applied_mapping->output_port << " with addr 0x" << std::hex << addr << std::dec
				<< " (translated to 0x" << std::hex << translated_addr << std::dec << ")"
				<< " and size " << buffer_size << "(" << size << ")" << std::endl
				<< "  with data = " << std::hex;
			for (unsigned int i = 0; i < buffer_size; i++)
			{
				logger << (unsigned int)(((uint8_t *)buffer)[buffer_index + i]) << " ";
			}
			logger << std::dec << EndDebugInfo;
		}
		if (unlikely(!(*memory_import[applied_mapping->output_port]))) return false;
		if (unlikely(!(*memory_import[applied_mapping->output_port])->WriteMemory(translated_addr, ((uint8_t *)buffer) + buffer_index, buffer_size)))
			return false;
	}
	return true;
}

template<class CONFIG>
bool Router<CONFIG>::T_get_direct_mem_ptr_cb(int id, transaction_type &trans, tlm::tlm_dmi &dmi)
{
	/* the first thing that must be done is the translation from the mapping table */
	if (unlikely(VerboseTLM())) 
	{
		logger << DebugInfo << "Received get_direct_mem_ptr on port " << id << ", forwarding it" << std::endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	/* check the address of the transaction to perform the port routing */
	MAPPING const *applied_mapping = 0;
	MemoryRegion mem_rgn;
	bool found = ApplyMap(trans, applied_mapping, &mem_rgn);
	if (unlikely(!found)) 
	{
		logger << DebugError << "Received get_direct_mem_ptr transaction on port " << id << " has an unmapped address"  << std::endl
			<< LOCATION << std::endl;
		TRANS(logger, trans);
		logger << EndDebug;
		dmi.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
		dmi.set_start_address(trans.get_address());
		dmi.set_end_address(trans.get_address() + trans.get_data_length() - 1);
		return false;
	}
	/* perform the address translation */
	uint64_t translated_addr = trans.get_address();
	translated_addr -= applied_mapping->range_start;
	translated_addr += applied_mapping->translation;
	if (unlikely(VerboseTLM() && (translated_addr != trans.get_address())))
	{
		logger << DebugInfo << "Performing address translation on transaction:" << std::endl;
		TRANS(logger, trans);
		logger << std::endl
			<< "--> address translated to 0x" << std::hex << translated_addr << std::dec
			<< EndDebug;
	}
	trans.set_address(translated_addr);
	/* forward the transaction to the selected output port */
	if(unlikely(VerboseTLM()))
	{
		logger << DebugInfo << "Forwarding get_direct_mem_ptr received on port " << id << " to port " << applied_mapping->output_port << std::endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	
	bool dmi_status = (*init_socket[applied_mapping->output_port])->get_direct_mem_ptr(trans, dmi);
	
	sc_dt::uint64 dmi_start_address = dmi.get_start_address();
	sc_dt::uint64 dmi_end_address = dmi.get_end_address();
	
	// do reverse translation on DMI
	dmi_start_address -= applied_mapping->translation;
	dmi_start_address += applied_mapping->range_start;
	dmi_end_address -= applied_mapping->translation;
	dmi_end_address += applied_mapping->range_start;

	// restrict address range of DMI
	sc_dt::uint64 start_range = std::max(mem_rgn.start_addr, applied_mapping->range_start);
	sc_dt::uint64 end_range = std::min(mem_rgn.end_addr, applied_mapping->range_end);

	if(dmi_end_address >= dmi_start_address) // prevent us from crazy targets behavior
	{
		if(dmi_start_address < start_range)
		{
			// cut lower region
			dmi.set_dmi_ptr(dmi.get_dmi_ptr() + (start_range - dmi_start_address));
			dmi.set_start_address(start_range);
		}
		else
		{
			dmi.set_start_address(dmi_start_address);
		}
			
		if(dmi_end_address > end_range)
		{
			// cut upper region
			dmi.set_end_address(end_range);
		}
		else
		{
			dmi.set_end_address(dmi_end_address);
		}
		
		tlm::tlm_dmi::dmi_access_e dmi_access = dmi.get_granted_access();
		switch(dmi_access)
		{
			case tlm::tlm_dmi::DMI_ACCESS_NONE:
				break;
			case tlm::tlm_dmi::DMI_ACCESS_READ:
				if((mem_rgn.mem_access & MEM_ACCESS_READ) == 0) dmi.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_NONE);
				break;
			case tlm::tlm_dmi::DMI_ACCESS_WRITE:
				if((mem_rgn.mem_access & MEM_ACCESS_WRITE) == 0) dmi.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_NONE);
				break;
			case tlm::tlm_dmi::DMI_ACCESS_READ_WRITE:
				if(mem_rgn.mem_access != MEM_ACCESS_READ_WRITE)
				{
					if((mem_rgn.mem_access & MEM_ACCESS_READ) != 0) dmi.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ);
					else if((mem_rgn.mem_access & MEM_ACCESS_WRITE) != 0) dmi.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_WRITE);
				}
				break;
		}
	}
	else
	{
		// deny all crazy target address space
		dmi.set_start_address(start_range);
		dmi.set_end_address(end_range);
		dmi.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_NONE);
		dmi_status = false;
	}

	// add router latency per byte
	if(dmi_status)
	{
		dmi.set_read_latency(dmi.get_read_latency() + (2 * cycle_time / ((CONFIG::BURST_LENGTH * CONFIG::INPUT_BUSWIDTH) / 8)));
		dmi.set_write_latency(dmi.get_write_latency() + (2 * cycle_time / ((CONFIG::BURST_LENGTH * CONFIG::INPUT_BUSWIDTH) / 8)));
	}

	unisim::kernel::tlm2::DMIGrant dmi_grant = dmi_status ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
	dmi_region_cache[id].Insert(dmi_grant, dmi);
#if 0
	sc_dt::uint64 dmi_start_address = dmi.get_start_address();
	sc_dt::uint64 dmi_end_address = dmi.get_end_address();
	
	// do reverse translation on DMI
	dmi_start_address -= applied_mapping->translation;
	dmi_start_address += applied_mapping->range_start;
	dmi_end_address -= applied_mapping->translation;
	dmi_end_address += applied_mapping->range_start;

	// restrict address range of DMI
	sc_dt::uint64 start_range = std::max(mem_rgn.start_addr, applied_mapping->range_start);
	sc_dt::uint64 end_range = std::min(mem_rgn.end_addr, applied_mapping->range_end);

	if(dmi_end_address >= dmi_start_address) // prevent us from crazy targets behavior
	{
		if(dmi_start_address < start_range)
		{
			// cut lower region
			dmi.set_dmi_ptr(dmi.get_dmi_ptr() + (start_range - dmi_start_address));
			dmi.set_start_address(start_range);
		}
		else
		{
			dmi.set_start_address(dmi_start_address);
		}
			
		if(dmi_end_address > end_range)
		{
			// cut upper region
			dmi.set_end_address(end_range);
		}
		else
		{
			dmi.set_end_address(dmi_end_address);
		}
		
		tlm::tlm_dmi::dmi_access_e dmi_access = dmi.get_granted_access();
		switch(dmi_access)
		{
			case tlm::tlm_dmi::DMI_ACCESS_NONE:
				break;
			case tlm::tlm_dmi::DMI_ACCESS_READ:
				if((mem_rgn.mem_access & MEM_ACCESS_READ) == 0) dmi.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_NONE);
				break;
			case tlm::tlm_dmi::DMI_ACCESS_WRITE:
				if((mem_rgn.mem_access & MEM_ACCESS_WRITE) == 0) dmi.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_NONE);
				break;
			case tlm::tlm_dmi::DMI_ACCESS_READ_WRITE:
				if(mem_rgn.mem_access != MEM_ACCESS_READ_WRITE)
				{
					if((mem_rgn.mem_access & MEM_ACCESS_READ) != 0) dmi.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ);
					else if((mem_rgn.mem_access & MEM_ACCESS_WRITE) != 0) dmi.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_WRITE);
				}
				break;
		}
	}
	else
	{
		// deny all crazy target address space
		dmi.set_start_address(start_range);
		dmi.set_end_address(end_range);
		dmi.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_NONE);
		dmi_status = false;
	}

	// add router latency per byte
	if(dmi_status)
	{
		dmi.set_read_latency(dmi.get_read_latency() + (2 * cycle_time / ((CONFIG::BURST_LENGTH * CONFIG::INPUT_BUSWIDTH) / 8)));
		dmi.set_write_latency(dmi.get_write_latency() + (2 * cycle_time / ((CONFIG::BURST_LENGTH * CONFIG::INPUT_BUSWIDTH) / 8)));
	}
#endif
	return dmi_status;
}

/*************************************************************************
 * Multi passthrough target socket callbacks                         END *
 *************************************************************************/

/*************************************************************************
 * Dispatch handlers                                               START *
 *************************************************************************/

template<class CONFIG>
void Router<CONFIG>::SendReq(unsigned int id, transaction_type &trans)
{
	/* a request is ready to be sent through the init_port
	 *   prepare phase and time */
	phase_type phase = tlm::BEGIN_REQ;
	sc_core::sc_time time(sc_core::SC_ZERO_TIME);
	
	if (unlikely(VerboseTLM()))
	{
		logger << DebugInfo << "Sending transaction request through init_socket[" << id << "]" << std::endl
			<< LOCAL_TIME(time) << std::endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	
	switch((*init_socket[id])->nb_transport_fw(trans, phase, time))
	{
		case tlm::TLM_ACCEPTED:
			/* the request has been accepted */
			if (unlikely(VerboseTLM()))
			{
				logger << DebugInfo << "Transaction request sent from init_req_fifo[" << id << "] accepted (TLM_ACCEPTED), waiting for the corresponding END_REQ" << std::endl
					<< LOCAL_TIME(time) << std::endl;
				TRANS(logger, trans);
				logger << EndDebug;
			}
			/* nothing else to do, just wait for the corresponding END_REQ */
			break;
		case tlm::TLM_UPDATED:
			/* check if the phase has changed */
			switch(phase) {
				case tlm::BEGIN_REQ:
					/* phase has not changed, consider it as a TLM_ACCEPTED */
					if (unlikely(VerboseTLM()))
					{
						logger << DebugInfo << "Transaction request sent from init_req_fifo[" << id << "] accepted (TLM_UPDATED, BEGIN_REQ), waiting for the corresponding END_REQ" << std::endl
							<< LOCAL_TIME(time) << std::endl;
						TRANS(logger, trans);
						logger << EndDebug;
					}
					/* nothing else to do, just wait for the corresponding END_REQ */
					break;
				case tlm::END_REQ:
					if (unlikely(VerboseTLM()))
					{
						logger << DebugInfo << "Transaction sent from init_req_fifo[" << id << "] accepted (TLM_UPDATED with phase END_REQ), removing the transaction from the request queue" << std::endl
							<< LOCAL_TIME(time) << std::endl;
						TRANS(logger, trans);
						logger << EndDebug;
					}
					m_req_dispatcher[id]->Completed(&trans, time);
					break;	
				case tlm::BEGIN_RESP:
					/* the request has been accepted, and the response has been produced
					 * check when the response can be queued into the response queue through the handled port (recovering the router extension)
					 * we must send an end response message */
					{
						const sc_core::sc_time& input_if_cycle_time = input_lat_lut.GetBaseLatency();
						const sc_core::sc_time& output_if_cycle_time = output_lat_lut.GetBaseLatency();
						unsigned int data_length = trans.get_data_length();
						const sc_core::sc_time& init_xfer_duration = InitTransferDuration(data_length);
						
						unsigned int targ_id;
						/* notify to the request dispatcher that the transaction request has been completed */
						m_req_dispatcher[id]->Completed(&trans, time);
						if(unlikely(!GetRouterExtension(trans, targ_id)))
						{
							logger << DebugError << "Could not find the router extension from transaction response" << std::endl
								<< LOCATION << std::endl
								<< LOCAL_TIME(time) << std::endl;
							ETRANS(logger, trans);
							logger << EndDebug;
							Object::Stop(-1);
							return; // should never occur
						}
						if (unlikely(VerboseTLM())) {
							logger << DebugInfo << "Transaction request sent through the init_port[" << id << "] accepted and response received (TLM_UPDATED, BEGIN_REQ), queueing the response to be sent through the targ_socket[" << targ_id << "]" << std::endl
								<< LOCAL_TIME(time) << std::endl;
							ETRANS(logger, trans);
							logger << EndDebug;
						}
						/* check when the request can be accepted by the router */
						sc_core::sc_time cur_time(sc_core::sc_time_stamp());
						cur_time += time;
						cur_time -= init_xfer_duration; // go back at the time data transfer started
						if (cur_time <= m_init_rsp_ready[id])
						{
							/* the init port is not ready to receive the response, get the time when it will be ready */
							cur_time = m_init_rsp_ready[id];
						}
						else
						{
							/* the init port is ready for the time the response is received, however we have to make sure
							 *   that the incomming transactions is synchronized with the router cycle_time */
							unisim::kernel::tlm2::AlignToClock(cur_time, output_if_cycle_time);
						}
						cur_time += init_xfer_duration; // advance to the time data transfer ended
						m_init_rsp_ready[id] = cur_time; // take care of bandwidth limit of init port by limiting next transaction acceptance
						
#if HAVE_TVS
						if(unlikely(enable_bandwidth_tracing))
						{
							if(trans.is_read())
							{
								TraceInitPortRead(id, data_length, cur_time, init_xfer_duration);
							}
							else if(trans.is_write())
							{
								TraceInitPortWrite(id, data_length, cur_time, init_xfer_duration);
							}
						}
#endif
						if(unlikely(cycle_time != sc_core::SC_ZERO_TIME))
						{
							cur_time += cycle_time; // account for latency of router
							if(unlikely(cycle_time != input_if_cycle_time))
							{
								unisim::kernel::tlm2::AlignToClock(cur_time, cycle_time);
							}
						}
						else
						{
							if(unlikely(input_if_cycle_time != output_if_cycle_time))
							{
								unisim::kernel::tlm2::AlignToClock(cur_time, input_if_cycle_time);
							}
						}
						
						/* push the response into the response dispatcher */
						time = cur_time;
						time -= sc_core::sc_time_stamp();
						m_rsp_dispatcher[targ_id]->Push(trans, time);

						/* immediately send an END_RESP through the init_port */
						phase = tlm::END_RESP;
						if(unlikely((*init_socket[id])->nb_transport_fw(trans, phase, time) != tlm::TLM_COMPLETED)) {
							logger << DebugError << "When sending END_RESP did not receive TLM_COMPLETED from init_socket[" << id << "]" << std::endl
								<< LOCATION << std::endl
								<< LOCAL_TIME(time) << std::endl;
							ETRANS(logger, trans);
							logger << EndDebug;
							Object::Stop(-1);
						}
					}
					break;
				case tlm::END_RESP:
					/* unexpected phase changes */
					logger << DebugError << "Unexpected phase change when sending request transaction through init_socket[" << id << "]" << std::endl
						<< LOCATION << std::endl
						<< LOCAL_TIME(time) << std::endl
						<< PHASE(phase) << std::endl;
					TRANS(logger, trans);
					logger << EndDebug;
					Object::Stop(-1);
					break;
			}
			break;
		case tlm::TLM_COMPLETED:
			/* the init_port has completely handled the transaction request
			 * if the request was a write we can remove the transaction
			 * if the request was a read we have to forward the response to the corresponding targ_port */
			{
				const sc_core::sc_time& input_if_cycle_time = input_lat_lut.GetBaseLatency();
				const sc_core::sc_time& output_if_cycle_time = output_lat_lut.GetBaseLatency();
				unsigned int data_length = trans.get_data_length();
				const sc_core::sc_time& init_xfer_duration = InitTransferDuration(data_length);
				
				/* notify to the request dispatcher that the transaction request has been completed */
				m_req_dispatcher[id]->Completed(&trans, time);
				unsigned int targ_id;
				if(unlikely(!GetRouterExtension(trans, targ_id)))
				{
					logger << DebugError << "Could not find the router extension from transaction response" << std::endl
						<< LOCATION << std::endl
						<< LOCAL_TIME(time) << std::endl;
					ETRANS(logger, trans);
					logger << EndDebug;
					Object::Stop(-1);
					return; // should never occur
				}
				if (unlikely(VerboseTLM()))
				{
					logger << DebugInfo << "Transaction request sent through the init_port[" << id << "] accepted and response received (TLM_UPDATED, BEGIN_REQ), queueing the response to be sent through the targ_socket[" << targ_id << "]" << std::endl
						<< LOCAL_TIME(time) << std::endl;
					ETRANS(logger, trans);
					logger << EndDebug;
				}
				/* check when the request can be accepted by the router */
				sc_core::sc_time cur_time(sc_core::sc_time_stamp());
				cur_time += time;
				cur_time -= init_xfer_duration; // go back at the time data transfer started
				if (cur_time <= m_init_rsp_ready[id]) {
					/* the init port is not ready to receive the response, get the time when it will be ready */
					cur_time = m_init_rsp_ready[id];
				} else {
					/* the init port is ready for the time the response is received, however we have to make sure
						*   that the incomming transactions is synchronized with the router cycle_time */
					unisim::kernel::tlm2::AlignToClock(cur_time, output_if_cycle_time);
				}
				cur_time += init_xfer_duration; // advance to the time data transfer ended
				m_init_rsp_ready[id] = cur_time; // take care of bandwidth limit of init port by limiting next transaction acceptance
				
#if HAVE_TVS
				if(unlikely(enable_bandwidth_tracing))
				{
					if(trans.is_read())
					{
						TraceInitPortRead(id, data_length, cur_time, init_xfer_duration);
					}
					else if(trans.is_write())
					{
						TraceInitPortWrite(id, data_length, cur_time, init_xfer_duration);
					}
				}
#endif

				if(unlikely(cycle_time != sc_core::SC_ZERO_TIME))
				{
					if(unlikely(cycle_time != output_if_cycle_time))
					{
						unisim::kernel::tlm2::AlignToClock(cur_time, cycle_time);
					}
					cur_time += cycle_time; // account for latency of router
					if(unlikely(cycle_time != input_if_cycle_time))
					{
						unisim::kernel::tlm2::AlignToClock(cur_time, input_if_cycle_time);
					}
				}
				else
				{
					if(unlikely(input_if_cycle_time != output_if_cycle_time))
					{
						unisim::kernel::tlm2::AlignToClock(cur_time, input_if_cycle_time);
					}
				}

				/* push the response into the response dispatcher */
				time = cur_time;
				time -= sc_core::sc_time_stamp();
				m_rsp_dispatcher[targ_id]->Push(trans, time);
			}
			break;
	}
}

template<class CONFIG>
void Router<CONFIG>::SendRsp(unsigned int id, transaction_type &trans)
{
	/* a response is ready to be sent through the targ_port[id]
	 *   prepare phase and time */
	phase_type phase = tlm::BEGIN_RESP;
	sc_core::sc_time time(sc_core::SC_ZERO_TIME);
	
	if (unlikely(VerboseTLM()))
	{
		logger << DebugInfo << "Sending transaction response through targ_socket[" << id << "]" << std::endl
			<< LOCAL_TIME(time) << std::endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	
#if HAVE_TVS
	if(unlikely(enable_bandwidth_tracing))
	{
		if(trans.is_read())
		{
			sc_core::sc_time cur_time(sc_core::sc_time_stamp());
			unsigned int data_length = trans.get_data_length();
			const sc_core::sc_time& targ_xfer_duration = TargetTransferDuration(data_length);
			TraceTargetPortRead(id, data_length, cur_time, targ_xfer_duration);
		}
	}
#endif
	switch((*targ_socket[id])->nb_transport_bw(trans, phase, time)) {
		case tlm::TLM_ACCEPTED:
			/* the response has been accepted */
			if (unlikely(VerboseTLM()))
			{
				logger << DebugInfo << "Transaction response sent through targ_socket[" << id << "] accepted (TLM_ACCEPTED), waiting for the corresponding END_RESP" << std::endl
					<< LOCAL_TIME(time) << std::endl;
				ETRANS(logger, trans);
				logger << EndDebug;
			}
			/* nothing else to do, just wait for the corresponding END_REQ */
			break;
		case tlm::TLM_UPDATED:
			logger << DebugError << "Unexpected TLM_UPDATED received when sending transaction response through targ_socket[" << id << "]" << std::endl
				<< LOCATION << std::endl
				<< LOCAL_TIME(time) << std::endl
				<< PHASE(phase) << std::endl;
			ETRANS(logger, trans);
			logger << EndDebug;
			break;
		case tlm::TLM_COMPLETED:
			if (unlikely(VerboseTLM()))
			{
				logger << DebugInfo << "Transaction response sent through targ_socket[" << id << "] completed (TLM_COMPLETED), removing it from the router" << std::endl
					<< LOCAL_TIME(time) << std::endl;
				ETRANS(logger, trans);
				logger << EndDebug;
			}
			/* the response has been completed, we can remove it from the response queue */
			m_rsp_dispatcher[id]->Completed(&trans, time);
			/* release the transaction */
			break;
	}
}

/*************************************************************************
 * Dispatch handlers                                                 END *
 *************************************************************************/

/*************************************************************************
 * Helper methods                                                  START *
 *************************************************************************/

template <class CONFIG>
bool Router<CONFIG>::ApplyMap(uint64_t addr, uint32_t size, MAPPING const *& applied_mapping, MemoryRegion *mem_rgn)
{
	for(MappingTableEntry *mte = mru_mapping, *prev_mte = 0; mte != 0; prev_mte = mte, mte = mte->next)
	{
		if(likely((addr >= mte->range_start) && ((addr + size - 1) <= mte->range_end)))
		{
			applied_mapping = mte;
			if(unlikely(mte != mru_mapping))
			{
				prev_mte->next = mte->next;
				mte->next = mru_mapping;
				mru_mapping = mte;
			}

			if(unlikely(mem_rgn))
			{
				mem_rgn->start_addr = applied_mapping->range_start;
				mem_rgn->end_addr = applied_mapping->range_end;
				mem_rgn->mem_access = MEM_ACCESS_READ_WRITE;
			}
			return true;
		}
	}
	return false;
}

template <class CONFIG>
bool Router<CONFIG>::ApplyMap(transaction_type &trans, MAPPING const *& applied_mapping, MemoryRegion *mem_rgn)
{
	uint64_t address = trans.get_address();
	unsigned int size = trans.get_data_length();
	if(unlikely(!ApplyMap(address, size, applied_mapping, mem_rgn)))
	{
		trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
		return false;
	}
	return true;
}

template <class CONFIG>
void Router<CONFIG>::ApplyMap(uint64_t addr, uint32_t size, std::vector<MAPPING const *> &port_mappings)
{
	sc_dt::uint64 cur_addr = addr;
	unsigned int cur_size = size;
	while (cur_size)
	{
		// try to find a mapping which address range contains current address
		bool found = false;
		for(MappingTableEntry *mte = mru_mapping; mte != 0; mte = mte->next)
		{
			if ((cur_addr >= mte->range_start) && (cur_addr <= mte->range_end))
			{
				// found a mapping which address range contains current address
				found = true;
				port_mappings.push_back(mte);

				sc_dt::uint64 range_width_minus_1 = mte->range_end - cur_addr;
				if(((cur_size - 1) > range_width_minus_1)) // partially covered
				{
					sc_dt::uint64 next_addr = mte->range_end + 1;
					if(cur_addr > next_addr) return; // detect address overflow
					cur_addr = next_addr;
					cur_size = cur_size - (range_width_minus_1 + 1);
				}
				else // totally covered
				{
					sc_dt::uint64 next_addr = cur_addr + cur_size;
					if(cur_addr > next_addr) return; // detect address overflow
					cur_addr = next_addr;
					cur_size = 0;
					break;
				}
			}
		}
		
		if (!found)
		{
			// no mapping found for current address
			
			// try to fast skip addresses until hitting a valid mapping
			sc_dt::uint64 closest_range_start = 0;
			for (MappingTableEntry *mte = mru_mapping; mte != 0; mte = mte->next)
			{
				if ((cur_addr < mte->range_start) && ((cur_addr + cur_size) > mte->range_start))
				{
					if (!found)
					{
						closest_range_start = mte->range_start;
					}
					else
					{
						if (closest_range_start > mte->range_start)
						{
							closest_range_start = mte->range_start;
						}
					}
					found = true;
				}
			}
			if (!found) return;
			
			// hitting a valid mapping: skip addresses and restart at mapping range start address
			cur_size = cur_size - (closest_range_start - cur_addr);
			cur_addr = closest_range_start;
		}
	}
}

template<class CONFIG>
void Router<CONFIG>::ApplyMap(const transaction_type &trans, std::vector<MAPPING const *> &port_mappings)
{
	sc_dt::uint64 addr = trans.get_address();
	unsigned int size = trans.get_data_length();
	ApplyMap(addr, size, port_mappings);
}

template <typename CONFIG>
void Router<CONFIG>::DMI_Invalidate()
{
	for(unsigned int output_port = 0; output_port < OUTPUT_SOCKETS; output_port++)
	{
		DMI_Invalidate(output_port);
	}
}

template <typename CONFIG>
void Router<CONFIG>::DMI_Invalidate(unsigned int output_port)
{
	I_invalidate_direct_mem_ptr_cb(output_port, 0, ~sc_dt::uint64(0));
}

template<class CONFIG>
void Router<CONFIG>::SetRouterExtension(transaction_type &trans, unsigned int port) 
{
	RouterPayloadExtension *router_extension;
	trans.get_extension(router_extension);
	if(!router_extension)
	{
		router_extension = new RouterPayloadExtension();
		router_extension->Push(port);
		trans.set_extension(router_extension);
	}
	else
	{
		router_extension->Push(port);
	}
}

template<class CONFIG>
bool Router<CONFIG>::GetRouterExtension(transaction_type &trans, unsigned int &port)
{
	RouterPayloadExtension *router_extension;
	trans.get_extension(router_extension);
	if(!router_extension) return false;
	if(router_extension->Empty()) return false;
	port = router_extension->Front();
	router_extension->Pop();
	return true;
}

/*************************************************************************
 * Helper methods                                                    END *
 *************************************************************************/

/*************************************************************************
 * Verbose methods                                                 START *
 *************************************************************************/

template <class CONFIG>
void Router<CONFIG>::SetVerboseAll()
{
	if (VERBOSE && verbose_all)
	{
		verbose_setup = true;
		verbose_tlm = true;
		verbose_tlm_debug = true;
		verbose_memory_interface = true;
	}
}

template<class CONFIG>
bool Router<CONFIG>::VerboseSetup() 
{
	return VERBOSE && verbose_setup;
}

template<class CONFIG>
bool Router<CONFIG>::VerboseTLM() 
{
	return VERBOSE && verbose_tlm;
}

template <class CONFIG>
bool Router<CONFIG>::VerboseTLMDebug() 
{
	return VERBOSE && verbose_tlm_debug;
}

template <class CONFIG>
bool Router<CONFIG>::VerboseMemoryInterface()
{
	return VERBOSE && verbose_memory_interface;
}

/*************************************************************************
 * Verbose methods                                                   END *
 *************************************************************************/

} // end of namespace generic_router
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_TCC__
