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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM_PROCESSOR_POWERPC_E600_CPU_HH__
#define __UNISIM_COMPONENT_TLM_PROCESSOR_POWERPC_E600_CPU_HH__

#include <systemc>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <unisim/component/cxx/processor/powerpc/e600/cpu.hh>
#include <unisim/component/tlm/message/snooping_fsb.hh>
#include <unisim/kernel/tlm/tlm.hh>
#include <unisim/component/tlm/message/interrupt.hh>

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace tlm {
namespace processor {
namespace powerpc {
namespace e600 {

using unisim::kernel::tlm::TlmMessage;
using unisim::kernel::tlm::TlmSendIf;
using unisim::util::garbage_collector::Pointer;
using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Statistic;
using unisim::kernel::variable::StatisticFormula;
using unisim::component::tlm::message::InterruptRequest;
using unisim::component::tlm::message::SnoopingFSBRequest;
using unisim::component::tlm::message::SnoopingFSBResponse;

template <typename TYPES, typename CONFIG>
class CPU
	: public sc_core::sc_module
	, public CONFIG::CPU
	, public TlmSendIf<SnoopingFSBRequest<typename TYPES::PHYSICAL_ADDRESS, CONFIG::FSB_BURST_SIZE>, SnoopingFSBResponse<CONFIG::FSB_BURST_SIZE> >
{
public:
	typedef typename CONFIG::CPU Super;
	typedef typename TYPES::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	typedef typename TYPES::STORAGE_ATTR STORAGE_ATTR;
	typedef typename Super::MachineCheckInterrupt MachineCheckInterrupt;
	typedef typename Super::SystemResetInterrupt SystemResetInterrupt;
	typedef typename Super::ExternalInterrupt ExternalInterrupt;
	typedef typename Super::SystemManagementInterrupt SystemManagementInterrupt;
	
	typedef SnoopingFSBRequest<PHYSICAL_ADDRESS, CONFIG::FSB_BURST_SIZE> FSBReq;
	typedef SnoopingFSBResponse<CONFIG::FSB_BURST_SIZE> FSBRsp;
	
//	// Bus SnoopingFSBRequest port
//	sc_core::sc_port<TlmSendIf<BusSnoopingFSBRequest, BusGrant> > bus_req_port;
	
	// Bus port
	sc_core::sc_port<TlmSendIf<FSBReq, FSBRsp> > bus_port;
	
	// Snoop port
	sc_core::sc_export<TlmSendIf<FSBReq, FSBRsp> > snoop_port;
	
	// Interrupt ports
	sc_core::sc_export<TlmSendIf<InterruptRequest> > external_interrupt_port;
	sc_core::sc_export<TlmSendIf<InterruptRequest> > hard_reset_port;
	sc_core::sc_export<TlmSendIf<InterruptRequest> > soft_reset_port;
	sc_core::sc_export<TlmSendIf<InterruptRequest> > mcp_port;
	sc_core::sc_export<TlmSendIf<InterruptRequest> > tea_port;
	sc_core::sc_export<TlmSendIf<InterruptRequest> > smi_port;
	
	CPU(const sc_core::sc_module_name& name, Object *parent = 0);
	virtual ~CPU();
	virtual void Synchronize();
	virtual bool EndSetup();
	inline void BusSynchronize() ALWAYS_INLINE;

	void Run();
	virtual bool Send(const Pointer<TlmMessage<FSBReq, FSBRsp> >& message);
	virtual void Reset();
	virtual void Idle();
	virtual void Halt();
	virtual void RunInternalTimers();
	inline void LazyRunInternalTimers() ALWAYS_INLINE;
	inline void AlignToBusClock() ALWAYS_INLINE;
	virtual void end_of_simulation();
protected:
// 	virtual bool BusRead(physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::WIMG wimg = CONFIG::WIMG_DEFAULT, bool rwitm = false);
// 	virtual bool BusWrite(physical_address_t physical_addr, const void *buffer, uint32_t size, typename CONFIG::WIMG wimg = CONFIG::WIMG_DEFAULT);
// 	virtual bool BusZeroBlock(physical_address_t physical_addr);
// 	virtual bool BusFlushBlock(physical_address_t physical_addr);
	virtual bool DataBusRead(PHYSICAL_ADDRESS physical_addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr, bool rwitm);
	virtual bool DataBusWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	virtual bool InstructionBusRead(PHYSICAL_ADDRESS physical_addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	virtual bool DebugDataBusRead(PHYSICAL_ADDRESS physical_addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	virtual bool DebugDataBusWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	virtual bool DebugInstructionBusRead(PHYSICAL_ADDRESS physical_addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
private:
	sc_core::sc_time cpu_cycle_time;
	sc_core::sc_time bus_cycle_time;
	sc_core::sc_time timer_cycle_time;
	sc_core::sc_time cpu_time;             //<! local time (relative to sc_core::sc_time_stamp)
	sc_core::sc_time run_time;             //<! absolute time (local time + sc_core::sc_time_stamp)
	sc_core::sc_time timer_time;           //<! absolute time from the internal timers point of view
	sc_core::sc_time nice_time;            //<! period of synchronization with other threads
	sc_core::sc_time idle_time;
	sc_core::sc_time timers_update_deadline; //<! deadline for updating internal timers in order to keep internal timer accuracy
	bool enable_host_idle;
	double ipc;
	sc_core::sc_event ev_max_idle;
	sc_core::sc_event ev_interrupt;
	sc_core::sc_time max_idle_time;
	double one;
	bool enable_monotonic_internal_timer;
	
	Parameter<sc_core::sc_time> param_cpu_cycle_time;
	Parameter<sc_core::sc_time> param_bus_cycle_time;
	Parameter<sc_core::sc_time> param_nice_time;
	Parameter<double> param_ipc;
	Parameter<bool> param_enable_host_idle;
	Statistic<sc_core::sc_time> stat_run_time;
	Statistic<sc_core::sc_time> stat_idle_time;
	Statistic<double> stat_one;
	StatisticFormula<double> stat_idle_rate;
	StatisticFormula<double> stat_load_rate;
	Parameter<bool> param_enable_monotonic_internal_timer;
	
	enum IRQ
	{
		IRQ_EXTERNAL_INTERRUPT,
		IRQ_HARD_RESET,
		IRQ_SOFT_RESET,
		IRQ_MCP,
		IRQ_TEA,
		IRQ_SMI
	};

	class IRQListener :
		public sc_core::sc_module,
		public TlmSendIf<InterruptRequest>
	{
	public:
		IRQListener(const sc_core::sc_module_name& name, IRQ irq, typename CONFIG::CPU *_cpu, sc_core::sc_event *ev);
		virtual bool Send(const Pointer<TlmMessage<InterruptRequest> >& message);
	private:
		IRQ irq;
		typename CONFIG::CPU *cpu;
		sc_core::sc_event *ev;
	};

	IRQListener external_interrupt_listener;
	IRQListener hard_reset_listener;
	IRQListener soft_reset_listener;
	IRQListener mcp_listener;
	IRQListener tea_listener;
	IRQListener smi_listener;
};

} // end of namespace e600
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim


#endif
