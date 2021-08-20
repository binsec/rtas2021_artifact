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

#include <unisim/util/debug/dwarf/c_loc_expr_parser.hh>
#include <unisim/util/parser/parser.tcc>
#include <iostream>
#include <stdlib.h>

// namespace unisim {
// namespace util {
// namespace lexer {
// 
// } // end of namespace lexer
// } // end of namespace util
// } // end of namespace unisim
// 
namespace unisim {
namespace util {
namespace parser {

template <> unsigned int AST<unisim::util::debug::dwarf::CLocType>::num_asts = 0;

} // end of namespace parser
} // end of namespace util
} // end of namespace unisim

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

std::ostream& operator << (std::ostream& os, const CLocType& c_loc_type)
{
	switch(c_loc_type)
	{
		case C_LOC_UNKNOWN: return os << "unknown expression";
		case C_LOC_ARITH: return os << "arithmetic expression";
		case C_LOC_EXPR: return os << "location expression";
	}
	return os;
}

CLocOperationStream::CLocOperationStream(Notation _notation)
	: notation(_notation)
	, storage()
{
}

CLocOperationStream::CLocOperationStream(const CLocOperationStream& c_loc_operation_stream)
	: notation(c_loc_operation_stream.notation)
	, storage()
{
	std::deque<const CLocOperation *>::const_iterator it;
	for(it = c_loc_operation_stream.storage.begin(); it != c_loc_operation_stream.storage.end(); it++)
	{
		storage.push_back((*it)->Clone());
	}
}

CLocOperationStream::~CLocOperationStream()
{
	Clear();
}

void CLocOperationStream::Push(const CLocOperation *op)
{
	storage.push_back(op);
}

const CLocOperation *CLocOperationStream::Pop()
{
	if(storage.empty()) return 0;
	const CLocOperation *op = storage.front();
	storage.pop_front();
	return op;
}

const CLocOperation *CLocOperationStream::PopBack()
{
	if(storage.empty()) return 0;
	const CLocOperation *op = storage.back();
	storage.pop_back();
	return op;
}

unsigned int CLocOperationStream::Size() const
{
	return storage.size();
}

bool CLocOperationStream::Empty() const
{
	return storage.empty();
}

Notation CLocOperationStream::GetNotation() const
{
	return notation;
}

void CLocOperationStream::Clear()
{
	std::deque<const CLocOperation *>::iterator it;
	for(it = storage.begin(); it != storage.end(); it++)
	{
		delete *it;
	}
	storage.clear();
}

std::ostream& operator << (std::ostream& os, const CLocOperationStream& c_loc_operation_stream)
{
	std::deque<const CLocOperation *>::const_iterator const_it;
	for(const_it = c_loc_operation_stream.storage.begin(); const_it != c_loc_operation_stream.storage.end(); const_it++)
	{
		const CLocOperation *c_loc_operation = *const_it;
		
		os << *c_loc_operation << ";";
	}
	
	return os;
}

CLocExprParser::CLocExprParser(std::istream *stream, std::ostream& _debug_info_stream, std::ostream& _debug_warning_stream, std::ostream& _debug_error_stream, bool _debug)
	: unisim::util::parser::Parser<CLocType>(stream, _debug_info_stream, _debug_warning_stream, _debug_error_stream, _debug)
	, debug_info_stream(_debug_info_stream)
	, debug_warning_stream(_debug_warning_stream)
	, debug_error_stream(_debug_error_stream)
	, c_loc_operation_stream(0)
{
	EnableToken("*");
	EnableToken(".");
	EnableToken("->", TOK_ARROW);
	EnableToken("(");
	EnableToken(")");
	EnableToken("[");
	EnableToken("]");
	EnableDecimalIntegerToken(TOK_INT_CONST, true /* parse sign */);
	EnableIdentifierToken(TOK_IDENT);
	EnableEatingSpace();
	EnableEatingTab();
	EnableEatingEndOfLine();
}

CLocExprParser::~CLocExprParser()
{
}

unisim::util::parser::Token<CLocType> *CLocExprParser::CreateToken(const char *text, unsigned int id, const unisim::util::lexer::Location& loc)
{
	if(debug)
	{
		debug_info_stream << "Creating Token \"" << text << "\"" << std::endl;
	}
	switch(id)
	{
		case unisim::util::lexer::Lexer<unisim::util::parser::Token<CLocType> >::TOK_EOF: return new unisim::util::parser::Token<CLocType>(text, id, loc); // end of file
		case '*': return new unisim::util::parser::PrefixOperator<CLocType>('*', loc, 1500, 1500);
		case '(': return new unisim::util::parser::PrefixOperator<CLocType>('(', ')', loc);
		case ')': return new unisim::util::parser::Token<CLocType>(')', loc);
		case '[': return new unisim::util::parser::InfixOperator<CLocType>('[', ']', loc, 1600);
		case ']': return new unisim::util::parser::Token<CLocType>(']', loc);
		case '.': return new unisim::util::parser::InfixOperator<CLocType>('.', loc, 1600, 1600, unisim::util::parser::LEFT_TO_RIGHT);
		case TOK_INT_CONST: return new unisim::util::parser::Literal<CLocType>(text, id, loc);
		case TOK_IDENT: return new unisim::util::parser::Literal<CLocType>(text, id, loc);
		case TOK_ARROW: return new unisim::util::parser::InfixOperator<CLocType>(text, id, loc, 1600, 1600, unisim::util::parser::LEFT_TO_RIGHT);
	}
	return 0;
}

