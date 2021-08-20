/*
 * s12xeetx.hh
 *
 *  Created on: 10 oct. 2011
 *      Author: rnouacer
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XEETX_HH_
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XEETX_HH_

#include <queue>

#include <systemc>

#include <tlm>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "tlm_utils/simple_target_socket.h"

#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/kernel/tlm2/tlm.hh"

#include "unisim/service/interfaces/registers.hh"

#include "unisim/service/interfaces/register.hh"
#include "unisim/util/debug/simple_register.hh"
#include "unisim/util/endian/endian.hh"

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>

#include "unisim/component/tlm2/memory/ram/memory.hh"
#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>

#include <unisim/util/debug/simple_register_registry.hh>

#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace tlm;

using tlm_utils::tlm_quantumkeeper;

using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::Object;
using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Statistic;
using unisim::kernel::VariableBase;
using unisim::kernel::variable::CallBackObject;
using unisim::kernel::logger::Logger;

using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

using unisim::service::interfaces::Registers;

using unisim::service::interfaces::Register;
using unisim::util::debug::SimpleRegister;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::Host2BigEndian;


using unisim::component::tlm2::memory::ram::DEFAULT_BURST_LENGTH;
using unisim::component::tlm2::memory::ram::DEFAULT_BUSWIDTH;
using unisim::component::tlm2::memory::ram::DEFAULT_DEBUG;
using unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE;
using unisim::component::tlm2::memory::ram::DEFAULT_ADDRESS;

using unisim::component::cxx::processor::hcs12x::service_address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::CONFIG;

#define DEFAULT_CMD_PIPELINE_SIZE 2
#define SECTOR_SIZE 4
#define WORD_SIZE 2

template <unsigned int CMD_PIPELINE_SIZE = DEFAULT_CMD_PIPELINE_SIZE, unsigned int BUSWIDTH = DEFAULT_BUSWIDTH, class ADDRESS = DEFAULT_ADDRESS, unsigned int BURST_LENGTH = DEFAULT_BURST_LENGTH, uint32_t PAGE_SIZE = DEFAULT_PAGE_SIZE, bool DEBUG = DEFAULT_DEBUG>
class S12XEETX
	: public unisim::component::tlm2::memory::ram::Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>
	, public CallBackObject
	, public Service<Registers>
	, virtual public tlm_bw_transport_if<XINT_REQ_ProtocolTypes>

{
public:
	typedef unisim::component::tlm2::memory::ram::Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG> inherited;

	//=========================================================
	//=                REGISTERS OFFSETS                      =
	//=========================================================

	enum REGS_OFFSETS {ECLKDIV, RESERVED1, RESERVED2, ECNFG, EPROT, ESTAT, ECMD, RESERVED3, EADDRHI, EADDRLO, EDATAHI, EDATALO};

	static const unsigned int REGISTERS_BANK_SIZE = 12;

	ServiceExport<Registers> registers_export;

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	tlm_utils::simple_target_socket<S12XEETX> bus_clock_socket;

	// interface with bus
	tlm_utils::simple_target_socket<S12XEETX> slave_socket;

	/**
	 * Constructor.
	 *
	 * @param name the name of the module
	 * @param parent the parent service
	 */
	S12XEETX(const sc_module_name& name, Object *parent = 0);
	/**
	 * Destructor
	 */
	virtual ~S12XEETX();

	void assertInterrupt(uint8_t interrupt_offset);

	void Process();
	void updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay);

	sc_time getBusClock() { return bus_cycle_time; }

	void setEEPROMClock();

	void abort_write_sequence();
	void erase_verify_cmd();
	void word_program_cmd();
	void sector_erase_cmd();
	void mass_erase_cmd();
	void sector_erase_abort_cmd();
	void sector_modify_cmd();

	/* Service methods */
	/** BeginSetup
	 * Initializes the service interface. */
	virtual bool BeginSetup();

	virtual void Reset();

	/**
	 * TLM2 Slave methods
	 */
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	virtual tlm_sync_enum nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);

	void read_write( tlm::tlm_generic_payload& trans, sc_time& delay );

	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================

	virtual bool ReadMemory(ADDRESS addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(ADDRESS addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=             EEPROM Registers Interface interface methods               =
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

private:

	class  TCommand {
	public:
		TCommand() : cmd(0x00), addr(0x0000), data(0x0000), cmdWrite(false) { }
		~TCommand() {}

		void setCmd(uint8_t _cmd) { this->cmd = _cmd; cmdWrite = true; }
		uint8_t getCmd() { return (cmd); }
		void setAddr(uint16_t _addr) { this->addr = _addr; }
		uint16_t getAddr() { return (addr); }
		void setData(uint16_t _data) { this->data = _data; }
		uint16_t getData() { return (data); }
		bool isCmdWrite() { return (cmdWrite); }
		void invalidateCmdWrite() { cmdWrite = false; }

	private:
		uint8_t cmd;
		uint16_t addr;
		uint16_t data;
		bool    cmdWrite;
	} ;

	queue<TCommand*> cmd_queue;
	TCommand* cmd_queue_back;

	void write_to_eeprom(physical_address_t address, void* data_ptr, unsigned int data_length);
	void fetchCommand();
	void writeCmd(uint8_t _cmd);
	void empty_cmd_queue();
	void setACCERR() { estat_reg = estat_reg | 0x10; }

	void ComputeInternalTime();

	tlm_quantumkeeper quantumkeeper;

	PayloadFabric<XINT_Payload> xint_payload_fabric;
	XINT_Payload *xint_payload;

	double	bus_cycle_time_int;	// The time unit is PS
	Parameter<double>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;

	double	oscillator_cycle_time_int;
	Parameter<double>	param_oscillator_cycle_time_int;
	sc_time		oscillator_cycle_time;

	sc_time		eeclk_time;
	sc_time		min_eeclk_time;

	sc_event	command_launch_event;

	bool sector_erase_abort_active;
	bool sector_erase_modify_active;
	bool sector_erase_abort_command_req;

	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	uint64_t protected_area_start_address;

	uint8_t cmd_interruptOffset;
	Parameter<uint8_t> param_cmd_interruptOffset;

	double erase_fail_ratio;
	Parameter<double> param_erase_fail_ratio;

	// Registers map
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	std::vector<unisim::kernel::VariableBase*> extended_registers_registry;

	uint8_t eclkdiv_reg, reserved1_reg, reserved2_reg, ecnfg_reg, eprot_reg, estat_reg, ecmd_reg, reserved3_reg;
	uint16_t eaddr_reg, edata_reg;

};


} // end of hcs12x
} // end of processor
} // end of tlm2
} // end of component
} // end of unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XEETX_HH_ */
