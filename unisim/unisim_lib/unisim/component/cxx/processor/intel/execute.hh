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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_EXECUTE_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_EXECUTE_HH__

#include <unisim/component/cxx/processor/intel/types.hh>
#include <stdexcept>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace intel {
  
  template <class ARCH, typename INT>
  void
  eval_PSZ( ARCH& arch, INT const& res )
  {
    typedef typename ARCH::bit_t bit_t;
    {
      INT red = res & INT( 0xff );
      for (int shift = 4; shift > 0; shift >>= 1) red ^= (red >> shift);
      arch.flagwrite( ARCH::FLAG::PF, not bit_t( red & INT( 1 ) ) );
    }
    
    INT const msbmask( INT( 1 ) << (atpinfo<ARCH,INT>::bitsize-1) );
    arch.flagwrite( ARCH::FLAG::SF, bit_t( res & msbmask ) );
    arch.flagwrite( ARCH::FLAG::ZF, bit_t( res == INT( 0 ) ) );
  }
  
  template <class ARCH, typename INT>
  INT
  eval_add( ARCH& arch, INT const& arg1, INT const& arg2 )
  {
    typedef typename ARCH::bit_t bit_t;
    INT res = arg1 + arg2;
    
    INT const msbmask( INT( 1 ) << (atpinfo<ARCH,INT>::bitsize-1) );
    arch.flagwrite( ARCH::FLAG::OF, bit_t( (((arg1 & arg2 & ~res) | (~arg1 & ~arg2 & res)) & msbmask) == msbmask ) );
    arch.flagwrite( ARCH::FLAG::CF, bit_t( ((((arg1 | arg2) & ~res) | (arg1 & arg2 & res)) & msbmask) == msbmask ) );
    
    eval_PSZ( arch, res );
    
    arch.flagwrite( ARCH::FLAG::AF, bit_t(0) ); /*:TODO:*/
    
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_or( ARCH& arch, INT const& arg1, INT const& arg2 )
  {
    typedef typename ARCH::bit_t bit_t;
    INT res = arg1 | arg2;
    
    arch.flagwrite( ARCH::FLAG::OF, bit_t( false ) );
    arch.flagwrite( ARCH::FLAG::CF, bit_t( false ) );
    
    arch.flagwrite( ARCH::FLAG::AF, bit_t(0) ); /*:TODO:*/
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_adc( ARCH& arch, INT const& arg1, INT const& arg2 )
  {
    typedef typename ARCH::bit_t bit_t;
    INT op2 = arg2 + INT( arch.flagread( ARCH::FLAG::CF ) );
    INT res = arg1 + op2;
    
    INT const msbmask( INT( 1 ) << (atpinfo<ARCH,INT>::bitsize-1) );
    arch.flagwrite( ARCH::FLAG::OF, bit_t( (((arg1 & op2 & ~res) | (~arg1 & ~op2 & res)) & msbmask) == msbmask ) );
    arch.flagwrite( ARCH::FLAG::CF, bit_t( ((((arg1 | op2) & ~res) | (arg1 & op2 & res)) & msbmask) == msbmask ) );
    
    arch.flagwrite( ARCH::FLAG::AF, bit_t(0) ); /*:TODO:*/
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_sbb( ARCH& arch, INT const& arg1, INT const& arg2 )
  {
    typedef typename ARCH::bit_t bit_t;
    INT op2 = arg2 + INT( arch.flagread( ARCH::FLAG::CF ) );
    INT res = arg1 - op2;
    
    INT const msbmask( INT( 1 ) << (atpinfo<ARCH,INT>::bitsize-1) );
    arch.flagwrite( ARCH::FLAG::OF, bit_t( (((arg1 & ~op2 & ~res) | (~arg1 & op2 & res)) & msbmask) == msbmask ) );
    arch.flagwrite( ARCH::FLAG::CF, bit_t( ((((~arg1 | op2) & res) | (~arg1 & op2 & ~res)) & msbmask) == msbmask ) );
    
    arch.flagwrite( ARCH::FLAG::AF, bit_t(0) ); /*:TODO:*/
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_and( ARCH& arch, INT const& arg1, INT const& arg2 )
  {
    typedef typename ARCH::bit_t bit_t;
    INT res = arg1 & arg2;
    
    arch.flagwrite( ARCH::FLAG::OF, bit_t( false ) );
    arch.flagwrite( ARCH::FLAG::CF, bit_t( false ) );
    
    arch.flagwrite( ARCH::FLAG::AF, bit_t(0) ); /*:TODO:*/
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_sub( ARCH& arch, INT const& arg1, INT const& arg2 )
  {
    typedef typename ARCH::bit_t bit_t;
    INT res = arg1 - arg2;
    
    INT const msbmask( INT( 1 ) << (atpinfo<ARCH,INT>::bitsize-1) );
    arch.flagwrite( ARCH::FLAG::OF, bit_t( (((arg1 & ~arg2 & ~res) | (~arg1 & arg2 & res)) & msbmask) == msbmask ) );
    arch.flagwrite( ARCH::FLAG::CF, bit_t( ((((~arg1 | arg2) & res) | (~arg1 & arg2 & ~res)) & msbmask) == msbmask ) );

    arch.flagwrite( ARCH::FLAG::AF, bit_t(0) ); /*:TODO:*/
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_xor( ARCH& arch, INT const& arg1, INT const& arg2 )
  {
    typedef typename ARCH::bit_t bit_t;
    INT res = arg1 ^ arg2;
    
    arch.flagwrite( ARCH::FLAG::OF, bit_t( false ) );
    arch.flagwrite( ARCH::FLAG::CF, bit_t( false ) );
    
    arch.flagwrite( ARCH::FLAG::AF, bit_t(0) ); /*:TODO:*/
    
    eval_PSZ( arch, res );
      
    return res;
  }
  
  /* Intel: The count is masked to 5 bits (or 6 bits if in 64-bit mode
   * and REX.W is used). The count range is limited to 0 to 31 (or 63
   * if 64-bit mode and REX.W is used).
   */
  template <unsigned BSZ> struct c_shift_counter {};
  template <> struct c_shift_counter< 8> { static uint8_t mask() { return uint8_t(0x1f); } };
  template <> struct c_shift_counter<16> { static uint8_t mask() { return uint8_t(0x1f); } };
  template <> struct c_shift_counter<32> { static uint8_t mask() { return uint8_t(0x1f); } };
  template <> struct c_shift_counter<64> { static uint8_t mask() { return uint8_t(0x3f); } };

  template <class ARCH, class INT> struct shift_counter
  {
    typedef typename ARCH::u8_t u8_t;
    static intptr_t const bitsize = atpinfo<ARCH,INT>::bitsize;
    static u8_t mask() { return u8_t( c_shift_counter<bitsize>::mask() ); }
  }; 

  template <class ARCH, typename INT>
  INT
  eval_rol( ARCH& arch, INT const& arg1, typename ARCH::u8_t const& arg2 )
  {
    typedef typename ARCH::bit_t bit_t;
    typedef typename ARCH::u8_t u8_t;
    intptr_t const bitsize = atpinfo<ARCH,INT>::bitsize;
    u8_t const u8bitsize( bitsize );
    INT const msb = INT( 1 ) << (bitsize-1);
    INT res( 0 );
    
    u8_t sharg = arg2 & shift_counter<ARCH,INT>::mask();
    
    sharg = sharg % u8bitsize;
    res = (arg1 << sharg) | (arg1 >> (u8bitsize - sharg));
    arch.flagwrite( ARCH::FLAG::CF, bit_t( res & INT( 1 ) ) );
    arch.flagwrite( ARCH::FLAG::OF, bit_t( (arg1 ^ res) & msb ) );
      
    arch.flagwrite( ARCH::FLAG::AF, bit_t(0) ); /*:TODO:*/
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_ror( ARCH& arch, INT const& arg1, typename ARCH::u8_t const& arg2 )
  {
    typedef typename ARCH::bit_t bit_t;
    typedef typename ARCH::u8_t u8_t;
    intptr_t const bitsize = atpinfo<ARCH,INT>::bitsize;
    u8_t const u8bitsize( bitsize );
    INT const msb = INT( 1 ) << (bitsize-1);
    INT res( 0 );
    
    u8_t sharg = arg2 & shift_counter<ARCH,INT>::mask();
    
    sharg = sharg % u8bitsize;
    res = (arg1 << (u8bitsize - sharg)) | (arg1 >> sharg);
    arch.flagwrite( ARCH::FLAG::CF, bit_t( res & msb ) );
    arch.flagwrite( ARCH::FLAG::OF, bit_t( (arg1 ^ res) & msb ) );
      
    arch.flagwrite( ARCH::FLAG::AF, bit_t(0) ); /*:TODO:*/
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_rcl( ARCH& arch, INT const& arg1, typename ARCH::u8_t const& arg2 )
  {
    typedef typename ARCH::bit_t bit_t;
    typedef typename ARCH::u8_t u8_t;
    intptr_t const bitsize = atpinfo<ARCH,INT>::bitsize;
    INT const msb = INT( 1 ) << (bitsize-1);
    u8_t lsh = (arg2 & shift_counter<ARCH,INT>::mask()) % u8_t( bitsize + 1 ), rsh = u8_t( bitsize ) - lsh;

    INT res;
    if (arch.Test(lsh != u8_t(0)))
      {
        res = (arg1 << lsh) | (INT(arch.flagread( ARCH::FLAG::CF )) << lsh >> 1) | (arg1 >> 1 >> rsh);
        arch.flagwrite( ARCH::FLAG::CF, bit_t( (arg1 >> rsh) & INT(1) ) );
      }
    else
      res = arg1;

    arch.flagwrite( ARCH::FLAG::OF, bit_t( (arg1 ^ res) & msb ) );
    arch.flagwrite( ARCH::FLAG::AF, bit_t(0) ); /*:TODO:*/
    
    eval_PSZ( arch, res );
      
    return res;
  }
  
  template <class ARCH, typename INT>
  INT
  eval_rcr( ARCH& arch, INT const& arg1, typename ARCH::u8_t const& arg2 )
  {
    typedef typename ARCH::bit_t bit_t;
    typedef typename ARCH::u8_t u8_t;
    intptr_t const bitsize = atpinfo<ARCH,INT>::bitsize;
    INT const msb = INT( 1 ) << (bitsize-1);
    u8_t rsh = (arg2 & shift_counter<ARCH,INT>::mask()) % u8_t( bitsize + 1 ), lsh = u8_t( bitsize ) - rsh;
    
    INT res;
    if (arch.Test(rsh != u8_t(0)))
      {
        res = (arg1 >> rsh) | (INT(arch.flagread( ARCH::FLAG::CF )) << lsh) | (arg1 << 1 << lsh);
        arch.flagwrite( ARCH::FLAG::CF, bit_t( (arg1 << lsh) & msb ) );
      }
    else
      res = arg1;
    

    arch.flagwrite( ARCH::FLAG::OF, bit_t( (arg1 ^ res) & msb ) );
    arch.flagwrite( ARCH::FLAG::AF, bit_t(0) ); /*:TODO:*/
    
    eval_PSZ( arch, res );
      
    return res;
  }
  
  /* TODO: whould implement large rotate through carry */
  // template <> u64_t eval_rcr( ARCH& arch, u64_t const& arg1, u8_t const& arg2 ) { throw 0; }
  
  template <class ARCH, typename INT>
  INT
  eval_shl( ARCH& arch, INT const& arg1, typename ARCH::u8_t const& arg2 )
  {
    typedef typename ARCH::bit_t bit_t;
    typedef typename ARCH::u8_t u8_t;
    intptr_t const bitsize = atpinfo<ARCH,INT>::bitsize;
    INT const msb = INT( 1 ) << (bitsize-1);
    INT res( 0 );
    
    u8_t sharg = arg2 & shift_counter<ARCH,INT>::mask();
    
    res = arg1 << sharg;
    arch.flagwrite( ARCH::FLAG::CF, bit_t( arch.Test( sharg >= u8_t( 1 ) ) ? ((arg1 << (sharg - u8_t( 1 ))) & msb) : INT( 0 ) ) );
    arch.flagwrite( ARCH::FLAG::OF, bit_t( (arg1 ^ res) & msb ) );
      
    arch.flagwrite( ARCH::FLAG::AF, bit_t(0) ); /*:TODO:*/
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_shr( ARCH& arch, INT const& arg1, typename ARCH::u8_t const& arg2 )
  {
    typedef typename ARCH::bit_t bit_t;
    typedef typename ARCH::u8_t u8_t;
    intptr_t const bitsize = atpinfo<ARCH,INT>::bitsize;
    INT const msb = INT( 1 ) << (bitsize-1);
    INT res( 0 );
    
    u8_t sharg = arg2 & shift_counter<ARCH,INT>::mask();
    
    res = arg1 >> sharg;
    arch.flagwrite( ARCH::FLAG::CF, bit_t( arch.Test( sharg >= u8_t( 1 ) ) ? ((arg1 >> (sharg - u8_t( 1 ))) & INT( 1 )) : INT( 0 ) ) );
    arch.flagwrite( ARCH::FLAG::OF, bit_t( arg1 & msb ) );
      
    arch.flagwrite( ARCH::FLAG::AF, bit_t(0) ); /*:TODO:*/
    
    eval_PSZ( arch, res );
      
    return res;
  }
  
  template <class ARCH, typename INT>
  INT
  eval_sar( ARCH& arch, INT const& arg1, typename ARCH::u8_t const& arg2 )
  {
    typedef typename ARCH::bit_t bit_t;
    typedef typename ARCH::u8_t u8_t;
    //intptr_t const bitsize = atpinfo<ARCH,INT>::bitsize;
    //INT const msb = INT( 1 ) << (bitsize-1);
    INT res( 0 );
    
    u8_t sharg = arg2 & shift_counter<ARCH,INT>::mask();
    
    res = INT( (typename atpinfo<ARCH,INT>::stype( arg1 )) >> sharg );
    arch.flagwrite( ARCH::FLAG::CF, bit_t( arch.Test( sharg >= u8_t( 1 ) ) ? ((arg1 >> (sharg - u8_t( 1 ))) & INT( 1 )) : INT( 0 ) ) );
    arch.flagwrite( ARCH::FLAG::OF, bit_t( false ) );
      
    arch.flagwrite( ARCH::FLAG::AF, bit_t(0) ); /*:TODO:*/
    
    eval_PSZ( arch, res );
      
    return res;
  }
  
  template <class ARCH>
  typename ARCH::bit_t
  eval_cond( ARCH& a, uint32_t _cc )
  {
    typedef typename ARCH::bit_t bit_t;
    bit_t res = bit_t( _cc & 1 );
    switch ((_cc >> 1) & 0x7) {
    case 0: res ^= a.flagread( ARCH::FLAG::OF ); break;
    case 1: res ^= a.flagread( ARCH::FLAG::CF ); break;
    case 2: res ^= a.flagread( ARCH::FLAG::ZF ); break;
    case 3: res ^= a.flagread( ARCH::FLAG::CF ) | a.flagread( ARCH::FLAG::ZF ); break;
    case 4: res ^= a.flagread( ARCH::FLAG::SF ); break;
    case 5: res ^= a.flagread( ARCH::FLAG::PF ); break;
    case 6: res ^= a.flagread( ARCH::FLAG::SF ) ^ a.flagread( ARCH::FLAG::OF ); break;
    case 7: res ^= a.flagread( ARCH::FLAG::ZF ) | (a.flagread( ARCH::FLAG::SF ) ^ a.flagread( ARCH::FLAG::OF )); break;
    }
    return res;
  }
  
  template <class ARCH, typename INT>
  void eval_div( ARCH& arch, INT& hi, INT& lo, INT const& divisor )
  {
    if (arch.Test(divisor == INT(0))) arch._DE();
    
    typedef typename atpinfo<ARCH,INT>::utype utype;
    typedef typename atpinfo<ARCH,INT>::twice twice;
    twice dividend = (twice( hi ) << int(atpinfo<ARCH,INT>::bitsize)) | twice( utype( lo ) );
    twice result = twice( dividend ) / twice( divisor );
    //if (twice( INT( result ) ) != result) arch._DE();
    lo = INT( result );
    hi = INT( twice( dividend ) % twice( divisor ) );
  }
  
  /* TODO: need to implement very large divisions */
  //  template <> void eval_div( ARCH& arch, u64_t& hi, u64_t& lo, u64_t const& divisor ) { throw 0; }
  //  template <> void eval_div( ARCH& arch, s64_t& hi, s64_t& lo, s64_t const& divisor ) { throw 0; }
  
  template <class ARCH, typename INT>
  void eval_mul( ARCH& arch, INT& hi, INT& lo, INT const& multiplier )
  {
    typedef typename ARCH::bit_t bit_t;
    // typedef typename atpinfo<ARCH,INT>::utype utype;
    typedef typename atpinfo<ARCH,INT>::twice twice;
    twice result = twice( lo ) * twice( multiplier );
    hi = INT( result >> int(atpinfo<ARCH,INT>::bitsize) );
    INT lores = INT( result );
    lo = lores;
    bit_t ovf = twice( lores ) != result;
    arch.flagwrite( ARCH::FLAG::OF, ovf );
    arch.flagwrite( ARCH::FLAG::CF, ovf );
  }
  
  /* TODO: need to implement very large multiplications */
  // template <> void eval_mul( ARCH& arch, u64_t& hi, u64_t& lo, u64_t const& divisor ) { throw 0; }
  // template <> void eval_mul( ARCH& arch, s64_t& hi, s64_t& lo, s64_t const& divisor ) { throw 0; }
  
  template <class ARCH>
  typename ARCH::u16_t
  fswread( ARCH& a )
  {
    typedef typename ARCH::u16_t u16_t;
    return
      (u16_t( 0 )                             <<  0 /* IE */) |
      (u16_t( 0 )                             <<  1 /* DE */) |
      (u16_t( 0 )                             <<  2 /* ZE */) |
      (u16_t( 0 )                             <<  3 /* OE */) |
      (u16_t( 0 )                             <<  4 /* UE */) |
      (u16_t( 0 )                             <<  5 /* PE */) |
      (u16_t( 0 )                             <<  6 /* SF */) |
      (u16_t( 0 )                             <<  7 /* ES */) |
      (u16_t( a.flagread( ARCH::FLAG::C0 ) ) <<  8 /* C0 */) |
      (u16_t( a.flagread( ARCH::FLAG::C1 ) ) <<  9 /* C1 */) |
      (u16_t( a.flagread( ARCH::FLAG::C2 ) ) << 10 /* C2 */) |
      (u16_t( a.ftopread() )                  << 11 /* ST */) |
      (u16_t( a.flagread( ARCH::FLAG::C3 ) ) << 14 /* C3 */) |
      (u16_t( 0 )                             << 15 /*  B */);
  }

  template <class ARCH>
  typename ARCH::u32_t
  eflagsread( ARCH& a )
  {
    typedef typename ARCH::u32_t u32_t;
    //00000286
    return
      (u32_t( a.flagread( ARCH::FLAG::CF ) ) <<  0 /* CF */ ) |
      (u32_t( 1 )                             <<  1 /*  1 */ ) |
      (u32_t( a.flagread( ARCH::FLAG::PF ) ) <<  2 /* PF */ ) |
      (u32_t( 0 )                             <<  3 /*  0 */ ) |
      (u32_t( a.flagread( ARCH::FLAG::AF ) ) <<  4 /* AF */ ) |
      (u32_t( 0 )                             <<  5 /*  0 */ ) |
      (u32_t( a.flagread( ARCH::FLAG::ZF ) ) <<  6 /* ZF */ ) |
      (u32_t( a.flagread( ARCH::FLAG::SF ) ) <<  7 /* SF */ ) |
      (u32_t( 0 )                             <<  8 /* TF */ ) |
      (u32_t( 0 )                             <<  9 /* IF */ ) |
      (u32_t( a.flagread( ARCH::FLAG::DF ) ) << 10 /* DF */ ) |
      (u32_t( a.flagread( ARCH::FLAG::OF ) ) << 11 /* OF */ ) |
      (u32_t( 0 )                             << 12 /*IOPL*/ ) |
      (u32_t( 0 )                             << 14 /* NT */ ) |
      (u32_t( 0 )                             << 15 /*  0 */ ) |
      (u32_t( 0 )                             << 16 /* RF */ ) |
      (u32_t( 0 )                             << 17 /* VM */ ) |
      (u32_t( 0 )                             << 18 /* AC */ ) |
      (u32_t( 0 )                             << 19 /* VIF*/ ) |
      (u32_t( 0 )                             << 20 /* VIP*/ ) |
      (u32_t( 0 )                             << 21 /* ID */ );
  }

  template <typename T> T const& Minimum( T const& l, T const& r ) { return l < r ? l : r; }
  template <typename T> T const& Maximum( T const& l, T const& r ) { return l > r ? l : r; }
  
  template <class ARCH>
  typename ARCH::f64_t
  eval_fprem1( ARCH& arch, typename ARCH::f64_t const& dividend, typename ARCH::f64_t const& modulus )
  {
    typedef typename ARCH::f64_t f64_t;
    typedef typename ARCH::u64_t u64_t;
    typedef typename ARCH::bit_t bit_t;
    
    f64_t res;
    f64_t const threshold = power( f64_t( 2. ), f64_t( 64. ) ); // should be 2**64
    if (arch.Test( (modulus * threshold) > dividend ))
      {
        f64_t quotient = firound( dividend / modulus, intel::x87frnd_nearest );
        res = fmodulo( dividend, modulus );
        u64_t uq = (arch.Test( quotient < f64_t( 0.0 ) )) ? u64_t( -quotient ) : u64_t( quotient );
        arch.flagwrite( ARCH::FLAG::C2, bit_t( false ) );
        arch.flagwrite( ARCH::FLAG::C0, bit_t( (uq >> 2) & u64_t( 1 ) ) );
        arch.flagwrite( ARCH::FLAG::C3, bit_t( (uq >> 1) & u64_t( 1 ) ) );
        arch.flagwrite( ARCH::FLAG::C1, bit_t( (uq >> 0) & u64_t( 1 ) ) );
      }
    else
      {
        f64_t const step = power( f64_t( 2. ), f64_t( 32. ) ); // should be 2**32
        f64_t pmodulus = modulus;
        while (arch.Test( (pmodulus *step) <= dividend )) pmodulus = pmodulus * f64_t( 2. );
        res = fmodulo( dividend, pmodulus );
        arch.flagwrite( ARCH::FLAG::C2, bit_t( true ) );
      }
    return res;
  }
  
  template <class ARCH>
  typename ARCH::f64_t
  eval_fprem( ARCH& arch, typename ARCH::f64_t const& dividend, typename ARCH::f64_t const& modulus )
  {
    typedef typename ARCH::f64_t f64_t;
    typedef typename ARCH::u64_t u64_t;
    typedef typename ARCH::bit_t bit_t;
    
    f64_t res;
    f64_t const threshold = power( f64_t( 2. ), f64_t( 64. ) ); // should be 2**64
    if (arch.Test( (modulus * threshold) > dividend ))
      {
        f64_t quotient = firound( dividend / modulus, intel::x87frnd_toward0 );
        res = fmodulo( dividend, modulus );
        u64_t uq = (arch.Test( quotient < f64_t( 0.0 ) )) ? u64_t( -quotient ) : u64_t( quotient );
        arch.flagwrite( ARCH::FLAG::C2, bit_t( false ) );
        arch.flagwrite( ARCH::FLAG::C0, bit_t( (uq >> 2) & u64_t( 1 ) ) );
        arch.flagwrite( ARCH::FLAG::C3, bit_t( (uq >> 1) & u64_t( 1 ) ) );
        arch.flagwrite( ARCH::FLAG::C1, bit_t( (uq >> 0) & u64_t( 1 ) ) );
      }
    else
      {
        f64_t const step = power( f64_t( 2. ), f64_t( 32. ) ); // should be 2**32
        f64_t pmodulus = modulus;
        while (arch.Test( (pmodulus *step) <= dividend )) pmodulus = pmodulus * f64_t( 2. );
        res = fmodulo( dividend, pmodulus );
        arch.flagwrite( ARCH::FLAG::C2, bit_t( true ) );
      }
    return res;
  }
  
} // end of namespace intel
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_EXECUTE_HH__

