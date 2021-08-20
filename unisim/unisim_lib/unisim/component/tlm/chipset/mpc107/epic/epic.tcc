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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM_CHIPSET_MPC107_EPIC_EPIC_TCC__
#define __UNISIM_COMPONENT_TLM_CHIPSET_MPC107_EPIC_EPIC_TCC__

#include <sstream>

namespace unisim {
namespace component {
namespace tlm {
namespace chipset {
namespace mpc107 {
namespace epic {
	
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

using std::stringstream;

#define LOCATION "In file " << __FILE__ << ", function " << __FUNCTION__ << ", line #" << __LINE__

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
EPIC(const sc_core::sc_module_name &name, Object *parent) :
	Object(name, parent, "MPC107 integrated Embedded Programmable Interrupt Controller (EPIC)"),
	sc_core::sc_module(name),
	unisim::component::cxx::chipset::mpc107::epic::EPIC<PHYSICAL_ADDR, DEBUG>("epic_impl", parent),
	slave_port("slave_port"),
	irq_master_port("irq_master_port"),
	soft_reset_master_port("soft_reset_master_port"),
	sdram_slave_port("sdram_slave_port"),
	sdram_clock_activated_mutex("sdram_clock_activated_mutex"),
	sdram_clock_activated(false) {
	slave_port(*this);
	/* create irq ports and the objects to handle them */
	for(unsigned int i = 0; i < inherited::NUM_IRQS; i++) {
		stringstream irq_slave_port_name;
		stringstream irq_slave_port_handler_name;
		
		irq_slave_port_name << "irq_slave_port[" << i << "]";
		irq_slave_port[i] = new sc_core::sc_export<TlmSendIf<IntType> >(irq_slave_port_name.str().c_str());
		irq_slave_port_handler_name << "irq_slave_port_handler[" << i << "]";
		irq_handler[i] = new RequestPortIdentifier<InterruptRequest>(irq_slave_port_handler_name.str().c_str(), i, *this);
		(*irq_slave_port[i])(*irq_handler[i]);  
	}
	
	SC_THREAD(SDRAMClock);
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
~EPIC() {
	for(unsigned int i = 0; i < inherited::NUM_IRQS; i++) {
		delete irq_slave_port[i];
		delete irq_handler[i];
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
Send(const PMsgType &message) {
	const PReqType& req = message->GetRequest();
	uint32_t data = 0;

	switch(req->type) {
	case ReqType::READ: 
		{
			if(inherited::verbose)
				inherited::logger << DebugInfo
					<< LOCATION
					<< "Read received at address 0x" << std::hex << req->addr << std::dec
					<< std::endl << EndDebugInfo;
			data = inherited::ReadRegister(req->addr, req->size);
			PRspType rsp = new(rsp) RspType();
			for(unsigned int i = 0; i < req->size; i ++) {
				rsp->read_data[i] = (uint8_t)((data >> (i * 8)) & (uint32_t)0x0ff);
			}
			message->rsp = rsp;
			message->GetResponseEvent()->notify();
			return true;
		}
		break;
	case ReqType::WRITE:
		if(inherited::verbose)
			inherited::logger << DebugInfo
				<< LOCATION
				<< "Write received at address 0x" << std::hex << req->addr << std::dec
				<< std::endl << EndDebugInfo;
		for(unsigned int i = 0; i < req->size; i++) {
			data = data + (((uint32_t)(req->write_data[i])) << (i * 8));
		}
		inherited::WriteRegister(req->addr, data, req->size);
		return true;
		break;
	}
	return false;
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG>
bool 
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
Send(const PIntMsgType &message, unsigned int id) {
	if(message->req->level)
		inherited::SetIRQ(id, message->req->serial_id);
	else
		inherited::UnsetIRQ(id, message->req->serial_id);
	return true;
}

template <class PHYSICAL_ADDR, 
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
SetINT() {
	PIntMsgType message = new(message) IntMsgType();
	PIntType irq = new(irq) IntType();
	
	irq->level = true;
	irq->serial_id = 0;
	
	message->req = irq;
	
	if(!irq_master_port->Send(message)) {
		inherited::logger << DebugError
			<< LOCATION
			<< "TODO: handle interrupt requests that could not be handled"
			<< std::endl << EndDebugError; 
		
		Object::Stop(-1);
	}
}

template <class PHYSICAL_ADDR, 
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
UnsetINT() {
	PIntMsgType message = new(message) IntMsgType();
	PIntType irq = new(irq) IntType();
	
	irq->level = false;
	irq->serial_id = 0;
	
	message->req = irq;
	
	if(!irq_master_port->Send(message)) {
		inherited::logger << DebugError
			<< LOCATION
			<< "TODO: handle interrupt requests that could not be handled"
			<< std::endl << EndDebugError; 
		
		Object::Stop(-1);
	}
}

template <class PHYSICAL_ADDR, 
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
SetSoftReset() {
	PIntMsgType message = new(message) IntMsgType();
	PIntType irq = new(irq) IntType();
	
	irq->level = true;
	irq->serial_id = 0;
	
	message->req = irq;
	
	if(!soft_reset_master_port->Send(message)) {
		inherited::logger << DebugError
			<< LOCATION
			<< "TODO: handle soft reset interrupt requests that could not be handled"
			<< std::endl << EndDebugError; 
		
		Object::Stop(-1);
	}
}

template <class PHYSICAL_ADDR, 
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void 
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
UnsetSoftReset() {
	PIntMsgType message = new(message) IntMsgType();
	PIntType irq = new(irq) IntType();
	
	irq->level = false;
	irq->serial_id = 0;
	
	message->req = irq;
	
	if(!soft_reset_master_port->Send(message)) {
		inherited::logger << DebugError
			<< LOCATION
			<< "TODO: handle soft reset interrupt requests that could not be handled"
			<< std::endl << EndDebugError; 
		
		Object::Stop(-1);
	}
}

template <class PHYSICAL_ADDR,
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
ActivateSDRAMClock() {
	sdram_clock_activated_mutex.lock();
	if(!sdram_clock_activated) {
		/* TODO: fix this function to set the initial delay time to the difference
		 *   between current time and the sdram(multiplied by eight) time */
		sc_core::sc_time sdram_time = sc_core::sc_time((double)(sdram_slave_port * 8), sc_core::SC_FS);
		sdram_clock_event.notify(sdram_time);
		sdram_clock_activated = true;
	}
	sdram_clock_activated_mutex.unlock();
}

template <class PHYSICAL_ADDR,
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
DeactivateSDRAMClock() {
	sdram_clock_activated_mutex.lock();
	sdram_clock_activated = false;
	sdram_clock_activated_mutex.unlock();
}

template <class PHYSICAL_ADDR,
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
SDRAMClock() {
	bool cont = false;
	
	while(1) {
		wait(sdram_clock_event);
		cont = true;
		while(cont) {
			bool activated = false;
			sdram_clock_activated_mutex.lock();
			activated = sdram_clock_activated;
			sdram_clock_activated_mutex.unlock();
			if(activated) {
				sc_core::sc_time time = sc_core::sc_time((double)(sdram_slave_port * 8), sc_core::SC_FS);
				uint64_t sdram_clock_cycles = 
					(uint64_t)(sc_core::sc_time_stamp() / time);
				inherited::TimeSignal(sdram_clock_cycles * 8);
				wait(time);
			} else 
				cont = false;
		}
	}
}

template <class PHYSICAL_ADDR,
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
StopSimulation() {
	Object::Stop(-1);
}

template <class PHYSICAL_ADDR,
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	bool DEBUG>
void
EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> ::
TimeEvent() {
}

#undef LOCATION

} // end of epic namespace
} // end of mpc107 namespace	
} // end of chipset namespace
} // end of tlm namespace
} // end of component namespace
} // end of full_system namespace

#endif // __UNISIM_COMPONENT_TLM_CHIPSET_MPC107_EPIC_EPIC_TCC__
