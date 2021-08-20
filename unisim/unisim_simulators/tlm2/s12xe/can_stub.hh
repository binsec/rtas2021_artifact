/*
 *  Copyright (c) 2008, 2015
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
 * Authors: Gilles	Mouchard <gilles.mouchard@cea.fr>
 * 			Reda	Nouacer  <reda.nouacer@cea.fr>
 */


#ifndef CAN_STUB_HH_
#define CAN_STUB_HH_


#include <systemc>
#include <inttypes.h>

#include <iostream>
#include <fstream>
#include <queue>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/parser.h>

#include <tlm.h>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>

#include <unisim/kernel/kernel.hh>

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>

using namespace std;

using namespace sc_core;
using namespace sc_dt;
using namespace tlm;
using namespace tlm_utils;

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::ServiceExportBase;

using unisim::component::cxx::processor::hcs12x::CONFIG;

using unisim::component::tlm2::processor::hcs12x::CAN_Payload;
//using unisim::component::tlm2::processor::hcs12x::CAN_DATATYPE;
//using unisim::component::tlm2::processor::hcs12x::CAN_DATATYPE_ARRAY;
using unisim::component::tlm2::processor::hcs12x::UNISIM_CAN_ProtocolTypes;

using unisim::kernel::tlm2::PayloadFabric;

class CAN_STUB :
	public Object
	, public sc_module

	, virtual public tlm_fw_transport_if<UNISIM_CAN_ProtocolTypes >
	, virtual public tlm_bw_transport_if<UNISIM_CAN_ProtocolTypes >

{
public:
	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, UNISIM_CAN_ProtocolTypes, 0> can_tx_sock;  // binded to RX pin of the CAN (master)

	tlm_target_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, UNISIM_CAN_ProtocolTypes, 0> can_rx_sock;  // binded to TX pin of the CAN (slave)

	CAN_STUB(const sc_module_name& name, Object *parent = 0);
	~CAN_STUB();

	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	virtual void Stop(int exit_status);

	bool isTerminated() { return terminated; }

	// Slave methods
	virtual bool get_direct_mem_ptr( CAN_Payload& payload, tlm_dmi&  dmi_data);
	virtual unsigned int transport_dbg( CAN_Payload& payload);

	virtual tlm_sync_enum nb_transport_fw( CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport( CAN_Payload& payload, sc_core::sc_time& t);
	// Master methods
	virtual tlm_sync_enum nb_transport_bw( CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr( sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	// Implementation
	void observe(CAN_DATATYPE &msg);
	void inject(CAN_DATATYPE msg);

	virtual void Get_CAN(CAN_DATATYPE *msg);
	virtual void Inject_CAN(CAN_DATATYPE msg);

	virtual void Inject_CANArray(CAN_DATATYPE_ARRAY msg);
	virtual void getCANArray(CAN_DATATYPE_ARRAY *msg);

	void processCAN_Inject();
	void processCAN_Observe();
	void watchdog();

	int RandomizeData(std::vector<CAN_DATATYPE* > &vect);
	int LoadXmlData(const char *filename, std::vector<CAN_DATATYPE* > &vect);
	void parseRow (xmlDocPtr doc, xmlNodePtr cur, CAN_DATATYPE &data);


protected:

	int bw_inject_count;
	sc_event can_bw_event;
	bool dont_care_bw_event;
	sc_event time_out_event;
	sc_event watchdog_enable_event;
	sc_time watchdog_delay;

	bool trace_enable;
	Parameter<bool> param_trace_enable;

	bool	cosim_enabled;
	Parameter<bool>		param_cosim_enabled;

	bool	xml_enabled;
	Parameter<bool>		param_xml_enabled;

	bool	rand_enabled;
	Parameter<bool>		param_rand_enabled;

private:

	peq_with_get<CAN_Payload > input_payload_queue;

	PayloadFabric<CAN_Payload > can_inject_payload_fabric;

	double	can_inject_stimulus_period;
	Parameter<double>	param_can_inject_stimulus_period;
	sc_time *can_inject_stimulus_period_sc;

	string can_inject_stimulus_file;
	Parameter<string>	param_can_inject_stimulus_file;

	bool broadcast_enabled;
	Parameter<bool>	param_broadcast_enabled;

	ofstream can_inject_output_file;
	ofstream can_observe_output_file;

	bool terminated;


	std::vector<CAN_DATATYPE* > can_inject_vect;
	std::vector<CAN_DATATYPE* > can_observe_vect;
};

#endif /* CAN_STUB_HH_ */
