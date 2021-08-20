/*
 *  Copyright (c) 2017-2018,
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
 
#include <unisim/component/cxx/processor/arm/disasm.hh>
#include <unisim/component/cxx/processor/arm/psr.hh>
#include <unisim/util/symbolic/ccode/ccode.hh>
#include <unisim/util/symbolic/symbolic.hh>
#include <top_arm32.tcc>
#include <top_thumb.tcc>

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <cstdio>

struct pphex
{
  pphex( unsigned _bytes, uint64_t _value ) : value(_value), bytes(_bytes) {} uint64_t value; unsigned bytes;
  friend std::ostream& operator << ( std::ostream& sink, pphex const& _ )
  {
    sink << "0x" << std::hex << std::setw(_.bytes*2) << std::setfill('0') << _.value << std::dec;
    return sink;
  }
};

#define CP15ENCODE( CRN, OPC1, CRM, OPC2 ) ((OPC1 << 12) | (CRN << 8) | (CRM << 4) | (OPC2 << 0))

struct Processor
{
  //   =====================================================================
  //   =                           Configuration                           =
  //   =====================================================================
    
  struct Config
  {
    // Following a standard armv7-a configuration
    static uint32_t const model = unisim::component::cxx::processor::arm::ARMEMU;
    static bool const     insns4T = true;
    static bool const     insns5E = true;
    static bool const     insns5J = true;
    static bool const     insns5T = true;
    static bool const     insns6  = true;
    static bool const     insnsRM = false;
    static bool const     insnsT2 = true;
    static bool const     insns7  = true;
  };
    
  //   =====================================================================
  //   =                             Data Types                            =
  //   =====================================================================
    
  typedef unisim::util::symbolic::SmartValue<double>   F64;
  typedef unisim::util::symbolic::SmartValue<float>    F32;
  typedef unisim::util::symbolic::SmartValue<bool>     BOOL;
  typedef unisim::util::symbolic::SmartValue<uint8_t>  U8;
  typedef unisim::util::symbolic::SmartValue<uint16_t> U16;
  typedef unisim::util::symbolic::SmartValue<uint32_t> U32;
  typedef unisim::util::symbolic::SmartValue<uint64_t> U64;
  typedef unisim::util::symbolic::SmartValue<int8_t>   S8;
  typedef unisim::util::symbolic::SmartValue<int16_t>  S16;
  typedef unisim::util::symbolic::SmartValue<int32_t>  S32;
  typedef unisim::util::symbolic::SmartValue<int64_t>  S64;
    
  typedef unisim::util::symbolic::FP                   FP;
  typedef unisim::util::symbolic::Expr                 Expr;
  typedef unisim::util::symbolic::ExprNode             ExprNode;
  typedef unisim::util::symbolic::ScalarType           ScalarType;
  typedef unisim::util::symbolic::ccode::ActionNode    ActionNode;
  typedef unisim::util::symbolic::ccode::CNode         CNode;
  typedef unisim::util::symbolic::ccode::Update        Update;
  typedef unisim::util::symbolic::ccode::CCode         CCode;
  typedef unisim::util::symbolic::ccode::SrcMgr        SrcMgr;

  struct RegReadBase : public CNode
  {
    RegReadBase() {}
    
    virtual char const* GetRegName() const = 0;
    
    virtual void translate( SrcMgr& srcmgr, CCode& ccode ) const { srcmgr << '$' << GetRegName(); }
    virtual void Repr( std::ostream& sink ) const { sink << GetRegName(); }
    
    virtual unsigned SubCount() const { return 0; }
  };

  template <typename RID>
  struct RegRead : public RegReadBase
  {
    typedef RegRead<RID> this_type;
    RegRead( RID _id, ScalarType::id_t _tid ) : RegReadBase(), tid(_tid), id(_id) {}
    virtual this_type* Mutate() const { return new this_type( *this ); }
    virtual ScalarType::id_t GetType() const { return tid; }
    virtual char const* GetRegName() const { return id.c_str(); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<this_type const&>( rhs ) ); }
    int compare( this_type const& rhs ) const
    {
      if (int delta = int(tid) - int(rhs.tid)) return delta;
      if (int delta = id.cmp( rhs.id )) return delta;
      return RegReadBase::compare( rhs );
    }

    ScalarType::id_t tid;
    RID id;
  };

  template <typename RID> static Expr newRegRead( RID id, ScalarType::id_t tp ) { return new RegRead<RID>( id, tp ); }

  struct ForeignRegister : public RegReadBase
  {
    ForeignRegister( uint8_t _mode, unsigned _idx )
      : RegReadBase(), name(), idx(_idx), mode(_mode)
    {
      if (mode == SYSTEM_MODE) mode = USER_MODE;
      std::ostringstream buf;
      buf << (RegID("r0") + idx).c_str() << '_' << mode_ident();
      strncpy(&name[0],buf.str().c_str(),sizeof(name)-1);
    }
    virtual ForeignRegister* Mutate() const { return new ForeignRegister( *this ); }
    virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
    char const* mode_ident() const
    {
      switch (mode)
        {
        case USER_MODE: return "usr";
        case FIQ_MODE: return "fiq";
        case IRQ_MODE: return "irq";
        case SUPERVISOR_MODE: return "svc";
        case MONITOR_MODE: return "mon";
        case ABORT_MODE: return "abt";
        case HYPERVISOR_MODE: return "hyp";
        case UNDEFINED_MODE: return "und";
        }
      throw 0;
      return "";
    }

    virtual char const* GetRegName() const { return &name[0]; }
    
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<ForeignRegister const&>( rhs ) ); }
    int compare( ForeignRegister const& rhs ) const
    {
      if (int delta = int(mode) - int(rhs.mode)) return delta;
      if (int delta = int(idx) - int(rhs.idx)) return delta;
      return RegReadBase::compare( rhs );
    }
    
    char name[8];
    unsigned idx;
    uint8_t mode;
  };
  
  struct RegWriteBase : public Update
  {
    RegWriteBase( Expr const& _value ) : value(_value) {}
      
    virtual char const* GetRegName() const = 0;
    virtual void Repr( std::ostream& sink ) const { sink << GetRegName() << " := "; value->Repr(sink); }
    virtual void translate( SrcMgr& srcmgr, CCode& ccode ) const { srcmgr << '$' << GetRegName() << " = " << ccode(value) << ";\n"; }
    virtual unsigned SubCount() const { return 1; }
    virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return value; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegWriteBase const&>( rhs ) ); }
    int compare( RegWriteBase const& rhs ) const { return 0; }
      
    Expr value;
  };

  template <typename RID>
  struct RegWrite : public RegWriteBase
  {
    typedef RegWrite<RID> this_type;
    RegWrite( RID _id, Expr const& _value ) : RegWriteBase(_value), id(_id) {}
    virtual this_type* Mutate() const { return new this_type( *this ); }
    virtual char const* GetRegName() const { return id.c_str(); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegWrite const&>( rhs ) ); }
    int compare( this_type const& rhs ) const
    {
      if (int delta = id.cmp( rhs.id )) return delta;
      return RegWriteBase::cmp( rhs );
    }

    RID id;
  };

  template <typename RID>
  static Expr newRegWrite( RID id, Expr const& value ) { return new RegWrite<RID>( id, value ); }
  
  struct Br : public RegWriteBase
  {
    enum type_t { Jump = 0, Call, Return } type;
    Br( Expr const& value, type_t bt ) : RegWriteBase( value ), type(bt) {}
  };
  
  struct PCWrite : public Br
  {
    PCWrite( Expr const& value, Br::type_t bt ) : Br( value, bt ) {}
    virtual void translate( SrcMgr& srcmgr, CCode& ccode ) const
    {
      srcmgr << "/* npc: " << ccode(value) << " */\n";
    }
    virtual PCWrite* Mutate() const { return new PCWrite( *this ); }
    virtual char const* GetRegName() const { return "pc"; }
  };

  struct Load : public CNode
  {
    Load( Expr const& _addr, unsigned _size, unsigned _alignment, bool _bigendian )
      : addr(_addr), size(_size), alignment(_alignment), bigendian(_bigendian)
    {}
    virtual Load* Mutate() const { return new Load( *this ); }
    virtual void Repr( std::ostream& sink ) const { sink << "Load(" << addr << ", " << size << ", " << alignment << ", " << bigendian << ")"; }
    virtual void translate( SrcMgr& srcmgr, CCode& ccode ) const
    {
      if (bigendian)
        throw 0;
      srcmgr << "(*(uint" << unisim::util::symbolic::ccode::dec(8 << size) << "_t*)(" << ccode(addr) << "))";
    }
    virtual ScalarType::id_t GetType() const { return unisim::util::symbolic::ScalarType::IntegerType( false, 8 << size ); }
    virtual unsigned SubCount() const { return 1; }
    virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return addr; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Load const&>( rhs ) ); }
    int compare( Load const& rhs ) const
    {
      if (int delta = CNode::compare( rhs )) return delta;
      if (int delta = int(size) - int(rhs.size)) return delta;
      if (int delta = int(alignment) - int(rhs.alignment)) return delta;
      return int(bigendian) - int(rhs.bigendian);
    }
    
    Expr addr;
    uint32_t size      :  4; // (log2) [1,2,4,8,...,32768] bytes
    uint32_t alignment :  4; // (log2) [1,2,4,8,...,32768] bytes
    uint32_t bigendian :  1; // 0=little-endian
    uint32_t reserved  : 23; // reserved
  };

  struct Store : public Update
  {
    Store( Expr const& _addr, Expr const& _value, unsigned _size, unsigned _alignment, bool _bigendian )
      : value(_value), addr(_addr), size(_size), alignment(_alignment), bigendian(_bigendian)
    {}
    virtual Store* Mutate() const { return new Store( *this ); }
    virtual void Repr( std::ostream& sink ) const { sink << "Store(" << addr << ", " << value << ", " << size << ", " << alignment << ", " << bigendian << ")"; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Store const&>( rhs ) ); }
    int compare( Store const& rhs ) const
    {
      if (int delta = int(size) - int(rhs.size)) return delta;
      if (int delta = int(alignment) - int(rhs.alignment)) return delta;
      return int(bigendian) - int(rhs.bigendian);
    }
    virtual unsigned SubCount() const { return 2; }
    virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return addr; case 1: return value; } return ExprNode::GetSub(idx); }
    virtual void translate( SrcMgr& srcmgr, CCode& ccode ) const
    {
      if (bigendian)
        throw 0;
      srcmgr << "(*(uint" << unisim::util::symbolic::ccode::dec( 8 << size ) << "_t*)(" << ccode(addr) << ")) = " << ccode(value) << ";\n";
    }
    
    Expr value, addr;
    uint32_t size      :  4; // (log2) [1,2,4,8,...,32768] bytes
    uint32_t alignment :  4; // (log2) [1,2,4,8,...,32768] bytes
    uint32_t bigendian :  1; // 0=little-endian
    uint32_t reserved  : 23; // reserved
  };
    
  struct ITCond {};
    
  struct Mode
  {
    Mode() {}
    bool     HasBR( unsigned index ) { return false; }
    bool     HasSPSR() { return false; }
    void     SetSPSR(U32 const& value) {}
    U32      GetSPSR() { return U32(); }
    void     Swap( Processor& ) {}
  };
    
  typedef std::map<std::pair<uint8_t,uint32_t>,Expr> ForeignRegisters;
  
  struct CP15Reg
  {
    virtual            ~CP15Reg() {}
    virtual unsigned    RequiredPL() { return 1; }
    virtual void        Write( Processor& proc, U32 const& value ) { proc.not_implemented(); }
    virtual U32         Read( Processor& proc ) { proc.not_implemented(); return U32(); }
    virtual char const* Describe() = 0;
  };

  //   =====================================================================
  //   =                      Construction/Destruction                     =
  //   =====================================================================
  
  struct StatusRegister
  {
    enum InstructionSet { Arm, Thumb, Jazelle, ThumbEE };
      
    typedef unisim::util::symbolic::Expr       Expr;
    StatusRegister()
      : iset(Arm)                               // Default is ARM instruction set
      , bigendian(false)                        // Default is Little Endian
      , mode(SUPERVISOR_MODE)                   // Default is SUPERVISOR_MODE
      , outitb(false)                           // Force status as if outside ITBlock
    {}

    bool IsThumb() const { return iset == Thumb; }
      
    InstructionSet iset;
    bool bigendian;
    uint8_t mode;
    bool outitb;
  };
  
  struct PSR : public StatusRegister
  {
    typedef unisim::component::cxx::processor::arm::RegisterField<31,1> NRF; /* Negative Integer Condition Flag */
    typedef unisim::component::cxx::processor::arm::RegisterField<30,1> ZRF; /* Zero     Integer Condition Flag */
    typedef unisim::component::cxx::processor::arm::RegisterField<29,1> CRF; /* Carry    Integer Condition Flag */
    typedef unisim::component::cxx::processor::arm::RegisterField<28,1> VRF; /* Overflow Integer Condition Flag */
    //typedef unisim::component::cxx::processor::arm::RegisterField<27,1> QRF; /* Cumulative saturation flag */
      
    typedef unisim::component::cxx::processor::arm::RegisterField<28,4> NZCVRF; /* Grouped Integer Condition Flags */
      
      
    typedef unisim::component::cxx::processor::arm::RegisterField<24,1> JRF; /* Jazelle execution state bit */
    typedef unisim::component::cxx::processor::arm::RegisterField< 9,1> ERF; /* Endianness execution state */
    typedef unisim::component::cxx::processor::arm::RegisterField< 5,1> TRF; /* Thumb execution state bit */
      
    typedef unisim::component::cxx::processor::arm::RegisterField< 0,5> MRF; /* Mode field */
      
    typedef unisim::component::cxx::processor::arm::RegisterField<10,6> ITHIRF;
    typedef unisim::component::cxx::processor::arm::RegisterField<25,2> ITLORF;
      
    typedef unisim::component::cxx::processor::arm::RegisterField< 0,32> ALLRF;
      
    static uint32_t const bg_mask = 0x08ff01c0; /* Q, 23-20, GE[3:0], A, I, F, are not handled for now */

    PSR( PSR const& ) = delete;
    PSR( PSR&& ) = delete;
    
    PSR( Processor& p, StatusRegister const& ref )
      : StatusRegister(ref)
      , proc( p )
      , n(newRegRead(RegID("n"), ScalarType::BOOL))
      , z(newRegRead(RegID("z"), ScalarType::BOOL))
      , c(newRegRead(RegID("c"), ScalarType::BOOL))
      , v(newRegRead(RegID("v"), ScalarType::BOOL))
      , itstate(ref.outitb ? U8(0).expr : newRegRead(RegID("itstate"), ScalarType::U8))
      , bg(newRegRead(RegID("cpsr"), ScalarType::U32))
    {}
    
    bool   GetJ() const { return (iset == Jazelle) or (iset == ThumbEE); }
    bool   GetT() const { return (iset ==   Thumb) or (iset == ThumbEE); }

    template <typename RF>
    void   Set( RF const& _, U32 const& value )
    {
      unisim::util::symbolic::StaticAssert<(RF::pos > 31) or ((RF::pos + RF::size) <= 28)>::check(); // NZCV
      unisim::util::symbolic::StaticAssert<(RF::pos > 26) or ((RF::pos + RF::size) <= 24)>::check(); // ITLO, J
      unisim::util::symbolic::StaticAssert<(RF::pos > 15) or ((RF::pos + RF::size) <=  9)>::check(); // ITHI, E
      unisim::util::symbolic::StaticAssert<(RF::pos >  5) or ((RF::pos + RF::size) <=  0)>::check(); // T, MODE
        
      return _.Set( bg, value );
    }
      
    template <typename RF>
    U32    Get( RF const& _ )
    {
      unisim::util::symbolic::StaticAssert<(RF::pos > 31) or ((RF::pos + RF::size) <= 28)>::check(); // NZCV
      unisim::util::symbolic::StaticAssert<(RF::pos > 26) or ((RF::pos + RF::size) <= 24)>::check(); // ITLO, J
      unisim::util::symbolic::StaticAssert<(RF::pos > 15) or ((RF::pos + RF::size) <=  9)>::check(); // ITHI, E
      unisim::util::symbolic::StaticAssert<(RF::pos >  5) or ((RF::pos + RF::size) <=  0)>::check(); // T, MODE
        
      return _.Get( bg );
    }
      
    void   SetBits( U32 const& bits, uint32_t mask );
    U32    GetBits();
    
    void   Set( NRF const& _, BOOL const& value ) { n = value.expr; }
    void   Set( ZRF const& _, BOOL const& value ) { z = value.expr; }
    void   Set( CRF const& _, BOOL const& value ) { c = value.expr; }
    void   Set( VRF const& _, BOOL const& value ) { v = value.expr; }
    void   Set( ERF const& _, U32 const& value ) { if (proc.Test(value != U32(bigendian))) proc.UnpredictableInsnBehaviour(); }
    void   Set( NZCVRF const& _, U32 const& value );
      
    void   SetITState( uint8_t init_val ) { itstate = U8(init_val); }
    BOOL   InITBlock() const { return (itstate & U8(0b1111)) != U8(0); }
      
    U32    Get( NRF const& _ ) { return U32(BOOL(n)); }
    U32    Get( ZRF const& _ ) { return U32(BOOL(z)); }
    U32    Get( CRF const& _ ) { return U32(BOOL(c)); }
    U32    Get( VRF const& _ ) { return U32(BOOL(v)); }
      
    /* ISetState */
    U32    Get( JRF const& _ ) { return U32(GetJ()); }
    U32    Get( TRF const& _ ) { return U32(GetT()); }
      
    /* Endianness */
    U32    Get( ERF const& _ ) { return U32(bigendian); }
    U32    Get( MRF const& _ ) { return U32(mode); }
    // U32 Get( ALL const& _ ) { return (U32(BOOL(n)) << 31) | (U32(BOOL(z)) << 30) | (U32(BOOL(c)) << 29) | (U32(BOOL(v)) << 28) | bg; }
      
    Processor& proc;
    Expr n, z, c, v; /* TODO: should handle q */
    U8 itstate;
    U32 bg;
  };

  Processor( Processor const& ) = delete;
  Processor( Processor&& ) = delete;

  Processor( StatusRegister const& ref_psr )
    : path(0)
    , reg_values()
    , next_insn_addr()
    , branch_type()
    , cpsr( *this, ref_psr )
    , spsr( newRegRead(RegID("spsr"), ScalarType::U32) )
    , sregs()
    , FPSCR( newRegRead(RegID("fpscr"), ScalarType::U32) )
    , FPEXC( newRegRead(RegID("fpexc"), ScalarType::U32) )
    , stores()
    , unpredictable(false)
    , is_it_assigned(false)
    , mode()
    , foreign_registers()
  {
    // GPR regs
    for (unsigned reg = 0; reg < 15; ++reg)
      reg_values[reg] = U32( newRegRead( RegID("r0") + reg, ScalarType::U32 ) );
      
    // Special registers
    for (SRegID reg; reg.next();)
      sregs[reg.idx()] = U32( newRegRead( reg, ScalarType::U32 ) );
  }

  struct AssertFalse : public CNode
  {
    AssertFalse() {}
    virtual AssertFalse* Mutate() const { return new AssertFalse( *this ); }
    virtual ScalarType::id_t GetType() const { return ScalarType::VOID; }
    virtual unsigned SubCount() const { return 0; }
    virtual void Repr( std::ostream& sink ) const { sink << "assert (false)"; }
    virtual void translate( SrcMgr& srcmgr, CCode& ccode ) const { throw 0; }
  };
    
  bool close( Processor const& ref )
  {
    bool complete = path->close();
    path->updates.insert( Expr( new PCWrite( next_insn_addr.expr, branch_type ) ) );
    if (unpredictable)
      {
        path->updates.insert( Expr( new AssertFalse() ) );
        return complete;
      }
    if (cpsr.n != ref.cpsr.n)
      path->updates.insert( newRegWrite( RegID("n"), cpsr.n ) );
    if (cpsr.z != ref.cpsr.z)
      path->updates.insert( newRegWrite( RegID("z"), cpsr.z ) );
    if (cpsr.c != ref.cpsr.c)
      path->updates.insert( newRegWrite( RegID("c"), cpsr.c ) );
    if (cpsr.v != ref.cpsr.v)
      path->updates.insert( newRegWrite( RegID("v"), cpsr.v ) );
    if (cpsr.itstate.expr != ref.cpsr.itstate.expr)
      path->updates.insert( newRegWrite( RegID("itstate"), cpsr.itstate.expr ) );
    if (cpsr.bg.expr != ref.cpsr.bg.expr)
      path->updates.insert( newRegWrite( RegID("cpsr"), cpsr.bg.expr ) );
    if (spsr.expr != ref.spsr.expr)
      path->updates.insert( newRegWrite( RegID("spsr"), spsr.expr ) );
    for (SRegID reg; reg.next();)
      if (sregs[reg.idx()].expr != ref.sregs[reg.idx()].expr)
        path->updates.insert( newRegWrite( reg, sregs[reg.idx()].expr ) );
    if (FPSCR.expr != ref.FPSCR.expr)
      path->updates.insert( newRegWrite( RegID("fpscr"), FPSCR.expr ) );
    if (FPEXC.expr != ref.FPEXC.expr)
      path->updates.insert( newRegWrite( RegID("fpexc"), FPEXC.expr ) );
    for (unsigned reg = 0; reg < 15; ++reg) {
      if (reg_values[reg].expr != ref.reg_values[reg].expr)
        path->updates.insert( newRegWrite( RegID("r0") + reg, reg_values[reg].expr ) );
    }
    for (ForeignRegisters::iterator itr = foreign_registers.begin(), end = foreign_registers.end(); itr != end; ++itr)
      {
        ForeignRegister ref(itr->first.first, itr->first.second);
        ref.Retain(); // Prevent deletion of this static instance
        Expr xref( new ForeignRegister(itr->first.first, itr->first.second) );
        if (itr->second == Expr(&ref)) continue;
        std::ostringstream buf;
        ref.Repr( buf );
        path->updates.insert( newRegWrite( RegID(buf.str().c_str()), itr->second ) );
      }
    for (std::set<Expr>::const_iterator itr = stores.begin(), end = stores.end(); itr != end; ++itr)
      path->updates.insert( *itr );
    return complete;
  }
  
  //   =====================================================================
  //   =                 Internal Instruction Control Flow                 =
  //   =====================================================================
  
  void UnpredictableInsnBehaviour() { unpredictable = true; }
  
  template <typename OP>
  void UndefinedInstruction( OP* op ) { not_implemented(); }

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

    Expr cexp( BOOL(cond).expr );
    if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
      return cnode->Get( bool() );

    return concretize( cexp );
  }
    
  void FPTrap( unsigned exc )
  {
    throw std::logic_error("unimplemented");
  }
    
  void not_implemented() { throw std::logic_error( "not implemented" ); }

  //   =====================================================================
  //   =             General Purpose Registers access methods              =
  //   =====================================================================
    
  U32  GetGPR( uint32_t id ) { return reg_values[id]; }
  
  // TODO: interworking branches are not correctly handled
  void SetGPR_mem( uint32_t id, U32 const& value )
  {
    if (id != 15)
      reg_values[id] = value;
    else
      SetNIA( value, B_JMP );
  }
  void SetGPR( uint32_t id, U32 const& value ) {
    if (id != 15)
      reg_values[id] = value;
    else
      SetNIA( value, B_JMP );
  }
  
  void SetGPR_usr( uint32_t id, U32 const& value ) { /* Only work in system mode instruction */ not_implemented(); }
  U32  GetGPR_usr( uint32_t id ) { /* Only work in system mode instruction */ not_implemented(); return U32(); }
    
  U32  GetNIA() { return next_insn_addr; }
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_DBG, B_RFE };
  void SetNIA( U32 const& nia, branch_type_t bt )
  {
    next_insn_addr = nia;
    branch_type = (bt == B_CALL) ? Br::Call : (bt == B_RET) ? Br::Return : Br::Jump;
  }

  Expr& GetForeignRegister( uint8_t foreign_mode, uint32_t idx )
  {
    Expr& result = foreign_registers[std::make_pair( foreign_mode, idx )];
    if (not result.node)
      result = new ForeignRegister( foreign_mode, idx );
    return result;
  }
    
  U32  GetBankedRegister( uint8_t foreign_mode, uint32_t idx )
  {
    if ((cpsr.mode == foreign_mode) or
        (idx < 8) or
        (idx >= 15) or
        ((foreign_mode != FIQ_MODE) and (cpsr.mode != FIQ_MODE) and (idx < 13))
        )
      return GetGPR( idx );
    return U32( GetForeignRegister( foreign_mode, idx ) );
  }
    
  void SetBankedRegister( uint8_t foreign_mode, uint32_t idx, U32 value )
  {
    if ((cpsr.mode == foreign_mode) or
        (idx < 8) or
        (idx >= 15) or
        ((foreign_mode != FIQ_MODE) and (cpsr.mode != FIQ_MODE) and (idx < 13))
        )
      return SetGPR( idx, value );
    GetForeignRegister( foreign_mode, idx ) = value.expr;
  }
    
  //   =====================================================================
  //   =              Special/System Registers access methods              =
  //   =====================================================================

  PSR& CPSR() { return cpsr; }
  
  U32  GetCPSR()                                 { return cpsr.GetBits(); }
  void SetCPSR( U32 const& bits, uint32_t mask ) { cpsr.SetBits( bits, mask ); }
    
  U32& SPSR() { not_implemented(); static U32 spsr_dummy; return spsr_dummy; }
  
  ITCond itcond() const { return ITCond(); }
  bool itblock() { return Test(cpsr.InITBlock()); }
  
  void ITSetState( uint32_t cond, uint32_t mask )
  {
    cpsr.SetITState( (cond << 4) | mask );
    is_it_assigned = true;
  }
  
  void ITAdvance()
  {
    if (is_it_assigned)
      is_it_assigned = false;
    else if (itblock())
      {
        U8 itstate( cpsr.itstate );
        itstate = (Test((itstate & U8(7)) != U8(0))) ? ((itstate & U8(-32)) | ((itstate << 1) & U8(31))) : U8(0);
        cpsr.itstate = itstate;
      }
  }
  
  Mode&  CurrentMode() { /* not_implemented(); */ return mode; }
  Mode&  GetMode(uint8_t) { not_implemented(); return mode; }
  
  virtual CP15Reg& CP15GetRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 );
  
  U32         CP15ReadRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
  { return CP15GetRegister( crn, opcode1, crm, opcode2 ).Read( *this ); }
  void        CP15WriteRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2, U32 const& value )
  { CP15GetRegister( crn, opcode1, crm, opcode2 ).Write( *this, value ); }
  char const* CP15DescribeRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
  { return CP15GetRegister( crn, opcode1, crm, opcode2 ).Describe(); }

  //   ====================================================================
  //   =         Vector and Floating-point Registers access methods       =
  //   ====================================================================

  U32 RoundTowardsZeroFPSCR() const
  {
    U32 fpscr = FPSCR;
    unisim::component::cxx::processor::arm::RMode.Set( fpscr, U32(unisim::component::cxx::processor::arm::RoundTowardsZero) );
    return fpscr;
  }
    
  U32 RoundToNearestFPSCR() const
  {
    U32 fpscr = FPSCR;
    unisim::component::cxx::processor::arm::RMode.Set( fpscr, U32(unisim::component::cxx::processor::arm::RoundToNearest) );
    return fpscr;
  }
    
  // U32 StandardValuedFPSCR() const   { return AHP.Mask( FPSCR ) | 0x03000000; }
    
  U32  GetVU32( unsigned idx ) { return U32(); }
  void SetVU32( unsigned idx, U32 val ) {}
  U64  GetVU64( unsigned idx ) { return U64(); }
  void SetVU64( unsigned idx, U64 val ) {}
  F32  GetVSR( unsigned idx ) { return F32(); }
  void SetVSR( unsigned idx, F32 val ) {}
  F64  GetVDR( unsigned idx ) { return F64(); }
  void SetVDR( unsigned idx, F64 val ) {}
    
  U32  GetVSU( unsigned idx ) { return U32(); }
  void SetVSU( unsigned idx, U32 val ) {}
  U64  GetVDU( unsigned idx ) { return U64(); }
  void SetVDU( unsigned idx, U64 val ) {}
  
  //   =====================================================================
  //   =                      Control Transfer methods                     =
  //   =====================================================================

  void BranchExchange( U32 const& target, branch_type_t branch_type ) { SetNIA( target, branch_type ); }
  void Branch( U32 const& target, branch_type_t branch_type ) { SetNIA( target, branch_type ); }
    
  void WaitForInterrupt() { not_implemented(); }
  void SWI( uint32_t imm ) { not_implemented(); }
  void BKPT( uint32_t imm ) { not_implemented(); }
  void CallSupervisor( uint32_t imm ) { not_implemented(); }
  bool IntegerZeroDivide( BOOL const& condition ) { return false; }
  
  //   =====================================================================
  //   =                       Memory access methods                       =
  //   =====================================================================
  
  U32  MemURead32( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 2, 0, false ) ) ); }
  U16  MemURead16( U32 const& addr ) { return U16( Expr( new Load( addr.expr, 1, 0, false ) ) ); }
  U32  MemRead32( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 2, 2, false ) ) ); }
  U16  MemRead16( U32 const& addr ) { return U16( Expr( new Load( addr.expr, 1, 1, false ) ) ); }
  U8   MemRead8( U32 const& addr ) { return U8( Expr( new Load( addr.expr, 0, 0, false ) ) ); }
  
  void MemUWrite32( U32 const& addr, U32 const& value ) { stores.insert( new Store( addr.expr, value.expr, 2, 0, false ) ); }
  void MemUWrite16( U32 const& addr, U16 const& value ) { stores.insert( new Store( addr.expr, value.expr, 1, 0, false ) ); }
  void MemWrite32( U32 const& addr, U32 const& value ) { stores.insert( new Store( addr.expr, value.expr, 2, 2, false ) ); }
  void MemWrite16( U32 const& addr, U16 const& value ) { stores.insert( new Store( addr.expr, value.expr, 1, 1, false ) ); }
  void MemWrite8( U32 const& addr, U8 const& value ) { stores.insert( new Store( addr.expr, value.expr, 0, 0, false ) ); }
    
  void SetExclusiveMonitors( U32 const& address, unsigned size ) { std::cerr << "SetExclusiveMonitors\n"; }
  bool ExclusiveMonitorsPass( U32 const& address, unsigned size ) { std::cerr << "ExclusiveMonitorsPass\n"; return true; }
  void ClearExclusiveLocal() { std::cerr << "ClearExclusiveMonitors\n"; }
  
  //   =====================================================================
  //   =                         Processor Storage                         =
  //   =====================================================================
  
  static const unsigned PC_reg = 15;
  static const unsigned LR_reg = 14;
  static const unsigned SP_reg = 13;

  /* masks for the different running modes */
  static uint32_t const USER_MODE = 0b10000;
  static uint32_t const FIQ_MODE = 0b10001;
  static uint32_t const IRQ_MODE = 0b10010;
  static uint32_t const SUPERVISOR_MODE = 0b10011;
  static uint32_t const MONITOR_MODE = 0b10110;
  static uint32_t const ABORT_MODE = 0b10111;
  static uint32_t const HYPERVISOR_MODE = 0b11010;
  static uint32_t const UNDEFINED_MODE = 0b11011;
  static uint32_t const SYSTEM_MODE = 0b11111;

  /* values of the different condition codes */
  static uint32_t const COND_EQ = 0x00;
  static uint32_t const COND_NE = 0x01;
  static uint32_t const COND_CS_HS = 0x02;
  static uint32_t const COND_CC_LO = 0x03;
  static uint32_t const COND_MI = 0x04;
  static uint32_t const COND_PL = 0x05;
  static uint32_t const COND_VS = 0x06;
  static uint32_t const COND_VC = 0x07;
  static uint32_t const COND_HI = 0x08;
  static uint32_t const COND_LS = 0x09;
  static uint32_t const COND_GE = 0x0a;
  static uint32_t const COND_LT = 0x0b;
  static uint32_t const COND_GT = 0x0c;
  static uint32_t const COND_LE = 0x0d;
  static uint32_t const COND_AL = 0x0e;
    
  /* mask for valid bits in processor control and status registers */
  static uint32_t const PSR_UNALLOC_MASK = 0x00f00000;

  struct SRegID : public unisim::util::identifier::Identifier<SRegID>
  {
    enum Code {
      SCTLR, ACTLR,
      CTR, MPIDR,
      ID_PFR0, CCSIDR, CLIDR, CSSELR,
      CPACR, NSACR,
      TTBR0, TTBR1, TTBCR,
      DACR,
      DFSR, IFSR, DFAR, IFAR,
      ICIALLUIS, BPIALLIS,
      ICIALLU, ICIMVAU, BPIALL,
      DCIMVAC, DCISW, DCCMVAC, DCCSW, DCCMVAU, DCCIMVAC,
      TLBIALLIS, TLBIALL, TLBIASID,
      VBAR,
      CONTEXTIDR,
      DIAGCR, CFGBAR, end
    } code;

    char const* c_str() const
    {
      switch (code)
        {
        case      SCTLR: return "sctlr";
        case      ACTLR: return "actlr";
        case        CTR: return "ctr";
        case      MPIDR: return "mpidr";
        case    ID_PFR0: return "id_pfr0";
        case     CCSIDR: return "ccsidr";
        case      CLIDR: return "clidr";
        case     CSSELR: return "csselr";
        case      CPACR: return "cpacr";
        case      NSACR: return "nsacr";
        case      TTBR0: return "ttbr0";
        case      TTBR1: return "ttbr1";
        case      TTBCR: return "ttbcr";
        case       DACR: return "dacr";
        case       DFSR: return "dfsr";
        case       IFSR: return "ifsr";
        case       DFAR: return "dfar";
        case       IFAR: return "ifar";
        case  ICIALLUIS: return "icialluis";
        case   BPIALLIS: return "bpiallis";
        case    ICIALLU: return "iciallu";
        case    ICIMVAU: return "icimvau";
        case     BPIALL: return "bpiall";
        case    DCIMVAC: return "dcimvac";
        case      DCISW: return "dcisw";
        case    DCCMVAC: return "dccmvac";
        case      DCCSW: return "dccsw";
        case    DCCMVAU: return "dccmvau";
        case   DCCIMVAC: return "dccimvac";
        case  TLBIALLIS: return "tlbiallis";
        case    TLBIALL: return "tlbiall";
        case   TLBIASID: return "tlbiasid";
        case       VBAR: return "vbar";
        case CONTEXTIDR: return "contextidr";
        case     DIAGCR: return "diagcr";
        case     CFGBAR: return "cfgbar";
        case end:        break;
        }
      return "NA";
    }

    SRegID() : code(end) {}
    SRegID( Code _code ) : code(_code) {}
    SRegID( char const* _code ) : code(end) { init(_code); }
  };
  
  U32& SReg( SRegID reg )
  {
    if (reg.code == SRegID::end)
      throw 0;
    return sregs[reg.idx()];
  }
    
  struct RegID : public unisim::util::identifier::Identifier<RegID>
  {
    enum Code
      {
        NA = 0,
        r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr,
        n, z, c, v, itstate, // q, ge0, ge1, ge2, ge3,
        cpsr, spsr,
        fpscr, fpexc,
        r8_fiq,
        r9_fiq,
        sl_fiq,
        fp_fiq,
        ip_fiq,
        sp_fiq,
        lr_fiq,
        r8_usr,
        r9_usr,
        sl_usr,
        fp_usr,
        ip_usr,
        sp_usr,
        lr_usr,
        sp_irq,
        sp_svc,
        sp_abt,
        sp_hyp,
        sp_und,
        lr_irq,
        lr_svc,
        lr_abt,
        lr_hyp,
        lr_und,
        end
      } code;

    char const* c_str() const
    {
      switch (code)
        {
        case         r0: return "r0";
        case         r1: return "r1";
        case         r2: return "r2";
        case         r3: return "r3";
        case         r4: return "r4";
        case         r5: return "r5";
        case         r6: return "r6";
        case         r7: return "r7";
        case         r8: return "r8";
        case         r9: return "r9";
        case         sl: return "sl";
        case         fp: return "fp";
        case         ip: return "ip";
        case         sp: return "sp";
        case         lr: return "lr";
        case          n: return "n";
        case          z: return "z";
        case          c: return "c";
        case          v: return "v";
        case    itstate: return "itstate";
        case       cpsr: return "cpsr";
        case       spsr: return "spsr";
        case      fpscr: return "fpscr";
        case      fpexc: return "fpexc";
        case     r8_fiq: return "r8_fiq";
        case     r9_fiq: return "r9_fiq";
        case     sl_fiq: return "sl_fiq";
        case     fp_fiq: return "fp_fiq";
        case     ip_fiq: return "ip_fiq";
        case     sp_fiq: return "sp_fiq";
        case     lr_fiq: return "lr_fiq";
        case     r8_usr: return "r8_usr";
        case     r9_usr: return "r9_usr";
        case     sl_usr: return "sl_usr";
        case     fp_usr: return "fp_usr";
        case     ip_usr: return "ip_usr";
        case     sp_usr: return "sp_usr";
        case     lr_usr: return "lr_usr";
        case     sp_irq: return "sp_irq";
        case     sp_svc: return "sp_svc";
        case     sp_abt: return "sp_abt";
        case     sp_hyp: return "sp_hyp";
        case     sp_und: return "sp_und";
        case     lr_irq: return "lr_irq";
        case     lr_svc: return "lr_svc";
        case     lr_abt: return "lr_abt";
        case     lr_hyp: return "lr_hyp";
        case     lr_und: return "lr_und";
        case         NA:
        case        end: break;
        }
      return "NA";
    }
      
    RegID() : code(end) {}
    RegID( Code _code ) : code(_code) {}
    RegID( char const* _code ) : code(end) { init( _code ); }
  };
    
  ActionNode*      path;
  U32              reg_values[16];
  U32              next_insn_addr;
  Br::type_t       branch_type;
  PSR              cpsr;
  U32              spsr;
  U32              sregs[SRegID::end];
  U32              FPSCR, FPEXC;
  std::set<Expr>   stores;
  bool             unpredictable;
  bool             is_it_assigned; /* determines wether current instruction is an IT one. */
  Mode             mode;
  ForeignRegisters foreign_registers;
};

bool CheckCondition( Processor& state, unsigned cond )
{
  Processor::BOOL N = state.cpsr.n, Z = state.cpsr.z, C = state.cpsr.c, V = state.cpsr.v, result(false);
  
  switch (cond)
    {
    case  0: result =                   Z; break; // eq; equal
    case  1: result =               not Z; break; // ne; not equal
    case  2: result =                   C; break; // cs/hs; unsigned higher or same
    case  3: result =               not C; break; // cc/lo; unsigned lower
    case  4: result =                   N; break; // mi; negative
    case  5: result =               not N; break; // pl; positive or zero
    case  6: result =                   V; break; // vs; overflow set
    case  7: result =               not V; break; // vc; overflow clear
    case  8: result =    not (not C or Z); break; // hi; unsigned higher
    case  9: result =        (not C or Z); break; // ls; unsigned lower or same
    case 10: result =       not (N xor V); break; // ge; signed greater than or equal
    case 11: result =           (N xor V); break; // lt; signed less than
    case 12: result = not(Z or (N xor V)); break; // gt; signed greater than
    case 13: result =    (Z or (N xor V)); break; // le; signed less than or equal
    case 14: return true;                  break; // al; always
    default:                                      // nv; never (illegal)
      throw std::logic_error( "bad condition" );
    }

  return state.Test(result);
}

