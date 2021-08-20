/*
 *  Copyright (c) 2008, 2011
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

#include <unisim/component/tlm2/processor/hcs12x/ect.hh>
#include "unisim/util/debug/simple_register.hh"
#include "unisim/util/endian/endian.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using unisim::util::debug::SimpleRegister;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::Host2BigEndian;

//template void ECT::IOC_Channel_t::checkChangeStateAndWait<uint8_t>(const sc_time clk);
//template void ECT::IOC_Channel_t::checkChangeStateAndWait<uint16_t>(const sc_time clk);

ECT::ECT(const sc_module_name& name, Object *parent) :
	Object(name, parent)
	, sc_module(name)

	, unisim::kernel::Client<TrapReporting>(name, parent)
	, unisim::kernel::Service<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::Service<Registers>(name, parent)
	, unisim::kernel::Client<Memory<physical_address_t> >(name, parent)

	, trap_reporting_import("trap_reporting_import", this)

	, slave_socket("slave_socket")

	, memory_export("memory_export", this)
	, memory_import("memory_import", this)
	, registers_export("registers_export", this)
	, logger(0)

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

	 // MC9S12XDP512V2 - ECT baseAddress
	, baseAddress(0x0040)
	, param_baseAddress("base-address", this, baseAddress)

	, offset_channel0_interrupt(0xEE)
	, param_offset_channel0_interrupt("interrupt-offset-channel0", this, offset_channel0_interrupt)
	, offset_timer_overflow_interrupt(0xDE)
	, param_offset_timer_overflow_interrupt("interrupt-offset-timer-overflow", this, offset_timer_overflow_interrupt)
	, pulse_accumulatorA_overflow_interrupt(0xDC)
	, param_pulse_accumulatorA_overflow_interrupt("interrupt-pulse-accumulatorA-overflow", this, pulse_accumulatorA_overflow_interrupt)
	, pulse_accumulatorB_overflow_interrupt(0xC8)
	, param_pulse_accumulatorB_overflow_interrupt("interrupt-pulse-accumulatorB-overflow", this, pulse_accumulatorB_overflow_interrupt)
	, pulse_accumulatorA_input_edge_interrupt(0xDA)
	, param_pulse_accumulatorA_input_edge_interrupt("interrupt-pulse-accumulator-input-edge", this, pulse_accumulatorA_input_edge_interrupt)
	, modulus_counter_interrupt(0xCA)
	, param_modulus_counter_interrupt("modulus-counter-interrupt", this, modulus_counter_interrupt)

	, debug_enabled(false)
	, param_debug_enabled("debug-enabled", this, debug_enabled)

	, builtin_signal_generator(false)
	, param_builtin_signal_generator("built-in-signal-generator-enable", this, builtin_signal_generator, "Use built-in signal generator or external instrument")
	, signal_generator_period_int(500000)
	, param_signal_generator_period("built-in-signal-generator-period", this, signal_generator_period_int, "Built-in Signal generator period in pico-seconds. Default 25000ps.")

	, portt_pin_reg("ioc", this, portt_pin, 8, "ECT pins")

	, prnt_write(false)
	, icsys_write(false)
	, main_timer_enabled(false)
	, down_counter_enabled(false)
	, delay_counter_enabled(false)
{

	param_bus_cycle_time_int.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_signal_generator_period.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

	char channelName[20];

	for (unsigned int i=0; i<4; i++) {
		sprintf (channelName, "pa%d", i);
		pc8bit[i] = new PulseAccumulator8Bit(this, i, &pacn_register[i], &paxh_registers[i]);
	}

	for (unsigned int i=0; i<8; i++) {

//		portt_pin[i] = false;
		portt_pin_reg[i] = false;

		portt_pin_reg[i].SetMutable(true);

		char channelName[20];

		sprintf (channelName, "ioc_%d", i);

		if (i < 4) {
			ioc_channel[i] = new IOC_Channel_t(channelName, this, i, &portt_pin[i], &tc_registers[i], &tcxh_registers[i], pc8bit[i]);
		} else {
			ioc_channel[i] = new IOC_Channel_t(channelName, this, i, &portt_pin[i], &tc_registers[i], NULL, NULL);
		}
		portt_pin_reg[i].AddListener(ioc_channel[i]);

	}


	logger = new unisim::kernel::logger::Logger(*this);

	interrupt_request(*this);
	slave_socket.register_b_transport(this, &ECT::read_write);
	bus_clock_socket.register_b_transport(this, &ECT::updateCRGClock);

	SC_HAS_PROCESS(ECT);

	SC_THREAD(runMainTimerCounter);

	SC_THREAD(runDownCounter);

	SC_THREAD(runBuildinSignalGenerator);

	pacA = new PulseAccumulatorA("PACA", this, &portt_pin[7], pc8bit[3], pc8bit[2]);
	portt_pin_reg[7].AddListener(pacA);

	pacB = new PulseAccumulatorB("PACB", this, &portt_pin[0], pc8bit[1], pc8bit[0]);
	portt_pin_reg[0].AddListener(pacB);

	Reset();

	xint_payload = xint_payload_fabric.allocate();

}

ECT::~ECT() {

	xint_payload->release();

	// Release registers_registry
	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

	if (logger) { delete logger; logger = NULL;}

}

void ECT::runBuildinSignalGenerator() {

	/* initialize random seed: */
	srand (12345);

	if (builtin_signal_generator) {
		while (true) {

			for (unsigned int i=0; i<8; i++) {
				/* generate signal code */
				bool signalValue = (1 == rand() % 2);

				if (portt_pin[i] != signalValue) {
					portt_pin_reg[i] = signalValue;

					// if edge sharing is enabled then channels [4,7] have been respectively stimulated by channels [0,3].
					if ((i < 4) && isSharingEdgeEnabled(1 << (i+4))) {
						portt_pin_reg[i+4] = signalValue;
					}
				}
			}

			wait(signal_generator_period);

		}

	}
}


inline void ECT::main_timer_enable() {
	main_timer_enabled = true;
	if ((tscr1_register & 0x80) != 0) {
		main_timer_enable_event.notify(sc_core::SC_ZERO_TIME);
	}
}

inline void ECT::main_timer_disable() {main_timer_enabled = false; }

/**
 * TODO:
 * Timer Module Stops While in Wait
 *  - 0 Allows the timer module to continue running during wait.
 *  - 1 Disables the timer counter, pulse accumulators and modulus down counter when the MCU is in wait mode.
 *  Timer interrupts cannot be used to get the MCU out of wait.
 */
void ECT::enterWaitMode() {
	if ((tscr1_register & 0x40) != 0) {
		main_timer_disable();
	}
}

void ECT::exitWaitMode() {
	main_timer_enable();
}

/**
 * TODO:
 * Timer and Modulus Counter Stop While in Freeze Mode
 *  - 0 Allows the timer and modulus counter to continue running while in freeze mode.
 *  - 1 Disables the timer and modulus counter whenever the MCU is in freeze mode. This is useful for emulation.
 *  The pulse accumulators do not stop in freeze mode.
 */
void ECT::enterFreezeMode() {
	if ((tscr1_register & 0x20) != 0) {
		main_timer_disable();
	}
}

void ECT::exitFreezeMode() {
	main_timer_enable();
}

void ECT::runMainTimerCounter() {

	while (true) {
		while (!main_timer_enabled || ((tscr1_register & 0x80) == 0)) {
			wait(main_timer_enable_event);
		}

		computeTimerPrescaledClock();
		while (main_timer_enabled && ((tscr1_register & 0x80) != 0)) {
			/**
			 * If the pulse accumulator is disabled (PACTL::PAEN=0) then
			 *     the prescaler clock from the timer is always used as an input clock to the timer counter.
			 * else
			 *     the pulse accumulator clock (PACLK) is used depending on PACTL::CLK[1:] bits configuration.
			 *
			 * The change from one selected clock to the other  happens immediately after these bits are written.
			 */
			if ((pactl_register & 0x40) == 0) {

				wait(prescaled_clock);

			} else {
				wait(paclk_event);
			}

			/**
			 * Note:
			 * If register TC7=0x0000 and TSCR2::TCRE=1, then the TCNT register will stay at 0x0000 continuously.
			 * If register TC7=0xFFFF and TSCR2::TCRE=1, then TFLG2::TOF flag will never be set when TCNT is reset from 0xFFFF to 0x0000.
			 */
			if (tcnt_register == 0xFFFF) {
				// Timer overflow
				tcnt_register = 0x0000;
				computeTimerPrescaledClock();

				for (uint8_t i=0; i<8; i++) {
					if (!isInputCapture(i) && ((ttov_register & (1 << i)) != 0)) {
						runChannelOutputCompareAction(i);
					}
				}

				if ((tc_registers[7] != 0xFFFF) || !isTimerCounterResetEnabled()) {
					// set TFLG2::TOF timer overflow flag
					tflg2_register = tflg2_register | 0x80;
				}

				assertInterrupt(offset_timer_overflow_interrupt);

			} else {
				if ((tc_registers[7] != 0x0000) || !isTimerCounterResetEnabled()) {
					tcnt_register = tcnt_register + 1;
				}
			}

			runOutputCompare();
		}
	}
}

void ECT::runDownCounter() {

	/**
	 * The 16-bit modulus down-counter can control the transfer of the IC registers and the pulse accumulators
	 * contents to the respective holding registers for a given period, every time the count reaches zero.
	 *
	 * The modulus down-counter can also be used as a stand-alone time base with periodic interrupt capability.
	 */

	while (true) {
		while ((!down_counter_enabled) || ((mcctl_register & 0x04) == 0)) {
			wait(down_counter_enable_event);
		}

		mccnt_register = mccnt_load_register;
		computeModulusCounterClock();

		while ((down_counter_enabled) && ((mcctl_register & 0x04) != 0)) {

			wait(mccnt_clock);

			mccnt_register = mccnt_register - 1;
			if (mccnt_register == 0) {
				// set MCFLG::MCZF flag
				mcflg_register = mcflg_register | 0x80;

				latchToHoldingRegisters();

				assertInterrupt(modulus_counter_interrupt);

				if ((mcctl_register & 0x40) != 0) {
					mccnt_register = mccnt_load_register;
					computeModulusCounterClock();
				}
			}
		}

	}
}

void ECT::runOutputCompare() {
	for (unsigned int i=0; i<8; i++) {
		if (!isInputCapture(i)) {
			ioc_channel[i]->runOutputCompare();
		}
	}
}

