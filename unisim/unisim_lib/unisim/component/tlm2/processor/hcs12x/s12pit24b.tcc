/*
 * s12pit24b.tcc
 *
 *  Created on: 3 août 2012
 *      Author: rnouacer
 */

#include <unisim/component/tlm2/processor/hcs12x/s12pit24b.hh>
#include <unisim/component/tlm2/processor/hcs12x/xint.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

template <uint8_t PIT_SIZE>
S12PIT24B<PIT_SIZE>::S12PIT24B(const sc_module_name& name, Object *parent) :
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

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

	, baseAddress(0x0340)
	, param_baseAddress("base-address", this, baseAddress)

	, param_interrupt_offset_channel("interrupt-offset-channel", this, interrupt_offset_channel, PIT_SIZE)

	, debug_enabled(false)
	, param_debug_enabled("debug-enabled", this, debug_enabled)

	, pitcflmt_register(0x00)
	, pitflt_register(0x00)
	, pitce_register(0x00)
	, pitmux_register(0x00)
	, pitinte_register(0x00)
	, pittf_register(0x00)
	, pitmtld0_register(0x00)
	, pitmtld1_register(0x0000)

{


	interrupt_request(*this);

	slave_socket.register_b_transport(this, &S12PIT24B::read_write);
	bus_clock_socket.register_b_transport(this, &S12PIT24B::updateBusClock);

//	SC_HAS_PROCESS(S12PIT24B);

	char counterName[20];

	for (uint8_t i=0; i<PIT_SIZE; i++) {

		sprintf (counterName, "CNT%d", i);

		counter[i] = new CNT16(counterName, this, i, &pitcnt_register[i], &pitld_register[i]);
	}

	xint_payload = xint_payload_fabric.allocate();

}

template <uint8_t PIT_SIZE>
S12PIT24B<PIT_SIZE>::~S12PIT24B() {

	xint_payload->release();

	// Release registers_registry
	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

	for (uint8_t i=0; i<PIT_SIZE; i++) {

		if (counter[i]) { delete counter[i]; counter[i] = NULL;}
	}

}

template <uint8_t PIT_SIZE>
S12PIT24B<PIT_SIZE>::CNT16::CNT16(const sc_module_name& name, S12PIT24B *_parent, uint8_t _index, uint16_t* _counter_register, uint16_t* _load_register) :
		sc_module(name), parent(_parent), index(_index), counter_register(_counter_register), load_register(_load_register), isEnabled(false)
{
	SC_HAS_PROCESS(CNT16);
	SC_THREAD(process);
}

template <uint8_t PIT_SIZE>
void S12PIT24B<PIT_SIZE>::CNT16::process() {

	while (true) {

		while ((!isEnabled) || !parent->isPITEnabled()) {
			wait(start_event);
		}

		*counter_register = *load_register;
		wait(period);
		while ((isEnabled) && parent->isPITEnabled() && (*counter_register != 0)) {

			*counter_register = *counter_register - 1;
			wait(period);
		}

		parent->setTimeoutFlag(index);

	}
}

template <uint8_t PIT_SIZE>
void S12PIT24B<PIT_SIZE>::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();
	unsigned int data_length = trans.get_data_length();

	if ((address >= baseAddress) && (address < (baseAddress + REGISTERS_BANK_SIZE))) {

		if (cmd == tlm::TLM_READ_COMMAND) {
			memset(data_ptr, 0, data_length);
			read(address - baseAddress, data_ptr, data_length);
		} else if (cmd == tlm::TLM_WRITE_COMMAND) {
			write(address - baseAddress, data_ptr, data_length);
		}

		trans.set_response_status( tlm::TLM_OK_RESPONSE );

	} else {
		trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
	}
}

template <uint8_t PIT_SIZE>
bool S12PIT24B<PIT_SIZE>::read(unsigned int offset, const void *buffer, unsigned int data_length) {

	switch (offset) {
	case PITCFLMT:	*((uint8_t *) buffer) = pitcflmt_register & 0xE0; break;	// 1 byte
	case PITFLT: *((uint8_t *) buffer) = 0x00; break;	// PITFLT read 0x00
	case PITCE: *((uint8_t *) buffer) = pitce_register; break;	// 1 byte
	case PITMUX: *((uint8_t *) buffer) = pitmux_register; break;	// 1 byte
	case PITINTE: *((uint8_t *) buffer) = pitinte_register; break;	// 1 byte
	case PITTF: *((uint8_t *) buffer) = pittf_register; break; // 1 byte
	case PITMTLD0: *((uint8_t *) buffer) = pitmtld0_register; break; // 1 byte
	case PITMTLD1: *((uint8_t *) buffer) = pitmtld1_register; break; // 1 byte
	default: {
		if ((offset >= PITLD0) && (offset < (PITLD0 + PIT_SIZE*4))) {
			uint8_t group_index = (offset - PITLD0) / 2;
			uint8_t group_register = (offset - PITLD0) % 2;
			if (group_register == 0) {
				*((uint16_t *) buffer) = Host2BigEndian(pitld_register[group_index]);
			} else {
				*((uint16_t *) buffer) = Host2BigEndian(pitcnt_register[group_index]);
			}
		}
		else {
			memset((void*) buffer, 0, data_length);
		}
	} break;

	}

	return (true);
}

//=====================================================================
//=             registers setters and getters                         =
//=====================================================================

template <uint8_t PIT_SIZE>
bool S12PIT24B<PIT_SIZE>::write(unsigned int offset, const void *buffer, unsigned int data_length) {

	switch (offset) {
	case PITCFLMT:	{
		if (data_length == 1) {
			uint8_t val = *((uint8_t *) buffer) & 0xE3;
			pitcflmt_register = val;	// 1 byte
		}
		else if (data_length == 2) 	{
			/**
			 * Any group of timers and micro timers can be restarted at the same time by using one16-bit write
			 * to the adjacent PITCFLMT and PITFLT registers with the relevant bits set.
			 */
			uint16_t val = BigEndian2Host(*((uint16_t *) buffer)) & 0xE3FF;
			pitcflmt_register = (val >> 8);	// high byte to PITCFLMT
			pitflt_register = (val & 0x00FF); // low byte to PITFLT

			uint8_t mask = 0x01;
			for (uint8_t i=0; (i < PIT_SIZE) && isPITEnabled(); i++) {
				if ((pitflt_register & mask) != 0) {
					counter[i]->load();
				}

				mask = mask << 1;
			}
		}

		uint8_t mask = 0x01;
		for (uint8_t i=0; (i < 2) && isPITEnabled(); i++) {
			if ((pitcflmt_register & mask) != 0) {
				updateMicroCouter(i);
			}

			mask = mask << 1;
		}

		if (!isPITEnabled()) {
			for (uint8_t i=0; i < PIT_SIZE; i++) {
				counter[i]->disable();
			}
			pittf_register = 0;
		}
	} break;
	case PITFLT: {
		uint8_t val = *((uint8_t *) buffer);
		pitflt_register = val; // 1 byte
		uint8_t mask = 0x01;
		for (uint8_t i=0; (i < PIT_SIZE) && isPITEnabled(); i++) {
			if ((pitflt_register & mask) != 0) {
				counter[i]->load();
			}

			mask = mask << 1;
		}
	} break;
	case PITCE: {
		uint8_t val = *((uint8_t *) buffer);
		pitce_register = val; // 1 byte
		uint8_t mask = 0x01;
		for (uint8_t i=0; i < PIT_SIZE; i++) {
			if ((pitce_register & mask) == 0) {
				pittf_register = pittf_register & ~mask;
				counter[i]->disable();
			} else {
				counter[i]->enable();
			}

			mask = mask << 1;
		}
	} break;
	case PITMUX: {
		uint8_t val = *((uint8_t *) buffer);
		pitmux_register = val; // 1 byte
		uint8_t mask = 0x01;
		for (uint8_t i=0; i < PIT_SIZE; i++) {
			if ((pitmux_register & mask) != 0) {
				counter[i]->setPeriod(periods[1]);
			} else {
				counter[i]->setPeriod(periods[0]);
			}

			mask = mask << 1;
		}

	} break;
	case PITINTE: {
		uint8_t val = *((uint8_t *) buffer);
		pitinte_register = val; // 1 byte
	} break;
	case PITTF: {
		uint8_t val = *((uint8_t *) buffer);
		pittf_register = pittf_register & ~val; // 1 byte
	} break;
	case PITMTLD0: {
		uint8_t val = *((uint8_t *) buffer);
		pitmtld0_register = val; // 1 byte
		updateMicroCouter(0);
	} break;
	case PITMTLD1: {
		uint8_t val = *((uint8_t *) buffer);
		pitmtld1_register = val; // 1 byte
		updateMicroCouter(1);
	} break;
	default: {
		if ((offset >= PITLD0) && (offset < (PITLD0 + PIT_SIZE*4))) {
			uint8_t group_index = (offset - PITLD0) / 2;
			uint8_t group_register = (offset - PITLD0) % 2;
			uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
			if (group_register == 0) {
				pitld_register[group_index] = val;
			} else {
				// write to PITCNTx registers has no meaning or effect
//				pitcnt_register[group_index] = val;
			}
		}
	} break;

	}

	return (true);
}

