#ifndef __UNISIM_COMPONENT_TLM_CHIPSET_MPC107_EPIC_TIMER_HH__
#define __UNISIM_COMPONENT_TLM_CHIPSET_MPC107_EPIC_TIMER_HH__

#include <systemc>
#include <list>

namespace unisim {
namespace component {
namespace tlm {
namespace chipset {
namespace mpc107 {
namespace epic {

using std::list;
	
class Timer {
private:
	/* forward declaration */
	class TimeHandler;
	
public:
	virtual void TimeEvent() = 0;

	void SetTimer(sc_core::sc_time &time);
	
private:
	void TimeHandlerProcess(TimeHandler *handler);
	
	class TimeHandler {
	public:
		sc_core::sc_time time;
		sc_core::sc_event event;
	};
	
	list<TimeHandler *> free_handlers;
};

} // end of epic namespace
} // end of mpc107 namespace	
} // end of chipset namespace
} // end of tlm namespace
} // end of component namespace
} // end of unisim namespace

#endif // __UNISIM_COMPONENT_TLM_CHIPSET_MPC107_EPIC_TIMER_HH__
