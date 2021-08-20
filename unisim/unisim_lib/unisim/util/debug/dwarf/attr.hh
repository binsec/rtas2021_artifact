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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_ATTR_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_ATTR_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/dwarf/leb128.hh>
#include <unisim/util/debug/dwarf/class.hh>
#include <inttypes.h>
#include <iosfwd>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_AttributeValue<MEMORY_ADDR>& dw_value);

template <class MEMORY_ADDR>
class DWARF_AttributeValue
{
public:
	DWARF_AttributeValue(unsigned int dw_class);
	virtual ~DWARF_AttributeValue();
	unsigned int GetClass() const;
	const char *GetClassName() const;
	virtual std::string to_string() const = 0;
	virtual void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_AttributeValue& dw_value);
protected:
	unsigned int dw_class;
};

template <class MEMORY_ADDR>
class DWARF_Address : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_Address(MEMORY_ADDR addr);
	virtual ~DWARF_Address();
	MEMORY_ADDR GetValue() const;
	virtual std::string to_string() const;
private:
	MEMORY_ADDR value;
};

template <class MEMORY_ADDR>
class DWARF_Block : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_Block(uint64_t length, const uint8_t *value);
	virtual ~DWARF_Block();
	uint64_t GetLength() const;
	const uint8_t *GetValue() const;
	virtual std::string to_string() const;
private:
	uint64_t length;
	const uint8_t *value;
};

template <class MEMORY_ADDR>
class DWARF_Constant : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_Constant(unsigned int dw_class);
	virtual ~DWARF_Constant() {}
	bool IsSigned() const;
	virtual int64_t to_int() const = 0;
	virtual uint64_t to_uint() const = 0;
};

template <class MEMORY_ADDR>
class DWARF_UnsignedConstant : public DWARF_Constant<MEMORY_ADDR>
{
public:
	DWARF_UnsignedConstant(uint64_t value);
	virtual ~DWARF_UnsignedConstant();
	uint64_t GetValue() const;
	virtual std::string to_string() const;
	virtual int64_t to_int() const;
	virtual uint64_t to_uint() const;
private:
	uint64_t value;
};

template <class MEMORY_ADDR>
class DWARF_SignedConstant : public DWARF_Constant<MEMORY_ADDR>
{
public:
	DWARF_SignedConstant(int64_t value);
	virtual ~DWARF_SignedConstant();
	int64_t GetValue() const;
	virtual std::string to_string() const;
	virtual int64_t to_int() const;
	virtual uint64_t to_uint() const;
private:
	int64_t value;
};

template <class MEMORY_ADDR>
class DWARF_UnsignedLEB128Constant : public DWARF_Constant<MEMORY_ADDR>
{
public:
	DWARF_UnsignedLEB128Constant(const DWARF_LEB128& leb128);
	virtual ~DWARF_UnsignedLEB128Constant();
	const DWARF_LEB128& GetValue() const;
	virtual std::string to_string() const;
	virtual int64_t to_int() const;
	virtual uint64_t to_uint() const;
private:
	DWARF_LEB128 value;
};

template <class MEMORY_ADDR>
class DWARF_SignedLEB128Constant : public DWARF_Constant<MEMORY_ADDR>
{
public:
	DWARF_SignedLEB128Constant(const DWARF_LEB128& leb128);
	virtual ~DWARF_SignedLEB128Constant();
	const DWARF_LEB128& GetValue() const;
	virtual std::string to_string() const;
	virtual int64_t to_int() const;
	virtual uint64_t to_uint() const;
private:
	DWARF_LEB128 value;
};

template <class MEMORY_ADDR>
class DWARF_Flag : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_Flag(bool value);
	virtual ~DWARF_Flag();
	bool GetValue() const;
	virtual std::string to_string() const;
private:
	bool value;
};

template <class MEMORY_ADDR>
class DWARF_LinePtr : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_LinePtr(uint64_t debug_line_offset);
	virtual ~DWARF_LinePtr();
	const DWARF_StatementProgram<MEMORY_ADDR> *GetValue() const;
	uint64_t GetDebugLineOffset() const;
	virtual std::string to_string() const;
	virtual void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
private:
	uint64_t debug_line_offset;
	const DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog;
};

template <class MEMORY_ADDR>
class DWARF_LocListPtr : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_LocListPtr(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, uint64_t debug_loc_offset);
	virtual ~DWARF_LocListPtr();
	const DWARF_LocListEntry<MEMORY_ADDR> *GetValue() const;
	uint64_t GetDebugLocOffset() const;
	virtual std::string to_string() const;
	virtual void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
private:
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	uint64_t debug_loc_offset;
	const DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry;
};

template <class MEMORY_ADDR>
class DWARF_MacPtr : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_MacPtr(uint64_t debug_macinfo_offset);
	virtual ~DWARF_MacPtr();
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *GetValue() const;
	uint64_t GetDebugMacInfoOffset() const;
	virtual std::string to_string() const;
	virtual void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
private:
	uint64_t debug_macinfo_offset;
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo_list_entry;
};

template <class MEMORY_ADDR>
class DWARF_RangeListPtr : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_RangeListPtr(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, uint64_t debug_ranges_offset);
	virtual ~DWARF_RangeListPtr();
	uint64_t GetDebugRangesOffset() const;
	const DWARF_RangeListEntry<MEMORY_ADDR> *GetValue() const;
	virtual std::string to_string() const;
	virtual void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
private:
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	uint64_t debug_ranges_offset;
	const DWARF_RangeListEntry<MEMORY_ADDR> *dw_range_list_entry;
};

template <class MEMORY_ADDR>
class DWARF_Reference : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_Reference(uint64_t debug_info_offset);
	virtual ~DWARF_Reference();
	const DWARF_DIE<MEMORY_ADDR> *GetValue() const;
	uint64_t GetDebugInfoOffset() const;
	virtual std::string to_string() const;
	virtual void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
private:
	uint64_t debug_info_offset;
	const DWARF_DIE<MEMORY_ADDR> *dw_die; // Available once resolved
};

template <class MEMORY_ADDR>
class DWARF_String : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_String(uint64_t debug_strp_offset);
	DWARF_String(const char *value);
	virtual ~DWARF_String();
	unsigned int GetLength() const;
	const char *GetValue() const;
	virtual std::string to_string() const;
private:
	uint64_t debug_strp_offset;
	const char *value;
};

template <class MEMORY_ADDR>
class DWARF_Expression : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_Expression(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, uint64_t length, const uint8_t *value);
	DWARF_Expression(const DWARF_CallFrameProgram<MEMORY_ADDR> *dw_cfp, uint64_t length, const uint8_t *value);
	DWARF_Expression(const DWARF_Expression<MEMORY_ADDR>& dw_exp);
	virtual ~DWARF_Expression();
	const DWARF_CompilationUnit<MEMORY_ADDR> *GetCompilationUnit() const;
	const DWARF_CallFrameProgram<MEMORY_ADDR> *GetCallFrameProgram() const;
	uint64_t GetLength() const;
	bool IsEmpty() const;
	const uint8_t *GetValue() const;
	virtual std::string to_string() const;
private:
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	const DWARF_CallFrameProgram<MEMORY_ADDR> *dw_cfp;
	uint64_t length;
	const uint8_t *value;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Attribute<MEMORY_ADDR>& dw_attribute);

template <class MEMORY_ADDR>
class DWARF_Attribute
{
public:
	DWARF_Attribute(const DWARF_DIE<MEMORY_ADDR> *dw_die, DWARF_AbbrevAttribute *dw_abbrev_attribute, DWARF_AttributeValue<MEMORY_ADDR> *dw_value);
	~DWARF_Attribute();
	const DWARF_AbbrevAttribute *GetAbbrevAttribute() const;
	const DWARF_AttributeValue<MEMORY_ADDR> *GetValue() const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_Attribute& dw_attribute);
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	std::ostream& to_XML(std::ostream& os);
	std::ostream& to_HTML(std::ostream& os) const;
private:
	const DWARF_DIE<MEMORY_ADDR> *dw_die;
	DWARF_AbbrevAttribute *dw_abbrev_attribute;
	DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
