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
 
#ifndef __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_SIC_SIC_HH__
#define __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_SIC_SIC_HH__

#include <systemc>
#include <tlm>
#include <tlm_utils/passthrough_target_socket.h>
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
// we can reuse the vic_int_source_identifier
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic_int_source_identifier.hh"
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace sic {

class SIC
	: public unisim::kernel::Object
	, public sc_module
	, public unisim::component::tlm2::chipset::arm926ejs_pxp::vic::VICIntSourceIdentifierInterface
{
private:
	/** total number of incomming interrupts */
	static const unsigned int NUM_SOURCE_INT = 31;
	/** total number of passthrough interrupts */
	static const unsigned int NUM_SOURCE_PT_INT = 11;
	/** total number of outgoing interrupts */
	static const unsigned int NUM_TARGET_INT = 11;

public:
	typedef tlm::tlm_base_protocol_types::tlm_payload_type transaction_type;
	typedef tlm::tlm_base_protocol_types::tlm_phase_type   phase_type;
	typedef tlm::tlm_sync_enum                             sync_enum_type;

	sc_core::sc_in<bool> *sicintsource[NUM_SOURCE_INT];
	sc_core::sc_in<bool> *sicptintsource[NUM_SOURCE_PT_INT];

	sc_core::sc_out<bool> *sicinttarget[NUM_TARGET_INT];

	/** Target socket for the bus connection */
	tlm_utils::passthrough_target_socket<SIC, 32>
		bus_target_socket;

	SC_HAS_PROCESS(SIC);
	SIC(const sc_module_name &name, Object *parent = 0);
	~SIC();

	virtual bool BeginSetup();

private:
	/** Registers storage */
	uint8_t regs[0x028UL];

	/** Update the status of the SIC depending on all the possible entries
	 */
	void UpdateStatus();

	/** Value of the source interrupts */
	uint32_t int_source;

	/** Value of the source passthrough interrupts */
	uint32_t ptint_source;

	/** Value of the target interrupts */
	uint32_t int_target;

	/** Source interrupt handling
	 *
	 * @param id the port identifier
	 * @param value the value of the port
	 */
	virtual void VICIntSourceReceived(int id, bool value);
	/** Source identifiers methods for the interrupts */
	unisim::component::tlm2::chipset::arm926ejs_pxp::vic::VICIntSourceIdentifier
		*source_identifier_method[NUM_SOURCE_INT];
	/** Source identifiers methods for the passthrough interrupts */
	unisim::component::tlm2::chipset::arm926ejs_pxp::vic::VICIntSourceIdentifier
		*pt_source_identifier_method[NUM_SOURCE_PT_INT];

	/* value of the output interrupt */
	bool sic_output_value;

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

	static const uint32_t SIC_STATUSAddr        = 0x0000UL;
	static const uint32_t SIC_RAWSTATAddr       = 0x0004UL;
	static const uint32_t SIC_ENABLEAddr        = 0x0008UL;
	static const uint32_t SIC_ENSETAddr         = 0x0008UL;
	static const uint32_t SIC_ENCLRAddr         = 0x000cUL;
	static const uint32_t SIC_SOFTINTSETAddr    = 0x0010UL;
	static const uint32_t SIC_SOFTINTCLRAddr    = 0x0014UL;
	static const uint32_t SIC_PICENABLEAddr     = 0x0020UL;
	static const uint32_t SIC_PICENSETAddr      = 0x0020UL;
	static const uint32_t SIC_PICENCLRAddr      = 0x0024UL;

	/** Gets the given registet address
	 *
	 * @param addr the register address
	 * @return the value of the register
	 */
	uint32_t GetRegister(uint32_t addr) const;
	/** Sets the given register address
	 *
	 * @param addr the reigster address
	 * @param value the value to use
	 */
	void SetRegister(uint32_t addr, uint32_t value);
	
	/** Get the SIC_STATUS register
	 *
	 * @return the value of the register
	 */
	uint32_t GetSIC_STATUS() const;
	/** Set the SIC_STATUS register
	 *
	 * @param value the value to set
	 */
	void SetSIC_STATUS(uint32_t value);

	/** Get the SIC_RAWSTAT register
	 *
	 * @return the value of the register
	 */
	uint32_t GetSIC_RAWSTAT() const;
	/** Set the SIC_RAWSTAT register
	 *
	 * @param value the value to set
	 */
	void SetSIC_RAWSTAT(uint32_t value);

	/** Get the SIC_ENABLE register
	 *
	 * @return the value of the register
	 */
	uint32_t GetSIC_ENABLE() const;
	/** Set the SIC_ENABLE register
	 *
	 * @param value the value to set
	 */
	void SetSIC_ENABLE(uint32_t value);

	/** Get the SIC_ENSET register
	 *
	 * @return the value of the register
	 */
	uint32_t GetSIC_ENSET() const;
	/** Set the SIC_ENSET register
	 *
	 * @param value the value to set
	 */
	void SetSIC_ENSET(uint32_t value);

	/** Get the SIC_ENCLR register
	 *
	 * @return the value of the register
	 */
	uint32_t GetSIC_ENCLR() const;
	/** Set the SIC_ENCLR register
	 *
	 * @param value the value to set
	 */
	void SetSIC_ENCLR(uint32_t value);

	/** Get the SIC_SOFTINTSET register
	 *
	 * @return the value of the register
	 */
	uint32_t GetSIC_SOFTINTSET() const;
	/** Set the SIC_SOFTINTSET register
	 *
	 * @param value the value to set
	 */
	void SetSIC_SOFTINTSET(uint32_t value);

	/** Get the SIC_SOFTINTCLR register
	 *
	 * @return the value of the register
	 */
	uint32_t GetSIC_SOFTINTCLR() const;
	/** Set the SIC_SOFTINTCLR register
	 *
	 * @param value the value to set
	 */
	void SetSIC_SOFTINTCLR(uint32_t value);

	/** Get the SIC_PICENABLE register
	 *
	 * @return the value of the register
	 */
	uint32_t GetSIC_PICENABLE() const;
	/** Set the SIC_PICENABLE register
	 *
	 * @param value the value to set
	 */
	void SetSIC_PICENABLE(uint32_t value);

	/** Get the SIC_PICENSET register
	 *
	 * @return the value of the register
	 */
	uint32_t GetSIC_PICENSET() const;
	/** Set the SIC_PICENSET register
	 *
	 * @param value the value to set
	 */
	void SetSIC_PICENSET(uint32_t value);

	/** Get the SIC_PICENCLR register
	 *
	 * @return the value of the register
	 */
	uint32_t GetSIC_PICENCLR() const;
	/** Set the SIC_PICENCLR register
	 *
	 * @param value the value to set
	 */
	void SetSIC_PICENCLR(uint32_t value);
	
	/**************************************************************************/
	/* Methods to get and set the registers                               END */
	/**************************************************************************/

	/** Base address of the VIC */
	uint32_t base_addr;
	/** UNISIM Parameter for the base address of the VIC */
	unisim::kernel::variable::Parameter<uint32_t> param_base_addr;

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
	static const uint32_t V_ININT     = 0x01UL <<  4;
	static const uint32_t V_OUTINT    = 0x01UL <<  5;
	static const uint32_t V_READ      = 0x01UL <<  6;
	static const uint32_t V_WRITE     = 0x01UL <<  7;
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

} // end of namespace sic
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_SIC_SIC_HH__

