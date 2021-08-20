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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_EXPR_VM_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_EXPR_VM_TCC__

#include <unisim/util/debug/dwarf/expr_vm.hh>
#include <unisim/util/debug/dwarf/option.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/endian/endian.hh>
#include <stack>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

using unisim::util::arithmetic::SignExtend;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::endian::Target2Host;

template <class MEMORY_ADDR>
DWARF_LocationPiece<MEMORY_ADDR>::DWARF_LocationPiece(unsigned int _dw_bit_size)
	: dw_loc_piece_type(DW_LOC_PIECE_NULL)
	, dw_bit_size(_dw_bit_size)
{
}

template <class MEMORY_ADDR>
DWARF_LocationPiece<MEMORY_ADDR>::DWARF_LocationPiece(unsigned int _dw_loc_piece_type, unsigned int _dw_bit_size)
	: dw_loc_piece_type(_dw_loc_piece_type)
	, dw_bit_size(_dw_bit_size)
{
}

template <class MEMORY_ADDR>
DWARF_LocationPiece<MEMORY_ADDR>::~DWARF_LocationPiece()
{
}

template <class MEMORY_ADDR>
unsigned int DWARF_LocationPiece<MEMORY_ADDR>::GetType() const
{
	return dw_loc_piece_type;
}

template <class MEMORY_ADDR>
unsigned int DWARF_LocationPiece<MEMORY_ADDR>::GetBitSize() const
{
	return dw_bit_size;
}

template <class MEMORY_ADDR>
DWARF_MemoryLocationPiece<MEMORY_ADDR>::DWARF_MemoryLocationPiece(MEMORY_ADDR _dw_addr)
	: DWARF_LocationPiece<MEMORY_ADDR>(DW_LOC_PIECE_MEMORY, 0)
	, dw_addr(_dw_addr)
	, dw_bit_offset(0)
{
}

template <class MEMORY_ADDR>
DWARF_MemoryLocationPiece<MEMORY_ADDR>::DWARF_MemoryLocationPiece(MEMORY_ADDR _dw_addr, unsigned int _dw_bit_offset, unsigned int _dw_bit_size)
	: DWARF_LocationPiece<MEMORY_ADDR>(DW_LOC_PIECE_MEMORY, _dw_bit_size)
	, dw_addr(_dw_addr)
	, dw_bit_offset(_dw_bit_offset)
{
}

