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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_CALL_FRAME_VM_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_CALL_FRAME_VM_TCC__

#include <stdexcept>
#include <unisim/util/debug/dwarf/option.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_CFARule<MEMORY_ADDR>::DWARF_CFARule(unsigned int _type)
	: type(_type)
{
}

template <class MEMORY_ADDR>
DWARF_CFARule<MEMORY_ADDR>::~DWARF_CFARule()
{
}

template <class MEMORY_ADDR>
unsigned int DWARF_CFARule<MEMORY_ADDR>::GetType() const
{
	return type;
}

template <class MEMORY_ADDR>
DWARF_CFARule<MEMORY_ADDR> *DWARF_CFARule<MEMORY_ADDR>::Clone()
{
	switch(type)
	{
		case DW_CFA_RULE_REGISTER_OFFSET:
			return new DWARF_CFARuleRegisterOffset<MEMORY_ADDR>(*reinterpret_cast<const DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *>(this));
		case DW_CFA_RULE_EXPRESSION:
			return new DWARF_CFARuleExpression<MEMORY_ADDR>(*reinterpret_cast<const DWARF_CFARuleExpression<MEMORY_ADDR> *>(this));
	}
	return 0;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CFARule<MEMORY_ADDR>& cfa_rule)
{
	return cfa_rule.Print(os);
}

template <class MEMORY_ADDR>
DWARF_CFARuleRegisterOffset<MEMORY_ADDR>::DWARF_CFARuleRegisterOffset(unsigned int _dw_reg_num, int64_t _dw_offset)
	: DWARF_CFARule<MEMORY_ADDR>(DW_CFA_RULE_REGISTER_OFFSET)
	, dw_reg_num(_dw_reg_num)
	, dw_offset(_dw_offset)
{
}

template <class MEMORY_ADDR>
DWARF_CFARuleRegisterOffset<MEMORY_ADDR>::DWARF_CFARuleRegisterOffset(const DWARF_CFARuleRegisterOffset<MEMORY_ADDR>& cfa_rule_offset)
	: DWARF_CFARule<MEMORY_ADDR>(DW_CFA_RULE_REGISTER_OFFSET)
	, dw_reg_num(cfa_rule_offset.dw_reg_num)
	, dw_offset(cfa_rule_offset.dw_offset)
{
}

template <class MEMORY_ADDR>
DWARF_CFARuleRegisterOffset<MEMORY_ADDR>::~DWARF_CFARuleRegisterOffset()
{
}

template <class MEMORY_ADDR>
void DWARF_CFARuleRegisterOffset<MEMORY_ADDR>::SetRegisterNumber(unsigned int _dw_reg_num)
{
	dw_reg_num = _dw_reg_num;
}

template <class MEMORY_ADDR>
void DWARF_CFARuleRegisterOffset<MEMORY_ADDR>::SetOffset(int64_t _dw_offset)
{
	dw_offset = _dw_offset;
}

template <class MEMORY_ADDR>
unsigned int DWARF_CFARuleRegisterOffset<MEMORY_ADDR>::GetRegisterNumber() const
{
	return dw_reg_num;
}

template <class MEMORY_ADDR>
int64_t DWARF_CFARuleRegisterOffset<MEMORY_ADDR>::GetOffset() const
{
	return dw_offset;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_CFARuleRegisterOffset<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "cfa=r" << dw_reg_num << "+" << dw_offset;
}

template <class MEMORY_ADDR>
DWARF_CFARuleExpression<MEMORY_ADDR>::DWARF_CFARuleExpression(const DWARF_Expression<MEMORY_ADDR> *_dw_expr)
	: DWARF_CFARule<MEMORY_ADDR>(DW_CFA_RULE_EXPRESSION)
	, dw_expr(_dw_expr)
{
}

template <class MEMORY_ADDR>
DWARF_CFARuleExpression<MEMORY_ADDR>::DWARF_CFARuleExpression(const DWARF_CFARuleExpression<MEMORY_ADDR>& cfa_rule_expression)
	: DWARF_CFARule<MEMORY_ADDR>(DW_CFA_RULE_EXPRESSION)
	, dw_expr(new DWARF_Expression<MEMORY_ADDR>(*cfa_rule_expression.dw_expr))
{
}

template <class MEMORY_ADDR>
DWARF_CFARuleExpression<MEMORY_ADDR>::~DWARF_CFARuleExpression()
{
	if(dw_expr) delete dw_expr;
}

template <class MEMORY_ADDR>
const DWARF_Expression<MEMORY_ADDR> *DWARF_CFARuleExpression<MEMORY_ADDR>::GetExpression() const
{
	return dw_expr;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_CFARuleExpression<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "cfa={" << *dw_expr << "}";
}

template <class MEMORY_ADDR>
DWARF_RegisterRule<MEMORY_ADDR>::DWARF_RegisterRule(unsigned int _dw_rule_type)
	: dw_rule_type(_dw_rule_type)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRule<MEMORY_ADDR>::~DWARF_RegisterRule()
{
}

template <class MEMORY_ADDR>
unsigned int DWARF_RegisterRule<MEMORY_ADDR>::GetType() const
{
	return dw_rule_type;
}

template <class MEMORY_ADDR>
DWARF_RegisterRule<MEMORY_ADDR> *DWARF_RegisterRule<MEMORY_ADDR>::Clone()
{
	switch(dw_rule_type)
	{
		case DW_REG_RULE_UNDEFINED:
			return new DWARF_RegisterRuleUndefined<MEMORY_ADDR>(*reinterpret_cast<const DWARF_RegisterRuleUndefined<MEMORY_ADDR> *>(this));
		case DW_REG_RULE_SAME_VALUE:
			return new DWARF_RegisterRuleSameValue<MEMORY_ADDR>(*reinterpret_cast<const DWARF_RegisterRuleSameValue<MEMORY_ADDR> *>(this));
		case DW_REG_RULE_OFFSET:
			return new DWARF_RegisterRuleOffset<MEMORY_ADDR>(*reinterpret_cast<const DWARF_RegisterRuleOffset<MEMORY_ADDR> *>(this));
		case DW_REG_RULE_VAL_OFFSET:
			return new DWARF_RegisterRuleValOffset<MEMORY_ADDR>(*reinterpret_cast<const DWARF_RegisterRuleValOffset<MEMORY_ADDR> *>(this));
		case DW_REG_RULE_REGISTER:
			return new DWARF_RegisterRuleRegister<MEMORY_ADDR>(*reinterpret_cast<const DWARF_RegisterRuleRegister<MEMORY_ADDR> *>(this));
		case DW_REG_RULE_EXPRESSION:
			return new DWARF_RegisterRuleExpression<MEMORY_ADDR>(*reinterpret_cast<const DWARF_RegisterRuleExpression<MEMORY_ADDR> *>(this));
		case DW_REG_RULE_VAL_EXPRESSION:
			return new DWARF_RegisterRuleValExpression<MEMORY_ADDR>(*reinterpret_cast<const DWARF_RegisterRuleValExpression<MEMORY_ADDR> *>(this));
	}
	return 0;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_RegisterRule<MEMORY_ADDR>& register_rule)
{
	return register_rule.Print(os);
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleUndefined<MEMORY_ADDR>::DWARF_RegisterRuleUndefined(const DWARF_RegisterRuleUndefined<MEMORY_ADDR>& rule)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_UNDEFINED)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleUndefined<MEMORY_ADDR>::DWARF_RegisterRuleUndefined()
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_UNDEFINED)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleUndefined<MEMORY_ADDR>::~DWARF_RegisterRuleUndefined()
{
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RegisterRuleUndefined<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "undefined";
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleSameValue<MEMORY_ADDR>::DWARF_RegisterRuleSameValue(const DWARF_RegisterRuleSameValue<MEMORY_ADDR>& rule)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_SAME_VALUE)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleSameValue<MEMORY_ADDR>::DWARF_RegisterRuleSameValue()
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_SAME_VALUE)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleSameValue<MEMORY_ADDR>::~DWARF_RegisterRuleSameValue()
{
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RegisterRuleSameValue<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "preserved";
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleOffset<MEMORY_ADDR>::DWARF_RegisterRuleOffset(const DWARF_RegisterRuleOffset<MEMORY_ADDR>& rule)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_OFFSET)
	, dw_offset(rule.dw_offset)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleOffset<MEMORY_ADDR>::DWARF_RegisterRuleOffset(int64_t _dw_offset)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_OFFSET)
	, dw_offset(_dw_offset)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleOffset<MEMORY_ADDR>::~DWARF_RegisterRuleOffset()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_RegisterRuleOffset<MEMORY_ADDR>::GetOffset() const
{
	return dw_offset;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RegisterRuleOffset<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "[cfa" << ((dw_offset < 0) ? "-" : "+") << ((dw_offset < 0) ? -dw_offset : dw_offset) << "]";
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleValOffset<MEMORY_ADDR>::DWARF_RegisterRuleValOffset(const DWARF_RegisterRuleValOffset<MEMORY_ADDR>& rule)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_VAL_OFFSET)
	, dw_offset(rule.dw_offset)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleValOffset<MEMORY_ADDR>::DWARF_RegisterRuleValOffset(int64_t _dw_offset)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_VAL_OFFSET)
	, dw_offset(_dw_offset)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleValOffset<MEMORY_ADDR>::~DWARF_RegisterRuleValOffset()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_RegisterRuleValOffset<MEMORY_ADDR>::GetOffset() const
{
	return dw_offset;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RegisterRuleValOffset<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "cfa+" << dw_offset;
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleRegister<MEMORY_ADDR>::DWARF_RegisterRuleRegister(const DWARF_RegisterRuleRegister<MEMORY_ADDR>& rule)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_REGISTER)
	, dw_reg_num(rule.dw_reg_num)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleRegister<MEMORY_ADDR>::DWARF_RegisterRuleRegister(unsigned int _dw_reg_num)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_REGISTER)
	, dw_reg_num(_dw_reg_num)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleRegister<MEMORY_ADDR>::~DWARF_RegisterRuleRegister()
{
}

template <class MEMORY_ADDR>
unsigned int DWARF_RegisterRuleRegister<MEMORY_ADDR>::GetRegisterNumber() const
{
	return dw_reg_num;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RegisterRuleRegister<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "r" << dw_reg_num;
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleExpression<MEMORY_ADDR>::DWARF_RegisterRuleExpression(const DWARF_RegisterRuleExpression<MEMORY_ADDR>& rule)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_EXPRESSION)
	, dw_expr(new DWARF_Expression<MEMORY_ADDR>(*rule.dw_expr))
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleExpression<MEMORY_ADDR>::DWARF_RegisterRuleExpression(const DWARF_Expression<MEMORY_ADDR> *_dw_expr)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_EXPRESSION)
	, dw_expr(new DWARF_Expression<MEMORY_ADDR>(*_dw_expr))
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleExpression<MEMORY_ADDR>::~DWARF_RegisterRuleExpression()
{
}

template <class MEMORY_ADDR>
const DWARF_Expression<MEMORY_ADDR> *DWARF_RegisterRuleExpression<MEMORY_ADDR>::GetExpression() const
{
	return dw_expr;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RegisterRuleExpression<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "[{" << *dw_expr << "}]";
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleValExpression<MEMORY_ADDR>::DWARF_RegisterRuleValExpression(const DWARF_RegisterRuleValExpression<MEMORY_ADDR>& rule)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_VAL_EXPRESSION)
	, dw_expr(new DWARF_Expression<MEMORY_ADDR>(*rule.dw_expr))
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleValExpression<MEMORY_ADDR>::DWARF_RegisterRuleValExpression(const DWARF_Expression<MEMORY_ADDR> *_dw_expr)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_VAL_EXPRESSION)
	, dw_expr(new DWARF_Expression<MEMORY_ADDR>(*_dw_expr))
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleValExpression<MEMORY_ADDR>::~DWARF_RegisterRuleValExpression()
{
}

template <class MEMORY_ADDR>
const DWARF_Expression<MEMORY_ADDR> *DWARF_RegisterRuleValExpression<MEMORY_ADDR>::GetExpression() const
{
	return dw_expr;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RegisterRuleValExpression<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "{" << *dw_expr << "}";
}

template <class MEMORY_ADDR>
DWARF_CFIRow<MEMORY_ADDR>::DWARF_CFIRow()
	: location(0)
	, cfa_rule(0)
	, reg_rules()
{
}

template <class MEMORY_ADDR>
DWARF_CFIRow<MEMORY_ADDR>::DWARF_CFIRow(MEMORY_ADDR _location)
	: location(_location)
	, cfa_rule(0)
	, reg_rules()
{
}

template <class MEMORY_ADDR>
DWARF_CFIRow<MEMORY_ADDR>::DWARF_CFIRow(const DWARF_CFIRow<MEMORY_ADDR>& cfi_row)
	: location(cfi_row.location)
	, cfa_rule(0)
	, reg_rules()
{
	DWARF_CFARule<MEMORY_ADDR> *orig_cfa_rule = cfi_row.GetCFARule();
	
	if(orig_cfa_rule)
	{
		cfa_rule = orig_cfa_rule->Clone();
	}
	
	typename std::map<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>::const_iterator it;
	
	for(it = cfi_row.reg_rules.begin(); it != cfi_row.reg_rules.end(); it++)
	{
		unsigned int reg_num = (*it).first;
		DWARF_RegisterRule<MEMORY_ADDR> *orig_rule = (*it).second;
		DWARF_RegisterRule<MEMORY_ADDR> *rule_copy = orig_rule ? orig_rule->Clone() : 0;
		reg_rules.insert(std::pair<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>(reg_num, rule_copy));
	}
}

template <class MEMORY_ADDR>
DWARF_CFIRow<MEMORY_ADDR>::DWARF_CFIRow(MEMORY_ADDR _location, const DWARF_CFIRow<MEMORY_ADDR>& cfi_row)
	: location(_location)
	, cfa_rule(0)
	, reg_rules()
{
	DWARF_CFARule<MEMORY_ADDR> *orig_cfa_rule = cfi_row.GetCFARule();
	
	if(orig_cfa_rule)
	{
		cfa_rule = orig_cfa_rule->Clone();
	}
		
	typename std::map<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>::const_iterator it;
	
	for(it = cfi_row.reg_rules.begin(); it != cfi_row.reg_rules.end(); it++)
	{
		unsigned int reg_num = (*it).first;
		DWARF_RegisterRule<MEMORY_ADDR> *orig_rule = (*it).second;
		DWARF_RegisterRule<MEMORY_ADDR> *rule_copy = orig_rule ? orig_rule->Clone() : 0;
		reg_rules.insert(std::pair<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>(reg_num, rule_copy));
	}
}

template <class MEMORY_ADDR>
DWARF_CFIRow<MEMORY_ADDR>::~DWARF_CFIRow()
{
	if(cfa_rule) delete cfa_rule;

	typename std::map<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>::const_iterator it;
	
	for(it = reg_rules.begin(); it != reg_rules.end(); it++)
	{
		DWARF_RegisterRule<MEMORY_ADDR> *reg_rule = (*it).second;
		if(reg_rule) delete reg_rule;
	}
}

template <class MEMORY_ADDR>
void DWARF_CFIRow<MEMORY_ADDR>::SetCFARule(DWARF_CFARule<MEMORY_ADDR> *_cfa_rule)
{
	if(cfa_rule) delete cfa_rule;
	cfa_rule = _cfa_rule;
}

