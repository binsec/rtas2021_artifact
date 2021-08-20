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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_VECTORBANK_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_VECTORBANK_HH__

#include <algorithm>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace intel {

  template <typename T>
  struct VectorTypeInfo
  {
    enum { bytecount = sizeof (T) };
    static void ToBytes( uint8_t* dst, T src )
    {
      for (unsigned idx = 0; idx < sizeof (T); ++idx)
        { dst[idx] = src & 0xff; src >>= 8; }
    }
    static void FromBytes( T& dst, uint8_t const* src )
    {
      T tmp = 0;
      for (unsigned idx = sizeof (T); idx-- > 0;)
        { tmp <<= 8; tmp |= uint32_t( src[idx] ); }
      dst = tmp;
    }
    static void Destroy( T& obj ) { /* Base scalar types don't need any specific destructor */ }
    static void Allocate( T& obj ) { /* Base scalar type doesn't need any specific constructor */ }
  };

  template <> struct VectorTypeInfo<float>
  {
    enum bytecount_t { bytecount = 4 };
    static void ToBytes( uint8_t* dst, float const& src ) { VectorTypeInfo<uint32_t>::ToBytes( dst, reinterpret_cast<uint32_t const&>( src ) ); }
    static void FromBytes( float& dst, uint8_t const* src ) { VectorTypeInfo<uint32_t>::FromBytes( reinterpret_cast<uint32_t&>( dst ), src ); }
    static void Destroy( float& obj ) { /* float type doesn't need any specific destructor */ }
    static void Allocate( float& obj ) { /* float type doesn't need any specific constructor */ }
  };

  template <> struct VectorTypeInfo<double>
  {
    enum bytecount_t { bytecount = 8 };
    static void ToBytes( uint8_t* dst, double const& src ) { VectorTypeInfo<uint64_t>::ToBytes( dst, reinterpret_cast<uint64_t const&>( src ) ); }
    static void FromBytes( double& dst, uint8_t const* src ) { VectorTypeInfo<uint64_t>::FromBytes( reinterpret_cast<uint64_t&>( dst ), src ); }
    static void Destroy( double& obj ) { /* double type doesn't need any specific destructor */ }
    static void Allocate( double& obj ) { /* double type doesn't need any specific constructor */ }
  };

  template <class CONFIG>
  struct VUnion
  {
    template <typename T> using TypeInfo = typename CONFIG:: template TypeInfo<T>;
    typedef typename CONFIG::Byte Byte;

    typedef void (*transfer_t)( Byte* dst, void* src, unsigned size, bool destroy );

    template <typename ELEM>
    static unsigned ItemCount() { return CONFIG::BYTECOUNT / TypeInfo<ELEM>::bytecount; }

    template <typename ELEM>
    static void Transfer( Byte* bytes, void* storage, unsigned size, bool destroy )
    {
      ELEM* vec = reinterpret_cast<ELEM*>( storage );

      Byte* dst_end = &bytes[size];
      for (unsigned idx = 0, end = CONFIG::BYTECOUNT / TypeInfo<ELEM>::bytecount; idx < end; ++idx)
        {
          Byte* dst = &bytes[idx*TypeInfo<ELEM>::bytecount];
          if (dst < dst_end)
            TypeInfo<ELEM>::ToBytes( dst, vec[idx] );
          if (destroy)
            TypeInfo<ELEM>::Destroy( vec[idx] );
        }
    }

    template <class ELEM>
    ELEM*
    rearrange( void* storage, unsigned final_size )
    {
      ELEM* res = reinterpret_cast<ELEM*>( storage );
      transfer_t current = &Transfer<ELEM>;
      unsigned const elem_size = TypeInfo<ELEM>::bytecount;
      if (transfer != current)
        {
          Byte buf[CONFIG::BYTECOUNT];
          transfer( &buf[0], storage, size, true );
          // if destination element is wider than source vector:
          for (unsigned idx = size; idx < elem_size; ++idx)
            buf[idx] = Byte(0u);
          Byte const* src_end = &buf[std::min(size, final_size)];
          for (unsigned idx = 0, end = CONFIG::BYTECOUNT / elem_size; idx < end; ++idx)
            {
              unsigned pos = idx*elem_size;
              Byte const* src = &buf[pos];
              TypeInfo<ELEM>::Allocate( res[idx] );
              if (src < src_end)
                TypeInfo<ELEM>::FromBytes( res[idx], src );
              else if (pos < final_size)
                res[idx] = ELEM(0);
            }
          transfer = current;
        }
      else if (size < final_size)
        {
          for (unsigned idx = size / elem_size, end = final_size / elem_size; idx < end; ++idx)
            res[idx] = ELEM(0);
        }
      size = final_size;
      return res;
    }

    template <typename ELEM>
    ELEM*
    GetStorage( void* storage, ELEM const&, unsigned final_size )
    {
      return rearrange<ELEM>( storage, final_size );
    }

    template <class ELEM>
    ELEM* GetConstStorage( void* storage, ELEM const&, unsigned final_size )
    {
      return rearrange<ELEM>( storage, std::max(size, final_size) );
    }

    static void initial( Byte*, void*, unsigned, bool ) {}

    VUnion() : transfer( &initial ), size(0) {}

    transfer_t transfer;
    unsigned   size;
  };

  // struct FlushInvalidate
  // {
  //   FlushInvalidate( unsigned _idx, uint8_t* _buffer, uint8_t const* _data )
  //     : idx(_idx), buffer(_buffer), data(_data)
  //   {}

  //   unsigned       idx;
  //   uint8_t*       buffer;
  //   uint8_t const* data;
  // };

  // template <typename valT, unsigned _RegCount, unsigned _RegSize>
  // struct VectorBankCache
  // {
  //   static uintptr_t const sub_count = _RegSize / VectorTypeInfo<valT>::bytecount;
  //   bool     valid[_RegCount];

  //   VectorBankCache() { for (unsigned idx = 0; idx < _RegCount; ++idx) valid[idx] = true; }

  //   template <typename dirT>
  //   valT*
  //   GetStorage( dirT& dir, unsigned idx )
  //   {
  //     uint8_t* data = &dir.storage[idx][0];
  //     valT* regvec = static_cast<valT*>( data );

  //     // Force validity
  //     if (not valid[idx]) {
  //       uint8_t buffer[_RegSize];
  //       FlushInvalidate copy( idx, buffer, data );
  //       dir.DoAll( copy );
  //       valid[idx] = true;
  //       for (unsigned sub = 0; sub < sub_count; ++sub)
  //         VectorTypeInfo<valT>::FromBytes( regvec[sub], &buffer[sub*VectorTypeInfo<valT>::bytecount] );
  //     }

  //     return regvec;
  //   }

  //   void Do( FlushInvalidate& copy )
  //   {
  //     unsigned idx = copy.idx;
  //     valT* regvec = static_cast<valT*>( copy.data );

  //     if (not valid[idx]) return;
  //     valid[idx] = false;
  //     if (not copy.buffer) return;

  //     for (unsigned sub = 0; sub < sub_count; ++sub)
  //       VectorTypeInfo<valT>::ToBytes( &copy.buffer[sub*VectorTypeInfo<valT>::bytecount], regvec[sub] );

  //     copy.buffer = 0;
  //   }
  // };

} // end of namespace intel
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_VECTORBANK_HH__
