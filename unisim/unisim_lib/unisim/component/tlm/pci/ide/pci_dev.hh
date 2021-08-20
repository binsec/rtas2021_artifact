/*
 *  Copyright (c) 2007,
 *  Universitat Politecnica de Catalunya (UPC)
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
 *   - Neither the name of UPC nor the names of its contributors may be used to
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
 * Authors: Paula Casero (pcasero@upc.edu), Alejandro Schenzle (schenzle@upc.edu)
 */

#ifndef __UNISIM_COMPONENT_TLM_PCI_IDE_PCI_DEV_HH__
#define __UNISIM_COMPONENT_TLM_PCI_IDE_PCI_DEV_HH__

#include <unisim/kernel/tlm/tlm.hh>
#include <unisim/component/tlm/message/pci.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/component/cxx/pci/ide/pcidev.hh>
#include <unisim/component/cxx/pci/ide/pci_master.hh>
#include <unisim/component/tlm/message/interrupt.hh>
#include <unisim/kernel/logger/logger.hh>

namespace unisim {
namespace component {
namespace tlm {
namespace pci {
namespace ide {

using namespace std;
using unisim::kernel::tlm::TlmMessage;
using unisim::kernel::tlm::TlmSendIf;
using unisim::component::tlm::message::PCIRequest;
using unisim::component::tlm::message::PCIResponse;
using unisim::util::garbage_collector::Pointer;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::ParameterArray;
using unisim::util::endian::Host2LittleEndian;
using unisim::component::tlm::message::InterruptRequest;

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE> 
class PCIDev: public sc_core::sc_module,
		virtual public Object,
  		public TlmSendIf<PCIRequest<ADDRESS_TYPE, MAX_DATA_SIZE>, PCIResponse<MAX_DATA_SIZE> >,
  		public virtual unisim::component::cxx::pci::ide::PCIMaster<ADDRESS_TYPE>,
  		public virtual unisim::component::cxx::pci::ide::EventManager
  		//public Service<PCIInterface<ADDRESS_TYPE> >,
  		//public Client<PCIInterface<ADDRESS_TYPE> >
{
private:
/*	static const uint32_t BAR_IO_MASK = 0x3;
	static const uint32_t BAR_MEM_MASK = 0xF;
	static const uint32_t NUM_BARS = 6;
	static const uint32_t VALUE_READ = 1;
	static const uint32_t SIZE_REQUESTED = 2;
	static const uint32_t SIZE_READ = 3;
	static const uint32_t STABLE = 4;
*/
	typedef PCIRequest < ADDRESS_TYPE, MAX_DATA_SIZE > ReqType;
	typedef PCIResponse < MAX_DATA_SIZE > RspType;
	typedef Pointer < ReqType > PReqType;
	typedef Pointer < RspType > PRspType;
	typedef TlmMessage < ReqType, RspType > MsgType;
	typedef Pointer < MsgType > PMsgType;
	typedef TlmSendIf < ReqType, RspType > TlmSendIfType;
	
	class DeviceRequest {
	public:
		DeviceRequest(bool _used = false) : used(_used) {}
	
		Pointer<TlmMessage<InterruptRequest> > message;
		bool used;
	};
	
	sc_core::sc_event dispatch_event;
	sc_core::sc_event intr_dispatch_event;
	DeviceRequest intr_device_request;
	list<unisim::component::cxx::pci::ide::Event *> event_stack;
protected:
	// Pci device implementation
	unisim::component::cxx::pci::ide::PciDev<ADDRESS_TYPE> *pciDev;

	// debug stuff
	unisim::kernel::logger::Logger logger;
	bool verbose;
	Parameter<bool> param_verbose;
public:
	sc_core::sc_export<TlmSendIfType> input_port;
	sc_core::sc_port<TlmSendIfType> output_port;
	sc_core::sc_port<TlmSendIf<InterruptRequest> > irq_port;
	//ServiceExport<PCIInterface<ADDRESS_TYPE> > *exp;
	//ServiceImport<PCIInterface<ADDRESS_TYPE> > *import[NUM_TARGETS];

	SC_HAS_PROCESS(PCIDev);

	PCIDev(const sc_core::sc_module_name &name, Object *parent = 0);
	void SetDevice(unisim::component::cxx::pci::ide::PciDev<ADDRESS_TYPE> * dev);
	virtual ~PCIDev();
	virtual bool BeginSetup ();
	virtual bool Send (const PMsgType &message);
	virtual void Reset();
	bool dmaRead(ADDRESS_TYPE addr, int size, uint8_t *data);
	bool dmaWrite(ADDRESS_TYPE addr, int size, const uint8_t *data);
	void postInt(int irqId);
	void clearInt(int irqId);
	void notify();
	void schedule(unisim::component::cxx::pci::ide::Event *ev);
	void EventDispatch();
	void IntrDispatch();
};

} // end of namespace ide
} // end of namespace pci
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif
