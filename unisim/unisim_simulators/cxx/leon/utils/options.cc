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
 
#include <utils/options.hh>
#include <iostream>
#include <map>
#include <cstdlib>
#include <strings.h>
#include <cstring>

namespace SSv8 {
  Option* Option::s_optlist;

  uint32_t Option::ui32( uint32_t _default ) const { return m_value ? std::strtoul( m_value, 0, 0 ) : _default; }
  int64_t  Option::si64( int64_t _default ) const { return m_value ? std::strtoll( m_value, 0, 0 ) : _default; }
  
  bool
  Option::yesno( bool _default ) const {
    if( not m_value ) return _default;
    if( strcasecmp( m_value, "yes" ) == 0 ) return true;
    if( strcasecmp( m_value, "no" ) == 0 ) return false;
    return _default;
  }
  
  // String Less Than Operator
  struct StrLT { bool operator()( char const* _s1, char const* _s2 ) const { return strcmp( _s1, _s2 ) < 0; } };
  
  int
  Option::parse( int _argc, char** argv ) {
    typedef std::map<char const*,Option*,StrLT> OptMap_t;
    OptMap_t optmap;
    
    for( Option* opt = s_optlist; opt; opt = opt->m_next )
      optmap[opt->m_name] = opt;
    
    for( int aidx = 0; aidx < _argc; ) {
      char const* optname = argv[aidx];
      if( optname[0] != '-' or optname[1] != '-' ) return aidx;
      aidx += 1;
      if( optname[2] == '\0' ) return aidx;
      if( strcmp( optname, "--help" ) == 0 ) throw Option::Help;
      if( aidx >= _argc ) return aidx - 1;
      OptMap_t::iterator opt = optmap.find( optname + 2 );
      if( opt == optmap.end() ) {
        std::cerr << "No such option: " << optname << std::endl;
        throw Option::Error;
      }
      opt->second->m_value = argv[aidx++];
    }
    std::cerr << "Unexpected end of command line arguments." << std::endl;
    throw Error;
    return 0;
  }
  
  std::ostream&
  Option::dump( std::ostream& _sink ) {
    int ralign = 0;
    for( Option* opt = s_optlist; opt; opt = opt->m_next ) {
      int count = strlen( opt->m_name ) + 6;
      if( count > ralign ) ralign = count;
    }
    
    for( Option* opt = s_optlist; opt; opt = opt->m_next ) {
      int count = 0;
      for( char const* ptr = "  --";      *ptr; ptr++, count++ ) _sink << *ptr;
      for( char const* ptr = opt->m_name; *ptr; ptr++, count++ ) _sink << *ptr;
      for( char const* ptr = "  ";        *ptr; ptr++, count++ ) _sink << *ptr;
      for( ; count < ralign; count++ ) _sink << ' ';
      _sink << opt->m_desc << std::endl;
    }
    
    return _sink;
  }
};
