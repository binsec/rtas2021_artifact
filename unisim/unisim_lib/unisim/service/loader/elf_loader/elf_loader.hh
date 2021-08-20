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
 
#ifndef __UNISIM_SERVICE_LOADER_ELF_LOADER_ELF_LOADER_HH__
#define __UNISIM_SERVICE_LOADER_ELF_LOADER_ELF_LOADER_HH__

#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/blob.hh>
#include <unisim/service/interfaces/backtrace.hh>

#include <unisim/util/loader/elf_loader/elf_loader.hh>

#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/symbol_table.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>

#include <iosfwd>

namespace unisim {
namespace service {
namespace loader {
namespace elf_loader {

// using namespace std;
using unisim::service::interfaces::Memory;
using namespace unisim::util::endian;
using unisim::util::debug::Statement;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::Object;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::variable::Parameter;
using unisim::util::debug::Symbol;
using unisim::util::debug::SymbolTable;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::StatementLookup;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::Blob;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::BackTrace;

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
class ElfLoaderImpl :
	public Client<Memory<MEMORY_ADDR> >,
	public Client<Registers>,
	public Service<Loader>,
	public Service<Blob<MEMORY_ADDR> >,
	public Service<SymbolTableLookup<MEMORY_ADDR> >,
	public Service<StatementLookup<MEMORY_ADDR> >,
	public Service<BackTrace<MEMORY_ADDR> >
{
public:
	ServiceImport<Memory<MEMORY_ADDR> > memory_import;
	ServiceImport<Registers> registers_import;
	ServiceExport<SymbolTableLookup<MEMORY_ADDR> > symbol_table_lookup_export;
	ServiceExport<Loader> loader_export;
	ServiceExport<Blob<MEMORY_ADDR> > blob_export;
	ServiceExport<StatementLookup<MEMORY_ADDR> > stmt_lookup_export;
	ServiceExport<BackTrace<MEMORY_ADDR> > backtrace_export;

	ElfLoaderImpl(const char *name, Object *parent = 0);
	virtual ~ElfLoaderImpl();

	virtual void OnDisconnect();
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	// unisim::service::interfaces::Loader
	virtual bool Load();
	
	// unisim::service::interfaces::Blob
	virtual const unisim::util::blob::Blob<MEMORY_ADDR> *GetBlob() const;

	// unisim::service::interfaces::SymbolTableLookup
	virtual void GetSymbols(typename std::list<const unisim::util::debug::Symbol<MEMORY_ADDR> *>& lst, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	virtual const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbol(const char *name, MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	virtual const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByAddr(MEMORY_ADDR addr) const;
	virtual const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByName(const char *name) const;
	virtual const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	virtual const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByAddr(MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	
	// unisim::service::interfaces::StatementLookup
	virtual void GetStatements(std::multimap<MEMORY_ADDR, const unisim::util::debug::Statement<MEMORY_ADDR> *>& stmts) const;
	virtual const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatement(MEMORY_ADDR addr, typename unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::FindStatementOption opt) const;
	virtual const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatements(std::vector<const unisim::util::debug::Statement<MEMORY_ADDR> *> &stmts, MEMORY_ADDR addr, typename unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::FindStatementOption opt) const;
	virtual const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatement(const char *filename, unsigned int lineno, unsigned int colno) const;
	virtual const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatements(std::vector<const unisim::util::debug::Statement<MEMORY_ADDR> *> &stmts, const char *filename, unsigned int lineno, unsigned int colno) const;

	// unisim::service::interfaces::BackTrace
	virtual std::vector<MEMORY_ADDR> *GetBackTrace(MEMORY_ADDR pc) const;
	virtual bool GetReturnAddress(MEMORY_ADDR pc, MEMORY_ADDR& ret_addr) const;
private:
	unisim::util::loader::elf_loader::ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym> *elf_loader;
	std::string filename;
	MEMORY_ADDR base_addr;
	bool force_base_addr;
	bool force_use_virtual_address;
	bool initialize_extra_segment_bytes;
	bool dump_headers;
	std::string dwarf_to_html_output_directory;
	std::string dwarf_to_xml_output_filename;
	std::string dwarf_register_number_mapping_filename;
	unisim::kernel::logger::Logger logger;
	bool verbose;
	endian_type endianness;
	bool parse_dwarf;
	bool debug_dwarf;
	
	Parameter<std::string> param_filename;
	Parameter<MEMORY_ADDR> param_base_addr;
	Parameter<bool> param_force_base_addr;
	Parameter<bool> param_force_use_virtual_address;
	Parameter<bool> param_initialize_extra_segment_bytes;
	Parameter<bool> param_dump_headers;
	Parameter<bool> param_verbose;
	Parameter<std::string> param_dwarf_to_html_output_directory;
	Parameter<std::string> param_dwarf_to_xml_output_filename;
	Parameter<std::string> param_dwarf_register_number_mapping_filename;
	Parameter<bool> param_parse_dwarf;
	Parameter<bool> param_debug_dwarf;
};

} // end of namespace elf_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
