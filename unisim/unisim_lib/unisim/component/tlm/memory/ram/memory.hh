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
 
#ifndef __UNISIM_COMPONENT_TLM_MEMORY_RAM_MEMORY_HH__
#define __UNISIM_COMPONENT_TLM_MEMORY_RAM_MEMORY_HH__

#include <systemc>
#include "unisim/kernel/tlm/tlm.hh"
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include "unisim/component/tlm/message/memory.hh"
#include "unisim/component/cxx/memory/ram/memory.hh"
#include "unisim/kernel/logger/logger.hh"

namespace unisim {
namespace component {
namespace tlm {
namespace memory {
namespace ram {

using unisim::kernel::tlm::TlmMessage;
using unisim::kernel::tlm::TlmSendIf;
using unisim::component::tlm::message::MemoryRequest;
using unisim::component::tlm::message::MemoryResponse;
using unisim::util::garbage_collector::Pointer;
using unisim::kernel::Object;
using unisim::kernel::Client;
using unisim::kernel::variable::Parameter;
using unisim::kernel::ServiceImport;

/**
 * Memory module for the simple bus.
 * It implements a tlm module using the TlmSendIf interface and (as it inherits from the memory service) the 
 *   service methods (Read and WriteMemory).
 */
template <class PHYSICAL_ADDR, uint32_t DATA_SIZE, uint32_t PAGE_SIZE = 1024 * 1024, bool DEBUG = false>
class Memory :
	public sc_core::sc_module,
	public unisim::component::cxx::memory::ram::Memory<PHYSICAL_ADDR, PAGE_SIZE>,
	public TlmSendIf<MemoryRequest<PHYSICAL_ADDR, DATA_SIZE>, 
					MemoryResponse<DATA_SIZE> > {
public:
	/**
	 * Memory port access. It is connected to the bus or the chipset.
	 * Uses a tlm interface.
	 */
	sc_core::sc_export<TlmSendIf<MemoryRequest<PHYSICAL_ADDR, DATA_SIZE>, 
						MemoryResponse<DATA_SIZE> > > slave_port;

	/**
	 * Constructor.
	 * 
	 * @param name the name of the module
	 * @param parent the parent service
	 */
	Memory(const sc_core::sc_module_name& name, Object *parent = 0);
	/**
	 * Destructor
	 */
	virtual ~Memory();

	/* Service methods */
	/** Setup
	 * Initializes the service interface. */
	virtual bool EndSetup();		
	
	/* Methods required by the TlmSendIf (Tlm send interface) */
	/** 
	 * Module Send method to receive memory requests (READ and WRITE) 
	 * 
	 * @param message the request to handle
	 * 
	 * @return false if the request can not be handled right now, true otherwise 
	 */
	virtual bool Send(const Pointer<TlmMessage<MemoryRequest<PHYSICAL_ADDR, DATA_SIZE>, 
											MemoryResponse<DATA_SIZE> > >& message);
	
private:
	/** The logger */
	unisim::kernel::logger::Logger logger;
	
	/** The frequency in sc_time format */
	sc_core::sc_time cycle_sctime;
	/** The parameter to set frequency */
	Parameter<sc_core::sc_time> param_cycle_time;
};

} // end of namespace unisim
} // end of namespace component
} // end of namespace tlm
} // end of namespace memory 
} // end of namespace ram 

#endif // __UNISIM_COMPONENT_TLM_MEMORY_RAM_MEMORY_HH__ 
