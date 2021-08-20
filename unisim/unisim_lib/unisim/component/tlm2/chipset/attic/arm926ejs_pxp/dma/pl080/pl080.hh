/*
 *  Copyright (c) 2011,
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
 
#ifndef __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_DMA_PL080_PL080_HH__
#define __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_DMA_PL080_PL080_HH__

#include <systemc>
#include <tlm>
#include <tlm_utils/passthrough_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <inttypes.h>
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/util/generic_peripheral_register/generic_peripheral_register.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace dma {
namespace pl080 {


class PL080
	: public unisim::kernel::Object
	, public sc_module
	, public unisim::util::generic_peripheral_register::GenericPeripheralRegisterInterface<uint32_t>
{
public:
	typedef tlm::tlm_base_protocol_types::tlm_payload_type transaction_type;
	typedef tlm::tlm_base_protocol_types::tlm_phase_type   phase_type;
	typedef tlm::tlm_sync_enum                             sync_enum_type;

	/** Output error interrupt port */
	sc_core::sc_out<bool> dmacinterr;
	/** Output end of count interrupt port */
	sc_core::sc_out<bool> dmacinttc;
	/** Output combined (error & end of count) interrupt port */
	sc_core::sc_out<bool> dmacintr;

	/** Target socket for the slave bus connection */
	tlm_utils::passthrough_target_socket<PL080, 32>
		bus_target_socket;

	/** Master socket for the master 1 bus connection */
	tlm_utils::simple_initiator_socket<PL080, 32>
		bus_master_1_socket;

	/** Master socket for the master 2 bus connection */
	tlm_utils::simple_initiator_socket<PL080, 32>
		bus_master_2_socket;

	SC_HAS_PROCESS(PL080);
	PL080(const sc_module_name &name, Object *parent = 0);
	~PL080();

	bool BeginSetup();
	bool EndSetup();

