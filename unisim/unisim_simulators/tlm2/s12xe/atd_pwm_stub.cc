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



#include "atd_pwm_stub.hh"

ATD_PWM_STUB::ATD_PWM_STUB(const sc_module_name& name, Object *parent) :
	Object(name)
	, sc_module(name)
	, atd1_master_sock("atd1_master_sock")
	, atd0_master_sock("atd0_master_sock")
	, slave_sock("slave_sock")

	, anx_stimulus_period(80000000) // 80 us
	, anx_stimulus_period_sc(NULL)

	, pwm_fetch_period(1e9) // 1 ms
	, pwm_fetch_period_sc(0)

	, trace_enable(false)
	, param_trace_enable("trace-enabled", this, trace_enable)

	, input_payload_queue("input_payload_queue")

	, param_anx_stimulus_period("anx-stimulus-period", this, anx_stimulus_period)
	, param_pwm_fetch_period("pwm-fetch-period", this, pwm_fetch_period)

	, terminated(false)

{
//	atd1_master_sock(*this);
//	atd0_master_sock(*this);
	slave_sock(*this);

	atd0_bw = new BW_IF<ATD0_SIZE>(atd0_bw_event);
	atd1_bw = new BW_IF<ATD1_SIZE>(atd1_bw_event);

	atd1_master_sock(*(this->atd1_bw));
	atd0_master_sock(*(this->atd0_bw));

//	atd1_payload = atd1_payload_fabric.allocate();
//	atd0_payload = atd0_payload_fabric.allocate();
}

ATD_PWM_STUB::~ATD_PWM_STUB() {

//	atd1_payload->release();
//	atd0_payload->release();

	if (atd0_bw) { delete atd0_bw; atd0_bw = NULL; }
	if (atd1_bw) { delete atd1_bw; atd1_bw = NULL; }

	if (trace_enable) {
		atd0_output_file.close();
		atd1_output_file.close();
		pwm_output_file.close();
	}

	if (anx_stimulus_period_sc) { delete anx_stimulus_period_sc; anx_stimulus_period_sc = NULL; }
	if (pwm_fetch_period_sc) { delete pwm_fetch_period_sc; pwm_fetch_period_sc = NULL; }
}

bool ATD_PWM_STUB::BeginSetup() {

	if (trace_enable) {
		atd0_output_file.open ("atd0_output.txt");
		atd1_output_file.open ("atd1_output.txt");
		pwm_output_file.open ("pwm_output.txt");
	}

	tlm_quantumkeeper::set_global_quantum(sc_time(0.0, SC_MS));

	anx_stimulus_period_sc = new sc_time(anx_stimulus_period, SC_PS);
	pwm_fetch_period_sc = new sc_time(pwm_fetch_period, SC_PS);

	return (true);
}

bool ATD_PWM_STUB::Setup(ServiceExportBase *srv_export) {
	return (true);
}

bool ATD_PWM_STUB::EndSetup() {
	return (true);
}

void ATD_PWM_STUB::Stop(int exit_status) {
	terminated = true;
}

// Slave methods
bool ATD_PWM_STUB::get_direct_mem_ptr( PWM_Payload<PWM_SIZE>& payload, tlm_dmi&  dmi_data)
{
	// Leave this empty as it is designed for memory mapped buses
	return (false);
}

unsigned int ATD_PWM_STUB::transport_dbg( PWM_Payload<PWM_SIZE>& payload)
{
	// Leave this empty as it is designed for memory mapped buses
	return (0);
}

tlm_sync_enum ATD_PWM_STUB::nb_transport_fw( PWM_Payload<PWM_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_REQ)
	{
		phase = END_REQ; // update the phase
		payload.acquire();

		input_payload_queue.notify(payload, t); // queue the payload and the associative time
		return (TLM_UPDATED);
	}

	// we received an unexpected phase, so we return TLM_ACCEPTED
	return (TLM_ACCEPTED);
}

void ATD_PWM_STUB::b_transport( PWM_Payload<PWM_SIZE>& payload, sc_core::sc_time& t)
{
	payload.acquire();
	input_payload_queue.notify(payload, t);
}

// Master methods
//tlm_sync_enum ATD_PWM_STUB::nb_transport_bw( ATD_Payload<ATD1_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t)
//{
//	if(phase == BEGIN_RESP)
//	{
//		// payload.release();
//		atd1_bw_event.notify();
//
//		return (TLM_COMPLETED);
//	}
//	return (TLM_ACCEPTED);
//}
//
//tlm_sync_enum ATD_PWM_STUB::nb_transport_bw( ATD_Payload<ATD0_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t)
//{
//	if(phase == BEGIN_RESP)
//	{
//		//payload.release();
//		atd0_bw_event.notify();
//		return (TLM_COMPLETED);
//	}
//	return (TLM_ACCEPTED);
//}
//
//void ATD_PWM_STUB::invalidate_direct_mem_ptr( sc_dt::uint64 start_range, sc_dt::uint64 end_range)
//{
//}

// Implementation
//void ATD_PWM_STUB::input(bool* pwmValue[PWM_SIZE])
void ATD_PWM_STUB::input(bool (*pwmValue)[PWM_SIZE])
{

	PWM_Payload<PWM_SIZE> *payload = NULL;

	wait(input_payload_queue.get_event());

	payload = input_payload_queue.get_next_transaction();

	if (trace_enable) {
		pwm_output_file << (pwm_quantumkeeper.get_current_time().to_seconds() * 1000) << " ms \t\t" << *payload <<  std::endl;
	}

	for (int i=0; i<PWM_SIZE; i++) {
		(*pwmValue)[i] = payload->pwmChannel[i];
	}

	payload->release();

	tlm_phase phase = BEGIN_RESP;
	sc_time local_time = SC_ZERO_TIME;
	slave_sock->nb_transport_bw( *payload, phase, local_time);

//	pwm_quantumkeeper.inc(*pwm_fetch_period_sc);
//	if (pwm_quantumkeeper.need_sync()) pwm_quantumkeeper.sync();

}

void ATD_PWM_STUB::output_ATD0(double anValue[ATD0_SIZE])
{
	tlm_phase phase = BEGIN_REQ;

	ATD_Payload<ATD0_SIZE> *atd0_payload = atd0_payload_fabric.allocate();

	for (int i=0; i<ATD0_SIZE; i++) {
		atd0_payload->anPort[i] = anValue[i];
	}


//	sc_time local_time = atd0_quantumkeeper.get_local_time();
	sc_time local_time = SC_ZERO_TIME;

	if (trace_enable) {
		atd0_output_file << (atd0_quantumkeeper.get_current_time().to_seconds() * 1000) << " ms \t\t" << *atd0_payload << std::endl;
	}

	tlm_sync_enum ret = atd0_master_sock->nb_transport_fw(*atd0_payload, phase, local_time);

	atd0_payload->release();

	wait(atd0_bw_event);

	switch(ret)
	{
		case TLM_ACCEPTED:
			// neither payload, nor phase and local_time have been modified by the callee
			atd0_quantumkeeper.sync(); // synchronize to leave control to the callee
			break;
		case TLM_UPDATED:
			// the callee may have modified 'payload', 'phase' and 'local_time'
			atd0_quantumkeeper.set(local_time); // increase the time
			if(atd0_quantumkeeper.need_sync()) atd0_quantumkeeper.sync(); // synchronize if needed

			break;
		case TLM_COMPLETED:
			// the callee may have modified 'payload', and 'local_time' ('phase' can be ignored)
			atd0_quantumkeeper.set(local_time); // increase the time
			if(atd0_quantumkeeper.need_sync()) atd0_quantumkeeper.sync(); // synchronize if needed
			break;
	}

	atd0_quantumkeeper.inc(*anx_stimulus_period_sc);
	if(atd0_quantumkeeper.need_sync()) atd0_quantumkeeper.sync();

}

void ATD_PWM_STUB::output_ATD1(double anValue[ATD1_SIZE])
{
	tlm_phase phase = BEGIN_REQ;

	ATD_Payload<ATD1_SIZE> *atd1_payload = atd1_payload_fabric.allocate();

	for (int i=0; i<ATD1_SIZE; i++) {
		atd1_payload->anPort[i] = anValue[i];
	}


//	sc_time local_time = atd1_quantumkeeper.get_local_time();
	sc_time local_time = SC_ZERO_TIME;

	if (trace_enable) {
		atd1_output_file << (atd1_quantumkeeper.get_current_time().to_seconds() * 1000) << " ms \t\t" << *atd1_payload << std::endl;
	}

	tlm_sync_enum ret = atd1_master_sock->nb_transport_fw(*atd1_payload, phase, local_time);

	atd1_payload->release();

	wait(atd1_bw_event);

	switch(ret)
	{
		case TLM_ACCEPTED:
			// neither payload, nor phase and local_time have been modified by the callee
			atd1_quantumkeeper.sync(); // synchronize to leave control to the callee
			break;
		case TLM_UPDATED:
			// the callee may have modified 'payload', 'phase' and 'local_time'
			atd1_quantumkeeper.set(local_time); // increase the time
			if(atd1_quantumkeeper.need_sync()) atd1_quantumkeeper.sync(); // synchronize if needed

			break;
		case TLM_COMPLETED:
			// the callee may have modified 'payload', and 'local_time' ('phase' can be ignored)
			atd1_quantumkeeper.set(local_time); // increase the time
			if(atd1_quantumkeeper.need_sync()) atd1_quantumkeeper.sync(); // synchronize if needed
			break;
	}

	atd1_quantumkeeper.inc(*anx_stimulus_period_sc);
	if(atd1_quantumkeeper.need_sync()) atd1_quantumkeeper.sync();

}
