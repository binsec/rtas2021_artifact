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
 
#ifndef __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_ATU_ATU_TCC__
#define __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_ATU_ATU_TCC__

#include <unisim/util/likely/likely.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {
namespace atu {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

#define LOCATION "In file " << __FILE__ << ", function " << __FUNCTION__ << ", line #" << __LINE__

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
ATU(const char *name, Object *parent) :
	Object(name, parent, "MPC107 integrated Address Translation Unit (ATU)"),
	logger(*this),
	verbose(false),
	param_verbose("verbose", this, verbose, "enable/disable verbosity") {
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
bool 
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
MemWrite(ADDRESS_TYPE addr, uint8_t *write_data, unsigned int size) {
	uint32_t data = 0;
	
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "System bus write access:" << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size << std::endl
			<< " - write_data =" << std::hex;
		for(unsigned int i = 0; i < size; i++)
			logger << " " << (unsigned int)write_data[i];
		logger << std::endl
			<< EndDebugInfo;
	}
	/* check the incoming size */
	if(size != 4) { // only 4 byte accesses allowed
		if(unlikely(DEBUG && verbose)) {
			logger << DebugError << LOCATION
				<< "ATU accesses must be 4 bytes size, ignoring operation"
				<< std::endl << EndDebugError;
		}
		return true; 
	}
	/* extract the write data */
	for(unsigned int i = 0; i < size; i++)
		data += (((uint32_t)(write_data[i])) << (i * 8));
	/* check the incoming address and set the register */
	addr = addr & (ADDRESS_TYPE)0xffff;
	switch(addr) {
	case Registers::ITWR:
		return SetITWR(data);
		break;
	case Registers::OMBAR:
		return SetOMBAR(data);
		break;
	case Registers::OTWR:
		return SetOTWR(data);
		break;
	default:
		if(unlikely(DEBUG && verbose)) {
			logger << DebugError << LOCATION
				<< "Write access to unknown ATU register, ignoring operation"
				<< std::endl << EndDebugError;
		}
		return true;
		break;
	}
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
bool 
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
MemRead(ADDRESS_TYPE addr, uint8_t *read_data, unsigned int size) {
	uint32_t data = 0;
	
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "System bus read access:" << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size << std::endl
			<< EndDebugInfo;
	}
	/* check the incoming size */
	if(size != 4) { // only 4 byte accesses allowed
		if(unlikely(DEBUG && verbose)) {
			logger << DebugError << LOCATION
				<< "ATU accesses must be 4 bytes size, ignoring operation"
				<< std::endl << EndDebugError;
		}
		return true; 
	}
	/* check the incoming address and get the register value */
	addr = addr & (ADDRESS_TYPE)0xffff;
	switch(addr) {
	case Registers::ITWR:
		data = GetITWR();
		break;
	case Registers::OMBAR:
		data = GetOMBAR();
		break;
	case Registers::OTWR:
		data = GetOTWR();
		break;
	default:
		if(unlikely(DEBUG && verbose)) {
			logger << DebugError << LOCATION
				<< "Read access to unknown ATU register, ignoring operation"
				<< std::endl << EndDebugError;
		}
		return true;
		break;
	}
	/* set the read data */
	for(unsigned int i = 0; i < size; i++)
		read_data[i] = (uint8_t)((data >> (i * 8)) & (uint32_t)0x0ff);
	
	return true;
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
bool 
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
PCIWrite(PCI_ADDRESS_TYPE addr, uint8_t *write_data, unsigned int size) {
	uint32_t data = 0;
	
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "PCI bus write access:" << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size << std::endl
			<< " - write_data =" << std::hex;
		for(unsigned int i = 0; i < size; i++)
			logger << " " << (unsigned int)write_data[i];
		logger << std::endl
			<< EndDebugInfo;
	}
	/* check the incoming size */
	if(size != 4) { // only 4 byte accesses allowed
		if(unlikely(DEBUG && verbose)) {
			logger << DebugError << LOCATION
				<< "ATU accesses must be 4 bytes size, ignoring operation"
				<< std::endl << EndDebugError;
		}
		return true; 
	}
	/* extract the write data */
	for(unsigned int i = 0; i < size; i++)
		data += (((uint32_t)(write_data[i])) << (i * 8));
	/* check the incoming address and set the register */
	addr = addr & (ADDRESS_TYPE)0x0fff;
	switch(addr) {
	case Registers::PCI_ITWR:
		return SetITWR(data);
		break;
	case Registers::PCI_OMBAR:
		return SetOMBAR(data);
		break;
	case Registers::PCI_OTWR:
		return SetOTWR(data);
		break;
	default:
		if(unlikely(DEBUG && verbose)) {
			logger << DebugError << LOCATION
				<< "Write access to unknown ATU register, ignoring operation"
				<< std::endl << EndDebugError;
		}
		return true;
		break;
	}
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
bool 
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
PCIRead(PCI_ADDRESS_TYPE addr, uint8_t *read_data, unsigned int size) {
	uint32_t data = 0;
	
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "PCI bus read access:" << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size << std::endl
			<< EndDebugInfo;
	}
	/* check the incoming size */
	if(size != 4) { // only 4 byte accesses allowed
		if(unlikely(DEBUG && verbose)) {
			logger << DebugError << LOCATION
				<< "ATU accesses must be 4 bytes size, ignoring operation"
				<< std::endl << EndDebugError;
		}
		return true; 
	}
	/* check the incoming address and get the register value */
	addr = addr & (ADDRESS_TYPE)0x0fff;
	switch(addr) {
	case Registers::PCI_ITWR:
		data = GetITWR();
		break;
	case Registers::PCI_OMBAR:
		data = GetOMBAR();
		break;
	case Registers::PCI_OTWR:
		data = GetOTWR();
		break;
	default:
		if(unlikely(DEBUG && verbose)) {
			logger << DebugError << LOCATION
				<< "Read access to unknown ATU register, ignoring operation"
				<< std::endl << EndDebugError;
		}
		return true;
		break;
	}
	/* set the read data */
	for(unsigned int i = 0; i < size; i++)
		read_data[i] = (uint8_t)((data >> (i * 8)) & (uint32_t)0x0ff);
	
	return true;
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
uint32_t 
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
GetITWR() {
	return regs.itwr;
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
uint32_t 
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
GetOMBAR() {
	return regs.ombar;
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
uint32_t 
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
GetOTWR() {
	return regs.otwr;
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
bool 
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
SetITWR(uint32_t data) {
	uint32_t orig_data = data;

	/* check reserved bits */
	data = data & ~((uint32_t)0x80000fe0);
	if(data != orig_data) {
		if(unlikely(DEBUG && verbose)) {
			logger << DebugWarning << LOCATION
				<< "Trying to modify ITWR reserved bits, masking reserved bits:" << std::endl
				<< " - original value = 0x" << std::hex << orig_data << std::dec << std::endl
				<< " - masked value = 0x" << std::hex << data << std::dec << std::endl
				<< EndDebugWarning;
		}
	}
	/* check inbound window size allowed values */
	uint32_t win_size = data & (uint32_t)0x01f;
	if((win_size >= (uint32_t)0x01 && win_size <= (uint32_t)0x0a) ||
			win_size >= (uint32_t)0x01e) {
		if(unlikely(DEBUG && verbose)) {
			logger << DebugError << LOCATION
				<< "Trying to write ITWR with invalid window size (0x"
				<< std::hex << win_size << std::dec << "), setting it to 0"
				<< std::endl << EndDebugError;
		}
		data = data & ~((uint32_t)0x01f);
	}
	/* set register value */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Modifying ITWR:" << std::endl
			<< " - previous value = 0x" << std::hex << regs.itwr << std::dec << std::endl
			<< " - new value = 0x" << std::hex << data << std::dec << std::endl
			<< EndDebugInfo;
	}
	regs.itwr = data;
	return true;
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
bool
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
SetOMBAR(uint32_t data) {
	uint32_t orig_data = data;
	
	/* check reserved bits */
	data = data | (uint32_t)0x80000000;
	if(data != orig_data) {
		if(unlikely(DEBUG && verbose)) {
			logger << DebugWarning << LOCATION
				<< "Trying to modify OMBAR bit 31, resetting it to 1:" << std::endl
				<< " - original value = 0x" << std::hex << orig_data << std::dec << std::endl
				<< " - masked value = 0x" << std::hex << data << std::dec << std::endl
				<< EndDebugWarning;
		}
		orig_data = data;
	}
	data = data & ~((uint32_t)0x0fff);
	if(data != orig_data) {
		if(unlikely(DEBUG && verbose)) {
			logger << DebugWarning << LOCATION
				<< "Trying to modify OMBAR reserved bits, resetting them to 0:" << std::endl
				<< " - original value = 0x" << std::hex << orig_data << std::dec << std::endl
				<< " - masked value = 0x" << std::hex << data << std::dec << std::endl
				<< EndDebugWarning;
		}
	}
	/* set register value */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Modifying OMBAR:" << std::endl
			<< " - previous value = 0x" << std::hex << regs.ombar << std::dec << std::endl
			<< " - new value = 0x" << std::hex << data << std::dec << std::endl
			<< EndDebugInfo;
	}
	regs.ombar = data;
	return true;;
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
bool
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
SetOTWR(uint32_t data) {
	uint32_t orig_data = data;

	/* check reserved bits */
	data = data & ~((uint32_t)0x80000fe0);
	if(data != orig_data) {
		if(unlikely(DEBUG && verbose)) {
			logger << DebugWarning << LOCATION
				<< "Trying to modify OTWR reserved bits, masking reserved bits:" << std::endl
				<< " - original value = 0x" << std::hex << orig_data << std::dec << std::endl
				<< " - masked value = 0x" << std::hex << data << std::dec << std::endl
				<< EndDebugWarning;
		}
	}
	/* check inbound window size allowed values */
	uint32_t win_size = data & (uint32_t)0x01f;
	if((win_size >= (uint32_t)0x01 && win_size <= (uint32_t)0x0a) ||
			win_size >= (uint32_t)0x01e) {
		if(unlikely(DEBUG && verbose)) {
			logger << DebugError << LOCATION
				<< "Trying to write 0TWR with invalid window size (0x"
				<< std::hex << win_size << std::dec << "), setting it to 0"
				<< std::endl << EndDebugError;
		}
		data = data & ~((uint32_t)0x01f);
	}
	/* set register value */
	if(unlikely(DEBUG && verbose)) {
		logger << DebugInfo << LOCATION
			<< "Modifying OTWR:" << std::endl
			<< " - previous value = 0x" << std::hex << regs.otwr << std::dec << std::endl
			<< " - new value = 0x" << std::hex << data << std::dec << std::endl
			<< EndDebugInfo;
	}
	regs.otwr = data;
	return true;
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
bool
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
InboundAddressTranslationEnabled() {
	if((regs.itwr & (uint32_t)0x01f) == (uint32_t)0)
		return false;
	return true;
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
bool
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
OutboundAddressTranslationEnabled() {
	if((regs.otwr & (uint32_t)0x01f) == (uint32_t)0)
		return false;
	return true;
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
uint32_t
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
InboundTranslationWindowSize() {
	return 1 << (regs.itwr & (uint32_t)0x01f);
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
uint32_t
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
OutboundTranslationWindowSize() {
	return 1 << (regs.otwr & (uint32_t)0x01f);
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
uint32_t
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
InboundTranslationBaseAddress() {
	return regs.itwr & (uint32_t)0xfffff000;
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
uint32_t
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
OutboundTranslationBaseAddress() {
	return regs.otwr & (uint32_t)0xfffff000;
}

template<class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG>
uint32_t
ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> ::
OutboundMemoryBaseAddress() {
	return regs.ombar & (uint32_t)0xfffff000;
}

} // end of namespace atu
} // end of namespace unisim
} // end of namespace component
} // end of namespace cxx
} // end of namespace chipset
} // end of namespace mpc107

#endif // __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_ATU_ATU_TCC__
