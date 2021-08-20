/***************************************************************************
                                sourcecode.cc
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

#include <sourcecode.hh>
#include <ostream>

SourceCode const* SourceCode::s_last_srccode = 0;

/** Create a C source code object
    @param _content a C string containing the C source code
    @param filename a filename object where was found the C source code
    @param lineno a line number where was found the C source code
    @return a C source code object
*/
SourceCode::SourceCode( ConstStr _content, FileLoc const& _fileloc )
  : content( _content ), fileloc( _fileloc )
{
  s_last_srccode = this;
}

/** Dump a C source code object into a stream
    @param source_code a C source code object to dump
    @param _sink a stream
*/
std::ostream&
operator<<( std::ostream& _sink, SourceCode const& _sc ) {
  _sink << '{' << _sc.content << '}';
  return _sink;
}

/** Create a parameter object
    @param c_type a C type
    @param c_symbol a C symbol
    @returns a parameter object
*/
CodePair::CodePair( SourceCode* _ctype, SourceCode* _csymbol )
  : ctype( _ctype ), csymbol( _csymbol )
{}

CodePair::~CodePair() {}

/** Dump a parameter object into a stream
    @param _sink a stream
*/
std::ostream&
operator << ( std::ostream& sink, CodePair const& _cp )
{
  sink << (*_cp.ctype) << ' ' << (*_cp.csymbol);
  
  return sink;
}
