/***************************************************************************
                                 product.cc
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

#include <product.hh>
#include <scanner.hh>
#include <cstdarg>
#include <isa.hh>
#include <sourcecode.hh>
#include <strtools.hh>
#include <fstream>
#include <iostream>

/** Constructor: Create a Product object
    @param _filename the filename attached to the output code
*/
Product::Product( ConstStr filename, bool sourcelines )
  : m_filename( filename ), m_lineno( 1 ), m_sourcelines( sourcelines )
{
  m_indentations.push_back( 0 );
}

/** Constructor: Create a FProduct object
    @param _prefix the filename prefix attached to the output code
*/
FProduct::FProduct( char const* prefix, char const* suffix, bool sourcelines )
  : Product( Str::fmt( "%s%s", prefix, suffix ), sourcelines ),
    m_sink( new std::ofstream( m_filename.str() ) )
{}

/** Constructor: Create a SProduct object
    @param _prefix the filename prefix attached to the output code
*/
SProduct::SProduct( ConstStr _filename, bool _sourcelines )
  : Product( _filename, _sourcelines )
{}

/** Destructor: Close the Product object */
FProduct::~FProduct() { delete m_sink; }

/** Output source code into the output file
    Also generate #line in the output file to link the C compiler error to the original source code
    @param _source the SourceCode object to dump
 */
Product&
Product::usercode( SourceCode const& source )
{
  return usercode( source.fileloc, "%s", source.content.str() );
}

/** Output source code with surrounding braces into the output file
    Also generate #line in the output file to link the C compiler error to the original source code
    @param _source the SourceCode object to dump
 */
Product&
Product::usercode( SourceCode const& source, char const* fmt )
{
  return usercode( source.fileloc, fmt, source.content.str() );
}

/** Output source code into the output file
    Also generate #line in the output file to link the C compiler error to the original source code
    @param filename the name of the file where source code was found
    @param lineno the line number where source code was found
    @param format a C string with format specifier (like in printf), referenced arguments in the format string must follow
*/
Product&
Product::usercode( FileLoc const& _fileloc, char const* _fmt, ... ) {
  if (m_sourcelines) {
    require_newline();
    code( "#line %u \"%s\"\n", _fileloc.getline(), _fileloc.getname().str() );
  }
  va_list args;
  for( intptr_t capacity = 128, size; true; capacity = (size > -1) ? size + 1 : capacity * 2 ) {
    /* stack allocation */
    char storage[capacity];
    /* Try to print in the allocated space. */
    va_start( args, _fmt );
    size = vsnprintf( storage, capacity, _fmt, args );
    va_end( args );
    /* If it didn't work, retry */
    if (size < 0 or size >= capacity) continue;
    
    /* Now storage is ok... */
    write( storage );
    break;
  }
  
  if (m_sourcelines) {
    require_newline();
    code( "#line %u \"%s\"\n", m_lineno + 1, m_filename.str() );
  }
  return *this;
}

/** Output a string into the output file
    @param format a C string with format specifier (like in printf), referenced arguments in the format string must follow
*/

Product&
Product::code( char const* _fmt, ... ) {
  va_list args;
  for( intptr_t capacity = 128, size; true; capacity = (size > -1) ? size + 1 : capacity * 2 ) {
    /* stack allocation */
    char storage[capacity];
    /* Try to print in the allocated space. */
    va_start( args, _fmt );
    size = vsnprintf( storage, capacity, _fmt, args );
    va_end( args );
    /* If it didn't work, retry */
    if (size < 0 or size >= capacity) continue;
    
    /* Now storage is ok... */
    write( storage );
    break;
  }
  return *this;
}

Product&
Product::require_newline() {
  if( m_line.empty() ) return *this;
  write( "\n" );
  return *this;
}

Product&
Product::ns_leave( std::vector<ConstStr> const& _namespace ) {
  for( intptr_t idx = _namespace.size(); (--idx) >= 0; )
    code( "} " );
  code( "\n" );
  return *this;
}

Product&
Product::ns_enter( std::vector<ConstStr> const& _namespace ) {
  char const* sep = "";
  for( std::vector<ConstStr>::const_iterator ns = _namespace.begin(); ns < _namespace.end(); sep = " ", ++ ns ) {
    code( "%snamespace %s {", sep, (*ns).str() );
  }
  code( "\n" );
  flatten_indentation();
  return *this;
}

Product&
Product::template_signature( Vector<CodePair> const& _tparams ) {
  if( _tparams.empty() ) return *this;

  code( "template <" );
  
  bool intra = false;
  for( Vector<CodePair>::const_iterator tp = _tparams.begin(); tp < _tparams.end(); ++ tp ) {
    if (intra) code( "," ); else intra = true;
    usercode( (**tp).ctype->fileloc, "\t%s", (**tp).ctype->content.str() );
    usercode( (**tp).csymbol->fileloc, "\t%s", (**tp).csymbol->content.str() );
  }
  
  code( ">\n" );
  return *this;
}


Product&
Product::template_abbrev( Vector<CodePair> const& _tparams ) {
  if( _tparams.empty() ) return *this;

  code( "<" );

  bool intra = false;
  for( Vector<CodePair>::const_iterator tp = _tparams.begin(); tp < _tparams.end(); ++ tp ) {
    if (intra) code( "," ); else intra = true;
    usercode( (**tp).csymbol->fileloc, "\t%s", (**tp).csymbol->content.str() );
  }
  
  code( ">" );
  return *this;
}

Product&
Product::flatten_indentation()
{
  if (m_indentations.empty())
    return *this;
  
  int indentation = m_indentations.back();
  
  typedef std::vector<int>::reverse_iterator RI;
  RI prev = m_indentations.rbegin();
  
  for (RI ind = m_indentations.rbegin(), istop = m_indentations.rend(); ind != istop; ++ind)
    {
      if (*ind == indentation) continue;
      prev = ind;
      break;
    }
  
  indentation = *prev;
  
  for (RI ind = m_indentations.rbegin(); ind != prev; ++ind )
    *ind = indentation;
  
  return *this;
}


Product&
Product::write( char const* _ptr )
{
  if (not _ptr)
    return *this;
  
  for( char chr = *_ptr; chr; chr = *++_ptr ) {
    if (chr == '\n') {
      int current_indentation = m_indentations.back();
      int braces = 0;
      // Computing brace depth and right stripping blank characters.
      {
        char const* lbuf = m_line.c_str();
        uintptr_t rstrip = 0;
        for (uintptr_t ridx = 0; lbuf[ridx]; ++ridx ) {
          if (lbuf[ridx] <= ' ') continue;
          rstrip = ridx + 1;
          if (     lbuf[ridx] == '{') ++braces;
          else if (lbuf[ridx] == '}') --braces;
        }
        m_line = m_line.substr( 0, rstrip );
      }
      if (m_line.empty()) { xwrite( "\n" ); m_lineno += 1; continue; }
      
      if (braces > 0) {
        while( (--braces) > 0) m_indentations.push_back( current_indentation );
        m_indentations.push_back( current_indentation + 1 );
      } else if (braces < 0) {
        int nlength = m_indentations.size() + braces;
        if (nlength > 0) {
          m_indentations.resize( nlength );
        } else {
          std::cerr << "Indentation error (line " << m_lineno << ").\n";
          m_indentations.clear();
          m_indentations.push_back( 0 );
        }
      }
      
      char first_char = *m_line.begin(), last_char = *m_line.rbegin();
      if (first_char == '#') current_indentation = 0;
      else if (/*'{'*/ first_char == '}') current_indentation = m_indentations.back();
      else if (last_char == ':') --current_indentation;
      
      while( (--current_indentation) >= 0 ) xwrite( "\t" );
      xwrite( m_line.c_str() );
      xwrite( "\n" );
      m_line.clear();
      m_lineno += 1;
      continue;
    }
    if( m_line.empty() and chr <= ' ' ) continue;
    m_line += chr;
  }
  return *this;
}

/** \brief flush the line buffer
 *
 */
void Product::flush() { if( not m_line.empty() ) write( "\n" ); }

void FProduct::xwrite( char const* _ptr ) { (*m_sink) << _ptr; }

bool FProduct::good() const { return m_sink->good(); };

void SProduct::xwrite( char const* _ptr ) { m_content += _ptr; }
