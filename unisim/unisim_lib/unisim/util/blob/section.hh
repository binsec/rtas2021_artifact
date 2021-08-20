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

#ifndef __UNISIM_UTIL_BLOB_SECTION_HH__
#define __UNISIM_UTIL_BLOB_SECTION_HH__

#include <string>

namespace unisim {
namespace util {
namespace blob {

template <class MEMORY_ADDR>
class Section
{
public:
	typedef enum
	{
		TY_UNKNOWN,  // unknown section type
		TY_NULL,
		TY_PROGBITS, // there's data in data
		TY_NOBITS,   // all zero's
		TY_ELF_SYMTAB,   // ELF symtab symbol table (should have a link to a string table)
		TY_COFF_SYMTAB,  // COFF symbol table (should have a link to a string table)
		TY_STRTAB    // string table
	} Type;
	
	typedef enum
	{
		SA_NULL = 0,
		SA_A = 1, // Alloc
		SA_W = 2, // Write
		SA_AW = 3,
		SA_X = 4, // Execute
		SA_AX = 5,
		SA_WX = 6,
		SA_AWX = 7
	} Attribute;

	Section(Type type, Attribute attr, const char *name, unsigned int alignment, unsigned int link, MEMORY_ADDR addr, MEMORY_ADDR size, void *data);
	Section(const Section<MEMORY_ADDR>& section);
	virtual ~Section();
	
	Type GetType() const;
	Attribute GetAttr() const;
	const char *GetName() const;
	unsigned int GetAlignment() const;
	MEMORY_ADDR GetAddr() const;
	MEMORY_ADDR GetSize() const;
	unsigned int GetLink() const;
	const void *GetData() const;
	void GetAddrRange(MEMORY_ADDR& min_addr, MEMORY_ADDR& max_addr) const;
	bool HasOverlap(MEMORY_ADDR min_addr, MEMORY_ADDR max_addr) const;
	
	void Catch() const;
	void Release() const;
private:
	Type type;               // section type
	Attribute attr;          // section attribute
	std::string name;        // section name (empty=unavailable)
	unsigned int alignment;  // alignment (0=unavailable)
	unsigned int link;       // link to another section
	MEMORY_ADDR addr;        // location in memory
	MEMORY_ADDR size;        // size in bytes of data
	void *data;
	unsigned int *refcount;
};

} // end of namespace blob
} // end of namespace util
} // end of namespace unisim

#endif

