/*
 *  Copyright (c) 2013,
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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_C_LOC_EXPR_PARSER_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_C_LOC_EXPR_PARSER_HH__

#include <unisim/util/parser/parser.hh>
#include <deque>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

class CLocOperation;
class CLocOpLiteralInteger;
class CLocOpLiteralIdentifier;
class CLocOperationStream;
class CLocExprParser;

typedef enum
{
	OP_LIT_INT,         // integer
	OP_LIT_IDENT,       // identifier
	OP_DEREF,           // *p
	OP_STRUCT_REF,      // s.x
	OP_STRUCT_DEREF,    // s->x
	OP_ARRAY_SUBSCRIPT, // a[]
} CLocOpcode;

std::ostream& operator << (std::ostream& os, const CLocOperation& op);

class CLocOperation
{
public:
	CLocOperation(CLocOpcode opcode);
	virtual ~CLocOperation();
	virtual CLocOperation *Clone() const;
	CLocOpcode GetOpcode() const;
private:
	CLocOpcode opcode;
	
	friend std::ostream& operator << (std::ostream& os, const CLocOperation& op);
};

class CLocOpLiteralInteger : public CLocOperation
{
public:
	CLocOpLiteralInteger(int value);
	int GetValue() const;
	virtual CLocOperation *Clone() const;
private:
	int value;
};

class CLocOpLiteralIdentifier : public CLocOperation
{
public:
	CLocOpLiteralIdentifier(const char *identifier);
	const char *GetIdentifier() const;
	virtual CLocOperation *Clone() const;
private:
	std::string identifier;
};

std::ostream& operator << (std::ostream& os, const CLocOperationStream& c_loc_operation_stream);

typedef enum
{
	PREFIX_NOTATION,
	INFIX_NOTATION,
	POSTFIX_NOTATION
} Notation;

class CLocOperationStream
{
public:
	CLocOperationStream(Notation notation);
	CLocOperationStream(const CLocOperationStream& c_loc_operation_stream);
	virtual ~CLocOperationStream();
	void Push(const CLocOperation *op);
	const CLocOperation *Pop();
	const CLocOperation *PopBack();
	unsigned int Size() const;
	bool Empty() const;
	Notation GetNotation() const;
	void Clear();
	
	friend std::ostream& operator << (std::ostream& os, const CLocOperationStream& c_loc_operation_stream);
private:
	Notation notation;
	std::deque<const CLocOperation *> storage;
};

typedef enum
{
	C_LOC_UNKNOWN = 0,
	C_LOC_ARITH,
	C_LOC_EXPR
} CLocType;

class CLocExprParser : public unisim::util::parser::Parser<CLocType>, public unisim::util::parser::Visitor<CLocType>
{
public:
	CLocExprParser(std::istream *stream, std::ostream& debug_info_stream, std::ostream& debug_warning_stream, std::ostream& debug_error_stream, bool debug = false);
	virtual ~CLocExprParser();

	bool Parse(CLocOperationStream& c_loc_operation_stream);

	virtual unisim::util::parser::Token<CLocType> *CreateToken(const char *text, unsigned int id, const unisim::util::lexer::Location& loc);
	virtual void Visit(unisim::util::parser::AST<CLocType> *ast);
	
	virtual bool Check(unisim::util::parser::Token<CLocType> *token, unisim::util::parser::AST<CLocType> *left, unisim::util::parser::AST<CLocType> *right);
	virtual bool Check(unisim::util::parser::Token<CLocType> *token, unisim::util::parser::AST<CLocType> *child);
	virtual bool Check(unisim::util::parser::Token<CLocType> *token);
private:
	enum
	{
		TOK_INT_CONST = TOK_EOF + 1,
		TOK_IDENT,
		TOK_ARROW,
	};
	
	std::ostream& debug_info_stream;
	std::ostream& debug_warning_stream;
	std::ostream& debug_error_stream;
	CLocOperationStream *c_loc_operation_stream;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
