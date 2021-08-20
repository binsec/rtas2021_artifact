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

#ifndef __UNISIM_SERVICE_DEBUG_GDB_SERVER_GDB_SERVER_HH__
#define __UNISIM_SERVICE_DEBUG_GDB_SERVER_GDB_SERVER_HH__

#include <unisim/util/endian/endian.hh>
#include <unisim/util/xml/xml.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/debug_selecting.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/register.hh>
#include <unisim/service/interfaces/memory.hh>

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>

#include <unisim/util/debug/event.hh>
#include <unisim/util/debug/breakpoint.hh>
#include <unisim/util/debug/watchpoint.hh>
#include <unisim/util/debug/fetch_insn_event.hh>
#include <unisim/util/debug/commit_insn_event.hh>
#include <unisim/util/debug/trap_event.hh>

#include <string>
#include <vector>
#include <fstream>

#include <pthread.h>
#include <stdint.h>

namespace unisim {
namespace service {
namespace debug {
namespace gdb_server {

typedef enum
{
	GDB_WAIT_CONNECTION_NEVER        = 0,
	GDB_WAIT_CONNECTION_STARTUP_ONLY = 1,
	GDB_WAIT_CONNECTION_ALWAYS       = 2
} GDBWaitConnectionMode;

typedef enum
{
	GDB_LITTLE_ENDIAN,
	GDB_BIG_ENDIAN
} GDBEndian;

inline char Nibble2HexChar(uint8_t v)
{
	v = v & 0xf; // keep only 4-bits
	return v < 10 ? '0' + v : 'a' + v - 10;
}

inline uint8_t HexChar2Nibble(char ch)
{
	if(ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
	if(ch >= '0' && ch <= '9') return ch - '0';
	if(ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
	return 0;
}

inline bool IsHexChar(char ch)
{
	if(ch >= 'a' && ch <= 'f') return true;
	if(ch >= '0' && ch <= '9') return true;
	if(ch >= 'A' && ch <= 'F') return true;
	return false;
}

class GDBRegister
{
public:
	GDBRegister();
	GDBRegister(const std::string& reg_name, int reg_bitsize, GDBEndian endian, unsigned int reg_num, const std::string& type, const std::string& group);
	inline const char *GetName() const { return name.c_str(); }
	inline int GetBitSize() const { return bitsize; }
	inline int GetByteSize() const { return bitsize / 8; }
	bool SetValue(unsigned int prc_num, const std::string& hex);
	bool SetValue(unsigned int prc_num, const void *buffer);
	bool GetValue(unsigned int prc_num, std::string& hex) const;
	bool GetValue(unsigned int prc_num, void *buffer) const;
	template <typename T> bool GetValue(unsigned int prc_num, T& v) const { unisim::service::interfaces::Register *reg = (prc_num < arch_regs.size()) ? arch_regs[prc_num] : 0; if(reg) reg->GetValue(v); return reg != 0; }
	template <typename T> bool SetValue(unsigned int prc_num, const T& v) { unisim::service::interfaces::Register *reg = (prc_num < arch_regs.size()) ? arch_regs[prc_num] : 0; if(reg) reg->SetValue(v); return reg != 0; }
	inline int GetHexLength() const { return bitsize / 4; }
	inline unisim::service::interfaces::Register *GetRegisterInterface(unsigned int prc_num) const { return (prc_num < arch_regs.size()) ? arch_regs[prc_num] : 0; }
	void SetRegisterInterface(unsigned int prc_num, unisim::service::interfaces::Register *reg);
	inline GDBEndian GetEndian() const { return endian; }
	unsigned int GetRegNum() const { return reg_num; }
	const char *GetType() const { return type.c_str(); }
	const char *GetGroup() const { return group.c_str(); }
	std::ostream& ToXML(std::ostream& os, unsigned int reg_num) const;
private:
	std::string name;
	int bitsize;
	std::vector<unisim::service::interfaces::Register *> arch_regs;
	GDBEndian endian;
	unsigned int reg_num;
	std::string type;
	std::string group;
};

class GDBFeature
{
public:
	GDBFeature(const std::string& feature_name, unsigned int id);
	void AddRegister(const GDBRegister *gdb_register);
	unsigned int GetId() const;
	std::ostream& ToXML(std::ostream& os, std::string req_filename = std::string()) const;
private:
	std::string name;
	unsigned int id;
	std::vector<const GDBRegister *> gdb_registers;
};

class GDBServerBase : public virtual unisim::kernel::Object
{
public:
	static const uint64_t SERVER_ACCEPT_POLL_PERIOD_MS      = 100  /* ms */;  // every 100 ms
	static const uint64_t NON_BLOCKING_READ_POLL_PERIOD_MS  = 10   /* ms */;  // every 10 ms
	static const uint64_t NON_BLOCKING_WRITE_POLL_PERIOD_MS = 10   /* ms */;  // every 10 ms

	GDBServerBase(const char *_name, unisim::kernel::Object *_parent);
	virtual ~GDBServerBase();
	
	virtual bool EndSetup();
	
	bool StartServer();
	bool StopServer();
	void WaitTime(unsigned int msec);
	std::string GetLastErrorString();
	bool GetChar(char& c, bool blocking);
	bool FlushOutput();
	bool PutChar(char c);
	bool IsConnected() const;
	
protected:
	static const unsigned int MAX_BUFFER_SIZE = 256;
	unisim::kernel::logger::Logger logger;
	int tcp_port;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	uintptr_t sock;
#else
	int sock;
#endif
	unsigned int session_num;
	bool sock_error;
	
	unsigned int input_buffer_size;
	unsigned int input_buffer_index;
	char input_buffer[MAX_BUFFER_SIZE];

	unsigned int output_buffer_size;
	char output_buffer[MAX_BUFFER_SIZE];
	
	bool verbose;
	bool debug;
	std::string remote_serial_protocol_input_traffic_recording_filename;
	std::ofstream remote_serial_protocol_input_traffic_recording_file;
	std::string remote_serial_protocol_output_traffic_recording_filename;
	std::ofstream remote_serial_protocol_output_traffic_recording_file;
	
	unisim::kernel::variable::Parameter<int> param_tcp_port;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::Parameter<bool> param_debug;
	unisim::kernel::variable::Parameter<std::string> param_remote_serial_protocol_input_traffic_recording_filename;
	unisim::kernel::variable::Parameter<std::string> param_remote_serial_protocol_output_traffic_recording_filename;
};

typedef enum
{
	GDB_SERVER_ERROR_EXPECTING_HEX               = 2,
	GDB_SERVER_ERROR_EXPECTING_COMMA             = 3,
	GDB_SERVER_ERROR_EXPECTING_COLON             = 4,
	GDB_SERVER_ERROR_EXPECTING_SEMICOLON         = 5,
	GDB_SERVER_ERROR_EXPECTING_ASSIGNMENT        = 6,
	GDB_SERVER_ERROR_GARBAGE                     = 7,
	GDB_SERVER_ERROR_EXPECTING_THREAD_ID         = 8,
	GDB_SERVER_ERROR_INVALID_THREAD_ID           = 9,
	GDB_SERVER_ERROR_EXPECTING_OPERATION         = 10,
	GDB_SERVER_ERROR_INVALID_OPERATION           = 11,
	GDB_SERVER_ERROR_CANT_DEBUG_PROCESSOR        = 12,
	GDB_SERVER_ERROR_UNKNOWN_REGISTER            = 13,
	GDB_SERVER_ERROR_CANT_READ_REGISTER          = 14,
	GDB_SERVER_ERROR_CANT_WRITE_REGISTER         = 15,
	GDB_SERVER_ERROR_CANT_READ_MEMORY            = 16,
	GDB_SERVER_ERROR_CANT_WRITE_MEMORY           = 17,
	GDB_SERVER_ERROR_MALFORMED_BINARY_DATA       = 18,
	GDB_SERVER_CANT_SET_BREAKPOINT_WATCHPOINT    = 19,
	GDB_SERVER_CANT_REMOVE_BREAKPOINT_WATCHPOINT = 20,
	GDB_SERVER_ERROR_EXPECTING_ACTION            = 21
}
GDBServerError;

std::ostream& operator << (std::ostream& os, const GDBServerError& gdb_server_error);

typedef enum
{
	GDB_SERVER_NO_ACTION,
	GDB_SERVER_ACTION_STEP,
	GDB_SERVER_ACTION_CONTINUE
}
GDBServerAction;

std::ostream& operator << (std::ostream& os, const GDBServerAction& gdb_server_action);

typedef enum
{
	GDB_MODE_SINGLE_THREAD,
	GDB_MODE_MULTI_THREAD
}
GDBMode;

std::ostream& operator << (std::ostream& os, const GDBMode& gdb_mode);

template <class ADDRESS>
class GDBServer
	: public GDBServerBase
	, public unisim::kernel::Service<unisim::service::interfaces::DebugYielding>
	, public unisim::kernel::Service<unisim::service::interfaces::DebugEventListener<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::DebugSelecting>
	, public unisim::kernel::Client<unisim::service::interfaces::DebugYieldingRequest>
	, public unisim::kernel::Client<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Disassembly<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Registers>
{
public:
	static const bool ALWAYS_ACCEPT_MULTIPROCESS_NEW_THREAD_ID_SYNTAX = true; // Note: as new thread-id syntax (for multiprocess) is backward compatible with old syntax
	static const long PROCESS_ID = 1;
	
	// Exports to debugger
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYielding>                debug_yielding_export;
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugEventListener<ADDRESS> > debug_event_listener_export;

	// Imports from debugger
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugYieldingRequest>         debug_yielding_request_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugSelecting>               debug_selecting_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >  debug_event_trigger_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Memory<ADDRESS> >             memory_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Registers>                    registers_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Disassembly<ADDRESS> >        disasm_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >  symbol_table_lookup_import;

	GDBServer(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~GDBServer();

	// unisim::service::interfaces::DebugYielding
	virtual void DebugYield();

	// unisim::service::interfaces::DebugEventListener<ADDRESS>
	virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event);
	
	virtual bool EndSetup();
	virtual void OnDisconnect();

	virtual void Kill();
private:
	bool VisitArchitecture(unisim::util::xml::Node *xml_node);
	bool VisitBreakPointKind(unisim::util::xml::Node *xml_node);
	bool VisitFeature(unisim::util::xml::Node *xml_node);
	bool VisitRegister(unisim::util::xml::Node *xml_node, GDBFeature *gdb_feature);
	bool VisitProgramCounter(unisim::util::xml::Node *xml_node);
	
	bool ParseChar(const std::string& s, std::size_t& pos, char& value);
	bool MatchChar(const std::string& s, std::size_t& pos, char value);
	bool MatchChar(const std::string& s, std::size_t& pos, char value1, char value2, char& value);
	bool MatchString(const std::string& s, std::size_t& pos, const std::string& value);
	template <typename T> bool ParseHex(const std::string& s, std::size_t& pos, T& value);
	template <typename T> bool ParseSignedHex(const std::string& s, std::size_t& pos, T& value);
	bool ParseThreadId(const std::string& s, std::size_t& pos, long& thread_id);
	bool GetPacket(char prefix, std::string& s, bool blocking);
	bool PutPacket(char prefix, const std::string& s);
	bool GetQuery(std::string& s, bool blocking);
	bool PutReply(const std::string& s);
	bool PutNotification(const std::string& s);
	bool PutErrorReply(GDBServerError gdb_server_error);
	bool PutErrorReply(GDBServerError gdb_server_error, unsigned int pos);
	bool SetCThread(long thread_id);
	bool SetGThread(long thread_id);
	bool OutputText(const char *s, int count);
	bool ReadRegisters();
	bool WriteRegisters(const std::string& hex);
	bool ReadRegister(unsigned int regnum);
	bool WriteRegister(unsigned int regnum, const std::string& hex);
	bool ReadMemoryHex(ADDRESS addr, uint32_t size);
	bool WriteMemoryHex(ADDRESS addr, const std::string& hex, uint32_t size);
	bool ReadMemoryBin(ADDRESS addr, uint32_t size);
	bool WriteMemoryBin(ADDRESS addr, const std::string& bin, uint32_t size);
	bool ReportProgramExit();
	bool ReportSignal(unsigned int signum);
	void ClearStopEvents();
	bool SetBreakpointWatchpoint(uint32_t type, ADDRESS addr, uint32_t kind);
	bool RemoveBreakpointWatchpoint(uint32_t type, ADDRESS addr, uint32_t kind);

	bool HandleVCont(const std::string& query, std::size_t& pos);
	bool HandleQRcmd(const std::string& query, std::size_t& pos);
	bool HandleQSupported(const std::string& query, std::size_t& pos);
	void SetGDBClientFeature(std::string gdb_client_feature);
	bool HandleQC();
	bool HandleQAttached(const std::string& query, std::size_t& pos);
	bool HandleQTStatus();
	bool HandleQStartNoAckMode();
	bool HandleQXferFeaturesRead(const std::string& query, std::size_t& pos);
	bool HandleQXferThreadsRead(const std::string& query, std::size_t& pos);
	bool HandleQfThreadInfo();
	bool HandleQsThreadInfo();
	bool HandleQThreadExtraInfo(const std::string& query, std::size_t& pos);
	bool HandleQRegisterInfo(const std::string& query, std::size_t& pos); // LLDB specific

	void KillFromThrdProcessCmd();
	void KillFromSimulationRun();
	void TriggerDebugYield();
	void Barrier();
	void Interrupt();
	bool ListenFetch(unsigned int prc_num);
	bool ListenFetch();
	bool UnlistenFetch(unsigned int prc_num);
	bool UnlistenFetch();
	
	bool ThreadIdToProcessorNumber(long thread_id, unsigned int& prc_num) const;
	long ProcessorNumberToThreadId(unsigned int prc_num) const;

	std::string EscapeString(const std::string& s) const;
	std::string HexEncodeString(const std::string& s) const;

	bool DisplayMonitoredInternals();

	std::string architecture_description_filename;
	GDBEndian endian;
	unsigned int gdb_arch_reg_num;
	std::string program_counter_name;

	std::vector<GDBFeature *> gdb_features;
	std::vector<GDBRegister *> gdb_registers;
	GDBRegister *gdb_pc;
	unsigned int num_processors;
	unsigned int g_prc_num;
	unsigned int c_prc_num;
	std::vector<GDBServerAction> prc_actions;
	unsigned int session_num;
	bool session_terminated;
	bool detached;
	std::vector<bool> listening_fetch;
	bool trap;
	std::vector<bool> prc_trap;
	bool synched;
	bool extended_mode;
	bool gdb_client_feature_multiprocess;
	bool gdb_client_feature_xmlregisters;
	bool gdb_client_feature_qrelocinsn;
	bool gdb_client_feature_swbreak;
	bool gdb_client_feature_hwbreak;
	bool gdb_client_feature_fork_events;
	bool gdb_client_feature_vfork_events;
	bool gdb_client_feature_exec_events;
	bool gdb_client_feature_vcont;
	bool gdb_client_feature_t32extensions;
	bool gdb_client_feature_qxfer_features_read;
	bool gdb_client_feature_qxfer_threads_read;
	bool no_ack_mode;
	std::map<uint32_t, uint32_t> arch_specific_breakpoint_kinds;
	
	std::vector<unisim::util::debug::FetchInsnEvent<ADDRESS> *> fetch_insn_events;
	
	std::vector<const unisim::util::debug::Event<ADDRESS> *> stop_events;
	
	unsigned int memory_atom_size;
	std::string monitor_internals;
	GDBWaitConnectionMode wait_connection_mode;
	bool enable_multiprocess_extension;
	bool enable_interrupt;
	unsigned int interrupt_poll_period_ms;
	GDBMode mode;

	unisim::kernel::variable::Parameter<unsigned int> param_memory_atom_size;
	unisim::kernel::variable::Parameter<std::string> param_architecture_description_filename;
	unisim::kernel::variable::Parameter<std::string> param_monitor_internals;
	unisim::kernel::variable::Parameter<GDBWaitConnectionMode> param_wait_connection_mode;
	unisim::kernel::variable::Parameter<bool> param_enable_multiprocess_extension;
	unisim::kernel::variable::Parameter<bool> param_enable_interrupt;
	unisim::kernel::variable::Parameter<unsigned int> param_interrupt_poll_period_ms;
	unisim::kernel::variable::Parameter<GDBMode> param_mode;

	///////////////////////////////////
	
	pthread_t thrd_process_cmd;
	pthread_t thrd_process_int;
	
	pthread_mutex_t thrd_process_cmd_create_mutex;
	pthread_cond_t thrd_process_cmd_create_cond;
	bool thrd_process_cmd_alive;
	bool stop_process_cmd_thrd;
	pthread_mutex_t thrd_process_int_create_mutex;
	pthread_cond_t thrd_process_int_create_cond;
	bool thrd_process_int_alive;
	bool stop_process_int_thrd;

	pthread_mutex_t thrd_run_mutex;
	pthread_cond_t thrd_run_cond;
	bool run_cond;
	
	pthread_mutex_t thrd_process_cmd_mutex;
	pthread_cond_t thrd_process_cmd_cond;
	bool process_cmd_cond;
	
	pthread_mutex_t thrd_mutex;
	
	bool wait_for_command_processing;

	bool StartServer();
	bool StopServer();
	void Lock();
	void Unlock();
	bool StartProcessCmdThrd();
	bool StopProcessCmdThrd();
	bool StartProcessIntThrd();
	bool StopProcessIntThrd();
	static void *ProcessCmdThrdEntryPoint(void *self);
	static void *ProcessIntThrdEntryPoint(void *self);
	void UnblockCommandProcessing();
	void WaitForCommandProcessing();
	void Run();
	void WaitForSimulationRun();
	virtual void ProcessCmdThrd();
	virtual void ProcessIntThrd();
	void ProcessCommands();
	bool Step();
	bool Continue();
	void ClearEvents();
};

} // end of namespace gdb_server
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
