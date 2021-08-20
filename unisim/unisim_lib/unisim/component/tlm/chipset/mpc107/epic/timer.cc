#include "unisim/component/tlm/chipset/mpc107/epic/timer.hh"
#include <systemc>

namespace unisim {
namespace component {
namespace tlm {
namespace chipset {
namespace mpc107 {
namespace epic {
	
void Timer::SetTimer(sc_core::sc_time &time) {
	TimeHandler *handler;
	
	if(free_handlers.empty()) {
		handler = new TimeHandler();
		sc_core::sc_spawn(sc_bind(&Timer::TimeHandlerProcess, this, handler));
	} else {
		handler = free_handlers.front();
		free_handlers.pop_front();
	}
	
	handler->time = time;
	handler->event.notify(sc_core::SC_ZERO_TIME);
}

void Timer::TimeHandlerProcess(TimeHandler *handler) {
	while(1) {
		wait(handler->event);
		wait(handler->time);
		TimeEvent();
		free_handlers.push_back(handler);
	}
}

} // end of epic namespace
} // end of mpc107 namespace	
} // end of chipset namespace
} // end of tlm namespace
} // end of component namespace
} // end of unisim namespace
