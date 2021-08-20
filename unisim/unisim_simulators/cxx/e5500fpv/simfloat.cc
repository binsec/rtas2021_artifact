/*
 *  Copyright (c) 2018,
 *  Commissariat a l'Energie Atomique (CEA)
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

#include <simfloat.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/simfloat/integer.hh>
#include <unisim/util/simfloat/integer.tcc>
#include <unisim/util/simfloat/floating.hh>
#include <unisim/util/simfloat/floating.tcc>
#include <unisim/util/likely/likely.hh>

SoftDouble::SoftDouble(int64_t src, Flags& flags)
  : impl()
{
  if (src == 0)
    return;
  
  bool neg = src < 0;
  impl.setNegative(neg);
  uint64_t mantissa = neg ? -src : src;

  unsigned exponent = unisim::util::arithmetic::BitScanReverse(mantissa), mantissa_bitcount = exponent;
  
  {
    // Stripping right zeroes
    unsigned lso = unisim::util::arithmetic::BitScanForward(mantissa);
    mantissa >>= lso;
    mantissa_bitcount -= lso;
  }

  unsigned const bitSizeMantissa = impl.bitSizeMantissa();
  if (bitSizeMantissa >= mantissa_bitcount)
    {
      mantissa <<= (bitSizeMantissa - mantissa_bitcount);
    }
  else
    {
      int const out = mantissa_bitcount - bitSizeMantissa;
      typedef Flags::Impl IFlags;
      IFlags& iflags = flags.impl;
      struct NearestRound
      {
        bool increment;
        NearestRound( IFlags& iflags, uint64_t mantissa, int const out )
          : increment(false)
        {
          int const tail = out - 1;
          uint64_t const _1 = int64_t(1);
          if (not iflags.isNearestRound()) 
            return;
          iflags.clearEffectiveRoundToEven();
          if (not ((mantissa >> tail) & _1)) // close to zero ?
            return;
          if ((increment = bool(mantissa & ((_1 << tail) - _1)))) // close to one ?
            return;
          iflags.setEffectiveRoundToEven(); // Half way...
          increment = bool((mantissa >> out) & _1);
        }
      };
        
      if ((iflags.isHighestRound() and not neg) or
          (iflags.isLowestRound()  and     neg) or
          NearestRound(iflags, mantissa, out).increment)
        {
          mantissa = (mantissa >> out) + 1;
          if (bool(mantissa >> (bitSizeMantissa+1)))
            { mantissa >>= 1; exponent += 1; }
          iflags.setApproximate(neg ? IFlags::Down : IFlags::Up);
        }
      else
        {
          mantissa = mantissa >> out;
          iflags.setApproximate(neg ? IFlags::Up : IFlags::Down);
        }
    }
          
 
  // Build exponent
  BuiltDoubleTraits::Exponent& impl_exponent = impl.querySBasicExponent();
  impl_exponent = impl.getZeroExponent(); // TypeTraits::getZeroExponent(biExponent);
  impl_exponent.add(exponent);
  
  // Build mantissa
  mantissa &= ((1ull << bitSizeMantissa) - 1ull); // normalizing
  for (unsigned idx = 0, wsz = (8*sizeof(unsigned int)), last = (bitSizeMantissa-1)/wsz; idx <= last; ++idx)
    impl.querySMantissa()[idx] = mantissa >> idx*wsz;
}
        

SoftFloat&
SoftFloat::fromRawBitsAssign(uint32_t raw_bits)
{
  impl.setChunk((void*) &raw_bits, unisim::util::endian::IsHostLittleEndian());

  return *this;
}

SoftDouble&
SoftDouble::fromRawBitsAssign(uint64_t raw_bits)
{
  impl.setChunk((void*) &raw_bits, unisim::util::endian::IsHostLittleEndian());

  return *this;
}

SoftFloat&
SoftFloat::convertAssign(const SoftDouble& op1, Flags& flags)
{
  Flags::Impl::Approximation source_approximation = flags.impl.getApproximation();
  
  impl_type::FloatConversion fcConversion;
  
  fcConversion.setSizeMantissa(52).setSizeExponent(11);
  fcConversion.setNegative(op1.isNegative());
  fcConversion.exponent()[0] = op1.impl.queryBasicExponent()[0];
  fcConversion.mantissa()[0] = op1.impl.queryMantissa()[0];
  fcConversion.mantissa()[1] = op1.impl.queryMantissa()[1];

  impl = impl_type(fcConversion, flags.impl);

  if (not flags.impl.isApproximate())
    {
      switch (source_approximation)
        {
        default: break;
        case Flags::Impl::ADownApproximate: flags.impl.setApproximate(Flags::Impl::Down);          break;
        case Flags::Impl::AUpApproximate:   flags.impl.setApproximate(Flags::Impl::Up);            break;
        }
    }
  
  return *this;
}

SoftDouble&
SoftDouble::convertAssign(const SoftFloat& op1, Flags& flags)
{
  impl_type::FloatConversion fcConversion;

  fcConversion.setSizeMantissa(23).setSizeExponent(8);
  fcConversion.setNegative(op1.isNegative());
  fcConversion.exponent()[0] = op1.impl.queryBasicExponent()[0];
  fcConversion.mantissa()[0] = op1.impl.queryMantissa()[0];
   
  impl = impl_type(fcConversion, flags.impl);
  
  return *this;
}

SoftFloat&  SoftFloat::  operator= (const SoftFloat& sfSource) { impl = sfSource.impl; return *this; }
SoftDouble& SoftDouble:: operator= (const SoftDouble& sdSource) { impl = sdSource.impl; return *this; }

SoftFloat&  SoftFloat:: assign(const SoftFloat& sfSource) { impl = sfSource.impl; return *this; }
SoftDouble& SoftDouble::assign(const SoftDouble& sdSource) { impl = sdSource.impl; return *this; }

uint32_t
SoftFloat::queryRawBits() const
{
  uint32_t uResult;
  impl.fillChunk(&uResult, unisim::util::endian::IsHostLittleEndian());
  
  return uResult;
}

uint64_t
SoftDouble::queryRawBits() const
{
  uint64_t uResult;
  impl.fillChunk( &uResult, unisim::util::endian::IsHostLittleEndian() );
  
  return uResult;
}

bool SoftFloat::isNegative() const { return impl.isNegative(); }
bool SoftDouble::isNegative() const { return impl.isNegative(); }
//bool SoftFloat::isPositive() const { return impl.isPositive(); }
//bool SoftDouble::isPositive() const { return impl.isPositive(); }
bool SoftFloat::isZero() const { return impl.isZero(); }
bool SoftDouble::isZero() const { return impl.isZero(); }
bool SoftFloat::isInfty() const { return impl.isInfty(); }
bool SoftDouble::isInfty() const { return impl.isInfty(); }

void SoftFloat::opposite() { impl.opposite(); }
void SoftDouble::opposite() { impl.opposite(); }

void SoftFloat::setPositive() { impl.setNegative(false); }
void SoftDouble::setPositive() { impl.setNegative(false); }
void SoftFloat::setNegative() { impl.setNegative(true); }
void SoftDouble::setNegative() { impl.setNegative(true); }

void SoftFloat::plusAssign(const SoftFloat& op1, Flags& flags) { impl.plusAssign(op1.impl,flags.impl); }
void SoftDouble::plusAssign(const SoftDouble& op1, Flags& flags) { impl.plusAssign(op1.impl,flags.impl); }
void SoftFloat::minusAssign(const SoftFloat& op1, Flags& flags) { impl.minusAssign(op1.impl,flags.impl); }
void SoftDouble::minusAssign(const SoftDouble& op1, Flags& flags) { impl.minusAssign(op1.impl,flags.impl); }
void SoftFloat::multAssign(const SoftFloat& op1, Flags& flags) { impl.multAssign(op1.impl,flags.impl); }
void SoftDouble::multAssign(const SoftDouble& op1, Flags& flags) { impl.multAssign(op1.impl,flags.impl); }
void SoftFloat::divAssign(const SoftFloat& op1, Flags& flags) { impl.divAssign(op1.impl,flags.impl); }
void SoftDouble::divAssign(const SoftDouble& op1, Flags& flags) { impl.divAssign(op1.impl,flags.impl); }

void SoftFloat::multAndAddAssign(const SoftFloat& a, const SoftFloat& b, Flags& flags) { impl.multAndAddAssign(a.impl, b.impl, flags.impl); }
void SoftDouble::multAndAddAssign(const SoftDouble& a, const SoftDouble& b, Flags& flags) { impl.multAndAddAssign(a.impl, b.impl, flags.impl); }

void SoftFloat::multAndSubAssign(const SoftFloat& a, const SoftFloat& b, Flags& flags) { impl.multAndSubAssign(a.impl, b.impl, flags.impl); }
void SoftDouble::multAndSubAssign(const SoftDouble& a, const SoftDouble& b, Flags& flags) { impl.multAndSubAssign(a.impl, b.impl, flags.impl); }

SoftFloat::ComparisonResult SoftFloat::compare( SoftFloat const& op1 ) const { return (SoftFloat::ComparisonResult)impl.compare( op1.impl ); }
SoftDouble::ComparisonResult SoftDouble::compare( SoftDouble const& op1 ) const { return (SoftDouble::ComparisonResult)impl.compare( op1.impl ); }

bool SoftFloat::isSNaN() const { return impl.isSNaN(); }
bool SoftDouble::isSNaN() const { return impl.isSNaN(); }
bool SoftFloat::isQNaN() const { return impl.isQNaN(); }
bool SoftDouble::isQNaN() const { return impl.isQNaN(); }
bool SoftFloat::isNaN() const { return impl.isNaN(); }
bool SoftDouble::isNaN() const { return impl.isNaN(); }
bool SoftFloat::isDenormalized() const { return impl.isDenormalized(); }
bool SoftDouble::isDenormalized() const { return impl.isDenormalized(); }

void SoftFloat::setQuiet() { impl.querySMantissa().setBitArray(impl.bitSizeMantissa()-1,1); }
void SoftDouble::setQuiet() { impl.querySMantissa().setBitArray(impl.bitSizeMantissa()-1,1); }

S32 SoftDouble::queryS32( Flags& flags )
{
  impl_type::IntConversion icResult;
  impl.retrieveInteger(icResult.setSigned(), flags.impl);
  return (int32_t) icResult.queryInt();
}

S64 SoftDouble::queryS64( Flags& flags )
{
  typedef Flags::Impl IFlags;
  IFlags& iflags = flags.impl;
  
  if (impl.isNaN())
    {
      if (impl.isSNaN())
        iflags.setSNaNOperand();
      return S64(1ll << 63);
    }
  else if (impl.isZero())
    {
      return S64(0);
    }

  bool neg = impl.isNegative();
  
  int bitSizeMantissa = impl.bitSizeMantissa();
  
  S64 src(0), msb(1ll << bitSizeMantissa);
  for (int ubsz = (sizeof(unsigned int)*8), idx = (bitSizeMantissa-1)/ubsz; idx >= 0; --idx)
    src = (src << ubsz) | impl.queryMantissa()[idx];
  src = (src & (msb-1)) | msb;
  
  int exponent = impl.queryExponentValue()-bitSizeMantissa;

  if (exponent >= 0)
    {
      if (neg) src = -src;
      S64 result = src << exponent;
      if (exponent < 64 and (result >> exponent) == src)
        return result; // S64 is enough to encode result
      iflags.setOverflow();
      iflags.setApproximate(neg ? IFlags::Up : IFlags::Down);
      result = S64(1ll << 63);
      return neg ? result : ~result;
    }

  S64 const halfbit( 1ll << (-1-exponent) );
  bool half = src & halfbit;
  bool tail = ((-exponent > 64) ? src : src & (halfbit-1)) != S64(0);
  S64  result = (-exponent >= 64) ? S64(0) : src >> -exponent;
  
  if (half or tail)
    {
      struct NearestRound
      {
        bool increment;
        NearestRound( IFlags& iflags, bool odd, bool half, bool tail )
          : increment(false)
        {
          if (not iflags.isNearestRound()) 
            return;
          iflags.clearEffectiveRoundToEven();
          if (not half) // close to zero ?
            return;
          if ((increment = tail)) // close to one ?
            return;
           // Half way...
          iflags.setEffectiveRoundToEven();
          increment = odd;
        }
      };
      if ((iflags.isHighestRound() and not neg) or
          (iflags.isLowestRound()  and     neg) or
          NearestRound(iflags, result & 1, half, tail).increment)
        result += 1;
    }

  return neg ? -result : result;
}

void
SoftFloat::sqrtAssign( Flags& flags )
{
  SoftDouble rse(*this, flags);
    
  rse.sqrtAssign( flags );

  convertAssign( rse, flags );
}
  
