
#ifndef __UNISIM_SERVICE_MONITOR_SERVER_HH__
#define __UNISIM_SERVICE_MONITOR_SERVER_HH__

#include <inttypes.h>

#include <unisim/kernel/kernel.hh>

#include <unisim/service/interfaces/monitor_if.hh>

using unisim::kernel::Object;
using unisim::kernel::ServiceExportBase;
using unisim::service::interfaces::Monitor_if;

namespace unisim {
namespace service {
namespace monitor {

template <class ADDRESS>
class MonitorServer
	: public Object
	, public Monitor_if<ADDRESS>
{
private:
	MonitorServer(const char *name, Object *parent = 0, const char *description = 0);

public:

	virtual ~MonitorServer();

	virtual void OnDisconnect();
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *service_export);
	virtual bool EndSetup();

	static MonitorServer* getInstance(Object *parent);
	static void releaseInstance();

	virtual int generate_monitor_spec(const char* file_path);
	virtual void getProperties(std::vector<std::string>&);
	virtual void refresh_value(const char* name, bool value);
	virtual void refresh_value(const char* name, double value);

	virtual void refresh_value(const char* name, bool value, double time);
	virtual void refresh_value(const char* name, double value, double time);

	virtual void refresh_time(double time);

protected:

private:
	static MonitorServer* singleton;
	static int singletonRef;
	Monitor_if<ADDRESS> *monitor;

};

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

#endif
