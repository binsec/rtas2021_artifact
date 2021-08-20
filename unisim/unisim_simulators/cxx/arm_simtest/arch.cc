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

#include <unisim/component/cxx/processor/arm/execute.hh>
#include <unisim/component/cxx/processor/arm/disasm.hh>
#include <unisim/component/cxx/processor/arm/isa/decode.hh>
#include <testutils.hh>
#include <arch.hh>
#include <sstream>
#include <iostream>
#include <set>

namespace ut
{
  void SourceReg::Repr( std::ostream& sink ) const { sink << unisim::component::cxx::processor::arm::DisasmRegister( reg ); }
  
  void
  Interface::irappend( uint8_t index, bool w, UniqueVId& uvi )
  {
    // We want register operands to be in {0-3,15} and register 15
    // (PC) is only allowed as a source
    if      (index == 15) {
      if (w) throw unisim::component::cxx::processor::arm::isa::Reject();
    }
    else if (index >= 4) {
      throw unisim::component::cxx::processor::arm::isa::Reject();
    }
    
    VirtualRegister& reg = irmap[index];
    if (reg.bad) reg.allocate( uvi );
    reg.source |= not w;
    reg.destination |= w;
    iruse.push_back( index );
  }
  
  void
  Interface::finalize( uint8_t _length )
  {
    length = _length;
    if (not usemem()) return; // done
    
    if (store_addrs.size() > 4) {
      // Incidentally, a stm can store r0, r1, r2, r3 and pc (this is
      // legal from the interface point of view). Unfortunately there
      // is only room for 4 values in the testbench.
      throw Untestable("Too many stores");
    }
    
    struct FinErr {};
    struct
    {
      std::set<unsigned> regs;
      void Process( Expr& expr )
      {
        struct RegRefs
        {
          RegRefs( unsigned _reg ) : reg( _reg ), count(0) {} unsigned reg, count;
          virtual void Process( Expr const& expr )
          {
            auto sreg = dynamic_cast<SourceReg const*>( expr.node );
            if (sreg and sreg->reg == reg) count += 1;
            
            for (unsigned idx = 0, end = expr->SubCount(); idx < end; ++idx)
              Process( expr->GetSub(idx) );
          }
        };
        {
          // No PC relative loads for now
          RegRefs pcrefs(15);
          pcrefs.Process( expr );
          if (pcrefs.count) { throw Untestable("PC relative address."); }
        }
        struct
        {
          std::map<unsigned,unsigned> candidates;
          void Process( Expr const& expr )
          {
            if      (auto n = dynamic_cast<SourceReg const*>(expr.node))
              { candidates[n->reg] += 1; }
            else if (auto n = expr->AsOpNode())
              {
                unisim::util::symbolic::Op const& op = n->op;
                if ((op.code != op.Add) and (op.code != op.Sub))
                  return;
                Process( n->GetSub(0) );
                Process( n->GetSub(1) );
              }
            else
              return;
          }
        } bregs;
        unsigned reg_idx = 16;
        bregs.Process( expr );
        for (auto x : bregs.candidates) {
          if (x.second != 1) continue;
          RegRefs chk( x.first );
          chk.Process( expr );
          if (chk.count != 1) continue;
          reg_idx = x.first;
        }
        if (reg_idx >= 16)
          throw Untestable( "No valid base register." );
        regs.insert( reg_idx );
      }
    } base;
    for (Expr& expr : store_addrs)
      base.Process( expr );
    for (Expr& expr : load_addrs)
      base.Process( expr );
    if (base.regs.size() != 1) {
      std::stringstream err;
      err << "Bad base register count: " << base.regs.size() << '.';
      throw Untestable( err.str() );
    }
    base_register = *(base.regs.begin());
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
    
    struct Variables
    {
      typedef Prologue::Regs Regs;
      Variables() : regs() {}
      Regs regs; std::set<Rule> rules;
      uint32_t operator[] ( unsigned idx )
      {
        Regs::iterator itr = regs.lower_bound( idx );
	// idx is less or equal to itr->first.
	if (itr == regs.end() or (idx < itr->first)) {
          uint32_t value = rand();
          itr = regs.insert( itr, Regs::value_type( idx, value ) );
        }
	return itr->second;
      }
      
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
    };
    
    struct GetRule
    {
      GetRule( Variables& _vars, unsigned _rbase ) : vars( _vars ), rbase( _rbase ), rule() {}
      Variables& vars;
      unsigned   rbase;
      Rule       rule;
      
      void Process( ExprNode const* node )
      {
        if (auto n = dynamic_cast<SourceReg const*>(node))
          {
            // Unless reg is not rbase, we're done !
            if (n->reg != rbase) throw Prologue::Error();
            vars.add( rule ); 
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
            return vars[n->reg];
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
    Variables vars;
    for (Expr const& expr : store_addrs)
      GetRule( vars, base_register ).Process( expr.node );
    for (Expr const& expr : load_addrs)
      GetRule( vars, base_register ).Process( expr.node );
    Rule rule = vars.GetBaseRule();
    return Prologue( vars.regs, rule.offset, rule.sign, base_register );
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
    if (int delta = psr.cmp( b.psr )) return delta;
    
    return 0; // All equal
  }

  int
  VirtualRegister::cmp( VirtualRegister const& b ) const
  {
    if (bad or b.bad) throw 0;
    if (int delta = _Cmp( vindex, b.vindex )) return delta;
    if (int delta = _Cmp( source, b.source )) return delta;
    if (int delta = _Cmp( destination, b.destination )) return delta;
    
    return 0; // All equal
  }
  
  void
  Arch::ITSetState( uint32_t cond, uint32_t mask )
  {
    throw ut::DontTest( "not under test (itstate modified)" );
  }

  void
  Arch::donttest_branch()
  {
    throw ut::DontTest( "not under test (branch)" );
  }

  void
  Arch::donttest_copro()
  {
    throw ut::DontTest( "not under test (coprocessor)" );
  }

  void
  Arch::donttest_system()
  {
    throw ut::DontTest( "not under test (system)" );
  }

  void Arch::reject() { throw unisim::component::cxx::processor::arm::isa::Reject(); }
  
} // end of namespace ut
