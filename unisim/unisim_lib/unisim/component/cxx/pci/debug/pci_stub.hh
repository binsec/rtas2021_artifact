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

#ifndef __UNISIM_COMPONENT_CXX_PCI_DEBUG_PCI_STUB_HH__
#define __UNISIM_COMPONENT_CXX_PCI_DEBUG_PCI_STUB_HH__

#include <inttypes.h>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/device/register.hh>
#include <unisim/kernel/logger/logger.hh>
#include <string>
#include <vector>
#include <unisim/util/netstub/netstub.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/util/debug/breakpoint_registry.hh>
#include <unisim/util/debug/watchpoint_registry.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/synchronizable.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/component/cxx/pci/types.hh>
#include <unisim/util/debug/memory_access_type.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace pci {
namespace debug {

using std::string;
using std::vector;
using std::list;

using namespace unisim::util::endian;
using unisim::service::interfaces::Memory;
using unisim::util::device::Register;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::util::debug::BreakpointRegistry;
using unisim::util::debug::WatchpointRegistry;
using unisim::util::debug::Symbol;
using unisim::service::interfaces::SymbolTableLookup;

using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::ServiceImport;
using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::ParameterArray;
using unisim::util::netstub::NetStub;
using unisim::service::interfaces::Synchronizable;
using unisim::service::interfaces::Registers;

template <class ADDRESS>
class PCIStub :
	public NetStub<ADDRESS>,
	public Service<Memory<ADDRESS> >,
	public Service<MemoryAccessReporting<ADDRESS> >,
	public Client<MemoryAccessReportingControl>,
	public Client<SymbolTableLookup<ADDRESS> >,
	public Client<Synchronizable>,
	public Client<Memory<ADDRESS> >,
	public Client<Registers>
{
public:
	static const unsigned int NUM_REGIONS = 6; // This value *must not* be modified

	typedef NetStub<ADDRESS> inherited;
	typedef Register<uint32_t> reg32_t;
	typedef Register<uint32_t> reg24_t;
	typedef Register<uint16_t> reg16_t;
	typedef Register<uint8_t> reg8_t;
	
	ServiceExport<Memory<ADDRESS> > memory_export;
	ServiceExport<MemoryAccessReporting<ADDRESS> > memory_access_reporting_export;
	ServiceImport<MemoryAccessReportingControl> memory_access_reporting_control_import;
	ServiceImport<SymbolTableLookup<ADDRESS> > symbol_table_lookup_import;
	ServiceImport<Synchronizable> synchronizable_import;
	ServiceImport<Memory<ADDRESS> > memory_import;
	ServiceImport<Registers> registers_import;
	
	PCIStub(const char *name, Object *parent = 0);
	virtual ~PCIStub();

	virtual void OnDisconnect();
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();
	virtual void Reset();
	bool Read(ADDRESS physical_addr, void *buffer, uint32_t size, unisim::component::cxx::pci::PCISpace space, const bool monitor = true);
	bool Write(ADDRESS physical_addr, const void *buffer, uint32_t size, unisim::component::cxx::pci::PCISpace space, const bool monitor = true);

	virtual void ServeRun(uint64_t duration, typename inherited::TIME_UNIT duration_tu, uint64_t& t, typename inherited::TIME_UNIT& tu, list<typename inherited::TRAP>& traps) = 0;
	virtual bool ServeRead(ADDRESS addr, void *buffer, uint32_t size, typename inherited::SPACE space);
	virtual bool ServeWrite(ADDRESS addr, const void *buffer, uint32_t size, typename inherited::SPACE space);
	virtual bool ServeReadRegister(const char *name, uint32_t& value);
	virtual bool ServeWriteRegister(const char *name, uint32_t value);
	virtual bool ServeSetBreakpoint(ADDRESS addr);
	virtual bool ServeSetBreakpoint(const char *symbol_name);
	virtual bool ServeSetReadWatchpoint(ADDRESS addr, uint32_t size, typename inherited::SPACE space);
	virtual bool ServeSetWriteWatchpoint(ADDRESS addr, uint32_t size, typename inherited::SPACE space);
	virtual bool ServeRemoveBreakpoint(ADDRESS addr);
	virtual bool ServeRemoveBreakpoint(const char *symbol_name);
	virtual bool ServeRemoveReadWatchpoint(ADDRESS addr, uint32_t size, typename inherited::SPACE space);
	virtual bool ServeRemoveWriteWatchpoint(ADDRESS addr, uint32_t size, typename inherited::SPACE space);

	virtual void ResetMemory();
	virtual bool WriteMemory(ADDRESS physical_addr, const void *buffer, uint32_t size);
	virtual bool ReadMemory(ADDRESS physical_addr, void *buffer, uint32_t size);
	virtual bool ReportMemoryAccess(typename unisim::util::debug::MemoryAccessType mat, typename unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size);
	virtual void ReportCommitInstruction(ADDRESS addr, unsigned int length);
	virtual void ReportFetchInstruction(ADDRESS next_addr);
	virtual void Trap();
private:
	BreakpointRegistry<ADDRESS> breakpoint_registry;
	WatchpointRegistry<ADDRESS> watchpoint_registry[3]; // one registry for each of the three address spaces

	bool SetupMemory();
	bool SetupMemoryAccessReporting();
protected:
	// Debug stuff
	unisim::kernel::logger::Logger logger;
	bool verbose;
	
	// PCI configuration registers
	reg16_t pci_conf_device_id;
	reg16_t pci_conf_vendor_id;
	reg16_t pci_conf_status;
	reg16_t pci_conf_command;
	reg24_t pci_conf_class_code;
	reg8_t pci_conf_revision_id;
	reg8_t pci_conf_bist;
	reg8_t pci_conf_header_type;
	reg8_t pci_conf_latency_timer;
	reg8_t pci_conf_cache_line_size;
	reg32_t pci_conf_base_addr[NUM_REGIONS];

	reg32_t pci_conf_carbus_cis_pointer;
	reg16_t pci_conf_subsystem_id;
	reg16_t pci_conf_subsystem_vendor_id;
	
	uint32_t pci_device_number;
	ADDRESS initial_base_addr[NUM_REGIONS];
	unisim::component::cxx::pci::PCISpace address_space[NUM_REGIONS]; // SP_MEM or SP_IO
	uint32_t region_size[NUM_REGIONS]; // in bytes
	uint8_t *storage[NUM_REGIONS]; // region contents
	unsigned int pci_bus_frequency; // in Mhz
	unsigned int bus_frequency; // in Mhz

	list<typename inherited::TRAP> traps;
private:
	Parameter<bool> param_verbose;
	Parameter<bool> param_is_server;
	Parameter<unsigned int> param_protocol;
	Parameter<string> param_pipe_name;
	Parameter<string> param_server_name;
	Parameter<unsigned int> param_tcp_port;
	ParameterArray<ADDRESS> param_initial_base_addr;
	ParameterArray<unisim::component::cxx::pci::PCISpace> param_address_space;
	ParameterArray<uint32_t> param_region_size;
	Parameter<uint32_t> param_pci_device_number;
	Parameter<unsigned int> param_pci_bus_frequency;
	Parameter<unsigned int> param_bus_frequency;
};

} // end of namespace debug
} // end of namespace pci
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
