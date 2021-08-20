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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_FDE_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_FDE_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/dwarf/fmt.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_FDE<MEMORY_ADDR>& dw_fde);

template <class MEMORY_ADDR>
class DWARF_FDE
{
public:
	DWARF_FDE(DWARF_Handler<MEMORY_ADDR> *dw_handler, DWARF_FrameSectionType dw_fst);
	~DWARF_FDE();
	
	int64_t Load(const uint8_t *rawdata, uint64_t max_size, MEMORY_ADDR section_addr, uint64_t offset);
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int id);
	unsigned int GetId() const;
	const DWARF_CIE<MEMORY_ADDR> *GetCIE() const;
	const DWARF_CallFrameProgram<MEMORY_ADDR> *GetInstructions() const;
	MEMORY_ADDR GetInitialLocation() const;
	MEMORY_ADDR GetAddressRange() const;
	bool IsTerminator() const;
	std::ostream& to_XML(std::ostream& os) const;
	std::ostream& to_HTML(std::ostream& os) const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_FDE<MEMORY_ADDR>& dw_fde);
private:
	DWARF_Handler<MEMORY_ADDR> *dw_handler;
	DWARF_FrameSectionType dw_fst;
	DWARF_Format dw_fmt;
	uint64_t offset;
	unsigned int id;

	uint64_t length;               // length not including field 'length'
	
	uint64_t cie_pointer;          // A constant offset into the .debug_frame section that denotes the CIE that is associated with this FDE.

	MEMORY_ADDR initial_location;  // An addressing-unit sized constant indicating the address of the first location associated with this table entry.
	                               // Also named PC begin in GNU .eh_frame

	MEMORY_ADDR address_range;     // An addressing unit sized constant indicating the number of bytes of program instructions described by this entry.
	                               // Also named PC range in GNU .eh_frame

	DWARF_LEB128 augmentation_length;                // GNU extension: augmentation length (in .eh_frame with augmentation string containing "z")
	
	DWARF_Block<MEMORY_ADDR> *augmentation_data;     // GNU extension: augmentation data (in .eh_frame with augmentation string containing "z")

	DWARF_CallFrameProgram<MEMORY_ADDR> *dw_call_frame_prog;
	const DWARF_CIE<MEMORY_ADDR> *dw_cie;
	
	unsigned int SizeOfEncodedPointer(uint8_t encoding) const;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
