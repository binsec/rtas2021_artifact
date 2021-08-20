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

#ifndef __UNISIM_COMPONENT_CXX_TLB_TLB_TCC__
#define __UNISIM_COMPONENT_CXX_TLB_TLB_TCC__

#include <string.h>

namespace unisim {
namespace component {
namespace cxx {
namespace tlb {

template <class CONFIG>
TLBEntry<CONFIG>::TLBEntry() : base_virtual_addr(0)
{
}

template <class CONFIG>
TLBEntry<CONFIG>::~TLBEntry()
{
}

template <class CONFIG>
inline void TLBEntry<CONFIG>::SetBaseVirtualAddr(VIRTUAL_ADDRESS base_virtual_addr)
{
	this->base_virtual_addr = base_virtual_addr;
}

template <class CONFIG>
inline typename TLBEntry<CONFIG>::VIRTUAL_ADDRESS TLBEntry<CONFIG>::GetBaseVirtualAddr() const
{
	return base_virtual_addr;
}

template <class CONFIG>
TLBSet<CONFIG>::TLBSet()
{
}

template <class CONFIG>
TLBSet<CONFIG>::~TLBSet()
{
}

template <class CONFIG>
TLBEntry<CONFIG>& TLBSet<CONFIG>::operator [] (uint32_t way)
{
	return entries[way];
}

template <class CONFIG>
TLB<CONFIG>::TLB()
{
}

template <class CONFIG>
TLB<CONFIG>::~TLB()
{
}

template <class CONFIG>
inline void TLB<CONFIG>::DecodeAddress(VIRTUAL_ADDRESS addr, VIRTUAL_ADDRESS& base_virtual_addr, uint32_t& index)
{
	base_virtual_addr = addr & (~((VIRTUAL_ADDRESS) CONFIG::PAGE_SIZE - 1));
	index = (addr / CONFIG::PAGE_SIZE) % NUM_SETS;
}

template <class CONFIG>
TLBSet<CONFIG>& TLB<CONFIG>::operator [] (uint32_t index)
{
	return sets[index];
}

} // end of namespace tlb
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_TLB_TLB_TCC__
