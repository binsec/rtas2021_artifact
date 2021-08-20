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
 
#ifndef SSV8_SYS_ELFLOADER_HH
#define SSV8_SYS_ELFLOADER_HH

#include <inttypes.h>
//#include <sparc/elf.h>
#include <iosfwd>

namespace SSv8 {
  
  struct BigEndianReader {
    uint8_t const*         m_base;
    intptr_t              m_size;
    
    enum Exception_t { BoundsError };
    BigEndianReader() : m_base( 0 ), m_size( 0 ) {}
    BigEndianReader( uint8_t const* _base, intptr_t _size ) : m_base( _base ), m_size( _size ) {}
    BigEndianReader( BigEndianReader const& _ber, intptr_t _base, intptr_t _size );
    
    BigEndianReader&          init( uint8_t const* _base, intptr_t _size )
    { m_base = _base; m_size = _size; return *this; }
    BigEndianReader&          init( BigEndianReader const& _ber, intptr_t _base, intptr_t _size );
    
    template <class Integer_t>
    Integer_t                 read( intptr_t _addr ) const {
      if( _addr < 0 or intptr_t(_addr + sizeof( Integer_t )) > m_size )
        throw BoundsError;
      
      Integer_t integer = 0;
      
      for( intptr_t idx = 0; idx < intptr_t( sizeof( Integer_t ) ); ++idx )
        integer = (integer << 8 | Integer_t( m_base[_addr + idx] ));
      return integer;
    }
    
    void                      read( intptr_t _addr, uint8_t* _buffer, intptr_t _size ) const;
    int                       cmp( intptr_t _addr, uint8_t const* _buffer, intptr_t _size ) const;
    char const*               cstring( intptr_t idx ) const;
    
    void                      setsize( intptr_t _size );
  };
  
  enum Exception_t { ImageError, SectionHeaderError };
 
  enum Version_t { versnone = 0, verscurrent = 1, versnum = 2 };
  
  struct Image : public BigEndianReader {
    
    Image( uint8_t const* _start, intptr_t _upbound );
    
    uint32_t                  entrypoint() const { return this->read<uint32_t>( 24 ); }
    uint32_t                  phoff() const { return this->read<uint32_t>( 28 ); }
    uint32_t                  shoff() const { return this->read<uint32_t>( 32 ); }
    uint16_t                  phentsize() const { return this->read<uint16_t>( 42 ); }
    uint16_t                  phnum() const { return this->read<uint16_t>( 44 ); }
    uint16_t                  shentsize() const { return this->read<uint16_t>( 46 ); }
    uint16_t                  shnum() const { return this->read<uint16_t>( 48 ); }
    uint16_t                  shstrndx() const { return this->read<uint16_t>( 50 ); }
    
    static int const          s_iclass = 4;
    static int const          s_idata = 5;
    static int const          s_iversion = 6;
    
    static char const* const  s_elfmag; /* "\177ELF" */
    enum IClass_t { classnone = 0, class32 = 1, class64 = 2, classnum = 3 };
    enum IData_t  { datanone = 0, data2lsb = 1, data2msb = 2, datanum = 3 };
    enum Type_t { etnone = 0, etrel = 1, etexec = 2, etdyn = 3, etcore = 4, etnum = 5 };
  };
  
  struct SectionHeader : public BigEndianReader {
    SectionHeader( Image const& _node, intptr_t _idx );
    
    uint32_t                  name() const { return this->read<uint32_t>( 0 ); }       /* section name */
    uint32_t                  type() const { return this->read<uint32_t>( 4 ); }       /* SHT_... */
    uint32_t                  flags() const { return this->read<uint32_t>( 8 ); }      /* SHF_... */
    uint32_t                  addr() const { return this->read<uint32_t>( 12 ); }      /* virtual address */
    uint32_t                  offset() const { return this->read<uint32_t>( 16 ); }    /* file offset */
    uint32_t                  size() const { return this->read<uint32_t>( 20 ); }      /* section size */
    uint32_t                  link() const { return this->read<uint32_t>( 24 ); }      /* misc info */
    uint32_t                  info() const { return this->read<uint32_t>( 28 ); }      /* misc info */
    uint32_t                  addralign() const { return this->read<uint32_t>( 32 ); } /* memory alignment */
    uint32_t                  entsize() const { return this->read<uint32_t>( 36 ); }   /* entry size if table */
    
  };
  
  struct StringTable : public BigEndianReader {
    StringTable( Image const& _img );
  };
  
  struct ProgramHeader : public BigEndianReader {
    enum PType_t { null = 0, load = 1, dynamic = 2, interp = 3, note = 4, shlib = 5, phdr = 6, tls = 7, num = 8 };
    
    ProgramHeader( Image const& _node, intptr_t _idx );
    
    uint32_t                  type() const { return this->read<uint32_t>( 0 ); }
    uint32_t                  offset() const { return this->read<uint32_t>( 4 ); }
    uint32_t                  vaddr() const { return this->read<uint32_t>( 8 ); }
    uint32_t                  paddr() const { return this->read<uint32_t>( 12 ); }
    uint32_t                  filesz() const { return this->read<uint32_t>( 16 ); }
    uint32_t                  memsz() const { return this->read<uint32_t>( 20 ); }
    uint32_t                  flags() const { return this->read<uint32_t>( 24 ); }
    uint32_t                  align() const { return this->read<uint32_t>( 28 ); }
  };
};

#endif // SSV8_SYS_ELFLOADER_HH
