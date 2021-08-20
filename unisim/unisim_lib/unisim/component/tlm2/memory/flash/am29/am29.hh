/*
 *  Copyright (c) 2011,
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
 
#ifndef __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_AM29_AM29_HH__
#define __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_AM29_AM29_HH__

#include <systemc>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <unisim/component/cxx/memory/flash/am29/am29.hh>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace flash {
namespace am29 {

using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::Object;
using unisim::kernel::Client;
using unisim::kernel::variable::Parameter;
  using namespace sc_core;

typedef uint64_t DEFAULT_ADDRESS;
const unsigned int DEFAULT_BUSWIDTH = 32; // 32-bit bus

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH, unsigned int BUSWIDTH = DEFAULT_BUSWIDTH>
class AM29 :
	public sc_core::sc_module,
	public unisim::component::cxx::memory::flash::am29::AM29<CONFIG, BYTESIZE, IO_WIDTH>,
	public tlm::tlm_fw_transport_if<>
{
public:
	typedef unisim::component::cxx::memory::flash::am29::AM29<CONFIG, BYTESIZE, IO_WIDTH> inherited;

	tlm::tlm_target_socket<BUSWIDTH> slave_sock;

	AM29(const sc_core::sc_module_name& name, Object *parent = 0);
	virtual ~AM29();

	virtual bool BeginSetup();
	
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
private:
	sc_core::sc_time cycle_time;
	Parameter<sc_core::sc_time> param_cycle_time;
};

} // end of namespace am29
} // end of namespace flash
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_AM29_AM29_HH__ 
