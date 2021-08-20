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

#ifndef __UNISIM_UTIL_BLOB_BLOB_TCC__
#define __UNISIM_UTIL_BLOB_BLOB_TCC__

#include <iostream>

namespace unisim {
namespace util {
namespace blob {

using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;

template <class MEMORY_ADDR>
Blob<MEMORY_ADDR>::Blob()
	: capability(CAP_DEFAULT)
	, filename()
	, architecture()
	, entry_point(0)
	, stack_base(0)
	, endian(E_BIG_ENDIAN)
	, file_endian(E_BIG_ENDIAN)
	, address_size(0)
	, memory_atom_size(1)
	, elf_phoff(0)
	, elf_phent(0)
	, elf_phnum(0)
	, elf_flags(0)
	, blobs()
	, sections()
	, segments()
	, refcount(0)
{
	refcount = new unsigned int();
	*refcount = 0;
}

template <class MEMORY_ADDR>
Blob<MEMORY_ADDR>::Blob(const Blob<MEMORY_ADDR>& _blob)
	: capability(_blob.capability)
	, filename(_blob.filename)
	, architecture(_blob.architecture)
	, entry_point(_blob.entry_point)
	, stack_base(_blob.stack_base)
	, endian(_blob.endian)
	, file_endian(_blob.file_endian)
	, address_size(_blob.address_size)
	, memory_atom_size(_blob.memory_atom_size)
	, elf_phoff(_blob.elf_phoff)
	, elf_phent(_blob.elf_phent)
	, elf_phnum(_blob.elf_phnum)
	, elf_flags(_blob.elf_flags)
	, blobs()
	, sections()
	, segments()
	, refcount(0)
{
	refcount = new unsigned int();
	*refcount = 0;

	const std::vector<const Blob<MEMORY_ADDR> *>& _blobs = _blob.blobs;
	typename std::vector<const Blob<MEMORY_ADDR> *>::const_iterator _blob_iter;
	for(_blob_iter = _blobs.begin(); _blob_iter != _blobs.end(); _blob_iter++)
	{
		const Blob<MEMORY_ADDR> *blob = *_blob_iter;
		
		AddBlob(blob);
	}

	const std::vector<const Section<MEMORY_ADDR> *>& _sections = _blob.sections;
	typename std::vector<const Section<MEMORY_ADDR> *>::const_iterator _section_iter;
	for(_section_iter = _sections.begin(); _section_iter != _sections.end(); _section_iter++)
	{
		const Section<MEMORY_ADDR> *section = *_section_iter;
		
		AddSection(section);
	}

	const std::vector<const Segment<MEMORY_ADDR> *>& _segments = _blob.segments;
	typename std::vector<const Segment<MEMORY_ADDR> *>::const_iterator _segment_iter;
	for(_segment_iter = _segments.begin(); _segment_iter != _segments.end(); _segment_iter++)
	{
		const Segment<MEMORY_ADDR> *segment = *_segment_iter;
		
		AddSegment(segment);
	}
}

template <class MEMORY_ADDR>
Blob<MEMORY_ADDR>::~Blob()
{
	typename std::vector<const Blob<MEMORY_ADDR> *>::const_iterator blob_iter;
	for(blob_iter = blobs.begin(); blob_iter != blobs.end(); blob_iter++)
	{
		(*blob_iter)->Release();
	}

	typename std::vector<const Section<MEMORY_ADDR> *>::const_iterator section_iter;
	for(section_iter = sections.begin(); section_iter != sections.end(); section_iter++)
	{
		(*section_iter)->Release();
	}
	
	typename std::vector<const Segment<MEMORY_ADDR> *>::const_iterator segment_iter;
	for(segment_iter = segments.begin(); segment_iter != segments.end(); segment_iter++)
	{
		(*segment_iter)->Release();
	}

	delete refcount;
}
	
template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::SetFilename(const char *_filename)
{
	filename = _filename;
	capability = (Capability)(capability | CAP_FILENAME);
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::SetFileFormat(FileFormat _ffmt)
{
	ffmt = _ffmt;
	capability = (Capability)(capability | CAP_FILE_FORMAT);
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::SetEntryPoint(MEMORY_ADDR _entry_point)
{
	entry_point = _entry_point;
	capability = (Capability)(capability | CAP_ENTRY_POINT);
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::SetArchitecture(const char *_architecture)
{
	architecture = _architecture;
	capability = (Capability)(capability | CAP_ARCHITECTURE);
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::SetStackBase(MEMORY_ADDR _stack_base)
{
	stack_base = _stack_base;
	capability = (Capability)(capability | CAP_STACK_BASE);
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::SetEndian(endian_type _endian)
{
	endian = _endian;
	capability = (Capability)(capability | CAP_ENDIAN);
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::SetFileEndian(endian_type _file_endian)
{
	file_endian = _file_endian;
	capability = (Capability)(capability | CAP_FILE_ENDIAN);
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::SetAddressSize(unsigned int _address_size)
{
	address_size = _address_size;
	capability = (Capability)(capability | CAP_ADDRESS_SIZE);
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::SetMemoryAtomSize(unsigned int _memory_atom_size)
{
	memory_atom_size = _memory_atom_size;
	capability = (Capability)(capability | CAP_MEMORY_ATOM_SIZE);
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::SetELF_PHOFF(uint64_t _elf_phoff)
{
	elf_phoff = _elf_phoff;
	capability = (Capability)(capability | CAP_ELF_PHOFF);
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::SetELF_PHENT(unsigned int _elf_phent)
{
	elf_phent = _elf_phent;
	capability = (Capability)(capability | CAP_ELF_PHENT);
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::SetELF_PHNUM(unsigned int _elf_phnum)
{
	elf_phnum = _elf_phnum;
	capability = (Capability)(capability | CAP_ELF_PHNUM);
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::SetELF_Flags(uint32_t _elf_flags)
{
	elf_flags = _elf_flags;
	capability = (Capability)(capability | CAP_ELF_FLAGS);
}

template <class MEMORY_ADDR>
Capability Blob<MEMORY_ADDR>::GetCapability() const
{
	// Synthesize overall blob capabilities
	Capability ret_capability = capability;
	typename std::vector<const Blob<MEMORY_ADDR> *>::const_iterator blob_iter;
	for(blob_iter = blobs.begin(); blob_iter != blobs.end(); blob_iter++)
	{
		const Blob<MEMORY_ADDR> *blob = *blob_iter;
		
		ret_capability = (Capability)(ret_capability | blob->GetCapability());
	}
	
	return ret_capability;
}

template <class MEMORY_ADDR>
const char *Blob<MEMORY_ADDR>::GetFilename() const
{
	if(capability & CAP_FILENAME) return filename.c_str();
	
	typename std::vector<const Blob<MEMORY_ADDR> *>::const_iterator blob_iter;
	for(blob_iter = blobs.begin(); blob_iter != blobs.end(); blob_iter++)
	{
		const Blob<MEMORY_ADDR> *blob = *blob_iter;
		
		if(blob->GetCapability() & CAP_FILENAME) return blob->GetFilename();
	}
	return 0;
}

template <class MEMORY_ADDR>
FileFormat Blob<MEMORY_ADDR>::GetFileFormat() const
{
	if(capability & CAP_FILE_FORMAT) return ffmt;
	return FFMT_UNKNOWN;
}

template <class MEMORY_ADDR>
MEMORY_ADDR Blob<MEMORY_ADDR>::GetEntryPoint() const
{
	if(capability & CAP_ENTRY_POINT) return entry_point;
	
	typename std::vector<const Blob<MEMORY_ADDR> *>::const_iterator blob_iter;
	for(blob_iter = blobs.begin(); blob_iter != blobs.end(); blob_iter++)
	{
		const Blob<MEMORY_ADDR> *blob = *blob_iter;
		
		if(blob->GetCapability() & CAP_ENTRY_POINT) return blob->GetEntryPoint();
	}
	return 0;
}

template <class MEMORY_ADDR>
const char *Blob<MEMORY_ADDR>::GetArchitecture() const
{
	if(capability & CAP_ARCHITECTURE) return architecture.c_str();
	
	typename std::vector<const Blob<MEMORY_ADDR> *>::const_iterator blob_iter;
	for(blob_iter = blobs.begin(); blob_iter != blobs.end(); blob_iter++)
	{
		const Blob<MEMORY_ADDR> *blob = *blob_iter;
		
		if(blob->GetCapability() & CAP_ARCHITECTURE) return blob->GetArchitecture();
	}
	return 0;
}

template <class MEMORY_ADDR>
MEMORY_ADDR Blob<MEMORY_ADDR>::GetStackBase() const
{
	if(capability & CAP_STACK_BASE) return stack_base;
	
	typename std::vector<const Blob<MEMORY_ADDR> *>::const_iterator blob_iter;
	for(blob_iter = blobs.begin(); blob_iter != blobs.end(); blob_iter++)
	{
		const Blob<MEMORY_ADDR> *blob = *blob_iter;
		
		if(blob->GetCapability() & CAP_STACK_BASE) return blob->GetStackBase();
	}
	return 0;
}

template <class MEMORY_ADDR>
endian_type Blob<MEMORY_ADDR>::GetEndian() const
{
	if(capability & CAP_ENDIAN) return endian;
	
	typename std::vector<const Blob<MEMORY_ADDR> *>::const_iterator blob_iter;
	for(blob_iter = blobs.begin(); blob_iter != blobs.end(); blob_iter++)
	{
		const Blob<MEMORY_ADDR> *blob = *blob_iter;
		
		if(blob->GetCapability() & CAP_ENDIAN) return blob->GetEndian();
	}
	return unisim::util::endian::E_BIG_ENDIAN;
}

template <class MEMORY_ADDR>
endian_type Blob<MEMORY_ADDR>::GetFileEndian() const
{
	if(capability & CAP_FILE_ENDIAN) return file_endian;
	return unisim::util::endian::E_BIG_ENDIAN;
}

template <class MEMORY_ADDR>
unsigned int Blob<MEMORY_ADDR>::GetAddressSize() const
{
	if(capability & CAP_ADDRESS_SIZE) return address_size;
	
	typename std::vector<const Blob<MEMORY_ADDR> *>::const_iterator blob_iter;
	for(blob_iter = blobs.begin(); blob_iter != blobs.end(); blob_iter++)
	{
		const Blob<MEMORY_ADDR> *blob = *blob_iter;
		
		if(blob->GetCapability() & CAP_ADDRESS_SIZE) return blob->GetAddressSize();
	}
	return 0;
}

template <class MEMORY_ADDR>
unsigned int Blob<MEMORY_ADDR>::GetMemoryAtomSize() const
{
	if(capability & CAP_MEMORY_ATOM_SIZE) return memory_atom_size;
	return 1;
}

template <class MEMORY_ADDR>
uint64_t Blob<MEMORY_ADDR>::GetELF_PHOFF() const
{
	return (capability & CAP_ELF_PHOFF) ? elf_phoff : 0;
}

template <class MEMORY_ADDR>
unsigned int Blob<MEMORY_ADDR>::GetELF_PHENT() const
{
	return (capability & CAP_ELF_PHENT) ? elf_phent : 0;
}

template <class MEMORY_ADDR>
unsigned int Blob<MEMORY_ADDR>::GetELF_PHNUM() const
{
	return (capability & CAP_ELF_PHNUM) ? elf_phnum : 0;
}

template <class MEMORY_ADDR>
unsigned int Blob<MEMORY_ADDR>::GetELF_Flags() const
{
	return (capability & CAP_ELF_FLAGS) ? elf_flags : 0;
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::GetAddrRange(MEMORY_ADDR& _min_addr, MEMORY_ADDR& _max_addr) const
{
	MEMORY_ADDR min_addr = (MEMORY_ADDR) -1;
	MEMORY_ADDR max_addr = 0;
	
	typename std::vector<const Blob<MEMORY_ADDR> *>::const_iterator blob_iter;
	for(blob_iter = blobs.begin(); blob_iter != blobs.end(); blob_iter++)
	{
		const Blob<MEMORY_ADDR> *blob = *blob_iter;
		MEMORY_ADDR blob_min_addr;
		MEMORY_ADDR blob_max_addr;
		blob->GetAddrRange(blob_min_addr, blob_max_addr);
		if(blob_min_addr < min_addr) min_addr = blob_min_addr;
		if(blob_max_addr > max_addr) max_addr = blob_max_addr;
	}

	typename std::vector<const Section<MEMORY_ADDR> *>::const_iterator section_iter;
	for(section_iter = sections.begin(); section_iter != sections.end(); section_iter++)
	{
		const Section<MEMORY_ADDR> *section = *section_iter;
		if(section->GetAttr() & Section<MEMORY_ADDR>::SA_A)
		{
			MEMORY_ADDR section_min_addr;
			MEMORY_ADDR section_max_addr;
			section->GetAddrRange(section_min_addr, section_max_addr);
			if(section_min_addr < min_addr) min_addr = section_min_addr;
			if(section_max_addr > max_addr) max_addr = section_max_addr;
		}
	}
	
	typename std::vector<const Segment<MEMORY_ADDR> *>::const_iterator segment_iter;
	for(segment_iter = segments.begin(); segment_iter != segments.end(); segment_iter++)
	{
		const Segment<MEMORY_ADDR> *segment = *segment_iter;
		if(segment->GetType() == Segment<MEMORY_ADDR>::TY_LOADABLE)
		{
			MEMORY_ADDR segment_min_addr;
			MEMORY_ADDR segment_max_addr;
			segment->GetAddrRange(segment_min_addr, segment_max_addr);
			if(segment_min_addr < min_addr) min_addr = segment_min_addr;
			if(segment_max_addr > max_addr) max_addr = segment_max_addr;
		}
	}

	_min_addr = min_addr;
	_max_addr = max_addr;
}

template <class MEMORY_ADDR>
bool Blob<MEMORY_ADDR>::HasOverlap(MEMORY_ADDR min_addr, MEMORY_ADDR max_addr) const
{
	typename std::vector<const Blob<MEMORY_ADDR> *>::const_iterator blob_iter;
	for(blob_iter = blobs.begin(); blob_iter != blobs.end(); blob_iter++)
	{
		const Blob<MEMORY_ADDR> *blob = *blob_iter;
		if(blob->HasOverlap(min_addr, max_addr)) return true;
	}

	typename std::vector<const Section<MEMORY_ADDR> *>::const_iterator section_iter;
	for(section_iter = sections.begin(); section_iter != sections.end(); section_iter++)
	{
		const Section<MEMORY_ADDR> *section = *section_iter;
		if(section->GetAttr() & Section<MEMORY_ADDR>::SA_A)
		{
			if(section->HasOverlap(min_addr, max_addr)) return true;
		}
	}
	
	typename std::vector<const Segment<MEMORY_ADDR> *>::const_iterator segment_iter;
	for(segment_iter = segments.begin(); segment_iter != segments.end(); segment_iter++)
	{
		const Segment<MEMORY_ADDR> *segment = *segment_iter;
		if(segment->GetType() == Segment<MEMORY_ADDR>::TY_LOADABLE)
		{
			if(segment->HasOverlap(min_addr, max_addr)) return true;
		}
	}

	return false;
}

template <class MEMORY_ADDR>
const std::vector<const Blob<MEMORY_ADDR> *>& Blob<MEMORY_ADDR>::GetBlobs() const
{
	return blobs;
}

template <class MEMORY_ADDR>
const std::vector<const Section<MEMORY_ADDR> *>& Blob<MEMORY_ADDR>::GetSections() const
{
	return sections;
}

template <class MEMORY_ADDR>
const std::vector<const Segment<MEMORY_ADDR> *>& Blob<MEMORY_ADDR>::GetSegments() const
{
	return segments;
}

template <class MEMORY_ADDR>
const Section<MEMORY_ADDR> *Blob<MEMORY_ADDR>::GetSection(unsigned int index) const
{
	return (index < sections.size()) ? sections[index] : 0;
}

template <class MEMORY_ADDR>
const Segment<MEMORY_ADDR> *Blob<MEMORY_ADDR>::GetSegment(unsigned int index) const
{
	return (index < segments.size()) ? segments[index] : 0;
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::AddBlob(const Blob<MEMORY_ADDR> *blob)
{
	blob->Catch();
	blobs.push_back(blob);
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::AddSection(const Section<MEMORY_ADDR> *section)
{
	section->Catch();
	sections.push_back(section);
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::AddSegment(const Segment<MEMORY_ADDR> *segment)
{
	segment->Catch();
	segments.push_back(segment);
}

template <class MEMORY_ADDR>
const Section<MEMORY_ADDR> *Blob<MEMORY_ADDR>::FindSection(const char *name, bool recursive) const
{
	typename std::vector<const Section<MEMORY_ADDR> *>::const_iterator section_iter;
	for(section_iter = sections.begin(); section_iter != sections.end(); section_iter++)
	{
		const Section<MEMORY_ADDR> *section = *section_iter;
		if(strcmp(section->GetName(), name) == 0)
		{
			return section;
		}
	}

	if(recursive)
	{
		typename std::vector<const Blob<MEMORY_ADDR> *>::const_iterator blob_iter;
		for(blob_iter = blobs.begin(); blob_iter != blobs.end(); blob_iter++)
		{
			const Blob<MEMORY_ADDR> *blob = *blob_iter;
			const Section<MEMORY_ADDR> *section = blob->FindSection(name);
			if(section) return section;
		}
	}
	return 0;
}

template <class MEMORY_ADDR>
const Section<MEMORY_ADDR> *Blob<MEMORY_ADDR>::FindSection(typename Section<MEMORY_ADDR>::Type section_type) const
{
	typename std::vector<const Section<MEMORY_ADDR> *>::const_iterator section_iter;
	for(section_iter = sections.begin(); section_iter != sections.end(); section_iter++)
	{
		const Section<MEMORY_ADDR> *section = *section_iter;
		if(section->GetType() == section_type)
		{
			return section;
		}
	}
	return 0;
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::Catch() const
{
	(*refcount)++;
}

template <class MEMORY_ADDR>
void Blob<MEMORY_ADDR>::Release() const
{
	if(--(*refcount) == 0)
	{
		delete this;
	}
}

} // end of namespace blob
} // end of namespace util
} // end of namespace unisim

#endif
