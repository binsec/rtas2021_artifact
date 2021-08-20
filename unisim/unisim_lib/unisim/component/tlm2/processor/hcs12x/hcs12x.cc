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


#include <systemc>
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/component/tlm2/processor/hcs12x/hcs12x.hh"
#include "unisim/component/tlm2/processor/hcs12x/xint.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::EndDebug;


HCS12X::
HCS12X(const sc_module_name& name, S12XMMC *_mmc, Object *parent) :
	Object(name, parent)
	, sc_module(name)
	, CPU(name, parent)

	, xint_interrupt_request("interrupt_request")

	, mmc(_mmc)

	, cpu_cycle_time()
	, bus_cycle_time()
	, cpu_time()
	, bus_time()
	, last_cpu_time()
	, nice_time(1.0, SC_US)
	, next_nice_time()
	, core_clock_int(250000)
	, param_nice_time("nice-time", this, nice_time)
	, param_core_clock("core-clock", this, core_clock_int)
	, enable_fine_timing(false)
	, param_enable_fine_timing("enable-fine-timing", this, enable_fine_timing)
	, verbose_tlm_bus_synchronize(false)
	, param_verbose_tlm_bus_synchronize("verbose-tlm-bus-synchronize", this, verbose_tlm_bus_synchronize)
	, verbose_tlm_run_thread(false)
	, param_verbose_tlm_run_thread("verbose-tlm-run-thread", this, verbose_tlm_run_thread)
	, verbose_tlm_commands(false)
	, param_verbose_tlm_commands("verbose-tlm-commands", this, verbose_tlm_commands)
	, last_instruction_counter(0)
{

	param_nice_time.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_core_clock.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

	SC_HAS_PROCESS(HCS12X);

	xint_interrupt_request(*this);

	bus_clock_socket.register_b_transport(this, &HCS12X::updateCRGClock);

	SC_THREAD(Run);


	xint_trans = payloadFabric.allocate();
	xint_phase = new tlm_phase(BEGIN_REQ);

}


HCS12X ::
~HCS12X() {

	delete xint_phase;
	xint_trans->release();

}

void
HCS12X ::
Stop(int ret) {
	Object::Stop(-1);
}


void
HCS12X ::
Sync() {
	sc_time time_spent = cpu_time - last_cpu_time;
	last_cpu_time = cpu_time;
	wait(time_spent);
}

void HCS12X ::sleep() {
/* TODO:
 * Stop All Clocks and puts the device in standby mode.
 * Asserting the ~RESET, ~XIRQ, or ~IRQ signals ends standby mode.
 */

	inherited::setState(STOP);

	/*
	 * Don't do wait because all clocks are stopped and there is no way to reset/resume the simulation.
	 * The simulation control is done by the debugger engine.
	 */

	reportTrap("CPU enter STOP mode");

//	wait(irq_event | reset_event);

}

void HCS12X ::wai() {
/* TODO:
 * Enter a wait state for an integer number of bus clock cycle
 * Only CPU12 clocks are stopped
 * Wait for not masked interrupts or non-masquable interrupts
 */

	inherited::setState(WAIT);

	wait(irq_event | reset_event);

}

bool
HCS12X ::BeginSetup() {

	if(!inherited::BeginSetup()) {
		if(debug_enabled && verbose_step)
			*inherited::logger << DebugError
				<< "Error while trying to set up the HCS12X cpu"
				<< std::endl << EndDebugError;
		return (false);
	}

	/* check verbose settings */
	if( debug_enabled && inherited::verbose_all) {
		verbose_tlm_bus_synchronize = true;
		verbose_tlm_run_thread = true;
	} else {
		if( verbose_tlm_bus_synchronize)
			*inherited::logger << DebugInfo
				<< "verbose-tlm-bus-synchronize = true"
				<< std::endl << EndDebugInfo;
		if( verbose_tlm_run_thread)
			*inherited::logger << DebugInfo
				<< "verbose-tlm-run-thread = true"
				<< std::endl << EndDebugInfo;
	}

	computeInternalTime();

	if(debug_enabled &&  inherited::verbose_setup) {
		*inherited::logger << DebugInfo
			<< "Setting CPU cycle time to " << cpu_cycle_time.to_string() << std::endl
			<< "Setting Bus cycle time to " << bus_cycle_time.to_string() << std::endl
			<< "Setting nice time to " << nice_time.to_string() << std::endl
//			<< "Setting IPC to " << ipc << std::endl
			<< EndDebugInfo;
	}

	this->Reset();

	return (true);

}

bool
HCS12X ::Setup(ServiceExportBase *srv_export) {
	return (inherited::Setup(srv_export));
}

bool
HCS12X ::EndSetup() {
	return (inherited::EndSetup());
}

void HCS12X::computeInternalTime() {

	/*
	 * From CRG specification (page 100) :
	 * The core clock is twice the bus clock as shown in Figure 2-18.
	 * But note that a CPU cycle corresponds to one bus clock.
	 */
	core_clock_time = sc_time((double)core_clock_int, SC_PS);

	bus_cycle_time = core_clock_time * 2;

	cpu_cycle_time = bus_cycle_time;

	tlm2_btrans_time = sc_time((double)0, SC_PS);

	for (int i=0; i<32; i++) opCyclesArray[i] = cpu_cycle_time * i;

}


void
HCS12X::Synchronize()
{
	sc_time time_spent = cpu_time - last_cpu_time;
	last_cpu_time = cpu_time;
	if (debug_enabled) {
		std::cerr << "HCS12X: time_spent=" << time_spent << std::endl;
	}
	wait(time_spent);
	cpu_time = sc_time_stamp();

}


void
HCS12X ::
Run() {
	uint8_t opCycles = 0;

	while(1) {


		if(debug_enabled && verbose_tlm_run_thread)
			*inherited::logger << DebugInfo
				<< "Executing step"
				<< std::endl << EndDebugInfo;

		opCycles = inherited::step();

		if(debug_enabled && verbose_tlm_run_thread)
			*inherited::logger << DebugInfo
				<< "Finished executing step"
				<< std::endl << EndDebugInfo;

		sc_time & time_per_instruction = opCyclesArray[opCycles];

		cpu_time += time_per_instruction;

		if (enable_fine_timing) {
			
			wait(time_per_instruction);
		} else {
			if(cpu_time >= next_nice_time) {
				next_nice_time = cpu_time + nice_time;
				Synchronize();
			}
		}

	}
}

void
HCS12X ::
Reset() {
	inherited::Reset();
}

void HCS12X::busWrite(MMC_DATA *buffer)
{
	mmc->cpu_access(MMC::WRITE, buffer);

}

void HCS12X::busRead(MMC_DATA *buffer)
{

	mmc->cpu_access(MMC::READ, buffer);

}

void HCS12X::updateCRGClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

    // From CRG specification (page 100) : BusClock = 2 * CoreClock
	core_clock_int = *external_bus_clock / 2;

	computeInternalTime();
}


tlm_sync_enum HCS12X::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm_phase& phase, sc_core::sc_time& t) {

	irq_event.notify();

	reqAsynchronousInterrupt();

	payload.set_response_status( tlm::TLM_OK_RESPONSE );

	return (TLM_ACCEPTED);
}

address_t HCS12X ::getIntVector(uint8_t &ipl)
	/*
	 * The CPU issues a signal that tells the interrupt module to drive
	 * the vector address of the highest priority pending exception onto the system address bus
	 * (the CPU12 does not provide this address)
	 *
	 * Priority is as follow: reset => sw-interrupt => hw-interrupt => spurious interrupt
	 *
	 * If (RAM_ACC_VIOL | SYS || SWI || TRAP) return IVBR;
	 * Else return INT_Vector
	 */
{

	address_t address; // get INT_VECTOR from XINT

	INT_TRANS_T *buffer = new INT_TRANS_T(0, 0, ipl);

	xint_trans->acquire();

	xint_trans->set_command( tlm::TLM_READ_COMMAND );
	xint_trans->set_address( 0 );
	xint_trans->set_data_ptr( (unsigned char *) buffer );

	xint_trans->set_data_length( sizeof(INT_TRANS_T) );
	xint_trans->set_streaming_width( sizeof(INT_TRANS_T) );

	xint_trans->set_byte_enable_ptr( 0 );
	xint_trans->set_dmi_allowed( false );
	xint_trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

	*xint_phase = BEGIN_REQ;
	xint_interrupt_request->nb_transport_bw(*xint_trans, *xint_phase, tlm2_btrans_time);

	if (xint_trans->is_response_error() )
		SC_REPORT_ERROR("HCS12X : ", "Unable to compute interrupt vector");

	if (hasNonMaskableAccessErrorInterrupt() || hasNonMaskableSWIInterrupt() || hasTrapInterrupt() || hasSysCallInterrupt())
	{
		ipl = buffer->getPriority();
		address = buffer->getVectorAddress() & 0xFF00;

		if (hasNonMaskableAccessErrorInterrupt()) {
			address = address | XINT::INT_ILLEGAL_ACCESS_RESET_OFFSET;
		}
		else if (hasNonMaskableSWIInterrupt()) {
			address = address | XINT::INT_SWI_OFFSET;
		}
		else if (hasTrapInterrupt()) {
			address = address | XINT::INT_TRAP_OFFSET;
		}
		else if (hasSysCallInterrupt()) {
			address = address | XINT::INT_SYSCALL_OFFSET;
		}

	} else { // CPU has AsynchronousInterrupt

		switch (buffer->getVectorAddress() & 0x00FF)
		{
			case 0: /* There is no interrupt with higher priority than current IPL*/ break;
			case XINT::INT_SYS_RESET_OFFSET: {
				/*
				 * Are mapped to vector 0xFFFE: Pin reset, Power-on reset, low-voltage reset, illegal address reset
				 */
				reqReset();
			}
			break;
			case XINT::INT_CLK_MONITOR_RESET_OFFSET: {
				reqReset();
			}
			break;
			case XINT::INT_COP_WATCHDOG_RESET_OFFSET: {
				reqReset();
			}
			break;
			case XINT::INT_XIRQ_OFFSET: {
				reqXIRQInterrupt();
			}
			break;
			case XINT::INT_MPU_ACCESS_ERROR_OFFSET: {
				reqMPUAccessErrorInterrupt();
			}
			break;
			default: {
				reqIbitInterrupt();
			}
			break;
		}

		ipl = buffer->getPriority();
		address = buffer->getVectorAddress();
	}


	delete buffer;

	return (address);

}

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#undef LOCATION

