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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_PWM_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_PWM_HH__

#include <systemc>

#include <inttypes.h>
#include <iostream>
#include <string>
#include <map>

#include <tlm>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "tlm_utils/simple_target_socket.h"

#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/tlm2/tlm.hh"

#include "unisim/service/interfaces/trap_reporting.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/registers.hh"

#include "unisim/service/interfaces/register.hh"

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>

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
using unisim::service::interfaces::TrapReporting;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::CallBackObject;
using unisim::kernel::variable::SignalArray;

using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;

using unisim::service::interfaces::Register;

using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::CONFIG;

using unisim::kernel::tlm2::PayloadFabric;

using unisim::component::tlm2::processor::hcs12x::UNISIM_PWM_ProtocolTypes;
using unisim::component::tlm2::processor::hcs12x::PWM_Payload;

template <uint8_t PWM_SIZE>
class PWM :
	public sc_module
	, public CallBackObject
	, virtual public tlm_bw_transport_if<UNISIM_PWM_ProtocolTypes<PWM_SIZE> >
	, virtual public tlm_bw_transport_if<XINT_REQ_ProtocolTypes>
	, public Service<Memory<physical_address_t> >
	, public Service<Registers>
	, public Client<Memory<physical_address_t> >
	, public Client<TrapReporting >

{
public:

	//=========================================================
	//=                REGISTERS OFFSETS                      =
	//=========================================================

	enum REGS_OFFSETS {PWME, PWMPOL, PWMCLK, PWMPRCLK, PWMCAE, PWMCTL,
						PWMTST /* Intended for factory test purposes only */,
						PWMPRSC /* Intended for factory test purposes only */,
						PWMSCLA, PWMSCLB,
						PWMSCNTA /* Intended for factory test purposes only */,
						PWMSCNTB /* Intended for factory test purposes only */,
						PWMCNT0, PWMCNT1, PWMCNT2, PWMCNT3, PWMCNT4, PWMCNT5, PWMCNT6, PWMCNT7,
						PWMPER0, PWMPER1, PWMPER2, PWMPER3, PWMPER4, PWMPER5, PWMPER6, PWMPER7,
						PWMDTY0, PWMDTY1, PWMDTY2, PWMDTY3, PWMDTY4, PWMDTY5, PWMDTY6, PWMDTY7, PWMSDN};

	static const unsigned int REGISTERS_BANK_SIZE = 40;

	tlm_initiator_socket<CONFIG::UNISIM2EXTERNAL_BUS_WIDTH, UNISIM_PWM_ProtocolTypes<PWM_SIZE> > master_sock;

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	// interface with bus
	tlm_utils::simple_target_socket<PWM> slave_socket;
	tlm_utils::simple_target_socket<PWM> bus_clock_socket;

	ServiceExport<Memory<physical_address_t> > memory_export;
	ServiceImport<Memory<physical_address_t> > memory_import;
	ServiceExport<Registers> registers_export;
	ServiceImport<TrapReporting > trap_reporting_import;

    PWM(const sc_module_name& name, Object *parent = 0);
    ~PWM();

	virtual void Reset();
	
	void updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay);

//    void refresh_channel(uint8_t channel_number);

    void	start();
    void	pwm7in_ChangeStatus(bool pwm7in_status);
    bool	isEmergencyShutdownEnable();
    void	setPWMInterruptFlag();

    sc_time getClockA();
    sc_time getClockB();
    sc_time getClockSA();
    sc_time getClockSB();

    //================================================================
    //=                    tlm2 Interface                            =
    //================================================================
    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
    virtual tlm_sync_enum nb_transport_bw(PWM_Payload<PWM_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t);

    virtual tlm_sync_enum nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);

	virtual void read_write( tlm::tlm_generic_payload& trans, sc_time& delay );

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================

	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	virtual void OnDisconnect();


	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================

	virtual void ResetMemory();
	virtual bool ReadMemory(physical_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(physical_address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=             Registers Interface interface methods               =
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

	bool	debug_enabled;
	Parameter<bool>	param_debug_enabled;

	SignalArray<bool> channel_output_reg;

protected:
//	void setOutput(uint8_t channel_index, bool value) { assert(channel_index < PWM_SIZE); output[channel_index] = value; };
	void setOutput(uint8_t channel_index, bool value) {
		assert(channel_index < PWM_SIZE);

		if (value != output[channel_index]) {
			channel_output_reg[channel_index] = value;

			refreshOutput(output);

		}

	};

	bool getOutput(uint8_t channel_index) { assert(channel_index < PWM_SIZE); return (output[channel_index]); }

private:
	void ComputeInternalTime();

	tlm_quantumkeeper quantumkeeper;
	PayloadFabric<PWM_Payload<PWM_SIZE> > payload_fabric;

	PayloadFabric<XINT_Payload> xint_payload_fabric;

	XINT_Payload *xint_payload;
	PWM_Payload<PWM_SIZE> *pwm_payload;

	double	bus_cycle_time_int;
	Parameter<double>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;

	sc_event pwm_bw_event;

	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	uint8_t interruptOffset;
	Parameter<uint8_t> param_interruptOffset;

//	bool	debug_enabled;
//	Parameter<bool>	param_debug_enabled;

	// Registers map
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	std::vector<unisim::kernel::VariableBase*> extended_registers_registry;

	sc_time clockVector[8];
	sc_time clockA, clockB, clockSA, clockSB;

	void updateClockAB();
	void updateScaledClockA();
	void updateScaledClockB();

	void refreshOutput(bool pwmValue[PWM_SIZE]);
	void assertInterrupt();

	uint8_t	pwme_register, pwmpol_register, pwmclk_register, pwmprclk_register;
	uint8_t pwmcae_register, pwmctl_register;

	// Reserved Register for factory testing
	uint8_t pwmtst_register, pwmprsc_register, pwmscnta_register, pwmscntb_register;

	uint8_t pwmscla_register, pwmsclb_register;

	uint8_t pwmcnt16_register[PWM_SIZE];
	uint8_t pwmper16_register[PWM_SIZE];
	uint8_t pwmdty16_register_value[PWM_SIZE];

	bool output[PWM_SIZE];

	class Channel_t : public sc_module {
	public:

		Channel_t(const sc_module_name& name, PWM *parent, const uint8_t channel_number, uint8_t *pwmcnt_ptr, uint8_t *pwmper_ptr, uint8_t *pwmdty_ptr);

		void Run();
		void wakeup();
		void disable();

		bool getOutput();
		void setOutput(bool val);
		uint8_t getPwmcnt_register();
		void setPwmcnt_register(uint8_t val);

		uint8_t getPWMPERValue();
		void setPWMPERValue(uint8_t val);
		uint8_t getPWMPERBuffer();
		void setPWMPERBuffer(uint8_t val);

		uint8_t getPWMDTYValue();
		void setPWMDTYValue(uint8_t val);
		uint8_t getPWMDTYBuffer();
		void setPWMDTYBuffer(uint8_t val);


	private:
//		bool output;

		PWM	*pwmParent;
		uint8_t channel_index;

		uint8_t *pwmcnt_register_ptr;
		uint8_t *pwmper_register_value_ptr;
		uint8_t pwmper_register_buffer;
		uint8_t *pwmdty_register_value_ptr;
		uint8_t pwmdty_register_buffer;

		uint8_t channelMask;

		sc_event wakeup_event;

		template <class T> void checkChangeStateAndWait(const sc_time clk);

	} *channel[PWM_SIZE];

	uint8_t pwmsdn_register;

};


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim


#endif /*__UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_PWM_HH__*/

