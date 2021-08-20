/***************************************************************************
                               sourcecode.hh
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

#ifndef __SOURCECODE_HH__
#define __SOURCECODE_HH__

#include <fwd.hh>
#include <conststr.hh>
#include <referencecounting.hh>
#include <errtools.hh>
#include <iosfwd>

/** A C source code object */
struct SourceCode : virtual ReferenceCounter
{
  ConstStr              content;    /**< the string containing the C source code */
  FileLoc               fileloc;    /**< the file location where was found the C source code */
  
  SourceCode( ConstStr _content, FileLoc const& _fileloc );
  
  static SourceCode const*  s_last_srccode;
};

std::ostream& operator<<( std::ostream& _sink, SourceCode const& _sc );

/** A C/C++ Code object object */
struct CodePair : virtual ReferenceCounter
{
  Ptr<SourceCode>       ctype;        /**< The C type of the parameter */
  Ptr<SourceCode>       csymbol;      /**< The C symbol of the parameter */
  
  CodePair( SourceCode* _ctype, SourceCode* _csymbol );
  ~CodePair();
};

std::ostream& operator<<( std::ostream& _sink, CodePair const& _cp );

#endif // __SOURCECODE_HH__
