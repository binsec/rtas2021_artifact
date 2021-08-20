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

#include <simfloat.hh>
#include <types.hh>
#include <unisim/util/symbolic/symbolic.hh>
#include <map>
#include <vector>
#include <set>

namespace e5500 { struct Operation; }

namespace ut
{
  typedef uint64_t Offset;
  
  struct Untestable
  {
    Untestable(std::string const& _reason) : reason(_reason) {}
    std::string reason;
  };

  struct ActionNode : public unisim::util::symbolic::Conditional<ActionNode> {};

  struct Interface
  {
    typedef unisim::util::symbolic::Expr       Expr;
    typedef unisim::util::symbolic::ExprNode   ExprNode;
    
    
    Interface( e5500::Operation& op );
    
    struct VirtualRegister
    {
      VirtualRegister() : vindex(0), source(false), destination(false), bad(true) { throw 0; }
      VirtualRegister( unsigned _index ) : vindex(_index), source(false), destination(false), bad(false) {}
    
      void addaccess( bool w ) { source |= not w; destination |= w; }
    
      int  cmp( VirtualRegister const& ) const;

      bool used() const { return source | destination; }
    
      uint8_t vindex      : 5;
      uint8_t source      : 1;
      uint8_t destination : 1;
      uint8_t bad         : 1;
    };

    struct RegBank
    {
      void append( uint8_t _index, bool w );

      int  cmp( RegBank const& ) const;
      uintptr_t count() const { return vmap.size(); }
      unsigned used() const
      {
        unsigned flags = 0;
        for (auto&& vr : vmap)
          flags |= unsigned(vr.second.used()) << vr.first;
        return flags;
      }
      
      std::map<unsigned,VirtualRegister> vmap;
      std::vector<unsigned>              refs;
    };

    uint32_t GetRegs() const
    {
      return (iregs.used() >> 4) | (fregs.used() >> 0)
        | (unsigned(cr.used())    <<  8)
        | (unsigned(fpscr.used()) <<  9)
        | (unsigned(xer.used())   << 10)
        // | (unsigned(lr.used())    << 11)
        // | (unsigned(ctr.used())   << 12)
        ;
    }

    void irappend( uint8_t idx, bool w )
    {
      if ((idx < 4) or (idx >= 8))
        throw Untestable("Wild register access");
      iregs.append( idx, w );
    }
    void frappend( uint8_t idx, bool w )
    {
      if ((idx < 4) or (idx >= 8))
        throw Untestable("Wild register access");
      fregs.append( idx, w );
    }
    
    int  cmp( Interface const& ) const;
    bool operator < ( Interface const& b ) const { return cmp( b ) < 0; }
    bool usemem() const { return mem_addrs.size(); }
    
    struct Prologue
    {
      struct Error {};
      typedef std::map<unsigned,Offset> Regs;
      Prologue() : regs(), offset(), sign() {}
      void Resolve(Interface const& iif);
      Regs regs; Offset offset; bool sign;
    };
    
    void GetPrologue( Prologue& p ) const;
    
    void PCRelPrologue( Prologue const& pc ) const;
    
    void load( Expr const& addr ) { mem_addrs.insert( addr ); }
    void store( Expr const& addr ) { mem_addrs.insert( addr ); mem_writes = true; }
    
    RegBank                            iregs, fregs;
    VirtualRegister                    xer, cr, fpscr;
    std::set<Expr>                     mem_addrs;
    uint8_t                            base_register;
    bool                               aligned;
    bool                               mem_writes;
    uint8_t                            length;
    bool                               retfalse;
  };
  
  struct SourceReg : public ArchExprNode
  {
    SourceReg( Arch& _arch, unsigned _reg ) : ArchExprNode( _arch ), reg( _reg ) {}
    virtual SourceReg* Mutate() const override { return new SourceReg(*this); }
    virtual void Repr( std::ostream& sink ) const;
    virtual unsigned SubCount() const { return 0; };
    virtual int cmp( unisim::util::symbolic::ExprNode const& rhs ) const override { return compare( dynamic_cast<SourceReg const&>( rhs ) ); }
    int compare( SourceReg const& rhs ) const { return int(reg) - int(rhs.reg); }
    virtual ScalarType::id_t GetType() const { return ScalarType::U64; }
    unsigned reg;
  };
  
  struct Unknown : public ArchExprNode
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    typedef unisim::util::symbolic::ScalarType ScalarType;
    
    Unknown( Arch& _arch ) : ArchExprNode( _arch ) {}
    virtual Unknown* Mutate() const { return new Unknown(*this); }
    virtual void Repr( std::ostream& sink ) const { sink << "?"; }
    virtual unsigned SubCount() const { return 0; };
    virtual Expr const& GetSub(unsigned idx) const { return ExprNode::GetSub(idx); };
    virtual int cmp( ExprNode const& brhs ) const override { return 0; }
    virtual ScalarType::id_t GetType() const { return ScalarType::VOID; }
  };
  
  template <class T>
  T make_unknown( Arch* arch ) { if (not arch) throw 0; return T( Unknown::Expr( new Unknown( *arch ) ) ); }
  
  struct XER
  {
    struct OV {};
    struct SO {};
    struct CA {};
    struct _0_3 {};
    
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) { XERAccess(true); xer_value = make_function("mix", xer_value.expr, value.expr ); }
    template <typename PART> void Set( uint32_t value ) { Set<PART,uint32_t>( unisim::util::symbolic::make_const(value) ); }
    template <typename PART> U64 Get() { XERAccess(false); return xer_value; }
    operator U64 () { XERAccess(false); return xer_value; }
    XER& operator= ( U64 const& value ) { XERAccess(true); xer_value = value; return *this; }
    XER& GetXER() { return *this; }
    
    struct XERNode : public ArchExprNode
    {
      typedef unisim::util::symbolic::ScalarType ScalarType;
      XERNode( Arch& _arch ) : ArchExprNode( _arch ) {}
      virtual XERNode* Mutate() const { return new XERNode(*this); }
      virtual void Repr( std::ostream& sink ) const;
      virtual unsigned SubCount() const { return 0; };
      virtual int cmp( ExprNode const& brhs ) const override { return 0; }
      virtual ScalarType::id_t GetType() const { return ScalarType::U64; }
    };
    
    XER( Arch& _arch ) : xer_value( new XERNode( _arch ) ) {}
    
    U64 xer_value;
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
    
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) { CRAccess(true); cr_value = make_function( "mix", cr_value.expr, value.expr ); }
    template <typename PART> void Set( uint32_t value ) { Set<PART,uint32_t>( unisim::util::symbolic::make_const(value) ); }
    template <typename PART> U32 Get() { CRAccess(false); return cr_value; }
    operator U32 () { CRAccess(false); return cr_value; }
    CR& operator= ( U32 const& value ) { CRAccess(true); cr_value = value; return *this; }
    CR& GetCR() { return *this; }
    
    struct CRNode : public ArchExprNode
    {
      CRNode( Arch& _arch ) : ArchExprNode( _arch ) {}
      virtual CRNode* Mutate() const { return new CRNode(*this); }
      virtual void Repr( std::ostream& sink ) const;
      virtual unsigned SubCount() const { return 0; };
      virtual int cmp( ExprNode const& brhs ) const override { return 0; }
      virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
    };

    CR( Arch& _arch ) : cr_value( new CRNode( _arch ) ) {}
    
    U32 cr_value;
    virtual void CRAccess( bool is_write ) = 0;
  };

  struct FPSCR
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    struct _0_3  {};  // FX, FEX, VX, OX bunch
    struct FX    {};     // Floating-Point Exception Summary
    struct FEX   {};     // Floating-Point Enabled Exception Summary
    struct VX    {};     // Floating-Point Invalid Operation Exception Summary
    struct OX    {};     // Floating-Point Overflow Exception
    struct UX    {};     // Floating-Point Underflow Exception
    struct ZX    {};     // Floating-Point Zero Divide Exception
    struct XX    {};     // Floating-Point Inexact Exception
    struct VXSNAN{};     // Floating-Point Invalid Operation Exception (SNaN)
    struct VXISI {};     // Floating-Point Invalid Operation Exception (inf - inf)
    struct VXIDI {};     // Floating-Point Invalid Operation Exception (inf / inf)
    struct VXZDZ {};     // Floating-Point Invalid Operation Exception (0 / 0)
    struct VXIMZ {};     // Floating-Point Invalid Operation Exception (inf * 0)
    struct VXVC  {};     // Floating-Point Invalid Operation Exception (Invalid Compare)
    struct FR    {};     // Floating-Point Fraction Rounded
    struct FI    {};     // Floating-Point Fraction Inexact
    struct FPRF          // Floating-Point Result Flags
    {
      static uint64_t          QNAN    () { return 0x11; }
      static uint64_t NEGATIVE_INFINITY() { return 0x9; }
      static uint64_t NEGATIVE_NORMAL  () { return 0x8; }
      static uint64_t NEGATIVE_DENORMAL() { return 0x18; }
      static uint64_t NEGATIVE_ZERO    () { return 0x12; }
      static uint64_t POSITIVE_ZERO    () { return 0x2; }
      static uint64_t POSITIVE_DENORMAL() { return 0x14; }
      static uint64_t POSITIVE_NORMAL  () { return 0x4; }
      static uint64_t POSITIVE_INFINITY() { return 0x5; }
    };
    struct C     {};     // Floating-Point Result Class Descriptor
    struct FPCC  {}; // Floating-Point Condition Code
    struct FL    {};     // Floating-Point Less Than or Negative
    struct FG    {};     // Floating-Point Greater Than or Positive
    struct FE    {};     // Floating-Point Equal or Zero
    struct FU    {};     // Floating-Point Unordered or NaN
    struct VXSOFT{};     // Floating-Point Invalid Operation Exception (Software-Defined Condition)
    struct VXSQRT{};     // Floating-Point Invalid Operation Exception (Invalid Square Root)
    struct VXCVI {};     // Floating-Point Invalid Operation Exception (Invalid Integer Convert)
    struct VE    {};     // Floating-Point Invalid Operation Exception Enable
    struct OE    {};     // Floating-Point Overflow Exception Enable
    struct UE    {};     // Floating-Point Underflow Exception Enable
    struct ZE    {};     // Floating-Point Zero Divide Exception  Enable
    struct XE    {};     // Floating-Point Inexact Exception Enable
    struct NI    {};     // Floating-Point Non-IEEE Mode
    struct RN    {}; // Floating-Point Rounding Control
  
    template <typename PART,typename T> void Set( SmartValue<T> const& value )
    { FPSCRAccess(false); FPSCRAccess(true); fpscr_value = make_function( "mix", fpscr_value.expr, value.expr ); }
    template <typename PART> void Set( uint64_t value ) { Set<PART,uint64_t>( unisim::util::symbolic::make_const(value) ); }
    template <typename PART> U64 Get() { FPSCRAccess(false); return fpscr_value; }
    operator U64 () { FPSCRAccess(false); return fpscr_value; }
    FPSCR& operator= ( U64 const& value ) { FPSCRAccess(true); fpscr_value = value; return *this; }
    FPSCR& GetFPSCR() { return *this; }
    void SetFPSCR(U64 const& value) { FPSCRAccess(true); fpscr_value = value; }

    void SetInexact( BOOL i )
    {
      Set<FI>( UINT(i) );
      // if (evenly(i))
      //   SetInvalid( XX() );
    }
  
    template <class FIELD>
    void SetInvalid( FIELD const& )
    {
      Set<FIELD>( UINT(1) );
      // SetException( VX() );
    }

    template <class FIELD>
    void SetException( FIELD const& )
    {
      Set<FIELD>( UINT(1) );
      // Set<FX>( UINT(1) );
      // struct Enable : Field<Enable, FIELD::offset1 + 22> {};
      // if (Get<Enable>() == UINT(1))
      //   Register<FPSCR>::Set<FEX>( UINT(1) );
    }
    struct FPSCRNode : public ArchExprNode
    {
      FPSCRNode( Arch& _arch ) : ArchExprNode( _arch ) {}
      virtual FPSCRNode* Mutate() const { return new FPSCRNode(*this); }
      virtual void Repr( std::ostream& sink ) const;
      virtual unsigned SubCount() const { return 0; };
      virtual int cmp( ExprNode const& brhs ) const override { return 0; }
      virtual ScalarType::id_t GetType() const { return ScalarType::U64; }
    };

    FPSCR( Arch& _arch ) : fpscr_value( new FPSCRNode( _arch ) ) {}
    
    U64 fpscr_value;
    virtual void FPSCRAccess( bool is_write ) = 0;
  };
  
  struct LR
  {
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) { throw Untestable("LR access");  }
    template <typename PART> void Set( unsigned ) { throw Untestable("LR access");  }
    template <typename PART> U64 Get() { return U64(0); }
    operator U64 () { return U64(0); }
    LR& operator= (U64 const& v) { throw Untestable("LR access"); return *this; }
    LR& GetLR() { return *this; }
    void SetLR(U64 const& v) { throw Untestable("LR access"); }
  };
  
  struct CTR
  {
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) { throw Untestable("CTR access"); }
    template <typename PART> void Set( unsigned ) { throw Untestable("CTR access"); }
    template <typename PART> U64 Get() { return U64(0); }
    operator U64 () { return U64(0); }
    CTR& operator= (U64 const& v) { throw Untestable("CTR access"); return *this; }
    CTR& GetCTR() { return *this; }
    void SetCTR(U64 const& v) { throw Untestable("CTR access"); }
  };
  
  struct MSR
  {
  struct SPV {};
  struct WE  {};
  struct CE  {};
  struct EE  {};
  struct PR  {};
  struct FP  {};
  struct ME  {};
  struct FE0 {};
  struct DE  {};
  struct FE1 {};
  struct IS  {};
  struct DS  {};
  struct PMM {};
  struct RI  {};
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) { throw Untestable("MSR access"); }
    template <typename PART> void Set( unsigned ) { throw Untestable("MSR access"); }
    template <typename PART> U64 Get() { throw Untestable("MSR access"); return U64(0); }
    operator U64 () { throw Untestable("MSR access"); return U64(0); }
    MSR& operator= (U64 const& v) { throw Untestable("MSR access"); return *this; }
    MSR& GetMSR() { throw Untestable("MSR access"); return *this; }
  };
  
  struct Arch : public XER, public CR, public MSR, public LR, public CTR, public FPSCR
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
    // typedef SPEFSCR SPEFSCR;
    
    Arch( Interface& _interface, ActionNode& root )
      : XER(*this), CR(*this), FPSCR(*this), interface(_interface), path(&root), cia( new CIA( *this ) )
    {
      for (unsigned reg = 0; reg < 32; ++reg)
        reg_values[reg] = U64( new SourceReg( *this, reg ) );
      for (unsigned reg = 0; reg < 32; ++reg)
        fpr_values[reg].expr = new SourceReg( *this, reg );
    }
    
    virtual void XERAccess( bool is_write ) { interface.xer.addaccess(is_write); }
    virtual void CRAccess( bool is_write ) { interface.cr.addaccess(is_write); }
    virtual void FPSCRAccess( bool is_write ) { interface.fpscr.addaccess(is_write); }
    
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
    
    struct FloatingPointUnavailableInterrupt
    {
      struct FloatingPointUnavailable { typedef FloatingPointUnavailableInterrupt Interrupt; };
    };
  
    template <class T> Interrupt ThrowException() { DispatchException( T() ); return Interrupt(); }
    
    template <class T> void DispatchException( T const& exc ) { donttest_system(); }
    void DispatchException( ProgramInterrupt::UnimplementedInstruction const& exc ) { throw Untestable("not implemented"); }
    void DispatchException( AlignmentInterrupt::UnalignedLoadStoreMultiple const& exc ) { interface.aligned = true; }
    
    
    bool Choose( Expr const& cexp )
    {
      bool predicate = path->proceed( cexp );
      path = path->next( predicate );
      return predicate;
    }
    
    template <typename T>
    static bool Concretize( SmartValue<T> const& cond )
    {
      if (not cond.expr.good())
        throw std::logic_error( "Not a valid condition" );

      Expr cexp( BOOL(cond).expr );
      if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
        return cnode->Get( bool() );
      
      Arch* arch = ArchExprNode::SeekArch(cexp);
      if (not arch)
        throw 0;
      
      return arch->Choose( cexp );
    }
    
    static bool Concretize( bool c ) { return c; }
    
    bool close() { return path->close(); }

    Interface&   interface;
    ActionNode*  path;
    U64          reg_values[32];
    U64          cia;
    SoftDouble   fpr_values[32];
    
    
    struct CIA : public ArchExprNode
    {
      CIA( Arch& _arch ) : ArchExprNode( _arch ) {}
      virtual CIA* Mutate() const override { return new CIA(*this); }
      virtual void Repr( std::ostream& sink ) const { sink << "CIA"; }
      virtual unsigned SubCount() const { return 0; };
      virtual int cmp( unisim::util::symbolic::ExprNode const& brhs ) const override { return 0; }
      virtual ScalarType::id_t GetType() const { return ScalarType::U64; }
    };
    
    U64 GetCIA() { return cia; };
    // bool EqualCIA(uint32_t pc) { return false; };
    U64 GetGPR(unsigned n) { gpr_append(n,false); return reg_values[n]; };
    void SetGPR(unsigned n, U64 value) { gpr_append(n,true); reg_values[n] = value; }

    SoftDouble const& GetFPR(unsigned n) { fpr_append(n,false); return fpr_values[n]; }
    void SetFPR(unsigned n, SoftDouble const& value) { fpr_append(n,true); fpr_values[n] = value; }
    
    static void LoadRepr( std::ostream& sink, Expr const& _addr, unsigned bits );
    
    template <unsigned BITS>
    struct Load : public ArchExprNode
    {
      Load( Arch& _arch, Expr const& _addr ) : ArchExprNode(_arch), addr(_addr) {}
      virtual Load* Mutate() const { return new Load(*this); }
      virtual void Repr( std::ostream& sink ) const { LoadRepr( sink, addr, BITS ); }
      virtual unsigned SubCount() const { return 2; };
      virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return addr; } return ExprNode::GetSub(idx); };
      virtual int cmp( unisim::util::symbolic::ExprNode const& brhs ) const override { return 0; }
      Expr addr;
      virtual ScalarType::id_t GetType() const { return ScalarType::IntegerType(false,BITS); }
    };
    
    template <unsigned BITS> Expr MemRead( ADDRESS const& _addr )
    {
      interface.load( _addr.expr );
      return new Load<BITS>( *this, _addr.expr );
    }
    template <unsigned BITS> void MemWrite( ADDRESS const& _addr, typename TypeFor<BITS>::U const& _val )
    {
      interface.store( _addr.expr );
    }
    
    bool Int8Load(unsigned n, ADDRESS const& address) { SetGPR(n, UINT(U8(MemRead<8>(address)))); return true; }
    bool Int16Load(unsigned n, ADDRESS const& address) { SetGPR(n, UINT(U16(MemRead<16>(address)))); return true; }
    bool Int32Load(unsigned n, ADDRESS const& address) { SetGPR(n, UINT(U32(MemRead<32>(address)))); return true; }
    bool Int64Load(unsigned n, ADDRESS const& address) { SetGPR(n, UINT(U64(MemRead<64>(address)))); return true; }
    
    bool SInt8Load(unsigned n, ADDRESS const& address) { SetGPR(n, UINT(S8(MemRead<8>(address)))); return true; }
    bool SInt16Load(unsigned n, ADDRESS const& address) { SetGPR(n, UINT(S16(MemRead<16>(address)))); return true; }
    bool SInt32Load(unsigned n, ADDRESS const& address) { SetGPR(n, UINT(S32(MemRead<32>(address)))); return true; }
    bool SInt64Load(unsigned n, ADDRESS const& address) { SetGPR(n, UINT(S64(MemRead<64>(address)))); return true; }

    // bool Int16LoadByteReverse(unsigned n, ADDRESS const& address) { SetGPR(n, ByteSwap(U32(U16(MemRead<16>(address))))); return true; }
    // bool Int32LoadByteReverse(unsigned n, ADDRESS const& address) { SetGPR(n, ByteSwap(U32(MemRead<32>(address)))); return true; }
    
    bool Int8Store(unsigned n, ADDRESS const& address ) { MemWrite<8>( address, U8(GetGPR(n)) ); return true; }
    bool Int16Store(unsigned n, ADDRESS const& address ) { MemWrite<16>( address, U16(GetGPR(n)) ); return true; }
    bool Int32Store(unsigned n, ADDRESS const& address ) { MemWrite<32>( address, U32(GetGPR(n)) ); return true; }
    bool Int64Store(unsigned n, ADDRESS const& address ) { MemWrite<64>( address, U64(GetGPR(n)) ); return true; }

    bool Fp32Load(unsigned n, ADDRESS const& address)
    {
      SoftFloat sf(SoftFloat::FromRawBits, MemRead<32>(address));
      Flags flags(Flags::RoundingMode(GetFPSCR().Get<FPSCR::RN>()));
      SetFPR(n, SoftDouble( sf, flags ));
      return true;
    }
    
    bool Fp64Load(unsigned n, ADDRESS const& addr) { return SetFPR(n, SoftDouble(SoftDouble::FromRawBits,MemRead<64>(addr))), true; }
    // bool Int16StoreByteReverse(unsigned n, ADDRESS const& address ) { MemWrite<16>( address, ByteSwap(U16(GetGPR(n))) ); return true; }
    // bool Int32StoreByteReverse(unsigned n, ADDRESS const& address ) { MemWrite<32>( address, ByteSwap(U32(GetGPR(n))) ); return true; }

    bool Fp32Store(unsigned n, ADDRESS const& addr)
    {
      Flags flags(Flags::RoundingMode(GetFPSCR().Get<FPSCR::RN>()));
      MemWrite<32>( addr, SoftFloat(GetFPR(n), flags).queryRawBits() );
      return true;
    }

    bool Fp64Store(unsigned n, ADDRESS const& addr) { MemWrite<64>( addr, GetFPR(n).queryRawBits() ); return true; }
    bool FpStoreLSW(unsigned n, ADDRESS const& addr) { MemWrite<32>( addr, U32(GetFPR(n).queryRawBits()) ); return true; }
    
    void gpr_append( unsigned idx, bool w ) { interface.irappend( idx, w ); }
    void fpr_append( unsigned idx, bool w ) { interface.frappend( idx, w ); }

    void donttest_system();
    void donttest_branch();
    // void donttest_illegal();
    
    bool Branch(U64 const& addr) { donttest_branch(); return false; }

    bool        GetMSR_FP() { return true; }
    bool        CheckFloatingPointException() { return true; }

    static bool const HAS_FPU = true;
    static bool const HAS_FLOATING_POINT_GRAPHICS_INSTRUCTIONS = true;
    static bool const HAS_FLOATING_POINT_SQRT = true;
  };


  inline U64 UnsignedMultiplyHigh( U64 lop, U64 rop ){ return U64( make_function( "UnsignedMultiplyHigh", lop.expr, rop.expr ) ); }
  inline S64 SignedMultiplyHigh( S64 lop, S64 rop ) { return S64( make_function( "SignedMultiplyHigh", lop.expr, rop.expr ) ); }
  inline U64 RotL32( U64 const& src, U8 const& sh ) { return U64( make_function( "RotL32", src.expr, sh.expr ) ); }
}

#endif // ARCH_HH
