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
 
#ifndef __UNISIM_SERVICE_LOADER_COFF_LOADER_TI_TCC__
#define __UNISIM_SERVICE_LOADER_COFF_LOADER_TI_TCC__

#include <inttypes.h>
#include <iostream>
#include <assert.h>

namespace unisim {
namespace service {
namespace loader {
namespace coff_loader {
namespace ti {

// TI's IDs
const uint16_t TARGET_ID_TMS320_C1X_C2X_C5X = 0x0092;
const uint16_t TARGET_ID_TMS320_C3X_C4X = 0x0093;
const uint16_t TARGET_ID_C80 = 0x0095;
const uint16_t TARGET_ID_TMS320_C54X = 0x0098;
const uint16_t TARGET_ID_TMS320_C6X = 0x0099;
const uint16_t TARGET_ID_TMS320_C28X = 0x009d;

// TI's IDs for little-endian headers
const uint16_t LEH_TARGET_ID_TMS320_C1X_C2X_C5X = TARGET_ID_TMS320_C1X_C2X_C5X;
const uint16_t LEH_TARGET_ID_TMS320_C3X_C4X = TARGET_ID_TMS320_C3X_C4X;
const uint16_t LEH_TARGET_ID_C80 = TARGET_ID_C80;
const uint16_t LEH_TARGET_ID_TMS320_C54X = TARGET_ID_TMS320_C54X;
const uint16_t LEH_TARGET_ID_TMS320_C6X = TARGET_ID_TMS320_C6X;
const uint16_t LEH_TARGET_ID_TMS320_C28X = TARGET_ID_TMS320_C28X;

// TI's IDs for big-endian headers
const uint16_t BEH_TARGET_ID_TMS320_C1X_C2X_C5X = 0x9200;
const uint16_t BEH_TARGET_ID_TMS320_C3X_C4X = 0x9300;
const uint16_t BEH_TARGET_ID_C80 = 0x9500;
const uint16_t BEH_TARGET_ID_TMS320_C54X = 0x9800;
const uint16_t BEH_TARGET_ID_TMS320_C6X = 0x9900;
const uint16_t BEH_TARGET_ID_TMS320_C28X = 0x9d00;

// magic numbers
const uint16_t COFF_V1_MAGIC = 0x00c1;
const uint16_t COFF_V2_MAGIC = 0x00c2;
const uint16_t AOUT_MAGIC = 0x0108;

// magic numbers for little-endian headers
const uint16_t LEH_COFF_V1_MAGIC = COFF_V1_MAGIC;
const uint16_t LEH_COFF_V2_MAGIC = COFF_V2_MAGIC;
const uint16_t LEH_AOUT_MAGIC = AOUT_MAGIC;

// magic numbers for big-endian headers
const uint16_t BEH_COFF_V1_MAGIC = 0xc100;
const uint16_t BEH_COFF_V2_MAGIC = 0xc200;
const uint16_t BEH_AOUT_MAGIC = 0x0801;

// file header flags
const uint32_t F_VERS0 = 0x0000;  // TMS320C30/C31 object code
const uint32_t F_RELFLG = 0x0001; // Relocation information was stripped from the file
const uint32_t F_EXEC = 0x0002;   // The file is executable (it contains no unresolved external references)
const uint32_t F_LNNO = 0x0004;   // Line numbers were stripped from the file
const uint32_t F_LSYMS = 0x0008;  // Local symbols were stripped from the file
const uint32_t F_VERS1 = 0x0010;  // TMS320C40/C44 object code
const uint32_t F_VERS2 = 0x0020;  // TMS320C32 object code
const uint32_t F_LITTLE = 0x0100; // Object data LSB first

// section flags
const uint32_t STYP_REG = 0x0000;    // Regular section (allocated, relocated, loaded)
const uint32_t STYP_DSECT = 0x0001;  // Dummy section (relocated, not allocated, not loaded)
const uint32_t STYP_NOLOAD = 0x0002; // Noload section (allocated, relocated, not loaded)
const uint32_t STYP_GROUP = 0x0004;  // Grouped section (formed from several input sections)
const uint32_t STYP_PAD = 0x0008;    // Padding section (loaded, not allocated, not relocated)
const uint32_t STYP_COPY = 0x0010;   // Copy section (relocated, loaded, but not allocated;
                                     // relocation and line-number entries are processed normally
const uint32_t STYP_TEXT = 0x0020;   // Section that contains executable code
const uint32_t STYP_DATA = 0x0040;   // Section that contains initialized data
const uint32_t STYP_BSS = 0x0080;    // Section that contains uninitialized data
const uint32_t STYP_ALIGN = 0x0700;  // Section that is aligned on a page boundary

// TI's COFF v0 file header
typedef struct __UNISIM_PACKED__
{
	uint16_t f_magic;     // magic number
	uint16_t f_nscns;     // number of sections
	uint32_t f_timdat;    // time and date stamp
	uint32_t f_symptr;    // file ptr to symtab
	uint32_t f_nsyms;     // number of symtab entries
	uint16_t f_opthdr;    // size of optional header
	uint16_t f_flags;     // flags
} filehdr_v0;

// TI's COFF V1/V2 file header
typedef struct __UNISIM_PACKED__
{
	uint16_t f_magic;     // magic number
	uint16_t f_nscns;     // number of sections
	uint32_t f_timdat;    // time and date stamp
	uint32_t f_symptr;    // file ptr to symtab
	uint32_t f_nsyms;     // number of symtab entries
	uint16_t f_opthdr;    // size of optional header
	uint16_t f_flags;     // flags
	uint16_t f_target_id; // COFF-TI specific: TI target magic number that can execute the file
} filehdr_v12;

typedef union __UNISIM_PACKED__
{
	filehdr_v0 v0;       // V0
	filehdr_v12 v12;     // V1 or V2
} filehdr;

// TI's COFF Optional file header
typedef struct __UNISIM_PACKED__
{
	uint16_t o_magic;        // magic number
	uint16_t o_vstamp;       // version stamp
	uint32_t o_tsize;        // text size in bytes, padded to FW bdry
	uint32_t o_dsize;        // initialized data
	uint32_t o_bsize;        // uninitialized data
	uint32_t o_entry;        // entry point
	uint32_t o_text_start;   // base of text used for this file
	uint32_t o_data_start;   // base of data used for this file
} aouthdr;

// Section header for TI's COFF0 and COFF1 files
typedef struct __UNISIM_PACKED__
{
	char s_name[8];        // section name
	uint32_t s_paddr;      // physical address
	uint32_t s_vaddr;      // virtual address
	uint32_t s_size;       // section size
	uint32_t s_scnptr;     // file ptr to raw data
	uint32_t s_relptr;     // file ptr to relocation
	uint32_t s_lnnoptr;    // file ptr to line numbers
	uint16_t s_nreloc;     // number of reloc entries
	uint16_t s_nlnno;      // number of line number entries
	uint16_t s_flags;      // flags
	uint8_t s_reserved;    // reserved
	uint8_t s_page;        // memory page number */
} scnhdr_v01;

// Section header for TI's COFF2 files
typedef struct __UNISIM_PACKED__
{
	char s_name[8];        // section name
	uint32_t s_paddr;      // physical address
	uint32_t s_vaddr;      // virtual address
	uint32_t s_size;       // section size
	uint32_t s_scnptr;     // file ptr to raw data
	uint32_t s_relptr;     // file ptr to relocation
	uint32_t s_lnnoptr;    // file ptr to line numbers
	uint32_t s_nreloc;     // number of reloc entries
	uint32_t s_nlnno;      // number of line number entries
	uint32_t s_flags;      // flags
	uint16_t s_reserved;   // reserved
	uint16_t s_page;       // memory page number */
} scnhdr_v2;

template <class MEMORY_ADDR>
void FileHandler<MEMORY_ADDR>::Register(unisim::service::loader::coff_loader::FileHandlerRegistry<MEMORY_ADDR> *file_handler_registry)
{
	uint16_t magic_numbers[] = {
		LEH_TARGET_ID_TMS320_C1X_C2X_C5X,
		LEH_TARGET_ID_TMS320_C3X_C4X,
		LEH_TARGET_ID_C80,
		LEH_TARGET_ID_TMS320_C54X,
		LEH_TARGET_ID_TMS320_C6X,
		LEH_TARGET_ID_TMS320_C28X,
		LEH_COFF_V1_MAGIC,
		LEH_COFF_V2_MAGIC,
		BEH_TARGET_ID_TMS320_C1X_C2X_C5X,
		BEH_TARGET_ID_TMS320_C3X_C4X,
		BEH_TARGET_ID_C80,
		BEH_TARGET_ID_TMS320_C54X,
		BEH_TARGET_ID_TMS320_C6X,
		BEH_TARGET_ID_TMS320_C28X,
		BEH_COFF_V1_MAGIC,
		BEH_COFF_V2_MAGIC
	};

	unsigned int i;
	for(i = 0; i < sizeof(magic_numbers) / sizeof(magic_numbers[0]); i++)
	{
		file_handler_registry->Register(new FileHandler<MEMORY_ADDR>(magic_numbers[i]));
	}
}

template <class MEMORY_ADDR>
FileHandler<MEMORY_ADDR>::FileHandler(uint16_t _magic)
	: magic(_magic)
{
}

template <class MEMORY_ADDR>
FileHandler<MEMORY_ADDR>::~FileHandler()
{
}

template <class MEMORY_ADDR>
uint16_t FileHandler<MEMORY_ADDR>::GetMagic() const
{
	return magic;
}


template <class MEMORY_ADDR>
const char *FileHandler<MEMORY_ADDR>::What() const
{
	switch(magic)
	{
		case LEH_TARGET_ID_TMS320_C1X_C2X_C5X:
			return "TI COFF v0 for TMS320C1x/C2x/C5x (little-endian headers)";
		case LEH_TARGET_ID_TMS320_C3X_C4X:
			return "TI COFF v0 for TMS320C3x/C4x (little-endian headers)";
		case LEH_TARGET_ID_C80:
			return "TI COFF v0 for C80 (little-endian headers)";
		case LEH_TARGET_ID_TMS320_C54X:
			return "TI COFF v0 for TMS320C54x (little-endian headers)";
		case LEH_TARGET_ID_TMS320_C6X:
			return "TI COFF v0 for TMS320C6x (little-endian headers)";
		case LEH_TARGET_ID_TMS320_C28X:
			return "TI COFF v0 for TMS320C28x (little-endian headers)";
		case LEH_COFF_V1_MAGIC:
			return "TI COFF v1 (little-endian headers)";
		case LEH_COFF_V2_MAGIC:
			return "TI COFF v2 (little-endian headers)";
		case BEH_TARGET_ID_TMS320_C1X_C2X_C5X:
			return "TI COFF v0 for TMS320C1x/C2x/C5x (big-endian headers)";
		case BEH_TARGET_ID_TMS320_C3X_C4X:
			return "TI COFF v0 for TMS320C3x/C4x (big-endian headers)";
		case BEH_TARGET_ID_C80:
			return "TI COFF v0 for C80 (big-endian headers)";
		case BEH_TARGET_ID_TMS320_C54X:
			return "TI COFF v0 for TMS320C54x (big-endian headers)";
		case BEH_TARGET_ID_TMS320_C6X:
			return "TI COFF v0 for TMS320C6x (big-endian headers)";
		case BEH_TARGET_ID_TMS320_C28X:
			return "TI COFF v0 for TMS320C28x (big-endian headers)";
		case BEH_COFF_V1_MAGIC:
			return "TI COFF v1 (big-endian headers)";
		case BEH_COFF_V2_MAGIC:
			return "TI COFF v2 (big-endian headers)";
	}
	return "?";
}

template <class MEMORY_ADDR>
unisim::service::loader::coff_loader::File<MEMORY_ADDR> *FileHandler<MEMORY_ADDR>::GetFile() const
{
	switch(magic)
	{
		case LEH_TARGET_ID_TMS320_C1X_C2X_C5X:
		case LEH_TARGET_ID_TMS320_C3X_C4X:
		case LEH_TARGET_ID_C80:
		case LEH_TARGET_ID_TMS320_C54X:
		case LEH_TARGET_ID_TMS320_C6X:
		case LEH_TARGET_ID_TMS320_C28X:
		case LEH_COFF_V1_MAGIC:
		case LEH_COFF_V2_MAGIC:
		case BEH_TARGET_ID_TMS320_C1X_C2X_C5X:
		case BEH_TARGET_ID_TMS320_C3X_C4X:
		case BEH_TARGET_ID_C80:
		case BEH_TARGET_ID_TMS320_C54X:
		case BEH_TARGET_ID_TMS320_C6X:
		case BEH_TARGET_ID_TMS320_C28X:
		case BEH_COFF_V1_MAGIC:
		case BEH_COFF_V2_MAGIC:
			return new File<MEMORY_ADDR>(magic);
	}

	return 0;
}

template <class MEMORY_ADDR>
File<MEMORY_ADDR>::File(uint16_t _magic)
	: magic(_magic)
	, num_sections(0)
	, aout_hdr_size(0)
	, time_date(0)
	, symbol_table_file_ptr(0)
	, num_symbols(0)
	, flags(0)
	, target_id(0)
	, entry_point(0)
	, text_base(0)
	, data_base(0)
	, text_size(0)
	, data_size(0)
	, bss_size(0)
	, section_table(0)
{
	switch(magic)
	{
		case LEH_TARGET_ID_TMS320_C1X_C2X_C5X:
		case LEH_TARGET_ID_TMS320_C3X_C4X:
		case LEH_TARGET_ID_C80:
		case LEH_TARGET_ID_TMS320_C54X:
		case LEH_TARGET_ID_TMS320_C6X:
		case LEH_TARGET_ID_TMS320_C28X:
		case LEH_COFF_V1_MAGIC:
		case LEH_COFF_V2_MAGIC:
			header_endianness = E_LITTLE_ENDIAN;
			break;
		case BEH_TARGET_ID_TMS320_C1X_C2X_C5X:
		case BEH_TARGET_ID_TMS320_C3X_C4X:
		case BEH_TARGET_ID_C80:
		case BEH_TARGET_ID_TMS320_C54X:
		case BEH_TARGET_ID_TMS320_C6X:
		case BEH_TARGET_ID_TMS320_C28X:
		case BEH_COFF_V1_MAGIC:
		case BEH_COFF_V2_MAGIC:
			header_endianness = E_BIG_ENDIAN;
			break;
		default:
			assert(0 == 1);
	}

	switch(magic)
	{
		case LEH_TARGET_ID_TMS320_C1X_C2X_C5X:
		case LEH_TARGET_ID_TMS320_C3X_C4X:
		case LEH_TARGET_ID_C80:
		case LEH_TARGET_ID_TMS320_C54X:
		case LEH_TARGET_ID_TMS320_C6X:
		case LEH_TARGET_ID_TMS320_C28X:
		case BEH_TARGET_ID_TMS320_C1X_C2X_C5X:
		case BEH_TARGET_ID_TMS320_C3X_C4X:
		case BEH_TARGET_ID_C80:
		case BEH_TARGET_ID_TMS320_C54X:
		case BEH_TARGET_ID_TMS320_C6X:
		case BEH_TARGET_ID_TMS320_C28X:
			ti_coff_version = 0;
			break;
		case LEH_COFF_V1_MAGIC:
		case BEH_COFF_V1_MAGIC:
			ti_coff_version = 1;
			break;
		case LEH_COFF_V2_MAGIC:
		case BEH_COFF_V2_MAGIC:
			ti_coff_version = 2;
			break;
		default:
			assert(0 == 1);
	}
}

template <class MEMORY_ADDR>
File<MEMORY_ADDR>::~File()
{
	if(section_table) delete section_table;
}

template <class MEMORY_ADDR>
bool File<MEMORY_ADDR>::ParseFileHeader(const void *raw_data)
{
	const filehdr *hdr = (const filehdr *) raw_data;

	assert(magic == hdr->v0.f_magic);
	
	num_sections = unisim::util::endian::Target2Host(header_endianness, hdr->v0.f_nscns);
	section_table = new SectionTable<MEMORY_ADDR>(num_sections);
	time_date = unisim::util::endian::Target2Host(header_endianness, hdr->v0.f_timdat);
	aout_hdr_size = unisim::util::endian::Target2Host(header_endianness, hdr->v0.f_opthdr);
	symbol_table_file_ptr = unisim::util::endian::Target2Host(header_endianness, hdr->v0.f_symptr);
	num_symbols = unisim::util::endian::Target2Host(header_endianness, hdr->v0.f_nsyms);
	flags = unisim::util::endian::Target2Host(header_endianness, hdr->v0.f_flags);

	switch(ti_coff_version)
	{
		case 0:
			target_id = 0;
			return true;
		case 1:
		case 2:
			target_id = unisim::util::endian::Target2Host(header_endianness, hdr->v12.f_target_id);

			switch(target_id)
			{
				case TARGET_ID_TMS320_C1X_C2X_C5X:
				case TARGET_ID_TMS320_C3X_C4X:
				case TARGET_ID_C80:
				case TARGET_ID_TMS320_C54X:
				case TARGET_ID_TMS320_C6X:
				case TARGET_ID_TMS320_C28X:
					return true;
			}
	}

	return false;
}

template <class MEMORY_ADDR>
bool File<MEMORY_ADDR>::ParseAoutHeader(const void *raw_data)
{
	const aouthdr *hdr = (const aouthdr *) raw_data;
	if(unisim::util::endian::Target2Host(header_endianness, hdr->o_magic) != AOUT_MAGIC) return false;
	entry_point = unisim::util::endian::Target2Host(header_endianness, hdr->o_entry);
	text_base = unisim::util::endian::Target2Host(header_endianness, hdr->o_text_start);
	data_base = unisim::util::endian::Target2Host(header_endianness, hdr->o_data_start);
	text_size = unisim::util::endian::Target2Host(header_endianness, hdr->o_tsize);
	data_size = unisim::util::endian::Target2Host(header_endianness, hdr->o_dsize);
	bss_size = unisim::util::endian::Target2Host(header_endianness, hdr->o_bsize);
	return true;
}

template <class MEMORY_ADDR>
bool File<MEMORY_ADDR>::ParseSectionHeader(unsigned int section_num, const void *shdr_raw_data)
{
	section_table->Insert(section_num, new Section<MEMORY_ADDR>(header_endianness, ti_coff_version, shdr_raw_data));
	return true;
}

template <class MEMORY_ADDR>
unisim::util::endian::endian_type File<MEMORY_ADDR>::GetFileEndian() const
{
	return header_endianness;
}

template <class MEMORY_ADDR>
unsigned int File<MEMORY_ADDR>::GetMemoryAtomSize() const
{
	return 4;
}

template <class MEMORY_ADDR>
void File<MEMORY_ADDR>::GetBasicTypeSizes(MEMORY_ADDR basic_type_sizes[NUM_BASIC_TYPES]) const
{
	basic_type_sizes[T_NULL] = 0;
	basic_type_sizes[T_VOID] = 0;
	basic_type_sizes[T_CHAR] = 1;
	basic_type_sizes[T_SHORT] = 1;
	basic_type_sizes[T_INT] = 1;
	basic_type_sizes[T_LONG] = 1;
	basic_type_sizes[T_FLOAT] = 1;
	basic_type_sizes[T_DOUBLE] = 1;
	basic_type_sizes[T_STRUCT] = 1;
	basic_type_sizes[T_UNION] = 1;
	basic_type_sizes[T_ENUM] = 1;
	basic_type_sizes[T_MOE] = 1;
	basic_type_sizes[T_UCHAR] = 1;
	basic_type_sizes[T_USHORT] = 1;
	basic_type_sizes[T_UINT] = 1;
	basic_type_sizes[T_ULONG] = 1;
}

template <class MEMORY_ADDR>
const char *File<MEMORY_ADDR>::GetArchitectureName() const
{
	switch(magic)
	{
		case LEH_TARGET_ID_TMS320_C1X_C2X_C5X:
		case BEH_TARGET_ID_TMS320_C1X_C2X_C5X:
			return "TI TMS320C1x/C2x/C5x";
		case LEH_TARGET_ID_TMS320_C3X_C4X:
		case BEH_TARGET_ID_TMS320_C3X_C4X:
			return "TI TMS320C3x/C4x";
		case LEH_TARGET_ID_C80:
		case BEH_TARGET_ID_C80:
			return "TI C80";
		case LEH_TARGET_ID_TMS320_C54X:
		case BEH_TARGET_ID_TMS320_C54X:
			return "TI TMS320C54x";
		case LEH_TARGET_ID_TMS320_C6X:
		case BEH_TARGET_ID_TMS320_C6X:
			return "TI TMS320C6x";
		case LEH_TARGET_ID_TMS320_C28X:
		case BEH_TARGET_ID_TMS320_C28X:
			return "TI TMS320C28x";
		case LEH_COFF_V1_MAGIC:
		case BEH_COFF_V1_MAGIC:
		case LEH_COFF_V2_MAGIC:
		case BEH_COFF_V2_MAGIC:
			switch(target_id)
			{
				case TARGET_ID_TMS320_C1X_C2X_C5X:
					return "TI TMS320C1x/C2x/C5x";
				case TARGET_ID_TMS320_C3X_C4X:
					return "TI TMS320C3x/C4x";
				case TARGET_ID_C80:
					return "TI C80";
				case TARGET_ID_TMS320_C54X:
					return "TI TMS320C54x";
				case TARGET_ID_TMS320_C6X:
					return "TI TMS320C6x";
				case TARGET_ID_TMS320_C28X:
					return "TI TMS320C28x";
			}
			break;
	}

	return "?";
}


template <class MEMORY_ADDR>
bool File<MEMORY_ADDR>::IsExecutable() const
{
	return flags & F_EXEC;
}

template <class MEMORY_ADDR>
unsigned int File<MEMORY_ADDR>::GetFileHeaderSize() const
{
	switch(ti_coff_version)
	{
		case 0:
			return sizeof(filehdr_v0);
		case 1:
		case 2:
			return  sizeof(filehdr_v12);
	}
	return 0;
}

template <class MEMORY_ADDR>
unsigned int File<MEMORY_ADDR>::GetAoutHeaderSize() const
{
	return aout_hdr_size;
}

template <class MEMORY_ADDR>
unsigned int File<MEMORY_ADDR>::GetSectionHeaderSize() const
{
	switch(ti_coff_version)
	{
		case 0:
		case 1:
			return sizeof(scnhdr_v01);
		case 2:
			return sizeof(scnhdr_v2);
	}
	return 0;
}

template <class MEMORY_ADDR>
unsigned int File<MEMORY_ADDR>::GetNumSections() const
{
	return num_sections;
}

template <class MEMORY_ADDR>
long File<MEMORY_ADDR>::GetSymbolTableFilePtr() const
{
	return symbol_table_file_ptr;
}

template <class MEMORY_ADDR>
unsigned long File<MEMORY_ADDR>::GetNumSymbols() const
{
	return num_symbols;
}

template <class MEMORY_ADDR>
MEMORY_ADDR File<MEMORY_ADDR>::GetEntryPoint() const
{
	return entry_point;
}

template <class MEMORY_ADDR>
MEMORY_ADDR File<MEMORY_ADDR>::GetTextBase() const
{
	return text_base;
}

template <class MEMORY_ADDR>
MEMORY_ADDR File<MEMORY_ADDR>::GetDataBase() const
{
	return data_base;
}

template <class MEMORY_ADDR>
MEMORY_ADDR File<MEMORY_ADDR>::GetTextSize() const
{
	return text_size;
}

template <class MEMORY_ADDR>
MEMORY_ADDR File<MEMORY_ADDR>::GetDataSize() const
{
	return data_size;
}

template <class MEMORY_ADDR>
MEMORY_ADDR File<MEMORY_ADDR>::GetBssSize() const
{
	return bss_size;
}

template <class MEMORY_ADDR>
const SectionTable<MEMORY_ADDR> *File<MEMORY_ADDR>::GetSectionTable() const
{
	return section_table;
}

template <class MEMORY_ADDR>
void File<MEMORY_ADDR>::DumpFileHeader(ostream& os) const
{
	os << " --- File Header ---";
	os << endl << "Magic number: 0x" << hex << magic << dec << " / TI COFF v" << ti_coff_version << " (" << (header_endianness == E_LITTLE_ENDIAN ? "little-endian" : "big-endian") << " headers)";
	os << endl << "Number of sections: " << num_sections;
	os << endl << "Time and date: " << time_date;
	os << endl << "Symbol table file pointer: " << symbol_table_file_ptr;
	os << endl << "Number of symbols: " << num_symbols;
	os << endl << "Flags: 0x" << hex << flags << dec;
	if(ti_coff_version > 0) os << endl << "Target id: " << target_id;
	os << endl;
}

template <class MEMORY_ADDR>
void File<MEMORY_ADDR>::DumpAoutHeader(ostream& os) const
{
	os << " --- Optional Header ---";
	os << endl << "Entry point: 0x" << hex << entry_point << dec;
	os << endl << "Text base: 0x" << hex << text_base << dec;
	os << endl << "Data base: 0x" << hex << data_base << dec;
	os << endl << "Text size: " << text_size << " 32-bit words";
	os << endl << "Data size: " << data_size << " 32-bit words";
	os << endl << "Bss size: " << bss_size << " 32-bit words";
	os << endl;
}

template <class MEMORY_ADDR>
Section<MEMORY_ADDR>::Section(endian_type _header_endianness, unsigned _ti_coff_version, const void *hdr_raw_data)
	: header_endianness(_header_endianness)
	, ti_coff_version(_ti_coff_version)
{
	switch(ti_coff_version)
	{
		case 0:
		case 1:
			{
				const scnhdr_v01 *hdr = (const scnhdr_v01 *) hdr_raw_data;

				char buffer[sizeof(hdr->s_name) + 1];
				memcpy(buffer, hdr->s_name, sizeof(hdr->s_name));
				buffer[sizeof(hdr->s_name)] = 0;
				name = buffer;

				vaddr = unisim::util::endian::Target2Host(header_endianness, hdr->s_vaddr);
				paddr = unisim::util::endian::Target2Host(header_endianness, hdr->s_paddr);
				size = unisim::util::endian::Target2Host(header_endianness, hdr->s_size);
				content_file_ptr = unisim::util::endian::Target2Host(header_endianness, hdr->s_scnptr);
				reloc_file_ptr = unisim::util::endian::Target2Host(header_endianness, hdr->s_relptr);
				lineno_file_ptr = unisim::util::endian::Target2Host(header_endianness, hdr->s_lnnoptr);
				num_reloc_entries = unisim::util::endian::Target2Host(header_endianness, hdr->s_nreloc);
				num_lineno_entries = unisim::util::endian::Target2Host(header_endianness, hdr->s_nlnno);
				flags = unisim::util::endian::Target2Host(header_endianness, hdr->s_flags);
				page = unisim::util::endian::Target2Host(header_endianness, hdr->s_page);
			}
			break;
		case 2:
			{
				const scnhdr_v2 *hdr = (const scnhdr_v2 *) hdr_raw_data;

				char buffer[sizeof(hdr->s_name) + 1];
				memcpy(buffer, hdr->s_name, sizeof(hdr->s_name));
				buffer[sizeof(hdr->s_name)] = 0;
				name = buffer;

				vaddr = unisim::util::endian::Target2Host(header_endianness, hdr->s_vaddr);
				paddr = unisim::util::endian::Target2Host(header_endianness, hdr->s_paddr);
				size = unisim::util::endian::Target2Host(header_endianness, hdr->s_size);
				content_file_ptr = unisim::util::endian::Target2Host(header_endianness, hdr->s_scnptr);
				reloc_file_ptr = unisim::util::endian::Target2Host(header_endianness, hdr->s_relptr);
				lineno_file_ptr = unisim::util::endian::Target2Host(header_endianness, hdr->s_lnnoptr);
				num_reloc_entries = unisim::util::endian::Target2Host(header_endianness, hdr->s_nreloc);
				num_lineno_entries = unisim::util::endian::Target2Host(header_endianness, hdr->s_nlnno);
				flags = unisim::util::endian::Target2Host(header_endianness, hdr->s_flags);
				page = unisim::util::endian::Target2Host(header_endianness, hdr->s_page);
			}
			break;
	}
}

template <class MEMORY_ADDR>
Section<MEMORY_ADDR>::~Section()
{
}

template <class MEMORY_ADDR>
const char *Section<MEMORY_ADDR>::GetName() const
{
	return name.c_str();
}

template <class MEMORY_ADDR>
MEMORY_ADDR Section<MEMORY_ADDR>::GetVirtualAddress() const
{
	return vaddr;
}

template <class MEMORY_ADDR>
MEMORY_ADDR Section<MEMORY_ADDR>::GetPhysicalAddress() const
{
	return paddr;
}

template <class MEMORY_ADDR>
MEMORY_ADDR Section<MEMORY_ADDR>::GetSize() const
{
	return size;
}

template <class MEMORY_ADDR>
long Section<MEMORY_ADDR>::GetContentFilePtr() const
{
	return content_file_ptr;
}

template <class MEMORY_ADDR>
typename unisim::service::loader::coff_loader::Section<MEMORY_ADDR>::Type Section<MEMORY_ADDR>::GetType() const
{
	if((flags & (STYP_DATA | STYP_TEXT)))
	{
		if((flags & STYP_COPY) && name == string(".cinit"))
			return unisim::service::loader::coff_loader::Section<MEMORY_ADDR>::ST_SPECIFIC_CONTENT;
		else
			return unisim::service::loader::coff_loader::Section<MEMORY_ADDR>::ST_LOADABLE_RAWDATA;
	}
	if((flags & STYP_BSS) && name == string(".stack"))
	{
		return unisim::service::loader::coff_loader::Section<MEMORY_ADDR>::ST_STACK;
	}
	return unisim::service::loader::coff_loader::Section<MEMORY_ADDR>::ST_NOT_LOADABLE;
}

template <class MEMORY_ADDR>
void Section<MEMORY_ADDR>::DumpHeader(ostream& os) const
{
	os << " --- Section Header  (TI COFF v" << ti_coff_version << ") ---";
	os << endl << "Section name: " << name;
	os << endl << "Physical address: 0x" << hex << paddr << dec;
	os << endl << "Virtual address: 0x" << hex << vaddr << dec;
	os << endl << "Section size: " << size << " 32-bit words";
	os << endl << "File pointer to content: " << content_file_ptr;
	os << endl << "File pointer to relocation: " << reloc_file_ptr;
	os << endl << "File pointer to line number: " << lineno_file_ptr;
	os << endl << "Number of relocation entries: " << num_reloc_entries;
	os << endl << "Number of line number entries: " << num_lineno_entries;
	os << endl << "Flags: 0x" << hex << flags << dec;
	os << endl << "Memory page number: " << page;
	os << endl;
}

template <class MEMORY_ADDR>
bool Section<MEMORY_ADDR>::LoadSpecificContent(OutputInterface<MEMORY_ADDR> *output, const void *content, uint32_t size) const
{
	if((flags & STYP_COPY) && name == string(".cinit"))
	{
		uint32_t *record = (uint32_t *) content;
		uint32_t nrecords = size;
		uint32_t nwords = 0;
		uint32_t addr = 0;
		int state = 0;

		// Note: Section .cinit has endianness of the target (i.e. little endian), not the endianness of the headers (little endian or big endian)
		while(nrecords > 0)
		{
			switch(state)
			{
				case 0:
					nwords = unisim::util::endian::LittleEndian2Host(*record++);
					nrecords--;
					state = 1;
					break;
				case 1:
					addr = unisim::util::endian::LittleEndian2Host(*record++);
					nrecords--;
					state = 2;
					break;
				case 2:
					// Do a bare copy of the record into memory, as the record has already the endianness of the target
					if(!output->Write(addr++, record++, 1)) return false;
					nrecords--;
					if(--nwords == 0) state = 0;
					break;
			}
		}
		return true;
	}

	return false;
}

} // end of namespace ti
} // end of namespace coff_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
