/*
 *  Copyright (c) 2016,
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
 
#ifndef __UNISIM_SERVICE_DEBUG_MONITOR_MONITOR_HH__
#define __UNISIM_SERVICE_DEBUG_MONITOR_MONITOR_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>
#include <unisim/service/interfaces/backtrace.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/debug_info_loading.hh>
#include <unisim/service/interfaces/data_object_lookup.hh>
#include <unisim/service/interfaces/subprogram_lookup.hh>

#include <unisim/util/debug/breakpoint.hh>
#include <unisim/util/debug/watchpoint.hh>

namespace unisim {
namespace service {
namespace debug {
namespace monitor {

class MonitorBase
{
public:
	MonitorBase();
	virtual ~MonitorBase();

	bool Start();
	
	static MonitorBase *GetInstance();
	static void SetCallback(int (* callback)(void));
	
	virtual int SetBreakpoint(const char *info, void (*callback)(int)) = 0;
	virtual int SetWatchpoint(const char *info, void (*callback)(int)) = 0;
	virtual int DeleteBreakpoint(int handle) = 0;
	virtual int DeleteWatchpoint(int handle) = 0;
	virtual int LookupDataObject(const char *info, size_t size) = 0;
	virtual int GetDataObjectValue(int handle, void *value) = 0;
	virtual int StopMe(int exit_status) = 0;
protected:
	
	static int (* callback)(void);
	static MonitorBase *instance;
};

template <typename ADDRESS>
class HardwareBreakpoint
	: public unisim::util::debug::Breakpoint<ADDRESS>
{
public:
	HardwareBreakpoint(ADDRESS addr, int handle, void (*callback)(int));
	virtual ~HardwareBreakpoint();
	
	void Report() const;
private:
	int handle;
	void (*callback)(int);
};

template <typename ADDRESS>
class HardwareWatchpoint
	: public unisim::util::debug::Watchpoint<ADDRESS>
{
public:
	HardwareWatchpoint(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, int handle, void (*callback)(int));
	virtual ~HardwareWatchpoint();
	void Report() const;
private:
	int handle;
	void (*callback)(int);
};

template <typename ADDRESS>
class SourceCodeBreakpoint
{
public:
	SourceCodeBreakpoint(const char *source_location, typename unisim::service::interfaces::DebugEventTrigger<ADDRESS> *debug_event_trigger_if, typename unisim::service::interfaces::StatementLookup<ADDRESS> *stmt_lookup_if, int handle, void (*callback)(int));
	virtual ~SourceCodeBreakpoint();
	bool Exists() const;
	int GetHandle() const;
	void Invalidate();
private:
	int handle;
	typename unisim::service::interfaces::DebugEventTrigger<ADDRESS> *debug_event_trigger_if;
	std::vector<HardwareBreakpoint<ADDRESS> *> hw_breakpoints;
};

template <typename ADDRESS>
class DataObjectWatchpoint
{
public:
	DataObjectWatchpoint(const char *data_location, typename unisim::service::interfaces::DebugEventTrigger<ADDRESS> *debug_event_trigger_if, typename unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if, int handle, void (*callback)(int));
	virtual ~DataObjectWatchpoint();
	bool Exists() const;
	int GetHandle() const;
	void Report() const;
	void Invalidate();
private:
	int handle;
	typename unisim::service::interfaces::DebugEventTrigger<ADDRESS> *debug_event_trigger_if;
	HardwareWatchpoint<ADDRESS> *hw_watchpoint;
};

template <typename ADDRESS>
class DataObject
{
public:
	DataObject(const char *data_location, size_t size, typename unisim::service::interfaces::Register *reg_pc, typename unisim::service::interfaces::DataObjectLookup<ADDRESS> *data_object_lookup_if, int handle);
	virtual ~DataObject();
	bool Exists() const;
	bool IsOptimizedOut() const;
	bool GetValue(void *) const;
private:
	int handle;
	size_t size;
	unisim::service::interfaces::Register *reg_pc;
	unisim::util::debug::DataObject<ADDRESS> *data_object;
};

template <typename ADDRESS>
class Monitor
	: public MonitorBase
	, public unisim::kernel::Service<unisim::service::interfaces::DebugEventListener<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Registers>
	, public unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::StatementLookup<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::BackTrace<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::DebugInfoLoading>
	, public unisim::kernel::Client<unisim::service::interfaces::DataObjectLookup<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::SubProgramLookup<ADDRESS> >
{
public:
	Monitor(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~Monitor();
	
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugEventListener<ADDRESS> > debug_event_listener_export;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugEventTrigger<ADDRESS> > debug_event_trigger_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Memory<ADDRESS> > memory_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Registers> registers_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::SymbolTableLookup<ADDRESS> > symbol_table_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::StatementLookup<ADDRESS> > stmt_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::BackTrace<ADDRESS> > backtrace_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugInfoLoading> debug_info_loading_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DataObjectLookup<ADDRESS> > data_object_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::SubProgramLookup<ADDRESS> > subprogram_lookup_import;
	
	virtual bool EndSetup();
	
	virtual int SetBreakpoint(const char *info, void (*callback)(int));
	virtual int SetWatchpoint(const char *info, void (*callback)(int));
	virtual int DeleteBreakpoint(int handle);
	virtual int DeleteWatchpoint(int handle);
	virtual int LookupDataObject(const char *info, size_t size);
	virtual int GetDataObjectValue(int handle, void *value);
	virtual int StopMe(int exit_status);
	
private:
	unisim::kernel::logger::Logger logger;
	unisim::service::interfaces::Register *reg_pc;
	std::vector<SourceCodeBreakpoint<ADDRESS> *> source_code_breakpoints;
	std::vector<DataObjectWatchpoint<ADDRESS> *> data_object_watchpoints;
	std::vector<DataObject<ADDRESS> *> tracked_data_objects;
	
	// DebugEventListener
	virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event);
	
};

} // end of namespace monitor
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
