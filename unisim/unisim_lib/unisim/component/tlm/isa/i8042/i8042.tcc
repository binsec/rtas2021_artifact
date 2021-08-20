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

#ifndef __UNISIM_COMPONENT_TLM_ISA_I8042_I8042_TCC__
#define __UNISIM_COMPONENT_TLM_ISA_I8042_I8042_TCC__

#include <cstring>

namespace unisim {
namespace component {
namespace tlm {
namespace isa {
namespace i8042 {
	
using unisim::component::cxx::isa::TransactionType;
using unisim::component::cxx::isa::ISASpace;
using unisim::component::cxx::isa::isa_address_t;
using unisim::component::cxx::isa::SP_IO;
using unisim::component::cxx::isa::SP_MEM;
using unisim::component::cxx::isa::TT_WRITE;
using unisim::component::cxx::isa::TT_READ;

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;


template <uint32_t MAX_DATA_SIZE>
I8042<MAX_DATA_SIZE>::I8042(const sc_core::sc_module_name& name, Object *parent) :
	Object(name, parent, "i8042 PS/2 keyboard/mouse controller"),
	sc_core::sc_module(name),
	unisim::component::cxx::isa::i8042::I8042(name, parent),
	isa_bus_cycle_time(),
	bus_cycle_time()
{
	kbd_irq_level = false;
	aux_irq_level = false;

	SC_HAS_PROCESS(I8042);
	
	bus_port(*this);
	
	SC_THREAD(KbdIrqMaster);
	SC_THREAD(AuxIrqMaster);

	SC_THREAD(CaptureKey);

	SC_THREAD(CaptureMouse);
}

template <uint32_t MAX_DATA_SIZE>
I8042<MAX_DATA_SIZE>::~I8042()
{
}

template <uint32_t MAX_DATA_SIZE>
bool I8042<MAX_DATA_SIZE>::EndSetup()
{
	if(!inherited::EndSetup()) return false;
	isa_bus_cycle_time = sc_core::sc_time(1.0 / (double) (*this)["isa-bus-frequency"], sc_core::SC_US);
	bus_cycle_time = sc_core::sc_time(1.0 / (double) (*this)["fsb-frequency"], sc_core::SC_US);
	kbd_irq_level = false;
	aux_irq_level = false;
	return true;
}

template <uint32_t MAX_DATA_SIZE>
bool I8042<MAX_DATA_SIZE>::Send(const Pointer<TlmMessage<ISAReq, ISARsp> > &message)
{
	const Pointer<ISAReq>& isa_req = message->GetRequest();
	isa_address_t isa_addr = isa_req->addr;
	ISASpace isa_space = isa_req->space;
	unsigned int isa_req_size = isa_req->size;
	TransactionType isa_transaction_type = isa_req->type;
	
	switch(isa_space)
	{
		case SP_IO:
			switch(isa_transaction_type)
			{
				case TT_READ:
				{
					Pointer<ISARsp> isa_rsp = new(isa_rsp) ISARsp();
				
					if(inherited::ReadIO(isa_addr, isa_rsp->read_data, isa_req_size))
					{
						message->rsp = isa_rsp;
						sc_core::sc_event *rsp_ev = message->GetResponseEvent();
						rsp_ev->notify(isa_bus_cycle_time);
					}
					else
					{
						// Respond anyway for now
						memset(isa_rsp->read_data, 0, isa_req_size);
						message->rsp = isa_rsp;
						sc_core::sc_event *rsp_ev = message->GetResponseEvent();
						rsp_ev->notify(isa_bus_cycle_time);
					}
				}
				break;
				case TT_WRITE:
					inherited::WriteIO(isa_addr, isa_req->write_data, isa_req_size);
					break;
			}
			break;
		case SP_MEM:
			break;
	}
		
	return true;
}

template <uint32_t MAX_DATA_SIZE>
void I8042<MAX_DATA_SIZE>::KbdIrqMaster()
{
	while(1)
	{
		wait(set_kbd_irq_ev);
		
		Pointer<TlmMessage<InterruptRequest> > message = new(message) TlmMessage<InterruptRequest>();
		Pointer<InterruptRequest> irq = new (irq) InterruptRequest();
		message->req = irq;
		
		irq->level = kbd_irq_level;
		irq->serial_id = 0;
		if(inherited::verbose)
		{
			inherited::logger << DebugInfo << "KBD Interrupt output goes " << (kbd_irq_level ? "high" : "low") << EndDebugInfo;
		}
		while(!kbd_irq_port->Send(message))
		{
			wait(bus_cycle_time);
		}
		wait(bus_cycle_time);
	}
}

template <uint32_t MAX_DATA_SIZE>
void I8042<MAX_DATA_SIZE>::AuxIrqMaster()
{
	while(1)
	{
		wait(set_aux_irq_ev);
		
		Pointer<TlmMessage<InterruptRequest> > message = new(message) TlmMessage<InterruptRequest>();
		Pointer<InterruptRequest> irq = new (irq) InterruptRequest();
		message->req = irq;
		
		irq->level = aux_irq_level;
		irq->serial_id = 0;
		if(inherited::verbose)
		{
			inherited::logger << DebugInfo << "AUX Interrupt output goes " << (aux_irq_level ? "high" : "low") << EndDebugInfo;
		}
		while(!aux_irq_port->Send(message))
		{
			wait(bus_cycle_time);
		}
		wait(bus_cycle_time);
	}
}

template <uint32_t MAX_DATA_SIZE>
void I8042<MAX_DATA_SIZE>::CaptureKey()
{
	while(1)
	{
		if(inherited::CaptureKey())
		{
			ev_repeat.cancel();
			ev_repeat.notify(sc_core::sc_time(inherited::typematic_delay / inherited::speed_boost, sc_core::SC_SEC));
		}
		wait(sc_core::sc_time(1.0 / (inherited::typematic_rate * inherited::speed_boost), sc_core::SC_SEC));
	}
}

template <uint32_t MAX_DATA_SIZE>
void I8042<MAX_DATA_SIZE>::CaptureMouse()
{
	while(1)
	{
		inherited::CaptureMouse();
		wait(sc_core::sc_time(1.0 / (inherited::aux_sample_rate * inherited::speed_boost), sc_core::SC_SEC));
	}
}

template <uint32_t MAX_DATA_SIZE>
void I8042<MAX_DATA_SIZE>::TriggerKbdInterrupt(bool level)
{
	if(kbd_irq_level != level)
	{
		wait(bus_cycle_time);
		kbd_irq_level = level;
		set_kbd_irq_ev.notify(sc_core::SC_ZERO_TIME);
		if(inherited::verbose)
		{
			inherited::logger << DebugInfo << "Trigger KBD interrupt level " << level << EndDebugInfo;
		}
	}
}

template <uint32_t MAX_DATA_SIZE>
void I8042<MAX_DATA_SIZE>::TriggerAuxInterrupt(bool level)
{
	if(aux_irq_level != level)
	{
		wait(bus_cycle_time);
		aux_irq_level = level;
		set_aux_irq_ev.notify(sc_core::SC_ZERO_TIME);
		if(inherited::verbose)
 		{
			inherited::logger << DebugInfo << "Trigger AUX interrupt level " << level << EndDebugInfo;
		}
	}
}

template <uint32_t MAX_DATA_SIZE>
void I8042<MAX_DATA_SIZE>::Reset()
{
	inherited::Reset();
}

template <uint32_t MAX_DATA_SIZE>
void I8042<MAX_DATA_SIZE>::Lock()
{
	mutex.lock();
}

template <uint32_t MAX_DATA_SIZE>
void I8042<MAX_DATA_SIZE>::Unlock()
{
	mutex.unlock();
}

} // end of namespace i8042
} // end of namespace isa
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif
