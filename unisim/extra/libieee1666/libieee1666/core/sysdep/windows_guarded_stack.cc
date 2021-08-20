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

#include "core/sysdep/windows_guarded_stack.h"

#include <stdexcept>

#include <windows.h>

namespace sc_core {

sc_windows_guarded_stack::sc_windows_guarded_stack(std::size_t stack_size)
{
	// get page size
	SYSTEM_INFO system_info;
	::GetSystemInfo(&system_info);
	page_size = system_info.dwPageSize;
	
	// compute page minimum page count that contains 'stack_size' bytes
	std::size_t page_count = (stack_size + page_size) / page_size;
	
	// account for one guard page
	page_count++;
	
	// compute mapped area length
	mapped_area_length = page_count * page_size;

	// map memory
	mapped_area = ::VirtualAlloc(
		0,                                          // let windows determine where to allocate region
		mapped_area_length,
		MEM_COMMIT,                                 // initialize with zeroes
		PAGE_READWRITE                              // allow R/W accesses only
	);

	if(!mapped_area) throw std::bad_alloc();
	
	// disallow any access to first page
	DWORD old_protect;
	
	BOOL status = ::VirtualProtect(
		mapped_area,
		page_size,                                  // one page
		PAGE_NOACCESS,                              // no accesses allowed
		&old_protect
	);
	
	if(!status)
	{
		::VirtualFree(
			mapped_area,
			mapped_area_length,
			MEM_RELEASE
		);
		
		throw std::bad_alloc();
	}
}

sc_windows_guarded_stack::~sc_windows_guarded_stack()
{
	if(mapped_area)
	{
		::VirtualFree(
			mapped_area,
			mapped_area_length,
			MEM_RELEASE
		);
	}
}

void *sc_windows_guarded_stack::get_top_of_the_stack() const
{
	return reinterpret_cast<void *>(reinterpret_cast<char *>(mapped_area) + mapped_area_length);
}

void *sc_windows_guarded_stack::get_stack_base() const
{
	return reinterpret_cast<void *>(reinterpret_cast<char *>(mapped_area) + page_size);
}

sc_stack *sc_windows_guarded_stack_system::create_stack(std::size_t stack_size)
{
	return new sc_windows_guarded_stack(stack_size);
}

} // end of namespace sc_core
