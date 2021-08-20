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

#ifndef E5500_SIMTEST_TYPES_HH
#define E5500_SIMTEST_TYPES_HH

#include <unisim/util/symbolic/symbolic.hh>
#include <inttypes.h>

namespace ut
{
  using unisim::util::symbolic::SmartValue;
  typedef SmartValue<bool>     BOOL;
  typedef SmartValue<uint8_t>  U8;
  typedef SmartValue<uint16_t> U16;
  typedef SmartValue<uint32_t> U32;
  typedef SmartValue<uint64_t> U64;
  typedef SmartValue<int8_t>   S8;
  typedef SmartValue<int16_t>  S16;
  typedef SmartValue<int32_t>  S32;
  typedef SmartValue<int64_t>  S64;

  typedef SmartValue<uint64_t> ADDRESS;
  typedef SmartValue<uint64_t> UINT;
  typedef SmartValue<int64_t>  SINT;
  
  template <unsigned BITS> struct TypeFor {};
  template <> struct TypeFor<8> { typedef U8 U; typedef S8 S; };
  template <> struct TypeFor<16> { typedef U16 U; typedef S16 S; };
  template <> struct TypeFor<32> { typedef U32 U; typedef S32 S; };
  template <> struct TypeFor<64> { typedef U64 U; typedef S64 S; };

  struct FunctionNodeBase : public unisim::util::symbolic::ExprNode
  {
    typedef unisim::util::symbolic::ExprNode ExprNode;
    typedef unisim::util::symbolic::Expr Expr;
    
    FunctionNodeBase( char const* _ident ) : ident(_ident) {}
    virtual void Repr( std::ostream& sink ) const;
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<FunctionNodeBase const&>( rhs ) ); }
    int compare( FunctionNodeBase const& b ) const;

    char const* ident;
  };
    
  template <unsigned SUBS>
  struct FunctionNode : FunctionNodeBase
  {
    typedef unisim::util::symbolic::ExprNode ExprNode;
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ScalarType ScalarType;
    typedef FunctionNode<SUBS> this_type;
    
    FunctionNode( char const* _ident ) : FunctionNodeBase(_ident) {}
    virtual this_type* Mutate() const { return new this_type(*this); };
    this_type* src(unsigned idx, Expr const& x) { srcs[idx] = x; return this; }
    virtual unsigned SubCount() const { return SUBS; };
    virtual Expr const& GetSub(unsigned idx) const { if (idx < SUBS) return srcs[idx]; return ExprNode::GetSub(idx); };
    virtual ScalarType::id_t GetType() const { return GetSub(0)->GetType(); }
    Expr srcs[SUBS];
  };

  /* 1 operand */
  inline unisim::util::symbolic::Expr
  make_function(char const* ident, unisim::util::symbolic::Expr const& op0)
  { return (new FunctionNode<1>(ident))->src(0,op0); }
  /* 2 operands */
  inline unisim::util::symbolic::Expr
  make_function(char const* ident,
                unisim::util::symbolic::Expr const& op0,
                unisim::util::symbolic::Expr const& op1)
  { return (new FunctionNode<2>(ident))->src(0,op0)->src(1,op1); }
  /* 3 operands */
  inline unisim::util::symbolic::Expr
  make_function(char const* ident,
                unisim::util::symbolic::Expr const& op0,
                unisim::util::symbolic::Expr const& op1,
                unisim::util::symbolic::Expr const& op2)
  { return (new FunctionNode<3>(ident))->src(0,op0)->src(1,op1)->src(2,op2); }
  
  struct Arch;

  struct ArchExprNode : public unisim::util::symbolic::ExprNode
  {
    typedef unisim::util::symbolic::ScalarType ScalarType;
    ArchExprNode( Arch& _arch ) : arch(_arch) {} Arch& arch;

    static Arch* SeekArch( unisim::util::symbolic::Expr const& expr )
    {
      if (ArchExprNode const* node = dynamic_cast<ArchExprNode const*>( expr.node ))
        return &node->arch;
      for (unsigned idx = 0, end = expr->SubCount(); idx < end; ++idx)
        if (Arch* found = SeekArch( expr->GetSub(idx)))
          return found;
      return 0;
    }
  };
}

#endif // E5500_SIMTEST_TYPES_HH
