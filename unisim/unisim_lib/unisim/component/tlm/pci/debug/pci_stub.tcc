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

#ifndef __UNISIM_COMPONENT_TLM_PCI_DEBUG_PCI_STUB_TCC__
#define __UNISIM_COMPONENT_TLM_PCI_DEBUG_PCI_STUB_TCC__

namespace unisim {
namespace component {
namespace tlm {
namespace pci {
namespace debug {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::PCIStub(const sc_core::sc_module_name& name, Object *parent) :
	Object(name, parent, "A module that implements a PCI target and acts as a co-simulation stub controlled over a TCP/IP or pipe connection."),
	sc_core::sc_module(name),
	unisim::component::cxx::pci::debug::PCIStub<ADDRESS_TYPE>(name, parent),
	bus_port("bus-port"),
	cpu_irq_port("cpu-irq-port"),
	pci_bus_cycle_time(),
	bus_cycle_time()
{
	SC_HAS_PROCESS(PCIStub);
	
	bus_port(*this);
	
	SC_THREAD(TriggerInterrupt);
	SC_THREAD(Process);
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::~PCIStub()
{
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
bool PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::Send(const Pointer<TlmMessage<PCIReq, PCIRsp> > &message)
{
	const Pointer<PCIRequest<ADDRESS_TYPE, MAX_DATA_SIZE> >& pci_req = message->GetRequest();
	ADDRESS_TYPE pci_addr = pci_req->addr;
	PCISpace pci_space = pci_req->space;
	unsigned int pci_req_size = pci_req->size;
	TransactionType pci_transaction_type = pci_req->type;
	
	switch(pci_transaction_type)
	{
		case TT_READ:
			{
				Pointer<PCIRsp> pci_rsp = new(pci_rsp) PCIRsp();
			
				inherited::Read(pci_addr, pci_rsp->read_data, pci_req_size, pci_space);
			
				message->rsp = pci_rsp;
				sc_core::sc_event *rsp_ev = message->GetResponseEvent();
				rsp_ev->notify(pci_bus_cycle_time);
			}
			break;
		case TT_WRITE:
			inherited::Write(pci_addr, pci_req->write_data, pci_req_size, pci_space);
			break;
	}
		
	return true;
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
bool PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::EndSetup()
{
	if(!inherited::EndSetup()) return false;
	pci_bus_cycle_time = sc_core::sc_time(1.0 / (double) (*this)["pci-bus-frequency"], sc_core::SC_US);
	bus_cycle_time = sc_core::sc_time(1.0 / (double) (*this)["bus-frequency"], sc_core::SC_US);
	return true;
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::ServeStart()
{
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::ServeStop()
{
	Object::Stop(-1);
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::ServeIntr(uint32_t intr_id, bool level)
{
	intr_fifo.write(level);
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::ServeRun(uint64_t duration, typename inherited::inherited::TIME_UNIT duration_tu, uint64_t& t, typename inherited::inherited::TIME_UNIT& tu, list<typename inherited::inherited::TRAP>& traps)
{
	if(duration)
	{
		sc_core::sc_time_unit sc_tu = sc_core::SC_MS;
		
		switch(duration_tu)
		{
			case inherited::inherited::TU_FS: sc_tu = sc_core::SC_FS; break;
			case inherited::inherited::TU_PS: sc_tu = sc_core::SC_PS; break;
			case inherited::inherited::TU_NS: sc_tu = sc_core::SC_NS; break;
			case inherited::inherited::TU_US: sc_tu = sc_core::SC_US; break;
			case inherited::inherited::TU_MS: sc_tu = sc_core::SC_MS; break;
			case inherited::inherited::TU_S: sc_tu = sc_core::SC_SEC; break;
		}
		inherited::synchronizable_import->Synchronize();
		wait(sc_core::sc_time((double) duration, sc_tu), trap);
	}
	else
	{
		wait(trap);
	}

	sc_core::sc_time time_res = sc_core::sc_get_time_resolution();
	double time_res_sec = time_res.to_seconds();

	if(::fabs(time_res_sec - 1.0) <= 0.5) tu = inherited::inherited::TU_S; else
	if(::fabs(time_res_sec - 1.0e-3) <= 0.5e-3) tu = inherited::inherited::TU_MS; else
	if(::fabs(time_res_sec - 1.0e-6) <= 0.5e-6) tu = inherited::inherited::TU_US; else
	if(::fabs(time_res_sec - 1.0e-9) <= 0.5e-9) tu = inherited::inherited::TU_NS; else
	if(::fabs(time_res_sec - 1.0e-12) <= 0.5e-12) tu = inherited::inherited::TU_PS; else
	if(::fabs(time_res_sec - 1.0e-15) <= 0.5e-15) tu = inherited::inherited::TU_FS; else ServeStop();
	
	t = (uint64_t) sc_core::sc_time_stamp().value();

	traps = inherited::traps;
	inherited::traps.clear();
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::TriggerInterrupt()
{
	while(1)
	{
		bool level = intr_fifo.read();
		
		Pointer<TlmMessage<InterruptRequest> > message = new(message) TlmMessage<InterruptRequest>();
		Pointer<InterruptRequest> irq = new (irq) InterruptRequest();
		irq->level = level;
		irq->serial_id = 0;
		message->req = irq;
		
		if(inherited::verbose)
		{
			inherited::logger << DebugInfo;
			inherited::logger << "sending interrupt request (level = " << level << ") at " << sc_core::sc_time_stamp().to_string() << std::endl;
			inherited::logger << EndDebugInfo;
		}

		while(!cpu_irq_port->Send(message))
		{
			wait(bus_cycle_time);
		}

		if(inherited::verbose)
		{
			inherited::logger << DebugInfo;
			inherited::logger << "accepted interrupt request (level = " << level << ") at " << sc_core::sc_time_stamp().to_string() << std::endl;
			inherited::logger << EndDebugInfo;
		}
	}
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::Process()
{
	while(1)
	{
		inherited::inherited::Serve();
	}
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::Trap()
{
	trap.notify(sc_core::SC_ZERO_TIME);
	wait(sc_core::SC_ZERO_TIME);
}

} // end of namespace debug
} // end of namespace pci
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_TLM_PCI_DEBUG_PCI_STUB_HH__ */
