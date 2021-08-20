/*
 *  Copyright (c) 2008,
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
 * This module implement the S12XINTV1 controler
 */

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_XINT_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_XINT_HH__

#include <inttypes.h>
#include <iostream>
#include <cmath>
#include <map>

#include <systemc>
#include "tlm_utils/tlm_quantumkeeper.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/multi_passthrough_target_socket.h"

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>

#include <unisim/kernel/kernel.hh>

#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/registers.hh"

#include "unisim/service/interfaces/register.hh"

#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>

#include <unisim/util/debug/simple_register_registry.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace tlm;
using namespace tlm_utils;

using unisim::component::cxx::processor::hcs12x::TOWNER;
using unisim::component::cxx::processor::hcs12x::address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::CONFIG;
using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::CallBackObject;
using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExportBase;

using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;

using unisim::service::interfaces::Register;

using unisim::component::tlm2::processor::hcs12x::XINT_REQ_ProtocolTypes;
using unisim::component::tlm2::processor::hcs12x::XINT_Payload;

using unisim::kernel::tlm2::PayloadFabric;


class XINT :
	public sc_module
	, public CallBackObject
	, public Service<Memory<physical_address_t> >
	, public Service<Registers>
	, public Client<Memory<physical_address_t> >
	, virtual public tlm_fw_transport_if<XINT_REQ_ProtocolTypes >

