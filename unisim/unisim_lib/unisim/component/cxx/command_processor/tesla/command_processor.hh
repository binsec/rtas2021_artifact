/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */
 
#ifndef UNISIM_COMPONENT_CXX_COMMAND_PROCESSOR_TESLA_COMMAND_PROCESSOR_HH
#define UNISIM_COMPONENT_CXX_COMMAND_PROCESSOR_TESLA_COMMAND_PROCESSOR_HH

#include <boost/shared_ptr.hpp>
#include <boost/integer.hpp>
#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/typed_registers.hh>
#include <unisim/service/interfaces/scheduler.hh>
#include <unisim/component/cxx/processor/tesla/interfaces.hh>
#include <unisim/component/cxx/scheduler/cuda_scheduler/cuda_scheduler.hh>
#include <unisim/component/cxx/command_processor/tesla/interfaces.hh>

using boost::shared_ptr;
using unisim::service::interfaces::TypedRegisters;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Scheduler;
using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::Object;
using unisim::component::cxx::processor::tesla::GPRID;
using unisim::component::cxx::processor::tesla::SMAddress;
using unisim::component::cxx::processor::tesla::ConfigurationRegisterID;

namespace unisim {
namespace component {
namespace cxx {
namespace command_processor {
namespace tesla {

struct Event
{
	void Wait();
	float ElapsedTime();
};

struct Context
{
};

template<class CONFIG>
struct CommandProcessor : virtual Object
{
    CommandFactory & Context(ctx);
    
    void Synchronize();
    void Start();
    void Stop();
};

} // end of namespace tesla
} // end of namespace command_processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
