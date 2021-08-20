/*
 * s12sci.cc
 *
 *  Created on: 8 août 2012
 *      Author: rnouacer
 */

#include <unisim/component/tlm2/processor/hcs12x/s12sci.hh>
#include <unisim/component/tlm2/processor/hcs12x/xint.hh>
#include <unisim/util/converter/convert.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {


S12SCI::S12SCI(const sc_module_name& name, Object *parent) :
	Object(name, parent)
	, sc_module(name)

	, unisim::kernel::Client<TrapReporting>(name, parent)
	, unisim::kernel::Client<CharIO>(name, parent)
	, unisim::kernel::Service<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::Client<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::Service<Registers>(name, parent)

	, trap_reporting_import("trap_reporting_import", this)
	, char_io_import("char_io_import", this)

	, memory_export("memory_export", this)
	, memory_import("memory_import", this)
	, registers_export("registers_export", this)

	, slave_socket("slave_socket")
	, bus_clock_socket("bus_clock_socket")

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

	, baseAddress(0x00C8)
	, param_baseAddress("base-address", this, baseAddress)

	, interrupt_offset(0xD6)
	, param_interrupt_offset("interrupt-offset", this, interrupt_offset)

	, tx_debug_enabled(false)
	, param_tx_debug_enabled("tx-debug-enabled", this, tx_debug_enabled)
	, rx_debug_enabled(false)
	, param_rx_debug_enabled("rx-debug-enabled", this, rx_debug_enabled)

	, scisr1_read(false)
	, idle_to_send(false)

	, txd(true)
	, txd_output_pin("TXD", this, txd, "TXD output")
	, txd_pin_enable(true)
	, param_txd_pin_enable("txd-pin-enable", this, txd_pin_enable, "param txd pin enable")
	, rxd(true)
	, rxd_input_pin("RXD", this, rxd, "RXD input")

	, scibdh_register(0x00)
	, scibdl_register(0x00)
	, scicr1_register(0x00)
	, sciasr1_register(0x00)
	, sciacr1_register(0x00)
	, sciacr2_register(0x00)
	, scicr2_register(0x00)
	, scisr1_register(0x00)
	, scisr2_register(0x00)
	, scidrh_register(0x00)
	, scidrl_register(0x00)

	, tx_shift_register(0x0000)
	, rx_shift_register(0x0000)

	, telnet_enabled(false)
	, param_telnet_enabled("telnet-enabled", this, telnet_enabled)

	, logger(*this)

{

	interrupt_request(*this);

	slave_socket.register_b_transport(this, &S12SCI::read_write);
	bus_clock_socket.register_b_transport(this, &S12SCI::updateBusClock);

	txd_output_pin.SetMutable(true);
	rxd_input_pin.SetMutable(true);

	SC_HAS_PROCESS(S12SCI);

	SC_THREAD(RunRx);

	SC_THREAD(RunTx);

	SC_THREAD(TelnetProcessInput);

	xint_payload = xint_payload_fabric.allocate();

}


S12SCI::~S12SCI() {

	xint_payload->release();

	// Release registers_registry
	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

}

void S12SCI::RunRx() {

	uint8_t idleCounter = 0;
	uint8_t breakCounter = 0;


	while (true) {

		while (!isReceiverEnabled()) {

			wait(rx_run_event);

			idleCounter = 0;
			breakCounter = 0;

		}

		uint8_t frameLength = getDataFrameLength();

		/**
		 *   - SCISR2::RAF bit is set when the receiver detects a logic 0 during the RT1 time period of the start bit search.
		 *   - SCISR2::RAF bit is cleared when the receiver detects an idle character.
		 */

		rx_shift_register = 0;
		bool lastRXD = true;

		// Don't use (!isReceiverActive()) in the following loop condition because RAF is cleared only when IDLE is detected
		while (isReceiverEnabled() && lastRXD && (idleCounter < frameLength)) {

			lastRXD = getRXD();
			if (!lastRXD) {
				setReceiverActive();
				breakCounter++;
				idleCounter = 0;
			} else {
				breakCounter = 0;
				idleCounter++;
			}

			wait(sci_baud_rate);

		}

		if (!isReceiverEnabled()) continue;

		// Is Receiver detects an IDLE character ?
		if (idleCounter == frameLength) {

			idleCounter = 0;

			// SCISR2::RAF receiver active flag is cleared when the receiver detects an IDLE character
			clearReceiverActive();

			if (isIdleLineWakeup() && isReceiverStandbay()) {
				if (rx_debug_enabled)	std::cout << "Receiver IDLE => Wakeup SCI" << std::endl;

				// Wakeup the receiver from stanby
				clearReceiverWakeup();
			} else {
				// set Idle
				if (!isIDLE()) {
					if (rx_debug_enabled)	std::cout << "Receiver IDLE " << std::endl;

					setIDLE();
				}
			}

		} else {

			uint16_t rx_shift_mask = 1;  // we don't need to keep the start bit in the RX shift register
			uint8_t index = 1;

			//-- handle breaking detection

			while (isReceiverEnabled() && (index < frameLength)) {

				lastRXD = getRXD();
				if (lastRXD) {
					rx_shift_register = rx_shift_register | rx_shift_mask;
					breakCounter = 0;
					if (!isIdleCountAfterStop()) {
						idleCounter++;
					}
				} else {
					breakCounter++;
					idleCounter = 0;
				}

				rx_shift_mask = rx_shift_mask << 1;
				index = index + 1;

				wait(sci_baud_rate);

			}

			if (!isReceiverEnabled()) continue;

			if (!lastRXD) {
				while (isReceiverEnabled() && (breakCounter < getBreakLength() && !lastRXD)) {
					lastRXD = getRXD();
					if (!lastRXD) {
						breakCounter++;
					}

					wait(sci_baud_rate);

				}

				if (breakCounter == getBreakLength()) {
					breakCounter = 0;
					if (rx_debug_enabled)	std::cout << "Receiver BREAK " << std::endl;
					if (isBreakDetectEnabled()) {
						setBreakDetectInterrupt();
						// TODO: ? May set noise flag NF, or receiver flag RAF ?
						// setNoiseFlag();
					} else {
						setFramingError();
						if (rx_debug_enabled)	std::cout << "Fraiming Error (1)" << std::endl;
						setRDRF();
						scidrh_register = 0;
						scidrl_register = 0;
						// TODO: ? May set the overrun flag OR, noise flag NF, parity error flag PE, or the receiver active flag RAF ?
						setNoiseFlag();
					}
				} else {
					// Stop bit not detected and it is not a break frame
					setFramingError();
					if (rx_debug_enabled)	std::cout << "Fraiming Error (2)" << std::endl;
					setRDRF();
					scidrh_register = 0;
					scidrl_register = 0;
				}
			}
			else {

				if (!isRDRFCleared()) {
					setOverrunFlag();
				} else {

					uint8_t lowByte = rx_shift_register & 0x00FF;
					uint8_t highByte = 0;
					 // clear stop bit
					if (isNineBitsMode()) {
						highByte = ((rx_shift_register & 0xFDFF) >> 8);
					} else {
						highByte = ((rx_shift_register & 0xFEFF) >> 8);
					}

					bool msBit = false;
					if (isNineBitsMode()) {
						msBit = highByte & 0x01; // get the 9th bit of the received data
						highByte = highByte & 0xFE; // clear parity bit
					} else {
						msBit = lowByte & 0x0080; // get the 8th bit of the received data
						lowByte = lowByte & 0x7F; // clear parity bit
					}

					if (isReceiverStandbay()) {
						if (isAddressWakeup() && msBit) {
							clearReceiverWakeup();
						}
					}

					// TODO: I have to emulate parity error
					if (isParityEnabled()) {
						if (msBit != (getParity<uint8_t>(lowByte) ^ isOddParity())) {
							setParityError();
						}
					}

					scidrl_register = lowByte;
					scidrh_register = (highByte << 7);

					if (rx_debug_enabled)	std::cout << sc_object::name() << ":: receive -> " << scidrl_register << std::endl;

					setRDRF();
				}
			}

		}

	}

}

// *** From Now ***

void S12SCI::TelnetProcessInput()
{
	while (telnet_enabled) {

		while (!isReceiverEnabled() && telnet_enabled) {

			wait(rx_run_event);
		}

		if(char_io_import)
		{
			char c;
			uint8_t v;

			if(!char_io_import->GetChar(c)) {
				wait(telnet_process_input_period);

				continue;
			} else {
				v = (uint8_t) c;
				if(rx_debug_enabled)
				{
					logger << DebugInfo << "Receiving ";
					if(v >= 32)
						logger << "character '" << c << "'";
					else
						logger << "control character 0x" << std::hex << (unsigned int) v << std::dec;
					logger << " from telnet client" << EndDebugInfo;
				}

				add(telnet_rx_fifo, v, telnet_rx_event);
			}

		} else {
			logger << DebugInfo << "Telnet not connected to " << sc_object::name() << EndDebugInfo;
		}

	}

}


inline bool S12SCI::getRXD() {

	static uint16_t telnet_charin_mask = 0x00;
	static uint16_t bufferin = 0;
	static uint16_t index = 0xFF;
	static uint16_t frameLength = getDataFrameLength();

	if (isLoopOperationEnabled()) {
		if (isTx2RxInternal()) {
			return (rxd);
		} else {
			return (txd);
		}

	} else {
		if (telnet_enabled) {

			if (index >= frameLength) {
//				TelnetProcessInput();

				if (isEmpty(telnet_rx_fifo)) {
					bufferin = buildFrame(0xFF, 0xFF, SCIIDLE);
					frameLength = getDataFrameLength();

				} else {

					uint8_t telnet_charin = 0x00;
					next(telnet_rx_fifo, telnet_charin, telnet_rx_event);
					if (telnet_charin == 0x03) {
						bufferin = buildFrame(0, 0, SCIBREAK);
						frameLength = getBreakLength();

					} else {

						if (rx_debug_enabled) std::cout << sc_object::name() << "::Telnet::getRXD  HAVE DATA" << std::endl;
						// TODO: I have to emulate address wakeup frame
						bufferin = buildFrame(0, telnet_charin, SCIDATA);
						frameLength = getDataFrameLength();
					}
				}

				telnet_charin_mask = 0x01;
				index = 0;
			}

			rxd = bufferin & telnet_charin_mask;
			telnet_charin_mask = telnet_charin_mask << 1;
			index++;
		}

		return (rxd);
	}

}

void S12SCI::RunTx() {

	while (true) {

		/**
		 *  Note:
		 *  - TXD is high impedance (logic 1) any time the transmitter is disabled
		 *  - When the transmit shift register is not transmitting a frame, the TXD pin goes to the idle condition, logic 1 (perpetual preamble).
		 *  - If at any time software clears the TE bit in SCICR2, the transmitter enable signal goes low and the transmit signal goes idle
		 */

		while (!isTransmitterEnabled()) {

			wait(tx_run_event);

			// Shifts a frame out (preamble) through the TXD pin at configured baud rate
			txShiftOut(SCIIDLE);
		}

		while (isTransmitterEnabled()) {

			if (!isTDRECleared() && !isSendBreak()) {
				wait(tx_load_event | tx_break_event);
				continue;
			}

			if (isSendBreak()) {

				/**
				 *   - Toggling SBK sends one break character (10 or 11 logic 0s, respectively 13 or 14 logics 0s if BRK13 is set).
				 *   - Toggling implies clearing the SBK bit before the break character has finished transmitting.
				 *   - As long as SBK is set, the transmitter continues to send complete break characters.
				 */

				txShiftOut(SCIBREAK);

				if (!isSendBreak()) {
					txShiftOut(SCIIDLE);
				}

			} else {

				txShiftOut(SCIDATA);

			}

			if (idle_to_send) {
				idle_to_send = false;
				txShiftOut(SCIIDLE);
			}

		}

	}

}

inline uint16_t S12SCI::buildFrame(uint8_t high, uint8_t low, SCIMSG msgType) {

	uint16_t result = 0;

	switch (msgType) {
		case SCIDATA: {
			result = low; // Load scidrl (8-low bits)
			bool isOddParity = (isParityEnabled())? getParity<uint8_t>(low) : false;

			if (isNineBitsMode()) {

				// transmit 9-bits
				if (isParityEnabled()) {

					if (isOddParity) {

						result = result | 0x0100; // set to 1 the MSB of 9-bits transmitted data
					} else {

						result = result & 0xFEFF; // set to 0 the MSB of 9-bits transmitted data
					}
				} else {

					result = result | ((((uint16_t) high) & 0x0040) << 2); // Load T8
				}

				// 3. Preface the frame with start bit and append it with a stop bit
				result = result << 1; // load start bit (one logic 0) at position 0
				result = result | 0x0400; // load stop bit (one logic 1) at position 11

			} else {

				// transmit 8-bits
				if (isParityEnabled()) {

					if (isOddParity) {

						result = result | 0x0080; // set to 1 the MSB of 8-bits transmitted data
					} else {

						result = result & 0xFF7F; // set to 0 the MSB of 8-bits transmitted data
					}
				}

				// 3. Preface the frame with start bit and append it with a stop bit
				result = result << 1; // load start bit (one logic 0) at position 0
				result = result | 0x0200; // load stop bit (one logic 1) at position 10

			}

		} break;
		case SCIIDLE: {

			if (isNineBitsMode()) {
				result = IDLE_11; // preamble of 11 logic 1s
			} else {
				result = IDLE_10; // preamble of 10 logic 1s
			}

		} break;
		case SCIBREAK: {
			result = 0;
		} break;
		default: break;
	}


	return (result);
}

//inline void S12SCI::txShiftOut(SCIMSG msgType, uint8_t length)
inline void S12SCI::txShiftOut(SCIMSG msgType)
{
	// SCISR1::TC is set high when the SCISR1::TDRE flag is set and no data, preamble, or break character is being transmitted.

	if (tx_debug_enabled)	std::cout << sc_object::name() << "::txShiftOut Start transmission -> " << std::hex << (unsigned int) tx_shift_register << std::dec << std::endl;

	clearTC();

	if (telnet_enabled) {
		switch (msgType) {
			case SCIDATA: {

				add(telnet_tx_fifo, scidrl_register, telnet_tx_event);
				TelnetProcessOutput(true);

			} break;
			case SCIIDLE: {

				TelnetSendString((const unsigned char*) "\r\nIDLE\r\n");

			} break;
			case SCIBREAK: {

				TelnetSendString((const unsigned char*) "\r\nBREAK\r\n");

			} break;
			default: break;
		}

	}
//	else
	if (txd_pin_enable)
	{

		uint8_t length = 0;
		switch (msgType) {
			case SCIDATA: {

				length = getDataFrameLength();
				tx_shift_register = buildFrame(scidrh_register, scidrl_register, SCIDATA);

			} break;
			case SCIIDLE: {

				length = getDataFrameLength();
				tx_shift_register = buildFrame(0xFF, 0xFF, SCIIDLE); // preamble of 11 logic 1s

			} break;
			case SCIBREAK: {

				length = getBreakLength();
				tx_shift_register = buildFrame(0, 0, SCIBREAK);

			} break;
			default: break;
		}

		// check baud rate generator (isBaudRateGeneratorEnabled())

		uint16_t tx_shift = tx_shift_register;
		uint8_t index = 0;

		while (isTransmitterEnabled() && (index < length) && !(isSendBreak() ^ (msgType == SCIBREAK))) {
//			txd = (tx_shift & 0x0001);
//			((Variable<bool>) txd_output_pin) = ((tx_shift & 0x0001) != 0);
//			*((VariableBase*) &txd_output_pin) = ((tx_shift & 0x0001) != 0);
			txd_output_pin = ((tx_shift & 0x0001) != 0);

			tx_shift = tx_shift >> 1;
			if (isLoopOperationEnabled() && isTx2RxInternal()) {
				rxd =  txd;
			}

			index++;
			wait(sci_baud_rate);
		}

	}

	setTDRE();

	setTC();

	if (tx_debug_enabled)	std::cout << sc_object::name() << "::txShiftOut transmission complete" << std::endl;

}

void S12SCI::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();
	unsigned int data_length = trans.get_data_length();

	if ((address >= baseAddress) && (address < (baseAddress + MEMORY_MAP_SIZE))) {

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

//=====================================================================
//=             registers setters and getters                         =
//=====================================================================

bool S12SCI::read(unsigned int offset, const void *buffer, unsigned int data_length) {

	// SCIBDH, SCIBDL, SCICR1 registers are accessible if the AMAP bit in the SCISR2 register is set to zero.
	// SCIASR1, SCIACR1, SCIACR2 registers are accessible if the AMAP bit in the SCISR2 register is set to one.

	switch (offset) {
		case (SCIBDH | SCIASR1): {
			if ((scisr2_register & 0x80) == 0) {
				if (data_length == 1) {
					*((uint8_t *) buffer) = scibdh_register;
				} else if (data_length == 2) {
					uint16_t val = (scibdh_register << 8) | scibdl_register;
					*((uint16_t *) buffer) = Host2BigEndian(val);
				}
			} else {
				*((uint8_t *) buffer) = sciasr1_register & 0x87;
			}
		} break;
		case (SCIBDL | SCIACR1): {
			if ((scisr2_register & 0x80) == 0) {
				*((uint8_t *) buffer) = scibdl_register;
			} else {
				*((uint8_t *) buffer) = sciacr1_register & 0x83;
			}
		} break;
		case (SCICR1 | SCIACR2): {
			// TODO: LIN Transmit Collision Detection is not implemented
			if ((scisr2_register & 0x80) == 0) {
				*((uint8_t *) buffer) = scicr1_register;
			} else {
				*((uint8_t *) buffer) = sciacr2_register & 0x07;
			}
		} break;

		case SCICR2: {
			*((uint8_t *) buffer) = scicr2_register;
		} break; // 1 byte
		case SCISR1: { // 1 byte
			*((uint8_t *) buffer) = scisr1_register;
			/* Write has no effect and clearing is done by read/write of SCI-Data-Register after reading SCISCR1 and data register has not changed between */
			scisr1_read = true;
		} break;
		case SCISR2: {
			*((uint8_t *) buffer) = (scisr2_register & 0x9E);
		} break; // 1 bytes

		case SCIDRH: { // 1 bytes
			if (data_length == 1) {
				*((uint8_t *) buffer) = scidrh_register;
			} else if (data_length == 2) {
				uint16_t val = (scidrh_register << 8) | scidrl_register;
				*((uint16_t *) buffer) = Host2BigEndian(val);

				// clear RDRF, IDLE, OR, NF, FE, PF flags
				if (isSCISR1_Read()) { (scisr1_register = scisr1_register & 0xC0); }
			}
		} break;
		case SCIDRL: {
			*((uint8_t *) buffer) = scidrl_register;

			// clear RDRF, IDLE, OR, NF, FE, PF flags
			if (isSCISR1_Read()) { (scisr1_register = scisr1_register & 0xC0); }
		} break; // 1 bytes

		case SCIBDH_BANK_OFFSET: {
			if (data_length == 1) {
				*((uint8_t *) buffer) = scibdh_register;
			} else if (data_length == 2) {
				uint16_t val = (scibdh_register << 8) | scibdl_register;
				*((uint16_t *) buffer) = Host2BigEndian(val);
			}
		} break;
		case SCIBDL_BANK_OFFSET: {
			*((uint8_t *) buffer) = scibdl_register;
		} break;
		case SCICR1_BANK_OFFSET: {
			*((uint8_t *) buffer) = scicr1_register;
		} break;
		case SCIASR1_BANK_OFFSET: {
			*((uint8_t *) buffer) = sciasr1_register;
		} break;
		case SCIACR1_BANK_OFFSET: {
			*((uint8_t *) buffer) = sciacr1_register;
		} break;
		case SCIACR2_BANK_OFFSET: {
			*((uint8_t *) buffer) = sciacr2_register;
		} break;

	}

	return (true);
}

bool S12SCI::write(unsigned int offset, const void *buffer, unsigned int data_length) {

	// SCIBDH, SCIBDL, SCICR1 registers are accessible if the AMAP bit in the SCISR2 register is set to zero.
	// SCIASR1, SCIACR1, SCIACR2 registers are accessible if the AMAP bit in the SCISR2 register is set to one.

	switch (offset) {
		case (SCIBDH | SCIASR1): {
			if ((scisr2_register & 0x80) == 0) {
				if (data_length == 1) {
					scibdh_register = *((uint8_t *) buffer);
				} else if (data_length == 2) {
					uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
					scibdh_register = val >> 8;
					scibdl_register = val & 0x00FF;
					ComputeBaudRate();
				}

				if (isBaudRateGeneratorEnabled()) { rx_run_event.notify(); tx_run_event.notify(); }

			} else {
				sciasr1_register = ~(*((uint8_t *) buffer) & 0x83) & sciasr1_register;
			}

		} break;

		case (SCIBDL | SCIACR1): {

			if ((scisr2_register & 0x80) == 0) {
				 scibdl_register = *((uint8_t *) buffer);
				 ComputeBaudRate();

				if (isBaudRateGeneratorEnabled()) { rx_run_event.notify(); tx_run_event.notify(); }

			} else {
				 sciacr1_register = (*((uint8_t *) buffer) & 0x83) | (sciacr1_register & 0x7C);
			}

		} break;

		case (SCICR1 | SCIACR2): {
			// TODO: LIN Transmit Collision Detection is not implemented
			if ((scisr2_register & 0x80) == 0) {
				 scicr1_register = *((uint8_t *) buffer);
			} else {
				 sciacr2_register = (*((uint8_t *) buffer) & 0x07) | (sciacr2_register & 0xF8);
			}
		} break;

		case SCICR2: {
			uint8_t old_scicr2 = scicr2_register;
			scicr2_register = *((uint8_t *) buffer);

			// check SCICR2::RE bit transition from 0 to 1
			if (((old_scicr2 & 0x04) == 0) && ((scicr2_register & 0x04) != 0)) {
				rx_run_event.notify();
			}

			// check SCICR2::TE bit transition from 0 to 1
			if (((old_scicr2 & 0x08) == 0) && ((scicr2_register & 0x08) != 0)) {
				tx_run_event.notify();
				/** clearing and then setting TE bit during a transmission queues an idle character
				 *  to be sent after the frame currently being transmitted.
				 */
				if (!isTransmissionComplete()) {
					idle_to_send = true;
				}
			}

			// check SCICR2::SBK bit transition from 0 to 1
			if (((old_scicr2 & 0x01) == 0) && ((scicr2_register & 0x01) != 0)) {
				tx_break_event.notify();
			}

		}

		break; // 1 byte

		case SCISR1: /* Write has no effect and clearing is done by read/write of SCI-Data-Register after reading SCISCR1 and data register has not changed between */ break; // 1 byte
		case SCISR2: { // 1 bytes
			scisr2_register = (*((uint8_t *) buffer) & 0x9E) | (scisr2_register & 0x61);
		}
		break;
		case SCIDRH: { // 1 bytes
			if (data_length == 1) {
				scidrh_register = *((uint8_t *) buffer);
			} else if (data_length == 2) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				scidrh_register = val >> 8;
				scidrl_register = val & 0x00FF;

				if (tx_debug_enabled)	std::cout << "TX Write SCIDRL" << std::endl;

				clearTDRE();

				if (isTransmitterEnabled()) {
					if (tx_debug_enabled)	std::cout << "TX_Load notify" << std::endl;
					tx_load_event.notify();
				}

			}

		} break;
		case SCIDRL: { // 1 bytes
			scidrl_register = *((uint8_t *) buffer);

			if (tx_debug_enabled)	std::cout << "TX Write SCIDRL @0x" << std::hex << (baseAddress+SCIDRL) << std::dec << "  " << (unsigned int) *((uint8_t *) buffer) << std::endl;

			clearTDRE();

			if (isTransmitterEnabled()) {
				if (tx_debug_enabled)	std::cout << "TX_Load notify" << std::endl;
				tx_load_event.notify();
			}

		} break;

		case SCIBDH_BANK_OFFSET: {
			if (data_length == 1) {
				scibdh_register = *((uint8_t *) buffer);
			} else if (data_length == 2) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				scibdh_register = val >> 8;
				scibdl_register = val & 0x00FF;
			}
		} break;
		case SCIBDL_BANK_OFFSET: {
			 scibdl_register = *((uint8_t *) buffer);
		} break;
		case SCICR1_BANK_OFFSET: {
			 scicr1_register = *((uint8_t *) buffer);
		} break;
		case SCIASR1_BANK_OFFSET: {
			 sciasr1_register= *((uint8_t *) buffer);
		} break;
		case SCIACR1_BANK_OFFSET: {
			 sciacr1_register = *((uint8_t *) buffer);
		} break;
		case SCIACR2_BANK_OFFSET: {
			 sciacr2_register = *((uint8_t *) buffer);
		} break;
	}

	return (true);
}


void S12SCI::assertInterrupt(uint8_t interrupt_offset) {

	/**
	 *  The Receiver Wakeup bit (RWU) enables the wakeup function and inhibits further receiver  interrupt requests.
	 *  Normally, hardware wakes the receiver by automatically clearing RWU
	 */

	if (isReceiverWakeupEnabled()) return;

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

// Master methods

void S12SCI::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

// Master methods

tlm_sync_enum S12SCI::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}


void S12SCI::ComputeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

	ComputeBaudRate();
}


void S12SCI::updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}

void S12SCI::ComputeBaudRate() {

	if (!isInfraredEnabled()) {
		// SCI baud rate = SCI bus clock / (16 x SBR[12:0])
		uint16_t sbr12_0 = ((scibdh_register & 0x1F) << 8) | scibdl_register;

		if (sbr12_0 != 0) {
			sci_baud_rate = bus_cycle_time * (16 * sbr12_0);
		} else {
			sci_baud_rate = sc_time(-1, SC_PS);
		}

	} else {
		// SCIbaud rate = SCI bus clock / (32 x SBR[12:1])
		uint16_t sbr12_1 = ((scibdh_register & 0x1F) << 7) | (scibdl_register >> 1);
		if (sbr12_1 != 0) {
			sci_baud_rate = bus_cycle_time * (32 * sbr12_1);
		} else {
			sci_baud_rate = sc_time(-1, SC_PS);
		}

	}

//	telnet_process_input_period = sc_time(1, SC_MS);
	telnet_process_input_period = sci_baud_rate * 8 * 8;

}

//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================


bool S12SCI::BeginSetup() {

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".SCIBDH", &scibdh_register));

	unisim::kernel::variable::Register<uint8_t> *scibdh_var = new unisim::kernel::variable::Register<uint8_t>("SCIBDH", this, scibdh_register, "SCI Baud Rate Registers High byte (SCIBDH)");
	extended_registers_registry.push_back(scibdh_var);
	scibdh_var->setCallBack(this, SCIBDH_BANK_OFFSET, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".SCIBDL", &scibdl_register));

	unisim::kernel::variable::Register<uint8_t> *scibdl_var = new unisim::kernel::variable::Register<uint8_t>("SCIBDL", this, scibdl_register, "SCI Baud Rate Registers Low byte (SCIBDL)");
	extended_registers_registry.push_back(scibdl_var);
	scibdl_var->setCallBack(this, SCIBDL_BANK_OFFSET, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".SCICR1", &scicr1_register));

	unisim::kernel::variable::Register<uint8_t> *scicr1_var = new unisim::kernel::variable::Register<uint8_t>("SCICR1", this, scicr1_register, "SCI Control Register 1 (SCICR1)");
	extended_registers_registry.push_back(scicr1_var);
	scicr1_var->setCallBack(this, SCICR1_BANK_OFFSET, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".SCIASR1", &sciasr1_register));

	unisim::kernel::variable::Register<uint8_t> *sciasr1_var = new unisim::kernel::variable::Register<uint8_t>("SCIASR1", this, sciasr1_register, "SCI Alternative Status Register 1 (SCIASR1)");
	extended_registers_registry.push_back(sciasr1_var);
	sciasr1_var->setCallBack(this, SCIASR1_BANK_OFFSET, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".SCIACR1", &sciacr1_register));

	unisim::kernel::variable::Register<uint8_t> *sciacr1_var = new unisim::kernel::variable::Register<uint8_t>("SCIACR1", this, sciacr1_register, "SCI Alternative Control Register (SCIACR1)");
	extended_registers_registry.push_back(sciacr1_var);
	sciacr1_var->setCallBack(this, SCIACR1_BANK_OFFSET, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".SCIACR2", &sciacr2_register));

	unisim::kernel::variable::Register<uint8_t> *sciacr2_var = new unisim::kernel::variable::Register<uint8_t>("SCIACR2", this, sciacr2_register, "SCI Alternative Control Register 2 (SCIACR2)");
	extended_registers_registry.push_back(sciacr2_var);
	sciacr2_var->setCallBack(this, SCIACR2_BANK_OFFSET, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".SCICR2", &scicr2_register));

	unisim::kernel::variable::Register<uint8_t> *scicr2_var = new unisim::kernel::variable::Register<uint8_t>("SCICR2", this, scicr2_register, "SCI Control Register 2 (SCICR2)");
	extended_registers_registry.push_back(scicr2_var);
	scicr2_var->setCallBack(this, SCICR2, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".SCISR1", &scisr1_register));

	unisim::kernel::variable::Register<uint8_t> *scisr1_var = new unisim::kernel::variable::Register<uint8_t>("SCISR1", this, scisr1_register, "SCI Status Register 1 (SCISR1)");
	extended_registers_registry.push_back(scisr1_var);
	scisr1_var->setCallBack(this, SCISR1, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".SCISR2", &scisr2_register));

	unisim::kernel::variable::Register<uint8_t> *scisr2_var = new unisim::kernel::variable::Register<uint8_t>("SCISR2", this, scisr2_register, "SCI Status Register 2 (SCISR2)");
	extended_registers_registry.push_back(scisr2_var);
	scisr2_var->setCallBack(this, SCISR2, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".SCIDRH", &scidrh_register));

	unisim::kernel::variable::Register<uint8_t> *scidrh_var = new unisim::kernel::variable::Register<uint8_t>("SCIDRH", this, scidrh_register, "SCI Data Registers High byte (SCIDRH)");
	extended_registers_registry.push_back(scidrh_var);
	scidrh_var->setCallBack(this, SCIDRH, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".SCIDRL", &scidrl_register));

	unisim::kernel::variable::Register<uint8_t> *scidrl_var = new unisim::kernel::variable::Register<uint8_t>("SCIDRL", this, scidrl_register, "SCI Data Registers Low byte (SCIDRL)");
	extended_registers_registry.push_back(scidrl_var);
	scidrl_var->setCallBack(this, SCIDRL, &CallBackObject::write, NULL);

	Reset();

	ComputeInternalTime();

	return (true);
}


bool S12SCI::Setup(ServiceExportBase *srv_export) {

	return (true);
}


bool S12SCI::EndSetup() {
	return (true);
}


Register* S12SCI::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

void S12SCI::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

void S12SCI::OnDisconnect() {
}


void S12SCI::Reset() {

	scibdh_register = 0x00;
	scibdl_register = 0x04;
	scicr1_register = 0x00;
	sciasr1_register = 0x00;
	sciacr1_register = 0x00;
	sciacr2_register = 0x00;
	scicr2_register = 0x00;
	scisr1_register = 0xC0;
	scisr2_register = 0x00;
	scidrh_register = 0x00;
	scidrl_register = 0x00;

	if(char_io_import)
	{
		char_io_import->ResetCharIO();
	}

}

void S12SCI::ResetMemory() {

	Reset();

}

