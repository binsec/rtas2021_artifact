/*
 *  Copyright (c) 2014,
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

#include "core/sysdep/posix_guarded_stack.h"

#if __LIBIEEE1666_VALGRIND__
#include <valgrind/valgrind.h>
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdexcept>

namespace sc_core {

sc_posix_guarded_stack::sc_posix_guarded_stack(std::size_t stack_size)
	: mapped_area(0)
	, mapped_area_length(0)
	, page_size(0)
#if __LIBIEEE1666_VALGRIND__
	, valgrind_stack_id(0)
#endif
{
	// get page size
	page_size = ::sysconf(_SC_PAGESIZE);
	
	// compute page minimum page count that contains 'stack_size' bytes
	std::size_t page_count = (stack_size + page_size) / page_size;
	
	// account for one guard page
	page_count++;
	
	// compute mapped area length
	mapped_area_length = page_count * page_size;

	int flags = MAP_PRIVATE;           // private memory area
#if defined(__APPLE__) || defined(__APPLE_CC__)
	flags = flags | MAP_ANON;
#else
	flags = flags | MAP_ANONYMOUS;     // initialize with zeroes on linux
#endif

#if defined(__FreeBSD__) || defined(__APPLE__) || defined(__APPLE_CC__)
	int fd = ::open("/dev/zero", O_RDONLY); // initialize with zeroes
#else
	int fd = -1;                            // unused with MAP_ANON/MAP_ANONYMOUS (for portability)
#endif

	// map memory
	mapped_area = ::mmap(
		0,                       // let the kernel choose the address at which to create the mapping
		mapped_area_length,      // mapped area length
		PROT_READ | PROT_WRITE,  // allow R/W accesses only
		flags,                   // flags
		fd,                      // file descriptor
		0                        // file offset
	);
	
	if(!mapped_area) throw std::bad_alloc();
	
#if defined(__FreeBSD__) || defined(__APPLE__) || defined(__APPLE_CC__)
	::close(fd);
#endif
	
	// disallow any access to first page
	::mprotect(mapped_area, page_size, PROT_NONE);
	
#if __LIBIEEE1666_VALGRIND__
	valgrind_stack_id = VALGRIND_STACK_REGISTER(reinterpret_cast<char *>(mapped_area) + page_size, reinterpret_cast<char *>(mapped_area) + mapped_area_length);
#endif
}

sc_posix_guarded_stack::~sc_posix_guarded_stack()
{
#if __LIBIEEE1666_VALGRIND__
	VALGRIND_STACK_DEREGISTER(valgrind_stack_id);
#endif
	if(mapped_area)
	{
		::munmap(mapped_area, mapped_area_length);
	}
}

void *sc_posix_guarded_stack::get_top_of_the_stack() const
{
	return reinterpret_cast<void *>(reinterpret_cast<char *>(mapped_area) + mapped_area_length);
}

void *sc_posix_guarded_stack::get_stack_base() const
{
	return reinterpret_cast<void *>(reinterpret_cast<char *>(mapped_area) + page_size);
}

sc_stack *sc_posix_guarded_stack_system::create_stack(std::size_t stack_size)
{
	return new sc_posix_guarded_stack(stack_size);
}

} // end of namespace sc_core
