/***************************************************************************
                                 scanner.hh
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

#ifndef __SCANNER_HH__
#define __SCANNER_HH__

#include <fwd.hh>
#include <conststr.hh>
#include <referencecounting.hh>
#include <vect.hh>
#include <string>

struct Scanner
{
  static ConstStr::Pool           symbols;          ///< The symbol database
  
  static bool                     parse( char const* _filename, Opts& opts, Isa& _isa );
};

#endif // __SCANNER_HH__
