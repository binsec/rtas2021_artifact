/*
 *  Copyright (c) 2007,
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
 
#ifndef __E5500FPV_FLOATING_HH__
#define __E5500FPV_FLOATING_HH__

#include <types.hh>
#include <unisim/util/simfloat/floating.hh>
#include <inttypes.h>

struct Flags
{
  struct Impl
  {
    enum RoundMode { RMNearest, RMLowest, RMHighest, RMZero };
    enum Approximation { AExact, ADownApproximate, AUpApproximate };
    enum ReadResult { RRTotal, RRPartial };
    enum QNaNResult
      {  QNNRUndefined, QNNRInftyMinusInfty, QNNRInftyOnInfty, QNNRZeroOnZero,
         QNNRInftyMultZero
      };
    enum FlowException { FENoException, FEOverflow, FEUnderflow, FEEnd };
      
    Impl()
      : fAvoidAllInfty(false), fAvoidDenormalized(false), fRoundToEven(true)
      , rmRound(RMNearest), aApproximation(AExact), rrReadResult(RRTotal)
      , fEffectiveRoundToEven(false), fSNaNOperand(false), qnrQNaNResult(QNNRUndefined)
      , feExcept(FENoException), fDivisionByZero(false)
    {}

    Impl(const Impl& rpSource)
      : fAvoidAllInfty(rpSource.fAvoidAllInfty), fAvoidDenormalized(rpSource.fAvoidDenormalized)
      , fRoundToEven(rpSource.fRoundToEven), rmRound(rpSource.rmRound)
      , aApproximation(rpSource.aApproximation), rrReadResult(rpSource.rrReadResult)
      , fEffectiveRoundToEven(rpSource.fEffectiveRoundToEven), fSNaNOperand(rpSource.fSNaNOperand), qnrQNaNResult(rpSource.qnrQNaNResult)
      , feExcept(rpSource.feExcept), fDivisionByZero(rpSource.fDivisionByZero)
    {}

    Impl& operator=(const Impl& rpSource)
    {
      aApproximation = rpSource.aApproximation;
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
    bool upApproximateInfty() const { return true; }
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

    // dynamic read parameters
    Impl& setNearestRound()   { rmRound = RMNearest; return *this; }
    Impl& setHighestRound()   { rmRound = RMHighest; return *this; }
    Impl& setLowestRound()    { rmRound = RMLowest; return *this; }
    Impl& setZeroRound()      { rmRound = RMZero; return *this; }

    bool isLowestRound() const { return rmRound == RMLowest; }
    bool isNearestRound() const { return rmRound == RMNearest; }
    bool isHighestRound() const { return rmRound == RMHighest; }
    bool isZeroRound() const { return rmRound == RMZero; }

    bool keepSignalingConversion() const { return false; }

    // dynamic write parameters
    bool isApproximate() const { return aApproximation != AExact; }
    bool isDownApproximate() const { return aApproximation == ADownApproximate; }
    bool isUpApproximate() const { return aApproximation == AUpApproximate; }
    Approximation getApproximation() const { return aApproximation; }

    void setDownApproximate() { aApproximation = ADownApproximate; }
    void setUpApproximate() { aApproximation = AUpApproximate; }
    void clearApproximate() { aApproximation = AExact; }
    enum Direction { Down = 0, Up };
    void setApproximate(Direction dDirection) {  aApproximation = ((dDirection == Down) ? ADownApproximate : AUpApproximate); }
    bool isApproximate(Direction dDirection) const {  return aApproximation == ((dDirection == Down) ? ADownApproximate : AUpApproximate); }
    bool hasSameApproximation(const Impl& rpSource) const {  return aApproximation == rpSource.aApproximation; }
    bool hasIncrementFraction(bool fNegative) const {  return fNegative ? isDownApproximate() : isUpApproximate(); }

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
    {
      aApproximation = AExact;
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
      
    void setRoundingMode(unsigned int rn_mode)
    {
      switch(rn_mode)
        {
        case PPC_NEAREST:  setNearestRound();  break;
        case PPC_ZERO:     setZeroRound();     break;
        case PPC_UP:       setHighestRound();  break;
        case PPC_DOWN:     setLowestRound();   break;
        }
    }
  
    static const unsigned int PPC_NEAREST = 0;
    static const unsigned int PPC_ZERO = 1;
    static const unsigned int PPC_UP = 2;
    static const unsigned int PPC_DOWN = 3;

  private:
    bool fAvoidAllInfty;
    bool fAvoidDenormalized;
    bool fRoundToEven;
      
    RoundMode rmRound;

    Approximation aApproximation;
    ReadResult rrReadResult;
    bool fEffectiveRoundToEven;
    bool fSNaNOperand;
    QNaNResult qnrQNaNResult;
    FlowException feExcept;
    bool fDivisionByZero;
  } impl;

  struct RoundingMode { RoundingMode(unsigned int rm) : mode(rm) {} unsigned int mode; };
  Flags( RoundingMode const& rm ) : impl() { impl.setRoundingMode( rm.mode ); }
  
  bool hasIncrementFraction(bool neg) const { return impl.hasIncrementFraction(neg); } // FPSCR.FR
  bool isApproximate() const { return impl.isApproximate(); }                          // FPSCR.FI
  bool isOverflow() const { return impl.isOverflow(); }                                // FPSCR.OX
  bool isUnderflow() const { return impl.isUnderflow(); }                              // FPSCR.UX
  bool isDivisionByZero() const { return impl.isDivisionByZero(); }                    // FPSCR.ZX
  bool hasSNaNOperand() const { return impl.hasSNaNOperand(); }                        // FPSCR.VXSNAN
  bool isInftyMinusInfty() const { return impl.isInftyMinusInfty(); }                  // FPSCR.VXISI
  bool isInftyOnInfty() const { return impl.isInftyOnInfty(); }                        // FPSCR.VXIDI
  bool isInftyMultZero() const { return impl.isInftyMultZero(); }                      // FPSCR.VXIMZ
  bool isZeroOnZero() const { return impl.isZeroOnZero(); }                            // FPSCR.VXZDZ
};

class SoftFloat;
class SoftDouble;

struct BuiltFloatTraits : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<23, 8>
{
  typedef Flags::Impl StatusAndControlFlags;
  
  struct MultExtension : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<23, 8>::MultExtension
  {
    typedef Flags::Impl StatusAndControlFlags;
  };
};

struct SoftFloat
{
  typedef unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltFloatTraits> impl_type;

  enum ComparisonResult
    { CRNaN=impl_type::CRNaN, CRLess=impl_type::CRLess, CREqual=impl_type::CREqual, CRGreater=impl_type::CRGreater };
  
  SoftFloat() : impl() {}
  
  enum __FromRawBits__ { FromRawBits };
  SoftFloat(__FromRawBits__, uint32_t source) { fromRawBitsAssign( source ); }
  SoftFloat(SoftDouble const& source, Flags& flags) { convertAssign( source, flags ); }
  
  SoftFloat& assign(const SoftFloat& source);
  SoftFloat& operator=(const SoftFloat& source);
  
  SoftFloat& fromRawBitsAssign(uint32_t source);
  SoftFloat& convertAssign(const SoftDouble& op1, Flags& flags);
  
  U32 queryRawBits() const;
  
  void plusAssign(const SoftFloat& op1, Flags& flags);
  void minusAssign(const SoftFloat& op1, Flags& flags);
  void multAssign(const SoftFloat& op1, Flags& flags);
  void divAssign(const SoftFloat& op1, Flags& flags);
  void multAndAddAssign(const SoftFloat& a, const SoftFloat& b, Flags& flags);
  void multAndSubAssign(const SoftFloat& a, const SoftFloat& b, Flags& flags);
  void opposite();
  void setQuiet();
  void setPositive();
  void setNegative();
  
  void sqrtAssign( Flags& flags );
  
  ComparisonResult compare( SoftFloat const& rhs ) const;
  
  BOOL isNegative() const;
  // BOOL isPositive() const;
  BOOL isZero() const;
  BOOL isInfty() const;
  BOOL isSNaN() const;
  BOOL isQNaN() const;
  BOOL isNaN() const;
  BOOL isDenormalized() const;
  
  impl_type impl;
};

struct BuiltDoubleTraits : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<52, 11>
{
  typedef Flags::Impl StatusAndControlFlags;
  struct MultExtension : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<52, 11>::MultExtension
  {
    typedef Flags::Impl StatusAndControlFlags;
  };
};

struct SoftDouble
{
  typedef unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltDoubleTraits> impl_type;

  enum ComparisonResult
    { CRNaN=impl_type::CRNaN, CRLess=impl_type::CRLess, CREqual=impl_type::CREqual, CRGreater=impl_type::CRGreater };
  
  SoftDouble() : impl() {}
  
  enum __FromRawBits__ { FromRawBits };
  SoftDouble(__FromRawBits__, uint64_t source) { fromRawBitsAssign( source ); }
  
  SoftDouble(SoftFloat const& source, Flags& flags) { convertAssign(source, flags); }
  SoftDouble(int64_t, Flags& flags);
  
  SoftDouble& assign(const SoftDouble& source);
  SoftDouble& operator=(const SoftDouble& source);
  
  SoftDouble& fromRawBitsAssign(uint64_t source);
  SoftDouble& convertAssign(const SoftFloat& op1, Flags& flags);
  
  U64  queryRawBits() const;
  S32  queryS32( Flags& flags );
  S64  queryS64( Flags& flags );
  
  void plusAssign(const SoftDouble& op1, Flags& flags);
  void minusAssign(const SoftDouble& op1, Flags& flags);
  void multAssign(const SoftDouble& op1, Flags& flags);
  void divAssign(const SoftDouble& op1, Flags& flags);
  void multAndAddAssign(const SoftDouble& a, const SoftDouble& b, Flags& flags);
  void multAndSubAssign(const SoftDouble& a, const SoftDouble& b, Flags& flags);
  void opposite();
  void setQuiet();
  void setPositive();
  void setNegative();
  
  void sqrtAssign( Flags& flags )
  {
    SoftDouble rse(*this);
    
    rse.rSqrtEstimAssign();

    fromRawBitsAssign( U64( 0x3ff0000000000000ULL ) ); // 1. 
    
    divAssign(rse,flags);
  }
  
  void rSqrtEstimAssign()
  {
    // First estimation of 1/sqrt(b), seed of Newton-Raphson algorithm
    // see http://www.mceniry.net/papers/Fast%20Inverse%20Square%20Root.pdf
    // mirror: http://www.daxia.com/bibis/upload/406Fast_Inverse_Square_Root.pdf
    // see also: http://www.azillionmonkeys.com/qed/sqroot.html

    union  { double as_fp_number; uint64_t as_unsigned_integer; } transfer;
    transfer.as_unsigned_integer = queryRawBits();
    double x = transfer.as_fp_number;
    transfer.as_unsigned_integer = (0xbfcdd6a18f6a6f55ULL - transfer.as_unsigned_integer) >> 1;
    double y = transfer.as_fp_number;
    
    // Five Newton-Raphson iterations are sufficient with 64-bit
    // floating point numbers
    for (unsigned i = 0; i < 5; i++)
      {
        y = y*(3 - x*y*y) / 2;
      }
    
    transfer.as_fp_number = y;
    fromRawBitsAssign( transfer.as_unsigned_integer );
  }

  ComparisonResult compare( SoftDouble const& rhs ) const;
  
  BOOL isNegative() const;
  // BOOL isPositive() const;
  BOOL isZero() const;
  BOOL isInfty() const;
  BOOL isSNaN() const;
  BOOL isQNaN() const;
  BOOL isNaN() const;
  BOOL isDenormalized() const;

  impl_type impl;
};

#endif /*  __E5500FPV_FLOATING_HH__ */
