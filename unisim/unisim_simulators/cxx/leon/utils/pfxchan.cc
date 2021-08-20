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
 
#include <utils/pfxchan.hh>
#include <ostream>
#include <cstdio>

namespace SSv8 {
  // Constructor
  PFXChan::PFXChan( char const* _prefix, std::ostream& _sink )
    : m_prefix( _prefix ), m_sink( _sink ), m_capacity( 0 ), m_size( 0 ), m_storage( 0 )
  {}
  
  // destructor
  PFXChan::~PFXChan() {
    delete [] m_storage;
  }
  
  // write one character
  PFXChan::int_type
  PFXChan::overflow( PFXChan::int_type _ch ) {
    if( _ch == EOF or _ch == 0 ) return _ch;
    
    if( (m_size+1) >= m_capacity ) {
      m_capacity = std::max<intptr_t>( 32, m_capacity*2 );
      char* nstorage = new char [m_capacity];
      for( intptr_t idx = m_size; (--idx) >= 0; ) nstorage[idx] = m_storage[idx];
      delete [] m_storage;
      m_storage = nstorage;
    }
    
    m_storage[m_size++] = _ch;
    if( _ch == '\n' ) {
      m_storage[m_size] = '\0';
      m_sink << m_prefix << m_storage << "\x1b[0m";
      m_size = 0;
    }
    return _ch;
  }

} // end of namespace SSv8
