/*
 *  Copyright (c) 2008,
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
 * This module implement the S12XMMC_V2 controler
 */

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_S12XMMC_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_S12XMMC_HH__

#include <inttypes.h>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#include <systemc>
#include <tlm>

#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/multi_passthrough_target_socket.h"
#include "tlm_utils/multi_passthrough_initiator_socket.h"

#include "unisim/service/interfaces/trap_reporting.hh"
#include <unisim/kernel/kernel.hh>
#include "unisim/kernel/tlm2/tlm.hh"

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>
#include <unisim/component/cxx/processor/hcs12x/mmc.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using namespace std;

using namespace sc_core;
using namespace sc_dt;
using tlm_utils::simple_initiator_socket;

using unisim::kernel::ServiceImport;
using unisim::kernel::Client;
using unisim::service::interfaces::TrapReporting;
using unisim::kernel::variable::Parameter;

using unisim::component::cxx::processor::hcs12x::ADDRESS;
using unisim::component::cxx::processor::hcs12x::MMC;
using unisim::component::cxx::processor::hcs12x::MMC_DATA;
using unisim::component::cxx::processor::hcs12x::S12MPU_IF;
using unisim::component::cxx::processor::hcs12x::address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::service_address_t;
using unisim::component::cxx::processor::hcs12x::CONFIG;
using unisim::component::cxx::processor::hcs12x::TSemaphore;
using unisim::component::cxx::processor::hcs12x::TOWNER;

using unisim::kernel::Object;
using unisim::kernel::tlm2::PayloadFabric;


class S12XMMC :
	public sc_module,
	public MMC,
	public Client<TrapReporting >

{
public:

	typedef MMC inherited;

	ServiceImport<TrapReporting > trap_reporting_import;
	
	tlm_utils::multi_passthrough_initiator_socket<S12XMMC> init_socket;

	S12XMMC(const sc_module_name& name, S12MPU_IF *_mpu = 0, Object *parent = 0);
	virtual ~S12XMMC();

	void xgate_access(MMC::ACCESS accessType, MMC_DATA *buffer);
	void cpu_access(MMC::ACCESS accessType, MMC_DATA *buffer);

	virtual bool BeginSetup();


private:

	tlm::tlm_generic_payload* mmc_trans;

	sc_time tlm2_btrans_time;
	PayloadFabric<tlm::tlm_generic_payload> payloadFabric;

	TSemaphore busSemaphore;
	sc_event   busSemaphore_event;

	bool accessBus(physical_address_t addr, MMC_DATA *buffer, tlm::tlm_command cmd);

	string memoryMapStr;
	Parameter<string> param_memoryMapStr;

	struct MemoryMapEntry {
		/**
		 * The module_index is necessary because some module have a fragmented address space
		 * Such modules are PIM, MMC and can be accounted for others
		 * This index also cover the case of EEPROM emulation by the flash
		 */
		unsigned int		module_index;
		physical_address_t	start_address;
		physical_address_t	end_address;
	};

	vector<MemoryMapEntry *> memoryMap;

}; /* end class S12XMMC */

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif /*__UNISIM_COMPONENT_TLM2_PROCESSOR_S12XMMC_HH__*/
