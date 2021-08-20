/***************************************************************************
                                 strtools.cc
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

#include <errtools.hh>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cassert>
#include <iostream>

void
FileLoc::err( char const* _fmt, ... ) const {
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
    this->loc( std::cerr ) << storage << std::endl;
    break;
  }
}

std::ostream&
FileLoc::loc( std::ostream& _sink ) const
{
  _sink << m_name << ':' << m_line << ':' << m_column << ": ";
  return _sink;
}
