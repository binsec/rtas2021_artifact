/*
 *  Copyright (c) 2008, 2011
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
 * This module implement the S12ECT16B8C_V2 controler
 */

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_ECT_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_ECT_HH__

#include <inttypes.h>
#include <iostream>
#include <cmath>
#include <map>

#include <systemc>

#include <tlm>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "tlm_utils/simple_target_socket.h"

#include "unisim/kernel/logger/logger.hh"
#include <unisim/kernel/kernel.hh>
#include "unisim/kernel/tlm2/tlm.hh"

#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/trap_reporting.hh"

#include "unisim/service/interfaces/register.hh"

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
using namespace tlm;
using namespace tlm_utils;

using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::EndDebug;

using unisim::kernel::Object;
using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::CallBackObject;
using unisim::kernel::variable::SignalArray;
using unisim::kernel::VariableBase;
using unisim::kernel::VariableBaseListener;
using unisim::service::interfaces::TrapReporting;

using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;

using unisim::service::interfaces::Register;

using unisim::component::cxx::processor::hcs12x::ADDRESS;
using unisim::component::cxx::processor::hcs12x::address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::CONFIG;

using unisim::kernel::Object;
using unisim::kernel::tlm2::PayloadFabric;

class ECT :
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

	enum REGS_OFFSETS {TIOS, CFORC, OC7M, OC7D, TCNT_HIGH, TCNT_LOW, TSCR1,
						TTOV, TCTL1, TCTL2, TCTL3, TCTL4, TIE, TSCR2, TFLG1, TFLG2,
						TC0_HIGH, TC0_LOW, TC1_HIGH, TC1_LOW, TC2_HIGH, TC2_LOW,
						TC3_HIGH, TC3_LOW, TC4_HIGH, TC4_LOW, TC5_HIGH, TC5_LOW,
						TC6_HIGH, TC6_LOW, TC7_HIGH, TC7_LOW, PACTL, PAFLG,
						PACN3, PACN2, PACN1, PACN0, MCCTL, MCFLG, ICPAR, DLYCT,
						ICOVW, ICSYS, RESERVED, TIMTST, PTPSR, PTMCPSR, PBCTL,
						PBFLG, PA3H, PA2H, PA1H, PA0H, MCCNT_HIGH, MCCNT_LOW,
						TC0H_HIGH, TC0H_LOW, TC1H_HIGH, TC1H_LOW,
						TC2H_HIGH, TC2H_LOW, TC3H_HIGH, TC3H_LOW};

	static const unsigned int MEMORY_MAP_SIZE = 64;

	//=========================================================
	//=                MODULE INTERFACE                       =
	//=========================================================

	ServiceImport<TrapReporting > trap_reporting_import;

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	tlm_utils::simple_target_socket<ECT> slave_socket;

	tlm_utils::simple_target_socket<ECT> bus_clock_socket;

	ServiceExport<Memory<physical_address_t> > memory_export;
	ServiceImport<Memory<physical_address_t> > memory_import;
	ServiceExport<Registers> registers_export;

	// the kernel logger
	unisim::kernel::logger::Logger *logger;

	ECT(const sc_module_name& name, Object *parent = 0);
	virtual ~ECT();

	virtual void Reset();
	
	void runBuildinSignalGenerator();

	void runMainTimerCounter();
	inline void main_timer_enable();
	inline void main_timer_disable();

	void runOutputCompare();

	void runDownCounter();
	inline void down_counter_enable() { down_counter_enabled = true; down_counter_enable_event.notify(sc_core::SC_ZERO_TIME); }
	inline void down_counter_disable() {down_counter_enabled = false; }

	inline void delay_counter_enable() { delay_counter_enabled = true; delay_counter_enable_event.notify(sc_core::SC_ZERO_TIME); }
	inline void delay_counter_disable() { delay_counter_enabled = false; }
	inline bool isDelayCounterEnabled() { return (delay_counter_enabled); }
	sc_time getEdgeDelayCounter() { return (edge_delay_counter_time); }

	inline void enterWaitMode();
	inline void exitWaitMode();
	inline void enterFreezeMode();
	inline void exitFreezeMode();

	void assertInterrupt(unsigned int interrupt_offset);
	void updateCRGClock(tlm::tlm_generic_payload& trans, sc_time& delay);

	inline void latchToHoldingRegisters();
	inline void computeDelayCounter();

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
	//=             Registers Interface interface methods               =
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
    inline bool isInputCapture(uint8_t channel_index);
    inline void setTimerInterruptFlag(uint8_t ioc_index) { tflg1_register = tflg1_register | (1 << ioc_index); }
    bool isNoInputCaptureOverWrite(uint8_t ioc_index) { return ((icovw_register & (1 << ioc_index)) != 0);  }
    uint16_t getMainTimerValue() { return (tcnt_register); }
    bool isLatchMode() { return ((icsys_register & 0x01) != 0); }

    bool isBufferEnabled() { return ((icsys_register & 0x02) != 0); }
    bool isPulseAccumulatorsMaximumCount() { return ((icsys_register & 0x04) != 0); }

	/**
	 * To operate the 16-bit pulse accumulators A and B (PACA and PACB) independently of input capture or
	 * output compare 7 and 0 respectively, the user must set the corresponding bits: IOSx = 1, OMx = 0, and
	 * OLx = 0. OC7M7 or OC7M0 in the OC7M register must also be cleared
	 */

    bool isPulseAccumulatorAEnabled() {
    	return (((pactl_register & 0x40) != 0) &&
    			((tios_register & 0x0C) != 0) &&
    			((tctl12_register & 0x00F0) == 0) &&
    			((oc7m_register & 0x80) == 0));
    }

    bool isPulseAccumulatorBEnabled() {
    	return (((pbctl_register & 0x40) != 0) &&
    			((tios_register & 0x03) != 0) &&
    			((tctl12_register & 0x000F) == 0) &&
    			((oc7m_register & 0x01) == 0));
    }

    bool isPulseAccumulators8BitEnabled(uint8_t pac_index) {
		if ((icpar_register & (1 << pac_index)) != 0) {
	    	if (pac_index < 2) {
	    		if ((pactl_register & 0x40) == 0) {
	    			return (true);
	    		}
	    	} else {
	    		if ((pbctl_register & 0x40) == 0) {
	    			return (true);
	    		}
	    	}
		}

    	return (false);
    }

    bool isOutputCompareMaskSet(uint8_t ioc_index) { return ((oc7m_register & (1 << ioc_index)) != 0); }
    uint8_t getInterruptOffsetChannel0() { return (offset_channel0_interrupt); }
    uint8_t getInterruptPulseAccumulatorAOverflow() { return (pulse_accumulatorA_overflow_interrupt); }
    uint8_t getInterruptPulseAccumulatorBOverflow() { return (pulse_accumulatorB_overflow_interrupt); }
    bool isInputOutputInterruptEnabled(uint8_t ioc_index) { return ((tie_register & (1 << ioc_index)) != 0); }
    bool isTimerOverflowinterruptEnabled() { return ((tscr2_register & 0x80) != 0); }
    bool isTimerCounterResetEnabled() { return ((tscr2_register & 0x08) != 0); }
    bool isTimerFlagSettingModeSet() { return (((icsys_register & 0x08) >> 3) != 0); }
    void setPulseAccumulatorAOverflowFlag() { paflg_register = paflg_register | 0x02; }
    void setPulseAccumulatorBOverflowFlag() { pbflg_register = pbflg_register | 0x02; }
    void setPulseAccumulatorAInputEdgeFlag() { paflg_register = paflg_register | 0x01; }

	void setOC7Dx(uint8_t ioc_index, bool data) {
		uint8_t val = (data)? 1 : 0;
		uint8_t mask = 1 << ioc_index;

		oc7d_register = (oc7d_register & ~mask) | (val << ioc_index);
	}

	bool getOC7Dx(uint8_t ioc_index) { return ((oc7d_register & (1 << ioc_index)) != 0); }

    uint8_t getClockSelection() { return ((pactl_register & 0x0C) >> 2); }

    bool isBuildin_edge_generator() { return (builtin_signal_generator); }

    void notify_paclk_event() { paclk_event.notify(sc_core::SC_ZERO_TIME); }

    /**
     * isValidEdge() method model the "Edge Detector" circuit
     */
    bool isValidEdge(uint8_t ioc_index) {

    	uint8_t signalValue;

    	signalValue = (portt_pin[ioc_index])? 1 /* rising edge */: 2 /* falling edge */;

		return ((signalValue == ioc_channel[ioc_index]->getValideEdge()) ||
				((ioc_channel[ioc_index]->getValideEdge() == 3) && ((signalValue == 1) || (signalValue == 2))));
    }

	void setPOLF(uint8_t ioc_index, bool detectedSignal) {
		// is rising edge ?
		uint8_t polf = (detectedSignal)? 1:0;

		uint8_t mask = (1 << ioc_index);
		uint8_t mcflg_register_tmp = mcflg_register & ~mask;
		mcflg_register = mcflg_register_tmp | (polf << ioc_index) ;
	}

	bool isTimerEnabled() { return ((tscr1_register & 0x80) != 0); }
	sc_time getBusClock() { return (bus_cycle_time); }

	void resetTimerCounter() { tcnt_register = 0x0000; }

	void runChannelOutputCompareAction(uint8_t ioc_index);

private:
	inline void computeInternalTime();
	inline void computeModulusCounterClock();
	inline void computeTimerPrescaledClock();
	inline void configureEdgeDetector();
    inline void configureOutputAction();

	tlm_quantumkeeper quantumkeeper;
	PayloadFabric<XINT_Payload> xint_payload_fabric;
	XINT_Payload *xint_payload;

	double	bus_cycle_time_int;	// The time unit is PS
	Parameter<double>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;

	sc_time prescaled_clock;

	sc_time mccnt_clock;

	uint16_t	edge_delay_counter;
	sc_time		edge_delay_counter_time;

	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	// Enhanced Capture Timer Channel 0 : IVBR + 0xEE
	// Enhanced Capture Timer Channel 1 : IVBR + 0xEC
	// Enhanced Capture Timer Channel 2 : IVBR + 0xEA
	// Enhanced Capture Timer Channel 3 : IVBR + 0xE8
	// Enhanced Capture Timer Channel 4 : IVBR + 0xE6
	// Enhanced Capture Timer Channel 5 : IVBR + 0xE4
	// Enhanced Capture Timer Channel 6 : IVBR + 0xE2
	// Enhanced Capture Timer Channel 7 : IVBR + 0xE0

	unsigned int offset_channel0_interrupt;
	Parameter<unsigned int> param_offset_channel0_interrupt;

	unsigned int offset_timer_overflow_interrupt;
	Parameter<unsigned int> param_offset_timer_overflow_interrupt;

	unsigned int pulse_accumulatorA_overflow_interrupt;
	Parameter<unsigned int> param_pulse_accumulatorA_overflow_interrupt;

	unsigned int pulse_accumulatorB_overflow_interrupt;
	Parameter<unsigned int> param_pulse_accumulatorB_overflow_interrupt;

	unsigned int pulse_accumulatorA_input_edge_interrupt;
	Parameter<unsigned int> param_pulse_accumulatorA_input_edge_interrupt;

	unsigned int modulus_counter_interrupt;
	Parameter<unsigned int> param_modulus_counter_interrupt;

	bool	debug_enabled;
	Parameter<bool>	param_debug_enabled;

	bool	builtin_signal_generator;
	Parameter<bool>	param_builtin_signal_generator;

	uint64_t	signal_generator_period_int;
	Parameter<uint64_t>	param_signal_generator_period;
	sc_time signal_generator_period;

	bool portt_pin[8];
	SignalArray<bool> portt_pin_reg;

	// Registers map
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	std::vector<unisim::kernel::VariableBase*> extended_registers_registry;

	bool prnt_write; // TSCR1::PRNT is write once bit
	bool icsys_write; // ICSYS register is write once in normal mode

	sc_event paclk_event;

	bool main_timer_enabled;
	sc_event main_timer_enable_event;
	bool down_counter_enabled;
	sc_event down_counter_enable_event;
	bool delay_counter_enabled;
	sc_event delay_counter_enable_event;

	//==============================
	//=            REGISTER SET    =
	//==============================

	uint8_t	tios_register, cforc_register, oc7m_register, oc7d_register, pacn_register[4], paxh_registers[4],
			tscr1_register, tie_register, tscr2_register, tflg1_register, tflg2_register,
			pactl_register, paflg_register, mcctl_register, mcflg_register, icpar_register,
			dlyct_register, icovw_register, icsys_register, reserved_address, timtst_register,
			ptpsr_register, ptmcpsr_register, pbctl_register, pbflg_register;

	uint16_t tcnt_register, ttov_register, tctl12_register, tctl34_register,
			tc_registers[8], mccnt_load_register, mccnt_register, tcxh_registers[4];


