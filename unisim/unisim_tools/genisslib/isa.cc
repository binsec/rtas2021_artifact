/***************************************************************************
                                    isa.cc
                             -------------------
    begin                : Thu May 25 2003
    copyright            : (C) 2003-2007 CEA and Universite Paris Sud
    authors              : Gilles Mouchard, Yves Lhuillier
    email                : gilles.mouchard@cea.fr, yves.lhuillier@cea.fr
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#include <isa.hh>
#include <operation.hh>
#include <subdecoder.hh>
#include <strtools.hh>
#include <action.hh>
#include <comment.hh>
#include <sourcecode.hh>
#include <bitfield.hh>
#include <variable.hh>
#include <specialization.hh>
#include <scanner.hh>
#include <product.hh>
#include <unistd.h>
#include <cassert>
#include <cerrno>
#include <iostream>
#include <riscgenerator.hh>
#include <ciscgenerator.hh>

/**
 * Construct for Opts (Default global options) */
Opts::Opts(char const* _outprefix)
  : outputprefix(_outprefix)
  , verbosity( 1 )
  , expandname( 0 )
  , depfilename( 0 )
  , minwordsize( 32 )
  , sourcelines( true )
  , privatemembers( true )
  , specialization( true )
  , comments( true )
{}

/** Constructor of Isa instance 
 */
Isa::Isa()
  : m_decoder( RiscDecoder )
  , m_is_subdecoder( false )
  , m_withcache( true )
  , m_withsource( false )
  , m_withencode( false )
  , m_little_endian( false )
  , m_asc_forder( false )
  , m_asc_worder( false )
  , m_minwordsize( 0 )
{}

/** Destructor for isa instance
 *  Where most of the allocated stuff will be released
 */
Isa::~Isa() {}

/** Search for an operation object
    @param operation_symbol a symbol object representing the operation
    @return the matching operation object, null if no operation object matches
*/
Operation*
Isa::operation( ConstStr _symbol )
{
  for (Vector<Operation>::iterator op = m_operations.begin(); op < m_operations.end(); ++ op)
    if ((**op).symbol == _symbol) return *op;
  
  return 0;
}

/** Remove an operation object from the global operation object list (m_operations)
    @param operation the operation object to remove
*/
void
Isa::remove( Operation* _op )
{
  for (Vector<Operation>::iterator iter = m_operations.begin(); iter < m_operations.end(); ++ iter) {
    if (*iter != _op) continue;
    m_operations.erase( iter );
    return;
  }
}

/** Add an operation object to the global operation object list
    (m_operations) and to active group accumulators (m_group_accs)
    @param operation the operation object to add
*/
void
Isa::add( Operation* _op )
{
  m_operations.append( _op );
  for (GroupAccumulators::iterator itr = m_group_accs.begin(), end = m_group_accs.end(); itr != end; ++itr)
    {
      // Check for duplicates ?
      for (Vector<Operation>::const_iterator found = itr->second->operations.begin(), fend = itr->second->operations.end();; ++found)
        {
          if (found == fend)
            { itr->second->operations.append( _op ); break; }
          if ((**found).symbol == _op->symbol)
            break;
        }
    }
}

/** Search the global group lists for the given symbol
    @param _symbol the group symbol looked for
    @return the found group
*/
Group*
Isa::group( ConstStr _symbol )
{
  for (Vector<Group>::iterator group = m_groups.begin(); group < m_groups.end(); ++ group)
    if ((*group)->symbol == _symbol) return *group;
  
  return 0;
}

/** Search for an action prototype object
    @param action_proto_symbol a symbol object representing the action prototype
    @return the matching action prototype object, null if no action prototype object matches
*/
ActionProto const*
Isa::actionproto( ConstStr _symbol ) const
{
  for (Vector<ActionProto>::const_iterator proto = m_actionprotos.begin(); proto < m_actionprotos.end(); ++ proto)
    if( (*proto)->m_symbol == _symbol ) return *proto;

  return 0;
}

