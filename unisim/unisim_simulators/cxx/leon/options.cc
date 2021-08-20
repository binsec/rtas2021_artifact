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
 
#include <options.hh>
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

namespace Star {
  Option Options::table[Option::optionscount] = {
    {"startdisasm",  0, "instruction address where to start disassembly."},
    {"exitpoint",    0, "instruction address where to stop simulation."},
    {"maxinsts",     0, "instruction count before simulation stops."},
  };

  Option::operator uint32_t() const { return std::strtoul( m_value, 0, 0 ); }
  Option::operator  int64_t() const { return strtoll( m_value, 0, 0 ); }
  Option::operator bool() const {
    if( not m_value ) return false;
    if( not strcasecmp( m_value, "yes" ) or
        not strcasecmp( m_value, "true" ) or
        atol( m_value ) > 0 ) return true;
    return false;
  }
  
  int
  Options::parse( int _argc, char** argv ) {
    for( int aidx = 0; aidx < _argc; ) {
      char const* optname = argv[aidx++];
      if( optname[0] != '-' or optname[1] != '-' ) return aidx - 1;
      if( optname[2] == '\0' ) return aidx;
      if( not strcmp( optname, "--help" ) ) throw Options::Help;
      if( aidx >= _argc ) return aidx - 1;
      Option& opt = byname( optname + 2 );
      opt.m_value = argv[aidx++];
    }
    return 0;
  }
  
  Option&
  Options::byname( char const* _name ) {
    for( int idx = 0; idx < Option::optionscount; ++idx )
      if( not strcmp( _name, table[idx].m_name ) ) return table[idx];
    cerr << "No such option: " << _name << endl;
    throw Options::Error;
  }

  std::ostream&
  Options::dump( std::ostream& _sink ) {
    int ralign = 0;
    for( int idx = 0; idx < Option::optionscount; ++idx ) {
      int count = strlen( table[idx].m_name ) + 6;
      if( count > ralign ) ralign = count;
    }
    for( int idx = 0; idx < Option::optionscount; ++idx ) {
      int count = 0;
      for( char const* ptr = "  --";            *ptr; ptr++, count++ ) _sink << *ptr;
      for( char const* ptr = table[idx].m_name; *ptr; ptr++, count++ ) _sink << *ptr;
      for( char const* ptr = "  ";              *ptr; ptr++, count++ ) _sink << *ptr;
      for( ; count < ralign; count++ ) _sink << ' ';
      _sink << table[idx].m_description << endl;
    }
    
    return _sink;
  }
};
