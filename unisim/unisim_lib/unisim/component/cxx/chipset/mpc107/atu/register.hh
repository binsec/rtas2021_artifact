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
 
#ifndef __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_ATU_REGISTER_HH__
#define __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_ATU_REGISTER_HH__

#include <inttypes.h>
#include <string>

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {
namespace atu {

using std::string;

class Registers {
public:
	Registers();
	~Registers();
	
	/** inbound translation window register */
	uint32_t itwr;
	/** outbound memory base address register */
	uint32_t ombar;
	/** outbound translation window register */
	uint32_t otwr;

	/** registers addresses from system bus view */
	static const uint32_t ITWR = (uint32_t)0x2310,
		OMBAR = (uint32_t)0x2300,
		OTWR = (uint32_t)0x2308;
	/** registers addresses from pci view */
	static const uint32_t PCI_ITWR = (uint32_t)0x310,
		PCI_OMBAR = (uint32_t)0x300,
		PCI_OTWR = (uint32_t)0x308;

	/* registers names */
	static const string
		ITWR_NAME,
		OMBAR_NAME,
		OTWR_NAME;
		
private:
};

} // end of namespace atu
} // end of namespace unisim
} // end of namespace component
} // end of namespace cxx
} // end of namespace chipset
} // end of namespace mpc107

#endif // __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_ATU_REGISTER_HH__
