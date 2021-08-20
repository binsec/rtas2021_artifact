
#include <unisim/service/monitor/monitor_server.hh>
#include <cassert>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_ARTIMON
	#include <unisim/service/monitor/artimon_monitor.hh>
	using unisim::service::monitor::ArtimonMonitor;
#else
	#include <unisim/service/monitor/default_monitor.hh>
	using unisim::service::monitor::DefaultMonitor;
#endif //  HAVE_ARTIMON

namespace unisim {
namespace service {
namespace monitor {

template <class ADDRESS>
MonitorServer<ADDRESS>* MonitorServer<ADDRESS>::singleton = 0;

template <class ADDRESS>
int MonitorServer<ADDRESS>::singletonRef = 0;

template <class ADDRESS>
MonitorServer<ADDRESS>::MonitorServer(const char *name, Object *parent, const char *description)
	: Object(name, parent)
{

#ifdef HAVE_ARTIMON
		monitor = new ArtimonMonitor<ADDRESS>("ArtimonMonitor", this);
#else
		monitor = new DefaultMonitor<ADDRESS>("DefaultMonitor", this);
#endif //  HAVE_ARTIMON

}

template <class ADDRESS>
MonitorServer<ADDRESS>::~MonitorServer()
{
	if (monitor) { delete monitor; monitor = NULL; }
}

template <class ADDRESS>
void MonitorServer<ADDRESS>::OnDisconnect()
{

}

template <class ADDRESS>
bool MonitorServer<ADDRESS>::BeginSetup()
{
	return true;
}

template <class ADDRESS>
bool MonitorServer<ADDRESS>::Setup(ServiceExportBase *service_export)
{
	return true;
}

template <class ADDRESS>
bool MonitorServer<ADDRESS>::EndSetup()
{
	return true;
}

template <class ADDRESS>
MonitorServer<ADDRESS>* MonitorServer<ADDRESS>::getInstance(Object *parent)
{
	if (singleton == NULL) {

		singleton = new MonitorServer<ADDRESS>("MonitorServer", parent);
		singletonRef = 0;
	}

	singletonRef++;

	return singleton;
}

template <class ADDRESS>
void MonitorServer<ADDRESS>::releaseInstance()
{
	assert((singletonRef > 0) && (singleton != NULL));

	singletonRef--;

	if (singletonRef == 0) {
		delete singleton; singleton = NULL; singletonRef = 0;
	}
}

template <class ADDRESS>
int MonitorServer<ADDRESS>::generate_monitor_spec(const char* file_path) {
	return monitor->generate_monitor_spec(file_path);
}

template <class ADDRESS>
void MonitorServer<ADDRESS>::getProperties(std::vector<std::string>& vect) {
	monitor->getProperties(vect);
}

template <class ADDRESS>
void MonitorServer<ADDRESS>::refresh_value(const char* name, bool value)
{
	monitor->refresh_value(name, value);
}

template <class ADDRESS>
void MonitorServer<ADDRESS>::refresh_value(const char* name, double value)
{
	monitor->refresh_value(name, value);
}

template <class ADDRESS>
void MonitorServer<ADDRESS>::refresh_value(const char* name, bool value, double time)
{
	monitor->refresh_value(name, value, time);
}

template <class ADDRESS>
void MonitorServer<ADDRESS>::refresh_value(const char* name, double value, double time)
{
	monitor->refresh_value(name, value, time);
}

template <class ADDRESS>
void MonitorServer<ADDRESS>::refresh_time(double time)
{
	monitor->refresh_time(time);
}


} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim


