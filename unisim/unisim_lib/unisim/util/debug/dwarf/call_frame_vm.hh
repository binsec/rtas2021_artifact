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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_CALL_FRAME_VM_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_CALL_FRAME_VM_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <map>
#include <vector>
#include <stack>
#include <set>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

const unsigned int DW_CFA_RULE_REGISTER_OFFSET = 0;
const unsigned int DW_CFA_RULE_EXPRESSION = 1;
	
const unsigned int DW_REG_RULE_UNDEFINED = 0;
const unsigned int DW_REG_RULE_SAME_VALUE = 1;
const unsigned int DW_REG_RULE_OFFSET = 2;
const unsigned int DW_REG_RULE_VAL_OFFSET = 3;
const unsigned int DW_REG_RULE_REGISTER = 4;
const unsigned int DW_REG_RULE_EXPRESSION = 5;
const unsigned int DW_REG_RULE_VAL_EXPRESSION = 6;

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CFARule<MEMORY_ADDR>& cfa_rule);

template <class MEMORY_ADDR>
class DWARF_CFARule
{
public:
	DWARF_CFARule(unsigned int type);
	virtual ~DWARF_CFARule();
	unsigned int GetType() const;
	DWARF_CFARule<MEMORY_ADDR> *Clone();
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_CFARule<MEMORY_ADDR>& cfa_rule);
protected:
	virtual std::ostream& Print(std::ostream& os) const = 0;
private:
	unsigned int type;
};

template <class MEMORY_ADDR>
class DWARF_CFARuleRegisterOffset : public DWARF_CFARule<MEMORY_ADDR>
{
public:
	DWARF_CFARuleRegisterOffset(unsigned int dw_reg_num, int64_t dw_offset);
	DWARF_CFARuleRegisterOffset(const DWARF_CFARuleRegisterOffset<MEMORY_ADDR>& cfa_rule_offset);
	virtual ~DWARF_CFARuleRegisterOffset();
	void SetRegisterNumber(unsigned int dw_reg_num);
	void SetOffset(int64_t dw_offset);
	unsigned int GetRegisterNumber() const;
	int64_t GetOffset() const;
protected:
	virtual std::ostream& Print(std::ostream& os) const;
private:
	unsigned int dw_reg_num;
	int64_t dw_offset;
};

template <class MEMORY_ADDR>
class DWARF_CFARuleExpression : public DWARF_CFARule<MEMORY_ADDR>
{
public:
	DWARF_CFARuleExpression(const DWARF_Expression<MEMORY_ADDR> *dw_expr);
	DWARF_CFARuleExpression(const DWARF_CFARuleExpression<MEMORY_ADDR>& cfa_rule_expression);
	virtual ~DWARF_CFARuleExpression();
	const DWARF_Expression<MEMORY_ADDR> *GetExpression() const;
protected:
	virtual std::ostream& Print(std::ostream& os) const;
private:
	const DWARF_Expression<MEMORY_ADDR> *dw_expr;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_RegisterRule<MEMORY_ADDR>& register_rule);

template <class MEMORY_ADDR>
class DWARF_RegisterRule
{
public:
	DWARF_RegisterRule(unsigned int dw_rule_type);
	virtual ~DWARF_RegisterRule();
	unsigned int GetType() const;
	DWARF_RegisterRule<MEMORY_ADDR> *Clone();
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_RegisterRule<MEMORY_ADDR>& register_rule);
protected:
	virtual std::ostream& Print(std::ostream& os) const = 0;
private:
	unsigned int dw_rule_type;
};

template <class MEMORY_ADDR>
class DWARF_RegisterRuleUndefined : public DWARF_RegisterRule<MEMORY_ADDR>
{
public:
	DWARF_RegisterRuleUndefined(const DWARF_RegisterRuleUndefined<MEMORY_ADDR>& rule);
	DWARF_RegisterRuleUndefined();
	virtual ~DWARF_RegisterRuleUndefined();
protected:
	virtual std::ostream& Print(std::ostream& os) const;
};

template <class MEMORY_ADDR>
class DWARF_RegisterRuleSameValue : public DWARF_RegisterRule<MEMORY_ADDR>
{
public:
	DWARF_RegisterRuleSameValue(const DWARF_RegisterRuleSameValue<MEMORY_ADDR>& rule);
	DWARF_RegisterRuleSameValue();
	virtual ~DWARF_RegisterRuleSameValue();
protected:
	virtual std::ostream& Print(std::ostream& os) const;
};

template <class MEMORY_ADDR>
class DWARF_RegisterRuleOffset : public DWARF_RegisterRule<MEMORY_ADDR>
{
public:
	DWARF_RegisterRuleOffset(const DWARF_RegisterRuleOffset<MEMORY_ADDR>& rule);
	DWARF_RegisterRuleOffset(int64_t dw_offset);
	virtual ~DWARF_RegisterRuleOffset();
	int64_t GetOffset() const;
protected:
	virtual std::ostream& Print(std::ostream& os) const;
private:
	int64_t dw_offset;
};

template <class MEMORY_ADDR>
class DWARF_RegisterRuleValOffset : public DWARF_RegisterRule<MEMORY_ADDR>
{
public:
	DWARF_RegisterRuleValOffset(const DWARF_RegisterRuleValOffset<MEMORY_ADDR>& rule);
	DWARF_RegisterRuleValOffset(int64_t dw_offset);
	virtual ~DWARF_RegisterRuleValOffset();
	int64_t GetOffset() const;
protected:
	virtual std::ostream& Print(std::ostream& os) const;
private:
	int64_t dw_offset;
};

template <class MEMORY_ADDR>
class DWARF_RegisterRuleRegister : public DWARF_RegisterRule<MEMORY_ADDR>
{
public:
	DWARF_RegisterRuleRegister(const DWARF_RegisterRuleRegister<MEMORY_ADDR>& rule);
	DWARF_RegisterRuleRegister(unsigned int dw_reg_num);
	virtual ~DWARF_RegisterRuleRegister();
	unsigned int GetRegisterNumber() const;
protected:
	virtual std::ostream& Print(std::ostream& os) const;
private:
	unsigned int dw_reg_num;
};

template <class MEMORY_ADDR>
class DWARF_RegisterRuleExpression : public DWARF_RegisterRule<MEMORY_ADDR>
{
public:
	DWARF_RegisterRuleExpression(const DWARF_RegisterRuleExpression<MEMORY_ADDR>& rule);
	DWARF_RegisterRuleExpression(const DWARF_Expression<MEMORY_ADDR> *dw_expr);
	virtual ~DWARF_RegisterRuleExpression();
	const DWARF_Expression<MEMORY_ADDR> *GetExpression() const;
protected:
	virtual std::ostream& Print(std::ostream& os) const;
private:
	const DWARF_Expression<MEMORY_ADDR> *dw_expr;
};

template <class MEMORY_ADDR>
class DWARF_RegisterRuleValExpression : public DWARF_RegisterRule<MEMORY_ADDR>
{
public:
	DWARF_RegisterRuleValExpression(const DWARF_RegisterRuleValExpression<MEMORY_ADDR>& rule);
	DWARF_RegisterRuleValExpression(const DWARF_Expression<MEMORY_ADDR> *dw_expr);
	virtual ~DWARF_RegisterRuleValExpression();
	const DWARF_Expression<MEMORY_ADDR> *GetExpression() const;
protected:
	virtual std::ostream& Print(std::ostream& os) const;
private:
	const DWARF_Expression<MEMORY_ADDR> *dw_expr;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CFIRow<MEMORY_ADDR>& cfi_row);

template <class MEMORY_ADDR>
class DWARF_CFIRow
{
public:
	DWARF_CFIRow();
	DWARF_CFIRow(MEMORY_ADDR location);
	DWARF_CFIRow(const DWARF_CFIRow<MEMORY_ADDR>& cfi_row);
	DWARF_CFIRow(MEMORY_ADDR location, const DWARF_CFIRow<MEMORY_ADDR>& cfi_row);
	~DWARF_CFIRow();
	void SetCFARule(DWARF_CFARule<MEMORY_ADDR> *cfa_rule);
	void SetRegisterRule(unsigned int reg_num, DWARF_RegisterRule<MEMORY_ADDR> *reg_rule);
	void RestoreRegisterRule(unsigned int reg_num);
	MEMORY_ADDR GetLocation() const;
	DWARF_CFARule<MEMORY_ADDR> *GetCFARule() const;
	DWARF_RegisterRule<MEMORY_ADDR> *GetRegisterRule(unsigned int reg_num) const;
	void GetRegisterRulesNumbers(std::set<unsigned int>& reg_rule_nums) const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_CFIRow<MEMORY_ADDR>& cfi_row);
private:
	MEMORY_ADDR location;
	DWARF_CFARule<MEMORY_ADDR> *cfa_rule;
	std::map<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *> reg_rules;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CFI<MEMORY_ADDR>& cfi);

template <class MEMORY_ADDR>
class DWARF_CFI
{
public:
	DWARF_CFI();
	DWARF_CFI(const DWARF_CFI<MEMORY_ADDR>& cfi);
	~DWARF_CFI();
	
	DWARF_CFIRow<MEMORY_ADDR> *operator[](MEMORY_ADDR loc) const;
	DWARF_CFIRow<MEMORY_ADDR> *GetRow(MEMORY_ADDR loc) const;
	DWARF_CFIRow<MEMORY_ADDR> *GetLowestRow(MEMORY_ADDR loc) const;
	DWARF_CFIRow<MEMORY_ADDR> *GetInitialRow() const;
	bool HasRow(MEMORY_ADDR loc) const;
	void InsertRow(DWARF_CFIRow<MEMORY_ADDR> *cfi_row);
	void CloneRow(MEMORY_ADDR cur_loc, MEMORY_ADDR new_loc);
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_CFI<MEMORY_ADDR>& cfi);
private:
	DWARF_CFIRow<MEMORY_ADDR> *initial_row;
	std::map<MEMORY_ADDR, DWARF_CFIRow<MEMORY_ADDR> *> cfi_rows;
};

template <class MEMORY_ADDR>
class DWARF_CallFrameVM
{
public:
	DWARF_CallFrameVM(const DWARF_Handler<MEMORY_ADDR> *dw_handler);
	~DWARF_CallFrameVM();
	bool Disasm(std::ostream& os, const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog);
	bool Execute(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, MEMORY_ADDR& location, DWARF_CFI<MEMORY_ADDR> *cfi);
	const DWARF_CFI<MEMORY_ADDR> *ComputeCFI(const DWARF_FDE<MEMORY_ADDR> *dw_fde);
private:
	const DWARF_Handler<MEMORY_ADDR> *dw_handler;
	bool debug;
	std::ostream& debug_info_stream;
	std::ostream& debug_warning_stream;
	std::ostream& debug_error_stream;
	
	bool RememberState(DWARF_CFI<MEMORY_ADDR> *cfi, MEMORY_ADDR loc);
	bool RestoreState(DWARF_CFI<MEMORY_ADDR> *cfi, MEMORY_ADDR loc);
	void ResetState();
	bool Run(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, std::ostream *os, MEMORY_ADDR *location, DWARF_CFI<MEMORY_ADDR> *cfi);
	
	std::stack<DWARF_CFIRow<MEMORY_ADDR> *> row_stack;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
