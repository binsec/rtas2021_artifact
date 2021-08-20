/*
 *  Copyright (c) 2007-2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY 
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_MEMORY_SPARSE_MEMORY_HH__
#define __UNISIM_COMPONENT_CXX_MEMORY_SPARSE_MEMORY_HH__

#include <algorithm>
#include <cstring>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace memory {
namespace sparse {

  struct NoMemSet { void operator() ( uint8_t* base, uintptr_t size ) const {} };
  
  template <typename addrT, addrT LoTagSizeT, addrT HiTagSizeT, typename _MemSet = NoMemSet>
  struct Memory
  {
    typedef addrT address_t;
    
    _MemSet _memset;
    
    Memory()
      : _memset()
    {
      for (intptr_t idx = 0; idx < (1<<s_bits); ++idx) {
        m_pages[idx] = 0;
      }
    }
    
    Memory( _MemSet const& __memset )
      : _memset( __memset )
    {
      for (intptr_t idx = 0; idx < (1<<s_bits); ++idx) {
        m_pages[idx] = 0;
      }
    }
    
    ~Memory()
    {
      for (intptr_t idx = 0; idx < (1<<s_bits); ++idx) {
        delete m_pages[idx];
      }
    }
    
    // Paginated content
    struct Page
    {
      static address_t const    s_bits = LoTagSizeT;
      
      Page*                     m_next;
      address_t                 m_address;
      uint8_t                   m_storage[1<<s_bits];

      Page( Page* _next, address_t _address, _MemSet const& _memset )
        : m_next( _next ), m_address( _address & (address_t(-1) << s_bits) )
      { _memset( &m_storage[0], 1<<s_bits ); }
      ~Page() { delete m_next; }
    };

    static address_t const      s_bits = HiTagSizeT;
    Page*                       m_pages[1 << s_bits];
    
    // Memory access functions
    Page*
    getpage( address_t _addr )
    {
      address_t pageidx = _addr >> Page::s_bits;
      address_t pageaddr = pageidx << Page::s_bits;
      pageidx = pageidx % (1 << s_bits);

      Page* match = m_pages[pageidx];
      while (match and match->m_address != pageaddr) match = match->m_next;
      return match ? match : (m_pages[pageidx] = new Page( m_pages[pageidx], pageaddr, this->_memset ));
    }

    Page*
    getpage_cache( address_t _addr )
    {
      address_t pageidx = _addr >> Page::s_bits;
      address_t pageaddr = pageidx << Page::s_bits;
      pageidx = pageidx % (1 << s_bits);
      
      Page* match = m_pages[pageidx];
      if (not match or match->m_address == pageaddr) return match;
      Page* node = match;
      while (node->m_next and node->m_next->m_address != pageaddr)  node = node->m_next;
      match = node->m_next;
      if (not match) return 0;
      /* MRU replacement */
      node->m_next = match->m_next;
      match->m_next = m_pages[pageidx];
      m_pages[pageidx] = match;
      return match;
    }

    void
    read( uint8_t* _buffer, address_t _addr, address_t _size )
    {
      {
        address_t offset = _addr % (1 << Page::s_bits);
        if (offset) {
          address_t size = (1 << Page::s_bits) - offset;
          size = std::min( size, _size );
          std::memcpy( (void*)_buffer, (void const*)(getpage( _addr )->m_storage + offset), size );
          _size -= size;
          _buffer += size;
          _addr += size;
        }
      }

      for (Page* cur; _size > 0 and (cur = getpage( _addr ));) {
        address_t size = (1 << Page::s_bits);
        size = std::min( size, _size );
        std::memcpy( (void*)_buffer, (void const*)(cur->m_storage), size );
        _size -= size;
        _buffer += size;
        _addr += size;
      }
    }
  
    void
    write( address_t _addr, uint8_t const* _buffer, address_t _size )
    {
      {
        address_t offset = _addr % (1 << Page::s_bits);
        if (offset) {
          address_t size = (1 << Page::s_bits) - offset;
          size = std::min( size, _size );
          std::memcpy( (void*)(getpage( _addr )->m_storage + offset), (void const*)_buffer, size );
          _size -= size;
          _buffer += size;
          _addr += size;
        }
      }

      for (Page* cur; _size > 0 and (cur = getpage( _addr ));) {
        address_t size = (1 << Page::s_bits);
        size = std::min( size, _size );
        std::memcpy( (void*)(cur->m_storage), (void const*)_buffer, size );
        _size -= size;
        _buffer += size;
        _addr += size;
      }
    }

    void
    clear( address_t _addr, address_t _size )
    {
      {
        address_t offset = _addr % (1 << Page::s_bits);
        if( offset ) {
          address_t size = (1 << Page::s_bits) - offset;
          size = std::min( size, _size );
          std::memset( (void*)(getpage( _addr )->m_storage + offset), 0, size );
          _size -= size;
          _addr += size;
        }
      }

      for (Page* cur; _size > 0 and (cur = getpage( _addr ));) {
        address_t size = (1 << Page::s_bits);
        size = std::min( size, _size );
        std::memset( (void*)(cur->m_storage), 0, size );
        _size -= size;
        _addr += size;
      }
    }
  };

} // end of namespace sparse
} // end of namespace memory
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_MEMORY_SPARSE_MEMORY_HH__
