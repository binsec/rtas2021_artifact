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

#ifndef __UNISIM_UTIL_BLOB_SECTION_TCC__
#define __UNISIM_UTIL_BLOB_SECTION_TCC__

#include <stdlib.h>
#include <string.h>

namespace unisim {
namespace util {
namespace blob {

template <class MEMORY_ADDR>
Section<MEMORY_ADDR>::Section(Type _type, Attribute _attr, const char *_name, unsigned int _alignment, unsigned int _link, MEMORY_ADDR _addr, MEMORY_ADDR _size, void *_data)
	: type(_type)
	, attr(_attr)
	, name(_name)
	, alignment(_alignment)
	, link(_link)
	, addr(_addr)
	, size(_size)
	, data(_data)
	, refcount(0)
{
	refcount = new unsigned int();
	*refcount = 0;
}

template <class MEMORY_ADDR>
Section<MEMORY_ADDR>::Section(const Section<MEMORY_ADDR>& section)
	: type(section.type)
	, attr(section.attr)
	, name(section.name)
	, alignment(section.alignment)
	, link(section.link)
	, addr(section.addr)
	, size(section.size)
	, data(0)
	, refcount(0)
{
	refcount = new unsigned int();
	*refcount = 0;

	if(section.data)
	{
		data = malloc(size);
		memcpy(data, section.data, size);
	}
}

template <class MEMORY_ADDR>
Section<MEMORY_ADDR>::~Section()
{
	if(data)
	{
		free(data);
	}
	delete refcount;
}

template <class MEMORY_ADDR>
typename Section<MEMORY_ADDR>::Type Section<MEMORY_ADDR>::GetType() const
{
	return type;
}

template <class MEMORY_ADDR>
typename Section<MEMORY_ADDR>::Attribute Section<MEMORY_ADDR>::GetAttr() const
{
	return attr;
}

template <class MEMORY_ADDR>
const char *Section<MEMORY_ADDR>::GetName() const
{
	return name.c_str();
}

template <class MEMORY_ADDR>
unsigned int Section<MEMORY_ADDR>::GetAlignment() const
{
	return alignment;
}

template <class MEMORY_ADDR>
MEMORY_ADDR Section<MEMORY_ADDR>::GetAddr() const
{
	return addr;
}

template <class MEMORY_ADDR>
MEMORY_ADDR Section<MEMORY_ADDR>::GetSize() const
{
	return size;
}

template <class MEMORY_ADDR>
unsigned int Section<MEMORY_ADDR>::GetLink() const
{
	return link;
}

template <class MEMORY_ADDR>
const void *Section<MEMORY_ADDR>::GetData() const
{
	return data;
}

template <class MEMORY_ADDR>
void Section<MEMORY_ADDR>::GetAddrRange(MEMORY_ADDR& min_addr, MEMORY_ADDR& max_addr) const
{
	min_addr = addr;
	max_addr = addr + size - 1;
}

template <class MEMORY_ADDR>
bool Section<MEMORY_ADDR>::HasOverlap(MEMORY_ADDR _min_addr, MEMORY_ADDR _max_addr) const
{
	MEMORY_ADDR min_addr = addr;
	MEMORY_ADDR max_addr = addr + size - 1;

	return ((max_addr < _max_addr) ? max_addr : _max_addr) >= ((min_addr < _min_addr) ? _min_addr : min_addr);
}

template <class MEMORY_ADDR>
void Section<MEMORY_ADDR>::Catch() const
{
	(*refcount)++;
}

template <class MEMORY_ADDR>
void Section<MEMORY_ADDR>::Release() const
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
