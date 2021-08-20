/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 *          Franck Vedrine (Franck.Vedrine@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_SIMFLOAT_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_SIMFLOAT_HH__

#include <unisim/util/simfloat/floating.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

class IntConversion {
  public:
   typedef unisim::util::simfloat::Numerics::Integer::TBigCellInt<unisim::util::simfloat::Numerics::Integer::Details::TCellIntegerTraits<(64 + 1)/(8*sizeof(unsigned))> > BigInt;

  private:
   BigInt biResult;
   int uSize;
   bool fUnsigned;
   bool fNegative;

  public:
   IntConversion() : uSize(32), fUnsigned(true), fNegative(false) {}
   IntConversion(const IntConversion& source)
      :  biResult(source.biResult), uSize(source.uSize), fUnsigned(source.fUnsigned), fNegative(source.fNegative) {}

   IntConversion& setSigned() { fUnsigned = false; return *this; }
   IntConversion& setUnsigned() { fUnsigned = true; return *this; }
   IntConversion& assignSigned(const BigInt& biResultSource)
      {  assert(!fUnsigned);
         biResult = biResultSource;
         if (biResult.cbitArray(uSize-1)) {
            fNegative = true;
            biResult.neg().inc();
         }
         else
            fNegative = false;
         return *this;
      }
   IntConversion& assignUnsigned(const BigInt& biResultSource)
      {  assert(fUnsigned);
         biResult = biResultSource;
         fNegative = false;
         return *this;
      }
   IntConversion& assign(int uValue)
      {  assert(!fUnsigned);
         biResult = ((fNegative = (uValue < 0)) != false)
            ? (unsigned int) uValue : (unsigned int) -uValue;
         return *this;
      }
   IntConversion& assign(unsigned int uValue)
      {  assert(fUnsigned); biResult = uValue; return *this; }

   int querySize() const { return uSize; }
   void setSize(int uBitSize) { assert(uBitSize <= 64); uSize = uBitSize; }
   int queryMaxDigits() const { return fUnsigned ? uSize : (uSize-1); }
   bool isUnsigned() const { return fUnsigned; }
   bool isSigned() const   { return !fUnsigned; }
   BigInt queryInt() const
      {  assert(!fUnsigned);
         BigInt biReturn = biResult;
         if (fNegative)
            biReturn.neg().inc();
         return biReturn;
      }
   const BigInt& queryUnsignedInt() const { assert(fUnsigned); return biResult; }
   unsigned int queryValue() const { return biResult.queryValue(); }
   void opposite() { assert(!fUnsigned); fNegative = !fNegative; }
   bool isPositive() const { return fUnsigned || !fNegative; }
   bool isNegative() const { return !fUnsigned && fNegative; }
   bool isDifferentZero() const { return !biResult.isZero(); }
   int log_base_2() const { return biResult.log_base_2(); }
   bool hasZero(int uDigits) const { return biResult.hasZero(uDigits); }
   bool cbitArray(int uLocalIndex) const { return biResult.cbitArray(uLocalIndex); }
   IntConversion& operator>>=(int uShift)
      {  assert(isPositive()); biResult >>= uShift; return *this; }
   IntConversion& operator<<=(int uShift)
      {  /*assert(isPositive()); Required by TBuiltDouble::setInteger*/ biResult <<= uShift; return *this; }
   IntConversion& operator&=(const IntConversion& icSource)
      {  assert(isPositive() && icSource.isPositive());
         biResult &= icSource.biResult;
         return *this;
      }
   IntConversion& neg() { if (!fUnsigned) fNegative = !fNegative; biResult.neg(); return *this; }
   IntConversion& inc() { biResult.inc(); return *this; }

   IntConversion& operator=(const IntConversion& icSource)
      {  biResult = icSource.biResult;
         fUnsigned = icSource.fUnsigned;
         fNegative = icSource.fNegative;
         return *this;
      }
   typedef BigInt::BitArray BitArray;
   BitArray bitArray(int uIndex) { assert(isPositive()); return biResult.bitArray(uIndex); }
   void setBitArray(int uIndex, bool fValue)
      {  assert(isPositive()); biResult.setBitArray(uIndex, fValue); }
   void setTrueBitArray(int uIndex)
      {  assert(isPositive()); biResult.setTrueBitArray(uIndex); }
   void setFalseBitArray(int uIndex)
      {  assert(isPositive()); biResult.setFalseBitArray(uIndex); }

   void setMin()
      {  biResult.clear();
         if (!fUnsigned)
            biResult.setTrueBitArray(uSize-1);
      }
   void setMax()
      {  biResult.clear(); 
         if (fUnsigned)
            biResult.neg();
         else
            biResult.neg(uSize-1);
      }
   unsigned int& operator[](int uIndex) { return biResult[uIndex]; }
   unsigned int operator[](int uIndex) const { return biResult[uIndex]; }
};

class FloatFlags : public unisim::util::simfloat::Numerics::Double::Details::DTDoubleElement::Access::StatusAndControlFlags {
  public:
   FloatFlags() { setRoundToEven(); /*clearKeepSignalingConversion();*/ }
};

struct BuiltFloatTraits : unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<23, 8> {
   typedef FloatFlags StatusAndControlFlags;
   typedef tesla::IntConversion IntConversion;
   typedef TFloatConversion<52,11> FloatConversion;
};

struct SoftHalfIEEE;

struct SoftFloatIEEE : unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltFloatTraits>
{
private:
	typedef unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltFloatTraits> inherited;
public:
	typedef FloatFlags StatusAndControlFlags;
	SoftFloatIEEE() : inherited() {}
	SoftFloatIEEE(const uint32_t& uFloat) { setChunk((void *) &uFloat, true /* little endian */); }

	void setInteger(uint32_t value, StatusAndControlFlags& scfFlags)
	{
		scfFlags.clear();
		IntConversion icConversion;
		icConversion.setSize(32);
		icConversion.setUnsigned();
		icConversion.assignUnsigned(value);
		inherited::setInteger(icConversion, scfFlags);
	}

	void setInteger(int32_t value, StatusAndControlFlags& scfFlags)
	{
		scfFlags.clear();
		IntConversion icConversion;
		icConversion.setSize(32);
		icConversion.setSigned();
		icConversion.assignSigned(value);
		inherited::setInteger(icConversion, scfFlags);
	}


	void setInteger(IntConversion const & icConversion, StatusAndControlFlags& scfFlags)
	{
		scfFlags.clear();
		inherited::setInteger(icConversion, scfFlags);
	}

	void retrieveInteger(uint32_t& result, StatusAndControlFlags& flags) const;
	void retrieveInteger(int32_t& result, StatusAndControlFlags& flags) const;
	void retrieveInteger(uint16_t& result, StatusAndControlFlags& flags) const;
	void retrieveInteger(int16_t& result, StatusAndControlFlags& flags) const;

	SoftFloatIEEE& operator=(const SoftFloatIEEE& sfSource)
	  {  return (SoftFloatIEEE&) inherited::operator=(sfSource); }
	SoftFloatIEEE& assign(const SoftFloatIEEE& sfSource)
	  {  return (SoftFloatIEEE&) inherited::operator=(sfSource); }

	SoftFloatIEEE& assign(const SoftHalfIEEE& sfHalf, inherited::StatusAndControlFlags & flags);
	 
	uint32_t queryValue() const
	  {  uint32_t uResult; fillChunk(&uResult, true /* little endian */); return uResult; }
	  
	inline SoftFloatIEEE& saturate();
	inline SoftFloatIEEE& signedSaturate();
	void setOnePreserveSign() {
		inherited::querySBasicExponent() = inherited::getZeroExponent();
		inherited::querySMantissa() = 0U;
	}
	
	SoftFloatIEEE& roundToInt(StatusAndControlFlags & flags) {
		if(queryExponent() < UBitSizeMantissa + 1) {
			IntConversion conv;
			conv.setSize(UBitSizeMantissa + 1);
			inherited::retrieveInteger(conv, flags);	// Rounded
			inherited::setInteger(conv, flags);	// Exact op, no rounding
		}
		// otherwise, it is already an integer (or an inf/NaN).
		return *this;
	}

};

struct BuiltHalfTraits : unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<10, 5> {
   typedef FloatFlags StatusAndControlFlags;
   typedef tesla::IntConversion IntConversion;
   typedef TFloatConversion<52,11> FloatConversion;
};

struct SoftHalfIEEE : unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltHalfTraits>
{
private:
	typedef unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltHalfTraits> inherited;
public:
	typedef FloatFlags StatusAndControlFlags;
	SoftHalfIEEE() : inherited() {}
	SoftHalfIEEE(const uint16_t& uFloat) { setChunk((void *) &uFloat, true /* little endian */); }

	SoftHalfIEEE& operator=(const SoftHalfIEEE& sfSource)
	  {  return (SoftHalfIEEE&) inherited::operator=(sfSource); }
	SoftHalfIEEE& assign(const SoftHalfIEEE& sfSource)
	  {  return (SoftHalfIEEE&) inherited::operator=(sfSource); }
	SoftHalfIEEE& assign(const SoftFloatIEEE& sfFloat, inherited::StatusAndControlFlags & flags);
	uint32_t queryValue() const
	  {  uint32_t uResult; fillChunk(&uResult, true /* little endian */); return uResult; }
};


// Denormal As Zero + Flush To Zero : no denormals
template<class BaseFloat>
struct FloatDAZFTZ : BaseFloat
{
private:
	typedef FloatDAZFTZ<BaseFloat> thisType;
	typedef typename BaseFloat::StatusAndControlFlags Flags;
public:
	FloatDAZFTZ() : BaseFloat() {}
	FloatDAZFTZ(const uint32_t& uFloat) { BaseFloat::setChunk((void *) &uFloat, true /* little endian */); }

	thisType& operator=(const thisType& sfSource)
	  {  return (thisType&) BaseFloat::operator=(sfSource); }
	thisType& assign(const thisType& sfSource)
	  {  return (thisType&) BaseFloat::operator=(sfSource); }
	thisType& assign(const SoftHalfIEEE& sfHalf, Flags & flags)
	  {  return (thisType&) BaseFloat::assign(sfHalf, flags); }
	thisType& assign(const SoftFloatIEEE& sfFloat, Flags & flags);

	uint32_t queryValue() const
	  {  uint32_t uResult; BaseFloat::fillChunk(&uResult, true /* little endian */); return uResult; }

	void setZeroPreserveSign() { BaseFloat::querySBasicExponent() = 0U; BaseFloat::querySMantissa() = 0U; }
	void flushDenormals() {
		if(BaseFloat::isDenormalized()) { setZeroPreserveSign(); }
	}

	thisType& plusAssign(const thisType& bdSource, Flags& scfFlags);
	thisType& minusAssign(const thisType& bdSource, Flags& scfFlags);
	thisType& multAssign(const thisType& bdSource, Flags& scfFlags);

	thisType& operator+=(const thisType& bdSource)
		{	return BaseFloat::plusAssign(bdSource, Flags().setNearestRound()); }
	thisType operator+(const thisType& bdSource) const
	{
		thisType bdThis = *this;
		bdThis.plusAssign(bdSource, Flags().setNearestRound());
		return bdThis;
	}
	thisType& operator-=(const thisType& bdSource)
		{  return BaseFloat::minusAssign(bdSource, Flags().setNearestRound()); }
	thisType operator-(const thisType& bdSource) const
	{	thisType bdThis = *this;
		bdThis.minusAssign(bdSource, Flags().setNearestRound());
		return bdThis;
	}
	thisType& operator*=(const thisType& bdSource)
		{  return BaseFloat::multAssign(bdSource, Flags().setNearestRound()); }
	thisType operator*(const thisType& bdSource) const
	{	thisType bdThis = *this;
		bdThis.multAssign(bdSource, Flags().setNearestRound());
		return bdThis;
	}
	
};


// G80-GT200 MAD : multiplication in round toward zero, then addition in current rounding mode
// MAD is *not* a FMA
template<class BaseFloat>
struct FloatMAD : BaseFloat
{
private:
	typedef FloatMAD<BaseFloat> thisType;
	
public:
	thisType& teslaMADAssign(const thisType& bdMult, const thisType& bdAdd, typename BaseFloat::StatusAndControlFlags& scfFlags);
};

typedef FloatMAD<FloatDAZFTZ<SoftFloatIEEE> > SoftFloatTesla;



} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
