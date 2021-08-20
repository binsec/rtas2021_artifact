/*
 * s12mscan.cc
 *
 *  Created on: 5 mars 2015
 *      Author: rnouacer
 */

#include <unisim/component/tlm2/processor/hcs12x/s12mscan.hh>
#include <unisim/component/tlm2/processor/hcs12x/xint.hh>
#include <unisim/util/converter/convert.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {


S12MSCAN::S12MSCAN(const sc_module_name& name, Object *parent) :
	Object(name, parent)
	, sc_module(name)

	, unisim::kernel::Client<TrapReporting>(name, parent)
	, unisim::kernel::Service<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::Client<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::Service<Registers>(name, parent)

	, trap_reporting_import("trap_reporting_import", this)

	, memory_export("memory_export", this)
	, memory_import("memory_import", this)
	, registers_export("registers_export", this)

	, slave_socket("slave_socket")
	, bus_clock_socket("bus_clock_socket")

	, can_rx_sock("can-rx-socket")
	, can_tx_sock("can-tx-socket")

	, rx_payload_queue("input_anx_payload_queue")

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

	, oscillator_clock_value(250000)
	, param_oscillator_clock_int("oscillator-clock", this, oscillator_clock_value)

	, baseAddress(0x0140)
	, param_baseAddress("base-address", this, baseAddress)

	, transmit_interrupt_offset(0xB0)
	, param_transmit_interrupt_offset("transmit-interrupt-offset", this, transmit_interrupt_offset)
	, receive_interrupt_offset(0xB2)
	, param_receive_interrupt_offset("receive-interrupt-offset", this, receive_interrupt_offset)
	, errors_interrupt_offset(0xB4)
	, param_errors_interrupt_offset("errors-interrupt-offset", this, errors_interrupt_offset)
	, wakeup_interrupt_offset(0xB6)
	, param_wakeup_interrupt_offset("wakeup-interrupt-offset", this, wakeup_interrupt_offset)

	, tx_debug_enabled(false)
	, param_tx_debug_enabled("tx-debug-enabled", this, tx_debug_enabled)
	, rx_debug_enabled(false)
	, param_rx_debug_enabled("rx-debug-enabled", this, rx_debug_enabled)

	, abortTransmission(false)

	, canctl0_register(0x11)
	, canctl1_register(0x11)
	, canbtr0_register(0x00)
	, canbtr1_register(0x00)
	, canrflg_register(0x00)
	, canrier_register(0x00)
	, cantflg_register(0x07)
	, cantier_register(0x00)
	, cantarq_register(0x00)
	, cantaak_register(0x00)
	, cantbsel_register(0x00)
	, canidac_register(0x00)
	, reserved1(0x00)
	, canmisc_register(0x00)
	, canrxerr_register(0x00)
	, cantxerr_register(0x00)

	, time_stamp(0)

	, hasArbitration(true)
	, txStatus(false)

	, logger(*this)

{

	param_oscillator_clock_int.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

	can_rx_sock(*this);
	can_tx_sock(*this);

	interrupt_request(*this);

	slave_socket.register_b_transport(this, &S12MSCAN::read_write);
	bus_clock_socket.register_b_transport(this, &S12MSCAN::updateBusClock);

	SC_HAS_PROCESS(S12MSCAN);

	SC_THREAD(RunRx);

	SC_THREAD(RunTx);

	xint_payload = xint_payload_fabric.allocate();

	for (int i=0; i<8; i++) {
		canidar_register[i] = 0x00;
		canidmr_register[i] = 0x00;
	}

	for (int i=0; i<5; i++) {
		for (int j=0; j<16; j++) {
			canrxfg_register[i][j] = 0x00;
		}
	}

	for (int i=0; i<3; i++) {
		for (int j=0; j<16; j++) {
			cantxfg_register[i][j] = 0x00;
		}
	}

}


S12MSCAN::~S12MSCAN() {

	xint_payload->release();

	// Release registers_registry
	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

}


// Master methods
void S12MSCAN::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

unsigned int S12MSCAN::transport_dbg(CAN_Payload& payload)
{
	// Leave this empty as it is designed for memory mapped buses
	return (0);
}

void S12MSCAN::b_transport(CAN_Payload& payload, sc_core::sc_time& t) {
	if (isCANEnabled()) {
		payload.acquire();
		cout << sc_time_stamp() << "  " << sc_object::name() << "::b_transport  " << payload << endl;
		rx_payload_queue.notify(payload, t);
	}
}

bool S12MSCAN::get_direct_mem_ptr(CAN_Payload& payload, tlm_dmi&  dmi_data) {
	return (false);
}

/**
 * Name : nb_transport_fw()
 *
 * Role :
 * 1/ Sample and Hold machine accepts analog signals from external world and stores them as capacitor charge on a storage node.
 */
tlm_sync_enum S12MSCAN::nb_transport_fw(CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case BEGIN_REQ:
			// accepts analog signals modeled by payload
			phase = END_REQ; // update the phase
			if (isCANEnabled()) {
				payload.acquire();
				if (rx_debug_enabled) {
					cout << sc_time_stamp() << "  " << sc_object::name() << "::nb_transport_fw  " << payload << endl;
				}
				rx_payload_queue.notify(payload, t); // queue the payload and the associative time
			}

			return (TLM_UPDATED);
		case END_REQ:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase END_REQ" << endl;
			Object::Stop(-1);
			break;
		case BEGIN_RESP:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase BEGIN_RESP" << endl;
			Object::Stop(-1);
			break;
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

tlm_sync_enum S12MSCAN::nb_transport_bw(CAN_Payload& can_rx_payload, tlm_phase& phase, sc_core::sc_time& t)
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
			//can_rx_payload.release();
			can_bw_event.notify();
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

void S12MSCAN::RunTimer() {

	while (true) {
		while (!isCANEnabled() || !isTimerActivated()) {

			wait(can_enable_event | timer_enable_event);
			time_stamp = 0;
		}

		wait(bit_time);
		time_stamp = (time_stamp == 0xFFFF)? 0: (time_stamp + 1);
	}
}

void S12MSCAN::RunRx() {

	while (true) {

		while (!isCANEnabled()) {
			wait(can_enable_event);

			if (rx_debug_enabled) {
				std::cout << sc_object::name() << ":: RX enabled " << std::endl;
			}
		}

		wait(rx_payload_queue.get_event());

		setReceiverActive();

		// buffer 4xIDByte + 8xDataByte + 1xLengthByte+ 1xPriorityByte + 2xTimeStampByte = 16xByte
		uint8_t rx_shift[CAN_MSG_SIZE];

		InputRX(&rx_shift);

		setRxBG(rx_shift);

		setIdle();

	}

}

void S12MSCAN::InputRX(uint8_t (*rx_shift)[CAN_MSG_SIZE])
{
	CAN_Payload *payload = NULL;

	payload = rx_payload_queue.get_next_transaction();

	if (rx_debug_enabled && payload) {
		cout << sc_object::name() << ":: Last Receive " << *payload << " - " << sc_time_stamp() << endl;
	}

	if (payload) {
		for (unsigned int i=0; i<CAN_MSG_SIZE; i++) {
			(*rx_shift)[i] = payload->msgVect[i];
		}
		payload->release();
		tlm_phase phase = BEGIN_RESP;
		sc_time local_time = SC_ZERO_TIME;
		can_rx_sock->nb_transport_bw( *payload, phase, local_time);

		if (rx_debug_enabled) {
			std::cout << sc_object::name() << "::InputRx " << *payload << "  at " << sc_time_stamp() << std::endl;
		}
	}

}

void S12MSCAN::RunTx() {

	while (true) {

		while (!isCANEnabled()) {

			wait(can_enable_event);

		}

		while (isCANEnabled()) {
			uint8_t tx_shift[CAN_MSG_SIZE];

			int txIndex = selectLoadedTx(tx_shift);

			if (txIndex == -1) {

				wait(tx_load_event);

				continue;
			}

			if (isLoopBack()) {
				CAN_Payload *payload = payload_fabric.allocate();
				payload->setMsgVect(tx_shift);
				sc_time local_time = SC_ZERO_TIME;
				rx_payload_queue.notify(*payload, local_time);
//				setRxBG(tx_shift);

			} else {
				// TODO: *** Get Arbitration (hasArbitration = true) ***
				addTimeStamp(tx_shift);


				if (!isArbitrationStatus()) {
					// TODO: handle arbitration lost
				} else {
					refreshOutput(tx_shift);
				}

			}

		}

	}

}

void S12MSCAN::refreshOutput(uint8_t tx_buffer_register[CAN_MSG_SIZE])
{
	tlm_phase phase = BEGIN_REQ;

	CAN_Payload* can_tx_payload = payload_fabric.allocate();

	for (int i=0; i<CAN_MSG_SIZE; i++) {
		can_tx_payload->msgVect[i] = tx_buffer_register[i];
	}

	sc_time local_time = SC_ZERO_TIME;

	if (tx_debug_enabled) {
		cout << sc_object::name() << ":: send " << *can_tx_payload << " - " << sc_time_stamp() << endl;
	}

	tlm_sync_enum ret = can_tx_sock->nb_transport_fw(*can_tx_payload, phase, local_time);

	can_tx_payload->release();

	wait(can_bw_event);

	switch(ret)
	{
		case TLM_ACCEPTED:
			// neither payload, nor phase and local_time have been modified by the callee
			break;
		case TLM_UPDATED:
			// the callee may have modified 'payload', 'phase' and 'local_time'
			break;
		case TLM_COMPLETED:
			// the callee may have modified 'payload', and 'local_time' ('phase' can be ignored)
			break;
	}

}

void S12MSCAN::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
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

bool S12MSCAN::read(unsigned int offset, const void *buffer, unsigned int data_length) {

	switch (offset) {
		case CANCTL0: {
			*((uint8_t *) buffer) = canctl0_register;
		} break;
		case CANCTL1: {
			*((uint8_t *) buffer) = canctl1_register;
		} break;
		case CANBTR0: {
			*((uint8_t *) buffer) = canbtr0_register;
		} break;
		case CANBTR1: {
			*((uint8_t *) buffer) = canbtr1_register;
		} break;
		case CANRFLG: {
			*((uint8_t *) buffer) = canrflg_register;
		} break;
		case CANRIER: {
			*((uint8_t *) buffer) = canrier_register;
		} break;
		case CANTFLG: {
			*((uint8_t *) buffer) = cantflg_register & 0x07;
		} break;
		case CANTIER: {
			*((uint8_t *) buffer) = cantier_register & 0x07;
		} break;
		case CANTARQ: {
			*((uint8_t *) buffer) = cantarq_register & 0x07;
		} break;
		case CANTAAK: {
			*((uint8_t *) buffer) = cantaak_register & 0x07;
		} break;
		case CANTBSEL: {
			*((uint8_t *) buffer) = 0;
			// Read: Find the lowest ordered bit set to 1, all other bits will be read as 0
			uint8_t mask = 1;
			for (int i=0; i<3; i++) {
				if ((cantbsel_register & mask) != 0) {
					*((uint8_t *) buffer) = cantbsel_register & mask;
					break;
				} else {
					mask = mask << 1;
				}
			}

		} break;
		case CANIDAC: {
			*((uint8_t *) buffer) = ((canidac_register & 0xF8) | can_hit_indicator[canrxfg_index.getHead()]) & 0x37;
		} break;
		case RESERVED1: {
			*((uint8_t *) buffer) = 0x00;
		} break;
		case CANMISC: {
			*((uint8_t *) buffer) = canmisc_register & 0x01;
		} break;
		case CANRXERR: {
			*((uint8_t *) buffer) = canrxerr_register;
		} break;
		case CANTXERR: {
			*((uint8_t *) buffer) = cantxerr_register;
		} break;
		case CANIDAR0: {
			*((uint8_t *) buffer) = canidar_register[0];
		} break;
		case CANIDAR1: {
			*((uint8_t *) buffer) = canidar_register[1];
		} break;
		case CANIDAR2: {
			*((uint8_t *) buffer) = canidar_register[2];
		} break;
		case CANIDAR3: {
			*((uint8_t *) buffer) = canidar_register[3];
		} break;
		case CANIDMR0: {
			*((uint8_t *) buffer) = canidmr_register[0];
		} break;
		case CANIDMR1: {
			*((uint8_t *) buffer) = canidmr_register[1];
		} break;
		case CANIDMR2: {
			*((uint8_t *) buffer) = canidmr_register[2];
		} break;
		case CANIDMR3: {
			*((uint8_t *) buffer) = canidmr_register[3];
		} break;
		case CANIDAR4: {
			*((uint8_t *) buffer) = canidar_register[4];
		} break;
		case CANIDAR5: {
			*((uint8_t *) buffer) = canidar_register[5];
		} break;
		case CANIDAR6: {
			*((uint8_t *) buffer) = canidar_register[6];
		} break;
		case CANIDAR7: {
			*((uint8_t *) buffer) = canidar_register[7];
		} break;
		case CANIDMR4: {
			*((uint8_t *) buffer) = canidmr_register[4];
		} break;
		case CANIDMR5: {
			*((uint8_t *) buffer) = canidmr_register[5];
		} break;
		case CANIDMR6: {
			*((uint8_t *) buffer) = canidmr_register[6];
		} break;
		case CANIDMR7: {
			*((uint8_t *) buffer) = canidmr_register[7];
		} break;

		default: {
			if ((offset >= CANRXFG_START) && (offset <= CANRXFG_END)) {
				if (rx_debug_enabled) {
					std::cout << sc_time_stamp() << "  " << sc_object::name() << "::Read CANRXFG"  << std::endl;
				}
				if (canrxfg_index.isEmpty()) {
					*((uint8_t *) buffer) = 0x00;
				} else {
					*((uint8_t *) buffer) = canrxfg_register[canrxfg_index.getHead()][offset - CANRXFG_START];
				}
			}
			else if ((offset >= CANTXFG_START) && (offset <= CANTXFG_END)) {
				if (isTxSelected()) {
					*((uint8_t *) buffer) = cantxfg_register[getTxIndex()][offset - CANTXFG_START];
				} else {
//					std::cerr << sc_object::name() << "::Warning: Try to read to CANTXFG but no TXi is selected !" << std::endl;
					*((uint8_t *) buffer) = 0x00;
				}

			} else {
				return false;
			}
		} break;
	}


	return (true);
}

bool S12MSCAN::write(unsigned int offset, const void *buffer, unsigned int data_length) {

	switch (offset) {
		case CANCTL0: {
			uint8_t value = *((uint8_t *) buffer);

			bool isSleeping = isSleepMode();

			if (isInitMode()) {
				value = (value & 0x07);
				canctl0_register = value;

				if (!isInitModeRequest()) { exitInitMode(); }

			} else {
				value = ((value & 0x80) == 0x80)? (value & 0x7F) : (value | (canctl0_register & 0x80));
				value = (isWakeupInterrupt())? (value & 0xFD): value;
				value = (value & 0xAF) | (canctl0_register & 0x50);

				canctl0_register = value;

				if (isInitModeRequest()) { enterInitMode(); }

				enableTimer();

			}

			if (isIdle() && isSleepModeRequest() && !isSleeping) { enterSleepMode(); }
			if (!isSleepModeRequest() && isSleeping) { exitSleepMode(); }

		} break;
		case CANCTL1: {
			if (!isInitMode()) break;

			uint8_t old_ctl1 = canctl1_register;

			uint8_t value = (*((uint8_t *) buffer) & 0xFC) | (canctl1_register & 0x03);
			canctl1_register = value;

			ComputeInternalTime();

			if (((old_ctl1 & 0x80) != 0x80) && ((canctl1_register & 0x80) == 0x80)) { enable_can(); }

		} break;
		case CANBTR0: {
			if (!isInitMode()) break;

			canbtr0_register = *((uint8_t *) buffer);

			ComputeInternalTime();
		} break;
		case CANBTR1: {
			if (!isInitMode()) break;

			canbtr1_register = *((uint8_t *) buffer);

			if (isThreeSamplePerBit() && (getTimeSegment1() < 2))
			{
				std::cerr << "Warning::" << sc_object::name() << ":: PHASE_SEG1 must be at least 2 time quanta (Tq) when Sampling is set to Three samples per bit" << std::endl;
			}

			ComputeInternalTime();
		} break;
		case CANRFLG: {
			if (isInitMode()) break;

			uint8_t value =  *((uint8_t *) buffer);

			uint8_t oldrflg = canrflg_register;
			canrflg_register = (canrflg_register & 0x3C) | (canrflg_register & ~(value | 0x3C));

			if (((oldrflg & 0x01) == 0x01)  && ((canrflg_register & 0x01) != 0x01)) {
				canrxfg_index.incHead();

				if (!canrxfg_index.isEmpty()) {
					setReceiverBufferFull();
				}
			}

		} break;
		case CANRIER: {
			if (isInitMode()) break;

			canrier_register = *((uint8_t *) buffer);

		} break;
		case CANTFLG: {
			if (isInitMode() || isListen()) break;

			uint8_t value =  *((uint8_t *) buffer) & 0x07;
			/*
			 * Clearing a TXEx flag also clears the corresponding ABTAKx (see Section 10.3.2.10, “MSCAN Transmitter
			 * Message Abort Acknowledge Register (CANTAAK)”).
			 * When listen-mode is active (see Section 10.3.2.2, “MSCAN Control Register 1 (CANCTL1)”) the TXEx flags
			 * cannot be cleared and no transmission is started.
			 */

			uint8_t oldtflg = cantflg_register;
			cantflg_register = cantflg_register & ~value;
			cantaak_register = cantaak_register & ~value;

			uint8_t mask = 1;
			for (int i=0; i<3; i++) {
				if (((oldtflg & mask) != 0) && ((cantflg_register & mask) == 0)) {
					tx_load_event.notify();
					break;
				} else {
					mask = mask << 1;
				}
			}

		} break;
		case CANTIER: {
			if (isInitMode()) break;

			cantier_register = *((uint8_t *) buffer) & 0x07;
		} break;
		case CANTARQ: {
			if (isInitMode()) break;

			// The CPU cannot reset CANTARQ::ABTRQx, it is reset automatically whenever the associated TXE flag is set.
			cantarq_register =  (cantarq_register & 0x07) | (*((uint8_t *) buffer) & 0x07);
		} break;
		case CANTAAK: {
			// NOP
		} break;
		case CANTBSEL: {
			if (isInitMode()) break;

			cantbsel_register = *((uint8_t *) buffer) & 0x07;

		} break;
		case CANIDAC: {
			if (!isInitMode()) break;

			canidac_register = (canidac_register & 0x07) | (*((uint8_t *) buffer) & 0x30);
		} break;
		case RESERVED1: {
			// NOP
		} break;
		case CANMISC: {
			uint8_t value = *((uint8_t *) buffer) & 0x01;

			canmisc_register = canmisc_register & ~value;
		} break;
		case CANRXERR: {
			// NOP
		} break;
		case CANTXERR: {
			// NOP
		} break;
		case CANIDAR0: {
			if (!isInitMode()) break;

			canidar_register[0] = *((uint8_t *) buffer);
		} break;
		case CANIDAR1: {
			if (!isInitMode()) break;

			canidar_register[1] = *((uint8_t *) buffer);
		} break;
		case CANIDAR2: {
			if (!isInitMode()) break;

			canidar_register[2] = *((uint8_t *) buffer);
		} break;
		case CANIDAR3: {
			if (!isInitMode()) break;

			canidar_register[3] = *((uint8_t *) buffer);
		} break;
		case CANIDMR0: {
			if (!isInitMode()) break;

			canidmr_register[0] = *((uint8_t *) buffer);
		} break;
		case CANIDMR1: {
			if (!isInitMode()) break;

			canidmr_register[1] = *((uint8_t *) buffer);
		} break;
		case CANIDMR2: {
			if (!isInitMode()) break;

			canidmr_register[2] = *((uint8_t *) buffer);
		} break;
		case CANIDMR3: {
			if (!isInitMode()) break;

			canidmr_register[3] = *((uint8_t *) buffer);
		} break;
		case CANIDAR4: {
			if (!isInitMode()) break;

			canidar_register[4] = *((uint8_t *) buffer);
		} break;
		case CANIDAR5: {
			if (!isInitMode()) break;

			canidar_register[5] = *((uint8_t *) buffer);
		} break;
		case CANIDAR6: {
			if (!isInitMode()) break;

			canidar_register[6] = *((uint8_t *) buffer);
		} break;
		case CANIDAR7: {
			if (!isInitMode()) break;

			canidar_register[7] = *((uint8_t *) buffer);
		} break;
		case CANIDMR4: {
			if (!isInitMode()) break;

			canidmr_register[4] = *((uint8_t *) buffer);
		} break;
		case CANIDMR5: {
			if (!isInitMode()) break;

			canidmr_register[5] = *((uint8_t *) buffer);
		} break;
		case CANIDMR6: {
			if (!isInitMode()) break;

			canidmr_register[6] = *((uint8_t *) buffer);
		} break;
		case CANIDMR7: {
			if (!isInitMode()) break;

			canidmr_register[7] = *((uint8_t *) buffer);
		} break;

		default: {
			if ((offset >= CANRXFG_START) && (offset <= CANRXFG_END)) {
				// NOP:
				std::cerr << sc_object::name() << "::Warning: Try to write to CANRXFG but write is unimplemented for this register !" << std::endl;
			}
			else if ((offset >= CANTXFG_START) && (offset <= CANTXFG_END)) {
				if (isTxSelected()) {
					cantxfg_register[getTxIndex()][offset - CANTXFG_START] = *((uint8_t *) buffer);
				} else {
					std::cerr << sc_object::name() << "::Warning: Try to write to CANTXFG but no TXi is selected !" << std::endl;
				}

			} else {
				return false;
			}
		} break;
	}

	return (true);
}

void S12MSCAN::assertInterrupt(uint8_t interrupt_offset) {

	if (tx_debug_enabled || rx_debug_enabled) {
		std::cout << sc_time_stamp() << "  " << sc_object::name() << "::AssertInterrupt  0x" << std::hex << (unsigned int) interrupt_offset << std::dec << std::endl;
	}

	tlm_phase phase = BEGIN_REQ;

	xint_payload->acquire();

	xint_payload->setInterruptOffset(interrupt_offset);

	sc_time local_time = SC_ZERO_TIME;

	tlm_sync_enum ret = interrupt_request->nb_transport_fw(*xint_payload, phase, local_time);

	xint_payload->release();

	switch(ret)
	{
		case TLM_ACCEPTED:
			// neither payload, nor phase and local_time have been modified by the callee
			break;
		case TLM_UPDATED:
			// the callee may have modified 'payload', 'phase' and 'local_time'
			break;
		case TLM_COMPLETED:
			// the callee may have modified 'payload', and 'local_time' ('phase' can be ignored)
			break;
	}

}

// Master methods

tlm_sync_enum S12MSCAN::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}


void S12MSCAN::ComputeInternalTime() {

	oscillator_clock = sc_time((double) oscillator_clock_value, SC_PS);
	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);
	sc_time can_clk;
	if (isBusClk()) {
		can_clk = bus_cycle_time;
	} else {
		can_clk = oscillator_clock;
	}

	bit_time = can_clk * getPrescaler() * (1 + getTimeSegment1() + getTimeSegment2());

}


void S12MSCAN::updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}

//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================


bool S12MSCAN::BeginSetup() {

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANCTL0", &canctl0_register));

	unisim::kernel::variable::Register<uint8_t> *canctl0_var = new unisim::kernel::variable::Register<uint8_t>("CANCTL0", this, canctl0_register, "CAN Control Register 0");
	extended_registers_registry.push_back(canctl0_var);
	canctl0_var->setCallBack(this, CANCTL0, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANCTL1", &canctl1_register));

	unisim::kernel::variable::Register<uint8_t> *canctl1_var = new unisim::kernel::variable::Register<uint8_t>("CANCTL1", this, canctl1_register, "CAN Control Register 1");
	extended_registers_registry.push_back(canctl1_var);
	canctl1_var->setCallBack(this, CANCTL1, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANBTR0", &canbtr0_register));

	unisim::kernel::variable::Register<uint8_t> *canbtr0_var = new unisim::kernel::variable::Register<uint8_t>("CANBTR0", this, canbtr0_register, "CAN  0");
	extended_registers_registry.push_back(canbtr0_var);
	canbtr0_var->setCallBack(this, CANBTR0, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANBTR1", &canbtr1_register));

	unisim::kernel::variable::Register<uint8_t> *canbtr1_var = new unisim::kernel::variable::Register<uint8_t>("CANBTR1", this, canbtr1_register, "CAN  1");
	extended_registers_registry.push_back(canbtr1_var);
	canbtr1_var->setCallBack(this, CANBTR1, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANRFLG", &canrflg_register));

	unisim::kernel::variable::Register<uint8_t> *canrflg_var = new unisim::kernel::variable::Register<uint8_t>("CANRFLG", this, canrflg_register, "CAN  ");
	extended_registers_registry.push_back(canrflg_var);
	canrflg_var->setCallBack(this, CANRFLG, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANRIER", &canrier_register));

	unisim::kernel::variable::Register<uint8_t> *canrier_var = new unisim::kernel::variable::Register<uint8_t>("CANRIER", this, canrier_register, "CAN  ");
	extended_registers_registry.push_back(canrier_var);
	canrier_var->setCallBack(this, CANRIER, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANTFLG", &cantflg_register));

	unisim::kernel::variable::Register<uint8_t> *cantflg_var = new unisim::kernel::variable::Register<uint8_t>("CANTFLG", this, cantflg_register, "CAN  ");
	extended_registers_registry.push_back(cantflg_var);
	cantflg_var->setCallBack(this, CANTFLG, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANTIER", &cantier_register));

	unisim::kernel::variable::Register<uint8_t> *cantier_var = new unisim::kernel::variable::Register<uint8_t>("CANTIER", this, cantier_register, "CAN  ");
	extended_registers_registry.push_back(cantier_var);
	cantier_var->setCallBack(this, CANTIER, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANTARQ", &cantarq_register));

	unisim::kernel::variable::Register<uint8_t> *cantarq_var = new unisim::kernel::variable::Register<uint8_t>("CANTARQ", this, cantarq_register, "CAN  ");
	extended_registers_registry.push_back(cantarq_var);
	cantarq_var->setCallBack(this, CANTARQ, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANTAAK", &cantaak_register));

	unisim::kernel::variable::Register<uint8_t> *cantaak_var = new unisim::kernel::variable::Register<uint8_t>("CANTAAK", this, cantaak_register, "CAN  ");
	extended_registers_registry.push_back(cantaak_var);
	cantaak_var->setCallBack(this, CANTAAK, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANTBSEL", &cantbsel_register));

	unisim::kernel::variable::Register<uint8_t> *cantbsel_var = new unisim::kernel::variable::Register<uint8_t>("CANTBSEL", this, cantbsel_register, "CAN  ");
	extended_registers_registry.push_back(cantbsel_var);
	cantbsel_var->setCallBack(this, CANTBSEL, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDAC", &canidac_register));

	unisim::kernel::variable::Register<uint8_t> *canidac_var = new unisim::kernel::variable::Register<uint8_t>("CANIDAC", this, canidac_register, "CAN  ");
	extended_registers_registry.push_back(canidac_var);
	canidac_var->setCallBack(this, CANIDAC, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANMISC", &canmisc_register));

	unisim::kernel::variable::Register<uint8_t> *canmisc_var = new unisim::kernel::variable::Register<uint8_t>("CANMISC", this, canmisc_register, "CAN  ");
	extended_registers_registry.push_back(canmisc_var);
	canmisc_var->setCallBack(this, CANMISC, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANRXERR", &canrxerr_register));

	unisim::kernel::variable::Register<uint8_t> *canrxerr_var = new unisim::kernel::variable::Register<uint8_t>("CANRXERR", this, canrxerr_register, "CAN  ");
	extended_registers_registry.push_back(canrxerr_var);
	canrxerr_var->setCallBack(this, CANRXERR, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANTXERR", &cantxerr_register));

	unisim::kernel::variable::Register<uint8_t> *cantxerr_var = new unisim::kernel::variable::Register<uint8_t>("CANTXERR", this, cantxerr_register, "CAN  ");
	extended_registers_registry.push_back(cantxerr_var);
	cantxerr_var->setCallBack(this, CANTXERR, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDAR0", &(canidar_register[0])));

	unisim::kernel::variable::Register<uint8_t> *canidar0_var = new unisim::kernel::variable::Register<uint8_t>("CANIDAR0", this, canidar_register[0], "CAN  ");
	extended_registers_registry.push_back(canidar0_var);
	canidar0_var->setCallBack(this, CANIDAR0, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDAR1", &(canidar_register[1])));

	unisim::kernel::variable::Register<uint8_t> *canidar1_var = new unisim::kernel::variable::Register<uint8_t>("CANIDAR1", this, canidar_register[1], "CAN  ");
	extended_registers_registry.push_back(canidar1_var);
	canidar1_var->setCallBack(this, CANIDAR1, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDAR2", &(canidar_register[2])));

	unisim::kernel::variable::Register<uint8_t> *canidar2_var = new unisim::kernel::variable::Register<uint8_t>("CANIDAR2", this, canidar_register[2], "CAN  ");
	extended_registers_registry.push_back(canidar2_var);
	canidar2_var->setCallBack(this, CANIDAR2, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDAR3", &(canidar_register[3])));

	unisim::kernel::variable::Register<uint8_t> *canidar3_var = new unisim::kernel::variable::Register<uint8_t>("CANIDAR3", this, canidar_register[3], "CAN  ");
	extended_registers_registry.push_back(canidar3_var);
	canidar3_var->setCallBack(this, CANIDAR3, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDMR0", &(canidmr_register[0])));

	unisim::kernel::variable::Register<uint8_t> *canidmr0_var = new unisim::kernel::variable::Register<uint8_t>("CANIDMR0", this, canidmr_register[0], "CAN  ");
	extended_registers_registry.push_back(canidmr0_var);
	canidmr0_var->setCallBack(this, CANIDMR0, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDMR1", &(canidmr_register[1])));

	unisim::kernel::variable::Register<uint8_t> *canidmr1_var = new unisim::kernel::variable::Register<uint8_t>("CANIDMR1", this, canidmr_register[1], "CAN  ");
	extended_registers_registry.push_back(canidmr1_var);
	canidmr1_var->setCallBack(this, CANIDMR1, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDMR2", &(canidmr_register[2])));

	unisim::kernel::variable::Register<uint8_t> *canidmr2_var = new unisim::kernel::variable::Register<uint8_t>("CANIDMR2", this, canidmr_register[2], "CAN  ");
	extended_registers_registry.push_back(canidmr2_var);
	canidmr2_var->setCallBack(this, CANIDMR2, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDMR3", &(canidmr_register[3])));

	unisim::kernel::variable::Register<uint8_t> *canidmr3_var = new unisim::kernel::variable::Register<uint8_t>("CANIDMR3", this, canidmr_register[3], "CAN  ");
	extended_registers_registry.push_back(canidmr3_var);
	canidmr3_var->setCallBack(this, CANIDMR3, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDAR4", &(canidar_register[4])));

	unisim::kernel::variable::Register<uint8_t> *canidar4_var = new unisim::kernel::variable::Register<uint8_t>("CANIDAR4", this, canidar_register[4], "CAN  ");
	extended_registers_registry.push_back(canidar4_var);
	canidar4_var->setCallBack(this, CANIDAR4, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDAR5", &(canidar_register[5])));

	unisim::kernel::variable::Register<uint8_t> *canidar5_var = new unisim::kernel::variable::Register<uint8_t>("CANIDAR5", this, canidar_register[5], "CAN  ");
	extended_registers_registry.push_back(canidar5_var);
	canidar5_var->setCallBack(this, CANIDAR5, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDAR6", &(canidar_register[6])));

	unisim::kernel::variable::Register<uint8_t> *canidar6_var = new unisim::kernel::variable::Register<uint8_t>("CANIDAR6", this, canidar_register[6], "CAN  ");
	extended_registers_registry.push_back(canidar6_var);
	canidar6_var->setCallBack(this, CANIDAR6, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDAR7", &(canidar_register[7])));

	unisim::kernel::variable::Register<uint8_t> *canidar7_var = new unisim::kernel::variable::Register<uint8_t>("CANIDAR7", this, canidar_register[7], "CAN  ");
	extended_registers_registry.push_back(canidar7_var);
	canidar7_var->setCallBack(this, CANIDAR7, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDMR4", &(canidmr_register[4])));

	unisim::kernel::variable::Register<uint8_t> *canidmr4_var = new unisim::kernel::variable::Register<uint8_t>("CANIDMR4", this, canidmr_register[4], "CAN  ");
	extended_registers_registry.push_back(canidmr4_var);
	canidmr4_var->setCallBack(this, CANIDMR4, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDMR5", &(canidmr_register[5])));

	unisim::kernel::variable::Register<uint8_t> *canidmr5_var = new unisim::kernel::variable::Register<uint8_t>("CANIDMR5", this, canidmr_register[5], "CAN  ");
	extended_registers_registry.push_back(canidmr5_var);
	canidmr5_var->setCallBack(this, CANIDMR5, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDMR6", &(canidmr_register[6])));

	unisim::kernel::variable::Register<uint8_t> *canidmr6_var = new unisim::kernel::variable::Register<uint8_t>("CANIDMR6", this, canidmr_register[6], "CAN  ");
	extended_registers_registry.push_back(canidmr6_var);
	canidmr6_var->setCallBack(this, CANIDMR6, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CANIDMR7", &(canidmr_register[7])));

	unisim::kernel::variable::Register<uint8_t> *canidmr7_var = new unisim::kernel::variable::Register<uint8_t>("CANIDMR7", this, canidmr_register[7], "CAN  ");
	extended_registers_registry.push_back(canidmr7_var);
	canidmr7_var->setCallBack(this, CANIDMR7, &CallBackObject::write, NULL);


//	CANRXFG_START=0x20, CANRXFG_END=0x2F, CANTXFG_START=0x30, CANTXFG_END=0x3F

	Reset();

	ComputeInternalTime();

	return (true);
}


bool S12MSCAN::Setup(ServiceExportBase *srv_export) {

	return (true);
}


bool S12MSCAN::EndSetup() {
	return (true);
}


Register* S12MSCAN::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

void S12MSCAN::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}


void S12MSCAN::OnDisconnect() {
}


void S12MSCAN::Reset() {

	canctl0_register = 0x11;
	canctl1_register = 0x11;
	canbtr0_register = 0x00;
	canbtr1_register = 0x00;
	canrflg_register = 0x00;
	canrier_register = 0x00;
	cantflg_register = 0x07;
	cantier_register = 0x00;
	cantarq_register = 0x00;
	cantaak_register = 0x00;
	cantbsel_register = 0x00;
	canidac_register = 0x00;
	reserved1 = 0x00;
	canmisc_register = 0x00;
	canrxerr_register = 0x00;
	cantxerr_register = 0x00;

	for (int i=0; i<8; i++) {
		canidar_register[i] = 0x00;
		canidmr_register[i] = 0x00;
	}

	for (int i=0; i<5; i++) {
		for (int j=0; j<16; j++) {
			canrxfg_register[i][j] = 0x00;
		}
	}

	for (int i=0; i<3; i++) {
		for (int j=0; j<16; j++) {
			cantxfg_register[i][j] = 0x00;
		}
	}

}

void S12MSCAN::ResetMemory() {

	Reset();
	
}

//=====================================================================
//=             memory interface methods                              =
//=====================================================================


bool S12MSCAN::ReadMemory(physical_address_t addr, void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress + MEMORY_MAP_SIZE))) {

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
			case CANCTL0: {
				*((uint8_t *) buffer) = canctl0_register;
			} break;
			case CANCTL1: {
				*((uint8_t *) buffer) = canctl1_register;
			} break;
			case CANBTR0: {
				*((uint8_t *) buffer) = canbtr0_register;
			} break;
			case CANBTR1: {
				*((uint8_t *) buffer) = canbtr1_register;
			} break;
			case CANRFLG: {
				*((uint8_t *) buffer) = canrflg_register;
			} break;
			case CANRIER: {
				*((uint8_t *) buffer) = canrier_register;
			} break;
			case CANTFLG: {
				*((uint8_t *) buffer) = cantflg_register;
			} break;
			case CANTIER: {
				*((uint8_t *) buffer) = cantier_register;
			} break;
			case CANTARQ: {
				*((uint8_t *) buffer) = cantarq_register;
			} break;
			case CANTAAK: {
				*((uint8_t *) buffer) = cantaak_register;
			} break;
			case CANTBSEL: {
				*((uint8_t *) buffer) = cantbsel_register;
			} break;
			case CANIDAC: {
				*((uint8_t *) buffer) = (canidac_register & 0xF8) | can_hit_indicator[canrxfg_index.getHead()];
			} break;
			case RESERVED1: {
				*((uint8_t *) buffer) = 0x00;
			} break;
			case CANMISC: {
				*((uint8_t *) buffer) = canmisc_register;
			} break;
			case CANRXERR: {
				*((uint8_t *) buffer) = canrxerr_register;
			} break;
			case CANTXERR: {
				*((uint8_t *) buffer) = cantxerr_register;
			} break;
			case CANIDAR0: {
				*((uint8_t *) buffer) = canidar_register[0];
			} break;
			case CANIDAR1: {
				*((uint8_t *) buffer) = canidar_register[1];
			} break;
			case CANIDAR2: {
				*((uint8_t *) buffer) = canidar_register[2];
			} break;
			case CANIDAR3: {
				*((uint8_t *) buffer) = canidar_register[3];
			} break;
			case CANIDMR0: {
				*((uint8_t *) buffer) = canidmr_register[0];
			} break;
			case CANIDMR1: {
				*((uint8_t *) buffer) = canidmr_register[1];
			} break;
			case CANIDMR2: {
				*((uint8_t *) buffer) = canidmr_register[2];
			} break;
			case CANIDMR3: {
				*((uint8_t *) buffer) = canidmr_register[3];
			} break;
			case CANIDAR4: {
				*((uint8_t *) buffer) = canidar_register[4];
			} break;
			case CANIDAR5: {
				*((uint8_t *) buffer) = canidar_register[5];
			} break;
			case CANIDAR6: {
				*((uint8_t *) buffer) = canidar_register[6];
			} break;
			case CANIDAR7: {
				*((uint8_t *) buffer) = canidar_register[7];
			} break;
			case CANIDMR4: {
				*((uint8_t *) buffer) = canidmr_register[4];
			} break;
			case CANIDMR5: {
				*((uint8_t *) buffer) = canidmr_register[5];
			} break;
			case CANIDMR6: {
				*((uint8_t *) buffer) = canidmr_register[6];
			} break;
			case CANIDMR7: {
				*((uint8_t *) buffer) = canidmr_register[7];
			} break;

			default: {
				if ((offset >= CANRXFG_START) && (offset <= CANRXFG_END)) {
					if (canrxfg_index.isEmpty()) {
						*((uint8_t *) buffer) = 0x00;
					} else {
						*((uint8_t *) buffer) = canrxfg_register[canrxfg_index.getHead()][offset - CANRXFG_START];
					}

				}
				else if ((offset >= CANTXFG_START) && (offset <= CANTXFG_END)) {
					if (isTxSelected()) {
						*((uint8_t *) buffer) = cantxfg_register[getTxIndex()][offset - CANTXFG_START];
					} else {
//						std::cerr << sc_object::name() << "::Warning: Try to read to CANTXFG but no TXi is selected !" << std::endl;
						*((uint8_t *) buffer) = 0x00;
					}

				} else {
					return false;
				}
			} break;
		}

		return (true);

	}

	return (false);
}


bool S12MSCAN::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress + MEMORY_MAP_SIZE))) {

		if (size == 0) {
			return (true);
		}

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
			case CANCTL0: {
				canctl0_register = *((uint8_t *) buffer);
			} break;
			case CANCTL1: {
				canctl1_register = *((uint8_t *) buffer);
			} break;
			case CANBTR0: {
				canbtr0_register = *((uint8_t *) buffer);
			} break;
			case CANBTR1: {
				canbtr1_register = *((uint8_t *) buffer);
			} break;
			case CANRFLG: {
				canrflg_register = *((uint8_t *) buffer);
			} break;
			case CANRIER: {
				canrier_register = *((uint8_t *) buffer);
			} break;
			case CANTFLG: {
				cantflg_register = *((uint8_t *) buffer);
			} break;
			case CANTIER: {
				cantier_register = *((uint8_t *) buffer);
			} break;
			case CANTARQ: {
				cantarq_register = *((uint8_t *) buffer);
			} break;
			case CANTAAK: {
				cantaak_register = *((uint8_t *) buffer);
			} break;
			case CANTBSEL: {
				cantbsel_register = *((uint8_t *) buffer);
			} break;
			case CANIDAC: {
				canidac_register = (*((uint8_t *) buffer) & 0x3) | (canidac_register & 0xC7);
			} break;
			case RESERVED1: {
				// NOP
			} break;
			case CANMISC: {
				canmisc_register = *((uint8_t *) buffer);
			} break;
			case CANRXERR: {
				canrxerr_register = *((uint8_t *) buffer);
			} break;
			case CANTXERR: {
				cantxerr_register = *((uint8_t *) buffer);
			} break;
			case CANIDAR0: {
				canidar_register[0] = *((uint8_t *) buffer);
			} break;
			case CANIDAR1: {
				canidar_register[1] = *((uint8_t *) buffer);
			} break;
			case CANIDAR2: {
				canidar_register[2] = *((uint8_t *) buffer);
			} break;
			case CANIDAR3: {
				canidar_register[3] = *((uint8_t *) buffer);
			} break;
			case CANIDMR0: {
				canidmr_register[0] = *((uint8_t *) buffer);
			} break;
			case CANIDMR1: {
				canidmr_register[1] = *((uint8_t *) buffer);
			} break;
			case CANIDMR2: {
				canidmr_register[2] = *((uint8_t *) buffer);
			} break;
			case CANIDMR3: {
				canidmr_register[3] = *((uint8_t *) buffer);
			} break;
			case CANIDAR4: {
				canidar_register[4] = *((uint8_t *) buffer);
			} break;
			case CANIDAR5: {
				canidar_register[5] = *((uint8_t *) buffer);
			} break;
			case CANIDAR6: {
				canidar_register[6] = *((uint8_t *) buffer);
			} break;
			case CANIDAR7: {
				canidar_register[7] = *((uint8_t *) buffer);
			} break;
			case CANIDMR4: {
				canidmr_register[4] = *((uint8_t *) buffer);
			} break;
			case CANIDMR5: {
				canidmr_register[5] = *((uint8_t *) buffer);
			} break;
			case CANIDMR6: {
				canidmr_register[6] = *((uint8_t *) buffer);
			} break;
			case CANIDMR7: {
				canidmr_register[7] = *((uint8_t *) buffer);
			} break;

			default: {
				if ((offset >= CANRXFG_START) && (offset <= CANRXFG_END)) {
					// NOP:
					std::cerr << sc_object::name() << "::Warning: Try to write to CANRXFG but write is unimplemented for this register !" << std::endl;
				}
				else if ((offset >= CANTXFG_START) && (offset <= CANTXFG_END)) {
					if (isTxSelected()) {
						cantxfg_register[getTxIndex()][offset - CANTXFG_START] = *((uint8_t *) buffer);
					} else {
						std::cerr << sc_object::name() << "::Warning: Try to write to CANTXFG but no TXi is selected !" << std::endl;
					}

				} else {
					return false;
				}
			} break;
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





