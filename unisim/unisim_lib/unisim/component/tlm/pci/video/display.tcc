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

#ifndef __UNISIM_COMPONENT_TLM_PCI_VIDEO_DISPLAY_TCC__
#define __UNISIM_COMPONENT_TLM_PCI_VIDEO_DISPLAY_TCC__

namespace unisim {
namespace component {
namespace tlm {
namespace pci {
namespace video {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
Display<ADDRESS_TYPE, MAX_DATA_SIZE>::Display(const sc_core::sc_module_name& name, Object *parent) :
	Object(name, parent, "PCI Video frame buffer display"),
	sc_core::sc_module(name),
	unisim::component::cxx::pci::video::Display<ADDRESS_TYPE>(name, parent),
	bus_port("bus_port"),
	pci_bus_cycle_time()
{
	bus_port(*this);
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
Display<ADDRESS_TYPE, MAX_DATA_SIZE>::~Display()
{
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
bool Display<ADDRESS_TYPE, MAX_DATA_SIZE>::Send(const Pointer<TlmMessage<PCIReq, PCIRsp> > &message)
{
	const Pointer<PCIRequest<ADDRESS_TYPE, MAX_DATA_SIZE> >& pci_req = message->GetRequest();
	ADDRESS_TYPE pci_addr = pci_req->addr;
	PCISpace pci_space = pci_req->space;
	unsigned int pci_req_size = pci_req->size;
	TransactionType pci_transaction_type = pci_req->type;
	
	switch(pci_space)
	{
		case SP_MEM:
			switch(pci_transaction_type)
			{
				case TT_READ:
				{
					Pointer<PCIRsp> pci_rsp = new(pci_rsp) PCIRsp();
				
					inherited::Read(pci_addr, pci_rsp->read_data, pci_req_size);
				
					message->rsp = pci_rsp;
					sc_core::sc_event *rsp_ev = message->GetResponseEvent();
					rsp_ev->notify(pci_bus_cycle_time);
				}
				break;
				case TT_WRITE:
					inherited::Write(pci_addr, pci_req->write_data, pci_req_size);
					break;
			}
			break;
		case SP_IO:
			if(inherited::verbose)
				inherited::logger << DebugWarning << "I don't have I/O space" << std::endl << EndDebugWarning;
			break;
		case SP_CONFIG:
			if(((pci_addr >> 11) & 31) != inherited::pci_device_number)
			{
				if(inherited::verbose)
				{
					inherited::logger << DebugWarning;
					inherited::logger << "out of range configuration space access\n" << std::endl;
					inherited::logger << "PCI config base address is 0x" << std::hex << inherited::pci_conf_base_addr << std::dec << std::endl;
					inherited::logger << "Requested address is 0x" << std::hex << pci_addr << std::dec << std::endl;
					inherited::logger << EndDebugWarning;
				}
			}
			
			switch(pci_transaction_type)
			{
				case TT_READ:
					{
						Pointer<PCIRsp> pci_rsp = new(pci_rsp) PCIRsp();
					
						unsigned int byte_num, pci_conf_offset;
						for(byte_num = 0, pci_conf_offset = pci_addr & 0xff; byte_num < pci_req_size; byte_num++, pci_conf_offset++)
						{
							pci_rsp->read_data[byte_num] = inherited::ReadConfigByte(pci_conf_offset);
							if(inherited::verbose)
								inherited::logger << DebugInfo << "Reading PCI config at 0x" << std::hex << pci_conf_offset << ": value=0x" << (unsigned int) pci_rsp->read_data[byte_num] << std::dec << std::endl << EndDebugInfo;
						}
					
						message->rsp = pci_rsp;
						sc_core::sc_event *rsp_ev = message->GetResponseEvent();
						rsp_ev->notify(pci_bus_cycle_time);
					}
					break;
				case TT_WRITE:
					{
						unsigned int byte_num, pci_conf_offset;
						unsigned int pci_req_size = pci_req->size;
						for(byte_num = 0, pci_conf_offset = pci_addr & 0xff; byte_num < pci_req_size; byte_num++, pci_conf_offset++)
						{
							if(inherited::verbose)
								inherited::logger << DebugInfo << "Writing PCI config at 0x" << std::hex << pci_conf_offset << ": value=0x" << (unsigned int) pci_req->write_data[byte_num] << std::dec << std::endl << EndDebugInfo;
							inherited::WriteConfigByte(pci_conf_offset, pci_req->write_data[byte_num]);
						}
					}
					break;
			}
			break;
	}
		
	return true;
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
bool Display<ADDRESS_TYPE, MAX_DATA_SIZE>::EndSetup()
{
	if(!inherited::EndSetup()) return false;
	pci_bus_cycle_time = sc_core::sc_time(1.0 / (double) (*this)["pci-bus-frequency"], sc_core::SC_US);
	return true;
}

} // end of namespace video
} // end of namespace pci
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_TLM_PCI_VIDEO_DISPLAY_TCC__ */
