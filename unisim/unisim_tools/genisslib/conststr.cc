/***************************************************************************
                                 conststr.cc
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

#include <conststr.hh>
#include <cstring>
#include <ostream>

ConstStr::Pool ConstStr::default_pool;

int
ConstStr::cmp( ConstStr const& rhs ) const
{
  if (c_string == rhs.c_string) return 0;
  if (not c_string) return -1;
  if (not rhs.c_string) return 1;
  return strcmp( c_string, rhs.c_string );
}

std::ostream&
operator << ( std::ostream& sink, ConstStr const& rhs )
{
  if (not rhs.c_string)
    sink << "<none>";
  else
    sink.write( rhs.c_string, strlen( rhs.c_string ) );
  
  return sink;
}
