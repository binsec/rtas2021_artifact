/*
 *  Copyright (c) 2007-2015,
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
 * Authors: Réda Nouacer (reda.nouacer@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_RISC16_CPU__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_RISC16_CPU__

#include <iostream>
#include <map>
#include <vector>

#include <inttypes.h>
#include <stdio.h>

#include "unisim/kernel/logger/logger.hh"
#include <unisim/kernel/kernel.hh>

#include "unisim/service/interfaces/trap_reporting.hh"
#include "unisim/service/interfaces/disassembly.hh"
#include "unisim/service/interfaces/debug_yielding.hh"
#include "unisim/service/interfaces/memory_access_reporting.hh"
#include <unisim/service/interfaces/memory.hh>
#include "unisim/service/interfaces/registers.hh"

#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/debug/simple_register_registry.hh>
#include <unisim/component/cxx/processor/risc16/isa.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace risc16 {

using unisim::service::interfaces::TrapReporting;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::DebugYielding;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::MemoryAccessReportingType;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::Registers;

using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::Object;
using unisim::kernel::ServiceImport;
using unisim::kernel::variable::Parameter;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;

using namespace std;

class CPU
	: public Decoder
	, public Service<MemoryAccessReportingControl>
	, public Service<Memory<uint64_t> >
	, public Service<Disassembly<uint64_t> >
	, public Service<Registers>
	, public Client<DebugYielding>
	, public Client<MemoryAccessReporting<uint64_t> >
	, public Client<Memory<uint64_t> >
{

public:
	static const unsigned int INS_SIZE = 2;
	
	ServiceExport<Memory<uint64_t> > memory_export;
	ServiceExport<MemoryAccessReportingControl> memory_access_reporting_control_export;
	ServiceExport<Disassembly<uint64_t> > disasm_export;
	ServiceExport<Registers> registers_export;

	ServiceImport<DebugYielding> debug_yielding_import;
	ServiceImport<MemoryAccessReporting<uint64_t> > memory_access_reporting_import;
	ServiceImport<Memory<uint64_t> > memory_import;
	ServiceImport<TrapReporting > trap_reporting_import;

public:

	CPU(const char *name, Object *parent = 0);
	virtual ~CPU();

	void Reset();
	uint16_t fetch(uint16_t addr);

	virtual uint16_t mem_read(uint16_t addr) = 0;

	virtual void mem_write(uint16_t addr, uint16_t val) = 0;

	void step_instruction();

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================

	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	virtual void OnDisconnect();

	//=====================================================================
	//=             memory access reporting control interface methods     =
	//=====================================================================

	virtual void RequiresMemoryAccessReporting(MemoryAccessReportingType type, bool report);

	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================

	virtual void ResetMemory();

	virtual bool ReadMemory(uint64_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(uint64_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=                   DebugDisasmInterface methods                    =
	//=====================================================================

	/**
		* Returns a string with the disassembling of the instruction found
		*   at address addr.
		*
		* @param addr The address of the instruction to disassemble.
		* @param next_addr The address following the requested instruction.
		* @return The disassembling of the requested instruction address.
		*/
	virtual string Disasm(uint64_t service_addr, uint64_t &next_addr);

	//=====================================================================
	//=                   Register interface methods                      =
	//=====================================================================
	
	virtual unisim::service::interfaces::Register *GetRegister(const char *name);
	virtual void ScanRegisters( unisim::service::interfaces::RegisterScanner& scanner );

	/** registers */
	uint16_t cia;
	uint16_t nia;
	uint16_t gpr[16];
private:

	bool requires_memory_access_reporting;      //< indicates if the memory accesses require to be reported
	bool requires_fetch_instruction_reporting;  //< indicates if the fetched instructions require to be reported
	bool requires_commit_instruction_reporting; //< indicates if the committed instructions require to be reported

	// the kernel logger
	unisim::kernel::logger::Logger logger;

	unisim::util::debug::SimpleRegisterRegistry registers_registry;
	std::vector<unisim::kernel::VariableBase *> extended_registers_registry;
};

} // end of namespace risc16
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_RISC16_CPU__
