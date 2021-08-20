/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/dual_timer/dt.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/util/endian/endian.hh"
#include <inttypes.h>
#include <assert.h>
#include <iostream>
#include <sstream>

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace dual_timer {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::LittleEndian2Host;

const uint32_t DualTimer::REGS_ADDR_ARRAY[DualTimer::NUMREGS] =
{
	TIMER1LOAD, // 1
	TIMER1VALUE, // 2
	TIMER1CONTROL, // 3
	TIMER1INTCLR, // 4
	TIMER1RIS, // 5
	TIMER1MIS, // 6
	TIMER1BGLOAD, // 7
	TIMER2LOAD, // 8
	TIMER2VALUE, // 9
	TIMER2CONTROL, // 10
	TIMER2INTCLR, // 11
	TIMER2RIS, // 12
	TIMER2MIS, // 13
	TIMER2BGLOAD, // 14
	TIMERITCR, // 15
	TIMERITOP, // 16
	TIMERPERIPHID0, // 17
	TIMERPERIPHID1, // 18
	TIMERPERIPHID2, // 19
	TIMERPERIPHID3, // 20
	TIMERPCELLID0, // 21
	TIMERPCELLID1, // 22
	TIMERPCELLID2, // 23
	TIMERPCELLID3, // 24
};

const char *DualTimer::REGS_NAME_ARRAY[DualTimer::NUMREGS] =
{
	"timer1load",
	"timer1value",
	"timer1control",
	"timer1intclr",
	"timer1ris",
	"timer1mis",
	"timer1bgload",
	"timer2load",
	"timer2value",
	"timer2control",
	"timer2intclr",
	"timer2ris",
	"timer2mis",
	"timer2bgload",
	"timeritcr",
	"timeritop",
	"timerperiphid0",
	"timerperiphid1",
	"timerperiphid2",
	"timerperiphid3",
	"timerpcellid0",
	"timerpcellid1",
	"timerpcellid2",
	"timerpcellid3"
};

DualTimer ::
DualTimer(const sc_module_name &name, Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_module(name)
	, unisim::util::generic_peripheral_register::GenericPeripheralRegisterInterface<uint32_t>()
	, bus_target_socket("bus_target_socket")
	, t1_update_time()
	, t2_update_time()
	, t1_event()
	, t2_event()
	, output_update_event()
	, base_addr(0)
	, param_base_addr("base-addr", this, base_addr,
			"Base address of the system controller.")
	, verbose(0)
	, param_verbose("verbose", this, verbose,
			"Verbose level (0 = no verbose).")
	, logger(*this)
{
	SC_THREAD(Timer1Handler);
	SC_THREAD(Timer2Handler);
	SC_THREAD(OutputUpdate);

	timint1_out_port.initialize(false);
	timint2_out_port.initialize(false);
	timintc_out_port.initialize(false);

	bus_target_socket.register_nb_transport_fw(this,
			&DualTimer::bus_target_nb_transport_fw);
	bus_target_socket.register_b_transport(this,
			&DualTimer::bus_target_b_transport);
	bus_target_socket.register_get_direct_mem_ptr(this,
			&DualTimer::bus_target_get_direct_mem_ptr);
	bus_target_socket.register_transport_dbg(this,
			&DualTimer::bus_target_transport_dbg);

	// init the registers values
	memset(regs, 0, 0x01000);
	uint32_t val = Host2LittleEndian((uint32_t)0xffffffffUL);
	memcpy(&regs[0x04], &val, 4);
	regs[0x08] = (uint8_t)0x20;
	memcpy(&regs[0x24], &val, 4);
	regs[0x28] = (uint8_t)0x20;
	regs[0xfe0] = (uint8_t)0x04;
	regs[0xfe4] = (uint8_t)0x18;
	regs[0xfe8] = (uint8_t)0x14;
	regs[0xff0] = (uint8_t)0x0d;
	regs[0xff4] = (uint8_t)0xf0;
	regs[0xff8] = (uint8_t)0x05;
	regs[0xffc] = (uint8_t)0xb1;
	
	for ( unsigned int i = 0; i < NUMREGS; i++ )
	{
		regs_accessor[i] = new
			unisim::util::generic_peripheral_register::GenericPeripheralWordRegister(
					this, REGS_ADDR_ARRAY[i]);
		std::stringstream desc;
		desc << REGS_NAME_ARRAY[i] << " register.";
		regs_service[i] = new
			unisim::kernel::variable::Register<unisim::util::generic_peripheral_register::GenericPeripheralWordRegister>(
					REGS_NAME_ARRAY[i], this, *regs_accessor[i], 
					desc.str().c_str());
	}
}

DualTimer ::
~DualTimer()
{
	for ( unsigned int i = 0; i < NUMREGS; i++ )
	{
		delete regs_service[i];
		regs_service[i] = 0;
		delete regs_accessor[i];
		regs_accessor[i] = 0;
	}
}

void
DualTimer ::
Timer1Handler()
{
	while ( 1 )
	{
		wait(t1_event);
		// first of all signal an interrupt if necessary:
		// 1 - check control enable (not sure is required if we 
		//     are here)
		// 2 - check that an interrupt is not already signaled
		// 3 - check that interrupts are enabled
		// 3 - signal the interrupt (and update RIS and MISregisters status)
		uint32_t control = GetRegister(TIMER1CONTROL);
		if ( GetEnable(control) )
		{
			if ( GetRegister(TIMER1RIS) == 0 )
			{
				SetRegister(TIMER1RIS, 1);
				if ( GetIntEnable(control) )
				{
					SetRegister(TIMER1MIS, 1);
					if ( VERBOSE(V0, V_INT) )
						logger << DebugInfo
							<< "Generating interrupt on output 1 and combined"
							<< " output (" << sc_time_stamp() << ")"
							<< EndDebugInfo;
				}
			}
		}
		output_update_event.notify(SC_ZERO_TIME);

		// update the counter, depending on the mode
		t1_update_time = sc_time_stamp();
		if ( GetEnable(control) )
		{
			// if the timer is in one shot mode then set the timer value simply
			//   to zero
			if ( GetOneShot(control) )
			{
				SetRegister(TIMER1VALUE, 0);
			}
			// if the timer is in free mode we can set it to 0xffffffff
			else if ( GetTimerMode(control) == false )
			{
				SetRegister(TIMER1VALUE, 0xffffffffUL);
			}
			else // the timer is periodic mdoe
			{
				SetRegister(TIMER1VALUE, GetRegister(TIMER1LOAD));
			}
		}

		// check if a new interrupt event is required
		if ( GetEnable(control) && GetIntEnable(control) )
		{
			// only free-running mode and periodic modes cause new
			//   interrupts, if in one shot mode do nothing
			if ( !GetOneShot(control) )
			{
				uint32_t prescale = GetPrescale(control);
				uint32_t counter = GetRegister(TIMER1VALUE);
				// modify the counter to be 16 or 32 bits
				if ( !GetTimerSize(control) )
					counter = counter & 0x0ffffUL;
				sc_core::sc_time delay =
					sc_core::sc_time(
							(double)counter * (double)timclken1_in_port 
							* prescale, 
							SC_PS);
				t1_event.notify(delay);
			}
		}

	}
}

void
DualTimer ::
Timer2Handler()
{
	while ( 1 )
	{
		wait(t2_event);
		// first of all signal an interrupt if necessary:
		// 1 - check control enable (not sure is required if we 
		//     are here)
		// 2 - check that an interrupt is not already signaled
		// 3 - check that interrupts are enabled
		// 3 - signal the interrupt (and update RIS and MISregisters status)
		uint32_t control = GetRegister(TIMER2CONTROL);
		if ( GetEnable(control) )
		{
			if ( GetRegister(TIMER2RIS) == 0 )
			{
				SetRegister(TIMER2RIS, 1);
				if ( GetIntEnable(control) )
				{
					SetRegister(TIMER2MIS, 1);
					if ( VERBOSE(V0, V_INT) )
						logger << DebugInfo
							<< "Generating interrupt on output 1 and combined"
							<< " output (" << sc_time_stamp() << ")"
							<< EndDebugInfo;
				}
			}
		}
		output_update_event.notify(SC_ZERO_TIME);

		// update the counter, depending on the mode
		t2_update_time = sc_time_stamp();
		if ( GetEnable(control) )
		{
			// if the timer is in one shot mode then set the timer value simply
			//   to zero
			if ( GetOneShot(control) )
			{
				SetRegister(TIMER2VALUE, 0);
			}
			// if the timer is in free mode we can set it to 0xffffffff
			else if ( GetTimerMode(control) == false )
			{
				SetRegister(TIMER2VALUE, 0xffffffffUL);
			}
			else // the timer is periodic mdoe
			{
				SetRegister(TIMER2VALUE, GetRegister(TIMER2LOAD));
			}
		}

		// check if a new interrupt event is required
		if ( GetEnable(control) && GetIntEnable(control) )
		{
			// only free-running mode and periodic modes cause new
			//   interrupts, if in one shot mode do nothing
			if ( !GetOneShot(control) )
			{
				uint32_t prescale = GetPrescale(control);
				uint32_t counter = GetRegister(TIMER2VALUE);
				// modify the counter to be 16 or 32 bits
				if ( !GetTimerSize(control) )
					counter = counter & 0x0ffffUL;
				sc_core::sc_time delay =
					sc_core::sc_time(
							(double)counter * (double)timclken2_in_port *
							prescale,
							SC_PS);
				t2_event.notify(delay);
			}
		}

	}
}

void
DualTimer::
OutputUpdate()
{
	while ( 1 )
	{
		wait(output_update_event);

		/** check the status of the raw interrupt status and the interrupt
		 * enable, if both are active then generate the required interrupt
		 */
		uint32_t t1ris = GetRegister(TIMER1RIS);
		uint32_t t2ris = GetRegister(TIMER2RIS);
		uint32_t t1control = GetRegister(TIMER1CONTROL);
		uint32_t t2control = GetRegister(TIMER2CONTROL);
		bool t1intenable = GetIntEnable(t1control);
		bool t2intenable = GetIntEnable(t2control);

		bool t1int = (t1ris != 0) && t1intenable;
		bool t2int = (t2ris != 0) && t2intenable;
		bool tcint = t1ris || t2ris;

		timint1_out_port = t1int;
		timint2_out_port = t2int;
		timintc_out_port = tcint;
	}
}

bool 
DualTimer::
BeginSetup()
{
	return true;
}

/**************************************************************************/
/* Virtual methods for the target socket for the bus connection     START */
/**************************************************************************/

tlm::tlm_sync_enum
DualTimer ::
bus_target_nb_transport_fw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	assert("TODO" == 0);
	return tlm::TLM_COMPLETED;
}

void 
DualTimer ::
bus_target_b_transport(transaction_type &trans, 
		sc_core::sc_time &delay)
{
	uint32_t addr = trans.get_address() - base_addr;
	uint8_t *data = trans.get_data_ptr();
	uint32_t size = trans.get_data_length();
	bool is_read = trans.is_read();
	bool handled = false;

	// check the address range
	if ( (addr >= 0x01000UL) ||
			((0x01000L - size) < addr) )
	{
		logger << DebugWarning
			<< "Incorrect access address range:" << std::endl
			<< " - read = " << is_read << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size;
		if ( !is_read )
		{
			logger << std::endl
				<< " - data =" << std::hex;
			for ( unsigned int i = 0; i < size; i++ )
				logger << " " << (unsigned int)data[i];
			logger << std::dec;
		}
		logger << std::endl
			<< "Ending transaction with address error."
			<< EndDebugWarning;
		// set the tlm error status
		trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
	}

	// do not accept request that access more than one register, neither accept
	//   streaming requests
	// NOTE: this maybe something that might be supported in future versions
	//       of the module implementation
	if ( (((addr & 0x03) + size) > 4) || trans.get_streaming_width() ) 
	{
		logger << DebugWarning
			<< "Multiple register access and/or streamed accesses"
			<< " are not supported:" << std::endl
			<< " - read = " << is_read << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size;
		if ( !is_read )
		{
			logger << std::endl
				<< " - data =" << std::hex;
			for ( unsigned int i = 0; i < size; i++ )
				logger << " " << (unsigned int)data[i];
			logger << std::dec;
		}
		logger << std::endl
			<< "Ending transaction with burst error."
			<< EndDebugWarning;
		// set the tlm error status
		trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
	}

	/* in some conditions the registers might need to be updated
	 * for example:
	 * - when reading Current Value Register, TimerXValue of t1 or t2 and
	 *     the timer is enabled
	 *
	 * To make it simple we update the status of the timers each time
	 *   a read/write operation is performed.
	 */
	UpdateStatus(delay);

	// handle reads
	if ( is_read )
	{
		memcpy(data, &regs[addr], size);
		handled = true;
	}

	else // it is write
	{
		/* Steps performed when writing:
		 * 1 - make a copy of destination register before modifying it
		 * 2 - update register with new data
		 * 3 - get the value of the new register
		 * 4 - verify new value of the register and perform required
		 *     actions
		 */
		uint32_t cur_addr = addr & ~(uint32_t)0x03UL;
		uint32_t prev_value, new_value;

		/* 1 - make a copy of destination register before modifying it
		 */
		prev_value = GetRegister(cur_addr);

		/* 2 - update register with new data */
		memcpy(&regs[addr], data, size);

		/* 3 - get the value of the new register */
		new_value = GetRegister(cur_addr);

		/* 4 - verify new value of the register and perform required 
		 *     actions */
		if ( ( cur_addr == TIMER1CONTROL ) ||
				( cur_addr == TIMER2CONTROL ) )
		{
			uint32_t unmod_value = new_value;
			new_value = (new_value & ~0xffffff00UL) | 
				(prev_value & 0xffffff00UL);

			/* if the new value needed to be fixed rewrite it to the 
			 *   register */
			if ( unmod_value != new_value )
			{
				SetRegister(cur_addr, new_value);
			}

			/* if the value did not change no further action is 
			 *   required */
			if ( prev_value == new_value )
			{
				handled = true;
			}

			else 
			{
				handled = true;
				/* if the new value is deactivating the timer nothing
				 *   needs to be done if the interrupts were not enabled,
				 *   however if the interrupts were enabled the interrupt
				 *   event should be deactivated */
				if ( (new_value & (uint32_t)0x080UL) == 0 )
				{
					if ( (prev_value & (uint32_t)0x080UL) != 0 )
					{
						if ( cur_addr == TIMER1CONTROL )
							t1_event.cancel();
						else
							t2_event.cancel();
						if ( VERBOSE(V0, V_STATUS) )
							logger << DebugInfo
								<< "Timer "
								<< ((cur_addr == TIMER1CONTROL) ?
										"1" : "2")
								<< " disabled at "
								<< sc_time_stamp()
								<< EndDebugInfo;
					}
				}

				else
				{
					/* if the TimerEn did change from 0 to 1 then set 
					 *   the update time for t1/t2 */
					if ( ((prev_value & (uint32_t)0x080) == 0) )
					{
						handled &= true;
						if ( cur_addr == TIMER1CONTROL )
							t1_update_time = sc_time_stamp();
						else
							t2_update_time = sc_time_stamp();

						if ( VERBOSE(V0, V_STATUS) )
							logger << DebugInfo
								<< "Timer "
								<< ((cur_addr == TIMER1CONTROL) ?
										"1" : "2")
								<< " enabled at "
								<< ((cur_addr == TIMER1CONTROL) ?
										t1_update_time :
										t2_update_time)
								<< EndDebugInfo;
					}
					else
					{
						/* update the value of the TimerXValue if the
						 *   timer was activated before the access */
						if ( (prev_value & (uint32_t)0x080) != 0 )
						{
							logger << DebugError
								<< "Update timer " 
								<< ((cur_addr == TIMER1CONTROL) ?
										"1" : "2") 
								<< std::endl
								<< " - prev value = 0x" << std::hex
								<< prev_value << std::endl
								<< " - new value = 0x" 
								<< new_value << std::dec
								<< EndDebugError;
							handled &= false;
						}
					}

					/* if the IntEnable entry is not active no further 
					 *   action is needed, just deactivate the interrupt
					 *   events */
					if ( (new_value & (uint32_t)0x020) == 0 )
					{
						if ( (prev_value & (uint32_t)0x020) )
						{
							if ( VERBOSE(V0, V_STATUS) )
								logger << DebugInfo
									<< "Timer "
									<< ((cur_addr == TIMER1CONTROL) ?
											"1" : "2")
									<< " interrupts disabled"
									<< EndDebugInfo;
							if ( cur_addr == TIMER1CONTROL )
								t1_event.cancel();
							else
								t2_event.cancel();
						}
						handled &= true;
					}

					else
					{

						if ( (prev_value & (uint32_t)0x020) == 0 )
						{
							sc_core::sc_time inttime;
							uint32_t control = GetRegister(cur_addr);
							uint32_t prescale = GetPrescale(control);
							// set a new event depending on the counter value
							if ( cur_addr == TIMER1CONTROL )
							{
								// first deactivate any possible interrupt event
								t1_event.cancel();
								uint32_t counter = GetRegister(TIMER1VALUE);
								if ( !GetTimerSize(control) )
									counter &= 0x0ffffUL;
								// compute the time it will require to generate
								//   the interrupt
								double tick_time = ((double)timclken1_in_port 
										* prescale * counter);
								inttime = sc_core::sc_time(tick_time, SC_PS);
								// notify when the interrupt should arrive
								t1_event.notify(inttime);
							}
							else
							{
								// first deactivate any possible interrupt event
								t2_event.cancel();
								uint32_t counter = GetRegister(TIMER2VALUE);
								if ( !GetTimerSize(control) )
									counter &= 0x0ffffUL;
								// compute the time it will require to generate
								//   the interrupt
								double tick_time = ((double)timclken2_in_port
										* prescale * counter);
								inttime = sc_core::sc_time(tick_time, SC_PS);
								// notify when the interrupt should arrive
								t2_event.notify(inttime);
							}
							if ( VERBOSE(V0, V_STATUS) )
								logger << DebugInfo
									<< "Timer "
									<< ((cur_addr == TIMER1CONTROL) ?
											"1" : "2")
									<< " interrupts enabled"
									<< " (interrupt in "
									<< inttime << ")"
									<< EndDebugInfo;
						}
						handled &= true;
					}
				}
			}
		}

		else if ( (cur_addr == TIMER1LOAD) ||
				(cur_addr == TIMER2LOAD) )
		{
			handled = true;
			if ( new_value == 0 )
				logger << DebugWarning
					<< "Setting "
					<< ((cur_addr == TIMER1LOAD) ?
							"TIMER1LOAD" :
							"TIMER2LOAD")
					<< " to 0, this might cause continuous interrupts"
					<< " under certain circumstances"
					<< EndDebugWarning;

			// update the timer 1/2 value to the new value
			if ( cur_addr == TIMER1LOAD )
			{
				t1_update_time = sc_time_stamp();
				SetRegister(TIMER1VALUE, new_value);
			}
			else
			{
				t2_update_time = sc_time_stamp();
				SetRegister(TIMER2VALUE, new_value);
			}

			// if the timer is interrupt enabled and enable then cancel the 
			//   previous interrupt and set a new one
			uint32_t control = 0;
			if ( cur_addr == TIMER1LOAD )
				control = GetRegister(TIMER1CONTROL);
			else
				control = GetRegister(TIMER2CONTROL);
			if ( GetEnable(control) && 
					GetIntEnable(control) )
			{
				uint32_t prescale = GetPrescale(control);
				sc_core::sc_time inttime;
				if ( cur_addr == TIMER1LOAD )
				{
					// first deactivate any possible interrupt event
					t1_event.cancel();
					// compute the time it will require to generate
					//   the interrupt
					double tick_time = 
						((double)timclken1_in_port * new_value * prescale);
					inttime = sc_core::sc_time(tick_time, SC_PS);
					// notify when the interrupt should arrive
					t1_event.notify(inttime);
				}
				else
				{
					// first deactivate any possible interrupt event
					t2_event.cancel();
					// compute the time it will require to generate
					//   the interrupt
					double tick_time = 
						((double)timclken2_in_port * new_value * prescale);
					inttime = sc_core::sc_time(tick_time, SC_PS);
					// notify when the interrupt should arrive
					t2_event.notify(inttime);
				}
				if ( VERBOSE(V0, V_STATUS) )
						logger << DebugInfo
							<< "Timer "
							<< ((cur_addr == TIMER1LOAD) ?
									"1" : "2")
							<< " interrupts enabled"
							<< " (interrupt in "
							<< inttime << ")"
							<< EndDebugInfo;
			}
		}

		else if ( cur_addr == TIMER1VALUE ||
				cur_addr == TIMER2VALUE )
		{
			handled = true;
			logger << DebugWarning
				<< "Trying to write into "
				<< ((cur_addr == TIMER1VALUE) ?
							"TIMER1VALUE" :
							"TIMER2VALUE")
				<< " which is read-only, ignoring new value (0x"
				<< std::hex << new_value << std::dec << ") and keeping"
				<< " the old value (0x"
				<< std::hex << prev_value << std::dec << ")"
				<< EndDebugWarning;
			if ( cur_addr == TIMER1VALUE )
				SetRegister(TIMER1VALUE, prev_value);
			else
				SetRegister(TIMER2VALUE, prev_value);
		}

		else if ( cur_addr == TIMER1INTCLR ||
				cur_addr == TIMER2INTCLR )
		{
			handled = true;
			if ( cur_addr == TIMER1INTCLR )
				SetRegister(TIMER1RIS, 0);
			else
				SetRegister(TIMER2RIS, 0);
			if ( VERBOSE(V0, V_STATUS) )
				logger << DebugInfo
					<< "Timer "
					<< ((cur_addr == TIMER1INTCLR) ?
							"1" : "2")
					<< " clearing interrupt raw register"
					<< EndDebugInfo;
			output_update_event.notify(SC_ZERO_TIME);
		}

		else if ( cur_addr == TIMER1BGLOAD ||
				cur_addr == TIMER2BGLOAD )
		{
			handled = true;
			if ( new_value == 0 )
				logger << DebugWarning
					<< "Setting "
					<< ((cur_addr == TIMER1LOAD) ?
							"TIMER1BGLOAD" :
							"TIMER2BGLOAD")
					<< " to 0, this might cause continuous interrupts"
					<< " under certain circumstances"
					<< EndDebugWarning;

			if ( cur_addr == TIMER1BGLOAD )
				SetRegister(TIMER1LOAD, new_value);
			else
				SetRegister(TIMER2LOAD, new_value);
		}
	}

	if ( !handled )
	{
		// display error
		logger << DebugError
			<< "Access to dual timer:" << std::endl
			<< " - read = " << is_read << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size;
		if ( !is_read )
		{
			logger << std::endl
				<< " - data =" << std::hex;
			for ( unsigned int i = 0; i < size; i++ )
				logger << " " << (unsigned int)data[i];
			logger << std::dec;
		}
		logger << std::endl
			<< "Stopping simulation because of unhandled behavior"
			<< EndDebugError;
		// stop simulation
		unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	}

	// everything went fine, update the status of the tlm response
	trans.set_response_status(tlm::TLM_OK_RESPONSE);

	if ( VERBOSE(V0, V_READ | V_WRITE) )
	{
		logger << DebugInfo
			<< "Access to dual timer:" << std::endl
			<< " - read = " << is_read << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size << std::endl
			<< " - data =" << std::hex;
		for ( unsigned int i = 0; i < size; i++ )
			logger << " " << (unsigned int)data[i];
		logger << std::dec
			<< EndDebugInfo;
	}
}

bool 
DualTimer ::
bus_target_get_direct_mem_ptr(transaction_type &trans, 
		tlm::tlm_dmi &dmi_data)
{
	assert("TODO" == 0);
	return false;
}

unsigned int 
DualTimer ::
bus_target_transport_dbg(transaction_type &trans)
{
	assert("TODO" == 0);
	return 0;
}

/**************************************************************************/
/* Virtual methods for the target socket for the bus connection       END */
/**************************************************************************/

/** Returns the register pointed by the given address
 *
 * @param addr the address to consider
 * @return the value of the register pointed by the address
 */
uint32_t 
DualTimer ::
GetRegister(uint32_t addr) const
{
	const uint8_t *data = &(regs[addr]);
	uint32_t value = 0;

	memcpy(&value, data, 4);
	value = LittleEndian2Host(value);
	return value;
}

/** Sets the register pointed by the given address
 *
 * @param addr the address to consider
 * @param value the value to set the register
 */
void 
DualTimer ::
SetRegister(uint32_t addr, uint32_t value)
{
	uint8_t *data = &(regs[addr]);

	value = Host2LittleEndian(value);
	memcpy(data, &value, 4);
}

/** Get interface for the generic peripheral register interface
 *
 * @param addr the address to consider
 * @return the value of the register pointed by the address
 */
uint32_t
DualTimer ::
GetPeripheralRegister(uint64_t addr)
{
	return GetRegister(addr);
}

/** Set interface for the generic peripheral register interface
 *
 * @param addr the address to consider
 * @param value the value to set the register to
 */
void
DualTimer ::
SetPeripheralRegister(uint64_t addr, uint32_t value)
{
	SetRegister(addr, value);
}

/** Update the status of the timer at the given time
 *
 * @param delay the delta time 
 */
void 
DualTimer ::
UpdateStatus(sc_core::sc_time &delay)
{
	// to make things easier synchronize with the engine
	wait(delay);
	delay = SC_ZERO_TIME;

	// update timer 1
	uint32_t old_timer = 0;
	if ( VERBOSE(V0, V_STATUS) )
		old_timer = GetRegister(TIMER1VALUE);
	UpdateTime(TIMER1CONTROL, TIMER1VALUE, timclken1_in_port, t1_update_time);
	if ( VERBOSE(V0, V_STATUS) )
	{
		uint32_t new_timer = GetRegister(TIMER1VALUE);
		if ( old_timer != new_timer)
			logger << DebugInfo
				<< "Updated timer 1 value from " << old_timer
				<< " (0x" << std::hex << old_timer << std::dec << ")"
				<< " to " << new_timer << " (0x" << std::hex
				<< new_timer << std::dec << ")"
				<< EndDebugInfo;
	}

	// update timer 2
	if ( VERBOSE(V0, V_STATUS) )
		old_timer = GetRegister(TIMER2VALUE);
	UpdateTime(TIMER2CONTROL, TIMER2VALUE, timclken1_in_port, t2_update_time);
	if ( VERBOSE(V0, V_STATUS) )
	{
		uint32_t new_timer = GetRegister(TIMER2VALUE);
		if ( old_timer != new_timer)
			logger << DebugInfo
				<< "Updated timer 1 value from " << old_timer
				<< " (0x" << std::hex << old_timer << std::dec << ")"
				<< " to " << new_timer << " (0x" << std::hex
				<< new_timer << std::dec << ")"
				<< EndDebugInfo;
	}
	
}

/** Update the counter
 *
 * @param control_addr the timer control register address
 * @param value_addr the timer counter register address
 * @param clken the clock enable of the given timer (in picoseconds)
 * @param update_time when was the timer counter last updated and the new time
 */
void
DualTimer ::
UpdateTime(uint32_t control_addr, uint32_t value_addr,
		uint64_t clken,
		sc_time &update_time)
{
	uint32_t control = GetRegister(control_addr);
	if ( control & (uint32_t)0x080 )
	{
		uint32_t prescale = GetPrescale(control); 
		const sc_core::sc_time &cur_time = sc_time_stamp();
		if ( update_time < cur_time )
		{
			sc_core::sc_time diff_time = cur_time - update_time;
			// double tick_time =
			// 	(((double)clken / (double)timclk_in_port) * prescale) *
			// 	(double)timclk_in_port;
			double tick_time =
				((double)clken * prescale);
			sc_core::sc_time tick = 
				sc_core::sc_time(tick_time, SC_PS);
			uint64_t diff = diff_time / tick;
			if ( diff != 0 )
			{
				uint32_t new_val = 0;
				uint32_t current_val = GetRegister(value_addr);
				new_val = current_val - diff; // fix ???
				if ( !GetTimerSize(control) )
					new_val = (GetRegister(value_addr) & 0xffff0000UL) |
						(new_val & 0x0ffffUL);
				SetRegister(value_addr, new_val);
				update_time = cur_time;
			}
		}
	}
}

/** Extracts the Enable bit from the given control register value
 *
 * @param control the control register value to use
 * @return true if the enable bit is enable, false otherwise
 */
bool 
DualTimer ::
GetEnable(uint32_t control) const
{
	return (control & 0x080UL);
}

/** Extracts the interrupt enable bit from the given control register value
 *
 * @param control the control register value to use
 * @return true if the interrupt enable bit is enable, false otherwise
 */
bool 
DualTimer ::
GetIntEnable(uint32_t control) const
{
	return (control & 0x020UL);
}

/** Return timer size from the given control value
 *
 * Returns true if the timer is in 32b mode, false otherwise
 *
 * @param control the control register value to use
 * @return true if the timer is in 32b mode, false otherwise
 */
bool
DualTimer ::
GetTimerSize(uint32_t control) const
{
	return (control & 0x02UL);
}

/** Extract prescale from the given control value
 *
 * @param value the value of the control register
 * @return the prescaling value
 */
uint32_t
DualTimer ::
GetPrescale(uint32_t value)
{
	uint32_t prescale = (value & (uint32_t)0x0c) >> 2;
	switch ( prescale )
	{
		case 0:
			prescale = 1; break;
		case 1: 
			prescale = 16; break;
		case 2:
			prescale = 256; break;
		default: 
			logger << DebugWarning
				<< "Undefined prescale, using 1"
				<< EndDebugWarning;
			prescale = 1;
			break;
	}
	return prescale;
}

/** Return timer mode from the givel control value
 *
 * Returns true if the timer is in periodic mode, false otherwise
 *
 * @param control the value of the control register
 * @return true if timer is in periodic mode, false otherwise
 */
bool 
DualTimer ::
GetTimerMode(uint32_t control) const
{
	return (control & 0x040UL);
}

/** Return one shot mode from the given control value
 *
 * Returns true if the timer is in one shot mode, false otherwise
 *
 * @param control the value of the control register
 * @return true if the timer is in one shot mode, false otherwise
 */
bool
DualTimer ::
GetOneShot(uint32_t control) const
{
	return (control & 0x01UL);
}

} // end of namespace dual_timer 
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim


