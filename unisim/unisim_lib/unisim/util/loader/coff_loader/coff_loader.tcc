/*
 *  Copyright (c) 2009-2018,
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
 
#ifndef __UNISIM_SERVICE_LOADER_COFF_LOADER_COFF_LOADER_TCC__
#define __UNISIM_SERVICE_LOADER_COFF_LOADER_COFF_LOADER_TCC__

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/likely/likely.hh>

#include <unisim/util/loader/coff_loader/ti/ti.hh>
#include <unisim/util/loader/coff_loader/ti/ti.tcc>

namespace unisim {
namespace util {
namespace loader {
namespace coff_loader {

using std::stringstream;
using unisim::kernel::Object;

template <class MEMORY_ADDR>
CoffLoader<MEMORY_ADDR>::CoffLoader(std::ostream& _debug_info_stream, std::ostream& _debug_warning_stream, std::ostream& _debug_error_stream, const unisim::util::blob::Blob<MEMORY_ADDR> *_blob)
	: debug_info_stream(_debug_info_stream)
	, debug_warning_stream(_debug_warning_stream)
	, debug_error_stream(_debug_error_stream)
	, filename()
	, entry_point(0)
	, top_addr(0)
	, stack_base(0)
	, dump_headers(false)
	, verbose(false)
	, blob(0)
	, const_blob(_blob)
	, symtab_handler(0)
{
	if(const_blob)
	{
		const_blob->Catch();
		ParseSymbols();
	}
	
	unisim::util::loader::coff_loader::ti::FileHandler<MEMORY_ADDR>::Register(&file_handler_registry);
}

template <class MEMORY_ADDR>
CoffLoader<MEMORY_ADDR>::~CoffLoader()
{
	if(symtab_handler)
	{
		delete symtab_handler;
		symtab_handler = 0;
	}

	if(blob)
	{
		blob->Release();
		blob = 0;
	}
	
	if(const_blob)
	{
		const_blob->Release();
		const_blob = 0;
	}
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::SetOption(Option opt, MEMORY_ADDR addr)
{
	switch(opt)
	{
		case OPT_FILENAME:
			break;
		case OPT_DUMP_HEADERS:
			break;
		default:
			break;
	}
	
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::SetOption(Option opt, const char *s)
{
	switch(opt)
	{
		case OPT_FILENAME:
			filename = s;
			break;
		default:
			break;
	}
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::SetOption(Option opt, bool flag)
{
	switch(opt)
	{
		case OPT_DUMP_HEADERS:
			dump_headers = flag;
			break;
		case OPT_VERBOSE:
			verbose = flag;
			break;
		default:
			break;
	}
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::GetOption(Option opt, MEMORY_ADDR& addr)
{
	addr = 0;
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::GetOption(Option opt, std::string& s)
{
	switch(opt)
	{
		case OPT_FILENAME:
			s = filename;
			break;
		default:
			s = std::string();
			break;
	}
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::GetOption(Option opt, bool& flag)
{
	switch(opt)
	{
		case OPT_DUMP_HEADERS:
			flag = dump_headers;
			break;
		case OPT_VERBOSE:
			flag = verbose;
			break;
		default:
			flag = false;
			break;
	}
}

template <class MEMORY_ADDR>
bool CoffLoader<MEMORY_ADDR>::Load()
{
	// File
	File<MEMORY_ADDR> *file = 0;
	
	if(blob)
	{
		blob->Release();
		blob = 0;
	}
	
	bool success = true;

	if(filename.empty())
	{
		debug_error_stream << "Don't know which executable file to load." << std::endl;
		return false;
	}
	
	std::ifstream is(filename.c_str(), std::ifstream::in | std::ifstream::binary);
	if(is.fail())
	{
		debug_error_stream << "Can't open executable \"" << filename << "\"" << std::endl;
		return false;
	}
	
	debug_info_stream << "Opening \"" << filename << "\"" << std::endl;

	if(is.seekg(0, std::ios::beg).fail())
	{
		return false;
	}

	// Read the magic number
	uint16_t magic;

	if(is.read((char *) &magic, sizeof(magic)).fail())
	{
		debug_error_stream << "Could not read file header or \"" << filename << "\" is not a COFF file." << std::endl;
		return false;
	}

	magic = unisim::util::endian::Host2LittleEndian(magic);

	FileHandler<MEMORY_ADDR> *file_handler = file_handler_registry[magic];

	if(!file_handler)
	{
		debug_error_stream << "Can't handle format of \"" << filename << "\"." << std::endl;
		debug_info_stream << "Supported formats are:" << std::endl;
		std::stringstream sstr;
		file_handler_registry.DumpFileHandlers(sstr);
		debug_info_stream << sstr.str() << std::endl;
		return false;
	}

	file = file_handler->GetFile();

	if(!file)
	{
		debug_error_stream<< file_handler->What() << " can't handle format of \"" << filename << "\"" << std::endl;
		return false;
	}

	// Read the remaining bytes in file header
	unsigned int file_hdr_size = file->GetFileHeaderSize();

	char hdr[file_hdr_size];

	memcpy(hdr, &magic, sizeof(uint16_t));
	//*(uint16_t *) hdr = magic;
	if(is.read(hdr + sizeof(magic), file_hdr_size - sizeof(magic)).fail())
	{
		debug_error_stream << "Could not read file header or \"" << filename << "\" is not a COFF file." << std::endl;
		delete file;
		file = 0;
		return false;
	}

	if(!file->ParseFileHeader(hdr))
	{
		debug_error_stream << "File header is not supported or \"" << filename << "\" is not a COFF file." << std::endl;
		delete file;
		file = 0;
		return false;
	}

	file_endianness = file->GetFileEndian();
	//file->GetBasicTypeSizes(basic_type_sizes);

	if(dump_headers)
	{
		std::stringstream sstr;
		file->DumpFileHeader(sstr);
		debug_info_stream << sstr.str() << std::endl;
	}

	debug_info_stream << "File \"" << filename << "\" has " << (file->GetFileEndian() == E_LITTLE_ENDIAN ? "little" : "big") << "-endian headers" << std::endl;
	debug_info_stream << "File \"" << filename << "\" is for " << file->GetArchitectureName() << std::endl;

	if(!file->IsExecutable())
	{
		debug_error_stream << "File \"" << filename << "\" is not an executable COFF file." << std::endl;
		delete file;
		file = 0;
		return false;
	}

	debug_info_stream << "File \"" << filename << "\" is executable" << std::endl;

	unsigned int aout_hdr_size = file->GetAoutHeaderSize();

	if(aout_hdr_size > 0)
	{
		char aout_hdr[aout_hdr_size];

		if(is.read(aout_hdr, aout_hdr_size).fail())
		{
			debug_error_stream << "Could not read optional header." << std::endl;
			delete file;
			file = 0;
			return false;
		}

		if(!file->ParseAoutHeader(aout_hdr))
		{
			debug_error_stream << "optional header is invalid or unsupported." << std::endl;
			delete file;
			file = 0;
			return false;
		}

		entry_point = file->GetEntryPoint();

		debug_info_stream << "Program entry point at 0x" << std::hex << entry_point << std::dec << std::endl;

		if(dump_headers)
		{
			std::stringstream sstr;
			file->DumpAoutHeader(sstr);
			debug_info_stream << sstr.str() << std::endl;
		}
	}
	else
	{
		debug_warning_stream << "File \"" << filename << "\" has no optional header." << std::endl;
		debug_warning_stream << "Program entry point is unknown" << std::endl;
	}

	unsigned int num_sections = file->GetNumSections();
	unsigned int shdr_size = file->GetSectionHeaderSize();
	memory_atom_size = file->GetMemoryAtomSize();
	unsigned int section_num;

	for(section_num = 0; section_num < num_sections; section_num++)
	{
		char shdr[shdr_size];

		if(is.read(shdr, shdr_size).fail())
		{
			debug_error_stream << "Can't read section headers" << std::endl;
			delete file;
			file = 0;
			return false;
		}
		
		if(!file->ParseSectionHeader(section_num, shdr))
		{
			debug_error_stream << "Section header #" << section_num << " is invalid or unsupported" << std::endl;
			delete file;
			file = 0;
			return false;
		}
	}

	blob = new unisim::util::blob::Blob<MEMORY_ADDR>();
	blob->Catch();
	
	blob->SetEntryPoint(entry_point * memory_atom_size);
	blob->SetArchitecture(file->GetArchitectureName());
	blob->SetFileEndian(file_endianness);
	blob->SetMemoryAtomSize(memory_atom_size);
	blob->SetFileFormat(unisim::util::blob::FFMT_COFF);
		
	const SectionTable<MEMORY_ADDR> *section_table = file->GetSectionTable();

	for(section_num = 0; section_num < num_sections; section_num++)
	{
		Section<MEMORY_ADDR> *section = (*section_table)[section_num];

		if(dump_headers)
		{
			std::stringstream sstr;
			section->DumpHeader(sstr);
			debug_info_stream << sstr.str() << std::endl;
		}

		typename Section<MEMORY_ADDR>::Type section_type = section->GetType();
		const char *section_name = section->GetName();
		MEMORY_ADDR section_addr = section->GetPhysicalAddress();
		MEMORY_ADDR section_size = section->GetSize();
		long section_content_file_ptr = section->GetContentFilePtr();

		void *section_content = 0;
		
		if(section_size > 0 && section_type != Section<MEMORY_ADDR>::ST_NOT_LOADABLE)
		{
			switch(section_type)
			{
				case Section<MEMORY_ADDR>::ST_LOADABLE_RAWDATA:
				case Section<MEMORY_ADDR>::ST_SPECIFIC_CONTENT:
				{
					debug_info_stream << "Loading section " << section_name << " (" << (section_size * memory_atom_size) << " bytes)" << std::endl;
					section_content = calloc(section_size, memory_atom_size);

					if(!section_content || is.seekg(section_content_file_ptr, std::ios::beg).fail() || is.read((char *) section_content, section_size * memory_atom_size).fail())
					{
						debug_error_stream << "Can't load section " << section_name << std::endl;
						if(section_content)
						{
							free(section_content);
							section_content = 0;
						}
						success = false;
					}
					break;
				}
				
				default:
					break;
			}
		}
		
		if(section_type == Section<MEMORY_ADDR>::ST_STACK)
		{
			blob->SetStackBase((MEMORY_ADDR) section_addr * memory_atom_size);
			debug_info_stream << "Stack base at 0x" << std::hex << section_addr << std::dec << std::endl;
		}

		typename unisim::util::blob::Section<MEMORY_ADDR>::Type blob_section_type = unisim::util::blob::Section<MEMORY_ADDR>::TY_UNKNOWN;
		switch(section_type)
		{
			case Section<MEMORY_ADDR>::ST_LOADABLE_RAWDATA:
			case Section<MEMORY_ADDR>::ST_SPECIFIC_CONTENT:
				blob_section_type = unisim::util::blob::Section<MEMORY_ADDR>::TY_PROGBITS;
				break;
			default:
				break;
		}
			
		typename unisim::util::blob::Section<MEMORY_ADDR>::Attribute blob_section_attr = unisim::util::blob::Section<MEMORY_ADDR>::SA_AWX; // FIXME

		typename unisim::util::blob::Section<MEMORY_ADDR> *blob_section = new unisim::util::blob::Section<MEMORY_ADDR>(
			blob_section_type,
			blob_section_attr,
			section_name,
			0,
			0,
			section_addr * memory_atom_size,
			section_size * memory_atom_size,
			section_content
		);
		
		blob->AddSection(blob_section);
		
		if(section_type == Section<MEMORY_ADDR>::ST_LOADABLE_RAWDATA)
		{
			void *segment_content = calloc(section_size, memory_atom_size);
			memcpy(segment_content, section_content, section_size * memory_atom_size);
			
			typename unisim::util::blob::Segment<MEMORY_ADDR>::Type blob_segment_type = unisim::util::blob::Segment<MEMORY_ADDR>::TY_LOADABLE;
				
			typename unisim::util::blob::Segment<MEMORY_ADDR>::Attribute blob_segment_attr = unisim::util::blob::Segment<MEMORY_ADDR>::SA_RWX; // FIXME

			typename unisim::util::blob::Segment<MEMORY_ADDR> *blob_segment = new unisim::util::blob::Segment<MEMORY_ADDR>(
				blob_segment_type,
				blob_segment_attr,
				0,
				section_addr * memory_atom_size,
				section_size * memory_atom_size,
				section_size * memory_atom_size,
				segment_content
			);
			
			blob->AddSegment(blob_segment);
		}
		else if(section_type == Section<MEMORY_ADDR>::ST_SPECIFIC_CONTENT)
		{
			// Note: ST_SPECIFIC_CONTENT desserve more attention because it is not raw data
			// For that specific content, a fake memory (implemented by class Segment) is used to catch memory write access of loader, and build one loadable segment
			typename unisim::util::blob::Segment<MEMORY_ADDR>::Type blob_segment_type = unisim::util::blob::Segment<MEMORY_ADDR>::TY_LOADABLE;
				
			typename unisim::util::blob::Segment<MEMORY_ADDR>::Attribute blob_segment_attr = unisim::util::blob::Segment<MEMORY_ADDR>::SA_RWX; // FIXME

			typename unisim::util::blob::Segment<MEMORY_ADDR> *blob_segment = new unisim::util::blob::Segment<MEMORY_ADDR>(
				blob_segment_type,
				blob_segment_attr,
				0
			);
			
			if(!section->LoadSpecificContent(blob_segment, section_content, section_size))
			{
				debug_error_stream << "Can't load specific content of section " << section_name << std::endl;
				success = false;
			}
			
			blob->AddSegment(blob_segment);
		}
	}
	
	long symtab_file_ptr = file->GetSymbolTableFilePtr();
	unsigned long num_symbols = file->GetNumSymbols();
	if(symtab_file_ptr && num_symbols)
	{
		debug_info_stream << "Loading symbol table" << std::endl;

		unsigned long symtab_size = num_symbols * sizeof(syment);

		void *symtab_content = calloc(symtab_size, 1);

		if(!symtab_content || is.seekg(symtab_file_ptr, std::ios::beg).fail() || is.read((char *) symtab_content, symtab_size).fail())
		{
			debug_error_stream << "Can't load symbol table" << std::endl;
			success = false;
			if(symtab_content) free(symtab_content);
		}
		else
		{
			debug_info_stream << "Loading string table" << std::endl;
			uint32_t string_table_size;

			if(is.read((char *) &string_table_size, sizeof(string_table_size)).fail())
			{
				debug_error_stream << "Can't load string table" << std::endl;
				success = false;
			}
			else
			{
				string_table_size = unisim::util::endian::Target2Host(file_endianness, string_table_size);
				void *string_table = calloc(string_table_size, 1);

				if(!string_table || is.read((char *) string_table + 4, string_table_size - 4).fail())
				{
					debug_error_stream << "Can't load string table" << std::endl;
					success = false;
					if(string_table) free(string_table);
				}
				else
				{
					blob->AddSection(
						new unisim::util::blob::Section<MEMORY_ADDR>(
							unisim::util::blob::Section<MEMORY_ADDR>::TY_STRTAB,
							unisim::util::blob::Section<MEMORY_ADDR>::SA_NULL,
							".strtab",
							0,
							0,
							0,
							string_table_size,
							string_table
						)
					);
					blob->AddSection(
						new unisim::util::blob::Section<MEMORY_ADDR>(
							unisim::util::blob::Section<MEMORY_ADDR>::TY_COFF_SYMTAB,
							unisim::util::blob::Section<MEMORY_ADDR>::SA_NULL,
							".symtab",
							0,
							blob->GetSections().size() - 1, // link to previously added string table
							0,
							symtab_size,
							symtab_content
						)
					);
				}
			}
		}
	}

	if(const_blob)
	{
		const_blob->Release();
		const_blob = 0;
	}
	const_blob = blob;
	if(const_blob)
	{
		const_blob->Catch();
	}
	ParseSymbols();
	
	if(file)
	{
		delete file;
	}
	
	return success;
}

template <class MEMORY_ADDR>
const unisim::util::blob::Blob<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::GetBlob() const
{
	return blob ? blob : const_blob;
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::ParseSymbols()
{
	if(symtab_handler)
	{
		delete symtab_handler;
	}

	symtab_handler = new unisim::util::debug::coff_symtab::Coff_SymtabHandler<MEMORY_ADDR>(debug_info_stream, debug_warning_stream, debug_error_stream, const_blob);
	
	if(symtab_handler)
	{
		if(unlikely(verbose))
		{
			debug_info_stream << "Building symbol table" << std::endl;
		}
		symtab_handler->Parse();
	}
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::GetSymbols(typename std::list<const unisim::util::debug::Symbol<MEMORY_ADDR> *>& lst, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	if(symtab_handler)
	{
		symtab_handler->GetSymbols(lst, type);
	}
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::FindSymbol(const char *name, MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symtab_handler ? symtab_handler->FindSymbol(name, addr, type) : 0;
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::FindSymbolByAddr(MEMORY_ADDR addr) const
{
	return symtab_handler ? symtab_handler->FindSymbolByAddr(addr) : 0;
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::FindSymbolByName(const char *name) const
{
	return symtab_handler ? symtab_handler->FindSymbolByName(name) : 0;
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symtab_handler ? symtab_handler->FindSymbolByName(name, type) : 0;
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::FindSymbolByAddr(MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symtab_handler ? symtab_handler->FindSymbolByAddr(addr, type) : 0;
}

template <class MEMORY_ADDR>
SectionTable<MEMORY_ADDR>::SectionTable(unsigned int num_sections)
	: sections(num_sections)
{
}

template <class MEMORY_ADDR>
SectionTable<MEMORY_ADDR>::~SectionTable()
{
	unsigned int section_num;
	unsigned int num_sections = sections.size();

	for(section_num = 0; section_num < num_sections; section_num++)
	{
		Section<MEMORY_ADDR> *section = sections[section_num];
		if(section) delete section;
	}
}

template <class MEMORY_ADDR>
void SectionTable<MEMORY_ADDR>::Insert(unsigned int section_num, Section<MEMORY_ADDR> *section)
{
	if(sections[section_num]) delete sections[section_num];
	sections[section_num] = section;
}

template <class MEMORY_ADDR>
Section<MEMORY_ADDR> *SectionTable<MEMORY_ADDR>::operator [] (unsigned int section_num) const
{
	return sections[section_num];
}

template <class MEMORY_ADDR>
unsigned int SectionTable<MEMORY_ADDR>::GetSize() const
{
	return sections.size();
}

template <class MEMORY_ADDR>
FileHandlerRegistry<MEMORY_ADDR>::FileHandlerRegistry()
{
	Reset();
}

template <class MEMORY_ADDR>
FileHandlerRegistry<MEMORY_ADDR>::~FileHandlerRegistry()
{
	Reset();
}

template <class MEMORY_ADDR>
void FileHandlerRegistry<MEMORY_ADDR>::Register(FileHandler<MEMORY_ADDR> *file_handler)
{
	uint16_t magic = file_handler->GetMagic();

	if(file_handlers[magic])
	{
		delete file_handlers[magic];
	}

	file_handlers[magic] = file_handler;
}

template <class MEMORY_ADDR>
FileHandler<MEMORY_ADDR> *FileHandlerRegistry<MEMORY_ADDR>::operator [] (uint16_t magic)
{
	return file_handlers[magic];
}

template <class MEMORY_ADDR>
void FileHandlerRegistry<MEMORY_ADDR>::DumpFileHandlers(std::ostream& os)
{
	typename std::map<uint16_t, FileHandler<MEMORY_ADDR> *>::iterator it;

	for(it = file_handlers.begin(); it != file_handlers.end(); it++)
	{
		FileHandler<MEMORY_ADDR> *file_handler = it->second;
		if(file_handler != 0)
			os << " - " << file_handler->What() << std::endl;
	}
}

template <class MEMORY_ADDR>
void FileHandlerRegistry<MEMORY_ADDR>::Reset()
{
	typename std::map<uint16_t, FileHandler<MEMORY_ADDR> *>::iterator it;

	for(it = file_handlers.begin(); it != file_handlers.end(); it++)
	{
		FileHandler<MEMORY_ADDR> *file_handler = it->second;
		if(file_handler) delete file_handler;
	}
	
	file_handlers.clear();
}


} // end of namespace coff_loader
} // end of namespace loader
} // end of namespace util
} // end of namespace unisim

#endif
