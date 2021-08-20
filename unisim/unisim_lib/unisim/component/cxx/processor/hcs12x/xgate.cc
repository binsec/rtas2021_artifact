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

#include <unisim/component/cxx/processor/hcs12x/s12xgate.hh>
#include <unisim/component/cxx/processor/hcs12x/xgate.hh>
#include <unisim/util/inlining/inlining.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace s12xgate {


using unisim::util::debug::SimpleRegister;

XGATE::XGATE(const char *name, Object *parent)
	: Object(name, parent)
	, unisim::kernel::Client<DebugYielding>(name, parent)
	, unisim::kernel::Service<Registers>(name, parent)
	, unisim::kernel::Service<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::Service<MemoryAccessReportingControl>(name, parent)
	, unisim::kernel::Client<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::Client<MemoryAccessReporting<physical_address_t> >(name, parent)
	, unisim::kernel::Client<SymbolTableLookup<physical_address_t> >(name, parent)
	, unisim::kernel::Client<TrapReporting>(name, parent)

	, registers_export("registers_export", this)
	, memory_export("memory_export", this)
	, memory_access_reporting_control_export("memory_access_reporting_control_export", this)
	, debug_yielding_import("debug_yielding_import", this)
	, memory_access_reporting_import("memory_access_reporting_import", this)
	, memory_import("memory_import", this)
	, trap_reporting_import("trap_reporting_import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import",  this)
	, logger(0)

	, cpu_cycle_time()
	, voltage()
	, bus_cycle_time()
	, cpu_cycle()
	, bus_cycle()

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

	, requires_memory_access_reporting(false)
	, requires_fetch_instruction_reporting(false)
	, requires_commit_instruction_reporting(false)

	, enable_trace(false)
	, param_enable_trace("enable-trace", this, enable_trace)

	, enable_file_trace(false)
	, param_enable_file_trace("enable-file-trace", this, enable_file_trace)

	, debug_enabled(false)
	, param_debug_enabled("debug-enabled", this, debug_enabled, "")

	, periodic_trap(-1)
	, param_periodic_trap("periodic-trap", this, periodic_trap)

	, version("V2")
	, param_version("version", this, version, "XGATE version. Supported are V2 and V3. Default is V2")

	, baseAddress(0x0380)
	, param_baseAddress("base-address", this, baseAddress, "XGATE base Address")

	, param_software_channel_id("software_channel_id", this, sofwtare_channel_id, XGATE_SIZE, "XGATE channel ID associated to software trigger. Determined on chip integration level (see Interrupts section of the SoC Guide.")

	, xgate_enabled(false)
	, stop_current_thread(false)
	, state(IDLE)
	, mode(NORMAL)

	, currentThreadTerminated(true)

	, currentRegisterBank(&registerBank[0])
	, isPendingThread(false)
	, pendingThreadRegisterBank(0)

	, interrupt_software_error(0x62)
	, param_interrupt_software_error("software-error-interrupt", this, interrupt_software_error, "XGATE Software error interrupt")

	, max_inst((uint64_t) -1)
	, param_max_inst("max-inst", this, max_inst)

	, instruction_counter(0)
	, stat_instruction_counter("instruction-counter", this, instruction_counter)
	, cycles_counter(0)
	, stat_cycles_counter("cycles-counter", this, cycles_counter)
	, data_load_counter(0)
	, stat_load_counter("data-load-counter", this, data_load_counter)
	, data_store_counter(0)
	, stat_store_counter("data-store-counter", this, data_store_counter)

	, xgmctl_register(0x0000)
	, xgswt_register(0x0000)
	, xgsem_register(0x0000)
	, xgispsel_register(0x00)



{
	param_max_inst.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_periodic_trap.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

	stat_instruction_counter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	stat_instruction_counter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	stat_cycles_counter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	stat_load_counter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	stat_store_counter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

	for (unsigned int i=0; i<XGATE_SIZE; i++) {
		param_software_channel_id.SetFormat(unisim::kernel::VariableBase::FMT_HEX);
		param_software_channel_id.SetMutable(true);
	}

	logger = new unisim::kernel::logger::Logger(*this);

	xgvbrPtr_register[0] = 0x0000;
	xgvbrPtr_register[1] = 0x0000;
	xgvbrPtr_register[2] = 0x0000;
	xgvbrPtr_register[3] = 0x0000;
	xgif_register[0] = 0x0000;
	xgif_register[1] = 0x0000;
	xgif_register[2] = 0x0000;
	xgif_register[3] = 0x0000;
	xgif_register[4] = 0x0000;
	xgif_register[5] = 0x0000;
	xgif_register[6] = 0x0000;
	xgif_register[7] = 0x0000;

	lastPC = 0;
}

XGATE::~XGATE()
{
	// Release registers_registry
	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

	if (logger) { delete logger; logger = NULL;}

	if (trace.is_open()) { trace.close(); }

}

/**************************************************************/
/* Verbose methods (protected)                          START */
/**************************************************************/

inline ALWAYS_INLINE
bool XGATE::VerboseSetup() {
	return (debug_enabled && verbose_setup);
}

inline ALWAYS_INLINE
bool XGATE::VerboseStep() {
	return (debug_enabled && verbose_step);
}

inline ALWAYS_INLINE
void XGATE::VerboseDumpRegs() {

	*logger << "\t- XGMCTL" << " = 0x" << std::hex << xgmctl_register << std::dec; //2-bytes
	*logger << "\t- XGCHID" << " = 0x" << std::hex << (unsigned int) currentRegisterBank->getXGCHID() << std::dec; // 1-byte
	*logger << "\t- XGVBR" << " = 0x" << std::hex << xgvbrPtr_register[0] << std::dec; // 2-bytes
	*logger << "\t- XGISP74" << " = 0x" << std::hex << xgvbrPtr_register[1] << std::dec; // 2-bytes
	*logger << "\t- XGISP31" << " = 0x" << std::hex << xgvbrPtr_register[2] << std::dec; // 2-bytes
	*logger << "\t- XGIF_7F_70" << " = 0x" << std::hex << xgif_register[0] << std::dec; // 2-bytes
	*logger << "\t- XGIF_6F_60" << " = 0x" << std::hex << xgif_register[1] << std::dec; // 2-bytes
	*logger << "\t- XGIF_5F_50" << " = 0x" << std::hex << xgif_register[2] << std::dec; // 2-bytes
	*logger << "\t- XGIF_4F_40" << " = 0x" << std::hex << xgif_register[3] << std::dec; // 2-bytes
	*logger << "\t- XGIF_3F_30" << " = 0x" << std::hex << xgif_register[4] << std::dec; // 2-bytes
	*logger << "\t- XGIF_2F_20" << " = 0x" << std::hex << xgif_register[5] << std::dec; // 2-bytes
	*logger << "\t- XGIF_1F_10" << " = 0x" << std::hex << xgif_register[6] << std::dec; // 2-bytes
	*logger << "\t- XGIF_0F_00" << " = 0x" << std::hex << xgif_register[7] << std::dec; // 2-bytes
	*logger << "\t- XGSWT" << " = 0x" << std::hex << xgswt_register << std::dec; // 2-bytes
	*logger << "\t- XGSEMM" << " = 0x" << std::hex << xgsem_register << std::dec; // 2-bytes
	*logger << "\t- XGCCR" << " = 0x" << std::hex << (unsigned int) currentRegisterBank->getXGCCR() << std::dec; // 1-byte
	*logger << "\t- XGPC" << " = 0x" << std::hex << currentRegisterBank->getXGPC() << std::dec; // 2-bytes
	*logger << "\t- XGR1" << " = 0x" << std::hex << currentRegisterBank->getXGRx(1) << std::dec; // 2-bytes
	*logger << "\t- XGR2" << " = 0x" << std::hex << currentRegisterBank->getXGRx(2) << std::dec; // 2-bytes
	*logger << "\t- XGR3" << " = 0x" << std::hex << currentRegisterBank->getXGRx(3) << std::dec; // 2-bytes
	*logger << "\t- XGR4" << " = 0x" << std::hex << currentRegisterBank->getXGRx(4) << std::dec; // 2-bytes
	*logger << "\t- XGR5" << " = 0x" << std::hex << currentRegisterBank->getXGRx(5) << std::dec; // 2-bytes
	*logger << "\t- XGR6" << " = 0x" << std::hex << currentRegisterBank->getXGRx(6) << std::dec; // 2-bytes
	*logger << "\t- XGR7" << " = 0x" << std::hex << currentRegisterBank->getXGRx(7) << std::dec; // 2-bytes
	*logger << std::endl;

}

inline ALWAYS_INLINE
void XGATE::VerboseDumpRegsStart() {
	if(debug_enabled && verbose_dump_regs_start) {
		*logger << DebugInfo
			<< "Register dump before starting instruction execution: " << std::endl;
		VerboseDumpRegs();
		*logger << EndDebugInfo;
	}
}

inline ALWAYS_INLINE
void XGATE::VerboseDumpRegsEnd() {
	if(debug_enabled && verbose_dump_regs_end) {
		*logger << DebugInfo
			<< "Register dump at the end of instruction execution: " << std::endl;
		VerboseDumpRegs();
		*logger << EndDebugInfo;
	}
}

bool XGATE::BeginSetup() {
	std::stringstream sstr_xgmctl_register_name;
	sstr_xgmctl_register_name << GetName() << ".XGMCTL";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgmctl_register_name.str().c_str(), &xgmctl_register));

	unisim::kernel::variable::Register<uint16_t> *xgmctl_var = new unisim::kernel::variable::Register<uint16_t>("XGMCTL", this, xgmctl_register, "XGATE Control Register (XGMCTL)");
	extended_registers_registry.push_back(xgmctl_var);
	xgmctl_var->setCallBack(this, XGMCTL, &CallBackObject::write, NULL);

	std::stringstream sstr_xgchid_register_name;
	sstr_xgchid_register_name << GetName() << ".XGCHID";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(sstr_xgchid_register_name.str().c_str(), currentRegisterBank->getXGCHIDPtr()));

	unisim::kernel::variable::Register<uint8_t> *xgchid_var = new unisim::kernel::variable::Register<uint8_t>("XGCHID", this, *(currentRegisterBank->getXGCHIDPtr()), "XGATE Channel ID Register (XGCHID)");
	extended_registers_registry.push_back(xgchid_var);
	xgchid_var->setCallBack(this, XGCHID, &CallBackObject::write, &CallBackObject::read);

	if (version.compare("V2") != 0) {
		std::stringstream sstr_xgchpl_register_name;
		sstr_xgchpl_register_name << GetName() << ".XGCHPL";
		registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(sstr_xgchpl_register_name.str().c_str(), currentRegisterBank->getXGCHPLPtr()));

		unisim::kernel::variable::Register<uint8_t> *xgchpl_var = new unisim::kernel::variable::Register<uint8_t>("XGCHPL", this, *(currentRegisterBank->getXGCHPLPtr()), "XGATE Channel Priority Level Register (XGCHPL)");
		extended_registers_registry.push_back(xgchpl_var);
		xgchpl_var->setCallBack(this, XGCHPL, &CallBackObject::write, &CallBackObject::read);

		std::stringstream sstr_xgispsel_register_name;
		sstr_xgispsel_register_name << GetName() << ".XGISPSEL";
		registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(sstr_xgispsel_register_name.str().c_str(), &xgispsel_register));

		unisim::kernel::variable::Register<uint8_t> *xgispsel_var = new unisim::kernel::variable::Register<uint8_t>("XGISPSEL", this, xgispsel_register, "XGATE Initial Stack Pointer Select Register (XGISPSEL)");
		extended_registers_registry.push_back(xgispsel_var);
		xgispsel_var->setCallBack(this, XGISPSEL, &CallBackObject::write, NULL);

		std::stringstream sstr_xgisp74_register_name;
		sstr_xgisp74_register_name << GetName() << ".XGISP74";
		registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgisp74_register_name.str().c_str(), &xgvbrPtr_register[1]));

		unisim::kernel::variable::Register<uint16_t> *xgisp74_var = new unisim::kernel::variable::Register<uint16_t>("XGISP74", this, xgvbrPtr_register[1], "XGATE Initial Stack Pointer for Interrupt Priorities 7 to 4 (XGISP74)");
		extended_registers_registry.push_back(xgisp74_var);
		xgisp74_var->setCallBack(this, XGVBR, &CallBackObject::write, NULL);

		std::stringstream sstr_xgisp31_register_name;
		sstr_xgisp31_register_name << GetName() << ".XGISP31";
		registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgisp31_register_name.str().c_str(), &xgvbrPtr_register[2]));

		unisim::kernel::variable::Register<uint16_t> *xgisp31_var = new unisim::kernel::variable::Register<uint16_t>("XGISP31", this, xgvbrPtr_register[2], "XGATE Initial Stack Pointer for Interrupt Priorities 3 to 1 (XGISP31)");
		extended_registers_registry.push_back(xgisp31_var);
		xgisp31_var->setCallBack(this, XGVBR, &CallBackObject::write, NULL);

	}

	std::stringstream sstr_xgvbr_register_name;
	sstr_xgvbr_register_name << GetName() << ".XGVBR";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgvbr_register_name.str().c_str(), &xgvbrPtr_register[0]));

	unisim::kernel::variable::Register<uint16_t> *xgvbr_var = new unisim::kernel::variable::Register<uint16_t>("XGVBR", this, xgvbrPtr_register[0], "XGATE Vector Base Address Register (XGVBR)");
	extended_registers_registry.push_back(xgvbr_var);
	xgvbr_var->setCallBack(this, XGVBR, &CallBackObject::write, NULL);

	std::stringstream sstr_xgif_7f_70_register_name;
	sstr_xgif_7f_70_register_name << GetName() << ".XGIF_7F_70";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgif_7f_70_register_name.str().c_str(), &xgif_register[0]));

	unisim::kernel::variable::Register<uint16_t> *xgif_7f_70_var = new unisim::kernel::variable::Register<uint16_t>("XGIF_7F_70", this, xgif_register[0], "XGATE Channel (7F-70) Interrupt Flag Register (XGIF_7F_70)");
	extended_registers_registry.push_back(xgif_7f_70_var);
	xgif_7f_70_var->setCallBack(this, XGIF_7F_70, &CallBackObject::write, NULL);

	std::stringstream sstr_xgif_6f_60_register_name;
	sstr_xgif_6f_60_register_name << GetName() << ".XGIF_6F_60";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgif_6f_60_register_name.str().c_str(), &xgif_register[1]));

	unisim::kernel::variable::Register<uint16_t> *xgif_6f_60_var = new unisim::kernel::variable::Register<uint16_t>("XGIF_6F_60", this, xgif_register[1], "XGATE Channel (6F-60) Interrupt Flag Register (XGIF_6F_60)");
	extended_registers_registry.push_back(xgif_6f_60_var);
	xgif_6f_60_var->setCallBack(this, XGIF_6F_60, &CallBackObject::write, NULL);

	std::stringstream sstr_xgif_5f_50_register_name;
	sstr_xgif_5f_50_register_name << GetName() << ".XGIF_5F_50";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgif_5f_50_register_name.str().c_str(), &xgif_register[2]));

	unisim::kernel::variable::Register<uint16_t> *xgif_5f_50_var = new unisim::kernel::variable::Register<uint16_t>("XGIF_5F_50", this, xgif_register[2], "XGATE Channel (5F-60) Interrupt Flag Register (XGIF_5F_50)");
	extended_registers_registry.push_back(xgif_5f_50_var);
	xgif_5f_50_var->setCallBack(this, XGIF_5F_50, &CallBackObject::write, NULL);

	std::stringstream sstr_xgif_4f_40_register_name;
	sstr_xgif_4f_40_register_name << GetName() << ".XGIF_4F_40";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgif_4f_40_register_name.str().c_str(), &xgif_register[3]));

	unisim::kernel::variable::Register<uint16_t> *xgif_4F_40_var = new unisim::kernel::variable::Register<uint16_t>("XGIF_4F_40", this, xgif_register[3], "XGATE Channel (4F_40) Register (XGIF_4F_40)");
	extended_registers_registry.push_back(xgif_4F_40_var);
	xgif_4F_40_var->setCallBack(this, XGIF_4F_40, &CallBackObject::write, NULL);

	std::stringstream sstr_xgif_3f_30_register_name;
	sstr_xgif_3f_30_register_name << GetName() << ".XGIF_3F_30";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgif_3f_30_register_name.str().c_str(), &xgif_register[4]));

	unisim::kernel::variable::Register<uint16_t> *xgif_3f_30_var = new unisim::kernel::variable::Register<uint16_t>("XGIF_3F_30", this, xgif_register[4], "XGATE channel (3F-30) Register (XGIF_3F_30)");
	extended_registers_registry.push_back(xgif_3f_30_var);
	xgif_3f_30_var->setCallBack(this, XGIF_3F_30, &CallBackObject::write, NULL);

	std::stringstream sstr_xgif_2f_20_register_name;
	sstr_xgif_2f_20_register_name << GetName() << ".XGIF_2F_20";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgif_2f_20_register_name.str().c_str(), &xgif_register[5]));

	unisim::kernel::variable::Register<uint16_t> *xgif_2f_20_var = new unisim::kernel::variable::Register<uint16_t>("XGIF_2F_20", this, xgif_register[5], "XGATE Channel (2F-20) Register (XGIF_2F_20)");
	extended_registers_registry.push_back(xgif_2f_20_var);
	xgif_2f_20_var->setCallBack(this, XGIF_2F_20, &CallBackObject::write, NULL);

	std::stringstream sstr_xgif_1f_10_register_name;
	sstr_xgif_1f_10_register_name << GetName() << ".XGIF_1F_10";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgif_1f_10_register_name.str().c_str(), &xgif_register[6]));

	unisim::kernel::variable::Register<uint16_t> *xgif_1f_10_var = new unisim::kernel::variable::Register<uint16_t>("XGIF_1F_10", this, xgif_register[6], "XGATE Channel (1F-10) Register (XGIF_1F_10)");
	extended_registers_registry.push_back(xgif_1f_10_var);
	xgif_1f_10_var->setCallBack(this, XGIF_1F_10, &CallBackObject::write, NULL);

	std::stringstream sstr_xgif_0f_00_register_name;
	sstr_xgif_0f_00_register_name << GetName() << ".XGIF_0F_00";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgif_0f_00_register_name.str().c_str(), &xgif_register[7]));

	unisim::kernel::variable::Register<uint16_t> *xgif_0f_00_var = new unisim::kernel::variable::Register<uint16_t>("XGIF_0F_00", this, xgif_register[7], "XGATE Channel (0F-00) Register (XGIF_0F_00)");
	extended_registers_registry.push_back(xgif_0f_00_var);
	xgif_0f_00_var->setCallBack(this, XGIF_0F_00, &CallBackObject::write, NULL);

	std::stringstream sstr_xgswt_register_name;
	sstr_xgswt_register_name << GetName() << ".XGSWT";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgswt_register_name.str().c_str(), &xgswt_register));

	unisim::kernel::variable::Register<uint16_t> *xgswt_var = new unisim::kernel::variable::Register<uint16_t>("XGSWT", this, xgswt_register, "XGATE Software Trigger Register (XGSWT)");
	extended_registers_registry.push_back(xgswt_var);
	xgswt_var->setCallBack(this, XGSWT, &CallBackObject::write, NULL);

	std::stringstream sstr_xgsemm_register_name;
	sstr_xgsemm_register_name << GetName() << ".XGSEMM";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgsemm_register_name.str().c_str(), &xgsem_register));

	unisim::kernel::variable::Register<uint16_t> *xgsemm_var = new unisim::kernel::variable::Register<uint16_t>("XGSEMM", this, xgsem_register, "XGATE Semaphore Register (XGSEMM)");
	extended_registers_registry.push_back(xgsemm_var);
	xgsemm_var->setCallBack(this, XGSEM, &CallBackObject::write, NULL);

	std::stringstream sstr_xgccr_register_name;
	sstr_xgccr_register_name << GetName() << ".XGCCR";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(sstr_xgccr_register_name.str().c_str(), currentRegisterBank->getXGCCRPtr()));

	unisim::kernel::variable::Register<uint8_t> *xgccr_var = new unisim::kernel::variable::Register<uint8_t>("XGCCR", this, *(currentRegisterBank->getXGCCRPtr()), "XGATE Condition Code Register (XGCCR)");
	extended_registers_registry.push_back(xgccr_var);
	xgccr_var->setCallBack(this, XGCCR, &CallBackObject::write, &CallBackObject::read);

	std::stringstream sstr_xgpc_register_name;
	sstr_xgpc_register_name << GetName() << ".XGPC";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgpc_register_name.str().c_str(), currentRegisterBank->getXGPCPtr()));

	unisim::kernel::variable::Register<uint16_t> *xgpc_var = new unisim::kernel::variable::Register<uint16_t>("XGPC", this, *(currentRegisterBank->getXGPCPtr()), "XGATE Program Counter Register (XGPC)");
	extended_registers_registry.push_back(xgpc_var);
	xgpc_var->setCallBack(this, XGPC, &CallBackObject::write, &CallBackObject::read);

	std::stringstream sstr_xgr1_register_name;
	sstr_xgr1_register_name << GetName() << ".XGR1";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgr1_register_name.str().c_str(), currentRegisterBank->getXGRxPtr(1)));

	unisim::kernel::variable::Register<uint16_t> *xgr1_var = new unisim::kernel::variable::Register<uint16_t>("XGR1", this, *(currentRegisterBank->getXGRxPtr(1)), "XGATE Register 1 (XGR1)");
	extended_registers_registry.push_back(xgr1_var);
	xgr1_var->setCallBack(this, XGR1, &CallBackObject::write, &CallBackObject::read);

	std::stringstream sstr_xgr2_register_name;
	sstr_xgr2_register_name << GetName() << ".XGR2";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgr2_register_name.str().c_str(), currentRegisterBank->getXGRxPtr(2)));

	unisim::kernel::variable::Register<uint16_t> *xgr2_var = new unisim::kernel::variable::Register<uint16_t>("XGR2", this, *(currentRegisterBank->getXGRxPtr(2)), "XGATE Register 2 (XGR2)");
	extended_registers_registry.push_back(xgr2_var);
	xgr2_var->setCallBack(this, XGR2, &CallBackObject::write, &CallBackObject::read);

	std::stringstream sstr_xgr3_register_name;
	sstr_xgr3_register_name << GetName() << ".XGR3";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgr3_register_name.str().c_str(), currentRegisterBank->getXGRxPtr(3)));

	unisim::kernel::variable::Register<uint16_t> *xgr3_var = new unisim::kernel::variable::Register<uint16_t>("XGR3", this, *(currentRegisterBank->getXGRxPtr(3)), "XGATE Register 3 (XGR3)");
	extended_registers_registry.push_back(xgr3_var);
	xgr3_var->setCallBack(this, XGR3, &CallBackObject::write, &CallBackObject::read);

	std::stringstream sstr_xgr4_register_name;
	sstr_xgr4_register_name << GetName() << ".XGR4";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgr4_register_name.str().c_str(), currentRegisterBank->getXGRxPtr(4)));

	unisim::kernel::variable::Register<uint16_t> *xgr4_var = new unisim::kernel::variable::Register<uint16_t>("XGR4", this, *(currentRegisterBank->getXGRxPtr(4)), "XGATE Register 4 (XGR4)");
	extended_registers_registry.push_back(xgr4_var);
	xgr4_var->setCallBack(this, XGR4, &CallBackObject::write, &CallBackObject::read);

	std::stringstream sstr_xgr5_register_name;
	sstr_xgr5_register_name << GetName() << ".XGR5";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgr5_register_name.str().c_str(), currentRegisterBank->getXGRxPtr(5)));

	unisim::kernel::variable::Register<uint16_t> *xgr5_var = new unisim::kernel::variable::Register<uint16_t>("XGR5", this, *(currentRegisterBank->getXGRxPtr(5)), "XGATE Register 5 (XGR5)");
	extended_registers_registry.push_back(xgr5_var);
	xgr5_var->setCallBack(this, XGR5, &CallBackObject::write, &CallBackObject::read);

	std::stringstream sstr_xgr6_register_name;
	sstr_xgr6_register_name << GetName() << ".XGR6";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgr6_register_name.str().c_str(), currentRegisterBank->getXGRxPtr(6)));

	unisim::kernel::variable::Register<uint16_t> *xgr6_var = new unisim::kernel::variable::Register<uint16_t>("XGR6", this, *(currentRegisterBank->getXGRxPtr(6)), "XGATE Register 6 (XGR6)");
	extended_registers_registry.push_back(xgr6_var);
	xgr6_var->setCallBack(this, XGR6, &CallBackObject::write, &CallBackObject::read);

	std::stringstream sstr_xgr7_register_name;
	sstr_xgr7_register_name << GetName() << ".XGR7";
	registers_registry.AddRegisterInterface(new SimpleRegister<uint16_t>(sstr_xgr7_register_name.str().c_str(), currentRegisterBank->getXGRxPtr(7)));

	unisim::kernel::variable::Register<uint16_t> *xgr7_var = new unisim::kernel::variable::Register<uint16_t>("XGR7", this, *(currentRegisterBank->getXGRxPtr(7)), "XGATE Register 7 (XGR7)");
	extended_registers_registry.push_back(xgr7_var);
	xgr7_var->setCallBack(this, XGR7, &CallBackObject::write, &CallBackObject::read);

	if (enable_file_trace) {
		trace.open("xgate_trace.txt");
	}

	Reset();

	return (true);
}

bool XGATE::Setup(ServiceExportBase *srv_export) {

	if (not memory_access_reporting_import) {
		requires_memory_access_reporting = false;
		requires_fetch_instruction_reporting = false;
		requires_commit_instruction_reporting = false;
	}

	return (true);

}

bool XGATE::EndSetup() {

	return (true);
}

void XGATE::OnDisconnect() {

}

void XGATE::Reset() {

	currentRegisterBank = &registerBank[0];

	xgmctl_register = 0x0000;
	xgvbrPtr_register[0] = 0x0000;
	xgvbrPtr_register[1] = 0x0000;
	xgvbrPtr_register[2] = 0x0000;
	xgvbrPtr_register[3] = 0x0000;
	xgif_register[0] = 0x0000;
	xgif_register[1] = 0x0000;
	xgif_register[2] = 0x0000;
	xgif_register[3] = 0x0000;
	xgif_register[4] = 0x0000;
	xgif_register[5] = 0x0000;
	xgif_register[6] = 0x0000;
	xgif_register[7] = 0x0000;
	xgswt_register = 0x0000;
	xgsem_register = 0x0000;

	registerBank[0].Reset();
	registerBank[1].Reset();

}

void XGATE::ResetMemory() {

	Reset();

}

void XGATE::Stop(int ret)
{
	exit(ret);
}

