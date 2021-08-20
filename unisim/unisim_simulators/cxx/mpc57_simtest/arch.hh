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

#ifndef ARCH_HH
#define ARCH_HH

#include <unisim/util/symbolic/symbolic.hh>
#include <map>
#include <vector>
#include <set>

namespace mpc57
{
  class Operation;
}

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

  typedef SmartValue<uint32_t> ADDRESS;
  typedef SmartValue<uint32_t> UINT;
  typedef SmartValue<int32_t>  SINT;
  
  template <unsigned BITS> struct TypeFor {};
  template <> struct TypeFor<8> { typedef U8 U; typedef S8 S; };
  template <> struct TypeFor<16> { typedef U16 U; typedef S16 S; };
  template <> struct TypeFor<32> { typedef U32 U; typedef S32 S; };
  template <> struct TypeFor<64> { typedef U64 U; typedef S64 S; };
  
  struct Untestable
  {
    Untestable(std::string const& _reason) : reason(_reason) {}
    std::string reason;
  };

  struct ActionNode : public unisim::util::symbolic::Conditional<ActionNode>  {};

  struct Interface
  {
    typedef unisim::util::symbolic::Expr       Expr;
    typedef unisim::util::symbolic::ExprNode   ExprNode;
    
    Interface( mpc57::Operation& op );
    
    struct VirtualRegister
    {
      VirtualRegister() : vindex(0), source(false), destination(false), bad(true) { throw 0; }
      VirtualRegister( unsigned _index ) : vindex(_index), source(false), destination(false), bad(false) {}
    
      uint8_t vindex      : 5;
      uint8_t source      : 1;
      uint8_t destination : 1;
      uint8_t bad         : 1;
    
      void addaccess( bool w ) { source |= not w; destination |= w; }
    
      int  cmp( VirtualRegister const& ) const;
    };
    
    std::map<unsigned,VirtualRegister> irmap;
    std::vector<unsigned>              iruse;
    VirtualRegister                    xer, cr, spefscr;
    std::set<Expr>                     mem_addrs;
    uint8_t                            base_register;
    bool                               aligned;
    bool                               mem_writes;
    uint8_t                            length;
    bool                               retfalse;

    
    void irappend( uint8_t _index, bool w );
    
    int  cmp( Interface const& ) const;
    bool operator < ( Interface const& b ) const { return cmp( b ) < 0; }
    bool usemem() const { return mem_addrs.size(); }
    
    
    struct Prologue
    {
      struct Error {};
      
      typedef std::map<unsigned,uint32_t> Regs;
      Prologue( Regs const& _regs, uint32_t _offset, bool _sign, uint8_t _base )
        : regs( _regs ), offset( _offset ), sign( _sign ), base( _base )
      {}
      Regs regs; uint32_t offset; bool sign; uint8_t base;
    };
    
    Prologue GetPrologue() const;
    
    void PCRelPrologue( Prologue const& pc ) const;
    
    void load( Expr const& addr ) { mem_addrs.insert( addr ); }
    void store( Expr const& addr ) { mem_addrs.insert( addr ); mem_writes = true; }
  };
  
  typedef unisim::util::symbolic::ScalarType ScalarType;
  
  struct SourceReg : public unisim::util::symbolic::ExprNode
  {
    SourceReg( unsigned _reg ) : reg( _reg ) {}
    virtual SourceReg* Mutate() const { return new SourceReg(*this); }
    virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
    virtual void Repr( std::ostream& sink ) const;
    virtual unsigned SubCount() const { return 0; };
    virtual int cmp( unisim::util::symbolic::ExprNode const& rhs ) const override { return compare( dynamic_cast<SourceReg const&>( rhs ) ); }
    int compare( SourceReg const& rhs ) const { return int(reg) - int(rhs.reg); }
    unsigned reg;
  };
  
  struct CPU;

  struct MixNode : public unisim::util::symbolic::ExprNode
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    MixNode( Expr const& _left, Expr const& _right ) : left(_left), right(_right) {}
    virtual MixNode* Mutate() const { return new MixNode(*this); }
    virtual ScalarType::id_t GetType() const { return left->GetType(); }
    virtual void Repr( std::ostream& sink ) const;
    virtual unsigned SubCount() const { return 2; };
    virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return left; case 1: return right; } return ExprNode::GetSub(idx); };
    virtual int cmp( ExprNode const& brhs ) const override { return compare(static_cast<MixNode const&>( brhs )); }
    int compare( MixNode const& rhs ) const { return 0; }
    Expr left, right;
  };
  
  struct UnknownBase : public unisim::util::symbolic::ExprNode
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    UnknownBase() {}
    virtual void Repr( std::ostream& sink ) const { sink << "?"; }
    virtual unsigned SubCount() const { return 0; };
    virtual Expr const& GetSub(unsigned idx) const { return ExprNode::GetSub(idx); };
    virtual int cmp( ExprNode const& brhs ) const override { return 0; }
  };

  template <typename T>
  struct Unknown : public UnknownBase
  {
    virtual UnknownBase* Mutate() const { return new Unknown<T>(*this); }
    virtual ScalarType::id_t GetType() const { return unisim::util::symbolic::TypeInfo<T>::GetType(); }
  };
  
  template <class T>
  T make_unknown() { return T( UnknownBase::Expr( new Unknown<typename T::value_type>() ) ); }
  
  struct XER
  {
    struct OV {};
    struct SO {};
    struct CA {};
    struct _0_3 {};
    
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) { XERAccess(true); xer_value = Expr( new MixNode( xer_value.expr, value.expr ) ); }
    template <typename PART> void Set( uint32_t value ) { Set<PART,uint32_t>( unisim::util::symbolic::make_const(value) ); }
    template <typename PART> U32 Get() { XERAccess(false); return xer_value; }
    operator U32 () { XERAccess(false); return xer_value; }
    XER& operator= ( U32 const& value ) { XERAccess(true); xer_value = value; return *this; }
    XER& GetXER() { return *this; }
    
    struct XERNode : public ExprNode
    {
      virtual XERNode* Mutate() const { return new XERNode(*this); }
      virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
      virtual void Repr( std::ostream& sink ) const;
      virtual unsigned SubCount() const { return 0; };
      virtual int cmp( ExprNode const& brhs ) const override { return 0; }
    };
    
    XER() : xer_value( new XERNode ) {}
    
    U32 xer_value;
    virtual void XERAccess( bool is_write ) = 0;
  };

  struct CR
  {
    struct CR0 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    struct CR1 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    struct CR2 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    struct CR3 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    struct CR4 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    struct CR5 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    struct CR6 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    struct CR7 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) { CRAccess(true); cr_value = Expr( new MixNode( cr_value.expr, value.expr ) ); }
    template <typename PART> void Set( uint32_t value ) { Set<PART,uint32_t>( unisim::util::symbolic::make_const(value) ); }
    template <typename PART> U32 Get() { CRAccess(false); return cr_value; }
    operator U32 () { CRAccess(false); return cr_value; }
    CR& operator= ( U32 const& value ) { CRAccess(true); cr_value = value; return *this; }
    CR& GetCR() { return *this; }
    
    struct CRNode : public ExprNode
    {
      virtual CRNode* Mutate() const { return new CRNode(*this); }
      virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
      virtual void Repr( std::ostream& sink ) const;
      virtual unsigned SubCount() const { return 0; };
      virtual int cmp( ExprNode const& brhs ) const override { return 0; }
    };

    CR() : cr_value( new CRNode ) {}

    U32 cr_value;
    virtual void CRAccess( bool is_write ) = 0;
  };
  
  struct LR
  {
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) { throw Untestable("LR access");  }
    template <typename PART> void Set( unsigned ) { throw Untestable("LR access");  }
    template <typename PART> U32 Get() { return U32(0); }
    operator U32 () { return U32(0); }
    LR& operator= (U32 const& v) { throw Untestable("LR access"); return *this; }
    LR& GetLR() { return *this; }
    void SetLR(U32 const& v) { throw Untestable("LR access"); }
  };
  
  struct CTR
  {
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) { throw Untestable("CTR access"); }
    template <typename PART> void Set( unsigned ) { throw Untestable("CTR access"); }
    template <typename PART> U32 Get() { return U32(0); }
    operator U32 () { return U32(0); }
    CTR& operator= (U32 const& v) { throw Untestable("CTR access"); return *this; }
    CTR& GetCTR() { return *this; }
    void SetCTR(U32 const& v) { throw Untestable("CTR access"); }
  };
  
  struct MSR
  {
    struct PR {};
    struct EE {};
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) { throw Untestable("MSR access"); }
    template <typename PART> void Set( unsigned ) { throw Untestable("MSR access"); }
    template <typename PART> U32 Get() { throw Untestable("MSR access"); return U32(0); }
    operator U32 () { throw Untestable("MSR access"); return U32(0); }
    MSR& operator= (U32 const& v) { throw Untestable("MSR access"); return *this; }
    MSR& GetMSR() { throw Untestable("MSR access"); return *this; }
  };

  struct SPEFSCR
  {
    struct FRMC
    {
      static uint32_t NEAREST() { return 0; }
      static uint32_t ZERO()    { return 1; }
      static uint32_t UP()      { return 2; }
      static uint32_t DOWN()    { return 3; }
    };
    struct FINVE {};
    struct FINV {};
    struct FINVS {};
    
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) { SPEFSCRAccess(true); spefscr_value = Expr( new MixNode( spefscr_value.expr, value.expr ) ); }
    template <typename PART> void Set( uint32_t value ) { Set<PART,uint32_t>( unisim::util::symbolic::make_const(value) ); }
    template <typename PART> U32 Get() { SPEFSCRAccess(false); return spefscr_value; }
    operator U32 () { SPEFSCRAccess(false); return spefscr_value; }
    SPEFSCR& operator= ( U32 const& value ) { SPEFSCRAccess(true); spefscr_value = value; return *this; }
    SPEFSCR& GetSPEFSCR() { return *this; }
    
    struct SPEFSCRNode : public ExprNode
    {
      virtual SPEFSCRNode* Mutate() const { return new SPEFSCRNode(*this); }
      virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
      virtual void Repr( std::ostream& sink ) const;
      virtual unsigned SubCount() const { return 0; };
      virtual int cmp( ExprNode const& brhs ) const override { return 0; }
    };
    
    BOOL SetInvalid( bool inv, bool invh=false )
    {
      this->template Set<FINV>( U32(inv) );
      //this->template Set<FINVH>( U32(invh) );
      return make_unknown<BOOL>();
    }
    BOOL SetUnderflow( bool uf ) { return make_unknown<BOOL>(); }
    BOOL SetOverflow( bool uf ) { return make_unknown<BOOL>(); }
    BOOL SetDivideByZero( bool uf ) { return make_unknown<BOOL>(); }
    
    SPEFSCR() : spefscr_value( new SPEFSCRNode ) {}
    
    U32 spefscr_value;
    virtual void SPEFSCRAccess( bool is_write ) = 0;
  };
    
  struct CPU : public XER, public CR, public MSR, public LR, public CTR, public SPEFSCR
  {
    typedef ut::BOOL BOOL;
    typedef ut::U8   U8;
    typedef ut::U16  U16;
    typedef ut::U32  U32;
    typedef ut::U64  U64;
    typedef ut::S8   S8;
    typedef ut::S16  S16;
    typedef ut::S32  S32;
    typedef ut::S64  S64;

    typedef ut::UINT UINT;
    typedef ut::SINT SINT;
    typedef ut::ADDRESS ADDRESS;
    
    
    typedef unisim::util::symbolic::Expr       Expr;
    typedef unisim::util::symbolic::ExprNode   ExprNode;
    
    typedef MSR MSR;
    typedef SPEFSCR SPEFSCR;
    
    CPU( Interface& _interface, ActionNode& root )
      : interface(_interface), path(&root), cia( new CIA )
    {     
      for (unsigned reg = 0; reg < 32; ++reg)
        reg_values[reg] = U32( new SourceReg( reg ) );
    }
    
    virtual void XERAccess( bool is_write ) { interface.xer.addaccess(is_write); }
    virtual void CRAccess( bool is_write ) { interface.cr.addaccess(is_write); }
    virtual void SPEFSCRAccess( bool is_write ) { interface.spefscr.addaccess(is_write); }
    
    struct Interrupt { void SetELEV(unsigned x); };
    
    struct SystemCallInterrupt
    {
      struct SystemCall {};
    };
    
    struct AlignmentInterrupt
    {
      struct UnalignedLoadStoreMultiple {};
    };
    
    struct ProgramInterrupt
    {
      struct UnimplementedInstruction {};
      struct IllegalInstruction {};
      struct Trap {};
      struct PrivilegeViolation {};
    };
    
    template <class T> Interrupt ThrowException() { DispatchException( T() ); return Interrupt(); }
    
    template <class T> void DispatchException( T const& exc ) { donttest_system(); }
    void DispatchException( ProgramInterrupt::UnimplementedInstruction const& exc ) { throw Untestable("not implemented"); }
    void DispatchException( AlignmentInterrupt::UnalignedLoadStoreMultiple const& exc ) { interface.aligned = true; }
    
    
    template <typename T> bool Cond( SmartValue<T> const& cond )
    {
      if (not cond.expr.good())
        throw std::logic_error( "Not a valid condition" );

      Expr cexp( BOOL(cond).expr );
      if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
        return cnode->Get( bool() );

      bool predicate = path->proceed( cexp );
      path = path->next( predicate );
      return predicate;
    }
    
    bool Cond(bool c) { return c; }
    
    bool close() { return path->close(); }

    Interface&   interface;
    ActionNode*  path;
    U32          reg_values[32];
    U32          cia;
    
    
    struct CIA : public ExprNode
    {
      CIA() {}
      virtual CIA* Mutate() const { return new CIA(*this) ; }
      virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
      virtual void Repr( std::ostream& sink ) const { sink << "CIA"; }
      virtual unsigned SubCount() const { return 0; };
      virtual int cmp( unisim::util::symbolic::ExprNode const& rhs ) const override { return 0; }
    };
    
    U32 GetCIA() { return cia; };
    bool EqualCIA(uint32_t pc) { return false; };
    U32 GetGPR(unsigned n) { gpr_append(n,false); return reg_values[n]; };
    void SetGPR(unsigned n, U32 value) { gpr_append(n,true); reg_values[n] = value; }
    
    static void LoadRepr( std::ostream& sink, Expr const& _addr, unsigned bits );
    
    template <unsigned BITS>
    struct Load : public ExprNode
    {
      Load( Expr const& _addr ) : addr(_addr) {}
      virtual Load* Mutate() const { return new Load(*this); }
      virtual ScalarType::id_t GetType() const { return ScalarType::IntegerType(false,BITS); }
      virtual void Repr( std::ostream& sink ) const { LoadRepr( sink, addr, BITS ); }
      virtual unsigned SubCount() const { return 2; };
      virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return addr; } return ExprNode::GetSub(idx); };
      virtual int cmp( unisim::util::symbolic::ExprNode const& brhs ) const override { return 0; }
      Expr addr;
    };
    
    template <unsigned BITS> Expr MemRead( U32 const& _addr )
    {
      interface.load( _addr.expr );
      return new Load<BITS>( _addr.expr );
    }
    template <unsigned BITS> void MemWrite( U32 const& _addr, typename TypeFor<BITS>::U const& _val )
    {
      interface.store( _addr.expr );
    }
    
    bool Int8Load(unsigned n, U32 const& address) { SetGPR(n, CPU::U32(CPU::U8(MemRead<8>(address)))); return true; }
    bool Int16Load(unsigned n, U32 const& address) { SetGPR(n, CPU::U32(CPU::U16(MemRead<16>(address)))); return true; }
    bool Int32Load(unsigned n, U32 const& address) { SetGPR(n, CPU::U32(MemRead<32>(address))); return true; }
    
    bool SInt8Load(unsigned n, U32 const& address) { SetGPR(n, CPU::U32(CPU::S8(MemRead<8>(address)))); return true; }
    bool SInt16Load(unsigned n, U32 const& address) { SetGPR(n, CPU::U32(CPU::S16(MemRead<16>(address)))); return true; }

    bool Int16LoadByteReverse(unsigned n, U32 const& address) { SetGPR(n, ByteSwap(CPU::U32(CPU::U16(MemRead<16>(address))))); return true; }
    bool Int32LoadByteReverse(unsigned n, U32 const& address) { SetGPR(n, ByteSwap(CPU::U32(MemRead<32>(address)))); return true; }
    
    bool Int8Store(unsigned n, U32 const& address ) { MemWrite<8>( address, U8(GetGPR(n)) ); return true; }
    bool Int16Store(unsigned n, U32 const& address ) { MemWrite<16>( address, U16(GetGPR(n)) ); return true; }
    bool Int32Store(unsigned n, U32 const& address ) { MemWrite<32>( address, U32(GetGPR(n)) ); return true; }

    bool Int16StoreByteReverse(unsigned n, U32 const& address ) { MemWrite<16>( address, ByteSwap(U16(GetGPR(n))) ); return true; }
    bool Int32StoreByteReverse(unsigned n, U32 const& address ) { MemWrite<32>( address, ByteSwap(U32(GetGPR(n))) ); return true; }

    void gpr_append( unsigned idx, bool w ) { interface.irappend( idx, w ); }

    void donttest_system();
    void donttest_branch();
    void donttest_illegal();
    
    char const* GetObjectFriendlyName(U32) { return "???"; }
    
    bool Branch(U32 const& addr) { donttest_branch(); return false; }
    
    bool Rfmci() { donttest_system(); return false; }
    bool Rfci() { donttest_system(); return false; }
    bool Rfdi() { donttest_system(); return false; }
    bool Rfi() { donttest_system(); return false; }

    bool Dcba(U32 const& addr) { donttest_system(); return false; }
    bool Dcbf(U32 const& addr) { donttest_system(); return false; }
    bool Dcbst(U32 const& addr) { donttest_system(); return false; }
    bool Dcbz(U32 const& addr) { donttest_system(); return false; }
    bool Dcbi(U32 const& addr) { donttest_system(); return false; }
    bool Icbi(U32 const& addr) { donttest_system(); return false; }
    bool Icbt(U32 const& addr) { donttest_system(); return false; }
    
    bool Msync() { donttest_system(); return false; }
    bool Isync() { donttest_system(); return false; }
    bool Mpure() { donttest_system(); return false; }
    bool Mpuwe() { donttest_system(); return false; }
    bool Mpusync() { donttest_system(); return false; }
    
    bool Lbarx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    bool Lharx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    bool Lwarx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    bool Stbcx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    bool Sthcx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    bool Stwcx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    bool MoveFromDCR(unsigned dcrn, U32& result) { donttest_system(); return false; }
    bool MoveFromSPR(unsigned dcrn, U32& result) { donttest_system(); return false; }
    bool MoveToSPR(unsigned dcrn, U32 const& result) { donttest_system(); return false; }
    
    bool CheckSPV() { return true; }
    bool Wait() { return false; }

    struct __EFPProcessInput__
    {
      __EFPProcessInput__( CPU& _cpu ) : cpu(_cpu), finv(false) {};
      template <class FLOAT>
      __EFPProcessInput__& set( FLOAT& input ) { finv |= not check_input( input ) ; return *this; }
      template <class FLOAT>
      static bool check_input( FLOAT& input )
      {
        // if (unlikely(input.isDenormalized()))
        //   {
        //     input.setZero(input.isNegative());
        //     return false;
        //   }
			
        // if (unlikely(input.hasInftyExponent()))
        //   return false;
			
        return true;
      }
      bool proceed()
      {
        // cpu.GetSPEFSCR().Set<typename SPEFSCR::FINV>(finv);
        // if (finv)
        //   {
        //     cpu.GetSPEFSCR().Set<typename SPEFSCR::FINVS>(true);
        //     if (cpu.GetSPEFSCR().Get<typename SPEFSCR::FINVE>())
        //       {
        //         cpu.ThrowException<typename ProgramInterrupt::UnimplementedInstruction>();
        //         return false;
        //       }
        //   }
        return true;
      }
		
      CPU& cpu;
      bool finv;
    };
	
    __EFPProcessInput__
    EFPProcessInput()
    {
      // GetSPEFSCR().Set<typename SPEFSCR::FG>(false);
      // GetSPEFSCR().Set<typename SPEFSCR::FX>(false);
      // GetSPEFSCR().Set<typename SPEFSCR::FGH>(false);
      // GetSPEFSCR().Set<typename SPEFSCR::FXH>(false);
      // GetSPEFSCR().Set<typename SPEFSCR::FDBZ>(false);
      // GetSPEFSCR().Set<typename SPEFSCR::FDBZH>(false);
      // GetSPEFSCR().SetDivideByZero( false );
      return __EFPProcessInput__( *this );
    }
	
    template <class FLOAT, class FLAGS>
    bool
    EFPProcessOutput( FLOAT& output, FLAGS const& flags )
    {
      // if (output.hasInftyExponent())
      //   {
      //     bool neg = output.isNegative();
      //     output.setInfty();
      //     output.setToPrevious();
      //     output.setNegative(neg);
      //   }
      // bool inexact = flags.isApproximate() and not spefscr.template Get<typename CPU::SPEFSCR::FINV>();
      // bool overflow = inexact and flags.isOverflow();
      // if (not GetSPEFSCR().SetOverflow( overflow ))
      //   return false;
      // bool underflow = inexact and flags.isUnderflow();
      // if (output.isDenormalized())
      //   {
      //     output.setZero(output.isNegative());
      //     inexact = true, underflow = true;
      //   }
      // if (not GetSPEFSCR().SetUnderflow( underflow ))
      //   return false;

      // if (inexact)
      //   {
      //     // Compute inexact flags (FG, FX)
      //     GetSPEFSCR().Set<typename SPEFSCR::FINXS>(true);
      //     if (spefscr.template Get<typename SPEFSCR::FINXE>())
      //       {
      //         this->template ThrowException<typename ProgramInterrupt::UnimplementedInstruction>();
      //         return false;
      //       }
      //   }

      return true;
    }
	
    
  };
  
  struct Flags
  {
    struct RoundingMode { RoundingMode(U32 const& rm) {} RoundingMode(unsigned rm) {} };
    Flags( RoundingMode const& rm ) {}
    Flags() {}
    void setRoundingMode(U32 const&) {}
    void setRoundingMode(unsigned) {}
    void setUnderflow() {}
    void setOverflow() {}
    void setDownApproximate() {}
    void setUpApproximate() {}
    void clearFlowException() {}
    BOOL isUpApproximate() { return make_unknown<BOOL>(); }
    BOOL isDownApproximate() { return make_unknown<BOOL>(); }
    BOOL isOverflow() { return make_unknown<BOOL>(); }
    BOOL takeOverflow() { return make_unknown<BOOL>(); }
  };
  
  struct BigInt
  {
    BigInt() {}
    BigInt& sub(int) { return *this; }
    BigInt& add(int) { return *this; }
    BOOL hasCarry() { return make_unknown<BOOL>(); }
    BOOL isZero() { return make_unknown<BOOL>(); }
  };
  
  struct SoftHalfFloat;
  
  struct SoftFloat
  {
    enum __FromRawBits__ { FromRawBits };
    SoftFloat( __FromRawBits__, U32 const& source ) {}
    
    SoftFloat( U32 const&, Flags& ) {}
    SoftFloat( S32 const&, Flags& ) {}
    enum __FromFraction__ { FromFraction };
    SoftFloat( __FromFraction__, U32 const&, Flags& ) {}
    SoftFloat( __FromFraction__, S32 const&, Flags& ) {}
    SoftFloat( SoftHalfFloat const&, Flags const& ) {}

    U32 queryU32( Flags& flags, unsigned fbits=0 ) { return make_unknown<U32>(); }
    S32 queryS32( Flags& flags, unsigned fbits=0 ) { return make_unknown<S32>(); }

    // SoftFloat& convertAssign( SoftHalfFloat const&, Flags& );
    // SoftFloat& fromRawBitsAssign( U32 const& );
    
    U32 queryRawBits() { return make_unknown<U32>(); }
    BOOL isNaN() { return make_unknown<BOOL>(); }
    BOOL operator == (SoftFloat const&) { return make_unknown<BOOL>(); }
    BOOL operator < (SoftFloat const&) { return make_unknown<BOOL>(); }
    BOOL operator > (SoftFloat const&) { return make_unknown<BOOL>(); }
    // void retrieveInteger(IntConversion const&, Flags const&) {}
    // BOOL isPositive() { return make_unknown<BOOL>(); }
    BOOL isNegative() { return make_unknown<BOOL>(); }
    void setNegative(bool=false) {}
    void setNegative(BOOL) {}
    void plusAssign(SoftFloat const&, Flags const& flags) {}
    void divAssign(SoftFloat const&, Flags const& flags) {}
    void multAssign(SoftFloat const&, Flags const& flags) {}
    void minusAssign(SoftFloat const&, Flags const& flags) {}
    void sqrtAssign(Flags const& flags) {}
    // void squareAssign(Flags const& flags) {}
    void multAndAddAssign(SoftFloat const&, SoftFloat const&, Flags const& flags) {}
    void multAndSubAssign(SoftFloat const&, SoftFloat const&, Flags const& flags) {}
    // void multNegativeAndAddAssign(SoftFloat const&, SoftFloat const&, Flags const& flags) {}
    // void multNegativeAndSubAssign(SoftFloat const&, SoftFloat const&, Flags const& flags) {}
    void maxAssign(SoftFloat const&) {}
    void minAssign(SoftFloat const&) {}
    
    // BigInt querySBasicExponent() { return BigInt(); }
    // BigInt queryBasicExponent() { return BigInt(); }
    BOOL isZero() { return make_unknown<BOOL>(); }
    BOOL isNormalized() { return make_unknown<BOOL>(); }
    // void clear() {}
    
    // BOOL hasInftyExponent() { return make_unknown<BOOL>(); }
    // BOOL isDenormalized() { return make_unknown<BOOL>(); }
  };

  struct SoftHalfFloat
  {
    enum __FromRawBits__ { FromRawBits };
    SoftHalfFloat( __FromRawBits__, U16 const& ) {}
    SoftHalfFloat( SoftFloat const&, Flags const& ) {}
    U16 queryRawBits() { return make_unknown<U16>(); }
  };
  
  static const unsigned int RN_NEAREST = 0;
  static const unsigned int RN_ZERO = 1;
  static const unsigned int RN_UP = 2;
  static const unsigned int RN_DOWN = 3;

  inline void GenSPEFSCR_FOVF(SPEFSCR& spefscr, const Flags& flags) { spefscr = make_unknown<U32>(); }
  inline void GenSPEFSCR_FUNF(SPEFSCR& spefscr, const Flags& flags) { spefscr = make_unknown<U32>(); }
  inline void GenSPEFSCR_FINXS(SPEFSCR& spefscr, const Flags& flags) { spefscr = make_unknown<U32>(); }
  inline void GenSPEFSCR_FDBZ(SPEFSCR& spefscr, const Flags& flags) { spefscr = make_unknown<U32>(); }
  inline void GenSPEFSCR_FG(SPEFSCR& spefscr, const Flags& flags, bool x=false) { spefscr = make_unknown<U32>(); }
  inline void GenSPEFSCR_FX(SPEFSCR& spefscr, const Flags& flags, bool x=false) { spefscr = make_unknown<U32>(); }
  inline void GenSPEFSCR_FG(SPEFSCR& spefscr, const SoftFloat& result) { spefscr = make_unknown<U32>(); }
  inline BOOL HasSPEFSCR_InvalidInput(const SoftFloat& input) { return make_unknown<BOOL>(); }
  template <class RESULT>
  inline void GenSPEFSCR_DefaultResults(SPEFSCR& spefscr, RESULT& result) { spefscr = make_unknown<U32>(); }
  template <class FLOAT>
  inline void GenSPEFSCR_FINV(SPEFSCR& spefscr, FLOAT& first, FLOAT* second = 0, FLOAT* third = 0, bool x=false, bool y=false) { spefscr = make_unknown<U32>(); }

  inline BOOL DoesSPEFSCR_TriggerException(const SPEFSCR& spefscr) { return BOOL(false); }
  
  inline void GenSPEFSCR_TriggerException(CPU* cpu) { cpu->donttest_system(); }
  
  extern void SignedAdd32(U32& result, U8& carry_out, U8& overflow, U8& sign, U32 x, U32 y, U8 carry_in);
  extern inline void SignedAdd32(U32& result, U8& carry_out, U8& overflow, U8& sign, U32 x, U32 y, int carry_in)
  { return SignedAdd32(result, carry_out, overflow, sign, x, y, U8(carry_in)); }

  struct MaskNode : public unisim::util::symbolic::ExprNode
  {
    typedef unisim::util::symbolic::Expr Expr;
    
    MaskNode( Expr const& _mb, Expr const& _me ) : mb(_mb), me(_me) {}
    virtual MaskNode* Mutate() const { return new MaskNode(*this); }
    virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
    virtual void Repr( std::ostream& sink ) const;
    virtual unsigned SubCount() const { return 2; };
    virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return mb; case 1: return me; } return ExprNode::GetSub(idx); };
    virtual int cmp( ExprNode const& brhs ) const override { return 0; }
    
    Expr mb, me;
  };
  
  extern U32 Mask(U32 mb, U32 me);
  
}

#endif // ARCH_HH
