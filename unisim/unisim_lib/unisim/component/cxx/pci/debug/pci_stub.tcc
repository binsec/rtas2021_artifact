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

#ifndef __UNISIM_COMPONENT_CXX_PCI_DEBUG_PCI_STUB_TCC__
#define __UNISIM_COMPONENT_CXX_PCI_DEBUG_PCI_STUB_TCC__

#include <unisim/util/likely/likely.hh>
#include <sstream>

namespace unisim {
namespace component {
namespace cxx {
namespace pci {
namespace debug {

using namespace std;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

using unisim::util::debug::Symbol;
	
template <class ADDRESS>
PCIStub<ADDRESS>::PCIStub(const char *name, Object *parent) :
	Object(name, parent),
	NetStub<ADDRESS>(false, 0, "localhost", 10000, "pipe"),
	Service<Memory<ADDRESS> >(name, parent),
	Service<MemoryAccessReporting<ADDRESS> >(name, parent),
	Client<MemoryAccessReportingControl>(name, parent),
	Client<SymbolTableLookup<ADDRESS> >(name, parent),
	Client<Synchronizable>(name, parent),
	Client<Memory<ADDRESS> >(name, parent),
	Client<Registers>(name, parent),
	memory_export("memory_export", this),
	memory_access_reporting_export("memory-access-reporting-export", this),
	memory_access_reporting_control_import("memory-access-reporting-control-import", this),
	symbol_table_lookup_import("symbol-table-lookup-import", this),
	synchronizable_import("synchronizable-import", this),
	memory_import("memory-import", this),
	registers_import("registers-import", this),
	breakpoint_registry(),
	watchpoint_registry(),
	logger(static_cast<Object&>(*this)),
	verbose(false),

	// PCI configuration registers initialization
	pci_conf_device_id("pci_conf_device_id", "PCI config Device ID", 0x0, 0x0000), // TBD
	pci_conf_vendor_id("pci_conf_vendor_id", "PCI config Vendor ID", 0x0, 0x0000), // TBD
	pci_conf_status("pci_conf_status", "PCI Config Status", 0x0, 0x0),
	pci_conf_command("pci_conf_command", "PCI Config Command", 0x007f, 0x0),
	pci_conf_class_code("pci_conf_class_code", "PCI Config Class Code", 0x0, 0x0),
	pci_conf_revision_id("pci_conf_revision_id", "PCI Config Revision ID", 0x0, 0x0),
	pci_conf_bist("pci_conf_bist", "PCI Config BIST", 0x0, 0x0),
	pci_conf_header_type("pci_conf_header_type", "PCI Config Header Type", 0x0, 0x0),
	pci_conf_latency_timer("pci_conf_latency_timer", "PCI Config Latency Timer", 0x0, 0x0),
	pci_conf_cache_line_size("pci_conf_cache_line_size", "PCI Config Cache Line Size", 0x0, 0x0),

	pci_conf_carbus_cis_pointer("pci_conf_carbus_cis_pointer", "PCI Config Carbus CIS Pointer", 0x0, 0x0),
	pci_conf_subsystem_id("pci_conf_subsystem_id", "PCI Config Subsystem ID", 0x0, 0x0),
	pci_conf_subsystem_vendor_id("pci_conf_subsystem_vendor_id", "PCI Config Subsystem Vendor ID", 0x0, 0x0),
	
