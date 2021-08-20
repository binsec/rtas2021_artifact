/***************************************************************************
                                subdecoder.cc
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

#include <subdecoder.hh>
#include <scanner.hh>
#include <comment.hh>
#include <variable.hh>
#include <sourcecode.hh>
#include <action.hh>
#include <bitfield.hh>
#include <action.hh>
#include <isa.hh>
#include <strtools.hh>
#include <cassert>
#include <ostream>

/** Delete a subdecoder object
*/
SDClass::~SDClass() {}

ConstStr
SDClass::qd_namespace() const {
  std::string buffer;
  std::string sep;

  for( std::vector<ConstStr>::const_iterator node = m_namespace.begin(); node != m_namespace.end(); ++ node, sep = "::" )
    buffer += sep + node->str();
  
  return ConstStr( buffer.c_str() );
}

SDInstance::SDInstance( ConstStr _symbol, SourceCode const* _template_scheme, SDClass const* _sdclass, FileLoc const& _fileloc )
  : m_symbol( _symbol ), m_template_scheme( _template_scheme ), m_sdclass( _sdclass ), m_fileloc( _fileloc )
{}

SDInstance::~SDInstance() {}
