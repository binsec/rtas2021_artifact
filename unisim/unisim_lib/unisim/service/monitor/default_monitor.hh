#ifndef __UNISIM_SERVICE_DEFAULT_MONITOR_HH__
#define __UNISIM_SERVICE_DEFAULT_MONITOR_HH__

#include <inttypes.h>

#include <fstream>

#include <unisim/kernel/kernel.hh>

#include <unisim/service/interfaces/monitor_if.hh>

using unisim::kernel::Object;
using unisim::kernel::ServiceExportBase;

using unisim::service::interfaces::Monitor_if;

namespace unisim {
namespace service {
namespace monitor {

template <class ADDRESS>
class DefaultMonitor
	: public Object
	, public Monitor_if<ADDRESS>

{
public:
	DefaultMonitor(const char *name, Object *parent = 0, const char *description = 0);
	virtual ~DefaultMonitor();

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

protected:

private:
	std::ofstream output_file;
	double last_time;

};

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

#endif

