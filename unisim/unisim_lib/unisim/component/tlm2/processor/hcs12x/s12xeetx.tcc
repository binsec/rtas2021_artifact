/*
 * s12xeetx.tcc
 *
 *  Created on: 10 oct. 2011
 *      Author: rnouacer
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XEETX_TCC_
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XEETX_TCC_


#include <unisim/component/tlm2/processor/hcs12x/s12xeetx.hh>


#define LOCATION __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

/**
 * Constructor.
 *
 * @param name the name of the module
 * @param parent the parent service
 */
/* Constructor */
template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::
S12XEETX(const sc_module_name& name, Object *parent)
	: Object(name, parent, "this module implements a memory")
	, unisim::component::tlm2::memory::ram::Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>(name, parent)
	, unisim::kernel::Service<Registers>(name, parent)

	, registers_export("registers_export", this)

	, bus_clock_socket("Bus-Clock")
	, slave_socket("slave_socket")

	, cmd_queue_back(0)

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

	, oscillator_cycle_time_int(250000)
	, param_oscillator_cycle_time_int("oscillator-cycle-time", this, oscillator_cycle_time_int)

	, min_eeclk_time(6667, SC_PS)

	, sector_erase_abort_active(false)
	, sector_erase_modify_active(false)
	, sector_erase_abort_command_req(false)

	, baseAddress(0x0110)
	, param_baseAddress("base-address", this, baseAddress)

	, cmd_interruptOffset(0xBA)
	, param_cmd_interruptOffset("command-interrupt", this, cmd_interruptOffset)

	, erase_fail_ratio(0.01)
	, param_erase_fail_ratio("erase-fail-ratio", this, erase_fail_ratio, "Ration to emulate erase failing. ")

	, eclkdiv_reg(0)
	, reserved1_reg(0)
	, reserved2_reg(0)
	, ecnfg_reg(0)
	, eprot_reg(0xFF)
	, estat_reg(0)
	, ecmd_reg(0)
	, reserved3_reg(0)
	, eaddr_reg(0)
	, edata_reg(0)

{

	param_oscillator_cycle_time_int.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

	interrupt_request(*this);
	slave_socket.register_b_transport(this, &S12XEETX::read_write);
	bus_clock_socket.register_b_transport(this, &S12XEETX::updateBusClock);

	SC_HAS_PROCESS(S12XEETX);

	SC_THREAD(Process);

	xint_payload = xint_payload_fabric.allocate();

}

/**
 * Destructor
 */
template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::
~S12XEETX() {

	xint_payload->release();

	// Release registers_registry
	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

	empty_cmd_queue();

}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::ComputeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

}


template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::Process()
{

	/**
	 *	The Initialization of the EPROT register is done by the Application Software
	 */

	sc_time delay;

	while (true) {

		// wait command launch by clearing ESTAT::CBEIF flag
		while (((estat_reg & 0x80) != 0) /*|| cmd_queue.empty()*/) {
			sc_module::wait(command_launch_event);
		}

		fetchCommand();

		// run the fetched command
		switch (ecmd_reg & 0x7F) {
			case 0x05: {
				/* Erase Verify */
				erase_verify_cmd();
			}
			break;
			case 0x20: {
				/* Word Program */
				word_program_cmd();
			}
			break;
			case 0x40: {
				/* Sector Erase */
				sector_erase_cmd();
			}
			break;
			case 0x41: {
				/* Mass Erase */
				mass_erase_cmd();
			}
			break;
			case 0x47: {
				/* Sector Erase Abort */
				sector_erase_abort_cmd();
			}
			break;
			case 0x60: {
				/* Sector Modify */
				sector_modify_cmd();
			}
			break;
			default: {
				/* unknown command */
				// set the ACCERR flag in the ESTAT register
				setACCERR();
			}
			break;
		}

		/**
		 * For all command write sequences except sector erase abort,
		 * the CBEIF flag will set four bus cycles after the CCIF flag is cleared indicating
		 * that the address, data, and command buffers are ready for a new command write sequence to begin.
		 * For sector erase abort operations, the CBEIF flag will remain clear until the operation completes.
		 * Except for the sector erase abort command, a buffered command will wait for the active operation to be completed
		 * before being launched.
		 * The sector erase abort command is launched when the CBEIF flag is cleared as part
		 * of a sector erase abort command write sequence.
		 * Once a command is launched, the completion of the command operation is indicated by the setting of the CCIF flag in the ESTAT register.
		 * The CCIF flag will set upon completion of all active and buffered commands.
		 */

		// set CCIF flag when all commands are completed
		if (cmd_queue.empty()) {
			estat_reg = estat_reg | 0x40;
			assertInterrupt(cmd_interruptOffset);
		}

		// isn't sector_erase_abort
		if ((ecmd_reg & 0x7F) != 0x47) {
			sc_module::wait(bus_cycle_time * 4);
		}

		// set CBEIF flag when address, data, command buffers are empty. This enable new write sequence.
		if (cmd_queue.empty()) {
			estat_reg = estat_reg | 0x80;
			assertInterrupt(cmd_interruptOffset);
		}

	}


}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::abort_write_sequence() {

	// remove the last command from the pipeline to abort command write sequence

	queue<TCommand*> tmpQueue;
	while (!cmd_queue.empty()) {
		TCommand * el = cmd_queue.front();
		cmd_queue.pop();
		if (el != cmd_queue_back) {
			tmpQueue.push(el);
		} else {
			if (!tmpQueue.empty()) {
				cmd_queue_back = tmpQueue.back();
			} else {
				cmd_queue_back = NULL;
			}
		}
	}

	while (!tmpQueue.empty()) {
		TCommand * el = tmpQueue.front();
		tmpQueue.pop();
		cmd_queue.push(el);
	}
};

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::erase_verify_cmd() {
	/**
	 * Verify all memory bytes in the EEPROM block are erased.
	 * If the EEPROM block is erased,
	 * the BLANK flag in the ESTAT register will set upon command completion
	 */
	sc_module::wait(bus_cycle_time * (inherited::bytesize/2 + 14));

	// is all words erased successfully ?
	if ((rand() % 101) > (erase_fail_ratio * 100)) {
		// set ESTAT::BLANK
		estat_reg = estat_reg | 0x04;
	}
}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::word_program_cmd() {
	/**
	 * Program a word (two bytes) in the EEPROM block
	 */

	/**
	 * EPROT::EPDIS = 0 protection enabled
	 * EPROT::EPDIS = 1 protection disabled
	 */
	bool protection_enabled = ((eprot_reg & 0x04) == 0);

	if (protection_enabled && ((inherited::org + ((eaddr_reg & 0xFFFE) + WORD_SIZE - 1)) >= protected_area_start_address)) {
		inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Try to word program at protected area @ 0x" << std::hex << eaddr_reg
			<< std::endl << EndDebugWarning;

		// Set ESTAT::PVIOL flag
		estat_reg = estat_reg | 0x20;
		return;
	}


	/**
	 * is aligned word ?
	 * Writing a byte or misaligned word to a valid EEPROM address is illegal and ACCERR flag is set.
	 */

	if (WriteMemory(inherited::org + (eaddr_reg & 0xFFFE), &edata_reg, 2)) {

		sc_module::wait(bus_cycle_time * (1 + 14));
	}

	inherited::write_counter++;

}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::sector_erase_cmd() {
	/**
	 * Erase all four memory bytes in a sector of the EEPROM block
	 */

	/**
	 * EPROT::EPDIS = 0 protection enabled
	 * EPROT::EPDIS = 1 protection disabled
	 */
	bool protection_enabled = ((eprot_reg & 0x04) == 0);

	if (protection_enabled && ((inherited::org + ((eaddr_reg & 0xFFFC) + SECTOR_SIZE - 1)) >= protected_area_start_address)) {
		inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Try to erase sector at protected area.@ 0x" << std::hex << eaddr_reg
			<< std::endl << EndDebugWarning;

		// Set ESTAT::PVIOL flag
		estat_reg = estat_reg | 0x20;
		return;
	}

	sector_erase_modify_active = true;

	uint8_t data[4] = {0xFF, 0xFF, 0xFF, 0xFF};

	for (uint8_t i=0; i<4; i++) {

		/**
		 * If the sector erase abort command is launched resulting in the early termination of an active sector erase or scetor modify operation, the ACCERR flag will set once the operation completes as indicated by the CCIF flag being set.
		 * Else if the sector erase or modify is completed before then ACCERR isn't set
		 */

		if (sector_erase_abort_command_req) {
			// set the ACCERR flag
			setACCERR();
			break;
		}

		if (WriteMemory(inherited::org + (eaddr_reg & 0xFFFC) + i, &data[i], 1)) {

			sc_module::wait(bus_cycle_time);

		}
	}

	if (sector_erase_abort_command_req) {
		sector_erase_abort_command_req = false;
	}

	inherited::write_counter++;

	sc_module::wait(bus_cycle_time * 14);

	sector_erase_modify_active = false;

}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::mass_erase_cmd() {
	/**
	 * Erase all memory bytes in the EEPROM block.
	 * A mass erase of the full EEPROM block is only possible when EPOPEN and EPDIS bits in the EPROT register are set prior launching the command.
	 */

	/**
	 * EPROT::EPDIS = 0 protection enabled
	 * EPROT::EPDIS = 1 protection disabled
	 */
	bool protection_enabled = ((eprot_reg & 0x04) == 0);

	if (protection_enabled) {
		inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Try to mass erase while EEPROM protection enabled." << std::endl << EndDebugWarning;

		// Set ESTAT::PVIOL flag
		estat_reg = estat_reg | 0x20;
		return;
	}

	inherited::write_counter++;

	sc_module::wait(bus_cycle_time * (inherited::bytesize/2 + 14));

}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::sector_erase_abort_cmd() {
	/**
	 * Abort the sector erase operation.
	 * The sector erase operation will terminate according to set procedure.
	 * The EEPROM sector should not be considered erased if the ACCERR flag is set upon command completion.
	 */

	sector_erase_abort_command_req = false;

	sector_erase_abort_active = true;

	sc_module::wait(bus_cycle_time * (1 + 14));

	sector_erase_abort_active = false;

}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::sector_modify_cmd() {
	/**
	 * Erase all four memory bytes in a sector of the EEPROM block and reprogram the addressed word.
	 */

	/**
	 * EPROT::EPDIS = 0 protection enabled
	 * EPROT::EPDIS = 1 protection disabled
	 */
	bool protection_enabled = ((eprot_reg & 0x04) == 0);

	if (protection_enabled && ((inherited::org + ((eaddr_reg & 0xFFFC) + SECTOR_SIZE - 1)) >= protected_area_start_address)) {
		inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Try to modify sector at protected area @ 0x" << std::hex << (unsigned int) inherited::org + (eaddr_reg & 0xFFFE)
			<< std::endl << EndDebugWarning;

		// Set ESTAT::PVIOL flag
		estat_reg = estat_reg | 0x20;
		return;
	}

	sector_erase_modify_active = true;

	// erase both words in a sector
	uint8_t data[4] = {0xFF, 0xFF, 0xFF, 0xFF};

	for (uint8_t i=0; i<4; i++) {

		/**
		 * If the sector erase abort command is launched resulting in the early termination of an active sector erase or scetor modify operation, the ACCERR flag will set once the operation completes as indicated by the CCIF flag being set.
		 * Else if the sector erase or modify is completed before then ACCERR isn't set
		 */

		if (sector_erase_abort_command_req) {
			// set the ACCERR flag
			setACCERR();
			break;
		}

		if (WriteMemory(inherited::org + (eaddr_reg & 0xFFFC) + i, &data[i], 1)) {

			sc_module::wait(bus_cycle_time);
		}
	}

	if (sector_erase_abort_command_req) {
		sector_erase_abort_command_req = false;
	} else {
		// program word at global_address while byte address bit 0 is ignored
		if (WriteMemory(inherited::org + (eaddr_reg & 0xFFFE), &edata_reg, 2)) {

			sc_module::wait(bus_cycle_time * (14 + 1));
		}
	}

	inherited::write_counter++;

	sector_erase_modify_active = false;

}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::write_to_eeprom(physical_address_t address, void* data_ptr, unsigned int data_length) {

	// Illegal operation: Writing to an EEPROM address before initializing the ECLKDIV register
	if ((eclkdiv_reg & 0x80) == 0) {
		inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Writing to an EEPROM address before initializing the ECLKDIV register is an illegal operation. " << std::endl << EndDebugWarning;

		setACCERR();
		return;
	}

	// Illegal operation: Writing a byte or misaligned word to a valid EEPROM address
	if ((data_length == 1) || ((address & 0x1) != 0)) {
		inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Writing a byte or misaligned word to a valid EEPROM address is an illegal operation " << data_length << " bytes @ 0x" << std::hex << address
			<< std::endl << EndDebugWarning;

		setACCERR();
		return;
	}

	// Illegal operation: Starting a command write sequence while a sector erase abort operation is active.
	if (sector_erase_abort_active) {
		inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Starting a command write sequence while a sector erase abort operation is active is an illegal operation " << std::endl << EndDebugWarning;

		setACCERR();
		return;
	}

	if ((cmd_queue_back != NULL) && !cmd_queue_back->isCmdWrite()) {
		cmd_queue_back->setAddr((uint16_t) (address - inherited::org) & 0x0FFF);
		if (data_length == 2) {
			cmd_queue_back->setData((uint16_t) *((uint16_t*) data_ptr));
		} else {
			cmd_queue_back->setData((uint8_t) *((uint8_t*) data_ptr));
		}
	} else {
		if ((cmd_queue_back != NULL) && cmd_queue_back->isCmdWrite() && ((estat_reg & 0x80) != 0)) {
			inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Writing to an EEPROM address after writing to the ECMD register and before launching the command is an illegal operation " << std::endl << EndDebugWarning;

			setACCERR();
			return;
		}
		else if (cmd_queue.size() < CMD_PIPELINE_SIZE) {
			TCommand * command = new TCommand();
			command->setAddr((uint16_t) (address - inherited::org) & 0x0FFF);
			if (data_length == 2) {
				command->setData((uint16_t) *((uint16_t*) data_ptr));
			} else {
				command->setData((uint8_t) *((uint8_t*) data_ptr));
			}

			cmd_queue_back = command;
			cmd_queue.push(command);
		} else {
			cmd_queue_back->setAddr((uint16_t) (address - inherited::org) & 0x0FFF);
			if (data_length == 2) {
				cmd_queue_back->setData((uint16_t) *((uint16_t*) data_ptr));
			} else {
				cmd_queue_back->setData((uint8_t) *((uint8_t*) data_ptr));
			}
			cmd_queue_back->invalidateCmdWrite();

			inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Command Write Sequence Lost/overwritten! " << std::endl << EndDebugWarning;

		}
	}

	if (((estat_reg & 0x80) ==0) && (cmd_queue.size() == 1)) {
		/**
		 * If  the CBEIF flag in the ESTAT register is clear when the first EEPROM array write occurs,
		 * Then the contents of the address and data buffers will be overwritten and the CBEIF flag will be set
		 */
		estat_reg = estat_reg | 0x80;
		eaddr_reg = cmd_queue_back->getAddr();
		edata_reg = cmd_queue_back->getData();
	}

}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::fetchCommand() {

	if (!cmd_queue.empty()) {
		TCommand *cmd = cmd_queue.front();
		cmd_queue.pop();
		ecmd_reg = cmd->getCmd();
		eaddr_reg = cmd->getAddr();
		edata_reg = cmd->getData();

		if (cmd_queue_back == cmd) {
			cmd_queue_back = NULL;
		}
		delete cmd;
	}
}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::writeCmd(uint8_t _cmd) {
	if (cmd_queue.empty()) {

		inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Writing to the ECMD register before writing to an EEPROM address is an illegal operation. " << std::endl << EndDebugWarning;

		setACCERR();

		return;
	}
	else if ((cmd_queue_back != NULL) && cmd_queue_back->isCmdWrite()) {
		inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Writing a second command to the ECMD register in the same command write sequence is an illegal operation. " << std::endl << EndDebugWarning;

		setACCERR();
		abort_write_sequence();

		return;
	} else {
		cmd_queue_back->setCmd(_cmd);

		// is sector erase abort command ?
		if (_cmd == 0x47) {
			sector_erase_abort_command_req = true;
		}
	}

	if (cmd_queue.size() == 1) {
		ecmd_reg = cmd_queue_back->getCmd();
	}
}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::empty_cmd_queue() {
	while (!cmd_queue.empty()) {
		TCommand *cmd = cmd_queue.front();
		cmd_queue.pop();

		delete cmd;
	}
	cmd_queue_back = NULL;
}

/* Service methods */
/** BeginSetup
 * Initializes the service interface. */
template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::BeginSetup()
{
	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".ECLKDIV", &eclkdiv_reg));

	unisim::kernel::variable::Register<uint8_t> *eclkdiv_var = new unisim::kernel::variable::Register<uint8_t>("ECLKDIV", this, eclkdiv_reg, "EEPROM Clock Divider Register (ECLKDIV)");
	extended_registers_registry.push_back(eclkdiv_var);
	eclkdiv_var->setCallBack(this, ECLKDIV, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".RESERVED1", &reserved1_reg));

	unisim::kernel::variable::Register<uint8_t> *reserved1_var = new unisim::kernel::variable::Register<uint8_t>("RESERVED1", this, reserved1_reg, "RESERVED1");
	extended_registers_registry.push_back(reserved1_var);
	reserved1_var->setCallBack(this, RESERVED1, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".RESERVED2", &reserved2_reg));

	unisim::kernel::variable::Register<uint8_t> *reserved2_var = new unisim::kernel::variable::Register<uint8_t>("RESERVED2", this, reserved2_reg, "RESERVED2");
	extended_registers_registry.push_back(reserved2_var);
	reserved2_var->setCallBack(this, RESERVED2, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".ECNFG", &ecnfg_reg));

	unisim::kernel::variable::Register<uint8_t> *ecnfg_var = new unisim::kernel::variable::Register<uint8_t>("ECNFG", this, ecnfg_reg, "EEPROM Configuration Register (ECNFG)");
	extended_registers_registry.push_back(ecnfg_var);
	ecnfg_var->setCallBack(this, ECNFG, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".EPROT", &eprot_reg));

	unisim::kernel::variable::Register<uint8_t> *eprot_var = new unisim::kernel::variable::Register<uint8_t>("EPROT", this, eprot_reg, "EEPROM Protection Register (EPROT)");
	extended_registers_registry.push_back(eprot_var);
	eprot_var->setCallBack(this, EPROT, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".ESTAT", &estat_reg));

	unisim::kernel::variable::Register<uint8_t> *estat_var = new unisim::kernel::variable::Register<uint8_t>("ESTAT", this, estat_reg, "EEPROM Status Register (ESTAT)");
	extended_registers_registry.push_back(estat_var);
	estat_var->setCallBack(this, ESTAT, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".ECMD", &ecmd_reg));

	unisim::kernel::variable::Register<uint8_t> *ecmd_var = new unisim::kernel::variable::Register<uint8_t>("ECMD", this, ecmd_reg, "EEPROM Command Register (ECMD)");
	extended_registers_registry.push_back(ecmd_var);
	ecmd_var->setCallBack(this, ECMD, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".RESERVED3", &reserved3_reg));

	unisim::kernel::variable::Register<uint8_t> *reserved3_var = new unisim::kernel::variable::Register<uint8_t>("RESERVED3", this, reserved3_reg, "RESERVED3");
	extended_registers_registry.push_back(reserved3_var);
	reserved3_var->setCallBack(this, RESERVED3, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".EADDR", &eaddr_reg));

	unisim::kernel::variable::Register<uint16_t> *eaddr_var = new unisim::kernel::variable::Register<uint16_t>("EADDR", this, eaddr_reg, "EEPROM Address Register");
	extended_registers_registry.push_back(eaddr_var);
	eaddr_var->setCallBack(this, EADDRHI, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".EDATA", &edata_reg));

	unisim::kernel::variable::Register<uint16_t> *edata_var = new unisim::kernel::variable::Register<uint16_t>("EDATA", this, edata_reg, "EEPROM Data Register");
	extended_registers_registry.push_back(edata_var);
	edata_var->setCallBack(this, EDATAHI, &CallBackObject::write, NULL);

	oscillator_cycle_time = sc_time(oscillator_cycle_time_int, SC_PS);

	Reset();

	return (inherited::BeginSetup());
}

/**
 * TLM2 Slave methods
 */
template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	return (inherited::get_direct_mem_ptr(payload, dmi_data));
}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
unsigned int S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::transport_dbg(tlm::tlm_generic_payload& payload)
{

	tlm::tlm_command cmd = payload.get_command();
	sc_dt::uint64 address = payload.get_address();
	void *data_ptr = payload.get_data_ptr();
	unsigned int data_length = payload.get_data_length();

	if ((address >= inherited::GetLowAddress()) && (address <= inherited::GetHighAddress())) {
		if (cmd == tlm::TLM_READ_COMMAND) {
			return (inherited::transport_dbg(payload));
		} else {
			write_to_eeprom( address, data_ptr, data_length);

			payload.set_response_status(tlm::TLM_OK_RESPONSE);
		}
	} else {
		payload.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
	}

	return (0);

}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
tlm::tlm_sync_enum S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase != tlm::BEGIN_REQ)
	{
		inherited::logger << DebugInfo << LOCATION
				<< ":" << (sc_time_stamp() + t).to_string()
				<< " : received an unexpected phase " << phase << std::endl
				<< EndDebugInfo;
		Object::Stop(-1);
	}

	tlm::tlm_command cmd = payload.get_command();
	sc_dt::uint64 address = payload.get_address();
	void *data_ptr = payload.get_data_ptr();
	unsigned int data_length = payload.get_data_length();

	if ((address >= inherited::GetLowAddress()) && (address <= inherited::GetHighAddress())) {
		if (cmd == tlm::TLM_READ_COMMAND) {

			return (inherited::nb_transport_fw(payload, phase, t));
		} else {

			write_to_eeprom(address, data_ptr, data_length);

			inherited::write_counter++;

			payload.set_response_status(tlm::TLM_OK_RESPONSE);

		}

	} else {
		payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
	}

	phase = tlm::BEGIN_RESP;

	return tlm::TLM_COMPLETED;
}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{

	tlm::tlm_command cmd = payload.get_command();
	sc_dt::uint64 address = payload.get_address();
	void *data_ptr = payload.get_data_ptr();
	unsigned int data_length = payload.get_data_length();

	if ((address >= inherited::GetLowAddress()) && (address <= inherited::GetHighAddress())) {
		if (cmd == tlm::TLM_READ_COMMAND) {

			inherited::b_transport(payload, t);
		} else {

			write_to_eeprom(address, data_ptr, data_length);

			payload.set_response_status( tlm::TLM_OK_RESPONSE );
		}
	} else {
		payload.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
	}

}

// Master methods
template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
tlm_sync_enum S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::assertInterrupt(uint8_t interrupt_offset) {

	// if CBEIE or CCIE flag set then enable interrupt
	if ((ecnfg_reg & 0xC0) == 0) return;

	// assert EEPROM_Command_Interrupt

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

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::Reset() {

	eclkdiv_reg = 0x00;
	reserved1_reg = 0x00;
	reserved2_reg = 0x00;
	ecnfg_reg = 0x00;
	eprot_reg = 0xFF;
	estat_reg = 0xC0;
	ecmd_reg = 0x00;
	reserved3_reg = 0x00;
	eaddr_reg = 0x0000;
	edata_reg = 0x0000;

	protected_area_start_address = inherited::hi_addr - 64 * ((eprot_reg & 0x07) + 1) + 1;

	empty_cmd_queue();

	ComputeInternalTime();

	setEEPROMClock();

}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::setEEPROMClock() {

	// if ECLKDIV::PRDIV8 set then enable a prescalar by 8
	eeclk_time = oscillator_cycle_time * ((eclkdiv_reg & 0x3F) + 1) * (((eclkdiv_reg & 0x40) != 0)? 8: 1);

	if (eeclk_time < min_eeclk_time) {
		inherited::logger << DebugWarning << sc_object::name() << ":: Setting EECLK to " << 1/eeclk_time.to_seconds() << " Hz can destroy the EEPROM. (EECLK < 150 kHz should be avoided)" << std::endl << EndDebugWarning;
	}
}


//=====================================================================
//=             memory interface methods                              =
//=====================================================================

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::ReadMemory(ADDRESS addr, void *buffer, uint32_t size)
{
	if ((addr >= baseAddress) && (addr <= (baseAddress+REGISTERS_BANK_SIZE))) {
		service_address_t offset = addr-baseAddress;
		switch (offset) {
			case ECLKDIV: *((uint8_t *) buffer) = eclkdiv_reg; break;
			case RESERVED1: *((uint8_t *) buffer) = reserved1_reg; break;
			case RESERVED2: *((uint8_t *) buffer) = reserved2_reg; break;
			case ECNFG: *((uint8_t *) buffer) = ecnfg_reg; break;
			case EPROT: *((uint8_t *) buffer) = eprot_reg; break;
			case ESTAT: *((uint8_t *) buffer) = estat_reg; break;
			case ECMD: *((uint8_t *) buffer) = ecmd_reg; break;
			case RESERVED3:	*((uint8_t *) buffer) = reserved3_reg; break;
			case EADDRHI: {
				if (size == 2) {
					*((uint16_t *) buffer) = eaddr_reg;
				} else {
					*((uint8_t *) buffer) = (uint8_t) ((eaddr_reg & 0xFF00) >> 8);
				}
			}
			break;
			case EADDRLO: *((uint8_t *) buffer) = (uint8_t) (eaddr_reg & 0x00FF); break;
			case EDATAHI: {
				if (size == 2) {
					*((uint16_t *) buffer) = edata_reg;
				} else {
					*((uint8_t *) buffer) = (uint8_t) ((edata_reg & 0xFF00) >> 8);
				}
			}
			break;
			case EDATALO: *((uint8_t *) buffer) = (uint8_t) (edata_reg & 0x00FF); break;
		}

		return (true);
	}

	return (inherited::ReadMemory(addr, buffer, size));
}

//template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
//bool S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::WriteMemory(service_address_t addr, const void *buffer, uint32_t size)
//{
//
//	if ((addr >= baseAddress) && (addr <= (baseAddress+EDATALO))) {
//
//		if (size == 0) {
//			return true;
//		}
//
//		service_address_t offset = addr-baseAddress;
//
//		return write(offset, buffer, size);
//	}
//
//	return inherited::WriteMemory(addr, buffer, size);
//}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::WriteMemory(ADDRESS addr, const void *buffer, uint32_t size)
{

	if ((addr >= baseAddress) && (addr <= (baseAddress+REGISTERS_BANK_SIZE))) {

		if (size == 0) {
			return (true);
		}

		service_address_t offset = addr-baseAddress;
		switch (offset) {
			case ECLKDIV:  eclkdiv_reg = *((uint8_t *) buffer); break;
			case RESERVED1: reserved1_reg = *((uint8_t *) buffer); break;
			case RESERVED2: reserved2_reg = *((uint8_t *) buffer); break;
			case ECNFG: ecnfg_reg = *((uint8_t *) buffer); break;
			case EPROT: eprot_reg = *((uint8_t *) buffer); break;
			case ESTAT: estat_reg = *((uint8_t *) buffer); break;
			case ECMD: ecmd_reg = *((uint8_t *) buffer); break;
			case RESERVED3:	ecmd_reg = *((uint8_t *) buffer); break;
			case EADDRHI: {
				if (size == 2) {
					eaddr_reg = *((uint16_t *) buffer);
				} else {
					eaddr_reg = (eaddr_reg & 0x00FF) | ((uint16_t) *((uint8_t *) buffer) << 8);
				}

			}
			break;
			case EADDRLO: eaddr_reg = (eaddr_reg & 0xFF00) | *((uint8_t *) buffer);	break;
			case EDATAHI: {
				if (size == 2) {
					edata_reg = *((uint16_t *) buffer);
				} else {
					edata_reg = (edata_reg & 0x00FF) | ((uint16_t) *((uint8_t *) buffer) << 8);
				}
			}
			break;
			case EDATALO: edata_reg= (edata_reg & 0xFF00) | *((uint8_t *) buffer); break;
		}

		return (true);
	}

	return (inherited::WriteMemory(addr, buffer, size));
}

//=====================================================================
//=             EEPROM Registers Interface interface methods               =
//=====================================================================

/**
 * Gets a register interface to the register specified by name.
 *
 * @param name The name of the requested register.
 * @return A pointer to the RegisterInterface corresponding to name.
 */
template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
Register * S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::GetRegister(const char *name) {

	return registers_registry.GetRegister(name);
}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner) {
	
	registers_registry.ScanRegisters(scanner);
}

//=====================================================================
//=             registers setters and getters                         =
//=====================================================================
template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::read(unsigned int offset, const void *buffer, unsigned int data_length)
{
	switch (offset) {
		case ECLKDIV: *((uint8_t *) buffer) = eclkdiv_reg; break;
		case RESERVED1: *((uint8_t *) buffer) = reserved1_reg; break;
		case RESERVED2: *((uint8_t *) buffer) = reserved2_reg; break;
		case ECNFG: *((uint8_t *) buffer) = ecnfg_reg & 0xC0; break;
		case EPROT: *((uint8_t *) buffer) = eprot_reg; break;
		case ESTAT: *((uint8_t *) buffer) = estat_reg & 0xF6; break;
		case ECMD: *((uint8_t *) buffer) = ecmd_reg & 0x7F; break;
		case RESERVED3:	*((uint8_t *) buffer) = reserved3_reg; break;
		case EADDRHI: {
			if (data_length == 2) {
				*((uint16_t *) buffer) = Host2BigEndian(eaddr_reg);
			} else {
				*((uint8_t *) buffer) = (uint8_t) ((eaddr_reg & 0xFF00) >> 8);
			}
		}
		break;
		case EADDRLO: *((uint8_t *) buffer) = (uint8_t) (eaddr_reg & 0x00FF); break;
		case EDATAHI: {
			if (data_length == 2) {
				*((uint16_t *) buffer) = Host2BigEndian(edata_reg);
			} else {
				*((uint8_t *) buffer) = (uint8_t) ((edata_reg & 0xFF00) >> 8);
			}
		}
		break;
		case EDATALO: *((uint8_t *) buffer) = (uint8_t) (edata_reg & 0x00FF); break;

		default: return (false);
	}

	return (true);
}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::write(unsigned int offset, const void *buffer, unsigned int data_length)
{

	if ((offset != ECMD) && ((cmd_queue_back != NULL) && !cmd_queue_back->isCmdWrite())) {
		inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Writing to any EEPROM register other than ECMD after writing to an EEPROM address is an illegal operation. " << std::endl << EndDebugWarning;

		setACCERR();
		abort_write_sequence();

	}
	else if ((offset != ESTAT) && ((cmd_queue_back != NULL) && cmd_queue_back->isCmdWrite())) {
		inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Writing to any EEPROM register other than ESTAT (to clear CBEIF) after writing to the ECMD register is an illegal operation. " << std::endl << EndDebugWarning;

		setACCERR();
		abort_write_sequence();

	}
	else {

		switch (offset) {
			case ECLKDIV: {
				uint8_t edivld_mask = 0x80;
				uint8_t value = *((uint8_t *) buffer);
				if ((eclkdiv_reg & edivld_mask) == 0) {
					// if ECLKDIV register hasn't written yet then accept write and set EDIVLD bit
					eclkdiv_reg = value | edivld_mask;

					// compute EECLK
					setEEPROMClock();
				}
			}
			break;
			case RESERVED1: reserved1_reg = *((uint8_t *) buffer); break;
			case RESERVED2: reserved2_reg = *((uint8_t *) buffer); break;
			case ECNFG: {
				uint8_t value = *((uint8_t *) buffer) & 0xC0;
				ecnfg_reg = value;
			}
			break;
			case EPROT: {

				uint8_t value = *((uint8_t *) buffer);

				// RNV[6:4] bits should remain in the erased state "1" for future enhancements.
				value = value | 0x70;

				if (((eprot_reg & 0x80) == 0) && ((value & 0x80) != 0)) {
					inherited::logger << DebugWarning << LOCATION << " : " << sc_object::name() << ":: Try writing to EPOPEN. EERPOM is in protected mode." << std::endl << EndDebugWarning;

					value = value & 0x7F;
				}

				if (((eprot_reg & 0x08) == 0) && ((value & 0x08) != 0)) {
					inherited::logger << DebugWarning << LOCATION << " : " << sc_object::name() << ":: Try writing to EPOPEN. EERPOM is in protected mode." << std::endl << EndDebugWarning;

					value = value & 0xF7;
				}

				// the EPS[2:0] bits can be written anytime until bit EPDIS is cleared
				if ((eprot_reg & 0x04) == 0) {
					inherited::logger << DebugWarning << LOCATION << " : " << sc_object::name() << ":: Try writing to EPS[2:0] bits. EPDIS is cleared." << std::endl << EndDebugWarning;

					value = (value & 0xF8) | (eprot_reg & 0x07);
				}

				eprot_reg = value;
				protected_area_start_address = inherited::hi_addr - 64 * ((eprot_reg & 0x07) + 1) + 1;

			}
			break;
			case ESTAT: {
				uint8_t value = *((uint8_t *) buffer);

				// clear PVIOL
				if ((value & 0x20) != 0) {
					value = value & 0xDF;
				}

				// clear ACCERR
				if ((value & 0x10) != 0) {
					value = value & 0xEF;
				}

				// clear FAIL
				if ((value & 0x02) != 0) {
					value = value & 0xFD;
				}

				/**
				 * The basic command write sequence is as follows:
				 * 1. Write to one address in the EEPROM
				 * 2. Write a valid command to the ECMD register
				 * 3. Clear the CBEIF flag in the ESTAT register by writing a 1 to CBEIF to launch the command.
				 */

				if ((value & 0x80) != 0) {
					if ((estat_reg & 0x30) != 0) {
						inherited::logger << DebugWarning << " : " << sc_object::name() << ":: can't launch new command. PVIOL/ACCERR flag is set." << std::endl << EndDebugWarning;
						break;
					} else {
						if ((eclkdiv_reg & 0x80) == 0) {
							/**
							 *  If the ECLKDIV register has not been written to,
							 *  the EEPROM command loaded during a command write sequence will not executed
							 *  and the ACCERR flag is set.
							 */
							// set the ACCERR flag in the ESTAT register
							value = value | 0x10;

						} else {
							// note: Clearing CBEIF outside of a command write sequence has no effect on the E2P state machine (CBEIF and CCIF)
							// note: Writing a 0 to CBEIF (clearing CBEIF) outside of a command write sequence will not set the ACCERR flag
							if (cmd_queue.empty()) {
								value = (value & 0x3B) | (estat_reg & 0xC4);
							} else {
								// clear CBEIF
								// CCIF is cleared automatically when CBEIF is cleared
								// BLANK is cleared automatically when CBEIF is cleared
								value = value & 0x3B;

								command_launch_event.notify();
							}
						}
					}

				} else {
					/**
					 * Note: command write sequence aborted by writing 0x00 to ESTAT register.
					 *
					 * Writing a 0 to CBEIF after writing an aligned word to the EEPROM address space
					 * but before CBEIF is cleared will abort a command write sequence and cause the ACCERR flag to be set.
					 */
					if ((cmd_queue_back != NULL) && ((estat_reg & 0x80) != 0)) {
						inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Writing a 0 to CBEIF after writing to the EEPROM address space but before CBEIF is cleared is an illegal operation " << std::endl << EndDebugWarning;

						value = value | 0x10;

					}
					abort_write_sequence();
				}

				estat_reg = value;

			}
			break;
			case ECMD: {
				uint8_t value = *((uint8_t *) buffer);
				value = (value & 0x7F) | (ecmd_reg & 0x80);

				uint8_t cmd = (value & 0x7F);

				if ( (cmd != 0x05) && (cmd != 0x20) && (cmd != 0x40) && (cmd != 0x41) && (cmd != 0x47) && (cmd != 0x60) && (cmd != 0x20) )
				{
					/* unknown command */
					// set the ACCERR flag in the ESTAT register
					inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Writing an invalid command to the ECMD register is an illegal operation. " << std::endl << EndDebugWarning;

					setACCERR();
					abort_write_sequence();
				} else {

					writeCmd(cmd);
				}

			}
			break;
			case RESERVED3:	ecmd_reg = *((uint8_t *) buffer); break;
			case EADDRHI: {
				if (data_length == 2) {
					eaddr_reg = BigEndian2Host(*((uint16_t *) buffer));
				} else {
					eaddr_reg = (eaddr_reg & 0x00FF) | ((uint16_t) *((uint8_t *) buffer) << 8);
				}

			}
			break;
			case EADDRLO: eaddr_reg = (eaddr_reg & 0xFF00) | *((uint8_t *) buffer);	break;
			case EDATAHI: {
				if (data_length == 2) {
					edata_reg = BigEndian2Host(*((uint16_t *) buffer));
				} else {
					edata_reg = (edata_reg & 0x00FF) | ((uint16_t) *((uint8_t *) buffer) << 8);
				}
			}
			break;
			case EDATALO: edata_reg= (edata_reg & 0xFF00) | *((uint8_t *) buffer); break;

			default: return (false);
		}

	}

	return (true);
}

template <unsigned int CMD_PIPELINE_SIZE, unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<CMD_PIPELINE_SIZE, BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
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

} // end of hcs12x
} // end of processor
} // end of tlm2
} // end of component
} // end of unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XEETX_TCC_ */
