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
 
#include <unisim/util/symbolic/binsec/binsec.hh>
#include <ostream>
#include <sstream>
#include <iomanip>

namespace unisim {
namespace util {
namespace symbolic {
namespace binsec {

  std::ostream&
  operator << ( std::ostream& sink, dbx const& _ )
  {
    sink << "0x" << std::hex << std::setw(_.bytes*2) << std::setfill('0') << _.value << std::dec;
    return sink;
  }

  bool
  Label::subst_next( std::string& s, int next )
  {
    uintptr_t pos = s.find( "<next>" );
    if (pos == std::string::npos)
      return false;
    { std::ostringstream buf; buf << next; s.replace(pos, 6, buf.str()); }
    return true;
  }

  Expr
  ASExprNode::Simplify( Expr const& expr )
  {
    unsigned subcount = expr->SubCount();
    
    if (ConstNodeBase const* node = expr->AsConstNode())
      {
        switch (node->GetType())
          {
          case ScalarType::S8:  return make_const( node->Get(  uint8_t() ) );
          case ScalarType::S16: return make_const( node->Get( uint16_t() ) );
          case ScalarType::S32: return make_const( node->Get( uint32_t() ) );
          case ScalarType::S64: return make_const( node->Get( uint64_t() ) );
          default: break;
          }
        return expr;
      }
    else if (OpNodeBase const* node = expr->AsOpNode())
      {
        switch (node->op.code)
          {
          default: break;
            
          case Op::And:
            if (subcount == 2)
              {
                Expr subs[2] = {ASExprNode::Simplify( node->GetSub(0) ), ASExprNode::Simplify( node->GetSub(1) )};
                  
                for (unsigned idx = 0; idx < 2; ++idx)
                  if (ConstNodeBase const* node = subs[idx].Eval(EvalSpace()))
                    {
                      Expr dispose(node);
                      uint64_t v = node->Get( uint64_t() );
                      if (v & (v+1))
                        continue;
                      if (v == 0)
                        return subs[idx];
                      unsigned bitsize = ScalarType(node->GetType()).bitsize, select = arithmetic::BitScanReverse(v)+1;
                      if (select >= bitsize)
                        return subs[idx^1];
                      BitFilter bf( subs[idx^1], bitsize, select, bitsize, false );
                      bf.Retain(); // Prevent deletion of this stack-allocated object
                      Expr res( bf.Simplify() );
                      return (res.node == &bf) ? new BitFilter( bf ) : res.node;
                    }
                
                return (subs[0] != node->GetSub(0)) or (subs[1] != node->GetSub(1)) ? make_operation( node->op.code, subs[0], subs[1] ) : node;
              }
            break;

          case Op::Cast:
            if (subcount == 1)
              {
                Expr sub = ASExprNode::Simplify( node->GetSub(0) );
            
                CastNodeBase const& cnb = dynamic_cast<CastNodeBase const&>( *expr.node );
                ScalarType src( cnb.GetSrcType() ), dst( cnb.GetType() );
                if (not dst.is_integer or not src.is_integer or (dst.bitsize == 1 and src.bitsize != 1))
                  {
                    // Complex casts
                    if (sub == node->GetSub(0))
                      return expr;
                    ExprNode* en = cnb.Mutate();
                    Expr res(en); // Exception safety
                    dynamic_cast<CastNodeBase&>( *en ).src = sub;
                    return res;
                  }

                if (src.bitsize == dst.bitsize)
                  return sub;
                  
                  
                BitFilter bf( sub, src.bitsize, std::min(src.bitsize, dst.bitsize), dst.bitsize, dst.bitsize > src.bitsize ? src.is_signed : false );
                bf.Retain(); // Not a heap-allocated object (never delete);
                Expr res( bf.Simplify() );
                return (res.node == &bf) ? new BitFilter( bf ) : res.node;
              }
            break;
          }
      }
    else if (ASExprNode const* node = dynamic_cast<ASExprNode const*>( expr.node ))
      {
        Expr res = node->Simplify();
        if (res.good())
          return res;
      }
    
    Expr subs[subcount];

    bool simplified = false;
    
    for (unsigned idx = 0; idx < subcount; ++idx)
      {
        Expr const& sub = expr->GetSub(idx);
        if (sub != (subs[idx] = ASExprNode::Simplify( sub )))
          simplified = true;
      }
    
    if (not simplified)
      return expr;

    ExprNode* res = expr->Mutate();

    for (unsigned idx = 0; idx < subcount; ++idx)
      const_cast<Expr&>(res->GetSub(idx)) = subs[idx];
    
    return Expr(res);
  }

  int
  ASExprNode::GenerateCode( Expr const& expr, Variables& vars, Label& label, std::ostream& sink )
  {
    /*** Pre expression process ***/
    Variables::iterator itr = vars.find( expr );
    if (itr != vars.end())
      {
        return static_cast<ASExprNode const*>( itr->second.node ) ->GenCode( label, vars, sink );
      }
    
    /*** Sub expression process ***/
    if (ConstNodeBase const* node = expr.Eval(EvalSpace()))
      {
        Expr dispose( node );
        switch (node->GetType())
          {
          case ScalarType::BOOL: sink << node->Get( int32_t() ) << "<1>";  return 1;
          case ScalarType::U8:  case ScalarType::S8:  sink << dbx(1,node-> Get( uint8_t() ));  return 8;
          case ScalarType::U16: case ScalarType::S16: sink << dbx(2,node->Get( uint16_t() )); return 16;
          case ScalarType::U32: case ScalarType::S32: sink << dbx(4,node->Get( uint32_t() )); return 32;
          case ScalarType::U64: case ScalarType::S64: sink << dbx(8,node->Get( uint64_t() )); return 64;
          default: break;
          }
        throw std::logic_error("can't encode type");
      }
    else if (OpNodeBase const* node = expr->AsOpNode())
      {
        switch (node->SubCount())
          {
          case 2: {
            int retsz = GenerateCode( node->GetSub(0), vars, label, sink << '(' );

            Expr rhs = node->GetSub(1);

            switch (node->op.code)
              {
              default:                sink << " [" << node->op.c_str() << "] "; break;
            
              case Op::Add:     sink << " + "; break;
              case Op::Sub:     sink << " - "; break;
              case Op::Mul:     sink << " * "; break;
        
              case Op::Xor:     sink << " xor "; break;
              case Op::Or:      sink << " or "; break;
              case Op::And:     sink << " and "; break;
        
              case Op::Teq:     sink << " = "; retsz = 1; break;
              case Op::Tne:     sink << " <> "; retsz = 1; break;
        
              case Op::Tle:     sink << " <=s "; retsz = 1; break;
              case Op::Tleu:    sink << " <=u "; retsz = 1; break;
        
              case Op::Tge:     sink << " >=s "; retsz = 1; break;
              case Op::Tgeu:    sink << " >=u "; retsz = 1; break;
        
              case Op::Tlt:     sink << " <s "; retsz = 1; break;
              case Op::Tltu:    sink << " <u "; retsz = 1; break;
        
              case Op::Tgt:     sink << " >s "; retsz = 1; break;
              case Op::Tgtu:    sink << " >u "; retsz = 1; break;

                struct
                {
                  Expr operator() (U8 x, int s)
                  {
                    if (s==16) return Simplify( U16(x).expr );
                    if (s==32) return Simplify( U32(x).expr );
                    if (s==64) return Simplify( U64(x).expr );
                    
                    return x.expr;
                  }
                } fixsh;
                
              case Op::Lsl:     sink << " lshift ";  rhs = fixsh( rhs, retsz ); break;
              case Op::Asr:     sink << " rshifts "; rhs = fixsh( rhs, retsz ); break;
              case Op::Lsr:     sink << " rshiftu "; rhs = fixsh( rhs, retsz ); break;
              case Op::Ror:     sink << " rrotate "; rhs = fixsh( rhs, retsz ); break;
                 
                // case Op::Mod: break;
                // case Op::Div: break;
              }

            sink << GetCode( rhs, vars, label ) << ')';
            return retsz;
          }
            
          case 1: {
            char const* operation = 0;
        
            switch (node->op.code)
              {
              default:              operation = node->op.c_str(); break;
        
              case Op::Not:    operation = "not "; break;
              case Op::Neg:    operation = "- "; break;
        
                // case Op::BSwp:  break;
                // case Op::BSF:   break;
            
              case Op::BSR:
                {
                  Label head(label);
                  int exit = label.allocate(), loop; 
                  {
                    std::ostringstream buffer;
                    buffer << "bsr_in<32> := " << GetCode(node->GetSub(0), vars, head) << " ; goto <next>";
                    head.write( buffer.str() );
                    head.write( "bsr_out<32> := 32<32> ; goto <next>" );
                  }
                  {
                    std::ostringstream buffer;
                    buffer << "if (bsr_in<32> = 0<32>) goto " << exit << " else goto <next>";
                    head.write( buffer.str() );
                    loop = head.write( "bsr_out<32> := bsr_out<32> - 1<32> ; goto <next>" );
                  }
                  {
                    std::ostringstream buffer;
                    buffer << "if ((bsr_in<32> rshiftu bsr_out<32>){0,0}) goto " << exit << " else goto " << loop;
                    head.write( buffer.str() );
                  }
      
                  sink << "bsr_out<32>";
      
                  return 32;
                }
              case Op::Cast:
                {
                  CastNodeBase const& cnb = dynamic_cast<CastNodeBase const&>( *expr.node );
                  ScalarType src( cnb.GetSrcType() ), dst( cnb.GetType() );
                  
                  if (dst.is_integer and src.is_integer)
                    {
                      /* At this point, only boolean casts should remain */
                      if ((src.bitsize <= 1) or (dst.bitsize != 1))
                        throw std::logic_error("Unexpected cast");
                      
                      sink << "(" << GetCode(cnb.src, vars, label) << " <> " << dbx(src.bitsize/8,0) << ")";
                    }
                  else
                    {
                      /* TODO: What to do with FP casts ? */
                      sink << dst.name << "( " << GetCode(cnb.src, vars, label) << " )";
                    }
      
                  return dst.bitsize;
                }
              }
    
            sink << '(' << operation << ' ';
            int retsz = GenerateCode( node->GetSub(0), vars, label, sink );
            sink << ')';
            return retsz;
          }
          default:
            break;
          }
      }
    else if (ASExprNode const* node = dynamic_cast<ASExprNode const*>( expr.node ))
      {
        return node->GenCode( label, vars, sink );
      }
      
    throw std::logic_error("No GenCode method");
    return 0;
  }

