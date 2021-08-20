/*
 *  Copyright (c) 2007,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 *          Reda NOUACER (reda.nouacer@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_HCS12X_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_HCS12X_HH__

#include <systemc>

#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include "unisim/kernel/tlm2/tlm.hh"
#include <unisim/kernel/variable/sc_time/sc_time.hh>

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/hcs12x.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>

#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12xmmc.hh>


#include <inttypes.h>
#include <string>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using namespace sc_core;
using namespace sc_dt;

using unisim::component::cxx::processor::hcs12x::CONFIG;

using unisim::component::cxx::processor::hcs12x::address_t;
using unisim::component::cxx::processor::hcs12x::CPU;
using unisim::component::cxx::processor::hcs12x::MMC_DATA;

using unisim::component::tlm2::processor::hcs12x::S12XMMC;

using unisim::kernel::variable::Parameter;
using unisim::kernel::Object;
using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExportBase;

using unisim::kernel::tlm2::ManagedPayload;
using unisim::kernel::tlm2::PayloadFabric;

using std::string;

class HCS12X :
	public sc_module
	, public CPU
	, virtual public tlm_fw_transport_if< >

{
public:
	typedef CPU inherited;

	// wake-up request from XINT
	tlm_target_socket< > xint_interrupt_request;
	tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm_phase& phase, sc_core::sc_time& t);

	void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t) { }
	bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm_dmi&  dmi_data) { return (false);}
	unsigned int transport_dbg(tlm::tlm_generic_payload& payload) { return (0); }
	tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& payload,	tlm_phase& phase, sc_core::sc_time& t) { return (TLM_ACCEPTED); }

	tlm_utils::simple_target_socket<HCS12X> bus_clock_socket;

	HCS12X(const sc_module_name& name, S12XMMC *_mmc, Object *parent = 0);
	virtual ~HCS12X();

	void Stop(int ret);
	void Sync();

	/* TODO:
	 * Stop All Clocks and puts the device in standby mode.
	 * Asserting the RESET, XIRQ, or IRQ signals ends standby mode.
	 */
	void sleep();

	/* TODO:
	 * Enter a wait state for an integer number of bus clock cycle
	 * Only CPU12 clocks are stopped
	 * Wait for not masked interrupt
	 */
	void wai();

	bool BeginSetup();
	bool Setup(ServiceExportBase *srv_export);
	bool EndSetup();

	void Run();

	void Reset();

	inline void busWrite(MMC_DATA *buffer);
	inline void busRead(MMC_DATA *buffer);

	//================================================================
    //=                    tlm2 Interface                            =
    //================================================================

	/* TODO:
	 * The CPU issues a signal that tells the interrupt module to drive
	 * the vector address of the highest priority pending exception onto the system address bus
	 * (the CPU12 does not provide this address)
	 *
	 * Priority is as follow: reset => sw-interrupt => hw-interrupt => spurious interrupt
	 *
	 * If (RAM_ACC_VIOL | SYS || SWI || TRAP) return IVBR;
	 * Else return INT_Vector
	 */
	address_t getIntVector(uint8_t &ipl);

	void updateCRGClock(tlm::tlm_generic_payload& trans, sc_time& delay);

private:
	void Synchronize();
	void computeInternalTime();

	S12XMMC *mmc;

	sc_event	irq_event,		// I-bit-Maskable Interrupt Requests and X-bit Non-Maskable Interrupt Requests
				reset_event;	// Hardware and Software Reset

	sc_time core_clock_time;
	sc_time cpu_cycle_time;
	sc_time bus_cycle_time;
	sc_time cpu_time;
	sc_time bus_time;
	sc_time last_cpu_time;
	sc_time nice_time;
	sc_time next_nice_time;

	sc_time tlm2_btrans_time;
	sc_time opCyclesArray[32];

	uint64_t core_clock_int;

	Parameter<sc_time> param_nice_time;
	Parameter<uint64_t> param_core_clock;

	bool enable_fine_timing;
	Parameter<bool> param_enable_fine_timing;
	
	// verbose parameters
	bool verbose_tlm_bus_synchronize;
	Parameter<bool> param_verbose_tlm_bus_synchronize;
	bool verbose_tlm_run_thread;
	Parameter<bool> param_verbose_tlm_run_thread;
	bool verbose_tlm_commands;
	Parameter<bool> param_verbose_tlm_commands;

	uint64_t last_instruction_counter;

	PayloadFabric<tlm::tlm_generic_payload> payloadFabric;

	tlm::tlm_generic_payload* xint_trans;
	tlm_phase *xint_phase;

};

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_PROCESSOR_HCS12X_HH__
