/*
 *  Copyright (c) 2008, 2012
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

/*
 * s12sci.hh
 *
 *  Created on: 8 août 2012
 *      Author: rnouacer
 *
 *  Description: This module implement the S12SCI V5
 *
 */



#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_S12SCI_HH_
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_S12SCI_HH_

#include <inttypes.h>
#include <iostream>
#include <cmath>
#include <map>
#include <queue>

#include <systemc>

#include <tlm>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/multi_passthrough_initiator_socket.h"
//#include "tlm_utils/multi_passthrough_target_socket.h"

#include <unisim/kernel/kernel.hh>
#include "unisim/kernel/tlm2/tlm.hh"

#include <unisim/kernel/logger/logger.hh>

#include "unisim/service/interfaces/char_io.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/trap_reporting.hh"

#include "unisim/service/interfaces/register.hh"
#include "unisim/util/debug/simple_register.hh"
#include "unisim/util/endian/endian.hh"

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>

#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>

#include <unisim/util/debug/simple_register_registry.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace tlm;
using namespace tlm_utils;

using unisim::kernel::Object;
using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::CallBackObject;
using unisim::kernel::VariableBase;
using unisim::kernel::variable::Signal;
using unisim::kernel::variable::Variable;

using unisim::service::interfaces::CharIO;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::TrapReporting;

using unisim::service::interfaces::Register;
using unisim::util::debug::SimpleRegister;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::Host2BigEndian;


using unisim::component::cxx::processor::hcs12x::ADDRESS;
using unisim::component::cxx::processor::hcs12x::address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::CONFIG;

using unisim::kernel::Object;
using unisim::kernel::tlm2::ManagedPayload;
using unisim::kernel::tlm2::PayloadFabric;

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;


class S12SCI :
	public sc_module
	, public CallBackObject
	, virtual public tlm_bw_transport_if<XINT_REQ_ProtocolTypes>
	, public Client<TrapReporting >
	, public Client<CharIO>
	, public Service<Memory<physical_address_t> >
	, public Client<Memory<physical_address_t> >
	, public Service<Registers>

{
public:

	enum SCIMSG {SCIDATA, SCIIDLE, SCIBREAK};

	static const unsigned int MEMORY_MAP_SIZE = 8;

	//=========================================================
	//=                REGISTERS OFFSETS                      =
	//=========================================================

	// SCIBDH, SCIBDL, SCICR1 registers are accessible if the AMAP bit in the SCISR2 register is set to zero.
	static const uint8_t SCIBDH	= 0x00;	// 1 byte
	static const uint8_t SCIBDL	= 0x01;	// 1 byte
	static const uint8_t SCICR1	= 0x02;	// 1 byte
	// SCIASR1, SCIACR1, SCIACR2 registers are accessible if the AMAP bit in the SCISR2 register is set to one.
	static const uint8_t SCIASR1	= 0x00;	// 1 byte
	static const uint8_t SCIACR1	= 0x01;	// 1 byte
	static const uint8_t SCIACR2	= 0x02; // 1 byte

	static const uint8_t SCICR2	= 0x03; // 1 byte
	static const uint8_t SCISR1	= 0x04; // 1 byte
	static const uint8_t SCISR2	= 0x05; // 1 bytes
	static const uint8_t SCIDRH	= 0x06; // 1 bytes
	static const uint8_t SCIDRL	= 0x07; // 1 bytes

	static const uint8_t MPU_REG_BANKS_OFFSET = 0x08;	// This offset is used to instruments all banked registers
	static const uint8_t SCIBDH_BANK_OFFSET	= MPU_REG_BANKS_OFFSET + 0x00;	// 1 byte
	static const uint8_t SCIBDL_BANK_OFFSET	= MPU_REG_BANKS_OFFSET + 0x01;	// 1 byte
	static const uint8_t SCICR1_BANK_OFFSET	= MPU_REG_BANKS_OFFSET + 0x02;	// 1 byte
	static const uint8_t SCIASR1_BANK_OFFSET	= MPU_REG_BANKS_OFFSET + 0x03;	// 1 byte
	static const uint8_t SCIACR1_BANK_OFFSET	= MPU_REG_BANKS_OFFSET + 0x04;	// 1 byte
	static const uint8_t SCIACR2_BANK_OFFSET	= MPU_REG_BANKS_OFFSET + 0x05; // 1 byte

	static const uint16_t IDLE_11 = 0x07FF; // preamble of 11 logic 1s
	static const uint16_t IDLE_10 = 0x03FF; // preamble of 10 logic 1s

	ServiceImport<TrapReporting > trap_reporting_import;
	ServiceImport<CharIO > char_io_import;

	ServiceExport<Memory<physical_address_t> > memory_export;
	ServiceImport<Memory<physical_address_t> > memory_import;
	ServiceExport<Registers> registers_export;

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	tlm_utils::simple_target_socket<S12SCI> slave_socket;
	tlm_utils::simple_target_socket<S12SCI> bus_clock_socket;

	S12SCI(const sc_module_name& name, Object *parent = 0);
	virtual ~S12SCI();

	virtual void Reset();
	
	void assertInterrupt(uint8_t interrupt_offset);
	void ComputeInternalTime();

	void RunRx();
	void RunTx();

    //================================================================
    //=                    tlm2 Interface                            =
    //================================================================
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	virtual tlm_sync_enum nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);

    virtual void read_write( tlm::tlm_generic_payload& trans, sc_time& delay );

    void updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay);

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================

	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	virtual void OnDisconnect();

	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================

	virtual void ResetMemory();
	virtual bool ReadMemory(physical_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(physical_address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=             S12SCI Registers Interface interface methods               =
	//=====================================================================

	/**
	 * Gets a register interface to the register specified by name.
	 *
	 * @param name The name of the requested register.
	 * @return A pointer to the RegisterInterface corresponding to name.
	 */
	virtual Register *GetRegister(const char *name);
	virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);

	//=====================================================================
	//=             registers setters and getters                         =
	//=====================================================================
	virtual bool read(unsigned int offset, const void *buffer, unsigned int data_length);
	virtual bool write(unsigned int offset, const void *buffer, unsigned int data_length);


protected:

private:
	tlm_quantumkeeper quantumkeeper;
	PayloadFabric<XINT_Payload> xint_payload_fabric;

	XINT_Payload *xint_payload;

	double	bus_cycle_time_int;
	Parameter<double>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;

	sc_time		sci_baud_rate;
	sc_time		telnet_process_input_period;

	sc_event tx_run_event, tx_load_event, tx_break_event, rx_run_event;

	// S12SCI baseAddress  SCI0=0x00C8:0x00CF  SCI1=0x00D0:0x00D7 SCI2=0x00B8:0x00BF SCI3=0x00C0:0x00C7 SCI4=0x0130:0x0137 SCI5=0x0138:0x013F
	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	uint8_t interrupt_offset;  // vector offset SCI0=0xD6  SCI1=0xD4 SCI2=0x8A SCI3=0x88 SCI4=0x86 SCI5=0x84
	Parameter<uint8_t> param_interrupt_offset;

//	bool	debug_enabled;
//	Parameter<bool>	param_debug_enabled;

	bool	tx_debug_enabled;
	Parameter<bool>	param_tx_debug_enabled;

	bool	rx_debug_enabled;
	Parameter<bool>	param_rx_debug_enabled;

	// Registers map
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	std::vector<unisim::kernel::VariableBase*> extended_registers_registry;

	bool scisr1_read;
	bool idle_to_send;

	bool txd;
	Signal<bool> txd_output_pin;
	bool txd_pin_enable;
	Parameter<bool> param_txd_pin_enable;

	bool rxd;
	Signal<bool> rxd_input_pin;

	inline bool getRXD();

	// =============================================
	// =            Registers                      =
	// =============================================

	// SCIBDH, SCIBDL, SCICR1 registers are accessible if the AMAP bit in the SCISR2 register is set to zero.
	uint8_t scibdh_register;	// 1 byte
	uint8_t scibdl_register;	// 1 byte
	uint8_t scicr1_register;	// 1 byte
	// SCIASR1, SCIACR1, SCIACR2 registers are accessible if the AMAP bit in the SCISR2 register is set to one.
	uint8_t sciasr1_register;	// 1 byte
	uint8_t sciacr1_register;	// 1 byte
	uint8_t sciacr2_register; // 1 byte

	uint8_t scicr2_register; // 1 byte
	uint8_t scisr1_register; // 1 byte
	uint8_t scisr2_register; // 1 bytes
	uint8_t scidrh_register; // 1 bytes
	uint8_t scidrl_register; // 1 bytes

	uint16_t tx_shift_register, rx_shift_register;

	void ComputeBaudRate();
	inline bool isInfraredEnabled() { return ((scibdh_register & 0x80) != 0); }
	inline bool isBaudRateGeneratorEnabled() {
		uint16_t sbr = (!isInfraredEnabled())? (((scibdh_register & 0x1F) << 8) | scibdl_register) : (((scibdh_register & 0x1F) << 7) | (scibdl_register >> 1));
		return (sbr != 0);
	}
	inline bool isLoopOperationEnabled() {
		// receiver pin is disconnected and the transmitter output is connected to receiver input
		return ((scicr1_register & 0x80) != 0);
	}

	inline bool isTx2RxInternal() { return ((scicr1_register & 0x20) == 0); }
	inline bool isSCIStopWaitMode() { return ((scicr1_register & 0x40) != 0); }
	inline bool isNineBitsMode() { /* SCICR1::M ?= 1 */ return ((scicr1_register & 0x10) != 0); }
	inline bool isIdleLineWakeup() { return ((scicr1_register & 0x08) == 0); }
	inline bool isIdleLineTypeStop() { return ((scicr1_register & 0x04) != 0); }
	inline bool isParityEnabled() { return ((scicr1_register & 0x02) != 0); }
	inline bool isOddParity() { return ((scicr1_register & 0x01) != 0); }
	inline void setParityError() { scisr1_read = false; scisr1_register = scisr1_register | 0x01;	}
	inline bool isReceiveInputActiveedgeInterruptEnabled() { return ((sciacr1_register & 0x80) != 0); }
	inline bool isBitErrorInterruptEnabled() { return ((sciacr1_register & 0x20) != 0); }
	inline bool isBreakDetectInterruptEnabled() { return ((sciacr1_register & 0x01) != 0); }
	inline bool isBreakDetectFeatureEnabled() { return ((sciacr2_register & 0x01) != 0); }
	inline bool isTransmitterEnabled() { return (((scicr2_register & 0x08) != 0)  && isBaudRateGeneratorEnabled()); }
	inline bool isReceiverEnabled() { return (((scicr2_register & 0x04) != 0) && isBaudRateGeneratorEnabled()); }
	inline bool isReceiverWakeupEnabled() { return ((scicr2_register & 0x02) != 0); }
	inline void clearReceiverWakeup() { scicr2_register = scicr2_register & 0xFD; }
	inline bool isTransmitBreakCharsEnabled() { return ((scicr2_register & 0x01) != 0); }
	inline bool isSCISR1_Read() { return (scisr1_read); }
	inline bool isBRK13Set() { return ((scisr2_register & 0x04) != 0);}
	inline bool isTXD_Output() { return ((scisr2_register & 0x02) != 0); }
	inline void setTDRE() {
		 scisr1_read = false;
		scisr1_register = scisr1_register | 0x80;
		// is Transmission (TDRE) interrupt enabled ?
		if ((scicr2_register & 0x80) != 0) {
			if (tx_debug_enabled)	std::cout << sc_object::name() << "::setTDRE" << std::endl;
			assertInterrupt(interrupt_offset);
		}
	}
	inline bool isTDRECleared() { return ((scisr1_register & 0x80) == 0); }
	inline void clearTDRE() {
		if (isSCISR1_Read()) {
			// clear SCISR1::TDRE bit
			(scisr1_register = scisr1_register & 0x7F);
		}
	}

	inline bool isSendBreak() { return ((scicr2_register & 0x01) != 0); }
	inline void clearTC() { scisr1_register = scisr1_register & 0xBF; }
	inline void setTC() {
		// SCISR1::TC is set high when the SCISR1::TDRE flag is set and no data, preamble, or break character is being transmitted.
		if (!(isTDRECleared() || isSendBreak())) {
			scisr1_register = scisr1_register | 0x40;
			// Is transmission complete interrupt enabled ?
			if ((scicr2_register & 0x40) != 0) {
				if (tx_debug_enabled)	std::cout << sc_object::name() << "::setTC" << std::endl;
				assertInterrupt(interrupt_offset);
			}
		}
	}
	inline bool isTransmissionComplete() { return ((scisr1_register & 0x40) != 0); }

	inline bool isIDLE() { return (scisr1_register & 0x10); }

	inline void setIDLE() {

		if (isReceiverStandbay()) return;

		scisr1_read = false;
		scisr1_register = scisr1_register | 0x10;
		//is Idle interrupt enabled ?
		if ((scicr2_register & 0x10) != 0) {
			if (rx_debug_enabled)	std::cout << sc_object::name() << "::setIDLE" << std::endl;
			assertInterrupt(interrupt_offset);
		}
	}

	inline bool isReceiverActive() { return ((scisr2_register & 0x01) != 0); }
	inline void setReceiverActive() { scisr2_register = scisr2_register | 0x01; }
	inline void clearReceiverActive() { scisr2_register = scisr2_register & 0xFE; }

	inline bool isIdleCountAfterStop() { return ((scicr1_register & 0x04) != 0); }

	inline uint8_t getBreakLength() {
		uint8_t length = getDataFrameLength();

		if (isBRK13Set()) {
			length = length + 3;
		}

		return (length);
	}

	inline uint8_t getDataFrameLength() {
		if (isNineBitsMode()) {
			return (11);
		} else {
			return (10);
		}
	}

	inline bool isBreakDetectEnabled() { return ((sciacr2_register & 0x01) != 0); }
	inline void setBreakDetectInterrupt() {

		if (isReceiverStandbay()) return;

		sciasr1_register = sciasr1_register | 0x01;
		// is break detect interrupt enabled ?
		if ((sciacr1_register & 0x01) != 0) {
			if (rx_debug_enabled)	std::cout << sc_object::name() << "::setBreakDetect" << std::endl;
			assertInterrupt(interrupt_offset);
		}
	}

	inline void setNoiseFlag() { scisr1_read = false; scisr1_register = scisr1_register | 0x04; }
	inline void setFramingError() { scisr1_read = false; scisr1_register = scisr1_register | 0x02; }
	inline bool isReceiverStandbay() { return ((scicr2_register & 0x02) != 0); }

	inline void setRDRF() {

		if (rx_debug_enabled)	std::cout << sc_object::name() << "::setRDRF received char " << (unsigned int) scidrl_register << std::endl;

		if (isReceiverStandbay()) return;

		scisr1_read = false;
		scisr1_register = scisr1_register | 0x20;
		// is receiver full interrupt enabled ?
		if ((scicr2_register & 0x20) != 0) {
			assertInterrupt(interrupt_offset);
		}
	}
	inline bool isRDRFCleared() { return ((scisr1_register & 0x20) == 0); }

	inline bool isAddressWakeup() { return ((scicr1_register & 0x08) != 0); }

	inline void setOverrunFlag() {

		if (isReceiverStandbay()) return;

		scisr1_read = false;
		scisr1_register = scisr1_register | 0x08;
		// is receiver full interrupt enabled ?
		if ((scicr2_register & 0x20) != 0) {
			if (rx_debug_enabled)	std::cout << sc_object::name() << "::setOverrunFlag" << std::endl;
			assertInterrupt(interrupt_offset);
		}
	}

	inline void txShiftOut(SCIMSG msgType);

	inline uint16_t buildFrame(uint8_t high, uint8_t low, SCIMSG msgType);

	// *** Telnet ***
	bool	telnet_enabled;
	Parameter<bool>	param_telnet_enabled;

	unisim::kernel::logger::Logger logger;

	std::queue<uint8_t> telnet_rx_fifo;
	std::queue<uint8_t> telnet_tx_fifo;
	sc_event telnet_rx_event, telnet_tx_event;

	inline void add(std::queue<uint8_t> &buffer_queue, const uint8_t& data, sc_event &event) {
	    buffer_queue.push(data);
	    event.notify();
	}

	inline void next(std::queue<uint8_t> &buffer_queue, uint8_t& data, sc_event &event) {

		while( isEmpty(buffer_queue))
		{
			wait(event);
		}

		data = buffer_queue.front();
		buffer_queue.pop();

	}

	inline bool isEmpty(std::queue<uint8_t> &buffer_queue) {

	    return (buffer_queue.empty());
	}

	inline size_t size(std::queue<uint8_t> &buffer_queue) {

	    return (buffer_queue.size());
	}

	inline void TelnetSendString(const unsigned char *msg) {

		while (*msg != 0)
			add(telnet_tx_fifo, *msg++, telnet_tx_event) ;

		TelnetProcessOutput(true);
	}

//	inline void TelnetProcessInput()
//	{
//		if(char_io_import)
//		{
//			char c;
//			uint8_t v;
//
//			if(!char_io_import->GetChar(c)) return;
//
//			v = (uint8_t) c;
//			if(rx_debug_enabled)
//			{
//				logger << DebugInfo << "Receiving ";
//				if(v >= 32)
//					logger << "character '" << c << "'";
//				else
//					logger << "control character 0x" << std::hex << (unsigned int) v << std::dec;
//				logger << " from telnet client" << EndDebugInfo;
//			}
//
//			add(telnet_rx_fifo, v, telnet_rx_event);
//
//		} else {
//			logger << DebugInfo << "Telnet not connected to " << sc_object::name() << EndDebugInfo;
//		}
//	}

	void TelnetProcessInput();
	inline void TelnetProcessOutput(bool flush_telnet_output)
	{
		if(char_io_import)
		{
			char c;
			uint8_t v;

			if(!isEmpty(telnet_tx_fifo))
			{
				do
				{
					next(telnet_tx_fifo, v, telnet_tx_event);
					c = (char) v;
					if(tx_debug_enabled)
					{
						logger << DebugInfo << "Sending ";
						if(v >= 32)
							logger << "character '" << c << "'";
						else
							logger << "control character 0x" << std::hex << (unsigned int) v << std::dec;
						logger << " to telnet client" << EndDebugInfo;
					}
					char_io_import->PutChar(c);
				}
				while(!isEmpty(telnet_tx_fifo));

			}

			if(flush_telnet_output)
			{
				char_io_import->FlushChars();
			}
		} else {
			logger << DebugInfo << "Telnet not connected to " << sc_object::name() << EndDebugInfo;
		}
	}


}; /* end class S12SCI */

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim



#endif /* __UNISIM_COMPONENT_TLM2_PROCESSOR_S12SCI_HH_ */
