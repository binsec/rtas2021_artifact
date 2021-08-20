/***************************************************************************
                                 comment.hh
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

#ifndef __COMMENT_HH__
#define __COMMENT_HH__

#include <fwd.hh>
#include <conststr.hh>
#include <errtools.hh>
#include <referencecounting.hh>
#include <vector>

/** A C/C++ comment object */
struct Comment : virtual ReferenceCounter
{
  ConstStr              content;
  FileLoc               fileloc;
  
  Comment( ConstStr _content, FileLoc const& _fileloc );
  Comment( Comment const& _comment );
  ~Comment();
};

#endif // __COMMENT_HH__
