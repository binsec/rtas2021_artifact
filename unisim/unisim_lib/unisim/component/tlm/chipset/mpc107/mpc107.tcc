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

#ifndef __UNISIM_COMPONENT_TLM_CHIPSET_MPC107_MPC107_TCC__
#define __UNISIM_COMPONENT_TLM_CHIPSET_MPC107_MPC107_TCC__

#include <sstream>

namespace unisim {
namespace component {
namespace tlm {
namespace chipset {
namespace mpc107 {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::Object;

using std::stringstream;

#define LOCATION	"In file " << __FILE__ << ", function " << __FUNCTION__ << ", line #" << __LINE__

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
MPC107(const sc_core::sc_module_name &name, Object *parent) :
	Object(name, parent, "MPC107 chipset"),
	sc_core::sc_module(name),
	Service<Memory<PHYSICAL_ADDR> >(name, parent),
	Client<Memory<PHYSICAL_ADDR> >(name, parent),
	pci_memory_client("pci-memory-client", this),
	slave_port("slave_port"),
	master_port("master_port"),
	pci_slave_port("pci_slave_port"),
	ram_master_port("ram_master_port"),
	rom_master_port("rom_master_port"),
	erom_master_port("erom_master_port"),
	pci_master_port("pci_master_port"),
	irq_master_port("irq_master_port"),
	soft_reset_master_port("soft_reset_master_port"),
	memory_export("memory_export", this),
	ram_import("ram_import", this),
	rom_import("rom_import", this),
	erom_import("erom_import", this),
	pci_import("pci_import", &pci_memory_client),
	logger(*this),
	verbose(false),
	param_verbose("verbose", this, verbose, "enable/disable verbosity"),
	dma(this, "DMA", this),
	atu("atu", this),
	epic("epic", this),
	epic_master_port("epic_master_port"),
	epic_memory_import("epic_memory_import", this),
	sdram_master_port("sdram_master_port"),
	sdram_to_epic_sig("sdram_to_epic_sig"),
	config_regs(),
	pci_controller(0, config_regs, addr_map, "pci_controller", this),
	config_addr(0),
	addr_map(config_regs, atu, "address_mapper", this),
	host_mode(true),
	a_address_map(false),
	memory_32bit_data_bus_size(true),
	rom0_8bit_data_bus_size(false),
	rom1_8bit_data_bus_size(false),
	frequency(0),
	sdram_cycle_time(0),
	param_host_mode("host_mode", this, host_mode, "enable/disable host mode"),
	param_a_address_map("a_address_map", this, a_address_map, "enable/disable address map A"),
	param_memory_32bit_data_bus_size("memory_32bit_data_bus_size", this, memory_32bit_data_bus_size, "enable/disable 32-bit data bus width"),
	param_rom0_8bit_data_bus_size("rom0_8bit_data_bus_size", this, rom0_8bit_data_bus_size, "enable/disable rom #0 8-bit data bus width"),
	param_rom1_8bit_data_bus_size("rom1_8bit_data_bus_size", this, rom1_8bit_data_bus_size, "enable/disable rom #1 8-bit data bus width"),
	param_frequency("frequency", this, frequency, "frequency in Mhz"),
	param_sdram_cycle_time("sdram_cycle_time", this, sdram_cycle_time, "SDRAM cycle time in picoseconds")
{
	param_frequency.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_sdram_cycle_time.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	
	slave_port(*this);
	pci_slave_port(*this);
	
	/* connect the irq input ports to the epic (interrupt controller) */
	for(unsigned int i = 0; 
		i < unisim::component::tlm::chipset::mpc107::epic::EPIC<PHYSICAL_ADDR, DEBUG>::NUM_IRQS; 
		i++) {
		stringstream irq_slave_port_name;
		
		irq_slave_port_name << "irq_slave_port[" << i << "]";
		irq_slave_port[i] = new sc_core::sc_export<TlmSendIf<IRQType> >(irq_slave_port_name.str().c_str());
		(*irq_slave_port[i])(*epic.irq_slave_port[i]);
	}
	
	/* connect the irq_master_port and soft_reset_master_port of the epic to the 
	 * irq_master_port and soft_reset_master_port respectively of the mpc107 */
	epic.irq_master_port(irq_master_port);
	epic.soft_reset_master_port(soft_reset_master_port);
	/* connect the mpc107 to the epic controller with the epic_master_port to send
	 *   read/write commands */
	epic_master_port(epic.slave_port);
	/* connect the sdram_master_port port to the epic sdram_inport port */
	sdram_master_port(sdram_to_epic_sig);
	epic.sdram_slave_port(sdram_to_epic_sig);
	
	epic_memory_import >> epic.memory_export;
	
	SC_THREAD(PCIDispatch);
	SC_THREAD(DispatchPCIReq);
	SC_THREAD(DispatchDMALocalMemoryAccess);
	SC_THREAD(DispatchDMAPCIAccess);
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
~MPC107() {
	
}
		
template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
BeginSetup() {
	/* IMPORTANT: before initializing the different components the configuration
	 *   registers must be set */
	if(!config_regs.Reset(host_mode, 
		a_address_map,
		memory_32bit_data_bus_size,
		rom0_8bit_data_bus_size, 
		rom1_8bit_data_bus_size)) {
		logger << DebugError
			<< LOCATION
			<< "Error while initializing the configuration registers"
			<< std::endl << EndDebugError;
		return false;
	}

	/* initialize the address map */
	if(!addr_map.Reset()) {
		logger << DebugError
			<< LOCATION
			<< "Error while initializing the address map"
			<< std::endl << EndDebugError;
		return false;
	}
	
	if(!host_mode) {
		logger << DebugError 
			<< LOCATION << "SERVICE_SETUP_ERROR: Only host mode is supported (sorry)" 
			<< std::endl << EndDebugError;
		return false; // only host mode supported
	}

	if(!frequency) {
		logger << DebugError 
			<< LOCATION 
			<< "SERVICE_SETUP_ERROR: frequency parameter has not been set" 
			<< std::endl << EndDebugError;
		return false;
	}
	cycle_time = sc_core::sc_time(1.0 / (double) frequency, sc_core::SC_US);
	if(!sdram_cycle_time) {
		logger << DebugError 
			<< LOCATION
			<< "SERVICE_SETUP_ERROR: sdram_cycle_time parameter has not been set" 
			<< std::endl << EndDebugError;
		return false;
	}
	/* set the initial sdram_frequency */
	sdram_master_port = sdram_cycle_time;

	return true;
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
SetupMemory()
{
	if(!rom_import) {
		logger << DebugError  << LOCATION
			<< "SERVICE_SETUP_ERROR: rom_import not connected" 
			<< std::endl << EndDebugError;
		return false;
	}
	if(!ram_import) {
		logger << DebugError << LOCATION 
			<< "SERVICE_SETUP_ERROR: ram_import not connected" 
			<< std::endl << EndDebugError;
		return false;
	}
	if(!erom_import) {
		logger << DebugError << LOCATION 
			<< "SERVICE_SETUP_ERROR: erom_import not connected" 
			<< std::endl << EndDebugError;
		return false;
	}
	if(!pci_import) {
		logger << DebugError << LOCATION 
			<< "SERVICE_SETUP_ERROR: pci_import not connected" 
			<< std::endl << EndDebugError;
		return false;
	}

	return true;
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
Setup(ServiceExportBase *srv_export) {
	if(srv_export == &memory_export) return SetupMemory();

	logger << DebugError << "Internal error" << EndDebugError;
	return false;
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DEBUG_PCI_REQ(const PPCIReqType &req) {
	logger << "(addr = 0x" << std::hex << req->addr << std::dec << ", " 
		<< "type = " << ((req->type == unisim::component::cxx::pci::TT_READ) ? "READ" : "WRITE") << ", " 
		<< "space = " << ((req->space == unisim::component::cxx::pci::SP_MEM) ? "MEMORY" : 
							(req->space == unisim::component::cxx::pci::SP_IO) ? "I/O" : "CONFIG") << ", " 
		<< "size = " << req->size;
	if(req->type == unisim::component::cxx::pci::TT_WRITE) {
		logger << ", data = 0x";
		for(unsigned int i = 0; i < req->size - 1; i++)
			logger << std::hex << (unsigned int)req->write_data[i] << " ";
		logger << (unsigned int)req->write_data[req->size - 1] << std::dec; 
	} 
	logger << ")";
}
		
template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
Send(const PPCIMsgType &pci_message) {
	const PPCIReqType& pci_req = pci_message->GetRequest();

	/* check if the chipset can response to pci memory space requests */
	/*if((config_regs.pci_command_reg.value & (1 << 1)) == 0) {
		if(unlikely(DEBUG && verbose)) {
			logger << DebugWarning
				<< LOCATION
				<< "Received pci request but the chipset is configured not to respond to pci memory space requests";
			DEBUG_PCI_REQ(pci_req);
			logger << "THE REQUEST IS ACCEPTED BUT A REPLY WILL NOT BE SENT" << std::endl << EndDebugWarning;
		}
		return true;
	}*/
	
	AddressMapEntry *entry = addr_map.GetEntryProc(pci_req->addr);
	if(entry == NULL) {
		logger << DebugError
			<< LOCATION
			<< "Could not get an address map entry for address 0x"
			<< std::hex << pci_req->addr << std::dec << " for a request comming from the pci bus"
			<< std::endl
			<< EndDebugError;
		Object::Stop(-1);
	}
	switch(entry->type) {
	case AddressMapEntry::LOCAL_MEMORY_SPACE:
		return SendPCItoMemory(pci_message, ram_master_port);
		break;
	case AddressMapEntry::RESERVED:
		logger << DebugError 
			<< LOCATION
			<< " Send() from PCI bus accessing a reserved entry address ";
		DEBUG_PCI_REQ(pci_req);
		logger << std::endl << EndDebugError;
		Object::Stop(-1);
		return false; // never executed
		break;
	case AddressMapEntry::EXTENDED_ROM_1:
		return SendPCItoMemory(pci_message, erom_master_port);
		break;
	case AddressMapEntry::EXTENDED_ROM_2:
		return SendPCItoMemory(pci_message, erom_master_port);
		break;
	case AddressMapEntry::PCI_MEMORY_SPACE:
		/* The request should be simply ignored,
		 * 	 even better it should be never received */
		logger << DebugError
			<< LOCATION
			<< " Send() from PCI bus to PCI memory space ";
		DEBUG_PCI_REQ(pci_req);
		logger << std::endl
			<< "This should never happen!!!!" << std::endl 
			<< EndDebugError;
		Object::Stop(-1);
		return false; // never executed
		break;
	case AddressMapEntry::ROM_SPACE_1:
		return SendPCItoMemory(pci_message, rom_master_port);
		break;
	case AddressMapEntry::ROM_SPACE_2:
		return SendPCItoMemory(pci_message, rom_master_port);
		break;
	default:
		logger << DebugError
			<< LOCATION
			<< " Send() received a request to the pci memory space that is not a "
			<< "read neither a write ";
		DEBUG_PCI_REQ(pci_req);
		logger << std::endl << EndDebugError;
		Object::Stop(-1);
		return false; // never executed
		break;
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DEBUG_BUS_REQ(const PReqType &req) {
	logger << "(addr = 0x" << std::hex << req->addr << std::dec
		<< ", type = ";
	switch(req->type) {
	case ReqType::READ:
		logger << "READ";
		break;
	case ReqType::READX:
		logger << "READX";
		break;
	case ReqType::WRITE:
		logger << "WRITE";
		break;
	case ReqType::INV_BLOCK:
		logger << "INV_BLOCK";
		break;
	case ReqType::FLUSH_BLOCK:
		logger << "FLUSH_BLOCK";
		break;
	case ReqType::ZERO_BLOCK:
		logger << "ZERO_BLOCK";
		break;
	case ReqType::INV_TLB:
		logger << "INV_TLB";
		break;
	}
	logger << ", global = " << (req->global ? "TRUE" : "FALSE")
		<< ", size = " << req->size;
	if(req->type == ReqType::WRITE) {
		logger << ", data = 0x" << std::hex;
		for(unsigned int i = 0; i < req->size - 1; i++) 
			logger << (unsigned int)req->write_data[i] << " ";
		logger << (unsigned int)req->write_data[req->size - 1];
	}
	logger << ")";
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
Send(const PMsgType &message) {
	const PReqType& req = message->GetRequest();
	PPCIReqType pci_req = new(pci_req) PCIReqType();
	PPCIMsgType pci_msg = new(pci_msg) PCIMsgType(pci_req);

	AddressMapEntry *entry = addr_map.GetEntryProc(req->addr);
	if(entry == NULL) {
		logger << DebugError
			<< LOCATION
			<< "Could not get an address map entry for address 0x"
			<< std::hex << req->addr << std::dec << " for a request comming from the system bus"
			<< std::endl
			<< EndDebugError;
		Object::Stop(-1);
		return false;
	}
	switch(entry->type) {
	case AddressMapEntry::LOCAL_MEMORY_SPACE:
		return SendFSBtoMemory(message, ram_master_port);
		break;
	case AddressMapEntry::RESERVED:
		logger << DebugError
			<< LOCATION
			<< "Send() from system bus accessing a reserved entry address ";
		DEBUG_BUS_REQ(req);
		logger << std::endl << EndDebugInfo;
		Object::Stop(-1);
		return false;
		break;
	case AddressMapEntry::EXTENDED_ROM_1:
		return SendFSBtoMemory(message, erom_master_port);
		break;
	case AddressMapEntry::EXTENDED_ROM_2:
		return SendFSBtoMemory(message, erom_master_port);
		break;
	case AddressMapEntry::PCI_MEMORY_SPACE:
		/* create a new pci message */
		switch(req->type) {
		case ReqType::READ:
		case ReqType::READX:
			if(!pci_controller.GetReadRequest(req->addr, req->size,
							pci_req->type,
							pci_req->addr,
							pci_req->space,
							pci_req->size) ) {
				logger << DebugError
					<< sc_core::sc_time_stamp().to_string()
					<< "Send() from system bus received a bad read request from the system bus ";
				DEBUG_BUS_REQ(req);
				logger << std::endl << EndDebugError;
				Object::Stop(-1);
			}
			if(!message->HasResponseEvent()) {
				logger << DebugError
					<< sc_core::sc_time_stamp().to_string()
					<< LOCATION
					<< "Send() from system bus received a read request without response event "; 
				DEBUG_BUS_REQ(req);
				logger << std::endl << EndDebugError;
				Object::Stop(-1);
			}
			break;
		case ReqType::WRITE:
			if(!pci_controller.GetWriteRequest(req->addr, req->write_data, req->size,
								pci_req->type,
								pci_req->addr,
								pci_req->space,
								pci_req->write_data,
								pci_req->size) ) {
				logger << DebugError
					<< LOCATION
					<< "Send() from system bus received a bad write request from the system bus ";
				DEBUG_BUS_REQ(req);
				logger << std::endl << EndDebugError;
				Object::Stop(-1);
			}
			break;
		default:
			logger << DebugError
				<< LOCATION
				<< "Send() from system bus received a request to the pci memory space that is not a "
				<< "read neither a write "; 
			DEBUG_BUS_REQ(req);
			logger << std::endl << EndDebugError;
			Object::Stop(-1);
			return false;
			break;
		}
		/* put the message in the in pci request list */
		req_list.push_back(message);
		bus_pci_req_list.push_back(pci_msg);
		if(bus_pci_req_list.size() == 1) {
			pci_dispatch_event.notify(sc_core::SC_ZERO_TIME);
		}
		return true;
		break;
	case AddressMapEntry::PCI_IO_SPACE_1:
		switch(req->type) {
		case ReqType::READ:
		case ReqType::READX:
			if(!pci_controller.GetIOReadRequest(req->addr, req->size,
							pci_req->type,
							pci_req->addr,
							pci_req->space,
							pci_req->size) ) {
				logger << DebugError
					<< sc_core::sc_time_stamp().to_string()
					<< LOCATION
					<< "Send() from system bus received a bad PCI I/O read request from the system bus ";
				DEBUG_BUS_REQ(req);
				logger << std::endl << EndDebugError;
				Object::Stop(-1);
			}
			if(!message->HasResponseEvent()) {
				logger << DebugError
					<< LOCATION
					<< "Send() from system bus received a PCI I/O read request without response event "; 
				DEBUG_BUS_REQ(req);
				logger << std::endl << EndDebugError;
				Object::Stop(-1);
			}			
			break;
		case ReqType::WRITE:
			if(!pci_controller.GetIOWriteRequest(req->addr, req->write_data, req->size,
								pci_req->type,
								pci_req->addr,
								pci_req->space,
								pci_req->write_data,
								pci_req->size) ) {
				logger << DebugError
					<< LOCATION
					<< "Send() from system bus received a bad PCI I/O write request from the system bus ";
				DEBUG_BUS_REQ(req);
				logger << std::endl << EndDebugError;
				Object::Stop(-1);
			}
			break;
		default:
			logger << DebugError
				<< LOCATION
				<< "Send() from system bus received a request to the PCI I/O space that is not a "
				<< "read neither a write request ";
			DEBUG_BUS_REQ(req);
			logger << std::endl << EndDebugError;
			Object::Stop(-1);
			return false;
			break;
		}
		/* put the message in the in pci request list */
		req_list.push_back(message);
		bus_pci_req_list.push_back(pci_msg);
		if(bus_pci_req_list.size() == 1)
			pci_dispatch_event.notify(sc_core::SC_ZERO_TIME);
		return true;
		break;
	case AddressMapEntry::PCI_IO_SPACE_2:
		if(unlikely(DEBUG && verbose)) {
			logger << DebugWarning
				<< LOCATION
				<< " accessing the PCI I/O space 1 ";
			DEBUG_BUS_REQ(req);
			logger << std::endl << EndDebugWarning;
		}
		return false;
		break;
	case AddressMapEntry::PCI_CONFIG_ADDRESS_REG:
		switch(req->type) {
		case ReqType::READ:
		case ReqType::READX: 
			{
				PRspType rsp = new(rsp) RspType();
				if(unlikely(verbose)) {
					logger << DebugWarning
						<< LOCATION
						<< "making a read to the pci config address register, "
						<< "I am returning 0, but maybe something else should be done "; 
					DEBUG_BUS_REQ(req);
					logger << std::endl << EndDebugWarning;
				}
				if(!message->HasResponseEvent()) {
					logger << DebugError
						<< LOCATION
						<< " received a read request without response event "; 
					DEBUG_BUS_REQ(req);
					logger << std::endl << EndDebugError;
					Object::Stop(-1);
				}
				memset(rsp->read_data, 0, MAX_TRANSACTION_DATA_SIZE);
				rsp->read_status = RspType::RS_MISS;
				message->rsp = rsp;
				message->GetResponseEvent()->notify(cycle_time);
			}	
			break;
		case ReqType::WRITE:
			if(!pci_controller.SetConfigAddr(req->addr,
								req->write_data,
								req->size)) {
				logger << DebugError
					<< LOCATION
					<< "bad write access to the configuration address register memory space ";
				DEBUG_BUS_REQ(req);
				logger << std::endl << EndDebugError;
				Object::Stop(-1);
			}
			/* nothing else to do */
			break;
		default:
			logger << DebugError
				<< LOCATION
				<< "Send() from system bus received a request to the pci config address register space that is not a "
				<< "read neither a write request ";
			DEBUG_BUS_REQ(req);
			logger << std::endl << EndDebugError;
			Object::Stop(-1);
			return false;
			break;
		}
		return true;
		break;
	case AddressMapEntry::PCI_CONFIG_DATA_REG:
		switch(req->type) {
		case ReqType::READ:
		case ReqType::READX:
			if(pci_controller.IsLocalAccess()) {
				PRspType rsp = new(rsp) RspType();
				if(!pci_controller.LocalReadConfigRequest(req->addr, req->size,
									rsp->read_data)) {
					logger << DebugError
						<< LOCATION
						<< "Send() from system bus received a bad read for the pci configuration data register ";
					DEBUG_BUS_REQ(req);
					logger << std::endl << EndDebugError;
					Object::Stop(-1);
				}
				message->rsp = rsp;
				message->GetResponseEvent()->notify(cycle_time);
				return true;			
			} else {
				if(!pci_controller.GetReadConfigRequest(req->addr, req->size,
									pci_req->type,
									pci_req->addr,
									pci_req->space,
									pci_req->size) ) {
					logger << DebugError
						<< LOCATION
						<< "Send() from system bus received a bad read for the PCI configuration data register ";
					DEBUG_BUS_REQ(req);
					logger << std::endl << EndDebugError;
					Object::Stop(-1);
				}
			}
			break;
		case ReqType::WRITE:
			if(pci_controller.IsLocalAccess()) {
				if(!pci_controller.LocalWriteConfigRequest(req->addr, req->write_data, req->size)) {
					logger << DebugError
						<< LOCATION
						<< "Send() from system bus received a bad write for the pci configuration data register ";
					DEBUG_BUS_REQ(req);
					logger << std::endl << EndDebugError;
					Object::Stop(-1);
				}
				return true;			
			} else {
				if(!pci_controller.GetWriteConfigRequest(req->addr, req->write_data, req->size,
									pci_req->type,
									pci_req->addr,
									pci_req->space,
									pci_req->write_data,
									pci_req->size) ) {
					logger << DebugError
						<< LOCATION
						<< "Send() from system bus received a bad write request "; 
					DEBUG_BUS_REQ(req);
					logger << std::endl << EndDebugError;
					Object::Stop(-1);
				}
			}
			break;
		default:
			logger << DebugError
				<< "Send() from system bus received a request to the pci config data register space that is not a "
				<< "read neither a write "; 
			DEBUG_BUS_REQ(req);
			logger << std::endl << EndDebugError;
			Object::Stop(-1);
			return false;
			break;
		}
		/* put the message in the in pci request list */
		req_list.push_back(message);
		bus_pci_req_list.push_back(pci_msg);
		if(bus_pci_req_list.size() == 1) {
			pci_dispatch_event.notify(sc_core::SC_ZERO_TIME);
		}
		return true;
		break;
	case AddressMapEntry::PCI_INT_ACK:
		logger << DebugError
			<< LOCATION
			<< "Accessing the pci interruption acknowledgement ";
		DEBUG_BUS_REQ(req);
		logger << std::endl << EndDebugError;
		Object::Stop(-1);
		return false;
		break;
	case AddressMapEntry::ROM_SPACE_1:
		return SendFSBtoMemory(message, rom_master_port);
		break;
	case AddressMapEntry::ROM_SPACE_2:
		return SendFSBtoMemory(message, rom_master_port);
		break;
	case AddressMapEntry::EUMB_EPIC_SPACE:
		return SendFSBtoMemory(message, epic_master_port);
		break;
	case AddressMapEntry::EUMB_ATU_SPACE:
		return SendFSBtoATU(message);
		break;
	case AddressMapEntry::EUMB_MSG_UNIT_SPACE:
	case AddressMapEntry::EUMB_DMA_SPACE:
	case AddressMapEntry::EUMB_I2C_SPACE:
	case AddressMapEntry::EUMB_DATA_PATH_DIAGNOSTICS_SPACE:
		logger << DebugError
			<< LOCATION
			<< "TODO Embedded utilities block mapping not implemented ";
		DEBUG_BUS_REQ(req);
		logger << std::endl << EndDebugError;
		Object::Stop(-1);
		break;
	default:
		logger << DebugError
			<< LOCATION
			<< "Accessing an unmapped (or unhalded) memory space ";
		DEBUG_BUS_REQ(req);
		logger << std::endl << EndDebugError;
		Object::Stop(-1);
		break;
	}
	return false;
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
Reset() {
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
ResetMemory() {
	/* forward the reset call */
	if(ram_import) ram_import->ResetMemory();
	if(rom_import) rom_import->ResetMemory();
	if(erom_import) erom_import->ResetMemory();
	if(pci_import) pci_import->ResetMemory();
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
ReadMemory(PHYSICAL_ADDR addr, void *buffer, uint32_t size) {
	PCI_ADDR pci_addr;
	
	AddressMapEntry *entry = addr_map.GetEntryProc(addr);
	if(entry == NULL) {
		if(unlikely(verbose)) {
			logger << DebugInfo
				<< LOCATION
				<< "Could not get an address map entry for address 0x"
				<< std::hex << addr << std::dec << " for a request comming from the system bus"
				<< std::endl
				<< EndDebugInfo;
		}
		return false;
	}
	switch(entry->type) {
	case AddressMapEntry::LOCAL_MEMORY_SPACE:
		if(unlikely(DEBUG && verbose))
			logger << DebugInfo
				<< LOCATION
				<< "ReadMemory() to extended ram (address = " 
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugInfo;
		return ram_import->ReadMemory(addr, buffer, size);
		break;
	case AddressMapEntry::RESERVED:
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning
				<< LOCATION
				<< " accessing a reserved entry address (address = " 
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::EXTENDED_ROM_1:
		if(unlikely(DEBUG && verbose))
			logger << DebugInfo
				<< LOCATION
				<< "ReadMemory() to extended rom 1 (address = " 
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugInfo;
		return erom_import->ReadMemory(addr, buffer, size);
		break;
	case AddressMapEntry::EXTENDED_ROM_2:
		if(unlikely(DEBUG && verbose))
			logger << DebugInfo
				<< LOCATION
				<< "ReadMemory() to extended rom 2 (address = " 
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugInfo;
		return erom_import->ReadMemory(addr, buffer, size);
		break;
	case AddressMapEntry::PCI_MEMORY_SPACE:
		if(unlikely(DEBUG && verbose))
			logger << DebugInfo << LOCATION
				<< "ReadMemory() to the PCI memory space (address = "
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugInfo;
		pci_controller.TranslateAddressSystemToPCIMem(addr, pci_addr);
		return pci_import->ReadMemory(pci_addr, buffer, size);
		break;
	case AddressMapEntry::PCI_IO_SPACE_1:
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning
				<< LOCATION
				<< " accessing the PCI IO space 1 (address = " 
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_IO_SPACE_2:
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning
				<< LOCATION
				<< " accessing the PCI IO space 2 (address = " 
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_CONFIG_ADDRESS_REG:
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning
				<< LOCATION
				<< " accessing the PCI Config Address Reg (address = " 
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_CONFIG_DATA_REG:
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning
				<< LOCATION
				<< " accessing the PCI Config Data Reg (address = " 
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_INT_ACK:
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning
				<< LOCATION
				<< " accessing the pci interruption acknowledgement (address = " 
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::ROM_SPACE_1:
		if(unlikely(DEBUG && verbose))
			logger << DebugInfo
				<< LOCATION
				<< "ReadMemory() to rom 1 (address = " << std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugInfo;
		return rom_import->ReadMemory(addr, buffer, size);
		break;
	case AddressMapEntry::ROM_SPACE_2:
		if(unlikely(DEBUG && verbose))
			logger << DebugInfo
				<< LOCATION
				<< "ReadMemory() to rom 2 (address = " << std::hex << addr << std::dec << ")"<< std::endl
				<< EndDebugInfo;
		return rom_import->ReadMemory(addr, buffer, size);
		break;
	case AddressMapEntry::EUMB_EPIC_SPACE:
		if(unlikely(DEBUG && verbose))
			logger << DebugInfo
				<< LOCATION
				<< "ReadMemory() to EPIC (address = " << std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugInfo;
		return epic_memory_import->ReadMemory(addr, buffer, size);
		break;
	case AddressMapEntry::EUMB_ATU_SPACE:
		return ReadMemorytoATU(addr, buffer, size);
		break;
	case AddressMapEntry::EUMB_MSG_UNIT_SPACE:
	case AddressMapEntry::EUMB_DMA_SPACE:
	case AddressMapEntry::EUMB_I2C_SPACE:
	case AddressMapEntry::EUMB_DATA_PATH_DIAGNOSTICS_SPACE:
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning
				<< LOCATION
				<< " Embedded utilities block mapping not fully implemented" << std::endl
				<< EndDebugWarning;
		return false;
		break;
	default:
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning
				<< LOCATION
				<< " accessing an unmapped (or unhalded) memory space (address = "
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugWarning;
		return false;
		break;
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
WriteMemory(PHYSICAL_ADDR addr, const void *buffer, uint32_t size) {
	PCI_ADDR pci_addr;

	AddressMapEntry *entry = addr_map.GetEntryProc(addr);
	if(entry == NULL) {
		if(unlikely(verbose)) {
			logger << DebugError
				<< LOCATION
				<< "Could not get an address map entry for address 0x"
				<< std::hex << addr << std::dec << " for a request comming from the system bus"
				<< std::endl
				<< EndDebugError;
		}
		return false;
	}
	switch(entry->type) {
	case AddressMapEntry::LOCAL_MEMORY_SPACE:
		if(unlikely(DEBUG && verbose))
			logger << DebugInfo
				<< "Write to ram (addr = 0x" << std::hex << addr << std::dec
				<< ", size = " << size << ")" << std::endl
				<< EndDebugInfo;
		return ram_import->WriteMemory(addr, buffer, size);
		break;
	case AddressMapEntry::RESERVED:
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning
				<< LOCATION
				<< " accessing a reserved entry address" << std::endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::EXTENDED_ROM_1:
		if(unlikely(DEBUG && verbose))
			logger << DebugInfo
				<< "WriteMemory() to extended rom 1" << std::endl
				<< EndDebugInfo;
		return erom_import->WriteMemory(addr, buffer, size);
		break;
	case AddressMapEntry::EXTENDED_ROM_2:
		if(unlikely(DEBUG && verbose))
			logger << DebugInfo
				<< "WriteMemory() to extended rom 2" << std::endl
				<< EndDebugInfo;
		return erom_import->WriteMemory(addr, buffer, size);
		break;
	case AddressMapEntry::PCI_MEMORY_SPACE:
		if(unlikely(DEBUG && verbose))
			logger << DebugInfo << LOCATION
				<< "WriteMemory() to the PCI memory space (address = "
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugInfo;
		pci_controller.TranslateAddressSystemToPCIMem(addr, pci_addr);
		return pci_import->WriteMemory(pci_addr, buffer, size);
		break;
	case AddressMapEntry::PCI_IO_SPACE_1:
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning
				<< LOCATION
				<< " accessing the PCI IO space 1 (address = " 
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_IO_SPACE_2:
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning
				<< LOCATION
				<< " accessing the PCI IO space 2 (address = " 
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_CONFIG_ADDRESS_REG:
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning
				<< LOCATION
				<< " accessing the PCI Config Address Reg (address = " 
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_CONFIG_DATA_REG:
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning
				<< LOCATION
				<< " accessing the PCI Config Data Reg (address = " 
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_INT_ACK:
		if(unlikely(DEBUG && verbose))
			logger << DebugWarning
				<< LOCATION
				<< " accessing the pci interruption acknowledgement (address = " 
				<< std::hex << addr << std::dec << ")" << std::endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::ROM_SPACE_1:
		if(unlikely(DEBUG && verbose))
			logger << DebugInfo
				<< "WriteMemory() to rom 1" << std::endl
				<< EndDebugInfo;
		return rom_import->WriteMemory(addr, buffer, size);
		break;
	case AddressMapEntry::ROM_SPACE_2:
		if(unlikely(DEBUG && verbose))
			logger << DebugInfo
				<< "WriteMemory() to rom 2" << std::endl
				<< EndDebugInfo;
		return rom_import->WriteMemory(addr, buffer, size);
		break;
	case AddressMapEntry::EUMB_EPIC_SPACE:
		if(unlikely(DEBUG && verbose))
			logger << DebugInfo << LOCATION
				<< "WriteMemory() to EPIC (address = 0x" << std::hex << addr << std::dec << ")"
				<< std::endl << EndDebugInfo;
		return epic_memory_import->WriteMemory(addr, buffer, size);
		break;
	case AddressMapEntry::EUMB_ATU_SPACE:
		return WriteMemorytoATU(addr, buffer, size);
		break;
	case AddressMapEntry::EUMB_MSG_UNIT_SPACE:
	case AddressMapEntry::EUMB_DMA_SPACE:
	case AddressMapEntry::EUMB_I2C_SPACE:
	case AddressMapEntry::EUMB_DATA_PATH_DIAGNOSTICS_SPACE:
		if(unlikely(verbose))
			logger << DebugWarning << LOCATION
				<< "Embedded utilities block mapping not fully implemented" << std::endl
				<< EndDebugWarning;
		return false;
		break;
	default:
		logger << DebugError
			<< "TODO(" << __FUNCTION__			<< ":" << __FILE__ << ":" << __LINE__ 
			<< "): accessing an unmapped (or unhalded) memory space (address = "
			<< std::hex << addr << std::dec << ")" << std::endl
			<< EndDebugError;
		return false;
		break;
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
LocalMemoryDispatch() {
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
PCIDispatch() {
	PMsgType msg;
	PPCIMsgType pci_msg;
	PPCIReqType pci_req;
	PPCIRspType pci_rsp;
	
	/* request reply event */
	sc_core::sc_event rsp_event;
	
	/* we wait for new events to take pci request from the
	 *  pci request queue if the request does not expect a response
	 *  we suppress the request from the queu. If the request expects
	 *  a response we wait for the response (notification of the event)
	 *  to remove it from the pci request queue. */ 
	while(1) {
		wait(pci_dispatch_event);
		pci_msg = bus_pci_req_list.front();
		msg = req_list.front();
		pci_req = pci_msg->GetRequest();
		if((config_regs.pci_command_reg.value & (1 << 2)) == 0) {
			if(unlikely(verbose)) {
				logger << DebugWarning
						<< LOCATION
						<< "A PCI request to send through the PCI bus is ready, but the chipset is configured not to send them (PCI command register bit Bus Master ";
				DEBUG_PCI_REQ(pci_req);
				logger << std::endl
					<< "   TODO: if write held the request, if read rise an exception (if enabled)" << std::endl
					<< EndDebugWarning;					
			}
			continue; 
		} 
		/* transform the bus request into a pci request */
		/* send the pci request */
		if(msg->HasResponseEvent()) {
			/* put the the rsp_event in the msg event queue 
			 * send it
			 * wait for the response
			 * send the response to the source of the request */
			pci_msg->PushResponseEvent(rsp_event);
			if(unlikely(DEBUG && verbose))
				logger << DebugInfo
					<< "Sending read message through the pci bus (addr = 0x" 
					<< std::hex << pci_req->addr << std::dec << ")" << std::endl
					<< EndDebugInfo;
			while(!pci_master_port->Send(pci_msg)) {
				if(unlikely(DEBUG && verbose))
					logger << DebugInfo
						<< "could not send message through the pci bus (addr = 0x" 
						<< std::hex << pci_req->addr << std::dec << ")" << std::endl
						<< EndDebugInfo;
				wait(cycle_time);
				if(unlikely(DEBUG && verbose))
					logger << DebugInfo
						<< "retrying to send message through the pci bus (addr = 0x" 
						<< std::hex << pci_req->addr << std::dec << ")" << std::endl
						<< EndDebugInfo;
			}
			wait(rsp_event);
			PRspType rsp = new(rsp) RspType();
			pci_req = pci_msg->req;
			if(!pci_msg->req) {
				logger << DebugError
					<< LOCATION
					<< " received a response for a pci message without request field" 
					<< std::endl << EndDebugError;
				Object::Stop(-1);
			}
			if(!pci_msg->rsp) {
				logger << DebugError
					<< LOCATION
					<< " received a response for a pci message without response field" 
					<< std::endl << EndDebugError;
				Object::Stop(-1);
			}
			pci_rsp = pci_msg->rsp;
			rsp->read_status = RspType::RS_MISS;
			memcpy(rsp->read_data, pci_rsp->read_data, pci_req->size);
			msg->rsp = rsp;
			msg->GetResponseEvent()->notify(sc_core::SC_ZERO_TIME); 
		} else {
			/* just send the message */
			if(unlikely(DEBUG && verbose)) {
				logger << DebugInfo
					<< "Sending write message through the pci bus (addr = 0x" 
					<< std::hex << pci_req->addr << std::dec << ", size = " << pci_req->size << ", data = ";
				for(unsigned int i = 0; i < pci_req->size; i++) {
					logger << std::hex << (unsigned int)pci_req->write_data[i] << std::dec << " ";
				}
				logger << ")" << std::endl << EndDebugInfo;
			}
			while(!pci_master_port->Send(pci_msg)) {
				if(unlikely(DEBUG && verbose)) {
					logger << DebugInfo
						<< "could not send write message through the pci bus (addr = 0x" 
						<< std::hex << pci_req->addr << std::dec << ", size = " << pci_req->size << ", data = ";
					for(unsigned int i = 0; i < pci_req->size; i++) {
						logger << std::hex << (unsigned int)pci_req->write_data[i] << std::dec << " ";
					}
					logger << ")" << std::endl << EndDebugInfo;
				}
				wait(cycle_time);
				if(unlikely(DEBUG && verbose)) {
					logger << DebugInfo
						<< "retrying send write message through the pci bus (addr = 0x" 
						<< std::hex << pci_req->addr << std::dec << ", size = " << pci_req->size << ", data = ";
					for(unsigned int i = 0; i < pci_req->size; i++) {
						logger << std::hex << (unsigned int)pci_req->write_data[i] << std::dec << " ";
					}
					logger << ")" << std::endl << EndDebugInfo;
				}
			}
		}
		bus_pci_req_list.pop_front();
		req_list.pop_front();
		if(!bus_pci_req_list.empty()) pci_dispatch_event.notify_delayed();
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
Pointer<TlmMessage<SnoopingFSBRequest<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE>, SnoopingFSBResponse<MAX_TRANSACTION_DATA_SIZE> > >
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
ConverttoFSBMsg(const PPCIMsgType &pci_msg, int size_done, int size) {
	PReqType fsb_req = new(fsb_req) ReqType();
	PMsgType fsb_msg = new(fsb_msg) MsgType(fsb_req);
	PPCIReqType pci_req = pci_msg->req;

	fsb_req->global = true;
	fsb_req->addr = pci_req->addr + size_done;
	fsb_req->size = (size < (int) MAX_TRANSACTION_DATA_SIZE)?size:MAX_TRANSACTION_DATA_SIZE;
	switch(pci_req->type) {
	case unisim::component::cxx::pci::TT_READ:
		fsb_req->type = ReqType::READ;
		break;
	case unisim::component::cxx::pci::TT_WRITE:
		memcpy(fsb_req->write_data, pci_req->write_data + size_done, fsb_req->size);
		fsb_req->type = ReqType::WRITE;
		break;
	default:
		logger << DebugError
			<< "Unknown pci request type" << std::endl
			<< EndDebugError;
		Object::Stop(-1);
		break;
	}
	
	return fsb_msg;
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
Pointer<TlmMessage<MemoryRequest<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE>, MemoryResponse<MAX_TRANSACTION_DATA_SIZE> > >
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
ConverttoMemMsg(const PPCIMsgType &pci_msg, int size_done, int size) {
	PMemReqType mem_req = new(mem_req) MemReqType();
	PMemMsgType mem_msg = new(mem_msg) MemMsgType(mem_req);
	PPCIReqType pci_req = pci_msg->req;

	mem_req->addr = pci_req->addr + size_done;
	mem_req->size = (size < (int) MAX_TRANSACTION_DATA_SIZE)?size:MAX_TRANSACTION_DATA_SIZE;
	switch(pci_req->type) {
	case unisim::component::cxx::pci::TT_READ:
		mem_req->type = MemReqType::READ;
		break;
	case unisim::component::cxx::pci::TT_WRITE:
		memcpy(mem_req->write_data, pci_req->write_data + size_done, mem_req->size);
		mem_req->type = MemReqType::WRITE;
		break;
	default:
		logger << DebugError
			<< "Unknown pci request type" << std::endl
			<< EndDebugError;
		Object::Stop(-1);
		break;
	}
	
	return mem_msg;
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
Pointer<TlmMessage<MemoryRequest<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE>, MemoryResponse<MAX_TRANSACTION_DATA_SIZE> > > 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
ConverttoMemMsg(const PMsgType &fsb_msg) {
	//std::cerr << "new MemMsgType" << std::endl;
	PMemMsgType mem_msg = new(mem_msg) MemMsgType();
	//std::cerr << "new MemReqType" << std::endl;
	PMemReqType mem_req = new(mem_req) MemReqType();
	const PReqType &fsb_req = fsb_msg->req;
	//std::cerr << "Copy mem_req into mem_msg" << std::endl;
	mem_msg->req = mem_req;
	
	mem_req->addr = fsb_req->addr;
	mem_req->size = fsb_req->size;
	switch(fsb_req->type) {
	case ReqType::READ:  // Read request
	case ReqType::READX: // Read with intent to modify
		mem_req->type = MemReqType::READ;
		break;
	case ReqType::WRITE: // Write request
		mem_req->type = MemReqType::WRITE;
		memcpy(mem_req->write_data, fsb_req->write_data, fsb_req->size);
		break;
	case ReqType::INV_BLOCK:    // Invalidate block
		logger << DebugError << LOCATION
			<< "Can not send INV_BLOCK request to the memory system"
			<< std::endl << EndDebugError;
		Object::Stop(-1);
		break;
	case ReqType::FLUSH_BLOCK:  // Flush block
		logger << DebugError << LOCATION
			<< "Can not send FLUSH_BLOCK request to the memory system"
			<< std::endl << EndDebugError;
		Object::Stop(-1);
		break;
	case ReqType::ZERO_BLOCK:   // Fill in block with zero
		logger << DebugError << LOCATION
			<< "Can not send ZERO_BLOCK request to the memory system"
			<< std::endl << EndDebugError;
		Object::Stop(-1);
		break;
	case ReqType::INV_TLB:      // Invalidate TLB set
		logger << DebugError << LOCATION
			<< "Can not send INV_TLB request to the memory system"
			<< std::endl << EndDebugError;
		Object::Stop(-1);
		break;
	}

	//std::cerr << "returning mem_msg" << std::endl;
	return mem_msg;
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
SendPCItoMemory(const PPCIMsgType &pci_msg, sc_core::sc_port<TlmSendIf<MemReqType, MemRspType> > &out_port) {
	bool notify = pci_req_list.empty();
	PCItoMemoryReqType *buf;
	
	if(free_pci_req_list.empty()) 
		buf = new PCItoMemoryReqType();
	else {
		buf = free_pci_req_list.front();
		free_pci_req_list.pop_front();
	}
	buf->pci_msg = pci_msg;
	buf->out_port = &out_port;
	pci_req_list.push_back(buf);
	if(notify) {
		dispatch_pci_req_ev.notify();
	}
	
	return true;
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
SendFSBtoMemory(const PMsgType &fsb_msg, sc_core::sc_port<TlmSendIf<MemReqType, MemRspType> > &out_port) {
	/* Convert the fsb message to a memory message */
	PMemMsgType mem_msg = PMemMsgType();
	
	mem_msg = ConverttoMemMsg(fsb_msg);

	/* Effectively end the message to the memory system */
	if(fsb_msg->HasResponseEvent()) {
		return AdvancedResponseListener<
				MemReqType,
				MemRspType,
				ReqType,
				RspType 
				>::Send(mem_msg, out_port, fsb_msg, slave_port);
	} else {
		return out_port->Send(mem_msg);
	}
	return true;
}

template <class PHYSICAL_ADDR,
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	class PCI_ADDR,
	uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DispatchPCIReq() {
	PCItoMemoryReqType *item;
	while(1) {
		wait(dispatch_pci_req_ev);
		/* get the first element in the list of pci request for the
		 *   memory system */
		item = pci_req_list.front();
		// We have to take into account that the width of the pci bus may be greater than the one of the system bus [paula]
        int size_done = 0;
		PPCIRspType pci_rsp = new(pci_rsp) PCIRspType();
		while (size_done< (int) item->pci_msg->req->size) {
		
            			
			/* prepare a request (and message) for the system bus */
			PMsgType fsb_msg = PMsgType();
			
			fsb_msg = ConverttoFSBMsg(item->pci_msg, size_done, item->pci_msg->req->size - size_done);
			sc_core::sc_event fsb_msg_ev;
			
			if(item->pci_msg->HasResponseEvent())
				fsb_msg->PushResponseEvent(fsb_msg_ev);

				/* send the message to the system bus */
			while(!master_port->Send(fsb_msg)) {
				if(unlikely(verbose))
					logger << DebugError
					<< "Message was not accepted by the system bus, retrying" << std::endl
					<< EndDebugError;
				wait(cycle_time);
			}
		
			/* wait for the message response if the request was
		 	*   a read message,
		 	*   if the request was a hit, then send the response
		 	*   to the pci bus, otherwise it will have to be sent to
		 	*   the memory system */
			if(fsb_msg->req->type == ReqType::READ) {
		
				wait(fsb_msg_ev);
				const PRspType &fsb_rsp = fsb_msg->GetResponse();
				if(!(fsb_rsp->read_status == RspType::RS_SHARED ||
					fsb_rsp->read_status == RspType::RS_MODIFIED)) {
					// the response was not received from the system bus,
					//   it needs to be sent to the memory system
					PMemMsgType mem_msg = PMemMsgType();
				
					mem_msg = ConverttoMemMsg(item->pci_msg, size_done, item->pci_msg->req->size - size_done); 
					while(!(*(item->out_port))->Send(mem_msg))
						wait(cycle_time);
					memcpy(pci_rsp->read_data + size_done, mem_msg->rsp->read_data + size_done,  mem_msg->req->size);	
					size_done += mem_msg->req->size;
				} else {					
				
					if(unlikely(DEBUG && verbose))
						logger << DebugInfo
						<< sc_core::sc_time_stamp().to_string()
						<< " received response to a pci request from the memory system, "
						<< " forwarding response to the PCI bus" << std::endl
						<< EndDebugInfo;
					memcpy(pci_rsp->read_data + size_done, fsb_rsp->read_data + size_done,  fsb_msg->req->size);
					size_done += fsb_msg->req->size;
				}
			} else { /* the request is a write request, no reply expected */
				/* if the request was a write then the request needs to be sent also to the
			 	*   memory system */
				PMemMsgType mem_msg = PMemMsgType();
				
				mem_msg = ConverttoMemMsg(item->pci_msg, size_done, item->pci_msg->req->size - size_done);
				while(!(*(item->out_port))->Send(mem_msg))
					wait(cycle_time);
				size_done += mem_msg->req->size;
			}
			
		}
		
		if (item->pci_msg->req->type == unisim::component::cxx::pci::TT_READ) {
            item->pci_msg->SetResponse(pci_rsp);
            item->pci_msg->GetResponseEvent()->notify(sc_core::SC_ZERO_TIME);
        }
		
		item->pci_msg = 0;
		pci_req_list.pop_front();
	    free_pci_req_list.push_back(item);
        if(!pci_req_list.empty()) dispatch_pci_req_ev.notify_delayed();
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
ResponseReceived(const PMemMsgType &msg, 
	sc_core::sc_port<TlmSendIf<MemReqType, MemRspType> > &port,
	const PPCIMsgType &orig_pci_msg,
	sc_core::sc_export<TlmSendIf<PCIReqType, PCIRspType> > &who_pci_master_port) {
	PPCIRspType pci_rsp = new(pci_rsp) PCIRspType();
	const PPCIReqType &pci_req = orig_pci_msg->GetRequest();
	const PMemRspType &rsp = msg->GetResponse();
	
	if(unlikely(DEBUG && verbose))
		logger << DebugInfo
			<< sc_core::sc_time_stamp().to_string()
			<< " received response to a pci request from the memory system, "
			<< " forwarding response to the PCI bus" << std::endl
			<< EndDebugInfo;
	
	memcpy(pci_rsp->read_data, rsp->read_data,  pci_req->size);
	orig_pci_msg->SetResponse(pci_rsp);
	orig_pci_msg->GetResponseEvent()->notify(sc_core::SC_ZERO_TIME);
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
ResponseReceived(const PMemMsgType &msg, 
	sc_core::sc_port<TlmSendIf<MemReqType, MemRspType> > &port,
	const PMsgType &orig_fsb_msg,
	sc_core::sc_export<TlmSendIf<ReqType, RspType> > &who_fsb_master_port) {
	PRspType fsb_rsp = new(fsb_rsp) RspType();
	const PReqType &fsb_req = orig_fsb_msg->GetRequest();
	const PMemRspType &rsp = msg->GetResponse();
	
	if(unlikely(DEBUG && verbose))
		logger << DebugInfo
			<< sc_core::sc_time_stamp().to_string()
			<< " received response to a fsb request from the memory system, "
			<< " forwarding response to the FSB bus" << std::endl
			<< EndDebugInfo;
	
	memcpy(fsb_rsp->read_data, rsp->read_data,  fsb_req->size);
	fsb_rsp->read_status = RspType::RS_SHARED; 
	orig_fsb_msg->SetResponse(fsb_rsp);
	orig_fsb_msg->GetResponseEvent()->notify(sc_core::SC_ZERO_TIME);
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DMARead(PHYSICAL_ADDR addr,
		unsigned int size,
		unsigned int channel) {
	/* create the memory request and put it in the dma request list
	 *   to local memory */
	PReqType req;
	PMsgType msg;
	PDMAMsgType dma_msg;
	
	req = new(req) ReqType();
	req->type = ReqType::READ;
	req->global = false;
	req->addr = addr;
	req->size = size;
	
	msg = new (msg) MsgType(req);
	
	dma_msg = new(dma_msg) DMAMsgType();
	dma_msg->msg = msg;
	dma_msg->channel = channel;
	
	dma_req_list.push_back(dma_msg);
	if(dma_req_list.size() == 1) {
		dispatchDMALocalMemoryAccessEvent.notify_delayed();
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DMAWrite(PHYSICAL_ADDR addr,
		Pointer<uint8_t> &data,
		unsigned int size,
		unsigned int channel) {
	/* create the memory request and put it in the dma request list
	 *   to local memory */
	PReqType req;
	PMsgType msg;
	PDMAMsgType dma_msg;
	
	req = new(req) ReqType();
	req->type = ReqType::WRITE;
	req->global = false;
	req->addr = addr;
	req->size = size;
	for(unsigned int i = 0; i < size; i++) {
		req->write_data[i] = data[i];
	}
	
	msg = new (msg) MsgType(req);
	
	dma_msg = new(dma_msg) DMAMsgType();
	dma_msg->msg = msg;
	dma_msg->channel = channel;
	
	dma_req_list.push_back(dma_msg);
	if(dma_req_list.size() == 1) {
		dispatchDMALocalMemoryAccessEvent.notify_delayed();
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DispatchDMALocalMemoryAccess() {
	sc_core::sc_event rsp_event;
	bool read = false;
	
	while(1) {
		wait(dispatchDMALocalMemoryAccessEvent);
		
		/* get the message waiting in the front of the list */
		PDMAMsgType dma_msg = dma_req_list.front();
		PMsgType msg = dma_msg->msg;
		
		/* check if it is a read, if so put an event in the message
		 *   to be able to wait for the response */
		read = msg->req->type == ReqType::READ;
		if(read) {
			msg->PushResponseEvent(rsp_event);			
		}
		
		/* send the message */
		while(!Send(msg)) {
			wait(cycle_time);
		}
		
		/* if the message was a read wait for the response, and signal
		 *   it to the DMA */
		if(read) {
			wait(rsp_event);

			Pointer<uint8_t> data = new(data) uint8_t[msg->req->size];
			for(unsigned int i = 0; i < msg->req->size; i++) {
				data[i] = msg->rsp->read_data[i];
			}
			dma.ReadReceived(dma_msg->channel,
				data,
				msg->req->size);
		}
		
		
		/* if still messages in the list, rise the event to be 
		 *   executed again */
		if(dma_req_list.size() != 0)
			dispatchDMALocalMemoryAccessEvent.notify_delayed();
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DMAPCIRead(PHYSICAL_ADDR addr,
		 unsigned int size,
		 unsigned int channel) {
	/* create the pci request and put it in th dma request list
	 *   to the pci memory */
	PPCIReqType req;
	PPCIMsgType msg;
	PDMAPCIMsgType dma_msg;
	
	req = new(req) PCIReqType();
	req->type = unisim::component::cxx::pci::TT_READ;
	req->addr = addr;
	req->size = size;
	
	msg = new(msg) PCIMsgType(req);
	
	dma_msg = new(dma_msg) DMAPCIMsgType();
	dma_msg->msg = msg;
	dma_msg->channel = channel;
	
	dma_pci_req_list.push_back(dma_msg);
	if(dma_pci_req_list.size() == 1) {
		dispatchDMAPCIAccessEvent.notify_delayed();
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DMAPCIWrite(PHYSICAL_ADDR addr,
		Pointer<uint8_t> &data,
		unsigned int size,
		unsigned int channel) {
	/* create the pci request and put it in th dma request list
	 *   to the pci memory */
	PPCIReqType req;
	PPCIMsgType msg;
	PDMAPCIMsgType dma_msg;
	
	req = new(req) PCIReqType();
	req->type = unisim::component::cxx::pci::TT_WRITE;
	req->addr = addr;
	req->size = size;
	for(unsigned int i = 0; i < size; i++) {
		req->write_data[i] = data[i];
	}
	
	msg = new(msg) PCIMsgType(req);
	
	dma_msg = new(dma_msg) DMAPCIMsgType();
	dma_msg->msg = msg;
	dma_msg->channel = channel;
	
	dma_pci_req_list.push_back(dma_msg);
	if(dma_pci_req_list.size() == 1) {
		dispatchDMAPCIAccessEvent.notify_delayed();
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DispatchDMAPCIAccess() {
	sc_core::sc_event rsp_event;
	bool read = false;
	
	while(1) {
		wait(dispatchDMALocalMemoryAccessEvent);
		
		/* get the message waiting in the front of the list */
		PDMAPCIMsgType dma_msg = dma_pci_req_list.front();
		PPCIMsgType msg = dma_msg->msg;
		
		/* check if it is a read, if so put an event in the message
		 *   to be able to wait for the response */
		read = msg->req->type == unisim::component::cxx::pci::TT_READ;
		if(read) {
			msg->PushResponseEvent(rsp_event);
		}
		
		/* send the message */
//		while(!Send(msg)) {
//			wait(cycle_time);
//		}
		
		/* if the message was a read wait for the response, and signal
		 *   it to the DMA */
//		if(read) {
//			wait(rsp_event);
//
//			Pointer<uint8_t> data = new(uint8_t) data[msg->req->size];
//			for(unsigned int i = 0; i < msg->req->size; i++) {
//				data[i] = msg->rsp->read_data[i];
//			}
//			dma->ReadReceived(dma_msg->channel,
//				data,
//				msg->req->size);
//		}
		
		
		/* if still messages in the list, rise the event to be 
		 *   executed again */
		if(dma_req_list.size() != 0)
			dispatchDMALocalMemoryAccessEvent.notify_delayed();
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
SendFSBtoATU(const PMsgType &message) {
	const PReqType& req = message->GetRequest();
	bool read = false;
	
	switch(req->type) {
	case ReqType::READ:
	case ReqType::READX:
		read = true;
		break;
	case ReqType::WRITE:
		read = false;
		break;
	case ReqType::INV_BLOCK:
	case ReqType::FLUSH_BLOCK:
	case ReqType::ZERO_BLOCK:
	case ReqType::INV_TLB:
		if(unlikely(verbose)) {
			logger << DebugError << LOCATION 
				<< "Invalid request type to send to the ATU controller"
				<< std::endl << EndDebugError;
		}
		return true;
		break;
	}
	
	bool ret = false;;
	// handle write requests
	if(!read) {
		ret = atu.MemWrite(req->addr, req->write_data, req->size);
		if(ret) ret = addr_map.Reset();
		return ret;
	}
	// handle read requests
	PRspType rsp = new(rsp) RspType();
	ret = atu.MemRead(req->addr, rsp->read_data, req->size);
	rsp->read_status = RspType::RS_SHARED;
	return ret;
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
ReadMemorytoATU(PHYSICAL_ADDR addr, void *buffer, uint32_t size) {
	return atu.MemRead(addr, (uint8_t *)buffer, size);
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
WriteMemorytoATU(PHYSICAL_ADDR addr, const void *buffer, uint32_t size) {
	bool ret = false;
	ret = atu.MemWrite(addr, (uint8_t *)buffer, size);
	if(ret) ret = addr_map.Reset();
	return ret;
}

#undef LOCATION

} // end of namespace unisim
} // end of namespace component
} // end of namespace tlm
} // end of namespace chipset
} // end of namespace mpc107

#endif // __UNISIM_COMPONENT_TLM_CHIPSET_MPC107_MPC107_TCC__