template <uint8_t PIT_SIZE>
void S12PIT24B<PIT_SIZE>::updateMicroCouter(uint8_t index) {

	if (index == 0) {
		periods[index] = bus_cycle_time * (pitmtld0_register + 1);
	} else {
		periods[index] = bus_cycle_time * (pitmtld1_register + 1);
	}

	uint8_t mask = 0x01;
	for (uint8_t i=0; i < PIT_SIZE; i++) {
		if (((pitmux_register & mask) != 0) && (index == 1)) {
			counter[i]->setPeriod(periods[1]);
		} else {
			counter[i]->setPeriod(periods[0]);
		}

		mask = mask << 1;
	}

}

template <uint8_t PIT_SIZE>
void S12PIT24B<PIT_SIZE>::setTimeoutFlag(uint8_t index) {
	pittf_register = pittf_register | (1 << index);

	if ((pitinte_register & (1 << index)) != 0) {
		assertInterrupt(interrupt_offset_channel[index]);
	}
}


template <uint8_t PIT_SIZE>
void S12PIT24B<PIT_SIZE>::assertInterrupt(uint8_t interrupt_offset) {

	tlm_phase phase = BEGIN_REQ;

	xint_payload->acquire();

	xint_payload->setInterruptOffset(interrupt_offset);

	sc_time local_time = quantumkeeper.get_local_time();

	tlm_sync_enum ret = interrupt_request->nb_transport_fw(*xint_payload, phase, local_time);

	xint_payload->release();

	switch(ret)
	{
		case TLM_ACCEPTED:
			// neither payload, nor phase and local_time have been modified by the callee
			quantumkeeper.sync(); // synchronize to leave control to the callee
			break;
		case TLM_UPDATED:
			// the callee may have modified 'payload', 'phase' and 'local_time'
			quantumkeeper.set(local_time); // increase the time
			if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed

			break;
		case TLM_COMPLETED:
			// the callee may have modified 'payload', and 'local_time' ('phase' can be ignored)
			quantumkeeper.set(local_time); // increase the time
			if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed
			break;
	}

}

// Master methods
template <uint8_t PIT_SIZE>
void S12PIT24B<PIT_SIZE>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

// Master methods
template <uint8_t PIT_SIZE>
tlm_sync_enum S12PIT24B<PIT_SIZE>::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}

template <uint8_t PIT_SIZE>
void S12PIT24B<PIT_SIZE>::ComputeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

	updateMicroCouter(0);
	updateMicroCouter(1);
}

template <uint8_t PIT_SIZE>
void S12PIT24B<PIT_SIZE>::updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}


//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================

template <uint8_t PIT_SIZE>
bool S12PIT24B<PIT_SIZE>::BeginSetup() {

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PITCFLMT", &pitcflmt_register));

	unisim::kernel::variable::Register<uint8_t> *pitcflmt_var = new unisim::kernel::variable::Register<uint8_t>("PITCFLMT", this, pitcflmt_register, "PIT Control and Force Load Micro Timer Register (PITCFLMT)");
	extended_registers_registry.push_back(pitcflmt_var);
	pitcflmt_var->setCallBack(this, PITCFLMT, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PITFLT", &pitflt_register));

	unisim::kernel::variable::Register<uint8_t> *pitflt_var = new unisim::kernel::variable::Register<uint8_t>("PITFLT", this, pitflt_register, "PIT Force Load Timer (PITFLT)");
	extended_registers_registry.push_back(pitflt_var);
	pitflt_var->setCallBack(this, PITFLT, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PITCE", &pitce_register));

	unisim::kernel::variable::Register<uint8_t> *pitce_var = new unisim::kernel::variable::Register<uint8_t>("PITCE", this, pitce_register, "PIT Channel Enable register (PITCE)");
	extended_registers_registry.push_back(pitce_var);
	pitce_var->setCallBack(this, PITCE, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PITMUX", &pitmux_register));

	unisim::kernel::variable::Register<uint8_t> *pitmux_var = new unisim::kernel::variable::Register<uint8_t>("PITMUX", this, pitmux_register, "PIT Multiplex register (PITMUX)");
	extended_registers_registry.push_back(pitmux_var);
	pitmux_var->setCallBack(this, PITMUX, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PITINTE", &pitinte_register));

	unisim::kernel::variable::Register<uint8_t> *pitinte_var = new unisim::kernel::variable::Register<uint8_t>("PITINTE", this, pitinte_register, "PIT Interrupt Enable register (PITINTE)");
	extended_registers_registry.push_back(pitinte_var);
	pitinte_var->setCallBack(this, PITINTE, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PITTF", &pittf_register));

	unisim::kernel::variable::Register<uint8_t> *pittf_var = new unisim::kernel::variable::Register<uint8_t>("PITTF", this, pittf_register, "PIT Time-out Flag Register (PITTF)");
	extended_registers_registry.push_back(pittf_var);
	pittf_var->setCallBack(this, PITTF, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PITMTLD0", &pitmtld0_register));

	unisim::kernel::variable::Register<uint8_t> *pitmtld0_var = new unisim::kernel::variable::Register<uint8_t>("PITMTLD0", this, pitmtld0_register, "PIT Micro Timer Load Register 0 (PITMTLD0)");
	extended_registers_registry.push_back(pitmtld0_var);
	pitmtld0_var->setCallBack(this, PITMTLD0, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PITMTLD1", &pitmtld1_register));

	unisim::kernel::variable::Register<uint8_t> *pitmtld1_var = new unisim::kernel::variable::Register<uint8_t>("PITMTLD1", this, pitmtld1_register, "PIT Micro Timer Load register 1 (PITMTLD1)");
	extended_registers_registry.push_back(pitmtld1_var);
	pitmtld1_var->setCallBack(this, PITMTLD1, &CallBackObject::write, NULL);

	for (unsigned int i=0; i<PIT_SIZE; i++) {
		std::stringstream sstr;
		sstr << "PITLD" << i;
		std::string shortName(sstr.str());
		
		registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + '.' + shortName, &pitld_register[i]));

		unisim::kernel::variable::Register<uint16_t> *pitld0_var = new unisim::kernel::variable::Register<uint16_t>(shortName.c_str(), this, pitld_register[i], "PIT Load Register (PITLD)");
		extended_registers_registry.push_back(pitld0_var);
		pitld0_var->setCallBack(this, (PITLD0 + i*4), &CallBackObject::write, NULL);
		
	}

	for (unsigned int i=0; i<PIT_SIZE; i++) {
		std::stringstream sstr;
		sstr << "PITCNT" << i;
		std::string shortName(sstr.str());
		
		registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + '.' + shortName, &pitcnt_register[i]));

		unisim::kernel::variable::Register<uint16_t> *pitcnt0_var = new unisim::kernel::variable::Register<uint16_t>(shortName.c_str(), this, pitcnt_register[i], "PIT Count Register (PITCNT)");
		extended_registers_registry.push_back(pitcnt0_var);
		pitcnt0_var->setCallBack(this, PITCNT0 + i*4, &CallBackObject::write, NULL);
	}

	Reset();

	ComputeInternalTime();

	return (true);
}

template <uint8_t PIT_SIZE>
bool S12PIT24B<PIT_SIZE>::Setup(ServiceExportBase *srv_export) {

	return (true);
}

template <uint8_t PIT_SIZE>
bool S12PIT24B<PIT_SIZE>::EndSetup() {
	return (true);
}

template <uint8_t PIT_SIZE>
Register* S12PIT24B<PIT_SIZE>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <uint8_t PIT_SIZE>
void S12PIT24B<PIT_SIZE>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

template <uint8_t PIT_SIZE>
void S12PIT24B<PIT_SIZE>::OnDisconnect() {
}

template <uint8_t PIT_SIZE>
void S12PIT24B<PIT_SIZE>::Reset() {

	pitcflmt_register = 0x00;
	pitflt_register = 0x00;
	pitce_register = 0x00;
	pitmux_register = 0x00;
	pitinte_register = 0x00;
	pittf_register = 0x00;
	pitmtld0_register = 0x00;
	pitmtld1_register = 0x0000;
	for (uint8_t i=0; i < PIT_SIZE; i++) {
		pitld_register[i] = 0x0000;
		pitcnt_register[i] = 0x0000;
	}

}

template <uint8_t PIT_SIZE>
void S12PIT24B<PIT_SIZE>::ResetMemory() {

	Reset();
	
}

//=====================================================================
//=             memory interface methods                              =
//=====================================================================

template <uint8_t PIT_SIZE>
bool S12PIT24B<PIT_SIZE>::ReadMemory(physical_address_t addr, void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress+REGISTERS_BANK_SIZE))) {

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
		case PITCFLMT:	*((uint8_t *) buffer) = pitcflmt_register; break;	// 1 byte
		case PITFLT: *((uint8_t *) buffer) = pitflt_register; break;	// 1 byte
		case PITCE: *((uint8_t *) buffer) = pitce_register; break;	// 1 byte
		case PITMUX: *((uint8_t *) buffer) = pitmux_register; break;	// 1 byte
		case PITINTE: *((uint8_t *) buffer) = pitinte_register; break;	// 1 byte
		case PITTF: *((uint8_t *) buffer) = pittf_register; break; // 1 byte
		case PITMTLD0: *((uint8_t *) buffer) = pitmtld0_register; break; // 1 byte
		case PITMTLD1: *((uint8_t *) buffer) = pitmtld1_register; break; // 1 byte
		default: {
			if ((offset >= PITLD0) && (offset < (PITLD0 + PIT_SIZE*4))) {
				uint8_t group_index = (offset - PITLD0) / 2;
				uint8_t group_register = (offset - PITLD0) % 2;
				if (group_register == 0) {
					*((uint16_t *) buffer) = Host2BigEndian(pitld_register[group_index]);
				} else {
					*((uint16_t *) buffer) = Host2BigEndian(pitcnt_register[group_index]);
				}
			}
		} break;
		}

		return (true);

	}

	return (false);
}

template <uint8_t PIT_SIZE>
bool S12PIT24B<PIT_SIZE>::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress+REGISTERS_BANK_SIZE))) {

		if (size == 0) {
			return (true);
		}

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
		case PITCFLMT:	{
			uint8_t val = *((uint8_t *) buffer);
			pitcflmt_register = val;	// 1 byte
		} break;
		case PITFLT: {
			uint8_t val = *((uint8_t *) buffer);
			pitflt_register = val; // 1 byte
		} break;
		case PITCE: {
			uint8_t val = *((uint8_t *) buffer);
			pitce_register = val; // 1 byte
		} break;
		case PITMUX: {
			uint8_t val = *((uint8_t *) buffer);
			pitmux_register = val; // 1 byte
		} break;
		case PITINTE: {
			uint8_t val = *((uint8_t *) buffer);
			pitinte_register = val; // 1 byte
		} break;
		case PITTF: {
			uint8_t val = *((uint8_t *) buffer);
			pittf_register = val; // 1 byte
		} break;
		case PITMTLD0: {
			uint8_t val = *((uint8_t *) buffer);
			pitmtld0_register = val; // 1 byte
		} break;
		case PITMTLD1: {
			uint8_t val = *((uint8_t *) buffer);
			pitmtld1_register = val; // 1 byte
		} break;
		default: {
			if ((offset >= PITLD0) && (offset < (PITLD0 + PIT_SIZE*4))) {
				uint8_t group_index = (offset - PITLD0) / 2;
				uint8_t group_register = (offset - PITLD0) % 2;
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				if (group_register == 0) {
					pitld_register[group_index] = val;
				} else {
					// write to PITCNTx registers has no meaning or effect
					pitcnt_register[group_index] = val;
				}
			}
		} break;
		}

		return (true);
	}

	return (false);

}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim




