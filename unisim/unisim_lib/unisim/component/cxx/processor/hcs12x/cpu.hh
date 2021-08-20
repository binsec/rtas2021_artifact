/*
 *  Copyright (c) 2008,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CPU_HH__

#ifdef GCC_INLINE
#undef GCC_INLINE
#endif

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define GCC_INLINE __attribute__((always_inline))
#else
#define GCC_INLINE
#endif


#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <stdlib.h>

#include <unisim/service/interfaces/loader.hh>
#include "unisim/service/interfaces/trap_reporting.hh"
#include "unisim/service/interfaces/debug_yielding.hh"
#include "unisim/service/interfaces/disassembly.hh"
#include "unisim/service/interfaces/memory_access_reporting.hh"
#include "unisim/service/interfaces/symbol_table_lookup.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/kernel/kernel.hh"

#include "unisim/service/interfaces/register.hh"
#include "unisim/util/debug/simple_register.hh"
#include <unisim/util/debug/simple_register_registry.hh>
#include "unisim/util/arithmetic/arithmetic.hh"
#include "unisim/util/endian/endian.hh"


#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/ccr.hh>
#include <unisim/component/cxx/processor/hcs12x/mmc.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>
#include <unisim/component/cxx/processor/hcs12x/exception.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

using unisim::kernel::Object;
using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::ServiceImport;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Statistic;
using unisim::kernel::variable::CallBackObject;

using unisim::service::interfaces::Loader;
using unisim::service::interfaces::TrapReporting;
using unisim::service::interfaces::DebugYielding;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::MemoryAccessReportingType;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;

using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::EndDebug;


using unisim::service::interfaces::Register;

using unisim::util::arithmetic::UnsignedAdd8;
using unisim::util::arithmetic::UnsignedAdd16;
using unisim::util::arithmetic::SignedSub8;
using unisim::util::arithmetic::SignedSub16;

using unisim::util::endian::endian_type;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::Host2BigEndian;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::LittleEndian2Host;

using std::string;
using std::stringstream;
using std::map;
using std::ostream;
using std::vector;

class CPU;

/* *******************************************************************
 * ********  Used for                                   **************
 * ********    Exchange/Transfer EB:  post-byte         **************
 * ********    Loop Primitive    LB:  post-byte         **************
 * ******************************************************************/
class EBLB {
public:

	class EBLBMSLSRegs {
	public:
		enum { A=0x0, B=0x1, CCR=0x2, TMPx=0x3, D=0x4, X=0x5, Y=0x6, SP=0x7 };
	};

	class EBLBRegs {
	public:
		enum {A=0x0, B=0x1, CCR=0x20, CCRL=0x21, CCRH=0x22, CCRW=0x23, TMP1=0x30, TMP2=0x31, TMP3=0x32, D=0x4, X=0x5, Y=0x6, SP=0x7};
	};

	EBLB(CPU *cpu) { this->cpu = cpu; }
	~EBLB() { cpu = NULL; }

	inline static string getRegName(unsigned int num) {
		switch (num) {
		case EBLBRegs::A: return ("A");
		case EBLBRegs::B: return ("B");
		case EBLBRegs::CCR: return ("CCR");
		case EBLBRegs::CCRL: return ("CCRL");
		case EBLBRegs::CCRH: return ("CCRH");
		case EBLBRegs::CCRW: return ("CCRW");
		case EBLBRegs::TMP1: return ("TMP1");
		case EBLBRegs::TMP2: return ("TMP2");
		case EBLBRegs::TMP3: return ("TMP3");
		case EBLBRegs::D: return ("D");
		case EBLBRegs::X: return ("X");
		case EBLBRegs::Y: return ("Y");
		case EBLBRegs::SP: return ("SP");
		default: return ("?");
		}
	}

	inline static unsigned int getRegSize(unsigned int num) {
		switch (num) {
		case EBLBRegs::A: return (8);
		case EBLBRegs::B: return (8);
		case EBLBRegs::CCR: return (8);
		case EBLBRegs::CCRL: return (8);
		case EBLBRegs::CCRH: return (8);
		case EBLBRegs::CCRW: return (16);
		case EBLBRegs::TMP1: return (16);
		case EBLBRegs::TMP2: return (16);
		case EBLBRegs::TMP3: return (16);
		case EBLBRegs::D: return (16);
		case EBLBRegs::X: return (16);
		case EBLBRegs::Y: return (16);
		case EBLBRegs::SP: return (16);
		default: return (0);
		}
	}

	/* Legal "rr" value for setter and getter functions
	 * 0x00:A; 0x01:B;
	 * 0x20:CCR; 0x21:CCRlow; 0x22:CCRhigh;
	 * 0x30:TMP1; 0x31:TMP2; 0x32:TMP3;
	 * 0x04:D; 0x05:X; 0x06:Y; 0x07:SP
	 */
	template <class T>	void setter(unsigned int rr, T val); // setter function
	template <class T>	T getter(unsigned int rr); // getter function
	template <class T> void exchange(unsigned int rrSrc, unsigned int rrDst);

private:
	CPU *cpu;

};	/***********   END EBLB  **********/


class CPU :
		public Decoder,
		public Client<Loader>,
		public Client<DebugYielding>,
		public Client<MemoryAccessReporting<physical_address_t> >,
		public Service<MemoryAccessReportingControl>,
		public Service<Disassembly<physical_address_t> >,
		public Service<Registers>,
		public Service<Memory<physical_address_t> >,
		public Client<Memory<physical_address_t> >,
		public Client<SymbolTableLookup<physical_address_t> >,
		public Client<TrapReporting >,
		public CallBackObject

{
public:
	enum REGS_OFFSETS {X, D, Y, SP, PC, A, B, CCRL, CCRH, CCR};

	//#define MAX_INS_SIZE	CodeType::maxsize;
	static const unsigned int MAX_INS_SIZE = 8;
	static const unsigned int QUEUE_SIZE = MAX_INS_SIZE;


	inline void queueFlush(unsigned int nByte); // flush is called after prefetch() to advance the queue cursor (first pointer)
	inline uint8_t* queueFetch(address_t addr, uint8_t* ins, unsigned int nByte);

private:
	address_t	queueCurrentAddress;
	uint8_t		queueBuffer[QUEUE_SIZE];

	int		queueFirst, queueNElement;

	inline void queueFill(address_t addr, int position, unsigned int nByte);

public:

	//=====================================================================
	//=                  public service imports/exports                   =
	//=====================================================================

	ServiceImport<Loader> loader_import;
	ServiceExport<Disassembly<physical_address_t> > disasm_export;
	ServiceExport<Registers> registers_export;
	ServiceExport<Memory<physical_address_t> > memory_export;
	ServiceExport<MemoryAccessReportingControl> memory_access_reporting_control_export;

	ServiceImport<DebugYielding> debug_yielding_import;
	ServiceImport<MemoryAccessReporting<physical_address_t> > memory_access_reporting_import;
	ServiceImport<Memory<physical_address_t> > memory_import;
	ServiceImport<SymbolTableLookup<physical_address_t> > symbol_table_lookup_import;

	// the kernel logger
	unisim::kernel::logger::Logger *logger;

	ServiceImport<TrapReporting > trap_reporting_import;

	//=====================================================================
	//=                    Constructor/Destructor                         =
	//=====================================================================

	CPU(const char *name, Object *parent = 0);
	virtual ~CPU();

	virtual void Reset();

	void setEntryPoint(address_t cpu_address);

	//==========================================
	//=     ISA - MEMORY ACCESS ROUTINES       =
	//==========================================

	inline uint8_t memRead8(address_t logicalAddress, ADDRESS::MODE type=ADDRESS::EXTENDED, bool isGlobal=false);
	inline void memWrite8(address_t logicalAddress,uint8_t val, ADDRESS::MODE type=ADDRESS::EXTENDED, bool isGlobal=false);

	inline uint16_t memRead16(address_t logicalAddress, ADDRESS::MODE type=ADDRESS::EXTENDED, bool isGlobal=false);
	inline void memWrite16(address_t logicalAddress,uint16_t val, ADDRESS::MODE type=ADDRESS::EXTENDED, bool isGlobal=false);

	//=====================================================================
	//=                    execution handling methods                     =
	//=====================================================================

	unsigned int step();	// Return the number of cpu cycles consumed by the operation
	virtual void Sync();

	enum STATES {RUNNING, WAIT, STOP};
	/**
	 * RUNNING:
	 * WAIT:
	 *     Enter a wait state for an integer number of bus clock cycle
	 *     Only CPU12 clocks are stopped
	 *     Wait for not masked interrupt
	 * STOP:
	 *     Stop All Clocks and puts the device in standby mode.
	 *     Asserting the RESET, XIRQ, or IRQ signals ends standby mode.
	 *     if S control bit = 1, the STOP instruction is disabled and acts like two-cycle NOP
	 */

	inline void setState(STATES st) { state = st; }
	virtual void sleep() = 0;
	virtual void wai() = 0;

	/*
	 * The CPU issues a signal that tells the interrupt module to drive
	 * the vector address of the highest priority pending exception onto the system address bus
	 * (the CPU12 does not provide this address)
	 *
	 * Priority is as follow: reset => sw-interrupt => hw-interrupt => spurious interrupt
	 *
	 * If (RAM_ACC_VIOL | SYS || SWI || TRAP) return IVBR;
	 * Else return INT_Vector
	 */
	virtual address_t getIntVector(uint8_t &ipl) = 0;

	//=====================================================================
	//=                    Interface with outside                         =
	//=====================================================================
	inline bool hasAsynchronousInterrupt() const { return (asynchronous_interrupt); }

	inline bool hasMaskableIbitInterrup() const { return (maskableIbit_interrupt); }
	inline bool hasNonMaskableXIRQInterrupt() const { return (nonMaskableXIRQ_interrupt); }
	inline bool hasNonMaskableAccessErrorInterrupt() const { return (nonMaskableAccessError_interrupt); }
	inline bool hasNonMaskableSWIInterrupt() const { return (nonMascableSWI_interrupt); }
	inline bool hasTrapInterrupt() const { return (trap_interrupt); }
	inline bool hasReset() const { return (reset); }
	inline bool hasMPUAccessErrorInterrupt() const { return (mpuAccessError_interrupt); }
	inline bool hasSysCallInterrupt() const { return (syscall_interrupt); }
	inline bool hasSpuriousInterrupt() const { return (spurious_interrupt); }

	//=====================================================================
	//=                    Exception handling methods                     =
	//=====================================================================

	// compute return address, save the CPU registers and then set I/X bit before the interrupt handling began
	void saveCPUContext();

	// Asynchronous Interrupts (including Resets, I-bit, XIRQ, IRQ)
	inline void handleException(const AsynchronousException& exc);

	// Hardware and Software reset (including COP, clock monitor, and pin)
	inline void handleResetException(address_t resetVector);

	// Non-maskable (X bit) interrupts
	inline void handleNonMaskableXIRQException(address_t xirqVector, uint8_t newIPL);

	// Non-maskable MPU Access Error interrupt
	inline void handleMPUAccessErrorException(address_t mpuAccessErrorVector, uint8_t newIPL);

	// Maskable (I bit) interrupt
	inline void handleMaskableIbitException(address_t ibitVector, uint8_t newIPL);

	// A software interrupt instruction (SWI) or BDM vector request
	inline void handleException(const NonMaskableSWIInterrupt& exc);

	// Unimplemented opcode trap
	inline void handleException(const TrapException& exc);

	// A system call interrupt instruction (SYS) (CPU12XV1 and CPU12XV2 only)
	inline void handleException(const SysCallInterrupt& exc);

	// A spurious interrupt
	inline void handleException(const SpuriousInterrupt& exc);

	// Non-maskable Access Error interrupts
	inline void handleException(const NonMaskableAccessErrorInterrupt& exc);

	//=====================================================================
	//=               Hardware check/acknowledgement methods              =
	//=====================================================================

	inline void ackAsynchronousInterrupt();
	inline void ackIbitInterrupt();
	inline void ackXIRQInterrupt();
	inline void ackAccessErrorInterrupt();
	inline void ackSWIInterrupt();
	inline void ackTrapInterrupt();
	inline void ackReset();
	inline void ackMPUAccessErrorInterrupt();
	inline void ackSysInterrupt();
	inline void ackSpuriousInterrupt();

	//=====================================================================
	//=                    Hardware interrupt request                     =
	//=====================================================================

	void reqAsynchronousInterrupt();
	void reqIbitInterrupt();
	void reqXIRQInterrupt();
	void reqAccessErrorInterrupt();
	void reqSWIInterrupt();
	void reqTrapInterrupt();
	void reqReset();
	void reqMPUAccessErrorInterrupt();
	void reqSysInterrupt();
	void reqSpuriousInterrupt();

	//======================================================================
	//=                  Registers Acces Routines                          =
	//======================================================================
	inline void    setRegA(uint8_t val);
	inline uint8_t getRegA();

	inline void    setRegB(uint8_t val);
	inline uint8_t getRegB();

	inline void    setRegD(uint16_t val); // regD == regA:regB
	inline uint16_t getRegD();

	inline void    setRegX(uint16_t val);
	inline uint16_t getRegX();

	inline void    setRegY(uint16_t val);
	inline uint16_t getRegY();

	inline void    setRegSP(uint16_t val);
	inline uint16_t getRegSP();

	inline void    setRegPC(uint16_t val);
	inline uint16_t getRegPC();

	inline void    setRegTMP(unsigned int index, uint16_t val);
	inline uint16_t getRegTMP(unsigned int index);

	bool read(unsigned int offset, const void *buffer, unsigned int data_length);
	bool write(unsigned int offset, const void *buffer, unsigned int data_length);

	/********************************************************************
	 * *******  Used for Indexed Operations XB: Postbyte Code  **********
	 * ******************************************************************/
	inline static string xb_getAddrRegLabel(unsigned int rr) {
		switch (rr) {
		case 0:
			return ("X");
		case 1:
			return ("Y");
		case 2:
			return ("SP");
		case 3:
			return ("PC");
		}
		return ("unknown");
	}

	inline static string xb_getAccRegLabel(unsigned int rr) {
		switch (rr) {
		case 0:
			return ("A");
		case 1:
			return ("B");
		case 2:
			return ("D");
		}
		return ("unknown"); // rr=11 see accumulator D offset indexed-indirect
	}

	inline uint16_t xb_getAddrRegValue(unsigned int rr);
	inline void xb_setAddrRegValue(unsigned int rr,uint16_t val);

	inline uint16_t xb_getAccRegValue(unsigned int rr);
	/*************  END  XB  ***************/

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================

	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	virtual void OnDisconnect();

	//=====================================================================
	//=             memory access reporting control interface methods     =
	//=====================================================================

	virtual void RequiresMemoryAccessReporting( MemoryAccessReportingType type, bool report );

	inline void monitorStore(address_t logicalAddress, uint32_t size, bool isGlobal);
	inline void monitorLoad(address_t logicalAddress, uint32_t size, bool isGlobal);

	inline void reportTrap(const char *c_str);

	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================

	virtual void ResetMemory();
	virtual bool ReadMemory(physical_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(physical_address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=             bus interface methods                              =
	//=====================================================================
	virtual void busWrite(MMC_DATA *buffer) = 0;
	virtual void busRead(MMC_DATA *buffer) = 0;

	//=====================================================================
	//=             CPURegistersInterface interface methods               =
	//=====================================================================

	/**
	 * Gets a register interface to the register specified by name.
	 *
	 * @param name The name of the requested register.
	 * @return A pointer to the RegisterInterface corresponding to name.
	 */
	virtual Register *GetRegister(const char *name);

    virtual void ScanRegisters( unisim::service::interfaces::RegisterScanner& scanner );

	//=====================================================================
	//=                   DebugDisasmInterface methods                    =
	//=====================================================================

	/**
	 * Returns a string with the disassembling of the instruction found
	 *   at address addr.
	 *
	 * @param addr The address of the instruction to disassemble.
	 * @param next_addr The address following the requested instruction.
	 * @return The disassembling of the requested instruction address.
	 */
	virtual string Disasm(physical_address_t addr, physical_address_t &next_addr);

	//=====================================================================
	//=                   Debugging methods                               =
	//=====================================================================

	string getObjectFriendlyName(physical_address_t addr);
	string getFunctionFriendlyName(physical_address_t addr);
	inline uint64_t getInstructionCounter() const { return (instruction_counter); }
	inline bool isVerboseException() const { return (debug_enabled && CONFIG::DEBUG_EXCEPTION_ENABLE && (verbose_all || verbose_exception)); }

	inline address_t getLastPC() {return (lastPC); }

	inline bool isDebuggerConnect() { return (debug_yielding_import); }

	//protected:
	class CCR_t *ccr;
	class EBLB	*eblb;

protected:
	//=====================================================================
	//=              CPU Cycle Time/Voltage/Bus Cycle Time                =
	//=====================================================================

//	uint64_t cpu_cycle_time; //!< CPU cycle time in ps
//	uint64_t voltage;        //!< CPU voltage in mV
//	uint64_t bus_cycle_time; //!< Front side bus cycle time in ps
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

	bool enable_trace;
	Parameter<bool> param_enable_trace;

	std::ofstream trace;
	bool enable_file_trace;
	Parameter<bool> param_enable_file_trace;

	uint64_t periodic_trap;
	Parameter<uint64_t> param_periodic_trap;

	// verbose methods
	inline bool VerboseSetup() GCC_INLINE;
	inline bool VerboseStep() GCC_INLINE;
	inline void VerboseDumpRegs() GCC_INLINE;
	inline void VerboseDumpRegsStart() GCC_INLINE;
	inline void VerboseDumpRegsEnd() GCC_INLINE;

	bool requires_memory_access_reporting;      //< indicates if the memory accesses require to be reported
	bool requires_fetch_instruction_reporting;  //< indicates if the fetched instructions require to be reported
	bool requires_commit_instruction_reporting; //< indicates if the committed instructions require to be reported
  
	bool	debug_enabled;
	Parameter<bool>	param_debug_enabled;


	//=====================================================================
	//=                   68HCS12X interrupt signals                      =
	//=====================================================================

	bool asynchronous_interrupt;
	bool maskableIbit_interrupt;	// I-Bit maskable interrupts => IVBR + 0x0012-0x00F2 (113 interrupts)
	bool nonMaskableXIRQ_interrupt;	// X-Bit (XIRQ) maskable interrupt => IVBR + 0x00F4
	bool nonMaskableAccessError_interrupt; // Memory Access Error Interrupt
	bool nonMascableSWI_interrupt;	// (SWI) => IVBR + 0x00F6
	// non maskable software interrupt request or background debug mode vector request
	bool trap_interrupt;			// non maskable unimplemented opcode => IVBR + 0x00F8
	bool reset;						// Hardware and Software interrupt =>  0xFFFA-0xFFFE
	bool mpuAccessError_interrupt; // non maskable MPU Access Error interrupt
	bool syscall_interrupt;			// SYS call interrupt =>
	bool spurious_interrupt;		// Spurious interrupt => IVBR + 0x0010 (default interrupt)

private:
	// cpu state
	STATES state;

	uint8_t		regA, regB;
	uint16_t    regX, regY, regSP, regPC, ccrReg;
	uint16_t	regTMP[3];

	address_t lastPC;

	// Registers map
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	std::vector<unisim::kernel::VariableBase*> extended_registers_registry;

	/** the instruction counter */
	uint64_t instruction_counter;
	uint64_t cycles_counter;
	uint64_t data_load_counter;
	uint64_t data_store_counter;
	uint64_t	max_inst;

	Statistic<uint64_t> stat_instruction_counter;
	Statistic<uint64_t> stat_cycles_counter;
	Statistic<uint64_t> stat_load_counter;
	Statistic<uint64_t> stat_store_counter;

	Parameter<uint64_t>	   param_max_inst;

};

// ==============================================
// =          MEMORY ACCESS ROUTINES            =
// ==============================================

inline void CPU::monitorLoad(address_t logicalAddress, uint32_t size, bool isGlobal)
{
//	physical_address_t pea = MMC::getInstance()->getCPU12XPhysicalAddress(logicalAddress, ADDRESS::EXTENDED,isGlobal,false, 0x00);

	data_load_counter++;

	// Memory access reporting
	if(memory_access_reporting_import && requires_memory_access_reporting)
	{
//		memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, pea, size);
		memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, logicalAddress, size);
	}
}

inline void CPU::monitorStore(address_t logicalAddress, uint32_t size, bool isGlobal)
{
//	physical_address_t pea = MMC::getInstance()->getCPU12XPhysicalAddress(logicalAddress, ADDRESS::EXTENDED,isGlobal,false, 0x00);

	data_store_counter++;

	// Memory access reporting
	if(memory_access_reporting_import && requires_memory_access_reporting)
	{
//		memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, pea, size);
		memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, logicalAddress, size);
	}
}

inline void CPU::reportTrap(const char *c_str) {
	if (trap_reporting_import) {
		trap_reporting_import->ReportTrap(*this, c_str);
	}

}

inline uint8_t CPU::memRead8(address_t logicalAddress, ADDRESS::MODE type, bool isGlobal) {

	uint8_t data;
	MMC_DATA mmc_data;

	mmc_data.logicalAddress = logicalAddress;
	mmc_data.type = type;
	mmc_data.isGlobal = isGlobal;
	mmc_data.buffer = &data;
	mmc_data.data_size = 1;

	busRead(&mmc_data);

	monitorLoad(logicalAddress, sizeof(data), isGlobal);

	return (data);
}

inline uint16_t CPU::memRead16(address_t logicalAddress, ADDRESS::MODE type, bool isGlobal) {

	uint16_t data;
	MMC_DATA mmc_data;

	mmc_data.logicalAddress = logicalAddress;
	mmc_data.type = type;
	mmc_data.isGlobal = isGlobal;
	mmc_data.buffer = &data;
	mmc_data.data_size = 2;

	busRead(&mmc_data);

	data = BigEndian2Host(data);

	monitorLoad(logicalAddress, sizeof(data), isGlobal);

	return (data);
}

inline void CPU::memWrite8(address_t logicalAddress,uint8_t data, ADDRESS::MODE type, bool isGlobal) {

	MMC_DATA mmc_data;

	mmc_data.logicalAddress = logicalAddress;
	mmc_data.type = type;
	mmc_data.isGlobal = isGlobal;
	mmc_data.buffer = &data;
	mmc_data.data_size = 1;
	mmc_data.isExecute = false;

	busWrite(&mmc_data);

	monitorStore(logicalAddress, sizeof(data), isGlobal);

}

inline void CPU::memWrite16(address_t logicalAddress,uint16_t data, ADDRESS::MODE type, bool isGlobal) {

	MMC_DATA mmc_data;

	data = Host2BigEndian(data);

	mmc_data.logicalAddress = logicalAddress;
	mmc_data.type = type;
	mmc_data.isGlobal = isGlobal;
	mmc_data.buffer = &data;
	mmc_data.data_size = 2;

	busWrite(&mmc_data);

	monitorStore(logicalAddress, sizeof(data), isGlobal);

}

//======================================================================
//=                  Registers Acces Routines                          =
//======================================================================

uint16_t CPU::xb_getAddrRegValue(unsigned int rr) {
	switch (rr) {
	case 0:
		return (getRegX());
	case 1:
		return (getRegY());
	case 2:
		return (getRegSP());
	case 3:
		return (getRegPC());
	}
	throw std::runtime_error("Internal error");
}

void CPU::xb_setAddrRegValue(unsigned int rr,uint16_t val) {
	switch (rr) {
	case 0:
		(setRegX(val)); break;
	case 1:
		(setRegY(val)); break;
	case 2:
		(setRegSP(val)); break;
	case 3:
		(setRegPC(val)); break;
	default:
		throw std::runtime_error("Internal error"); break;
	}
}


