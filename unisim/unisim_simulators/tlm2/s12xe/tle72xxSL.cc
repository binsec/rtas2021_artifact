/*
 * tle72xxSL.cc
 *
 *  Created on: 8 août 2012
 *      Author: rnouacer
 */

#include <tle72xxSL.hh>


TLE72XXSL::TLE72XXSL(const sc_module_name& name, Object *parent) :
	Object(name, parent)
	, sc_module(name)

	, unisim::kernel::Client<CharIO>(name, parent)

	, char_io_import("char-io-import", this)

	, bus_clock_socket("bus-clock-socket")
	, tx_socket("tx-socket")
	, rx_socket("rx-socket")

	, frameLength(8)

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

	, interrupt_offset(0xD8)
	, param_interrupt_offset("interrupt-offset", this, interrupt_offset)

	, debug_enabled(false)
	, param_debug_enabled("debug-enabled", this, debug_enabled)

	, tx_debug_enabled(false)
	, param_tx_debug_enabled("tx-debug-enabled", this, tx_debug_enabled)
	, rx_debug_enabled(false)
	, param_rx_debug_enabled("rx-debug-enabled", this, rx_debug_enabled)

	, state(IDLE)
	, abortTransmission(false)
	, spisr_read(false)
	, validFrameWaiting(false)

	, mosi(false)
	, mosi_pin("MOSI", this, mosi, "master output and slave input pin")

	, miso(false)
	, miso_pin("MISO", this, miso, "master input and slave output pin")

	, ss(true)
	, ss_pin("SS", this, ss, "Select signal pin. If MODFEN and SSOE bit are set, the SS pin is configured as slave select output (low during transmission and high for idle).")

	, sck(false)
	, sck_pin("SCK", this, sck, "SPI clock")

	, spicr1_register(0x04)
	, spicr2_register(0x00)
	, spibr_register(0x00)
	, spisr_register(0x20)
	, spidr_register(0x0000)

	, spidr_rx_buffer(0)

	, telnet_enabled(false)
	, param_telnet_enabled("telnet-enabled", this, telnet_enabled)

	, logger(*this)

{

	bus_clock_socket.register_b_transport(this, &TLE72XXSL::updateBusClock);

	rx_socket.register_b_transport(this, &TLE72XXSL::rx_b_transport);

	SC_HAS_PROCESS(TLE72XXSL);

	SC_THREAD(TxRun);
	SC_THREAD(RxRun);

	SC_THREAD(TelnetProcessInput);

}


TLE72XXSL::~TLE72XXSL() {

	// Release registers_registry
	map<string, unisim::service::interfaces::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		if(reg_iter->second)
			delete reg_iter->second;
	}

	registers_registry.clear();

	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}
}

void TLE72XXSL::TxRun() {

	while (true) {
		while (!isSPIEnabled()) {
			if (tx_debug_enabled)	std::cout << sc_object::name() << "::Tx  (1) " << std::endl;

			wait(tx_run_event);

			if (tx_debug_enabled)	std::cout << sc_object::name() << "::Tx  (2) " << std::endl;

		}

		while (isSPIEnabled() && !isAbortTransmission()) {

			if (tx_debug_enabled)	std::cout << sc_object::name() << "::Tx  (3) " << std::endl;

			if (isSPTEF()) {
				if (tx_debug_enabled)	std::cout << sc_object::name() << "::Tx  (4) " << std::endl;

				wait(tx_load_event);

				if (tx_debug_enabled)	std::cout << sc_object::name() << "::Tx  (5) " << std::endl;

				continue;
			}

			if (checkModeFaultError()) { break; }

			if (tx_debug_enabled)	std::cout << sc_object::name() << "::Tx  (6) " << std::endl;

			if (tx_debug_enabled)	std::cout << sc_object::name() << "::txShiftOut Start transmission -> " << std::hex << (unsigned int) spidr_register << std::dec << std::endl;

			uint16_t tx_shift = spidr_register;

			setSPTEF();

			startTransmission();

			// use Telnet as an echo
			if (telnet_enabled) {
				if (is16bitsMode()) {
					if (isLSBFirst()) {
						add(telnet_tx_fifo, ((uint8_t*) &tx_shift)[0], telnet_tx_event);
						add(telnet_tx_fifo, ((uint8_t*) &tx_shift)[1], telnet_tx_event);
					} else {
						add(telnet_tx_fifo, ((uint8_t*) &tx_shift)[1], telnet_tx_event);
						add(telnet_tx_fifo, ((uint8_t*) &tx_shift)[0], telnet_tx_event);
					}
				} else {
					add(telnet_tx_fifo, ((uint8_t*) &tx_shift)[0], telnet_tx_event);
				}
				TelnetProcessOutput(true);

				// TODO I have to emulate Mode Fault Error
			}
			else
			{

				if (checkModeFaultError()) {
					// TODO I have to emulate Mode Fault Error
					break;
				}

				tlm::tlm_generic_payload *payload = spi_payload_fabric.allocate();
				if (is16bitsMode()) {
					payload->set_data_length(2);
					payload->set_data_ptr((unsigned char*) &tx_shift);
				} else {
					payload->set_data_length(1);
					payload->set_data_ptr((unsigned char*) &((&tx_shift)[0]));
				}

				wait(spi_baud_rate * frameLength);

				sc_time local_time = SC_ZERO_TIME;
				tx_socket->b_transport(*payload, local_time);
				payload->release();

//				// TODO I have to rewrite the following code using TLM and stub
//				uint8_t index = 0;
//				while (isSPIEnabled() && (index < frameLength) && !isAbortTransmission()) {
//
//					if (checkModeFaultError()) {
//						// TODO I have to emulate Mode Fault Error
//						break;
//					}
//
//					if (isLSBFirst()) {
//						pinWrite(tx_shift & 0x0001);
//
//						tx_shift = tx_shift >> 1;
//					} else {
//						if (is16bitsMode()) {
//							pinWrite(tx_shift & 0x8000);
//						} else {
//							pinWrite(tx_shift & 0x0080);
//						}
//
//						tx_shift = tx_shift << 1;
//					}
//
//					index++;
//
//					wait(spi_baud_rate);
//
//				}

			}

			endTransmission();

			if (tx_debug_enabled)	std::cout << sc_object::name() << "::Tx  (7) " << std::endl;

		}

		if (isAbortTransmission()) { abortTransmission = false; }

	}
}

void TLE72XXSL::RxRun() {

	while (true) {
		while (!isSPIEnabled()) {
			if (rx_debug_enabled)	std::cout << sc_object::name() << "::Rx  (1) " << std::endl;

			wait(rx_run_event);

			if (rx_debug_enabled)	std::cout << sc_object::name() << "::Rx  (2) " << std::endl;
		}

		bool newFrameStart = false;
		while (isSPIEnabled() && !newFrameStart) {

			if (rx_debug_enabled)	std::cout << sc_object::name() << "::Rx  (3) " << std::endl;

			if (telnet_enabled) {

				if (rx_debug_enabled)	std::cout << sc_object::name() << "::Rx  (4) " << std::endl;

				wait(spi_baud_rate);

//				TelnetProcessInput();

				newFrameStart = !isEmpty(telnet_rx_fifo);

				if (rx_debug_enabled)	std::cout << sc_object::name() << "::Rx  (5) " << std::endl;

			} else {
				if (rx_debug_enabled)	std::cout << sc_object::name() << "::Rx  (6) " << std::endl;

				wait(rx_buffer_event);
				newFrameStart = true;
			}

		}

		if (rx_debug_enabled)	std::cout << sc_object::name() << "::Rx  (7) " << std::endl;

		if (!newFrameStart) { continue; }

		if (rx_debug_enabled)	std::cout << sc_object::name() << "::Rx  (8) " << std::endl;

		if (isValideFrameWaiting()) {
			if (rx_debug_enabled)	std::cout << sc_object::name() << "::Rx  (9) " << std::endl;

			setValideFrameWaiting(false);

			if (rx_debug_enabled)	std::cout << sc_object::name() << "::Rx  (10) " << std::endl;
		}

		if (rx_debug_enabled)	std::cout << sc_object::name() << "::Rx  (11) " << std::endl;

		if (telnet_enabled) {

			uint16_t rx_shift = 0;

			setActive();

			if (rx_debug_enabled)	std::cout << sc_object::name() << "::Rx  (12) " << std::endl;

			if (is16bitsMode()) {
				if (isLSBFirst()) {
					next(telnet_rx_fifo, ((uint8_t*) &rx_shift)[0], telnet_rx_event);
					next(telnet_rx_fifo, ((uint8_t*) &rx_shift)[1], telnet_rx_event);
				} else {
					next(telnet_rx_fifo, ((uint8_t*) &rx_shift)[1], telnet_rx_event);
					next(telnet_rx_fifo, ((uint8_t*) &rx_shift)[0], telnet_rx_event);
				}
			} else {
				next(telnet_rx_fifo, ((uint8_t*) &rx_shift)[0], telnet_rx_event);
			}

			if (rx_debug_enabled) std::cout << sc_object::name() << "::Telnet::get  HAVE DATA" << std::endl;

			wait(spi_baud_rate * frameLength);

			spidr_rx_buffer = rx_shift;
			setSPIDR(spidr_rx_buffer);

			if (rx_debug_enabled)	std::cout << sc_object::name() << "::Rx  (13) " << std::endl;

			setIdle();
		}
		else // TLM transaction
		{
			setSPIDR(spidr_rx_buffer);
		}

		if (rx_debug_enabled)	std::cout << sc_object::name() << "::Rx  (14) " << std::endl;
	}
}

void TLE72XXSL::rx_b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	payload.acquire();
// 	unsigned int length = payload.get_data_length();
// 	unsigned char* data = payload.get_data_ptr();

	setSSLow(false);
	setActive();

	if (is16bitsMode()) {
//		if (payload.get_data_length() != 2)
//		{
//			std::cerr << sc_object::name() << "::Warning interface is configured for 2-bytes" << std::endl;
//		}
		spidr_rx_buffer = *((uint16_t *) payload.get_data_ptr());
	} else {
//		if (payload.get_data_length() != 1)
//		{
//			std::cerr << sc_object::name() << "::Warning interface is configured for 1-byte" << std::endl;
//		}
		spidr_rx_buffer = *((uint8_t *) payload.get_data_ptr());
	}

	wait(spi_baud_rate * frameLength);

	rx_buffer_event.notify();

	setSSLow(true);

	setIdle();

	payload.release();
}

void TLE72XXSL::TelnetProcessInput()
{
	while (telnet_enabled) {
		while (!isSPIEnabled()) {

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
				if(debug_enabled)
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

//=====================================================================
//=             registers setters and getters                         =
//=====================================================================

bool TLE72XXSL::read(unsigned int offset, const void *buffer, unsigned int data_length) {

//	std::cout << sc_time_stamp() << "  " << sc_object::name() << "::read offset= " << std::dec << offset << "  size=" << data_length << std::endl;

	switch (offset) {
		case SPICR1: {
			*((uint8_t *) buffer) = spicr1_register;
		} break;	// 1 byte
		case SPICR2: {
			*((uint8_t *) buffer) = spicr2_register & 0x5B;
		} break;	// 1 byte
		case SPIBR: {
			*((uint8_t *) buffer) = spibr_register & 0x77;
		} break;	// 1 byte
		case SPISR: {
			// Write has no effect. Flags are cleared by reading the SPISR register and doing read/write operation
			spisr_read = true;
			*((uint8_t *) buffer) = spisr_register;
			spisr_register = spisr_register & 0xB0;
		} break; // 1 byte
		case RESERVED1: {
			if (data_length == 2) {
				*((uint16_t *) buffer) = Host2BigEndian(spidr_register);
				if (rx_debug_enabled || tx_debug_enabled) {
					std::cout << sc_time_stamp() << "  " <<  sc_object::name() << "::read offset= " << std::dec << offset << "  size=" << data_length << "  data=0x" << std::hex << (unsigned int) BigEndian2Host(*((uint16_t *)buffer)) << std::dec << std::endl;
				}
			} else {
				*((uint8_t *) buffer) = (uint8_t) (spidr_register >> 8);
				if (rx_debug_enabled || tx_debug_enabled) {
					std::cout << sc_time_stamp() << "  " <<  sc_object::name() << "::read offset= " << std::dec << offset << "  size=" << data_length << "  data=0x" << std::hex << (unsigned int) *((uint8_t*) buffer) << std::dec << std::endl;
				}
			}

			if (isSPISR_Read()) {
				if (isValideFrameWaiting()) {
					spidr_register = spidr_rx_buffer;
					setSPIF();
					validFrameWaiting = false;
				} else {
					// clear SPIF
					spisr_register = spisr_register & 0x7F;
				}
			}

		} break; // 1 byte
		case SPIDR: {
			*((uint8_t *) buffer) = (uint8_t) (spidr_register & 0x00FF);
			if (isSPISR_Read()) {
				if (isValideFrameWaiting()) {
					spidr_register = spidr_rx_buffer;
					setSPIF();
					validFrameWaiting = false;
				} else {
					// clear SPIF
					spisr_register = spisr_register & 0x7F;
				}
			}
		} break; // 1 bytes
		case RESERVED2: {
			*((uint8_t *) buffer) = 0x00;
		} break; // 1 bytes
		case RESERVED3: {
			*((uint8_t *) buffer) = 0x00;
		} break; // 1 bytes
	}

	return (true);
}

bool TLE72XXSL::write(unsigned int offset, const void *buffer, unsigned int data_length) {

	switch (offset) {
		case SPICR1: {
			uint8_t old = spicr1_register;
			spicr1_register = *((uint8_t *) buffer);

			if (isActive() && (((old & 0x1F) ^ (spicr1_register & 0x1F)) != 0)) {
				abortTX();
			}

			if (((old & 0x40) == 0) && isSPIEnabled()) {
				enableSPI();
			}

			if (((old & 0x40) != 0) && !isSPIEnabled()) {
				disableSPI();
			}

			if (isSPISR_Read()) {
				// clear MODF flag
				spisr_register = spisr_register & 0xEF;
			}

			// TODO: I have not understand the meaning and use of SPISCR1::SSOE bit also of SS pin !!!

		} break;	// 1 byte
		case SPICR2: {
			uint8_t old = spicr2_register;
			spicr2_register = *((uint8_t *) buffer) & 0x5B;

			frameLength = (is16bitsMode())? 16: 8;

			if (isActive() && (((old & 0x59) ^ (spicr2_register & 0x59)) != 0)) {
				abortTX();
			}

		} break;	// 1 byte
		case SPIBR:  {
			uint8_t old = spibr_register;
			spibr_register = *((uint8_t *) buffer) & 0x77;
			if ((old & 0x77) ^ (spibr_register & 0x77)) {
				ComputeBaudRate();

				if (isActive()) {
					abortTX();
				}
			}

		} break;	// 1 byte
		case SPISR:  {
			// Write has no effect. Flags are cleared by reading the SPISR register and doing read/write operation
		} break; // 1 byte

		case RESERVED1: {
			if (rx_debug_enabled || tx_debug_enabled) {
				if (data_length == 1) {
					std::cout << sc_time_stamp() << "  " << sc_object::name() << "::write offset= " << std::dec << offset << "  size=" << data_length << "  data=0x" << std::hex << (unsigned int) *((uint8_t*) buffer) << std::dec << std::endl;
				} else {
					std::cout << sc_time_stamp() << "  " << sc_object::name() << "::write offset= " << std::dec << offset << "  size=" << data_length << "  data=0x" << std::hex << (unsigned int) BigEndian2Host(*((uint16_t *)buffer)) << std::dec << std::endl;
				}
			}

			if (!isSPTEF() || isSPISR_Read()) {

				// (SPTEF = 1) has to be read before a write to SPIDR can happen and taken into account
				if (data_length == 2) {
//					if (!is16bitsMode()) {
//						std::cerr << sc_object::name() << "::Warning interface is configured for 1-bytes" << std::endl;
//					}
					spidr_register = BigEndian2Host(*((uint16_t *) buffer));
				} else {
//					if (is16bitsMode()) {
//						std::cerr << sc_object::name() << "::Warning interface is configured for 2-bytes" << std::endl;
//					}
					spidr_register = (spidr_register & 0x00FF) | (((uint16_t)*((uint8_t *) buffer)) << 8);
				}

				tx_load_event.notify();
			}

			if (isSPISR_Read()) {
				// clear SPTEF flag
				spisr_register = spisr_register & 0xDF;
			}

		} break; // 1 byte

		case SPIDR:  {
			if (rx_debug_enabled || tx_debug_enabled) {
				if (data_length == 1) {
					std::cout << sc_object::name() << "::write offset= " << std::dec << offset << "  size=" << data_length << "  data=0x" << std::hex << (unsigned int) *((uint8_t*) buffer) << std::dec << std::endl;
				} else {
					std::cout << sc_object::name() << "::write offset= " << std::dec << offset << "  size=" << data_length << "  data=0x" << std::hex << (unsigned int) BigEndian2Host(*((uint16_t *)buffer)) << std::dec << std::endl;
				}
			}

			if (!isSPTEF() || isSPISR_Read()) {
				// (SPTEF = 1) has to be read before a write to SPIDR can happen and taken into account
				spidr_register = (spidr_register & 0xFF00) | *((uint8_t *) buffer);

				tx_load_event.notify();
			}

			if (isSPISR_Read()) {
				// clear SPTEF flag
				spisr_register = spisr_register & 0xDF;
			}

		} break; // 1 bytes
		case RESERVED2:  break; // 1 bytes
		case RESERVED3:  break; // 1 bytes

	}

	return (true);
}


void TLE72XXSL::assertInterrupt(uint8_t interrupt_offset) {


}

// Master methods

void TLE72XXSL::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

void TLE72XXSL::ComputeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

	ComputeBaudRate();

}


void TLE72XXSL::updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}

void TLE72XXSL::ComputeBaudRate() {

	/**
	 * The baud rate divisor equation is as follows:
           BaudRateDivisor = (SPPR + 1) * 2(SPR + 1)

       The baud rate can be calculated with the following equation:
           Baud Rate = BusClock / BaudRateDivisor
	 */
	uint16_t sppr = (spibr_register >> 4) & 0x07;
	uint16_t spr = spibr_register & 0x07;
	uint16_t baudRateDivisor = (sppr + 1) * pow(2, spr+1);

	spi_baud_rate = bus_cycle_time * baudRateDivisor;

//	telnet_process_input_period = sc_time(1, SC_MS);
	telnet_process_input_period = spi_baud_rate * 8 * 8;

}

//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================


bool TLE72XXSL::BeginSetup() {

	char buf[80];

	sprintf(buf, "%s.SPICR1",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &spicr1_register);

	unisim::kernel::variable::Register<uint8_t> *spicr1_var = new unisim::kernel::variable::Register<uint8_t>("SPICR1", this, spicr1_register, "SPI Control Register 1 (SPICR1)");
	extended_registers_registry.push_back(spicr1_var);
	spicr1_var->setCallBack(this, SPICR1, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SPICR2",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &spicr2_register);

	unisim::kernel::variable::Register<uint8_t> *spicr2_var = new unisim::kernel::variable::Register<uint8_t>("SPICR2", this, spicr2_register, "SPI Control Register 2 (SPICR2)");
	extended_registers_registry.push_back(spicr2_var);
	spicr2_var->setCallBack(this, SPICR2, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SPIBR",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &spibr_register);

	unisim::kernel::variable::Register<uint8_t> *spibr_var = new unisim::kernel::variable::Register<uint8_t>("SPIBR", this, spibr_register, "SPI Baud Rate Register (SPIBR)");
	extended_registers_registry.push_back(spibr_var);
	spibr_var->setCallBack(this, SPIBR, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SPISR",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &spisr_register);

	unisim::kernel::variable::Register<uint8_t> *spisr_var = new unisim::kernel::variable::Register<uint8_t>("SPISR", this, spisr_register, "SPI Status register (SPISR)");
	extended_registers_registry.push_back(spisr_var);
	spisr_var->setCallBack(this, SPISR, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SPIDR",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &spidr_register);

	unisim::kernel::variable::Register<uint16_t> *spidr_var = new unisim::kernel::variable::Register<uint16_t>("SPIDR", this, spidr_register, "SPI Data register (SPIDR)");
	extended_registers_registry.push_back(spidr_var);
	spidr_var->setCallBack(this, SPIDR, &CallBackObject::write, NULL);

	Reset();

	ComputeInternalTime();

	return (true);
}


bool TLE72XXSL::Setup(ServiceExportBase *srv_export) {

	return (true);
}


bool TLE72XXSL::EndSetup() {
	return (true);
}


void TLE72XXSL::OnDisconnect() {
}


void TLE72XXSL::Reset() {

	spicr1_register = 0x04;
	spicr2_register = 0x00;
	spibr_register = 0x00;
	spisr_register = 0x20;
	spidr_register = 0x0000;

	frameLength = 8;

	if(char_io_import)
	{
		char_io_import->ResetCharIO();
	}

}

