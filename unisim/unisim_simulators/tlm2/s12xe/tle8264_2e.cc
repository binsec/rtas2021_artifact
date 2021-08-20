
#include "tle8264_2e.hh"

TLE8264_2E::TLE8264_2E(const sc_module_name& name, Object *parent) :
	Object(name)
	, sc_module(name)

	, interrupt_request("interrupt-request")
	, bus_clock_socket("bus-clock-socket")
	, can_slave_rx_sock("slave-rx")
	, can_slave_tx_sock("slave-tx")

	, spi_tx_socket("tx-socket")
	, spi_rx_socket("rx-socket")

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

	, current_mode(INIT)
	, current_cmd(INIT)
	, last_state(INIT)
	, wd_refresh(false)
	, wk_state_register(WK_STATE_DEFAULT)
	, reserved(0x0000)
	, spi_rx_buffer(false)
	, SPIWake_read(false)

	, terminated(false)

	, frame_time(256, SC_MS)
	, limp_home(false)
	, sig_limp_home("limp-home", this, limp_home)

// Interrupt ID
	, reset_interrupt(0xFE)
	, param_reset_interrupt("reset-interrupt", this, reset_interrupt)
	, int_interrupt(0xF2) // look to BCM specification
	, param_int_interrupt("int-interrupt", this, int_interrupt)

	, debug_enabled(false)
	, param_debug_enabled("debug-enabled", this, debug_enabled)

{
	SC_HAS_PROCESS(TLE8264_2E);

	SC_THREAD(WatchDogThread);
	SC_THREAD(stateMachineThread);

	xint_payload = xint_payload_fabric.allocate();

	interrupt_request(*this);

	bus_clock_socket.register_b_transport(this, &TLE8264_2E::updateBusClock);

	spi_rx_socket.register_b_transport(this, &TLE8264_2E::spi_rx_b_transport);

	// target
	can_slave_rx_sock.register_b_transport(this, &TLE8264_2E::can_slave_b_transport);
	can_slave_rx_sock.register_get_direct_mem_ptr(this, &TLE8264_2E::can_slave_get_direct_mem_ptr);
	can_slave_rx_sock.register_nb_transport_fw(this, &TLE8264_2E::can_slave_nb_transport_fw);
	can_slave_rx_sock.register_transport_dbg(this, &TLE8264_2E::can_slave_transport_dbg);

	can_master_rx_sock.register_b_transport(this, &TLE8264_2E::can_master_b_transport);
	can_master_rx_sock.register_get_direct_mem_ptr(this, &TLE8264_2E::can_master_get_direct_mem_ptr);
	can_master_rx_sock.register_nb_transport_fw(this, &TLE8264_2E::can_master_nb_transport_fw);
	can_master_rx_sock.register_transport_dbg(this, &TLE8264_2E::can_master_transport_dbg);

	// initiator
	can_slave_tx_sock.register_invalidate_direct_mem_ptr(this, &TLE8264_2E::can_slave_invalidate_direct_mem_ptr);
	can_slave_tx_sock.register_nb_transport_bw(this, &TLE8264_2E::can_slave_nb_transport_bw);

	can_master_tx_sock.register_invalidate_direct_mem_ptr(this, &TLE8264_2E::can_master_invalidate_direct_mem_ptr);
	can_master_tx_sock.register_nb_transport_bw(this, &TLE8264_2E::can_master_nb_transport_bw);

}

TLE8264_2E::~TLE8264_2E() { }

bool TLE8264_2E::BeginSetup() {

	Reset();

	return (true);
}


bool TLE8264_2E::Setup(ServiceExportBase *srv_export) {

	return (true);
}


bool TLE8264_2E::EndSetup() {
	return (true);
}

void TLE8264_2E::OnDisconnect() {
}

void TLE8264_2E::Reset() {

	std::cout << sc_time_stamp() << " TLE::Reset" << std::endl;

	wk_state_register = WK_STATE_DEFAULT;
	wd_refresh = false;

	// interrupt mask
	mask_registers[0b000] = REG_000_MSK_DEFAULT;
	mask_registers[0b001] = REG_001_MSK_DEFAULT;
	mask_registers[0b010] = REG_010_MSK_DEFAULT;
	mask_registers[0b011] = REG_011_MSK_DEFAULT;

	// interrupt status
	status_registers[0b000] = REG_000_DEFAULT;
	status_registers[0b001] = REG_001_DEFAULT;
	status_registers[0b010] = REG_010_DEFAULT;
	status_registers[0b011] = REG_011_DEFAULT;

//	reserved = REG_011_DEFAULT;

	// configuration registers
	cfg_registers[0b100] = REG_100_DEFAULT;
	cfg_registers[0b101] = REG_101_DEFAULT;
	cfg_registers[0b110] = REG_110_DEFAULT;
	cfg_registers[0b111] = REG_111_DEFAULT;

}

void TLE8264_2E::Stop(int exit_status) {}

void TLE8264_2E::ComputeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

}

void TLE8264_2E::stateMachineThread()
{
	while (true) {

		wait(state_machine_event);

		setCmd(spi_rx_buffer);
	}
}


void TLE8264_2E::spi_rx_b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	payload.acquire();
// 	unsigned int length = payload.get_data_length();
// 	unsigned char* data = payload.get_data_ptr();

//	if (payload.get_data_length() != 2)
//	{
//		std::cerr << sc_object::name() << "::Warning interface is configured for 2-bytes" << std::endl;
//	}

	spi_rx_buffer = *((uint16_t *) payload.get_data_ptr());

	state_machine_event.notify();

	payload.release();
}

tlm_sync_enum TLE8264_2E::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}

