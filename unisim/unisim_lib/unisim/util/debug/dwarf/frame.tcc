/*
 *  Copyright (c) 2012,
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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_FRAME_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_FRAME_TCC__

#include <iostream>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;

template <class MEMORY_ADDR>
DWARF_RegSet<MEMORY_ADDR>::DWARF_RegSet()
	: reg_set()
{
}

template <class MEMORY_ADDR>
void DWARF_RegSet<MEMORY_ADDR>::UndefRegister(unsigned int reg_num)
{
	typename std::map<unsigned int, MEMORY_ADDR>::iterator iter = reg_set.find(reg_num);
	if(iter != reg_set.end())
	{
		reg_set.erase(iter);
	}
}

template <class MEMORY_ADDR>
bool DWARF_RegSet<MEMORY_ADDR>::ReadRegister(unsigned int reg_num, MEMORY_ADDR& reg_value) const
{
	typename std::map<unsigned int, MEMORY_ADDR>::const_iterator iter = reg_set.find(reg_num);
	if(iter != reg_set.end())
	{
		reg_value = (*iter).second;
		return true;
	}
	
	return false;
}

template <class MEMORY_ADDR>
void DWARF_RegSet<MEMORY_ADDR>::WriteRegister(unsigned int reg_num, MEMORY_ADDR value)
{
// 	std::cerr << "r" << reg_num << "<-0x" << std::hex << value << std::dec << std::endl;
	reg_set[reg_num] = value;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_RegSet<MEMORY_ADDR>& dw_reg_set)
{
	typename std::map<unsigned int, MEMORY_ADDR>::const_iterator iter;
	
	for(iter = dw_reg_set.reg_set.begin(); iter != dw_reg_set.reg_set.end(); iter++)
	{
		unsigned int reg_num = (*iter).first;
		MEMORY_ADDR reg_value = (*iter).second;
		
		os << "<r" << reg_num << "=0x" << std::hex << reg_value << std::dec << "> ";
	}
	
	return os;
}

template <class MEMORY_ADDR>
DWARF_Frame<MEMORY_ADDR>::DWARF_Frame(const DWARF_Handler<MEMORY_ADDR> *_dw_handler, unsigned int _prc_num)
	: dw_handler(_dw_handler)
	, prc_num(_prc_num)
	, sp_reg_num(_dw_handler->GetRegisterNumberMapping()->GetSPRegNum())
	, endianness(_dw_handler->GetArchEndianness())
	, address_size(_dw_handler->GetArchAddressSize())
	, dw_cfa_spec(_dw_handler->GetCFA_Specification())
	, dw_cfa_reg_rule_offset_spec(_dw_handler->GetCFA_RegRuleOffsetSpecification())
	, mem_if(_dw_handler->GetMemoryInterface(prc_num))
	, pc_is_defined(false)
	, pc(0)
	, cfa(0)
	, reg_num_set()
	, dw_reg_set()
{
	dw_handler->GetRegisterNumberMapping(prc_num)->EnumRegisterNumbers(reg_num_set);
}

template <class MEMORY_ADDR>
DWARF_Frame<MEMORY_ADDR>::DWARF_Frame(const DWARF_Handler<MEMORY_ADDR> *_dw_handler, unsigned int _prc_num, MEMORY_ADDR _pc)
	: dw_handler(_dw_handler)
	, prc_num(_prc_num)
	, sp_reg_num(_dw_handler->GetRegisterNumberMapping()->GetSPRegNum())
	, endianness(_dw_handler->GetArchEndianness())
	, address_size(_dw_handler->GetArchAddressSize())
	, dw_cfa_spec(_dw_handler->GetCFA_Specification())
	, dw_cfa_reg_rule_offset_spec(_dw_handler->GetCFA_RegRuleOffsetSpecification())
	, mem_if(_dw_handler->GetMemoryInterface(prc_num))
	, pc_is_defined(true)
	, pc(_pc)
	, cfa(0)
	, reg_num_set()
	, dw_reg_set()
{
	dw_handler->GetRegisterNumberMapping(prc_num)->EnumRegisterNumbers(reg_num_set);
}

template <class MEMORY_ADDR>
DWARF_Frame<MEMORY_ADDR>::~DWARF_Frame()
{
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_Frame<MEMORY_ADDR>::ReadCFA() const
{
	return cfa;
}

template <class MEMORY_ADDR>
bool DWARF_Frame<MEMORY_ADDR>::GetPC(MEMORY_ADDR& pc_value) const
{
	if(pc_is_defined)
	{
		pc_value = pc;
		return true;
	}
	return false;
}

template <class MEMORY_ADDR>
const DWARF_RegSet<MEMORY_ADDR>& DWARF_Frame<MEMORY_ADDR>::GetRegSet() const
{
	return dw_reg_set;
}

template <class MEMORY_ADDR>
bool DWARF_Frame<MEMORY_ADDR>::ReadRegister(unsigned int reg_num, MEMORY_ADDR& reg_value) const
{
	return dw_reg_set.ReadRegister(reg_num, reg_value);
}

template <class MEMORY_ADDR>
bool DWARF_Frame<MEMORY_ADDR>::ReadAddrFromMemory(MEMORY_ADDR addr, MEMORY_ADDR& read_addr, unsigned int read_size) const
{
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
				read_addr = unisim::util::endian::Target2Host(endianness, value);
			}
			break;
		case 3:
			{
				uint8_t buf[3] = { 0, 0, 0 };
				if(!mem_if->ReadMemory(addr, buf, read_size)) return false;
				switch(endianness)
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
				read_addr = unisim::util::endian::Target2Host(endianness, value);
			}
			break;
		case sizeof(uint64_t):
			{
				uint32_t value = 0;
				if(!mem_if->ReadMemory(addr, &value, read_size)) return false;
				read_addr = unisim::util::endian::Target2Host(endianness, value);
			}
			break;
		default:
			return false;
	}
//	std::cerr << "ReadAddrFromMemory(0x" << std::hex << addr << ", 0x" << read_addr << ", " << std::dec << read_size << ");" << std::endl;
	return true;
}

template <class MEMORY_ADDR>
bool DWARF_Frame<MEMORY_ADDR>::LoadArchRegs()
{
	DWARF_RegisterNumberMapping *reg_num_mapping = dw_handler->GetRegisterNumberMapping(prc_num);
	
	std::set<unsigned int>::const_iterator iter;
	
	for(iter = reg_num_set.begin(); iter != reg_num_set.end(); iter++)
	{
		unsigned int dw_reg_num = *iter;
		const unisim::service::interfaces::Register *arch_reg = reg_num_mapping->GetArchReg(dw_reg_num);
		
		if(!arch_reg)
		{
			throw std::runtime_error("Internal error");
			return false;
		}
		
		unsigned int arch_reg_size = arch_reg->GetSize();
		MEMORY_ADDR reg_value = 0;
		
		switch(arch_reg_size)
		{
			case 1:
				{
					uint8_t arch_reg_value = 0;
					arch_reg->GetValue(&arch_reg_value);
					reg_value = arch_reg_value;
				}
				break;
			case 2:
				{
					uint16_t arch_reg_value = 0;
					arch_reg->GetValue(&arch_reg_value);
					reg_value = arch_reg_value;
				}
				break;
			case 4:
				{
					uint32_t arch_reg_value = 0;
					arch_reg->GetValue(&arch_reg_value);
					reg_value = arch_reg_value;
				}
				break;
			case 8:
				{
					uint64_t arch_reg_value = 0;
					arch_reg->GetValue(&arch_reg_value);
					reg_value = arch_reg_value;
				}
				break;
			default:
				throw std::runtime_error("Internal error");
				return false;
		}
		
		dw_reg_set.WriteRegister(dw_reg_num, reg_value);
	}
	
	return true;
}

template <class MEMORY_ADDR>
bool DWARF_Frame<MEMORY_ADDR>::ComputeCFA(const DWARF_CFIRow<MEMORY_ADDR> *cfi_row, const DWARF_Frame<MEMORY_ADDR> *next_frame)
{
	DWARF_CFARule<MEMORY_ADDR> *cfa_rule = cfi_row->GetCFARule();
	
	switch(cfa_rule->GetType())
	{
		case DW_CFA_RULE_REGISTER_OFFSET:
			{
				DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *cfa_rule_reg_ofs = reinterpret_cast<DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *>(cfa_rule);
				unsigned int dw_reg_num = cfa_rule_reg_ofs->GetRegisterNumber();
				int64_t offset = cfa_rule_reg_ofs->GetOffset();
				MEMORY_ADDR reg_value = 0;
				if(!next_frame->ReadRegister(dw_reg_num, reg_value)) return false;
				cfa = reg_value + offset;
			}
			break;
		case DW_CFA_RULE_EXPRESSION:
			{
				DWARF_CFARuleExpression<MEMORY_ADDR> *cfa_rule_expr = reinterpret_cast<DWARF_CFARuleExpression<MEMORY_ADDR> *>(cfa_rule);
				DWARF_ExpressionVM<MEMORY_ADDR> cfa_rule_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_handler, this);
				bool cfa_rule_expr_status = cfa_rule_expr_vm.Execute(cfa_rule_expr->GetExpression(), cfa, 0);
				if(!cfa_rule_expr_status) return false;
			}
			break;
	}
	
	return true;
}

template <class MEMORY_ADDR>
bool DWARF_Frame<MEMORY_ADDR>::Unwind(const DWARF_CFIRow<MEMORY_ADDR> *cfi_row, const DWARF_Frame<MEMORY_ADDR> *next_frame, unsigned int dw_ret_addr_reg_num)
{
	reg_num_set = next_frame->reg_num_set;
	std::set<unsigned int>::const_iterator iter;
	
	for(iter = reg_num_set.begin(); iter != reg_num_set.end(); iter++)
	{
		unsigned int dw_reg_num = *iter;
		
		MEMORY_ADDR reg_value = 0;
		if(next_frame->ReadRegister(dw_reg_num, reg_value))
		{
			dw_reg_set.WriteRegister(dw_reg_num, reg_value);
		}
	}
	
	MEMORY_ADDR sp = 0;
	next_frame->ReadRegister(sp_reg_num, sp);
	
	DWARF_CFARule<MEMORY_ADDR> *cfa_rule = cfi_row->GetCFARule();
	
	switch(cfa_rule->GetType())
	{
		case DW_CFA_RULE_REGISTER_OFFSET:
			{
				DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *cfa_rule_reg_ofs = reinterpret_cast<DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *>(cfa_rule);
				unsigned int dw_reg_num = cfa_rule_reg_ofs->GetRegisterNumber();
				int64_t offset = cfa_rule_reg_ofs->GetOffset();
				MEMORY_ADDR reg_value = 0;
				if(!next_frame->ReadRegister(dw_reg_num, reg_value)) return false;
				cfa = reg_value + offset;
			}
			break;
		case DW_CFA_RULE_EXPRESSION:
			{
				DWARF_CFARuleExpression<MEMORY_ADDR> *cfa_rule_expr = reinterpret_cast<DWARF_CFARuleExpression<MEMORY_ADDR> *>(cfa_rule);
				DWARF_ExpressionVM<MEMORY_ADDR> cfa_rule_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_handler, this);
				bool cfa_rule_expr_status = cfa_rule_expr_vm.Execute(cfa_rule_expr->GetExpression(), cfa, 0);
				if(!cfa_rule_expr_status) return false;
			}
			break;
	}
	
	typename std::set<unsigned int> dw_reg_nums;
	
	cfi_row->GetRegisterRulesNumbers(dw_reg_nums); // complete register set with "virtual" register rules

	typename std::set<unsigned int>::iterator dw_reg_nums_iter;
	for(dw_reg_nums_iter = dw_reg_nums.begin(); dw_reg_nums_iter != dw_reg_nums.end(); dw_reg_nums_iter++)
	{
		unsigned int dw_reg_num = *dw_reg_nums_iter;
		DWARF_RegisterRule<MEMORY_ADDR> *reg_rule = cfi_row->GetRegisterRule(dw_reg_num);
		
		if(reg_rule)
		{
			switch(reg_rule->GetType())
			{
				case DW_REG_RULE_UNDEFINED:
					dw_reg_set.UndefRegister(dw_reg_num);
					break;
				case DW_REG_RULE_SAME_VALUE:
					{
						MEMORY_ADDR reg_value;
						if(!next_frame->ReadRegister(dw_reg_num, reg_value)) return false;
						dw_reg_set.WriteRegister(dw_reg_num, reg_value);
					}
					break;
				case DW_REG_RULE_OFFSET:
					{
						DWARF_RegisterRuleOffset<MEMORY_ADDR> *reg_rule_offset = reinterpret_cast<DWARF_RegisterRuleOffset<MEMORY_ADDR> *>(reg_rule);
						int64_t offset = reg_rule_offset->GetOffset();
						MEMORY_ADDR addr = 0;
						switch(dw_cfa_reg_rule_offset_spec)
						{
							case DW_CFA_REG_RULE_OFFSET_IS_CFA_RELATIVE: // standard
								addr = cfa + offset;
								break;
							case DW_CFA_REG_RULE_OFFSET_IS_SP_RELATIVE: // non-standard
								addr = sp + offset;
								break;
						}
						MEMORY_ADDR prev_reg_value;
						if(!next_frame->ReadAddrFromMemory(addr, prev_reg_value, (dw_reg_num == dw_ret_addr_reg_num) ? dw_handler->GetReturnAddressSize(cfi_row->GetLocation()) : address_size)) return false;
						dw_reg_set.WriteRegister(dw_reg_num, prev_reg_value);
					}
					break;
				case DW_REG_RULE_VAL_OFFSET:
					{
						DWARF_RegisterRuleValOffset<MEMORY_ADDR> *reg_rule_val_offset = reinterpret_cast<DWARF_RegisterRuleValOffset<MEMORY_ADDR> *>(reg_rule);
						int64_t offset = reg_rule_val_offset->GetOffset();
						MEMORY_ADDR prev_reg_value = 0;
						switch(dw_cfa_reg_rule_offset_spec)
						{
							case DW_CFA_REG_RULE_OFFSET_IS_CFA_RELATIVE: // standard
								prev_reg_value = cfa + offset;
								break;
							case DW_CFA_REG_RULE_OFFSET_IS_SP_RELATIVE: // non-standard
								prev_reg_value = sp + offset;
								break;
						}
						dw_reg_set.WriteRegister(dw_reg_num, prev_reg_value);
					}
					break;
				case DW_REG_RULE_REGISTER:
					{
						DWARF_RegisterRuleRegister<MEMORY_ADDR> *reg_rule_reg = reinterpret_cast<DWARF_RegisterRuleRegister<MEMORY_ADDR> *>(reg_rule);
						unsigned int dw_prev_reg_num = reg_rule_reg->GetRegisterNumber();
						MEMORY_ADDR prev_reg_value;
						if(!next_frame->ReadRegister(dw_prev_reg_num, prev_reg_value)) return false;
						dw_reg_set.WriteRegister(dw_reg_num, prev_reg_value);
					}
					break;
				case DW_REG_RULE_EXPRESSION:
					{
						DWARF_RegisterRuleExpression<MEMORY_ADDR> *reg_rule_expr = reinterpret_cast<DWARF_RegisterRuleExpression<MEMORY_ADDR> *>(reg_rule);
						DWARF_ExpressionVM<MEMORY_ADDR> reg_rule_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_handler, this);
						MEMORY_ADDR prev_reg_value_addr = 0;
						bool reg_rule_expr_status = reg_rule_expr_vm.Execute(reg_rule_expr->GetExpression(), prev_reg_value_addr, 0);
						if(!reg_rule_expr_status) return false;
						MEMORY_ADDR prev_reg_value = 0;
						bool read_status = ReadAddrFromMemory(prev_reg_value_addr, prev_reg_value, (dw_reg_num == dw_ret_addr_reg_num) ? dw_handler->GetReturnAddressSize(cfi_row->GetLocation()) : address_size);
						if(!read_status) return false;
						dw_reg_set.WriteRegister(dw_reg_num, prev_reg_value);
					}
					break;
				case DW_REG_RULE_VAL_EXPRESSION:
					{
						DWARF_RegisterRuleValExpression<MEMORY_ADDR> *reg_rule_val_expr = reinterpret_cast<DWARF_RegisterRuleValExpression<MEMORY_ADDR> *>(reg_rule);
						DWARF_ExpressionVM<MEMORY_ADDR> reg_rule_val_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_handler, this);
						MEMORY_ADDR prev_reg_value = 0;
						bool reg_rule_expr_status = reg_rule_val_expr_vm.Execute(reg_rule_val_expr->GetExpression(), prev_reg_value, 0);
						if(!reg_rule_expr_status) return false;
						dw_reg_set.WriteRegister(dw_reg_num, prev_reg_value);
					}
					break;
			}
		}
	}
	
	// architectural rule: SP = CFA (standard) or CFA + call context size (non-standard)
	switch(dw_cfa_spec)
	{
		case DW_CFA_IS_SP_AT_THE_CALL_SITE_IN_THE_PREVIOUS_FRAME:
			dw_reg_set.WriteRegister(sp_reg_num, cfa);
			break;
		case DW_CFA_IS_SP_VALUE_ON_ENTRY_TO_THE_CURRENT_FRAME:
			dw_reg_set.WriteRegister(sp_reg_num, cfa + dw_handler->GetReturnAddressSize(cfi_row->GetLocation()));
			break;
	}

	// return address of next frame
	pc_is_defined = dw_reg_set.ReadRegister(dw_ret_addr_reg_num, pc);
	
	return true;
}

template <class MEMORY_ADDR>
unsigned int DWARF_Frame<MEMORY_ADDR>::GetProcessorNumber() const
{
	return prc_num;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
