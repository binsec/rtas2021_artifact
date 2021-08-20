/*
 *  Copyright (c) 2015,
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

#ifndef E5500_SIMTEST_SIMFLOAT_HH
#define E5500_SIMTEST_SIMFLOAT_HH

#include <types.hh>
#include <unisim/util/symbolic/symbolic.hh>

namespace ut
{
  struct Flags
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    struct RoundingMode : public Expr
    {
      RoundingMode(UINT const& rm) : Expr( rm.expr ) {}
      RoundingMode(unsigned rm) : Expr( unisim::util::symbolic::make_const<unsigned>( rm ) ) {}
    };

    Flags( RoundingMode const& _rm ) : rm(_rm) {}
    
    BOOL hasIncrementFraction(BOOL neg) const { return make_function("fraction_rounded", op); }
    BOOL isApproximate() const { return make_function("isApproximate", op); }         // FPSCR.FI
    BOOL isOverflow() const { return make_function("isOverflow", op); }               // FPSCR.OX
    BOOL isUnderflow() const { return make_function("isUnderflow", op); }             // FPSCR.UX
    BOOL isDivisionByZero() const { return make_function("isDivisionByZero", op); }   // FPSCR.ZX
    BOOL hasSNaNOperand() const { return make_function("hasSNaNOperand", op); }       // FPSCR.VXSNAN
    BOOL isInftyMinusInfty() const { return make_function("isInftyMinusInfty", op); } // FPSCR.VXISI
    BOOL isInftyOnInfty() const { return make_function("isInftyOnInfty", op); }       // FPSCR.VXIDI
    BOOL isInftyMultZero() const { return make_function("isInftyMultZero", op); }     // FPSCR.VXIMZ
    BOOL isZeroOnZero() const { return make_function("isZeroOnZero", op); }           // FPSCR.VXZDZ

    void from( Expr const& _op ) { op = _op; }

    Expr rm, op;
  };

  struct SoftFloat;
  struct SoftDouble;

  template <class FP, class ARCH>
  int FPCompare( ARCH& arch, FP const& lhs, FP const& rhs )
  {
    bool ge = arch.Choose( make_function( "GE", lhs.expr, rhs.expr ) );
    bool le = arch.Choose( make_function( "LE", lhs.expr, rhs.expr ) );
    return 2*int(ge) + int(le);
  }

  template <class FP>
  int FPCompare( FP const& lhs, FP const& rhs )
  {
    Arch*         arch = ArchExprNode::SeekArch(lhs.expr);
    if (not arch) arch = ArchExprNode::SeekArch(rhs.expr);
    if (not arch) throw 0;
    return FPCompare( *arch, lhs, rhs );
  }

  struct SoftFloat
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;

    enum ComparisonResult
      { CRNaN = 0, CRLess = 1, CRGreater = 2, CREqual = 3 };
    
    enum __FromRawBits__ { FromRawBits };
    SoftFloat( __FromRawBits__, U32 const& source ) : expr(make_function("RawU32ToFloat", source.expr)) {}
    SoftFloat( SoftDouble const& source, Flags& flags );
    
    U32 queryRawBits() const { return Expr(make_function("FloatToRawU32", expr)); }

    void plusAssign(SoftFloat const& s, Flags& f) { expr = make_function("plus",s.expr,f.rm); f.from(expr); }
    void minusAssign(SoftFloat const& s, Flags& f) { expr = make_function("minus",s.expr,f.rm); f.from(expr); }
    void multAssign(SoftFloat const& s, Flags& f) { expr = make_function("mult",s.expr,f.rm); f.from(expr); }
    void divAssign(SoftFloat const& s, Flags& f) { expr = make_function("div",s.expr,f.rm); f.from(expr); }
    void multAndAddAssign(SoftFloat const& s1, SoftFloat const& s2, Flags& f)
    { expr = make_function("multAndAdd",s1.expr,s2.expr,f.rm); f.from(expr); }
    void multAndSubAssign(SoftFloat const& s1, SoftFloat const& s2, Flags& f)
    { expr = make_function("multAndSub",s1.expr,s2.expr,f.rm); f.from(expr); }
    void opposite() { expr = make_function("opposite",expr); }
    void setQuiet() { expr = make_function("setQuiet",expr); }
    void setPositive() { expr = make_function("setPos",expr); }
    
    ComparisonResult compare( SoftFloat const& rhs ) const { return ComparisonResult( FPCompare( *this, rhs ) ); }

    BOOL isNegative() const { return make_function("isNeg",expr); }
    BOOL isNaN() const { return make_function("isNaN", expr); }
    BOOL isQNaN() const { return make_function("isQNaN", expr); }
    BOOL isZero() const { return make_function("isZero",expr); }

    unisim::util::symbolic::Expr expr;
  };

  struct SoftDouble
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;

    enum ComparisonResult
      { CRNaN = 0, CRLess = 1, CRGreater = 2, CREqual = 3 };
  
    SoftDouble() : expr() {}
    
    enum __FromRawBits__ { FromRawBits };
    SoftDouble( __FromRawBits__, U64 const& source ) { fromRawBitsAssign(source); }
    
    SoftDouble(SoftFloat const& source, Flags& flags) { convertAssign(source, flags); }
    SoftDouble( S64 const& source, Flags& flags )
      : expr(make_function("S64ToDouble", source.expr, flags.rm))
    { flags.from( expr ); }

    SoftDouble& convertAssign( SoftFloat const& source, Flags& flags )
    {
      expr = make_function("FloatToDouble", source.expr, flags.rm);
      flags.from( expr );
      return *this;
    }
    SoftDouble& fromRawBitsAssign( U64 const& source )
    {
      expr = make_function( "RawU64ToDouble", source.expr );
      return *this;
    }
    
    U64 queryRawBits() const { return make_function( "DoubleToRawU64", expr ); }
    
    S64 queryS64( Flags& flags ) const
    {
      Expr res( make_function("DoubleToS64", expr, flags.rm) );
      flags.from(res);
      return res;
    }
    S32 queryS32( Flags& flags ) const
    {
      Expr res( make_function("DoubleToS32", expr, flags.rm) );
      flags.from(res);
      return res;
    }

    void plusAssign(SoftDouble const& s, Flags& f) { expr = make_function("plus",s.expr,f.rm); f.from(expr); }
    void minusAssign(SoftDouble const& s, Flags& f) { expr = make_function("minus",s.expr,f.rm); f.from(expr); }
    void multAssign(SoftDouble const& s, Flags& f) { expr = make_function("mult",s.expr,f.rm); f.from(expr); }
    void divAssign(SoftDouble const& s, Flags& f) { expr = make_function("div",s.expr,f.rm); f.from(expr); }
    void multAndAddAssign(SoftDouble const& s1, SoftDouble const& s2, Flags& f)
    { expr = make_function("multAndAdd",s1.expr,s2.expr,f.rm); f.from(expr); }
    void multAndSubAssign(SoftDouble const& s1, SoftDouble const& s2, Flags& f)
    { expr = make_function("multAndSub",s1.expr,s2.expr,f.rm); f.from(expr); }
    void opposite() { expr = make_function("opposite",expr); }
    void setQuiet() { expr = make_function("setQuiet",expr); }
    void setPositive() { expr = make_function("setPos",expr); }
    void setNegative() { expr = make_function("setNeg",expr); }
    void rSqrtEstimAssign() { expr = make_function("rSqrtEstim",expr); }
    
    void sqrtAssign(Flags& flags) { expr = make_function("sqrt",expr,flags.rm); flags.from(expr); }
    
    ComparisonResult compare( SoftDouble const& rhs ) const { return ComparisonResult( FPCompare( *this, rhs ) ); }

    BOOL isNegative() const { return make_function("isNeg",expr); }
    BOOL isNaN() const { return make_function("isNaN",expr); }
    BOOL isQNaN() const { return make_function("isQNaN",expr); }
    BOOL isSNaN() const { return make_function("isSNaN",expr); }
    BOOL isZero() const { return make_function("isZero",expr); }
    BOOL isInfty() const { return make_function("isInfty",expr); }

    unisim::util::symbolic::Expr expr;
  };

  inline SoftFloat::SoftFloat( SoftDouble const& source, Flags& flags )
    : expr(make_function("DoubleToFloat", source.expr, flags.rm))
  { flags.from( expr ); }

  template <typename T> void GenFPSCR_OX(U64& r, Flags const& f) { r.expr = make_function("GenFPSCR_OX",r.expr,f.op); }
  template <typename T> void GenFPSCR_UX(U64& r, Flags const& f) { r.expr = make_function("GenFPSCR_UX",r.expr,f.op); }
  template <typename T> void GenFPSCR_VXSNAN(U64& r, Flags const& f) { r.expr = make_function("GenFPSCR_VXSNAN",r.expr,f.op); }
  template <typename T> void GenFPSCR_VXISI(U64& r, Flags const& f) { r.expr = make_function("GenFPSCR_VXISI",r.expr,f.op); }
  template <typename T> void GenFPSCR_VXCVI(U64& r, Flags const& f) { r.expr = make_function("GenFPSCR_VXISI",r.expr,f.op); }
  template <typename T,typename D> void GenFPSCR_FR(U64& r, D const& d, Flags const& f) { r.expr = make_function("GenFPSCR_FR",r.expr,f.op); }
  template <typename T> void GenFPSCR_FI(U64& r, Flags const& f) { r.expr = make_function("GenFPSCR_FI",r.expr,f.op); }
  template <typename T,typename D> void GenFPSCR_FPRF(U64& r, D const& d) { r.expr = make_function("GenFPSCR_FI",r.expr,d.expr); }
  template <typename T> void GenFPSCR_XX(U64& r) { r.expr = make_function("GenFPSCR_XX",r.expr); }
  template <typename T> void GenFPSCR_FX(U64& r, U64& ar) { r.expr = make_function("GenFPSCR_FX",r.expr,ar.expr); }
  template <typename T> void GenFPSCR_VX(U64& r) { r.expr = make_function("GenFPSCR_VX",r.expr); }
  template <typename T> void GenFPSCR_FEX(U64& r) { r.expr = make_function("GenFPSCR_FEX",r.expr); }
}

#endif // E5500_SIMTEST_SIMFLOAT_HH
