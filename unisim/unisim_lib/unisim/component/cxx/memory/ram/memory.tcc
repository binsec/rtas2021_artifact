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
 *          Daniel Gracia Perez
 */
 
#ifndef __UNISIM_COMPONENT_CXX_MEMORY_RAM_MEMORY_TCC__
#define __UNISIM_COMPONENT_CXX_MEMORY_RAM_MEMORY_TCC__

#include <inttypes.h>
#include "unisim/kernel/kernel.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/util/hash_table/hash_table.hh"
#include "unisim/util/likely/likely.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace memory {
namespace ram {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
MemoryPage<PHYSICAL_ADDR, PAGE_SIZE>::MemoryPage(PHYSICAL_ADDR _key, uint8_t initial_byte_value) :
	key(_key),
	next(0)
{
	storage = new uint8_t[PAGE_SIZE];
	memset(storage, initial_byte_value, PAGE_SIZE);
}

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
MemoryPage<PHYSICAL_ADDR, PAGE_SIZE>::~MemoryPage()
{
	delete[] storage;
}

inline const std::string u32toa(uint32_t v)
{
  std::stringstream sstr;
  sstr << v;
  return sstr.str();
}

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
Memory<PHYSICAL_ADDR, PAGE_SIZE>::Memory(const  char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent, "this module implements a memory")
	, unisim::kernel::Service<unisim::service::interfaces::Memory<PHYSICAL_ADDR> >(name, parent)
	, memory_export("memory-export", this)
	, org(0)
	, bytesize(0)
	, lo_addr(0)
	, hi_addr(0)
	, memory_usage(0)
	, verbose(false)
	, logger(*this)
	, hash_table()
	, param_org("org", this, org, "memory origin/base address")
	, param_bytesize("bytesize", this, bytesize, "memory size in bytes")
	, stat_memory_usage("memory-usage", this, memory_usage, (std::string("target memory usage in bytes (page granularity of ") + u32toa(PAGE_SIZE) + " bytes)").c_str())
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, initial_byte_value(0x00)
	, param_initial_byte_value("initial-byte-value", this, initial_byte_value, "initial value for all bytes of memory")
	, input_filename()
	, param_input_filename("input-filename", this, input_filename, "Input filename")
	, output_filename()
	, param_output_filename("output-filename", this, output_filename, "output filename")
	, output_file(0)
{
	stat_memory_usage.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	stat_memory_usage.SetVisible(false);

	param_bytesize.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_initial_byte_value.SetFormat(unisim::kernel::VariableBase::FMT_HEX);
}

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
Memory<PHYSICAL_ADDR, PAGE_SIZE>::~Memory()
{
	SaveToOutputFile();
	
	if(output_file)
	{
		delete output_file;
	}
}

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
PHYSICAL_ADDR Memory<PHYSICAL_ADDR, PAGE_SIZE>::GetLowAddress()
{
	return (lo_addr);
}

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
PHYSICAL_ADDR Memory<PHYSICAL_ADDR, PAGE_SIZE>::GetHighAddress()
{
	return (hi_addr);
}

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
void Memory<PHYSICAL_ADDR, PAGE_SIZE>::OnDisconnect()
{
}

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
bool Memory<PHYSICAL_ADDR, PAGE_SIZE>::BeginSetup()
{
	lo_addr = org;
	hi_addr = org + (bytesize - 1);
	ResetMemory();
	return true;
}

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
void Memory<PHYSICAL_ADDR, PAGE_SIZE>::ResetMemory()
{
	hash_table.Reset();
	memory_usage = 0;

	LoadFromInputFile();
	SaveToOutputFile();
}

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
bool Memory<PHYSICAL_ADDR, PAGE_SIZE>::WriteMemory(PHYSICAL_ADDR physical_addr, const void *buffer, uint32_t size)
{
	if(!size) return true;
	uint32_t copied;
	PHYSICAL_ADDR addr;

	if(physical_addr < lo_addr || (physical_addr + (size - 1)) > hi_addr || (physical_addr + size) < physical_addr) return false;
	// the third condition is for testing overwrapping (gdb did it !)

	PHYSICAL_ADDR key;
	MemoryPage<PHYSICAL_ADDR, PAGE_SIZE> *page;
	copied = 0;
	
	addr = physical_addr - lo_addr;
	do
	{
		uint32_t copy_size;
		uint32_t max_copy_size;
	
		key = (addr + copied) / PAGE_SIZE;
		page = hash_table.Find(key);
		if(!page)
		{
			page = new MemoryPage<PHYSICAL_ADDR, PAGE_SIZE>(key, initial_byte_value);
			hash_table.Insert(page);
			memory_usage += PAGE_SIZE;
			if(unlikely(output_file))
			{
				UpdateOutputFile(key * PAGE_SIZE, page->storage, PAGE_SIZE);
			}
		}
	
		max_copy_size = PAGE_SIZE - ((addr + copied) & (PAGE_SIZE - 1));
	
		if(size - copied > max_copy_size)
			copy_size = max_copy_size;
		else
			copy_size = size - copied;
		
		memcpy(&page->storage[(addr + copied) & (PAGE_SIZE - 1)],
		       &((uint8_t *)buffer)[copied],
		       copy_size);
	
		if(unlikely(output_file))
		{
			UpdateOutputFile(addr + copied, &page->storage[(addr + copied) & (PAGE_SIZE - 1)], copy_size);
		}
		
		copied += copy_size;
	} while(copied != size);
	
	return true;
}

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
bool Memory<PHYSICAL_ADDR, PAGE_SIZE>::ReadMemory(PHYSICAL_ADDR physical_addr, void *buffer, uint32_t size)
{
	if(!size) return true;
	uint32_t copied;
	PHYSICAL_ADDR addr;

	if(physical_addr < lo_addr || (physical_addr + (size - 1)) > hi_addr || (physical_addr + size) < physical_addr) return false;
	// the third condition is for testing overwrapping (gdb did it !)
	
	PHYSICAL_ADDR key = physical_addr / PAGE_SIZE;
	MemoryPage<PHYSICAL_ADDR, PAGE_SIZE> *page;
	copied = 0;
	
	addr = physical_addr - lo_addr;
	do
	{
		uint32_t copy_size;
		uint32_t max_copy_size;
	
		key = (addr + copied) / PAGE_SIZE;
		page = hash_table.Find(key);
		if(!page)
		{
			page = new MemoryPage<PHYSICAL_ADDR, PAGE_SIZE>(key, initial_byte_value);
			hash_table.Insert(page);
			memory_usage += PAGE_SIZE;
			if(unlikely(output_file))
			{
				UpdateOutputFile(key * PAGE_SIZE, page->storage, PAGE_SIZE);
			}
		}
	
		max_copy_size = PAGE_SIZE - ((addr + copied) & (PAGE_SIZE - 1));
	
		if(size - copied > max_copy_size)
			copy_size = max_copy_size;
		else
			copy_size = size - copied;

		memcpy(&((uint8_t *)buffer)[copied], 
		       &page->storage[(addr + copied) & (PAGE_SIZE - 1)],
		       copy_size);
	
		copied += copy_size;
	} while(copied != size);
	
	return true;
}

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
bool Memory<PHYSICAL_ADDR, PAGE_SIZE>::WriteMemory(PHYSICAL_ADDR physical_addr, const void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width)
{
	if(!size) return true;
	uint32_t offset;
	PHYSICAL_ADDR addr;
	uint32_t byte_enable_offset;
	uint32_t copied;

	if(physical_addr < lo_addr || (physical_addr + (size - 1)) > hi_addr || (physical_addr + size) < physical_addr) return false;
	// the third condition is for testing overwrapping (gdb did it !)

	PHYSICAL_ADDR key;
	MemoryPage<PHYSICAL_ADDR, PAGE_SIZE> *page;
	byte_enable_offset = 0;
	copied = 0;
	offset = 0;
	if(streaming_width == 0) streaming_width = size;
	
	addr = physical_addr - lo_addr;

	do
	{
		uint32_t copy_size;
		uint32_t max_page_copy_size;
	
		key = (addr + offset) / PAGE_SIZE;
		page = hash_table.Find(key);
		if(!page)
		{
			page = new MemoryPage<PHYSICAL_ADDR, PAGE_SIZE>(key, initial_byte_value);
			hash_table.Insert(page);
			memory_usage += PAGE_SIZE;
			if(unlikely(output_file))
			{
				UpdateOutputFile(key * PAGE_SIZE, page->storage, PAGE_SIZE);
			}
		}
	
		max_page_copy_size = PAGE_SIZE - ((addr + offset) & (PAGE_SIZE - 1));

		do
		{
			// Compute how many bytes remain in the buffer
			copy_size = size - offset;

			// Saturate the amount of bytes with the maximum allowed for the page
			if(copy_size > max_page_copy_size) copy_size = max_page_copy_size;

			// Compute how many bytes remain until the end of the streaming window
			uint32_t max_streaming_copy_size = streaming_width - offset;

			// Saturate the amount of bytes with the maximum allowed for the streaming window
			if(copy_size > max_streaming_copy_size) copy_size = max_streaming_copy_size;
	
			// Compute the offset from the beginning of the page
			uint32_t page_offset = (addr + offset) & (PAGE_SIZE - 1);

			if(byte_enable_length)
			{
				uint8_t *src = &((uint8_t *)buffer)[copied];
				uint8_t *dst = &page->storage[page_offset];

				do
				{
					uint8_t mask = byte_enable[byte_enable_offset];
					// Combine the buffer and the page content according to the byte enable buffer
					*dst = ((*dst) & ~mask) | ((*src) & mask);
					// cycle through the byte enable buffer
					if(++byte_enable_offset >= byte_enable_length) byte_enable_offset = 0;
				} while(++src, ++dst, --copy_size);
			}
			else
			{
				memcpy(&page->storage[page_offset], &((uint8_t *)buffer)[copied], copy_size);
			}

			if(unlikely(output_file))
			{
				UpdateOutputFile(addr + offset, &page->storage[page_offset], copy_size);
			}

			offset += copy_size;
			copied += copy_size;

			max_page_copy_size -= copy_size;

			// Check if the end of the streaming window has been reached
			if(offset == streaming_width)
			{
				// Restart from the initial address
				offset = 0;
				if((addr / PAGE_SIZE) != key)
				{
					// Force another page lookup
					break;
				}
			}
		} while((copied != size) && max_page_copy_size); // Loop until some bytes remain
	} while(copied != size);
	
	return true;
}

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
bool Memory<PHYSICAL_ADDR, PAGE_SIZE>::ReadMemory(PHYSICAL_ADDR physical_addr, void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width)
{
	if(!size) return true;
	uint32_t offset;
	PHYSICAL_ADDR addr;
	uint32_t byte_enable_offset;
	uint32_t copied;

	if(physical_addr < lo_addr || (physical_addr + (size - 1)) > hi_addr || (physical_addr + size) < physical_addr) return false;
	// the third condition is for testing overwrapping (gdb did it !)

	PHYSICAL_ADDR key;
	MemoryPage<PHYSICAL_ADDR, PAGE_SIZE> *page;
	byte_enable_offset = 0;
	copied = 0;
	offset = 0;
	if(streaming_width == 0) streaming_width = size;
	
	addr = physical_addr - lo_addr;

	do
	{
		uint32_t copy_size;
		uint32_t max_page_copy_size;
	
		key = (addr + offset) / PAGE_SIZE;
		page = hash_table.Find(key);
		if(!page)
		{
			page = new MemoryPage<PHYSICAL_ADDR, PAGE_SIZE>(key, initial_byte_value);
			hash_table.Insert(page);
			memory_usage += PAGE_SIZE;
			if(unlikely(output_file))
			{
				UpdateOutputFile(key * PAGE_SIZE, page->storage, PAGE_SIZE);
			}
		}
	
		max_page_copy_size = PAGE_SIZE - ((addr + offset) & (PAGE_SIZE - 1));

		do
		{
			// Compute how many bytes remain in the buffer
			copy_size = size - offset;

			// Saturate the amount of bytes with the maximum allowed for the page
			if(copy_size > max_page_copy_size) copy_size = max_page_copy_size;

			// Compute how many bytes remain until the end of the streaming window
			uint32_t max_streaming_copy_size = streaming_width - offset;

			// Saturate the amount of bytes with the maximum allowed for the streaming window
			if(copy_size > max_streaming_copy_size) copy_size = max_streaming_copy_size;
	
			// Compute the offset from the beginning of the page
			uint32_t page_offset = (addr + offset) & (PAGE_SIZE - 1);

			if(byte_enable_length)
			{
				uint8_t *src = &page->storage[page_offset];
				uint8_t *dst = &((uint8_t *)buffer)[copied];
				uint32_t copy_left = copy_size;
				do
				{
					uint8_t mask = byte_enable[byte_enable_offset];
					// Combine the buffer and the page content according to the byte enable buffer
					*dst = ((*dst) & ~mask) | ((*src) & mask);
					// cycle through the byte enable buffer
					if(++byte_enable_offset >= byte_enable_length) byte_enable_offset = 0;
				} while (++src, ++dst, --copy_left);
			}
			else
			{
				memcpy(&((uint8_t *)buffer)[copied], &page->storage[page_offset], copy_size);
			}

			offset += copy_size;
			copied += copy_size;

			max_page_copy_size -= copy_size;

			// Check if the end of the streaming window has been reached
			if(offset == streaming_width)
			{
				// Restart from the initial address
				offset = 0;
				if((addr / PAGE_SIZE) != key)
				{
					// Force another page lookup
					break;
				}
			}
		} while((copied != size) && max_page_copy_size); // Loop until some bytes remain
	} while(copied != size);
	
	return true;
}

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
void *Memory<PHYSICAL_ADDR, PAGE_SIZE>::GetDirectAccess(PHYSICAL_ADDR physical_addr, PHYSICAL_ADDR& physical_start_addr, PHYSICAL_ADDR& physical_end_addr)
{
	PHYSICAL_ADDR addr;

	if(physical_addr < lo_addr || physical_addr > hi_addr) return 0;

	PHYSICAL_ADDR key;
	MemoryPage<PHYSICAL_ADDR, PAGE_SIZE> *page;
	
	addr = physical_addr - lo_addr;
	key = addr / PAGE_SIZE;
	page = hash_table.Find(key);
	if(!page)
	{
		page = new MemoryPage<PHYSICAL_ADDR, PAGE_SIZE>(key, initial_byte_value);
		hash_table.Insert(page);
		memory_usage += PAGE_SIZE;
	}
	
	PHYSICAL_ADDR page_start_addr = key * PAGE_SIZE;
	PHYSICAL_ADDR page_end_addr = page_start_addr + PAGE_SIZE - 1;
	
	physical_start_addr = page_start_addr + lo_addr;
	physical_end_addr = page_end_addr + lo_addr;
	
	if(physical_start_addr > hi_addr) physical_start_addr = hi_addr;
	if(physical_end_addr > hi_addr) physical_end_addr = hi_addr;
	
	return page->storage;
}

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
void Memory<PHYSICAL_ADDR, PAGE_SIZE>::LoadFromInputFile()
{
	if(!input_filename.empty())
	{
		std::ifstream input_file(input_filename.c_str(), std::ios_base::binary);
		
		if(input_file.is_open())
		{
			input_file.seekg(0, input_file.end);
			
			if(input_file.bad())
			{
				logger << DebugWarning << "I/O error while seeking to end of \"" << input_filename << "\"" << EndDebugWarning;
			}
			else
			{
				std::streampos file_length = input_file.tellg();
				
				input_file.seekg(0, input_file.beg);
				
				if(input_file.bad())
				{
					logger << DebugWarning << "I/O error while seeking to begin of \"" << input_filename << "\"" << EndDebugWarning;
				}
				else
				{
					std::streamsize rem_file_length = file_length;
					
					if(verbose)
					{
						logger << DebugInfo << "Loading " << file_length << " bytes from \"" << input_filename << "\"" << EndDebugInfo;
					}
				
					PHYSICAL_ADDR file_offset = 0;
					
					do
					{
						PHYSICAL_ADDR key = (org + file_offset) / PAGE_SIZE;
						MemoryPage<PHYSICAL_ADDR, PAGE_SIZE> *page = hash_table.Find(key);
						if(!page)
						{
							page = new MemoryPage<PHYSICAL_ADDR, PAGE_SIZE>(key, initial_byte_value);
							hash_table.Insert(page);
							memory_usage += PAGE_SIZE;
						}
						
						std::streamsize n = (rem_file_length > PAGE_SIZE) ? PAGE_SIZE : rem_file_length;
						
						input_file.read((char *) page->storage, n);
					
						if(input_file.bad())
						{
							logger << DebugWarning << "I/O error while reading from \"" << input_filename << "\"" << EndDebugWarning;
							break;
						}
						
						rem_file_length -= n;
						file_offset += n;
					}
					while(rem_file_length > 0);

					if(verbose)
					{
						logger << DebugInfo << file_offset << " bytes loaded from \"" << input_filename << "\"" << EndDebugInfo;
					}
				}
			}
		}
		else
		{
			logger << DebugWarning << "Can't open for input \"" << input_filename << "\"" << EndDebugWarning;
		}
	}
}

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
void Memory<PHYSICAL_ADDR, PAGE_SIZE>::SaveToOutputFile()
{
	if(!output_filename.empty())
	{
		if(!output_file)
		{
			output_file = new std::ofstream(output_filename.c_str(), std::ios_base::binary);
		}

		if(output_file->is_open())
		{
			std::map<PHYSICAL_ADDR, MemoryPage<PHYSICAL_ADDR, PAGE_SIZE> *> map = (std::map<PHYSICAL_ADDR, MemoryPage<PHYSICAL_ADDR, PAGE_SIZE> *>) hash_table;
			typename std::map<PHYSICAL_ADDR, MemoryPage<PHYSICAL_ADDR, PAGE_SIZE> *>::const_iterator it;
			
			for(it = map.begin(); it != map.end(); it++)
			{
				MemoryPage<PHYSICAL_ADDR, PAGE_SIZE> *page = (*it).second;
				
				UpdateOutputFile(page->key * PAGE_SIZE, page->storage, PAGE_SIZE);
			}
		}
		else
		{
			logger << DebugWarning << "Can't open for output \"" << output_filename << "\"" << EndDebugWarning;
			delete output_file;
			output_file = 0;
			unisim::kernel::Object::Stop(-1);
		}
	}
}

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE>
void Memory<PHYSICAL_ADDR, PAGE_SIZE>::UpdateOutputFile(PHYSICAL_ADDR addr, const void *buffer, unsigned int length)
{
	if(output_file)
	{
		uint64_t offset = addr - org;
		
		output_file->seekp(offset, std::ios_base::beg);
		
		if(output_file->bad())
		{
			logger << DebugWarning << "I/O error while seeking in \"" << output_filename << "\"" << EndDebugWarning;
			return;
		}

		if(verbose)
		{
			logger << DebugInfo << "Writing " << length << " bytes at offset 0x" << std::hex << offset << std::dec << " to \"" << output_filename << "\"" << EndDebugInfo;
		}

		output_file->write((const char *) buffer, length);
		
		if(output_file->bad())
		{
			logger << DebugWarning << "I/O error while writing to \"" << output_filename << "\"" << EndDebugWarning;
			return;
		}
		
		output_file->flush();
	}
}

} // end of namespace ram
} // end of namespace memory
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim 

#endif // __UNISIM_COMPONENT_CXX_MEMORY_RAM_MEMORY_TCC__