void ECT::assertInterrupt(unsigned int interrupt_offset) {

	if ( ((interrupt_offset >= (offset_channel0_interrupt - 0xE)) && (interrupt_offset <= offset_channel0_interrupt ) && isInputOutputInterruptEnabled((offset_channel0_interrupt - interrupt_offset)/2))
		|| ((interrupt_offset == offset_timer_overflow_interrupt) && isTimerOverflowinterruptEnabled())
		|| ((interrupt_offset == pulse_accumulatorA_overflow_interrupt) && ((pactl_register & 0x02) != 0))
		|| ((interrupt_offset == pulse_accumulatorB_overflow_interrupt) && ((pbctl_register & 0x02) != 0))
		|| ((interrupt_offset == pulse_accumulatorA_input_edge_interrupt) && ((pactl_register & 0x01) != 0))
		|| ((interrupt_offset == modulus_counter_interrupt) && (mcctl_register & 0x80) != 0) )
	{
		tlm_phase phase = BEGIN_REQ;

		xint_payload->acquire();

		xint_payload->setInterruptOffset(interrupt_offset);

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

}

// Master methods
void ECT::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

// Master methods
tlm_sync_enum ECT::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}


void ECT::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();
	unsigned int data_length = trans.get_data_length();

	if ((address >= baseAddress) && (address < (baseAddress + (unsigned int) MEMORY_MAP_SIZE))) {

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

bool ECT::read(unsigned int offset, const void *buffer, unsigned int data_length) {

	switch (offset) {
		case TIOS: {
			*((uint8_t *)buffer) =  tios_register;
		} break;
		case CFORC: {
			*((uint8_t *)buffer) = 0x00;
		} break;
		case OC7M: {
			*((uint8_t *)buffer) = oc7m_register;
		} break;
		case OC7D: {
			*((uint8_t *)buffer) =  oc7d_register;
		} break;
		case TCNT_HIGH: {
			if (data_length == 2) {
				*((uint16_t *)buffer) = Host2BigEndian(tcnt_register);
			} else {
				*((uint8_t *)buffer) = tcnt_register >> 8;
			}
			if ((tscr1_register & 0x10) != 0) {
				tflg2_register = tflg2_register & 0x7F;
			}
		} break;
		case TCNT_LOW: {
			*((uint8_t *)buffer) = tcnt_register & 0x00FF;
			if ((tscr1_register & 0x10) != 0) {
				tflg2_register = tflg2_register & 0x7F;
			}
		} break;
		case TSCR1: {
			*((uint8_t *)buffer) = tscr1_register & 0xF8;
		} break;
		case TTOV: {
			*((uint8_t *)buffer) = ttov_register;
		} break;
		case TCTL1: {
			if (data_length == 2) {
				*((uint16_t *)buffer) = Host2BigEndian(tctl12_register);
			} else {
				*((uint8_t *)buffer) =  tctl12_register >> 8;
			}

		} break;
		case TCTL2: {
			*((uint8_t *)buffer) =  tctl12_register & 0x00FF;
		} break;
		case TCTL3: {
			if (data_length == 2) {
				*((uint16_t *)buffer) = Host2BigEndian(tctl34_register);
			} else {
				*((uint8_t *)buffer) = tctl34_register >> 8;
			}

		} break;
		case TCTL4: {
			*((uint8_t *)buffer) = tctl34_register & 0x00FF;
		} break;
		case TIE: {
			*((uint8_t *)buffer) =  tie_register;
		} break;
		case TSCR2: {
			*((uint8_t *)buffer) =  tscr2_register & 0x8F;
		} break;
		case TFLG1: {
			*((uint8_t *)buffer) = tflg1_register;
		} break;
		case TFLG2: {
			*((uint8_t *)buffer) = tflg2_register;
		} break;
		case PACTL: {
			*((uint8_t *)buffer) =  pactl_register & 0x7F;
		} break;
		case PAFLG: {
			*((uint8_t *)buffer) = paflg_register & 0x03;
		} break;
		case PACN3: {
			if (data_length == 2) {
				*((uint16_t *)buffer) = Host2BigEndian((((uint16_t) pacn_register[3]) << 8) | pacn_register[2]);
			} else {
				*((uint8_t *)buffer) = pacn_register[3];
			}
			if ((tscr1_register & 0x10) != 0) {
				paflg_register = paflg_register & 0xFC;
			}

		} break;
		case PACN2: {
			*((uint8_t *)buffer) = pacn_register[2];
			if ((tscr1_register & 0x10) != 0) {
				paflg_register = paflg_register & 0xFC;
			}

		} break;
		case PACN1: {
			if (data_length == 2) {
				*((uint16_t *)buffer) = Host2BigEndian((((uint16_t) pacn_register[1]) << 8) | pacn_register[0]);
			} else {
				*((uint8_t *)buffer) = pacn_register[1];
			}
			if ((tscr1_register & 0x10) != 0) {
				pbflg_register = pbflg_register & 0xFD;
			}

		} break;
		case PACN0: {
			*((uint8_t *)buffer) = pacn_register[0];
			if ((tscr1_register & 0x10) != 0) {
				pbflg_register = pbflg_register & 0xFD;
			}
		} break;
		case MCCTL: {
			*((uint8_t *)buffer) = mcctl_register & 0xE7;
		} break;
		case MCFLG: {
			*((uint8_t *)buffer) = mcflg_register & 0x8F;
		} break;
		case ICPAR: {
			*((uint8_t *)buffer) = icpar_register & 0x0F;
		} break;
		case DLYCT: {
			*((uint8_t *)buffer) = dlyct_register;
		} break;
		case ICOVW: {
			*((uint8_t *)buffer) = icovw_register;
		} break;
		case ICSYS: {
			*((uint8_t *)buffer) = icsys_register;
		} break;
		case RESERVED: { /* Reserved Address */} break;
		case TIMTST: { /* Timer Test Register */} break;
		case PTPSR: {
			*((uint8_t *)buffer) = ptpsr_register;
		} break;
		case PTMCPSR: {
			*((uint8_t *)buffer) = ptmcpsr_register;
		} break;
		case PBCTL: {
			*((uint8_t *)buffer) = pbctl_register & 0x42;
		} break;
		case PBFLG: {
			*((uint8_t *)buffer) = pbflg_register & 0x02;
		} break;

		default: {

			if ((offset == MCCNT_HIGH) || (offset == MCCNT_LOW)) {
				uint16_t readed_value = mccnt_register;
				if ((mcctl_register & 0x20) != 0) {
					readed_value = mccnt_load_register;
				}

				if ((tscr1_register & 0x10) != 0) {
					mcflg_register = mcflg_register & 0x7F;
				}

				if (offset == MCCNT_HIGH) {
					if (data_length == 2) {
						*((uint16_t *)buffer) = Host2BigEndian(readed_value);
					} else {
						*((uint8_t *)buffer) = readed_value >> 8;
					}
				} else {
					*((uint8_t *)buffer) = readed_value & 0x00FF;
				}
			} else if ((offset >= TC0_HIGH) && (offset <= TC7_LOW)) {
				/**
				 * The IC channels are composed of four standard IC registers and four buffered IC channels.
				 *  - An IC register is empty when it has been read or latched into the holding register.
				 *  - A holding register is empty when it has been read
				 */
				uint8_t tc_offset = offset - TC0_HIGH;
				uint8_t tc_offset_index = tc_offset/2;
				if ((tc_offset % 2) == 0) // TCx_High ?
				{
					if (data_length == 2) {
						*((uint16_t *)buffer) = Host2BigEndian(tc_registers[tc_offset_index]);
						tc_registers[tc_offset_index] = 0x0000;
					} else {
						*((uint8_t *)buffer) =  tc_registers[tc_offset_index] >> 8;
						tc_registers[tc_offset_index] = tc_registers[tc_offset_index] & 0x00FF;
					}
				} else {
					*((uint8_t *)buffer) =  tc_registers[tc_offset_index] & 0x00FF;
					tc_registers[tc_offset_index] = tc_registers[tc_offset_index] & 0xFF00;
				}

				if ((tscr1_register & 0x10) != 0) {
					tflg1_register = tflg1_register & ~(1 << (tc_offset_index));
				}
			}
			else if ((offset >= PA3H) && (offset <= PA0H)) {
				uint8_t paxh_offset = offset - PA3H;
				*((uint8_t *)buffer) = paxh_registers[paxh_offset];
			}
			else if ((offset >= TC0H_HIGH) && (offset <= TC3H_LOW)) {
				uint8_t tcxh_offset = offset - TC0H_HIGH;
				uint8_t tcxh_offset_index = tcxh_offset/2;

				if ((tcxh_offset % 2) == 0) // TCxH_High ?
				{
					if (data_length == 2) {
						*((uint16_t *)buffer) = Host2BigEndian(tcxh_registers[tcxh_offset_index]);
						tcxh_registers[tcxh_offset_index] = 0x0000;
					} else {
						*((uint8_t *)buffer) = tcxh_registers[tcxh_offset_index] >> 8;
						tcxh_registers[tcxh_offset_index] = tcxh_registers[tcxh_offset_index] & 0x00FF;
					}

				} else {
					*((uint8_t *)buffer) = tcxh_registers[tcxh_offset_index] & 0x00FF;
					tcxh_registers[tcxh_offset_index] = tcxh_registers[tcxh_offset_index] & 0xFF00;
				}
				/**
				 * In queue mode, reads of the holding register will latch the corresponding pulse accumulator value to its holding register
				 */
				if (((tcxh_offset/2) < 4) && !isLatchMode()) {
					paxh_registers[tcxh_offset/2] = pacn_register[tcxh_offset/2];
					pacn_register[tcxh_offset/2] = 0x00;
				}

				if ((tscr1_register & 0x10) != 0) {
					tflg1_register = tflg1_register & ~(1 << (tcxh_offset/2));
				}

			} else {
				return (false);
			}

		}
		break;
	}

	return (true);
}

bool ECT::write(unsigned int offset, const void *buffer, unsigned int data_length) {

	if (debug_enabled) {
		std::cout << sc_time_stamp() << "  " << sc_object::name() << ":: write at " << std::dec << offset << "  value 0x" << std::hex << ((data_length==2)? BigEndian2Host(*((uint16_t*) buffer)): *((uint8_t*) buffer)) << std::dec << std::endl;
	}

	switch (offset) {
		case TIOS: {
			 tios_register = *((uint8_t *)buffer);
		} break;
		case CFORC: {
			cforc_register = *((uint8_t *)buffer);

			for (uint8_t i=0; i<8; i++) {
				if (!isInputCapture(i) && ((cforc_register & (1 << i)) != 0)) {
					runChannelOutputCompareAction(i);
				}
			}

			// 1 state is transient in the case of CFORC register
			cforc_register = 0;

		} break;
		case OC7M: {
			oc7m_register = *((uint8_t *)buffer);
		} break;
		case OC7D: {
			oc7d_register = *((uint8_t *)buffer);
		} break;
		case TCNT_HIGH: {

			*logger << DebugWarning << "Try writing to TCNT High register! Has no meaning or effect." << std::endl << EndDebugWarning;

			if (data_length == 2) {
				tcnt_register = BigEndian2Host(*((uint16_t *)buffer));
			} else {
				tcnt_register = (tcnt_register & 0x00FF) | ((uint16_t) *((uint8_t *)buffer) << 8);
			}

			if ((tscr1_register & 0x10) != 0) {
				tflg2_register = tflg2_register & 0x7F;
			}

		} break;
		case TCNT_LOW: {
			*logger << DebugWarning << "Try writing to TCNT High register! Has no meaning or effect." << std::endl << EndDebugWarning;

			tcnt_register = (tcnt_register & 0xFF00) | *((uint8_t *)buffer);

			if ((tscr1_register & 0x10) != 0) {
				tflg2_register = tflg2_register & 0x7F;
			}

		} break;
		case TSCR1: {
			uint8_t old_prnt_bit = tscr1_register & 0x08;
			*((uint8_t *)buffer) = (tscr1_register & 0x07) | (*((uint8_t *)buffer) & 0xF8);

			if (prnt_write) {
				*((uint8_t *)buffer) = (*((uint8_t *)buffer) & 0xF7) | old_prnt_bit;
			} else {
				prnt_write = true;
			}
			tscr1_register = *((uint8_t *)buffer);

			if ((tscr1_register & 0x80) != 0) {
				main_timer_enable();
			} else {
				main_timer_disable();
			}

		} break;
		case TTOV: {
			ttov_register = *((uint8_t *)buffer);
		} break;
		case TCTL1: {
			if (data_length == 2) {
				tctl12_register = BigEndian2Host(*((uint16_t *)buffer));
			} else {
				tctl12_register = (tctl12_register & 0x00FF) | ((uint16_t) *((uint8_t *)buffer) << 8);
			}
			configureOutputAction();

		} break;
		case TCTL2: {
			tctl12_register = (tctl12_register & 0xFF00) | *((uint8_t *)buffer);
			configureOutputAction();

		} break;
		case TCTL3: {
			if (data_length == 2) {
				tctl34_register = BigEndian2Host(*((uint16_t *)buffer));
			} else {
				tctl34_register = (tctl34_register & 0x00FF) | ((uint16_t) *((uint8_t *)buffer) << 8);
			}
			configureEdgeDetector();
		} break;
		case TCTL4: {
			tctl34_register = (tctl34_register & 0xFF00) | *((uint8_t *)buffer);
			configureEdgeDetector();
		} break;
		case TIE: {
			tie_register = *((uint8_t *)buffer);
		} break;
		case TSCR2: {
			tscr2_register = (tscr2_register & 0x70) | (*((uint8_t *)buffer) & 0x8F);
			computeTimerPrescaledClock();

		} break;
		case TFLG1: {
			// writing a one to the flag clears the flag.
			// writing a zero will not affect the current status of the bit
			// when TSCR1::TFFCA=1, the flag cannot be cleared via the normal flag clearing mechanism

			// chech TSCR1::TFFCA bit is cleared
			if ((tscr1_register & 0x10) == 0) {
				uint8_t val = *((uint8_t *)buffer);
				tflg1_register = tflg1_register & ~val;
			}
		} break;
		case TFLG2: {
			// writing a one to the flag clears the flag.
			// writing a zero will not affect the current status of the bit
			// when TSCR1::TFFCA=1, the flag cannot be cleared via the normal flag clearing mechanism

			if ((tscr1_register & 0x10) == 0) {
				uint8_t val = (*((uint8_t *)buffer) & 0x80);

				tflg2_register = tflg2_register & ~val;
			}

		} break;
		case PACTL: {
			uint8_t masked_value = (pactl_register & 0x80) | (*((uint8_t *)buffer) & 0x7F);
			pactl_register = masked_value;

			switch ((pactl_register & 0x30) >> 4) {
				case 0:  // Falling edge
				{
					// For edge detector configuration, I use the same codification as for TCTL3/TCTL4 register
					pacA->setValideEdge(2);
					pacA->setMode(false);
				} break;
				case 1: // Rising edge
				{
					// For edge detector configuration, I use the same codification as for TCTL3/TCTL4 register
					pacA->setValideEdge(1);
					pacA->setMode(false);
				} break;
				case 2: // Divide by 64 clock enabled with pin high level
				{
					pacA->setValideEdge(1); // I use rising edge code for High level
					pacA->setMode(true);
				} break;
				case 3: // Divide by 64 clock enabled with pin low level
				{
					pacA->setValideEdge(2); // I use falling edge code for Low level
					pacA->setMode(true);
				} break;


			}

			if ((pactl_register & 0x40) != 0) {
				pacA->wakeup();
			}


		} break;
		case PAFLG: {
			// writing a one to the flag clears the flag.
			// writing a zero will not affect the current status of the bit
			// when TSCR1::TFFCA=1, the flag cannot be cleared via the normal flag clearing mechanism

			if ((tscr1_register & 0x10) == 0) {
				uint8_t val = (*((uint8_t *)buffer) & 0x03);
				paflg_register = paflg_register & ~val;
			}
		} break;
		case PACN3: {
			if (data_length == 2) {
				pacn_register[3] = BigEndian2Host(*((uint16_t *)buffer)) >> 8;
				pacn_register[2] = BigEndian2Host(*((uint16_t *)buffer)) & 0x00FF;
			} else {
				pacn_register[3] = *((uint8_t *)buffer);
			}
			if ((tscr1_register & 0x10) != 0) {
				paflg_register = paflg_register & 0xFC;
			}

		} break;
		case PACN2: {
			pacn_register[2] = *((uint8_t *)buffer);
			if ((tscr1_register & 0x10) != 0) {
				paflg_register = paflg_register & 0xFC;
			}
		} break;
		case PACN1: {
			if (data_length == 2) {
				pacn_register[1] = BigEndian2Host(*((uint16_t *)buffer)) >> 8;
				pacn_register[0] = BigEndian2Host(*((uint16_t *)buffer)) & 0x00FF;
			} else {
				pacn_register[1] = *((uint8_t *)buffer);
			}
			if ((tscr1_register & 0x10) != 0) {
				pbflg_register = pbflg_register & 0xFD;
			}

		} break;
		case PACN0: {
			pacn_register[0] = *((uint8_t *)buffer);
			if ((tscr1_register & 0x10) != 0) {
				pbflg_register = pbflg_register & 0xFD;
			}
		} break;
		case MCCTL: {
			mcctl_register = *((uint8_t *)buffer);

			// MCCTL::ICLAT input Capture Force latch Action ?
			if ((mcctl_register & 0x10) !=0) {
				latchToHoldingRegisters();
				mcctl_register = mcctl_register & 0xEF;
			}

			// Force Load Register into the Modulus Counter count register ? (MCCTL::FLMC=1 and MCCTL::MCEN=1)
			if ((mcctl_register && 0x08) != 0) {
				if ((mcctl_register & 0x04) != 0) {
					/**
					 * Loads the load register into the modulus counter count register (MCCNT).
					 * And resets the modulus counter prescaler.
					 */
					mccnt_register = mccnt_load_register;
				}
				mcctl_register = mcctl_register & 0xF7;
			}

			if ((mcctl_register & 0x04) == 0) {
				down_counter_disable();
				mccnt_register = 0xFFFF;
			} else {
				down_counter_enable();
			}
		} break;
		case MCFLG: {
			if ((tscr1_register & 0x10) == 0) {
				uint8_t val = *((uint8_t *)buffer) & 0x80;
				mcflg_register = mcflg_register & ~val;
			}

		} break;
		case ICPAR: {
			icpar_register = (icpar_register & 0xF0) | (*((uint8_t *)buffer) & 0x0F);
		} break;
		case DLYCT: {
			dlyct_register = *((uint8_t *)buffer);
			computeDelayCounter();
		} break;
		case ICOVW: {
			icovw_register = *((uint8_t *)buffer);
		} break;
		case ICSYS: {
			if (icsys_write) {
				*logger << DebugWarning << "ICSYS register has already been written! This register is write once in normal modes." << std::endl << EndDebugWarning;
			} else {
				icsys_write = true;
				icsys_register = *((uint8_t *)buffer);
			}

		} break;
		case RESERVED: { /* Reserved Address */} break;
		case TIMTST: { /* Timer Test Register */} break;
		case PTPSR: {
			ptpsr_register = *((uint8_t *)buffer);
		} break;
		case PTMCPSR: {
			ptmcpsr_register = *((uint8_t *)buffer);
		} break;
		case PBCTL: {
			pbctl_register = (pbctl_register & 0xBD) | (*((uint8_t *)buffer) & 0x42);
			 pacB->wakeup();

		} break;
		case PBFLG: {
			// when TSCR1::TFFCA=1, the flag cannot be cleared via the normal flag clearing mechanism
			if ((tscr1_register & 0x10) == 0) {
				uint8_t val = *((uint8_t *)buffer) & 0x02;
				pbflg_register = pbflg_register & ~val;
			}

		} break;

		default: {
			if ((offset == MCCNT_HIGH) || (offset == MCCNT_LOW)) {
				uint16_t old_mccnt_register = mccnt_register;

				if ((tscr1_register & 0x10) != 0) {
					mcflg_register = mcflg_register & 0x7F;
				}

				if (offset == MCCNT_HIGH) {
					if (data_length == 2) {
						mccnt_load_register = BigEndian2Host(*((uint16_t *)buffer));
					} else {
						mccnt_load_register = (mccnt_load_register & 0x00FF) | ((uint16_t) (*((uint8_t *)buffer) << 8));
					}
				} else {
					mccnt_load_register = (mccnt_load_register & 0xFF00) | *((uint8_t *)buffer);
				}

				/**
				 * If the modulus down counter is enabled (MCCTL::MCEN=1) and modulus mode is enabled (MCCTL::MODMC=1),
				 * a write to MCCNT will update the load register with the value written to it. The count register
				 * will not be updated with the new value until the next counter underflow.
				 *
				 * The FLMC bit in MCCTL can be used to immediately update the count register with the new value
				 * if an immediate load is desired
				 *
				 *  If the modulus mode is not enabled (MCCTL::MODMC=0), a write to MCCNT will clear the modulus prescaler and
				 *  will immediately update the counter register with the value written to it
				 *  and down-counts to 0x0000 and stops.
				 *
				 */

				if (((mcctl_register & 0x44) != 0x44) || ((mcctl_register & 0x08) != 0)) {
					mccnt_register = mccnt_load_register;
					computeModulusCounterClock();

					if ((mcctl_register & 0x40) == 0) {
						mcctl_register = mcctl_register & 0xFC;
					}
				}

				/**
				 * If a 0x0000 is written into MCCNT when LATQ and BUFFEN in ICSYS register are set,
				 * the input capture and pulse accumulator registers will be latched.
				 */
				if ((mccnt_register == 0x0000) && (old_mccnt_register != 0)) {
					if ((icsys_register & 0x03) == 0x03) {
						for (unsigned int i=0; i < 3; i++) {
							ioc_channel[i]->latchToHoldingRegisters();
						}

					}
				}

			} else if ((offset >= TC0_HIGH) && (offset <= TC7_LOW)) {
				uint8_t tc_offset = offset - TC0_HIGH;
				uint8_t tc_offset_index = (tc_offset/2);
				uint8_t channel_mask = (1 << tc_offset_index);

				// is the channel configured for "input capture" ?
				if ((tios_register & channel_mask) == 0) {
					*logger << DebugWarning << "Channel " << std::dec << (tc_offset%2) << " is configured for <<input capture>>. Writing Has no meaning or effect." << std::endl << EndDebugWarning;
				}
				// The channel is configured for "output compare"
				else {
					if ((tc_offset % 2) == 0) // TCx_High ?
					{
						if (data_length == 2) {
							tc_registers[tc_offset_index] = BigEndian2Host(*((uint16_t *)buffer));
						} else {
							tc_registers[tc_offset_index] = (tc_registers[tc_offset_index] & 0x00FF) | ((uint16_t) *((uint8_t *)buffer) << 8);
						}

					} else {
						tc_registers[tc_offset_index] = (tc_registers[tc_offset_index] & 0xFF00) | *((uint8_t *)buffer);
					}
				}

				if ((tscr1_register & 0x10) != 0) {
					tflg1_register = tflg1_register & ~(1 << (tc_offset/2));
				}

			}
			else if ((offset >= PA3H) && (offset <= PA0H)) {
				/* don't accept write */;
			}
			else if ((offset >= TC0H_HIGH) && (offset <= TC3H_LOW)) {
				/* don't accept write */;
			} else {
				return (false);
			}
		}
		break;

	}

	return (true);
}

inline void ECT::configureOutputAction() {

	for (uint8_t i=0; i<8; i++) {
		uint8_t outputAction = ((uint16_t) (tctl12_register & (3 << (i*2))) >> (i*2)) & 0x03;

		ioc_channel[i]->setOutputAction(outputAction);
	}
}

inline void ECT::configureEdgeDetector() {

	for (uint8_t i=0; i<8; i++) {
		uint8_t edgeConfig = ((uint16_t) (tctl34_register & (3 << (i*2))) >> (i*2)) & 0x03;

		ioc_channel[i]->setValideEdge(edgeConfig);
	}
}

inline void ECT::computeDelayCounter() {
	// TSCR1::PRNT = 0 ?
	if ((tscr1_register & 0x08) == 0) {
		// 32 * 2^dly10 * 4 = {disable,  256, 512, 1024} bus cycles
		uint8_t dly10 = dlyct_register & 0x03;
		if (dly10 == 0) {
			delay_counter_disable();

		} else {
			edge_delay_counter = 32 * pow((double)2, dly10) * 4;
			edge_delay_counter_time = bus_cycle_time * edge_delay_counter;
			delay_counter_enable();
		}
	} else {
		// (dly70 + 1) * 4 = {disable, 8, ..., 1024} bus cycles
		if (dlyct_register == 0) {
			delay_counter_disable();
		} else {
			edge_delay_counter = (dlyct_register + 1) * 4;
			edge_delay_counter_time = bus_cycle_time * edge_delay_counter;
			delay_counter_enable();
		}
	}
}

ECT::PulseAccumulator8Bit::PulseAccumulator8Bit(ECT *parent, const uint8_t pacn_number, uint8_t *pacn_ptr, uint8_t* pah_ptr) :
	  pacn_index(pacn_number)
	, ectParent(parent)
	, pacn_register_ptr(pacn_ptr)
	, pah_register_ptr(pah_ptr)

{

}

bool ECT::PulseAccumulator8Bit::countEdge8Bit() {

	bool overflow = false;

	if (*pacn_register_ptr == 0xFF) {
		overflow = true;
		if (!ectParent->isPulseAccumulatorsMaximumCount()) {
			*pacn_register_ptr = 0x00;
		}
		if (pacn_index == 3) {
			ectParent->setPulseAccumulatorAOverflowFlag();
			ectParent->assertInterrupt(ectParent->getInterruptPulseAccumulatorAOverflow());
		} else if (pacn_index == 1) {
			ectParent->setPulseAccumulatorBOverflowFlag();
			ectParent->assertInterrupt(ectParent->getInterruptPulseAccumulatorBOverflow());
		}
	} else {
		*pacn_register_ptr = *pacn_register_ptr + 1;
	}

	return (overflow);
}

void ECT::PulseAccumulator8Bit::latchToHoldingRegisters() {

	*pah_register_ptr = *pacn_register_ptr;
	*pacn_register_ptr = 0x00;
}

ECT::PulseAccumulator16Bit::PulseAccumulator16Bit(const sc_module_name& name, ECT *parent, bool* pinLogic, PulseAccumulator8Bit *pacn_high, PulseAccumulator8Bit *pacn_low) :
   	  sc_module(name)
	, ectParent(parent)
	, channelPinLogic(pinLogic)
	, pacn_high_ptr(pacn_high)
	, pacn_low_ptr(pacn_low)

{

	SC_HAS_PROCESS(ECT::PulseAccumulator16Bit);

	SC_THREAD(process);
//	sensitive << pulse_accumulator_enable_event;

}

void ECT::PulseAccumulator16Bit::process() {
	runPulseAccumulator();
}

void ECT::PulseAccumulator16Bit::latchToHoldingRegisters() {
	pacn_high_ptr->latchToHoldingRegisters();
	pacn_low_ptr->latchToHoldingRegisters();

}

ECT::PulseAccumulatorA::PulseAccumulatorA(const sc_module_name& name, ECT *parent, bool* pinLogic, PulseAccumulator8Bit *pacn_high, PulseAccumulator8Bit *pacn_low) :
	ECT::PulseAccumulator16Bit::PulseAccumulator16Bit(name, parent, pinLogic, pacn_high, pacn_low)
	, isGatedTimeMode(false), valideEdge(0)
{

}

void ECT::PulseAccumulatorA::runPulseAccumulator() {

	while (true) {

		while (!ectParent->isPulseAccumulatorAEnabled()) {
			wait(pulse_accumulator_enable_event);
		}

		/**
		 *  check PACTL::PAMOD
		 *  0-> Event counter mode
		 *  1-> Gated time accumulator mode
		 */

		/**
		 * For PAMOD bit = 0 (event counter mode).
		 *   PACTL::PEDGE = 0 Falling edges on PT7 pin cause the count to be incremented
		 *   PACTL::PEDGE = 1 Rising edges on PT7 pin cause the count to be incremented
		 *
		 * For PAMOD bit = 1 (gated time accumulation mode).
		 *   PACTL::PEDGE = 0 PT7 input pin high enables bus clock divided by 64 to Pulse Accumulator and
		 *     the trailing falling edge on PT7 sets the PAIF flag.
		 *   PACTL::PEDGE = 1 PT7 input pin low enables bus clock divided by 64 to Pulse Accumulator and
		 *     the trailing rising edge on PT7 sets the PAIF flag.
		 *
		 * If the timer is not active (TEN = 0 in TSCR1), there is no divide-by-64 since the ÷64 clock is generated by the timer prescaler
		 */

		wait(edge_event);

		if (!ectParent->isValidEdge(7)) { continue; }

		if (isGatedTimeModeEnabled())  // if (PACTL::PAMOD != 0) then gated time accumulation mode
		{

			bool detected_signal = *channelPinLogic;

			if (ectParent->isTimerEnabled()) {
				wait(gate_time);
			} else {
				wait(ectParent->getBusClock());
			}

			if (detected_signal != *channelPinLogic) {
				continue;
			}
		}

		/**
		 * PAFLG::PAIF Pulse Accumulator Input edge Flag - Set when the selected edge is detected at PT7 input pin.
		 * In event mode the event edge triggers PAIF,
		 * and in gated time accumulation mode the trailing edge of the gate signal at the PT7 input pin triggers PAIF.
		 */
		ectParent->setPulseAccumulatorAInputEdgeFlag();

		uint8_t clksel = ectParent->getClockSelection();

		if (clksel == 1) {
			// Main timer counter use PACLK as input to timer counter clock
			ectParent->notify_paclk_event();
		}

		bool pacn_low_overflow = pacn_low_ptr->countEdge8Bit();
    	if (pacn_low_overflow) {
			if (clksel == 2) {
				// Main timer counter use PACLK/256 as timer counter clock frequency
				ectParent->notify_paclk_event();
			}

    		pacn_low_ptr->clearPACN();
    		bool pacn_high_overflow = pacn_high_ptr->countEdge8Bit();

			if (pacn_high_overflow && (clksel == 3)) {
				// Main Timer counter use PACLK/65536 as timer counter clock frequency
				ectParent->notify_paclk_event();
			}

    	}

	}

}


ECT::PulseAccumulatorB::PulseAccumulatorB(const sc_module_name& name, ECT *parent, bool* pinLogic, PulseAccumulator8Bit *pacn_high, PulseAccumulator8Bit *pacn_low) :
	ECT::PulseAccumulator16Bit::PulseAccumulator16Bit(name, parent, pinLogic, pacn_high, pacn_low)
{

}

void ECT::PulseAccumulatorB::runPulseAccumulator() {
	while (true) {

		while (!ectParent->isPulseAccumulatorBEnabled()) {
			wait(pulse_accumulator_enable_event);
		}

		wait(edge_event);

		if (!ectParent->isValidEdge(0)) { continue; }
		if (ectParent->isDelayCounterEnabled()) {
			bool detected_signal = *channelPinLogic;

			wait(ectParent->getEdgeDelayCounter());

			if (detected_signal != *channelPinLogic) {
				continue;
			}
		}

		if (pacn_low_ptr->countEdge8Bit()) {
			pacn_low_ptr->clearPACN();
			pacn_high_ptr->countEdge8Bit();
		}
	}

}

inline void ECT::latchToHoldingRegisters() {
	/**
	 * The contents of the input capture registers TC0 to TC3
	 * and their corresponding 8-bit pulse accumulators to be latched into the associated holding registers.
	 * The pulse accumulators will be automatically cleared when the latch action occurs;
	 */

	// is latch mode enabled (ICSYS::LATQ=1 and ICSYS::BUFFEN=1)
	if (isLatchMode() && isBufferEnabled()) {
		for (unsigned int i=0; i<4; i++) {
			ioc_channel[i]->latchToHoldingRegisters();
		}
	}
}

//=====================================================
//=             ECT Channel methods                   =
//=====================================================

ECT::IOC_Channel_t::IOC_Channel_t(const sc_module_name& name, ECT *parent, const uint8_t index, bool* pinLogic, uint16_t *tc_ptr, uint16_t* tch_ptr, PulseAccumulator8Bit* pc8bit) :
	  sc_module(name)
	, ectParent(parent)

	, edge_event()
	, shared_edge_event()

	, ioc_index(index)
	, valideEdge(1)
	, outputAction(0)

	, tc_register_ptr(tc_ptr)
	, tch_register_ptr(tch_ptr)
	, pulseAccumulator(pc8bit)
	, channelPinLogic(pinLogic)

{

	iocMask = (0x01 << ioc_index);

	SC_HAS_PROCESS(IOC_Channel_t);

	SC_THREAD(runInputCapture);
//	sensitive << edge_event;

}

/**
 * This thread emulate the edge detector using :
 * - Edge configuration TCL3/TCL4,
 * - Edge sharing ICSYS
 * - Delay DLYCT
 *
 * 0: Capture disabled
 * 1: Capture on rising edges only
 * 2: Capture on falling edges only
 * 3: Capture on any edge (rising or falling)
 */

void ECT::IOC_Channel_t::runInputCapture() {

	while (true) {

		wait(edge_event);

		if ((ioc_index < 4) || ((ioc_index > 3) && !ectParent->isSharingEdgeEnabled(1 << ioc_index))) {

			if (!ectParent->isValidEdge(ioc_index)) { continue; }
			if ((ioc_index < 4) && ectParent->isDelayCounterEnabled()) {
				bool detected_signal = *channelPinLogic;

				wait(ectParent->getEdgeDelayCounter());

				if (detected_signal != *channelPinLogic) {
					continue;
				}
			}

			// is sharing edge enabled ?
			if ((ioc_index < 4) && ectParent->isSharingEdgeEnabled(1 << (ioc_index+4))) {
				ectParent->notifySharedEdgeTo(ioc_index+4);
			}

		}


		if (ioc_index < 4) {
			/**
			 * MCFLG::POLF[3:0] First Input Capture polarity status.
			 * These are read only bits. Writes to these bits have no effect.
			 * Each status bit gives the polarity of the first edge which has caused
			 * an input capture to occur after capture latch has been read.
			 *
			 * An IC register is empty when it has been read or latched into the holding register.
			 */
			if (*tc_register_ptr == 0) {
				ectParent->setPOLF(ioc_index, channelPinLogic);
			}
		}

		// Non-Buffered IC channels
		if (tch_register_ptr == NULL) {
			if (!ectParent->isNoInputCaptureOverWrite(ioc_index) || (*tc_register_ptr == 0x0000)) {
				*tc_register_ptr = ectParent->getMainTimerValue();
				// set the TFLG1::CxF to show that input capture occurs
				if (ectParent->isInputOutputInterruptEnabled(ioc_index)) {
					std::cout << sc_time_stamp() << "  " << sc_object::name() << " runInputCapture (1) 0x" << std::hex << ectParent->getInterruptOffsetChannel0() - (ioc_index * 2) << std::endl;
					ectParent->setTimerInterruptFlag(ioc_index);
					ectParent->assertInterrupt(ectParent->getInterruptOffsetChannel0() - (ioc_index * 2));
				}
			}
		}
		else //  buffered IC Channels
		{
			if (ectParent->isPulseAccumulators8BitEnabled(pulseAccumulator->getIndex())) {
				pulseAccumulator->countEdge8Bit();
			}

			// IC latch mode (ICSYS::LATQ=1)
			if (ectParent->isLatchMode()) {
				if (!ectParent->isNoInputCaptureOverWrite(ioc_index) || ((*tc_register_ptr == 0x0000) && (*tch_register_ptr == 0x0000))) {
					// In case of latching, the contents of the holding register are overwritten
					*tc_register_ptr = ectParent->getMainTimerValue();
					if (ectParent->isBufferEnabled()) {
						*tch_register_ptr = *tc_register_ptr;
					}
				}

				/**
				 *  ICSYS::TFMOD=0 : The timer flags C3F-C0F in the TFLG1 are set
				 *                 when a valid input capture transition on the corresponding port pin occurs.
				 *  ICSYS::LATQ=1  : If the queue mode is not engaged, the timer flags C3F–C0F are set the same way as for TFMOD = 0.
				 */
				if (ectParent->isInputOutputInterruptEnabled(ioc_index)) {
					std::cout << sc_time_stamp() << "  " << sc_object::name() << " runInputCapture (2) 0x" << std::hex << ectParent->getInterruptOffsetChannel0() - (ioc_index * 2) <<  std::endl;
					ectParent->setTimerInterruptFlag(ioc_index);
					ectParent->assertInterrupt(ectParent->getInterruptOffsetChannel0() - (ioc_index * 2));
				}

			}
			else // IC Queue Mode (ICSYS::LATQ=0)
			{
				if (!ectParent->isNoInputCaptureOverWrite(ioc_index) || ((*tc_register_ptr == 0x0000) && (*tch_register_ptr == 0x0000))) {
					if (ectParent->isBufferEnabled()) {
						*tch_register_ptr = *tc_register_ptr;
						/**
						 *  ICSYS::TFMOD=1 : If in queue mode (BUFEN = 1 and LATQ = 0), the timer flags C3F–C0F in TFLG1 are set only when a latch
						 *                      on the corresponding holding register occurs.
						 *                   If the queue mode is not engaged, the timer flags C3F–C0F are set the same way as for TFMOD = 0.
						 */

						if (ectParent->isTimerFlagSettingModeSet() && (*tch_register_ptr != 0x0000)) {
							if (ectParent->isInputOutputInterruptEnabled(ioc_index)) {
								std::cout << sc_time_stamp() << "  " << sc_object::name() << " runInputCapture (3) 0x" << std::hex << ectParent->getInterruptOffsetChannel0() - (ioc_index * 2) <<  std::endl;
								ectParent->setTimerInterruptFlag(ioc_index);
								ectParent->assertInterrupt(ectParent->getInterruptOffsetChannel0() - (ioc_index * 2));
							}
						}
					}
					*tc_register_ptr = ectParent->getMainTimerValue();
				}
			}

		}

	}

}

void ECT::IOC_Channel_t::latchToHoldingRegisters() {

	/**
	 * The contents of the input capture registers TC0 to TC3
	 * and their corresponding 8-bit pulse accumulators to be latched into the associated holding registers.
	 */

	*tch_register_ptr = *tc_register_ptr;
	*tc_register_ptr = 0x0000;

	pulseAccumulator->latchToHoldingRegisters();
}

void ECT::IOC_Channel_t::runOutputCompare() {

	bool result = (*tc_register_ptr == ectParent->getMainTimerValue());
	ectParent->setOC7Dx(ioc_index, result);
	if (result) {
		ectParent->runChannelOutputCompareAction(ioc_index);
		// set the TFLG1::CxF to show that output compare is detected
		if (ectParent->isInputOutputInterruptEnabled(ioc_index)) {
			std::cout << sc_time_stamp() << "  " << sc_object::name() << " ioc_index " << (unsigned int) ioc_index << " runOutputCompare (1) 0x" << std::hex << ectParent->getInterruptOffsetChannel0() - (ioc_index * 2) << std::endl;
			ectParent->setTimerInterruptFlag(ioc_index);
			ectParent->assertInterrupt(ectParent->getInterruptOffsetChannel0() - (ioc_index * 2));
		}

		/**
		 * if TSCR2::TCRE=1, then the timer counter is reset by a successful channel 7 output compare.
		 * This mode of operation is similar to an up-counting modulus counter.
		 */
		if ((ioc_index == 7) && ectParent->isTimerCounterResetEnabled()) {
			ectParent->resetTimerCounter();
		}
	}

}

void ECT::runChannelOutputCompareAction(uint8_t ioc_index) {
	/**
	 * This method is called by writing to "CFORC" register or by starting output compare thread or by main timer overflow.
	 * 1. Run the action which is programmed for output compare "x" to occur immediately.
	 * 2. The interrupt flag does not get set.
	 * 3. The forced output compare take precedence over a successful channel 7 output compare
	 */

	bool old_portt_value = portt_pin[ioc_index];

	switch (ioc_channel[ioc_index]->getOutputAction()) {
		case 0: /* Timer disconnected from output pin logic */ break;
		case 1: {
			// Toggle OCx output line

			/**
			 * check if channel corresponding OC7Dx bit in the output compare 7 data register (OC7D)
			 * will be transfered to the timer port on a successful channel 7 output compare ?
			 */
			if (isOutputCompareMaskSet(ioc_index) && !isInputCapture(ioc_index)) {
//				portt_pin[ioc_index] = getOC7Dx(ioc_index);
				portt_pin_reg[ioc_index] = getOC7Dx(ioc_index);
			}

		} break;
		case 2: {
			// Clear OCx output line to zero
			if (!isOutputCompareMaskSet(ioc_index)) {
//				portt_pin[ioc_index] = false;
				portt_pin_reg[ioc_index] = false;
			}
		} break;
		case 3: {
			// Set OCx output line to one
			if (!isOutputCompareMaskSet(ioc_index)) {
//				portt_pin[ioc_index] = true;
				portt_pin_reg[ioc_index] = true;
			}
		} break;
	}

	if (portt_pin[ioc_index] != old_portt_value) {
		if ((ioc_index == 0) && isPulseAccumulatorBEnabled()) {
			pacB->notifyEdge();
		}

		if ((ioc_index == 7) && isPulseAccumulatorAEnabled()) {
			pacA->notifyEdge();
		}

	}

}

//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================

bool ECT::BeginSetup() {

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".TIOS", &tios_register));

	unisim::kernel::variable::Register<uint8_t> *tios_var = new unisim::kernel::variable::Register<uint8_t>("TIOS", this, tios_register, "Timer Input Capture/Output Compare Select");
	extended_registers_registry.push_back(tios_var);
	tios_var->setCallBack(this, TIOS, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CFORC", &cforc_register));

	unisim::kernel::variable::Register<uint8_t> *cforc_var = new unisim::kernel::variable::Register<uint8_t>("CFORC", this, cforc_register, "Timer Compare Force Register");
	extended_registers_registry.push_back(cforc_var);
	cforc_var->setCallBack(this, CFORC, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".OC7M", &oc7m_register));

	unisim::kernel::variable::Register<uint8_t> *oc7m_var = new unisim::kernel::variable::Register<uint8_t>("OC7M", this, oc7m_register, "Output Compare 7 Mask Register");
	extended_registers_registry.push_back(oc7m_var);
	oc7m_var->setCallBack(this, OC7M, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".OC7D", &oc7d_register));

	unisim::kernel::variable::Register<uint8_t> *oc7d_var = new unisim::kernel::variable::Register<uint8_t>("OC7D", this, oc7d_register, "Output Compare 7 Data Register");
	extended_registers_registry.push_back(oc7d_var);
	oc7d_var->setCallBack(this, OC7D, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".TCNT", &tcnt_register));

	unisim::kernel::variable::Register<uint16_t> *tcnt_var = new unisim::kernel::variable::Register<uint16_t>("TCNT", this, tcnt_register, "Timer Count Register");
	extended_registers_registry.push_back(tcnt_var);
	tcnt_var->setCallBack(this, TCNT_HIGH, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".TSCR1", &tscr1_register));

	unisim::kernel::variable::Register<uint8_t> *tscr1_var = new unisim::kernel::variable::Register<uint8_t>("TSCR1", this, tscr1_register, "Timer System Control Register 1");
	extended_registers_registry.push_back(tscr1_var);
	tscr1_var->setCallBack(this, TSCR1, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".TTOV", &ttov_register));

	unisim::kernel::variable::Register<uint16_t> *ttov_var = new unisim::kernel::variable::Register<uint16_t>("TTOV", this, ttov_register, "Timer Toggle Overflow Register");
	extended_registers_registry.push_back(ttov_var);
	ttov_var->setCallBack(this, TTOV, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".TCTL12", &tctl12_register));

	unisim::kernel::variable::Register<uint16_t> *tctl12_var = new unisim::kernel::variable::Register<uint16_t>("TCTL12", this, tctl12_register, "Timer Control Register 1-2");
	extended_registers_registry.push_back(tctl12_var);
	tctl12_var->setCallBack(this, TCTL1, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".TCTL34", &tctl34_register));

	unisim::kernel::variable::Register<uint16_t> *tctl34_var = new unisim::kernel::variable::Register<uint16_t>("TCTL34", this, tctl34_register, "Timer Control Register 3-4");
	extended_registers_registry.push_back(tctl34_var);
	tctl34_var->setCallBack(this, TCTL3, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".TIE", &tie_register));

	unisim::kernel::variable::Register<uint8_t> *tie_var = new unisim::kernel::variable::Register<uint8_t>("TIE", this, tie_register, "Timer Interrupt Enable Register");
	extended_registers_registry.push_back(tie_var);
	tie_var->setCallBack(this, TIE, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".TSCR2", &tscr2_register));

	unisim::kernel::variable::Register<uint8_t> *tscr2_var = new unisim::kernel::variable::Register<uint8_t>("TSCR2", this, tscr2_register, "Timer System Control Register 2");
	extended_registers_registry.push_back(tscr2_var);
	tscr2_var->setCallBack(this, TSCR2, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".TFLG1", &tflg1_register));

	unisim::kernel::variable::Register<uint8_t> *tflg1_var = new unisim::kernel::variable::Register<uint8_t>("TFLG1", this, tflg1_register, "Main Timer Interrupt Flag 1");
	extended_registers_registry.push_back(tflg1_var);
	tflg1_var->setCallBack(this, TFLG1, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".TFLG2", &tflg2_register));

	unisim::kernel::variable::Register<uint8_t> *tflg2_var = new unisim::kernel::variable::Register<uint8_t>("TFLG2", this, tflg2_register, "Main Timer Interrupt Flag 2");
	extended_registers_registry.push_back(tflg2_var);
	tflg2_var->setCallBack(this, TFLG2, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".TC0", &tc_registers[0]));

	unisim::kernel::variable::Register<uint16_t> *tc0_var = new unisim::kernel::variable::Register<uint16_t>("TC0", this, tc_registers[0], "Timer Input Capture/Output Compare Register 0");
	extended_registers_registry.push_back(tc0_var);
	tc0_var->setCallBack(this, TC0_HIGH, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".TC1", &tc_registers[1]));

	unisim::kernel::variable::Register<uint16_t> *tc1_var = new unisim::kernel::variable::Register<uint16_t>("TC1", this, tc_registers[1], "Timer Input Capture/Output Compare Register 1");
	extended_registers_registry.push_back(tc1_var);
	tc1_var->setCallBack(this, TC1_HIGH, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".TC2", &tc_registers[2]));

	unisim::kernel::variable::Register<uint16_t> *tc2_var = new unisim::kernel::variable::Register<uint16_t>("TC2", this, tc_registers[2], "Timer Input Capture/Output Compare Register 2");
	extended_registers_registry.push_back(tc2_var);
	tc2_var->setCallBack(this, TC2_HIGH, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".TC3", &tc_registers[3]));

	unisim::kernel::variable::Register<uint16_t> *tc3_var = new unisim::kernel::variable::Register<uint16_t>("TC3", this, tc_registers[3], "Timer Input Capture/Output Compare Register 3");
	extended_registers_registry.push_back(tc3_var);
	tc3_var->setCallBack(this, TC3_HIGH, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".TC4", &tc_registers[4]));

	unisim::kernel::variable::Register<uint16_t> *tc4_var = new unisim::kernel::variable::Register<uint16_t>("TC4", this, tc_registers[4], "Timer Input Capture/Output Compare Register 4");
	extended_registers_registry.push_back(tc4_var);
	tc4_var->setCallBack(this, TC4_HIGH, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".TC5", &tc_registers[5]));

	unisim::kernel::variable::Register<uint16_t> *tc5_var = new unisim::kernel::variable::Register<uint16_t>("TC5", this, tc_registers[5], "Timer Input Capture/Output Compare Register 5");
	extended_registers_registry.push_back(tc5_var);
	tc5_var->setCallBack(this, TC5_HIGH, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".TC6", &tc_registers[6]));

	unisim::kernel::variable::Register<uint16_t> *tc6_var = new unisim::kernel::variable::Register<uint16_t>("TC6", this, tc_registers[6], "Timer Input Capture/Output Compare Register 6");
	extended_registers_registry.push_back(tc6_var);
	tc6_var->setCallBack(this, TC6_HIGH, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".TC7", &tc_registers[7]));

	unisim::kernel::variable::Register<uint16_t> *tc7_var = new unisim::kernel::variable::Register<uint16_t>("TC7", this, tc_registers[7], "Timer Input Capture/Output Compare Register 7");
	extended_registers_registry.push_back(tc7_var);
	tc7_var->setCallBack(this, TC7_HIGH, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PACTL", &pactl_register));

	unisim::kernel::variable::Register<uint8_t> *pactl_var = new unisim::kernel::variable::Register<uint8_t>("PACTL", this, pactl_register, "16-Bit Pulse Accumulator A Control Register");
	extended_registers_registry.push_back(pactl_var);
	pactl_var->setCallBack(this, PACTL, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PAFLG", &paflg_register));

	unisim::kernel::variable::Register<uint8_t> *paflg_var = new unisim::kernel::variable::Register<uint8_t>("PAFLG", this, paflg_register, "Pulse Accumulator A Flag Register");
	extended_registers_registry.push_back(paflg_var);
	paflg_var->setCallBack(this, PAFLG, &CallBackObject::write, NULL);

	for (unsigned int i=0; i<4; i++) {
		std::stringstream sstr;
		sstr << "PACN" << i;
		std::string shortName(sstr.str());
		
		registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + '.' + shortName, ((uint8_t*) &pacn_register[i])));

		unisim::kernel::variable::Register<uint8_t> *pacn_var = new unisim::kernel::variable::Register<uint8_t>(shortName.c_str(), this, pacn_register[i], "Pulse Accumulator Count Register");
		extended_registers_registry.push_back(pacn_var);
		pacn_var->setCallBack(this, PACN0-i, &CallBackObject::write, NULL);
	}
	
	for (unsigned int i=0; i<4; i++) {
		std::stringstream sstr;
		sstr << "PA" << i << "H";
		std::string shortName(sstr.str());

		registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + '.' + shortName, ((uint8_t*) &paxh_registers[i])));

		unisim::kernel::variable::Register<uint8_t> *paxh_var = new unisim::kernel::variable::Register<uint8_t>(shortName.c_str(), this, paxh_registers[i], "8-Bit Pulse Accumulator Holding Register");
		extended_registers_registry.push_back(paxh_var);
		paxh_var->setCallBack(this, PA0H-i, &CallBackObject::write, NULL);

	}

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".MCCTL", &mcctl_register));

	unisim::kernel::variable::Register<uint8_t> *mcctl_var = new unisim::kernel::variable::Register<uint8_t>("MCCTL", this, mcctl_register, "16-Bit Modulus Down Counter Register");
	extended_registers_registry.push_back(mcctl_var);
	mcctl_var->setCallBack(this, MCCTL, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".MCFLG", &mcflg_register));

	unisim::kernel::variable::Register<uint8_t> *mcflg_var = new unisim::kernel::variable::Register<uint8_t>("MCFLG", this, mcflg_register, "16-Bit Modulus Down Counter Flag Register");
	extended_registers_registry.push_back(mcflg_var);
	mcflg_var->setCallBack(this, MCFLG, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".ICPAR", &icpar_register));

	unisim::kernel::variable::Register<uint8_t> *icpar_var = new unisim::kernel::variable::Register<uint8_t>("ICPAR", this, icpar_register, "Input Control Pulse Accumulator Register");
	extended_registers_registry.push_back(icpar_var);
	icpar_var->setCallBack(this, ICPAR, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".DLYCT", &dlyct_register));

	unisim::kernel::variable::Register<uint8_t> *dlyct_var = new unisim::kernel::variable::Register<uint8_t>("DLYCT", this, dlyct_register, "Delay Counter Control Register");
	extended_registers_registry.push_back(dlyct_var);
	dlyct_var->setCallBack(this, DLYCT, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".ICOVW", &icovw_register));

	unisim::kernel::variable::Register<uint8_t> *icovw_var = new unisim::kernel::variable::Register<uint8_t>("ICOVW", this, icovw_register, "Input Control Overwrite Register");
	extended_registers_registry.push_back(icovw_var);
	icovw_var->setCallBack(this, ICOVW, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".ICSYS", &icsys_register));

	unisim::kernel::variable::Register<uint8_t> *icsys_var = new unisim::kernel::variable::Register<uint8_t>("ICSYS", this, icsys_register, "Input Control System Control Register");
	extended_registers_registry.push_back(icsys_var);
	icsys_var->setCallBack(this, ICSYS, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".TIMTST", &timtst_register));

	unisim::kernel::variable::Register<uint8_t> *timtst_var = new unisim::kernel::variable::Register<uint8_t>("TIMTST", this, timtst_register, "Timer Test Register");
	extended_registers_registry.push_back(timtst_var);
	timtst_var->setCallBack(this, TIMTST, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PTPSR", &ptpsr_register));

	unisim::kernel::variable::Register<uint8_t> *ptpsr_var = new unisim::kernel::variable::Register<uint8_t>("PTPSR", this, ptpsr_register, "Precision Timer Prescaler Select Register");
	extended_registers_registry.push_back(ptpsr_var);
	ptpsr_var->setCallBack(this, PTPSR, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PTMCPSR", &ptmcpsr_register));

	unisim::kernel::variable::Register<uint8_t> *ptmcpsr_var = new unisim::kernel::variable::Register<uint8_t>("PTMCPSR", this, ptmcpsr_register, "Precision Timer Modulus Counter Prescaler Select Register");
	extended_registers_registry.push_back(ptmcpsr_var);
	ptmcpsr_var->setCallBack(this, PTMCPSR, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PBCTL", &pbctl_register));

	unisim::kernel::variable::Register<uint8_t> *pbctl_var = new unisim::kernel::variable::Register<uint8_t>("PBCTL", this, pbctl_register, "16-Bit Pulse Accumulator B Control Register");
	extended_registers_registry.push_back(pbctl_var);
	pbctl_var->setCallBack(this, PBCTL, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PBFLG", &pbflg_register));

	unisim::kernel::variable::Register<uint8_t> *pbflg_var = new unisim::kernel::variable::Register<uint8_t>("PBFLG", this, pbflg_register, "16-Bit Pulse Accumulator B Flag Register");
	extended_registers_registry.push_back(pbflg_var);
	pbflg_var->setCallBack(this, PBFLG, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".MCCNT", &mccnt_register));

	unisim::kernel::variable::Register<uint16_t> *mccnt_var = new unisim::kernel::variable::Register<uint16_t>("MCCNT", this, mccnt_register, "Modulus Down-Counter Count Register");
	extended_registers_registry.push_back(mccnt_var);
	mccnt_var->setCallBack(this, MCCNT_HIGH, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".TC0H", &tcxh_registers[0]));

	unisim::kernel::variable::Register<uint16_t> *tc0h_var = new unisim::kernel::variable::Register<uint16_t>("TC0H", this, tcxh_registers[0], "Timer Input Capture Holding Register 0");
	extended_registers_registry.push_back(tc0h_var);
	tc0h_var->setCallBack(this, TC0H_HIGH, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".TC1H", &tcxh_registers[1]));

	unisim::kernel::variable::Register<uint16_t> *tc1h_var = new unisim::kernel::variable::Register<uint16_t>("TC1H", this, tcxh_registers[1], "Timer Input Capture Holding Register 1");
	extended_registers_registry.push_back(tc1h_var);
	tc1h_var->setCallBack(this, TC1H_HIGH, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".TC2H", &tcxh_registers[2]));

	unisim::kernel::variable::Register<uint16_t> *tc2h_var = new unisim::kernel::variable::Register<uint16_t>("TC2H", this, tcxh_registers[2], "Timer Input Capture Holding Register 2");
	extended_registers_registry.push_back(tc2h_var);
	tc2h_var->setCallBack(this, TC2H_HIGH, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".TC3H", &tcxh_registers[3]));

	unisim::kernel::variable::Register<uint16_t> *tc3h_var = new unisim::kernel::variable::Register<uint16_t>("TC3H", this, tcxh_registers[3], "Timer Input Capture Holding Register 3");
	extended_registers_registry.push_back(tc3h_var);
	tc3h_var->setCallBack(this, TC3H_HIGH, &CallBackObject::write, NULL);

	Reset();

	computeInternalTime();

	return (true);
}

