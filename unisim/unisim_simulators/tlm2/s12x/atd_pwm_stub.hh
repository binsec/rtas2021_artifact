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
 * Authors: Gilles	Mouchard <gilles.mouchard@cea.fr>
 * 			Reda	Nouacer  <reda.nouacer@cea.fr>
 */


#ifndef ATD_PWM_STUB_HH_
#define ATD_PWM_STUB_HH_


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

#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>

using namespace std;

using namespace sc_core;
using namespace sc_dt;
using namespace tlm;
using namespace tlm_utils;

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::ServiceExportBase;

using unisim::component::tlm2::processor::hcs12x::PWM_Payload;
using unisim::component::tlm2::processor::hcs12x::ATD_Payload;
using unisim::component::tlm2::processor::hcs12x::UNISIM_PWM_ProtocolTypes;
using unisim::component::tlm2::processor::hcs12x::UNISIM_ATD_ProtocolTypes;

using unisim::kernel::tlm2::PayloadFabric;

#define ATD1_SIZE	16
#define ATD0_SIZE	8
#define PWM_SIZE	8


static const unsigned int UNISIM2RTB_BUS_WIDTH = 32; // in bits (unused in this example)
static const unsigned int RTB2UNISIM_BUS_WIDTH = 32; // in bits (unused in this example)

class ATD_PWM_STUB :
	public Object,
	public sc_module,

	virtual public tlm_fw_transport_if<UNISIM_PWM_ProtocolTypes<PWM_SIZE> >,
	virtual public tlm_bw_transport_if<UNISIM_ATD_ProtocolTypes<ATD0_SIZE> >,
	virtual public tlm_bw_transport_if<UNISIM_ATD_ProtocolTypes<ATD1_SIZE> >
{
public:
	tlm_initiator_socket<RTB2UNISIM_BUS_WIDTH, UNISIM_ATD_ProtocolTypes<ATD1_SIZE> > atd1_master_sock;
	tlm_initiator_socket<RTB2UNISIM_BUS_WIDTH, UNISIM_ATD_ProtocolTypes<ATD0_SIZE> > atd0_master_sock;

	tlm_target_socket<UNISIM2RTB_BUS_WIDTH, UNISIM_PWM_ProtocolTypes<PWM_SIZE> > slave_sock;

	ATD_PWM_STUB(const sc_module_name& name, Object *parent = 0);
	~ATD_PWM_STUB();

	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	virtual void Stop(int exit_status);

	// Slave methods
	virtual bool get_direct_mem_ptr( PWM_Payload<PWM_SIZE>& payload, tlm_dmi&  dmi_data);
	virtual unsigned int transport_dbg( PWM_Payload<PWM_SIZE>& payload);

	virtual tlm_sync_enum nb_transport_fw( PWM_Payload<PWM_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport( PWM_Payload<PWM_SIZE>& payload, sc_core::sc_time& t);
	// Master methods
	virtual tlm_sync_enum nb_transport_bw( ATD_Payload<ATD1_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual tlm_sync_enum nb_transport_bw( ATD_Payload<ATD0_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr( sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	// Implementation
//	void input(bool pwmValue[PWM_SIZE]);
	void input(bool (*pwmValue)[PWM_SIZE]);
	void output_ATD1(double anValue[ATD1_SIZE]);
	void output_ATD0(double anValue[ATD0_SIZE]);

	virtual void Inject_ATD0(double anValue[8]) {}
	virtual void Inject_ATD1(double anValue[16]) {}
	virtual void Get_PWM(bool (*pwmValue)[PWM_SIZE]) {}

	bool isTerminated() { return terminated; }

protected:
	double	anx_stimulus_period;
	sc_time *anx_stimulus_period_sc;

	double	pwm_fetch_period;
	sc_time *pwm_fetch_period_sc;

	sc_event atd0_bw_event, atd1_bw_event;

	bool trace_enable;
	Parameter<bool> param_trace_enable;

	tlm_quantumkeeper atd0_quantumkeeper;
	tlm_quantumkeeper atd1_quantumkeeper;

private:

	tlm_quantumkeeper pwm_quantumkeeper;

	peq_with_get<PWM_Payload<PWM_SIZE> > input_payload_queue;

	PayloadFabric<ATD_Payload<ATD1_SIZE> > atd1_payload_fabric;
	ATD_Payload<ATD1_SIZE> *atd1_payload;

	PayloadFabric<ATD_Payload<ATD0_SIZE> > atd0_payload_fabric;
	ATD_Payload<ATD0_SIZE> *atd0_payload;

	double	bus_cycle_time;
	sc_time		cycle_time;


	Parameter<double>	param_anx_stimulus_period;
	Parameter<double>	param_pwm_fetch_period;

	ofstream atd0_output_file;
	ofstream atd1_output_file;
	ofstream pwm_output_file;

	bool terminated;

};

#endif /* ATD_PWM_STUB_HH_ */
