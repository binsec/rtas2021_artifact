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

#ifndef __UNISIM_COMPONENT_TLM_ISA_I8042_I8042_HH__
#define __UNISIM_COMPONENT_TLM_ISA_I8042_I8042_HH__

#include <systemc>
#include "unisim/kernel/tlm/tlm.hh"
#include "unisim/kernel/kernel.hh"
#include "unisim/util/garbage_collector/garbage_collector.hh"
#include <unisim/component/tlm/message/interrupt.hh>
#include <unisim/component/cxx/isa/i8042/i8042.hh>
#include <unisim/component/tlm/message/isa.hh>
#include <string>

namespace unisim {
namespace component {
namespace tlm {
namespace isa {
namespace i8042 {

using unisim::kernel::tlm::TlmSendIf;
using unisim::kernel::tlm::TlmMessage;
using unisim::component::tlm::message::ISARequest;
using unisim::component::tlm::message::ISAResponse;


using unisim::kernel::Object;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::variable::Parameter;
using unisim::util::garbage_collector::Pointer;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::component::tlm::message::InterruptRequest;

template <uint32_t MAX_DATA_SIZE>
class I8042 :
	public sc_core::sc_module,
	public TlmSendIf<ISARequest<MAX_DATA_SIZE>, ISAResponse<MAX_DATA_SIZE> >,
	public unisim::component::cxx::isa::i8042::I8042
{
public:

	typedef unisim::component::cxx::isa::i8042::I8042 inherited;
	typedef ISARequest<MAX_DATA_SIZE> ISAReq;
	typedef ISAResponse<MAX_DATA_SIZE> ISARsp;
	
	// from ISA bus
	sc_core::sc_export<TlmSendIf<ISAReq, ISARsp> > bus_port;
	
	// to CPU
	sc_core::sc_port<TlmSendIf<InterruptRequest> > kbd_irq_port;
	sc_core::sc_port<TlmSendIf<InterruptRequest> > aux_irq_port;

	I8042(const sc_core::sc_module_name& name, Object *parent = 0);
	virtual ~I8042();
	void KbdIrqMaster();
	void AuxIrqMaster();
	void CaptureKey();
	void CaptureMouse();
	virtual bool EndSetup();
	virtual bool Send(const Pointer<TlmMessage<ISAReq, ISARsp> > &message);
	virtual void TriggerKbdInterrupt(bool level);
	virtual void TriggerAuxInterrupt(bool level);
	virtual void Reset();
	virtual void Lock();
	virtual void Unlock();
private:
	
	bool kbd_irq_level;
	bool aux_irq_level;
	
	sc_core::sc_time isa_bus_cycle_time;
	sc_core::sc_time bus_cycle_time;
	sc_core::sc_event set_kbd_irq_ev;
	sc_core::sc_event set_aux_irq_ev;
	sc_core::sc_mutex mutex;
	sc_core::sc_event ev_repeat;
};

} // end of namespace i8042
} // end of namespace isa
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_TLM_ISA_I8042_I8042_HH__ */
