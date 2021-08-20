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
 
#ifndef __CLEX_HH__
#define __CLEX_HH__

#include <string>
#include <inttypes.h>

namespace CLex
{
  struct Scanner
  {
    Scanner() : lidx(0), cidx(0), lch('\n'), putback(false) {}

    virtual char const* sourcename() const = 0;
    virtual bool sourcegood() const = 0;
    virtual bool sourceget( char& lch ) = 0;
    virtual ~Scanner() {}
    
    enum Next
      {
        ObjectOpening, ObjectClosing,
        ArrayOpening, ArrayClosing,
        GroupOpening, GroupClosing,
        StringQuotes, Number, Name,
        Comma, Colon, Assign, Star, Dot,
        Less, More,
        SemiColon, QuestionMark,
        EoF
      };

    static char const* nextName( Scanner::Next n );

    bool nextchar();

    struct Unexpected {};
    Unexpected unexpected() const;
    Unexpected syntax_error() const;

    char getchar();

    Next whatsnext() const;
    Next next();

    struct Loc
    {
      Loc( Scanner const& scanner ) : s(scanner) {} Scanner const& s;
      friend std::ostream& operator << (std::ostream& sink, Loc const& el ) { return el.s.loc(sink); return sink; }
    };
    Loc loc() const { return Loc(*this); }
    std::ostream& loc( std::ostream& ) const;

    struct Sink { virtual bool append( char ch ) = 0; virtual ~Sink() {} };
    
    bool get_name( Sink& );
    
    bool get_number( Sink& );
    
    double get_double();
    std::string get_double_as_string();
    uint64_t get_u64();
    int64_t  get_s64();
    
    bool get_string( Sink& );

    bool expect( char const* s, bool (Scanner::*member)(Sink&) );
    void get( std::string& s, bool (Scanner::*member)(Sink&) );
    
    uintptr_t lidx, cidx;
    Next lnext;
    char lch;
    bool putback;
  };

  template <uintptr_t SZ>
  struct BlocSink : Scanner::Sink
  {
    BlocSink() : idx(0) {}
    bool append( char ch )
    {
      buffer[idx++] = ch;
      return idx < sizeof(buffer);
    };

    char const* begin() const { return &buffer[0]; }
    char const* end() const { return &buffer[idx]; }

    char buffer[SZ];
    unsigned idx;
  };

} /* end of namespace CLEX */

#endif /* __CLEX_HH__ */
