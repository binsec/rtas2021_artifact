/*
 *  Copyright (c) 2009,
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
 
#ifndef __UNISIM_UTIL_LOADER_COFF_LOADER_TI_HH__
#define __UNISIM_UTIL_LOADER_COFF_LOADER_TI_HH__

#include <inttypes.h>
#include <string>

namespace unisim {
namespace util {
namespace loader {
namespace coff_loader {
namespace ti {

template <class MEMORY_ADDR>
class FileHandler : public unisim::util::loader::coff_loader::FileHandler<MEMORY_ADDR>
{
public:
	static void Register(unisim::util::loader::coff_loader::FileHandlerRegistry<MEMORY_ADDR> *file_handler_registry);

	FileHandler(uint16_t magic);
	virtual ~FileHandler();
	
	virtual uint16_t GetMagic() const;
	virtual const char *What() const;
	virtual unisim::util::loader::coff_loader::File<MEMORY_ADDR> *GetFile() const;
private:
	uint16_t magic;
};

template <class MEMORY_ADDR>
class File : public unisim::util::loader::coff_loader::File<MEMORY_ADDR>
{
public:
	File(uint16_t magic);
	virtual ~File();

	virtual bool ParseFileHeader(const void *header);
	virtual bool ParseAoutHeader(const void *header);
	virtual bool ParseSectionHeader(unsigned int section_num, const void *section_headers);

	virtual unisim::util::endian::endian_type GetFileEndian() const;
	virtual unsigned int GetMemoryAtomSize() const;
	//virtual void GetBasicTypeSizes(MEMORY_ADDR basic_type_sizes[NUM_BASIC_TYPES]) const;
	virtual const char *GetArchitectureName() const;
	virtual bool IsExecutable() const;
	virtual unsigned int GetFileHeaderSize() const;
	virtual unsigned int GetAoutHeaderSize() const;
	virtual unsigned int GetSectionHeaderSize() const;
	virtual unsigned int GetNumSections() const;
	virtual long GetSymbolTableFilePtr() const;
	virtual unsigned long GetNumSymbols() const;
	virtual MEMORY_ADDR GetEntryPoint() const;
	virtual MEMORY_ADDR GetTextBase() const;
	virtual MEMORY_ADDR GetDataBase() const;
	virtual MEMORY_ADDR GetTextSize() const;
	virtual MEMORY_ADDR GetDataSize() const;
	virtual MEMORY_ADDR GetBssSize() const;
	virtual const SectionTable<MEMORY_ADDR> *GetSectionTable() const;
	virtual void DumpFileHeader(std::ostream& os) const;
	virtual void DumpAoutHeader(std::ostream& os) const;
private:
	uint16_t magic;
	endian_type header_endianness;
	unsigned int ti_coff_version;
	unsigned int num_sections;
	unsigned int aout_hdr_size;
	uint32_t time_date;
	uint32_t symbol_table_file_ptr;
	uint32_t num_symbols;
	uint16_t flags;
	uint16_t target_id;
	MEMORY_ADDR entry_point;
	MEMORY_ADDR text_base;
	MEMORY_ADDR data_base;
	MEMORY_ADDR text_size;
	MEMORY_ADDR data_size;
	MEMORY_ADDR bss_size;
	SectionTable<MEMORY_ADDR> *section_table;
};

template <class MEMORY_ADDR>
class Section : public unisim::util::loader::coff_loader::Section<MEMORY_ADDR>
{
public:
	Section(endian_type endianness, unsigned int ti_coff_version, const void *header);
	virtual ~Section();

	virtual const char *GetName() const;
	virtual MEMORY_ADDR GetVirtualAddress() const;
	virtual MEMORY_ADDR GetPhysicalAddress() const;
	virtual MEMORY_ADDR GetSize() const;
	virtual long GetContentFilePtr() const;
	virtual void DumpHeader(std::ostream& os) const;
	virtual typename unisim::util::loader::coff_loader::Section<MEMORY_ADDR>::Type GetType() const;
	virtual bool LoadSpecificContent(unisim::service::interfaces::Memory<MEMORY_ADDR> *output, const void *content, uint32_t size) const;
private:
	endian_type header_endianness;
	unsigned int ti_coff_version;
	std::string name;
	MEMORY_ADDR vaddr;
	MEMORY_ADDR paddr;
	MEMORY_ADDR size;
	long content_file_ptr;
	long reloc_file_ptr;
	long lineno_file_ptr;
	long num_reloc_entries;
	long num_lineno_entries;
	uint32_t flags;
	uint16_t page;
};

} // end of namespace ti
} // end of namespace coff_loader
} // end of namespace loader
} // end of namespace util
} // end of namespace unisim

#endif