bool CLocExprParser::Parse(CLocOperationStream& _c_loc_operation_stream)
{
	Advance();
	unisim::util::parser::AST<CLocType> *ast = ParseExpression();
	
	if(parser_error) return false;
	if(!ast) return false;
	
	if(token)
	{
		if(!token->IsEndOfFile())
		{
			ErrorUnexpectedToken(token);
			delete ast;
			return false;
		}
	}
	
	//ast->GenerateGraphviz("ast.dot");

	c_loc_operation_stream = &_c_loc_operation_stream;
	unisim::util::parser::DepthFirstSearchTraversalOrder order = unisim::util::parser::DFS_PRE_ORDER;
	switch(c_loc_operation_stream->GetNotation())
	{
		case PREFIX_NOTATION:
			order = unisim::util::parser::DFS_PRE_ORDER;
			break;
		case INFIX_NOTATION:
			order = unisim::util::parser::DFS_IN_ORDER;
			break;
		case POSTFIX_NOTATION:
			order = unisim::util::parser::DFS_POST_ORDER;
			break;
	}
	ast->DFS(this, order);
	c_loc_operation_stream = 0;
	
	delete ast;
	return true;
}

// expr <- left_expr OP right_expr
bool CLocExprParser::Check(unisim::util::parser::Token<CLocType> *token, unisim::util::parser::AST<CLocType> *left, unisim::util::parser::AST<CLocType> *right)
{
	switch(token->GetId())
	{
		case '[':
			// loc_expr <- loc_expr '[' arith_expr ']'
			if(debug)
			{
				debug_info_stream << "loc_expr <- loc_expr '[' arith_expr ']'" << std::endl;
			}
			if(left->GetToken()->GetValue() != C_LOC_EXPR)
			{
				unisim::util::lexer::Lexer<unisim::util::parser::Token<CLocType> >::FinishScanningLine();
				unisim::util::parser::Parser<CLocType>::parser_error = true;
				unisim::util::lexer::Lexer<unisim::util::parser::Token<CLocType> >::PrintFriendlyLocation(token->GetLocation());
				debug_error_stream << token->GetLocation() << ", subscripted value is neither array nor pointer" << std::endl;
				return false;
			}
			if(right->GetToken()->GetValue() != C_LOC_ARITH)
			{
				unisim::util::lexer::Lexer<unisim::util::parser::Token<CLocType> >::FinishScanningLine();
				unisim::util::parser::Parser<CLocType>::parser_error = true;
				unisim::util::lexer::Lexer<unisim::util::parser::Token<CLocType> >::PrintFriendlyLocation(token->GetLocation());
				debug_error_stream << token->GetLocation() << ", expected array subscript" ;
				if(token) debug_error_stream << " before '" << right->GetToken()->GetText() << "'";
				debug_error_stream << std::endl;
				return false;
			}
			token->SetValue(C_LOC_EXPR);
			return true;
		case '.':
			// loc_expr <- loc_expr '.' lit_ident
			if(debug)
			{
				debug_info_stream << "loc_expr <- loc_expr '.' lit_ident" << std::endl;
			}
			if(right->GetToken()->GetId() != TOK_IDENT)
			{
				unisim::util::parser::Parser<CLocType>::ErrorExpectedToken(TOK_IDENT, right->GetToken());
				return false;
			}
			if(left->GetToken()->GetValue() != C_LOC_EXPR)
			{
				unisim::util::lexer::Lexer<unisim::util::parser::Token<CLocType> >::FinishScanningLine();
				unisim::util::parser::Parser<CLocType>::parser_error = true;
				unisim::util::lexer::Lexer<unisim::util::parser::Token<CLocType> >::PrintFriendlyLocation(token->GetLocation());
				debug_error_stream << token->GetLocation() << ", request for member \"" << right->GetToken()->GetText() << "\" in something not a structure" << std::endl;
				return false;
			}
			token->SetValue(C_LOC_EXPR);
			return true;
		case TOK_ARROW:
			// loc_expr <- loc_expr '->' lit_ident
			if(debug)
			{
				debug_info_stream << "loc_expr <- loc_expr '->' lit_ident" << std::endl;
			}
			if(right->GetToken()->GetId() != TOK_IDENT)
			{
				unisim::util::parser::Parser<CLocType>::ErrorExpectedToken(TOK_IDENT, right->GetToken());
				return false;
			}
			if(left->GetToken()->GetValue() != C_LOC_EXPR)
			{
				unisim::util::lexer::Lexer<unisim::util::parser::Token<CLocType> >::FinishScanningLine();
				unisim::util::parser::Parser<CLocType>::parser_error = true;
				unisim::util::lexer::Lexer<unisim::util::parser::Token<CLocType> >::PrintFriendlyLocation(token->GetLocation());
				debug_error_stream << token->GetLocation() << ", request for member \"" << right->GetToken()->GetText() << "\" in something not a pointed structure" << std::endl;
				return false;
			}
			token->SetValue(C_LOC_EXPR);
			return true;
	}
	return false;
}

