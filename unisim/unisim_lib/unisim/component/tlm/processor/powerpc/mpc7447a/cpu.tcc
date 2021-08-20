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
 
#ifndef __UNISIM_COMPONENT_TLM_PROCESSOR_POWERPC_MPC7447A_CPU_TCC__
#define __UNISIM_COMPONENT_TLM_PROCESSOR_POWERPC_MPC7447A_CPU_TCC__

#include <unistd.h>

namespace unisim {
namespace component {
namespace tlm {
namespace processor {
namespace powerpc {
namespace mpc7447a {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
	
template <class CONFIG>
CPU<CONFIG>::CPU(const sc_module_name& name, Object *parent)
	: Object(name, parent, "PowerPC MPC7447A CPU")
	, sc_module(name)
	, unisim::component::cxx::processor::powerpc::mpc7447a::CPU<CONFIG>(name, parent)
	, bus_port("bus-port")
	, snoop_port("snoop-port")
	, cpu_cycle_time()
	, bus_cycle_time()
	, timer_cycle_time()
	, cpu_time()
	, run_time()
	, timer_time()
	, nice_time(1.0, SC_MS)
	, idle_time()
	, timers_update_deadline()
	, enable_host_idle(false)
	, ipc(2.0)
	, one(1.0)
	, param_bus_cycle_time("bus-cycle-time", this, bus_cycle_time, "bus cycle time")
	, param_nice_time("nice-time", this, nice_time, "maximum time between synchonizations")
	, param_ipc("ipc", this, ipc, "targeted average instructions per second")
	, param_enable_host_idle("enable-host-idle", this, enable_host_idle, "Enable/Disable host idle periods when target is idle")
	, stat_run_time("run-time", this, run_time, "run time")
	, stat_idle_time("idle-time", this, idle_time, "idle time")
	, stat_one("one", this, one, "one")
	, stat_idle_rate("idle-rate", this, "/", &stat_idle_time, &stat_run_time, "idle rate")
	, stat_load_rate("load-rate", this, "-", &stat_one, &stat_idle_rate, "load rate")
	, external_interrupt_listener("external_interrupt_listener", CONFIG::EXC_EXTERNAL, this, &ev_interrupt)
	, hard_reset_listener("hard_reset_listener", CONFIG::EXC_SYSTEM_RESET_HARD, this, &ev_interrupt)
	, soft_reset_listener("soft_reset_listener", CONFIG::EXC_SYSTEM_RESET_SOFT, this, &ev_interrupt)
	, mcp_listener("mcp_listener", CONFIG::EXC_MACHINE_CHECK_MCP, this, &ev_interrupt)
	, tea_listener("tea_listener", CONFIG::EXC_MACHINE_CHECK_TEA, this, &ev_interrupt)
	, smi_listener("smi_listener", CONFIG::EXC_SYSTEM_MANAGEMENT, this, &ev_interrupt)
{
	stat_one.SetMutable(false);
	stat_one.SetSerializable(false);
	stat_one.SetVisible(false);
	param_ipc.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	
	SC_HAS_PROCESS(CPU);
	
	snoop_port(*this);
	external_interrupt_port(external_interrupt_listener);
	hard_reset_port(hard_reset_listener);
	soft_reset_port(soft_reset_listener);
	mcp_port(mcp_listener);
	tea_port(tea_listener);
	smi_port(smi_listener);

	SC_THREAD(Run);
}

template <class CONFIG>
CPU<CONFIG>::~CPU()
{
}

template <class CONFIG>
void CPU<CONFIG>::Synchronize()
{
	wait(cpu_time);
	cpu_time = SC_ZERO_TIME;
	run_time = sc_time_stamp();
	if(!inherited::linux_os_import) RunInternalTimers();
}
	
template <class CONFIG>
bool CPU<CONFIG>::EndSetup()
{
	if(!inherited::EndSetup()) return false;
	cpu_cycle_time = sc_time((double) inherited::cpu_cycle_time, SC_PS);
	timer_cycle_time = 4 * bus_cycle_time;
	return true;
}

template <class CONFIG>
void CPU<CONFIG>::end_of_simulation()
{
	run_time = sc_time_stamp();
	cpu_time = SC_ZERO_TIME;
	if(!inherited::linux_os_import) RunInternalTimers(); // make run_time match timer_time
}

template <class CONFIG>
inline void CPU<CONFIG>::LazyRunInternalTimers()
{
	if(unlikely(run_time >= timers_update_deadline))
	{
		RunInternalTimers();
	}
}

template <class CONFIG>
inline void CPU<CONFIG>::AlignToBusClock()
{
	sc_dt::uint64 bus_cycle_time_tu = bus_cycle_time.value();
	sc_dt::uint64 run_time_tu = run_time.value();
	sc_dt::uint64 modulo = run_time_tu % bus_cycle_time_tu;
	if(!modulo) return; // already aligned
	
	sc_dt::uint64 time_alignment_tu = bus_cycle_time_tu - modulo;
	sc_time time_alignment(time_alignment_tu, false);
	cpu_time += time_alignment;
	run_time += time_alignment;
}

template <class CONFIG>
void CPU<CONFIG>::Idle()
{
	// This is called within thread Run()
	
	// First synchronize with other threads
	Synchronize();
	
	// Compute the time to consume before an internal timer event occurs
	max_idle_time = timer_cycle_time;
	max_idle_time *= inherited::GetMaxIdleTime();
	max_idle_time += timer_time;
	max_idle_time -= run_time;
	
	// Notify an event
	ev_max_idle.notify(max_idle_time);
	
	// wait for either an internal timer event or an external event
	sc_time old_time_stamp(sc_time_stamp());
	wait(ev_max_idle | ev_interrupt);
	sc_time new_time_stamp(sc_time_stamp());
	
	// cancel event because we can't know which wake up condition occured
	ev_max_idle.cancel();
	
	// compute the time spent by the SystemC wait
	sc_time delta_time(new_time_stamp);
	delta_time -= old_time_stamp;
	
	if(enable_host_idle)
	{
		usleep(delta_time.to_seconds() * 1.0e6); // leave host CPU when target CPU is idle
	}
	
	idle_time += delta_time;
	
	// update overall run time
	run_time = new_time_stamp;
	
	// run internal timers
	LazyRunInternalTimers();
}

template <class CONFIG>
inline void CPU<CONFIG>::RunInternalTimers()
{
	sc_dt::uint64 delta_time_tu = run_time.value() - timer_time.value();
	sc_dt::uint64 timer_cycle_time_tu = timer_cycle_time.value();
	uint64_t delta = delta_time_tu / timer_cycle_time_tu;
	inherited::RunTimers(delta);
	sc_dt::uint64 t_tu = timer_cycle_time_tu * delta;
	sc_time t(t_tu, false);
	timer_time += t;
	timers_update_deadline = timer_time + (inherited::GetTimersDeadline() * timer_cycle_time);
}

template <class CONFIG>
void CPU<CONFIG>::Run()
{
	sc_time time_per_instruction = cpu_cycle_time / ipc;
	
	while(1)
	{
			inherited::StepOneInstruction();
			// update local time (relative to sc_time_stamp)
			cpu_time += time_per_instruction;
			// update absolute time (local time + sc_time_stamp)
			run_time += time_per_instruction;
			// Run internal timers
			LazyRunInternalTimers();
			// Periodically synchronize with other threads
			if(unlikely(cpu_time >= nice_time))
			{
				Synchronize();
			}
	}
}

template <class CONFIG>
bool CPU<CONFIG>::Send(const Pointer<TlmMessage<FSBReq, FSBRsp> >& message)
{
	return true;
}

template <class CONFIG>
void CPU<CONFIG>::Reset()
{
}
	
template <class CONFIG>
bool CPU<CONFIG>::BusRead(physical_address_t physical_addr, void *buffer, uint32_t size, typename CONFIG::WIMG wimg, bool rwitm)
{
#ifdef DEBUG_POWERPC
	cerr << sc_time_stamp() << " " << name() << "::BusRead()" << endl;
#endif
	AlignToBusClock();
	Synchronize();
		
	Pointer<FSBReq> req = new(req) FSBReq(); // bus transaction request
	Pointer<FSBRsp> rsp; // bus transaction response
	sc_event rsp_ev; // event notified when bus transaction response is ready
	Pointer<TlmMessage<FSBReq, FSBRsp> > msg =
			new(msg) TlmMessage<FSBReq, FSBRsp>(req, rsp_ev); // message for bus transaction
	
	req->type = rwitm ? FSBReq::READX : FSBReq::READ; // transaction is a READ/READX
	req->global = (wimg & CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED) ? true : false; // whether transaction is global or not
	req->addr = physical_addr; // transaction address
	req->size = size; // actual transaction size
	
	// loop until the request succeeds
#ifdef DEBUG_POWERPC
	cerr << sc_time_stamp() << " PPC::BusRead: loop until the request succeeds" << endl;
#endif
	// request succeeds when none of the processors is busy or have a modified copy of the request data in there caches
	do
	{
		// loop until bus transaction request is accepted
#ifdef DEBUG_POWERPC
		cerr << sc_time_stamp() << " PPC::BusRead: loop until bus transaction request is accepted" << endl;
#endif
		while(!bus_port->Send(msg))
		{
			// if bus transaction request is not accepted then retry later
			wait(bus_cycle_time);
			run_time = sc_time_stamp();
			cpu_time = SC_ZERO_TIME;
			if(!inherited::linux_os_import) LazyRunInternalTimers();
#ifdef DEBUG_POWERPC
			cerr << sc_time_stamp() << " PPC::BusRead: retry transaction request" << endl;
#endif
		}
		
		// bus transaction request has been accepted at this point
#ifdef DEBUG_POWERPC
		cerr << sc_time_stamp() << " PPC::BusRead: transaction request accepted" << endl;
		cerr << sc_time_stamp() << " PPC::BusRead: waiting for response" << endl;
#endif
		// wait for the bus transaction response
		wait(rsp_ev);
		run_time = sc_time_stamp();
		cpu_time = SC_ZERO_TIME;
		if(!inherited::linux_os_import) LazyRunInternalTimers();
		rsp = msg->GetResponse();
#ifdef DEBUG_POWERPC
		cerr << sc_time_stamp() << " PPC::BusRead: received response" << endl;
		if(!rsp->read_status & (FSBRsp::RS_BUSY | FSBRsp::RS_MODIFIED))
			cerr << sc_time_stamp() << " PPC::BusRead: response read status is neither RS_BUSY nor RS_MODIFIED" << endl;
#endif
	} while(rsp->read_status & (FSBRsp::RS_BUSY | FSBRsp::RS_MODIFIED));
	
	// bus transaction response has been received at this point
#ifdef DEBUG_POWERPC
	cerr << sc_time_stamp() << " PPC::BusRead: copying response data" << endl;
#endif
	// copy the data from the response
	memcpy(buffer, rsp->read_data, size);
	
#ifdef DEBUG_POWERPC
	if(rsp->read_status & FSBRsp::RS_SHARED)
		cerr << sc_time_stamp() << " PPC::BusRead: read status contains RS_SHARED" << endl;
#endif

	// get the bus transaction response read status in order to update the block state
	return true;
}
	
	
template <class CONFIG>
bool CPU<CONFIG>::BusWrite(physical_address_t physical_addr, const void *buffer, uint32_t size, typename CONFIG::WIMG wimg)
{
#ifdef DEBUG_POWERPC
	cerr << sc_time_stamp() << " " << name() << "::BusWrite()" << endl;
#endif
	AlignToBusClock();
	Synchronize();
		
	Pointer<FSBReq > req = new(req) FSBReq(); // bus transaction request
	Pointer<TlmMessage<FSBReq, FSBRsp > > msg =
			new(msg) TlmMessage<FSBReq, FSBRsp >(req); // message for bus transaction
	
	req->type = FSBReq::WRITE; // transaction is a WRITE
	req->global = (wimg & CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED) ? true : false; // whether transaction is global or not
	req->addr = physical_addr; // transaction address
	req->size = size; // actual transaction size
	// copy the data into the bus transaction request
#ifdef DEBUG_POWERPC
	cerr << sc_time_stamp() << " PPC::BusWrite: copying data into transaction request" << endl;
#endif
	memcpy(req->write_data, buffer, size);
		
	// loop until bus transaction request is accepted
#ifdef DEBUG_POWERPC
	cerr << sc_time_stamp() << " PPC::BusWrite: loop until the request succeeds" << endl;
#endif
	while(!bus_port->Send(msg))
	{
		// if bus transaction request is not accepted then retry later
		wait(bus_cycle_time);
		run_time = sc_time_stamp();
		cpu_time = SC_ZERO_TIME;
		if(!inherited::linux_os_import) LazyRunInternalTimers();
#ifdef DEBUG_POWERPC
		cerr << sc_time_stamp() << " PPC::BusWrite: retry transaction request" << endl;
#endif
	}
	// bus transaction request has been accepted at this point
#ifdef DEBUG_POWERPC
	cerr << sc_time_stamp() << " PPC::BusWrite: transaction request accepted" << endl;
#endif
	req = 0;
	msg = 0;
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::BusZeroBlock(physical_address_t physical_addr)
{
#ifdef DEBUG_POWERPC
	cerr << sc_time_stamp() << " " << name() << "::BusZeroBlock()" << endl;
#endif
	AlignToBusClock();
	Synchronize();
		
	Pointer<FSBReq > req = new(req) FSBReq(); // bus transaction request
	Pointer<TlmMessage<FSBReq, FSBRsp > > msg =
			new(msg) TlmMessage<FSBReq, FSBRsp >(req); // message for bus transaction
	
	req->type = FSBReq::ZERO_BLOCK; // transaction is a ZERO_BLOCK
	req->global = true; // whether transaction is global or not
	req->addr = physical_addr; // transaction address
	req->size = 0; // actual transaction size
		
	// loop until bus transaction request is accepted
#ifdef DEBUG_POWERPC
	cerr << sc_time_stamp() << " PPC::BusZeroBlock: loop until the request succeeds" << endl;
#endif
	while(!bus_port->Send(msg))
	{
		// if bus transaction request is not accepted then retry later
		wait(bus_cycle_time);
		run_time = sc_time_stamp();
		cpu_time = SC_ZERO_TIME;
		if(!inherited::linux_os_import) LazyRunInternalTimers();
#ifdef DEBUG_POWERPC
		cerr << sc_time_stamp() << " PPC::BusZeroBlock: retry transaction request" << endl;
#endif
	}
	// bus transaction request has been accepted at this point
#ifdef DEBUG_POWERPC
	cerr << sc_time_stamp() << " PPC::BusZeroBlock: transaction request accepted" << endl;
#endif
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::BusFlushBlock(physical_address_t physical_addr)
{
#ifdef DEBUG_POWERPC
	cerr << sc_time_stamp() << " " << name() << "::BusFlushBlock()" << endl;
#endif
	AlignToBusClock();
	Synchronize();
		
	Pointer<FSBReq > req = new(req) FSBReq(); // bus transaction request
	Pointer<TlmMessage<FSBReq, FSBRsp > > msg =
			new(msg) TlmMessage<FSBReq, FSBRsp >(req); // message for bus transaction
	
	req->type = FSBReq::FLUSH_BLOCK; // transaction is a FLUSH_BLOCK
	req->global = true; // whether transaction is global or not
	req->addr = physical_addr; // transaction address
	req->size = 0; // actual transaction size
		
	// loop until bus transaction request is accepted
#ifdef DEBUG_POWERPC
	cerr << sc_time_stamp() << " PPC::BusFlushBlock: loop until the request succeeds" << endl;
#endif
	while(!bus_port->Send(msg))
	{
		// if bus transaction request is not accepted then retry later
		wait(bus_cycle_time);
		run_time = sc_time_stamp();
		cpu_time = SC_ZERO_TIME;
		if(!inherited::linux_os_import) LazyRunInternalTimers();
#ifdef DEBUG_POWERPC
		cerr << sc_time_stamp() << " PPC::BusFlushBlock: retry transaction request" << endl;
#endif
	}
	// bus transaction request has been accepted at this point
#ifdef DEBUG_POWERPC
	cerr << sc_time_stamp() << " PPC::BusFlushBlock: transaction request accepted" << endl;
#endif
	return true;
}

template <class CONFIG>
CPU<CONFIG>::IRQListener::IRQListener(const sc_module_name& name, unsigned int _irq, unisim::component::cxx::processor::powerpc::mpc7447a::CPU<CONFIG> *_cpu, sc_event *_ev) :
	sc_module(name),
	irq(_irq),
	cpu(_cpu),
	ev(_ev)
{
}
	
template <class CONFIG>
bool CPU<CONFIG>::IRQListener::Send(const Pointer<TlmMessage<InterruptRequest> >& message)
{
	if(message->req->level)
		cpu->SetException(irq);
	else
		cpu->ResetException(irq);
	
	ev->notify(SC_ZERO_TIME);
	return true;
}

} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif
