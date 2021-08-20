
#ifndef __UNISIM_SERVICE_ARTIMON_MONITOR_HH__
#define __UNISIM_SERVICE_ARTIMON_MONITOR_HH__

#include <inttypes.h>

#include <vector>
#include <string>

#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/monitor_if.hh>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ARTIMON_WITH_IO
#define ARTIMON_WITH_IO
#endif

#include <artimon.h>

#ifdef __cplusplus
}
#endif

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::ServiceExportBase;

using unisim::service::interfaces::Monitor_if;

namespace unisim {
namespace service {
namespace monitor {

template <class ADDRESS>
class ArtimonMonitor
		: public Object
		, public Monitor_if<ADDRESS>

{
public:
	ArtimonMonitor(const char *name, Object *parent = 0, const char *description = 0);
	virtual ~ArtimonMonitor();

	virtual void OnDisconnect();
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *service_export);
	virtual bool EndSetup();

	virtual int generate_monitor_spec(const char* file_path);
	virtual void getProperties(std::vector<std::string>&);
	int getPropertyIndex(const char* name);

	virtual void refresh_value(const char* name, bool value);
	virtual void refresh_value(const char* name, double value);

	virtual void refresh_value(const char* name, bool value, double time);
	virtual void refresh_value(const char* name, double value, double time);

	virtual void refresh_time(double time);

protected:

private:
	int nb_main;

	double * main_values;

	int nb_atoms, nb_sig;

	bool isInitialized;

};

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

#endif

