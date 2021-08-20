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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_SERVICE_DEBUG_DEBUGGER_DEBUGGER_TCC__
#define __UNISIM_SERVICE_DEBUG_DEBUGGER_DEBUGGER_TCC__

#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/util/debug/breakpoint_registry.tcc>
#include <unisim/util/debug/watchpoint_registry.tcc>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <stdexcept>
#include <fstream>

namespace unisim {
namespace service {
namespace debug {
namespace debugger {

template <typename CONFIG>
Debugger<CONFIG>::Debugger(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent, "Debugger back-end")
	, unisim::kernel::Client<unisim::service::interfaces::Blob<ADDRESS> >(name, parent)
	, debug_yielding_export()
	, memory_access_reporting_export()
	, trap_reporting_export()
	, debug_yielding_request_export()
	, debug_selecting_export()
	, debug_event_trigger_export()
	, disasm_export()
	, memory_export()
	, registers_export()
	, symbol_table_lookup_export()
	, stmt_lookup_export()
	, backtrace_export()
	, debug_info_loading_export()
	, data_object_lookup_export()
	, subprogram_lookup_export()
	, blob_import("blob-import", this)
	, memory_access_reporting_control_import()
	, disasm_import()
	, memory_import()
	, registers_import()
	, debug_event_listener_import()
	, debug_yielding_import()
	, prc_gate()
	, front_end_gate()
	, sel_prc_gate()
	, requires_fetch_instruction_reporting()
	, requires_commit_instruction_reporting()
	, requires_memory_access_reporting()
	, verbose(false)
	, dwarf_to_html_output_directory()
	, dwarf_register_number_mapping_filename()
	, parse_dwarf(false)
	, debug_dwarf(false)
	, sel_cpu()
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_dwarf_to_html_output_directory("dwarf-to-html-output-directory", this, dwarf_to_html_output_directory, "DWARF v2/v3 to HTML output directory")
	, param_dwarf_register_number_mapping_filename("dwarf-register-number-mapping-filename", this, dwarf_register_number_mapping_filename, "DWARF register number mapping filename")
	, param_parse_dwarf("parse-dwarf", this, parse_dwarf, "Enable/Disable parsing of DWARF debugging informations")
	, param_debug_dwarf("debug-dwarf", this, debug_dwarf, "Enable/Disable debugging of DWARF")
	, param_sel_cpu("sel-cpu", this, sel_cpu, MAX_FRONT_ENDS, "CPU being debugged by front-end")
	, logger(*this)
	, setup_debug_info_done(false)
	, mutex()
	, breakpoint_registry()
	, watchpoint_registry()
	, fetch_insn_event_set()
	, commit_insn_event_set()
	, trap_event_set()
	, schedule_mutex()
	, schedule(0)
	, elf32_loaders()
	, elf64_loaders()
	, coff_loaders()
	, enable_elf32_loaders()
	, enable_elf64_loaders()
	, enable_coff_loaders()
{
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&schedule_mutex, NULL);
	
	unsigned int prc_num;
	unsigned int front_end_num;
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		param_sel_cpu[front_end_num].SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream debug_yielding_export_name_sstr;
		debug_yielding_export_name_sstr << "debug-yielding-export[" << prc_num << "]";
		debug_yielding_export[prc_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYielding>(debug_yielding_export_name_sstr.str().c_str(), this);
		
		std::stringstream memory_access_reporting_export_name_sstr;
		memory_access_reporting_export_name_sstr << "memory-access-reporting-export[" << prc_num << "]";
		memory_access_reporting_export[prc_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::MemoryAccessReporting<ADDRESS> >(memory_access_reporting_export_name_sstr.str().c_str(), this);

		std::stringstream trap_reporting_export_name_sstr;
		trap_reporting_export_name_sstr << "trap-reporting-export[" << prc_num << "]";
		trap_reporting_export[prc_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::TrapReporting>(trap_reporting_export_name_sstr.str().c_str(), this);
		
		std::stringstream memory_access_reporting_control_import_name_sstr;
		memory_access_reporting_control_import_name_sstr << "memory-access-reporting-control-import[" << prc_num << "]";
		memory_access_reporting_control_import[prc_num] = new unisim::kernel::ServiceImport<unisim::service::interfaces::MemoryAccessReportingControl>(memory_access_reporting_control_import_name_sstr.str().c_str(), this);

		std::stringstream disasm_import_name_sstr;
		disasm_import_name_sstr << "disasm-import[" << prc_num << "]";
		disasm_import[prc_num] = new unisim::kernel::ServiceImport<unisim::service::interfaces::Disassembly<ADDRESS> >(disasm_import_name_sstr.str().c_str(), this);

		std::stringstream memory_import_name_sstr;
		memory_import_name_sstr << "memory-import[" << prc_num << "]";
		memory_import[prc_num] = new unisim::kernel::ServiceImport<unisim::service::interfaces::Memory<ADDRESS> >(memory_import_name_sstr.str().c_str(), this);

		std::stringstream registers_import_name_sstr;
		registers_import_name_sstr << "registers-import[" << prc_num << "]";
		registers_import[prc_num] = new unisim::kernel::ServiceImport<unisim::service::interfaces::Registers>(registers_import_name_sstr.str().c_str(), this);
	}
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		std::stringstream debug_yielding_request_export_name_sstr;
		debug_yielding_request_export_name_sstr << "debug-yielding-request-export[" << front_end_num << "]";
		debug_yielding_request_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYieldingRequest>(debug_yielding_request_export_name_sstr.str().c_str(), this);

		std::stringstream debug_selecting_export_name_sstr;
		debug_selecting_export_name_sstr << "debug-selecting-export[" << front_end_num << "]";
		debug_selecting_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DebugSelecting>(debug_selecting_export_name_sstr.str().c_str(), this);
		
		std::stringstream debug_event_trigger_export_name_sstr;
		debug_event_trigger_export_name_sstr << "debug-event-trigger-export[" << front_end_num << "]";
		debug_event_trigger_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >(debug_event_trigger_export_name_sstr.str().c_str(), this);
		
		std::stringstream disasm_export_name_sstr;
		disasm_export_name_sstr << "disasm-export[" << front_end_num << "]";
		disasm_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::Disassembly<ADDRESS> >(disasm_export_name_sstr.str().c_str(), this);
		
		std::stringstream memory_export_name_sstr;
		memory_export_name_sstr << "memory-export[" << front_end_num << "]";
		memory_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::Memory<ADDRESS> >(memory_export_name_sstr.str().c_str(), this);
		
		std::stringstream registers_export_name_sstr;
		registers_export_name_sstr << "registers-export[" << front_end_num << "]";
		registers_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::Registers>(registers_export_name_sstr.str().c_str(), this);
		
		std::stringstream symbol_table_lookup_export_name_sstr;
		symbol_table_lookup_export_name_sstr << "symbol-table-lookup-export[" << front_end_num << "]";
		symbol_table_lookup_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >(symbol_table_lookup_export_name_sstr.str().c_str(), this);
		
		std::stringstream stmt_lookup_export_name_sstr;
		stmt_lookup_export_name_sstr << "stmt-lookup-export[" << front_end_num << "]";
		stmt_lookup_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::StatementLookup<ADDRESS> >(stmt_lookup_export_name_sstr.str().c_str(), this);
		
		std::stringstream backtrace_export_name_sstr;
		backtrace_export_name_sstr << "backtrace-export[" << front_end_num << "]";
		backtrace_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::BackTrace<ADDRESS> >(backtrace_export_name_sstr.str().c_str(), this);
		
		std::stringstream debug_info_loading_export_name_sstr;
		debug_info_loading_export_name_sstr << "debug-info-loading-export[" << front_end_num << "]";
		debug_info_loading_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DebugInfoLoading>(debug_info_loading_export_name_sstr.str().c_str(), this);
		
		std::stringstream data_object_lookup_export_name_sstr;
		data_object_lookup_export_name_sstr << "data-object-lookup-export[" << front_end_num << "]";
		data_object_lookup_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DataObjectLookup<ADDRESS> >(data_object_lookup_export_name_sstr.str().c_str(), this);
		
		std::stringstream subprogram_lookup_export_name_sstr;
		subprogram_lookup_export_name_sstr << "subprogram-lookup-export[" << front_end_num << "]";
		subprogram_lookup_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::SubProgramLookup<ADDRESS> >(subprogram_lookup_export_name_sstr.str().c_str(), this);
		
		std::stringstream debug_event_listener_import_name_sstr;
		debug_event_listener_import_name_sstr << "debug-event-listener-import[" << front_end_num << "]";
		debug_event_listener_import[front_end_num] = new unisim::kernel::ServiceImport<unisim::service::interfaces::DebugEventListener<ADDRESS> >(debug_event_listener_import_name_sstr.str().c_str(), this);
		
		std::stringstream debug_yielding_import_name_sstr;
		debug_yielding_import_name_sstr << "debug-yielding-import[" << front_end_num << "]";
		debug_yielding_import[front_end_num] = new unisim::kernel::ServiceImport<unisim::service::interfaces::DebugYielding>(debug_yielding_import_name_sstr.str().c_str(), this);
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream prc_gate_name_sstr;
		prc_gate_name_sstr << "processor" << prc_num << "-gate";
		prc_gate[prc_num] = new ProcessorGate(prc_gate_name_sstr.str().c_str(), prc_num, this);
	}
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		std::stringstream front_end_gate_name_sstr;
		front_end_gate_name_sstr << "front-end" << front_end_num << "-gate";
		front_end_gate[front_end_num] = new FrontEndGate(front_end_gate_name_sstr.str().c_str(), front_end_num, this); // Note: Processor gates shall be created before front-end gates
	}
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		DebugSelect(front_end_num, sel_cpu[front_end_num]);
	}
}

template <typename CONFIG>
Debugger<CONFIG>::~Debugger()
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		delete elf32_loaders[i];
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		delete elf64_loaders[i];
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		delete coff_loaders[i];
	}

	unsigned int prc_num;
	unsigned int front_end_num;

	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		if(fetch_insn_event_set[prc_num].size())
		{
			do
			{
				typename std::set<unisim::util::debug::FetchInsnEvent<ADDRESS> *>::iterator it = fetch_insn_event_set[prc_num].begin();
				unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = *it;
				fetch_insn_event_set[prc_num].erase(it);
				fetch_insn_event->Release();
			}
			while(fetch_insn_event_set[prc_num].size());
		}

		if(commit_insn_event_set[prc_num].size())
		{
			do
			{
				typename std::set<unisim::util::debug::CommitInsnEvent<ADDRESS> *>::iterator it = commit_insn_event_set[prc_num].begin();
				unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event = *it;
				commit_insn_event_set[prc_num].erase(it);
				commit_insn_event->Release();
			}
			while(commit_insn_event_set[prc_num].size());
		}

		if(trap_event_set[prc_num].size())
		{
			do
			{
				typename std::set<unisim::util::debug::TrapEvent<ADDRESS> *>::iterator it = trap_event_set[prc_num].begin();
				unisim::util::debug::TrapEvent<ADDRESS> *trap_event = *it;
				trap_event_set[prc_num].erase(it);
				trap_event->Release();
			}
			while(trap_event_set[prc_num].size());
		}
	}

	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete debug_yielding_export[prc_num];
		delete memory_access_reporting_export[prc_num];
		delete trap_reporting_export[prc_num];
		delete memory_access_reporting_control_import[prc_num];
		delete disasm_import[prc_num];
		delete memory_import[prc_num];
		delete registers_import[prc_num];
	}
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		delete debug_yielding_request_export[front_end_num];
		delete debug_selecting_export[front_end_num];
		delete debug_event_trigger_export[front_end_num];
		delete disasm_export[front_end_num];
		delete memory_export[front_end_num];
		delete registers_export[front_end_num];
		delete symbol_table_lookup_export[front_end_num];
		delete stmt_lookup_export[front_end_num];
		delete backtrace_export[front_end_num];
		delete debug_info_loading_export[front_end_num];
		delete data_object_lookup_export[front_end_num];
		delete subprogram_lookup_export[front_end_num];
		delete debug_event_listener_import[front_end_num];
		delete debug_yielding_import[front_end_num];
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete prc_gate[prc_num];
	}
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		delete front_end_gate[front_end_num];
	}
	
	pthread_mutex_destroy(&schedule_mutex);
	pthread_mutex_destroy(&mutex);
}

template <typename CONFIG>
bool Debugger<CONFIG>::BeginSetup()
{
	unsigned int prc_num;
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		requires_fetch_instruction_reporting[prc_num] = false;
		requires_commit_instruction_reporting[prc_num] = false;
		requires_memory_access_reporting[prc_num] = false;
	}
	
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		delete elf32_loaders[i];
	}
	elf32_loaders.clear();

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		delete elf64_loaders[i];
	}
	elf64_loaders.clear();

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		delete coff_loaders[i];
	}
	coff_loaders.clear();
	
	setup_debug_info_done = false;
	
	return true;
}

template <typename CONFIG>
bool Debugger<CONFIG>::SetupDebugInfo(const unisim::util::blob::Blob<ADDRESS> *blob)
{
	typename unisim::util::blob::FileFormat ffmt = blob->GetFileFormat();
	
	switch(ffmt)
	{
		case unisim::util::blob::FFMT_UNKNOWN:
			break;
		case unisim::util::blob::FFMT_ELF32:
			{
				unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = new unisim::util::loader::elf_loader::Elf32Loader<ADDRESS>(blob);
				
				elf32_loader->SetDebugInfoStream(logger.DebugInfoStream());
				elf32_loader->SetDebugWarningStream(logger.DebugWarningStream());
				elf32_loader->SetDebugErrorStream(logger.DebugErrorStream());
				unsigned int prc_num;
				for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
				{
					elf32_loader->SetRegistersInterface(prc_num, prc_gate[prc_num]->registers_import);
					elf32_loader->SetMemoryInterface(prc_num, prc_gate[prc_num]->memory_import);
				}
				elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf);
				elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose);
				elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_REGISTER_NUMBER_MAPPING_FILENAME, unisim::kernel::Object::GetSimulator()->SearchSharedDataFile(dwarf_register_number_mapping_filename.c_str()).c_str());
				elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_DEBUG_DWARF, debug_dwarf);

				elf32_loader->ParseSymbols();
				elf32_loaders.push_back(elf32_loader);
				unsigned int front_end_num;
				for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
				{
					enable_elf32_loaders[front_end_num].push_back(true);
				}
			}
			break;
		case unisim::util::blob::FFMT_ELF64:
			{
				unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = new unisim::util::loader::elf_loader::Elf64Loader<ADDRESS>(blob);
				
				elf64_loader->SetDebugInfoStream(logger.DebugInfoStream());
				elf64_loader->SetDebugWarningStream(logger.DebugWarningStream());
				elf64_loader->SetDebugErrorStream(logger.DebugErrorStream());
				unsigned int prc_num;
				for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
				{
					elf64_loader->SetRegistersInterface(prc_num, prc_gate[prc_num]->registers_import);
					elf64_loader->SetMemoryInterface(prc_num, prc_gate[prc_num]->memory_import);
				}
				elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf);
				elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose);
				elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_REGISTER_NUMBER_MAPPING_FILENAME, unisim::kernel::Object::GetSimulator()->SearchSharedDataFile(dwarf_register_number_mapping_filename.c_str()).c_str());
				elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_DEBUG_DWARF, debug_dwarf);

				elf64_loader->ParseSymbols();
				elf64_loaders.push_back(elf64_loader);
				unsigned int front_end_num;
				for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
				{
					enable_elf64_loaders[front_end_num].push_back(true);
				}
			}
			break;
		case unisim::util::blob::FFMT_COFF:
			{
				unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = new unisim::util::loader::coff_loader::CoffLoader<ADDRESS>(logger.DebugInfoStream(), logger.DebugWarningStream(), logger.DebugErrorStream(), blob);
				
				coff_loader->ParseSymbols();
				coff_loaders.push_back(coff_loader);
				unsigned int front_end_num;
				for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
				{
					enable_coff_loaders[front_end_num].push_back(true);
				}
			}
			break;
	}
	
	const typename std::vector<const unisim::util::blob::Blob<ADDRESS> *>& sibling_blobs = blob->GetBlobs();
	
	typename std::vector<const unisim::util::blob::Blob<ADDRESS> *>::const_iterator it;
	for(it = sibling_blobs.begin(); it != sibling_blobs.end(); it++)
	{
		const unisim::util::blob::Blob<ADDRESS> *sibling_blob = *it;
		SetupDebugInfo(sibling_blob);
	}
	return true;
}

