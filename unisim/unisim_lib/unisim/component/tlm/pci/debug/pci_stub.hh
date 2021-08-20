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

#ifndef __UNISIM_COMPONENT_TLM_PCI_DEBUG_PCI_STUB_HH__
#define __UNISIM_COMPONENT_TLM_PCI_DEBUG_PCI_STUB_HH__

#include <systemc>
#include "unisim/kernel/tlm/tlm.hh"
#include "unisim/kernel/kernel.hh"
#include "unisim/util/garbage_collector/garbage_collector.hh"
#include "unisim/service/interfaces/memory.hh"
#include <unisim/component/tlm/message/pci.hh>
#include <unisim/component/tlm/message/interrupt.hh>
#include <unisim/component/cxx/pci/debug/pci_stub.hh>
#include <string>
//#include <values.h>
#include <math.h>

namespace unisim {
namespace component {
namespace tlm {
namespace pci {
namespace debug {

using namespace std;

using unisim::kernel::tlm::TlmSendIf;
using unisim::kernel::tlm::TlmMessage;
using unisim::component::tlm::message::PCIRequest;
using unisim::component::tlm::message::PCIResponse;
using unisim::component::cxx::pci::TT_READ;
using unisim::component::cxx::pci::TT_WRITE;
using unisim::component::cxx::pci::SP_MEM;
using unisim::component::cxx::pci::SP_IO;
using unisim::component::cxx::pci::SP_CONFIG;


using unisim::kernel::Object;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::variable::Parameter;
using unisim::util::garbage_collector::Pointer;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::service::interfaces::Memory;
using unisim::component::tlm::message::InterruptRequest;
using unisim::component::cxx::pci::TransactionType;
using unisim::component::cxx::pci::PCISpace;

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
class PCIStub :
	public sc_core::sc_module,
	public TlmSendIf<PCIRequest<ADDRESS_TYPE, MAX_DATA_SIZE>, PCIResponse<MAX_DATA_SIZE> >,
	public unisim::component::cxx::pci::debug::PCIStub<ADDRESS_TYPE>
{
public:

	typedef unisim::component::cxx::pci::debug::PCIStub<ADDRESS_TYPE> inherited;
	typedef PCIRequest<ADDRESS_TYPE, MAX_DATA_SIZE> PCIReq;
	typedef PCIResponse<MAX_DATA_SIZE> PCIRsp;
	
	// from PCI bus
	sc_core::sc_export<TlmSendIf<PCIReq, PCIRsp> > bus_port;

	// to CPU
	sc_core::sc_port<TlmSendIf<InterruptRequest> > cpu_irq_port;

	PCIStub(const sc_core::sc_module_name& name, Object *parent = 0);
	virtual ~PCIStub();
	virtual bool Send(const Pointer<TlmMessage<PCIReq, PCIRsp> > &message);
	virtual bool EndSetup();
	virtual void ServeStart();
	virtual void ServeStop();
	virtual void ServeIntr(uint32_t intr_id, bool level);
	virtual void ServeRun(uint64_t duration, typename inherited::inherited::TIME_UNIT duration_tu, uint64_t& t, typename inherited::inherited::TIME_UNIT& tu, list<typename inherited::inherited::TRAP>& traps);
	void TriggerInterrupt();
	void Process();
	virtual void Trap();
private:
	
	sc_core::sc_time pci_bus_cycle_time;
	sc_core::sc_time bus_cycle_time;
	sc_core::sc_fifo<bool> intr_fifo;
	sc_core::sc_event trap;
	bool stopped;
};

} // end of namespace debug
} // end of namespace pci
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_TLM_PCI_DEBUG_PCI_STUB_HH__ */
