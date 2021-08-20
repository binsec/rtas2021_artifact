/*
 *  Copyright (c) 2011,
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

#ifndef __UNISIM_COMPONENT_TLM2_GPIO_XILINX_XPS_GPIO_GPIO_SWITCHES_HH__
#define __UNISIM_COMPONENT_TLM2_GPIO_XILINX_XPS_GPIO_GPIO_SWITCHES_HH__

#include <systemc>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <unisim/component/cxx/com/xilinx/xps_gpio/xps_gpio.hh>
#include <unisim/component/tlm2/interrupt/types.hh>
#include <unisim/service/interfaces/keyboard.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace xilinx {
namespace xps_gpio {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::Client;
using unisim::kernel::ServiceImport;
using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::tlm2::Schedule;
using unisim::service::interfaces::Keyboard;

typedef unisim::kernel::tlm2::SimplePayload<bool> GPIOPayload;

typedef unisim::kernel::tlm2::SimpleProtocolTypes<bool> GPIOProtocolTypes;

template <unsigned int NUM_SWITCHES>
class GPIO_Switches
	: public sc_core::sc_module
	, public Client<Keyboard>
{
public:
	static const bool threaded_model = false;
	
	typedef tlm::tlm_initiator_socket<0, GPIOProtocolTypes> gpio_master_socket;
	typedef tlm::tlm_target_socket<0, GPIOProtocolTypes> gpio_slave_socket;
	
	// GPIO input
	gpio_slave_socket *gpio_slave_sock[NUM_SWITCHES];

	// GPIO output
	gpio_master_socket *gpio_master_sock[NUM_SWITCHES];
	
	// Keyboard import
	ServiceImport<Keyboard> keyboard_import;

	GPIO_Switches(const sc_core::sc_module_name& name, Object *parent = 0);
	virtual ~GPIO_Switches();
	
	virtual bool BeginSetup();
	
	// Forward b_transport/nb_transport callbacks for GPIO
	void gpio_b_transport(unsigned int pin, GPIOPayload& trans, sc_core::sc_time& t);
	tlm::tlm_sync_enum gpio_nb_transport_fw(unsigned int pin, GPIOPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	unsigned int gpio_transport_dbg(unsigned int pin, GPIOPayload& payload);
	bool gpio_get_direct_mem_ptr(unsigned int pin, GPIOPayload& payload, tlm::tlm_dmi& dmi_data);

	// Backward nb_transport callbacks for GPIO
	tlm::tlm_sync_enum gpio_nb_transport_bw(unsigned int pin, GPIOPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void gpio_invalidate_direct_mem_ptr(unsigned int pin, sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	void Process();
	
protected:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	
	Parameter<bool> param_verbose;
private:
	sc_core::sc_time polling_period;

	/** The parameter for the cycle time */
	Parameter<sc_core::sc_time> param_polling_period;

	unisim::kernel::tlm2::FwRedirector<GPIO_Switches<NUM_SWITCHES>, GPIOProtocolTypes> *gpio_fw_redirector[NUM_SWITCHES];
	unisim::kernel::tlm2::BwRedirector<GPIO_Switches<NUM_SWITCHES>, GPIOProtocolTypes> *gpio_bw_redirector[NUM_SWITCHES];

	PayloadFabric<GPIOPayload> gpio_payload_fabric;

	bool KeyToPin(uint8_t key_num, unsigned int& pin);
	void Poll();
};

} // end of namespace xps_gpio
} // end of namespace xilinx
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
