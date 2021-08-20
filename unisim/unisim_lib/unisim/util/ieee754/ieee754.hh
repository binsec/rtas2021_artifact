/*
 *  Copyright (c) 2014,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_UTIL_IEEE754_IEEE754_HH__
#define __UNISIM_UTIL_IEEE754_IEEE754_HH__

#include <unisim/util/simfloat/floating.hh>

#include <inttypes.h>
#include <string>

namespace unisim {
namespace util {
namespace ieee754 {

   class Flags {
     protected:
      enum RoundMode { RMNearest, RMLowest, RMHighest, RMZero };
      enum Approximation { AExact, ADownApproximate, AUpApproximate };
      enum ReadResult { RRTotal, RRPartial };
      enum QNaNResult
         {  QNNRUndefined, QNNRInftyMinusInfty, QNNRInftyOnInfty, QNNRZeroOnZero,
            QNNRInftyMultZero
         };
      enum FlowException { FENoException, FEOverflow, FEUnderflow, FEEnd };
      
     private:
      bool fAvoidAllInfty;
      bool fAvoidDenormalized;
      bool fRoundToEven;
      bool fUpApproximateInfty;
      
      RoundMode rmRound;
      bool fKeepSignalingConversion;

      Approximation aApproximation;
      ReadResult rrReadResult;
      bool fEffectiveRoundToEven;
      bool fSNaNOperand;
      QNaNResult qnrQNaNResult;
      FlowException feExcept;
      bool fDivisionByZero;

     public:
      Flags()
         :  fAvoidAllInfty(false), fAvoidDenormalized(false), fRoundToEven(true),
            fUpApproximateInfty(false), rmRound(RMNearest),
            fKeepSignalingConversion(true), aApproximation(AExact), rrReadResult(RRTotal),
            fEffectiveRoundToEven(false), fSNaNOperand(false), qnrQNaNResult(QNNRUndefined),
            feExcept(FENoException), fDivisionByZero(false) {}
      Flags(const Flags& rpSource)
         :  fAvoidAllInfty(rpSource.fAvoidAllInfty), fAvoidDenormalized(rpSource.fAvoidDenormalized),
            fRoundToEven(rpSource.fRoundToEven), fUpApproximateInfty(rpSource.fUpApproximateInfty),
            rmRound(rpSource.rmRound), fKeepSignalingConversion(rpSource.fKeepSignalingConversion),
            aApproximation(rpSource.aApproximation), rrReadResult(rpSource.rrReadResult),
            fEffectiveRoundToEven(rpSource.fEffectiveRoundToEven), fSNaNOperand(rpSource.fSNaNOperand), qnrQNaNResult(rpSource.qnrQNaNResult),
            feExcept(rpSource.feExcept), fDivisionByZero(rpSource.fDivisionByZero) {}

      Flags& operator=(const Flags& rpSource)
         {  aApproximation = rpSource.aApproximation;
            rrReadResult = rpSource.rrReadResult;
            fEffectiveRoundToEven = rpSource.fEffectiveRoundToEven;
            fSNaNOperand = rpSource.fSNaNOperand;
            qnrQNaNResult = rpSource.qnrQNaNResult;
            feExcept = rpSource.feExcept;
            fDivisionByZero = rpSource.fDivisionByZero;
            return *this;
         }
      bool isRoundToEven() const { return fRoundToEven && isNearestRound(); }
      bool isPositiveZeroMAdd() { return true; }
      bool isInftyAvoided() const { return true; }
      bool isAllInftyAvoided() const { return fAvoidAllInfty; }
      bool doesAvoidInfty(bool fNegative) const
         {  return fAvoidAllInfty || (fNegative ? (rmRound >= RMHighest) : (rmRound & RMLowest)); }
      bool isDenormalizedAvoided() const { return fAvoidDenormalized; }
      bool keepNaNSign() const { return true; }
      bool produceMultNaNPositive() const { return true; }
      bool produceDivNaNPositive() const { return true; }
      bool produceAddNaNPositive() const { return true; }
      bool produceSubNaNPositive() const { return true; }
      bool upApproximateInfty() const { return fUpApproximateInfty; }
      bool upApproximateInversionForNear() const { return true; }
      bool chooseNaNAddBeforeMult() const { return true; }
      bool isConvertNaNNegative() const { return true; }
      bool acceptMinusZero() const { return true; }

      void setAvoidAllInfty()   { fAvoidAllInfty = true; }
      void clearAvoidAllInfty() { fAvoidAllInfty = false; }
      void setAvoidDenormalized() { fAvoidDenormalized = true; }
      void clearAvoidDenormalized() { fAvoidDenormalized = false; }
      void setRoundToEven() { fRoundToEven = true; }
      void clearRoundToEven() { fRoundToEven = false; }
      void setUpApproximateInfty() { fUpApproximateInfty = true; }
      void clearUpApproximateInfty() { fUpApproximateInfty = false; }

      // dynamic read parameters
      Flags& setNearestRound()   { rmRound = RMNearest; return *this; }
      Flags& setHighestRound()   { rmRound = RMHighest; return *this; }
      Flags& setLowestRound()    { rmRound = RMLowest; return *this; }
      Flags& setZeroRound()      { rmRound = RMZero; return *this; }

      bool isLowestRound() const { return rmRound == RMLowest; }
      bool isNearestRound() const { return rmRound == RMNearest; }
      bool isHighestRound() const { return rmRound == RMHighest; }
      bool isZeroRound() const { return rmRound == RMZero; }

      Flags& setKeepSignalingConversion() { fKeepSignalingConversion = true; return *this; }
      Flags& clearKeepSignalingConversion() { fKeepSignalingConversion = false; return *this; }
      bool keepSignalingConversion() const { return fKeepSignalingConversion; }

      // dynamic write parameters
      bool isApproximate() const { return aApproximation != AExact; }
      bool isDownApproximate() const { return aApproximation == ADownApproximate; }
      bool isUpApproximate() const { return aApproximation == AUpApproximate; }
      void setDownApproximate() { aApproximation = ADownApproximate; }
      void setUpApproximate() { aApproximation = AUpApproximate; }
      void clearApproximate() { aApproximation = AExact; }
      enum Direction { Down, Up };
      void setApproximate(Direction dDirection)
         {  aApproximation = ((dDirection == Down) ? ADownApproximate : AUpApproximate); }
      bool isApproximate(Direction dDirection) const
         {  return aApproximation == ((dDirection == Down) ? ADownApproximate : AUpApproximate); }
      bool hasSameApproximation(const Flags& rpSource) const
         {  return aApproximation == rpSource.aApproximation; }
      bool hasIncrementFraction(bool fNegative) const
         {  return fNegative ? isDownApproximate() : isUpApproximate(); }

      void setEffectiveRoundToEven() { fEffectiveRoundToEven = true; }
      void clearEffectiveRoundToEven() { fEffectiveRoundToEven = false; }
      bool hasEffectiveRoundToEven() { return fEffectiveRoundToEven; }

      void setPartialRead() { rrReadResult = RRPartial; }
      void setTotalRead() { rrReadResult = RRTotal; }
      bool isPartialRead() const { return rrReadResult == RRPartial; }
      bool isTotalRead() const { return rrReadResult == RRTotal; }
      bool hasPartialRead() const { return rrReadResult != RRTotal; }

      void setSNaNOperand() { fSNaNOperand = true; }
      bool hasSNaNOperand() const { return fSNaNOperand; }
      
      void setInftyMinusInfty() { assert(!qnrQNaNResult); qnrQNaNResult = QNNRInftyMinusInfty; }
      void setInftyOnInfty() { assert(!qnrQNaNResult); qnrQNaNResult = QNNRInftyOnInfty; }
      void setZeroOnZero() { assert(!qnrQNaNResult); qnrQNaNResult = QNNRZeroOnZero; }
      void setInftyMultZero() { assert(!qnrQNaNResult); qnrQNaNResult = QNNRInftyMultZero; }
      bool hasQNaNResult() const { return qnrQNaNResult; }
      bool isInftyMinusInfty() const { return qnrQNaNResult == QNNRInftyMinusInfty; }
      bool isInftyOnInfty() const { return qnrQNaNResult == QNNRInftyOnInfty; }
      bool isZeroOnZero() const { return qnrQNaNResult == QNNRZeroOnZero; }
      bool isInftyMultZero() const { return qnrQNaNResult == QNNRInftyMultZero; }

      void clear()
         {  aApproximation = AExact;
            rrReadResult = RRTotal;
            fEffectiveRoundToEven = false;
            fSNaNOperand = false;
            qnrQNaNResult = QNNRUndefined;
            feExcept = FENoException;
            fDivisionByZero = false;
         }

      bool isDivisionByZero() const { return fDivisionByZero; }
      void setDivisionByZero() { fDivisionByZero = true; }
      void clearFlowException() { feExcept = FENoException; }
      void setOverflow() { feExcept = FEOverflow; }
      void setUnderflow() { feExcept = FEUnderflow; }
      bool isOverflow() const { return feExcept == FEOverflow; }
      bool isUnderflow() const { return feExcept == FEUnderflow; }
   };

class BuiltDoubleTraits : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<52, 11> {
  public:
   typedef Flags StatusAndControlFlags;
   class MultExtension : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<52, 11>::MultExtension {
     public:
      typedef Flags StatusAndControlFlags;
   };
};

class SoftFloat;
class SoftDouble : public unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltDoubleTraits> {
  private:
   typedef unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltDoubleTraits> inherited;

  public:
   SoftDouble() : inherited() {}
   SoftDouble(const SoftFloat& sfFloat, Flags& rpParams);
   SoftDouble(const uint64_t& uDouble) { setChunk((void *) &uDouble, unisim::util::endian::IsHostLittleEndian()); }
   SoftDouble& operator=(const SoftDouble& sdSource)
      {  return (SoftDouble&) inherited::operator=(sdSource); }
   SoftDouble& assign(const SoftDouble& sdSource)
      {  return (SoftDouble&) inherited::operator=(sdSource); }
   SoftDouble& assign(const SoftFloat& sfFloat, Flags& rpParams);

   uint64_t queryValue() const
      {  uint64_t uResult; fillChunk(&uResult, unisim::util::endian::IsHostLittleEndian()); return uResult; }
};

class BuiltFloatTraits : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<23, 8> {
  public:
   typedef Flags StatusAndControlFlags;
   class MultExtension : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<23, 8>::MultExtension {
     public:
      typedef Flags StatusAndControlFlags;
   };
};

class SoftFloat : public unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltFloatTraits> {
  private:
   typedef unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltFloatTraits> inherited;

  public:
   SoftFloat() : inherited() {}
   SoftFloat(const SoftDouble& sdDouble, Flags& rpParams);
   SoftFloat(const uint32_t& uFloat) { setChunk((void *) &uFloat, unisim::util::endian::IsHostLittleEndian()); }

   SoftFloat& operator=(const SoftFloat& sfSource)
      {  return (SoftFloat&) inherited::operator=(sfSource); }
   SoftFloat& assign(const SoftFloat& sfSource)
      {  return (SoftFloat&) inherited::operator=(sfSource); }
   SoftFloat& assign(const SoftDouble& sdDouble, Flags& rpParams);
   uint32_t queryValue() const
      {  uint32_t uResult; fillChunk(&uResult, unisim::util::endian::IsHostLittleEndian()); return uResult; }
};

inline SoftDouble&
SoftDouble::assign(const SoftFloat& sfFloat, Flags& rpParams) {
   FloatConversion fcConversion;
   fcConversion.setSizeMantissa(23).setSizeExponent(8);
   fcConversion.setNegative(sfFloat.isNegative());
   fcConversion.exponent()[0] = sfFloat.queryBasicExponent()[0];
   fcConversion.mantissa()[0] = sfFloat.queryMantissa()[0];
   inherited source(fcConversion, rpParams);
   return (SoftDouble&) inherited::operator=(source);
}

inline
SoftDouble::SoftDouble(const SoftFloat& sfFloat, Flags& rpParams)
   { assign(sfFloat, rpParams); }

inline SoftFloat&
SoftFloat::assign(const SoftDouble& sdDouble, Flags& rpParams) {
   FloatConversion fcConversion;
   fcConversion.setSizeMantissa(52).setSizeExponent(11);
   fcConversion.setNegative(sdDouble.isNegative());
   fcConversion.exponent()[0] = sdDouble.queryBasicExponent()[0];
   fcConversion.mantissa()[0] = sdDouble.queryMantissa()[0];
   fcConversion.mantissa()[1] = sdDouble.queryMantissa()[1];
   return (SoftFloat&) inherited::operator=(inherited(fcConversion, rpParams));
}

inline
SoftFloat::SoftFloat(const SoftDouble& sdDouble, Flags& rpParams)
   { assign(sdDouble, rpParams); }

} // end of namespace ieee754
} // end of namespace util
} // end of namespace unisim

#endif
