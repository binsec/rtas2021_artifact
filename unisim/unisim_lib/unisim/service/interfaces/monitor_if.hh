
#ifndef __UNISIM_SERVICE_INTERFACES_MONITOR_IF_HH__
#define __UNISIM_SERVICE_INTERFACES_MONITOR_IF_HH__

#include <unisim/service/interfaces/interface.hh>

#include <vector>

namespace unisim {
namespace service {
namespace interfaces {

template <class ADDRESS>
class Monitor_if : public ServiceInterface
{
public:
	virtual int generate_monitor_spec(const char* file_path) = 0;
	virtual void getProperties(std::vector<std::string>&) = 0;

	virtual void refresh_value(const char* name, bool value) = 0;
	virtual void refresh_value(const char* name, double value) = 0;

	virtual void refresh_value(const char* name, bool value, double time) = 0;
	virtual void refresh_value(const char* name, double value, double time) = 0;

	virtual void refresh_time(double time) = 0;

};

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

#endif
