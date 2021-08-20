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

#ifndef __UNISIM_COMPONENT_CXX_CACHE_CACHE_HH__
#define __UNISIM_COMPONENT_CXX_CACHE_CACHE_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace cache {

template <class CONFIG> class CacheBlock;
template <class CONFIG> class CacheLine;
template <class CONFIG> class CacheSet;
template <class CONFIG> class Cache;

template <class CONFIG>
class CacheBlock
{
public:
	typedef typename CONFIG::BLOCK_STATUS STATUS;
	typedef typename CONFIG::ADDRESS ADDRESS;
	static const uint32_t SIZE = CONFIG::CACHE_BLOCK_SIZE;

	CacheBlock();
	~CacheBlock();
	inline uint8_t& operator [] (uint32_t offset);
	static inline void DecodeAddress(ADDRESS addr, ADDRESS& base_addr, uint32_t& offset, uint32_t& size_to_block_boundary);
	inline ADDRESS GetBaseAddr() const;
	inline void Zero();
	inline void Write(const void *buffer, uint32_t offset, uint32_t size);
	inline void Read(void *buffer, uint32_t offset, uint32_t size);

	STATUS status;
protected:
	friend class CacheLine<CONFIG>;
	inline void SetBaseAddr(ADDRESS base_addr);
private:
	ADDRESS base_addr;
	uint8_t storage[SIZE];
};

template <class CONFIG>
class CacheLine
{
public:
	typedef typename CONFIG::LINE_STATUS STATUS;
	typedef typename CONFIG::ADDRESS ADDRESS;
	static const uint32_t BLOCKS_PER_LINE = CONFIG::CACHE_BLOCKS_PER_LINE;
	static const uint32_t SIZE = CacheBlock<CONFIG>::SIZE * BLOCKS_PER_LINE;

	CacheLine();
	~CacheLine();

	inline void SetBaseAddr(ADDRESS addr);
	inline ADDRESS GetBaseAddr() const;
	static inline void DecodeAddress(ADDRESS addr, ADDRESS& line_base_addr, ADDRESS& block_base_addr, uint32_t& sector, uint32_t& offset, uint32_t& size_to_block_boundary);
	inline CacheBlock<CONFIG>& operator [] (uint32_t sector);

	STATUS status;
protected:
private:
	ADDRESS base_addr;
	CacheBlock<CONFIG> blocks[BLOCKS_PER_LINE];
};

template <class CONFIG>
class CacheSet
{
public:
	typedef typename CONFIG::SET_STATUS STATUS;
	typedef typename CONFIG::ADDRESS ADDRESS;
	static const uint32_t ASSOCIATIVITY = CONFIG::CACHE_ASSOCIATIVITY;
	static const uint32_t SIZE = CacheLine<CONFIG>::SIZE * ASSOCIATIVITY;

	CacheSet();
	~CacheSet();

	static inline void DecodeAddress(ADDRESS addr, ADDRESS& line_base_addr, ADDRESS& block_base_addr, uint32_t& sector, uint32_t& offset, uint32_t& size_to_block_boundary);
	inline CacheLine<CONFIG>& operator [] (uint32_t way);

	STATUS status;
protected:
private:
	CacheLine<CONFIG> lines[ASSOCIATIVITY];
};

template <class CONFIG>
class Cache
{
public:
	typedef typename CONFIG::CACHE_STATUS STATUS;
	typedef typename CONFIG::ADDRESS ADDRESS;
	static const uint32_t SIZE = CONFIG::CACHE_SIZE;
	static const uint32_t NUM_SETS = CONFIG::CACHE_SIZE / CONFIG::CACHE_BLOCK_SIZE / CONFIG::CACHE_BLOCKS_PER_LINE / CacheSet<CONFIG>::ASSOCIATIVITY;

	Cache();
	~Cache();

	static inline void DecodeAddress(ADDRESS addr, ADDRESS& line_base_addr, ADDRESS& block_base_addr, uint32_t& index, uint32_t& sector, uint32_t& offset, uint32_t& size_to_block_boundary);
	inline CacheSet<CONFIG>& operator [] (uint32_t index);

	STATUS status;

protected:
private:
	CacheSet<CONFIG> sets[NUM_SETS];
};

} // end of namespace cache
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_CACHE_CACHE_HH__