void TLE8264_2E::invalidate_direct_mem_ptr( sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

/** CAN slave interface micro->tle **/
bool TLE8264_2E::can_slave_get_direct_mem_ptr(CAN_Payload& payload, tlm_dmi&  dmi_data) {
	return (false);
}

unsigned int TLE8264_2E::can_slave_transport_dbg(CAN_Payload& payload)
{
	// Leave this empty as it is designed for memory mapped buses
	return (0);
}

tlm_sync_enum TLE8264_2E::can_slave_nb_transport_fw(CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	tlm_sync_enum resp = TLM_ACCEPTED;
	switch(phase)
	{
		case BEGIN_REQ:
			// accepts analog signals modeled by payload
//			phase = END_REQ; // update the phase
//			payload.acquire();
//			rx_payload_queue.notify(payload, t); // queue the payload and the associative time

			slave_rx_event.notify();
			resp = can_master_tx_sock->nb_transport_fw(payload, phase, t);

			return (resp);
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

	return (resp);
}

tlm_sync_enum TLE8264_2E::can_slave_nb_transport_bw(CAN_Payload& can_rx_payload, tlm_phase& phase, sc_core::sc_time& t)
{
	tlm_sync_enum resp = TLM_ACCEPTED;
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
			resp = can_master_rx_sock->nb_transport_bw(can_rx_payload, phase, t);
			return (resp);
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

void TLE8264_2E::can_slave_b_transport(CAN_Payload& payload, sc_core::sc_time& t) {
//	payload.acquire();
//	rx_payload_queue.notify(payload, t);

	slave_rx_event.notify();
	can_master_tx_sock->b_transport(payload, t);
}

void TLE8264_2E::can_slave_invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

/** CAN master interface tle->bus **/
bool TLE8264_2E::can_master_get_direct_mem_ptr(CAN_Payload& payload, tlm_dmi&  dmi_data) {
	return (false);
}

unsigned int TLE8264_2E::can_master_transport_dbg(CAN_Payload& payload)
{
	// Leave this empty as it is designed for memory mapped buses
	return (0);
}

tlm_sync_enum TLE8264_2E::can_master_nb_transport_fw(CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	tlm_sync_enum resp = TLM_ACCEPTED;
	switch(phase)
	{
		case BEGIN_REQ:
//			phase = END_REQ; // update the phase
//			payload.acquire();
//			rx_payload_queue.notify(payload, t); // queue the payload and the associative time

			master_rx_event.notify();
			resp = can_slave_tx_sock->nb_transport_fw(payload, phase, t);

			return (resp);
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

	return (resp);
}

tlm_sync_enum TLE8264_2E::can_master_nb_transport_bw(CAN_Payload& can_rx_payload, tlm_phase& phase, sc_core::sc_time& t)
{
	tlm_sync_enum resp = TLM_ACCEPTED;

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
			//can_bw_event.notify();
			resp = can_slave_rx_sock->nb_transport_bw(can_rx_payload, phase, t);
			return (resp);

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

void TLE8264_2E::can_master_b_transport(CAN_Payload& payload, sc_core::sc_time& t) {
//	payload.acquire();
//	rx_payload_queue.notify(payload, t);

	master_rx_event.notify();
	can_slave_tx_sock->b_transport(payload, t);

}

void TLE8264_2E::can_master_invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

void TLE8264_2E::updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}


void TLE8264_2E::setLimpHome(bool val) {
	cfg_registers[0] = (cfg_registers[0] & 0xFFBF) | ((val)? LIMPHOME: 0x0000);
}
bool TLE8264_2E::isLimpHome() { return ((cfg_registers[0] & LIMPHOME) != 0); }

bool TLE8264_2E::isINT() { return ((status_registers[0] & INT) != 0); }
bool TLE8264_2E::isWKCyclicEnabled() { return ((status_registers[0] & WKCYCLIC) != 0); }

void TLE8264_2E::WatchDogThread() {

	while (true) {
		while (!isWatchDogEnable()) {
			wait(watchdog_enable_event);
		}

		while (isWatchDogEnable()) {
			uint16_t state_before = current_mode;

			wait(frame_time);

			if ((state_before == INIT) && (state_before == current_mode)) {
				setLimpHome(true);
				current_mode = RESTART;
				execute_restart();
			}
			else if (isWKCyclicEnabled() && (state_before == STOP) && (state_before == current_mode)) {
				if (isINT() && !SPIWake_read) {
					current_mode = RESTART;
					execute_restart();
				} else {
					assert_int_interrup();
				}
			} else if ((state_before == SWFLASH) && (state_before == current_mode)) {
				/* Note: A mode change to Restart mode can be caused by SPI command,
				 *    a timeout or window watchdog failure or an undervoltage reset
				 */

				// when leaving the SWFlash a reset is generated
				assert_reset_interrupt();
				current_mode = RESTART;
				execute_restart();
			} else if ((state_before == RESTART) && (state_before == current_mode)) {
				current_mode = NORMAL;
				execute_normal();
			}
		}
	}
}

void TLE8264_2E::refresh_watchdog()
{
	// TODO: compute frame_time for watchdog and effectif refresh of watchdog
	wd_refresh = false;
}

void TLE8264_2E::execute_init() {
	// TODO: Workaround. This action is done by default. To confirm !!!
	Reset();
}

void TLE8264_2E::execute_restart() {
	/*
	 * TODO:
	 * The first SPI output data will provide information about the reason for entering Restart Mode.
	 * The reason for entering Restart Mode is stored and kept until the microcontroller reads
	 * the corresponding “LH0..2” or “RM0..1” SPI bits.
	 */
	if ((cfg_registers[0] & RESETDELAY) == 0) {
		current_mode = NORMAL;
		execute_normal();
	}
}

void TLE8264_2E::execute_swflash() {

}

void TLE8264_2E::execute_normal() {
	if (!isWatchDogEnable()) { std::cerr << "In Normal Mode the watchdog needs to be triggered !!!" << std::endl; }

}
void TLE8264_2E::execute_sleep() {
	// Watchdog is disabled by entering Sleep Mode and the last configuration isn't saved
	cfg_registers[2] = REG_110_DEFAULT;

	/*
	 *  TODO: emulate sleep mode and wakeup waiting
	 *  As work-around the SBC goes directly to RESTART mode
	 *
	 * Note:
	 * The first SPI output data when going to SBC Normal Mode will always indicate the wake up source,
	 * as well as the SBC Sleep Mode to indicate where the device comes from and why it left the state.
	 *
	 * The CAN wakeup event has to be set using random canmsg generator or cosimulation stub
	 */

	/*
	 * In case of a wakeup from Sleep Mode the wake source is seen at the interrupt bit
	 * (config. select 0b000) an interrupt is not generated
	 */
	status_registers[00] = status_registers[0] | INT;
	current_mode = RESTART;

	// To change. I have choose to set wakeup reason to CAN event. Others event are possible LIN, WK-pin, cyclic
	status_registers[0b000] = ((mask_registers[0] & WKCAN) != 0)? status_registers[0] | WKCAN: status_registers[0];

	execute_restart();
}
void TLE8264_2E::execute_stop() {

}
void TLE8264_2E::execute_sailsafe() {

}

uint16_t TLE8264_2E::execute_readonly(uint16_t sel)
{
    // response word = WK(1) REG(9) SEL(3) CMD(3)

	uint16_t response = wk_state_register | (sel << 3) | READONLY;

	switch (sel)
	{
		case 0b000: {
			if ((current_mode == STOP) && isWKCyclicEnabled() && isINT()) {
				SPIWake_read = true;
			}
			status_registers[0] = status_registers[0] & 0x100; // clear all interrupt flags and left 'INT' as it
			response = response | (status_registers[0] << 6);
		} break;
		case 0b001: {
			status_registers[1] = status_registers[1] & 0x000;
			response = response | (status_registers[1] << 6);
		} break;
		case 0b010: {
			status_registers[2] = status_registers[2] & 0x000;
			response = response | (status_registers[2] << 6);
		} break;
		case 0b011: {
// 20160513: start update
			/* NOP; reserved register */
//			std::cerr << "TLE8264_2E:: read of reserved register 0b011" << std::endl;
//			response = response | reserved;

			std::cerr << "TLE8264_2E:: read (done) of reserved register 0b011" << std::endl;
			status_registers[3] = status_registers[3] & 0x000;
			response = response | (status_registers[3] << 6);

// 20160513: end update
		} break;
		case 0b100: {
			response = response | (cfg_registers[0] << 6);
		} break;
		case 0b101: {
			response = response | (cfg_registers[1] << 6);
		} break;
		case 0b110: {
			response = response | (cfg_registers[2] << 6);
		} break;
		case 0b111: {
			response = response | (cfg_registers[3] << 6);
		} break;
	}

	if (((status_registers[0] & 0x07F) == 0) && ((status_registers[1] & 0x17F) == 0) && ((status_registers[2] & 0x1FF) == 0) && ((status_registers[3] & 0x1FF) == 0))
	{
		status_registers[0] = status_registers[0] & 0x0FF;  // clear 'INT' flag. equivalent to 'status_registers[0] = REG_000_DEFAULT'
		if (sel == 0b000) {
			response = wk_state_register | (sel << 3) | READONLY;  // all flags of status_reg_000 are cleared
		}
	}

	return (response);
}

void TLE8264_2E::assertInterrupt(uint8_t interrupt_offset) {


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

void TLE8264_2E::assert_int_interrup() {
	status_registers[0] = status_registers[0b000] | INT;
	assertInterrupt(int_interrupt);
}
void TLE8264_2E::assert_reset_interrupt() {
	status_registers[1] = status_registers[1] | RESET;
	assertInterrupt(reset_interrupt);
}

uint16_t TLE8264_2E::triggerStateMachine(uint16_t spi_cmd) {

	uint16_t new_mode = spi_cmd & 0x0007;
	uint16_t sel  = (spi_cmd & 0x0038) >> 3;
	uint16_t val = spi_cmd >> 6;

	if (debug_enabled) {
		std::cout << sc_time_stamp() << " " << sc_object::name() << "::triggerStateMachine()  execute CMD 0x" << std::hex << spi_cmd << std::dec << std::endl;
	}

	switch (current_cmd & 0x0007)
	{
	case INIT: {
		switch (new_mode)
		{
		case INIT: {
			// No mode change
			current_cmd = write(sel, val);
		} break;

// 20160513: start add - transition due to "Init mode not successful"
		case RESTART: {
			current_mode = new_mode;
			current_cmd = write(sel, val);
			// when leaving the SWFlash a reset is generated
			assert_reset_interrupt();
			execute_restart();
		} break;
// 20160513: end adding

		case NORMAL: {
			current_mode = new_mode;
			current_cmd = write(sel, val);
			execute_normal();
		} break;
		case SWFLASH: {
			current_mode = new_mode;
			current_cmd = write(sel, val);
			execute_swflash();
		} break;
		case READONLY: {
			/*no change but triggers wd and clear int ...*/
			return (execute_readonly(sel));

		} break;
		default: std::cout << sc_time_stamp() << " (1) change mode not supported 0x" << std::hex << spi_cmd << std::dec << std::endl; break;
		}

	} break;
	case RESTART: {
		switch (new_mode)
		{
		case INIT: {
			current_cmd = write(sel, val);
		} break;
//					case NORMAL: current_cmd = normal(spi_cmd); break;
		case READONLY: {
			/*no change but triggers wd and clear int ...*/
			return (execute_readonly(sel));
		} break;
		default: std::cout << sc_time_stamp() << " (2) change mode not supported 0x" << std::hex << spi_cmd << std::dec << std::endl; break;
		}

	} break;
	case SWFLASH: {
		switch (new_mode)
		{
		case INIT: {
			current_cmd = write(sel, val);
		} break;
		case SWFLASH: {
			// No mode change
			current_cmd = write(sel, val);
		} break;
		case RESTART: {
			current_mode = new_mode;
			current_cmd = write(sel, val);
			// when leaving the SWFlash a reset is generated
			assert_reset_interrupt();
			execute_restart();
		} break;
		case READONLY: {
			/*no change but triggers wd and clear int ...*/
			return (execute_readonly(sel));
		} break;
		default: std::cout << sc_time_stamp() << " (3) change mode not supported 0x" << std::hex << spi_cmd << std::dec << std::endl; break;
		}

	} break;
	case NORMAL: {
		switch (new_mode)
		{
		case INIT: {
			current_cmd = write(sel, val);
		} break;
		case RESTART: {
			std::cout << sc_time_stamp() << " (4) change mode (done) normal->restart 0x" << std::hex << spi_cmd << std::dec << " Register updated"<< std::endl;
// 20160513: the commented line is replaced by the below code
//			current_cmd = write(sel, val);

// 20160513: start adding
			current_mode = new_mode;
			current_cmd = write(sel, val);
			// when leaving the SWFlash a reset is generated
			assert_reset_interrupt();
			execute_restart();
// 20160513: end adding
		} break;
		case SWFLASH: {
			current_mode = new_mode;
			current_cmd = write(sel, val);
			assert_reset_interrupt(); // A reset is triggered when entering SWFlash from Normal
			execute_swflash();
		} break;
		case NORMAL: {
			current_cmd = write(sel, val);
		} break;
		case SLEEP: {
			current_cmd = write(sel, val);
			if (((mask_registers[0] & 0x0F) == 0) && ((cfg_registers[0] & (CYCLICWK | WKPIN)) ==0 ))
			{
				/*
				 * In Sleep Mode, not all wake-up sources should be inhibited,
				 * this is required to not program the device in a mode where it can not wake up.
				 * If all wake sources are inhibited when sending the SBC to Sleep Mode,
				 * the SBC does not go to Sleep Mode, the microcontroller is informed via the INT output, and the SPI bit “Fail SPI” is set.
				 */
				status_registers[1] = status_registers[1] | FAILSPI;
				assert_int_interrup();

			} else {
				current_mode = new_mode;
				execute_sleep();
			}

		} break;
		case STOP: {
			uint16_t watchdog_settings = cfg_registers[2];
			current_cmd = write(sel, val);
			if (((mask_registers[0] & 0x0F) == 0) && ((cfg_registers[0] & (CYCLICWK | WKPIN)) ==0 ))
			{
				/*
				 * If all wakeup sources are disabled, (CAN, LIN, WK, cyclic wake) the watchdog can not be disabled,
				 * the SBC stays in Normal Mode and the watchdog continues with the old settings.
				 */
				cfg_registers[2] = watchdog_settings;
			} else {
				current_mode = new_mode;
				execute_stop();
			}
		} break;
		case READONLY: {
			/*no change but triggers wd and clear int ...*/
			return (execute_readonly(sel));
		} break;
		default: std::cout << sc_time_stamp() << " (5) change mode not supported 0x" << std::hex << spi_cmd << std::dec << std::endl; break;
		}

	} break;
	case SLEEP: {
		switch (new_mode)
		{
		case INIT: {
			current_cmd = write(sel, val);
		} break;
		case SLEEP: {
			current_cmd = write(sel, val);
		} break;
		//					case RESTART: current_cmd = restart(spi_cmd); break;
		case READONLY: {
			/*no change but triggers wd and clear int ...*/
			return (execute_readonly(sel));
		} break;
		default: std::cout << sc_time_stamp() << " (6) change mode not supported 0x" << std::hex << spi_cmd << std::dec << std::endl; break;
		}

	} break;
	case STOP: {
		switch (new_mode)
		{
		case INIT: {
			current_cmd = write(sel, val);
		} break;
		case NORMAL: {
			current_mode = new_mode;
			current_cmd = write(sel, val);
			execute_normal();
		} break;
		case SLEEP: {
			current_mode = new_mode;
			current_cmd = write(sel, val);
			execute_sleep();
		} break;
		case STOP: {
			current_cmd = write(sel, val);
		} break;
		case READONLY: {
			/*no change but triggers wd and clear int ...*/
			return (execute_readonly(sel));
		} break;
		default: std::cout << sc_time_stamp() << " (7) change mode not supported 0x" << std::hex << spi_cmd << std::dec << std::endl; break;
		}

	} break;
	case FAILSAFE: {
		switch (new_mode)
		{
		case INIT: {
			current_cmd = write(sel, val);
		} break;
		//					case RESTART: current_cmd = restart(spi_cmd); break;
		case FAILSAFE: {
			current_cmd = write(sel, val);
		} break;
		case READONLY: {
			/*no change but triggers wd and clear int ...*/
			return (execute_readonly(sel));
		} break;
		default: std::cout << sc_time_stamp() << " (8) change mode not supported 0x" << std::hex << spi_cmd << std::dec << std::endl; break;
		}

	} break;
	default: std::cout << sc_time_stamp() << " Reserved mode not supported 0x" << std::hex << spi_cmd << std::dec << std::endl; break;
	}

	// The default value of 'WD refresh bit' is 0. The first trigger must be 1 and the following triggers must be 0.
	if (wd_refresh ^ ((spi_cmd & 0x8000) != 0x8000))
	{
		wd_refresh = true;
		refresh_watchdog();
	}

	return (current_cmd);
}

void TLE8264_2E::setCmd(uint16_t cmd) {

	uint16_t state_to_send = last_state;

	last_state = triggerStateMachine(cmd);

	tlm::tlm_generic_payload *payload = spi_payload_fabric.allocate();

	payload->set_data_length(2);
	payload->set_data_ptr((unsigned char*) &state_to_send);

	sc_time local_time = SC_ZERO_TIME;
	spi_tx_socket->b_transport(*payload, local_time);

	payload->release();

}

uint16_t TLE8264_2E::write(uint16_t sel, uint16_t val) {

    // response word = WK(1) REG(9) SEL(3) CMD(3)

	uint16_t response = wk_state_register | (sel << 3) | current_mode;
	/* 0b000-0b001-0b010-0b011: Interrupt mask. A value of 0 will set the SBC into the opposite state. */
	switch (sel)
	{
		case 0b000: {
			mask_registers[0] = ~(mask_registers[0] ^ val) & 0x01FF;
			response = response | (status_registers[0] << 6);
		} break;
		case 0b001: {
			mask_registers[1] = ~(mask_registers[1] ^ val) & 0x01FF;
			response = response | (status_registers[1] << 6);
		} break;
		case 0b010: {
			mask_registers[2] = ~(mask_registers[2] ^ val) & 0x01FF;
			response = response | (status_registers[2] << 6);
		} break;
		case 0b011: {
// 20160513: start update
			/* NOP; reserved register */
//			std::cerr << "TLE8264_2E:: write to reserved register 0b011 value=0x" << std::hex << val << std::endl;
			std::cerr << "TLE8264_2E:: write (done) to reserved register 0b011 value=0x" << std::hex << val << std::endl;
			mask_registers[3] = ~(mask_registers[3] ^ val) & 0x01FF;
			response = response | (status_registers[3] << 6);
// 20160513: end update
		} break;
		case 0b100: {
			cfg_registers[0] = val;
			response = response | (cfg_registers[0] << 6);
		} break;
		case 0b101: {
			cfg_registers[1] = val;
			response = response | (cfg_registers[1] << 6);
		} break;
		case 0b110: {
// 20160513: start add
			std::cout << sc_time_stamp() << " TLE::WD write before 0x" << std::hex << cfg_registers[2] << " new value 0x" << std::hex << val << std::endl;
// 20160513: end add
			cfg_registers[2] = val;
			computeWDTimeer();
			if (isWatchDogEnable()) { watchdog_enable_event.notify(); }

			response = response | (cfg_registers[2] << 6);
		} break;
		case 0b111: {
			/* NOP; reserved register */
			response = response | (cfg_registers[3] << 6);
		} break;
	}

	return (response);
}
