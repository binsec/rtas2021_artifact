/*
 *  Copyright (c) 2019-2020,
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

#ifndef ARMD64DBA_ARCH_HH
#define ARMD64DBA_ARCH_HH

#include <unisim/component/cxx/processor/intel/isa/intel.hh>
#include <unisim/component/cxx/processor/intel/modrm.hh>
#include <unisim/component/cxx/processor/intel/disasm.hh>
#include <unisim/component/cxx/processor/intel/types.hh>
#include <unisim/component/cxx/processor/intel/vectorbank.hh>
#include <unisim/util/symbolic/binsec/binsec.hh>
#include <unisim/util/symbolic/symbolic.hh>

// template <typename ARCH, unsigned OPSIZE> struct TypeFor {};

// template <typename ARCH> struct TypeFor<ARCH, 8> { typedef typename ARCH:: S8 S; typedef typename ARCH:: U8 U; };
// template <typename ARCH> struct TypeFor<ARCH,16> { typedef typename ARCH::S16 S; typedef typename ARCH::U16 U; };
// template <typename ARCH> struct TypeFor<ARCH,32> { typedef typename ARCH::S32 S; typedef typename ARCH::U32 U; typedef typename ARCH::F32 F; };
// template <typename ARCH> struct TypeFor<ARCH,64> { typedef typename ARCH::S64 S; typedef typename ARCH::U64 U; typedef typename ARCH::F64 F; };

template <typename T>
struct SmartValueTraits
{
  typedef typename unisim::util::symbolic::TypeInfo<typename T::value_type> traits;
  enum { bytecount = traits::BYTECOUNT };
  static unisim::util::symbolic::ScalarType::id_t GetType() { return traits::GetType(); }
};

struct VmmRegister
{
  VmmRegister() = default;
  VmmRegister(unisim::util::symbolic::Expr const& _expr) : expr(_expr) {}
  unisim::util::symbolic::Expr expr;
};
    
template <>
struct SmartValueTraits<VmmRegister>
{
  enum { bytecount = 32 };
  static unisim::util::symbolic::ScalarType::id_t GetType() { return unisim::util::symbolic::ScalarType::VOID; }
};

struct Processor
{
  //   =====================================================================
  //   =                           Configuration                           =
  //   =====================================================================
  struct Unimplemented {};
  struct Undefined {};
    
  //   =====================================================================
  //   =                             Data Types                            =
  //   =====================================================================
  template <typename T> using SmartValue = unisim::util::symbolic::SmartValue<T>;
  template <typename T> using VectorTypeInfo = unisim::component::cxx::processor::intel::VectorTypeInfo<T>;
  template <typename A, unsigned S> using TypeFor = typename unisim::component::cxx::processor::intel::TypeFor<A,S>;
  typedef unisim::component::cxx::processor::intel::Operation<Processor> Operation;
  
  typedef SmartValue<uint8_t>     u8_t;
  typedef SmartValue<uint16_t>    u16_t;
  typedef SmartValue<uint32_t>    u32_t;
  typedef SmartValue<uint64_t>    u64_t;
  typedef SmartValue<int8_t>      s8_t;
  typedef SmartValue<int16_t>     s16_t;
  typedef SmartValue<int32_t>     s32_t;
  typedef SmartValue<int64_t>     s64_t;
  typedef SmartValue<bool>        bit_t;

  typedef u64_t addr_t;
  
  typedef SmartValue<float>       f32_t;
  typedef SmartValue<double>      f64_t;
  typedef SmartValue<long double> f80_t;

  typedef unisim::component::cxx::processor::intel::GObLH GObLH;
  typedef unisim::component::cxx::processor::intel::GOb GOb;
  typedef unisim::component::cxx::processor::intel::GOw GOw;
  typedef unisim::component::cxx::processor::intel::GOd GOd;
  typedef unisim::component::cxx::processor::intel::GOq GOq;

  typedef unisim::component::cxx::processor::intel::SSE SSE;
  typedef unisim::component::cxx::processor::intel::XMM XMM;
  typedef unisim::component::cxx::processor::intel::YMM YMM;
    
  typedef unisim::component::cxx::processor::intel::RMOp<Processor> RMOp;
    
  typedef unisim::util::symbolic::Expr Expr;
  typedef unisim::util::symbolic::ExprNode ExprNode;
  typedef unisim::util::symbolic::ScalarType ScalarType;
    
  typedef GOq   GR;
  typedef u64_t gr_type;
  
  typedef unisim::util::symbolic::binsec::ActionNode   ActionNode;
  typedef unisim::util::symbolic::binsec::Branch       Br;

  struct OpHeader { OpHeader( uint64_t _address ) : address( _address ) {} uint64_t address; };

  template <typename RID>
  struct RegRead : public unisim::util::symbolic::binsec::RegRead
  {
    typedef unisim::util::symbolic::binsec::RegRead Super;
    RegRead( RID _id ) : Super(), id(_id) {}
    virtual RegRead* Mutate() const { return new RegRead( *this ); }
    virtual ScalarType::id_t GetType() const
    { return unisim::util::symbolic::TypeInfo<typename RID::register_type>::GetType(); }
    virtual void GetRegName( std::ostream& sink ) const { sink << id.c_str(); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegRead const&>( rhs ) ); }
    int compare( RegRead const& rhs ) const { if (int delta = Super::compare(rhs)) return delta; return id.cmp( rhs.id ); }

    RID id;
  };

  template <typename RID>
  static Expr newRegRead( RID id ) { return new RegRead<RID>( id ); }

  template <typename RID>
  struct RegWrite : public unisim::util::symbolic::binsec::RegWrite
  {
    typedef RegWrite<RID> this_type;
    typedef unisim::util::symbolic::binsec::RegWrite Super;
    RegWrite( RID _id, Expr const& _value ) : Super(_value), id(_id) {}
    virtual this_type* Mutate() const { return new this_type( *this ); }
    virtual void GetRegName( std::ostream& sink ) const { sink << id.c_str(); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegWrite const&>( rhs ) ); }
    int compare( RegWrite const& rhs ) const { if (int delta = id.cmp( rhs.id )) return delta; return Super::compare( rhs ); }
    
    RID id;
  };

  template <typename RID>
  static Expr newRegWrite( RID id, Expr const& value ) { return new RegWrite<RID>( id, value ); }
  
  struct Goto : public Br
  {
    Goto( Expr const& value ) : Br( value ) {}
    virtual Goto* Mutate() const override { return new Goto( *this ); }
    virtual void GetRegName( std::ostream& sink ) const override { sink << "pc"; }
    virtual void annotate(std::ostream& sink) const override { return; }
  };

  struct Call : public Goto
  {
    Call( Expr const& value, uint64_t ra ) : Goto( value ), return_address( ra ) {}
    virtual Call* Mutate() const override { return new Call( *this ); }
    virtual void annotate(std::ostream& sink) const override { sink << " // call (" << unisim::util::symbolic::binsec::dbx(8,return_address) << ",0)"; }

    uint64_t return_address;
  };

  
  u64_t                       tscread() { throw Unimplemented(); return u64_t( 0 ); }
    
  struct FLAG : public unisim::util::identifier::Identifier<FLAG>
  {
    typedef bool register_type;
    enum Code { CF = 0, PF, AF, ZF, SF, DF, OF, C0, C1, C2, C3, end } code;
      
    char const* c_str() const
    {
      switch (code)
        {
        case CF: return "cf";
        case PF: return "pf";
        case AF: return "af";
        case ZF: return "zf";
        case SF: return "sf";
        case DF: return "df";
        case OF: return "of";
        case C0: return "c0";
        case C1: return "c1";
        case C2: return "c2";
        case C3: return "c3";
        case end: break;
        }
      return "NA";
    }

    FLAG() : code(end) {}
    FLAG( Code _code ) : code(_code) {}
    FLAG( char const* _code ) : code(end) { init(_code); }
  };

  Expr                        flagvalues[FLAG::end];
    
  bit_t                       flagread( FLAG flag ) { return bit_t(flagvalues[flag.idx()]); }
  void                        flagwrite( FLAG flag, bit_t fval ) { flagvalues[flag.idx()] = fval.expr; }

  struct RIRegID : public unisim::util::identifier::Identifier<RIRegID>
  {
    typedef uint64_t register_type;
    enum Code { rax = 0, rcx = 1, rdx = 2, rbx = 3, rsp = 4, rbp = 5, rsi = 6, rdi = 7,
                r8, r9, r10, r11, r12, r13, r14, r15,
                end } code;

    char const* c_str() const
    {
      switch (code)
        {
        case rax: return "rax";
        case rcx: return "rcx";
        case rdx: return "rdx";
        case rbx: return "rbx";
        case rsp: return "rsp";
        case rbp: return "rbp";
        case rsi: return "rsi";
        case rdi: return "rdi";
        case r8:  return "r8";
        case r9: return "r9";
        case r10: return "r10";
        case r11: return "r11";
        case r12: return "r12";
        case r13: return "r13";
        case r14: return "r14";
        case r15: return "r15";
        
        case end: break;
        }
      return "NA";
    }
    
    RIRegID() : code(end) {}
    RIRegID( Code _code ) : code(_code) {}
    RIRegID( char const* _code ) : code(end) { init( _code ); }
  };

  struct SRegID : public unisim::util::identifier::Identifier<SRegID>
  {
    typedef uint16_t register_type;
    enum Code { es, cs, ss, ds, fs, gs, end } code;
      
    char const* c_str() const
    {
      switch (code)
        {
        case  es: return "es";
        case  cs: return "cs";
        case  ss: return "ss";
        case  ds: return "ds";
        case  fs: return "fs";
        case  gs: return "gs";
        case end: break;
        }
      return "NA";
    }
      
    SRegID() : code(end) {}
    SRegID( Code _code ) : code(_code) {}
    SRegID( char const* _code ) : code(end) { init( _code ); }
  };

  struct FRegID : public unisim::util::identifier::Identifier<FRegID>
  {
    typedef double register_type;
    enum Code { st0=0, st1, st2, st3, st4, st5, st6, st7, end } code;
    char const* c_str() const { return &"st0\0st1\0st2\0st3\0st4\0st5\0st6\0st7"[(unsigned(code) % 8)*4]; }
    FRegID() : code(end) {}
    FRegID( Code _code ) : code(_code) {}
    FRegID( char const* _code ) : code(end) { init( _code ); }
  };
    
  u16_t                       segregread( unsigned idx ) { throw Unimplemented(); return u16_t(); }
  void                        segregwrite( unsigned idx, u16_t value ) { throw Unimplemented(); }

  typedef std::map<unsigned,unsigned> RegMap;

  enum {VREGCOUNT = 16, GREGCOUNT = 16, FREGCOUNT=8};
  
  static unsigned const GREGSIZE = 8;

  Expr                        eregread( unsigned reg, unsigned size, unsigned pos );
  void                        eregwrite( unsigned reg, unsigned size, unsigned pos, Expr const& xpr );

  Expr                        regvalues[GREGCOUNT][GREGSIZE];

  void                        eregsinks( RIRegID reg );
 

  // VRegRead should never be a binsec::node (no dba available for it)
  struct VRegRead : public unisim::util::symbolic::ExprNode
  {
    VRegRead( unsigned _reg ) : reg(_reg) {}
    virtual VRegRead* Mutate() const { return new VRegRead( *this ); }
    virtual ScalarType::id_t GetType() const { return ScalarType::VOID; }
    virtual unsigned SubCount() const override { return 0; }
    virtual void Repr( std::ostream& sink ) const override { sink << "VRegRead(" << std::dec << reg << ")"; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<VRegRead const&>( rhs ) ); }
    int compare( VRegRead const& rhs ) const { return int(reg) - int(rhs.reg); }
    unsigned reg;
  };
  
  template <class GOP>
  typename TypeFor<Processor,GOP::SIZE>::u regread( GOP const&, unsigned idx )
  {
    return typename TypeFor<Processor,GOP::SIZE>::u( eregread( idx, GOP::SIZE / 8, 0 ) );
  }

  u8_t regread( GObLH const&, unsigned idx ) { return u8_t( eregread( idx%4, 1, (idx >> 2) & 1 ) ); }

  template <class GOP> void regwrite( GOP const&, unsigned idx, typename TypeFor<Processor,GOP::SIZE>::u const& val )
  { eregwrite( idx, 8, 0, u64_t(val).expr ); }

  void regwrite( GObLH const&, unsigned idx, u8_t val ) { eregwrite( idx%4, 1, (idx>>2) & 1, val.expr ); }
  void regwrite( GOb const&, unsigned idx, u8_t val )   { eregwrite( idx, 1, 0, val.expr ); }
  void regwrite( GOw const&, unsigned idx, u16_t val )  { eregwrite( idx, 2, 0, val.expr ); }

  enum ipproc_t { ipjmp, ipcall, ipret };
    
  addr_t                      getnip() { return next_insn_addr; }
  void                        setnip( addr_t nip, ipproc_t ipproc = ipjmp )
  {
    next_insn_addr = nip;
    next_insn_mode = ipproc;
  }

  // From SIMTEST
  template <class T>
  struct SPRRead : public unisim::util::symbolic::ExprNode
  {
    typedef SPRRead<T> this_type;
    virtual this_type* Mutate() const override { return new this_type( *this ); }
    virtual unsigned SubCount() const override { return 0; }
    virtual int cmp( unisim::util::symbolic::ExprNode const& rhs ) const override { return 0; }
    virtual void Repr( std::ostream& sink ) const override { sink << T::name() << "Read()"; }
    virtual ScalarType::id_t GetType() const override { return T::scalar_type; }
  };

  struct RIP { static ScalarType::id_t const scalar_type = ScalarType::U64; static char const* name() { return "RIP"; } };

  struct RIPRead : public SPRRead<RIP> 
  {
    typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;
  };
    
  struct RIPWrite : public unisim::util::symbolic::binsec::RegWrite
  {
    typedef unisim::util::symbolic::binsec::RegWrite Super;
    RIPWrite( Expr const& _value, ipproc_t _hint ) : Super( _value ), hint(_hint) {}
    virtual RIPWrite* Mutate() const override { return new RIPWrite( *this ); }
    virtual void GetRegName(std::ostream& sink) const override { sink << "rip"; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RIPWrite const&>( rhs ) ); }
    int compare( RIPWrite const& rhs ) const { if (int delta = Super::compare(rhs)) return delta; return int(hint) - int(rhs.hint); }
    ipproc_t hint;
  };
    
  void                        fnanchk( f64_t value ) {};

  int                         fcwreadRC() const { return 0; }
  u16_t                       fcwread() const { throw Unimplemented(); /*FCW access*/ return u16_t(); }
  void                        fcwwrite( u16_t value ) { throw Unimplemented(); /*FCW access*/}
  void                        finit() { throw Unimplemented(); /*FCW access*/ }

  void                        fxam() { throw Unimplemented(); }

      
  template <typename dstT>
  struct Load : public unisim::util::symbolic::binsec::Load
  {
    typedef Load<dstT> this_type;
    typedef unisim::util::symbolic::binsec::Load Super;
    Load( unsigned bytes, unsigned segment, Expr const& addr )
      : Super(addr, bytes, 1, false)
    {
      if (segment >= 4) /*FS|GS relative addressing*/
        throw Unimplemented();
    }
    virtual this_type* Mutate() const override { return new this_type( *this ); }
    virtual ScalarType::id_t GetType() const { return unisim::util::symbolic::TypeInfo<dstT>::GetType(); }
    virtual int cmp( ExprNode const& rhs ) const override { return Super::compare( dynamic_cast<Super const&>( rhs ) ); }
  };
    
  // struct FRegWrite : public Update
  // {
  //   FRegWrite( unsigned _idx, Expr const& _value )
  //     : value(_value), idx(_idx)
  //   {}
  //   virtual FRegWrite* Mutate() const override { return new FRegWrite(*this); }
  //   virtual void Repr( std::ostream& sink ) const override { sink << "FRegWrite( " << idx << ", " << value << " )"; }
  //   virtual unsigned SubCount() const override { return 1; }
  //   virtual Expr const& GetSub(unsigned idx) const override { if (idx == 0) return value; return ExprNode::GetSub(idx); }
  //   virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<FRegWrite const&>( rhs ) ); }
  //   int compare( FRegWrite const& rhs ) const { return int(idx) - int(rhs.idx); }
  //   Expr value;
  //   unsigned idx;
  // };

  struct Store : public unisim::util::symbolic::binsec::Store
  {
    typedef unisim::util::symbolic::binsec::Store Super;
    Store( unsigned bytes, unsigned segment, Expr const& addr, Expr const& value )
      : Super(addr, value, bytes, 1, false)
    {
      if (segment >= 4) /*FS|GS relative addressing*/
        throw Unimplemented();
    }
    virtual Store* Mutate() const override { return new Store( *this ); }
    virtual int cmp( ExprNode const& rhs ) const override { return Super::compare( dynamic_cast<Super const&>(rhs) ); }
  };

  std::set<Expr> stores;
    
  template <typename dstT> Expr PerformLoad( dstT const&, unsigned bytes, unsigned segment, addr_t const& addr )
  {
    //interface.memaccess( addr.expr, false );
    return new Load<dstT>( bytes, segment, addr.expr );
  }
  void PerformStore( unsigned bytes, unsigned segment, addr_t const& addr, Expr const& value )
  {
    //interface.memaccess( addr.expr, true );
    stores.insert( Expr( new Store( bytes, segment, addr.expr, value ) ) );
  }

  template<unsigned OPSIZE>
  typename TypeFor<Processor,OPSIZE>::u
  memread( unsigned seg, addr_t const& addr )
  {
    typedef typename TypeFor<Processor,OPSIZE>::u u_type;
    typedef typename u_type::value_type uval_type;
    return u_type( PerformLoad( uval_type(), OPSIZE/8, seg, addr.expr ) );
  }

  template <unsigned OPSIZE>
  void
  memwrite( unsigned seg, addr_t const& addr, typename TypeFor<Processor,OPSIZE>::u val )
  {
    PerformStore( OPSIZE/8, seg, addr, val.expr );
  }
    
  struct FTop : public unisim::util::symbolic::ExprNode
  {
    virtual FTop* Mutate() const { return new FTop(*this); }
    virtual unsigned SubCount() const { return 0; }
    virtual int cmp(ExprNode const&) const override { return 0; }
    virtual ScalarType::id_t GetType() const { return ScalarType::U8; }
    virtual void Repr( std::ostream& sink ) const;
  };

  struct FTopWrite : public unisim::util::symbolic::binsec::RegWrite
  {
    typedef unisim::util::symbolic::binsec::RegWrite Super;
    FTopWrite( u8_t const& ftop ) : Super( ftop.expr ) {}
    virtual FTopWrite* Mutate() const override { return new FTopWrite(*this); }
    virtual void GetRegName(std::ostream& sink) const { sink << "ftop"; }
    virtual int cmp(ExprNode const& rhs) const override { return Super::compare(dynamic_cast<Super const&>(rhs)); }
  };

  u16_t                       ftopread() { throw Undefined(); /*FCW access*/; return u16_t(); }
  unsigned                    ftop;

  Expr&                       fpaccess(unsigned r, bool w);
  bool                        fpdiff(unsigned r);
    
  Expr                        fpregs[8];
  void                        fpush( f64_t value ) { throw Unimplemented(); }
  void                        fwrite( unsigned idx, f64_t value ) { throw Unimplemented(); }
  f64_t                       fpop() { throw Unimplemented(); return f64_t(); }
  f64_t                       fread( unsigned idx ) { throw Unimplemented(); return f64_t(); }
    
  void                        fmemwrite32( unsigned seg, addr_t const& addr, f32_t val ) { PerformStore(  4, seg, addr, val.expr ); }
  void                        fmemwrite64( unsigned seg, addr_t const& addr, f64_t val ) { PerformStore(  8, seg, addr, val.expr ); }
  void                        fmemwrite80( unsigned seg, addr_t const& addr, f80_t val ) { PerformStore( 10, seg, addr, val.expr ); }
    
  f32_t                       fmemread32( unsigned seg, addr_t const& addr ) { return f32_t( PerformLoad( (float)0,        4, seg, addr ) ); }
  f64_t                       fmemread64( unsigned seg, addr_t const& addr ) { return f64_t( PerformLoad( (double)0,       8, seg, addr ) ); }
  f80_t                       fmemread80( unsigned seg, addr_t const& addr ) { return f80_t( PerformLoad( (long double)0, 10, seg, addr ) ); }
 
  template <unsigned OPSIZE>
  typename TypeFor<Processor,OPSIZE>::u
  regread( unsigned idx )
  {
    typedef typename TypeFor<Processor,OPSIZE>::u u_type;
      
    if (OPSIZE==8)                    return u_type( eregread( idx%4, 1, (idx>>2) & 1 ) );
    if ((OPSIZE==16) or (OPSIZE==32)) return u_type( eregread( idx, OPSIZE/8, 0 ) );
    throw 0;
    return u_type(0);
  }

  template <unsigned OPSIZE>
  void
  regwrite( unsigned idx, typename TypeFor<Processor,OPSIZE>::u const& value )
  {
    if  (OPSIZE==8)                   return eregwrite( idx%4, 1, (idx>>2) & 1, value.expr );
    if ((OPSIZE==16) or (OPSIZE==32)) return eregwrite( idx, OPSIZE/8, 0, value.expr );
    throw 0;
  }
    
  template <unsigned OPSIZE>
  typename TypeFor<Processor,OPSIZE>::u
  pop()
  {
    // TODO: handle stack address size
    gr_type sptr = regread( GR(), 4 );
    regwrite( GR(), 4, sptr + gr_type( OPSIZE/8 ) );
    return memread<OPSIZE>( unisim::component::cxx::processor::intel::SS, addr_t(sptr) );
  }

  template <unsigned OPSIZE>
  void
  push( typename TypeFor<Processor,OPSIZE>::u const& value )
  {
    // TODO: handle stack address size
    gr_type sptr = regread( GR(), 4 ) - gr_type( OPSIZE/8 );
    memwrite<OPSIZE>( unisim::component::cxx::processor::intel::SS, addr_t(sptr), value );
    regwrite( GR(), 4, sptr );
  }

  void shrink_stack( addr_t const& offset ) { regwrite( GR(), 4, regread( GR(), 4 ) + offset ); }
  void grow_stack( addr_t const& offset ) { regwrite( GR(), 4, regread( GR(), 4 ) - offset ); }
    
  template <class GOP>
  typename TypeFor<Processor,GOP::SIZE>::u
  rmread( GOP const& g, RMOp const& rmop )
  {
    if (not rmop.is_memory_operand())
      return regread( g, rmop.ereg() );

    return memread<GOP::SIZE>( rmop->segment, rmop->effective_address( *this ) );
  }

  template <class GOP>
  void
  rmwrite( GOP const& g, RMOp const& rmop, typename TypeFor<Processor,GOP::SIZE>::u const& value )
  {
    if (not rmop.is_memory_operand())
      return regwrite( g, rmop.ereg(), value );

    return memwrite<GOP::SIZE>( rmop->segment, rmop->effective_address( *this ), value );
  }

  template <unsigned OPSIZE>
  typename TypeFor<Processor,OPSIZE>::f
  fpmemread( unsigned seg, addr_t const& addr )
  {
    typedef typename TypeFor<Processor,OPSIZE>::f f_type;
    typedef typename f_type::value_type f_ctype;
    return f_type( PerformLoad( f_ctype(), OPSIZE/8, seg, addr ) );
  }

  template <unsigned OPSIZE>
  typename TypeFor<Processor,OPSIZE>::f
  frmread( RMOp const& rmop )
  {
    typedef typename TypeFor<Processor,OPSIZE>::f f_type;
    if (not rmop.is_memory_operand()) return f_type( fread( rmop.ereg() ) );
    return this->fpmemread<OPSIZE>( rmop->segment, rmop->effective_address( *this ) );
  }
    
  template <unsigned OPSIZE>
  void
  fpmemwrite( unsigned seg, addr_t const& addr, typename TypeFor<Processor,OPSIZE>::f const& value )
  {
    PerformStore( OPSIZE/8, seg, addr, value.expr );
  }
    
  template <unsigned OPSIZE>
  void
  frmwrite( RMOp const& rmop, typename TypeFor<Processor,OPSIZE>::f const& value )
  {
    if (not rmop.is_memory_operand()) return fwrite( rmop.ereg(), f64_t( value ) );
    fpmemwrite<OPSIZE>( rmop->segment, rmop->effective_address( *this ), value );
  }

  // struct MXCSRRead : public RegReadBase
  // {
  //   virtual MXCSRRead* Mutate() const override { return new MXCSRRead( *this ); }
  //   virtual void GetRegName(std::ostream& sink) const override { sink << "mxcsr"; };
  //   virtual ScalarType::id_t GetType() const override { return ScalarType::U16; }
  //   virtual unsigned SubCount() const override { return 0; }
  // };
    
  // struct MXCSRWrite : public RegWriteBase
  // {
  //   MXCSRWrite( Expr const& _value ) : RegWriteBase( _value ) {}
  //   virtual MXCSRWrite* Mutate() const override { return new MXCSRWrite( *this ); }
  //   virtual void GetRegName(std::ostream& sink) const override { sink << "mxcsr"; }
  // };
    
  u32_t mxcsread() { throw Undefined(); /*mxcsr access*/; return u32_t(); };
  void mxcswrite( u32_t const& value ) { throw Undefined(); /*mxcsr access*/; }

  struct VUConfig
  {
    static unsigned const BYTECOUNT = SmartValueTraits<VmmRegister>::bytecount;
    static unsigned const REGCOUNT = VREGCOUNT;
    struct Byte
    {
      Byte() : sexp(), span() {}
      Byte( Expr const& _sexp, int _span ) : sexp(_sexp), span(_span) {}
      Byte(unsigned value) : sexp(unisim::util::symbolic::make_const(uint8_t(value))), span(1) {}
        
      void source( Expr const& _sexp, unsigned _span ) { sexp = _sexp; span = _span; }
      void repeat( unsigned dist ) { sexp = Expr(); span = dist; }

      unsigned is_source() const { return sexp.node ? span : 0; }
      unsigned is_repeat() const { return sexp.node ? 0 : span; }
      bool     is_none() const   { return not sexp.node and not span; }
      ExprNode const* get_node() const { return sexp.node; }
      unsigned size() const { if (not sexp.good()) throw 0; return span; }
      Expr const& expr() const { return sexp; }
        
    protected:
      Expr     sexp;
      unsigned span;
    };

    struct VMix : public ExprNode
    {
      VMix( Expr const& _l, Expr const& _r ) : l(_l), r(_r) {}
      virtual unsigned SubCount() const override { return 0; }
      virtual void Repr( std::ostream& sink ) const override { sink << "VMix( " << l << ", " << r << " )"; }
      virtual int cmp( ExprNode const& brhs ) const override { return 0; }
      virtual VMix* Mutate() const override { return new VMix( *this ); }
      virtual ScalarType::id_t GetType() const override { return l->GetType(); }
      Expr l, r;
    };

    struct VTransBase : public Byte, public ExprNode
    {
      VTransBase( Byte const& byte, int _rshift ) : Byte( byte ), rshift(_rshift) {}
      virtual unsigned SubCount() const override { return 0; }
      virtual void Repr( std::ostream& sink ) const override { sink << "VTrans<"  << ScalarType(GetType()).name << ">({" << sexp << "," << span << "}, " << rshift << ")"; }
      virtual int cmp( ExprNode const& brhs ) const override { return compare( dynamic_cast<VTransBase const&>(brhs) ); }
      int compare( VTransBase const& rhs ) const
      {
        if (int delta = int(span) - int(rhs.span)) return delta;
        return rshift - rhs.rshift;
      }
      int rshift;
    };
      
    template <typename T>
    struct VTrans : public VTransBase
    {
      VTrans( Byte const& byte, int rshift ) : VTransBase( byte, rshift ) {}
      typedef VTrans<T> this_type;
      virtual this_type* Mutate() const override { return new this_type( *this ); }
      virtual ScalarType::id_t GetType() const override { return SmartValueTraits<T>::GetType(); }
    };

    template <typename T>
    struct TypeInfo
    {
      enum { bytecount = SmartValueTraits<T>::bytecount };
      static void ToBytes( Byte* dst, T& src )
      {
        dst->source( src.expr, SmartValueTraits<T>::bytecount );
        for (unsigned idx = 1, end = SmartValueTraits<T>::bytecount; idx < end; ++idx)
          dst[idx].repeat(idx);
      }
      static void FromBytes( T& dst, Byte const* byte )
      {
        if (byte->is_none())
          {
            /* leave uninitialized */
            return;
          }
          
        if (int pos = byte->is_repeat())
          {
            Byte const* base = byte - pos;
            if (not base->is_source()) throw 0;
            dst = T(Expr(new VTrans<T>(*base, -pos)));
            return;
          }
          
        if (unsigned src_size = byte->is_source())
          {
            Expr res = new VTrans<T>(*byte, 0);
            for (unsigned next = src_size, idx = 1; next < bytecount; ++idx)
              {
                // Requested read is a concatenation of multiple source values
                for (;idx < next; ++idx)
                  if (byte[idx].get_node()) throw "corrupted source";
                if (ExprNode const* node = byte[idx].get_node())
                  res = new VMix( new VTrans<T>(byte[idx], int(idx)), res );
                else
                  throw "missing value";
                if (unsigned span = byte[idx].is_source())
                  next = idx + span;
                else
                  throw "corrupted source";
              }
            dst = T(res);
            return;
          }

        throw "corrupted source";
      }
      static void Destroy( T& obj ) { obj.~T(); }
      static void Allocate( T& obj ) { new (&obj) T(); }
    };
  };
  
  struct VmmBrick { char _[sizeof(u8_t)]; };
  typedef unisim::component::cxx::processor::intel::VUnion<VUConfig> VUnion;
  VUnion umms[VUConfig::REGCOUNT];
  VmmBrick vmm_storage[VUConfig::REGCOUNT][VUConfig::BYTECOUNT];
    
  template <class VR> static unsigned vmm_wsize( VR const& vr ) { return VR::size() / 8; }
  static unsigned vmm_wsize( unisim::component::cxx::processor::intel::SSE const& ) { return VUConfig::BYTECOUNT; }

  template <class VR, class ELEM>
  struct VmmIndirectRead : public ExprNode
  {
    typedef unisim::util::symbolic::TypeInfo<typename ELEM::value_type> traits;
    enum { elemcount = VR::SIZE / 8 / traits::BYTECOUNT };
    VmmIndirectRead( ELEM const* elems, u8_t const& _sub) : sub(_sub.expr) { for (unsigned idx = 0, end = elemcount; idx < end; ++idx) sources[idx] = elems[idx].expr; }
    typedef VmmIndirectRead<VR,ELEM> this_type;
    virtual this_type* Mutate() const override { return new this_type( *this ); }
    virtual void Repr( std::ostream& sink ) const override
    {
      sink << "VmmIndirectRead<" << VR::SIZE << ","  << ScalarType(GetType()).name << ">(";
      for (unsigned idx = 0, end = elemcount; idx < end; ++idx)
        sink << sources[idx] << ", ";
      sink << sub << ")";
    }
    virtual unsigned SubCount() const { return elemcount+1; }
    virtual Expr const& GetSub(unsigned idx) const { if (idx < elemcount) return sources[idx]; if (idx == elemcount) return sub; return ExprNode::GetSub(idx); }
    virtual ScalarType::id_t GetType() const override { return traits::GetType(); }
    virtual int cmp( ExprNode const& brhs ) const override { return compare( dynamic_cast<this_type const&>(brhs) ); }
    int compare( this_type const& rhs ) const { return 0; }
    Expr sources[elemcount];
    Expr sub;
  };

  template <class VR, class ELEM>
  ELEM vmm_read( VR const& vr, unsigned reg, u8_t const& sub, ELEM const& e )
  {
    ELEM const* elems = umms[reg].GetConstStorage( &vmm_storage[reg][0], e, vr.size() / 8 );
    return ELEM(Expr(new VmmIndirectRead<VR, ELEM>( elems, sub.expr )));
  }
    
  template <class VR, class ELEM>
  ELEM vmm_read( VR const& vr, unsigned reg, unsigned sub, ELEM const& e )
  {
    ELEM const* elems = umms[reg].GetConstStorage( &vmm_storage[reg][0], e, vr.size() / 8 );
    return elems[sub];
  }
    
  template <class VR, class ELEM>
  void vmm_write( VR const& vr, unsigned reg, unsigned sub, ELEM const& e )
  {
    ELEM* elems = umms[reg].GetStorage( &vmm_storage[reg][0], e, vmm_wsize( vr ) );
    elems[sub] = e;
  }
    
  template <class VR, class ELEM>
  ELEM vmm_read( VR const& vr, RMOp const& rmop, unsigned sub, ELEM const& e )
  {
    if (not rmop.is_memory_operand()) return vmm_read( vr, rmop.ereg(), sub, e );
    return vmm_memread( rmop->segment, rmop->effective_address( *this ) + addr_t(sub*VUConfig::TypeInfo<ELEM>::bytecount), e );
  }

  template <class VR, class ELEM>
  void vmm_write( VR const& vr, RMOp const& rmop, unsigned sub, ELEM const& e )
  {
    if (not rmop.is_memory_operand()) return vmm_write( vr, rmop.ereg(), sub, e );
    return vmm_memwrite( rmop->segment, rmop->effective_address( *this ) + addr_t(sub*VUConfig::TypeInfo<ELEM>::bytecount), e );
  }
    
  // Integer case
  template <class ELEM> ELEM vmm_memread( unsigned seg, addr_t addr, ELEM const& e )
  {
    typedef unisim::component::cxx::processor::intel::atpinfo<Processor,ELEM> atpinfo;
    return ELEM(memread<atpinfo::bitsize>(seg,addr));
  }
  
  f32_t vmm_memread( unsigned seg, addr_t addr, f32_t const& e ) { return fmemread32( seg, addr ); }
  f64_t vmm_memread( unsigned seg, addr_t addr, f64_t const& e ) { return fmemread64( seg, addr ); }
  f80_t vmm_memread( unsigned seg, addr_t addr, f80_t const& e ) { return fmemread80( seg, addr ); }
  
  // Integer case
  template <class ELEM> void vmm_memwrite( unsigned seg, addr_t addr, ELEM const& e )
  {
    typedef unisim::component::cxx::processor::intel::atpinfo<Processor,ELEM> atpinfo;
    memwrite<atpinfo::bitsize>(seg,addr,typename atpinfo::utype(e));
  }
  
  void vmm_memwrite( unsigned seg, addr_t addr, f32_t const& e ) { return fmemwrite32( seg, addr, e ); }
  void vmm_memwrite( unsigned seg, addr_t addr, f64_t const& e ) { return fmemwrite64( seg, addr, e ); }
  void vmm_memwrite( unsigned seg, addr_t addr, f80_t const& e ) { return fmemwrite80( seg, addr, e ); }

  //   =====================================================================
  //   =                      Construction/Destruction                     =
  //   =====================================================================

private:
  Processor( Processor const& );
  
public:
  
  Processor();
  
  bool close( Processor const& ref, uint32_t linear_nia );
  
  void step( Operation const& op );
  
  void noexec( Operation const& op );
  
  void interrupt(int op, int code) { throw Unimplemented(); }
  
  void syscall() { throw Unimplemented(); }
  
  void unimplemented() { throw Unimplemented(); }
  
  void cpuid() { throw Unimplemented(); /*hardware*/ }
  
  void xgetbv() { throw Unimplemented(); /*hardware*/ }
  
  void stop() { throw Unimplemented(); /*hardware*/ }
  
  void _DE()  { throw Unimplemented(); /*system*/ }
  
  //   =====================================================================
  //   =                 Internal Instruction Control Flow                 =
  //   =====================================================================

  bool concretize( Expr const& cexp )
  {
    bool predicate = path->proceed( cexp );
    path = path->next( predicate );
    return predicate;
  }
  
  template <typename T>
  bool Test( unisim::util::symbolic::SmartValue<T> const& cond )
  {
    if (not cond.expr.good())
      throw std::logic_error( "Not a valid condition" );

    Expr cexp( bit_t(cond).expr );
    if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
      return cnode->Get( bool() );

    return concretize( cexp );
  }

  static Operation* Decode(uint64_t address, uint8_t const* bytes);
  
  ActionNode*      path;
  uint64_t         return_address;
  addr_t           next_insn_addr;
  ipproc_t         next_insn_mode;
  //  branch_type_t    branch_type;
};

#endif // ARMD64DBA_ARCH_HH
