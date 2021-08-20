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

#include <unisim/component/cxx/processor/hcs12x/hcs12x.hh>
#include <unisim/component/cxx/processor/hcs12x/cpu.hh>
#include <unisim/util/inlining/inlining.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

using std::cout;

using unisim::util::debug::SimpleRegister;
using unisim::util::debug::Symbol;

template void EBLB::setter<uint8_t>(unsigned int rr, uint8_t val);
template void EBLB::setter<uint16_t>(unsigned int rr, uint16_t val);
template uint8_t EBLB::getter<uint8_t>(unsigned int rr);
template uint16_t EBLB::getter<uint16_t>(unsigned int rr);
template void EBLB::exchange<uint8_t>(unsigned int rrSrc, unsigned int rrDst);
template void EBLB::exchange<uint16_t>(unsigned int rrSrc, unsigned int rrDst);

CPU::CPU(const char *name, Object *parent)
	: Object(name, parent)
	, Client<Loader>(name,  parent)
	, unisim::kernel::Client<DebugYielding>(name, parent)
	, unisim::kernel::Client<MemoryAccessReporting<physical_address_t> >(name, parent)
	, unisim::kernel::Service<MemoryAccessReportingControl>(name, parent)
	, unisim::kernel::Service<Disassembly<physical_address_t> >(name, parent)
	, unisim::kernel::Service<Registers>(name, parent)
	, unisim::kernel::Service<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::Client<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::Client<SymbolTableLookup<physical_address_t> >(name, parent)
	, unisim::kernel::Client<TrapReporting>(name, parent)
	, queueCurrentAddress(0xFFFE)
	, queueFirst(-1)
	, queueNElement(0)
	, loader_import("loader-import",  this)
	, disasm_export("disasm_export", this)
	, registers_export("registers_export", this)
	, memory_export("memory_export", this)
	, memory_access_reporting_control_export("memory_access_reporting_control_export", this)
	, debug_yielding_import("debug_yielding_import", this)
	, memory_access_reporting_import("memory_access_reporting_import", this)
	, memory_import("memory_import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import",  this)
	, logger(0)
	, trap_reporting_import("trap_reporting_import", this)
	, cpu_cycle(0)
	, bus_cycle(0)
	, verbose_all(false)
	, param_verbose_all("verbose-all", this, verbose_all)
	, verbose_setup(false)
	, param_verbose_setup("verbose-setup", this, verbose_setup)
	, verbose_step(false)
	, param_verbose_step("verbose-step", this, verbose_step)
	, verbose_dump_regs_start(false)
	, param_verbose_dump_regs_start("verbose-dump-regs-start", this, verbose_dump_regs_start)
	, verbose_dump_regs_end(false)
	, param_verbose_dump_regs_end("verbose-dump-regs-end", this, verbose_dump_regs_end)
	, verbose_exception(false)
	, param_verbose_exception("verbose-exception", this, verbose_exception)
	, enable_trace(false)
	, param_enable_trace("enable-trace", this, enable_trace)
	, enable_file_trace(false)
	, param_enable_file_trace("enable-file-trace", this, enable_file_trace)
	, periodic_trap(-1)
	, param_periodic_trap("periodic-trap", this, periodic_trap)
	, requires_memory_access_reporting(false)
	, requires_fetch_instruction_reporting(false)
	, requires_commit_instruction_reporting(false)
	, debug_enabled(false)
	, param_debug_enabled("debug-enabled", this, debug_enabled)
	, asynchronous_interrupt(false)
	, maskableIbit_interrupt(false)
	, nonMaskableXIRQ_interrupt(false)
	, nonMaskableAccessError_interrupt(false)
	, nonMascableSWI_interrupt(false)
	, trap_interrupt(false)
	, reset(false)
	, mpuAccessError_interrupt(false)
	, syscall_interrupt(false)
	, spurious_interrupt(false)
	, state(CPU::RUNNING)
	, lastPC(0)
	, instruction_counter(0)
	, cycles_counter(0)
	, data_load_counter(0)
	, data_store_counter(0)
	, max_inst((uint64_t) -1)
	, stat_instruction_counter("instruction-counter", this, instruction_counter)
	, stat_cycles_counter("cycles-counter", this, cycles_counter)
	, stat_load_counter("data-load-counter", this, data_load_counter)
	, stat_store_counter("data-store-counter", this, data_store_counter)
	, param_max_inst("max-inst",this,max_inst)
{
	disasm_export.SetupDependsOn(memory_import);
	memory_export.SetupDependsOn(memory_import);
	
	param_max_inst.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_periodic_trap.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

	stat_instruction_counter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	stat_instruction_counter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	stat_cycles_counter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	stat_load_counter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	stat_store_counter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	

    ccr = new CCR_t(&ccrReg);

    eblb = new EBLB(this);

	logger = new unisim::kernel::logger::Logger(*this);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>("A", &regA));
	extended_registers_registry.push_back(new unisim::kernel::variable::Register<uint8_t>("A", this, regA, "Accumulator register A"));

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>("B", &regB));
	extended_registers_registry.push_back(new unisim::kernel::variable::Register<uint8_t>("B", this, regB, "Accumulator register B"));

	registers_registry.AddRegisterInterface(new ConcatenatedRegister<uint16_t,uint8_t>("D", &regA, &regB));
//	extended_registers_registry.push_back(new ConcatenatedRegisterView<uint16_t,uint8_t>("D", this,  &regA, &regB, "Accumulator register D"));

	ConcatenatedRegisterView<uint16_t,uint8_t> *d_var = new ConcatenatedRegisterView<uint16_t,uint8_t>("D", this,  &regA, &regB, "16-bits Accumulator register (D=A:B)");
	extended_registers_registry.push_back(d_var);
	d_var->setCallBack(this, D, &CallBackObject::write, NULL);


	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>("X", &regX));
//	extended_registers_registry.push_back(new unisim::kernel::variable::Register<uint16_t>("X", this, regX, "Index register X"));

	unisim::kernel::variable::Register<uint16_t> *x_var = new unisim::kernel::variable::Register<uint16_t>("X", this, regX, "16-bits index register (X)");
	extended_registers_registry.push_back(x_var);
	x_var->setCallBack(this, X, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>("Y", &regY));
//	extended_registers_registry.push_back(new unisim::kernel::variable::Register<uint16_t>("Y", this, regY, "Index register Y"));

	unisim::kernel::variable::Register<uint16_t> *y_var = new unisim::kernel::variable::Register<uint16_t>("Y", this, regY, "16-bits index register (Y)");
	extended_registers_registry.push_back(y_var);
	y_var->setCallBack(this, Y, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>("SP", &regSP));
//	extended_registers_registry.push_back(new unisim::kernel::variable::Register<uint16_t>("SP", this, regSP, "Stack Pointer SP"));

	unisim::kernel::variable::Register<uint16_t> *sp_var = new unisim::kernel::variable::Register<uint16_t>("SP", this, regSP, "16-bits stack pointer register (SP)");
	extended_registers_registry.push_back(sp_var);
	sp_var->setCallBack(this, SP, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>("PC", &regPC));
//	extended_registers_registry.push_back(new unisim::kernel::variable::Register<uint16_t>("PC", this, regPC, "Program counter PC"));

	unisim::kernel::variable::Register<uint16_t> *pc_var = new unisim::kernel::variable::Register<uint16_t>("PC", this, regPC, "16-bits program counter register (PC)");
	extended_registers_registry.push_back(pc_var);
	pc_var->setCallBack(this, PC, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(ccr);
//	extended_registers_registry.push_back(new unisim::kernel::variable::Register<uint16_t>(ccr->GetName(), this, ccrReg, "CCR"));

	unisim::kernel::variable::Register<uint16_t> *ccr_var = new unisim::kernel::variable::Register<uint16_t>(ccr->GetName(), this, ccrReg, "16-bits condition code register (CCR)");
	extended_registers_registry.push_back(ccr_var);
	ccr_var->setCallBack(this, CCR, &CallBackObject::write, NULL);

	unisim::service::interfaces::Register *ccrl = ccr->GetLowRegister();
	registers_registry.AddRegisterInterface(ccrl);
	extended_registers_registry.push_back(new TimeBaseRegisterView(ccrl->GetName(), this, ccrReg, TimeBaseRegisterView::TB_LOW, "CCR LOW"));

	unisim::service::interfaces::Register *ccrh = ccr->GetHighRegister();
	registers_registry.AddRegisterInterface(ccrh);
	extended_registers_registry.push_back(new TimeBaseRegisterView(ccrh->GetName(), this, ccrReg, TimeBaseRegisterView::TB_HIGH, "CCR HIGH"));

}

CPU::~CPU()
{
	if (eblb) { delete eblb; eblb = NULL;}
	// ccr will be release while releasing registers_registry
	//if (ccr) { delete ccr; ccr = NULL;}

	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

	if (logger) { delete logger; logger = NULL;}

	if (trace.is_open()) { trace.close(); }
}

void CPU::setEntryPoint(address_t cpu_address)
{
	setRegPC(cpu_address);
}


void CPU::Reset()
{
	regA = 0;
	regB = 0;
	regX = 0;
	regY = 0;
	regSP = 0;
	regPC = 0;
	regTMP[0] = 0;
	regTMP[1] = 0;
	regTMP[2] = 0;
	lastPC = 0;

	ccr->reset();
	for (unsigned int i=0; i < QUEUE_SIZE; i++) queueBuffer[i] = 0;
}

void CPU::ResetMemory()
{
	
	Reset();

}

bool CPU::read(unsigned int offset, const void *buffer, unsigned int data_length) {

	std::cout << "Read back" << std::endl;

	switch (offset) {
		case X: *((uint16_t *) buffer) = Host2BigEndian(getRegX()); break;
		case D: *((uint16_t *) buffer) = Host2BigEndian(getRegD()); break;
		case Y: *((uint16_t *) buffer) = Host2BigEndian(getRegY()); break;
		case SP: *((uint16_t *) buffer) = Host2BigEndian(getRegSP()); break;
		case PC: *((uint16_t *) buffer) = Host2BigEndian(getRegPC()); break;
		case A: *((uint8_t *) buffer) = getRegA(); break;
		case B: *((uint8_t *) buffer) = getRegB(); break;
		case CCRL: *((uint8_t *) buffer) = ccr->getCCRLow(); break;
		case CCRH: *((uint8_t *) buffer) = ccr->getCCRHigh(); break;
		case CCR: *((uint16_t *) buffer) = Host2BigEndian(ccr->getCCR()); break;
		default: return (false);
	}

	return (true);
}

bool CPU::write(unsigned int offset, const void *buffer, unsigned int data_length) {

	std::cout << "Write back" << std::hex << *((uint16_t *) buffer) << std::endl;

	switch (offset) {
		case X: setRegX(BigEndian2Host(*((uint16_t *) buffer))); break;
		case D: setRegD(BigEndian2Host(*((uint16_t *) buffer))); break;
		case Y: setRegY(BigEndian2Host(*((uint16_t *) buffer))); break;
		case SP: setRegSP(BigEndian2Host(*((uint16_t *) buffer))); break;
		case PC: setRegPC(BigEndian2Host(*((uint16_t *) buffer))); break;
		case A: setRegA(*((uint8_t *) buffer)); break;
		case B: setRegB(*((uint8_t *) buffer)); break;
		case CCRL: ccr->setCCRLow(*((uint8_t *) buffer)); break;
		case CCRH: ccr->setCCRHigh(*((uint8_t *) buffer)); break;
		case CCR: ccr->setCCR(BigEndian2Host(*((uint16_t *) buffer))); break;
		default: return (false);
	}

	return (true);
}

//=====================================================================
//=                    execution handling methods                     =
//=====================================================================


void CPU::Sync()
{

}


unsigned int CPU::step()
{
	try
	{
		if (hasAsynchronousInterrupt())
			throw AsynchronousException();
                
		if (state == STOP)
		{
			reportTrap("CPU is in STOP mode");
			return 0;
		}

		address_t curPC = getRegPC();

		if (verbose_step)
			*logger << DebugInfo << "Starting step at PC = 0x" << std::hex << curPC << std::dec << std::endl << EndDebugInfo;

                if (requires_fetch_instruction_reporting and memory_access_reporting_import)
			memory_access_reporting_import->ReportFetchInstruction(MMC::getInstance()->getCPU12XPagedAddress(curPC));
		
		if (debug_enabled)
			VerboseDumpRegsStart();
		
		if (debug_yielding_import)
			debug_yielding_import->DebugYield();
		

		if (requires_memory_access_reporting and memory_access_reporting_import)
			memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_INSN, curPC, MAX_INS_SIZE);


		if (debug_enabled && verbose_step)
		{
			*logger << DebugInfo
				<< "Fetching (reading) instruction at address 0x"
				<< std::hex << curPC << std::dec
				<< std::endl << EndDebugInfo;
		}

		CodeType insn(CodeType::capacity * 8);
		queueFetch(curPC, &insn.str[0], CodeType::capacity);

		/* Decode current PC */
		if (debug_enabled && verbose_step)
		{
			stringstream ctstr;
			ctstr << insn;
			*logger << DebugInfo
				<< "Decoding instruction at 0x"
				<< std::hex << curPC << std::dec
				<< " (0x" << std::hex << ctstr.str() << std::dec << ")"
				<< std::endl << EndDebugInfo;
		}

		Operation* op = this->Decode(MMC::getInstance()->getCPU12XPagedAddress(curPC), insn);
		unsigned int insn_length = op->GetLength();
		if (insn_length % 8) throw "InternalError";

		queueFlush(insn_length/8);

		/* Execute instruction */

		if (debug_enabled && verbose_step) {
			stringstream disasm_str;
			stringstream ctstr;

			op->disasm(disasm_str);

			ctstr << op->GetEncoding();
			*logger << DebugInfo << (Object::GetSimulator()->GetSimTime())
				<< " : Executing instruction "
				<< disasm_str.str()
				<< " at PC = 0x" << std::hex << curPC << std::dec
				<< " (0x" << std::hex << ctstr.str() << std::dec << ") , Instruction Counter = " << instruction_counter
				<< "  " << EndDebugInfo	<< std::endl;
		}

		lastPC = curPC;
		setRegPC(curPC + (insn_length/8));

		if (enable_trace) {
			stringstream disasm_str;
			stringstream ctstr;

			ctstr << op->GetEncoding();

			disasm_str << "Cycles = " << std::dec << cycles_counter
				<< " : Time = " << std::dec << (Object::GetSimulator()->GetSimTime())
				<< " : PC = 0x" << std::hex << curPC << std::dec << " : "
				<< getFunctionFriendlyName(curPC) << " : ";

			op->disasm(disasm_str);

			disasm_str	<< " : (0x" << std::hex << ctstr.str() << std::dec << " ) "
				<< std::endl;

			if (enable_file_trace) {
				trace << disasm_str.str();
			} else {
				std::cout << disasm_str.str();
			}
		}

		unsigned opCycles = op->execute(this);

		cycles_counter += opCycles;

		VerboseDumpRegsEnd();

		instruction_counter++;

		if (trap_reporting_import and ((instruction_counter % periodic_trap) == 0))
			trap_reporting_import->ReportTrap();

		if (requires_commit_instruction_reporting and memory_access_reporting_import)
			memory_access_reporting_import->ReportCommitInstruction(MMC::getInstance()->getCPU12XPagedAddress(curPC), (insn_length/8));

		if (instruction_counter >= max_inst)
			Stop(0);

		return opCycles;
	}

	catch (AsynchronousException& exc) {
		handleException(exc);
	}

	catch (NonMaskableAccessErrorInterrupt& exc) {
		handleException(exc);
	}

	catch (NonMaskableSWIInterrupt& exc) {
		handleException(exc);
	}

	catch (TrapException& exc) {
		handleException(exc);
	}

	catch (SysCallInterrupt& exc) {
		handleException(exc);
	}

	catch (SpuriousInterrupt& exc) {
		handleException(exc);
	}

	catch(Exception& e)
	{
		if(debug_enabled && verbose_step)
			*logger << DebugError << "uncaught processor exception :" << e.what() << std::endl << EndDebugError;
		Stop(1);
	}

	return 0;
}

//=====================================================================
//=              HCS12X (CPU12X) Queue Handling                       =
//=====================================================================

uint8_t* CPU::queueFetch(address_t addr, uint8_t* ins, unsigned int nByte)
{

	if (nByte > QUEUE_SIZE) return (NULL);

	if (addr != queueCurrentAddress)
	{
		queueFill(addr, 0, QUEUE_SIZE);
		queueFirst = 0;
		queueNElement = QUEUE_SIZE;
		queueCurrentAddress = addr;
	}
	else if (nByte > (unsigned int) queueNElement)
	{
		queueFill(addr+queueNElement, (queueFirst+queueNElement) % QUEUE_SIZE, QUEUE_SIZE-queueNElement);
		queueNElement = QUEUE_SIZE;
	}

	for (unsigned int i=0; i < nByte; i++)
	{
		ins[i] = queueBuffer[(queueFirst + i) % QUEUE_SIZE];
	}

	return (ins);
}

void CPU::queueFill(address_t addr, int position, unsigned int nByte)
{
	uint8_t buff[QUEUE_SIZE];

	MMC_DATA mmc_data;

	mmc_data.logicalAddress = addr;
	mmc_data.type = ADDRESS::EXTENDED;
	mmc_data.isGlobal = false;
	mmc_data.buffer = buff;
	mmc_data.data_size = nByte;

	busRead(&mmc_data);

	for (unsigned int i=0; i<nByte; i++)
	{
		queueBuffer[position] = buff[i];
		position = (position + 1) % QUEUE_SIZE;
	}

}

void CPU::queueFlush(unsigned int nByte)
{
	queueFirst = (queueFirst + nByte) % QUEUE_SIZE;
	queueNElement = queueNElement - nByte;
	queueCurrentAddress = queueCurrentAddress + nByte;
}

//=====================================================================
//=                    Exception handling methods                     =
//=====================================================================

// compute return address, save the CPU registers and then set I/X bit before the interrupt handling began
void CPU::saveCPUContext() {

	if ((state == STOP) || (state == WAIT)) {
		setState(RUNNING);
		// Don't save CPU context because STOP or WAI instructions have already done it.
	} else {

		/*
		 * *** Save context sequence for CPU12X ***
		 *
		 * - The instruction queue is refilled
		 * - The return address is calculated
		 * - Save the return address and CPU registers as follow:
		 *    . M(SP+8) <= RTNH:RTNL
		 *    . M(SP+6) <= YH:YL
		 *    . M(SP+4) <= XH:XL
		 *    . M(SP+2) <= B:A
		 *    . M(SP)   <= CCRH:CCRL
		 */

		setRegSP(getRegSP()-2);
		memWrite16(getRegSP(), getRegPC());

		setRegSP(getRegSP()-2);
		memWrite16(getRegSP(), getRegY());

		setRegSP(getRegSP()-2);
		memWrite16(getRegSP(), getRegX());

		setRegSP(getRegSP()-1);
		memWrite8(getRegSP(), getRegA());
		setRegSP(getRegSP()-1);
		memWrite8(getRegSP(), getRegB());

		setRegSP(getRegSP()-2);
		memWrite16(getRegSP(), ccr->getCCR());

	}

}

// AsynchronousException
inline void CPU::handleException(const AsynchronousException& exc)
{

	uint8_t newIPL = ccr->getIPL();
	address_t asyncVector = getIntVector(newIPL);

	asynchronous_interrupt = false;

	if (CONFIG::HAS_RESET && hasReset()) {
		handleResetException(asyncVector);
	}

	if (CONFIG::HAS_NON_MASKABLE_XIRQ_INTERRUPT && hasNonMaskableXIRQInterrupt()) {
		handleNonMaskableXIRQException(asyncVector, newIPL);
	}

	if (hasMPUAccessErrorInterrupt()) {
		handleMPUAccessErrorException(asyncVector, newIPL);
	}

	if (CONFIG::HAS_MASKABLE_IBIT_INTERRUPT && hasMaskableIbitInterrup()) {
		handleMaskableIbitException(asyncVector, newIPL);
	}

}

inline void CPU::ackAsynchronousInterrupt()
{
	if (CONFIG::HAS_RESET)  asynchronous_interrupt |= reset;
	if (CONFIG::HAS_NON_MASKABLE_XIRQ_INTERRUPT)  asynchronous_interrupt |= nonMaskableXIRQ_interrupt;
	if (CONFIG::HAS_MASKABLE_IBIT_INTERRUPT) asynchronous_interrupt |= maskableIbit_interrupt;

}

void CPU::reqAsynchronousInterrupt()
{
	asynchronous_interrupt = true;
}

// Hardware and Software reset
inline void CPU::handleResetException(address_t resetVector)
{

	ackReset();

	this->Reset();
	// clear U-bit for CPU12XV2
	ccr->clrIPL();

	address_t address = memRead16(resetVector);

	setEntryPoint(address);

}

inline void CPU::ackReset()
{
	reset = false;
	ackAsynchronousInterrupt();
}

void CPU::reqReset()
{
	reset = true;
}

// NonMaskable XIRQ (X bit) interrupts
inline void CPU::handleNonMaskableXIRQException(address_t xirqVector, uint8_t newIPL)
{
	ackXIRQInterrupt();

	if (ccr->getX() != 0) return;

	saveCPUContext();

	ccr->setI();
	// clear U-bit for CPU12XV2
	ccr->setIPL(newIPL);
	ccr->setX();

	address_t address = memRead16(xirqVector);

	setEntryPoint(address);

}

inline void CPU::ackXIRQInterrupt()
{
	nonMaskableXIRQ_interrupt = false;
	ackAsynchronousInterrupt();
}

void CPU::reqXIRQInterrupt()
{
	if (ccr->getX() == 0) nonMaskableXIRQ_interrupt = true;
}

// Non-maskable MPU Access Error interrupt
inline void CPU::handleMPUAccessErrorException(address_t mpuAccessErrorVector, uint8_t newIPL)
{
	ackMPUAccessErrorInterrupt();

	saveCPUContext();

	ccr->setIPL(newIPL);

	address_t address = memRead16(mpuAccessErrorVector);

	setEntryPoint(address);

}

inline void CPU::ackMPUAccessErrorInterrupt()
{
	mpuAccessError_interrupt = false;
	ackAsynchronousInterrupt();
}

void CPU::reqMPUAccessErrorInterrupt()
{
	mpuAccessError_interrupt = true;
}

// Maskable (I bit) interrupt
inline void CPU::handleMaskableIbitException(address_t ibitVector, uint8_t newIPL)
{
	ackIbitInterrupt();

	if (ccr->getI() != 0) return;

	if (newIPL > ccr->getIPL()) {
		saveCPUContext();

		ccr->setI();
		// clear U-bit for CPU12XV2
		ccr->setIPL(newIPL);

		address_t address = memRead16(ibitVector);

		setEntryPoint(address);
	}

}

inline void CPU::ackIbitInterrupt()
{
	maskableIbit_interrupt = false;
	ackAsynchronousInterrupt();
}

void CPU::reqIbitInterrupt()
{
	uint8_t iBit = ccr->getI();

	if (iBit == 0) maskableIbit_interrupt = true;
}