//	/**
//	 * PORTT: Timer Port Data Register
//	 * Read : Any time (input return pin level; outputs return data register contents)
//	 * Write: Data stored in an internal latch (drives pins only if configured for output).
//	 *
//	 * Since the output compare 7 register (OC7) shares pins with the pulse accumulator input,
//	 * the only way for the pulse accumulator to receive an independent input from OC7 is by setting
//	 * both OM7 and OL7 to be 0, and also OC7M7 in OC7M register to be 0.
//	 * OC7 can still reset the counter if enabled while PT7 is used as an input to the pulse accumulator.
//	 *
//	 * note: Writes do not change pin state when the pin is configured for timer output.
//	 *       The minimum pulse width for pulse accumulator input should always be greater
//	 *       than the width of two module clocks due to input synchronizer circuitry.
//	 */
//	uint8_t portt_register;

	class PulseAccumulator8Bit {
	public:
		PulseAccumulator8Bit(ECT *parent, const uint8_t pacn_number, uint8_t *pacn_ptr, uint8_t* pah_ptr);

		void latchToHoldingRegisters();
		bool countEdge8Bit();
		void clearPACN() {	*pacn_register_ptr = 0x00;}
		uint8_t getIndex() { return (pacn_index); }

	protected:

	private:
		uint8_t pacn_index;

		ECT	*ectParent;

		uint8_t *pacn_register_ptr;
		uint8_t* pah_register_ptr;

	};

	PulseAccumulator8Bit* pc8bit[4];

	bool isSharingEdgeEnabled(uint8_t mask) { return ((icsys_register & mask) != 0); }
	void notifySharedEdgeTo(uint8_t index) { ioc_channel[index]->notifyEdge(); }


	class IOC_Channel_t : public sc_module, public VariableBaseListener {
	public:

		IOC_Channel_t(const sc_module_name& name, ECT *parent, const uint8_t index, bool* pinLogic, uint16_t *tc_ptr, uint16_t* tch_ptr, PulseAccumulator8Bit* pc8bit);

		void runOutputCompare();
		void latchToHoldingRegisters();
		void runInputCapture();
		void setValideEdge(uint8_t edgeConfig) { valideEdge = edgeConfig; }
		uint8_t getValideEdge() { return (valideEdge); }
		void setOutputAction(uint8_t outputAction) { this->outputAction = outputAction; };
		uint8_t getOutputAction() { return (outputAction); };

		void notifyEdge() { edge_event.notify(sc_core::SC_ZERO_TIME); }

		virtual void VariableBaseNotify(const VariableBase *var) {
			if (ectParent->isInputCapture(ioc_index)) {

				// if edge sharing is enabled then channels [4,7] have been respectively stimulated by channels [0,3].
				if ((ioc_index > 3) && !ectParent->isSharingEdgeEnabled(1 << ioc_index)) {
					notifyEdge();
				}

			}
		}

	private:
		ECT	*ectParent;

		sc_event edge_event;
		sc_event shared_edge_event;

		uint8_t ioc_index;
		uint8_t iocMask;
		uint8_t valideEdge;
		uint8_t outputAction;

		uint16_t* tc_register_ptr;
		uint16_t* tch_register_ptr;
		PulseAccumulator8Bit* pulseAccumulator;
		bool* channelPinLogic;


	} *ioc_channel[8];

	class PulseAccumulator16Bit : public sc_module, public VariableBaseListener {
	public:
		PulseAccumulator16Bit(const sc_module_name& name, ECT *parent, bool* pinLogic, PulseAccumulator8Bit *pacn_high, PulseAccumulator8Bit *pacn_low);

		void process();
		void latchToHoldingRegisters();

		virtual void runPulseAccumulator() = 0;
		virtual void wakeup() = 0;
		void notifyEdge() { edge_event.notify(sc_core::SC_ZERO_TIME); }

		virtual void VariableBaseNotify(const VariableBase *var) = 0;

	protected:
		ECT	*ectParent;
		sc_event pulse_accumulator_enable_event;
		bool* channelPinLogic;

		PulseAccumulator8Bit *pacn_high_ptr;
		PulseAccumulator8Bit *pacn_low_ptr;

		sc_event edge_event;

	private:

	};

	class PulseAccumulatorA : public PulseAccumulator16Bit {
	public:
		PulseAccumulatorA(const sc_module_name& name, ECT *parent, bool* pinLogic, PulseAccumulator8Bit *pacn_high, PulseAccumulator8Bit *pacn_low);

		virtual void runPulseAccumulator();
		virtual void wakeup() {
			 if (ectParent->isPulseAccumulatorAEnabled()) {
				 pulse_accumulator_enable_event.notify(sc_core::SC_ZERO_TIME);
			 }
		}

		void setMode(bool mode) { isGatedTimeMode = mode; }
		bool isGatedTimeModeEnabled() { return (isGatedTimeMode); }
		void setGateTime(sc_time bus_cycle_time) { gate_time = bus_cycle_time * 64; }

		void setValideEdge(uint8_t edgeConfig) { valideEdge = edgeConfig; }
		uint8_t getValideEdge() { return (valideEdge); }

		virtual void VariableBaseNotify(const VariableBase *var) {
			// check pulse accumulator A enable
			if (ectParent->isPulseAccumulatorAEnabled()) {
				notifyEdge();
			}
		}


	protected:

	private:

		bool isGatedTimeMode;
		uint8_t valideEdge;
		sc_time gate_time;

	} *pacA;

	class PulseAccumulatorB : public PulseAccumulator16Bit {
	public:
		PulseAccumulatorB(const sc_module_name& name, ECT *parent, bool* pinLogic, PulseAccumulator8Bit *pacn_high, PulseAccumulator8Bit *pacn_low);

		virtual void runPulseAccumulator();
		virtual void wakeup() {
			 if (ectParent->isPulseAccumulatorBEnabled()) {
				 pulse_accumulator_enable_event.notify(sc_core::SC_ZERO_TIME);
			 }
		}

		virtual void VariableBaseNotify(const VariableBase *var) {
			// check pulse accumulator B enable
			if (ectParent->isPulseAccumulatorBEnabled()) {
				notifyEdge();
			}
		}

	protected:

	private:

	} *pacB;


}; /* end class ECT */

bool ECT::isInputCapture(uint8_t channel_index) {
	if ((tios_register & (1 << channel_index)) == 0) {
		return (true);
	} else {
		return (false);
	}
}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif /*__UNISIM_COMPONENT_TLM2_PROCESSOR_ECT_HH__*/
