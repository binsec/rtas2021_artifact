/*
 * S12XFTMX.hh
 *
 *  Created on: 28 January 2013
 *      Author: rnouacer
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XFTMX_HH_
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XFTMX_HH_

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
using unisim::kernel::variable::ParameterArray;
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

template <unsigned int BUSWIDTH = DEFAULT_BUSWIDTH, class ADDRESS = DEFAULT_ADDRESS, unsigned int BURST_LENGTH = DEFAULT_BURST_LENGTH, uint32_t PAGE_SIZE = DEFAULT_PAGE_SIZE, bool DEBUG = DEFAULT_DEBUG>
class S12XFTMX :
	  public unisim::component::tlm2::memory::ram::Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>
	, public CallBackObject
	, public Service<Registers>
	, virtual public tlm_bw_transport_if<XINT_REQ_ProtocolTypes>

{
public:
	typedef unisim::component::tlm2::memory::ram::Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG> inherited;

	//=========================================================
	//=                REGISTERS OFFSETS                      =
	//=========================================================

	enum REGS_OFFSETS {
		FCLKDIV, FSEC, FCCOBIX, FECCRIX, FCNFG, FERCNFG, FSTAT, FERSTAT, FPROT, EPROT,
		FCCOBHI, FCCOBLO, ETAGHI, ETAGLO, FECCRHI, FECCRLO, FOPT, FRSV0, FRSV1, FRSV2,
		FCCOB000, FCCOB001, FCCOB010, FCCOB011, FCCOB100, FCCOB101,
		FECCR000, FECCR001, FECCR010, FECCR011, FECCR100, FECCR101, FECCR110, FECCR111
	};

	static const unsigned int REGISTERS_BANK_SIZE = 20;

	static const uint8_t EraseVerifyAllBlocks = 0x01;
	static const uint8_t EraseVerifyBlock = 0x02;
	static const uint8_t EraseVerifyPFlashSection = 0x03;
	static const uint8_t ReadOnce = 0x04;
	static const uint8_t LoadDataField = 0x05;
	static const uint8_t ProgramPFlash = 0x06;
	static const uint8_t ProgramOnce = 0x07;
	static const uint8_t EraseAllBlocks = 0x08;
	static const uint8_t ErasePFlashBlock = 0x09;
	static const uint8_t ErasePFlashSector = 0x0A;
	static const uint8_t UnsecureFlash = 0x0B;
	static const uint8_t VerifyBackdoorAccessKey = 0x0C;
	static const uint8_t SetUserMarginLevel = 0x0D;
	static const uint8_t SetFieldMarginLevel = 0x0E;
	static const uint8_t FullPartitionDFlash = 0x0F;
	static const uint8_t EraseVerifyDFlashSector = 0x10;
	static const uint8_t ProgramDFlash = 0x11;
	static const uint8_t EraseDFlashSector = 0x12;
	static const uint8_t EnableEEPROMEmulation = 0x13;
	static const uint8_t DisableEEPROMEmulation = 0x14;
	static const uint8_t EEPROMEmulationQuery = 0x15;
	static const uint8_t PartitionDFlash = 0x20;

	static const uint16_t WORD_SIZE = 2;
	static const uint16_t DFLASH_SECTOR_SIZE = 256;
	static const uint16_t PHRASE_SIZE = 8;
	static const uint16_t PFLASH_SECTOR_SIZE = 1024;
	static const uint32_t PFLASH_SECTION_SIZE = 128*PFLASH_SECTOR_SIZE;
	static const uint16_t EEE_NON_VOLATILE_SPACE_SIZE = 8;
	static const uint16_t BACKDOOR_ACCESS_KEY_SIZE = 8;

	ServiceExport<Registers> registers_export;

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	tlm_utils::simple_target_socket<S12XFTMX> bus_clock_socket;

	// interface with bus
	tlm_utils::simple_target_socket<S12XFTMX> slave_socket;

	/**
	 * Constructor.
	 *
	 * @param name the name of the module
	 * @param parent the parent service
	 */
	S12XFTMX(const sc_module_name& name, Object *parent = 0);
	/**
	 * Destructor
	 */
	virtual ~S12XFTMX();

	void assertInterrupt(uint8_t interrupt_offset);

	void Process();
	void updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay);
	sc_time getBusClock() { return bus_cycle_time; }

	void setFLASHClock();

	void eraseVerifyAllBlocks_cmd();
	void eraseVerifyBlock_cmd();
	void eraseVerifyPFlashSection_cmd();
	void readOnce_cmd();
	void loadDataField_cmd();
	void programPFlash_cmd();
	void programOnce_cmd();
	void eraseAllBlocks_cmd();
	void erasePFlashBlock_cmd();
	void erasePFlashSector_cmd();
	void unsecureFlash_cmd();
	void verifyBackdoorAccessKey_cmd();
	void setUserMarginLevel_cmd();
	void setFieldMarginLevel_cmd();
	void fullPartitionDFlash_cmd();
	void eraseVerifyDFlashSection_cmd();
	void programDFlash_cmd();
	void eraseDFlashSector_cmd();
	void enableEEPROMEmulation_cmd();
	void disableEEPROMEmulation_cmd();
	void eEPROMEmulationQuery_cmd();
	void partitionDFlash_cmd();

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

	virtual bool ReadMemory(ADDRESS physical_addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(ADDRESS physical_addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=             FLASH Registers Interface interface methods               =
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

	enum INTERRUPT_SOURCES {
		flashCommandComplete_interruptSource, flashEEEEraseError_interruptSource,
		flashEEEProgramError_interruptSource, flashEEEProtectionViolation_interruptSource,
		flashEEEErrorType1Violation_interruptSource, flashEEEErrorType0Violation_interruptSource,
		eccDoublebitFaultOnFlashRead_interruptSource, eccSingleBitFaultOnFlashRead_interruptSource
	};

	void ComputeInternalTime();
	void write_to_flash(physical_address_t address, void* data_ptr, unsigned int data_length);

	tlm_quantumkeeper quantumkeeper;

	PayloadFabric<XINT_Payload> xint_payload_fabric;

	double	bus_cycle_time_int;	// The time unit is PS
	Parameter<double>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;

	double	oscillator_cycle_time_int;
	Parameter<double>	param_oscillator_cycle_time_int;
	sc_time		oscillator_cycle_time;

	sc_event command_launch_event;

	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	uint8_t flashFaultDetect_interruptOffset;
	Parameter<uint8_t> param_flashFaultDetect_interruptOffset;

	uint8_t flash_interruptOffset;
	Parameter<uint8_t> param_flash_interruptOffset;

	double erase_fail_ratio;
	Parameter<double> param_erase_fail_ratio;

	string			pflash_blocks_description_string;
	Parameter<string> param_pflash_blocks_description_string;
	struct BlockDescription {
		physical_address_t start_address;
		physical_address_t end_address;
	};
	vector<BlockDescription*> pflash_blocks_description;

	physical_address_t	pflash_start_address;
	Parameter<physical_address_t> param_pflash_start_address;
	physical_address_t	pflash_end_address;
	Parameter<physical_address_t> param_pflash_end_address;

	physical_address_t pflash_protectable_high_address;
	Parameter<physical_address_t> param_pflash_protectable_high_address;
	physical_address_t pflash_protectable_low_address;
	Parameter<physical_address_t> param_pflash_protectable_low_address;

	physical_address_t fprot_high_low_addr;
	physical_address_t fprot_low_high_addr;

	physical_address_t	blackdoor_comparison_key_address;	// 8 bytes
	Parameter<physical_address_t> param_blackdoor_comparison_key_address;
	physical_address_t	pflash_protection_byte_address;		// 1 byte
	Parameter<physical_address_t> param_pflash_protection_byte_address;
	physical_address_t	eee_protection_byte_address;		// 1 byte
	Parameter<physical_address_t> param_eee_protection_byte_address;
	physical_address_t	flash_nonvolatile_byte_address;		// 1 byte
	Parameter<physical_address_t> param_flash_nonvolatile_byte_address;
	physical_address_t	flash_security_byte_address;		// 1 byte
	Parameter<physical_address_t> param_flash_security_byte_address;

	physical_address_t	dflash_start_address;
	Parameter<physical_address_t> param_dflash_start_address;
	physical_address_t	dflash_end_address;
	Parameter<physical_address_t> param_dflash_end_address;

	physical_address_t	eee_nonvolatile_information_register_start_address;
	Parameter<physical_address_t> param_eee_nonvolatile_information_register_start_address;
	uint16_t			eee_nonvolatile_information_register_size;
	Parameter<uint16_t> param_eee_nonvolatile_information_register_size;
	physical_address_t	eee_tag_ram_start_address;
	Parameter<physical_address_t> param_eee_tag_ram_start_address;
	uint16_t			eee_tag_ram_size;
	Parameter<uint16_t> param_eee_tag_ram_size;

	physical_address_t eee_protectable_high_address;
	Parameter<physical_address_t> param_eee_protectable_high_address;

	physical_address_t eprot_low_addr;

	physical_address_t	memory_controller_scratch_ram_start_address;
	Parameter<physical_address_t> param_memory_controller_scratch_ram_start_address;
	uint16_t			memory_controller_scratch_ram_size;
	Parameter<uint16_t> param_memory_controller_scratch_ram_size;
	physical_address_t	buffer_ram_start_address;
	Parameter<physical_address_t> param_buffer_ram_start_address;
	physical_address_t	buffer_ram_end_address;
	Parameter<physical_address_t> param_buffer_ram_end_address;

	physical_address_t	dflash_partition_user_access_address;
	Parameter<physical_address_t> param_dflash_partition_user_access_address;
	physical_address_t	dflash_partition_user_access_address_duplicate;
	Parameter<physical_address_t> param_dflash_partition_user_access_address_duplicate;
	physical_address_t	buffer_ram_partition_eee_operation_address;
	Parameter<physical_address_t> param_buffer_ram_partition_eee_operation_address;
	physical_address_t	buffer_ram_partition_eee_operation_address_duplicate;
	Parameter<physical_address_t> param_buffer_ram_partition_eee_operation_address_duplicate;

	uint16_t			max_number_sectors_dflash;
	Parameter<uint16_t>	param_max_number_sectors_dflash;
	uint16_t			max_number_sectors_buffer_ram;
	Parameter<uint16_t>	param_max_number_sectors_buffer_ram;
	uint16_t			min_number_sectors_in_dflash_for_eee;
	Parameter<uint16_t>	param_min_number_sectors_in_dflash_for_eee;
	uint16_t			min_ratio_dflash_buffer_ram;
	Parameter<uint16_t>	param_min_ratio_dflash_buffer_ram;

	sc_time		fclk_time;
	long		min_fclk_time_int;
	sc_time		min_fclk_time;
	Parameter<long> param_min_fclk_time;
	long		max_fclk_time_int;
	sc_time		max_fclk_time;
	Parameter<long> param_max_fclk_time;

	vector <uint16_t*> loadDataFieldBuffer;
	bool partitionDFlashCmd_Launched;
	bool fullPartitionDFlashCmd_Launched;
	bool eepromEmulationEnabled;
	bool verifyBackdoorAccessKey_Failed;

	uint16_t sector_erased_count;
	uint8_t dead_sector_count;
	uint8_t ready_sector_count;

	// Registers map
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	std::vector<unisim::kernel::VariableBase*> extended_registers_registry;

	uint8_t  fclkdiv_reg, fsec_reg, fccobix_reg, feccrix_reg, fcnfg_reg, fercnfg_reg, fstat_reg, ferstat_reg, fprot_reg, eprot_reg, fopt_reg, frsv0_reg, frsv1_reg, frsv2_reg;
	uint16_t fccob_reg[6], etag_reg, feccr_reg[8];

	inline bool isCommandCompleteInterruptEnable() { return ((fcnfg_reg & 0x80) != 0); }
	inline bool isignoreSingleBitFault() { return ((fcnfg_reg & 0x10) != 0); }
	inline bool isForceDoubleBitFaultdetect() { return ((fcnfg_reg & 0x02) != 0); }
	inline bool isForceSingleBitFaultDetect() { return ((fcnfg_reg & 0x01) != 0); }

	inline bool isEpromEraseErrorInterruptenable() { return ((fercnfg_reg & 0x80) != 0); }
	inline bool isEpromProgramErrorInterruptEnable() { return ((fercnfg_reg & 0x40) != 0); }
	inline bool isEpromProtectionViolationEnable() { return ((fercnfg_reg & 0x10) != 0); }
	inline bool isEpromErrorType1InterruptEnable() { return ((fercnfg_reg & 0x08) != 0); }
	inline bool isEpromErrorType0InterruptEnable() { return ((fercnfg_reg & 0x04) != 0); }
	inline bool isDoubleBitFaultDetectInterruptEnable() { return ((fercnfg_reg & 0x02) != 0); }
	inline bool isSingleBitFaultDetectInterruptEnable() { return ((fercnfg_reg & 0x01) != 0); }

	inline void setCCIF() {
		fstat_reg = fstat_reg | 0x80;
		if (isCommandCompleteInterruptEnable()) assertInterrupt(flash_interruptOffset);
	}
	inline bool isCommandLaunched() { return ((fstat_reg & 0x80) == 0x00); }

	inline void validateFlashRead(physical_address_t address) {
		if (isCommandLaunched()) {
			uint16_t flashBlock_addr = (address >> 16) & 0x00FF;
			if (flashBlock_addr == (fccob_reg[0] & 0x00FF)) {
				feccr_reg[0] = flashBlock_addr;
				feccr_reg[1] = address & 0xFFFF;
				for (uint8_t i=2; i<8; i++) {
					feccr_reg[i] = 0x0000;
				}
				setSFDIF();
				setDFDIF();
			}
		}

	}

	inline bool isACCERR() { return ((fstat_reg & 0x20) != 0); }
	inline void setACCERR() { fstat_reg = fstat_reg | 0x20; }
	inline bool isFPVIOL() { return ((fstat_reg & 0x10) != 0); }
	inline void setFPVIOL() { fstat_reg = fstat_reg | 0x10; }
	inline void setEPVIOLIF() { ferstat_reg =ferstat_reg | 0x10; }

	inline bool isPFlashProtected(physical_address_t low_addr, physical_address_t high_addr) {

		bool find = false;
		find |= (!((fprot_reg & 0x80) != 0) != !((fprot_reg & 0x20) == 0)) && (fprot_high_low_addr <= high_addr) && (low_addr <= pflash_protectable_high_address);
		find |= (!((fprot_reg & 0x80) != 0) != !((fprot_reg & 0x04) == 0)) && (pflash_protectable_low_address <= high_addr) && (low_addr <= fprot_low_high_addr);

		return find;
	}

	inline bool isEEEProtected(physical_address_t low_addr, physical_address_t high_addr) {

		return (!((eprot_reg & 0x80) != 0) != !((eprot_reg & 0x08) == 0)) && (eprot_low_addr <= high_addr) && (low_addr <= eee_protectable_high_address);
	}

	inline void setSFDIF() {
		ferstat_reg = ferstat_reg | 0x01;
		if ((fercnfg_reg & 0x01) != 0) assertInterrupt(flashFaultDetect_interruptOffset);
	}
	inline void setDFDIF() {
		ferstat_reg = ferstat_reg | 0x02;
		if ((fercnfg_reg & 0x02) != 0) assertInterrupt(flashFaultDetect_interruptOffset);
	}

	inline bool isLoadDataFieldCommandSequenceActive() {
		return (loadDataFieldBuffer.size() > 0);
	}

	inline bool isPartitionDFlashCmd_Launched() {
		return (partitionDFlashCmd_Launched);
	}

	inline bool isFullPartitionDFlashCmd_Launched() {
		return (fullPartitionDFlashCmd_Launched);
	}

	inline bool isEepromEmulationEnabled() {
		return (eepromEmulationEnabled);
	}

	inline void unsecureFlash() {
		/**
		 *  FSEC::SEC bits values
		 *  00, 01, 11 => Flash secured
		 *  10 => Flash unsecured
		 */
		fsec_reg = (fsec_reg & 0xFC) | 0x02;
	}

	inline bool isFlashSecured() {
		return ((fsec_reg & 0x03) != 0x02);
	}

	inline bool isBackdoorkeySecurityEnabled() {
		return ((fsec_reg & 0x80) == 0x80);
	}
};


} // end of hcs12x
} // end of processor
} // end of tlm2
} // end of component
} // end of unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XFTMX_HH_ */