// Unimplemented opcode trap
inline void CPU::handleException(const TrapException& exc)
{
	reqTrapInterrupt();

	uint8_t newIPL = ccr->getIPL();

	address_t trapVector = getIntVector(newIPL);

	saveCPUContext();

	ccr->setI();
	// clear U-bit for CPU12XV2

	address_t address = memRead16(trapVector);

	setEntryPoint(address);

	ackTrapInterrupt();
}

inline void CPU::ackTrapInterrupt()
{
	trap_interrupt = false;
}

inline void CPU::reqTrapInterrupt()
{
	trap_interrupt = true;
}

// A software interrupt instruction (SWI) or BDM vector request
inline void CPU::handleException(const NonMaskableSWIInterrupt& exc)
{
	reqSWIInterrupt();

	uint8_t newIPL = ccr->getIPL();

	address_t swiVector = getIntVector(newIPL);

	saveCPUContext();

	ccr->setI();
	// clear U-bit for CPU12XV2

	address_t address = memRead16(swiVector);

	setEntryPoint(address);

	ackSWIInterrupt();
}

inline void CPU::ackSWIInterrupt()
{
	nonMascableSWI_interrupt = false;
}

inline void CPU::reqSWIInterrupt()
{
	nonMascableSWI_interrupt = true;
}

// A system call interrupt instruction (SYS) (CPU12XV1 and CPU12XV2 only)
inline void CPU::handleException(const SysCallInterrupt& exc)
{
	reqSysInterrupt();

	uint8_t newIPL = ccr->getIPL();

	address_t sysCallVector = getIntVector(newIPL);

	saveCPUContext();

	ccr->setI();
	// clear U-bit for CPU12XV2

	address_t address = memRead16(sysCallVector);

	setEntryPoint(address);

	ackSysInterrupt();
}

inline void CPU::ackSysInterrupt()
{
	syscall_interrupt = false;
}

inline void CPU::reqSysInterrupt()
{
	syscall_interrupt = true;
}

// A spurious interrupt
inline void CPU::handleException(const SpuriousInterrupt& exc)
{
	reqSpuriousInterrupt();

	uint8_t newIPL = ccr->getIPL();

	address_t spuriousVector = getIntVector(newIPL);

	saveCPUContext();

	ccr->setI();
	// clear U-bit for CPU12XV2

	address_t address = memRead16(spuriousVector);

	setEntryPoint(address);

	ackSpuriousInterrupt();
}

inline void CPU::ackSpuriousInterrupt()
{
	spurious_interrupt = false;
}

inline void CPU::reqSpuriousInterrupt()
{
	spurious_interrupt = true;
}

// NonMaskable Access Error interrupts
inline void CPU::handleException(const NonMaskableAccessErrorInterrupt& exc)
{
	reqAccessErrorInterrupt();

	uint8_t newIPL = ccr->getIPL();

	address_t accessErrorVector = getIntVector(newIPL);

	address_t address = memRead16(accessErrorVector);

	setEntryPoint(address);

	ackAccessErrorInterrupt();

}

inline void CPU::ackAccessErrorInterrupt()
{
	nonMaskableAccessError_interrupt = false;
	ackAsynchronousInterrupt();
}

inline void CPU::reqAccessErrorInterrupt()
{
	nonMaskableAccessError_interrupt = true;
	reqAsynchronousInterrupt();
}


// ======================================================
// =    Transfer/Exchange: Registers setters/getters    =
// ======================================================

template <class T>
void EBLB::setter(unsigned int rr, T val) // setter function
	/* Legal "rr" value for setter and getter functions
	 * 0x00:A; 0x01:B;
	 * 0x20:CCR; 0x21:CCRlow; 0x22:CCRhigh; 0x23:CCRWord
	 * 0x30:TMP1; 0x31:TMP2; 0x32:TMP3;
	 * 0x04:D; 0x05:X; 0x06:Y; 0x07:SP
	 */
{
	switch (rr) {
		case EBLBRegs::A: {
			cpu->setRegA((uint8_t) val);
		} break;
		case EBLBRegs::B: {
			cpu->setRegB((uint8_t) val);
		} break;
		case EBLBRegs::CCR: {
			cpu->ccr->setCCRLow((uint8_t) val);
		} break;
		case EBLBRegs::CCRL: {
			cpu->ccr->setCCRLow((uint8_t) val);
		} break;
		case EBLBRegs::CCRH: {
			cpu->ccr->setCCRHigh((uint8_t) val);
		} break;
		case EBLBRegs::CCRW: {
			cpu->ccr->setCCR((uint16_t) val);
		} break;
		case EBLBRegs::TMP1: {
			cpu->setRegTMP(0, (uint16_t) val);
		} break;
		case EBLBRegs::TMP2: {
			cpu->setRegTMP(1, (uint16_t) val);
		} break;
		case EBLBRegs::TMP3: {
			cpu->setRegTMP(2, (uint16_t) val);
		} break;
		case EBLBRegs::D: {
			cpu->setRegD((uint16_t) val);
		} break;
		case EBLBRegs::X: {
			cpu->setRegX((uint16_t) val);
		} break;
		case EBLBRegs::Y: {
			cpu->setRegY((uint16_t) val);
		} break;
		case EBLBRegs::SP: {
			cpu->setRegSP((uint16_t) val);
		} break;
		default: break;
	}
}

template <class T>
T EBLB::getter(unsigned int rr) // getter function
	/* Legal "rr" value for setter and getter functions
	 * 0x00:A; 0x01:B;
	 * 0x20:CCR; 0x21:CCRlow; 0x22:CCRhigh; 0x23:CCRWord
	 * 0x30:TMP1; 0x31:TMP2; 0x32:TMP3;
	 * 0x04:D; 0x05:X; 0x06:Y; 0x07:SP
	 */
{
	switch (rr) {
		case EBLBRegs::A: {
			return ((uint8_t) cpu->getRegA());
		} break;
		case EBLBRegs::B: {
			return ((uint8_t) cpu->getRegB());
		} break;
		case EBLBRegs::CCR: {
			return ((uint8_t) cpu->ccr->getCCRLow());
		} break;
		case EBLBRegs::CCRL: {
			return ((uint8_t) cpu->ccr->getCCRLow());
		} break;
		case EBLBRegs::CCRH: {
			return ((uint8_t) cpu->ccr->getCCRHigh());
		} break;
		case EBLBRegs::CCRW: {
			return ((uint16_t) cpu->ccr->getCCR());
		} break;
		case EBLBRegs::TMP1: {
			return ((uint16_t) cpu->getRegTMP(0));
		} break;
		case EBLBRegs::TMP2: {
			return ((uint16_t) cpu->getRegTMP(1));
		} break;
		case EBLBRegs::TMP3: {
			return ((uint16_t) cpu->getRegTMP(2));
		} break;
		case EBLBRegs::D: {
			return ((uint16_t) cpu->getRegD());
		} break;
		case EBLBRegs::X: {
			return ((uint16_t) cpu->getRegX());
		} break;
		case EBLBRegs::Y: {
			return ((uint16_t) cpu->getRegY());
		} break;
		case EBLBRegs::SP: {
			return ((uint16_t) cpu->getRegSP());
		} break;
		default: break;
	}
	throw std::runtime_error("Internal error");
}

template <class T>
void EBLB::exchange(unsigned int rrSrc, unsigned int rrDst) {
	T tmp = getter<T>(rrSrc);
	setter<T>(rrSrc, getter<T>(rrDst));
	setter<T>(rrDst, tmp);
}

//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================

bool CPU::BeginSetup() {

	/* check verbose settings */
	if(debug_enabled && verbose_all) {
		verbose_setup = true;
		verbose_step = true;
		verbose_dump_regs_start = true;
		verbose_dump_regs_end = true;
	}
	if(debug_enabled && verbose_all) {
		*logger << DebugInfo
			<< "verbose-all = true"
			<< std::endl << EndDebugInfo;
	} else {
		if(debug_enabled && verbose_setup)
			*logger << DebugInfo
				<< "verbose-setup = true"
				<< std::endl << EndDebugInfo;
		if(debug_enabled && verbose_step)
			*logger << DebugInfo
				<< "verbose-step = true"
				<< std::endl << EndDebugInfo;
		if(debug_enabled && verbose_dump_regs_start)
			*logger << DebugInfo
				<< "verbose-dump-regs-end = true"
				<< std::endl << EndDebugInfo;
		if(debug_enabled && verbose_dump_regs_end)
			*logger << DebugInfo
				<< "verbose-dump-regs-start = true"
				<< std::endl << EndDebugInfo;
	}

	/* setting debugging registers */
	if(verbose_setup)
		*logger << DebugInfo
			<< "Initializing debugging registers"
			<< std::endl << EndDebugInfo;

	if (enable_file_trace) {
		trace.open("cpu_trace.txt");
	}

	return (true);
}

bool CPU::Setup(ServiceExportBase *srv_export) {

	if (not memory_access_reporting_import)
	{
		requires_memory_access_reporting = false;
		requires_commit_instruction_reporting = false;
		requires_fetch_instruction_reporting = false;
	}

	return (true);
}

bool CPU::EndSetup() {
	return (true);
}

void CPU::OnDisconnect()
{
}

//=====================================================================
//=             memory access reporting control interface methods     =
//=====================================================================

/** Set/unset the reporting of memory accesses.
 *
 * @param report if true/false sets/unsets the reporting of memory
 *        acesseses
 */
