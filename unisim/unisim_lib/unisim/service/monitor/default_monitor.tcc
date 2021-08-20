

#include <unisim/service/monitor/default_monitor.hh>

#include <iostream>

namespace unisim {
namespace service {
namespace monitor {

template <class ADDRESS>
DefaultMonitor<ADDRESS>::DefaultMonitor(const char *name, Object *parent, const char *description)
	: Object(name, parent)
	, last_time(0)

{

}

template <class ADDRESS>
DefaultMonitor<ADDRESS>::~DefaultMonitor()
{
	output_file.close();
}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::OnDisconnect()
{

}

template <class ADDRESS>
bool DefaultMonitor<ADDRESS>::BeginSetup()
{
	output_file.open ("monitor_output.txt", std::ofstream::out);

	if (output_file.is_open())
	{
		output_file <<"Time , Property-name , value" << std::endl;
		output_file <<"-------------------------------" << std::endl;
	} else {
		std::cerr << "Error opening file -> monitor_output.txt";
	}

	return true;
}

template <class ADDRESS>
bool DefaultMonitor<ADDRESS>::Setup(ServiceExportBase *service_export)
{
	return true;
}

template <class ADDRESS>
bool DefaultMonitor<ADDRESS>::EndSetup()
{
	return true;
}

template <class ADDRESS>
int DefaultMonitor<ADDRESS>::generate_monitor_spec(const char* file_path) {
	return (0);
}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::getProperties(std::vector<std::string>& vect) {

}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::refresh_value(const char* name, bool value)
{

	output_file << last_time << " , " << name << " , " << value << std::endl;
}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::refresh_value(const char* name, double value)
{

	output_file << last_time << " , " << name << " , " << value << std::endl;

}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::refresh_value(const char* name, bool value, double time)
{

	last_time = time;

	output_file << last_time << " , " << name << " , " << value << std::endl;

}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::refresh_value(const char* name, double value, double time)
{

	last_time = time;

	output_file << last_time << " , " << name << " , " << value << std::endl;

}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::refresh_time(double time)
{

	last_time = time;
}

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

