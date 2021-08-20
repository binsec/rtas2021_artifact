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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#include <unisim/component/tlm2/processor/hcs12x/pwm.hh>
#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include "unisim/util/debug/simple_register.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using unisim::component::cxx::processor::hcs12x::CONFIG;
using unisim::util::debug::SimpleRegister;

template <uint8_t PWM_SIZE>
PWM<PWM_SIZE>::PWM(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	unisim::kernel::Service<Memory<physical_address_t> >(name, parent),
	unisim::kernel::Service<Registers>(name, parent),
	unisim::kernel::Client<Memory<physical_address_t> >(name, parent),
	unisim::kernel::Client<TrapReporting>(name, parent),

	master_sock("master_socket"),
	slave_socket("slave_socket"),

	memory_export("memory_export", this),
	memory_import("memory_import", this),
	registers_export("registers_export", this),
	trap_reporting_import("trap_reporting_import", this),

	debug_enabled(false),
	param_debug_enabled("debug-enabled", this, debug_enabled),
	channel_output_reg("channel", this, output, PWM_SIZE, "PWM output"),

	bus_cycle_time_int(0),
	param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int),

	baseAddress(0x0300), // MC9S12XDP512V2 - PWM baseAddress
	param_baseAddress("base-address", this, baseAddress),
	interruptOffset(0x8C),
	param_interruptOffset("interrupt-offset", this, interruptOffset)
{

	param_bus_cycle_time_int.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

	uint8_t channel_number;

	char channelName[20];

	for (uint8_t i=0; i<PWM_SIZE; i++) {
#if BYTE_ORDER == BIG_ENDIAN
		channel_number = i;
#else
		channel_number =  PWM_SIZE-i-1;
#endif

		sprintf (channelName, "channel_%d", channel_number);

		channel[channel_number] = new Channel_t(channelName, this, channel_number, &pwmcnt16_register[i], &pwmper16_register[i], &pwmdty16_register_value[i]);

		output[i] = false;
	}

	channel_output_reg.SetMutable(true);

	// Reserved Register for factory testing
	pwmtst_register = pwmprsc_register = pwmscnta_register = pwmscntb_register = 0;

	master_sock(*this);
	interrupt_request(*this);
	slave_socket.register_b_transport(this, &PWM::read_write);
	bus_clock_socket.register_b_transport(this, &PWM::updateBusClock);

	xint_payload = xint_payload_fabric.allocate();
//	pwm_payload = payload_fabric.allocate();

	Reset();

}

template <uint8_t PWM_SIZE>
PWM<PWM_SIZE>::~PWM() {

	// Release registers_registry
	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

	xint_payload->release();
//	pwm_payload->release();

}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::start() {
	for (int i=0; i<PWM_SIZE; i++) {
		channel[i]->wakeup();
	}
}

/**
 * This function is called when PWM7IN (when channel[7].mode == INPUT) input pin is asserted
 *
 * remark: PWM7IN is channel[7]
 */
template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::pwm7in_ChangeStatus(bool pwm7in_status) {

	//const uint8_t pwm7ena_mask = 0x01;
	const uint8_t pwmlvl_mask = 0x10;
	const uint8_t set_pwm7in_mask = 0x04;
	const uint8_t clear_pwm7in_mask = 0xFB;
	const uint8_t pwm7inl_mask = 0x02;

	bool pwmLVL = ((pwmsdn_register & pwmlvl_mask) != 0);

	/**
	 * Any change from passive to asserted(active) state or from active to passive state
	 * will be flagged by setting the PWMIF flag = 1
	 */
	if (((pwmsdn_register & pwm7inl_mask) != 0) ^ pwm7in_status) {
		setPWMInterruptFlag();
	}

	if (pwm7in_status) {
		pwmsdn_register = pwmsdn_register | set_pwm7in_mask;
	} else {
		pwmsdn_register = pwmsdn_register & clear_pwm7in_mask;
	}

	if (((pwmsdn_register & pwm7inl_mask) != 0) == pwm7in_status) {
		for (int i=0; i<PWM_SIZE; i++) {
			channel[i]->setOutput(pwmLVL);
		}
	}
}

template <uint8_t PWM_SIZE>
bool PWM<PWM_SIZE>::isEmergencyShutdownEnable() {
	const uint8_t pwn7ena_mask = 0x01;

	return ((pwmsdn_register & pwn7ena_mask) != 0);
}

// Master methods
template <uint8_t PWM_SIZE>
tlm_sync_enum PWM<PWM_SIZE>::nb_transport_bw( XINT_Payload& xint_payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		xint_payload.release();
		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}


template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::assertInterrupt() {
	// assert a PWM Emergency Shutdown Interrupt (vector = VectorBase + 0x8C)

	tlm_phase phase = BEGIN_REQ;

	xint_payload->acquire();

	xint_payload->setInterruptOffset(interruptOffset);

	sc_time local_time = quantumkeeper.get_local_time();

	tlm_sync_enum ret = interrupt_request->nb_transport_fw(*xint_payload, phase, local_time);

	xint_payload->release();
	
	switch(ret)
	{
		case TLM_ACCEPTED:
			// neither payload, nor phase and local_time have been modified by the callee
			quantumkeeper.sync(); // synchronize to leave control to the callee
			break;
		case TLM_UPDATED:
			// the callee may have modified 'payload', 'phase' and 'local_time'
			quantumkeeper.set(local_time); // increase the time
			if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed

			break;
		case TLM_COMPLETED:
			// the callee may have modified 'payload', and 'local_time' ('phase' can be ignored)
			quantumkeeper.set(local_time); // increase the time
			if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed
			break;
	}

}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::setPWMInterruptFlag() {

	const uint8_t pwmif_mask = 0x80;

	pwmsdn_register = pwmsdn_register | pwmif_mask;

	const uint8_t pwmie_mask = 0x40;

	if ((pwmsdn_register & pwmie_mask) != 0) {
		assertInterrupt();
	}
}


// Master methods
template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

template <uint8_t PWM_SIZE>
tlm_sync_enum PWM<PWM_SIZE>::nb_transport_bw(PWM_Payload<PWM_SIZE>& pwm_payload, tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case BEGIN_REQ:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase BEGIN_REQ" << endl;

			Object::Stop(-1);
			break;
		case END_REQ:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase END_REQ" << endl;
			Object::Stop(-1);
			break;
		case BEGIN_RESP:
			//pwm_payload.release();
			pwm_bw_event.notify();
			return (TLM_COMPLETED);
		case END_RESP:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase END_RESP" << endl;
			Object::Stop(-1);
			break;
		default:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase" << endl;
			Object::Stop(-1);
			break;
	}

	return (TLM_ACCEPTED);
}

/*
template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::refresh_channel(uint8_t channel_number) {

	bool pwmChannelOutput[PWM_SIZE];

	for (int i=0; i < PWM_SIZE; i++) {
		pwmChannelOutput[i] = channel[i]->getOutput();
	}

	refreshOutput(pwmChannelOutput);

}
*/

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::refreshOutput(bool pwmValue[PWM_SIZE])
{
	tlm_phase phase = BEGIN_REQ;

	PWM_Payload<PWM_SIZE>* pwm_payload = payload_fabric.allocate();

	for (int i=0; i<PWM_SIZE; i++) {
		pwm_payload->pwmChannel[i] = pwmValue[i];
	}

//	quantumkeeper.inc(bus_cycle_time); // TODO: has to take in account the DTY and PERIOD and not the bus_cycle_time
//	if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed

//	sc_time local_time = quantumkeeper.get_local_time();
	sc_time local_time = SC_ZERO_TIME;

	if (debug_enabled) {
		cout << sc_object::name() << ":: send " << *pwm_payload << " - " << sc_time_stamp() << endl;
	}

	tlm_sync_enum ret = master_sock->nb_transport_fw(*pwm_payload, phase, local_time);

	pwm_payload->release();
	
	wait(pwm_bw_event);

	switch(ret)
	{
		case TLM_ACCEPTED:
			// neither payload, nor phase and local_time have been modified by the callee
			quantumkeeper.sync(); // synchronize to leave control to the callee
			break;
		case TLM_UPDATED:
			// the callee may have modified 'payload', 'phase' and 'local_time'
			quantumkeeper.set(local_time); // increase the time
			if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed

			break;
		case TLM_COMPLETED:
			// the callee may have modified 'payload', and 'local_time' ('phase' can be ignored)
			quantumkeeper.set(local_time); // increase the time
			if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed
			break;
	}

	quantumkeeper.inc(bus_cycle_time); // TODO: has to take in account the DTY and PERIOD and not the bus_cycle_time
	if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed

}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();
	unsigned int data_length = trans.get_data_length();

	if ((address >= baseAddress) && (address < (baseAddress + REGISTERS_BANK_SIZE))) {

		if (cmd == tlm::TLM_READ_COMMAND) {
			memset(data_ptr, 0, data_length);
			read(address - baseAddress, data_ptr, data_length);
		} else if (cmd == tlm::TLM_WRITE_COMMAND) {
			write(address - baseAddress, data_ptr, data_length);
		}

		trans.set_response_status( tlm::TLM_OK_RESPONSE );

	} else {
		trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
	}
}

template <uint8_t PWM_SIZE>
bool PWM<PWM_SIZE>::read(unsigned int offset, const void *buffer, unsigned int data_length) {

	switch (offset) {
		case PWME: *((uint8_t *) buffer) = pwme_register; break;
		case PWMPOL: *((uint8_t *) buffer) = pwmpol_register; break;
		case PWMCLK: *((uint8_t *) buffer) = pwmclk_register; break;
		case PWMPRCLK: {
			const uint8_t pwmprclk_readMask = 0x77;
			*((uint8_t *) buffer) = pwmprclk_register & pwmprclk_readMask;
		}
		break;
		case PWMCAE: *((uint8_t *) buffer) = pwmcae_register; break;
		case PWMCTL: {
			const uint8_t pwmctl_readMask = 0xFC;
			*((uint8_t *) buffer) =	pwmctl_register & pwmctl_readMask;
		}
		break;
		case PWMTST: {
			const uint8_t pwmtst_readMask = 0X00;
			*((uint8_t *) buffer) = pwmtst_register & pwmtst_readMask;
		}
		break;
		case PWMPRSC: {
			const uint8_t pwmprsc_readMask = 0x00;
			*((uint8_t *) buffer) = pwmprsc_register & pwmprsc_readMask;
		}
		break;
		case PWMSCLA: *((uint8_t *) buffer) = pwmscla_register; break;
		case PWMSCLB: *((uint8_t *) buffer) = pwmsclb_register; break;
		case PWMSCNTA: {
			const uint8_t pwmscnta_readMask = 0x00;
			*((uint8_t *) buffer) = pwmscnta_register & pwmscnta_readMask;
		}
		break;
		case PWMSCNTB: {
			const uint8_t pwmscntb_readMask = 0x00;
			*((uint8_t *) buffer) = pwmscntb_register & pwmscntb_readMask;
		}
		break;
		case PWMSDN: {
			const uint8_t pwmsdn_readMask = 0xD7;
			*((uint8_t *) buffer) = pwmsdn_register & pwmsdn_readMask;
		}
		break;
		default: {
			// PWMCNTx
			if ((offset >= PWMCNT0) && (offset <= PWMCNT7)) {
				*((uint8_t *) buffer) = channel[offset-PWMCNT0]->getPwmcnt_register();
			} else	// PWMPERx
			if ((offset >= PWMPER0) && (offset <= PWMPER7)) {
				*((uint8_t *) buffer) = channel[offset-PWMPER0]->getPWMPERValue();
			} else	// PWMDTYx
			if ((offset >= PWMDTY0) && (offset <= PWMDTY7)) {
				*((uint8_t *) buffer) = channel[offset-PWMDTY0]->getPWMDTYValue();
			} else {
				return (false);
			}
		}
		break;
	}

	return (true);

}

template <uint8_t PWM_SIZE>
bool PWM<PWM_SIZE>::write(unsigned int offset, const void *buffer, unsigned int data_length) {

	uint8_t val = *((uint8_t *) buffer);

	switch (offset) {
		case PWME: {
			bool isEnable;
			uint8_t mask = 0x01;

			for (uint8_t index=0; index<PWM_SIZE; index++) {
				isEnable = ((pwme_register & mask) != 0);
				if (((val & mask) != 0) && !isEnable) {
					channel[index]->wakeup();
				}
				if (((val & mask) == 0) && isEnable) {
					channel[index]->disable();
				}
				mask = mask << 1;
			}
			pwme_register = val;

		}
		break;
		case PWMPOL: {
			uint8_t mask = 0x01;

			pwmpol_register = val;

			for (uint8_t i=0; i<PWM_SIZE; i++) {
				channel[i]->setOutput((pwmpol_register & mask) != 0);
				mask = mask << 1;
			}
		}
		break;
		case PWMCLK:
			pwmclk_register = val; break;
		case PWMPRCLK: {
			const uint8_t pwmprclk_writeMask = 0x77; // bit 3 and 7 can't be written
			pwmprclk_register = val & pwmprclk_writeMask;
			updateClockAB();
		}
		break;
		case PWMCAE:
			pwmcae_register = val; break;
		case PWMCTL: {
			uint8_t oldValue = pwmctl_register;
			uint8_t conDelta = (oldValue ^ val) & 0xF0;
			uint8_t conDeltaMask = 0x10;
			uint8_t pwmeRegs = pwme_register;
			uint8_t pwmeMask = 0x01;

			uint8_t newValue = val & 0xFC; // bit 0 and 1 can't be written
			for (int i=0; i<4; i++) {
				if ((conDelta & conDeltaMask) != 0) // control bit has to be changed ?
				{
					if (!(((pwmeRegs & pwmeMask) != 0) || ((pwmeRegs & (pwmeMask << 1)) != 0))) // are channels disabled ?
					{
						newValue = ~(oldValue & conDeltaMask) | newValue;
					}
					else {
						newValue = (oldValue & conDeltaMask) | newValue;
					}
				}
				else {
					newValue = (oldValue & conDeltaMask) | newValue;
				}

				conDeltaMask = conDeltaMask << 1;
				pwmeMask = pwmeMask << 2;
			}

			pwmctl_register = newValue;

		}
			break;
		case PWMTST: /* Intended for factory test purposes only */ break;
		case PWMPRSC: /* Intended for factory test purposes only */ break;
		case PWMSCLA: {
			pwmscla_register = val;
			updateScaledClockA();
		}
		break;
		case PWMSCLB: {
			pwmsclb_register = val;
			updateScaledClockB();
		}
		break;
		case PWMSCNTA: /* Intended for factory test purposes only */ break;
		case PWMSCNTB: /* Intended for factory test purposes only */ break;

		case PWMSDN: {

			const uint8_t pwmsdn_writeMask = 0xF3;
			const uint8_t pwmif_mask = 0x80;
			const uint8_t pwm7ena_mask = 0x01;
			const uint8_t pwmrstrt_mask = 0x20;
			const uint8_t pwm7inl_mask = 0x02;
			//const uint8_t pwm7in_mask = 0x04;


			/*
			 * The flag PWMIF (bit-7) is cleared by writing a logic 1 to it.
			 * Writing a 0 has no effect.
			 */
			if ((val & pwmif_mask) != 0) {
				val = val & 0x7F;
			} else {
				val = val & ((pwmsdn_register & pwmif_mask) | 0x7F);
			}

			/**
			 * The PWM can only be restarted if the PWM channel input 7 is de-asserted
			 */
			if (((val & pwmrstrt_mask) != 0) && !((pwmsdn_register & pwm7ena_mask) != 0) && !((val & pwm7ena_mask) != 0)) {
				/*
				 * After writing a logic 1 to the PWMRSTRT bit (trigger event)
				 * the PWM channel start running after the corresponding counter passes next "counter=0" phase.
				 */
				start();
			}

			/**
			 * The interrupt flag PWMIF is set when PWMENA is being asserted while the level at PWM7 is active
			 *
			 * i.e. the channel 7 is forced to INPUT mode (emergency enable) and the current pin7 signal level
			 *      is the same as the active level of the emergency shutdown
			 */
			if (((pwmsdn_register & pwm7ena_mask) == 0) && ((val & pwm7ena_mask) != 0) &&
					(((val & pwm7inl_mask) != 0) == channel[7]->getOutput()))
			{
				setPWMInterruptFlag();
			}

			pwmsdn_register = val & pwmsdn_writeMask;
		}
		break;
		default: {
			// PWMCNTx
			if ((offset >= PWMCNT0) && (offset <= PWMCNT7)) {
				// Any external write operation to the counter register sets it to zero (0)
				channel[offset-PWMCNT0]->setPwmcnt_register(0);
			} else 	// PWMPERx
			if ((offset >= PWMPER0) && (offset <= PWMPER7)) {

				uint8_t pwmeMask = 0x01;

				channel[offset-PWMPER0]->setPWMPERBuffer(val);

				pwmeMask = pwmeMask << (offset - PWMPER0);
				if (!(pwme_register & pwmeMask)) {
					channel[offset-PWMPER0]->setPWMPERValue(val);
				}

			} else	// PWMDTYx
			if ((offset >= PWMDTY0) && (offset <= PWMDTY7)) {

				uint8_t pwmeMask = 0x01;

				channel[offset-PWMDTY0]->setPWMDTYBuffer(val);

				pwmeMask = pwmeMask << (offset - PWMDTY0);
				if ((pwme_register & pwmeMask) == 0) {
					channel[offset-PWMDTY0]->setPWMDTYValue(val);
				}

			} else {
				return (false);
			}
		}
		break;
	}

	return (true);
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::updateClockAB() {

	uint8_t pckaMask = 0x07;
	uint8_t pcka = pwmprclk_register & pckaMask;

	clockA = clockVector[pcka];

	uint8_t pckbMask = 0x70;
	uint8_t pckb = (pwmprclk_register & pckbMask) >> 4;

	clockB = clockVector[pckb];

	updateScaledClockA();
	updateScaledClockB();
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::updateScaledClockA() {

	sc_time clockSA;

	if (pwmscla_register != 0x00) {
		clockSA = getClockA() * (2 * pwmscla_register);
	} else {
		clockSA = getClockA() * (2 * 256);
	}

}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::updateScaledClockB() {

	sc_time clockSB;

	if (pwmsclb_register != 0x00) {
		clockSB = getClockB() * (2 * pwmsclb_register);
	} else {
		clockSB = getClockB() * (2 * 256);
	}

}

template <uint8_t PWM_SIZE>
sc_time PWM<PWM_SIZE>::getClockA() {
	return (clockA);
}

template <uint8_t PWM_SIZE>
sc_time PWM<PWM_SIZE>::getClockB() {
	return (clockB);
}

template <uint8_t PWM_SIZE>
sc_time PWM<PWM_SIZE>::getClockSA() {
	return (clockSA);
}

template <uint8_t PWM_SIZE>
sc_time PWM<PWM_SIZE>::getClockSB() {
	return (clockSB);
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::ComputeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

	for (int i=0; i < 8; i++) {
		clockVector[i] = bus_cycle_time * (1 << i);
	}

}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}


	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================

template <uint8_t PWM_SIZE>
bool PWM<PWM_SIZE>::BeginSetup() {


	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PWME", &pwme_register));

	unisim::kernel::variable::Register<uint8_t> *pwme_var = new unisim::kernel::variable::Register<uint8_t>("PWME", this, pwme_register, "PWM Enable Register (PWME)");
	extended_registers_registry.push_back(pwme_var);
	pwme_var->setCallBack(this, PWME, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PWMPOL", &pwmpol_register));

	unisim::kernel::variable::Register<uint8_t> *pwmpol_var = new unisim::kernel::variable::Register<uint8_t>("PWMPOL", this, pwmpol_register, "PWM Polarity Register (PWMPOL)");
	extended_registers_registry.push_back(pwmpol_var);
	pwmpol_var->setCallBack(this, PWMPOL, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PWMCLK", &pwmclk_register));

	unisim::kernel::variable::Register<uint8_t> *pwmclk_var = new unisim::kernel::variable::Register<uint8_t>("PWMCLK", this, pwmclk_register, "PWM Clock Select Register (PWMCLK)");
	extended_registers_registry.push_back(pwmclk_var);
	pwmclk_var->setCallBack(this, PWMCLK, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PWMPRCLK", &pwmprclk_register));

	unisim::kernel::variable::Register<uint8_t> *pwmprclk_var = new unisim::kernel::variable::Register<uint8_t>("PWMPRCLK", this, pwmprclk_register, "PWM Prescale Clock Select Register (PWMPRCLK)");
	extended_registers_registry.push_back(pwmprclk_var);
	pwmprclk_var->setCallBack(this, PWMPRCLK, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PWMCAE", &pwmcae_register));

	unisim::kernel::variable::Register<uint8_t> *pwmcae_var = new unisim::kernel::variable::Register<uint8_t>("PWMCAE", this, pwmcae_register, "PWM Center Align Enable Register (PWMCAE)");
	extended_registers_registry.push_back(pwmcae_var);
	pwmcae_var->setCallBack(this, PWMCAE, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PWMCTL", &pwmctl_register));

	unisim::kernel::variable::Register<uint8_t> *pwmctl_var = new unisim::kernel::variable::Register<uint8_t>("PWMCTL", this, pwmctl_register, "PWM Control Register (PWMCTL)");
	extended_registers_registry.push_back(pwmctl_var);
	pwmctl_var->setCallBack(this, PWMCTL, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PWMTST", &pwmtst_register));

	unisim::kernel::variable::Register<uint8_t> *pwmtst_var = new unisim::kernel::variable::Register<uint8_t>("PWMTST", this, pwmtst_register, "Reserved Register (PWMTST)");
	extended_registers_registry.push_back(pwmtst_var);
	pwmtst_var->setCallBack(this, PWMTST, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PWMPRSC", &pwmprsc_register));

	unisim::kernel::variable::Register<uint8_t> *pwmprsc_var = new unisim::kernel::variable::Register<uint8_t>("PWMPRSC", this, pwmprsc_register, "Reserved Register (PWMPRSC)");
	extended_registers_registry.push_back(pwmprsc_var);
	pwmprsc_var->setCallBack(this, PWMCTL, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PWMSCLA", &pwmscla_register));

	unisim::kernel::variable::Register<uint8_t> *pwmscla_var = new unisim::kernel::variable::Register<uint8_t>("PWMSCLA", this, pwmscla_register, "PWM Scale A Register (PWMSCLA)");
	extended_registers_registry.push_back(pwmscla_var);
	pwmscla_var->setCallBack(this, PWMSCLA, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PWMSCLB", &pwmsclb_register));

	unisim::kernel::variable::Register<uint8_t> *pwmsclb_var = new unisim::kernel::variable::Register<uint8_t>("PWMSCLB", this, pwmsclb_register, "PWM Scale B Register (PWMSCLB)");
	extended_registers_registry.push_back(pwmsclb_var);
	pwmsclb_var->setCallBack(this, PWMSCLB, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PWMSCNTA", &pwmscnta_register));

	unisim::kernel::variable::Register<uint8_t> *pwmscnta_var = new unisim::kernel::variable::Register<uint8_t>("PWMSCNTA", this, pwmscnta_register, "Reserved Registers (PWMSCNTA)");
	extended_registers_registry.push_back(pwmscnta_var);
	pwmscnta_var->setCallBack(this, PWMSCNTA, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PWMSCNTB", &pwmscntb_register));

	unisim::kernel::variable::Register<uint8_t> *pwmscntb_var = new unisim::kernel::variable::Register<uint8_t>("PWMSCNTB", this, pwmscntb_register, "Reserved Registers (PWMSCNTB)");
	extended_registers_registry.push_back(pwmscntb_var);
	pwmscntb_var->setCallBack(this, PWMSCNTB, &CallBackObject::write, NULL);

	for (int i=0; i<PWM_SIZE; i++) {
		
		std::stringstream sstr;
		sstr << "PWMCNT" << i;
		std::string shortName(sstr.str());

		registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + '.' + shortName, &pwmcnt16_register[i]));
		
		unisim::kernel::variable::Register<uint8_t> *pwmcnt0_var = new unisim::kernel::variable::Register<uint8_t>(shortName.c_str(), this, pwmcnt16_register[i], "PWM Channel Counter Register");
		extended_registers_registry.push_back(pwmcnt0_var);
		pwmcnt0_var->setCallBack(this, PWMCNT0+i, &CallBackObject::write, NULL);
	}
	
	for (int i=0; i<PWM_SIZE; i++) {
		
		std::stringstream sstr;
		sstr << "PWMPER" << i;
		std::string shortName(sstr.str());
	
		registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + '.' + shortName, &pwmper16_register[i]));

		unisim::kernel::variable::Register<uint8_t> *pwmper0_var = new unisim::kernel::variable::Register<uint8_t>(shortName.c_str(), this, pwmper16_register[i], "PWM Channel Period Register");
		extended_registers_registry.push_back(pwmper0_var);
		pwmper0_var->setCallBack(this, PWMPER0+i, &CallBackObject::write, NULL);
	}

	for (int i=0; i<PWM_SIZE; i++) {
		
		std::stringstream sstr;
		sstr << "PWMDTY" << i;
		std::string shortName(sstr.str());
		
		registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + '.' + shortName, &pwmdty16_register_value[i]));

		unisim::kernel::variable::Register<uint8_t> *pwmdty0_var = new unisim::kernel::variable::Register<uint8_t>(shortName.c_str(), this, pwmdty16_register_value[i], "PWM Channel Duty Register");
		extended_registers_registry.push_back(pwmdty0_var);
		pwmdty0_var->setCallBack(this, PWMDTY0+i, &CallBackObject::write, NULL);

	}

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PWMSDN", &pwmsdn_register));

	unisim::kernel::variable::Register<uint8_t> *pwmsdn_var = new unisim::kernel::variable::Register<uint8_t>("PWMSDN", this, pwmsdn_register, "PWM Shutdown Register (PWMSDN)");
	extended_registers_registry.push_back(pwmsdn_var);
	pwmsdn_var->setCallBack(this, PWMSDN, &CallBackObject::write, NULL);

	ComputeInternalTime();

	return (true);

}

template <uint8_t PWM_SIZE>
bool PWM<PWM_SIZE>::Setup(ServiceExportBase *srv_export) {

	return (true);
}

template <uint8_t PWM_SIZE>
bool PWM<PWM_SIZE>::EndSetup() {

	return (true);
}


/**
 * Gets a register interface to the register specified by name.
 *
 * @param name The name of the requested register.
 * @return A pointer to the RegisterInterface corresponding to name.
 */
template <uint8_t PWM_SIZE>
Register* PWM<PWM_SIZE>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::OnDisconnect() {
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Reset() {

	pwme_register =  0;
	pwmpol_register = 0;
	pwmclk_register = 0;
	pwmprclk_register = 0;
	pwmcae_register = 0;
	pwmctl_register = 0;
	pwmscla_register = 0;
	pwmsclb_register = 0;

	for (int i=0; i < PWM_SIZE; i++) {
		pwmcnt16_register[i] = 0;
		pwmper16_register[i] = 0xFF;
		pwmdty16_register_value[i] = 0xFF;
	}

	pwmsdn_register = 0;

}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::ResetMemory() {

	Reset();

}

	//=====================================================
	//=             PWM Channel methods                   =
	//=====================================================
template <uint8_t PWM_SIZE>
PWM<PWM_SIZE>::Channel_t::Channel_t(const sc_module_name& name, PWM *parent, const uint8_t channel_num, uint8_t *pwmcnt_ptr, uint8_t *pwmper_ptr, uint8_t *pwmdty_ptr) :
	sc_module(name),
	pwmParent(parent),
	channel_index(channel_num),
	pwmcnt_register_ptr(pwmcnt_ptr),
	pwmper_register_value_ptr(pwmper_ptr),
	pwmdty_register_value_ptr(pwmdty_ptr),
	channelMask(0)

{

	channelMask = (0x01 << channel_index);

	SC_HAS_PROCESS(Channel_t);

	SC_THREAD(Run);
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::Run() {

	sc_time clk;

	uint8_t bit1Mask = 0x02;


	uint8_t ctl_register;
	uint8_t conMask = 0x10 << (channel_index / 2);
	/*
	 * PWMCTL bits interpretation
	 * bit7: CON67, bit6: CON45, bit5: CON23, bit4: CON01
	 * when
	 *  - CONxy=1 then channel x high order and channel y low order
	 *  - only channel Y registers and output are used to control the behavior of the concatenated channel
	 */

	bool isEnable = false;

	while (true) {
		isEnable = ((pwmParent->pwme_register & channelMask) != 0);
		ctl_register = pwmParent->pwmctl_register;

		/*
		 * - Take in account the channel state (Enable or Disable)
		 * - Is Channel concatenated ?
		 */
		while ((((ctl_register & conMask) != 0) && (channel_index % 2 == 0)) || (!isEnable))
		{
			wait(wakeup_event);

			isEnable = ((pwmParent->pwme_register & channelMask) != 0);
			ctl_register = pwmParent->pwmctl_register;
		}

		/*
	 	 *  - Select clock source using PCLKx
	 	 *     channel 0, 1, 4, 5 then clock A or SA
	 	 *     channel 2, 3, 6, 7 then clock B or SB
		 */
		if ((pwmParent->pwmclk_register & channelMask)  != 0) {
			if ((channel_index & bit1Mask) != 0) {
				clk = pwmParent->getClockB();
			} else {
				clk = pwmParent->getClockA();
			}

		} else {
			if ((channel_index & bit1Mask) != 0) {
				clk = pwmParent->getClockSB();
			} else {
				clk = pwmParent->getClockSA();
			}

		}

		if (((ctl_register & conMask) != 0) && (channel_index % 2 != 0)) { // 16-bit mode
			checkChangeStateAndWait<uint16_t>(clk);
		} else { // 8-bit mode
			checkChangeStateAndWait<uint8_t>(clk);
		}
	}
}

template <uint8_t PWM_SIZE>
template <class T> void PWM<PWM_SIZE>::Channel_t::checkChangeStateAndWait(const sc_time clk) {

	uint16_t toPeriod;
	bool	isCenterAligned = true;
	//int8_t increment = 1;

	T dty = *((T *) pwmdty_register_value_ptr);
	T period = *((T *) pwmper_register_value_ptr);
//	T cnt = *((T *) pwmcnt_register_ptr);
	bool isPolarityHigh = ((pwmParent->pwmpol_register & channelMask) != 0);

	// Handle PWM Boundary Cases

	if (period == 0x00) 	// Counter = 0x00 and does not count
	{

		setPwmcnt_register(0);
		if (isPolarityHigh) {
				setOutput(true);
		} else {
				setOutput(false);
		}

//		pwmParent->refresh_channel(channel_index);

		wait(wakeup_event);
		return;
	}

	if (((dty == 0x00) && (period > 0x00) && !isPolarityHigh) ||
			((dty >= period) && isPolarityHigh))
	{

		setOutput(true);

//		pwmParent->refresh_channel(channel_index);

		wait(wakeup_event);
		return;

	} else if (((dty == 0x00) && (period > 0x00) && isPolarityHigh) ||
			((dty >= period) && !isPolarityHigh))
	{

		setOutput(false);

//		pwmParent->refresh_channel(channel_index);

		wait(wakeup_event);
		return;
	}

	// Handle PWM Normal Cases

	// Check alignment and compute the current period
	isCenterAligned = ((pwmParent->pwmcae_register & channelMask) != 0);  // is Center Aligned ?

	toPeriod = period - dty;

	*((T *) pwmcnt_register_ptr) = dty;

	wait(dty*clk);

	setOutput(!getOutput());

//	pwmParent->refresh_channel(channel_index);

	if (*((T *) pwmcnt_register_ptr) == dty) // The counter can be reset by software during wait
	{
		*((T *) pwmcnt_register_ptr) = period;

		wait(toPeriod*clk);

		if (*((T *) pwmcnt_register_ptr) == period) // The counter can be reset by software during wait
		{
			if (isCenterAligned) {
				// count in decrement mode

				*((T *) pwmcnt_register_ptr) = dty;

				wait(toPeriod*clk);
			}

			setOutput(!getOutput());

//			pwmParent->refresh_channel(channel_index);

			if (isCenterAligned) {

				if (*((T *) pwmcnt_register_ptr) == dty) // The counter can be reset by software during wait
				{
					wait(dty*clk);
				}

			}

			setPwmcnt_register(0);	// end of period

		}

	}

}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::wakeup() {
	wakeup_event.notify();
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::disable() {
	/*
	 *  Buffered registers are updated :
	 *  - The effective period ends
	 *  - The counter is written (counter reset to $00)
	 *  - The channel is disabled
	 */

	setPWMPERValue(getPWMPERBuffer());
	setPWMDTYValue(getPWMDTYBuffer());
}

template <uint8_t PWM_SIZE>
bool PWM<PWM_SIZE>::Channel_t::getOutput() { return (pwmParent->getOutput(channel_index)); }
/**
 * The channel output is meaningful only if the channel is enabled
 */
template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::setOutput(bool val) {

	if ((pwmParent->pwme_register & channelMask) != 0) pwmParent->setOutput(channel_index, val);
}

template <uint8_t PWM_SIZE>
uint8_t PWM<PWM_SIZE>::Channel_t::getPwmcnt_register() { return (*pwmcnt_register_ptr); }

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::setPwmcnt_register(uint8_t val) {
	*pwmcnt_register_ptr = val;
	if (*pwmcnt_register_ptr == 0) {
		setPWMPERValue(getPWMPERBuffer());
		setPWMDTYValue(getPWMDTYBuffer());
	}
}

template <uint8_t PWM_SIZE>
uint8_t PWM<PWM_SIZE>::Channel_t::getPWMPERValue() { return (*pwmper_register_value_ptr); }

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::setPWMPERValue(uint8_t val) {
	*pwmper_register_value_ptr = val;
}

template <uint8_t PWM_SIZE>
uint8_t PWM<PWM_SIZE>::Channel_t::getPWMPERBuffer() { return (pwmper_register_buffer); }

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::setPWMPERBuffer(uint8_t val) { pwmper_register_buffer = val; }

template <uint8_t PWM_SIZE>
uint8_t PWM<PWM_SIZE>::Channel_t::getPWMDTYValue() { return (*pwmdty_register_value_ptr); }

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::setPWMDTYValue(uint8_t val) { *pwmdty_register_value_ptr = val; }

template <uint8_t PWM_SIZE>
uint8_t PWM<PWM_SIZE>::Channel_t::getPWMDTYBuffer() { return (pwmdty_register_buffer); }

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::setPWMDTYBuffer(uint8_t val) { pwmdty_register_buffer = val; }


	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================
template <uint8_t PWM_SIZE>
bool PWM<PWM_SIZE>::ReadMemory(physical_address_t addr, void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr <= (baseAddress+REGISTERS_BANK_SIZE))) {

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
			case PWME: *((uint8_t *)buffer) = pwme_register; break;
			case PWMPOL: *((uint8_t *)buffer) = pwmpol_register; break;
			case PWMCLK: *((uint8_t *)buffer) = pwmclk_register; break;
			case PWMPRCLK: *((uint8_t *)buffer) = pwmprclk_register; break;
			case PWMCAE: *((uint8_t *)buffer) = pwmcae_register; break;
			case PWMCTL: *((uint8_t *)buffer) =	pwmctl_register; break;
			case PWMTST: *((uint8_t *)buffer) = pwmtst_register; break;
			case PWMPRSC: *((uint8_t *)buffer) = pwmprsc_register; break;
			case PWMSCLA: *((uint8_t *)buffer) = pwmscla_register; break;
			case PWMSCLB: *((uint8_t *)buffer) = pwmsclb_register; break;
			case PWMSCNTA: *((uint8_t *)buffer) = pwmscnta_register; break;
			case PWMSCNTB: *((uint8_t *)buffer) = pwmscntb_register; break;
			case PWMSDN: *((uint8_t *)buffer) = pwmsdn_register; break;
			default: {
				// PWMCNTx
				if ((offset >= PWMCNT0) && (offset <= PWMCNT7)) {
					*((uint8_t *)buffer) = pwmcnt16_register[offset - PWMCNT0];
				} else	// PWMPERx
				if ((offset >= PWMPER0) && (offset <= PWMPER7)) {
					*((uint8_t *)buffer) = pwmper16_register[offset - PWMPER0];
				} else	// PWMDTYx
				if ((offset >= PWMDTY0) && (offset <= PWMDTY7)) {
					*((uint8_t *)buffer) = pwmdty16_register_value[offset - PWMDTY0];
				} else {
					return (false);
				}
			}
			break;
		}

		return (true);
	}

	return (false);
}

//template <uint8_t PWM_SIZE>
//bool PWM<PWM_SIZE>::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size) {
//
//	if ((addr >= baseAddress) && (addr <= (baseAddress+PWMSDN))) {
//
//		if (size == 0) {
//			return true;
//		}
//
//		physical_address_t offset = addr-baseAddress;
//
//		return write(offset, *((uint8_t *)buffer));
//	}
//
//	return false;
//}

template <uint8_t PWM_SIZE>
bool PWM<PWM_SIZE>::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr <= (baseAddress+REGISTERS_BANK_SIZE))) {

		if (size == 0) {
			return (true);
		}

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
			case PWME: pwme_register = *((uint8_t *)buffer); break;
			case PWMPOL: pwmpol_register = *((uint8_t *)buffer); break;
			case PWMCLK: pwmclk_register = *((uint8_t *)buffer); break;
			case PWMPRCLK: pwmprclk_register = *((uint8_t *)buffer); break;
			case PWMCAE: pwmcae_register = *((uint8_t *)buffer); break;
			case PWMCTL: pwmctl_register = *((uint8_t *)buffer); break;
			case PWMTST: pwmtst_register = *((uint8_t *)buffer); break;
			case PWMPRSC: pwmprsc_register = *((uint8_t *)buffer); break;
			case PWMSCLA: pwmscla_register = *((uint8_t *)buffer); break;
			case PWMSCLB: pwmsclb_register = *((uint8_t *)buffer); break;
			case PWMSCNTA: pwmscnta_register = *((uint8_t *)buffer); break;
			case PWMSCNTB: pwmscntb_register = *((uint8_t *)buffer); break;
			case PWMSDN: pwmsdn_register = *((uint8_t *)buffer); break;
			default: {
				// PWMCNTx
				if ((offset >= PWMCNT0) && (offset <= PWMCNT7)) {
					pwmcnt16_register[offset - PWMCNT0] = *((uint8_t *)buffer);
				} else	// PWMPERx
				if ((offset >= PWMPER0) && (offset <= PWMPER7)) {
					pwmper16_register[offset - PWMPER0] = *((uint8_t *)buffer);
				} else	// PWMDTYx
				if ((offset >= PWMDTY0) && (offset <= PWMDTY7)) {
					pwmdty16_register_value[offset - PWMDTY0] = offset >= PWMDTY0;
				} else {
					return (false);
				}
			}
			break;
		}

		return (true);
	}

	return (false);
}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