template <class MEMORY_ADDR>
DWARF_MemoryLocationPiece<MEMORY_ADDR>::~DWARF_MemoryLocationPiece()
{
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_MemoryLocationPiece<MEMORY_ADDR>::GetAddress() const
{
	return dw_addr;
}

template <class MEMORY_ADDR>
unsigned int DWARF_MemoryLocationPiece<MEMORY_ADDR>::GetBitOffset() const
{
	return dw_bit_offset;
}


template <class MEMORY_ADDR>
DWARF_RegisterLocationPiece<MEMORY_ADDR>::DWARF_RegisterLocationPiece(unsigned int _dw_reg_num)
	: DWARF_LocationPiece<MEMORY_ADDR>(DW_LOC_PIECE_REGISTER, 0)
	, dw_reg_num(_dw_reg_num)
	, dw_bit_offset(0)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterLocationPiece<MEMORY_ADDR>::DWARF_RegisterLocationPiece(unsigned int _dw_reg_num, unsigned int _dw_bit_offset, unsigned int _dw_bit_size)
	: DWARF_LocationPiece<MEMORY_ADDR>(DW_LOC_PIECE_REGISTER, _dw_bit_size)
	, dw_reg_num(_dw_reg_num)
	, dw_bit_offset(_dw_bit_offset)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterLocationPiece<MEMORY_ADDR>::~DWARF_RegisterLocationPiece()
{
}

template <class MEMORY_ADDR>
unsigned int DWARF_RegisterLocationPiece<MEMORY_ADDR>::GetRegisterNumber() const
{
	return dw_reg_num;
}

template <class MEMORY_ADDR>
unsigned int DWARF_RegisterLocationPiece<MEMORY_ADDR>::GetBitOffset() const
{
	return dw_bit_offset;
}

template <class MEMORY_ADDR>
DWARF_Location<MEMORY_ADDR>::DWARF_Location()
	: dw_loc_type(DW_LOC_NULL)
	, dw_reg_num(0)
	, dw_addr(0)
	, dw_implicit_simple_value(0)
	, dw_implicit_block_value(0)
	, dw_byte_size(0)
	, dw_bit_offset(0)
	, dw_bit_size(0)
	, dw_encoding(0)
{
}

template <class MEMORY_ADDR>
DWARF_Location<MEMORY_ADDR>::~DWARF_Location()
{
	Clear();
	ClearRanges();
}

template <class MEMORY_ADDR>
void DWARF_Location<MEMORY_ADDR>::Clear()
{
	dw_loc_type = DW_LOC_NULL;
	dw_reg_num = 0;
	dw_addr = 0;
	dw_implicit_simple_value = 0;
	dw_byte_size = 0;
	dw_bit_offset = 0;
	dw_bit_size = 0;
	dw_encoding = 0;

	if(dw_implicit_block_value)
	{
		delete dw_implicit_block_value;
		dw_implicit_block_value = 0;
	}

	unsigned int num_location_pieces = dw_location_pieces.size();
	unsigned int i;
	
	for(i = 0; i < num_location_pieces; i++)
	{
		DWARF_LocationPiece<MEMORY_ADDR> *dw_location_piece = dw_location_pieces[i];
		
		delete dw_location_piece;
	}
	dw_location_pieces.clear();
}

template <class MEMORY_ADDR>
void DWARF_Location<MEMORY_ADDR>::ClearRanges()
{
	ranges.clear();
}

template <class MEMORY_ADDR>
unsigned int DWARF_Location<MEMORY_ADDR>::GetType() const
{
	return dw_loc_type;
}

template <class MEMORY_ADDR>
void DWARF_Location<MEMORY_ADDR>::Add(DWARF_LocationPiece<MEMORY_ADDR> *dw_loc_piece)
{
	if((dw_loc_type != DW_LOC_NULL) && (dw_loc_type != DW_LOC_COMPOSITE)) throw std::runtime_error("Internal error");
	dw_loc_type = DW_LOC_COMPOSITE;
	dw_location_pieces.push_back(dw_loc_piece);
}

template <class MEMORY_ADDR>
const std::vector<DWARF_LocationPiece<MEMORY_ADDR> *>& DWARF_Location<MEMORY_ADDR>::GetLocationPieces() const
{
	return dw_location_pieces;
}

template <class MEMORY_ADDR>
void DWARF_Location<MEMORY_ADDR>::SetRegisterNumber(unsigned int _dw_reg_num)
{
	if(dw_loc_type != DW_LOC_NULL) throw std::runtime_error("Internal error");
	dw_loc_type = DW_LOC_SIMPLE_REGISTER;
	dw_reg_num = _dw_reg_num;
}

template <class MEMORY_ADDR>
void DWARF_Location<MEMORY_ADDR>::SetAddress(MEMORY_ADDR _dw_addr)
{
	if(dw_loc_type != DW_LOC_NULL) throw std::runtime_error("Internal error");
	dw_loc_type = DW_LOC_SIMPLE_MEMORY;
	dw_addr = _dw_addr;
}

template <class MEMORY_ADDR>
void DWARF_Location<MEMORY_ADDR>::SetImplicitValue(MEMORY_ADDR dw_implicit_value)
{
	if(dw_loc_type != DW_LOC_NULL) throw std::runtime_error("Internal error");
	dw_loc_type = DW_LOC_IMPLICIT_SIMPLE_VALUE;
	dw_implicit_simple_value = dw_implicit_value;
}

template <class MEMORY_ADDR>
void DWARF_Location<MEMORY_ADDR>::SetImplicitValue(DWARF_Block<MEMORY_ADDR> *dw_implicit_value)
{
	if(dw_loc_type != DW_LOC_NULL) throw std::runtime_error("Internal error");
	dw_loc_type = DW_LOC_IMPLICIT_BLOCK_VALUE;
	dw_implicit_block_value = dw_implicit_value;
}

template <class MEMORY_ADDR>
unsigned int DWARF_Location<MEMORY_ADDR>::GetRegisterNumber() const
{
	return dw_reg_num;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_Location<MEMORY_ADDR>::GetAddress() const
{
	return dw_addr;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_Location<MEMORY_ADDR>::GetImplicitSimpleValue() const
{
	return dw_implicit_simple_value;
}

template <class MEMORY_ADDR>
const DWARF_Block<MEMORY_ADDR> *DWARF_Location<MEMORY_ADDR>::GetImplicitBlockValue() const
{
	return dw_implicit_block_value;
}

template <class MEMORY_ADDR>
void DWARF_Location<MEMORY_ADDR>::SetByteSize(uint64_t _dw_byte_size)
{
	dw_byte_size = _dw_byte_size;
}

template <class MEMORY_ADDR>
void DWARF_Location<MEMORY_ADDR>::SetBitOffset(int64_t _dw_bit_offset)
{
	dw_bit_offset = _dw_bit_offset;
}

template <class MEMORY_ADDR>
void DWARF_Location<MEMORY_ADDR>::SetBitSize(uint64_t _dw_bit_size)
{
	dw_bit_size = _dw_bit_size;
}

template <class MEMORY_ADDR>
void DWARF_Location<MEMORY_ADDR>::SetEncoding(uint8_t _dw_encoding)
{
	dw_encoding = _dw_encoding;
}

template <class MEMORY_ADDR>
void DWARF_Location<MEMORY_ADDR>::SetRanges(const std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >& _ranges)
{
	ranges = _ranges;
}


template <class MEMORY_ADDR>
uint64_t DWARF_Location<MEMORY_ADDR>::GetByteSize() const
{
	return dw_byte_size;
}

template <class MEMORY_ADDR>
int64_t DWARF_Location<MEMORY_ADDR>::GetBitOffset() const
{
	return dw_bit_offset;
}

template <class MEMORY_ADDR>
uint64_t DWARF_Location<MEMORY_ADDR>::GetBitSize() const
{
	return dw_bit_size;
}

template <class MEMORY_ADDR>
uint8_t DWARF_Location<MEMORY_ADDR>::GetEncoding() const
{
	return dw_encoding;
}

template <class MEMORY_ADDR>
const std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >& DWARF_Location<MEMORY_ADDR>::GetRanges() const
{
	return ranges;
}

template <class MEMORY_ADDR>
std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >& DWARF_Location<MEMORY_ADDR>::GetRanges()
{
	return ranges;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Location<MEMORY_ADDR>& dw_loc)
{
	switch(dw_loc.GetType())
	{
		case DW_LOC_SIMPLE_MEMORY:
			{
				MEMORY_ADDR dw_addr = dw_loc.GetAddress();
				uint64_t dw_byte_size = dw_loc.GetByteSize();
				int64_t dw_bit_offset = dw_loc.GetBitOffset();
				uint64_t dw_bit_size = dw_loc.GetBitSize();
				os << "DW_LOC_SIMPLE_MEMORY: addr=0x" << std::hex << dw_addr << std::dec << ", bit_offset=" << dw_bit_offset << ", bit_size=" << dw_bit_size << ", byte_size=" << dw_byte_size << std::endl;
			}
			break;
		case DW_LOC_SIMPLE_REGISTER:
			{
				int64_t dw_bit_offset = dw_loc.GetBitOffset();
				uint64_t dw_bit_size = dw_loc.GetBitSize();

				unsigned int dw_reg_num = dw_loc.GetRegisterNumber();
				os << "DW_LOC_SIMPLE_REGISTER: dw_reg_num=" << dw_reg_num << ", dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
			}
			break;
		case DW_LOC_COMPOSITE:
			{
				const std::vector<DWARF_LocationPiece<MEMORY_ADDR> *>& dw_loc_pieces = dw_loc.GetLocationPieces();

				typename std::vector<DWARF_LocationPiece<MEMORY_ADDR> *>::const_iterator dw_loc_piece_iter;
				for(dw_loc_piece_iter = dw_loc_pieces.begin(); dw_loc_piece_iter != dw_loc_pieces.end(); dw_loc_piece_iter++)
				{
					DWARF_LocationPiece<MEMORY_ADDR> *dw_loc_piece = *dw_loc_piece_iter;
					
					switch(dw_loc_piece->GetType())
					{
						case DW_LOC_PIECE_REGISTER:
							{
								DWARF_RegisterLocationPiece<MEMORY_ADDR> *dw_reg_loc_piece = (DWARF_RegisterLocationPiece<MEMORY_ADDR> *) dw_loc_piece;
								
								unsigned int dw_reg_num = dw_reg_loc_piece->GetRegisterNumber();
								unsigned int dw_bit_offset = dw_reg_loc_piece->GetBitOffset();
								unsigned int dw_bit_size = dw_reg_loc_piece->GetBitSize();
								os << "DW_LOC_PIECE_REGISTER: dw_reg_num=" << dw_reg_num << ", dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
							}
							break;
						case DW_LOC_PIECE_MEMORY:
							{
								DWARF_MemoryLocationPiece<MEMORY_ADDR> *dw_mem_loc_piece = (DWARF_MemoryLocationPiece<MEMORY_ADDR> *) dw_loc_piece;
								
								MEMORY_ADDR dw_addr = dw_mem_loc_piece->GetAddress();
								unsigned int dw_bit_offset = dw_mem_loc_piece->GetBitOffset();
								unsigned int dw_bit_size = dw_mem_loc_piece->GetBitSize();
								os << "DW_LOC_PIECE_MEMORY: dw_addr=0x" << std::hex << dw_addr << std::dec << ", dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
							}
							break;
					}
					
				}
			}
			break;
			
		case DW_LOC_IMPLICIT_SIMPLE_VALUE:
			{
				int64_t dw_bit_offset = dw_loc.GetBitOffset();
				uint64_t dw_bit_size = dw_loc.GetBitSize();

				MEMORY_ADDR dw_implicit_simple_value = dw_loc.GetImplicitSimpleValue();
				os << "DW_LOC_IMPLICIT_SIMPLE_VALUE: dw_implicit_simple_value=" << dw_implicit_simple_value << ", dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
				break;
			}
			break;
		case DW_LOC_IMPLICIT_BLOCK_VALUE:
			{
				const DWARF_Block<MEMORY_ADDR> *dw_implicit_block_value = dw_loc.GetImplicitBlockValue();
				
				int64_t dw_bit_offset = dw_loc.GetBitOffset();
				
				// compute min(die bit size, block bit length)
				uint64_t dw_bit_size = dw_loc.GetBitSize();
				uint64_t dw_block_bit_size = 8 * dw_implicit_block_value->GetLength();
				if(dw_bit_size > dw_block_bit_size) dw_bit_size = dw_block_bit_size;
				
				os << "DW_LOC_IMPLICIT_BLOCK_VALUE: dw_implicit_block_value=" << dw_implicit_block_value->GetValue() << ", dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
			}
			break;
		default:
			os << "?" << std::endl;
			break;
	}
	return os;
}

template <class MEMORY_ADDR>
DWARF_ExpressionVM<MEMORY_ADDR>::DWARF_ExpressionVM(const DWARF_Handler<MEMORY_ADDR> *_dw_handler, unsigned int _prc_num)
	: dw_handler(_dw_handler)
	, prc_num(_prc_num)
	, reg_num_mapping(_dw_handler->GetRegisterNumberMapping(prc_num))
	, mem_if(_dw_handler->GetMemoryInterface(prc_num))
	, dw_frame(0)
	, file_endianness(_dw_handler->GetFileEndianness())
	, arch_endianness(_dw_handler->GetArchEndianness())
	, file_address_size(_dw_handler->GetFileAddressSize())
	, arch_address_size(_dw_handler->GetArchAddressSize())
	, frame_base(0)
	, has_frame_base(false)
	, object_addr(0)
	, has_object_addr(false)
	, pc(0)
	, has_pc(false)
	, debug(false)
	, debug_info_stream(_dw_handler->GetDebugInfoStream())
	, debug_warning_stream(_dw_handler->GetDebugWarningStream())
	, debug_error_stream(_dw_handler->GetDebugErrorStream())
{
	dw_handler->GetOption(OPT_DEBUG, debug);
}

template <class MEMORY_ADDR>
DWARF_ExpressionVM<MEMORY_ADDR>::DWARF_ExpressionVM(const DWARF_Handler<MEMORY_ADDR> *_dw_handler, DWARF_Frame<MEMORY_ADDR> *_dw_frame)
	: dw_handler(_dw_handler)
	, prc_num(_dw_frame->GetProcessorNumber())
	, reg_num_mapping(_dw_handler->GetRegisterNumberMapping(prc_num))
	, mem_if(_dw_handler->GetMemoryInterface(prc_num))
	, dw_frame(_dw_frame)
	, file_endianness(_dw_handler->GetFileEndianness())
	, arch_endianness(_dw_handler->GetArchEndianness())
	, file_address_size(_dw_handler->GetFileAddressSize())
	, arch_address_size(_dw_handler->GetArchAddressSize())
	, frame_base(0)
	, has_frame_base(false)
	, object_addr(0)
	, has_object_addr(false)
	, pc(0)
	, has_pc(false)
	, debug(false)
	, debug_info_stream(_dw_handler->GetDebugInfoStream())
	, debug_warning_stream(_dw_handler->GetDebugWarningStream())
	, debug_error_stream(_dw_handler->GetDebugErrorStream())
{
	dw_handler->GetOption(OPT_DEBUG, debug);
}


template <class MEMORY_ADDR>
DWARF_ExpressionVM<MEMORY_ADDR>::~DWARF_ExpressionVM()
{
}

template <class MEMORY_ADDR>
bool DWARF_ExpressionVM<MEMORY_ADDR>::Disasm(std::ostream& os, const DWARF_Expression<MEMORY_ADDR> *dw_expr)
{
	return Run(dw_expr, &os, 0, 0);
}

template <class MEMORY_ADDR>
bool DWARF_ExpressionVM<MEMORY_ADDR>::Execute(const DWARF_Expression<MEMORY_ADDR> *dw_expr, MEMORY_ADDR& result_addr, DWARF_Location<MEMORY_ADDR> *dw_location)
{
	return Run(dw_expr, 0, &result_addr, dw_location);
}

template <class MEMORY_ADDR>
bool DWARF_ExpressionVM<MEMORY_ADDR>::Run(const DWARF_Expression<MEMORY_ADDR> *dw_expr, std::ostream *os, MEMORY_ADDR *result_addr, DWARF_Location<MEMORY_ADDR> *dw_location)
{
	if(dw_location) dw_location->Clear();
	
	bool executing = (result_addr != 0) || (dw_location != 0);
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = dw_expr->GetCompilationUnit();
	const DWARF_CallFrameProgram<MEMORY_ADDR> *dw_cfp = dw_expr->GetCallFrameProgram();
	
	uint8_t address_size = dw_cu ? dw_cu->GetAddressSize() : dw_cfp->GetHandler()->GetFileAddressSize();
	endian_type file_endianness = dw_cu ? dw_cu->GetHandler()->GetFileEndianness() : dw_cfp->GetHandler()->GetFileEndianness();
//	uint8_t offset_size = dw_cu ? dw_cu->GetOffsetSize() : 0; // offsets are unused for expressions in a call frame program
	
	uint64_t expr_length = dw_expr->GetLength();
	const uint8_t *expr = dw_expr->GetValue();
	
	uint8_t opcode;
	
	bool in_dw_op_reg = false;
	bool in_dw_op_stack_value = false;
	bool got_unknown_opcode = false;
	bool got_unsupported_vendor_specific_extension = false;
	
	if(expr_length)
	{
		uint64_t expr_pos = 0;
		
		do
		{
			opcode = expr[expr_pos];
			expr_pos++;
			
			if(got_unknown_opcode || got_unsupported_vendor_specific_extension)
			{
				if(os)
				{
					*os << "0x" << std::hex << (unsigned int) opcode << std::dec;
				}
			}
			else
			{
				switch(opcode)
				{
					case DW_OP_lit0:
					case DW_OP_lit1:
					case DW_OP_lit2:
					case DW_OP_lit3:
					case DW_OP_lit4:
					case DW_OP_lit5:
					case DW_OP_lit6:
					case DW_OP_lit7:
					case DW_OP_lit8:
					case DW_OP_lit9:
					case DW_OP_lit10:
					case DW_OP_lit11:
					case DW_OP_lit12:
					case DW_OP_lit13:
					case DW_OP_lit14:
					case DW_OP_lit15:
					case DW_OP_lit16:
					case DW_OP_lit17:
					case DW_OP_lit18:
					case DW_OP_lit19:
					case DW_OP_lit20:
					case DW_OP_lit21:
					case DW_OP_lit22:
					case DW_OP_lit23:
					case DW_OP_lit24:
					case DW_OP_lit25:
					case DW_OP_lit26:
					case DW_OP_lit27:
					case DW_OP_lit28:
					case DW_OP_lit29:
					case DW_OP_lit30:
					case DW_OP_lit31:
						{
							MEMORY_ADDR literal_value = (opcode - DW_OP_lit0);
							if(os) *os << "DW_OP_lit" << literal_value;
							if(executing)
							{
								dw_stack.push_back(literal_value);
							}
						}
						break;
					case DW_OP_addr:
						{
							MEMORY_ADDR addr;
							if((expr_pos + address_size) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_addr: missing " << address_size << "-byte address operand" << std::endl;
								return false;
							}
							switch(address_size)
							{
								case 4:
									{
										uint32_t addr32;
										memcpy(&addr32, expr + expr_pos, file_address_size);
										addr32 = Target2Host(file_endianness, addr32);
										addr = addr32;
									}
									break;
								case 8:
									{
										uint64_t addr64;
										memcpy(&addr64, expr + expr_pos, file_address_size);
										addr64 = Target2Host(file_endianness, addr64);
										addr = addr64;
									}
									break;
								default:
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_addr: unsupported address operand size (" << address_size << " bytes)" << std::endl;
									return false;
							}
							expr_pos += address_size;
							if(os) *os << "DW_OP_addr 0x" << std::hex << addr << std::dec;
							if(executing) dw_stack.push_back(addr);
						}
						break;
					case DW_OP_const1u:
						{
							uint8_t dw_const;
							if((expr_pos + sizeof(dw_const)) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_const1u: missing 1-byte unsigned constant operand" << std::endl;
								return false;
							}
							memcpy(&dw_const, expr + expr_pos, sizeof(dw_const));
							dw_const = Target2Host(file_endianness, dw_const);
							expr_pos += sizeof(dw_const);
							if(os) *os << "DW_OP_const1u " << (uint32_t) dw_const;
							if(executing)
							{
								MEMORY_ADDR value = dw_const;
								dw_stack.push_back(value);
							}
						}
						break;
					case DW_OP_const1s:
						{
							uint8_t dw_const;
							if((expr_pos + sizeof(dw_const)) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_const1s: missing 1-byte signed constant operand" << std::endl;
								return false;
							}
							memcpy(&dw_const, expr + expr_pos, sizeof(dw_const));
							dw_const = Target2Host(file_endianness, dw_const);
							expr_pos += sizeof(dw_const);
							if(os) *os << "DW_OP_const1s " << (int64_t)(int8_t) dw_const;
							if(executing)
							{
								MEMORY_ADDR value = SignExtend((MEMORY_ADDR) dw_const, 8 * sizeof(dw_const));
								dw_stack.push_back(value);
							}
						}
						break;
					case DW_OP_const2u:
						{
							uint16_t dw_const;
							if((expr_pos + sizeof(dw_const)) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_const2u: missing 2-byte unsigned constant operand" << std::endl;
								return false;
							}
							memcpy(&dw_const, expr + expr_pos, sizeof(dw_const));
							dw_const = Target2Host(file_endianness, dw_const);
							expr_pos += sizeof(dw_const);
							if(os) *os << "DW_OP_const2u " << (uint32_t) dw_const;
							if(executing)
							{
								MEMORY_ADDR value = dw_const;
								dw_stack.push_back(value);
							}
						}
						break;
					case DW_OP_const2s:
						{
							uint16_t dw_const;
							if((expr_pos + sizeof(dw_const)) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_const2s: missing 2-byte signed constant operand" << std::endl;
								return false;
							}
							memcpy(&dw_const, expr + expr_pos, sizeof(dw_const));
							dw_const = Target2Host(file_endianness, dw_const);
							expr_pos += sizeof(dw_const);
							if(os) *os << "DW_OP_const2s " << (int64_t)(int16_t) dw_const;
							if(executing)
							{
								MEMORY_ADDR value = SignExtend((MEMORY_ADDR) dw_const, 8 * sizeof(dw_const));
								dw_stack.push_back(value);
							}
						}
						break;
					case DW_OP_const4u:
						{
							uint32_t dw_const;
							if((expr_pos + sizeof(dw_const)) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_const4u: missing 4-byte unsigned constant operand" << std::endl;
								return false;
							}
							memcpy(&dw_const, expr + expr_pos, sizeof(dw_const));
							dw_const = Target2Host(file_endianness, dw_const);
							expr_pos += sizeof(dw_const);
							if(os) *os << "DW_OP_const4u " << dw_const;
							if(executing)
							{
								MEMORY_ADDR value = dw_const;
								dw_stack.push_back(value);
							}
						}
						break;
					case DW_OP_const4s:
						{
							uint32_t dw_const;
							if((expr_pos + sizeof(dw_const)) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_const4s: missing 4-byte signed constant operand" << std::endl;
								return false;
							}
							memcpy(&dw_const, expr + expr_pos, sizeof(dw_const));
							dw_const = Target2Host(file_endianness, dw_const);
							expr_pos += sizeof(dw_const);
							if(os) *os << "DW_OP_const4s " << (int64_t)(int32_t) dw_const;
							if(executing)
							{
								MEMORY_ADDR value = SignExtend((MEMORY_ADDR) dw_const, 8 * sizeof(dw_const));
								dw_stack.push_back(value);
							}
						}
						break;
					case DW_OP_const8u:
						{
							uint64_t dw_const;
							if((expr_pos + sizeof(dw_const)) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_const8u: missing 8-byte unsigned constant operand" << std::endl;
								return false;
							}
							memcpy(&dw_const, expr + expr_pos, sizeof(dw_const));
							dw_const = Target2Host(file_endianness, dw_const);
							expr_pos += sizeof(dw_const);
							if(os) *os << "DW_OP_const8u " << dw_const;
							if(executing)
							{
								MEMORY_ADDR value = dw_const;
								dw_stack.push_back(value);
							}
						}
						break;
					case DW_OP_const8s:
						{
							uint64_t dw_const;
							if((expr_pos + sizeof(dw_const)) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_const8s: missing 8-byte signed constant operand" << std::endl;
								return false;
							}
							memcpy(&dw_const, expr + expr_pos, sizeof(dw_const));
							dw_const = Target2Host(file_endianness, dw_const);
							expr_pos += sizeof(dw_const);
							if(os) *os << "DW_OP_const8s " << (int64_t) dw_const;
							if(executing)
							{
								MEMORY_ADDR value = SignExtend((MEMORY_ADDR) dw_const, 8 * sizeof(dw_const));
								dw_stack.push_back(value);
							}
						}
						break;
					case DW_OP_constu:
						{
							DWARF_LEB128 dw_const_leb128;
							int64_t sz;
							if((sz = dw_const_leb128.Load(expr + expr_pos, expr_length - expr_pos)) < 0)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_constu: missing LEB128 unsigned constant operand" << std::endl;
								return false;
							}
							expr_pos += sz;
							
							uint64_t value = (uint64_t) dw_const_leb128;
							if(os) *os << "DW_OP_constu " << value;
							if(executing)
							{
								MEMORY_ADDR addr = value;
								dw_stack.push_back(addr);
							}
						}
						break;
					case DW_OP_consts:
						{
							DWARF_LEB128 dw_const_leb128;
							int64_t sz;
							if((sz = dw_const_leb128.Load(expr + expr_pos, expr_length - expr_pos)) < 0)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_consts: missing LEB128 signed constant operand" << std::endl;
								return false;
							}
							expr_pos += sz;
							
							int64_t value = (int64_t) dw_const_leb128;
							if(os) *os << "DW_OP_consts " << value;
							if(executing)
							{
								MEMORY_ADDR addr = value;
								dw_stack.push_back(addr);
							}
						}
						break;
					case DW_OP_fbreg:
						{
							DWARF_LEB128 dw_offset_leb128;
							int64_t sz;
							if((sz = dw_offset_leb128.Load(expr + expr_pos, expr_length - expr_pos)) < 0)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_fbreg: missing LEB128 signed offset operand" << std::endl;
								return false;
							}
							expr_pos += sz;
							
							int64_t offset = (int64_t) dw_offset_leb128;
							if(os) *os << "DW_OP_fbreg " << offset;
							if(executing)
							{
								// push onto the stack (frame base + offset)
								if(!has_frame_base)
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_fbreg " << offset << ": frame base address is not set" << std::endl;
									return false;
								}
								dw_stack.push_back(frame_base + offset);
							}
						}
						break;
					case DW_OP_breg0:
					case DW_OP_breg1:
					case DW_OP_breg2:
					case DW_OP_breg3:
					case DW_OP_breg4:
					case DW_OP_breg5:
					case DW_OP_breg6:
					case DW_OP_breg7:
					case DW_OP_breg8:
					case DW_OP_breg9:
					case DW_OP_breg10:
					case DW_OP_breg11:
					case DW_OP_breg12:
					case DW_OP_breg13:
					case DW_OP_breg14:
					case DW_OP_breg15:
					case DW_OP_breg16:
					case DW_OP_breg17:
					case DW_OP_breg18:
					case DW_OP_breg19:
					case DW_OP_breg20:
					case DW_OP_breg21:
					case DW_OP_breg22:
					case DW_OP_breg23:
					case DW_OP_breg24:
					case DW_OP_breg25:
					case DW_OP_breg26:
					case DW_OP_breg27:
					case DW_OP_breg28:
					case DW_OP_breg29:
					case DW_OP_breg30:
					case DW_OP_breg31:
						{
							uint8_t dw_reg_num = opcode - DW_OP_breg0;
							DWARF_LEB128 dw_offset_leb128;
							int64_t sz;
							if((sz = dw_offset_leb128.Load(expr + expr_pos, expr_length - expr_pos)) < 0)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_breg" << (unsigned int) dw_reg_num << ": missing LEB128 signed offset operand" << std::endl;
								return false;
							}
							expr_pos += sz;
							
							int64_t offset = (int64_t) dw_offset_leb128;
							if(os) *os << "DW_OP_breg" << (unsigned int) dw_reg_num << " " << offset;
							if(executing)
							{
								MEMORY_ADDR reg_value = 0;
								if(!ReadRegister(dw_reg_num, reg_value)) return false;
								// push onto the stack reg value + offset
								dw_stack.push_back(reg_value + offset);
							}
						}
						break;
					case DW_OP_bregx:
						{
							DWARF_LEB128 dw_reg_num_leb128;
							int64_t sz;
							if((sz = dw_reg_num_leb128.Load(expr + expr_pos, expr_length - expr_pos)) < 0)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_bregx: missing LEB128 register number operand" << std::endl;
								return false;
							}
							expr_pos += sz;
							
							uint64_t dw_reg_num = (uint64_t) dw_reg_num_leb128;

							DWARF_LEB128 dw_offset_leb128;
							if((sz = dw_offset_leb128.Load(expr + expr_pos, expr_length - expr_pos)) < 0)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_bregx: missing LEB128 signed offset operand" << std::endl;
								return false;
							}
							expr_pos += sz;
							
							int64_t offset = (int64_t) dw_offset_leb128;
							
							if(os) *os << "DW_OP_bregx " << dw_reg_num;
							if(executing)
							{
								MEMORY_ADDR reg_value = 0;
								if(!ReadRegister(dw_reg_num, reg_value)) return false;
								// push onto the stack reg value + offset
								dw_stack.push_back(reg_value + offset);
							}
						}
						break;
					case DW_OP_dup:
						if(os) *os << "DW_OP_dup";
						if(executing)
						{
							if(dw_stack.empty())
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_dup: DWARF stack is empty" << std::endl;
								return false;
							}
							MEMORY_ADDR addr = dw_stack.back();
							dw_stack.push_back(addr);
						}
						break;
					case DW_OP_drop:
						if(os) *os << "DW_OP_drop";
						if(executing)
						{
							if(dw_stack.empty())
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_drop: DWARF stack is empty" << std::endl;
								return false;
							}
							dw_stack.pop_back();
						}
						break;
					case DW_OP_pick:
						{
							uint8_t dw_index;
							if((expr_pos + sizeof(dw_index)) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_pick: missing 1-byte unsigned index operand" << std::endl;
								return false;
							}
							memcpy(&dw_index, expr + expr_pos, sizeof(dw_index));
							dw_index = Target2Host(file_endianness, dw_index);
							expr_pos += sizeof(dw_index);
							if(os) *os << "DW_OP_pick " << (unsigned int) dw_index;
							if(executing)
							{
								unsigned int dw_stack_size = dw_stack.size();
								if(dw_index >= dw_stack_size)
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_pick: DWARF stack index (" << (unsigned int)  dw_index << ") is out of range (must be < " << dw_stack_size << ")" << std::endl;
									return false;
								}
								MEMORY_ADDR pick_value = dw_stack[dw_stack_size - 1 - dw_index];
								dw_stack.push_back(pick_value);
							}
						}
						break;
					case DW_OP_over:
						if(os) *os << "DW_OP_over";
						if(executing)
						{
							unsigned int dw_index = 1; // same as DW_OP_pick 1
							unsigned int dw_stack_size = dw_stack.size();
							if(dw_index >= dw_stack_size)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_over: DWARF stack (" << dw_stack_size << " elements) is too small for operation" << std::endl;
								return false;
							}
							MEMORY_ADDR pick_value = dw_stack[dw_stack_size - 1 - dw_index];
							dw_stack.push_back(pick_value);
						}
						break;
					case DW_OP_swap:
						if(os) *os << "DW_OP_swap";
						if(executing)
						{
							unsigned int dw_stack_size = dw_stack.size();
							if(dw_stack_size < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_swap: DWARF stack (" << dw_stack_size << " elements) is too small for operation" << std::endl;
								return false;
							}
							MEMORY_ADDR second_value = dw_stack[dw_stack_size - 2];
							MEMORY_ADDR first_value = dw_stack[dw_stack_size - 1];
							// swap the top two stack entries
							dw_stack[dw_stack_size - 2] = first_value;
							dw_stack[dw_stack_size - 1] = second_value;
						}
						break;
					case DW_OP_rot:
						if(os) *os << "DW_OP_rot";
						if(executing)
						{
							unsigned int dw_stack_size = dw_stack.size();
							if(dw_stack_size < 3)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_rot: DWARF stack (" << dw_stack_size << " elements) is too small for operation" << std::endl;
								return false;
							}
							MEMORY_ADDR third_value = dw_stack[dw_stack_size - 3];
							MEMORY_ADDR second_value = dw_stack[dw_stack_size - 2];
							MEMORY_ADDR first_value = dw_stack[dw_stack_size - 1];
							// rotates the first three stack entries
							dw_stack[dw_stack_size - 3] = second_value;
							dw_stack[dw_stack_size - 2] = first_value;
							dw_stack[dw_stack_size - 1] = third_value;
						}
						break;
					case DW_OP_deref:
						if(os) *os << "DW_OP_deref";
						if(executing)
						{
							if(dw_stack.empty())
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_deref: DWARF stack is empty" << std::endl;
								return false;
							}
							MEMORY_ADDR addr = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR read_addr = 0;
							
							if(!ReadAddrFromMemory(addr, read_addr))
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_deref: failed reading at memory address 0x" << std::hex << addr << std::dec << std::endl;
								return false;
							}
							dw_stack.push_back(read_addr);
						}
						break;
					case DW_OP_deref_size:
						{
							uint8_t dw_size;
							if((expr_pos + sizeof(dw_size)) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_deref_size: missing 1-byte size operand" << std::endl;
								return false;
							}
							memcpy(&dw_size, expr + expr_pos, sizeof(dw_size));
							dw_size = Target2Host(file_endianness, dw_size);
							expr_pos += sizeof(dw_size);
							if(os) *os << "DW_OP_deref_size " << (uint32_t) dw_size;
							if(executing)
							{
								if(dw_stack.empty())
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_deref_size: DWARF stack is empty" << std::endl;
									return false;
								}
								MEMORY_ADDR addr = dw_stack.back();
								dw_stack.pop_back();
								MEMORY_ADDR read_addr = 0;
								
								if(!ReadAddrFromMemory(addr, read_addr, dw_size))
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_deref_size: failed reading at memory address 0x" << std::hex << addr << std::dec << std::endl;
									return false;
								}
								dw_stack.push_back(read_addr);
							}
						}
						break;
					case DW_OP_xderef:
						if(os) *os << "DW_OP_xderef";
						if(executing)
						{
							unsigned int dw_stack_size = dw_stack.size();
							if(dw_stack_size < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_xderef: DWARF stack (" << dw_stack_size << " elements) is too small for operation" << std::endl;
								return false;
							}
							MEMORY_ADDR addr = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR addr_space = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR read_addr = 0;
							
							if(!ReadAddrFromMemory(addr, read_addr, 0, addr_space))
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_xderef: failed reading at memory address 0x" << std::hex << addr << std::dec << std::endl;
								return false;
							}
							dw_stack.push_back(read_addr);
						}
						break;
					case DW_OP_xderef_size:
						{
							uint8_t dw_size;
							if((expr_pos + sizeof(dw_size)) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_xderef_size: missing 1-byte size operand" << std::endl;
								return false;
							}
							memcpy(&dw_size, expr + expr_pos, sizeof(dw_size));
							dw_size = Target2Host(file_endianness, dw_size);
							expr_pos += sizeof(dw_size);
							if(os) *os << "DW_OP_xderef_size " << (uint32_t) dw_size;
							if(executing)
							{
								unsigned int dw_stack_size = dw_stack.size();
								if(dw_stack_size < 2)
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_xderef_size: DWARF stack (" << dw_stack_size << " elements) is too small for operation" << std::endl;
									return false;
								}
								MEMORY_ADDR addr = dw_stack.back();
								dw_stack.pop_back();
								MEMORY_ADDR addr_space = dw_stack.back();
								dw_stack.pop_back();
								MEMORY_ADDR read_addr = 0;
								
								if(!ReadAddrFromMemory(addr, read_addr, dw_size, addr_space))
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_xderef_size: failed reading at memory address 0x" << std::hex << addr << std::dec << std::endl;
									return false;
								}
								dw_stack.push_back(read_addr);
							}
						}
						break;
					case DW_OP_push_object_address:
						if(os) *os << "DW_OP_push_object_address";
						if(executing)
						{
							// DW_OP_push_object_address is not meaningful in an operand of these instructions because
							// there is no object context to provide a value to push.
							if(dw_cfp)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_push_object_address: not meaningful in a call frame program" << std::endl;
								return false;
							}

							if(!dw_cu)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_push_object_address: only meaningful in a DIE" << std::endl;
								return false;
							}
							
							// push object address
							if(!has_object_addr)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_push_object_address: object address is not set" << std::endl;
								return false;
							}
							dw_stack.push_back(object_addr);
						}
						break;
					case DW_OP_form_tls_address:
						if(os) *os << "DW_OP_form_tls_address";
						if(executing)
						{
							// Currently unimplemented.
							debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_form_tls_address: currently unimplemented" << std::endl;
							return false;
						}
						break;
					case DW_OP_call_frame_cfa:
						if(os) *os << "DW_OP_call_frame_cfa";
						if(executing)
						{
							// DW_OP_call_frame_cfa is not meaningful in an operand of these instructions because its use
							// would be circular.
							if(dw_cfp)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call_frame_cfa: not meaningful in a call frame program" << std::endl;
								return false;
							}
							
							if(!has_pc)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call_frame_cfa: don't know PC" << std::endl;
								return false;
							}
							
							MEMORY_ADDR cfa = 0;
							if(!dw_handler->ComputeCFA(prc_num, pc, cfa))
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call_frame_cfa: computing of CFA failed" << std::endl;
								return false;
							}
							// Push CFA
							dw_stack.push_back(cfa);
							break;
						}
						break;
					case DW_OP_abs:
						if(os) *os << "DW_OP_abs";
						if(executing)
						{
							if(dw_stack.empty())
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_abs: DWARF stack is empty" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							if(op1 & (1ULL << ((8 * sizeof(MEMORY_ADDR)) - 1))) // is it negative ?
							{
								dw_stack.back() = -op1;
							}
						}
						break;
					case DW_OP_and:
						if(os) *os << "DW_OP_and";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_and: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR op2 = dw_stack.back();
							dw_stack.pop_back();
							dw_stack.push_back(op1 & op2);
						}
						break;
					case DW_OP_div:
						if(os) *os << "DW_OP_div";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_div: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							int64_t op1 = dw_stack.back();
							dw_stack.pop_back();
							int64_t op2 = dw_stack.back();
							dw_stack.pop_back();
							if(!op1)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_div: division by zero" << std::endl;
								return false; // division by zero
							}
							dw_stack.push_back(op2 / op1);
						}
						break;
					case DW_OP_minus:
						if(os) *os << "DW_OP_minus";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_minus: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR op2 = dw_stack.back();
							dw_stack.pop_back();
							dw_stack.push_back(op2 - op1);
						}
						break;
					case DW_OP_mod:
						if(os) *os << "DW_OP_mod";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_mod: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR op2 = dw_stack.back();
							dw_stack.pop_back();
							if(!op1)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_mod: division by zero" << std::endl;
								return false; // division by zero
							}
							dw_stack.push_back(op2 % op1); // Note: unsigned modulus
						}
						break;
					case DW_OP_mul:
						if(os) *os << "DW_OP_mul";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_mul: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR op2 = dw_stack.back();
							dw_stack.pop_back();
							dw_stack.push_back(op2 * op1);
						}
						break;
					case DW_OP_neg:
						if(os) *os << "DW_OP_neg";
						if(executing)
						{
							if(dw_stack.empty())
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_neg: DWARF stack is empty" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.back() = -op1;
						}
						break;
					case DW_OP_not:
						if(os) *os << "DW_OP_not";
						if(executing)
						{
							if(dw_stack.empty())
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_not: DWARF stack is empty" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.back() = ~op1;
						}
						break;
					case DW_OP_or:
						if(os) *os << "DW_OP_or";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_or: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR op2 = dw_stack.back();
							dw_stack.pop_back();
							dw_stack.push_back(op2 | op1);
						}
						break;
					case DW_OP_plus:
						if(os) *os << "DW_OP_plus";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_plus: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR op2 = dw_stack.back();
							dw_stack.pop_back();
							dw_stack.push_back(op2 + op1);
						}
						break;
					case DW_OP_plus_uconst:
						{
							DWARF_LEB128 dw_uconst_leb128;
							int64_t sz;
							if((sz = dw_uconst_leb128.Load(expr + expr_pos, expr_length - expr_pos)) < 0)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_plus_uconst: missing LEB128 unsigned constant operand" << std::endl;
								return false;
							}
							expr_pos += sz;
							
							MEMORY_ADDR dw_uconst = (MEMORY_ADDR) dw_uconst_leb128;
							if(os) *os << "DW_OP_plus_uconst " << dw_uconst;
							if(executing)
							{
								if(dw_stack.empty())
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_plus_uconst: DWARF stack is empty" << std::endl;
									return false;
								}
								MEMORY_ADDR op1 = dw_stack.back();
								dw_stack.pop_back();
								dw_stack.push_back(dw_uconst + op1);
							}
						}
						break;
					case DW_OP_shl:
						if(os) *os << "DW_OP_shl";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_shl: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR op2 = dw_stack.back();
							dw_stack.pop_back();
							dw_stack.push_back(op2 << op1);
						}
						break;
					case DW_OP_shr:
						if(os) *os << "DW_OP_shr";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_shr: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR op2 = dw_stack.back();
							dw_stack.pop_back();
							dw_stack.push_back(op2 >> op1);
						}
						break;
					case DW_OP_shra:
						if(os) *os << "DW_OP_shra";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_shra: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR op2 = dw_stack.back();
							dw_stack.pop_back();
							dw_stack.push_back((signed) op2 >> op1);
						}
						break;
					case DW_OP_xor:
						if(os) *os << "DW_OP_xor";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_xor: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR op2 = dw_stack.back();
							dw_stack.pop_back();
							dw_stack.push_back(op2 ^ op1);
						}
						break;
					case DW_OP_le:
						if(os) *os << "DW_OP_le";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_le: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR op2 = dw_stack.back();
							dw_stack.pop_back();
							dw_stack.push_back(((signed) op2 <= (signed) op1) ? 1 : 0);
						}
						break;
					case DW_OP_ge:
						if(os) *os << "DW_OP_ge";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_ge: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR op2 = dw_stack.back();
							dw_stack.pop_back();
							dw_stack.push_back(((signed) op2 >= (signed) op1) ? 1 : 0);
						}
						break;
					case DW_OP_eq:
						if(os) *os << "DW_OP_eq";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_eq: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR op2 = dw_stack.back();
							dw_stack.pop_back();
							dw_stack.push_back((op2 == op1) ? 1 : 0);
						}
						break;
					case DW_OP_lt:
						if(os) *os << "DW_OP_lt";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_lt: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR op2 = dw_stack.back();
							dw_stack.pop_back();
							dw_stack.push_back(((signed) op2 < (signed) op1) ? 1 : 0);
						}
						break;
					case DW_OP_gt:
						if(os) *os << "DW_OP_gt";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_gt: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR op2 = dw_stack.back();
							dw_stack.pop_back();
							dw_stack.push_back(((signed) op2 > (signed) op1) ? 1 : 0);
						}
						break;
					case DW_OP_ne:
						if(os) *os << "DW_OP_ne";
						if(executing)
						{
							if(dw_stack.size() < 2)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_ne: DWARF stack (" << dw_stack.size() << " elements) is too small for binary operation" << std::endl;
								return false;
							}
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR op2 = dw_stack.back();
							dw_stack.pop_back();
							dw_stack.push_back((op2 != op1) ? 1 : 0);
						}
						break;
					case DW_OP_skip:
						{
							int16_t dw_skip_amount;
							if((expr_pos + sizeof(dw_skip_amount)) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_skip: missing 2-byte signed constant operand" << std::endl;
								return false;
							}
							memcpy(&dw_skip_amount, expr + expr_pos, sizeof(dw_skip_amount));
							dw_skip_amount = Target2Host(file_endianness, dw_skip_amount);
							expr_pos += sizeof(dw_skip_amount);
							if(os) *os << "DW_OP_skip " << dw_skip_amount;
							if(executing)
							{
								if(dw_skip_amount > 0)
								{
									if(expr_pos + dw_skip_amount > expr_length)
									{
										debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_skip: forward skip amount is out of range" << std::endl;
										return false;
									}
								}
								else
								{
									if(expr_pos < (uint64_t) -dw_skip_amount)
									{
										debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_skip: backward skip amount is out of range" << std::endl;
										return false;
									}
								}
								
								expr_pos += dw_skip_amount;
							}
						}
						break;
					case DW_OP_bra:
						{
							int16_t dw_skip_amount;
							if((expr_pos + sizeof(dw_skip_amount)) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_bra: missing 2-byte signed constant operand" << std::endl;
								return false;
							}
							memcpy(&dw_skip_amount, expr + expr_pos, sizeof(dw_skip_amount));
							dw_skip_amount = Target2Host(file_endianness, dw_skip_amount);
							expr_pos += sizeof(dw_skip_amount);
							if(os) *os << "DW_OP_bra " << dw_skip_amount;
							if(executing)
							{
								if(dw_stack.empty())
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_bra: DWARF stack is empty" << std::endl;
									return false;
								}
								MEMORY_ADDR cond = dw_stack.back();
								dw_stack.pop_back();
								if(cond)
								{
									if(dw_skip_amount > 0)
									{
										if(expr_pos + dw_skip_amount > expr_length)
										{
											debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_bra: forward skip amount is out of range" << std::endl;
											return false;
										}
									}
									else
									{
										if(expr_pos < (uint64_t) -dw_skip_amount)
										{
											debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_bra: backward skip amount is out of range" << std::endl;
											return false;
										}
									}
									expr_pos += dw_skip_amount;
								}
							}
						}
						break;
					case DW_OP_call2:
						{
							// DW_OP_call2, DW_OP_call4 and DW_OP_call_ref operators are not meaningful in an
							// operand of these instructions because there is no mapping from call frame information to any
							// corresponding debugging compilation unit information, thus there is no way to interpret the call
							// offset.
							if(dw_cfp)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call2: not meaningful in a call frame program" << std::endl;
								return false;
							}
							
							uint16_t debug_info_offset16;
							if((expr_pos + sizeof(debug_info_offset16)) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call2: missing 2-byte unsigned offset operand" << std::endl;
								return false;
							}
							memcpy(&debug_info_offset16, expr + expr_pos, sizeof(debug_info_offset16));
							debug_info_offset16 = Target2Host(file_endianness, debug_info_offset16);
							expr_pos += sizeof(debug_info_offset16);
							if(os) *os << "DW_OP_call2 " << debug_info_offset16;
							if(executing)
							{
								const DWARF_DIE<MEMORY_ADDR> *dw_die = dw_handler->FindDIE(debug_info_offset16);
								if(!dw_die)
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call2: can't find DIE at offset 0x" << std::hex << debug_info_offset16 << std::dec << std::endl;
									return false;
								}
								const DWARF_Attribute<MEMORY_ADDR> *dw_at_location = dw_die->FindAttribute(DW_AT_location);
								if(dw_at_location)
								{
									const DWARF_AttributeValue<MEMORY_ADDR> *dw_at_location_value = dw_at_location->GetValue();
									if(dw_at_location_value->GetClass() != DW_CLASS_EXPRESSION)
									{
										debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call2: DW_AT_location attribute of referenced DIE is not a DWARF expression" << std::endl;
										return false;
									}
									const DWARF_Expression<MEMORY_ADDR> *dw_at_location_expr = (const DWARF_Expression<MEMORY_ADDR> *) dw_at_location_value;
									MEMORY_ADDR call_result_addr;
									DWARF_Location<MEMORY_ADDR> dw_call_location;
									bool call_status = Run(dw_at_location_expr, os, &call_result_addr, &dw_call_location);
									if(!call_status)
									{
										debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call2: DWARF expression evaluation failed" << std::endl;
										return false;
									}
								}
								else
								{
									debug_warning_stream << "DW_OP_call2: operation has no effect because referenced DIE has no DW_AT_location attribute" << std::endl;
								}
							}
						}
						break;
					case DW_OP_call4:
						{
							// DW_OP_call2, DW_OP_call4 and DW_OP_call_ref operators are not meaningful in an
							// operand of these instructions because there is no mapping from call frame information to any
							// corresponding debugging compilation unit information, thus there is no way to interpret the call
							// offset.
							if(dw_cfp)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call4: not meaningful in a call frame program" << std::endl;
								return false;
							}
							
							uint32_t debug_info_offset32;
							if((expr_pos + sizeof(debug_info_offset32)) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call4: missing 4-byte unsigned offset operand" << std::endl;
								return false;
							}
							memcpy(&debug_info_offset32, expr + expr_pos, sizeof(debug_info_offset32));
							debug_info_offset32 = Target2Host(file_endianness, debug_info_offset32);
							expr_pos += sizeof(debug_info_offset32);
							if(os) *os << "DW_OP_call4 " << debug_info_offset32;
							if(executing)
							{
								const DWARF_DIE<MEMORY_ADDR> *dw_die = dw_handler->FindDIE(debug_info_offset32);
								if(!dw_die)
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call4: can't find DIE at offset 0x" << std::hex << debug_info_offset32 << std::dec << std::endl;
									return false;
								}
								const DWARF_Attribute<MEMORY_ADDR> *dw_at_location = dw_die->FindAttribute(DW_AT_location);
								if(dw_at_location)
								{
									const DWARF_AttributeValue<MEMORY_ADDR> *dw_at_location_value = dw_at_location->GetValue();
									if(dw_at_location_value->GetClass() != DW_CLASS_EXPRESSION)
									{
										debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call4: DW_AT_location attribute of referenced DIE is not a DWARF expression" << std::endl;
										return false;
									}
									const DWARF_Expression<MEMORY_ADDR> *dw_at_location_expr = (const DWARF_Expression<MEMORY_ADDR> *) dw_at_location_value;
									MEMORY_ADDR call_result_addr;
									DWARF_Location<MEMORY_ADDR> dw_call_location;
									bool call_status = Run(dw_at_location_expr, os, &call_result_addr, &dw_call_location);
									if(!call_status)
									{
										debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call4: DWARF expression evaluation failed" << std::endl;
										return false;
									}
								}
								else
								{
									debug_warning_stream << "DW_OP_call4: operation has no effect because referenced DIE has no DW_AT_location attribute" << std::endl;
								}
							}
						}
						break;
					case DW_OP_call_ref:
						{
							// DW_OP_call2, DW_OP_call4 and DW_OP_call_ref operators are not meaningful in an
							// operand of these instructions because there is no mapping from call frame information to any
							// corresponding debugging compilation unit information, thus there is no way to interpret the call
							// offset.
							if(dw_cfp)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call_ref: not meaningful in a call frame program" << std::endl;
								return false;
							}
							
							if(!dw_cu)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call_ref: only meaningful in a DIE" << std::endl;
								return false;
							}
							uint64_t debug_info_offset;
							switch(dw_cu->GetOffsetSize())
							{
								case 4:
									{
										uint32_t debug_info_offset32;
									
										if((expr_pos + sizeof(debug_info_offset32)) > expr_length)
										{
											debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call_ref: missing 4-byte unsigned offset operand" << std::endl;
											return false;
										}
										memcpy(&debug_info_offset32, expr + expr_pos, sizeof(debug_info_offset32));
										debug_info_offset32 = Target2Host(file_endianness, debug_info_offset32);
										expr_pos += sizeof(debug_info_offset32);
										debug_info_offset = debug_info_offset32;
									}
									break;
								case 8:
									{
										uint64_t debug_info_offset64;
									
										if((expr_pos + sizeof(debug_info_offset64)) > expr_length)
										{
											debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call_ref: missing 8-byte unsigned offset operand" << std::endl;
											return false;
										}
										memcpy(&debug_info_offset64, expr + expr_pos, sizeof(debug_info_offset64));
										debug_info_offset64 = Target2Host(file_endianness, debug_info_offset64);
										expr_pos += sizeof(debug_info_offset64);
										debug_info_offset = debug_info_offset64;
									}
									break;
								default:
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call_ref: unsupported unsigned offset operand size" << std::endl;
									return false;
							}
							if(os) *os << "DW_OP_call_ref " << debug_info_offset;
							if(executing)
							{
								const DWARF_DIE<MEMORY_ADDR> *dw_die = dw_handler->FindDIE(debug_info_offset);
								if(!dw_die)
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call_ref: can't find DIE at offset 0x" << std::hex << debug_info_offset << std::dec << std::endl;
									return false;
								}
								const DWARF_Attribute<MEMORY_ADDR> *dw_at_location = dw_die->FindAttribute(DW_AT_location);
								if(dw_at_location)
								{
									const DWARF_AttributeValue<MEMORY_ADDR> *dw_at_location_value = dw_at_location->GetValue();
									if(dw_at_location_value->GetClass() != DW_CLASS_EXPRESSION)
									{
										debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call_ref: DW_AT_location attribute of referenced DIE is not a DWARF expression" << std::endl;
										return false;
									}
									const DWARF_Expression<MEMORY_ADDR> *dw_at_location_expr = (const DWARF_Expression<MEMORY_ADDR> *) dw_at_location_value;
									MEMORY_ADDR call_result_addr;
									DWARF_Location<MEMORY_ADDR> dw_call_location;
									bool call_status = Run(dw_at_location_expr, os, &call_result_addr, &dw_call_location);
									if(!call_status)
									{
										debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_call_ref: DWARF expression evaluation failed" << std::endl;
										return false;
									}
								}
								else
								{
									debug_warning_stream << "DW_OP_call_ref: operation has no effect because referenced DIE has no DW_AT_location attribute" << std::endl;
								}
							}
						}
						break;
					case DW_OP_nop:
						if(os) *os << "DW_OP_nop";
						break;
					case DW_OP_reg0:
					case DW_OP_reg1:
					case DW_OP_reg2:
					case DW_OP_reg3:
					case DW_OP_reg4:
					case DW_OP_reg5:
					case DW_OP_reg6:
					case DW_OP_reg7:
					case DW_OP_reg8:
					case DW_OP_reg9:
					case DW_OP_reg10:
					case DW_OP_reg11:
					case DW_OP_reg12:
					case DW_OP_reg13:
					case DW_OP_reg14:
					case DW_OP_reg15:
					case DW_OP_reg16:
					case DW_OP_reg17:
					case DW_OP_reg18:
					case DW_OP_reg19:
					case DW_OP_reg20:
					case DW_OP_reg21:
					case DW_OP_reg22:
					case DW_OP_reg23:
					case DW_OP_reg24:
					case DW_OP_reg25:
					case DW_OP_reg26:
					case DW_OP_reg27:
					case DW_OP_reg28:
					case DW_OP_reg29:
					case DW_OP_reg30:
					case DW_OP_reg31:
						{
							uint8_t dw_reg_num = opcode - DW_OP_reg0;
							if(os) *os << "DW_OP_reg" << (unsigned int) dw_reg_num;
							if(executing)
							{
								if(!dw_location)
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_reg" << (unsigned int) dw_reg_num << ": only allowed in location expressions" << std::endl;
									return false; // DW_OP_reg* are only allowed in location expressions
								}
								
								// Each register name operator must be used alone (as a DWARF expression
								// consisting of just that one operation).
								if((expr_pos != expr_length) && (expr[expr_pos] != DW_OP_piece))
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_reg" << (unsigned int) dw_reg_num << ": each register name operator must be used alone" << std::endl;
									return false;
								}
								
								in_dw_op_reg = true; // indicate that we started a DW_OP_reg* and that the register number is at top of the stack
								dw_stack.push_back(dw_reg_num);
							}
						}
						break;
					case DW_OP_regx:
						{
							DWARF_LEB128 dw_reg_num_leb128;
							int64_t sz;
							if((sz = dw_reg_num_leb128.Load(expr + expr_pos, expr_length - expr_pos)) < 0)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_regx: missing LEB128 register number operand" << std::endl;
								return false;
							}
							expr_pos += sz;
							
							MEMORY_ADDR dw_reg_num = (MEMORY_ADDR) dw_reg_num_leb128;
							if(os) *os << "DW_OP_regx " << dw_reg_num;
							if(executing)
							{
								if(!dw_location)
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_regx " << dw_reg_num << ": only allowed in location expressions" << std::endl;
									return false; // DW_OP_reg* are only allowed in location expressions
								}

								// Each register name operator must be used alone (as a DWARF expression
								// consisting of just that one operation).
								if((expr_pos != expr_length) && (expr[expr_pos] != DW_OP_piece))
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_regx" << dw_reg_num << ": each register name operator must be used alone" << std::endl;
									return false;
								}
								
								in_dw_op_reg = true; // indicate that we started a DW_OP_reg* and that the register number is at top of the stack
								dw_stack.push_back(dw_reg_num);
							}
						}
						break;
					case DW_OP_piece:
						{
							DWARF_LEB128 dw_byte_size_leb128;
							int64_t sz;
							if((sz = dw_byte_size_leb128.Load(expr + expr_pos, expr_length - expr_pos)) < 0)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_piece: missing LEB128 byte size operand" << std::endl;
								return false;
							}
							expr_pos += sz;
							
							uint64_t dw_byte_size = (uint64_t) dw_byte_size_leb128;
							if(os) *os << "DW_OP_piece " << dw_byte_size;
							if(executing)
							{
								uint64_t dw_bit_size = dw_byte_size * 8;
								if(!dw_location)
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_piece: only allowed in location expressions" << std::endl;
									return false; // DW_OP_piece is only allowed in location expressions
								}

								if(dw_stack.empty())
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_piece: DWARF stack is empty" << std::endl;
									return false; // DW_OP_piece must be preceeded by a register name operator or an address operation that have pushed a register number or an address on the stack
								}
								
								DWARF_LocationPiece<MEMORY_ADDR> *dw_loc_piece;
								if(in_dw_op_reg)
								{
									// register location
									MEMORY_ADDR dw_reg_num = dw_stack.back();
									dw_stack.pop_back();
									dw_loc_piece = new DWARF_RegisterLocationPiece<MEMORY_ADDR>(dw_reg_num, 0, dw_bit_size);
									in_dw_op_reg = false;
								}
								else
								{
									// memory location
									MEMORY_ADDR addr = dw_stack.back();
									dw_stack.pop_back();
									dw_loc_piece = new DWARF_MemoryLocationPiece<MEMORY_ADDR>(addr, 0, dw_bit_size);
								}
								
								dw_location->Add(dw_loc_piece);
								
								// Each simple location expression describes the location of one piece of the object;
								// each composition operator describes which part of the object is located there. Each simple
								// location expression that is a DWARF expression is evaluated independently of any others (as
								// though on its own separate stack).
								dw_stack.clear();
							}
						}
						break;
					case DW_OP_bit_piece:
						{
							DWARF_LEB128 dw_bit_size_leb128;
							int64_t sz;
							if((sz = dw_bit_size_leb128.Load(expr + expr_pos, expr_length - expr_pos)) < 0)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_bit_piece: missing LEB128 bit size operand" << std::endl;
								return false;
							}
							expr_pos += sz;
							
							uint64_t dw_bit_size = (uint64_t) dw_bit_size_leb128;

							DWARF_LEB128 dw_bit_offset_leb128;
							if((sz = dw_bit_offset_leb128.Load(expr + expr_pos, expr_length - expr_pos)) < 0)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_bit_piece: missing LEB128 offset operand" << std::endl;
								return false;
							}
							expr_pos += sz;
							
							uint64_t dw_bit_offset = (uint64_t) dw_bit_offset_leb128;
							
							if(os) *os << "DW_OP_bit_piece " << dw_bit_size << ", " << dw_bit_offset;
							if(executing)
							{
								if(!dw_location)
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_bit_piece: only allowed in location expressions" << std::endl;
									return false; // DW_OP_bit_piece is only allowed in location expressions
								}
								
								DWARF_LocationPiece<MEMORY_ADDR> *dw_loc_piece;
								if(dw_stack.empty())
								{
									// null location
									dw_loc_piece = new DWARF_LocationPiece<MEMORY_ADDR>(dw_bit_size);
								}
								else
								{
									if(in_dw_op_reg)
									{
										// register location
										MEMORY_ADDR dw_reg_num = dw_stack.back();
										dw_stack.pop_back();
										dw_loc_piece = new DWARF_RegisterLocationPiece<MEMORY_ADDR>(dw_reg_num, dw_bit_offset, dw_bit_size); // whole register
										in_dw_op_reg = false;
									}
									else
									{
										// memory location
										MEMORY_ADDR addr = dw_stack.back();
										dw_stack.pop_back();
										dw_loc_piece = new DWARF_MemoryLocationPiece<MEMORY_ADDR>(addr, dw_bit_offset, dw_bit_size);
									}
								}
								
								dw_location->Add(dw_loc_piece);
								
								// Each simple location expression describes the location of one piece of the object;
								// each composition operator describes which part of the object is located there. Each simple
								// location expression that is a DWARF expression is evaluated independently of any others (as
								// though on its own separate stack).
								dw_stack.clear();
							}
						}
						break;
						
					case DW_OP_implicit_value:
						{
							DWARF_LEB128 dw_length_leb128;
							int64_t sz;
							if((sz = dw_length_leb128.Load(expr + expr_pos, expr_length - expr_pos)) < 0)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_implicit_value: missing LEB128 length operand" << std::endl;
								return false;
							}
							expr_pos += sz;
							
							uint64_t dw_length = (uint64_t) dw_length_leb128;
							
							if((expr_pos + dw_length) > expr_length)
							{
								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_implicit_value: missing " << dw_length << "-byte block value operand" << std::endl;
								return false;
							}
							
							uint8_t dw_block_data[dw_length];
							memcpy(dw_block_data, expr + expr_pos, dw_length);
							
							expr_pos += dw_length;
							
							if(os)
							{
								*os << "DW_OP_implicit_value " << dw_length << ",";
								uint64_t i;
								for(i = 0; i < dw_length; i++)
								{
									*os << " 0x" << std::hex << dw_block_data[i] << std::dec;
								}
							}
							
							if(executing)
							{
								if(!dw_location)
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_implicit_value: only supported in location expressions" << std::endl;
									return false; // DW_OP_implicit_value is only supported in location expressions
								}
								
								DWARF_Block<MEMORY_ADDR> *dw_block = new DWARF_Block<MEMORY_ADDR>(dw_length, dw_block_data);
								
								dw_location->SetImplicitValue(dw_block);
							}
						}
						break;
					case DW_OP_stack_value:
						{
							if(os) *os << "DW_OP_stack_value";
							if(executing)
							{
// 								// Currently unimplemented.
// 								debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_stack_value: currently unimplemented" << std::endl;
// 								return false;
								if(!dw_location)
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_stack_value: only allowed in location expressions" << std::endl;
									return false; // DW_OP_stack_value is only allowed in location expressions
								}
								
								// DW_OP_stack_value terminates the expression
								if(expr_pos != expr_length)
								{
									debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", DW_OP_stack_value shall terminate the expression" << std::endl;
									return false;
								}
								
								in_dw_op_stack_value = true; // indicate that we started a DW_OP_stack_value and that the actual value is at top of the stack
							}
						}
						break;
						
					default:
						const char *producer = dw_cu ? dw_cu->GetProducer() : 0;
						debug_warning_stream << "In File \"" << dw_handler->GetFilename() << "\", in DWARF expression, unsupported ";
						if((opcode >= DW_OP_lo_user) && (opcode <= DW_OP_hi_user))
						{
							debug_warning_stream << "vendor specific extension";
							got_unsupported_vendor_specific_extension = true;
						}
						else
						{
							debug_warning_stream << "unknown operation";
							got_unknown_opcode = true;
						}
						
						debug_warning_stream << " " << DWARF_GetOPName(opcode) << " (opcode 0x" << std::hex << (unsigned int) opcode << std::dec << ") from " << (producer ? producer : "an unknown producer") << std::endl;
						
						if(os) *os << DWARF_GetOPName(opcode);
						if(executing)
						{
							debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", while evaluating a DWARF expression, unknown or invalid operation (0x" << std::hex << (unsigned int) opcode << std::dec << ")" << std::endl;
							return false;
						}
						break;
				}
			}
			
			if(os && (expr_pos < expr_length))
			{
				*os << "; "; // add a separator
			}
		}
		while(expr_pos < expr_length);
	}
	
	if(result_addr && !in_dw_op_reg && !dw_stack.empty()) *result_addr = dw_stack.back();
	if(dw_location && !dw_stack.empty())
	{
		if(in_dw_op_reg)
		{
			// register location
			MEMORY_ADDR dw_reg_num = dw_stack.back();
			dw_location->SetRegisterNumber(dw_reg_num);
		}
		else if(in_dw_op_stack_value)
		{
			// implicit value
			MEMORY_ADDR dw_stack_value = dw_stack.back();
			dw_location->SetImplicitValue(dw_stack_value);
		}
		else
		{
			// memory location
			MEMORY_ADDR addr = dw_stack.back();
			dw_location->SetAddress(addr);
		}
	}

	return !got_unsupported_vendor_specific_extension && !got_unknown_opcode;
}

