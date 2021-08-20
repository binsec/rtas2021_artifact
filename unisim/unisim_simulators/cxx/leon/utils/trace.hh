/*
 *  Copyright (c) 2007-2020,
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
 
#ifndef SSV8_UTILS_TRACE_HPP
#define SSV8_UTILS_TRACE_HPP

#include <inttypes.h>
#include <map>
#include <iosfwd>

namespace SSv8 {
  struct Trace {
    struct Sink_t {
      std::ostream*             m_stream;
      Sink_t() : m_stream( 0 ) {}
      ~Sink_t();
      Sink_t&                   init( std::streambuf* _sb );
    };
    
    struct Less {
      bool operator()( char const* _s1, char const* _s2 ) const
      { while( *_s1 != '\0' and *_s1 == *_s2 ) ++_s1, ++_s2; return *_s1 < *_s2; }
    };
    
    typedef std::map<char const*, Sink_t, Less> Channels_t;
    static Channels_t           s_chans;
    static std::ostream&        initchan( char const* _name, std::streambuf* _sb = 0 )
    { return *s_chans[_name].init( _sb ).m_stream; }
    static std::ostream&        chan( char const* _name )
    { std::ostream* stream = s_chans[_name].m_stream; return stream ? *stream : defaultchan( _name ); }
    static std::ostream&        defaultchan( char const* _name );
    
    enum Exception_t { KeyError };
  };
  
};

#endif // SSV8_UTILS_TRACE_HPP
