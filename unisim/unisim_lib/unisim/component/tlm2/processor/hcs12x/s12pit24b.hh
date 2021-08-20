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
 * This module implement the S12PIT24B 4CV1/8CV2
 */

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_S12PIT24B_HH_
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_S12PIT24B_HH_

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
//#include "tlm_utils/multi_passthrough_target_socket.h"

#include <unisim/kernel/kernel.hh>
#include "unisim/kernel/tlm2/tlm.hh"

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
using unisim::kernel::variable::ParameterArray;
using unisim::kernel::variable::CallBackObject;
using unisim::kernel::VariableBase;

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

template <uint8_t PIT_SIZE>
class S12PIT24B :
	public sc_module
	, public CallBackObject
	, virtual public tlm_bw_transport_if<XINT_REQ_ProtocolTypes>
	, public Client<TrapReporting >
	, public Service<Memory<physical_address_t> >
	, public Service<Registers>
	, public Client<Memory<physical_address_t> >

{
public:
	//=========================================================
	//=                REGISTERS OFFSETS                      =
	//=========================================================

	static const uint8_t PITCFLMT	= 0x00;	// 1 byte
	static const uint8_t PITFLT	= 0x01;	// 1 byte
	static const uint8_t PITCE		= 0x02;	// 1 byte
	static const uint8_t PITMUX	= 0x03;	// 1 byte
	static const uint8_t PITINTE	= 0x04;	// 1 byte
	static const uint8_t PITTF		= 0x05; // 1 byte
	static const uint8_t PITMTLD0	= 0x06; // 1 byte
	static const uint8_t PITMTLD1	= 0x07; // 1 byte
	static const uint8_t PITLD0	= 0x08; // 2 bytes
	static const uint8_t PITCNT0	= 0x0A; // 2 bytes
	static const uint8_t PITLD1	= 0x0C; // 2 bytes
	static const uint8_t PITCNT1	= 0x0E; // 2 bytes
	static const uint8_t PITLD2	= 0x10; // 2 bytes
	static const uint8_t PITCNT2	= 0x12; // 2 bytes
	static const uint8_t PITLD3	= 0x14; // 2 bytes
	static const uint8_t PITCNT3	= 0x16; // 2 bytes
	static const uint8_t PITLD4	= 0x18; // 2 bytes
	static const uint8_t PITCNT4	= 0x1A; // 2 bytes
	static const uint8_t PITLD5	= 0x1C; // 2 bytes
	static const uint8_t PITCNT5	= 0x1E; // 2 bytes
	static const uint8_t PITLD6	= 0x20; // 2 bytes
	static const uint8_t PITCNT6	= 0x22; // 2 bytes
	static const uint8_t PITLD7	= 0x24; // 2 bytes
	static const uint8_t PITCNT7	= 0x26; // 2 bytes

	static const unsigned int REGISTERS_BANK_SIZE = 40;

	ServiceImport<TrapReporting > trap_reporting_import;

	ServiceExport<Memory<physical_address_t> > memory_export;
	ServiceImport<Memory<physical_address_t> > memory_import;
	ServiceExport<Registers> registers_export;

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	tlm_utils::simple_target_socket<S12PIT24B> slave_socket;
	tlm_utils::simple_target_socket<S12PIT24B> bus_clock_socket;

	S12PIT24B(const sc_module_name& name, Object *parent = 0);
	virtual ~S12PIT24B();

	virtual void Reset();

	void assertInterrupt(uint8_t interrupt_offset);
	void setTimeoutFlag(uint8_t index);
	bool isPITEnabled() { return ((pitcflmt_register & 0x80) != 0); }
	void ComputeInternalTime();
	void updateMicroCouter(uint8_t index);

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
	//=             XINT Registers Interface interface methods               =
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

	PayloadFabric<tlm::tlm_generic_payload> payloadFabric;
	XINT_Payload *xint_payload;

	double	bus_cycle_time_int;
	Parameter<double>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;

	sc_time periods[2];

	// MC9S12XDP512V2 - PIT baseAddress
	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	uint8_t interrupt_offset_channel[PIT_SIZE];  // vector offset= 0x5E  ID = 0x2F
	ParameterArray<uint8_t> param_interrupt_offset_channel;

	bool	debug_enabled;
	Parameter<bool>	param_debug_enabled;

	// Registers map
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	std::vector<unisim::kernel::VariableBase*> extended_registers_registry;

	// =============================================
	// =            Registers                      =
	// =============================================

	uint8_t pitcflmt_register;
	uint8_t pitflt_register;
	uint8_t pitce_register;
	uint8_t pitmux_register;
	uint8_t pitinte_register;
	uint8_t pittf_register;
	uint8_t pitmtld0_register;
	uint8_t pitmtld1_register;
	uint16_t pitld_register[PIT_SIZE];
	uint16_t pitcnt_register[PIT_SIZE];

	class CNT16 : public sc_module {
	public:
		CNT16(const sc_module_name& name, S12PIT24B *_parent, uint8_t _index, uint16_t* _counter_register, uint16_t* _load_register);
		~CNT16() { }

		void enable() { isEnabled = true; start_event.notify(); }
		void disable() { isEnabled = false; }
		void load() { counter_register = load_register; }
		void setPeriod(sc_time _period) { period = _period; }
		void process();

	protected:

	private:
		sc_event start_event;
		sc_time period;

		S12PIT24B *parent;
		uint8_t index;
		uint16_t* counter_register;
		uint16_t* load_register;
		bool isEnabled;

	} *counter[PIT_SIZE];

}; /* end class S12PIT24B */

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim



#endif /* __UNISIM_COMPONENT_TLM2_PROCESSOR_S12PIT24B_HH_ */
