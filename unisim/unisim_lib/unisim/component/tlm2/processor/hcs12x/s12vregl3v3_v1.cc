/*
 * S12VREGL3V3_v1.cc
 *
 *  Created on: 8 août 2012
 *      Author: rnouacer
 */

#include <unisim/component/tlm2/processor/hcs12x/s12vregl3v3_v1.hh>
#include <unisim/component/tlm2/processor/hcs12x/xint.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

S12VREGL3V3::S12VREGL3V3(const sc_module_name& name, Object *parent) :
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

	, base_address(0x02F0)
	, param_base_address("base-address", this, base_address)

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

	, debug_enabled(false)
	, param_debug_enabled("debug-enabled", this, debug_enabled)

{

	slave_socket.register_b_transport(this, &S12VREGL3V3::read_write);
	bus_clock_socket.register_b_transport(this, &S12VREGL3V3::updateBusClock);

// 	SC_HAS_PROCESS(S12VREGL3V3);

//	SC_THREAD(TxRun);
//	SC_THREAD(RxRun);

}


S12VREGL3V3::~S12VREGL3V3() {

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

void S12VREGL3V3::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{

	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();
	unsigned int data_length = trans.get_data_length();

	if ((address >= base_address) && (address < base_address+MEMORY_MAP_SIZE)) {
		if (cmd == tlm::TLM_READ_COMMAND) {
			memset(data_ptr, 0, data_length);

			std::cout << "S12VREGL3V3::Warning: READ access to 0x" << std::hex << (address - base_address) << std::endl;

			read(address - base_address, data_ptr, data_length);
		} else if (cmd == tlm::TLM_WRITE_COMMAND) {

			std::cout << "S12VREGL3V3::Warning: WRITE access to 0x" << std::hex << (address - base_address) << std::endl;

			write(address - base_address, data_ptr, data_length);
		}

		trans.set_response_status( tlm::TLM_OK_RESPONSE );

		return;
	}

	trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
}


bool S12VREGL3V3::read(unsigned int offset, const void *buffer, unsigned int data_length) {

//	std::cout << sc_object::name() << "::read offset= " << std::dec << offset << "  size=" << data_length << std::endl;

	*((uint8_t *) buffer) = vreg_register[offset];

	return (true);
}

bool S12VREGL3V3::write(unsigned int offset, const void *buffer, unsigned int data_length) {

	//	std::cout << sc_object::name() << "::write offset= " << std::dec << offset << "  data=" << (unsigned int) *((uint8_t *) buffer) << std::endl;

	vreg_register[offset] = *((uint8_t *) buffer);

	return (true);
}



// Master methods

void S12VREGL3V3::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

void S12VREGL3V3::ComputeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

}


void S12VREGL3V3::updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}


//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================


bool S12VREGL3V3::BeginSetup() {

	Reset();

	ComputeInternalTime();

	return (true);
}


bool S12VREGL3V3::Setup(ServiceExportBase *srv_export) {

	return (true);
}


bool S12VREGL3V3::EndSetup() {
	return (true);
}


Register* S12VREGL3V3::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

void S12VREGL3V3::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}


void S12VREGL3V3::OnDisconnect() {
}


void S12VREGL3V3::Reset() {

	memset(vreg_register, 0, MEMORY_MAP_SIZE);
}

void S12VREGL3V3::ResetMemory() {
	
	Reset();

}

//=====================================================================
//=             memory interface methods                              =
//=====================================================================


bool S12VREGL3V3::ReadMemory(physical_address_t address, void *buffer, uint32_t size) {

	if ((address >= base_address) && (address < base_address+MEMORY_MAP_SIZE)) {

		*((uint8_t *) buffer) = vreg_register[address - base_address];

		return true;
	}

	return (false);
}


bool S12VREGL3V3::WriteMemory(physical_address_t address, const void *buffer, uint32_t size) {

	if ((address >= base_address) && (address < base_address+MEMORY_MAP_SIZE)) {

		vreg_register[address - base_address] = *((uint8_t *) buffer);

		return true;
	}

	return (false);
}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim






