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
 
#ifndef __UNISIM_COMPONENT_TLM_BRIDGE_SNOOPINGFSBTOMEM_BRIDGE_TCC__
#define __UNISIM_COMPONENT_TLM_BRIDGE_SNOOPINGFSBTOMEM_BRIDGE_TCC__

#include <cmath>

namespace unisim {
namespace component {
namespace tlm {
namespace bridge {
namespace snooping_fsb_to_mem {

using unisim::kernel::Object;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

#define LOCATION "In function " << __FUNCTION__ << ", file " << __FILE__ << " line #" << __LINE__

template <class CONFIG>
Bridge<CONFIG> ::
Bridge(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	Service<Memory<fsb_address_t> >(name, parent),
	Client<Memory<mem_address_t> >(name, parent),
	slave_port("slave-port"),
	master_port("master-port"),
	memory_export("memory-export", this),
	memory_import("memory-import", this),
	logger(*this),
	verbose_all(false),
	param_verbose_all("verbose_all", this, verbose_all),
	buffer_req("buffer_req", CONFIG::BUFFER_SIZE),
	fsb_cycle_sctime(),
	mem_cycle_sctime(),
	fsb_cycle_time(0),
	param_fsb_cycle_time("fsb-cycle-time", this, fsb_cycle_time),
	mem_cycle_time(0),
	param_mem_cycle_time("mem-cycle-time", this, mem_cycle_time)
{
	slave_port(*this);
	
	SC_THREAD(DispatchMemory);
	
	Object::SetupDependsOn(memory_import);
}

template<class CONFIG>
Bridge<CONFIG> ::
~Bridge() {
	
}

template<class CONFIG>
bool
Bridge<CONFIG> ::
Setup() {
	if(fsb_cycle_time == 0) {
		logger << DebugError << LOCATION
			<< "fsb_cycle_time parameter should be set to a value bigger than 0"
			<< std::endl
			<< EndDebugError;
		return false;
	}
	
	if(mem_cycle_time == 0) {
		logger << DebugError << LOCATION
			<< "mem_cycle_time paramater should be set to a value bigger than 0"
			<< std::endl
			<< EndDebugError;
		return false;
	}
	
	if(CONFIG::FSB_BURST_SIZE != CONFIG::MEM_BURST_SIZE) {
		logger << DebugError << LOCATION
			<< "the size of the two busses differ, they should be the same"
			<< std::endl
			<< EndDebugError;
		return false;
	}
	
	fsb_cycle_sctime = sc_time((double)fsb_cycle_time, SC_PS);
	mem_cycle_sctime = sc_time((double)mem_cycle_time, SC_PS);
	
	if(CONFIG::DEBUG && verbose_all) {
		logger << DebugInfo << LOCATION
			<< "fsb_cycle_time = " << fsb_cycle_sctime.to_string()
			<< std::endl << EndDebugInfo;
		logger << DebugInfo << LOCATION
			<< "mem_cycle_time = " << mem_cycle_sctime.to_string()
			<< std::endl << EndDebugInfo;
	}
	
	return true;
}

template<class CONFIG>
bool
Bridge<CONFIG> ::
//Send(const p_fsb_msg_t &fsb_msg) {
Send(const Pointer<TlmMessage<SnoopingFSBRequest<typename CONFIG::fsb_address_t, Bridge<CONFIG>::FSB_BURST_SIZE>, SnoopingFSBResponse<Bridge<CONFIG>::FSB_BURST_SIZE> > >& fsb_msg) {
	bool dispatch_needed = false;
	
	/* check if there is room for the request in the queue,
	 *   if not report that the request can not be accepted (return false) */
	if(buffer_req.num_available() == 0)
		dispatch_needed = true;
	if(!buffer_req.nb_write(fsb_msg))
		return false;
		
	if(dispatch_needed) {
		dispatch_mem_ev.notify((ceil(sc_time_stamp() / mem_cycle_sctime) * mem_cycle_sctime) - sc_time_stamp());
	}
	
	return true;
}

template<class CONFIG>
void 
Bridge<CONFIG> ::
DispatchMemory() {
	
	while(1) {
		p_fsb_msg_t fsb_msg;
		sc_event read_ev;

		wait(dispatch_mem_ev);
		if(!buffer_req.nb_read(fsb_msg)) {
			logger << DebugError << LOCATION
				<< "trying to dispatch a message to the memory bus when none is available" << std::endl
				<< "Stopping simulation"
				<< std::endl << EndDebugError;
			Object::Stop(-1);
		}
		
		const p_fsb_req_t &fsb_req = fsb_msg->req;
		p_mem_req_t mem_req = new(mem_req) mem_req_t();
		p_mem_msg_t mem_msg = new(mem_msg) mem_msg_t(mem_req);
		mem_req->addr = fsb_req->addr;
		mem_req->size = fsb_req->size;
		sc_event *rsp_ev;
		p_mem_rsp_t mem_rsp;
		switch(fsb_req->type) {
		case fsb_req_t::READ:
		case fsb_req_t::READX:
			mem_req->type = mem_req_t::READ;
			mem_msg->PushResponseEvent(read_ev);
			while(!master_port->Send(mem_msg))
				wait(mem_cycle_sctime);
			wait(read_ev);
			mem_rsp = mem_msg->rsp;
			fsb_msg->rsp = new(fsb_msg->rsp) fsb_rsp_t();
			memcpy(fsb_msg->rsp->read_data, mem_rsp->read_data, fsb_req->size);
			fsb_msg->rsp->read_status = fsb_rsp_t::RS_SHARED;
			rsp_ev = fsb_msg->GetResponseEvent();
			rsp_ev->notify((ceil(sc_time_stamp() / fsb_cycle_sctime)* fsb_cycle_sctime) - sc_time_stamp());
			break;
		case fsb_req_t::WRITE:
			mem_req->type = mem_req_t::WRITE;
			memcpy(mem_req->write_data, fsb_req->write_data, fsb_req->size);
			while(!master_port->Send(mem_msg))
				wait(mem_cycle_sctime);
			break;
		default:
			logger << DebugWarning << LOCATION
				<< "message ";
			switch(fsb_req->type) {
			case fsb_req_t::INV_BLOCK: logger << "INV_BLOCK"; break;
			case fsb_req_t::FLUSH_BLOCK: logger << "FLUSH_BLOCK"; break;
			case fsb_req_t::ZERO_BLOCK: logger << "ZERO_BLOCK"; break;
			case fsb_req_t::INV_TLB: logger << "INV_TLB"; break;
			default: break;
			}
			logger << " will be ignored (can not be handled by this module)"
				<< std::endl << EndDebugWarning;
		}
		
		if(buffer_req.num_available() != 0) {
			dispatch_mem_ev.notify((ceil(sc_time_stamp() / mem_cycle_sctime) * mem_cycle_sctime) - sc_time_stamp());
		}
	}
}

template<class CONFIG>
void
Bridge<CONFIG> ::
Reset() {
	p_fsb_msg_t fsb_msg;
	while(buffer_req.nb_read(fsb_msg));
	return;
}

template<class CONFIG>
bool
Bridge<CONFIG> ::
ReadMemory(typename CONFIG::fsb_address_t addr, void *buffer, uint32_t size) {
	if(memory_import)
		return memory_import->ReadMemory((typename CONFIG::mem_address_t)addr, buffer, size);
	return false;
}

template<class CONFIG>
bool
Bridge<CONFIG> ::
WriteMemory(typename CONFIG::fsb_address_t addr, const void *buffer, uint32_t size) {
	if(memory_import)
		return memory_import->WriteMemory((typename CONFIG::mem_address_t)addr, buffer, size);
	return false;
}

#undef LOCATION

} // end of namespace snooping_fsb_to_mem
} // end of namespace bridge
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM_BRIDGE_SNOOPINGFSBTOMEM_BRIDGE_TCC__