template <typename CONFIG>
bool Debugger<CONFIG>::SetupDebugInfo()
{
	if(setup_debug_info_done) return true;
	if(!blob_import)
	{
		logger << DebugError << "Blob import is not connected" << EndDebugError;
		return false;
	}
	const unisim::util::blob::Blob<ADDRESS> *blob = blob_import->GetBlob();
	if(!blob) return true; // no blob
	bool status = SetupDebugInfo(blob);
	if(status) setup_debug_info_done = true;
	return setup_debug_info_done;
}

template <typename CONFIG>
void Debugger<CONFIG>::UpdateReportingRequirements(unsigned int prc_num)
{
	requires_fetch_instruction_reporting[prc_num] = breakpoint_registry.HasBreakpoints(prc_num) || !fetch_insn_event_set[prc_num].empty();
	requires_commit_instruction_reporting[prc_num] = !commit_insn_event_set[prc_num].empty();
	requires_memory_access_reporting[prc_num] = watchpoint_registry.HasWatchpoints(prc_num);

	prc_gate[prc_num]->RequiresMemoryAccessReporting(unisim::service::interfaces::REPORT_MEM_ACCESS, requires_memory_access_reporting[prc_num]);
	prc_gate[prc_num]->RequiresMemoryAccessReporting(unisim::service::interfaces::REPORT_FETCH_INSN, requires_fetch_instruction_reporting[prc_num]);
	prc_gate[prc_num]->RequiresMemoryAccessReporting(unisim::service::interfaces::REPORT_COMMIT_INSN, requires_commit_instruction_reporting[prc_num]);
}

template <typename CONFIG>
void Debugger<CONFIG>::ScheduleFrontEnd(unsigned int front_end_num)
{
	pthread_mutex_lock(&schedule_mutex);
	schedule |= 1 << front_end_num;
	pthread_mutex_unlock(&schedule_mutex);
}

template <typename CONFIG>
bool Debugger<CONFIG>::NextScheduledFrontEnd(unsigned int& front_end_num)
{
	pthread_mutex_lock(&schedule_mutex);
	if(unisim::util::arithmetic::BitScanForward(front_end_num, schedule))
	{
		schedule &= ~(1 << front_end_num);
		pthread_mutex_unlock(&schedule_mutex);
		return true;
	}
	pthread_mutex_unlock(&schedule_mutex);
	return false;
}

template <typename CONFIG>
bool Debugger<CONFIG>::IsScheduleEmpty() const
{
	return schedule == 0;
}

// Exports to CPUs

// unisim::service::interfaces::DebugYielding (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::DebugYield(unsigned int prc_num)
{
	if(likely(IsScheduleEmpty())) return;

	unsigned int front_end_num = 0;
	
	while(NextScheduledFrontEnd(front_end_num))
	{
		front_end_gate[front_end_num]->DebugYield();
	}
}

// unisim::service::interfaces::MemoryAccessReporting<ADDRESS> (tagged)
template <typename CONFIG>
bool Debugger<CONFIG>::ReportMemoryAccess(unsigned int prc_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size)
{
	bool overlook = true;
	
#if 0
	if(unlikely(!requires_memory_access_reporting[prc_num]))
	{
		logger << DebugWarning << "Processor #" << prc_num << " reports memory access even if it has been asked not to" << EndDebugWarning;
		return overlook;
	}
#endif
	
	if(unlikely(watchpoint_registry.HasWatchpoints(mat, mt, addr, size, prc_num)))
	{
		unsigned int front_end_num;
		
		for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
		{
			Dispatcher<unisim::util::debug::Watchpoint<ADDRESS> > watchpoint_dispatcher(*this, front_end_num, overlook);
			
			// Note: this function may alter local variable "overlook"
			if(watchpoint_registry.template FindWatchpoints<Dispatcher<unisim::util::debug::Watchpoint<ADDRESS> > >(mat, mt, addr, size, prc_num, front_end_num, watchpoint_dispatcher))
			{
				ScheduleFrontEnd(front_end_num);
			}
		}
	}
	
	return overlook;
}

template <typename CONFIG>
void Debugger<CONFIG>::ReportCommitInstruction(unsigned int prc_num, ADDRESS addr, unsigned int length)
{
#if 0
	if(unlikely(!requires_commit_instruction_reporting[prc_num]))
	{
		logger << DebugWarning << "Processor #" << prc_num << " reports instruction commit even if it has been asked not to" << EndDebugWarning;
	}
#endif

	if(unlikely(!commit_insn_event_set[prc_num].empty()))
	{
		typename std::set<unisim::util::debug::CommitInsnEvent<ADDRESS> *>::iterator it;
		
		for(it = commit_insn_event_set[prc_num].begin(); it != commit_insn_event_set[prc_num].end(); it++)
		{
			unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event = *it;
			unsigned int front_end_num = commit_insn_event->GetFrontEndNumber();
			commit_insn_event->SetAddress(addr);
			commit_insn_event->SetLength(length);
			front_end_gate[front_end_num]->OnDebugEvent(commit_insn_event);
			ScheduleFrontEnd(front_end_num);
		}
	}
}

template <typename CONFIG>
void Debugger<CONFIG>::ReportFetchInstruction(unsigned int prc_num, ADDRESS next_addr)
{
#if 0
	if(unlikely(!requires_fetch_instruction_reporting[prc_num]))
	{
		logger << DebugWarning << "Processor #" << prc_num << " reports instruction fetch even if it has been asked not to" << EndDebugWarning;
	}
#endif
	if(unlikely(breakpoint_registry.HasBreakpoints(next_addr, prc_num)))
	{
		unsigned int front_end_num;
		
		for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
		{
			Dispatcher<unisim::util::debug::Breakpoint<ADDRESS> > breakpoint_dispatcher(*this, front_end_num);
			
			if(breakpoint_registry.template FindBreakpoints<Dispatcher<unisim::util::debug::Breakpoint<ADDRESS> > >(next_addr, prc_num, front_end_num, breakpoint_dispatcher))
			{
				ScheduleFrontEnd(front_end_num);
			}
		}
	}
	
	if(unlikely(!fetch_insn_event_set[prc_num].empty()))
	{
		typename std::set<unisim::util::debug::FetchInsnEvent<ADDRESS> *>::iterator it;
		
		for(it = fetch_insn_event_set[prc_num].begin(); it != fetch_insn_event_set[prc_num].end(); it++)
		{
			unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = *it;
			unsigned int front_end_num = fetch_insn_event->GetFrontEndNumber();
			fetch_insn_event->SetAddress(next_addr);
			front_end_gate[front_end_num]->OnDebugEvent(fetch_insn_event);
			ScheduleFrontEnd(front_end_num);
		}
	}
}


// unisim::service::interfaces::TrapReporting (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::ReportTrap(unsigned int prc_num)
{
	if(unlikely(!trap_event_set[prc_num].empty()))
	{
		typename std::set<unisim::util::debug::TrapEvent<ADDRESS> *>::iterator it;
		
		for(it = trap_event_set[prc_num].begin(); it != trap_event_set[prc_num].end(); it++)
		{
			unisim::util::debug::TrapEvent<ADDRESS> *trap_event = *it;
			unsigned int front_end_num = trap_event->GetFrontEndNumber();
			trap_event->SetTrapObject(0);
			trap_event->SetTrapMessage("");
			front_end_gate[front_end_num]->OnDebugEvent(trap_event);
			ScheduleFrontEnd(front_end_num);
		}
	}
}

template <typename CONFIG>
void Debugger<CONFIG>::ReportTrap(unsigned int prc_num, const unisim::kernel::Object &obj)
{
	if(unlikely(!trap_event_set[prc_num].empty()))
	{
		typename std::set<unisim::util::debug::TrapEvent<ADDRESS> *>::iterator it;
		
		for(it = trap_event_set[prc_num].begin(); it != trap_event_set[prc_num].end(); it++)
		{
			unisim::util::debug::TrapEvent<ADDRESS> *trap_event = *it;
			unsigned int front_end_num = trap_event->GetFrontEndNumber();
			trap_event->SetTrapObject(&obj);
			trap_event->SetTrapMessage("");
			front_end_gate[front_end_num]->OnDebugEvent(trap_event);
			ScheduleFrontEnd(front_end_num);
		}
	}
}

template <typename CONFIG>
void Debugger<CONFIG>::ReportTrap(unsigned int prc_num, const unisim::kernel::Object &obj, const std::string &str)
{
	if(unlikely(!trap_event_set[prc_num].empty()))
	{
		typename std::set<unisim::util::debug::TrapEvent<ADDRESS> *>::iterator it;
		
		for(it = trap_event_set[prc_num].begin(); it != trap_event_set[prc_num].end(); it++)
		{
			unisim::util::debug::TrapEvent<ADDRESS> *trap_event = *it;
			unsigned int front_end_num = trap_event->GetFrontEndNumber();
			trap_event->SetTrapObject(&obj);
			trap_event->SetTrapMessage(str);
			front_end_gate[front_end_num]->OnDebugEvent(trap_event);
			ScheduleFrontEnd(front_end_num);
		}
	}
}

template <typename CONFIG>
void Debugger<CONFIG>::ReportTrap(unsigned int prc_num, const unisim::kernel::Object &obj, const char *c_str)
{
	if(unlikely(!trap_event_set[prc_num].empty()))
	{
		typename std::set<unisim::util::debug::TrapEvent<ADDRESS> *>::iterator it;
		
		for(it = trap_event_set[prc_num].begin(); it != trap_event_set[prc_num].end(); it++)
		{
			unisim::util::debug::TrapEvent<ADDRESS> *trap_event = *it;
			unsigned int front_end_num = trap_event->GetFrontEndNumber();
			trap_event->SetTrapObject(&obj);
			trap_event->SetTrapMessage(c_str);
			front_end_gate[front_end_num]->OnDebugEvent(trap_event);
			ScheduleFrontEnd(front_end_num);
		}
	}
}

// Export to Front-ends

// unisim::service::interfaces::DebugSelecting (tagged)
template <typename CONFIG>
bool Debugger<CONFIG>::DebugSelect(unsigned int front_end_num, unsigned int prc_num)
{
	if(prc_num >= NUM_PROCESSORS) return false;
	
	sel_prc_gate[front_end_num] = prc_gate[prc_num];
	
	return true;
}

template <typename CONFIG>
unsigned int Debugger<CONFIG>::DebugGetSelected(unsigned int front_end_num) const
{
	if(front_end_num >= MAX_FRONT_ENDS) return 0;
	
	return sel_prc_gate[front_end_num]->GetProcessorNumber();
}

// unisim::service::interfaces::DebugYieldingRequest (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::DebugYieldRequest(unsigned int front_end_num)
{
	if(front_end_num >= MAX_FRONT_ENDS) return;

	ScheduleFrontEnd(front_end_num);
}

