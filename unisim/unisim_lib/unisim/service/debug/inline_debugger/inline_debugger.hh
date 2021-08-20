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
 
#ifndef __UNISIM_SERVICE_DEBUG_INLINE_DEBUGGER_INLINE_DEBUGGER_HH__
#define __UNISIM_SERVICE_DEBUG_INLINE_DEBUGGER_INLINE_DEBUGGER_HH__

#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>
#include <unisim/service/interfaces/backtrace.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/profiling.hh>
#include <unisim/service/interfaces/debug_info_loading.hh>
#include <unisim/service/interfaces/data_object_lookup.hh>
#include <unisim/service/interfaces/subprogram_lookup.hh>

#include <unisim/util/debug/profile.hh>
#include <unisim/util/debug/breakpoint.hh>
#include <unisim/util/debug/watchpoint.hh>
#include <unisim/util/debug/fetch_insn_event.hh>
#include <unisim/util/debug/trap_event.hh>
#include <unisim/util/loader/elf_loader/elf32_loader.hh>
#include <unisim/util/loader/elf_loader/elf64_loader.hh>
#include <unisim/util/ieee754/ieee754.hh>

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>

#include <inttypes.h>
#include <string>
#include <vector>
#include <queue>
#include <list>
#include <stack>
#include <set>

namespace unisim {
namespace service {
namespace debug {
namespace inline_debugger {

typedef enum
{
	INLINE_DEBUGGER_MODE_WAITING_USER,
	INLINE_DEBUGGER_MODE_STEP_INSTRUCTION,
	INLINE_DEBUGGER_MODE_STEP,
	INLINE_DEBUGGER_MODE_CONTINUE,
	INLINE_DEBUGGER_MODE_CONTINUE_UNTIL,
	INLINE_DEBUGGER_MODE_RESET,
	INLINE_DEBUGGER_MODE_TRAVERSE
} InlineDebuggerRunningMode;

class InlineDebuggerBase : public virtual unisim::kernel::Object
{
public:
	InlineDebuggerBase(const char *_name, unisim::kernel::Object *parent);
	virtual ~InlineDebuggerBase();
	virtual void SigInt();
	
	std::string SearchFile(const char *filename);
	bool LocateFile(const char *file_path, std::string& match_file_path);

protected:
	virtual void Interrupt() = 0;
	
	std::string search_path;
	unisim::kernel::variable::Parameter<std::string> param_search_path;
};

template <class ADDRESS>
class InlineDebugger
	: public InlineDebuggerBase
	, public unisim::kernel::Service<unisim::service::interfaces::DebugYielding>
	, public unisim::kernel::Service<unisim::service::interfaces::DebugEventListener<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::DebugYieldingRequest>
	, public unisim::kernel::Client<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Disassembly<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Registers>
	, public unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::StatementLookup<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::BackTrace<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Profiling<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::DebugInfoLoading>
	, public unisim::kernel::Client<unisim::service::interfaces::DataObjectLookup<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::SubProgramLookup<ADDRESS> >
{
public:
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYielding>                debug_yielding_export;
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugEventListener<ADDRESS> > debug_event_listener_export;
	
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugYieldingRequest>         debug_yielding_request_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >  debug_event_trigger_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Disassembly<ADDRESS> >        disasm_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Memory<ADDRESS> >             memory_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Registers>                    registers_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >  symbol_table_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::StatementLookup<ADDRESS> >    stmt_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::BackTrace<ADDRESS> >          backtrace_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Profiling<ADDRESS> >          profiling_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugInfoLoading>             debug_info_loading_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DataObjectLookup<ADDRESS> >   data_object_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::SubProgramLookup<ADDRESS> >   subprogram_lookup_import;
	
	InlineDebugger(const char *name, Object *parent = 0);
	virtual ~InlineDebugger();

	// unisim::service::interfaces::DebugYielding
	virtual void DebugYield();
	
	// unisim::service::interfaces::DebugEventListener<ADDRESS>
	virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event);

	virtual bool EndSetup();
	virtual void OnDisconnect();
	
