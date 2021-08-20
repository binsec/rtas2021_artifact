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
 
#ifndef __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_DMA_BUFFER_HH__
#define __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_DMA_BUFFER_HH__

#include <inttypes.h>
#include "unisim/util/garbage_collector/garbage_collector.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {
namespace dma {

using unisim::util::garbage_collector::Pointer;

template <class PHYSICAL_ADDR>
class Buffer {
private:
	class BufferEntry {
	public:
		Pointer<uint8_t> data;
		unsigned int size;
		unsigned int index;
		Pointer<BufferEntry> next;
	};

	Pointer<BufferEntry> buffer;
	Pointer<BufferEntry> last_buffer;
	uint32_t buffer_size;

public:
	Buffer();
	~Buffer();
	void Initialize(PHYSICAL_ADDR source_addr, 
									PHYSICAL_ADDR destination_addr,
									PHYSICAL_ADDR _size);
	void Write(Pointer<uint8_t> &_data, unsigned int _size);
	unsigned int Read(Pointer<uint8_t> &_data, unsigned int _size);
	unsigned int Size();
	bool TransferFinished();
	bool ExpectsData();

	PHYSICAL_ADDR init_source_addr;
	PHYSICAL_ADDR init_destination_addr;
	PHYSICAL_ADDR transfer_size;
	PHYSICAL_ADDR transferred_size;
};

} // end of namespace dma
} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace dma

#endif // __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_DMA_BUFFER_HH__
