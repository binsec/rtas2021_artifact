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
 
#ifndef __UNISIM_COMPONENT_TLM_CHIPSET_MPC107_EPIC_EPIC_HH__
#define __UNISIM_COMPONENT_TLM_CHIPSET_MPC107_EPIC_EPIC_HH__

#include <inttypes.h>
#include <systemc>
#include "unisim/kernel/tlm/tlm.hh"
#include "unisim/kernel/kernel.hh"
#include "unisim/util/garbage_collector/garbage_collector.hh"
#include "unisim/component/tlm/message/interrupt.hh"
#include "unisim/component/tlm/message/memory.hh"
#include "unisim/component/cxx/chipset/mpc107/epic/epic.hh"
#include "unisim/component/tlm/chipset/mpc107/epic/timer.hh"

namespace unisim {
namespace component {
namespace tlm {
namespace chipset {
namespace mpc107 {
namespace epic {

using unisim::kernel::tlm::TlmSendIf;
using unisim::kernel::tlm::TlmMessage;
using unisim::kernel::tlm::RequestPortIdentifier;
using unisim::kernel::tlm::RequestPortIdentifierInterface;
using unisim::kernel::Object;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::util::garbage_collector::Pointer;
using unisim::component::tlm::message::InterruptRequest;
using unisim::component::tlm::message::MemoryRequest;
using unisim::component::tlm::message::MemoryResponse;

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		bool DEBUG = false>
class EPIC :
	public sc_core::sc_module,
	public TlmSendIf<MemoryRequest<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE>, 
					 MemoryResponse<MAX_TRANSACTION_DATA_SIZE> >,
	public RequestPortIdentifierInterface<InterruptRequest>,
	public unisim::component::cxx::chipset::mpc107::epic::EPIC<PHYSICAL_ADDR, DEBUG>,
	public Timer {
private:
	typedef MemoryRequest<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE> ReqType;
	typedef Pointer<ReqType> PReqType;
	typedef MemoryResponse<MAX_TRANSACTION_DATA_SIZE> RspType;
	typedef Pointer<RspType> PRspType;
	typedef TlmMessage<ReqType, RspType> MsgType;
	typedef Pointer<MsgType> PMsgType;
	
	typedef InterruptRequest IntType;
	typedef Pointer<InterruptRequest> PIntType;
	typedef TlmMessage<IntType> IntMsgType;
	typedef Pointer<IntMsgType> PIntMsgType; 

	typedef unisim::component::cxx::chipset::mpc107::epic::EPIC<PHYSICAL_ADDR, DEBUG> inherited;
public:
	SC_HAS_PROCESS(EPIC);
	
	/* Module ports declaration */
	/** Input port for incomming requests from the mpc107 bus */
	sc_core::sc_export<TlmSendIf<ReqType, RspType> > slave_port;
	/** Input port for the incomming interruptions */
	sc_core::sc_export<TlmSendIf<IntType> > *irq_slave_port[inherited::NUM_IRQS];
	/* output port for the outgoing interruptions */
	sc_core::sc_port<TlmSendIf<IntType> > irq_master_port;
	/* output port for the soft reset interruption */
	sc_core::sc_port<TlmSendIf<IntType> > soft_reset_master_port;
	/* input port for the sdram time signal */
	sc_core::sc_in<uint64_t> sdram_slave_port;

	EPIC(const sc_core::sc_module_name &name, Object *parent = 0);
	virtual ~EPIC();

	/* Interface of the EPIC to the MPC107 */
	virtual bool Send(const PMsgType &message);
	/* Interface of the interrupts port */
	virtual bool Send(const PIntMsgType &message, unsigned int id);
	
	/* Methods required by chipsets::mpc107::epic::EPIC */
	virtual void SetINT();
	virtual void UnsetINT();
	virtual void SetSoftReset();
	virtual void UnsetSoftReset();
	// virtual void Notify(uint64_t sdram_cycles);
	virtual void StopSimulation();
	virtual void ActivateSDRAMClock();
	virtual void DeactivateSDRAMClock();
	
	/* Method required by tlm::chipsets::mpc107::epic::Timer */
	virtual void TimeEvent();
	
private:
	RequestPortIdentifier<IntType> *irq_handler[inherited::NUM_IRQS];
	
	sc_core::sc_mutex sdram_clock_activated_mutex;
	bool sdram_clock_activated;
	
	void SDRAMClock();
	sc_core::sc_event sdram_clock_event;
};
	
} // end of epic namespace
} // end of mpc107 namespace	
} // end of chipset namespace
} // end of tlm namespace
} // end of component namespace
} // end of full_system namespace

#endif // __UNISIM_COMPONENT_TLM_CHIPSET_MPC107_EPIC_EPIC_HH__
