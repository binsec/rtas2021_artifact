/*
 *  Copyright (c) 2009,
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

#include <inttypes.h>

#include <unisim/service/interfaces/memory.hh>
#include <unisim/kernel/kernel.hh>

namespace unisim {
namespace service {
namespace translator {
namespace memory_address {
namespace memory {

using unisim::service::interfaces::Memory;

using unisim::kernel::Object;
using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;

template <class FROM_ADDRESS, class TO_ADDRESS>
class Translator :
	public Service<Memory<FROM_ADDRESS> >,
	public Client<Memory<TO_ADDRESS> >
{
public:
	ServiceExport<Memory<FROM_ADDRESS> > memory_export;
	ServiceImport<Memory<TO_ADDRESS> > memory_import;

	Translator(const char* name, Object *parent = 0);
	virtual ~Translator();
	
	virtual void ResetMemory();
	virtual bool ReadMemory(FROM_ADDRESS addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(FROM_ADDRESS addr, const void *buffer, uint32_t size);
};

} // end of namespace memory
} // end of namespace memory_address
} // end of namespace translator 
} // end of namespace service
} // end of namespace unisim 