private:

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

	/**************************************************************************/
	/* Virtual methods for the master sockets for the bus connection    START */
	/**************************************************************************/

	sync_enum_type bus_master_1_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	sync_enum_type bus_master_2_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void bus_master_1_invalidate_direct_mem_ptr(sc_dt::uint64 addr,
			sc_dt::uint64 size);
	void bus_master_2_invalidate_direct_mem_ptr(sc_dt::uint64 addr,
			sc_dt::uint64 size);

	/**************************************************************************/
	/* Virtual methods for the master sockets for the bus connection      END */
	/**************************************************************************/

	/** Registers storage */
	uint8_t regs[0x01000UL];

	/**************************************************************************/
	/* Methods to get and set the registers                             START */
	/**************************************************************************/

	static const uint32_t 
		DMACIntStatus         = 0x0000UL,
		DMACIntTCStatus       = 0x0004UL,
		DMACIntTCClear        = 0x0008UL,
		DMACIntErrorStatus    = 0x000cUL,
		DMACIntErrClr         = 0x0010UL,
		DMACRawIntTCStatus    = 0x0014UL,
		DMACRawIntErrorStatus = 0x0018UL,
		DMACEnbldChns         = 0x001cUL,
		DMACSoftBReq          = 0x0020UL,
		DMACSoftSReq          = 0x0024UL,
		DMACSoftLBReq         = 0x0028UL,
		DMACSoftLSReq         = 0x002cUL,
		DMACConfiguration     = 0x0030UL,
		DMACSync              = 0x0034UL,
		DMACC0_Base           = 0x0100UL,
		DMACC1_Base           = 0x0120UL,
		DMACC2_Base           = 0x0140UL,
		DMACC3_Base           = 0x0160UL,
		DMACC4_Base           = 0x0180UL,
		DMACC5_Base           = 0x01a0UL,
		DMACC6_Base           = 0x01c0UL,
		DMACC7_Base           = 0x01e0UL,
		DMACCxSrcAddr         = 0x0000UL,
		DMACCxDestAddr        = 0x0004UL,
		DMACCxLLI             = 0x0008UL,
		DMACCxControl         = 0x000cUL,
		DMACCxConfiguration   = 0x0010UL,
		DMACC0SrcAddr         = DMACC0_Base + DMACCxSrcAddr,
		DMACC0DestAddr        = DMACC0_Base + DMACCxDestAddr,
		DMACC0LLI             = DMACC0_Base + DMACCxLLI,
		DMACC0Control         = DMACC0_Base + DMACCxControl,
		DMACC0Configuration   = DMACC0_Base + DMACCxConfiguration,
		DMACC1SrcAddr         = DMACC1_Base + DMACCxSrcAddr,
		DMACC1DestAddr        = DMACC1_Base + DMACCxDestAddr,
		DMACC1LLI             = DMACC1_Base + DMACCxLLI,
		DMACC1Control         = DMACC1_Base + DMACCxControl,
		DMACC1Configuration   = DMACC1_Base + DMACCxConfiguration,
		DMACC2SrcAddr         = DMACC2_Base + DMACCxSrcAddr,
		DMACC2DestAddr        = DMACC2_Base + DMACCxDestAddr,
		DMACC2LLI             = DMACC2_Base + DMACCxLLI,
		DMACC2Control         = DMACC2_Base + DMACCxControl,
		DMACC2Configuration   = DMACC2_Base + DMACCxConfiguration,
		DMACC3SrcAddr         = DMACC3_Base + DMACCxSrcAddr,
		DMACC3DestAddr        = DMACC3_Base + DMACCxDestAddr,
		DMACC3LLI             = DMACC3_Base + DMACCxLLI,
		DMACC3Control         = DMACC3_Base + DMACCxControl,
		DMACC3Configuration   = DMACC3_Base + DMACCxConfiguration,
		DMACC4SrcAddr         = DMACC4_Base + DMACCxSrcAddr,
		DMACC4DestAddr        = DMACC4_Base + DMACCxDestAddr,
		DMACC4LLI             = DMACC4_Base + DMACCxLLI,
		DMACC4Control         = DMACC4_Base + DMACCxControl,
		DMACC4Configuration   = DMACC4_Base + DMACCxConfiguration,
		DMACC5SrcAddr         = DMACC5_Base + DMACCxSrcAddr,
		DMACC5DestAddr        = DMACC5_Base + DMACCxDestAddr,
		DMACC5LLI             = DMACC5_Base + DMACCxLLI,
		DMACC5Control         = DMACC5_Base + DMACCxControl,
		DMACC5Configuration   = DMACC5_Base + DMACCxConfiguration,
		DMACC6SrcAddr         = DMACC6_Base + DMACCxSrcAddr,
		DMACC6DestAddr        = DMACC6_Base + DMACCxDestAddr,
		DMACC6LLI             = DMACC6_Base + DMACCxLLI,
		DMACC6Control         = DMACC6_Base + DMACCxControl,
		DMACC6Configuration   = DMACC6_Base + DMACCxConfiguration,
		DMACC7SrcAddr         = DMACC7_Base + DMACCxSrcAddr,
		DMACC7DestAddr        = DMACC7_Base + DMACCxDestAddr,
		DMACC7LLI             = DMACC7_Base + DMACCxLLI,
		DMACC7Control         = DMACC7_Base + DMACCxControl,
		DMACC7Configuration   = DMACC7_Base + DMACCxConfiguration,
		DMACPeriphID0         = 0x0fe0UL,
		DMACPeriphID1         = 0x0fe4UL,
		DMACPeriphID2         = 0x0fe8UL,
		DMACPeriphID3         = 0x0fecUL,
		DMACPCellID0          = 0x0ff0UL,
		DMACPCellID1          = 0x0ff4UL,
		DMACPCellID2          = 0x0ff8UL,
		DMACPCellID3          = 0x0ffcUL;

	static const uint32_t NUMREGS = 62; // Note: Test registers are not available
	static const uint32_t REGS_ADDR_ARRAY[NUMREGS];
	static const char *REGS_NAME_ARRAY[NUMREGS];

	/** Get interface for the generic peripheral register interface
	 *
	 * @param addr the address to consider
	 * @return the value of the register pointed by the address
	 */
	virtual uint32_t GetPeripheralRegister(uint64_t addr);
	/** Set interface for the generic peripheral register interface
	 *
	 * @param addr the address to consider
	 * @param value the value to set the register to
	 */
	virtual void SetPeripheralRegister(uint64_t addr, uint32_t value);

	/** Returns the register pointed by the given address
	 *
	 * @param addr the address to consider
	 * @return the value of the register pointed by the address
	 */
	uint32_t GetRegister(uint32_t addr) const;
	/** Sets the register pointed by the given address
	 *
	 * @param addr the address to consider
	 * @param value the value to set the register
	 */
	void SetRegister(uint32_t addr, uint32_t value);

	/**************************************************************************/
	/* Methods to get and set the registers                               END */
	/**************************************************************************/
	
	/** Base address of the dma */
	uint32_t base_addr;
	/** UNISIM Parameter for the base address of the dma */
	unisim::kernel::variable::Parameter<uint32_t> param_base_addr;

	/** Register helpers to use the UNISIM Register service */
	unisim::util::generic_peripheral_register::GenericPeripheralWordRegister *
		regs_accessor[NUMREGS];
	/** UNISIM Registers for the dma registers */
	unisim::kernel::variable::Register<
		unisim::util::generic_peripheral_register::GenericPeripheralWordRegister
		> *
		regs_service[NUMREGS];

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
	static const uint32_t V_STATUS    = 0x01UL <<  4;
	static const uint32_t V_OUTIRQ    = 0x02UL <<  5;
	static const uint32_t V_TRANS     = 0x03UL <<  6;
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

} // end of namespace pl080
} // end of namespace dma
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_VIC_VIC_HH__

