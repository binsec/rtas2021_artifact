/*
 * RESERVED.cc
 *
 *  Created on: 8 août 2012
 *      Author: rnouacer
 */

#include <unisim/component/tlm2/processor/hcs12x/reserved.hh>
#include <unisim/component/tlm2/processor/hcs12x/xint.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

address_t const RESERVED::ADDRESS_SPACE[RESERVED::ADDRESS_ARRAY_SIZE][2] = {{0x0018,0x0019}, {0x0030,0x0031}, {0x00E8,0x00EF}, {0x00E8,0x00EF}, {0x02F8,0x02FF}, {0x0328,0x032F}, {0x03C0,0x03CF}, {0x0400,0x07FF} };

RESERVED::RESERVED(const sc_module_name& name, Object *parent) :
	Object(name, parent)
	, sc_module(name)

	, unisim::kernel::Client<TrapReporting>(name, parent)
	, unisim::kernel::Service<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::Service<Registers>(name, parent)
	, unisim::kernel::Client<Memory<physical_address_t> >(name, parent)

	, trap_reporting_import("trap_reporting_import", this)

	, memory_export("memory_export", this)
	, memory_import("memory_import", this)
	, registers_export("registers_export", this)

	, slave_socket("slave_socket")
	, bus_clock_socket("bus_clock_socket")

	, logger(*this)

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

	, debug_enabled(false)
	, param_debug_enabled("debug-enabled", this, debug_enabled)


{

	slave_socket.register_b_transport(this, &RESERVED::read_write);
	bus_clock_socket.register_b_transport(this, &RESERVED::updateBusClock);

// 	SC_HAS_PROCESS(RESERVED);

//	SC_THREAD(TxRun);
//	SC_THREAD(RxRun);

}


RESERVED::~RESERVED() {

	// Release registers_registry
	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}
}



//=====================================================================
//=             registers setters and getters                         =
//=====================================================================

void RESERVED::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{

	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();
	unsigned int data_length = trans.get_data_length();

	for (unsigned int i=0; i<ADDRESS_ARRAY_SIZE; i++) {
		if ((address >= ADDRESS_SPACE[i][0]) && (address <= ADDRESS_SPACE[i][1])) {
			if (cmd == tlm::TLM_READ_COMMAND) {
				memset(data_ptr, 0, data_length);

	//			std::cerr << "RESERVED::Warning: READ access to 0x" << std::hex << (address - baseAddress) << std::endl;

				read(address - ADDRESS_SPACE[i][0], data_ptr, data_length);
			} else if (cmd == tlm::TLM_WRITE_COMMAND) {

	//			std::cerr << "RESERVED::Warning: WRITE access to 0x" << std::hex << (address - baseAddress) << std::endl;

				write(address - ADDRESS_SPACE[i][0], data_ptr, data_length);
			}

			trans.set_response_status( tlm::TLM_OK_RESPONSE );

			return;
		}
	}

	trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
}


bool RESERVED::read(unsigned int offset, const void *buffer, unsigned int data_length) {

//	std::cout << sc_object::name() << "::read offset= " << std::dec << offset << "  size=" << data_length << std::endl;

//	*((uint8_t *) buffer) = reserved_register[offset];

	return (true);
}

bool RESERVED::write(unsigned int offset, const void *buffer, unsigned int data_length) {

	//	std::cout << sc_object::name() << "::write offset= " << std::dec << offset << "  data=" << (unsigned int) *((uint8_t *) buffer) << std::endl;

//	reserved_register[offset] = *((uint8_t *) buffer);

	return (true);
}



// Master methods

void RESERVED::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

void RESERVED::ComputeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

}


void RESERVED::updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}


//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================


bool RESERVED::BeginSetup() {

	Reset();

	ComputeInternalTime();

	return (true);
}


bool RESERVED::Setup(ServiceExportBase *srv_export) {

	return (true);
}


bool RESERVED::EndSetup() {
	return (true);
}


Register* RESERVED::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

void RESERVED::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

void RESERVED::OnDisconnect() {
}


void RESERVED::Reset() {

//	memset(reserved_register, 0, MEMORY_MAP_SIZE);
}

void RESERVED::ResetMemory() {

	Reset();

}

//=====================================================================
//=             memory interface methods                              =
//=====================================================================


bool RESERVED::ReadMemory(physical_address_t address, void *buffer, uint32_t size) {

	for (unsigned int i=0; i<ADDRESS_ARRAY_SIZE; i++) {
		if ((address >= ADDRESS_SPACE[i][0]) && (address <= ADDRESS_SPACE[i][1])) {

//			*((uint8_t *) buffer) = reserved_register[address - ADDRESS_SPACE[i][0]];

			return true;
		}
	}

	return (false);
}


bool RESERVED::WriteMemory(physical_address_t address, const void *buffer, uint32_t size) {

	for (unsigned int i=0; i<ADDRESS_ARRAY_SIZE; i++) {
		if ((address >= ADDRESS_SPACE[i][0]) && (address <= ADDRESS_SPACE[i][1])) {

//			reserved_register[address - ADDRESS_SPACE[i][0]] = *((uint8_t *) buffer);

			return true;
		}
	}

	return (false);
}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim






