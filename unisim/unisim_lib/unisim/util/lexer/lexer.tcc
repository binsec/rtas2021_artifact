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

#ifndef __UNISIM_UTIL_LEXER_LEXER_TCC__
#define __UNISIM_UTIL_LEXER_LEXER_TCC__

#include <unisim/util/dictionary/dictionary.hh>
#include <iostream>

namespace unisim {
namespace util {
namespace lexer {

template <class TOKEN>
Lexer<TOKEN>::Lexer(std::istream *_stream, std::ostream& _debug_info_stream, std::ostream& _debug_warning_stream, std::ostream& _debug_error_stream, bool _debug)
	: line()
	, text()
	, token_dictionnary(_debug_info_stream, _debug_warning_stream, _debug_error_stream, _debug)
	, token_map()
	, enable_ident_token(false)
	, ident_token_id(0)
	, enable_decimal_integer_token(false)
	, enable_sign(false)
	, decimal_integer_token_id(0)
	, enable_double_quoted_string_token(false)
	, double_quoted_string_token_id(0)
	, enable_eating_space(false)
	, enable_eating_tab(false)
	, enable_eating_eol(false)
	, stream(_stream)
	, loc()
	, debug_info_stream(_debug_info_stream)
	, debug_warning_stream(_debug_warning_stream)
	, debug_error_stream(_debug_error_stream)
	, debug(_debug)
	, finished_scanning_line(false)
	, lexer_error(false)
{
}

template <class TOKEN>
Lexer<TOKEN>::~Lexer()
{
}

template <class TOKEN>
void Lexer<TOKEN>::EnableToken(const char *text, unsigned int id)
{
	if(!id)
	{
		id = *text;
	}
	
	token_dictionnary.Add(text, id);
	token_map[id] = text;
}

template <class TOKEN>
void Lexer<TOKEN>::EnableIdentifierToken(unsigned int id)
{
	enable_ident_token = true;
	ident_token_id = id;
	token_map[id] = "identifier";
}

template <class TOKEN>
void Lexer<TOKEN>::EnableDecimalIntegerToken(unsigned int id, bool _enable_sign)
{
	enable_decimal_integer_token = true;
	enable_sign = _enable_sign;
	decimal_integer_token_id = id;
	token_map[id] = "decimal integer";
}

template <class TOKEN>
void Lexer<TOKEN>::EnableDoubleQuotedStringToken(unsigned int id)
{
	enable_double_quoted_string_token = true;
	double_quoted_string_token_id = id;
	token_map[id] = "double quoted string";
}

template <class TOKEN>
void Lexer<TOKEN>::EnableEatingSpace()
{
	enable_eating_space = true;
}

template <class TOKEN>
void Lexer<TOKEN>::EnableEatingTab()
{
	enable_eating_tab = true;
}

template <class TOKEN>
void Lexer<TOKEN>::EnableEatingEndOfLine()
{
	enable_eating_eol = true;
}

template <class TOKEN>
bool Lexer<TOKEN>::IsDecimalDigit(char c) const
{
	return (c >= '0') && (c <= '9');
}

template <class TOKEN>
bool Lexer<TOKEN>::IsAlpha(char c) const
{
	return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_');
}

template <class TOKEN>
bool Lexer<TOKEN>::IsAlphaNumeric(char c) const
{
	return IsDecimalDigit(c) || IsAlpha(c);
}

template <class TOKEN>
TOKEN *Lexer<TOKEN>::Next()
{
	text.clear();
	
	if(stream->eof())
	{
		return CreateToken("end of input", TOK_EOF, loc);
	}
	
	if(stream->fail())
	{
		ErrorIO();
		return 0;
	}
	
	int state = 0;
	char c;
	
	TOKEN *token = 0;
	bool unknown_token = false;
	bool has_sign = false;
	
	do
	{
		switch(state)
		{
			case 0:
				{
					unisim::util::dictionary::DictionaryEntry<unsigned int> *entry = token_dictionnary.FindDictionaryEntry(stream, text);
					if(entry)
					{
						line += text;
						unsigned int token_id = entry->GetValue();
						token = CreateToken(text.c_str(), token_id, loc); 
						unsigned int n = text.length();
						unsigned int i;
						for(i = 0; i < n; i++)
						{
							c = text[i];
							if((c == '\n') || (c == '\r'))
							{
								line.clear();
								loc.IncLineNo();
								finished_scanning_line = false;
							}
						}
					}
					
					if(token) break;
					
					if(stream->get(c).eof())
					{
						token =  CreateToken("end of input", TOK_EOF, loc);
						break;
					}
					if(stream->fail())
					{
						ErrorIO();
						return 0;
					}
					if(debug)
					{
						debug_info_stream << state << ":get '" << c << "'" << std::endl;
					}

					if((enable_eating_space && (c == ' ')) ||
					   (enable_eating_tab && (c == '\t')))
					{
						loc.IncColNo();
						if(debug)
						{
							debug_info_stream << state << ":eat" << std::endl;
						}
						break;
					}
					
					if(enable_eating_eol && ((c == '\n') || (c == '\r')))
					{
						line.clear();
						loc.IncLineNo();
						finished_scanning_line = false;
						if(debug)
						{
							debug_info_stream << state << ":eat" << std::endl;
						}
						break;
					}

					if(enable_double_quoted_string_token && (c == '\"'))
					{
						// start of double quoted string
						line += c;
						text += c;
						state = 1;
						break;
					}
					
					if(enable_decimal_integer_token)
					{
						has_sign = enable_sign && (c == '-');
						
						if(has_sign || IsDecimalDigit(c))
						{
							// start of decimal integer
							line += c;
							text += c;
							state = 2;
							break;
						}
					}

					if(enable_ident_token && IsAlpha(c))
					{
						// start of identifier
						line += c;
						text += c;
						state = 3;
						break;
					}

					if(debug)
					{
						debug_info_stream << state << ":putback '" << c << "'" << std::endl;
					}
					stream->putback(c);
				}
				if(!token) unknown_token = true;
				break;
			case 1: // content of double quoted string
				if(!stream->get(c).good())
				{
					ErrorExpectedToken("\"");
					return 0; // unfinished character string
				}
				line += c;
				text += c;
				if(debug)
				{
					debug_info_stream << state << ":get '" << c << "'" << std::endl;
				}
				
				if(c == '\"')
				{
					// end of double quoted string
					token = CreateToken(text.c_str(), double_quoted_string_token_id, loc);
					break;
				}
				break;
			case 2: // decimal integer
				stream->get(c);

				if(debug)
				{
					if(stream->good())
					{
						debug_info_stream << state << ":get '" << c << "'" << std::endl;
					}

				}
				if(stream->good() && IsDecimalDigit(c))
				{
					line += c;
					text += c;
				}
				else
				{
					if(stream->good() && has_sign && (text.length() < 2))
					{
						// a decimal digit is expected after '-'
						stream->putback(c);
						stream->putback('-');
						c = '-';
						unknown_token = true;
						break;
					}
					
					if(debug)
					{
						if(stream->good()) debug_info_stream << state << ":get '" << c << "'" << std::endl;
					}

					if(stream->eof())
					{
						stream->clear(); // clear state if we encountered an eof
					}
					else
					{
						if(debug)
						{
							debug_info_stream << state << ":putback '" << c << "'" << std::endl;
						}

						stream->putback(c);
						if(stream->fail())
						{
							ErrorIO();
							return 0;
						}
					}
					
					token = CreateToken(text.c_str(), decimal_integer_token_id, loc);
					break;
				}
				break;
			case 3: // identifier
				stream->get(c);
				
				if(stream->good() && IsAlphaNumeric(c))
				{
					if(debug)
					{
						debug_info_stream << state << ":get '" << c << "'" << std::endl;
					}
					
					line += c;
					text += c;
				}
				else
				{
					if(debug)
					{
						if(stream->good()) debug_info_stream << state << ":get '" << c << "'" << std::endl;
					}
					
					if(stream->eof())
					{
						stream->clear(); // clear state if we encountered an eof
					}
					else
					{
						if(debug)
						{
							debug_info_stream << state << ":putback '" << c << "'" << std::endl;
						}
						
						stream->putback(c);
						if(stream->fail())
						{
							ErrorIO();
							return 0;
						}
					}
					token = CreateToken(text.c_str(), ident_token_id, loc);
					break;
				}
				break;
		}
	}
	while(!token && !unknown_token);
	
	if(unknown_token)
	{
		ErrorUnknownToken(c);
	}

	loc.IncColNo(text.length());

	return token;
}

template <class TOKEN>
void Lexer<TOKEN>::FinishScanningLine()
{
	if(!finished_scanning_line)
	{
		char c;
		while(stream->get(c))
		{
			if((c == '\n') || (c == '\r')) break;
			line += c;
		}
		finished_scanning_line = true;
	}
}

template <class TOKEN>
const char *Lexer<TOKEN>::GetLine() const
{
	return line.c_str();
}

template <class TOKEN>
const char *Lexer<TOKEN>::GetText() const
{
	return text.c_str();
}

template <class TOKEN>
const Location& Lexer<TOKEN>::GetLocation() const
{
	return loc;
}

template <class TOKEN>
const char *Lexer<TOKEN>::GetTokenText(unsigned int token_id) const
{
	std::map<unsigned int, std::string>::const_iterator it = token_map.find(token_id);
	return (it != token_map.end()) ? (*it).second.c_str() : "";
}

template <class TOKEN>
void Lexer<TOKEN>::PrintFriendlyLocation(const Location& _loc) const
{
	debug_info_stream << line << std::endl;
	unsigned int colno = _loc.GetColNo();
	while(--colno)
	{
		debug_info_stream << " ";
	}
	debug_info_stream << "^" << std::endl;
}

template <class TOKEN>
void Lexer<TOKEN>::ErrorUnknownToken(char c)
{
	FinishScanningLine();
	lexer_error = true;
	PrintFriendlyLocation(loc);
	debug_error_stream << loc << ", unknown token '" << c << "'" << std::endl; 
}

template <class TOKEN>
void Lexer<TOKEN>::ErrorExpectedToken(const char *_text)
{
	FinishScanningLine();
	lexer_error = true;
	PrintFriendlyLocation(loc);
	debug_error_stream << loc << ", expected token '" << _text << "'" << std::endl; 
}

template <class TOKEN>
void Lexer<TOKEN>::ErrorIO()
{
	lexer_error = true;
	PrintFriendlyLocation(loc);
	debug_error_stream << loc << ", I/O error" << std::endl; 
}


} // end of namespace lexer
} // end of namespace util
} // end of namespace unisim


#endif
