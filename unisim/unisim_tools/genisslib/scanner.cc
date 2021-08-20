/***************************************************************************
                                   scanner.cc
                             -------------------
    begin                : Thu Nov 21 2019
    copyright            : (C) 2019-2020 CEA and Universite Paris Sud
    authors              : Gilles Mouchard, Yves Lhuillier
    email                : gilles.mouchard@cea.fr, yves.lhuillier@cea.fr
***************************************************************************/

#include <scanner.hh>
#include <isa.hh>
#include <comment.hh>
#include <sourcecode.hh>
#include <action.hh>
#include <operation.hh>
#include <bitfield.hh>
#include <variable.hh>
#include <specialization.hh>
#include <subdecoder.hh>
#include <strtools.hh>
#include <clex.hh>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <cstring>
#include <cassert>
#include <unistd.h>

ConstStr::Pool           Scanner::symbols;

namespace
{
  FileLoc GetFileLoc(CLex::Scanner& s)
  {
    return FileLoc(s.sourcename(), s.lidx, s.cidx );
  }
  SourceCode GetSourceCode(CLex::Scanner& src)
  {
    FileLoc && fl = GetFileLoc(src);
    std::string buf;
    int depth = 0;
    enum { InCode = 0, InString, InEsc } state = InCode;

    for (;;)
      {
        char ch = src.getchar();
        switch (state)
          {
          case InCode:
            if      (ch == '"')  state = InString;
            if      (ch == '{')  depth += 1;
            else if (ch == '}')  depth -= 1;
            break;
            
          case InString:
            if      (ch == '"')  state = InCode;
            else if (ch == '\\') state = InEsc;
            break;
            
          case InEsc:
            state = InString;
            break;
          }
        if (depth < 0) break;
        buf += ch;
      }

    return SourceCode( buf.c_str(), std::move(fl) );
  }
  
  unsigned GetInteger(CLex::Scanner& src)
  {
    unsigned base = 10;
    if (src.lch == '0')
      {
        char d = src.nextchar() ? src.lch : '\0';
        if (isdigit(d))                { base = 10; /*not octal for now*/ }
        else if (d == 'b' or d == 'x') { base = (d == 'b') ? 2 : 16; src.getchar(); }
        else { src.putback = true; return 0; }
      }

    unsigned res = 0;
    for (bool hasdigit = false;; hasdigit = true)
      {
        unsigned digit = 16;
        if      ('0' <= src.lch and src.lch <= '9')
          { digit = src.lch - '0'; }
        else if ('a' <= src.lch and src.lch <= 'f')
          { digit = src.lch - 'a' + 10; }
        else if ('A' <= src.lch and src.lch <= 'F')
          { digit = src.lch - 'A' + 10; }
        else if (hasdigit)
          break;
        
        if (digit >= base)
          throw src.unexpected();

        res = res*base + digit;
        if (not src.nextchar()) break;
      }

    src.putback = true;
    return res;
  }

  unsigned GetArraySize(CLex::Scanner& src)
  {
    if (src.next() != CLex::Scanner::ArrayOpening)
      throw src.unexpected();
    if (src.next() != CLex::Scanner::Number)
      throw src.unexpected();
    unsigned size = GetInteger(src);
    if (src.next() != CLex::Scanner::ArrayClosing)
      throw src.unexpected();
    return size;
  }
  
  Ptr<Operation>
  GetOp(CLex::Scanner& src, Isa& isa, Vector<Comment>& comments )
  {
    if (src.next() != CLex::Scanner::Name)
      throw src.unexpected();
    
    ConstStr symbol;
    FileLoc && symfl = GetFileLoc(src);
    {
      std::string buf;
      src.get(buf,&CLex::Scanner::get_name);
      symbol = ConstStr(buf.c_str(),Scanner::symbols);
    }

    if (src.next() != CLex::Scanner::GroupOpening)
      throw src.unexpected();
    
    Vector<BitField> bitfields;
    for (;;)
      {
        switch (src.next())
          {
          case CLex::Scanner::More:
            {
              // Separator
              bool rewind = src.next() != CLex::Scanner::Less;
              if (rewind)
                {
                  if (src.lnext != CLex::Scanner::Name or
                      not src.expect("rewind", &CLex::Scanner::get_name) or
                      src.next() != CLex::Scanner::Less)
                    throw src.unexpected();
                }
              bitfields.push_back( new SeparatorBitField( rewind ) );
            }
            break;
            
          case CLex::Scanner::Number:
            {
              // OpCode
              unsigned bits = GetInteger(src), size = GetArraySize(src);
              bitfields.push_back( new OpcodeBitField( size, bits ) );
            }
            break;

          case CLex::Scanner::QuestionMark:
            {
              // Unused
              unsigned size = GetArraySize(src);
              bitfields.push_back( new UnusedBitField( size ) );
            }
            break;

          case CLex::Scanner::Star:
            {
              // SubOp
              if (src.next() != CLex::Scanner::Name)
                throw src.unexpected();
              ConstStr symbol;
              {
                std::string buf;
                src.get(buf, &CLex::Scanner::get_name);
                symbol = ConstStr( buf.c_str(), Scanner::symbols );
              }
              if (src.next() != CLex::Scanner::ArrayOpening)
                throw src.unexpected();
              if (src.next() != CLex::Scanner::Name)
                throw src.unexpected();
              ConstStr sdinstance_symbol;
              {
                std::string buf;
                src.get(buf, &CLex::Scanner::get_name); 
                sdinstance_symbol = ConstStr( buf.c_str(), Scanner::symbols );
              }
              if (src.next() != CLex::Scanner::ArrayClosing)
                throw src.unexpected();
              if (SDInstance const* sdinstance = isa.sdinstance( sdinstance_symbol ))
                {
                  bitfields.push_back( new SubOpBitField( symbol, sdinstance ) );
                }
              else
                {
                  std::cerr << "error: subdecoder instance `" << sdinstance_symbol.str() << "' not declared.\n";
                  throw src.unexpected();
                }
            }
            break;

          default:
            {
              // Operand
              int shift = 0;
              bool sext = false;
              std::string buf;
              
              if (src.lnext == CLex::Scanner::Name)
                {
                  for (;;)
                    {
                      src.get(buf, &CLex::Scanner::get_name);
                      if (buf == "shl")
                        {
                          if (src.next() != CLex::Scanner::Less)
                            throw src.unexpected();
                          if (src.next() != CLex::Scanner::Number)
                            throw src.unexpected();
                          shift = -GetInteger(src);
                          if (src.next() != CLex::Scanner::More)
                            throw src.unexpected();
                        }
                      else if (buf == "sext")
                        {
                          sext = true;
                        }
                      else
                        break;
                      
                      if (src.next() != CLex::Scanner::Name)
                        {
                          if (src.lnext != CLex::Scanner::Less)
                            throw src.unexpected();
                          break;
                        }
                    }
                }

              int final_size = 0;

              if (src.lnext == CLex::Scanner::Less)
                {
                  if (src.next() != CLex::Scanner::Number)
                    throw src.unexpected();
                  final_size = GetInteger(src);
                  if (src.next() != CLex::Scanner::More)
                    throw src.unexpected();
                  if (src.next() != CLex::Scanner::Name)
                    throw src.unexpected();
                  src.get(buf, &CLex::Scanner::get_name);
                }

              if (src.lnext != CLex::Scanner::Name)
                throw src.unexpected();
                
              ConstStr symbol( buf.c_str(), Scanner::symbols );
              
              unsigned size = GetArraySize(src);

              bitfields.push_back( new OperandBitField( size, symbol, shift, final_size, sext ) );
            }
            break;
          }

        if (src.next() == CLex::Scanner::GroupClosing)
          break;

        if (src.lnext != CLex::Scanner::Colon)
          throw src.unexpected();
      }
    
    return new Operation(symbol, bitfields, comments, 0, symfl);
  }

  void
  GetVarList( CLex::Scanner& source, Vector<Variable>& var_list )
  {
    for (;;)
      {
        if (source.next() != CLex::Scanner::Name)
          throw source.unexpected();

        ConstStr varname;
        {
          std::string buf;
          source.get(buf, &CLex::Scanner::get_name);
          varname = ConstStr(buf.c_str(), Scanner::symbols);
        }
        if (source.next() != CLex::Scanner::Colon)
          throw source.unexpected();
        if (source.next() != CLex::Scanner::ObjectOpening)
          throw source.unexpected();
        SourceCode* c_type = new SourceCode(GetSourceCode(source));
        if (source.next() != CLex::Scanner::Assign)
          { var_list.push_back( new Variable( varname, c_type, 0 ) ); break; }
        if (source.next() != CLex::Scanner::ObjectOpening)
          throw source.unexpected();
        SourceCode* c_init = new SourceCode(GetSourceCode(source));
        var_list.push_back( new Variable( varname, c_type, c_init ) );
        if (source.next() != CLex::Scanner::Comma)
          break;
      }
  }
}

bool
Scanner::parse( char const* _filename, Opts& opts, Isa& isa )
{
  std::cerr << "Opening " << _filename << '\n';
  isa.m_includes.push_back( _filename );
  
  struct FileScanner : public CLex::Scanner
  {
    FileScanner(char const* f) : CLex::Scanner(), ifs(f), filename(f), esc('\n') {}
    virtual char const* sourcename() const override { return filename.c_str(); }
    virtual bool sourcegood() const override { return ifs.good(); }
    virtual bool sourceget(char& lch) override
    {
      if (esc != '\n') { lch = esc; esc = '\n'; return true; }
      for (;;)
        {
          bool good = ifs.get(lch).good();
          if (not good or lch != '\\') return good;
          if (not ifs.get(esc).good()) throw Unexpected();
          if (esc != '\n') break;
          lidx += 1;
          cidx = 0;
        }
      return true;
    }
    
    std::ifstream ifs;
    std::string filename;
    char esc;
  } source(_filename);
  
  if (not source.ifs.good())
    {
      std::cerr << "error: cannot open `" << _filename << "'\n";
      return false;
    }

  Vector<Comment> comments; // TODO: handle comments... or not
  
  source.next();

  while (source.lnext != CLex::Scanner::EoF)
    {
      switch (source.lnext) /* global statement */
        {
        case CLex::Scanner::Name:
          {
            FileLoc && cmdfl = GetFileLoc(source);
            std::string cmd;
            source.get(cmd, &CLex::Scanner::get_name);
        
            if ((cmd == "decl") or
                (cmd == "impl"))
              {
                auto& member = (cmd[0] == 'd') ? isa.m_decl_srccodes : isa.m_impl_srccodes;
                if (source.next() != CLex::Scanner::ObjectOpening)
                  throw source.unexpected();
                member.push_back( new SourceCode(GetSourceCode(source)) );
                source.next();
              }
            else if ((cmd == "action"))
              {
                SourceCode* returns = 0;
                if (source.next() == CLex::Scanner::ObjectOpening)
                  {
                    returns = new SourceCode(GetSourceCode(source));
                    source.next();
                  }
                
                ConstStr symbol;
                FileLoc && symfl = GetFileLoc(source);
                if (source.lnext != CLex::Scanner::Name)
                  throw source.unexpected();
                {
                  std::string buf;
                  source.get(buf, &CLex::Scanner::get_name);
                  symbol = ConstStr(buf.c_str(),Scanner::symbols);
                  ActionProto const*  prev_proto = isa.actionproto( symbol );
                  if (prev_proto)
                    {
                      symfl.err( "error: action prototype `%s' redefined", prev_proto->m_symbol.str() );
                      prev_proto->m_fileloc.err( "action prototype `%s' previously defined here", prev_proto->m_symbol.str() );
                      throw CLex::Scanner::Unexpected();
                    }
                }

                Vector<CodePair> param_list;
                if (source.next() != CLex::Scanner::GroupOpening)
                  throw source.unexpected();

                if (source.next() != CLex::Scanner::GroupClosing)
                  {
                    source.putback = true;
                    for (;;)
                      {
                        if (source.next() != CLex::Scanner::ObjectOpening)
                          throw source.unexpected();
                        SourceCode&& tp = GetSourceCode(source);
                        if (source.next() != CLex::Scanner::ObjectOpening)
                          throw source.unexpected();
                        SourceCode&& nm = GetSourceCode(source);
                        param_list.append( new CodePair( new SourceCode(std::move(tp)), new SourceCode(std::move(nm)) ) );
                        if (source.next() == CLex::Scanner::GroupClosing)
                          break;
                        if (source.lnext != CLex::Scanner::Comma)
                          throw source.unexpected();
                      }
                  }

                bool isconst = false;
                if (source.next() == CLex::Scanner::Name)
                  {
                    if (not source.expect( "const", &CLex::Scanner::get_name ))
                      throw source.unexpected();
                    isconst = true;
                    source.next();
                  }
                
                if (source.lnext != CLex::Scanner::ObjectOpening)
                  throw source.unexpected();

                SourceCode* default_sourcecode = new SourceCode(GetSourceCode(source));
                ActionProto* ap = new ActionProto( ActionProto::Common, symbol, returns, param_list, isconst, default_sourcecode, comments, symfl );
                comments.clear();
                isa.m_actionprotos.push_back( ap );
                source.next();
              }
            else if ((cmd == "include"))
              {
                if (source.next() != CLex::Scanner::StringQuotes)
                  throw source.unexpected();
                
                std::string buf;
                source.get(buf, &CLex::Scanner::get_string);
                
                ConstStr filename = opts.locate( buf.c_str() );

                if (not parse( filename.str(), opts, isa ))
                  {
                    std::cerr << source.loc() << ": parse error.\n";
                    return false;
                  }
                
                source.next();
              }
            else if ((cmd == "namespace"))
              {
                for (;;)
                  {
                    if (source.next() != CLex::Scanner::Name)
                      throw source.unexpected();
                    std::string cmd;
                    source.get(cmd, &CLex::Scanner::get_name);
                    isa.m_namespace.push_back( ConstStr( cmd.c_str(), Scanner::symbols ) );
                    if (source.next() != CLex::Scanner::Colon)
                      break;
                    if (source.getchar() != ':')
                      throw source.unexpected();
                  }
                
              }
            else if ((cmd == "set"))
              {
                if (source.next() != CLex::Scanner::Name)
                  throw source.unexpected();

                ConstStr key;
                {
                  CLex::BlocSink<16> ident;
                  if (not source.get_name(ident))
                    { std::cerr << "error: too long identifier\n";  throw source.unexpected(); }
                  ident.append('\0');
                  key = ConstStr(ident.buffer, Scanner::symbols);
                }

                switch (source.next())
                  {
                  default:
                    std::cerr << "error: not a set-value\n";
                    throw source.unexpected();

                  case CLex::Scanner::Name:
                    {
                      std::string value;
                      source.get(value, &CLex::Scanner::get_name);
                      isa.setparam( key, ConstStr(value.c_str(), Scanner::symbols) );
                    }
                    break;

                  case CLex::Scanner::ObjectOpening:
                    isa.setparam( key, GetSourceCode(source) );
                    break;
                  }
                source.next();
              }
            else if ((cmd == "subdecoder"))
              {
                std::vector<ConstStr> nmspc;
                for (;;)
                  {
                    if (source.next() != CLex::Scanner::Name)
                      throw source.unexpected();
                    std::string buf;
                    source.get(buf, &CLex::Scanner::get_name);
                    nmspc.push_back( ConstStr( buf.c_str(), Scanner::symbols ) );
                    if (source.next() != CLex::Scanner::Colon)
                      break;
                    if (source.getchar() != ':')
                      throw source.unexpected();
                  }
                SDClass const* sdclass = isa.sdclass( nmspc );

                if (source.lnext == CLex::Scanner::Name)
                  {
                    if (not sdclass)
                      {
                        cmdfl.err( "error: subdecoder has not been declared" );
                        throw CLex::Scanner::Unexpected();
                      }
                    
                    ConstStr symbol;
                    FileLoc && symfl = GetFileLoc(source);
                    {
                      std::string buf;
                      source.get(buf, &CLex::Scanner::get_name);
                      symbol = ConstStr(buf.c_str(), Scanner::symbols);
                    }

                    if (SDInstance const* sdinstance = isa.sdinstance( symbol ))
                      {
                        symfl.err( "error: subdecoder instance `%s' redefined", symbol.str() );
                        sdinstance->m_fileloc.err( "subdecoder instance `%s' previously defined here", symbol.str() );
                        throw CLex::Scanner::Unexpected();
                      }

                    SourceCode* tpscheme = 0;
                    
                    if (source.next() == CLex::Scanner::Less)
                      {
                        if (source.next() != CLex::Scanner::ObjectOpening)
                          throw source.unexpected();
                        tpscheme = new SourceCode(GetSourceCode(source));
                        if (source.next() != CLex::Scanner::More)
                          throw source.unexpected();
                        source.next();
                      }
                    
                    isa.m_sdinstances.append( new SDInstance( symbol, tpscheme, sdclass, symfl ) );
                  }
                else if (source.lnext == CLex::Scanner::ArrayOpening)
                  {
                    FileLoc&& sdloc = GetFileLoc(source);
                    if (sdclass)
                      {
                        sdloc.err( "error: subdecoder class redeclared." );
                        sdclass->m_fileloc.err( "subdecoder class previously declared here." );
                        throw CLex::Scanner::Unexpected();
                      }
  
                    std::vector<unsigned> insnsizes;
                    for (;;)
                      {
                        if (source.next() != CLex::Scanner::Number)
                          throw source.unexpected();
                        insnsizes.push_back(GetInteger(source));
                        if (source.next() != CLex::Scanner::Comma)
                          break;
                      }
                    if (source.lnext != CLex::Scanner::ArrayClosing)
                      throw source.unexpected();
                    
                    isa.m_sdclasses.append( new SDClass( nmspc, insnsizes.begin(), insnsizes.end(), sdloc ) );
                    source.next();
                  }
                else
                  {
                    throw source.unexpected();
                  }
              }
            else if ((cmd == "template"))
              {
                if (source.next() != CLex::Scanner::Less)
                  throw source.unexpected();
                for (;;)
                  {
                    if (source.next() != CLex::Scanner::ObjectOpening)
                      throw source.unexpected();
                    SourceCode&& tp = GetSourceCode(source);
                    if (source.next() != CLex::Scanner::ObjectOpening)
                      throw source.unexpected();
                    SourceCode&& nm = GetSourceCode(source);
                    isa.m_tparams.append( new CodePair( new SourceCode(std::move(tp)), new SourceCode(std::move(nm)) ) );
                    if (source.next() == CLex::Scanner::More)
                      break;
                    if (source.lnext != CLex::Scanner::Comma)
                      throw source.unexpected();
                  }
                source.next();
              }
            else if ((cmd == "op"))
              {
                isa.add(GetOp(source, isa, comments));
                comments.clear();
                source.next();
              }
            else if ((cmd == "specialize"))
              {
                if (source.next() != CLex::Scanner::Name)
                  throw source.unexpected();

                Specialization* spec = 0;
                FileLoc && symfl = GetFileLoc(source);
                {
                  std::string buf;
                  source.get(buf, &CLex::Scanner::get_name);
                  if (Operation* operation = isa.operation( ConstStr(buf.c_str(), Scanner::symbols) ))
                    {
                      Vector<Constraint> none;
                      spec = new Specialization(operation, none);
                    }
                  else
                    {
                      symfl.err( "error: operation `%s' not defined", buf.c_str() );
                      throw CLex::Scanner::Unexpected();
                    }
                }
                
                if (source.next() != CLex::Scanner::GroupOpening)
                  throw source.unexpected();
                
                for (;;)
                  {
                    if (source.next() != CLex::Scanner::Name)
                      throw source.unexpected();
                    std::string buf;
                    source.get(buf, &CLex::Scanner::get_name);
                    
                    if (source.next() != CLex::Scanner::Assign)
                      throw source.unexpected();

                    if (source.next() != CLex::Scanner::Number)
                      throw source.unexpected();
                    unsigned value = GetInteger(source);
                    
                    spec->m_constraints.push_back( new Constraint( ConstStr(buf.c_str(), Scanner::symbols), value ) );
                    
                    if (source.next() != CLex::Scanner::Comma)
                      break;
                  }
                    
                if (source.lnext != CLex::Scanner::GroupClosing)
                  throw source.unexpected();

                isa.m_specializations.push_back( spec );
                
                source.next();
              }
            else if ((cmd == "var"))
              {
                Vector<Variable> var_list;
                GetVarList(source, var_list);
                isa.m_vars.append( var_list );
              }
            else if ((cmd == "group"))
              {
                if (source.next() != CLex::Scanner::Name)
                  throw source.unexpected();
                ConstStr symbol;
                FileLoc && symfl = GetFileLoc(source);
                {
                  std::string buf;
                  source.get(buf, &CLex::Scanner::get_name);
                  symbol = ConstStr(buf.c_str(), Scanner::symbols);
                }

                source.next();
                if (source.lnext == CLex::Scanner::Name)
                  {
                    CLex::BlocSink<16> grpcmd;
                    if (not source.get_name(grpcmd))
                      throw source.unexpected();
                    grpcmd.append('\0');
                    isa.group_command( symbol, ConstStr(grpcmd.buffer,Scanner::symbols), symfl );
                  }
                else if (source.lnext == CLex::Scanner::GroupOpening)
                  {
                    do {
                      FileLoc def; char const* err = 0;
                      if  (Operation* prev_op = isa.operation( symbol ))
                        { err = "operation"; def = prev_op->fileloc; }
                      else if (Group* prev_gr = isa.group( symbol ))
                        { err =     "group"; def = prev_gr->fileloc; }
                      else
                        break;
                      
                      symfl.err( "error: %s name conflicts with %s `%s'", err, err, symbol.str() );
                      def.err( "%s `%s' previously defined here", err, symbol.str() );
                      throw CLex::Scanner::Unexpected();
                      
                    } while (0);
                    
                    struct : Isa::OOG
                    {
                      void with( Operation& operation )
                      {
                        if (opsyms.insert(operation.symbol).second)
                          group->operations.append( &operation );
                      }
                      std::set<ConstStr> opsyms;
                      Group* group;
                    } oog;
                    oog.group = new Group( symbol, symfl );
                    
                    for (;;)
                      {
                        if (source.next() != CLex::Scanner::Name)
                          throw source.unexpected();
                        FileLoc&& errfl = GetFileLoc(source);
                        std::string buf;
                        source.get(buf, &CLex::Scanner::get_name);
                        if (not isa.for_ops(ConstStr(buf,Scanner::symbols), oog))
                          {
                            errfl.err( "error: undefined operation or group `%s'", buf.c_str() );
                            throw CLex::Scanner::Unexpected();
                          }
                        if (source.next() != CLex::Scanner::Comma)
                          break;
                      }
                    
                    if (source.lnext != CLex::Scanner::GroupClosing)
                      throw source.unexpected();
                    
                    isa.m_groups.push_back( oog.group );
                  }
                else
                  throw source.unexpected();

                source.next();
              }
            else
              {
                ConstStr symbol(cmd.c_str(), Scanner::symbols);
                if (source.next() != CLex::Scanner::Dot)
                  throw source.unexpected();
                ConstStr attribute;
                if (source.next() != CLex::Scanner::Name)
                  throw source.unexpected();
                FileLoc && attrfl = GetFileLoc(source);
                {
                  std::string buf;
                  source.get(buf, &CLex::Scanner::get_name);
                  attribute = ConstStr(buf.c_str(), Scanner::symbols);
                }

                static ConstStr   var( "var",        Scanner::symbols );
                static ConstStr   spe( "specialize", Scanner::symbols );
                
                if (attribute == var)
                  {
                    struct : public Isa::OOG
                    {
                      void with( Operation& operation ) override { operation.variables.append(var_list); }
                      Vector<Variable> var_list;
                    } oog;
                    
                    GetVarList(source, oog.var_list);

                    if (not isa.for_ops( symbol, oog ))
                      {
                        cmdfl.err( "error: undefined operation or group `%s'", symbol.str() );
                        throw CLex::Scanner::Unexpected();
                      }
                  }
                else if (attribute == spe)
                  {
                    if (source.next() != CLex::Scanner::GroupOpening)
                      throw source.unexpected();
                    
                    ConstStr symbol(cmd.c_str(), Scanner::symbols);
                    isa.m_user_orderings.push_back( Isa::Ordering() );
                    Isa::Ordering& order = isa.m_user_orderings.back();
                    order.fileloc = cmdfl;
                    order.top_op = symbol;

                    for (;;)
                      {
                        if (source.next() != CLex::Scanner::Name)
                          throw source.unexpected();
                        std::string buf;
                        source.get(buf, &CLex::Scanner::get_name);
                        order.under_ops.push_back( ConstStr(buf.c_str(), Scanner::symbols) );
                        
                        if (source.next() != CLex::Scanner::Comma)
                          break;
                      }
                    
                    if (source.lnext != CLex::Scanner::GroupClosing)
                      throw source.unexpected();
                    
                    source.next();
                  }
                else
                  {
                    if (source.next() != CLex::Scanner::Assign)
                      throw source.unexpected();
                    
                    struct : Isa::OOG
                    {
                      SourceCode* actioncode;
                      ActionProto const* actionproto;
                      FileLoc actloc;
                      Vector<Comment>* comments; 
                      void with( Operation& operation ) override
                      {
                        if (Action const* prev_action = operation.action( actionproto ))
                          {
                            actloc.err( "error: action `%s.%s' redefined", operation.symbol.str(), actionproto->m_symbol.str() );
                            prev_action->m_fileloc.err( "action `%s.%s' previously defined here", operation.symbol.str(), actionproto->m_symbol.str() );
                            exit( -1 );
                          }
                        
                        operation.add( new Action( actionproto, actioncode, *comments, actloc ) );
                      }
                    } oog;
                    
                    if (source.next() != CLex::Scanner::ObjectOpening)
                      throw source.unexpected();
                    oog.actioncode = new SourceCode(GetSourceCode(source));
                    oog.actloc = attrfl;
                    oog.comments = &comments;
                    /* Action must belong to an action prototype */
                    if (not (oog.actionproto = isa.actionproto( attribute )))
                      {
                        cmdfl.err( "error: undefined action prototype `%s'", attribute.str() );
                        throw CLex::Scanner::Unexpected();
                      }

                    if (not isa.for_ops( symbol, oog ))
                      {
                        cmdfl.err( "error: undefined operation or group `%s'", symbol.str() );
                        throw CLex::Scanner::Unexpected();
                      }
                    comments.clear();
                    source.next();
                  }
              }
          
          }
          break;

        case CLex::Scanner::ObjectOpening:
          {
            Ptr<SourceCode> sc = new SourceCode(GetSourceCode(source));
            if (source.next() == CLex::Scanner::Colon)
              {
                if (source.next() != CLex::Scanner::Name or not source.expect("op", &CLex::Scanner::get_name))
                  throw source.unexpected();
                auto op = GetOp(source, isa, comments);
                comments.clear();
                op->condition = sc;
                isa.add(op);
              }
            else
              isa.m_decl_srccodes.push_back(sc);
            source.next();
          }
          break;

        case CLex::Scanner::SemiColon:
          source.next();
          break;
      
        default:
          throw source.unexpected();
        }
    }
  
  return true;
}