{
public:
	//========================================================
	//=   XINT Registers addresses and Reset Values          =
	//========================================================

	enum {IVBR,	INT_XGPRIO, INT_CFADDR, INT_CFDATA0, INT_CFDATA1, INT_CFDATA2, INT_CFDATA3, INT_CFDATA4, INT_CFDATA5, INT_CFDATA6, INT_CFDATA7 };

	static const uint8_t XINT_MEMMAP_SIZE = 11;
	static address_t XINT_REGS_ADDRESSES[XINT_MEMMAP_SIZE];
	static uint8_t XINT_REGS_RESET_VALUES[XINT_MEMMAP_SIZE];

	//==================================================================
	//=           XINT  Fixed Interrupt vectors Offsets                =
	//==================================================================

	static const uint8_t	INT_SYS_RESET_OFFSET			= 0xFE;
	static const uint8_t	INT_ILLEGAL_ACCESS_RESET_OFFSET	= 0xFE;
	static const uint8_t	INT_CLK_MONITOR_RESET_OFFSET	= 0xFC;
	static const uint8_t	INT_COP_WATCHDOG_RESET_OFFSET	= 0xFA;
	static const uint8_t	INT_TRAP_OFFSET					= 0xF8;
	static const uint8_t	INT_SWI_OFFSET					= 0xF6;
	static const uint8_t	INT_XIRQ_OFFSET					= 0xF4;
	static const uint8_t	INT_IRQ_OFFSET					= 0xF2;
	static const uint8_t	INT_RAM_ACCESS_VIOLATION_OFFSET	= 0x60;	// used for 68HCS12XD only
	static const uint8_t	INT_MPU_ACCESS_ERROR_OFFSET		= 0x14;	// used for 68HCS12XE only
	static const uint8_t	INT_SYSCALL_OFFSET				= 0x12;
	static const uint8_t	INT_SPURIOUS_OFFSET				= 0x10;

	static const unsigned int MEMORY_MAP_SIZE = 16;

	/*
	 * 0xFFFE				: pin reset, power-on reset, low-voltage reset, illegal address reset
	 * 0xFFFC				: clock monitor reset
	 * 0xFFFA				: COP watchdog reset
	 * (ivbr + 0x00F8)		: unimplemented opcode trap
	 * (ivbr + 0x00F6)		: software interrupt instruction (SWI) or BDM vector request
	 * (ivbr + 0x00F4)		: !XIRQ interrupt request
	 * (ivbr + 0x00F2)		: !IRQ interrupt request
	 * (ivbr +0x00F0-0x0012): Device specific I-bit maskable interrupt source
	 * (ivbr + 0x0010)		: Spurious interrupt
	 */

	// interface with bus
	tlm_target_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes,0> interrupt_request;


	// to connect to CPU
	tlm_utils::simple_initiator_socket<XINT> toCPU12X_request;
	tlm_utils::simple_initiator_socket<XINT> toXGATE_request;

	// interface with bus
	tlm_utils::simple_target_socket<XINT> slave_socket;

	ServiceExport<Memory<physical_address_t> > memory_export;
	ServiceImport<Memory<physical_address_t> > memory_import;
	ServiceExport<Registers> registers_export;

	XINT(const sc_module_name& name, Object *parent = 0);
	virtual ~XINT();

	virtual void Reset();
	
	void run(); // Priority Decoder and Interrupt selection

	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	virtual void ResetMemory();
	virtual bool ReadMemory(physical_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(physical_address_t addr, const void *buffer, uint32_t size);

    //================================================================
    //=                    tlm2 Interface                            =
    //================================================================

	// interrupt request from peripherals
    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	virtual unsigned int transport_dbg(XINT_Payload& payload);
	virtual tlm_sync_enum nb_transport_fw(XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport(XINT_Payload& payload, sc_core::sc_time& t);
	virtual bool get_direct_mem_ptr(XINT_Payload& payload, tlm_dmi&  dmi_data);

	// backward method for interrupts handling by CPU and XGATE
//	virtual tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& payload, tlm_phase& phase, sc_core::sc_time& t);
	tlm_sync_enum cpu_nb_transport_bw( tlm::tlm_generic_payload& payload, tlm_phase& phase, sc_core::sc_time& t);
	tlm_sync_enum xgate_nb_transport_bw( tlm::tlm_generic_payload& payload, tlm_phase& phase, sc_core::sc_time& t);


	virtual void read_write( tlm::tlm_generic_payload& trans, sc_time& delay );

	//=====================================================================
	//=             registers setters and getters                         =
	//=====================================================================
	virtual bool read(unsigned int address, const void *buffer, unsigned int data_length);
	virtual bool write(unsigned int address, const void *buffer, unsigned int data_length);

	/**
	 * Gets a register interface to the register specified by name.
	 *
	 * @param name The name of the requested register.
	 * @return A pointer to the RegisterInterface corresponding to name.
	 */
	virtual Register *GetRegister(const char *name);
	virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);

	//==============================================================
	//=              XINT Registers Access Routines                =
	//==============================================================

	uint8_t getIVBR();
	void setIVBR(uint8_t value);

	uint8_t	getINT_XGPRIO();
	void setINT_XGPRIO(uint8_t value);

	uint8_t	getINT_CFADDR();
	void setINT_CFADDR(uint8_t value);

	uint8_t	read_INT_CFDATA(uint8_t index);
	void write_INT_CFDATA(uint8_t index, uint8_t value);

protected:

private:
	static const uint8_t XINT_SIZE		= 128; // Number of recognized/handled interrupts
	static const uint8_t CFDATA_WINDOW_SIZE	= 8;

	PayloadFabric<tlm::tlm_generic_payload> payloadFabric;

	tlm::tlm_generic_payload* trans;;

	tlm_phase *phase;

	class PendingInterrupt {
	public:
		PendingInterrupt() : state(false), payload(0) {}
		~PendingInterrupt() { /*releasePayload();*/ }

		void setState(bool _state) { state = _state; }
		bool getState() { return (state); }
		void setPayload(XINT_Payload* _payload) { payload = _payload; }
		XINT_Payload getPayload() { return (*payload); }
		void releasePayload() { if (payload) {payload->reset(); payload->release(); payload = NULL; } }

	private:
		bool state;
		XINT_Payload* payload;

	} interrupt_flags[XINT_SIZE];

	sc_time zeroTime;

	peq_with_get<XINT_Payload> input_payload_queue;

	uint8_t ivbr;
	uint8_t	int_xgprio;
	uint8_t	int_cfaddr;
	uint8_t	*int_cfwdata;

	sc_event retry_event;

	bool	debug_enabled;
	Parameter<bool>	param_debug_enabled;

	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	// Registers map
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	std::vector<unisim::kernel::VariableBase*> extended_registers_registry;

	bool selectInterrupt(TOWNER::OWNER owner, INT_TRANS_T &buffer);

public:

	//==============================================
	//=              Interrupt Vectors             =
	//==============================================

	address_t get_SysReset_Vector() { return (0xFFFE); }
	address_t get_IllegalAccessReset_Vector() { return (0xFFFE); }
	address_t get_ClockMonitorReset_Vector() { return (0xFFFC); }
	address_t get_COPWatchdogReset_Vector() { return (0xFFFA); }
	address_t get_XIRQ_Vector() { return (((address_t) getIVBR() << 8) + INT_XIRQ_OFFSET); }
	address_t get_MPUACCESSERROR_Vector() { return (((address_t) getIVBR() << 8) + INT_MPU_ACCESS_ERROR_OFFSET); }

	address_t get_Spurious_Vector() { return (((address_t) getIVBR() << 8) + INT_SPURIOUS_OFFSET); } // Spurious interrupt

/*
	address_t get_Trap_Vector() { return ((address_t) getIVBR() << 8) + 0xF8; } // Shared interrupt vector for traps ($FFF8:$FFF9)
	address_t get_SWI_Vector() { return ((address_t) getIVBR() << 8) + 0xF6 ; }
	address_t get_IRQEN_Vector() { return ((address_t) getIVBR() << 8) + 0xF2 ; }

	address_t get_RTI_Vector() { return ((address_t) getIVBR() << 8) + 0xF0 ; } // Real Time Interrupt Vector
	address_t get_ECT_Ch0_Vector() { return ((address_t) getIVBR() << 8) + 0xEE ; } // Enhanced Capture Timer Channel 0
	address_t get_ECT_Ch1_Vector() { return ((address_t) getIVBR() << 8) + 0xEC ; } // Enhanced Capture Timer Channel 1
	address_t get_ECT_Ch2_Vector() { return ((address_t) getIVBR() << 8) + 0xEA ; } // Enhanced Capture Timer Channel 2
	address_t get_ECT_Ch3_Vector() { return ((address_t) getIVBR() << 8) + 0xE8 ; } // Enhanced Capture Timer Channel 3
	address_t get_ECT_Ch4_Vector() { return ((address_t) getIVBR() << 8) + 0xE6 ; } // Enhanced Capture Timer Channel 4
	address_t get_ECT_Ch5_Vector() { return ((address_t) getIVBR() << 8) + 0xE4 ; } // Enhanced Capture Timer Channel 5
	address_t get_ECT_Ch6_Vector() { return ((address_t) getIVBR() << 8) + 0xE2 ; } // Enhanced Capture Timer Channel 6
	address_t get_ECT_Ch7_Vector() { return ((address_t) getIVBR() << 8) + 0xE0 ; } // Enhanced Capture Timer Channel 7
	address_t get_ECT_Overflow_Vector() { return ((address_t) getIVBR() << 8) + 0xDE ; } // Enhanced capture Timer Overflow
	address_t get_PAcc_A_Overflow_Vector() { return ((address_t) getIVBR() << 8) + 0xDC ; } // Pulse Accumulator A Overflow
	address_t get_PAcc_Input_edge_Vector() { return ((address_t) getIVBR() << 8) + 0xDA ; } // Pulse Accumulator A Input Edge
	address_t get_SPI0_Vector() { return ((address_t) getIVBR() << 8) + 0xD8 ; }
	address_t get_SCI0_Vector() { return ((address_t) getIVBR() << 8) + 0xD6 ; }
	address_t get_SCI1_Vector() { return ((address_t) getIVBR() << 8) + 0xD4 ; }
	address_t get_ATD0_Vector() { return ((address_t) getIVBR() << 8) + 0xD2 ; }
	address_t get_ATD1_Vector() { return ((address_t) getIVBR() << 8) + 0xD0 ; }
	address_t get_PortJ_Vector() { return ((address_t) getIVBR() << 8) + 0xCE ; }
	address_t get_PortH_Vector() { return ((address_t) getIVBR() << 8) + 0xCC ; }
	address_t get_MDC_Underflow_Vector() { return ((address_t) getIVBR() << 8) + 0xCA ; } // Modulus Down Counter Underflow
	address_t get_PAcc_B_Overflow_Vector() { return ((address_t) getIVBR() << 8) + 0xC8 ; } // Pulse Accumulator B Overflow
	address_t get_CRGPLL_Lock_Vector() { return ((address_t) getIVBR() << 8) + 0xC6 ; }
	address_t get_CRG_SelfClockMode_Vector() { return ((address_t) getIVBR() << 8) + 0xC4 ; }
	address_t get_IIC0_Bus_Vector() { return ((address_t) getIVBR() << 8) + 0xC0 ; }
	address_t get_SPI1_Vector() { return ((address_t) getIVBR() << 8) + 0xBE ; }
	address_t get_SPI2_Vector() { return ((address_t) getIVBR() << 8) + 0xBC ; }
	address_t get_EEPROM_Vector() { return ((address_t) getIVBR() << 8) + 0xBA ; }
	address_t get_FLASH_Vector() { return ((address_t) getIVBR() << 8) + 0xB8 ; }
	address_t get_CAN0_WakeUp_Vector() { return ((address_t) getIVBR() << 8) + 0xB6 ; }
	address_t get_CAN0_Errors_Vector() { return ((address_t) getIVBR() << 8) + 0xB4 ; }
	address_t get_CAN0_Receive_Vector() { return ((address_t) getIVBR() << 8) + 0xB2 ; }
	address_t get_CAN0_Transmit_Vector() { return ((address_t) getIVBR() << 8) + 0xB0 ; }
	address_t get_CAN1_WakeUp_Vector() { return ((address_t) getIVBR() << 8) + 0xAE ; }
	address_t get_CAN1_Errors_Vector() { return ((address_t) getIVBR() << 8) + 0xAC ; }
	address_t get_CAN1_Receive_Vector() { return ((address_t) getIVBR() << 8) + 0xAA ; }
	address_t get_CAN1_Transmit_Vector() { return ((address_t) getIVBR() << 8) + 0xA8 ; }
	address_t get_CAN2_WakeUp_Vector() { return ((address_t) getIVBR() << 8) + 0xA6 ; }
	address_t get_CAN2_Errors_Vector() { return ((address_t) getIVBR() << 8) + 0xA4 ; }
	address_t get_CAN2_Receive_Vector() { return ((address_t) getIVBR() << 8) + 0xA2 ; }
	address_t get_CAN2_Transmit_Vector() { return ((address_t) getIVBR() << 8) + 0xA0 ; }
	address_t get_CAN3_WakeUp_Vector() { return ((address_t) getIVBR() << 8) + 0x9E ; }
	address_t get_CAN3_Errors_Vector() { return ((address_t) getIVBR() << 8) + 0x9C ; }
	address_t get_CAN3_Receive_Vector() { return ((address_t) getIVBR() << 8) + 0x9A ; }
	address_t get_CAN3_Transmit_Vector() { return ((address_t) getIVBR() << 8) + 0x98 ; }
	address_t get_CAN4_WakeUp_Vector() { return ((address_t) getIVBR() << 8) + 0x96 ; }
	address_t get_CAN4_Errors_Vector() { return ((address_t) getIVBR() << 8) + 0x94 ; }
	address_t get_CAN4_Receive_Vector() { return ((address_t) getIVBR() << 8) + 0x92 ; }
	address_t get_CAN4_Transmit_Vector() { return ((address_t) getIVBR() << 8) + 0x90 ; }
	address_t get_PortP_Vector() { return ((address_t) getIVBR() << 8) + 0x8E ; }
	address_t get_PWM_Shutdown_Vector() { return ((address_t) getIVBR() << 8) + 0x8C ; }
	address_t get_SCI2_Vector() { return ((address_t) getIVBR() << 8) + 0x8A ; }
	address_t get_SCI3_Vector() { return ((address_t) getIVBR() << 8) + 0x88 ; }
	address_t get_SCI4_Vector() { return ((address_t) getIVBR() << 8) + 0x86 ; }
	address_t get_SCI5_Vector() { return ((address_t) getIVBR() << 8) + 0x84 ; }
	address_t get_IIC1_Bus_Vector() { return ((address_t) getIVBR() << 8) + 0x82 ; }
	address_t get_LVI_Vector() { return ((address_t) getIVBR() << 8) + 0x80 ; }
	address_t get_API_Vector() { return ((address_t) getIVBR() << 8) + 0x7E ; }
	address_t get_PIT_Ch0_Vector() { return ((address_t) getIVBR() << 8) + 0x7A ; } // Periodic Interrupt Timer Channel 0
	address_t get_PIT_Ch1_Vector() { return ((address_t) getIVBR() << 8) + 0x78 ; } // Periodic Interrupt Timer Channel 1
	address_t get_PIT_Ch2_Vector() { return ((address_t) getIVBR() << 8) + 0x76 ; } // Periodic Interrupt Timer Channel 2
	address_t get_PIT_Ch3_Vector() { return ((address_t) getIVBR() << 8) + 0x74 ; } // Periodic Interrupt Timer Channel 3
	address_t get_XGATE_SW_Trigger0_Vector() { return ((address_t) getIVBR() << 8) + 0x72 ; }
	address_t get_XGATE_SW_Trigger1_Vector() { return ((address_t) getIVBR() << 8) + 0x70 ; }
	address_t get_XGATE_SW_Trigger2_Vector() { return ((address_t) getIVBR() << 8) + 0x6E ; }
	address_t get_XGATE_SW_Trigger3_Vector() { return ((address_t) getIVBR() << 8) + 0x6C ; }
	address_t get_XGATE_SW_Trigger4_Vector() { return ((address_t) getIVBR() << 8) + 0x6A ; }
	address_t get_XGATE_SW_Trigger5_Vector() { return ((address_t) getIVBR() << 8) + 0x68 ; }
	address_t get_XGATE_SW_Trigger6_Vector() { return ((address_t) getIVBR() << 8) + 0x66 ; }
	address_t get_XGATE_SW_Trigger7_Vector() { return ((address_t) getIVBR() << 8) + 0x64 ; }
	address_t get_XGATE_SW_Error_Vector() { return ((address_t) getIVBR() << 8) + 0x62 ; }
	address_t get_S12X_RAV_Vector() { return ((address_t) getIVBR() << 8) + 0x60 ; } // S12XCPU RAM Access Violation

	address_t get_Sys_Vector() { return ((address_t) getIVBR() << 8) + 0x12; } // System call interrupt vector is $FF12:$FF13

 */

}; /* end class XINT */

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif /*__UNISIM_COMPONENT_TLM2_PROCESSOR_XINT_HH__*/
