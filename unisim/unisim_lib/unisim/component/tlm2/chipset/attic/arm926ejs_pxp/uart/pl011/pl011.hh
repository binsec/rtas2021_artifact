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
 
/** Models the ARM PrimeCell UART (PL011) */

#ifndef __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_UART_PL011_PL011_HH__
#define __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_UART_PL011_PL011_HH__

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
namespace uart {
namespace pl011 {

class PL011
	: public unisim::kernel::Object
	, public sc_module
{
public:
	typedef tlm::tlm_base_protocol_types::tlm_payload_type transaction_type;
	typedef tlm::tlm_base_protocol_types::tlm_phase_type   phase_type;
	typedef tlm::tlm_sync_enum                             sync_enum_type;

	/** Target socket for the bus connection */
	tlm_utils::passthrough_target_socket<PL011, 32>
		bus_target_socket;

	/** Output interrupt ports */
	sc_core::sc_out<bool> uartrxintr;
	sc_core::sc_out<bool> uarttxintr;
	sc_core::sc_out<bool> uartrtintr;
	sc_core::sc_out<bool> uartmsintr;
	sc_core::sc_out<bool> uarteintr;
	sc_core::sc_out<bool> uartintr;

	SC_HAS_PROCESS(PL011);
	PL011(const sc_module_name &name, Object *parent = 0);
	~PL011();

	virtual bool EndSetup();

private:
	/** Registers storage */
	uint8_t regs[4096];

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

	/** Telnet socket for uart input and output */
	int sock;

	/** Base address of the uart */
	uint32_t base_addr;
	/** UNISIM Parameter for the base address of the uart */
	unisim::kernel::variable::Parameter<uint32_t> param_base_addr;

	/** Reference clock (pclock) period in picoseconds */
	uint64_t pclk;
	/** UNISIM Parameter for the reference clock */
	unisim::kernel::variable::Parameter<uint64_t> param_pclk;

	/** External uart clock (timclk) period in picoseconds */
	uint64_t uartclk;
	/** UNISIM Parameter for the external timer module clock */
	unisim::kernel::variable::Parameter<uint64_t> param_uartclk;

	/** Enable logger output for UART messages */
	bool enable_logger;
	/** UNISIM Parameter for the enable logger option */
	unisim::kernel::variable::Parameter<bool> param_enable_logger;

	/** Enable telnet redirection for UART messages */
	bool enable_telnet;
	/** UNISIM Parameter for then enable telnet option */
	unisim::kernel::variable::Parameter<bool> param_enable_telnet;

	/** TCP port used for the telnet socket */
	unsigned int tcp_port;
	/** UNISIM Parameter for the TCP port used for the telnet socket */
	unisim::kernel::variable::Parameter<unsigned int> param_tcp_port;

	bool TransmitChar(uint8_t ch);
	bool TelnetPutChar(uint8_t ch);
	void TelnetPutPacket(std::string packet);

	/** Interface to the UNISIM logger */
	unisim::kernel::logger::Logger logger;
};

} // end of namespace pl011
} // end of namespace uart
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_UART_PL011_HH__

