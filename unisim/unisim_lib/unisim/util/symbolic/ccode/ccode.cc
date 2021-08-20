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
 
#include <unisim/util/symbolic/ccode/ccode.hh>
#include <ostream>
#include <sstream>
#include <iomanip>

namespace unisim {
namespace util {
namespace symbolic {
namespace ccode {
  
  void
  ActionNode::Repr( std::ostream& sink ) const
  {
    sink << "ActionNode\n{";
    for (auto && update : updates)
      update->Repr(sink << '\n');
    for (unsigned choice = 0; choice < 2; ++choice)
      if (ActionNode* next = nexts[choice])
        next->Repr( sink << "\n  nexts[" << choice << "]: " );
    sink << "\n}\n";
  }
  
  namespace {
    struct SinksMerger
    {
      typedef unisim::util::symbolic::Expr Expr;
      void operator () ( std::set<Expr>& sinks, Expr const& l, Expr const& r ) { sinks.insert( l ); }
    };
  }  

  void
  ActionNode::simplify()
  {
    if (not cond.good())
      return;

    for (unsigned choice = 0; choice < 2; ++choice)
      if (ActionNode* next = nexts[choice])
        next->simplify();

    if (nexts[0] and nexts[1])
      factorize( updates, nexts[0]->updates, nexts[1]->updates, SinksMerger() );

    bool leaf = true;
    for (unsigned choice = 0; choice < 2; ++choice)
      if (ActionNode* next = nexts[choice])
        {
          if (next->cond.good() or next->updates.size()) leaf = false;
          else { delete next; nexts[choice] = 0; }
        }
    
    if (leaf)
      {
        cond = unisim::util::symbolic::Expr();
        return;
      }

    if (unisim::util::symbolic::OpNodeBase const* onb = cond->AsOpNode())
      if ((onb->op.code == onb->op.Not) and (nexts[false]))
        {
          // If condition begins with a logical not, remove the not and
          //   swap if then else branches
          cond = onb->GetSub(0);
          std::swap( nexts[false], nexts[true] );
        }

    if (not nexts[true])
      {
        cond = unisim::util::symbolic::make_operation("not", cond);
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
        // First level of expression is not functionnal (architectural side effect)
        for (auto && update : node->updates)
          this->Flood( update );
        for (unsigned choice = 0; choice < 2; ++choice)
          if (ActionNode* next = node->nexts[choice])
            { Sec sub(next,this); }
      }
      std::map<Expr,unsigned>& stats; Sec* up;
    } sec(this,0);
  }

  void
  SrcMgr::putchar( char _ch )
  {
    if      (_ch == '\0')
      { throw TextFormatingError; }
    
    else if (_ch == '\n')
      {
        // Unconditionally skipping last blank characters
        blank.clear();
        {
          // Processing local indentation
          int local_indent = indent;
          if (line.length() > 0) {
            if (line[0] == '}' or *(line.end()-1) == ':') local_indent -= 2;
            else if (line[0] == '#')                       local_indent = 0;
          }
          if (local_indent < 0) throw IndentError;
          while (--local_indent >= 0) blank += ' ';
        }
        // Computing global indentation
        for( char const* ptr = line.c_str(); *ptr != '\0'; ptr++ ) {
          switch( *ptr ) {
          case '{': indent += 2; break;
          case '}': indent -= 2; break;
          }
        }
        if (indent < 0) throw IndentError;
    
        // writing line (with indentation)
        sink << blank << line << '\n';
        blank.clear();
        line.clear();
      }

    else if (_ch <= ' ')
      {
        blank += _ch;
      }
    
    else
      {
        if (blank.length())
          {
            // Inserting accumulated blank characters if not at the beginning of the line
            if (line.length())
              line += blank;
            blank.clear();
          }
        line += _ch;
      }
  }

  SrcMgr& operator << (SrcMgr& s, fpt f)
  {
    std::ostringstream buf;
    buf << f.value;
    s << buf.str();
    return s;
  }
  
  char const*
  Variable::GetType() const
  {
    char tp = name[0];
    if (tp == 's')
      tp = name[1];
    
    switch (tp)
      {
      case 'l': return "bool";
      case 'b': return "uint8_t";
      case 'w': return "uint16_t";
      case 'd': return "uint32_t";
      case 'q': return "uint64_t";
      case 'f': return "float";
      case 'g': return "double";
        
      }
    char const* inv = "invalid type";
    throw inv;
    return inv;
  }

