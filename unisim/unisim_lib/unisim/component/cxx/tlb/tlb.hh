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

#ifndef __UNISIM_COMPONENT_CXX_TLB_TLB_HH__
#define __UNISIM_COMPONENT_CXX_TLB_TLB_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace tlb {

template <class CONFIG> class TLBEntry;
template <class CONFIG> class TLBSet;
template <class CONFIG> class TLB;

template <class CONFIG>
class TLBEntry
{
public:
	typedef typename CONFIG::ENTRY_STATUS STATUS;
	typedef typename CONFIG::VIRTUAL_ADDRESS VIRTUAL_ADDRESS;
	typedef typename CONFIG::PTE PTE;
	static const uint32_t PAGE_SIZE = CONFIG::PAGE_SIZE;

	TLBEntry();
	~TLBEntry();
	inline void SetBaseVirtualAddr(VIRTUAL_ADDRESS base_virtual_addr);
	inline VIRTUAL_ADDRESS GetBaseVirtualAddr() const;

	STATUS status;
	PTE pte;
protected:
private:
	VIRTUAL_ADDRESS base_virtual_addr;
};

template <class CONFIG>
class TLBSet
{
public:
	typedef typename CONFIG::SET_STATUS STATUS;
	typedef typename CONFIG::VIRTUAL_ADDRESS VIRTUAL_ADDRESS;
	static const uint32_t ASSOCIATIVITY = CONFIG::TLB_ASSOCIATIVITY;

	TLBSet();
	~TLBSet();
	inline TLBEntry<CONFIG>& operator [] (uint32_t way);

	STATUS status;
protected:
private:
	TLBEntry<CONFIG> entries[ASSOCIATIVITY];
};

template <class CONFIG>
class TLB
{
public:
	typedef typename CONFIG::VIRTUAL_ADDRESS VIRTUAL_ADDRESS;
	static const uint32_t NUM_SETS = CONFIG::TLB_NUM_ENTRIES / TLBSet<CONFIG>::ASSOCIATIVITY;

	TLB();
	~TLB();

	static inline void DecodeAddress(VIRTUAL_ADDRESS addr, VIRTUAL_ADDRESS& base_virtual_addr, uint32_t& index);
	inline TLBSet<CONFIG>& operator [] (uint32_t index);

protected:
private:
	TLBSet<CONFIG> sets[NUM_SETS];
};

} // end of namespace tlb
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_TLB_TLB_HH__
