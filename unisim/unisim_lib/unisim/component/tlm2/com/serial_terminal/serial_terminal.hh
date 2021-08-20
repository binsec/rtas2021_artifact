/*
 *  Copyright (c) 2017,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_TLM2_COM_SERIAL_TERMINAL_SERIAL_TERMINAL_HH__
#define __UNISIM_COMPONENT_TLM2_COM_SERIAL_TERMINAL_SERIAL_TERMINAL_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm_serial.hh>
#include <unisim/kernel/tlm2/clock.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <unisim/service/interfaces/char_io.hh>
#include <queue>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace serial_terminal {

enum ParityType
{
	PARITY_TYPE_NONE = 0,
	PARITY_TYPE_EVEN = 1,
	PARITY_TYPE_ODD  = 2
};

enum BitOrder
{
	LSB,
	MSB
};
	
class SerialTerminal
	: public sc_core::sc_module
	, unisim::kernel::Client<unisim::service::interfaces::CharIO>
{
public:
	enum SerialInterface
	{
		TX_IF,
		RX_IF
	};

	typedef unisim::kernel::tlm2::tlm_serial_peripheral_socket_tagged<SerialTerminal> TX_type;
	typedef unisim::kernel::tlm2::tlm_serial_peripheral_socket_tagged<SerialTerminal> RX_type;

	TX_type TX;
	RX_type RX;
	sc_core::sc_in<bool> CLK;
	
	unisim::kernel::ServiceImport<unisim::service::interfaces::CharIO> char_io_import;
	
	SerialTerminal(const sc_core::sc_module_name& name, unisim::kernel::Object *parent);
	virtual ~SerialTerminal();
	
	virtual bool EndSetup();
	
	virtual void end_of_elaboration();
	
	void nb_receive(int id, unisim::kernel::tlm2::tlm_serial_payload& payload, const sc_core::sc_time& t);

	void PollingProcess();
private:
	unisim::kernel::logger::Logger logger;
	unisim::kernel::tlm2::ClockPropertiesProxy clk_prop_proxy; // proxy to get clock properties from clock port
	sc_core::sc_time clock_period;                 // Master clock period
	sc_core::sc_time clock_start_time;             // Master clock start time
	bool clock_posedge_first;                      // Master clock posedge first ?
	double clock_duty_cycle;                       // Master clock duty cycle
	unisim::kernel::tlm2::tlm_input_bitstream rx_input; // Rx timed input bit stream
	sc_core::sc_time rx_time;
	sc_core::sc_time baud_period_lower_bound;
	sc_core::sc_time baud_period;
	sc_core::sc_time baud_period_upper_bound;
	sc_core::sc_event polling_event;
	sc_core::sc_event tx_event;
	sc_core::sc_time tx_ready_time;
	std::queue<uint8_t> tx_fifo;
	std::queue<uint8_t> rx_fifo;
	sc_core::sc_time polling_period;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_polling_period;
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	ParityType parity_type;
	unisim::kernel::variable::Parameter<ParityType> param_parity_type;
	unsigned int num_stop_bits;
	unisim::kernel::variable::Parameter<unsigned int> param_num_stop_bits;
	BitOrder bit_order;
	unisim::kernel::variable::Parameter<BitOrder> param_bit_order;
	unsigned int num_data_bits;
	unisim::kernel::variable::Parameter<unsigned int> param_num_data_bits;
	double baud_tolerance;
	unisim::kernel::variable::Parameter<double> param_baud_tolerance;
	sc_core::sc_time boot_receive_delay;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_boot_receive_delay;
	
	void ClockPropertiesChangedProcess();
	void TX_Process();
	bool RX_InputStatus();
	void IncrementRxTime();
	void RX_Process();
	void ProcessInput();
	void ProcessOutput();
};

} // end of namespace serial_terminal
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim


#endif