  SrcMgr&
  operator << (SrcMgr& srcmgr, Variables const& vars)
  {
    char const* last_type = 0;
    char const* sep = "";
    
    for (auto && var : vars)
      {
        char const* that_type = var.GetType();
        if (that_type != last_type)
          {
            srcmgr << sep << var.GetType();
            last_type = that_type;
            sep = ";\n";
          }
        else
          srcmgr << ',';
        srcmgr << ' ' << var.GetName() <<  "=0";
      }
    srcmgr << sep;
    
    return srcmgr;
  }
  
  SrcMgr& operator << (SrcMgr& srcmgr, CCode::ExprPrinter const& ep)
  {
    using unisim::util::symbolic::ConstNodeBase;
    using unisim::util::symbolic::CastNodeBase;
    using unisim::util::symbolic::OpNodeBase;
    using unisim::util::symbolic::Op;
    using unisim::util::symbolic::Expr;
    using unisim::util::symbolic::ScalarType;

    /*** Pre expression process ***/
    {
      auto itr = ep.ccode.tmps.find( ep.expr );
      if (itr != ep.ccode.tmps.end())
        {
          srcmgr << itr->second.GetName();
          return srcmgr;
        }
    }
    
    /*** Sub expression process ***/
    if (ConstNodeBase const* node = ep.expr.Eval(unisim::util::symbolic::EvalSpace()))
      {
        Expr cexp( node );
        switch (node->GetType())
          {
          case ScalarType::BOOL: srcmgr << (node->Get( uint8_t() ) ? "true" : "false"); break;
          case ScalarType::U8:   srcmgr << hex( node->Get( uint8_t() ) );         break;
          case ScalarType::U16:  srcmgr << hex( node->Get( uint16_t() ) );        break;
          case ScalarType::U32:  srcmgr << hex( node->Get( uint32_t() ) );        break;
          case ScalarType::U64:  srcmgr << hex( node->Get( uint64_t() ) );        break;
          case ScalarType::S8:   srcmgr << dec( node->Get( int8_t() ) );         break;
          case ScalarType::S16:  srcmgr << dec( node->Get( int16_t() ) );        break;
          case ScalarType::S32:  srcmgr << dec( node->Get( int32_t() ) );        break;
          case ScalarType::S64:  srcmgr << dec( node->Get( int64_t() ) );        break;
          case ScalarType::F32:
          case ScalarType::F64:  srcmgr << fpt( node->Get( double() ) );      break;
          default: throw std::logic_error("can't encode type");
          }
        return srcmgr;
      }
    else if (OpNodeBase const* node = ep.expr->AsOpNode())
      {
        switch (node->SubCount())
          {
          case 2: {
            Expr const& lhs(node->GetSub(0));
            Expr const& rhs(node->GetSub(1));
            
            struct
            {
              void operator () (SrcMgr& srcmgr, CCode& ccode, Expr const& lhs, Expr const& rhs, char const* op )
              { srcmgr << '(' << ccode(lhs) << op << ccode(rhs) << ')';  }
            } cbinop;

            switch (node->op.code)
              {
              default:                srcmgr << '(' << ep.ccode(lhs) << " [" << node->op.c_str() << "] " << ep.ccode(rhs) << ')'; break;
            
              case Op::Add:     cbinop( srcmgr, ep.ccode, lhs, rhs, " + " ); break;
              case Op::Sub:     cbinop( srcmgr, ep.ccode, lhs, rhs, " - " ); break;
              case Op::Mul:     cbinop( srcmgr, ep.ccode, lhs, rhs, " * " ); break;
              case Op::Div:     cbinop( srcmgr, ep.ccode, lhs, rhs, " / " ); break;
              case Op::Mod:     cbinop( srcmgr, ep.ccode, lhs, rhs, " % " ); break;
        
              case Op::Xor:     cbinop( srcmgr, ep.ccode, lhs, rhs, " xor " ); break;
              case Op::Or:      cbinop( srcmgr, ep.ccode, lhs, rhs, (lhs->GetType() == ScalarType::BOOL) ? " or " : " | " ); break;
              case Op::And:     cbinop( srcmgr, ep.ccode, lhs, rhs, (lhs->GetType() == ScalarType::BOOL) ? " and " : " & " ); break;
        
              case Op::Teq:     cbinop( srcmgr, ep.ccode, lhs, rhs, " == " ); break;
              case Op::Tne:     cbinop( srcmgr, ep.ccode, lhs, rhs, " != " ); break;
        
              case Op::Tle:     cbinop( srcmgr, ep.ccode, lhs, rhs, " <= " ); break;
              case Op::Tleu:    cbinop( srcmgr, ep.ccode, lhs, rhs, " <= " ); break;
        
              case Op::Tge:     cbinop( srcmgr, ep.ccode, lhs, rhs, " >= " ); break;
              case Op::Tgeu:    cbinop( srcmgr, ep.ccode, lhs, rhs, " >= " ); break;
        
              case Op::Tlt:     cbinop( srcmgr, ep.ccode, lhs, rhs, " < " ); break;
              case Op::Tltu:    cbinop( srcmgr, ep.ccode, lhs, rhs, " < " ); break;
        
              case Op::Tgt:     cbinop( srcmgr, ep.ccode, lhs, rhs, " > " ); break;
              case Op::Tgtu:    cbinop( srcmgr, ep.ccode, lhs, rhs, " > " ); break;

              case Op::Lsl:     cbinop( srcmgr, ep.ccode, lhs, rhs, " << " ); break;
              case Op::Asr:     cbinop( srcmgr, ep.ccode, lhs, rhs, " >> " ); break;
              case Op::Lsr:     cbinop( srcmgr, ep.ccode, lhs, rhs, " >> " ); break;
              case Op::Ror:     srcmgr << "Rotate( " << ep.ccode(lhs) << ", " << ep.ccode(rhs) << " )"; break;
              }
          } break;
            
          case 1: {
            Expr operand( node->GetSub(0) );
            
            switch (node->op.code)
              {
              default:         srcmgr << "([" << node->op.c_str() << "] " << ep.ccode(operand) << ")"; break;
        
              case Op::Not:    srcmgr << (operand->GetType() == ScalarType::BOOL ? "(not " : "(~") << ep.ccode(operand) << ")"; break;
              case Op::Neg:    srcmgr << "(-" << ep.ccode(operand) << ")"; break;
        
                // case Op::BSwp:  break;
                // case Op::BSF:   break;
            
              case Op::BSR:
                srcmgr << "BitScanReverse( " << ep.ccode(operand) << " )";
                break;
                
              case Op::Cast:
                {
                  CastNodeBase const& cnb = dynamic_cast<CastNodeBase const&>( *ep.expr.node );
                  ScalarType src( cnb.GetSrcType() ), dst( cnb.GetType() );

                  char const* tpname = 0;
                  switch (cnb.GetType())
                    {
                    case ScalarType::BOOL: tpname = "bool"; break;
                    case ScalarType::U8:   tpname = "uint8_t"; break;
                    case ScalarType::U16:  tpname = "uint16_t"; break;
                    case ScalarType::U32:  tpname = "uint32_t"; break;
                    case ScalarType::U64:  tpname = "uint64_t"; break;
                    case ScalarType::S8:   tpname = "int8_t"; break;
                    case ScalarType::S16:  tpname = "int16_t"; break;
                    case ScalarType::S32:  tpname = "int32_t"; break;
                    case ScalarType::S64:  tpname = "int64_t"; break;
                    default: throw std::logic_error("cast error");
                    }
                  srcmgr << tpname << "(" << ep.ccode(operand) << ")";
                } break;
              }
          } break;
            
          default: throw std::logic_error("cast error");
          }
      }
    else if (CNode const* node = dynamic_cast<CNode const*>( ep.expr.node ))
      {
        node->translate( srcmgr, ep.ccode );
      }
    else
      throw std::logic_error("No C generation available");
      
    return srcmgr;
  }

  void
  CCode::make_temp( SrcMgr& sink, Expr const& e )
  {
    using unisim::util::symbolic::ScalarType;
    char const* prefix;
    switch (e->GetType())
      {
      case ScalarType::BOOL: prefix = "ltmp"; break;
      case ScalarType::U8:   prefix = "btmp"; break;
      case ScalarType::U16:  prefix = "wtmp"; break;
      case ScalarType::U32:  prefix = "dtmp"; break;
      case ScalarType::U64:  prefix = "qtmp"; break;
      case ScalarType::S8:   prefix = "sbtmp"; break;
      case ScalarType::S16:  prefix = "swtmp"; break;
      case ScalarType::S32:  prefix = "sdtmp"; break;
      case ScalarType::S64:  prefix = "sqtmp"; break;
      case ScalarType::F32:  prefix = "ftmp"; break;
      case ScalarType::F64:  prefix = "gtmp"; break;
      case ScalarType::VOID: prefix = "void"; break;
      default: throw std::logic_error("can't encode type");
      }
    std::ostringstream buf;
    buf << prefix << '_' << tmps.size();

    Variable tmp( buf.str() );
    sink << tmp.GetType() << ' ' << tmp.GetName() << " = " << ExprPrinter(*this, e) << ";\n";
    tmps.emplace( std::piecewise_construct, std::forward_as_tuple(e), std::forward_as_tuple(tmp) );
  }

} /* end of namespace ccode */
} /* end of namespace symbolic */
} /* end of namespace util */
} /* end of namespace unisim */

