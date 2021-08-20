
#ifndef __UNISIM_SERVICE_MONITOR_HH__
#define __UNISIM_SERVICE_MONITOR_HH__

#include <string>
#include <list>

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>

#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/monitor_if.hh>
#include <unisim/service/interfaces/registers.hh>

#include <unisim/service/monitor/monitor_server.hh>

#include <unisim/util/converter/convert.hh>
#include <unisim/util/debug/breakpoint_registry.hh>
#include <unisim/util/debug/watchpoint_registry.hh>
#include <unisim/util/debug/symbol.hh>

#include <unisim/kernel/logger/logger.hh>

using namespace std;

using unisim::util::debug::BreakpointRegistry;
using unisim::util::debug::WatchpointRegistry;
using unisim::util::debug::Watchpoint;
using unisim::util::debug::Symbol;

using unisim::kernel::Client;
using unisim::kernel::Object;
using unisim::kernel::Service;
using unisim::kernel::VariableBase;
using unisim::kernel::variable::Parameter;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;

using unisim::service::interfaces::DebugEventListener;
using unisim::service::interfaces::DebugEventTrigger;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Monitor_if;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Registers;

using unisim::service::monitor::MonitorServer;

namespace unisim {
namespace service {
namespace monitor {

typedef enum { GDB_LITTLE_ENDIAN, GDB_BIG_ENDIAN } GDBEndian;

template <class ADDRESS>
class Monitor
	 : public Service<Monitor_if<ADDRESS> >

	, public Service<DebugEventListener<ADDRESS> >
	, public unisim::kernel::Client<DebugEventTrigger<ADDRESS> >
	, public Client<SymbolTableLookup<ADDRESS> >
	, public Client<Memory<ADDRESS> >
	, public Client<Registers>

{
public:
	ServiceExport<Monitor_if<ADDRESS> > monitor_export;

	ServiceExport<DebugEventListener<ADDRESS> > debug_event_listener_export;
	ServiceImport<DebugEventTrigger<ADDRESS> > debug_event_trigger_import;

	ServiceImport<SymbolTableLookup<ADDRESS> > symbol_table_lookup_import;

	ServiceImport<Memory<ADDRESS> > memory_import;
	ServiceImport<Registers> registers_import;

	Monitor(const char *name, Object *parent = 0, const char *description = 0);
	virtual ~Monitor();

	// DebugEventListener
	virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS>* event);

	virtual void OnDisconnect();
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *service_export);
	virtual bool EndSetup();

	virtual int generate_monitor_spec(const char* file_path);
	virtual void getProperties(std::vector<std::string>&);
	virtual void refresh_value(const char* name, bool value);
	virtual void refresh_value(const char* name, double value);

	virtual void refresh_value(const char* name, bool value, double time);
	virtual void refresh_value(const char* name, double value, double time);

	virtual void refresh_time(double time);

	bool isProperty(const char* name);

protected:

private:
	MonitorServer<ADDRESS>* server;

	std::string xml_spec_file_path;
	Parameter<std::string> param_spec_xml_file_path;

	unisim::kernel::logger::Logger logger;

	double local_time;

	GDBEndian endian;

	bool verbose;
	Parameter<bool> param_verbose;

	vector<string> properties;
	string propertyList;
	Parameter<string> param_propertyList;

	bool InternalReadMemory(ADDRESS addr, uint32_t size, string& packet);
};

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

#endif
