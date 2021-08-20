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
 
#ifndef __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_VIC_VIC_HH__
#define __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_VIC_VIC_HH__

#include <systemc>
#include <tlm>
#include <tlm_utils/passthrough_target_socket.h>
#include <inttypes.h>
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic_int_source_identifier.hh"
#include "unisim/util/generic_peripheral_register/generic_peripheral_register.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace vic {

class VIC
	: public unisim::kernel::Object
	, public sc_module
	, public VICIntSourceIdentifierInterface
	, public unisim::util::generic_peripheral_register::GenericPeripheralRegisterInterface<uint32_t>
{
private:
	/** total number of incomming interrupts */
	static const unsigned int NUM_SOURCE_INT = 32;
	/** total number of vectored interrupts */
	static const unsigned int NUM_VECT_INT = 16;

public:
	typedef tlm::tlm_base_protocol_types::tlm_payload_type transaction_type;
	typedef tlm::tlm_base_protocol_types::tlm_phase_type   phase_type;
	typedef tlm::tlm_sync_enum                             sync_enum_type;

	sc_core::sc_in<bool> *vicintsource[NUM_SOURCE_INT];
	sc_core::sc_in<bool> nvicfiqin;
	sc_core::sc_in<bool> nvicirqin;
	sc_core::sc_in<uint32_t> vicvectaddrin;

	sc_core::sc_out<bool> nvicfiq;
	sc_core::sc_out<bool> nvicirq;
	sc_core::sc_out<uint32_t> vicvectaddrout;

	/** Target socket for the bus connection */
	tlm_utils::passthrough_target_socket<VIC, 32>
		bus_target_socket;

	SC_HAS_PROCESS(VIC);
	VIC(const sc_module_name &name, Object *parent = 0);
	~VIC();

	virtual bool BeginSetup();

private:
	/** Semaphore to ensure that only one process is modifying the VIC state
	 */
	sc_core::sc_semaphore state_semaphore;

	/** event that handles the update process of the VIC
	 */
	sc_core::sc_event update_status_event;
	/** Status update thread handler
	 * Basically it just waits for update status events and updates the status of
	 *   the VIC depending on all the possible entries by calling the UpdateStatus
	 *   method.
	 */
	void UpdateStatusHandler();
	/** Update the status of the VIC depending on all the possible entries
	 */
	void UpdateStatus();
	/** Is the VIC forwarding the nvicirqin?
	 *
	 * This variable inform just of the VIC status, it is only used for verbose
	 *   requirements.
	 */
	bool forwarding_nvicirqin;
	/** Previous nvicirqin value
	 *
	 * This variable informs just of the VIC status, it is only used for verbose
	 *   requirements.
	 */
	bool nvicirqin_value;

	/** Indicates if a vectored interrupt is being serviced and if so the 
	 *    nesting level
	 */
	unsigned int vect_int_serviced;
	/** If vect_int_serviced is true then this indicates the priority level
	 */
	uint32_t vect_int_serviced_level[NUM_VECT_INT];
	/** Indicates if an interrupt is ready for vectored service
	 */
	bool vect_int_for_service;
	/** Level that would be serviced if requested
	 */ 
	unsigned int max_vect_int_for_service;

	/** Registers storage */
	uint8_t regs[0x01000UL];

	/** Value of the source identifier */
	uint32_t int_source;
	/** Source interrupt handling */
	virtual void VICIntSourceReceived(int id, bool value);
	/** Source identifiers methods */
	VICIntSourceIdentifier *source_identifier_method[NUM_SOURCE_INT];

	/** value of the nVICFIQ */
	bool nvicfiq_value;
	/** value of the nVICIRQ */
	bool nvicirq_value;
	/** value of the VICVECTADDROUT */
	uint32_t vicvectaddrout_value;

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
	/* Methods to get and set the registers                             START */
	/**************************************************************************/

	static const uint32_t VICIRQSTATUSAddr      = 0x0000UL;
	static const uint32_t VICFIQSTATUSAddr      = 0x0004UL;
	static const uint32_t VICRAWINTRAddr        = 0x0008UL;
	static const uint32_t VICINTSELECTAddr      = 0x000cUL;
	static const uint32_t VICINTENABLEAddr      = 0x0010UL;
	static const uint32_t VICINTENCLEARAddr     = 0x0014UL;
	static const uint32_t VICSOFTINTAddr        = 0x0018UL;
	static const uint32_t VICSOFTINTCLEARAddr   = 0x001cUL;
	static const uint32_t VICPROTECTIONAddr     = 0x0020UL;
	static const uint32_t VICVECTADDRAddr       = 0x0030UL;
	static const uint32_t VICDEFVECTADDRAddr    = 0x0034UL;
	static const uint32_t VICVECTADDRBaseAddr   = 0x0100UL;
	static const uint32_t VICVECTCNTLBaseAddr   = 0x0200UL;
	static const uint32_t VICPERIPHIDBaseAddr   = 0x0fe0UL;
	static const uint32_t VICPCELLIDBaseAddr    = 0x0ff0UL;

	static const uint32_t VICITCRAddr           = 0x0300UL;
	static const uint32_t VICITIP1Addr          = 0x0304UL;
	static const uint32_t VICITIP2Addr          = 0x0308UL;
	static const uint32_t VICITOP1Addr          = 0x030cUL;
	static const uint32_t VICITOP2Addr          = 0x0310UL;

	static const uint32_t NUMREGS = 55; // Note: Test registers are not available
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

	/** Returns the enable field of the VICVECTCNTL register.
	 *
	 * @param value the register value
	 * @return true if enable is set, false otherwise
	 */
	bool VECTCNTLEnable(uint32_t value);
	/** Returns the source field of the VICVECTCNTL register
	 *
	 * @param value the register value
	 * @return the value of the source field
	 */
	uint32_t VECTCNTLSource(uint32_t value);

	/** Returns the register pointed by the given address
	 *
	 * @param addr the address to consider
	 * @return the value of the register pointed by the address
	 */
	uint32_t GetRegister(uint32_t addr) const;
	/** Returns the register pointed by the given address plus index * 4
	 *
	 * @param addr the base address to consider
	 * @param index the register index from the base address
	 * @return the value of the register pointed by address + (index * 4)
	 */
	uint32_t GetRegister(uint32_t addr, uint32_t index) const;
	/** Sets the register pointed by the given address
	 *
	 * @param addr the address to consider
	 * @param value the value to set the register
	 */
	void SetRegister(uint32_t addr, uint32_t value);

	/**************************************************************************/
	/* Methods to get and set the registers                               END */
	/**************************************************************************/

	/** Get the source field from the given vector control register value
	 *
	 * @param value the value of the vector control register
	 * @return the source field of the value
	 */
	uint32_t VECTCNTLSource(uint32_t value) const;
	/** Get the enable field from the given vector control register value
	 *
	 * @param value the value of the vector control register
	 * @return the enable field of the value
	 */
	bool VECTCNTLEnable(uint32_t value) const;

	/** Base address of the VIC */
	uint32_t base_addr;
	/** UNISIM Parameter for the base address of the VIC */
	unisim::kernel::variable::Parameter<uint32_t> param_base_addr;

	/** Register helpers to use the UNISIM Register service */
	unisim::util::generic_peripheral_register::GenericPeripheralWordRegister *
		regs_accessor[NUMREGS];
	/** UNISIM Registers for the timer registers */
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
	static const uint32_t V_INIRQ     = 0x01UL <<  4;
	static const uint32_t V_INFIQ     = 0x01UL <<  5;
	static const uint32_t V_INVICIRQ  = 0x01UL <<  6;
	static const uint32_t V_INVICFIQ  = 0x01UL <<  7;
	static const uint32_t V_OUTIRQ    = 0x01UL <<  8;
	static const uint32_t V_OUTFIQ    = 0x01UL <<  9;
	static const uint32_t V_STATUS    = 0x01UL << 12;
	static const uint32_t V_TRANS     = 0x01UL << 16;
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

} // end of namespace vic
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_VIC_VIC_HH__

