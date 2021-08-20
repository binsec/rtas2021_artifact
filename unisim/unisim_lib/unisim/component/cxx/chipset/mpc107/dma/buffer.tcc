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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_DMA_BUFFER_TCC__
#define __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_DMA_BUFFER_TCC__

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {
namespace dma {

template <class PHYSICAL_ADDR>
Buffer<PHYSICAL_ADDR> ::
Buffer()
	: buffer_size(0)
	, init_source_addr(0)
	, init_destination_addr(0)
	, transfer_size(0)
	, transferred_size(0)
{
}

template <class PHYSICAL_ADDR>
Buffer<PHYSICAL_ADDR> ::
~Buffer() {
}

template <class PHYSICAL_ADDR>
void
Buffer<PHYSICAL_ADDR> ::
Initialize(PHYSICAL_ADDR source_addr, 
					 PHYSICAL_ADDR destination_addr,
					 PHYSICAL_ADDR _size) {
	init_source_addr = source_addr;
	init_destination_addr = destination_addr;
	transfer_size = _size;
	transferred_size = 0;
	buffer_size = 0;
}

template <class PHYSICAL_ADDR>
void 
Buffer<PHYSICAL_ADDR> ::
Write(Pointer<uint8_t> &_data, unsigned int _size) {
	Pointer<BufferEntry> entry = new(entry) BufferEntry();
	entry->data = new(entry->data) uint8_t[_size];
	for(unsigned int i = 0; i < _size; i++)
		entry->data[i] = _data[i];
	entry->size = _size;
	entry->index = 0;

	if(buffer_size == 0) {
		buffer = entry;
		last_buffer = entry;
	} else {
		last_buffer->next = entry;
	}
	buffer_size += _size;
}

template <class PHYSICAL_ADDR>
unsigned int
Buffer<PHYSICAL_ADDR> ::
Read(Pointer<uint8_t> &_data, unsigned int _size) {
	unsigned int i = 0;
	unsigned int ret_size = buffer_size;

	while((i != _size) && (buffer_size != 0)) {
		if(buffer->size <= (_size - i)) {
			for(unsigned int j = 0; j < buffer->size; j++) {
				_data[i] = buffer->data[j + buffer->index];
				i++;
			}
			buffer_size -= buffer->size;
			transferred_size += buffer->size;
			buffer = buffer->next;
		} else {
			unsigned int j;

			for(j = 0; j < (_size - i); j++) {
				_data[i] = buffer->data[j + buffer->index];
				i++;
			}
			buffer_size -= j;
			transferred_size += j;
			buffer->index = j;
			buffer->size = buffer->size - j;
		}
	}

	ret_size -= buffer_size;
	return ret_size;
}

/**
 * Gets the size of the buffer.
 *
 * @return the size of the buffer
 */
template <class PHYSICAL_ADDR>
unsigned int
Buffer<PHYSICAL_ADDR> ::
Size() {
	return buffer_size;
}

/**
 * Returns true if the transferred number of bytes is equal than the 
 *   transfer size.
 *
 * @return true if the transferred number of bytes is equal than the transfer
 *   size, false otherwise
 */
template <class PHYSICAL_ADDR>
bool
Buffer<PHYSICAL_ADDR> ::
TransferFinished() {
	return transfer_size == transferred_size;
}

/**
 * Returns true if data is still expected by the buffer.
 *
 * @return true if data is still expected by the buffer
 */
template <class PHYSICAL_ADDR>
bool
Buffer<PHYSICAL_ADDR> ::
ExpectsData() {
	return (transferred_size + buffer_size) != transfer_size;
}

} // end of namespace dma
} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace dma

#endif // __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_DMA_BUFFER_TCC__
