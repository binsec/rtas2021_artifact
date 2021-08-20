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
 
#ifndef __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_DUAL_TIMER_DT_HH__
#define __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_DUAL_TIMER_DT_HH__

#include <inttypes.h>
#include <systemc>
#include <tlm>
#include <tlm_utils/passthrough_target_socket.h>
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/util/generic_peripheral_register/generic_peripheral_register.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace dual_timer {

class DualTimer
	: public unisim::kernel::Object
	, public sc_module
	, public unisim::util::generic_peripheral_register::GenericPeripheralRegisterInterface<uint32_t>
{
public:
	typedef tlm::tlm_base_protocol_types::tlm_payload_type transaction_type;
	typedef tlm::tlm_base_protocol_types::tlm_phase_type   phase_type;
	typedef tlm::tlm_sync_enum                             sync_enum_type;

	/** Timer module clock input port */
	sc_in<uint64_t> timclk_in_port;
	/** Timer module clock enable 1 input port */
	sc_in<uint64_t> timclken1_in_port;
	/** Timer module clock enable 2 input port */
	sc_in<uint64_t> timclken2_in_port;

	/** Timer interrupt 1 output port */
	sc_out<bool> timint1_out_port;
	/** Timer interrupt 2 output port */
	sc_out<bool> timint2_out_port;
	/** Timer interrupt combined output port */
	sc_out<bool> timintc_out_port;

	/** Target socket for the bus connection */
	tlm_utils::passthrough_target_socket<DualTimer, 32>
		bus_target_socket;

	SC_HAS_PROCESS(DualTimer);
	DualTimer(const sc_module_name &name, Object *parent = 0);
	~DualTimer();

	virtual bool BeginSetup();

private:
	/** Thread handler of timer 1 */
	void Timer1Handler();
	/** Thread handler of timer 2 */
	void Timer2Handler();
	/** Thread handler of output signals timintx and timintc */
	void OutputUpdate();

	/** Registers storage */
	uint8_t regs[0x01000];
	/** Timer 1 last update time */
	sc_time t1_update_time;
	/** Timer 2 last update time */
	sc_time t2_update_time;
	/** Timer 1 event produced when timer reaches threshold
	 *  The event only takes place if the interrupts are enabled */
	sc_event t1_event;
	/** Timer 2 event produced when timer reaches threshold
	 *  The event only takes place if the interrupts are enabled */
	sc_event t2_event;
	/** Output update event to indicate that the update of the output signals
	 */
	sc_event output_update_event;

	/**************************************************************************/
	/* Virtual methods for the target socket for the bus connection     START */
	/**************************************************************************/

	sync_enum_type bus_target_nb_transport_fw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void bus_target_b_transport(transaction_type &trans, 
			sc_core::sc_time &delay);
	bool bus_target_get_direct_mem_ptr(transaction_type &trans, 
			tlm::tlm_dmi &dmi_data);
	unsigned int bus_target_transport_dbg(transaction_type &trans);

	/**************************************************************************/
	/* Virtual methods for the target socket for the bus connection       END */
	/**************************************************************************/

	/**************************************************************************/
	/* Registers and accessors                                          START */
	/**************************************************************************/

	static const uint32_t TIMER1LOAD      = 0x0000UL;
	static const uint32_t TIMER1VALUE     = 0x0004UL;
	static const uint32_t TIMER1CONTROL   = 0x0008UL;
	static const uint32_t TIMER1INTCLR    = 0x000cUL;
	static const uint32_t TIMER1RIS       = 0x0010UL;
	static const uint32_t TIMER1MIS       = 0x0014UL;
	static const uint32_t TIMER1BGLOAD    = 0x0018UL;
	static const uint32_t TIMER2LOAD      = 0x0020UL;
	static const uint32_t TIMER2VALUE     = 0x0024UL;
	static const uint32_t TIMER2CONTROL   = 0x0028UL;
	static const uint32_t TIMER2INTCLR    = 0x002cUL;
	static const uint32_t TIMER2RIS       = 0x0030UL;
	static const uint32_t TIMER2MIS       = 0x0034UL;
	static const uint32_t TIMER2BGLOAD    = 0x0038UL;
	static const uint32_t TIMERITCR       = 0x0f00UL;
	static const uint32_t TIMERITOP       = 0x0f04UL;
	static const uint32_t TIMERPERIPHID0  = 0x0fe0UL;
	static const uint32_t TIMERPERIPHID1  = 0x0fe4UL;
	static const uint32_t TIMERPERIPHID2  = 0x0fe8UL;
	static const uint32_t TIMERPERIPHID3  = 0x0fecUL;
	static const uint32_t TIMERPCELLID0   = 0x0ff0UL;
	static const uint32_t TIMERPCELLID1   = 0x0ff4UL;
	static const uint32_t TIMERPCELLID2   = 0x0ff8UL;
	static const uint32_t TIMERPCELLID3   = 0x0ffcUL;

	static const uint32_t NUMREGS = 24;
	static const uint32_t REGS_ADDR_ARRAY[NUMREGS];
	static const char *REGS_NAME_ARRAY[NUMREGS];

	/** Returns the register pointed by the given address
	 *
	 * @param addr the address to consider
	 * @return the value of the register pointed by the address
	 */
	uint32_t GetRegister(uint32_t addr) const;
	/** Sets the register pointed by the given address
	 *
	 * @param addr the address to consider
	 * @param value the value to set the register
	 */
	void SetRegister(uint32_t addr, uint32_t value);

	/** Get interface for the generic peripheral register interface
	 *
	 * @param addr the address to consider
	 * @return the value of the register pointed by the address
	 */
	virtual uint32_t GetPeripheralRegister(uint64_t addr);
	/** Set interface for the generic peripheral register interface
	 *
	 * @param addr the address to consider
	 * @param value the value to set the register to
	 */
	virtual void SetPeripheralRegister(uint64_t addr, uint32_t value);

	/**************************************************************************/
	/* Registers and accessors                                            END */
	/**************************************************************************/

	/** Update the status of the timer at the given time
	 *
	 * @param delay the delta time 
	 */
	void UpdateStatus(sc_core::sc_time &delay);

	/** Update the counter
	 *
	 * @param control_addr the timer control register address
	 * @param value_addr the timer counter register address
	 * @param clken the clock enable of the given timer (in picoseconds)
	 * @param update_time when was the timer counter last updated and the new time
	 */
	void UpdateTime(uint32_t control_addr, uint32_t value_addr,
			uint64_t clken, sc_time &update_time);

	/** Extracts the Enable bit from the given control register value
	 *
	 * @param control the control register value to use
	 * @return true if the enable bit is enable, false otherwise
	 */
	bool GetEnable(uint32_t control) const;

	/** Extracts the interrupt enable bit from the given control register value
	 *
	 * @param control the control register value to use
	 * @return true if the interrupt enable bit is enable, false otherwise
	 */
	bool GetIntEnable(uint32_t control) const;

	/** Return timer size from the given control value
	 *
	 * Returns true if the timer is in 32b mode, false otherwise
	 *
	 * @param control the control register value to use
	 * @return true if the timer is in 32b mode, false otherwise
	 */
	bool GetTimerSize(uint32_t control) const;

	/** Extract prescale from the given control value
	 *
	 * @param value the value of the control register
	 * @return the prescaling value
	 */
	uint32_t GetPrescale(uint32_t control);

	/** Return timer mode from the givel control value
	 *
	 * Returns true if the timer is in periodic mode, false otherwise
	 *
	 * @param control the value of the control register
	 * @return true if timer is in periodic mode, false otherwise
	 */
	bool GetTimerMode(uint32_t control) const;

	/** Return one shot mode from the given control value
	 *
	 * Returns true if the timer is in one shot mode, false otherwise
	 *
	 * @param control the value of the control register
	 * @return true if the timer is in one shot mode, false otherwise
	 */
	bool GetOneShot(uint32_t control) const;

	/** Base address of the system controller */
	uint32_t base_addr;
	/** UNISIM Parameter for the base address of the system controller */
	unisim::kernel::variable::Parameter<uint32_t> param_base_addr;

	/** Register helpers to use the UNISIM Register service */
	unisim::util::generic_peripheral_register::GenericPeripheralWordRegister *
		regs_accessor[NUMREGS];
	/** UNISIM Registers for the timer registers */
	unisim::kernel::variable::Register<
		unisim::util::generic_peripheral_register::GenericPeripheralWordRegister
		> *
		regs_service[NUMREGS];

	/** Verbose */
	uint32_t verbose;
	/** UNISIM Paramter for verbose */
	unisim::kernel::variable::Parameter<uint32_t> param_verbose;
	/** Verbose levels */
	static const uint32_t V0 = 0x01UL;
	static const uint32_t V1 = 0x03UL;
	static const uint32_t V2 = 0x07UL;
	static const uint32_t V3 = 0x0fUL;
	/** Verbose target mask */
	static const uint32_t V_READ     = 0x01UL << 4;
	static const uint32_t V_WRITE    = 0x01UL << 5;
	static const uint32_t V_STATUS   = 0x01UL << 6;
	static const uint32_t V_INT      = 0x01UL << 7;
	/** Check if we should verbose */
	bool VERBOSE(uint32_t level, uint32_t mask) const
	{
		uint32_t ok_level = level & verbose;
		uint32_t ok_mask = (~verbose) & mask; 
		return ok_level && ok_mask;
	};

	/** Interface to the UNISIM logger */
	unisim::kernel::logger::Logger logger;

};

} // end of namespace dual_timer
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_DUAL_TIMER_DT_HH__

