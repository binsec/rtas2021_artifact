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

#ifndef __UNISIM_COMPONENT_TLM_PCI_IDE_PCI_DEV_TCC__
#define __UNISIM_COMPONENT_TLM_PCI_IDE_PCI_DEV_TCC__

namespace unisim {
namespace component {
namespace tlm {
namespace pci {
namespace ide {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE>::PCIDev(const sc_core::sc_module_name &name, Object *parent):
	Object(name, parent, "PCI device"),
	sc_core::sc_module(name),
	intr_device_request(),
	pciDev(0),
	logger(*this),
	verbose(false),
	param_verbose("verbose", this, verbose),
	input_port("PCI_input_port"),
	output_port("PCI_output_port"),
	irq_port("irq_port")
	//,
	//Service<PCIInterface<ADDRESS_TYPE> >(name, parent),
	//Client<PCIInterface<ADDRESS_TYPE> >(name, parent),
	/*param_base_address("base-address", this, base_address),
	param_size("size", this, size),
	param_device_number("device-number", this, device_number),
	param_register_number("register-number", this, register_number) */{

	//Initialize system c input port
	input_port (*this);	
	SC_THREAD(EventDispatch);
	//SC_THREAD(IntrDispatch);

}

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE>::SetDevice(unisim::component::cxx::pci::ide::PciDev<ADDRESS_TYPE> * dev) {
		pciDev = dev;
}

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE>::~PCIDev() {
	if(pciDev) delete pciDev;
}

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
bool PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE>::BeginSetup () {
	pciDev->SetPCIMaster(this);
	pciDev->SetEventManager(this);
	Reset ();
	return true;
}

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
bool PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE>::Send (const PMsgType &message) {
	bool ret = true;
	PReqType req = message->req;
	
	switch (req->type) {
	case unisim::component::cxx::pci::TT_READ: {
		
		switch (req->space) {
			case unisim::component::cxx::pci::SP_CONFIG:
				{		
				PRspType res = new(res) RspType();
				ret = pciDev->readConfig(req->addr & 0xFF, req->size, res->read_data);
				message->SetResponse(res);
		
				if(unlikely(verbose))
				{
					logger << DebugInfo
					<< "(" << __FUNCTION__
					<< ":" << __FILE__
					<< ":" << __LINE__ << "): "
					<< "read access to configuration register 0x"
					<< std::hex << req->addr << std::dec 
					<< " size = " << req->size
					<< " data = 0x";
					for(unsigned int i = 0; i < req->size - 1; i++)
						logger << std::hex << (unsigned int)res->read_data[i] << std::dec << " ";
					logger << std::hex << (unsigned int)res->read_data[req->size - 1] << std::dec << std::endl;
					logger << EndDebugInfo;
				}
		
				}
				break;
			case unisim::component::cxx::pci::SP_MEM: 
				{
				PRspType res = new(res) RspType();
				ret = pciDev->readMem(req->addr, req->size, res->read_data);
				message->SetResponse(res);

				if(unlikely(verbose))
				{
					logger << DebugInfo
					<< "(" << __FUNCTION__
					<< ":" << __FILE__
					<< ":" << __LINE__ << "): "
					<< "read access to MEM 0x"
					<< std::hex << req->addr << std::dec 
					<< " size = " << req->size
					<< " data = 0x";
					for(unsigned int i = 0; i < req->size - 1; i++)
						logger << std::hex << (unsigned int)res->read_data[i] << std::dec << " ";
					logger << std::hex << (unsigned int)res->read_data[req->size - 1] << std::dec << std::endl;
					logger << EndDebugInfo;
				}

				}
				break;
			case unisim::component::cxx::pci::SP_IO:
				{
					PRspType res = new(res) RspType();
					ret = pciDev->readIO(req->addr, req->size, res->read_data);
					message->SetResponse(res);
				}
				break;
			
		}
		
		
		if(message->HasResponseEvent())
				message->GetResponseEvent()->notify(sc_core::sc_time(1.0 / (double) 33, sc_core::SC_US));
		}
		break;
	case unisim::component::cxx::pci::TT_WRITE: {
		switch (req->space) {
			case unisim::component::cxx::pci::SP_CONFIG: {
				ret = pciDev->writeConfig(req->addr & 0xFF, req->size, req->write_data);
				
				if(unlikely(verbose))
				{
					logger << DebugInfo << "(" << __FUNCTION__
					<< ":" << __FILE__
					<< ":" << __LINE__ << "): "
					<< "write access to configuration register 0x"
					<< std::hex << req->addr << std::dec 
					<< " size = " << req->size
					<< " data = 0x";
					for(unsigned int i = 0; i < req->size - 1; i++)
						logger << std::hex << (unsigned int)req->write_data[i] << std::dec << " ";
					logger << std::hex << (unsigned int)req->write_data[req->size - 1] << std::dec << std::endl;
					logger << EndDebugInfo;
				}
				
			}
				break;
			case unisim::component::cxx::pci::SP_MEM: { 
				ret = pciDev->writeMem(req->addr, req->size, req->write_data);
				break;
			}
			case unisim::component::cxx::pci::SP_IO: { 
				ret = pciDev->writeIO(req->addr, req->size, req->write_data);
				break;
			}
		}
		break;
		} 
	default:
		{
			if(unlikely(verbose))
			{
				logger << DebugError << "No esta definido el codigo!" << std::endl;
				logger << "  (" << __FUNCTION__ << ":"
					<< __FILE__ << ":"
					<< __LINE__ << ")" << std::endl;
				logger << EndDebugError;
			}
		ret = false;
		}
	}
	dispatch_event.notify(sc_core::sc_time(1.0 / (double) 33, sc_core::SC_US));
	intr_dispatch_event.notify(sc_core::sc_time(1.0 / (double) 33, sc_core::SC_US));
	return ret;
}

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE>::Reset() {
}

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
bool PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE>::dmaRead(ADDRESS_TYPE addr, int size, uint8_t *data) {
	
	int size_done = 0;
	
	while (size_done < size) {
	
		PReqType req = new(req) ReqType();
		PMsgType message = new(message) MsgType(req);
		sc_core::sc_event pci_event;
		message->PushResponseEvent(pci_event);
		req->addr = addr + size_done;
		req->type = unisim::component::cxx::pci::TT_READ;
		req->space = unisim::component::cxx::pci::SP_MEM;
		req->size = (size - size_done) < (int) MAX_DATA_SIZE?(size - size_done):MAX_DATA_SIZE;
		//todo: implement resend?
		output_port->Send(message);
		wait(pci_event);
	
		PRspType rsp = message->rsp;
		memcpy(data + size_done, rsp->read_data, req->size);
		size_done += req->size;
	}
	return true;		
}

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
bool PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE>::dmaWrite(ADDRESS_TYPE addr, int size, const uint8_t *data) {
	if(unlikely(verbose))
		logger << DebugInfo << "Doing dma write" << std::endl << EndDebugInfo;
		
	int size_done = 0;
	
	while (size_done < size) {
	
		PReqType req = new(req) ReqType();
		PMsgType pci_msg = new(pci_msg) MsgType(req);
		req->addr = addr + size_done;
		req->type = unisim::component::cxx::pci::TT_WRITE;
		req->space = unisim::component::cxx::pci::SP_MEM;
		req->size = (size - size_done) <= (int) MAX_DATA_SIZE?(size - size_done):MAX_DATA_SIZE;
		memcpy(req->write_data, data + size_done, req->size);
		//todo: implement resend?		
		output_port->Send(pci_msg);
		
		/* fprintf(stderr, "Writing: ");
		for (int i=0; i < (req->size/8); i++) {
			fprintf(stderr, " %x ", req->write_data[i]);
		}*/		
		size_done += req->size;
	}
	return true;	
}

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE>::postInt(int irqId){
	Pointer<TlmMessage<InterruptRequest> > message = new(message) TlmMessage<InterruptRequest>();
	Pointer<InterruptRequest> irq = new (irq) InterruptRequest();
	irq->level = true;
	irq->serial_id = 0;
//	sc_core::sc_event ack_ev;
	message->req = irq;
//	message->PushResponseEvent(ack_ev);
	
	irq_port->Send(message);
//	intr_device_request.message = message;
//	intr_device_request.used = true;
/*	while(!irq_port->Send(message))
	{
		wait(bus_cycle_time);
	}*/
	
//	wait(ack_ev);
}

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE>::clearInt(int irqId){
	Pointer<TlmMessage<InterruptRequest> > message = new(message) TlmMessage<InterruptRequest>();
	Pointer<InterruptRequest> irq = new (irq) InterruptRequest();
	irq->level = false;
	irq->serial_id = 0;
//	sc_core::sc_event ack_ev;
	message->req = irq;
//	message->PushResponseEvent(ack_ev);
	
	irq_port->Send(message);
}

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE>::notify() {
	dispatch_event.notify(sc_core::SC_ZERO_TIME);
}

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE>::schedule(unisim::component::cxx::pci::ide::Event *ev) {
		event_stack.push_back(ev);
		dispatch_event.notify(sc_core::SC_ZERO_TIME);
}

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE>::EventDispatch() {
	while(1) {
		if (event_stack.empty()) {
			wait(dispatch_event);
				//wait(1.0, sc_core::SC_FS);
		} else {
			//TODO PC: Event and PCIDev should be one level up in the hierarchy
			unisim::component::cxx::pci::ide::Event * ev = event_stack.front();
			event_stack.pop_front();
			ev->process();
			
		}
	}
}

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIDev<ADDRESS_TYPE, MAX_DATA_SIZE>::IntrDispatch() {
	while(1) {
		wait(intr_dispatch_event);
		if (intr_device_request.used) {
			Pointer<TlmMessage<InterruptRequest> > message = intr_device_request.message;
			while(!(irq_port->Send(message))) {
				wait(1.0, sc_core::SC_FS);
			}
/* 	  		if(message->HasResponseEvent()) {
				wait(message->GetResponseEvent());
				cerr << "received response " << endl;
				//TODO: Pau I THINK WE SHOULd be adding another event to the event stack and wait on that event!
				if(!message->rsp) {
					cerr << "ERROR("
						<< __FUNCTION__
						<< ":" << __FILE__
						<< ":" << __LINE__
						<< "): received a response for a pci message without response field"
						<< endl;
					Object::Stop(-1);
					wait();
				}			 	
			} */
			intr_device_request.used = false;
		}
	}
}

} // end of namespace ide
} // end of namespace pci
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif
