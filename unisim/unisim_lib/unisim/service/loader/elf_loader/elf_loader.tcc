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
 
#ifndef __UNISIM_SERVICE_LOADER_ELF_LOADER_ELF_LOADER_TCC__
#define __UNISIM_SERVICE_LOADER_ELF_LOADER_ELF_LOADER_TCC__

#include <unisim/util/endian/endian.hh>
#include <unisim/util/likely/likely.hh>
#include <iostream>
#include <fstream>
#include <stdlib.h>

namespace unisim {
namespace service {
namespace loader {
namespace elf_loader {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::ElfLoaderImpl(const char *name, Object *parent)
	: Object(name, parent, "this service implements an ELF Loader")
	, Client<Memory<MEMORY_ADDR> >(name, parent)
	, Client<Registers>(name, parent)
	, Service<Loader>(name, parent)
	, Service<Blob<MEMORY_ADDR> >(name, parent)
	, Service<SymbolTableLookup<MEMORY_ADDR> >(name, parent)
	, Service<StatementLookup<MEMORY_ADDR> >(name, parent)
	, Service<BackTrace<MEMORY_ADDR> >(name, parent)
	, memory_import("memory-import", this)
	, registers_import("registers-import", this)
	, symbol_table_lookup_export("symbol-table-lookup-export", this)
	, loader_export("loader-export", this)
	, blob_export("blob-export", this)
	, stmt_lookup_export("stmt-lookup-export", this)
	, backtrace_export("backtrace-export", this)
	, elf_loader(0)
	, filename()
	, base_addr(0)
	, force_base_addr(false)
	, force_use_virtual_address(false)
	, initialize_extra_segment_bytes(true)
	, dump_headers(false)
	, logger(*this)
	, verbose(false)
	, endianness(E_LITTLE_ENDIAN)
	, parse_dwarf(false)
	, debug_dwarf(false)
	, param_filename("filename", this, filename,
			"the ELF filename to load into memory")
	, param_base_addr("base-addr", this, base_addr, 
			"if force-base-addr is true"
			" force base address for a unique program segment,"
			" otherwise ignored")
	, param_force_base_addr("force-base-addr", this, force_base_addr,
			"if true force base address for a unique program segment")
	, param_force_use_virtual_address("force-use-virtual-address", this, 
			force_use_virtual_address, 
			"force use of virtual addresses instead of physical addresses (default false)")
	, param_initialize_extra_segment_bytes("initialize-extra-segment-bytes", this, initialize_extra_segment_bytes, "whether to initialize extra bytes in segments (p_filesz < p_memsz) to zero (true for standard ELF files)")
	, param_dump_headers("dump-headers", this, dump_headers, 
			"dump headers while loading ELF file")
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, param_dwarf_to_html_output_directory("dwarf-to-html-output-directory", 
			this, dwarf_to_html_output_directory, 
			"DWARF v2/v3 to HTML output directory")
	, param_dwarf_to_xml_output_filename("dwarf-to-xml-output-filename", this, dwarf_to_xml_output_filename, "DWARF v2/v3 to XML output filename")
	, param_dwarf_register_number_mapping_filename("dwarf-register-number-mapping-filename", this, dwarf_register_number_mapping_filename, "DWARF register number mapping filename")
	, param_parse_dwarf("parse-dwarf", this, parse_dwarf,
			"Enable/Disable parsing of DWARF debugging informations")
	, param_debug_dwarf("debug-dwarf", this, debug_dwarf,
			"Enable/Disable debugging of DWARF")
{
	loader_export.SetupDependsOn(memory_import);
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::~ElfLoaderImpl()
{
	if(elf_loader)
	{
		delete elf_loader;
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::OnDisconnect()
{
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
bool ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::Load()
{
	if(!memory_import) return false;

	const unisim::util::blob::Blob<MEMORY_ADDR> *blob = elf_loader->GetBlob();
	if(!blob) return false;
	
	bool success = true;
	const typename std::vector<const unisim::util::blob::Segment<MEMORY_ADDR> *>& segments = blob->GetSegments();
	typename std::vector<const unisim::util::blob::Segment<MEMORY_ADDR> *>::const_iterator segment_iter;
	for(segment_iter = segments.begin(); segment_iter != segments.end(); segment_iter++)
	{
		const unisim::util::blob::Segment<MEMORY_ADDR> *segment = *segment_iter;
		
		if(segment->GetType() == unisim::util::blob::Segment<MEMORY_ADDR>::TY_LOADABLE)
		{
			MEMORY_ADDR write_size = initialize_extra_segment_bytes ? segment->GetSize() : segment->GetDataSize();
			if (write_size) {
				if(unlikely(verbose))
				{
					logger << DebugInfo << "Writing segment (" << write_size << " bytes) at @0x" << std::hex << segment->GetAddr() << " - @0x" << (segment->GetAddr() +  write_size - 1) << std::dec << EndDebugInfo;
				}

				if(!memory_import->WriteMemory(segment->GetAddr(), segment->GetData(), write_size))
				{
					logger << DebugError << "Can't write into memory (@0x" << std::hex << segment->GetAddr() << " - @0x" << (segment->GetAddr() +  write_size - 1) << std::dec << ")" << EndDebugError;
					success = false;
				}
			}
		}
	}
	return success;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
bool ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::BeginSetup()
{
	if(elf_loader)
	{
		delete elf_loader;
		elf_loader = 0;
	}

	elf_loader = new unisim::util::loader::elf_loader::ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>();

	elf_loader->SetDebugInfoStream(logger.DebugInfoStream());
	elf_loader->SetDebugWarningStream(logger.DebugWarningStream());
	elf_loader->SetDebugErrorStream(logger.DebugErrorStream());
	elf_loader->SetRegistersInterface(/* prc_num */ 0, registers_import);
	elf_loader->SetMemoryInterface(/* prc_num */ 0, memory_import);
	elf_loader->SetOption(unisim::util::loader::elf_loader::OPT_FILENAME, Object::GetSimulator()->SearchSharedDataFile(filename.c_str()).c_str());
	elf_loader->SetOption(unisim::util::loader::elf_loader::OPT_FORCE_BASE_ADDR, force_base_addr);
	elf_loader->SetOption(unisim::util::loader::elf_loader::OPT_FORCE_USE_VIRTUAL_ADDRESS, force_use_virtual_address);
	elf_loader->SetOption(unisim::util::loader::elf_loader::OPT_BASE_ADDR, base_addr);
	elf_loader->SetOption(unisim::util::loader::elf_loader::OPT_DUMP_HEADERS, dump_headers);
	elf_loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose);
	elf_loader->SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf);
	elf_loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_TO_HTML_OUTPUT_DIRECTORY, dwarf_to_html_output_directory.c_str());
	elf_loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_TO_XML_OUTPUT_FILENAME, dwarf_to_xml_output_filename.c_str());
	elf_loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_REGISTER_NUMBER_MAPPING_FILENAME, Object::GetSimulator()->SearchSharedDataFile(dwarf_register_number_mapping_filename.c_str()).c_str());
	elf_loader->SetOption(unisim::util::loader::elf_loader::OPT_DEBUG_DWARF, debug_dwarf);
	
	return elf_loader->Load();
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
bool ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::Setup(ServiceExportBase *srv_export)
{
	if(srv_export == &symbol_table_lookup_export) return elf_loader != 0;
	if(srv_export == &loader_export) return elf_loader != 0;
	if(srv_export == &blob_export) return elf_loader != 0;
	if(srv_export == &stmt_lookup_export) return elf_loader != 0;
	if(srv_export == &backtrace_export) return elf_loader != 0;

	logger << DebugError << "Internal Error" << EndDebugError;
	return false;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
bool ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::EndSetup()
{
	if(memory_import)
	{
		return Load();
	}
	return true;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::blob::Blob<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetBlob() const
{
	return elf_loader ? elf_loader->GetBlob() : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetSymbols(typename std::list<const unisim::util::debug::Symbol<MEMORY_ADDR> *>& lst, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	if(elf_loader)
	{
		elf_loader->GetSymbols(lst, type);
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindSymbol(const char *name, MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return elf_loader ? elf_loader->FindSymbol(name, addr, type) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindSymbolByAddr(MEMORY_ADDR addr) const
{
	return elf_loader ? elf_loader->FindSymbolByAddr(addr) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindSymbolByName(const char *name) const
{
	return elf_loader ? elf_loader->FindSymbolByName(name) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return elf_loader ? elf_loader->FindSymbolByName(name, type) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindSymbolByAddr(MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return elf_loader ? elf_loader->FindSymbolByAddr(addr, type) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetStatements(std::multimap<MEMORY_ADDR, const unisim::util::debug::Statement<MEMORY_ADDR> *>& stmts) const
{
	if(!elf_loader) return;
	const typename std::multimap<MEMORY_ADDR, const unisim::util::debug::Statement<MEMORY_ADDR> *>& elf_loader_stmts = elf_loader->GetStatements();
	typename std::multimap<MEMORY_ADDR, const unisim::util::debug::Statement<MEMORY_ADDR> *>::const_iterator iter;
	for(iter = elf_loader_stmts.begin(); iter != elf_loader_stmts.end(); iter++)
	{
		stmts.insert(std::pair<MEMORY_ADDR, const unisim::util::debug::Statement<MEMORY_ADDR> *>((*iter).first, (*iter).second));
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const unisim::util::debug::Statement<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindStatement(MEMORY_ADDR addr, typename unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::FindStatementOption opt) const
{
	return elf_loader ? elf_loader->FindStatement(addr, opt) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const unisim::util::debug::Statement<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindStatements(std::vector<const unisim::util::debug::Statement<MEMORY_ADDR> *> &stmts, MEMORY_ADDR addr, typename unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::FindStatementOption opt) const
{
	return elf_loader ? elf_loader->FindStatements(stmts, addr, opt) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const unisim::util::debug::Statement<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindStatement(const char *filename, unsigned int lineno, unsigned int colno) const
{
	return elf_loader ? elf_loader->FindStatement(filename, lineno, colno) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const unisim::util::debug::Statement<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindStatements(std::vector<const unisim::util::debug::Statement<MEMORY_ADDR> *> &stmts, const char *filename, unsigned int lineno, unsigned int colno) const
{
	return elf_loader ? elf_loader->FindStatements(stmts, filename, lineno, colno) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
std::vector<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetBackTrace(MEMORY_ADDR pc) const
{
	return elf_loader ? elf_loader->GetBackTrace(/* prc_num */ 0, pc) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
bool ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetReturnAddress(MEMORY_ADDR pc, MEMORY_ADDR& ret_addr) const
{
	return elf_loader ? elf_loader->GetReturnAddress(/* prc_num */ 0, pc, ret_addr) : false;
}

} // end of namespace elf_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