/** Remove an action prototype object from the global action prototype object list (action_proto_list)
    @param actionproto the action prototype object to remove
*/
void
Isa::remove( ActionProto const* _ap )
{
  for (Vector<ActionProto>::iterator iter = m_actionprotos.begin(); iter < m_actionprotos.end(); ++ iter) {
    if (*iter != _ap) continue;
    m_actionprotos.erase( iter );
    return;
  }
}

/** Dump all objects from the global objects list (source_code_list, action_proto_list, operation_list) into a stream
    with the same format as in the original code
    @param _sink a stream
*/
void
Isa::expand( std::ostream& _sink ) const
{
  // dumping namespace
  if( not m_namespace.empty() ) {
    _sink << "namespace ";
    char const* sep = "";
    for (std::vector<ConstStr>::const_iterator piece = m_namespace.begin(); piece < m_namespace.end(); sep = "::", ++ piece)
      _sink << sep << (*piece);
    _sink << '\n';
  }
  
  // dumping template parameters
  if( not m_tparams.empty() ) {
    _sink << "template <";
    char const* sep = "";
    for (Vector<CodePair>::const_iterator iter = m_tparams.begin(); iter < m_tparams.end(); sep = ", ", ++ iter)
      _sink << sep << *(*iter);
    _sink << ">\n";
  }
  _sink << '\n';
  
  // dumping global parameters
  _sink << "set endianness " << (m_little_endian ? "little" : "big") << "\n";
  _sink << "set addressclass {" << m_addrtype << "}\n";
  
  for (Vector<SourceCode>::const_iterator srccode = m_decl_srccodes.begin(); srccode < m_decl_srccodes.end(); ++ srccode)
    _sink << "decl " << *(*srccode) << "\n\n";
  for (Vector<SourceCode>::const_iterator srccode = m_impl_srccodes.begin(); srccode < m_impl_srccodes.end(); ++ srccode)
    _sink << "impl " << *(*srccode) << "\n\n";
  _sink << '\n';

  if( not m_vars.empty() ) {
    _sink << "var ";
    char const* sep = "";
    for (Vector<Variable>::const_iterator var = m_vars.begin(); var < m_vars.end(); sep = ", ", ++ var)
      _sink << sep << *(*var);
    _sink << '\n';
  }
  _sink << '\n';
  
  for (Vector<ActionProto>::const_iterator ap = m_actionprotos.begin(); ap < m_actionprotos.end(); ++ap)
    _sink << *(*ap) << '\n';
  
  _sink << '\n';
  
  for (Vector<Operation>::const_iterator op = m_operations.begin(); op < m_operations.end(); ++ op)
    _sink << *(*op) << '\n';
}

Generator*
Isa::generator( Isa& _source, Opts const& _options ) const
{
  switch (m_decoder)
    {
    case RiscDecoder: return new RiscGenerator( _source, _options );
    case CiscDecoder: return new CiscGenerator( _source, _options );
      // case VliwDecoder: return new VliwGenerator( this ); break;
    default: break;
    }
  
  assert( false );
  
  return 0;
}

bool
Isa::sanity_checks() const
{
  if (not m_addrtype.str())
    {
      std::cerr << "error: no architecture address type found." << std::endl;
      return false;
    }
  
  if (m_operations.size() == 0)
    {
      std::cerr << "error: no operation ins ISA descriptions." << std::endl;
      return false;
    }
  
  // Checking operations
  for (Vector<Operation>::const_iterator op = m_operations.begin(); op < m_operations.end(); ++ op) {
    // Looking for bitfield conflicts
    for (Vector<BitField>::const_iterator bf = (**op).bitfields.begin(); bf < (**op).bitfields.end(); ++ bf) {
      ConstStr bf_symbol = (**bf).getsymbol();
      if (not bf_symbol.str()) continue;
      for (Vector<BitField>::const_iterator pbf = (**op).bitfields.begin(); pbf < bf; ++ pbf) {
        ConstStr pbf_symbol = (**pbf).getsymbol();
        if (pbf_symbol != bf_symbol) continue;
        (**op).fileloc.err( "error: duplicated bit field `%s' in operation `%s'", bf_symbol.str(), (**op).symbol.str() );
        return false;
      }
    }
    // Looking for variable conflicts
    if( not (**op).variables.empty() ) {
      for (Vector<Variable>::const_iterator checked = (**op).variables.begin(); checked < (**op).variables.end(); ++ checked) {
        Vector<Variable>::const_iterator found;
        for (found = m_vars.begin(); found < m_vars.end(); ++ found)
          if ((**found).symbol == (**checked).symbol) break;
        
        if( (found < m_vars.end()) ) {
          (**checked).ctype->fileloc.err( "error: in operation `%s', variable `%s' is already defined as global",
                                              (**op).symbol.str(), (**checked).symbol.str() );
          (**found).ctype->fileloc.err( "variable `%s' previously defined here", (**found).symbol.str() );
          return false;
        }
        
        for (found = (**op).variables.begin(); found < checked; ++ found)
          if( (**found).symbol == (**checked).symbol ) break;

        if (found < checked) {
          (**checked).ctype->fileloc.err( "error: in operation `%s', variable `%s' is defined several times", (**op).symbol.str(), (**checked).symbol.str() );
          return false;
        }
      }
    }
  }
  return true;
}

/** Output a rule file (<filename>) suitable for make describing the dependencies of the main source file.
    @param _sink a stream
*/
void
Isa::deps( std::ostream& _sink, char const* _prefix ) const
{
  if( m_tparams.empty() ) {
    _sink << _prefix << ".cc " << _prefix << ".hh:";
  } else {
    _sink << _prefix << ".tcc " << _prefix << ".hh:";
  }
  for (std::vector<ConstStr>::const_iterator inc = m_includes.begin(); inc < m_includes.end(); ++ inc)
    _sink << " \\\n " << *inc;
  _sink << "\n\n";
}

void
Isa::specialize()
{
  for (Vector<Specialization>::iterator spec = m_specializations.begin(); spec < m_specializations.end(); ++ spec) {
    m_operations.push_back( (**spec).newop() );
  }
}

void
Isa::setparam( ConstStr key, ConstStr value )
{
  static ConstStr    codetype( "codetype",        Scanner::symbols );
  static ConstStr      scalar( "scalar",          Scanner::symbols );
  static ConstStr      buffer( "buffer",          Scanner::symbols );
  static ConstStr  subdecoder( "subdecoder_p",    Scanner::symbols );
  static ConstStr   withcache( "withcache_p",     Scanner::symbols );
  static ConstStr  withsource( "withsource_p",    Scanner::symbols );
  static ConstStr withcomment( "withcomment_p",   Scanner::symbols );
  static ConstStr  withencode( "withencode_p",    Scanner::symbols );
  static ConstStr      istrue( "true",            Scanner::symbols );
  static ConstStr     isfalse( "false",           Scanner::symbols );
  static ConstStr  endianness( "endianness",      Scanner::symbols );
  static ConstStr       isbig( "big",             Scanner::symbols );
  static ConstStr    islittle( "little",          Scanner::symbols );
  static ConstStr      forder( "fields_order",    Scanner::symbols );
  static ConstStr      worder( "words_order",     Scanner::symbols );
  static ConstStr      isdesc( "descending",      Scanner::symbols );
  static ConstStr       isasc( "ascending",       Scanner::symbols );
  
  if      (key == codetype) {
    if      (value == scalar) m_decoder = RiscDecoder;
    else if (value == buffer) m_decoder = CiscDecoder;
    else throw UnknownIdent( value );
  }
  
  else if (key == subdecoder) {
    if      (value == istrue)  { m_is_subdecoder = true; m_withcache = false; }
    else if (value == isfalse) { m_is_subdecoder = false; }
  }
  
  else if (key == withcache) {
    if      (value == istrue)  { m_withcache = true; m_is_subdecoder = false; }
    else if (value == isfalse) { m_withcache = false; }
  }
  
  else if (key == withsource) {
    if      (value == istrue)  m_withsource = true;
    else if (value == isfalse) m_withsource = false;
    else throw UnknownIdent( value );
  }
  
  else if (key == withencode) {
    if      (value == istrue)  m_withencode = true;
    else if (value == isfalse) m_withencode = false;
    else throw UnknownIdent( value );
  }
  
  else if (key == endianness) {
    if      (value == isbig)    m_little_endian = false;
    else if (value == islittle) m_little_endian = true;
    else throw UnknownIdent( value );
  }
  
  else if (key == forder) {
    if      (value == isdesc) m_asc_forder = false;
    else if (value == isasc)  m_asc_forder = true;
    else throw UnknownIdent( value );
  }
  
  else if (key == worder) {
    if      (value == isdesc) m_asc_worder = false;
    else if (value == isasc)  m_asc_worder = true;
    else throw UnknownIdent( value );
  }
  
  else throw UnknownIdent( key );
}

void
Isa::setparam( ConstStr key, SourceCode const& value )
{
  static ConstStr  addressclass( "addressclass",  Scanner::symbols );
  static ConstStr codetypeclass( "codetypeclass", Scanner::symbols );
  
  if        (key == addressclass) {
    m_addrtype = value.content;
  } else if (key == codetypeclass) {
    //m_codetype = value->content;
  }
  
  else throw UnknownIdent( key );
}

void
Isa::setparam( ConstStr key, unsigned int value )
{
  static ConstStr  minwordsize( "minwordsize", Scanner::symbols );
  
  if        (key == minwordsize) {
    m_minwordsize = value;
  }
  
  else throw UnknownIdent( key );
}

SDClass const*
Isa::sdclass( std::vector<ConstStr>& _namespace ) const
{
  for (Vector<SDClass>::const_iterator sdc = m_sdclasses.begin(); sdc != m_sdclasses.end(); ++ sdc) {
    if( (**sdc).m_namespace == _namespace ) return *sdc;
  }
  return 0;
}

SDInstance const*
Isa::sdinstance( ConstStr _symbol ) const
{
  for (Vector<SDInstance>::const_iterator sdi = m_sdinstances.begin(); sdi != m_sdinstances.end(); ++ sdi) {
    if( (**sdi).m_symbol == _symbol ) return *sdi;
  }
  return 0;
}

void
Isa::group_command( ConstStr group_symbol, ConstStr _command, FileLoc const& fl )
{
  static ConstStr  group_begin( "begin",  Scanner::symbols );
  static ConstStr  group_end  ( "end",    Scanner::symbols );
  
  if (_command == group_begin)
    {
      m_group_accs[group_symbol] = new Group( group_symbol, fl );
    }
  else if (_command == group_end)
    {
      GroupAccumulators::iterator ga = m_group_accs.find( group_symbol );
      
      {
        /* group accumulator should exist */
        if (ga == m_group_accs.end()) {
          fl.err( "error: no corresponding `group %s %s' to `group %s %s' directive.",
                  group_symbol.str(), group_begin.str(), group_symbol.str(), group_end.str() );
          throw ParseError();
        }
      
        /* Operations and groups name should not conflict */
        if (Operation* prev_op = operation( group_symbol ))
          {
            fl.err( "error: group name conflicts with operation `%s'", group_symbol.str() );
            prev_op->fileloc.err( "operation `%s' previously defined here", group_symbol.str() );
            throw ParseError();
          }

        if (Group* prev_gr = group( group_symbol ))
          {
            fl.err( "conflicting group `%s' redefined", group_symbol.str() );
            prev_gr->fileloc.err( "group `%s' previously defined here", group_symbol.str() );
            throw ParseError();
          }
      }
      
      m_groups.push_back( ga->second );
      m_group_accs.erase( ga );
    }
}

bool
Isa::for_ops( ConstStr group_symbol, OOG& oog )
{
  if (Operation* op = operation( group_symbol ))
    {
      oog.with(*op);
    }
  else if (Group* gr = group( group_symbol ))
    {
      for (Vector<Operation>::iterator gop = gr->operations.begin(); gop < gr->operations.end(); ++ gop)
        oog.with(**gop);
    }
  else
    return false;
  return true;
}