void
CPU::RequiresMemoryAccessReporting( MemoryAccessReportingType type, bool report )
{
	switch (type) {
	case unisim::service::interfaces::REPORT_MEM_ACCESS:  requires_memory_access_reporting = report; break;
	case unisim::service::interfaces::REPORT_FETCH_INSN:  requires_fetch_instruction_reporting = report; break;
	case unisim::service::interfaces::REPORT_COMMIT_INSN: requires_commit_instruction_reporting = report; break;
	default: throw 0;
	}
}

/**************************************************************/
/* Verbose methods (protected)                          START */
/**************************************************************/

inline ALWAYS_INLINE
bool CPU::VerboseSetup() {
	return (debug_enabled && verbose_setup);
}

inline ALWAYS_INLINE
bool CPU::VerboseStep() {
	return (debug_enabled && verbose_step);
}

inline ALWAYS_INLINE
void CPU::VerboseDumpRegs() {

	*logger << "\t- A" << " = 0x" << std::hex << getRegA() << std::dec;
	*logger << "\t- B" << " = 0x" << std::hex << getRegB() << std::dec;
	*logger << "\t- D" << " = 0x" << std::hex << getRegD() << std::dec;
	*logger << std::endl;
	*logger << "\t- X" << " = 0x" << std::hex << getRegX() << std::dec;
	*logger << "\t- Y" << " = 0x" << std::hex << getRegY() << std::dec;
	*logger << std::endl;
	*logger << "\t- SP" << " = 0x" << std::hex << getRegSP() << std::dec;
	*logger << "\t- PC" << " = 0x" << std::hex << getRegPC() << std::dec;
	*logger << std::endl;

}

inline ALWAYS_INLINE
void CPU::VerboseDumpRegsStart() {
	if(debug_enabled && verbose_dump_regs_start) {
		*logger << DebugInfo
			<< "Register dump before starting instruction execution: " << std::endl;
		VerboseDumpRegs();
		*logger << EndDebugInfo;
	}
}

inline ALWAYS_INLINE
void CPU::VerboseDumpRegsEnd() {
	if(debug_enabled && verbose_dump_regs_end) {
		*logger << DebugInfo
			<< "Register dump at the end of instruction execution: " << std::endl;
		VerboseDumpRegs();
		*logger << EndDebugInfo;
	}
}

//=====================================================================
//=             memory interface methods                              =
//=====================================================================

bool CPU::ReadMemory(physical_address_t addr, void *buffer, uint32_t size)
{
	if (memory_import) {
		return (memory_import->ReadMemory(addr, (uint8_t *) buffer, size));
	}

	return (false);
}

bool CPU::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size)
{
	if (size == 0) {
		return (true);
	}

	if (memory_import) {
		return (memory_import->WriteMemory(addr, (uint8_t *) buffer, size));
	}

	return (false);
}

//=====================================================================
//=             CPURegistersInterface interface methods               =
//=====================================================================

string CPU::getObjectFriendlyName(physical_address_t addr)
{
	stringstream sstr;

	const Symbol<physical_address_t> *symbol = NULL;

	if (symbol_table_lookup_import) {
		symbol = symbol_table_lookup_import->FindSymbolByAddr(addr, Symbol<physical_address_t>::SYM_OBJECT);
	}

	if(symbol)
		sstr << symbol->GetFriendlyName(addr);
	else
		sstr << "0x" << std::hex << addr << std::dec;

	return (sstr.str());
}

string CPU::getFunctionFriendlyName(physical_address_t addr)
{
	stringstream sstr;

	const Symbol<physical_address_t> *symbol = NULL;

	if (symbol_table_lookup_import) {
		symbol = symbol_table_lookup_import->FindSymbolByAddr(addr, Symbol<physical_address_t>::SYM_FUNC);
	}

	if(symbol)
		sstr << symbol->GetFriendlyName(addr);
	else
		sstr << "0x" << std::hex << addr << std::dec;

	return (sstr.str());
}

/**
 * Gets a register interface to the register specified by name.
 *
 * @param name The name of the requested register.
 * @return A pointer to the RegisterInterface corresponding to name.
 */
Register* CPU::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

void CPU::ScanRegisters( unisim::service::interfaces::RegisterScanner& scanner )
{
	registers_registry.ScanRegisters(scanner);
}


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
string CPU::Disasm(physical_address_t service_addr, physical_address_t &next_addr)
{
	CodeType insn( CodeType::capacity*8 );
	ReadMemory(service_addr, &insn.str[0], CodeType::capacity);
	Operation* op = this->Decode(service_addr, insn);

	stringstream disasmBuffer;
	op->disasm(disasmBuffer);

	unsigned int insn_length = op->GetLength();
	if (insn_length % 8) throw "InternalError";
	next_addr = service_addr + (insn_length/8);

	return (disasmBuffer.str());
}

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
