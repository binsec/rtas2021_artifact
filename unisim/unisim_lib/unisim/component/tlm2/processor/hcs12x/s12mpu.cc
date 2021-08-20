/*
 * S12MPU.cc
 *
 *  Created on: 8 août 2012
 *      Author: rnouacer
 */

#include <unisim/component/tlm2/processor/hcs12x/s12mpu.hh>
#include <unisim/component/tlm2/processor/hcs12x/xint.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {


S12MPU::S12MPU(const sc_module_name& name, Object *parent) :
	Object(name, parent)
	, sc_module(name)

	, unisim::kernel::Client<TrapReporting>(name, parent)
	, unisim::kernel::Service<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::Service<Registers>(name, parent)
	, unisim::kernel::Client<Memory<physical_address_t> >(name, parent)

	, trap_reporting_import("trap_reporting_import", this)

	, slave_socket("slave_socket")

	, memory_export("memory_export", this)
	, memory_import("memory_import", this)
	, registers_export("registers_export", this)

	, baseAddress(0x0114)
	, param_baseAddress("base-address", this, baseAddress)

	, interrupt_offset(0x14)
	, param_interrupt_offset("interrupt-offset", this, interrupt_offset, "MPU Access Error interrupt")

	, debug_enabled(false)
	, param_debug_enabled("debug-enabled", this, debug_enabled)

	, mpuflg_register(0x00)
	, mpuastat0_register(0x00)
	, mpuastat1_register(0x00)
	, mpuastat2_register(0x00)
	, reserved(0x00)
	, mpusel_register(0x00)

{


	interrupt_request(*this);

	slave_socket.register_b_transport(this, &S12MPU::read_write);

// 	SC_HAS_PROCESS(S12MPU);

}


S12MPU::~S12MPU() {

	// Release registers_registry
	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

}

bool S12MPU:: validate(TOWNER::OWNER who, physical_address_t addr, uint32_t size, bool isWrite, bool isExecute) {

	bool isViolation = false;

	for (uint8_t i=0; i<MPU_DESC_NUMBER; i++) {
		/**
		 * is descriptor applicable to the requester ?
		 * MSTR0 and MSTR1 are used for CPU (NOTE: Distinction between supervisor and user mode is not implemented)
		 * MSTR2 for XGATE
		 * MSTR3 for master3 (not implemented)
		 */
		bool isApplicable = false;
		if (who == TOWNER::CPU12X) {
			isApplicable = ((mpudesc[i][0] & 0xC0) != 0);
		} else if (who == TOWNER::XGATE) {
			isApplicable = ((mpudesc[i][0] & 0x20) != 0);
		} else {
			isApplicable = ((mpudesc[i][0] & 0x10) != 0);
		}

		if (isApplicable) {
			uint32_t aligned_start_addr = ((addr >> 3) << 3); // The granularity of each descriptor is 8-bytes
			uint32_t aligned_end_addr = (((addr + size - 1) >> 3) << 3); // The granularity of each descriptor is 8-bytes

			// check protection range limits
			uint32_t low_addr = (uint32_t) ((mpudesc[i][0] & 0x0F) << 19)
											| (mpudesc[i][1] << 11)
											| (mpudesc[i][2] << 3);

			uint32_t high_addr = (uint32_t) ((mpudesc[i][3] & 0x0F) << 19)
											| (mpudesc[i][4] << 11)
											| (mpudesc[i][5] << 3)
											| 0x7; // all the 8-bits are covered by the protection range

			// check access violation and set MPUFLG register
			if ((aligned_start_addr <= high_addr) && (aligned_end_addr >= low_addr)) {
				// The Violations are combined
				if (isWrite && ((mpudesc[i][3] & 0x80) != 0)) {
					isViolation = true;
				}

				if (isExecute && ((mpudesc[i][3] & 0x40) != 0)) {
					isViolation = true;
				}
			}
		}
	}

	if (isViolation) {
		cerr << "MPU:: Violation detected" << endl;

		assertInterrupt();
		return false;
	}

	return (true);
}

void S12MPU::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();
	unsigned int data_length = trans.get_data_length();

	if ((address >= baseAddress) && (address < (baseAddress + MEMORY_MAP_SIZE))) {

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

//=====================================================================
//=             registers setters and getters                         =
//=====================================================================

bool S12MPU::read(unsigned int offset, const void *buffer, unsigned int data_length) {

	/**
	 * The module address 0x0006-0x000B (MPUDESC0, MPUDESC1, MPUDESC2, MPUDESC3, MPUDESC4, MPUDESC5)
	 * represent a window in the register map through which different descriptor registers
	 * are visible depending on the selection of MPUSEL register
	 */

	switch (offset) {
		case MPUFLG: *((uint8_t *) buffer) = mpuflg_register & 0xE1; break;
		case MPUASTAT0: *((uint8_t *) buffer) = mpuastat0_register & 0x7F; break;
		case MPUASTAT1: *((uint8_t *) buffer) = mpuastat1_register; break;
		case MPUASTAT2: *((uint8_t *) buffer) = mpuastat2_register; break;
		case RESERVED: *((uint8_t *) buffer) = 0x00; break;
		case MPUSEL: *((uint8_t *) buffer) = mpusel_register & 0x87; break;
		case MPUDESC0: *((uint8_t *) buffer) = mpudesc[(mpusel_register & 0x07)][0]; break;
		case MPUDESC1: *((uint8_t *) buffer) = mpudesc[(mpusel_register & 0x07)][1]; break;
		case MPUDESC2: *((uint8_t *) buffer) = mpudesc[(mpusel_register & 0x07)][2]; break;
		case MPUDESC3: *((uint8_t *) buffer) = mpudesc[(mpusel_register & 0x07)][3] & 0xCF; break;
		case MPUDESC4: *((uint8_t *) buffer) = mpudesc[(mpusel_register & 0x07)][4]; break;
		case MPUDESC5: *((uint8_t *) buffer) = mpudesc[(mpusel_register & 0x07)][5]; break;
		default:
			if ((offset >= MPU_DESC_BANKS_OFFSET) && (offset < (MPU_DESC_BANKS_OFFSET + (MPU_DESC_NUMBER * MPU_DESC_WINDOW_SIZE)))) {
				/**
				 * don't use the following linearization because it suppose specific allocation
				 * *((uint8_t *) buffer) = ((uint8_t *) mpudesc)[offset - MPU_DESC_BANKS_OFFSET]; break;
				 */
				*((uint8_t *) buffer) = mpudesc[(offset - MPU_DESC_BANKS_OFFSET) / MPU_DESC_NUMBER][(offset - MPU_DESC_BANKS_OFFSET) % MPU_DESC_NUMBER]; break;

			} else {
				return (false);
			}
	}

	return (true);
}

bool S12MPU::write(unsigned int offset, const void *buffer, unsigned int data_length) {

	switch (offset) {
		case MPUFLG: {
			// only AEF is writable, the rest of flags are automatically update when next violation occur
			uint8_t val = ~(*((uint8_t *) buffer) & 0x80) & mpuflg_register;
			mpuflg_register = val;
		} break;
		case MPUASTAT0: break;
		case MPUASTAT1: break;
		case MPUASTAT2: break;
		case RESERVED: break;
		case MPUSEL: {
			uint8_t val = *((uint8_t *) buffer) & 0x87;
			mpusel_register = val;
		} break;
		case MPUDESC0: {
			uint8_t val = *((uint8_t *) buffer);
			mpudesc[(mpusel_register & 0x07)][0] = val;
		} break;
		case MPUDESC1: {
			uint8_t val = *((uint8_t *) buffer);
			mpudesc[(mpusel_register & 0x07)][1] = val;
		} break;
		case MPUDESC2: {
			uint8_t val = *((uint8_t *) buffer);
			mpudesc[(mpusel_register & 0x07)][2] = val;
		} break;
		case MPUDESC3: {
			uint8_t val = *((uint8_t *) buffer) & 0xCF;
			mpudesc[(mpusel_register & 0x07)][3] = val;
		} break;
		case MPUDESC4: {
			uint8_t val = *((uint8_t *) buffer);
			mpudesc[(mpusel_register & 0x07)][4] = val;
		} break;
		case MPUDESC5: {
			uint8_t val = *((uint8_t *) buffer);
			mpudesc[(mpusel_register & 0x07)][5] = val;
		} break;

		default:
			if ((offset >= MPU_DESC_BANKS_OFFSET) && (offset < (MPU_DESC_BANKS_OFFSET + (MPU_DESC_NUMBER * MPU_DESC_WINDOW_SIZE)))) {
				/**
				 * don't use the following linearization because it suppose specific allocation
				 * *((uint8_t *) buffer) = ((uint8_t *) mpudesc)[offset - MPU_DESC_BANKS_OFFSET]; break;
				 */
				mpudesc[(offset - MPU_DESC_BANKS_OFFSET) / MPU_DESC_NUMBER][(offset - MPU_DESC_BANKS_OFFSET) % MPU_DESC_NUMBER] = *((uint8_t *) buffer);
				break;

			} else {
				return (false);
			}

	}

	return (true);
}


void S12MPU::assertInterrupt() {

	tlm_phase phase = BEGIN_REQ;
	XINT_Payload *payload = xint_payload_fabric.allocate();

	payload->setInterruptOffset(interrupt_offset);

	sc_time local_time = quantumkeeper.get_local_time();

	tlm_sync_enum ret = interrupt_request->nb_transport_fw(*payload, phase, local_time);

	payload->release();

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

void S12MPU::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

// Master methods

tlm_sync_enum S12MPU::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}


//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================


bool S12MPU::BeginSetup() {

	Reset();

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".MPUFLG", &mpuflg_register));

	unisim::kernel::variable::Register<uint8_t> *mpuflg_var = new unisim::kernel::variable::Register<uint8_t>("MPUFLG", this, mpuflg_register, "MPU Flag register (MPUFLG)");
	extended_registers_registry.push_back(mpuflg_var);
	mpuflg_var->setCallBack(this, MPUFLG, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".MPUASTAT0", &mpuastat0_register));

	unisim::kernel::variable::Register<uint8_t> *mpuastat0_var = new unisim::kernel::variable::Register<uint8_t>("MPUASTAT0", this, mpuastat0_register, "MPU Address Status register 0 (MPUASTAT0)");
	extended_registers_registry.push_back(mpuastat0_var);
	mpuastat0_var->setCallBack(this, MPUASTAT0, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".MPUASTAT1", &mpuastat1_register));

	unisim::kernel::variable::Register<uint8_t> *mpuastat1_var = new unisim::kernel::variable::Register<uint8_t>("MPUASTAT1", this, mpuastat1_register, "MPU Address Status register 1 (MPUASTAT1)");
	extended_registers_registry.push_back(mpuastat1_var);
	mpuastat1_var->setCallBack(this, MPUASTAT1, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".MPUASTAT2", &mpuastat2_register));

	unisim::kernel::variable::Register<uint8_t> *mpuastat2_var = new unisim::kernel::variable::Register<uint8_t>("MPUASTAT2", this, mpuastat2_register, "MPU Address Status register 2 (MPUASTAT2)");
	extended_registers_registry.push_back(mpuastat2_var);
	mpuastat2_var->setCallBack(this, MPUASTAT2, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".MPUSEL", &mpusel_register));

	unisim::kernel::variable::Register<uint8_t> *mpusel_var = new unisim::kernel::variable::Register<uint8_t>("MPUSEL", this, mpusel_register, "MPU Descriptor select register (MPUSEL)");
	extended_registers_registry.push_back(mpusel_var);
	mpusel_var->setCallBack(this, MPUSEL, &CallBackObject::write, NULL);

