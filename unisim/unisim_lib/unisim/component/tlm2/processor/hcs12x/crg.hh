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
 * This module implement the S12CRG_V6 controler
 */

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_CRG_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_CRG_HH__

#include <inttypes.h>
#include <iostream>
#include <cmath>
#include <map>

#include <systemc>

#include <tlm>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/multi_passthrough_initiator_socket.h"
//#include "tlm_utils/multi_passthrough_target_socket.h"

#include <unisim/kernel/kernel.hh>
#include "unisim/kernel/tlm2/tlm.hh"

#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/trap_reporting.hh"

#include "unisim/service/interfaces/register.hh"
#include "unisim/util/debug/simple_register.hh"

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
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::CallBackObject;
using unisim::kernel::VariableBase;

using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::TrapReporting;

using unisim::service::interfaces::Register;
using unisim::util::debug::SimpleRegister;

using unisim::component::cxx::processor::hcs12x::ADDRESS;
using unisim::component::cxx::processor::hcs12x::address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::CONFIG;

using unisim::kernel::Object;
using unisim::kernel::tlm2::ManagedPayload;
using unisim::kernel::tlm2::PayloadFabric;

class CRG :
	public sc_module
	, public CallBackObject
	, virtual public tlm_bw_transport_if<XINT_REQ_ProtocolTypes>
	, public Client<TrapReporting >
	, public Service<Memory<physical_address_t> >
	, public Service<Registers>
	, public Client<Memory<physical_address_t> >

{
public:
	//=========================================================
	//=                REGISTERS OFFSETS                      =
	//=========================================================

	enum REGS_OFFSETS {SYNR, REFDV, CTFLG, CRGFLG, CRGINT, CLKSEL, PLLCTL, RTICTL,
					COPCTL, FORBYP, CTCTL, ARMCOP};

	static const unsigned int MEMORY_MAP_SIZE = 12;

	ServiceImport<TrapReporting > trap_reporting_import;

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	tlm_utils::multi_passthrough_initiator_socket<CRG> bus_clock_socket;

	tlm_utils::simple_target_socket<CRG> slave_socket;

//	tlm_utils::multi_passthrough_target_socket<CRG> reset_socket;

	ServiceExport<Memory<physical_address_t> > memory_export;
	ServiceImport<Memory<physical_address_t> > memory_import;
	ServiceExport<Registers> registers_export;

	CRG(const sc_module_name& name, Object *parent = 0);
	virtual ~CRG();

	virtual void Reset();
	
	void runRTI();
	void runCOP();
	void runClockMonitor();

	void assertInterrupt(unsigned int interrupt_offset);

    //================================================================
    //=                    tlm2 Interface                            =
    //================================================================
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
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
	//=             XINT Registers Interface interface methods               =
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


protected:

private:
	tlm_quantumkeeper quantumkeeper;

	PayloadFabric<XINT_Payload> xint_payload_fabric;
	XINT_Payload *xint_payload;

	PayloadFabric<tlm::tlm_generic_payload> payloadFabric;
	tlm::tlm_generic_payload* bus_clk_trans;

	uint32_t	oscillator_clock_value;	// The time unit is PS
	Parameter<uint32_t>	param_oscillator_clock_int;
	sc_time		oscillator_clock;

	// A number greater equal than 4096 rising OSCCLK edges within a check window is called osc ok.
	sc_time		osc_ok;
	uint32_t		osc_ok_int;
	Parameter<uint32_t> param_osc_ok;
	bool				osc_fail;
	Parameter<bool>	var_osc_fail;

	/**
	 * A time window of 50,000 VCO clock cycles is called check window
	 * VCO clock cycles are generated by the PLL when running at minimum frequency Fscm.
	 */
	sc_time		check_window;
	uint32_t	check_window_int;
	Parameter<uint32_t>	param_check_window;

	double pll_stabilization_delay_value;
	Parameter<double>	param_pll_stabilization_delay;
	sc_time		pll_stabilization_delay;

	uint32_t self_clock_mode_value;
	Parameter<uint32_t> param_self_clock_mode_clock;
	sc_time self_clock_mode_clock;

	sc_time		pll_clock;
	sc_time		bus_clock;

	sc_event	lock_track_detector_event;
	sc_event	track_detector_event;

	sc_event	clockmonitor_enable_event;
	sc_event	rti_enable_event;
	sc_event	cop_enable_event;

	bool armcop_write_enabled;
	bool clock_monitor_enabled; // reset to TRUE; the clock monitor is enabled by default

	bool cop_timeout_reset;
	bool cop_timeout_restart;
	bool copwai_write; // CLKSEL::WOPWAI is write once bit

	bool rti_enabled;
	bool cop_enabled;
	bool scme_write;

	// MC9S12XDP512V2 - CRG baseAddress
	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	unsigned int interrupt_offset_rti;
	Parameter<unsigned int> param_interrupt_offset_rti;

	unsigned int interrupt_offset_pll_lock;
	Parameter<unsigned int> param_interrupt_offset_pll_lock;

	unsigned int interrupt_offset_self_clock_mode;
	Parameter<unsigned int> param_interrupt_offset_self_clock_mode;

	bool	debug_enabled;
	Parameter<bool>	param_debug_enabled;

	// Registers map
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	std::vector<unisim::kernel::VariableBase*> extended_registers_registry;

	// RTI Frequency Divide Rate
	double rti_fdr;

	inline void runPLL_LockTrack_Detector();
	inline void initialize_rti_counter();
	inline void updateBusClock();

	void systemReset();

	void activateSelfClockMode();
	void deactivateSelfClockMode();

	void activateStopMode();
	void activateWaitMode();
	void wakeupFromStopMode();

	// =============================================
	// =            Registers                      =
	// =============================================

	uint8_t synr_register, refdv_register, ctflg_register, crgflg_register, crgint_register,
			clksel_register, pllctl_register, rtictl_register, copctl_register, forbyp_register,
			ctctl_register, armcop_register;

}; /* end class CRG */

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif /*__UNISIM_COMPONENT_TLM2_PROCESSOR_CRG_HH__*/

