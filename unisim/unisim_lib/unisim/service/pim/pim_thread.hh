/*
 * pim_thread.hh
 *
 *  Created on: 20 janv. 2011
 *      Author: rnouacer
 */

#ifndef PIM_THREAD_HH_
#define PIM_THREAD_HH_

#include <fstream>

#include <math.h>
#include <unistd.h>

#include <unisim/kernel/kernel.hh>

#include <unisim/service/interfaces/time.hh>

#include <unisim/util/converter/convert.hh>
#include <unisim/service/time/sc_time/time.hh>

#include <unisim/service/pim/network/SocketThread.hpp>
#include <unisim/service/pim/gdbthread.hh>

using namespace std;

namespace unisim {
namespace service {
namespace pim {

using unisim::kernel::Object;
using unisim::kernel::VariableBaseListener;
using unisim::kernel::Simulator;
using unisim::kernel::VariableBase;

class PIMThread : virtual public Object, public SocketThread, VariableBaseListener {
public:
	PIMThread(const char *_name, Object *_parent = 0);
	virtual ~PIMThread();

	virtual void run();

	double GetSimTime();
	bool UpdateTimeRatio();
	double GetLastTimeRatio();

	virtual void VariableBaseNotify(const VariableBase *var);

private:
	string name;
	double last_time_ratio;
	ofstream pim_trace_file;

	GDBThread *gdbThread;

};

} // end pim
} // end service
} // end unisim


#endif /* PIM_THREAD_HH_ */
