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

#ifndef __UNISIM_COMPONENT_TLM_BRIDGE_PCI_ISA_BRIDGE_TCC__
#define __UNISIM_COMPONENT_TLM_BRIDGE_PCI_ISA_BRIDGE_TCC__

namespace unisim {
namespace component {
namespace tlm {
namespace bridge {
namespace pci_isa {

using namespace std;

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
Bridge<ADDRESS_TYPE, MAX_DATA_SIZE>::Bridge(const sc_core::sc_module_name& name, Object *parent) :
	Object(name, parent, "PCI-to-ISA bridge"),
	sc_core::sc_module(name),
	unisim::component::cxx::bridge::pci_isa::Bridge<ADDRESS_TYPE>(name, parent),
	pci_slave_port("pci-slave-port"),
	//pci_master_port("pci-master-port"),
	//isa_slave_port("isa-slave-port"),
	isa_master_port("isa-master-port"),
	pci_bus_cycle_time(),
	isa_bus_cycle_time()
{
	SC_HAS_PROCESS(Bridge);
	
	pci_slave_port(*this);
	//isa_slave_port(*this);
	
	SC_THREAD(ISAMaster);
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
Bridge<ADDRESS_TYPE, MAX_DATA_SIZE>::~Bridge()
{
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
bool Bridge<ADDRESS_TYPE, MAX_DATA_SIZE>::Send(const Pointer<TlmMessage<PCIReq, PCIRsp> > &message)
{
	return from_pci.nb_write(message);
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
bool Bridge<ADDRESS_TYPE, MAX_DATA_SIZE>::BeginSetup()
{
	if(!inherited::BeginSetup()) return false;
	pci_bus_cycle_time = sc_core::sc_time(1.0 / (double) (*this)["pci-bus-frequency"], sc_core::SC_US);
	isa_bus_cycle_time = sc_core::sc_time(1.0 / (double) (*this)["isa-bus-frequency"], sc_core::SC_US);
	return true;
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void Bridge<ADDRESS_TYPE, MAX_DATA_SIZE>::ISAMaster()
{
	while(1)
	{
		Pointer<TlmMessage<PCIReq, PCIRsp> > pci_msg = from_pci.read();
		
		const Pointer<PCIRequest<ADDRESS_TYPE, MAX_DATA_SIZE> >& pci_req = pci_msg->GetRequest();
		ADDRESS_TYPE pci_addr = pci_req->addr;
		PCISpace pci_space = pci_req->space;
		unsigned int pci_req_size = pci_req->size;
		TransactionType pci_transaction_type = pci_req->type;
		
		switch(pci_space)
		{
			case unisim::component::cxx::pci::SP_MEM:
			case unisim::component::cxx::pci::SP_IO:
				{
					// PCI-to-ISA
					unisim::component::cxx::isa::ISASpace isa_space;
					unisim::component::cxx::isa::TransactionType isa_transaction_type;
					unisim::component::cxx::isa::isa_address_t isa_addr;
					// Translate PCI address to ISA address
					bool legal_access = inherited::TranslatePCItoISA(pci_space, pci_transaction_type, pci_addr, pci_req_size, isa_space, isa_transaction_type, isa_addr);
					
					if(inherited::verbose)
					{
						inherited::logger << DebugInfo;
						inherited::logger << "Translating PCI-to-ISA request " << (*pci_req) << std::endl;
						inherited::logger << EndDebugInfo;
					}
	
					if(legal_access)
					{
						wait(GetSynchro(isa_bus_cycle_time, sc_core::SC_ZERO_TIME));
	
						Pointer<ISAReq> isa_req = new(isa_req) ISAReq(); // bus transaction request
						Pointer<ISARsp> isa_rsp; // bus transaction response
						sc_core::sc_event isa_rsp_ev; // event notified when bus transaction response is ready
						Pointer<TlmMessage<ISAReq, ISARsp> > isa_msg =
								new(isa_msg) TlmMessage<ISAReq, ISARsp>(isa_req, isa_rsp_ev); // message for bus transaction
	
						isa_req->space = isa_space;
						isa_req->type = isa_transaction_type;
						isa_req->addr = isa_addr; // transaction address
						isa_req->size = pci_req_size; // actual transaction size

						switch(isa_transaction_type)
						{
							case unisim::component::cxx::isa::TT_READ:
								// nothing to do
								break;
							case unisim::component::cxx::isa::TT_WRITE:
								memcpy(isa_req->write_data, pci_req->write_data, pci_req->size);
								break;
						}
	
						if(inherited::verbose)
						{
							inherited::logger << DebugInfo;
							inherited::logger << "Forwarding to ISA " << (*isa_req) << std::endl;
							inherited::logger << EndDebugInfo;
						}

						while(!isa_master_port->Send(isa_msg))
						{
							// if bus transaction request is not accepted then retry later
							wait(isa_bus_cycle_time);
						}
						// bus transaction request has been accepted at this point
						switch(isa_transaction_type)
						{
							case unisim::component::cxx::isa::TT_READ:
								{
									// wait for the bus transaction response
									wait(isa_rsp_ev);

									isa_rsp = isa_msg->GetResponse();
									Pointer<PCIRsp> pci_rsp = new(pci_rsp) PCIRsp();
									pci_msg->rsp = pci_rsp;
									memcpy(pci_rsp->read_data, isa_rsp->read_data, isa_req->size);

									if(inherited::verbose)
									{
										inherited::logger << DebugInfo;
										inherited::logger << "ISA responsed " << (*isa_rsp) << std::endl;
										inherited::logger << EndDebugInfo;
									}
								}
								break;
							case unisim::component::cxx::isa::TT_WRITE:
								// nothing to do
								break;
						}
					}
					else
					{
						if(inherited::verbose)
						{
							inherited::logger << DebugWarning;
							inherited::logger << "Out of range PCI-to-ISA request " << (*pci_req) << std::endl;
							inherited::logger << EndDebugWarning;
						}
					}

					switch(pci_transaction_type)
					{
						case unisim::component::cxx::pci::TT_READ:
							{
								sc_core::sc_event *pci_rsp_ev = pci_msg->GetResponseEvent();
								pci_rsp_ev->notify(GetSynchro(pci_bus_cycle_time, sc_core::SC_ZERO_TIME));
								if(inherited::verbose)
								{
									inherited::logger << DebugInfo << ": Forwarding to PCI " << (*pci_msg->rsp) << std::endl << EndDebugInfo;
								}
							}
							break;
						case unisim::component::cxx::pci::TT_WRITE:
							// nothing to do
							break;
					}
				}
				break;

			case unisim::component::cxx::pci::SP_CONFIG:
				if(((pci_addr >> 11) & 31) != inherited::pci_device_number)
				{
					if(inherited::verbose)
					{
						inherited::logger << DebugWarning;
						inherited::logger << "out of range configuration space access\n" << std::endl;
						inherited::logger << "PCI config base address is 0x" << std::hex << (inherited::pci_device_number << 11) << std::dec << std::endl;
						inherited::logger << "Requested address is 0x" << std::hex << pci_addr << std::dec << std::endl;
						inherited::logger << EndDebugWarning;
					}
				}
				
				switch(pci_transaction_type)
				{
					case unisim::component::cxx::pci::TT_READ:
						{
							Pointer<PCIRsp> pci_rsp = new(pci_rsp) PCIRsp();
						
							unsigned int byte_num, pci_conf_offset;
							for(byte_num = 0, pci_conf_offset = pci_addr & 0xff; byte_num < pci_req_size; byte_num++, pci_conf_offset++)
							{
								pci_rsp->read_data[byte_num] = inherited::ReadConfigByte(pci_conf_offset);
								if(inherited::verbose)
									inherited::logger << DebugInfo << "Reading PCI config at 0x" << std::hex << pci_conf_offset << ": value=0x" << (unsigned int) pci_rsp->read_data[byte_num] << std::dec << std::endl << EndDebugInfo;
							}
						
							pci_msg->rsp = pci_rsp;
							sc_core::sc_event *pci_rsp_ev = pci_msg->GetResponseEvent();
							pci_rsp_ev->notify(pci_bus_cycle_time);
						}
						break;
					case unisim::component::cxx::pci::TT_WRITE:
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
	}
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
sc_core::sc_time Bridge<ADDRESS_TYPE, MAX_DATA_SIZE>::GetSynchro(const sc_core::sc_time& cycle_time, const sc_core::sc_time& process_delay)
{
	sc_dt::uint64 current_time_tu = sc_core::sc_time_stamp().value();
	sc_dt::uint64 process_delay_tu = process_delay.value();
	sc_dt::uint64 next_time_tu = current_time_tu + process_delay_tu;
	sc_dt::uint64 cycle_time_tu = cycle_time.value();
	sc_dt::uint64 time_phase_tu = next_time_tu % cycle_time_tu;
	return sc_core::sc_time(next_time_tu - current_time_tu + (cycle_time_tu - time_phase_tu), false);
}

} // end of namespace pci_isa
} // end of namespace bridge
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_TLM_BRIDGE_PCI_ISA_BRIDGE_TCC__ */