// unisim::service::interfaces::DebugEventTrigger<ADDRESS> (tagged)
template <typename CONFIG>
bool Debugger<CONFIG>::Listen(unsigned int front_end_num, unisim::util::debug::Event<ADDRESS> *event)
{
//	std::cerr << "Listen(" << front_end_num << ", " << event << ")" << std::endl;
	int event_prc_num = event->GetProcessorNumber();
	
	unsigned int prc_num = (event_prc_num < 0) ? sel_prc_gate[front_end_num]->GetProcessorNumber() : event_prc_num;
	
	if(prc_num >= NUM_PROCESSORS) return false;
	
	event->SetProcessorNumber(prc_num);
	
	int event_front_end_num = event->GetFrontEndNumber();
	if(event_front_end_num < 0)
	{
		event->SetFrontEndNumber(front_end_num);
	}
	else if((unsigned int) event_front_end_num != front_end_num)
	{
		return false;
	}
	
	switch(event->GetType())
	{
		case unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT:
			{
				unisim::util::debug::Breakpoint<ADDRESS> *brkp = static_cast<unisim::util::debug::Breakpoint<ADDRESS> *>(event);
				
				if(!breakpoint_registry.SetBreakpoint(brkp)) return false;
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT:
			{
				unisim::util::debug::Watchpoint<ADDRESS> *wp = static_cast<unisim::util::debug::Watchpoint<ADDRESS> *>(event);
				
				if(!watchpoint_registry.SetWatchpoint(wp)) return false;
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_FETCH_INSN:
			{
				unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = static_cast<unisim::util::debug::FetchInsnEvent<ADDRESS> *>(event);

				if(fetch_insn_event_set[prc_num].find(fetch_insn_event) != fetch_insn_event_set[prc_num].end())
				{
					return false;
				}
				
				fetch_insn_event_set[prc_num].insert(fetch_insn_event);
				fetch_insn_event->Catch();
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_COMMIT_INSN:
			{
				unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event = static_cast<unisim::util::debug::CommitInsnEvent<ADDRESS> *>(event);

				if(commit_insn_event_set[prc_num].find(commit_insn_event) != commit_insn_event_set[prc_num].end())
				{
					return false;
				}

				commit_insn_event_set[prc_num].insert(commit_insn_event);
				commit_insn_event->Catch();
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_TRAP:
			{
				unisim::util::debug::TrapEvent<ADDRESS> *trap_event = static_cast<unisim::util::debug::TrapEvent<ADDRESS> *>(event);

				if(trap_event_set[prc_num].find(trap_event) != trap_event_set[prc_num].end())
				{
					return false;
				}
				
				trap_event_set[prc_num].insert(trap_event);
				trap_event->Catch();
			}
			break;

		case unisim::util::debug::Event<ADDRESS>::EV_UNKNOWN:
			return false;
			
	}
	
	UpdateReportingRequirements(prc_num);
	
	return true;
}

template <typename CONFIG>
bool Debugger<CONFIG>::Unlisten(unsigned int front_end_num, unisim::util::debug::Event<ADDRESS> *event)
{
	int event_prc_num = event->GetProcessorNumber();
	
	unsigned int prc_num = (event_prc_num < 0) ? sel_prc_gate[front_end_num]->GetProcessorNumber() : event_prc_num;

	if(prc_num >= NUM_PROCESSORS) return false;
	
	event->SetProcessorNumber(prc_num);
	
	int event_front_end_num = event->GetFrontEndNumber();
	if(event_front_end_num < 0)
	{
		event->SetFrontEndNumber(front_end_num);
	}
	else if((unsigned int) event_front_end_num != front_end_num)
	{
		return false;
	}

	switch(event->GetType())
	{
		case unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT:
			{
				unisim::util::debug::Breakpoint<ADDRESS> *brkp = static_cast<unisim::util::debug::Breakpoint<ADDRESS> *>(event);
				
				if(!breakpoint_registry.RemoveBreakpoint(brkp)) return false;
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT:
			{
				unisim::util::debug::Watchpoint<ADDRESS> *wp = static_cast<unisim::util::debug::Watchpoint<ADDRESS> *>(event);
				
				if(!watchpoint_registry.RemoveWatchpoint(wp)) return false;
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_FETCH_INSN:
			{
				unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = static_cast<unisim::util::debug::FetchInsnEvent<ADDRESS> *>(event);

				typename std::set<unisim::util::debug::FetchInsnEvent<ADDRESS> *>::const_iterator it = fetch_insn_event_set[prc_num].find(fetch_insn_event);
				if(it == fetch_insn_event_set[prc_num].end())
				{
					return false;
				}
				
				fetch_insn_event_set[prc_num].erase(fetch_insn_event);
				fetch_insn_event->Release();
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_COMMIT_INSN:
			{
				unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event = static_cast<unisim::util::debug::CommitInsnEvent<ADDRESS> *>(event);

				typename std::set<unisim::util::debug::CommitInsnEvent<ADDRESS> *>::const_iterator it = commit_insn_event_set[prc_num].find(commit_insn_event);
				if(it == commit_insn_event_set[prc_num].end())
				{
					return false;
				}
				
				commit_insn_event_set[prc_num].erase(commit_insn_event);
				commit_insn_event->Release();
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_TRAP:
			{
				unisim::util::debug::TrapEvent<ADDRESS> *trap_event = static_cast<unisim::util::debug::TrapEvent<ADDRESS> *>(event);

				typename std::set<unisim::util::debug::TrapEvent<ADDRESS> *>::const_iterator it = trap_event_set[prc_num].find(trap_event);
				if(it == trap_event_set[prc_num].end())
				{
					return false;
				}
				
				trap_event_set[prc_num].erase(trap_event);
				trap_event->Release();
			}
			break;
		default:
			// ignore
			return false;
			
	}
	
	UpdateReportingRequirements(prc_num);
	
	return true;
}

template <typename CONFIG>
bool Debugger<CONFIG>::IsEventListened(unsigned int front_end_num, unisim::util::debug::Event<ADDRESS> *event) const
{
	unsigned int prc_num = event->GetProcessorNumber();
	
	switch(event->GetType())
	{
		case unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT:
			{
				typename unisim::util::debug::Breakpoint<ADDRESS> *brkp = static_cast<unisim::util::debug::Breakpoint<ADDRESS> *>(event);
				
				return breakpoint_registry.HasBreakpoint(brkp);
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT:
			{
				typename unisim::util::debug::Watchpoint<ADDRESS> *wp = static_cast<unisim::util::debug::Watchpoint<ADDRESS> *>(event);
				
				return watchpoint_registry.HasWatchpoint(wp);
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_FETCH_INSN:
			{
				unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = static_cast<unisim::util::debug::FetchInsnEvent<ADDRESS> *>(event);

				typename std::set<unisim::util::debug::FetchInsnEvent<ADDRESS> *>::const_iterator it = fetch_insn_event_set[prc_num].find(fetch_insn_event);
				return it != fetch_insn_event_set[prc_num].end();
			}
		case unisim::util::debug::Event<ADDRESS>::EV_COMMIT_INSN:
			{
				unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event = static_cast<unisim::util::debug::CommitInsnEvent<ADDRESS> *>(event);

				typename std::set<unisim::util::debug::CommitInsnEvent<ADDRESS> *>::const_iterator it = commit_insn_event_set[prc_num].find(commit_insn_event);
				return it != commit_insn_event_set[prc_num].end();
			}
		case unisim::util::debug::Event<ADDRESS>::EV_TRAP:
			{
				unisim::util::debug::TrapEvent<ADDRESS> *trap_event = static_cast<unisim::util::debug::TrapEvent<ADDRESS> *>(event);

				typename std::set<unisim::util::debug::TrapEvent<ADDRESS> *>::const_iterator it = trap_event_set[prc_num].find(trap_event);
				return it != trap_event_set[prc_num].end();
			}
		case unisim::util::debug::Event<ADDRESS>::EV_UNKNOWN:
			return false;
			
	}
	return false;
}

template <typename CONFIG>
void Debugger<CONFIG>::EnumerateListenedEvents(unsigned int front_end_num, std::list<unisim::util::debug::Event<ADDRESS> *>& lst, typename unisim::util::debug::Event<ADDRESS>::Type ev_type) const
{
	lst.clear();
	if((ev_type == unisim::util::debug::Event<ADDRESS>::EV_UNKNOWN) || (ev_type == unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT))
	{
		breakpoint_registry.EnumerateBreakpoints(front_end_num, lst);
	}
	if((ev_type == unisim::util::debug::Event<ADDRESS>::EV_UNKNOWN) || (ev_type == unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT))
	{
		watchpoint_registry.EnumerateWatchpoints(front_end_num, lst);
	}
	if((ev_type == unisim::util::debug::Event<ADDRESS>::EV_UNKNOWN) || (ev_type == unisim::util::debug::Event<ADDRESS>::EV_FETCH_INSN))
	{
		unsigned int prc_num;
		for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
		{
			typename std::set<unisim::util::debug::FetchInsnEvent<ADDRESS> *>::const_iterator it;
			for(it = fetch_insn_event_set[prc_num].begin(); it != fetch_insn_event_set[prc_num].end(); it++)
			{
				unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = *it;
				if((unsigned int) fetch_insn_event->GetFrontEndNumber() == front_end_num)
				{
					lst.push_back(fetch_insn_event);
				}
			}
		}
	}
	if((ev_type == unisim::util::debug::Event<ADDRESS>::EV_UNKNOWN) || (ev_type == unisim::util::debug::Event<ADDRESS>::EV_COMMIT_INSN))
	{
		unsigned int prc_num;
		for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
		{
			typename std::set<unisim::util::debug::CommitInsnEvent<ADDRESS> *>::const_iterator it;
			for(it = commit_insn_event_set[prc_num].begin(); it != commit_insn_event_set[prc_num].end(); it++)
			{
				unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event = *it;
				if((unsigned int) commit_insn_event->GetFrontEndNumber() == front_end_num)
				{
					lst.push_back(commit_insn_event);
				}
			}
		}
	}
	if((ev_type == unisim::util::debug::Event<ADDRESS>::EV_UNKNOWN) || (ev_type == unisim::util::debug::Event<ADDRESS>::EV_TRAP))
	{
		unsigned int prc_num;
		for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
		{
			typename std::set<unisim::util::debug::TrapEvent<ADDRESS> *>::const_iterator it;
			for(it = trap_event_set[prc_num].begin(); it != trap_event_set[prc_num].end(); it++)
			{
				unisim::util::debug::TrapEvent<ADDRESS> *trap_event = *it;
				if((unsigned int) trap_event->GetFrontEndNumber() == front_end_num)
				{
					lst.push_back(trap_event);
				}
			}
		}
	}
}

template <typename CONFIG>
void Debugger<CONFIG>::ClearEvents(unsigned int front_end_num)
{
	std::list<unisim::util::debug::Event<ADDRESS> *> lst;
	
	EnumerateListenedEvents(front_end_num, lst, unisim::util::debug::Event<ADDRESS>::EV_UNKNOWN);
	
	typename std::list<unisim::util::debug::Event<ADDRESS> *>::iterator it;
	
	for(it = lst.begin(); it != lst.end(); it++)
	{
		unisim::util::debug::Event<ADDRESS> *event = *it;
		Unlisten(front_end_num, event);
	}
}

template <typename CONFIG>
bool Debugger<CONFIG>::SetBreakpoint(unsigned int front_end_num, ADDRESS addr)
{
	unsigned int prc_num = sel_prc_gate[front_end_num]->GetProcessorNumber();
	bool status = breakpoint_registry.SetBreakpoint(addr, prc_num, front_end_num);
	UpdateReportingRequirements(prc_num);
	return status;
}

template <typename CONFIG>
bool Debugger<CONFIG>::RemoveBreakpoint(unsigned int front_end_num, ADDRESS addr)
{
	unsigned int prc_num = sel_prc_gate[front_end_num]->GetProcessorNumber();
	bool status = breakpoint_registry.RemoveBreakpoint(addr, prc_num, front_end_num);
	UpdateReportingRequirements(prc_num);
	return status;
}

template <typename CONFIG>
bool Debugger<CONFIG>::HasBreakpoints(unsigned int front_end_num, ADDRESS addr)
{
	unsigned int prc_num = sel_prc_gate[front_end_num]->GetProcessorNumber();
	return breakpoint_registry.HasBreakpoints(addr, prc_num, front_end_num);
}

template <typename CONFIG>
bool Debugger<CONFIG>::SetWatchpoint(unsigned int front_end_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, bool overlook)
{
	unsigned int prc_num = sel_prc_gate[front_end_num]->GetProcessorNumber();
	bool status = watchpoint_registry.SetWatchpoint(mat, mt, addr, size, overlook, prc_num, front_end_num);
	UpdateReportingRequirements(prc_num);
	return status;
}

template <typename CONFIG>
bool Debugger<CONFIG>::RemoveWatchpoint(unsigned int front_end_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size)
{
	unsigned int prc_num = sel_prc_gate[front_end_num]->GetProcessorNumber();
	bool status = watchpoint_registry.RemoveWatchpoint(mat, mt, addr, size, prc_num, front_end_num);
	UpdateReportingRequirements(prc_num);
	return status;
}

template <typename CONFIG>
bool Debugger<CONFIG>::HasWatchpoints(unsigned int front_end_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size)
{
	unsigned int prc_num = sel_prc_gate[front_end_num]->GetProcessorNumber();
	return watchpoint_registry.HasWatchpoints(mat, mt, addr, size, prc_num, front_end_num);
}

// unisim::service::interfaces::Disassembly<ADDRESS> (tagged)
template <typename CONFIG>
std::string Debugger<CONFIG>::Disasm(unsigned int front_end_num, ADDRESS addr, ADDRESS& next_addr)
{
	return sel_prc_gate[front_end_num]->Disasm(addr, next_addr);
}

// unisim::service::interfaces::Memory<ADDRESS> (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::ResetMemory(unsigned int front_end_num)
{
	sel_prc_gate[front_end_num]->ResetMemory();
}

template <typename CONFIG>
bool Debugger<CONFIG>::ReadMemory(unsigned int front_end_num, ADDRESS addr, void *buffer, uint32_t size)
{
	return sel_prc_gate[front_end_num]->ReadMemory(addr, buffer, size);
}

template <typename CONFIG>
bool Debugger<CONFIG>::WriteMemory(unsigned int front_end_num, ADDRESS addr, const void *buffer, uint32_t size)
{
	return sel_prc_gate[front_end_num]->WriteMemory(addr, buffer, size);
}


// unisim::service::interfaces::Registers (tagged)
template <typename CONFIG>
unisim::service::interfaces::Register *Debugger<CONFIG>::GetRegister(unsigned int front_end_num, const char *name)
{
	return sel_prc_gate[front_end_num]->GetRegister(name);
}

template <typename CONFIG>
void Debugger<CONFIG>::ScanRegisters(unsigned int front_end_num, unisim::service::interfaces::RegisterScanner& scanner)
{
	return sel_prc_gate[front_end_num]->ScanRegisters(scanner);
}

// unisim::service::interfaces::SymbolTableLookup<ADDRESS> (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::GetSymbols(unsigned int front_end_num, typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>& lst, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			elf32_loader->GetSymbols(lst, type);
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			elf64_loader->GetSymbols(lst, type);
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_coff_loaders[front_end_num][i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			coff_loader->GetSymbols(lst, type);
		}
	}
}

template <typename CONFIG>
const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSymbol(unsigned int front_end_num, const char *name, ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf32_loader->FindSymbol(name, addr, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf64_loader->FindSymbol(name, addr, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_coff_loaders[front_end_num][i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = coff_loader->FindSymbol(name, addr, type);
			if(symbol) return symbol;
		}
	}
	return 0;
}

template <typename CONFIG>
const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSymbolByAddr(unsigned int front_end_num, ADDRESS addr) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf32_loader->FindSymbolByAddr(addr);
			if(symbol) return symbol;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf64_loader->FindSymbolByAddr(addr);
			if(symbol) return symbol;
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_coff_loaders[front_end_num][i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = coff_loader->FindSymbolByAddr(addr);
			if(symbol) return symbol;
		}
	}
	return 0;
}

template <typename CONFIG>
const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSymbolByName(unsigned int front_end_num, const char *name) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf32_loader->FindSymbolByName(name);
			if(symbol) return symbol;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf64_loader->FindSymbolByName(name);
			if(symbol) return symbol;
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_coff_loaders[front_end_num][i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = coff_loader->FindSymbolByName(name);
			if(symbol) return symbol;
		}
	}
	return 0;
}

template <typename CONFIG>
const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSymbolByName(unsigned int front_end_num, const char *name, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf32_loader->FindSymbolByName(name, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf64_loader->FindSymbolByName(name, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_coff_loaders[front_end_num][i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = coff_loader->FindSymbolByName(name, type);
			if(symbol) return symbol;
		}
	}
	return 0;
}

template <typename CONFIG>
const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSymbolByAddr(unsigned int front_end_num, ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf32_loader->FindSymbolByAddr(addr, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf64_loader->FindSymbolByAddr(addr, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_coff_loaders[front_end_num][i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = coff_loader->FindSymbolByAddr(addr, type);
			if(symbol) return symbol;
		}
	}
	return 0;
}


// unisim::service::interfaces::StatementLookup<ADDRESS> (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::GetStatements(unsigned int front_end_num, std::multimap<ADDRESS, const unisim::util::debug::Statement<ADDRESS> *>& stmts) const
{
	typename std::multimap<ADDRESS, const unisim::util::debug::Statement<ADDRESS> *>::const_iterator iter;
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename std::multimap<ADDRESS, const unisim::util::debug::Statement<ADDRESS> *>& elf32_stmts = elf32_loader->GetStatements();
			for(iter = elf32_stmts.begin(); iter != elf32_stmts.end(); iter++)
			{
				stmts.insert(std::pair<ADDRESS, const unisim::util::debug::Statement<ADDRESS> *>((*iter).first, (*iter).second));
			}
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename std::multimap<ADDRESS, const unisim::util::debug::Statement<ADDRESS> *>& elf64_stmts = elf64_loader->GetStatements();
			for(iter = elf64_stmts.begin(); iter != elf64_stmts.end(); iter++)
			{
				stmts.insert(std::pair<ADDRESS, const unisim::util::debug::Statement<ADDRESS> *>((*iter).first, (*iter).second));
			}
		}
	}
}

template <typename CONFIG>
const unisim::util::debug::Statement<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindStatement(unsigned int front_end_num, ADDRESS addr, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const
{
	const unisim::util::debug::Statement<ADDRESS> *ret_stmt = 0;
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf32_loader->FindStatement(addr, opt);
			if(stmt)
			{
				switch(opt)
				{
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEAREST_LOWER_OR_EQUAL_STMT:
						if(stmt->GetAddress() <= addr)
						{
							if(!ret_stmt || ((addr - stmt->GetAddress()) < (addr - ret_stmt->GetAddress()))) ret_stmt = stmt;
						}
						break;
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEXT_STMT:
						if(stmt->GetAddress() > addr)
						{
							if(!ret_stmt || ((stmt->GetAddress() - addr) < (ret_stmt->GetAddress() - addr))) ret_stmt = stmt;
						}
						break;
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_EXACT_STMT:
						return stmt;
						break;
				}
			}
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf64_loader->FindStatement(addr, opt);
			if(stmt)
			{
				switch(opt)
				{
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEAREST_LOWER_OR_EQUAL_STMT:
						if(stmt->GetAddress() <= addr)
						{
							if(!ret_stmt || ((addr - stmt->GetAddress()) < (addr - ret_stmt->GetAddress()))) ret_stmt = stmt;
						}
						break;
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEXT_STMT:
						if(stmt->GetAddress() > addr)
						{
							if(!ret_stmt || ((stmt->GetAddress() - addr) < (ret_stmt->GetAddress() - addr))) ret_stmt = stmt;
						}
						break;
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_EXACT_STMT:
						return stmt;
						break;
				}
			}
		}
	}
	
	return ret_stmt;
}

template <typename CONFIG>
const unisim::util::debug::Statement<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindStatements(unsigned int front_end_num, std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, ADDRESS addr, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const
{
	const unisim::util::debug::Statement<ADDRESS> *ret_stmt = 0;
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf32_loader->FindStatements(stmts, addr, opt);
			if(stmt)
			{
				switch(opt)
				{
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEAREST_LOWER_OR_EQUAL_STMT:
						if(stmt->GetAddress() <= addr)
						{
							if(!ret_stmt || ((addr - stmt->GetAddress()) < (addr - ret_stmt->GetAddress()))) ret_stmt = stmt;
						}
						break;
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEXT_STMT:
						if(stmt->GetAddress() > addr)
						{
							if(!ret_stmt || ((stmt->GetAddress() - addr) < (ret_stmt->GetAddress() - addr))) ret_stmt = stmt;
						}
						break;
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_EXACT_STMT:
						return stmt;
						break;
				}
			}
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf64_loader->FindStatements(stmts, addr, opt);
			if(stmt)
			{
				switch(opt)
				{
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEAREST_LOWER_OR_EQUAL_STMT:
						if(stmt->GetAddress() <= addr)
						{
							if(!ret_stmt || ((addr - stmt->GetAddress()) < (addr - ret_stmt->GetAddress()))) ret_stmt = stmt;
						}
						break;
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEXT_STMT:
						if(stmt->GetAddress() > addr)
						{
							if(!ret_stmt || ((stmt->GetAddress() - addr) < (ret_stmt->GetAddress() - addr))) ret_stmt = stmt;
						}
						break;
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_EXACT_STMT:
						return stmt;
						break;
				}
			}
		}
	}
	
	return ret_stmt;
}

template <typename CONFIG>
const unisim::util::debug::Statement<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindStatement(unsigned int front_end_num, const char *filename, unsigned int lineno, unsigned int colno) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf32_loader->FindStatement(filename, lineno, colno);
			if(stmt) return stmt;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf64_loader->FindStatement(filename, lineno, colno);
			if(stmt) return stmt;
		}
	}
	return 0;
}

template <typename CONFIG>
const unisim::util::debug::Statement<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindStatements(unsigned int front_end_num, std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, const char *filename, unsigned int lineno, unsigned int colno) const
{
	const typename unisim::util::debug::Statement<ADDRESS> *ret = 0;
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf32_loader->FindStatements(stmts, filename, lineno, colno);
			
			if(!ret) ret = stmt;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf64_loader->FindStatements(stmts, filename, lineno, colno);
			
			if(!ret) ret = stmt;
		}
	}
	
	return ret;
}

// unisim::service::interfaces::BackTrace<ADDRESS> (tagged)
template <typename CONFIG>
std::vector<typename CONFIG::ADDRESS> *Debugger<CONFIG>::GetBackTrace(unsigned int front_end_num, ADDRESS pc) const
{
	unsigned int prc_num = sel_prc_gate[front_end_num]->GetProcessorNumber();
	
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			std::vector<ADDRESS> *backtrace = elf32_loader->GetBackTrace(prc_num, pc);
			if(backtrace) return backtrace;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			std::vector<ADDRESS> *backtrace = elf64_loader->GetBackTrace(prc_num, pc);
			if(backtrace) return backtrace;
		}
	}
	
	return 0;
}

template <typename CONFIG>
bool Debugger<CONFIG>::GetReturnAddress(unsigned int front_end_num, ADDRESS pc, ADDRESS& ret_addr) const
{
	unsigned int prc_num = sel_prc_gate[front_end_num]->GetProcessorNumber();

	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			if(elf32_loader->GetReturnAddress(prc_num, pc, ret_addr)) return true;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			if(elf64_loader->GetReturnAddress(prc_num, pc, ret_addr)) return true;
		}
	}
	
	return false;
}


// unisim::service::interfaces::DebugInfoLoading (tagged)
template <typename CONFIG>
bool Debugger<CONFIG>::LoadDebugInfo(unsigned int front_end_num, const char *filename)
{
	uint8_t magic[8];
	
	std::ifstream f(filename, std::ifstream::in | std::ifstream::binary);
	
	if(f.fail())
	{
		logger << DebugError << "Can't open input \"" << filename << "\"" << EndDebugError;
		return false;
	}

	// Note: code below is nearly equivalent to istream::readsome
	// I no longer use it because it is bugged with i586-mingw32msvc-g++ (version 4.2.1-sjlj on Ubuntu)
	unsigned int size;
	for(size = 0; size < sizeof(magic); size++)
	{
		f.read((char *) &magic[size], 1);
		if(!f.good()) break;
	}
	
	if(size >= 2)
	{
		if(((magic[0] == 0xc1) && (magic[1] == 0x00))
		|| ((magic[0] == 0xc2) && (magic[1] == 0x00))
		|| ((magic[0] == 0x92) && (magic[1] == 0x00))
		|| ((magic[0] == 0x93) && (magic[1] == 0x00))
		|| ((magic[0] == 0x95) && (magic[1] == 0x00))
		|| ((magic[0] == 0x98) && (magic[1] == 0x00))
		|| ((magic[0] == 0x99) && (magic[1] == 0x00))
		|| ((magic[0] == 0x9d) && (magic[1] == 0x00))
		|| ((magic[0] == 0x00) && (magic[1] == 0xc1))
		|| ((magic[0] == 0x00) && (magic[1] == 0xc2))
		|| ((magic[0] == 0x00) && (magic[1] == 0x92))
		|| ((magic[0] == 0x00) && (magic[1] == 0x93))
		|| ((magic[0] == 0x00) && (magic[1] == 0x95))
		|| ((magic[0] == 0x00) && (magic[1] == 0x98))
		|| ((magic[0] == 0x00) && (magic[1] == 0x99))
		|| ((magic[0] == 0x00) && (magic[1] == 0x9d)))
		{
			// TI COFF file detected
			unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = new unisim::util::loader::coff_loader::CoffLoader<ADDRESS>(logger.DebugInfoStream(), logger.DebugWarningStream(), logger.DebugErrorStream());
			
			coff_loader->SetOption(unisim::util::loader::coff_loader::OPT_FILENAME, filename);
			coff_loader->SetOption(unisim::util::loader::coff_loader::OPT_VERBOSE, verbose);
			
			if(!coff_loader->Load())
			{
				logger << DebugError << "Loading input \"" << filename << "\" failed" << EndDebugError;
				delete coff_loader;
				return false;
			}
			coff_loaders.push_back(coff_loader);
			unsigned int i;
			for(i = 0; i < MAX_FRONT_ENDS; i++)
			{
				enable_coff_loaders[front_end_num].push_back((front_end_num == i));
			}
			return true;
		}
	}

	if(size >= 5)
	{
		if((magic[0] == 0x7f) && (magic[1] == 'E') && (magic[2] == 'L') && (magic[3] == 'F'))
		{
			// ELF file detected
			switch(magic[4])
			{
				case 1:
					{
						unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = new unisim::util::loader::elf_loader::Elf32Loader<ADDRESS>();
						
						elf32_loader->SetDebugInfoStream(logger.DebugInfoStream());
						elf32_loader->SetDebugWarningStream(logger.DebugWarningStream());
						elf32_loader->SetDebugErrorStream(logger.DebugErrorStream());
						unsigned int prc_num;
						for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
						{
							elf32_loader->SetRegistersInterface(prc_num, prc_gate[prc_num]->registers_import);
							elf32_loader->SetMemoryInterface(prc_num, prc_gate[prc_num]->memory_import);
						}
						elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_FILENAME, filename);
						elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose);
						elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf);
						elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_REGISTER_NUMBER_MAPPING_FILENAME, unisim::kernel::Object::GetSimulator()->SearchSharedDataFile(dwarf_register_number_mapping_filename.c_str()).c_str());
						elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_DEBUG_DWARF, debug_dwarf);
						
						if(!elf32_loader->Load())
						{
							logger << DebugError << "Loading input \"" << filename << "\" failed" << EndDebugError;
							delete elf32_loader;
							return false;
						}
						elf32_loaders.push_back(elf32_loader);
						unsigned int i;
						for(i = 0; i < MAX_FRONT_ENDS; i++)
						{
							enable_elf32_loaders[i].push_back(front_end_num == i);
						}
						return true;
					}
					break;
				case 2:
					{
						unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = new unisim::util::loader::elf_loader::Elf64Loader<ADDRESS>();
						
						elf64_loader->SetDebugInfoStream(logger.DebugInfoStream());
						elf64_loader->SetDebugWarningStream(logger.DebugWarningStream());
						elf64_loader->SetDebugErrorStream(logger.DebugErrorStream());
						unsigned int prc_num;
						for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
						{
							elf64_loader->SetRegistersInterface(prc_num, prc_gate[prc_num]->registers_import);
							elf64_loader->SetMemoryInterface(prc_num, prc_gate[prc_num]->memory_import);
						}
						elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_FILENAME, filename);
						elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose);
						elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf);
						elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_REGISTER_NUMBER_MAPPING_FILENAME, unisim::kernel::Object::GetSimulator()->SearchSharedDataFile(dwarf_register_number_mapping_filename.c_str()).c_str());
						elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_DEBUG_DWARF, debug_dwarf);

						if(!elf64_loader->Load())
						{
							logger << DebugError << "Loading input \"" << filename << "\" failed" << EndDebugError;
							delete elf64_loader;
							return false;
						}
						elf64_loaders.push_back(elf64_loader);
						unsigned int i;
						for(i = 0; i < MAX_FRONT_ENDS; i++)
						{
							enable_elf64_loaders[i].push_back(front_end_num == i);
						}
						return true;
					}
					break;
			}
		}
	}
	logger << DebugWarning << "Can't handle symbol table of input \"" << filename << "\"" << EndDebugWarning;
	return false;
}