void XGATE::Sync()
{

}

Decoder decoder;

unsigned int XGATE::step()
{
	try
	{
		address_t curPC = getXGPC();

		if (verbose_step)
			*logger << DebugInfo << "Starting step at PC = 0x" << std::hex << curPC << std::dec << std::endl << EndDebugInfo;

		if (requires_fetch_instruction_reporting and memory_access_reporting_import)
			memory_access_reporting_import->ReportFetchInstruction(MMC::getInstance()->getXGATEPagedAddress(curPC));

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

		CodeType 	insn;
		fetchInstruction(curPC, &insn.str[0], CodeType::capacity);

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

		Operation* op = decoder.Decode(MMC::getInstance()->getXGATEPagedAddress(curPC), insn);
		unsigned int insn_length = op->GetLength();
		if (insn_length % 8) throw "InternalError";

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
		setXGPC(curPC + (insn_length/8));

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

		op->execute(this);

		unsigned opCycles = op->getCycles();

		cycles_counter += opCycles;

		VerboseDumpRegsEnd();

		instruction_counter++;

		if ((trap_reporting_import) && ((instruction_counter % periodic_trap) == 0)) {
			trap_reporting_import->ReportTrap();
		}

		if (requires_commit_instruction_reporting and memory_access_reporting_import)
			memory_access_reporting_import->ReportCommitInstruction(MMC::getInstance()->getXGATEPagedAddress(curPC), (insn_length/8));

		if (instruction_counter >= max_inst)
			Stop(0);
		
		return opCycles;
	}

	catch (Exception& e)
	{
		if(debug_enabled && verbose_step)
			*logger << DebugError << "uncaught processor exception :" << e.what() << std::endl << EndDebugError;
		Stop(1);
	}

	return 0;

}

void XGATE::fetchInstruction(address_t addr, uint8_t* ins, unsigned int nByte)
{

	MMC_DATA mmc_data;

	mmc_data.logicalAddress = addr;
	mmc_data.type = ADDRESS::EXTENDED;
	mmc_data.isGlobal = false;
	mmc_data.buffer = ins;
	mmc_data.data_size = nByte;

	busRead(&mmc_data);

}

void XGATE::
terminateCurrentThread() {

	if (isPendingThread) {
		currentRegisterBank = pendingThreadRegisterBank;
	} else {
		currentThreadTerminated = true;
		state = IDLE;
		setXGCHID(0);
		setXGCHPL(0);
	}

}

void XGATE::riseErrorCondition() {
	/**
	 * XGMCTL::XGSWEIF
	 *  XGATE Software Error Interrupt Flag — This bit signals a pending Software Error Interrupt.
	 *  It is set if the RISC core detects an error condition (see Section 6.4.5, “Software Error Detection”).
	 *  The RISC core is stopped while this bit is set.
	 *  Clearing this bit will terminate the current thread and cause the XGATE to become idle.
	 */

	xgmctl_register = xgmctl_register | 0x0002;
	state = STOP;
}

bool XGATE::lockSemaphore(TOWNER::OWNER owner, unsigned int index) {
	if (semphore[index].lock(owner)) {
		if (owner == TOWNER::CPU12X) {
			xgsem_register = xgsem_register | (1 << index);
		} else {
			xgsem_register = xgsem_register & ~(1 << index);
		}

		return (true);
	}

	return (false);
}

bool XGATE::unlockSemaphore(TOWNER::OWNER owner, unsigned int index) {
	if (semphore[index].unlock(owner)) {
		xgsem_register = xgsem_register & ~(1 << index);
		return (true);
	}

	return (false);
}

//=====================================================================
//=             memory access reporting control interface methods     =
//=====================================================================

void XGATE::RequiresMemoryAccessReporting( MemoryAccessReportingType type, bool report )
{
	switch (type) {
	case unisim::service::interfaces::REPORT_MEM_ACCESS:  requires_memory_access_reporting = report; break;
	case unisim::service::interfaces::REPORT_FETCH_INSN:  requires_fetch_instruction_reporting = report; break;
	case unisim::service::interfaces::REPORT_COMMIT_INSN: requires_commit_instruction_reporting = report; break;
	default: throw 0;
	}
}

bool XGATE::ReadMemory(physical_address_t addr, void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress + MEMORY_MAP_SIZE))) {

		if (size == 0) {
			return (true);
		}

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
			case XGMCTL: {
				if (size == 2) {
					*((uint16_t *) buffer) = xgmctl_register;
				} else {
					*((uint8_t *)buffer) = xgmctl_register >> 8;
				}

			} break;
			case XGMCTL+1: {
				*((uint8_t *)buffer) = xgmctl_register & 0x00FF;
			} break;

			case XGCHID: *((uint8_t *) buffer) = getXGCHID(); break;
			case XGCHPL: *((uint8_t *) buffer) = getXGCHPL(); break;
			case RESERVED2: *((uint16_t *) buffer) = 0; break;
			case XGISPSEL: *((uint8_t *) buffer) = getXGISPSEL(); break;
			case XGVBR: {
				if (size == 2) {
					*((uint16_t *) buffer) = getXGVBR();
				} else {
					*((uint8_t *) buffer) = (uint8_t) (getXGVBR() >> 8);
				}
			} break;
			case XGVBR+1: {
				*((uint8_t *) buffer) = (uint8_t) getXGVBR();
			} break;
			case XGSWT: {
				if (size == 2) {
					*((uint16_t *) buffer) = xgswt_register;
				} else {
					*((uint8_t *) buffer) = (uint8_t) (xgswt_register >> 8);
				}
			} break;
			case XGSWT+1: {
				*((uint8_t *) buffer) = (uint8_t) xgswt_register;
			} break;
			case XGSEM: {
				if (size == 2) {
					*((uint16_t *) buffer) = xgsem_register;
				} else {
					*((uint8_t *) buffer) = (uint8_t) (xgsem_register >> 8);
				}
			} break;
			case XGSEM+1: {
				*((uint8_t *) buffer) = (uint8_t) xgsem_register;
			} break;
			case RESERVED4: *((uint16_t *) buffer) = 0; break;
			case XGCCR: *((uint8_t *) buffer) = getXGCCR(); break;
			case XGPC: {
				if (size == 2) {
					*((uint16_t *) buffer) = getXGPC();
				} else {
					*((uint8_t *) buffer) = (uint8_t) (getXGPC() >> 8);
				}
			} break;
			case XGPC+1: {
				*((uint8_t *) buffer) = (uint8_t) getXGPC();
			} break;
			case RESERVED5: *((uint16_t *) buffer) = 0; break;
			case RESERVED6: *((uint16_t *) buffer) = 0; break;
			case XGR1: {
				if (size == 2) {
					*((uint16_t *) buffer) = getXGRx(1);
				} else {
					*((uint8_t *) buffer) = (uint8_t) (getXGRx(1) >> 8);
				}
			} break;
			case XGR1+1: {
				*((uint8_t *) buffer) = (uint8_t) getXGRx(1);
			} break;
			case XGR2: {
				if (size == 2) {
					*((uint16_t *) buffer) = getXGRx(2);
				} else {
					*((uint8_t *) buffer) = (uint8_t) (getXGRx(2) >> 8);
				}
			} break;
			case XGR2+1: {
				*((uint8_t *) buffer) = (uint8_t) getXGRx(2);
			} break;
			case XGR3: {
				if (size == 2) {
					*((uint16_t *) buffer) = getXGRx(3);
				} else {
					*((uint8_t *) buffer) = (uint8_t) (getXGRx(3) >> 8);
				}
			} break;
			case XGR3+1: {
				*((uint8_t *) buffer) = (uint8_t) getXGRx(3);
			} break;
			case XGR4: {
				if (size == 2) {
					*((uint16_t *) buffer) = getXGRx(4);
				} else {
					*((uint8_t *) buffer) = (uint8_t) (getXGRx(4) >> 8);
				}
			} break;
			case XGR4+1: {
				*((uint8_t *) buffer) = (uint8_t) getXGRx(4);
			} break;
			case XGR5: {
				if (size == 2) {
					*((uint16_t *) buffer) = getXGRx(5);
				} else {
					*((uint8_t *) buffer) = (uint8_t) (getXGRx(5) >> 8);
				}
			} break;
			case XGR5+1: {
				*((uint8_t *) buffer) = (uint8_t) getXGRx(5);
			} break;
			case XGR6: {
				if (size == 2) {
					*((uint16_t *) buffer) = getXGRx(6);
				} else {
					*((uint16_t *) buffer) = (uint8_t) (getXGRx(6) >> 8);
				}
			} break;
			case XGR6+1: {
				*((uint8_t *) buffer) = (uint8_t) getXGRx(6);
			} break;
			case XGR7: {
				if (size == 2) {
					*((uint16_t *) buffer) = getXGRx(7);
				} else {
					*((uint8_t *) buffer) = (uint8_t) (getXGRx(7) >> 8);
				}
			} break;
			case XGR7+1: {
				*((uint8_t *) buffer) = (uint8_t) getXGRx(7);
			} break;

			default: {
				if ((offset >= XGIF_7F_70) && (offset <= (XGIF_0F_00 + 1))) {
					if (size == 2) {
						*((uint16_t *) buffer) = xgif_register[(offset - XGIF_7F_70)/2];
					} else {
						if (((offset - XGIF_7F_70) % 2) == 0) {
							*((uint8_t *) buffer) = (uint8_t) (xgif_register[(offset - XGIF_7F_70)/2] >> 8);
						} else {
							*((uint8_t *) buffer) = (uint8_t) xgif_register[(offset - XGIF_7F_70)/2];
						}
					}

				} else {
					return (false);
				}
				break;
			}
		}

		return (true);
	}

	return (false);
}

