/*
 *  Copyright (c) 2010,
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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_LOC_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_LOC_HH__

#include <unisim/util/debug/dwarf/fwd.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_LocListEntry<MEMORY_ADDR>& dw_loc_list_entry);

template <class MEMORY_ADDR>
class DWARF_LocListEntry
{
public:
	DWARF_LocListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu);
	~DWARF_LocListEntry();
	bool IsBaseAddressSelection() const;
	bool IsEndOfList() const;
	MEMORY_ADDR GetBegin() const;
	MEMORY_ADDR GetEnd() const;
	MEMORY_ADDR GetBaseAddress() const;
	bool HasOverlap(MEMORY_ADDR base_addr, MEMORY_ADDR addr, MEMORY_ADDR length) const;
	uint64_t GetOffset() const;
	const DWARF_Expression<MEMORY_ADDR> *GetLocationExpression() const;
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int id);
	std::string GetHREF() const;
	unsigned int GetId() const;
	const DWARF_LocListEntry<MEMORY_ADDR> *GetNext() const;
	int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	std::ostream& to_XML(std::ostream& os) const;
	std::ostream& to_HTML(std::ostream& os) const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_LocListEntry<MEMORY_ADDR>& dw_loc_list_entry);
private:
	friend class DWARF_Handler<MEMORY_ADDR>;
	uint64_t offset;
	unsigned int id;
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	DWARF_LocListEntry<MEMORY_ADDR> *next;
	MEMORY_ADDR begin;
	MEMORY_ADDR end;
	DWARF_Expression<MEMORY_ADDR> *dw_loc_expr;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
