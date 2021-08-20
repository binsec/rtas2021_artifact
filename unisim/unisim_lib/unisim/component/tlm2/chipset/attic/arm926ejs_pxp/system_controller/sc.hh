/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_SYSTEM_CONTROLLER_SC_HH__
#define __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_SYSTEM_CONTROLLER_SC_HH__

#include <systemc>
#include <tlm>
#include <tlm_utils/passthrough_target_socket.h>
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace system_controller {

class SystemController
	: public unisim::kernel::Object
	, public sc_module
{
public:
	typedef tlm::tlm_base_protocol_types::tlm_payload_type transaction_type;
	typedef tlm::tlm_base_protocol_types::tlm_phase_type   phase_type;
	typedef tlm::tlm_sync_enum                             sync_enum_type;

	/** Source Reference Clock port */
	sc_out<uint64_t> refclk_out_port;
	/** Source Timer Module Clock Enable 0 port */
	sc_out<uint64_t> timclken0_out_port;
	/** Source Timer Module Clock Enable 1 port */
	sc_out<uint64_t> timclken1_out_port;
	/** Source Timer Module Clock Enable 2 port */
	sc_out<uint64_t> timclken2_out_port;
	/** Source Timer Module Clock Enable 3 port */
	sc_out<uint64_t> timclken3_out_port;

	/** Target socket for the bus connection */
	tlm_utils::passthrough_target_socket<SystemController, 32>
		bus_target_socket;

	SC_HAS_PROCESS(SystemController);
	SystemController(const sc_module_name &name, Object *parent = 0);
	~SystemController();

	virtual bool BeginSetup();

private:
	/** Registers storage */
	uint8_t regs[256];

	/**************************************************************************/
	/* Virtual methods for the target socket for the bus connection     START */
	/**************************************************************************/

	sync_enum_type bus_target_nb_transport_fw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void bus_target_b_transport(transaction_type &trans, 
			sc_core::sc_time &delay);
	bool bus_target_get_direct_mem_ptr(transaction_type &trans, 
			tlm::tlm_dmi &dmi_data);
	unsigned int bus_target_transport_dbg(transaction_type &trans);

	/**************************************************************************/
	/* Virtual methods for the target socket for the bus connection       END */
	/**************************************************************************/

	/** Base address of the system controller */
	uint32_t base_addr;
	/** UNISIM Parameter for the base address of the system controller */
	unisim::kernel::variable::Parameter<uint32_t> param_base_addr;

	/** Reference clock (refclock) period in picoseconds */
	uint64_t refclk;
	/** UNISIM Parameter for the reference clock */
	unisim::kernel::variable::Parameter<uint64_t> param_refclk;

	/** External timer module clock (timclk) period */
	uint64_t timclk;
	/**UNISIM Parameter for the external timer module clock */
	unisim::kernel::variable::Parameter<uint64_t> param_timclk;

	/** Verbose */
	uint32_t verbose;
	/** UNISIM Paramter for verbose */
	unisim::kernel::variable::Parameter<uint32_t> param_verbose;
	/** Verbose levels */
	static const uint32_t V0 = 0x01UL;
	static const uint32_t V1 = 0x03UL;
	static const uint32_t V2 = 0x07UL;
	static const uint32_t V3 = 0x0fUL;
	/** Verbose target mask */
	static const uint32_t V_READ      = 0x01UL <<  4;
	static const uint32_t V_WRITE     = 0x01UL <<  5;
	static const uint32_t V_STATUS    = 0x01UL <<  6;
	/** Check if we should verbose */
	bool VERBOSE(uint32_t level, uint32_t mask) const
	{
		uint32_t ok_level = level & verbose;
		uint32_t ok_mask = (~verbose) & mask; 
		return ok_level && ok_mask;
	};

	/** Interface to the UNISIM logger */
	unisim::kernel::logger::Logger logger;
};

} // end of namespace system_controller
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_SYSTEM_CONTROLLER_SC_HH__