//=====================================================================
//=             memory interface methods                              =
//=====================================================================


bool S12SCI::ReadMemory(physical_address_t addr, void *buffer, uint32_t size) {

	// SCIBDH, SCIBDL, SCICR1 registers are accessible if the AMAP bit in the SCISR2 register is set to zero.
	// SCIASR1, SCIACR1, SCIACR2 registers are accessible if the AMAP bit in the SCISR2 register is set to one.

	if ((addr >= baseAddress) && (addr < (baseAddress + MEMORY_MAP_SIZE))) {

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
			case (SCIBDH | SCIASR1): {
				if ((scisr2_register & 0x80) == 0) {
					if (size == 1) {
						*((uint8_t *) buffer) = scibdh_register;
					} else if (size == 2) {
						uint16_t val = (scibdh_register << 8) | scibdl_register;
						*((uint16_t *) buffer) = Host2BigEndian(val);
					}

				} else {
					*((uint8_t *) buffer) = sciasr1_register;
				}
			} break;
			case (SCIBDL | SCIACR1): {
				if ((scisr2_register & 0x80) == 0) {
					*((uint8_t *) buffer) = scibdl_register;
				} else {
					*((uint8_t *) buffer) = sciacr1_register;
				}
			} break;
			case (SCICR1 | SCIACR2): {
				if ((scisr2_register & 0x80) == 0) {
					*((uint8_t *) buffer) = scicr1_register;
				} else {
					*((uint8_t *) buffer) = sciacr2_register;
				}
			} break;

			case SCICR2: {
				*((uint8_t *) buffer) = scicr2_register;
			} break; // 1 byte
			case SCISR1: {
				*((uint8_t *) buffer) = scisr1_register;
			} break; // 1 byte
			case SCISR2: {
				*((uint8_t *) buffer) = scisr2_register;
			} break; // 1 bytes
			case SCIDRH: {
				if (size == 1) {
					*((uint8_t *) buffer) = scidrh_register & 0xD0;
				} else if (size == 2) {
					uint16_t val = ((scidrh_register & 0xD0) << 8) | scidrl_register;
					*((uint16_t *) buffer) = Host2BigEndian(val);
				}
			} break; // 1 bytes
			case SCIDRL: {
				*((uint8_t *) buffer) = scidrl_register;
			} break; // 1 bytes

		}

		return (true);

	}

	return (false);
}


bool S12SCI::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size) {

	// SCIBDH, SCIBDL, SCICR1 registers are accessible if the AMAP bit in the SCISR2 register is set to zero.
	// SCIASR1, SCIACR1, SCIACR2 registers are accessible if the AMAP bit in the SCISR2 register is set to one.

	if ((addr >= baseAddress) && (addr < (baseAddress + MEMORY_MAP_SIZE))) {

		if (size == 0) {
			return (true);
		}

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
			case (SCIBDH | SCIASR1): {
				if ((scisr2_register & 0x80) == 0) {
					if (size == 1) {
						scibdh_register = *((uint8_t *) buffer);
					} else if (size == 2) {
						uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
						scibdh_register = val >> 8;
						scibdl_register = val & 0x00FF;
					}

				} else {
					 sciasr1_register= *((uint8_t *) buffer);
				}
			} break;

			case (SCIBDL | SCIACR1): {
				if ((scisr2_register & 0x80) == 0) {
					 scibdl_register = *((uint8_t *) buffer);
				} else {
					 sciacr1_register = *((uint8_t *) buffer);
				}
			} break;

			case (SCICR1 | SCIACR2): {
				if ((scisr2_register & 0x80) == 0) {
					 scicr1_register = *((uint8_t *) buffer);
				} else {
					 sciacr2_register = *((uint8_t *) buffer);
				}
			} break;

			case SCICR2: {
				scicr2_register = *((uint8_t *) buffer);
			} break; // 1 byte

			case SCISR1: {
				scisr1_register = *((uint8_t *) buffer);
			} break; // 1 byte

			case SCISR2: {
				scisr2_register = *((uint8_t *) buffer);
			} break; // 1 bytes

			case SCIDRH: {
				if (size == 1) {
					scidrh_register = (*((uint8_t *) buffer) & 0x7F) | (scidrh_register & 0x80);
				} else if (size == 2) {
					uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
					scidrh_register = ((val >> 8) & 0x7F) | (scidrh_register & 0x80);
					scidrl_register = val & 0x00FF;
				}
			} break; // 1 bytes

			case SCIDRL: {
				scidrl_register = *((uint8_t *) buffer);
			} break; // 1 bytes
		}

		return (true);
	}

	return (false);

}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim





