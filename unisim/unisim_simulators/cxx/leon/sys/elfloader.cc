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
 
#include <sys/elfloader.hh>
#include <cstring>

namespace SSv8 {
  BigEndianReader::BigEndianReader( BigEndianReader const& _ber, intptr_t _base, intptr_t _size )
  { init( _ber, _base, _size ); }
  
  BigEndianReader&
  BigEndianReader::init( BigEndianReader const& _ber, intptr_t _base, intptr_t _size ) {
    if( _base < 0 or (_base + _size) > _ber.m_size )
      throw BoundsError;
    m_base = _ber.m_base + _base;
    m_size = _size;
    return *this;
  }

  void
  BigEndianReader::read( intptr_t _addr, uint8_t* _buffer, intptr_t _size ) const {
    if( _addr < 0 or (_addr + _size) > m_size )
      throw BoundsError;
    memcpy( (void*)_buffer, (void*)(m_base + _addr), _size );
  }

  int
  BigEndianReader::cmp( intptr_t _addr, uint8_t const* _buffer, intptr_t _size ) const {
    if( _addr < 0 or (_addr + _size) > m_size )
      throw BoundsError;
    for( intptr_t idx = 0; idx < _size; idx++ ) {
      uint8_t a = m_base[_addr+idx], b = _buffer[idx];
      if( a == b ) continue;
      return (a > b ? 1 : -1);
    }
    return 0;
  }
  
  char const*
  BigEndianReader::cstring( intptr_t _addr ) const {
    if( _addr < 0 ) throw BoundsError;
    intptr_t upbound = _addr;
    do { if( upbound >= m_size ) throw BoundsError; } while( m_base[upbound++] != 0 );
    return (char const*)(m_base + _addr);
  }
  
  char const* const Image::s_elfmag = "\177ELF";
  
  Image::Image( uint8_t const* _start, intptr_t _size )
    : BigEndianReader( _start, _size )
  {
    /* Constructor */
    if( this->cmp( 0, (uint8_t const*)s_elfmag, 4 ) // magic
        or this->read<uint8_t>( s_iclass ) != class32 // 32 bits
        or this->read<uint8_t>( s_idata ) != data2msb // endianness
        or this->read<uint8_t>( s_iversion ) != verscurrent // elf version
        //        or this->read<uint8_t>( EI_OSABI ) != ELFOSABI_SOLARIS // solaris
        )
      throw ImageError;
    
    if( this->read<uint16_t>( 16 ) != etexec // type
        or this->read<uint16_t>( 18 ) != 2 // machine (2 = EM_SPARC, see <elf.h>)
        or this->read<uint32_t>( 20 ) != verscurrent // version
        or this->read<uint32_t>( 36 ) != 0 // processor-specific
        or this->read<uint16_t>( 40 ) < 52 // header size
        )
      throw ImageError;
  }
  
  SectionHeader::SectionHeader( Image const& _img, intptr_t _idx )
    : BigEndianReader( _img, _img.shoff() + _idx*_img.shentsize(), _img.shentsize() )
  {
    if( m_size != 40 )
      throw BoundsError;
  }
  
  StringTable::StringTable( Image const& _img ) {
    SSv8::SectionHeader shstrtab( _img, _img.shstrndx() );
    intptr_t offset = shstrtab.offset(), size = shstrtab.size();
    if( offset < 0 or (offset + size) > _img.m_size )
      throw BoundsError;
    BigEndianReader::init( _img, offset, size );
  }
  
  ProgramHeader::ProgramHeader( Image const& _img, intptr_t _idx )
    : BigEndianReader( _img, _img.phoff() + _idx*_img.phentsize(), _img.phentsize() )
  {
    if( m_size != 32 or intptr_t(_img.phoff() + (_idx + 1)*_img.phentsize()) > _img.m_size )
      throw BoundsError;
  }
  
};