bool XGATE::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress + MEMORY_MAP_SIZE))) {

		if (size == 0) {
			return (true);
		}

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
			case XGMCTL: {
				if (size == 2) {
					uint16_t val = *((uint16_t *) buffer);
					xgmctl_register = val;
				} else {
					xgmctl_register = (xgmctl_register & 0x00FF) | (((uint16_t) *((uint8_t *)buffer)) << 8);
				}

			} break;
			case XGMCTL+1: {
				xgmctl_register = (xgmctl_register & 0xFF00) | ((uint16_t) *((uint8_t *)buffer));
			} break;
			case XGCHID: {
				uint8_t val = *((uint8_t *) buffer);
				setXGCHID(val);
			} break;
			case XGCHPL: {
				uint8_t val = *((uint8_t *) buffer);
				setXGCHPL(val);
			} break;
			case RESERVED2: break;
			case XGISPSEL: {
				uint8_t val = *((uint8_t *) buffer);
				setXGISPSEL(val);
			} break;
			case XGVBR: {
				if (size == 2) {
					uint16_t val = *((uint16_t *) buffer);
					setXGVBR(val);
				} else {
					setXGVBR((getXGVBR() & 0x00FF) | (((uint16_t) *((uint8_t *)buffer)) << 8));
				}
			} break;
			case XGVBR+1: {
				setXGVBR((getXGVBR() & 0xFF00) | ((uint16_t) *((uint8_t *)buffer)));
			} break;
			case XGSWT: {
				if (size == 2) {
					uint16_t val = *((uint16_t *) buffer);
					xgswt_register = val;
				} else {
					xgswt_register = (xgswt_register & 0x00FF) | (((uint16_t) *((uint8_t *)buffer)) << 8);
				}
			} break;
			case XGSWT+1: {
				xgswt_register = (xgswt_register & 0xFF00) | ((uint16_t) *((uint8_t *)buffer));
			} break;
			case XGSEM: {
				if (size == 2) {
					uint16_t val = *((uint16_t *) buffer);
					xgsem_register = val;
				} else {
					xgsem_register = (xgsem_register & 0x00FF) | (((uint16_t) *((uint8_t *)buffer)) << 8);
				}
			} break;
			case XGSEM+1: {
				xgsem_register = (xgsem_register & 0xFF00) | ((uint16_t) *((uint8_t *)buffer));
			} break;
			case RESERVED4: break;
			case XGCCR: {
				uint8_t val = *((uint8_t *) buffer);
				currentRegisterBank->setXGCCR(val);
			} break;
			case XGPC: {
				if (size == 2) {
					uint16_t val = *((uint16_t *) buffer);
					currentRegisterBank->setXGPC(val);
				} else {
					currentRegisterBank->setXGPC((currentRegisterBank->getXGPC() & 0x00FF) | (((uint16_t) *((uint8_t *)buffer)) << 8));
				}
			} break;
			case XGPC+1: {
				currentRegisterBank->setXGPC((currentRegisterBank->getXGPC() & 0xFF00) | ((uint16_t) *((uint8_t *)buffer)));
			} break;
			case RESERVED5: break;
			case RESERVED6: break;
			case XGR1: {
				if (size == 2) {
					uint16_t val = *((uint16_t *) buffer);
					currentRegisterBank->setXGRx(1, val);
				} else {
					currentRegisterBank->setXGRx(1, (currentRegisterBank->getXGRx(1) & 0x00FF) | (((uint16_t) *((uint8_t *)buffer)) << 8));
				}
			} break;
			case XGR1+1:{
				currentRegisterBank->setXGRx(1, (currentRegisterBank->getXGRx(1) & 0xFF00) | ((uint16_t) *((uint8_t *)buffer)));
			} break;
			case XGR2: {
				if (size == 2) {
					uint16_t val = *((uint16_t *) buffer);
					currentRegisterBank->setXGRx(2, val);
				} else {
					currentRegisterBank->setXGRx(2, (currentRegisterBank->getXGRx(2) & 0x00FF) | (((uint16_t) *((uint8_t *)buffer)) << 8));
				}
			} break;
			case XGR2+1:{
				currentRegisterBank->setXGRx(2, (currentRegisterBank->getXGRx(2) & 0xFF00) | ((uint16_t) *((uint8_t *)buffer)));
			} break;
			case XGR3: {
				if (size == 2) {
					uint16_t val = *((uint16_t *) buffer);
					currentRegisterBank->setXGRx(3, val);
				} else {
					currentRegisterBank->setXGRx(3, (currentRegisterBank->getXGRx(3) & 0x00FF) | (((uint16_t) *((uint8_t *)buffer)) << 8));
				}
			} break;
			case XGR3+1:{
				currentRegisterBank->setXGRx(3, (currentRegisterBank->getXGRx(3) & 0xFF00) | ((uint16_t) *((uint8_t *)buffer)));
			} break;
			case XGR4: {
				if (size == 2) {
					uint16_t val = *((uint16_t *) buffer);
					currentRegisterBank->setXGRx(4, val);
				} else {
					currentRegisterBank->setXGRx(4, (currentRegisterBank->getXGRx(4) & 0x00FF) | (((uint16_t) *((uint8_t *)buffer)) << 8));
				}
			} break;
			case XGR4+1:{
				currentRegisterBank->setXGRx(4, (currentRegisterBank->getXGRx(4) & 0xFF00) | ((uint16_t) *((uint8_t *)buffer)));
			} break;
			case XGR5: {
				if (size == 2) {
					uint16_t val = *((uint16_t *) buffer);
					currentRegisterBank->setXGRx(5, val);
				} else {
					currentRegisterBank->setXGRx(5, (currentRegisterBank->getXGRx(5) & 0x00FF) | (((uint16_t) *((uint8_t *)buffer)) << 8));
				}
			} break;
			case XGR5+1:{
				currentRegisterBank->setXGRx(5, (currentRegisterBank->getXGRx(5) & 0xFF00) | ((uint16_t) *((uint8_t *)buffer)));
			} break;
			case XGR6: {
				if (size == 2) {
					uint16_t val = *((uint16_t *) buffer);
					currentRegisterBank->setXGRx(6, val);
				} else {
					currentRegisterBank->setXGRx(6, (currentRegisterBank->getXGRx(6) & 0x00FF) | (((uint16_t) *((uint8_t *)buffer)) << 8));
				}
			} break;
			case XGR6+1:{
				currentRegisterBank->setXGRx(6, (currentRegisterBank->getXGRx(6) & 0xFF00) | ((uint16_t) *((uint8_t *)buffer)));
			} break;
			case XGR7: {
				if (size == 2) {
					uint16_t val = *((uint16_t *) buffer);
					currentRegisterBank->setXGRx(7, val);
				} else {
					currentRegisterBank->setXGRx(7, (currentRegisterBank->getXGRx(7) & 0x00FF) | (((uint16_t) *((uint8_t *)buffer)) << 8));
				}
			} break;
			case XGR7+1:{
				currentRegisterBank->setXGRx(7, (currentRegisterBank->getXGRx(7) & 0xFF00) | ((uint16_t) *((uint8_t *)buffer)));
			} break;

			default: {
				if ((offset >= XGIF_7F_70) && (offset <= (XGIF_0F_00 + 1))) {
					if (size == 2) {
						uint16_t val = *((uint16_t *) buffer);
						xgif_register[(offset - XGIF_7F_70)/2] = val;
					} else {
						if (((offset - XGIF_7F_70) % 2) == 0) {
							xgif_register[(offset - XGIF_7F_70)/2] = (xgif_register[(offset - XGIF_7F_70)/2] & 0x00FF) | (((uint16_t) *((uint8_t *)buffer)) << 8);
						} else {
							xgif_register[(offset - XGIF_7F_70)/2] = (xgif_register[(offset - XGIF_7F_70)/2] & 0xFF00) | ((uint16_t) *((uint8_t *)buffer));
						}
					}
				}

				break;
			}
		}

		return (true);
	}

	return (false);
}

Register *XGATE::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

void XGATE::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

bool XGATE::read(unsigned int offset, const void *buffer, unsigned int data_length)
{

	switch (offset) {
		case XGMCTL: *((uint16_t *) buffer) = Host2BigEndian(xgmctl_register & 0x00FB); break;
		case XGCHID: {
			*((uint8_t *) buffer) = getXGCHID();
		} break;
		case XGCHPL: *((uint8_t *) buffer) = getXGCHPL(); break;
		case RESERVED2: *((uint16_t *) buffer) = 0; break;
		case XGISPSEL: *((uint8_t *) buffer) = getXGISPSEL(); break;
		case XGVBR: *((uint16_t *) buffer) = Host2BigEndian(getXGVBR() & 0xFFFE); break;
		case XGSWT: *((uint16_t *) buffer) = Host2BigEndian(xgswt_register & 0x00FF); break;
		case XGSEM: *((uint16_t *) buffer) = Host2BigEndian(xgsem_register & 0x00FF); break;
		case RESERVED4: *((uint16_t *) buffer) = 0; break;
		case XGCCR: *((uint8_t *) buffer) = currentRegisterBank->getXGCCR() & 0x0F; break;
		case XGPC: *((address_t *) buffer) = Host2BigEndian(currentRegisterBank->getXGPC()); break;
		case RESERVED5: *((uint16_t *) buffer) = 0; break;
		case RESERVED6: *((uint16_t *) buffer) = 0; break;
		case XGR1: *((uint16_t *) buffer) = Host2BigEndian(currentRegisterBank->getXGRx(1)); break;
		case XGR2: *((uint16_t *) buffer) = Host2BigEndian(currentRegisterBank->getXGRx(2)); break;
		case XGR3: *((uint16_t *) buffer) = Host2BigEndian(currentRegisterBank->getXGRx(3)); break;
		case XGR4: *((uint16_t *) buffer) = Host2BigEndian(currentRegisterBank->getXGRx(4)); break;
		case XGR5: *((uint16_t *) buffer) = Host2BigEndian(currentRegisterBank->getXGRx(5)); break;
		case XGR6: *((uint16_t *) buffer) = Host2BigEndian(currentRegisterBank->getXGRx(6)); break;
		case XGR7: *((uint16_t *) buffer) = Host2BigEndian(currentRegisterBank->getXGRx(7)); break;

		default: {
			if ((offset >= XGIF_7F_70) && (offset <= (XGIF_0F_00 + 1))) {
				unsigned int index = (offset - XGIF_7F_70)/2;
				if (index < 2) {
					*((uint16_t *) buffer) = Host2BigEndian(xgif_register[index] & 0x01FF);
				}
				else if (index >= XGIF_0F_00) {
					*((uint16_t *) buffer) = Host2BigEndian(xgif_register[index] & 0xFE00);
				}
				else {
					*((uint16_t *) buffer) = Host2BigEndian(xgif_register[index]);
				}
			}

			break;
		}

	}

	return (true);

}

bool XGATE::write(unsigned int offset, const void *buffer, unsigned int data_length)
{
	switch (offset) {
		case XGMCTL: {
			uint16_t val = BigEndian2Host(*((uint16_t *) buffer)) & 0xFBFB;
			// control bits can only be set or cleared if a "1" is written to its mask bit in the same register access.
			uint16_t mask = val >> 8;

			for (unsigned int i=0,j=1; i<8; i++,j=j*2) {
				if ((mask & j) != 0) {
					if ((val & j) != 0) {
						xgmctl_register = xgmctl_register | j;
					} else {
						xgmctl_register = xgmctl_register & ~j;
					}
				}
			}
			// Freeze and debug modes aren't emulated, hence we don't take care about XGFRZ and XGDBG bits
			if ((xgmctl_register & 0x0080) == 0x0080) {
				enbale_xgate();
			} else {
				/**
				 *  All XGATE interrupts (pending requests) can be disabled by the XGIE bit in the XGATE module control register (XGMCTL)
				 *  The pending request are canceled by clearing XGSWT register
				 */
				disable_xgate();
			}

			// check clearing of XGSWEIF bit
			if (((val & mask) & 0x02) != 0) {
				/**
				 * XGMCTL::XGSWEIF
				 *  XGATE Software Error Interrupt Flag — This bit signals a pending Software Error Interrupt.
				 *  It is set if the RISC core detects an error condition (see Section 6.4.5, “Software Error Detection”).
				 *  The RISC core is stopped while this bit is set.
				 *  Clearing this bit will terminate the current thread and cause the XGATE to become idle.
				 */

				xgmctl_register = xgmctl_register & 0xFFFD;
				terminateCurrentThread();
			}

			if ((xgmctl_register & 0x04) != 0) {
				fakeXGATEActivity();
			}
		} break;
		case XGCHID: {
			// The XGCHID register shows the identifier of the currently active channel. It is only writable in debug mode.
			if (mode == DEBUG) {
				uint8_t val = *((uint8_t *) buffer);
				setXGCHID(val & 0x7F);
			}

		} break;
		case XGCHPL: break;
		case RESERVED2: break;
		case XGISPSEL: {
			uint8_t val = *((uint8_t *) buffer);
			setXGISPSEL(val & 0x03);
		} break;
		case XGVBR: {
			// Writable if the module is disabled (XGMCTL::XGE = 0) and idle (XGCHID = 0x00)
			// I use "RUNNING state and not IDLE because it includes "IDLE" and "STOP" modes

			if (((xgmctl_register & 0x0080) == 0) && (getXGCHID() == 0x00)) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));

				setXGVBR(val & 0xFFFE);
			}
		} break;
		case XGSWT: {
			/**
			 * NOTE:
			 * The XGATE channel IDs that are associated with the eight software triggers
			 * are determined on chip integration level. (see Section “Interrupts” of the Soc Guide)
			 * XGATE software triggers work like any peripheral interrupt.
			 * They can be used as XGATE requests as well as S12X_CPU interrupts.
			 * The target of the software trigger must be selected in the S12X_INT module
			 */
			uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
			uint16_t mask = val >> 8;

			for (unsigned int i=0,j=1; i<8; i++,j=j*2) {
				if ((mask & j) != 0) {
					if ((val & j) != 0) {
						xgswt_register = xgswt_register | j;

						// Software trigger pending if XGIE bit is set
						if ((xgmctl_register & 0x0080) == 0x0080) {
							triggerChannelThread();
						}

					} else {
						xgswt_register = xgswt_register & ~j;
					}
				}
			}

		} break;
		case XGSEM: {

			uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
			uint16_t mask = val >> 8;

			for (unsigned int i=0,j=1; i<8; i++,j=j*2) {
				if ((mask & j) != 0) {
					if ((val & j) != 0) {
						lockSemaphore(TOWNER::CPU12X, i);
					} else {
						unlockSemaphore(TOWNER::CPU12X, i);
					}
				}
			}

		} break;
		case RESERVED4: break;
		case XGCCR: {
			if (mode == DEBUG) {
				uint8_t val = *((uint8_t *) buffer);
				currentRegisterBank->setXGCCR(val & 0x0F);
			}
		} break;
		case XGPC: {
			if (mode == DEBUG) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				currentRegisterBank->setXGPC(val);
			}
		} break;
		case RESERVED5: break;
		case RESERVED6: break;
		case XGR1: {
			if (mode == DEBUG) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				currentRegisterBank->setXGRx(1, val);
			}
		} break;
		case XGR2: {
			if (mode == DEBUG) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				currentRegisterBank->setXGRx(2, val);
			}
		} break;
		case XGR3: {
			if (mode == DEBUG) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				currentRegisterBank->setXGRx(3, val);
			}
		} break;
		case XGR4: {
			if (mode == DEBUG) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				currentRegisterBank->setXGRx(4, val);
			}
		} break;
		case XGR5: {
			if (mode == DEBUG) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				currentRegisterBank->setXGRx(5, val);
			}
		} break;
		case XGR6: {
			if (mode == DEBUG) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				currentRegisterBank->setXGRx(6, val);
			}
		} break;
		case XGR7: {
			if (mode == DEBUG) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				currentRegisterBank->setXGRx(7, val);
			}
		} break;

		default: {
			if ((offset >= XGIF_7F_70) && (offset <= (XGIF_0F_00 + 1))) {
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				xgif_register[(offset - XGIF_7F_70)/2] = (xgif_register[(offset - XGIF_7F_70)/2] & ~val);
			} else {
				return (false);
			}
			break;
		}
	}

	return (true);

}


//=====================================================================
//=                   Debugging methods                               =
//=====================================================================

string XGATE::getObjectFriendlyName(physical_address_t addr)
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

string XGATE::getFunctionFriendlyName(physical_address_t addr)
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



} // end of namespace s12xgate
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim



