/*
 *  Copyright (c) 2007,
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
 
#ifndef __UNISIM_SERVICE_DEBUG_INLINE_DEBUGGER_INLINE_DEBUGGER_TCC_
#define __UNISIM_SERVICE_DEBUG_INLINE_DEBUGGER_INLINE_DEBUGGER_TCC_

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <set>
#include <stdexcept>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/debug/data_object_initializer.hh>
#include <unisim/util/debug/data_object_initializer.tcc>
#include <unisim/util/likely/likely.hh>

#if defined(HAVE_CONFIG_H)
//#include "unisim/service/debug/inline_debugger/config.h"
#include "config.h"
#endif

#if defined(HAVE_LIBEDIT)
#include <editline/readline.h>
#endif

#include <unisim/util/endian/endian.hh>
#include <fstream>
#include <iostream>

namespace unisim {
namespace service {
namespace debug {
namespace inline_debugger {

template <class ADDRESS>
InlineDebugger<ADDRESS>::InlineDebugger(const char *_name, Object *_parent)
	: Object(_name, _parent, "this service implements a built-in debugger in the terminal console")
	, InlineDebuggerBase(_name, _parent)
	, unisim::kernel::Service<unisim::service::interfaces::DebugYielding>(_name, _parent)
	, unisim::kernel::Service<unisim::service::interfaces::DebugEventListener<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugYieldingRequest>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Disassembly<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Registers>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::StatementLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::BackTrace<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Profiling<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugInfoLoading>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DataObjectLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::SubProgramLookup<ADDRESS> >(_name, _parent)
	, debug_yielding_export("debug-yielding-export", this)
	, debug_event_listener_export("debug-event-listener-export", this)
	, debug_yielding_request_import("debug-yielding-request-import", this)
	, debug_event_trigger_import("debug-event-trigger-import", this)
	, disasm_import("disasm-import", this)
	, memory_import("memory-import", this)
	, registers_import("registers-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, stmt_lookup_import("stmt-lookup-import", this)
	, backtrace_import("backtrace-import", this)
	, profiling_import("profiling-import", this)
	, debug_info_loading_import("debug-info-loading-import", this)
	, data_object_lookup_import("data-object-lookup-import", this)
	, subprogram_lookup_import("subprogram-lookup-import", this)
	, logger(*this)
	, memory_atom_size(1)
	, init_macro()
	, output()
	, program_counter_name("pc")
	, param_memory_atom_size("memory-atom-size", this, memory_atom_size, "size of the smallest addressable element in memory")
	, param_init_macro("init-macro", this, init_macro, "path to initial macro to run when debugger starts")
	, param_output("output", this, output, "path to output file where to redirect the debugger outputs")
	, param_program_counter_name("program-counter-name", this, program_counter_name, "name of program counter")
	, program_counter(0)
	, listening_fetch(false)
	, listening_trap(false)
	, trap(false)
	, running_mode(INLINE_DEBUGGER_MODE_WAITING_USER)
	, cia(0)
	, disasm_addr(0)
	, dump_addr(0)
	, cont_until_addr(0)
	, last_stmt(0)
	, prompt(std::string(_name) + "> ")
	, output_stream(0)
	, std_output_stream(&std::cout)
	, std_error_stream(&std::cerr)
	, tracked_data_objects()
	, fetch_insn_event(0)
	, trap_event(0)
	, visited_instructions()
	, is_started(false)
{
	param_memory_atom_size.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	
	fetch_insn_event = new unisim::util::debug::FetchInsnEvent<ADDRESS>();
	fetch_insn_event->Catch();
	
	trap_event = new unisim::util::debug::TrapEvent<ADDRESS>();
	trap_event->Catch();
}

template <class ADDRESS>
InlineDebugger<ADDRESS>::~InlineDebugger()
{
	if(output_stream)
	{
		delete output_stream;
	}
	
	typename std::list<unisim::util::debug::DataObject<ADDRESS> *>::iterator it;
	
	for(it = tracked_data_objects.begin(); it != tracked_data_objects.end(); it++)
	{
		delete (*it);
	}
	
	fetch_insn_event->Release();
	
	if(listening_trap)
	{
		if(debug_event_trigger_import)
		{
			if(debug_event_trigger_import->Unlisten(trap_event))
			{
				listening_trap = false;
			}
			else
			{
				(*std_output_stream) << "Can't unlisten trap events" << std::endl;
			}
		}
	}
	
	trap_event->Release();
}

template<class ADDRESS>
bool InlineDebugger<ADDRESS>::EndSetup()
{
	if(!output.empty())
	{
		output_stream = new std::ofstream(output.c_str(), std::ofstream::out);
		
		if(output_stream->fail())
		{
			std::cerr << "ERROR! Can't open output \"" << output << "\"" << std::endl;
			delete output_stream;
			output_stream = 0;
			return false;
		}
		
		std_output_stream = output_stream;
		std_error_stream = output_stream;
	}
	else
	{
		std_output_stream = &std::cout;
		std_error_stream = &std::cerr;
	}

	if(memory_atom_size != 1 &&
	   memory_atom_size != 2 &&
	   memory_atom_size != 4 &&
	   memory_atom_size != 8 &&
	   memory_atom_size != 16)
	{
		std::cerr << this->GetName() << "ERROR! memory-atom-size must be either 1, 2, 4, 8 or 16" << std::endl;
		return false;
	}
	
	if(!init_macro.empty())
	{
		LoadMacro(init_macro.c_str());
	}
	
	Interrupt();
	
	if(!registers_import) return false;
	
	program_counter = registers_import->GetRegister(program_counter_name.c_str());
	
	if(!program_counter)
	{
		std::cerr << this->GetName() << "ERROR! can't access program counter" << std::endl;
		return false;
	}
	
	if(debug_event_trigger_import)
	{
		if(!debug_event_trigger_import->Listen(trap_event))
		{
			(*std_output_stream) << "Can't listen trap events" << std::endl;
			return false;
		}
		listening_trap = true;
	}
	
	
	return true;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::OnDisconnect()
{
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::Interrupt()
{
	trap = true;
	
	if(debug_yielding_request_import)
	{
		debug_yielding_request_import->DebugYieldRequest();
	}
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::ListenFetch()
{
	if(!fetch_insn_event) return false;
	if(listening_fetch) return true;
	
	if(debug_event_trigger_import)
	{
		if(debug_event_trigger_import->Listen(fetch_insn_event))
		{
			listening_fetch = true;
			return true;
		}
	}

	(*std_output_stream) << "Can't track fetched instructions" << std::endl;
	return false;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::UnlistenFetch()
{
	if(!fetch_insn_event) return false;
	if(!listening_fetch) return true;

	if(debug_event_trigger_import)
	{
		if(debug_event_trigger_import->Unlisten(fetch_insn_event))
		{
			listening_fetch = false;
			return true;
		}
	}

	(*std_output_stream) << "Can't untrack fetched instructions" << std::endl;
	return false;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
{
	typename unisim::util::debug::Event<ADDRESS>::Type event_type = event->GetType();
	
	if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_FETCH_INSN))
	{
		const unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = static_cast<const unisim::util::debug::FetchInsnEvent<ADDRESS> *>(event);
		
		cia = fetch_insn_event->GetAddress();
		trap = true;
	}
	else if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_COMMIT_INSN))
	{
	}
	else if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT))
	{
		const unisim::util::debug::Breakpoint<ADDRESS> *breakpoint = static_cast<const unisim::util::debug::Breakpoint<ADDRESS> *>(event);
		(*std_output_stream) << "-> Reached " << (*breakpoint) << std::endl;
		trap = true;
	}
	else if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT))
	{
		const unisim::util::debug::Watchpoint<ADDRESS> *watchpoint = static_cast<const unisim::util::debug::Watchpoint<ADDRESS> *>(event);
		(*std_output_stream) << "-> Reached " << (*watchpoint) << std::endl;
		trap = true;
	}
	else if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_TRAP))
	{
		const unisim::util::debug::TrapEvent<ADDRESS> *trap_event = static_cast<const unisim::util::debug::TrapEvent<ADDRESS> *>(event);
		(*std_output_stream) << "-> Received " << (*trap_event) << std::endl;
		trap = true;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::Tokenize(const std::string& str, std::vector<std::string>& tokens)
{
	std::string token;
	std::stringstream sstr(str);

	while(sstr >> token) tokens.push_back(token);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DebugYield()
{
	if(Killed()) return;
	if(!trap) return;
	is_started = true;
	
	// Ctrl-C, breakpoint or watchpoint condition occured
	
	bool recognized = false;
	ADDRESS addr;
	ADDRESS cont_addr;
	unsigned int size;
	ADDRESS next_addr;

	program_counter->GetValue(cia);
	
	if(running_mode == INLINE_DEBUGGER_MODE_CONTINUE_UNTIL)
	{
		DeleteBreakpoint(cont_until_addr);
	}

	if(running_mode == INLINE_DEBUGGER_MODE_STEP)
	{
		const unisim::util::debug::Statement<ADDRESS> *stmt = FindStatement(cia);
		if(!stmt || !stmt->IsBeginningOfSourceStatement() || (stmt == last_stmt)) return;
	}

	if((running_mode == INLINE_DEBUGGER_MODE_STEP_INSTRUCTION) ||
	   (running_mode == INLINE_DEBUGGER_MODE_STEP) ||
	   (running_mode == INLINE_DEBUGGER_MODE_TRAVERSE))
	{
		UnlistenFetch();
	}
		
	PrintTrackedDataObjects();
	
	Disasm(cia, 1, next_addr);

	if(running_mode == INLINE_DEBUGGER_MODE_TRAVERSE)
	{
		if(IsVisited(cia))
		{
			return;
		}
		
		UnlistenFetch();
	}
	
	while(1)
	{
		trap = false;
		bool interactive = false;
		//(*std_output_stream) << "> ";
		if(Killed()) return;
		if(!GetLine(prompt.c_str(), line, interactive))
		{
			this->Stop(0);
			return;
		}

		if(interactive && IsBlankLine(line))
		{
	 		line = last_line;
//			(*std_output_stream) << "\033[1A\033[2C" << line << std::endl;
		}
		
		parm.clear();
		Tokenize(line, parm);
		int nparms = parm.size();
	
		recognized = false;

		switch(nparms)
		{
			case 0:
				if(!interactive) continue;
				break;
			case 1:
				{
					unisim::service::interfaces::Register *reg = registers_import->GetRegister(parm[0].c_str());

					if(reg)
					{
						recognized = true;
						unsigned int size = reg->GetSize();
						uint8_t value[size];
						reg->GetValue(&value);
						int i;
						(*std_output_stream) << reg->GetName() << " = 0x" << std::hex;
#if BYTE_ORDER == BIG_ENDIAN
						for(i = 0; i < (int) size; i++)
#else
						for(i = size - 1; i >= 0; i--)
#endif
						{
							(*std_output_stream) << (value[i] >> 4);
							(*std_output_stream) << (value[i] & 15);
						}
						(*std_output_stream) << std::dec << std::endl;
						break;
					}
				}

				if(IsHelpCommand(parm[0].c_str()))
				{
					recognized = true;
					Help();
					break;
				}

				if(IsQuitCommand(parm[0].c_str()))
				{
					recognized = true;
					this->Stop(0);
					return;
				}

				if(IsStepInstructionCommand(parm[0].c_str()))
				{
					running_mode = INLINE_DEBUGGER_MODE_STEP_INSTRUCTION;
					if(interactive) last_line = line;
					ListenFetch();
					return;
				}

				if(IsTraverseCommand(parm[0].c_str()))
				{
					running_mode = INLINE_DEBUGGER_MODE_TRAVERSE;
					if(interactive) last_line = line;
					ListenFetch();
					return;
				}

				if(IsStepCommand(parm[0].c_str()))
				{
					running_mode = INLINE_DEBUGGER_MODE_STEP;
					if(interactive) last_line = line;
					last_stmt = FindStatement(cia);
					ListenFetch();
					return;
				}

				if(IsNextInstructionCommand(parm[0].c_str()))
				{
					if(interactive) last_line = line;
					disasm_import->Disasm(cia, cont_addr);
					if(HasBreakpoint(cont_addr))
					{
						running_mode = INLINE_DEBUGGER_MODE_CONTINUE;
					}
					else
					{
						running_mode = INLINE_DEBUGGER_MODE_CONTINUE_UNTIL;
						cont_until_addr = cont_addr;
						SetBreakpoint(cont_until_addr);
					}
					return;
				}

				if(IsContinueCommand(parm[0].c_str()))
				{
					running_mode = INLINE_DEBUGGER_MODE_CONTINUE;
					
					if(interactive) last_line = line;
					return;
				}

				if(IsDisasmCommand(parm[0].c_str()))
				{
					recognized = true;
					Disasm(disasm_addr, 10, disasm_addr);
					break;
				}

				if(IsDumpCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpMemory(dump_addr);
					dump_addr += 256;
					break;
				}

				if(IsBreakCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpBreakpoints();
					break;
				}

				if(IsWatchCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpWatchpoints();
					break;
				}
				
				if(IsResetCommand(parm[0].c_str()))
				{
					trap = false;
					last_line.clear();
					line.clear();
					running_mode = INLINE_DEBUGGER_MODE_RESET;
					disasm_addr = 0;
					dump_addr = 0;
					//return DebugControl<ADDRESS>::DBG_RESET;
					// FIXME: should reset
					return;
				}

				if (IsMonitorCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpVariables("m");
					break;
				}
				
				if (IsRegistersCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpRegisters();
					break;
				}

				if (IsRegisterCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpVariables("m", 0, unisim::kernel::VariableBase::VAR_REGISTER);
					break;
				}
				
				if (IsStatisticCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpVariables("m", 0, unisim::kernel::VariableBase::VAR_STATISTIC);
					break;
				}
				
				if (IsParameterCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpVariables("m", 0, unisim::kernel::VariableBase::VAR_PARAMETER);
					break;
				}

				if(IsProfileCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpProgramProfile();
					if(!trap) DumpDataProfile(false /* read */);
					if(!trap) DumpDataProfile(true /* write */);
					break;
				}

				if(IsBackTraceCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpBackTrace();
					break;
				}
				
				if(IsFinishCommand(parm[0].c_str()))
				{
					ADDRESS ret_addr;
					if(GetReturnAddress(ret_addr))
					{
						if(interactive) last_line = line;
						running_mode = INLINE_DEBUGGER_MODE_CONTINUE_UNTIL;
						cont_until_addr = ret_addr;
						SetBreakpoint(cont_until_addr);
						return;
					}
					else
					{
						recognized = true;
						(*std_output_stream) << "Don't know how to return" << std::endl;
					}
					break;
				}
				
				if(IsListSymbolsCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpSymbols();
					break;
				}
				
				if(IsListSourcesCommand(parm[0].c_str()))
				{
					recognized = true;
					ListSourceFiles();
					break;
				}

				if(IsListBinariesCommand(parm[0].c_str()))
				{
					recognized = true;
					ListBinaryFiles();
					break;
				}
				
				if(IsListDataObjectsCommand(parm[0].c_str()))
				{
					recognized = true;
					ListDataObjects();
					break;
				}
				break;
			case 2:
				if(IsDumpDataObjectCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpDataObject(parm[1].c_str());
					break;
				}
				
				if(IsPrintDataObjectCommand(parm[0].c_str()))
				{
					recognized = true;
					PrintDataObject(parm[1].c_str());
					break;
				}

				if(IsEditDataObjectCommand(parm[0].c_str()))
				{
					recognized = true;
					EditDataObject(parm[1].c_str());
					break;
				}

				if(IsListDataObjectsCommand(parm[0].c_str()))
				{
					if(strcmp(parm[1].c_str(), "global") == 0)
					{
						recognized = true;
						ListDataObjects(unisim::service::interfaces::DataObjectLookup<ADDRESS>::SCOPE_GLOBAL_ONLY);
					}
					else if(strcmp(parm[1].c_str(), "local") == 0)
					{
						recognized = true;
						ListDataObjects(unisim::service::interfaces::DataObjectLookup<ADDRESS>::SCOPE_LOCAL_ONLY);
					}
					break;
				}

				if(IsTrackDataObjectCommand(parm[0].c_str()))
				{
					recognized = true;
					TrackDataObject(parm[1].c_str());
					break;
				}

				if(IsUntrackDataObjectCommand(parm[0].c_str()))
				{
					recognized = true;
					UntrackDataObject(parm[1].c_str());
					break;
				}

				if(IsWhatIsCommand(parm[0].c_str()))
				{
					recognized = true;
					PrintDataObjectType(parm[1].c_str());
					break;
				}

				if(IsLoadConfigCommand(parm[0].c_str()))
				{
					recognized = true;
					this->GetSimulator()->LoadVariables(parm[1].c_str(), unisim::kernel::VariableBase::VAR_PARAMETER);
					break;
				}
				
				if(IsEnableBinaryCommand(parm[0].c_str()))
				{
					recognized = true;
					EnableBinary(parm[1].c_str(), true);
					break;
				}

				if(IsDisableBinaryCommand(parm[0].c_str()))
				{
					recognized = true;
					EnableBinary(parm[1].c_str(), false);
					break;
				}

				if(IsDisasmCommand(parm[0].c_str()) && ParseAddr(parm[1].c_str(), disasm_addr))
				{
					recognized = true;
					Disasm(disasm_addr, 10, disasm_addr);
					break;
				}

				if(IsDumpCommand(parm[0].c_str()) && ParseAddr(parm[1].c_str(), dump_addr))
				{
					recognized = true;
					DumpMemory(dump_addr);
					dump_addr += 256;
					break;
				}

				if(IsEditCommand(parm[0].c_str()) && ParseAddr(parm[1].c_str(), addr))
				{
					recognized = true;
					EditMemory(addr);
					break;
				}

				if(IsBreakCommand(parm[0].c_str()) && ParseAddr(parm[1].c_str(), addr))
				{
					recognized = true;
					SetBreakpoint(addr);
					break;
				}

				if(IsWatchCommand(parm[0].c_str()) && ParseAddrRange(parm[1].c_str(), addr, size))
				{
					recognized = true;
					SetWriteWatchpoint(addr, size);
					break;
				}

				if(IsDeleteCommand(parm[0].c_str()) && ParseAddr(parm[1].c_str(), addr))
				{
					recognized = true;
					DeleteBreakpoint(addr);
					break;
				}
				
				if(IsDeleteWatchCommand(parm[0].c_str()) && ParseAddrRange(parm[1].c_str(), addr, size))
				{
					recognized = true;
					DeleteWriteWatchpoint(addr, size);
					break;
				}

				if(IsContinueCommand(parm[0].c_str()) && ParseAddr(parm[1].c_str(), cont_addr))
				{
					running_mode = INLINE_DEBUGGER_MODE_CONTINUE_UNTIL;
					if(interactive) last_line = line;
					cont_until_addr = cont_addr;
					SetBreakpoint(cont_until_addr);
					return;
				}
				
				if (IsMonitorCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpVariables(parm[0].c_str(), parm[1].c_str());
					break;
				}
				
				if (IsRegisterCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpVariables(parm[0].c_str(), parm[1].c_str(), unisim::kernel::VariableBase::VAR_REGISTER);
					break;
				}
				
				if (IsStatisticCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpVariables(parm[0].c_str(), parm[1].c_str(), unisim::kernel::VariableBase::VAR_STATISTIC);
					break;
				}
				
				if (IsParameterCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpVariables(parm[0].c_str(), parm[1].c_str(), unisim::kernel::VariableBase::VAR_PARAMETER);
					break;
				}

				if(IsProfileCommand(parm[0].c_str()))
				{
					if(strcmp(parm[1].c_str(), "on") == 0)
					{
						recognized = true;
						EnableProfiling();
						break;
					}
					else if(strcmp(parm[1].c_str(), "off") == 0)
					{
						recognized = true;
						DisableProfiling();
						break;
					}
					else if(strcmp(parm[1].c_str(), "clear") == 0)
					{
						recognized = true;
						ClearProfile();
						break;
					}
					else if(strcmp(parm[1].c_str(), "status") == 0)
					{
						recognized = true;
						DumpProfilingStatus();
						break;
					}
					else if((strcmp(parm[1].c_str(), "prog") == 0) || (strcmp(parm[1].c_str(), "program") == 0))
					{
						recognized = true;
						DumpProgramProfile();
						break;
					}
					else if(strcmp(parm[1].c_str(), "data") == 0)
					{
						recognized = true;
						DumpDataProfile(false /* read */);
						DumpDataProfile(true /* write */);
						break;
					}
				}

				if(IsLoadSymbolTableCommand(parm[0].c_str()))
				{
					recognized = true;
					LoadSymbolTable(parm[1].c_str());
					break;
				}

				if(IsListSymbolsCommand(parm[0].c_str()))
				{
					recognized = true;
					DumpSymbols(parm[1].c_str());
					break;
				}

				if(IsMacroCommand(parm[0].c_str()))
				{
					recognized = true;
					LoadMacro(parm[1].c_str());
					break;
				}
				
				if(IsInfoSubProgramCommand(parm[0].c_str()))
				{
					recognized = true;
					InfoSubProgram(parm[1].c_str());
				}
				break;
			case 3:
				if(IsWatchCommand(parm[0].c_str()) && ParseAddrRange(parm[1].c_str(), addr, size))
				{
					if(strcmp(parm[2].c_str(), "read") == 0)
					{
						recognized = true;
						SetReadWatchpoint(addr, size);
					}
					if(strcmp(parm[2].c_str(), "write") == 0)
					{
						recognized = true;
						SetWriteWatchpoint(addr, size);
					}
					break;
				}
				
				if(IsDeleteWatchCommand(parm[0].c_str()) && ParseAddrRange(parm[1].c_str(), addr, size))
				{
					if(strcmp(parm[2].c_str(), "read") == 0)
					{
						recognized = true;
						DeleteReadWatchpoint(addr, size);
					}
					if(strcmp(parm[2].c_str(), "write") == 0)
					{
						recognized = true;
						DeleteWriteWatchpoint(addr, size);
					}
					break;
				}
				
				if(IsProfileCommand(parm[0].c_str()))
				{
					if(strcmp(parm[1].c_str(), "data") == 0)
					{
						if(strcmp(parm[2].c_str(), "on") == 0)
						{
							recognized = true;
							EnableDataProfiling();
						}
						else if(strcmp(parm[2].c_str(), "off") == 0)
						{
							recognized = true;
							DisableDataProfiling();
						}
						else if(strcmp(parm[2].c_str(), "clear") == 0)
						{
							recognized = true;
							ClearDataProfile();
						}
						else if(strcmp(parm[2].c_str(), "status") == 0)
						{
							recognized = true;
							DumpDataProfilingStatus();
							break;
						}
						else if(strcmp(parm[2].c_str(), "read") == 0)
						{
							recognized = true;
							DumpDataProfile(false /* read */);
						}
						else if(strcmp(parm[2].c_str(), "write") == 0)
						{
							recognized = true;
							DumpDataProfile(true /* write */);
						}
					}
					else if((strcmp(parm[1].c_str(), "prog") == 0) || (strcmp(parm[1].c_str(), "program") == 0))
					{
						if(strcmp(parm[2].c_str(), "on") == 0)
						{
							recognized = true;
							EnableProgramProfiling();
						}
						else if(strcmp(parm[2].c_str(), "off") == 0)
						{
							recognized = true;
							DisableProgramProfiling();
						}
						else if(strcmp(parm[2].c_str(), "clear") == 0)
						{
							recognized = true;
							ClearProgramProfile();
						}
						else if(strcmp(parm[2].c_str(), "status") == 0)
						{
							recognized = true;
							DumpProgramProfilingStatus();
							break;
						}
					}
				}

				if (IsMonitorCommand(parm[0].c_str(), parm[1].c_str()))
				{
					recognized = true;
					std::stringstream str;
					str << parm[0].c_str();
					str << parm[1].c_str();
					DumpVariables(str.str().c_str(), parm[2].c_str());
					break;
				}
				
				if (IsRegisterCommand(parm[0].c_str(), parm[1].c_str()))
				{
					recognized = true;
					std::stringstream str;
					str << parm[0].c_str();
					str << parm[1].c_str();
					DumpVariables(str.str().c_str(), parm[2].c_str(), unisim::kernel::VariableBase::VAR_REGISTER);
					break;
				}
				
				if (IsStatisticCommand(parm[0].c_str(), parm[1].c_str()))
				{
					recognized = true;
					std::stringstream str;
					str << parm[0].c_str();
					str << parm[1].c_str();
					DumpVariables(str.str().c_str(), parm[2].c_str(), unisim::kernel::VariableBase::VAR_STATISTIC);
					break;
				}
				
				if (IsParameterCommand(parm[0].c_str(), parm[1].c_str()))
				{
					recognized = true;
					std::stringstream str;
					str << parm[0].c_str();
					str << parm[1].c_str();
					DumpVariables(str.str().c_str(), parm[2].c_str(), unisim::kernel::VariableBase::VAR_PARAMETER);
					break;
				}

				if(IsSetDataObjectCommand(parm[0].c_str()))
				{
					recognized = true;
					SetDataObject(parm[1].c_str(), parm[2].c_str());
					break;
				}

				break;
			case 4:
				
				if(IsProfileCommand(parm[0].c_str()))
				{
					if(strcmp(parm[1].c_str(), "data") == 0)
					{
						if(strcmp(parm[2].c_str(), "read") == 0)
						{
							if(strcmp(parm[3].c_str(), "on") == 0)
							{
								recognized = true;
								EnableDataReadProfiling();
							}
							else if(strcmp(parm[3].c_str(), "off") == 0)
							{
								recognized = true;
								DisableDataReadProfiling();
							}
							else if(strcmp(parm[3].c_str(), "clear") == 0)
							{
								recognized = true;
								ClearDataReadProfile();
							}
							else if(strcmp(parm[3].c_str(), "status") == 0)
							{
								recognized = true;
								DumpDataReadProfilingStatus();
								break;
							}
						}
						else if(strcmp(parm[2].c_str(), "write") == 0)
						{
							if(strcmp(parm[3].c_str(), "on") == 0)
							{
								recognized = true;
								EnableDataWriteProfiling();
							}
							else if(strcmp(parm[3].c_str(), "off") == 0)
							{
								recognized = true;
								DisableDataWriteProfiling();
							}
							else if(strcmp(parm[3].c_str(), "clear") == 0)
							{
								recognized = true;
								ClearDataWriteProfile();
							}
							else if(strcmp(parm[3].c_str(), "status") == 0)
							{
								recognized = true;
								DumpDataWriteProfilingStatus();
								break;
							}
						}
					}
				}
				break;
			case EOF:
				recognized = true;
				break;

		} // end of switch

		if((nparms >= 3) && IsMonitorSetCommand(parm[0].c_str()))
		{
			recognized = true;
			std::stringstream sstr;
			int i;
			
			for(i = 2; i < nparms; i++)
			{
				if(i > 2) sstr << " ";
				sstr << parm[i];
			}
			SetVariable(parm[1].c_str(), sstr.str().c_str());
		}

		if(!recognized)
		{
			(*std_output_stream) << "Unrecognized command.  Try \"help\"." << std::endl;
			for (int i = 0; i < nparms; i++)
				(*std_output_stream) << parm[i] << " ";
			(*std_output_stream) << std::endl;
		}
		
		if(interactive) last_line = line;
	}
	this->Stop(0);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::Help()
{
	(*std_output_stream) << "HELP:" << std::endl;
	(*std_output_stream) << "================================ LOAD ==========================================" << std::endl;
	(*std_output_stream) << "<st | symtab> <filename>" << std::endl;
	(*std_output_stream) << "    load symbol table from file <filename>" << std::endl;
	(*std_output_stream) << "=============================== SCRIPTING ======================================" << std::endl;
	(*std_output_stream) << "<mac | macro> <filename>" << std::endl;
	(*std_output_stream) << "    run debugger commands from file named 'filename'" << std::endl;
	(*std_output_stream) << "================================ EXECUTE =======================================" << std::endl;
	(*std_output_stream) << "<c | cont | continue> [<symbol | *address>]" << std::endl;
	(*std_output_stream) << "    continue to execute instructions until program reaches a breakpoint," << std::endl;
	(*std_output_stream) << "    a watchpoint, 'symbol' or 'address'" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<si | stepi>" << std::endl;
	(*std_output_stream) << "    execute one instruction" << std::endl;
	(*std_output_stream) << "<s | step>" << std::endl;
	(*std_output_stream) << "    continue executing instructions until control reaches a different source line" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<traverse>" << std::endl;
	(*std_output_stream) << "    step continuously until control reaches an unvisited instruction" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<ni | nexti>" << std::endl;
	(*std_output_stream) << "    continue to execute instructions until the processor reaches" << std::endl;
	(*std_output_stream) << "    next contiguous instruction, a breakpoint or a watchpoint" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<r | run>" << std::endl;
	(*std_output_stream) << "    restart the simulation from the beginning" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<fin | finish>" << std::endl;
	(*std_output_stream) << "    continue executing instructions until returning from current function" << std::endl;
	(*std_output_stream) << "================================ INSPECT =======================================" << std::endl;
	(*std_output_stream) << "<dis | disasm | disassemble> [<symbol | *address>]" << std::endl;
	(*std_output_stream) << "    continue to disassemble starting from 'symbol', 'address', or after" << std::endl;
	(*std_output_stream) << "    the previous disassembly" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<d | dump> [<symbol | *address>]" << std::endl;
	(*std_output_stream) << "    dump memory starting from 'symbol', 'address', or after the previous dump" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<regs | registers>" << std::endl;
	(*std_output_stream) << "    dump registers" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<bt | backtrace>" << std::endl;
	(*std_output_stream) << "    display backtrace" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<e | ed | edit> <symbol | *address>" << std::endl;
	(*std_output_stream) << "    edit data memory starting from 'symbol' or 'address'" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<register name>" << std::endl;
	(*std_output_stream) << "    display the register value" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<variable name | hierarchical variable name>" << std::endl;
	(*std_output_stream) << "    display all the objects variables that are named as the given variable name," << std::endl;
	(*std_output_stream) << "    or that contain part of the given name" << std::endl;
	(*std_output_stream) << "    note: this command is overriden if the given name matches any of the other" << std::endl;
	(*std_output_stream) << "    inline debugger commands" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<m | monitor>[/<format>] [<variable name>]" << std::endl;
	(*std_output_stream) << "<reg | register>[/<format>] [<register name>]" << std::endl;
	(*std_output_stream) << "<param | parameter>[/<format>] [<parameter name>]" << std::endl;
	(*std_output_stream) << "<stat | statistic>[/<format>] [<statistic name>]" << std::endl;
	(*std_output_stream) << "    display the given simulator variable (register/parameter/statistic), or " << std::endl;
	(*std_output_stream) << "    display all variable (register/parameter/statistic) names if none is given" << std::endl;
	(*std_output_stream) << "    when giving a variable name a format can be used, the available formats are:" << std::endl;
	(*std_output_stream) << "     x     print variable as an integer in hexadecimal format" << std::endl;
	(*std_output_stream) << "     d     print variable as a signed integer" << std::endl;
	(*std_output_stream) << "     u     print variable as an unsigned integer" << std::endl;
	(*std_output_stream) << "     o     print variable as an integer in octal format" << std::endl;
	(*std_output_stream) << "     t     print variable as an integer in binary format" << std::endl;
	(*std_output_stream) << "     a     print variable as an address (equivalent to 'x')" << std::endl;
	(*std_output_stream) << "     c     print variable as a string" << std::endl;
	(*std_output_stream) << "     f     print variable as a float (actually is considered as a double)" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "set <variable name> <value>" << std::endl;
	(*std_output_stream) << "    sets the variable (register/parameter/statistic) to the given value" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<sym | symbol> [<symbol name>]" << std::endl;
	(*std_output_stream) << "    Display information about symbols having 'symbol name' in their name" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<p | prof | profile> on" << std::endl;
	(*std_output_stream) << "<p | prof | profile> prog on" << std::endl;
	(*std_output_stream) << "<p | prof | profile> program on" << std::endl;
	(*std_output_stream) << "<p | prof | profile> data on" << std::endl;
	(*std_output_stream) << "<p | prof | profile> data read on" << std::endl;
	(*std_output_stream) << "<p | prof | profile> data write on" << std::endl;
	(*std_output_stream) << "<p | prof | profile> off" << std::endl;
	(*std_output_stream) << "<p | prof | profile> prog off" << std::endl;
	(*std_output_stream) << "<p | prof | profile> program off" << std::endl;
	(*std_output_stream) << "<p | prof | profile> data off" << std::endl;
	(*std_output_stream) << "<p | prof | profile> data read off" << std::endl;
	(*std_output_stream) << "<p | prof | profile> data write off" << std::endl;
	(*std_output_stream) << "    enable/disable the program and data profiling" << std::endl;
	(*std_output_stream) << "<p | prof | profile> status" << std::endl;
	(*std_output_stream) << "<p | prof | profile> prog status" << std::endl;
	(*std_output_stream) << "<p | prof | profile> program status" << std::endl;
	(*std_output_stream) << "<p | prof | profile> data status" << std::endl;
	(*std_output_stream) << "<p | prof | profile> data read status" << std::endl;
	(*std_output_stream) << "<p | prof | profile> data write status" << std::endl;
	(*std_output_stream) << "    display the program and data profiling status (on/off)" << std::endl;
	(*std_output_stream) << "<p | prof | profile> clear" << std::endl;
	(*std_output_stream) << "<p | prof | profile> prog clear" << std::endl;
	(*std_output_stream) << "<p | prof | profile> program clear" << std::endl;
	(*std_output_stream) << "<p | prof | profile> data clear" << std::endl;
	(*std_output_stream) << "<p | prof | profile> data read clear" << std::endl;
	(*std_output_stream) << "<p | prof | profile> data write clear" << std::endl;
	(*std_output_stream) << "    clear the collected program and data profiles" << std::endl;
	(*std_output_stream) << "<p | prof | profile>" << std::endl;
	(*std_output_stream) << "<p | prof | profile> prog" << std::endl;
	(*std_output_stream) << "<p | prof | profile> program" << std::endl;
	(*std_output_stream) << "<p | prof | profile> data" << std::endl;
	(*std_output_stream) << "<p | prof | profile> data read" << std::endl;
	(*std_output_stream) << "<p | prof | profile> data write" << std::endl;
	(*std_output_stream) << "    display the program/data profile" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<dumpobject | dumobj | dob> <data object name>" << std::endl;
	(*std_output_stream) << "    dump data object bytes" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<printobject | printobj | pob> <data object name>" << std::endl;
	(*std_output_stream) << "    print data object in a programmer friendly manner" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<listobjects | lsobj | lob> [<local>|<global>]" << std::endl;
	(*std_output_stream) << "    list data objects" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<editobject | editobj | eob> <data object name>" << std::endl;
	(*std_output_stream) << "    edit data object bytes" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<setobject | setobj | sob> <data object name>" << std::endl;
	(*std_output_stream) << "    set data object value in a programmer friendly manner" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<trackobject | trackobj | tob> <data object name>" << std::endl;
	(*std_output_stream) << "    track data object and repeatedly print the data object value at each stepping" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<untrackobject | untrackobj | utob> <data object name>" << std::endl;
	(*std_output_stream) << "    untrack data object" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<whatis> <data object name>" << std::endl;
	(*std_output_stream) << "    print data object type" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<info> <subprogram name>" << std::endl;
	(*std_output_stream) << "    print information about a subprogram" << std::endl;
	(*std_output_stream) << "========================= BREAKPOINTS/WATCHPOINTS ==============================" << std::endl;
	(*std_output_stream) << "<b | break> [<symbol | *address | filename:lineno>]" << std::endl;
	(*std_output_stream) << "    set a breakpoint at 'symbol', 'address', or 'filename:lineno'. If 'symbol', 'address'," << std::endl;
	(*std_output_stream) << "    or 'filename:lineno' are not specified, display the breakpoint list" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<w | watch> [<symbol | *address[#<size>]>] [<read | write>]" << std::endl;
	(*std_output_stream) << "    set a watchpoint at 'symbol' or 'address'." << std::endl;
	(*std_output_stream) << "    When using 'continue' and 'next' commands, the debugger will spy CPU loads" << std::endl;
	(*std_output_stream) << "    and stores. The debugger will return to command line prompt once a load," << std::endl;
	(*std_output_stream) << "    or a store will access to 'symbol' or 'address'." << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<del | delete> <symbol | *address | filename:lineno>" << std::endl;
	(*std_output_stream) << "    delete the breakpoint at 'symbol', 'address', or 'filename:lineno'" << std::endl << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<delw | delwatch> <symbol | *address> [<read | write>] [<size>]" << std::endl;
	(*std_output_stream) << "    delete the watchpoint at 'symbol' or 'address'" << std::endl;
	(*std_output_stream) << "============================== MISCELLANEOUS ===================================" << std::endl;
	(*std_output_stream) << "<sources>" << std::endl;
	(*std_output_stream) << "    Locate source files" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<enable | disable> <filename>" << std::endl;
	(*std_output_stream) << "    enable/disable symbolic debugging of binary file <filename>" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<files>" << std::endl;
	(*std_output_stream) << "    list binary files and whether symbolic debugging is enabled or disabled" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<h | ? | help>" << std::endl;
	(*std_output_stream) << "    display the help" << std::endl;
	(*std_output_stream) << "--------------------------------------------------------------------------------" << std::endl;
	(*std_output_stream) << "<quit | q>" << std::endl;
	(*std_output_stream) << "    quit the built-in debugger" << std::endl << std::endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::Disasm(ADDRESS addr, int count, ADDRESS &next_addr)
{
	if(count > 0)
	{
		std::streamsize width = (*std_output_stream).width();
		std::ostream::char_type fill = (*std_output_stream).fill();
		std::ios_base::fmtflags flags = (*std_output_stream).flags();
		
		bool first = true;
		const unisim::util::debug::Symbol<ADDRESS> *last_symbol = 0;
		do
		{
			const unisim::util::debug::Statement<ADDRESS> *stmt = FindStatement(addr, first ? unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEAREST_LOWER_OR_EQUAL_STMT : unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_EXACT_STMT);
			first = false;
			
			if(stmt)
			{
				const char *source_filename = stmt->GetSourceFilename();
				if(source_filename)
				{
					unsigned int lineno = stmt->GetLineNo();
					unsigned int colno = stmt->GetColNo();
					std::string source_path;
					const char *source_dirname = stmt->GetSourceDirname();
					if(source_dirname)
					{
						source_path += source_dirname;
						source_path += '/';
					}
					source_path += source_filename;
					DumpSource(source_path.c_str(), lineno, colno, 1);
				}
			}
			
			(*std_output_stream).fill('0');
			const unisim::util::debug::Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByAddr(addr);
			std::string s = disasm_import->Disasm(addr, next_addr);
			
			if(symbol)
			{
				if(symbol != last_symbol)
				{
					(*std_output_stream) << "0x" << std::hex;
					(*std_output_stream).width((2 * sizeof(ADDRESS)) / memory_atom_size);
					(*std_output_stream) << (addr / memory_atom_size) << std::dec;
					(*std_output_stream) << " <";
					(*std_output_stream) << symbol->GetFriendlyName(addr);
					
					(*std_output_stream) << ">:" << std::endl;
					
					last_symbol = symbol;
				}
			}
			else
			{
				if(symbol != last_symbol)
				{
					(*std_output_stream) << "?";
					last_symbol = symbol;
				}
			}
			(*std_output_stream) << "0x" << std::hex;
			(*std_output_stream).width((2 * sizeof(ADDRESS)) / memory_atom_size);
			(*std_output_stream) << (addr / memory_atom_size) << ":" << std::dec;
			
			(*std_output_stream).fill('0');
			(*std_output_stream) << std::hex;
			while(addr < next_addr)
			{
				uint8_t value = 0;
				if(memory_import->ReadMemory(addr, &value, 1))
				{
					(*std_output_stream).width(2);
					(*std_output_stream) << (unsigned int) value;
				}
				else
				{
					(*std_output_stream) << "??";
				}
				(*std_output_stream) << " ";
				
				addr++;
			}
			
			(*std_output_stream) << " " << s << std::endl;
			(*std_output_stream).fill(' ');
			(*std_output_stream).width(width);
		} while(addr = next_addr, --count > 0);
		
		(*std_output_stream).width(width);
		(*std_output_stream).fill(fill);
		(*std_output_stream).flags(flags);
	}
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::HasBreakpoint(ADDRESS addr)
{
	if(debug_event_trigger_import)
	{
		return debug_event_trigger_import->HasBreakpoints(addr);
	}
	return false;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::SetBreakpoint(ADDRESS addr)
{
	if(debug_event_trigger_import)
	{
		if(debug_event_trigger_import->SetBreakpoint(addr))
		{
			return;
		}
	}
	(*std_output_stream) << "Can't set breakpoint at 0x" << std::hex << addr << std::dec << std::endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::SetReadWatchpoint(ADDRESS addr, uint32_t size)
{
	if(debug_event_trigger_import)
	{
		if(debug_event_trigger_import->SetWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size, true))
		{
			return;
		}
	}
	(*std_output_stream) << "Can't set watchpoint at 0x" << std::hex << addr << std::dec << std::endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::SetWriteWatchpoint(ADDRESS addr, uint32_t size)
{
	if(debug_event_trigger_import)
	{
		if(debug_event_trigger_import->SetWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size, true))
		{
			return;
		}
	}
	(*std_output_stream) << "Can't set watchpoint at 0x" << std::hex << addr << std::dec << std::endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DeleteBreakpoint(ADDRESS addr)
{
	if(debug_event_trigger_import)
	{
		if(debug_event_trigger_import->RemoveBreakpoint(addr))
		{
			return;
		}
	}
	(*std_output_stream) << "Can't remove breakpoint at 0x" << std::hex << addr << std::dec << std::endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DeleteReadWatchpoint(ADDRESS addr, uint32_t size)
{
	if(debug_event_trigger_import)
	{
		if(debug_event_trigger_import->RemoveWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size))
		{
			return;
		}
	}
	(*std_output_stream) << "Can't remove read watchpoint at 0x" << std::hex << addr << std::dec << " (" << size << " bytes)" << std::endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DeleteWriteWatchpoint(ADDRESS addr, uint32_t size)
{
	if(debug_event_trigger_import)
	{
		if(debug_event_trigger_import->RemoveWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size))
		{
			return;
		}
	}
	(*std_output_stream) << "Can't remove write watchpoint at 0x" << std::hex << addr << std::dec << " (" << size << " bytes)" << std::endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpBreakpoints()
{
	std::list<unisim::util::debug::Event<ADDRESS> *> events;
	debug_event_trigger_import->EnumerateListenedEvents(events, unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT);
	typename std::list<unisim::util::debug::Event<ADDRESS> *>::const_iterator iter;

	for(iter = events.begin(); iter != events.end(); iter++)
	{
		unisim::util::debug::Event<ADDRESS> *event = *iter;
		unisim::util::debug::Breakpoint<ADDRESS> *brkp = (unisim::util::debug::Breakpoint<ADDRESS> *) event;
		ADDRESS addr = brkp->GetAddress();
		
		(*std_output_stream) << "*0x" << std::hex << (addr / memory_atom_size) << std::dec << " (";
		
		const unisim::util::debug::Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByAddr(addr);
		
		if(symbol)
		{
			(*std_output_stream) << symbol->GetFriendlyName(addr);
		}
		else
		{
			(*std_output_stream) << "?";
		}
		(*std_output_stream) << ")";
		
		const unisim::util::debug::Statement<ADDRESS> *stmt = FindStatement(addr, unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEAREST_LOWER_OR_EQUAL_STMT);
		
		if(stmt)
		{
			const char *source_filename = stmt->GetSourceFilename();
			if(source_filename)
			{
				unsigned int lineno = stmt->GetLineNo();
				unsigned int colno = stmt->GetColNo();
				std::string source_path;
				const char *source_dirname = stmt->GetSourceDirname();
				if(source_dirname)
				{
					source_path += source_dirname;
					source_path += '/';
				}
				source_path += source_filename;
				(*std_output_stream) << " in ";
				DumpSource(source_path.c_str(), lineno, colno, 1);
			}
		}

		(*std_output_stream) << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpWatchpoints()
{
	std::list<unisim::util::debug::Event<ADDRESS> *> events;
	debug_event_trigger_import->EnumerateListenedEvents(events, unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT);
	typename std::list<unisim::util::debug::Event<ADDRESS> *>::const_iterator iter;

	for(iter = events.begin(); iter != events.end(); iter++)
	{
		unisim::util::debug::Event<ADDRESS> *event = *iter;
		unisim::util::debug::Watchpoint<ADDRESS> *wt = (unisim::util::debug::Watchpoint<ADDRESS> *) event;
		ADDRESS addr = wt->GetAddress();
		uint32_t size = wt->GetSize();
		unisim::util::debug::MemoryAccessType mat = wt->GetMemoryAccessType();
		unisim::util::debug::MemoryType mt = wt->GetMemoryType();
		
		switch(mt)
		{
			case unisim::util::debug::MT_INSN:
				(*std_output_stream) << "insn"; // it should never occur
				break;
			case unisim::util::debug::MT_DATA:
				(*std_output_stream) << "data";
				break;
		}
		(*std_output_stream) << " ";
		switch(mat)
		{
			case unisim::util::debug::MAT_READ:
				(*std_output_stream) << " read";
				break;
			case unisim::util::debug::MAT_WRITE:
				(*std_output_stream) << "write";
				break;
			default:
				(*std_output_stream) << "  (?)";
				break;
		}
		(*std_output_stream) << " ";
		
		(*std_output_stream) << "*0x" << std::hex << (addr / memory_atom_size) << std::dec << "#" << (size / memory_atom_size) << " (";
		
		const unisim::util::debug::Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByAddr(addr);
		
		if(symbol)
		{
			(*std_output_stream) << symbol->GetFriendlyName(addr);
		}
		else
		{
			(*std_output_stream) << "?";
		}
		(*std_output_stream) << ")" << std::endl;
	}
}


template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpMemory(ADDRESS addr)
{
	unsigned int i, j;
	std::streamsize width = (*std_output_stream).width();

	(*std_output_stream).fill(' ');
	(*std_output_stream).width(2 * sizeof(addr));
	(*std_output_stream) << "address" << std::hex;
	(*std_output_stream).width(0);
	for(i = 0; i < 16 / memory_atom_size; i++)
	{
		(*std_output_stream) << "  " << i;
		for(j = 1; j < memory_atom_size; j++) (*std_output_stream) << "   ";
	}
	(*std_output_stream) << std::endl;
	for(i = 0; i < 16; i++)
	{
		(*std_output_stream) << std::hex;
		(*std_output_stream).fill('0');
		(*std_output_stream).width(2 * sizeof(addr));
		(*std_output_stream) << (addr / memory_atom_size) << " ";
		(*std_output_stream).fill(' ');
		for(j = 0; j < 16; j++, addr++)
		{
			uint8_t value = 0;
			if(memory_import->ReadMemory(addr, &value, 1))
			{
				(*std_output_stream) << (uint32_t)(value >> 4);
				(*std_output_stream) << (uint32_t)(value & 15);
			}
			else
			{
				(*std_output_stream) << "??";
			}
			if(j < 15) (*std_output_stream) << " ";
		}
		addr -= 16;
		(*std_output_stream) << std::dec << "  ";
		for(j = 0; j < 16; j++, addr++)
		{
			uint8_t value = 0;
			memory_import->ReadMemory(addr, &value, 1);
			(*std_output_stream) << (char)((value >= ' ' && value < 128) ? value : '.');
		}
		(*std_output_stream) << std::endl;
	}
	(*std_output_stream).width(width);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpRegisters()
{
	struct Printer : unisim::service::interfaces::RegisterScanner
	{
		InlineDebugger<ADDRESS> *inline_debugger;
		
		virtual void Append( unisim::service::interfaces::Register* reg)
		{
			unsigned int size = reg->GetSize();
			uint8_t value[size];
			reg->GetValue(&value);
			int i;
			(*inline_debugger->std_output_stream) << reg->GetName() << " = 0x" << std::hex;
#if BYTE_ORDER == BIG_ENDIAN
			for(i = 0; i < (int) size; i++)
#else
			for(i = size - 1; i >= 0; i--)
#endif
			{
				(*inline_debugger->std_output_stream) << (value[i] >> 4);
				(*inline_debugger->std_output_stream) << (value[i] & 15);
			}
			(*inline_debugger->std_output_stream) << std::dec << std::endl;
		}
	} printer;
	
	printer.inline_debugger = this;
	registers_import->ScanRegisters(printer);
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::EditBuffer(ADDRESS addr, std::vector<uint8_t>& buffer)
{
	std::string line;

	(*std_output_stream) << "Entering edit mode." << std::endl;
	do
	{
		std::stringstream sstr;
		sstr << "0x" << std::hex;
		sstr.fill('0');
		sstr.width(2 * sizeof(addr));
		sstr << addr;
		sstr << ": ";
		
		bool interactive = false;
		if(Killed() || !GetLine(sstr.str().c_str(), line, interactive))
		{
			return false;
		}
	
		if(IsBlankLine(line)) break;

		const char *ptr = line.c_str();
		char *endptr = 0;
		
		do
		{
			uint8_t value = strtoul(ptr, &endptr, 0);
			
			if(ptr == endptr) break;
		
			buffer.push_back(value);
			
			addr++;
			ptr = endptr;
		}
		while(1);
	}
	while(1);
	(*std_output_stream) << "Leaving edit mode." << std::endl;
	return true;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::EditMemory(ADDRESS addr)
{
	std::vector<uint8_t> buffer;
	if(!EditBuffer(addr, buffer)) return false;
	
	unsigned int written = 0;
	unsigned int failed = 0;
	
	unsigned int buffer_size = buffer.size();
	unsigned int i;
	for(i = 0; i < buffer_size; i++, addr++)
	{
		if(memory_import->WriteMemory(addr, &buffer[i], 1))
			written++;
		else
			failed++;
	}
	(*std_output_stream) << written << " of " << (failed + written) << " bytes written" << std::endl;
	return true;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpVariables(const char *cmd, const char *name, typename unisim::kernel::VariableBase::Type type)
{
	bool found = false;
	std::list<unisim::kernel::VariableBase *> lst;
	std::list<unisim::kernel::VariableBase *>::iterator iter;

	GetSimulator()->GetVariables(lst, type);
	
	if(!lst.size()) return;
	
	for(iter = lst.begin(); iter != lst.end(); iter++)
	{
		unisim::kernel::VariableBase *var = *iter;
		std::string var_name(var->GetName());
		
		if(!name || ((*name == '~') && var_name.find(name + 1, 0, strlen(name + 1)) != std::string::npos) || (var_name.compare(name) == 0))
		{
			DumpVariable(cmd, var);
			found = true;
		}
	}
	
	if(!found)
	{
		(*std_output_stream) << "unknown ";
		switch(type)
		{
			case unisim::kernel::VariableBase::VAR_REGISTER:
				(*std_output_stream) << "register";
				break;
			case unisim::kernel::VariableBase::VAR_STATISTIC:
				(*std_output_stream) << "statistic";
				break;
			case unisim::kernel::VariableBase::VAR_PARAMETER:
				(*std_output_stream) << "parameter";
				break;
			default:
				(*std_output_stream) << "variable";
				break;
		}
		(*std_output_stream) << " \"" << ((*name == '~') ? name + 1 : name) << "\"" << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::MonitorGetFormat(const char *cmd, char &format)
{
	unsigned int len = 0;
	
	if (strncmp(cmd, "monitor/", 8) == 0) len = 8;
	if (strncmp(cmd, "m/", 2) == 0) len = 2;
	if (strncmp(cmd, "register/", 9) == 0) len = 9;
	if (strncmp(cmd, "reg/", 4) == 0) len = 4;
	if (strncmp(cmd, "statistic/", 10) == 0) len = 10;
	if (strncmp(cmd, "stat/", 5) == 0) len = 5;
	if (strncmp(cmd, "parameter/", 10) == 0) len = 10;
	if (strncmp(cmd, "param/", 6) == 0) len = 6;
	
	if (len == 0)
	{
		// no format was used, returning default format 'c'
		format = 'c';
		return;
	}
	
	cmd = cmd + len;
	
	// accepted formats = x, d, u, o, t, a, c, f
	if (strcmp(cmd, "x") == 0) 
	{
		format = 'x';
		return;
	}
	if (strcmp(cmd, "d") == 0)
	{
		format = 'd';
		return;
	}
	if (strcmp(cmd, "u") == 0)
	{
		format = 'u';
		return;
	}
	if (strcmp(cmd, "o") == 0)
	{
		format = 'o';
		return;
	}
	if (strcmp(cmd, "t") == 0)
	{
		format = 't';
		return;
	}
	if (strcmp(cmd, "a") == 0)
	{
		format = 'a';
		return;
	}
	if (strcmp(cmd, "c") == 0)
	{
		format = 'c';
		return;
	}
	if (strcmp(cmd, "f") == 0)
	{
		format = 'f';
		return;
	}
	(*std_error_stream) << "WARNING: unknow monitor/register/statistic/parameter format (" << cmd << "). See help for available formats." << std::endl;
	format = 'c';
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpVariable(const char *cmd, const unisim::kernel::VariableBase *variable)
{
	switch (variable->GetType())
	{
		case unisim::kernel::VariableBase::VAR_ARRAY:
			(*std_output_stream) << " A";
			break;
		case unisim::kernel::VariableBase::VAR_PARAMETER:
			(*std_output_stream) << " P";
			break;
		case unisim::kernel::VariableBase::VAR_STATISTIC:
			(*std_output_stream) << " S";
			break;
		case unisim::kernel::VariableBase::VAR_REGISTER:
			(*std_output_stream) << " R";
			break;
		default:
			(*std_output_stream) << " ?";
			break;
	}

	(*std_output_stream) << "\t";
	/* extract the format if any */
	char format = 0;
	MonitorGetFormat(cmd, format);
	
	(*std_output_stream) << variable->GetName() << " = ";
	switch (format)
	{
		case 'x':
			(*std_output_stream) << "0x" << std::hex << ((unsigned long long) *variable) << std::dec;
			break;
		case 'd':
			(*std_output_stream) << ((long long) *variable);
			break;
		case 'u':
			(*std_output_stream) << ((unsigned long long) *variable);
			break;
		case 'o':
			(*std_output_stream).setf(std::ios::oct);
			(*std_output_stream) << "0" << ((unsigned long long) *variable);
			(*std_output_stream).unsetf(std::ios::oct);
			break;
		case 't':
			(*std_output_stream) << "0b";
			for (unsigned int i = 0; i < sizeof(unsigned long long) * 8; i++)
				(*std_output_stream) << (int)((((unsigned long long) *variable) >> i) & 1);
			break;
		case 'a':
			(*std_output_stream) << "0x" << std::hex << ((unsigned long long) *variable) << std::dec;
			break;
		case 'c':
			(*std_output_stream) << ((std::string) *variable);
			break;
		case 'f':
			(*std_output_stream) << ((double) *variable);
			break;
	}
	
	(*std_output_stream) << std::endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpSymbols(const typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>& symbols, const char *name)
{
	bool found = false;
	
	typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>::const_iterator symbol_iter;
	for(symbol_iter = symbols.begin(); symbol_iter != symbols.end(); symbol_iter++)
	{
		const unisim::util::debug::Symbol<ADDRESS> *symbol = *symbol_iter;
		
		std::string symbol_name(symbol->GetName());
		
		if(!name || ((*name == '~') && symbol_name.find(name + 1, 0, strlen(name + 1)) != std::string::npos) || (symbol_name.compare(name) == 0))
		{
			if(!found)
			{
				// print header of table
				(*std_output_stream) << "  ";
				(*std_output_stream).fill(' ');
				(*std_output_stream).width(2 * sizeof(ADDRESS));
				(*std_output_stream) << "Address";
				(*std_output_stream).fill(' ');
				(*std_output_stream).width(1 + (2 * sizeof(ADDRESS)));
				(*std_output_stream) << "Size";
				(*std_output_stream).width(10);
				(*std_output_stream) << "Type";
				(*std_output_stream).width(0);
				(*std_output_stream) << " Name";
				(*std_output_stream) << std::endl;
			}
			
			std::string type_name;
			
			switch(symbol->GetType())
			{
				case unisim::util::debug::Symbol<ADDRESS>::SYM_NOTYPE:
					type_name = "no type";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_OBJECT:
					type_name = "object";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_FUNC:
					type_name = "function";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_SECTION:
					type_name = "section";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_FILE:
					type_name = "file";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_COMMON:
					type_name = "common";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_TLS:
					type_name = "tls";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_NUM:
					type_name = "num";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_LOOS:
					type_name = "loos";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_HIOS:
					type_name = "hios";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_LOPROC:
					type_name = "loproc";
					break;
				case unisim::util::debug::Symbol<ADDRESS>::SYM_HIPROC:
					type_name = "hiproc";
					break;
			}
			
			(*std_output_stream) << std::hex << "0x";
			(*std_output_stream).fill('0');
			(*std_output_stream).width(2 * sizeof(ADDRESS));
			(*std_output_stream) << (symbol->GetAddress() / memory_atom_size) << std::dec;
			(*std_output_stream).fill(' ');
			(*std_output_stream).width(1 + (2 * sizeof(ADDRESS)));
			(*std_output_stream) << (symbol->GetSize() / memory_atom_size);
			(*std_output_stream).width(10);
			(*std_output_stream) << type_name;
			(*std_output_stream).width(0);
			(*std_output_stream) << " " << symbol->GetName();
			(*std_output_stream) << std::endl;
			
			found = true;
		}
	}
	
	if(!found)
	{
		(*std_output_stream) << "unknown symbol \"" << ((*name == '~') ? name + 1 : name) << "\"" << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpSymbols(const char *name)
{
	unsigned int i;
	typename unisim::util::debug::Symbol<ADDRESS>::Type types[] = {
		unisim::util::debug::Symbol<ADDRESS>::SYM_NOTYPE,
		unisim::util::debug::Symbol<ADDRESS>::SYM_OBJECT,
		unisim::util::debug::Symbol<ADDRESS>::SYM_FUNC,
		unisim::util::debug::Symbol<ADDRESS>::SYM_SECTION,
		unisim::util::debug::Symbol<ADDRESS>::SYM_FILE,
		unisim::util::debug::Symbol<ADDRESS>::SYM_COMMON,
		unisim::util::debug::Symbol<ADDRESS>::SYM_TLS,
		unisim::util::debug::Symbol<ADDRESS>::SYM_NUM,
		unisim::util::debug::Symbol<ADDRESS>::SYM_LOOS,
		unisim::util::debug::Symbol<ADDRESS>::SYM_HIOS,
		unisim::util::debug::Symbol<ADDRESS>::SYM_LOPROC,
		unisim::util::debug::Symbol<ADDRESS>::SYM_HIPROC
	};
	
	typename std::list<const unisim::util::debug::Symbol<ADDRESS> *> symbols;
	
	for(i = 0; i < (sizeof(types) / sizeof(types[0])); i++)
	{
		typename unisim::util::debug::Symbol<ADDRESS>::Type type = types[i];

		if(symbol_table_lookup_import)
		{
			symbol_table_lookup_import->GetSymbols(symbols, type);
		}
	}
	
	if(!symbols.empty())
	{
		DumpSymbols(symbols, name);
	}
}


template <class ADDRESS>
void InlineDebugger<ADDRESS>::SetVariable(const char *name, const char *value)
{
	unisim::kernel::VariableBase *variable = GetSimulator()->FindVariable(name);
	
	if (variable->IsVoid())
	{
		(*std_output_stream) << "Unknow variable (" << name << ")" << std::endl;
		return;
	}

	*(variable) = value;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpProgramProfile()
{
	if(!profiling_import) return;
	(*std_output_stream) << "Program profile:" << std::endl;

	const unisim::util::debug::Profile<ADDRESS>& program_profile = profiling_import->GetProfile(unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC);
	typename std::map<ADDRESS, uint64_t> map = program_profile;
	typename std::map<ADDRESS, uint64_t>::const_iterator iter;

	for(iter = map.begin(); !trap && iter != map.end(); iter++)
	{
		ADDRESS addr = (*iter).first;
		const unisim::util::debug::Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByAddr(addr);
		
		ADDRESS next_addr;

		std::string s = disasm_import->Disasm(addr, next_addr);

		(*std_output_stream) << "0x" << std::hex;
		(*std_output_stream).fill('0');
		(*std_output_stream).width(8);
		(*std_output_stream) << (addr / memory_atom_size) << std::dec;

		if(symbol)
		{
			if(symbol)
			{
				(*std_output_stream) << " <";
				(*std_output_stream) << symbol->GetFriendlyName(addr);
				(*std_output_stream) << ">";
			}
		}
		else
		{
			(*std_output_stream) << " <?>";
		}

		(*std_output_stream).fill(' ');
		(*std_output_stream).width(0);
		(*std_output_stream) << ":" << (*iter).second << " times:" << s << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::LoadSymbolTable(const char *filename)
{
	if(!debug_info_loading_import) return;

	std::string match_file_path;
	if(LocateFile(filename, match_file_path))
	{
		if(debug_info_loading_import->LoadDebugInfo(match_file_path.c_str()))
		{
			(*std_output_stream) << "Symbols from \"" << filename << "\" loaded" << std::endl;
		}
		else
		{
			(*std_output_stream) << "No symbols loaded" << std::endl;
		}
	}
	else
	{
		(*std_output_stream) << "Can't locate \"" << filename << "\"" << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::LoadMacro(const char *filename)
{
	std::string path(SearchFile(filename));
	
	std::ifstream f(path.c_str(), std::ifstream::in);
	
	if(f.fail())
	{
		(*std_error_stream) << "WARNING! Can't open macro file \"" << path << "\"" << std::endl;
		return;
	}
	
	std::stack<std::string> macro;
	std::string line;
		
	do
	{
		getline(f, line);
		if(f.bad())
		{
			(*std_error_stream) << "WARNING! I/O error while reading file \"" << path << "\"" << std::endl;
			return;
		}
		if (f.eof()) break;
		macro.push(line);
	} while(!f.eof());
	
	while(!macro.empty())
	{
		const std::string& s = macro.top();
		exec_queue.push_front(s);
		macro.pop();
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpDataProfile(bool write)
{
	if(!profiling_import) return;
	(*std_output_stream) << "Data " << (write ? "write" : "read") << " profile:" << std::endl;
	const unisim::util::debug::Profile<ADDRESS>& data_profile = profiling_import->GetProfile(write ? unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_WRITE : unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_READ);
	typename std::map<ADDRESS, uint64_t> map = data_profile;
	typename std::map<ADDRESS, uint64_t>::const_iterator iter;

	for(iter = map.begin(); !trap && iter != map.end(); iter++)
	{
		ADDRESS addr = (*iter).first;
		const unisim::util::debug::Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByAddr(addr);
		
		(*std_output_stream) << "0x" << std::hex;
		(*std_output_stream).fill('0');
		(*std_output_stream).width(8);
		(*std_output_stream) << (addr / memory_atom_size) << std::dec;

		if(symbol)
		{
			if(symbol)
			{
				(*std_output_stream) << " <";
				(*std_output_stream) << symbol->GetFriendlyName(addr);
				(*std_output_stream) << ">";
			}
		}
		else
		{
			(*std_output_stream) << " <?>";
		}

		(*std_output_stream).fill(' ');
		(*std_output_stream).width(0);
		(*std_output_stream) << ":" << (*iter).second << " times" << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpSource(const char *source_path, unsigned int lineno, unsigned int colno, unsigned int count)
{
	bool match;
	std::string match_source_path;
	
	if((match = LocateFile(source_path, match_source_path)))
	{
		(*std_output_stream) << match_source_path;
	}
	else
	{
		(*std_output_stream) << source_path << " (not found)";
	}
	
	(*std_output_stream) << ":" << lineno;
	(*std_output_stream) << ": ";

	if(match)
	{
		std::ifstream f(match_source_path.c_str(), std::ifstream::in);
		std::string line;
		
		unsigned int n = 1;
		unsigned int printed = 0;
		do
		{
			getline(f, line);
			if(f.fail() || f.eof()) break;
			
			if((n >= lineno) && (n < (lineno + count)))
			{
				(*std_output_stream) << line << std::endl;
				printed++;
			}
			n++;
		} while(printed < count);
	}
	else
	{
		(*std_output_stream) << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpBackTrace()
{
	std::vector<ADDRESS> *backtrace = backtrace_import->GetBackTrace(cia);
	
	if(backtrace)
	{
		unsigned int i;
		unsigned int n = backtrace->size();
		for(i = 0; i < n; i++)
		{
			(*std_output_stream) << "#" << i << " ";
			
			ADDRESS addr = (*backtrace)[i];
			
			const unisim::util::debug::Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByAddr(i == 0 ? addr : (addr - 1));
			
			(*std_output_stream) << "0x" << std::hex;
			(*std_output_stream) << (addr / memory_atom_size) << std::dec;
			if(symbol)
			{
				(*std_output_stream) << " in <";
				(*std_output_stream) << symbol->GetFriendlyName(addr);
				(*std_output_stream) << ">";
			}

			const unisim::util::debug::Statement<ADDRESS> *stmt = FindStatement(i == 0 ? addr : (addr - 1), unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEAREST_LOWER_OR_EQUAL_STMT);
			
			if(stmt)
			{
				const char *source_filename = stmt->GetSourceFilename();
				if(source_filename)
				{
					unsigned int lineno = stmt->GetLineNo();
					std::string source_path;
					const char *source_dirname = stmt->GetSourceDirname();
					if(source_dirname)
					{
						source_path += source_dirname;
						source_path += '/';
					}
					source_path += source_filename;
					(*std_output_stream) << " from ";
					std::string match_source_path;
					if(LocateFile(source_path.c_str(), match_source_path))
					{
						(*std_output_stream) << match_source_path;
					}
					else
					{
						(*std_output_stream) << source_path;
					}
	
					(*std_output_stream) << ":" << lineno;
				}
			}
			
			(*std_output_stream) << std::endl;
		}
		delete backtrace;
	}
	else
	{
		(*std_output_stream) << "No backtrace" << std::endl;
	}
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::GetReturnAddress(ADDRESS& ret_addr) const
{
	return backtrace_import->GetReturnAddress(cia, ret_addr);
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::ParseAddrRange(const char *s, ADDRESS& addr, unsigned int& size)
{
	if(*s == '*')
	{
		std::string str(s + 1);
		
		std::size_t pos = str.find_first_of('#');
		
		std::stringstream sstr_addr(str.substr(0, pos));
		
		if(sstr_addr >> std::hex >> addr)
		{
			if(sstr_addr.eof())
			{
				addr *= memory_atom_size;
			
				if(pos == std::string::npos)
				{
					size = memory_atom_size;
					return true;
				}

				std::stringstream sstr_size(str.substr(pos + 1));
				
				if(sstr_size >> std::dec >> size)
				{
					if(sstr_size.eof())
					{
						size *= memory_atom_size;
						return true;
					}
				}
			}
		}
	}
	
	const unisim::util::debug::Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByName(s);
	
	if(symbol)
	{
		addr = symbol->GetAddress();
		size = symbol->GetSize();
		if(size < 1) size = 1;
		return true;
	}
	
	return false;
}


template <class ADDRESS>
bool InlineDebugger<ADDRESS>::ParseAddr(const char *s, ADDRESS& addr)
{
	if(*s == '*')
	{
		std::string str(s + 1);
		
		std::stringstream sstr_addr(str);
		
		if(sstr_addr >> std::hex >> addr)
		{
			if(!sstr_addr.eof()) return false;
			
			addr *= memory_atom_size;
			return true;
		}
	}

	const unisim::util::debug::Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByName(s);
	
	if(symbol)
	{
		addr = symbol->GetAddress();
		return true;
	}
	
	std::string filename;
	unsigned int lineno;
	while(*s && *s != ':')
	{
		filename += *s;
		s++;
	}
	if(*s == ':') s++;
	
	std::stringstream sstr_lineno(s);
	if(!(sstr_lineno >> lineno)) return false;
	if(!sstr_lineno.eof()) return false;
	
	const unisim::util::debug::Statement<ADDRESS> *stmt = stmt_lookup_import->FindStatement(filename.c_str(), lineno, 0);
	
	if(stmt)
	{
		addr = stmt->GetAddress();
		return true;
	}
	
	return false;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::ParseIntegerValue(const char *s, uint64_t& value)
{
	if(strcmp(s, "true") == 0)
	{
		value = 1;
		return true;
	}
	
	if(strcmp(s, "false") == 0)
	{
		value = 0;
		return true;
	}
	
	bool neg = false;
	
	if(*s == '-')
	{
		neg = true;
		s++;
	}
	
	if((strlen(s) >= 2) && (*s == '0') && ((*(s + 1) == 'x') || (*(s + 1) == 'X')))
	{
		std::string str(s);
		std::stringstream sstr_hex_value(str);
		if(sstr_hex_value >> std::hex >> value)
		{
			if(sstr_hex_value.eof())
			{
				if(neg) value = -value;
				return true;
			}
		}
	}

	std::string str(s);
	std::stringstream sstr_dec_value(str);
	if(sstr_dec_value >> value)
	{
		if(sstr_dec_value.eof())
		{
			if(neg) value = -value;
			return true;
		}
	}

	return false;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::ParseFloatValue(const char *s, unisim::util::ieee754::SoftDouble& value)
{
	std::stringstream sstr(s);

	unisim::util::ieee754::Flags flags;
	value.read(sstr, flags);

	return sstr.eof();
}

template <class ADDRESS>
const unisim::util::debug::Statement<ADDRESS> *InlineDebugger<ADDRESS>::FindStatement(ADDRESS addr, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const
{
	switch(opt)
	{
		case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_EXACT_STMT:
			return stmt_lookup_import->FindStatement(addr, opt);
		case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEAREST_LOWER_OR_EQUAL_STMT:
		case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEXT_STMT:
			{
				const unisim::util::debug::Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByAddr(addr);
				ADDRESS func_start_addr = addr;
				ADDRESS func_end_addr = addr;
				if(symbol)
				{
					ADDRESS symbol_addr = symbol->GetAddress();
					ADDRESS symbol_size = symbol->GetSize();
					typename unisim::util::debug::Symbol<ADDRESS>::Type symbol_type = symbol->GetType();
					if((symbol_size != 0) && (symbol_type == unisim::util::debug::Symbol<ADDRESS>::SYM_FUNC))
					{
						func_start_addr = symbol_addr;
						func_end_addr = symbol_addr + symbol_size - 1;
					}
				}

				const unisim::util::debug::Statement<ADDRESS> *stmt = stmt_lookup_import->FindStatement(addr, opt);
				
				if(stmt && (stmt->GetAddress() >= func_start_addr) && (stmt->GetAddress() <= func_end_addr))
				{
					return stmt;
				}
			}
			break;
	}
	return 0;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ListSourceFiles()
{
	std::set<std::string> source_filenames;
	std::multimap<ADDRESS, const unisim::util::debug::Statement<ADDRESS> *> stmts;
	stmt_lookup_import->GetStatements(stmts);
	typename std::multimap<ADDRESS, const unisim::util::debug::Statement<ADDRESS> *>::const_iterator stmt_iter;
	for(stmt_iter = stmts.begin(); stmt_iter != stmts.end(); stmt_iter++)
	{
		const unisim::util::debug::Statement<ADDRESS> *stmt = (*stmt_iter).second;
		
		const char *source_filename = stmt->GetSourceFilename();
		if(source_filename)
		{
			std::string source_path;
			const char *source_dirname = stmt->GetSourceDirname();
			if(source_dirname)
			{
				source_path += source_dirname;
				source_path += '/';
			}
			source_path += source_filename;
			
			source_filenames.insert(source_path);
		}
	}
	
	std::set<std::string>::const_iterator source_filename_iter;
	for(source_filename_iter = source_filenames.begin(); source_filename_iter != source_filenames.end(); source_filename_iter++)
	{
		const std::string& source_path = (*source_filename_iter);
		std::string match_source_path;
		if(LocateFile(source_path.c_str(), match_source_path))
		{
			(*std_output_stream) << "[OK] " << match_source_path << std::endl;
		}
		else
		{
			(*std_output_stream) << "[EE] " << source_path << " (not found)" << std::endl;
		}
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::EnableBinary(const char *filename, bool enable)
{
	std::string match_file_path;
	const char *file_path;
	if(LocateFile(filename, match_file_path))
	{
		file_path = match_file_path.c_str();
	}
	else
	{
		file_path = filename;
	}
	
	if(debug_info_loading_import->EnableBinary(file_path, enable))
	{
		(*std_output_stream) << "Binary File \"" << file_path << "\" " << (enable ? "enabled" : "disabled") << std::endl;
	}
	else
	{
		(*std_error_stream) << "Can't " << (enable ? "enable" : "disable") << " binary File \"" << file_path << "\"" << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ListBinaryFiles()
{
	std::list<std::string> lst;
	debug_info_loading_import->EnumerateBinaries(lst);
	std::list<std::string>::const_iterator iter;
	for(iter = lst.begin(); iter != lst.end(); iter++)
	{
		const std::string& filename = *iter;
		bool enabled = debug_info_loading_import->IsBinaryEnabled(filename.c_str());
		(*std_output_stream) << filename << ": " << (enabled ? "enabled" : "disabled") << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::EnableProgramProfiling()
{
	if(!profiling_import) return;
	profiling_import->SetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, true);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::EnableDataReadProfiling()
{
	if(!profiling_import) return;
	profiling_import->SetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_READ, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, true);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::EnableDataWriteProfiling()
{
	if(!profiling_import) return;
	profiling_import->SetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_WRITE, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, true);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::EnableDataProfiling()
{
	EnableDataReadProfiling();
	EnableDataWriteProfiling();
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::EnableProfiling()
{
	EnableProgramProfiling();
	EnableDataProfiling();
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DisableProgramProfiling()
{
	if(!profiling_import) return;
	profiling_import->SetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, false);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DisableDataReadProfiling()
{
	if(!profiling_import) return;
	profiling_import->SetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_READ, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, false);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DisableDataWriteProfiling()
{
	if(!profiling_import) return;
	profiling_import->SetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_WRITE, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, false);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DisableDataProfiling()
{
	DisableDataReadProfiling();
	DisableDataWriteProfiling();
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DisableProfiling()
{
	DisableProgramProfiling();
	DisableDataProfiling();
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ClearProgramProfile()
{
	if(!profiling_import) return;
	profiling_import->ClearProfile(unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ClearDataReadProfile()
{
	if(!profiling_import) return;
	profiling_import->ClearProfile(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_READ);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ClearDataWriteProfile()
{
	if(!profiling_import) return;
	profiling_import->ClearProfile(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_WRITE);
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ClearDataProfile()
{
	ClearDataReadProfile();
	ClearDataWriteProfile();
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ClearProfile()
{
	ClearProgramProfile();
	ClearDataProfile();
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpProgramProfilingStatus()
{
	if(!profiling_import) return;
	bool status = false;
	profiling_import->GetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_INSN_EXEC, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, status);
	(*std_output_stream) << "Program profiling is " << (status ? "on" : "off") << std::endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpDataReadProfilingStatus()
{
	if(!profiling_import) return;
	bool status = false;
	profiling_import->GetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_READ, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, status);
	(*std_output_stream) << "Data read profiling is " << (status ? "on" : "off") << std::endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpDataWriteProfilingStatus()
{
	if(!profiling_import) return;
	bool status = false;
	profiling_import->GetProfileOption(unisim::service::interfaces::Profiling<ADDRESS>::PROF_DATA_WRITE, unisim::service::interfaces::Profiling<ADDRESS>::OPT_ENABLE_PROF, status);
	(*std_output_stream) << "Data write profiling is " << (status ? "on" : "off") << std::endl;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpDataProfilingStatus()
{
	DumpDataReadProfilingStatus();
	DumpDataWriteProfilingStatus();
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpProfilingStatus()
{
	DumpProgramProfilingStatus();
	DumpDataProfilingStatus();
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::DumpDataObject(const char *data_object_name)
{
	if(data_object_lookup_import)
	{
		unisim::util::debug::DataObject<ADDRESS> *data_object = data_object_lookup_import->FindDataObject(data_object_name, cia);
		
		if(data_object)
		{
			if(!data_object->IsOptimizedOut())
			{
				if(data_object->Fetch())
				{
					ADDRESS data_object_bit_size = data_object->GetBitSize();
					ADDRESS data_object_byte_size = (data_object_bit_size + 7) / 8;
					unisim::util::endian::endian_type data_object_endian = data_object->GetEndian();
					uint8_t data_object_raw_value[data_object_byte_size];
					memset(data_object_raw_value, 0, data_object_byte_size);
					
					ADDRESS buf_bit_offset = 0;
					if(data_object_endian == unisim::util::endian::E_BIG_ENDIAN)
					{
						ADDRESS l_bit_size = data_object_bit_size % 8;
						buf_bit_offset = l_bit_size ? 8 - l_bit_size : 0;
					}
					if(data_object->Read(0, data_object_raw_value, buf_bit_offset, data_object_bit_size))
					{
						std::ios::fmtflags std_output_stream_saved_flags(std_output_stream->flags());
						
						(*std_output_stream) << data_object_name << " = [ ";
						ADDRESS byte_offset;
						(*std_output_stream).fill('0');

						for(byte_offset = 0; byte_offset < data_object_byte_size; byte_offset++)
						{
							if(byte_offset) (*std_output_stream) << " ";
							(*std_output_stream) << "0x" << std::hex;
							(*std_output_stream).width(2);
							(*std_output_stream) << (unsigned int) data_object_raw_value[byte_offset] << std::dec;
						}
						(*std_output_stream) << " ]";
						
						std_output_stream->flags(std_output_stream_saved_flags);
						
						(*std_output_stream) << std::endl;
					}
					else
					{
						(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" can't be read" << std::endl;
					}
				}
				else
				{
					(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" can't be fetched" << std::endl;
				}
			}
			else
			{
				(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" is optimized out" << std::endl;
			}
				
			delete data_object;
		}
		else
		{
			(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" not found" << std::endl;
		}
	}
	else
	{
		(*std_output_stream) << "Can't lookup data objects" << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::PrintDataObject(unisim::util::debug::DataObject<ADDRESS> *data_object)
{
	const char *data_object_name = data_object->GetName();
	
	if(data_object->Exists())
	{
		if(!data_object->IsOptimizedOut())
		{
			unisim::util::debug::DataObjectInitializer<ADDRESS> data_object_initializer = unisim::util::debug::DataObjectInitializer<ADDRESS>(data_object, cia, data_object_lookup_import);
			(*std_output_stream) << data_object_name << " = " << data_object_initializer << std::endl;
		}
		else
		{
			(*std_output_stream) << data_object_name << " = <optimized out>" << std::endl;
		}
	}
	else
	{
		(*std_output_stream) << data_object_name << " = <unavailable>" << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::PrintDataObject(const char *data_object_name)
{
	if(data_object_lookup_import)
	{
		if(*data_object_name == '&')
		{
			// requesting address of data object
			data_object_name++; // skip '&'
			unisim::util::debug::DataObject<ADDRESS> *data_object = data_object_lookup_import->FindDataObject(data_object_name, cia);
			
			if(data_object)
			{
				ADDRESS data_object_addr = 0;
				if(data_object->GetAddress(data_object_addr))
				{
					(*std_output_stream) << "&" << data_object_name << " = @0x" << std::hex << data_object_addr << std::dec << std::endl;
				}
				else
				{
					(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" has no address" << std::endl;
				}
				delete data_object;
			}
			else
			{
				(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" not found" << std::endl;
			}
		}
		else
		{
			unisim::util::debug::DataObject<ADDRESS> *data_object = data_object_lookup_import->FindDataObject(data_object_name, cia);
			// requesting value of data object
			if(data_object)
			{
				PrintDataObject(data_object);
				delete data_object;
			}
			else
			{
				(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" not found" << std::endl;
			}
		}
	}
	else
	{
		(*std_output_stream) << "Can't lookup data objects" << std::endl;
	}
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::EditDataObject(const char *data_object_name)
{
	bool status = true;

	if(data_object_lookup_import)
	{
		unisim::util::debug::DataObject<ADDRESS> *data_object = data_object_lookup_import->FindDataObject(data_object_name, cia);
		
		if(data_object)
		{
			if(!data_object->IsOptimizedOut())
			{
				if(data_object->Fetch())
				{
					std::vector<uint8_t> buffer;
					if(!EditBuffer(0, buffer)) return false;
					
					ADDRESS data_object_bit_size = data_object->GetBitSize();
					ADDRESS data_object_byte_size = (data_object_bit_size + 7) / 8;
					unisim::util::endian::endian_type data_object_endian = data_object->GetEndian();
					uint8_t data_object_raw_value[data_object_byte_size];
					memset(data_object_raw_value, 0, data_object_byte_size);
					
					ADDRESS buf_bit_offset = 0;
					if(data_object_endian == unisim::util::endian::E_BIG_ENDIAN)
					{
						ADDRESS l_bit_size = data_object_bit_size % 8;
						buf_bit_offset = l_bit_size ? 8 - l_bit_size : 0;
					}

					if(data_object->Read(0, data_object_raw_value, buf_bit_offset, data_object_bit_size))
					{
						unsigned int buffer_size = buffer.size();
						unsigned int i;
						unsigned int n = (data_object_byte_size < buffer_size) ? data_object_byte_size : buffer_size;
						if(data_object_endian == unisim::util::endian::E_BIG_ENDIAN)
						{
							unsigned int j;
							for(i = data_object_byte_size - n, j = 0; j < n; i++, j++)
							{
								data_object_raw_value[i] = buffer[j];
							}
						}
						else
						{
							for(i = 0; i < n; i++)
							{
								data_object_raw_value[i] = buffer[i];
							}
						}
						
						if(data_object->Write(0, data_object_raw_value, buf_bit_offset, data_object_bit_size))
						{
							if(!data_object->Commit())
							{
								status = false;
								(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" can't be committed" << std::endl;
							}
						}
						else
						{
							status = false;
							(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" can't be written" << std::endl;
						}
					}
					else
					{
						status = false;
						(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" can't be read" << std::endl;
					}
				}
				else
				{
					status = false;
					(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" can't be fetched" << std::endl;
				}
			}
			else
			{
				status = false;
				(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" is optimized out" << std::endl;
			}
			
			delete data_object;
		}
		else
		{
			status = false;
			(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" not found" << std::endl;
		}
	}
	else
	{
		status = false;
		(*std_output_stream) << "Can't lookup data objects" << std::endl;
	}
	return status;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::SetDataObject(const char *data_object_name, const char *literal)
{
	bool status = true;

	if(data_object_lookup_import)
	{
		unisim::util::debug::DataObject<ADDRESS> *data_object = data_object_lookup_import->FindDataObject(data_object_name, cia);
		
		if(data_object)
		{
			if(!data_object->IsOptimizedOut())
			{
				if(data_object->Fetch())
				{
					uint64_t data_object_raw_value = 0;
					
					ADDRESS data_object_bit_size = data_object->GetBitSize();
						
					const unisim::util::debug::Type *data_object_type = data_object->GetType();
					
					// follow typedefs
					while(data_object_type->GetClass() == unisim::util::debug::T_TYPEDEF)
					{
						data_object_type = ((unisim::util::debug::Typedef *) data_object_type)->GetType();
					}
					
					switch(data_object_type->GetClass())
					{
						case unisim::util::debug::T_UNKNOWN:
							break;
						case unisim::util::debug::T_CHAR:
						case unisim::util::debug::T_INTEGER:
						case unisim::util::debug::T_BOOL:
						case unisim::util::debug::T_POINTER:
						case unisim::util::debug::T_ENUM:
							if(!ParseIntegerValue(literal, data_object_raw_value))
							{
								status = false;
								(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" only accepts an integral value" << std::endl;
							}
							break;
						case unisim::util::debug::T_FLOAT:
							{
								unisim::util::ieee754::SoftDouble float_value;
								
								if(!ParseFloatValue(literal, float_value))
								{
									status = false;
									(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" only accepts a floating-point value" << std::endl;
									break;
								}
								switch(data_object_bit_size)
								{
									case 32:
										{
											unisim::util::ieee754::Flags flags;
											
											unisim::util::ieee754::SoftFloat sf = unisim::util::ieee754::SoftFloat(float_value, flags);
											data_object_raw_value = sf.queryValue();
										}
										break;
									case 64:
										data_object_raw_value = float_value.queryValue();
										break;
									default:
										(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" can't be set (only 32-bit or 64-bit floating-point values are supported)" << std::endl;
										break;
								}
							}
							break;
						case unisim::util::debug::T_STRUCT:
						case unisim::util::debug::T_UNION:
						case unisim::util::debug::T_CLASS:
						case unisim::util::debug::T_INTERFACE:
						case unisim::util::debug::T_ARRAY:
						case unisim::util::debug::T_TYPEDEF:
						case unisim::util::debug::T_FUNCTION:
						case unisim::util::debug::T_CONST:
						case unisim::util::debug::T_VOID:
						case unisim::util::debug::T_VOLATILE:
							status = false;
							(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" is not a base type" << std::endl;
							break;
					}
					
					if(status)
					{
						if(data_object->Write(0, data_object_raw_value, data_object_bit_size))
						{
							if(!data_object->Commit())
							{
								status = false;
								(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" can't be committed" << std::endl;
							}
						}
						else
						{
							status = false;
							(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" can't be written" << std::endl;
						}
					}
				}
				else
				{
					status = false;
					(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" can't be fetched" << std::endl;
				}
			}
			else
			{
				status = false;
				(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" is optimized out" << std::endl;
			}
			
			delete data_object;
		}
		else
		{
			status = false;
			(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" not found" << std::endl;
		}
	}
	else
	{
		status = false;
		(*std_output_stream) << "Can't lookup data objects" << std::endl;
	}
	return status;
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::ListDataObjects(typename unisim::service::interfaces::DataObjectLookup<ADDRESS>::Scope scope)
{
	if(data_object_lookup_import)
	{
		std::set<std::string> data_object_name_set;
		data_object_lookup_import->EnumerateDataObjectNames(data_object_name_set, cia, scope);
		
		std::set<std::string>::const_iterator it;
		for(it = data_object_name_set.begin(); it != data_object_name_set.end(); it++)
		{
			(*std_output_stream) << (*it) << std::endl;
		}
	}
	else
	{
		(*std_output_stream) << "Can't lookup data objects" << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::TrackDataObject(const char *data_object_name)
{
	if(data_object_lookup_import)
	{
		unisim::util::debug::DataObject<ADDRESS> *data_object = data_object_lookup_import->FindDataObject(data_object_name, cia);
		
		if(data_object)
		{
			tracked_data_objects.push_back(data_object);
		}
		else
		{
			(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" not found" << std::endl;
		}
	}
	else
	{
		(*std_output_stream) << "Can't lookup data objects" << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::UntrackDataObject(const char *data_object_name)
{
	typename std::list<unisim::util::debug::DataObject<ADDRESS> *>::iterator it;
	
	for(it = tracked_data_objects.begin(); it != tracked_data_objects.end(); it++)
	{
		if(strcmp((*it)->GetName(), data_object_name) == 0)
		{
			it = tracked_data_objects.erase(it);
		}
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::PrintTrackedDataObjects()
{
	typename std::list<unisim::util::debug::DataObject<ADDRESS> *>::iterator it;
	
	for(it = tracked_data_objects.begin(); it != tracked_data_objects.end(); it++)
	{
		unisim::util::debug::DataObject<ADDRESS> *data_object = *it;
		
		data_object->Seek(cia);
		
		PrintDataObject(data_object);
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::PrintDataObjectType(unisim::util::debug::DataObject<ADDRESS> *data_object)
{
	const char *data_object_name = data_object->GetName();
	
	if(data_object->Exists())
	{
		const unisim::util::debug::Type *data_object_type = data_object->GetType();
		(*std_output_stream) << data_object_type->BuildCDecl(data_object_name) << ";" << std::endl;
	}
	else
	{
		(*std_output_stream) << "Type of Data object \"" << data_object_name << "\" is not available" << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::PrintDataObjectType(const char *data_object_name)
{
	unisim::util::debug::DataObject<ADDRESS> *data_object = data_object_lookup_import->FindDataObject(data_object_name, cia);
	
	if(data_object)
	{
		PrintDataObjectType(data_object);
	}
	else
	{
		(*std_output_stream) << "At PC=0x" << std::hex << cia << std::dec << ", Data object \"" << data_object_name << "\" not found" << std::endl;
	}
}

template <class ADDRESS>
void InlineDebugger<ADDRESS>::InfoSubProgram(const char *subprogram_name)
{
	const unisim::util::debug::SubProgram<ADDRESS> *subprogram = subprogram_lookup_import->FindSubProgram(subprogram_name);
	
	if(subprogram)
	{
		(*std_output_stream) << "Subprogram \"" << subprogram_name << "\" found" << std::endl;
		(*std_output_stream) << "Arity: " << subprogram->GetArity() << std::endl;
		for(unsigned i = 0; i < subprogram->GetArity(); i++)
		{
			const unisim::util::debug::FormalParameter *formal_param = subprogram->GetFormalParameter(i);
			
			const char *formal_param_name = formal_param->GetName();
			const unisim::util::debug::Type *formal_param_type = formal_param->GetType();
			(*std_output_stream) << "Type of parameter #" << i << ": " << formal_param_type->BuildCDecl(formal_param_name) << std::endl;
		}
		
		delete subprogram;
	}
	else
	{
		(*std_output_stream) << "Subprogram \"" << subprogram_name << "\" not found" << std::endl;
	}
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::GetLine(const char *prompt, std::string& line, bool& interactive)
{
	if(trap || exec_queue.empty())
	{
		if(trap)
		{
			trap = false;
			exec_queue.clear();
		}
#if defined(HAVE_LIBEDIT)
		char *line_read;
		do
		{
			if(std_output_stream != &std::cout)
			{
				(*std_output_stream) << prompt;
			}
			line_read = readline(prompt);
			if(Killed()) return false;
			if(!line_read)
			{
				rl_redisplay(); // work around when terminal size changes
			}
				
		} while(!line_read);

		if(std_output_stream != &std::cout)
		{
			(*std_output_stream) << line_read << std::endl;
		}
	/* If the line has any text in it,
		save it on the history. */
		if(*line_read) add_history(line_read);

		line = line_read;

		free(line_read);
		
		interactive = true;
		return true;
#else
		if(std_output_stream != &std::cout)
		{
			std::cout << prompt;
		}
		(*std_output_stream) << prompt;
		getline(std::cin, line);
		if(std::cin.fail()) return false;
		if(Killed()) return false;
		if(std_output_stream != &std::cout)
		{
			(*std_output_stream) << line << std::endl;
		}
		interactive = true;
		return true;
#endif
	}
	else
	{
		line = exec_queue.front();
		(*std_output_stream) << prompt << line << std::endl;
		exec_queue.pop_front();
		interactive = false;
		return true;
	}
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsBlankLine(const std::string& line) const
{
	unsigned int n = line.length();
	unsigned int i;
	for(i = 0; i < n; i++)
	{
		if ((line[i] != ' ') && (line[i] != '\t') && (line[i] != '\n') && (line[i] != '\r')) return false;
	}
	return true;
}


template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsQuitCommand(const char *cmd) const
{
	return strcmp(cmd, "q") == 0 || strcmp(cmd, "quit") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsStepInstructionCommand(const char *cmd) const
{
	return strcmp(cmd, "si") == 0 || strcmp(cmd, "stepi") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsTraverseCommand(const char *cmd) const
{
  return strcmp(cmd, "traverse") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsStepCommand(const char *cmd) const
{
	return strcmp(cmd, "s") == 0 || strcmp(cmd, "step") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsNextInstructionCommand(const char *cmd) const
{
	return strcmp(cmd, "ni") == 0 || strcmp(cmd, "nexti") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsContinueCommand(const char *cmd) const
{
	return strcmp(cmd, "c") == 0 || strcmp(cmd, "cont") == 0 || strcmp(cmd, "continue") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsDisasmCommand(const char *cmd) const
{
	return strcmp(cmd, "dis") == 0 || strcmp(cmd, "disasm") == 0 || strcmp(cmd, "disassemble") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsBreakCommand(const char *cmd) const
{
	return strcmp(cmd, "b") == 0 || strcmp(cmd, "break") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsWatchCommand(const char *cmd) const
{
	return strcmp(cmd, "w") == 0 || strcmp(cmd, "watch") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsDeleteCommand(const char *cmd) const
{
	return strcmp(cmd, "del") == 0 || strcmp(cmd, "delete") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsDeleteWatchCommand(const char *cmd) const
{
	return strcmp(cmd, "delw") == 0 || strcmp(cmd, "delwatch") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsDumpCommand(const char *cmd) const
{
	return strcmp(cmd, "d") == 0 || strcmp(cmd, "dump") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsRegistersCommand(const char *cmd) const
{
	return strcmp(cmd, "regs") == 0 || strcmp(cmd, "registers") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsEditCommand(const char *cmd) const
{
	return strcmp(cmd, "e") == 0 || strcmp(cmd, "ed") == 0 || strcmp(cmd, "edit") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsHelpCommand(const char *cmd) const
{
	return strcmp(cmd, "h") == 0 || strcmp(cmd, "?") == 0 || strcmp(cmd, "help") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsResetCommand(const char *cmd) const
{
	return strcmp(cmd, "r") == 0 || strcmp(cmd, "run") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsMonitorCommand(const char *cmd, const char *format) const
{
	if (format)
	{
		return (strcmp(cmd, "m") == 0 || strcmp(cmd, "monitor") == 0)
		&& strncmp(format, "/", 1) == 0;
	}
	return strcmp(cmd, "m") == 0 || strcmp(cmd, "monitor") == 0 
	|| strncmp(cmd, "m/", 2) == 0 || strncmp(cmd, "monitor/", 8) == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsRegisterCommand(const char *cmd, const char *format) const
{
	if (format)
	{
		return (strcmp(cmd, "reg") == 0 || strcmp(cmd, "register") == 0)
		&& strncmp(format, "/", 1) == 0;
	}
	return strcmp(cmd, "reg") == 0 || strcmp(cmd, "register") == 0 
	|| strncmp(cmd, "reg/", 2) == 0 || strncmp(cmd, "register/", 8) == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsStatisticCommand(const char *cmd, const char *format) const
{
	if (format)
	{
		return (strcmp(cmd, "stat") == 0 || strcmp(cmd, "statistic") == 0)
		&& strncmp(format, "/", 1) == 0;
	}
	return strcmp(cmd, "stat") == 0 || strcmp(cmd, "statistic") == 0 
	|| strncmp(cmd, "stat/", 2) == 0 || strncmp(cmd, "statistic/", 8) == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsParameterCommand(const char *cmd, const char *format) const
{
	if (format)
	{
		return (strcmp(cmd, "param") == 0 || strcmp(cmd, "parameter") == 0)
		&& strncmp(format, "/", 1) == 0;
	}
	return strcmp(cmd, "param") == 0 || strcmp(cmd, "parameter") == 0 
	|| strncmp(cmd, "param/", 2) == 0 || strncmp(cmd, "parameter/", 8) == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsMonitorSetCommand(const char *cmd) const
{
	return strcmp(cmd, "set") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsProfileCommand(const char *cmd) const
{
	return strcmp(cmd, "p") == 0 || strcmp(cmd, "prof") == 0 || strcmp(cmd, "profile") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsBackTraceCommand(const char *cmd) const
{
	return strcmp(cmd, "bt") == 0 || strcmp(cmd, "backtrace") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsFinishCommand(const char *cmd) const
{
	return strcmp(cmd, "fin") == 0 || strcmp(cmd, "finish") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsLoadSymbolTableCommand(const char *cmd) const
{
	return strcmp(cmd, "st") == 0 || strcmp(cmd, "symtab") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsListSymbolsCommand(const char *cmd) const
{
	return strcmp(cmd, "sym") == 0 || strcmp(cmd, "symbol") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsMacroCommand(const char *cmd) const
{
	return strcmp(cmd, "mac") == 0 || strcmp(cmd, "macro") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsLoadConfigCommand(const char *cmd) const
{
	return strcmp(cmd, "conf") == 0 || strcmp(cmd, "config") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsListSourcesCommand(const char *cmd) const
{
	return strcmp(cmd, "sources") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsEnableBinaryCommand(const char *cmd) const
{
	return strcmp(cmd, "enable") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsDisableBinaryCommand(const char *cmd) const
{
	return strcmp(cmd, "disable") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsListBinariesCommand(const char *cmd) const
{
	return strcmp(cmd, "files") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsDumpDataObjectCommand(const char *cmd) const
{
	return strcmp(cmd, "dob") == 0 || strcmp(cmd, "dumpobj") == 0 || strcmp(cmd, "dumpobject") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsPrintDataObjectCommand(const char *cmd) const
{
	return strcmp(cmd, "pob") == 0 || strcmp(cmd, "printobj") == 0 || strcmp(cmd, "printobject") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsEditDataObjectCommand(const char *cmd) const
{
	return strcmp(cmd, "eob") == 0 || strcmp(cmd, "editobj") == 0 || strcmp(cmd, "editobject") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsSetDataObjectCommand(const char *cmd) const
{
	return strcmp(cmd, "sob") == 0 || strcmp(cmd, "setobj") == 0 || strcmp(cmd, "setobject") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsListDataObjectsCommand(const char *cmd) const
{
	return strcmp(cmd, "lob") == 0 || strcmp(cmd, "lstobj") == 0 || strcmp(cmd, "listobjects") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsTrackDataObjectCommand(const char *cmd) const
{
	return strcmp(cmd, "tob") == 0 || strcmp(cmd, "trackobj") == 0 || strcmp(cmd, "trackobject") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsUntrackDataObjectCommand(const char *cmd) const
{
	return strcmp(cmd, "utob") == 0 || strcmp(cmd, "untrackobj") == 0 || strcmp(cmd, "untrackobject") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsWhatIsCommand(const char *cmd) const
{
	return strcmp(cmd, "whatis") == 0;
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsInfoSubProgramCommand(const char *cmd) const
{
	return strcmp(cmd, "info") == 0;
}

template <class ADDRESS>
InlineDebugger<ADDRESS>::VisitedInstructionPage::VisitedInstructionPage()
{
	std::fill_n(flags,WORD_COUNT,0);
}

template <class ADDRESS>
bool
InlineDebugger<ADDRESS>::VisitedInstructionPage::Get(ADDRESS cia)
{
	uint32_t mask = uint32_t(1) << (cia % BITS_PER_WORD);
	unsigned idx = (cia / BITS_PER_WORD) % WORD_COUNT;
	bool visited( flags[idx] & mask );
	flags[idx] |= mask;
	return visited;
}

template <class ADDRESS>
ADDRESS
InlineDebugger<ADDRESS>::VisitedInstructionPage::MaskLowerBits(ADDRESS cia)
{
  return cia & -(ADDRESS(1) << OFFSET_BITS);
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsVisited(ADDRESS _cia)
{
	ADDRESS hi_address = VisitedInstructionPage::MaskLowerBits(_cia);
	VisitedInstructionPage& vip = visited_instructions[hi_address];
	return vip.Get(_cia);
}

template <class ADDRESS>
bool InlineDebugger<ADDRESS>::IsStarted() const
{
	return is_started;
}


} // end of namespace inline_debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
