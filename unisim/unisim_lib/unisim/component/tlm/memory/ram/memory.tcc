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
 
#ifndef __UNISIM_COMPONENT_TLM_MEMORY_RAM_MEMORY_TCC__
#define __UNISIM_COMPONENT_TLM_MEMORY_RAM_MEMORY_TCC__

#define LOCATION "in function " << __FUNCTION__ << ", file " << __FILE__ << ", line #" << __LINE__

namespace unisim {
namespace component {
namespace tlm {
namespace memory {
namespace ram {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

/* Constructor */
template <class PHYSICAL_ADDR, uint32_t DATA_SIZE, uint32_t PAGE_SIZE, bool DEBUG>
Memory<PHYSICAL_ADDR, DATA_SIZE, PAGE_SIZE, DEBUG>::
Memory(const sc_core::sc_module_name& name, Object *parent)
	: Object(name, parent, "Memory")
	, sc_core::sc_module(name)
	, unisim::component::cxx::memory::ram::Memory<PHYSICAL_ADDR, PAGE_SIZE>(name, parent)
	, slave_port("slave-port")
	, logger(*this)
	, cycle_sctime()
	, param_cycle_time("cycle-time", this, cycle_sctime, "RAM memory cycle time")
{
	param_cycle_time.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	slave_port(*this);
}

/* Destructor */
template <class PHYSICAL_ADDR, uint32_t DATA_SIZE, uint32_t PAGE_SIZE, bool DEBUG>
Memory<PHYSICAL_ADDR, DATA_SIZE, PAGE_SIZE, DEBUG>::
~Memory() {
}

/* ClientIndependentSetup */
template <class PHYSICAL_ADDR, uint32_t DATA_SIZE, uint32_t PAGE_SIZE, bool DEBUG>
bool Memory<PHYSICAL_ADDR, DATA_SIZE, PAGE_SIZE, DEBUG>::
EndSetup() {
	if(unlikely(DEBUG && this->verbose)) 
		logger << DebugInfo << LOCATION
			<< " cycle time of " << cycle_sctime 
			<< std::endl << EndDebugInfo;
	if(cycle_sctime == sc_core::SC_ZERO_TIME) {
		logger << DebugError << LOCATION
			<< "cycle time must be different than 0" << std::endl
			<< EndDebugError;
		return false;
	}
	return unisim::component::cxx::memory::ram::Memory<PHYSICAL_ADDR, PAGE_SIZE>::EndSetup();
}

/* Tlm Send method to handle incomming memory requests */
template <class PHYSICAL_ADDR, uint32_t DATA_SIZE, uint32_t PAGE_SIZE, bool DEBUG>
bool 
Memory<PHYSICAL_ADDR, DATA_SIZE, PAGE_SIZE, DEBUG>::
Send(const Pointer<TlmMessage<MemoryRequest<PHYSICAL_ADDR, DATA_SIZE>, 
	MemoryResponse<DATA_SIZE> > >& message) {
	const Pointer<MemoryRequest<PHYSICAL_ADDR, DATA_SIZE> >& req = message->GetRequest();
	Pointer<MemoryResponse<DATA_SIZE> > rsp = new(rsp) MemoryResponse<DATA_SIZE>();
	message->SetResponse(rsp);
	
	switch(req->type)
	{
		case MemoryRequest<PHYSICAL_ADDR, DATA_SIZE>::READ: {
				if(unlikely(DEBUG && this->verbose))
					logger << DebugInfo << LOCATION
						<< sc_core::sc_time_stamp().to_string() 
						<< " Send() received a READ request" << std::endl
						<< EndDebugInfo; 	
				this->ReadMemory(req->addr, rsp->read_data, req->size);
				sc_core::sc_event *rsp_ev = message->GetResponseEvent();
				if(rsp_ev) rsp_ev->notify(cycle_sctime);
			}
			break;
			
		case MemoryRequest<PHYSICAL_ADDR, DATA_SIZE>::WRITE:
			if(unlikely(DEBUG && this->verbose))
				logger << DebugInfo << LOCATION
					<< sc_core::sc_time_stamp().to_string() 
					<< " Send() received a WRITE request" << std::endl
					<< EndDebugInfo; 	
			this->WriteMemory(req->addr, req->write_data, req->size);
			break;
	}
	return true;
}

} // end of namespace unisim
} // end of namespace component
} // end of namespace tlm
} // end of namespace memory 
} // end of namespace ram 

#undef LOCATION

#endif // __UNISIM_COMPONENT_TLM_MEMORY_RAM_MEMORY_TCC__