	bool IsStarted() const;
protected:
	virtual void Interrupt();
private:
	unisim::kernel::logger::Logger logger;
	unsigned int memory_atom_size;
	std::string init_macro;
	std::string output;
	std::string program_counter_name;
	unisim::kernel::variable::Parameter<unsigned int> param_memory_atom_size;
	unisim::kernel::variable::Parameter<std::string> param_init_macro;
	unisim::kernel::variable::Parameter<std::string> param_output;
	unisim::kernel::variable::Parameter<std::string> param_program_counter_name;

	unisim::service::interfaces::Register *program_counter;
	bool listening_fetch;
	bool listening_trap;
	bool trap;
	InlineDebuggerRunningMode running_mode;

	ADDRESS cia;
	ADDRESS disasm_addr;
	ADDRESS dump_addr;
	ADDRESS cont_until_addr;
	const unisim::util::debug::Statement<ADDRESS> *last_stmt;

	std::list<std::string> exec_queue;
	std::string prompt;
	std::string last_line;
	std::string line;
	std::vector<std::string> parm;
	std::ostream *output_stream;
	std::ostream *std_output_stream;
	std::ostream *std_error_stream;
	
	std::list<unisim::util::debug::DataObject<ADDRESS> *> tracked_data_objects;
	
	unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event;
	unisim::util::debug::TrapEvent<ADDRESS> *trap_event;
	
	class VisitedInstructionPage
	{
	public:
		VisitedInstructionPage();
		bool Get(ADDRESS cia);
		static ADDRESS MaskLowerBits(ADDRESS cia);
		
	private:
		static unsigned const OFFSET_BITS = 12;
		typedef uint32_t Words;
		static unsigned const BITS_PER_WORD = 8*sizeof (Words);
		static unsigned const WORD_COUNT = (1 << OFFSET_BITS) / BITS_PER_WORD;
		Words flags[WORD_COUNT];
	};
	std::map<ADDRESS,VisitedInstructionPage> visited_instructions;
	
	bool is_started;

	void Tokenize(const std::string& str, std::vector<std::string>& tokens);
	bool ParseAddr(const char *s, ADDRESS& addr);
	bool ParseAddrRange(const char *s, ADDRESS& addr, unsigned int& size);
	bool ParseIntegerValue(const char *s, uint64_t& value);
	bool ParseFloatValue(const char *s, unisim::util::ieee754::SoftDouble& value);
	bool GetLine(const char *prompt, std::string& line, bool& interactive);
	bool IsBlankLine(const std::string& line) const;
	bool IsQuitCommand(const char *cmd) const;
	bool IsStepInstructionCommand(const char *cmd) const;
	bool IsTraverseCommand(const char *cmd) const;
	bool IsStepCommand(const char *cmd) const;
	bool IsNextInstructionCommand(const char *cmd) const;
	bool IsContinueCommand(const char *cmd) const;
	bool IsDisasmCommand(const char *cmd) const;
	bool IsBreakCommand(const char *cmd) const;
	bool IsFinishCommand(const char *cmd) const;
	bool IsWatchCommand(const char *cmd) const;
	bool IsDeleteCommand(const char *cmd) const;
	bool IsDeleteWatchCommand(const char *cmd) const;
	bool IsDumpCommand(const char *cmd) const;
	bool IsRegistersCommand(const char *cmd) const;
	bool IsEditCommand(const char *cmd) const;
	bool IsHelpCommand(const char *cmd) const;
	bool IsResetCommand(const char *cmd) const;
	bool IsMonitorCommand(const char *cmd, const char *format = 0) const;
	bool IsRegisterCommand(const char *cmd, const char *format = 0) const;
	bool IsStatisticCommand(const char *cmd, const char *format = 0) const;
	bool IsParameterCommand(const char *cmd, const char *format = 0) const;
	bool IsSymbolCommand(const char *cmd) const;
	bool IsMonitorSetCommand(const char *cmd) const;
	bool IsProfileCommand(const char *cmd) const;
	bool IsLoadCommand(const char *cmd) const;
	bool IsBackTraceCommand(const char *cmd) const;
	bool IsLoadSymbolTableCommand(const char *cmd) const;
	bool IsListSymbolsCommand(const char *cmd) const;
	bool IsMacroCommand(const char *cmd) const;
	bool IsLoadConfigCommand(const char *cmd) const;
	bool IsListSourcesCommand(const char *cmd) const;
	bool IsEnableBinaryCommand(const char *cmd) const;
	bool IsDisableBinaryCommand(const char *cmd) const;
	bool IsListBinariesCommand(const char *cmd) const;
	bool IsDumpDataObjectCommand(const char *cmd) const;
	bool IsPrintDataObjectCommand(const char *cmd) const;
	bool IsEditDataObjectCommand(const char *cmd) const;
	bool IsSetDataObjectCommand(const char *cmd) const;
	bool IsListDataObjectsCommand(const char *cmd) const;
	bool IsTrackDataObjectCommand(const char *cmd) const;
	bool IsUntrackDataObjectCommand(const char *cmd) const;
	bool IsWhatIsCommand(const char *cmd) const;
	bool IsInfoSubProgramCommand(const char *cmd) const;

