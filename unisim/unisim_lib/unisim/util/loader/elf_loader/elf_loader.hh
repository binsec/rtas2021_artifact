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
 
#ifndef __UNISIM_UTIL_LOADER_ELF_LOADER_ELF_LOADER_HH__
#define __UNISIM_UTIL_LOADER_ELF_LOADER_ELF_LOADER_HH__

#include <unisim/util/loader/elf_loader/elf32.h>
#include <unisim/util/loader/elf_loader/elf64.h>
#include <unisim/util/debug/dwarf/dwarf.hh>
#include <unisim/util/blob/blob.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/elf_symtab/elf_symtab.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>
#include <unisim/service/interfaces/data_object_lookup.hh>

#include <iosfwd>

namespace unisim {
namespace util {
namespace loader {
namespace elf_loader {

// using namespace std;
using namespace unisim::util::endian;
using unisim::util::debug::Statement;
using unisim::util::debug::elf_symtab::ELF_SymtabHandler;
using unisim::util::blob::Blob;

typedef enum
{
	OPT_FILENAME,
	OPT_FORCE_BASE_ADDR,
	OPT_FORCE_USE_VIRTUAL_ADDRESS,
	OPT_BASE_ADDR,
	OPT_DUMP_HEADERS,
	OPT_VERBOSE,
	OPT_PARSE_DWARF,
	OPT_DWARF_TO_HTML_OUTPUT_DIRECTORY,
	OPT_DWARF_TO_XML_OUTPUT_FILENAME,
	OPT_DWARF_REGISTER_NUMBER_MAPPING_FILENAME,
	OPT_DEBUG_DWARF
} Option;

template <class MEMORY_ADDR, unsigned int ElfClass, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
class ElfLoaderImpl
{
public:
	typedef Elf_Ehdr Elf_Ehdr_type;
	typedef Elf_Phdr Elf_Phdr_type;
	typedef Elf_Shdr Elf_Shdr_type;
	typedef Elf_Sym Elf_Sym_type;
	
	ElfLoaderImpl(const unisim::util::blob::Blob<MEMORY_ADDR> *blob = 0);
	virtual ~ElfLoaderImpl();
	
	bool Load();
	void ParseSymbols();
	
	void SetDebugInfoStream(std::ostream& debug_info_stream);
	void SetDebugWarningStream(std::ostream& debug_warning_stream);
	void SetDebugErrorStream(std::ostream& debug_error_stream);
	void SetRegistersInterface(unsigned int prc_num, unisim::service::interfaces::Registers *regs_if);
	void SetMemoryInterface(unsigned int prc_num, unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if);
	void SetFileName(char const* filename);

	void SetOption(Option opt, MEMORY_ADDR addr);
	void SetOption(Option opt, const char *s);
	void SetOption(Option opt, bool flag);
	
	void GetOption(Option opt, MEMORY_ADDR& addr) const;
	void GetOption(Option opt, std::string& s) const;
	void GetOption(Option opt, bool& flag) const;
	
	const unisim::util::blob::Blob<MEMORY_ADDR> *GetBlob() const;
	
	typedef typename unisim::util::debug::Symbol<MEMORY_ADDR> Symbol;

	void GetSymbols(typename std::list<const Symbol*>& lst, typename Symbol::Type type) const;
	const Symbol *FindSymbol(const char *name, MEMORY_ADDR addr, typename Symbol::Type type) const;
	const Symbol *FindSymbolByAddr(MEMORY_ADDR addr) const;
	const Symbol *FindSymbolByName(const char *name) const;
	const Symbol *FindSymbolByName(const char *name, typename Symbol::Type type) const;
	const Symbol *FindSymbolByAddr(MEMORY_ADDR addr, typename Symbol::Type type) const;
	
	const std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>& GetStatements() const;
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatement(MEMORY_ADDR addr, typename unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::FindStatementOption opt) const;
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatements(std::vector<const unisim::util::debug::Statement<MEMORY_ADDR> *> &stmts, MEMORY_ADDR addr, typename unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::FindStatementOption opt) const;
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatement(const char *filename, unsigned int lineno, unsigned int colno) const;
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatements(std::vector<const unisim::util::debug::Statement<MEMORY_ADDR> *> &stmts, const char *filename, unsigned int lineno, unsigned int colno) const;

	std::vector<MEMORY_ADDR> *GetBackTrace(unsigned int prc_num, MEMORY_ADDR pc) const;
	bool GetReturnAddress(unsigned int prc_num, MEMORY_ADDR pc, MEMORY_ADDR& ret_addr) const;
	
	unisim::util::debug::DataObject<MEMORY_ADDR> *GetDataObject(unsigned int prc_num, const char *data_object_name, const char *filename  = 0, const char *compilation_unit_name = 0) const;
	unisim::util::debug::DataObject<MEMORY_ADDR> *FindDataObject(unsigned int prc_num, const char *data_object_name, MEMORY_ADDR pc) const;
	void EnumerateDataObjectNames(std::set<std::string>& name_set, MEMORY_ADDR pc, typename unisim::service::interfaces::DataObjectLookup<MEMORY_ADDR>::Scope scope = unisim::service::interfaces::DataObjectLookup<MEMORY_ADDR>::SCOPE_BOTH_GLOBAL_AND_LOCAL) const;
	
	const unisim::util::debug::SubProgram<MEMORY_ADDR> *FindSubProgram(unsigned int prc_num, const char *subprogram_name, const char *filename = 0, const char *compilation_unit_name = 0) const;
private:
	std::ostream *debug_info_stream;
	std::ostream *debug_warning_stream;
	std::ostream *debug_error_stream;
	std::string filename;
	MEMORY_ADDR base_addr;
	bool force_base_addr;
	bool force_use_virtual_address;
	bool dump_headers;
	unisim::util::blob::Blob<MEMORY_ADDR> *blob;
	const unisim::util::blob::Blob<MEMORY_ADDR> *const_blob;
	ELF_SymtabHandler<MEMORY_ADDR, Elf_Sym> *symtab_handler;
	unisim::util::debug::dwarf::DWARF_Handler<MEMORY_ADDR> *dw_handler;
	std::vector<unisim::service::interfaces::Registers *> regs_if;
	std::vector<unisim::service::interfaces::Memory<MEMORY_ADDR> *> mem_if;
	std::string dwarf_to_html_output_directory;
	std::string dwarf_to_xml_output_filename;
	std::string dwarf_register_number_mapping_filename;
	bool verbose;
	endian_type endianness;
	bool parse_dwarf;
	bool debug_dwarf;
	std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *> no_stmts;
private:
	void DumpElfHeader(const Elf_Ehdr *hdr, std::ostream& os);
	void DumpProgramHeader(const Elf_Phdr *phdr, std::ostream& os);
	void DumpSectionHeader(const Elf_Shdr *shdr, const char *string_table, std::ostream& os);
	void DumpSymbol(const Elf_Sym *sym, const char *string_table, std::ostream& os);
	void DumpSymbolTable(const Elf_Shdr *shdr, const char *content, const char *string_table, std::ostream& os);
	static void SwapProgramHeader(Elf_Phdr *phdr);
	static void SwapSectionHeader(Elf_Shdr *shdr);
public:	
	static bool NeedEndianSwap(const Elf_Ehdr *hdr);
	static void SwapElfHeader(Elf_Ehdr *hdr);
	static Elf_Ehdr *ReadElfHeader(std::istream& is);
	static Elf_Phdr *ReadProgramHeaders(const Elf_Ehdr *hdr, std::istream& is);
	static Elf_Shdr *ReadSectionHeaders(const Elf_Ehdr *hdr, std::istream& is);
	static const Elf_Shdr *GetNextSectionHeader(const Elf_Ehdr *hdr, const Elf_Shdr *shdr);
	static char *LoadSectionHeaderStringTable(const Elf_Ehdr *hdr, const Elf_Shdr *shdr_table, std::istream& is);
	static MEMORY_ADDR GetSectionSize(const Elf_Shdr *shdr);
	static MEMORY_ADDR GetSectionAddr(const Elf_Shdr *shdr);
	static MEMORY_ADDR GetSectionType(const Elf_Shdr *shdr);
	static MEMORY_ADDR GetSectionAlignment(const Elf_Shdr *shdr);
	static MEMORY_ADDR GetSectionLink(const Elf_Shdr *shdr);
	static bool LoadSection(const Elf_Ehdr *hdr, const Elf_Shdr *shdr, void *buffer, std::istream& is);
	static MEMORY_ADDR GetSegmentType(const Elf_Phdr *phdr);
	static MEMORY_ADDR GetSegmentFlags(const Elf_Phdr *phdr);
	static MEMORY_ADDR GetSegmentMemSize(const Elf_Phdr *phdr);
	static MEMORY_ADDR GetSegmentFileSize(const Elf_Phdr *phdr);
  	MEMORY_ADDR GetSegmentAddr(const Elf_Phdr *phdr);
	static MEMORY_ADDR GetSegmentAlignment(const Elf_Phdr *phdr);
	static bool LoadSegment(const Elf_Ehdr *hdr, const Elf_Phdr *phdr, void *buffer, std::istream& is);
	static MEMORY_ADDR GetSectionFlags(const Elf_Shdr *shdr);
	static const char *GetSectionName(const Elf_Shdr *shdr, const char *string_table);
	static void DumpRawData(const void *content, MEMORY_ADDR size);
	static const char *GetArchitecture(const Elf_Ehdr *hdr);
	static uint8_t GetAddressSize(const Elf_Ehdr *hdr);
private:
	std::ostream& GetDebugInfoStream() const;
	std::ostream& GetDebugWarningStream() const;
	std::ostream& GetDebugErrorStream() const;
	unisim::service::interfaces::Registers *GetRegistersInterface(unsigned int prc_num = 0) const;
	unisim::service::interfaces::Memory<MEMORY_ADDR> *GetMemoryInterface(unsigned int prc_num = 0) const;
};

// base template traits for standard ElfLoaderImpl declination
template <class ADDRESS_TYPE, class PARAMETER_TYPE> struct StdElf {};

} // end of namespace elf_loader
} // end of namespace loader
} // end of namespace util
} // end of namespace unisim

#endif