bool ECT::Setup(ServiceExportBase *srv_export) {
	return (true);
}

bool ECT::EndSetup() {
	return (true);
}

/**
 * Gets a register interface to the register specified by name.
 *
 * @param name The name of the requested register.
 * @return A pointer to the RegisterInterface corresponding to name.
 */
Register* ECT::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

void ECT::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

void ECT::OnDisconnect() {
}

void ECT::Reset() {

	tios_register = 0x00;
	cforc_register = 0x00;
	oc7m_register = 0x00;
	oc7d_register = 0x00;
	tcnt_register = 0x0000;
	tscr1_register = 0x0000;
	ttov_register = 0x00;
	tctl12_register = 0x0000;
	tctl34_register = 0x0000;
	tie_register = 0x0000;
	tscr2_register = 0x00;
	tflg1_register = 0x00;
	tflg2_register = 0x00;
	for (unsigned int i=0; i<8; i++) { tc_registers[i] = 0x0000; }
	pactl_register = 0x00;
	paflg_register = 0x00;
	for (unsigned int i=0; i<4; i++) { pacn_register[i] = 0x00; }
	mcctl_register = 0x00;
	mcflg_register = 0x00;
	icpar_register = 0x00;
	dlyct_register = 0x00;
	icovw_register = 0x00;
	icsys_register = 0x00;
	timtst_register = 0x00;
	ptpsr_register = 0x00;
	ptmcpsr_register = 0x00;
	pbctl_register = 0x00;
	pbflg_register = 0x00;
	for (unsigned int i=0; i<4; i++) { paxh_registers[i] = 0x00; }
	mccnt_register = 0xFFFF;
	for (unsigned int i=0; i<4; i++) { tcxh_registers[i] = 0x0000; }

	prnt_write = false;
	icsys_write = false;
	main_timer_enabled = false;
	down_counter_enabled = false;
	delay_counter_enabled = false;

	for (unsigned int i=0; i<8; i++) {
		portt_pin_reg[i] = false;
	}

}

void ECT::ResetMemory() {

	Reset();

}

void ECT::computeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

	signal_generator_period =  sc_time((double) signal_generator_period_int, SC_PS);

	pacA->setGateTime(bus_cycle_time);

}

void ECT::computeTimerPrescaledClock() {

	// is TSCR1::PRNT=0 ?
	if ((tscr1_register & 0x08) == 0) {
		prescaled_clock = pow((double) 2, (tscr2_register & 0x07)) * bus_cycle_time;
	} else {
		prescaled_clock = (ptpsr_register + 1) * bus_cycle_time;
	}

}


void ECT::computeModulusCounterClock() {

	mccnt_clock = bus_cycle_time;

	// check TSCR1::PRNT Precision Timer bit
	if ((tscr1_register & 0x08) != 0) {
		mccnt_clock = bus_cycle_time * (ptmcpsr_register+1);
	} else {
		uint8_t mccnt_prescaler = mcctl_register & 0x03;
		if (mccnt_prescaler > 0) {
			mccnt_clock = bus_cycle_time * pow((double) 2, mccnt_prescaler+1);
		}
	}

}

void ECT::updateCRGClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	computeInternalTime();
}

//=====================================================================
//=             memory interface methods                              =
//=====================================================================

bool ECT::ReadMemory(physical_address_t addr, void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr <= (baseAddress + (unsigned int) TC3H_LOW))) {

		physical_address_t offset = addr-baseAddress;

		memset(buffer, 0, size);

		switch (offset) {
			case TIOS: {
				*((uint8_t *)buffer) =  tios_register;
			} break;
			case CFORC: {
				*((uint8_t *)buffer) = cforc_register;
			} break;
			case OC7M: {
				*((uint8_t *)buffer) = oc7m_register;
			} break;
			case OC7D: {
				*((uint8_t *)buffer) =  oc7d_register;
			} break;
			case TCNT_HIGH: {
				if (size == 2) {
					*((uint16_t *)buffer) = tcnt_register;
				} else {
					*((uint8_t *)buffer) = tcnt_register >> 8;
				}
			} break;
			case TCNT_LOW: {
				*((uint8_t *)buffer) = tcnt_register & 0x00FF;
			} break;
			case TSCR1: {
				*((uint8_t *)buffer) = tscr1_register;
			} break;
			case TTOV: {
				*((uint8_t *)buffer) = ttov_register;
			} break;
			case TCTL1: {
				if (size == 2) {
					*((uint16_t *)buffer) = tctl12_register;
				} else {
					*((uint8_t *)buffer) =  tctl12_register >> 8;
				}

			} break;
			case TCTL2: {
				*((uint8_t *)buffer) =  tctl12_register & 0x00FF;
			} break;
			case TCTL3: {
				if (size == 2) {
					*((uint16_t *)buffer) = tctl34_register;
				} else {
					*((uint8_t *)buffer) = tctl34_register >> 8;
				}

			} break;
			case TCTL4: {
				*((uint8_t *)buffer) = tctl34_register & 0x00FF;
			} break;
			case TIE: {
				*((uint8_t *)buffer) =  tie_register;
			} break;
			case TSCR2: {
				*((uint8_t *)buffer) =  tscr2_register;
			} break;
			case TFLG1: {
				*((uint8_t *)buffer) = tflg1_register;
			} break;
			case TFLG2: {
				*((uint8_t *)buffer) = tflg2_register;
			} break;
			case PACTL: {
				*((uint8_t *)buffer) =  pactl_register;
			} break;
			case PAFLG: {
				*((uint8_t *)buffer) = paflg_register;
			} break;
			case PACN3: {
				if (size == 2) {
					*((uint16_t *)buffer) = (((uint16_t) pacn_register[3]) << 8) | pacn_register[2];
				} else {
					*((uint8_t *)buffer) = pacn_register[3];
				}

			} break;
			case PACN2: {
				*((uint8_t *)buffer) = pacn_register[2];

			} break;
			case PACN1: {
				if (size == 2) {
					*((uint16_t *)buffer) = (((uint16_t) pacn_register[1]) << 8) | pacn_register[0];
				} else {
					*((uint8_t *)buffer) = pacn_register[1];
				}

			} break;
			case PACN0: {
				*((uint8_t *)buffer) = pacn_register[0];
			} break;
			case MCCTL: {
				*((uint8_t *)buffer) = mcctl_register;
			} break;
			case MCFLG: {
				*((uint8_t *)buffer) = mcflg_register;
			} break;
			case ICPAR: {
				*((uint8_t *)buffer) = icpar_register;
			} break;
			case DLYCT: {
				*((uint8_t *)buffer) = dlyct_register;
			} break;
			case ICOVW: {
				*((uint8_t *)buffer) = icovw_register;
			} break;
			case ICSYS: {
				*((uint8_t *)buffer) = icsys_register;
			} break;
			case RESERVED: { *((uint8_t *)buffer) = 0; } break;
			case TIMTST: { *((uint8_t *)buffer) = 0; } break;
			case PTPSR: {
				*((uint8_t *)buffer) = ptpsr_register;
			} break;
			case PTMCPSR: {
				*((uint8_t *)buffer) = ptmcpsr_register;
			} break;
			case PBCTL: {
				*((uint8_t *)buffer) = pbctl_register;
			} break;
			case PBFLG: {
				*((uint8_t *)buffer) = pbflg_register;
			} break;

			default: {

				if ((offset == MCCNT_HIGH) || (offset == MCCNT_LOW)) {

					if (offset == MCCNT_HIGH) {
						if (size == 2) {
							*((uint16_t *)buffer) = mccnt_register;
						} else {
							*((uint8_t *)buffer) = mccnt_register >> 8;
						}
					} else {
						*((uint8_t *)buffer) = mccnt_register & 0x00FF;
					}
				} else if ((offset >= TC0_HIGH) && (offset <= TC7_LOW)) {

					uint8_t tc_offset = offset - TC0_HIGH;
					uint8_t tc_offset_index = tc_offset/2;
					if ((tc_offset % 2) == 0) // TCx_High ?
					{
						if (size == 2) {
							*((uint16_t *)buffer) = tc_registers[tc_offset_index];
						} else {
							*((uint8_t *)buffer) =  tc_registers[tc_offset_index] >> 8;
						}
					} else {
						*((uint8_t *)buffer) =  tc_registers[tc_offset_index] & 0x00FF;
					}

				}
				else if ((offset >= PA3H) && (offset <= PA0H)) {
					uint8_t paxh_offset = offset - PA3H;
					*((uint8_t *)buffer) = paxh_registers[paxh_offset];
				}
				else if ((offset >= TC0H_HIGH) && (offset <= TC3H_LOW)) {
					uint8_t tcxh_offset = offset - TC0H_HIGH;
					uint8_t tcxh_offset_index = tcxh_offset/2;

					if ((tcxh_offset % 2) == 0) // TCxH_High ?
					{
						if (size == 2) {
							*((uint16_t *)buffer) = tcxh_registers[tcxh_offset_index];
						} else {
							*((uint8_t *)buffer) = tcxh_registers[tcxh_offset_index] >> 8;
						}

					} else {
						*((uint8_t *)buffer) = tcxh_registers[tcxh_offset_index] & 0x00FF;
					}
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

//bool ECT::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size) {
//
//	if ((addr >= baseAddress) && (addr <= (baseAddress+TC3H_LOW))) {
//
//		if (size == 0) {
//			return true;
//		}
//
//		physical_address_t offset = addr-baseAddress;
//
//		return write(offset, (uint8_t *) buffer, size);
//	}
//
//	return false;
//}

bool ECT::WriteMemory(physical_address_t addr, const void *value, uint32_t size) {

	if ((addr >= baseAddress) && (addr <= (baseAddress + (unsigned int) TC3H_LOW))) {

		if (size == 0) {
			return (true);
		}

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
			case TIOS: tios_register = *((uint8_t *)value); break;
			case CFORC: cforc_register = *((uint8_t *)value); break;
			case OC7M: oc7m_register = *((uint8_t *)value); break;
			case OC7D: oc7d_register = *((uint8_t *)value); break;
			case TCNT_HIGH: {

				if (size == 2) {
					tcnt_register = *((uint16_t *)value);
				} else {
					tcnt_register = (tcnt_register & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
				}

			} break;
			case TCNT_LOW: tcnt_register = (tcnt_register & 0xFF00) | *((uint8_t *)value); break;
			case TSCR1:	tscr1_register = *((uint8_t *)value); break;
			case TTOV: ttov_register = *((uint8_t *)value); break;
			case TCTL1: {
				if (size == 2) {
					tctl12_register = *((uint16_t *)value);
				} else {
					tctl12_register = (tctl12_register & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
				}

			} break;
			case TCTL2: {
				tctl12_register = (tctl12_register & 0xFF00) | *((uint8_t *)value);

			} break;
			case TCTL3: {
				if (size == 2) {
					tctl34_register = *((uint16_t *)value);
				} else {
					tctl34_register = (tctl34_register & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
				}
			} break;
			case TCTL4: {
				tctl34_register = (tctl34_register & 0xFF00) | *((uint8_t *)value);
			} break;
			case TIE: {
				tie_register = *((uint8_t *)value);
			} break;
			case TSCR2: {
				tscr2_register = *((uint8_t *)value);

			} break;
			case TFLG1: {
				tflg1_register = *((uint8_t *)value);
			} break;
			case TFLG2: {
				tflg2_register = *((uint8_t *)value);
			} break;
			case PACTL: {
				pactl_register = *((uint8_t *)value);
			} break;
			case PAFLG: {
				paflg_register = *((uint8_t *)value);
			} break;
			case PACN3: {
				if (size == 2) {
					pacn_register[3] = *((uint16_t *)value) >> 8;
					pacn_register[2] = *((uint16_t *)value) & 0x00FF;
				} else {
					pacn_register[3] = *((uint8_t *)value);
				}

			} break;
			case PACN2: {
				pacn_register[2] = *((uint8_t *)value);
			} break;
			case PACN1: {
				if (size == 2) {
					pacn_register[1] = *((uint16_t *)value) >> 8;
					pacn_register[0] = *((uint16_t *)value) & 0x00FF;
				} else {
					pacn_register[1] = *((uint8_t *)value);
				}

			} break;
			case PACN0: {
				pacn_register[0] = *((uint8_t *)value);
			} break;
			case MCCTL: {
				mcctl_register = *((uint8_t *)value);
			} break;
			case MCFLG: {
				mcflg_register = *((uint8_t *)value);
			} break;
			case ICPAR: {
				icpar_register = *((uint8_t *)value);
			} break;
			case DLYCT: {
				dlyct_register = *((uint8_t *)value);
			} break;
			case ICOVW: {
				icovw_register = *((uint8_t *)value);
			} break;
			case ICSYS: {
				icsys_register = *((uint8_t *)value);
			} break;
			case RESERVED: reserved_address = *((uint8_t *)value); break;
			case TIMTST: timtst_register = *((uint8_t *)value); break;
			case PTPSR: {
				ptpsr_register = *((uint8_t *)value);
			} break;
			case PTMCPSR: {
				ptmcpsr_register = *((uint8_t *)value);
			} break;
			case PBCTL: {
				pbctl_register = *((uint8_t *)value);
			} break;
			case PBFLG: {
				pbflg_register = *((uint8_t *)value);
			} break;

			default: {
				if ((offset == MCCNT_HIGH) || (offset == MCCNT_LOW)) {
					if (offset == MCCNT_HIGH) {
						if (size == 2) {
							mccnt_register = *((uint16_t *)value);
						} else {
							mccnt_register = (mccnt_load_register & 0x00FF) | ((uint16_t) (*((uint8_t *)value) << 8));
						}
					} else {
						mccnt_register = (mccnt_register & 0xFF00) | *((uint8_t *)value);
					}

				} else if ((offset >= TC0_HIGH) && (offset <= TC7_LOW)) {
					uint8_t tc_offset = offset - TC0_HIGH;
					uint8_t tc_offset_index = tc_offset/2;
					if ((tc_offset % 2) == 0) // TCx_High ?
					{
						if (size == 2) {
							tc_registers[tc_offset_index] = *((uint16_t *)value);
						} else {
							tc_registers[tc_offset_index] = (tc_registers[tc_offset_index] & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
						}

					} else {
						tc_registers[tc_offset_index] = (tc_registers[tc_offset_index] & 0xFF00) | *((uint8_t *)value);
					}

				}
				else if ((offset >= PA3H) && (offset <= PA0H)) {
					uint8_t paxh_offset = offset - PA3H;
					paxh_registers[paxh_offset] = *((uint8_t *)value);
				}
				else if ((offset >= TC0H_HIGH) && (offset <= TC3H_LOW)) {

					uint8_t tcxh_offset = offset - TC0H_HIGH;
					uint8_t tcxh_offset_index = tcxh_offset/2;

					if ((tcxh_offset % 2) == 0) // TCxH_High ?
					{
						if (size == 2) {
							tcxh_registers[tcxh_offset_index] = *((uint16_t *)value);
						} else {
							tcxh_registers[tcxh_offset_index] = (tcxh_registers[tcxh_offset_index] & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
						}

					} else {
						tcxh_registers[tcxh_offset_index] = (tcxh_registers[tcxh_offset_index] & 0xFF00) | *((uint8_t *)value);
					}

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


