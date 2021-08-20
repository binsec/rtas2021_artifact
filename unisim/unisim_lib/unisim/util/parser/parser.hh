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

#ifndef __UNISIM_UTIL_PARSER_PARSER_HH__
#define __UNISIM_UTIL_PARSER_PARSER_HH__

#include <unisim/util/lexer/lexer.hh>
#include <iosfwd>
#include <string>

namespace unisim {
namespace util {
namespace parser {

template <class ABSTRACT_VALUE> class Token;
template <class ABSTRACT_VALUE> class Parser;
template <class ABSTRACT_VALUE> class AST;
template <class ABSTRACT_VALUE> class Visitor;

template <class ABSTRACT_VALUE>
std::ostream& operator << (std::ostream& os, const Token<ABSTRACT_VALUE>& token);

template <class ABSTRACT_VALUE>
std::ostream& operator << (std::ostream& os, const AST<ABSTRACT_VALUE>& ast);

template <class ABSTRACT_VALUE>
class Token
{
public:
	Token(const unisim::util::lexer::Location& loc);
	Token(char c, const unisim::util::lexer::Location& loc, unsigned int lbp = 0);
	Token(const char *text, unsigned int id, const unisim::util::lexer::Location& loc, unsigned int lbp = 0);
	virtual ~Token();
	const char *GetText() const;
	unsigned int GetId() const;
	bool IsEndOfFile() const;
	unsigned int GetLBP() const;
	const unisim::util::lexer::Location& GetLocation() const;
	void SetValue(ABSTRACT_VALUE value);
	ABSTRACT_VALUE GetValue() const;
	
	friend std::ostream& operator << <ABSTRACT_VALUE>(std::ostream& os, const Token<ABSTRACT_VALUE>& token);
	
	virtual AST<ABSTRACT_VALUE> *led(Parser<ABSTRACT_VALUE> *parser, AST<ABSTRACT_VALUE> *left);
	virtual AST<ABSTRACT_VALUE> *nud(Parser<ABSTRACT_VALUE> *parser);
protected:
	std::string text;
	unsigned int id;
	unisim::util::lexer::Location loc;
	unsigned int lbp;  // left binding power
	ABSTRACT_VALUE value;
};

template <class ABSTRACT_VALUE>
class Visitor
{
public:
	virtual ~Visitor() {}
	virtual void Visit(AST<ABSTRACT_VALUE> *ast) = 0;
};

typedef enum
{
	DFS_PRE_ORDER,
	DFS_IN_ORDER,
	DFS_POST_ORDER
} DepthFirstSearchTraversalOrder;

template <class ABSTRACT_VALUE>
class AST
{
public:
	AST(Token<ABSTRACT_VALUE> *value);
	AST(Token<ABSTRACT_VALUE> *token, AST<ABSTRACT_VALUE> *first);
	AST(Token<ABSTRACT_VALUE> *token, AST<ABSTRACT_VALUE> *first, AST<ABSTRACT_VALUE> *second);
	AST(Token<ABSTRACT_VALUE> *token, AST<ABSTRACT_VALUE> *first, AST<ABSTRACT_VALUE> *second, AST<ABSTRACT_VALUE> *third);
	virtual ~AST();
	void GenerateGraphviz(const char *filename) const;
	void DFS(Visitor<ABSTRACT_VALUE> *visitor, DepthFirstSearchTraversalOrder order);
	
	const Token<ABSTRACT_VALUE> *GetToken() const;
	unsigned int GetArity() const;
	const AST<ABSTRACT_VALUE> *GetFirst() const;
	const AST<ABSTRACT_VALUE> *GetSecond() const;
	const AST<ABSTRACT_VALUE> *GetThird() const;
private:
	unsigned int id;
	unsigned int arity;
	Token<ABSTRACT_VALUE> *token;
	AST<ABSTRACT_VALUE> *children[3];
	friend std::ostream& operator << <ABSTRACT_VALUE> (std::ostream& os, const AST<ABSTRACT_VALUE>& ast);
	void GenerateGraphvizVertex(std::ostream& os) const;
	void GenerateGraphvizEdge(std::ostream& os) const;
	
	static unsigned int num_asts;
};

template <class ABSTRACT_VALUE>
class Parser : public unisim::util::lexer::Lexer<Token<ABSTRACT_VALUE> >
{
public:
	Parser(std::istream *stream, std::ostream& debug_info_stream, std::ostream& debug_warning_stream, std::ostream& debug_error_stream, bool debug = false);
	virtual ~Parser();
	
	AST<ABSTRACT_VALUE> *ParseExpression(unsigned int rbp = 0);
	
	bool Advance(unsigned int token_id = 0);
	
	void ErrorUnexpectedToken(const Token<ABSTRACT_VALUE> *token);
	void ErrorExpectedToken(unsigned int expected_token_id, const Token<ABSTRACT_VALUE> *token = 0);
	void ErrorExpectedExpression();
	void InternalError();
	std::ostream& GetDebugInfoStream();
	std::ostream& GetDebugWarningStream();
	std::ostream& GetDebugErrorStream();

	virtual bool Check(Token<ABSTRACT_VALUE> *token, AST<ABSTRACT_VALUE> *left, AST<ABSTRACT_VALUE> *right) = 0;
	virtual bool Check(Token<ABSTRACT_VALUE> *token, AST<ABSTRACT_VALUE> *child) = 0;
	virtual bool Check(Token<ABSTRACT_VALUE> *token) = 0;

	bool IsDebugging() const;
protected:
	bool parser_error;
	std::ostream& debug_info_stream;
	std::ostream& debug_warning_stream;
	std::ostream& debug_error_stream;
	bool debug;
	Token<ABSTRACT_VALUE> *token;
};

template <class ABSTRACT_VALUE>
class Literal : public Token<ABSTRACT_VALUE>
{
public:
	Literal(const char *text, unsigned int id, const unisim::util::lexer::Location& loc);
	virtual ~Literal() {}
	
	virtual AST<ABSTRACT_VALUE> *nud(Parser<ABSTRACT_VALUE> *parser);
private:
};

typedef enum
{
	LEFT_TO_RIGHT,
	RIGHT_TO_LEFT
} Associativity;

// expr <- expr OP expr
template <class ABSTRACT_VALUE>
class InfixOperator : virtual public Token<ABSTRACT_VALUE>
{
public:
	InfixOperator(char c, const unisim::util::lexer::Location& loc, unsigned int token_lbp, unsigned int bp, Associativity assoc);
	InfixOperator(const char *text, unsigned int id, const unisim::util::lexer::Location& loc, unsigned int token_lbp, unsigned int bp, Associativity assoc);
	
	// Indexing operator
	InfixOperator(char c, char end_of_subscript_c, const unisim::util::lexer::Location& loc, unsigned token_lbp);
	InfixOperator(const char *text, unsigned int id, unsigned int end_of_subscript_id, const unisim::util::lexer::Location& loc, unsigned token_lbp);

	virtual ~InfixOperator() {}

	virtual AST<ABSTRACT_VALUE> *led(Parser<ABSTRACT_VALUE> *parser, AST<ABSTRACT_VALUE> *left);
private:
	unsigned int bp;
	Associativity assoc;
	unsigned int end_of_subscript_id; // for an indexng operator (like '[')
};

// expr <- OP expr
template <class ABSTRACT_VALUE>
class PrefixOperator : virtual public Token<ABSTRACT_VALUE>
{
public:
	PrefixOperator(char c, const unisim::util::lexer::Location& loc, unsigned int token_lbp, unsigned bp);
	PrefixOperator(const char *text, unsigned int id, const unisim::util::lexer::Location& loc, unsigned int token_lbp, unsigned bp);
	
	// Grouping operator
	PrefixOperator(char c, char closing_c, const unisim::util::lexer::Location& loc);
	PrefixOperator(const char *text, unsigned int id, unsigned int closing_id, const unisim::util::lexer::Location& loc);
	
	virtual ~PrefixOperator() {}

	virtual AST<ABSTRACT_VALUE> *nud(Parser<ABSTRACT_VALUE> *parser);
private:
	unsigned int bp;
	unsigned int closing_id; // for a grouping operator (like '(')
};

template <class ABSTRACT_VALUE>
class PrefixInfixOperator : public PrefixOperator<ABSTRACT_VALUE>, public InfixOperator<ABSTRACT_VALUE>
{
public:
	PrefixInfixOperator(char c, const unisim::util::lexer::Location& loc, unsigned int token_lbp, unsigned int prefix_bp /* for nud */, unsigned int infix_bp /* for led */, Associativity infix_assoc);
	PrefixInfixOperator(const char *text, unsigned int id, const unisim::util::lexer::Location& loc, unsigned int token_lbp, unsigned int prefix_bp /* for nud */, unsigned int infix_bp /* for led */, Associativity infix_assoc);
private:
};

// expr <- expr OP
template <class ABSTRACT_VALUE>
class SuffixOperator : virtual public Token<ABSTRACT_VALUE>
{
public:
	SuffixOperator(char c, const unisim::util::lexer::Location& loc, unsigned int token_lbp);
	SuffixOperator(const char *text, unsigned int id, const unisim::util::lexer::Location& loc, unsigned int token_lbp);

	virtual ~SuffixOperator() {}

	virtual AST<ABSTRACT_VALUE> *led(Parser<ABSTRACT_VALUE> *parser, AST<ABSTRACT_VALUE> *left);
private:
};

template <class ABSTRACT_VALUE>
class PrefixSuffixOperator : public PrefixOperator<ABSTRACT_VALUE>, public SuffixOperator<ABSTRACT_VALUE>
{
public:
	PrefixSuffixOperator(char c, const unisim::util::lexer::Location& loc, unsigned int token_lbp, unsigned int prefix_bp /* for nud */);
	PrefixSuffixOperator(const char *text, unsigned int id, const unisim::util::lexer::Location& loc, unsigned int token_lbp, unsigned int prefix_bp /* for nud */);
private:
};

} // end of namespace parser
} // end of namespace util
} // end of namespace unisim

#endif