uint16_t CPU::xb_getAccRegValue(unsigned int rr) {
	switch (rr) {
	case 0:
		return (getRegA());
	case 1:
		return (getRegB());
	case 2:
		return (getRegD());
	}
	throw std::runtime_error("Internal error");
}


void CPU::setRegA(uint8_t val) { regA = val; }

uint8_t CPU::getRegA() { return (regA); }

void CPU::setRegB(uint8_t val) { regB = val; }
uint8_t CPU::getRegB() { return (regB); }

void CPU::setRegD(uint16_t val) {
	// regD == regA:regB

	setRegA((uint8_t) (val >> 8));
	setRegB((uint8_t) (val & 0x00FF));

}

uint16_t CPU::getRegD() {
	// regD == regA:regB
	uint16_t val = (((uint16_t) getRegA()) << 8) | getRegB();
	return (val);
}

void CPU::setRegX(uint16_t val) { regX = val; }
uint16_t CPU::getRegX() { return (regX); }

void CPU::setRegY(uint16_t val) { regY = val; }

uint16_t CPU::getRegY() { return (regY); }

void CPU::setRegSP(uint16_t val) { regSP = val; }
uint16_t CPU::getRegSP() { return (regSP); }

void CPU::setRegPC(uint16_t val) {
	if (val == 0) {
		reportTrap("Try to set PC to 0x0000");
	} else {
		regPC = val;
	}

}
uint16_t CPU::getRegPC() { return (regPC); }

void CPU::setRegTMP(unsigned int index, uint16_t val) { regTMP[index] = val; }
uint16_t CPU::getRegTMP(unsigned int index) { return (regTMP[index]); }


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CPU_HH__

