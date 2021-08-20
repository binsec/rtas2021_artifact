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

#ifndef __UNISIM_COMPONENT_TLM2_TIMER_XILINX_XPS_TIMER_CAPTURE_TRIGGER_STUB_HH__
#define __UNISIM_COMPONENT_TLM2_TIMER_XILINX_XPS_TIMER_CAPTURE_TRIGGER_STUB_HH__

#include <systemc>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <unisim/component/tlm2/timer/xilinx/xps_timer/xps_timer.hh>
#include <unisim/util/random/random.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace timer {
namespace xilinx {
namespace xps_timer {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;

class CaptureTriggerStub
	: public Object
	, public sc_core::sc_module
	, tlm::tlm_bw_transport_if<unisim::component::tlm2::timer::xilinx::xps_timer::CaptureTriggerProtocolTypes>
{
public:
	tlm::tlm_initiator_socket<0, unisim::component::tlm2::timer::xilinx::xps_timer::CaptureTriggerProtocolTypes> master_sock;
	
	CaptureTriggerStub(const sc_core::sc_module_name& name, Object *parent = 0);
	
	virtual bool BeginSetup();

	virtual tlm::tlm_sync_enum nb_transport_bw(unisim::component::tlm2::timer::xilinx::xps_timer::CaptureTriggerPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	
	void Process();
private:
	unisim::kernel::logger::Logger logger;
	unisim::util::random::Random random;
	unisim::kernel::tlm2::PayloadFabric<unisim::component::tlm2::timer::xilinx::xps_timer::CaptureTriggerPayload> capture_trigger_payload_fabric;
	sc_core::sc_time cycle_time;
	sc_core::sc_time nice_time;
	sc_core::sc_time time;
	bool verbose;
	bool enable;
	bool randomized_output;
	int32_t random_seed;
	uint32_t random_period_min;
	uint32_t random_period_max;
	Parameter<sc_core::sc_time> param_cycle_time;
	Parameter<sc_core::sc_time> param_nice_time;
	Parameter<bool> param_verbose;
	Parameter<bool> param_enable;
	Parameter<bool> param_randomized_output;
	Parameter<int32_t> param_random_seed;
	Parameter<uint32_t> param_random_period_min;
	Parameter<uint32_t> param_random_period_max;
};

} // end of namespace xps_timer
} // end of namespace xilinx
} // end of namespace timer
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
