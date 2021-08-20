/*
 *  Copyright (c) 2007,
 *  Commissariat a l'Energie Atomique (CEA)
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
 
#ifndef __UNISIM_COMPONENT_TLM_MEMORY_FLASH_AM29_AM29_HH__
#define __UNISIM_COMPONENT_TLM_MEMORY_FLASH_AM29_AM29_HH__

#include <systemc>
#include <unisim/component/tlm/message/memory.hh>
#include <unisim/kernel/tlm/tlm.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <unisim/component/cxx/memory/flash/am29/am29.hh>

namespace unisim {
namespace component {
namespace tlm {
namespace memory {
namespace flash {
namespace am29 {

using unisim::kernel::tlm::TlmMessage;
using unisim::kernel::tlm::TlmSendIf;
using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::util::garbage_collector::Pointer;
using unisim::component::cxx::memory::flash::am29::CMD_READ;
using unisim::component::cxx::memory::flash::am29::CMD_WRITE;

using unisim::component::tlm::message::MemoryRequest;
using unisim::component::tlm::message::MemoryResponse;

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH, uint32_t MAX_TRANSACTION_DATA_SIZE>
class AM29 :
	public sc_core::sc_module,
	public unisim::component::cxx::memory::flash::am29::AM29<CONFIG, BYTESIZE, IO_WIDTH>,
	public TlmSendIf<MemoryRequest<typename CONFIG::ADDRESS, MAX_TRANSACTION_DATA_SIZE>, MemoryResponse<MAX_TRANSACTION_DATA_SIZE> >
{
public:
	typedef unisim::component::cxx::memory::flash::am29::AM29<CONFIG, BYTESIZE, IO_WIDTH> inherited;

	sc_core::sc_export<TlmSendIf<MemoryRequest<typename CONFIG::ADDRESS, MAX_TRANSACTION_DATA_SIZE>, 
						MemoryResponse<MAX_TRANSACTION_DATA_SIZE> > > slave_port;

	AM29(const sc_core::sc_module_name& name, Object *parent = 0);
	virtual ~AM29();
	virtual bool BeginSetup();
	virtual bool Send(const Pointer<TlmMessage<MemoryRequest<typename CONFIG::ADDRESS, MAX_TRANSACTION_DATA_SIZE>, MemoryResponse<MAX_TRANSACTION_DATA_SIZE> > >& message);
	void Process();
private:
	sc_core::sc_fifo<Pointer<TlmMessage<MemoryRequest<typename CONFIG::ADDRESS, MAX_TRANSACTION_DATA_SIZE>, MemoryResponse<MAX_TRANSACTION_DATA_SIZE> > > > input_queue;
	sc_core::sc_time cycle_time;
	Parameter<sc_core::sc_time> param_cycle_time;
};

} // end of namespace am29
} // end of namespace flash
} // end of namespace memory
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM_MEMORY_FLASH_AM29_AM29_HH__
