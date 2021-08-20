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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_CALL_FRAME_PROG_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_CALL_FRAME_PROG_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/dwarf/fmt.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

static const unsigned int DW_CFP_INITIAL_INSTRUCTIONS = 0;
static const unsigned int DW_CFP_INSTRUCTIONS = 1;
	
template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog);

template <class MEMORY_ADDR>
class DWARF_CallFrameProgram
{
public:
	DWARF_CallFrameProgram(DWARF_Handler<MEMORY_ADDR> *dw_handler, uint64_t length, const uint8_t *program, unsigned int type, DWARF_Format dw_fmt);
	~DWARF_CallFrameProgram();
	DWARF_Handler<MEMORY_ADDR> *GetHandler() const;
	DWARF_Format GetFormat() const;
	uint64_t GetLength() const;
	const uint8_t *GetProgram() const;
	unsigned int GetType() const;
	void SetCIE(const DWARF_CIE<MEMORY_ADDR> *dw_cie);
	const DWARF_CIE<MEMORY_ADDR> *GetCIE() const;
	void SetFDE(const DWARF_FDE<MEMORY_ADDR> *dw_fde);
	const DWARF_FDE<MEMORY_ADDR> *GetFDE() const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog);
private:
	friend class DWARF_CallFrameVM<MEMORY_ADDR>;

	unsigned int type;
	DWARF_Handler<MEMORY_ADDR> *dw_handler;
	uint64_t length;
	DWARF_Format dw_fmt;
	const uint8_t *program;
	const DWARF_CIE<MEMORY_ADDR> *dw_cie;
	const DWARF_FDE<MEMORY_ADDR> *dw_fde;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
