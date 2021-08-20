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
 
#ifndef SSV8_OPTIONS_HPP
#define SSV8_OPTIONS_HPP

#include <inttypes.h>
#include <iosfwd>

namespace SSv8 {
  struct Option {
    char const*                 m_name;
    char const*                 m_value;
    char const*                 m_desc;
  private:
    Option*                     m_next;
    
  public:
    enum Exception_t { Help = 0, Error };
    
    Option( char const* _name, char const* _value, char const* _desc )
      : m_name( _name ), m_value( _value ), m_desc( _desc ), m_next( s_optlist )
    { s_optlist = this; }
    
    char const*                 str() const { return m_value; }
    uint32_t                    ui32( uint32_t _default ) const;
    int64_t                     si64( int64_t _default ) const;
    bool                        yesno( bool _default ) const;
    
  private:
    static Option*              s_optlist;

  public:
    static int                  parse( int _args, char** argv );
    static std::ostream&        dump( std::ostream& _sink );
  };
};

#endif // STAR_OPTIONS_HPP