// expr <- OP expr
bool CLocExprParser::Check(unisim::util::parser::Token<CLocType> *token, unisim::util::parser::AST<CLocType> *child)
{
	switch(token->GetId())
	{
		case '*':
			// loc_expr <- '*' loc_expr
			if(debug)
			{
				debug_info_stream << "loc_expr <- '*' loc_expr" << std::endl;
			}
			if(child->GetToken()->GetValue() != C_LOC_EXPR)
			{
				unisim::util::lexer::Lexer<unisim::util::parser::Token<CLocType> >::FinishScanningLine();
				unisim::util::parser::Parser<CLocType>::parser_error = true;
				unisim::util::lexer::Lexer<unisim::util::parser::Token<CLocType> >::PrintFriendlyLocation(token->GetLocation());
				debug_error_stream << token->GetLocation() << ", dereferencing \"" << child->GetToken()->GetText() << "\" which is not a pointer" << std::endl;
				return false;
			}
			token->SetValue(C_LOC_EXPR);
			return true;
	}
	return false;
}

// expr <- literal
bool CLocExprParser::Check(unisim::util::parser::Token<CLocType> *token)
{
	switch(token->GetId())
	{
		case TOK_INT_CONST:
			// arith_expr <- lit_int
			if(debug)
			{
				debug_info_stream << "arith_expr <- lit_int" << std::endl;
			}
			token->SetValue(C_LOC_ARITH);
			return true;
		case TOK_IDENT:
			// loc_expr <- lit_ident
			if(debug)
			{
				debug_info_stream << "loc_expr <- lit_ident" << std::endl;
			}
			token->SetValue(C_LOC_EXPR);
			return true;
	}
	return false;
}

void CLocExprParser::Visit(unisim::util::parser::AST<CLocType> *ast)
{
	if(c_loc_operation_stream)
	{
		const unisim::util::parser::Token<CLocType> *token = ast->GetToken();
		
		switch(token->GetId())
		{
			case '*':
				c_loc_operation_stream->Push(new CLocOperation(OP_DEREF));
				break;
			case '[':
				c_loc_operation_stream->Push(new CLocOperation(OP_ARRAY_SUBSCRIPT));
				break;
			case '.':
				c_loc_operation_stream->Push(new CLocOperation(OP_STRUCT_REF));
				break;
			case TOK_INT_CONST:
				c_loc_operation_stream->Push(new CLocOpLiteralInteger(atoi(token->GetText())));
				break;
			case TOK_IDENT:
				c_loc_operation_stream->Push(new CLocOpLiteralIdentifier(token->GetText()));
				break;
			case TOK_ARROW:
				c_loc_operation_stream->Push(new CLocOperation(OP_STRUCT_DEREF));
				break;
		}
	}
}

CLocOperation::CLocOperation(CLocOpcode _opcode)
	: opcode(_opcode)
{
}

CLocOperation::~CLocOperation()
{
}

CLocOperation *CLocOperation::Clone() const
{
	return new CLocOperation(opcode);
}

CLocOpcode CLocOperation::GetOpcode() const
{
	return opcode;
}

std::ostream& operator << (std::ostream& os, const CLocOperation& op)
{
	switch(op.opcode)
	{
		case OP_LIT_INT:
			os << "OP_LIT_INT " << ((const CLocOpLiteralInteger *) &op)->GetValue();
			break;
		case OP_LIT_IDENT:
			os << "OP_LIT_IDENT " << ((const CLocOpLiteralIdentifier *) &op)->GetIdentifier();
			break;
		case OP_DEREF:
			os << "OP_DEREF";
			break;
		case OP_STRUCT_REF:
			os << "OP_STRUCT_REF";
			break;
		case OP_STRUCT_DEREF:
			os << "OP_STRUCT_DEREF";
			break;
		case OP_ARRAY_SUBSCRIPT:
			os << "OP_ARRAY_SUBSCRIPT";
			break;
	}
	return os;
}

CLocOpLiteralInteger::CLocOpLiteralInteger(int _value)
	: CLocOperation(OP_LIT_INT)
	, value(_value)
{
}

int CLocOpLiteralInteger::GetValue() const
{
	return value;
}

CLocOperation *CLocOpLiteralInteger::Clone() const
{
	return new CLocOpLiteralInteger(value);
}

CLocOpLiteralIdentifier::CLocOpLiteralIdentifier(const char *_identifier)
	: CLocOperation(OP_LIT_IDENT)
	, identifier(_identifier)
{
}

const char *CLocOpLiteralIdentifier::GetIdentifier() const
{
	return identifier.c_str();
}

CLocOperation *CLocOpLiteralIdentifier::Clone() const
{
	return new CLocOpLiteralIdentifier(identifier.c_str());
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
