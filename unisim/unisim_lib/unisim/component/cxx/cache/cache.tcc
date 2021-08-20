/*
 *  Copyright (c) 2008,
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

#ifndef __UNISIM_COMPONENT_CXX_CACHE_CACHE_TCC__
#define __UNISIM_COMPONENT_CXX_CACHE_CACHE_TCC__

#include <string.h>

namespace unisim {
namespace component {
namespace cxx {
namespace cache {

template <class CONFIG>
CacheBlock<CONFIG>::CacheBlock() : base_addr(0)
{
	memset(storage, 0, SIZE);
}

template <class CONFIG>
CacheBlock<CONFIG>::~CacheBlock()
{
}

template <class CONFIG>
inline uint8_t& CacheBlock<CONFIG>::operator [] (uint32_t offset)
{
	return storage[offset];
}

template <class CONFIG>
inline void CacheBlock<CONFIG>::SetBaseAddr(ADDRESS addr)
{
	base_addr = addr;
}

template <class CONFIG>
inline typename CacheBlock<CONFIG>::ADDRESS CacheBlock<CONFIG>::GetBaseAddr() const
{
	return base_addr;
}

template <class CONFIG>
inline void CacheBlock<CONFIG>::Zero()
{
	memset(storage, 0, SIZE);
}

template <class CONFIG>
inline void CacheBlock<CONFIG>::Write(const void *buffer, uint32_t offset, uint32_t size)
{
	memcpy(storage + offset, buffer, size);
}

template <class CONFIG>
inline void CacheBlock<CONFIG>::Read(void *buffer, uint32_t offset, uint32_t size)
{
	memcpy(buffer, storage + offset, size);
}

template <class CONFIG>
CacheLine<CONFIG>::CacheLine()
{
}

template <class CONFIG>
CacheLine<CONFIG>::~CacheLine()
{
}

template <class CONFIG>
inline typename CONFIG::ADDRESS CacheLine<CONFIG>::GetBaseAddr() const
{
	return base_addr;
}

template <class CONFIG>
void CacheLine<CONFIG>::SetBaseAddr(ADDRESS base_addr)
{
	this->base_addr = base_addr;
	uint32_t sector;
	for(sector = 0; sector < BLOCKS_PER_LINE; sector++)
	{
		blocks[sector].SetBaseAddr(base_addr + (sector * CacheBlock<CONFIG>::SIZE));
	}
}

template <class CONFIG>
inline CacheBlock<CONFIG>& CacheLine<CONFIG>::operator [] (uint32_t sector)
{
	return blocks[sector];
}

template <class CONFIG>
CacheSet<CONFIG>::CacheSet()
{
}

template <class CONFIG>
CacheSet<CONFIG>::~CacheSet()
{
}

template <class CONFIG>
inline CacheLine<CONFIG>& CacheSet<CONFIG>::operator [] (uint32_t way)
{
	return lines[way];
}

template <class CONFIG>
Cache<CONFIG>::Cache()
{
}

template <class CONFIG>
Cache<CONFIG>::~Cache()
{
}

template <class CONFIG>
inline void Cache<CONFIG>::DecodeAddress(ADDRESS addr, ADDRESS& line_base_addr, ADDRESS& block_base_addr, uint32_t& index, uint32_t& sector, uint32_t& offset, uint32_t& size_to_block_boundary)
{
	offset = addr & (CONFIG::CACHE_BLOCK_SIZE - 1);
	block_base_addr = addr & (~(CONFIG::CACHE_BLOCK_SIZE - 1));
	size_to_block_boundary = CONFIG::CACHE_BLOCK_SIZE - offset;
	line_base_addr = addr & ~((CONFIG::CACHE_BLOCK_SIZE * CONFIG::CACHE_BLOCKS_PER_LINE) - 1);
	sector = (addr / CONFIG::CACHE_BLOCK_SIZE) & (CONFIG::CACHE_BLOCKS_PER_LINE - 1);
	index = (addr / CONFIG::CACHE_BLOCK_SIZE / CONFIG::CACHE_BLOCKS_PER_LINE) % NUM_SETS;
}

template <class CONFIG>
inline CacheSet<CONFIG>& Cache<CONFIG>::operator [] (uint32_t index)
{
	return sets[index];
}

} // end of namespace cache
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_CACHE_CACHE_TCC__
