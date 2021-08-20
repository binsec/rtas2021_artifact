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

#include <top_mpc57.hh>
#include <testutils.hh>
#include <arch.hh>
#include <sstream>
#include <iostream>
#include <set>

namespace ut
{
  void SourceReg::Repr( std::ostream& sink ) const { sink << 'r' << unsigned( reg ); }
  void MaskNode::Repr( std::ostream& sink ) const { sink << "Mask( " << mb << "," << me << " )"; }
  void CPU::LoadRepr( std::ostream& sink, Expr const& _addr, unsigned bits ) { sink << "Load<"<<bits<<">( " << _addr << " )"; }
  void MixNode::Repr( std::ostream& sink ) const { sink << "Mix( " << left << ", " << right << " )"; }
  void XER::XERNode::Repr( std::ostream& sink ) const { sink << "XER"; }
  void CR::CRNode::Repr( std::ostream& sink ) const { sink << "CR"; }
  void SPEFSCR::SPEFSCRNode::Repr( std::ostream& sink ) const { sink << "SPEFSCR"; }

  void CPU::Interrupt::SetELEV(unsigned x) {}
  
  void
  Interface::irappend( uint8_t index, bool w )
  {
    // We want register operands to be in {4-7}
    if ((index < 4) or (index >= 8))
      throw Untestable("Wild register access");
    
    iruse.push_back( index );
    auto itr = irmap.find(index);
    if ((itr == irmap.end()) or (index < itr->first))
      itr = irmap.emplace_hint( itr, std::piecewise_construct, std::forward_as_tuple( index ), std::forward_as_tuple( irmap.size() ) );
    VirtualRegister& reg = itr->second;
    reg.source |= not w;
    reg.destination |= w;
  }
  
  Interface::Interface( mpc57::Operation& op )
    : xer(0), cr(0), spefscr(0), base_register(-1), aligned(false), mem_writes(false), length(op.GetLength()), retfalse(false)
  {
    bool has_valid_path = false;
    for (ActionNode path_root;;)
      {
        CPU cpu( *this, path_root );
        
        has_valid_path |= op.execute( &cpu );
        
        if (cpu.close())
          break;
      }
    
    if (not has_valid_path)
      throw ut::Untestable("illegal");
    
    if (not usemem()) return; // done
    
    struct BaseRegChecker
    {
      virtual void Process( Expr const& expr )
      {
        if (auto n = dynamic_cast<SourceReg const*>( expr.node ))
          {
            uint32_t reg  = (1u << n->reg);
            invalid |= (invalid | visited | (valid_path ? 0 : -1)) & reg;
            visited |= reg;
            return;
          }
        
        bool valid_node = false;
        
        if (auto n = expr->AsOpNode())
          {
            auto op = n->op;
            valid_node = ((op.code == op.Add) or (op.code == op.Sub));
          }
        
        bool valid_prev = valid_path;
        valid_path = valid_prev and valid_node;
        
        for (unsigned idx = 0, end = expr->SubCount(); idx < end; ++idx)
          Process( expr->GetSub(idx) );
        
        valid_path = valid_prev;
      }
      uint32_t valid() { return visited & ~invalid; }
      BaseRegChecker() : valid_path(true), invalid(0), visited(0) {}
      bool valid_path;
      uint32_t invalid, visited;
    };
    
    // We want register operands to be in {4-7}
    uint32_t valid_base_regs = 0xf0;
    
    for (Expr expr : mem_addrs) {
      BaseRegChecker brc;
      brc.Process(expr);
      valid_base_regs &= brc.valid();
    }
    
    if (valid_base_regs == 0)
      throw Untestable("no base address register");
    base_register =__builtin_ctz(valid_base_regs);
  }
  
