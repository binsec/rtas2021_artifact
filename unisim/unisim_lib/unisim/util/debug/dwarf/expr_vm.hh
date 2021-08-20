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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_EXPR_VM_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_EXPR_VM_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/util/endian/endian.hh>
#include <set>
#include <vector>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

const unsigned int DW_LOC_PIECE_NULL = 0;
const unsigned int DW_LOC_PIECE_REGISTER = 1;
const unsigned int DW_LOC_PIECE_MEMORY = 2;

template <class MEMORY_ADDR>
class DWARF_LocationPiece
{
public:
	DWARF_LocationPiece(unsigned int dw_bit_size);
	DWARF_LocationPiece(unsigned int dw_loc_type, unsigned int dw_bit_size);
	~DWARF_LocationPiece();
	unsigned int GetType() const;
	unsigned int GetBitSize() const;
private:
	unsigned int dw_loc_piece_type;
	unsigned int dw_bit_size;
};

template <class MEMORY_ADDR>
class DWARF_MemoryLocationPiece : public DWARF_LocationPiece<MEMORY_ADDR>
{
public:
	DWARF_MemoryLocationPiece(MEMORY_ADDR dw_addr);
	DWARF_MemoryLocationPiece(MEMORY_ADDR dw_addr, unsigned int dw_bit_offset, unsigned int dw_bit_size);
	~DWARF_MemoryLocationPiece();
	MEMORY_ADDR GetAddress() const;
	unsigned int GetBitOffset() const;
private:
	MEMORY_ADDR dw_addr;
	unsigned int dw_bit_offset;
};

template <class MEMORY_ADDR>
class DWARF_RegisterLocationPiece : public DWARF_LocationPiece<MEMORY_ADDR>
{
public:
	DWARF_RegisterLocationPiece(unsigned int dw_reg_num);
	DWARF_RegisterLocationPiece(unsigned int dw_reg_num, unsigned int dw_bit_offset, unsigned int dw_bit_size);
	~DWARF_RegisterLocationPiece();
	unsigned int GetRegisterNumber() const;
	unsigned int GetBitOffset() const;
private:
	unsigned int dw_reg_num;
	unsigned int dw_bit_offset;
};

const unsigned int DW_LOC_NULL                  = 0;
const unsigned int DW_LOC_SIMPLE_REGISTER       = 1;
const unsigned int DW_LOC_SIMPLE_MEMORY         = 2;
const unsigned int DW_LOC_COMPOSITE             = 3;
const unsigned int DW_LOC_IMPLICIT_SIMPLE_VALUE = 4;
const unsigned int DW_LOC_IMPLICIT_BLOCK_VALUE  = 5;

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Location<MEMORY_ADDR>& dw_loc);

template <class MEMORY_ADDR>
class DWARF_Location
{
public:
	DWARF_Location();
	~DWARF_Location();
	void Clear();
	void ClearRanges();
	unsigned int GetType() const;
	void Add(DWARF_LocationPiece<MEMORY_ADDR> *dw_loc_piece);
	const std::vector<DWARF_LocationPiece<MEMORY_ADDR> *>& GetLocationPieces() const;
	void SetRegisterNumber(unsigned int dw_reg_num);
	void SetAddress(MEMORY_ADDR dw_addr);
	void SetImplicitValue(MEMORY_ADDR dw_implicit_value);
	void SetImplicitValue(DWARF_Block<MEMORY_ADDR> *dw_implicit_value);
	unsigned int GetRegisterNumber() const;
	MEMORY_ADDR GetAddress() const;
	MEMORY_ADDR GetImplicitSimpleValue() const;
	const DWARF_Block<MEMORY_ADDR> *GetImplicitBlockValue() const;
	void SetByteSize(uint64_t byte_size);
	void SetBitOffset(int64_t bit_offset);
	void SetBitSize(uint64_t bit_size);
	void SetEncoding(uint8_t encoding);
	void SetRanges(const std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >& ranges);
	uint64_t GetByteSize() const;
	int64_t GetBitOffset() const;
	uint64_t GetBitSize() const;
	uint8_t GetEncoding() const;
	const std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >& GetRanges() const;
	std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >& GetRanges();
	
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_Location<MEMORY_ADDR>& dw_loc);
private:
	unsigned int dw_loc_type;
	unsigned int dw_reg_num;
	MEMORY_ADDR dw_addr;
	MEMORY_ADDR dw_implicit_simple_value;
	DWARF_Block<MEMORY_ADDR> *dw_implicit_block_value;
	uint64_t dw_byte_size;
	int64_t dw_bit_offset;
	uint64_t dw_bit_size;
	uint8_t dw_encoding;
	std::vector<DWARF_LocationPiece<MEMORY_ADDR> *> dw_location_pieces;
	std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> > ranges;
};

template <class MEMORY_ADDR>
class DWARF_ExpressionVM
{
public:
	DWARF_ExpressionVM(const DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int prc_num);
	DWARF_ExpressionVM(const DWARF_Handler<MEMORY_ADDR> *dw_handler, DWARF_Frame<MEMORY_ADDR> *dw_frame);
	~DWARF_ExpressionVM();
	
	bool Disasm(std::ostream& os, const DWARF_Expression<MEMORY_ADDR> *dw_expr);
	bool Execute(const DWARF_Expression<MEMORY_ADDR> *dw_expr, MEMORY_ADDR& result_addr, DWARF_Location<MEMORY_ADDR> *dw_location);
	void SetFrameBase(MEMORY_ADDR frame_base);
	void SetObjectAddress(MEMORY_ADDR object_addr);
	void SetPC(MEMORY_ADDR pc);
	void Push(MEMORY_ADDR addr);
private:
	const DWARF_Handler<MEMORY_ADDR> *dw_handler;
	unsigned int prc_num;
	const DWARF_RegisterNumberMapping *reg_num_mapping;
	unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if;
	DWARF_Frame<MEMORY_ADDR> *dw_frame;
	unisim::util::endian::endian_type file_endianness;
	unisim::util::endian::endian_type arch_endianness;
	unsigned int file_address_size;
	unsigned int arch_address_size;
	std::vector<MEMORY_ADDR> dw_stack;
	MEMORY_ADDR frame_base;
	bool has_frame_base;
	MEMORY_ADDR object_addr;
	bool has_object_addr;
	MEMORY_ADDR pc;
	bool has_pc;
	bool debug;
	std::ostream& debug_info_stream;
	std::ostream& debug_warning_stream;
	std::ostream& debug_error_stream;

	bool Run(const DWARF_Expression<MEMORY_ADDR> *dw_expr, std::ostream *os, MEMORY_ADDR *result_addr, DWARF_Location<MEMORY_ADDR> *dw_location);

	bool ReadRegister(unsigned int dw_reg_num, MEMORY_ADDR& reg_value) const;
	bool ReadAddrFromMemory(MEMORY_ADDR addr, MEMORY_ADDR& read_addr, unsigned int read_size = 0, MEMORY_ADDR addr_space = 0) const;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
