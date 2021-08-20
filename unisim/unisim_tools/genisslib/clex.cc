/*
 *  Copyright (c) 2018,
 *  Commissariat a l'Energie Atomique (CEA),
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */
 
#include <clex.hh>
#include <iostream>
#include <sstream>

namespace CLex
{
  char const*
  Scanner::nextName( Scanner::Next n )
  {
    switch (n)
      {
      case ObjectOpening: return "ObjectOpening";
      case ObjectClosing: return "ObjectClosing";
      case ArrayOpening: return "ArrayOpening";
      case ArrayClosing: return "ArrayClosing";
      case GroupOpening: return "GroupOpening";
      case GroupClosing: return "GroupClosing";
      case StringQuotes: return "StringQuotes";
      case Number: return "Number";
      case Name: return "Name";
      case Comma: return "Comma";
      case Colon: return "Colon";
      case Assign: return "Assign";
      case Star: return "Star";
      case Dot: return "Dot";
      case SemiColon: return "SemiColon";
      case QuestionMark: return "QuestionMark";
      case Less: return "Less";
      case More: return "More";
      case EoF: return "EoF";
      }
    return "???";
  }

  bool
  Scanner::nextchar()
  {
    if (putback) { putback = false; return sourcegood(); }
      
    if (lch == '\n')  { lidx += 1; cidx = 0; }
    else              { cidx += 1; }

    return sourceget(lch);
  }

  Scanner::Unexpected
  Scanner::unexpected() const
  {
    std::cerr << loc() << "error: unexpected "  << nextName( lnext ) << std::endl;
    return Unexpected();
  }

  Scanner::Unexpected
  Scanner::syntax_error() const
  {
    std::cerr << loc() << "syntax error" << std::endl;
    return Unexpected();
  }

  char Scanner::getchar()
  {
    if (not nextchar())
      { lnext = EoF; throw unexpected(); }
    return lch;
  }

  Scanner::Next
  Scanner::whatsnext() const
  {
    if (isalpha(lch)) return Name;
    if (isdigit(lch)) return Number;
    switch (lch)
      {
      default:     throw syntax_error();
      case '"':    return StringQuotes;
      case '{':    return ObjectOpening;
      case '}':    return ObjectClosing;
      case '[':    return ArrayOpening;
      case ']':    return ArrayClosing;
      case '(':    return GroupOpening;
      case ')':    return GroupClosing;
      case ',':    return Comma;
      case ':':    return Colon;
      case '-':    return Number;
      case '_':    return Name;
      case '=':    return Assign;
      case '*':    return Star;
      case '.':    return Dot;
      case '<':    return Less;
      case '>':    return More;
      case ';':    return SemiColon;
      case '?':    return QuestionMark;
      }
    return EoF;
  }

  Scanner::Next
  Scanner::next()
  {
    while (nextchar())
      {
        if (isspace(lch))
          continue;

        switch (lch)
          {
          default: return lnext = whatsnext();
          case '/':
          case '#':
            /* skipping comments */
            switch (lch == '#' ? '/' : getchar())
              {
              case '*': for (;;) { if (getchar() != '*') continue; do {} while (getchar() == '*'); if (lch == '/') break; } break;
              case '/': do {} while (getchar() != '\n'); break;
              default: throw syntax_error();
              }
            break;
          }
      }
    return lnext = EoF;
  }

  std::ostream&
  Scanner::loc( std::ostream& sink ) const
  {
    sink << sourcename() << ":" << lidx << ":" << cidx << ": ";
    return sink;
  }

  bool
  Scanner::get_name( Sink& name )
  {
    do { if (not name.append(lch)) return false; }
    while (nextchar() and (isalnum(lch) or lch == '_'));
    putback = true;
    return true;
  }
  
