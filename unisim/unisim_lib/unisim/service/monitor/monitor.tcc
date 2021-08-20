
#include <unisim/service/monitor/monitor.hh>

#include <unisim/util/converter/convert.hh>

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;


namespace unisim {
namespace service {
namespace monitor {

template <class ADDRESS>
Monitor<ADDRESS>::Monitor(const char *name, Object *parent, const char *description)
	: Object(name, parent)
	, Service<Monitor_if<ADDRESS> >(name, parent)
	, Service<DebugEventListener<ADDRESS> >(name, parent)
	, Client<DebugEventTrigger<ADDRESS> >(name, parent)
	, Client<SymbolTableLookup<ADDRESS> >(name, parent)
	, Client<Memory<ADDRESS> >(name, parent)
	, Client<Registers>(name, parent)

	, monitor_export("monitor-export", this)
	, debug_event_listener_export("debug-event-listener-export", this)
	, debug_event_trigger_import("debug-event-trigger-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, memory_import("memory-import", this)
	, registers_import("registers-import", this)

	, xml_spec_file_path("xml_spec_file_path.xml")
	, param_spec_xml_file_path("xml-spec-file-path", this, xml_spec_file_path)

	, logger(*this)

	, local_time(0)

	, endian (GDB_BIG_ENDIAN)

	, verbose(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")

	, propertyList("")
	, param_propertyList("property-list", this, propertyList, "List of properties to monitor. The list use white spaces as a separator e.g. p1 p2")


{
	server = MonitorServer<ADDRESS>::getInstance(this);
}

template <class ADDRESS>
Monitor<ADDRESS>::~Monitor()
{
	MonitorServer<ADDRESS>::releaseInstance();
}

template <class ADDRESS>
void Monitor<ADDRESS>::OnDisconnect()
{

}

template <class ADDRESS>
bool Monitor<ADDRESS>::BeginSetup()
{
	// int result =
          generate_monitor_spec(xml_spec_file_path.c_str());

	stringSplit(propertyList, " ", properties);

	return true;
}

template <class ADDRESS>
bool Monitor<ADDRESS>::Setup(ServiceExportBase *service_export)
{
	return true;
}

template <class ADDRESS>
bool Monitor<ADDRESS>::EndSetup()
{
	bool has_architecture_endian = false;

	VariableBase* architecture_endian = Object::GetSimulator()->FindParameter("endian");
	if (architecture_endian != NULL) {
		string endianstr = *architecture_endian;
		if(endianstr == "little")
		{
			endian = GDB_LITTLE_ENDIAN;
			has_architecture_endian = true;
		}
		else if(endianstr == "big")
		{
			endian = GDB_BIG_ENDIAN;
			has_architecture_endian = true;
		}
	}
	
	if(!has_architecture_endian)
	{
		logger << DebugWarning << "Endian is not set; Assuming " << endian << " for architecture" << EndDebugWarning;
	}

	//*** Hack code to test if monitor is working well */

	if (symbol_table_lookup_import) {

		vector<string> vect;
		getProperties(vect);
		for (unsigned i=0; i<vect.size(); i++) {
			string property = vect[i];

			const unisim::util::debug::Symbol<ADDRESS> *symb = symbol_table_lookup_import->FindSymbolByName(property.c_str(), Symbol<ADDRESS>::SYM_OBJECT);
			if (symb != NULL) {

				ADDRESS addr = symb->GetAddress();
				uint32_t size = symb->GetSize();

				if(debug_event_trigger_import->SetWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size, false))
				{
	//				std::cout << "EndSetup (4)" << std::endl;

				} else {

	//				std::cout << "EndSetup (5)" << std::endl;

				}

			}

		}
	}

	//**  Hack code end */

	return true;
}

template <class ADDRESS>
int Monitor<ADDRESS>::generate_monitor_spec(const char* file_path) {
	return (server->generate_monitor_spec(file_path));
}

template <class ADDRESS>
void Monitor<ADDRESS>::getProperties(std::vector<std::string>& vect) {

	vect.clear();
	vect = properties;

	// TODO: Final version has to get properties from monitor implementation (default_monitor and artimon_monitor)
}

template <class ADDRESS>
void Monitor<ADDRESS>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS>* event)
{

	switch(event->GetType())
	{
		case unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT:
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT: {
			const Watchpoint<ADDRESS> *watchpoint_hit = dynamic_cast<const Watchpoint<ADDRESS> *> (event);

			string name;

			list<const Symbol<ADDRESS> *> symbol_registries;

			if (symbol_table_lookup_import) {
				symbol_table_lookup_import->GetSymbols(symbol_registries, Symbol<ADDRESS>::SYM_OBJECT);

			}

			typename list<const Symbol<ADDRESS> *>::const_iterator symbol_iter;

			string value;

			for(symbol_iter = symbol_registries.begin(); symbol_iter != symbol_registries.end(); symbol_iter++)
			{

				if ((*symbol_iter)->GetAddress() == watchpoint_hit->GetAddress()) {

					name = (*symbol_iter)->GetName();
					value = "";

					if(!InternalReadMemory((*symbol_iter)->GetAddress(), (*symbol_iter)->GetSize(), value))
					{
						if(verbose)
						{
							logger << DebugWarning << memory_import.GetName() << "->ReadSymbol has reported an error" << EndDebugWarning;
						}
					}

					unsigned long d = 0;
					hexString2Number(value, &d, (*symbol_iter)->GetSize(), (endian == GDB_BIG_ENDIAN)? "big":"little");

//					refresh_value(name.c_str(), (double) d, local_time++);
					refresh_value(name.c_str(), (double) d, Object::GetSimulator()->GetSimTime()*1000);

					break;
				}

			}

		}
			break;
		default:
			// ignore event
			return;
	}

}

template <class ADDRESS>
bool Monitor<ADDRESS>::InternalReadMemory(ADDRESS addr, uint32_t size, string& packet)
{
	bool read_error = false;
	bool overwrapping = false;

	char ch[2];
	ch[1] = 0;

	if(size > 0)
	{
		do
		{
			uint8_t value = 0;
			if(!overwrapping)
			{
				if(!memory_import->ReadMemory(addr, &value, 1))
				{
					read_error = true;
					value = 0;
				}
			}
			ch[0] = nibble2HexChar(value >> 4);
			packet += ch;
			ch[0] = nibble2HexChar(value & 0xf);
			packet += ch;
			if((addr + 1) == 0) overwrapping = true;
		} while(++addr, --size);
	}

	if(read_error)
	{
		return (false);
	}

	return (true);
}

template <class ADDRESS>
void Monitor<ADDRESS>::refresh_value(const char* name, bool value)
{
	server->refresh_value(name, value);
}

template <class ADDRESS>
void Monitor<ADDRESS>::refresh_value(const char* name, double value)
{
	server->refresh_value(name, value);
}

template <class ADDRESS>
void Monitor<ADDRESS>::refresh_value(const char* name, bool value, double time)
{
	server->refresh_value(name, value, time);
}

template <class ADDRESS>
void Monitor<ADDRESS>::refresh_value(const char* name, double value, double time)
{
	server->refresh_value(name, value, time);
}


template <class ADDRESS>
void Monitor<ADDRESS>::refresh_time(double time)
{
	server->refresh_time(time);
}

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim


