/*
 *  Copyright (c) 2012,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_XGATE_HH_
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_XGATE_HH_

#include <map>
#include <iostream>
#include <fstream>
#include <string>

#include <inttypes.h>
#include <stdlib.h>

#include "unisim/kernel/kernel.hh"

#include "unisim/service/interfaces/trap_reporting.hh"
#include "unisim/service/interfaces/debug_yielding.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/symbol_table_lookup.hh"

#include "unisim/kernel/logger/logger.hh"

#include "unisim/util/debug/simple_register.hh"
#include "unisim/service/interfaces/register.hh"
#include "unisim/util/arithmetic/arithmetic.hh"
#include "unisim/util/endian/endian.hh"

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>
#include <unisim/component/cxx/processor/hcs12x/mmc.hh>
#include <unisim/component/cxx/processor/hcs12x/exception.hh>

#include <unisim/util/debug/simple_register_registry.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace s12xgate {

using std::string;
using std::stringstream;
using std::map;
using std::ostream;
using std::vector;

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::ParameterArray;
using unisim::kernel::variable::Statistic;
using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::variable::CallBackObject;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::EndDebug;

using unisim::service::interfaces::TrapReporting;
using unisim::service::interfaces::DebugYielding;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::MemoryAccessReportingType;

using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::SymbolTableLookup;

using unisim::util::debug::SimpleRegister;
using unisim::service::interfaces::Register;
using unisim::util::debug::Symbol;
using unisim::util::endian::endian_type;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::Host2BigEndian;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::LittleEndian2Host;

using unisim::util::arithmetic::UnsignedAdd8;
using unisim::util::arithmetic::UnsignedAdd16;
using unisim::util::arithmetic::SignedSub8;
using unisim::util::arithmetic::SignedSub16;

using unisim::component::cxx::processor::hcs12x::ADDRESS;
using unisim::component::cxx::processor::hcs12x::address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::MMC;
using unisim::component::cxx::processor::hcs12x::MMC_DATA;
using unisim::component::cxx::processor::hcs12x::CONFIG;
using unisim::component::cxx::processor::hcs12x::Exception;
using unisim::component::cxx::processor::hcs12x::AsynchronousException;
using unisim::component::cxx::processor::hcs12x::TSemaphore;
using unisim::component::cxx::processor::hcs12x::TOWNER;

class XGCCR_t
{
public:
	static const uint8_t SETC	=0x01;
	static const uint8_t CLRC=0xFE;

	static const uint8_t SETV=0x02;
	static const uint8_t CLRV=0xFD;

	static const uint8_t SETZ=0x04;
	static const uint8_t CLRZ=0xFB;

	static const uint8_t SETN=0x08;
	static const uint8_t CLRN=0xF7;

	XGCCR_t(uint8_t* _ccrReg) : ccrReg(_ccrReg) { };
	~XGCCR_t() { };

	inline uint8_t getC();
	inline void 	setC();
	inline void 	clrC();

	inline uint8_t getV();
	inline void 	setV();
	inline void 	clrV();

	inline uint8_t getZ();
	inline void 	setZ();
	inline void 	clrZ();

	inline uint8_t getN();
	inline void 	setN();
	inline void 	clrN();

	inline uint8_t getCCR();
	inline void setCCR(uint8_t val);

	string toString() {
		stringstream strm;

		strm << "---- " << ((getN())? "N":"-") << ((getZ())? "Z":"-") << ((getV())? "V":"-") << ((getC())? "C":"-");

		return (strm.str());
	}
	void reset() { *ccrReg = 0x00; }

private:

	uint8_t* ccrReg; // ---- NZVC

}; // end class XGCCR_t


inline uint8_t XGCCR_t::getC() { return (*ccrReg & SETC);};
inline void 	XGCCR_t::setC() { *ccrReg |= SETC;};
inline void 	XGCCR_t::clrC() { *ccrReg &= CLRC;};

inline uint8_t XGCCR_t::getV() { return ((*ccrReg & SETV) >> 1);};
inline void 	XGCCR_t::setV() { *ccrReg |= SETV;};
inline void 	XGCCR_t::clrV() { *ccrReg &= CLRV;};

inline uint8_t XGCCR_t::getZ() { return ((*ccrReg & SETZ) >> 2);};
inline void 	XGCCR_t::setZ() { *ccrReg |= SETZ;};
inline void 	XGCCR_t::clrZ() { *ccrReg &= CLRZ;};

inline uint8_t XGCCR_t::getN() { return ((*ccrReg & SETN) >> 3);};
inline void 	XGCCR_t::setN() { *ccrReg |= SETN;};
inline void 	XGCCR_t::clrN() { *ccrReg &= CLRN;};

inline uint8_t XGCCR_t::getCCR() {
	return (*ccrReg);
};

inline void XGCCR_t::setCCR(uint8_t val) {

	*ccrReg = val;
};


// *****************************************************************************************


class XGATE :
	public CallBackObject,
	public Client<DebugYielding>,
	public Service<Registers>,
	public Service<Memory<physical_address_t> >,
	public Service<MemoryAccessReportingControl>,
	public Client<Memory<physical_address_t> >,
	public Client<MemoryAccessReporting<physical_address_t> >,
	public Client<SymbolTableLookup<physical_address_t> >,
	public Client<TrapReporting >

{
public:
	static const unsigned int MAX_INS_SIZE = 2;


	//=========================================================
	//=                XGATE MODES                            =
	//=========================================================
	enum MODES {NORMAL, FREEZE, DEBUG}; // Freeze and Debug modes arn't emulated

	//=========================================================
	//=                XGATE STATES                           =
	//=========================================================
	enum STATES {RUNNING, IDLE, STOP};

	//=========================================================
	//=                REGISTERS OFFSETS                      =
	//=========================================================

	static const unsigned int XGMCTL = 0x00; //2-bytes
	static const unsigned int XGCHID = 0x02; // 1-byte
	static const unsigned int XGCHPL = 0x03; // 1-byte For XGATE.V2 this byte is reserved. For XGATE.V3 it hold the current thread priority.
	static const unsigned int RESERVED2 = 0x04; // 1-byte
	static const unsigned int XGISPSEL = 0x05; // 1-byte For XGATE.V2 this byte is reserved. For XGATE.V3 it is mapped to selection register.
	static const unsigned int XGVBR = 0x06; // 2-bytes For XGATE.V3 this 2-bytes depending on the content of XGISPSEL is mapped to {XGVBR, XGISP74, XGISP31}
	static const unsigned int XGIF_7F_70 = 0x08; // 2-bytes
	static const unsigned int XGIF_6F_60 = 0x0A; // 2-bytes
	static const unsigned int XGIF_5F_50 = 0x0C; // 2-bytes
	static const unsigned int XGIF_4F_40 = 0x0E; // 2-bytes
	static const unsigned int XGIF_3F_30 = 0x10; // 2-bytes
	static const unsigned int XGIF_2F_20 = 0x12; // 2-bytes
	static const unsigned int XGIF_1F_10 = 0x14; // 2-bytes
	static const unsigned int XGIF_0F_00 = 0x16; // 2-bytes
	static const unsigned int XGSWT = 0x18; // 2-bytes
	static const unsigned int XGSEM = 0x1A; // 2-bytes
	static const unsigned int RESERVED4 = 0x1C; // 1-byte
	static const unsigned int XGCCR = 0x1D; // 1-byte
	static const unsigned int XGPC = 0x1E; // 2-bytes
	static const unsigned int RESERVED5 = 0x20; // 1-byte
	static const unsigned int RESERVED6 = 0x21; // 1-byte
	static const unsigned int XGR1 = 0x22; // 2-bytes
	static const unsigned int XGR2 = 0x24; // 2-bytes
	static const unsigned int XGR3 = 0x26; // 2-bytes
	static const unsigned int XGR4 = 0x28; // 2-bytes
	static const unsigned int XGR5 = 0x2A; // 2-bytes
	static const unsigned int XGR6 = 0x2C; // 2-bytes
	static const unsigned int XGR7 = 0x2E; // 2-bytes
	static const unsigned int RESERVED7 = 0x30; // 5*2-bytes internal registers not visible outside XGATE

	static const unsigned int MEMORY_MAP_SIZE = 64;

	//=====================================================================
	//=                  public service imports/exports                   =
	//=====================================================================

	ServiceExport<Registers> registers_export;
	ServiceExport<Memory<physical_address_t> > memory_export;
	ServiceExport<MemoryAccessReportingControl> memory_access_reporting_control_export;

	ServiceImport<DebugYielding> debug_yielding_import;
	ServiceImport<MemoryAccessReporting<physical_address_t> > memory_access_reporting_import;
	ServiceImport<Memory<physical_address_t> > memory_import;
	ServiceImport<TrapReporting > trap_reporting_import;
	ServiceImport<SymbolTableLookup<physical_address_t> > symbol_table_lookup_import;

	XGATE(const char *name, Object *parent);
	~XGATE();

	virtual void Reset();
	
	unsigned int step();	// Return the number of cpu cycles consumed by the operation
	virtual void Stop(int ret);
	virtual void Sync();

	virtual void enbale_xgate() = 0;
	virtual void disable_xgate() = 0;

	virtual void assertInterrupt(unsigned int offset, bool isXGATE_flag = false) = 0;

	void assert_software_error_interrupt() {
		assertInterrupt(interrupt_software_error, false);
	}

	void assertChannelInterrupt(unsigned int offset) {
		unsigned int reg_index = offset / 8;
		unsigned int flag_index = offset % 8;
		xgif_register[reg_index] = xgif_register[reg_index] | (1 << flag_index);

		assertInterrupt(offset, true);
	}

	virtual void riseErrorCondition();

	virtual void triggerChannelThread() = 0;
	void terminateCurrentThread();

	void fakeXGATEActivity() {
		/**
		 * TODO: take into account XGMCTL::XGFACT bit
		 * Fake XGATE Activity—This bit forces the XGATE to flag activity to the MCU even when it is idle.
		 * When it is set the MCU will never enter system stop mode which assures that peripheral modules
		 * will be clocked during XGATE idle periods
		 * Read:
		 *   0 XGATE will only flag activity if it is not idle or in debug mode.
		 *   1 XGATE will always signal activity to the MCU.
		 * Write:
		 *   0 Only flag activity if not idle or in debug mode.
		 *   1 Always signal XGATE activity.
		 */

		/**
		 * To implement this functionality, I have to link the CRG and The XGATE or to have a variable at simulator (MCU) level
		 */
	}

	virtual void busWrite(MMC_DATA *buffer) = 0;
	virtual void busRead(MMC_DATA *buffer) = 0;

	inline void reportTrap(const char *c_str);

	//==========================================
	//=     ISA - MEMORY ACCESS ROUTINES       =
	//==========================================

	inline uint8_t memRead8(address_t logicalAddress);
	inline void memWrite8(address_t logicalAddress,uint8_t val);

	inline uint16_t memRead16(address_t logicalAddress);
	inline void memWrite16(address_t logicalAddress,uint16_t val);

	inline void monitorStore(address_t logicalAddress, uint32_t size);
	inline void monitorLoad(address_t logicalAddress, uint32_t size);

	//=====================================================================
	//=                  Memory Access Reportings methods                 =
	//=====================================================================

	virtual void RequiresMemoryAccessReporting( MemoryAccessReportingType type, bool report );

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================

	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	virtual void OnDisconnect();
	virtual void ResetMemory();


	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================

	virtual bool ReadMemory(physical_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(physical_address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=             Registers Interface methods                           =
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
	//=             Internal Registers access methods                     =
	//=====================================================================


	//=====================================================================
	//=             memory map access methods                             =
	//=====================================================================
	virtual bool read(unsigned int offset, const void *buffer, unsigned int data_length);
	virtual bool write(unsigned int offset, const void *buffer, unsigned int data_length);

	//=====================================================================
	//=                   Debugging methods                               =
	//=====================================================================

	string getObjectFriendlyName(physical_address_t addr);
	string getFunctionFriendlyName(physical_address_t addr);
	inline uint64_t getInstructionCounter() const { return (instruction_counter); }
	inline bool isVerboseException() const { return (debug_enabled && CONFIG::DEBUG_EXCEPTION_ENABLE && (verbose_all || verbose_exception)); }

protected:

	// the kernel logger
	unisim::kernel::logger::Logger *logger;

	//=====================================================================
	//=              XGATE Cycle Time/Voltage/Bus Cycle Time                =
	//=====================================================================

	uint64_t cpu_cycle_time; //!< XGATE cycle time in ps
	uint64_t voltage;        //!< XGATE voltage in mV
	uint64_t bus_cycle_time; //!< Front side bus cycle time in ps
	uint64_t cpu_cycle;      //!< Number of cpu cycles
	uint64_t bus_cycle;      //!< Number of front side bus cycles

	//utils attributes
	bool verbose_all;
	Parameter<bool> param_verbose_all;
	bool verbose_setup;
	Parameter<bool> param_verbose_setup;
	bool verbose_step;
	Parameter<bool> param_verbose_step;
	bool verbose_dump_regs_start;
	Parameter<bool> param_verbose_dump_regs_start;
	bool verbose_dump_regs_end;
	Parameter<bool> param_verbose_dump_regs_end;
	bool verbose_exception;
	Parameter<bool> param_verbose_exception;

	bool requires_memory_access_reporting;      //< indicates if the memory accesses require to be reported
	bool requires_fetch_instruction_reporting;  //< indicates if the fetched instructions require to be reported
	bool requires_commit_instruction_reporting; //< indicates if the committed instructions require to be reported
  
	bool enable_trace;
	Parameter<bool> param_enable_trace;

	std::ofstream trace;
	bool enable_file_trace;
	Parameter<bool> param_enable_file_trace;

	bool	debug_enabled;
	Parameter<bool>	param_debug_enabled;

	uint64_t periodic_trap;
	Parameter<uint64_t> param_periodic_trap;

	string version;
	Parameter<string> param_version;

	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	static const unsigned int XGATE_SIZE = 8;

	unsigned int sofwtare_channel_id[XGATE_SIZE];
	ParameterArray<unsigned int> param_software_channel_id;

	bool xgate_enabled;
	bool stop_current_thread;

	STATES state;
	MODES mode;
	bool currentThreadTerminated;

	// verbose methods
	inline bool VerboseSetup() GCC_INLINE;
	inline bool VerboseStep() GCC_INLINE;
	inline void VerboseDumpRegs() GCC_INLINE;
	inline void VerboseDumpRegsStart() GCC_INLINE;
	inline void VerboseDumpRegsEnd() GCC_INLINE;


	/**
	 * XGATE V3 and higher support Interruptible thread execution and have Two register banks to support fast context switching between threads
	 *
	 * XGATE Register Bank
	 * 	A register bank consists of registers R1-R7, CCR and the PC.
	 * 	Each interrupt level is associated with	one register bank.
	 */

	class TRegisterBank {
	public:
		TRegisterBank() {
			xgchpl_register = 0x0000;
			xgchid_register = 0x0000;
			xgccr_register = 0x0000;
			xgpc_register = 0x0000;
			xgr_register[1] = 0x0000;
			xgr_register[2] = 0x0000;
			xgr_register[3] = 0x0000;
			xgr_register[4] = 0x0000;
			xgr_register[5] = 0x0000;
			xgr_register[6] = 0x0000;
			xgr_register[7] = 0x0000;

			ccr = new XGCCR_t(&xgccr_register);
		}

		~TRegisterBank() { if (ccr) { delete ccr; ccr = NULL; }	}

		inline void setXGCHPL(uint8_t val) { xgchpl_register = val; }
		inline uint8_t getXGCHPL() { return (xgchpl_register); }
		inline uint8_t* getXGCHPLPtr() { return (&xgchpl_register); }
		inline void setXGCHID(uint8_t val) { xgchid_register = val; }
		inline uint8_t getXGCHID() { return (xgchid_register); }
		inline uint8_t* getXGCHIDPtr() { return (&xgchid_register); }
		inline void setXGPC(address_t val) { xgpc_register = val; }
		inline address_t getXGPC() { return (xgpc_register); }
		inline address_t* getXGPCPtr() { return (&xgpc_register); }
		inline void setXGRx(unsigned int index, uint16_t val) { xgr_register[index] = ((index == 0)? 0:val); } // R0 is tied to the value 0x0000
		inline uint16_t getXGRx(unsigned int index) { return (((index == 0)? 0: xgr_register[index])); }
		inline uint16_t* getXGRxPtr(unsigned int index) { return (&xgr_register[index]); }
		inline void setXGCCR(uint8_t val) { xgccr_register = val & 0x0F; }
		inline uint8_t getXGCCR() { return (xgccr_register); }
		inline uint8_t* getXGCCRPtr() { return (&xgccr_register); }

		XGCCR_t* getCCR() { return (ccr); }

		void Reset() {
			xgchpl_register = 0x0000;
			xgchid_register = 0x0000;
			xgccr_register = 0x0000;
			xgpc_register = 0x0000;
			xgr_register[1] = 0x0000;
			xgr_register[2] = 0x0000;
			xgr_register[3] = 0x0000;
			xgr_register[4] = 0x0000;
			xgr_register[5] = 0x0000;
			xgr_register[6] = 0x0000;
			xgr_register[7] = 0x0000;
		}

	private:
		uint8_t xgchpl_register, xgchid_register;
		uint16_t xgr_register[8];
		uint8_t xgccr_register;
		uint16_t xgpc_register;

		XGCCR_t* ccr;

	} *currentRegisterBank;

	bool isPendingThread;
	TRegisterBank *pendingThreadRegisterBank;

private:

	// Registers map
	unisim::util::debug::SimpleRegisterRegistry registers_registry;
	std::vector<unisim::kernel::VariableBase*> extended_registers_registry;

	unsigned int				interrupt_software_error;
	Parameter<unsigned int>	param_interrupt_software_error;

	uint64_t	max_inst;
	Parameter<uint64_t>	   param_max_inst;

	/** the instruction counter */
	uint64_t instruction_counter;
	Statistic<uint64_t> stat_instruction_counter;
	uint64_t cycles_counter;
	Statistic<uint64_t> stat_cycles_counter;
	uint64_t data_load_counter;
	Statistic<uint64_t> stat_load_counter;
	uint64_t data_store_counter;
	Statistic<uint64_t> stat_store_counter;

	//===============================================================
	//=         XGATE Memory Map                                    =
	//===============================================================


	uint16_t xgmctl_register, xgif_register[8], xgswt_register, xgsem_register;

	// For XGATE.V3 XGVBR register is shared between {XGVBR, XGISP74, XGISP31} depending on the content of XGISPSEL register
	uint16_t xgvbrPtr_register[4]; // 	xgvbrPtr[0]<->xgvbr xgvbrPtr[1]<->xgisp74 xgvbrPtr[2]<->xgisp31_register;
	uint8_t xgispsel_register;

	TRegisterBank registerBank[2];

	address_t lastPC;

	TSemaphore semphore[8];

	void fetchInstruction(address_t addr, uint8_t* ins, unsigned int nByte);

public:

	XGCCR_t* getCCR() { return (currentRegisterBank->getCCR()); }


	void setState(STATES st) { state = st; }

	//=========================================================
	//=                REGISTERS ACCESSORS                    =
	//=========================================================

	bool lockSemaphore(TOWNER::OWNER owner, unsigned int index);
	bool unlockSemaphore(TOWNER::OWNER owner, unsigned int index);

	bool isINTRequestEnabled() { return ((xgmctl_register & 0x0080) != 0); }

	inline void setXGPC(address_t val) { currentRegisterBank->setXGPC(val); }
	inline address_t getXGPC() { return (currentRegisterBank->getXGPC()); }
	inline address_t getLastXGPC() {return (lastPC); }

	inline uint8_t getXGCHPL() { return ((version.compare("V2") == 0)? 0: currentRegisterBank->getXGCHPL()); }
	inline void setXGCHPL(uint8_t val) { currentRegisterBank->setXGCHPL(((version.compare("V2") == 0)? 0: val)); }
	inline uint8_t getXGISPSEL() { return ((version.compare("V2") == 0)? 0: xgispsel_register); }
	inline void setXGISPSEL(uint8_t val) { xgispsel_register = ((version.compare("V2") == 0)? 0: val); }
	inline uint16_t getXGVBR() { return (xgvbrPtr_register[((version.compare("V2") == 0)? 0: (xgispsel_register & 0x03))]); }
	inline void setXGVBR(uint16_t val) { xgvbrPtr_register[((version.compare("V2") == 0)? 0: (xgispsel_register & 0x03))] = val; }

	inline uint16_t getXGISP31() { return (xgvbrPtr_register[1]); }
	inline void setXGISP31(uint16_t val) { xgvbrPtr_register[1] = val; }
	inline uint16_t getXGISP74() { return (xgvbrPtr_register[2]); }
	inline void setXGISP74(uint16_t val) { xgvbrPtr_register[2] = val; }

	inline TRegisterBank* getRegisterBank(unsigned int priority) {

		if ((version.compare("V2") != 0) & (priority > 3)) {
			return (&registerBank[1]);
		} else {
			return (&registerBank[0]);
		}
	}

	void preloadXGR7(unsigned int priority) {

		/**
		 * XGATE V3 and higher
		 * R7 is either preloaded with the content of XGISP74 if the interrupt priority of the current channel is in the range 7 to 4,
		 * or it is with preloaded the content of XGISP31 if the interrupt priority of the current channel is in the range 3 to 1.
		 */

		if (version.compare("V2") != 0) {
			if (priority > 3) {
				setXGRx(7, getXGISP74());
			} else {
				setXGRx(7, getXGISP31());
			}
		} else {
			// R7 is unchanged for XGATE V2
		}

	}

	inline void setXGSWT(uint16_t val) { xgswt_register = val; }
	inline uint16_t getXGSWT() { return (xgswt_register); }
	inline void setXGRx(unsigned int index, uint16_t val) { currentRegisterBank->setXGRx(index, val); } // R0 is tied to the value 0x0000
	inline uint16_t getXGRx(unsigned int index) { return (currentRegisterBank->getXGRx(index)); }
	inline void setXGCHID(uint8_t val) { currentRegisterBank->setXGCHID(val); }
	inline uint8_t getXGCHID() { return (currentRegisterBank->getXGCHID()); }
	inline void setXGCCR(uint8_t val) { currentRegisterBank->setXGCCR(val); }
	inline uint8_t getXGCCR() { return (currentRegisterBank->getXGCCR()); }

	inline static std::string getXGRxName(unsigned int index) {
		stringstream name;

		name << "R" << (unsigned int) index;

		return (name.str());
	}

	inline bool isInterruptEnabled() { return (xgmctl_register & 0x0001); }
};

inline uint8_t XGATE::memRead8(address_t logicalAddress) {

	uint8_t data;
	MMC_DATA mmc_data;

	mmc_data.logicalAddress = logicalAddress;
	mmc_data.type = ADDRESS::EXTENDED;
	mmc_data.isGlobal = false;
	mmc_data.buffer = &data;
	mmc_data.data_size = 1;

	busRead(&mmc_data);

	monitorLoad(logicalAddress, sizeof(data));

	return (data);
}

inline uint16_t XGATE::memRead16(address_t logicalAddress) {

	uint16_t data;
	MMC_DATA mmc_data;

	mmc_data.logicalAddress = logicalAddress;
	mmc_data.type = ADDRESS::EXTENDED;
	mmc_data.isGlobal = false;
	mmc_data.buffer = &data;
	mmc_data.data_size = 2;

	busRead(&mmc_data);

	data = BigEndian2Host(data);

	monitorLoad(logicalAddress, sizeof(data));

	return (data);
}

inline void XGATE::memWrite8(address_t logicalAddress,uint8_t data) {

	MMC_DATA mmc_data;

	mmc_data.logicalAddress = logicalAddress;
	mmc_data.type = ADDRESS::EXTENDED;
	mmc_data.isGlobal = false;
	mmc_data.buffer = &data;
	mmc_data.data_size = 1;

	busWrite(&mmc_data);

	monitorStore(logicalAddress, sizeof(data));

}

inline void XGATE::memWrite16(address_t logicalAddress,uint16_t data) {

	MMC_DATA mmc_data;

	data = Host2BigEndian(data);

	mmc_data.logicalAddress = logicalAddress;
	mmc_data.type = ADDRESS::EXTENDED;
	mmc_data.isGlobal = false;
	mmc_data.buffer = &data;
	mmc_data.data_size = 2;

	busWrite(&mmc_data);

	monitorStore(logicalAddress, sizeof(data));

}

// ==============================================
// =          MEMORY ACCESS ROUTINES            =
// ==============================================

inline void XGATE::monitorLoad(address_t logicalAddress, uint32_t size)
{
	physical_address_t pea = MMC::getInstance()->getXGATEPhysicalAddress(logicalAddress);

	data_load_counter++;

	// Memory access reporting
	if(memory_access_reporting_import)
	{
		memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, pea, size);
	}
}

inline void XGATE::monitorStore(address_t logicalAddress, uint32_t size)
{
	physical_address_t pea = MMC::getInstance()->getXGATEPhysicalAddress(logicalAddress);

	data_store_counter++;

	// Memory access reporting
	if(memory_access_reporting_import)
	{
		memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, pea, size);
	}
}

inline void XGATE::reportTrap(const char *c_str) {
	if (trap_reporting_import) {
		trap_reporting_import->ReportTrap(*this, c_str);
	}

}

} // end of namespace s12xgate
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_XGATE_HH_ */
