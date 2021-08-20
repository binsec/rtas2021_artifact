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
 * S12MPU.hh
 *
 *  Created on: 8 août 2012
 *      Author: rnouacer
 *
 *  Description: This module implement the S12MPU V5
 *
 */



#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_S12MPU_HH_
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_S12MPU_HH_

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
#include <unisim/component/cxx/processor/hcs12x/s12mpu_if.hh>

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

using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::TrapReporting;

using unisim::service::interfaces::Register;
using unisim::util::debug::SimpleRegister;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::Host2BigEndian;

using unisim::component::cxx::processor::hcs12x::S12MPU_IF;
using unisim::component::cxx::processor::hcs12x::ADDRESS;
using unisim::component::cxx::processor::hcs12x::address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::CONFIG;
using unisim::component::cxx::processor::hcs12x::TOWNER;

using unisim::kernel::Object;
using unisim::kernel::tlm2::ManagedPayload;
using unisim::kernel::tlm2::PayloadFabric;

class S12MPU :
	public sc_module
	, public S12MPU_IF
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

	static const uint8_t MPUFLG = 0X00;
	static const uint8_t MPUASTAT0 = 0X01;
	static const uint8_t MPUASTAT1 = 0X02;
	static const uint8_t MPUASTAT2 = 0X03;
	static const uint8_t RESERVED = 0X04;
	static const uint8_t MPUSEL = 0X05;
	static const uint8_t MPUDESC0 = 0X06;
	static const uint8_t MPUDESC1 = 0X07;
	static const uint8_t MPUDESC2 = 0X08;
	static const uint8_t MPUDESC3 = 0X09;
	static const uint8_t MPUDESC4 = 0X0A;
	static const uint8_t MPUDESC5 = 0X0B;

	static const uint8_t MPU_DESC_NUMBER = 8;	// Number of protection descriptors

	static const uint8_t MPU_DESC_BANKS_OFFSET = 0x0C;	// This offset is used to instruments all descriptors
	static const uint8_t MPU_DESC_WINDOW_SIZE = 6;

	static const unsigned int MEMORY_MAP_SIZE = 12;

	ServiceImport<TrapReporting > trap_reporting_import;

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	tlm_utils::simple_target_socket<S12MPU> slave_socket;

	ServiceExport<Memory<physical_address_t> > memory_export;
	ServiceImport<Memory<physical_address_t> > memory_import;
	ServiceExport<Registers> registers_export;

	S12MPU(const sc_module_name& name, Object *parent = 0);
	virtual ~S12MPU();

	virtual void Reset();
	
	virtual bool validate(TOWNER::OWNER who, physical_address_t addr, uint32_t size, bool isWrite, bool isExecute);
	virtual void assertInterrupt();

    //================================================================
    //=                    tlm2 Interface                            =
    //================================================================
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	virtual tlm_sync_enum nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);

    virtual void read_write( tlm::tlm_generic_payload& trans, sc_time& delay );

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================

	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	virtual void OnDisconnect();
	virtual void ResetMemory();


	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================

	virtual bool ReadMemory(physical_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(physical_address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=             S12MPU Registers Interface interface methods               =
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

	// S12MPU baseAddress
	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	uint8_t interrupt_offset;
	Parameter<uint8_t> param_interrupt_offset;

	bool	debug_enabled;
	Parameter<bool>	param_debug_enabled;

	// Registers map
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	std::vector<unisim::kernel::VariableBase*> extended_registers_registry;

	// =============================================
	// =            Registers                      =
	// =============================================

	uint8_t mpuflg_register;	// 1 byte
	uint8_t mpuastat0_register, mpuastat1_register, mpuastat2_register;	// 1*3 byte (22:16, 15:8, 7:0)
	uint8_t reserved;	// 1 byte
	uint8_t mpusel_register;		// 1 byte

	/**
	 * The module address 0x0006-0x000B represent a window in the register map
	 * through which different descriptor registers are visible depending on the selection of MPUSEL register
	 */

	uint8_t mpudesc[MPU_DESC_NUMBER][MPU_DESC_WINDOW_SIZE];

}; /* end class S12MPU */

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim



#endif /* __UNISIM_COMPONENT_TLM2_PROCESSOR_S12MPU_HH_ */