bool CheckCondition( Processor& state, Processor::ITCond const& cond )
{
  typedef Processor::BOOL BOOL;
  typedef Processor::U8   U8;
  BOOL N = state.cpsr.n, Z = state.cpsr.z, C = state.cpsr.c, V = state.cpsr.v;
  U8 cc = (state.cpsr.itstate >> 4);
  return state.Test(
    ((state.cpsr.itstate & U8(0b1111)) == U8(0)) or // unconditional
    ((cc == U8(0)) and (Z)) or // eq; equal
    ((cc == U8(1)) and (not Z)) or // ne; not equal
    ((cc == U8(2)) and (C)) or // cs/hs; unsigned higher or same
    ((cc == U8(3)) and (not C)) or // cc/lo; unsigned lower
    ((cc == U8(4)) and (N)) or // mi; negative
    ((cc == U8(5)) and (not N)) or // pl; positive or zero
    ((cc == U8(6)) and (V)) or // vs; overflow set
    ((cc == U8(7)) and (not V)) or // vc; overflow clear
    ((cc == U8(8)) and (not (not C or Z))) or // hi; unsigned higher
    ((cc == U8(9)) and ((not C or Z))) or // ls; unsigned lower or same
    ((cc == U8(10)) and (not (N xor V))) or // ge; signed greater than or equal
    ((cc == U8(11)) and ((N xor V))) or // lt; signed less than
    ((cc == U8(12)) and (not(Z or (N xor V)))) or // gt; signed greater than
    ((cc == U8(13)) and ((Z or (N xor V)))) or // le; signed less than or equal
    ((cc == U8(14)) and (unisim::util::symbolic::make_const( true ))));
}

void UpdateStatusSub( Processor& state, Processor::U32 const& res, Processor::U32 const& lhs, Processor::U32 const& rhs )
{
  typedef Processor::S32 S32;
  typedef Processor::U32 U32;
  Processor::BOOL neg = S32(res) < S32(0);
  state.cpsr.n = neg.expr;
  state.cpsr.z = ( res == U32(0) ).expr;
  state.cpsr.c = ( lhs >= rhs ).expr;
  state.cpsr.v = ( neg xor (S32(lhs) < S32(rhs)) ).expr;
}

void UpdateStatusSubWithBorrow( Processor& state, Processor::U32 const& res, Processor::U32 const& lhs, Processor::U32 const& rhs, Processor::U32 const& borrow )
{
  typedef Processor::S32 S32;
  typedef Processor::U32 U32;
  Processor::BOOL neg = S32(res) < S32(0);
  state.cpsr.n = neg.expr;
  state.cpsr.z = ( res == U32(0) ).expr;
  if (state.Test(borrow != U32(0)))
    {
      state.cpsr.c = ( lhs >  rhs ).expr;
      state.cpsr.v = ( neg xor (S32(lhs) <= S32(rhs)) ).expr;
    }
  else
    {
      state.cpsr.c = ( lhs >= rhs ).expr;
      state.cpsr.v = ( neg xor (S32(lhs) <  S32(rhs)) ).expr;
    }
}