	pci_device_number(0),
	pci_bus_frequency(33),
	bus_frequency(0),
	traps(),
	// Parameters initialization
	param_verbose("verbose", this, verbose),
	param_is_server("is-server", this, inherited::is_server),
	param_protocol("protocol", this, inherited::protocol),
	param_pipe_name("pipe-name", this, inherited::pipename),
	param_server_name("server-name", this, inherited::server_name),
	param_tcp_port("tcp-port", this, inherited::tcp_port),
	param_initial_base_addr("initial-base-addr", this, initial_base_addr, NUM_REGIONS),
	param_address_space("address-space", this, address_space, NUM_REGIONS),
	param_region_size("region-size", this, region_size, NUM_REGIONS),
	param_pci_device_number("pci-device-number", this, pci_device_number),
	param_pci_bus_frequency("pci-bus-frequency", this, pci_bus_frequency),
	param_bus_frequency("bus-frequency", this, bus_frequency)
{
	unsigned int num_region;

	for(num_region = 0; num_region < NUM_REGIONS; num_region++)
	{
		region_size[num_region] = 0;
		storage[num_region] = 0;
	}
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::BeginSetup()
{
	Reset();
	
	return true;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::SetupMemory()
{
	if(unlikely(verbose))
	{
		unsigned int num_region;

		for(num_region = 0; num_region < NUM_REGIONS; num_region++)
		{
			if(region_size[num_region] > 0)
			{
				logger << DebugInfo;
				logger << "Region #" << num_region << " : initial base address is 0x" << std::hex << initial_base_addr[num_region] << std::dec << ", region size is " << region_size[num_region] << " bytes, address space is ";
				switch(address_space[num_region])
				{
					case unisim::component::cxx::pci::SP_MEM:
						logger << "mem";
						break;
					case unisim::component::cxx::pci::SP_IO:
						logger << "i/o";
						break;
					case unisim::component::cxx::pci::SP_CONFIG:
						logger << "cfg";
				}
				logger << std::endl;
				logger << EndDebugInfo;
			}
			else
			{
				logger << DebugInfo;
				logger << "No region #" << num_region << std::endl;
				logger << EndDebugInfo;
			}
		}
	}
	// PCI configuration registers initialization	

	pci_conf_command = pci_conf_command | 0x2; // active memory space

	unsigned int num_region;
	for(num_region = 0; num_region < NUM_REGIONS; num_region++)
	{
		if(region_size[num_region] > 0)
		{
			stringstream sstr_short_name;
			sstr_short_name << "pci_conf_base_addr[" << num_region << "]";
			string short_name = sstr_short_name.str();
			stringstream sstr_long_name;
			sstr_long_name << "PCI Config Base Address " << num_region << "]";
			string long_name = sstr_long_name.str();
			switch(address_space[num_region])
			{
				case unisim::component::cxx::pci::SP_MEM:
					pci_conf_base_addr[num_region].Initialize(short_name.c_str(), long_name.c_str(), 0xfffffff0UL, initial_base_addr[num_region]);
					break;
				case unisim::component::cxx::pci::SP_IO:
					pci_conf_base_addr[num_region].Initialize(short_name.c_str(), long_name.c_str(), 0xfffffffcUL, initial_base_addr[num_region]);
					break;
				case unisim::component::cxx::pci::SP_CONFIG:
					if(unlikely(verbose))
					{
						logger << DebugInfo;
						logger << "ignoring region #" << num_region << " because it's configuration space" << std::endl;
						logger << EndDebugInfo;
					}
					region_size[num_region] = 0;
					continue;
			}

			storage[num_region] = new uint8_t[region_size[num_region]];
			memset(storage[num_region], 0, region_size[num_region]);
		}
	}
	
	return true;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::SetupMemoryAccessReporting()
{
	return true;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::Setup(ServiceExportBase *srv_export)
{
	if(srv_export == &memory_export) return SetupMemory();
	if(srv_export == &memory_access_reporting_export) return SetupMemoryAccessReporting();
	
	logger << DebugError << "Internal error" << EndDebugError;
	
	return false;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::EndSetup()
{
	if(memory_access_reporting_control_import) {
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				unisim::service::interfaces::REPORT_MEM_ACCESS,
				false);
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				unisim::service::interfaces::REPORT_FETCH_INSN,
				false);
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				unisim::service::interfaces::REPORT_COMMIT_INSN,
				false);
	}

	if(!memory_import)
	{
		if(unlikely(verbose))
		{
			logger << DebugError;
			logger << memory_import.GetName() << " is not connected" << std::endl;
			logger << EndDebugError;
		}
		return false;
	}

	if(!registers_import)
	{
		if(unlikely(verbose))
		{
			logger << DebugError;
			logger << registers_import.GetName() << " is not connected" << std::endl;
			logger << EndDebugError;
		}
		return false;
	}

	if(!synchronizable_import)
	{
		if(unlikely(verbose))
		{
			logger << DebugError;
			logger << synchronizable_import.GetName() << " is not connected" << std::endl;
			logger << EndDebugError;
		}
		return false;
	}
	
	return inherited::Initialize();
}

template <class ADDRESS>
PCIStub<ADDRESS>::~PCIStub()
{
	Reset();
}

template <class ADDRESS>
void PCIStub<ADDRESS>::OnDisconnect()
{
}

template <class ADDRESS>
void PCIStub<ADDRESS>::Reset()
{
	unsigned int num_region;

	for(num_region = 0; num_region < NUM_REGIONS; num_region++)
	{
		if(storage[num_region])
		{
			delete[] storage[num_region];
			storage[num_region] = 0;
		}
	}
	
	watchpoint_registry[unisim::component::cxx::pci::SP_MEM].Reset();
	watchpoint_registry[unisim::component::cxx::pci::SP_IO].Reset();
	watchpoint_registry[unisim::component::cxx::pci::SP_CONFIG].Reset();
	breakpoint_registry.Reset();
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::Write(ADDRESS addr, const void *buffer, uint32_t size, PCISpace space, const bool monitor)
{
	if(space == unisim::component::cxx::pci::SP_CONFIG)
	{
		if(size > 0)
		{
			if(((addr >> 11) & 31) != pci_device_number) return false;

			uint32_t offset = addr & 0xff;

			uint8_t *data = (uint8_t *) buffer;
			do
			{
				uint8_t value = *data;

				if(unlikely(verbose))
				{
					logger << DebugInfo;
					logger << "Writing 0x" << std::hex << value << std::dec << " in configuration space at 0x" << std::hex << addr << std::dec << std::endl;
					logger << EndDebugInfo;
				}

				switch(offset)
				{
					case 0x04: pci_conf_command = (pci_conf_command & 0xff00) | value; continue;
					case 0x05: pci_conf_command = (pci_conf_command & 0x00ff) | ((uint16_t) value << 8); continue;
					case 0x0f: pci_conf_bist = value; continue;
				}

				if(offset >= 0x10 && offset <= 0x27)
				{
					uint32_t num_region = (offset - 0x10) >> 2;
					if(address_space[num_region] != unisim::component::cxx::pci::SP_CONFIG)
					{
						uint32_t mask = ~(0xffUL << ((offset - 0x10) & 0x3));
						switch(address_space[num_region])
						{
							case unisim::component::cxx::pci::SP_MEM:
								mask = mask & 0xfffffff0UL;
								break;
							case unisim::component::cxx::pci::SP_IO:
								mask = mask & 0xfffffffcUL;
								break;
							default:
								logger << DebugError << "Internal error" << EndDebugError;
								Object::Stop(-1);
								return false;
						}
				
						pci_conf_base_addr[num_region] = ((pci_conf_base_addr[num_region] & mask) == mask && value == 0xff) ?
						                                 (~region_size[num_region] + 1) : (pci_conf_base_addr[num_region] & mask) | value;
					}
					continue;
				}
			} while(++offset, ++data, --size);
		}
		
		return true;
	}

	unsigned int num_region;

	for(num_region = 0; num_region < NUM_REGIONS; num_region++)
	{
		if(region_size[num_region] == 0) return false;
		if(address_space[num_region] != space) continue;
		if(addr >= pci_conf_base_addr[num_region] && (addr + (size - 1)) < (pci_conf_base_addr[num_region] + region_size[num_region]) && (addr + size) >= addr)
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo;
				logger << "Writing " << size << " bytes at 0x" << std::hex << addr << std::dec << " (region #" << num_region << ")" << std::endl;
				logger << EndDebugInfo;
			}
			memcpy(storage[num_region] + addr - pci_conf_base_addr[num_region], buffer, size);

			if(monitor)
			{
				typename inherited::SPACE sp;
				switch(space)
				{
					case unisim::component::cxx::pci::SP_MEM: sp = inherited::SP_DEV_MEM; break;
					case unisim::component::cxx::pci::SP_IO: sp = inherited::SP_DEV_IO; break;
					default:
						logger << DebugError << "Internal error" << EndDebugError;
						Object::Stop(-1);
						return false;
				}
				if(watchpoint_registry[sp].HasWatchpoints(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size))
				{
					synchronizable_import->Synchronize();
			
					typename inherited::TRAP trap;
					trap.type = inherited::TRAP_WATCHPOINT;
					trap.watchpoint.wtype = inherited::WATCHPOINT_WRITE;
					trap.watchpoint.addr = addr;
					trap.watchpoint.size = size;
					trap.watchpoint.space = sp;
					traps.push_back(trap);
			
					Trap();
				}
			}
			return true;
		}
	}
	return false;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::Read(ADDRESS addr, void *buffer, uint32_t size, PCISpace space, const bool monitor)
{
	if(space == unisim::component::cxx::pci::SP_CONFIG)
	{
		if(size > 0)
		{
			if(((addr >> 11) & 31) != pci_device_number) return false;

			uint32_t offset = addr & 0xff;

			uint8_t *data = (uint8_t *) buffer;
			do
			{
				switch(offset)
				{
					case 0x00: *data = pci_conf_vendor_id; continue;
					case 0x01: *data = pci_conf_vendor_id >> 8; continue;
					case 0x02: *data = pci_conf_device_id; continue;
					case 0x03: *data = pci_conf_device_id >> 8; continue;
					case 0x04: *data = pci_conf_command; continue;
					case 0x05: *data = pci_conf_command >> 8; continue;
					case 0x06: *data = pci_conf_status; continue;
					case 0x07: *data = pci_conf_status >> 8; continue;
					case 0x08: *data = pci_conf_revision_id; continue;
					case 0x09: *data = pci_conf_class_code; continue;
					case 0x0a: *data = pci_conf_class_code >> 8; continue;
					case 0x0b: *data = pci_conf_class_code >> 16; continue;
					case 0x0c: *data = pci_conf_cache_line_size; continue;
					case 0x0d: *data = pci_conf_latency_timer; continue;
					case 0x0e: *data = pci_conf_header_type; continue;
					case 0x0f: *data = pci_conf_bist; continue;
					case 0x28: *data = 0xf1; continue;
					case 0x29: *data = 0xff; continue;
					case 0x2a: *data = 0xff; continue;
					case 0x2b: *data = 0xff; continue;
				}

				if(offset >= 0x10 && offset <= 0x27)
				{
					uint32_t num_region = (offset - 0x10) >> 2;
					uint32_t shift = 8 * ((offset - 0x10) & 0x3);
					*data = pci_conf_base_addr[num_region] >> shift;
					continue;
				}
			
				*data = 0x00;

				if(unlikely(verbose))
				{
					logger << DebugInfo;
					logger << "Reading 0x" << std::hex << (*data) << std::dec << " in configuration space at 0x" << std::hex << addr << std::dec << std::endl;
					logger << EndDebugInfo;
				}
			} while(++offset, ++data, --size);
		}
		
		return true;
	}

	unsigned int num_region;

	for(num_region = 0; num_region < NUM_REGIONS; num_region++)
	{
		if(region_size[num_region] == 0) return false;
		if(address_space[num_region] != space) continue;
		if(addr >= pci_conf_base_addr[num_region] && (addr + (size - 1)) < (pci_conf_base_addr[num_region] + region_size[num_region]) && (addr + size) >= addr)
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo;
				logger << "Reading " << size << " bytes at 0x" << std::hex << addr << std::dec << " (region #" << num_region << ")" << std::endl;
				logger << EndDebugInfo;
			}
			memcpy(buffer, storage[num_region] + addr - pci_conf_base_addr[num_region], size);

			if(monitor)
			{
				typename inherited::SPACE sp;
				switch(space)
				{
					case unisim::component::cxx::pci::SP_MEM: sp = inherited::SP_DEV_MEM; break;
					case unisim::component::cxx::pci::SP_IO: sp = inherited::SP_DEV_IO; break;
					default:
						logger << DebugError << "Internal error" << EndDebugError;
						Object::Stop(-1);
						return false;
				}
				if(watchpoint_registry[sp].HasWatchpoints(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size))
				{
					synchronizable_import->Synchronize();
			
					typename inherited::TRAP trap;
					trap.type = inherited::TRAP_WATCHPOINT;
					trap.watchpoint.wtype = inherited::WATCHPOINT_READ;
					trap.watchpoint.addr = addr;
					trap.watchpoint.size = size;
					trap.watchpoint.space = sp;
					traps.push_back(trap);
			
					Trap();
				}
			}

			return true;
		}
	}
	return false;
}

template <class ADDRESS>
void PCIStub<ADDRESS>::ResetMemory()
{
	Reset();
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::ReadMemory(ADDRESS physical_addr, void *buffer, uint32_t size)
{
	return Read(physical_addr, buffer, size, unisim::component::cxx::pci::SP_MEM, false);
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::WriteMemory(ADDRESS physical_addr, const void *buffer, uint32_t size)
{
	return Write(physical_addr, buffer, size, unisim::component::cxx::pci::SP_MEM, false);
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::ServeRead(ADDRESS addr, void *buffer, uint32_t size, typename inherited::SPACE space)
{
	switch(space)
	{
		case inherited::SP_CPU_MEM: return memory_import->ReadMemory(addr, buffer, size);
		case inherited::SP_DEV_MEM: return Read(addr, buffer, size, unisim::component::cxx::pci::SP_MEM, false);
		case inherited::SP_DEV_IO: return Read(addr, buffer, size, unisim::component::cxx::pci::SP_IO, false);
	}
	return false;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::ServeWrite(ADDRESS addr, const void *buffer, uint32_t size, typename inherited::SPACE space)
{
	switch(space)
	{
		case inherited::SP_CPU_MEM: return memory_import->WriteMemory(addr, buffer, size);
		case inherited::SP_DEV_MEM: return Write(addr, buffer, size, unisim::component::cxx::pci::SP_MEM, false);
		case inherited::SP_DEV_IO: return Write(addr, buffer, size, unisim::component::cxx::pci::SP_IO, false);
	}
	return false;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::ServeReadRegister(const char *name, uint32_t& value)
{
	unisim::service::interfaces::Register *reg = registers_import->GetRegister(name);
	if(!reg) return false;
	if(reg->GetSize() != sizeof(value)) return false;
	reg->GetValue(&value);
	return true;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::ServeWriteRegister(const char *name, uint32_t value)
{
	unisim::service::interfaces::Register *reg = registers_import->GetRegister(name);
	if(!reg) return false;
	if(reg->GetSize() != sizeof(value)) return false;
	reg->SetValue(&value);
	return true;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::ReportMemoryAccess(typename unisim::util::debug::MemoryAccessType mat, typename unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size)
{
	if(!watchpoint_registry[inherited::SP_CPU_MEM].HasWatchpoints()) return true;
	if(watchpoint_registry[inherited::SP_CPU_MEM].HasWatchpoints(mat, mt, addr, size))
	{
		synchronizable_import->Synchronize();

		typename inherited::TRAP trap;
		trap.type = inherited::TRAP_WATCHPOINT;
		switch(mat)
		{
			case unisim::util::debug::MAT_READ:
				trap.watchpoint.wtype = inherited::WATCHPOINT_READ;
				break;
			case unisim::util::debug::MAT_WRITE:
				trap.watchpoint.wtype = inherited::WATCHPOINT_WRITE;
				break;
			default:
				// Ignore report
				return true;
		}
		trap.watchpoint.addr = addr;
		trap.watchpoint.size = size;
		trap.watchpoint.space = inherited::SP_CPU_MEM;
		traps.push_back(trap);

		Trap();
	}
	
	return true;
}

template <class ADDRESS>
void PCIStub<ADDRESS>::ReportCommitInstruction(ADDRESS addr, unsigned int length)
{
}

template <class ADDRESS>
void PCIStub<ADDRESS>::ReportFetchInstruction(ADDRESS next_addr)
{
	if(!breakpoint_registry.HasBreakpoints()) return;
	if(breakpoint_registry.HasBreakpoints(next_addr, 0 /* CPU #0 */))
	{
		synchronizable_import->Synchronize();

		typename inherited::TRAP trap;
		trap.type = inherited::TRAP_BREAKPOINT;
		trap.breakpoint.addr = next_addr;
		traps.push_back(trap);

		Trap();
	}
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::ServeSetBreakpoint(ADDRESS addr)
{
	bool ret = breakpoint_registry.SetBreakpoint(addr);
	if(memory_access_reporting_control_import)
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				unisim::service::interfaces::REPORT_FETCH_INSN,
				breakpoint_registry.HasBreakpoints());
	return ret;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::ServeSetBreakpoint(const char *symbol_name)
{
	if(!symbol_table_lookup_import)
	{
		if(unlikely(verbose))
		{
			logger << DebugWarning;
			logger << "No symbol table connected. Can't lookup symbol \"" << symbol_name << "\"" << std::endl;
			logger << EndDebugWarning;
		}
		return false;
	}

	const Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByName(symbol_name);

	if(!symbol)
	{
		if(unlikely(verbose))
		{
			logger << DebugWarning;
			logger << "Symbol \"" << symbol_name << "\" not found" << std::endl;
			logger << EndDebugWarning;
		}
		return false;
	}
	bool ret = breakpoint_registry.SetBreakpoint(symbol->GetAddress());
	if(memory_access_reporting_control_import)
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				unisim::service::interfaces::REPORT_FETCH_INSN,
				breakpoint_registry.HasBreakpoints());
	return ret;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::ServeSetReadWatchpoint(ADDRESS addr, uint32_t size, typename inherited::SPACE space)
{
	bool ret = watchpoint_registry[space].SetWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size);
	if(memory_access_reporting_control_import)
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				unisim::service::interfaces::REPORT_MEM_ACCESS,
				watchpoint_registry[inherited::SP_CPU_MEM].HasWatchpoints() || 
				watchpoint_registry[inherited::SP_DEV_MEM].HasWatchpoints() ||
				watchpoint_registry[inherited::SP_DEV_IO].HasWatchpoints());
	return ret;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::ServeSetWriteWatchpoint(ADDRESS addr, uint32_t size, typename inherited::SPACE space)
{
	bool ret = watchpoint_registry[space].SetWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size);
	if(memory_access_reporting_control_import)
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				unisim::service::interfaces::REPORT_MEM_ACCESS,
				watchpoint_registry[inherited::SP_CPU_MEM].HasWatchpoints() || 
				watchpoint_registry[inherited::SP_DEV_MEM].HasWatchpoints() ||
				watchpoint_registry[inherited::SP_DEV_IO].HasWatchpoints());
	return ret;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::ServeRemoveBreakpoint(ADDRESS addr)
{
	bool ret = breakpoint_registry.RemoveBreakpoint(addr);
	if(memory_access_reporting_control_import)
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				unisim::service::interfaces::REPORT_FETCH_INSN,
				breakpoint_registry.HasBreakpoints());
	return ret;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::ServeRemoveBreakpoint(const char *symbol_name)
{
	if(!symbol_table_lookup_import)
	{
		if(unlikely(verbose))
		{
			logger << DebugWarning;
			logger << "No symbol table connected. Can't lookup symbol \"" << symbol_name << "\"" << std::endl;
			logger << EndDebugWarning;
		}
		return false;
	}

	const Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByName(symbol_name);

	if(!symbol)
	{
		if(unlikely(verbose))
		{
			logger << DebugWarning;
			logger << "Symbol \"" << symbol_name << "\" not found" << std::endl;
			logger << EndDebugWarning;
		}
		return false;
	}

	bool ret = breakpoint_registry.RemoveBreakpoint(symbol->GetAddress());
	if(memory_access_reporting_control_import)
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				unisim::service::interfaces::REPORT_FETCH_INSN,
				breakpoint_registry.HasBreakpoints());
	return ret;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::ServeRemoveReadWatchpoint(ADDRESS addr, uint32_t size, typename inherited::SPACE space)
{
	bool ret = watchpoint_registry[space].RemoveWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size);
	if(memory_access_reporting_control_import)
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				unisim::service::interfaces::REPORT_MEM_ACCESS,
				watchpoint_registry[inherited::SP_CPU_MEM].HasWatchpoints() || 
				watchpoint_registry[inherited::SP_DEV_MEM].HasWatchpoints() ||
				watchpoint_registry[inherited::SP_DEV_IO].HasWatchpoints());
	return ret;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::ServeRemoveWriteWatchpoint(ADDRESS addr, uint32_t size, typename inherited::SPACE space)
{
	bool ret = watchpoint_registry[space].RemoveWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size);
	if(memory_access_reporting_control_import)
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				unisim::service::interfaces::REPORT_MEM_ACCESS,
				watchpoint_registry[inherited::SP_CPU_MEM].HasWatchpoints() || 
				watchpoint_registry[inherited::SP_DEV_MEM].HasWatchpoints() ||
				watchpoint_registry[inherited::SP_DEV_IO].HasWatchpoints());
	return ret;
}


template <class ADDRESS>
void PCIStub<ADDRESS>::Trap()
{
}

} // end of namespace debug
} // end of namespace pci
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
