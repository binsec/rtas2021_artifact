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

#ifndef __UNISIM_UTIL_PARSER_PARSER_TCC__
#define __UNISIM_UTIL_PARSER_PARSER_TCC__

#include <unisim/util/parser/parser.hh>
#include <unisim/util/lexer/lexer.tcc>
#include <iostream>
#include <fstream>
#include <inttypes.h>

namespace unisim {
namespace util {
namespace parser {

template <class ABSTRACT_VALUE>
Token<ABSTRACT_VALUE>::Token(const unisim::util::lexer::Location& _loc)
	: text("")
	, id(0)
	, loc(_loc)
	, lbp(0)
	, value()
{
}

template <class ABSTRACT_VALUE>
Token<ABSTRACT_VALUE>::Token(char _c, const unisim::util::lexer::Location& _loc, unsigned int _lbp)
	: text(1, _c)
	, id((uint8_t) _c)
	, loc(_loc)
	, lbp(_lbp)
	, value()
{
}

template <class ABSTRACT_VALUE>
Token<ABSTRACT_VALUE>::Token(const char *_text, unsigned int _id, const unisim::util::lexer::Location& _loc, unsigned int _lbp)
	: text(_text)
	, id(_id)
	, loc(_loc)
	, lbp(_lbp)
	, value()
{
}

template <class ABSTRACT_VALUE>
Token<ABSTRACT_VALUE>::~Token()
{
}

template <class ABSTRACT_VALUE>
const char *Token<ABSTRACT_VALUE>::GetText() const
{
	return text.c_str();
}

template <class ABSTRACT_VALUE>
unsigned int Token<ABSTRACT_VALUE>::GetId() const
{
	return id;
}

template <class ABSTRACT_VALUE>
unsigned int Token<ABSTRACT_VALUE>::GetLBP() const
{
	return lbp;
}

template <class ABSTRACT_VALUE>
const unisim::util::lexer::Location& Token<ABSTRACT_VALUE>::GetLocation() const
{
	return loc;
}

template <class ABSTRACT_VALUE>
void Token<ABSTRACT_VALUE>::SetValue(ABSTRACT_VALUE _value)
{
	value = _value;
}

template <class ABSTRACT_VALUE>
ABSTRACT_VALUE Token<ABSTRACT_VALUE>::GetValue() const
{
	return value;
}

template <class ABSTRACT_VALUE>
AST<ABSTRACT_VALUE> *Token<ABSTRACT_VALUE>::led(Parser<ABSTRACT_VALUE> *parser, AST<ABSTRACT_VALUE> *left)
{
	if(parser->IsDebugging())
	{
		std::ostream& debug_info_stream = parser->GetDebugInfoStream();
		debug_info_stream << *this << "->Token::led(";
		if(left) debug_info_stream << *left; else debug_info_stream << "null";
		debug_info_stream << ") returns null" << std::endl;
	}
	parser->ErrorUnexpectedToken(this);
	delete left;
	return 0;
}

template <class ABSTRACT_VALUE>
AST<ABSTRACT_VALUE> *Token<ABSTRACT_VALUE>::nud(Parser<ABSTRACT_VALUE> *parser)
{
	if(parser->IsDebugging())
	{
		std::ostream& debug_info_stream = parser->GetDebugInfoStream();
		debug_info_stream << *this << "->Token::nud() returns null" << std::endl;
	}
	parser->ErrorUnexpectedToken(this);
	return 0;
}

template <class ABSTRACT_VALUE>
bool Token<ABSTRACT_VALUE>::IsEndOfFile() const
{
	return id == unisim::util::lexer::Lexer<Token>::TOK_EOF;
}

template <class ABSTRACT_VALUE>
std::ostream& operator << (std::ostream& os, const Token<ABSTRACT_VALUE>& token)
{
	if((token.id > 0) && (token.id < unisim::util::lexer::Lexer<Token<ABSTRACT_VALUE> >::TOK_EOF))
	{
		os << "'" << (char) token.id << "'";
	}
	else
	{
		switch(token.id)
		{
			case unisim::util::lexer::Lexer<Token<ABSTRACT_VALUE> >::TOK_EOF:
				os << "end-of-file";
				break;
			default:
				os << token.id;
				break;
		}
	}
	os << ":" << token.lbp << ":\"";
	
	if(token.id)
	{
		unsigned int text_length = token.text.length();
		unsigned int i;
		for(i = 0; i < text_length; i++)
		{
			char c = token.text[i];
			switch(c)
			{
				case '\n':
				case '\r':
					os << "<eol>";
					break;
				case '\t':
					os << "<tab>";
					break;
				default:
					os << c;
					break;
			}
		}
	}
	else
	{
		os << "<eof>";
	}
	os << "\"";
	return os;
}

template <class ABSTRACT_VALUE>
AST<ABSTRACT_VALUE>::AST(Token<ABSTRACT_VALUE> *_token)
	: id(num_asts++)
	, arity(0)
	, token(_token)
	, children()
{
	children[0] = children[1] = children[2] = 0;
}

template <class ABSTRACT_VALUE>
AST<ABSTRACT_VALUE>::AST(Token<ABSTRACT_VALUE> *_token, AST<ABSTRACT_VALUE> *_first)
	: id(num_asts++)
	, arity(_first ? 1 : 0)
	, token(_token)
	, children()
{
	children[0] = _first;
	children[1] = children[2] = 0;
}

template <class ABSTRACT_VALUE>
AST<ABSTRACT_VALUE>::AST(Token<ABSTRACT_VALUE> *_token, AST<ABSTRACT_VALUE> *_first, AST<ABSTRACT_VALUE> *_second)
	: id(num_asts++)
	, arity(_first ? (_second ? 2 : 1) : (_second ? 1 : 0))
	, token(_token)
	, children()
{
	children[0] = _first ? _first : _second;
	children[1] = _first ? _second : 0;
	children[2] = 0;
}

template <class ABSTRACT_VALUE>
AST<ABSTRACT_VALUE>::AST(Token<ABSTRACT_VALUE> *_token, AST<ABSTRACT_VALUE> *_first, AST<ABSTRACT_VALUE> *_second, AST<ABSTRACT_VALUE> *_third)
	: id(num_asts++)
	, arity(_first ? (_second ? (_third ? 3 : 2) : (_third ? 2 : 1)) : (_second ? (_third ? 2 : 1) : (_third ? 1 : 0)))
	, token(_token)
	, children()
{
	children[0] = _first ? (_second ? _second : _third) : (_second ? _second : _third);
	children[1] = _first ? (_second ? _second : _third) : (_second ? _third : 0);
	children[2] = _first ? (_second ? _third : 0) : 0;
}

template <class ABSTRACT_VALUE>
AST<ABSTRACT_VALUE>::~AST()
{
	if(token) delete token;
	unsigned int i;
	for(i = 0; i < arity; i++)
	{
		delete children[i];
	}
}

template <class ABSTRACT_VALUE>
const Token<ABSTRACT_VALUE> *AST<ABSTRACT_VALUE>::GetToken() const
{
	return token;
}

template <class ABSTRACT_VALUE>
const AST<ABSTRACT_VALUE> *AST<ABSTRACT_VALUE>::GetFirst() const
{
	return children[0];
}

template <class ABSTRACT_VALUE>
const AST<ABSTRACT_VALUE> *AST<ABSTRACT_VALUE>::GetSecond() const
{
	return children[1];
}

template <class ABSTRACT_VALUE>
const AST<ABSTRACT_VALUE> *AST<ABSTRACT_VALUE>::GetThird() const
{
	return children[2];
}

template <class ABSTRACT_VALUE>
std::ostream& operator << (std::ostream& os, const AST<ABSTRACT_VALUE>& ast)
{
	os << "AST(" << (*ast.token);
	unsigned int i;
	for(i = 0; i < ast.arity; i++)
	{
		switch(i)
		{
			case 0: os << ",first="; break;
			case 1: os << ",second="; break;
			case 2: os << ",third="; break;
		}
		if(ast.children[i])
		{
			os << (*ast.children[i]);
		}
		else
		{
			os << "null";
		}
	}
	os << ")";
	return os;
}

inline std::string graphviz_escape_text(const char *s)
{
	std::string r;
	char c = *s;
	
	if(c)
	{
		do
		{
			if(c == '"') r += '\\';
			r += c;
		}
		while((c = *++s) != 0);
	}
	return r;
}

template <class ABSTRACT_VALUE>
void AST<ABSTRACT_VALUE>::GenerateGraphvizVertex(std::ostream& os) const
{
	os << "node [label=\"" << graphviz_escape_text(token->GetText()) << "\"]; n" << id << ";" << std::endl;
	unsigned int i;
	for(i = 0; i < arity; i++)
	{
		children[i]->GenerateGraphvizVertex(os);
	}
}

template <class ABSTRACT_VALUE>
void AST<ABSTRACT_VALUE>::GenerateGraphvizEdge(std::ostream& os) const
{
	unsigned int i;
	for(i = 0; i < arity; i++)
	{
		os << "n" << id << "->n" << children[i]->id << ";" << std::endl;
		children[i]->GenerateGraphvizEdge(os);
	}
}

template <class ABSTRACT_VALUE>
void AST<ABSTRACT_VALUE>::GenerateGraphviz(const char *filename) const
{
	std::ofstream f(filename);
	
	f << "digraph {" << std::endl;
	GenerateGraphvizVertex(f);
	GenerateGraphvizEdge(f);
	f << "}" << std::endl;
}

template <class ABSTRACT_VALUE>
void AST<ABSTRACT_VALUE>::DFS(Visitor<ABSTRACT_VALUE> *visitor, DepthFirstSearchTraversalOrder order)
{
	if(order == DFS_PRE_ORDER)
	{
		visitor->Visit(this);
	}
	
	if(arity)
	{
		unsigned int i;
		for(i = 0; i < (arity - 1); i++)
		{
			children[i]->DFS(visitor, order);
			if(order == DFS_IN_ORDER)
			{
				visitor->Visit(this);
			}
		}
		children[arity - 1]->DFS(visitor, order);
		if(arity < 2)
		{
			if(order == DFS_IN_ORDER)
			{
				visitor->Visit(this);
			}
		}
	}
	else if(order == DFS_IN_ORDER)
	{
		visitor->Visit(this);
	}
	
	if(order == DFS_POST_ORDER)
	{
		visitor->Visit(this);
	}
}

template <class ABSTRACT_VALUE>
unsigned int AST<ABSTRACT_VALUE>::GetArity() const
{
	return arity;
}

template <class ABSTRACT_VALUE>
Parser<ABSTRACT_VALUE>::Parser(std::istream *_stream, std::ostream& _debug_info_stream, std::ostream& _debug_warning_stream, std::ostream& _debug_error_stream, bool _debug)
	: unisim::util::lexer::Lexer<Token<ABSTRACT_VALUE> >(_stream, _debug_info_stream, _debug_warning_stream, _debug_error_stream, _debug)
	, parser_error(false)
	, debug_info_stream(_debug_info_stream)
	, debug_warning_stream(_debug_warning_stream)
	, debug_error_stream(_debug_error_stream)
	, debug(_debug)
	, token(0)
{
}

template <class ABSTRACT_VALUE>
Parser<ABSTRACT_VALUE>::~Parser()
{
	if(token) delete token;
}

template <class ABSTRACT_VALUE>
AST<ABSTRACT_VALUE> *Parser<ABSTRACT_VALUE>::ParseExpression(unsigned int rbp)
{
	if(!token)
	{
		// no more token
		ErrorExpectedExpression();
		if(debug)
		{
			debug_info_stream << "Parser::ParseExpression returns null" << std::endl;
		}
		return 0;
	}
	if(token->IsEndOfFile())
	{
		ErrorUnexpectedToken(token);
		if(debug)
		{
			debug_info_stream << "Parser::ParseExpression returns null" << std::endl;
		}
		return 0;
	}
	Token<ABSTRACT_VALUE> *t = token;
	Advance();
	AST<ABSTRACT_VALUE> *left = t->nud(this);
	if(!left)
	{
		delete t;
		if(debug)
		{
			debug_info_stream << "Parser::ParseExpression returns null" << std::endl;
		}
		return 0;
	}
	if(!token)
	{
		// no more token, we should have stopped right before this point (no end of file token with lbp=0 ?)
		InternalError();
		delete left;
		if(debug)
		{
			debug_info_stream << "Parser::ParseExpression returns null" << std::endl;
		}
		return 0;
	}
	while(left && token && !token->IsEndOfFile() && (rbp < token->GetLBP()))
	{
		t = token;
		Advance();
		AST<ABSTRACT_VALUE> *ast = t->led(this, left);
		if(!ast)
		{
			delete t;
			if(debug)
			{
				debug_info_stream << "Parser::ParseExpression returns null" << std::endl;
			}
			return 0;
		}
		left = ast;
	}
	
	if(parser_error)
	{
		delete left;
		left = 0;
	}
	
	if(debug)
	{
		debug_info_stream << "Parser::ParseExpression() returns ";
		if(left) debug_info_stream << *left; else debug_info_stream << "null";
		debug_info_stream << std::endl;
	}
	return left;
}

template <class ABSTRACT_VALUE>
bool Parser<ABSTRACT_VALUE>::Advance(unsigned int token_id)
{
	if(token_id)
	{
		if(token)
		{
			if(token->GetId() != token_id)
			{
				ErrorExpectedToken(token_id, token);
				return false;
			}
			delete token;
		}
		else
		{
			ErrorExpectedToken(token_id);
			return false;
		}
	}
	token = unisim::util::lexer::Lexer<Token<ABSTRACT_VALUE> >::Next();
	if(unisim::util::lexer::Lexer<Token<ABSTRACT_VALUE> >::lexer_error) parser_error = true;
	return true;
}

template <class ABSTRACT_VALUE>
void Parser<ABSTRACT_VALUE>::ErrorExpectedToken(unsigned int expected_token_id, const Token<ABSTRACT_VALUE> *token)
{
	unisim::util::lexer::Lexer<Token<ABSTRACT_VALUE> >::FinishScanningLine();
	parser_error = true;
	unisim::util::lexer::Lexer<Token<ABSTRACT_VALUE> >::PrintFriendlyLocation(token->GetLocation());
	debug_error_stream << token->GetLocation() << ", expected '" << unisim::util::lexer::Lexer<Token<ABSTRACT_VALUE> >::GetTokenText(expected_token_id) << "'";
	if(token) debug_error_stream << " before '" << token->GetText() << "'";
	debug_error_stream << std::endl;
}

template <class ABSTRACT_VALUE>
void Parser<ABSTRACT_VALUE>::ErrorUnexpectedToken(const Token<ABSTRACT_VALUE> *token)
{
	unisim::util::lexer::Lexer<Token<ABSTRACT_VALUE> >::FinishScanningLine();
	parser_error = true;
	unisim::util::lexer::Lexer<Token<ABSTRACT_VALUE> >::PrintFriendlyLocation(token->GetLocation());
	debug_error_stream << token->GetLocation() << ", unexpected '" << token->GetText() << "'" << std::endl;
}

template <class ABSTRACT_VALUE>
void Parser<ABSTRACT_VALUE>::ErrorExpectedExpression()
{
	unisim::util::lexer::Lexer<Token<ABSTRACT_VALUE> >::FinishScanningLine();
	parser_error = true;
	unisim::util::lexer::Lexer<Token<ABSTRACT_VALUE> >::PrintFriendlyLocation(unisim::util::lexer::Lexer<Token<ABSTRACT_VALUE> >::GetLocation());
	debug_error_stream << unisim::util::lexer::Lexer<Token<ABSTRACT_VALUE> >::GetLocation() << ", expected expression" << std::endl;
}

template <class ABSTRACT_VALUE>
void Parser<ABSTRACT_VALUE>::InternalError()
{
	parser_error = true;
	debug_error_stream << "Internal parser error!" << std::endl;
}

template <class ABSTRACT_VALUE>
std::ostream& Parser<ABSTRACT_VALUE>::GetDebugInfoStream()
{
	return debug_info_stream;
}

template <class ABSTRACT_VALUE>
std::ostream& Parser<ABSTRACT_VALUE>::GetDebugWarningStream()
{
	return debug_warning_stream;
}

template <class ABSTRACT_VALUE>
std::ostream& Parser<ABSTRACT_VALUE>::GetDebugErrorStream()
{
	return debug_error_stream;
}

template <class ABSTRACT_VALUE>
bool Parser<ABSTRACT_VALUE>::IsDebugging() const
{
	return debug;
}

template <class ABSTRACT_VALUE>
Literal<ABSTRACT_VALUE>::Literal(const char *_text, unsigned int _id, const unisim::util::lexer::Location& _loc)
	: Token<ABSTRACT_VALUE>(_text, _id, _loc)
{
}

template <class ABSTRACT_VALUE>
AST<ABSTRACT_VALUE> *Literal<ABSTRACT_VALUE>::nud(Parser<ABSTRACT_VALUE> *parser)
{
	AST<ABSTRACT_VALUE> *ast = parser->Check(this) ? new AST<ABSTRACT_VALUE>(this) : 0;
	if(parser->IsDebugging())
	{
		std::ostream& debug_info_stream = parser->GetDebugInfoStream();
		debug_info_stream << *this << "->Literal::nud() returns " << *ast << std::endl;
	}
	return ast;
}

template <class ABSTRACT_VALUE>
InfixOperator<ABSTRACT_VALUE>::InfixOperator(char _c, const unisim::util::lexer::Location& _loc, unsigned int _token_lbp, unsigned int _bp, Associativity _assoc)
	: Token<ABSTRACT_VALUE>(_c, _loc, _token_lbp)
	, bp(_bp)
	, assoc(_assoc)
	, end_of_subscript_id(0)
{
}

template <class ABSTRACT_VALUE>
InfixOperator<ABSTRACT_VALUE>::InfixOperator(const char *_text, unsigned int _id, const unisim::util::lexer::Location& _loc, unsigned int _token_lbp, unsigned int _bp, Associativity _assoc)
	: Token<ABSTRACT_VALUE>(_text, _id, _loc, _token_lbp)
	, bp(_bp)
	, assoc(_assoc)
	, end_of_subscript_id(0)
{
}

template <class ABSTRACT_VALUE>
InfixOperator<ABSTRACT_VALUE>::InfixOperator(char _c, char _end_of_subscript_c, const unisim::util::lexer::Location& _loc, unsigned _token_lbp)
	: Token<ABSTRACT_VALUE>(_c, _loc, _token_lbp)
	, bp(0)
	, assoc(LEFT_TO_RIGHT)
	, end_of_subscript_id((uint8_t) _end_of_subscript_c)
{
}

template <class ABSTRACT_VALUE>
InfixOperator<ABSTRACT_VALUE>::InfixOperator(const char *_text, unsigned int _id, unsigned int _end_of_subscript_id, const unisim::util::lexer::Location& _loc, unsigned _token_lbp)
	: Token<ABSTRACT_VALUE>(_text, _id, _loc, _token_lbp)
	, bp(0)
	, assoc(LEFT_TO_RIGHT)
	, end_of_subscript_id(_end_of_subscript_id)
{
}

template <class ABSTRACT_VALUE>
AST<ABSTRACT_VALUE> *InfixOperator<ABSTRACT_VALUE>::led(Parser<ABSTRACT_VALUE> *parser, AST<ABSTRACT_VALUE> *left)
{
	AST<ABSTRACT_VALUE> *ast = 0;
	AST<ABSTRACT_VALUE> *right = parser->ParseExpression((assoc == LEFT_TO_RIGHT) ? bp : bp - 1);
	
	if(right)
	{
		if(!end_of_subscript_id || parser->Advance(end_of_subscript_id))
		{
			ast = (left && right && parser->Check(this, left, right)) ? new AST<ABSTRACT_VALUE>(this, left, right) : 0;
		}
	}
	
	if(parser->IsDebugging())
	{
		std::ostream& debug_info_stream = parser->GetDebugInfoStream();
		debug_info_stream << *this << "->InfixOperator::led(";
		if(left) debug_info_stream << *left; else debug_info_stream << "null";
		debug_info_stream << ") returns ";
		if(ast) debug_info_stream << *ast; else debug_info_stream << "null";
		debug_info_stream << std::endl;
	}
	
	if(!ast && left) delete left;
	if(!ast && right) delete right;
	return ast;
}

template <class ABSTRACT_VALUE>
PrefixOperator<ABSTRACT_VALUE>::PrefixOperator(char _c, const unisim::util::lexer::Location& _loc, unsigned int _token_lbp, unsigned _bp)
	: Token<ABSTRACT_VALUE>(_c, _loc, _token_lbp)
	, bp(_bp)
	, closing_id(0)
{
}

template <class ABSTRACT_VALUE>
PrefixOperator<ABSTRACT_VALUE>::PrefixOperator(const char *_text, unsigned int _id, const unisim::util::lexer::Location& _loc, unsigned int _token_lbp, unsigned _bp)
	: Token<ABSTRACT_VALUE>(_text, _id, _loc, _token_lbp)
	, bp(_bp)
	, closing_id(0)
{
}

template <class ABSTRACT_VALUE>
PrefixOperator<ABSTRACT_VALUE>::PrefixOperator(char _c, char _closing_c, const unisim::util::lexer::Location& _loc)
	: Token<ABSTRACT_VALUE>(_c, _loc)
	, bp(0)
	, closing_id((uint8_t) _closing_c)
{
}

template <class ABSTRACT_VALUE>
PrefixOperator<ABSTRACT_VALUE>::PrefixOperator(const char *_text, unsigned int _id, unsigned int _closing_id, const unisim::util::lexer::Location& _loc)
	: Token<ABSTRACT_VALUE>(_text, _id, _loc)
	, bp(0)
	, closing_id(_closing_id)
{
}

template <class ABSTRACT_VALUE>
AST<ABSTRACT_VALUE> *PrefixOperator<ABSTRACT_VALUE>::nud(Parser<ABSTRACT_VALUE> *parser)
{
	AST<ABSTRACT_VALUE> *ast = 0;
	AST<ABSTRACT_VALUE> *right = parser->ParseExpression(bp);
	if(closing_id)
	{
		if(right)
		{
			// grouping operator
			if(!parser->Advance(closing_id))
			{
				delete right;
				right = 0;
			}
		}
		ast = right;
	}
	else
	{
		ast = (right && parser->Check(this, right)) ? new AST<ABSTRACT_VALUE>(this, right) : 0;
	}
	
	if(parser->IsDebugging())
	{
		std::ostream& debug_info_stream = parser->GetDebugInfoStream();
		debug_info_stream << *this << "->PrefixOperator::nud() returns ";
		if(ast) debug_info_stream << *ast; else debug_info_stream << "null";
		debug_info_stream << std::endl;
	}
	
	if(closing_id && ast) delete this; // delete token as it is not stored in the ast
	return ast;
}

template <class ABSTRACT_VALUE>
PrefixInfixOperator<ABSTRACT_VALUE>::PrefixInfixOperator(char _c, const unisim::util::lexer::Location& _loc, unsigned int _token_lbp, unsigned int _prefix_bp /* for nud */, unsigned int _infix_bp /* for led */, Associativity _infix_assoc)
	: Token<ABSTRACT_VALUE>(_c, _loc, _token_lbp)
	, PrefixOperator<ABSTRACT_VALUE>(_c, _loc, _token_lbp, _prefix_bp)
	, InfixOperator<ABSTRACT_VALUE>(_c, _loc, _token_lbp, _infix_bp, _infix_assoc)
{
}

template <class ABSTRACT_VALUE>
PrefixInfixOperator<ABSTRACT_VALUE>::PrefixInfixOperator(const char *_text, unsigned int _id, const unisim::util::lexer::Location& _loc, unsigned int _token_lbp, unsigned int _prefix_bp /* for nud */, unsigned int _infix_bp /* for led */, Associativity _infix_assoc)
	: Token<ABSTRACT_VALUE>(_text, _id, _loc, _token_lbp)
	, PrefixOperator<ABSTRACT_VALUE>(_text, _id, _loc, _token_lbp, _prefix_bp)
	, InfixOperator<ABSTRACT_VALUE>(_text, _id, _loc, _token_lbp, _infix_bp, _infix_assoc)
{
}

template <class ABSTRACT_VALUE>
SuffixOperator<ABSTRACT_VALUE>::SuffixOperator(char _c, const unisim::util::lexer::Location& _loc, unsigned int _token_lbp)
	: Token<ABSTRACT_VALUE>(_c, _loc, _token_lbp)
{
}

template <class ABSTRACT_VALUE>
SuffixOperator<ABSTRACT_VALUE>::SuffixOperator(const char *_text, unsigned int _id, const unisim::util::lexer::Location& _loc, unsigned int _token_lbp)
	: Token<ABSTRACT_VALUE>(_text, _id, _loc, _token_lbp)
{
}

template <class ABSTRACT_VALUE>
AST<ABSTRACT_VALUE> *SuffixOperator<ABSTRACT_VALUE>::led(Parser<ABSTRACT_VALUE> *parser, AST<ABSTRACT_VALUE> *left)
{
	AST<ABSTRACT_VALUE> *ast = (left && parser->Check(this, left)) ? new AST<ABSTRACT_VALUE>(this, left) : 0;
	
	if(parser->IsDebugging())
	{
		std::ostream& debug_info_stream = parser->GetDebugInfoStream();
		debug_info_stream << *this << "->SuffixOperator::led(";
		if(left) debug_info_stream << *left; else debug_info_stream << "null";
		debug_info_stream << ") returns " << *ast << std::endl;
	}
	return ast;
}

template <class ABSTRACT_VALUE>
PrefixSuffixOperator<ABSTRACT_VALUE>::PrefixSuffixOperator(char _c, const unisim::util::lexer::Location& _loc, unsigned int _token_lbp, unsigned int _prefix_bp /* for nud */)
	: Token<ABSTRACT_VALUE>(_c, _loc, _token_lbp)
	, PrefixOperator<ABSTRACT_VALUE>(_c, _loc, _token_lbp, _prefix_bp)
	, SuffixOperator<ABSTRACT_VALUE>(_c, _loc, _token_lbp)
{
}

template <class ABSTRACT_VALUE>
PrefixSuffixOperator<ABSTRACT_VALUE>::PrefixSuffixOperator(const char *_text, unsigned int _id, const unisim::util::lexer::Location& _loc, unsigned int _token_lbp, unsigned int _prefix_bp /* for nud */)
	: Token<ABSTRACT_VALUE>(_text, _id, _loc, _token_lbp)
	, PrefixOperator<ABSTRACT_VALUE>(_text, _id, _loc, _token_lbp, _prefix_bp)
	, SuffixOperator<ABSTRACT_VALUE>(_text, _id, _loc, _token_lbp)
{
}

} // end of namespace parser
} // end of namespace util
} // end of namespace unisim

#endif