  int
  BitFilter::GenCode( Label& label, Variables& vars, std::ostream& sink ) const
  {
    if (extend == source)
      {
        /* TODO: check GetCode bitsize == extend == source */
        sink << "(" << GetCode(input, vars, label) << " and " << dbx(source/8, (1ull << select)-1) << ")";
        return extend;
      }
    bool extension = extend > select;
    bool selection = source > select;
    if (extension)
      sink << '(' << (sxtend ? "exts " : "extu ");
    if (selection)
      sink << '(';

    int chksize = ASExprNode::GenerateCode( input, vars, label, sink );
    if (chksize != source) throw 0;
    
    if (selection)
        sink << " {0," << (select-1) << "})";
    if (extension)
      sink << ' ' << extend << ')';
    
    return extend;
  }
  
  void
  BitFilter::Repr( std::ostream& sink ) const
  {
    sink << "BitFilter(" << input
         << ", " << source
         << ", " << select
         << ", " << extend
         << ", " << (sxtend ? "signed" : "unsigned")
         << ")";
  }
  
  int
  RegRead::GenCode( Label& label, Variables& vars, std::ostream& sink ) const
  {
    unsigned bitsize = ScalarType(GetType()).bitsize;
    GetRegName( sink );
    sink << "<" << bitsize << ">";
    return bitsize;
  }
  
  void
  RegRead::Repr( std::ostream& sink ) const
  {
    GetRegName( sink );
  }
  
  int
  RegWrite::GenCode( Label& label, Variables& vars, std::ostream& sink ) const
  {
    GetRegName( sink );
    sink << "<" << std::dec << ScalarType(value->GetType()).bitsize << "> := " << GetCode(value, vars, label);
    return 0;
  }
  
  void RegWrite::Repr( std::ostream& sink ) const { GetRegName( sink ); sink << " := "; value->Repr(sink); }
  
  int
  Load::GenCode( Label& label, Variables& vars, std::ostream& sink ) const
  {
    /* TODO: exploit alignment info */
    sink << "@[" << GetCode(addr, vars, label) << ',' << (bigendian?"->":"<-") << ',' << bytecount() << "]";
    return 8*bytecount();
  }

  void
  Load::Repr( std::ostream& sink ) const
  {
    sink << "[" << addr << ',' << bytecount() << ",^" << alignment << ',' << (bigendian ? "be" : "le") << "]";
  }
  
  int
  Store::GenCode( Label& label, Variables& vars, std::ostream& sink ) const
  {
    /* TODO: exploit alignment info */
    sink << "@[" << GetCode(addr, vars, label) << ',' << (bigendian?"->":"<-") << ',' << bytecount() << "] := " << GetCode(value, vars, label);
    return 0;
  }

  void
  Store::Repr( std::ostream& sink ) const
  {
    sink << "[" << addr << ',' << size << ",^" << alignment << ',' << (bigendian ? "be" : "le") << "] := " << value;
  }

  namespace {
    struct SinksMerger { void operator () ( std::set<Expr>& sinks, Expr const& l, Expr const& r ) { sinks.insert( l ); } };
  }

  void
  ActionNode::simplify()
  {
    while (cond.good())
      {
        cond = ASExprNode::Simplify( cond );
        if (ConstNodeBase const* c = cond.ConstSimplify())
          {
            //            std::cerr << "warning: post simplification of if-then-else condition\n";
            bool eval = c->Get(bool());
            ActionNode* always = nexts[eval];
            ActionNode* never = nexts[not eval];
            delete never;
            // Import always branch
            for (unsigned idx = 0; idx < 2; ++idx)
              {
                nexts[idx] = always->nexts[idx];
                always->nexts[idx] = 0;
              }
            cond = always->cond;
            sinks.insert(always->sinks.begin(), always->sinks.end());
            delete always;
          }
        else
          break;
      }
    
    {
      std::set<Expr> nsinks;
      for (std::set<Expr>::const_iterator itr = sinks.begin(), end = sinks.end(); itr != end; ++itr)
        {
          Expr x = ASExprNode::Simplify( *itr );
          x.ConstSimplify();
          nsinks.insert( x );
        }
      std::swap(nsinks, sinks);
    }
    
    if (not cond.good())
      return;
    
    for (unsigned choice = 0; choice < 2; ++choice)
      if (ActionNode* next = nexts[choice])
        next->simplify();
    
    factorize( sinks, nexts[0]->sinks, nexts[1]->sinks, SinksMerger() );
    
    bool leaf = true;
    for (unsigned choice = 0; choice < 2; ++choice)
      if (ActionNode* next = nexts[choice])
        {
          if (next->cond.good() or next->sinks.size()) leaf = false;
          else { delete next; nexts[choice] = 0; }
        }
    
    if (leaf)
      cond = Expr();
    else if (OpNodeBase const* onb = cond->AsOpNode())
      if (onb->op.code == onb->op.Not)
        {
          // If condition begins with a logical not, remove the not and
          //   swap if then else branches
          cond = onb->GetSub(0);
          std::swap( nexts[false], nexts[true] );
        }
      
  }

  void
  ActionNode::commit_stats()
  {
    struct Sec
    {
      void Flood( Expr const& e )
      {
        for (unsigned idx = 0, end = e->SubCount(); idx < end; ++idx)
          this->Process( e->GetSub(idx) );
      }
      void Process( Expr const& e )
      {
        if (not e->SubCount())
          return;
        bool cont = true;
        for (Sec* sec = this; sec; sec = sec->up)
          cont &= (sec->stats[e]++ == 0);
        if (cont)
          this->Flood( e );
      }
      Sec( ActionNode* node, Sec* _up )
        : stats(node->sestats), up(_up)
      {
        // First level of expression is not functionnal (architectual side effect)
        for (std::set<Expr>::const_iterator itr = node->sinks.begin(), end = node->sinks.end(); itr != end; ++itr)
          this->Flood( *itr );
        for (unsigned choice = 0; choice < 2; ++choice)
          if (ActionNode* next = node->nexts[choice])
            { Sec sub(next,this); }
      }
      std::map<Expr,unsigned>& stats; Sec* up;
    } sec(this,0);
  }
  