template <typename CONFIG>
bool Debugger<CONFIG>::EnableBinary(unsigned int front_end_num, const char *filename, bool enable)
{
	bool found = false;
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf32_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				enable_elf32_loaders[front_end_num][i] = enable;
				found = true;
			}
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf64_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				enable_elf64_loaders[front_end_num][i] = enable;
				found = true;
			}
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = coff_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				enable_coff_loaders[front_end_num][i] = enable;
				found = true;
			}
		}
	}
	
	return found;
}

template <typename CONFIG>
void Debugger<CONFIG>::EnumerateBinaries(unsigned int front_end_num, std::list<std::string>& lst) const
{
	// Note: For now all front-ends see all binaries
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf32_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			lst.push_back(std::string(blob->GetFilename()));
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf64_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			lst.push_back(std::string(blob->GetFilename()));
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = coff_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			lst.push_back(std::string(blob->GetFilename()));
		}
	}
}

template <typename CONFIG>
bool Debugger<CONFIG>::IsBinaryEnabled(unsigned int front_end_num, const char *filename) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf32_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				return enable_elf32_loaders[front_end_num][i];
			}
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf64_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				return enable_elf64_loaders[front_end_num][i];
			}
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = coff_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				return enable_coff_loaders[front_end_num][i];
			}
		}
	}
	
	return false;
}


// unisim::service::interfaces::DataObjectLookup<ADDRESS> (tagged)
template <typename CONFIG>
unisim::util::debug::DataObject<typename CONFIG::ADDRESS> *Debugger<CONFIG>::GetDataObject(unsigned int front_end_num, const char *data_object_name, const char *filename, const char *compilation_unit_name) const
{
	unsigned int prc_num = sel_prc_gate[front_end_num]->GetProcessorNumber();

	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			unisim::util::debug::DataObject<ADDRESS> *data_object = elf32_loader->GetDataObject(prc_num, data_object_name, filename, compilation_unit_name);
			if(data_object) return data_object;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			unisim::util::debug::DataObject<ADDRESS> *data_object = elf64_loader->GetDataObject(prc_num, data_object_name, filename, compilation_unit_name);
			if(data_object) return data_object;
		}
	}
	
	return 0;
}

template <typename CONFIG>
unisim::util::debug::DataObject<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindDataObject(unsigned int front_end_num, const char *data_object_name, ADDRESS pc) const
{
	unsigned int prc_num = sel_prc_gate[front_end_num]->GetProcessorNumber();

	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			unisim::util::debug::DataObject<ADDRESS> *data_object = elf32_loader->FindDataObject(prc_num, data_object_name, pc);
			if(data_object) return data_object;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			unisim::util::debug::DataObject<ADDRESS> *data_object = elf64_loader->FindDataObject(prc_num, data_object_name, pc);
			if(data_object) return data_object;
		}
	}
	
	return 0;
}

template <typename CONFIG>
void Debugger<CONFIG>::EnumerateDataObjectNames(unsigned int front_end_num, std::set<std::string>& name_set, ADDRESS pc, typename unisim::service::interfaces::DataObjectLookup<ADDRESS>::Scope scope) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			elf32_loader->EnumerateDataObjectNames(name_set, pc, scope);
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			elf64_loader->EnumerateDataObjectNames(name_set, pc, scope);
		}
	}
}


// unisim::service::interfaces::SubProgramLookup<ADDRESS> (tagged)
template <typename CONFIG>
const unisim::util::debug::SubProgram<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSubProgram(unsigned int front_end_num, const char *subprogram_name, const char *filename, const char *compilation_unit_name) const
{
	unsigned int prc_num = sel_prc_gate[front_end_num]->GetProcessorNumber();

	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const unisim::util::debug::SubProgram<ADDRESS> *subprogram = elf32_loader->FindSubProgram(prc_num, subprogram_name, filename, compilation_unit_name);
			if(subprogram) return subprogram;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const unisim::util::debug::SubProgram<ADDRESS> *subprogram = elf64_loader->FindSubProgram(prc_num, subprogram_name, filename, compilation_unit_name);
			if(subprogram) return subprogram;
		}
	}
	
	return 0;
}

template <typename CONFIG>
void Debugger<CONFIG>::Lock()
{
	pthread_mutex_lock(&mutex);
}

template <typename CONFIG>
void Debugger<CONFIG>::Unlock()
{
	pthread_mutex_unlock(&mutex);
}

} // end of namespace debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
