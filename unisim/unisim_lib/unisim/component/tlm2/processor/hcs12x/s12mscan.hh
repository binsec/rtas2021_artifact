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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

/*
 * s12sci.hh
 *
 *  Created on: 5 mars 2015
 *      Author: rnouacer
 *
 *  Description: This module implement the S12MSCAN V3
 *
 */



#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_S12MSCAN_HH_
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_S12MSCAN_HH_

#include <inttypes.h>
#include <iostream>
#include <cmath>
#include <map>

#include <systemc>

#include <tlm>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/multi_passthrough_initiator_socket.h"

#include <unisim/kernel/kernel.hh>
#include "unisim/kernel/tlm2/tlm.hh"

#include <unisim/kernel/logger/logger.hh>

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

using unisim::component::tlm2::processor::hcs12x::CAN_Payload;
//using unisim::component::tlm2::processor::hcs12x::CAN_DATATYPE;
using unisim::component::tlm2::processor::hcs12x::UNISIM_CAN_ProtocolTypes;

using unisim::kernel::tlm2::ManagedPayload;
using unisim::kernel::tlm2::PayloadFabric;

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <unsigned int MAX_SIZE>
class CircularIndex {
public:

	CircularIndex() : head(0), tail(0),	length(0) {	}
	~CircularIndex() {}

	int getHead() { return (head); }
	int getTail() { return (tail); }

	bool incHead()  // Get element
	{
		if (isEmpty()) { return (false); }

		length--;
	    head = (head+1) % MAX_SIZE;

	    return (true);
	}

	bool incTail()   // Put element
	{
		if (isFull()) {	return (false);	}

		length++;
	    tail = (tail+1) % MAX_SIZE;

	    return (true);
	}

	bool isEmpty() { return (length == 0); }

	bool isFull() { return (length == MAX_SIZE); }

	int size() { return length;	}

private:
    int head;
    int tail;
    int length;
};

class S12MSCAN :
	public sc_module
	, public CallBackObject
	, public Client<TrapReporting >
	, public Service<Memory<physical_address_t> >
	, public Client<Memory<physical_address_t> >
	, public Service<Registers>
	, virtual public tlm_bw_transport_if<XINT_REQ_ProtocolTypes>
	, virtual public tlm_fw_transport_if<UNISIM_CAN_ProtocolTypes >
	, virtual public tlm_bw_transport_if<UNISIM_CAN_ProtocolTypes >

{
public:

	static const unsigned int MEMORY_MAP_SIZE = 64;


	//=========================================================
	//=                REGISTERS OFFSETS                      =
	//=========================================================

	enum REGS_OFFSET {CANCTL0, CANCTL1, CANBTR0, CANBTR1, CANRFLG, CANRIER, CANTFLG, CANTIER, CANTARQ, CANTAAK,
						CANTBSEL, CANIDAC, RESERVED1, CANMISC, CANRXERR, CANTXERR,
						CANIDAR0, CANIDAR1, CANIDAR2, CANIDAR3,	CANIDMR0, CANIDMR1, CANIDMR2, CANIDMR3,
						CANIDAR4, CANIDAR5, CANIDAR6, CANIDAR7,	CANIDMR4, CANIDMR5, CANIDMR6, CANIDMR7,
						CANRXFG_START=0x20, CANRXFG_END=0x2F, CANTXFG_START=0x30, CANTXFG_END=0x3F};


	ServiceImport<TrapReporting > trap_reporting_import;

	ServiceExport<Memory<physical_address_t> > memory_export;
	ServiceImport<Memory<physical_address_t> > memory_import;
	ServiceExport<Registers> registers_export;

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	tlm_utils::simple_target_socket<S12MSCAN> slave_socket;
	tlm_utils::simple_target_socket<S12MSCAN> bus_clock_socket;

	tlm_target_socket<CAN_BUS_WIDTH, UNISIM_CAN_ProtocolTypes > can_rx_sock;
	tlm_initiator_socket<CAN_BUS_WIDTH, UNISIM_CAN_ProtocolTypes > can_tx_sock;


	S12MSCAN(const sc_module_name& name, Object *parent = 0);
	virtual ~S12MSCAN();

	virtual void Reset();

	void assertInterrupt(uint8_t interrupt_offset);
	void ComputeInternalTime();

	void RunRx();
	void RunTx();
	void RunTimer();
	void InputRX(uint8_t (*anValue)[CAN_MSG_SIZE]);
	void refreshOutput(uint8_t tx_buffer_register[CAN_MSG_SIZE]);

    //================================================================
    //=                    tlm2 Interface                            =
    //================================================================
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	virtual tlm_sync_enum nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);

    virtual void read_write( tlm::tlm_generic_payload& trans, sc_time& delay );

    void updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay);

	// Slave methods
	virtual bool get_direct_mem_ptr( CAN_Payload& payload, tlm_dmi&  dmi_data);
	virtual unsigned int transport_dbg( CAN_Payload& payload);

	virtual tlm_sync_enum nb_transport_fw( CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport( CAN_Payload& payload, sc_core::sc_time& t);
	// Master methods
	virtual tlm_sync_enum nb_transport_bw( CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);

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
	//=        S12MSCAN Registers Interface interface methods             =
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

	peq_with_get<CAN_Payload > rx_payload_queue;

	PayloadFabric<CAN_Payload > payload_fabric;

	PayloadFabric<XINT_Payload> xint_payload_fabric;

	XINT_Payload *xint_payload;

	double	bus_cycle_time_int;
	Parameter<double>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;

	uint32_t	oscillator_clock_value;	// The time unit is PS
	Parameter<uint32_t>	param_oscillator_clock_int;
	sc_time		oscillator_clock;

	sc_time		bit_time;

	sc_event can_enable_event, timer_enable_event, tx_run_event, tx_load_event, tx_break_event, rx_run_event;

	sc_event can_bw_event;

	// S12MSCAN baseAddress  CAN0=0x0140:0x017F  CAN1=0x0180:0x01BF CAN2=0x01C0:0x01FF CAN3=0x0200:0x023F CAN4=0x0280:0x02BF
	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	uint8_t transmit_interrupt_offset;
	Parameter<uint8_t> param_transmit_interrupt_offset;
	uint8_t receive_interrupt_offset;
	Parameter<uint8_t> param_receive_interrupt_offset;
	uint8_t errors_interrupt_offset;
	Parameter<uint8_t> param_errors_interrupt_offset;
	uint8_t wakeup_interrupt_offset;
	Parameter<uint8_t> param_wakeup_interrupt_offset;

	bool	tx_debug_enabled;
	Parameter<bool>	param_tx_debug_enabled;

	bool	rx_debug_enabled;
	Parameter<bool>	param_rx_debug_enabled;

	// Registers map
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	std::vector<unisim::kernel::VariableBase*> extended_registers_registry;

	bool abortTransmission;

	// =============================================
	// =            Registers                      =
	// =============================================

	uint8_t canctl0_register;
	uint8_t canctl1_register;
	uint8_t canbtr0_register;
	uint8_t canbtr1_register;
	uint8_t canrflg_register;
	uint8_t canrier_register;
	uint8_t cantflg_register;
	uint8_t cantier_register;
	uint8_t cantarq_register;
	uint8_t cantaak_register;
	uint8_t cantbsel_register;
	uint8_t canidac_register;
	uint8_t reserved1;
	uint8_t canmisc_register;
	uint8_t canrxerr_register;
	uint8_t cantxerr_register;

	uint8_t canidar_register[8];
	uint8_t canidmr_register[8];

	uint8_t canrxfg_register[5][CAN_MSG_SIZE];
	uint8_t cantxfg_register[3][CAN_MSG_SIZE];
	uint8_t can_hit_indicator[5];

	CircularIndex<5> canrxfg_index;

	struct CANFG {
		uint8_t idrx[4]; // IDR: identifier Register
		uint8_t dsrx[8]; // DSR: Data Segment Register
		uint8_t dlr; // DLR: Data Length Register
		uint8_t tbpr; // tbpr: Transmit Buffer Priority register. Not applicable for receive buffers
		uint8_t tsrh, tsrl;  // tsr: Time stamp register. Read-only for CPU
	} ;

	// the time stamp is clocked by the bit clock rate.
	sc_event timer_start_event;
	uint16_t time_stamp;
	sc_time time_quanta;

	bool hasArbitration;
	bool txStatus;

	inline bool isReceiverEnabled() { return (isCANEnabled() /* TODO: clarify if can receiver is enabled automatically ? */); }
	inline bool isTransmitterEnabled() { return (isCANEnabled()  /* TODO: clarify if can transmitter is enabled automatically ? */); }

	inline bool isAbortTransmission() {

		return (abortTransmission);
	}

	inline void abortTX() {
		// TODO finalize abort transmission

		setIdle();
		abortTransmission = true;
	}

	inline void setTransmission(bool status) { txStatus = status; }
	inline bool isTransmission() { return (txStatus); }

	inline void setArbitrationStatus(bool status) { hasArbitration = status; }
	inline bool isArbitrationStatus() { return (hasArbitration); }

	inline void enable_can() {
		setIdle();

		can_enable_event.notify();
		std::cout << sc_object::name() << ":: Enabled " << std::endl;
	}


	inline bool isInitModeRequest() { return (canctl0_register & 0x01); }
	inline void enterInitMode() {

		canctl0_register = canctl0_register & 0x07;
		canrflg_register = canrflg_register & 0x3C;
		canrier_register = canrier_register & 0x3C;
		cantflg_register = 0x07;
		cantier_register = 0x00;
		cantarq_register = 0x00;
		cantaak_register = 0x00;
		cantbsel_register = 0x00;

		canctl1_register = canctl1_register | 0x01; // Acknowledge Initialization mode
	}

	inline void exitInitMode() {

		canctl1_register = canctl1_register & 0xFE; // clear Acknowledge Initialization mode

		// TODO: add some stuff to check CAN bus synchronization
		/*
		 * When this bit is cleared by the CPU, the MSCAN restarts and then tries to synchronize to the CAN bus.
		 * If the MSCAN is not in bus-off state, it synchronizes after 11 consecutive recessive bits on the CAN bus;
		 * if the MSCAN is in bus-off state, it continues to wait for 128 occurrences of 11 consecutive recessive bits.
		 */

		// set synchronized to CAN bus
		setSynchronized();
	}

	inline bool isInitMode() { return ((canctl0_register & 0x01) && (canctl1_register & 0x01)); }

	inline void setReceiverActive() { canctl0_register = canctl0_register | 0x40; }
	inline bool isReceiverActive() { return ((canctl0_register & 0x40) == 0x40); }
	inline void setIdle() { canctl0_register = canctl0_register & 0xBF; }
	inline bool isIdle() { return ((canctl0_register & 0x40) != 0x40) && (!isTransmission()); }


	inline bool isStopInWait() { return (canctl0_register & 0x20); }

	inline void setSynchronized() { canctl0_register = canctl0_register | 0x10; }
	inline void setNotSynchronized() { canctl0_register = canctl0_register & 0xEF; }
	inline bool isSynchronized() { return (canctl0_register & 0x10); }
	inline bool isTimerActivated() { return (canctl0_register & 0x08); }
	inline void enableTimer() { if (isTimerActivated()) { timer_enable_event.notify(); } }

	inline bool isWakeupEnabled() { return (canctl0_register & 0x04); }
	inline bool isWakeupInterrupt() { return (canrflg_register & 0x80); }

	inline bool isSleepModeRequest() { return (canctl0_register & 0x02); }
	inline void enterSleepMode() { canctl1_register = canctl1_register | 0x02; }
	inline bool isSleepMode() { return (((canctl0_register & 0x02)) && ((canctl1_register & 0x02))); }
	inline void exitSleepMode() { canctl0_register = canctl0_register & 0xFD; canctl1_register = canctl1_register & 0xFD; }

	inline bool isCANEnabled() { return ((canctl1_register & 0x80) == 0x80); }
	inline bool isBusClk() { return ((canctl1_register & 0x40) == 0x40); }
	inline bool isLoopBack() { return ((canctl1_register & 0x20) == 0x20); }
	inline bool isListen() { return ((canctl1_register & 0x10) == 0x10); }
	inline bool isBusOffRecoveryMode() { return ((canctl1_register & 0x08) != 0x08); }
	inline bool isWakeupAny() { return ((canctl1_register & 0x04) != 0x04); }

	inline sc_time getSyncJump() { return time_quanta * (((canbtr0_register & 0xC0) >> 6) + 1); }
	inline int getPrescaler() {	return ((canbtr0_register & 0x3F) + 1);	}
	inline bool isThreeSamplePerBit() { return ((canbtr1_register & 0x80) != 0); }
	inline int getTimeSegment2() { return (((canbtr1_register & 0x70) >> 4) + 1); }
	inline int getTimeSegment1() { return ((canbtr1_register & 0x0F) + 1); }

	inline void setWakeupInterrupt() {

		canrflg_register = (canrflg_register | 0x80);

		if (isWakeupInterruptEnable()) {
			assertInterrupt(wakeup_interrupt_offset);
		}
	}
	inline void setCANStatusChangeInterrupt(bool statusEnableInterrupt) {

		canrflg_register = (canrflg_register | 0x40);

		if (isCANStatusChangeInterruptEnable() && statusEnableInterrupt) {
			assertInterrupt(errors_interrupt_offset);
		}
	}

	enum RECEIVER_STATUS {RxOK=0b00, RxWRN=0b01, RxERR=0b10, RxBusOff=0b11};
	inline void setReceiverStatus(RECEIVER_STATUS status) {

		RECEIVER_STATUS old_status = static_cast<RECEIVER_STATUS>((canrflg_register & 0x30) >> 4);
		canrflg_register = (canrflg_register & 0xCF) | (status << 4);

		bool isRxOK ((old_status == RxOK) ^ (status == RxOK)); /* 0 <= receive error counter <= 96 */
		bool isRxWRN = ((old_status == RxWRN) ^ (status == RxWRN)); /* 96 < receive error counter <= 127 */
		bool isRxERR = ((old_status == RxERR) ^ (status == RxERR)); /* 127 < receive error counter <= 255 */
		bool isBusOff = ((old_status == RxBusOff) ^ (status == RxBusOff)); /* 255 < receive error counter */

		bool statusEnableInterrupt = false;
		switch (getReceiverSensitivityLevel()) {
			case 0b00: /* 00: No Interrupt */ {
				statusEnableInterrupt = false;
			} break;
			case 0b01: /* 01: generate interrupt only if the receiver enters or leaves "bus-off" state */ {
				statusEnableInterrupt = isBusOff;
			} break;
			case 0b10: /* 10: generate interrupt only if the receiver enters or leaves "RxErr" or "bus-off" state */ {
				statusEnableInterrupt = isRxERR || isBusOff;
			} break;
			case 0b11: /* 11: generate interrupt on all state changes (enters or leaves "RxWRN" or "RxErr" or "bus-off" state) */ {
				statusEnableInterrupt = isRxWRN || isRxERR || isBusOff || isRxOK;
			} break;
		}

		setCANStatusChangeInterrupt(statusEnableInterrupt);
	}

	enum TRANSMITTER_STATUS {TxOK=0b00, TxWRN=0b01, TxERR=0b10, TxBusOff=0b11};
	inline void setTransmitterStatus(TRANSMITTER_STATUS status) {

		TRANSMITTER_STATUS old_status = static_cast<TRANSMITTER_STATUS>((canrflg_register & 0x0C) >> 2);
		canrflg_register = (canrflg_register & 0xF3) | (status << 2);

		bool isTxOK = ((old_status == TxOK) ^ (status == TxOK)); /* 0 <= transmit error counter <= 96 */
		bool isTxWRN = ((old_status == TxWRN) ^ (status == TxWRN)); /* 96 < transmit error counter <= 127 */
		bool isTxERR = ((old_status == TxERR) ^ (status == TxERR)); /* 127 < transmit error counter <= 255 */
		bool isBusOff = ((old_status == TxBusOff) ^ (status == TxBusOff)); /* 255 < transmit error counter */

		bool statusEnableInterrupt = false;
		switch (getTransmitterSensitivityLevel()) {
			case 0b00: /* 00: No Interrupt */ {
				statusEnableInterrupt = false;
			} break;
			case 0b01: /* 01: generate interrupt only if the transmitter enters or leaves "bus-off" state */ {
				statusEnableInterrupt = isBusOff;
			} break;
			case 0b10: /* 10: generate interrupt only if the transmitter enters or leaves "TxErr" or "bus-off" state */ {
				statusEnableInterrupt = isTxERR || isBusOff;
			} break;
			case 0b11: /* 11: generate interrupt on all state changes (enters or leaves "TxWRN" or "TxErr" or "bus-off" state) */ {
				statusEnableInterrupt = isTxWRN || isTxERR || isBusOff || isTxOK;
			} break;
		}

		setCANStatusChangeInterrupt(statusEnableInterrupt);
	}

	inline void setOverrunInterrupt() {

		canrflg_register = canrflg_register | 0x02;
		if (isOverrunInterruptEnable()) {
			assertInterrupt(errors_interrupt_offset);
		}
	}

	inline void setReceiverBufferFull() {

		canrflg_register = canrflg_register | 0x01;

		if (rx_debug_enabled) {
			std::cout << sc_time_stamp() << "  " << sc_object::name() << "  RXF is SET " << std::endl;
		}

		if (isReceiverFullInterruptEnable()) {
			if (rx_debug_enabled) {
				std::cout << sc_time_stamp() << "  " << sc_object::name() << "  FullInterrupt " << sc_time_stamp() << std::endl;
			}
			assertInterrupt(receive_interrupt_offset);
		}
	}

	inline bool isWakeupInterruptEnable() { return ((canrier_register & 0x80) != 0); }
	inline bool isCANStatusChangeInterruptEnable() { return ((canrier_register & 0x40) != 0x00); /* A CAN Status Change event causes an error interrupt request.*/}
	inline int getReceiverSensitivityLevel() { return ((canrier_register & 0x30) >> 4); }
	inline int getTransmitterSensitivityLevel() { return ((canrier_register & 0x0C) >> 0x02); }
	inline bool isOverrunInterruptEnable() { return ((canrier_register & 0x02) != 0x00); }
	inline bool isReceiverFullInterruptEnable() { return ((canrier_register & 0x01) != 0x00); }

	inline bool isTransmitBufferEmpty(int index) { return ((cantflg_register & (1 << index)) != 0); }
	inline void setTransmitBufferEmpty(int index) {
		cantier_register = cantier_register | (1 << index);

		// When a TXEx flag is set, the corresponding CANTARQ::ABTRQx bit is cleared
		cantarq_register = cantarq_register & ~(1 << index);

		if (isTransmitInterruptEnable(index)) {
			assertInterrupt(transmit_interrupt_offset);
		}
	}

	inline bool isTransmitInterruptEnable(int index) { return ((cantier_register & (1 << index)) != 0); }

	inline bool isAbortRequest(int index) { return ((cantarq_register & (1 << index)) != 0); }

	inline void setTransmitAbortAcknowledge(int index) { cantaak_register = cantaak_register | (1 << index); }

	inline int selectLoadedTx(uint8_t (&tx_buffer)[16]) {

		int index = -1;

		uint8_t mask = 1;
		uint8_t priority = 0xFF;
		struct CANFG *cantxfg;
		for (int i=0; (i<3); i++) {

			if ((cantflg_register & mask) == 0) {
				if (isAbortRequest(i)) {
					setTXE(i);
					setTransmitAbortAcknowledge(i);
				} else {
					cantxfg = reinterpret_cast<CANFG*>(cantxfg_register[i]);
					if (cantxfg->tbpr < priority) {
						index = i;
						priority = cantxfg->tbpr;
					}
				}

			}

			mask = mask << 1;
		}

		if (index != -1) {
			for (int i=0; i<16; i++) {
				tx_buffer[i] = cantxfg_register[index][i];
			}

			setTXE(index);
		}

		return (index);
	}

	inline void setTXE(int index) {

		uint8_t mask = (1 << index);
		cantflg_register = cantflg_register | mask;
		cantarq_register = cantarq_register & ~mask;
	}

	inline bool isTxSelected() { return ((cantbsel_register & 0x07) != 0); }
	inline int getTxIndex() {

		uint8_t mask = 1;
		for (int i=0; i<3; i++) {
			if ((cantbsel_register & mask) != 0) {
				return i;
			} else {
				mask = mask << 1;
			}
		}

		return -1;
	}

	inline int getIdentifierAcceptanceMode() { return ((canidac_register & 0x30) >> 4); }
	inline void setIdentifierAcceptanceHitIndicator(uint8_t value) { canidac_register = (canidac_register & 0x30) | (value & 0x07); }

	inline void setBusOffStateHold() { canmisc_register = canmisc_register | 0x01; }

	inline void incReceiverErrorCounter() {

		canrxerr_register = (canrxerr_register < 255)? (canrxerr_register+1) : canrxerr_register;

		if ((canrxerr_register >= 0) && (canrxerr_register <= 96)) {
			setReceiverStatus(RxOK); /* 0 <= receive error counter <= 96 */
		}
		else if ((96 < canrxerr_register) && (canrxerr_register <= 127)) {
			setReceiverStatus(RxWRN); /* 96 < receive error counter <= 127 */
		}
		else if ((127 < canrxerr_register) && (canrxerr_register <= 255)) {
			setReceiverStatus(RxERR); /* 127 < receive error counter <= 255 */
		}
		else {
			setReceiverStatus(RxBusOff); /* 255 < receive error counter */
		}

	}

	inline void incTransmitterErrorCounter() {

		cantxerr_register = (cantxerr_register < 255)? (cantxerr_register+1) : cantxerr_register;

		if ((cantxerr_register >= 0) && (cantxerr_register <= 96)) {
			setTransmitterStatus(TxOK); /* 0 <= Transmitter error counter <= 96 */
		}
		else if ((96 < cantxerr_register) && (cantxerr_register <= 127)) {
			setTransmitterStatus(TxWRN); /* 96 < Transmitter error counter <= 127 */
		}
		else if ((127 < cantxerr_register) && (cantxerr_register <= 255)) {
			setTransmitterStatus(TxERR); /* 127 < Transmitter error counter <= 255 */
		}
		else {
			setTransmitterStatus(TxBusOff); /* 255 < Transmitter error counter */
		}

	}

	inline bool setRxBG(uint8_t (&rx_buffer)[16], bool isTransmitter=false) {

		if (rx_debug_enabled) {
			std::cout << sc_object::name() << "::  setRxBG *** " << sc_time_stamp() << std::endl;
		}

		addTimeStamp(rx_buffer);

		if (!isTransmitter) {
			uint8_t hit_index = 0;
			if (checkAcceptance(rx_buffer, hit_index)) {
				if (rx_debug_enabled) {
					std::cout << sc_object::name() << "  Accepted *** " << sc_time_stamp() << std::endl;
				}

				setCanRxFG(rx_buffer, hit_index);
				return (true);
			} else {
				if (rx_debug_enabled) {
					std::cout << sc_object::name() << "  Not Accepted *** " << sc_time_stamp() << std::endl;
				}
			}
		}

		return (false);
	}

	inline bool checkAcceptance(uint8_t rx_buffer[16], uint8_t &hit_index) {

		struct CANFG *canrxfg = reinterpret_cast<CANFG*>(rx_buffer);
		uint8_t mode = (canidac_register & 0x30) >> 4;

		/*
		 *    note: background receive buffer is modeled as rx_buffer_register[16]
		 *
		 *  CANIDAC register: Is used for identifier acceptance control.
		 *    - CANIDAC::IDAM[1:0] (Identifier Acceptance Mode): The CPU sets these flags to define the identifier acceptance filter organization.
		 *      In filter closed mode, no message is accepted such that the foreground buffer is never reloaded.
		 *    - CANIDAC::IDHIT[2:0] (Identifier acceptance Hit Indicator): The MSCAN sets these flags to indicate an identifier acceptance hit.
		 *      The IDHITx indicators are related to the message in the foreground buffer (RxFG). When a message gets shifted into
		 *      the foreground buffer of the receiver FIFO the indicators are update.
		 */

		/*  CANIADR register:
		 *  On reception, each message is written into the background receive buffer. The CPU is notified to
		 *  read the message only if it passes the criteria in the identifier acceptance and identifier mask registers
		 *  (accepted); otherwise, the message is overwritten by the next message (dropped).
		 *  The acceptance registers of the MSCAN are applied on the IDR0–IDR3 registers (see Section 10.3.3.1,
		 *  “Identifier Registers (IDR0–IDR3)”) of incoming messages in a bit by bit manner (see Section 10.4.3,
		 *  “Identifier Acceptance Filter”).
		 *  For extended identifiers, all four acceptance and mask registers are applied. For standard identifiers, only
		 *  the first two (CANIDAR0/1, CANIDMR0/1) are applied.
		 */

		 /*  CANIDMR register:
		 *  The identifier mask register specifies which of the corresponding bits in the identifier acceptance register
		 *  are relevant for acceptance filtering. To receive standard identifiers in 32 bit filter mode, it is required to
		 *  program the last three bits (AM[2:0]) in the mask registers CANIDMR1 and CANIDMR5 to “don’t care.”
		 *  To receive standard identifiers in 16 bit filter mode, it is required to program the last three bits (AM[2:0])
		 *  in the mask registers CANIDMR1, CANIDMR3, CANIDMR5, and CANIDMR7 to “don’t care.”
		 */

//		switch (mode) {
//			case 0: // 32-bits acceptance filters
//			{
//				bool hit0 = true;
//				bool hit1 = true;
//				for (int i=0; i<4; i++) {
//					hit0 = hit0 && (((canrxfg->idrx[i] | canidmr_register[i]) & canidar_register[i]) == canidar_register[i]);
//					hit1 = hit1 && (((canrxfg->idrx[i] | canidmr_register[i+4]) & canidar_register[i+4]) == canidar_register[i+4]);
//				}
//				if (hit0) {
//					hit_index = 0;
//				} else if (hit1) {
//					hit_index = 1;
//				}
//			} break;
//			case 1: // 16-bits acceptance filters
//			{
//				bool hit[4] = {true, true, true, true};
//				for (int i=0; i<2; i++) {
//					hit[0] = hit[0] && (((canrxfg->idrx[i] | canidmr_register[i]) & canidar_register[i]) == canidar_register[i]);
//					hit[1] = hit[1] && (((canrxfg->idrx[i] | canidmr_register[i+2]) & canidar_register[i+2]) == canidar_register[i+2]);
//					hit[2] = hit[2] && (((canrxfg->idrx[i] | canidmr_register[i+4]) & canidar_register[i+4]) == canidar_register[i+4]);
//					hit[3] = hit[3] && (((canrxfg->idrx[i] | canidmr_register[i+6]) & canidar_register[i+6]) == canidar_register[i+6]);
//				}
//				for (int i=0; i<4; i++) {
//					if (hit[i]) {
//						hit_index = i;
//						break;
//					}
//				}
//			} break;
//			case 2: // 8-bits acceptance filters
//			{
//				for (int i=0; i<8; i++) {
//					if (((canrxfg->idrx[0] | canidmr_register[i]) & canidar_register[i]) == canidar_register[i]) {
//						hit_index = i;
//						break;
//					}
//				}
//			} break;
//			case 3: // Filter closed
//			{
//				return (false);
//			} break;
//		}

		switch (mode) {
			case 0: // 32-bits acceptance filters
			{
				bool hit0 = true;
				bool hit1 = true;
				for (int i=0; i<4; i++) {
					hit0 = hit0 && ((canrxfg->idrx[i] & canidmr_register[i]) == canidar_register[i]);
					hit1 = hit1 && ((canrxfg->idrx[i] & canidmr_register[i+4]) == canidar_register[i+4]);
				}
				if (hit0) {
					hit_index = 0;
				} else if (hit1) {
					hit_index = 1;
				}
			} break;
			case 1: // 16-bits acceptance filters
			{
				bool hit[4] = {true, true, true, true};
				for (int i=0; i<2; i++) {
					hit[0] = hit[0] && ((canrxfg->idrx[i] & canidmr_register[i]) == canidar_register[i]);
					hit[1] = hit[1] && ((canrxfg->idrx[i] & canidmr_register[i+2]) == canidar_register[i+2]);
					hit[2] = hit[2] && ((canrxfg->idrx[i] & canidmr_register[i+4]) == canidar_register[i+4]);
					hit[3] = hit[3] && ((canrxfg->idrx[i] & canidmr_register[i+6]) == canidar_register[i+6]);
				}
				for (int i=0; i<4; i++) {
					if (hit[i]) {
						hit_index = i;
						break;
					}
				}
			} break;
			case 2: // 8-bits acceptance filters
			{
				for (int i=0; i<8; i++) {
					if ((canrxfg->idrx[0] & canidmr_register[i]) == canidar_register[i]) {
						hit_index = i;
						break;
					}
				}
			} break;
			case 3: // Filter closed
			{
				return (false);
			} break;
		}

		return (true);
	}

	inline bool setCanRxFG(uint8_t rx_buffer[16], uint8_t hit_index) {

		if (canrxfg_index.isFull()) {
			setOverrunInterrupt();
			return (false);
		}

		unsigned int tail = canrxfg_index.getTail();
		can_hit_indicator[tail] = hit_index;
		for (int i=0; i<16; i++) {
			canrxfg_register[tail][i] = rx_buffer[i];
		}

		setReceiverBufferFull();

		canrxfg_index.incTail();

		return (true);
	}

	inline void addTimeStamp(uint8_t (&tx_buffer)[16]) {

		struct CANFG *cantxfg;
		cantxfg = reinterpret_cast<CANFG*>(tx_buffer);

		cantxfg->tsrh = (time_stamp & 0xFF00) >> 8;
		cantxfg->tsrl = (time_stamp &  0x00FF);

	}

	inline void reset_time_stamp() { time_stamp = 0; }

	unisim::kernel::logger::Logger logger;


}; /* end class S12MSCAN */

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim



#endif /* __UNISIM_COMPONENT_TLM2_PROCESSOR_S12MSCAN_HH_ */