	void Help();
	bool ListenFetch();
	bool UnlistenFetch();
	void Disasm(ADDRESS addr, int count, ADDRESS& next_addr);
	bool HasBreakpoint(ADDRESS addr);
	void SetBreakpoint(ADDRESS addr);
	void SetReadWatchpoint(ADDRESS addr, uint32_t size);
	void SetWriteWatchpoint(ADDRESS addr, uint32_t size);
	void DeleteBreakpoint(ADDRESS addr);
	void DeleteReadWatchpoint(ADDRESS addr, uint32_t size);
	void DeleteWriteWatchpoint(ADDRESS addr, uint32_t size);
	void DumpBreakpoints();
	void DumpWatchpoints();
	void DumpMemory(ADDRESS addr);
	void DumpRegisters();
	bool EditBuffer(ADDRESS addr, std::vector<uint8_t>& buffer);
	bool EditMemory(ADDRESS addr);
	void DumpSymbols(const typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>& symbols, const char *name = 0);
	void DumpSymbols(const char *name = 0);
	void MonitorGetFormat(const char *cmd, char &format);
	void DumpVariables(const char *cmd, const char *name = 0, typename unisim::kernel::VariableBase::Type type = unisim::kernel::VariableBase::VAR_VOID);
	void DumpVariable(const char *cmd, const unisim::kernel::VariableBase *variable);
	void SetVariable(const char *name, const char *value);
	void DumpProgramProfile();
	void DumpDataProfile(bool write);
	void LoadSymbolTable(const char *filename);
	void LoadMacro(const char *filename);
	void DumpSource(const char *filename, unsigned int lineno, unsigned int colno, unsigned int count);
	void DumpBackTrace();
	bool GetReturnAddress(ADDRESS& ret_addr) const;
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(ADDRESS addr, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt = unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_EXACT_STMT) const;
	void EnableProgramProfiling();
	void EnableDataReadProfiling();
	void EnableDataWriteProfiling();
	void EnableDataProfiling();
	void EnableProfiling();
	void DisableProgramProfiling();
	void DisableDataReadProfiling();
	void DisableDataWriteProfiling();
	void DisableDataProfiling();
	void DisableProfiling();
	void ClearProgramProfile();
	void ClearDataReadProfile();
	void ClearDataWriteProfile();
	void ClearDataProfile();
	void ClearProfile();
	void DumpProgramProfilingStatus();
	void DumpDataReadProfilingStatus();
	void DumpDataWriteProfilingStatus();
	void DumpDataProfilingStatus();
	void DumpProfilingStatus();
	void ListSourceFiles();
	void EnableBinary(const char *filename, bool enable);
	void ListBinaryFiles();
	void DumpDataObject(const char *data_object_name);
	void PrintDataObject(const char *data_object_name);
	bool EditDataObject(const char *data_object_name);
	bool SetDataObject(const char *data_object_name, const char *literal);
	void ListDataObjects(typename unisim::service::interfaces::DataObjectLookup<ADDRESS>::Scope scope = unisim::service::interfaces::DataObjectLookup<ADDRESS>::SCOPE_BOTH_GLOBAL_AND_LOCAL);
	void TrackDataObject(const char *data_object_name);
	void UntrackDataObject(const char *data_object_name);
	void PrintDataObject(unisim::util::debug::DataObject<ADDRESS> *data_object);
	void PrintTrackedDataObjects();
	void PrintDataObjectType(unisim::util::debug::DataObject<ADDRESS> *data_object);
	void PrintDataObjectType(const char *data_object_name);
	void InfoSubProgram(const char *subprogram_name);
	bool IsVisited(ADDRESS _cia);
};

} // end of namespace inline_debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
