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
 
#ifndef __UNISIM_COMPONENT_TLM_BRIDGE_SNOOPINGFSBTOMEM_BRIDGE_HH__
#define __UNISIM_COMPONENT_TLM_BRIDGE_SNOOPINGFSBTOMEM_BRIDGE_HH__

#include <systemc>
#include "unisim/component/tlm/message/snooping_fsb.hh"
#include "unisim/component/tlm/message/memory.hh"
#include "unisim/kernel/tlm/tlm.hh"
#include "unisim/util/garbage_collector/garbage_collector.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/kernel/logger/logger.hh"
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm {
namespace bridge {
namespace snooping_fsb_to_mem {

using unisim::kernel::variable::Parameter;
using unisim::kernel::Object;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::kernel::tlm::TlmMessage;
using unisim::kernel::tlm::TlmSendIf;
using unisim::util::garbage_collector::Pointer;
using unisim::component::tlm::message::SnoopingFSBRequest;
using unisim::component::tlm::message::SnoopingFSBResponse;
using unisim::component::tlm::message::MemoryRequest;
using unisim::component::tlm::message::MemoryResponse;
using unisim::service::interfaces::Memory;

template <class CONFIG>
class Bridge :
	public sc_module,
	public TlmSendIf<SnoopingFSBRequest<typename CONFIG::fsb_address_t, CONFIG::FSB_BURST_SIZE>,
		SnoopingFSBResponse<CONFIG::FSB_BURST_SIZE> >,
	public Service<Memory<typename CONFIG::fsb_address_t> >,
	public Client<Memory<typename CONFIG::mem_address_t> > {
private:
	typedef typename CONFIG::fsb_address_t fsb_address_t;
	typedef typename CONFIG::mem_address_t mem_address_t;
	static const unsigned int FSB_BURST_SIZE = CONFIG::FSB_BURST_SIZE;
	static const unsigned int MEM_BURST_SIZE = CONFIG::MEM_BURST_SIZE;
	typedef SnoopingFSBRequest<fsb_address_t, FSB_BURST_SIZE> fsb_req_t;
	typedef SnoopingFSBResponse<FSB_BURST_SIZE> fsb_rsp_t;
	typedef Pointer<fsb_req_t> p_fsb_req_t;
	typedef Pointer<fsb_rsp_t> p_fsb_rsp_t;
	typedef TlmMessage<fsb_req_t, fsb_rsp_t> fsb_msg_t;
	typedef Pointer<fsb_msg_t> p_fsb_msg_t;
	typedef MemoryRequest<mem_address_t, MEM_BURST_SIZE> mem_req_t;
	typedef MemoryResponse<MEM_BURST_SIZE> mem_rsp_t;
	typedef Pointer<mem_req_t> p_mem_req_t;
	typedef Pointer<mem_rsp_t> p_mem_rsp_t;
	typedef TlmMessage<mem_req_t, mem_rsp_t> mem_msg_t;
	typedef Pointer<mem_msg_t> p_mem_msg_t;
	
	
public:
	SC_HAS_PROCESS(Bridge);
	
	sc_export<TlmSendIf<fsb_req_t, fsb_rsp_t> > slave_port;
	sc_port<TlmSendIf<mem_req_t, mem_rsp_t> > master_port;

	ServiceExport<Memory<fsb_address_t> > memory_export;
	ServiceImport<Memory<mem_address_t> > memory_import;

	Bridge(const sc_module_name& name, Object *parent = 0);
	virtual ~Bridge();
	
	virtual bool Setup();
	
	//virtual bool Send(const p_fsb_msg_t &fsb_msg);
	virtual bool Send(const Pointer<TlmMessage<SnoopingFSBRequest<typename CONFIG::fsb_address_t, Bridge<CONFIG>::FSB_BURST_SIZE>, SnoopingFSBResponse<Bridge<CONFIG>::FSB_BURST_SIZE> > >& fsb_msg);

	virtual void Reset();
	virtual bool ReadMemory(typename CONFIG::fsb_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(typename CONFIG::fsb_address_t addr, const void *buffer, uint32_t size);

protected:
	unisim::kernel::logger::Logger logger;
	bool verbose_all;
	Parameter<bool> param_verbose_all;
private:
	sc_fifo<p_fsb_msg_t> buffer_req;
	sc_time fsb_cycle_sctime;
	sc_time mem_cycle_sctime;
	
	uint64_t fsb_cycle_time;
	Parameter<uint64_t> param_fsb_cycle_time;
	uint64_t mem_cycle_time;
	Parameter<uint64_t> param_mem_cycle_time;
	
	sc_event dispatch_mem_ev;
	void DispatchMemory();
};

} // end of namespace snooping_fsb_to_mem
} // end of namespace bridge
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM_BRIDGE_SNOOPINGFSBTOMEM_BRIDGE_HH__
