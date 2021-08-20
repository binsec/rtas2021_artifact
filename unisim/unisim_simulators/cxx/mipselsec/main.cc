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

#include <unisim/component/cxx/processor/mips/isa/disasm.hh>
#include <unisim/component/cxx/processor/mips/isa/mipsel.tcc>
#include <unisim/util/symbolic/binsec/binsec.hh>
#include <unisim/util/symbolic/symbolic.hh>

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <cstdio>

struct Processor
{
  struct Unimplemented {};
  struct Undefined {};
    
    
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
  
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_DBG, B_RFE };
  
//   typedef unisim::util::symbolic::FP                   FP;
  typedef unisim::util::symbolic::Expr                 Expr;
  typedef unisim::util::symbolic::ScalarType           ScalarType;
  
  typedef unisim::util::symbolic::binsec::ActionNode   ActionNode;
  typedef unisim::util::symbolic::binsec::Load         Load;
  typedef unisim::util::symbolic::binsec::Store        Store;
  typedef unisim::util::symbolic::binsec::Branch       Br;

  template <typename RID>
  struct RegRead : public unisim::util::symbolic::binsec::RegRead
  {
    typedef RegRead<RID> this_type;
    typedef unisim::util::symbolic::binsec::RegRead Super;
    RegRead( RID _id, ScalarType::id_t _tp ) : Super(), tp(_tp), id(_id) {}
    virtual this_type* Mutate() const { return new this_type( *this ); }
    virtual ScalarType::id_t GetType() const { return tp; }
    virtual void GetRegName( std::ostream& sink ) const { sink << id.c_str(); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegRead const&>( rhs ) ); }
    int compare( RegRead const& rhs ) const { if (int delta = int(tp) - int(rhs.tp)) return delta; if (int delta = id.cmp( rhs.id )) return delta; return Super::compare(rhs); }

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
    Call( Expr const& value, uint32_t ra ) : Goto( value ), return_address( ra ) {}
    virtual Call* Mutate() const override { return new Call( *this ); }
    virtual void annotate(std::ostream& sink) const override { sink << " // call (" << unisim::util::symbolic::binsec::dbx(4,return_address) << ",0)"; }

    uint32_t return_address;
  };

  struct RegID : public unisim::util::identifier::Identifier<RegID>
  {
    enum Code
      {
       NA = 0, at, v0, v1, a0, a1, a2, a3,
       t0, t1, t2, t3, t4, t5, t6, t7,
       s0, s1, s2, s3, s4, s5, s6, s7,
       t8, t9, k0, k1, gp, sp, fp, ra,
       npc, hi, lo,
       end
      } code;

    char const* c_str() const
    {
      switch (code)
        {
        case at:  return "at";
        case v0:  return "v0";
        case v1:  return "v1";
        case a0:  return "a0";
        case a1:  return "a1";
        case a2:  return "a2";
        case a3:  return "a3";
        case t0:  return "t0";
        case t1:  return "t1";
        case t2:  return "t2";
        case t3:  return "t3";
        case t4:  return "t4";
        case t5:  return "t5";
        case t6:  return "t6";
        case t7:  return "t7";
        case s0:  return "s0";
        case s1:  return "s1";
        case s2:  return "s2";
        case s3:  return "s3";
        case s4:  return "s4";
        case s5:  return "s5";
        case s6:  return "s6";
        case s7:  return "s7";
        case t8:  return "t8";
        case t9:  return "t9";
        case k0:  return "k0";
        case k1:  return "k1";
        case gp:  return "gp";
        case sp:  return "sp";
        case fp:  return "fp";
        case ra:  return "ra";
        case npc: return "npc";
        case lo:  return "lo";
        case hi:  return "hi";
        case NA:
        case end: break;
        }
      return "NA";
    }
      
    RegID() : code(end) {}
    RegID( Code _code ) : code(_code) {}
    RegID( char const* _code ) : code(end) { init( _code ); }
  };

  //   =====================================================================
  //   =                      Construction/Destruction                     =
  //   =====================================================================
  
private:
  Processor( Processor const& );
public:
  
  Processor()
    : path(0)
    , gprs()
    , hi(), lo()
    , insn_addrs()
    , branch_type(B_JMP)
    , return_address()
    , stores()
  {
    // GPR regs
    gprs[0] = U32(0);
    for (unsigned reg = 1; reg < 32; ++reg)
      gprs[reg] = U32( newRegRead( RegID("at") + (reg-1), ScalarType::U32 ) );
    hi = newRegRead( RegID("hi"), ScalarType::U32 );
    lo = newRegRead( RegID("lo"), ScalarType::U32 );

  }

  void execute(unisim::component::cxx::processor::mips::isa::Operation<Processor>* insn)
  {
    uint32_t insn_addr = insn->GetAddr();
    insn_addrs[0] = U32(insn_addr);
    insn_addrs[1] = U32(newRegRead( RegID("npc"), ScalarType::U32 ));
    insn_addrs[2] = insn_addrs[1] + U32(4);
    return_address = insn_addr + 8;
    insn->execute( *this );
  }
  
  bool close( Processor const& ref )
  {
    bool complete = path->close();
    if (branch_type == B_CALL)
      path->add_sink( new Call( insn_addrs[2].expr, return_address ) );
    else
      path->add_sink( new Goto( insn_addrs[2].expr ) );
    for (unsigned reg = 1; reg < 32; ++reg)
      {
        if (gprs[reg].expr != ref.gprs[reg].expr)
          path->add_sink( newRegWrite( RegID("at") + reg, gprs[reg].expr ) );
      }
    if (hi.expr != ref.hi.expr) path->add_sink( newRegWrite( RegID("hi"), hi.expr ) );
    if (lo.expr != ref.lo.expr) path->add_sink( newRegWrite( RegID("lo"), lo.expr ) );
    for (std::set<Expr>::const_iterator itr = stores.begin(), end = stores.end(); itr != end; ++itr)
      path->add_sink( *itr );
    return complete;
  }

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
  
  bool     IsBigEndian() const { return false; }
  
  //   =====================================================================
  //   =             General Purpose Registers access methods              =
  //   =====================================================================
    
  void     SetGPR(unsigned idx, U32 value) { if (idx) gprs[idx] = value; }
  U32      GetGPR(unsigned idx) { return gprs[idx]; }
  void     SetDivU(U32 rs, U32 rt) { lo = rs / rt; hi = rs % rt; }
  void     SetHI(unsigned ra, U32 value) { if (ra) throw "nope"; hi = value; }
  void     SetLO(unsigned ra, U32 value) { if (ra) throw "nope"; lo = value; }
  U32      GetHI(unsigned ra) { if (ra) throw "nope"; return hi; }
  U32      GetLO(unsigned ra) { if (ra) throw "nope"; return lo; }

  U32      GetHWR(unsigned idx) { throw Unimplemented(); return U32(); }
  
  //   =====================================================================
  //   =                      Control Transfer methods                     =
  //   =====================================================================

  U32  GetPC() { return insn_addrs[0]; }
  void Branch( U32 const& target, branch_type_t _branch_type ) { insn_addrs[2] = target; branch_type = _branch_type; }
  void CancelDelaySlot() { insn_addrs[1] = insn_addrs[2]; insn_addrs[2] += U32(4); }
    
  void SysCall( unsigned imm ) { throw Unimplemented(); }
  
  //   =====================================================================
  //   =                       Memory access methods                       =
  //   =====================================================================
  U32 MemLoad( U32 const&, Expr const& addr ) { return U32(Expr( new Load(addr, 2, 2, false) ) ); }
  U16 MemLoad( U16 const&, Expr const& addr ) { return U16(Expr( new Load(addr, 1, 1, false) ) ); }
  U8  MemLoad( U8  const&, Expr const& addr ) { return  U8(Expr( new Load(addr, 0, 0, false) ) ); }
  S32 MemLoad( S32 const&, Expr const& addr ) { return S32( U32(Expr( new Load(addr, 2, 2, false) ) ) ); }
  S16 MemLoad( S16 const&, Expr const& addr ) { return S16( U16(Expr( new Load(addr, 1, 1, false) ) ) ); }
  S8  MemLoad( S8  const&, Expr const& addr ) { return  S8(  U8(Expr( new Load(addr, 0, 0, false) ) ) ); }
  template <class T> T MemRead( U32 const& addr ) { return MemLoad( T(), addr.expr ); }
  
  void MemStore( Expr const& addr, U32 const& value ) { stores.insert( new Store( addr, value.expr, 2, 2, false ) ); }
  void MemStore( Expr const& addr, U16 const& value ) { stores.insert( new Store( addr, value.expr, 1, 1, false ) ); }
  void MemStore( Expr const& addr, U8  const& value ) { stores.insert( new Store( addr, value.expr, 0, 0, false ) ); }
  template <typename U> void MemWrite( U32 const& addr, U value ) { return MemStore( addr.expr, value ); }
    
  void AtomicBegin(U32 const&) { }
  BOOL AtomicUpdate(U32 const&) { return BOOL(true); }

  ActionNode*      path;
  U32              gprs[32];
  U32              hi, lo;
  U32              insn_addrs[3];
  branch_type_t    branch_type;
  uint32_t         return_address;
  std::set<Expr>   stores;
  //  bool             unpredictable;
  
};

;

struct MIPSISA : public unisim::component::cxx::processor::mips::isa::Decoder<Processor>
{
  typedef unisim::component::cxx::processor::mips::isa::CodeType CodeType;
  typedef unisim::component::cxx::processor::mips::isa::Operation<Processor> Operation;
  static CodeType mkcode( uint32_t code ) { return CodeType( code ); }
  static bool const is_thumb = true;
};

struct Translator
{
  typedef unisim::util::symbolic::binsec::ActionNode ActionNode;
  
  Translator( uint32_t _addr, uint32_t _code )
    : addr(_addr), code(_code), coderoot(new ActionNode)
  {}
  ~Translator() { delete coderoot; }
  
  template <class ISA>
  void
  extract( std::ostream& sink, ISA& isa )
  {
    sink << "(address . " << unisim::util::symbolic::binsec::dbx(4, addr) << ")\n";
  
    // Instruction decoding
    struct Instruction
    {
      typedef typename ISA::Operation Operation;
      Instruction(ISA& isa, uint32_t addr, uint32_t code)
        : operation(0), bytecount(0)
      {
        operation = isa.NCDecode( addr, ISA::mkcode( code ) );
        unsigned bitlength = operation->GetLength(); 
        if ((bitlength != 32) and ((bitlength != 16) or not ISA::is_thumb))
          { delete operation; operation = 0; }
        bytecount = bitlength/8;
      }
      ~Instruction() { delete operation; }
      Operation* operator -> () { return operation; }
      Operation* get() const { return operation; }
      
      Operation* operation;
      unsigned   bytecount;
    };
    
    Instruction instruction( isa, addr, code );
    
    {
      uint32_t encoding = instruction->GetEncoding();
      sink << "(opcode . " << unisim::util::symbolic::binsec::dbx(4, encoding) << ")\n(size . 4)\n";
    }
    
    Processor::U32      insn_addr = unisim::util::symbolic::make_const(addr); //< concrete instruction address
    // Processor::U32      insn_addr = Expr(new InstructionAddress()); //< symbolic instruction address
    Processor reference;
    
    // Disassemble
    sink << "(mnemonic . \"";
    try { instruction->disasm( sink ); }
    catch (...) { sink << "(bad)"; }
    sink << "\")\n";
    
    // Get actions
    for (bool end = false; not end;)
      {
        Processor state;
        state.path = coderoot;
        state.execute(instruction.get());
        end = state.close( reference );
      }
    coderoot->simplify();
    coderoot->commit_stats();
  }

  void translate( std::ostream& sink )
  {
    try
      {
        MIPSISA thumbisa;
        extract( sink, thumbisa );
      }
    catch (Processor::Undefined const&)
      {
        sink << "(undefined)\n";
        return;
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
      sink << "(" << unisim::util::symbolic::binsec::dbx(4, addr) << ',' << itr->first << ") " << itr->second << std::endl;
  }

  uint32_t    addr, code;
  ActionNode* coderoot;
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
    "usage: <program> <address> <encoding>\n";
}

int
main( int argc, char** argv )
{
  if (argc != 3)
    {
      std::cerr << "Wrong number of CLI arguments.\n" << usage();
      return 1;
    }

  uint32_t addr, code;

  if (not getu32(addr, argv[1]) or not getu32(code, argv[2]))
    {
      std::cerr << "<addr> and <code> should be 32bits numeric values.\n" << usage();
      return 1;
    }

  Translator actset( addr, code );
  
  actset.translate( std::cout );
  
  return 0;
}

