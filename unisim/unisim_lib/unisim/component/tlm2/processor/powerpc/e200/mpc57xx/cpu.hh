/*
 *  Copyright (c) 2007-2010,
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

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_E200_MPC57XX_CPU_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_E200_MPC57XX_CPU_HH__

#include <systemc>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/tlm2/clock.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <unisim/component/cxx/processor/powerpc/e200/mpc57xx/cpu.hh>
#include <inttypes.h>
#include <stack>
#include <vector>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace powerpc {
namespace e200 {
namespace mpc57xx {

using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::Object;
using unisim::kernel::Client;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Statistic;
using unisim::kernel::variable::StatisticFormula;
using unisim::kernel::logger::Logger;

template <typename TYPES, typename CONFIG>
class CPU
	: public sc_core::sc_module
	, public CONFIG::CPU
	, public tlm::tlm_bw_transport_if<>
	, public tlm::tlm_fw_transport_if<>
{
public:
	typedef typename TYPES::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	typedef typename TYPES::STORAGE_ATTR STORAGE_ATTR;
	
	typedef typename CONFIG::CPU Super;
	typedef typename Super::MachineCheckInterrupt MachineCheckInterrupt;
	typedef typename Super::InstructionStorageInterrupt InstructionStorageInterrupt;
	typedef typename Super::DataStorageInterrupt DataStorageInterrupt;
	typedef typename Super::SystemResetInterrupt SystemResetInterrupt;
	typedef typename Super::ExternalInputInterrupt ExternalInputInterrupt;
	typedef typename Super::CriticalInputInterrupt CriticalInputInterrupt;
	typedef tlm::tlm_initiator_socket<CONFIG::INSN_FSB_WIDTH * 8> i_ahb_master_if_type;
	typedef tlm::tlm_initiator_socket<CONFIG::DATA_FSB_WIDTH * 8> d_ahb_master_if_type;
	typedef tlm::tlm_target_socket<CONFIG::INCO_FSB_WIDTH * 8> ahb_slave_if_type;

	i_ahb_master_if_type i_ahb_if; // instruction master AHB interface
	d_ahb_master_if_type d_ahb_if; // data master AHB interface
	ahb_slave_if_type s_ahb_if;  // slave AHB interface
	sc_core::sc_in<bool>                m_clk;            // clock
	sc_core::sc_in<bool>                m_por;            // power-on reset
	sc_core::sc_in<bool>                p_reset_b;        // reset
	sc_core::sc_in<bool>                p_nmi_b;          // NMI
	sc_core::sc_in<bool>                p_mcp_b;          // MCP
	sc_core::sc_in<sc_dt::sc_uint<30> > p_rstbase;        // reset base
	sc_core::sc_in<sc_dt::sc_uint<8> >  p_cpuid;          // CPU ID
	sc_core::sc_in<bool>                p_extint_b;       // external interrupt
	sc_core::sc_in<bool>                p_crint_b;        // critical interrupt
	sc_core::sc_in<bool>                p_avec_b;         // autovector
	sc_core::sc_in<sc_dt::sc_uint<14> > p_voffset;        // Interrupt vector offset for vectored interrupts
	sc_core::sc_out<bool>               p_iack;           // interrupt acknowledge
	
	CPU(const sc_core::sc_module_name& name, unisim::kernel::Object *parent = 0);
	virtual ~CPU();
	
	virtual void end_of_elaboration();
	virtual bool EndSetup();

	// Backward path
	virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	
	// Forward path
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	
	virtual void Synchronize();
	virtual void Idle();
	virtual void Halt();
	virtual void InterruptAcknowledge();
	virtual void InvalidateDirectMemPtr(PHYSICAL_ADDRESS start_addr, PHYSICAL_ADDRESS end_addr);
	
	void SampleInputs();
	void UpdateSpeed();
	
	void P_IACK_Process();
	void ExternalEventProcess();
	void ClockPropertiesChangedProcess();
	void Run();
	
protected:
	sc_core::sc_time GetBurstLatency(uint32_t size, const sc_core::sc_time& latency) const;
	virtual bool DataBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr, bool rwitm);
	virtual bool DataBusWrite(PHYSICAL_ADDRESS addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	virtual bool InstructionBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);

	virtual bool DebugDataBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	virtual bool DebugDataBusWrite(PHYSICAL_ADDRESS addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	virtual bool DebugInstructionBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);

private:
	unisim::kernel::tlm2::ClockPropertiesProxy m_clk_prop_proxy;
	PayloadFabric<tlm::tlm_generic_payload> payload_fabric;
	sc_core::sc_time time_per_instruction;
	double clock_multiplier;
	sc_core::sc_time bus_cycle_time;         //<! Bus (AHB) cycle time
	sc_core::sc_time run_time;               //<! absolute timer (local time + sc_time_stamp)
	sc_core::sc_time idle_time;              //<! total idle time
	bool enable_host_idle;
	sc_core::sc_event external_event;
	sc_core::sc_event int_ack_event;
	double ipc;
	double one;
	bool enable_dmi;
	bool debug_dmi;
	unsigned int ahb_master_id;
	unisim::kernel::tlm2::QuantumKeeper qk;

	Parameter<double> param_clock_multiplier;
	Parameter<double> param_ipc;
	Parameter<bool> param_enable_host_idle;
	Parameter<bool> param_enable_dmi;
	Parameter<bool> param_debug_dmi;
	Statistic<double> stat_one;
	Statistic<sc_core::sc_time> stat_run_time;
	Statistic<sc_core::sc_time> stat_idle_time;
	StatisticFormula<double> stat_idle_rate;
	StatisticFormula<double> stat_load_rate;
	Parameter<unsigned int> param_ahb_master_id;
	
	unisim::kernel::tlm2::DMIRegionCache i_dmi_region_cache;
	unisim::kernel::tlm2::DMIRegionCache d_dmi_region_cache;

	inline void AlignToBusClock() ALWAYS_INLINE;
};

} // end of namespace mpc57xx
} // end of namespace e200
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
