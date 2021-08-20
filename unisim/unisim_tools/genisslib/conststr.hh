/***************************************************************************
                                 conststr.hh
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

#ifndef __STRIMMUTABLE_HH__
#define __STRIMMUTABLE_HH__

#include <set>
#include <string>
#include <iosfwd>
#include <inttypes.h>

/** ConstStr Class for handling immutable and intensively shared
 *    strings .
 **/

struct ConstStr
{
  typedef std::set<std::string> Pool;
  
  // Construction / Destruction / Assignation
  ConstStr() : c_string( 0 ) {}
  
  ConstStr( char const* _str ) : c_string( _str ? default_pool.insert( _str ).first->c_str() : 0 ) {}
  
  ConstStr( std::string const& _str ) : c_string( default_pool.insert( _str ).first->c_str() ) {}
  
  ConstStr( std::string const& _str, Pool& _set ) : c_string( _set.insert( _str ).first->c_str() ) {}
  
  ConstStr( ConstStr const& _ref ) : c_string( _ref.c_string ) {}
  
  char const* str() const { return c_string; }
  
  int cmp( ConstStr const& rhs ) const;

  bool operator == ( ConstStr const& rhs ) const { return cmp( rhs ) == 0; }
  bool operator != ( ConstStr const& rhs ) const { return cmp( rhs ) != 0; }
  bool operator <  ( ConstStr const& rhs ) const { return cmp( rhs ) <  0; }
  bool operator <= ( ConstStr const& rhs ) const { return cmp( rhs ) <= 0; }
  bool operator >  ( ConstStr const& rhs ) const { return cmp( rhs ) >  0; }
  bool operator >= ( ConstStr const& rhs ) const { return cmp( rhs ) >= 0; }
  
  friend std::ostream& operator << ( std::ostream& sink, ConstStr const& rhs );
  
  static Pool default_pool; //< Default shared string pool
  
private:
  char const* c_string;
};

#endif // __STRIMMUTABLE_HH__
