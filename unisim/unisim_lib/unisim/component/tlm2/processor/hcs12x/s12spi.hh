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
 * s12spi.hh
 *
 *  Created on: 8 août 2012
 *      Author: rnouacer
 *
 *  Description: This module implement the S12SPI V4
 *
 */


#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_S12SPI_HH_
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_S12SPI_HH_

#include <tlm>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/multi_passthrough_initiator_socket.h"

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

#include <systemc>

#include <iostream>
#include <queue>
#include <map>
#include <cmath>
#include <inttypes.h>


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

template <unsigned int SPI_VERSION>
struct S12SPI_CONFIG
{
};

template <>
struct S12SPI_CONFIG<4>
{
	static const bool IS_16BITS = false;
	typedef uint8_t DATA_TYPE;
};

template <>
struct S12SPI_CONFIG<5>
{
	static const bool IS_16BITS = true;
	typedef uint16_t DATA_TYPE;
};

template <unsigned int SPI_VERSION>
class S12SPI :
	public sc_module
	, public CallBackObject
	, virtual public tlm_bw_transport_if<XINT_REQ_ProtocolTypes>
	, public Client<TrapReporting >
	, public Service<Memory<physical_address_t> >
	, public Service<Registers>
	, public Client<Memory<physical_address_t> >
	, public Client<CharIO>

{
public:
	//=========================================================
	//=                REGISTERS OFFSETS                      =
	//=========================================================

	static const uint8_t SPICR1	= 0x00;	// 1 byte
	static const uint8_t SPICR2	= 0x01;	// 1 byte
	static const uint8_t SPIBR		= 0x02;	// 1 byte
	static const uint8_t SPISR		= 0x03; // 1 byte
	static const uint8_t RESERVED1	= 0x04; // 1 byte
	static const uint8_t SPIDR		= 0x05; // 1 bytes
	static const uint8_t RESERVED2	= 0x06; // 1 bytes
	static const uint8_t RESERVED3	= 0x07; // 1 bytes

	static const unsigned int MEMORY_MAP_SIZE = 8;

	enum STATUS {IDLE, ACTIVE};

	ServiceImport<TrapReporting > trap_reporting_import;
	ServiceImport<CharIO > char_io_import;

	ServiceExport<Memory<physical_address_t> > memory_export;
	ServiceImport<Memory<physical_address_t> > memory_import;
	ServiceExport<Registers> registers_export;

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	tlm_utils::simple_target_socket<S12SPI> slave_socket;
	tlm_utils::simple_target_socket<S12SPI> bus_clock_socket;

	tlm_utils::simple_initiator_socket<S12SPI> tx_socket;
	tlm_utils::simple_target_socket<S12SPI> rx_socket;

	S12SPI(const sc_module_name& name, Object *parent = 0);
	virtual ~S12SPI();

	virtual void Reset();

	void assertInterrupt(uint8_t interrupt_offset);
	void ComputeInternalTime();

	void TxRun();
	void RxRun();

    //================================================================
    //=                    tlm2 Interface                            =
    //================================================================
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	virtual tlm_sync_enum nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);

    virtual void read_write( tlm::tlm_generic_payload& trans, sc_time& delay );

    void updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay);

	// target method
//	virtual tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload&, tlm_phase& phase, sc_core::sc_time& t);
//	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm_dmi&  dmi_data);
	virtual void rx_b_transport(tlm::tlm_generic_payload&, sc_core::sc_time& t);

	// master method
//	virtual tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload&, tlm_phase& phase, sc_core::sc_time& t);

//	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);


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
	//=             S12SPI Registers Interface interface methods               =
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

	uint16_t frameLength;

	tlm_quantumkeeper quantumkeeper;
	PayloadFabric<XINT_Payload> xint_payload_fabric;

	XINT_Payload *xint_payload;

	PayloadFabric<tlm::tlm_generic_payload> spi_payload_fabric;

	double	bus_cycle_time_int;
	Parameter<double>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;

	sc_time		spi_baud_rate;
	sc_time		telnet_process_input_period;

	sc_event tx_run_event, rx_run_event, tx_load_event;

	// S12SPI baseAddress  SPI0=0x00D8:0x00DF  SPI1=0x00F0:0x00F7 SPI2=0x00F8:0x00FF
	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	uint8_t interrupt_offset;  // vector offset SPI0=0xD8  SPI1=0xBE SPI2=0xBC
	Parameter<uint8_t> param_interrupt_offset;

	bool	debug_enabled;
	Parameter<bool>	param_debug_enabled;

	bool	tx_debug_enabled;
	Parameter<bool>	param_tx_debug_enabled;
	bool	rx_debug_enabled;
	Parameter<bool>	param_rx_debug_enabled;

	// Registers map
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	std::vector<unisim::kernel::VariableBase*> extended_registers_registry;

	STATUS state;
	bool abortTransmission;
	bool spisr_read;
	bool validFrameWaiting;

	bool mosi;
	unisim::kernel::variable::Signal<bool> mosi_pin;

	bool miso;
	unisim::kernel::variable::Signal<bool> miso_pin;

	bool ss;
	unisim::kernel::variable::Signal<bool> ss_pin;

	bool sck;
	unisim::kernel::variable::Signal<bool> sck_pin;

	// =============================================
	// =            Registers                      =
	// =============================================

	uint8_t spicr1_register;	// 1 byte
	uint8_t spicr2_register;	// 1 byte
	uint8_t spibr_register; // 1 byte
	uint8_t spisr_register; // 1 byte

	typename S12SPI_CONFIG<SPI_VERSION>::DATA_TYPE spidr_register; // 2 bytes

	typename S12SPI_CONFIG<SPI_VERSION>::DATA_TYPE spidr_rx_buffer;
	sc_event rx_buffer_event;

	inline void ComputeBaudRate();

	inline bool isInterruptEnabled() { return ((spicr1_register & 0x80) != 0); }
	inline bool isSPIEnabled() { return ((spicr1_register & 0x40) != 0); }
	inline void enableSPI() {
		setIdle();

		tx_run_event.notify();
		rx_run_event.notify();

	}

	inline void disableSPI() {
		// if SPI is disabled (idle status) then the status bits in SPISR register are reseted
		spisr_register = 0x20;
		setIdle();
		if (isMaster()) {
			abortTX();
		}
	}

	inline bool isTransmitInterruptEnabled() { return ((spicr1_register & 0x20) != 0); }

	inline bool isMaster() { return ((spicr1_register & 0x10) != 0); }

	inline void setSlaveMode() { spicr1_register = spicr1_register & 0xEF; }

	inline bool getSCKIdle() {
		if ((spicr1_register) != 0) {
			return (true);
		} else {
			return (false);
		}
	}

	inline bool isAbortTransmission() {

		return (abortTransmission);
	}

	inline void setIdle() { state = IDLE; }
	inline void setActive() { state = ACTIVE; }
	inline bool isIdle() { return (state == IDLE); }
	inline bool isActive() { return (state == ACTIVE); }

	inline void abortTX() {
		// TODO finalize abort transmission

		if (isMaster()) {
			setIdle();
			abortTransmission = true;
		}
	}

	inline bool isLSBFirst() { return ((spicr1_register & 0x01) != 0); }

	inline bool is16bitsMode() { return S12SPI_CONFIG<SPI_VERSION>::IS_16BITS && ((spicr2_register & 0x40) != 0); }
	inline bool isOutputBufferEnabled() { return ((spicr2_register & 0x08) != 0); }
	inline bool isClkStopInWait() { return ((spicr2_register & 0x02) != 0); }

	inline bool isSPC0Set() { return ((spicr2_register & 0x01) != 0); }

	inline bool isSlaveSelect() { return (((spicr1_register & 0x02) != 0) && ((spicr2_register & 0x10) != 0)); }
	inline bool checkModeFaultError() {
		// is Detecting Mode Fault Error enabled ?
		if (((spicr1_register & 0x02) != 0) || ((spicr2_register & 0x10) == 0)) { return (false); }

		if (isSSLow()) {
			abortTX();
			setSlaveMode();
			setMODF();
			return (true);
		}

		return (false);
	}

	inline bool isDetectingModeFaultError() { return (((spicr1_register & 0x02) == 0) && ((spicr2_register & 0x10) != 0)); }

	inline bool pinRead() {

		bool value;

		if (!(isSPC0Set() ^ isMaster())) {
			value = mosi;
		} else {
			value = miso;
		}

		return (value);
	}

	inline void pinWrite(bool value) {

		if (isMaster()) {
//			mosi = value;
			mosi_pin = value;
		} else {
//			miso = value;
			miso_pin = value;
		}
	}

	inline void setSPIF() {
		spisr_read = false;
		spisr_register = spisr_register | 0x80;
		if (isInterruptEnabled()) {
			assertInterrupt(interrupt_offset);
		}
	}

	inline bool isSPIF() { return ((spisr_register & 0x80) != 0); }

	inline void setSPTEF() {
		spisr_read = false;
		spisr_register = spisr_register | 0x20;
		if (isTransmitInterruptEnabled()) {
			assertInterrupt(interrupt_offset);
		}
	}

	inline bool isSPTEF() { return ((spisr_register & 0x20) != 0); }

	inline void setMODF() {
		spisr_read = false;
		spisr_register = spisr_register | 0x10;

		if (isInterruptEnabled()) {
			assertInterrupt(interrupt_offset);
		}

	}

	inline bool isSPISR_Read() { return (spisr_read); }

	inline bool isValideFrameWaiting() { return (validFrameWaiting); }
	inline void setValideFrameWaiting(bool b) { validFrameWaiting = b; }
	inline void setSPIDR(typename S12SPI_CONFIG<SPI_VERSION>::DATA_TYPE spidr) {
		if (isSPIF()) {
			validFrameWaiting = true;
		} else {
			spidr_register = spidr;
			setSPIF();
		}
	}

	inline void setSSLow(bool val) {  ss = val; }
	inline bool isSSLow() { return (!ss); }

	inline void startTransmission() {
		setActive();
		if (isSlaveSelect()) {
//			ss = false;
			ss_pin = false;
		}
	}
	inline void endTransmission() {
		setIdle();
		if (isSlaveSelect()) {
//			ss = true;
			ss_pin = true;
		}
	}

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
//			if(debug_enabled)
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
					if(debug_enabled)
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


}; /* end class S12SPI */

typedef S12SPI<4> S12SPIV4;
typedef S12SPI<5> S12SPIV5;

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim



#endif /* __UNISIM_COMPONENT_TLM2_PROCESSOR_S12SPI_HH_ */
