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
 
#ifndef __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_ETHERNET_SMSC_LAN91C111_HH__
#define __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_ETHERNET_SMSC_LAN91C111_HH__

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
namespace ethernet {

class SMSC_LAN91C111
	: public unisim::kernel::Object
	, public sc_module
{
public:
	typedef tlm::tlm_base_protocol_types::tlm_payload_type transaction_type;
	typedef tlm::tlm_base_protocol_types::tlm_phase_type   phase_type;
	typedef tlm::tlm_sync_enum                             sync_enum_type;

	/** Target socket for the bus connection */
	tlm_utils::passthrough_target_socket<SMSC_LAN91C111, 32>
		bus_target_socket;

	SC_HAS_PROCESS(SMSC_LAN91C111);
	SMSC_LAN91C111(const sc_module_name &name, Object *parent = 0);
	~SMSC_LAN91C111();

	virtual bool BeginSetup();

private:
	/** Registers storage */
	uint8_t regs[65536];

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

	/** Interface to the UNISIM logger */
	unisim::kernel::logger::Logger logger;

	/** Current bank */
	uint8_t cur_bank;
	
	/** Read a byte from the current selected bank
	 * All read accesses are decomposed into read byte accesses to this
	 *   method.
	 *
	 * @param addr the address to access in the bank
	 * @return the readed value
	 */
	uint8_t ReadByte(uint8_t addr);
	/** Write a byte into the current selected bank
	 * All write accesses are decomposed into write byte accesses to this
	 *   method.
	 *
	 * @param addr the address to access in the bank
	 * @param data the data to write
	 */
	void WriteByte(uint8_t addr, uint8_t data);
	/** Read byte into bank 0
	 *
	 * @param addr the address to access in the bank
	 * @return the readed value
	 */
	uint8_t ReadByteBank0(uint8_t addr);
	/** Read byte into bank 1 
	 *
	 * @param addr the address to access in the bank
	 * @return the readed value
	 */
	uint8_t ReadByteBank1(uint8_t addr);
	/** Read byte into bank 2
	 *
	 * @param addr the address to access in the bank
	 * @return the readed value
	 */
	uint8_t ReadByteBank2(uint8_t addr);
	/** Read byte into bank 3
	 *
	 * @param addr the address to access in the bank
	 * @return the readed value
	 */
	uint8_t ReadByteBank3(uint8_t addr);
	/** Write byte into bank 0
	 *
	 * @param addr the address to access in the bank
	 * @param data the data to write
	 */
	void WriteByteBank0(uint8_t addr, uint8_t data);
	/** Write byte into bank 1
	 *
	 * @param addr the address to access in the bank
	 * @param data the data to write
	 */
	void WriteByteBank1(uint8_t addr, uint8_t data);
	/** Write byte into bank 2
	 *
	 * @param addr the address to access in the bank
	 * @param data the data to write
	 */
	void WriteByteBank2(uint8_t addr, uint8_t data);
	/** Write byte into bank 3
	 *
	 * @param addr the address to access in the bank
	 * @param data the data to write
	 */
	void WriteByteBank3(uint8_t addr, uint8_t data);
};

} // end of namespace ethernet
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_ETHERNET_SMSC_LAN91C111_HH__

