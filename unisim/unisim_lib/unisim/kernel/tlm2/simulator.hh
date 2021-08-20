/*
 *  Copyright (c) 2015-2016,
 *  Commissariat a l'Energie Atomique et aux Energies Alternatives (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_KERNEL_TLM2_SIMULATOR_HH__
#define __UNISIM_KERNEL_TLM2_SIMULATOR_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <systemc>
#include <tlm>

namespace unisim {
namespace kernel {
namespace tlm2 {

class Simulator
	: public unisim::kernel::Simulator
	, public unisim::kernel::Object
	, public sc_core::sc_module
{
public:
	Simulator(sc_core::sc_module_name const& name, int argc, char **argv, void (*LoadBuiltInConfig)(unisim::kernel::Simulator *simulator) = 0);
	virtual ~Simulator();
	void Run();
	virtual void Stop(unisim::kernel::Object *object, int exit_status, bool asynchronous = false);
	int GetExitStatus() const;
	bool IsRunning() const;
	bool SimulationStarted() const;
	bool SimulationFinished() const;
protected:
	unisim::kernel::logger::Logger logger;
private:
	sc_core::sc_time max_time;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_max_time;
	unisim::kernel::variable::Statistic<sc_core::sc_time> stat_cur_sim_time;
	sc_core::sc_time global_quantum;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_global_quantum;
	sc_core::sc_time can_global_quantum;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_can_global_quantum;
	bool stop_called;
	int exit_status;
};

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_TLM2_SIMULATOR_HH__
