/*
 *  Copyright (c) 2012,
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

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_S12XGATE_HH_
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_S12XGATE_HH_

#include <string>

#include <assert.h>

#include <systemc>
#include <tlm>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"

#include <unisim/kernel/kernel.hh>
#include "unisim/kernel/tlm2/tlm.hh"
#include <unisim/kernel/variable/sc_time/sc_time.hh>

#include <unisim/component/cxx/processor/hcs12x/xgate.hh>
#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>

#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12xmmc.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace s12xgate {

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace tlm;
using namespace tlm_utils;

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::EndDebug;

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;

using unisim::kernel::ServiceExportBase;
using unisim::kernel::tlm2::PayloadFabric;

using unisim::component::cxx::processor::hcs12x::address_t;
using unisim::component::cxx::processor::hcs12x::CONFIG;
using unisim::component::cxx::processor::s12xgate::XGATE;
using unisim::component::cxx::processor::hcs12x::MMC_DATA;
using unisim::component::cxx::processor::hcs12x::ADDRESS;
using unisim::component::cxx::processor::hcs12x::MMC;

using unisim::component::tlm2::processor::hcs12x::S12XMMC;
using unisim::component::tlm2::processor::hcs12x::XINT_REQ_ProtocolTypes;
using unisim::component::tlm2::processor::hcs12x::XINT_Payload;
using unisim::component::tlm2::processor::hcs12x::INT_TRANS_T;

class S12XGATE :
	public sc_module
	,public XGATE
	, virtual public tlm_bw_transport_if<XINT_REQ_ProtocolTypes>
	, virtual public tlm_fw_transport_if< >


{
public:
	typedef XGATE inherited;

	tlm_utils::simple_target_socket<S12XGATE> target_socket;
	tlm_utils::simple_target_socket<S12XGATE> bus_clock_socket;

	// wake-up request from XINT
	tlm_target_socket< > xint_interrupt_request;

	tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm_phase& phase, sc_core::sc_time& t);
	void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t) { }
	bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm_dmi&  dmi_data) { return (false);}
	unsigned int transport_dbg(tlm::tlm_generic_payload& payload) { return (0); }
	tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& payload,	tlm_phase& phase, sc_core::sc_time& t) { return (TLM_ACCEPTED); }


	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;
	tlm_sync_enum nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);
	void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) { }


	S12XGATE(const sc_module_name& name, S12XMMC *_mmc, Object *parent = 0);
	virtual ~S12XGATE();

	bool BeginSetup();
	bool Setup(ServiceExportBase *srv_export);
	bool EndSetup();

	void Reset();

	void Run();

	void Stop(int ret);
	void Sync();

	address_t getIntVector(unsigned int& priority);
	void handleAsynchronousInterrupt();

	void assertInterrupt(unsigned int offset, bool isXGATE_flag);

	void enbale_xgate();
	void disable_xgate();
	void triggerChannelThread();
	void riseErrorCondition();


	//================================================================
	//=                    tlm2 Interface                            =
	//================================================================

	void read_write( tlm::tlm_generic_payload& trans, sc_time& delay );
	void updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay);

	void busWrite(MMC_DATA *buffer);
	void busRead(MMC_DATA *buffer);

protected:

private:

	S12XMMC *mmc;

	tlm_quantumkeeper quantumkeeper;

	PayloadFabric<tlm::tlm_generic_payload> xint_target_payload_fabric;
	INT_TRANS_T *xint_buffer;
	tlm::tlm_generic_payload* xint_target_trans;
	tlm_phase *xint_phase;

	PayloadFabric<XINT_Payload> xint_init_payload_fabric;
	XINT_Payload *xint_init_payload;

	sc_event xgate_enable_event;
	sc_event xgate_idle_event;
	sc_event xgate_newthread_event;

	double	bus_cycle_time_int;
	Parameter<double>	param_bus_cycle_time_int;

	sc_time nice_time;
	sc_time next_nice_time;
	Parameter<sc_time> param_nice_time;

	sc_time cpu_cycle_time;
	sc_time bus_cycle_time;
	sc_time cpu_time;
	sc_time bus_time;
	sc_time last_cpu_time;

	sc_time core_clock_time;
	uint64_t core_clock_int;
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

	sc_time tlm2_btrans_time;
	sc_time opCyclesArray[32];

	void Synchronize();
	void computeInternalTime();

	bool asynchronous_interrupt;

	void reqAsynchronousInterrupt()	{ asynchronous_interrupt = true; }
	inline bool hasAsynchronousInterrupt() const { return (asynchronous_interrupt); }
	void ackAsynchronousInterrupt() { asynchronous_interrupt = false; }


};

} // end of namespace s12xgate
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim


#endif /* __UNISIM_COMPONENT_TLM2_PROCESSOR_S12XGATE_HH_ */
