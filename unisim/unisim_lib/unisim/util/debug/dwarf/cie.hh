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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_CIE_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_CIE_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/dwarf/fmt.hh>
#include <unisim/util/debug/dwarf/version.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CIE<MEMORY_ADDR>& dw_cie);

template <class MEMORY_ADDR>
class DWARF_CIE
{
public:
	DWARF_CIE(DWARF_Handler<MEMORY_ADDR> *dw_handler, DWARF_FrameSectionType fst);
	~DWARF_CIE();
	
	int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int id);
	unsigned int GetId() const;
	DWARF_Version GetDWARFVersion() const;
	std::string GetHREF() const;
	const char *GetAugmentation() const;
	const DWARF_LEB128& GetCodeAlignmentFactor() const;
	const DWARF_LEB128& GetDataAlignmentFactor() const;
	unsigned int GetReturnAddressRegister() const;
	const DWARF_CallFrameProgram<MEMORY_ADDR> *GetInitialInstructions() const;
	bool IsTerminator() const;
	bool GetAugmentationData(char c, uint64_t& value) const;
	MEMORY_ADDR DecodePointer(MEMORY_ADDR pc, MEMORY_ADDR encoded_ptr) const;
	
	std::ostream& to_XML(std::ostream& os) const;
	std::ostream& to_HTML(std::ostream& os) const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_CIE<MEMORY_ADDR>& dw_cie);
private:
	DWARF_Handler<MEMORY_ADDR> *dw_handler;
	DWARF_FrameSectionType dw_fst;
	DWARF_Format dw_fmt;
	DWARF_Version dw_ver;
	uint64_t offset;
	unsigned int id;
	
	uint64_t length;          // length not including field 'length'
	
	uint64_t cie_id;          // 32-bit all 1's for 32-bit DWARF, 64-bit all 1's for 64-bit DWARF
	
	uint8_t version;          // Independent of DWARF version number. DWARF v2/version=1; DWARF v3/version=3
	
	const char *augmentation; // UTF-8 string
	
	uint64_t eh_data;         // GNU extension: EH Data (in .eh_frame with augmentation string containing "eh")
	
	DWARF_LEB128 code_alignment_factor; // unsigned
	
	DWARF_LEB128 data_alignment_factor; // signed
	
	uint8_t dw2_return_address_register;
	DWARF_LEB128 dw3_return_address_register; // unsigned
	
	DWARF_LEB128 augmentation_length;                // GNU extension: augmentation length (in .eh_frame with augmentation string containing "z")
	
	DWARF_Block<MEMORY_ADDR> *augmentation_data;     // GNU extension: augmentation data (in .eh_frame with augmentation string containing "z")
	
	DWARF_CallFrameProgram<MEMORY_ADDR> *dw_initial_call_frame_prog;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
