#include "simulator.hh"

extern "C" int __u_start(int argc, char **argv, int (*callback)(void));
extern "C" int __u_set_breakpoint(const char *info, void (*callback)(int));
extern "C" int __u_set_watchpoint(const char *info, void (*callback)(int));
extern "C" int __u_delete_breakpoint(int);
extern "C" int __u_delete_watchpoint(int);
extern "C" int __u_lookup_data_object(const char *info, size_t size);
extern "C" int __u_val(int handler, void *value);
extern "C" int __u_stop(int);

int __u_start(int argc, char **argv, int (*callback)(void))
{
	Simulator::EnableMonitor(callback);
	
	return sc_core::sc_elab_and_sim(argc, argv);
}

int __u_set_breakpoint(const char *info, void (*callback)(int))
{
	unisim::service::debug::monitor::MonitorBase *monitor = unisim::service::debug::monitor::MonitorBase::GetInstance();
	
	return monitor->SetBreakpoint(info, callback);
}

int __u_set_watchpoint(const char *info, void (*callback)(int))
{
	unisim::service::debug::monitor::MonitorBase *monitor = unisim::service::debug::monitor::MonitorBase::GetInstance();
	
	return monitor->SetWatchpoint(info, callback);
}

int __u_delete_breakpoint(int handle)
{
	unisim::service::debug::monitor::MonitorBase *monitor = unisim::service::debug::monitor::MonitorBase::GetInstance();
	
	return monitor->DeleteBreakpoint(handle);
}

int __u_delete_watchpoint(int handle)
{
	unisim::service::debug::monitor::MonitorBase *monitor = unisim::service::debug::monitor::MonitorBase::GetInstance();
	
	return monitor->DeleteWatchpoint(handle);
}

int __u_lookup_data_object(const char *info, size_t size)
{
	unisim::service::debug::monitor::MonitorBase *monitor = unisim::service::debug::monitor::MonitorBase::GetInstance();
	
	return monitor->LookupDataObject(info, size);
}

int __u_val(int handle, void *value)
{
	unisim::service::debug::monitor::MonitorBase *monitor = unisim::service::debug::monitor::MonitorBase::GetInstance();
	
	return monitor->GetDataObjectValue(handle, value);
}

int __u_stop(int exit_status)
{
	unisim::service::debug::monitor::MonitorBase *monitor = unisim::service::debug::monitor::MonitorBase::GetInstance();
	
	return monitor->StopMe(exit_status);
}