//	static const uint8_t MPU_DESC_BANKS_OFFSET = 0x0C;	// This offset is used to instruments all descriptors
//	static const uint8_t MPU_DESC_WINDOW_SIZE = 6;

	for (unsigned int i=0; i<MPU_DESC_NUMBER; i++) {
		for (unsigned int j=0; j<MPU_DESC_WINDOW_SIZE; j++) {
			
			std::stringstream sstr;
			sstr << "MPUDESC" << j << '_' << i;
			std::string shortName(sstr.str());
			
			registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + '.' + shortName, &(mpudesc[i][j])));

			unisim::kernel::variable::Register<uint8_t> *mpudesc_var = new unisim::kernel::variable::Register<uint8_t>(shortName.c_str(), this, mpudesc[i][j], "MPU Descriptor register");
			extended_registers_registry.push_back(mpudesc_var);
			mpusel_var->setCallBack(this, MPU_DESC_BANKS_OFFSET + (i * MPU_DESC_WINDOW_SIZE) + j, &CallBackObject::write, NULL);
		}
	}

	return (true);
}


bool S12MPU::Setup(ServiceExportBase *srv_export) {

	return (true);
}


bool S12MPU::EndSetup() {
	return (true);
}


Register* S12MPU::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

void S12MPU::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

void S12MPU::OnDisconnect() {
}


void S12MPU::Reset() {

	mpuflg_register = 0x00;
	mpuastat0_register = 0x00;
	mpuastat1_register = 0x00;
	mpuastat2_register = 0x00;
	reserved = 0x00;
	mpusel_register = 0x00;

	for (int i=0; i < 8; i++) {
		mpudesc[i][0] = 0x00;
		mpudesc[i][1] = 0x00;
		mpudesc[i][2] = 0x00;
		mpudesc[i][3] = 0x0F;
		mpudesc[i][4] = 0xFF;
		mpudesc[i][5] = 0xFF;
	}

	mpudesc[0][0] = 0xE0;

}

void S12MPU::ResetMemory() {

	Reset();

}

//=====================================================================
//=             memory interface methods                              =
//=====================================================================


bool S12MPU::ReadMemory(physical_address_t addr, void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress + MEMORY_MAP_SIZE))) {

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
			case MPUFLG: *((uint8_t *) buffer) = mpuflg_register; break;
			case MPUASTAT0: *((uint8_t *) buffer) = mpuastat0_register; break;
			case MPUASTAT1: *((uint8_t *) buffer) = mpuastat1_register; break;
			case MPUASTAT2: *((uint8_t *) buffer) = mpuastat2_register; break;
			case RESERVED: *((uint8_t *) buffer) = 0x00; break;
			case MPUSEL: *((uint8_t *) buffer) = mpusel_register; break;
			case MPUDESC0: *((uint8_t *) buffer) = mpudesc[(mpusel_register & 0x07)][0]; break;
			case MPUDESC1: *((uint8_t *) buffer) = mpudesc[(mpusel_register & 0x07)][1]; break;
			case MPUDESC2: *((uint8_t *) buffer) = mpudesc[(mpusel_register & 0x07)][2]; break;
			case MPUDESC3: *((uint8_t *) buffer) = mpudesc[(mpusel_register & 0x07)][3]; break;
			case MPUDESC4: *((uint8_t *) buffer) = mpudesc[(mpusel_register & 0x07)][4]; break;
			case MPUDESC5: *((uint8_t *) buffer) = mpudesc[(mpusel_register & 0x07)][5]; break;
			default: return (false);
		}

		return (true);

	}

	return (false);
}


bool S12MPU::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress + MEMORY_MAP_SIZE))) {

		if (size == 0) {
			return (true);
		}

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
			case MPUFLG: mpuflg_register = *((uint8_t *) buffer); break;
			case MPUASTAT0: mpuastat0_register = *((uint8_t *) buffer); break;
			case MPUASTAT1: mpuastat1_register = *((uint8_t *) buffer); break;
			case MPUASTAT2: mpuastat2_register = *((uint8_t *) buffer); break;
			case RESERVED: break;
			case MPUSEL: mpusel_register = *((uint8_t *) buffer); break;
			case MPUDESC0: mpudesc[(mpusel_register & 0x07)][0] = *((uint8_t *) buffer); break;
			case MPUDESC1: mpudesc[(mpusel_register & 0x07)][1] = *((uint8_t *) buffer); break;
			case MPUDESC2: mpudesc[(mpusel_register & 0x07)][2] = *((uint8_t *) buffer); break;
			case MPUDESC3: mpudesc[(mpusel_register & 0x07)][3] = *((uint8_t *) buffer); break;
			case MPUDESC4: mpudesc[(mpusel_register & 0x07)][4] = *((uint8_t *) buffer); break;
			case MPUDESC5: mpudesc[(mpusel_register & 0x07)][5] = *((uint8_t *) buffer); break;
			default: return (false);
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





