/*
 *  Copyright (c) 2010,
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

#ifndef __UNISIM_UTIL_BLOB_BLOB_HH__
#define __UNISIM_UTIL_BLOB_BLOB_HH__

#include <unisim/util/blob/section.hh>
#include <unisim/util/blob/segment.hh>
#include <unisim/util/endian/endian.hh>
#include <string>
#include <vector>


namespace unisim {
namespace util {
namespace blob {

using unisim::util::endian::endian_type;

typedef enum
{
	FFMT_UNKNOWN,
	FFMT_ELF32,
	FFMT_ELF64,
	FFMT_COFF
} FileFormat;

typedef enum
{
	CAP_DEFAULT = 0,
	CAP_FILENAME = 1,
	CAP_FILE_FORMAT = 2,
	CAP_ENTRY_POINT = 4,
	CAP_ARCHITECTURE = 8,
	CAP_STACK_BASE = 16,
	CAP_ENDIAN = 32,
	CAP_FILE_ENDIAN = 64,
	CAP_ADDRESS_SIZE = 128,
	CAP_MEMORY_ATOM_SIZE = 256,
	CAP_ELF_PHOFF = 512,
	CAP_ELF_PHENT = 1024,
	CAP_ELF_FLAGS = 2048,
	CAP_ELF_PHNUM = 4096
} Capability;

template <class MEMORY_ADDR>
class Blob
{
public:
	Blob();
	Blob(const Blob<MEMORY_ADDR>& blob);
	virtual ~Blob();
	
	void SetFilename(const char *filename);
	void SetFileFormat(FileFormat ffmt);
	void SetEntryPoint(MEMORY_ADDR entry_point);
	void SetArchitecture(const char *architecture);
	void SetStackBase(MEMORY_ADDR stack_base);
	void SetEndian(endian_type endian);
	void SetFileEndian(endian_type file_endian);
	void SetAddressSize(unsigned int address_size);
	void SetMemoryAtomSize(unsigned int memory_atom_size);
	void SetELF_PHOFF(uint64_t elf_phoff);
	void SetELF_PHENT(unsigned int elf_phent);
	void SetELF_PHNUM(unsigned int elf_phnum);
	void SetELF_Flags(uint32_t elf_flags);
	
	Capability GetCapability() const;
	const char *GetFilename() const;
	FileFormat GetFileFormat() const;
	MEMORY_ADDR GetEntryPoint() const;
	const char *GetArchitecture() const;
	MEMORY_ADDR GetStackBase() const;
	endian_type GetEndian() const;
	endian_type GetFileEndian() const;
	unsigned int GetAddressSize() const;
	unsigned int GetMemoryAtomSize() const;
	uint64_t GetELF_PHOFF() const;
	unsigned int GetELF_PHENT() const;
	unsigned int GetELF_PHNUM() const;
	uint32_t GetELF_Flags() const;
	const std::vector<const Blob<MEMORY_ADDR> *>& GetBlobs() const;
	const std::vector<const Section<MEMORY_ADDR> *>& GetSections() const;
	const std::vector<const Segment<MEMORY_ADDR> *>& GetSegments() const;
	const Section<MEMORY_ADDR> *GetSection(unsigned int index) const;
	const Segment<MEMORY_ADDR> *GetSegment(unsigned int index) const;
	void GetAddrRange(MEMORY_ADDR& min_addr, MEMORY_ADDR& max_addr) const;
	bool HasOverlap(MEMORY_ADDR min_addr, MEMORY_ADDR max_addr) const;
	
	void AddBlob(const Blob<MEMORY_ADDR> *blob);
	void AddSection(const Section<MEMORY_ADDR> *section);
	void AddSegment(const Segment<MEMORY_ADDR> *segment);
	
	const Section<MEMORY_ADDR> *FindSection(const char *name, bool recursive = true) const;
	const Section<MEMORY_ADDR> *FindSection(typename Section<MEMORY_ADDR>::Type section_type) const;
	
	void Catch() const;
	void Release() const;
private:
	Capability capability;
	std::string filename;
	std::string architecture;
	FileFormat ffmt;
	MEMORY_ADDR entry_point;
	MEMORY_ADDR stack_base;
	endian_type endian;
	endian_type file_endian;
	unsigned int address_size;
	unsigned int memory_atom_size;
	uint64_t elf_phoff;
	unsigned int elf_phent;
	unsigned int elf_phnum;
	uint32_t elf_flags;
	std::vector<const Blob<MEMORY_ADDR> *> blobs;
	std::vector<const Section<MEMORY_ADDR> *> sections;
	std::vector<const Segment<MEMORY_ADDR> *> segments;
	unsigned int *refcount;
};


} // end of namespace blob
} // end of namespace util
} // end of namespace unisim

#endif

