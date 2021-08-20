/*
 *  Copyright (c) 2009-2020,
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

#include <unisim/util/symbolic/binsec/binsec.hh>
#include <unisim/util/symbolic/symbolic.hh>
#include <aarch64dec.tcc>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <set>

struct Processor
{
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
  typedef unisim::util::symbolic::ScalarType           ScalarType;
  typedef unisim::util::symbolic::binsec::ActionNode   ActionNode;

  typedef unisim::util::symbolic::binsec::Store        Store;
  typedef unisim::util::symbolic::binsec::Branch       Branch;
  
  template <typename RID>
  struct RegRead : public unisim::util::symbolic::binsec::RegRead
  {
    typedef RegRead<RID> this_type;
    typedef unisim::util::symbolic::binsec::RegRead Super;
    RegRead( RID _id, ScalarType::id_t _tp ) : Super(), tp(_tp), id(_id) {}
    virtual this_type* Mutate() const { return new this_type( *this ); }
    virtual ScalarType::id_t GetType() const override { return tp; }
    virtual void GetRegName( std::ostream& sink ) const override { sink << id.c_str(); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegRead const&>( rhs ) ); }
    int compare( RegRead const& rhs ) const { if (int delta = int(tp) - int(rhs.tp)) return delta; if (int delta = id.cmp( rhs.id )) return delta; return Super::compare(rhs); }
    virtual Expr Simplify() const { return this; }

    ScalarType::id_t tp;
    RID id;
  };

  template <typename RID>
  static Expr newRegRead( RID id, ScalarType::id_t tp ) { return new RegRead<RID>( id, tp ); }

  template <typename RID>
  struct RegWrite : public unisim::util::symbolic::binsec::RegWrite
  {
    typedef RegWrite<RID> this_type;
    typedef unisim::util::symbolic::binsec::RegWrite Super;
    RegWrite( RID _id, Expr const& _value ) : Super(_value), id(_id) {}
    virtual this_type* Mutate() const { return new this_type( *this ); }
    
    virtual void GetRegName( std::ostream& sink ) const override { sink << id.c_str(); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegWrite const&>( rhs ) ); }
    int compare( RegWrite const& rhs ) const { if (int delta = id.cmp( rhs.id )) return delta; return Super::cmp( rhs ); }
    virtual Expr Simplify() const override
    {
      Expr nvalue( ASExprNode::Simplify( value ) );
      return nvalue != value ? new RegWrite<RID>( id, nvalue ) : this;
    }
    
    RID id;
  };
  
  template <typename RID>
  static RegWrite<RID>* newRegWrite( RID id, Expr const& value )
  { return new RegWrite<RID>( id, value ); }
  
  struct Goto : public unisim::util::symbolic::binsec::Branch
  {
    Goto( Expr const& value ) : unisim::util::symbolic::binsec::Branch( value ) {}
    virtual Goto* Mutate() const override { return new Goto( *this ); }
    virtual void GetRegName( std::ostream& sink ) const override { sink << "pc"; }
    virtual void annotate(std::ostream& sink) const override { return; }
  };

  struct Call : public Goto
  {
    Call( Expr const& value, uint32_t ra ) : Goto( value ), return_address( ra ) {}
    virtual Call* Mutate() const override { return new Call( *this ); }
    virtual void annotate(std::ostream& sink) const override { sink << " // call (" << unisim::util::symbolic::binsec::dbx(4,return_address) << ",0)"; }

    uint32_t return_address;
  };

  //   =====================================================================
  //   =                      Construction/Destruction                     =
  //   =====================================================================
    
  struct StatusRegister
  {
    typedef unisim::util::symbolic::Expr       Expr;
    StatusRegister() {}
  };

  Processor(StatusRegister const& ref_psr, U64 const& insn_addr)
    : path(0)
    , gpr()
    , flags()
    , current_instruction_address(insn_addr)
    , next_instruction_address(insn_addr + U64(4))
    , branch_type(B_JMP)
    , stores()
    , unpredictable( false )
  {
    for (GPR reg; reg.next();)
      gpr[reg.idx()] = newRegRead(reg, ScalarType::U64);
    for (Flag flag; flag.next();)
      flags[flag.idx()] = newRegRead(flag, ScalarType::BOOL);
  }
  
  bool
  close( Processor const& ref, uint64_t linear_nia )
  {
    bool complete = path->close();
    if (branch_type == B_CALL)
      path->sinks.insert( Expr( new Call( next_instruction_address.expr, linear_nia ) ) );
    else
      path->sinks.insert( Expr( new Goto( next_instruction_address.expr ) ) );
    if (unpredictable)
      {
        path->sinks.insert( Expr( new unisim::util::symbolic::binsec::AssertFalse() ) );
        return complete;
      }
    
    for (GPR reg; reg.next();)
      if (gpr[reg.idx()] != ref.gpr[reg.idx()])
        path->sinks.insert( Expr( newRegWrite( reg, gpr[reg.idx()] ) ) );
    
    for (Flag flag; flag.next();)
      if (flags[flag.idx()] != ref.flags[flag.idx()])
        path->sinks.insert( Expr( newRegWrite( flag, flags[flag.idx()] ) ) );

    for (std::set<Expr>::const_iterator itr = stores.begin(), end = stores.end(); itr != end; ++itr)
      path->sinks.insert( *itr );
    
    return complete;
  }

  //   =====================================================================
  //   =                 Internal Instruction Control Flow                 =
  //   =====================================================================
    
  template <typename OPER>
  void UndefinedInstruction(OPER const*) { throw unisim::component::cxx::processor::arm::isa::Reject(); }
    
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
  
  // template <typename T>
  // bool Cond( unisim::util::symbolic::SmartValue<T> const& cond )
  // {
  //   if (not cond.expr.good())
  //     throw std::logic_error( "Not a valid condition" );
      
  //   Expr cexp( BOOL(cond).expr );
  //   if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
  //     return cnode->Get( bool() );
      
  //   bool predicate = path->proceed( cexp );
  //   path = path->next( predicate );
  //   return predicate;
  // }

  //   =====================================================================
  //   =             General Purpose Registers access methods              =
  //   =====================================================================
    
  U64  GetGSR(unsigned id) const { return U64(gpr[id]); }
  U64  GetGZR(unsigned id) const { return id == 31 ? U64(0) : U64(gpr[id]); }
  template <typename T>
  void SetGSR(unsigned id, unisim::util::symbolic::SmartValue<T> const& val) { gpr[id] = U64(val).expr; }
  template <typename T>
  void SetGZR(unsigned id, unisim::util::symbolic::SmartValue<T> const& val) { if (id == 31) return; gpr[id] = U64(val).expr; }
  
  //   =====================================================================
  //   =                  Arithmetic Flags access methods                  =
  //   =====================================================================
  void SetNZCV(BOOL const& N, BOOL const& Z, BOOL const& C, BOOL const& V)
  {
    flags[3] = N.expr; flags[2] = Z.expr; flags[1] = C.expr; flags[0] = V.expr;
  }
  U8   GetNZCV() const
  {
    U8 res = U8(BOOL(flags[0]));
    for (int idx = 1; idx < 4; ++idx)
      res = res | (U8(BOOL(flags[idx])) << idx );
    return res;
  }
  BOOL GetCarry() const { return BOOL(flags[1]); }
    
  //   ====================================================================
  //   =                 Vector  Registers access methods                 
  //   ====================================================================

  //   U8  GetVU8 ( unsigned reg, unsigned sub ) { return VectorStorage<U8> (reg)[sub]; }
  //   U16 GetVU16( unsigned reg, unsigned sub ) { return VectorStorage<U16>(reg)[sub]; }
  //   U32 GetVU32( unsigned reg, unsigned sub ) { return VectorStorage<U32>(reg)[sub]; }
  //   U64 GetVU64( unsigned reg, unsigned sub ) { return VectorStorage<U64>(reg)[sub]; }
  //   S8  GetVS8 ( unsigned reg, unsigned sub ) { return VectorStorage<S8> (reg)[sub]; }
  //   S16 GetVS16( unsigned reg, unsigned sub ) { return VectorStorage<S16>(reg)[sub]; }
  //   S32 GetVS32( unsigned reg, unsigned sub ) { return VectorStorage<S32>(reg)[sub]; }
  //   S64 GetVS64( unsigned reg, unsigned sub ) { return VectorStorage<S64>(reg)[sub]; }
  
  //   void SetVU8 ( unsigned reg, unsigned sub, U8  value ) { VectorStorage<U8> (reg)[sub] = value; }
  //   void SetVU16( unsigned reg, unsigned sub, U16 value ) { VectorStorage<U16>(reg)[sub] = value; }
  //   void SetVU32( unsigned reg, unsigned sub, U32 value ) { VectorStorage<U32>(reg)[sub] = value; }
  //   void SetVU64( unsigned reg, unsigned sub, U64 value ) { VectorStorage<U64>(reg)[sub] = value; }
  //   void SetVS8 ( unsigned reg, unsigned sub, S8  value ) { VectorStorage<S8> (reg)[sub] = value; }
  //   void SetVS16( unsigned reg, unsigned sub, S16 value ) { VectorStorage<S16>(reg)[sub] = value; }
  //   void SetVS32( unsigned reg, unsigned sub, S32 value ) { VectorStorage<S32>(reg)[sub] = value; }
  //   void SetVS64( unsigned reg, unsigned sub, S64 value ) { VectorStorage<S64>(reg)[sub] = value; }
  
  //   void SetVU8 ( unsigned reg, U8 value )  { VectorZeroedStorage<U8> (reg)[0] = value; }
  //   void SetVU16( unsigned reg, U16 value ) { VectorZeroedStorage<U16>(reg)[0] = value; }
  //   void SetVU32( unsigned reg, U32 value ) { VectorZeroedStorage<U32>(reg)[0] = value; }
  //   void SetVU64( unsigned reg, U64 value ) { VectorZeroedStorage<U64>(reg)[0] = value; }
  //   void SetVS8 ( unsigned reg, S8 value )  { VectorZeroedStorage<S8> (reg)[0] = value; }
  //   void SetVS16( unsigned reg, S16 value ) { VectorZeroedStorage<S16>(reg)[0] = value; }
  //   void SetVS32( unsigned reg, S32 value ) { VectorZeroedStorage<S32>(reg)[0] = value; }
  //   void SetVS64( unsigned reg, S64 value ) { VectorZeroedStorage<S64>(reg)[0] = value; }
  
  //   void ClearHighV( unsigned reg, unsigned bytes ) { for (unsigned idx = bytes; idx < VUnion::BYTECOUNT; idx+=1 ) vector_data[reg][idx] = 0; }

  //   =====================================================================
  //   =              Special/System Registers access methods              =
  //   =====================================================================

  U64  GetPC() const { return current_instruction_address; }
  U64  GetNPC() const { return next_instruction_address; }

  void        CheckSystemAccess( uint8_t op1 ) { throw 0; }
  U64         ReadSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2 ) { throw 0; return U64(); }
  void        WriteSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, U64 value ) { throw 0; }
  void        DescribeSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, std::ostream& sink )
  {
    sink << "Unknown system register (op0=" << unsigned(op0) << ", op1=" << unsigned(op1) << ", crn=" << unsigned(crn) << ", crm=" << unsigned(crm) << ", op2=" << unsigned(op2) << ")";
  }
  void        NameSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, std::ostream& sink )
  {
    uint32_t sys_reg_idx = 0;
    sys_reg_idx = (sys_reg_idx << 1) | op0;
    sys_reg_idx = (sys_reg_idx << 3) | op1;
    sys_reg_idx = (sys_reg_idx << 4) | crn;
    sys_reg_idx = (sys_reg_idx << 4) | crm;
    sys_reg_idx = (sys_reg_idx << 3) | op2;
    sink << "#" << std::hex << sys_reg_idx << std::dec;
  }
  
  //   =====================================================================
  //   =                      Control Transfer methods                     =
  //   =====================================================================
  
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET };
  void BranchTo(U64 const& npc, branch_type_t bt)
  {
    next_instruction_address = npc;
    branch_type = bt;
  }
  
  void CallSupervisor( uint32_t imm ) { throw 0;  }
  
  //   =====================================================================
  //   =                       Memory access methods                       =
  //   =====================================================================
  
  struct Load : public unisim::util::symbolic::binsec::Load
  {
    Load( Expr const& addr, unsigned size, unsigned alignment, bool bigendian )
      : unisim::util::symbolic::binsec::Load(addr, size, alignment, bigendian)
    {}
  };
  
  U64  MemRead64(U64 addr) { return U64( Expr( new Load( addr.expr, 3, 0, false ) ) ); }
  U32  MemRead32(U64 addr) { return U32( Expr( new Load( addr.expr, 2, 0, false ) )); }
  U16  MemRead16(U64 addr) { return U16( Expr( new Load( addr.expr, 1, 0, false ) ) ); }
  U8   MemRead8 (U64 addr) { return U8 ( Expr( new Load( addr.expr, 0, 0, false ) ) ); }
    
  void MemWrite64(U64 addr, U64 value) { stores.insert( new Store( addr.expr, value.expr, 3, 0, false ) ); }
  void MemWrite32(U64 addr, U32 value) { stores.insert( new Store( addr.expr, value.expr, 2, 0, false ) ); }
  void MemWrite16(U64 addr, U16 value) { stores.insert( new Store( addr.expr, value.expr, 1, 0, false ) ); }
  void MemWrite8 (U64 addr, U8  value) { stores.insert( new Store( addr.expr, value.expr, 0, 0, false ) ); }
    
  void ClearExclusiveLocal() { throw 0; }
  void SetExclusiveMonitors( U64, unsigned size ) { throw 0; }
  bool ExclusiveMonitorsPass( U64 addr, unsigned size ) { throw 0; }
  
  //   =====================================================================
  //   =                         Processor Storage                         =
  //   =====================================================================
  
  struct GPR : public unisim::util::identifier::Identifier<GPR>
  {
    enum Code
      {
        x0,  x1,  x2,  x3,  x4,  x5,  x6,  x7,  x8,  x9,  x10, x11, x12, x13, x14, x15,
        x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, sp, end
      } code;

    char const* c_str() const
    {
      static char const* names[] =
        {
          "x0",  "x1",  "x2",  "x3",  "x4",  "x5",  "x6",  "x7",  "x8",  "x9",  "x10", "x11", "x12", "x13", "x14", "x15",
          "x16", "x17", "x18", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "x29", "x30", "sp", "NA"
        };
      return names[int(code)];
    }
    
    GPR() : code(end) {}
    GPR( Code _code ) : code(_code) {}
    GPR( char const* _code ) : code(end) { init( _code ); }
  };

  struct Flag : public unisim::util::identifier::Identifier<Flag>
  {
    enum Code { N, Z, C, V, end } code;

    char const* c_str() const
    {
      static char const* names[] = {"n", "z", "c", "v", "NA"};
      return names[int(code)];
    }
    
    Flag() : code(end) {}
    Flag( Code _code ) : code(_code) {}
    Flag( char const* _code ) : code(end) { init( _code ); }
  };

  ActionNode*      path;
  Expr             gpr[GPR::end];
  Expr             flags[Flag::end];
  U64              current_instruction_address;
  U64              next_instruction_address;
  branch_type_t    branch_type;
  std::set<Expr>   stores;
  bool             unpredictable;
};

struct Translator
{
  typedef unisim::component::cxx::processor::arm::isa::arm64::Decoder<Processor> Decoder;
  typedef unisim::component::cxx::processor::arm::isa::arm64::Operation<Processor> Operation;
  
  typedef unisim::util::symbolic::binsec::ActionNode ActionNode;
  
  Translator( uint64_t _addr, uint32_t _code )
    : coderoot(new ActionNode), addr(_addr), code(_code)
  {}
  ~Translator() { delete coderoot; }
   
  void
  translate( std::ostream& sink )
  {
    sink << "(address . " << unisim::util::symbolic::binsec::dbx(8, addr) << ")\n";
    sink << "(opcode . " << unisim::util::symbolic::binsec::dbx(4, code) << ")\n";
    sink << "(size . 4)\n";
    
    struct Instruction
    {
      Instruction(uint32_t addr, uint32_t code)
        : operation(0)
      {
        try
          {
            Decoder decoder;
            operation = decoder.NCDecode( addr, code );
          }
        catch (unisim::component::cxx::processor::arm::isa::Reject const&)
          { operation = 0; }
      }
      ~Instruction() { delete operation; }
      Operation* operator -> () { return operation; }

      Operation* operation;
    };

    Instruction instruction(addr, code);

    if (not instruction.operation)
      {
        sink << "(illegal)\n";
        return;
      }

    Processor::U64      insn_addr = unisim::util::symbolic::make_const(addr); //< concrete instruction address
    // Processor::U64      insn_addr = Expr(new InstructionAddress); //< symbolic instruction address
    Processor reference( status, insn_addr );

    // Disassemble
    sink << "(mnemonic . \"";
    try { instruction->disasm( reference, sink ); }
    catch (...) { sink << "(bad)"; }
    sink << "\")\n";

    // Get actions
    try
      {
        for (bool end = false; not end;)
          {
            Processor state( reference );
            state.path = coderoot;
            instruction->execute( state );
            end = state.close( reference, addr + 4 );
          }
        coderoot->simplify();
        coderoot->commit_stats();
      }
    catch (...)
      {
        sink << "(unimplemented)\n";
        return;
      }

    // Translate to DBA
    unisim::util::symbolic::binsec::Program program;
    program.Generate( coderoot );
    typedef unisim::util::symbolic::binsec::Program::const_iterator Iterator;
    for (Iterator itr = program.begin(), end = program.end(); itr != end; ++itr)
      sink << "(" << unisim::util::symbolic::binsec::dbx(8, addr) << ',' << itr->first << ") " << itr->second << std::endl;
  }

  Processor::StatusRegister status;
  ActionNode*               coderoot;
  uint64_t                  addr;
  uint32_t                  code;
};

template <typename T>
T getu( T& res, char const* arg )
{
  char *end;
  uint64_t tmp = strtoull( arg, &end, 0 );
  if ((*arg == '\0') or (*end != '\0'))
    return false;
  res = tmp;
  return true;
}

char const* usage()
{
  return
    "usage: <program> [<flags>] <address> <encoding>\n";
}

int
main( int argc, char** argv )
{
  if (argc < 3)
    {
      std::cerr << "Wrong number of CLI arguments.\n" << usage();
      return 1;
    }
  
  uint64_t addr;
  uint32_t code;

  if (not getu(addr, argv[argc-2]) or not getu(code, argv[argc-1]))
    {
      std::cerr << "<addr> and <code> should be, respectively, 64bits and 32bits numeric values.\n" << usage();
      return 1;
    }

  Translator translator( addr, code );

  translator.translate( std::cout );

  return 0;
}
