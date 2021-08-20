/*
 * S12XFTMX.tcc
 *
 *  Created on: 28 January 2013
 *      Author: rnouacer
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XFTMX_TCC_
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XFTMX_TCC_


#include <unisim/component/tlm2/processor/hcs12x/s12xftmx.hh>
#include <unisim/util/converter/convert.hh>

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
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::
S12XFTMX(const sc_module_name& name, Object *parent) :
	Object(name, parent, "this module implements a memory")
	, unisim::component::tlm2::memory::ram::Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>(name, parent)
	, unisim::kernel::Service<Registers>(name, parent)
	, registers_export("registers_export", this)

	, bus_clock_socket("Bus-Clock")
	, slave_socket("slave_socket")

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

	, oscillator_cycle_time_int(250000)
	, param_oscillator_cycle_time_int("oscillator-cycle-time", this, oscillator_cycle_time_int)

	, baseAddress(0x0100)
	, param_baseAddress("base-address", this, baseAddress)

	, flashFaultDetect_interruptOffset(0xBA)
	, param_flashFaultDetect_interruptOffset("flash-fault-detect-interrupt", this, flashFaultDetect_interruptOffset)

	, flash_interruptOffset(0xB8)
	, param_flash_interruptOffset("flash-interrupt", this, flash_interruptOffset)

	, erase_fail_ratio(0.01)
	, param_erase_fail_ratio("erase-fail-ratio", this, erase_fail_ratio)

	, pflash_blocks_description_string("7C0000,7FFFFF;7A0000,7BFFFF;780000,79FFFF;740000,77FFFF;700000,73FFFF")
	, param_pflash_blocks_description_string("pflash-blocks-description", this, pflash_blocks_description_string)

	, pflash_start_address(0x700000)
	, param_pflash_start_address("pflash-start-address", this, pflash_start_address)
	, pflash_end_address(0x7FFFFF)
	, param_pflash_end_address("pflash-end-address", this, pflash_end_address)

	, pflash_protectable_high_address(0x7FFFFF)
	, param_pflash_protectable_high_address("pflash-protectable-high-address", this, pflash_protectable_high_address)
	, pflash_protectable_low_address(0x7F8000)
	, param_pflash_protectable_low_address("pflash-protectable-low-address", this, pflash_protectable_low_address)

	, blackdoor_comparison_key_address(0x7FFF00)
	, param_blackdoor_comparison_key_address("blackdoor-comparison-key-address", this, blackdoor_comparison_key_address)
	, pflash_protection_byte_address(0x7FFF0C)
	, param_pflash_protection_byte_address("pflash-protection-byte-address", this, pflash_protection_byte_address)
	, eee_protection_byte_address(0x7FFF0D)
	, param_eee_protection_byte_address("eee-protection-byte-address", this, eee_protection_byte_address)
	, flash_nonvolatile_byte_address(0x7FFF0E)
	, param_flash_nonvolatile_byte_address("flash-nonvolatile-byte-address", this, flash_nonvolatile_byte_address, "Flash non-volatile (option) byte address")
	, flash_security_byte_address(0x7FFF0F)
	, param_flash_security_byte_address("flash-security-byte-address", this, flash_security_byte_address)

	, dflash_start_address(0x100000)
	, param_dflash_start_address("dflash-start-address", this, dflash_start_address)
	, dflash_end_address(0x107FFF)
	, param_dflash_end_address("dflash-end-address", this, dflash_end_address)

	, eee_nonvolatile_information_register_start_address(0x120000)
	, param_eee_nonvolatile_information_register_start_address("eee-nonvolatile-information-register-start-address", this, eee_nonvolatile_information_register_start_address)
	, eee_nonvolatile_information_register_size(128)
	, param_eee_nonvolatile_information_register_size("eee-nonvolatile-information-register-size", this, eee_nonvolatile_information_register_size)
	, eee_tag_ram_start_address(0x122000)
	, param_eee_tag_ram_start_address("eee_tag-ram-start-address", this, eee_tag_ram_start_address)
	, eee_tag_ram_size(256)
	, param_eee_tag_ram_size("EEE-tag-RAM-size", this, eee_tag_ram_size)
	, eee_protectable_high_address(0x13FFFF)
	, param_eee_protectable_high_address("eee-protectable-high-address", this, eee_protectable_high_address)

	, memory_controller_scratch_ram_start_address(0x124000)
	, param_memory_controller_scratch_ram_start_address("memory-controller-scratch-ram-start-address", this, memory_controller_scratch_ram_start_address)
	, memory_controller_scratch_ram_size(1024)
	, param_memory_controller_scratch_ram_size("memory-controller-scratch-ram-size", this, memory_controller_scratch_ram_size)
	, buffer_ram_start_address(0x13F000)
	, param_buffer_ram_start_address("buffer-ram-start-address", this, buffer_ram_start_address)
	, buffer_ram_end_address(0x13FFFF)
	, param_buffer_ram_end_address("buffer-ram-end-address", this, buffer_ram_end_address)

	, dflash_partition_user_access_address(0x120000)
	, param_dflash_partition_user_access_address("dflash-partition-user-access-address", this, dflash_partition_user_access_address)
	, dflash_partition_user_access_address_duplicate(0x120002)
	, param_dflash_partition_user_access_address_duplicate("dflash-partition-user-access-address-duplicate", this, dflash_partition_user_access_address_duplicate)
	, buffer_ram_partition_eee_operation_address(0x120004)
	, param_buffer_ram_partition_eee_operation_address("buffer-ram-partition-eee-operation-address", this, buffer_ram_partition_eee_operation_address)
	, buffer_ram_partition_eee_operation_address_duplicate(0x120006)
	, param_buffer_ram_partition_eee_operation_address_duplicate("buffer-ram-partition-eee-operation-address-duplicate", this, buffer_ram_partition_eee_operation_address_duplicate)

	, max_number_sectors_dflash(128)
	, param_max_number_sectors_dflash("max-number-sectors-dflash", this, max_number_sectors_dflash)
	, max_number_sectors_buffer_ram(16)
	, param_max_number_sectors_buffer_ram("max-number-sectors-buffer-ram", this, max_number_sectors_buffer_ram)
	, min_number_sectors_in_dflash_for_eee(12)
	, param_min_number_sectors_in_dflash_for_eee("min-number-sectors-in-dflash-for-eee", this, min_number_sectors_in_dflash_for_eee)
	, min_ratio_dflash_buffer_ram(8)
	, param_min_ratio_dflash_buffer_ram("min-ratio-dflash-buffer-ram", this, min_ratio_dflash_buffer_ram)

	, fclk_time(1250000, SC_PS) // 0.8 MHz
	, min_fclk_time_int(1250000) // 0.8 MHz
	, param_min_fclk_time("min-fclk-time", this, min_fclk_time_int, "Specify minimum frequency of FTM")
	, max_fclk_time_int(952000)  // 1.05 MHz
	, param_max_fclk_time("max-fclk-time", this, max_fclk_time_int, "Specify maximum frequency of FTM")

	, partitionDFlashCmd_Launched(false)
	, fullPartitionDFlashCmd_Launched(false)
	, eepromEmulationEnabled(false)
	, verifyBackdoorAccessKey_Failed(false)

	, sector_erased_count(0)
	, dead_sector_count(0)
	, ready_sector_count(0)

	, fclkdiv_reg(0x00)
	, fsec_reg(0x00)
	, fccobix_reg(0x00)
	, feccrix_reg(0x00)
	, fcnfg_reg(0x00)
	, fercnfg_reg(0x00)
	, fstat_reg(0x80)
	, ferstat_reg(0x00)
	, fprot_reg(0x00)
	, eprot_reg(0x00)
	, fopt_reg(0x00)
	, frsv0_reg(0x00)
	, frsv1_reg(0x00)
	, frsv2_reg(0x00)
	, etag_reg(0x0000)

{

	param_oscillator_cycle_time_int.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

	interrupt_request(*this);
	slave_socket.register_b_transport(this, &S12XFTMX::read_write);
	bus_clock_socket.register_b_transport(this, &S12XFTMX::updateBusClock);

	SC_HAS_PROCESS(S12XFTMX);

	SC_THREAD(Process);

}

/**
 * Destructor
 */
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::
~S12XFTMX() {

	// Release registers_registry
	for (unsigned int i=0; i<extended_registers_registry.size(); i++) {
		delete extended_registers_registry[i];
	}

	for (unsigned int i = 0; i < pflash_blocks_description.size(); ++i) {
		delete pflash_blocks_description[i];
	}
	pflash_blocks_description.clear();

	for (unsigned int i = 0; i < loadDataFieldBuffer.size(); ++i) {
		free (loadDataFieldBuffer[i]);
	}
	loadDataFieldBuffer.clear();

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::ComputeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

}


template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::Process()
{

	sc_time delay;

	while (true) {

		// wait command launch ...
		while (!isCommandLaunched()) {
			sc_module::wait(command_launch_event);
		}

		uint8_t cmd = (uint8_t) (fccob_reg[0] >> 8);
		switch (cmd) {
			case EraseVerifyAllBlocks: eraseVerifyAllBlocks_cmd(); break;
			case EraseVerifyBlock: eraseVerifyBlock_cmd(); break;
			case EraseVerifyPFlashSection: eraseVerifyPFlashSection_cmd(); break;
			case ReadOnce: readOnce_cmd(); break;
			case LoadDataField: loadDataField_cmd(); break;
			case ProgramPFlash: programPFlash_cmd(); break;
			case ProgramOnce: programOnce_cmd(); break;
			case EraseAllBlocks: eraseAllBlocks_cmd(); break;
			case ErasePFlashBlock: erasePFlashBlock_cmd(); break;
			case ErasePFlashSector: erasePFlashSector_cmd(); break;
			case UnsecureFlash: unsecureFlash_cmd(); break;
			case VerifyBackdoorAccessKey: verifyBackdoorAccessKey_cmd(); break;
			case SetUserMarginLevel: setUserMarginLevel_cmd(); break;
			case SetFieldMarginLevel: setFieldMarginLevel_cmd(); break;
			case FullPartitionDFlash: fullPartitionDFlash_cmd(); break;
			case EraseVerifyDFlashSector: eraseVerifyDFlashSection_cmd(); break;
			case ProgramDFlash: programDFlash_cmd(); break;
			case EraseDFlashSector: eraseDFlashSector_cmd(); break;
			case EnableEEPROMEmulation: enableEEPROMEmulation_cmd(); break;
			case DisableEEPROMEmulation: disableEEPROMEmulation_cmd(); break;
			case EEPROMEmulationQuery: eEPROMEmulationQuery_cmd(); break;
			case PartitionDFlash: partitionDFlash_cmd(); break;
			default: {
				// Invalid command
				inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Invalid command." << std::endl << EndDebugWarning;
				setACCERR();
			} break;
		}

		setCCIF();
	}

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::eraseVerifyAllBlocks_cmd()
{
	// Erase Verify All Blocks command will verify that all P-Flash and D-Flash blocks have been erased (i.e. all bytes=0xFF)

	if (fccobix_reg != 0) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		setACCERR();
		return;
	}

	// TODO: I have to emulate erase error

	// TODO: FSTAT::MGSTAT1 is set if any errors have been encountered during the read
	// TODO: FSTAT::MGSTAT0 is set if any non-correctable errors have been encountered during the read

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::eraseVerifyBlock_cmd()
{
	// Erase Verify Block command allows the user to verify that an entire P-Flash or D-Flash block has been erased.

	physical_address_t block_addr = ((physical_address_t) (fccob_reg[0] & 0x00FF) << 16);

	if (fccobix_reg != 0) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		setACCERR();
		return;
	}

	// Set ACCERR if an invalid global address [22:16] is supplied
	bool find = false;
	find |= (((dflash_start_address ^ block_addr) & 0xFF0000) == 0);
	for (uint8_t i=0; (i < pflash_blocks_description.size()) && !find; i++) {
		find |= (((pflash_blocks_description[i]->start_address ^ block_addr) & 0xFF0000) == 0);
	}

	if (!find) {
		setACCERR();
		return;
	}

	// TODO: I have to emulate erase error

	// TODO: FSTAT::MGSTAT1 is set if any errors have been encountered during the read
	// TODO: FSTAT::MGSTAT0 is set if any non-correctable errors have been encountered during the read

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::eraseVerifyPFlashSection_cmd()
{
	// Erase Verify P-Flash Section command will verify that a section of code in the P-Flash memory is erased

	physical_address_t addr = ((physical_address_t) (fccob_reg[0] & 0x00FF) << 16) | fccob_reg[1];
//	uint16_t number_phrases = fccob_reg[2]; // 1 phrase is a group of 8 bytes

	if (fccobix_reg != 2) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		setACCERR();
		return;
	}

	// Set ACCERR if an invalid global address [22:0] is supplied
	// Set ACCERR if the requested section crosses a 256K byte boundary
	bool find = false;
	for (uint8_t i=0; (i < pflash_blocks_description.size()) && !find; i++) {
		find |= ((pflash_blocks_description[i]->start_address <= addr) && (pflash_blocks_description[i]->end_address >= (addr + PFLASH_SECTION_SIZE -  1)));
	}

	if (!find) {
		setACCERR();
		return;
	}

	// Set ACCERR if a misaligned phrase address is supplied (global_address[2:0] != 000)
	if ((addr & 0x7) != 0) {
		setACCERR();
		return;
	}

	// TODO: I have to emulate erase error

	// TODO: FSTAT::MGSTAT1 is set if any errors have been encountered during the read
	// TODO: FSTAT::MGSTAT0 is set if any non-correctable errors have been encountered during the read

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::readOnce_cmd()
{
	// The Read Once command provides read access to a reserved 64 byte field (8 phrases each phrase is 8 bytes)
	// located in the non-volatile information register of P-Flash block 0.

	if (fccobix_reg != 2) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		setACCERR();
		return;
	}

	uint16_t phrase_index = fccob_reg[1];

	if (phrase_index > 7) {
		setACCERR();
		return;
	}

	uint16_t word;
	physical_address_t phrase_address = (pflash_blocks_description[0])->start_address + (phrase_index*PHRASE_SIZE);
	for (uint8_t i=0; i<4; i++) {
		inherited::ReadMemory(phrase_address + (i*WORD_SIZE), &word, WORD_SIZE);
		fccob_reg[2 + i] = BigEndian2Host(word);
		// TODO: FSTAT::MGSTAT1 is set if any errors have been encountered during the read
		// TODO: FSTAT::MGSTAT0 is set if any non-correctable errors have been encountered during the read
	}

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::loadDataField_cmd()
{
	// Load Data Field Command is executed to provide FCCOB parameters for multiple P-Flash blocks
	// for a future simultaneous program operation in the P-Flash memory space.

	/**
	 * This command allows you to program multiple phrases into separate flash blocks simultaneously.
	 * Basically you load up the data matching the specification in the command but it will not program
	 * until you launch the program P-Flash command.
	 * Used appropriately, this can save time to program the whole flash
	 */

	if (fccobix_reg != 5) {
		setACCERR();
		return;
	}

	physical_address_t addr = ((physical_address_t) (fccob_reg[0] & 0x00FF) << 16) | fccob_reg[1];

	// Set FSAT::ACCERR if an invalid global address [22:0] is supplied
	bool find = false;
	for (uint8_t i=0; (i < pflash_blocks_description.size()) && !find; i++) {
		find |= ((pflash_blocks_description[i]->start_address <= addr) && (pflash_blocks_description[i]->end_address >= addr));
	}

	if (!find) {
		setACCERR();
		return;
	}

	// Set FSAT::ACCCERR if a misaligned phrase address is supplied (addr[2:0] != 000)
	if ((addr & 0x7) != 0) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		for (unsigned int i = 0; i < loadDataFieldBuffer.size(); ++i) {
			// set FSAT::ACCERR if the selected block has previously been selected in the same command sequence
			if (((loadDataFieldBuffer[i])[0] & 0xFF) == (fccob_reg[0] & 0xFF)) {
				setACCERR();
				return;
			}

			// set FSAT::ACCERR if global address [17:0] does not match that previously supplied in the same command sequence
			physical_address_t loaded_addr = ((physical_address_t) ((loadDataFieldBuffer[i])[0] & 0x00FF) << 16) | (loadDataFieldBuffer[i])[1];
			if ( (loaded_addr & 0x03FFFF) != (addr & 0x03FFFF)) {
				setACCERR();
				return;
			}
		}

	}

	// set FSAT::FPVIOL if the global address [22:0] points to a protected area
	if (isPFlashProtected(addr, addr + PHRASE_SIZE - 1)) {
		setFPVIOL();
		return;
	}

	uint16_t *fccob_data = (uint16_t *) malloc(6*sizeof(uint16_t));
	memcpy (fccob_data, fccob_reg, 6*sizeof(uint16_t));

	loadDataFieldBuffer.push_back(fccob_data);


}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::programPFlash_cmd()
{
	/**
	 * The Program P-Flash operation will program a previously erased phrase in the P-Flash memory
	 * using an embedded algorithm.
	 */

	if (fccobix_reg != 5) {
		setACCERR();
		return;
	}

	physical_address_t addr = ((physical_address_t) (fccob_reg[0] & 0x00FF) << 16) | fccob_reg[1];

	// Set FSAT::ACCERR if an invalid global address [22:0] is supplied
	bool find = false;
	for (uint8_t i=0; (i < pflash_blocks_description.size()) && !find; i++) {
		find |= ((pflash_blocks_description[i]->start_address <= addr) && (pflash_blocks_description[i]->end_address >= addr));
	}

	if (!find) {
		setACCERR();
		return;
	}

	// Set FSAT::ACCCERR if a misaligned phrase address is supplied (addr[2:0] != 000)
	if ((addr & 0x7) != 0) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		for (unsigned int i = 0; i < loadDataFieldBuffer.size(); ++i) {
			// set FSAT::ACCERR if the selected block has previously been selected in the same command sequence
			if (((loadDataFieldBuffer[i])[0] & 0xFF) == (fccob_reg[0] & 0xFF)) {
				setACCERR();
				return;
			}

			// set FSAT::ACCERR if global address [17:0] does not match that previously supplied in the same command sequence
			physical_address_t loaded_addr = ((physical_address_t) ((loadDataFieldBuffer[i])[0] & 0x00FF) << 16) | (loadDataFieldBuffer[i])[1];
			if ( (loaded_addr & 0x03FFFF) != (addr & 0x03FFFF)) {
				setACCERR();
				return;
			}
		}

	}

	// set FSAT::FPVIOL if the global address [22:0] points to a protected area
	if (isPFlashProtected(addr, addr + PHRASE_SIZE - 1)) {
		setFPVIOL();
		return;
	}


	// execute effectif P-Flash program
	uint16_t word;
	for (uint8_t i=0; i<4; i++) {
		word = Host2BigEndian(fccob_reg[2 + i]);
		inherited::WriteMemory(addr + (i*WORD_SIZE), &word, WORD_SIZE);
		// TODO: FSTAT::MGSTAT1 is set if any errors have been encountered during the verify operation
		// TODO: FSTAT::MGSTAT0 is set if any non-correctable errors have been encountered during the verify operation
	}

	if (loadDataFieldBuffer.size() > 0) {
		for (unsigned int i = 0; i < loadDataFieldBuffer.size(); ++i) {
			physical_address_t loaded_addr = ((physical_address_t) ((loadDataFieldBuffer[i])[0] & 0x00FF) << 16) | (loadDataFieldBuffer[i])[1];

			for (uint8_t j=0; j<4; j++) {
				word = Host2BigEndian((loadDataFieldBuffer[i])[2 + j]);
				inherited::WriteMemory(loaded_addr + (j*WORD_SIZE), &word, WORD_SIZE);
				// TODO: FSTAT::MGSTAT1 is set if any errors have been encountered during the verify operation
				// TODO: FSTAT::MGSTAT0 is set if any non-correctable errors have been encountered during the verify operation
			}

			free (loadDataFieldBuffer[i]);
		}
		loadDataFieldBuffer.clear();

	}

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::programOnce_cmd()
{
	/**
	 * The Program Once command restricts programming to a reserved 64 byte field (8 phrases) in the
	 * nonvolatile information register located in P-Flash block 0.
	 */

//	physical_address_t addr = ((physical_address_t) (fccob_reg[0] & 0x00FF) << 16) | fccob_reg[1];

	if (fccobix_reg != 5) {
		setACCERR();
		return;
	}

	uint16_t phrase_index = fccob_reg[1];

	if (phrase_index > 7) {
		setACCERR();
		return;
	}

	uint16_t word;
	physical_address_t phrase_address = (pflash_blocks_description[0])->start_address + (phrase_index*PHRASE_SIZE);
	for (uint8_t i=0; i<4; i++) {
		word = Host2BigEndian(fccob_reg[2 + i]);
		inherited::WriteMemory(phrase_address + (i*WORD_SIZE), &word, WORD_SIZE);
		// TODO: FSTAT::MGSTAT1 is set if any errors have been encountered during the verify operation
		// TODO: FSTAT::MGSTAT0 is set if any non-correctable errors have been encountered during the verify operation
	}

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::eraseAllBlocks_cmd()
{
	/**
	 * The Erase All Blocks operation will erase the entire P-Flash and D-Flash memory space
	 * including the EEE non-volatile information register.
	 */

	if (fccobix_reg != 0) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		setACCERR();
		return;
	}

	// set FSTAT::FPVIOL if any area of the P-Flash memory is protected
	bool find = false;
	find |= (!((fprot_reg & 0x80) != 0) != !((fprot_reg & 0x20) == 0));
	find |= (!((fprot_reg & 0x80) != 0) != !((fprot_reg & 0x04) == 0));
	if (find) {
		setFPVIOL();
		return;
	}

	// set FERSTAT::EPVIOLIF if any area of the buffer RAM EEE partition is protected
	if (!((eprot_reg & 0x80) != 0) != !((eprot_reg & 0x08) == 0)) {
		setEPVIOLIF();
		return;
	}

	// Erase All blocks of P-Flash
	void *buffer = malloc(PFLASH_SECTOR_SIZE);
	memset(buffer, 0xFF , PFLASH_SECTOR_SIZE);
	for (unsigned int i = 0; i < pflash_blocks_description.size(); ++i) {
		uint16_t nbre_sector = (pflash_blocks_description[i]->end_address - pflash_blocks_description[i]->start_address + 1) / PFLASH_SECTOR_SIZE;
		for (uint16_t j=0; j < nbre_sector; j++) {
			WriteMemory(pflash_blocks_description[i]->start_address + (j * PFLASH_SECTOR_SIZE), buffer, PFLASH_SECTOR_SIZE);
		}
	}
	free(buffer);

	// Erase All blocks of D-Flash
	buffer = malloc(DFLASH_SECTOR_SIZE);
	memset(buffer, 0xFF , DFLASH_SECTOR_SIZE);
	uint16_t nbre_sector = (dflash_end_address - dflash_start_address + 1) / DFLASH_SECTOR_SIZE;
	for (uint16_t i=0; i < nbre_sector; i++) {
		WriteMemory(dflash_start_address + (i * DFLASH_SECTOR_SIZE), buffer, DFLASH_SECTOR_SIZE);
	}
	free(buffer);


//	// Erase the EEE non-volatile information register
	buffer = malloc(EEE_NON_VOLATILE_SPACE_SIZE);
	memset(buffer, 0xFF , EEE_NON_VOLATILE_SPACE_SIZE);
	WriteMemory(dflash_partition_user_access_address, buffer, EEE_NON_VOLATILE_SPACE_SIZE);
	free(buffer);

	// TODO: FSTAT::MGSTAT1 is set if any errors have been encountered during the verify operation
	// TODO: FSTAT::MGSTAT0 is set if any non-correctable errors have been encountered during the verify operation

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::erasePFlashBlock_cmd()
{

	// The Erase P-Flash Block operation will erase all addresses in a P-Flash block

	if (fccobix_reg != 1) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		setACCERR();
		return;
	}

	physical_address_t addr = ((physical_address_t) (fccob_reg[0] & 0x00FF) << 16) | fccob_reg[1];

	// set FSTAT::ACCERR if an invalid global address [22:16] is supplied
	bool find = false;
	for (uint8_t i=0; (i < pflash_blocks_description.size()) && !find; i++) {
		find |= (((pflash_blocks_description[i]->start_address ^ addr) & 0xFF0000) == 0);
	}

	if (!find) {
		setACCERR();
		return;
	}

	void *buffer = malloc(PFLASH_SECTOR_SIZE);
	memset(buffer, 0xFF , PFLASH_SECTOR_SIZE);
	for (unsigned int i = 0; i < pflash_blocks_description.size(); ++i) {
		// Identify the P-Flash block
		if ((addr >= pflash_blocks_description[i]->start_address)
				&& (addr <= pflash_blocks_description[i]->end_address))
		{
			// set FSTAT::FPVIOL if an area of the selected P-Flash block is protected
			if (isPFlashProtected(pflash_blocks_description[i]->start_address, pflash_blocks_description[i]->end_address)) {
				setFPVIOL();
				break;
			}

			uint16_t nbre_sector = (pflash_blocks_description[i]->end_address - pflash_blocks_description[i]->start_address + 1) / PFLASH_SECTOR_SIZE;
			for (uint16_t j=0; j < nbre_sector; j++) {
				WriteMemory(pflash_blocks_description[i]->start_address + (j * PFLASH_SECTOR_SIZE), buffer, PFLASH_SECTOR_SIZE);
			}
		}
	}
	free(buffer);

	// TODO: FSTAT::MGSTAT1 is set if any errors have been encountered during the verify operation
	// TODO: FSTAT::MGSTAT0 is set if any non-correctable errors have been encountered during the verify operation

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::erasePFlashSector_cmd()
{

	// The Erase P-Flash sector operation will erase all addresses in a p-Flash sector

	if (fccobix_reg != 1) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		setACCERR();
		return;
	}

	physical_address_t addr = ((physical_address_t) (fccob_reg[0] & 0x00FF) << 16) | fccob_reg[1];

	// Set FSTAT::ACCERR if an invalid global address [22:16] is supplied
	bool find = false;
	for (uint8_t i=0; (i < pflash_blocks_description.size()) && !find; i++) {
		find |= (((pflash_blocks_description[i]->start_address ^ addr) & 0xFF0000) == 0);
	}

	if (!find) {
		setACCERR();
		return;
	}

	void *buffer = malloc(PFLASH_SECTOR_SIZE);
	memset(buffer, 0xFF , PFLASH_SECTOR_SIZE);
	for (unsigned int i = 0; i < pflash_blocks_description.size(); ++i) {
		uint16_t nbre_sector = (pflash_blocks_description[i]->end_address - pflash_blocks_description[i]->start_address + 1) / PFLASH_SECTOR_SIZE;
		for (uint16_t j=0; j < nbre_sector; j++) {
			// Identify the P-Flash sector
			physical_address_t sector_addr = pflash_blocks_description[i]->start_address + (j * PFLASH_SECTOR_SIZE);
			if ((addr >= sector_addr)
					&& (addr <= (sector_addr + PFLASH_SECTOR_SIZE - 1)))
			{
				// set FSTAT::FPVIOL if the selected P-Flash sector is protected
				if (isPFlashProtected(sector_addr, sector_addr + PFLASH_SECTOR_SIZE - 1)) {
					setFPVIOL();
					break;
				}

				WriteMemory(sector_addr, buffer, PFLASH_SECTOR_SIZE);
			}
		}

	}
	free(buffer);

	// TODO: FSTAT::MGSTAT1 is set if any errors have been encountered during the verify operation
	// TODO: FSTAT::MGSTAT0 is set if any non-correctable errors have been encountered during the verify operation


}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::unsecureFlash_cmd()
{

	/**
	 * The Unsecure Flash command will erase the entire P-Flash and D-Flash memory space and,
	 * if the erase is successful, will release security.
	 */

	if (fccobix_reg != 0) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		setACCERR();
		return;
	}

	// set FSTAT::FPVIOL if any area of the P-Flash memory is protected
	bool find = false;
	find |= (!((fprot_reg & 0x80) != 0) != !((fprot_reg & 0x20) == 0));
	find |= (!((fprot_reg & 0x80) != 0) != !((fprot_reg & 0x04) == 0));
	if (find) {
		setFPVIOL();
		return;
	}

	// Erase All blocks of P-Flash
	void *buffer = malloc(PFLASH_SECTOR_SIZE);
	memset(buffer, 0xFF , PFLASH_SECTOR_SIZE);
	for (unsigned int i = 0; i < pflash_blocks_description.size(); ++i) {
		uint16_t nbre_sector = (pflash_blocks_description[i]->end_address - pflash_blocks_description[i]->start_address + 1) / PFLASH_SECTOR_SIZE;
		for (uint16_t j=0; j < nbre_sector; j++) {
			WriteMemory(pflash_blocks_description[i]->start_address + (j * PFLASH_SECTOR_SIZE), buffer, PFLASH_SECTOR_SIZE);
		}
	}
	free(buffer);

	// Erase All blocks of D-Flash
	buffer = malloc(DFLASH_SECTOR_SIZE);
	memset(buffer, 0xFF , DFLASH_SECTOR_SIZE);
	uint16_t nbre_sector = (dflash_end_address - dflash_start_address + 1) / DFLASH_SECTOR_SIZE;
	for (uint16_t i=0; i < nbre_sector; i++) {
		WriteMemory(dflash_start_address + (i * DFLASH_SECTOR_SIZE), buffer, DFLASH_SECTOR_SIZE);
	}
	free(buffer);

	// TODO: FSTAT::MGSTAT1 is set if any errors have been encountered during the verify operation
	// TODO: FSTAT::MGSTAT0 is set if any non-correctable errors have been encountered during the verify operation

	// If the memory controller verifies that the entire flash memory space was properly erased, security will be released.
	unsecureFlash();
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::verifyBackdoorAccessKey_cmd()
{

	// The verify Backdoor Access Key command release security
	// if user supplied keys match those stored in the Flash security bytes of the Flash configuration field.

	if (fccobix_reg != 0) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		setACCERR();
		return;
	}


	if (isBackdoorkeySecurityEnabled() && !verifyBackdoorAccessKey_Failed) {
		uint16_t buffer[BACKDOOR_ACCESS_KEY_SIZE/2];
		ReadMemory(blackdoor_comparison_key_address, buffer, BACKDOOR_ACCESS_KEY_SIZE);

		bool success = true;
		for (uint8_t i=0; (i<4) && success; i++) {
			success &= (buffer[i] == Host2BigEndian(fccob_reg[1+i]));
		}

		if (success) {
			unsecureFlash();
		} {
			verifyBackdoorAccessKey_Failed = true;
		}
	} else {
		setACCERR();
		return;
	}

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::setUserMarginLevel_cmd()
{

	/**
	 * User margin levels can be used to check that Flash memory contents have adequate margin for normal level read operations.
	 * If unexpected results are encountered when checking Flash memory contents at user margin levels,
	 * a potential loss of information has been detected.
	 */

	inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Set User Margin Level Command is not implemented." << std::endl << EndDebugWarning;

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::setFieldMarginLevel_cmd()
{

	/**
	 * Field margin levels must only be used during verify of the initial factory programming.
	 *
	 * Field margin levels can be used to check that Flash memory contents have adequate margin
	 * for data retention at the normal level setting.
	 * If unexpected results are encountered when checking Flash memory contents at field
	 * margin levels, the Flash memory contents should be erased and reprogrammed.
	 */

	inherited::logger << DebugWarning << " : " << sc_object::name() << ":: Set Field Margin Level Command is not implemented." << std::endl << EndDebugWarning;

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::fullPartitionDFlash_cmd()
{

//	physical_address_t addr = ((physical_address_t) (fccob_reg[0] & 0x00FF) << 16) | fccob_reg[1];
	uint16_t nbre_DFlash_user_sector = fccob_reg[1];
	uint16_t nbre_bufferRAM_EEE_sector = fccob_reg[2];

	if (fccobix_reg != 2) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		setACCERR();
		return;
	}

	// TODO: FSTAT::ACCERR is set if command not available in current mode

	// Validate the DFPART and ERPART values
	if (nbre_DFlash_user_sector > max_number_sectors_dflash) {
		setACCERR();
		return;
	}

	if (nbre_bufferRAM_EEE_sector > max_number_sectors_buffer_ram) {
		setACCERR();
		return;
	}

	if (nbre_bufferRAM_EEE_sector > 0) {
		if ((max_number_sectors_dflash - nbre_DFlash_user_sector) < min_number_sectors_in_dflash_for_eee) {
			setACCERR();
			return;
		}

		if (((max_number_sectors_dflash - nbre_DFlash_user_sector)/nbre_bufferRAM_EEE_sector) < min_ratio_dflash_buffer_ram) {
			setACCERR();
			return;
		}
	}

	// TODO: FSTAT::MGSTAT1 set if any errors have been encountered during the read
	// TODO: FSTAT::MGSTAT0 set if any non-correctable errors have been encountered during the read

	// Erase the D-Flash block
	void *buffer = malloc(256);
	memset(buffer, 0xFF , 256);
	for (uint16_t i=0; i < nbre_DFlash_user_sector; i++) {
		WriteMemory(dflash_start_address + (i * 256), buffer, 256);
	}
	free(buffer);

	// Erase the EEE non-volatile information register
	buffer = malloc(8);
	memset(buffer, 0xFF , 8);
	WriteMemory(dflash_partition_user_access_address, buffer, 8);
	free(buffer);

	// Program DFPART to the EEE non-volatile information register at global address 0x12_0000
	WriteMemory(dflash_partition_user_access_address, &nbre_DFlash_user_sector, 2);

	// Program a duplicate DFPART to the EEE non-volatile information register at global address	0x12_0002
	WriteMemory(dflash_partition_user_access_address_duplicate, &nbre_DFlash_user_sector, 2);

	// Program ERPART to the EEE non-volatile information register at global address 0x12_0004
	WriteMemory(buffer_ram_partition_eee_operation_address, &nbre_bufferRAM_EEE_sector, 2);

	// Program a duplicate ERPART to the EEE non-volatile information register at global address	0x12_0006
	WriteMemory(buffer_ram_partition_eee_operation_address_duplicate, &nbre_bufferRAM_EEE_sector, 2);

	fullPartitionDFlashCmd_Launched =true;
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::eraseVerifyDFlashSection_cmd()
{

	physical_address_t addr = ((physical_address_t) (fccob_reg[0] & 0x00FF) << 16) | fccob_reg[1];

	uint16_t number_words = fccob_reg[2];

	if (fccobix_reg != 2) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		setACCERR();
		return;
	}

	if ((addr & 0x1) != 0) {
		setACCERR();
		return;
	}

	// Set ACCERR if an invalid global address [22:0] is supplied
	// Set ACCERR if the requested section breaches the end of the D-Flash block
	if ((dflash_start_address > addr) || (dflash_end_address < (addr + (number_words * WORD_SIZE) - 1) )) {
		setACCERR();
		return;
	}

	// Set ACCERR if the global address [22:0] points to an area of the D-Flash EEE partition
	// Set ACCERR if the requested section goes into the D-Flash EEE partition

	// Get D-Flash user partition size
	uint16_t nbre_DFlash_user_sector = 0;
	ReadMemory(dflash_partition_user_access_address, &nbre_DFlash_user_sector, 2);

	if ((dflash_start_address + nbre_DFlash_user_sector * DFLASH_SECTOR_SIZE) < (addr + number_words * WORD_SIZE) ) {
		setACCERR();
		return;
	}

	// TODO: I have to emulate erase error

	// TODO: FSTAT::MGSTAT1 is set if any errors have been encountered during the read
	// TODO: FSTAT::MGSTAT0 is set if any non-correctable errors have been encountered during the read


}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::programDFlash_cmd()
{

	physical_address_t addr = ((physical_address_t) (fccob_reg[0] & 0x00FF) << 16) | fccob_reg[1];

	uint8_t number_words = fccobix_reg - 0x2;

	if ((fccobix_reg < 0x2) || (fccobix_reg > 0x5)) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		setACCERR();
		return;
	}

	// TODO: Set FSTAT::ACCERR if command not available in current mode (Table 29-30)

	// Set FSTAT::ACCERR if an invalid global address [22:0] is supplied
	// Set FSTAT::ACCERR if the requested group of words breaches the end of the D-Flash block
	if ((dflash_start_address > addr) || (dflash_end_address < (addr + (number_words * WORD_SIZE) - 1) )) {
		setACCERR();
		return;
	}

	// Set FSTAT::ACCERR if a misaligned word address is supplied
	if ((addr & 0x1) != 0) {
		setACCERR();
		return;
	}

	// Set ACCERR if the global address [22:0] points to an area of the D-Flash EEE partition
	// Set ACCERR if the requested section goes into the D-Flash EEE partition

	// Get D-Flash user partition size
	uint16_t nbre_DFlash_user_sector = 0;
	ReadMemory(dflash_partition_user_access_address, &nbre_DFlash_user_sector, 2);

	if ((dflash_start_address + nbre_DFlash_user_sector * DFLASH_SECTOR_SIZE) < (addr + number_words * WORD_SIZE) ) {
		setACCERR();
		return;
	}


	// execute effective D-Flash program
	uint16_t word;
	for (uint8_t i=0; i < number_words; i++) {
		word = Host2BigEndian(fccob_reg[0x2 + i]);
		inherited::WriteMemory(addr + (i*WORD_SIZE), &word, WORD_SIZE);
		// TODO: FSTAT::MGSTAT1 is set if any errors have been encountered during the verify operation
		// TODO: FSTAT::MGSTAT0 is set if any non-correctable errors have been encountered during the verify operation
	}


}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::eraseDFlashSector_cmd()
{

	physical_address_t addr = ((physical_address_t) (fccob_reg[0] & 0x00FF) << 16) | fccob_reg[1];

	if ((fccobix_reg & 0x1) != 0) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		setACCERR();
		return;
	}

	// TODO: set FSTAT::ACCERR if command not available in current mode

	// Set FSTAT::ACCERR if an invalid global address [22:0] is supplied
	if ((dflash_start_address > addr) || (dflash_end_address < (addr + DFLASH_SECTOR_SIZE - 1) )) {
		setACCERR();
		return;
	}

	// Set FSTAT::ACCERR if a misaligned word address is supplied
	if ((addr & 0x1) != 0) {
		setACCERR();
		return;
	}

	// set FSAT::ACCERR if the global address [22:0] points to the D-Flash EEE partition

	// Get D-Flash user partition size
	uint16_t nbre_DFlash_user_sector = 0;
	ReadMemory(dflash_partition_user_access_address, &nbre_DFlash_user_sector, 2);

	if ((dflash_start_address + nbre_DFlash_user_sector * DFLASH_SECTOR_SIZE) < (addr + DFLASH_SECTOR_SIZE) ) {
		setACCERR();
		return;
	}

	// execute erase D-Flash sector command
	void *buffer = malloc(DFLASH_SECTOR_SIZE);
	memset(buffer, 0xFF , DFLASH_SECTOR_SIZE);
	uint16_t nbre_sector = (dflash_end_address - dflash_start_address + 1) / DFLASH_SECTOR_SIZE;
	for (uint16_t j=0; j < nbre_sector; j++) {
		// Identify the D-Flash sector
		if ((addr >= (dflash_start_address + (j * DFLASH_SECTOR_SIZE)))
				&& (addr <= (dflash_start_address + (j * DFLASH_SECTOR_SIZE) + DFLASH_SECTOR_SIZE - 1)))
		{

			WriteMemory(dflash_start_address + (j * DFLASH_SECTOR_SIZE), buffer, DFLASH_SECTOR_SIZE);
		}
	}
	free(buffer);


	// TODO: FSTAT::MGSTAT1 is set if any errors have been encountered during the verify operation
	// TODO: FSTAT::MGSTAT0 is set if any non-correctable errors have been encountered during the verify operation

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::enableEEPROMEmulation_cmd()
{

	if (fccobix_reg != 0) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		setACCERR();
		return;
	}

	if (!isFullPartitionDFlashCmd_Launched() && !isPartitionDFlashCmd_Launched()) {
		setACCERR();
		return;
	}

	eepromEmulationEnabled = true;
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::disableEEPROMEmulation_cmd()
{

	if (fccobix_reg != 0) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		setACCERR();
		return;
	}

	if (!isFullPartitionDFlashCmd_Launched() && !isPartitionDFlashCmd_Launched()) {
		setACCERR();
		return;
	}

	eepromEmulationEnabled = false;

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::eEPROMEmulationQuery_cmd()
{

	if (fccobix_reg != 0) {
		setACCERR();
		return;
	}

	if (isLoadDataFieldCommandSequenceActive()) {
		setACCERR();
		return;
	}

	// TODO: FSTAT::ACCERR is set if command not available in the current mode

	 inherited::ReadMemory(dflash_partition_user_access_address, &fccob_reg[1], 2);
	 inherited::ReadMemory(buffer_ram_partition_eee_operation_address, &fccob_reg[2], 2);
	 fccob_reg[3] = sector_erased_count;

	 /**
	  *  TODO:
	  * 	implement dead_sector_count
	  * 	implement ready_sector_count;
	  */

	 if ((fccob_reg[1] == 0xFFFF) && (fccob_reg[2] == 0xFFFF)) {
		 fccob_reg[4] = 0;
	 } else {
		 fccob_reg[4] = fccob_reg[1] + fccob_reg[2];
	 }

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::partitionDFlash_cmd()
{

	if (isPartitionDFlashCmd_Launched()) {
		setACCERR();
		return;
	}

	fullPartitionDFlash_cmd();

	partitionDFlashCmd_Launched = true;
}

/* Service methods */
/** BeginSetup
 * Initializes the service interface. */
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::BeginSetup()
{
	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".FCLKDIV", &fclkdiv_reg));

	unisim::kernel::variable::Register<uint8_t> *fclkdiv_var = new unisim::kernel::variable::Register<uint8_t>("FCLKDIV", this, fclkdiv_reg, "FLASH Clock Divider Register (FCLKDIV)");
	extended_registers_registry.push_back(fclkdiv_var);
	fclkdiv_var->setCallBack(this, FCLKDIV, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".FSEC", &fsec_reg));

	unisim::kernel::variable::Register<uint8_t> *fsec_var = new unisim::kernel::variable::Register<uint8_t>("FSEC", this, fsec_reg, "FLASH Security Register (FSEC)");
	extended_registers_registry.push_back(fsec_var);
	fsec_var->setCallBack(this, FSEC, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".FCCOBIX", &fccobix_reg));

	unisim::kernel::variable::Register<uint8_t> *fccobix_var = new unisim::kernel::variable::Register<uint8_t>("FCCOBIX", this, fccobix_reg, "FLASH CCOB index Register (FCCOBIX)");
	extended_registers_registry.push_back(fccobix_var);
	fccobix_var->setCallBack(this, FCCOBIX, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".FECCRIX", &feccrix_reg));

	unisim::kernel::variable::Register<uint8_t> *feccrix_var = new unisim::kernel::variable::Register<uint8_t>("FECCRIX", this, feccrix_reg, "FLASH ECCR index Register (FECCRIX)");
	extended_registers_registry.push_back(feccrix_var);
	feccrix_var->setCallBack(this, FECCRIX, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".FCNFG", &fcnfg_reg));

	unisim::kernel::variable::Register<uint8_t> *fcnfg_var = new unisim::kernel::variable::Register<uint8_t>("FCNFG", this, fcnfg_reg, "FLASH Configuration Register (FCNFG)");
	extended_registers_registry.push_back(fcnfg_var);
	fcnfg_var->setCallBack(this, FCNFG, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".FERCNFG", &fercnfg_reg));

	unisim::kernel::variable::Register<uint8_t> *fercnfg_var = new unisim::kernel::variable::Register<uint8_t>("FERCNFG", this, fercnfg_reg, "FLASH Error Configuration Register (FERCNFG)");
	extended_registers_registry.push_back(fercnfg_var);
	fercnfg_var->setCallBack(this, FERCNFG, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".FSTAT", &fstat_reg));

	unisim::kernel::variable::Register<uint8_t> *fstat_var = new unisim::kernel::variable::Register<uint8_t>("FSTAT", this, fstat_reg, "FLASH Status Register (FSTAT)");
	extended_registers_registry.push_back(fstat_var);
	fstat_var->setCallBack(this, FSTAT, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".FERSTAT", &ferstat_reg));

	unisim::kernel::variable::Register<uint8_t> *ferstat_var = new unisim::kernel::variable::Register<uint8_t>("FERSTAT", this, ferstat_reg, "FLASH error Status Register (FERSTAT)");
	extended_registers_registry.push_back(ferstat_var);
	ferstat_var->setCallBack(this, FERSTAT, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".FPROT", &fprot_reg));

	unisim::kernel::variable::Register<uint8_t> *fprot_var = new unisim::kernel::variable::Register<uint8_t>("FPROT", this, fprot_reg, "P-FLASH Protection Register (FPROT)");
	extended_registers_registry.push_back(fprot_var);
	fprot_var->setCallBack(this, FPROT, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".EPROT", &eprot_reg));

	unisim::kernel::variable::Register<uint8_t> *eprot_var = new unisim::kernel::variable::Register<uint8_t>("EPROT", this, eprot_reg, "EEE Protection Register (EPROT)");
	extended_registers_registry.push_back(eprot_var);
	eprot_var->setCallBack(this, EPROT, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".FCCOB000", &(fccob_reg[0])));

	unisim::kernel::variable::Register<uint16_t> *fccob000_var = new unisim::kernel::variable::Register<uint16_t>("FCCOB000", this, fccob_reg[0], "Flash Common Command Object Register (FCCOB000)");
	extended_registers_registry.push_back(fccob000_var);
	fccob000_var->setCallBack(this, FCCOB000, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".FCCOB001", &(fccob_reg[1])));

	unisim::kernel::variable::Register<uint16_t> *fccob001_var = new unisim::kernel::variable::Register<uint16_t>("FCCOB001", this, fccob_reg[1], "Flash Common Command Object Register (FCCOB001)");
	extended_registers_registry.push_back(fccob001_var);
	fccob001_var->setCallBack(this, FCCOB001, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".FCCOB010", &fccob_reg[2]));

	unisim::kernel::variable::Register<uint16_t> *fccob010_var = new unisim::kernel::variable::Register<uint16_t>("FCCOB010", this, fccob_reg[2], "Flash Common Command Object Register (FCCOB010)");
	extended_registers_registry.push_back(fccob010_var);
	fccob010_var->setCallBack(this, FCCOB010, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".FCCOB011", &(fccob_reg[3])));

	unisim::kernel::variable::Register<uint16_t> *fccob011_var = new unisim::kernel::variable::Register<uint16_t>("FCCOB011", this, fccob_reg[3], "Flash Common Command Object Register (FCCOB011)");
	extended_registers_registry.push_back(fccob011_var);
	fccob011_var->setCallBack(this, FCCOB011, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".FCCOB100", &(fccob_reg[4])));

	unisim::kernel::variable::Register<uint16_t> *fccob100_var = new unisim::kernel::variable::Register<uint16_t>("FCCOB100", this, fccob_reg[4], "Flash Common Command Object Register (FCCOB100)");
	extended_registers_registry.push_back(fccob100_var);
	fccob100_var->setCallBack(this, FCCOB100, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".FCCOB101", &(fccob_reg[5])));

	unisim::kernel::variable::Register<uint16_t> *fccob101_var = new unisim::kernel::variable::Register<uint16_t>("FCCOB101", this, fccob_reg[5], "Flash Common Command Object Register (FCCOB101)");
	extended_registers_registry.push_back(fccob101_var);
	fccob101_var->setCallBack(this, FCCOB101, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".ETAG", &etag_reg));

	unisim::kernel::variable::Register<uint16_t> *etag_var = new unisim::kernel::variable::Register<uint16_t>("ETAG", this, etag_reg, "EEE Tag Counter Register (ETAG)");
	extended_registers_registry.push_back(etag_var);
	etag_var->setCallBack(this, ETAGHI, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".FECCR000", &(feccr_reg[0])));

	unisim::kernel::variable::Register<uint16_t> *feccr000_var = new unisim::kernel::variable::Register<uint16_t>("FECCR000", this, feccr_reg[0], "Flash ECC Error Results Register (FECCR000)");
	extended_registers_registry.push_back(feccr000_var);
	feccr000_var->setCallBack(this, FECCR000, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".FECCR001", &(feccr_reg[1])));

	unisim::kernel::variable::Register<uint16_t> *feccr001_var = new unisim::kernel::variable::Register<uint16_t>("FECCR001", this, feccr_reg[1], "Flash ECC Error Results Register (FECCR001)");
	extended_registers_registry.push_back(feccr001_var);
	feccr001_var->setCallBack(this, FECCR001, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".FECCR010", &(feccr_reg[2])));

	unisim::kernel::variable::Register<uint16_t> *feccr010_var = new unisim::kernel::variable::Register<uint16_t>("FECCR010", this, feccr_reg[2], "Flash ECC Error Results Register (FECCR010)");
	extended_registers_registry.push_back(feccr010_var);
	feccr010_var->setCallBack(this, FECCR010, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".FECCR011", &(feccr_reg[3])));

	unisim::kernel::variable::Register<uint16_t> *feccr011_var = new unisim::kernel::variable::Register<uint16_t>("FECCR011", this, feccr_reg[3], "Flash ECC Error Results Register (FECCR011)");
	extended_registers_registry.push_back(feccr011_var);
	feccr011_var->setCallBack(this, FECCR011, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".FECCR100", &(feccr_reg[4])));

	unisim::kernel::variable::Register<uint16_t> *feccr100_var = new unisim::kernel::variable::Register<uint16_t>("FECCR100", this, feccr_reg[4], "Flash ECC Error Results Register (FECCR100)");
	extended_registers_registry.push_back(feccr100_var);
	feccr100_var->setCallBack(this, FECCR100, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".FECCR101", &(feccr_reg[5])));

	unisim::kernel::variable::Register<uint16_t> *feccr101_var = new unisim::kernel::variable::Register<uint16_t>("FECCR101", this, feccr_reg[5], "Flash ECC Error Results Register (FECCR101)");
	extended_registers_registry.push_back(feccr101_var);
	feccr101_var->setCallBack(this, FECCR101, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".FECCR110", &(feccr_reg[6])));

	unisim::kernel::variable::Register<uint16_t> *feccr110_var = new unisim::kernel::variable::Register<uint16_t>("FECCR110", this, feccr_reg[6], "Flash ECC Error Results Register (FECCR110)");
	extended_registers_registry.push_back(feccr110_var);
	feccr110_var->setCallBack(this, FECCR110, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(std::string(sc_object::name()) + ".FECCR111", &(feccr_reg[7])));

	unisim::kernel::variable::Register<uint16_t> *feccr111_var = new unisim::kernel::variable::Register<uint16_t>("FECCR111", this, feccr_reg[7], "Flash ECC Error Results Register (FECCR111)");
	extended_registers_registry.push_back(feccr111_var);
	feccr111_var->setCallBack(this, FECCR111, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".FOPT", &fopt_reg));

	unisim::kernel::variable::Register<uint8_t> *fopt_var = new unisim::kernel::variable::Register<uint8_t>("FOPT", this, fopt_reg, "Flash Option Register (FOPT)");
	extended_registers_registry.push_back(fopt_var);
	fopt_var->setCallBack(this, FOPT, &CallBackObject::write, NULL);

	oscillator_cycle_time = sc_time(oscillator_cycle_time_int, SC_PS);

	min_fclk_time = sc_time(min_fclk_time_int, SC_PS);
	max_fclk_time = sc_time(max_fclk_time_int, SC_PS);

	vector<string> result;
	stringSplit(pflash_blocks_description_string, ";", result);
	for (unsigned int i = 0; i < result.size(); ++i) {
		vector<string> oneBlockSegments;
		stringSplit(result[i], ",", oneBlockSegments);
		BlockDescription *oneBlock = new BlockDescription();
		std::stringstream ss;
		ss << std::hex << oneBlockSegments[0] << " " << std::hex << oneBlockSegments[1] << std::dec;
		ss >> std::hex >> oneBlock->start_address >> std::hex >> oneBlock->end_address;

		pflash_blocks_description.push_back(oneBlock);
		oneBlockSegments.clear();
	}
	result.clear();

	Reset();

	return (inherited::BeginSetup());
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::write_to_flash(physical_address_t address, void* data_ptr, unsigned int data_length) {

	WriteMemory(address, data_ptr, data_length);
}

/**
 * TLM2 Slave methods
 */
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	return (inherited::get_direct_mem_ptr(payload, dmi_data));
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
unsigned int S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::transport_dbg(tlm::tlm_generic_payload& payload)
{

	tlm::tlm_command cmd = payload.get_command();
	sc_dt::uint64 address = payload.get_address();
	void *data_ptr = payload.get_data_ptr();
	unsigned int data_length = payload.get_data_length();

	if ((address >= inherited::GetLowAddress()) && (address <= inherited::GetHighAddress())) {
		if (cmd == tlm::TLM_READ_COMMAND) {
			return (inherited::transport_dbg(payload));
		} else {
			write_to_flash( address, data_ptr, data_length);

			payload.set_response_status(tlm::TLM_OK_RESPONSE);

		}
	} else {
		payload.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
	}


	return (0);

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
tlm::tlm_sync_enum S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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
			validateFlashRead(address);

			return (inherited::nb_transport_fw(payload, phase, t));
		} else {

			write_to_flash(address, data_ptr, data_length);

			inherited::write_counter++;

			payload.set_response_status(tlm::TLM_OK_RESPONSE);

		}

	} else {
		payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
	}


	phase = tlm::BEGIN_RESP;

	return tlm::TLM_COMPLETED;

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{

	tlm::tlm_command cmd = payload.get_command();
	sc_dt::uint64 address = payload.get_address();
	void *data_ptr = payload.get_data_ptr();
	unsigned int data_length = payload.get_data_length();

	if ((address >= inherited::GetLowAddress()) && (address <= inherited::GetHighAddress())) {
		if (cmd == tlm::TLM_READ_COMMAND) {
			validateFlashRead(address);

			inherited::b_transport(payload, t);
		} else {

			write_to_flash(address, data_ptr, data_length);

			payload.set_response_status( tlm::TLM_OK_RESPONSE );
		}
	} else {
		payload.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
	}

}

// Master methods
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
tlm_sync_enum S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::assertInterrupt(uint8_t interrupt_offset) {

	// assert EEPROM_Command_Interrupt

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

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::Reset() {

	fclkdiv_reg = 0x00;
	fsec_reg = 0x00;
	inherited::ReadMemory(flash_security_byte_address, &fsec_reg, 1);
	fccobix_reg = 0x00;
	feccrix_reg = 0x00;
	fcnfg_reg = 0x00;
	fercnfg_reg = 0x00;
	fstat_reg = 0x80;
	ferstat_reg = 0x00;
	fprot_reg = 0x00;
	inherited::ReadMemory(pflash_protection_byte_address, &fprot_reg, 1);
	eprot_reg = 0x00;
	inherited::ReadMemory(eee_protection_byte_address, &eprot_reg, 1);
	fopt_reg = 0x00;
	inherited::ReadMemory(flash_nonvolatile_byte_address, &fopt_reg, 1);
	frsv0_reg = 0x00;
	frsv1_reg = 0x00;
	frsv2_reg = 0x00;
	for (uint8_t i=0; i<6; i++) {
		fccob_reg[i] = 0x00;
	}

	etag_reg = 0x0000;

	fprot_high_low_addr = pflash_protectable_high_address - (pow(2, (((fprot_reg & 0x18) >> 3) + 1)) * 1024) + 1;
	fprot_low_high_addr = pflash_protectable_low_address - (pow(2, ((fprot_reg & 0x02))) * 1024);
	eprot_low_addr = eee_protectable_high_address - (((eprot_reg & 0x07) + 1) * 64) + 1;

	partitionDFlashCmd_Launched = false;
	fullPartitionDFlashCmd_Launched = false;
	eepromEmulationEnabled = false;

	ComputeInternalTime();

	setFLASHClock();

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::setFLASHClock() {

	if ((fclkdiv_reg & 0x80) != 0) {
		fclk_time = oscillator_cycle_time * ((fclkdiv_reg & 0x7F) + 1);

		if ((fclk_time < max_fclk_time) || (fclk_time > min_fclk_time)) {
			inherited::logger << DebugWarning << sc_object::name() << ":: Setting Flash CLK to " << ((1e-6)/fclk_time.to_seconds()) << " MHz can destroy the FLASH." << std::endl << EndDebugWarning;
		}
	}
}


//=====================================================================
//=             memory interface methods                              =
//=====================================================================

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::ReadMemory(ADDRESS addr, void *buffer, uint32_t size)
{
	if ((addr >= baseAddress) && (addr < (baseAddress+REGISTERS_BANK_SIZE))) {
		service_address_t offset = addr-baseAddress;
		switch (offset) {
			case FCLKDIV: {
				*((uint8_t *) buffer) = fclkdiv_reg;
			} break;
			case FSEC: {
				*((uint8_t *) buffer) = fsec_reg;
			} break;
			case FCCOBIX: {
				*((uint8_t *) buffer) = fccobix_reg;
			} break;
			case FECCRIX: {
				*((uint8_t *) buffer) = feccrix_reg;
			} break;
			case FCNFG: {
				*((uint8_t *) buffer) = fcnfg_reg;
			} break;
			case FERCNFG: {
				*((uint8_t *) buffer) = fercnfg_reg;
			} break;
			case FSTAT: {
				*((uint8_t *) buffer) = fstat_reg;
			} break;
			case FERSTAT: {
				*((uint8_t *) buffer) = ferstat_reg;
			} break;
			case FPROT: {
				*((uint8_t *) buffer) = fprot_reg;
			} break;
			case EPROT: {
				*((uint8_t *) buffer) = eprot_reg;
			} break;
			case FCCOBHI: {
				if (size == 2) {
					*((uint16_t *) buffer) = Host2BigEndian(fccob_reg[fccobix_reg]);
				} else {
					*((uint8_t *) buffer) = (uint8_t) (fccob_reg[fccobix_reg] >> 8);
				}
			} break;
			case FCCOBLO: {
				*((uint8_t *) buffer) = (uint8_t) (fccob_reg[fccobix_reg] & 0x00FF);
			} break;
			case ETAGHI: {
				if (size == 2) {
					*((uint16_t *) buffer) = Host2BigEndian(etag_reg);
				} else {
					*((uint8_t *) buffer) = (uint8_t) (etag_reg >> 8);
				}
			} break;
			case ETAGLO: {
				*((uint8_t *) buffer) = (uint8_t) (etag_reg & 0x00FF);
			} break;
			case FECCRHI: {
				if (size == 2) {
					*((uint16_t *) buffer) = Host2BigEndian(feccr_reg[feccrix_reg]);
				} else {
					*((uint8_t *) buffer) = (uint8_t) (feccr_reg[feccrix_reg] >> 8);
				}
			} break;
			case FECCRLO: {
				*((uint8_t *) buffer) = (uint8_t) (feccr_reg[feccrix_reg] & 0x00FF);
			} break;
			case FOPT: {
				*((uint8_t *) buffer) = fopt_reg;
			} break;
			case FRSV0: {
				*((uint8_t *) buffer) = 0;
			} break;
			case FRSV1: {
				*((uint8_t *) buffer) = 0;
			} break;
			case FRSV2: {
				*((uint8_t *) buffer) = 0;
			} break;

			default: return (false);
		}

		return (true);
	}

	return (inherited::ReadMemory(addr, buffer, size));
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::WriteMemory(ADDRESS addr, const void *buffer, uint32_t size)
{

	if ((addr >= baseAddress) && (addr < (baseAddress+REGISTERS_BANK_SIZE))) {

		if (size == 0) {
			return (true);
		}

		service_address_t offset = addr-baseAddress;
		switch (offset) {
			case FCLKDIV: {
				 fclkdiv_reg = *((uint8_t *) buffer);
			} break;
			case FSEC: {
				 fsec_reg = *((uint8_t *) buffer);
			} break;
			case FCCOBIX: {
				 fccobix_reg = *((uint8_t *) buffer);
			} break;
			case FECCRIX: {
				 feccrix_reg = *((uint8_t *) buffer);
			} break;
			case FCNFG: {
				 fcnfg_reg = *((uint8_t *) buffer);
			} break;
			case FERCNFG: {
				 fercnfg_reg = *((uint8_t *) buffer);
			} break;
			case FSTAT: {
				 fstat_reg = *((uint8_t *) buffer);
			} break;
			case FERSTAT: {
				 ferstat_reg = *((uint8_t *) buffer);
			} break;
			case FPROT: {
				 fprot_reg = *((uint8_t *) buffer);
			} break;
			case EPROT: {
				 eprot_reg = *((uint8_t *) buffer);
			} break;
			case FCCOBHI: {
				if (size == 2) {
					 fccob_reg[fccobix_reg] = BigEndian2Host(*((uint16_t *) buffer));
				} else {
					fccob_reg[fccobix_reg] = (fccob_reg[fccobix_reg] & 0x00FF) | ((uint16_t) *((uint8_t *) buffer) << 8);
				}
			} break;
			case FCCOBLO: {
				fccob_reg[fccobix_reg] = (fccob_reg[fccobix_reg] & 0xFF00) | *((uint8_t *) buffer);
			} break;
			case ETAGHI: {
				if (size == 2) {
					etag_reg = BigEndian2Host(*((uint16_t *) buffer));
				} else {
					etag_reg = (etag_reg & 0x00FF) | ((uint16_t) *((uint8_t *) buffer) << 8);
				}
			} break;
			case ETAGLO: {
				etag_reg = (etag_reg & 0xFF00) | *((uint8_t *) buffer);
			} break;
			case FECCRHI: {
				if (size == 2) {
					feccr_reg[feccrix_reg] = BigEndian2Host(*((uint16_t *) buffer));
				} else {
					feccr_reg[feccrix_reg] = (feccr_reg[feccrix_reg] & 0x00FF) | ((uint16_t) *((uint8_t *) buffer) << 8);
				}
			} break;
			case FECCRLO: {
				feccr_reg[feccrix_reg] = (feccr_reg[feccrix_reg] & 0xFF00) | *((uint8_t *) buffer);
			} break;
			case FOPT: {
				fopt_reg = *((uint8_t *) buffer);
			} break;
			case FRSV0: {
			} break;
			case FRSV1: {
			} break;
			case FRSV2: {
			} break;

			default: return (false);
		}

		return (true);
	}

	return (inherited::WriteMemory(addr, buffer, size));
}

//=====================================================================
//=             FLASH Registers Interface interface methods           =
//=====================================================================

/**
 * Gets a register interface to the register specified by name.
 *
 * @param name The name of the requested register.
 * @return A pointer to the RegisterInterface corresponding to name.
 */
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
Register * S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::GetRegister(const char *name) {

	return registers_registry.GetRegister(name);
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner) {
	
	registers_registry.ScanRegisters(scanner);
}

//=====================================================================
//=             registers setters and getters                         =
//=====================================================================
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::read(unsigned int offset, const void *buffer, unsigned int data_length)
{
	switch (offset) {
		case FCLKDIV: {
			*((uint8_t *) buffer) = fclkdiv_reg;
		} break;
		case FSEC: {
			*((uint8_t *) buffer) = fsec_reg;
		} break;
		case FCCOBIX: {
			*((uint8_t *) buffer) = fccobix_reg & 0x07;
		} break;
		case FECCRIX: {
			*((uint8_t *) buffer) = feccrix_reg & 0x07;
		} break;
		case FCNFG: {
			*((uint8_t *) buffer) = fcnfg_reg & 0x93;
		} break;
		case FERCNFG: {
			*((uint8_t *) buffer) = fercnfg_reg & 0xDF;
		} break;
		case FSTAT: {
			*((uint8_t *) buffer) = fstat_reg & 0xBF;
		} break;
		case FERSTAT: {
			*((uint8_t *) buffer) = ferstat_reg & 0xDF;
		} break;
		case FPROT: {
			*((uint8_t *) buffer) = fprot_reg;
		} break;
		case EPROT: {
			*((uint8_t *) buffer) = eprot_reg;
		} break;
		case FCCOBHI: {
			if (data_length == 2) {
				*((uint16_t *) buffer) = Host2BigEndian(fccob_reg[fccobix_reg]);
			} else {
				*((uint8_t *) buffer) = (uint8_t) (fccob_reg[fccobix_reg] >> 8);
			}
		} break;
		case FCCOBLO: {
			*((uint8_t *) buffer) = (uint8_t) (fccob_reg[fccobix_reg] & 0x00FF);
		} break;
		case ETAGHI: {
			if (data_length == 2) {
				*((uint16_t *) buffer) = Host2BigEndian(etag_reg);
			} else {
				*((uint8_t *) buffer) = (uint8_t) (etag_reg >> 8);
			}
		} break;
		case ETAGLO: {
			*((uint8_t *) buffer) = (uint8_t) (etag_reg & 0x00FF);
		} break;
		case FECCRHI: {
			if (data_length == 2) {
				*((uint16_t *) buffer) = Host2BigEndian(feccr_reg[feccrix_reg]);
			} else {
				*((uint8_t *) buffer) = (uint8_t) (feccr_reg[feccrix_reg] >> 8);
			}
		} break;
		case FECCRLO: {
			*((uint8_t *) buffer) = (uint8_t) (feccr_reg[feccrix_reg] & 0x00FF);
		} break;
		case FOPT: {
			*((uint8_t *) buffer) = fopt_reg;
		} break;
		case FRSV0: {
			*((uint8_t *) buffer) = 0;
		} break;
		case FRSV1: {
			*((uint8_t *) buffer) = 0;
		} break;
		case FRSV2: {
			*((uint8_t *) buffer) = 0;
		} break;

		default: {

			if ((offset >= FCCOB000) && (offset <= FCCOB101)) {
				*((uint16_t *) buffer) = Host2BigEndian(feccr_reg[offset - FCCOB000]);
			} else if ((offset >= FECCR000) && (offset <= FECCR111)) {
				*((uint16_t *) buffer) = Host2BigEndian(feccr_reg[offset - FECCR000]);
			} else {
				return (false);
			}
			break;
		}
	}

	return (true);
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::write(unsigned int offset, const void *buffer, unsigned int data_length)
{

	switch (offset) {
		case FCLKDIV: {
			uint8_t fdivld_mask = 0x80;
			uint8_t value = *((uint8_t *) buffer);
			if ((fclkdiv_reg & fdivld_mask) == 0) {
				// if FCLKDIV register hasn't written yet then accept write and set FDIVLD bit
				fclkdiv_reg = value | fdivld_mask;

				// compute FCLK
				setFLASHClock();
			}

		} break;
		case FSEC: {
			/**
			 * The FSEC register is loaded with the contents of the Flash security byte in the
			 * flash configuration field at global address 0x7F_FF0F located in P-Flash memory
			 * If a double bit fault is detected while reading the P-Flash phrase containing
			 * the Flash security byte during the reset sequence, all bits in the FSEC register will be
			 * set to leave the Flash module in a secured state with backdoor key access disabled
			 */
		} break;
		case FCCOBIX: {
			fccobix_reg = *((uint8_t *) buffer) & 0x07;
		} break;
		case FECCRIX: {
			feccrix_reg = *((uint8_t *) buffer) & 0x07;
		} break;
		case FCNFG: {
			fcnfg_reg = *((uint8_t *) buffer) & 0x93;
		} break;
		case FERCNFG: {
			fercnfg_reg = *((uint8_t *) buffer) & 0xDF;
		} break;
		case FSTAT: {
			uint8_t val = (*((uint8_t *) buffer) & 0xB0) | (fstat_reg & 0x0F);
			// check ACCERR
			if ((val & 0x20) != 0) {
				val = val & 0xDF;
			}
			// check PVIOL
			if ((val & 0x10) != 0) {
				val = val & 0xEF;
			}

			if ((val & 0x80) != 0) {
				if (isACCERR() || isFPVIOL()) {
					inherited::logger << DebugWarning << " : " << sc_object::name() << ":: can't launch new command. FPVIOL/ACCERR flag is set." << std::endl << EndDebugWarning;
				} else {
					if ((fclkdiv_reg & 0x80) == 0) {
						/**
						 *  If the ECLKDIV register has not been written to,
						 *  the new command is not launched
						 *  and the ACCERR flag is set.
						 */
						// set the ACCERR flag in the FSTAT register
						val = val | 0x10;

					} else {
//						if ((fstat_reg & 0x80) == 0) {
//							inherited::logger << DebugWarning << " : " << sc_object::name() << ":: can't launch new command. The last command has not finished yet." << std::endl << EndDebugWarning;
//						} else {
//							val = val & 0x7F;
//							command_launch_event.notify();
//						}

						val = val & 0x7F;
						command_launch_event.notify();

					}
				}
			}
			fstat_reg = val;
		} break;
		case FERSTAT: {
			uint8_t val = *((uint8_t *) buffer) & 0xDF;
			ferstat_reg = ferstat_reg & (~val);
		} break;
		case FPROT: {

			uint8_t value = *((uint8_t *) buffer) & 0xBF;

			bool oldOPEN = ((fprot_reg & 0x80) != 0);
			bool newOPEN = ((value & 0x80) != 0);
			bool oldHDIS = ((fprot_reg & 0x20) != 0);
			bool newHDIS = ((value & 0x20) != 0);
			bool oldLDIS = ((fprot_reg & 0x04) != 0);
			bool newLDIS = ((fprot_reg & 0x04) != 0);

			fprot_reg = ((!newOPEN || oldOPEN ) && (!newHDIS || oldHDIS) && (!newLDIS || oldLDIS))? value : fprot_reg;

			fprot_high_low_addr = pflash_protectable_high_address - (pow(2, (((fprot_reg & 0x18) >> 3) + 1)) * 1024) + 1;
			fprot_low_high_addr = pflash_protectable_low_address - (pow(2, ((fprot_reg & 0x02))) * 1024);

		} break;
		case EPROT: {

			uint8_t value = *((uint8_t *) buffer) & 0x8F;

			bool oldOPEN = ((eprot_reg & 0x80) != 0);
			bool newOPEN = ((value & 0x80) != 0);
			bool oldDIS = ((eprot_reg & 0x08) != 0);
			bool newDIS = ((value & 0x08) != 0);

			eprot_reg = ((!newOPEN || oldOPEN ) && (!newDIS || oldDIS))? value : eprot_reg;

			eprot_low_addr = eee_protectable_high_address - (((eprot_reg & 0x07) + 1) * 64) + 1;

		} break;
		case FCCOBHI: {
//			if ((fstat_reg & 0x80) == 0) {
//				inherited::logger << DebugWarning << " : " << sc_object::name() << ":: try launching new command while the last command has not finished yet." << std::endl << EndDebugWarning;
//			} else {
//				if (data_length == 2) {
//					 fccob_reg[fccobix_reg] = BigEndian2Host(*((uint16_t *) buffer));
//				} else {
//					fccob_reg[fccobix_reg] = (fccob_reg[fccobix_reg] & 0x00FF) | ((uint16_t) *((uint8_t *) buffer) << 8);
//				}
//			}

			if (data_length == 2) {
				 fccob_reg[fccobix_reg] = BigEndian2Host(*((uint16_t *) buffer));
			} else {
				fccob_reg[fccobix_reg] = (fccob_reg[fccobix_reg] & 0x00FF) | ((uint16_t) *((uint8_t *) buffer) << 8);
			}

		} break;
		case FCCOBLO: {
//			if ((fstat_reg & 0x80) == 0) {
//				inherited::logger << DebugWarning << " : " << sc_object::name() << ":: can't launch new command. The last command has not finished yet." << std::endl << EndDebugWarning;
//			} else {
//				fccob_reg[fccobix_reg] = (fccob_reg[fccobix_reg] & 0xFF00) | *((uint8_t *) buffer);
//			}

			fccob_reg[fccobix_reg] = (fccob_reg[fccobix_reg] & 0xFF00) | *((uint8_t *) buffer);

		} break;
		case ETAGHI: {
			// Not writable
		} break;
		case ETAGLO: {
			// Not writable
		} break;
		case FECCRHI: {
			// Not writable
		} break;
		case FECCRLO: {
			// Not writable
		} break;
		case FOPT: {
			// Not writable
		} break;
		case FRSV0: {
		} break;
		case FRSV1: {
		} break;
		case FRSV2: {
		} break;

		default: {

			if ((offset >= FCCOB000) && (offset <= FCCOB101)) {
				fccob_reg[offset - FCCOB000] = BigEndian2Host(*((uint16_t *) buffer));
			} else	if ((offset >= FECCR000) && (offset <= FECCR111)) {
				feccr_reg[offset - FECCR000] = BigEndian2Host(*((uint16_t *) buffer));
			} else{
				return (false);
			}
			break;
		}
	}

	return (true);
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XFTMX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
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

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XFTMX_TCC_ */
