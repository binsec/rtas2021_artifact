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

#ifndef __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_DMA_DMA_TCC__
#define __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_DMA_DMA_TCC__

#include <unisim/util/likely/likely.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {
namespace dma {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

#define LOCATION "In file " << __FILE__ << ", function " << __FUNCTION__ << ", line #" << __LINE__

template <class PHYSICAL_ADDR,
		 bool DEBUG>
DMA<PHYSICAL_ADDR, DEBUG> ::
DMA(DMAClientInterface<PHYSICAL_ADDR> *_client,
	const char *name, Object *parent)
	: Object(name, parent, "MPC107 integrated Direct Memory Access (DMA) controller")
	, logger(*this)
	, verbose(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, reg()
	, client(_client)
{
}

template <class PHYSICAL_ADDR,
		 bool DEBUG>
DMA<PHYSICAL_ADDR, DEBUG> ::
~DMA() {
}

template <class PHYSICAL_ADDR,
		 bool DEBUG>
bool
DMA<PHYSICAL_ADDR, DEBUG> ::
BeginSetup() {
	return true;
}

template <class PHYSICAL_ADDR,
		 bool DEBUG>
void
DMA<PHYSICAL_ADDR, DEBUG> ::
Reset() {
}

template <class PHYSICAL_ADDR,
		 bool DEBUG>
uint32_t 
DMA<PHYSICAL_ADDR, DEBUG> ::
ReadRegister(PHYSICAL_ADDR addr, uint32_t size, bool pci_access) {
 	PHYSICAL_ADDR dma_addr = 0;
 	uint32_t val = 0;
	
 	dma_addr = MaskAddress(addr, pci_access);
 	switch(dma_addr) {
 	case Registers::DMR0 :
 		val = GetDMR(0, pci_access);
 		break;
 	case Registers::DSR0 :
 		val = GetDSR(0, pci_access);
 		break;
 	case Registers::CDAR0 :
 		val = GetCDAR(0, pci_access);
 		break;
 	case Registers::SAR0 :
 		val = GetSAR(0, pci_access);
 		break;
 	case Registers::DAR0 :
 		val = GetDAR(0, pci_access);
 		break;
 	case Registers::BCR0 :
 		val = GetBCR(0, pci_access);
 		break;
 	case Registers::NDAR0 :
 		val = GetNDAR(0, pci_access);
 		break;
 	case Registers::DMR1 :
 		val = GetDMR(1, pci_access);
 		break;
 	case Registers::DSR1 :
 		val = GetDSR(1, pci_access);
 		break;
 	case Registers::CDAR1 :
 		val = GetCDAR(1, pci_access);
 		break;
 	case Registers::SAR1 :
 		val = GetSAR(1, pci_access);
 		break;
 	case Registers::DAR1 :
 		val = GetDAR(1, pci_access);
 		break;
 	case Registers::BCR1 :
 		val = GetBCR(1, pci_access);
 		break;
 	case Registers::NDAR1 :
 		val = GetNDAR(1, pci_access);
 		break;
 	default:
 		if(unlikely(verbose)) {
 			logger << DebugError << LOCATION
 				<< "Unknow address access (0x"
 				<< std::hex << addr << std::dec << ")"
 				<< std::endl << EndDebugError;
 		}
 		break;
 	}
 	return val;
}

template <class PHYSICAL_ADDR,
		 bool DEBUG>
void
DMA<PHYSICAL_ADDR, DEBUG> ::
WriteRegister(PHYSICAL_ADDR addr, uint32_t data, uint32_t size, 
		bool pci_access) {
}

template <class PHYSICAL_ADDR,
		 bool DEBUG>
PHYSICAL_ADDR
DMA<PHYSICAL_ADDR, DEBUG> ::
MaskAddress(PHYSICAL_ADDR addr, bool pci_access) {
	PHYSICAL_ADDR val = addr;

	val = val & 0x0ffff;
	/* make it look as a local access if it is a pci access */
	if(pci_access) {
		val = val | 0x01000; 
	}

	return val;
}

template <class PHYSICAL_ADDR,
					bool DEBUG>
uint32_t
DMA<PHYSICAL_ADDR, DEBUG> ::
GetDMR(unsigned int channel, bool pci_access) {
	return reg.dmr[channel];
}

template <class PHYSICAL_ADDR,
					bool DEBUG>
uint32_t
DMA<PHYSICAL_ADDR, DEBUG> ::
GetDSR(unsigned int channel, bool pci_access) {
	return reg.dsr[channel];
}

template <class PHYSICAL_ADDR,
					bool DEBUG>
uint32_t
DMA<PHYSICAL_ADDR, DEBUG> ::
GetCDAR(unsigned int channel, bool pci_access) {
	return reg.cdar[channel];
}

template <class PHYSICAL_ADDR,
					bool DEBUG>
uint32_t
DMA<PHYSICAL_ADDR, DEBUG> ::
GetSAR(unsigned int channel, bool pci_access) {
	return reg.sar[channel];
}

template <class PHYSICAL_ADDR,
					bool DEBUG>
uint32_t
DMA<PHYSICAL_ADDR, DEBUG> ::
GetDAR(unsigned int channel, bool pci_access) {
	return reg.dar[channel];
}

template <class PHYSICAL_ADDR,
					bool DEBUG>
uint32_t
DMA<PHYSICAL_ADDR, DEBUG> ::
GetBCR(unsigned int channel, bool pci_access) {
	return reg.bcr[channel];
}

template <class PHYSICAL_ADDR,
					bool DEBUG>
uint32_t
DMA<PHYSICAL_ADDR, DEBUG> ::
GetNDAR(unsigned int channel, bool pci_access) {
	return reg.ndar[channel];
}

template <class PHYSICAL_ADDR,
					bool DEBUG>
uint32_t 
DMA<PHYSICAL_ADDR, DEBUG> ::
CheckReservedBits(string &reg_name,
									uint32_t data,
									uint32_t mask) {
	uint32_t out_data = data;

	if(data & mask) {
		out_data = data & ~mask;
		if(unlikely(verbose))
			logger 
				<< DebugWarning << LOCATION
				<< "Trying to write reserved bits of "
				<< reg_name << " (original value = 0x"
				<< std::hex << data << std::dec << ", modified value = 0x"
				<< std::hex << out_data << std::dec << ")"
				<< std::endl << EndDebugWarning;
	}
	return out_data;
}

template <class PHYSICAL_ADDR,
					bool DEBUG>
void
DMA<PHYSICAL_ADDR, DEBUG> ::
LogRegFieldInfo(string &field_name,
								uint32_t data,
								uint32_t mask) {
	unsigned int hbit = 0, lbit = 0;
	bool found = false;
	uint32_t value = 0;
	unsigned int count = 0;

	/* find low bit */
	found = false;
	count = 0;
	for(uint32_t i = 1; !found && i < 32; i = i << 1) {
		if(i & mask) {
			found = true;
			lbit = count;
		} 
		count++;
	}

	/* find high bit */
	found = false;
	count = 31;
	for(uint32_t i = (uint32_t)0x80000000;
			!found && i > 0; i = i >> 1) {
		if(i & mask) {
			found = true;
			hbit = count;
		}
		count--;
	}

	/* get field value */
	value = data & mask;
	value = value >> lbit;

	logger << std::endl << " - " << field_name << "(bit";
	if(hbit != lbit) 
		logger << "s " << hbit << "-" << lbit;
	else
		logger << " " << hbit;
	logger 
		<< ") = 0x" << std::hex << value << std::dec << std::endl;
}

template <class PHYSICAL_ADDR,
					bool DEBUG>
void
DMA<PHYSICAL_ADDR, DEBUG> ::
SetDMR(unsigned int channel, uint32_t data, bool pci_access) {
	string reg_name = channel?
		Registers::DMR_NAME_1:
		Registers::DMR_NAME_0;
	uint32_t previous_data = reg.dmr[channel];

	/* check that reserved bits are not modified */
	data = CheckReservedBits(reg_name,
													 data,
													 Registers::DMR_RESERVED_MASK);

	/* TODO: periodic DMA not supported, 
	 *   needs to be implemented. */
	if(data & Registers::DMR_PDE) {
		if(unlikely(verbose))
			logger
				<< DebugWarning << LOCATION
				<< "Periodic DMA not supported, setting PDE bit "
				<< "(bit 18) back to 0"
				<< EndDebugWarning;
		data = data & ~Registers::DMR_PDE;
	}

	/* TODO: chaining mode not supported,
	 *   needs to be implemented. */
	if(data & ~Registers::DMR_CTM) {
		if(unlikely(verbose))
			logger
				<< DebugWarning << LOCATION
				<< "Chaining mode not supported, setting CTM bit "
				<< "(bit 2) to 1"
				<< EndDebugWarning;
		data = data | Registers::DMR_CTM;
	}

	/* TODO: chaining mode not supported,
	 *   needs to be implemented. */
	if(data & Registers::DMR_CC) {
		if(unlikely(verbose))
			logger
				<< DebugWarning << LOCATION
				<< "Chaining mode not supported, setting CC bit "
				<< "(bit 1) back to 0"
				<< EndDebugWarning;
		data = data & ~Registers::DMR_CC;
	}

	/* check that DAHE and SAHE are not set at the same time,
	 *   if both are set notify the error, and reset both of 
	 *   them (NOTE: we could reset only one, but there is 
	 *   no a correct solution, this situation should never
	 *   occur) */
	if((data & Registers::DMR_DAHE) && (data & Registers::DMR_SAHE)) {
		if(unlikely(verbose)) {
			logger
				<< DebugError << LOCATION
				<< "Bits DMR[DAHE] and DMR[SAHE] are set at the same time, "
				<< "resetting both bits to 0"
				<< EndDebugError;
		}
		data = data & ~Registers::DMR_DAHE;
		data = data & ~Registers::DMR_SAHE;
	}

	/* if DEBUG on then display bits that are set */
	if(unlikely(DEBUG && verbose)) {
		logger
			<< DebugInfo << LOCATION
			<< "Setting register " << reg_name
			<< " with value 0x" << std::hex << data << std::dec << ":";
		LogRegFieldInfo(*new string("LMDC"), data, Registers::DMR_LMDC);
		LogRegFieldInfo(*new string("IRQS"), data, Registers::DMR_IRQS);
		LogRegFieldInfo(*new string("PDE"), data, Registers::DMR_PDE);
		LogRegFieldInfo(*new string("DAHTS"), data, Registers::DMR_DAHTS);
		LogRegFieldInfo(*new string("SAHTS"), data, Registers::DMR_SAHTS);
		LogRegFieldInfo(*new string("DAHE"), data, Registers::DMR_DAHE);
		LogRegFieldInfo(*new string("SAHE"), data, Registers::DMR_SAHE);
		LogRegFieldInfo(*new string("PRC"), data, Registers::DMR_PRC);
		LogRegFieldInfo(*new string("EIE"), data, Registers::DMR_EIE);
		LogRegFieldInfo(*new string("EOTIE"), data, Registers::DMR_EOTIE);
		LogRegFieldInfo(*new string("DL"), data, Registers::DMR_DL);
		LogRegFieldInfo(*new string("CTM"), data, Registers::DMR_CTM);
		LogRegFieldInfo(*new string("CC"), data, Registers::DMR_CC);
		LogRegFieldInfo(*new string("CS"), data, Registers::DMR_CS);
		logger << EndDebugInfo;
	}

	/* set register */
	reg.dmr[channel] = data;

	/* check if a channel transfer needs to be done */
	if((data & Registers::DMR_CS) != (previous_data & Registers::DMR_CS)) {
		if((data & Registers::DMR_CS) && 
			 !(reg.dsr[channel] & Registers::DSR_CB))
			ChannelStart(channel);
		else
			ChannelStop(channel);
	}
}

template <class PHYSICAL_ADDR,
					bool DEBUG>
void
DMA<PHYSICAL_ADDR, DEBUG> ::
SetDSR(unsigned int channel, uint32_t data, bool pci_access) {
	string reg_name = channel ?
		Registers::DSR_NAME_1:
		Registers::DSR_NAME_0;

	/* check that reserved bits are not modified */
	data = CheckReservedBits(reg_name,
													 data,
													 Registers::DSR_RESERVED_MASK);

	/* check that CB bit is not modified */
	if((data & Registers::DSR_CB) != (reg.dsr[channel] & Registers::DSR_CB)) {
		if(unlikely(verbose)) 
			logger
				<< DebugWarning << LOCATION
				<< "Trying to modify read-only register field DSR[CB],"
				<< " setting it back to "
				<< (reg.dsr[channel] & Registers::DSR_CB?"1":"0")
				<< EndDebugWarning;
		if(reg.dsr[channel] & Registers::DSR_CB)
			data = data | Registers::DSR_CB;
		else
			data = data & ~Registers::DSR_CB;
	}

	/* check the LME is removed only if set */
	if((data & Registers::DSR_LME) && 
		 (reg.dsr[channel] | ~Registers::DSR_LME)) {
		if(unlikely(verbose))
			logger
				<< DebugWarning << LOCATION
				<< "Trying to unset DSR[LME] when this is not set, "
				<< "setting it to 0" << EndDebugWarning;
		data = data & ~Registers::DSR_LME;
	}

	/* check the PE is removed only if set */
	if((data & Registers::DSR_PE) && 
		 (reg.dsr[channel] | ~Registers::DSR_PE)) {
		if(unlikely(verbose))
			logger
				<< DebugWarning << LOCATION
				<< "Trying to unset DSR[PE] when this is not set, "
				<< "setting it to 0" << EndDebugWarning;
		data = data & ~Registers::DSR_PE;
	}

	/* check the EOSI is removed only if set */
	if((data & Registers::DSR_EOSI) && 
		 (reg.dsr[channel] | ~Registers::DSR_EOSI)) {
		if(unlikely(verbose))
			logger
				<< DebugWarning << LOCATION
				<< "Trying to unset DSR[EOSI] when this is not set, "
				<< "setting it to 0" << EndDebugWarning;
		data = data & ~Registers::DSR_EOSI;
	}

	/* check the EOCAI is removed only if set */
	if((data & Registers::DSR_EOCAI) && 
		 (reg.dsr[channel] | ~Registers::DSR_EOCAI)) {
		if(unlikely(verbose))
			logger
				<< DebugWarning << LOCATION
				<< "Trying to unset DSR[EOCAI] when this is not set, "
				<< "setting it to 0" << EndDebugWarning;
		data = data & ~Registers::DSR_EOCAI;
	}

	/* if DEBUG on then display bits that are set */
	if(unlikely(DEBUG && verbose)) {
		logger
			<< DebugInfo << LOCATION
			<< "Setting register " << reg_name
			<< " with value 0x" << std::hex << data << std::dec << ":";
		LogRegFieldInfo(*new string("LME"), data, Registers::DSR_LME);
		LogRegFieldInfo(*new string("PE"), data, Registers::DSR_PE);
		LogRegFieldInfo(*new string("CB"), data, Registers::DSR_CB);
		LogRegFieldInfo(*new string("EOSI"), data, Registers::DSR_EOSI);
		LogRegFieldInfo(*new string("EOCAI"), data, Registers::DSR_EOCAI);
		logger << EndDebugInfo;
	}

	/* START set register */
	/* check if LME needs to be clearead */
	if(data & Registers::DSR_LME) {
		if(unlikely(DEBUG && verbose))
			logger
				<< DebugInfo << LOCATION
				<< "Clearing DSR[LME]"
				<< EndDebugInfo;
		reg.dsr[channel] = reg.dsr[channel] & ~Registers::DSR_LME;
	}
	/* check if PE needs to be clearead */
	if(data & Registers::DSR_PE) {
		if(unlikely(DEBUG && verbose))
			logger
				<< DebugInfo << LOCATION
				<< "Clearing DSR[PE]"
				<< EndDebugInfo;
		reg.dsr[channel] = reg.dsr[channel] & ~Registers::DSR_PE;
	}
	/* check if EOSI needs to be clearead */
	if(data & Registers::DSR_EOSI) {
		if(unlikely(DEBUG && verbose))
			logger
				<< DebugInfo << LOCATION
				<< "Clearing DSR[EOSI]"
				<< EndDebugInfo;
		reg.dsr[channel] = reg.dsr[channel] & ~Registers::DSR_EOSI;
	}
	/* check if EOCAI needs to be clearead */
	if(data & Registers::DSR_EOCAI) {
		if(unlikely(DEBUG && verbose))
			logger
				<< DebugInfo << LOCATION
				<< "Clearing DSR[EOCAI]"
				<< EndDebugInfo;
		reg.dsr[channel] = reg.dsr[channel] & ~Registers::DSR_EOCAI;
	}
	/* END set register */
}

template <class PHYSICAL_ADDR,
					bool DEBUG>
void
DMA<PHYSICAL_ADDR, DEBUG> ::
SetCDAR(unsigned int channel, uint32_t data, bool pci_access) {
	string reg_name = channel ?
		Registers::CDAR_NAME_1:
		Registers::CDAR_NAME_0;

	/* check that reserved bits are not modified */
	data = CheckReservedBits(reg_name,
													 data,
													 Registers::CDAR_RESERVED_MASK);

	/* TODO: snooping not supported,
	 *   needs to be implemented. */
	if(data & Registers::CDAR_SNEN) {
		if(unlikely(verbose))
			logger
				<< DebugWarning << LOCATION
				<< "DMA snooping not supported, setting CDAR[SNEN] bit "
				<< "(bit 4) to 0"
				<< EndDebugWarning;
		data = data & ~Registers::CDAR_SNEN;
	}

	/* TODO: channel mode not supported,
	 *   needs to be implemented. */
	if(data & Registers::CDAR_EOSIE) {
		if(unlikely(verbose))
			logger
				<< DebugWarning << LOCATION
				<< "Channel mode not supported, setting CDAR[EOSIE] bit "
				<< "(bit 3) to 0"
				<< EndDebugWarning;
		data = data & ~Registers::CDAR_EOSIE;
	}

	/* if DEBUG on then display bits that are set */
	if(unlikely(DEBUG && verbose)) {
		logger
			<< DebugInfo << LOCATION
			<< "Setting register " << reg_name
			<< " with value 0x" << std::hex << data << std::dec << ":";
		LogRegFieldInfo(*new string("CDA"), data, Registers::CDAR_CDA);
		LogRegFieldInfo(*new string("SNEN"), data, Registers::CDAR_SNEN);
		LogRegFieldInfo(*new string("EOSIE"), data, Registers::CDAR_EOSIE);
		LogRegFieldInfo(*new string("CTT"), data, Registers::CDAR_CTT);
		logger << EndDebugInfo;
	}

	/* set register */
	reg.cdar[channel] = data;
}

template <class PHYSICAL_ADDR,
					bool DEBUG>
void
DMA<PHYSICAL_ADDR, DEBUG> ::
SetSAR(unsigned int channel, uint32_t data, bool pci_access) {
	string reg_name = channel ?
		Registers::SAR_NAME_1:
		Registers::SAR_NAME_0;

	/* check that reserved bits are not modified */
	data = CheckReservedBits(reg_name,
													 data,
													 Registers::SAR_RESERVED_MASK);

	/* if DEBUG on then display bits that are set */
	if(unlikely(DEBUG && verbose)) {
		logger
			<< DebugInfo << LOCATION
			<< "Setting register " << reg_name
			<< " with value 0x" << std::hex << data << std::dec << ":";
		LogRegFieldInfo(*new string("SAR"), data, Registers::SAR_SAR);
		logger << EndDebugInfo;
	}

	/* set register */
	reg.sar[channel] = data;
}

template <class PHYSICAL_ADDR,
					bool DEBUG>
void
DMA<PHYSICAL_ADDR, DEBUG> ::
SetDAR(unsigned int channel, uint32_t data, bool pci_access) {
	string reg_name = channel ?
		Registers::DAR_NAME_1:
		Registers::DAR_NAME_0;

	/* check that reserved bits are not modified */
	data = CheckReservedBits(reg_name,
													 data,
													 Registers::DAR_RESERVED_MASK);

	/* if DEBUG on then display bits that are set */
	if(unlikely(DEBUG && verbose)) {
		logger
			<< DebugInfo << LOCATION
			<< "Setting register " << reg_name
			<< " with value 0x" << std::hex << data << std::dec << ":";
		LogRegFieldInfo(*new string("DAR"), data, Registers::DAR_DAR);
		logger << EndDebugInfo;
	}

	/* set register */
	reg.dar[channel] = data;
}

template <class PHYSICAL_ADDR,
					bool DEBUG>
void
DMA<PHYSICAL_ADDR, DEBUG> ::
SetBCR(unsigned int channel, uint32_t data, bool pci_access) {
	string reg_name = channel ?
		Registers::BCR_NAME_1:
		Registers::BCR_NAME_0;

	/* check that reserved bits are not modified */
	data = CheckReservedBits(reg_name,
													 data,
													 Registers::BCR_RESERVED_MASK);

	/* if DEBUG on then display bits that are set */
	if(unlikely(DEBUG && verbose)) {
		logger
			<< DebugInfo << LOCATION
			<< "Setting register " << reg_name
			<< " with value 0x" << std::hex << data << std::dec << ":";
		LogRegFieldInfo(*new string("BCR"), data, Registers::BCR_BCR);
		logger << EndDebugInfo;
	}

	/* set register */
	reg.bcr[channel] = data;
}

template <class PHYSICAL_ADDR,
					bool DEBUG>
void
DMA<PHYSICAL_ADDR, DEBUG> ::
SetNDAR(unsigned int channel, uint32_t data, bool pci_access) {
	string reg_name = channel ?
		Registers::NDAR_NAME_1:
		Registers::NDAR_NAME_0;

	/* check that reserved bits are not modified */
	data = CheckReservedBits(reg_name,
													 data,
													 Registers::NDAR_RESERVED_MASK);

	/* TODO: snooping not supported,
	 *   needs to be implemented. */
	if(data & Registers::NDAR_NDSNEN) {
		if(unlikely(verbose))
			logger
				<< DebugWarning << LOCATION
				<< "DMA snooping not supported, setting NDAR[NDSNEN] bit "
				<< "(bit 4) to 0"
				<< EndDebugWarning;
		data = data & ~Registers::CDAR_SNEN;
	}

	/* if DEBUG on then display bits that are set */
	if(unlikely(DEBUG && verbose)) {
		logger
			<< DebugInfo << LOCATION
			<< "Setting register " << reg_name
			<< " with value 0x" << std::hex << data << std::dec << ":";
		LogRegFieldInfo(*new string("NDA"), data, Registers::NDAR_NDA);
		LogRegFieldInfo(*new string("NDSNEN"), data, Registers::NDAR_NDSNEN);
		LogRegFieldInfo(*new string("NDEOSIE"), data, Registers::NDAR_NDEOSIE);
		LogRegFieldInfo(*new string("NDCTT"), data, Registers::NDAR_NDCTT);
		LogRegFieldInfo(*new string("EOTD"), data, Registers::NDAR_EOTD);
		logger << EndDebugInfo;
	}

	/* set register */
	reg.ndar[channel] = data;
}

/**
 * Method to send DMA read requests.
 * check how much data has to be read:
 * - get base address to read
 * - compute amount of data to read:
 *   - 8 bytes if address aligned and remaining size bigger or equal 
 *     to 8 bytes
 *   - less than 8 bytes if address not aligned and/or remaining size
 *     smaller than 8 bytes
 * - send the read command checking if it is for the local memory or the
 *   PCI memory space
 * - update the base address register SAR and the byte count register BCR
 *
 * @param channel the channel performing the read request
 */
template <class PHYSICAL_ADDR,
					bool DEBUG>
void
DMA<PHYSICAL_ADDR, DEBUG> ::
SendRead(unsigned int channel) {
	PHYSICAL_ADDR base_addr; /* !< the base address to read */
	unsigned int size; /* !< the amount of data to read */
	unsigned int max_size; /* !< the maximum data that is needed to read */

	/* get base address to read */
	base_addr = reg.sar[channel];
	
	/* compute the amoung of data to read */
	if(base_addr & (PHYSICAL_ADDR)0x07) {
		/* the address is misaligned, the size to read is the minimum of the
		 * amount necessary to align the address and the remaining size */
		size = 8 - (base_addr & (PHYSICAL_ADDR)0x07);
	} else {
		/* address is aligned, the size to read is the minimum of 8 bytes and
		 * the remaning size */
		size = 8;
	}
	max_size = reg.bcr[channel];
	if(max_size < size) size = max_size;
	
	/* send the read command, checking if it for the local memory or the PCI
	 *   memory space */
	switch((reg.cdar[channel] & Registers::CDAR_CTT) >> 1) {
	case 0:
	case 1:
		/* local memory space read */
		client->DMARead(base_addr, size, channel);
		break;
	case 2:
	case 3:
		/* PCI memory space read */
		client->DMAPCIRead(base_addr, size, channel);
		break;
	}
	
	/* update registers */
	reg.sar[channel] += size;
	reg.bcr[channel] -= size;
}

/**
 * Function call by the mcp107 when a DMA read is received.
 * If channel start bit DMR[CS] is set then:
 * - put data in the buffer
 * - if buffer size bigger or equal than 8 bytes or if there is not more
 *   data to read then send write
 * - if there is still data to read send read, if not then set the busy
 *   bit DSR[CB] to false
 * If channel start bit DMR[CS] is not set then:
 * - put data in the buffer
 * - send read with the data remaining in the buffer
 * - set the busy bit DSR[CB] to false
 *
 * @param channel the channel for which this read was performed
 * @param buffer the data read
 * @param size the amount of data read
 */
template <class PHYSICAL_ADDR,
					bool DEBUG>
void
DMA<PHYSICAL_ADDR, DEBUG> ::
ReadReceived(unsigned int channel,
						 Pointer<uint8_t> &data,
						 unsigned int size) {
	/* if channel start bit DMR[CS] is set then:
	 * - put data in the buffer
	 * - if buffer size bigger or equal than 8 bytes or if there is not more
	 *   data to read then send write
	 * - if there is still data to read send read, if not then set the busy
	 *   bit DSR[CB] to false if the transfer is finished
	 */

	/* check if channel start bit DMR[CS] is set */
	if(reg.dmr[channel] & Registers::DMR_CS) {
		/* fill the buffer with the incomming data */
		buffer[channel].Write(data, size);
		/* send write if there is enough data in the buffer */
		if(buffer[channel].Size() >= 8)
			SendWrite(channel, false);
		/* send write if there is no more data to read and still data in the
		 *   buffer */
		if((reg.bcr[channel] == 0) && (buffer[channel].Size() != 0))
			SendWrite(channel, !buffer[channel].ExpectsData());
		/* check if there is still data to read, and send read if so,
		 *   otherwise set busy bit DSR[CB] to false if the transfer is
		 *   finished */
		if(reg.bcr[channel] != 0)
			SendRead(channel);
		else
			if(buffer[channel].TransferFinished()) 
				reg.dsr[channel] = reg.dsr[channel] & ~(Registers::DSR_CB);
	}
	/* if channel start bit DMR[CS] is not set then:
	 * - put data in the buffer
	 * - send read with the data remaining in the buffer
	 * - set the busy bit DSR[CB] to false if the transfer is finished
	 */
	else { // if(!(dmr[channel] & DMR_CS))
		/* fill the buffer with the incomming data */
		buffer[channel].Write(data, size);
		/* send write if there is data in the buffer */
		if(buffer[channel].Size() != 0)
			SendWrite(channel, !buffer[channel].ExpectsData());
		/* unset the busy bit DSR[CB] if the transfer is finished */
		if(buffer[channel].TransferFinished())
			 reg.dsr[channel] = reg.dsr[channel] & ~(Registers::DSR_CB);
	}	
}

/**
 * Method responsible of sending DMA write request.
 * If it is not the last transfer, then send a write request of 8 bytes
 *   checking if it is for the PCI memory space or the local memory space.
 * If it is the last transfer, then send as many writes as necessary to
 *   empty the buffer checking if it is for the PCI memory space or the
 *   local memory space.
 *
 * @param channel the channel that is performing the write request
 * @param last indicates if the DMA buffer must be emptied in this call
 */
template <class PHYSICAL_ADDR,
					bool DEBUG>
void
DMA<PHYSICAL_ADDR, DEBUG> ::
SendWrite(unsigned int channel, bool last) {
	PHYSICAL_ADDR target_addr; // !< the target address to perform the write
	unsigned int size; // !< the size of the write
	Pointer<uint8_t> data; // !< the data to write
	unsigned int buf_size; // !< the size of the data read from the DMA buffer
	bool pci_write; /* !< indicates if the transfer if for PCI memory space or
									 *    for the local memory space */   

	/* check if the write is for the PCI memory space or for the local
	 *   memory space */
	switch((reg.cdar[channel] & Registers::CDAR_CTT) >> 1) {
	case 0:
	case 2:
		/* local memory space write */
		pci_write = false;
		break;
	case 1:
	case 3:
		/* PCI memory space write */
		pci_write = true;
		break;
	default:
		logger << DebugError << "Internal error" << EndDebugError;
		Object::Stop(-1);
		return;
	}

	/* check if it is the last write request */
	if(last) {
		/* it is the last write request, the buffer must be emptied */
		/* if the target address is not aligned then send a first write with
		 *   less than 8 bytes to align the target address */
		if(reg.dar[channel] & (PHYSICAL_ADDR)0x07) {
			target_addr = reg.dar[channel];
			size = 8 - (target_addr & (PHYSICAL_ADDR)0x07);
			data = new(data) uint8_t[size];
			buf_size = buffer[channel].Read(data, size);
			/* size and tmp_size should be equal */
			if(size != buf_size) {
				if(unlikely(verbose))
					logger << DebugError << LOCATION
						<< "Internal error, wrong number of bytes in the"
						<< " DMA buffer (expected " << size << "bytes"
						<< " and found only " << buf_size << "bytes)"
						<< EndDebugError;
				return;
			}
			/* perform write */
			if(pci_write) 
				client->DMAPCIWrite(target_addr, data, size, channel);
			else
				client->DMAWrite(target_addr, data, size, channel);
			/* update register DAR */
			reg.dar[channel] += size;
		}
		while(!buffer[channel].TransferFinished()) {
			target_addr = reg.dar[channel];
			size = (buffer[channel].Size() > 8)?8:buffer[channel].Size();
			data = new(data) uint8_t[size];
			buf_size = buffer[channel].Read(data, size);
			/* size and tmp_size should be equal */
			if(size != buf_size) {
				if(unlikely(verbose))
					logger << DebugError << LOCATION
						<< "Internal error, wrong number of bytes in the"
						<< " DMA buffer (expected " << size << "bytes"
						<< " and found only " << buf_size << "bytes)"
						<< EndDebugError;
				return;
			}
			/* perform write */
			if(pci_write) 
				client->DMAPCIWrite(target_addr, data, size, channel);
			else
				client->DMAWrite(target_addr, data, size, channel);
			/* update register DAR */
			reg.dar[channel] += size;
		}
	} else {
		/* this is not the last write, check that there is at least 8 bytes in
		 *   in the buffer */
		while(buffer[channel].Size() > 8) {
			target_addr = reg.dar[channel];
			/* perform write to align the following writes */
			size = 8 - (target_addr & (PHYSICAL_ADDR)0x07);
			data = new(data) uint8_t[size];
			buf_size = buffer[channel].Read(data, size);
			/* size and tmp_size should be equal */
			if(size != buf_size) {
				if(unlikely(verbose))
					logger << DebugError << LOCATION
						<< "Internal error, wrong number of bytes in the"
						<< " DMA buffer (expected " << size << "bytes"
						<< " and found only " << buf_size << "bytes)"
						<< EndDebugError;
				return;
			}
			/* perform write */
			if(pci_write) 
				client->DMAPCIWrite(target_addr, data, size, channel);
			else
				client->DMAWrite(target_addr, data, size, channel);
			/* update register DAR */
			reg.dar[channel] += size;
		}
	}
}

/* TODO: implement channel start (called by SetDMR, when
 *   CS bit is set to 1)
 */
template <class PHYSICAL_ADDR,
					bool DEBUG>
void
DMA<PHYSICAL_ADDR, DEBUG> ::
ChannelStart(unsigned int channel) {
	PHYSICAL_ADDR source_address = reg.sar[channel];
	PHYSICAL_ADDR destination_address = reg.dar[channel];
	PHYSICAL_ADDR transfer_size = reg.bcr[channel];

	if(unlikely(verbose))
		logger
			<< DebugError << LOCATION
			<< "Channel start needs to be implemented"
			<< EndDebugError;

	/* set the busy bit DSR[CB] to indicate that a DMA transfer 
	 *   is active */ 
	reg.dsr[channel] = reg.dsr[channel] | Registers::DSR_CB;
	/* initialize the buffer */
	buffer[channel].Initialize(source_address, 
														 destination_address,
														 transfer_size);
	/* check initial and end address */
	SendRead(channel);
}

/* TODO: implement channel stop (called by SetDMR, when
 *   CS bit is set to 0)
 */
template <class PHYSICAL_ADDR,
					bool DEBUG>
void
DMA<PHYSICAL_ADDR, DEBUG> ::
ChannelStop(unsigned int channel) {
	if(unlikely(verbose))
		logger
			<< DebugError << LOCATION
			<< "Channel stop needs to be implemented"
			<< EndDebugError;
}

#undef LOCATION

} // end of namespace dma
} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_DMA_DMA_TCC__

