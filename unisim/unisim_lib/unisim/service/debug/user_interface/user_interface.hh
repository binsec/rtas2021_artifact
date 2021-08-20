/*
 *  Copyright (c) 2019,
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

#ifndef __UNISIM_SERVICE_DEBUG_USER_INTERFACE_USER_INTERFACE_HH__
#define __UNISIM_SERVICE_DEBUG_USER_INTERFACE_USER_INTERFACE_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/service/interfaces/http_server.hh>
#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/debug_selecting.hh>
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
#include <unisim/util/debug/event.hh>
#include <unisim/util/debug/breakpoint.hh>
#include <unisim/util/debug/watchpoint.hh>
#include <unisim/util/debug/fetch_insn_event.hh>
#include <unisim/util/debug/commit_insn_event.hh>
#include <unisim/util/debug/trap_event.hh>
#include <unisim/util/likely/likely.hh>

#include <pthread.h>

#include <vector>

namespace unisim {
namespace service {
namespace debug {
namespace user_interface {

template <typename ADDRESS>
struct UserInterface
	: unisim::kernel::Service<unisim::service::interfaces::HttpServer>
	, unisim::kernel::Service<unisim::service::interfaces::DebugYielding>
	, unisim::kernel::Service<unisim::service::interfaces::DebugEventListener<ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::DebugSelecting>
	, unisim::kernel::Client<unisim::service::interfaces::DebugYieldingRequest>
	, unisim::kernel::Client<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::Disassembly<ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::Registers>
	, unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::StatementLookup<ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::BackTrace<ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::Profiling<ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::DebugInfoLoading>
	, unisim::kernel::Client<unisim::service::interfaces::DataObjectLookup<ADDRESS> >
	, unisim::kernel::Client<unisim::service::interfaces::SubProgramLookup<ADDRESS> >

{
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer>                   http_server_export;
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYielding>                debug_yielding_export;
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugEventListener<ADDRESS> > debug_event_listener_export;
	
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugYieldingRequest>         debug_yielding_request_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugSelecting>               debug_selecting_import;
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
	
	UserInterface(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~UserInterface();
	
	virtual bool EndSetup();
	virtual void Kill();
	
	// unisim::service::interfaces::HttpServer
	virtual bool ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	virtual void ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner);
	
	// unisim::service::interfaces::DebugYielding
	virtual void DebugYield();
	
	// unisim::service::interfaces::DebugEventListener<ADDRESS>
	virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event);

protected:
	unisim::kernel::logger::Logger logger;
	
private:
	struct PropertySetter : unisim::util::hypapp::Form_URL_Encoded_Decoder
	{
		PropertySetter(UserInterface<ADDRESS>& user_interface);
		
		virtual bool FormAssign(const std::string& name, const std::string& value);
		void Apply();
		
		UserInterface<ADDRESS>& user_interface;
		bool stepi;
		bool step;
		bool cont;
		bool intr;
		bool halt;
	};
	
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	bool debug;
	unisim::kernel::variable::Parameter<bool> param_debug;
	double min_cont_refresh_period;
	unisim::kernel::variable::Parameter<double> param_min_cont_refresh_period;
	double max_cont_refresh_period;
	unisim::kernel::variable::Parameter<double> param_max_cont_refresh_period;
	bool enable_cache;
	unisim::kernel::variable::Parameter<bool> param_enable_cache;
	std::string title;
	unisim::kernel::variable::Parameter<std::string> param_title;
	unsigned int memory_atom_size;
	unisim::kernel::variable::Parameter<unsigned int> param_memory_atom_size;
	
	bool stepi;
	bool cont;
	bool intr;
	bool halt;
	double refresh_period;
	pthread_mutex_t mutex;
	pthread_mutex_t mutex_post;
	pthread_cond_t cond_cont;
	pthread_mutex_t mutex_cont;
	unsigned int num_processors;
	unsigned int curr_prc_num;
	std::vector<unisim::util::debug::FetchInsnEvent<ADDRESS> *> fetch_insn_events;
	std::vector<bool> listening_fetch;
	std::vector<const unisim::util::debug::Event<ADDRESS> *> stop_events;
	std::vector<bool> prc_trap;
	
	bool ListenFetch(unsigned int prc_num);
	bool UnlistenFetch(unsigned int prc_num);
	void ClearStopEvents();
	void WaitForUser();
	void Continue();
	void Interrupt();
	void Lock();
	void Unlock();
	void LockPost();
	void UnlockPost();
};

} // end of namespace user_interface
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_USER_INTERFACE_USER_INTERFACE_HH__