template <class MEMORY_ADDR>
bool DWARF_ExpressionVM<MEMORY_ADDR>::ReadRegister(unsigned int dw_reg_num, MEMORY_ADDR& reg_value) const
{
	if(dw_frame) return dw_frame->ReadRegister(dw_reg_num, reg_value);
	
	const unisim::service::interfaces::Register *arch_reg = reg_num_mapping->GetArchReg(dw_reg_num);
	
	if(arch_reg)
	{
		unsigned int reg_size = arch_reg->GetSize();
		switch(reg_size)
		{
			case 1:
				{
					uint8_t arch_reg_value;
					arch_reg->GetValue(&arch_reg_value);
					reg_value = arch_reg_value;
				}
				return true;
			case 2:
				{
					uint16_t arch_reg_value;
					arch_reg->GetValue(&arch_reg_value);
					reg_value = arch_reg_value;
				}
				return true;
			case 4:
				{
					uint32_t arch_reg_value;
					arch_reg->GetValue(&arch_reg_value);
					reg_value = arch_reg_value;
				}
				return true;
			case 8:
				{
					uint64_t arch_reg_value;
					arch_reg->GetValue(&arch_reg_value);
					reg_value = arch_reg_value;
				}
				return true;
			default:
				debug_error_stream << "register size (" << reg_size << " bytes) is unsupported for reading" << std::endl;
				return false;
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_ExpressionVM<MEMORY_ADDR>::ReadAddrFromMemory(MEMORY_ADDR addr, MEMORY_ADDR& read_addr, unsigned int read_size, MEMORY_ADDR addr_space) const
{
	// FIXME: addr_space is currently ignored in our implementation
	if(read_size > arch_address_size)
	{
		debug_error_stream << "memory read of " << read_size << " bytes is unsupported" << std::endl;
		return false;
	}
	if(!read_size) read_size = arch_address_size;
	
	switch(read_size)
	{
		case sizeof(uint8_t):
			{
				uint8_t value = 0;
				if(!mem_if->ReadMemory(addr, &value, read_size)) return false;
				read_addr = value;
			}
			break;
		case sizeof(uint16_t):
			{
				uint16_t value = 0;
				if(!mem_if->ReadMemory(addr, &value, read_size)) return false;
				read_addr = unisim::util::endian::Target2Host(arch_endianness, value);
			}
			break;
		case 3:
			{
				uint8_t buf[3] = { 0, 0, 0 };
				if(!mem_if->ReadMemory(addr, buf, read_size)) return false;
				switch(arch_endianness)
				{
					case E_BIG_ENDIAN:
						read_addr = ((uint32_t) buf[0] << 16) | ((uint32_t) buf[1] << 8) | (uint32_t) buf[2];
						break;
					case E_LITTLE_ENDIAN:
						read_addr = (uint32_t) buf[0] | ((uint32_t) buf[1] << 8) | ((uint32_t) buf[2] << 16);
						break;
					default:
						read_addr = 0;
						break;
				}
			}
			break;
		case sizeof(uint32_t):
			{
				uint32_t value = 0;
				if(!mem_if->ReadMemory(addr, &value, read_size)) return false;
				read_addr = unisim::util::endian::Target2Host(arch_endianness, value);
			}
			break;
		case 5:
			{
				uint8_t buf[5] = { 0, 0, 0, 0, 0 };
				if(!mem_if->ReadMemory(addr, buf, read_size)) return false;
				switch(arch_endianness)
				{
					case E_BIG_ENDIAN:
						read_addr = ((uint64_t) buf[0] << 32) | ((uint64_t) buf[1] << 24) | ((uint64_t) buf[2] << 16) | ((uint64_t) buf[3] << 8) | (uint64_t) buf[4];
						break;
					case E_LITTLE_ENDIAN:
						read_addr = (uint64_t) buf[0] | ((uint64_t) buf[1] << 8) | ((uint64_t) buf[2] << 16) | ((uint64_t) buf[3] << 24) | ((uint64_t) buf[4] << 32);
						break;
					default:
						read_addr = 0;
						break;
				}
			}
			break;
		case 6:
			{
				uint8_t buf[6] = { 0, 0, 0, 0, 0, 0 };
				if(!mem_if->ReadMemory(addr, buf, read_size)) return false;
				switch(arch_endianness)
				{
					case E_BIG_ENDIAN:
						read_addr = ((uint64_t) buf[0] << 40) | ((uint64_t) buf[1] << 32) | ((uint64_t) buf[2] << 24) | ((uint64_t) buf[3] << 16) | ((uint64_t) buf[4] << 8) | (uint64_t) buf[5];
						break;
					case E_LITTLE_ENDIAN:
						read_addr = (uint64_t) buf[0] | ((uint64_t) buf[1] << 8) | ((uint64_t) buf[2] << 16) | ((uint64_t) buf[3] << 24) | ((uint64_t) buf[4] << 32) | ((uint64_t) buf[4] << 40);
						break;
					default:
						read_addr = 0;
						break;
				}
			}
			break;
		case 7:
			{
				uint8_t buf[7] = { 0, 0, 0, 0, 0, 0, 0 };
				if(!mem_if->ReadMemory(addr, buf, read_size)) return false;
				switch(arch_endianness)
				{
					case E_BIG_ENDIAN:
						read_addr = ((uint64_t) buf[0] << 48) | ((uint64_t) buf[1] << 40) | ((uint64_t) buf[2] << 32) | ((uint64_t) buf[3] << 24) | ((uint64_t) buf[4] << 16) | ((uint64_t) buf[5] << 8) | (uint64_t) buf[6];
						break;
					case E_LITTLE_ENDIAN:
						read_addr = (uint64_t) buf[0] | ((uint64_t) buf[1] << 8) | ((uint64_t) buf[2] << 16) | ((uint64_t) buf[3] << 24) | ((uint64_t) buf[4] << 32) | ((uint64_t) buf[4] << 40) | ((uint64_t) buf[5] << 48);
						break;
					default:
						read_addr = 0;
						break;
				}
			}
			break;
		case sizeof(uint64_t):
			{
				uint32_t value = 0;
				if(!mem_if->ReadMemory(addr, &value, read_size)) return false;
				read_addr = unisim::util::endian::Target2Host(arch_endianness, value);
			}
			break;
		default:
			debug_error_stream << "memory read of " << read_size << " bytes is unsupported" << std::endl;
			return false;
	}
	return true;
}

template <class MEMORY_ADDR>
void DWARF_ExpressionVM<MEMORY_ADDR>::SetFrameBase(MEMORY_ADDR _frame_base)
{
	frame_base = _frame_base;
	has_frame_base = true;
}

template <class MEMORY_ADDR>
void DWARF_ExpressionVM<MEMORY_ADDR>::SetObjectAddress(MEMORY_ADDR _object_addr)
{
	object_addr = _object_addr;
	has_object_addr = true;
}

template <class MEMORY_ADDR>
void DWARF_ExpressionVM<MEMORY_ADDR>::SetPC(MEMORY_ADDR _pc)
{
	pc = _pc;
	has_pc = true;
}

template <class MEMORY_ADDR>
void DWARF_ExpressionVM<MEMORY_ADDR>::Push(MEMORY_ADDR addr)
{
	dw_stack.push_back(addr);
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
