#ifndef _TLE8264_2E_HH_
#define _TLE8264_2E_HH_

#include <systemc>
#include <inttypes.h>

#include <iostream>
#include <queue>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/parser.h>

#include <tlm.h>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

#include <unisim/kernel/kernel.hh>

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>

using namespace std;

using namespace sc_core;
using namespace sc_dt;
using namespace tlm;
using namespace tlm_utils;

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Signal;
using unisim::kernel::ServiceExportBase;

using unisim::component::cxx::processor::hcs12x::CONFIG;

using unisim::component::tlm2::processor::hcs12x::XINT_Payload;
using unisim::component::tlm2::processor::hcs12x::XINT_REQ_ProtocolTypes;

using unisim::component::tlm2::processor::hcs12x::CAN_Payload;
//using unisim::component::tlm2::processor::hcs12x::CAN_DATATYPE;
//using unisim::component::tlm2::processor::hcs12x::CAN_DATATYPE_ARRAY;
using unisim::component::tlm2::processor::hcs12x::UNISIM_CAN_ProtocolTypes;

using unisim::kernel::tlm2::PayloadFabric;

class TLE8264_2E :
		public Object
	, public sc_module

	, virtual public tlm_bw_transport_if<XINT_REQ_ProtocolTypes>
//	, virtual public tlm_fw_transport_if<UNISIM_CAN_ProtocolTypes >
//	, virtual public tlm_bw_transport_if<UNISIM_CAN_ProtocolTypes >
{
public:
	static const unsigned int INIT=0b000;
	static const unsigned int RESTART=0b001;
	static const unsigned int SWFLASH=0b010;
	static const unsigned int NORMAL=0b011;
	static const unsigned int SLEEP=0b100;
	static const unsigned int STOP=0b101;
	static const unsigned int FAILSAFE=0b110;
	static const unsigned int READONLY=0b111;

	//		static const uint16_t mask		= 0b00000001 00000000;

	// reg 0b000
	static const uint16_t WKCAN		= 0b0000000000000001;
	static const uint16_t WKLIN1	= 0b0000000000000010;
	static const uint16_t WKLIN2	= 0b0000000000000100;
	static const uint16_t WKLIN3	= 0b0000000000001000;
	static const uint16_t WK0PIN	= 0b0000000000010000;
	static const uint16_t WK1PIN	= 0b0000000000100000;
	static const uint16_t WKCYCLIC	= 0b0000000001000000;
	static const uint16_t INT		= 0b0000000100000000;

	// reg 0b001
	static const uint16_t RESET		= 0b0000000010000000;
	static const uint16_t FAILSPI	= 0b0000000001000000;

	// reg 0b100
	static const uint16_t RESETDELAY= 0b0000000000000100;
	static const uint16_t WKPIN		= 0b0000000000010000;
	static const uint16_t LIMPHOME	= 0b0000000001000000;
	static const uint16_t CYCLICWK	= 0b0000000010000000;

	// reg 0b110
	static const uint16_t CHKSUM	= 0b0000000100000000;
	static const uint16_t WDONOFF	= 0b0000000010000000;
	static const uint16_t TIOUTWIN	= 0b0000000001000000;
	static const uint16_t SETTO1	= 0b0000000000100000;
	static const uint16_t TIMEBITS	= 0b0000000000011111;

	static const uint16_t WK_STATE_DEFAULT		= 0b1000000000;
	static const uint16_t WD_REFRESH_DEFAULT	= 0b0000000000;
	static const uint16_t REG_000_MSK_DEFAULT	= 0b0000111111;
	static const uint16_t REG_001_MSK_DEFAULT	= 0b0111111111;
	static const uint16_t REG_010_MSK_DEFAULT	= 0b0010101101;
	static const uint16_t REG_000_DEFAULT		= 0b0000000000;
	static const uint16_t REG_001_DEFAULT 		= 0b0000000000;
	static const uint16_t REG_010_DEFAULT 		= 0b0000000000;
	static const uint16_t REG_011_DEFAULT 		= 0b0000000000; // input-output reserved register
	static const uint16_t REG_100_DEFAULT 		= 0b0100010101;
	static const uint16_t REG_101_DEFAULT 		= 0b0100000000;
	static const uint16_t REG_110_DEFAULT 		= 0b0111111111;
	static const uint16_t REG_111_DEFAULT 		= 0b0000000000; // input reserved register

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	tlm_utils::simple_target_socket<TLE8264_2E> bus_clock_socket;

	tlm_utils::simple_target_socket<TLE8264_2E, CAN_BUS_WIDTH, UNISIM_CAN_ProtocolTypes > can_slave_rx_sock;
	tlm_utils::simple_initiator_socket<TLE8264_2E, CAN_BUS_WIDTH, UNISIM_CAN_ProtocolTypes > can_slave_tx_sock;

	tlm_utils::simple_target_socket<TLE8264_2E, CAN_BUS_WIDTH, UNISIM_CAN_ProtocolTypes > can_master_rx_sock;
	tlm_utils::simple_initiator_socket<TLE8264_2E, CAN_BUS_WIDTH, UNISIM_CAN_ProtocolTypes > can_master_tx_sock;

	tlm_utils::simple_initiator_socket<TLE8264_2E> spi_tx_socket;
	tlm_utils::simple_target_socket<TLE8264_2E> spi_rx_socket;

	TLE8264_2E(const sc_module_name& name, Object *parent = 0);
	~TLE8264_2E();

	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	virtual void OnDisconnect();
	virtual void Reset();

	virtual void Stop(int exit_status);

	bool isTerminated() { return terminated; }
	void ComputeInternalTime();

	void WatchDogThread();
	void stateMachineThread();

	void spi_rx_b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);

	// Slave methods
	virtual tlm_sync_enum nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr( sc_dt::uint64 start_range, sc_dt::uint64 end_range);
    void updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay);

    /** CAN slave interface micro->tle **/
    // Slave methods
	virtual bool can_slave_get_direct_mem_ptr( CAN_Payload& payload, tlm_dmi&  dmi_data);
	virtual unsigned int can_slave_transport_dbg( CAN_Payload& payload);

	virtual tlm_sync_enum can_slave_nb_transport_fw( CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual void can_slave_b_transport( CAN_Payload& payload, sc_core::sc_time& t);
	// Master methods
	virtual tlm_sync_enum can_slave_nb_transport_bw( CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual void can_slave_invalidate_direct_mem_ptr( sc_dt::uint64 start_range, sc_dt::uint64 end_range);

    /** CAN initiator interface tle->can_bus **/
	// Slave methods
	virtual bool can_master_get_direct_mem_ptr( CAN_Payload& payload, tlm_dmi&  dmi_data);
	virtual unsigned int can_master_transport_dbg( CAN_Payload& payload);

	virtual tlm_sync_enum can_master_nb_transport_fw( CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual void can_master_b_transport( CAN_Payload& payload, sc_core::sc_time& t);
	// Master methods
	virtual tlm_sync_enum can_master_nb_transport_bw( CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual void can_master_invalidate_direct_mem_ptr( sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	void setLimpHome(bool val);
	bool isLimpHome();

	bool isINT();
	bool isWKCyclicEnabled();

	void refresh_watchdog();

	void execute_init(/*uint16_t sel*/);
	void execute_restart(/*uint16_t sel*/);
	void execute_swflash(/*uint16_t sel*/);
	void execute_normal(/*uint16_t sel*/);
	void execute_sleep(/*uint16_t sel*/);
	void execute_stop(/*uint16_t sel*/);
	void execute_sailsafe(/*uint16_t sel*/);
	uint16_t execute_readonly(uint16_t sel);

	void assertInterrupt(uint8_t int_offset);

	void assert_int_interrup();
	void assert_reset_interrupt();

	uint16_t triggerStateMachine(uint16_t spi_cmd);
	void setCmd(uint16_t cmd);

	uint16_t write(uint16_t sel, uint16_t val);
	void read(uint16_t sel, uint16_t& val);

private:
//	peq_with_get<CAN_Payload > rx_payload_queue;

	sc_event slave_rx_event, master_rx_event;

	PayloadFabric<CAN_Payload > payload_fabric;

	tlm_quantumkeeper quantumkeeper;
	PayloadFabric<XINT_Payload> xint_payload_fabric;

	XINT_Payload *xint_payload;

	PayloadFabric<tlm::tlm_generic_payload> spi_payload_fabric;

	double	bus_cycle_time_int;
	Parameter<double>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;


	uint16_t current_mode;
	uint16_t current_cmd;
	uint16_t last_state;

	bool wd_refresh;
	uint16_t wk_state_register;
	uint16_t mask_registers[3]; // 9lsb are mask bits and the 10th bit is wd_refresh
	uint16_t status_registers[3]; // 9lsb are status bits and the 10th bit is wk_state
	uint16_t reserved;
	uint16_t cfg_registers[8]; // 9-bits by register

	uint16_t spi_rx_buffer;


	bool SPIWake_read;

	bool terminated;

	sc_time frame_time;
	sc_event watchdog_enable_event;
	sc_event state_machine_event;

	bool limp_home;
	Signal<bool> sig_limp_home;

	// Interrupt ID
	uint8_t reset_interrupt;
	Parameter<uint8_t> param_reset_interrupt;
	uint8_t int_interrupt;
	Parameter<uint8_t> param_int_interrupt;

	bool debug_enabled;
	Parameter<bool> param_debug_enabled;

	// status 000
	bool isIntWKCANEnabled() { return ((mask_registers[0] & 0x0001) != 0); }
	bool isIntWKLINxEnabled(int num) { assert(num > 0); return ((mask_registers[0] & (0x0001 << num)) != 0); }
	bool isIntWKPINEnabled() { return ((mask_registers[0] & 0x0030) != 0); }

	// status 001
	bool isIntOTPVEnabled() { return ((mask_registers[1] & 0x0001) != 0); }
	bool isIntOTHSCANEnabled() { return ((mask_registers[1] & 0x0002) != 0); }
	bool isIntOTVenabled() { return ((mask_registers[1] & 0x0004) != 0); }
	bool isIntUVVCC2Enabled() { return ((mask_registers[1] & 0x0008) != 0); }
	bool isIntUVVCC3Enabled() { return ((mask_registers[1] & 0x0010) != 0); }
	bool isIntICC3Enabled() { return ((mask_registers[1] & 0x0020) != 0); }
	bool isIntSPIFailEnabled() { return ((mask_registers[1] & 0x0040) != 0); }
	bool isIntResetEnabled() { return ((mask_registers[1] & 0x0080) != 0); }
	bool isIntWringWDEnabled() { return ((mask_registers[1] & 0x0100) != 0); }

	// status 010
	bool isIntCANTxDFailureEnabled() { return ((mask_registers[2] & 0x0001) != 0); }
	bool isIntCANRxDFailureEnabled() { return ((mask_registers[2] & 0x0002) != 0); }
	bool isIntCANTxDRxDFailureEnabled() { return ((mask_registers[2] & 0x0003) != 0); }

	bool isIntCANBusFailureEnabled() { return ((mask_registers[2] & 0x0004) != 0); }

	bool isIntLIN1TxDFailureEnabled(int num) { return ((mask_registers[2] & 0x0008) != 0); }
	bool isIntLIN1RxDFailureEnabled(int num) { return ((mask_registers[2] & 0x0010) != 0); }
	bool isIntLIN1TxDRxDFailureEnabled(int num) { return ((mask_registers[2] & 0x0018) != 0); }

	bool isIntLIN2TxDFailureEnabled(int num) { return ((mask_registers[2] & 0x0020) != 0); }
	bool isIntLIN2RxDFailureEnabled(int num) { return ((mask_registers[2] & 0x0040) != 0); }
	bool isIntLIN2TxDRxDFailureEnabled(int num) { return ((mask_registers[2] & 0x0060) != 0); }

	bool isIntLIN3TxDFailureEnabled(int num) { return ((mask_registers[2] & 0x0080) != 0); }
	bool isIntLIN3RxDFailureEnabled(int num) { return ((mask_registers[2] & 0x0100) != 0); }
	bool isIntLIN3TxDRxDFailureEnabled(int num) { return ((mask_registers[2] & 0x0180) != 0); }

	// TODO: cfg 100

	// cfg 101
	bool isLIN1WakeCapable() { return ((cfg_registers[1] & 0x0001) != 0); }
	bool isLIN1ReceiveOnly() { return ((cfg_registers[1] & 0x0002) != 0); }
	bool isLIN1NormalMode() { return ((cfg_registers[1] & 0x0003) != 0); }

	bool isLIN2WakeCapable() { return ((cfg_registers[1] & 0x0004) != 0); }
	bool isLIN2ReceiveOnly() { return ((cfg_registers[1] & 0x0008) != 0); }
	bool isLIN2NormalMode() { return ((cfg_registers[1] & 0x000C) != 0); }

	bool isLIN3WakeCapable() { return ((cfg_registers[1] & 0x0010) != 0); }
	bool isLIN3ReceiveOnly() { return ((cfg_registers[1] & 0x0020) != 0); }
	bool isLIN3NormalMode() { return ((cfg_registers[1] & 0x0030) != 0); }

	bool isCANWakeCapable() { return ((cfg_registers[1] & 0x0040) != 0); }
	bool isCANReceiveOnly() { return ((cfg_registers[1] & 0x0080) != 0); }
	bool isCANNormalMode() { return ((cfg_registers[1] & 0x00C0) != 0); }

	// cfg 110
	bool isTimeOutActivated() { return ((cfg_registers[2] & 0x0040) !=0); }
	bool isWatchDogEnable() { return ((cfg_registers[2] & WDONOFF) != 0); }
	void computeWDTimeer() {
		int index = cfg_registers[2] & 0x001F;
		frame_time = (index < 0x10)? sc_time((index + 1) * 16, SC_MS) : sc_time(index * 48 - 464, SC_MS);
	}
};

#endif // _TLE8264_2E_HH_