  Interface::Prologue
  Interface::GetPrologue() const
  {
    struct Rule
    {
      Rule() : offset(0), sign(false) {} uint32_t offset; bool sign;
      void rsub( uint32_t value ) { offset = value - offset; sign = not sign; }
      bool operator<( Rule const& rhs ) const
      {
        if (sign != rhs.sign) throw Prologue::Error();
        return offset < rhs.offset;
      }
    };
    
    struct Rules
    {
      void add( Rule const& rule ) { rules.insert( rule ); }
      
      Rule GetBaseRule()
      {
        if (not rules.begin()->sign) {
          auto itr = rules.rbegin(), end = rules.rend();
          for (uint32_t lo = rules.begin()->offset, hi = itr->offset; (hi - lo) >= 16;) {
            if (++itr == end) throw Prologue::Error();
            lo = hi; hi = itr->offset;
          }
          return *itr;
        } /* else */
        auto itr = rules.begin(), end = rules.end();
        for (uint32_t lo = itr->offset, hi = rules.rbegin()->offset; (hi - lo) >= 16;) {
          if (++itr == end) throw Prologue::Error();
          hi = lo; lo = itr->offset;
        }
        return *itr;
      }

      std::set<Rule> rules;
    };

    struct Registers : Prologue::Regs
    {
      uint32_t eval(unsigned idx)
      {
        Prologue::Regs::iterator itr = lower_bound( idx );
	// idx is less or equal to itr->first.
	if (itr == end() or (idx < itr->first)) {
          uint32_t value = unsigned( rand() ) % 128; /* ppc se_li's immediate is 7 bit unsigned */
          itr = insert( itr, Prologue::Regs::value_type( idx, value ) );
        }
	return itr->second;
      }
    };
    
    struct GetRule
    {
      GetRule( Registers& _regs, Rules& _rules, unsigned _rbase ) : regs( _regs ), rules(_rules), rbase( _rbase ), rule() {}
      Registers& regs;
      Rules&     rules;
      unsigned   rbase;
      Rule       rule;
      
      void Process( ExprNode const* node )
      {
        if (auto n = dynamic_cast<SourceReg const*>(node))
          {
            // Unless reg is not rbase, we're OK !
            if (n->reg != rbase) throw Prologue::Error();
            rules.add( rule ); 
            return;
          }
        
        if (auto n = node->AsOpNode())
          {
            switch (n->op.code)
              {
              case unisim::util::symbolic::Op::Add:
                try {
                  GetRule trial(*this);
                  trial.rule.offset -= GetValue( n->GetSub(0).node );
                  trial.Process( n->GetSub(1).node );
                  rule = trial.rule;
                } catch (Prologue::Error const&) {
                  rule.offset -= GetValue( n->GetSub(1).node );
                  Process( n->GetSub(0).node );
                }
                return;
                
              case unisim::util::symbolic::Op::Sub:
                try {
                  GetRule trial(*this);
                  trial.rule.rsub( GetValue( n->GetSub(0).node ) );
                  trial.Process( n->GetSub(1).node );
                  rule = trial.rule;
                } catch (Prologue::Error const&) {
                  rule.offset += GetValue( n->GetSub(1).node );
                  Process( n->GetSub(0).node );
                }
                return;
                
              default:
                break;
              }
          }
        
        throw Prologue::Error();
      }
      
      uint32_t GetValue( ExprNode const* node )
      {
        if (auto n = dynamic_cast<SourceReg const*>(node))
          {
            if (n->reg == rbase) throw Prologue::Error();
            return regs.eval(n->reg);
          }
        
        if (auto n = dynamic_cast<unisim::util::symbolic::ConstNode<uint32_t> const*>(node))
          return n->value;
        if (auto n = dynamic_cast<unisim::util::symbolic::ConstNode<int32_t> const*>(node))
          return n->value;
        if (auto n = dynamic_cast<unisim::util::symbolic::CastNode<int32_t,uint32_t> const*>(node))
          return GetValue( n->src.node );
        if (auto n = dynamic_cast<unisim::util::symbolic::CastNode<uint32_t,int32_t> const*>(node))
          return GetValue( n->src.node );

        if (auto n = node->AsOpNode())
          {
            uint32_t lval = GetValue( n->GetSub(0).node );
            uint32_t rval = n->SubCount() > 1 ? GetValue( n->GetSub(1).node ) : 0;
            using unisim::util::symbolic::Op;
            switch (n->op.code)
              {
              case Op::Add: return lval + rval;
              case Op::Sub: return lval - rval;
              case Op::Lsl: return lval << rval;
              case Op::Lsr: return lval >> rval;
              case Op::Or:  return lval | rval;
              case Op::And: return lval & rval;
              case Op::Asr: return int32_t(lval) >> rval;
              case Op::Neg: return -lval;
              case Op::Not: return ~lval;
              default: break;
              }
          }
        
        
        // if (auto n = dynamic_cast<MulNode const*>(node))
        //   return lval * rval;
        // if (auto n = dynamic_cast<DivNode const*>(node))
        //   return lval / rval;
        // else if (auto n = dynamic_cast<TeqNode const*>(node)) {}
        // else if (auto n = dynamic_cast<TneNode const*>(node)) {}
        // else if (auto n = dynamic_cast<TgeNode const*>(node)) {}
        // else if (auto n = dynamic_cast<TleNode const*>(node)) {}
        // else if (auto n = dynamic_cast<TgtNode const*>(node)) {}
        // else if (auto n = dynamic_cast<TltNode const*>(node)) {}
        // else if (auto n = dynamic_cast<ByteSwapNode const*>(node)) {}
        // else if (auto n = dynamic_cast<RORNode const*>(node)) {}
        // else if (auto n = dynamic_cast<CLZNode const*>(node)) {}
        {
          std::stringstream err;
          err << "Can't compute value of ";
          node->Repr( err );
          std::cerr << err.str() << std::endl;
        }
        throw Prologue::Error();
        return 0;
      }
    };
    
    Registers regs;
    Rules rules;
    
    for (Expr const& expr : mem_addrs)
      GetRule( regs, rules, base_register ).Process( expr.node );
    
    Rule rule = rules.GetBaseRule();
    return Prologue( regs, rule.offset, rule.sign, base_register );
  }

  namespace { template <typename T> int _Cmp( T a, T b ) { return (a < b) ? -1 : (a > b) ? +1 : 0; } }
  
  int
  Interface::cmp( Interface const& b ) const
  {
    if (int delta = _Cmp( iruse.size(), b.iruse.size() )) { return delta; }
    if (int delta = _Cmp( irmap.size(), b.irmap.size() )) { return delta; }
    
    for (unsigned idx = 0; idx < iruse.size(); ++idx) {
      unsigned areg = iruse[idx], breg = b.iruse[idx];
      if (int delta = _Cmp( int( areg == 15 ), int( breg == 15 ) )) { return delta; }
      if (int delta = irmap.at(areg).cmp( b.irmap.at(breg) )) { return delta; }
    }
    if (int delta = xer.cmp( b.xer )) return delta;
    if (int delta = cr.cmp( b.cr )) return delta;
    if (int delta = spefscr.cmp( b.spefscr )) return delta;
    
    return 0; // All equal
  }

  int
  Interface::VirtualRegister::cmp( VirtualRegister const& b ) const
  {
    if (int delta = _Cmp( vindex, b.vindex )) return delta;
    if (int delta = _Cmp( source, b.source )) return delta;
    if (int delta = _Cmp( destination, b.destination )) return delta;
    
    return 0; // All equal
  }
  
  void  CPU::donttest_branch()  { throw ut::Untestable("branch"); }

  void  CPU::donttest_system()  { throw ut::Untestable("system"); }
  
  // void SignedAdd32(U32& result, U8& carry_out, U8& overflow, U8& sign, U32 x, U32 y, U8 carry_in)
  // {
  //   U32 res = x + y + U32(carry_in);
  //   U32 carry31 = ((x & y) | ((res ^ x ^ y) & (x | y)));
  //   carry_out = U8((carry31 >> 31) & U32(1));
  //   result = res;
  //   sign = U8(S32(res) > S32(0));
  //   overflow = U8(((carry31 ^ (res ^ x ^ y)) >> 31) & U32(1));
  // }
  
  inline U32 Mask(U32 mb, U32 me) { return U32(new MaskNode( mb.expr, me.expr )); }

} // end of namespace ut