template <class MEMORY_ADDR>
void DWARF_CFIRow<MEMORY_ADDR>::SetRegisterRule(unsigned int reg_num, DWARF_RegisterRule<MEMORY_ADDR> *reg_rule)
{
	if(!reg_rule)
	{
		reg_rule = new DWARF_RegisterRuleUndefined<MEMORY_ADDR>();
	}
	
	typename std::map<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>::iterator it = reg_rules.find(reg_num);
	
	if(it != reg_rules.end())
	{
		DWARF_RegisterRule<MEMORY_ADDR> *old_reg_rule = (*it).second;
		reg_rules.erase(it);
		if(old_reg_rule) delete old_reg_rule;
	}
	reg_rules.insert(std::pair<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>(reg_num, reg_rule));
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_CFIRow<MEMORY_ADDR>::GetLocation() const
{
	return location;
}

template <class MEMORY_ADDR>
DWARF_CFARule<MEMORY_ADDR> *DWARF_CFIRow<MEMORY_ADDR>::GetCFARule() const
{
	return cfa_rule;
}

template <class MEMORY_ADDR>
DWARF_RegisterRule<MEMORY_ADDR> *DWARF_CFIRow<MEMORY_ADDR>::GetRegisterRule(unsigned int reg_num) const
{
	typename std::map<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>::const_iterator it = reg_rules.find(reg_num);
	return (it != reg_rules.end()) ? (*it).second : 0;
}

template <class MEMORY_ADDR>
void DWARF_CFIRow<MEMORY_ADDR>::GetRegisterRulesNumbers(std::set<unsigned int>& reg_rule_nums) const
{
	typename std::map<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>::const_iterator it;
	
	for(it = reg_rules.begin(); it != reg_rules.end(); it++)
	{
		unsigned int dw_reg_num = (*it).first;
		reg_rule_nums.insert(dw_reg_num);
	}
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CFIRow<MEMORY_ADDR>& cfi_row)
{
	os << std::hex << "pc=0x" << cfi_row.location << std::dec << "  <";
	if(cfi_row.cfa_rule)
	{
		os << *cfi_row.cfa_rule;
	}
	else
	{
		os << "cfa=?";
	}
	os << ">";
	typename std::map<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>::const_iterator it;
		
	for(it = cfi_row.reg_rules.begin(); it != cfi_row.reg_rules.end(); it++)
	{
		os << " <";
		DWARF_RegisterRule<MEMORY_ADDR> *reg_rule = (*it).second;
		unsigned int reg_num = (*it).first;
		if(reg_rule)
		{
			os << "r" << reg_num << "=" << *reg_rule;
		}
		else
		{
			os << "r" << reg_num << "=undefined";
		}
		os << ">";
	}
	return os;
}

template <class MEMORY_ADDR>
DWARF_CFI<MEMORY_ADDR>::DWARF_CFI()
	: initial_row(new DWARF_CFIRow<MEMORY_ADDR>())
{
}

template <class MEMORY_ADDR>
DWARF_CFI<MEMORY_ADDR>::DWARF_CFI(const DWARF_CFI<MEMORY_ADDR>& cfi)
	: initial_row(0)
{
	typename std::map<MEMORY_ADDR, DWARF_CFIRow<MEMORY_ADDR> *>::const_iterator iter;
	
	for(iter = cfi.cfi_rows.begin(); iter != cfi.cfi_rows.end(); iter++)
	{
		DWARF_CFIRow<MEMORY_ADDR> *row = (*iter).second;
		DWARF_CFIRow<MEMORY_ADDR> *new_row = new DWARF_CFIRow<MEMORY_ADDR>(*row);
		cfi_rows.insert(std::pair<MEMORY_ADDR, DWARF_CFIRow<MEMORY_ADDR> *>(new_row->GetLocation(), new_row));
	}
	
	initial_row = new DWARF_CFIRow<MEMORY_ADDR>(*cfi.GetInitialRow());
}

template <class MEMORY_ADDR>
DWARF_CFI<MEMORY_ADDR>::~DWARF_CFI()
{
	typename std::map<MEMORY_ADDR, DWARF_CFIRow<MEMORY_ADDR> *>::iterator iter;
	
	for(iter = cfi_rows.begin(); iter != cfi_rows.end(); iter++)
	{
		delete (*iter).second;
	}
	
	if(initial_row) delete initial_row;
}

template <class MEMORY_ADDR>
DWARF_CFIRow<MEMORY_ADDR> *DWARF_CFI<MEMORY_ADDR>::operator[](MEMORY_ADDR loc) const
{
	typename std::map<MEMORY_ADDR, DWARF_CFIRow<MEMORY_ADDR> *>::const_iterator iter = cfi_rows.find(loc);
	
	return (iter != cfi_rows.end()) ? (*iter).second : 0;
}

template <class MEMORY_ADDR>
DWARF_CFIRow<MEMORY_ADDR> *DWARF_CFI<MEMORY_ADDR>::GetRow(MEMORY_ADDR loc) const
{
	typename std::map<MEMORY_ADDR, DWARF_CFIRow<MEMORY_ADDR> *>::const_iterator iter = cfi_rows.find(loc);
	
	return (iter != cfi_rows.end()) ? (*iter).second : 0;
}

template <class MEMORY_ADDR>
DWARF_CFIRow<MEMORY_ADDR> *DWARF_CFI<MEMORY_ADDR>::GetLowestRow(MEMORY_ADDR loc) const
{
	typename std::map<MEMORY_ADDR, DWARF_CFIRow<MEMORY_ADDR> *>::const_iterator iter;
	
	DWARF_CFIRow<MEMORY_ADDR> *prev_row = 0;
	for(iter = cfi_rows.begin(); iter != cfi_rows.end(); iter++)
	{
		MEMORY_ADDR cur_loc = (*iter).first;
		DWARF_CFIRow<MEMORY_ADDR> *cur_row = (*iter).second;
		
		if(loc < cur_loc) break;
		prev_row = cur_row;
	}
	return prev_row;
}

template <class MEMORY_ADDR>
DWARF_CFIRow<MEMORY_ADDR> *DWARF_CFI<MEMORY_ADDR>::GetInitialRow() const
{
	return initial_row;
}

template <class MEMORY_ADDR>
bool DWARF_CFI<MEMORY_ADDR>::HasRow(MEMORY_ADDR loc) const
{
	typename std::map<MEMORY_ADDR, DWARF_CFIRow<MEMORY_ADDR> *>::const_iterator iter = cfi_rows.find(loc);
	return (iter != cfi_rows.end());
}

template <class MEMORY_ADDR>
void DWARF_CFI<MEMORY_ADDR>::InsertRow(DWARF_CFIRow<MEMORY_ADDR> *cfi_row)
{
	typename std::map<MEMORY_ADDR, DWARF_CFIRow<MEMORY_ADDR> *>::iterator iter = cfi_rows.find(cfi_row->GetLocation());
	
	if(iter != cfi_rows.end())
	{
		DWARF_CFIRow<MEMORY_ADDR> *old_cfi_row = (*iter).second;
		cfi_rows.erase(iter);
		delete old_cfi_row;
	}
	
	if(cfi_row)
	{
		cfi_rows.insert(std::pair<MEMORY_ADDR, DWARF_CFIRow<MEMORY_ADDR> *>(cfi_row->GetLocation(), cfi_row));
	}
}

template <class MEMORY_ADDR>
void DWARF_CFI<MEMORY_ADDR>::CloneRow(MEMORY_ADDR cur_loc, MEMORY_ADDR new_loc)
{
	typename std::map<MEMORY_ADDR, DWARF_CFIRow<MEMORY_ADDR> *>::iterator iter = cfi_rows.find(cur_loc);
	
	if(iter != cfi_rows.end())
	{
		DWARF_CFIRow<MEMORY_ADDR> *new_row = new DWARF_CFIRow<MEMORY_ADDR>(new_loc, *(*iter).second);
		cfi_rows.insert(std::pair<MEMORY_ADDR, DWARF_CFIRow<MEMORY_ADDR> *>(new_loc, new_row));
	}
	else
	{
		DWARF_CFIRow<MEMORY_ADDR> *new_row = new DWARF_CFIRow<MEMORY_ADDR>(new_loc, *initial_row);
		cfi_rows.insert(std::pair<MEMORY_ADDR, DWARF_CFIRow<MEMORY_ADDR> *>(new_loc, new_row));
	}
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CFI<MEMORY_ADDR>& cfi)
{
	typename std::map<MEMORY_ADDR, DWARF_CFIRow<MEMORY_ADDR> *>::const_iterator iter;
	
	for(iter = cfi.cfi_rows.begin(); iter != cfi.cfi_rows.end(); iter++)
	{
		DWARF_CFIRow<MEMORY_ADDR> *row = (*iter).second;
		os << *row << std::endl;
	}
	return os;
}

template <class MEMORY_ADDR>
DWARF_CallFrameVM<MEMORY_ADDR>::DWARF_CallFrameVM(const DWARF_Handler<MEMORY_ADDR> *_dw_handler)
	: dw_handler(_dw_handler)
	, debug(false)
	, debug_info_stream(_dw_handler->GetDebugInfoStream())
	, debug_warning_stream(_dw_handler->GetDebugWarningStream())
	, debug_error_stream(_dw_handler->GetDebugErrorStream())
{
	dw_handler->GetOption(OPT_DEBUG, debug);
}

template <class MEMORY_ADDR>
DWARF_CallFrameVM<MEMORY_ADDR>::~DWARF_CallFrameVM()
{
	ResetState();
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::Disasm(std::ostream& os, const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog)
{
	return Run(dw_call_frame_prog, &os, 0, 0);
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::Execute(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, MEMORY_ADDR& location, DWARF_CFI<MEMORY_ADDR> *cfi)
{
	return Run(dw_call_frame_prog, 0, &location, cfi);
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::RememberState(DWARF_CFI<MEMORY_ADDR> *cfi, MEMORY_ADDR loc)
{
	DWARF_CFIRow<MEMORY_ADDR> *row = cfi->GetRow(loc);
	
	if(!row)
	{
		if(debug)
		{
			debug_error_stream << "no CFI row for PC 0x" << std::hex << loc << std::dec << std::endl;
		}
		return false;
	}

	DWARF_CFIRow<MEMORY_ADDR> *row_copy = new DWARF_CFIRow<MEMORY_ADDR>(*row);
	row_stack.push(row_copy);
	return true;
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::RestoreState(DWARF_CFI<MEMORY_ADDR> *cfi, MEMORY_ADDR loc)
{
	if(row_stack.empty())
	{
		if(debug)
		{
			debug_error_stream << "implicit stack is empty" << std::endl;
		}
		return false;
	}

	DWARF_CFIRow<MEMORY_ADDR> *row = row_stack.top();
	DWARF_CFIRow<MEMORY_ADDR> *row_copy = new DWARF_CFIRow<MEMORY_ADDR>(loc, *row);
	row_stack.pop();
	delete row;
	cfi->InsertRow(row_copy);
	return true;
}

template <class MEMORY_ADDR>
void DWARF_CallFrameVM<MEMORY_ADDR>::ResetState()
{
	while(!row_stack.empty())
	{
		DWARF_CFIRow<MEMORY_ADDR> *row = row_stack.top();
		row_stack.pop();
		delete row;
	}
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::Run(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, std::ostream *os, MEMORY_ADDR *_cur_location, DWARF_CFI<MEMORY_ADDR> *cfi)
{
	const DWARF_CIE<MEMORY_ADDR> *dw_cie = dw_call_frame_prog.GetCIE();
	if(!dw_cie)
	{
		if(debug)
		{
			debug_error_stream << "call frame program has no CIE" << std::endl;
		}
		return false;
	}
	
	unsigned int dw_cfp_type = dw_call_frame_prog.GetType();
	MEMORY_ADDR cur_location = _cur_location ? *_cur_location : 0;
	uint64_t program_length = dw_call_frame_prog.length;
	endian_type file_endianness = dw_call_frame_prog.GetHandler()->GetFileEndianness();
	uint8_t file_address_size = (dw_call_frame_prog.GetFormat() == FMT_DWARF64) ? 8 : 4;
	const uint8_t *program = dw_call_frame_prog.program;
	bool status = true;
	
	if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
	{
		if(cfi)
		{
			if(!cfi->HasRow(cur_location))
			{
				DWARF_CFIRow<MEMORY_ADDR> *cur_row = new DWARF_CFIRow<MEMORY_ADDR>(cur_location, *cfi->GetInitialRow());
				cfi->InsertRow(cur_row);
			}
		}
	}

	if(program_length)
	{
		do
		{
			uint8_t opcode = *program;
			program++;
			program_length--;
			
			MEMORY_ADDR new_location = cur_location;
			
			switch(opcode & 0xc0) // high 2 bits
			{
				case DW_CFA_advance_loc:
					{
						/* The DW_CFA_advance instruction takes a single operand (encoded with the opcode) that
						represents a constant delta. The required action is to create a new table row with a location
						value that is computed by taking the current entry’s location value and adding the value of
						delta * code_alignment_factor. All other values in the new row are initially identical
						to the current row. */

						uint8_t delta = opcode & 0x3f;
						
						uint64_t advance_loc = delta * (uint64_t) dw_cie->GetCodeAlignmentFactor();

						if(os) *os << "DW_CFA_advance_loc(" << advance_loc << ")";
						
						if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
						{
							new_location += advance_loc;
							
							if(cfi && (cur_location != new_location))
							{
								cfi->CloneRow(cur_location, new_location);
							}
						}
					}
					break;
				case DW_CFA_offset:
					{
						/* The DW_CFA_offset instruction takes two operands: a register number (encoded with the
						opcode) and an unsigned LEB128 constant representing a factored offset. The required action
						is to change the rule for the register indicated by the register number to be an offset(N) rule
						with a value of (N = factored offset * data_alignment_factor). */

						uint8_t reg_num = opcode & 0x3f;
						DWARF_LEB128 factored_offset;
						int64_t sz;
						
						if((sz = factored_offset.Load(program, program_length)) < 0)
						{
							if(debug)
							{
								debug_error_stream << "DW_CFA_offset: missing LEB128 unsigned factored offset operand" << std::endl;
							}
							status = false;
							break;
						}
						program += sz;
						program_length -= sz;

						int64_t n = (uint64_t) factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();

						if(os) *os << "DW_CFA_offset(" << (uint32_t) reg_num << "," << factored_offset.to_string(false) << ") /* r" << (uint32_t) reg_num << " = [ cfa" << (n >= 0 ? "+" : "") << n << "] */";

						if(cfi)
						{
							DWARF_CFIRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? cfi->GetInitialRow() : cfi->GetRow(cur_location);
							
							if(!cur_row)
							{
								if(debug)
								{
									debug_error_stream << "DW_CFA_offset: can't get current CFI row" << std::endl;
								}
								status = false;
								break;
							}
							
							DWARF_RegisterRuleOffset<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleOffset<MEMORY_ADDR>(n);
							
							cur_row->SetRegisterRule(reg_num, new_rule);
						}
					}
					break;
				case DW_CFA_restore:
					{
						/* The DW_CFA_restore instruction takes a single operand (encoded with the opcode) that
						represents a register number. The required action is to change the rule for the indicated
						register to the rule assigned it by the initial_instructions in the CIE. */
						uint8_t reg_num = opcode & 0x3f;
						
						if(os) *os << "DW_CFA_restore(" << (uint32_t) reg_num << ")";

						if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
						{
							if(cfi)
							{
								const DWARF_CFIRow<MEMORY_ADDR> *initial_row = cfi->GetInitialRow();
								
								if(!initial_row)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_restore: initial CFI row is missing" << std::endl;
									}
									status = false;
									break;
								}

								DWARF_CFIRow<MEMORY_ADDR> *cur_row = cfi->GetRow(cur_location);
								
								if(!cur_row)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_restore: can't get current CFI row" << std::endl;
									}
									status = false;
									break;
								}
								
								DWARF_RegisterRule<MEMORY_ADDR> *initial_reg_rule = initial_row->GetRegisterRule(reg_num);
								
								DWARF_RegisterRule<MEMORY_ADDR> *reg_rule_copy = initial_reg_rule ? initial_reg_rule->Clone() : new DWARF_RegisterRuleUndefined<MEMORY_ADDR>();
								
								cur_row->SetRegisterRule(reg_num, reg_rule_copy);
							}
						}
					}
					break;
				case 0x00:
					switch(opcode)
					{
						case DW_CFA_nop:
							/* The DW_CFA_nop instruction has no operands and no required actions. It is used as padding
							to make a CIE or FDE an appropriate size. */
							if(os) *os << "DW_CFA_nop /* padding */";
							break;
						case DW_CFA_set_loc:
							{
								/* The DW_CFA_set_loc instruction takes a single operand that represents an address. The
								required action is to create a new table row using the specified address as the location. All
								other values in the new row are initially identical to the current row. The new location value
								should always be greater than the current one. */

								MEMORY_ADDR addr;
								
								switch(file_address_size)
								{
									case sizeof(uint32_t):
										{
											if(program_length < sizeof(uint32_t))
											{
												if(debug)
												{
													debug_error_stream << "DW_CFA_set_loc: missing 4-byte address operand" << std::endl;
												}
												status = false;
												break;
											}
											uint32_t value;
											memcpy(&value, program, sizeof(uint32_t));
											addr = Target2Host(file_endianness, value);
											program += sizeof(uint32_t);
											program_length -= sizeof(uint32_t);
										}
										break;
									case sizeof(uint64_t):
										{
											if(program_length < sizeof(uint64_t))
											{
												if(debug)
												{
													debug_error_stream << "DW_CFA_set_loc: missing 8-byte address operand" << std::endl;
												}
												status = false;
												break;
											}
											uint64_t value;
											memcpy(&value, program, sizeof(uint64_t));
											addr = Target2Host(file_endianness, value);
											program += sizeof(uint64_t);
											program_length -= sizeof(uint64_t);
										}
										break;
									default:
										if(debug)
										{
											debug_error_stream << "DW_CFA_set_loc: unsupported address size" << std::endl;
										}
										status = false;
										break;
								}
								if(!status) break;
								if(os) *os << "DW_CFA_set_loc(0x" << std::hex << addr << std::dec << ")";
								if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
								{
									new_location = addr;
								}
							}
							break;
						case DW_CFA_advance_loc1:
							{
								/* The DW_CFA_advance_loc1 instruction takes a single ubyte operand that represents a
								constant delta. This instruction is identical to DW_CFA_advance_loc except for the encoding
								and size of the delta operand. */
								uint8_t delta;
								
								if(program_length < sizeof(delta))
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_advance_loc1: missing 1-byte delta operand" << std::endl;
									}
									status = false;
									break;
								}
								memcpy(&delta, program, sizeof(delta));
								delta = Target2Host(file_endianness, delta);
								program += sizeof(delta);
								program_length -= sizeof(delta);
								uint64_t advance_loc = delta * (uint64_t) dw_cie->GetCodeAlignmentFactor();
								
								if(os) *os << "DW_CFA_advance_loc1(" << advance_loc << ")";
								
								if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
								{
									new_location += advance_loc;
									if(cfi && (cur_location != new_location))
									{
										cfi->CloneRow(cur_location, new_location);
									}
								}
							}
							break;
						case DW_CFA_advance_loc2:
							{
								/* The DW_CFA_advance_loc2 instruction takes a single uhalf operand that represents a
								constant delta. This instruction is identical to DW_CFA_advance_loc except for the encoding
								and size of the delta operand. */
								uint16_t delta;
								
								if(program_length < sizeof(delta))
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_advance_loc2: missing 2-byte delta operand" << std::endl;
									}
									status = false;
									break;
								}
								memcpy(&delta, program, sizeof(delta));
								delta = Target2Host(file_endianness, delta);
								program += sizeof(delta);
								program_length -= sizeof(delta);
								uint64_t advance_loc = delta * (uint64_t) dw_cie->GetCodeAlignmentFactor();

								if(os) *os << "DW_CFA_advance_loc2(" << advance_loc << ")";

								if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
								{
									new_location += advance_loc;
									if(cfi && (cur_location != new_location))
									{
										cfi->CloneRow(cur_location, new_location);
									}
								}
							}
							break;
						case DW_CFA_advance_loc4:
							{
								/* The DW_CFA_advance_loc4 instruction takes a single uword operand that represents a
								constant delta. This instruction is identical to DW_CFA_advance_loc except for the encoding
								and size of the delta operand. */
								uint32_t delta;
								
								if(program_length < sizeof(delta))
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_advance_loc4: missing 4-byte delta operand" << std::endl;
									}
									status = false;
									break;
								}
								memcpy(&delta, program, sizeof(delta));
								delta = Target2Host(file_endianness, delta);
								program += sizeof(delta);
								program_length -= sizeof(delta);
								uint64_t advance_loc = delta * (uint64_t) dw_cie->GetCodeAlignmentFactor();

								if(os) *os << "DW_CFA_advance_loc4(" << advance_loc << ")";

								if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
								{
									new_location += advance_loc;
									if(cfi && (cur_location != new_location))
									{
										cfi->CloneRow(cur_location, new_location);
									}
								}
							}
							break;
						case DW_CFA_offset_extended:
							{
								/* The DW_CFA_offset_extended instruction takes two unsigned LEB128 operands
								representing a register number and a factored offset. This instruction is identical to
								DW_CFA_offset except for the encoding and size of the register operand. */
								DWARF_LEB128 reg_num;
								DWARF_LEB128 factored_offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_offset_extended: missing LEB128 register number operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								if((sz = factored_offset.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_offset_extended: missing LEB128 factored offset operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								int64_t n = (uint64_t) factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();

								if(os) *os << "DW_CFA_offset_extended(" << reg_num.to_string(false) << "," << factored_offset.to_string(false) << ") /* r" << reg_num.to_string(false) << " at cfa" << (n >= 0 ? "+" : "") << n << " */";

								if(cfi)
								{
									DWARF_CFIRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? cfi->GetInitialRow() : cfi->GetRow(cur_location);
									
									if(!cur_row)
									{
										if(debug)
										{
											debug_error_stream << "DW_CFA_offset_extended: can't get current CFI row" << std::endl;
										}
										status = false;
										break;
									}
									
									DWARF_RegisterRuleOffset<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleOffset<MEMORY_ADDR>(n);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
							}
							break;
						case DW_CFA_restore_extended:
							{
								/* The DW_CFA_restore_extended instruction takes a single unsigned LEB128 operand that
								represents a register number. This instruction is identical to DW_CFA_restore except for the
								encoding and size of the register operand. */
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_restore_extended: missing LEB128 register number operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								if(os) *os << "DW_CFA_restore_extended(" << reg_num.to_string(false) << ")";

								if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
								{
									if(cfi)
									{
										const DWARF_CFIRow<MEMORY_ADDR> *initial_row = cfi->GetInitialRow();
										
										if(!initial_row)
										{
											if(debug)
											{
												debug_error_stream << "DW_CFA_restore_extended: initial CFI row is missing" << std::endl;
											}
											status = false;
											break;
										}
										
										DWARF_CFIRow<MEMORY_ADDR> *cur_row = cfi->GetRow(cur_location);
										
										if(!cur_row)
										{
											if(debug)
											{
												debug_error_stream << "DW_CFA_restore_extended: can't get current CFI row" << std::endl;
											}
											status = false;
											break;
										}
										
										DWARF_RegisterRule<MEMORY_ADDR> *initial_reg_rule = initial_row->GetRegisterRule(reg_num);
										
										DWARF_RegisterRule<MEMORY_ADDR> *reg_rule_copy = initial_reg_rule ? initial_reg_rule->Clone() : new DWARF_RegisterRuleUndefined<MEMORY_ADDR>();
										
										cur_row->SetRegisterRule(reg_num, reg_rule_copy);
									}
								}
							}
							break;
						case DW_CFA_undefined:
							{
								/* The DW_CFA_undefined instruction takes a single unsigned LEB128 operand that
								represents a register number. The required action is to set the rule for the specified register to
								"undefined." */
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_undefined: missing LEB128 register number operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								if(os) *os << "DW_CFA_undefined(" << reg_num.to_string(false) << ") /* r" << reg_num.to_string(false) << " not preserved */";

								if(cfi)
								{
									DWARF_CFIRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? cfi->GetInitialRow() : cfi->GetRow(cur_location);
									
									if(!cur_row)
									{
										if(debug)
										{
											debug_error_stream << "DW_CFA_undefined: can't get current CFI row" << std::endl;
										}
										status = false;
										break;
									}
									
									DWARF_RegisterRuleUndefined<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleUndefined<MEMORY_ADDR>();
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
							}
							break;
						case DW_CFA_same_value:
							{
								/* The DW_CFA_same_value instruction takes a single unsigned LEB128 operand that
								represents a register number. The required action is to set the rule for the specified register to
								"same value." */
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_same_value: missing LEB128 register number operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								if(os) *os << "DW_CFA_same_value(" << reg_num.to_string(false) << ") /* r" << reg_num.to_string(false) << " preserved */";

								if(cfi)
								{
									DWARF_CFIRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? cfi->GetInitialRow() : cfi->GetRow(cur_location);
									
									if(!cur_row)
									{
										if(debug)
										{
											debug_error_stream << "DW_CFA_same_value: can't get current CFI row" << std::endl;
										}
										status = false;
										break;
									}
									
									DWARF_RegisterRuleSameValue<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleSameValue<MEMORY_ADDR>();
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
							}
							break;
						case DW_CFA_register:
							{
								/* The DW_CFA_register instruction takes two unsigned LEB128 operands representing
								register numbers. The required action is to set the rule for the first register to be the second
								register. */
								DWARF_LEB128 reg_num1;
								DWARF_LEB128 reg_num2;
								int64_t sz;
								
								if((sz = reg_num1.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_register: missing first LEB128 unsigned factored offset operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								if((sz = reg_num2.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_register: missing second LEB128 unsigned factored offset operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;

								if(os) *os << "DW_CFA_register(" << reg_num1.to_string(false) << ", " << reg_num2.to_string(false) << ") /* r" << reg_num1.to_string(false) << "=r" << reg_num2.to_string(false) << " */";
								
								if(cfi)
								{
									DWARF_CFIRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? cfi->GetInitialRow() : cfi->GetRow(cur_location);
									
									if(!cur_row)
									{
										if(debug)
										{
											debug_error_stream << "DW_CFA_register: can't get current CFI row" << std::endl;
										}
										status = false;
										break;
									}
									
									DWARF_RegisterRuleRegister<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleRegister<MEMORY_ADDR>((unsigned int) reg_num2);
									cur_row->SetRegisterRule(reg_num1, new_rule);
								}
							}
							break;
						case DW_CFA_remember_state:
							/* The DW_CFA_remember_state instruction takes no operands. The required action is to push
							the set of rules for every register onto an implicit stack. */
							if(os) *os << "DW_CFA_remember_state";
							if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
							{
								if(cfi)
								{
									if(!RememberState(cfi, cur_location))
									{
										if(debug)
										{
											debug_error_stream << "DW_CFA_remember_state: operation failed" << std::endl;
										}
										status = false;
										break;
									}
								}
							}
							break;
						case DW_CFA_restore_state:
							/* The DW_CFA_restore_state instruction takes no operands. The required action is to pop the
							set of rules off the implicit stack and place them in the current row. */
							if(os) *os << "DW_CFA_restore_state";
							if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
							{
								if(cfi)
								{
									if(!RestoreState(cfi, cur_location))
									{
										if(debug)
										{
											debug_error_stream << "DW_CFA_restore_state: operation failed" << std::endl;
										}
										status = false;
										break;
									}
								}
							}
							break;
						case DW_CFA_def_cfa:
							{
								/* The DW_CFA_def_cfa instruction takes two unsigned LEB128 operands representing a
								register number and a (non-factored) offset. The required action is to define the current CFA
								rule to use the provided register and offset. */
								DWARF_LEB128 reg_num;
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_def_cfa: missing LEB128 register number operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								if((sz = offset.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_def_cfa: missing LEB128 non-factored offset operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								if(os) *os << "DW_CFA_def_cfa(" << reg_num.to_string(false) << ", " << offset.to_string(false) << ") /* cfa = r" << reg_num.to_string(false) << "+" << offset.to_string(false) << " */";
								
								if(cfi)
								{
									DWARF_CFIRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? cfi->GetInitialRow() : cfi->GetRow(cur_location);
									
									if((dw_cfp_type == DW_CFP_INSTRUCTIONS) && !cur_row)
									{
										cur_row = new DWARF_CFIRow<MEMORY_ADDR>(cur_location, *cfi->GetInitialRow());
										cfi->InsertRow(cur_row);
									}
									
									DWARF_CFARule<MEMORY_ADDR> *new_cfa_rule = new DWARF_CFARuleRegisterOffset<MEMORY_ADDR>(reg_num, offset);
									cur_row->SetCFARule(new_cfa_rule);
								}
							}
							break;
						case DW_CFA_def_cfa_register:
							{
								/* The DW_CFA_def_cfa_register instruction takes a single unsigned LEB128 operand
								representing a register number. The required action is to define the current CFA rule to use
								the provided register (but to keep the old offset). */
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_def_cfa_register: missing LEB128 register number operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								if(os) *os << "DW_CFA_def_cfa_register(" << reg_num.to_string(false) << ") /* cfa = r" << reg_num.to_string(false) << "+ old offset */";
								
								if(cfi)
								{
									DWARF_CFIRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? cfi->GetInitialRow() : cfi->GetRow(cur_location);
									
									if((dw_cfp_type == DW_CFP_INSTRUCTIONS) && !cur_row)
									{
										cur_row = new DWARF_CFIRow<MEMORY_ADDR>(cur_location, *cfi->GetInitialRow());
										cfi->InsertRow(cur_row);
									}
									
									DWARF_CFARule<MEMORY_ADDR> *cur_cfa_rule = cur_row->GetCFARule();
									
									if(!cur_cfa_rule)
									{
										cur_cfa_rule = new DWARF_CFARuleRegisterOffset<MEMORY_ADDR>(reg_num, 0);
										cur_row->SetCFARule(cur_cfa_rule);
									}
									
									if(cur_cfa_rule->GetType() != DW_CFA_RULE_REGISTER_OFFSET)
									{
										if(debug)
										{
											debug_error_stream << "DW_CFA_def_cfa_register: invalid operation because current CFA rule is not defined to use a register and offset" << std::endl;
										}
										status = false;
										break;
									}

									reinterpret_cast<DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *>(cur_cfa_rule)->SetRegisterNumber(reg_num);
								}
							}
							break;
						case DW_CFA_def_cfa_offset:
							{
								/* The DW_CFA_def_cfa_offset instruction takes a single unsigned LEB128 operand
								representing a (non-factored) offset. The required action is to define the current CFA rule to
								use the provided offset (but to keep the old register). */
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = offset.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_def_cfa_offset: missing LEB128 non-factored offset operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								if(os) *os << "DW_CFA_def_cfa_offset(" << offset.to_string(false) << ") /* cfa = old register + " << offset.to_string(false) << "*/";
								
								if(cfi)
								{
									DWARF_CFIRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? cfi->GetInitialRow() : cfi->GetRow(cur_location);
									
									if((dw_cfp_type == DW_CFP_INSTRUCTIONS) && !cur_row)
									{
										cur_row = new DWARF_CFIRow<MEMORY_ADDR>(cur_location, *cfi->GetInitialRow());
										cfi->InsertRow(cur_row);
									}
									
									DWARF_CFARule<MEMORY_ADDR> *cur_cfa_rule = cur_row->GetCFARule();
									
									if(!cur_cfa_rule || (cur_cfa_rule->GetType() != DW_CFA_RULE_REGISTER_OFFSET))
									{
										if(debug)
										{
											debug_error_stream << "DW_CFA_def_cfa_offset: invalid operation because current CFA rule is not defined to use a register and offset" << std::endl;
										}
										status = false;
										break;
									}
									
									reinterpret_cast<DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *>(cur_cfa_rule)->SetOffset((uint64_t) offset);
								}
							}
							break;
						case DW_CFA_def_cfa_expression:
							{
								/* The DW_CFA_def_cfa_expression instruction takes a single operand encoded as a
								DW_FORM_block value representing a DWARF expression. The required action is to
								establish that expression as the means by which the current CFA is computed.
								The DW_OP_call2, DW_OP_call4, DW_OP_call_ref DW_OP_push_object_address and
								DW_OP_call_frame_cfa DWARF operators (see Section 2.5.1) cannot be used in such a
								DWARF expression. */
								DWARF_LEB128 leb128_block_length;
								int64_t sz;
								if((sz = leb128_block_length.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_def_cfa_expression: missing or truncated DWARF expression (LEB128 block length)" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;

								uint64_t block_length = (uint64_t) leb128_block_length;
								if(program_length < block_length)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_def_cfa_expression: missing or truncated DWARF expression" << std::endl;
									}
									status = false;
									break;
								}

								DWARF_Expression<MEMORY_ADDR> *dw_expr = new DWARF_Expression<MEMORY_ADDR>(&dw_call_frame_prog, block_length, program);
								program += block_length;
								program_length -= block_length;
								
								if(os) *os << "DW_CFA_def_cfa_expression({" << dw_expr->to_string() << "}) /* cfa = expr */";
								
								if(cfi)
								{
									DWARF_CFIRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? cfi->GetInitialRow() : cfi->GetRow(cur_location);
									
									if((dw_cfp_type == DW_CFP_INSTRUCTIONS) && !cur_row)
									{
										cur_row = new DWARF_CFIRow<MEMORY_ADDR>(cur_location, *cfi->GetInitialRow());
										cfi->InsertRow(cur_row);
									}
									
									DWARF_CFARule<MEMORY_ADDR> *new_cfa_rule = new DWARF_CFARuleExpression<MEMORY_ADDR>(dw_expr);
									
									cur_row->SetCFARule(new_cfa_rule);
								}
								if(!cfi) delete dw_expr;
							}
							break;
						case DW_CFA_expression:
							{
								/* The DW_CFA_expression instruction takes two operands: an unsigned LEB128 value
								representing a register number, and a DW_FORM_block value representing a DWARF
								expression. The required action is to change the rule for the register indicated by the register
								number to be an expression(E) rule where E is the DWARF expression. That is, the DWARF
								expression computes the address. The value of the CFA is pushed on the DWARF evaluation
								stack prior to execution of the DWARF expression.
								The DW_OP_call2, DW_OP_call4, DW_OP_call_ref, DW_OP_push_object_address and
								DW_OP_call_frame_cfa DWARF operators (see Section 2.5.1) cannot be used in such a
								DWARF expression. */
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_expression: missing LEB128 register number operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;

								DWARF_LEB128 leb128_block_length;
								if((sz = leb128_block_length.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_expression: missing or truncated DWARF expression (LEB128 block length)" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;

								uint64_t block_length = (uint64_t) leb128_block_length;
								if(program_length < block_length)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_expression: missing or truncated DWARF expression" << std::endl;
									}
									status = false;
									break;
								}

								DWARF_Expression<MEMORY_ADDR> *dw_expr = new DWARF_Expression<MEMORY_ADDR>(&dw_call_frame_prog, block_length, program);
								program += block_length;
								program_length -= block_length;
								
								if(os) *os << "DW_CFA_expression(" << reg_num.to_string(false) << ", {" << dw_expr->to_string() << "}) /* r" << reg_num.to_string(false) << " = [expr] */";
								
								if(cfi)
								{
									DWARF_CFIRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? cfi->GetInitialRow() : cfi->GetRow(cur_location);
									
									if(!cur_row)
									{
										if(debug)
										{
											debug_error_stream << "DW_CFA_expression: can't get current CFI row" << std::endl;
										}
										status = false;
										break;
									}
									
									DWARF_RegisterRuleExpression<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleExpression<MEMORY_ADDR>(dw_expr);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
								
								if(!cfi) delete dw_expr;
							}
							break;
						case DW_CFA_offset_extended_sf:
							{
								/* The DW_CFA_offset_extended_sf instruction takes two operands: an unsigned LEB128
								value representing a register number and a signed LEB128 factored offset. This instruction is
								identical to DW_CFA_offset_extended except that the second operand is signed and
								factored. */
								DWARF_LEB128 reg_num;
								DWARF_LEB128 signed_factored_offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_offset_extended_sf: missing LEB128 register number operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								if((sz = signed_factored_offset.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_offset_extended_sf: missing LEB128 signed factored offset operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								int64_t n = (int64_t) signed_factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();
								
								if(os) *os << "DW_CFA_offset_extended_sf(" << reg_num.to_string(false) << "," << signed_factored_offset.to_string(true) << ") /* r" << reg_num.to_string(false) << " = [cfa" << (n >= 0 ? "+" : "") << n << "] */";
								
								if(cfi)
								{
									DWARF_CFIRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? cfi->GetInitialRow() : cfi->GetRow(cur_location);
									
									if(!cur_row)
									{
										if(debug)
										{
											debug_error_stream << "DW_CFA_offset_extended_sf: can't get current CFI row" << std::endl;
										}
										status = false;
										break;
									}
									
									DWARF_RegisterRuleOffset<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleOffset<MEMORY_ADDR>(n);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
							}
							break;
						case DW_CFA_def_cfa_sf:
							{
								/* The DW_CFA_def_cfa_sf instruction takes two operands: an unsigned LEB128 value
								representing a register number and a signed LEB128 factored offset. This instruction is
								identical to DW_CFA_def_cfa except that the second operand is signed and factored. */
								DWARF_LEB128 reg_num;
								DWARF_LEB128 signed_factored_offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_def_cfa_sf: missing LEB128 register number operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								if((sz = signed_factored_offset.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_def_cfa_sf: missing LEB128 signed factored offset operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								int64_t n = (int64_t) signed_factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();
								if(os) *os << "DW_CFA_def_cfa_sf(" << reg_num.to_string(false) << ", " << signed_factored_offset.to_string(true) << ") /* cfa = " << reg_num.to_string(false) << (n >= 0 ? "+" : "") << n << " */";

								if(cfi)
								{
									DWARF_CFIRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? cfi->GetInitialRow() : cfi->GetRow(cur_location);
									
									if((dw_cfp_type == DW_CFP_INSTRUCTIONS) && !cur_row)
									{
										cur_row = new DWARF_CFIRow<MEMORY_ADDR>(cur_location, *cfi->GetInitialRow());
										cfi->InsertRow(cur_row);
									}
									
									DWARF_CFARule<MEMORY_ADDR> *new_cfa_rule = new DWARF_CFARuleRegisterOffset<MEMORY_ADDR>(reg_num, n);
									cur_row->SetCFARule(new_cfa_rule);
								}
							}
							break;
						case DW_CFA_def_cfa_offset_sf:
							{
								/* The DW_CFA_def_cfa_offset_sf instruction takes a signed LEB128 operand representing a
								factored offset. This instruction is identical to DW_CFA_def_cfa_offset except that the
								operand is signed and factored. */
								DWARF_LEB128 signed_factored_offset;
								int64_t sz;
								
								if((sz = signed_factored_offset.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_def_cfa_offset_sf: missing LEB128 signed factored offset operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								int64_t n = (int64_t) signed_factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();
								if(os) *os << "DW_CFA_def_cfa_offset_sf(" << signed_factored_offset.to_string(true) << ") /* cfa = old register + " << (n > 0 ? "+":"") << n << " */";

								if(cfi)
								{
									DWARF_CFIRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? cfi->GetInitialRow() : cfi->GetRow(cur_location);
									
									if((dw_cfp_type == DW_CFP_INSTRUCTIONS) && !cur_row)
									{
										cur_row = new DWARF_CFIRow<MEMORY_ADDR>(cur_location, *cfi->GetInitialRow());
										cfi->InsertRow(cur_row);
									}
									
									DWARF_CFARule<MEMORY_ADDR> *cur_cfa_rule = cur_row->GetCFARule();
									
									if(!cur_cfa_rule || (cur_cfa_rule->GetType() != DW_CFA_RULE_REGISTER_OFFSET))
									{
										if(debug)
										{
											debug_error_stream << "DW_CFA_def_cfa_offset_sf: invalid operation because current CFA rule is not defined to use a register and offset" << std::endl;
										}
										status = false;
										break;
									}
									
									reinterpret_cast<DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *>(cur_cfa_rule)->SetOffset(n);
								}
							}
							break;
						case DW_CFA_val_offset:
							{
								/* The DW_CFA_val_offset instruction takes two unsigned LEB128 operands representing a
								register number and a factored offset. The required action is to change the rule for the
								register indicated by the register number to be a val_offset(N) rule with a value of (N =
								factored offset * data_alignment_factor). */
								DWARF_LEB128 reg_num;
								DWARF_LEB128 factored_offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_val_offset: missing LEB128 register number operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								if((sz = factored_offset.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_val_offset: missing LEB128 factored offset operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								int64_t n = (uint64_t) factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();
								if(os) *os << "DW_CFA_val_offset(" << reg_num.to_string(false) << "," << factored_offset.to_string(false) << ") /* r" << reg_num.to_string(false) << " = cfa" << (n >= 0 ? "+" : "") << n << " */";

								if(cfi)
								{
									DWARF_CFIRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? cfi->GetInitialRow() : cfi->GetRow(cur_location);
									
									if(!cur_row)
									{
										if(debug)
										{
											debug_error_stream << "DW_CFA_val_offset: can't get current CFI row" << std::endl;
										}
										status = false;
										break;
									}
									
									DWARF_RegisterRuleValOffset<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleValOffset<MEMORY_ADDR>(n);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
							}
							break;
						case DW_CFA_val_offset_sf:
							{
								/* The DW_CFA_val_offset_sf instruction takes two operands: an unsigned LEB128 value
								representing a register number and a signed LEB128 factored offset. This instruction is
								identical to DW_CFA_val_offset except that the second operand is signed and factored. */
								DWARF_LEB128 reg_num;
								DWARF_LEB128 signed_factored_offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_val_offset_sf: missing LEB128 register number operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								if((sz = signed_factored_offset.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_val_offset_sf: missing LEB128 signed factored offset operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								int64_t n = (int64_t) signed_factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();

								if(os) *os << "DW_CFA_val_offset_sf(" << reg_num.to_string(false) << "," << signed_factored_offset.to_string(true) << ") /* r" << reg_num.to_string(false) << " = cfa" << (n >= 0 ? "+" : "") << n << " */";

								if(cfi)
								{
									DWARF_CFIRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? cfi->GetInitialRow() : cfi->GetRow(cur_location);
									
									if(!cur_row)
									{
										if(debug)
										{
											debug_error_stream << "DW_CFA_val_offset_sf: can't get current CFI row" << std::endl;
										}
										status = false;
										break;
									}
									
									DWARF_RegisterRuleValOffset<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleValOffset<MEMORY_ADDR>(n);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
							}
							break;
						case DW_CFA_val_expression:
							{
								/* The DW_CFA_val_expression instruction takes two operands: an unsigned LEB128 value
								representing a register number, and a DW_FORM_block value representing a DWARF
								expression. The required action is to change the rule for the register indicated by the register
								number to be a val_expression(E) rule where E is the DWARF expression. That is, the
								DWARF expression computes the value of the given register. The value of the CFA is
								pushed on the DWARF evaluation stack prior to execution of the DWARF expression.
								The DW_OP_call2, DW_OP_call4, DW_OP_call_ref and DW_OP_push_object_address
								DWARF operators (see Section 2.5.1) cannot be used in such a DWARF expression. */
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_val_expression: missing LEB128 register number operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;

								DWARF_LEB128 leb128_block_length;
								if((sz = leb128_block_length.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_val_expression: missing or truncated DWARF expression (LEB128 block length)" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;

								uint64_t block_length = (uint64_t) leb128_block_length;
								if(program_length < block_length)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_val_expression: missing or truncated DWARF expression" << std::endl;
									}
									status = false;
									break;
								}

								DWARF_Expression<MEMORY_ADDR> *dw_expr = new DWARF_Expression<MEMORY_ADDR>(&dw_call_frame_prog, block_length, program);
								program += block_length;
								program_length -= block_length;
								
								if(os) *os << "DW_CFA_val_expression(" << reg_num.to_string(false) << ", {" << dw_expr->to_string() << "})";

								if(cfi)
								{
									DWARF_CFIRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? cfi->GetInitialRow() : cfi->GetRow(cur_location);
									
									if(!cur_row)
									{
										if(debug)
										{
											debug_error_stream << "DW_CFA_val_expression: can't get current CFI row" << std::endl;
										}
										status = false;
										break;
									}
									
									DWARF_RegisterRuleExpression<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleExpression<MEMORY_ADDR>(dw_expr);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
								
								if(!cfi) delete dw_expr;
							}
							break;
						
						case DW_CFA_GNU_args_size:
							{
								DWARF_LEB128 argument_size;
								int64_t sz;
								
								if((sz = argument_size.Load(program, program_length)) < 0)
								{
									if(debug)
									{
										debug_error_stream << "DW_CFA_GNU_args_size: missing LEB128 argument size operand" << std::endl;
									}
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								// ignored
								
								if(os) *os << "DW_CFA_GNU_args_size(" << argument_size.to_string(false) << ")";
							}
							break;
							
						default:
							if(debug)
							{
								debug_error_stream << "invalid call frame instruction 0x" << std::hex << (unsigned int) opcode << std::dec << std::endl;
							}
							status = false;
							break;
					}
					break;
				default:
					if(debug)
					{
						debug_error_stream << "invalid call frame instruction 0x" << std::hex << (unsigned int) opcode << std::dec << std::endl;
					}
					status = false;
					break;
			}
			
			cur_location = new_location;
			if(os && program_length) *os << std::endl;
		}
		while(status && program_length);
	}

	return status;
}

template <class MEMORY_ADDR>
const DWARF_CFI<MEMORY_ADDR> *DWARF_CallFrameVM<MEMORY_ADDR>::ComputeCFI(const DWARF_FDE<MEMORY_ADDR> *dw_fde)
{
	MEMORY_ADDR initial_location = dw_fde->GetInitialLocation();
	const DWARF_CIE<MEMORY_ADDR> *dw_cie = dw_fde->GetCIE();
		
	DWARF_CFI<MEMORY_ADDR> *cfi = new DWARF_CFI<MEMORY_ADDR>();
			
	const DWARF_CallFrameProgram<MEMORY_ADDR> *initial_instructions = dw_cie->GetInitialInstructions();
			
	MEMORY_ADDR location = 0;
	
	if(!Execute(*initial_instructions, location, cfi))
	{
		delete cfi;
		return 0;
	}
	location = initial_location;
	const DWARF_CallFrameProgram<MEMORY_ADDR> *instructions = dw_fde->GetInstructions();
	if(!Execute(*instructions, location, cfi))
	{
		delete cfi;
		return 0;
	}
	return cfi;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_CALL_FRAME_VM_TCC__
