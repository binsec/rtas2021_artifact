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

#include <arch.hh>
#include <top_ppc64.hh>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
// #include <set>

namespace ut
{
  void SourceReg::Repr( std::ostream& sink ) const { sink << 'r' << unsigned( reg ); }
  void Arch::LoadRepr( std::ostream& sink, Expr const& _addr, unsigned bits ) { sink << "Load<"<<bits<<">( " << _addr << " )"; }
  void XER::XERNode::Repr( std::ostream& sink ) const { sink << "XER"; }
  void CR::CRNode::Repr( std::ostream& sink ) const { sink << "CR"; }
  void FPSCR::FPSCRNode::Repr( std::ostream& sink ) const { sink << "FPSCR"; }
  
  Interface::Interface( e5500::Operation& op )
    : xer(0)
    , cr(0)
    , fpscr(0)
    , base_register(-1)
    , aligned(false)
    , mem_writes(false)
    , length(op.GetLength())
    , retfalse(false)
  {
    bool has_valid_path = false;
    for (ActionNode path_root;;)
      {
        Arch cpu( *this, path_root );
        
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
            unsigned reg  = (1u << n->reg);
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

  void
  Interface::Prologue::Resolve(Interface const& iif)
  {
    struct Rule
    {
      Rule() : offset(0), sign(false) {} Offset offset; bool sign;
      void rsub( Offset value ) { offset = value - offset; sign = not sign; }
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
          for (Offset lo = rules.begin()->offset, hi = itr->offset; (hi - lo) >= 16;) {
            if (++itr == end) throw Prologue::Error();
            lo = hi; hi = itr->offset;
          }
          return *itr;
        } /* else */
        auto itr = rules.begin(), end = rules.end();
        for (Offset lo = itr->offset, hi = rules.rbegin()->offset; (hi - lo) >= 16;) {
          if (++itr == end) throw Prologue::Error();
          hi = lo; lo = itr->offset;
        }
        return *itr;
      }

      std::set<Rule> rules;
    };

    static struct 
    {
      Offset operator()(Regs& regs, unsigned idx) const
      {
        auto itr = regs.lower_bound( idx );
	// idx is less or equal to itr->first.
	if (itr == regs.end() or (idx < itr->first))
          {
            /* ppc li's immediate is 16 bit signed */
            Offset value = int16_t(rand());
            itr = regs.insert( itr, Prologue::Regs::value_type( idx, value ) );
          }
	return itr->second;
      }
    } eval;
    
    struct GetRule
    {
      GetRule( Regs& _regs, Rules& _rules, unsigned _rbase ) : regs( _regs ), rules(_rules), rbase( _rbase ), rule() {}
      Regs&      regs;
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
      
      Offset GetValue( ExprNode const* node )
      {
        if (auto n = dynamic_cast<SourceReg const*>(node))
          {
            if (n->reg == rbase) throw Prologue::Error();
            return eval(regs, n->reg);
          }
        
        if (auto n = dynamic_cast<unisim::util::symbolic::ConstNode<uint64_t> const*>(node))
          return Offset( n->value );
        // if (auto n = dynamic_cast<unisim::util::symbolic::ConstNode<int64_t> const*>(node))
        //   return n->value;
        // if (auto n = dynamic_cast<unisim::util::symbolic::CastNode<int32_t,uint32_t> const*>(node))
        //   return GetValue( n->src.node );
        // if (auto n = dynamic_cast<unisim::util::symbolic::CastNode<uint32_t,int32_t> const*>(node))
        //   return GetValue( n->src.node );

        if (auto n = node->AsOpNode())
          {
            Offset lval( GetValue( n->GetSub(0).node ) );
            Offset rval( n->SubCount() > 1 ? GetValue( n->GetSub(1).node ) : Offset( 0 ) );
            using unisim::util::symbolic::Op;
            switch (n->op.code)
              {
              case Op::Add: return Offset( lval + rval );
              case Op::Sub: return Offset( lval - rval );
              case Op::Lsl: return Offset( lval << rval );
              case Op::Lsr: return Offset( lval >> rval );
              case Op::Or:  return Offset( lval | rval );
              case Op::And: return Offset( lval & rval );
              case Op::Asr: return Offset( int64_t(lval) >> rval );
              case Op::Neg: return Offset( -lval );
              case Op::Not: return Offset( ~lval );
              default: break;
              }
          }
        
        std::cerr << "Can't compute value of " << Expr(node) << std::endl;
        
        throw Prologue::Error();
        return Offset( 0 );
      }
    };

    Rules rules;
    for (Expr const& expr : iif.mem_addrs)
      GetRule( regs, rules, iif.base_register ).Process( expr.node );
    
    Rule rule = rules.GetBaseRule();
    offset = rule.offset;
    sign = rule.sign;
  }

  int
  Interface::cmp( Interface const& b ) const
  {
    if (int delta = iregs.cmp( b.iregs )) return delta;
    if (int delta = fregs.cmp( b.fregs )) return delta;
        
    if (int delta = xer.cmp( b.xer )) return delta;
    if (int delta = cr.cmp( b.cr )) return delta;
    if (int delta = fpscr.cmp( b.fpscr )) return delta;
    
    return 0; // All equal
  }

  void
  Interface::RegBank::append( uint8_t index, bool w )
  {
    refs.push_back( index );
    auto itr = vmap.find(index);
    if ((itr == vmap.end()) or (index < itr->first))
      itr = vmap.emplace_hint( itr, std::piecewise_construct, std::forward_as_tuple( index ), std::forward_as_tuple( vmap.size() ) );
    VirtualRegister& reg = itr->second;
    reg.source |= not w;
    reg.destination |= w;
  }
  
namespace { template <typename T> int _Cmp( T a, T b ) { return (a < b) ? -1 : (a > b) ? +1 : 0; } }

  int
  Interface::RegBank::cmp( Interface::RegBank const& b ) const
  {
    if (int delta = _Cmp( refs.size(), b.refs.size() )) { return delta; }
    if (int delta = _Cmp( vmap.size(), b.vmap.size() )) { return delta; }
    
    for (unsigned idx = 0; idx < refs.size(); ++idx)
      {
        if (int delta = vmap.at(refs[idx]).cmp( b.vmap.at(b.refs[idx]) ))
          return delta;
      }
    return 0;
  }
  
  int
  Interface::VirtualRegister::cmp( VirtualRegister const& b ) const
  {
    if (int delta = _Cmp( vindex, b.vindex )) return delta;
    if (int delta = _Cmp( source, b.source )) return delta;
    if (int delta = _Cmp( destination, b.destination )) return delta;
    
    return 0; // All equal
  }
  
  void  Arch::donttest_branch()  { throw ut::Untestable("branch"); }

  void  Arch::donttest_system()  { throw ut::Untestable("system"); }

} //end of namespace ut
