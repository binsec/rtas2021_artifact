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
 
#ifndef __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_ATU_ATU_HH__
#define __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_ATU_ATU_HH__

#include "unisim/kernel/kernel.hh"
#include <unisim/kernel/variable/variable.hh>
#include "unisim/kernel/logger/logger.hh"
#include "unisim/component/cxx/chipset/mpc107/atu/register.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {
namespace atu {

using unisim::kernel::Object;
using unisim::kernel::ServiceImport;
using unisim::kernel::variable::Parameter;
using unisim::component::cxx::chipset::mpc107::atu::Registers;

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG = false>
class ATU :
	virtual public Object {
public:
	ATU(const char *name, Object *parent = 0);

	bool MemWrite(ADDRESS_TYPE addr, uint8_t *write_data, unsigned int size);
	bool MemRead(ADDRESS_TYPE addr, uint8_t *read_data, unsigned int size);
	bool PCIWrite(PCI_ADDRESS_TYPE addr, uint8_t *write_data, unsigned int size);
	bool PCIRead(PCI_ADDRESS_TYPE addr, uint8_t *read_data, unsigned int size);

	uint32_t GetITWR();
	uint32_t GetOMBAR();
	uint32_t GetOTWR();
	
	bool InboundAddressTranslationEnabled();
	bool OutboundAddressTranslationEnabled();
	uint32_t InboundTranslationWindowSize();
	uint32_t OutboundTranslationWindowSize();
	uint32_t InboundTranslationBaseAddress();
	uint32_t OutboundTranslationBaseAddress();
	uint32_t OutboundMemoryBaseAddress();
	// note: the inbound memory base address can not be found in the ATU, but
	//   in the LMBAR register of the MPC107

protected:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	Parameter<bool> param_verbose;
private:
	Registers regs;
	
	bool SetITWR(uint32_t data);
	bool SetOMBAR(uint32_t data);
	bool SetOTWR(uint32_t data);
};

} // end of namespace atu
} // end of namespace unisim
} // end of namespace component
} // end of namespace cxx
} // end of namespace chipset
} // end of namespace mpc107

#endif // __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_ATU_ATU_HH__
