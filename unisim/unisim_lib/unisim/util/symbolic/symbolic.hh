/*
 *  Copyright (c) 2007-2017,
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
 
#ifndef __UNISIM_UTIL_SYMBOLIC_SYMBOLIC_HH__
#define __UNISIM_UTIL_SYMBOLIC_SYMBOLIC_HH__

#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/identifier/identifier.hh>
#include <ostream>
#include <map>
#include <stdexcept>
#include <limits>
#include <typeinfo>
#include <cstring>
#include <inttypes.h>

namespace unisim {
namespace util {
namespace symbolic {

  template <typename Bool> struct AssertBool {};
  template <>              struct AssertBool<bool> { static void check() {} };
  
  template <bool test> struct StaticAssert {};
  template <> struct StaticAssert<true> { static void check() {}; };
  
  template <class T, class U>  struct CmpTypes { static bool const same = false; };

  template <class T>  struct CmpTypes<T,T> { static bool const same = true; };
  
  struct ScalarType
  {
    enum id_t { VOID, BOOL, U8, U16, U32, U64, S8, S16, S32, S64, F32, F64 };
    static id_t IntegerType( bool is_signed, unsigned bits )
    {
      switch (bits) {
      default: throw VOID;
      case 1:  return BOOL;
      case 8:  return is_signed ? S8 :  U8;
      case 16: return is_signed ? S16 : U16;
      case 32: return is_signed ? S32 : U32;
      case 64: return is_signed ? S64 : U64;
      }
      return VOID;
    }
    ScalarType( id_t id )
      : name(0), bitsize(0), is_signed(false), is_integer(false)
    {
      switch (id)
        {
        case VOID: bitsize = 0;  is_integer = false; is_signed = false; name = "VOID"; break;
        case BOOL: bitsize = 1;  is_integer = true;  is_signed = false; name = "BOOL"; break;
        case U8:   bitsize = 8;  is_integer = true;  is_signed = false; name = "U8";  break;
        case S8:   bitsize = 8;  is_integer = true;  is_signed = true;  name = "S8";  break;
        case U16:  bitsize = 16; is_integer = true;  is_signed = false; name = "U16"; break;
        case S16:  bitsize = 16; is_integer = true;  is_signed = true;  name = "S16"; break;
        case U32:  bitsize = 32; is_integer = true;  is_signed = false; name = "U32"; break;
        case S32:  bitsize = 32; is_integer = true;  is_signed = true;  name = "S32"; break;
        case U64:  bitsize = 64; is_integer = true;  is_signed = false; name = "U64"; break;
        case S64:  bitsize = 64; is_integer = true;  is_signed = true;  name = "S64"; break;
        case F32:  bitsize = 32; is_integer = false; is_signed = true;  name = "F32"; break;
        case F64:  bitsize = 64; is_integer = false; is_signed = true;  name = "F64"; break;
        }
    }
    char const* name;
    unsigned bitsize;
    bool is_signed, is_integer;
  };
  
  struct Expr;
  
  struct ConstNodeBase;
  struct OpNodeBase;

  struct EvalSpace { virtual ~EvalSpace() {} };
  
  struct ExprNode
  {
    virtual ~ExprNode() {}
    ExprNode() : refs(0) {}
    mutable uintptr_t refs;
    void Retain() const { ++refs; }
    void Release() const { if (--refs == 0) delete this; }
    /* Generic functions */
    virtual unsigned SubCount() const = 0;
    virtual Expr const& GetSub(unsigned idx) const { throw std::logic_error("out of bound sub expression"); }
    virtual void Repr( std::ostream& sink ) const = 0;
    virtual int cmp( ExprNode const& ) const = 0;
    virtual ConstNodeBase const* Eval( EvalSpace const&, ConstNodeBase const** ) const { return 0; }
    virtual ConstNodeBase const* AsConstNode() const { return 0; }
    virtual OpNodeBase const*    AsOpNode() const { return 0; }
    virtual ExprNode* Mutate() const = 0;
    virtual ScalarType::id_t GetType() const = 0;
  };
  
  struct Op : public identifier::Identifier<Op>
  {
    enum Code
      {
        Xor, And, Or,
        Ror, Rol, Lsl, Asr, Lsr,
        Add, Sub, Div, Mod, Mul, Min, Max,
        Teq, Tne, Tge, Tgt, Tle, Tlt, Tgeu, Tgtu, Tleu, Tltu,
        BSwp, BSR, BSF, Not, Neg,
        FCmp, FSQB, FFZ, FNeg, FSqrt, FAbs, FDen, FMod, FPow,
        Cast,
        end
      } code;
    
    char const* c_str() const
    {
      switch (code)
        {
        case   Xor: return "Xor";
        case   And: return "And";
        case    Or: return "Or";
        case   Add: return "Add";
        case   Sub: return "Sub";
        case   Div: return "Div";
        case   Mod: return "Mod";
        case   Mul: return "Mul";
        case   Min: return "Min";
        case   Max: return "Max";
        case   Ror: return "Ror";
        case   Rol: return "Rol";
        case   Lsl: return "Lsl";
        case   Asr: return "Asr";
        case   Lsr: return "Lsr";
        case   Teq: return "Teq";
        case   Tne: return "Tne";
        case   Tge: return "Tge";
        case   Tgt: return "Tgt";
        case   Tle: return "Tle";
        case   Tlt: return "Tlt";
        case  Tgeu: return "Tgeu";
        case  Tgtu: return "Tgtu";
        case  Tleu: return "Tleu";
        case  Tltu: return "Tltu";
        case  FCmp: return "FCmp";
        case  BSwp: return "BSwp";
        case   BSR: return "BSR";
        case   BSF: return "BSF";
        case   Not: return "Not";
        case   Neg: return "Neg";
        case  FSQB: return "FSQB";
        case   FFZ: return "FFZ";
        case  FNeg: return "FNeg";
        case FSqrt: return "FSqrt";
        case  FAbs: return "FAbs";
        case  FDen: return "FDen";
        case  FMod: return "FMod";
        case  FPow: return "FPow";
        case  Cast: return "Cast";
        case   end: break;
        }
      return "NA";
    }
    
    Op() : code(end) {}
    Op( Code _code ) : code(_code) {}
    Op( char const* _code ) : code(end) { init( _code ); }
  };
  
  template <typename VALUE_TYPE>
  struct TypeInfo
  {
    static void Repr( std::ostream& sink, VALUE_TYPE v )
    {
      sink << (std::numeric_limits<VALUE_TYPE>::is_signed ? 'S' : 'U')
           << (8*sizeof(VALUE_TYPE)) << "( 0x"
           << std::hex << uint64_t(v) << " )"<< std::dec;
    };
    enum { BYTECOUNT = sizeof(VALUE_TYPE) };
    static unsigned bitsize() { return 8*sizeof(VALUE_TYPE); }
    static ScalarType::id_t GetType()
    {
      if (std::numeric_limits<VALUE_TYPE>::is_integer)
        {
          bool is_signed = std::numeric_limits<VALUE_TYPE>::is_signed;
          //int bits = std::numeric_limits<VALUE_TYPE>::digits + (is_signed ? 1 : 0);
          int bits = 8*sizeof(VALUE_TYPE);
          return ScalarType::IntegerType( is_signed, bits );
        }
      throw std::logic_error("not an integer type");
    }
  };
  
  template <> struct TypeInfo<bool>
  {
    static void Repr( std::ostream& sink, bool v ) { sink << "BOOL( " << int(v) << " )"; }
    static unsigned bitsize() { return 1; }
    static ScalarType::id_t GetType() { return ScalarType::BOOL; }
  };
  template <> struct TypeInfo<float>
  {
    static void Repr( std::ostream& sink, float v ) { sink << "F32( " << v << " )"; }
    enum { BYTECOUNT = 4 };
    static unsigned bitsize() { return 32; }
    static ScalarType::id_t GetType() { return ScalarType::F32; }
  };
  template <> struct TypeInfo<double>
  {
    static void Repr( std::ostream& sink, double v ) { sink << "F64( " << v << " )"; }
    enum { BYTECOUNT = 8 };
    static unsigned bitsize() { return 64; }
    static ScalarType::id_t GetType() { return ScalarType::F64; }
  };
  
  struct ConstNodeBase : public ExprNode
  {
    virtual unsigned SubCount() const { return 0; };
    virtual ConstNodeBase const* Eval( EvalSpace const&, ConstNodeBase const** ) const override { return this; }
    ConstNodeBase const* AsConstNode() const { return this; };
    virtual ConstNodeBase* apply( Op op, ConstNodeBase const** args ) const = 0;
    virtual float Get( float ) const = 0;
    virtual double Get( double ) const = 0;
    virtual long double Get( long double ) const = 0;
    virtual bool Get( bool ) const = 0;
    virtual uint8_t Get( uint8_t ) const = 0;
    virtual uint16_t Get( uint16_t ) const = 0;
    virtual uint32_t Get( uint32_t ) const = 0;
    virtual uint64_t Get( uint64_t ) const = 0;
    virtual int8_t Get( int8_t ) const = 0;
    virtual int16_t Get( int16_t ) const = 0;
    virtual int32_t Get( int32_t ) const = 0;
    virtual int64_t Get( int64_t ) const = 0;
    static std::ostream& warn();
  };
  
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalMul( VALUE_TYPE l, VALUE_TYPE r ) { return l * r; }
  bool EvalMul( bool, bool );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalMod( VALUE_TYPE l, VALUE_TYPE r ) { return l % r; }
  long double EvalMod( long double l, long double r );
  double     EvalMod( double l, double r );
  float      EvalMod( float l, float r );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalXor( VALUE_TYPE l, VALUE_TYPE r ) { return l ^ r; }
  long double EvalXor( long double l, long double r );
  double     EvalXor( double l, double r );
  float      EvalXor( float l, float r );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalAnd( VALUE_TYPE l, VALUE_TYPE r ) { return l & r; }
  long double EvalAnd( long double l, long double r );
  double     EvalAnd( double l, double r );
  float      EvalAnd( float l, float r );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalOr( VALUE_TYPE l, VALUE_TYPE r ) { return l | r; }
  long double EvalOr( long double l, long double r );
  double     EvalOr( double l, double r );
  float      EvalOr( float l, float r );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalNot( VALUE_TYPE val ) { return ~val; }
  inline bool EvalNot( bool val ) { return not val; }
  long double EvalNot( long double val );
  double     EvalNot( double val );
  float      EvalNot( float val );

  template <typename VALUE_TYPE>
  VALUE_TYPE EvalSHL( VALUE_TYPE l, uint8_t shift ) { return l << shift; }
  bool       EvalSHL( bool, uint8_t );
  long double EvalSHL( long double, uint8_t );
  double     EvalSHL( double, uint8_t );
  float      EvalSHL( float, uint8_t );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalSHR( VALUE_TYPE l, uint8_t shift ) { return l >> shift; }
  long double EvalSHR( long double, uint8_t );
  double     EvalSHR( double, uint8_t );
  float      EvalSHR( float, uint8_t );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalByteSwap( VALUE_TYPE v ) { throw std::logic_error( "No ByteSwap for this type" ); }
  uint32_t   EvalByteSwap( uint32_t v );
  uint16_t   EvalByteSwap( uint16_t v );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalBitScanReverse( VALUE_TYPE v ) { throw std::logic_error( "No BitScanReverse for this type" ); }
  uint32_t   EvalBitScanReverse( uint32_t v );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalBitScanForward( VALUE_TYPE v ) { throw std::logic_error( "No BitScanForward for this type" ); }
  uint32_t   EvalBitScanForward( uint32_t v );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalRotateRight( VALUE_TYPE v, uint8_t shift ) { throw std::logic_error( "No RotateRight for this type" ); }
  uint32_t   EvalRotateRight( uint32_t v, uint8_t shift );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalRotateLeft( VALUE_TYPE v, uint8_t shift ) { throw std::logic_error( "No RotateLeft for this type" ); }
  uint32_t   EvalRotateLeft( uint32_t v, uint8_t shift );
  
  struct Expr
  {
    Expr() : node() {} ExprNode const* node;
    Expr( Expr const& expr ) : node( expr.node ) { if (node) node->Retain(); }
    Expr( ExprNode const* _node ) : node( _node ) { if (node) node->Retain(); }
    ~Expr() { if (node) node->Release(); }
    
    Expr&  operator = ( Expr const& expr )
    {
      if (expr.node) expr.node->Retain();
      ExprNode const* old_node = node;
      node = expr.node;
      if (old_node) old_node->Release();
      return *this;
    }
    
    ExprNode const* operator->() const { return node; }
    ExprNode const* operator->() { return node; }
    ExprNode const& operator* () const { return *node; }
    
    int compare( Expr const& rhs ) const
    {
      // Do not compare null expressions
      if (not rhs.node) return     node ?  1 : 0;
      if (not     node) return rhs.node ? -1 : 0;
      
      /* First compare actual types */
      const std::type_info* til = &typeid(*node);
      const std::type_info* tir = &typeid(*rhs.node);
      if (til < tir) return -1;
      if (til > tir) return +1;
      
      /* Same types, call derived comparator */
      if (int delta = node->cmp( *rhs.node ))
        return delta;

      /* Compare sub operands recursively */
      unsigned subcount = node->SubCount();
      if (int delta = int(subcount) - int(rhs.node->SubCount()))
        return delta;
      for (unsigned idx = 0; idx < subcount; ++idx)
        if (int delta = node->GetSub(idx).compare(rhs.node->GetSub(idx)))
          return delta;

      /* equal to us*/
      return 0;
    }
    
    bool operator != ( Expr const& rhs ) const { return compare( rhs ) != 0; }
    bool operator == ( Expr const& rhs ) const { return compare( rhs ) == 0; }
    bool operator  < ( Expr const& rhs ) const { return compare( rhs )  < 0; }
    bool operator  > ( Expr const& rhs ) const { return compare( rhs )  > 0; }
    
    ConstNodeBase const* Eval( EvalSpace const& evs ) const;
    ConstNodeBase const* ConstSimplify();
    
    bool good() const { return node; }
    friend std::ostream& operator << (std::ostream&, Expr const&);
  };
  
  struct OpNodeBase : public ExprNode
  {
    OpNodeBase( Op _op ) : op(_op) { if (op.code == op.end) throw 0; }
    virtual OpNodeBase const* AsOpNode() const { return this; }
    virtual ScalarType::id_t GetType() const
    {
      switch (op.code)
        {
        case Op::BSwp: case Op::Not: case Op::Neg:  case Op::BSR:   case Op::BSF:
        case Op::FSQB: case Op::FFZ: case Op::FNeg: case Op::FSqrt: case Op::FAbs:
        case Op::FMod: case Op::FPow: 
        case Op::Xor:  case Op::And: case Op::Or:
        case Op::Lsl:  case Op::Lsr: case Op::Asr:  case Op::Ror:   case Op::Rol:
        case Op::Add:  case Op::Sub: case Op::Min:  case Op::Max:
        case Op::Mul:  case Op::Div: case Op::Mod:
       
          return GetSub(0)->GetType();
          
        case Op::FDen:
        case Op::Teq: case Op::Tne:  case Op::Tleu: case Op::Tle:  case Op::Tltu:
        case Op::Tlt: case Op::Tgeu: case Op::Tge:  case Op::Tgtu: case Op::Tgt:
          return ScalarType::BOOL;
            
        case Op::FCmp:
          return ScalarType::S32;
          
        case Op::Cast: /* Should have been handled elsewhere */
        case Op::end:   throw std::logic_error("???");
        }
      return ScalarType::VOID;
    }
    
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<OpNodeBase const&>( rhs ) ); }
    int compare( OpNodeBase const& rhs ) const { return op.cmp( rhs.op ); }
    
    virtual void Repr( std::ostream& sink ) const
    {
      sink << op.c_str() << "( ";
      char const* sep = "";
      for (unsigned idx = 0, end = SubCount(); idx < end; sep = ", ", ++idx)
        sink << sep << GetSub(idx);
      sink << " )";
    }
    
    Op op;
  };
  
  template <typename VALUE_TYPE>
  struct ConstNode : public ConstNodeBase
  {
    typedef ConstNode<VALUE_TYPE> this_type;
    
    ConstNode( VALUE_TYPE _value ) : value( _value ) {} VALUE_TYPE value;
    virtual this_type* Mutate() const { return new this_type( *this ); };
    
    virtual void Repr( std::ostream& sink ) const { TypeInfo<VALUE_TYPE>::Repr( sink, value ); }
    
    static VALUE_TYPE GetValue( ConstNodeBase const* cnb );
  
    ConstNodeBase*
    apply( Op op, ConstNodeBase const** args ) const
    {
      switch (op.code)
        {
        case Op::BSwp:  return new this_type( EvalByteSwap( value ) );
        case Op::Not:   return new this_type( EvalNot( value ) );
        case Op::Neg:   return new this_type( - value );
        case Op::BSR:   return new this_type( EvalBitScanReverse( value ) );
        case Op::BSF:   return new this_type( EvalBitScanForward( value ) );
        case Op::Min:   return new this_type( std::min( value, GetValue( args[1] ) ) );
        case Op::Max:   return new this_type( std::max( value, GetValue( args[1] ) ) );
        case Op::Xor:   return new this_type( EvalXor( value, GetValue( args[1] ) ) );
        case Op::And:   return new this_type( EvalAnd( value, GetValue( args[1] ) ) );
        case Op::Or:    return new this_type( EvalOr( value, GetValue( args[1] ) ) );
        case Op::Lsl:   return new this_type( EvalSHL( value, args[1]->Get( uint8_t() ) ) );
        case Op::Lsr:   
        case Op::Asr:   return new this_type( EvalSHR( value, args[1]->Get( uint8_t() ) ) );
        case Op::Add:   return new this_type( value + GetValue( args[1] ) );
        case Op::Sub:   return new this_type( value - GetValue( args[1] ) );
        case Op::Mul:   return new this_type( EvalMul( value, GetValue( args[1] ) ) );
        case Op::Div:   return new this_type( value / GetValue( args[1] ) );
        case Op::Mod:   return new this_type( EvalMod( value, GetValue( args[1] ) ) );
          
        case Op::Teq:   return new ConstNode   <bool>   ( value == GetValue( args[1] ) );
        case Op::Tne:   return new ConstNode   <bool>   ( value != GetValue( args[1] ) );
        case Op::Tleu:  
        case Op::Tle:   return new ConstNode   <bool>   ( value <= GetValue( args[1] ) );
        case Op::Tltu:  
        case Op::Tlt:   return new ConstNode   <bool>   ( value <  GetValue( args[1] ) );
        case Op::Tgeu:  
        case Op::Tge:   return new ConstNode   <bool>   ( value >= GetValue( args[1] ) );
        case Op::Tgtu:  
        case Op::Tgt:   return new ConstNode   <bool>   ( value >  GetValue( args[1] ) );
        case Op::Ror:   return new this_type( EvalRotateRight( value, args[1]->Get( uint8_t() ) ) );
        case Op::Rol:   return new this_type( EvalRotateLeft( value, args[1]->Get( uint8_t() ) ) );
          
        case Op::FSQB:  break;
        case Op::FFZ:   break;
        case Op::FNeg:  break;
        case Op::FSqrt: break;
        case Op::FAbs:  break;
        case Op::FDen:  break;
        case Op::FCmp:  break;
        case Op::FMod:  break;
        case Op::FPow:  break;
          
        case Op::Cast: /* Should have been handled elsewhere */
        case Op::end:   throw std::logic_error("???");
        }
      
      warn() << "Unhandled unary operation: " << op.c_str() << "\n";
      return 0;
    }
    float Get( float ) const { return value; }
    double Get( double ) const { return value; }
    long double Get( long double ) const { return value; }
    bool Get( bool ) const { return value; }
    uint8_t Get( uint8_t ) const { return value; }
    uint16_t Get( uint16_t ) const { return value; }
    uint32_t Get( uint32_t ) const { return value; }
    uint64_t Get( uint64_t ) const { return value; }
    int8_t Get( int8_t ) const { return value; }
    int16_t Get( int16_t ) const { return value; }
    int32_t Get( int32_t ) const { return value; }
    int64_t Get( int64_t ) const { return value; }
    ScalarType::id_t GetType() const { return TypeInfo<VALUE_TYPE>::GetType(); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<this_type const&>( rhs ) ); }
    int compare( this_type const& rhs ) const { return (value < rhs.value) ? -1 : (value > rhs.value) ? +1 : 0; }
  };

  template <typename VALUE_TYPE>
  VALUE_TYPE ConstNode<VALUE_TYPE>::GetValue( ConstNodeBase const* cnb ) { return dynamic_cast<ConstNode<VALUE_TYPE> const&>( *cnb ).value; }

  template <typename VALUE_TYPE>
  Expr make_const( VALUE_TYPE value ) { return Expr( new ConstNode<VALUE_TYPE>( value ) ); }
  
  template <unsigned SUBCOUNT>
  struct OpNode : public OpNodeBase
  {
    typedef OpNode<SUBCOUNT> this_type;
    
    OpNode( Op _op ) : OpNodeBase(_op) {}
    virtual this_type* Mutate() const { return new this_type( *this ); }

    virtual ConstNodeBase const* Eval( EvalSpace const&, ConstNodeBase const** cnbs ) const override { return cnbs[0]->apply( op, &cnbs[0] ); }
    
    virtual unsigned SubCount() const { return SUBCOUNT; };
    virtual Expr const& GetSub(unsigned idx) const
    {
      if (idx >= SUBCOUNT)
        return ExprNode::GetSub(idx);
      return subs[idx];
    };
    
    Expr subs[SUBCOUNT];
  };
  
  struct CastNodeBase : public OpNodeBase
  {
    CastNodeBase( Expr const& _src ) : OpNodeBase( Op::Cast ), src(_src) {}
    virtual ScalarType::id_t GetSrcType() const = 0;
    virtual unsigned SubCount() const { return 1; };
    virtual Expr const& GetSub(unsigned idx) const { if (idx!= 0) return ExprNode::GetSub(idx); return src; }
    virtual void Repr( std::ostream& sink ) const { sink << ScalarType( GetType() ).name; sink << "( " << src << " )"; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<CastNodeBase const&>( rhs ) ); }
    int compare( CastNodeBase const& rhs ) const { return 0; }
    
    Expr src;
  };
  
  template <typename DST_VALUE_TYPE, typename SRC_VALUE_TYPE>
  struct CastNode : public CastNodeBase
  {
    typedef CastNode<DST_VALUE_TYPE,SRC_VALUE_TYPE> this_type;
    CastNode( Expr const& _src ) : CastNodeBase( _src ) {}
    virtual this_type* Mutate() const { return new this_type( *this ); }
    virtual ScalarType::id_t GetSrcType() const { return TypeInfo<SRC_VALUE_TYPE>::GetType(); }
    virtual ScalarType::id_t GetType() const { return TypeInfo<DST_VALUE_TYPE>::GetType(); }
    virtual ConstNodeBase const* Eval( EvalSpace const&, ConstNodeBase const** cnbs ) const override { return new ConstNode<DST_VALUE_TYPE>( cnbs[0]->Get( DST_VALUE_TYPE() ) ); }
  };
  
  /* 1 operand operation */
  inline Expr make_operation( Op op, Expr const& operand )
  {
    OpNode<1>* res = new OpNode<1>( op );
    res->subs[0] = operand;
    return res;
  }
  
  /* 2 operands operation */
  inline Expr make_operation( Op op, Expr const& left, Expr const& right )
  {
    OpNode<2>* res = new OpNode<2>( op );
    res->subs[0] = left; res->subs[1] = right;
    return res;
  }
  
  template <typename VALUE_TYPE>
  struct SmartValue
  {
    typedef VALUE_TYPE value_type;
    typedef SmartValue<value_type> this_type;
    
    Expr expr;
    
    SmartValue() : expr(0) {}
    
    SmartValue( Expr const& _expr )
      : expr( _expr )
    {
      expr.ConstSimplify();
    }
    
    explicit SmartValue( value_type value ) : expr( make_const( value ) ) {}

    template <typename SRC_VALUE_TYPE>
    explicit SmartValue( SmartValue<SRC_VALUE_TYPE> const& other )
    {
      if (CmpTypes<SRC_VALUE_TYPE,VALUE_TYPE>::same) {
        expr = other.expr;
      } else {
        expr = new CastNode<VALUE_TYPE,SRC_VALUE_TYPE>( other.expr );
        expr.ConstSimplify();
      }
    }

    static bool const is_signed = std::numeric_limits<value_type>::is_signed;
    
    this_type& operator = ( this_type const& other ) { expr = other.expr; return *this; }
    
    template <typename SHT>
    this_type operator << ( SHT sh ) const { return this_type( make_operation( "Lsl", expr, make_const<uint8_t>(sh) ) ); }
    template <typename SHT>
    this_type operator >> ( SHT sh ) const { return this_type( make_operation( is_signed ? "Asr" : "Lsr", expr, make_const<uint8_t>(sh) ) ); }
    template <typename SHT>
    this_type& operator <<= ( SHT sh ) { expr = make_operation( "Lsl", expr, make_const<uint8_t>(sh) ); return *this; }
    template <typename SHT>
    this_type& operator >>= ( SHT sh ) { expr = make_operation( is_signed?"Asr":"Lsr", expr, make_const<uint8_t>(sh) ); return *this; }
    
    template <typename SHT>
    this_type operator << ( SmartValue<SHT> const& sh ) const { return this_type( make_operation( "Lsl", expr, SmartValue<uint8_t>(sh).expr ) ); }
    template <typename SHT>
    this_type operator >> ( SmartValue<SHT> const& sh ) const {return this_type( make_operation( is_signed?"Asr":"Lsr", expr, SmartValue<uint8_t>(sh).expr ) ); }
    
    this_type operator - () const { return this_type( make_operation( "Neg", expr ) ); }
    this_type operator ~ () const { return this_type( make_operation( "Not", expr ) ); }
    
    this_type& operator += ( this_type const& other ) { expr = make_operation( "Add", expr, other.expr ); return *this; }
    this_type& operator -= ( this_type const& other ) { expr = make_operation( "Sub", expr, other.expr ); return *this; }
    this_type& operator *= ( this_type const& other ) { expr = make_operation( "Mul", expr, other.expr ); return *this; }
    this_type& operator /= ( this_type const& other ) { expr = make_operation( "Div", expr, other.expr ); return *this; }
    this_type& operator %= ( this_type const& other ) { expr = make_operation( "Mod", expr, other.expr ); return *this; }
    this_type& operator ^= ( this_type const& other ) { expr = make_operation( "Xor", expr, other.expr ); return *this; }
    this_type& operator &= ( this_type const& other ) { expr = make_operation( "And", expr, other.expr ); return *this; }
    this_type& operator |= ( this_type const& other ) { expr =  make_operation( "Or", expr, other.expr ); return *this; }
    
    this_type operator + ( this_type const& other ) const { return this_type( make_operation( "Add", expr, other.expr ) ); }
    this_type operator - ( this_type const& other ) const { return this_type( make_operation( "Sub", expr, other.expr ) ); }
    this_type operator * ( this_type const& other ) const { return this_type( make_operation( "Mul", expr, other.expr ) ); }
    this_type operator / ( this_type const& other ) const { return this_type( make_operation( "Div", expr, other.expr ) ); }
    this_type operator % ( this_type const& other ) const { return this_type( make_operation( "Mod", expr, other.expr ) ); }
    this_type operator ^ ( this_type const& other ) const { return this_type( make_operation( "Xor", expr, other.expr ) ); }
    this_type operator & ( this_type const& other ) const { return this_type( make_operation( "And", expr, other.expr ) ); }
    this_type operator | ( this_type const& other ) const { return this_type( Expr(  make_operation( "Or", expr, other.expr ) ) ); }
    
    SmartValue<bool> operator == ( this_type const& other ) const { return SmartValue<bool>( make_operation( "Teq", expr, other.expr ) ); }
    SmartValue<bool> operator != ( this_type const& other ) const { return SmartValue<bool>( make_operation( "Tne", expr, other.expr ) ); }
    SmartValue<bool> operator <= ( this_type const& other ) const { return SmartValue<bool>( make_operation( is_signed ? "Tle" : "Tleu", expr, other.expr ) ); }
    SmartValue<bool> operator >= ( this_type const& other ) const { return SmartValue<bool>( make_operation( is_signed ? "Tge" : "Tgeu", expr, other.expr ) ); }
    SmartValue<bool> operator < ( this_type const& other ) const  { return SmartValue<bool>( make_operation( is_signed ? "Tlt" : "Tltu", expr, other.expr ) ); }
    SmartValue<bool> operator > ( this_type const& other ) const  { return SmartValue<bool>( make_operation( is_signed ? "Tgt" : "Tgtu", expr, other.expr ) ); }
    
    SmartValue<bool> operator ! () const
    { AssertBool<value_type>::check(); return SmartValue<bool>( make_operation( "Not", expr ) ); }

    SmartValue<bool> operator && ( SmartValue<bool> const& other ) const
    { AssertBool<value_type>::check(); return SmartValue<bool>( make_operation( "And", expr, other.expr ) ); }
    
    SmartValue<bool> operator || ( SmartValue<bool> const& other ) const
    { AssertBool<value_type>::check(); return SmartValue<bool>( Expr(  make_operation( "Or", expr, other.expr ) ) ); }
  };

  template <typename T> SmartValue<T> Minimum( SmartValue<T> const& l, SmartValue<T> const& r ) { return SmartValue<T>( make_operation( "Min", l.expr, r.expr ) ); }
  template <typename T> SmartValue<T> Maximum( SmartValue<T> const& l, SmartValue<T> const& r ) { return SmartValue<T>( make_operation( "Max", l.expr, r.expr ) ); }
  
  template <typename UTP>
  UTP ByteSwap( UTP const& value ) { return UTP( make_operation( "BSwp", value.expr ) ); }
  
  template <typename UTP>
  UTP RotateRight( UTP const& value, uint8_t sh ) { return UTP( make_operation( "Ror", value.expr, make_const<uint8_t>(sh) ) ); }
  template <typename UTP, typename STP>
  UTP RotateRight( UTP const& value, STP const& sh ) { return UTP( make_operation( "Ror", value.expr, SmartValue<uint8_t>(sh).expr ) ); }
  
  template <typename UTP>
  UTP RotateLeft( UTP const& value, uint8_t sh ) { return UTP( make_operation( "Rol", value.expr, make_const<uint8_t>(sh) ) ); }
  template <typename UTP, typename STP>
  UTP RotateLeft( UTP const& value, STP const& sh ) { return UTP( make_operation( "Rol", value.expr, SmartValue<uint8_t>(sh).expr ) ); }
  
  template <typename UTP>
  UTP BitScanReverse( UTP const& value ) { return UTP( make_operation( "BSR", value.expr ) ); }
  
  template <typename UTP>
  UTP BitScanForward( UTP const& value ) { return UTP( make_operation( "BSF", value.expr ) ); }
  
  template <typename T>
  SmartValue<T> power( SmartValue<T> const& left, SmartValue<T> const& right ) { return SmartValue<T>( make_operation( "FPow", left.expr, right.expr ) ); }
  
  template <typename T>
  SmartValue<T> fmodulo( SmartValue<T> const& left, SmartValue<T> const& right ) { return SmartValue<T>( make_operation( "FMod", left.expr, right.expr ) ); }
  
  struct FP
  {
    struct DefaultNaNNode : public ExprNode
    {
      DefaultNaNNode( int _fsz ) : fsz( _fsz ) {} int fsz;
      virtual DefaultNaNNode* Mutate() const { return new DefaultNaNNode( *this ); }
      virtual void Repr( std::ostream& sink ) const { sink << "DefaultNaN()"; }
      virtual unsigned SubCount() const { return 0; };
      virtual ScalarType::id_t GetType() const { return fsz==32 ? TypeInfo<float>::GetType() : TypeInfo<double>::GetType(); }
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<DefaultNaNNode const&>( rhs ) ); }
      int compare( DefaultNaNNode const& rhs ) const { return fsz - rhs.fsz; }
    };
    
    static inline
    void SetDefaultNan( SmartValue<float>& result )
    { result = SmartValue<float>( Expr( new DefaultNaNNode( 32 ) ) ); }
    
    static inline
    void SetDefaultNan( SmartValue<double>& result )
    { result = SmartValue<double>( Expr( new DefaultNaNNode( 64 ) ) ); }
    
    template <typename FLOAT> static
    void SetQuietBit( FLOAT& op )
    {
      op = FLOAT( make_operation( "FSQB", op.expr ) );
    }

    template <typename FLOAT> static
    SmartValue<bool>
    FlushToZero( FLOAT& op, SmartValue<uint32_t> const& fpscr_val )
    {
      op = FLOAT( make_operation( "FFZ", op.expr ) );
      return SmartValue<bool>( make_operation( "FDen", op.expr ) );
    }

    template <typename FLOAT> static
    SmartValue<int32_t> Compare( FLOAT op1, FLOAT op2, SmartValue<uint32_t> const& fpscr_val )
    {
      return SmartValue<int32_t>( make_operation( "FCmp", op1.expr, op2.expr ) );
    }

    struct IsNaNNode : public ExprNode
    {
      IsNaNNode( Expr const& _src, bool _signaling ) : src(_src), signaling(_signaling) {} Expr src; bool signaling;
      virtual IsNaNNode* Mutate() const { return new IsNaNNode( *this ); }
      virtual void Repr( std::ostream& sink ) const { sink << "IsNaN(" << src << ")"; }
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<IsNaNNode const&>( rhs ) ); }
      int compare( IsNaNNode const& rhs ) const { return int(signaling) - int(rhs.signaling); }
      virtual unsigned SubCount() const { return 1; };
      virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return src; };
      virtual ScalarType::id_t GetType() const { return ScalarType::BOOL; }
    };
    
    template <typename FLOAT> static
    SmartValue<bool>
    IsSNaN( FLOAT const& op )
    {
      return SmartValue<bool>( Expr( new IsNaNNode( op.expr, true ) ) );
    }
    
    template <typename FLOAT> static
    SmartValue<bool>
    IsQNaN( FLOAT const& op )
    {
      return SmartValue<bool>( Expr( new IsNaNNode( op.expr, false ) ) );
    }
    
    template <typename FLOAT, class ARCH> static
    void Add( FLOAT& acc, FLOAT const& op2, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      acc = FLOAT( make_operation( "Add", acc.expr, op2.expr ) );
    }

    template <typename FLOAT, class ARCH> static
    void Sub( FLOAT& acc, FLOAT const& op2, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      acc = FLOAT( make_operation( "Sub", acc.expr, op2.expr ) );
    }

    template <typename FLOAT, class ARCH> static
    void Div( FLOAT& acc, FLOAT const& op2, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      acc = FLOAT( make_operation( "Div", acc.expr, op2.expr ) );
    }

    template <typename FLOAT, class ARCH> static
    void Mul( FLOAT& acc, FLOAT const& op2, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      acc = FLOAT( make_operation( "Mul", acc.expr, op2.expr ) );
    }

    struct MulAddNode : public ExprNode
    {
      MulAddNode( Expr const& _acc, Expr const& _left, Expr const& _right )
        : acc( _acc ), left( _left ), right( _right )
      {} Expr acc, left, right;
      virtual MulAddNode* Mutate() const { return new MulAddNode( *this ); }
      
      virtual unsigned SubCount() const { return 3; };
      virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return acc; case 1: return left; case 2: return right; } return ExprNode::GetSub(idx); };
      
      virtual void Repr( std::ostream& sink ) const { sink << "MulAdd( " << acc << ", " << left << ", " << right << " )"; }
      virtual ScalarType::id_t GetType() const { return GetSub(0)->GetType(); }
      
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<MulAddNode const&>( rhs ) ); }
      int compare( MulAddNode const& rhs ) const { return 0; }
    };
    
    template <typename FLOAT, class ARCH> static
    void MulAdd( FLOAT& acc, FLOAT const& op1, FLOAT const& op2, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      acc = FLOAT( Expr( new MulAddNode( acc.expr, op1.expr, op2.expr ) ) );
    }
    
    struct IsInvalidMulAddNode : public ExprNode
    {
      IsInvalidMulAddNode( Expr const& _acc, Expr const& _left, Expr const& _right )
        : acc( _acc ), left( _left ), right( _right )
      {} Expr acc, left, right;
      virtual IsInvalidMulAddNode* Mutate() const { return new IsInvalidMulAddNode( *this ); }
      virtual ScalarType::id_t GetType() const { return ScalarType::BOOL; }
      
      virtual unsigned SubCount() const { return 3; };
      virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return acc; case 1: return left; case 2: return right; } return ExprNode::GetSub(idx); };
      
      virtual void Repr( std::ostream& sink ) const { sink << "IsInvalidMulAdd( " << acc << ", " << left << ", " << right << " )"; }
      
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<IsInvalidMulAddNode const&>( rhs ) ); }
      int compare( IsInvalidMulAddNode const& rhs ) const { return 0; }
    };
    
    template <typename SOFTDBL> static
    SmartValue<bool>
    IsInvalidMulAdd( SOFTDBL const& acc, SOFTDBL const& op1, SOFTDBL const& op2, SmartValue<uint32_t> const& fpscr_val )
    {
      return SmartValue<bool>( Expr( new IsInvalidMulAddNode( acc.expr, op1.expr, op2.expr ) ) );
    }
    
    template <typename FLOAT> static
    void Neg( FLOAT& acc ) { acc = FLOAT( make_operation( "FNeg", acc.expr ) ); }

    template <typename FLOAT> static
    void Abs( FLOAT& acc ) { acc = FLOAT( make_operation( "FAbs", acc.expr ) ); }
    
    template <typename FLOAT, class ARCH> static
    void Sqrt( FLOAT& acc, ARCH& arch, SmartValue<uint32_t> const& fpscr_val ) { acc = FLOAT( make_operation( "FSqrt", acc.expr ) ); }

    struct FtoFNode : public ExprNode
    {
      FtoFNode( Expr const& _src, int _ssz, int _dsz )
        : src( _src ), ssz( _ssz ), dsz( _dsz )
      {} Expr src; int ssz; int dsz;
      virtual FtoFNode* Mutate() const { return new FtoFNode( *this ); }
      
      virtual void Repr( std::ostream& sink ) const { sink << "FtoF( " << src << " )"; }
      virtual unsigned SubCount() const { return 1; }
      virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return src; }
      virtual ScalarType::id_t GetType() const
      {
        switch (dsz) { case 32: return ScalarType::F32; case 64: return ScalarType::F64; }
        throw 0; return ScalarType::F32;
      }
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<FtoFNode const&>( rhs ) ); }
      int compare( FtoFNode const& rhs ) const
      {
        if (int delta = ssz - rhs.ssz) return delta;
        return dsz - rhs.dsz;
      }
    };
    
    template <typename ofpT, typename ifpT, class ARCH> static
    void FtoF( SmartValue<ofpT>& dst, SmartValue<ifpT> const& src, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      dst = SmartValue<ofpT>( Expr( new FtoFNode( src.expr, TypeInfo<ifpT>::bitsize(), TypeInfo<ofpT>::bitsize() ) ) );
    }

    template <typename intT, typename fpT>
    struct FtoINode : public ExprNode
    {
      FtoINode( Expr const& _src, int _fb ) : src( _src ), fb( _fb ) {} Expr src; int fb;
      virtual FtoINode* Mutate() const { return new FtoINode( *this ); }
      virtual ScalarType::id_t GetType() const { return TypeInfo<intT>::GetType(); }
      
      virtual void Repr( std::ostream& sink ) const { sink << "FtoI( " << src << " )"; }
      virtual unsigned SubCount() const { return 1; }
      virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return src; }
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<FtoINode const&>( rhs ) ); }
      int compare( FtoINode const& rhs ) const { return fb - rhs.fb; }
    };

    template <typename intT, typename fpT, class ARCH> static
    void FtoI( SmartValue<intT>& dst, SmartValue<fpT> const& src, int fracbits, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      dst = SmartValue<intT>( Expr( new FtoINode<intT,fpT>( src.expr, fracbits) ) );
    }

    template <typename fpT, typename intT>
    struct ItoFNode : public ExprNode
    {
      ItoFNode( Expr const& _src, int _fb )
        : src( _src ), fb( _fb )
      {} Expr src; int fb;
      virtual ItoFNode* Mutate() const override { return new ItoFNode( *this ); }
      virtual ScalarType::id_t GetType() const override { return TypeInfo<fpT>::GetType(); }
      virtual void Repr( std::ostream& sink ) const override { sink << "ItoF( " << src << " )"; }
      virtual unsigned SubCount() const override { return 1; }
      virtual Expr const& GetSub(unsigned idx) const override { if (idx != 0) return ExprNode::GetSub(idx); return src; }
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<ItoFNode const&>( rhs ) ); }
      int compare( ItoFNode const& rhs ) const { return fb - rhs.fb; }
    };
    
    template <typename fpT, typename intT, class ARCH> static
    void ItoF( SmartValue<fpT>& dst, SmartValue<intT> const& src, int fracbits, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      dst = SmartValue<fpT>( Expr( new ItoFNode<fpT,intT>( src.expr, fracbits ) ) );
    }
    
  };

  template <class T>
  struct Choice
  {
    Choice() : nexts(), previous() {}
    ~Choice() { delete nexts[0]; delete nexts[1]; }

    T*      next(bool choice) { return nexts[choice]; }
    T*   getnext(bool choice) { if (not nexts[choice]) setnext(choice, new T); return nexts[choice]; }
    void setnext(bool choice, T* nxt) { nexts[choice] = nxt; nxt->previous = static_cast<T*>(this); }
    
    bool proceed()
    {
      if (getnext(false)->previous) return false;
      if (getnext(true)->previous)  return true;
      throw *this;                  return 0;
    }
    
    bool close()
    {
      if (T* p = previous)
        {
          previous = 0;
          if (this == p->nexts[1])
            return p->close();
          return false;
        }
      return true;
    }
  
    T*    nexts[2];
    T*    previous;
  };

  template <class T>
  struct Conditional : public Choice<T>
  {
    Conditional() : Choice<T>(), cond() {}

    bool  proceed( Expr const& _cond );
  
    Expr  cond;
  };
  
  template <class T>
  bool
  Conditional<T>::proceed( Expr const& _cond )
  {
    if (not cond.good()) cond = _cond;
    else if (cond != _cond) throw cond;

    return Choice<T>::proceed();
  }
  
  template <class PoolT, typename Merger>
  void
  factorize( PoolT& dst, PoolT& lho, PoolT& rho, Merger merger )
  {
    for (typename PoolT::iterator lhi = lho.begin(), rhi = rho.begin(), lie = lho.end(), rie = rho.end(); lhi != lie and rhi != rie; )
      {
        if (lho.value_comp()(*lhi, *rhi))
          ++lhi;
        else if (lho.value_comp()(*rhi, *lhi))
          ++rhi;
        else
          {
            merger( dst, *lhi, *rhi );
            lho.erase( lhi++ );
            rho.erase( rhi++ );
          }
      }
  }

  template <class T>
  struct ExprScanner
  {
    void
    Flood( Expr const& e )
    {
      for (unsigned idx = 0, end = e->SubCount(); idx < end; ++idx)
        static_cast<T*>(this)->Process( e->GetSub(idx) );
    }
  };

} /* end of namespace symbolic */
} /* end of namespace util */
} /* end of namespace unisim */

#endif /* __UNISIM_UTIL_SYMBOLIC_SYMBOLIC_HH__ */