  void
  Program::Generate( ActionNode const* action_tree )
  {
    struct Context
    {
      Context() : upper(0) {}
      Context( Context* _up ) : upper(_up)
      {
        if (_up) next_tmp = _up->next_tmp;
        if (_up) vars = _up->vars;
      }
      
      void add_pending( Expr e ) { pendings.push_back(e); }
      bool has_pending() const { return pendings.size() > 0; }
      
      Expr const& addvar( std::string const& name, unsigned size, Expr const& expr )
      {
        Expr& place = vars[expr];
      
        if (place.good())
          throw std::logic_error( "multiple temporary definitions" );
      
        struct TmpVar : public ASExprNode
        {
          TmpVar( std::string const& _ref, unsigned rsz )
            : ref(_ref), dsz(rsz)
          {}
          virtual TmpVar* Mutate() const { return new TmpVar(*this); }
          virtual int GenCode( Label& label, Variables& vars, std::ostream& sink ) const { sink << ref; return dsz; }
          virtual ScalarType::id_t GetType() const { return ScalarType::IntegerType(false, dsz); }
          virtual int cmp( ExprNode const& rhs ) const override { return ref.compare( dynamic_cast<TmpVar const&>( rhs ).ref ); }
          virtual unsigned SubCount() const { return 0; }
          virtual void Repr( std::ostream& sink ) const { sink << ref; }
          virtual Expr Simplify() const { return 0; }
          std::string ref;
          int dsz;
        };
      
        place = new TmpVar( name, size );
        return place;
      }      
      std::string nxtname( RegWrite const* rw, unsigned rsize ) { std::ostringstream buf; rw->GetRegName( buf << "nxt_" ); buf << "<" << rsize << ">"; return buf.str(); }
      std::string tmpname( unsigned size ) { std::ostringstream buffer; buffer << "tmp" << size << '_' << (next_tmp[size]++) << "<" << size << ">"; return buffer.str(); }

      void GenCode( ActionNode const* action_tree, Label const& start, Label const& after )
      {
        Branch const* nia = 0;
    
        // Using a delayed writing mechanism so that the last code line
        // produced is linked to the next code line given by the upper
        // context instead of a fresh new one
        struct Head
        {
          Head( Label const& _start, int _after ) : cur(_start), after(_after) {}
          ~Head()
          {
            if (not pending.size()) return;
            Label::subst_next(pending, after);
            cur.write( pending );
            pending.clear();
          }

          Label& current() { flush(); return cur; }
          void write( std::string const& s )
          {
            flush();
            if (s.size()==0) throw std::runtime_error("xxx");
            pending = s;
          }
          void flush()
          {
            if (not pending.size()) return;
            cur.write( pending );
            pending.clear();
          }
          Label cur;
          int after;
          std::string pending;
        } head( start, after.GetID() );

        {
          // Keeping track of expressions involved in register
          // assignment. Their temporary name will differ from regular
          // temporary.
          std::map<Expr,RegWrite const*> rtmps;
          for (std::set<Expr>::const_iterator itr = action_tree->sinks.begin(), end = action_tree->sinks.end(); itr != end; ++itr)
            {
              if (RegWrite const* rw = dynamic_cast<RegWrite const*>( itr->node ))
                rtmps[rw->value] = rw;
            }
          
          // Ordering Sub Expressions by size of expressions (so that
          // smaller expressions are factorized in larger ones)
          struct CSE : public std::multimap<unsigned,Expr>
          {
            void Process( Expr const& expr )
            {
              insert( std::make_pair( CountSubs( expr ), expr ) );
            }
            unsigned CountSubs( Expr const& expr )
            {
              unsigned sum = 1;
              for (unsigned idx = 0, end = expr->SubCount(); idx < end; ++idx)
                sum += CountSubs( expr->GetSub(idx) );
              return sum;
            }
          } cse;

          for (std::map<Expr,unsigned>::const_iterator itr = action_tree->sestats.begin(), end = action_tree->sestats.end(); itr != end; ++itr)
            {
              if (itr->second < 2)
                continue; // No reuse
              if (this->vars.count(itr->first))
                continue; // Already defined
              cse.Process(itr->first);
            }

          for (std::multimap<unsigned,Expr>::const_iterator itr = cse.begin(), end = cse.end(); itr != end; ++itr)
            {
              std::string tmp_src, tmp_dst;
              int retsize;
              {
                std::ostringstream buffer;
                retsize = ASExprNode::GenerateCode( itr->second, this->vars, head.current(), buffer );
                tmp_src = buffer.str();
                std::map<Expr,RegWrite const*>::const_iterator rtmp = rtmps.find(itr->second);
                tmp_dst = rtmp != rtmps.end() ? this->nxtname( rtmp->second, retsize ) : this->tmpname( retsize );
                this->addvar( tmp_dst, retsize, itr->second );
              }
              {
                std::ostringstream buffer;
                buffer << tmp_dst << " := " << tmp_src << "; goto <next>";
                head.write( buffer.str() );
              }
            }
        }

        for (std::set<Expr>::const_iterator itr = action_tree->sinks.begin(), end = action_tree->sinks.end(); itr != end; ++itr)
          {
            if (RegWrite const* rw = dynamic_cast<RegWrite const*>( itr->node ))
              {
                Expr const  value = rw->value;
                unsigned    rsize = ScalarType(value->GetType()).bitsize;

                if (not value->AsConstNode() and not this->vars.count(value))
                  {
                    std::string vname = this->nxtname( rw, rsize );
                    std::ostringstream buffer;
                    buffer << vname << " := " << GetCode(value, this->vars, head.current()) << "; goto <next>";
                    head.write( buffer.str() );
                    this->addvar( vname, rsize, value );
                  }

                if (Branch const* branch = dynamic_cast<Branch const*>( rw ))
                  { nia = branch; }
                else
                  { this->add_pending( *itr ); }
              }
            else
              {
                std::ostringstream buffer;
                if (ASExprNode::GenerateCode( *itr, this->vars, head.current(), buffer ))
                  throw 0;

                buffer << "; goto <next>";
                head.write( buffer.str() );
              }
          }

        if (not action_tree->cond.good())
          {
            if (action_tree->sinks.size() == 0)
              throw std::logic_error( "empty leaf" );
          }
        else
          {
            std::ostringstream buffer;
            Label ifinsn(head.current());
            buffer << "if " << GetCode(action_tree->cond, this->vars, ifinsn) << " ";

            // Preparing room for if then else code
            Label endif( after );
            if (nia or (after.valid() and (this->has_pending())))
              endif.allocate();

            if (not action_tree->nexts[0]) {
              Label ifthen(ifinsn);
              buffer << " goto " << ifthen.allocate() << " else goto " << endif.GetID();
              ifinsn.write( buffer.str() );
              Context nxt(this);
              nxt.GenCode( action_tree->nexts[1], ifthen, endif );
            } else if (not action_tree->nexts[1]) {
              Label ifelse(ifinsn);
              buffer << " goto " << endif.GetID() << " else goto " << ifelse.allocate();
              ifinsn.write( buffer.str() );
              Context nxt(this);
              nxt.GenCode( action_tree->nexts[0], ifelse, endif );
            } else {
              Label ifthen(ifinsn), ifelse(ifinsn);
              buffer << " goto " << ifthen.allocate() << " else goto " << ifelse.allocate();
              ifinsn.write( buffer.str() );
              {
                Context nxt(this);
                nxt.GenCode( action_tree->nexts[1], ifthen, endif );
              }
              {
                Context nxt(this);
                nxt.GenCode( action_tree->nexts[0], ifelse, endif );
              }
            }

            head.current() = endif;
          }

        for (Pendings::iterator itr = this->pendings.begin(), end = this->pendings.end(); itr != end; ++itr)
          {
            std::ostringstream buffer;
            if (ASExprNode::GenerateCode( *itr, this->vars, head.current(), buffer ))
              throw 0;

            buffer << "; goto <next>";
            head.write( buffer.str() );
          }

        if (not nia)
          return;

        Label current( head.current() );

        for (Context* uc = this->upper; uc; uc = uc->upper)
          {
            for (Pendings::iterator itr = uc->pendings.begin(), end = uc->pendings.end(); itr != end; ++itr)
              {
                std::ostringstream buffer;
                if (ASExprNode::GenerateCode( *itr, this->vars, current, buffer ))
                  throw 0;

                buffer << "; goto <next>";
                current.write( buffer.str() );
              }
          }

        std::ostringstream buffer;
        Expr const& target = nia->value;
        buffer << "goto (" << GetCode(target, this->vars, current) << (target->AsConstNode() ? ",0" : "") << ")";
        nia->annotate( buffer );
        current.write( buffer.str() );
      }

      Context* upper;
      typedef std::vector<Expr> Pendings;
      Pendings pendings;
      Variables vars;
      std::map<unsigned,unsigned> next_tmp;
    };
  
    Label beglabel(*this), endlabel(*this);
    beglabel.allocate();
    Context ctx;
    ctx.GenCode( action_tree, beglabel, endlabel );
  }
  

} /* end of namespace binsec */
} /* end of namespace symbolic */
} /* end of namespace util */
} /* end of namespace unisim */

