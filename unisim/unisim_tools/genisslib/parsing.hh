/***************************************************************************
                                 lex.hh
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

#ifndef __LEX_HH__
#define __LEX_HH__

#include <main.hh>
#include <comment.hh>
#include <isa.hh>
#include <errtools.hh>
#include <vector>

struct Parsing
{
  static Vector<Comment>          comments;         ///< Comments accumulator;
  static FileLoc                  fileloc;          ///< file location in scanned file
  static Isa*                     s_isa;
  
  static bool                     include( char const* _filename );
  static Isa&                     isa() { return *s_isa; }
  static ConstStr                 tokenname( int _token );
};


// int yyerror( char const* _s );
// int yypanicf( char const *_filename, int _lineno, char const* _format, ... );
// int yyparse();

#endif // __LEX_HH__