void UpdateStatusAdd( Processor& state, Processor::U32 const& res, Processor::U32 const& lhs, Processor::U32 const& rhs )
{
  typedef Processor::S32 S32;
  typedef Processor::U32 U32;
  Processor::BOOL neg = S32(res) < S32(0);
  state.cpsr.n = neg.expr;
  state.cpsr.z = ( res == U32(0) ).expr;
  state.cpsr.c = ( lhs >  ~rhs ).expr;
  state.cpsr.v = ( neg xor (S32(lhs) <= S32(~rhs)) ).expr;
}

void UpdateStatusAddWithCarry( Processor& state, Processor::U32 const& res, Processor::U32 const& lhs, Processor::U32 const& rhs, Processor::U32 const& carry )
{
  typedef Processor::S32 S32;
  typedef Processor::U32 U32;
  Processor::BOOL neg = S32(res) < S32(0);
  state.cpsr.n = neg.expr;
  state.cpsr.z = ( res == U32(0) ).expr;
  if (state.Test(carry != U32(0)))
    {
      state.cpsr.c = ( lhs >= ~rhs ).expr;
      state.cpsr.v = ( neg xor (S32(lhs) <  S32(~rhs)) ).expr;
    }
  else
    {
      state.cpsr.c = ( lhs >  ~rhs ).expr;
      state.cpsr.v = ( neg xor (S32(lhs) <= S32(~rhs)) ).expr;
    }
}


struct ARMISA : public unisim::component::cxx::processor::arm::isa::arm32::Decoder<Processor>
{
  typedef unisim::component::cxx::processor::arm::isa::arm32::CodeType CodeType;
  typedef unisim::component::cxx::processor::arm::isa::arm32::Operation<Processor> Operation;
  static CodeType mkcode( uint32_t code ) { return CodeType( code ); }
  static bool const is_thumb = false;
};

struct THUMBISA : public unisim::component::cxx::processor::arm::isa::thumb::Decoder<Processor>
{
  typedef unisim::component::cxx::processor::arm::isa::thumb::CodeType CodeType;
  typedef unisim::component::cxx::processor::arm::isa::thumb::Operation<Processor> Operation;
  static CodeType mkcode( uint32_t code ) { return CodeType( code ); }
  static bool const is_thumb = true;
};

struct Translator
{
  typedef unisim::util::symbolic::ccode::ActionNode ActionNode;
  typedef Processor::StatusRegister StatusRegister;
  
  Translator( uint32_t _addr, std::vector<uint8_t> const& _code )
    : addr(_addr), code(_code), coderoot(new ActionNode)
  {}
  ~Translator() { delete coderoot; }
  
  template <class ISA>
  void
  extract( std::ostream& sink, ISA& isa )
  {
    sink << "/* address: " << pphex(4,addr) << " */\n";
  
    // Instruction decoding
    struct Instruction
    {
      typedef typename ISA::Operation Operation;
      Instruction(Instruction && rhs) noexcept
      {
        operation = rhs.operation;
        bytecount = rhs.bytecount;
        rhs.operation = 0;
      }
      Instruction(Instruction const& rhs) = delete;
      Instruction(ISA& isa, uint32_t addr, uint8_t const* codebuf)
        : operation(0), bytecount(0)
      {
        uint32_t code = 0;
        for (int byte = 4; --byte >= 0;)
          code = (code << 8) | codebuf[byte];
        
        operation = isa.NCDecode( addr, ISA::mkcode( code ) );
        unsigned bitlength = operation->GetLength();
        if ((bitlength != 32) and ((bitlength != 16) or not ISA::is_thumb))
          {
            delete operation;
            operation = 0;
            throw unisim::component::cxx::processor::arm::isa::Reject();
          }
        bytecount = bitlength/8;
      }
      ~Instruction() { delete operation; }
      Operation* operator -> () const { return operation; }
      
      Operation* operation;
      unsigned   bytecount;
    };

    std::vector<Instruction> instructions;
    
    Processor reference( status );
    
    for (uint32_t offset = 0, end = code.size(); offset < end;)
      {
        try
          { instructions.emplace_back( isa, addr + offset, &code[offset] ); }
        catch( unisim::component::cxx::processor::arm::isa::Reject const& )
          {
            sink << "/* Illegal instruction bytes:";
            for (int idx = 4; --idx >= 0;)
              sink << ' ' << std::hex << std::setw(2) << std::setfill('0') << unsigned(code[offset+idx]) << std::dec;
            sink << " */\n";
            return;
          }
        Instruction const& instruction = instructions.back();
        {
          // Disassemble
          uint32_t encoding = instruction->GetEncoding();
          if (instruction.bytecount == 2)
            encoding &= 0xffff;
          sink << "/* " << pphex(4,addr + offset) << " " << (instruction.bytecount == 2 ? "    " : "") << "["
               << pphex(instruction.bytecount, encoding) << "]:  ";
          try { instruction->disasm( reference, sink ); }
          catch (...) { sink << "(bad)"; }
          sink << " */\n";
        }
        offset += instruction.bytecount;
      }
    
    
    // Get actions
    try
      {
        bool is_thumb = status.IsThumb();
        for (bool end = false; not end;)
          {
            Processor state( status );
            state.path = coderoot;
            for (Instruction const& instruction : instructions)
              {
                // Fetch
                uint32_t insn_addr = instruction->GetAddr();
                state.SetNIA( Processor::U32(insn_addr + instruction.bytecount), Processor::B_JMP );
                state.reg_values[15] = Processor::U32(insn_addr + (is_thumb ? 4 : 8) );
                // Execute
                instruction->execute( state );
                if (is_thumb)
                  state.ITAdvance();
              }
            end = state.close( reference );
          }
        // coderoot->simplify();
        // coderoot->commit_stats();
      }
    catch (...)
      {
        sink << "(unimplemented)\n";
        return;
      }
  }

  void translate( std::ostream& ostr )
  {
    if      (status.iset == status.Arm)
      {
        ARMISA armisa;
        extract( ostr, armisa );
      }
    else if (status.iset == status.Thumb)
      {
        THUMBISA thumbisa;
        extract( ostr, thumbisa );
      }
    else
      throw 0;
    
    coderoot->simplify();
    
    coderoot->commit_stats();

    typedef unisim::util::symbolic::ccode::CCode CCode;
    typedef unisim::util::symbolic::ccode::SrcMgr SrcMgr;
    typedef unisim::util::symbolic::ccode::Update Update;
    typedef unisim::util::symbolic::ccode::Variable Variable;
    typedef unisim::util::symbolic::Expr Expr;

    CCode ccode;
    SrcMgr sink( ostr );

    
    struct Coder
    {
      Coder( SrcMgr& _srcmgr, CCode& _ccode ) : srcmgr(_srcmgr), ccode(_ccode) {}

      void generate(ActionNode* node)
      {
        // Ordering Sub Expressions by size of expressions (so that
        // smaller expressions are factorized in larger ones)
        struct CSE : public std::multimap<unsigned,Expr>
        {
          void Process( Expr const& expr ) { insert( std::make_pair( CountSubs( expr ), expr ) ); }
          unsigned CountSubs( Expr const& expr )
          {
            unsigned sum = 1;
            for (unsigned idx = 0, end = expr->SubCount(); idx < end; ++idx)
              sum += CountSubs( expr->GetSub(idx) );
            return sum;
          }
        } cse;

        for (std::map<Expr,unsigned>::const_iterator itr = node->sestats.begin(), end = node->sestats.end(); itr != end; ++itr)
          {
            // Check if reused and not already defined
            if ((itr->second > 1) and not ccode.tmps.count( itr->first ))
              cse.Process(itr->first);
          }

        for (std::multimap<unsigned,Expr>::const_iterator itr = cse.begin(), end = cse.end(); itr != end; ++itr)
          {
            ccode.make_temp( srcmgr, itr->second );
          }

        for (auto && update : node->updates)
          {
            Update const& ud = dynamic_cast<Update const&>( *update.node );
            
            for (unsigned idx = 0, end = ud.SubCount(); idx < end; ++idx)
              {
                Expr value = ud.GetSub(idx);
                
                if (value->AsConstNode() or ccode.tmps.count( value ))
                  continue;
                
                ccode.make_temp( srcmgr, value );
              }
          }

        if (node->cond.good())
          {
            std::map<Expr,Variable> saved_tmps( ccode.tmps );
            srcmgr << "if (" << ccode(node->cond) << ")\n";
            if (ActionNode* next = node->nexts[true])
              {
                srcmgr << "{\n";
                generate( next );
                srcmgr << "}\n";
                ccode.tmps = saved_tmps;
              }
            else
              throw 0;
            if (ActionNode* next = node->nexts[false])
              {
                srcmgr << "else\n{\n";
                generate( next );
                srcmgr << "}\n";
                ccode.tmps = saved_tmps;
              }
          }

        // Commit architectural updates
        typedef Processor::PCWrite PCWrite;
        PCWrite const*  npc = 0;
        for (Expr const& update : node->updates)
          {
            if (PCWrite const* pcw = dynamic_cast<PCWrite const*>( update.node ))
              { npc = pcw; continue; }
            srcmgr << ccode(update);
          }
        if (npc)
          srcmgr << ccode(Expr(npc));
      }
      
      SrcMgr& srcmgr;
      CCode&   ccode;
    } coder(sink, ccode);
    
    coder.generate( coderoot );
  }

  Processor::StatusRegister status;
  uint32_t                    addr;
  std::vector<uint8_t> const& code;
  ActionNode*                 coderoot;
};
  
uint32_t getu32( uint32_t& res, char const* arg )
{
  char *end;
  uint32_t tmp = strtoul( arg, &end, 0 );
  if ((*arg == '\0') or (*end != '\0'))
    return false;
  res = tmp;
  return true;
}

char const* usage()
{
  return
    "usage: <program> arm|thumb <address> <encoding>\n";
}

int
main( int argc, char** argv )
{
  if (argc != 4)
    {
      std::cerr << "Wrong number of arguments.\n" << usage();
      return 1;
    }

  uint32_t addr;
  if (not getu32(addr, argv[2]))
    {
      std::cerr << "<addr> should be a 32 bit numeric value pointing at start of code.";
      return 1;
    }
  struct : public std::vector<uint8_t>
  {
    void fromhex( char const* s, uintptr_t pos = 0 )
    {
      uint8_t byte = 0;
      for (unsigned idx = 0; idx < 2; ++idx)
        {
          byte <<= 4;
          switch (char ch = *s++)
            {
            default:
              resize( pos ); return;
            case '0': case '1': case '2': case '3': case '4': case '6': case '5': case '7': case '8': case '9':
              byte |= ch - '0'; break;
            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
              byte |= ch - 'a' + 10; break;
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
              byte |= ch - 'A' + 10; break;
            }
        }
      fromhex( s, pos+1 );
      at(pos) = byte;
    }
      
  } code;

  code.fromhex( argv[3] );

  if (not code.size())
    {
      std::cerr << "<code> should be an hex encoded instruction stream (as found in memory).\n" << usage();
      return 1;
    }

  Translator actset( addr, code );
  Translator::StatusRegister& status = actset.status;
  
  std::string exec_flags(argv[1]);
  exec_flags += ',';
  for (std::string::iterator cur = exec_flags.begin(), end = exec_flags.end(), nxt; cur != end; cur = nxt+1 )
    {
      nxt = std::find( cur, end, ',' );
      std::string flag( cur, nxt );
      if (flag.size() == 0)
        continue;
      if      (flag == "arm")        { status.iset = status.Arm; }
      else if (flag == "thumb")      { status.iset = status.Thumb; }
      else if (flag == "little")     { status.bigendian = false; }
      else if (flag == "big")        { status.bigendian = true; }
      else if (flag == "user")       { status.mode = Processor::USER_MODE; }
      else if (flag == "fiq")        { status.mode = Processor::FIQ_MODE; }
      else if (flag == "irq")        { status.mode = Processor::IRQ_MODE; }
      else if (flag == "supervisor") { status.mode = Processor::SUPERVISOR_MODE; }
      else if (flag == "monitor")    { status.mode = Processor::MONITOR_MODE; }
      else if (flag == "abort")      { status.mode = Processor::ABORT_MODE; }
      else if (flag == "hypervisor") { status.mode = Processor::HYPERVISOR_MODE; }
      else if (flag == "undefined")  { status.mode = Processor::UNDEFINED_MODE; }
      else if (flag == "system")     { status.mode = Processor::SYSTEM_MODE; }
      else if (flag == "outitb")     { status.outitb = true; }
      else
        {
          std::cerr << "Unknown execution state flag: " << flag << std::endl;
          return 1;
        }
    }

  actset.translate( std::cout );
  
  return 0;
}

Processor::CP15Reg&
Processor::CP15GetRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
{
  switch (CP15ENCODE( crn, opcode1, crm, opcode2 ))
    {
      /****************************
       * Identification registers *
       ****************************/
    case CP15ENCODE( 0, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CTR, Cache Type Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("ctr"); }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 0, 0, 0, 5 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "MPIDR, Multiprocessor Affinity Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("mpidr"); }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 0, 0, 1, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ID_PFR0, Processor Feature Register 0"; }
          U32 Read( Processor& proc ) { return proc.SReg("id_pfr0"); }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 0, 1, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CCSIDR, Cache Size ID Registers"; }
          U32 Read( Processor& proc ) { return proc.SReg("ccsidr"); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 0, 1, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CLIDR, Cache Level ID Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("clidr"); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 0, 2, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CSSELR, Cache Size Selection Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("csselr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("csselr") = value; }
        } x;
        return x;
      } break;
      
      /****************************
       * System control registers *
       ****************************/
    case CP15ENCODE( 1, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "SCTLR, System Control Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("sctlr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("sctlr") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 1, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ACTLR, Auxiliary Control Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("actlr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("actlr") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 1, 0, 0, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CPACR, Coprocessor Access Control Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("cpacr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("cpacr") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 1, 0, 1, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "NSACR, Non-Secure Access Control Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("nsacr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("nsacr") = value; }
        } x;
        return x;
      } break;

      /*******************************************
       * Memory protection and control registers *
       *******************************************/
    case CP15ENCODE( 2, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TTBR0, Translation Table Base Register 0"; }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("ttbr0") = value; }
          U32 Read( Processor& proc ) { return proc.SReg("ttbr0"); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 2, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TTBR1, Translation Table Base Register 1"; }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("ttbr1") = value; }
          U32 Read( Processor& proc ) { return proc.SReg("ttbr1"); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 2, 0, 0, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TTBCR, Translation Table Base Control Register"; }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("ttbcr") = value; }
          U32 Read( Processor& proc ) { return proc.SReg("ttbcr"); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 3, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DACR, Domain Access Control Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("dacr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dacr") = value; }
        } x;
        return x;
      } break;


      /*********************************
       * Memory system fault registers *
       *********************************/
    case CP15ENCODE( 5, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DFSR, Data Fault Status Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("dfsr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dfsr") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 5, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "IFSR, Instruction Fault Status Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("ifsr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("ifsr") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 6, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DFAR, Data Fault Status Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("dfar"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dfar") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 6, 0, 0, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "IFAR, Instruction Fault Status Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("ifar"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("ifar") = value; }
        } x;
        return x;
      } break;

      /***************************************************************
       * Cache maintenance, address translation, and other functions *
       ***************************************************************/
          
    case CP15ENCODE( 7, 0, 1, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ICIALLUIS, Invalidate all instruction caches to PoU Inner Shareable"; }
          U32 Read( Processor& proc ) { return proc.SReg("icialluis"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("icialluis") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 1, 6 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "BPIALLIS, Invalidate all branch predictors Inner Shareable"; }
          U32 Read( Processor& proc ) { return proc.SReg("bpiallis"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("bpiallis") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 5, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ICIALLU, Invalidate all instruction caches to PoU"; }
          U32 Read( Processor& proc ) { return proc.SReg("iciallu"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("iciallu") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 5, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ICIMVAU, Clean data* cache line by MVA to PoU"; }
          U32 Read( Processor& proc ) { return proc.SReg("icimvau"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("icimvau") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 5, 6 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "BPIALL, Invalidate all branch predictors"; }
          U32 Read( Processor& proc ) { return proc.SReg("bpiall"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("bpiall") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 6, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCIMVAC, Invalidate data* cache line by MVA to PoC"; }
          U32 Read( Processor& proc ) { return proc.SReg("dcimvac"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dcimvac") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 6, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCISW, Invalidate data* cache line by set/way"; }
          U32 Read( Processor& proc ) { return proc.SReg("dcisw"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dcisw") = value; }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 7, 0, 10, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCMVAC, Clean data* cache line by MVA to PoC"; }
          U32 Read( Processor& proc ) { return proc.SReg("dccmvac"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dccmvac") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 10, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCSW, Clean data* cache line by set/way"; }
          U32 Read( Processor& proc ) { return proc.SReg("dccsw"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dccsw") = value; }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 7, 0, 11, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCMVAU, Clean data* cache line by MVA to PoU"; }
          U32 Read( Processor& proc ) { return proc.SReg("dccmvau"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dccmvau") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 14, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCIMVAC, Clean and invalidate data* cache line by MVA to PoC"; }
          U32 Read( Processor& proc ) { return proc.SReg("dccimvac"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dccimvac") = value; }
        } x;
        return x;
      } break;
          
      /******************************
       * TLB maintenance operations *
       ******************************/

    case CP15ENCODE( 8, 0, 3, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TLBIALLIS, Invalidate entire TLB Inner Shareable"; }
          U32 Read( Processor& proc ) { return proc.SReg("tlbiallis"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("tlbiallis") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 8, 0, 7, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TLBIALL, invalidate unified TLB"; }
          U32 Read( Processor& proc ) { return proc.SReg("tlbiall"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("tlbiall") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 8, 0, 7, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TLBIASID, invalidate unified TLB by ASID match"; }
          U32 Read( Processor& proc ) { return proc.SReg("tlbiasid"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("tlbiasid") = value; }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 12, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "VBAR, Vector Base Address Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("vbar"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("vbar") = value; }
        } x;
        return x;
      } break;
          
      /***********************************/
      /* Context and thread ID registers */
      /***********************************/

    case CP15ENCODE( 13, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CONTEXTIDR, Context ID Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("contextidr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("contextidr") = value; }
        } x;
        return x;
      } break;

      /* BOARD specific */
          
    case CP15ENCODE( 15, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DIAGCR, undocumented Diagnostic Control register"; }
          U32 Read( Processor& proc ) { return proc.SReg("diagcr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("diagcr") = value; }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 15, 4, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CFGBAR, Configuration Base Address"; }
          U32 Read( Processor& proc ) { return proc.SReg("cfgbar"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("cfgbar") = value; }
        } x;
        return x;
      } break;

    }

  static struct CP15Error : public CP15Reg {
    char const* Describe() { return "Unknown CP15 register"; }
  } err;
  return err;
}

void
Processor::PSR::Set( NZCVRF const& _, U32 const& value )
{
  n = BOOL( unisim::component::cxx::processor::arm::RegisterField< 3,1>().Get( value ) ).expr;
  z = BOOL( unisim::component::cxx::processor::arm::RegisterField< 2,1>().Get( value ) ).expr;
  c = BOOL( unisim::component::cxx::processor::arm::RegisterField< 1,1>().Get( value ) ).expr;
  v = BOOL( unisim::component::cxx::processor::arm::RegisterField< 0,1>().Get( value ) ).expr;
}

Processor::U32
Processor::PSR::GetBits()
{
  return
    (U32(BOOL(n)) << 31) |
    (U32(BOOL(z)) << 30) |
    (U32(BOOL(c)) << 29) |
    (U32(BOOL(v)) << 28) |
    (U32(itstate >> U8(2)) << 10) | (U32(itstate & U8(0b11)) << 25) |
    U32((uint32_t(GetJ()) << 24) | (uint32_t(GetT()) << 5) | uint32_t(mode)) |
    bg;
}
      
void
Processor::PSR::SetBits( U32 const& bits, uint32_t mask )
{
  if (NRF().Get(mask)) { n = BOOL( NRF().Get(bits) ).expr; NRF().Set(mask, 0u); }
  if (ZRF().Get(mask)) { z = BOOL( ZRF().Get(bits) ).expr; ZRF().Set(mask, 0u); }
  if (CRF().Get(mask)) { c = BOOL( CRF().Get(bits) ).expr; CRF().Set(mask, 0u); }
  if (VRF().Get(mask)) { v = BOOL( VRF().Get(bits) ).expr; VRF().Set(mask, 0u); }
        
  if (ITHIRF().Get(mask) or ITLORF().Get(mask))
    {
      itstate = U8((ITHIRF().Get(bits) << 2) | ITLORF().Get(bits));
      uint32_t itmask = ITHIRF().getmask<uint32_t>() | ITLORF().getmask<uint32_t>();
      if ((mask & itmask) != itmask)
        throw 0;
      mask &= ~itmask;
      ITHIRF().Set(mask, 0u); ITLORF().Set(mask, 0u);
    }
        
  if (MRF().Get(mask))
    {
      if (MRF().Get(mask) != 0x1f)
        throw 0;
      U32       nmode = MRF().Get(bits);
      MRF().Set(mask, 0u);
      if (proc.Test(nmode != U32(mode)))
        proc.UnpredictableInsnBehaviour();
    }
        
  if (JRF().Get(mask)) { if (proc.Test(JRF().Get(bits) != U32(GetJ())))    { proc.UnpredictableInsnBehaviour(); } JRF().Set(mask, 0u); }
  if (TRF().Get(mask)) { if (proc.Test(TRF().Get(bits) != U32(GetT())))    { proc.UnpredictableInsnBehaviour(); } TRF().Set(mask, 0u); }
  if (ERF().Get(mask)) { if (proc.Test(ERF().Get(bits) != U32(bigendian))) { proc.UnpredictableInsnBehaviour(); } ERF().Set(mask, 0u); }
        
  bg = (bg & U32(~mask)) | (bits & U32(mask));
}