  bool
  Scanner::get_number( Sink& number )
  {
    if (lch == '-') { if (not number.append(lch)) return false; getchar(); }
      
    if (lch == '0')
      {
        if (not number.append('0')) return false;
        if (nextchar() and isdigit(lch)) throw syntax_error();
      }
    else if (isdigit(lch))
      {
        do { if (not number.append(lch)) return false; } while (nextchar() and isdigit(lch));
      }
    else
      throw syntax_error();

    if (not sourcegood()) { putback = true; return true; }
      
    if (lch == '.')
      {
        if (not number.append('.')) return false;
        if (not isdigit(getchar())) throw syntax_error();
        do { if (not number.append(lch)) return false; } while (nextchar() and isdigit(lch));
        if (not sourcegood()) { putback = true; return true; }
      }
      
    if (lch == 'e' or lch == 'E')
      {
        if (not number.append(lch)) return false;
        getchar();
        if (lch == '-' or lch == '+') { if (not number.append(lch)) return false; getchar(); }
        if (not isdigit(lch)) throw syntax_error();
        do { if (not number.append(lch)) return false; } while (nextchar() and isdigit(lch));
      }
      
    putback = true;
    return true;
  }

  double
  Scanner::get_double()
  {
    BlocSink<42> num;
      
    if (not get_number(num) or not num.append('\0'))
      {
        std::cerr << loc() << " double parse buffer overflow\n";
        throw Unexpected();
      }
    double res;
    std::istringstream in( &num.buffer[0] );
    in >> res;
    return res;
  }
    
  std::string
  Scanner::get_double_as_string()
  {
    BlocSink<42> num;
      
    if (not get_number(num) or not num.append('\0'))
      {
        std::cerr << loc() << " double parse buffer overflow\n";
        throw Unexpected();
      }
    return &num.buffer[0];
  }
    
  uint64_t
  Scanner::get_u64()
  {
    BlocSink<22> num;

    if (not get_number(num) or not num.append('\0'))
      {
        std::cerr << loc() << " u64 parse buffer overflow\n";
        throw Unexpected();
      }

    char* end;
    uint64_t res = strtoull(&num.buffer[0], &end, 10);
    if (*end)
      {
        std::cerr << loc() << " u64 parse error: " << end << std::endl;
        throw Unexpected();
      }
      
    return res;
  }

  int64_t
  Scanner::get_s64()
  {
    BlocSink<22> num;

    if (not get_number(num) or not num.append('\0'))
      {
        std::cerr << loc() << " s64 parse buffer overflow\n";
        throw Unexpected();
      }

    char* end;
    int64_t res = strtoll(&num.buffer[0], &end, 10);
    if (*end)
      {
        std::cerr << loc() << " u64 parse error: " << end << std::endl;
        throw Unexpected();
      }
      
    return res;
  }

  bool
  Scanner::get_string(Sink& s)
  {
    for (bool cont = true; cont;)
      {
        switch (getchar())
          {
          default: if (not s.append(lch)) return false; break;
          case '"': cont = false; break;
          case '\n': std::cerr << loc() << "error: unexpected end of line\n"; throw 0;
          case '\\':
            switch (getchar())
              {
              default: throw syntax_error();
              case '"': case '\\': if (not s.append(lch)) return false; break;
              case '\n': break;
              case 'n': if (not s.append('\n')) return false; break;
              }
            break;
          }
      }
    return true;
  }

  bool
  Scanner::expect( char const* s, bool (Scanner::*member)(Sink&) )
  {
    struct _ : Sink
    {
      _( char const* _expected ) : expected(_expected) {} char const* expected;
      bool append( char ch ) { return ch == *expected++; }
    } mandatory(s);
    return (this->*member)( mandatory );
  }

  void
  Scanner::get( std::string& s, bool (Scanner::*member)(Sink&) )
  {
    s.clear();
    struct _ : Sink
    {
      _( std::string& _buffer ) : buffer(_buffer) {} std::string& buffer;
      bool append( char ch )  { buffer += ch; return true; }
    } fetch(s);
    if (not (this->*member)( fetch ))
      throw Unexpected();
  }
}

