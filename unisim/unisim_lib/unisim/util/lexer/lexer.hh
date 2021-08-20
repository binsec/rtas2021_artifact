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

#ifndef __UNISIM_UTIL_LEXER_LEXER_HH__
#define __UNISIM_UTIL_LEXER_LEXER_HH__

#include <unisim/util/dictionary/dictionary.hh>
#include <iosfwd>
#include <string>
#include <map>

namespace unisim {
namespace util {
namespace lexer {

class Location;
template <class TOKEN> class Lexer;

std::ostream& operator << (std::ostream& os, const Location& loc);

class Location
{
public:
	Location();
	void IncLineNo(unsigned int delta = 1);
	void IncColNo(unsigned int delta = 1);
	void SetFilename(const char *filename);
	
	const char *GetFilename() const;
	unsigned int GetLineNo() const;
	unsigned int GetColNo() const;
	
private:
	std::string filename;
	unsigned int lineno;
	unsigned int colno;
	
	friend std::ostream& operator << (std::ostream& os, const Location& loc);
};

template <class TOKEN>
class Lexer
{
public:
	static const unsigned int TOK_EOF = 256;
	
	Lexer(std::istream *stream, std::ostream& debug_info_stream, std::ostream& debug_warning_stream, std::ostream& debug_error_stream, bool debug = false);
	virtual ~Lexer();
	
	void EnableToken(const char *text, unsigned int id = 0);
	void EnableIdentifierToken(unsigned int id);
	void EnableDecimalIntegerToken(unsigned int id, bool enable_sign);
	void EnableDoubleQuotedStringToken(unsigned int id);
	void EnableEatingSpace();
	void EnableEatingTab();
	void EnableEatingEndOfLine();
	void FinishScanningLine();
	const char *GetLine() const;
	const char *GetText() const;
	const Location& GetLocation() const;
	const char *GetTokenText(unsigned int token_id) const;
	void PrintFriendlyLocation(const Location& loc) const;
	
	TOKEN *Next();
	
	virtual TOKEN *CreateToken(const char *text, unsigned int id, const Location& loc) = 0;
private:
	std::string line;
	std::string text;
	unisim::util::dictionary::Dictionary<unsigned int> token_dictionnary;
	std::map<unsigned int, std::string> token_map;
	bool enable_ident_token;
	unsigned int ident_token_id;
	bool enable_decimal_integer_token;
	bool enable_sign;
	unsigned int decimal_integer_token_id;
	bool enable_double_quoted_string_token;
	unsigned int double_quoted_string_token_id;
	bool enable_eating_space;
	bool enable_eating_tab;
	bool enable_eating_eol;
	std::istream *stream;
	Location loc;
	std::ostream& debug_info_stream;
	std::ostream& debug_warning_stream;
	std::ostream& debug_error_stream;
	bool debug;
	bool finished_scanning_line;
	
	bool IsDecimalDigit(char c) const;
	bool IsAlpha(char c) const;
	bool IsAlphaNumeric(char c) const;

	void ErrorUnknownToken(char c);
	void ErrorExpectedToken(const char *text);
	void ErrorIO();
protected:
	bool lexer_error;
};

} // end of namespace lexer
} // end of namespace util
} // end of namespace unisim

#endif
