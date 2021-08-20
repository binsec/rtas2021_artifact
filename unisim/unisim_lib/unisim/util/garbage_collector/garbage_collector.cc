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
 
#include "unisim/util/garbage_collector/garbage_collector.hh"
#if DEBUG_GC >= 2
#include <unisim/util/backtrace/backtrace.hh>
#endif
#include <stdlib.h>
#include <string.h>
#include <iostream>

using std::cerr;
using std::endl;

void *operator new(std::size_t size, unisim::util::garbage_collector::PointerBase)
{
//	cerr << "GarbageCollector::operator new " << size << endl;
	return unisim::util::garbage_collector::GarbageCollector::Allocate(size);
}

void *operator new[](std::size_t size, unisim::util::garbage_collector::PointerBase)
{
//	cerr << "GarbageCollector::operator new[] " << size << endl;
	return unisim::util::garbage_collector::GarbageCollector::Allocate(size);
}

namespace unisim {
namespace util {
namespace garbage_collector {

bool GarbageCollector::done_setup = GarbageCollector::Setup();
#if DEBUG_GC >= 1
unsigned long long GarbageCollector::num_catches = 0;
unsigned long long GarbageCollector::num_collects = 0;
unsigned long long GarbageCollector::num_allocs = 0;
unsigned long long GarbageCollector::num_reuse = 0;
unsigned long long GarbageCollector::num_free = 0;
unsigned long long GarbageCollector::memory_usage = 0;
#endif

#if DEBUG_GC >= 2
static const unsigned long long ABNORMAL_ALLOC_THRESHOLD = 0xffffffffffffffffULL;
#endif

MemoryBlock *GarbageCollector::free_blocks[GarbageCollector::MAX_MEMORY_BLOCK_SIZE / GarbageCollector::MIN_MEMORY_BLOCK_SIZE];

void GarbageCollector::AtExit() {
	Reset();
}

bool GarbageCollector::Setup()
{
	if(!done_setup)
	{
		atexit(AtExit);
		memset(free_blocks, 0, sizeof(free_blocks));
	}
	return true;
}

void GarbageCollector::Reset()
{
	unsigned int i;
#if DEBUG_GC >= 1
	std::cerr << "GC: usage=" << memory_usage << " bytes ("<< (memory_usage / 1024) <<" KB), alloc=" << num_allocs << " blocks, reuse=" << num_reuse << " blocks, free=" << num_free << " blocks, catch=" << num_catches << " blocks, collect=" << num_collects << " blocks, catch-collect=" << (num_catches - num_collects) << " blocks" << std::endl;
#endif
	//	cerr << "GarbageCollector::Reset()" << endl;
	for(i = 0; i < MAX_MEMORY_BLOCK_SIZE / MIN_MEMORY_BLOCK_SIZE; i++)
	{
		MemoryBlock * block = free_blocks[i];
		if(block)
		{
			MemoryBlock *next_block;
			do
			{
				next_block = block->next;
				free(block);
			} while((block = next_block) != 0);
		}
		free_blocks[i] = 0;
	}
#if DEBUG_GC >= 1
	num_catches = 0;
	num_collects = 0;
	num_allocs = 0;
	num_reuse = 0;
	num_free = 0;
	memory_usage = 0;
#endif
}

void *GarbageCollector::Allocate(unsigned int size)
{
	MemoryBlock *block;
	unsigned int index = size/ MIN_MEMORY_BLOCK_SIZE;
	if(index >= MAX_MEMORY_BLOCK_SIZE / MIN_MEMORY_BLOCK_SIZE)
	{
		block = (MemoryBlock *) malloc(sizeof(MemoryBlock) + size);
		block->magic = MAGIC;
		block->refcount = 0;
		block->size = size;
//		cerr << "GarbageCollector::Allocate(" << size << ") on big blocks: " << block << endl;
#if DEBUG_GC >= 1
		num_allocs++;
		memory_usage += sizeof(MemoryBlock) + size;
#endif
#if DEBUG_GC >= 2
		if(num_allocs >= ABNORMAL_ALLOC_THRESHOLD)
		{
			std::cerr << "Alloc(" << block << ") of size " << block->size << ":" << std::endl;
			std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
		}
#endif
 		return block + 1;
	}

	if(free_blocks[index])
	{
		block = free_blocks[index];
		free_blocks[index] = block->next;
		
//		cerr << "GarbageCollector::Allocate(" << size << ") on free blocks: " << block << endl;		
#if DEBUG_GC >= 1
		num_reuse++;
#endif
#if DEBUG_GC >= 2
		if(num_allocs >= ABNORMAL_ALLOC_THRESHOLD)
		{
			std::cerr << "Reuse(" << block << ") of size " << block->size << ":" << std::endl;
			std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
		}
#endif
 		return block + 1;
	}
	
//		(index + 1) * MIN_MEMORY_BLOCK_SIZE
	block = (MemoryBlock *) malloc(sizeof(MemoryBlock) + ((index + 1) * MIN_MEMORY_BLOCK_SIZE));
//	block = (MemoryBlock *) malloc(sizeof(MemoryBlock) + ((size + MIN_MEMORY_BLOCK_SIZE - 1) & ~(MIN_MEMORY_BLOCK_SIZE - 1)));
	block->magic = MAGIC;
	block->refcount = 0;
	block->size = size;
//	cerr << "GarbageCollector::Allocate(" << size << ") on a new block: " << block << endl;
#if DEBUG_GC >= 1
	num_allocs++;
	memory_usage += sizeof(MemoryBlock) + ((index + 1) * MIN_MEMORY_BLOCK_SIZE);
#endif
#if DEBUG_GC >= 2
	if(num_allocs >= ABNORMAL_ALLOC_THRESHOLD)
	{
		std::cerr << "Alloc(" << block << ") of size " << block->size << ":" << std::endl;
		std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
	}
#endif
	
	return block + 1;
}

bool GarbageCollector::Collect(void *p)
{
	MemoryBlock *block = (MemoryBlock *) p - 1;
#if DEBUG_GC >= 1
	num_collects++;
#endif
#if DEBUG_GC >= 2
	if(num_allocs >= ABNORMAL_ALLOC_THRESHOLD)
	{
		std::cerr << "Collect(" << block << ") of size " << block->size << ":" << std::endl;
		std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
	}
#endif
	if(block->magic != MAGIC)
	{
		cerr << "WARNING! Doing collect on unmanaged block" << endl;
		abort();
	}
	//cerr << "GarbageCollector::Collect(0x" << p << "): refcount=" << block->refcount << endl;
	if(--block->refcount != 0) return false;

	unsigned int index = block->size/ MIN_MEMORY_BLOCK_SIZE;
	if(index >= MAX_MEMORY_BLOCK_SIZE / MIN_MEMORY_BLOCK_SIZE)
	{
#if DEBUG_GC >= 1
		num_free++;
#endif
#if DEBUG_GC >= 2
		if(num_allocs >= ABNORMAL_ALLOC_THRESHOLD)
		{
			std::cerr << "free(" << block << ") of size " << block->size << ":" << std::endl;
			std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
		}
#endif
		free(block);
	}
	else
	{
		block->next = free_blocks[index];
		free_blocks[index] = block;
	}

	return true;
}

void GarbageCollector::Catch(void *p)
{
	MemoryBlock *block = (MemoryBlock *) p - 1;
#if DEBUG_GC >= 1
	num_catches++;
#endif
#if DEBUG_GC >= 2
	if(num_allocs >= ABNORMAL_ALLOC_THRESHOLD)
	{
		std::cerr << "Catch(" << block << ") of size " << block->size << ":" << std::endl;
		std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
	}
#endif
	if(block->magic != MAGIC)
	{
		cerr << "WARNING! Doing catch on unmanaged block" << endl;
		abort();
	}
	//cerr << "GarbageCollector::Catch(0x" << p << "): refcount=" << block->refcount << endl;
	block->refcount++;
}

} // end of namespace garbage_collector
} // end of namespace util
} // end of namespace unisim
