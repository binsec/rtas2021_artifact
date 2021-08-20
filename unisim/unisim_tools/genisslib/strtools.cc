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

#include <strtools.hh>
#include <string>
#include <cstdarg>
#include <cstring>
#include <cassert>
#include <cctype>
//#include <iostream>
#include <cstdio>

namespace Str
{
  ConstStr
  fmt( char const* _fmt, ... )
  {
    va_list ap;

    for( intptr_t capacity = 128, size; true; capacity = (size > -1) ? size + 1 : capacity * 2 ) {
      /* allocation */
      char storage[capacity];
      /* Try to print in the allocated space. */
      va_start( ap, _fmt );
      size = vsnprintf( storage, capacity, _fmt, ap );
      va_end( ap );
      /* If it worked, return */
      if (size >= 0 and size < capacity)
        return ConstStr( storage );
    }
    assert( false );

    return ConstStr("");
  }

  ConstStr
  upper( char const* _str )
  {
    std::string buffer( _str );
    for (std::string::iterator itr = buffer.begin(); itr != buffer.end(); ++itr)
      {
        char ch = *itr;
        if (ch >= 'a' and ch <= 'z')
          *itr = (ch - 'a' + 'A');
      }
    
    return ConstStr( buffer.c_str() );
  }
  
  ConstStr
  capitalize( char const* _str )
  {
    std::string buffer( _str );
    
    char& ch = buffer[0];
    if (ch >= 'a' and ch <= 'z')
      ch = (ch - 'a' + 'A');
    
    return ConstStr( buffer.c_str() );
  }
  
  ConstStr
  tokenize( char const* _str )
  {
    std::string buffer( _str );
    
    for (std::string::iterator itr = buffer.begin(); itr != buffer.end(); ++itr)
      { 
        char ch = *itr;
        *itr = isalnum( ch ) ? ch : '_';
      }

    return ConstStr( buffer.c_str() );
  }

  ConstStr
  dqcstring( char const* _str )
  {
    std::string buffer( "\"" );
    
    static char const* hextab = "0123456789abcdef";
    
    for( char const* ptr = _str; *ptr; ++ ptr ) {
      char ch = *ptr;
      switch( ch ) {
      case '\t': buffer += "\\t"; break;
      case '\n': buffer += "\\n"; break;
      case '\r': buffer += "\\r"; break;
      case  '"': buffer += "\\\""; break;
      case '\\': buffer += "\\\\"; break;
      default:
        {
          if (ch >= 127 or ch < 32) {
            buffer += "\\x";
            buffer += hextab[unsigned( ch ) / 256];
            buffer += hextab[unsigned( ch ) % 256];
          } else {
            buffer += ch;
          }
        }
        break;
      }
    }
    buffer += "\"";
    
    return ConstStr( buffer.c_str() );
  }

} // end of namespace Str
